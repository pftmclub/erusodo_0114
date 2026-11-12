r"""char-elesis CH0: build ch0_checklist_rows.tsv (Elesis rows of JUNE2017_CHARACTER_PORT_CHECKLIST.md -> phase / n/a).
    python june2017_re/char_elesis/ch0_build_checklist_rows.py
Modelled on char_ara/ch0_build_checklist_rows.py. Columns: section, checklist_line, row, elesis_evidence, phase, note.
AL( hint_line, anchor ) finds the row in the CURRENT checklist (first line >= hint-8 holding the anchor substring) and stops the build
when it is not found; a moved row prints a NOTE. Elesis: classes 8 / 29 / 30 / 35 (June-only) / 120 / 121 / 125 (June-only),
June range 32b4e770-32b90b60. Evidence is read from ch0_class_map.tsv, ch0_state_skill.tsv, ch0_numtables.json, ch0_vtable_all.tsv,
ch0_keys.tsv, ch0_blood.tsv, ch0_summon.tsv, ch0_wos.tsv, ch0_reg.tsv, ch0b_stage_list.tsv, ch1_held_motions.tsv, ch0_motion_question.tsv,
plus a byte scan of the June exe (unpacked_x2.exe) for callers of 3201ad10 over her range.
Every Part A1 / A2 / A3 row of plan_checklist_rows.md must be covered by a `plan=` tag (checked at the end).
"""
import csv, json, re, os, bisect, struct, collections
csv.field_size_limit(10 ** 9)
HERE = os.path.dirname(os.path.abspath(__file__)) + '/'
T = os.path.normpath(HERE + '../../') + '/'
CL = open(T + 'JUNE2017_CHARACTER_PORT_CHECKLIST.md', encoding='utf-8').read().split('\n')
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
LO, HI = 0x32b4e770, 0x32b90b70


def AL(n, sub):
    for k in range(max(1, n - 8), len(CL) + 1):
        if sub in CL[k - 1]:
            if k != n:
                print('NOTE: anchor %r expected L%d, found L%d' % (sub, n, k))
            return 'L%d' % k
    raise SystemExit('line %d moved: %r not found' % (n, sub))


def rd(f):
    return [r for r in csv.reader(open(HERE + f, encoding='utf-8', newline=''), delimiter='\t') if r]


# ---- June exe byte scan: callers of 3201ad10 in her range
_d = open(EXE, 'rb').read()
_pe = struct.unpack_from('<I', _d, 0x3c)[0]
_ns = struct.unpack_from('<H', _d, _pe + 6)[0]
_os = struct.unpack_from('<H', _d, _pe + 20)[0]
_base = struct.unpack_from('<I', _d, _pe + 24 + 28)[0]
_secs = []
_o = _pe + 24 + _os
for _i in range(_ns):
    _vs, _va, _rs, _ro = struct.unpack_from('<IIII', _d, _o + 8)
    _secs.append((_va, _vs, _ro))
    _o += 40


def off(va):
    for v, vs, ro in _secs:
        if v <= va - _base < v + vs:
            return ro + (va - _base - v)


def call_hits(seg_lo, seg_hi, target):
    seg = _d[off(seg_lo):off(seg_hi)]
    out = []
    for i in range(len(seg) - 5):
        if seg[i] in (0xe8, 0xe9):
            rel = struct.unpack_from('<i', seg, i + 1)[0]
            if (seg_lo + i + 5 + rel) & 0xffffffff == target:
                out.append('%08x' % (seg_lo + i))
    return out


# sanity: the scan must see a known caller (Chung range 32bec286)
assert call_hits(0x32bec000, 0x32bed000, 0x3201ad10) == ['32bec286']
H3201AD10 = call_hits(LO, HI, 0x3201ad10)

# ---- evidence tables
cm_rows = rd('ch0_class_map.tsv')
cm = [r for r in cm_rows if re.fullmatch('[0-9a-f]{8}', r[0])]
cm.sort(key=lambda r: int(r[0], 16))
cm_ad = [int(r[0], 16) for r in cm]
cmd = {r[0]: r for r in cm}
assert (cm[0][0], cm[-1][0], len(cm)) == ('32b4e770', '32b90b60', 687)


def cr(a):
    """class-map row of the function containing address a"""
    return cm[bisect.bisect_right(cm_ad, int(a, 16)) - 1]


def cp(a):
    return cr(a)[6]


def cpn(a):
    r = cr(a)
    return '%s (%s)' % (r[0], r[6]) + ('' if r[1] != 'dead' else ' dead')


st_rows = [r for r in rd('ch0_state_skill.tsv') if not r[0].startswith('#')]
st = {r[0]: r for r in st_rows if len(r) > 7 and not r[0].isdigit() and r[0] not in ('state', 'skill_id')}
sk = {r[1]: r for r in st_rows if len(r) > 5 and r[0].isdigit()}          # name -> skill row (id, name, type, classes, states, phase, note)
sk_id = {r[0]: r for r in st_rows if len(r) > 5 and r[0].isdigit()}
vt = {r[0]: r for r in rd('ch0_vtable_all.tsv')[1:]}
keys = {r[0]: r for r in rd('ch0_keys.tsv')[1:] if not r[0].startswith('#')}
reg = rd('ch0_reg.tsv')
blood = {r[0]: r for r in rd('ch0_blood.tsv')[1:]}
SRC = open(T + 'june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua', encoding='utf-8', errors='replace').read().split('\n')


def src_top(ln):
    t = None
    for i in range(ln):
        m = re.match(r'^([A-Za-z_0-9]+) = \{', SRC[i])
        if m:
            t = m.group(1)
    return t


def src_lines(pat):
    return [i + 1 for i, l in enumerate(SRC) if re.search(pat, l)]


def vover(slot):
    r = vt[slot]
    return r[4] == 'Y'


def vdesc(slot):
    r = vt[slot]
    return 'vslot %s: Elesis %s, base %s, %s' % (slot, r[1], r[2], 'OVERRIDDEN' if r[4] == 'Y' else 'not overridden')


