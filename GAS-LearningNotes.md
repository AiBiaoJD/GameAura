# GAS 架构学习笔记
## 目录

- [一、GAS 启动流程](#一gas-启动流程)
- [二、角色生命周期](#二角色生命周期)
- [三、GAS 核心模块](#三gas-核心模块)
- [四、一次攻击的完整数据流](#四一次攻击的完整数据流)
- [五、数据传递方式对比](#五数据传递方式对比)
- [六、GE 三种计算方式](#六ge-三种计算方式)
- [七、Source vs Target vs Server vs Client](#七source-vs-target-vs-server-vs-client)
- [八、C++ 与蓝图的职责分工](#八c-与蓝图的职责分工)
- [九、BlueprintCallable vs BlueprintPure](#九blueprintcallable-vs-blueprintpure)
- [十、GameplayCue 注意事项](#十gameplaycue-注意事项)
- [十一、关键踩坑](#十一关键踩坑)
- [十二、UI 架构：HUD → WidgetController → Widget（完整版）](#十二ui-架构hud-widgetcontroller-widget完整版)
- [十三、virtual vs BlueprintNativeEvent vs BlueprintImplementableEvent vs BlueprintCallable](#十三virtual-vs-blueprintnativeevent-vs-blueprintimplementableevent-vs-blueprintcallable)
- [十四、Live Coding 与 CDO 缓存](#十四live-coding-与-cdo-缓存)
- [十五、接口在游戏开发中的使用](#十五接口在游戏开发中的使用)
- [十六、GA 实例化策略（Instancing Policy）](#十六ga-实例化策略instancing-policy)
- [十七、Lyra 武器射击流程与 GAS 预测机制](#十七lyra-武器射击流程与-gas-预测机制)
- [十八、Model→Controller→Widget 委托架构深度解析](#十八modelcontrollerwidget-委托架构深度解析)
- [十九、InputTag → Ability 激活完整链路](#十九inputtag-ability-激活完整链路)
- [二十、Ability Commit、Cost/Cooldown 与 GE Tag 容器详解](#二十ability-commitcostcooldown-与-ge-tag-容器详解)
- [二十一、AbilityTask 体系与 AsyncTask 深入](#二十一abilitytask-体系与-asynctask-深入)
- [二十二、编辑器与工具技巧](#二十二编辑器与工具技巧)
- [二十三、Attribute 越界防护与异步任务 UI 集成](#二十三attribute-越界防护与异步任务-ui-集成)
- [二十四、属性复制完整链路：OnRep → REPNOTIFY → Delegate → UI](#二十四属性复制完整链路onrep-repnotify-delegate-ui)
- [二十五、PlayerState XP/Level 委托 → WidgetController → Widget 完整链路](#二十五playerstate-xplevel-委托-widgetcontroller-widget-完整链路)
- [二十六、敌人 XP 奖励 & Meta Attribute & Passive Ability 事件监听](#二十六敌人-xp-奖励-meta-attribute-passive-ability-事件监听)
- [二十七、PlayerInterface 桥接：AttributeSet → Character → PlayerState](#二十七playerinterface-桥接attributeset-character-playerstate)
- [二十八、接口 BlueprintNativeEvent、网络策略、冷却机制](#二十八接口-blueprintnativeevent网络策略冷却机制)
- [二十九、UE5 相机系统：SpringArm + Camera 完全指南](#二十九ue5-相机系统springarm-camera-完全指南)
- [三十、Niagara 粒子面向相机 & 构造函数组件创建细节](#三十niagara-粒子面向相机-构造函数组件创建细节)
- [三十一、AttributePoint/SpellPoint 点数系统 + 菜单初始广播](#三十一attributepointspellpoint-点数系统-菜单初始广播)
- [三十二、网络架构：客户端/服务器分工、Ability UI 显示链路、InputTag 一物三用](#三十二网络架构客户端服务器分工ability-ui-显示链路inputtag-一物三用)
- [三十三、WidgetController 架构深入 + UMG ViewModel（MVVM）对比](#三十三widgetcontroller-架构深入-umg-viewmodelmvvm对比)
- [三十四、SpellMenu 技能解锁：为什么判断放 C++，而不是把 Level 传蓝图](#三十四spellmenu-技能解锁为什么判断放-c而不是把-level-传蓝图)
- [三十五、SpellMenu 点击技能球 → 按钮 Enable：ASC/PS 复制时序与缓存策略](#三十五spellmenu-点击技能球-按钮-enableascps-复制时序与缓存策略)
- [三十六、代码规范踩坑：委托 Signature / GameplayTag 空格 / 文件编码](#三十六代码规范踩坑委托-signature-gameplaytag-空格-文件编码)
- [三十七、Spec复制 vs ClientRPC时序：描述不能依赖spec + GameMode只在服务器](#三十七spec复制-vs-clientrpc时序描述不能依赖spec--gamemode只在服务器)
- [三十八、C++ 中文字面量的多引号拼接坑（RichText 描述乱码终极定位）](#三十八c-中文字面量的多引号拼接坑richtext-描述乱码终极定位)
- [三十九、RPC 判断框架 + 复制机制 + 装备功能完整拆解](#三十九rpc-判断框架--复制机制--装备功能完整拆解)
- [四十、装备功能 My_ 版实现 + 缓存策略 + 代码复查](#四十装备功能-my_-版实现--缓存策略--代码复查)
- [四十一、异步节点与委托：C++ 怎么通知蓝图（深度版）](#四十一异步节点与委托c-怎么通知蓝图深度版)
- [四十二、SpellMenu 功能收尾（完成）](#四十二spellmenu-功能收尾完成)
- [四十三、构建速度排查：磁盘、UBT 并行、.uproject vs .sln](#四十三构建速度排查磁盘ubt-并行uproject-vs-sln)
- [四十四、伤害全链路 + 两套属性回调](#四十四伤害全链路--两套属性回调)
- [四十五、GA 数据传输三阶段：Params → Spec → Context](#四十五ga-数据传输三阶段params--spec--context)
- [四十六、ASC 的身份：AbilityActorInfo 与双端初始化](#四十六asc-的身份abilityactorinfo-与双端初始化)
- [四十七、Debuff 功能完整实现：动态 GE + Context 三段接力](#四十七debuff-功能完整实现动态-ge--context-三段接力)
- [四十八、GAS 语义澄清：Source/Target、IsDead 守卫、类型系统](#四十八gas-语义澄清sourcetargetisdead-守卫类型系统)
- [四十九、状态型特效与委托生命周期](#四十九状态型特效与委托生命周期)
- [五十、UE 接口的 U 类与 I 类](#五十ue-接口的-u-类与-i-类)
- [五十一、灼烧期间不播 HitReact：Activation Blocked Tags 机制](#五十一灼烧期间不播-hitreactactivation-blocked-tags-机制)
- [五十二、三条链路总览：Debuff / DeathImpulse / Knockback](#五十二三条链路总览debuff--deathimpulse--knockback)

---

> 2026-06-20，基于 Aura 项目实战总结

---

## 一、GAS 启动流程

```
游戏启动
  └─ AssetManager::StartInitialLoading()
       ├─ InitializeNativeGameplayTags()         — 注册所有 GameplayTag
       └─ UAbilitySystemGlobals::Get().InitGlobalData()
            ├─ 扫描 GameplayCueNotifyPaths 下的蓝图
            ├─ 缓存 TargetData 的 ScriptStruct
            └─ 注册 EffectContext 工厂（AllocGameplayEffectContext）
```

**关键配置**（`Config/DefaultGame.ini`）：

```ini
[/Script/GameplayAbilities.AbilitySystemGlobals]
+AbilitySystemGlobalsClassName="/Script/Aura.MyAbilitySystemGlobals"
+GameplayCueNotifyPaths=/Game/Blueprints/AbilitySystem/GameplayCueNotifies
+GameplayCueNotifyPaths=/Game/MyBlueprints/AbilitySystem/Enemy/Cue
```

`AbilitySystemGlobalsClassName` 指定自定义 Globals 子类（如果有自定义 EffectContext 必须配）。
`GameplayCueNotifyPaths` 指定扫描 GameplayCue 蓝图的路径，不在路径里的 Cue 不会被识别。

---

## 二、角色生命周期

### 敌人

```
Constructor → CreateDefaultSubobject(ASC, AttributeSet)
  → AutoPossessAI 检查 AIControllerClass
    → 有值 → 自动创建 My_AuraAIController
        → Possess(Pawn)
            → PossessedBy()
                ├─ InitializeBlackboard
                ├─ RunBehaviorTree()          ← 行为树自动启动
                └─ 设置黑板值(RangeAttack/CombatDistance/HitReacting)
  → BeginPlay()
      ├─ My_InitAbilityActorInfo()
      ├─ InitializeDefaultAttribute()
      └─ GiveStartupAbilities()               ← 从 DataAsset 读
```

- **AutoPossessAI**：`PlacedInWorldOrSpawned` → 两种都自动创建；`PlacedInWorld` → 只关卡放置；`Spawned` → 只 SpawnActor；`Disabled` → 必须手动调 SpawnDefaultController
- **ASC 放 Pawn 上**：敌人死后无需保留属性
- **Ability 初始化走 DataAsset**：敌人通过 `My_DA_CharacterClassInfo` 的 `CommonAbility` + `CharacterClassInformation[CharacterClass].StartupAbilities` 赋予

### 玩家

```
PossessedBy() → My_InitAbilityActorInfo() → AddCharacterAbilities()
OnRep_PlayerState() → My_InitAbilityActorInfo()   ← 客户端走这条
```

- **ASC 放 PlayerState 上**：角色重生切关属性不丢；全客户端可见
- **PossessedBy 时才能拿到 PlayerState**，BeginPlay 时 GetPlayerState() 可能还是 nullptr

### PossessedBy / OnPossess / UnPossessed

| 函数 | 视角 | 时机 |
|------|------|------|
| `PossessedBy` | Pawn | Controller 接管了我 |
| `OnPossess` | Controller | 我接管了 Pawn |
| `UnPossessed` | Pawn | Controller 释放了我 |
| `OnUnPossess` | Controller | 我释放了 Pawn |

---

## 三、GAS 核心模块

| 模块 | 作用 | 自定义 |
|------|------|--------|
| **GameplayTags** | 身份证号，所有通信的基本语言 | `FMy_AuraGameplayTags` |
| **ASC** | 发动机，每个角色一个 | `UMy_AuraAbilitySystemComponent` |
| **AttributeSet** | 属性表（血/蓝/攻/防），自动复制 | `UMy_AuraAttributeSet` |
| **GameplayEffect** | 配方，改变属性的方式 | `My_GE_CauseDamage` |
| **GameplayAbility** | 技能本体 | `UMy_AuraDamageGameplayAbility` |
| **ExeCalc** | 自定义伤害计算公式 | `My_ExeCalc_Damage` |
| **MMC** | 属性推导（如 MaxHealth = Vigor×10 + 100） | `My_MMC_MaxHealth` |
| **EffectContext** | 伤害携带的额外数据包 | `FMY_AuraGamePlayEffectContext` |
| **GameplayCue** | 特效/音效，Tag 映射到蓝图 | `GC_FireBlast` |
| **AbilityTask** | 异步等待节点 | `PlayMontageAndWait`, `WaitGameplayEvent` |
| **GameplayEvent** | Ability 内部信号（AnimNotify→逻辑） | `Montage.Attack.1` |
| **Cost/Cooldown** | 消耗和冷却，本质也是 GE | `GE_Cost_FireBlast` |
| **AbilitySystemGlobals** | 全局工厂 + Cue 路径 | `UMyAbilitySystemGlobals` |

---

## 四、一次攻击的完整数据流

```
Ability::ActivateAbility()
  ├─ MakeEffectContext()
  │   → 创建 FMY_AuraGamePlayEffectContext { bCriticalHit, bBlockedHit }
  │
  ├─ MakeOutgoingSpec(My_GE_CauseDamage, Level, Context)
  │   → 创建 FGameplayEffectSpec（伤害数据包）
  │
  ├─ SetByCaller(Damage.Fire, 50)       ← 运行时填伤害值
  │
  ├─ ApplyToTarget(Spec, TargetASC)
  │   → 触发 My_GE_CauseDamage 蓝图
  │       └─ ExeCalc::Execute()
  │           ├─ Spec.GetSetByCallerMagnitude(Damage.Fire) → 50
  │           ├─ IsCriticalHit(Context) → true
  │           ├─ SourceAttr->GetAttackPower() → 25
  │           ├─ TargetAttr->GetArmor() → 8
  │           └─ TargetAttr->SetHealth(Health - Damage)
  │
  └─ ──── 网络 ────→ 客户端
        ├─ Attribute(Health) — 自动复制（GAS 内置）
        └─ Context(bCriticalHit) — 手写 NetSerialize（你的自定义）
              ↓
         UI: 扣血+暴击黄字+GameplayCue 特效
```

---

## 五、数据传递方式对比

### SetByCaller — 传数值（最常用）

**适用**：伤害值、概率、系数、时长。**复制：自动随 GE Spec**。

- GE 蓝图配置：Modifier → MagnitudeCalculationType = SetByCaller，Tag = Damage.Fire
- C++ 填值：`SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, Value)`
- ExeCalc 读取：`Spec.GetSetByCallerMagnitude(DamageTag)`

### MMC — 属性推导

**适用**：值需要从属性动态计算（如 MaxHealth = Vigor×10 + 100）。
**复制：结果就是 Attribute 值本身，自动复制。**

```cpp
float CalculateBaseMagnitude(const FGameplayEffectSpec& Spec) const
{
    GetCapturedAttributeMagnitude(VigorDef, Spec, Params, Vigor);
    return 100.0f + 8.0f * Vigor + 10.0f * PlayerLevel;
}
```

### ExeCalc — 全手动计算

**适用**：复杂公式（暴击翻倍+护甲减伤+元素克制）。GE 的 Modifiers 全部跳过，一切你手写。

```cpp
void Execute(...)
{
    float Damage = Spec.GetSetByCallerMagnitude(DamageType);
    if (IsCriticalHit(Context)) Damage *= 2;
    Damage -= TargetAttr->GetArmor();
    TargetAttr->SetHealth(Health - Damage);
}
```

### EffectContext — 传自定义字段

**适用**：bool/Class/Actor/Vector 等复杂类型。**复制：必须手写 NetSerialize**。

- 头文件声明：`FMY_AuraGamePlayEffectContext` 继承 `FGameplayEffectContext`，加 UPROPERTY 字段
- NetSerialize：用位标记法 — 服务器先发 9bit"菜单"，客户端按菜单读，跳过空字段节省带宽
- 全局注册：`MyAbilitySystemGlobals::AllocGameplayEffectContext()` 返回自定义类型
- 蓝图桥接：在 `My_AuraAbilitySystemLibrary` 里包 BlueprintCallable

### TargetData — 目标数据

**适用**：AOE 多目标、鼠标指向位置。**复制：Ability 内置网络通道自动同步。**

### GameplayEvent — Ability 内部信号

**适用**：AnimNotify → Ability 逻辑。**不复制**，只在本 Ability 实例内广播。

```cpp
// Montage AnimNotify 触发 → WaitGameplayEvent 等待
SendGameplayEvent("Montage.Attack.1");
```

---

## 六、GE 三种计算方式

```
GE 蓝图 Modifier 配置（三选一或组合）:

  SetByCaller    → GAS 直接从 Tag 取值，不改动（你填 50 就是 50）
  MMC            → GAS 调你的 C++ 类，读属性+算，返回一个 float
  ExeCalc        → GAS 完全放手，Modifiers 全跳过，你全包
```

**关键理解**：GE 就是一张配置表，**Apply 那一刻才触发计算**。Apply 前 Source 已知（施法者），Apply 时 Target 才知道（被打的）。

---

## 七、Source vs Target vs Server vs Client

```
Source  = 攻击者/施法者（游戏角色）
Target  = 受击者（游戏角色）
Server  = 运算权（权威计算）
Client  = 表现权（UI/特效/音效）

两对完全正交的概念。
```

- 服务器持有所有人属性，读 Source → 算 → 改 Target → 复制到所有客户端
- Attribute 传"结果"（血剩多少），Context 传"过程标签"（暴击/格挡），客户端需要两者做表现

---

## 八、C++ 与蓝图的职责分工

| C++ | 蓝图 |
|-----|------|
| 数据结构定义（EffectContext、AttributeSet） | 能力流程（Montage→Wait→伤害→End） |
| 复杂计算（ExeCalc、MMC） | 节点连线/时序/条件分支 |
| 网络序列化（NetSerialize） | 数值配置（DamageEffectClass 选取） |
| 蓝图可调的工具函数（GetSpawnLocations） | 策划快速迭代，不用改 C++ |

**GAS 设计哲学**：C++ 做引擎，蓝图做配表 + 流程。UGameplayAbility 基类是纯框架，不包含任何伤害/治疗逻辑。

---

## 九、BlueprintCallable vs BlueprintPure

| | Callable | Pure |
|------|:--:|:--:|
| 执行引脚 | 有白色 Execute ▶ | 无，直接数据连线 |
| 副作用 | 可以有 | 不应有 |
| 节点颜色 | 蓝色 | 绿色 |
| 例子 | `GetSpawnLocations()`（做 LineTrace） | `GetRandomMinionClass()`（纯取值） |

---

## 十、GameplayCue 注意事项

- Ability 蓝图里直接 SpawnSystem → **只有服务器能看到，客户端看不见**
- 正确做法：Ability 带 `GameplayCue.xxx` Tag → GAS 通知所有客户端执行 `GC_xxx` 蓝图
- `GC_xxx` 蓝图放 `GameplayCueNotifyPaths` 配置的路径下
- 新建 Cue 不在已有路径下 → 必须加新路径到 DefaultGame.ini

---

## 十一、关键踩坑

1. **UE5.5 Duplicate 蓝图 Bug**：复制蓝图时 CreateDefaultSubobject 组件可能不正确继承，导致 AttributeSet 为 nullptr→崩溃。必须从 C++ 父类右键 Create Blueprint
2. **敌人 Ability 初始化**：走 DataAsset（`CharacterClassInfo`），不调 `AddCharacterAbilities()`
3. **FBlackboardKeySelector**：自定义 BTTask 必须在 `InitializeFromAsset` 中调 `ResolveSelectedKey`
4. **GBK 编码**：C++ 文件是 GBK，不能直接用 Edit/sed 改中文注释
5. **单例模式的 GameplayTags**：同一个 Tag 注册两次会导致冲突



## 十二、UI 架构：HUD → WidgetController → Widget（完整版）

### 核心分层

```
① ASC（数据层）     → 属性变化触发委托
② WidgetController（翻译层）→ GAS 原生委托→自定义委托（蓝图可用）
③ Widget（显示层）   → 蓝图绑定、更新 UI
```

### 为什么需要中间层（委托桥接）

GAS 内置 `GetGameplayAttributeValueChangeDelegate` 返回 **C++ 原生多播委托**（DECLARE_MULTICAST_DELEGATE），蓝图不能直接绑定。
必须用 WidgetController 将 C++ 原生委托 → `BlueprintAssignable` 动态委托。

```
ASC 原生委托（蓝图不可见）
  → AddLambda → OnHealthChanged.Broadcast(Data.NewValue)
    → 蓝图 Bind Event → SetPercent/SetText
```

### 继承层级

```
AHUD
└── AMy_AuraHUD                            ← 管理器：创建 Widget + WidgetController
      \ 持有: OverlayWidget, OverlayWidgetController, MenuWidgetController

UObject
├── UMy_AuraWidgetController               ← 基类：持有 PC, PS, ASC, AS
│     \ 方法: SetWidgetControllerParams / BroadcastInitiaValues / BindCallbacksToDependencies
│     ├── UMy_OverlayWidgetController      ← 玩家主 HUD：Health/Mana + 消息
│     │     \ 委托: OnHealthChanged, OnMaxHealthChanged, OnManaChanged, OnMaxManaChanged
│     └── UMy_AttributeMenuWidgetController ← 属性面板：所有属性详细信息
│           \ 委托: OnAttributeInfo

UUserWidget
└── UMy_AuraUserWidget                     ← 所有 Widget 基类
      \ 持有: TObjectPtr<UObject> WidgetController
      \ 方法: SetWidgetController() → 存引用 + 触发 BP 事件 WidgetControllerSet()
      \ BP 子类: My_BP_OverlayWidget, My_WBP_EnemyHealthBar
```

### 玩家完整流程

#### 初始化阶段

1. **PlayerState 构造**（My_AuraPlayerState.cpp:15-20）
   `CreateDefaultSubobject<ASC>` + `CreateDefaultSubobject<AS>`，所有属性初值 = 0

2. **PossessedBy / OnRep_PlayerState**（Aura_Character.cpp:33/49）
   → `My_InitAbilityActorInfo()`
   - `InitAbilityActorInfo(PlayerState, this)`（:67）告诉 ASC Owner=PS, Avatar=Character
   - `AbilityActorInfoSet()`（:70）绑 GE 应用回调 → RPC（只影响消息 UI）
   - 从 PS 拷贝 ASC/AS 指针（:73-74）
   - **`HUD->InitOverlay(PC, PS, ASC, AS)`（:85）** ← 关键入口
   - `InitializeDefaultAttribute()`（:91）应用初始 GE

3. **HUD::InitOverlay()**（My_AuraHUD.cpp:33-54）：
   - `CreateWidget` 创建 Widget（:39）
   - `GetOverlayWidgetController` 创建 Controller → `BindCallbacksToDependencies()` 绑定 ASC 委托（:16）
   - `Widget->SetWidgetController(OWC)` 关联 Widget 与 Controller（:47）
     → 触发蓝图 `WidgetControllerSet()` 事件，蓝图里 Bind Event
   - `BroadcastInitiaValues()` 推送当前值（:50）

#### 运行时阶段

1. GE 修改属性 → `PreAttributeChange()`（My_AuraAttributeSet.cpp:82）钳制
2. GAS 内部触发 `GetGameplayAttributeValueChangeDelegate`
3. WidgetController Lambda 收到 → `OnHealthChanged.Broadcast(NewValue)`（My_OverlayWidgetController.cpp:29）
4. Widget 蓝图收到 → 更新血条/文字

5. `PostGameplayEffectExecute()`（AS.cpp:104）
   - 处理 IncomingDamage → SetHealth → 可能再次触发委托
   - 致死判定：`CombatInterface->Die()`
   - 受击标签：`TryActivateAbilitiesByTag(HitReact)`
   - 伤害数字：`ShowDamageText()` → PC RPC 显示

#### 消息 UI 走不同路线

1. GE 应用 → `OnGameplayEffectAppliedDelegateToSelf`（仅服务端触发）
2. `ClientEffectApplied` RPC（My_AuraAbilitySystemComponent.cpp:22-31）
3. 客户端提取 AssetTags → `EffectAssetTags.Broadcast(TagContainer)`
4. WidgetController Lambda 检查 Message 标签（OverlayWidgetController.cpp:52-73）
5. `OnMessageWidgetRow.Broadcast(Row)` → 消息 Widget 显示

### 敌人流程（自身兼任 WidgetController）

**BeginPlay()**（Enemy_Characte.cpp:104-145）：

```
① My_InitAbilityActorInfo()
     → InitAbilityActorInfo(this, this)   ← Owner=Avatar=自己
     → AbilityActorInfoSet()
     → InitializeDefaultAttribute()        ← 从 DataAsset 根据职业/等级选 GE

② UWidgetComponent->GetUserWidgetObject()
     → AuraUserWidget->SetWidgetController(this)   ← 敌人自己是 Controller
       → 触发蓝图 WidgetControllerSet() → Bind Event OnHealthChanged

③ ASC->GetGameplayAttributeValueChangeDelegate(Health).AddLambda(   :123
       [this](auto& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
   )
   ASC->GetGameplayAttributeValueChangeDelegate(MaxHealth).AddLambda( :128)

④ OnHealthChanged.Broadcast(当前值)     :134  推初始值
   OnMaxHealthChanged.Broadcast(当前值)  :135
```

### 玩家 vs 敌人对比

| | 玩家 | 敌人 |
|---|---|---|
| ASC/AS 位置 | PlayerState（跨关卡存活） | Character 自身 CreateDefaultSubobject |
| WidgetController | UMy_OverlayWidgetController（独立 UObject） | AEnemy_Characte 自身（this） |
| 委托绑定 | BindCallbacksToDependencies() | BeginPlay 里手写 Lambda |
| HUD | AMy_AuraHUD 管理 | 无（WidgetComponent 直接挂角色上） |
| 委托声明 | OverlayWidgetController.h:52-61 | Enemy_Characte.h:41-45（同一类型） |
| 调用时机 | PossessedBy / OnRep_PlayerState | BeginPlay |

### 所有委托速查

| 委托 | 类型 | 定义位置 | 触发者 | 绑定者 |
|------|------|---------|-------|-------|
| GetGameplayAttributeValueChangeDelegate | C++ 原生 | GAS 内置 | ASC 内部自动 | WidgetController 的 AddLambda |
| OnHealthChanged | BlueprintAssignable | OverlayWidgetController.h:52 | WidgetController/Enemy | Widget 蓝图 WidgetControllerSet |
| EffectAssetTags | C++ 原生 | My_AuraASC.h | ClientEffectApplied RPC | OverlayWidgetController Lambda |
| OnMessageWidgetRow | BlueprintAssignable | OverlayWidgetController.h:64 | OverlayWidgetController | 消息 Widget 蓝图 |
| OnAttributeInfo | BlueprintAssignable | AttributeMenuWidgetController.h:30 | AttributeMenuWidgetController | 属性面板 Widget 蓝图 |

### WidgetController 四个参数

`FMY_WidgetControllerParams` 打包 PC, PS, ASC, AS。当前**实际只用到了 ASC 和 AS**。
PC 和 PS 是预留的（读玩家等级、控制输入等），存了但未使用。

### 为什么不能在 BeginPlay 初始化 UI

- 客户端 BeginPlay 时 PlayerState 可能还没网络复制过来
- PossessedBy（服务端）和 OnRep_PlayerState（客户端）时四个参数才都有效
- 敌人的 BeginPlay 可以——因为 ASC/AS 是自己 CreateDefaultSubobject 的，不依赖网络

### GE 执行顺序与委托触发

```
① PreAttributeChange(Attribute, NewValue)  — 可修改 NewValue（钳制用）
② GAS 写入属性值                            — 触发 GetGameplayAttributeValueChangeDelegate
③ PostAttributeChange(Attribute, Old, New) — 项目未使用
④ PostGameplayEffectExecute(Data)          — IncomingDamage / SetHealth（可能再次触发②）
```

### GAS 上下文结构速查

| 结构 | 一句话 | 位置 |
|------|--------|------|
| FAbilityActorInfo | ASC 的 "我是谁"（Owner, Avatar, PC） | ASC 内部 |
| FGameplayEffectContext | GE 的 "谁打的"（Instigator, HitResult） | GE Spec 内部 |
| FGameplayEffectSpec | GE 完整数据包（等级、Tag、Modifiers、Context） | 应用 GE 时 |
| FGameplayEffectModCallbackData | PostGE 回调参数（EffectSpec + EvaluatedData + Target） | PostGameplayEffectExecute |
| FMy_EffectProperties | 从上面提取的简化版（Source/Target 角色+ASC） | AS.cpp:176 |
| FOnAttributeChangeData | 属性变了多少（OldValue, NewValue, Attribute） | Lambda 参数 Data.NewValue |

### TSubclassOf vs TObjectPtr

```cpp
UPROPERTY(EditAnywhere)
TSubclassOf<UMy_AuraUserWidget> OverlayWidgetClass;  // 配方：蓝图里选"哪个类"

UPROPERTY()
TObjectPtr<UMy_AuraUserWidget> OverlayWidget;         // 蛋糕：运行时 CreateWidget 造出来的实例
```

### PlayerState 和 PlayerController 在 GAS 中的角色

- **PlayerState** = 替身。Characters 会销毁，PS 跨关卡存活，ASC/AS 放 PS 上不丢
- **PlayerController** = 执剑人。处理输入、拥有 HUD、有网络权限发 RPC
- **HUD** 由 GameMode 的 HUDClass 配置，引擎自动 spawn 并挂到 PlayerController，不需要手动创建

### MVVM 简介

Model-View-ViewModel，UE5.5 成熟。把手动委托翻译自动化：
- 变量标记 `FieldNotify` → 引擎自动通知
- Widget 属性右键 Create Binding → 选 ViewModel 变量
- 不需要自定义委托、不需要 Lambda 转发、不需要 BroadcastInitiaValues
- ViewModel 本质上就是 WidgetController，只是通知机制从手动变为引擎自动

### 初始值推送的必要性

`GetGameplayAttributeValueChangeDelegate` 只在值**变化**时触发。初始化时属性已是 100，
Widget 绑定时不会收到回调。所以必须手动 `BroadcastInitiaValues()` 推一次当前值。
`BindCallbacksToDependencies` = 管线（管将来），`BroadcastInitiaValues` = 快照（推当前）。

### 两种调用时机模式

`BindCallbacksToDependencies` 和 `BroadcastInitiaValues` 都在 **HUD** 里调用，但推送初始值的时机不同：

**Overlay（常驻 HUD）— C++ 侧全包**（My_AuraHUD.cpp:33-54）：

```
InitOverlay()
  ├─ GetOverlayWidgetController()  → BindCallbacksToDependencies()  // 绑管线
  ├─ Widget->SetWidgetController(OWC)                               // Widget 关联 Controller
  └─ OWC->BroadcastInitiaValues()                                   // 推快照（HUD 初始化时立即推）
```

Overlay 常驻显示，HUD 初始化时 Widget 已创建好，所以在 C++ 里直接推初始值。

**AttributeMenu（按需打开的面板）— BP 侧推快照**（My_AuraHUD.cpp:21-29）：

```
GetMenuWidgetController()
  └─ BindCallbacksToDependencies()   // 只绑管线，不推快照

// 蓝图 Widget 的 Construct 事件：
Construct
  ├─ SetWidgetController(MenuWidgetController)   // 关联 Controller
  └─ BroadcastInitiaValues()                     // 推快照（Widget 构造时才调）
```

Menu 是按需打开的面板，HUD 只负责创建 Controller + 绑回调。`BroadcastInitiaValues` 推迟到蓝图层 Widget 的 `Construct` 里调用——Menu 打开时才推当前值，避免提前推送没人听的广播。

**总结**：两种方式本质一样，只是**快照推送的时机不同**——常驻 UI 在 HUD 创建时推，按需 UI 在 Widget 构造时推。

---

## 十三、virtual vs BlueprintNativeEvent vs BlueprintImplementableEvent vs BlueprintCallable

| 关键字 | C++ 默认实现 | BP 可调 | BP 可覆盖 | 适用场景 |
|--------|:-----------:|:------:|:--------:|---------|
|  | ✅ 有 | ✅ | ❌ | 逻辑固定，所有类型一样 |
|  | ✅ 有() | ✅ | ✅ | C++ 给默认，BP 可选覆盖 |
|  | ❌ 无 | ✅ | ✅(必须) | 纯表现层，C++ 不管 |
| 无 UFUNCTION 的  | ✅ 或  | ❌ | ✅(C++子类) | 纯 C++ 内部调用 |

**核心区别**： = C++ 子类可重写， = BP 子类也可重写。UE 的 UFUNCTION 会自动生成 virtual，所以接口函数不加 virtual 也是虚函数。

---

## 十四、Live Coding 与 CDO 缓存

### CDO (Class Default Object)

每个 UClass 启动时创建的一个模板对象，BP 编译后的字节码、属性绑定、所有 C++ 属性偏移量全固化在里面。

### 内存布局

C++ 对象在内存中是一段连续字节，父类成员在前，子类成员在后。BP 编译时记的是**硬偏移量**而非属性名。



### 为什么有时 Live Coding 可以，有时不行

| 能 Hot Reload | 不能（必须重启编辑器） |
|--------------|---------------------|
| 函数体  修改 | 增删 UPROPERTY |
| 新增未被引用的属性 | 增删 UFUNCTION / 虚函数 |
| Tag 增删 | 修改类继承层级 |
| | **基类改动**（所有子类 CDO 全脏） |

**根因**：Live Coding 只 patch 内存中的 C++ 类，不重建 BP 的 CDO。类布局变化时 CDO 里的旧偏移量指向错误位置，导致"missing property"。

---

## 十五、接口在游戏开发中的使用

### 什么时候用接口

一个信号：**你写代码时发现自己在问"这个东西是什么类型？"**。

三类典型场景：

1. **调用方不关心你是谁，只关心你能做什么**
   - ExeCalc 读等级：
   - 比  强——不依赖具体类

2. **完全不相关的类型共享同一个行为**
   - 角色、木桶、可破坏门都需要 ，但不可能共享一个基类

3. **蓝图避免 Cast 地狱**
   -  vs 每个类型写一个分支

### 什么时候不用接口

接口**不能有成员变量、不能有非虚函数体、不能有构造函数逻辑**。数据（ASC, AttributeSet）和通用逻辑（InitAbilityActorInfo）必须放基类。



### 接口两种声明方式的影响

# 0 "<stdin>"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "<stdin>"

### GAS 项目不一定需要额外伤害接口

UE 内置的  已经提供 。只要所有会受伤的东西都挂 ASC， 通过  统一处理，不需要 。只有无 ASC 的东西（如简单木桶）才考虑新接口。

### 实际开发节奏

前期不急着设计接口 → 类型多了 Cast 写到痛 → 自然知道该抽什么。但以下接口建议早期就写：**伤害/受伤**、**交互系统**、**存档/读档**。UE 引擎本身的 、 就是这个思路。

---

## 十六、GA 实例化策略（Instancing Policy）

### 三种策略

| 策略 | 实例 | 生命周期 | 变量持久化 | 能用 EndAbility？ |
|------|------|---------|-----------|:--:|
| `NoInstancing` | 无，跑在 CDO 上 | 永久(CDO) | N/A | ❌ |
| `InstancedPerExecution` | 每次激活新建 UObject | 新实例，旧实例挂起 | ❌ 天然归零 | ✅ 调不调都能再激活 |
| `InstancedPerActor` | 每个 Actor 一个 | 跟 Actor 同寿 | ✅ 保留上次值 | **必须调**，否则不允许多次激活 |

### ASC 内部视角

```
InstancedPerExecution:
  ASC
  └── AbilitySpec(SummonTag)
        ├── 实例 A（第一次激活，已 End）
        ├── 实例 B（第二次激活，已 End）
        └── 实例 C（第三次激活，活跃中）

InstancedPerActor:
  ASC
  └── AbilitySpec(SummonTag)
        └── 唯一实例（每次激活复用，变量不重置）
```

### 为什么 EndAbility 不会让 delegate 失效

**核心事实**：蓝图的 `AddDynamic`（Assign Delegate 节点）是 **UObject 强引用**。目标对象只要被引用就不会被 GC。

```
小兵 Actor → OnDestroyed delegate → AddDynamic(GA 实例)
                                        ↑
                                 强引用，GA 实例引用计数+1
```

`EndAbility` 做的事：通知 ASC "这次激活周期结束了"，改了一个状态标记。**不删除 UObject，不释放内存。**

GA 实例的生命周期由 GC 决定，不由 `EndAbility` 决定。只要还有 UObject 持有它的强引用，它就活着。

### Summon GA 实例生命周期（InstancedPerExecution）

以 Summon GA 为例，实战验证：

```
第一次激活:
  ASC 建实例 A → Spawn 5 只小兵 → 每只 OnDestroyed 强引用 A
  → EndAbility（或不调）→ ASC 标记 A 不活跃
  → 5 只小兵活着 → 5 个强引用指向 A → GC 不收 A
  → 某只小兵死 → delegate 触发 → IncreaseMinionCount(-1) → 正常执行
  → 5 只全死 → 所有引用释放 → A 被 GC

第二次激活（MinionCount < 5，BT 再调 TryActivateAbilitiesByTag）:
  ASC 建新实例 B → SpawnLocationIndex 天然为 0 → 和第一次完全一致
```

A 和 B 各自独立，互不干扰。**教程用 `InstancedPerExecution` + False 分支不加 EndAbility 是对的**。

### InstancedPerActor 的额外代价

切到 `InstancedPerActor` 后需要手动处理两件事：

1. **变量持久化**：`SpawnLocationIndex` 第一次激活后 = 5，第二次还是 5。必须在 `K2_ActivateAbility` 开头手动归零
2. **必须 EndAbility**：唯一实例还处于 Active 状态时，`TryActivateAbilitiesByTag` 判定已激活，跳过不执行

**选择建议**：如果 GA 需要绑 delegate 等异步回调，两种策略都可以——`InstancedPerExecution` 更省事（变量自动归零、不需要 EndAbility），`InstancedPerActor` 适合需要跨激活持久保存状态的场景。


---

## 十七、Lyra 武器射击流程与 GAS 预测机制

> 2026-06-24，对比 Lyra Hitscan 方案与项目 My_ 投射物方案

### 17.1 四种 GAS Ability 网络配置

C++ 构造函数中设置的四个属性（Lyra 的  基类）：

# 0 "<stdin>"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "<stdin>"

| 配置 | 含义 | Lyra 选择 |
|------|------|-----------|
| ReplicationPolicy | Ability Spec 要不要复制到客户端 | ReplicateNo（GiveAbility 提前授权） |
| InstancingPolicy | 每次激活 new 还是复用 | InstancedPerActor（需要存成员变量） |
| NetExecutionPolicy | Ability 代码在哪端执行 | **LocalPredicted**（两端都跑） |
| NetSecurityPolicy | 谁有权发起激活 | ClientOrServer（玩家按鼠标发起） |

### 17.2 GAS 预测覆盖范围

GAS 预测只覆盖  内通过 ASC API 做的操作：



**本质**：GAS 预测 = 客户端提前改 Attribute 值，等服务器 Attribute 复制回来比对，不一致就自动回滚。
Actor 生成不在预测范围内，UActorChannel 只做"服务器生成 → 自动复制到客户端"。

### 17.3 Lyra 完整开枪流程（自动步枪）



### 17.4 为什么 Lyra 用 Hitscan 而不是投射物

**Hitscan** = 瞬时射线检测（LineTrace），弹道飞行时间 = 0ms。

GAS 预测模型天然适配瞬时操作：开枪 → 0ms 命中 → Apply GE → GAS 预测接管。
投射物有"飞行中"状态（300ms），这段时间 GAS 帮不了你——弹道预测、碰撞预测都得自己写。

Lyra 不做投射物不是偷懒，是刻意回避了投射物预测的复杂性。
如果 Lyra 要做火箭筒，需要上"Client-Fakey, Server Auth"方案。

### 17.5 客户端预测 vs 服务器权威

| | Lyra Hitscan | My_版（投射物） | Client-Fakey（工业PvP） |
|---|---|---|---|
| 弹道 Actor | 无 | 有 | 有（3类：Client/Server/Sim） |
| bReplicates | — | true | **false**（都不复制） |
| 客户端感知延迟 | 0ms | ≈RTT/2 | 0ms（本地假弹） |
| 特效播放 | 本地直接播 | Multicast RPC | 各端独立播 |
| 伤害 | 服务器 Apply GE | 服务器 Apply GE | 服务器 Apply GE |
| 适用 | PvP 射击 | PvE ARPG | PvP 弹道武器 |

### 17.6 My_版 HasAuthority() 是否多余

**不多余，必须保留**。原因：

- Actor 生成必须服务器权威，即使设了 LocalPredicted，客户端 SpawnActor 也不会自动复制到服务器
- 投射物碰撞必须在服务器算伤害，客户端只依赖 Actor 复制看位置
- PvE ARPG 不需要客户端预测弹道——100ms 延迟感知不到

### 17.7 输入排队机制（ProcessAbilityInput）

**不是 GAS 内置，是 Lyra 自己写的**。GAS 内置只有 PressInputID/ReleaseInputID（数字 ID）。

Lyra 新增：GameplayTag 匹配 + 三个数组（Pressed/Held/Released）+ 激活策略枚举 + ProcessAbilityInput。

排队原因：
1. 防止同帧按下+松开导致 WaitInputRelease 丢失
2. 保证 InputPressed 标记在 TryActivateAbility 之前设置
3. 统一"激活新实例 vs 传事件给已激活实例"的判断入口

### 17.8 TargetData 流程与 RPC 数量

一次开枪涉及 2 条 GAS 内置 RPC：



Attribute 复制是 UActorChannel 的一部分，不是独立 RPC。

### 17.9 Lyra 的 UnconfirmedHitMarkers

Lyra 自定义的命中标记预测系统——客户端 Trace 命中后立刻显示"命中标记"UI，
等服务器 ClientConfirmTargetData 回来后清理或撤回。和 GAS 伤害预测是两套独立系统。

### 17.10 选择建议

- PvE ARPG → My_版方案（服务器生成投射物 + Actor 复制 + Multicast RPC）— 简单够用
- PvP 射击 → 优先考虑 Hitscan（Lyra 方案），实在需要弹道才上 Client-Fakey
- Client-Fakey：三种投射物分开（Client/Server/Sim），bReplicates=false，对象池，服务器只跑碰撞不跑 VFX

---

## 十八、Model→Controller→Widget 委托架构深度解析

### 18.1 两条数据流的本质差异

项目中存在两条 UI 数据流，复杂度不同：

| | 属性变化（健康/法力） | 能力信息（图标/输入绑定） |
|---|---|---|
| 委托数量 | 2 个 | 3 个 |
| 数据性质 | **持续流**（GE 随时修改属性） | **一次性事件**（GiveAbility 只调一次） |
| 监听方式 | 系统内置，永久监听 | 自己写，需处理时序竞态 |
| 复杂度来源 | 翻译层（Tag→Widget 数据格式） | 时序竞态 + 翻译层 |

### 18.2 属性变化：2 委托 = 工业标准

```
ASC (Model)
  │ OnGameplayEffectAppliedDelegateToSelf（系统委托，永久监听）
  ▼
OverlayWidgetController (中间层)
  │ 翻译：FGameplayEffectSpec → float NewValue
  │ 广播自定义委托
  ▼
UserWidget
  │ OnHealthChanged (蓝图委托) → 更新 UI
```

- 系统委托是**流式的**——绑定一次，后续所有 GE 修改都自动触发
- `BroadcastInitiaValues()` 手动推当前值兜底
- **这套就是 Lyra 的做法，工业标准**

### 18.3 能力信息：为什么多了第 3 个委托

能力授予（`GiveAbility`）只在初始化时调一次，**错过就没有第二次通知**。存在时序竞态：

```
情况 A：GiveAbility 先执行，Controller 后创建
  → OnAbilityGiven 已广播 → Controller 错过了
  → 需要标志位兜底

情况 B：Controller 先创建，GiveAbility 后执行
  → 绑定委托 → 等通知 → 收到后处理
  → 正常路径
```

所以多了 `OnAbilityGiven`——它不是一个"翻译层"，而是一个**时钟同步信号**：

```
AddCharacterAbilitiesFromASC
    │ Broadcast OnAbilityGiven（时钟信号——"能力给好了"）
    ▼
OnInitializeStartupAbilities（Controller 回调）
    │ 绑定 Fmy_ForEachAbility::BindLambda(...)
    │ 调用 ASC->ForEachAbility(OnEachAbility)
    ▼
ForEachAbility 内部（安全遍历 + FScopedAbilityListLock）
    │ Execute(每个 AbilitySpec)
    ▼
Lambda：查 AbilityDA → 组装 FMy_AuraAbilityInfo → Broadcast OnAbilityInfo
    ▼
Widget 接收 OnAbilityInfo → 更新 UI
```

### 18.4 三个委托的职责

| 委托 | 方向 | 类型 | 职责 |
|---|---|---|---|
| `OnAbilityGiven` | M→C | 多播 | 时钟信号——解决"谁先谁后"的时序问题 |
| `Fmy_ForEachAbility` | M→C | 单播 | 回调参数——安全遍历（FScopedAbilityListLock） |
| `OnAbilityInfo` | C→W | 多播 | 翻译层——组装好的 AbilityInfo 传给 Widget |

单播委托用 `Execute` 不用 `Broadcast` 的原因：单播只有一个接收者，`Bind` + `Execute` 语义更准确。

### 18.5 Fmy_ForEachAbility 为什么是局部变量

```cpp
Fmy_ForEachAbility OnEachAbility;  // 栈上创建
OnEachAbility.BindLambda([...]{...});
AuraASC->ForEachAbility(OnEachAbility);  // 同步执行，用完即弃
// 函数返回，OnEachAbility 销毁——安全
```

`ForEachAbility` 是**同步**的——当场遍历、当场 Execute。局部变量在整个调用期间存活，函数返回后才销毁，不存在悬垂引用。

### 18.6 工业上的替代方案

**方案一：分阶段初始化（最常用）**

从流程上保证 UI 一定在 Ability 之后创建：

```
BeginPlay → Init ASC → GiveAbility → Set bReady = true
                                      → Create UI / WidgetController
                                      → 检查 bReady（永远 true，不需要委托）
```

很多项目就这么干——靠游戏流程消除时序，代码里根本不需要 `OnAbilityGiven`。

**方案二：标志位 + 委托兜底（本项目）**

保证不了创建顺序时（多人、动态加载、网络复制）就用这套。UE 源码里自己也是这么写的，`UAbilitySystemComponent::OnRegister` 里一堆类似模式。

**方案三：MVVM / ViewModel**

UE5.1 有 `UMVVMViewModelBase`，但 GAS 项目几乎没人用——GAS 委托体系太成熟了，硬套 ViewModel 反而多一层胶水。WidgetController 就是 UE 社区约定俗成的 "ViewModel"，只是没叫这名字。

### 18.7 委托速查

| | 单播 | 多播 |
|---|---|---|
| 声明 | `DECLARE_DELEGATE` | `DECLARE_MULTICAST_DELEGATE` |
| 绑定 | `BindStatic / BindUObject / BindLambda` | `Add / AddUObject / AddDynamic` |
| 调用 | `Execute()` / `ExecuteIfBound()` | `Broadcast()` |
| 接收者 | 1 个 | N 个 |

### 18.8 RAII 锁

```cpp
FScopedAbilityListLock ActiveScopeLock(*this);  // 构造 = 锁定
for (...) { ... }
// 析构 = 自动解锁（return / 异常都安全）
```

**必须给变量名**——无名临时对象会在本行结束就析构，等于没锁。

---

## 十九、InputTag → Ability 激活完整链路

### 19.1 链路总览

```
物理按键（RMB）
  → Enhanced Input Action
    → InputConfig DataAsset（FAuraInputAction: InputAction → InputTag）
      → UAuraInputComponent::BindAbilityActions()
        → PlayerController::AbilityInputTagHeld(InputTag)
          → ASC::AbilityInputTagHeld(InputTag)
            → 遍历 GetActivatableAbilities()
            → Spec.DynamicAbilityTags.HasTagExact(InputTag) 匹配
              → TryActivateAbility() → 能力激活
```

### 19.2 三层映射关系

| 层 | 做什么 | 代码位置 |
|---|---|---|
| **InputConfig** | 物理按键 → InputTag | `AuraInputConfig.h:11-20`，`FAuraInputAction` 结构体 |
| **ASC 匹配** | InputTag → AbilitySpec | `AuraAbilitySystemComponent.cpp:95-110`，`AbilityInputTagHeld()` |
| **UI 槽位** | AbilitySpec → InputTag（反过来读） | `My_OverlayWidgetController.cpp:91-107`，`GetInputTagFromAbilitySpec()` |

### 19.3 StartupInputTag 写入时机

能力授予时，从 CDO 读取 `StartupInputTag` 写入 `DynamicAbilityTags`（**不是激活时才写**）：

```cpp
// AuraAbilitySystemComponent.cpp:52-66
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);  // ← 这里
            GiveAbility(AbilitySpec);
        }
    }
}
```

改蓝图的 `StartupInputTag` 从 `InputTag.LMB` 到 `InputTag.RMB`，就是改了匹配键。ASC 遍历时不再匹配 LMB，转为匹配 RMB。

### 19.4 运行时切换槽位

`ServerEquipAbility()` 可以通过 `AssignSlotToAbility()` 运行时换 InputTag：

```cpp
// AuraAbilitySystemComponent.cpp:240-244
void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
    ClearSlot(&Spec);                        // 移除旧 InputTag
    Spec.DynamicAbilityTags.AddTag(Slot);    // 写入新 InputTag
}
```

本质都是操作 `DynamicAbilityTags`——ASC 匹配逻辑不关心标签是静态写的还是运行时换的，只认标签本身。

### 19.5 UI 读取能力信息

```cpp
// My_OverlayWidgetController.cpp:91-107
void UMy_OverlayWidgetController::OnInitializeStartupAbilities(UMy_AuraAbilitySystemComponent* AuraASC)
{
    Fmy_ForEachAbility OnEachAbility;
    OnEachAbility.BindLambda([this, AuraASC](const FGameplayAbilitySpec& AbilitySpec)
    {
        FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(
            AuraASC->GetAbilityTagFromAbilitySpec(AbilitySpec)   // 从 Ability.AbilityTags 找 "My_Abilities" 前缀
        );
        info.InputTag = AuraASC->GetInputTagFromAbilitySpec(AbilitySpec);  // 从 DynamicAbilityTags 找 "My_InputTag" 前缀
        OnAbilityInfo.Broadcast(info);  // UI Widget 收到并显示
    });
    AuraASC->ForEachAbility(OnEachAbility);
}
```

两个方向用同一个 InputTag 做键：
- **正方向**：按键 → InputTag → 激活能力
- **反方向**：AbilitySpec → DynamicAbilityTags → 读 InputTag → UI 槽位显示

### 19.6 GiveAbility 必须在服务器调用

`UAbilitySystemComponent::GiveAbility()` 内部检查 `IsOwnerActorAuthoritative()`，**非权威端调用无效**，不会把 AbilitySpec 加到 `ActivatableAbilities`。外层去掉 `HasAuthority()` 绕不过引擎内部检查。客户端能力列表只能通过：**服务器 GiveAbility → 网络复制 → `OnRep_ActivateAbilities()`** 获得。

### 19.7 OnRep_ActivateAbilities 的作用

```cpp
// AuraAbilitySystemComponent.cpp:448-457
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();  // 引擎：替换客户端 ActivatableAbilities 为服务器数据
    if (!bStartupAbilitiesGiven)
    {
        bStartupAbilitiesGiven = true;
        AbilitiesGivenDelegate.Broadcast();  // 通知 UI：能力复制到了，可以刷新了
    }
}
```

如果自定义 ASC 没有重写这个函数，客户端在能力复制到达后不会收到任何通知，UI 永远不会刷新。

### 19.8 LMB 的特殊处理

LMB 在 PlayerController 中有双重语义——**寻路 + 攻击**，通过 `TargetingStatus` 区分：

```cpp
// AuraPlayerController.cpp:210-237
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(InputTag_LMB))
    {
        GetASC()->AbilityInputTagHeld(InputTag);  // RMB/1/2/3/4 直接转发 ASC
        return;
    }
    // LMB：检查是否 TargetingEnemy 或 Shift按下 → 攻击；否则 → 寻路
    if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
        GetASC()->AbilityInputTagHeld(InputTag);
    else
        // 寻路逻辑...
}
```

这就是为什么改 RMB 后右键直接攻击——RMB 跳过 LMB 的寻路分支，直接走 ASC 激活。

---

## 二十、Ability Commit、Cost/Cooldown 与 GE Tag 容器详解

> 2026-06-30，深入 CommitAbility 流程、GE 五大 Tag 容器、能力网络配置实践

### 20.1 CommitAbility 完整流程

`UGameplayAbility::CommitAbility()` 是能力的"最终审核关卡"：

```cpp
bool UGameplayAbility::CommitAbility(...)
{
    // ① 最后一道检查（验资源 + 验冷却 + 验 Tag）
    //    注释: "Last chance to fail" — 激活与 Commit 之间可能有时间差
    if (!CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
        return false;  // 不通过 → 能力激活中止

    // ② 执行消耗（扣资源 + 上冷却）
    CommitExecute(Handle, ActorInfo, ActivationInfo);

    // ③ 蓝图 Hook（OnCommitAbility 事件入口）
    K2_CommitExecute();

    // ④ 广播通知（UI 监听冷却/资源变化用）
    ActorInfo->AbilitySystemComponent->NotifyAbilityCommit(this);

    return true;
}
```

**CommitCheck 内部三道关卡**：

| 检查项 | 源码函数 | 说明 |
|--------|---------|------|
| 冷却检查 | `CheckCooldown()` | `ASC->HasAnyMatchingGameplayTags(CooldownTags)` → 有 Tag = 冷却中 = 失败 |
| 资源检查 | `CheckCost()` | 当前属性值是否 ≥ Cost 所需（如 Mana ≥ 30） |
| Tag 条件 | `DoesAbilitySatisfyTagRequirements()` | Ability Tag 是否被 Block 禁用 |

**CommitExecute 内部**：

```cpp
void CommitExecute(...)
{
    ApplyCost(Handle, ActorInfo);        // 应用 CostGE → 扣除资源
    ApplyCooldown(Handle, ActorInfo);    // 应用 CooldownGE → 授予冷却 Tag
}
```

**关键**：蓝图勾选 "Commit Ability" 或 C++ 手动调 `CommitAbility()`，这套逻辑才生效。不调 → 跳过所有检查 → 无条件激活 — 这就是项目当前状态。

### 20.2 CheckCooldown 为什么只查 Tag 不删 Tag

```cpp
bool CheckCooldown(...)
{
    if (ASC->HasAnyMatchingGameplayTags(*CooldownTags))
    {
        OptionalRelevantTags->AddTag(ActivateFailCooldownTag);  // 记下失败原因
        return false;  // 有冷却 Tag → 不放行
    }
    return true;  // 没有 → 放行
}
```

`CheckCooldown` 是**看门大爷**（只查证），不是**清洁工**（不删 Tag）。

Tag 的移除由 **Duration GE 的生命周期**自动完成：

```
ApplyCooldown() → ASC->ApplyGameplayEffectToSelf(CooldownGE, Duration=5.0)
  → GE 挂到 ActiveGameplayEffects 列表
  → GE 授予 Cooldown Tag（GrantedTagsAdded）

5 秒后 → GE Duration 到期
  → ASC 自动 RemoveActiveGameplayEffect()
  → GE 移除时，其 GrantedTags 自动从 ASC 撤销
  → Cooldown Tag 消失 → CheckCooldown() 通过 → 可以再放
```

### 20.3 为什么冷却用 Duration GE 而不是 Instant

| GE 策略 | 施加 | 移除 | 冷却结果 |
|---------|------|------|----------|
| Instant | 瞬间加 Tag | 瞬间移除 | ❌ 冷却 0 秒，等于没冷却 |
| Duration | 加 Tag | N 秒后**自动移除** | ✅ 冷却时间内 Tag 持续存在 |
| Infinite | 加 Tag | **永久不自动移除** | ❌ 冷却永不结束，需手动 Remove |

Duration 完美匹配"挂 Tag N 秒 → 自动消失"的需求。GE 的 Duration 值 = 冷却秒数。

### 20.4 冷却时长来源

在蓝图/C++ 里设的 `CooldownDuration` 不是直接写死到 GE 里，而是通过 `GetCooldownTime()` 运行时覆盖：

```cpp
float UGameplayAbility::GetCooldownTime(float InLevel) const
{
    return CooldownDuration.GetValueAtLevel(InLevel);  // 蓝图设的冷却时长
}

// ApplyCooldown 内部：
float Duration = GetCooldownTime(Level);
GEHandle = ApplyGameplayEffectToOwner(CooldownGE, Level);
SetGameplayEffectDuration(GEHandle, Duration);  // 用这个值覆盖 GE 的 Duration
```

### 20.5 GE 五大 Tag 容器各司其职

| 容器 | 作用对象 | 施加时 | 移除时 | 典型用途 |
|------|---------|--------|--------|---------|
| **GrantedTagsAdded** | 目标 ASC | 授予 Tag 到目标 OwnedTags | — | ✅ **冷却**、Buff 标记、状态标签 |
| **GrantedTagsRemoved** | 目标 ASC | — | 撤销 OwnedTags 中对应 Tag | 显式声明"该带走什么" |
| **AssetTags** | GE 自身 | 给 GE 贴身份标签 | — | 让其他 GE 的 RemoveEffectsWithTags 能匹配到它 |
| **ApplicationTagReq** | 目标 ASC Tag | **前置条件**：必须有/必须没有指定 Tag | — | "必须先中毒才能点燃" |
| **OngoingTagReq** | 目标 ASC Tag | **维持条件**：不满足就暂停效果 | 条件恢复后自动恢复 | "必须离地才有飞行加速" |
| **RemoveEffectsWithTags** | 目标身上匹配的 GE | **先清场**：移除 AssetTags 匹配的所有活跃 GE | — | 解药移除所有毒系 Debuff、同类 Buff 替换 |

**冷却 Tag 必须放 GrantedTagsAdded**——因为 `CheckCooldown()` 查的是 ASC 的 OwnedTags，只有 GrantTags 才能把 Tag 挂到 ASC 身上。

### 20.6 各容器的实际用法示例

**AssetTags — 解药移除毒 Debuff**：
```
毒 Debuff GE: AssetTags = Debuff.Poison
解药 GE: RemoveGameplayEffectsWithTags = Debuff.Poison
  → 施加解药 → 身上所有 AssetTags 匹配 Debuff.Poison 的 GE 全部移除
```

**ApplicationTagRequirements — 条件施法**：
```
点燃 GE: ApplicationTagRequirements.Required = Debuff.Poison
  → 目标没中毒 → 点燃施不上
```

**OngoingTagRequirements — 条件维持**：
```
飞行 Buff GE: OngoingTagRequirements.Required = Status.Airborne
  → 落地 → 效果暂停 → 跳起来 → 恢复（Duration 照走）
```

**RemoveGameplayEffectsWithTags — 同类 Buff 替换**：
```
加速 30% GE: RemoveGameplayEffectsWithTags = Buff.Speed
  → 阻止叠加，30% 替换 20%
```

### 20.7 SpawnActor + EndAbility 时序问题

**问题场景**（火球投射物能力）：

```
服务器：
  ActivateAbility → SpawnActor(火球) → EndAbility（同一帧）
  → 火球 Actor 网络初始化还没完成，EndAbility 就结束了
客户端：
  → 火球初始状态还没复制过来 → 看不到或瞬移
```

**根本原因**：`SpawnActor` 创建的火球需要至少一帧才能完成首次 Actor Channel 复制。同一帧内 `EndAbility`，火球还在网络队列里。

**解决方案**：

| 方案 | 做法 | 适用 |
|------|------|------|
| Delay | SpawnActor 后 Delay 0.1s → EndAbility | 简单，硬编码时间 |
| WaitNetSync | SpawnActor 后 → WaitNetSync → EndAbility | 更可靠，等网络同步确认 |
| LocalPredicted | ReplicationPolicy=ReplicateYes + NetExecutionPolicy=LocalPredicted | Ability 生命周期接管，不需 Delay |

### 20.8 ReplicationPolicy 与 NetExecutionPolicy 的互斥

| ReplicationPolicy | NetExecutionPolicy | 实际行为 |
|-------------------|-------------------|---------|
| **ReplicateYes** | LocalPredicted | ✅ 客户端有 AbilitySpec → 本地预测激活 |
| **DoNotReplicate** | LocalPredicted | ⚠️ 客户端没有 AbilitySpec → `LocalPredicted` 前提不成立 → 实际 = **ServerOnly** |
| **ReplicateYes** | ServerOnly | 客户端有 Spec 但不激活 → 只有服务器执行 |
| **DoNotReplicate** | ServerOnly | ✅ 一致：客户端无 Spec，纯服务器执行 |

**关键**：`ReplicationPolicy` 决定客户端有没有 AbilitySpec；`NetExecutionPolicy` 决定有 Spec 后谁执行。
`DoNotReplicate` + `LocalPredicted` 同时开 → DoNotReplicate 胜出，LocalPredicted 是摆设。

### 20.9 纯服务器生成 + 复制的投射物

伤害不是客户端火球算的——客户端火球只是"视觉替身"：

```
服务器火球（真）          客户端火球（复制品/视觉替身）
  ├─ 飞行                   ├─ 飞行（同步来的位置）
  ├─ 碰撞 → 触发伤害        ├─ 碰撞（本地检测，仅播放特效）
  ├─ Apply GE → 算伤害       ├─ 不 Apply GE（被服务器拒绝）
  └─ 修改 AttributeSet       └─ AttributeSet 靠网络同步更新
```

伤害流程：服务器算 → AttributeSet 自动复制 → 客户端血条更新。客户端火球只管看，不管算。

### 20.10 完整配置速查

**纯服务器投射物（当前方案）**：
```
ReplicationPolicy: DoNotReplicate
NetExecutionPolicy: ServerOnly
结果：客户端不激活，火球靠 Actor Replication，需要 Delay
```

**LocalPredicted 攻击（后续可选）**：
```
ReplicationPolicy: ReplicateYes
NetExecutionPolicy: LocalPredicted
结果：客户端预测激活，不需要 Delay，但 SpawnActor 要加 HasAuthority 判断
```

---

## 二十一、AbilityTask 体系与 AsyncTask 深入

> 2026-06-30，任务系统架构、WaitCooldownChange 完整实现、委托绑定策略

### 21.1 AbilityTask 静态工厂方法模式

所有 AbilityTask 都遵循固定模板：

```cpp
UCLASS()
class UMyTask : public UAbilityTask
{
public:
    // ① static 工厂函数 — 不依赖实例就能调用
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UMyTask* CreateTask(UGameplayAbility* OwningAbility, float ExtraParam);
    
    // ② 委托 = 蓝图输出执行引脚
    UPROPERTY(BlueprintAssignable)
    FDelegate OnSuccess;
    
private:
    float MyExtraParam;  // ③ 参数存成员变量
    virtual void Activate() override;  // ④ ASC 创建 Task 后自动调用
};
```

**为什么必须是 static**：蓝图节点执行时 Task 实例还不存在。`static` 让 UE 蓝图 VM 用类名直接调函数，不依赖实例。函数内部 `NewAbilityTask<T>()` 才创建实例。

```
编译时 → 蓝图图表存 "调哪个 static 函数" 的签名
运行时 → 蓝图 VM 用类名::static函数() → NewObject → 返回实例 → Activate()
```

### 21.2 UFUNCTION meta 四个标记

```cpp
UFUNCTION(BlueprintCallable, meta = (
    DisplayName = "MyTargetDateUnderMouse",    // 蓝图节点显示的名字
    HidePin = "OwningAbility",                 // 隐藏 OwningAbility 引脚
    DefaultToSelf = "OwningAbility",           // 隐藏的引脚自动填 "当前 Ability"
    BlueprintInternalUseOnly = "true"           // 不在蓝图右键菜单出现
))
static UMyTask* CreateTask(UGameplayAbility* OwningAbility);
```

`BlueprintInternalUseOnly` **不是**"运行时使用"的意思——它只是阻止用户在蓝图右键菜单里搜到并手动拖出这个工厂函数。蓝图节点本身是 UE 的 `UK2Node_LatentGameplayTaskCall` 解析 Task 类后用反射找到 factory 函数来间接调用的。

### 21.3 UE 如何识别工厂函数

**不靠函数名，靠返回值类型**：

```
扫描 UAbilityTask 子类 → 找 UFUNCTION 满足：
  ① 是 static
  ② 返回值类型 = 这个类自己的指针
  ③ 标记了 BlueprintCallable + BlueprintInternalUseOnly
→ 找到了 = 工厂函数
→ 其他参数自动暴露为蓝图节点输入引脚
```

函数名叫 `CreateTask`、`MakeInstance`、`Factory` 都行，UE 不读名字。

### 21.4 TargetDataUnderMouse 完整流程

```cpp
// Activate — Task 创建后自动调用，分流客户端/服务器
void UMy_TargetDateUnderMouse::Activate()
{
    if (IsLocallyControlled())  // 本地 PC
    {
        SendMouseCursurData();  // 拿鼠标位置 → 发给服务器 → 本地预览
    }
    else  // 服务器
    {
        // 注册回调 → 等客户端 RPC 发 TargetData 过来
        ASC->AbilityTargetDataSetDelegate(Handle, Key).AddUObject(this, &OnTargetDataReplicatedCallback);
        // 处理竞态：TargetData 可能在注册回调前已经到了
        bool bCalled = ASC->CallReplicatedTargetDataDelegatesIfSet(Handle, Key);
        if (!bCalled) SetWaitingOnRemotePlayerData();
    }
}
```

```
完整数据流：
  客户端：GetHitResultUnderCursor → ServerSetReplicatedTargetData(RPC) → 服务器
           同时本地也 Broadcast ValidData → 蓝图预览特效
  服务器：OnTargetDataReplicatedCallback → ConsumeClientReplicatedTargetData
            → Broadcast ValidData → 蓝图做权威逻辑（Spawn 火球/算伤害）
```

**双向 RPC**：不是只有 S→C。客户端有输入数据（鼠标），服务器有权威数据（属性/Actor），两者各走各的 RPC。

### 21.5 AbilityTask vs BlueprintAsyncActionBase

| | AbilityTask | AsyncTask (BlueprintAsyncActionBase) |
|--|-------------|--------------------------------------|
| 父类 | `UAbilityTask` | `UBlueprintAsyncActionBase` |
| 工厂宏 | `NewAbilityTask<T>(OwningAbility)` | `NewObject<T>()` 手动创建 |
| 生命周期 | 跟 Ability 实例绑定 | 独立，手动 `EndTask` |
| Ability 结束后 | 自动取消 | 继续活着 |
| 适用 | Ability 内部流程（PlayMontage、WaitEvent） | **跨 Ability 的全局监听**（冷却 UI） |

冷却 UI 必须用 AsyncTask——Ability 结束后冷却还在走，AbilityTask 会跟着 Ability 一起销毁，没法继续监听 Tag 变化。

### 21.6 WaitCooldownChange 完整实现

```cpp
UMy_WaitCoolDownChange* CreateWaitCoolDownChange(ASC, CooldownTag)
{
    // ① 监听 Tag 增/删（冷却结束用）
    ASC->RegisterGameplayTagEvent(CooldownTag, NewOrRemoved)
        .AddUObject(this, &CooldownTagChanged);
    
    // ② 监听 Duration GE 挂上（冷却开始用，需要拿剩余时间）
    ASC->OnActiveGameplayEffectAddedDelegateToSelf
        .AddUObject(this, &OnActiveEffectAdded);
}

// Tag 消失 → 冷却结束
void CooldownTagChanged(Tag, NewCount)
{
    if (NewCount == 0) CooldownEnd.Broadcast(0.f);
}

// Duration GE 上身后 → 获取剩余时间 → 冷却开始
void OnActiveEffectAdded(TargetASC, Spec, Handle)
{
    if (AssetTags 或 GrantedTags 匹配 CooldownTag)
    {
        float Time = ASC->GetActiveEffectsTimeRemaining(Query);
        CooldownStart.Broadcast(Time);
    }
}
```

**为什么冷却开始用 OnActiveEffectAdded 而不是 CooldownTagChanged**：
`CooldownTagChanged` 参数只有 `(GameplayTag, NewCount)`，能告诉你"Tag 出现了"，但说不出冷却还剩几秒。冷却秒数存在 GE 的 Duration 里，只有 `OnActiveEffectAdded` 能拿到 GE Handle → 查出剩余时间。

### 21.7 ASC 五个 GE 生命周期委托

| 委托 | 触发时机 | 哪端 | GE 类型 |
|------|---------|:--:|--------|
| `AppliedDelegateToSelf` | GE 施加到自身 | 仅 S | Instant + Duration |
| `AppliedDelegateToTarget` | GE 施加到他人 | 仅 S | Instant + Duration |
| **`ActiveEffectAddedToSelf`** | Duration GE 挂上 | **S + C** | 仅 Duration |
| `PeriodicExecuteOnSelf` | 周期性 GE 定时执行 | 仅 S | 仅 Periodic |
| `PeriodicExecuteOnTarget` | 周期性 GE 定时执行 | 仅 S | 仅 Periodic |

五个里面只有 `ActiveEffectAddedToSelf` **客户端也触发**，所以冷却 UI 系统用它。

### 21.8 AddUObject vs AddLambda

| | AddUObject | AddLambda |
|--|-----------|-----------|
| 安全机制 | 弱引用，对象销毁后**自动解绑** | 无保护，Lambda 裸捕获 |
| 对象销毁后广播 | 不回调，安全 ✅ | 野指针 → 💥 崩溃 |
| 适用 | 被监听者活得比监听者久 | 两者同寿或监听者更长命 |
| 例子 | `ASC->RegisterTagEvent.AddUObject(this, &Callback)` | `ASC->GetAttrChangeDelegate(HP).AddLambda([this]{...})` |

**结论**：跨对象的监听用 `AddUObject`（安全网），自己的组件内部监听用 `AddLambda`（省事）。

### 21.9 EndTask — 为什么必须手动解绑

委托是 **ASC → Task 的单向指针**。ASC 活到游戏结束，Task 可能早被销毁。不主动斩断连线 → 野指针 → Tag 变化时崩溃。

```cpp
void EndTask()
{
    ASC->RegisterGameplayTagEvent(Tag).RemoveAll(this);           // 解绑 ①
    ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this); // 解绑 ②
    SetReadyToDestroy();
    MarkAsGarbage();
}
```

蓝图 Widget 的 Destruct 事件里调 `EndTask` → 解绑 → 安全销毁。**两个委托都要解绑**，漏一个就是野指针。

---

## 二十二、编辑器与工具技巧

> 2026-06-30

### 22.1 Rider 不索引 UE 引擎文件

**问题**：Rider 能打开引擎 `.cpp` 文件但无语法高亮、无 F12 跳转。

**原因**：Rider 默认只索引项目源文件，引擎文件是"项目外文件"当纯文本打开。

**解决**：删 `.idea` 和 `.vs` 目录 → 用 Rider 重新打开 `.uproject` → 等索引进度条跑完。如果引擎源码物理不存在（Epic Launcher 没勾选"编辑器符号用于调试"），则要先下载。

### 22.2 Live Coding 的边界

| 能 Hot Reload | 不能（必须重启编辑器） |
|--------------|---------------------|
| 改 .cpp 函数体 | 改 .h（增删 UPROPERTY/UFUNCTION） |
| 新增未被引用的属性 | 修改类继承层级 |
| 加注释/日志 | 改模块初始化相关代码 |

**典型崩溃**：改 .h 后用 Live Coding → `DEFINE_LOG_CATEGORY` 被注册两次 → Fatal error。解决：关编辑器 → `rm -rf Intermediate Binaries` → Rider Build → 重开。

### 22.3 蓝图窗口拖出主界面

蓝图被拖成独立浮动窗口后：UE 顶部 → **Window** → **Load Layout** → **Default Editor Layout** 一键恢复。

### 22.4 GBK 文件被 Edit 破坏后的恢复

Edit/Write 改含中文的 `.h/.cpp` 会把 GBK → UTF-8 → 中文乱码。恢复方法：

1. 用 Write 工具创建 Python 脚本文件（UTF-8 脚本可含中文）
2. 脚本里 `open(target, 'w', encoding='gbk')` 完整重建
3. 验证：`open(target, 'r', encoding='gbk')` 无异常


## 二十三、Attribute 越界防护与异步任务 UI 集成

> 2026-07-01，冷却显示实现、Mana 越界修复、AsyncTask 元数据

### 23.1 Mana 可能超过 MaxMana 的三个原因

1. **初始化时序**：`InitVitalAttributeEffectClass`（Instant）设 Mana = MaxMana 时，`SecondaryAttributes`（Infinite GE）计算的 MaxMana 可能还未生效，导致 Mana 被设成错误值
2. **MaxMana 变化时 Mana 不联动**：`PreAttributeChange` 只在 Mana 自己变化时 clamp——MaxMana 降低了，Mana 没人管
3. **网络复制乱序**：`OnRep_Mana` 和 `OnRep_MaxMana` 可能不同帧到达—Mana 先到，MaxMana 后到，中间 Mana > MaxMana

### 23.2 三层防护（My_AuraAttributeSet.cpp）

```cpp
// 层① PreAttributeChange — MaxMana/MaxHealth 下降时主动压 Mana/Health
if (Attribute == GetMaxManaAttribute())
    SetMana(FMath::Min(GetMana(), NewValue));       // 只降不升
if (Attribute == GetMaxHealthAttribute())
    SetHealth(FMath::Min(GetHealth(), NewValue));

// 层② PostGameplayEffectExecute — GE 执行后兜底 clamp（本来就有的）
if (Attribute == GetManaAttribute())
    SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));

// 层③ OnRep — 网络复制到达时兜底
void OnRep_Mana(...) {
    GAMEPLAYATTRIBUTE_REPNOTIFY(...);
    const_cast<...>(this)->SetMana(FMath::Min(GetMana(), GetMaxMana()));
}
```

`FMath::Min` 只降不升——如果 MaxMana 变大（升级），不会错误地把 Mana 拉高。

注意 `OnRep` 是 const 函数，需要 `const_cast` 去掉 const 才能调 `SetMana`。

### 23.3 蓝条 + 数字显示

蓝条百分比变化在消耗少时不明显（20/430 ≈ 4.6%）。标准做法是蓝条旁加文字 `{Mana} / {MaxMana}`，蓝图里绑 `OnManaChanged` + `OnMaxManaChanged` 更新 TextBlock。

### 23.4 UCLASS meta 回顾

```cpp
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class UMy_WaitCoolDownChange : public UBlueprintAsyncActionBase
```

- `BlueprintType` — 这个类能当蓝图变量类型
- `ExposedAsyncProxy` — 告诉蓝图 VM：工厂函数画成**延迟节点**，`BlueprintAssignable` 委托自动变成输出执行引脚
- `= "AsyncTask"` — 输出引脚的名字，返回 AsyncTask 实例引用（可 Promote to Variable 然后调 `EndTask()`）

### 23.5 CooldownTag 加入 AbilityInfo

`FMy_AuraAbilityInfo` 新增 `CoolDownTag`，每个技能配自己的冷却 Tag。蓝图 `SpellGlobal` 根据 AbilityInfo 拿到 CooldownTag，传给 `WaitCoolDownChange` 工厂函数，实现技能冷却图标的动态显示。

---

## 二十四、属性复制完整链路：OnRep → REPNOTIFY → Delegate → UI

> 2026-07-03，属性如何从服务端变化到客户端 UI 刷新的每一步

### 24.1 完整流程（以服务端 GE 扣血 30 为例）

```
服务端：
① GE 执行，Modifier 作用于 Health → CurrentValue 100→70
② PreAttributeChange(Health, NewValue=70) → Clamp(0, MaxHealth)
③ 值写入成员变量 Health.BaseValue=70, Health.CurrentValue=70
④ PostAttributeChange(Health, OldValue=100, NewValue=70)  ← 仅服务端，适合日志
⑤ PostGameplayEffectExecute(Data)  ← 仅服务端，伤害/死亡逻辑
⑥ UE 网络层检测 Health 变化 → 序列化 → 发给所有客户端

客户端收到网络包：
⑦ UE 反序列化，先拷贝旧值：OldHealth = 当前客户端 Health(如 100)
⑧ UE 把新值直接写入成员变量：Health.BaseValue=70, Health.CurrentValue=70
⑨ UE 检测到 ReplicatedUsing=OnRep_Health → 调用 OnRep_Health(OldHealth=100)
⑩ OnRep_Health 内部：
    a. GAMEPLAYATTRIBUTE_REPNOTIFY(Health, OldHealth)
       → SetBaseAttributeValueFromReplication(Health属性, 当前值70, 旧参数100)
       → 聚合器：旧 BaseValue=100，新 BaseValue=70 → 不同 → 广播 Delegate
    b. const_cast + SetHealth + FMath::Min  ← 兜底夹
⑪ WidgetController 回调触发 → OnHealthChanged.Broadcast(70)
⑫ UI Widget 收到 → 血条刷新
```

### 24.2 没有 OnRep 会怎样

UE 网络复制**仍然会把新值写进成员变量**（步骤⑦⑧照常执行），`GetHealth()` 确实返回新值。但：

| | 没有 OnRep | 有 OnRep |
|---|---|---|
| `Health` 成员变量 | ✓ 更新了 | ✓ 更新了 |
| `GetHealth()` 返回值 | ✓ 新值 | ✓ 新值 |
| ASC 聚合器重算 | ✗ 没参与 | ✓ 重算 |
| `GetGameplayAttributeValueChangeDelegate` | ✗ 不触发 | ✓ 触发 |

**委托的触发不是由"成员变量被写了"驱动的，而是由"聚合器重算后发现值变了"驱动的。** 没有 OnRep，聚合器根本没参与，委托链是断的。

### 24.3 SetBaseAttributeValueFromReplication 内部

`GAMEPLAYATTRIBUTE_REPNOTIFY` 不是委托，它是个宏，展开后调 ASC 的 `SetBaseAttributeValueFromReplication()`：

```cpp
#define GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, PropertyName, OldValue) \
{ \
    static FProperty* ThisProperty = FindFieldChecked<FProperty>(...); \
    GetOwningAbilitySystemComponentChecked()->SetBaseAttributeValueFromReplication( \
        FGameplayAttribute(ThisProperty), PropertyName, OldValue); \
}
```

`SetBaseAttributeValueFromReplication()` 内部做了三件事：

```
① 找到 Health 属性的聚合器（Aggregator）
② 更新聚合器 BaseValue = 新值
③ 重新计算 CurrentValue = BaseValue + Σ(所有挂着的 GE modifier)
④ 如果 CurrentValue 真的变了 → 广播 GetGameplayAttributeValueChangeDelegate
```

**这才是真正的委托**——`GetGameplayAttributeValueChangeDelegate`。WidgetController 通过 `AddLambda` 注册回调到它上面。

### 24.4 OldHealth 参数哪来的

OnRep 执行时，成员变量已被覆盖为新值。那对比用的是谁的旧值？

**UE 在写新值之前，先拷贝了一份旧值**：

```
客户端收到网络包:
  1. 拷贝当前 Health → 存为 OldHealth 副本
  2. 把网络数据写入 Health 成员变量（覆盖）
  3. 调用 OnRep_Health(OldHealth)  ← 传入步骤1的拷贝
```

所以 `OldHealth` 和当前 `GetHealth()` 是不同的——一个是覆盖前的值，一个是覆盖后的值。聚合器内部对比的是 `OldHealth(旧)` vs `新值`，不是从成员变量再读一次。

### 24.5 REPNOTIFY_Always 为什么安全

```cpp
DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
```

`REPNOTIFY_Always` 强制每次复制都调 OnRep（即使值没变），好处是切换角色/加载存档/重新同步 ASC 时强制走一遍通知链。

**但 OnRep 调了不代表委托一定广播**——聚合器内部用 OldHealth 对比新值：

```
服务端再次同步 Health=30，客户端已经是 30:
  → 拷贝旧值: OldHealth = 30
  → 写入新值: Health = 30
  → OnRep(OldHealth=30)
  → SetBaseAttributeValueFromReplication(新=30, 旧=30)
  → 聚合器: 30 vs 30 → 相同 → 不广播 ✓
```

三层防护：

| 层级 | 机制 | 作用 |
|------|------|------|
| UE 网络层 | `REPNOTIFY_Always` | 强制每次调 OnRep（防切换角色时 UI 残留旧值） |
| ASC 聚合器 | 新旧值对比 | 值没变就不更新、不广播（防无意义 UI 刷新） |
| 业务层 | `const_cast` + `SetHealth` + `FMath::Min` | 兜底保证血量不越界 |

### 24.6 OnRep 是 const 函数

`OnRep_Health` 被声明为 `const`，因为 UE 的复制回调不期望你修改对象状态。但如果需要兜底 clamp（如 `SetHealth(FMath::Min(...))`），必须 `const_cast` 去掉 const：

```cpp
void UMy_AuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Health, OldHealth);
    const_cast<UMy_AuraAttributeSet*>(this)->SetHealth(FMath::Min(GetHealth(), GetMaxHealth()));
}
```

### 24.7 项目中绑定委托的位置

**玩家 Overlay**（`My_OverlayWidgetController.cpp:27`）：
```cpp
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHealthAttribute())
    .AddLambda([this](const FOnAttributeChangeData& Data) {
        OnHealthChanged.Broadcast(Data.NewValue);
    });
```

**敌人**（`Enemy_Characte.cpp:123`）：
```cpp
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
    .AddLambda([this](const FOnAttributeChangeData& Data) {
        OnHealthChanged.Broadcast(Data.NewValue);
    });
```

**属性菜单**（`My_AttributeMenuWidgetController.cpp:28`）：
```cpp
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value())
    .AddLambda([this, Pair](const FOnAttributeChangeData& Data) {
        BroadAttributeInfo(Pair.Key, Pair.Value());
    });
```

三者模式一致：`GetGameplayAttributeValueChangeDelegate` → `AddLambda` → 翻译为自定义蓝图委托 → UI 刷新。

### 24.8 属性变化的四种触发源

| 触发源 | 走 PreAttributeChange | 走 PostGameplayEffectExecute | 触发 Delegate |
|--------|:---:|:---:|:---:|
| GE 修改（服务端） | ✅ | ✅ | ✅ |
| `SetHealth()` 直接调用（服务端） | ✅ | ❌ (不走 GE) | ✅ |
| 网络复制到达（客户端） | ❌ (不走) | ❌ (不走 GE) | ✅ (OnRep→REPNOTIFY) |
| 纯单机本地修改 | ✅ | ❌ | ✅ |

---

## 二十五、PlayerState XP/Level 委托 → WidgetController → Widget 完整链路

> 2026-07-27，XP 不走 AttributeSet——走 PlayerState 的 C++ 多播委托 + LevelUpInfo DataAsset 翻译

### 25.1 XP 和 Health/Mana 的数据流差异

Health/Mana 走 AttributeSet + ASC 聚合器 + `GetGameplayAttributeValueChangeDelegate`。XP 走的是**完全不同的路**：

| | Health/Mana | XP/Level |
|---|---|---|
| 数据存在 | `UMy_AuraAttributeSet`（FGameplayAttributeData） | `AMy_AuraPlayerState`（int32 成员变量） |
| 触发源 | GE 修改 / ASC 聚合器 | `SetXP()` / `AddXP()` / `OnRep_XP()` |
| 委托类型 | `GetGameplayAttributeValueChangeDelegate`（系统） | `FMy_OnPlayerStateChanged`（自定义 C++ 多播） |
| 翻译层 | `AddLambda` 取 `Data.NewValue` 直接转发 | `OnXPChangedFunc` 中 `LevelUpInfo->FindLevelForXP()` 把原始 XP 转为进度条百分比 |

XP 不在 AttributeSet 里——它是 PlayerState 上的 plain `int32`，网络复制靠 `DOREPLIFETIME` + `OnRep`，和 GAS 的 Attribute 复制是两套独立机制。

### 25.2 PlayerState 端：委托触发点

```cpp
// 三个触发点都会广播 OnXPChanged：
void SetXP(int32 number)   { XP = number; OnXPChanged.Broadcast(XP); }     // 服务端直接设
void AddXP(int32 number)   { XP += number; OnXPChanged.Broadcast(XP); }    // 服务端累加
void OnRep_XP(int32)       { OnXPChanged.Broadcast(XP); }                   // 客户端收到复制
```

`OnLevelChanged` 同理，`SetLevel` / `AddToLevel` / `OnRep_Level` 三个入口。

### 25.3 WidgetController 端：BindCallbacksToDependencies 中的绑定

```cpp
AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
// OnXPChanged — 逻辑较长（算百分比），写成成员函数 + AddUObject
MyPS->OnXPChanged.AddUObject(this, &UMy_OverlayWidgetController::OnXPChangedFunc);
// OnLevelChanged — 只需转发，用 AddLambda
MyPS->OnLevelChanged.AddLambda([this](int32 NewLevel) {
    OnPlayerLevelChanged.Broadcast(NewLevel);
});
```

### 25.4 OnXPChangedFunc — 为什么需要翻译

PlayerState 广播的是**原始累积 XP**（如 500），但 UI 只认 0~1 的百分比。必须通过 `LevelUpInfo` DataAsset 翻译：

```cpp
void UMy_OverlayWidgetController::OnXPChangedFunc(int32 NewXP)
{
    const int32 Level = MyPS->LevelUpInfo->FindLevelForXP(NewXP);  // 当前处于哪个等级段
    if (Level <= MaxLevel && Level > 0)
    {
        const int32 LevelUpReq = LevelUpInformation[Level].LevelUpRequirement;       // 下一级门槛
        const int32 PrevLevelUpReq = LevelUpInformation[Level - 1].LevelUpRequirement; // 当前级门槛
        const float XPBarPercent = static_cast<float>(NewXP - PrevLevelUpReq)
                                 / static_cast<float>(LevelUpReq - PrevLevelUpReq);
        OnXPPercentChanged.Broadcast(XPBarPercent);  // 进度条百分比 (0~1)
    }
}
```

**为什么 Level 不用翻译**：等级本身就是 UI 要显示的值，3 就是 3，不需要转换。

### 25.5 FindLevelForXP 算法

```cpp
int32 UMy_LevelUpInfo::FindLevelForXP(int32 XP) const
{
    for (int i = 1; i < LevelUpInformation.Num(); i++)
    {
        if (XP < LevelUpInformation[i].LevelUpRequirement)  // 没到下一级门槛
            return i;  // 返回当前级别
    }
    return LevelUpInformation.Num();  // 超过最高级，返回 MaxLevel
}
```

```
LevelUpInformation 数组:
  [0] Level 1 起点 (0 XP)
  [1] Level 2 起点 (300 XP)
  [2] Level 3 起点 (700 XP)

FindLevelForXP(500) → i=1: 500<300? 否 → i=2: 500<700? 是 → return 2
  → 意思是"正在朝 Level 2 前进"
  → LevelUpInformation[2] = 700 (下一级), LevelUpInformation[1] = 300 (当前级)
  → 进度 = (500-300) / (700-300) = 0.5
```

### 25.6 完整数据流

```
捡药水
  → GE 写入 IncomingXP（Meta Attribute）
    → ExecCalc 读取 IncomingXP
      → PlayerState->AddXP(NewXP)
        ├─ XP += NewXP
        └─ OnXPChanged.Broadcast(XP)          ← C++ 多播委托
            │
            ▼  (客户端：OnRep_XP → OnXPChanged.Broadcast)
            │
    ┌───────────────────────────────────────┐
    │  WidgetController 翻译层               │
    │  OnXPChangedFunc(NewXP)               │
    │    → LevelUpInfo->FindLevelForXP()    │
    │    → 算出百分比 0.0~1.0               │
    │    → OnXPPercentChanged.Broadcast(%)  │ ← 自定义蓝图委托
    └───────────────────────────────────────┘
            │
            ▼
    UI Widget 绑定 OnXPPercentChanged → 进度条 SetPercent()
    UI Widget 绑定 OnPlayerLevelChanged → 等级文本 SetText()
```

### 25.7 LevelUpInfo 放在 PlayerState 上的原因

```cpp
// AMy_AuraPlayerState.h
UPROPERTY(EditDefaultsOnly)
TObjectPtr<UMy_LevelUpInfo> LevelUpInfo;
```

放在 PlayerState 而不是 WidgetController 上——**多个 WidgetController（Overlay、AttributeMenu、SpellMenu 等）都能通过 `Cast<AMy_AuraPlayerState>(PlayerState)->LevelUpInfo` 共享同一份等级表**，不用各自存一份。

### 25.8 AddUObject vs AddLambda 选择原则

| 委托绑定 | 用哪个 | 原因 |
|----------|--------|------|
| `OnXPChanged` | `AddUObject` | 逻辑 > 3 行，写成独立成员函数 `OnXPChangedFunc` |
| `OnLevelChanged` | `AddLambda` | 只有一行转发，不值得单独声明函数 |

`AddUObject` 有生命周期保护（UObject 销毁 → 自动解绑），`AddLambda` 没有。但这里 WidgetController 绑在 PlayerState 的委托上，PlayerState 一定比 WidgetController 活得久，所以 `AddLambda` 也安全。

### 25.9 BroadcastInitiaValues 中 XP 初始值

当前 `BroadcastInitiaValues` 不广播 XP——XP 只在 `OnXPChanged` 触发时更新。如果需要开局就显示 XP 进度条，在 `BroadcastInitiaValues` 中加：

```cpp
AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
OnXPChangedFunc(MyPS->GetXP());
OnPlayerLevelChanged.Broadcast(MyPS->GetPlayerLevel());
```

### 25.10 两个 OnLevelChanged 不要搞混

| 委托 | 所属 | 方向 |
|------|------|------|
| `AMy_AuraPlayerState::OnLevelChanged` | PlayerState | M → C（PlayerState 通知 WidgetController） |
| `UMy_OverlayWidgetController::OnPlayerLevelChanged` | WidgetController | C → W（WidgetController 通知 Widget） |

WidgetController 不直接暴露 PlayerState 的委托给 Widget——它用自己的委托做翻译中转，保持上层 UI 和底层数据解耦。

### 25.11 OnRep_Level 网络复制完整流程（服务器推送 → 客户端 OnRep）

> 核心认知：`Level` 变量在服务器和每个客户端**各有一份**，是独立内存变量。复制 = 引擎把服务器那份的值**拷贝**到客户端那份。

```
服务器机器:   Level = 3   ← 权威值（真值）
客户端机器:   Level = 3   ← 拷贝值（可能是过期的）
```

#### 服务器端（权威，主动推送）

```cpp
// 升级逻辑最终调用 Aura_Character.cpp:111
AuraPlayerState->AddToLevel(1);   // 服务器当前 Level = 3

void AMy_AuraPlayerState::AddToLevel(int32 number)  // My_AuraPlayerState.cpp:64
{
    Level += number;                 // ① 服务器的 Level：3 → 4，被标记 dirty
    OnLevelChanged.Broadcast(Level); // ② 服务器 UI 立即更新（与网络无关）
}
```

- **①** `UPROPERTY(ReplicatedUsing = OnRep_Level)` 让引擎自动把属性标 dirty
- **②** 服务器自己立刻知道，直接 broadcast 给服务器端 UI

服务器**不会**等客户端问，也**不会**调 `OnRep_Level`。它每帧检查 dirty 属性，把 `Level=4` 打包成 bunch，通过 PlayerState 的 Actor Channel 推给所有客户端（`NetUpdateFrequency = 100` 是每秒最多推 100 次，只有 dirty 才发）。

#### 客户端（被通知）

```
收到 bunch
① 先把本地旧值 3 存临时变量            → 这就是 OldLevel
② 用新值 4 覆盖本地 Level              → 客户端 Level: 3 → 4
③ 有 OnRep → 调用 OnRep_Level(3)
```

```cpp
void AMy_AuraPlayerState::OnRep_Level(int32 OldLevel)  // My_AuraPlayerState.cpp:36
{
    OnLevelChanged.Broadcast(Level);  // 客户端 UI 收到通知，显示 4 级
}
```

#### 双入口模式总结

`AddToLevel` 和 `OnRep_Level` 里是**同一行 broadcast**，但跑在**不同机器**上：

| 入口 | 跑在哪台机器 | 触发时机 | 作用 |
|------|------|----------|------|
| `SetLevel` / `AddToLevel` 内 broadcast | 服务器 | 服务器改值时 | 更新服务器 UI |
| `OnRep_Level` 内 broadcast | 客户端 | 收到网络复制后 | 更新客户端 UI |

**`OnRep_Level` 不是服务器调的**——是引擎替客户端调的：每当新值从网络到达并覆盖本地值后，引擎自动喊 `OnRep_Level(旧值)`，让客户端有机会"对变化做出反应"。`OldLevel` 参数就是给你对比用的（如"从 3 变 4 = 升了一级"）。

#### 注意点

- **单机测试看不到 OnRep**：Standalone 下服务器=客户端，`AddToLevel` 直接广播就完事，复制不发生，`OnRep_Level` 永不调用。验证复制需 ≥2 玩家或监听服务器。
- **`DOREPLIFETIME` 默认 `REPNOTIFY_OnChanged`**：只有客户端当前值 ≠ 新值才调 OnRep，设成相同值不触发。AttributeSet 用的 `REPNOTIFY_Always` 是"值相同也触发"（`My_AuraAttributeSet.cpp:52`），两者语义不同。
- **Listen Server 主机**：既是服务器又是客户端，会收到自己的复制 → `AddToLevel` 和 `OnRep_Level` 都 broadcast，可能重复广播。单机学习项目无影响。
- **别在 OnRep 里写服务器逻辑**：它只在客户端跑，`HasAuthority()` 在此通常为 false。

---

## 二十六、敌人 XP 奖励 & Meta Attribute & Passive Ability 事件监听

### 26.1 敌人 XP 奖励表 — FScalableFloat + CurveTable

每个敌人类型（Elementalist / Warrior / Ranger）击杀后奖励的 XP 不同，且随敌人等级缩放：

```cpp
// My_CharacterClassInfo.h
USTRUCT(BlueprintType)
struct FMy_CharacterClassDefaultInfo
{
    // ...
    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    FScalableFloat XPReward = FScalableFloat();  // 常数 or 曲线
};
```

`FScalableFloat` 可以在编辑器中填入固定值（如 100），或绑定 CurveTable 曲线。运行时通过 `GetValueAtLevel(EnemyLevel)` 取值——等级越高 XP 越多。

```cpp
// My_AuraAbilitySystemLibrary::GetXPRewardForClassAndLevel()
int32 UMy_AuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(
    const UObject* WorldContextObject, EMy_CharacterClass CharacterType, int32 level)
{
    UMy_CharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
    FMy_CharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterType);
    float Xp = ClassDefaultInfo.XPReward.GetValueAtLevel(level);
    return static_cast<int32>(Xp);
}
```

### 26.2 CharacterClass 上移重构

`CharacterClass` 原来只在 `Enemy_Characte` 中定义，但玩家也需要（PassiveAbility 查询 XP 奖励时需要知道击杀的是哪种敌人）。重构到基类 `MyCharacter_Base`：

```
MyCharacter_Base (新位置)
  ├─ Aura_Character   → 构造中设为 Elementalist
  └─ Enemy_Characte   → 蓝图编辑器中配置
```

`CombatInterface` 新增 `GetCharacterClass()`，解耦 AttributeSet 对具体子类的依赖。

### 26.3 IncomingXP — Meta Attribute 桥梁模式

```
敌人死亡 → SendGameplayEvent("Attributes.Meta.IncomingXP", XP值)
  → Aura 的 PassiveAbility 监听到 Event
    → 创建 GE (SetByCaller 方式填入 XP 值)
      → GE 写入 Aura 的 IncomingXP (Meta Attribute)
        → PostGameplayEffectExecute 检测 IncomingXP 变化
          → 消费 IncomingXP → 转发到 PlayerState.AddXP()
```

`IncomingXP` 不是 Health/Mana 那样的真实属性——它是**一次性数据管道**：
- GE 只能修改 Attribute，无法直接传参给函数
- IncomingXP 作为临时跳板：GE 写入 → AS 回调拦截 → 读走清零 → 转发真实目标

```cpp
// My_AuraAttributeSet.h
FGameplayAttributeData IncomingXP;                       // Meta Attribute
ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, IncomingXP);

// My_AuraAttributeSet.cpp — PostGameplayEffectExecute
if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
{
    const float LocalIncomingXP = GetIncomingXP();
    SetIncomingXP(0.f);  // 读走清零，防止预测双重触发
    // → 转发给 PlayerState（当前为 TODO / WIP）
}
```

### 26.4 Passive Ability — 事件驱动的被动技能

Passive Ability 不需要玩家按键触发，而是在游戏启动时自动激活，持续监听事件：

```cpp
// MyCharacter_Base.h — 玩家专用
TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbility;

// My_AuraAbilitySystemComponent::AddCharacterPassiveAbilitiesFromASC()
void UMy_AuraAbilitySystemComponent::AddCharacterPassiveAbilitiesFromASC(
    const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbility)
{
    for (auto& Ability : StartupPassiveAbility)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
        GiveAbilityAndActivateOnce(AbilitySpec);  // 给予 + 立即激活一次
    }
}
```

`GiveAbilityAndActivateOnce` — 给 ASC 注册该 Ability 并立即激活。因为 Passive 不绑定输入，Activate 后会持续运行（如 `WaitGameplayEvent` 无限等待）。

### 26.5 GA_ListenForEvents 内部机制 — SetByCaller + WaitGameplayEvent

Passive GA 的核心蓝图逻辑：

```
GA_ListenForEvents (Passive, 持续运行)
  │
  ├─ WaitGameplayEvent(EventTag = "Attributes.Meta.IncomingXP")
  │     ↓ 收到 Event
  ├─ 获取 Payload.EventMagnitude (XP 值)
  ├─ 创建 GE_Spec (使用 GE_EventBasedEffect 类)
  ├─ AssignTagSetByCallerMagnitude(Spec, DataTag, Magnitude)
  │     ↓ DataTag = "My_Attribute.Meta.IncomingXP"
  │     ↓ Magnitude = Payload.EventMagnitude
  └─ ApplyGameplayEffectSpecToSelf(Spec)
        ↓
        GE 写入 IncomingXP (Meta Attribute)
```

关键点：
- **WaitGameplayEvent** — GAS 内置节点，Ability 挂起等待指定 Tag 的 GameplayEvent
- **EventTag 本身承担双重角色**：既是监听 Key（WaitGameplayEvent 筛选），又是 SetByCaller 的 DataTag（GE 知道修改哪个属性）
- **SetByCaller Magnitude** — GE 不预先写死数值，运行时通过 `AssignTagSetByCallerMagnitude(Spec, Tag, Value)` 动态注入
- **Payload.EventMagnitude** — `SendGameplayEventToActor` 传入的 XP 值，由击杀逻辑计算后携带

### 26.6 EventTag 的三重身份

同一个 Tag `My_Attribute.Meta.IncomingXP` 在流程中扮演三个角色：

| 阶段 | 角色 | 说明 |
|------|------|------|
| 敌人死亡 | GameplayEvent 的 EventTag | `SendGameplayEventToActor` 发送 |
| Passive GA | WaitGameplayEvent 的监听 Key | Ability 筛选要响应的事件 |
| GE Spec | SetByCaller 的 DataTag | GE 知道"我该修改哪个 Attribute" |

这得益于 GAS 的 SetByCaller 机制——GE 的 Modifier 设置 `ModifierMagnitude = SetByCaller`，运行时通过 DataTag 匹配：`AssignTagSetByCallerMagnitude(Spec, Tag, Value)` → GE 执行时用 Tag 查 Value → 写入同名 Attribute。

### 26.7 完整数据流总结

```
击杀敌人
  ├─ Enemy_Characte::CharacterClass → GetCharacterClass()
  ├─ My_AuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Class, Level)
  │     → ClassDefaultInfo.XPReward.GetValueAtLevel(Level)   ← CurveTable 查表
  └─ SendGameplayEventToActor(Aura, "My_Attribute.Meta.IncomingXP", Payload)
        │
        ▼
Aura 的 GA_ListenForEvents (已激活的 Passive Ability)
  ├─ WaitGameplayEvent("My_Attribute.Meta.IncomingXP")   ← 监听到
  ├─ 读取 Payload.EventMagnitude (XP 值)
  ├─ 创建 GE Spec
  ├─ Spec = AssignTagSetByCallerMagnitude("My_Attribute.Meta.IncomingXP", XP值)
  └─ ApplyGameplayEffectSpecToSelf(Spec)
        │
        ▼
My_AuraAttributeSet::PostGameplayEffectExecute
  ├─ 检测到 IncomingXP 被修改
  ├─ LocalXP = GetIncomingXP(); SetIncomingXP(0.f)   ← 消费清零
  └─ → PlayerState->AddXP(LocalXP)                   ← 转发到真实数据
        │
        ▼
PlayerState → WidgetController → UI 更新
```

### 26.8 敌人 vs 玩家初始化方式汇总

| | 敌人 (Enemy_Characte) | 玩家 (Aura_Character) |
|---|---|---|
| 入口 | `BeginPlay()` | `PossessedBy()` |
| 属性初始化 | `InitializeDefaultAttribute()` 重写版 → DataAsset | `InitializeDefaultAttribute()` 基类版 → GE |
| 能力来源 | `GiveStartupAbilities()` → DataAsset 按 CharacterClass 取 | `AddCharacterAbilities()` → 私有 TArray 成员 |
| Passive 能力 | 无 | `AddCharacterPassiveAbilitiesFromASC()` → `StartupPassiveAbility` 数组 |
| 为什么不同 | 敌人类型多，DataAsset 集中管理策划可配 | 只有玩家，直接硬编码数组更简洁 |

### 26.9 ⚠️ Meta Attribute 清空必须用 SetXxx(0.f) 而非 SetXxx(LocalValue)

```cpp
// ❌ 错误 — SetIncomingXP(LocalIncomingXP) 是 no-op
const float Local = GetIncomingXP();
SetIncomingXP(Local);

// ✅ 正确 — 消费后归零
const float Local = GetIncomingXP();
SetIncomingXP(0.f);
```

原因：Meta Attribute 被消费后必须归零，否则 GE 再次触发（如网络预测/重同步）会导致重复处理。

### 26.10 SendXPEvent — 敌人死亡时发送 XP GameplayEvent

```
敌人死亡（PostGameplayEffectExecute 检测到致命伤害）
  ├─ CombatInterface->Die()
  └─ SendXPEvent(Props)                    ← 新增
       ├─ 获取 TargetLevel（敌人等级）
       ├─ 获取 TargetClass（敌人职业）
       ├─ GetXPRewardForClassAndLevel()     ← CurveTable 查表得到原始 XP
       └─ SendGameplayEventToActor(SourceCharacter, "Meta.IncomingXP", XP)
            ↓
        玩家 Passive GA 监听到 → SetByCaller → GE → IncomingXP
```

```cpp
void UMy_AuraAttributeSet::SendXPEvent(const FMy_EffectProperties& Props) const
{
    if (IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(Props.TargetAvatarActor))
    {
        const int32 TargetLevel = CombatInterface->GetPlayerLevel();
        const EMy_CharacterClass TargetClass = IMy_CombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
        const int32 XP = UMy_AuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

        const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();
        FGameplayEventData EventData;
        EventData.EventTag = GameplayTags.My_Attribute_Meta_IncomingXP;
        EventData.EventMagnitude = XP;
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.My_Attribute_Meta_IncomingXP, EventData);
    }
}
```

关键点：
- `TargetCharacter` 是**敌人**（被杀者），`SourceCharacter` 是**玩家**（杀人者）
- `SendGameplayEventToActor` 发送给 `SourceCharacter`，这样玩家的 Passive GA 才能监听到
- `EventMagnitude` 携带 XP 值，Passive GA 通过 `Payload.EventMagnitude` 读取

### 26.11 ⚠️ 接口 BlueprintNativeEvent 必须用 Execute_ 调用

接口里的 `BlueprintNativeEvent` / `BlueprintImplementableEvent` 和普通类里的行为不同：

```cpp
// UHT 生成的接口函数体（.gen.cpp）
EMy_CharacterClass IMy_CombatInterface::GetCharacterClass()
{
    check(0 && "Do not directly call Event functions in Interfaces. Call Execute_GetCharacterClass instead.");
    // 直接调用 = 断言崩溃！
}
```

```cpp
// 正确的调用方式
IMy_CombatInterface::Execute_GetCharacterClass(Actor);  // ✅
```

`Execute_` 内部逻辑：先找蓝图重写（`FindFunction` + `ProcessEvent`），找不到才走 C++ `_Implementation`。

| 场景 | 直接调 | Execute_ |
|------|:--:|:--:|
| 接口 BlueprintNativeEvent | ❌ check(0) 崩 | ✅ |
| 普通类 BlueprintNativeEvent | ✅ | 不需要 |
| 普通 virtual（无 UFUNCTION） | ✅ | 没有 Execute_ |

规则：**接口里的 UFUNCTION 事件，无脑用 Execute_**。和 Cast 不 Cast 无关——即使 Cast 成接口指针也不会绕过 `check(0)`。

---

## 二十七、PlayerInterface 桥接：AttributeSet → Character → PlayerState

> 2026-08-06，XP 流程收尾——AttributeSet 通过接口间接操作 PlayerState，UI 完整显示

### 27.1 问题：AttributeSet 不能直接依赖 PlayerState

`HandleIncomingXP` 在 AttributeSet 中执行，需要把 XP 加到 PlayerState。但 AttributeSet 是通用属性层（玩家和敌人都用），直接 include PlayerState 会导致：

- **敌人也依赖 PlayerState**——编译能过但设计上低层模块反向依赖了高层
- **紧耦合**——将来换存储方式（如存档系统接管 XP）需要改 AttributeSet

### 27.2 解决方案：接口居中翻译

```
AttributeSet                PlayerInterface              Aura_Character            PlayerState
     │                            │                            │                        │
     │  Implements<IPlayer>()?    │                            │                        │
     ├───────────────────────────►│                            │                        │
     │  Execute_AddToXP(XP)       │                            │                        │
     │ ──────────────────────────►│  AddToXP_Implementation()  │                        │
     │                            │ ──────────────────────────►│  GetPlayerState<PS>()  │
     │                            │                            │ ──────────────────────►│
     │                            │                            │       PS->AddXP(XP)    │
```

### 27.3 接口定义

```cpp
// My_PlayerInterface.h
class AURA_API IMy_PlayerInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent)
    void AddToXP(int32 InXp);
};
```

### 27.4 Character 实现接口

```cpp
// Aura_Character.h — 新增接口继承
class AAura_Character : public AMyCharacter_Base, public IMy_PlayerInterface

// Aura_Character.cpp — 实现
void AAura_Character::AddToXP_Implementation(int32 InXp)
{
    AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
    check(AuraPlayerState);
    AuraPlayerState->AddXP(InXp);
}
```

`AAura_Character` 的职责：拿到 PlayerState 指针，转发 XP。自己不存数据。

### 27.5 AttributeSet 调用接口

```cpp
// My_AuraAttributeSet.cpp — HandleIncomingXP
if (Props.SourceCharacter->Implements<UMy_PlayerInterface>())
{
    IMy_PlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
}
```

`Implements<T>()` 运行时用 UHT 反射数据检测类的继承链中是否声明了该接口。敌人不实现 `IMy_PlayerInterface` → 返回 false → 整段跳过。全类型安全，不存在 nullptr 风险。

### 27.6 Implements vs Cast

| | `Cast<T>()` | `Implements<T>()` |
|---|---|---|
| 机制 | vtable 查找 | UHT 反射数据 |
| 需要 | T 的类定义（include） | T 的前向声明 |
| 耦合 | 紧（依赖具体类） | 松（只依赖接口头文件） |

AttributeSet 只需要 `#include "My_Interraction/My_PlayerInterface.h"`，不需要知道 `AAura_Character` 的存在。

### 27.7 LevelUpInfo 必须蓝图中赋值

```cpp
// My_AuraPlayerState.h
UPROPERTY(EditDefaultsOnly)
TObjectPtr<UMy_LevelUpInfo> LevelUpInfo;
```

`EditDefaultsOnly` 不会自动创建实例——必须在 PlayerState 蓝图的 Class Defaults 中手动选择 LevelUpInfo DataAsset。否则运行时 `LevelUpInfo == nullptr`，WidgetController 的 `OnXPChangedFunc` 中 `checkf(MyPS->LevelUpInfo)` 断言失败崩溃。

### 27.8 完整 XP 流程（最终版）

```
击杀敌人
  │
  └─ SendXPEvent(Props)                      ← AttributeSet 查 CurveTable 算 XP
       └─ SendGameplayEventToActor(玩家, "Meta.IncomingXP", XP值)
            │
            ▼
       玩家 Passive GA (GA_ListenForEvents)
         ├─ WaitGameplayEvent("Meta.IncomingXP")   ← 监听到
         ├─ AssignTagSetByCallerMagnitude           ← 填 XP 到 GE Spec
         └─ ApplyGameplayEffectSpecToSelf           ← GE 写入 IncomingXP
              │
              ▼
         PostGameplayEffectExecute
           ├─ HandleIncomingXP:
           │    ├─ LocalXP = GetIncomingXP()
           │    ├─ SetIncomingXP(0.f)              ← 读走清零
           │    └─ PlayerInterface::Execute_AddToXP(Character, XP)  ← 接口桥接
           │         │
           │         ▼
           │    AAura_Character::AddToXP_Implementation
           │         └─ PlayerState->AddXP(XP)
           │              ├─ XP += InXP
           │              └─ OnXPChanged.Broadcast(XP)    ← 广播委托
           │                   │
           │                   ▼
           │              WidgetController::OnXPChangedFunc
           │                   ├─ LevelUpInfo->FindLevelForXP()  ← 翻译为百分比
           │                   └─ OnXPPercentChanged.Broadcast(%)
           │                        │
           │                        ▼
           │                   UI Widget → XP 进度条更新
```

### 27.9 C++ 继承访问控制总结

| 继承方式 | public 成员变… | protected 成员变… | 外部能调接口？ |
|---------|---------------|-------------------|:---:|
| `public` 继承 | public | protected | ✅ |
| `protected` 继承 | protected | protected | ❌ |
| `private` 继承 | private | private | ❌ |

UE 中继承接口永远用 `public`。"是一个"的关系必须对外可见。

---

## 二十八、接口 BlueprintNativeEvent、网络策略、冷却机制

> 2026-08-07，接口函数重构 + 客户端火球调试实战

### 28.1 virtual → BlueprintNativeEvent 重构要点

**改前：**
```cpp
// .h
virtual int32 GetPlayerLevel();  // 纯 C++ virtual，BP 看不见

// .cpp
int32 IMy_CombatInterface::GetPlayerLevel() { return 0; }
```

**改后：**
```cpp
// .h
UFUNCTION(BlueprintNativeEvent)
int32 GetPlayerLevel();  // UHT 生成 Execute_ + _Implementation 包装

// .cpp — 默认实现可以删掉！UHT 自动生成 virtual int32 GetPlayerLevel_Implementation() { return 0; }
```

**调用方式变化：**

| 方面 | virtual | BlueprintNativeEvent |
|------|---------|---------------------|
| C++ 调用 | `Cast<IXXX>(obj)->Func()` | `IXXX::Execute_Func(obj)` |
| Implements 检查 | 不需要（直接 Cast） | `obj->Implements<UXxx>()` 先检查 |
| BP 可见 | ❌ | ✅ |
| UHT 生成默认实现 | 需要手动在 .cpp 写 | 自动生成 return 默认值 |
| 直接调用包装函数 | — | 会 crash："Do not directly call Event functions in Interfaces. Call Execute_XXX instead." |

**⚠️ `Implements<>` 必须传 U 类，不能传 I 类：**
```cpp
✅ obj->Implements<UMy_CombatInterface>()   // UHT 生成的 UINTERFACE 类
❌ obj->Implements<IMy_CombatInterface>()   // I 类没有 UClass，行为未定义
```

UHT 为 I 类生成的包装函数：
```cpp
// .gen.cpp
int32 IMy_CombatInterface::GetPlayerLevel()
{
    check(0 && "Do not directly call Event functions in Interfaces. Call Execute_GetPlayerLevel instead.");
    // 永远不要直接调用！使用 Execute_GetPlayerLevel()
}
```

### 28.2 NetExecutionPolicy 四种模式

| 策略 | 客户端激活 | 触发逻辑 | 典型用途 |
|------|:---------:|---------|---------|
| **LocalPredicted** | ✅ 立即 | 客户+服务端都激活；客户端先播，服务端验证 | 投射物、跳跃、冲刺 |
| **LocalOnly** | ✅ 立即 | 仅客户端激活；服务端不执行 | 纯UI/视觉效果 |
| **ServerInitiated** | ⏳ 等待 | 服务端先激活→确认后客户端跟进 | 需服务端计算结果 |
| **ServerOnly** | ❌ 不激活 | 仅服务端激活；客户端只接收属性复制 | AI技能、GM命令 |

**ServerOnly 陷阱**：属性变化会复制（法力扣了），但动画/Actor 生成不复制——客户端看不到任何反馈。

### 28.3 投射物生成架构

```
LocalPredicted + HasAuthority 守卫：

客户端 ActivateAbility → 播动画 ✅ → SpawnProjectile → HasAuthority? No → return
                                                        ↓
服务端 ActivateAbility → 播动画 → SpawnProjectile → HasAuthority? Yes → 生成火球
                                                                       ↓
                                                                Replicate 到客户端 ✅
```

**关键**：动画是"预测"的（客户端立即播），火球是"复制"的（服务端生成→同步）
——两件事走不同路径。

### 28.4 冷却机制：GE + Tag 阻塞

```
按下技能 → CooldownGE 应用到身上
             └─ GrantedTags: Cooldown.FireBolt
                    │
                    ▼
              Ability 的 ActivationBlockedTags 包含 Cooldown.FireBolt
                    │
                    ▼
              冷却期间 TryActivateAbility → false（Tag 阻塞）
                    │
              CooldownGE 过期 → Tag 自动移除
                    │
                    ▼
              可以再次激活 ✅
```

本质：**GE 贴 Tag → Tag 阻塞 Ability → GE 过期 Tag 消失 → 解除阻塞**

### 28.5 Git Bisect 定位 Bug

```bash
git log --oneline           # 确认提交历史
git checkout <commit-hash>  # 逐个回退测试（二分法）
# 编译 → 编辑器测试 → 不行继续往回跳
# 找到最后一个"好"的版本，diff 对比下一个"坏"的版本
git checkout master         # 回到最新
```

本次实战经验：bug 不一定是你最近引入的——可能藏了很久。先确认是哪个 commit 引入的，再分析改动。盲目回退自己的最新改动会浪费排查时间。

---

## 二十九、UE5 相机系统：SpringArm + Camera 完全指南

> 2026-08-11，C++ 构造函数配置相机，理解各种旋转参数，主流游戏配置对比

### 29.1 旋转的三个基本轴

```
Pitch = 点头（绕 Y 轴：上下摆头）
Yaw   = 摇头（绕 Z 轴：左右转身）
Roll  = 歪头（绕 X 轴：侧着头靠肩膀）

放角色身上：
  Pitch → 身体前倾/后仰（别开，角色会躺倒）
  Yaw   → 全身左右转
  Roll  → 歪身子（几乎永远不开）
```

三个开关在 Pawn 上的效果：

| 参数 | 开了的效果 |
|------|-----------|
| `bUseControllerRotationYaw` | 鼠标左右移 → 角色原地转向 |
| `bUseControllerRotationPitch` | 鼠标上下移 → 角色前倾后仰（**别开**） |
| `bUseControllerRotationRoll` | 鼠标侧移 → 角色歪身子（**永远别开**） |

### 29.2 旋转控制链

```
Controller（鼠标/手柄输入）
  → Pawn::ControlRotation（存储累积旋转值）
    → SpringArm（可选：跟不跟这个旋转）
      → Camera（挂在弹簧臂末端）
```

**关键**：`bUsePawnControlRotation` 里的 "Pawn" 有误导性。数据确实存在 Pawn 上，但**值是由 Controller 写入的**：

```
手柄右摇杆 → PlayerController::AddYawInput/AddPitchInput
           → 写入 Pawn::ControlRotation
           → SpringArm 读这个值来旋转
```

Controller 是"方向盘"，Pawn::ControlRotation 是"转向机"，SpringArm 的开关决定"跟不跟转向机走"。

### 29.3 三个关键开关

| 开关 | 在哪设 | 含义 |
|------|--------|------|
| `bUseControllerRotationYaw/Pitch/Roll` | Pawn（角色） | 角色的身体跟不跟鼠标转 |
| `SetUsingAbsoluteRotation(bool)` | SpringArm | true=世界绝对旋转；false=跟 Pawn/Controller |
| `bUsePawnControlRotation` | SpringArm | 弹簧臂跟不跟 Pawn::ControlRotation |

**重要**：`bUsePawnControlRotation = true` 会**覆盖** `SetRelativeRotation` 手动设置的旋转值。因为弹簧臂直接读 ControlRotation（默认 0° 水平），不理 RelativeRotation。

### 29.4 三种主流配置

#### FPS / 越肩第三人称（使命召唤、吃鸡）

```
体验：鼠标完全控制视角旋转，角色身体 Yaw 跟鼠标，相机全跟
```

```cpp
// Pawn（角色）
bUseControllerRotationYaw   = true;   // 鼠标左右 → 角色转
bUseControllerRotationPitch = false;
bUseControllerRotationRoll  = false;

// SpringArm — 跟 Controller 旋转
SpringArm->bUsePawnControlRotation = true;
// 不调 SetUsingAbsoluteRotation（默认 false = 跟 Pawn Rotation）

// Camera — 挂在弹簧臂末端，不用再设
Camera->bUsePawnControlRotation = false;
```

#### 黑魂 / 怪猎（右摇杆转视角，角色不转）

```
体验：右摇杆看风景，角色身体跟移动方向转，和相机解耦
```

```cpp
// Pawn（角色）— 不跟右摇杆
bUseControllerRotationYaw   = false;  // 角色身体不跟相机
bUseControllerRotationPitch = false;
bUseControllerRotationRoll  = false;

// 角色跟移动方向（左摇杆）
GetCharacterMovement()->bOrientRotationToMovement = true;

// SpringArm — 跟右摇杆（Controller）
SpringArm->bUsePawnControlRotation = true;

// Camera
Camera->bUsePawnControlRotation = false;
```

#### Top-Down / MOBA（暗黑、LOL）

```
体验：相机固定俯角，鼠标只移动角色/释放技能，不转视角
```

```cpp
// Pawn（角色）— 全 false
bUseControllerRotationYaw   = false;
bUseControllerRotationPitch = false;
bUseControllerRotationRoll  = false;

// 角色朝移动方向转
GetCharacterMovement()->bOrientRotationToMovement = true;

// SpringArm — 固定俯角，世界绝对旋转
SpringArm->SetUsingAbsoluteRotation(true);          // 不受任何人影响
SpringArm->SetRelativeRotation(FRotator(-45, 0, 0)); // 45° 斜向下
SpringArm->bDoCollisionTest = false;                // 不撞墙拉近
SpringArm->TargetArmLength = 800.f;                 // 弹簧臂长度

// Camera — 不跟任何旋转
Camera->bUsePawnControlRotation = false;
```

### 29.5 bUsePawnControlRotation 和 SetRelativeRotation 的互斥

```
bUsePawnControlRotation = false → 用你手动设的 RelativeRotation（-45° 生效）
bUsePawnControlRotation = true  → 无视 RelativeRotation，用 ControlRotation（默认 0° 水平）
```

如果设了 `bUsePawnControlRotation = true` 但没写输入处理，ControlRotation 默认 `(0,0,0)`，相机就变成水平方向。需要手动设初始 ControlRotation：

```cpp
// PlayerController::BeginPlay
SetControlRotation(FRotator(-45.f, 0.f, 0.f));
```

### 29.6 Camera 的 bUsePawnControlRotation 基本不需要管

Camera 是 SpringArm 的子节点，Transform 完全继承自 SpringArm。SpringArm 已经把旋转管死了，Camera 设 `false` 即可。设 `true` 反而可能产生奇怪的双重旋转。

### 29.7 写 ControlRotation 但不使用 = 没影响

`AddYawInput` / `SetControlRotation` 只是往 Pawn::ControlRotation 写值。如果 SpringArm 的 `bUsePawnControlRotation = false`，没人读这个值，写了也等于白写。对游戏表现零影响。

---

## 三十、Niagara 粒子面向相机 & 构造函数组件创建细节

> 2026-08-11

### 30.1 Niagara 升级特效始终面向相机

```cpp
const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
const FRotator ToCamera = (CameraLocation - NiagaraLocation).Rotation();
LevelUpNiagaraComponent->SetWorldRotation(ToCamera);
```

`ToCamera` 是**绝对世界朝向**（从 Niagara 指向相机的方向），不是"旋转差值"。用 `SetWorldRotation` 直接设到位。

### 30.2 为什么不用 AddLocalRotation

`AddLocalRotation` 是在**当前旋转上叠加增量**，在 Local 空间加。

- `SetWorldRotation(ToCamera)`：**"你的朝向 = 这个方向"** → 正对相机 ✅
- `AddLocalRotation(ToCamera)`：**"在你的朝向上再扭这么一个幅度"** → 语义不对，且如果父组件有旋转，Local 和 World 空间不重合会歪 ❌

### 30.3 bAutoActivate 是成员变量不是函数

```cpp
// ❌ 编译错误 C2064
LevelUpNiagaraComponent->bAutoActivate(false);

// ✅ 正确
LevelUpNiagaraComponent->bAutoActivate = false;
```

`bAutoActivate` 是 `UActorComponent` 的 public 成员变量，赋值不是函数调用。

### 30.4 统计帧率命令

```
stat fps       — 显示帧率（再输一次关闭）
stat unit      — 帧时间详情（Game/Draw/GPU 耗时）
stat none      — 一次性关闭所有 stat
```

---

## 三十一、AttributePoint/SpellPoint 点数系统 + 菜单初始广播

> 2026-08-14。属性点/法术点系统：PlayerState 存储复制 + Character 转发 + 菜单 WidgetController 初始广播。核心是两个模式："双入口广播"和"事件驱动 UI 要补存量"。

### 31.1 完整链路

```
升级 GE → My_AuraAttributeSet::PostGameplayEffectExecute 算奖励
  → Execute_AddToAttributePoint(SourceCharacter, 奖励)
  → AAura_Character::AddToAttributePoint_Implementation   ← 转发（曾漏成 TODO，是断链点）
  → AMy_AuraPlayerState::AddAttributePoint()
  → OnAttributePointChanged.Broadcast
      ├─ 服务器：AddAttributePoint 内直接广播
      └─ 客户端：OnRep_AttributePoint → 广播（复制流程见 25.11）
  → 菜单 OnPlayerAttributeChanged → 点数文本
```

### 31.2 PlayerState 端四件套（与 Level/XP 完全同款）

```cpp
UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoint)
int32 AttributePoint;

void SetAttributePoint(int32 n) { AttributePoint = n; OnAttributePointChanged.Broadcast(AttributePoint); }  // 服务器
void AddAttributePoint(int32 n) { AttributePoint += n; OnAttributePointChanged.Broadcast(AttributePoint); } // 服务器
void OnRep_AttributePoint(int32 Old) { OnAttributePointChanged.Broadcast(AttributePoint); }                 // 客户端
// DOREPLIFETIME(AMy_AuraPlayerState, AttributePoint);
```

`SpellPoint` 同理。**每个可复制点数据都要凑齐：属性 + DOREPLIFETIME + OnRep + Set/Add + 委托**，是一套固定模板。

### 31.3 菜单初始广播的坑（BroadcastInitiaValues）

WidgetController 两个阶段分工不同：

| 函数 | 职责 | 时机 |
|------|------|------|
| `BroadcastInitiaValues` | 推当前值（存量） | 创建时一次 |
| `BindCallbacksToDependencies` | 绑委托，响应将来变化（增量） | 创建时一次 |

**坑**：只绑委托、不广播初始值 → 菜单首次打开显示 0/陈旧。

**原理**：委托是"时点事件"，只能通知到"那一刻已存在的监听者"。升级发生在菜单创建**之前**，lambda 注册晚于变化 → 错过。且 UI 是**被动接收**（解耦架构），不会主动查数据。

**修复**：`BroadcastInitiaValues` 里主动查一次当前值：

```cpp
AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
if (MyPS)
{
	OnPlayerAttributeChanged.Broadcast(MyPS->GetAttributePoint());
}
```

**通用原则：创建时补存量，变化时推增量，两条路都得有。**（XP 进度条 25.9 同款坑）

### 31.4 WidgetController 单例 + AddLambda 重复绑定坑

HUD 用 `if (XxxWidgetController == nullptr)` 缓存控制器，保证只创建一次：

```cpp
if (MenuWidgetController == nullptr)
{
	MenuWidgetController = NewObject<...>(this, MenuWidgetControllerClass);
	MenuWidgetController->SetWidgetControllerParams(WCParams);
	MenuWidgetController->BindCallbacksToDependencies();  // 只在这里执行一次
}
return MenuWidgetController;
```

**去掉守卫的坑**：每次创建都调 `BindCallbacksToDependencies` → `AddLambda([this])` 往 PlayerState 委托上**累积**绑定。打开菜单 N 次 → 一次点数变化触发 N 次广播（浪费）；旧控制器被 GC 后 lambda 还挂着（访问已销毁 UObject → 崩溃）；强捕获让旧对象泄漏。

**这个守卫保证了"绑定一次"的不变式**——所有 `AddLambda` 代码都依赖它。

### 31.5 子 widget Assign 与菜单 Broadcast 的时序

菜单是 Overlay 的子 widget：子 widget（行）做 Assign，菜单做 Broadcast。

```
子 widget Construct → 自己调 My_GetMenuWidgetController（创建控制器 + C++ 绑定）
  → Assign OnAttributeInfo / OnPlayerAttributeChanged
菜单 Construct → 调 My_GetMenuWidgetController（返回缓存）→ Broadcast Initial Values
```

**关键**：
- UMG 子 widget 先 Construct、父后 Construct → 时序天然正确
- 子 widget 必须**自己拿控制器**（`My_GetMenuWidgetController`），不能等父传——否则 Assign 绑空
- 所有调用走同一 HUD → 同一控制器，不会创建第二个（见 31.4）

## 三十二、网络架构：客户端/服务器分工、Ability UI 显示链路、InputTag 一物三用

### 32.1 核心心法：一套代码，两种角色

服务器和客户端运行的是**同一份代码**（同一引擎、同一 Actor 类、同一函数）。区别是"角色"：

- **服务器 = 裁判**：持有权威数据、执行规则；没有 UI、没有输入、没有渲染
- **客户端 = 观众**：有 UI、有输入；只有数据的复制副本

比喻：裁判和观众站在同一个球场看同一场球（世界状态同步），但吹哨、记分只有裁判有权。

### 32.2 客户端与服务器各自拥有什么（修正"服务器什么都有"）

"服务器什么都有"只说对一半：**服务器有所有数据的真值，但没有 UI/输入**。

| | 客户端 | 服务器 |
|---|---|---|
| UI / 按钮 | ✅（Overlay、菜单） | ❌ 不创建 UMG Widget |
| 输入（键鼠） | ✅ | ❌ 无头运行 |
| 渲染 | ✅ | ❌ |
| 数据 | 复制副本 | 权威真值 |
| 规则执行 | 预测/表现 | 权威执行 |

**"拥有"分两种：数据副本人人有，权威身份只属服务器。**

### 32.3 PlayerController 分配

规则：**服务器为每个连入的玩家 spawn 一个 PC，且每个 PC 只复制给对应的拥有者客户端**（`bOnlyRelevantToOwner`）。

```
服务器: PC[玩家1] PC[玩家2] + 所有权威数据
客户端1: PC[玩家1]（只看到自己）+ 所有玩家的 PlayerState 副本
客户端2: PC[玩家2]（只看到自己）+ 所有玩家的 PlayerState 副本
```

- PC 只对拥有者相关 → 客户端代码里 `PlayerController->GetPlayerState()/GetHUD()` 永远是"自己"的 → UI、WidgetController、按钮都是自己的
- PlayerState 复制给**所有**客户端（要看别人等级/血量），但只有服务器能改真值
- 服务器持有所有 PC 的权威实例，但**不执行 UI**

### 32.4 三种同步机制（同一段代码如何分流）

"都运行同一段代码"的真实含义：**每个函数都存在于两端，但只有匹配角色的一端真正执行函数体**。三种标记决定谁执行：

1. **Replication**：`UPROPERTY(ReplicatedUsing=OnRep)` 服务器改值 → 复制 → 客户端 OnRep 只在客户端跑
2. **Server RPC**：`UFUNCTION(Server, Reliable)` 客户端调用 → `_Implementation` 只在服务器执行
3. **Client RPC**：`UFUNCTION(Client, Reliable)` 服务器调用 → `_Implementation` 只在客户端执行（例：`ClientEffectApplied`）

### 32.5 为什么服务器"没有按钮" + 升级链路职责划分

按钮属于人机交互，**只有客户端创建**。服务器的工作是接收请求、验证、执行规则，它不"操作 UI"。

升级按钮完整链路（本项目 My_ 版已实现）：

```
客户端: 按钮点击 → AttributeMenuWC::UpgradeAttribute → ASC::UpgradeAttribute
         → 查 GetAttributePointFormPlayerState() > 0（客户端缓存值，防呆）
         → ServerUpgradeAttribute()（Server RPC，客户端函数体不执行）
服务器: ServerUpgradeAttribute_Implementation
         → 再查点数（权威真值，防作弊）
         → SendGameplayEventToActor(属性Tag) → 改属性
         → AddToAttributePoint(-1)
结果: 复制回客户端 → 菜单刷新显示
```

### 32.6 Ability UI 显示链路（OnAbilityGiven 双路径）

**问题根源**：`GiveAbility` 只在服务器 + multicast 委托不跨网络 → 客户端 UI 收不到"能力已就绪"的通知。

**双路径补广播**：

```
服务器: AddCharacterAbilitiesFromASC → GiveAbility
         → bStartupAbilityGiven=true（服务器的副本）
         → OnAbilityGiven.Broadcast（只对本机 UI=监听服务器主机有用）
客户端: ActiveAbilities 复制下来 → OnRep_ActivateAbilities
         → bStartupAbilityGiven:false→true（客户端的副本）
         → OnAbilityGiven.Broadcast（本地补广播）
```

**bStartupAbilityGiven 是非复制属性，每台机器一份**：
- 服务器在 AddCharacterAbilitiesFromASC 置 true
- 客户端在 OnRep_ActivateAbilities 置 true
- 作用：OnRep 会多次触发（以后加新能力数组再变），旗标保证只初始化一次

**WidgetController 时序兜底（存量/增量）**：

```cpp
if (AuraASC->bStartupAbilityGiven)
    OnInitializeStartupAbilities(AuraASC);       // 存量：直接拉当前列表
else
    AuraASC->OnAbilityGiven.AddUObject(...);      // 增量：订阅等广播
```

竞态：`GiveAbility` 和 "HUD 创建 WidgetController" 时序不定。if/else 两种顺序都覆盖。

完整链路：服务器 PossessedBy → InitAbilityActorInfo → AddCharacterAbilities（HasAuthority 保险）→ GiveAbility → ActiveAbilities 复制 → 客户端 OnRep → 广播 → WidgetController if/else → OnInitializeStartupAbilities → ForEachAbility 遍历 → 查 AbilityDA → OnAbilityInfo.Broadcast → 图标显示。

### 32.7 没有 OnRep 的后果（远程客户端必坏，不是"运气"）

⚠️ 修正：删掉 OnRep 不是"运气好能显示/运气不好不显示"，而是**远程客户端 100% 必坏**。

原因：客户端 `bStartupAbilityGiven` 永远 false（非复制 + 只有 OnRep 置位）→ WidgetController 永远走 else 订阅 → 广播无人发 → 技能栏永远空白。

"运气"只是描述"没有 if/else 只靠订阅"的旧写法；有了 OnRep + if/else，两种时序都覆盖，不靠运气。

| 机器 | 没有 OnRep 的后果 |
|---|---|
| 远程客户端 | ❌ 必坏 |
| 监听服务器主机 | ✅ 靠服务器广播直通本机 UI |
| 单机 | ✅ 同上 |

### 32.8 InputTag 一物三用：设置 → 存储 → 输入路由 + UI 显示

同一个 Tag，三个环节：

```
① 设置：Ability BP 的 StartUpInputTag = My_InputTag.1
② 存储：AddCharacterAbilitiesFromASC
         AbilitySpec.DynamicAbilityTags.AddTag(StartUpInputTag)  // tag 跟 spec 复制到客户端
③ 两条消费路径，读同一 DynamicAbilityTags：
   输入路由：按"1" → InputConfig 查 My_InputTag.1 → ASC 遍历
            spec.DynamicAbilityTags.HasTagExact(Tag) → 激活 ✅
   UI 显示：GetInputTagFromAbilitySpec 从 DynamicAbilityTags 读回 tag
            → info.InputTag → 图标显示"1" ✅
```

**UI 不是自己猜按键，而是把能力身上存的 tag 读出来显示 → 永远和输入绑定一致。**

tag 是"按键身份证"不是物理键：物理键→tag 映射在 InputConfig DataAsset 改；tag→能力 匹配靠 StartUpInputTag 改。

### 32.9 升级按钮 Anti-cheat 细节（本次实现回顾）

本次 `ServerUpgradeAttribute_Implementation` 的服务器校验：

```cpp
if (IMy_PlayerInterface::Execute_GetAttributePointFormPlayerState(GetAvatarActor()) <= 0) return;
```

✅ 已修正为 `<= 0`：等于 0 就 return，保证永不为负。客户端 `> 0` 是防呆、服务器 `<= 0` 才是防作弊边界。

### 32.10 GAS 执行顺序：GE 执行中 SetHealth(GetMaxHealth()) 读到旧 Max

**症状**：升级时 `PostGameplayEffectExecute` 里 `SetHealth(GetMaxHealth())`，MaxHealth 只升了上限、血量没跟着补满。

**原因**：两条时间线错开——

```
GE 执行过程中：
  InternalExecuteMod → PreAttributeChange → 写 BaseValue
  → PostAttributeChange → PostGameplayEffectExecute   ← 此刻在此
  → 整条 GE 完成后 → UpdateAllAggregators 全局刷新     ← MMC 在这里才跑
```

- Instant GE 的 `PostGameplayEffectExecute` 在**聚合器刷新之前**执行
- MaxHealth 是 MMC（MMC 依赖 VIgor + Level）算出来的**聚合值**，此时还没重算 → `GetMaxHealth()` 读到的还是旧上限
- 所以升级补满不能写在 GE 执行中，只能**置标记**，等 MMC 刷新后补

**修法（教程 bool 标记法）**——`My_AuraAttributeSet`：

```cpp
// 升级分支：只置标记，不 SetHealth
bTopOffHealthOnLevelUp = true;
bTopOffManaOnLevelUp   = true;

// PostAttributeChange：MMC 已刷新，MaxHealth/MaxMana 已是新值，再补满
if (bTopOffHealthOnLevelUp && Attribute == GetMaxHealthAttribute())
{
    SetHealth(GetMaxHealth());
    bTopOffHealthOnLevelUp = false;
}
if (bTopOffManaOnLevelUp && Attribute == GetMaxManaAttribute())
{
    SetMana(GetMaxMana());
    bTopOffManaOnLevelUp = false;
}
```

**要点**：`PostAttributeChange` 对所有类型 GE 都触发（含 Infinite），也晚于聚合器刷新——所以补满（读 GetMaxHealth）必须放这里，不能放 `PostGameplayEffectExecute`。

### 32.11 AbilityTag [None] 报错：被动技能没配 My_Abilities tag

**症状**：PIE 2 人 ListenServer 报 `Can not find info form AbilityTag [None]`，单人不报。

**链路**：Overlay 遍历 ASC 所有能力 → `GetAbilityTagFromAbilitySpec` 找 `My_Abilities.*` 前缀 → 被动技能没配 → 返回 None → `FindAbilityInfoFromTag(None)` 查不到 → 报错。

**为什么 1P 不报、2P 报**——给能力的顺序：

```
AddCharacterAbilities：
  ① AddCharacterAbilitiesFromASC(主动)     → 结束即广播 OnAbilityGiven
  ②（此刻被动还没给，遍历不到 → 1P 不报错）
  ③ AddCharacterPassiveAbilitiesFromASC(被动)
```

- 1P：广播发生在 ③ 之前 → 被动不在列表 → 不报
- 2P：客户端能力整体复制，`OnRep_ActivateAbilities` 触发广播时被动已在列表 → 遍历到它 → [None]

**无害**：被动不需要图标，UI 忽略空 info，只是日志难看。

**修法**：`OnInitializeStartupAbilities` lambda 加守卫：

```cpp
const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromAbilitySpec(AbilitySpec);
if (AbilityTag.IsValid())  // 跳过被动这类没配 My_Abilities tag 的能力
{
    FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTag);
    info.InputTag = AuraASC->GetInputTagFromAbilitySpec(AbilitySpec);
    OnAbilityInfo.Broadcast(info);
}
```

其拓扑根源见下节 32.12。

### 32.12 PIE ListenServer 多人测试拓扑（镜像 pawn 是万恶之源，两个 bug 同一根源）

先理解**标准网络模型**（任何 UE 多人游戏都一样）：

- 所有玩家的**真实 pawn 都住在服务器世界**（Auth=1）——真正算 gameplay 的地方
- 每个客户端有**自己的客户端视图世界**：服务器把状态复制过来，客户端看到的是**副本**（Auth=0）
  - 自己控制的那份副本 = **自主代理**：本地预测，操作经 RPC 发到服务器、作用在服务器世界的真实 pawn 上
  - 别人的角色 = **模拟代理**：纯展示，跟着服务器数据走
- **pawn 总数公式**：服务器世界永远只有 **N 个真实 pawn**；每个客户端视图里有 **N 个副本**。总数 = N + N×客户端数 = **N²**：2P=4、3P=9（不是 6——每个客户端看到的是**所有** N 个角色的副本，不只"自己+别人"各 1 个）

PIE 2 人 + ListenServer 时，编辑器给**每个连接**各建一个"客户端视图世界"——连**主机连接也有**（这是 PIE 特有）：

```
服务器世界（权威，唯一）             主机的客户端视图（PIE 特有）    客户端的客户端视图
├─ pawn A  Auth=1 主机真实           └─ pawn A' Auth=0 主机看自己    └─ pawn B' Auth=0 客户端看自己
└─ pawn B  Auth=1 客户端真实
→ 一共 4 个 pawn（2×Auth=1 + 2×Auth=0）
```

| pawn | 在哪 | Role | 是什么 |
|------|------|------|--------|
| A | 服务器世界 | Auth=1 | 主机玩家的真实 pawn（权威） |
| B | 服务器世界 | Auth=1 | **客户端玩家的真实 pawn（权威）**——客户端所有操作最终作用在这 |
| A' | 主机的客户端视图 | Auth=0 | 主机"看自己"的副本。**PIE 特有**：打包后主机直接看服务器世界，没有这个镜像 |
| B' | 客户端的客户端视图 | Auth=0 | 客户端自己控制的副本（自主代理）。**所有多人模式都有**，真实数据在服务器 pawn B 上 |

**关键区别：两个镜像 pawn 的 ASC 归属不同**

| 谁的镜像 | 用的 ASC | 二次 InitAbilityActorInfo 的后果 |
|----------|----------|-------------------------------|
| 主机 A' | **共享主机权威 PlayerState/ASC**（主机 PC 驻留服务器世界，同一份 ASC 被两个 pawn 先后初始化） | 权威 ASC 上**二次施放属性 GE** → 覆盖 MMC 捕获链 → 升级 MaxHealth 不重算（430 钉死） |
| 客户端 B' | **独立的非权威 ASC** | 二次施放 GE 是 no-op（只改显示、不改数据）→ 一直正常 |

**主机进程里实际执行的初始化**（PIE 所有世界都在同一进程跑）：

```
服务器世界：  A → PossessedBy → Init ①（主机权威 ASC）
             B → PossessedBy → Init ②（客户端B的权威 ASC）
复制视图世界：A' → OnRep_PlayerState → Init ①'（同一份权威 ASC ← 二次！）
             B' → OnRep_PlayerState → Init（客户端B的非权威副本，无害）
```

主机端有 A、B 两个角色，但初始化执行时 **A 和 A' 把同一个权威 ASC 碰了两遍**（①+①'）——A' 那次的 `InitializeDefaultAttribute` 二次施放属性 GE，就是断 MMC 链的那一下。

**为什么 2P 才会出问题、单人/客户端不出**：镜像 pawn 只在编辑器多窗口 PIE 里存在；它二次跑 `My_InitAbilityActorInfo()` 会重复两件事——

1. 二次施放属性 GE（`InitializeDefaultAttribute`）→ 断 MMC 捕获链 → **MaxHealth bug**
2. 重新 `InitOverlay` → 重新遍历全部能力 → 此时被动技能已在列表（无 `My_Abilities` tag）→ **AbilityTag [None] 报错**

**打包后为什么没有**：

| 环境 | 主机结构 | 会不会二次施放 |
|------|----------|--------------|
| 打包 ListenServer | 主机视图 = 服务器世界本身，一个 pawn、一次 PossessedBy | 不会 |
| 打包 Dedicated | 主机是普通客户端，独立进程，非权威 ASC，GE 施放 no-op | 不会 |
| PIE 2 人 | 主机多一个镜像 pawn，共享权威 ASC | 会 ← 唯一 |

> 补充：打包的**客户端**视图里也仍有 A'（它看到的"主机"副本），也会跑 `OnRep_PlayerState` → `My_InitAbilityActorInfo` → 施放 GE——但用的是客户端**非权威 ASC 副本**，改的是本地临时值、随即被服务器复制覆盖，**碰不到主机权威 ASC** → 无害。关键区别始终是：**A' 拿到的是不是权威 ASC**。

**调试要点**：
- 数 pawn / 看镜像：`My_InitAbilityActorInfo` 里打 `GetLocalRole()`，`Role_Authority`=真 pawn、`Role_SimulatedProxy`=镜像
- 结论：这两个 bug 是 **PIE 测试工具的产物**，不是游戏逻辑错误，打包多人不受影响（建议打包实测一次确认）

## 三十三、WidgetController 架构深入 + UMG ViewModel（MVVM）对比

> 本节回答三个问题：为什么 AuraWidgetController 里放两套 4 个成员、Broadcast/BindCallbacks 到底在哪被调用、以及社区这套"Controller"模式在工业界/官方视角的定位（MVVM）。

### 33.1 为什么 AuraWidgetController 有两套 4 个成员（懒加载 getter = memoization）

`UAuraWidgetController` 里实际是 8 个成员 + 4 个 getter：

| 泛型 4 个（基类类型） | 派生 4 个（Aura 类型） | getter |
|---|---|---|
| `PlayerController` | `AuraPlayerController` | `GetAuraPC()` |
| `PlayerState` | `AuraPlayerState` | `GetAuraPS()` |
| `AbilitySystemComponent` | `AuraAbilitySystemComponent` | `GetAuraASC()` |
| `AttributeSet` | `AuraAttributeSet` | `GetAuraAS()` |

**分工**：
- 泛型 4 个由 `SetWidgetControllerParams()` 原样存入（来自 `FWidgetControllerParams`），负责**解耦**——设置阶段不关心具体类型。
- 派生 4 个是 getter 里 `Cast` 的**缓存结果**，负责**类型安全访问**——真正干活要用派生类型（`GetAbilityTagFromSpec`、`ForEachAbility` 等只有 `UAuraAbilitySystemComponent` 有）。

**懒加载模式**（memoization）：
```cpp
AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
    if (AuraPlayerController == nullptr)      // 只有第一次才 cast
    {
        AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
    }
    return AuraPlayerController;               // 之后直接读缓存
}
```
与"每处直接 cast"完全等效，只是：
1. 调用点更短（`GetAuraPC()->X()` vs `Cast<AAuraPlayerController>(PlayerController)->X()`）
2. **整个生命周期只 cast 一次**（`ForEachAbility` 对每个能力回调 lambda 里反复访问时才有意义）
3. 泛型→派生是**单点翻译**：具体类型将来变了只改 getter 一处

> 注意：这是**工程整洁度偏好**，不是功能必需。每处直接 cast、或在 `SetWidgetControllerParams` 里 eager cast 都行，效果一样。

### 33.2 BroadcastInitiaValues / BindCallbacksToDependencies 到底在哪被调用

完整调用链（My_ 版）：

```
AAura_Character::My_InitAbilityActorInfo()          ← PossessedBy(服务器) 和 OnRep_PlayerState(客户端) 都会进
└─ AuraHUD->InitOverlay(PC, PS, ASC, AS)             (Aura_Character.cpp:185)
   ├─ CreateWidget 创建 OverlayWidget
   ├─ GetOverlayWidgetController()                   ← 首次懒创建
   │    ├─ NewObject
   │    ├─ SetWidgetControllerParams(WCParams)
   │    └─ BindCallbacksToDependencies()             ← ① 绑委托在这（HUD getter 内）
   ├─ OverlayWidget->SetWidgetController(Controller) ← 触发蓝图 WidgetControllerSet()
   ├─ OverlayWidgetController->BroadcastInitiaValues() ← ② 广播在这（必须在 ① 之后）
   └─ OverlayWidget->AddToViewport()
```

**两个关键时序**：
1. `BindCallbacksToDependencies()` 在 HUD 的 `GetXXXWidgetController()` 里、controller 首次 NewObject 后调用——**订阅数据源的变化委托**（属性一变 controller 收到再转播给 widget）。
2. `BroadcastInitiaValues()` 必须在 `SetWidgetController` **之后**调用——广播是触发委托，widget 还没把自己的委托绑到 controller 上时，广播就"发给了空气"。顺序不能反。

**Menu 不同**：Menu 的 `BroadcastInitiaValues()` **不在 C++ 里调用**。蓝图 WBP_AttributeMenu 打开时调 `My_GetMenuWidgetController()`（静态库函数）→ 内部走 HUD `GetMenuWidgetController()`（**此时 BindCallbacks 已在 C++ 里跑完**）→ 蓝图拿到"绑定完毕的成品"后再调 `BroadcastInitiaValues()`。所以蓝图里只有广播、没有 BindCallbacks——后者是纯 C++ 函数（无 UFUNCTION），蓝图根本调不到，也不需要调。

### 33.3 Overlay 与 Menu 的创建路径为什么不同（谁决定"什么时候创建"）

| | Overlay | Menu |
|---|---|---|
| 何时创建 | 游戏开始（InitOverlay 固定入口） | 玩家按键打开时 |
| 谁触发 | C++（角色初始化链自动） | 蓝图（输入事件/按钮） |
| controller 来源 | C++ 直接 `SetWidgetController` | 蓝图调静态函数 |
| `My_GetXXXWidgetController` 是否用到 | ❌（grep 只有定义无调用） | ✅ |

**根因**：创建时机由"谁"决定。Overlay 是常驻 HUD，角色初始化时 PC/PS/ASC/AS 必然就绪，C++ 有明确时机（`InitOverlay`）。Menu 是"玩家想开才要"，触发点是输入事件、代码在蓝图——C++ 初始化链不可能预知玩家何时按键，所以需要一个蓝图随时能调的入口。静态函数就是封装了"取 PC→HUD→PS/ASC/AS→组参数"的样板代码，蓝图一个节点搞定。

**为什么不启动时就创建好藏着**：省资源（菜单 WBP 常驻不用浪费）+ 刷新时机对（打开才广播一次）+ 绑定只跑一次（controller 被 HUD 缓存，第二次打开复用同一个、只重新广播不重复绑定）。

### 33.4 控制器数量 = 面板数量，子 Widget 共享

整个 UI 只有**少数几个 controller**，每个对应一个"面板"，面板内所有子 Widget 共享它：

| 面板 | 共享 controller |
|---|---|
| Overlay（血条/蓝条/技能球…） | OverlayWidgetController |
| AttributeMenu（属性行…） | AttributeMenuWidgetController |
| SpellMenu | SpellMenuWidgetController |

子 Widget 不自己持有 controller，通过父级面板的 `WidgetController`（`SetWidgetController` 触发蓝图 `WidgetControllerSet()` 事件）传下来。为什么一个面板只用一个：**数据源相同**（都读同一 ASC/AttributeSet）+ **委托是一对多广播**（血条绑 Health、蓝条绑 Mana，同一 controller 转播）+ **HUD 是唯一创建者与缓存点**。

### 33.5 这套模式是行业标准吗

**结论：原理是，类不是。**
- `AuraWidgetController` 这个具体类是 **GAS 社区教程约定**（Stephen Ulibarri 的 Aura 课程），不是 Epic 官方、也不是全行业通用。
- 背后的**设计原则**（UI 与数据源解耦、一屏一适配器、懒创建缓存、委托驱动更新）是行业共识，对应 **MVVM / MVC / Presenter** 模式。
- **UE5 官方现代化路线**：UMG ViewModel（MVVM），5.1+ 内置，用 FieldNotify + 编辑器拖线绑定。
- **Epic 官方示例 Lyra 根本不用 WidgetController**：UI 直接组件取数 + `UGameplayMessageSubsystem` 事件总线。

### 33.6 UMG ViewModel（MVVM）是什么、怎么用（血条例子）

UMG ViewModel = **UE5 内置 MVVM 框架**。WidgetController 是"手写 MVVM"，ViewModel 是"引擎帮你做 MVVM"——不用手动委托/Broadcast，属性一变引擎自动通知绑定的控件。

**核心三件套**：
```cpp
UCLASS()
class UMy_HealthViewModel : public UMVVMViewModelBase
{
    GENERATED_BODY()
public:
    void SetHealthPercent(float NewValue)
    {
        UE_MVVM_SET_PROPERTY_VALUE(HealthPercent, NewValue); // 设值 + 通知（值没变自动跳过）
    }
private:
    UPROPERTY(FieldNotify, Setter, Getter)   // 声明"可被监听"
    float HealthPercent = 0.f;
};
```
- `UMVVMViewModelBase`：VM 基类
- `FieldNotify`：声明该属性可被监听
- `UE_MVVM_SET_PROPERTY_VALUE`：设值并触发通知，内置"值没变就跳过"守卫

**使用 4 步**：
1. 建 VM 类（继承 `UMVVMViewModelBase`）
2. Widget BP 的 MVVM 面板加 ViewModel Source（`Create Instance` 模式：widget 构建时自动 new、销毁时自动释放）
3. 设计器选中控件属性（如 ProgressBar.Percent）→ 绑定图标 → Create Binding → 选 VM + 属性路径（如 `MyHealthVM.HealthPercent`）
4. 事件里 `Init(ASC)` 喂数据：VM 订阅 ASC 属性变化委托，收到变化就 `Set` 自己的属性

**数据流对比**：
```
WidgetController：ASC变 → 订阅lambda → controller手动Broadcast → widget手动Set控件
MVVM：            ASC变 → VM订阅委托 → VM里 Set → FieldNotify自动通知 → ProgressBar自动刷新
```
**唯一省掉的环节就是"手动通知 UI"**——取数、订阅这些活 VM 照样要写。绑定首次评估时自动读当前值，所以连"广播初始值"都不用了。

### 33.7 MVVM 关键坑：必须走通知通道，不能裸赋值

```cpp
HealthPercent = 0.5f;                              // ❌ 血条不动，没走通知
UE_MVVM_SET_PROPERTY_VALUE(HealthPercent, 0.5f);   // ✅ 血条动
```
蓝图里对 FieldNotify 属性用 Set 节点时也要接它的 notify 输出。另外两点：
- **VM 不是空降有数据**：得有人 Init 喂 ASC（对应 WidgetController 的 SetWidgetControllerParams）。
- **Create Instance 是每 widget 一个 VM**：多 Widget 要共享同一 VM 需用 Provider/Extension 模式（共享问题正是 Controller 的 HUD 缓存占优的地方）。

### 33.8 VM 能否完全替代 Controller

| Controller 职责 | VM 替代？ |
|---|---|
| 广播初始值 / 广播变化给 UI | ✅ 替代（FieldNotify 自动通知） |
| 从 ASC 取数、订阅变化 | ⚠️ 搬家到 VM，代码量没少 |
| 创建 / 注入参数（HUD 缓存） | ⚠️ 换成 widget 持有 VM + 自己 Init，还是得有人喂 |
| 业务操作（如 `UpgradeAttribute`） | ⚠️ 变成 VM 方法（按钮点击 → VM 方法 → 调 ASC） |

**结论**：纯 MVVM 可行，Controller 这个"中间人角色"退场，但活全搬进 VM——省的是"手动 Broadcast"那段，代码总量没少。也可混用（Controller 管业务/取数，VM 只管 UI 属性）。选择依据：**多 Widget 共享数据** → Controller（HUD 缓存）占优；**UI 自动刷新、少样板** → MVVM 占优。

## 三十四、SpellMenu 技能解锁：为什么判断放 C++，而不是把 Level 传蓝图

### 34.1 一句话结论

技能解锁 = **ASC 真正 `GiveAbility`（创建能力 spec）+ 状态标签挂到 spec**，不是"改一下显示"。
判断和授权都在 C++ 做，状态随 `AbilityInfo` 委托广播成"成品"给 UI；蓝图只读状态渲染，从不参与"能不能解锁"的判断。

### 34.2 教程完整链路

```cpp
// 升级入口（AuraCharacter::AddToLevel）
AuraPlayerState->AddToLevel(Level);                  // ① 广播 level → Overlay 显示
AuraASC->UpdateAbilityStatuses(Level);               // ② 解锁判断（关键）
```

```cpp
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)   // 遍历 DataAsset
    {
        if (!Info.AbilityTag.IsValid()) continue;
        if (Level < Info.LevelRequirement) continue;   // 等级门槛
        if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)  // 前置/已拥有检查
        {
            FGameplayAbilitySpec Spec(Info.Ability, 1);
            Spec.DynamicAbilityTags.AddTag(Abilities_Status_Eligible);  // 状态挂 spec
            GiveAbility(Spec);                    // 真给能力
            ClientUpdateAbilityStatus(...);       // RPC 广播状态变化
        }
    }
}
```

```cpp
// SpellMenuWidgetController 绑 AbilityStatusChanged → 组装 → 广播成品
FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
Info.StatusTag = StatusTag;
AbilityInfoDelegate.Broadcast(Info);   // 蓝图只收这个，拿到就能渲染
```

### 34.3 为什么不把 Level 直接传蓝图（"两个委托方案"为什么不行）

**先给结论**：蓝图上"比较 level vs LevelUpRequirement"完全写得出来，但"解锁"绕不开 C++：

| 关卡 | 为什么必须 C++ |
|---|---|
| `GiveAbility` 不是 BlueprintCallable | 蓝图上没有"给技能"节点 |
| `DynamicAbilityTags` / `MarkAbilitySpecDirty` | 纯 C++，且只有 C++ 能保证状态复制到客户端 |
| 服务器权威 | 多人下给能力必须走 Server RPC 校验，蓝图无此通道 |
| 前置技能检查 | 要查 ASC 当前已拥有的 spec（`GetSpecFromAbilityTag`） |

既然授权必须写 C++ 包装函数，那"判断"顺手也放 C++，连包装函数都不用写——这就是教程省掉一切蓝图层样板的原因。

**"两个委托先后"问题的本质与解法**：
- 蓝图上两个委托（Level、AbilityInfo）各自独立触发，没有"两个都到了再合并"的原生机制。
- 解法是**存变量 + 幂等刷新**：两个事件各自写入自己的变量，然后都调用同一个 `Refresh` 函数；`Refresh` 永远读两个变量的最新值。缺数据的刷新只是"空转"，最后一次刷新必然数据齐全，结果正确。
- 但蓝图手搓这个要自己处理：Map 按 tag 去重（`OnAbilityInfo` 会重播）、Level 未到时的中间态（加 `bHasLevel` 守卫）——全是 C++ 里免费的东西。

**更根本的三条原因**：
1. **状态是共享单一事实源**：`StatusTag` 存在 spec 的 DynamicAbilityTags，被输入绑定、装备/花点逻辑、Overlay 图标、SpellMenu 多处读。蓝图本地算的是"副本"，装备一次就脱节。
2. **联机一致性**：DynamicAbilityTags 随 ability spec 复制到所有客户端，服务器授权 → 状态复制 → 每端渲染同一真值。蓝图本地算则每端各算一遍，本机可能显示"已解锁"但服务器还没授权。
3. **UI 是视图不是逻辑层**：WidgetController 模式 = "数据下推、事件上抛"，UI 只渲染 Controller 广播的结果。

### 34.4 我的 My_ 版对应实现（已完成）

`My_` 版照教程补完两块：
1. `UMy_AuraAbilitySystemComponent` 新增 `UpdateAbilityStatuses(int32 Level)`（遍历 `AbilityInfo` DataAsset，等级达门槛且未拥有的能力 `GiveAbility` 并挂 `Status.Eligible`，再 `ClientUpdateAbilityStatus` RPC 广播）；配套 `GetSpecFromAbilityTag`（查前置/已拥有）、`GetStatusTagFromAbilitySpec`
2. 升级入口 `AAura_Character::AddToPlayerLevel_Implementation` 在 `AddToLevel` 后调用 `UpdateAbilityStatuses(IMy_CombatInterface::Execute_GetPlayerLevel(this))` 触发解锁

配套改动（一次到位）：
- `FMy_AuraAbilityInfo` 新增 `StatusTag` / `LevelUpRequirement` / `AbilityClass` 字段
- `My_AuraWidgetController::BroadcastAbilityInfo` 广播时补上 `StatusTag`，Overlay/SpellMenu 共用同一广播
- `SpellMenuWidgetController::BindCallbacksToDependencies` 绑 `OnAbilityStatusChanged` → 组装 info → 广播 `OnAbilityInfo`，菜单行自动刷新
- 新增 `My_Abilities.Status.*`（Locked/Eligible/Unlocked/Equipped）与 `My_Abilities.Type.*`（Offensive/Passive/None）标签组，含 Lighting.Electrocute / HitReact
- 蓝图：`My_DA_AbilityInfo` 配 `LevelUpRequirement`，GameMode 挂 `AbilityInfo`，新增 Electrocute GA、SpellMenu/SpellGlobe UI 资产，Fire 相关蓝图移入 `Ability/Fire/` 子目录
- 启动能力（`AddCharacterAbilitiesFromASC`）现直接挂 `Status.Equipped`，Overlay 图标正常显示

### 34.5 升级 → SpellMenu 技能图标：完整数据流（My_ 实现）

一条从"打怪得经验"到"技能树图标解锁"的完整链路，分 6 步，服务器/客户端分工清晰。

**① 触发升级（服务器，AttributeSet）**
`My_AuraAttributeSet::PostGameplayEffectExecute` 结算 IncomingXP → 算得 `NumLevelUp > 0` → `IMy_PlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUp)`

**② 角色入口（服务器，Character）**
`AAura_Character::AddToPlayerLevel_Implementation`：
- `AuraPlayerState->AddToLevel(N)` → `Level += N`，`OnLevelChanged` 广播 → Overlay 等级数字刷新（`Level` 是 `ReplicatedUsing = OnRep_Level`，客户端 OnRep 也刷）
- `AuraASC->UpdateAbilityStatuses(IMy_CombatInterface::Execute_GetPlayerLevel(this))` — 传**升级后的总等级**（不是差值）

**③ 解锁判断（服务器，ASC）**
`UMy_AuraAbilitySystemComponent::UpdateAbilityStatuses(Level)`：
- `UMy_AuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())` 从 GameMode 拿 `AbilityInfo` DataAsset
- 遍历 `AbilityInformation`：`AbilityTag` 无效 / `AbilityClass` 空 / `Level < LevelUpRequirement` → `continue`
- `GetSpecFromAbilityTag(AbilityTag)` 为 `nullptr`（还没拥有）→ 进入解锁
- `GiveAbility(Spec)` 挂 `Status.Eligible` — **这是"解锁"的本体**，不是改显示
- `ClientUpdateAbilityStatus(AbilityTag, Eligible)` — Client Reliable RPC 跨端

**④ RPC 跨端（客户端）**
`ClientUpdateAbilityStatus_Implementation` → `OnAbilityStatusChanged.Broadcast(AbilityTag, StatusTag)`

**⑤ WidgetController 组装（客户端）**
`SpellMenuWidgetController` 在 HUD `GetSpellMenuWidgetController()` 懒加载时 `BindCallbacksToDependencies()` 已绑 lambda：
- `FindAbilityInfoFromTag(AbilityTags)` → 取完整 info 结构
- `info.StatusTag = StatusTag` — 用广播来的状态覆盖
- `OnAbilityInfo.Broadcast(info)` — 广播"成品"，蓝图直接收

**⑥ UI 蓝图渲染（客户端）**
WBP 技能行绑 `OnAbilityInfo` → 按 `StatusTag` 决定图标显示（锁定 / 可学 / 已装备）

**两条广播路径（为什么蓝图零改动）**：
- 初始：菜单打开 → `BroadcastInitiaValues` → `BroadcastAbilityInfo` → `ForEachAbility` 逐条组装（含 StatusTag）→ 广播 → 初始状态正确
- 动态：升级解锁 → ③④⑤ → 只广播变化的那条技能
- 两条都走 `OnAbilityInfo` 同一个委托，蓝图只需绑一次

**职责划分**：
| 端 | 角色 | 做的事 |
|---|---|---|
| 服务器 | 判断 + 授权 | 算等级、判门槛、`GiveAbility`、挂状态、发 RPC |
| 客户端 | 接收 + 渲染 | 收 RPC、绑委托、组 info、刷图标 |

### 34.6 一句话总结

**判断在 C++、授权在 C++、状态作为 spec 的一部分广播出去、UI 只负责显示**——逻辑层决定"是什么"，表现层只显示"显示什么"。所有"两个委托先后 / 蓝图 giveAbility / 多端一致"的困惑都源于想把这个职责上移到 UI，教程把它留在 C++ 后一切自然消失。

---

## 三十五、SpellMenu 点击技能球 → 按钮 Enable：ASC/PS 复制时序与缓存策略

> 2026-08-30，SpellMenu 功能第二阶段：点击技能球后，根据"技能状态 + 法术点数"决定 **花点 / 装备** 两个按钮是否可用。

### 35.1 功能流程

点击技能球 → `SpellGlobeSelected(AbilityTag)`（BlueprintCallable）：

1. 取当前法术点数 `GetAuraPS()->GetSpellPoint()`
2. 判断技能状态：
   - `bTagValid = AbilityTag.IsValid()`
   - `bTagNone = AbilityTag.MatchesTag(My_Abilities_None)` — 占位"空"tag
   - `Spec = GetSpecFromAbilityTag(AbilityTag)`，`bSpecValid = Spec != nullptr`
   - 三者任一不成立 → **Locked**（技能没授予 / tag 无效）
   - 否则 → `GetStatusTagFromAbilitySpec(*Spec)` 从 Spec 动态标签读真实状态
3. 缓存 `SelectedAbility = { AbilityTag, StatusTag }`
4. `ShouldEnableButton(状态, 点数)` → 广播 `OnSpellGlobeSelect(bSpend, bEquip)`

### 35.2 按钮规则表

| 状态 | 花点按钮 | 装备按钮 |
|---|---|---|
| Locked | ✗ | ✗ |
| Eligible | ✓（点数 > 0） | ✗ |
| Equipped | ✓（点数 > 0） | ✓ |
| Unlocked | ✓（点数 > 0） | ✓ |

### 35.3 ★ 核心：ASC/PS 复制时序问题（为什么缓存状态、现读点数）

**问题**：按钮是否可用 = f(技能状态, 法术点数)，需要**同时**知道两个输入。而这两个值来自**两条独立的网络复制通道**：

- **StatusTag** → ASC（`AbilityStatusChanged` 回调）
- **SpellPoint** → PlayerState（`OnSpellPointChanged` 回调）

服务器 → 客户端复制时，**两者到达的时间 / 顺序不确定**（可能 Status 先到、点数后到，或反过来）。

**为什么状态必须缓存、点数可以现读**：

| | 能否随时现读 | 原因 |
|---|---|---|
| 点数 | ✅ 可以 | `GetAuraPS()->GetSpellPoint()` 是复制属性，客户端随时是最新值 |
| 状态 | ❌ 不行 | Locked（未授予）时**根本没有 Spec 可读**；已授予的 Spec 复制也有延迟 |

所以策略：

- **状态**：回调（`AbilityStatusChanged`）送来的就是权威值 → 存到 `SelectedAbility.StatusTag`
- **点数**：直接用 `GetAuraPS()->GetSpellPoint()` 现读，**不缓存**

**双回调重算**（`BindCallbacksToDependencies`）：

```
状态回调触发 → 更新缓存状态 → 用【新状态 + 现读点数】重算 → 广播
点数回调触发 → 广播新点数   → 用【缓存状态 + 新点数】重算 → 广播
```

任一回调触发，手里都有"另一边的最近已知值"→ 无论先后顺序，**最终都正确**（后到的回调会用两边最新值重算覆盖）。

### 35.4 一句话总结

**两个输入不同步到达 → 分别保存"最近一次已知值"，任一变化就用"新值 + 另一边缓存值"重算**。缓存的不是"旧错误值"，而是"当前最完整的信息"。

---

## 三十六、代码规范踩坑：委托 Signature / GameplayTag 空格 / 文件编码

### 36.1 委托类型命名规范（Signature 后缀）

- `DECLARE_DYNAMIC_MULTICAST_DELEGATE_*` 创建的**类型**统一加 `Signature` 后缀：`FMy_OnPlayerStateChangedSignature`
- 作用：区分**类型**（函数签名形状：参数 + 返回）与**实例**（`OnPlayerSpellPointChanged` 成员变量，真正 `Broadcast` 的事件）
- `DECLARE_*` 宏必须写全：类型名 + 参数（如 `TwoParams(FMy_XSignature, bool, a, bool, b)`），缺参数会编译报错
- 纯 C++ 委托（`DECLARE_MULTICAST_DELEGATE`）不暴露蓝图，加不加 Signature 都行，但统一加更整齐

### 36.2 GameplayTag 末尾空格坑

- 原生 tag 名**不能有末尾空格**：`FName("My_Abilities.Lighting.Electrocute ")` 会报
  `LogGameplayTags: Invalid tag ... Tag ends with space!`
- UE 会自动去掉空格替换，但会一直打错误日志 → 声明时就要检查结尾

### 36.3 文件编码规则（GBK vs UTF-8）

- 项目 C++ 文件编码**不统一**：`My_Character/` 是 UTF-8，多数其他文件是 GBK，少数是纯 ASCII
- **改文件前先检测编码**（用 Python 分别按 GBK / UTF-8 解码，看哪种出正常中文）
- **GBK 文件不能用 Edit / Write 工具**（会强制转 UTF-8 导致中文乱码）→ 必须 Python + `encoding='gbk'` + `newline=''`（保留 CRLF）
- UTF-8 / 纯 ASCII 文件可以直接用 Edit 工具，无乱码风险

### 36.4 Rider 断点不响排查

- 启动用 **bug 图标（Debug）**，不是绿色三角（Run 不挂调试器，断点永远不响）
- 断点圆点**实心红** = 已绑定；**空心灰** = 没绑上（Live Coding 热重载后常失效 → 完整 Rebuild + 重新打断点）
- 断点可能打在**未执行的分支**（如 if/else 里的 else），先放无条件执行的行验证
- 编译配置必须 **DebugGame Editor**（Development 会开优化 + 去调试符号）

---

## 三十七、Spec复制 vs ClientRPC时序：描述不能依赖spec + GameMode只在服务器

> 2026-08-30，SpellMenu 点击 Locked 技能 → 升级变 Eligible 后，host 描述不更新 / 远程客户端崩溃。属于 **GAS / UE 多人开发非常经典的坑**。

### 37.1 现象

| 端 | 现象 |
|---|---|
| 服务器 | 一切正常 |
| 远程客户端 | 点击 Locked 技能 → **崩溃**（`GetAbilityInfo` 返回 null 后解引用） |
| host（服务器+客户端同进程） | 升级后技能 Locked→Eligible，选中技能的描述**仍显示 Locked**（`OnAbilityStatusChanged` 触发了，但 `GetSpecFromAbilityTag` 为 null） |

### 37.2 两个根因

**根因 A：spec（复制属性）与 ClientRPC 到达次序不定**

升级流程：服务器 `GiveAbility(spec带Eligible)` → `ClientUpdateAbilityStatus(Eligible,1)`（Client RPC）
- **spec**：走复制属性（`FGameplayAbilitySpecContainer`），要等**下一个复制周期** → 晚到
- **ClientRPC**：服务器改了**立即发** → 先到

客户端收到 RPC 回调时，**spec 往往还没复制到** → `GetSpecFromAbilityTag` 返回 null。
旧代码用"spec 在不在"判断"是不是 Locked" → spec 没到就被误判为 Locked → 显示错误描述。
（host 上同进程也中招：本地 Client RPC 的处理点与 GiveAbility 的 spec"可见"点不同步。）

**根因 B：`GetAbilityInfo` 依赖 GameMode（服务器专属）**

```cpp
GetAbilityInfo() → UGameplayStatics::GetGameMode()   // GameMode 只在服务器创建
```
远程客户端没有 GameMode → 返回 null → 描述函数里 `info->...` **空指针崩溃**。
（单人 PIE standalone 时 server==client，GameMode 一直在，所以从没暴露——**联机才炸**。）

### 37.3 修复原则（改动点）

1. **判断状态用 StatusTag，不用"spec 是否存在"**：StatusTag 是服务器随 RPC 送来的**权威值**，spec 是"数据库"，慢半拍。
2. **数据源用 WidgetController 自己的 `AbilityDA`**（EditDefaultsOnly，两端都有），不再走 GameMode。
3. **取描述不要求 spec 就位**：spec 在 → 用它（等级准）；spec 没到 → `AbilityDA` 的 `AbilityClass` 的 **CDO** + RPC 带来的 `AbilityLevel` 取描述（`AbilitySpec->Ability` 本来就是该类的 CDO，结果一致）。

```cpp
// 改后 GetDescriptionByAbilityTag(AbilityTag, StatusTag, AbilityLevel, AbilityDA, ...)：
if (StatusTag != Locked)          // 已授予
{
    if (spec 存在) → spec->Level + GetDescription()
    else           → AbilityDA->AbilityClass CDO + AbilityLevel   // ★ host 空窗期走这里
}
else → Locked 描述（AbilityDA 取 LevelUpRequirement）              // 不再用 GameMode
```

### 37.4 这是 GAS/游戏设计里的常见问题吗？—— 是，经典中的经典

- **"RPC 先到、复制属性后到"**：UE 网络里可靠 RPC 与属性复制是**两条通道**，次序无保证。任何"服务器改状态 → RPC 通知客户端 → 客户端立刻读复制属性"的模式都可能踩。**通用规则：变化通知（RPC 参数）是那一刻的最新真值，持久状态（复制属性）要等同步——回调里用通知带的参数，别读还没同步的属性。**
- **"GameMode 只在服务器"**：`GetGameMode()` / `GetGameInstance()` 是**服务器/standalone 才有**的经典陷阱，凡是"客户端也要读配置数据"的功能都应把数据放在**两端都有的对象**（WidgetController/PlayerState 的 DataAsset 引用、或复制）。
- GAS 专属：**Ability 的授予（spec）是复制，不是预测**；预测只针对激活/执行。客户端不能假设"刚 GiveAbility 的 spec 立刻可读"。

### 37.5 一句话总结

**别拿"不稳定的 spec 到没到"去猜"稳定的状态"**——状态判断用 RPC 自带的权威参数，数据用两端都有的 AbilityDA，取描述不依赖 spec（CDO 兜底），问题自然消失。

## 三十八、C++ 中文字面量的多引号拼接坑（RichText 描述乱码终极定位）

> 2026-09-14。折腾最久的一个 bug，最终在**二进制层面**定位并验证。结论与「文件编码/BOM」无关。

### 38.1 现象

SpellMenu 里火球（焰矢）的技能描述，**第一行中文正常，从第二行起全是乱码**：

```
焰矢                     ← 正常
褰撳墠绛夌骇               ← 应该是「当前等级」
娑堣€楄摑閲               ← 应该是「消耗蓝量」
鍐峰嵈鏃堕棿               ← 应该是「冷却时间」
```

「正常一行 + 后面乱码」这个特征，是整条排查路的钥匙——它说明**不是编码整体错**，
而是某个边界之后的内容才错。

### 38.2 乱码指纹（先学会认它）

乱码 = **正确的 UTF-8 字节被按 GBK 解码**：

```python
"当前等级".encode('utf-8').decode('gbk')   # -> '褰撳墠绛夌骇'
"焰矢".encode('utf-8').decode('gbk')       # -> '鐒扮煝'
"发射".encode('utf-8').decode('gbk')       # -> '鍙戝皠'
"冷却时间".encode('utf-8').decode('gbk')   # -> '鍐峰嵈鏃堕棿'
```

反过来，如果看到的是 `???` 或 `�`（U+FFFD），那是**另一个**原因（按 ASCII/UTF-8 读 GBK 字节，
编译期就被替换成替换字符）。**两种乱码外观不同，根因也不同，别混为一谈。**

### 38.3 根因：相邻字符串字面量的多引号拼接

C++ 允许把相邻的字符串字面量自动拼接，很多人为了"排版整齐"这么写：

```cpp
// ❌ 出错写法：只有第一段是对的，第二段起全乱码
return FString::Printf(TEXT(
    "<Title>焰矢</>\n\n"
    "<Small>当前等级 </><Level>1</>\n\n"
    "<Small>消耗蓝量 </><ManaCost>%.1f</>\n\n"
    "<Small>冷却时间 </><CoolDown>%.1f</>\n\n"
    "<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </>"
    "<Damage>%d</>"
    "<Default> 点火焰伤害，并有几率使目标灼烧。</>\n\n"
    "<Small>升级后可同时发射更多焰矢。</>"),
    ManaCost, Cooldown, Damage);
```

```cpp
// ✅ 正确写法：一对引号，一行到底（内容完全一样）
return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>1</>\n\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>\n\n<Small>升级后可同时发射更多焰矢。</>"), ManaCost, Cooldown, Damage);
```

**注意**：按 C++ 标准，相邻字面量拼接是编译器的正常行为，理论上**不该**影响编码。
实测在这个工具链上（MSVC 14.34 + UE `/source-charset:utf-8`）就是会坏，且可稳定复现。
**机制暂无定论，但现象与规避方法都已用二进制证据钉死。**

### 38.4 决定性证据（对照实验）

同一文件、同样内容、同样编译参数，**唯一变量是引号对数**：

| 写法 | 引号对数 | DLL 里 `当前等级` | DLL 里 `褰撳墠绛夌骇` |
|------|---------|------------------|---------------------|
| 多行多对拼接 | 8 对 | 0 | **3** ❌ |
| 单行一对 | 1 对 | **3** | 0 ✅ |

顺带验证过的兜底方案：把中文全写成 `\uXXXX` 转义（源码纯 ASCII）也能正常编译，
但可读性差，不如"一对引号"。

### 38.5 排查过程中被排除的嫌疑（都是清白的）

| 嫌疑 | 检查结果 |
|------|----------|
| 文件编码 | UTF-8 + BOM，正确 |
| 编译器字符集开关 | `/source-charset:utf-8 /execution-charset:utf-8`，正确 |
| Rider 全局/项目编码 | 都是 UTF-8 |
| `.idea/encodings.xml` 单文件覆盖 | 无 |
| `.gitattributes` | 不存在 |
| UE `BuildConfiguration.xml` | 空 |
| 时间戳 / 陈旧产物 | `.obj`、DLL 都比源码新；删掉强制重编后乱码依旧 |

**教训**：一开始把力气全花在"编码设置"上，方向就错了。**"第一行正常"这个线索早就指向了字面量边界。**

### 38.6 二进制验证法（比肉眼看 UI 靠谱）

改完**编译后**直接查产物，不要靠眼睛看界面：

```python
data = open(r'Binaries/Win64/UnrealEditor-Aura-Win64-DebugGame.dll', 'rb').read()
print('正确:', data.count('当前等级'.encode('utf-16-le')))     # 期望 > 0
print('乱码:', data.count('褰撳墠绛夌骇'.encode('utf-16-le')))   # 期望 == 0
```

FString/TEXT 在 Windows 上是 **UTF-16LE**，所以要按 `utf-16-le` 编码后搜字节。

### 38.7 规矩（已写入 CLAUDE.md）

1. 一个 `TEXT("...")` 里放**全部**内容，`\n\n` 也写在引号内
2. 绝不为了"看着整齐"把字符串拆成多行多对引号
3. 文件仍是 UTF-8 with BOM + CRLF

### 38.8 顺带：两件容易和它混淆的事

**① 缺字形（豆腐块 `□`）≠ 编码错。**
同一个字符串里 `焰矢` 显示正常、别的字变 `□`，那是 **RichText 样式用的字体没有该字形**。
本例中 `My_DT_RichTextStyle` 里：

| 样式 | 字体 | 用途 |
|------|------|------|
| `Title` / `Default` / `Small` | 中文字体（站酷仓耳渔阳体） | 装中文 |
| `Damage` / `ManaCost` / `Level` / `CoolDown` / `Percent` / `Time` | `Amarante-Regular`（仅拉丁） | 只装数字 |

**数字样式不装中文是故意的**——所以如果中文被误放进 `<Damage>` 这类标签里，就会变豆腐块。

**② 编辑器工具提示的中文乱码**是 UE 自带的本地化资源问题（引擎 zh-Hans 翻译被按 GBK 解码），
与项目源码无关，不影响编译产物与运行。要消除可在
`编辑器偏好设置 → 通用 → 区域和语言 → 编辑器语言` 改回 English。

### 38.9 更彻底的方案（以后可选）

这次是**绕开**了编译器，但中文终究还在 C++ 源码里。正统做法是把描述文本搬进
**DataAsset / DataTable**（如 `My_AbilityInfo`）：

- 中文只存在于 `.uasset`，**完全不经过 C++ 编译器**，这类坑从根上消失
- 改文案不用重编译，编辑器里直接改
- 天然支持本地化（`FText` + 多语言表）

### 38.10 一句话总结

**中文 RichText 字符串必须写成一对外引号、一行到底；"第一行正常、后面乱码"就是多引号拼接的指纹，
用 DLL 里的 UTF-16LE 字节比对即可一秒定案。**

## 三十九、RPC 判断框架 + 复制机制 + 装备功能完整拆解

> 2026-09-16。起因：学 Equip 功能时被教程一堆函数绕晕，且不知道"什么时候该用 RPC"。
> 本章给出**可自己推导的判断框架**，而不是背模式。

### 39.1 最底层心智模型：权威副本 vs 本地副本

```
┌──────────────────────────────┐
│  服务器（Server）             │  ← 唯一可以改数据的地方（Authority）
│  【权威副本】                 │
└───────────┬──────────────────┘
            │ 复制（单向：服务器 → 客户端）
            ▼
┌──────────────────────────────┐
│  客户端（Client）             │  ← 数据只是【只读副本】
│  【本地副本】                 │
└──────────────────────────────┘
```

**唯一规则**：

> 客户端改自己的副本 = **白改**。下一次复制到达时会被服务器覆盖。
> 所以**任何要"留下来"的改动，都必须让服务器去做**。

**银行类比**：手机 App 显示余额 = 只读副本；要改余额必须发**请求**给银行（Server RPC），
银行改完再推送结果（复制 / 通知 RPC）。

**⚠️ 准确表述**：不是"客户端改不了"，而是"**客户端改了不算数**"。
技术上你可以在客户端本地改 `DynamicAbilityTags`（能编译、能跑、当时看起来生效），但：
不上传服务器、下次复制被覆盖、别的玩家看不到。

### 39.2 判断框架：四个问题，按顺序问

#### Q1：这个数据有"权威副本"吗？谁是 Authority？

| 数据在哪 | Authority |
|---|---|
| `AttributeSet`（血、蓝） | 服务器 |
| `ASC` 的 `AbilitySpec`（技能列表、DynamicTags） | 服务器 |
| `PlayerState` / `GameState` 上的东西 | 服务器 |
| 纯本地 UI 状态（菜单开没开） | **客户端自己** → 不需要 RPC |
| 音效音量、画质设置 | **客户端自己** → 不需要 RPC |

> 本项目事实：`My_AuraPlayerState.cpp:13-14` —— ASC 建在 **PlayerState** 上，且 `SetIsReplicated(true)`。

#### Q2：我是"改"还是"看"？

```
只读（显示、计算、判断）→ 直接读本地副本，不需要 RPC
要改                    → 必须走 Server RPC
```

**客户端可以放心"读"本地副本**（例：`EquipButtonPressed` 里读 `GetSlotFromAbilityTag`）。
**只有"写"需要 RPC。**

#### Q3：改完之后，谁需要【立刻】知道？

| 谁需要知道 | 用什么 |
|---|---|
| **没人急**（下次复制自然同步） | **什么都不用** ← 最常见 |
| **只有自己** | `Client` RPC |
| **所有人（含服务器）** | `NetMulticast` RPC |
| **需要"旧值/变化过程"** | 在通知里把旧值也带上（如 `PreviousSlot`） |

#### Q4：后进玩家 / 掉线重连也要正确吗？

```
要 → 数据必须走【复制】，不能只靠 RPC
```

**RPC 是"一次性事件"，复制是"持续状态"。**

经典坑：玩家 A 装备完（RPC 只发给 A）→ 玩家 B 中途加入 → B 收不到那个 RPC，
**只能靠属性复制同步到正确状态**。

### 39.3 Server RPC 的两种用途（很多人只知道第一种）

| 用途 | 含义 | 例子 |
|---|---|---|
| **① 请求改数据** | "帮我改" | `ServerEquipAbility`、`ServerUpgradeAttribute` |
| **② 上报信息** | "我这边发生了这个，你（服务器）不知道" | 开火请求、`ServerSetReplicatedTargetData` |

**服务器"看不到"客户端独有的东西**：

```
客户端独有：鼠标点击、按键输入、相机朝向、屏幕中心射线命中点、UI 点了哪个球
服务器知道：Pawn 位置/旋转（复制来的）、血量、已释放的技能
```

### 39.4 最实用的判断口诀

> ## 问：**服务器能自己算出来吗？**
> ```
> 能  → 不需要 RPC
> 不能 → 需要上报（Server RPC）
> ```

| 情况 | 服务器能自己算吗 | 需要 RPC 吗 |
|---|---|---|
| 陷阱踩到扣血 | ✅ 能 | ❌ 不要 |
| 敌人 AI 攻击玩家 | ✅ 能 | ❌ 不要 |
| 持续伤害（DOT） | ✅ 能 | ❌ 不要 |
| **Hitscan 打中谁 / 打在哪** | ❌ 不能 | ✅ 要上报 |
| **点击了哪个技能球** | ❌ 不能 | ✅ 要上报 |
| **按了什么键放技能** | ❌ 不能 | ✅ 要上报（**GAS 框架代劳**） |
| **敌人掉血结算** | ✅ 能（服务器端发生） | ❌ **完全不需要** |

### 39.5 ⚠️ 不是所有联机功能都要 RPC

| 功能 | 需要 Server RPC？ | 为什么 |
|---|---|---|
| 陷阱 / DOT / AI 攻击掉血 | ❌ | 服务器自己算 + 复制属性 |
| 敌人血条更新 | ❌ | `OnRep_Health` 自动触发 |
| 经验值增加 | ❌ | 复制属性 |
| 打开菜单 | ❌ | 纯本地 UI |
| 改技能槽 | ✅ | 信息在客户端（点了哪个球） |
| 加属性点 | ✅ | 要改复制数据（但**不需要 Client RPC**） |

### 39.6 通知机制的四种组合（Server RPC 的搭档）

| 模式 | 形式 | 适用 |
|---|---|---|
| **1. 只有 Server RPC** | 改复制属性，无人急 | `ServerUpgradeAttribute` |
| **2. Server + Client RPC** | 只自己要知道 + 要旧值 + 要立刻 | **装备技能**、切武器、拖背包 |
| **3. Server + Multicast** | 所有人要看 | 开门、开箱、范围特效、被动特效 |
| **4. 纯复制（无 RPC 通知）** | 血条、蓝条、经验 | — |

> 本项目里 `MulticastActivatePassiveEffect` 就是模式 3（`NetMulticast, Unreliable`），
> 因为**被动的粒子特效别人也该看到**。

### 39.7 自检清单："会不会被冲掉？"

写代码时问自己：

> **「如果我在这行后面直接改，服务器会不会把它冲掉？」**
> `会冲掉` → 必须 Server RPC；`不会` → 本地改就行

| 要改的东西 | 会被冲掉吗 | 怎么办 |
|---|---|---|
| `Health` / `Mana`（Replicated 属性） | ✅ 会 | Server RPC |
| `AbilitySpec.DynamicAbilityTags` | ✅ 会 | Server RPC + **MarkDirty** |
| `PlayerState` 上的复制变量 | ✅ 会 | Server RPC |
| 本地 UI 变量 / WidgetController 缓存 | ❌ 不会 | 直接改 |
| GA 内部状态（技能执行中） | ❌ 通常不会 | 直接改（GAS 预测管） |

### 39.8 为什么不能让客户端直接扣血（服务器权威的意义）

```
❌ 客户端算命中 → 客户端调扣血   → 改内存就能秒杀全场（作弊）
✅ 客户端上报 → 服务器【验证】→ 服务器结算
     服务器可检查：距离合理吗？在视野里吗？冷却好了吗？蓝够吗？目标能被伤害吗？
```

**CommitAbility 就是验证入口**：不够蓝 / 冷却没好 → 服务器取消执行，客户端预测被回滚。

### 39.9 一次开火的完整链路（标出每个 RPC 谁发的）

```
【客户端】                        【服务器】
    │ ① 左键按下 AbilityInputTagHeld   │
    │ ② TryActivateAbility ──────────▶ │ ★ RPC-1 激活请求（GAS 自动发）
    │    (本地也跑一遍 = 预测)          │ ③ CommitAbility 检查冷却/蓝量
    │ ④ 算屏幕中心射线 → 命中敌人       │
    │ ⑤ ServerSetReplicatedTargetData ▶│ ★ RPC-2 上报命中（GAS 提供接口）
    │                                  │ ⑥ 验证命中合法性
    │                                  │ ⑦ 施放 GE_Damage → Health -= 50
    │                                  │    ★ 这步【不需要 RPC】，直接改
    │◀──────── 属性复制 ───────────────┤ ⑧ Health 复制给所有客户端
    │ ⑨ OnRep_Health → 血条更新         │
```

**结论**：扣血不需要 RPC（服务器自己算的）；需要 RPC 的是"**我要开火**"和"**打中了谁**"。

> 本项目火球是**投射物**（服务器 SpawnActor + 复制），所以连"上报命中"都不需要 ——
> 投射物在两台机器上各自飞，服务器上碰撞到敌人就地结算。
> 项目里唯一的 TargetData RPC 是给"鼠标点击选目标"用的（`My_TargetDateFromMouse`），
> 因为"鼠标点在哪"只有客户端知道。

### 39.10 FastArray 复制 vs 普通复制属性（关键区别）

#### 真实源码事实（**曾一度搞错，特此更正**）

```cpp
// GameplayAbilitySpec.h:223-225
/** Optional ability tags that are replicated. */
UPROPERTY()                                    // ← 没有 NotReplicated！
FGameplayTagContainer DynamicAbilityTags;
```

**`DynamicAbilityTags` 是【复制】的。** 但 `FGameplayAbilitySpec` 继承自：

```cpp
// GameplayAbilitySpec.h:162
struct GAMEPLAYABILITIES_API FGameplayAbilitySpec : public FFastArraySerializerItem
```

#### FastArray 靠"脏标记"做增量复制

```cpp
struct FGameplayAbilitySpecContainer : public FFastArraySerializer
{
    UPROPERTY()
    TArray<FGameplayAbilitySpec> Items;      // 整个数组，FastArrayDeltaSerialize 复制
};
```

```
FastArray 增量复制：只发送【被标记为脏】的 Item，没标的跳过（省带宽）
你改了字段的【值】→ 脏标记【不会自动置位】
   因为引擎不比对内容，只认那个标记
```

#### 引擎实现

```cpp
void UAbilitySystemComponent::MarkAbilitySpecDirty(FGameplayAbilitySpec& Spec, bool WasAddOrRemove)
{
    if (IsOwnerActorAuthoritative())
    {
        // ServerOnly 的技能改动不需要同步（除非是添加/移除）
        if (!(Spec.Ability && Spec.Ability->NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly && !WasAddOrRemove))
        {
            bIsNetDirty = true;                        // ① 整个 ASC 需要网络更新
            ActivatableAbilities.MarkItemDirty(Spec);  // ② ★ 标记这个 Item 脏
        }
        AbilitySpecDirtiedCallbacks.Broadcast(Spec);
    }
}
```

**引擎文档注释直接写明**（`AbilitySystemComponent.h` 多处）：

```cpp
/** Returns an ability spec from a handle. If modifying call MarkAbilitySpecDirty */
FGameplayAbilitySpec* FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle Handle);
```

#### 两者对比

| | 改 `DynamicAbilityTags` | 改 `Health` |
|---|---|---|
| 数据在哪 | `AbilitySpec`（ASC / PlayerState） | `AttributeSet` |
| 是复制数据吗 | ✅ 是 | ✅ 是 |
| 服务器改要 RPC 吗 | ❌ 不要（Authority 直接写） | ❌ 不要 |
| **要手动标脏吗** | ✅ **要**（`MarkAbilitySpecDirty`） | ❌ **不要**（自动比对） |
| 客户端要通知吗 | ✅ 要（"只有自己关心"的事件） | ❌ 不要（绑 `OnRep` 就够） |

**一句话**：

```
普通复制属性    → 引擎【自动】比对前后值，变了就发      → 不用管
FastArray Item  → 引擎【不比对】，要你【手动标脏】才发  → 必须管
```

#### 验证方法（以后自己查）

```
1. 找结构体定义（如 GameplayAbilitySpec.h）
2. 看字段 UPROPERTY 有没有 NotReplicated
3. 看结构体是否继承 FFastArraySerializerItem
   → 是 → 改完要 MarkItemDirty

路径：D:\UE Engine\UE_5.2\Engine\Plugins\Runtime\GameplayAbilities\Source\
        GameplayAbilities\Public\GameplayAbilitySpec.h
```

### 39.11 装备（Equip）功能完整拆解

#### 为什么要两个 RPC

| 通道 | 职责 |
|---|---|
| `ServerEquipAbility`（Server RPC） | **请求改数据** —— 客户端没有 Authority |
| `MarkAbilitySpecDirty` | **保证数据最终一致** —— 后进玩家也正确 |
| `ClientEquipAbility`（Client RPC） | **让 UI 立刻知道 + 携带旧槽位信息** |

**为什么必须有 Client RPC**（三件事属性复制给不了）：

1. **`PreviousSlot` 拿不到** ← 最硬的理由。复制只给"最终状态"，不给"变化过程"。
   UI 要清掉旧槽的球，必须知道旧槽是哪个；否则得自己缓存旧状态 + 算 diff。
2. **时序**：RPC 当帧/下一帧到；属性复制要等 `NetUpdateFrequency` 周期（几十~上百 ms），UI 会"顿"一下。
3. **广播精确度**：RPC 只广播 2 条（旧槽清空 + 新槽装备）；靠复制只能全量重播。

**⚠️ 但 `ServerEquipAbility` 是绝对省不掉的**（客户端没 Authority，本地改会被覆盖）。
**能省的只有 `ClientEquipAbility`**，且仅在"纯单机 / 你是 Listen Server Host"时。

> 工业替代方案（Lyra）：不写 Client RPC，改为在 `OnRep_ActivateAbilities` 里 diff 新旧数组。
> 更健壮（丢包/重连/后进全对），但要写 diff 逻辑，对学习项目太重。

#### 函数按层分组

**第 1 组：ASC 查询辅助（纯读取）**

| 函数 | 作用 | 关键点 |
|---|---|---|
| `GetInputTagFromSpec(Spec)` | 读 Spec 的 InputTag | 遍历 `DynamicAbilityTags` 找 `InputTag` 前缀 |
| `GetSlotFromAbilityTag(Tag)` | 按技能查槽位 | **= `GetInputTagFromSpec`** |
| `GetStatusFromSpec(Spec)` | 读状态 Tag | 找 `Abilities.Status` 前缀 |
| `GetStatusFromAbilityTag(Tag)` | 按技能查状态 | = `GetStatusFromSpec` |
| `SlotIsEmpty(Slot)` | 这个槽有人占吗 | 遍历所有 Spec |
| `AbilityHasSlot(Spec, Slot)` | 这个技能占着**这个**槽吗 | `HasTagExact(Slot)` |
| `AbilityHasAnySlot(Spec)` | 这个技能**装备了**吗 | `HasTag("InputTag")` |
| `GetSpecWithSlot(Slot)` | 谁占着这个槽 | 返回 Spec 指针 |
| `IsPassiveAbility(Spec)` | 是被动技能吗 | 查 `AbilityInfo.AbilityType` |

**第 2 组：ASC 修改辅助**

| 函数 | 作用 | 实现要点 |
|---|---|---|
| `ClearSlot(Spec)` | 清掉**这个技能**的槽 | 读 InputTag → `RemoveTag` |
| `AssignSlotToAbility(Spec, Slot)` | 换键核心 | **`ClearSlot` 然后 `AddTag(Slot)`** |
| `ClearAbilitiesOfSlot(Slot)` | 清掉**这个槽**上的所有技能 | 遍历 → 调 `ClearSlot`（教程中未调用，预留） |

**第 3 组：ASC 装备主流程（RPC）**

| 函数 | 位置 | 作用 |
|---|---|---|
| `ServerEquipAbility(AbilityTag, Slot)` | `AuraASC.cpp:336` | 服务器权威：冲突处理 + 改 Tag + 标脏 |
| `ClientEquipAbility(Tag, Status, Slot, PrevSlot)` | `AuraASC.cpp:389` | 回客户端：广播 `AbilityEquipped` |

**第 4 组：SpellMenu UI**

| 函数 | 位置 | 触发 | 作用 |
|---|---|---|---|
| `EquipButtonPressed()` | `:117` | 点「装备」 | 进选槽模式 + 记录当前槽到 `SelectedSlot` |
| `SpellRowGlobePressed(SlotTag, AbilityType)` | `:131` | 点装备行槽位 | 类型校验 → `ServerEquipAbility` |
| `OnAbilityEquipped(...)` | `:142` | 收到装备完成 | **广播 AbilityInfo 给 UI** |
| `GlobeDeselect()` / `SpellGlobeSelected()` | `:102` / `:57` | 取消 / 点球 | 退出选槽模式 |

#### Slot 和 InputTag 是同一个东西

```cpp
// 教程：名字叫 Slot，实际就是拿 InputTag
FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
    if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
        return GetInputTagFromSpec(*Spec);   // ★ 直接转调
    return FGameplayTag();
}
```

**只是两个语境的命名约定**：

| 领域 | 用词 | 场景 |
|---|---|---|
| 输入绑定 | `InputTag` | "按哪个键" → `AbilityInputTagHeld` |
| UI 槽位 | `Slot` | "显示在哪个格子 / 能不能塞进去" → `SlotIsEmpty` |

**为什么分开命名**：读 `AbilityHasSlot` / `AbilityHasAnySlot` 比
`AbilityHasInputTag` / `AbilityHasAnyInputTag` 更能表达"装备了没"这个语义。

#### 装备完整时序

```
① 点「装备」按钮
   EquipButtonPressed()
     ├─ AbilityType = AbilityInfo[火球].AbilityType
     ├─ WaitForEquipDelegate.Broadcast(AbilityType)   → UI 高亮那一排
     ├─ bWaitingForEquipSelection = true
     └─ 若已装备 → SelectedSlot = GetSlotFromAbilityTag(火球)

② 点装备行的「按键3」格
   SpellRowGlobePressed(SlotTag=InputTag.3, AbilityType=Offensive)
     ├─ if (!bWaitingForEquipSelection) return;        ← 不在选槽模式就忽略
     ├─ if (技能类型 != 槽的类型) return;               ← 主动不能装被动槽
     └─ ServerEquipAbility(火球, InputTag.3)           ← Server RPC

③ 服务器 ServerEquipAbility_Implementation(火球, InputTag.3)
     ├─ PrevSlot = InputTag.1                          ← 记下旧槽
     ├─ 校验 Status 是 Equipped / Unlocked
     ├─ SlotIsEmpty(InputTag.3)?
     │     └─ 若被占：同一技能？→ 直接返回；否则 ClearSlot(占位者)
     ├─ if (!AbilityHasAnySlot(火球)) → Unlocked 改成 Equipped
     ├─ AssignSlotToAbility(火球, InputTag.3)           ← ★ 真正换键
     │     └─ ClearSlot(火球) → AddTag(InputTag.3)
     ├─ MarkAbilitySpecDirty(火球)                      ← 触发属性复制
     └─ ClientEquipAbility(火球, Equipped, InputTag.3, InputTag.1)

④ 客户端 ClientEquipAbility_Implementation
     └─ AbilityEquipped.Broadcast(火球, Equipped, InputTag.3, InputTag.1)

⑤ SpellMenu 的 OnAbilityEquipped 收到          ← ★★ 广播给 UI 在这里
     ├─ Broadcast(旧槽 = 空: AbilityTag=None, InputTag=InputTag.1)
     │      → UI 把「按键1」的球拿走
     ├─ Broadcast(新槽: AbilityTag=火球, InputTag=InputTag.3, Status=Equipped)
     │      → UI 在「按键3」放上火球的球
     ├─ StopWaitingForEquipDelegate.Broadcast(Offensive)  → 播放取消选择动画
     ├─ SpellGlobeReassignedDelegate.Broadcast(火球)      → 通知球重排
     └─ GlobeDeselect()                                   → 清空选中
```

#### "为什么改了 InputTag，球的显示位置就变了"

> **不是 UI 主动挪球，而是：服务器改了数据 → 通知客户端 → 客户端广播新的 AbilityInfo → UI 用新数据重画。**

**两个可能的触发路径**：

| 路径 | 谁触发 | 特点 |
|---|---|---|
| **A. 精确广播**（教程采用） | `OnAbilityEquipped` 手动广播 2 条 | 立刻、带旧槽信息 |
| **B. 全量重播** | 属性复制 → `OnRep_ActivateAbilities` → `BroadcastAbilityInfo()` | 有延迟、拿不到旧槽 |

教程选 A：**`ClientEquipAbility` 的四个参数里直接带着 `Slot` 和 `PreviousSlot`**，
所以客户端立刻就能广播，不用等属性复制。

### 39.12 一句话总结

**判断要不要 RPC，只问一句："服务器能自己算出来吗？"**
能算 → 直接改复制属性（不用 RPC）；不能算 → Server RPC 上报/请求。
改完要不要额外通知，看"谁需要立刻知道"：没人/自己/所有人 ⇒ 无 / Client / Multicast。
**而改 FastArray 里的东西（如 `DynamicAbilityTags`），无论走哪条路，都必须 `MarkAbilitySpecDirty`。**

## 四十、装备功能 My_ 版实现 + 缓存策略 + 代码复查

> 2026-09-16。第三十九章讲的是"教程为什么这么设计"，本章记**自己动手实现时踩到的具体问题**。

### 40.1 我的实现与教程的对应关系

| 教程 | 我的实现 | 位置 |
|---|---|---|
| `EquipButtonPressed` | `EquippedButtonPressed` | `My_SpellMenuWidgetController.cpp:187` |
| `SpellRowGlobePressed` | `EquipSpellRowGlobePressed` | `:201` |
| `OnAbilityEquipped` | `OnAbilityEquipped` | `:210` |
| `ServerEquipAbility` | `ServerEquipAbility_Implementation` | `My_AuraAbilitySystemComponent.cpp:291` |
| `ClientEquipAbility` | `ClientEquipAbility_Implementation` | `:315` |
| `ClearSlot` / `ClearAbilitiesOfSlot` / `AbilityHasSlot` | 同名 | `:320` / `:327` / `:339` |

**术语对照**：我用 `AbilityDA->FindAbilityInfoFromTag(Tag)`，教程用 `AbilityInfo->FindAbilityInfoForTag(Tag)`
（只是 DataAsset 成员名不同）。

### 40.2 缓存 vs 现读：判断标准

#### 结论

```
缓存什么？ → 【稳定标识】：AbilityTag、InputTag（选中的是谁、绑在哪个键）
现读什么？ → 【易变状态】：StatusTag（Locked/Eligible/Unlocked/Equipped）
```

#### 怎么判断"会不会变"

```cpp
SelectedAbility.Ability                                   // 缓存（选中后不会变）
SelectedAbility.Status                                    // 缓存 + 关键处现读（会变）
GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.AbilityTag)   // 现读
GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag)    // 现读
```

#### ⚠️ 一个容易讲错的点

`SelectedAbility.Status` **不是"随时可能过期"的危险缓存** —— 它有维护机制：

```cpp
// SpellMenuWidgetController.cpp:20-22
if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
{
    SelectedAbility.Status = StatusTag;      // 状态一变就同步
}
```

**正常流程下（点球→花点→点装备），缓存是跟得上的。**

**所以现读的真正理由是**：

1. **不依赖任何约定** —— 不用追"缓存在哪被改、有没有漏改"
2. **收益大于成本** —— 缓存易变值只省一次 O(n) 遍历（技能就十几个），但增加维护心智负担
3. **一致性** —— "它装备了吗"和"它占哪个槽"这两句应该从**同一份来源**读

> **准确表述**：不是"缓存会坏"，而是"易变数据现读更省心、更自洽"。

### 40.3 发现：`SelectedSlot` 是死变量

#### 证据

```
SpellMenuWidgetController.h:70      声明
SpellMenuWidgetController.cpp:127   唯一的赋值
                                     → 全项目再无任何读取
```

**教程和我自己的实现都是"只赋值、从不读"。** 全项目 grep（`Source` + `Content`）确认无第二处引用。

#### 为什么教程会写它

排查了 `ServerEquipAbility` 每条分支，**确认它完全不依赖 `SelectedSlot`**：

```cpp
const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);   // 服务器自己查旧槽
```

**服务器自己就能查出旧槽，不需要 UI 告诉它。**

而"点同一槽取消装备"这个功能，**服务器端逻辑已经覆盖**（`SlotIsEmpty` + 同技能判断），
也不需要 UI 传 `SelectedSlot`。

> **推测**：教程作者早期版本想做 UI 层的同槽处理，后来服务器逻辑覆盖了，变量忘删。

#### 什么情况下它才有用

| 用途 | 说明 |
|---|---|
| **点当前槽 = 真正卸下技能** | 需要新增 `ServerUnequipAbility`，并在 `EquipSpellRowGlobePressed` 里判断 `SlotTag == SelectedSlot` |
| **UI 高亮"当前装备的槽"** | 其实也不需要 —— `OnAbilityInfo` 广播的 `InputTag` 里已经有这个信息 |

**决定**：先保留（加注释说明是预留），不删。

### 40.4 代码复查发现的 5 个问题

#### 🔴 问题 1：少了「同槽同技能 → 提前返回」

教程有这段（`AuraAbilitySystemComponent.cpp:355-360`），我的实现没有：

```cpp
if (SpecWithSlot)
{
    if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
    {
        ClientEquipAbility(AbilityTag, Equipped, Slot, PrevSlot);
        return;                        // 占着这个槽的就是同一个技能 → 什么都不做
    }
    ...
}
```

**后果**：火球已在槽1，再点一次槽1 → 会 `ClearSlot` + 重新 `AddTag`，
**白折腾一遍并重播动画**。

#### 🟠 问题 2：清槽顺序导致"自己清自己"

```cpp
ClearAbilitiesOfSlot(Slot);   // ← 槽里如果是火球自己，这里已经把它清了
ClearSlot(AbilitySpec);       // ← 再清一次（空操作，冗余）
AbilitySpec->DynamicAbilityTags.AddTag(Slot);
```

**且这个顺序让"同技能判断"没法插入**（要判断时 `AbilitySpec` 已经没槽了）。

**教程的写法**：先判断同技能 → 再处理占位者 → 最后 `AssignSlotToAbility`（内部 `ClearSlot` + `AddTag`）。

#### 🟡 问题 3：`bStatusValid` 为 false 时仍然广播

```cpp
if (bStatusValid) { ...改数据... }
ClientEquipAbility(..., Equipped, ...);   // ← 没改成功也通知"已装备"
```

教程也有这个毛病。更稳的写法是把 `ClientEquipAbility` 放进 `if (bStatusValid)`。

#### 🟡 问题 4：`AbilityHasSlot` 的注释写错了

```cpp
// 装备了没（任意槽）        ← 错
bool AbilityHasSlot(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot);
```

实现是 `Tag.MatchesTagExact(Slot)` —— **"占着【指定的这个】槽吗"**。

**"装备了没"是另一个函数**（教程 `AbilityHasAnySlot`，判断有没有任意 `InputTag` 前缀的 Tag）。

#### 🟡 问题 5：`OnAbilityEquipped` 少了 `GlobeDeselect()`

教程末尾有 `GlobeDeselect()`（`SpellMenuWidgetController.cpp:162`），作用是
**清空 `SelectedAbility` + 广播"取消选中"（右侧描述面板清空）**。

缺了这一步 → 装备完成后菜单可能仍显示上一个技能的选中态。

### 40.5 踩坑：`edit` 工具会吞掉 UTF-8 BOM

**现象**：用 `edit` 工具改完 3 个 UTF-8+BOM 的 C++ 文件后，BOM 消失了：

```
HEAD 版本  前3字节 = 239,187,191  (有 BOM)
改后       前3字节 =  47, 47, 32  (//)  ← BOM 没了
```

**影响**：内容本身没坏（中文注释是合法 UTF-8，能编译），
但**无 BOM 的 UTF-8 文件在中文 Windows 上被编辑器保存时可能回退成 GBK** → 又掉进编码坑。

**教训**：

> **用 `edit` 类工具改 UTF-8+BOM 文件后，必须验证前 3 字节。**

**验证命令**（只读）：

```powershell
$b = [IO.File]::ReadAllBytes('文件路径')
if ($b[0] -eq 239 -and $b[1] -eq 187 -and $b[2] -eq 191) { '有 BOM' } else { '无 BOM' }
```

**修复**（只加 3 字节，内容不动）：

```powershell
$b = [IO.File]::ReadAllBytes($f)
[IO.File]::WriteAllBytes($f, [byte[]](239,187,191) + $b)
```

**为什么是"有时候会"**：同样用 `edit` 改过的 `My_AuraAbilitySystemLibrary.cpp` BOM 没丢 ——
所以**不能靠运气，每次改完都要查**。

### 40.6 输入链路：DataAsset → 委托 → ASC 匹配

> 起因：改完 InputTag 后，发现这个 GA 的**触发按键真的换了**。这里把机制讲透。

#### 核心认知：`InputTag` 是"按键"和"技能"之间的唯一纽带

```
物理按键  ──[输入配置表，固定]──▶  InputTag  ──[DynamicAbilityTags 匹配]──▶  技能
   ↑                                 ↑                                  ↑
固定不变                    装备系统在这里做文章                  谁带着这个标签谁响应
```

**改 `InputTag` = 把纽带的另一端从旧槽解开、接到新槽上。**

#### 三段式链路（绑定阶段 / 触发阶段 / 匹配阶段）

**① 绑定阶段**（`SetupInputComponent` 时执行一次）

```cpp
// My_AuraEnhancedInputComponent.h:26-48
for (auto& Temp : InputConfig->InputActionToTags)
{
    if (Temp.InputAction && Temp.InputTag.IsValid())
    {
        BindAction(Temp.InputAction, ETriggerEvent::Started,   Object, PressedFunc, Temp.InputTag);
        BindAction(Temp.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Temp.InputTag);
        BindAction(Temp.InputAction, ETriggerEvent::Triggered, Object, HeldFunc,    Temp.InputTag);
    }
}
```

**关键点**：`BindAction` 的**最后一个参数 `Temp.InputTag` 是"预绑定"到回调上的值（Payload）**。

```
键盘 IA_3 ──绑定──▶ 委托（把 "InputTag.3" 固化在里面）──▶ 你的函数
```

**⚠️ 精确表述**：Tag 不是"装在键盘上"，也不是"装在函数上"，而是**装在委托上、随调用一起传进去**。

**为什么这个区别重要**：`AbilityInputTagHeld` **只有一个实例**，所有按键绑的都是它：

```
IA_1 ─┐
IA_2 ─┤
IA_3 ─┼─▶ 全部指向同一个 AbilityInputTagHeld，只是各自携带不同的 Tag
IA_4 ─┤
IA_5 ─┘
```

**② 触发阶段**

```
按下数字键 3 → IA_3 触发 ETriggerEvent::Started
            → 委托执行 → AbilityInputTagPressed( "InputTag.3" )
```

三种 `ETriggerEvent` 的区别：

| ETriggerEvent | 何时触发 | 绑给谁 |
|---|---|---|
| `Started` | 刚按下（**一次**） | `AbilityInputTagPressed` |
| `Triggered` | 按住期间（**每帧**） | `AbilityInputTagHeld` |
| `Completed` | 松开（**一次**） | `AbilityInputTagReleased` |

```
按下 ──── Started（一次）
  │
  │  ← 按住期间 Triggered 每帧都调（按住 1 秒 ≈ 60 次）
  │
松开 ──── Completed（一次）
```

> **不是"一次按键触发 3 个函数"**，而是**三个不同时机各自触发各自那个函数**。
> 且三个都是**可选**的（`if (PressedFunc)` 才绑），纯瞬发技能不需要 `Held`。

**③ 匹配阶段**（GAS 找技能）

```cpp
// My_AuraAbilitySystemComponent.cpp:88-103
void UMy_AuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))   // ★ 精确等号匹配
        {
            AbilitySpecInputPressed(AbilitySpec);
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}
```

**语义**：**"谁的 `DynamicAbilityTags` 里正好有这个 Tag，就激活谁。"**

`HasTagExact` 是**完全相等**，不是前缀匹配。
理论上两个技能带同一个 Tag 会一起激活，但装备系统保证"一个槽只有一个技能"，所以实际一对一。

#### 匹配之前还有一道"路由门禁"（在 Controller 里）

```cpp
// My_Aura_Controller.cpp:153-159
void AMy_Aura_Controller::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(...My_InputTag_LMB))   // 不是左键
    {
        GetAuraASC()->AbilityInputTagHeld(InputTag);     // 直接转发给 ASC
        return;
    }
    // 是左键 → 还要判断"在瞄准 / 按着 Shift"，否则走点击移动
    if (bTargeting || bShiftKeyDown) { ...激活... }
    else { ...点击移动... }
}
```

**所以完整顺序是**：

```
按键 → Tag
   ↓
① Controller 路由：是不是 LMB？要不要走移动逻辑？        ← 业务分流
   ↓
② ASC 匹配：谁的 DynamicAbilityTags 精确等于这个 Tag？   ← GAS 技能查找
   ↓
③ 激活
```

#### 实战推演：装备前后按键行为的变化

```
【装备前】火球带 My_InputTag.1
    按【数字键1】→ IA_1 → 参数 = InputTag.1
        → ASC 遍历：谁有 InputTag.1？→ 火球有 → 激活 ✅
    按【数字键3】→ 参数 = InputTag.3 → 没人有 → 什么也不发生 ❌

【装备后】火球带 My_InputTag.3（旧的 InputTag.1 被 ClearSlot 清掉了）
    按【数字键1】→ 参数 = InputTag.1 → 没人有 → 什么也不发生 ❌
    按【数字键3】→ 参数 = InputTag.3 → 火球有 → 激活 ✅
```

**这就是"改 InputTag 后触发按键跟着变"的完整解释。**

#### 为什么同一个 `InputTag` 是"一物三用"

| 用途 | 体现在哪 |
|---|---|
| **执行**：按哪个键触发 | `AbilityInputTagHeld` 的 `HasTagExact` 匹配 |
| **存储**：技能绑在哪个键 | `AbilitySpec.DynamicAbilityTags` |
| **显示**：UI 上球放哪个格子 | `AbilityInfo.InputTag` → `OnAbilityInfo` 广播 |

**三者共用同一个 Tag，所以改一处 → 三处同步变化。**
这就是"改了 InputTag，UI 位置和按键**同时**变了"的原因。

#### 为什么客户端也必须 `MarkAbilitySpecDirty`

`AbilityInputTagHeld` **在客户端本地执行**（客户端要能立刻响应按键）

```
服务器上：火球在槽3
客户端本地（复制没到）：火球还在槽1
→ 按数字键1，客户端本地匹配上了 → 错误地激活火球（预测）
→ 服务器："你槽1没技能" → 拒绝 → 回滚 → 手感抽搐
```

**所以 `MarkAbilitySpecDirty` 保证的"数据同步"，直接决定按键匹配是否与服务器一致。**

#### 两张表配合，决定"哪个键触发哪个技能"

| 表 | 在哪 | 决定什么 |
|---|---|---|
| **输入配置表** | `My_AuraInputConfig` DataAsset（{InputAction, InputTag} 数组） | 物理键 ↔ InputTag |
| **StartUpInputTag** | 每个技能的 GA 蓝图里 | 技能初始绑哪个 InputTag |

```
技能蓝图 StartUpInputTag = My_InputTag.1     ← 决定初始装在哪（装备系统可改）
输入配置表  键盘3 ←→ My_InputTag.3           ← 决定哪个键触发哪个 Tag
```

**自己验证的三步**：

1. 打开 `My_AuraInputConfig` DataAsset → 看清键 ↔ Tag 的表
2. 打开火球的 GA 蓝图 → 看 `StartUpInputTag`
3. 进游戏：装备火球到槽3 → 按 1（无效）→ 按 3（触发）

### 40.7 本章一句话总结

**装备功能实现本身不难（服务器改 Tag + 两个 RPC + 广播两条 Info），
难的是"为什么这么设计"和"缓存/现读怎么选"。
而 `SelectedSlot` 这种"只写不读"的教程遗留物，提醒我们：
读教程代码时要主动验证每个变量的真实用途，不要假设它有用。**

---

## 四十一、异步节点与委托：C++ 怎么通知蓝图（深度版）

> 第二十一章讲了「怎么用」，本章讲「**为什么长这样**」。
> 看完本章，任何 `UBlueprintAsyncActionBase` 节点（包括引擎自带的 `UAbilityAsync_*` 系列）都能一眼看懂。

### 41.1 一个类比：异步节点 = 雇了一个临时工

把整个 `UMy_WaitCoolDownChange` 想象成**一个临时工**：

| 代码 | 类比 |
|---|---|
| `UMy_WaitCoolDownChange` 这个类 | 一个**临时工** |
| 工厂函数 `WaitCoolDownChange` | **雇佣手续**（造出这个临时工） |
| 成员 `ASC` / `CoolDownTag` | 交代他「盯谁的、盯哪个冷却」 |
| 白箭头 `Cool Down Start` / `Cool Down End` | 你留下的**电话号码** |
| `CoolDownStart.Broadcast(...)` | 临时工**打电话通知你** |
| 蓝色 `Async Task` 脚 + `EndTask()` | **解雇**他（`Async Task` 脚就是"他本人"） |

**整个类只干一件事：替你去听一个"蓝图绑不到"的事件，有情况再通知蓝图。**

### 41.2 为什么要雇这个临时工 —— 蓝图有两道墙翻不过去

待监听的两件事（冷却 GE 被加上 / 冷却 Tag 数归零）都发生在 **ASC** 里，而 ASC 的委托是**原生委托**：

```cpp
// GameplayEffectTypes.h:940
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayEffectTagCountChanged, const FGameplayTag, int32);
//      ^^^^^^^^^^^^^^^^^^^^^^ 没有 DYNAMIC

// AbilitySystemComponent.h:104
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAppliedDelegate, UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle);
//      ^^^^^^^^^^^^^^^^^^^^^^ 同样没有 DYNAMIC
```

| 墙 | 内容 |
|---|---|
| **墙 1** | **原生委托对蓝图完全不可见** —— 它不是 `UPROPERTY`、没有对应 UFunction、不是 `BlueprintAssignable` |
| **墙 2** | **就算是 dynamic 也没用** —— 蓝图只能绑「对象身上的委托**属性**」，绑不了「某个函数的**返回值**」。而 `RegisterGameplayTagEvent` 返回的恰恰是 `FOnGameplayEffectTagCountChanged&` |

> ⚠️ **措辞要准确**：不是「不写异步节点不行」，而是「**蓝图没法直接绑 ASC 的委托**」。
> 你也可以在自己能改的类上写一个「转发委托」绕过去 —— 见 41.10 路线 B。

**另一个容易搞错的点**：不要以为「技能触发」就等于「冷却开始」。这两件事根本不是一回事 ——

- 技能可能**释放失败**（蓝不够、被打断）
- 技能放成功，但冷却 GE 被**抵抗/免疫**
- 冷却会**叠加**（多个来源加同一个 Tag）
- 冷却会被**驱散**提前结束
- **剩余时间**只有 ASC 算得出来，技能不知道

所以 UI 要盯的是 **ASC**，不是技能。

### 41.3 工厂函数是什么 —— 它是门票，不是可选项

**一句话：造这个对象、并把它交出去的那个 `static` 函数。**

```cpp
//        返回值就是本类 → 所以叫"工厂"（生产者）
UMy_WaitCoolDownChange* UMy_WaitCoolDownChange::WaitCoolDownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCoolDownTag)
{
    UMy_WaitCoolDownChange* Obj = NewObject<UMy_WaitCoolDownChange>();   // ① 造
    Obj->ASC = AbilitySystemComponent;                                   // ② 记下盯谁
    Obj->CoolDownTag = InCoolDownTag;                                    // ③ 记下盯哪个 Tag

    if (!IsValid(AbilitySystemComponent) || !InCoolDownTag.IsValid())    // ④ 参数不对就别雇了
    {
        Obj->EndTask();
        return nullptr;        // 返回空 → 节点展开后有一层 IsValid 判断，会直接走 Then，不崩
    }

    // ⑤ 临时工自己去 ASC 那里登记监听（★ 翻译发生在这一刻）
    AbilitySystemComponent->RegisterGameplayTagEvent(InCoolDownTag, EGameplayTagEventType::NewOrRemoved)
        .AddUObject(Obj, &UMy_WaitCoolDownChange::CoolDownTagChanged);
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf
        .AddUObject(Obj, &UMy_WaitCoolDownChange::OnActiveEffectAdded);

    return Obj;                // ⑥ 交货：引擎把他存进蓝图节点的局部变量
}
```

**为什么必须有它？因为它是这个节点类型的门票。**

引擎规定：想让一个 `UBlueprintAsyncActionBase` 子类变成蓝图节点，必须有一个
**「`static` + 返回值是本类指针 + `UFUNCTION(BlueprintCallable)`」** 的函数。
原因是节点运行时得**凭空造出一个对象**，而蓝图中唯一能「带参数调 C++ 造对象」的方式，就是调 `static` 函数。

> 类比：工厂函数 = **雇佣手续**。没有它，蓝图节点不知道该找谁去要人。

### 41.4 引脚从哪来：**引擎定结构，你定名字**

节点不是普通函数节点，它的每一个脚都由 UE 的 `UK2Node_AsyncAction`（继承 `UK2Node_BaseAsyncTask`）按固定规则生成，规则在 `K2Node_BaseAsyncTask.cpp` 的 `AllocateDefaultPins()`。

| 引脚 | 谁定 | 依据 |
|---|---|---|
| 左 `Execute` / 右 `Then` | **引擎**写死 | `K2Node_BaseAsyncTask.cpp:99 / 116` |
| 蓝色 `Async Task` 脚**存不存在** | **引擎** | 有 `ExposedAsyncProxy` 元数据才生成（line 119） |
| 蓝色脚**叫什么名字** | **你** | `UCLASS(meta=(ExposedAsyncProxy = "AsyncTask"))` |
| 左侧有哪些输入脚 | **你** | 工厂函数的参数表（line 179 起遍历） |
| 输入脚的名字 | **你** | 参数变量名（`AbilitySystemComponent` → 显示成 `Ability System Component`） |
| 右侧有几颗白箭头 | **你** | 有几个多播委托属性（line 147 遍历） |
| 白箭头的名字 | **你** | 委托属性名（`CoolDownStart` → `Cool Down Start`） |
| 绿色参数脚 | **你（间接）** | 委托宏里写的参数名（line 162，只取**第一个**委托的签名） |
| 引脚的排列顺序 | **引擎** | 代码里 `CreatePin` 的调用顺序 = 从上到下 |

**三条补充规则**：

1. **返回值不生成脚** —— 工厂函数的 `ReturnParm` 被 line 187-191 跳过，改头换面成了蓝色的 `Async Task` 脚
2. **`const FGameplayTag&` 生成"引用脚"** —— line 194 `PinParams.bIsReference = true`，不能手填字面量，必须喂变量
3. **所有委托必须有相同签名** —— line 155-158 只取第一个委托的签名函数；line 334 注释原话：*"each delegate must have the same signature"*

**硬限制**：这种节点**只能放在 Event Graph / 宏里**，不能放进函数里（line 81-91）。标题右边的小闹钟图标就是标记。

### 41.5 节点展开图（编译期）

这个节点在编译时会被展开成一张小图（`K2Node_BaseAsyncTask.cpp:415-514`）：

```
Execute ─► [ WaitCoolDownChange ] ─► IsValid(Proxy)? ─┬─ True ─► AddDelegate(Proxy->CoolDownStart, 事件A)
            工厂函数                                   │         │
            │ 返回值 ──────────────► 蓝色 Async Task    ├─► AddDelegate(Proxy->CoolDownEnd, 事件B)
            │                                          │         │
            │                                          │         └─► Activate() ─► Then
            │                                          │
            │                                          └─ False ─► Then   （Proxy 为 null 时直接跳过去）
            └─ 参数 → 输入脚
```

| 从图里能看出的结论 | 说明 |
|---|---|
| **绑监听在工厂函数之后**，外面包了 `IsValid` | 这就是返回 `nullptr` 安全的原因 |
| **`Activate()` 排在最后** | line 485-504；它是引擎留给你的「绑定齐了、可以开始干活了」的钩子（你这里没重写，是空的） |
| **一个委托都没有 → 编译报错** | line 478-482：`"BaseAsyncTask: Proxy has no delegates defined"` |
| 你连在 `Then` 后面的东西 | 实际接在 `Activate` 之后 |
| `HideThen` 元数据 | 可以干掉 `Then` 脚（line 107） |

### 41.6 `BlueprintInternalUseOnly` = 菜单护栏

```cpp
UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"))
static UMy_WaitCoolDownChange* WaitCoolDownChange(...);
```

**一句话：让这个函数「编译期合法、用户菜单里搜不到」。**

```cpp
// EdGraphSchema_K2.cpp:950-954
bool UEdGraphSchema_K2::CanUserKismetCallFunction(const UFunction* Function)
{
	return Function && 
		(Function->HasAllFunctionFlags(FUNC_BlueprintCallable) 
		 && !Function->GetBoolMetaData(FBlueprintMetadata::MD_BlueprintInternalUseOnly)   // ← 就是这里
		 && ...);
}
```

| | 加了 meta | 不加 meta |
|---|---|---|
| 蓝图右键菜单能搜到 `Wait Cool Down Change` | ❌ 搜不到 | ✅ 搜得到 |
| 能画出**普通函数节点** | ❌ 不能 | ✅ **能** |
| 异步节点（白箭头那个）能用 | ✅ 照常能用 | ✅ 也能用 |

> **不加 meta 的后果不是「少个功能」，而是同一个函数在蓝图里出现了两个入口。**

**错图长这样**（不加 meta 时能画出来，能编译、能保存、运行不报错，但全错）：

```
Event BeginPlay ──► [ Wait Cool Down Change ]        ← 普通函数节点（没有白箭头！）
                        ├─ Ability System Component ● ← OverlayWidgetController.ASC
                        ├─ In Cool Down Tag        ● ← Cooldown.Fire.FireBolt
                        └─ Return Value            ○ ← 空着
```

这时代码里的 ⑤ 已经把**两条监听登记到 ASC 上**了，但：

| 时刻 | 引擎干了什么 | 结果 |
|---|---|---|
| BeginPlay | 工厂函数跑了，监听挂上了 | 对象活了 |
| 放火球 | `CoolDownStart.Broadcast(HighestTime)` | **广播给 0 个听众** |
| 冷却结束 | `CoolDownEnd.Broadcast(0.f)` | 同样没人听 |
| 之后 | 没人调 `EndTask()` | ⚠️ 对象**永不回收** |

**表面现象**：UI 冷却倒计时永远不动。你去查原因时会以为是 `WaitCoolDownChange` 写错了，**其实 C++ 一行都没错，是连线错了。**

加了 meta 之后，你**根本没有机会**画出那张错图 —— 白箭头天生就在节点上，没接就是没接，一眼看得出来。

**旁证**：引擎自己也这么用。`UBlueprintAsyncActionBase::Activate` 的声明就是：

```cpp
/** Called to trigger the action once the delegates have been bound */
UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
virtual void Activate();
```

注释和 meta 是配套的：**「这个函数只给专用节点用，不给你手动用。」**

### 41.7 `BlueprintAssignable` 是「权限」，不是「外观」

常见误解：「`BlueprintAssignable` 就是让委托能在蓝图用，那为什么它变成了输出引脚？」

> **`BlueprintAssignable` 回答的是「蓝图有没有权限对这个委托 Add/Remove/Assign」，它不规定 UI 画成什么样。**

引擎其实是**两步把关**：

| 步骤 | 代码 | 检查条件 |
|---|---|---|
| ① 生成白箭头 | `K2Node_BaseAsyncTask.cpp:149` | 只看是不是 `FMulticastDelegateProperty` —— **连 `BlueprintAssignable` 都没查** |
| ② 点白箭头去绑 / 编译展开 | `K2Node_MCDelegate.cpp:64-66` | 检查 `CPF_BlueprintAssignable`，否则报 `Event Dispatcher is not 'BlueprintAssignable'` |

**真正决定「形态」的是：委托的宿主对象什么时候存在。**

| | 普通变量上的委托（Actor / Widget 上的） | 异步节点上的委托 |
|---|---|---|
| 对象**编辑期**存在吗 | ✅ 存在 | ❌ **不存在**，运行期才 `NewObject` |
| 蓝图怎么绑 | 拖变量 → 右键 → `Assign / Bind Event to XXX` | 节点**自己画一颗脚**给你拖 |
| 引擎的绑定入口 | `UK2Node_DelegateSet` 的菜单项 | `K2Node_BaseAsyncTask` 里写死的输出脚 |
| 展开后的结果 | `AddDelegate(对象->委托, 自定义事件)` | **一模一样** |

**同一个 `BlueprintAssignable` 属性，两种宿主 → 两种呈现。** 变量有「实例」可以指，所以给你一颗 Bind 节点；异步节点的对象还不存在，唯一能指的「靶子」就是节点自己，于是 UE 干脆把绑定入口画在节点上。

**再强调：白箭头「长得像 exec」是假象**（源码 line 151 写死的就是 `PC_Exec`）：

| | `Then`（真 exec） | `Cool Down Start`（假 exec） |
|---|---|---|
| 语义 | 这行做完了，继续往下 | **在这里注册一个回调** |
| 接上后立刻执行吗 | 立刻 | **不立刻**，等 `Broadcast` |
| 能接几个 | 多个 | 多个（多播） |

### 41.8 两种委托：原生 vs dynamic（本项目里同时用了这两种）

| | `CoolDownStart` / `CoolDownEnd` | ASC 的那两个 |
|---|---|---|
| 宏 | `DECLARE_**DYNAMIC**_MULTICAST_...` | `DECLARE_MULTICAST_...` |
| 类型 | **dynamic 委托** | **原生委托** |
| 内部存什么 | **函数名字符串**（靠反射找） | **C++ 函数指针**（直接调） |
| 绑什么 | 必须是 UFunction | 普通成员函数即可 |
| 蓝图能绑吗 | ✅ 能 | ❌ 不能 |
| 需要 `UFUNCTION()` 吗 | 绑它的蓝图事件天生是 UFunction | **不需要** |

这正好解释了你代码里的一个「不对称」：

```cpp
// 你绑 ASC 的两个：AddUObject + 普通成员函数（没有 UFUNCTION）
AbilitySystemComponent->RegisterGameplayTagEvent(...).AddUObject(this, &UMy_WaitCoolDownChange::CoolDownTagChanged);
AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UMy_WaitCoolDownChange::OnActiveEffectAdded);

// 蓝图绑你的两个：必须是 UFunction（蓝图自定义事件天生满足）
CoolDownStart.Broadcast(HighestTime);
```

> **一句话**：原生委托走「函数指针」，dynamic 委托走「函数名 + 反射」。
> 前者蓝图看不见，后者蓝图能绑。**异步节点的全部工作，就是把左边翻译成右边。**

### 41.9 `Broadcast` 的完整链路（含 UHT 生成的证据）

**证据就在自己项目里**：

```
Intermediate/Build/Win64/UnrealEditor/Inc/Aura/UHT/
├── My_WaitCoolDownChange.generated.h    ← 声明
└── My_WaitCoolDownChange.gen.cpp        ← 实现（关键都在这）
```

**① 宏做的事**（`DelegateCombinations.h:53`）—— 你写的 `OneParam` 展开后：

```cpp
FUNC_DECLARE_DYNAMIC_MULTICAST_DELEGATE( FWeakObjectPtr, DelegateName,
    DelegateName##_DelegateWrapper,          // ← wrapper 函数名
    FUNC_CONCAT( Param1Type InParam1 ),      // ← wrapper 的参数表
    FUNC_CONCAT( *this, InParam1 ),          // ← 广播时怎么调 wrapper
    void, Param1Type )
```

两件事同时发生：**生成一个类型**（内部 = 回调列表 + `Broadcast`），**并把 wrapper 的名字交给引擎**。

**② UHT 生成的 wrapper 函数**（`My_WaitCoolDownChange.gen.cpp:55-62`，本项目真实代码）：

```cpp
void FMy_CoolDownChangeSignature_DelegateWrapper(const FMulticastScriptDelegate& My_CoolDownChangeSignature, float TimeRemaining)
{
	struct _Script_Aura_eventMy_CoolDownChangeSignature_Parms
	{
		float TimeRemaining;
	};
	_Script_Aura_eventMy_CoolDownChangeSignature_Parms Parms;
	Parms.TimeRemaining = TimeRemaining;                                  // ① 装进"参数包"
	My_CoolDownChangeSignature.ProcessMulticastDelegate<UObject>(&Parms); // ② 遍历所有回调，逐个调用
}
```

**这就是「为什么不是立刻执行、要等 Broadcast」的全部秘密。**

**③ UHT 还生成了一个 UFunction**（`gen.cpp:34 / 43`）：

```cpp
NewProp_TimeRemaining = { "TimeRemaining", ..., EPropertyGenFlags::Float, ... };   // 参数属性
FuncParams = { ..., "My_CoolDownChangeSignature__DelegateSignature", ... };        // 签名函数
```

**④ `CoolDownStart` 登记时指向它**（`gen.cpp:213`）：

```cpp
FMulticastDelegatePropertyParams NewProp_CoolDownStart =
{ "CoolDownStart", ..., EPropertyGenFlags::InlineMulticastDelegate, ...,
  STRUCT_OFFSET(UMy_WaitCoolDownChange, CoolDownStart),                              // 变量偏移
  Z_Construct_UDelegateFunction_..._My_CoolDownChangeSignature__DelegateSignature,   // ★ SignatureFunction
  ... };
```

**闭环了** —— 蓝图那颗绿色 `Time Remaining` 脚的来源：

```
gen.cpp:34/43   生成属性 TimeRemaining(float) + UFunction 签名
gen.cpp:213     CoolDownStart 属性指向该 UFunction
        ↓
K2Node_BaseAsyncTask.cpp:157   DelegateSignatureFunction = Property->SignatureFunction;
        ↓
K2Node_BaseAsyncTask.cpp:170   CreatePin(EGPD_Output, ..., Param->GetFName());
        ↓
   蓝图上的 "Time Remaining"
```

**⑤ 完整的三次调用链**（★ 中间那次是**你自己**发起的，别漏）：

```
【第1次】ASC 的原生委托 ──► 你的 C++ 函数 OnActiveEffectAdded(...)
                                  │
                                  │  ← 这里是你自己的代码！可以判断、计算、过滤
                                  ▼
【第2次】你 ──► CoolDownStart.Broadcast(HighestTime)
                    │
                    │  → DelegateWrapper(*this, HighestTime)
                    │  → Parms.TimeRemaining = HighestTime
                    │  → ProcessMulticastDelegate(&Parms)
                    │     → 遍历 { 对象, 函数名 }
                    ▼
【第3次】引擎 ──► 蓝图里你绑的自定义事件，收到 TimeRemaining = 8.0
```

**⑥ 参数是你定的，不是 UE 给的**：

| 你写 | 蓝图上出现 |
|---|---|
| `_OneParam(..., float, TimeRemaining)` | 绿色脚 `Time Remaining`（float） |
| `_TwoParams(..., float, Remaining, bool, bStarted)` | 绿色脚**两颗** |
| `DECLARE_DYNAMIC_MULTICAST_DELEGATE()`（无参） | **一颗都没有**，只有白箭头 |

**注意参数个数不一样**：

```
ASC 原生委托给 OnActiveEffectAdded 的参数（3 个）：
    UAbilitySystemComponent* TargetASC / const FGameplayEffectSpec& Spec / FActiveGameplayEffectHandle Handle
                    │
                    │  ← 你的代码从这 3 个里"挑重点"，算出 1 个数
                    ▼
你给 UI 的参数（1 个）：
    float TimeRemaining
```

**这就是「翻译官」的价值** —— 原始信息又杂又多（`Spec`、`Handle` 蓝图根本用不了），翻译成 UI 真正想要的那一个数。

> 绿色脚是**节点上的输出脚**，它把值送出来；**不是**自动塞进事件体，你得自己拖线过去用。

### 41.10 什么时候需要异步节点 —— 两条路线对比

**判据一句话**：

> ## **你能不能改到「发信号的那一行」？**
> - **能** → 用普通 dynamic 委托 + 蓝图 `Bind Event`（路线 B，最省事）
> - **不能**（引擎内部、原生委托）→ 要么写转发委托，要么写异步节点
> - 再看**是不是「一次性任务」**：是 → 异步节点更合适（自带结束）

**路线 A：异步节点**（教程选的）—— 包一个临时工替你去听。

**路线 B：转发委托** —— 在自己能改的类上开一个 dynamic 委托，把 ASC 的原生事件转发出去：

```cpp
// 在 UMy_AuraAbilitySystemComponent 里（自己的类，能改）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownChanged, float, TimeRemaining);

UPROPERTY(BlueprintAssignable)
FOnCooldownChanged OnCooldownStarted;      // ← 蓝图能 Bind Event

void UMy_AuraAbilitySystemComponent::OnGEEffectAdded(...)
{
    OnCooldownStarted.Broadcast(剩余时间);   // 转发
}
```

蓝图：`Get Aura ASC → Bind Event to On Cooldown Started`。

| | 路线 B：转发委托 | 路线 A：异步节点 |
|---|---|---|
| 额外类 | 不加新类 | 加一个 `UBlueprintAsyncActionBase` 子类 |
| 委托放哪 | **必须找宿主对象**（ASC / Character） | **不用宿主**，节点自己造 |
| 蓝图怎么用 | 先拿到对象，再 Bind Event | 在 Event Graph 里直接拖白箭头 |
| 生命周期 | 委托**常驻**，永远挂着 | `EndTask()` 明确结束，用完就扔 |
| 换监听对象 | 得换宿主或重写 | 参数传不同 ASC 即可，同一节点通用 |
| 适合 | **长期存在、多人多地都要听的广播** | **一次性的、有始有终的任务** |

> **不是「必须用异步节点」，而是「蓝图没法直接绑 ASC 的委托」。** 两条路都通，教程选 A 是风格偏好。
> 另外：GAS 引擎自带一批异步节点（`UAbilityAsync_WaitGameplayTagAdded` / `WaitAttributeChanged` / `WaitGameplayEffectApplied`…），**先翻引擎有没有现成的，再决定自己写。**

### 41.11 生命周期：为什么 `EndTask` 必须调

`UBlueprintAsyncActionBase` 的构造函数（`BlueprintAsyncActionBase.cpp:13-20`）：

```cpp
if (!HasAnyFlags(RF_ClassDefaultObject))
{
    SetFlags(RF_StrongRefOnFrame);   // ← 盖章："GC 别收我"
}
```

`RF_StrongRefOnFrame` 的定义（`ObjectMacros.h:543`）：

```
///< References to this object from persistent function frame are handled as strong ones.
```

蓝图 ubergraph 的**持久帧**（存局部变量的地方）在 GC 时的处理（`BlueprintGeneratedClass.cpp:1909-1926`）：

```cpp
if (!Object->HasAnyFlags(RF_StrongRefOnFrame))
{
    // 没有这个标记 → 按弱引用处理，GC 时会被清成 nullptr
    if (InnerCollector.MarkWeakObjectReferenceForClearing(&Object)) return;
}
InnerCollector.AddReferencedObject(Object, ...);   // 有这个标记 → 强引用，GC 不收
```

翻译成人话：

> 蓝图节点之间的连线（局部变量）默认是**弱引用** —— 这是故意的，否则一个 Latent 节点会把对象钉死一辈子。
> 但异步节点需要「活到任务结束」，所以基类在构造时给自己盖了个章：**「我这个持久帧里的引用，当强引用算。」**

```
EndTask() → SetReadyToDestroy() → ClearFlags(RF_StrongRefOnFrame) → 下次 GC 回收
                ↑
         没人调 → 章一直在 → 对象一直在 → 泄漏
```

**为什么平时察觉不到**：PIE 结束整个 World 被销毁，什么都回收了。打包后反复进出关卡才会看到内存曲线不对，而且**不报错、不崩、日志干净**。

### 41.12 本章一句话总结

**异步节点不是「C++ 主动通知蓝图」的唯一方式，而是「当发信号的那一行你改不到时」的翻译官。
工厂函数是它的门票（引擎规定必须有），白箭头是它替你造的绑定入口，
`Broadcast` 是你自己主动发起的那一次调用 —— 中间隔着你的 C++ 代码，参数也是你重新挑过的。**

**记忆口诀**：

> **能改到发信号那一行 → 普通委托就够了。**
> **改不到（引擎内部的事）→ 要么自己转发，要么雇个临时工（异步节点）。**

---

## 四十二、SpellMenu 功能收尾（完成）

### 42.1 本次改动：新增 `OnSpellGlobeReassign` 委托

```cpp
// My_SpellMenuWidgetController.h —— 新增
// 转配技能后取消 SelectImage 的显示委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_SpellGlobeReassignSignature, const FGameplayTag&, Abilitytag);

UPROPERTY(BlueprintAssignable)
FMy_SpellGlobeReassignSignature OnSpellGlobeReassign;
```

```cpp
// My_SpellMenuWidgetController.cpp —— OnAbilityEquipped() 末尾
OnStopWaitForEquipSelection.Broadcast(AbilityDA->FindAbilityInfoFromTag(AbilityTag).AbilityType);
OnSpellGlobeReassign.Broadcast(AbilityTag);      // ← 新增：通知球体刷新，取消"选中态"显示
```

### 42.2 为什么不用教程的 `GlobeDeselect()`

这正是第 40.4 节复查问题 #5（`OnAbilityEquipped` 缺少收尾）的**另一种解法**：

| 做法 | 机制 | 特点 |
|---|---|---|
| **教程**：`GlobeDeselect()` | 清空 `SelectedAbility` 并广播「取消选中」 | 描述面板**整个清空**，用户刚看到的技能信息没了 |
| **本实现**：`OnSpellGlobeReassign.Broadcast(AbilityTag)` | 只广播「这个技能变了」，怎么做交给蓝图 | 描述面板可以**保留**，只刷新球体上的槽位显示 |

> **设计取舍**：教程是「一刀切清空」，本实现是「广播事实，让 UI 自己决定」——后者更灵活，也更符合"Controller 只负责通知，不负责决定 UI 长什么样"的分层原则。

### 42.3 SpellMenu 功能清单（收尾核对）

| 功能 | 入口 | 数据流 |
|---|---|---|
| 开关菜单 | 按 Tab | `My_AuraPlayerController` → HUD 切 UI 模式 |
| 点击技能球 → 显示描述 | `My_WBP_SpellGlobe_Button` | `SpellGlobeSelected` → `OnAbilityInfo` |
| 升级按钮（Enable 判定） | `EquippedButtonPressed` | StatusTag + SpellPoint 决定（见第 35 章） |
| 装备按钮 → 进入选槽模式 | `EquippedButtonPressed` | `OnWaitForEquipSelection` 广播 |
| 点槽位 → 装备 | `My_WBP_Equipted_Button` | `EquipSpellRowGlobePressed` → ASC `ServerEquipAbility`（见第 39 章） |
| 装备后刷新 HUD 图标 | — | ASC `OnAbilityEquipped` → `My_OverlayWidgetController`（两次 `OnAbilityInfo` 广播：清旧槽 + 填新槽） |
| 装备后刷新球体 | — | `OnStopWaitForEquipSelection` + **`OnSpellGlobeReassign`**（本次新增） |

### 42.4 一句话总结

**装备功能的最后一块拼图不是"多调一个函数"，而是"用广播替代清空"——
Controller 只广播事实（哪个技能变了），把"UI 该长什么样"留给蓝图决定。
这也是整个 SpellMenu 从头到尾一直遵循的分层原则。**

---

## 四十三、构建速度排查：磁盘、UBT 并行、.uproject vs .sln

### 43.1 真正的瓶颈：项目在机械硬盘上

| 盘符 | 物理盘（注册表 `disk\Enum`） | 介质 | 容量 | 可用 |
|---|---|---|---|---|
| **C:** | `NVMe KINGSTON SNVS500` | ✅ **NVMe SSD** | 465 GB | 只剩 53 GB |
| **D:** | `WDC WD10EZEX-00BBHA0` | ❌ **7200 转机械硬盘** | 932 GB | 512 GB |

而**所有关键路径都在 D 盘**：

```
D:\UE Engine\UE_5.2                  ← 引擎源码 + 链接时要读的 .lib
D:\UE5 Project\...\Source            ← 你的源码
D:\UE5 Project\...\Intermediate      ← 2.35 GB，obj / 响应文件 / PCH
      └─ Build\Win64\x64\AuraEditor\DebugGame\Engine\
             SharedPCH.Engine.NonOptimized.ShadowErrors.InclOrderUnreal5_0.h.pch   ← 1.7 GB
D:\UE5 Project\...\Binaries          ← 0.17 GB，DLL / PDB
```

| | 7200 转机械 | 入门 NVMe | 倍数 |
|---|---|---|---|
| 顺序读 | ~150 MB/s | ~1500~2000 MB/s | 10~13× |
| **4K 随机读 IOPS** | ~100~150 | ~40,000+ | **约 300×** |
| 访问延迟 | ~8~12 ms | ~0.1 ms | ~100× |

**编译/链接恰好是「大量小文件 + 随机读写」** —— 机械盘最怕的场景。
再加上内存被 Rider 占了 10.5 GB（可用只剩 7~11 GB），那个 1.7 GB 的 PCH **缓存不住，每个 `cl.exe` 都要从机械盘重读**。

> ⚠️ **注意**：慢的原因**不是「盘符叫 D」**，而是「D 那块盘是机械硬盘」。
> 页面文件（`c:\pagefile.sys`）和 Rider 缓存（`%LOCALAPPDATA%\JetBrains`）都已经在 SSD 上，**这两处是对的**。

**可行的改善**（按性价比）：

| 方案 | 成本 | 收益 |
|---|---|---|
| 把 `Intermediate` + `Binaries`（共 2.5 GB）用 **junction** 挪到 C 盘 | 0 | PCH 读取 + obj/PDB 写入全上 SSD |
| 加一块 SSD（1TB SATA 即可），引擎+项目整体搬过去 | 一块盘 | 根本解（连引擎头文件、`.lib` 都上 SSD） |
| 日常用 Live Coding（跳过链接 = 跳过最大一笔磁盘写入） | 0 | 在机械盘上尤其明显 |

> ⚠️ 删 junction 必须用 `cmd /c rmdir`；**不要用 `Remove-Item -Recurse`**（某些 PowerShell 版本会穿透联接删掉真实内容）。

### 43.2 UBT 的并行数是怎么算的

```
最大并行数 = Min( 物理核数 , 可用物理内存 ÷ 每动作内存 )
```

源码 `UnrealBuildTool\System\Utils.cs:1310-1322`：

```csharp
if (MemoryPerActionBytes > 0)
{
    long FreeMemoryBytes = GetFreeMemoryBytes();          // = GC 看到的「总内存 − 已用」
    int TotalMemoryActions = Convert.ToInt32(FreeMemoryBytes / MemoryPerActionBytes);
    if (TotalMemoryActions < MaxActionsToExecuteInParallel)
        MaxActionsToExecuteInParallel = Math.Max(1, Math.Min(MaxActionsToExecuteInParallel, TotalMemoryActions));
}
```

**关键坑：`Target.cs` 里的 `MemoryPerActionGB` 只能调大，不能调小。**
`ParallelExecutor.cs:44 / 98`：

```csharp
private static double MemoryPerActionBytes = 1.5 * 1024 * 1024 * 1024;   // 硬编码默认 1.5 GB

double MemoryPerActionBytesComputed = Math.Max(MemoryPerActionBytes, MemoryPerActionBytesOverride);
//                                            ↑ 1.5 GB 是地板，填 0.9 被直接吃掉
```

所以：

| 写什么 | 结果 |
|---|---|
| `MemoryPerActionGB = 0.9` | ❌ **无效** —— `Max(1.5, 0.9) = 1.5` |
| `MemoryPerActionGB = 3.0` | ✅ 生效，但**更保守**（并行数变少） |
| 想真正放开 | 只能改 `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` 里的 `MemoryPerActionBytes`（源码注释：*Set to 0 to disable free memory checking*） |

> **而且就算放开了收益也很小**：`Building 8 actions with 7 processes` —— 8 个动作只差 1 个并行位；
> 而**链接必须等所有编译完成**，关键路径几乎不变。

### 43.3 `.uproject` vs `.sln`

| | `Aura.uproject`（0.7 KB） | `Aura.sln`（3.3 KB） |
|---|---|---|
| 是什么 | **项目的源头**（JSON） | **给 IDE 看的工程索引** |
| 谁生成 | 你 / UE 编辑器 | **UBT 自动生成** |
| 谁认它 | UE 编辑器、UBT、打包 | 只有 IDE（VS / Rider） |
| 删了会怎样 | ❌ 项目没了 | ✅ 重新生成即可 |

```
Aura.uproject  +  Aura.Build.cs  +  AuraEditor.Target.cs   ← 源头（你维护）
        │  UBT -Mode=GenerateProjectFiles
        ▼
Aura.sln  +  Intermediate\ProjectFiles\{UE5,Aura}.vcxproj  ← 生成物
        │
        ▼
Rider / Visual Studio
```

**一个很有说明力的细节**：`Aura.sln` 被 git 跟踪，但它引用的两个 `.vcxproj` 在 `Intermediate\` 下、被 `.gitignore` 排除。
→ 别人克隆仓库后，`Aura.sln` 是个**空壳**，必须先 `GenerateProjectFiles`。
→ **这就证明了 `.sln` 是派生物。**

**为什么 Rider 两个都能打开**：它装了两套集成。

| 打开 | 机制 | 能力 |
|---|---|---|
| **`.uproject`** | **Rider for Unreal Engine** 插件 | 全功能（蓝图索引、`.uasset` 跳转、Target/Config 下拉、Live Coding） |
| `.sln` | 通用 C++ / MSBuild | 能编译能跳转，缺 UE 专属功能 |

JetBrains [官方文档](https://www.jetbrains.com/help/rider/2022.3/Unreal_Engine__Before_You_Start.html)原话：

> *"You can work with the `.uproject` directly in JetBrains Rider, **without generating a Visual Studio solution**… On Windows, you can alternatively open your `.sln` files."*

**结论：日常用 `.uproject` 打开**（JetBrains 主推路径）。注意这跟"引擎索引"无关 —— 换打开方式**不影响** F12 跳引擎代码。

### 43.4 一句话总结

**慢的不是"哪个盘符"，是"哪块盘"。D 盘是机械硬盘，而引擎 + 项目 + 所有编译产物都在上面 —— 这才是"改 2 行也慢"的头号原因；
`MemoryPerActionGB` 调小无效（被 1.5 GB 地板吃掉），因为瓶颈根本不是并行度；
`.uproject` 是源、`.sln` 是 UBT 生成的派生物，日常开 `.uproject`。**

---

## 四十四、伤害全链路 + 两套属性回调

> 本章把「一次火球从按键到落血」完整串一遍，并回答三个最容易混的问题：
> ① `SetByCaller` 和 `EffectContext` 谁管什么；② `ExecCalc` 什么时候被调用；
> ③ `PostGameplayEffectExecute` 和 `PostAttributeChange` 有什么区别。

### 44.1 核心认知：伤害是「两段式」的

**「写配方」和「算伤害」是两个时间点，中间隔着火球的飞行时间。**

| 阶段 | 时刻 | 谁在跑 | 干了什么 |
|---|---|---|---|
| **写配方** | 技能激活时 | `SpawnProjectile` | 造 Spec + 填伤害数字（**不算**） |
| 飞行 | 中间几秒 | `AMy_ProjectileActor` | GAS 完全不参与 |
| **算伤害** | **火球撞到人** | `My_ExeCalc_Damage` | 这才是计算 |
| 落血 | 紧接着 | `PostGameplayEffectExecute` | 扣血 / 飘字 / 死亡 |

**类比**：`MakeOutgoingSpec` = 拿空白处方笺；`SetSetByCallerMagnitude` = 在处方上写「火焰 20」；
火球 = 快递员；`ApplyGameplayEffectSpecToSelf` = 把处方交给药房；`ExecCalc` = **药房按处方配药**。

### 44.2 完整链路（16 步，含行号）

| # | 时刻 | 位置 | 关键代码 |
|---|---|---|---|
| 1 | 按 1 | PlayerController → ASC | `AbilityInputTagHeld` → `TryActivateAbility` |
| 2 | GA 激活 | `GA_FireBolt`（**蓝图**） | `ActivateAbility` |
| 3 | 等鼠标 | WaitTargetData Task | 异步 |
| 4 | 生成火球 | `My_AuraProjectileSpell.cpp` | `SpawnProjectile` |
| 5 | **写配方** | 同上 `:60` | `MakeOutgoingSpec` ← **不计算** |
| 6 | **填数字** | 同上 `:65` | `SetSetByCallerMagnitude` |
| 7 | 交出 Spec | 同上 `:67` | `Projectile->DamageEffectSpecHandle = ...` |
| 8 | **火球飞行** | `AMy_ProjectileActor` | GAS 不参与 |
| 9 | 命中 | `My_ProjectileActor.cpp:44` | `OnSphereOverlap` |
| 10 | **执行 GE** | 同上 `:73` | `ApplyGameplayEffectSpecToSelf` |
| 11 | **算伤害** | `My_ExeCalc_Damage.cpp:64` | `Execute_Implementation` |
| 12 | 抓属性 | 同上 `:110-119` | `AttemptCalculateCapturedAttributeMagnitude` |
| 13 | **读回伤害** | 同上 `:150` | `GetSetByCallerMagnitude` |
| 14 | 输出 | 同上 `:219` | `AddOutputModifier(IncomingDamage)` |
| 15 | **扣血** | `My_AuraAttributeSet.cpp:126` | `PostGameplayEffectExecute` |
| 16 | 飘字/死亡 | 同上 `:161 / :174` | `Die()` / `ShowDamageText` |

> ⚠️ **`My_AuraFireBolt.cpp` 里只有两个描述函数**，`SpawnProjectile` 在 C++ 里**没有任何调用点**
> —— 说明"生成火球"的逻辑在**蓝图 `GA_FireBolt`** 的 Event Graph 里。

### 44.3 ★ 两条数据通道（最容易混的地方）

| | **SetByCaller** | **EffectContext** |
|---|---|---|
| 方向 | **进去** ⬇ | **出来** ⬆ |
| 路径 | 技能 → Spec → ExecCalc | ExecCalc → Context → 外部 |
| 写入时机 | **创建 Spec 时**（`:65`） | **ExecCalc 执行中**（`:178 / :207`） |
| 读出时机 | **ExecCalc 中**（`:150`） | **GE 执行完之后**（`AttributeSet:172-173`） |
| 装什么 | `TMap<FGameplayTag, float>` | 你自定义的字段 |
| 用途 | 把技能配好的数值**送进去算** | 把算出来的结果**带出来** |

**一句话**：

```
SetByCaller  = 【进去】的通道   （技能 → 计算）
EffectContext = 【出来】的通道   （计算 → 结果使用方 / 客户端）
```

**为什么 Debuff 必须用 Context？** 因为"这次到底有没有触发 debuff"是 ExecCalc 执行时**随机**出来的
（`ExecCalc_Damage.cpp:87` `FMath::RandRange(1,100) < 几率`）——创建 Spec 时根本不存在，SetByCaller 帮不上忙。

**为什么不能塞进属性里？** 因为 `IncomingDamage` 用完立刻被清零（`My_AuraAttributeSet.cpp:149` `SetIncomingDamage(0.f)`），装不住。

### 44.4 ★ ExecCalc 有**两个出口**（不是"所有东西都放 Context"）

| | **出口 A：改属性** | **出口 B：带元数据** |
|---|---|---|
| API | `OutExecutionOutput.AddOutputModifier(...)` | 往 `Spec.GetContext()` 写字段 |
| 放什么 | **数值本身** | **关于这次事件的描述** |
| 你的代码 | `My_ExeCalc_Damage.cpp:219-223` → `IncomingDamage` | `:178 / :207` → 暴击、格挡（将来 Debuff） |
| 谁读 | 引擎（GE 系统自动改属性） | **你自己** |
| 为什么走这条 | 属性是 GE 系统的本职 | "是不是暴击"**没有对应属性**，装不住 |

```cpp
// 出口 A
OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
    UMy_AuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));   // :219

// 出口 B
UMy_AuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritical);             // :207
```

### 44.5 自定义 EffectContext：为什么必须是 Struct、为什么单独一个文件

**它是什么**：跟着 GE 走的「随身包裹」。每次施法 `MakeEffectContext()` 造一个，
跟着 Spec 走完「ExecCalc → GE 执行结束 → PostGameplayEffectExecute → 复制到客户端」。

**为什么必须是 USTRUCT**：

| 原因 | 说明 |
|---|---|
| 基类就是 USTRUCT | `FGameplayEffectContext` 本身就是，你只能跟着 |
| 网络复制 | 需要 `NetSerialize` + `TStructOpsTypeTraits<WithNetSerializer=true>` |
| 反射 | `GetScriptStruct()` 让引擎知道反序列化用哪个类型 |
| 会被复制多次 | 所以必须实现 `Duplicate()`（含 HitResult 深拷贝） |

**为什么单独一个文件（不放进 `My_AuraDamageGameplayAbility.h`）**：

```
        Ability（逻辑层）        My_AuraDamageGameplayAbility
              ↑ 应该向下依赖
        ExecCalc（计算层）        My_ExeCalc_Damage
              ↑
        AttributeSet（数据层）    My_AuraAttributeSet
              ↑
        Types（纯数据类型）  ← 放这里才对 ✅
```

- 它被 **Ability / ExecCalc / AttributeSet / Globals 四个层共享**
- 放进 GA 的头文件 → **数据层反过来依赖技能层**（层次颠倒）+ 拉进一堆 GAS 依赖
- **老实说这不是编译器硬性规定**：技术上可以在任何头文件里（类外面）声明 `USTRUCT`，共享同一个 `.generated.h`
  —— 真正的原因是「分层」和「共享」，**不是 UHT 不允许**

**5 个使用触点**（少一个机制就不生效）：

| # | 位置 | 作用 |
|---|---|---|
| 1 | `Config/DefaultGame.ini:8`<br>`+AbilitySystemGlobalsClassName="/Script/Aura.MyAbilitySystemGlobals"` | ★ **全局注册** |
| 2 | `MyAbilitySystemGlobals.cpp:9` | ★ **工厂**：`return new FMY_AuraGamePlayEffectContext();` |
| 3 | `My_AuraAbilityTypes.cpp` | `NetSerialize` 实现 |
| 4 | `My_AuraAbilitySystemLibrary.cpp:139-169` | **收口**：`Get/Set` 辅助，内部 `static_cast` |
| 5 | `My_ExeCalc_Damage.cpp:178/207`（写）<br>`My_AuraAttributeSet.cpp:172-173`（读） | 写入端 / 读取端 |

**⚠️ `NetSerialize` 的位掩码坑**：

```cpp
if (bIsBlockedHit)  RepBits |= 1 << 7;
if (bIsCriticalHit) RepBits |= 1 << 8;
Ar.SerializeBits(&RepBits, 9);      // ★ 9 = 最大位号 + 1
```

**加字段必须同时改这个数字，否则新字段静默丢失、不报错。**
（`FBitReader::SerializeBits` 是精确按位：`BitReader.h:74` `Pos += LengthBits`，不取整到字节）

> 🔍 顺手发现：教程的 `AuraAbilityTypes.cpp` 用了 **0~19 共 20 位**，但写的是 `SerializeBits(&RepBits, 19)`
> —— bit 19（`RadialDamageOrigin`）在网络复制时会丢。以后抄这段记得「位数 = 最大位号 + 1」。

### 44.6 `FGameplayEffectModCallbackData Data` 里有什么

引擎定义得非常小（`GameplayEffectExtension.h:17-30`）：

```cpp
struct FGameplayEffectModCallbackData
{
    const struct FGameplayEffectSpec&       EffectSpec;      // ← ★ Context 装在这里面
    struct FGameplayModifierEvaluatedData&  EvaluatedData;   // 这次改哪个属性、改成什么
    class UAbilitySystemComponent&          Target;          // ★ 目标 ASC（现成的）
};
```

**所以**：`Data.EffectSpec.GetContext()` 就是"这一次 GE"的 Context。

**为什么很多 GE 都在调却不会串台？** 因为 `Data` 是引擎**为这一次执行现场构造**的（看那个构造函数），
每个 `Data` 都牢牢绑着它自己那一个 Spec。

**`SetEffectProperty` 做的事 = 把 `Data` 里 3 个字段，补全成一个好用的 `Props`**：

| `Props` 字段 | 从哪来 | 代码行 |
|---|---|---|
| `EffectContextHandle` | `Data.EffectSpec.GetContext()` | `:255` |
| **`SourceASC`** | **`Context.GetOriginalInstigatorAbilitySystemComponent()`** | `:256` |
| `SourceAvatarActor` | `SourceASC->AbilityActorInfo->AvatarActor` | `:260` |
| `SourceController` | `PlayerController`，空则用 `Pawn->GetController()` 兜底 | `:262-270` |
| `SourceCharacter` | `Cast<ACharacter>(SourceController->GetPawn())` | `:273` |
| `TargetAvatarActor` | `Data.Target.AbilityActorInfo->AvatarActor` | `:280` |
| `TargetCharacter` | `Cast<ACharacter>(TargetAvatarActor)` | `:283` |
| **`TargetASC`** | `Data.Target` **本身** | `:284` |

**★ 注意 Source / Target 的不对称**：

- **Target 是现成的** —— `PostGameplayEffectExecute` 本来就是 target 的 AttributeSet 在跑，"我是谁"当然知道
- **Source 要绕一圈** —— 只能从 Context 里翻 `GetOriginalInstigatorAbilitySystemComponent()`
  （拿的是**原始发起者**的 ASC，玩家的 ASC 挂在 PlayerState 上，不是 AuraCharacter）

### 44.7 ★ `PostGameplayEffectExecute` 的真实触发条件

**不是「GE 执行完就一定调」，而是「有一个 Modifier 真的要写属性才调」。**

整个 GAS 插件里**只有一个调用点**（`GameplayEffect.cpp:2933`，在 `InternalExecuteMod` 里）：

```cpp
// GameplayEffect.cpp:2894
bool FActiveGameplayEffectsContainer::InternalExecuteMod(FGameplayEffectSpec& Spec, FGameplayModifierEvaluatedData& ModEvalData)
{
    UAttributeSet* AttributeSet = ... Owner->GetAttributeSubobject(AttributeSetClass);   // :2906

    if (AttributeSet)                                        // ★ 门槛 1：AttributeSet 必须存在
    {
        FGameplayEffectModCallbackData ExecuteData(Spec, ModEvalData, *Owner);   // ★ 构造 Data
        if (AttributeSet->PreGameplayEffectExecute(ExecuteData))                 // ★ 门槛 2：可以拒绝
        {
            ApplyModToAttribute(...);                                            // :2920 真正写值
            AttributeSet->PostGameplayEffectExecute(ExecuteData);                // :2933 ★★ 就是这里
        }
    }
    else
    {
        ABILITY_LOG(Log, TEXT("%s does not have attribute %s. Skipping modifier"), ...);   // :2954 不调用
    }
}
```

**它是「按 Modifier」调用的，不是「按 GE」**（`GameplayEffect.cpp:2140-2146`）：

```cpp
for (int32 ModIdx = 0; ModIdx < SpecToUse.Modifiers.Num(); ++ModIdx)     // 遍历所有 Modifier
{
    ModifierSuccessfullyExecuted |= InternalExecuteMod(SpecToUse, EvalData);   // 每个一次
}
```

| GE 配置 | 会调吗 | 原因 |
|---|---|---|
| Instant + **Modifiers** | ✅ 每个 Modifier 一次 | 有东西要写 |
| Instant + **ExecCalc 有输出** | ✅ | `AddOutputModifier` 也算 Modifier |
| Instant + **ExecCalc 无输出** | ❌ | 循环里没东西 |
| Duration GE **刚被加上** | ❌ | 挂上去 ≠ 执行 Modifier |
| Duration GE **周期到点** | ✅（若有 Modifier） | `GameplayEffect.cpp:3506` |
| Duration GE **过期移除** | ❌ | 移除不走 Modifier |
| **只加 GrantedTags 的 GE** | ❌ | 没有 Modifier |
| **只播 GameplayCue 的 GE** | ❌ | 没有 Modifier |
| `PreGameplayEffectExecute` 返回 `false` | ❌ | 这次修改被丢弃 |
| 目标 ASC 上**没有这个 AttributeSet** | ❌ | 打 log 跳过 |

> ⚠️ **重要例子修正**：**冷却 GE 不会调用它！**
> 冷却 GE（`GE_FireBolt_Cooldown`）是「**Tag 型 GE**」—— 只有一个 `GrantedTags`，**没有任何 Modifier**，
> 它根本不碰属性。**这就是"Tag 型 GE"和"数值型 GE"的根本区别。**

**还有两条"改属性但不调用它"的路**（`AbilitySystemComponent.h:213-220` 注释明说）：

```cpp
/**
 *	This does not invoke Pre/PostGameplayEffectExecute calls on the attribute set.
 *	No GameplayEffectSpec is created or is applied!
 */
void ApplyModToAttribute(const FGameplayAttribute &Attribute, ...);
```

| API | 会调 Post 吗 |
|---|---|
| `ApplyGameplayEffectSpecToSelf(Spec)` | ✅（走 Modifier 时） |
| `ASC->ApplyModToAttribute(...)` | ❌ **明确不会** |
| `ASC->ApplyModToAttributeUnsafe(...)` | ❌ 不会 |
| `ASC->SetNumericAttributeBase(...)` | ❌ 不走这条通道 |

### 44.8 ★ `PostAttributeChange` vs `PostGameplayEffectExecute`

**一句话：一个是「属性层」，一个是「GE 层」。**

| | `Pre/PostAttributeChange` | `Pre/PostGameplayEffectExecute` |
|---|---|---|
| **层级** | **属性层**（AttributeSet 自己的事） | **GE 层**（GameplayEffect 系统的事） |
| **谁触发** | **任何**属性赋值 | **只有 GE Modifier** 写属性 |
| **签名** | `(Attribute, float& NewValue)` / `(Attribute, Old, New)` | `(const FGameplayEffectModCallbackData& Data)` |
| **知道是谁改的吗** | ❌ **不知道** | ✅ 知道（`Data.EffectSpec` 有完整上下文） |
| **能改值 / 拒绝吗** | ✅ Pre 能改值，不能拒 | ✅ Pre 能返回 `false` 拒绝 |
| **典型用途** | 钳制数值、**依赖属性联动** | **伤害落血 / 飘字 / 死亡 / Debuff** |

#### 源码：属性层的唯一入口

```cpp
// AttributeSet.cpp:77 —— 整个 GAS 里只有 AbilitySystemComponent.cpp:342 调它
void FGameplayAttribute::SetNumericValueChecked(float& NewValue, class UAttributeSet* Dest) const
{
    OldValue = DataPtr->GetCurrentValue();
    Dest->PreAttributeChange(*this, NewValue);              // :100
    DataPtr->SetCurrentValue(NewValue);                     // 写内存
    Dest->PostAttributeChange(*this, OldValue, NewValue);   // :102
    MARK_PROPERTY_DIRTY(Dest, StructProperty);              // 标记网络脏 → 触发复制
}
```

而 `SetHealth(x)` 这个宏最终也走这里（`AttributeSet.h:438-446`）：

```cpp
#define GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    FORCEINLINE void Set##PropertyName(float NewVal) \
    { \
        AbilityComp->SetNumericAttributeBase(Get##PropertyName##Attribute(), NewVal); \
    }
```

→ `SetHealth(50)` → `SetNumericAttributeBase` → `SetNumericAttribute_Internal`(:342) → `SetNumericValueChecked`
→ **`PreAttributeChange` / `PostAttributeChange`** ✅

#### ★ GE 改属性时，**两套都会走**（洋葱式嵌套）

```
【GE Modifier 改属性】
├─ PreGameplayEffectExecute            GameplayEffect.cpp:2917   ← GE 层（可拒绝）
│   └─ ApplyModToAttribute                                     :2920
│        └─ SetNumericAttribute_Internal                       :2758
│             └─ SetNumericValueChecked
│                  ├─ PreAttributeChange        AttributeSet.cpp:100   ← 属性层（可改值）
│                  ├─ 写内存
│                  ├─ PostAttributeChange       AttributeSet.cpp:102   ← 属性层
│                  └─ MARK_PROPERTY_DIRTY（触发复制）
│   └─ PostGameplayEffectExecute        GameplayEffect.cpp:2933  ← GE 层
└─ 完成

【直接 SetHealth(x)】
SetHealth(x) → SetNumericAttributeBase → SetNumericValueChecked
     ├─ PreAttributeChange
     ├─ 写内存
     └─ PostAttributeChange
✗ 没有 Pre/PostGameplayEffectExecute！
```

> **结论**：`PostAttributeChange` 覆盖面**更大**（所有属性变化），
> `PostGameplayEffectExecute` 覆盖面**更小**（只有 GE Modifier），但**信息更丰富**。

#### 为什么"升级补满血"必须放 `PostAttributeChange`

你的代码（`My_AuraAttributeSet.cpp:110-121`、`:199-202`）：

```cpp
// PostGameplayEffectExecute 里（处理 IncomingXP）
bTopOffHealthOnLevelUp = true;    // ★ 只置标记，不动血

// PostAttributeChange 里
if (bTopOffHealthOnLevelUp && Attribute == GetMaxHealthAttribute())
{
    SetHealth(GetMaxHealth());     // ★ 此刻才是新 Max
    bTopOffHealthOnLevelUp = false;
}
```

时序：

```
升级（GE 执行 IncomingXP）
   ├─ 加属性点、技能点
   ├─ 标记 bTopOffHealthOnLevelUp = true
   └─ 想在这里 SetHealth(GetMaxHealth())
         ⚠️ 读到的是【旧 Max】—— MaxHealth 的 MMC 聚合器还没重算
   ▼ GE 执行结束
   MMC 重新计算 MaxHealth（因为 Strength 变了）
         └─ SetNumericAttribute_Internal(MaxHealth, 新值)
               └─ ★ PostAttributeChange(MaxHealth, 旧, 新)   ← 只有这里抓得到
                     └─ 此刻 GetMaxHealth() = 新 Max ✅
```

> **核心原因：MMC 刷新走的是「属性层」通道，不是「GE 层」。**
> 所以 `PostGameplayEffectExecute` 里抓不到 MMC 的刷新结果 —— **只能在 `PostAttributeChange` 里抓。**

#### 决策表：该放哪个

| 你想做的事 | 放哪 |
|---|---|
| 钳制数值（血量不超过上限） | **`PreAttributeChange`**（能改值，任何来源都拦得住） |
| Max 变了 → Current 跟着调 | `PreAttributeChange`（教程做法）/ `PostAttributeChange` |
| 处理伤害（落血 / 死亡 / 飘字） | **`PostGameplayEffectExecute`**（需要来源信息） |
| 读 ExecCalc 写的 Context 字段 | **`PostGameplayEffectExecute`**（只有这里有 `Data.EffectSpec`） |
| 升级后补满血（依赖 MMC 刷新） | **`PostAttributeChange`** |
| 给 UI 推数据 | **`GetGameplayAttributeValueChangeDelegate`**（第三条通道） |

#### 附：`PreAttributeChange` 里的重入

```cpp
// My_AuraAttributeSet.cpp:100-103
if (Attribute == GetMaxHealthAttribute())
{
    SetHealth(FMath::Min(GetHealth(), NewValue));   // ← 在 Pre 里又 Set 另一个属性
}
```

会造成重入（`PreAttributeChange(MaxHealth)` 还没结束时，`Health` 已经被改了），
**不会无限递归**（`Health` 的 Pre 不碰 `MaxHealth`），教程也这么做 —— 但记住有这回事。

### 44.9 本章一句话总结

**伤害是两段式的：「写配方」（技能激活）和「算伤害」（火球撞到人）隔着几秒，`ExecCalc` 只在后一段被调用。**

**ExecCalc 有两条出口：数值走 `AddOutputModifier`（改属性），描述数据走 `EffectContext`（带出来）。
`SetByCaller` 是「进去」的通道，`EffectContext` 是「出来」的通道 —— 方向相反，别混。**

**`PostGameplayEffectExecute` 不是「GE 执行完就调」，而是「有 Modifier 真的写属性才调」——
所以冷却 GE（纯 Tag 型）永远走不到它。**

**`PostAttributeChange` 是属性层的门卫（任何写入都过），`PostGameplayEffectExecute` 是 GE 层的门卫（只有 GE 引发，但带完整上下文）。
GE 改属性时两套按「GE 层 → 属性层 → 写 → 属性层 → GE 层」嵌套执行。**

---

## 四十五、GA 数据传输三阶段：Params → Spec → Context

> 本章是「一次伤害」的**数据流总纲**：三个阶段、三个载体、各自归谁。
> 触发条件、两套回调的细节见第 44 章。

### 45.1 ★ 三阶段总览（本章核心）

```
【阶段①  参数】GA 侧
    FMy_DamageEffectParams
      ├─ 谁产出：GA 的 MakeDamageEffectParamsFromClassDefaults()
      ├─ 存哪：   投射物的 UPROPERTY 成员（跨越飞行时间）
      └─ 方向：   进 ⬇
              │
              ▼  命中时
【阶段②  载体】Spec
    UAuraAbilitySystemLibrary::ApplyDamageEffect(Params)
      ├─ MakeEffectContext()                    → 造 Context
      ├─ MakeOutgoingSpec(GE类, 等级, Context)   → 造 Spec
      ├─ AssignTagSetByCallerMagnitude × N       → ★ 参数写进 Spec
      └─ TargetASC->ApplyGameplayEffectSpecToSelf(Spec)
              │
              ▼  GE 执行
【阶段③  结果】Context
    ExecCalc 读 SetByCaller → 算 → 写 Context
      └─ 方向：出 ⬆（其实双向）
              │
              ▼  GE 执行完
    PostGameplayEffectExecute 读 Context → 落血 / 飘字 / 死亡 / 造 DOT
```

| 阶段 | 载体 | 谁产出 | 谁消费 | 时机 | 方向 | 要复制吗 |
|---|---|---|---|---|---|---|
| ① | **`FMy_DamageEffectParams`** | GA | `ApplyDamageEffect` | 放技能 → 命中 | **进** ⬇ | ❌ 不需要 |
| ② | **`FGameplayEffectSpec`** | `MakeOutgoingSpec` | `ExecCalc` / 引擎 | 命中 → GE 执行 | **运输** | 跟着 GE |
| ③ | **`FMY_AuraGamePlayEffectContext`** | `ExecCalc` | `PostGameplayEffectExecute` / 客户端 | GE 执行中 → 之后 | **出** ⬆ | ✅ **必须** |

> **一句话记**：**Params 装料 → Spec 运输 → Context 回执。**

### 45.2 阶段一：Params —— GA 侧的"参数清单"

```cpp
USTRUCT(BlueprintType)
struct FMy_DamageEffectParams
{
    UPROPERTY() TObjectPtr<UObject>            WorldContextObject;          // 拿 World 用
    UPROPERTY() TSubclassOf<UGameplayEffect>   DamageGameplayEffectClass;   // 用哪个 GE
    UPROPERTY() TObjectPtr<UAbilitySystemComponent> SourceASC;             // 谁打的
    UPROPERTY() TObjectPtr<UAbilitySystemComponent> TargetASC;             // 打谁（命中时才填！）
    UPROPERTY() float        BaseDamage;
    UPROPERTY() float        AbilityLevel;
    UPROPERTY() FGameplayTag DamageType;
    UPROPERTY() float        DebuffChance / DebuffDamage / DebuffFrequency / DebuffDuration;
};
```

**它的 4 个作用**：

| # | 作用 | 说明 |
|---|---|---|
| 1 | **参数打包** | 10+ 个参数 → 1 个，调用点可读、加字段不动签名 |
| 2 | **跨层跨时间传递** | GA 填 → 存投射物 → 命中时消费 |
| 3 | **蓝图可用** | `USTRUCT(BlueprintType)` + `UPROPERTY` → Make/Break；`UPARAM(ref)` → 蓝图能原地改 |
| 4 | ★ **把"造 Spec"推迟到命中时** | 见下 |

#### ★ 第 4 点才是它存在的真正理由

| | **旧做法（存 Spec）** | **新做法（存 Params）** |
|---|---|---|
| 造 Spec 的时机 | **生成火球时** | **命中时** |
| 投射物存什么 | `FGameplayEffectSpecHandle` | `FMy_DamageEffectParams` |
| 命中时的信息 | ❌ **拿不到**（出发时就定死了） | ✅ 能拿到 |

**哪些参数必须"命中时"才知道**：

| 参数 | 为什么 |
|---|---|
| **径向伤害原点** | 爆炸中心 = **命中点** |
| **击退方向** | = `命中点 − 施法者位置` |
| **死亡冲量** | 同上 |

> **所以 `TargetASC` 在 `SpawnProjectile` 里传 `nullptr` 是对的** —— 出发时根本不知道打谁：
> ```cpp
> Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);   // 出发时
> // ...
> DamageEffectParams.TargetASC = TargetASC;      // 命中时才填（My_ProjectileActor.cpp）
> ```

### 45.3 阶段二：Spec —— 载体（`ApplyDamageEffect` 六步）

```cpp
FGameplayEffectContextHandle UMy_AuraAbilitySystemLibrary::ApplyDamageEffect(const FMy_DamageEffectParams& Params)
{
    const AActor* SourceAvatarActor = Params.SourceASC->GetAvatarActor();                          // ① 拿 Avatar
    const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();

    FGameplayEffectContextHandle EffectContextHandle = Params.SourceASC->MakeEffectContext();      // ② 造 Context
    EffectContextHandle.AddSourceObject(SourceAvatarActor);

    FGameplayEffectSpecHandle EffectSpecHandle =                                                   // ③ 造 Spec
        Params.SourceASC->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbilityLevel, EffectContextHandle);

    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Params.DamageType,        Params.BaseDamage);      // ④ ★ 写 SetByCaller
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.My_Debuff_Chance,    Params.DebuffChance);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.My_Debuff_Damage,    Params.DebuffDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.My_Debuff_Duration,  Params.DebuffDuration);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.My_Debuff_Frequency, Params.DebuffFrequency);

    Params.TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);                        // ⑤ ★ 应用 Spec
    return EffectContextHandle;                                                                     // ⑥ 返回 Context
}
```

#### ⚠️ 两个最容易错的地方（本项目实际踩过）

**坑 1：用错 API —— `ApplyGameplayEffectToSelf` vs `ApplyGameplayEffectSpecToSelf`**

```cpp
// AbilitySystemComponent.h:755
FActiveGameplayEffectHandle ApplyGameplayEffectToSelf(
    const UGameplayEffect* GameplayEffect,              // ← 要 GE 的【类默认对象】
    float Level,
    const FGameplayEffectContextHandle& EffectContext,
    FPredictionKey PredictionKey = FPredictionKey());

// AbilitySystemComponent.h:320
virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(
    const FGameplayEffectSpec& GameplayEffect);          // ← 这个才是要 Spec
```

| | `ApplyGameplayEffectToSelf` | `ApplyGameplayEffectSpecToSelf` |
|---|---|---|
| 参数 | GE 默认对象 + Level + Context | **已造好的 Spec** |
| 内部行为 | 它自己**再造一遍** `MakeOutgoingSpec` | 直接用你给的 Spec |
| 什么时候用 | 你**没有** Spec | **你已经有 Spec** ← ★ 你的情况 |

> **写错不只是编译不过** —— 就算能编过，也等于**把刚造的 Spec 扔掉重造，SetByCaller 全丢**。

**坑 2：漏掉整段 `AssignTagSetByCallerMagnitude`**

漏了会怎样（整条链推下来）：

```
Spec 里没有 SetByCaller 数据
   ↓
ExecCalc：Spec.GetSetByCallerMagnitude(Pair.Key, false, 0.f) → 全部返回 0
   ↓
for (DamageToResistance) { Damage += 0 * ... } → Damage 永远是 0
   ↓
IncomingDamage = 0
   ↓
PostGameplayEffectExecute 里 if (LocalIncomingDamage > 0.f) → 【不成立】
   ↓
❌ 不扣血、不飘字、不死亡 —— 而且不报错
```

> **这就是 `Params` 存在的意义**：它就是为了给这一步供料。
> **没有 SetByCaller，`Params.BaseDamage` 和 4 个 Debuff 参数全是废数据。**

#### 阶段② 的三条要点

| 要点 | 说明 |
|---|---|
| **Spec 不是计算者** | 它只负责**运输**（运 SetByCaller + 运 Context），真正算的是挂在 GE 上的 `ExecCalc` |
| **float 走 SetByCaller，非 float 走 Context** | `FVector`/`bool`/自定义类型塞不进 `TMap<Tag,float>`，只能进 Context |
| **这里是"造 Context"的地方** | 而且**输入型**字段（击退/径向）也在这里写进 Context |

### 45.4 阶段三：Context —— 结果回传

（机制细节见 **44.5 / 44.7 / 44.8**，这里只放归属）

| | 内容 |
|---|---|
| **谁写** | `ExecCalc`（暴击 / 格挡 / Debuff）+ `ApplyDamageEffect`（击退 / 径向 —— 输入型） |
| **谁读** | `PostGameplayEffectExecute`（落血/飘字/死亡/造 DOT）、客户端（特效） |
| **为什么要复制** | 服务器算的暴击、Debuff 结果，客户端也要用来显示 |
| **为什么双向** | 击退方向是"进"（给 ExecCalc 算），暴击是"出"（给 AttributeSet 用） |

> **`Params` 只进不出 → 不需要复制、没有 `NetSerialize`。**
> **`Context` 双向 → 必须 `Duplicate` + 必须 `NetSerialize`。**

### 45.5 本章一句话总结

**一次伤害的数据流是「三阶段、三载体」：**

**① `FMy_DamageEffectParams`（GA 装料，进）→ ② `FGameplayEffectSpec`（运输，含 SetByCaller + Context）→ ③ `FMY_AuraGamePlayEffectContext`（回执，出）**

**Params 的真正价值是「把造 Spec 推迟到命中那一刻」—— 这样击退方向、径向原点这些命中时才知道的信息才拿得到。**

**两个最容易错的地方：`ApplyGameplayEffectSpecToSelf` 别写成 `ApplyGameplayEffectToSelf`；`AssignTagSetByCallerMagnitude` 一行都别漏（漏了不报错，只是伤害永远是 0）。**

---

## 四十六、ASC 的身份：AbilityActorInfo 与双端初始化

### 46.1 `GetAbilitySystemComponentFromActorInfo()` 的原理

它没有魔法 —— 就是读一个被**注入**进来的字段：

```cpp
// GameplayAbility.cpp:1053
UAbilitySystemComponent* UGameplayAbility::GetAbilitySystemComponentFromActorInfo() const
{
    if (!ensure(CurrentActorInfo)) { return nullptr; }
    return CurrentActorInfo->AbilitySystemComponent.Get();     // :1059
}
```

**三份信息拼起来**：

| # | 信息 | 谁给的 | 什么时候 |
|---|---|---|---|
| ① | "在哪个 ASC 上调的" | **隐式的 `this`** | 调 `TryActivateAbility` 那一刻 |
| ② | "这个 ASC 的宿主/化身是谁" | **`InitAbilityActorInfo(...)`** | 角色初始化时（你写的） |
| ③ | "这次激活的上下文" | 引擎**注入**给技能实例 | `CallActivateAbility` 内部 |

**注入链（`GameplayAbility.cpp`）**：

```
CallActivateAbility(Handle, ActorInfo, ...)          :828
   ├─ PreActivate(...)                               :830  ★ 先注入
   │     └─ SetCurrentInfo(...)                      :789
   │           └─ SetCurrentActorInfo(...)           :1887
   │                 └─ CurrentActorInfo = ActorInfo;// :1872 ★★ 关键赋值
   └─ ActivateAbility(...)                           :831  ← 你的代码从这里开始
```

> **顺序很重要**：`PreActivate`（注入）在 `ActivateAbility`（你的代码）**之前** ——
> 所以你在 `ActivateAbility` 里一调就能拿到值。

**⚠️ 坑：`NonInstanced` 技能拿不到**

```cpp
// GameplayAbility.cpp:1868
void UGameplayAbility::SetCurrentActorInfo(...) const
{
    if (IsInstantiated())          // ★ 只有实例化的技能才赋值
    {
        CurrentActorInfo = ActorInfo;
        CurrentSpecHandle = Handle;
    }
}
```

| InstancingPolicy | 能用 `GetAbilitySystemComponentFromActorInfo()` 吗 |
|---|---|
| `NonInstanced` | ❌ **不能** —— `CurrentActorInfo` 一直是 null，`ensure` 会失败 |
| `InstancedPerActor` | ✅ 能用 |
| `InstancedPerExecution` | ✅ 能用 |

### 46.2 玩家 ASC 的挂载：PlayerState 创建 + Character 缓存 + 接口

**三个角色**：

| 谁 | 有什么 | 语义 |
|---|---|---|
| `AMy_AuraPlayerState` | `CreateDefaultSubobject<UMy_AuraAbilitySystemComponent>(...)` | ASC **住在这里** |
| `AAura_Character` | `UPROPERTY() TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;` | **只是指针缓存** |
| `AMyCharacter_Base` | 实现 `IAbilitySystemInterface::GetAbilitySystemComponent()` | 把缓存暴露出去 |

**链路**：

```
① AMy_AuraPlayerState 构造函数（My_AuraPlayerState.cpp:13-16）  ← 创建组件
        │
② AAura_Character::My_InitAbilityActorInfo()（Aura_Character.cpp:166）
        ├─ :171  ASC->InitAbilityActorInfo(AuraPlayerState, this)     ← 告诉 ASC 谁是 Owner/Avatar
        ├─ :178  AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();  ★ 拷指针
        └─ :179  AttributeSet           = AuraPlayerState->GetAttributeSet();
        │
③ 使用
        Character->GetAbilitySystemComponent()                        → 缓存指针
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(X)  → 走接口，通用
        PlayerState->GetAbilitySystemComponent()                      → 真组件
        ↑ 三者返回【同一个对象】
```

**为什么要实现接口**：引擎的通用机制靠它 ——

```cpp
// My_ProjectileActor.cpp 命中时
UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)
// 内部：Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()
```

> **火球不需要知道打中的是玩家还是敌人、ASC 挂在谁身上** —— 只要对方实现了接口就能拿到。
> 这就是"接口 = 通用契约"（同第 27 章的思路）。

### 46.3 为什么两端各初始化一次

```cpp
// Aura_Character.cpp:47
void AAura_Character::PossessedBy(AController* NewController)  // ★ 服务器专属
{
    Super::PossessedBy(NewController);
    My_InitAbilityActorInfo();     // ← 服务器
    AddCharacterAbilities();       // ← 只有服务器（GiveAbility 必须服务器调）
}

// Aura_Character.cpp:58
void AAura_Character::OnRep_PlayerState()                      // ★ 客户端专属
{
    Super::OnRep_PlayerState();
    My_InitAbilityActorInfo();     // ← 客户端
}
```

#### 为什么 `PossessedBy` 只在服务器

```cpp
// Controller.cpp:304-314
void AController::Possess(APawn* InPawn)
{
    if (!bCanPossessWithoutAuthority && !HasAuthority())      // ★ 权限门禁
    {
        UE_LOG(LogController, Warning,
            TEXT("Trying to possess %s without network authority! Request will be ignored."), ...);
        return;                                               // ★ 客户端直接 return
    }
    ...
    OnPossess(InPawn);                                        // :326
}

// Controller.cpp:361  →  InPawn->PossessedBy(this)
```

**客户端走的是完全另一条线**：

```cpp
// PlayerController.cpp:763  ★ _Implementation = RPC
void APlayerController::ClientRestart_Implementation(APawn* NewPawn)
{
    SetPawn(NewPawn);                      // :770
    AcknowledgePossession(GetPawn());      // :788
    // ★ 这条路上【没有】PossessedBy
}
// :868 AcknowledgePossession → :877 ServerAcknowledgePossession（RPC 回服务器）
```

| | 服务器 | 客户端 |
|---|---|---|
| 触发链 | `Possess` → `OnPossess` → **`Pawn->PossessedBy`** | `ClientRestart`(RPC) → `SetPawn` → `AcknowledgePossession` |
| 会调 `PossessedBy` | ✅ | ❌ |
| PlayerState 就绪 | ✅ | ❌ **还没**（靠复制） |
| 用什么钩子 | `PossessedBy` | **`OnRep_PlayerState`** |

#### 为什么客户端必须等 `OnRep_PlayerState`

```cpp
// Pawn.h:158
UPROPERTY(replicatedUsing=OnRep_PlayerState, ...)
TObjectPtr<APlayerState> PlayerState;      // ★ 这是【复制属性】
```

**`Pawn->PlayerState` 是网络复制过来的**，所以：

```
客户端 Possess 那一刻
   └─ GetPlayerState<AMy_AuraPlayerState>() → ❌ nullptr

（几帧后 PlayerState 复制到位）
   └─ 引擎自动调 OnRep_PlayerState() → ✅ 这时才能拿到
```

而 `My_InitAbilityActorInfo` 里有：

```cpp
AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
check(AuraPlayerState);        // ⚠️ null 就直接崩
```

> **服务器不需要 `OnRep_PlayerState`**（PlayerState 是本地创建的，不走复制 → 不触发）。
> **两个函数互补，不是重复。**

### 46.4 Owner vs Avatar（及敌人为什么不同）

**玩家**：

```cpp
AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
//                                                                  ↑ OwnerActor    ↑ AvatarActor
```

| 角色 | 是谁 | 为什么 |
|---|---|---|
| **OwnerActor** | `AMy_AuraPlayerState` | ASC 挂载处，**GAS 数据在这** —— PlayerState 重生/切关卡不销毁，数据才保得住 |
| **AvatarActor** | `AAura_Character` | 场景里代表玩家，位置/Socket/动画都靠它 |

| 函数 | 返回 |
|---|---|
| `GetAbilitySystemComponentFromActorInfo()` | PlayerState 上的 ASC |
| `GetAvatarActorFromActorInfo()` | **Aura_Character** |
| `GetOwningActorFromActorInfo()` | **AuraPlayerState** |

> **这解释了 `SetEffectProperty` 里的"不对称"**（44.6）：
> 玩家的 Owner ≠ Avatar，所以 `SourceASC` 要从 Context 里翻 `GetOriginalInstigatorAbilitySystemComponent()`。

**敌人**：

```cpp
// Enemy_Characte.cpp:160
AbilitySystemComponent->InitAbilityActorInfo(this, this);   // Owner == Avatar == 自己
```

| | 玩家 | 敌人 |
|---|---|---|
| ASC 创建在哪 | **PlayerState 构造函数** | **Character 构造函数** |
| `InitAbilityActorInfo` | `(PlayerState, Character)` | `(this, this)` |
| Character 的 ASC 成员 | **拷贝来的指针** | **就是组件本身** |

> **同一套接口、同一套调用方式，底层挂载方式完全不同** —— 这正是 `IAbilitySystemInterface` 的价值。

### 46.5 本章一句话总结

**UE 能识别"谁释放的技能"，靠的是三份信息拼起来：① `TryActivateAbility` 隐式的 `this`；② 你写的 `InitAbilityActorInfo(Owner, Avatar)`；③ 激活时引擎把 ActorInfo 指针注入技能实例（`PreActivate` → `CurrentActorInfo = ActorInfo`）。**

**`GetAbilitySystemComponentFromActorInfo()` 只是读第 ③ 步那个字段 —— 它的语义是「当前这次激活挂在哪个 ASC 上」，不是静态的"技能拥有者"。**

**玩家的 ASC 住在 PlayerState，Character 只是缓存指针并用 `IAbilitySystemInterface` 暴露出去；
初始化必须分两端：服务器靠 `PossessedBy`（客户端根本不调它），客户端靠 `OnRep_PlayerState`（因为 `Pawn->PlayerState` 是复制属性，早调会 `check` 崩）。**

---

## 四十七、Debuff 功能完整实现：动态 GE + Context 三段接力

### 47.1 完整链路（一图看懂）

```
【ExecCalc】My_DetermineDebuff
   读 SetByCaller（几率/伤害/时长/频率）→ 掷骰 → 写 Context
        │
        │  ① SetByCaller 是【进去】的通道   ② Context 是【出来】的通道
        ▼
【Context】NetSerialize（bit 9~13，SerializeBits = 14）
        │
        ▼
【AttributeSet】PostGameplayEffectExecute
   └─ if (IsSuccessfulDebuff(Context)) → Debuff(Props)
         ├─ 从 Context 读回 4 个参数
         ├─ NewObject<UGameplayEffect> 现场造一个 GE
         └─ ApplyGameplayEffectSpecToSelf → DOT 周期性掉血
                │
                ▼  每次 tick
           IncomingDamage 改变 → PostGameplayEffectExecute
                → 飘字 / 死亡判定 / HitReact 全部自动复用 ✅
```

**关键设计**：Debuff 的 Modifier **指向 `IncomingDamage`** —— 于是 DOT 的伤害走的是**和即时伤害完全同一条管道**，
飘字、死亡、受击动画一行都不用重写。

### 47.2 `Debuff()` 六步（`My_AuraAttributeSet.cpp`）

```cpp
void UMy_AuraAttributeSet::Debuff(const FMy_EffectProperties& Props)
{
    const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();

    // ① 从【旧】Context（伤害 GE 的那个）读 ExecCalc 算好的参数
    const FGameplayTag DamageType = UMy_AuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
    const float DebuffDamage      = UMy_AuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
    const float DebuffDuration    = UMy_AuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
    const float DebuffFrequency   = UMy_AuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

    // ② 运行时造 GE（没有资产可用，见 47.4 / 47.5）
    const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
    UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

    // ③ 填 GE 的"周期三件套"
    Effect->DurationPolicy    = EGameplayEffectDurationType::HasDuration;  // ★ 不设默认是 Instant，周期全废
    Effect->Period            = DebuffFrequency;
    Effect->DurationMagnitude = FScalableFloat(DebuffDuration);            // ★ 不能直接赋 float
    Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageToDebuff[DamageType]);  // = Granted Tags
    Effect->bExecutePeriodicEffectOnApplication = false;                   // ★ 见 47.6 坑 1

    // ④ Modifier 指向 IncomingDamage —— 复用整套伤害管道
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.Attribute         = UMy_AuraAttributeSet::GetIncomingDamageAttribute();
    ModifierInfo.ModifierOp        = EGameplayModOp::Additive;
    ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
    Effect->Modifiers.Add(ModifierInfo);

    // ⑤ 造 Context（必需）+ Spec（★ 用栈对象，别 new）
    FGameplayEffectContextHandle Context = Props.SourceASC->MakeEffectContext();
    Context.AddSourceObject(Props.SourceAvatarActor);

    // ⑥ 应用
    FGameplayEffectSpec Spec(Effect, Context, 1.f);
    Props.TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}
```

### 47.3 `InheritableOwnedTagsContainer` **就是**编辑器的 "Granted Tags"

```cpp
// GameplayEffect.h:2049-2051
/** These tags are applied to the actor I am applied to */
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags,
          meta=(DisplayName="GrantedTags", Categories="OwnedTagsCategory"))
//              ~~~~~~~~~~~~~~~~~~~~~~~~~~ ★ 编辑器显示名
FInheritedTagContainer InheritableOwnedTagsContainer;
```

**容易混的三个容器**（就挨在一起）：

| C++ 变量名 | 编辑器显示名 | 注释原文 | 含义 |
|---|---|---|---|
| `InheritableGameplayEffectTags` | GameplayEffectAssetTag | *"tags the GE **has** and **DOES NOT** give to the actor"* | GE **自己**有 |
| **`InheritableOwnedTagsContainer`** | **GrantedTags** ★ | *"applied **to the actor** I am applied to"* | **给目标** |
| `InheritableBlockedAbilityTagsContainer` | GrantedBlockedAbilityTags | — | 给目标加"封锁技能"标签 |

**验证**（`GameplayEffect.cpp:1233-1257`）：

```cpp
void FGameplayEffectSpec::GetAllGrantedTags(OUT FGameplayTagContainer& Container) const
{
	Container.AppendTags(DynamicGrantedTags);
	if (Def) { Container.AppendTags(Def->InheritableOwnedTagsContainer.CombinedTags); }   // ★ 就是它
}
void FGameplayEffectSpec::GetAllAssetTags(OUT FGameplayTagContainer& Container) const
{
	Container.AppendTags(GetDynamicAssetTags());
	if (Def) { Container.AppendTags(Def->InheritableGameplayEffectTags.CombinedTags); }   // ← 另一个
}
```

> **你其实早就在用它** —— 冷却异步任务里的 `Spec.GetAllGrantedTags(GrantedTags)` 读的就是这个容器，
> 而 `GE_FireBolt_Cooldown` 那一栏是在**编辑器**里配的。
>
> **"编辑器配" 和 "C++ AddTag" 是同一个字段的两个入口** —— 区别只在于
> **运行时用 `NewObject` 造的 GE 没有编辑器界面，只能代码填**。

### 47.4 `GetTransientPackage()` 是什么

```cpp
// UObjectGlobals.h:1642 —— ★ 它其实就是 NewObject 的默认 Outer
template< class T >
T* NewObject(UObject* Outer = (UObject*)GetTransientPackage())
```

```cpp
// Obj.cpp:4834-4835 —— 它是在哪创建的
GObjTransientPkg = NewObject<UPackage>(nullptr, TEXT("/Engine/Transient"), RF_Transient);
GObjTransientPkg->AddToRoot();
```

| 点 | 含义 |
|---|---|
| 名字 = `/Engine/Transient` | 引擎启动时创建的特殊包 |
| `RF_Transient` | ★ **瞬态 → 永远不会被保存到磁盘（不进 .pak）** |
| `AddToRoot()` | 常驻内存 |

**为什么必须用它**：UObject 有严格的树状结构，每个对象都要挂在某个 Outer 下。选不同的 Outer：

| Outer | 结果 |
|---|---|
| ✅ `GetTransientPackage()` | 不属于任何资产 → **不会被打包、不出现在 Content Browser** |
| ⚠️ `this` | 生命周期绑在身上，而且会被序列化进关卡/存档 |
| ❌ 蓝图资产所在的包 | **会被当成那个资产的一部分 → 污染资产** |

> **类比**：Transient Package = **草稿纸**（写完就扔）；蓝图资产的包 = **正式书页**（会被出版社印进去）。

### 47.5 `Effect->DurationMagnitude = DebuffDuration;` 为什么编译不过

**因为它的类型不是 `float`**（`GameplayEffect.h:1970`）：

```cpp
FGameplayEffectModifierMagnitude DurationMagnitude;    // ★ 不是 float
```

**为什么设计成这么复杂** —— 它要支持 4 种取值模式（`:237-258` 的 4 个构造函数）：

| 模式 | 类型 | 含义 |
|---|---|---|
| ScalableFloat | `FScalableFloat` | 常数 / 按等级查曲线 |
| AttributeBased | `FAttributeBasedFloat` | 从属性推导 |
| CustomCalculationClass | `FCustomCalculationBasedFloat` | 自定义 MMC |
| **SetByCaller** | `FSetByCallerFloat` | **从 Spec 的 SetByCaller 表读** |

**为什么不能隐式转**：

```cpp
FScalableFloat(float);                          // ScalableFloat.h:23（非 explicit）
FGameplayEffectModifierMagnitude(const FScalableFloat&);   // GameplayEffect.h:243
```

→ `float → FScalableFloat → FGameplayEffectModifierMagnitude` 需要**两次用户定义转换**，
而 **C++ 标准规定一次隐式转换序列里最多只能有【一次】用户定义转换** → 编译错误。

**修法**（显式做掉第一次）：

```cpp
Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
```

**★ 回到 `Period` 的硬限制**：

| 字段 | 类型 | 支持 SetByCaller？ |
|---|---|---|
| `DurationMagnitude` | `FGameplayEffectModifierMagnitude` | ✅ 支持（`GameplayEffect.cpp:3186` 有专门分支） |
| `Modifiers[].ModifierMagnitude` | `FGameplayEffectModifierMagnitude` | ✅ 支持 |
| **`Period`** | **`FScalableFloat`**（`GameplayEffect.h:1974`） | ❌ **不支持**（只有常数/曲线） |

→ **`DebuffFrequency` 必须写进 `Period`，传不进去** → **这就是"必须动态造 GE"的硬性原因**。

### 47.6 ★ 动态 GE 的三个坑（本项目实际踩过）

#### 坑 1：`bExecutePeriodicEffectOnApplication` 默认 `true` → 命中瞬间多飘一个数字

```cpp
// GameplayEffect.h:1976-1978
/** If true, the effect executes on application and then at every period interval.
 *  If false, no execution occurs until the first period elapses. */
bool bExecutePeriodicEffectOnApplication;      // 默认 true（GameplayEffect.cpp:60）
```

```cpp
// GameplayEffect.cpp:3231-3234 —— 引擎就是这么干的
if (AppliedEffectSpec.Def->bExecutePeriodicEffectOnApplication)
{
    TimerManager.SetTimerForNextTick(Delegate);      // ★ 下一帧立刻执行一次周期效果
}
```

**现象**：火球命中 → 飘【直伤 100】→ **下一帧**飘【灼烧 5】—— 肉眼看是"同时飘两个数字"。

**修法**：

```cpp
Effect->bExecutePeriodicEffectOnApplication = false;    // 等第一个 Period 过去才开始掉血
```

> **这是设计选择不是 bug**：`true` = "命中立刻烫一下"（很多 ARPG 的灼烧就这德行）；
> `false` = "延迟发作"（中毒/腐蚀类）。**教程没设这一行**，所以它也是"立刻烫一下"。

#### 坑 2：`StackingType` / `StackLimitCount` 在动态 GE 上**不生效**

```cpp
Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;   // 看起来有用
Effect->StackLimitCount = 1;                                             // 实际没用
```

**引擎判断堆叠靠 `Spec.Def` 的【指针相等】**（`GameplayEffect.cpp:2518-2538`）：

```cpp
FActiveGameplayEffect* FActiveGameplayEffectsContainer::FindStackableActiveGameplayEffect(const FGameplayEffectSpec& Spec)
{
    const UGameplayEffect* GEDef = Spec.Def;
    ...
    for (FActiveGameplayEffect& ActiveEffect : this)
    {
        // ★★ 要求是【同一个 GE 对象】
        if (ActiveEffect.Spec.Def == Spec.Def && (...)) { StackableGE = &ActiveEffect; break; }
    }
}
```

**而 `Debuff()` 每次 `NewObject<UGameplayEffect>` 都造一个全新对象** → `Def` 指针永远不同
→ **堆叠匹配永远失败** → 连打两枪会有**两个独立的灼烧**。

#### 坑 3：`new FGameplayEffectSpec` 会泄漏

```cpp
// ❌ 教程的写法
if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
{
    ...
    Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
}   // ← 从来没 delete

// ✅ 更安全：FGameplayEffectSpec 是值类型，Apply 内部会拷贝，栈对象就行
FGameplayEffectSpec Spec(Effect, Context, 1.f);
Props.TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
```

`FGameplayEffectSpec` **不是 UObject**（不受 GC 管理）→ `new` 了不 `delete` 就是纯泄漏。

> **教程写 `new` 是为了拿到 Spec 去往 Context 里塞 `SetDamageType`** ——
> 而那个 `SetDamageType` 目前**全项目没人读**（见 48 章），所以这笔代码纯属成本。

### 47.7 一句话总结

**Debuff 的实现是「Context 三段接力」的收尾：ExecCalc 算好 → Context 带出来 → AttributeSet 里动态造 GE 落地。**

**必须动态造 GE 的硬性原因**：`Period` 是 `FScalableFloat`（不支持 SetByCaller），而 `DebuffFrequency` 必须写进它。

**四个必填项**：`DurationPolicy = HasDuration`、`Period`、`DurationMagnitude`、`InheritableOwnedTagsContainer`（= Granted Tags）。
**一个建议项**：`bExecutePeriodicEffectOnApplication = false`（否则命中瞬间多飘一个数字）。

---

## 四十八、GAS 语义澄清：Source/Target、IsDead 守卫、类型系统

### 48.1 ★ `ApplyGameplayEffectSpecToSelf` 的 Source / Target 是谁

**`ToSelf` 只决定 Target，Source 由 Context 决定。**

```cpp
// AbilitySystemComponent.cpp:409-417
FGameplayEffectContextHandle UAbilitySystemComponent::MakeEffectContext() const
{
	FGameplayEffectContextHandle Context = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	// By default use the owner and avatar as the instigator and causer
	// ~~~~~~ ★ 引擎注释原话
	check(AbilityActorInfo.IsValid());
	Context.AddInstigator(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get());   // ★★
	return Context;
}
```

| | 由谁决定 | 怎么决定 |
|---|---|---|
| **Target** | `ApplyGameplayEffectSpecToSelf` 的调用者 | `this` = 你在哪个 ASC 上调 |
| **Source** | **Context** | `MakeEffectContext()` 用**调用它的那个 ASC** 的 Owner/Avatar |

**所以正确写法是"两个不同的 ASC 各管一头"**：

```cpp
Props.SourceASC->MakeEffectContext()               // → Source = 攻击者
Props.TargetASC->ApplyGameplayEffectSpecToSelf(Spec)   // → Target = 受害者
```

**⚠️ 两个都用 `TargetASC` 的话**：Context 的 Instigator 变成受害者 →
ExecCalc 里 `DEFINE_ATTRIBUTE_CAPTUREDEF(..., CriticalHitChance, Source, ...)` 会去抓**被打的人的暴击率**
→ **不报错，只是数值全错**。

**另**：`ApplyGameplayEffectSpecToTarget(Spec, TargetASC)` 和 `TargetASC->ToSelf(Spec)` **完全等价**
（`AbilitySystemComponent.cpp:724` 就是一行转发）。

### 48.2 ★ 教程的 `IsDead` 守卫（我一开始读漏了）

`AuraAttributeSet.cpp:132-139`：

```cpp
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// ★★★ 就是这一行 —— 位置很讲究：在 SetEffects 之后、属性分流之前
	if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())   { ... }
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute()) { HandleIncomingDamage(Props); }
	...
}
```

**没有这一行会怎样**（本项目实际踩过）：

```
敌人死亡（bDead = true）
    ↓
Debuff 的 DOT tick → IncomingDamage 改变 → PostGameplayEffectExecute
    ↓
（没有守卫，继续往下）
    ↓
HandleIncomingDamage → NewHealth = 0 - 5 = -5 → bFatal = true（永远为真！）
    ↓
Die() + SendXPEvent() → IncomingXP += XPReward → 【一直升级】
```

**为什么 `bFatal` 永远为真**：血被 Clamp 到 0，再吃 5 点伤害就是 `-5`，`-5 <= 0` 依然成立。

**一行挡住四个副作用**：

| 副作用 | 有守卫后 |
|---|---|
| 反复加 XP → 一直升级 | ✅ 停 |
| 反复 `Die()` | ✅ 停 |
| 一直飘伤害数字 | ✅ 停 |
| 属性反复变 → 无意义网络复制 | ✅ 停 |

**本项目实现**（比教程多一个判空，更稳）：

```cpp
if (Props.TargetCharacter &&
    Props.TargetCharacter->Implements<UMy_CombatInterface>() &&
    IMy_CombatInterface::Execute_IsDead(Props.TargetCharacter))
{
    return;
}
```

> **教训**：读教程代码时，"守卫/早退"这类**一行就返回**的代码最容易看漏 ——
> 而它们往往正是解决问题的关键。**上下文要整段读，别只读出错的那一段。**

### 48.3 `static_cast` vs `Cast<>`

| | **`static_cast<T*>(Ptr)`** | **`Cast<T>(Ptr)`** |
|---|---|---|
| 是什么 | **C++ 关键字** | **UE 模板函数** |
| 能用在什么上 | 任何类型 | **只能 `UObject`** |
| 运行时检查 | ❌ **无** | ✅ 有（反射 `IsA`） |
| 类型不符 | 返回**看似有效实则错误**的指针 → UB | 返回 **`nullptr`** |
| 开销 | 0 | 一次反射查找 |

**为什么这里的 Context 只能用 `static_cast`**：

```cpp
// GameplayEffectTypes.h:218-219
USTRUCT()                                          // ★★ USTRUCT，不是 UCLASS
struct GAMEPLAYABILITIES_API FGameplayEffectContext { ... };
```

**USTRUCT 没有 `IsA()`、没有 `UClass`** → `Cast<>` 用不了。

**更安全的 USTRUCT 版写法**（用 `GetScriptStruct()` 代替 `IsA()`）：

```cpp
const FGameplayEffectContext* Raw = EffectContextHandle.Get();
if (Raw && Raw->GetScriptStruct() == FMY_AuraGamePlayEffectContext::StaticStruct())
{
    return static_cast<const FMY_AuraGamePlayEffectContext*>(Raw)->IsBlockedHit();
}
```

> 教程和本项目都没加（因为 `MyAbilitySystemGlobals::AllocGameplayEffectContext()` 保证类型一定对）——
> **知道有这条路就行**。

### 48.4 `EffectContext` 的 Get/Set 为什么收口到 Library

**关键点：`Handle.Get()` 返回的是【基类】指针**（`GameplayEffectTypes.h:499`）：

```cpp
FGameplayEffectContext* Get() { return IsValid() ? Data.Get() : nullptr; }   // ★ 基类！
```

**所以 `static_cast` 躲不掉** —— 就算字段改成 `public` 也一样。两条实际写法对比：

```cpp
// ① 直接访问（字段 public）
static_cast<FMY_AuraGamePlayEffectContext*>(Handle.Get())->DebuffDamage = 5.f;   // 78 字符

// ② 走 Library
UMy_AuraAbilitySystemLibrary::SetDebuffDamage(Handle, 5.f);                       // 58 字符 ✅ 更短
```

**而且引擎自己就是这么设计的**：`FGameplayEffectContext` 基类的字段也是 `protected`（`GameplayEffectTypes.h:389-400`），
并提供 `GetInstigator()` 访问器，`Handle` 上还包了 `AddInstigator()` 转发（`:518`）。

**Library 的真正价值（3 条）**：

| # | 价值 | 说明 |
|---|---|---|
| 1 | **判空收口** | `Handle.Get()` 可能是 null，忘了判就崩；Library 里判一次，所有调用点安全 |
| 2 | **cast 只写一次** | 换 Context 类型只改一个文件，不用满项目找 `static_cast<长类型名>` |
| 3 | **蓝图可用** | ★ **蓝图无法 `static_cast` C++ 子类** —— 这条是硬限制 |

**结论**：直接访问**不会更省事**，只是把成本从"Library 写一次"挪到"每个调用点各写一次"。

### 48.5 "收口"是什么意思

> **把散落在多处的"同一件事"，集中到一个地方做。**

| | 散口 | 收口 |
|---|---|---|
| 生活类比 | 公司 30 人每人兜里一把大门钥匙，换密码要挨个通知 | 门口设收发室，只改一个地方 |
| 本项目例子 | 每个文件各自 `static_cast<FMY_AuraGamePlayEffectContext*>(Handle.Get())` | `UMy_AuraAbilitySystemLibrary::SetDebuffDamage(Handle, 5.f)` |
| 成本 | **每次调用**都要付（cast + 判空） | **一次性**（写 10 个小函数） |
| 风险 | "有的地方改了、有的地方忘了" | 无 |

**你项目里已经有很多"收口"**：

| 已存在的东西 | 收口了什么 |
|---|---|
| `IAbilitySystemInterface` | "从任意 Actor 拿 ASC" → 火球不用知道打中的是玩家还是敌人 |
| `SetEffectProperty(Data, Props)` | "从 Data 推导 Source/Target 完整信息" |
| `ApplyDamageEffect(Params)` | "用 Params 造 Spec 并应用" |
| `BindAbilityAction(..., InputTag)` | "把按键和 Tag 绑在委托上" |
| Library 里那 10 个 Get/Set | "怎么从 Handle 拿到我的 Context" |

### 48.6 一句话总结

**`ToSelf` 只管 Target，Source 由 `MakeEffectContext()` 的调用者决定 —— 写错不报错，只是数值全错。**

**`PostGameplayEffectExecute` 开头的 `IsDead` 早退，一行挡住"死后还在处理 GE"引发的连锁问题（一直升级 / 反复 Die / 一直飘字）。**

**`FGameplayEffectContext` 是 USTRUCT → `Cast<>` 用不了 → 只能用 `static_cast`；而 `Handle.Get()` 返回基类指针，所以 cast 躲不掉 —— 这正是把 Get/Set 收口到 Library 的理由（判空 + 只写一次 + 蓝图可用）。**

---

## 四十九、状态型特效与委托生命周期

> 本章回答两个问题：**什么时候需要自定义 Niagara 组件**；**委托该用哪种绑定方式**。

### 49.1 ★ 三类特效：谁负责"结束"决定一切

| 类型 | 谁负责开始 | **谁负责结束** | 项目里的例子 | 要自定义组件吗 |
|---|---|---|---|---|
| **① 一次性** | 触发点代码 | **Niagara 自己**（Duration 到了就停） | 命中特效 `My_ProjectileActor.cpp:94`、点击提示 `AuraPlayerController.cpp:189`、FireBlast Cue、升级特效 | ❌ 直接 `SpawnSystemAtLocation` / `Activate(true)` |
| **② Actor 生命期型** | 触发点代码 | **Actor 自己**（销毁时一起走） | （拖尾类，项目里目前没有） | ❌ 挂上就行 |
| **③ 状态型** | 状态出现 | ★ **必须有人盯着状态** | **Burn / Stun Debuff**、**被动技能特效** | ✅ **必须自定义组件** |

**铁证**：项目里**只有两个**自定义 Niagara 组件，而**两个都是状态型**：

- `UDebuffNiagaraComponent` —— 监听 **GameplayTag**
- `UPassiveNiagaraComponent` —— 监听装备状态

**它们结构几乎一样（这不是巧合）**：

| | `UDebuffNiagaraComponent` | `UPassiveNiagaraComponent` |
|---|---|---|
| 构造 | `bAutoActivate = false;` | 同 |
| `BeginPlay` 注册监听 | `RegisterGameplayTagEvent(DebuffTag, ...)` | `ActivatePassiveEffect.AddUObject(...)` |
| ASC 没就绪的兜底 | `GetOnASCRegisteredDelegate()` | 同 |
| 状态变 → 亮/灭 | `DebuffTagChanged` | `OnPassiveActivate` |
| 额外判断 | `IsDead`（死了别亮） | `ActivateIfEquipped`（没装备别亮） |

> **判断标准只有一条**：**这个特效有没有一个需要被关掉的「状态」？**

### 49.2 状态型难在哪：**开始容易，结束难**

Debuff 有 **5 条结束路径**：

| # | 怎么结束 | 手动版要写 | 组件版 |
|---|---|---|---|
| 1 | Duration 到期 | `Deactivate()` | 自动（Tag 计数 1→0） |
| 2 | 被驱散 | `Deactivate()` | 自动 |
| 3 | 被免疫 | 压根没 Activate | 自动（Tag 一直 0） |
| 4 | **主人死亡** | `Deactivate()` | ⚠️ **Tag 可能没变** → 需额外处理（见 49.4） |
| 5 | ASC 销毁 | — | 组件跟角色一起销毁 |

**手动版漏一个就是"残影"bug**；组件版只盯一个 Tag，**一个监听点覆盖大部分路径**。

**另一个本质差别：网络**

| | 一次性特效 | 状态型特效 |
|---|---|---|
| 谁播 | 服务器用 `NetMulticast` RPC 广播<br>（如 `MulticastPlayImpactEffects`） | **每个端各自响应自己收到的 Tag** |
| 为什么 | "碰撞"只发生在服务器 | ★ **不需要额外 RPC** —— Tag 复制本身就是广播 |
| 数据通道 | RPC | **`GrantedTags`（会复制）** |

> ⚠️ **关键**：`IncomingDamage` 是 Meta Attribute，**只在服务器变** →
> 靠它驱动特效的话客户端永远看不到；而 **Tag 是复制的** → 客户端自动同步。

### 49.3 ★ `AddLambda` vs `AddWeakLambda`

| | **`AddLambda(L)`** | **`AddWeakLambda(O, L)`** |
|---|---|---|
| 多存什么 | 只存 lambda | lambda + **`TWeakObjectPtr<O>`** |
| 对象死了 | ❌ **照样执行** → 野指针 | ✅ **静默跳过** |
| 能 `RemoveAll(O)` | ❌ 没绑对象 | ✅ |

**源码证据**（`DelegateInstancesImpl.h:846-855`）：

```cpp
private:
	// Context object - the validity of this object controls the validity of the lambda
	//                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ★ 引擎注释原话
	TWeakObjectPtr<UserClass> ContextObject;
	mutable typename TRemoveConst<FunctorType>::Type Functor;
```

```cpp
// :835-844
bool ExecuteIfSafe(ParamTypes... Params) const final
{
	if (ContextObject.IsValid()) { (void)this->Payload.ApplyAfter(Functor, Params...); return true; }
	return false;                                  // 死了 → 静默跳过
}
```

**为什么这里必须用**：组件订阅的是**角色的委托**，角色很可能比组件活得久；
而 lambda 里 `[this]` 捕获的 `this` **不被 GC 追踪**（不是 UPROPERTY）→
组件销毁后委托仍持有它 → 角色下次广播时执行 → 解引用已销毁的 `this`。

> **判断口诀**：**"这个委托会不会比我活得久？"** 会 → **必须 `AddWeakLambda`**。
>
> ⚠️ **教程自己两处不一致**：`PassiveNiagaraComponent.cpp:27` 用 `AddLambda`（不安全），
> `DebuffNiagaraComponent.cpp:27` 用 `AddWeakLambda`（安全）→ **照后者写**。

### 49.4 ★ 死亡处理的两条路径（本项目实际实现）

```cpp
// 路径 A：Tag 变化时判 IsDead
void DebuffTagChanged(...)
{
	const bool bOwnerAlive = ... && !IMy_CombatInterface::Execute_IsDead(GetOwner());
	if (NewCount > 0 && bOwnerAlive) { Activate(); } else { Deactivate(); }
}

// 路径 B：订阅死亡委托
CombatInterface->GetOnDeath().AddDynamic(this, &UMy_DebuffNiagaraComponent::OnOwnerDeath);
void OnOwnerDeath(AActor*) { Deactivate(); }
```

| | **路径 A** | **路径 B** |
|---|---|---|
| 触发时机 | **Tag 变化**时（加/减） | **死亡那一刻** |
| 防的是 | 「死后**才**发生 Tag 变化」（延迟 GE 加 Tag / GE 到期移除） | ★ 「**死亡时 Tag 根本没变**」 |

**路径 A 单独不够**：

```
角色死亡 → Debuff GE 的 Duration 还有 3 秒 → Tag 还在
        → Tag 没变 → DebuffTagChanged 根本不会被调用
        → 特效留在尸体上 ❌
        → ★ 只有路径 B 能救
```

**两条都会调 `Deactivate()`**（死亡时一次、GE 到期时一次），但 **`Deactivate()` 幂等，无害**。

**★ 本项目写法比教程更好**：

| | 教程 | 本项目 |
|---|---|---|
| 路径 B | `MulticastHandleDeath` 里**硬编码** `BurnDebuffComponent->Deactivate(); StunDebuffComponent->Deactivate();` | 角色只 `OnDeath.Broadcast(this)`，**组件自己订阅自己关** |
| 加第 3 个 Debuff 要改哪 | ❌ **必须改角色** | ✅ **完全不用改** |
| 角色需要知道有哪些组件吗 | ✅ 需要 | ❌ 不需要 |

### 49.5 "ASC 就绪"委托：为什么需要、为什么必须过接口

**为什么需要**：组件的 `BeginPlay` 可能**早于 ASC 初始化**（玩家的 ASC 要等 `OnRep_PlayerState`）
→ 不能假设 ASC 一定在，也不能每帧轮询 → 用委托"你好了告诉我一声"。

```cpp
if (ASC) { ASC->RegisterGameplayTagEvent(...).AddUObject(...); }
else if (CombatInterface)
{
	CombatInterface->GetOnASCRegistered().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
	{
		InASC->RegisterGameplayTagEvent(...).AddUObject(...);
	});
}
```

**为什么过接口而不是 `Cast<AAuraCharacterBase>`**：组件是挂在**任何战斗单位**上的通用零件 →
`Cast<具体类>` 会把它绑死；`Cast<ICombatInterface>` 只要求"你是个战斗单位"。

> ⚠️ **两个必须成对的东西（本项目都踩过）**：
> 1. **接口必须返回引用**（`FMy_ASCRegisteredSignature&`）——
>    返回**值**的话 `AddWeakLambda` 加到的是**临时副本**上，广播时什么都不触发（**静默失效**）
> 2. **必须有 `Broadcast`** —— 教程在玩家/敌人的 `InitAbilityActorInfo` 各广播一次；
>    漏了同样**静默失效**（不报错、不崩，只是监听永远不生效）

### 49.6 一句话总结

**判断"要不要自定义 Niagara 组件"只看一条：这个特效有没有一个需要被关掉的「状态」。**
**状态型（Debuff / 被动）→ 必须自定义组件；一次性（命中 / 爆炸 / 升级）→ 直接播。**

**状态型难在"结束"**：Debuff 有 5 条结束路径 → 手动关必漏；
组件只盯一个 Tag → **一个监听点覆盖大部分路径**，而且搭 Tag 复制的便车，**零网络代码**。

**订阅长寿命委托必须用 `AddWeakLambda`**（lambda 里 `[this]` 不被 GC 追踪），
**且"死亡"要两条路径都做**（Tag 变化 + 死亡委托）—— 因为死亡时 Tag 可能没变。

---

## 五十、UE 接口的 U 类与 I 类

### 50.1 `UINTERFACE` 模式：为什么有两个类

```cpp
UINTERFACE(MinimalAPI, BlueprintType)
class UMy_CombatInterface : public UInterface     // ← U 类：反射层的"壳"
{
	GENERATED_BODY()
};

class AURA_API IMy_CombatInterface                // ← I 类：逻辑层的"实体"
{
	GENERATED_BODY()
	// 虚函数、UFUNCTION、委托都声明在这
};
```

| | **U 类** | **I 类** |
|---|---|---|
| 有什么 | `StaticClass()`、`UClassType`（由 `GENERATED_BODY()` 生成） | 虚函数、`Execute_Xxx` 静态函数 |
| 干什么 | 承载 `UClass`，被**反射查询** | 声明虚函数，被 **`Cast` / `Execute_`** 用 |

### 50.2 ★ 用在哪（本项目踩过的 C2039 报错）

**报错原文**：

```
Class.h(3785): Error C2039 : "StaticClass": 不是 "IMy_CombatInterface" 的成员
```

**根因**（`Class.h:3781-3786`）：

```cpp
template<class T>
FORCEINLINE bool UObject::Implements() const
{
	UClass const* const MyClass = GetClass();
	return MyClass && MyClass->ImplementsInterface(T::StaticClass());
	//                                        ~~~~~~~~~~~~~~~ ★ 要 T 有 StaticClass()
}
```

**`StaticClass()` 只有 UCLASS / UINTERFACE 类才有** —— I 类只是普通 C++ 类，没有它。

| 用法 | 写哪个 | 内部机制 |
|---|---|---|
| `Obj->Implements<**U**My_CombatInterface>()` | ★ **U 类** | `T::StaticClass()` |
| `Class->ImplementsInterface(**U**My_CombatInterface::StaticClass())` | ★ **U 类** | 同上 |
| `Cast<**I**My_CombatInterface>(Obj)` | **I 类** | `InterfaceType::UClassType`（I 类生成的 typedef） |
| `**I**My_CombatInterface::Execute_IsDead(Obj)` | **I 类** | 反射执行函数定义在 I 类上 |
| `Cast<**I**...>(Obj)->SomeFunc()` | **I 类** | 调 C++ 虚函数 |

> **口诀**：**「查类型用 U，调用 / 转换用 I」**

**自查命令**（有输出就是写错了）：

```powershell
Get-ChildItem -Path Source -Recurse -File -Include *.cpp,*.h |
  Select-String -Pattern 'Implements<I\w+'
```

### 50.3 ★ 委托选型：原生 vs DYNAMIC（由"谁来处理"决定）

本项目正好有两个委托，**声明方式刻意相反**：

| | **`FMy_ASCRegisteredSignature`** | **`FMy_DeathSignature`** |
|---|---|---|
| 声明 | `DECLARE_MULTICAST_DELEGATE_OneParam`<br>★ **原生** | `DECLARE_**DYNAMIC**_MULTICAST_DELEGATE_OneParam`<br>★ **动态** |
| 绑定方式 | `AddWeakLambda` / `AddUObject` | **`AddDynamic`**（要求回调是 `UFUNCTION()`） |
| **谁处理** | **纯 C++**（`UMy_DebuffNiagaraComponent`） | ★ **蓝图**（`BlueprintImplementableEvent`） |
| 内部存什么 | C++ 函数指针 / lambda | **函数名字符串**（走反射） |
| 性能 | 快 | 慢（`ProcessEvent`） |

**教程的 `OnDeathDelegate` 就是给蓝图用的**（`AuraBeamSpell.h:30-33`）：

```cpp
UFUNCTION(BlueprintImplementableEvent)
void PrimaryTargetDied(AActor* DeadActor);      // ← C++ 只声明，实现在 GA_Electrocute 蓝图里
```

**规律**：

> **处理者在 C++ → 用原生委托；处理者在蓝图 → 必须用 DYNAMIC。**

**为什么闪电链需要"死亡通知"**：闪电链依次电击 N 个目标 ——
主目标死了要**结束技能**，附加目标死了要**跳过**；
没有委托就只能每帧轮询所有目标（丑 + 抓不准时机）。

> **顺带**：教程用 `IsAlreadyBound` 防重复绑定（因为 `TraceFirstTarget` 会被多次调用）——
> 这和笔记 31.4 那条「`AddLambda` 重复绑定坑」是同一个问题。

### 50.4 GameplayCue 是什么（一句话 + 要点）

**GAS 的「表现层投递系统」** —— 逻辑层只发一个 `GameplayCue.XXX` Tag，
引擎按 Tag 在 `GameplayCueNotifyPaths` 扫出来的映射表里找到 Notify 蓝图，并在**所有客户端**播放。

| 要点 | 说明 |
|---|---|
| **两种 Notify** | `UGameplayCueNotify_Static`（UObject，一次性）/ `AGameplayCueNotify_Actor`（AActor，可持续）<br>内置子类：`Burst`（粒子+音效）、`HitImpact` |
| **四个事件** | `OnActive` / `WhileActive`（每帧）/ `Executed` / `Removed` |
| **两种入口** | GE 资产的 `GameplayCues` 数组（最常用）/ 代码调 `ExecuteGameplayCue`（本项目 `AuraFireBall.cpp:41`） |
| **自带网络复制** | `InvokeGameplayCueExecuted` 走 NetMulticast —— 解决笔记第 10 章那条"服务器放特效客户端看不见" |
| ⚠️ **最易踩的坑** | Notify 蓝图里的 `GameplayCueTag` **必须和触发时用的 Tag 一致**，否则**静默不执行、不报错** |

**本项目的配置**：

```ini
; Config/DefaultGame.ini:9-10
+GameplayCueNotifyPaths=/Game/Blueprints/AbilitySystem/GameplayCueNotifies
+GameplayCueNotifyPaths=/Game/MyBlueprints/AbilitySystem/Enemy/Cue
```

**和自定义组件的分工**：

| | GameplayCue | 自定义 Niagara 组件 |
|---|---|---|
| 适合 | **一次性表现**（爆炸 / 命中 / 音效） | **需要"自治"的持续状态**（Debuff / 被动） |
| 持续型能用吗 | ✅ 能（`AddGameplayCue` + `Actor` 型 Notify） | ✅ 能 |
| 缺点 | 持续型仍要 `Add` / `Remove` **成对调用**（有漏掉 Remove 的风险） | 换特效要改组件的资产引用 |

### 50.5 附：一次 BOM 脚本事故（教训）

我写"全项目 BOM 审计"脚本时，条件写成了「**无 BOM 就补**」，
而不是「**含非 ASCII 且无 BOM 才补**」→ **一次性改了 183 个文件**（含大量教程参考文件）。

**已全部还原**（剥掉那 3 个字节，脚本是幂等的）。

**教训**：

- **批处理脚本的"影响范围"必须先打印出来再执行**（先列名单 → 确认 → 再改），不要一边判定一边写
- 本项目 BOM 的正确判据是：**`非 ASCII` 且 `无 BOM`** ——
  纯 ASCII 的文件加不加 BOM 都行，**但别去动它们**（会污染 git diff，把用户自己的改动淹掉）

### 50.6 一句话总结

**UE 接口有两个类：`Implements<>` / `ImplementsInterface()` 用 U 类（要 `StaticClass()`），
`Cast<>` / `Execute_Xxx` 用 I 类。口诀：查类型用 U，调用 / 转换用 I。**

**委托选型由"谁来处理"决定：处理者在 C++ → 原生委托；处理者在蓝图 → DYNAMIC（`AddDynamic` + `UFUNCTION`）。**

**GameplayCue 是"用 Tag 投递表现"的通用机制**（自带网络复制），适合一次性表现；
需要"自治"的持续状态（Debuff / 被动）用自定义组件更省心。

---

## 五十一、灼烧期间不播 HitReact：Activation Blocked Tags 机制

> 起因：敌人被点燃（Burn）后，DOT 每 tick 掉一次血，**每 tick 都被判定为「挨打」**，
> 于是受击动画被反复触发 —— 敌人在地上抽搐个不停、路也走不了。
>
> 解决办法：在 `My_GA_HitReact` 的 **`Activation Blocked Tags`** 里加上 `My_Debuff.Burn`。

### 51.1 触发源：DOT 每一次 tick 都会喊一次 HitReact

`My_AuraAttributeSet.cpp:181-195`：

```cpp
else   // 没死
{
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(FMy_AuraGameplayTags::GetInstance().My_EffectGranted_HitReact);
    Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);   // ← 185 行：喊受击
}
...
if (UMy_AuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
{
    Debuff(Props);                                              // ← 194 行：挂 DOT
}
```

因为 DOT 的伤害走的是**同一条** `IncomingDamage` 通道：

```
DOT 每 0.5s tick → 改 IncomingDamage
   → PostGameplayEffectExecute → HandleIncomingDamage
        → 没死 → TryActivateAbilitiesByTag(HitReact)   ← 每 tick 喊一遍
```

**被烧 5 秒 = 喊 10 遍受击。**

> ★ 注意代码顺序：**先喊 HitReact（185 行），后挂 DOT（194 行）** ——
> 这个顺序决定了「直击那一下仍然会播」，见 51.5。

### 51.2 关键认知：`TryActivateAbilitiesByTag` 不是「硬启动」

它不绕开检查，而是走**完整的激活链**：

```
TryActivateAbilitiesByTag(Tag)
   └─ TryActivateAbility(Handle)
        └─ InternalTryActivateAbility
             └─ CanActivateAbility
                  └─ DoesAbilitySatisfyTagRequirements(...)   // GameplayAbility.cpp:356
```

**所以「Tag 不合适」能挡住它。**

### 51.3 `ActivationBlockedTags` 检查的是【自己】身上的 Tag

`GameplayAbility.cpp:214-231`：

```cpp
if (ActivationBlockedTags.Num() || ActivationRequiredTags.Num())
{
    static FGameplayTagContainer AbilitySystemComponentTags;
    AbilitySystemComponentTags.Reset();

    AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);   // ★ 查自己
    if (AbilitySystemComponentTags.HasAny(ActivationBlockedTags))
    {
        bBlocked = true;                                                        // ★ 有就激活失败
    }
    if (!AbilitySystemComponentTags.HasAll(ActivationRequiredTags)) { bMissing = true; }
}
```

> **语义**：「只要我自己身上有这些 Tag，我这个技能就不许激活。」
>
> `My_GA_HitReact` 的 `Ability Tags` = `My_EffectGranted.HitReact`（`TryActivateAbilitiesByTag` 靠它找到技能），
> 加进 `Activation Blocked Tags` 的是 `My_Debuff.Burn`。

### 51.4 `My_Debuff.Burn` 怎么跑到敌人身上的 —— Tag 生命周期 = DOT 生命周期

```
① My_AuraAttributeSet.cpp:228   Debuff() 给动态 GE 挂 Granted Tags
   Effect->InheritableOwnedTagsContainer.AddTag(*DebuffTagPtr);   // My_Debuff.Burn
                          │
② GE 被应用时，引擎把 Granted Tags 写进 Owner 的 Tag 表
   GameplayEffect.cpp:3384
   Owner->UpdateTagMap(Effect.Spec.Def->InheritableOwnedTagsContainer.CombinedTags, 1);
                          │
③ 敌人 ASC 从此【拥有】My_Debuff.Burn → GetOwnedGameplayTags() 里就有它
                          │
④ DOT 到期被移除时撤销
   GameplayEffect.cpp:3684
   Owner->UpdateTagMap(..., -1);
```

配套证据：`FGameplayEffectSpec::GetAllGrantedTags`（`GameplayEffect.cpp:1233-1240`）
= `DynamicGrantedTags` + `Def->InheritableOwnedTagsContainer.CombinedTags`。

★ **Tag 的存在时间恰好等于 `DebuffDuration`** —— 灼烧期间不抽、灼烧一结束立刻恢复，正是想要的效果。

> 这也解释了为什么**用 Tag 而不是加代码判断**很划算：
> 「灼烧中」这个状态**本来就已经有现成的 Tag 在表达**了（GE 的 Granted Tags），
> 直接复用它就行，零代码。

### 51.5 完整时序：为什么「直击那一下」仍然会播

```
【火球直击，非致命】
  └─ HandleIncomingDamage
       ├─ ① TryActivateAbilitiesByTag(HitReact)      // 185 行，先喊
       │      → 此刻身上【还没有】Burn tag → 通过 → ✅ 播放受击（被打当然要抽）
       └─ ② Debuff() → 挂 DOT GE → Burn tag 上身     // 194 行，后挂（UpdateTagMap +1）

【0.5s 后 DOT tick】
  └─ HandleIncomingDamage
       └─ TryActivateAbilitiesByTag(HitReact)
              → GetOwnedGameplayTags() 里有 My_Debuff.Burn
              → ActivationBlockedTags 命中 → bBlocked = true
              → ❌ 激活失败 → 不播受击   ← 就是想要的效果
  ...（重复 N 次，全被挡）

【DOT 到期被移除】
  └─ UpdateTagMap(-1) → Burn tag 消失
       → 之后任何伤害 → HitReact 又能播了 ✅
```

### 51.6 ★ 六组 Tag 栏位对照表（最容易配错的地方）

名字很像，但**方向和对象完全不同**：

| 栏位 | 引擎检查处 | 语义 | 一句话 |
|---|---|---|---|
| **Ability Tags** | — | 这个技能**叫什么名字** | `TryActivateAbilitiesByTag` 靠它找到技能 |
| **Activation Blocked Tags** | `GameplayAbility.cpp:222` | **我自己有这些 Tag → 我就不能激活** | 「我中毒了就别让我抽」✅ 本次用的 |
| **Activation Required Tags** | `GameplayAbility.cpp:227` | 我自己**必须全有**才能激活 | 「只有狂暴状态才能放」 |
| **Block Abilities with Tag** | `GameplayAbility.cpp:209` → `AreAbilityTagsBlocked(AbilityTags)` | **我激活期间**，让**别的**带这些 Tag 的技能无法激活 | 「我放大招时别人别插队」 |
| **Cancel Abilities with Tag** | 激活时 | **我激活时把别人取消掉** | 「我一放技能就打断你读条」 |
| **Source / Target Blocked Tags** | `GameplayAbility.cpp:237 / 253` | 检查**施法方 / 目标方**（另一个人）的 Tag | 用于「对别人」的场景 |

> **记忆法**：
> - **Activation** 开头 → 管**自己能不能放**
> - **Block / Cancel** 开头 → 管**别人**（我激活时影响别人）
> - **Source / Target** 开头 → 管**对面那个人**

### 51.7 ⚠️ 这个方案的副作用：一刀切

| 场景 | 会不会播受击 |
|---|---|
| 火球直击（挂 DOT 之前） | ✅ 播 |
| 灼烧 DOT tick | ❌ 不播（**这正是目的**） |
| **灼烧期间被第二发火球直击** | ❌ **也不播**（副作用） |
| **灼烧期间被近战砍** | ❌ **也不播**（副作用） |
| 灼烧结束后被打 | ✅ 恢复正常 |

**它挡的是「身上有 Burn tag 时的所有 HitReact 激活」，而不是「DOT 这一次伤害」。**

### 51.8 【待定 · 以后再决定】方案 B：在代码里区分「这次伤害是不是 DOT tick」

**触发条件**：如果哪天觉得「**灼烧期间新挨的直击也应该抽**」，那 `Activation Blocked Tags` 就不够精确了，
就要换成方案 B。

**做法**：给 Context 加一个「这次是 DOT 伤害」的标记，触发 HitReact 前判一下：

```cpp
else
{
    // 只有不是 DOT tick 才播受击
    if (!UMy_AuraAbilitySystemLibrary::IsDOTDamage(Props.EffectContextHandle))
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(FMy_AuraGameplayTags::GetInstance().My_EffectGranted_HitReact);
        Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
    }
}
```

**标记要打在哪里**：`My_AuraAttributeSet.cpp:240` —— `Debuff()` 里**新造的那个 Context**：

```cpp
FGameplayEffectContextHandle Context = Props.SourceASC->MakeEffectContext();
Context.AddSourceObject(Props.SourceAvatarActor);
UMy_AuraAbilitySystemLibrary::SetIsDOTDamage(Context, true);   // ★ 关键
```

**涉及改动（备忘清单）**：

1. `FMY_AuraGamePlayEffectContext` 加 `bool bIsDOTDamage` + Get/Set（`My_AuraAbilityTypes.h`）
2. `NetSerialize` 加一位 —— **当前最大位号已经是 14**（DeathImpulse），新位号 15 →
   `SerializeBits(&RepBits, 16)`；⚠️ 加了字段忘了改位数 = **新字段静默丢失**（不报错，只是客户端收不到）
3. `My_AuraAbilitySystemLibrary` 加 `Get/SetIsDOTDamage`
4. `HandleIncomingDamage` 里加上面那个判断

**取舍**：

| 方案 | 优点 | 缺点 |
|---|---|---|
| **A. `Activation Blocked Tags`（当前）** | **零代码**，纯配置；Tag 生命周期自动跟随 GE | 一刀切：灼烧期间**所有**受击都不播 |
| **B. Context 标记 + 代码判断** | 精确：只有 DOT tick 不播 | 要动结构体 + `NetSerialize` + Library 四五个文件 |
| A + B 一起 | 最稳（双保险） | — |

**结论：先按 A 用着。等真觉得「灼烧期间被打却不抽」别扭了，再上 B。**

### 51.9 附：同一个根因的另一处 —— DOT 打死人没有死亡冲量【待定】

`Debuff()` 里那个新造的 Context（`My_AuraAttributeSet.cpp:240`）**除了没带 DOT 标记，也没带 `DeathImpulse`**：

```cpp
FGameplayEffectContextHandle Context = Props.SourceASC->MakeEffectContext();  // ← 全新，DeathImpulse = (0,0,0)
Context.AddSourceObject(Props.SourceAvatarActor);
FGameplayEffectSpec Spec(Effect, Context, 1.f);
Props.TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
```

于是被灼烧打死时，`HandleIncomingDamage` 从 Context 里取到的冲量是 `(0,0,0)`：

```
【直击打死】Context A（ApplyDamageEffect 造的）→ DeathImpulse ✅ → 尸体飞出去 ✅
【灼烧 DOT 打死】Context B（Debuff() 造的）   → DeathImpulse ❌ (0,0,0) → 尸体原地不动 ❌
```

**注意**：`Die()` 其实**被调用了**（尸体也变布娃娃），只是**传进去的冲量是零向量**。

**最小修法**（在 `My_AuraAttributeSet.cpp:240` 之后两行）：

```cpp
// ★ 把【旧】Context 的死亡冲量转存到【新】Context
UMy_AuraAbilitySystemLibrary::SetDeathImpulse(
    Context,
    UMy_AuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
```

⚠️ **不能**顺手把 `bIsSuccessfulDebuff` 也复制过去 ——
那会让 DOT **每次 tick 都再挂一个新 DOT**（`HandleIncomingDamage` 末尾会再调 `Debuff()`），无限套娃。
现在因为新 Context 该标志默认 `false`，天然不会递归。

> **共同根因**：`Debuff()` 新造的 Context 是个「白板」。
> 凡是 DOT 打死人/触发受击时需要的信息，**都得手动从旧 Context 搬过去** ——
> 这跟第 45 章「Params → Spec → Context 三段接力」是同一个道理：
> **Context 不会自动继承任何东西。**

### 51.10 一句话总结

**`Activation Blocked Tags` 的语义是「我自己身上有这些 Tag，我就不许激活」——
它查的是 `GetOwnedGameplayTags()`（自己 ASC）而不是对面。**

**而 `My_Debuff.Burn` 是 DOT GE 的 `Granted Tags`，
由引擎在 GE 应用 / 移除时自动 `UpdateTagMap(+1 / -1)`（`GameplayEffect.cpp:3384 / 3684`）挂到敌人 ASC 上 ——
所以这个 Tag 的存在时间恰好等于灼烧持续时间。**

**`TryActivateAbilitiesByTag` 走完整 `CanActivateAbility` 检查（`GameplayAbility.cpp:356 → 222`），
所以 DOT 每 tick 想喊受击时，一看到身上的 Burn 就直接判失败 → 不播。
直击那一下仍然会播，因为代码顺序是「先喊 HitReact、后挂 DOT」。**

**副作用是一刀切：灼烧期间所有受击都不播。
若要「灼烧期间新挨的直击仍然要抽」，需改用方案 B（给 Context 打 DOT 标记 + 代码判断），
见 51.8 的备忘清单。**

---

## 五十二、三条链路总览：Debuff / DeathImpulse / Knockback

> 这一章把「伤害附带的三件事」串成一张总图：
> **Debuff（持续伤害）**、**DeathImpulse（死亡击飞）**、**Knockback（受击位移）**。
> 三条链路走的是**同一根骨架**，区别只在「**在哪判定**」和「**结果放哪**」。

### 52.1 共同骨架：四段式

```
① 能力 BP 配置      Damage Type / Debuff Xxx / Death Impulse Magnitude / Knockback Xxx
        │
        ▼
② 打包参数          MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor)
        │              → FMy_DamageEffectParams（一个"原料箱"结构体）
        ▼
③ 分流              ApplyDamageEffect(Params)
        │              ├─ float  → Spec 的 SetByCaller（ExecCalc 读）
        │              └─ 非float → Context（PostGameplayEffectExecute 读）
        ▼
④ 消费              ExecCalc（判定）/ AttributeSet（造 GE、Die、LaunchCharacter）
```

**记住一句话：能力给值 → 中间某处判定 → AttributeSet 执行。**

### 52.2 总览图

```
【能力 BP】敌人 Melee / 火球 / Aura 的火球
   Damage Type │ Debuff Chance·Damage·Duration·Frequency │ Death Impulse Magnitude │ Knockback Chance·Magnitude
        │
        │ ① MakeDamageEffectParamsFromClassDefaults(TargetActor)      My_AuraDamageGameplayAbility.cpp:16
        ▼
   FMy_DamageEffectParams Params          （My_AuraAbilityTypes.h:6）
        │
        ├─【近战】Params.DeathImpulse = ToTarget * Magnitude          My_AuraDamageGameplayAbility.cpp:43
        │         Params.Knockback    = 掷骰通过 ? ToTarget * Magnitude : 0   ← ★ 52.6
        │
        └─【投射物】ExposeOnSpawn 带过去，撞人时才算
                   DamageEffectParams.DeathImpulse = GetActorForwardVector() * Magnitude   My_ProjectileActor.cpp:73
                   DamageEffectParams.Knockback    = 掷骰通过 ? Rotation(45°) * Magnitude : 0  My_ProjectileActor.cpp:76
        │
        │ ② ApplyDamageEffect(Params)                                  My_AuraAbilitySystemLibrary.cpp:126
        ▼
   ┌───────────────────────────────┴────────────────────────────────┐
   │ SetByCaller（float，给 ExecCalc 读）                            │ Context（非 float，给 Post 读）
   │   DamageType        = BaseDamage                               │   SetDeathImpulse(Params.DeathImpulse)
   │   My_Debuff_Chance  = Params.DebuffChance                      │   SetKnockback(Params.Knockback)
   │   My_Debuff_Damage / _Duration / _Frequency                    │   （★ 没有 KnockbackChance！）
   └───────────────────────────────┬────────────────────────────────┘
                                   │
                                   ▼
                    GameplayEffectSpec ──► ExecCalc My_ExeCalc_Damage
                                   │
                                   │ ③ My_DetermineDebuff（My_ExeCalc_Damage.cpp:72）
                                   │      读 My_Debuff_Chance + 目标抗性 → 掷骰
                                   │      命中 → 把结果写回 Context：
                                   │        SetIsSuccessfulDebuff(true)
                                   │        SetDamageType(DamageType)
                                   │        SetDebuffDamage / _Duration / _Frequency
                                   ▼
                    PostGameplayEffectExecute ──► 同 Context
                                   │
                                   ▼
              UMy_AuraAttributeSet::HandleIncomingDamage     My_AuraAttributeSet.cpp:160
                                   │
                    ┌──────────────┼───────────────────────────┐
                    │              │                           │
              bFatal│         !bFatal│                           │
                    ▼              ▼                           ▼
        GetDeathImpulse    TryActivate(HitReact)      IsSuccessfulDebuff?
        → Die(Impulse)     GetKnockback → LaunchCharacter   → Debuff(Props)
                    │              │                           │
                    ▼              ▼                           ▼
        MulticastHandleDeath  （受击位移）           造 DOT GE（GrantedTags = Debuff Tag）
        Mesh / Weapon              每秒 IncomingDamage += DebuffDamage
        AddImpulse                 → 又回到 HandleIncomingDamage（循环）
```

### 52.3 ★ 三条链路对照表（最重要的一张表）

| | **Debuff** | **DeathImpulse** | **Knockback** |
|---|---|---|---|
| 原始参数 | `DebuffChance` / `Damage` / `Frequency` / `Duration` | `DeathImpulseMagnitude` | `KnockbackChance` / `Magnitude` |
| 参数存放 | BP → `Params` | BP → `Params` | BP → `Params` |
| **在哪判定** | **ExecCalc**（`My_DetermineDebuff`） | **不判定**（死了就飞） | **上游**：近战 `MakeDamageEffectParams…`／投射物 `OnSphereOverlap` |
| **为什么在那判定** | 需要**目标抗性**（只有 ExecCalc 抓得到）；且 Chance 已进 Spec | — | Chance **没进 GE**，下游读不到（见 52.7） |
| **结果放哪** | Context：`bIsSuccessfulDebuff` + `DamageType` + 3 个 float | `Params.DeathImpulse` → Context | `Params.Knockback` → Context |
| 结果类型 | bool + Tag + float | `FVector` | `FVector` |
| **谁消费** | `My_AuraAttributeSet::Debuff()` | `HandleIncomingDamage` 的 `bFatal` 分支 | `HandleIncomingDamage` 的 `!bFatal` 分支 |
| 最终表现 | 每秒掉血（DOT GE） | `Mesh` / `Weapon` 的 `AddImpulse` | `LaunchCharacter` |
| 触发时机 | 可能触发（几率） | **必触发**（只要致死） | 可能触发（几率） |

### 52.4 链路 A：Debuff（逐段）

| 段 | 位置 | 做什么 |
|---|---|---|
| ① | 能力 BP | 配 `Damage Type` + `DebuffChance/Damage/Frequency/Duration` |
| ② | `My_AuraDamageGameplayAbility.cpp:26-29` | 原样搬进 `Params`（**不判定**） |
| ③ | `My_AuraAbilitySystemLibrary.cpp:137-141` | `AssignTagSetByCallerMagnitude` 写进 **Spec** |
| ④ | `My_ExeCalc_Damage.cpp:72-152` | **判定**：读 Chance → 算「几率 × (100−抗性)/100」→ 掷骰 → 写 **Context** |
| ⑤ | `My_AuraAttributeSet.cpp:192-195` | `IsSuccessfulDebuff` 为真 → 调 `Debuff()` |
| ⑥ | `My_AuraAttributeSet.cpp:205-252` | 造动态 GE：`GrantedTags = DamageToDebuff[DamageType]`，`Period/Duration`，**按 Debuff Tag 分流加 Modifier** |
| ⑦ | GE 每 tick | `IncomingDamage += DebuffDamage` → 回到 `HandleIncomingDamage` |

**关键代码：**
```cpp
// ④ 判定（ExecCalc）
const float EffectiveDebuffChance = SourceDebuffChance * (100.f - TargetDebuffResistance) / 100.f;
const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
if (bDebuff)
{
    UMy_AuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
    UMy_AuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);
    UMy_AuraAbilitySystemLibrary::SetDebuffDamage / _Duration / _Frequency(...);
}
```

```cpp
// ⑥ 造 GE（AttributeSet）—— ★ 按 Debuff 种类分流
const FGameplayTag* DebuffTagPtr = GameplayTags.DamageToDebuff.Find(DamageType);
if (DebuffTagPtr == nullptr) { return; }                       // 找不到就跳过，不崩
Effect->InheritableOwnedTagsContainer.AddTag(*DebuffTagPtr);   // = GE 蓝图的 "Granted Tags"
Effect->Period = DebuffFrequency;
Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
Effect->bExecutePeriodicEffectOnApplication = false;           // 等第一个 Period 才掉血

if (DebuffTagPtr->MatchesTagExact(GameplayTags.My_Debuff_Burn))  // ★ 只有灼烧掉血
{
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.Attribute = UMy_AuraAttributeSet::GetIncomingDamageAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
    Effect->Modifiers.Add(ModifierInfo);
}
```

**「哪种伤害 → 哪种 Debuff」的表**（`My_AuraGamePlayTags_Singleton.cpp:305-309`）：

| 伤害类型 | Debuff Tag | 现在有数值效果吗 |
|---|---|---|
| `My_DamageType.Fire` | `My_Debuff.Burn` | ✅ 每秒掉血 |
| `My_DamageType.Lighting` | `My_Debuff.Stun` | ❌ 只挂 Tag |
| `My_DamageType.Arcane` | `My_Debuff.Arcane` | ❌ 只挂 Tag |
| `My_DamageType.Physical` | `My_Debuff.Physical` | ❌ 只挂 Tag |

> **「效果种类」由 Debuff Tag 决定，不是由 DamageType 决定。**
> 判断要写 `if (DebuffTagPtr->MatchesTagExact(...))` 而不是 `if (DamageType == ...)` ——
> 将来物理配一个「流血」，只要加个分支，伤害类型那边的逻辑不用动。

### 52.5 链路 B：DeathImpulse（逐段）

| 段 | 位置 | 做什么 |
|---|---|---|
| ① | 能力 BP | 配 `Death Impulse Magnitude`（6000~14000 量级） |
| ② | `My_AuraDamageGameplayAbility.cpp:43` | 近战：`Params.DeathImpulse = ToTarget * Magnitude`（**必给，不掷骰**） |
| ②' | `My_ProjectileActor.cpp:73` | 投射物：`= GetActorForwardVector() * Magnitude` |
| ③ | `My_AuraAbilitySystemLibrary.cpp:133` | `SetDeathImpulse(Context, Params.DeathImpulse)` → 写 **Context** |
| ④ | `My_AuraAttributeSet.cpp:169-178` | `bFatal` → `GetDeathImpulse(Context)` → `CombatInterface->Die(Impulse)` |
| ⑤ | `My_CombatInterface.h:60` | `virtual void Die(const FVector& DeathImpulse) = 0;`（**跨类抽象口**） |
| ⑥ | `MyCharacter_Base.cpp:107-110` | `Die()` → `MulticastHandleDeath(Impulse)`（NetMulticast） |
| ⑦ | `MyCharacter_Base.cpp:113-126` | `Weapon->AddImpulse(Impulse * 0.1f, NAME_None, true)`<br>`GetMesh()->AddImpulse(Impulse, NAME_None, true)` |

**方向差异（重要）：**

| 来源 | 方向算法 | 说明 |
|---|---|---|
| 近战 | `(Target - Self).Rotation()` + `Pitch = 45°` | 对着目标，抬高 45°（斜上飞） |
| 投射物 | `GetActorForwardVector()` | 顺着飞来的方向 |

**为什么 `Die()` 要带参数？**（第 27 章「接口居中翻译」的同一套路）
- `AttributeSet` **不能** `Cast<AAuraEnemy>`（会反向依赖、加新敌人要改数据层）
- 所以定义 `ICombatInterface::Die(const FVector&)` —— **「你是战斗单位，就得能按这个方向死」**
- 谁实现了接口谁就自动能用，`AttributeSet` 一行都不用改

**注意**：`AddImpulse` 是 **`UPrimitiveComponent`** 的方法，不在 `AActor` / `ACharacter` 上 ——
所以必须先 `GetMesh()` / `Weapon` 拿到组件再调。

### 52.6 链路 C：Knockback（逐段）

| 段 | 位置 | 做什么 |
|---|---|---|
| ① | 能力 BP | 配 `Knockback Chance`（示例 0 / 20）、`Knockback Magnitude`（示例 400 / 600） |
| ② | `My_AuraDamageGameplayAbility.cpp:49-52` | **近战：在这里掷骰** `<br>` `if (FMath::RandRange(1,100) < KnockbackChance) Params.Knockback = ToTarget * Magnitude;` |
| ②' | `My_ProjectileActor.cpp:76-89` | **投射物：撞人时掷骰** `<br>` 命中 → `Rotation.Pitch = 45°` → `Rotation.Vector() * Magnitude`；`else` 兜底清零 |
| ③ | `My_AuraAbilitySystemLibrary.cpp:134` | `SetKnockback(Context, Params.Knockback)` |
| ④ | `My_AuraAttributeSet.cpp:186-190` | 非致命分支：`GetKnockback(Context)` → `if (!IsNearlyZero(1.f))` → `Props.TargetCharacter->LaunchCharacter(Knockback, true, true)` |

```cpp
// ④ 消费
const FVector Knockback = UMy_AuraAbilitySystemLibrary::GetKnockback(Props.EffectContextHandle);
if (!Knockback.IsNearlyZero(1.f))
{
    Props.TargetCharacter->LaunchCharacter(Knockback, true, true);   // XY 和 Z 都覆盖
}
```

**为什么掷骰放"上游"而不是 `HandleIncomingDamage` 里？**

> **因为 Debuff 的 DOT 每 tick 也会走 `HandleIncomingDamage`。**
> 如果把掷骰放那里，就会变成「被灼烧时每 0.5 秒重新掷一次骰」——
> 烧着烧着人就被推着走。**一次攻击只能掷一次骰。**

### 52.7 ★ 判据：判定只能发生在「读得到 Chance 的地方」

| 位置 | 读得到 `DebuffChance`? | 读得到 `KnockbackChance`? |
|---|---|---|
| 能力（`Params`） | ✅ | ✅ |
| 投射物（成员 `DamageEffectParams`） | ✅ | ✅ |
| **Spec / ExecCalc** | ✅（走了 SetByCaller） | ❌ **没写进去** |
| **Context / AttributeSet** | ❌（只有判定结果） | ❌（只有结果向量） |

**根因**：`ApplyDamageEffect` 把 Debuff 的 4 个 float 都 `AssignTagSetByCallerMagnitude` 了，
**唯独没有 `My_Knockback_Chance`**：

```cpp
// My_AuraAbilitySystemLibrary.cpp:133-141
SetDeathImpulse(EffectContextHandle, Params.DeathImpulse);
SetKnockback(EffectContextHandle, Params.Knockback);          // ← 只传【结果向量】
FGameplayEffectSpecHandle EffectSpecHandle = ...MakeOutgoingSpec(...);

AssignTagSetByCallerMagnitude(EffectSpecHandle, Params.DamageType,   Params.BaseDamage);
AssignTagSetByCallerMagnitude(EffectSpecHandle, My_Debuff_Chance,    Params.DebuffChance);   // ★ 传了
AssignTagSetByCallerMagnitude(EffectSpecHandle, My_Debuff_Damage,    Params.DebuffDamage);
AssignTagSetByCallerMagnitude(EffectSpecHandle, My_Debuff_Duration,  Params.DebuffDuration);
AssignTagSetByCallerMagnitude(EffectSpecHandle, My_Debuff_Frequency, Params.DebuffFrequency);
// ← 没有 My_Knockback_Chance 这一行 → ExecCalc 读不到 → 只能在能力/投射物里判
```

**想让 Knockback 也去 ExecCalc 统一判定？要补两处：**
1. `ApplyDamageEffect` 里加一行 `AssignTagSetByCallerMagnitude(..., My_Knockback_Chance, Params.KnockbackChance)`
2. ExecCalc 里掷骰后 `SetKnockback(Spec.GetContext(), 方向向量)`

| 方案 | 优点 | 缺点 |
|---|---|---|
| **当前（能力 + 投射物各自掷）** | 改动小 | **两处代码，容易漏**（近战就漏过一次） |
| **搬进 ExecCalc** | 一处覆盖所有伤害路径；和 Debuff 对称 | 要补 SetByCaller + 方向计算 |

### 52.8 传输通道怎么选

| 数据 | 走哪条通道 | 原因 |
|---|---|---|
| 伤害值、Debuff 4 个 float | **SetByCaller**（`TMap<FGameplayTag, float>`） | 是 float，且 ExecCalc 要读 |
| `DeathImpulse` / `Knockback`（**FVector**） | **Context** | **`SetByCallerTagMagnitudes` 只能装 float，FVector 塞不进去** |
| `bIsSuccessfulDebuff` / `IsBlockedHit` / `IsCriticalHit` | **Context** | 布尔结果，非 float |

### 52.9 Context 的 NetSerialize 位表（加字段必看）

`FMY_AuraGamePlayEffectContext::NetSerialize`（`My_AuraAbilityTypes.cpp`）：

| 位 | 字段 |
|---|---|
| 0 | `Instigator` |
| 1 | `EffectCauser` |
| 2 | `AbilityCDO` |
| 3 | `SourceObject` |
| 4 | `Actors` |
| 5 | `HitResult` |
| 6 | `bHasWorldOrigin` |
| 7 | `bIsBlockedHit` |
| 8 | `bIsCriticalHit` |
| 9 | **`bIsSuccessfulDebuff`** |
| 10 | `DebuffDamage` |
| 11 | `DebuffDuration` |
| 12 | `DebuffFrequency` |
| 13 | `DamageType` |
| 14 | **`DeathImpulse`** |
| 15 | **`Knockback`** |

```cpp
// ★★ 位数 = 最大位号 + 1。当前最大位号是 15，所以写 16。
Ar.SerializeBits(&RepBits, 16);
```

> ⚠️ **加了新字段却忘了改这里 → 新字段静默丢失**
> （只在客户端失效，不报错、不崩溃、很难查）。

### 52.10 文件 ↔ 函数 责任表

| 文件 | 函数 | 在这三条链路里的角色 |
|---|---|---|
| `My_AuraDamageGameplayAbility.h` | `DebuffChance/Damage/Frequency/Duration`、`DeathImpulseMagnitude`、`KnockbackChance/Magnitude` | **三条链路的参数源头**（C++ 默认值） |
| `My_AuraDamageGameplayAbility.cpp` | `MakeDamageEffectParamsFromClassDefaults` | **打包 `Params`**；近战算 `DeathImpulse`；**近战掷 Knockback** |
| `My_ProjectileActor.cpp` | `OnSphereOverlap` | **投射物掷 Knockback**；投射物算 `DeathImpulse` |
| `My_AuraAbilitySystemLibrary.cpp` | `ApplyDamageEffect` | **分流器**：float → SetByCaller；FVector/bool → Context |
| 同上 | `Get/SetDeathImpulse`、`Get/SetKnockback`、`Get/SetDamageType`、`Get/SetDebuff…` | **Context 读写门面**（`static_cast` + 空指针检查） |
| `My_ExeCalc_Damage.cpp` | `Execute_Implementation` | 抓 4 个抗性、调 `My_DetermineDebuff`、算最终伤害 |
| 同上 | `My_DetermineDebuff` | **Debuff 判定**（掷骰 + 把结果写进 Context） |
| `My_AuraAttributeSet.cpp` | `HandleIncomingDamage` | **总调度**：致命→`Die`；非致命→HitReact + `LaunchCharacter`；最后→`Debuff` |
| 同上 | `Debuff` | **造 DOT GE**（GrantedTags + Period + 按 Debuff Tag 分流的 Modifier） |
| `My_CombatInterface.h` | `Die(const FVector&)` | 跨类调用的**抽象口**（让 AttributeSet 不依赖具体角色类） |
| `MyCharacter_Base.cpp` | `Die` / `MulticastHandleDeath` | **死亡表现**：`Mesh` / `Weapon` 的 `AddImpulse` |
| `Enemy_Characte.cpp` | `Die` | 敌人额外处理（`SetLifeSpan` + 黑板 `Dead`） |
| `My_AuraAbilityTypes.h` | `FMy_DamageEffectParams` | **参数原料箱**（BP → 打包 → 执行前） |
| 同上 | `FMY_AuraGamePlayEffectContext` | **结果箱**（跨 Spec / ExecCalc / Post 传递） |
| `My_AuraAbilityTypes.cpp` | `NetSerialize` | Context 的网络复制（位表见 52.9） |
| `My_AuraGamePlayTags_Singleton.cpp` | `DamageToDebuff` / `DamageToResistance` | **伤害类型 → Debuff / 抗性** 的映射表 |

### 52.11 本次修掉的两个坑

**坑 1：击退几率形同虚设（近战）**
```cpp
// ❌ 改前：无条件赋值 → KnockbackChance 填 0 也照样击退
Params.Knockback = ToTarget * KnockbackMagnitude;

// ✅ 改后：先掷骰
if (FMath::RandRange(1, 100) < KnockbackChance)
{
    Params.Knockback = ToTarget * KnockbackMagnitude;
}
```
外加投射物补 `else { DamageEffectParams.Knockback = FVector::ZeroVector; }` 兜底
（防止参数生成阶段预置过非零值）。

**坑 2：所有 Debuff 都掉血**
```cpp
// ❌ 改前：无条件加 IncomingDamage Modifier → Physical / Arcane / Stun 也每秒掉血
Effect->Modifiers.Add(ModifierInfo);

// ✅ 改后：按 Debuff 种类分流，只有 Burn 掉血
if (DebuffTagPtr->MatchesTagExact(GameplayTags.My_Debuff_Burn))
{
    Effect->Modifiers.Add(ModifierInfo);
}
```

**两个坑的共同点**：**「几率」这个词出现在代码里，但从来没有人读它。**
配置项在蓝图里静静地躺着，代码一边倒地无条件执行 ——
**排查这类问题的第一步：全局搜那个参数名，看它被读过几次。**

### 52.12 一句话总结

**三条链路共用一根骨架：
`能力 BP 配参数 → MakeDamageEffectParamsFromClassDefaults 打包 → ApplyDamageEffect 分流 → ExecCalc 判定 → AttributeSet 消费`。**

| | 判定在哪 | 结果怎么走 | 谁执行 |
|---|---|---|---|
| **Debuff** | **ExecCalc**（要目标抗性） | Context：bool + Tag + float | `Debuff()` 造 DOT GE → 每秒掉血 |
| **DeathImpulse** | 不判定（致死必给） | `Params` → Context（FVector） | `Die()` → `MulticastHandleDeath` → `AddImpulse` |
| **Knockback** | **上游**（能力 / 投射物，因为 Chance 没进 GE） | `Params` → Context（FVector） | `HandleIncomingDamage` → `LaunchCharacter` |

**判据：判定只能发生在「读得到 Chance 的地方」。**
Debuff 的 Chance 走了 SetByCaller 进 Spec → ExecCalc 能判；
Knockback 的 Chance 没进 GE → 只能在调用 `ApplyDamageEffect` 之前判。

**传输通道的规矩：float 走 SetByCaller（ExecCalc 要读），FVector / bool 走 Context。**

**`Die(const FVector&)` 之所以要带参数：让 `AttributeSet` 通过 `ICombatInterface` 调用，
不 `Cast` 到任何具体角色类（同第 27 章「接口居中翻译」）。**
