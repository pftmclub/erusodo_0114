r"""char-ara CH0 item 4: build ch0_checklist_rows.tsv (Ara rows of JUNE2017_CHARACTER_PORT_CHECKLIST.md -> phase / n/a).
    python june2017_re/char_ara/ch0_build_checklist_rows.py
Copy of char_rena/ch0_build_checklist_rows.py with Ara's verdicts (no Rena verdict kept unless the same reasoning holds).
Ara: Ara_Martial_Artist, classes 7 / 28 / 31 / 34 (June-only) / 118 / 119 / 124 (June-only), June range 32b90b70-32bd86e6.
Line numbers are checked against the CURRENT checklist (1053 lines, 2026-11-10): AL( line, text ) stops the build when a line moved.
Evidence is read from ch0_state_skill.tsv, ch0_class_map.tsv, ch0_keys.tsv, ch0_enum_names.tsv, ch0_passive_ids.tsv, ch0_numtables.json,
ch0_vtable_slots.tsv, ch0_s3_baseline.txt, plus byte scans of the June exe (unpacked_x2.exe) over the Ara range for the
displacement / call-target rows (scan helpers inlined below).
"""
import csv, json, re, collections, os, bisect, struct
csv.field_size_limit(10 ** 9)
HERE = os.path.dirname(os.path.abspath(__file__)) + '/'
T = os.path.normpath(HERE + '../../') + '/'
CL = open(T + 'JUNE2017_CHARACTER_PORT_CHECKLIST.md', encoding='utf-8').read().split('\n')
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
LO, HI = 0x32b90b70, 0x32bd86e6


def AL(n, sub):
    """first line >= n-8 containing sub; prints a note when it is not n"""
    for k in range(max(1, n - 8), len(CL) + 1):
        if sub in CL[k - 1]:
            if k != n:
                print('NOTE: anchor %r expected L%d, found L%d' % (sub, n, k))
            return 'L%d' % k
    raise SystemExit('line %d moved: %r not found' % (n, sub))


def AR(a, b, sub):
    for k in range(a, b + 1):
        if sub in CL[k - 1]:
            return 'L%d-L%d' % (a, b)
    raise SystemExit('range %d-%d lacks %r' % (a, b, sub))


def rd(f):
    return [r for r in csv.reader(open(HERE + f, encoding='utf-8', newline=''), delimiter='\t') if r]


# ---- June exe byte scans (no disassembler needed for these)
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


_seg = _d[off(LO):off(HI)]


def disp_hits(vals):
    n = 0
    for dsp in vals:
        n += len(re.findall(re.escape(struct.pack('<I', dsp)), _seg, re.S))
    return n


def call_hits(target):
    out = []
    for i in range(len(_seg) - 5):
        if _seg[i] in (0xe8, 0xe9):
            rel = struct.unpack_from('<i', _seg, i + 1)[0]
            if (LO + i + 5 + rel) & 0xffffffff == target:
                out.append(LO + i)
    return out


# ---- evidence tables
cm = [r for r in rd('ch0_class_map.tsv') if re.fullmatch('[0-9a-f]{8}', r[0])]
cm.sort(key=lambda r: int(r[0], 16))
cm_ad = [int(r[0], 16) for r in cm]


def cp(a):
    """class-map phase of the function containing address a"""
    return cm[bisect.bisect_right(cm_ad, int(a, 16)) - 1][6]


st = [r for r in rd('ch0_state_skill.tsv') if not r[0].startswith('#') and r[0] not in ('state', 'skill_id') and len(r) > 7 and not r[0].isdigit()]
st_ph = {r[0]: r[7] for r in st if r[0].startswith('ASI_') or r[0].startswith('INIT')}
rows_b = [r for r in rd('ch0_s3_baseline.txt') if not r[0].startswith('#')]
hdr = [r for r in rows_b if r[0] == 'unit_class_name'][0]
data = [dict(zip(hdr, r)) for r in rows_b if r[0] != 'unit_class_name']
cls_of, name_of = collections.defaultdict(set), {}
for d_ in data:
    cls_of[int(d_['skill_id'])].add(int(d_['unit_class_value']))
    name_of[d_['skill_name']] = int(d_['skill_id'])
PH = collections.OrderedDict([(7, 'CH2a'), (28, 'CH2b'), (31, 'CH2c'), (34, 'CH2d'), (118, 'CH3a'), (119, 'CH3b'), (124, 'CH3c')])


def first_phase(sid, nm=''):
    if nm.startswith('RNSI_HA_'):
        return 'CH3d'
    return PH[min(cls_of[sid], key=lambda c: list(PH).index(c))]


# state-table lines of the June script, for the key rows
SRC = open(T + 'june_src_fixed/data036_kom/Ara_Martial_Artist.src.lua', encoding='utf-8', errors='replace').read().split('\n')
_top = None
_tops = []
for l in SRC:
    m = re.match(r'^([A-Za-z_0-9]+) = \{', l)
    if m:
        _top = m.group(1)
    _tops.append(_top)


def keylines(pat):
    return [(i + 1, _tops[i], st_ph.get(_tops[i], '?')) for i, l in enumerate(SRC) if re.search(pat, l)]


def kst(pat):
    return '; '.join('%s L%d [%s]' % (s, ln, p) for ln, s, p in keylines(pat))


ORDER = ['CH0b', 'CH1a', 'CH1b', 'CH1c', 'CH1d', 'CH1d2', 'CH1e', 'CH1f', 'CH1g', 'CH1h', 'CH1i', 'CH1j', 'CH1k', 'CH1m', 'CH1n', 'CH1o', 'CH1p',
         'CH1q', 'CH1q2', 'CH1 final', 'CH5', 'CHNa', 'CHNb', 'CH2a', 'CH2b0', 'CH2b', 'CH2b2', 'CH2c0', 'CH2c', 'CH2c2', 'CH2d', 'CH2d2', 'CH2d3',
         'CH3a', 'CH3a2', 'CH3b', 'CH3c', 'CH3c2', 'CH3d', 'CH4a', 'CH4b', 'CH6', 'Z']

R = []


def phases_ord(items):
    out = []
    for p in items:
        for q in p.split():
            if q not in out:
                out.append(q)
    return ' '.join(out)



def add(sec_, line, row, ev, phase, note=''):
    R.append((sec_, line, row, ev, phase, note))


def phases_of(items):
    s = set()
    for p in items:
        for q in p.split():
            s.add(q)
    return ' '.join(sorted(s, key=lambda x: ORDER.index(x) if x in ORDER else 999))


# ---------------- intro
add('intro', AL(4, 'Ara, Elesis'), 'June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, Add)',
    'Ara is a March class with a June update: 7 classes UC_ARA_MARTIAL_ARTIST 7, LITTLE_HSIEN 28, LITTLE_DEVIL 31, LITTLE_SPECTER 34 (June-only), SAKRA_DEVANAM 118, YAMA_RAJA 119, ASURA 124 (June-only); '
    'June range 32b90b70-32bd86e6 (class map: 1408 functions)', 'n/a', 'scope; the June-only classes 34 / 124 ride CHNa / CHNb')
add('intro', AL(14, 'Elsword_SwordMan.lua'), 'character script Ara_Martial_Artist.lua feeds SetDamageDataFromLUA',
    'June src 338 top-level tables (331 states + 6 INIT_* + PROMOTION_ITEM_ID_LIST; 319 mapped, 12 june-dead-data n/a, 7 non-state; ch0_state_skill.tsv); March data036 Ara_Martial_Artist.lua 148,831 B bytecode (1B 4B 4C 81, kom_contents), June 250,093 B (1B 4B 4C 82); '
    'not on the core list (tools/data/lua_core_list.tsv names Ara in no row)',
    'CH1 final', 'adopt-june (June bytecode via ljconv) after the states exist; add a core-list row first only if a phase must edit the script (none so far)')

# ---------------- section 1
nt = json.load(open(HERE + 'ch0_numtables.json'))
for o in nt:
    ln, top, tl, tname, tstart, idx, app, apptop, dam = o
    nm = app[0].replace('SKILL_ID.', '')
    sid = name_of[nm]
    owners = sorted(cls_of[sid])
    ph = st_ph.get(top) or first_phase(sid, nm)
    add('§1', AL(55, 'Ara_Martial_Artist'), 'numerical-only %s table in state %s (script L%d; NUMERICAL_DATA_INDEX = %s at L%d)' % (tname, top, tstart, idx, ln),
        'APPLIED_SKILL_ID %s %d (classes %s); no DAMAGE row in the block' % (nm, sid, ', '.join(map(str, owners))),
        ph, 'NUM use line from the play-test; phase = ch0_state_skill.tsv phase of the state (skill owner class phase %s); state %s starts at script L%d' % (first_phase(sid, nm), top, tl))
nph = sorted(set(st_ph.get(o[1]) or first_phase(name_of[o[6][0].replace('SKILL_ID.', '')]) for o in nt), key=lambda x: ORDER.index(x))
add('§1', AL(55, 'Ara_Martial_Artist'), 'numerical-only DAMAGE_DATA + numerical override live (the 14 tables above; gap 14 of charport_scan 2026-11-10)',
    'Ara_Martial_Artist 14 numerical-only + 49 other (plan_charport_scan_20261110); plan_numerical_rows.tsv: 160 numerical blocks over 59 distinct Ara ids (27000-27602), all result=ok',
    ' '.join(nph), 'each skill phase checks its ids reach the override (NUM use lines)')
add('§1', AL(91, '**Three blocks**'), 'blocks that index past their skill data need a JUNE_BUGS row',
    'plan_numerical_rows.tsv: 0 rows with a result other than ok (160 ok)', 'n/a', 'no zero-row JUNE_BUGS entry for Ara')
