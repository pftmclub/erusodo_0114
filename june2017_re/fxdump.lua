--[[ Phase 10b harness: execute effect lua (mar-2014 bytecode or source) against
stub engine objects and dump everything it defines, as JSON.

    luajit fxdump.lua OUT.json FILE [FILE ...]

Run with the vendored KOG LuaJIT (elstools/vendor/luajit/luajit.exe), which
loads 1b 4b 4c 81 natively. Do NOT feed it June 0x82 files: its 0x82 opcode
table has the same 0x2f-0x37 swap as elstools (see README). Convert with
ljconv.py first.

All FILEs run in order in ONE shared environment, as the client runs them in
one lua_State: the later DamageEffect_* files deepcopy templets defined by
earlier ones, and every file indexes Enum.lua's tables. Pass Enum.lua first.

The environment fails loudly rather than guessing:
  - only an explicit allowlist of Lua builtins, three engine stubs
    (g_pEffectSet, g_pDamageManager, g_pUnitManager), D3DXVECTOR3 and the
    INIT_PHYSIC probe exist (see below);
  - a stub method that is not listed raises;
  - any other global read returns nil and is recorded under "missing_reads"
    (a nil enum table then raises at the index, which is recorded as the
    file's error and ends that file, as it would in the client).

Output (strings: every byte >= 0x80 is written as \u00XX, so decode JSON
strings as latin-1 to get the original bytes back):
  files         [{file, ok, error}]
  calls         [{file, recv, method, args}]   one per stub call, args serialised at call time
  globals       {name: {file, redefined:[file...], value}}   final values
  missing_reads {name: {file: count}}
]]

local out_path = arg[1]
local files = {}
for i = 2, #arg do files[#files + 1] = arg[i] end
assert(out_path and #files > 0, 'usage: luajit fxdump.lua OUT.json FILE [FILE ...]')

local cur_file = '?'
local calls, missing, defs, store = {}, {}, {}, {}
local file_results = {}

-- Serialise a value into a Lua table tree the JSON writer understands. Tables
-- are copied at call time; cycles become "<cycle>".
local function snap(v, seen)
  local t = type(v)
  if t ~= 'table' then
    if t == 'function' then return {['$fn'] = true} end
    if t == 'userdata' or t == 'thread' then return {['$' .. t] = true} end
    return v
  end
  seen = seen or {}
  if seen[v] then return {['$cycle'] = true} end
  seen[v] = true
  local r = {['$t'] = {}}
  for k, x in pairs(v) do
    local key
    if type(k) == 'string' then key = k
    elseif type(k) == 'number' then key = string.format('[%.17g]', k)
    else key = '[' .. type(k) .. ']' end
    r['$t'][key] = snap(x, seen)
  end
  seen[v] = nil
  return r
end

local function stub(recv, methods)
  local o = {}
  for _, m in ipairs(methods) do
    o[m] = function(self, ...)
      local args = {}
      for i = 1, select('#', ...) do args[i] = snap((select(i, ...))) end
      calls[#calls + 1] = {file = cur_file, recv = recv, method = m, args = args, n = select('#', ...)}
    end
  end
  return setmetatable(o, {__index = function(_, k)
    error(string.format('stub %s has no method %s (add it only if the client binds it)', recv, tostring(k)), 2)
  end})
end

-- Things the client provides that are not effect data:
--   D3DXVECTOR3  - lua_tinker class (KTDXApp.cpp:1539); recorded as its three numbers.
--   INIT_PHYSIC  - NOT bound by the engine: every character script assigns it as a
--                  global, so an effect file that reads it sees whichever unit script
--                  ran last. Probed: each field read is recorded and yields NaN, so a
--                  value computed from it is visibly not real in the dump.
local probes = {}
local function probe(name)
  return setmetatable({}, {__index = function(_, k)
    local key = name .. '.' .. tostring(k)
    local m = probes[key] or {}
    probes[key] = m
    m[cur_file] = (m[cur_file] or 0) + 1
    return 0 / 0
  end})
end

local allow = {
  D3DXVECTOR3 = function(x, y, z) return {['$D3DXVECTOR3'] = true, x = x, y = y, z = z} end,
  INIT_PHYSIC = probe('INIT_PHYSIC'),
  pairs = pairs, ipairs = ipairs, next = next, type = type, tostring = tostring,
  tonumber = tonumber, select = select, unpack = unpack, setmetatable = setmetatable,
  getmetatable = getmetatable, rawget = rawget, rawset = rawset, rawequal = rawequal,
  error = error, assert = assert, pcall = pcall, string = string, table = table, math = math,
  g_pEffectSet = stub('g_pEffectSet', {'AddEffectSetTemplet_LUA', 'AddEffectSet_Description', 'SetEffectSetVersion'}),
  g_pDamageManager = stub('g_pDamageManager', {'InsertDamageData_LUA'}),
  g_pUnitManager = stub('g_pUnitManager', {'AddUnitViewerEffectSet'}),
}

-- The raw env stays empty so every global write goes through __newindex.
local env = setmetatable({}, {
  __index = function(_, k)
    local v = store[k]
    if v ~= nil then return v end
    v = allow[k]
    if v ~= nil then return v end
    local m = missing[k] or {}
    missing[k] = m
    m[cur_file] = (m[cur_file] or 0) + 1
    return nil
  end,
  __newindex = function(_, k, v)
    local d = defs[k]
    if d == nil then defs[k] = {file = cur_file, redefined = {}}
    else d.redefined[#d.redefined + 1] = cur_file end
    store[k] = v
  end,
})

for _, f in ipairs(files) do
  cur_file = f:match('[^/\\]+$')
  local chunk, err = loadfile(f)
  local res = {file = cur_file, ok = false}
  if not chunk then
    res.error = 'load: ' .. tostring(err)
  else
    setfenv(chunk, env)
    local ok, e = pcall(chunk)
    res.ok = ok
    if not ok then res.error = tostring(e) end
  end
  file_results[#file_results + 1] = res
  io.stderr:write(string.format('%s: %s\n', cur_file, res.ok and 'ok' or res.error))
end

---------------------------------------------------------------- JSON writer
local fh = assert(io.open(out_path, 'wb'))
local buf, nbuf = {}, 0
local function w(s)
  nbuf = nbuf + 1
  buf[nbuf] = s
  if nbuf >= 4096 then fh:write(table.concat(buf, '', 1, nbuf)); nbuf = 0 end
end
local function jstr(s)
  return '"' .. s:gsub('[%c"\\\128-\255]', function(c)
    if c == '"' then return '\\"' elseif c == '\\' then return '\\\\' end
    return string.format('\\u%04x', c:byte())
  end) .. '"'
end
local function jnum(n)
  if n ~= n then return '{"$num":"nan"}' end
  if n == math.huge then return '{"$num":"inf"}' end
  if n == -math.huge then return '{"$num":"-inf"}' end
  if n == math.floor(n) and math.abs(n) < 2^53 then return string.format('%d', n) end
  return string.format('%.17g', n)
end
local function jval(v)
  local t = type(v)
  if t == 'string' then w(jstr(v))
  elseif t == 'number' then w(jnum(v))
  elseif t == 'boolean' then w(v and 'true' or 'false')
  elseif t == 'nil' then w('null')
  elseif t == 'table' then
    -- Arrays built by this script are written as arrays; everything else as objects.
    if v[1] ~= nil or next(v) == nil and getmetatable(v) == 'array' then
      w('[')
      for i = 1, #v do if i > 1 then w(',') end jval(v[i]) end
      w(']')
    else
      local keys = {}
      for k in pairs(v) do keys[#keys + 1] = tostring(k) end
      table.sort(keys)
      w('{')
      for i, k in ipairs(keys) do
        if i > 1 then w(',') end
        w(jstr(k)); w(':'); jval(v[k])
      end
      w('}')
    end
  else w(jstr('<' .. t .. '>')) end
end

w('{"files":'); jval(file_results)
w(',"calls":[')
for i, c in ipairs(calls) do
  if i > 1 then w(',') end
  w('{"file":'); w(jstr(c.file)); w(',"recv":'); w(jstr(c.recv)); w(',"method":'); w(jstr(c.method))
  w(',"args":[')
  for j = 1, c.n do if j > 1 then w(',') end jval(c.args[j]) end
  w(']}')
end
w('],"globals":{')
local names = {}
for k in pairs(defs) do names[#names + 1] = k end
table.sort(names)
for i, k in ipairs(names) do
  if i > 1 then w(',') end
  w(jstr(k)); w(':{"file":'); w(jstr(defs[k].file)); w(',"redefined":[')
  for j, f in ipairs(defs[k].redefined) do if j > 1 then w(',') end w(jstr(f)) end
  w('],"value":'); jval(snap(store[k])); w('}')
end
w('},"missing_reads":'); jval(missing)
w(',"probe_reads":'); jval(probes)
w('}')
fh:write(table.concat(buf, '', 1, nbuf))
fh:close()
