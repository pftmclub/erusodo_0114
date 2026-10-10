-- June effect C3z-3 (2026-10-10): run DungeonData.lua (BOM source or KOG 0x81 bytecode; this is the KOG luajit) in a stub
-- environment and print every AddDungeonData / AddDefaultRoomTitle_LUA call with its table serialized in full (nested tables
-- included, keys sorted). Driven by june2017_re/dungeon_registry_census.py.
--   luajit dungeon_registry_dump.lua FILE   -> "R <index> <method> <serialized table>" per call, then "F <index> <key> <value>"
--                                              per top-level field of a table argument; "E <error>" on a failure
-- Unknown globals resolve to markers: STR_ID_123 -> "@STR_ID_123", DUNGEON_ID.X -> "@DUNGEON_ID.X", OTHER.X -> "@OTHER.X".
local path = arg[1]
local marks = setmetatable({}, { __mode = "k" })
local function marker(n)
  local p = setmetatable({}, { __index = function(_, k) return "@" .. n .. "." .. tostring(k) end })
  marks[p] = "@" .. n
  return p
end
local function ser(v)
  local t = type(v)
  if t == "table" then
    if marks[v] then return marks[v] end
    local keys = {}
    for k in pairs(v) do keys[#keys + 1] = k end
    table.sort(keys, function(a, b)
      if type(a) == type(b) then return a < b end
      return type(a) == "number"
    end)
    local out = {}
    for _, k in ipairs(keys) do out[#out + 1] = tostring(k) .. "=" .. ser(v[k]) end
    return "{" .. table.concat(out, ",") .. "}"
  elseif t == "string" then
    return string.format("%q", v):gsub("\r", "\\r"):gsub("\n", "\\n")
  end
  return tostring(v)
end
local n = 0
local mgr = setmetatable({}, { __index = function(_, m)
  return function(_, t)
    n = n + 1
    print("R\t" .. n .. "\t" .. m .. "\t" .. ser(t))
    if type(t) == "table" and not marks[t] then
      for k, v in pairs(t) do print("F\t" .. n .. "\t" .. tostring(k) .. "\t" .. ser(v)) end
    end
  end
end })
local env = setmetatable({}, { __index = function(t, k)
  if k == "g_pDungeonManager" then return mgr end
  local g = _G[k]
  if g ~= nil and k ~= "arg" then return g end
  local p = marker(k)
  rawset(t, k, p); return p
end })
local f, err = loadfile(path)
if not f then print("E\t" .. tostring(err)); return end
setfenv(f, env)
local ok, rerr = pcall(f)
if not ok then print("E\t" .. tostring(rerr)) end