# ---- plan rows (coverage check)
PL = open(HERE + 'plan_checklist_rows.md', encoding='utf-8').read().split('\n')
plan_keys = set()
part = None
for l in PL[9:96]:
    if l.startswith('## Part A1'):
        part = 'A1'
    elif l.startswith('## Part A2'):
        part = 'A2'
    elif l.startswith('## Part A3'):
        part = 'A3'
    elif l.startswith('## Part B'):
        break
    elif l.startswith('|') and part and not l.startswith('|---') and not l.startswith('| Sec') and not l.startswith('| L '):
        c = [x.strip() for x in l.strip('|').split('|')]
        if part == 'A2':
            plan_keys.add('A2:' + c[0])
        else:
            plan_keys.add('%s:%s' % (part, c[1]))
covered = set()
R = []


def add(sec_, line, row, ev, phase, note='', plan=()):
    for p in plan:
        covered.add(p)
    tag = (' [plan ' + ', '.join(plan) + ']') if plan else ''
    R.append((sec_, line, row, ev, phase, note + tag))


# ---------------- intro
add('intro', AL(4, 'Ara, Elesis'), 'June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, Add)',
    'Elesis is a March class with a June update: classes UC_ELESIS_KNIGHT 8, SABER_KNIGHT 29, PYRO_KNIGHT 30, DARK_KNIGHT 35 (June-only), GRAND_MASTER 120, BLAZING_HEART 121, CRIMSON_AVENGER 125 (June-only); '
    'June range 32b4e770-32b90b60 (class map: 687 functions)', 'n/a', 'scope; the June-only classes 35 / 125 ride CHNa / CHNb', ('A1:4',))
nenum = [n for n in ('RNSI_A_SCA_BLOOD_STING', 'RNSI_A_SGM_UPPER_SLASH', 'RNSI_P_SBH_BLAZE_ABSOLU', 'RNSI_SA_SCA_CHASING_SWORD')]
enum_rows = {r[1]: r for r in rd('ch0_enum_names.tsv') if r[0] == 'skill'}
add('intro', AL(14, 'Character scripts (`Elsword_SwordMan.lua`'), 'character script ELESIS_KNIGHT.lua feeds SetDamageDataFromLUA',
    'June src %d lines (june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua, 399,379 B), 311 top-level tables (305 state + 6 INIT_*; 299 mapped, 6 june-dead-data n/a; ch0_state_skill.tsv); '
    'March data036 ELESIS_KNIGHT.lua 157,500 B bytecode (1B 4B 4C 81, kom_contents); not on the core list (tools/data/lua_core_list.tsv names Elesis in no row). '
    'luabind enum gate: 4 skill names the script uses are missing from the live Enum.lua: %s' % (len(SRC) - 1, '; '.join('%s %s' % (n, enum_rows[n][2]) for n in nenum)),
    'CH1 final', 'adopt-june ELESIS_KNIGHT.lua (June bytecode via ljconv) after the states exist; CH1 adds the 4 missing Enum.lua names (core-list Enum) before packing')

# ---------------- section 1
nt = json.load(open(HERE + 'ch0_numtables.json'))
assert len(nt) == 20
for o in nt:
    ln, top, tl, tname, tstart, idx, app, apptop, dam = o
    nm = app[0].replace('SKILL_ID.', '')
    sr = sk[nm]
    ph = st[top][7]
    add('§1', AL(52, '| `ELESIS_KNIGHT` | 20'), 'numerical-only %s table in state %s (script L%d; NUMERICAL_DATA_INDEX = %s at L%d)' % (tname, top, tstart, idx, ln),
        'APPLIED_SKILL_ID %s %s (classes %s); no DAMAGE row in the block; state %s starts at script L%d' % (nm, sr[0], sr[3], top, tl),
        ph, 'NUM use line from the play-test; phase = ch0_state_skill.tsv phase of the state', ('A1:52',))

# ---------------- section 2-3
k = keys['IGNORE_DEFENCE_DECLINE']
add('§2', AL(123, '| `IGNORE_DEFENCE_DECLINE` |'), 'IGNORE_DEFENCE_DECLINE (to-implement, 8b group 1): ELESIS_KNIGHT x2',
    '%s uses in ELESIS_KNIGHT.src.lua at L%s (states %s); engine reader live at X2Lib/X2DamageManager.cpp:752 (June reader 32632390 SetDamageDataFromLUA; ch0_keys.tsv census implemented/8a); '
    'March ELESIS_KNIGHT.lua bytecode lacks the string' % (k[2], ', '.join(map(str, src_lines('IGNORE_DEFENCE_DECLINE'))), ', '.join(src_top(l) for l in src_lines('IGNORE_DEFENCE_DECLINE'))),
    'CH1 final', 'the data arrives with the adopted script; the row\'s "to-implement" status is out of date (reader present)', ('A1:123',))
k = keys['APPLIED_SKILL_ID']
add('§2', AL(116, '| `APPLIED_SKILL_ID` |'), 'APPLIED_SKILL_ID (parsed since 8c): every character script',
    '%s uses in ELESIS_KNIGHT over %s tables (ch0_keys.tsv): the 20 numerical-only blocks of section 1 plus damage rows; readers %s' % (k[2], k[3], k[9][:140]),
    'CH1 final', 'script data arrives with the adopted script; the numerical ones are the section 1 rows', ('A3:116, 150',))
add('§3', AL(150, '| Applied skill ID |'), 'Applied skill ID: absent, every character (167 uses)',
    'Elesis uses = the section 2 row (%s uses over %s tables); the numerical override is live (S4)' % (k[2], k[3]), 'CH1 final', 'covered by the section 2 row', ('A3:116, 150',))

