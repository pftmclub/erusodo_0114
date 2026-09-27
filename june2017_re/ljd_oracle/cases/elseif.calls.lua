local function obj(M, make_nil, rand)
	local x = M.obj("x", { Go = M.NIL })
	return M.obj("obj", { Make = make_nil and M.NIL or x, Rand = rand or 0,
		Vec = M.obj("vec", {}), Use = M.NIL })
end

return {
	{ id = "method/a1", fn = "elseif_method", args = function(M) return 1, obj(M, false) end },
	{ id = "method/a2", fn = "elseif_method", args = function(M) return 2, obj(M, false) end },
	{ id = "method/a2-nil", fn = "elseif_method", args = function(M) return 2, obj(M, true) end },
	{ id = "deep/a1", fn = "elseif_deep", args = function(M) return 1, true, obj(M, false) end },
	{ id = "deep/a3-b", fn = "elseif_deep", args = function(M) return 3, true, obj(M, false) end },
	{ id = "deep/a3-nob", fn = "elseif_deep", args = function(M) return 3, false, obj(M, false) end },
	{ id = "number/1", fn = "elseif_number", args = function(M) return obj(M, false, 5) end },
	{ id = "number/2", fn = "elseif_number", args = function(M) return obj(M, false, 6) end },
	{ id = "number/3", fn = "elseif_number", args = function(M) return obj(M, false, 7) end },
	{ id = "number/0", fn = "elseif_number", args = function(M) return obj(M, false, 8) end },
	{ id = "reused/a1", fn = "elseif_reused_reg", args = function(M) return 1, obj(M, false) end },
	{ id = "reused/a3", fn = "elseif_reused_reg", args = function(M) return 3, obj(M, false) end },
}
