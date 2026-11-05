"""char-raven CH0 item 3: ch0_keys.tsv = plan_charport_keys.tsv + owner + readers for the 10 june-only, 6 neither, 6 march-only-site keys and the 4 named readers.
python june2017_re/char_raven/ch0_build_keys.py  (reads ch0_state_skill.tsv for each state's phase)"""
import os, re, csv, bisect, collections
HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..')) + '/'
L = open(T + 'june_src_fixed/data036_kom/RAVEN_FIGHTER.src.lua', encoding='utf-8', errors='replace').read().split('\n')
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


BUFF_OWNER = 'n/a (engine buff sub-table key, read by stringized type name on both sides)'
BUFF_RD = ('June: plain str %s read by %s (CX2BuffFactor parser) and %s (CX2BuffTemplet / Finalizer parser), both outside the Raven range. '
           'March: %s; no direct literal. March Raven script has the string. States: %%s')
SPEC, RD = {}, {}


def put(k, owner, reader):
    SPEC[k] = owner
    RD[k] = reader


put('BBT_CHANGE_NOW_HP_PER_SECOND', BUFF_OWNER, BUFF_RD % ('33526fe4', 'FUN_32d4bbf0 @32d4bc77', 'FUN_32e80450 @32e80518', 'X2BuffBehaviorTemplet.cpp:59 CASE_BEHAVIOR_TEMPLET, X2BuffFactor.cpp:118 CASE_BEHAVIOR_FACTOR_SKILL'))
put('BBT_DASH_IMPOSSIBLE', BUFF_OWNER, BUFF_RD % ('33526f98', 'FUN_32d4bbf0 @32d4bcb4', 'FUN_32e80450 @32e806f1', 'X2BuffBehaviorTemplet.cpp:62, X2BuffFactor.cpp:121 / :312'))
put('BBT_FROZEN', BUFF_OWNER, BUFF_RD % ('335269c4', 'FUN_32d4bbf0 @32d4c0a6', 'FUN_32e80450 @32e81ec9', 'X2BuffBehaviorTemplet.cpp:106'))
put('BBT_NEVER_MOVE', BUFF_OWNER, BUFF_RD % ('3352698c', 'FUN_32d4bbf0 @32d4c0e2', 'FUN_32e80450 @32e8204f', 'X2BuffBehaviorTemplet.cpp:109'))
put('BBT_STUN', BUFF_OWNER, BUFF_RD % ('335269f0', 'FUN_32d4bbf0 @32d4c07e', 'FUN_32e80450 @32e81dc5', 'X2BuffBehaviorTemplet.cpp:104, X2BuffFactor.cpp:176'))
put('BFT_HITTED_COUNT', BUFF_OWNER, BUFF_RD % ('33527100', 'FUN_32d4c750 @32d4c801', 'FUN_32ec36f0 @32ec3853', 'X2BuffFinalizerTemplet.cpp:59, X2BuffFactor.cpp:492 / :513'))
put('BFT_TIME', BUFF_OWNER, BUFF_RD % ('33527124', 'FUN_32d4c750 @32d4c7d7', 'FUN_32ec36f0 @32ec3798', 'X2BuffFinalizerTemplet.cpp:57 CASE_FINALIZER_TEMPLET, X2BuffFactor.cpp:490'))
put('FORCE_DYNAMIC_CAMERA_INDEX', 'Stage 9 (camera; not a class-port item)',
    'June: xor key 3205c0e8 in 32057b80 CX2GUUser::CommonStateStart (outside the Raven range). March: no literal in X2Lib / KTDXLIB. '
    'March Raven script lacks the key (bytecode string absent); June script sets it x15. States: %s')
put('RETAIN_CAMERA_STATE', 'Stage 9 (camera)',
    'June: xor key 3205b934 in 32057b80 CX2GUUser::CommonStateStart. March: none; March Raven script lacks it. States: %s')
put('IMMUNITY_EDT_LIST_AT_THIS_STATE', 'n/a (backlog Stage 2 per-state immunity lists, as Chung / Elsword)',
    'June: plain str 3347325c read by FUN_31fdaaa0 @31fdac15 (CX2GUUser per-state parse, +0x8648, beside IMMUNITY_LIST_AT_THIS_STATE); '
    'not in the Raven range. March: none (no IMMUNITY_EDT literal in X2Lib / KTDXLIB); March Raven script lacks it. States: %s')