# ---------------- section 6
add('§6', AL(217, 'Standing check for every class port'), 'class overrides ReservedSkillStart (June 32057b80 @32057c4c-cb2) and extends the X2GUUser hook chain',
    '%s; override 32b73e60 (class map %s, base 31aa1d30); March CX2GUEL has no ReservedSkillStart' % (vdesc('0x800'), cp('32b73e60')),
    'CH1k', 'write CX2GUEL::ReservedSkillStart (MP + cool time at skill-state start), the standing rule', ('A3:214',))

# ---------------- section 7
add('7', AL(245, '| `322a7660`'), '322a7660 (BFI, count, level, x) / wrapper 322a8a70 callers: Elesis 32b7e96c / 32b7ebae (BFI 910 / 913)',
    '32b7e96c is inside 32b7e740 %s (Grand Master aura, BFI 909 / 910, class map %s); 32b7ebae is inside 32b7e9a0 %s (transcendence berserker chain BFI 911 / 912 / 913, class map %s); caller must pass x'
    % (cr('32b7e96c')[5], cp('32b7e96c'), cr('32b7ebae')[5], cp('32b7ebae')),
    'CH4a CH4c', 'CH4a: 32b7e96c in 32b7e740 (BFI 910, Grand Master aura); CH4c: 32b7ebae in 32b7e9a0 (BFI 913)', ('A1:242',))
add('7', AL(246, '`31fe8130` / `31fe8050` (user `AnimPlay`)'), 'user AnimPlay extra skin anims (+0x8e84 / +0x8e88), +0x8080 (every user)',
    'engine, every user; no Elesis address in the class range', 'n/a', 'engine', ('A3:243',))
add('7', AL(248, '`3208e160` (unit vslot 152'), 'second after-image +0x6758 / second head frame / second AFTER_IMAGE table',
    'engine, every user; %s; AFTER_IMAGE_COLOR 1 use in ELESIS_KNIGHT (script L%s, reader X2GUUser.cpp:17360 live)' % (vdesc('0x850'), ', '.join(map(str, src_lines('AFTER_IMAGE_COLOR')))),
    'n/a', 'engine; the script key arrives with CH1 final adoption', ('A3:245, 247, 248',))
add('7', AL(250, '+0x7c80` "after-image on by buff"'), '+0x7c80 buff after-image (every user)', 'engine, every user; no Elesis address', 'n/a', 'engine', ('A3:245, 247, 248',))
add('7', AL(251, 'After-image byte `+0x1d1`'), '+0x1d1 suspend / restore of the after-images (every user)', 'engine, every user; no Elesis address', 'n/a', 'engine', ('A3:245, 247, 248',))
sm = [r for r in rd('ch0_summon.tsv')[1:] if not r[0].startswith('SUPPORT')]
sumids = ', '.join('%s %s' % (r[4], r[5]) for r in sm)
add('7', AL(258, '`31fc8200` (`IsSummonSkill`'), 'IsSummonSkill 31fc8200 (54 June skill ids) and the summon cases of 3230c1a0 / 322ece70 (every class with a summon)',
    'her summon sites (ch0_summon.tsv): %s; CheckSummonSpecialAttack and the 54-id list already ported (X2GUUser.cpp:36995; ids 28018 / 28023 / 28054 / 28057 present at X2GUUser.cpp:28260-28263); '
    'SetUserSummonedNPCInfo cases 0x663 / 0x7b3 and the +0x8e74 writer 322ece70 are base code (ch0_summon.tsv)' % sumids,
    'CH3b0', 'spawn-rule phase before the skill phases (Chung CH3b0 precedent); NPC data037 ELESIS_FIRE_BLOSSOM.lua reads the related skill level', ('A3:255, 280',))
add('7', AL(259, 'Per-class code that writes the state power rate after `DoStateEndStart`'), 'per-class code scaling the cached state power rate after DoStateEndStart (June cache +0x7cc8)',
    'every class; her state callbacks are the carriers (class map phases CH1a-CH3d)', 'CH1a-CH3d', 'per state: keep June\'s *= / = in each state callback', ('A3:256',))
add('7', AL(261, '32b6ac80'), 'Crest of Time snapshot: vslot 0x740 per-class gauge override EL 32b6ac80 (pair 32b6acb0, vslot 0x744)',
    '%s; %s; class map %s / %s (StoreGageSnapshot / RestoreGageSnapshot); bases 32009780 / 3204da60' % (vdesc('0x740'), vdesc('0x744'), cp('32b6ac80'), cp('32b6acb0')),
    'CH1q', 'write her pair like Elsword\'s; the bases 32009780 / 3204da60 stay char-add\'s', ('A1:258',))
add('7', AL(265, 'Every class\'s vslot 510'), 'vslot 510 lookup prefix at the top of SpecialAttackEventProcess (509)',
    '%s; override 32b84a50 (class map %s)' % (vdesc('0x7f8'), cp('32b84a50')), 'CH1k', 'done in March (7d); the port keeps it', ('A1:262',))
add('7', AL(266, 'Every class\'s June skill-use check'), 'skill-use check (cool-time / MP / hyper-stone block) wrapped in JuneIsDevUnlimitedSkillUse()',
    'gate sits in the 510 lookup 32b84a50 (%s); SetEquippedSkillFuncBySkillSlotIndex 32b8c470 (%s) should have none' % (cp('32b84a50'), cp('32b8c470')),
    'CH1k', 'CH1n confirms 32b8c470 has none (as CH1n did for Ara)', ('A3:263',))
e187 = [r for r in reg if r[2] == '187'][0]
add('7', AL(267, '`32b90420` (EL, returns `0xbb`)'), 'GetSpecialItemUsableExtraStateID 32b90420 (EL, returns 0xbb), vslot 0x4ac',
    '%s; class map %s; June state 0xbb = id 187 = %s (ch0_reg.tsv, InitState part %s@%s)' % (vdesc('0x4ac'), cp('32b90420'), e187[3], e187[0], e187[1]),
    'CH1h', 'return the state by name (ESI_SWORD_WAIT), never the number; March\'s 0xbb is ESI_A_SGM_SNAP_CRASHER_GROUND', ('A1:264',))
