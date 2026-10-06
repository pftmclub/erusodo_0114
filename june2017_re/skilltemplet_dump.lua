--[[ Effect C1a: run a NewSkillTempletVer2.lua (March 0x81 bytecode, or June converted with ljconv.py)
against a recording g_pSkillTree stub and write one line per call:
    SK <tab> m_eID <tab> key \1 serialized-value \2 ...     AddSkillTemplet_LUA
    TR / GT <tab> serialized table                           AddSkillTreeTemplet_LUA / AddGuildSkillTreeTemplet_LUA
    OT <tab> method <tab> serialized arg                     anything else (June: OpenScriptFilePostProcess)
Enum tables are recording proxies (SKILL_POWER_RATE_TYPE.X reads as the string "SKILL_POWER_RATE_TYPE.X").

    elstools/vendor/luajit/luajit.exe june2017_re/skilltemplet_dump.lua OUT.tsv FILE.lua
Run on bytecode, not on the decompiled .src.lua: ljd prints long strings that contain "]]" unescaped.
]]
-- usage: luajit skdump.lua OUT FILE ; writes one line per AddSkillTemplet_LUA: serialized table
local out = assert(io.open(arg[1], "wb"))
local function ser(v)
  local t = type(v)
  if t == "table" then
    local keys = {}
    for k in pairs(v) do keys[#keys+1] = k end
    table.sort(keys, function(a,b) return tostring(a) < tostring(b) end)
    local parts = {}
    for _,k in ipairs(keys) do parts[#parts+1] = tostring(k) .. "=" .. ser(v[k]) end
    return "{" .. table.concat(parts, ",") .. "}"
  elseif t == "string" then return string.format("%q", v):gsub("\n","n")
  else return tostring(v) end
end
local counts = {}
local stub = {}
setmetatable(stub, {__index = function(_, m)
  return function(self, tbl, ...)
    counts[m] = (counts[m] or 0) + 1
    if m == "AddSkillTemplet_LUA" then
      out:write("SK\t", tostring(tbl.m_eID), "\t")
      for k, v in pairs(tbl) do end
      local keys = {}
      for k in pairs(tbl) do keys[#keys+1] = k end
      table.sort(keys, function(a,b) return tostring(a) < tostring(b) end)
      for _,k in ipairs(keys) do out:write(tostring(k), "\1", ser(tbl[k]), "\2") end
      out:write("\n")
    elseif m == "AddSkillTreeTemplet_LUA" or m == "AddGuildSkillTreeTemplet_LUA" then
      out:write(m == "AddSkillTreeTemplet_LUA" and "TR\t" or "GT\t", ser(tbl), "\n")
    else
      out:write("OT\t", m, "\t", ser(tbl), "\n")
    end
  end
end})
g_pSkillTree = stub
True = 1; False = 0; TRUE = 1; FALSE = 0
local seen = {} ; setmetatable(_G, {__index = function(_, k) if not seen[k] then seen[k] = true; io.stderr:write("enum-proxy ", tostring(k), "\n") end; return setmetatable({}, {__index = function(_, m) return tostring(k) .. "." .. tostring(m) end}) end})
local f = assert(loadfile(arg[2]))
local ok, err = pcall(f)
io.stderr:write("ok=", tostring(ok), " err=", tostring(err), "\n")
for k,v in pairs(counts) do io.stderr:write(k, "=", v, "\n") end
out:close()