add('§1', AL(99, 'SKILL_NUMERICAL_DEBUG'), 'retire SKILL_NUMERICAL_DEBUG', 'already retired by Aisha CH2a (row l.100)', 'n/a', '')

# ---------------- section 2
napp = len(keylines('APPLIED_SKILL_ID'))
numln = {o[0] for o in nt}
beside = [x for x in keylines('APPLIED_SKILL_ID') if not any(abs(x[0] - n) <= 10 for n in numln)]
add('§2', AL(116, '`APPLIED_SKILL_ID`'), 'APPLIED_SKILL_ID (parsed since 8c)',
    '%d uses in Ara_Martial_Artist over 12 tables (ch0_keys.tsv): 14 numerical-only blocks + %d beside a DAMAGE row (%s); engine reader done' % (napp, len(beside), '; '.join('%s L%d' % (s, ln) for ln, s, p in beside)),
    phases_of(nph + ['CH2d3']), 'with the section 1 rows by owner')
add('§2', AL(117, '`NUMERICAL_DATA_INDEX`'), 'NUMERICAL_DATA_INDEX', '14 uses = the 14 section 1 tables (script L%d ... L%d)' % (nt[0][0], nt[-1][0]), phases_of(nph), '= the section 1 rows')
add('§2', AL(118, '`NATURAL_FORCE_TYPE`'), 'NATURAL_FORCE_TYPE x77 in Lire_ElvenRanger', 'Rena only; none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(119, 'HIT_ADD_COMBINATION_POINT'), 'HIT_ADD_COMBINATION_POINT (Luciel_Duo only)', 'none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(120, 'ADDITIONAL_ATTACK_TRIGGER_TYPE'), 'ADDITIONAL_ATTACK_TRIGGER_TYPE (Raven only)', 'none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(121, 'BUFF_FACTOR_ID_ULTIMATE'), 'BUFF_FACTOR_ID_ULTIMATE x3 in Ara_Martial_Artist ("to-implement, 8b group 2")',
    '3 uses in 3 tables: %s. Reader: CX2DamageManager::ParsingBuffFactorID reads BUFF_FACTOR_ID_ULTIMATE at X2DamageManager.cpp:5204, but only through June\'s 31fcf3e0 memo-equipped gate, '
    'which the comment at X2DamageManager.cpp:5116-5123 (written 2026-10-05, before S3) says is false here, so the key may be inert; the row\'s "to-implement" status is out of date (reader present, gate off)' % kst('BUFF_FACTOR_ID_ULTIMATE'),
    'CH3a', 'script data arrives with CH1 final; CH3a (Turbulent Wave) checks whether the gate is live when it plays the skill, a stand-in note otherwise')