add('7', AL(277, '`32011960( bool )`'), 'ally-summon focus target (32011960 / 31fd0590 / 32011ab0, user vslot 0x85c)',
    'every user, engine; no Elesis address; her summons are NPC 1567 / 1635 / 1641 / 1971 (ch0_summon.tsv)', 'n/a', 'engine', ('A3:274',))
b = blood['32b87880']
add('7', AL(278, '`32b87880` (buff Phase 9b, item 5)'), '32b87880 Blood Runaway: BFI 462 at the +0x9760 gauge, erases BTI 426 past stack 4',
    'ch0_blood.tsv: %s; class map %s; also push 7 of her delegate list (32b88300)' % (b[3][:230], cp('32b87880')), 'CH4c', 'CH4a declares the push in InitPassiveSkillState, CH4c writes the body', ('A1:275',))
add('7', AL(281, 'Spec A (option member `+0x6bb0`'), 'GetCharacterSpecializationA / B readers (Aisha, Eve, Rena: her June readers not named)',
    'her WSP MP factor reads them (CalculateLinearElesisSpecializationA/BValue, X2SocketItem.h:1439); class map CH1q holds her vslot 0x720 / 0x740 / 0x744 / 0x74 / 0x78 Way of Sword overrides',
    'CH1q', 'her Way-of-Sword MP factor is the 7an L952 row', ('A3:278',))
add('7', AL(283, '`3230c1a0` (`CX2Game::SetUserSummonedNPCInfo`'), '3230c1a0 SetUserSummonedNPCInfo (rewritten in June) via 322ece70 per-case flags (every class with a summon)',
    'her summon ids %s; June own cases for 1635 / 1971 (flags 1), default for 1641; the +0x8e74 writer 322ed3b8 inside 322ece70 is base code; ch0_summon.tsv' % sumids, 'CH3b0', 'with the IsSummonSkill row above', ('A3:255, 280',))
add('7', AL(286, '`3201ad10` (the skill-group power socket rate)'), '3201ad10 socket power rate callers by range',
    'byte scan of unpacked_x2.exe over 32b4e770-32b90b70 for call / jmp 3201ad10: %s (scan validated on the known Chung caller 32bec286)' % (', '.join(H3201AD10) if H3201AD10 else '0 callers'),
    'n/a' if not H3201AD10 else ' '.join(sorted(set(cp(h) for h in H3201AD10))), 'no caller in her range' if not H3201AD10 else 'callers ' + ', '.join(H3201AD10), ('A3:283',))
add('7', AL(289, '`3201f850` (`CX2GUUser::SetSkillLevelStateData'), 'SetSkillLevelStateData 3201f850: June multiplies the state power rate by ( 3201ad10( skill ) + 1.0 )',
    'March X2GUEL.cpp:2977 multiplies nothing (plan); class map CH1o holds the 4 functions that feed it (%s)' % ', '.join(r[0] for r in cm if r[6] == 'CH1o'), 'CH1o', 'the term inside March\'s class copy (Aisha CH1i / Ara CH1o precedent)', ('A1:286',))
add('7', AL(296, 'Class readers of the tutorial flag'), '32b74065 (CX2GUEL::DamageReact, Iron Will): the branch needs !tutorial',
    '32b74065 is inside 32b73ff0 DamageReact (class map %s; %s)' % (cp('32b74065'), vdesc('0x60') if '0x60' in vt else 'vslot 0x60 override'), 'CH1h', 'DamageReact 32b73ff0 whole', ('A1:293',))
add('7', AL(331, '`31fdbbb0` (skill-memo entry'), 'skill-memo 31fdbbb0 lookup behind user vslot 0x758 (every user)',
    'engine lookup; per-skill memo states in her script; class-side memo condition block is 32b80f20 (class map %s)' % cp('32b80f20'), 'CH2a-CH3d CH4b', 'per skill with the skill phases; MemoConditionElesis side = CH4b', ('A3:328',))
add('7', AL(333, 'the per-class blocks in funcmap rows `329bc560`, `32b80f20`'), '32b80f20 SetDamageExceptionProcess (vslot 0x2b8) per-class memo / passive block',
    '%s; class map %s; June-only @32b81008-32b811ef (skills 28416 / 28417)' % (vdesc('0x2b8'), cp('32b80f20')), 'CH4b', 'whole function', ('A1:330',))

# ---------------- 7b / 7e
add('7e', AL(363, 'C9c-2b'), 'C9c-2b retired nine March hand-made unit-viewer effects incl. SPK, SBH', 'done by C9c-2b (the table\'s 11 live rows drive them)', 'n/a', 'done; CH5 only confirms', ('A1:363',))
mq = rd('ch0_motion_question.tsv')
lob = [r for r in mq if 'LobbyReadyAction' in ' '.join(r)]
add('7e', AL(372, '`UnitViewerEffectSetTable.lua` row `SPK_LobbyReadyAction`'), 'UnitViewerEffectSetTable.lua row SPK_LobbyReadyAction (Pyro Knight)',
    'live June Motion_EL_Lobby.x spells SPK_LobbyReadyAction / SSK_LobbyReadyAction where March\'s X2UnitViewerUI plays ...Readyaction (ch0_motion_question.tsv: %d rows)' % len(lob),
    'CH5', 'CH5 checks the lobby call (the row and June\'s Motion_EL_Lobby.x are in since C9g-2)', ('A1:369',))
add('7e', AL(375, 'lost under June\'s player: the SBH / ALP end windows'), 'lost under June\'s player: SBH / ALP end windows, class-change mesh outline, avatar-capture effects',
    'Elesis among the classes', 'n/a', 'nothing to port, not a regression', ('A1:372',))

