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

## 中文编码规则（⚠️ 关键，2026-09-13 修正）

**结论：C++ 源文件统一用 UTF-8 with BOM（带 BOM），不要用 GBK。**

### 为什么是 UTF-8+BOM 而不是 GBK

UE 会给 MSVC 传这两个开关（可在 `Intermediate/Build/.../*.obj.response` 里看到）：

```
/source-charset:utf-8
/execution-charset:utf-8
```

编译器被强制按 UTF-8 读源码。因此：

| 内容 | GBK 文件会发生什么 |
|------|--------------------|
| 中文**注释** | 不参与编译 → 程序能跑，但 IDE 里显示乱码 |
| 中文**字符串字面量** | 字节非法 → 编译期被替换成 U+FFFD（`���`）→ 运行时必然显示乱码 |

**关键区分：中文在注释里还是字面量里。**

- 中文只在注释 → GBK 也能跑（项目里 5 个 GBK 文件属于这类，所以一直"没事"）
- 中文进了字符串字面量（如 RichText 文本）→ **必须 UTF-8+BOM**，否则编译出来就是坏的

### BOM 为什么必须带

编辑器（Rider/VS）对**不含 BOM** 的文件，在中文 Windows 上保存时可能回退到系统代码页 936(GBK)，
把 UTF-8 文件改写成 GBK。**BOM（文件头 `EF BB BF`）就是阻止这件事的路标。**

### 已踩过的真实坑（2026-09-13）

`My_AuraGameplayAbilityBase.cpp` 在 git 里两次提交都是 **UTF-8 无 BOM**（当时全是 ASCII）。
加中文时被编辑器存成 **GBK**，于是 RichText 描述里的中文显示异常。

诊断命令：

```python
d = open('file.cpp','rb').read()
print('BOM:', d[:3] == b'\xef\xbb\xbf')
for enc in ['utf-8','gbk']:
    try:
        d.decode(enc); print(enc, 'OK')
    except Exception as e:
        print(enc, 'FAIL', e)
```

### 正确的读写方式

⚠️ **必须用 `decode('gbk')` / `decode('utf-8-sig')` 显式读，`bytes` 直写，并保留 CRLF。**

```python
# 读：当前是 GBK 的文件
text = open(f, 'rb').read().decode('gbk')
# 读：已是 UTF-8+BOM 的文件
text = open(f, 'rb').read().decode('utf-8-sig')
# 写：UTF-8 + BOM（bytes 直写，不经文本模式，避免换行被改）
open(f, 'wb').write(b'\xef\xbb\xbf' + text.encode('utf-8'))
```

**禁止操作**：
- 不要用 `Edit` 工具直接改含中文的 `.h`/`.cpp`（工具输出的编码不可控，会把 GBK/UTF-8 混着写坏）
- 不要全局字符串替换 `L"` → `TEXT("`（会误伤作为**结束引号**的 `L"`，破坏语法）
- 不要用编辑器的"另存为 UTF-8"来处理 GBK 文件（编辑器可能猜错源编码）；用 Python 显式指定
- 写入必须 `bytes` 直写或用 `newline=''`，否则 Windows 上会变成 `\r\r\n`，编译报 C4335

### ⚠️ 中文字符串字面量必须写成一个引号，禁止跨行拼接（2026-09-14 实证）

**这是本会话折腾最久的一个坑，结论是二进制层面验证过的。**

C++ 允许把相邻的字符串字面量自动拼接：

```cpp
// ❌ 这样写的，只有第一段是对的，后面全变成乱码
return FString::Printf(TEXT(
    "<Title>焰矢</>\n\n"
    "<Small>当前等级 </><Level>1</>\n\n"
    "<Small>消耗蓝量 </><ManaCost>%.1f</>\n\n"
    ...
), ManaCost, Cooldown, Damage);
```

```cpp
// ✅ 必须写成一对引号、一行到底
return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>1</>\n\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>\n\n<Small>升级后可同时发射更多焰矢。</>"), ManaCost, Cooldown, Damage);
```

**现象**：多段拼接时，UI 里第一段中文正常显示，从第二段起变乱码
（`当前等级` → `褰撳墠绛夌骇`）。

