#!/usr/bin/env python
"""Atlas/Elysion Phase A3 generator: region enums, Lua + C++ (both halves).

Reads june2017_re/atlas_a3_ids.tsv (the census's `ids.missing` rows before this phase, June's values) and writes
  1. the Lua rows into the staged data036 Enum.lua / DungeonEnum.lua (BOM + CRLF text, edited in place),
  2. a bytepatch spec for the C++ mirrors under SERV_IRUHADEV_ELYSION_PORT.

  python -I june2017_re/atlas_a3_gen.py lua  [--dry-run]     # edit the staged Lua
  python -I june2017_re/atlas_a3_gen.py spec OUT.txt         # write the bytepatch spec

Nothing here invents a value: every id is a census row (June), the extras are June's own
(WI_GUILD_AGIT 623, WI_SPA_RUBEN 725, LMUCM 18006-18008 and the three Lua tables, read from
june_src_fixed/data036_kom/Enum.src.lua).  NUI_STORY_QUEST / NUI_TRANSCEND_QUEST are the quest
plan's (user, 2026-10-16) and are excluded.
"""
import csv, os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(TRUNK, 'tools'))
IDS_TSV = os.path.join(HERE, 'atlas_a3_ids.tsv')
KC = os.environ.get('X2_KOM_CONTENTS') or r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents'
STAGE = os.path.join(KC, 'data036_kom')
JUNE_ENUM = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom', 'Enum.src.lua')
FLAG = 'SERV_IRUHADEV_ELYSION_PORT'
QUEST_DEFERRED = {'NUI_STORY_QUEST', 'NUI_TRANSCEND_QUEST'}
EXTRA_WI = [('WI_GUILD_AGIT', 623), ('WI_SPA_RUBEN', 725)]       # June Enum.src.lua:2354, :2482 (A1b row B91)
LMUCM = [('LMUCM_LOCAL_MAP_MOVE_UP', 18006), ('LMUCM_LOCAL_MAP_MOVE_DOWN', 18007), ('LMUCM_CLOSE_GUIDE_WINDOW', 18008)]
# June's C++ markers (A1a, region_reach.tsv): explicit values on the C++ side, the same numbers in the Lua sentinels.
MARKERS = {'VMI_BATTLE_FIELD_REST_END': 1009, 'VMI_VILLAGE_MAP_END': 20011, 'VMI_BATTLE_FIELD_END': 40045,
           'WI_END': 779, 'DI_END': 41530}
HDR = 'Iruha : 2026-10-17 // Atlas/Elysion port Phase A3 -- '


def load_ids():
    """-> {table: [(name, value)] sorted by value}.  Read from atlas_a3_ids.tsv, the snapshot of the census's `ids.missing` rows
    (june2017_re/region_census.tsv before this phase), minus the quest plan's two NUIs, plus the extras above.  The census itself
    no longer lists them once A3 is applied, so the snapshot is what makes this script and atlas_a3_check.py repeatable."""
    out = collections.defaultdict(list)
    for r in csv.reader(open(IDS_TSV, encoding='utf-8'), delimiter='	'):
        if r[0] == 'table':
            continue
        if r[1] in QUEST_DEFERRED:
            continue
        out[r[0]].append((r[1], int(r[2])))
    for n, v in EXTRA_WI:
        if (n, v) not in out['WORLD_ID']:
            out['WORLD_ID'].append((n, v))
    return {t: sorted(set(v), key=lambda x: (x[1], x[0])) for t, v in out.items()}


def june_table(name):
    """Body lines (name, value) of one June Enum.src.lua table."""
    lines = open(JUNE_ENUM, 'rb').read().decode('utf-8-sig').split('\n')
    i = next(k for k, l in enumerate(lines) if l.startswith(name + ' = {'))
    rows = []
    for l in lines[i + 1:]:
        if l.startswith('}'):
            break
        m = re.match(r'\s*(\w+)\s*=\s*(-?\d+),?\s*$', l)
        rows.append((m.group(1), int(m.group(2))))
    return rows


