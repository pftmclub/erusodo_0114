-- ljd_oracle driver (Phase 1 of RANOX_LUA_DECOMPILE_FIX_PLAN.md).
--
--     luajit.exe driver.lua <chunk> <calls.lua>
--
-- Loads <chunk> (0x81 bytecode: the original, or the ljd round trip after
-- compile-batch -v mar-2014) into its own environment, runs its main chunk,
-- then makes every call listed by <calls.lua> (plain source, never seen by
-- ljd) and prints one event per line to stdout. run_oracle.py diffs the two
-- sides' output. Lines starting with '#' are informational and not diffed.
--
-- Unknown-global detection. The environment has NO poisoned keys written into
-- it: a name the main chunk did not write is simply absent, and the env's
-- __index logs the read. A sentinel value stored in the env could not see
-- `slot6 == 1` or `slot5 ~= nil` (Lua 5.1 runs no __eq for table-vs-number or
-- table-vs-nil), so it would miss half of the known Kenta instances. Classes:
--     POISON_READ slotN    a slotN global never written in this call
--     UPVALUE_READ uvN     ljd's name for a captured upvalue, now a global
--     GLOBAL_READ name     any other unknown global (a named local dropped in
--                          a -g build reads as its own name, not slotN)
-- The read then returns a sentinel that logs its uses (POISON_USE) and keeps
-- going, so later dropped reads in the same call are logged too. An order
-- comparison against a number raises before any metamethod can run; that
-- ends the call and is logged as "POISON_READ <name> (fatal)".
--
-- Before every call the environment is restored to its state right after the
-- main chunk ran, so names the main chunk wrote (the round trip's file-scope
-- slotN) are exempt and every other slotN starts unwritten. Writes are
-- tracked explicitly (see env below), so `slot2 = nil` counts as a write.

jit.off()
jit.flush()

local chunk_path, calls_path = arg[1], arg[2]
local BUDGET = 1000000

