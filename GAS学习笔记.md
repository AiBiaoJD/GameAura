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