**已验证的边界**：

| 写法 | 结果 |
|------|------|
| 一对外引号 · 单行写满 | ✅ 正常 |
| 多对外引号 · 跨行拼接 | ❌ 第二段起乱码 |
| `\uXXXX` 全转义（纯 ASCII 源码） | ✅ 正常（可用作兜底方案） |

**判定方法**（改完编译后直接查二进制，不靠肉眼看 UI）：

```python
data = open(r'Binaries/Win64/UnrealEditor-Aura-Win64-DebugGame.dll', 'rb').read()
print('正确:', data.count('当前等级'.encode('utf-16-le')))    # 期望 > 0
print('乱码:', data.count('褰撳墠绛夌骇'.encode('utf-16-le')))  # 期望 == 0
```

乱码字 = `正确中文.encode('utf-8').decode('gbk')`，例如
`焰矢`→`鐒扮煝`、`发射`→`鍙戝皠`、`冷却时间`→`鍐峰嵈鏃堕棿`。

**写中文 RichText 描述时的规矩**（`My_AuraFireBolt.cpp` 等）：
1. 一个 `TEXT("...")` 里放**全部**内容，`\n\n` 也写在里面
2. 绝不为了「看着整齐」把字符串拆成多行多对引号
3. 文件仍是 UTF-8 with BOM + CRLF

### 检查整个项目

```python
import os
for dp,_,fs in os.walk('Source'):
    for fn in fs:
        if fn.endswith(('.cpp','.h')):
            p=os.path.join(dp,fn); d=open(p,'rb').read()
            if d[:3]!=b'\xef\xbb\xbf':
                try: d.decode('utf-8')
                except Exception: print('非UTF8:', p)
                else:
                    if any(b>127 for b in d): print('UTF8无BOM(含非ASCII):', p)
```

## 关键踩坑

### 不要用 “转成 GBK” 的旧思路（⚠️ 2026-09-13 澄清）

本文件早先的版本写着「C++ 源文件是 GBK 编码」，导致误以为「要把文件转成 GBK」。
**正确的理解是**：

- 项目里绝大多数文件本来就是 **UTF-8**（228 个 C++ 文件中 222 个是 UTF-8）
- 早期那条 “用 Python + GBK” 的规则，本意是「**保持文件原有编码**，别被 Edit 工具转坏」，
  而不是「把文件转成 GBK」
- **中文注释里的乱码**和**中文字符串的乱码**是两回事：
  - 注释乱码 → 只是看得难受，不影响程序
  - 字符串乱码 → 编译期就被破坏，运行时必然错
- 结论：**统一用 UTF-8 with BOM**，注释和字符串两种情况都是对的

### 修 C++ 中文文件的三条铁律（血泪）

1. **不要用全局字符串替换**
   `text.replace('L"', 'TEXT("')` 会误伤作为**结束引号**的 `L"`，破坏语法。
   要替换就替换**整行**，或用足够长的唯一锚点。
2. **改完必须做「剥掉字符串字面量后」的括号配对校验**
   字符串内部含 `<Default>%s, </><Level>%d</>` 这类内容，直接数括号必错。
3. **改完必须编译验证**
   脚本校验通过不等于能编译；UE 会明确报出括号/引号不匹配的行号。

### L"" 与 TEXT("") 的区别

- `L"..."` 是裸的宽字符串字面量（`wchar_t*`）
- `TEXT("...")` 是 UE 宏，按平台展开为 `L"..."` 或 `u8"..."`
- Windows 上两者都能喂给 `FString::Printf` 的 `%s`，但**UE 规范统一用 `TEXT()`**
- 同一个字面量的**前后引号必须风格一致**：`TEXT("...")` 或 `L"..."`，不能混成 `TEXT("...` + `"`


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
- `GAS-LearningNotes.md` 使用 UTF-8 编码；C++ 源文件用 UTF-8 with BOM（见上文「中文编码规则」）

## 会话与成本管理（DSH）

使用 DeepSeek Harness 学习时的会话策略。判据是**「下一话题是否需要沿用当前上下文」**，不是「教程章节是否换新」——教程章节通常连续，下一章常建立在上一章代码上。

