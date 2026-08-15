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
```