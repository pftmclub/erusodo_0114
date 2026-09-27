-- ljdiff runtime (Phase 2a of RANOX_LUA_DECOMPILE_FIX_PLAN.md).
--
--     luajit.exe ljdiff.lua MODE CHUNK OUT TYPES REENTRANT SEEDS LABEL SIDE
--
--   MODE       learn | trace
--   CHUNK      0x81 bytecode (ljconv mar-2014 original, or a compile-batch
--              -v mar-2014 round trip)
--   OUT        learn: TSV of newly learned types; trace: the JSON trace
--   TYPES      types.tsv (read-only here; ljdiff.py merges what learn found)
--   REENTRANT  reentrant.tsv (engine methods that call back into Lua)
--   SEEDS      calls per file-defined function, seeds 1..SEEDS
--   LABEL/SIDE written into the trace header only
--
-- Run with cwd = elstools/vendor/luajit (jit.vmdef is loaded from there).
-- ljdiff.py drives it; this file never decides anything about a diff.
--
-- What one run does: load CHUNK under setfenv into a recording environment,
-- run the main chunk (every global write recorded), then call every
-- file-defined function once per seed with mock arguments. Before every call
-- the environment goes back to its post-main-chunk state.
--
-- Unknown names (reads the main chunk never wrote), as in ljd_oracle/driver.lua:
--   POISON_READ slotN    never written in this call  -> log-and-continue sentinel
--   UPVALUE_READ uvN     ljd's name for a captured upvalue, now a global
--   anything else        an engine global: served by the mock, "GREAD name"
-- Names the main chunk wrote are exempt. Writes are tracked explicitly, so
-- `slot2 = nil` still counts as written.
--
-- Mock values ("nodes"). Every engine value is a node with a path (a2, a2:GetPos()#1,
-- g_pX2Game.GetLineMap, ...). Indexing a node gives a member node; calling a member
-- node records "CALL path:Method(args) -> ret". What a call returns (and what a
-- field read or an argument is) comes from the type oracle:
--   number / string    a deterministic value from hash(seed, path, per-member count)
--   object (default)   a new node, or nil with probability 1/6 (returns only)
-- The oracle is learned ONLY on the original side (MODE=learn), then frozen:
--   arithmetic/unm on a node  -> exact (the node's own metamethod runs): number
--   concat                    -> exact: string
--   node < node               -> exact (__lt runs): both number
--   node < number             -> Lua raises before any metamethod. The error
--                                handler reads the erroring frame's registers and
--                                scans its ISLT/ISGE/ISLE/ISGT instructions for a
--                                node-vs-number operand pair; one candidate is
--                                "compare", several is "compare-ambiguous" (the
--                                newest node wins). An ambiguous guess that is
--                                later indexed/called as a number is pinned back
--                                to object ("conflict").
--   x:M() == false/true/7/"s" -> equality never raises, so before running, a
--                                static pass over the original's bytecode finds
--                                TGETS/GGET rA "M"; CALL rA; ... ISEQP/ISNEP,
--                                ISEQN/ISNEN, ISEQS/ISNES on rA: boolean / number
--                                / string ("static-iseqp" etc.)
-- Keys: "ret NAME" (what calling member NAME returns), "field NAME", "arg FN#I",
-- "global NAME". Name-only, shared across files.
-- In trace mode the same metamethods raise the message Lua itself would.
-- node == node is never learned from: __eq answers true with probability 1/3
-- from hash(seed, the sorted pair of paths), logged as "EQ", so branches gated
-- on GetNowStateID() == GetStateID_LUA(...) get explored on both sides alike.
--
-- Frames. Every file-defined function is wrapped when it is assigned, so nested
-- calls between file functions show up as FCALL/FRET and keep a frame stack.
-- A slotN a callee (or its callees) wrote, read by the caller afterwards
-- without rewriting it first, is "CLOBBER caller callee slotN had|unset"
-- (had = the caller had written it before the call). Only the round trip has
-- slotN globals, so only it can clobber.
-- Engine re-entry: a CALL whose method is listed "synchronous" in reentrant.tsv
-- is handled by that row's `sim` column:
--   named-arg args=...   run the file function named by the first string arg
--   state-table START=F END=G args=...
--                        the first string arg names a global state table; run
--                        the old state's G (if this receiver changed state
--                        earlier in the call), then the new state's F
--   none:<why>           not simulated; the call is counted in the call's
--                        "unsim" set so a sweep can say what it could not see
-- args= lists what the engine passes (app g_pKTDXApp, game g_pX2Game, main
-- g_pMain, sgame g_pSquareGame, this the receiver). A re-entered function runs
-- as a nested frame, the way lua_tinker::call would: its script errors are
-- swallowed and recorded (REENTER_ERROR), control errors are not.
--
-- Harness errors (a bug in this file, a chunk that will not load) are
-- HARNESS_ERROR lines and counts in the trace; the script's own errors are
-- outcomes. A count hook ends runaway calls with BUDGET.

jit.off()
jit.flush()

local bit = require('bit')
local band, bxor, lshift, rshift, tobit = bit.band, bit.bxor, bit.lshift, bit.rshift, bit.tobit
local sbyte, sformat = string.byte, string.format
local funcinfo, funcbc = jit.util.funcinfo, jit.util.funcbc

local MODE, CHUNK, OUT, TYPES_PATH, REENTRANT_PATH = arg[1], arg[2], arg[3], arg[4], arg[5]
local NSEEDS, LABEL, SIDE = tonumber(arg[6]) or 4, arg[7] or '?', arg[8] or '?'
assert(MODE == 'learn' or MODE == 'trace', 'usage: ljdiff.lua learn|trace CHUNK OUT TYPES REENTRANT SEEDS LABEL SIDE')

local CALL_BUDGET = 2000000        -- VM instructions per function call
local MAIN_BUDGET = 200000000      -- for the main chunk (big data files)
local HOOK_STEP = 100000
local CALL_EVENT_CAP = 4000
local MAIN_EVENT_CAP = 400000
local LEARN_ATTEMPTS = 60          -- retries of one call while learning
local FMT_ENTRIES = 256            -- entries printed per table level
local RESTORE_CELL_CAP = 400000    -- deep-restore main-chunk tables up to this size

