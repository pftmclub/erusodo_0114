local function obj(M, v, flag)
	return M.obj("obj", { Get = v, A = M.NIL, B = M.NIL, Use = M.NIL, Flag = flag })
end

return {
	{ id = "number/pos", fn = "merge_number", args = function(M) return obj(M, 5) end },
	{ id = "number/neg", fn = "merge_number", args = function(M) return obj(M, -5) end },
	{ id = "nil/val", fn = "merge_nil_check", args = function(M) return obj(M, 7) end },
	{ id = "nil/nil", fn = "merge_nil_check", args = function(M) return obj(M, M.NIL) end },
	{ id = "and/t", fn = "merge_and_cond", args = function(M) return obj(M, 7, true) end },
	{ id = "and/f", fn = "merge_and_cond", args = function(M) return obj(M, 7, false) end },
	{ id = "arms/big", fn = "merge_both_arms", args = function(M) return obj(M, 20) end },
	{ id = "arms/small", fn = "merge_both_arms", args = function(M) return obj(M, 3) end },
}
