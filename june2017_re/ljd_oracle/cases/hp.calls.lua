local function npc(M, hp, f1, f2)
	return M.obj("npc", { GetNowHP = hp, GetMaxHP = 100, Do = M.NIL,
		GetFlag = M.by({ [1] = f1, [2] = f2 }) })
end

return {
	{ id = "ifs/0.9", fn = "hp_two_ifs", args = function(M) return npc(M, 90) end },
	{ id = "ifs/0.5", fn = "hp_two_ifs", args = function(M) return npc(M, 50) end },
	{ id = "ifs/0.1", fn = "hp_two_ifs", args = function(M) return npc(M, 10) end },
	{ id = "and/0.1-ff", fn = "hp_two_ifs_and", args = function(M) return npc(M, 10, false, false) end },
	{ id = "and/0.1-tf", fn = "hp_two_ifs_and", args = function(M) return npc(M, 10, true, false) end },
	{ id = "and/0.5-tt", fn = "hp_two_ifs_and", args = function(M) return npc(M, 50, true, true) end },
}
