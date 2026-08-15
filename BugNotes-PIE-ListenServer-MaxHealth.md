# Bug 记录：PIE 2人 ListenServer 主机 MaxHealth 升级不重算（MMC 捕获链被断）

- 日期：2026-08-16
- 状态：**代码已退回教程原版**，待确认教程是否后续处理
- 结论预告：这是 PIE 测试工具的产物，**打包后的多人游戏不会有**（分析结论，建议打包实测确认）

---

## 一、现象

| 环境 | 主机升级后 MaxHealth |
|------|---------------------|
| 单人 PIE | 正常（430 → 440） |
| **2人 PIE ListenServer 主机** | **钉在 430 不升** |
| 2人 PIE ListenServer 客户端 | 正常（430 → 440） |

## 二、复现

1. PIE 设 2 个玩家，Net Mode = Play As Client / Listen Server
2. 主机吃经验升级（多打怪）
3. 观察主机 UI 的 MaxHealth：不升；客户端正常升到 440

## 三、核心证据（调试日志，已随回退移除）

- `[InitActorInfo]` 显示共 **4 个 pawn**（2×Auth=1 服务器 pawn + 2×Auth=0 客户端视图镜像 pawn）
- **主机**上 XP GE 施加时 `[MMC-MaxHealth]` **0 次** —— 捕获链从开始就是断的
- **客户端**正常触发 MMC 重算（Level=1 也会触发）

## 四、根因分析

1. PIE 2 人时，编辑器给**主机的连接**也单独建了一个"客户端视图世界" → 主机多出一个 **Auth=0 的镜像 pawn**
2. 该镜像 pawn 与主机真实 pawn **共享权威 PlayerState/ASC**（主机 PC 驻留服务器）
3. 镜像 pawn 二次执行 `My_InitAbilityActorInfo()` → 在权威 ASC 上**二次施放属性 GE**（`InitializeDefaultAttribute`）
4. 二次施放覆盖 MMC 捕获链 → MaxHealth 的聚合器不再依赖 VIgor → 升级时全局刷新只重求值 VIgor、**MMC 不再跟随**

> 客户端玩家没有此问题：它的镜像 pawn 用的是**独立的非权威 ASC**，二次施放 GE 是 no-op（仅 UI 显示，不改数据）。

## 五、为什么打包后没有（分析，未实测）

| 环境 | 结构 | 会不会二次施放 |
|------|------|--------------|
| 打包 Listen Server | 主机视图 = 服务器世界本身，一个 pawn、一次 PossessedBy（同单人，一直正常） | 不会 |
| 打包 Dedicated Server | 主机玩家是普通客户端，独立进程，非权威 ASC，GE 施放 no-op（同 PIE 客户端，一直正常） | 不会 |
| **PIE 2 人** | 主机多一个镜像 pawn，共享权威 ASC | **会** ← 唯一 |

**镜像 pawn 只在编辑器多窗口 PIE 里存在。** 打包后建议实测一次 2 人升级确认。

## 六、尝试过的修法（全部已回退）

| 方案 | 做法 | 结果 |
|------|------|------|
| A' | 按 avatar 判断跳过整个初始化 | ❌ 弄没了 UI/技能，废弃 |
| B | 升级时显式重施副属性 GE（移除旧实例+按新等级重施） | ❌ 只治标，其他影响 Vigor/MaxHealth 的 GE（道具/Buff/属性点）仍会坏，废弃 |
| **C** | `InitializeDefaultAttribute` 加"副属性GE已生效则跳过"守卫 | ✅ 技术上能修（语义判断、不碰 UI/技能），但决定先回退，看教程是否后续处理 |

## 七、如果教程没处理，恢复修法 C

只需改 `Source/Aura/Private/My_Character/Aura_Character.cpp` 的 `My_InitAbilityActorInfo()` 末尾：

```cpp
	//4.使用Effect初始化Aura的PrimaryAttribute
	//守卫：副属性GE已生效则不再施放（初始化是幂等的，只需一次）。
	bool bSecondaryAlreadyApplied = false;
	for (const FActiveGameplayEffect& ActiveGE : AbilitySystemComponent->GetActiveGameplayEffects())
	{
		if (ActiveGE.Spec.Def && DefaultSecondAttributeEffectClass == ActiveGE.Spec.Def->GetClass())
		{
			bSecondaryAlreadyApplied = true;
			break;
		}
	}
	if (!bSecondaryAlreadyApplied)
	{
		InitializeDefaultAttribute();
	}
```

> 注意：`Aura_Character.cpp` 是 **UTF-8**，可直接用编辑器/Edit 工具改。

## 八、关键代码位置

- `Source/Aura/Private/My_Character/Aura_Character.cpp` — `My_InitAbilityActorInfo()`（双路径：`PossessedBy` 服务器 + `OnRep_PlayerState` 客户端）
- `Source/Aura/Private/My_Character/MyCharacter_Base.cpp` — `InitializeDefaultAttribute()` / `ApplyEffectToSelf()`（施放 Primary/Secondary/Vital GE）
- `Source/Aura/Private/MY_AbilitySystem/ModMagCale/My_MMC_MaxHealth.cpp` — MMC **实时读 Level**（`Spec.GetContext().GetSourceObject()` → `Execute_GetPlayerLevel`），所以显式重施即可重算
