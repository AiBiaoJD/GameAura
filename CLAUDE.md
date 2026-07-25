# Aura — UE5 Gameplay Ability System 学习项目

基于 UE5 + GAS 的 ARPG 战斗系统学习项目。角色包含远程（Goblin/Shaman）与近战（Ghoul/Demon/Warrior）敌人类型，通过行为树 + EQS 驱动 AI，使用 MotionWarping 配合攻击动画。

## 项目结构

```
Source/Aura/
├── Public/ 和 Private/
│   ├── My_AI/              # 自定义 AI 节点（BTTask、AIController）
│   ├── My_Character/       # 角色基类与敌人角色
│   ├── My_AbilitySystem/   # GAS 核心：ASC、AttributeSet、Ability、GameplayCue、ExeCalc
│   ├── My_AbilityActor/    # 投射物等能力 Actor
│   ├── My_Interraction/    # CombatInterface 等交互接口
│   ├── My_UI/              # HUD、WidgetController、Overlay
│   ├── My_Controler/       # PlayerController
│   ├── My_EffectActor/     # 效果 Actor（如药水拾取）
│   └── My_Input/           # 输入配置
└── Content/
    └── MyBlueprints/       # 蓝图资产（AI/行为树、角色、能力、UI）
```

**约定**：自定义 C++ 类统一用 `My_` 前缀；蓝图资产放在 `MyBlueprints/` 下。

## 中文注释与 GBK 编码（⚠️ 关键）

C++ 源文件是 **GBK 编码**（Windows 中文 codepage 936）。UE5 在中文 Windows 上默认以 GBK 保存 C++ 文件。

**禁止操作**：
- 不要用 `Edit` 工具直接修改含中文注释的 `.h`/`.cpp`——会转 UTF-8 导致全部中文乱码（**即使只改 ASCII 部分的文本，整个文件都会被重新编码为 UTF-8**）
- 不要用 `Write` 工具覆盖含中文的 `.h`/`.cpp`——同样会转 UTF-8
- 不要用 `sed` 插入中文文本

**安全做法**：用 Python + 显式 GBK 编码 + `newline=''`：

⚠️ **写入 GBK 文件必须加 `newline=''`**——不加会导致 Python 在 Windows 上自动把 `\r\n` 转换成 `\r\r\n`，编译时报 C4335 "检测到 Mac 文件格式"。

### 方案一：简单替换（仅 ASCII 字符改动）

```bash
/c/Users/79467/anaconda3/python -c "
with open('file.cpp', 'r', encoding='gbk') as f:
    content = f.read()
content = content.replace('old_ascii_pattern', 'replacement')
with open('file.cpp', 'w', encoding='gbk', newline='') as f:
    f.write(content)
"
```

### 方案二：完全重建文件（需要写中文注释时）

当 Python 命令行传中文参数可能被终端编码破坏时，先用 `Write` 工具写一个 `.py` 脚本文件（UTF-8 编码的脚本直接写中文没问题），再用 Bash 执行它：

```bash
# 1. 用 Write 工具创建 _fix_temp.py，脚本里硬编码中文文本和完整文件内容
#    - 脚本中 open() 写文件时使用 encoding='gbk', newline=''
#    - Python 字符串内显式使用 \r\n 换行
# 2. 执行脚本
/c/Users/79467/anaconda3/python _fix_temp.py
# 3. 删除临时脚本
rm _fix_temp.py
```

### 已损坏文件的恢复

如果 Edit/Write 已经破坏了 GBK 文件：
1. 用 Python `open(f, 'rb').read().decode('gbk')` 会报错（非法多字节序列）
2. 必须用方案二完全重建文件——用 Python 脚本按 `encoding='gbk'` 重新写一遍
3. 验证：`open(f, 'r', encoding='gbk')` 无异常 + 代码逻辑正确

Read 工具显示的中文是乱码属于正常现象——以 Python round-trip 验证为准。

## 关键踩坑

### FBlackboardKeySelector 必须 ResolveSelectedKey
自定义 BTTask 中如果用 `FBlackboardKeySelector` 读黑板值，必须在 `InitializeFromAsset` 中调用 `ResolveSelectedKey`，否则运行时 KeyID 无效，只能拿到默认值：
```cpp
void UMy_BTTask_MoveTo::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    AcceptableRadiusKey.ResolveSelectedKey(*Asset.BlackboardAsset);  // 名字 → 运行时 ID
}
```

### UBTDecorator_BlackboardBase 不支持 NotifyObserver
自定义装饰器不要继承 `UBTDecorator_BlackboardBase`——它没有 `NotifyObserver` 机制，值变化时不会重新评估。系统内置的 `UBTDecorator_Blackboard` 才有。如果只是比较黑板值 vs 常量，直接用系统装饰器。

