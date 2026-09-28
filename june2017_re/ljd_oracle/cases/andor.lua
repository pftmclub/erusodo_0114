-- Phase 4 second review: `c and <value> or d`, where the value is a closure, a
-- call or a constructor. Stock ljd decompiles these correctly; a fix that keeps
-- the and/or's result register breaks the unwarper's expression rebuild.

function ao_closure(o, c)
	local f = c and function() return 1 end or o
	return f
end

function ao_call(o, c, d)
	return (c and d) and o:Get() or (d and o)
end

function ao_ctor(o, c, d)
	return (c and d) and {} or (d and o)
end