put('ANUM_SPEED', 'n/a (June script typo for ANIM_SPEED; no reader on either side; keep verbatim)',
    'June: no string anywhere (Ghidra strings filter empty, xorstrings.tsv none). March: no literal in X2Lib / KTDXLIB. The same typo is in the March '
    'Raven script (bytecode string present), so nothing regresses: the state plays its anim at the default speed. JUNE_BUGS row candidate. States: %s')
put('HEAD_IK', 'n/a (no reader anywhere)',
    'June: no string (strings filter empty, xorstrings none). March: only commented, X2GUUser.cpp:16975 (//LUA_GET_VALUE HEAD_IK). '
    'March script has the string. Same as Chung / Elsword. States: %s')
put('HEAD_IK_MAX_ANGLE', 'n/a (no reader anywhere)',
    'June: no string. March: only commented, X2GameUnit.cpp:4600 (//HEAD_IK_MAX_ANGLE; also _ANGLE_CHANGE_RATE, _RADIUS). March script has the string. Same as Chung / Elsword')
put('STOP_RESERVE_TIME_ATT', 'n/a (no reader anywhere; census june-ignores/8b)',
    'June: no string (strings filter empty). March: no literal in X2Lib / KTDXLIB. March script has the string x1. States: %s')
put('STOP_RESERVE_TIME_DEF', 'n/a (no reader anywhere; census june-ignores/8b)', 'as STOP_RESERVE_TIME_ATT. States: %s')
put('STOP_TIME_ATK', 'n/a (script typo for STOP_TIME_ATT; no reader on either side; keep verbatim)',
    'June: no string (filter empty). March: X2DamageManager.cpp:671-672 read STOP_TIME_ATT / STOP_TIME_DEF only; _ATK is read nowhere. '
    'Present x8 in the March Raven script too (bytecode), so no regression: the attacker hit-stop of those 8 tables is 0 on both sides. JUNE_BUGS row candidate. States: %s')
put('BLADE_MASTERY', 'n/a (dead on both sides)',
    'June: no string (strings filter empty, xorstrings none). March: X2GURaven.cpp:5122 (CommonStateStart) sits under #ifndef BALANCE_BLADE_MASTER_20130117; '
    'that flag is defined (Always.h) and the cl /P stream of X2GURaven.cpp has no "BLADE_MASTERY" literal: not compiled. '
    'June\'s script drops no input a live reader needs. States: %s')
put('UNIT_SLASH_TRACE0', 'n/a (engine reads it on both sides; format key UNIT_SLASH_TRACE%d)',
    'June: plain "UNIT_SLASH_TRACE" 334879b8 read by 3227af80 CX2GameUnit::DoUnitSlashTraceInCommonStateStart (only caller CommonStateStart @3205b628, funcmap 873). '
    'March: X2GameUnit.cpp:17474 DoUnitSlashTraceInCommonStateStart (UNIT_SLASH_TRACE_MANAGER_TEST on, Always.h:2857). March Raven script has UNIT_SLASH_TRACE0 (x1, no 1..). '
    'June sets it in 37 states')
put('BUFF_FACTOR_ID_AND_APPLY_TIME', 'n/a (engine reads it on both sides; ported by skill S3e)',
    'June: plain str 3347474c read by 31fe32d0 CX2GUUser::ParsingBuffFactorID (+ _ULTIMATE 3347476c) and 3275ee70 (NPC twin), funcmap 3240 / 3262. '
    'March: X2GUUser.cpp:48255 ParseStateInsertBuffFactorTable, :48393 ApplyAllStateInsertBuffFactor; X2GUNPC.cpp:34034 / :34103. '
    'The March Raven script LACKS the key (0 in bytecode); June ADDS it x2: the reader needs the input and June supplies it. States: %s')
