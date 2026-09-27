-- Kenta shapes: plain-Lua reconstructions of the dropped-assignment sites in
-- RAGE_DRYADKENTA.lua (see RANOX_LUA_DECOMPILE_BUG.md and the fixed
-- march2014/update/data037/RAGE_DRYADKENTA.src.lua). Engine calls are
-- renamed short; the control flow and the order of the locals follow the
-- bytecode as described in the fix comments.

-- COMMON_FRAMEMOVE: a ratio stored once, compared in an `and` inside the if
-- and again inside the elseif.
function hp_ratio_elseif(npc)
	if npc:GetState() == 1 or npc:GetState() == 2 then
		local r = npc:GetNowHP() / npc:GetMaxHP()
		if npc:GetFlag(1) == false and r <= 0.6 then
			npc:Do("dash60")
			npc:SetFlag(1, true)
		elseif npc:GetFlag(2) == false and r <= 0.2 then
			npc:Do("dash20")
			npc:SetFlag(2, true)
		end
	end
end

-- ATTACK_A_ATTACK_DASH_STATE_START: one value tested by an `or` chain.
function or_chain(npc)
	local idx = npc:GetLastTouchLineIndex()
	if idx == 0 or idx == 1 or idx == 4 or idx == 5 then
		npc:StateChange("WAIT")
	end
end

-- ATTACK_A_ATTACK_DASH_STATE_START, second half: a value passed as a call
-- argument inside the condition, then used again in the then-branch.
function cond_call_arg(npc)
	local sp = npc:GetLineGroupStartPos()
	if npc:IsRightTarget(sp) == npc:GetIsRight() then
		npc:SetVector(2, sp)
	else
		npc:SetVector(2, npc:GetLineGroupEndPos())
	end
end

-- ATTACK_A_ATTACK_DASH_STATE_START, middle: a nil test with a fallback
-- assignment, then a second nil test. The June decompile prints
-- `if slot2:GetTargetUser() == nil then slot4 = ... end; if slot4 ~= nil`.
function fallback_user(npc)
	local u = npc:GetTargetUser()
	if u == nil then
		u = npc:GetStoredUser()
	end
	if u ~= nil then
		npc:SetIsRight(not npc:IsRightTarget(u:GetPos()))
	end
end

-- SPECIAL_ATTACK_FIRE_FRAME_MOVE: two locals, both tested by one `and`, both
-- used in the body.
function and_two_locals(game, npc)
	if npc:AnimEventTimer(1.2) then
		local lm = game:GetLineMap()
		local es = game:GetEffectSet()
		if lm ~= nil and es ~= nil then
			es:Play("fire", npc, lm:GetStartPosition(8))
		end
	end
end

-- ATTACK_B_WAIT_FRAME_MOVE: the softlock. `es`'s register was written by an
-- unrelated branch earlier (v / u), so a dropped `es = ...` reads a vector or
-- a unit, not nil. Also carries the RandVal elseif (r == 1 / r == 2).
function effectset_stale(game, npc)
	if npc:GetFlag(0) == false then
		return
	end
	if npc:GetInt(0) - 2 < npc:GetElapsed(0) then
		local pos = npc:GetVector(4)
		if game:GetLineMap() ~= nil then
			local v = npc:GetVector(0)
			if npc:GetInt(1) == 2 then
				v = npc:GetVector(1)
			end
			local r = npc:GetRandVal() % 3
			local c = v.x
			if r == 1 then
				c = v.y
			elseif r == 2 then
				c = v.z
			end
			pos = game:GetLineMap():GetStartPosition(c)
		else
			local u = game:GetNearestUser(npc:GetPos())
			if u ~= nil then
				pos = u:GetPos()
				pos.y = u:GetLandPosition().y
			end
		end
		local es = game:GetEffectSet()
		if es ~= nil then
			es:Play("mark", npc, pos)
		end
		npc:SetVector(4, pos)
		npc:SetFlag(0, false)
	end
end