### CombatDistance 系统
每个敌人有独立的 `CombatDistance`（在 `AEnemy_Characte` 上，蓝图可调），在 `PossessedBy` 时写入黑板 `CombatDistance`。自定义 `My_BTTask_MoveTo` 从黑板读取该值作为 MoveTo 的 `AcceptableRadius`，实现每敌人不同战斗距离。

### UE5.5 Duplicate 蓝图 Bug — 禁止复制蓝图来创建新敌人（⚠️ 2026.5.6 确认）

UE5.5 存在 Duplicate Blueprint 的已知 Bug：复制蓝图时 C++ 构造函数中 `CreateDefaultSubobject` 创建的组件（如 `AttributeSet`）可能不会正确继承，导致运行时 `CastChecked<UMy_AuraAttributeSet>(AttributeSet)` 崩溃（nullptr）。

- **崩溃特征**：`AEnemy_Characte::BeginPlay()` line 121 → `Cast of nullptr to My_AuraAttributeSet failed`
- **必做**：创建新敌人类型（如 DemonRanger）时，从 C++ 父类 `AEnemy_Characte` **右键 → Create Blueprint** 重新建立，**禁止复制现有敌人蓝图**
- 复制行为树、Montage 等其他资产暂未发现此问题

### 敌人能力初始化流程 — 两套路径（⚠️ 不要混用）

敌人和玩家走**不同的** Ability 初始化路径：

**敌人**（`AEnemy_Characte`）：
`BeginPlay()` → `My_InitAbilityActorInfo()` → `InitializeDefaultAttribute()`（重写版）
→ `UMy_AuraAbilitySystemLibrary::GiveStartupAbilities(this, ASC, CharacterClass)`
→ 从 `My_DA_CharacterClassInfo` DataAsset 读取 `CommonAbility` + `CharacterClassInformation[CharacterClass].StartupAbilities`
→ `ASC->GiveAbility()`

**玩家**（`AAura_Character`）：
`PossessedBy()` → `My_InitAbilityActorInfo()` → `InitializeDefaultAttribute()`（基类版，仅 GE）
→ `AddCharacterAbilities()` → 使用角色 private 成员 `StartupAbility` 数组

`AMyCharacter_Base::AddCharacterAbilities()` **敌人不调用它**。敌人完全依赖 `My_DA_CharacterClassInfo` DataAsset（通过 GameMode 引用）赋予能力。不要在敌人 C++ 里找 `AddCharacterAbilities()` 的调用——它不存在。

### GameplayCue 与 MontageEvent 时序
- 近战攻击：Montage 中通过 AnimNotify 触发 MontageEvent → Ability 中 `WaitGameplayEvent` 等待 → 触发 GameplayCue（声音/血效）
- `NetExecutionPolicy`：LocalPredicted 在客户端立即执行但可能不准；ServerOnly 等服务器确认后播放，延迟但准确
- MotionWarping 的 `WarpTargetName` 是 FName，通过 `AddOrUpdateWarpTargetFromLocation` 在攻击前更新目标位置

### AnimationEditorPreviewActor_0 错误可忽略

动画蓝图编辑器预览含 AnimNotify（发送 GameplayEvent）的动画时，预览 Actor 没有 ASC，会报：
`UAbilitySystemBlueprintLibrary::SendGameplayEventToActor: Invalid ASC from AnimationEditorPreviewActor_0`
这是编辑器预览的正常日志，**不影响运行时**。收到这个错误只需关闭动画编辑器。

## 角色类型

| Class | 类型 | 攻击方式 |
|-------|------|----------|
| Warrior | 近战 | 武器 Socket 攻击，MotionWarping 贴近 |
| Ranger | 远程 | 投射物（Projectile），通过 BehaviorTree RangeAttack 黑板键分流 |

## 行为树结构

```
Selector(根)
├── Child 0: Remote Attack（远程敌人）
├── Child 1: Melee Attack（近战敌人，使用 My_BTTask_MoveTo + CombatDistance）
├── Child 2: Chase（追击，兜底）
└── Child 3: Search（EQS 搜索）
```

## 学习笔记与 Git 规则

- **每次学习/讨论结束后**：将新知识点写入仓库根目录的 `GAS-LearningNotes.md`，按章节编号追加或更新已有章节
- **完成教程阶段或功能后**：提交所有改动（源码 + 蓝图 + 笔记），推送到 GitHub
- `GAS-LearningNotes.md` 使用 UTF-8 编码（和 C++ 文件的 GBK 不同）
