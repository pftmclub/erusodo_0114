-- June effect C3z-3 (2026-10-10): strict load test for March's DungeonData.lua. Loads March's real enum files into one
-- global table (the client loads them into the same Lua state before DungeonData.lua), then runs DungeonData.lua with only
-- g_pDungeonManager stubbed. Any unknown global, or a missing member of an enum table, raises: in the client that stops the
-- script and every later row is never registered.
--   luajit dungeon_registry_strict.lua DUNGEONDATA ENUM [ENUM ...]
-- Prints "OK <rows> <titles>" or "FAIL <message>" lines; a FAIL line per row whose dungeonID is not a number or whose
-- EXTRA_REQUIRE_DUNGEON_ID list has a hole (AddDungeonData_LUA reads it with GetValue(i) and stops at the first nil).
local dd = arg[1]
local env = {}
setmetatable(env, { __index = _G })
for i = 2, #arg do
  local f = assert(loadfile(arg[i]))
  setfenv(f, env)
  local ok, err = pcall(f)
  if not ok then print("FAIL enum " .. arg[i] .. ": " .. tostring(err)); return end
end
-- every table of numbers is an enum: missing member -> error
for k, t in pairs(env) do
  if type(t) == "table" and getmetatable(t) == nil then
    local num = true
    for _, v in pairs(t) do if type(v) ~= "number" then num = false; break end end
    if num then
      setmetatable(t, { __index = function(_, m) error("missing enum member " .. k .. "." .. tostring(m), 2) end })
    end
  end
end
local rows, titles, fails, seen = 0, 0, 0, {}
local mgr = {
  AddDefaultRoomTitle_LUA = function(_, s)
    if type(s) ~= "number" then print("FAIL title not a number"); fails = fails + 1 end
    titles = titles + 1
  end,
  AddDungeonData = function(_, t)
    rows = rows + 1
    if type(t.dungeonID) ~= "number" then print("FAIL row " .. rows .. ": dungeonID " .. tostring(t.dungeonID)); fails = fails + 1; return end
    if type(t.dungeonName) ~= "number" then print("FAIL row " .. rows .. ": dungeonName"); fails = fails + 1 end
    if type(t.dataFileName) ~= "string" then print("FAIL row " .. rows .. ": dataFileName"); fails = fails + 1 end
    if seen[t.dungeonID] then print("FAIL row " .. rows .. ": duplicate dungeonID " .. t.dungeonID .. " (row " .. seen[t.dungeonID] .. ")"); fails = fails + 1 end
    seen[t.dungeonID] = rows
    local e = t.EXTRA_REQUIRE_DUNGEON_ID
    if e ~= nil then
      local n = 0
      for _ in pairs(e) do n = n + 1 end
      if n ~= #e then print("FAIL row " .. rows .. ": EXTRA_REQUIRE_DUNGEON_ID has a hole"); fails = fails + 1 end
    end
  end,
}
local strict = setmetatable({ g_pDungeonManager = mgr }, { __index = function(_, k)
  local v = env[k]
  if v == nil then error("unknown global " .. tostring(k), 2) end
  return v
end, __newindex = function(t, k, v) rawset(t, k, v) end })
local f, err = loadfile(dd)
if not f then print("FAIL load: " .. tostring(err)); return end
setfenv(f, strict)
local ok, rerr = pcall(f)
if not ok then print("FAIL run (stopped after row " .. rows .. "): " .. tostring(rerr)); return end
print(string.format("OK %d rows, %d titles, %d fail(s)", rows, titles, fails))
