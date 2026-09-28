-- Call scenarios for valuectl.lua.

local function o(M, get)
	return M.obj("o", { Get = get, Default = 7, Use = M.NIL, A = M.NIL, B = M.NIL, C = M.NIL })
end

return {
	{ id = "and/tt", fn = "vctl_and_value", args = function(M) return 1, 2 end },
	{ id = "and/ft", fn = "vctl_and_value", args = function(M) return false, 2 end },
	{ id = "ordef/val", fn = "vctl_or_default", args = function(M) return o(M, 5) end },
	{ id = "ordef/nil", fn = "vctl_or_default", args = function(M) return o(M, M.NIL) end },
	{ id = "ortemp/a", fn = "vctl_or_after_temp", args = function(M) return o(M, 5), 3 end },
	{ id = "ortemp/noa", fn = "vctl_or_after_temp", args = function(M) return o(M, 5), false end },
	{ id = "andor/tt", fn = "vctl_and_or_arg", args = function(M) return o(M, 5), 1, 2 end },
	{ id = "andor/f", fn = "vctl_and_or_arg", args = function(M) return o(M, 5), false, 2 end },
}
