-- Phase 4 pins ("Phase 3 result", "Pin before fixing"): two Kenta shapes the
-- kenta case only reaches inside a larger function, isolated.

-- BW-3 (ATTACK_B_WAIT_FRAME_MOVE): a value assigned inside a nested if, used
-- as the target of a field set, and read again after both ifs close. ljd
-- folds the call into the field-set target, so the later reads see the
-- register's older value. Phase 3 site (d): the collection that drops it is
-- rooted at the inner block and never visits the reads outside it.
function bw3_field_target(game, npc)
	local pos = npc:GetVector(4)
	if game:Ready() then
		local u = game:GetNearestUser()
		if u ~= nil then
			pos = u:GetPos()
			pos.y = u:GetLandPosition().y
		end
	end
	npc:SetVector(4, pos)
end

-- FI-1/FI-2 (SPECIAL_ATTACK_FIRE_FRAME_MOVE): a value computed, then a second
-- value assigned before the test that reads both, once per timer block; the
-- second block reuses the first block's registers.
function fi_two_timers(game, npc)
	if npc:AnimEventTimer(1.2) then
		local lm = game:GetLineMap()
		local es = game:GetEffectSet()
		if lm ~= nil and es ~= nil then
			es:Play("fire1", npc, lm:GetStartPosition(8), npc:GetRotateDegree())
		end
	end
	if npc:AnimEventTimer(1.35) then
		local lm = game:GetLineMap()
		local es = game:GetEffectSet()
		if lm ~= nil and es ~= nil then
			es:Play("fire2", npc, lm:GetStartPosition(9), npc:GetRotateDegree())
		end
	end
end
