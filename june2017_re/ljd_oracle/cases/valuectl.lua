-- Controls for value-producing and/or (Phase 4). The bytecode tests a
-- register and then reads it again after the branches merge - the shape a
-- liveness-based fix is likeliest to over-fire on. Every one of these is a
-- genuine single-use temporary: ljd must keep inlining them, and
-- expected.tsv pins how many assignments it prints.

function vctl_and_value(a, b)
	return a and b
end

function vctl_or_default(o)
	local x = o:Get() or o:Default()
	o:Use(x)
end

-- A single-use temporary in a register, then an `or` whose left operand lives
-- in another register and whose result lands in that first register
-- (ISTC/ISFC: the copy happens only on the jump path).
function vctl_or_after_temp(o, a)
	o:A(o:Get())
	local x = a or o:Default()
	o:B(x)
end

function vctl_and_or_arg(o, a, b)
	o:C(a and b or o:Default())
end