local function pack(...) return { n = select('#', ...), ... } end

---------------------------------------------------------------- hashing
local function h32(s)
	local h = tobit(0x811c9dc5)
	for i = 1, #s do
		h = bxor(h, sbyte(s, i))
		h = tobit(h * 403 + lshift(h, 24))   -- h * 16777619 mod 2^32
	end
	if h < 0 then h = h + 4294967296 end
	return h
end

---------------------------------------------------------------- control errors
-- Raised through script code; the sandbox's pcall re-raises them.
local LEARN = setmetatable({}, { __tostring = function() return '__LJDIFF_LEARN__' end })
local BUDGET = setmetatable({}, { __tostring = function() return '__LJDIFF_BUDGET__' end })
local HARNESS_MT = { __tostring = function(e) return '__LJDIFF_HARNESS__ ' .. e.msg end }
local SCRIPT_MT = {}
local function is_control(e)
	return e == LEARN or e == BUDGET or getmetatable(e) == HARNESS_MT
end
local function serr(msg) error(setmetatable({ msg = msg }, SCRIPT_MT), 0) end

---------------------------------------------------------------- per-call event state
local EV, nev, cap, truncated
local rec          -- the current call's record (poison/upvalue/clobber/harness sets)
local function new_rec(ecap)
	EV, nev, cap, truncated = {}, 0, ecap, false
	rec = { poison = {}, upvalue = {}, clobber = {}, harness = {}, unsim = {}, reenter = 0, fatal = nil }
end
local function emit(s)
	nev = nev + 1
	if nev > cap then truncated = true; return end
	EV[nev] = s
