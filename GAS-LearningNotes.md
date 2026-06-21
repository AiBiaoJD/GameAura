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



## 十二、UI 架构：HUD → WidgetController → Widget

### 继承层级



### 两条独立的数据通路

**玩家**（HUD 体系）：


**敌人**（自身兼任 WidgetController）：


### 三层委托链



### TSubclassOf vs TObjectPtr

# 0 "<stdin>"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "<stdin>"

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


## 十二、UI 架构：HUD → WidgetController → Widget

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

### 两条独立的数据通路

**玩家（HUD 体系）**：
```
Aura_Character.cpp:85 → AuraHUD->InitOverlay(PC, PS, ASC, AS)
  └─ My_AuraHUD.cpp:33  InitOverlay()
       ├─ NewObject<UMy_OverlayWidgetController>     ← 创建独立控制器
       ├─ BindCallbacksToDependencies()              ← 接线：ASC 属性委托 → 控制器委托
       │    My_OverlayWidgetController.cpp:27-30
       │    ASC.GetGameplayAttributeValueChangeDelegate(Health)
       │       .AddLambda(→ OnHealthChanged.Broadcast(新值))
       ├─ CreateWidget(OverlayWidgetClass)           ← 创建血条 Widget
       ├─ Widget.SetWidgetController(Controller)     ← 关联 → 触发 BP WidgetControllerSet
       ├─ BroadcastInitiaValues()                     ← 立即广播当前值
       └─ Widget.AddToViewport()
```

**敌人（自身兼任 WidgetController）**：
```
Enemy_Characte.cpp:104  BeginPlay()
  ├─ My_InitAbilityActorInfo()
  ├─ Widget.SetWidgetController(this)               ← 敌人自己就是控制器
  │    → 触发 BP WidgetControllerSet → Cast to AEnemy_Characte → 绑定 OnHealthChanged
  ├─ ASC.GetGameplayAttributeValueChangeDelegate(Health)
  │     .AddLambda(→ OnHealthChanged.Broadcast(新值))  Enemy_Characte.cpp:123-126
  └─ OnHealthChanged.Broadcast(当前值)                Enemy_Characte.cpp:134
```

### 三层委托链

```
GAS 层(引擎)          →  控制器层(解耦中间人)     →  UI 层(显示)
ASC 属性变化委托        OnHealthChanged 自定义委托   BP 事件更新血条
C++ 接线               C++ 暴露，BlueprintAssignable  BP 订阅
```

### TSubclassOf vs TObjectPtr

```cpp
UPROPERTY(EditAnywhere)
TSubclassOf<UMy_AuraUserWidget> OverlayWidgetClass;  // 配方：蓝图里选"哪个类"

UPROPERTY()
TObjectPtr<UMy_AuraUserWidget> OverlayWidget;         // 蛋糕：运行时 CreateWidget 造出来的实例
```

---

## 十三、virtual vs BlueprintNativeEvent vs BlueprintImplementableEvent vs BlueprintCallable

| 关键字 | C++ 默认实现 | BP 可调 | BP 可覆盖 | 适用场景 |
|--------|:-----------:|:------:|:--------:|---------|
| `BlueprintCallable` | ✅ 有 | ✅ | ❌ | 逻辑固定，所有类型一样 |
| `BlueprintNativeEvent` | ✅ 有(`_Implementation`) | ✅ | ✅ | C++ 给默认，BP 可选覆盖 |
| `BlueprintImplementableEvent` | ❌ 无 | ✅ | ✅(必须) | 纯表现层，C++ 不管 |
| 无 UFUNCTION 的 `virtual` | ✅ 或 `=0` | ❌ | ✅(C++子类) | 纯 C++ 内部调用 |

**核心区别**：`virtual` = C++ 子类可重写，`BlueprintNativeEvent` = BP 子类也可重写。UE 的 UFUNCTION 会自动生成 virtual，所以接口函数不加 virtual 也是虚函数。

**BlueprintNativeEvent 的真正作用**：让 BP 覆盖**逻辑**，不是换数据。不同类型配不同 Montage 通过 UPROPERTY 就够，不需要 NativeEvent。NativeEvent 留给"同一个函数，不同 BP 需要不同分支逻辑"的场景。

---

## 十四、Live Coding 与 CDO 缓存

### CDO (Class Default Object)

每个 UClass 启动时创建的一个模板对象，BP 编译后的字节码、属性绑定、所有 C++ 属性偏移量全固化在里面。

### 内存布局

C++ 对象在内存中是一段连续字节，父类成员在前，子类成员在后。BP 编译时记的是**硬偏移量**而非属性名：

```
改前: [...父类成员...DeathSound(0xC8)][OnHealthChanged(0xE0)][OnMaxHealthChanged(0xE8)]
改后: [...父类成员...DeathSound(0xC8)][MinionCount(0xD0)][OnHealthChanged(0xE4)][OnMaxHealthChanged(0xEC)]
                                                                         ↑ 偏移全错
```

### 为什么有时 Live Coding 可以，有时不行

| 能 Hot Reload | 不能（必须重启编辑器） |
|--------------|---------------------|
| 函数体 `.cpp` 修改 | 增删 UPROPERTY |
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
   - ExeCalc 读等级：`Cast<IMy_CombatInterface>(Actor)->GetPlayerLevel()`
   - 比 `Cast<AEnemy_Characte>` 强——不依赖具体类

2. **完全不相关的类型共享同一个行为**
   - 角色、木桶、可破坏门都需要 `Die()`，但不可能共享一个基类

3. **蓝图避免 Cast 地狱**
   - `DoesImplementInterface → 调接口函数` vs 每个类型写一个分支

### 什么时候不用接口

接口**不能有成员变量、不能有非虚函数体、不能有构造函数逻辑**。数据（ASC, AttributeSet）和通用逻辑（InitAbilityActorInfo）必须放基类。

```
接口管"能做什么"（行为契约）    基类管"有什么东西"（数据 + 通用实现）
GetPlayerLevel()                  ASC, AttributeSet, Health
Die()                            My_InitAbilityActorInfo(), ApplyEffectToSelf()
GetWeaponSockLocation()          Weapon 组件
```

### 接口两种声明方式的影响

```cpp
virtual void Die() = 0;           // 纯虚：子类必须实现，否则编译不过 → 编译器强制执行
UFUNCTION(BlueprintNativeEvent)   // NativeEvent：有空的默认实现，没写就走空 → 静默无效果
void Die();                       //
```

### GAS 项目不一定需要额外伤害接口

UE 内置的 `IAbilitySystemInterface` 已经提供 `GetAbilitySystemComponent()`。只要所有会受伤的东西都挂 ASC，`CauseDamage` 通过 `ApplyGameplayEffectSpecToTarget` 统一处理，不需要 `IDamageable`。只有无 ASC 的东西（如简单木桶）才考虑新接口。

### 实际开发节奏

前期不急着设计接口 → 类型多了 Cast 写到痛 → 自然知道该抽什么。但以下接口建议早期就写：**伤害/受伤**、**交互系统**、**存档/读档**。UE 引擎本身的 `IAbilitySystemInterface`、`INavAgentInterface` 就是这个思路。
