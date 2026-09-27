-- Lead 1 (_fill_simple_refs skipping id == -1 references): a value read by an
-- if condition and read again after the if/else merges.

function merge_number(obj)
	local x = obj:Get()
	if x > 0 then
		obj:A()
	else
		obj:B()
	end
	return x
end

function merge_nil_check(obj)
	local x = obj:Get()
	if x ~= nil then
		obj:A(1)
	end
	obj:Use(x)
end

function merge_and_cond(obj)
	local x = obj:Get()
	if obj:Flag() and x ~= nil then
		obj:A()
	end
	obj:Use(x)
end

-- Read in the condition, then in both arms, then after the merge.
function merge_both_arms(obj)
	local x = obj:Get()
	if x > 10 then
		obj:A(x)
	else
		obj:B(x)
	end
	return x * 2
end
