# Bug 记录：PIE 2人 ListenServer 报 AbilityTag [None]（被动技能无 My_Abilities tag）

- 日期：2026-08-16
- 状态：**已修复**（守卫跳过无 tag 的能力）
- 结论：良性报错，不影响功能。根源与 MaxHealth bug 同属 PIE 2P 复制时序，但表现完全不同。

---

## 一、现象

PIE 设 2 个玩家 ListenServer 时，日志报：

```
LogAura: Error: Can not find info form AbilityTag [None] on AbilityInfo [My_DA_AbilityInfo]
```

单人 PIE 不报。

## 二、报错链路

```
My_OverlayWidgetController::OnInitializeStartupAbilities   // 遍历 ASC 所有能力
  → ForEachAbility
  → GetAbilityTagFromAbilitySpec(AbilitySpec)              // 在 AbilityTags 里找 "My_Abilities.*" 前缀
  → 该能力没配 → 返回 None
  → FindAbilityInfoFromTag(None)                           // 查不到 → 报 [None] Error
```

即：**ASC 里至少有一个能力的 `AbilityTags` 没有 `My_Abilities.*` tag**——是被动技能（如 `My_GA_ListenForEvent`）。被动是内部机制，不需要技能图标，所以没配 tag。

## 三、为什么 1P 不报、2P 报（核心）

**服务器给能力有先后顺序**（`AMyCharacter_Base::AddCharacterAbilities`）：

```cpp
ASC->AddCharacterAbilitiesFromASC(StartupAbility);          // ① 先给主动技能
// ② 结束时就广播 OnAbilityGiven → Overlay 遍历技能
//    ← 此刻被动还没给，遍历不到它 → 不报错
ASC->AddCharacterPassiveAbilitiesFromASC(StartupPassiveAbility);  // ③ 后给被动技能
```

- **1P（服务器自己）**：广播发生在 ③ 之前 → 被动不在列表 → 不报错
- **2P**：客户端能力是**整体复制**过来的，`OnRep_ActivateAbilities` 触发广播时被动已在列表 → 遍历到它 → 无 tag → [None]。主机镜像 pawn 二次初始化同理。

## 四、有没有害

没有。被动技能本来就不该有图标，widget 广播一个空的 `FMy_AuraAbilityInfo`，UI 忽略。只是日志难看。

## 五、修法（已应用）

`My_OverlayWidgetController.cpp` 的 `OnInitializeStartupAbilities` lambda 加守卫，跳过无 tag 的能力：

```cpp
const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromAbilitySpec(AbilitySpec);
if (AbilityTag.IsValid())
{
    FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTag);
    info.InputTag = AuraASC->GetInputTagFromAbilitySpec(AbilitySpec);
    OnAbilityInfo.Broadcast(info);
}
```

（该文件是 UTF-8，可直接用编辑器改）

## 六、关键代码位置

- `Source/Aura/Private/My_UI/WidgetController/My_OverlayWidgetController.cpp` — `OnInitializeStartupAbilities`（报错入口，已加守卫）
- `Source/Aura/Private/MY_AbilitySystem/Data/My_AbilityInfo.cpp` — `FindAbilityInfoFromTag`（Error 日志来源，无条件报错）
- `Source/Aura/Private/My_Character/MyCharacter_Base.cpp` — `AddCharacterAbilities`（主动先给、被动后给的顺序）
- `Source/Aura/Private/MY_AbilitySystem/My_AuraAbilitySystemComponent.cpp` — `GetAbilityTagFromAbilitySpec` / `AddCharacterPassiveAbilitiesFromASC`
