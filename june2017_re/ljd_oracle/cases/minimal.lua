-- The smallest reproducing shape found (Phase 1): a value tested by an if
-- condition straight after it is assigned, and read again after the if. The
-- if body can be empty and the value can be a plain field read. ljd folds the
-- right-hand side into the condition and the later read becomes a bare slotN.

function minimal(o)
	local x = o:Get()
	if x then
	end
	return x
end

function minimal_field(o)
	local x = o.v
	if x then
		o:A()
	end
	return x
end

-- Paired control: the same, except the condition tests something else. ljd
-- keeps this assignment, which is what isolates "first read is an if
-- condition" as the trigger.
function minimal_ctl(o, y)
	local x = o:Get()
	if y then
		o:A()
	end
	return x
end