# ---------------- 7h
add('7h', AL(426, '`32024ff0` list variants'), '32024ff0 effect-set list variants (_TRANS_*, _FINAL, _FINAL_ULTIMATE) for every class with June scripts',
    'keys in ELESIS_KNIGHT: EFFECT_SET_LIST_EX %s uses, _APPLIED_SKILL %s, _NO_TRANS %s, _TRANS_1 %s, _ULTIMATE_EX %s, _ULTIMATE_EX_APPLIED_SKILL %s; engine readers live X2GUUser.cpp:49881-49923'
    % tuple(keys[x][2] for x in ('EFFECT_SET_LIST_EX', 'EFFECT_SET_LIST_EX_APPLIED_SKILL', 'EFFECT_SET_LIST_EX_NO_TRANS', 'EFFECT_SET_LIST_EX_TRANS_1', 'EFFECT_SET_LIST_ULTIMATE_EX', 'EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL')),
    'CH1p', 'engine readers live; the script rows ride CH1 final', ('A3:423',))
add('7h', AL(427, '`+0x38` (`RELATE_SKILL_ID`'), 'RELATE_SKILL_ID (Ain / Ara / Elesis / Raven / Rose scripts), user slots 0x4c4 / 0x4c8',
    '%s; %s; June ELESIS_KNIGHT.src.lua has %d RELATE_SKILL_ID lines (ch0_keys.tsv has no such key)' % (vdesc('0x4c4'), vdesc('0x4c8'), len(src_lines('RELATE_SKILL_ID'))),
    'CH1 final', 'data only: she overrides neither slot, so no slot port', ('A1:424',))
add('7h', AL(428, '`CUSTOM_STATE_DELETE_SOUND` data'), 'SOUND_PLAY_EX, STATE_SOUND_PLAY, CUSTOM_STATE_DELETE_SOUND data',
    'ELESIS_KNIGHT: CUSTOM_STATE_DELETE_SOUND %s use (script L%s), SOUND_PLAY_EX %s uses; engine readers ported (X2GUUser.cpp:17207, :17593)' % (keys['CUSTOM_STATE_DELETE_SOUND'][2], keys['CUSTOM_STATE_DELETE_SOUND'][4], keys['SOUND_PLAY_EX'][2]),
    'CH1 final', 'script adoption; nothing extra in code', ('A1:425',))
add('7h', AL(429, 'Class code writing `+0x3f00` directly'), 'class code writing +0x3f00 directly (32b77b30) and AddEffectSetDeleteCondition 3227a570 callers (32b7db80 x2)',
    '32b77b30: %s; 32b7db80: %s' % (cpn('32b77b30'), cpn('32b7db80')), cp('32b7db80'),
    '32b77b30 has no call, jmp or pointer reference in the exe (class map: dead, n/a); 32b7db80 is Avatar of the Fire FrameMove: register DCES_SKILL_STATE_END effect sets there', ('A1:426',))
add('7h', AL(430, '`USEFUL_VALUE0..4` per-state float rows'), 'USEFUL_VALUE0..4 per-state float rows; example reader 32b6ced0',
    'ELESIS_KNIGHT: USEFUL_VALUE0 %s uses in %s tables (first script L%s); 32b6ced0 = %s' % (keys['USEFUL_VALUE0'][2], keys['USEFUL_VALUE0'][3], keys['USEFUL_VALUE0'][4], cpn('32b6ced0')), cp('32b6ced0'),
    'phase of the reader state; the remaining uses ride their state phases', ('A1:427', 'A2:427', 'A3:427'))
add('7h', AL(431, '`USI_WAIT_STATE_CHANGE`'), 'USI_WAIT_STATE_CHANGE registered in every class InitState (all 12)',
    'ESI_WAIT_STATE_CHANGE: %s; registration in the InitState wrapper 32b87a60 (class map %s)' % (st['ESI_WAIT_STATE_CHANGE'][2], cp('32b87a60')), 'CH1m', 'the state callbacks are CH1a, the registration row is CH1m', ('A3:428',))

# ---------------- 7i / 7j
add('7i', AL(447, '`32b8c470` Elesis'), 'SetEquippedSkillFuncBySkillSlotIndex EL 32b8c470: drop the snapshot call, state byte +0x11 = 1 on normal and hyper entries',
    '%s; class map %s' % (vdesc('0x7b4'), cp('32b8c470')), 'CH1n', '', ('A1:444',))
add('7j', AL(457, '`32b7beb0` (CX2GUUser vslot `0x4f8`'), '32b7beb0 (vslot 0x4f8, attacker side): override of C10e\'s empty hook, called ( DD, damage )',
    'ch0_blood.tsv: %s ... getter vslot 0x724 = +0x9718 (SA_HP_INTAKE_REL_DAMAGE of RNSI_P_SDK_DARK_FORCE 28410, 32b903f0); class map %s' % (blood['32b7beb0'][3][:200], cp('32b7beb0')), 'CH4c',
    'CORRECTION to the plan: it is Dark Force\'s HP intake in Vigor (UpNowHp amount = 0x724 getter x damage), not the Way-of-Sword attacker damage effect', ('A1:454',))
add('7j', AL(458, '`32b898a0` (CX2GUUser vslot `0x4fc`'), '32b898a0 (vslot 0x4fc, attacker side): second empty hook override',
    'ch0_blood.tsv: Judgment bleeding + HP intake; +0x9748 = u16 level of RNSI_P_SDK_JUDGMENT 28411, +0x974c = its chance (ability 5), written by 32b88300; class map %s' % cp('32b898a0'), 'CH4c',
    'name the members from the script: judgmentLevel / judgmentChance', ('A1:455',))

# ---------------- 7l / 7n / 7o
add('7l', AL(475, 'Bounding user states'), 'bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP registered in every class InitState',
    'her registration inside InitState part 1 32b81be0 (class map %s) @32b83364 (funcmap 2173); <prefix>DAMAGE_BOUNDING_DOWN in her script' % cp('32b81be0'), 'CH1m', '', ('A3:472, 509',))
add('7l', AL(512, '`<prefix>DAMAGE_BOUNDING_DOWN`'), 'All 12 classes: <prefix>DAMAGE_BOUNDING_DOWN = -4000',
    'ELESIS_KNIGHT carries its ESI_DAMAGE_BOUNDING_DOWN (script L%s); registered by 32b81be0 @32b83364' % (', '.join(map(str, src_lines('DAMAGE_BOUNDING_DOWN'))) or '?'), 'CH1m', 'verbatim with the script', ('A3:472, 509',))
