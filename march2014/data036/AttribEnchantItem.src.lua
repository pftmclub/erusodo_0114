g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 90,
	m_Time = 5,
	m_fRate = 0.05,
	m_fAttribReiterationRate = 0.1,
	m_ThirdDamagePercent = 120,
	m_fBlazeMpDrain = 3,
	m_FirstDamagePercent = 60,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_BLAZE
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 27,
	m_fRate = 0.3,
	m_fAttribReiterationRate = 0.6,
	m_ThirdDamagePercent = 36,
	m_fBlazeMpDrain = 3,
	m_FirstDamagePercent = 18,
	m_Time = 5,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_BLAZE,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.06,
	m_FirstDefenceDebuffPercent = 10,
	m_fAttribReiterationRate = 0.06,
	m_ThirdDefenceDebuffPercent = 30,
	m_Time = 10,
	m_SecondDefenceDebuffPercent = 20,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_FROZEN,
	m_arrAttackSpeedDecrease = {
		10,
		20,
		30
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_ThirdDefenceDebuffPercent = 45,
	m_FirstDefenceDebuffPercent = 20,
	m_fAttribReiterationRate = 0.36,
	m_fRate = 0.36,
	m_Time = 10,
	m_SecondDefenceDebuffPercent = 30,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_FROZEN,
	bIsForNPC = True,
	m_arrAttackSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 37.5,
	m_fRate = 0.05,
	m_fAttribReiterationRate = 0.05,
	m_ThirdDamagePercent = 50,
	m_Time = 15,
	m_FirstDamagePercent = 25,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_POISON,
	m_arrMovementSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 14,
	m_fRate = 0.3,
	m_fAttribReiterationRate = 0.3,
	m_ThirdDamagePercent = 18,
	m_FirstDamagePercent = 8,
	m_Time = 15,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_POISON,
	bIsForNPC = True,
	m_arrMovementSpeedDecrease = {
		30,
		45,
		60
	},
	m_arrJumpSpeedDecrease = {
		30,
		45,
		60
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fPiercingDamageFactor = 40,
	m_fRate = 0.06,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_PIERCING
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.36,
	m_fPiercingDamageFactor = 40,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_PIERCING,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.05,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 3,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_SHOCK
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.3,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 2,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_SHOCK,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_HPDrainPercent = 20,
	m_fRate = 0.05,
	m_MPDrain = 4,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_SNATCH
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.3,
	m_HPDrainPercent = 13,
	m_MPDrain = 6,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_SNATCH,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 180,
	m_Time = 5,
	m_fRate = 0.08,
	m_fAttribReiterationRate = 0.16,
	m_ThirdDamagePercent = 240,
	m_fBlazeMpDrain = 4,
	m_FirstDamagePercent = 120,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_BLAZE_MASTER
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 54,
	m_fRate = 0.48,
	m_fAttribReiterationRate = 0.96,
	m_ThirdDamagePercent = 72,
	m_fBlazeMpDrain = 4,
	m_FirstDamagePercent = 36,
	m_Time = 5,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_BLAZE_MASTER,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.12,
	m_FirstDefenceDebuffPercent = 10,
	m_fAttribReiterationRate = 0.12,
	m_ThirdDefenceDebuffPercent = 30,
	m_Time = 10,
	m_SecondDefenceDebuffPercent = 20,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_FROZEN_MASTER,
	m_arrAttackSpeedDecrease = {
		10,
		20,
		30
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_ThirdDefenceDebuffPercent = 45,
	m_FirstDefenceDebuffPercent = 20,
	m_fAttribReiterationRate = 0.72,
	m_fRate = 0.72,
	m_Time = 13,
	m_SecondDefenceDebuffPercent = 30,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_FROZEN_MASTER,
	bIsForNPC = True,
	m_arrAttackSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 75,
	m_fRate = 0.08,
	m_fAttribReiterationRate = 0.08,
	m_ThirdDamagePercent = 100,
	m_Time = 15,
	m_FirstDamagePercent = 50,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_POISON_MASTER,
	m_arrMovementSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 24,
	m_fRate = 0.48,
	m_fAttribReiterationRate = 0.48,
	m_ThirdDamagePercent = 32,
	m_FirstDamagePercent = 16,
	m_Time = 15,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_POISON_MASTER,
	bIsForNPC = True,
	m_arrMovementSpeedDecrease = {
		30,
		45,
		60
	},
	m_arrJumpSpeedDecrease = {
		30,
		45,
		60
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fPiercingDamageFactor = 40,
	m_fRate = 0.12,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_PIERCING
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.72,
	m_fPiercingDamageFactor = 40,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_PIERCING,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.08,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 3.5,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_SHOCK
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.6,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 2.5,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_SHOCK,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_HPDrainPercent = 40,
	m_fRate = 0.08,
	m_MPDrain = 6,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_SNATCH
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.48,
	m_HPDrainPercent = 26,
	m_MPDrain = 12,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_MASTER_SNATCH,
	bIsForNPC = True
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 270,
	m_Time = 5,
	m_fRate = 0.1,
	m_fAttribReiterationRate = 0.2,
	m_ThirdDamagePercent = 360,
	m_fBlazeMpDrain = 5,
	m_FirstDamagePercent = 180,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_BLAZE_DOUBLE_MASTER
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.15,
	m_FirstDefenceDebuffPercent = 10,
	m_fAttribReiterationRate = 0.15,
	m_ThirdDefenceDebuffPercent = 30,
	m_Time = 10,
	m_SecondDefenceDebuffPercent = 20,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_FROZEN_DOUBLE_MASTER,
	m_arrAttackSpeedDecrease = {
		10,
		20,
		30
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 112.5,
	m_fRate = 0.1,
	m_fAttribReiterationRate = 0.1,
	m_ThirdDamagePercent = 150,
	m_Time = 15,
	m_FirstDamagePercent = 75,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_POISON_DOUBLE_MASTER,
	m_arrMovementSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fPiercingDamageFactor = 40,
	m_fRate = 0.15,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_DOUBLE_MASTER_PIERCING
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fTimeStun = 4,
	m_fRate = 0.1,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_DOUBLE_MASTER_SHOCK_OLD
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_HPDrainPercent = 60,
	m_fRate = 0.1,
	m_MPDrain = 8,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_DOUBLE_MASTER_SNATCH
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 360,
	m_Time = 5,
	m_fRate = 0.12,
	m_fAttribReiterationRate = 0.24,
	m_ThirdDamagePercent = 480,
	m_fBlazeMpDrain = 6,
	m_FirstDamagePercent = 240,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_BLAZE_MASTER
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.18,
	m_FirstDefenceDebuffPercent = 10,
	m_fAttribReiterationRate = 0.18,
	m_ThirdDefenceDebuffPercent = 30,
	m_Time = 10,
	m_SecondDefenceDebuffPercent = 20,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_FROZEN_MASTER,
	m_arrAttackSpeedDecrease = {
		10,
		20,
		30
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_SecondDamagePercent = 150,
	m_fRate = 0.12,
	m_fAttribReiterationRate = 0.12,
	m_ThirdDamagePercent = 200,
	m_Time = 15,
	m_FirstDamagePercent = 100,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_POISON_MASTER,
	m_arrMovementSpeedDecrease = {
		20,
		30,
		40
	}
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.1,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 4,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_DOUBLE_MASTER_SHOCK
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fPiercingDamageFactor = 40,
	m_fRate = 0.18,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_PIERCING_MASTER
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_fRate = 0.12,
	m_fShockDebuffDamageFactor = 0.3,
	m_fTimeStun = 4.5,
	m_iShockDebuffTime = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_SHOCK_MASTER
})
g_pCX2EnchantItem:AddEnchantData_LUA({
	m_HPDrainPercent = 80,
	m_fRate = 0.12,
	m_MPDrain = 10,
	m_ExtraDamageType = EXTRA_DAMAGE_TYPE.EDT_ENCHANT_QUADRA_SNATCH_MASTER
})
