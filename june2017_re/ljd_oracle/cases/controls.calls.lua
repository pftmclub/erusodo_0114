local function obj(M, v)
	return M.obj("obj", {
		F = M.f(function(self, a) return a * 10 end),
		G = M.f(function(self, a) return a + 1 end),
		Make = M.obj("fresh", { Go = M.NIL }),
		Get = v, A = M.NIL, B = M.NIL,
	})
end

return {
	{ id = "nested", fn = "ctl_nested_call", args = function(M) return obj(M), 4 end },
	{ id = "fresh", fn = "ctl_method_on_fresh", args = function(M) return obj(M) end },
	{ id = "single-local", fn = "ctl_single_use_local", args = function(M) return obj(M, 41) end },
	{ id = "single-cond/val", fn = "ctl_single_use_cond", args = function(M) return obj(M, 1) end },
	{ id = "single-cond/nil", fn = "ctl_single_use_cond", args = function(M) return obj(M, M.NIL) end },
	{ id = "arith", fn = "ctl_arith", args = function(M) return 7, 3 end },
	{ id = "two-straight", fn = "ctl_two_uses_straight", args = function(M) return obj(M, 9) end },
}