add('7n', AL(499, '| Elesis | `32b73bf0`'), 'class-skill float: writer 32b73bf0 (StateStart: base vslot 0x7c0, then float = 0), reader 32b7d550',
    '32b73bf0 = %s; 32b7d550 = %s (ch0_class_map.tsv names 32b73bf0 as the StartFuture of ESI_DASH_JUMP_COMBO_SDK_ZX_CHARGE)' % (cpn('32b73bf0'), cpn('32b7d550')), 'CH1g',
    'both are dash-jump combo states: the skill driving the float is the SDK ZX dash jump combo', ('A1:496',))
sp = src_lines(r'0\.527')
add('7o', AL(530, '**Elesis / Shay:**'), 'ELESIS_KNIGHT and PVP_HERO_SHAY each carry SPEED_TIME1 = { 150, 127, 0.527, 0.8, 2 } (iFlag (int)0.8 = 0 never fires)',
    'ELESIS_KNIGHT.src.lua L%d-%d in state %s (%s)' % (sp[0] - 4, sp[0] + 3, src_top(sp[0]), st[src_top(sp[0])][7]), 'CH1 final', 'carry verbatim, "keep it as June has it"', ('A1:527',))

# ---------------- 7s / 7t
add('7s', AL(582, 'X2DungeonManager.cpp:252'), 'X2DungeonManager.cpp:252-552 reads COMMON / ELSWORD / ... / ELESIS / ADD', 'she has her reader', 'n/a', 'nothing for her', ('A1:579',))
tm = rd('ch0_unittemplet_elesis.tsv')
hc = [r[0] for r in tm if len(r) > 2 and r[1] == 'm_HyperCutIn']
add('7t', AL(597, '**June\'s `UnitTemplet.lua`**'), 'every June class script sets the awakening key (ESI_*_HYPER_MODE); UnitTemplet.lua m_HyperCutIn per class',
    'June script tables ESI_EGM_HYPER_MODE L%s, ESI_EBH_HYPER_MODE L%s, ESI_ECA_HYPER_MODE L%s (X2GUUser.cpp:34184-34189 already names them); ch0_unittemplet_elesis.tsv m_HyperCutIn june-only rows: %s'
    % (src_lines('^ESI_EGM_HYPER_MODE')[0], src_lines('^ESI_EBH_HYPER_MODE')[0], src_lines('^ESI_ECA_HYPER_MODE')[0], ', '.join(hc)),
    'CH1 final CH5', 'CH1 final: the script keys; CH5: her UnitTemplet.lua rows (core-list)', ('A3:592-595',))
add('7t', AL(626, 'g4_cutin_names'), 'cut-in name tables: each class port replaces its table with June\'s names (g4_cutin_names.tsv)',
    'her March table is X2GUEL.cpp:58-73 (HQ_CutIn_Elesis*.dds, SGM / SBH sets)', 'CH5', 'swap her table for June\'s names', ('A3:603-632',))
add('7t', AL(619, '`USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE`'), 'USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE (ELESIS_KNIGHT, 3 states), SOUND_SET3_RATE, STATE_SOUND_PLAY<n> in a sound set',
    'ELESIS_KNIGHT: USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE %s uses (script L%s), SOUND_SET3_RATE %s use (L%s), SOUND_SET3 %s use (L%s); engine readers X2GUUser.cpp:17723 / :17735 live'
    % (keys['USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE'][2], ', '.join(map(str, src_lines('USE_PREVIOUS_STATE_SOUNT'))), keys['SOUND_SET3_RATE'][2], keys['SOUND_SET3_RATE'][4], keys['SOUND_SET3'][2], keys['SOUND_SET3'][4]),
    'CH1 final', 'script adopted; engine side done', ('A1:616-617',))

# ---------------- 7x / 7y / 7z
add('7x', AL(670, '### 7x. Combo hits must carry'), 'combos must carry AT_COMMAND; every class port takes June\'s script with its tags',
    'ELESIS_KNIGHT.src.lua: %d lines carry AT_COMMAND' % len(src_lines('AT_COMMAND')), 'CH1 final', 'script adoption', ('A3:667-673',))
add('7y', AL(685, '32b7a0a0'), 'GetLandPosition bMonster: June adds a call in an Elesis anim-event NPC create (32b7a0a0 @32b7a354, true, NPC 0x663 / 0x7b3)',
    '32b7a0a0 = %s; Wall of Prominence NPC 1635 (0x663) and Fire King 1971 (0x7b3), ch0_summon.tsv WALL-1 / WALL-2' % cpn('32b7a0a0'), 'CH3b', 'the skill that spawns NPC 1635 / 1971 (RNSI_SA_SBH_WALL_OF_PROMINENCE 28018 / 28054), after CH3b0', ('A1:682',))
add('7z', AL(705, '`UpdateEquipCollisionData`'), 'UpdateEquipCollisionData 31fd8c00 / 329657d0 pair exists for every class',
    'engine R14c; %s' % vdesc('0x638'), 'n/a', 'engine', ('A3:700-704',))
add('7ae', AL(728, 'Elesis and Add leave it blank'), 'census: an id with no March state falls into the class switch default:; "Elesis and Add leave it blank"',
    'CH0 appended 82 rows for classes 35 / 125 (53 distinct ids) to june2017_re/s3_reform_ids_no_state.tsv; ch0_state_skill.tsv part 2 lists her 127 June ids with phases', 'CH0 CH2a-CH3d',
    'CH0 done (census regenerated); the skill phases CH2 / CH3 fill the ids', ('A1:725',))

