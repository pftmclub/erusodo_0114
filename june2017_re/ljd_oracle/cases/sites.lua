-- Phase 4 code review: three more ways slotworks can delete a write whose value is
-- still read, besides the simple-inline sites Phase 3 found.

-- Mass assignment: two results of one call, the first copied into a local and read
-- again after a branch. _remove_invalid_references drops that later read (id == -1)
-- and _eliminate_mass_assignments renames the call's destination to the copy.
function massive_reuse(o, c)
	local a, b = o:Two()
	local x = a
	if c then
		o:Use(b)
	end
	return a, x
end

-- Table constructor: an element set the constructor can't absorb (index 1 already
-- holds a value), with the table itself inlined into its one other use.
function ctor_refold(o)
	local t = { 1 }
	t[1] = o:Get()
	o:Use(t)
end

-- Two reads under one holder (`t[t]`): the policy counts one of them and the inline
-- replaces only that one.
function dup_holder(o)
	local t = o:Get()
	return t[t]
end
