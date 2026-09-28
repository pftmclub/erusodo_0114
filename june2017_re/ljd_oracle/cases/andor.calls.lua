-- Call scenarios for andor.lua.

local function o(M)
	return M.obj("o", { Get = 5 })
end

return {
	{ id = "closure/c", fn = "ao_closure", args = function(M) return o(M), true end },
	{ id = "closure/noc", fn = "ao_closure", args = function(M) return o(M), false end },
	{ id = "call/tt", fn = "ao_call", args = function(M) return o(M), true, true end },
	{ id = "call/ft", fn = "ao_call", args = function(M) return o(M), false, true end },
	{ id = "call/ff", fn = "ao_call", args = function(M) return o(M), false, false end },
	{ id = "ctor/tt", fn = "ao_ctor", args = function(M) return o(M), true, true end },
	{ id = "ctor/ft", fn = "ao_ctor", args = function(M) return o(M), false, true end },
}