# ---------------- 7ad motions
stage = rd('ch0b_stage_list.tsv')[1:]
m206 = [r for r in stage if r[1] == 'data206']
held = rd('ch1_held_motions.tsv')[1:]
add('7ad', AL(762, 'SI_A_EL_LEAP_CRASH'), 'motions: 6 March animations missing under June\'s Motion_EL*.x (SI_A_EL_LEAP_CRASH_*, SI_A_EL_RUSHING_SWORD, two SSK names), frame count 79 vs 80',
    'ch0b_stage_list.tsv: new kom data206 holds %d June-only motion files + %d March copies (Common / Emotion / Field / PK); ch1_held_motions.tsv holds %s for CH1 final; ch0_motion_question.tsv: %d March animations not in any June motion file'
    % (len([r for r in m206 if r[3] == 'missing']), len([r for r in m206 if r[3] == 'march-copy']), ' and '.join(r[0] for r in held), len([r for r in mq if r[-1].startswith('not in any')])),
    'CH0b CH1 final', 'CH0b stages Common / Field / Emotion / PK and the 57 June-only motions (data206); Motion_EL_BH.x and MOTION_EL_GM.x wait for CH1 final; the 6 lost March names are renamed by June (SI_A_SSK_LEAP_CRASH_*, SI_A_SSK_RUSHING_SWORD, SI_SA_EL_SPIRAL_BLAST, SI_SA_SGM_VICTORIOUS_SWORD): the June script resolves them at CH1 final', ('A1:750-767',))

# ---------------- 7ag / 7ah / 7ai / 7aj
add('7ag', AL(811, '`CommonSpecialAttackEventProcess`\'s `iSkillSlotIndexPressed_`'), 'pressed-index space (March 4 + 4 -> June 5 + 5): CountUsedSkill, CurrentSkillKeyManager, m_kSkillSlotKey',
    'her CanUseSpecialAttack one caller X2GUEL.cpp:1809, CountUsedSkill X2GUEL.cpp:3157; the shared-reader stand-in (X2GUUser.cpp:26778-26797) lists her as unported', 'CH1p', 'she joins the June set under her _LIVE', ('A3:805-835',))
add('7ah', AL(844, 'Static_EL_SKILL_B'), 'Static_EL_SKILL_B(_B) / _R(_B) class statics driven by CX2Game::UpdateElswordSkillSlotEffect',
    'her UI 32e69a80 calls UpdateElswordSkillSlotEffect too (ch0_wos.tsv row 105: CX2ElesisMyGageUI Update 32e69a80); divergent row 427 done by char-elsword CH1q2', 'CH1q', 'nothing new; check her UI call', ('A3:841',))
add('7ai', AL(874, '`32b818b0`'), 'JuneShowSkillNameStateString 3200ab60: callers include 32b818b0 (skills 28201 / 28218, after 32b80c60)',
    '32b818b0 = %s (override 0x80c NoStateChangeActionFrameMove); 32b80c60 is called by it (Endurance Power states ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE ids 205 / 207, _MEMO 206)' % cpn('32b818b0'), 'CH1k', 'call it where June does', ('A1:868-869', 'A2:868'))
add('7ai', AL(876, '**First live use of the reserved-buff path.**'), 'first live use of the reserved-buff path: BUFF_FACTOR_ID_AND_APPLY_TIME in Elesis\'s script (Endurance Power, memo state, Flame Sword)',
    'ELESIS_KNIGHT: BUFF_FACTOR_ID_AND_APPLY_TIME %s uses in %s tables; Endurance Power states %s / %s [%s], Flame Sword state ESI_SI_A_SBH_FLAME_SWORD [%s] (28210); readers X2GUUser.cpp:48625 / :48763 live'
    % (keys['BUFF_FACTOR_ID_AND_APPLY_TIME'][2], keys['BUFF_FACTOR_ID_AND_APPLY_TIME'][3], 'ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE', 'ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO', st['ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE'][7], st['ESI_SI_A_SBH_FLAME_SWORD'][7]),
    'CH1 final CH3b2', 'CH1 final play-test (Endurance Power, CH1k states) shows the [JUNE_SKILL] SBUF parse / frame / state end lines; CH3b2 Flame Sword 28210', ('A1:870-871',))
add('7aj', AL(882, '**Equipped-slot icon overrides**'), 'equipped-slot icon overrides 328e0aa0 per class type (0x6d, 0x7b, 0x72, 0x73, 0x1a Eve, 0x7e, 0x24)',
    'her unit type is not obviously among them (not decoded); %s' % vdesc('0x56c'), 'CH5', 'confirm her unit type in June', ('A3:876',))

# ---------------- 7al
add('7al', AL(908, '**`+0x9440`, the MP a reserved skill spent**'), '+0x9440 (MP a reserved skill spent): read only by 32b60650 / 32b63be0',
    '32b60650 = %s; 32b63be0 = %s' % (cpn('32b60650'), cpn('32b63be0')), cp('32b60650') + ' ' + cp('32b63be0'), 'member in CH1; the two reader states (Critical Dive, Blazing Dance Finish) read it in their phases', ('A2:902-903',))
add('7al', AL(910, '**New CX2GUUser hooks for every class**'), 'new CX2GUUser hooks: JuneAfterEquipSlotChange 0x654, JuneAfterImageFrameMove 0x850, JuneSummonedNPCInfo 0x860, JuneReservedSkillStart 512, UpdateEquipCollisionData 0x638',
    '0x654 / 0x850 / 0x860 / 0x638 not overridden (%s | %s | %s | %s); ReservedSkillStart: %s' % (vdesc('0x654'), vdesc('0x850'), vdesc('0x860'), vdesc('0x638'), vdesc('0x800')), 'CH1k',
    'n/a for 0x654 / 0x850 / 0x860 / 0x638; ReservedSkillStart (32b73e60) is CH1k', ('A3:904-905',))
