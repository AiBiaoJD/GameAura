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
