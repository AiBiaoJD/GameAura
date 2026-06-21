# GAS Attribute → UI Widget 完整流程

> 学习日期: 2026-06-22

## 核心分层

```
① ASC（数据层）     → 属性变化触发委托
② WidgetController（翻译层）→ GAS 原生委托→自定义委托（蓝图可用）
③ Widget（显示层）   → 蓝图绑定、更新 UI
```

## 委托桥接（为什么需要中间层）

GAS 内置 `GetGameplayAttributeValueChangeDelegate` 返回的是 C++ 原生委托，**蓝图不能直接绑定**。
WidgetController 用 Lambda 将其转换为 `BlueprintAssignable` 动态委托，Widget 蓝图才能 Bind Event。

```
ASC 原生委托（DECLARE_MULTICAST_DELEGATE）
  → AddLambda → OnHealthChanged.Broadcast(Data.NewValue)
    → 蓝图 Bind Event → SetPercent/SetText
```

## 玩家流程（三阶段）

### 初始化阶段
1. **PlayerState 构造**（My_AuraPlayerState.cpp:15-20）`CreateDefaultSubobject<ASC>` + `CreateDefaultSubobject<AS>`
2. **PossessedBy / OnRep_PlayerState**（Aura_Character.cpp:33/49）→ `My_InitAbilityActorInfo()`
3. `InitAbilityActorInfo(PlayerState, this)`（Aura_Character.cpp:67）告诉 ASC "我是谁"
4. `AbilityActorInfoSet()`（My_AuraAbilitySystemComponent.cpp:15-20）绑 GE 应用回调 → RPC
5. `HUD->InitOverlay(PC, PS, ASC, AS)`（Aura_Character.cpp:85）
6. HUD 内（My_AuraHUD.cpp:33-54）：
   - `CreateWidget` 创建 Widget
   - `GetOverlayWidgetController` 创建 Controller → `BindCallbacksToDependencies()` 绑定 ASC 委托
   - `Widget->SetWidgetController(OWC)` → 触发蓝图 `WidgetControllerSet()` 事件
   - `BroadcastInitiaValues()` 推送当前值给 Widget
7. `InitializeDefaultAttribute()`（Aura_Character.cpp:91）应用初始 GE，属性 0→100，触发委托

### 运行时阶段
1. GE 修改属性 → `PreAttributeChange()`（AS.cpp:82）钳制
2. GAS 内部触发 `GetGameplayAttributeValueChangeDelegate`
3. WidgetController Lambda 收到 → `OnHealthChanged.Broadcast(NewValue)`
4. Widget 蓝图收到 → 更新 UI
5. `PostGameplayEffectExecute()`（AS.cpp:104）处理 IncomingDamage/HitReact/致死

### 消息 UI 走不同路线
1. GE 应用 → `OnGameplayEffectAppliedDelegateToSelf`（仅服务端）
2. `ClientEffectApplied` RPC（ASC.cpp:22-31）→ 客户端提取 AssetTags
3. `EffectAssetTags.Broadcast(TagContainer)` → WidgetController Lambda 检查 Message 标签
4. `OnMessageWidgetRow.Broadcast(Row)` → Widget 显示消息

## 敌人流程（简化）

敌人自己就是 WidgetController，没有 HUD 层：

**BeginPlay()**（Enemy_Characte.cpp:104-145）：
1. `My_InitAbilityActorInfo()` — `InitAbilityActorInfo(this, this)`，敌人自己就是 Owner + Avatar
2. `SetWidgetController(this)` — Widget 绑到敌人自身
3. ASC 委托 `.AddLambda → OnHealthChanged.Broadcast` — 手写翻译
4. `OnHealthChanged.Broadcast(当前值)` — 推初始值

## 关键区别

| | 玩家 | 敌人 |
|---|---|---|
| ASC/AS 位置 | PlayerState | Character 自己 |
| WidgetController | UMy_OverlayWidgetController | AEnemy_Characte 自身 |
| 委托绑定 | BindCallbacksToDependencies | BeginPlay 手写 Lambda |
| HUD | AMy_AuraHUD 管理 | 无 |

## 委托种类

| 委托 | 类型 | 定义位置 | 用途 |
|------|------|---------|------|
| GetGameplayAttributeValueChangeDelegate | C++ 原生 | GAS 内置 | 属性值变化触发 |
| OnHealthChanged | BlueprintAssignable | OverlayWidgetController.h:52 | 血条更新 |
| EffectAssetTags | C++ 原生 | ASC.h | GE 应用通知 |
| OnMessageWidgetRow | BlueprintAssignable | OverlayWidgetController.h:64 | 消息 UI |
| OnAttributeInfo | BlueprintAssignable | AttributeMenuWidgetController.h:30 | 属性面板 |

## GAS 上下文结构速查

| 结构 | 一句话 | 位置 |
|------|--------|------|
| FAbilityActorInfo | ASC 的 "我是谁" | ASC 内部，InitAbilityActorInfo 设置 |
| FGameplayEffectContext | GE 的 "谁打的" | GE Spec 内部 |
| FGameplayEffectSpec | GE 完整数据包 | 应用 GE 时 |
| FGameplayEffectModCallbackData | PostGE 回调参数 | PostGameplayEffectExecute() |
| FMy_EffectProperties | 从上面提取的简化版 | AS.cpp:176 SetEffectProperty() |
| FOnAttributeChangeData | 属性变了多少 | Lambda 参数 Data.NewValue |

## GE 执行顺序

1. PreAttributeChange(Attribute, NewValue) — 可修改 NewValue（钳制）
2. GAS 写入属性值 → 触发 GetGameplayAttributeValueChangeDelegate
3. PostAttributeChange(Attribute, OldValue, NewValue)
4. PostGameplayEffectExecute(Data) — IncomingDamage / SetHealth（可能再次触发②）

## WidgetController 四个参数

传递 PC, PS, ASC, AS。当前**实际只用到了 ASC 和 AS**。
PC 和 PS 是预留的（读玩家等级、控制输入等）。

## PlayerState 和 PlayerController 在 GAS 中的角色

- **PlayerState** = 替身。Characters 会销毁，PS 跨关卡存活，ASC/AS 放 PS 上不丢
- **PlayerController** = 执剑人。处理输入、拥有 HUD、有网络权限发 RPC
- **HUD** 由 GameMode 的 HUDClass 配置，引擎自动 spawn 并挂在 PlayerController 上

## 调用时机

- **PossessedBy** → 服务端，四个参数都有效（PC/PS/ASC/AS）
- **OnRep_PlayerState** → 客户端，PS 网络复制完成
- **BeginPlay** → 客户端 PS 可能还没复制，不能用 InitOverlay
- AbilityActorInfoSet() 必须在 InitAbilityActorInfo() 之后（依赖 ActorInfo 有效）

## MVVM 概念

Model-View-ViewModel，UE5.5 成熟。把你手写的委托翻译自动化：
- 变量标记 `FieldNotify` → 引擎自动通知
- Widget 属性右键 Create Binding → 选 ViewModel 变量
- 不需要自定义委托、不需要 Lambda 转发、不需要 BroadcastInitiaValues
- ViewModel 本质上就是 WidgetController，只是通知机制从手动变为引擎自动
