function GetAttribEnchantRequiredED(slot0, slot1, slot2, slot3)
	return (slot2 + GET_ED_FACTOR_BY_GRADE(slot3)) * 100 * GET_ATTRIB_FACTOR_BY_ENCHANT_COUNT(slot0) * GET_EQUIP_FACTOR(slot1)
end

function GET_ED_FACTOR_BY_GRADE(slot0)
	if slot0 == ITEM_GRADE.IG_RARE then
		return 2
	elseif slot0 == ITEM_GRADE.IG_ELITE then
		return 4
	elseif slot0 == ITEM_GRADE.IG_UNIQUE then
		return 6
	elseif slot0 == ITEM_GRADE.IG_LEGEND then
		return 8
	end

	return 0
end

function GET_ATTRIB_FACTOR_BY_ENCHANT_COUNT(slot0)
	if slot0 == 0 then
		return 1
	elseif slot0 == 1 then
		return 3
	elseif slot0 == 2 then
		return 6
	elseif slot0 == 3 then
		return 12
	elseif slot0 == 4 then
		return 24
	end

	return 0
end

function GET_EQUIP_FACTOR(slot0)
	if slot0 == true then
		return 1
	else
		return 0.25
	end
end
