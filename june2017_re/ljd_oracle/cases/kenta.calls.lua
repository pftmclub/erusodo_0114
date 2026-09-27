-- Call scenarios for kenta.lua. Loaded as source by driver.lua; never
-- decompiled. Each entry: id, fn, args(M) -> the call's arguments.

local function npc_hp(M, state, hp, f1, f2)
	return M.obj("npc", {
		GetState = state, GetNowHP = hp, GetMaxHP = 100,
		GetFlag = M.by({ [1] = f1, [2] = f2 }),
		Do = M.NIL, SetFlag = M.NIL,
	})
end

local function vec(M, tag, x, y, z)
	return M.obj(tag, { x = M.field(x), y = M.field(y), z = M.field(z) })
end

local function b_wait(M, has_lm, int1, rand, near, has_es)
	local npc = M.obj("npc", {
		GetFlag = M.by({ [0] = true }),
		GetInt = M.by({ [0] = 3, [1] = int1 }),
		GetElapsed = 5,
		GetVector = M.by({ [0] = vec(M, "v0", 1, 2, 3), [1] = vec(M, "v1", 4, 5, 6),
		                   [4] = vec(M, "v4", 0, -5000, 0) }),
		GetRandVal = rand,
		GetPos = vec(M, "npcpos", 7, 8, 9),
		SetVector = M.NIL, SetFlag = M.NIL,
	})
	local lm = M.obj("linemap", { GetStartPosition = M.f(function(self, c)
		return vec(M, "start" .. tostring(c), c, 0, 0) end) })
	local user = M.obj("user", {
		GetPos = vec(M, "userpos", 10, 11, 12),
		GetLandPosition = vec(M, "land", 0, 99, 0),
	})
	local es = M.obj("effectset", { Play = M.NIL })
	local game = M.obj("game", {
		GetLineMap = has_lm and lm or M.NIL,
		GetNearestUser = near and user or M.NIL,
		GetEffectSet = has_es and es or M.NIL,
	})
	return game, npc
end

return {
	{ id = "hp/ratio0.5-flags-ff", fn = "hp_ratio_elseif", args = function(M) return npc_hp(M, 1, 50, false, false) end },
	{ id = "hp/ratio0.1-flag1-set", fn = "hp_ratio_elseif", args = function(M) return npc_hp(M, 1, 10, true, false) end },
	{ id = "hp/ratio0.9-flags-ff", fn = "hp_ratio_elseif", args = function(M) return npc_hp(M, 2, 90, false, false) end },
	{ id = "hp/state-miss", fn = "hp_ratio_elseif", args = function(M) return npc_hp(M, 3, 10, false, false) end },

	{ id = "or/idx0", fn = "or_chain", args = function(M) return M.obj("npc", { GetLastTouchLineIndex = 0, StateChange = M.NIL }) end },
	{ id = "or/idx4", fn = "or_chain", args = function(M) return M.obj("npc", { GetLastTouchLineIndex = 4, StateChange = M.NIL }) end },
	{ id = "or/idx2", fn = "or_chain", args = function(M) return M.obj("npc", { GetLastTouchLineIndex = 2, StateChange = M.NIL }) end },

	{ id = "arg/right", fn = "cond_call_arg", args = function(M)
		return M.obj("npc", { GetLineGroupStartPos = vec(M, "startpos", 1, 0, 0), IsRightTarget = true,
			GetIsRight = true, SetVector = M.NIL, GetLineGroupEndPos = vec(M, "endpos", 2, 0, 0) }) end },
	{ id = "arg/left", fn = "cond_call_arg", args = function(M)
		return M.obj("npc", { GetLineGroupStartPos = vec(M, "startpos", 1, 0, 0), IsRightTarget = false,
			GetIsRight = true, SetVector = M.NIL, GetLineGroupEndPos = vec(M, "endpos", 2, 0, 0) }) end },

	{ id = "fallback/target", fn = "fallback_user", args = function(M)
		return M.obj("npc", { GetTargetUser = M.obj("target", { GetPos = vec(M, "tpos", 1, 0, 0) }),
			GetStoredUser = M.NIL, SetIsRight = M.NIL, IsRightTarget = true }) end },
	{ id = "fallback/stored", fn = "fallback_user", args = function(M)
		return M.obj("npc", { GetTargetUser = M.NIL,
			GetStoredUser = M.obj("stored", { GetPos = vec(M, "spos", 2, 0, 0) }),
			SetIsRight = M.NIL, IsRightTarget = false }) end },

	{ id = "and2/both", fn = "and_two_locals", args = function(M)
		return M.obj("game", { GetLineMap = M.obj("linemap", { GetStartPosition = vec(M, "sp8", 8, 0, 0) }),
			GetEffectSet = M.obj("effectset", { Play = M.NIL }) }),
			M.obj("npc", { AnimEventTimer = true }) end },
	{ id = "and2/no-es", fn = "and_two_locals", args = function(M)
		return M.obj("game", { GetLineMap = M.obj("linemap", { GetStartPosition = vec(M, "sp8", 8, 0, 0) }),
			GetEffectSet = M.NIL }),
			M.obj("npc", { AnimEventTimer = true }) end },

	{ id = "bwait/lm-rand1", fn = "effectset_stale", args = function(M) return b_wait(M, true, 1, 1, false, true) end },
	{ id = "bwait/lm-int2-rand2", fn = "effectset_stale", args = function(M) return b_wait(M, true, 2, 2, false, true) end },
	{ id = "bwait/lm-rand0-noes", fn = "effectset_stale", args = function(M) return b_wait(M, true, 1, 3, false, false) end },
	{ id = "bwait/nolm-near", fn = "effectset_stale", args = function(M) return b_wait(M, false, 1, 0, true, true) end },
	{ id = "bwait/nolm-nonear", fn = "effectset_stale", args = function(M) return b_wait(M, false, 1, 0, false, true) end },
}
