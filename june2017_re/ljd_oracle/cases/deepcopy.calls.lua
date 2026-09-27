return {
	{ id = "flat", fn = "copy_it", args = function(M) return { a = 1, b = "x" } end },
	{ id = "nested", fn = "copy_it", args = function(M) return { a = 1, b = { c = 2, d = { 3, 4 } } } end },
	{ id = "global/flat", fn = "deepcopy_global", args = function(M) return { a = 1, b = "x" } end },
	{ id = "global/nested", fn = "deepcopy_global", args = function(M) return { a = 1, b = { c = 2 } } end },
}