-- emit() is capped per call so a runaway round trip (deepcopy recursing on a
-- sentinel) cannot flood the trace; mark() is for structure and outcomes,
-- which always print.
local EVENT_CAP = 2000
local out, events = {}, 0
local function mark(s) out[#out + 1] = s end
local function emit(s)
	events = events + 1
	if events == EVENT_CAP + 1 then out[#out + 1] = 'EVENTS_TRUNCATED' end
	if events > EVENT_CAP then return end
	out[#out + 1] = s
end

local MOCKS = setmetatable({}, { __mode = 'k' })   -- mock -> tag
local SENT = setmetatable({}, { __mode = 'k' })    -- sentinel -> name
local WRAP = setmetatable({}, { __mode = 'k' })    -- script wrapper -> kind

-- Formatting: never print an address; plain tables print with sorted keys so
-- two sides that build the same table in a different order still agree.
local fmt
local function fmtnum(n)
	if n ~= n then return 'nan' end
	if n == math.huge then return 'inf' end
	if n == -math.huge then return '-inf' end
	if n == math.floor(n) and math.abs(n) < 1e15 then return string.format('%d', n) end
	return string.format('%.14g', n)
end
local function keyless(a, b)
	local ta, tb = type(a), type(b)
	if ta ~= tb then return ta < tb end
	if ta == 'number' or ta == 'string' then return a < b end
	return fmt(a) < fmt(b)
end
fmt = function(v, depth)
	local t = type(v)
	if t == 'number' then return fmtnum(v)
	elseif t == 'string' then return string.format('%q', v)
	elseif t == 'nil' or t == 'boolean' then return tostring(v)
	elseif t == 'table' then
		if MOCKS[v] then return '<' .. MOCKS[v] .. '>' end
		if SENT[v] then return '<poison ' .. SENT[v] .. '>' end
		depth = depth or 0
		if depth > 8 then return '{...}' end
		local keys = {}
		for k in pairs(v) do keys[#keys + 1] = k end
		table.sort(keys, keyless)
		local parts = {}
		for _, k in ipairs(keys) do
			parts[#parts + 1] = '[' .. fmt(k, depth + 1) .. ']=' .. fmt(v[k], depth + 1)
		end
		return '{' .. table.concat(parts, ',') .. '}'
	end
	return '<' .. t .. '>'
end
local function fmtlist(n, ...)
	local parts = {}
	for i = 1, n do parts[i] = fmt((select(i, ...))) end
	return table.concat(parts, ', ')
end

-- Error text: drop the "chunk:line: " prefix and the variable annotation,
-- which differ between a -g original ("local 'es'"), a stripped one (none)
-- and the round trip ("global 'slot5'") for the same failure.
local function normerr(e)
	if type(e) ~= 'string' then return 'non-string error ' .. fmt(e) end
	e = e:gsub('^.-:%d+: ', '')
	e = e:gsub("%a+ '[^']*' %((an? %a+ value)%)", '%1')
	return e
end

-- Sentinel: what an unknown global reads as. Logs every use and continues.
local last_read
local SMT = {}
local function sentinel(name)
	local s = setmetatable({}, SMT)
	SENT[s] = name
	return s
end
local function suse(s, what) emit('POISON_USE ' .. SENT[s] .. ' ' .. what) end
SMT.__index = function(s, k) suse(s, 'index ' .. fmt(k)); return s end
SMT.__newindex = function(s, k) suse(s, 'newindex ' .. fmt(k)) end
SMT.__call = function(s) suse(s, 'call'); return s end
SMT.__len = function(s) suse(s, 'len'); return 0 end
SMT.__tostring = function(s) return '<poison ' .. SENT[s] .. '>' end
for _, mm in ipairs({ 'add', 'sub', 'mul', 'div', 'mod', 'pow', 'unm', 'concat', 'lt', 'le', 'eq' }) do
	SMT['__' .. mm] = function(a, b)
		local s = SENT[a] and a or b
		suse(s, mm)
		if mm == 'lt' or mm == 'le' or mm == 'eq' then return false end
		return s
	end
end

-- Mock objects. Strict: touching a member the script does not define raises a
-- deterministic error, so a method called on the wrong (stale) object shows
-- up as a divergence instead of being served silently.
local M = {}
M.NIL = setmetatable({}, { __tostring = function() return 'NIL' end })
function M.seq(...) local w = { n = select('#', ...), ... }; WRAP[w] = 'seq'; return w end
function M.by(t) WRAP[t] = 'by'; return t end
function M.f(fn) local w = { fn }; WRAP[w] = 'f'; return w end
function M.field(v) local w = { v }; WRAP[w] = 'field'; return w end

local function unnil(v) if v == M.NIL then return nil end return v end

function M.obj(tag, script)
	local o = {}
	local store, counts = {}, {}
	MOCKS[o] = tag
	for k, v in pairs(script) do
		if WRAP[v] == 'field' then store[k] = v[1] end
	end
	setmetatable(o, {
		__index = function(self, k)
			if store[k] ~= nil then
				emit('GET ' .. tag .. '.' .. tostring(k) .. ' -> ' .. fmt(unnil(store[k])))
				return unnil(store[k])
			end
			local v = script[k]
			if v == nil then error('mock ' .. tag .. ' has no member ' .. tostring(k), 0) end
			return function(...)
				local n = select('#', ...)
				local args
				local first = ...
				if first == self then
					args = tag .. ':' .. tostring(k) .. '(' .. fmtlist(n - 1, select(2, ...)) .. ')'
				else
					args = tag .. '.' .. tostring(k) .. '(' .. fmtlist(n, ...) .. ')'
				end
				counts[k] = (counts[k] or 0) + 1
				local kind, r = WRAP[v]
				if kind == 'seq' then
					r = v[math.min(counts[k], v.n)]
				elseif kind == 'by' then
					local key = (first == self) and select(2, ...) or first
					r = v[key]
					if r == nil then error('mock ' .. tag .. ':' .. tostring(k) .. ' has no case for ' .. fmt(key), 0) end
				elseif kind == 'f' then
					r = v[1](...)
				else
					r = v
				end
				r = unnil(r)
				emit('CALL ' .. args .. ' -> ' .. fmt(r))
				return r
			end
		end,
		__newindex = function(self, k, v)
			emit('SET ' .. tag .. '.' .. tostring(k) .. ' = ' .. fmt(v))
			store[k] = (v == nil) and M.NIL or v
		end,
	})
	return o
end

-- The chunk's environment.
local BASE = {
	pairs = pairs, ipairs = ipairs, next = next, type = type, select = select,
	tostring = tostring, tonumber = tonumber, unpack = unpack, error = error,
	pcall = pcall, assert = assert, setmetatable = setmetatable,
	getmetatable = getmetatable, rawget = rawget, rawset = rawset,
	math = math, string = string, table = table,
	LOG = function(...) emit('LOG ' .. fmtlist(select('#', ...), ...)) end,
}
-- env stays empty: every read and write goes through its metatable, so a
-- global the chunk assigned nil still counts as written (a plain table would
-- drop the key and the next read would look like a poisoned one).
local env, store, written = {}, {}, {}
local reads_this_call = 0
setmetatable(env, {
	__newindex = function(t, k, v)
		written[k] = true
		store[k] = v
	end,
	__index = function(t, k)
		if written[k] then return store[k] end
		local b = BASE[k]
		if b ~= nil then return b end
		local cls
		if type(k) == 'string' and k:match('^slot%d+$') then cls = 'POISON_READ'
		elseif type(k) == 'string' and k:match('^uv%d+$') then cls = 'UPVALUE_READ'
		else cls = 'GLOBAL_READ' end
		emit(cls .. ' ' .. tostring(k))
		last_read = cls .. ' ' .. tostring(k)
		reads_this_call = reads_this_call + 1
		return sentinel(tostring(k))
	end,
})

local function budget_hook() error('__ORACLE_BUDGET__', 0) end

-- Run fn(...) under the budget hook; returns ok, n, results... or false, err.
local function guarded(fn, ...)
	local args = { n = select('#', ...), ... }
	debug.sethook(budget_hook, '', BUDGET)
	local res = (function(...) return { n = select('#', ...), ... } end)(
		pcall(function() return fn(unpack(args, 1, args.n)) end))
	debug.sethook()
	return res
end

local function report(res)
	if res[1] then
		mark('RET ' .. fmtlist(res.n - 1, unpack(res, 2, res.n)))
		return
	end
	local e = res[2]
	if e == '__ORACLE_BUDGET__' then mark('BUDGET'); return end
	local ne = normerr(e)
	mark('ERROR ' .. ne)
	if last_read and ne:match('^attempt to compare') and ne:match('table') then
		mark(last_read .. ' (fatal)')
	end
end

-- Main chunk.
local chunk, lerr = loadfile(chunk_path)
if not chunk then
	io.write('LOAD_ERROR ' .. normerr(lerr) .. '\n')
	os.exit(2)
end
setfenv(chunk, env)
mark('MAIN_BEGIN')
last_read = nil
events = 0
report(guarded(chunk))
mark('MAIN_END')

local main_store, main_written, names = {}, {}, {}
for k in pairs(written) do main_written[k] = true; main_store[k] = store[k]; names[#names + 1] = tostring(k) end
table.sort(names)
mark('# MAIN_WRITES ' .. table.concat(names, ' '))

-- Back to the post-main-chunk state: main-chunk names keep their values and
-- stay exempt; every other name is unwritten again.
local function restore()
	store, written = {}, {}
	for k in pairs(main_written) do written[k] = true; store[k] = main_store[k] end
end

-- Calls.
local spec = dofile(calls_path)
for i, c in ipairs(spec) do
	restore()
	last_read = nil
	reads_this_call = 0
	events = 0
	local id = c.id or (c.fn .. '#' .. i)
	mark('CALL_BEGIN ' .. id .. ' ' .. c.fn)
	local fn = written[c.fn] and store[c.fn]
	if type(fn) ~= 'function' then
		mark('ERROR no function ' .. c.fn)
	else
		local args = (function(...) return { n = select('#', ...), ... } end)(c.args(M))
		report(guarded(fn, unpack(args, 1, args.n)))
	end
	mark('CALL_END ' .. id)
end

io.write(table.concat(out, '\n'), '\n')