add('§2', AL(122, 'IGNORE_DEFENCE_RATE'), 'IGNORE_DEFENCE_RATE and _ON_PVP (Raven only)', 'none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(123, 'IGNORE_DEFENCE_DECLINE'), 'IGNORE_DEFENCE_DECLINE (ELESIS_KNIGHT only)', 'none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(124, 'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID'), 'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID and _RELATIVE_SKILL_ID (Elsword only)', 'none in Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AR(110, 126, 'DAMAGE_RATE_TO_SUPERAMOR'), 'other DamageData keys (the bold rows l.125-126: _RELATIVE_BUFF_TEMPLATE_ID, DAMAGE_RATE_TO_SUPERAMOR)', 'none used by Ara_Martial_Artist (ch0_keys.tsv)', 'n/a', '')
add('§2', AL(140, '`DAMAGE_SKILL_ID`'), 'DAMAGE_SKILL_ID x103, "all in Elsword_DamageData.lua"', 'x103 are Rose ids; Ara_Martial_Artist.src.lua has zero', 'n/a', 'Rose (Stage 8) data')

# ---------------- section 3-4
add('§3', AL(148, 'Numerical skill data'), 'Numerical skill data: absent, "every June character (section 1)"', 'engine side ported (S4); Ara\'s 14 numerical-only tables = the section 1 rows', 'n/a', 'covered by section 1')
add('§3', AL(150, 'Applied skill ID'), 'Applied skill ID: absent, every character (167 uses)', '16 Ara uses (the section 2 row); the numerical override is live (S4); the _ULTIMATE gate 31fcf3e0 is the l.121 row', 'n/a', 'covered by section 1 / the l.121 row')
add('§4', AL(166, '`CUSTOM_RENDER_STATE`'), 'CUSTOM_RENDER_STATE enum table: Add x7, Ara x1 ("statement 338 of 338, ASI_SA_ASR_FOX_PHANTOM")',
    'script: ASI_SA_ASR_FOX_PHANTOM [CH3c] holds the CUSTOM_RENDER_STATE key (L21006, CRS_* value L21009); live data036 Enum.lua (BOM source) has 0 CUSTOM_RENDER_STATE hits, so the statement raises when CH1 final adopts the script (last statement: only ASI_SA_ASR_FOX_PHANTOM, class 124, is lost); '
    'plan_charport_scan_20261110 ENUM line for Ara: CUSTOM_RENDER_STATE x1 only (no other missing table)',
    'CH3c', 'CH0 decision: CH3c adds the Enum table / labelled stand-in until june-engine R5 (7w); CH1 final expects the one load error; see the l.659-666 rows')
add('§4', AL(170, 'five missing tables'), 'the effect files\' five enum tables (NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE, ... ), Phase 11\'s', 'plan_charport_scan_20261110 ENUM line for Ara lists only CUSTOM_RENDER_STATE', 'n/a', 'Phase 11 done; no table to add for Ara')
ec = collections.Counter(r[4] for r in rd('ch0_enum_names.tsv') if not r[0].startswith('#') and r[0] != 'kind')
cc = collections.Counter(r[7] for r in rd('ch0_enum_names.tsv') if not r[0].startswith('#') and r[0] != 'kind')
sphases = [r[7] for r in st if r[7] != 'n/a'] + [r[5] for r in rd('ch0_state_skill.tsv') if len(r) > 5 and r[0].isdigit()]
add('§4', AL(178, 'Missing members of tables March *does* have'), 'June-only members of tables March has (SKILL_ID names, ARA_STATE_ID) fail silently',
    '139 distinct Ara skill ids (ch0_enum_names.tsv): C++ enum name %d / lookalike %d / none %d; core Enum.lua name %d / absent %d; 0 ids lack a June name. '
    'ARA_STATE_ID (ch0_state_enum_names.tsv): one C++ enum (X2GUAra.h:57-252) holds every June prefix (125 live members), June Enum.lua lists 38 names; registered June states not in it: ASI_ 131 + USI_ 5 + other 2; '
    'the SI_ ids March code reads that differ from June\'s tree ids: ch0_passive_ids.tsv (21 DIFFERS + 20 DIFFERS(note) + 12 not live in the cl /P stream, 12 same)' % (ec['name'], ec['lookalike'], ec['none'], cc['name'], cc['absent']),
    phases_of(sphases), 'house convention: raw id + one-line RNSI_ / state-name comment per case; passive ids (types 1, 8) = CH4a, hyper actives (type 13) = CH3d')

# ---------------- section 5 keys
kc = collections.Counter(r[11] for r in rd('ch0_keys.tsv') if r[11] != 'verdict')
add('§5', AL(184, 'Every non-DamageData key'), 'per-character parser census', 'ch0_keys.tsv: %d keys; june-only %d, neither %d, march-only-site %d, both %d' % (sum(kc.values()), kc['june-only'], kc['neither'], kc['march-only-site'], kc['both']), 'CH1p', 'per-key owners follow')
add('§5', AL(184, 'Every non-DamageData key'), 'keys FORCE_DYNAMIC_CAMERA_INDEX x15, RETAIN_CAMERA_STATE x1', 'reader 32057b80 CX2GUUser::CommonStateStart; March has no reader', 'n/a', 'engine camera keys (Stage 9; as Chung / Elsword / Raven / Rena)')
add('§5', AL(184, 'Every non-DamageData key'), 'buff sub-table keys BBT_CHANGE_NOW_HP_PER_SECOND x7, BFT_TIME x7', 'read by stringized type name on both sides (ch0_keys.tsv: readers outside the Ara range)', 'n/a', 'as Chung / Elsword / Raven / Rena')
add('§5', AL(184, 'Every non-DamageData key'), 'key IMMUNITY_EDT_LIST_AT_THIS_STATE x3', 'states %s; June reader 31fdaaa0 (per-state parse in CX2GUUser); March has none (ch0_keys.tsv)' % kst('IMMUNITY_EDT'), 'n/a', 'backlog Stage 2 per-state immunity lists (as Chung / Elsword / Raven / Rena)')
add('§5', AL(184, 'Every non-DamageData key'), 'INIT_COMPONENT keys FORCE_POWER_COUNT_AT_DUNGEON_START, _AT_PVP_START, FULL_HYPER_MODE_ADVANTAGE_COUNT / _INTERVAL_TIME / _SOCKET_RATE, GAIN_FORCEPOWER_USEDHYPERCOUNT, GAIN_MP_BY_FPCONVERSION_HYPERMODE (7 keys x1)',
    'June-only (ch0_keys.tsv); read by InitComponent 32bb0da0 (override 0x3c, class map CH1h); members written under _FP', 'CH1h', 'CH0 decision: the 7 INIT_COMPONENT keys = CH1h (reads under _FP; members and the force-power use are CH1q)')
add('§5', AL(184, 'Every non-DamageData key'), 'class keys IS_SPECTER_STATE x40, TAIL_TYPE x1 (+ ANIM_NAME_TAIL x7, TRANSITION_TAIL x7)',
    'IS_SPECTER_STATE on 40 states, loader 320506f0 @320523c2 / 32057b80 @3205866c, one class reader 32bceca0 @32bcee1c (ch0_specter.tsv); TAIL_TYPE in PROMOTION_ITEM_ID_LIST; ANIM_NAME_TAIL / TRANSITION_TAIL read by ChangeAnimTail 32bd7280', 'CH1q2', 'CH0 decision: IS_SPECTER_STATE -> CH1q2; the states that carry the keys ride their own phases (script data)')
add('§5', AL(184, 'Every non-DamageData key'), 'class keys ADD_FORCE_POWER x9, USE_FORCE_POWER x26, FP_CANCEL x36, BERSERK x40, APPLY_MEMO x2, BERSERK sound (USE_BERSERK_SOUND x39)',
    'read by both sides (ch0_keys.tsv "both"); CommonStateStart 32bd2a40 / 32bd2a40-family reads (CH1j with _FP lines); plan CH1p gives the June-only script keys FP_CANCEL / ADD_FORCE_POWER / BERSERK / APPLY_MEMO an owner; APPLY_MEMO x2 sits in the dead tables ASI_SA_ALS_SPECTER_OF_BLADE / _FINISH (june-dead-data, n/a)',
    'CH1p CH1j CH1q', 'plan CH1p: keys; CH1j: the CommonStateStart reads; CH1q: the members and helpers')
add('§5', AL(184, 'Every non-DamageData key'), 'neither-side keys HEAD_IK x9 and the element keys EARTH / FIRE / ICE / LIGHTNING / UNIVERSAL x1 each', 'no reader on either side (ch0_keys.tsv)', 'n/a', 'keep verbatim in the script')
add('§5', AL(184, 'Every non-DamageData key'), 'march-only-site keys AFTER_IMAGE x24, READY_SOUND, STATE_STRING x6',
    'engine reads them on both sides; no key drops a March reader\'s input (ch0_keys.tsv)', 'n/a', 'census artifacts')
add('§5', AL(184, 'Every non-DamageData key'), 'keys HYPER_MODE_COUNT x1, MAX_HYPER_MODE_TIME x1, SHOW_HYPER_CUT_IN_TIME x6, SECRET_SKILL_STATE_ID x12, TRIGGER_STATE_ID_TO_SECRET_SKILL x12',
    'INIT_COMPONENT / INIT_SYSTEM keys both parsers read (ch0_keys.tsv both)', 'n/a', 'script data')

# ---------------- section 6
add('§6', AL(208, 'Only then bring in'), 'adopt the character\'s June script with luasrc.py adopt-june, rerun charport_scan.py', 'Ara_Martial_Artist.lua is not on the core list; plan CH1 final adopts it', 'CH1 final', 'see the l.14 row')
add('§6', AL(214, '**Standing check'), 'standing check: the class overrides ReservedSkillStart (June 32057b80 @32057c4c-cb2) and extends the X2GUUser chain; InitPassiveSkillState reads RNSI_*',
    'Ara overrides vslot 0x800 (512) = 32bb1ff0 (class map CH1k: per-class skill use / cooldown part of SpecialAttackEventProcess); InitPassiveSkillState 32bcd9c0 (CH4a)', 'CH1k', 'CH0 decision: ReservedSkillStart = CH1k; the hand-off asks two casts per skill')
add('§6', AL(216, 'Pilot close-out'), 'mark rule: a character port marks the rows it did with (Ara: CHn)', 'process rule for the closing phase', 'Z', 'Z marks the checklist rows')

# ---------------- section 7 intro table
add('§7', AL(236, '`32bb22d0` / `32bb2740`'), 'Ara hyper effect start / end 32bb22d0 / 32bb2740 (vslots 49 / 50): buff hyper count loop, End writes the count to 0',
    'vtable 0xc4 / 0xc8 overrides, class map %s / %s; the row\'s transcended return is the l.978 row' % (cp('32bb22d0'), cp('32bb2740')), 'CH1q', 'CH0 decision: force-power / berserk rows = CH1q (March has both functions, June reworked them: whole)')
add('§7', AL(237, '`32ba6cc0`'), 'buff hyper count readers / writers: 32ba6cc0 (vslot 39), 32ba6d00 (vslot 147), 32badc80 (485), 32baf270 (397), 32bc8be0 (198), 32bc8030 (17)',
    'class map: 32ba6cc0 IsFullHyperMode %s; 32ba6d00 SetSpecificValueByBuffTempletID %s (ch0_fp: hyper cases CH1q, BTI 423 / 424 cases CH1q2); 32badc80 %s; 32baf270 %s; 32bc8be0 %s (hook of 32bc5bc0, ch0_fp CH1q); 32bc8030 ReInit %s (ch0_fp: CH1h with _FP lines). '
    'TEXT WRONG: the row says vslot 147 maps "count 1 -> state 0x294, 2 -> state 0x295"; the exe pushes 0x294 / 0x295 / 0x9c as the BFI argument of vslot 0x21c (SetBuffFactorToGameUnitByBuffFactorID) = BFI_ARA_FIRST_HYPER_MODE 660 / BFI_ARA_SECOND_HYPER_MODE 661 / BFI_ARA_FULL_HYPER_MODE 156 (s3e_bybfi_callers.tsv @32ba6df1 / @32ba6e16 / @32ba6e40)'
    % (cp('32ba6cc0'), cp('32ba6d00'), cp('32badc80'), cp('32baf270'), cp('32bc8be0'), cp('32bc8030')),
    phases_ord(['CH1q', cp('32ba6d00'), cp('32bc8be0'), cp('32bc8030'), cp('32baf270'), cp('32badc80')]), 'CH0 decision: l.236-237 = CH1q (the first function of each group); the other functions go by their class-map phase (the 485 read rides the l.284 row, CH4b)')
add('§7', AL(242, '`322a7660`'), 'SetBuffAccumulationCount / AddBuffAccumulationCount (322a7660 / 322a8a70) callers: Eve, Elesis, Rose, Luciel, Aisha, CX2GUUser code', 'byte scan of the Ara range: 0 calls to 322a7660 / 322a8a70 / 3227c360 / 3236a2f0', 'n/a', 'Ara is not a caller (the add-N-stacks path is the l.1031 row: 3229da40)')
add('§7', AR(243, 248, 'every user'), 'user AnimPlay extra skin anims, second after-image modes, +0x7c80 buff after-image, +0x1d1 suspend-restore, bone rotation (l.243, 245-248)',
    'every user, engine (4b-iii hand-off); Ara state functions read the inlined dual-body anim pick (+0x8e90 byte / +0x8e84, 335 / 437 sites) which the March single body already stands in for (7o / R14); 0 hits for +0x8e80 / +0x7c80 / +0x941c / +0x8080 in the Ara range', 'n/a', 'engine owners')
add('§7', AL(253, '`32a07dc0` (Add)'), 'class skill-use function sets the cooldown then calls ApplySkillCoolTimeRateByBuff (the other 9 classes wired in Phase 6b)',
    'Ara is already wired in March (X2GUAra.cpp:3957 GetUnit()->ApplySkillCoolTimeRateByBuff( pSkillTemplet->m_eID )); June caller 3235a710 @32bb2268 sits in 32bb1ff0 (0x800, class map %s)' % cp('32bb1ff0'), 'CH1k', 'rides with the slot-512 skill-use body 32bb1ff0')
add('§7', AL(255, '`31fc8200`'), 'IsSummonSkill (54 June skill ids) and the summon cases of 3230c1a0 that March lacks', 'Ara has no summons (CH0): X2GUAra.cpp / .h have 0 CreateNPC / CreateNPCReq / GetNearestNPCUnitUID; 0 calls to 3230c1a0 / 322ece70 / 3204e160 / 3200bec0 in the Ara range; the l.255 NPC ids are other classes\'', 'n/a', 'no summon case for Ara')
add('§7', AL(256, 'Per-class code that writes the state power rate'), 'class code scales the cached state power rate (+0x7cc8)',
    'a byte scan of the June exe over 32b90b70-32bd86e6 for the displacements +0x7cc8 / +0x7ccc / +0x7cd0 finds %d hits (the same scan over Rena\'s range finds exactly her three functions); March X2GUAra.cpp writes no m_fNowStatePowerRate: its only power-rate store is stateData_.m_fPowerRate in SetSkillLevelStateData (:3001 / :3006)' % disp_hits([0x7cc8, 0x7ccc, 0x7cd0]), 'n/a', 'no Ara function writes the cached state power rate')
add('§7', AL(257, '`329e31e0` (Add)'), 'ChangeMaxCharacterAddResource Ara 32baafc0 (vslot 201 +0x324): force power x rate, +0x9644 over-max via vslot 60, June SetForcePower 32ba6ef0, skill 27417 learned -> vslot 143',
    'class map: 32baafc0 override 0x324 %s; 32ba6ef0 SetForcePower helper %s (spec-B reader 32ba6f31 inside); 27417 = RNSI_P_AYR_OPEN_MIND' % (cp('32baafc0'), cp('32ba6ef0')), 'CH1q', 'CH0 decision: force-power parts of l.257 = CH1q; needs a member for +0x9644')
add('§7', AL(258, '`320072d0` and `3202b9a0`'), 'death prevention (Ara pieces): skill 27409 learned with +0x867c -> +0xaf0 type 7 and 32bc5a50 -> force-invincible, +0x8e74 = 0x27',
    '27409 = RNSI_P_AYR_RESURRECTION (type 8 passive, class 119, CH4a in ch0_state_skill part 2). 32bc5a50 is CLASS-MAP-MISLABELLED: ch0_class_map.tsv has it as "not-Ara, n/a, shared / folded code" (called only from 32007353 in 320072d0), but its body reads CX2GUAra +0x96c0, the gauge force power (32a31a00 >= 5) and skill 0x6b11 = 27409: it is Ara\'s resurrection test, so it needs a CH4a row', 'CH4a', 'CH0 decision: 27409 death prevention = CH4a; fix the class map row of 32bc5a50 (n/a -> CH4a)')
add('§7', AL(258, '`320072d0` and `3202b9a0`'), 'Crest of Time snapshot: vslot 0x740 per-class gauge override Ara 32ba4840 (and 0x744 restore, base 3204da60)',
    'vtable 0x740 = 32ba4840 (class map %s), 0x744 = 32ba7b50 (%s); Ara has her own gauge override (CX2AraGageData), so write both as StoreGageSnapshot / RestoreGageSnapshot (Elsword CH1q / Rena CH1q precedent)' % (cp('32ba4840'), cp('32ba7b50')), 'CH1q', 'CH0 decision: force-power parts of l.258 = CH1q')
add('§7', AL(262, 'Every class\'s vslot 510'), 'every class\'s vslot 510 lookup (Phase 7d put the quick-slot prefix at the top of the eight March classes\' SpecialAttackEventProcess: Ara among them)',
    'Ara is one of the eight (X2GUAra.cpp:3594 IsQuickSlotSpecialItemWaiting in CanUseSpecialAttack); June lookup body 32bcfaa0 (override 0x7f8, slot 510, class map %s)' % cp('32bcfaa0'), 'CH1k', 'keep the prefix in June\'s 510 body 32bcfaa0')
add('§7', AL(263, 'Every class\'s June skill-use check'), 'dev free-skill gate around the cool-time / MP / hyper-stone block of the skill-use check',
    'March block X2GUAra.cpp:3775-3783 (#ifdef TOGGLE_UNLIMITED_SKILL_USE) in CanUseSpecialAttack; June skill-use parts 32bcfaa0 (slot 510), 32ba6610 (511), 32bb1ff0 (512), class map CH1k; 32bd09d0 must be confirmed to have no such block', 'CH1n CH1k',
    'CH0 decision: dev free-skill gate = CH1n (JuneIsDevUnlimitedSkillUse(), standing rule 2026-10-22; CH1n also confirms 32bd09d0 has none); the lookup body is CH1k')
add('§7', AL(265, 'The same vslot 510 copies'), 'riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup (Ara and Elesis hold it in CanUseSpecialAttack)',
    'Ara carries it (X2GUAra.cpp:3637 IsRidingKeySkillWaiting, JUNE_BUFF_RIDING_KEY_WAIT); June 3202cba0 @32bcfcfa sits in 32bcfaa0 (class map %s)' % cp('32bcfaa0'), 'CH1k', 'keep in June\'s 510 lookup 32bcfaa0')
add('§7', AL(267, '`32bd32f0 @32bd339e-32bd3454`'), 'CX2GUAra::GetReduceDebuffDurationRate 32bd32f0 @32bd339e-32bd3454 (vslot 167): skill 27415 (RNSI_P_ASD_OPEN_MIND) tail',
    'vtable 0x29c = 32bd32f0 (class map %s); March X2GUAra.cpp:13933; 27415 = RNSI_P_ASD_OPEN_MIND (class 118, CH4a); the tail\'s 31fceab0 @32bd33c1 and 32007b10 @32bd3447 are in range' % cp('32bd32f0'), 'CH4a', 'CH0 decision: l.267 = CH4a (the function is class map CH1h, the 27415 tail is the passive piece)')
add('§7', AL(274, '`32011960( bool )`'), 'ally-summon focus target (32011960 / 31fd0590 / 32011ab0, user vslot 0x85c): every user, a June-only CX2GUUser system', 'byte scan of the Ara range: 0 calls to 32011960 / 31fd0590 / 32011ab0; Ara does not override 0x85c / 0x860; no Ara summons', 'n/a', 'engine; empty virtuals')
add('§7', AL(278, 'Spec A (option member'), 'spec A / spec B readers: Ara 32bc9a60 (A), 32ba6ef0 (B)',
    'class map: 32bc9a60 helper called by 32bceca0 %s (ch0_fp.tsv: "(new) berserk hit advantage roll", CH1q); 32ba6ef0 SetForcePower %s' % (cp('32bc9a60'), cp('32ba6ef0')), 'CH1q', 'CH0 decision: l.278 / l.943 spec readers = CH1q')
add('§7', AL(280, '`3230c1a0`'), 'SetUserSummonedNPCInfo summon stats for every class with a summon', 'Ara has no summons (see the l.255 row); 0 calls to 3230c1a0 / 322ece70 in the Ara range', 'n/a', '')
add('§7', AL(283, '`3201ad10`'), '3201ad10 skill-group power socket rate called from class code (Eve, Add, Rose, Chung, Elsword ranges)',
    'byte scan of the Ara range: %d calls to 3201ad10; plan CH4b re-checks "if an Ara function calls 3201ad10"' % len(call_hits(0x3201ad10)), 'n/a', 'no Ara caller; the SetSkillLevelStateData term (l.286) is the one place Ara reads the rate (it is the base 3201f850\'s)')
add('§7', AL(284, '`32b0e670`'), 'vslot 485 override 32badc80 (Ara vtable 33505314): skill-group power socket pass 31fe14f0 over the list 32d3f040',
    'vtable 0x794 = 32badc80 (class map %s); base slot 33093ad0 is the empty hook' % cp('32badc80'), 'CH4b', 'CH0 decision: vslot 485 l.284 = CH4b; ports with the l.237 485 read')
add('§7', AL(286, '`3201f850`'), 'SetSkillLevelStateData hoisted into CX2GUUser; March copy X2GUAra.cpp:2989; multiply the state power rate by ( 3201ad10( skill ) + 1.0 ); callers 32bd358d..32bd3db2',
    'March copy at X2GUAra.cpp:2989 (current; the row\'s line is right) with m_fPowerRate stores :3001 / :3006; June callers: %d calls to 3201f850 in the Ara range, ALL inside 32bd34c0 (override 0x68c, class map %s), spanning %s..%s. TEXT INCOMPLETE: the row names only ..32bd3db2 (the first 18 of them)' % (len(call_hits(0x3201f850)), cp('32bd34c0'), '%08x' % min(call_hits(0x3201f850)), '%08x' % max(call_hits(0x3201f850))), phases_ord(['CH1o', cp('32bd34c0')]),
    'CH0 decision: l.286 = CH1o (add the term or hoist); the 0x68c override 32bd34c0 (InitAquiredActiveSkillState) that holds the callers is its own class-map phase and calls it')
add('§7', AL(293, 'Class readers of the tutorial flag'), 'tutorial flag in CX2GUAra::InitComponent 32bb0e2c: tutorial -> force power 3 (32a32640( 3 ); March handles PvP and training only)',
    '32bb0e2c lies in 32bb0da0 (InitComponent override 0x3c, class map %s); c14c_tutorial_reads.tsv row 94; 32a32640 is called @32bb0e61; March X2GUAra.cpp:3017 (sets FP 2 for dungeon and training)' % cp('32bb0e2c'), phases_ord(['CH1q', cp('32bb0e2c')]),
    'CH0 decision: l.293 = CH1q; the line itself is written in CH1h inside InitComponent under _FP (plan CH1h scope names it too); read g_pMain->GetIsPlayingTutorial() directly')

# 7a
add('§7a', AR(304, 311, 'modifier'), 'AddDamageEffectModifier list (tags 5 / 12 / 16 / 20 ...) and the attach-buff-to-effect-set variants',
    'byte scan of the Ara range: 0 calls to 321db050 / 321dad70 / 321dae90 / 321db1b0 / 321e3650 / 321e3730 / 321e3830 / 321f4690; no Ara row in l.305-311', 'n/a', 'Ara pushes no modifier (checks every phase\'s listing)')
add('§7a', AL(312, 'CEffect `+0xb18` writer'), 'CEffect +0xb18 writer 32bb0bd6 (in a function called from 32bbcdc0, 32bbcff0, 32bbd240, 32bbd3a0) and unit vslot 0x328 overrides: "not yet attributed (the 32bb.... range)" = Ara',
    '32bb0bd6 is `fstp [ebp+0xb18]` in helper 32bb09f0 (class map: %s, callers ASI_A_ALS_VIEW_BETWEEN / _FINISH %s, ASI_A_ALS_SHAKE_CHAOS_FINISH, ASI_A_AA_RAID_ATTACK_FINISH %s); '
    'Ara\'s vslot 0x328 override is 32ba6440 (class map %s; its body reads the passives 27417 / 27420 and ch0_fp.tsv lists it as a CH4a passive handler)' % (cp('32bb09f0'), cp('32bbcdc0'), cp('32bbd3a0'), cp('32ba6440')), 'CH2d',
    'CH0 Opus decision: l.312 writer goes with its helper 32bb09f0 (class map CH2d; callers CH2d3 / CH3c2 states), not CH1h as the plan said; the vslot 0x328 override 32ba6440 is CH4a; the CEffect member and base vslot 0x328 are engine C7')

# 7b
add('§7b', AL(328, '`31fdbbb0` (skill-memo entry'), 'skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0 (every user)',
    'Ara calls 31fdbbb0 @32bcf11d (class map %s) and 31fdbcc0 @32baa6fd / @32bcfd78 / @32bd0ad0 (32bd0ad0 in SetEquippedSkillFuncBySkillSlotIndex 32bd09d0, class map %s); MemoConditionAra RTTI 3373d604 (vtable 33502680, ctor 32ba46f0) = CH4b; the Ara script\'s APPLY_MEMO x2 and EFFECT_SET_LIST_EX_MEMO x2 sit in the dead tables ASI_SA_ALS_SPECTER_OF_BLADE / _FINISH' % (cp('32bcf11d'), cp('32bd0ad0')), 'CH1n CH4b',
    'CH0 decision: memo -> CH4b; CH1n reads the 32bd09d0 head\'s memo test (as Eve / Elsword / Raven / Rena)')
add('§7b', AL(329, '`31fdbbb0` / `31fdbcc0` readers'), '320284c0 and 3230c1a0 memo readers (skills 31301 / 31321 / 22027)', 'Rose skills 31301 / 31321 and memo 22027: not Ara', 'n/a', 'S3d consumers show their lines when CH2 / CH3 bind the first memo-equipped skill (CH6 walk item)')
add('§7b', AL(330, 'the per-class blocks in funcmap rows'), 'Ara\'s vslot 0x780 32bca010 (27415 RNSI_P_ASD_OPEN_MIND / 27417 RNSI_P_AYR_OPEN_MIND blocks; brief kickoffs/june-effect/C8c.md)',
    'vtable 0x780 = 32bca010 (class map %s, base 33093ad0 = the empty hook CX2GUUser::JuneDamageExceptionClassHook); body calls 31fceab0 five times and 32007b10 six times (passive tests)' % cp('32bca010'), 'CH4a', 'CH0 decision: the 0x780 hook = CH4a, whole (the digest said CH4b)')

# 7c
add('§7c', AR(353, 357, 'sites'), 'effect-alpha tags (167 Eve sites, Elsword, Aisha, Add, Rena rows)',
    'Ara has no row; byte scan of the Ara range: 0 calls to the five tag helpers 32296d60 / 32296ee0 / 32296da0 / 32296e40 / 32296f30; c10a_alpha_sites.tsv has 0 Ara-range rows', 'n/a', 'CH0 fact: 0 alpha-tag rows for Ara')

# 7e
add('§7e', AL(368, '`UnitViewerEffectSetTable.lua` row `ALD_LobbyWait`'), 'unit-viewer ALD_LobbyWait row; March\'s hand-made orb (m_hSeqEnergyBullet1-3) is dead code; retire the three handles under the player\'s flag with the user\'s OK',
    'X2UnitViewerUI.h:337-339, .cpp :80-82 / :228-230 / :588-625 / :1192-1210 / :1538-1550 / :2158-2169 / :2236-2501; the row matches since C9g-2; Open decision 2 of the Ara plan', 'CH5', 'needs the user\'s OK (plan Open decision 2): retire or keep')

# 7h
add('§7h', AL(422, 'Elsword 25 functions'), 'UPDOWN_CANCEL_AFTER (+0x8ca4) up / down cancel branches (Elsword, Raven, Lire)', 'Ara_Martial_Artist.src.lua has 0 UPDOWN_CANCEL_AFTER (ch0_keys.tsv: absent)', 'n/a', 'not Ara')
add('§7h', AL(423, '`32024ff0` list variants'), 'EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE; Ara among "every class with June scripts")',
    'script: EFFECT_SET_LIST x50, EFFECT_SET_LIST_EX x61, _EX_MEMO x2 (dead tables), _FINAL x5 (%s), _FINAL_ULTIMATE x2 (ASI_SA_ALD_WOLF_SPEAR, ASI_SA_ALH_LINK_FLOWER); engine reader 32024ff0 done (Eve CH1r)' % kst('EFFECT_SET_LIST_FINAL ').replace('ASI_SA_ALD_WOLF_SPEAR L14804 [CH2c]; ', ''),
    'CH1 final CH2b CH2c CH3c2', 'script data arrives with the script; the first users: Link Flower (CH2b), Wolf Spear (CH2c), Raid (CH3c2)')
add('§7h', AL(424, '`EffectSetToPlay` entry `+0x34`'), 'EffectSetToPlay +0x34 / +0x38 and user slots 0x4c4 / 0x4c8 around each play in 32007570: Ara 32bc1530; RELATE_SKILL_ID in Ain / Ara / Elesis / Raven / Rose scripts',
    'vtable 0x4c8 = 32bc1530 (class map %s, base 33093ad0); script RELATE_SKILL_ID x6 in 3 states (%s); POSSIBLE_NASOD_BARRIER x0' % (cp('32bc1530'), ', '.join('%s x%d [%s]' % (k, v, st_ph[k]) for k, v in collections.Counter(x[1] for x in keylines('RELATE_SKILL_ID')).items())), 'CH4b', 'CH0 decision: 0x4c8 l.424 = CH4b (add the two base virtuals then); the RELATE_SKILL_ID states are CH3c2 (script data CH1 final)')
add('§7h', AL(425, '`SOUND_PLAY_EX`'), 'SOUND_PLAY_EX / STATE_SOUND_PLAY / CUSTOM_STATE_DELETE_SOUND data (Add, Elesis, Luciel, Arme, Aisha)', 'Ara_Martial_Artist.src.lua has 0 of the three keys', 'n/a', '')
add('§7h', AL(426, 'Class code writing `+0x3f00` directly'), '+0x3f00 writers and AddEffectSetDeleteCondition callers (not identified)',
    'byte scan of the Ara range: 0 calls to 3227a570; the one +0x3f00 byte match (32bd48c7) is inside another instruction, not a lea ecx,[esi+0x3f00]; Ara script DELETE_EFFECT_SET_ON_STATE_END x2 (%s) is the C9h-4d data key, engine done' % kst('DELETE_EFFECT_SET_ON_STATE_END'), 'n/a', '')
add('§7h', AL(427, '`USEFUL_VALUE0..4`'), 'USEFUL_VALUE rows and getters (every class with June scripts)',
    'Ara script USEFUL_VALUE0 x2 (%s); c9h4f_useful_value_readers.tsv has 0 Ara-range rows (the engine readers 31fc8620 / 31fd0260 read these two)' % kst('USEFUL_VALUE0'), 'CH1 final', 'rows = script data; no class reader')
add('§7h', AL(428, '`USI_WAIT_STATE_CHANGE`'), 'USI_WAIT_STATE_CHANGE registered in every class InitState', 'Ara script ASI_WAIT_STATE_CHANGE L937 (june-only, CH1a); registered by the InitState wrapper 32bcf4a0 @303 id 130 (ch0_state_skill.tsv); WIN\'s OnEventProcess = the delegate 320718d0', 'CH1m CH1a', 'registration CH1m; the state\'s callbacks are CH1a')

# 7i
add('§7i', AL(440, '(June `+0x7cc8`)'), 'class code multiplies m_fNowStatePowerRate (June +0x7cc8), never the entry\'s', 'zero Ara functions touch +0x7cc8 / +0x7ccc / +0x7cd0 (the l.256 scan)', 'n/a', 'no Ara state port has anything to keep or convert')
add('§7i', AL(444, '`32d223e0` Elsword'), 'SetEquippedSkillFuncBySkillSlotIndex Ara 32bd09d0: drop the snapshot call, set state byte +0x11 = 1',
    '32bd09d0 (override 0x7b4 + registrar, class map %s); the snapshot call 31fd2af0 is @32bd0a65; March X2GUAra.cpp:1890; the June-only 0x68c override 32bd34c0 (InitAquiredActiveSkillState, class map) is a second skill-listing function (ch0_skill_listing_rows.tsv)' % cp('32bd09d0'), 'CH1n', 'CH0 decision: l.444 slot-state byte = CH1n; skill cases follow their skills (CH2a-CH3d)')

# 7l
add('§7l', AL(472, 'Bounding user states'), 'bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every class InitState + their tables',
    'Ara script: ASI_DAMAGE_BOUNDING_DOWN L6372, _LANDCRASH L6386, _UP (june-only, CH1b); InitState body 32bca580 @1104 / 1107 / 1112 ids 120 / 121 / 122 (ch0_reg.tsv)', 'CH1p CH1m', 'hook list says CH1p (states with the script), CH1m registers them; the states\' own functions are CH1b')
add('§7l', AL(472, 'Bounding user states'), 'March\'s state id is a UCHAR (June\'s is 16-bit)', 'X2GUAra.cpp / .h have no UCHAR state id (CH0 grep: 0 hits)', 'n/a', '7l state-id width check passes')

# 7o
add('§7o', AL(509, 'All 12 classes in 7l'), 'SET_LIMIT_MAX_G_SPEED in <prefix>DAMAGE_BOUNDING_DOWN (-4000)', 'script: %s (the only use)' % kst('SET_LIMIT_MAX_G_SPEED'), 'CH1 final', 'script data, arrives with the script; reader done (C3g)')
add('§7o', AR(516, 521, 'APPLY_MOVEMENT_SPEED'), 'Ara\'s APPLY_MOVEMENT_SPEED = TRUE in ASI_AIRIAL_DASH (Ara_Martial_Artist.src.lua:7334) with PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED * 1.5',
    'script: %s; PASSIVE_SPEED_X also in %s; engine side ported (C3h)' % (kst('APPLY_MOVEMENT_SPEED'), kst('PASSIVE_SPEED_X').split('; ')[1]), 'CH1a', 'CH0 decision: l.516 = CH1a (carry the key verbatim; script data arrives with CH1 final)')
add('§7o', AR(522, 526, 'SPEED_TIME'), 'June SPEED_TIME additions (C3l): ASI_A_ARA_QUICK_STAB SPEED_TIME_ULTIMATE0 / 1 (2 rows); ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH SPEED_TIME0 = { 2550, 0, 0.496, 2, TRUE }',
    'script: %s; SPEED_TIME0 { 2550 ... TRUE } at L17433 in ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH [%s] (skill owners: Quick Stab RNSI_A_ALH_QUICK_STAB class 28 = CH2b, Specter of Storm class 34 = CH2d)' % (kst('SPEED_TIME_ULTIMATE'), st_ph['ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH']), 'CH2b2 CH2d2',
    'CH0 decision: SPEED_TIME l.522-526 = the skill\'s phase (the state phases CH2b2 / CH2d2); the _ULTIMATE gate stays off until m_DeleteSkillIDByUpgrade (backlog Stage 3c)')

# 7p
add('§7p', AL(548, '| All classes | `TIME_STOP_IMMUNE_STATE = TRUE`'), 'TIME_STOP_IMMUNE_STATE = TRUE in the class script (76 June states over 16 class files)',
    'Ara script 3 states: %s; reader C3e-4 done' % kst('TIME_STOP_IMMUNE_STATE'), 'CH1 final', 'script data; the three states\' functions are CH3d (hyper actives)')

# 7s
add('§7s', AL(579, 'reads COMMON'), 'dungeon-registry per-class tables: X2DungeonManager.cpp:252-552 reads ARA already', 'March already reads ARA', 'n/a', '')

# 7t
add('§7t', AL(592, '**The class script:**'), 'SHOW_HYPER_CUT_IN_TIME in the class script ("plus Ara\'s berserk ones")',
    'Ara script 6 states: %s; ch0_state_skill.tsv puts the six tables under CH1a (hyper / berserk mode states) while plan CH3d carries "the 6 SHOW_HYPER_CUT_IN_TIME states (7t)"' % kst('SHOW_HYPER_CUT_IN_TIME'), 'CH1 final CH3d', 'script data; the cut-in play is CH3d (plan) / CH1a (state map): CH3d checks the cut-in names once the states exist')
add('§7t', AL(594, 'June\'s `UnitTemplet.lua`'), 'June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cutin_*_Transcendence textures',
    'ch0_unittemplet_ara.tsv: 7 Ara rows (5 March + 34 / 124 new); m_HyperCutIn on 118 / 119 / 124 (HQ_Cutin_ASD / AYR / ASR_Transcendence), m_BerserkCutIn on 118 / 119 / 124 (HQ_Cutin_ASD_1 / AYR_1 / ASR_1_Transcendence), m_fLobbyRotationY on all 7 (1 / 15 / 1 / 10 / 15 / 1 / 40 for 7 / 28 / 118 / 31 / 119 / 34 / 124); '
    'names STR_ID_31537 / 31539 (34) and 31538 / 31540 (124), present in general.ess and both StringID_def; the 6 .gfx are in data195 already (kom_contents)', 'CH5 CH3d', 'rows = CH5 (BOM source, core list UnitTemplet.lua; the 34 / 124 rows wait for CHNa if the loader refuses them); the .gfx consumers = CH3d')
add('§7t', AL(602, '| Chung | unit vslot `0xa4` override'), 'unit vslot 0xa4 berserk-by-chance override (Chung 32c24b90)', 'Ara overrides 0x9c (IsFullHyperMode) and 0xa0 (SetFullHyperMode) but has NO 0xa4 override (ch0_vtable_slots.tsv)', 'n/a', 'confirmed')
add('§7t', AL(603, '| Ara | user vslot `0x51c` override `32bd85f0`'), 'user vslot 0x51c override 32bd85f0: now state 0x17 or 0x9b (ASI_BERSERK_MODE) -> override JuneIsHyperModeState',
    'vtable 0x51c = 32bd85f0 (class map %s; reads the protected state at +0x7cbc / +0x7cec and compares with 0x17 first)' % cp('32bd85f0'), 'CH1q', 'CH0 decision: IsHyperModeState = CH1q')
add('§7t', AL(607, '**Hyper active skills'), 'hyper active skills: cut-in shown whatever the option says (type 13)', 'Ara: 6 type-13 ids (ch0_state_skill.tsv part 2 CH3d: 2 per class 118 / 119 / 124; states ASI_HA_ASD_THOUSANDS_BLOSSOMS / ASI_HA_AYR_ENERGY_CANNON / ASI_HA_AA_NINE_TAILED_FOX)', 'CH3d', 'play-test: option Hide, use a hyper active -> the cut-in shows')
add('§7t', AL(613, '**Sound sets'), 'sound sets: Ara / Luciel JuneIsHyperModeState also picks the HYPER / BERSERK sound set (until it lands Ara\'s ASI_BERSERK_MODE picks as a plain state)',
    'the override is 32bd85f0 (the l.603 row); the June keys USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE (Elesis only) and SOUND_SET3_RATE and up are absent in the Ara script (max SOUND_SET2_RATE)', 'CH1q', 'rides with IsHyperModeState')
add('§7t', AL(620, '**Skill cut-in names'), 'skill cut-in names -> June .gfx: June HQ_CutIn_Ara_*; March s_SkillCutInSet names .dds textures',
    'March cut-in table X2GUAra.cpp:36-60 (7 names, 0 .gfx; g4_cutin_names.tsv) + the InitState index switch :280-310 (sets m_iSkillCutInSetIndex 0 / 1 / 2); June names HQ_CutIn_Ara_01 / _11 / _AS_01 / _AS_11 / _SD_01 / _SD_11 / _YR_01 / _YR_11 / _Cash_16 / _Cash_17 (35 HQ_CutIn_Ara_*.gfx already in kom_contents, data152 and others); no 34 / 124 index arm',
    'CH5', 'replace s_SkillCutInSet and the switch (CH0b stages the .gfx); the 34 / 124 arms with CHNa')

# 7u
add('§7u', AR(634, 641, 'class'), 'March-only effect templets the re-adoption dropped (Raven, Elsword, Rena, Aisha, Chung, Eve rows)', 'c11a_march_only_templets.tsv: 0 rows for Ara (CH0 fact); no Ara row in the table', 'n/a', 'CH0 fact: 7u = 0 rows for Ara')

# 7w
add('§7w', AL(664, 'and come with their ports'), 'CUSTOM_RENDER_STATE class users: Ara_Martial_Artist among the scripts that carry the key; C++ writers of the buff override +0x8e64 (Raven\'s Power Assault shown; Ara not named)',
    'script key x1 in ASI_SA_ASR_FOX_PHANTOM (CH3c); BUT Ara\'s C++ also WRITES +0x8e64 (byte scan of the Ara range): `mov [esi+0x8e64], 1` @32ba1d2d (ASI_SA_AA_SPECTER_OF_THUNDER_FIND FrameMove, class map %s) and @32ba1ddb (_FIND_LANDING FrameMove %s), `mov [esi+0x8e64], ebx` @32bbe885 (ASI_SA_ALS_VERITABLE_PANDEMONIUM FrameMove %s) with `cmp [esi+0x8e64], 2` @32bbec3f (_FINISH FrameMove %s), and READS it in ApplyRenderParam 32b96a30 @32b96a74 (override 0x3a8, %s): '
    'CRS_SPECTOR_OF_THUNDER = 1, CRS_VERITABLE_PANDEMONIUM = 2. TEXT INCOMPLETE: the row names only Raven as a C++ writer' % (cp('32ba1d2d'), cp('32ba1ddb'), cp('32bbe887'), cp('32bbec3f'), cp('32b96a74')),
    'CH3c CH2d CH2d2 CH1h', 'CH0 decision: CUSTOM_RENDER_STATE = CH3c labelled stand-in (Raven\'s Power Assault shape) until june-engine R5; the 4 C++ writer functions need the same stand-in in CH2d / CH2d2; the ApplyRenderParam read is CH1h')

# 7x
add('§7x', AL(669, 'June\'s class scripts tag combo'), 'combo hits carry June\'s AT_COMMAND tag; adopt the script with its tags', 'Ara script AT_COMMAND 36 + AT_SPECIAL 8 (June script count; March script has none, ch0_keys.tsv); stand-in SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN stays until all played classes are ported', 'CH1 final CH1d CH1d2', 'script data (with the combo states CH1d / CH1d2 and the line combos CH1e-CH1g)')

# 7y
add('§7y', AL(676, '### 7y.'), 'GetLandPosition bMonster in class code', 'w1c_land_position_callers.tsv has 0 Ara-range rows; byte scan of the Ara range: 0 calls to 31c7bb00', 'n/a', '')

# 7aa
add('§7aa', AL(702, 'exists for **every class**'), 'basic-item pair (every class): 329657d0 / UpdateEquipCollisionData 31fd8c00',
    'engine plan R14c; Ara\'s vslot 0x634 override 32baf270 (class map %s) calls 31fd8c00 @32baf313 / @32baf344 and 3201c3a0 @32baf2d1 / @32baf336 and 31fd5b80 @32baf32f; Ara does not override 0x638 UpdateEquipCollisionData (ch0_vtable_slots.tsv)' % cp('32baf270'), 'n/a', 'engine plan R14c; the 0x634 body is CH1i')

# 7ae
add('§7ae', AR(713, 745, 'Per family'), 'an id of a listed type with no case plays the class\'s first state (Ara: first state of the switch = Wind Wedge)',
    'Ara: 288 rows over 139 ids, 281 no_state, 7 has_state (id 900 once per class); ch0_s3_baseline.txt; ch0_state_skill.tsv part 2: 139 / 139 mapped (per phase CH2a 14, CH2b 18, CH2c 18, CH2d 18, CH3a 14, CH3b 14, CH3c 14, CH3d 6, CH4a 22, CH1a 1)',
    'CH1a CH2a CH2b CH2c CH2d CH3a CH3b CH3c CH3d CH4a', 'each skill phase binds its ids (class 7: 14; first jobs 28 / 31 / 34: 18 each shared with the second jobs 118 / 119 / 124: 14 each; hyper actives 6 = CH3d; passives 22 = CH4a); the skill switch default = the class first skill state')
add('§7ae', AL(733, '49 rows (Chung upgrade ids'), 'rows with no June SKILL_ID name', 'Ara: 0 of 288 rows lack a June name (ch0_s3_baseline.txt skill_name empty count 0)', 'n/a', 'no unnamed-id task')
add('§7ae', AL(726, 'June\'s types 14-16'), 'types 14-16 / 17 / 18 named in the class switch',
    'Ara rows by type (ch0_s3_baseline.txt): 6 x91, 15 x59, 14 x49, 16 x49, 8 x24, 12 x7, 13 x6, 1 x3; 32bd09d0 type switch (ch0_skill_listing_rows.tsv); Ara never calls ResetLeftSkillCoolTimeBySkillType (X2GUAra.cpp: 0; June range: 0 calls to 32895390)', 'CH1n CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'switch head CH1n; the cases ride their skills')

# 7ad
add('§7ad', AL(760, 'plus skill animations of Add, Ara'), 'stage June Motion_ARA* files with the class script; skill animations of Ara (renamed in June)',
    'ch0_motion_question.tsv: June vs March per file, Motion_ARA.x 3 new names, Motion_ARA_Common.x 1 dropped / 27 new, Motion_Ara_LD.x 2 dropped / 7 new, ...; new June files Motion_Ara_LS.x / Motion_Ara_AA.x (34 / 124); held for CH1 final in ch1_held_motions.tsv (Motion_Ara_LD.x, Motion_Ara_SD.x, placement data033, march-copy until the script lands); the rest copied by CH0b',
    'CH0b CH1 final', 'CH0b stages June\'s Ara motions, CH1 final copies the 2 held files with the script; rerun the census after staging')

# 7ac
add('§7ac', AR(775, 778, 'Ara:'), 'SLockOnSystem::ProcessLockOn 3200fa20 @3200ff98 (March X2GUAra.cpp:5435) with CanDown( NULL, &lineIndex ), the candidate-loop CanDown @3200fcad; shared with Luciel',
    'March ProcessLockOn = X2GUAra.cpp:5366-5476 (CanDown at :5449; the row\'s :5435 is stale); callers of 3200fa20 in the Ara range: @32b9b427 (ASI_SA_ALH_SWALLOW_ASSAULT_READY Start 32b9b3c0, class map %s) and @32bc20d6 (ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK EventProcess 32bc1f30, %s); w1f_candown_callers.tsv rows 3200fcad / 3200ff98' % (cp('32b9b427'), cp('32bc20d6')), 'CH2b0',
    'CH0 decision: lock-on (7ac l.775-782) = CH2b0 (flag _LOCKON, amend CH3c20 to "hoist only"); the Swallow Assault states are class 118 ids 27011 / 27052 (CH3a in the class map, plan text says CH2b)')
add('§7ac', AR(775, 777, 'SWALLOW_ASSAULT'), 'ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK EventProcess (@32bc204c, March :11264) and _FINISH StateStart (@32bb70ff, :11371) pass NULL, &lineIndex',
    'June @32bc204c in 32bc1f30 (%s) and @32bb70ff in 32bb7010 (%s); March CanDown calls now at X2GUAra.cpp:11298 / :11405 (the row\'s :11264 / :11371 are stale); the 8 other Ara states with CanDown (Wait, HyperWait, Walk, JumpLanding, Dash, DashEnd, DashJumpLanding, DashJumpPowerLanding) are "March shape: forwarder" in the w1f tsv' % (cp('32bc204c'), cp('32bb70ff')), 'CH3a CH2b0',
    'the two call sites ride the 118 skill states (class map CH3a; plan CH2b names Swallow Assault under class 28) on CH2b0\'s lock-on')
add('§7ac', AL(781, '`32021390`'), 'June-only helpers with NULL, &idx: user landing prediction 32021390 (Add / Luciel / Arme callers)',
    'TEXT INCOMPLETE: Ara has a caller too: 32021390 @32b9402e inside 32b93fe0 (ASI_DASH_JUMP_COMBO_ALS_ZX StartFuture, class map %s); w1f tsv lists "Add / Luciel / Ara-EL / Arme"; Aisha ported it as CX2GUArme_VioletMage::JuneLandingPrediction' % cp('32b9402e'), 'CH1g', 'port the helper (a per-class JuneLandingPrediction or a hoist) with the ALS ZX state')
add('§7ac', AL(782, 'Ara landing helper `32bc0420`'), 'Ara landing helper 32bc0420 ( NULL, &idx )', '32bc0420 helper (class map: CH2b0, "lock-on landing helper"); its CanDown @32bc051f is "in NULL, out ptr / DIFFERS"', 'CH2b0', 'CH0 decision: lock-on rows = CH2b0')

# 7af
add('§7af', AL(785, '### 7af.'), 'camera linear tracking / SUB_CAMERA sequencer', 'Ara script SUB_CAMERA0-10 / CAMERA keys are plain script data; byte scan of the Ara range: 0 calls to 31fb6af0 / 31fae7b0 / 31ff5df0 / 31fae6d0 / 31faee30 / 31fb3640 / 31fae910 / 31fb5920; no LINEARMOVING / TILTANGLE in the script', 'n/a', 'engine')

# 7ag
add('§7ag', AL(805, '### 7ag.'), 'pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every > 3 / -4 decode; decode slot + 1, never SAKP_Q..R',
    'Ara has its own March 4 + 4 decode: X2GUAra.cpp:3673-3674 (iPressedSkillSlotIndex > 3, - 4), :3983 / :3994 / :4043 / :4054 (m_iPressedSkillSlotIndex > 3), CountUsedSkill :3926 / :4018, GetSkillSlotIndexUsed :4137; 0 CurrentSkillKeyManager and 0 SAKP_ uses; X2GUUser.cpp:26729 stand-in list does not include UT_ARA', 'CH1p', 'convert to June\'s A 0-4 / B 5-9 and join UT_ARA to the June set under _LIVE (as UT_ELSWORD / UT_RAVEN / UT_LIRE)')
add('§7ag', AL(814, '- A-TC is locked'), 'TC slot A-5 / B-5 states and the Stage 6 skips', 'Ara transcendence 118 / 119 / 124 (ch0_classn.tsv: not on the allow-list, TranscendCheck refuses 1726)', 'CH3d', '')

# 7ai
add('§7ai', AR(852, 855, 'Ara'), 'pass June\'s skip / adjust / level / creator at every ported BFI call (row example: "Ara 32a4dcaa 0x7ea0")',
    'TEXT WRONG for Ara: 32a4dcaa lies in Ain\'s range (32a38770-32a7a0f0), not Ara\'s (CH0). Ara\'s own by-BFI calls (s3e_bybfi_callers.tsv, part 2): @32b94bb4 (ASI_SA_ALS_SPECTER_OF_STORM End, BFI 0x24f, %s); @32ba6df1 / @32ba6e16 / @32ba6e40 (32ba6d00 SetSpecificValueByBuffTempletID, BFI 0x294 / 0x295 / 0x9c, %s); @32bab361 (ASI_STANDUP_ROLLING_FRONT_FRONT End, BFI 0xb5, %s); @32bb881c (ASI_A_ALH_BREAKING_ATTACK FrameMove, BFI 0xb5, %s); @32bc825b (ReInit 32bc8030, BFI 0x108 with creator / adjust = ebx, %s); @32bc8752 (32bc84b0, BFI 0x108, creator / adjust = ebp, %s); @32bc8888 (32bc84b0, explicit adjust 0x6a4d = RNSI_A_AYR_REFLECTION 27213, BFI 0x103, %s)' % (cp('32b94bb4'), cp('32ba6df1'), cp('32bab361'), cp('32bb881c'), cp('32bc825b'), cp('32bc8752'), cp('32bc8888')),
    'CH2d CH1h CH1b CH3a2 CH1k', 'each phase passes June\'s arguments at the calls it ports')
add('§7ai', AR(866, 867, 'JuneShowSkillNameStateString'), 'JuneShowSkillNameStateString( iSkillID ) 3200ab60 callers include 32bc84b0 (Ara range)', 'Ara\'s caller: 3200ab60 @32bc89af in NoStateChangeActionFrameMove (vtable 0x80c = 32bc84b0, class map %s)' % cp('32bc84b0'), 'CH1k', 'CH0 decision: call where June does')
add('§7ai', AL(868, 'First live use of the reserved-buff path'), 'BUFF_FACTOR_ID_AND_APPLY_TIME "appears only in Elesis\'s script among user classes ... the [JUNE_SKILL] SBUF lines first appear with the Elesis port"',
    'TEXT WRONG for Ara: the June Ara script has the key in 4 states (%s) and BUFF_FACTOR_ID_AND_APPLY_TIME_ULTIMATE in %s; the live readers are X2GUUser.cpp:48514 / :48652 and X2GUNPC.cpp:34046 / :34115 (ch0_keys.tsv: March script lacks the string, June has it)' % (kst('BUFF_FACTOR_ID_AND_APPLY_TIME ').replace(' ,', ','), kst('BUFF_FACTOR_ID_AND_APPLY_TIME_ULTIMATE')), 'CH2d CH3b CH3c CH3c2',
    'the first Ara live use of the reserved-buff path: Specter of Thunder (CH2d), High Speed Ready ULTIMATE variant (CH3b), Necromancy (CH3c), Specter Walk (CH3c2); their play-tests check the [JUNE_SKILL] SBUF parse / frame / state end lines')
# 7aj
add('§7aj', AL(874, 'Equipped-slot icon overrides'), 'equipped-slot icon overrides 328e0aa0 per class type (0x6d, 0x7b, 0x72, 0x73, 0x1a, 0x7e, 0x24)', 'Ara\'s class types 7 / 28 / 31 / 34 / 118 (0x76) / 119 (0x77) / 124 (0x7c) are none of them; byte scan of the Ara range: 0 calls to 328e0aa0', 'n/a', '')

# 7al
add('§7al', AL(902, '**New CX2GUUser hooks for every class**'), 'new CX2GUUser hooks for every class: 0x654, 0x850, 0x860, 512, UpdateEquipCollisionData 0x638',
    'Ara vtable (ch0_vtable_slots.tsv) overrides 0x800 (512) = 32bb1ff0 (class map %s); not 0x654, 0x850, 0x860, 0x638' % cp('32bb1ff0'), 'CH1k', 'the 0x800 body is the per-class skill use / cooldown part of SpecialAttackEventProcess')

# 7am
add('§7am', AL(920, '| Ara | `32bcd9c0`'), 'delegate-list registrations in InitPassiveSkillState: Ara 32bcd9c0 (and 32bb0d10, called from it): A 32b95910, A 32ba7e40, A 32b95980, A 32b959a0; A 32baf810, A 32baad30',
    '32bcd9c0 = InitPassiveSkillState (vtable 0x690, class map %s); 32bb0d10 helper called by it (%s); the six thunks 32bd5470 / 32bd6b10 / 32bd5480 / 32bd5490 / 32bd7a90 / 32bd6c80 target 32b95910 / 32ba7e40 / 32b95980 / 32b959a0 / 32baf810 / 32baad30 (all class map %s)' % (cp('32bcd9c0'), cp('32bb0d10'), cp('32b95910')), 'CH4a', 'CH0 decision: delegates l.920 = CH4a (the digest said CH4b); each callback whole, replace March\'s pushes with June\'s six')
add('§7am', AL(930, 'March\'s own pushes'), 'March\'s own pushes (Elsword, Raven, Ara, Elesis, Add) are replaced by June\'s set',
    'Ara\'s March pushes are in SetAttackResultDelegate (X2GUAra.cpp:16851+): InsertDelegateProcessToAttackResult( AddHitUnitInfoByDamageData ) :16868 and ( UpNowForcePowerHitOthers ) :16871 = June\'s 32baad30 (via thunk 32bd6c80); March\'s InitPassiveSkillState (:1514) pushes none', 'CH4a', 'CH0 decision: l.930 = CH4a (same port)')

# 7an
add('§7an', AL(940, '| vslot `0x428`'), 'vslot 0x428 JuneGetDefenceRateBySkill: Ara 32b956a0 (the override, whole)', 'vtable 0x428 = 32b956a0 (class map %s); March X2GUAra has no override (base returns 1.0)' % cp('32b956a0'), 'CH4b', 'CH0 decision: 0x428 l.940 = CH4b')
add('§7an', AL(943, '`GetCharacterSpecializationB()` (June `+0x6d04`) readers'), 'GetCharacterSpecializationB() readers: Ara 32ba6f31 ("the reading function, whole")', '32ba6f31 lies in 32ba6ef0 (SetForcePower, class map %s); the spec-A reader 32bc9a60 (l.278) is the same group' % cp('32ba6f31'), 'CH1q', 'CH0 decision: spec-B reader = CH1q (inside SetForcePower)')

# 7ao
add('§7ao', AL(954, 'Every class also brings'), 'every class also brings: emulator allow-list id, LevelUpEvent TYPE row + DLG_Transcendental_Change_Guide_Ara, JuneGetHyperModeTableName cases, HQ_Cutin_*_Transcendence, hyper-class skills + TRANSCENDENTAL_SKILL_DATA, helpers 31fcac90 / 31fcad70 / 31fceab0 / 31ff1e80',
    'Handlers_Quest.cpp:1411-1442 TranscendCheck: no Ara class on the allow-list (118 / 119 / 124 -> 1726); LevelUpEvent.lua level-70 TYPE row {118, 119, 124} -> DLG_Transcendental_Change_Guide_Ara.lua (June; CH3d); textures POPUP_TR_S_DEVANAM / _Y_RAJA / _ASURA missing in March (ch0b_stage_list.tsv, data215); GetHyperModeTableName already names ASI_ASD / AYR / AA_HYPER_MODE (X2GUUser.cpp:34086-34092), June 31fdd490 @32bcb264 inside 32bca580 = CH1m; '
    'helpers: only 31fceab0 has Ara callers (' + str(len(call_hits(0x31fceab0))) + ' call sites, each its own phase\'s), 0 callers of 31fcac90 / 31fcad70 / 31ff1e80', 'CH1m CH3d CH5', 'CH1m = GetHyperModeTableName registration; CH3d = skills + TRANSCENDENTAL_SKILL_DATA + guide + allow-list; CH5 = cut-in names, pictures')
add('§7ao', AL(959, 'its hyper-effect start'), 'hyper-effect start returns at once for a transcended unit', '32bb22d0 (vtable 0xc4, class map %s)' % cp('32bb22d0'), 'CH3d CH1q', 'the function itself = CH1q (l.236); the transcended return = CH3d (CH0 decision: 7ao rows = CH3d)')
add('§7ao', AL(967, 'Any class with an aura-accelerator skill'), 'ApplyAuraAcceleratorBuffToGameUnit 32028790 for a class with an aura-accelerator skill', 'Ara has no 0x7b0 override (ch0_vtable_slots.tsv)', 'n/a', '')
add('§7ao', AL(978, '| Ara | `32bb22d0`'), 'transcend Ara: 32bb22d0 hyper effect start (vslot 49) branch; 31fdd800 berserk table by class, only caller 32bca580, port whole',
    '32bb22d0 = vtable 0xc4 (class map %s); 31fdd800 has exactly one caller in the exe: 32bcb35b inside the InitState body 32bca580 (class map %s), next to 31fdd490 @32bcb264' % (cp('32bb22d0'), cp('32bcb35b')), 'CH3d CH1m CH1q',
    'CH0 decision: 7ao rows = CH3d (the transcended return); plan CH1m registers 31fdd800 / GetHyperModeTableName with InitState; CH1q ports the function body')

# 7ap
add('§7ap', AL(991, '- **Ara:** 0x3ac'), 'Ara: 0x3ac JuneModifyFinalDamageByGameUnitType 32ba45d0 (move March\'s live 4-arg body), 0x6a8 JuneShownSkillCoolTime 32b958e0 (also called from skill use 32bcff21)',
    'vtable 0x3ac = 32ba45d0 (class map %s), 0x6a8 = 32b958e0 (%s); March X2GUAra.cpp:16942 holds the live 4-arg body (declared X2GUAra.h:663); the skill-use call 32bcff21 lies in 32bcfaa0 (%s)' % (cp('32ba45d0'), cp('32b958e0'), cp('32bcfaa0')), 'CH4b CH1k', 'CH0 decision: 0x3ac / 0x6a8 l.991 = CH4b; the skill-use call rides the 510 lookup body (CH1k)')

# 7aq
add('§7aq', AL(997, '### 7aq.'), 'three-orb ball on the own gauge UI (Raven, Eve rows)', 'Ara not named; Ara\'s gauge UI is CX2AraMyGageUI (force-power pips, InitWakeOrb 32e5b550); its force-power work is the CH1q gauge rows', 'n/a', 'NF-style gauge work = CH1q')

# 7ar
add('§7ar', AR(1007, 1009, 'GetX2UnitClassCompatibility'), 'GetX2UnitClassCompatibility 31ed6500: June cases for unit classes 33-46 / 123-136 / 200-202 belong to classes March lacks; Ara 34 and 124 fall in that range (inferred, the row does not name Ara)',
    'ch0_classn.tsv: X2Unit.h:1456-2172 Ara arms name only 7 / 28 / 118 / 31 / 119, default false; 34 and 124 refuse every item until June\'s cases are added (June 31ed6500: 34 accepts {7, 34}, 124 accepts {7, 34, 124})', 'CHNa', 'CH0 decision: l.1007-1009 for 34 / 124 = CHNa; also the divergent row GetX2UnitClassCompatibility verdict out')

# 7at
add('§7at-7au', AR(1031, 1035, 'add-N-stacks'), 'Ara add-N-stacks buff path: 3229da40 (CX2GameUnit add N to a buff\'s stack count by BTI) calls 3236a2f0; only callers 32bbb050 / 32bbb720',
    'byte scan: callers of 3229da40 in the Ara range are exactly @32bbb29d (32bbb050 = ASI_SA_ALD_WOLF_BREAK FrameMove) and @32bbb974 (32bbb720 = ASI_SA_AYR_OPEN_THE_HELL FrameMove); both class map %s; 0 callers of 3236a2f0 in the range; june-hud H3 gave PushBuff a count' % cp('32bbb050'), 'CH2c0', 'CH0 decision: 7at = CH2c0 (flag _STACK); P_AYR_GOOD_ENERGY_BALL on CH2c0\'s path is the CH4b piece')

# 7av
add('§7av', AL(1044, '### 7av.'), 'class state animation rate, user vslot 0x824 (a class port overrides the slot its June vtable overrides and removes its March block)',
    'Ara has no 0x824 override (ch0_vtable_slots.tsv) and X2GUUser.cpp ResetAnimSpeed has no Ara block (only Chung / Lire SI_P_LCR_AGILE_MOVEMENT)', 'n/a', '')

with open(HERE + 'ch0_checklist_rows.tsv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(['section', 'checklist_line', 'row', 'ara_evidence', 'phase', 'note'])
    w.writerows(R)
print(len(R))
print(collections.Counter(r[4].split()[0] if r[4] else '' for r in R).most_common(60))
print([i for i, r in enumerate(R) if '?' in r[4]])
