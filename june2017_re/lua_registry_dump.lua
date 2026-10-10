-- June effect C3z-2 (2026-10-10): run a registry .lua (BOM source or KOG 0x81 bytecode; this is the KOG luajit) in a stub
-- environment and print what it registers. Driven by june2017_re/dungeon_npc_census.py.
--   luajit lua_registry_dump.lua enum  FILE TABLE   -> "K <name> <value>" for every pair of global TABLE after the run
--   luajit lua_registry_dump.lua calls FILE         -> "C <method> <key>=<value> ..." (tab-separated) per g_pX:Method({...}) call
-- Unknown globals resolve to markers: NPC_UNIT_ID.X -> "NUI:X", OTHER.X -> "E:OTHER.X"; a nested table field prints as "{}".
local mode, path, tname = arg[1], arg[2], arg[3]
local function enumproxy(n)
  return setmetatable({}, { __index = function(_, k)
    if n == "NPC_UNIT_ID" then return "NUI:" .. tostring(k) end
    return "E:" .. n .. "." .. tostring(k)
  end })
end
local function flat(v)
  if type(v) == "table" then return "{}" end
  return (tostring(v):gsub("[\t\r\n]", " "))
end
local function caller(n)
  return setmetatable({}, { __index = function(_, m)
    return function(_, t)
      local out = { "C", m }
      if type(t) == "table" then
        for k, v in pairs(t) do out[#out + 1] = tostring(k) .. "=" .. flat(v) end
      else
        out[#out + 1] = "arg=" .. flat(t)
      end
      print(table.concat(out, "\t"))
    end
  end })
end
local env = setmetatable({}, { __index = function(t, k)
  local g = _G[k]
  if g ~= nil and k ~= "arg" then return g end
  local p
  if mode == "calls" and type(k) == "string" and k:sub(1, 2) == "g_" then p = caller(k) else p = enumproxy(k) end
  rawset(t, k, p); return p
end })
local f, err = loadfile(path)
if not f then print("E\t" .. tostring(err)); return end
setfenv(f, env)
local ok, rerr = pcall(f)
if not ok then print("E\t" .. tostring(rerr)) end
if mode == "enum" then
  local t = rawget(env, tname)
  if type(t) ~= "table" then print("E\tno table " .. tostring(tname)); return end
  for k, v in pairs(t) do print("K\t" .. tostring(k) .. "\t" .. tostring(v)) end
end
