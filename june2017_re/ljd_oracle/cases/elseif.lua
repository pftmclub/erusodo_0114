-- The Kenta shape inside elseif chains: a local whose first read is an
-- elseif condition (not the leading if), then read again in that body.

-- x's first read is the elseif's own condition.
function elseif_method(a, obj)
	local x = obj:Make()
	if a == 1 then
		return "one"
	elseif x ~= nil then
		x:Go()
	end
	return "end"
end

-- The same, one level deeper, with an `and`.
function elseif_deep(a, b, obj)
	local x = obj:Make()
	if a == 1 then
		return "one"
	elseif a == 2 then
		return "two"
	elseif b and x ~= nil then
		x:Go(b)
		return "go"
	end
	return "end"
end

-- A number tested by successive elseifs (the RandVal shape on its own).
function elseif_number(obj)
	local r = obj:Rand() % 4
	local c = "zero"
	if r == 1 then
		c = "one"
	elseif r == 2 then
		c = "two"
	elseif r == 3 then
		c = "three"
	end
	return c
end

-- Register reuse: x's register first holds an unrelated value in the leading
-- branch, then the elseif tests a fresh call result.
function elseif_reused_reg(a, obj)
	if a == 1 then
		local v = obj:Vec()
		obj:Use(v)
	end
	local x = obj:Make()
	if a == 2 then
		return "two"
	elseif x ~= nil then
		x:Go()
	end
	return "end"
end
