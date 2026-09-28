-- Call scenarios for gaps.lua.

local function o(M)
	return M.obj("o", { Get = 7, Use = M.NIL })
end

return {
	{ id = "slotkey", fn = "gap_slot_key", args = function(M) return o(M) end },
	{ id = "knil", fn = "gap_knil", args = function(M) return o(M) end },
	{ id = "closure", fn = "gap_closure", args = function(M) return o(M) end },
}