# ----------------------------------------------------------------------------------------------------- Lua
def lua_edit_table(lines, table, entries, comment, replace=None):
    """lines: list of bytes lines (no EOL). Insert entries before the table's closing brace."""
    i = next(k for k, l in enumerate(lines) if l.startswith(table.encode() + b' = {'))
    j = next(k for k in range(i + 1, len(lines)) if lines[k].startswith(b'}'))
    body = lines[i + 1:j]
    have = {}
    for l in body:
        m = re.match(rb'\s*(\w+)\s*=\s*(-?\d+)', l)
        if m:
            have[m.group(1).decode()] = int(m.group(2))
    for n, v in entries:
        if n in have:
            raise SystemExit('DUPLICATE in %s: %s (have %s)' % (table, n, have[n]))
    # trailing comma on the last real row
    k = j - 1
    while k > i and (not lines[k].strip() or lines[k].lstrip().startswith(b'--')):
        k -= 1
    if k > i and not lines[k].rstrip().endswith(b','):
        lines[k] = lines[k].rstrip() + b','
    add = [b'\t-- ' + c.encode() for c in comment]
    add += [('\t%s = %d,' % (n, v)).encode() for n, v in entries]
    lines[j:j] = add
    return len(entries)


def sub_line(lines, old, new):
    hits = [k for k, l in enumerate(lines) if l.rstrip() == old]
    if len(hits) != 1:
        raise SystemExit('anchor not unique (%d): %r' % (len(hits), old))
    lines[hits[0]] = new


def split_crlf(b):
    assert b.startswith(b'\xef\xbb\xbf')
    t = b[3:]
    assert b'\r\n' in t and t.count(b'\n') == t.count(b'\r\n'), 'mixed EOL'
    return t.split(b'\r\n')


def do_lua(dry):
    ids = load_ids()
    jt = {t: june_table(t) for t in ('LINE_MOVE_TYPE', 'DRAMATIC_MESSAGE_TYPE', 'VILLAGENPC_RELATIONDATA_TYPE')}
    p = os.path.join(STAGE, 'Enum.lua')
    L = split_crlf(open(p, 'rb').read())
    cmt = lambda what: [HDR + what + ' (June values, june2017_re/region_census.tsv ids.missing)']
    cnt = {}
    cnt['WORLD_ID'] = lua_edit_table(L, 'WORLD_ID', ids['WORLD_ID'], cmt('Atlas/Elysion worlds, WI_GUILD_AGIT, WI_SPA_RUBEN'))
    cnt['VILLAGE_MAP_ID'] = lua_edit_table(L, 'VILLAGE_MAP_ID', ids['VILLAGE_MAP_ID'], cmt('Atlas Station, Elysion village, lounge, Fields 01-04'))
    cnt['LOCAL_MAP_ID'] = lua_edit_table(L, 'LOCAL_MAP_ID', ids['LOCAL_MAP_ID'], cmt('LMI_ELYSION'))
    cnt['HOUSE_ID'] = lua_edit_table(L, 'HOUSE_ID', ids['HOUSE_ID'], cmt('Atlas/Elysion houses'))
    cnt['NPC_UNIT_ID'] = lua_edit_table(L, 'NPC_UNIT_ID', ids['NPC_UNIT_ID'],
                                        cmt('Atlas/Elysion NPCs (NUI_STORY_QUEST 10142 / NUI_TRANSCEND_QUEST 10153 stay with the quest plan)'))
    cnt['LOCAL_MAP_UI_CUSTOM_MSG'] = lua_edit_table(L, 'LOCAL_MAP_UI_CUSTOM_MSG', LMUCM, cmt('LMUCM 18006-18008'))
    cnt['TRIGGER_LINE_ACTION_TYPE'] = lua_edit_table(L, 'TRIGGER_LINE_ACTION_TYPE', [('TAT_LINE_CHANGE_MOVABLE_DATA', 8)],
                                                     cmt('June Enum.src.lua:1983; C++ comes with E5'))
    # sentinels
    sub_line(L, b'\tWI_END = 573,', ('\tWI_END = %d,' % MARKERS['WI_END']).encode())
    sub_line(L, b'\tVMI_BATTLE_FIELD_END = 40028,', ('\tVMI_BATTLE_FIELD_END = %d,' % MARKERS['VMI_BATTLE_FIELD_END']).encode())
    # stale "omitted" comments
    sub_line(L, b'\t-- 10009/10010 (LMI_ELYSION/LMI_EVENT) are June-only content and stay omitted.',
             b'\t-- 10009 (LMI_ELYSION) was added by Atlas A3 (below); 10010 (LMI_EVENT) is June-only content and stays omitted.')
    sub_line(L, b'\t-- LMUCM_CLOSE_GUIDE_WINDOW) are deliberately omitted: Elysion-only, no March user.',
             b'\t-- LMUCM_CLOSE_GUIDE_WINDOW) were omitted here until Atlas A3 added them (below), for the Elysion local-map page.')
    sub_line(L, b'\t-- June\'s TAT_LINE_CHANGE_MOVABLE_DATA = 8 is not here: March has no C++ for it.',
             b'\t-- June\'s TAT_LINE_CHANGE_MOVABLE_DATA = 8 was added by Atlas A3 (below); no March C++ reads it until E5.')
    # new tables (June verbatim), before the closing of the file: after LOCAL_MAP_UI_CUSTOM_MSG's table
    new = []
    for t in ('LINE_MOVE_TYPE', 'DRAMATIC_MESSAGE_TYPE', 'VILLAGENPC_RELATIONDATA_TYPE'):
        if any(l.startswith(t.encode() + b' = {') for l in L):
            raise SystemExit('table exists: ' + t)
        new.append(('-- ' + HDR + 'new table %s, June values (Enum.src.lua); C++ with %s').encode() % (t.encode(), b'E5' if t.startswith('LINE') else b'E6' if t.startswith('DRAM') else b'E1'))
        new.append((t + ' = {').encode())
        rows = jt[t]
        for k, (n, v) in enumerate(rows):
            new.append(('\t%s = %d%s' % (n, v, ',' if k < len(rows) - 1 else '')).encode())
        new.append(b'}')
        cnt[t] = len(rows)
    at = next(k for k, l in enumerate(L) if l.startswith(b'UI_PERSONAL_TRADE_CUSTOM_MSG = {'))
    L[at:at] = new
    out = b'\xef\xbb\xbf' + b'\r\n'.join(L)
    # DungeonEnum.lua
    p2 = os.path.join(STAGE, 'DungeonEnum.lua')
    D = split_crlf(open(p2, 'rb').read())
    cnt['DUNGEON_ID'] = lua_edit_table(D, 'DUNGEON_ID', ids['DUNGEON_ID'], cmt('Atlas/Elysion dungeons (Hero rows are june-dungeons C3y-5)'))
    out2 = b'\xef\xbb\xbf' + b'\r\n'.join(D)
    print('Lua rows added:', dict(cnt))
    if dry:
        print('dry-run: nothing written')
        return
    open(p, 'wb').write(out)
    open(p2, 'wb').write(out2)
    print('wrote', p, len(out), p2, len(out2))


