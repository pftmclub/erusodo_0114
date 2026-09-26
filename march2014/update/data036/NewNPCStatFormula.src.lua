function CalculateExtraStatValue(slot0, slot1, slot2, slot3, slot4, slot5, slot6, slot7)
	if EXTRA_NPC_STAT_TYPE.ENST_HP == slot0 then
		return math.floor((340 + 60 * slot4) * (9.994 + 0.006 * slot6 * slot6) * slot5 * GetHPFactorByGameType(slot4, slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_PHYSIC_ATT == slot0 then
		return math.floor((340 + 60 * slot4) * (0.98 + 0.02 * slot6) * slot5 * GetAttackFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_MAGIC_ATT == slot0 then
		return math.floor((340 + 60 * slot4) * (0.98 + 0.02 * slot6) * slot5 * GetAttackFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_PHYSIC_DEF == slot0 or EXTRA_NPC_STAT_TYPE.ENST_MAGIC_DEF == slot0 then
		slot8 = 0

		return math.floor((slot7 == 1 and slot5 * DefFactor(slot4) * HenirDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * HenirDef(slot4)) or slot7 == 2 and slot5 * DefFactor(slot4) * SecretNormDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * SecretNormDef(slot4)) or slot7 == 3 and slot5 * DefFactor(slot4) * SecretLutoDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * SecretLutoDef(slot4)) or slot5 * DefFactor(slot4) * NormalDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * NormalDef(slot4))) * GetDefenseFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_ACCURACY == slot0 then
		return slot1 * (slot2 * slot4 + slot3) * slot5 + 1
	elseif EXTRA_NPC_STAT_TYPE.ENST_AVOIDANCE == slot0 then
		return slot1 * (slot2 * slot4 * slot4 + slot3) / 1000 * slot5 + 1
	end

	return slot5
end

function HenirDef(slot0)
	return 110.4 + 19.368 * slot0 - 0.11956 * slot0 * slot0 + 0.00039539 * slot0 * slot0 * slot0
end

function NormalDef(slot0)
	return 67.215 + 12.554 * slot0
end

function SecretNormDef(slot0)
	return 195.17 + 41.191 * slot0
end

function SecretLutoDef(slot0)
	return 273.09 + 63.979 * slot0
end

function DefFactor(slot0)
	return 258.65 + 39.35 * slot0
end

function GetHPFactorByGameType(slot0, slot1)
	if slot1 == 2 then
		return 0.95 * math.pow((slot0 - 70) / 36, 2.5) + 1.05
	elseif slot1 == 3 then
		return 1 * math.pow((slot0 - 70) / 36, 3.5) + 1.5
	else
		return 1
	end
end

function GetAttackFactorByGameType(slot0)
	if slot0 == 2 then
		return 1.5
	elseif slot0 == 3 then
		return 2
	else
		return 1
	end
end

function GetDefenseFactorByGameType(slot0)
	if slot0 == 2 or slot0 == 3 then
		return 1.2
	else
		return 1
	end
end
