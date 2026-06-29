# GAS 架构学习笔记

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
