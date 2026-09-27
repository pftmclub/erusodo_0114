-- List the bytecode of named file functions in a 0x81 chunk (the original
-- side, usually), so a fix-rerun restoration can cite the instructions it
-- restores. Run with cwd = elstools/vendor/luajit (jit.bc lives there).
--
--     luajit.exe bcfn.lua CHUNK FN [FN ...]
--
-- The main chunk runs against an auto-vivifying stub environment: every
-- unknown global is a callable, indexable stub, so enum reads and registry
-- calls succeed and the file's `function NAME` definitions land in it.
local bc = require('jit.bc')
local chunk = assert(loadfile(arg[1]))
local stub
local SMT = {}
SMT.__index = function() return stub end
SMT.__call = function() return stub end
stub = setmetatable({}, SMT)
local env = setmetatable({}, { __index = function() return stub end })
setfenv(chunk, env)
assert(pcall(chunk))
for i = 2, #arg do
	local f = rawget(env, arg[i])
	if type(f) ~= 'function' then
		io.write('-- ', arg[i], ': not a function defined by the main chunk\n')
	else
		io.write('-- ', arg[i], '\n')
		bc.dump(f, io.stdout, true)
		io.write('\n')
	end
end
