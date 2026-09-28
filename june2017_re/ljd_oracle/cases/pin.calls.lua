-- Call scenarios for pin.lua.

local function vec(M, tag, x, y, z)
	return M.obj(tag, { x = M.field(x), y = M.field(y), z = M.field(z) })
end

local function bw3(M, ready, near)
	local npc = M.obj("npc", { GetVector = M.by({ [4] = vec(M, "v4", 0, -5000, 0) }), SetVector = M.NIL })
	local user = M.obj("user", {
		GetPos = vec(M, "userpos", 10, 11, 12),
		GetLandPosition = vec(M, "land", 0, 99, 0),
	})
	local game = M.obj("game", { Ready = ready, GetNearestUser = near and user or M.NIL })
	return game, npc
end

local function fi(M, t12, t135, has_lm, has_es)
	local npc = M.obj("npc", { AnimEventTimer = M.by({ [1.2] = t12, [1.35] = t135 }), GetRotateDegree = 45 })
	local lm = M.obj("linemap", { GetStartPosition = M.f(function(self, i)
		return vec(M, "start" .. tostring(i), i, 0, 0) end) })
	local es = M.obj("effectset", { Play = M.NIL })
	local game = M.obj("game", {
		GetLineMap = has_lm and lm or M.NIL,
		GetEffectSet = has_es and es or M.NIL,
	})
	return game, npc
end

return {
	{ id = "bw3/near", fn = "bw3_field_target", args = function(M) return bw3(M, true, true) end },
	{ id = "bw3/nonear", fn = "bw3_field_target", args = function(M) return bw3(M, true, false) end },
	{ id = "bw3/notready", fn = "bw3_field_target", args = function(M) return bw3(M, false, true) end },

	{ id = "fi/both", fn = "fi_two_timers", args = function(M) return fi(M, true, true, true, true) end },
	{ id = "fi/first", fn = "fi_two_timers", args = function(M) return fi(M, true, false, true, true) end },
	{ id = "fi/second", fn = "fi_two_timers", args = function(M) return fi(M, false, true, true, true) end },
	{ id = "fi/nolm", fn = "fi_two_timers", args = function(M) return fi(M, true, true, false, true) end },
	{ id = "fi/noes", fn = "fi_two_timers", args = function(M) return fi(M, true, true, true, false) end },
}
