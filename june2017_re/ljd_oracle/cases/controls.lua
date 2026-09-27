-- Controls: genuine single-use temporaries ljd SHOULD inline, and one
-- straight-line multi-use local it must NOT inline. Behaviour must match;
-- expected.tsv also pins how many assignments ljd prints for each, so a
-- Phase 4 fix that stops inlining everything shows up here.

function ctl_nested_call(obj, x)
	return obj:F(obj:G(x))
end

function ctl_method_on_fresh(obj)
	obj:Make():Go(1)
end

function ctl_single_use_local(obj)
	local t = obj:Get()
	return t + 1
end

function ctl_single_use_cond(obj)
	if obj:Get() ~= nil then
		obj:A()
	end
end

function ctl_arith(a, b)
	return (a + b) * (a - b)
end

-- Two reads, no branch between them: the assignment must survive.
function ctl_two_uses_straight(obj)
	local t = obj:Get()
	obj:A(t)
	obj:B(t)
end
