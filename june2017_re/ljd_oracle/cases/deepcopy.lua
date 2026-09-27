-- The known SECOND ljd defect (june2017_re/README.md, "ljd caveat"): a
-- recursive file-local deepcopy, as in DamageEffect.lua, is mis-structured
-- (slot1[k] = uv0(v) hoisted into a bogus `return { ... }`). Labelled
-- control: it is not the dropped-assignment class and must never count for
-- or against a Phase 4 fix.

local function deepcopy(o)
	local r = {}
	for k, v in pairs(o) do
		if type(v) == "table" then
			r[k] = deepcopy(v)
		else
			r[k] = v
		end
	end
	return r
end

function copy_it(t)
	return deepcopy(t)
end

-- The same body recursing through a global instead of an upvalue. copy_it's
-- round trip dies on the uv0 global before it reaches the body; this one
-- runs the mis-structured body itself.
function deepcopy_global(o)
	local r = {}
	for k, v in pairs(o) do
		if type(v) == "table" then
			r[k] = deepcopy_global(v)
		else
			r[k] = v
		end
	end
	return r
end
