-- June effect C3z-2 (2026-10-10): run one dungeon script (stock-header LuaJIT bytecode or source) in a stub environment and print
-- its NPC entries the way the client's loaders walk them (X2Dungeon.cpp STAGE_LIST / STAGE<n>, X2DungeonStage.cpp SUB_STAGE<n>,
-- X2DungeonSubStage.cpp LoadNPCData / New_LoadNPCData / ParseNpcGroupTable: NPC_GROUP[1..], SUB_NPC<i>, NPC_GROUP_RATE + NPC_GROUP<N>).
-- Every global the script reads resolves to a marker: NPC_UNIT_ID.X -> "NUI:X", OTHER.X -> "E:OTHER.X". Driven by
-- june2017_re/dungeon_npc_census.py; prints tab-separated lines on stdout:
--   N <stage> <sub> <form> <group> <index> <subnpc> <npc_id>     form = plain | rate ; subnpc = - or the SUB_NPC index
--   K <stage> <sub> <cond> <index> <npc_id>                      CLEAR_COND<cond>.NPC_ID[index] (CC_KILL_NPC_TYPE)
--   R <name>                                                     every NPC_UNIT_ID name the main chunk reads
--   E <message>                                                  load or run error
local path = arg[1]
local read = {}
local function enumproxy(tname)
  return setmetatable({}, { __index = function(_, k)
    if tname == "NPC_UNIT_ID" then read[k] = true; return "NUI:" .. tostring(k) end
    return "E:" .. tname .. "." .. tostring(k)
  end })
end
local env = setmetatable({}, { __index = function(t, k)
  local g = _G[k]
  if g ~= nil and k ~= "arg" then return g end
  local p = enumproxy(k); rawset(t, k, p); return p
end })
local f, err = loadfile(path)
if not f then print("E\t" .. tostring(err)); return end
setfenv(f, env)
local ok, rerr = pcall(f)
if not ok then print("E\t" .. tostring(rerr)) end

local function idof(t)
  local v = rawget(t, "NPC_ID")
  if v == nil then return "nil" end
  if type(v) == "table" then return "table" end
  return tostring(v)
end
local function group(st, sb, form, g, tbl)
  local i = 1
  while type(tbl[i]) == "table" do
    local e, any, j = tbl[i], false, 0
    while type(e["SUB_NPC" .. j]) == "table" do
      print(table.concat({ "N", st, sb, form, g, i, j, idof(e["SUB_NPC" .. j]) }, "\t")); any = true; j = j + 1
    end
    if not any then print(table.concat({ "N", st, sb, form, g, i, "-", idof(e) }, "\t")) end
    i = i + 1
  end
end
local SL = rawget(env, "STAGE_LIST")
if type(SL) == "table" then
  local s = 0
  while type(SL["STAGE" .. s]) == "table" do
    local stage, b = SL["STAGE" .. s], 0
    while type(stage["SUB_STAGE" .. b]) == "table" do
      local sub = stage["SUB_STAGE" .. b]
      if type(sub.NPC_GROUP_RATE) == "table" then
        local g = 0
        while type(sub["NPC_GROUP" .. g]) == "table" do group(s, b, "rate", g, sub["NPC_GROUP" .. g]); g = g + 1 end
      elseif type(sub.NPC_GROUP) == "table" then
        group(s, b, "plain", 0, sub.NPC_GROUP)
      end
      -- CLEAR_COND<c>.NPC_ID (CC_KILL_NPC_TYPE): the client reads [1..] until the first nil, so a nil id ends the list
      local c = 0
      while type(sub["CLEAR_COND" .. c]) == "table" do
        local ids = sub["CLEAR_COND" .. c].NPC_ID
        if type(ids) == "table" then
          local i = 1
          while ids[i] ~= nil do print(table.concat({ "K", s, b, c, i, tostring(ids[i]) }, "\t")); i = i + 1 end
        end
        c = c + 1
      end
      print(table.concat({ "B", s, b }, "\t"))
      b = b + 1
    end
    s = s + 1
  end
end
for k in pairs(read) do print("R\t" .. tostring(k)) end