end
local function harness(msg)
	rec.harness[#rec.harness + 1] = msg
	emit('HARNESS_ERROR ' .. msg)
	return setmetatable({ msg = msg }, HARNESS_MT)
end

---------------------------------------------------------------- formatting
local NODE = setmetatable({}, { __mode = 'k' })     -- node -> info
local SENT = setmetatable({}, { __mode = 'k' })     -- sentinel -> name
local WRAPPER = setmetatable({}, { __mode = 'k' })  -- wrapper -> file function name

local fmt
local function fmtnum(n)
	if n ~= n then return 'nan' end
	if n == math.huge then return 'inf' end
	if n == -math.huge then return '-inf' end
	if n == math.floor(n) and math.abs(n) < 1e15 then return sformat('%d', n) end
	return sformat('%.14g', n)
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
	elseif t == 'string' then return sformat('%q', v)
	elseif t == 'nil' or t == 'boolean' then return tostring(v)
	elseif t == 'function' then
		return WRAPPER[v] and ('<fn ' .. WRAPPER[v] .. '>') or '<function>'
	elseif t == 'table' then
		if NODE[v] then return '<' .. NODE[v].path .. '>' end
		if SENT[v] then return '<poison ' .. SENT[v] .. '>' end
		depth = depth or 0
		if depth > 8 then return '{...}' end
		local keys = {}
		for k in pairs(v) do keys[#keys + 1] = k end
		table.sort(keys, keyless)
		local parts = {}
		for i, k in ipairs(keys) do
			if i > FMT_ENTRIES then parts[#parts + 1] = '...+' .. (#keys - FMT_ENTRIES); break end
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
local function kstr(k)
	if type(k) == 'string' then return k end
	return '[' .. fmt(k) .. ']'
end

-- Error text: drop the "chunk:line: " prefix and the variable annotation,
-- which differ between the original ("local"/none) and the round trip
-- ("global 'slot5'") for the same failure.
local function normerr(e)
	if type(e) ~= 'string' then return 'non-string error ' .. fmt(e) end
	e = e:gsub('^.-:%d+: ', '')
	e = e:gsub("%a+ '[^']*' %((an? %a+ value)%)", '%1')
	return e
end

---------------------------------------------------------------- types
local TYPES = {}           -- "kind\tname" -> number|string|object
local LEARNED = {}         -- rows learned in this process (learn mode)
local CONFLICT_OK = {}     -- key -> true if it was learned by an ambiguous guess here
local types_h = 0
do
	local fh = io.open(TYPES_PATH, 'rb')
	if fh then
		local data = fh:read('*a')
		fh:close()
		types_h = h32(data)
		for line in data:gmatch('[^\n]+') do
			line = line:gsub('\r$', '')
			if line:sub(1, 1) ~= '#' then
				local kind, name, typ = line:match('^([^\t]+)\t([^\t]+)\t([^\t]+)')
				if kind and kind ~= 'kind' then TYPES[kind .. '\t' .. name] = typ end
			end
		end
	end
end
local WHERE = 'main'       -- file/function currently running, for learned rows

local function learn(key, typ, evidence)
	if MODE ~= 'learn' or TYPES[key] ~= nil then return end
	TYPES[key] = typ
	LEARNED[#LEARNED + 1] = { key, typ, evidence, WHERE }
	if evidence == 'compare-ambiguous' then CONFLICT_OK[key] = true end
	error(LEARN, 0)
end

---------------------------------------------------------------- re-entry table
local REENTRANT = {}       -- lua method name -> parsed sim of its simulated row
local UNSIM = {}           -- lua method name -> reason (synchronous, not simulated)
do
	local fh = io.open(REENTRANT_PATH, 'rb')
	if fh then
		local header
		for line in fh:read('*a'):gmatch('[^\n]+') do
			line = line:gsub('\r$', '')
			if line:sub(1, 1) ~= '#' then
				local cols = {}
				for c in (line .. '\t'):gmatch('([^\t]*)\t') do cols[#cols + 1] = c end
				if not header then
					header = {}
					for i, c in ipairs(cols) do header[c] = i end
				else
					local row = {}
					for c, i in pairs(header) do row[c] = cols[i] or '' end
					local sim = row.sim or ''
					if row.verdict == 'synchronous' and row.lua_name ~= '' then
						if sim == '' or sim == '-' then
							error('reentrant.tsv: synchronous row without sim: ' .. row.lua_name)
						elseif sim:match('^none') then
							UNSIM[row.lua_name] = UNSIM[row.lua_name] or sim
						else
							local kind = sim:match('^(%S+)')
							local r = { kind = kind, args = {} }
							for k, v in sim:gmatch('(%u+)=(%S+)') do r[k] = v end
							for a in (sim:match('args=(%S+)') or ''):gmatch('[^,]+') do r.args[#r.args + 1] = a end
							if kind ~= 'named-arg' and kind ~= 'state-table' then
								error('reentrant.tsv: unknown sim kind ' .. tostring(kind) .. ' for ' .. row.lua_name)
							end
							assert(REENTRANT[row.lua_name] == nil, 'reentrant.tsv: two simulated rows for ' .. row.lua_name)
							REENTRANT[row.lua_name] = r
						end
					end
				end
			end
		end
		fh:close()
	end
end

---------------------------------------------------------------- deterministic values
local SEED = 0
local function numval(h)
	local r, q = h % 100, math.floor(h / 100)
	if r < 60 then return q % 8 end                 -- small ints: enums, indices, counts
	if r < 85 then return (q % 1000) / 1000 end     -- fractions: ratios, rates
	return q % 10000                                -- large: HP, times, ids
end
local function hval(path, count)
	return h32(SEED .. '|' .. path .. '|' .. (count or 0))
end

---------------------------------------------------------------- nodes
local NMT = {}
local serial = 0
local IN_MAIN = false
local MAIN_NODES = {}
local function newnode(path, key, parent, name)
	if #path > 160 then path = path:sub(1, 60) .. '~' .. sformat('%08x', h32(path)) .. '~' .. path:sub(-60) end
	local n = setmetatable({}, NMT)
	serial = serial + 1
	NODE[n] = { path = path, key = key, parent = parent, name = name,
		members = {}, sets = {}, counts = {}, ncalls = 0, serial = serial, used = false }
	if IN_MAIN then MAIN_NODES[#MAIN_NODES + 1] = n end
	return n
end

-- The value a typed key yields. nilable: returns only.
local function typed(key, path, count, nilable)
	local t = TYPES[key]
	if t == 'number' then return numval(hval(path, count)), true end
	if t == 'string' then return 's' .. (hval(path, count) % 1000), true end
	if t == 'boolean' then return hval(path, count) % 2 == 0, true end
	if nilable and hval(path, count) % 6 == 0 then return nil, true end
	return nil, false          -- caller makes a node
end

local NILBOX = {}
local function box(v) if v == nil then return NILBOX end return v end
local function unbox(v) if v == NILBOX then return nil end return v end

-- Mock bodies run under guard(): an intentional script-visible error goes
-- through serr(), control errors pass through, anything else is a harness bug.
local function guard(body)
	return function(...)
		local r = pack(pcall(body, ...))
		if r[1] then return unpack(r, 2, r.n) end
		local e = r[2]
		if is_control(e) then error(e, 0) end
		if getmetatable(e) == SCRIPT_MT then error(e.msg, 0) end
		error(harness('mock: ' .. tostring(e)), 0)
	end
end

local invoke_reentry   -- defined with the frames below

NMT.__metatable = 'ljdiff-mock'
NMT.__tostring = function(n) return '<' .. NODE[n].path .. '>' end
NMT.__index = guard(function(n, k)
	local info = NODE[n]
	info.used = true
	local ks = kstr(k)
	local s = info.sets[k]
	if s ~= nil then
		local v = unbox(s)
		emit('GET ' .. info.path .. '.' .. ks .. ' -> ' .. fmt(v))
		return v
	end
	local m = info.members[k]
	if m ~= nil then return unbox(m) end
	local key = 'field\t' .. ks
	local v, done = typed(key, info.path .. '.' .. ks, 0, false)
	if done then
		emit('GET ' .. info.path .. '.' .. ks .. ' -> ' .. fmt(v))
	else
		v = newnode(info.path .. '.' .. ks, key, n, ks)
	end
	info.members[k] = box(v)
	return v
end)
NMT.__newindex = guard(function(n, k, v)
	local info = NODE[n]
	info.used = true
	emit('SET ' .. info.path .. '.' .. kstr(k) .. ' = ' .. fmt(v))
	info.sets[k] = box(v)
end)
NMT.__call = guard(function(n, ...)
	local info = NODE[n]
	info.used = true
	local nargs = select('#', ...)
	local first = ...
	local desc, argtext
	local parent = info.parent
	-- rawequal: node == node runs __eq below, which would label a call on a
	-- stale receiver as a method call on the right one
	if parent ~= nil and nargs >= 1 and rawequal(first, parent) then
		desc = NODE[parent].path .. ':' .. info.name
		argtext = fmtlist(nargs - 1, select(2, ...))
	else
		desc = info.path
		argtext = fmtlist(nargs, ...)
	end
	info.ncalls = info.ncalls + 1
	local c = info.ncalls
	local rkey = 'ret\t' .. (info.name or '()')
	local rpath = desc .. '()#' .. c
	local r, done = typed(rkey, rpath, c, true)
	if not done then r = newnode(rpath, rkey, nil, nil) end
	emit('CALL ' .. desc .. '(' .. argtext .. ') -> ' .. fmt(r))
	if info.name then
		local row = REENTRANT[info.name]
		if row then invoke_reentry(row, info.name, parent, ...)
		elseif UNSIM[info.name] then rec.unsim[info.name] = true end
	end
	return r
end)
local function arith(op, typ)
	return guard(function(a, b)
		local n = NODE[a] and a or b
		learn(NODE[n].key, typ, 'arith-' .. op)
		if op == 'concat' then serr('attempt to concatenate a table value') end
		serr('attempt to perform arithmetic on a table value')
	end)
end
for _, op in ipairs({ 'add', 'sub', 'mul', 'div', 'mod', 'pow', 'unm' }) do NMT['__' .. op] = arith(op, 'number') end
NMT.__concat = arith('concat', 'string')
local function order(op)
	return guard(function(a, b)
		-- Lua 5.1 runs __lt/__le only when both operands are tables with the
		-- same handler, i.e. both nodes here.
		learn(NODE[a].key, 'number', 'compare-both')
		learn(NODE[b].key, 'number', 'compare-both')
		serr('attempt to compare two table values')
	end)
end
NMT.__lt = order('lt')
NMT.__le = order('le')
NMT.__eq = guard(function(a, b)
	local pa, pb = NODE[a].path, NODE[b].path
	if pb < pa then pa, pb = pb, pa end
	local r = hval(pa .. '==' .. pb, 0) % 3 == 0
	emit('EQ <' .. NODE[a].path .. '> <' .. NODE[b].path .. '> -> ' .. tostring(r))
	return r
end)

---------------------------------------------------------------- poison sentinel
local last_read
local SMT = {}
local function sentinel(name)
	local s = setmetatable({}, SMT)
	SENT[s] = name
	return s
end
local function suse(s, what) emit('POISON_USE ' .. SENT[s] .. ' ' .. what) end
SMT.__metatable = 'ljdiff-poison'
SMT.__index = guard(function(s, k) suse(s, 'index ' .. fmt(k)); return s end)
SMT.__newindex = guard(function(s, k) suse(s, 'newindex ' .. fmt(k)) end)
SMT.__call = guard(function(s) suse(s, 'call'); return s end)
SMT.__tostring = function(s) return '<poison ' .. SENT[s] .. '>' end
for _, mm in ipairs({ 'add', 'sub', 'mul', 'div', 'mod', 'pow', 'unm', 'concat', 'lt', 'le', 'eq' }) do
	SMT['__' .. mm] = guard(function(a, b)
		local s = SENT[a] and a or b
		suse(s, mm)
		if mm == 'lt' or mm == 'le' or mm == 'eq' then return false end
		return s
	end)
end

---------------------------------------------------------------- compare attribution
local BC = {}
do
	local vmdef = require('jit.vmdef')
	for op = 0, 255 do
		local s = vmdef.bcnames:sub(op * 6 + 1, op * 6 + 6)
		if s == '' then break end
		s = s:gsub(' +$', '')
		if s == 'ISLT' or s == 'ISGE' or s == 'ISLE' or s == 'ISGT' then BC[op] = s end
	end
	assert(BC[0] == 'ISLT' and BC[3] == 'ISGT', 'unexpected jit.vmdef opcode order')
end

-- Called (through pcall) from the error handler. The erroring frame is the
-- first Lua frame above this function, the pcall and the handler. Returns
-- (node, evidence) or nil.
local handler
local function attribute_compare()
	local LV, f
	for lv = 2, 12 do
		local info = debug.getinfo(lv, 'fS')
		if not info then return nil end
		if info.what ~= 'C' and info.func ~= handler then LV, f = lv, info.func; break end
	end
	if not f then return nil end
	local fi = funcinfo(f)
	if not fi or not fi.stackslots then return nil end   -- a C function
	local regs = {}
	for i = 1, fi.stackslots do
		local name, v = debug.getlocal(LV, i)
		if not name then break end
		regs[i - 1] = v
	end
	local function cand(v)
		local ni = NODE[v]
		return ni and not ni.used and TYPES[ni.key] == nil
	end
	local found, nfound = {}, 0
	local pc = 0
	while true do
		local ins = funcbc(f, pc)
		if not ins then break end
		if BC[band(ins, 0xff)] then
			local ra, rd = band(rshift(ins, 8), 0xff), rshift(ins, 16)
			local va, vd = regs[ra], regs[rd]
			local pick
			if cand(va) and type(vd) == 'number' then pick = va
			elseif cand(vd) and type(va) == 'number' then pick = vd end
			if pick and not found[pick] then found[pick] = true; nfound = nfound + 1 end
		end
		pc = pc + 1
	end
	if nfound == 0 then return nil end
	local best
	for n in pairs(found) do
		if not best or NODE[n].serial > NODE[best].serial then best = n end
	end
	return best, (nfound == 1) and 'compare' or 'compare-ambiguous'
end

-- Static pass (learn mode, original only): x:M() compared with a boolean,
-- number or string constant. Equality raises nothing at run time, so this is
-- the only way to learn those types.
local OPS = {}
do
	local vmdef = require('jit.vmdef')
	for op = 0, 255 do
		local s = vmdef.bcnames:sub(op * 6 + 1, op * 6 + 6)
		if s == '' then break end
		OPS[(s:gsub(' +$', ''))] = op
	end
end
local BCM_DST, BCM_BASE = 1, 2
local EQ_OPS = {
	[OPS.ISEQP] = 'p', [OPS.ISNEP] = 'p', [OPS.ISEQN] = 'number', [OPS.ISNEN] = 'number',
	[OPS.ISEQS] = 'string', [OPS.ISNES] = 'string',
}
local function writes(ins, m, r)
	local ma, a = band(m, 7), band(rshift(ins, 8), 0xff)
	return (ma == BCM_DST and a == r) or (ma == BCM_BASE and a <= r)
end
local function static_learn(root, is_file_fn)
	local seen, n = {}, 0
	local function scan(f)
		if seen[f] then return end
		seen[f] = true
		local code = {}
		local pc = 0
		while true do
			local ins, m = funcbc(f, pc)
			if not ins then break end
			code[pc] = { ins, m }
			pc = pc + 1
		end
		for i = 0, pc - 1 do
			local ins = code[i][1]
			local op = band(ins, 0xff)
			if op == OPS.FNEW then
				local child = jit.util.funck(f, -rshift(ins, 16) - 1)
				if child then scan(child) end
			elseif (op == OPS.CALL or op == OPS.CALLM) and rshift(ins, 24) ~= 1 then
				local ra = band(rshift(ins, 8), 0xff)
				-- the instruction that loaded the callee into rA
				local name
				for j = i - 1, math.max(0, i - 64), -1 do
					local ji, jm = code[j][1], code[j][2]
					local jop = band(ji, 0xff)
					if jop == OPS.TGETS and band(rshift(ji, 8), 0xff) == ra then
						name = jit.util.funck(f, -band(rshift(ji, 16), 0xff) - 1); break
					elseif jop == OPS.GGET and band(rshift(ji, 8), 0xff) == ra then
						name = jit.util.funck(f, -rshift(ji, 16) - 1); break
					elseif writes(ji, jm, ra) then break end
				end
				if type(name) == 'string' then
					for j = i + 1, math.min(pc - 1, i + 64) do
						local ji, jm = code[j][1], code[j][2]
						local kind = EQ_OPS[band(ji, 0xff)]
						if kind and band(rshift(ji, 8), 0xff) == ra then
							local d = rshift(ji, 16)
							local typ, ev = kind, 'static-iseq' .. (kind == 'number' and 'n' or 's')
							-- pri operand: 0 nil (says nothing), 1 false, 2 true
							if kind == 'p' then typ, ev = (d == 1 or d == 2) and 'boolean' or nil, 'static-iseqp' end
							-- a function the chunk defines itself is not an engine method
							if is_file_fn(name) then typ = nil end
							if typ then
								local key = 'ret\t' .. name
								if TYPES[key] == nil then
									TYPES[key] = typ
									LEARNED[#LEARNED + 1] = { key, typ, ev, 'static' }
									n = n + 1
								end
								break
							end
						elseif writes(ji, jm, ra) then break end
					end
				end
			end
		end
	end
	scan(root)
	return n
end

local pending_attr     -- set by the first (deepest) handler run of an error
handler = function(e)
	if pending_attr == nil and MODE == 'learn' and type(e) == 'string' then
		local a, b = e:match('attempt to compare (%a+) with (%a+)')
		if a == 'table' or b == 'table' then
			local ok, n, ev = pcall(attribute_compare)
			if ok and n then pending_attr = { n = n, ev = ev } else pending_attr = false end
		end
	end
	return e
end
local function clear_attr() pending_attr = nil end

---------------------------------------------------------------- environment
local math_mock = {}
for k, v in pairs(math) do math_mock[k] = v end
local rand_count = 0
math_mock.random = function(m, n)
	rand_count = rand_count + 1
	local x = hval('math.random', rand_count) / 4294967296
	local r
	if m == nil then r = x
	elseif n == nil then r = math.floor(x * m) + 1
	else r = m + math.floor(x * (n - m + 1)) end
	emit('RANDOM math.random(' .. fmtlist((m == nil) and 0 or ((n == nil) and 1 or 2), m, n) .. ') -> ' .. fmt(r))
	return r
end
math_mock.randomseed = function(...) emit('RANDOM math.randomseed(' .. fmtlist(select('#', ...), ...) .. ')') end

local env = {}
local BASE = {
	pairs = pairs, ipairs = ipairs, next = next, type = type, select = select,
	tostring = tostring, tonumber = tonumber, unpack = unpack, error = error,
	assert = assert, setmetatable = setmetatable, getmetatable = getmetatable,
	rawget = rawget, rawset = rawset, rawequal = rawequal,
	math = math_mock, string = string, table = table, _G = env,
	-- a script's own pcall must not swallow the harness's control errors
	-- (and an error it does swallow must not keep a pending compare attribution)
	pcall = function(f, ...)
		local r = pack(pcall(f, ...))
		if not r[1] then
			if is_control(r[2]) then error(r[2], 0) end
			clear_attr()
		end
		return unpack(r, 1, r.n)
	end,
	xpcall = function(f, h)
		local r = pack(xpcall(f, function(e) if is_control(e) then return e end return h(e) end))
		if not r[1] then
			if is_control(r[2]) then error(r[2], 0) end
			clear_attr()
		end
		return unpack(r, 1, r.n)
	end,
}

local store, written = {}, {}
local GNODES = {}               -- engine global name -> node, per call
local FRAMES = {}

local function isslot(k) return type(k) == 'string' and k:match('^slot%d+$') ~= nil end

local RAW = setmetatable({}, { __mode = 'k' })       -- wrapper -> raw function
local WRAP_OF = setmetatable({}, { __mode = 'k' })   -- raw function -> wrapper
local function wrap(name, f)
	if WRAPPER[f] then return f end
	if WRAP_OF[f] then return WRAP_OF[f] end
	local w
	w = function(...)
		local parent = FRAMES[#FRAMES]
		local frame = { fn = name, own = {}, desc = {}, taint = {} }
		if parent then emit('FCALL ' .. name .. '(' .. fmtlist(select('#', ...), ...) .. ')') end
		FRAMES[#FRAMES + 1] = frame
		local args = pack(...)
		local r = pack(xpcall(function() return f(unpack(args, 1, args.n)) end, handler))
		FRAMES[#FRAMES] = nil
		if parent then
			for k in pairs(frame.desc) do
				parent.desc[k] = true
				parent.taint[k] = { callee = name, had = parent.own[k] and 'had' or 'unset' }
			end
			if r[1] then emit('FRET ' .. name .. ' -> ' .. fmtlist(r.n - 1, unpack(r, 2, r.n)))
			elseif not is_control(r[2]) then emit('FERR ' .. name .. ' ' .. normerr(r[2])) end
		end
		if not r[1] then error(r[2], 0) end
		return unpack(r, 2, r.n)
	end
	WRAPPER[w] = name
	RAW[w] = f
	WRAP_OF[f] = w
	return w
end

setmetatable(env, {
	__newindex = function(t, k, v)
		if type(v) == 'function' and type(k) == 'string' then v = wrap(k, v) end
		written[k] = true
		store[k] = v
		if isslot(k) then
			local fr = FRAMES[#FRAMES]
			if fr then fr.own[k] = true; fr.desc[k] = true; fr.taint[k] = nil end
		end
	end,
	__index = function(t, k)
		if written[k] then
			if isslot(k) then
				local fr = FRAMES[#FRAMES]
				local tn = fr and fr.taint[k]
				if tn then
					local line = 'CLOBBER ' .. fr.fn .. ' ' .. tn.callee .. ' ' .. k .. ' ' .. tn.had
					if not rec.clobber[line] then rec.clobber[line] = true; emit(line) end
					fr.taint[k] = nil
				end
			end
			return store[k]
		end
		local b = BASE[k]
		if b ~= nil then return b end
		if isslot(k) then
			emit('POISON_READ ' .. k)
			rec.poison[k] = true
			last_read = 'POISON_READ ' .. k
			return sentinel(k)
		end
		if type(k) == 'string' and k:match('^uv%d+$') then
			emit('UPVALUE_READ ' .. k)
			rec.upvalue[k] = true
			last_read = 'UPVALUE_READ ' .. k
			return sentinel(k)
		end
		local g = GNODES[k]
		if g ~= nil then return unbox(g) end
		local ks = kstr(k)
		emit('GREAD ' .. ks)
		local v, done = typed('global\t' .. ks, ks, 0, false)
		if not done then v = newnode(ks, 'global\t' .. ks, nil, ks) end
		GNODES[k] = box(v)
		return v
	end,
})

---------------------------------------------------------------- budget
local steps, step_limit = 0, 0
local function hook()
	steps = steps + HOOK_STEP
	if steps >= step_limit then error(BUDGET, 0) end
end
local function run_guarded(limit, fn)
	steps, step_limit = 0, limit
	pending_attr = nil
	debug.sethook(hook, '', HOOK_STEP)
	local r = pack(xpcall(fn, handler))
	debug.sethook()
	return r
end

-- Outcome text for a finished run; also classifies harness errors.
local function outcome(r)
	if r[1] then return 'RET ' .. fmtlist(r.n - 1, unpack(r, 2, r.n)) end
	local e = r[2]
	if e == BUDGET then return 'BUDGET' end
	if e == LEARN then return 'LEARN' end
	if getmetatable(e) == HARNESS_MT then return 'HARNESS ' .. e.msg end
	local ne = normerr(e)
	if last_read and ne:match('^attempt to compare') and ne:match('table') then
		rec.fatal = last_read .. ' (fatal)'
	end
	return 'ERROR ' .. ne
end

-- learn mode: turn a finished run's error into a retype, or return oc as is.
local function learn_from(oc)
	if MODE ~= 'learn' then return oc end
	-- a compare error the handler attributed to one node
	if oc:match('^ERROR attempt to compare') and pending_attr then
		local ok, e = pcall(learn, NODE[pending_attr.n].key, 'number', pending_attr.ev)
		if not ok and e == LEARN then return 'LEARN' end
	end
	-- an ambiguous guess now used as an object -> pin it back
	if oc:match('^ERROR attempt to index a number') or oc:match('^ERROR attempt to call a number') then
		local last = LEARNED[#LEARNED]
		if last and CONFLICT_OK[last[1]] then
			CONFLICT_OK[last[1]] = nil
			TYPES[last[1]] = 'object'
			last[2], last[3] = 'object', 'conflict'
			return 'LEARN'
		end
	end
	return oc
end

---------------------------------------------------------------- main chunk + restore
local chunk, lerr = loadfile(CHUNK)

local main_store, main_written, main_nodes_state
local function run_main()
	store, written, GNODES, FRAMES = {}, {}, {}, {}
	MAIN_NODES = {}
	SEED, rand_count, last_read = 0, 0, nil
	WHERE = 'main'
	new_rec(MAIN_EVENT_CAP)
	IN_MAIN = true
	setfenv(chunk, env)
	local r = run_guarded(MAIN_BUDGET, chunk)
	IN_MAIN = false
	return r
end

-- Deep copy of the post-main state so a call that mutates a main-chunk table
-- cannot leak into the next call. Functions and nodes are shared; nodes get
-- their own state reset separately.
local restore_mode = 'deep'
local function count_cells(v, seen, n)
	if type(v) ~= 'table' or NODE[v] or SENT[v] or seen[v] then return n end
	seen[v] = true
	for k, x in pairs(v) do
		n = n + 1
		if n > RESTORE_CELL_CAP then return n end
		n = count_cells(k, seen, n)
		n = count_cells(x, seen, n)
	end
	return n
end
local function deepcopy(v, seen)
	if type(v) ~= 'table' or NODE[v] or SENT[v] then return v end
	if seen[v] then return seen[v] end
	local c = {}
	seen[v] = c
	for k, x in pairs(v) do c[deepcopy(k, seen)] = deepcopy(x, seen) end
	local mt = getmetatable(v)
	if mt ~= nil then setmetatable(c, mt) end
	return c
end
local function snapshot_main()
	main_store, main_written = {}, {}
	for k in pairs(written) do main_written[k] = true; main_store[k] = store[k] end
	local n, seen = 0, {}
	for _, v in pairs(main_store) do
		n = count_cells(v, seen, n)
		if n > RESTORE_CELL_CAP then break end
	end
	if n > RESTORE_CELL_CAP then restore_mode = 'shallow' end
	main_nodes_state = {}
	for i, nd in ipairs(MAIN_NODES) do
		local info = NODE[nd]
		local m, s = {}, {}
		for k, v in pairs(info.members) do m[k] = v end
		for k, v in pairs(info.sets) do s[k] = v end
		main_nodes_state[i] = { m, s, info.ncalls }
	end
end
local function restore()
	written, GNODES, FRAMES = {}, {}, {}
	if restore_mode == 'deep' then
		store = deepcopy(main_store, {})
	else
		store = {}
		for k, v in pairs(main_store) do store[k] = v end
	end
	for k in pairs(main_written) do written[k] = true end
	for i, nd in ipairs(MAIN_NODES) do
		local info, st = NODE[nd], main_nodes_state[i]
		info.members, info.sets, info.ncalls = {}, {}, st[3]
		for k, v in pairs(st[1]) do info.members[k] = v end
		for k, v in pairs(st[2]) do info.sets[k] = v end
		info.used = false
		info.state = nil
	end
end

---------------------------------------------------------------- entry points
-- Global file functions, plus functions stored one or more levels deep in
-- main-chunk tables (named by path), each called with mock arguments.
local function entry_points()
	local list, seenf = {}, {}
	local names = {}
	for k in pairs(main_written) do if type(k) == 'string' then names[#names + 1] = k end end
	table.sort(names)
	for _, k in ipairs(names) do
		local v = main_store[k]
		if type(v) == 'function' and WRAPPER[v] and not seenf[v] then
			seenf[v] = true
			list[#list + 1] = { name = k, raw = RAW[v] }
		end
	end
	local visited = {}
	local function walk(t, path, chain, depth)
		if depth > 6 or visited[t] or NODE[t] or SENT[t] then return end
		visited[t] = true
		local keys = {}
		for k in pairs(t) do keys[#keys + 1] = k end
		table.sort(keys, keyless)
		for _, k in ipairs(keys) do
			local v = rawget(t, k)
			local p = path .. (type(k) == 'string' and ('.' .. k) or ('[' .. fmt(k) .. ']'))
			local ch = { unpack(chain) }
			ch[#ch + 1] = k
			if type(v) == 'function' and not seenf[v] then
				seenf[v] = true
				list[#list + 1] = { name = p, nested = true, chain = ch, raw = RAW[v] or v }
			elseif type(v) == 'table' then
				walk(v, p, ch, depth + 1)
			end
		end
	end
	for _, k in ipairs(names) do
		local v = main_store[k]
		if type(v) == 'table' then walk(v, k, { k }, 1) end
	end
	return list
end

local function mock_args(ep)
	local fi = funcinfo(ep.raw)
	local n = (fi and fi.params) or 0
	local args = { n = n }
	for i = 0, n - 1 do
		local key = 'arg\t' .. ep.name .. '#' .. i
		local v, done = typed(key, 'a' .. i, 0, false)
		if not done then v = newnode('a' .. i, key, nil, nil) end
		args[i + 1] = v
	end
	return args
end

-- Engine re-entry (reentrant.tsv, see the header). Engine globals passed as
-- arguments are fetched without a GREAD event: the engine, not the script,
-- is reading them.
local function engine_arg(name, parent)
	if name == 'this' then return parent end
	local g = ({ app = 'g_pKTDXApp', game = 'g_pX2Game', main = 'g_pMain', sgame = 'g_pSquareGame' })[name]
	if not g then error(harness('reentrant.tsv: unknown args entry ' .. name), 0) end
	if written[g] then return store[g] end
	local v = GNODES[g]
	if v == nil then
		v = newnode(g, 'global\t' .. g, nil, g)
		GNODES[g] = v
	end
	return unbox(v)
end

local function file_fn(name)
	if type(name) == 'string' and written[name] and type(store[name]) == 'function' then return store[name] end
	return nil
end

local function reenter(method, fname, row, parent)
	local f = file_fn(fname)
	if not f then
		emit('REENTER ' .. method .. ' -> ' .. tostring(fname) .. ' (not defined in this file)')
		return
	end
	rec.reenter = rec.reenter + 1
	emit('REENTER ' .. method .. ' -> ' .. fname)
	local a = {}
	for i, name in ipairs(row.args) do a[i] = engine_arg(name, parent) end
	local r = pack(pcall(f, unpack(a, 1, #row.args)))
	if not r[1] then
		if is_control(r[2]) then error(r[2], 0) end
		pending_attr = nil     -- swallowed: must not claim the caller's next error
		emit('REENTER_ERROR ' .. fname .. ' ' .. normerr(r[2]))
	end
end

invoke_reentry = function(row, method, parent, ...)
	local sarg
	for i = 1, select('#', ...) do
		local a = select(i, ...)
		if type(a) == 'string' then sarg = a; break end
	end
	if row.kind == 'named-arg' then
		if sarg and file_fn(sarg) then reenter(method, sarg, row, parent)
		else emit('REENTER ' .. method .. ' -> (no file function named)') end
		return
	end
	-- state-table: silent when the argument names no state table with a START
	-- field - that is how CX2GUNPC's deferred StateChange_LUA (same Lua name)
	-- stays unsimulated in NPC scripts, which register states in INIT_STATE.
	local t = sarg and written[sarg] and store[sarg]
	if type(t) ~= 'table' then return end
	local start = rawget(t, row.START)
	if type(start) ~= 'string' then return end
	local pinfo = parent and NODE[parent]
	local old = pinfo and pinfo.state
	if old and row.END then
		local ot = written[old] and store[old]
		local fin = type(ot) == 'table' and rawget(ot, row.END)
		if type(fin) == 'string' then reenter(method, fin, row, parent) end
	end
	if pinfo then pinfo.state = sarg end
	reenter(method, start, row, parent)
end

---------------------------------------------------------------- run
local function sorted_keys(set)
	local l = {}
	for k in pairs(set) do l[#l + 1] = k end
	table.sort(l)
	return l
end

local calls_out = {}
local main_out
local load_error

if not chunk then
	load_error = normerr(lerr)
else
	if MODE == 'learn' then
		for attempt = 1, LEARN_ATTEMPTS do
			local r = run_main()
			if r[1] then break end
			if r[2] ~= LEARN and learn_from(outcome(r)) ~= 'LEARN' then break end
		end
		-- after main, so it knows which names the chunk defines itself
		static_learn(chunk, function(name) return written[name] and type(store[name]) == 'function' end)
	end
	local r = run_main()
	main_out = { outcome = outcome(r), events = EV, nevents = nev, truncated = truncated, rec = rec }
	snapshot_main()

	local eps = entry_points()
	main_out.entries = eps

	for _, ep in ipairs(eps) do
		for seed = 1, NSEEDS do
			local result
			for attempt = 1, (MODE == 'learn') and LEARN_ATTEMPTS or 1 do
				restore()
				SEED, rand_count, last_read = seed, 0, nil
				WHERE = ep.name .. '#' .. seed
				new_rec(CALL_EVENT_CAP)
				local f
				if ep.nested then
					-- walk the same key chain in this call's restored store
					local v = store
					for _, key in ipairs(ep.chain) do
						if type(v) ~= 'table' then v = nil; break end
						v = rawget(v, key)
					end
					f = v
					if type(f) == 'function' and not WRAPPER[f] then f = wrap(ep.name, f) end
				else
					f = store[ep.name]
				end
				local args = mock_args(ep)
				local r
				if type(f) ~= 'function' then
					r = { false, harness('entry ' .. ep.name .. ' is not a function after restore'), n = 2 }
				else
					r = run_guarded(CALL_BUDGET, function() return f(unpack(args, 1, args.n)) end)
				end
				local oc = learn_from(outcome(r))
				result = { fn = ep.name, seed = seed, arity = args.n,
					args = fmtlist(args.n, unpack(args, 1, args.n)),
					outcome = oc, events = EV, nevents = nev, truncated = truncated, rec = rec }
				if oc ~= 'LEARN' then break end
			end
			calls_out[#calls_out + 1] = result
		end
	end
end

---------------------------------------------------------------- output
if MODE == 'learn' then
	local fh = assert(io.open(OUT, 'wb'))
	for _, l in ipairs(LEARNED) do
		local kind, name = l[1]:match('^([^\t]+)\t(.*)$')
		fh:write(kind, '\t', name, '\t', l[2], '\t', l[3], '\t', LABEL .. ':' .. l[4], '\n')
	end
	-- learn-mode harness errors still have to surface
	local nh = 0
	if main_out then nh = nh + #main_out.rec.harness end
	for _, c in ipairs(calls_out) do nh = nh + #c.rec.harness end
	fh:close()
	io.write(sformat('learned %d harness %d load %s\n', #LEARNED, nh, load_error or 'ok'))
	os.exit(load_error and 3 or 0)
end

local fh = assert(io.open(OUT, 'wb'))
local buf, nbuf = {}, 0
local function w(s)
	nbuf = nbuf + 1
	buf[nbuf] = s
	if nbuf >= 4096 then fh:write(table.concat(buf, '', 1, nbuf)); nbuf = 0 end
end
local function jstr(s)
	return '"' .. s:gsub('[%c"\\\128-\255]', function(c)
		if c == '"' then return '\\"' elseif c == '\\' then return '\\\\' end
		return sformat('\\u%04x', c:byte())
	end) .. '"'
end
local function jlist(l, perline)
	if #l == 0 then w('[]'); return end
	w('[')
	for i, s in ipairs(l) do
		if i > 1 then w(',') end
		if perline then w('\n  ') end
		w(jstr(s))
	end
	if perline then w('\n  ') end
	w(']')
end

local totals = { harness = 0, poison = 0, upvalue = 0, clobber = 0, budget = 0, reenter = 0, unsim = 0, truncated = 0 }
local function write_run(r, extra)
	local rc = r.rec
	w('{')
	if extra then extra() end
	w('"outcome":' .. jstr(r.outcome))
	w(',"fatal":' .. (rc.fatal and jstr(rc.fatal) or 'null'))
	w(',"poison":'); jlist(sorted_keys(rc.poison))
	w(',"upvalue":'); jlist(sorted_keys(rc.upvalue))
	w(',"clobber":'); jlist(sorted_keys(rc.clobber))
	w(',"harness":'); jlist(rc.harness)
	w(',"unsim":'); jlist(sorted_keys(rc.unsim))
	w(',"reenter":' .. rc.reenter)
	w(',"nevents":' .. r.nevents .. ',"truncated":' .. tostring(r.truncated))
	w(',"events":'); jlist(r.events, true)
	w('}')
	totals.harness = totals.harness + #rc.harness + (r.outcome:match('^HARNESS') and 1 or 0)
	totals.poison = totals.poison + #sorted_keys(rc.poison)
	totals.upvalue = totals.upvalue + #sorted_keys(rc.upvalue)
	totals.clobber = totals.clobber + #sorted_keys(rc.clobber)
	totals.budget = totals.budget + (r.outcome == 'BUDGET' and 1 or 0)
	totals.reenter = totals.reenter + rc.reenter
	totals.unsim = totals.unsim + #sorted_keys(rc.unsim)
	totals.truncated = totals.truncated + (r.truncated and 1 or 0)
end

w('{"label":' .. jstr(LABEL) .. ',"side":' .. jstr(SIDE) .. ',"chunk":' .. jstr(CHUNK:match('[^/\\]+$')))
w(',"seeds":' .. NSEEDS .. ',"types_h32":' .. jstr(sformat('%08x', types_h)))
w(',"load_error":' .. (load_error and jstr(load_error) or 'null'))
if main_out then
	w(',\n"restore":' .. jstr(restore_mode))
	local ws = {}
	for k in pairs(main_written) do ws[#ws + 1] = kstr(k) end
	table.sort(ws)
	w(',\n"main_writes":'); jlist(ws)
	local en = {}
	for _, ep in ipairs(main_out.entries) do
		local fi = funcinfo(ep.raw)
		en[#en + 1] = ep.name .. '/' .. ((fi and fi.params) or '?') .. (ep.nested and ' nested' or '')
	end
	w(',\n"entries":'); jlist(en)
	w(',\n"main":')
	write_run(main_out)
	w(',\n"calls":[')
	for i, c in ipairs(calls_out) do
		if i > 1 then w(',') end
		w('\n')
		write_run(c, function()
			w('"fn":' .. jstr(c.fn) .. ',"seed":' .. c.seed .. ',"arity":' .. c.arity .. ',"args":' .. jstr(c.args) .. ',')
		end)
	end
	w('\n]')
end
if load_error then totals.harness = totals.harness + 1 end
w(',\n"totals":{')
local tk = sorted_keys(totals)
for i, k in ipairs(tk) do w((i > 1 and ',' or '') .. jstr(k) .. ':' .. totals[k]) end
w('}}\n')
fh:write(table.concat(buf, '', 1, nbuf))
fh:close()
io.write(sformat('trace calls %d harness %d poison %d upvalue %d clobber %d budget %d reenter %d truncated %d load %s\n',
	#calls_out, totals.harness, totals.poison, totals.upvalue, totals.clobber, totals.budget,
	totals.reenter, totals.truncated, load_error or 'ok'))
os.exit(load_error and 3 or 0)