### 判断是否开新会话

| 场景 | 操作 | 原因 |
|------|------|------|
| 同一系统延续（继续改 SpellMenu / 同一能力系统） | **继续当前会话** | 开新会话会导致重复读同一批文件，未命中 token（¥1/M）反而更贵 |
| 转到独立系统（AttributeSet / 网络复制 / GameplayCue 等） | 开新会话 | 旧系统的细节对新话题无用，留着只是白付命中费 |
| 同一 bug 连续调试中 | **坚决别开** | 开了会丢失「前面查到哪了」的进度 |
| 上下文接近上限 | `/compact` 或开新会话 | 这是为了能继续干活，**不是省钱手段** |

更省事的实操判据：**自己都说不清「上一章改了哪些文件、为什么那么改」时，就该开新会话了。**

### 每次学完一个功能模块

1. 更新 `GAS-LearningNotes.md`（按章节追加或更新已有章节）
2. commit & push（源码 + 蓝图 + 笔记）
3. 按上面的判据决定是否换会话

### 新会话开场白模板

新会话里唯一不必重复付费的就是开场那句话，所以要自带上下文：

```
继续 UE5 GAS 教程的 <章节名>。
先读 GAS-LearningNotes.md 的「<章节>」了解之前做到哪了。
本次目标：<具体要做什么>。
代码在 <路径>。
⚠️ 改 C++ 前先检测编码；含中文的文件必须是 UTF-8 with BOM。
```

### 成本结构（2026-09 实测，425 次调用）

| 项目 | 占比 | 单价 | 说明 |
|------|------|------|------|
| 缓存未命中输入 | **44%** | ¥1/M | 「新进上下文」：新问题 + 工具结果 |
| 输出 | **33%** | ¥4.32/M | thinking token 也算这里，最贵一档 |
| 缓存命中 | 23% | ¥0.02/M | 上下文堆积，便宜 50 倍 |

**关键认知**：贵的是「进新内容」，不是「读旧内容」。缓存命中率长期在 96% 左右。

### 省钱要点（按收益排序）

1. **`reasoningEffort` 从 `high` 降到 `low`** — 直接砍占 33% 的输出

   DeepSeek 只有四档（**没有 medium**）：`off` / `low` / `high` / `max`

   | 档位 | 官方定位 | 建议 |
   |------|----------|------|
   | `off` | 不需要推理的简单任务 | 只查语法/改错别字时可用 |
   | `low` | 常规任务或对延迟敏感 | **学习场景首选** |
   | `high` | 多数任务的默认平衡（**当前设置**） | 复杂调试、时序问题分析时用 |
   | `max` | 最难、质量优先的任务 | 极少需要，最贵 |

   跟教程做小改动、写注释、改 UI 这类，`low` 够用；遇到 ASC/PS 时序、
   网络复制这种硬问题时临时调回 `high`。设置位置：`~/.dsh/settings.yaml`
   的 `agent-default-model.reasoningEffort`，或在 GUI 模型下拉框里选。
2. **少让 agent 大范围读源码** — 直接砍占 44% 的未命中
   - ❌ "帮我分析整个 SpellMenu 实现"（读一堆文件，全是 ¥1/M）
   - ✅ "`My_SpellMenuWidgetController.cpp` 的 `SpellGlobeSelected` 函数我想加 X，怎么改"（只读那个函数）
3. **一次问清楚，别来回追问** — 每次追问都是一次新调用，都带一遍完整上下文
4. **新会话先读笔记，别读源码** — 笔记几 KB，源码几百 KB，建立上下文成本差一个量级
5. `/compact` 只在上下文快满时用，**不要当省钱手段**（它省不到未命中那 44%，还要花输出 token 生成摘要，并打断缓存前缀）

### 插件环境

- `dsh-cost-meter` ≥ 1.7.18 才内置 2026-09-10 新价（CNY 0.02/1/4），旧版按涨价后的旧价计算会少报约 5 倍
- 官方余额核对：`https://api.deepseek.com/user/balance`，与插件显示的差额若超过阈值，先查插件版本