# ----------------------------------------------------------------------------------------------------- C++ spec
def spec_entry(op, anchor, text):
    return '@@ %s\n@@ anchor\n%s\n@@ text\n%s\n@@ end\n' % (op, anchor, text)


def block(entries, indent, width=None):
    return ''.join('%s%s = %d,\n' % (indent, n, v) for n, v in entries).rstrip('\n')


def do_spec(out_path):
    ids = load_ids()
    S = []
    note = lambda what: '//{{ %s%s' % (HDR, what)
    tabs2, tabs3 = '\t\t', '\t\t\t'

    def split_vmi(lo, hi):
        return [(n, v) for n, v in ids['VILLAGE_MAP_ID'] if lo <= v < hi]

    # ---- KncWX2Server/Common/Enum/Enum.h : VILLAGE_MAP_ID, LOCAL_MAP_ID
    S.append('@@ file KncWX2Server/Common/Enum/Enum.h\n')
    rest = split_vmi(1000, 2000)
    S.append(spec_entry('replace', '\t\tVMI_BATTLE_FIELD_REST_END,',
        '#ifdef %s\n\t%s\n%s\n\t\tVMI_BATTLE_FIELD_REST_END = %d,\t// June 32268dd0 (A1a marker table)\n\t//}}\n#else SERV_IRUHADEV_ELYSION_PORT\n\t\tVMI_BATTLE_FIELD_REST_END,\n#endif %s'
        % (FLAG, note('rest field'), block(rest, tabs2), MARKERS['VMI_BATTLE_FIELD_REST_END'], FLAG)))
    vil = split_vmi(20000, 29999)
    S.append(spec_entry('replace', '\t\tVMI_VILLAGE_MAP_END,',
        '#ifdef %s\n\t%s\n%s\n\t\tVMI_VILLAGE_MAP_END = %d,\t// June has VMI_EN = 20010 before it; March has no VMI_EN\n\t//}}\n#else SERV_IRUHADEV_ELYSION_PORT\n\t\tVMI_VILLAGE_MAP_END,\n#endif %s'
        % (FLAG, note('village'), block(vil, tabs2), MARKERS['VMI_VILLAGE_MAP_END'], FLAG)))
    lounge = split_vmi(30000, 39999)
    S.append(spec_entry('insert_after', '\t\tVMI_DUNGEON_LOUNGE_HAMEL,',
        '#ifdef %s\n\t%s\n%s\n\t//}}\n#endif %s' % (FLAG, note('dungeon lounge'), block(lounge, tabs2), FLAG)))
    fields = split_vmi(40000, 49999)
    S.append(spec_entry('replace', '\t\tVMI_BATTLE_FIELD_END,',
        '#ifdef %s\n\t%s\n%s\n\t\tVMI_BATTLE_FIELD_END = %d,\t// June 325712b0 etc.; 40033-40044 are June boss-raid ids March lacks\n\t//}}\n#else SERV_IRUHADEV_ELYSION_PORT\n\t\tVMI_BATTLE_FIELD_END,\n#endif %s'
        % (FLAG, note('battle field'), block(fields, tabs2), MARKERS['VMI_BATTLE_FIELD_END'], FLAG)))
    S.append(spec_entry('insert_after', '\t\tLMI_SANDER\t\t\t\t\t= 10007,',
        '#ifdef %s\n\t%s\n%s\n\t//}}\n#endif %s' % (FLAG, note('LOCAL_MAP_ID'), block(ids['LOCAL_MAP_ID'], tabs2), FLAG)))

    # ---- DungeonEnum.h
    S.append('@@ file KncWX2Server/Common/Enum/DungeonEnum.h\n')
    S.append(spec_entry('replace', '\t\tDI_END\t\t\t\t\t\t\t\t\t\t= 41311,',
        '#ifdef %s\n\t%s\n%s\n\t\tDI_END\t\t\t\t\t\t\t\t\t\t= %d,\n\t//}}\n#else SERV_IRUHADEV_ELYSION_PORT\n\t\tDI_END\t\t\t\t\t\t\t\t\t\t= 41311,\n#endif %s'
        % (FLAG, note('DUNGEON_ID, Hero rows are june-dungeons'), block(ids['DUNGEON_ID'], tabs2), MARKERS['DI_END'], FLAG)))

    # ---- XSLUnitManager.h (server NPC_UNIT_ID)
    S.append('@@ file KncWX2Server/Common/X2Data/XSLUnitManager.h\n')
    S.append(spec_entry('insert_before', '\t//{{ Iruha : 2026-09-19 // Ranox/Lanox port -- new NPC_UNIT_ID (server) values,',
        '#ifdef %s\n\t%s\n%s\n\t//}}\n#endif %s' % (FLAG, note('NPC_UNIT_ID (server)'), block(ids['NPC_UNIT_ID'], tabs2), FLAG)))

    # ---- X2World.h : WORLD_ID (WI_END explicit)
    S.append('@@ file X2Lib/X2World.h\n')
    S.append(spec_entry('replace', '\t\t\tWI_END,',
        '#ifdef %s\n\t\t%s\n%s\n\t\t\tWI_END = %d,\t// June (A1a marker table); the 4 boss worlds 6xx/7xx are Lua-only (C3y-1b)\n\t\t//}}\n#else SERV_IRUHADEV_ELYSION_PORT\n\t\t\tWI_END,\n#endif %s'
        % (FLAG, note('WORLD_ID'), block(ids['WORLD_ID'], tabs3), MARKERS['WI_END'], FLAG)))

    # ---- X2UnitManager.h : NPC_UNIT_ID (NUI_MAX_LIMIT stays implicit = last + 1; last member is the largest value)
    S.append('@@ file X2Lib/X2UnitManager.h\n')
    nui = ids['NPC_UNIT_ID']
    assert nui[-1][1] == max(v for _, v in nui) and nui[-1][1] < 65535
    S.append(spec_entry('insert_after', '\t\t\tNUI_MUL_MAI_EVENT\t\t\t\t\t= 10190,\n#endif SERV_IRUHADEV_RANOX_PORT\n\t\t//}}',
        '#ifdef %s\n\t\t%s\n%s\n\t\t//}}\n#endif %s' % (FLAG, note('NPC_UNIT_ID; NUI_MAX_LIMIT stays implicit (last + 1, <= 65536 for X2Item_Preprocessing.inl:1033)'),
                                                          block(nui, tabs3), FLAG)))

    # ---- X2LocationManager.h : HOUSE_ID, LOCAL_MAP_ID, IsVillageField
    S.append('@@ file X2Lib/X2LocationManager.h\n')
    S.append(spec_entry('insert_before', '\t\t\t//{{ Iruha : 2026-09-19 // Ranox port Phase 28 -- HOUSE_ID mirror of',
        '#ifdef %s\n\t\t\t%s\n%s\n\t\t\t//}}\n#endif %s' % (FLAG, note('HOUSE_ID'), block(ids['HOUSE_ID'], tabs3), FLAG)))
    S.append(spec_entry('insert_before', '\t\t\tLMI_RANOX = 10008,\t/// SERV_IRUHADEV_RANOX_PORT: intentional value alias of LMI_CHINA (dead code -- SERV_CHINA_ADVENTURE is never #defined; only use site X2PartyUI.cpp:1940)',
        '#ifdef %s\n\t\t\t%s\n%s\n\t\t\t//}}\n#endif %s' % (FLAG, note('LOCAL_MAP_ID'), block(ids['LOCAL_MAP_ID'], tabs3), FLAG)))
    S.append(spec_entry('replace', '\t\t\tif ( eVillageID >= SEnum::VMI_RUBEN && eVillageID <= SEnum::VMI_RANOX )',
        '#ifdef %s\n\t\t\t//{{ %sIsVillageField: June 32268dd0 inlines VMI_RUBEN..20010 (VMI_ELYSION 20009 and VMI_EN 20010; March has no VMI_EN)\n'
        '\t\t\tif ( eVillageID >= SEnum::VMI_RUBEN && eVillageID <= 20010 )\n\t\t\t//}}\n#else %s\n'
        '\t\t\tif ( eVillageID >= SEnum::VMI_RUBEN && eVillageID <= SEnum::VMI_RANOX )\n#endif %s' % (FLAG, HDR, FLAG, FLAG)))

    # ---- X2LocalMapUI.h : LMUCM 18006-18008 (explicit)
    S.append('@@ file X2Lib/X2LocalMapUI.h\n')
    S.append(spec_entry('insert_after', '\t\tLMUCM_DUNGEON_MOUSE_OUT,',
        '#ifdef %s\n\t%s\n%s\n\t//}}\n#endif %s' % (FLAG, note('LMUCM 18006-18008, June values (the Elysion page scroll + guide window); explicit, never positional'), block(LMUCM, tabs2), FLAG)))

    # ---- offline enum seed
    S.append('@@ file X2Lib/Offline/X2OfflineLuaEnumSeed.h\n')
    row = lambda n, v: '\t\t{ "%s", %*d },' % (n, 44 - len(n), v)
    origs = {'VMI_BATTLE_FIELD_REST_END': ('\t\t{ "VMI_BATTLE_FIELD_REST_END",               1007 },', 1007),
             'VMI_VILLAGE_MAP_END': ('\t\t{ "VMI_VILLAGE_MAP_END",                      20008 },', 20008),
             'VMI_BATTLE_FIELD_END': ('\t\t{ "VMI_BATTLE_FIELD_END",                     40026 },', 40026)}
    for name in ('VMI_BATTLE_FIELD_REST_END', 'VMI_VILLAGE_MAP_END', 'VMI_BATTLE_FIELD_END'):
        orig, old = origs[name]
        S.append(spec_entry('replace', orig, '#ifdef %s\n%s\t// June marker (A3); was %d\n#else SERV_IRUHADEV_ELYSION_PORT\n%s\n#endif %s'
                            % (FLAG, row(name, MARKERS[name]), old, orig, FLAG)))
    seed_rows = ''.join('\t\t{ "%s", %d },\n' % (n, v) for n, v in ids['VILLAGE_MAP_ID']).rstrip('\n')
    S.append(spec_entry('insert_after', '\t\t{ "VMI_BATTLE_FIELD_RANOX_FIELD_04",         40032 },',
        '#ifdef %s\n\t%s\n%s\n\t//}}\n#endif %s' % (FLAG, note('Atlas Station, Elysion village, lounge and Fields 01-04 (SEnum::VILLAGE_MAP_ID, Enum.h)'), seed_rows, FLAG)))
    return S, ids


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    if sys.argv[1] == 'lua':
        do_lua('--dry-run' in sys.argv)
    elif sys.argv[1] == 'spec':
        S, ids = do_spec(sys.argv[2])
        open(sys.argv[2], 'w', newline='\n', encoding='ascii').write(''.join(S))
        print('spec parts', len(S), '->', sys.argv[2])
