local function o(M, v)
	return M.obj("o", { Get = v, v = M.field(v), A = M.NIL })
end

return {
	{ id = "minimal/val", fn = "minimal", args = function(M) return o(M, 3) end },
	{ id = "minimal/false", fn = "minimal", args = function(M) return o(M, false) end },
	{ id = "field/val", fn = "minimal_field", args = function(M) return o(M, 3) end },
	{ id = "ctl/y", fn = "minimal_ctl", args = function(M) return o(M, 3), true end },
	{ id = "ctl/noy", fn = "minimal_ctl", args = function(M) return o(M, 3), false end },
}