add('7am', AL(929, '| Elesis | `32b88300`'), 'delegate lists: Elesis registering function 32b88300; pushes 32b52470, 32b6fd50, 32b7e3b0, 32b524b0, 32b6f820, 32b872d0, 32b87880',
    '32b88300 = InitPassiveSkillState (%s); delegates: %s' % (cp('32b88300'), ', '.join('%s (%s)' % (a, cp(a)) for a in '32b52470 32b524b0 32b6fd50 32b6f820 32b7e3b0 32b872d0 32b87880'.split())), 'CH4a CH4c',
    'CH4a: 32b52470, 32b524b0, 32b6fd50, 32b6f820, 32b7e3b0, 32b872d0 via thunk-table adapters; CH4c: 32b87880 (Blood Runaway)', ('A1:923',))
add('7am', AL(938, 'March\'s own pushes'), 'March\'s own pushes (Elsword, Raven, Ara, Elesis, Add) are replaced by June\'s set', 'her March pushes live in X2GUEL.cpp InitPassiveSkillState', 'CH4a', 'replace with June\'s set', ('A1:932',))
add('7an', AL(948, '| vslot `0x428`'), 'vslot 0x428 JuneGetDefenceRateBySkill: EL 32b52260 (override, whole)', '%s; class map %s (GetDefenceRateBySkill)' % (vdesc('0x428'), cp('32b52260')), 'CH4b', 'the override, whole', ('A1:942',))
add('7an', AL(952, 'Way-of-Sword MP factor'), 'AttackResult\'s Way-of-Sword MP factor (32012b50 @32012d98, @32012ee3); Elesis\'s vslot 0x720 rate stands in at 1.0',
    '%s = +0x9714 (SA_WAY_OF_SWORD_POINT_MULTIPLIER of RNSI_P_SDK_DARK_FORCE 28410, default 1.0; getter 32b903e0, class map %s); GetWayOfSwordPointGainRate replaces the stand-ins X2DamageEffect.cpp:5543 and X2GUUser.cpp:14837' % (vdesc('0x720'), cp('32b903e0')),
    'CH1q', 'replace the stand-in with her vslot 0x720 override (base 3208e1b0 = 1.0)', ('A1:946',))

# ---------------- 7ao / 7ap / 7ar / 7av
add('7ao', AL(965, 'its cases in `JuneGetHyperModeTableName`'), 'every class also brings: transcend allow-list ids, LevelUpEvent.lua TYPE row, DLG_Transcendental_Change_Guide_<Class>.lua, JuneGetHyperModeTableName cases, hyper cut-in names, 18 SPRT_HYPER_CLASS + TRANSCENDENTAL_SKILL_DATA rows',
    'her ESI_EGM_ / ESI_EBH_ / ESI_ECA_ hyper tables already named in X2GUUser.cpp:34184-34189 (JuneGetHyperModeTableName); class 35 is not an awakening class', 'CH3d', 'with the transcendence skill phase', ('A3:954-960',))
add('7ao', AL(988, '| Elesis | `32b74300`'), '32b74300 (hyper effect start, vslot 49): the branch (returns at once when transcended)',
    '%s; class map %s' % (vdesc('0xc4'), cp('32b74300')), 'CH1h CH3d', 'CH1h ports it; CH3d tests the transcended unit', ('A1:981',))
add('7ap', AL(996, '0x3ac'), 'June slots with a base: 0x3ac / 0x3b0 / 0x56c / 0x584 / 0x65c / 0x6a8 (none listed for Elesis)',
    ' | '.join(vdesc(s) for s in ('0x3ac', '0x3b0', '0x56c', '0x584', '0x65c', '0x6a8')), 'n/a', 'none overridden', ('A3:987-997',))
add('7ar', AL(1016, 'June\'s `GetX2UnitClassCompatibility`'), 'GetX2UnitClassCompatibility 31ed6500: June cases for 33-46 / 123-136 / 200-202; 35 and 125 fall in that range',
    'the row does not name her; her 35 / 125 arms are added under SERV_IRUHADEV_JUNE_CLASS_PATH (Ara\'s pieces)', 'CHNa', 'reuses Ara\'s class-generic pieces', ('A3:1007-1011',))
add('7av', AL(1055, 'June\'s `ResetAnimSpeed`'), 'ResetAnimSpeed per-class blocks / user vslot 0x824 GetStateAnimSpeedRate', vdesc('0x824'), 'n/a', 'not overridden', ('A3:1046-1055',))

# ---------------- Part A2 leftovers
add('7', AL(240, '`32b90d20`'), 'Ara hyper-count row (32b90d20, vslot 147)', '32b90d20 is above her range end 32b90b60 (Ara\'s: starts 32b90b70)', 'n/a', 'NOT Elesis', ('A2:237',))
add('funcmap', 'funcmap.tsv L1420', 'funcmap row 32070e00 writer 32b45e0c named "CX2GUEL region"',
    '32b45e0c is in Lu/Ciel\'s range 32ae4080-32b4e76f; funcmap L1420 already carries the CH0 correction', 'n/a', 'Lu/Ciel\'s; the funcmap row is corrected, no Elesis action', ('A2:funcmap L1420',))

# ---------------- coverage + write
missing = sorted(plan_keys - covered)
extra = sorted(covered - plan_keys)
if missing:
    print('PLAN ROWS NOT COVERED:', missing)
if extra:
    print('TAGS NOT IN PLAN:', extra)
order = collections.OrderedDict()


def is_na(p):
    return p == 'n/a'


n_rows = len(R)
n_na = sum(1 for r in R if is_na(r[4]))
n_q = sum(1 for r in R if r[4] == '?')
with open(HERE + 'ch0_checklist_rows.tsv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(['section', 'checklist_line', 'row', 'elesis_evidence', 'phase', 'note'])
    for r in R:
        w.writerow([c.replace('\t', ' ').replace('\n', ' ') for c in r])
    f.write('# totals: %d rows = %d with a phase + %d n/a + %d ?; plan rows %d, uncovered %d: %s\n' % (n_rows, n_rows - n_na - n_q, n_na, n_q, len(plan_keys), len(missing), ' '.join(missing) or 'none'))
print('rows', n_rows, 'n/a', n_na, '?', n_q, 'plan rows', len(plan_keys), 'uncovered', missing, 'extra', extra)
print('3201ad10 callers in her range:', H3201AD10)
