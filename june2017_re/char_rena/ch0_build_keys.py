"""char-rena CH0 item 3: ch0_keys.tsv = plan_charport_keys.tsv + owner + readers for the 8 june-only, 3 neither, 4 march-only-site keys. Copy of char_raven/ch0_build_keys.py.
python june2017_re/char_rena/ch0_build_keys.py  (reads ch0_state_skill.tsv for each state's phase; the March script counts are read from the March Lire_ElvenRanger.lua bytecode)"""
import os, re, csv, bisect, collections
HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..')) + '/'
L = open(T + 'june_src_fixed/data036_kom/Lire_ElvenRanger.src.lua', encoding='utf-8', errors='replace').read().split('\n')
MS = open('F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/data036_kom/Lire_ElvenRanger.lua', 'rb').read()
tops = [(i + 1, m.group(1)) for i, l in enumerate(L) for m in [re.match(r'^(\w+) = \{', l)] if m]
ln = [t[0] for t in tops]
ph = {}
for r in csv.reader(open(HERE + '/ch0_state_skill.tsv', encoding='utf-8'), delimiter='\t'):
    if len(r) > 7 and r[0] and not r[0].startswith('#') and r[0] != 'state':
        ph[r[0]] = r[7]


def states(key):
    c = collections.OrderedDict()
    for i, l in enumerate(L):
        if re.match(r'\s*%s\s*=' % re.escape(key), l):
            s = tops[bisect.bisect_right(ln, i + 1) - 1][1]
            c[s] = c.get(s, 0) + 1
    return c


def mscript(key):
    return 'March Lire script (bytecode) has the string x%d' % MS.count(key.encode()) if MS.count(key.encode()) else 'March Lire script (bytecode) lacks the string'


BUFF_OWNER = 'n/a (engine buff sub-table key, read by stringized type name on both sides)'
BUFF_RD = ('June: plain str %s read by %s (CX2BuffFactor parser) and %s (CX2BuffTemplet / Finalizer parser), both outside the Rena range. '
           'March: %s; no direct literal. %s. States: %%s')
SPEC, RD = {}, {}


def put(k, owner, reader):
    SPEC[k] = owner
    RD[k] = reader


put('BBT_CHANGE_NOW_HP_PER_SECOND', BUFF_OWNER, BUFF_RD % ('33526fe4', 'FUN_32d4bbf0 @32d4bc77', 'FUN_32e80450 @32e80518', 'X2BuffBehaviorTemplet.cpp:59 CASE_BEHAVIOR_TEMPLET, X2BuffFactor.cpp:118 CASE_BEHAVIOR_FACTOR_SKILL', mscript('BBT_CHANGE_NOW_HP_PER_SECOND')))
put('BFT_TIME', BUFF_OWNER, BUFF_RD % ('33527124', 'FUN_32d4c750 @32d4c7d7', 'FUN_32ec36f0 @32ec3798', 'X2BuffFinalizerTemplet.cpp:57 CASE_FINALIZER_TEMPLET, X2BuffFactor.cpp:490', mscript('BFT_TIME')))
put('FORCE_DYNAMIC_CAMERA_INDEX', 'Stage 9 (camera; not a class-port item)',
    'June: xor key 3205c0e8 in 32057b80 CX2GUUser::CommonStateStart (outside the Rena range). March: no literal in X2Lib / KTDXLIB. ' + mscript('FORCE_DYNAMIC_CAMERA_INDEX') + '. States: %s')
put('RETAIN_CAMERA_STATE', 'Stage 9 (camera)',
    'June: xor key 3205b934 in 32057b80 CX2GUUser::CommonStateStart. March: none. ' + mscript('RETAIN_CAMERA_STATE') + '. States: %s')
put('IMMUNITY_EDT_LIST_AT_THIS_STATE', 'n/a (backlog Stage 2 per-state immunity lists, as Chung / Elsword / Raven)',
    'June: plain str 3347325c read by FUN_31fdaaa0 @31fdac15 (CX2GUUser per-state parse, +0x8648, beside IMMUNITY_LIST_AT_THIS_STATE); '
    'not in the Rena range. March: none (no IMMUNITY_EDT literal in X2Lib / KTDXLIB); ' + mscript('IMMUNITY_EDT_LIST_AT_THIS_STATE') + '. States: %s')
put('IMMUNE_TO_SUPER_ARMOR_DOWN', 'n/a here: backlog Stage 2 per-state immunity lists (engine reader 31fdaaa0 from CX2GUUser::CommonStateStart @32057e68; bullet amended by char-rena CH0)',
    'June: xor key (plan_charport_keys.tsv june_src = xor), reader 31fdaaa0 (the CX2GUUser per-state parse that also reads the IMMUNITY_* lists; address from the census, key address and call site not read). '
    'March: no literal in X2Lib / KTDXLIB (plan march_n 0); ' + mscript('IMMUNE_TO_SUPER_ARMOR_DOWN') + '. States: %s')
