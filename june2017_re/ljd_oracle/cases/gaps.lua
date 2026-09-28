-- Phase 4 second review: three more writes slotworks deletes while the value is
-- still needed.

-- A constructor element whose key is a slot that a later inline in the same batch
-- turns into an integer, which then won't fold.
function gap_slot_key(o)
	local t = { 1 }
	local k = 1
	t[k] = o:Get()
	o:Use(t)
end

-- KNIL: `local a, b` is one `slotA, slotB = nil`; eliminating it for a deletes b's
-- write too.
function gap_knil(o)
	local a, b
	o:Use(a)
	o:Use(b, b)
end

-- A closure capturing a temporary that has one other read.
function gap_closure(o)
	local t = o:Get()
	o:Use(t)
	return function() return t end
end