put('LeftDirCamear', 'n/a (engine reads it on both sides; already ported)',
    'June: "LeftDirCamear" 33471358 (+ RightDirCamear 33471368) read by 31fb6af0 CX2Camera::PlayLuaCamera (SERV_IRUHADEV_JUNE_DIVERGE_LUA_CAMERA, funcmap 7303). '
    'March: X2Camera.cpp:191 and :339 (Direction == true picks the table by facing). Both keys are in the March Raven script; June sets it x2 in SUB_CAMERA1: no input dropped. States: %s')
put('AFTER_IMAGE', 'n/a (engine reads it on both sides; census artifact)',
    'June: plain str 3346efb4 (the census saw the prefixed 3346efb2) read by June_CEffect_LoadAfterImage 31f74a9f, 32057b80 @3205b62f (CX2GUUser::CommonStateStart) '
    'and 32784c00 @3278d4c9 (NPC state parse). March: X2GUUser.cpp:17293 (state AFTER_IMAGE {start,time}), X2GUNPC.cpp:6193 / :16638, X2DamageEffect.cpp:10903 (CEffect::LoadAfterImage). '
    'March Raven script has the string; June sets it in 74 states: no input dropped')
put('READY_SOUND', 'n/a (engine reads it on both sides; census artifact)',
    'June: plain str 33487924 read by FUN_3227cc90 @3227ce26, FUN_32275ef0 @32276149 (June CX2GameUnit::InitDevice), FUN_327f4e30 @327f582f (stage data), '
    'FUN_32d287c0 @32d28a19, FUN_32d29470 @32d29606, FUN_326e4b60 @326e4db9, FUN_326e55f0 @326e5786. March: X2GameUnit.cpp:4199 InitDevice(Device&, Lua) and :4262, '
    'X2Pet.cpp:1825, X2DungeonStage.cpp:516. June script x1 (INIT_DEVICE); March script has the string')
put('BURNING_NASOD_HAND_POSSIBLE', 'CH1 final (script data) + CH4a (24401 setter with the = 2 write)',
    'June: xor key decoded @32c43911 inside Raven\'s vtable 0x7c4 CommonStateStart override 32c43380 (int3 block 32c43120-32c43be7; xorstrings names its entry 32c43250 and the class map '
    'marks 32c43120 / 32c43250 dead, but the key read and the +0x4108 = 2 write @32c43ac4 are in 32c43380): +0x9688 (skill 24401 learned) and the key -> ChangeDamageType( +0x968c, +0x9690 ). '
    'March: X2GURaven.cpp:5112 in CX2GURaven::CommonStateStart (RAVEN_SECOND_CLASS_CHANGE) -> ChangeDamageType. Checklist 7b L327: the 24401 setter (32c57660) and the = 2 write together. '
    'June script x24: states %s')
put('MAX_HYPER_MODE_TIME', 'n/a (engine reads it on both sides)',
    'June: xor key @32289acf in 32289090 CX2GameUnit::InitComponent (June) and @322a6634 in 322a5f90 (CX2GameUnit init, funcmap C3h). '
    'March: X2GameUnit.cpp:856 (ReInit) and :4593 (InitComponent), default 30.f. Raven script INIT_COMPONENT L269 = 30 (= the default)')
put('HYPER_MODE_COUNT', 'n/a (NPC-only reader on both sides; unread for a user)',
    'June: xor key @327a4d9e in 327a0fa0 CX2GUNPC::InitComponent only. March: X2GUNPC.cpp:4512 CX2GUNPC::InitComponent only. '
    'Raven script INIT_COMPONENT L271 = 1: no user reader on either side')
put('DAMAGE_DATA_THIRD', 'n/a (reader and both script states are dead in June)',
    'June: plain str 33509570 read by 32c5c5a0 (BURSTING_BLADE_ATTACK FrameMove shape; class map: no call, jmp or pointer reference = dead), also Chung 32bee840 and Lire 32caca80. '
    'March: X2GURaven.cpp:18182 (RSI_SA_RWT_BURSTING_BLADE_ATTACK_FrameMove), also X2GUChung.cpp:23733, X2GULire_ElvenRanger.cpp:13565. '
    'June script x2: %s are june-dead-data (string absent from the exe); June\'s Bursting Blade is RSI_SA_RVC_BURSTING_BLADE (skill 24040, CH3c)')

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