put('BOW_SKILL_STATE', 'CH1j + CH4b (flag +0x981d: set in CommonStateStart 32ca81b0 @32ca82c2, read in CommonStateEnd 32ca5060 @32ca5078 (CH1j) and GetActualCoolTime 32c7be00 @32c7be7b (CH4b))',
    'June: xor key read by 32ca81b0 (plan_charport_keys.tsv june_readers; key address not read). March: no literal in X2Lib / KTDXLIB (plan march_n 0); '
    + mscript('BOW_SKILL_STATE') + '. States: %s')
put('CREATE_TRAP_BLADE', 'CH1j (read inside Rena\'s vtable 0x7c4 CommonStateStart override 32ca81b0)',
    'June: xor key read by 32ca81b0 (plan_charport_keys.tsv june_readers; key address not read). March: no literal in X2Lib / KTDXLIB (plan march_n 0); '
    + mscript('CREATE_TRAP_BLADE') + '. States: %s')
put('ATTACK_ACTION_TYPE', 'n/a (no reader on either side; census june-ignores/8b)',
    'June: no string (plan june_src none). March: no literal in X2Lib / KTDXLIB (plan march_n 0); ' + mscript('ATTACK_ACTION_TYPE') + '. States: %s')
put('HEAD_IK', 'n/a (no reader anywhere)',
    'June: no string. March: only commented, X2GUUser.cpp:16975 (//LUA_GET_VALUE HEAD_IK). ' + mscript('HEAD_IK') + '. Same as Chung / Elsword / Raven. States: %s')
put('JUMP_CANCEL_AFTER', 'n/a (no reader on either side; the data stays in the script)',
    'June: no string (plan june_src none). March: no literal in X2Lib / KTDXLIB (plan march_n 0); ' + mscript('JUMP_CANCEL_AFTER') + '. States: %s')
put('UNIT_SLASH_TRACE0', 'n/a (engine reads it on both sides; format key UNIT_SLASH_TRACE%d)',
    'June: plain "UNIT_SLASH_TRACE" 334879b8 read by 3227af80 CX2GameUnit::DoUnitSlashTraceInCommonStateStart (only caller CommonStateStart @3205b628, funcmap 873). '
    'March: X2GameUnit.cpp:17474 DoUnitSlashTraceInCommonStateStart (UNIT_SLASH_TRACE_MANAGER_TEST on, Always.h:2857). ' + mscript('UNIT_SLASH_TRACE0') + '. States: %s')
put('UNIT_SLASH_TRACE1', 'n/a (engine reads it on both sides; format key UNIT_SLASH_TRACE%d)',
    'as UNIT_SLASH_TRACE0 (same reader, index 1). ' + mscript('UNIT_SLASH_TRACE1') + '. States: %s')
put('AFTER_IMAGE', 'n/a (engine reads it on both sides; census artifact)',
    'June: plain str 3346efb4 (the census saw the prefixed 3346efb2) read by June_CEffect_LoadAfterImage 31f74a9f, 32057b80 @3205b62f (CX2GUUser::CommonStateStart) '
    'and 32784c00 @3278d4c9 (NPC state parse). March: X2GUUser.cpp:17293 (state AFTER_IMAGE {start,time}), X2GUNPC.cpp:6193 / :16638, X2DamageEffect.cpp:10903 (CEffect::LoadAfterImage). '
    + mscript('AFTER_IMAGE') + '. States: %s')
put('READY_SOUND', 'n/a (engine reads it on both sides; census artifact)',
    'June: plain str 33487924 read by FUN_3227cc90 @3227ce26, FUN_32275ef0 @32276149 (June CX2GameUnit::InitDevice), FUN_327f4e30 @327f582f (stage data), '
    'FUN_32d287c0 @32d28a19, FUN_32d29470 @32d29606, FUN_326e4b60 @326e4db9, FUN_326e55f0 @326e5786. March: X2GameUnit.cpp:4199 InitDevice(Device&, Lua) and :4262, '
    'X2Pet.cpp:1825, X2DungeonStage.cpp:516. ' + mscript('READY_SOUND') + '. States: %s')

rows = list(csv.reader(open(HERE + '/plan_charport_keys.tsv', encoding='utf-8'), delimiter='\t'))
out = [rows[0] + ['owner', 'readers']]
cnt = collections.Counter()
for r in rows[1:]:
    k = r[0]
    own = rd = ''
    if k in SPEC or r[11] in ('june-only', 'neither', 'march-only-site'):
        if k not in SPEC:
            raise SystemExit('no owner for ' + k)
        own, rd = SPEC[k], RD[k]
        if '%s' in rd:
            rd = rd % ('; '.join('%s [%s]' % (s, ph.get(s, 'INIT' if s.startswith('INIT_') else '?')) + (' x%d' % n if n > 1 else '') for s, n in states(k).items()))
        cnt[r[11]] += 1
    out.append(r + [own, rd])
with open(HERE + '/ch0_keys.tsv', 'w', encoding='utf-8', newline='') as f:
    csv.writer(f, delimiter='\t', lineterminator='\n').writerows(out)
print(len(out) - 1, 'rows;', dict(cnt), 'with owner:', sum(1 for r in out[1:] if r[-2]))
