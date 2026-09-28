-- Call scenarios for sites.lua.

local function o(M, get)
	return M.obj("o", { Two = M.f(function() return 1, 2 end), Use = M.NIL, Get = get })
end

return {
	{ id = "massive/c", fn = "massive_reuse", args = function(M) return o(M), true end },
	{ id = "massive/noc", fn = "massive_reuse", args = function(M) return o(M), false end },
	{ id = "ctor/refold", fn = "ctor_refold", args = function(M) return o(M, 7) end },
	{ id = "dup/str", fn = "dup_holder", args = function(M) return o(M, "len") end },
}
