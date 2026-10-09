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

## 中文编码规则（详见笔记「五十八、C++ 中文编码规矩」）

一句话版：

- C++ 源文件统一 **UTF-8 with BOM + CRLF**；`GAS-LearningNotes.md` 是 UTF-8 **无** BOM。
- 改含中文的 `.cpp`/`.h` **前后各校验一次 BOM**（`edit` 类工具会吞掉 BOM）。
- 中文字符串字面量必须写在一对 `TEXT("...")` 里、**禁止跨行拼接**。
- 改完必须**编译验证**（脚本校验通过 ≠ 能编译）。

完整规则、诊断命令、踩坑记录 → 见 `GAS-LearningNotes.md` 第五十八章。

## ⚠️ 文件操作规则（硬规矩，2026-09-16 用户明确要求）

**绝对不要在项目文件夹里新建任何文件。需要新建文件时，交给用户自己操作。**

### 禁止新建的东西

- `_*.py`（AI 诊断脚本）
- `_*.txt` / `_*.log`（诊断报告）
- `compile_commands.json` 等生成物
- `backups/` 备份目录
- `__pycache__/`

### 正确做法

| 场景 | 做法 |
|------|------|
| 诊断/排查 | **优先用只读命令**（`git status`、`Get-Content`、read 工具），不落盘 |
| 必须跑脚本 | 写到**项目文件夹之外**；用完**立即删除** |
| 项目外也写不了 | **先问用户**，同意后才写；写完立即删 |
| 改代码 | **只改已有文件，可直接改，不必询问** |
| 需要新建文件 | **停下来问用户**，由用户自己建 |

### 判断标准

做完一件事后，`git status` 里**只应出现用户自己的改动**。
若出现了 AI 产生的文件，说明做错了。

### 为什么定这条规矩

曾有一次会话里 AI 在项目根目录留下了 20+ 个 `_*.py` / `_*.txt` 临时文件和一个
5.9 MB 的 `compile_commands.json`，把用户的项目文件夹弄乱了。

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
- `GAS-LearningNotes.md` 使用 UTF-8 编码（无 BOM）；C++ 源文件用 UTF-8 with BOM（见笔记第五十八章）

## 会话与成本管理（DSH）

### 单价与实测

| 项目 | 单价（USD/1M，CNY 按 7.1 折算） | 占 2026-10-10 那天账单 |
|------|------|------|
| 缓存未命中输入 | $0.15 ≈ **¥1.07/M** | **60%** |
| 输出（含 thinking） | $0.6 ≈ ¥4.26/M | 18% |
| 缓存命中 | $0.003 ≈ ¥0.021/M | 21% |

- **命中比未命中便宜 50 倍；输出比未命中还贵 4 倍。**
- 29 天累计 $6.19 ≈ ¥44，2103 次调用。
- **贵的只有「新进上下文」**：读一个大文件、跑一条刷屏命令、贴一屏日志。
- 判断某天为什么贵，看**「每次调用平均未命中多少 token」**：
  10/8（旧启动方式）= 579；10/10 = 25,333 —— 44 倍差距，全在这里。

### 三个决策

**1. 什么时候开新会话** —— 判据：**下一件事需不需要沿用当前上下文**

| 情况 | 做法 |
|------|------|
| 同一系统延续 / 同一 bug 没调完 | **留在这个会话**（读过的文件已是缓存，重开=未命中重来） |
| 转到独立系统（AttributeSet / 网络复制 / GameplayCue 等） | 开新会话，开场先读笔记对应章节 |
| 上下文 80%+ 且这个话题快结束 | 先写笔记，再开新会话 |

自己都说不清「上一章改了哪些文件、为什么那么改」→ 就该开新会话了。

**2. 什么时候 `/compact`** —— 只有两种情况：① 上下文快满但**必须**接着当前话题干（调试链不能断）；② 明显变慢 / 报错。

- **不要为了省钱 compact**：它把便宜的缓存内容换成将来 ¥1.07/M 的重读（贵 50 倍）。
- 一次 compact ≈ ¥0.1（重算整段前缀 + 生成摘要），不是主要开销，但也**省不到钱**。
- **compact 前先让 AI 把结论写进本笔记** —— 压掉的是缓存，不是知识。
- 接下来还要用今天读过的那批文件 → **别 compact**，等这个主题收尾。

**3. 怎么提问（省钱的唯一实操）**

| 类型 | 怎么说 |
|------|--------|
| 概念 / 时序 | 直接问，加一句**「先查笔记第 N 章，不够再读源码」** |
| 某段代码 | **「`文件` 的 `函数名` 我要改成 X」**（文件 + 函数名 → 只读那一段） |
| 报错 | 贴**最小**那段日志，不要整屏 |
| 大范围 | ❌「帮我分析整个 XXX 系统」；✅ 一次一个文件 |

### 硬性禁令（这些就是钱坑）

- ❌ 递归列目录（`Get-ChildItem -Recurse` 之类）—— 曾一条命令刷出 125KB ≈ 3 万 token
- ❌ 整个读大文件（笔记 409KB → **只读指定章节**）
- ❌ 重写整个源文件（把已有代码原样再输出一遍 = 按输出价 ¥4.26/M 计费）→ 用 `edit` 做小改
- ❌ 为了讲原理长篇输出 → 要讲就**一次性写进笔记**，永久复用

### 新会话开场白模板

新会话里唯一不必重复付费的就是开场那句话，所以要自带上下文：

```
继续 UE5 GAS 教程的 <章节名>。
先读 GAS-LearningNotes.md 的「<章节>」了解之前做到哪了。
本次目标：<具体要做什么>。
代码在 <路径>。
⚠️ 改 C++ 前先检测编码；含中文的文件必须是 UTF-8 with BOM。
```

### 每次学完一个功能模块

1. 更新 `GAS-LearningNotes.md`（按章节追加或更新已有章节）
2. commit & push（源码 + 蓝图 + 笔记）
3. 按上面的判据决定是否换会话

### 配置备忘

- `reasoningEffort` 已经是 `low`（`~/.dsh/profiles/desktop/cordis.patch.yml` 的 `agent-default-model`），别再动。
- 桌面 app 用 profile `desktop`（引擎 0.2.0-rc.2，端口 19387），`.bat` 用 profile `web`（npm `dsh` 0.1.5-rc.1，端口 3080）；**账单完全一样**（同一账号、同一模型、同一份 cost-meter 账本），差别只是 app 把费用/余额显示在界面上。
- `dsh-cost-meter` ≥ 1.7.18 才内置 2026-09-10 新价；官方余额核对：`https://api.deepseek.com/user/balance`。
