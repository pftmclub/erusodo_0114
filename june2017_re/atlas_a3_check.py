#!/usr/bin/env python
"""Atlas/Elysion Phase A3 gate: every region id has the same value in the staged Lua and in the flag-resolved C++.

  python -I june2017_re/atlas_a3_check.py [--no-probe]

Lua  = the staged data036 Enum.lua / DungeonEnum.lua (luajit evaluates the real tables).
C++  = the enum bodies of the six headers after the preprocessor's flag resolution (region_census / buff_census: flags are
       probed with cl /P, so SERV_IRUHADEV_ELYSION_PORT is read as the compiler sees it in every project, not as the #define text).
Ids  = june2017_re/atlas_a3_gen.py's list (census ids.missing, minus the quest plan's two NUIs, plus WI_GUILD_AGIT / WI_SPA_RUBEN).
Prints one summary line per check; exit 1 on any mismatch, unresolved id, duplicate name, or duplicate value.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import region_census as RC
import buff_census as BC
import atlas_a3_gen as G

TRUNK = RC.TRUNK
T = lambda rel: os.path.join(TRUNK, rel.replace('/', os.sep))
CPP = {   # table -> [(header, enum name)]
    'WORLD_ID': [('X2Lib/X2World.h', 'WORLD_ID')],
    'VILLAGE_MAP_ID': [('KncWX2Server/Common/Enum/Enum.h', 'VILLAGE_MAP_ID')],
    'LOCAL_MAP_ID': [('KncWX2Server/Common/Enum/Enum.h', 'LOCAL_MAP_ID'), ('X2Lib/X2LocationManager.h', 'LOCAL_MAP_ID')],
    'HOUSE_ID': [('X2Lib/X2LocationManager.h', 'HOUSE_ID')],
    'DUNGEON_ID': [('KncWX2Server/Common/Enum/DungeonEnum.h', 'DUNGEON_ID')],
    'NPC_UNIT_ID': [('X2Lib/X2UnitManager.h', 'NPC_UNIT_ID'), ('KncWX2Server/Common/X2Data/XSLUnitManager.h', 'NPC_UNIT_ID')],
}
LUA = {'WORLD_ID': 'Enum.lua', 'VILLAGE_MAP_ID': 'Enum.lua', 'LOCAL_MAP_ID': 'Enum.lua', 'HOUSE_ID': 'Enum.lua',
       'NPC_UNIT_ID': 'Enum.lua', 'DUNGEON_ID': 'DungeonEnum.lua', 'LOCAL_MAP_UI_CUSTOM_MSG': 'Enum.lua'}
CPP_LMUCM = ('X2Lib/X2LocalMapUI.h', 'LOCAL_MAP_UI_CUSTOM_MSG')
# C++-only sentinels (no Lua twin) and Lua-only names are reported, not compared
MARK_LUA = {'WORLD_ID': ['WI_END'], 'VILLAGE_MAP_ID': ['VMI_BATTLE_FIELD_END'], 'DUNGEON_ID': ['DI_END']}
MARK_CPP = {'WORLD_ID': ['WI_END'], 'VILLAGE_MAP_ID': ['VMI_BATTLE_FIELD_REST_END', 'VMI_VILLAGE_MAP_END', 'VMI_BATTLE_FIELD_END'],
            'DUNGEON_ID': ['DI_END'], 'NPC_UNIT_ID': ['NUI_MAX_LIMIT']}


def main():
    probe = '--no-probe' not in sys.argv
    ids = G.load_ids()
    ids['LOCAL_MAP_UI_CUSTOM_MSG'] = list(G.LMUCM)
    heads = sorted({h for v in CPP.values() for h, _ in v} | {CPP_LMUCM[0]})
    RC.prime_flags([T(h) for h in heads], probe)
    en = {}
    for h in heads:
        text, unk = RC.active(T(h), probe)
        if unk:
            print('FLAG-UNRESOLVED in %s: %s' % (h, sorted(unk)))
        en[h] = BC.parse_cpp_enums(text)
    lua = {}
    for t, fn in LUA.items():
        d, e = RC.lj_enum(RC.kc('data036', fn), t)
        lua[t] = {k: int(v) for k, v in d.items() if re.match(r'^-?\d+$', v)}
        for x in e:
            print('LUA-ERROR', t, x)
    bad = 0
    unresolved = checked = 0
    for t, want in ids.items():
        cpp_defs = [(h, n) for h, n in (CPP.get(t) or [CPP_LMUCM])]
        for name, val in want:
            lv = lua[t].get(name)
            if lv != val:
                print('LUA  %s.%s: want %d, Lua %s' % (t, name, val, lv)); bad += 1; unresolved += 1
            for h, n in cpp_defs:
                cv = en[h].get(n, {}).get(name)
                if cv != val:
                    print('C++  %s.%s in %s: want %d, C++ %s' % (t, name, h, val, cv)); bad += 1; unresolved += 1
            checked += 1
    print('ids checked %d (both halves resolve: %s)' % (checked, 'YES' if not unresolved else 'NO, %d unresolved' % unresolved))
    # sentinels
    for t, ms in MARK_CPP.items():
        for m in ms:
            vals = {h: en[h].get(n, {}).get(m) for h, n in CPP[t]}
            lv = lua[t].get(m)
            print('sentinel %-28s C++ %s%s' % (m, vals, ('  Lua %s' % lv) if m in MARK_LUA.get(t, []) else ''))
            if m in MARK_LUA.get(t, []) and any(v != lv for v in vals.values()):
                print('  MISMATCH C++/Lua'); bad += 1
            want = G.MARKERS.get(m)
            if want is not None and any(v != want for v in vals.values()):
                print('  MISMATCH vs June %d' % want); bad += 1
    nm = en['X2Lib/X2UnitManager.h']['NPC_UNIT_ID']
    print('NUI_MAX_LIMIT %s (limit 65536: %s)' % (nm.get('NUI_MAX_LIMIT'), 'ok' if nm.get('NUI_MAX_LIMIT', 10 ** 9) <= 65536 else 'FAIL'))
    if nm.get('NUI_MAX_LIMIT', 10 ** 9) > 65536:
        bad += 1
    # both NUI enums agree on every shared name
    a, b = nm, en['KncWX2Server/Common/X2Data/XSLUnitManager.h']['NPC_UNIT_ID']
    diff = [(k, a[k], b[k]) for k in a if k in b and a[k] != b[k] and k != 'NUI_MAX_LIMIT']
    print('NUI client/server enums: %d shared names, %d differ %s' % (len([k for k in a if k in b]), len(diff), diff[:5]))
    # the A3 ids are not duplicated by name or value inside any table (Lua and each C++ enum)
    dups = 0
    for t, want in ids.items():
        names = {n for n, _ in want}
        views = [('Lua', lua[t])] + [(h, en[h][n]) for h, n in (CPP.get(t) or [CPP_LMUCM])]
        for label, tab in views:
            byv = collections.defaultdict(list)
            for k, v in tab.items():
                byv[v].append(k)
            for v, ks in byv.items():
                if len(ks) > 1 and names & set(ks):
                    print('DUP-VALUE %s %s = %s : %s' % (label, t, v, ks)); dups += 1
    print('duplicate values touching an A3 id: %d' % dups)
    bad += dups
    # Lua names the C++ lacks, for the tables A3 mirrors (informational: Lua-only adoptions by earlier phases)
    for t in ('WORLD_ID',):
        lonly = sorted(set(lua[t]) - set(en['X2Lib/X2World.h']['WORLD_ID']) - set(MARK_LUA.get(t, [])))
        print('Lua-only %s names (no C++ name; earlier phases, not A3): %d' % (t, len(lonly)))
    print('A3 CHECK %s' % ('PASS' if not bad else 'FAIL (%d)' % bad))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
