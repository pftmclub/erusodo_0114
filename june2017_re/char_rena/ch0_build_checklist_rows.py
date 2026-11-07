r"""char-rena CH0 item 4: build ch0_checklist_rows.tsv (Rena rows of JUNE2017_CHARACTER_PORT_CHECKLIST.md -> phase / n/a).
    python june2017_re/char_rena/ch0_build_checklist_rows.py
Copy of char_raven/ch0_build_checklist_rows.py, adapted to Rena (Lire_ElvenRanger, classes 3 / 12 / 13 / 24 / 102 / 103 / 114,
June range 32c6a8d0-32cb6c6d). Line numbers are anchored by text in the CURRENT checklist (1048 lines, 2026-11-07).
Rena's evidence is read from ch0_state_skill.tsv, ch0_class_map.tsv, ch0_keys.tsv, ch0_enum_names.tsv, ch0_passive_ids.tsv,
ch0_numtables.json, ch0_nf.tsv, ch0_traps.tsv, ch0_vtable_slots.tsv, ch0_s3_baseline.txt, plan_checklist_rows.md (each digest line re-verified).
"""
import csv, json, re, collections, os, bisect
csv.field_size_limit(10 ** 9)
HERE = os.path.dirname(os.path.abspath(__file__)) + '/'
T = os.path.normpath(HERE + '../../') + '/'
CL = open(T + 'JUNE2017_CHARACTER_PORT_CHECKLIST.md', encoding='utf-8').read().split('\n')


def A(sub, after=1):
    for i in range(after - 1, len(CL)):
        if sub in CL[i]:
            return 'L%d' % (i + 1)
    raise SystemExit('anchor not found: ' + sub)


def S(head):
    """span of the section whose heading starts with head: 'La-Lb'"""
    hs = [i for i, l in enumerate(CL) if l.startswith('## ') or l.startswith('### ')]
    for k, i in enumerate(hs):
        if CL[i].lstrip('#').strip().startswith(head):
            j = hs[k + 1] if k + 1 < len(hs) else len(CL)
            return 'L%d-L%d' % (i + 2, j)
    raise SystemExit('heading not found: ' + head)


def rd(f):
    return [r for r in csv.reader(open(HERE + f, encoding='utf-8', newline=''), delimiter='\t') if r]


# ---- evidence tables
cm = [r for r in rd('ch0_class_map.tsv') if re.fullmatch('[0-9a-f]{8}', r[0])]
cm.sort(key=lambda r: int(r[0], 16))
cm_ad = [int(r[0], 16) for r in cm]
cm_by = {r[0]: r for r in cm}


def cphase(a):
    """phase of the class-map function that contains address a"""
    r = cm_by.get(a) or cm[bisect.bisect_right(cm_ad, int(a, 16)) - 1]
    return r[6], r[0]


st = [r for r in rd('ch0_state_skill.tsv') if not r[0].startswith('#') and r[0] not in ('state', 'skill_id') and len(r) > 7]
st_ph = {r[0]: r[7] for r in st if r[0].startswith('LESI_') or r[0].startswith('INIT')}
rows_b = [r for r in rd('ch0_s3_baseline.txt') if not r[0].startswith('#')]
hdr = [r for r in rows_b if r[0] == 'unit_class_name'][0]
data = [dict(zip(hdr, r)) for r in rows_b if r[0] != 'unit_class_name']
cls_of, name_of = collections.defaultdict(set), {}
for d in data:
    cls_of[int(d['skill_id'])].add(int(d['unit_class_value']))
    name_of[d['skill_name']] = int(d['skill_id'])
PH = collections.OrderedDict([(3, 'CH2a'), (12, 'CH2b'), (13, 'CH2c'), (24, 'CH2d'), (102, 'CH3a'), (103, 'CH3b'), (114, 'CH3c')])


def first_phase(sid, nm=''):
    if nm.startswith('RNSI_HA_'):
        return 'CH3d'
    return PH[min(cls_of[sid], key=lambda c: list(PH).index(c))]


R = []


def add(sec_, line, row, ev, phase, note=''):
    R.append((sec_, line, row, ev, phase, note))


# ---------------- intro
add('intro', A('(Elsword, Aisha, Rena'), 'June update of a March one (Elsword, Aisha, Rena, Raven, ...)',
    'Rena is a March class with a June update: 7 classes UC_LIRE_ELVEN_RANGER 3, COMBAT_RANGER 12, SNIPING_RANGER 13, TRAPPING_RANGER 24, WIND_SNEAKER 102, GRAND_ARCHER 103, NIGHT_WATCHER 114', 'n/a', 'scope')
add('intro', A('Character scripts (`Elsword_SwordMan.lua`'), 'character script Lire_ElvenRanger.lua feeds SetDamageDataFromLUA',
    'June src 352 top-level tables (6 INIT_* + 346 LESI_ states: 322 mapped, 24 june-dead-data; ch0_state_skill.tsv); March data036 Lire_ElvenRanger.lua 160,708 B bytecode (kom_contents, 2026-09-22); not on the core list (tools/data/lua_core_list.tsv names Lire in no row)',
    'CH1 final', 'adopt-june (June bytecode via ljconv) after the states exist; add a core-list row first only if a phase must edit the script (none so far)')

# ---------------- section 1
nt = json.load(open(HERE + 'ch0_numtables.json'))
for o in nt:
    ln, top, tl, tname, tstart, idx, app, apptop, dam = o
    nm = app[0].replace('SKILL_ID.', '')
    sid = name_of[nm]
    owners = sorted(cls_of[sid])
    ph = st_ph.get(top) or first_phase(sid, nm)
    add('§1', A('`Lire_ElvenRanger` | 12 |'), 'numerical-only %s table in state %s (script L%d; NUMERICAL_DATA_INDEX = %s at L%d)' % (tname, top, tstart, idx, ln),
        'APPLIED_SKILL_ID %s %d (classes %s); no DAMAGE row in the block' % (nm, sid, ', '.join(map(str, owners))),
        ph, 'NUM use line from the play-test; phase = ch0_state_skill.tsv phase of the state (skill owner class phase %s); state %s starts at script L%d' % (first_phase(sid, nm), top, tl))
nph = sorted(set(st_ph.get(o[1]) or first_phase(name_of[o[6][0].replace('SKILL_ID.', '')]) for o in nt))
add('§1', A('`Lire_ElvenRanger` | 12 |'), 'numerical-only DAMAGE_DATA + numerical override live (the 12 tables above; gap 12 of charport_scan 2026-11-07)',
    'Lire_ElvenRanger 12 numerical-only + 65 other (plan_charport_scan_20261107); plan_numerical_rows.tsv: 141 numerical blocks over 57 distinct ids, all result=ok',
    ' '.join(nph), 'each skill phase checks its ids reach the override (NUM use lines)')
add('§1', S('1. Blocker'), 'blocks that index past their skill data need a JUNE_BUGS row',
    'plan_numerical_rows.tsv: 0 rows with a result other than ok (141 ok)', 'n/a', 'no zero-row JUNE_BUGS entry for Rena')
add('§1', S('1. Blocker'), 'retire SKILL_NUMERICAL_DEBUG', 'already retired by Aisha CH2a', 'n/a', '')

# ---------------- section 2
add('§2', A('| `APPLIED_SKILL_ID` |'), 'APPLIED_SKILL_ID (parsed since 8c)',
    '17 uses in Lire_ElvenRanger over 9 tables (ch0_keys.tsv): 12 numerical-only blocks + 5 beside a DAMAGE row; engine reader done',
    ' '.join(nph), 'with the section 1 rows by owner')
add('§2', A('| `NUMERICAL_DATA_INDEX` |'), 'NUMERICAL_DATA_INDEX', '12 uses = the 12 section 1 tables (script L%d ... L%d)' % (nt[0][0], nt[-1][0]),
    ' '.join(nph), '= the section 1 rows')
add('§2', A('| `NATURAL_FORCE_TYPE` |'), 'NATURAL_FORCE_TYPE x77 in Lire_ElvenRanger, "unsupported (parsed, logged), 8c, done"',
    '77 uses over 68 states (ch0_keys.tsv; script L3097 ...); March parses it only under ADD_RENA_SYSTEM (dormant). Open decision 1 answered (a) 2026-11-07: natural force on, March ADD_RENA_SYSTEM code is the base, June bodies where they differ (ch0_nf.tsv: 99 CH1q + 27 CH1q2 rows, 24 absent in June)',
    'CH1q', 'the parse + consumers are CH1q (`_NF`); the 68 states carrying the key ride their own phases (script data arrives with CH1 final)')
add('§2', A('| `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, `'), 'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID and _RELATIVE_SKILL_ID', 'none used by Lire_ElvenRanger (ch0_keys.tsv)', 'n/a', '')
add('§2', A('| `ADDITIONAL_ATTACK_TRIGGER_TYPE` |'), 'ADDITIONAL_ATTACK_TRIGGER_TYPE (Raven only)', 'none in Lire_ElvenRanger (ch0_keys.tsv); reader 32610fb0 already ported', 'n/a', '')
add('§2', A('| `BUFF_FACTOR_ID_ULTIMATE` |'), 'BUFF_FACTOR_ID_ULTIMATE', 'Ara only; none in Lire_ElvenRanger', 'n/a', '')
add('§2', A('| `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP` |'), 'IGNORE_DEFENCE_RATE and _ON_PVP (Raven only)',
    'none in Lire_ElvenRanger (ch0_keys.tsv). Rena\'s ignore-defence is a code write: Powerful Bowstring in SetDamageExceptionProcess 32cad510 @32cada25 (CH4b)', 'n/a', 'engine reader done; Rena\'s own use is the CH4b write')
add('§2', A('| `IGNORE_DEFENCE_DECLINE` |'), 'IGNORE_DEFENCE_DECLINE', 'ELESIS_KNIGHT only; none in Lire_ElvenRanger', 'n/a', '')
add('§2', S('2. DamageData keys'), 'other DamageData keys (HIT_ADD_COMBINATION_POINT, DAMAGE_RATE_TO_SUPERAMOR)', 'none used by Lire_ElvenRanger (ch0_keys.tsv)', 'n/a', '')
add('§2', A('`DAMAGE_SKILL_ID` (8b group 1) appears 103 times'), 'DAMAGE_SKILL_ID x103, "all in Elsword_DamageData.lua"',
    'x103 are Rose ids (see char_elsword); Lire_ElvenRanger.src.lua has zero', 'n/a', 'Rose (Stage 8) data')

# ---------------- section 3-4
add('§3', A('| Natural force | code exists'), 'Natural force: ADD_RENA_SYSTEM is off under US_SERVICE; "a decision for you"; also gates the first-attack / relate-skill copies (C6a2 321ef5ea-321ef632)',
    'Open decision 1 answered (a) 2026-11-07 (JUNE_CHARACTER_RENA_PHASES.md): natural force on, March\'s dormant ADD_RENA_SYSTEM code is the base, June\'s bodies where they differ. ch0_nf.tsv 225 rows: class tail, X2GUUser, X2DamageManager / X2DamageEffect / X2EffectSet first-attack copies, X2GUNPC AttackResultByType 32773ee0, CX2RenaGageData / CX2RenaMyGageUI, X2GageManager',
    'CH1q CH1q2', 'CH1q = class + engine bodies (flag `_NF` defines ADD_RENA_SYSTEM), CH1q2 = gauge UI and emulator (27 rows)')
add('§3', S('3. Systems'), 'systems absent (numerical data, combination point, Rose weapon, Lu/Ciel duo)', 'numerical data = section 1; the rest are other characters', 'n/a', 'covered by section 1')
add('§4', S('4. Enum tables'), 'missing enum tables (CUSTOM_RENDER_STATE, DUO_TYPE, ROSE_WEAPON_TYPE)', 'plan_charport_scan_20261107.txt has no Lire ENUM line: no table to add', 'n/a', 'CH1 final reruns charport_scan.py after adopting the script')
ec = collections.Counter(r[4] for r in rd('ch0_enum_names.tsv') if not r[0].startswith('#') and r[0] != 'kind')
cc = collections.Counter(r[7] != '' and r[7] != '-' for r in rd('ch0_enum_names.tsv') if not r[0].startswith('#') and r[0] != 'kind')
add('§4', A('- **Missing members of tables March *does* have**'), 'June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGER_STATE_ID) fail silently',
    '130 distinct Rena skill ids (ch0_enum_names.tsv): C++ enum name %d / lookalike %d / none %d; core Enum.lua name 62 / absent 68; 0 ids lack a June name. '
    'LIRE_ELVENRANGER_STATE_ID (ch0_state_enum_names.tsv): one C++ enum holds every June prefix (101 live members), June Enum.lua lists 121 names, 129 registered June states are not in it' % (ec['name'], ec['lookalike'], ec['none']),
    'CH1a CH1b CH1c CH1d CH1d2 CH1e CH1f CH1f2 CH1g CH1g2 CH1g3 CH2a CH2b CH2c CH2d CH3a CH3b CH3c CH3d CH4a',
    'house convention: raw id + one-line RNSI_ / state-name comment per case (as Eve / Raven); passive ids (types 1, 8) = CH4a, hyper actives (type 13) = CH3d; the SI_ ids March code reads that differ from June\'s tree ids: ch0_passive_ids.tsv (1+49 DIFFERS, 26 not live, 22 same)')

# ---------------- section 5 keys
kc = collections.Counter(r[11] for r in rd('ch0_keys.tsv') if r[11] != 'verdict')
add('§5', S('5. What no scan'), 'per-character parser census', 'ch0_keys.tsv: %d keys; june-only %d, neither %d, march-only-site %d, both %d' % (sum(kc.values()), kc['june-only'], kc['neither'], kc['march-only-site'], kc['both']), 'CH1p', 'per-key owners follow')
add('§5', S('5. What no scan'), 'keys FORCE_DYNAMIC_CAMERA_INDEX x15, RETAIN_CAMERA_STATE x1', 'reader 32057b80 CX2GUUser::CommonStateStart; March has no reader', 'n/a', 'engine camera keys (Stage 9; same as Chung / Elsword / Raven)')
add('§5', S('5. What no scan'), 'buff sub-table keys BBT_CHANGE_NOW_HP_PER_SECOND x3, BFT_TIME x3', 'read by stringized type name on both sides (ch0_keys.tsv: no reader)', 'n/a', 'same as Chung / Elsword / Raven')
add('§5', S('5. What no scan'), 'key IMMUNITY_EDT_LIST_AT_THIS_STATE x14', 'June reader 31fdaaa0 (per-state parse in CX2GUUser); March has none (ch0_keys.tsv)', 'n/a', 'backlog Stage 2 per-state immunity lists (as Chung / Elsword / Raven)')
add('§5', S('5. What no scan'), 'key IMMUNE_TO_SUPER_ARMOR_DOWN x10', 'engine reader 31fdaaa0 from CX2GUUser::CommonStateStart @32057e68 (ch0_keys.tsv owner column); not a Rena function', 'n/a', 'routed to backlog Stage 2 "Per-state immunity lists" (JUNE_PORT_BACKLOG_ORDER.md, bullet amended by char-rena CH0), with IMMUNITY_EDT_LIST_AT_THIS_STATE')
add('§5', S('5. What no scan'), 'class keys BOW_SKILL_STATE x26, CREATE_TRAP_BLADE x2',
    'both read inside Rena\'s CommonStateStart override 32ca81b0 (vslot 0x7c4, class map CH1j); BOW_SKILL_STATE on 26 states (CH2a CH2c CH3b CH1f2), CREATE_TRAP_BLADE on LESI_SA_RNW_ERADICATION_START / _ATTACK (CH3c)', 'CH1j', 'reader = CH1j; the states that set the keys ride their own phases (script data)')
add('§5', S('5. What no scan'), 'neither-side keys ATTACK_ACTION_TYPE x12, HEAD_IK x9, JUMP_CANCEL_AFTER x4', 'no reader on either side (ch0_keys.tsv)', 'n/a', 'keep verbatim in the script')
add('§5', S('5. What no scan'), 'march-only-site keys AFTER_IMAGE x49, READY_SOUND, UNIT_SLASH_TRACE0 / 1',
    'engine reads them on both sides; no key drops a March reader\'s input (ch0_keys.tsv)', 'n/a', 'census artifacts')
add('§5', S('5. What no scan'), 'keys DAMAGE_DATA_THIRD x1, DAMAGE_DATA_LAST x2, HYPER_MODE_COUNT x1, MAX_HYPER_MODE_TIME x1',
    'DAMAGE_DATA_THIRD LESI_SA_RNW_FURIOUS_ENGAGE (CH3c); DAMAGE_DATA_LAST LESI_A_LE_RISING_FALCON (CH2b) + FURIOUS_ENGAGE; HYPER_MODE_COUNT / MAX_HYPER_MODE_TIME sit in INIT_COMPONENT (script L287 / L285, engine reads)', 'n/a', 'script data; both parsers read them (ch0_keys.tsv both)')
add('§6', A('4. Decide on `ADD_RENA_SYSTEM`'), '4. Decide on ADD_RENA_SYSTEM (section 3) before touching Rena', 'answered 2026-11-07, Open decision 1 (a)', 'n/a', 'decision made; implementation = CH1q')
add('§6', A('Stage 7 characters mark'), 'mark rule: a character port marks the rows it did with (Rena: CHn)', 'process rule for the closing phase', 'Z', 'Z marks the checklist rows')

# ---------------- section 7 intro table
sp = [cphase(a)[0] for a in ('32c87ec0', '32ca68b0', '32ca81b0')]
add('§7', A('Per-class code that writes the state power rate'), 'class code scales the cached state power rate (+0x7cc8)',
    'a byte scan of the June exe over 32c6a8d0-32cb6c6d for the three displacements +0x7cc8 / +0x7ccc / +0x7cd0 finds exactly three functions, each with the protected-float read (mov eax,[esi+7ccc]; mov ecx,[esi+7cd0]; add esi,7cc8; xor): '
    '32c87ec0 (LESI_SA_LCR_SHARPFALL_LANDING Start, %s), 32ca68b0 (LESI_SA_RTR_FATALITY Start, %s), 32ca81b0 (CommonStateStart override, %s). March Rena has no assigner: its only write is `*=` at X2GULire_ElvenRanger.cpp:19195 / :19197 (LESI_SA_RTR_FATALITY_StateStart, m_sFatalityData.GetPowerRate())' % tuple(sp),
    'CH3a CH2d CH1j', 'keep June\'s operator in each (decode *= / = when porting); the scan reads the displacement only, so assign vs multiply is each phase\'s check')
add('§7', A('Per-class code that writes the state power rate'), 'March Rena RTR_FATALITY m_fNowStatePowerRate *= m_sFatalityData.GetPowerRate() (:19195)',
    'June counterpart 32ca68b0 (LESI_SA_RTR_FATALITY Start, ch0_class_map.tsv CH2d), same state in the three-function scan above', 'CH2d', 'Fatality skill; March\'s multiply stays only if June\'s body does the same')
add('§7', A('`32024ff0` list variants') if False else A('Every class\'s vslot 510, the skill lookup'), 'quick-slot special-item prefix at the top of SpecialAttackEventProcess (vslot 509 / 510)',
    'Rena is one of the eight March classes that carry it (X2GULire_ElvenRanger.cpp:5238-5243, JUNE_BUFF_QUICKSLOT_ITEM); class map: lookup body 32cac200 (override 0x7f8, slot 510, CH1k), SpecialAttackEventProcess 32c6a970 (override 0x7f4, uncarved, CH1k)', 'CH1k', 'keep the prefix in June\'s 509 / 510 bodies (lead decision: l.262 skill lookup 32cac200 = CH1k)')
add('§7', A('Every class\'s June skill-use check'), 'dev free-skill gate around the cool-time / MP / hyper-stone block of the skill-use check',
    'March block X2GULire_ElvenRanger.cpp:5317-5325 (#ifdef TOGGLE_UNLIMITED_SKILL_USE); June skill-use parts 32ca50b0 (slot 511) and 32c89a20 (slot 512), class map CH1k', 'CH1k', 'JuneIsDevUnlimitedSkillUse() (standing rule 2026-10-22); CH1n checks 32cb06d0 has no such block')
add('§7', A('| `32a07dc0` (Add), `32b1eef0` (Luciel)'), 'class skill-use function sets the cooldown then calls ApplySkillCoolTimeRateByBuff',
    'Rena is already wired in March (X2GULire_ElvenRanger.cpp:5463 GetUnit()->ApplySkillCoolTimeRateByBuff( pSkillTemplet->m_eID ))', 'CH1k', 'rides with the slot-511 / 512 skill-use bodies 32ca50b0 / 32c89a20')
add('§7', A('The same vslot 510 copies, further down'), 'riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup', 'Rena carries it (X2GULire_ElvenRanger.cpp:5300-5305, JUNE_BUFF_RIDING_KEY_WAIT)', 'CH1k', 'keep in June\'s 510 lookup 32cac200')
add('§7', A('`31fc8200` (`IsSummonSkill`'), 'IsSummonSkill (54 June skill ids) and the summon cases of 3230c1a0 that March lacks (NPC ids ... 2083 ...)',
    'DISAGREES with the digest (it listed no summon row). Rena summons NPC traps: ch0_traps.tsv has 4 June CreateNPCReq sites (32c95882 NUI 457 dead, 32c99a1a Evoke 1020, 32c9e42e Anger of Elf 1420, 32ca11f2 NUI_RENA_TARGETING_GUNGNIR 2083 = an id in this row) and the summon-info setters 32c85900 / 32c86340 / 32c863a0 / 32c87860 / 32c878f0 (3230bbbd..3230c8b6 region)',
    'CH2d0', 'CH2d0 maps IsSummonSkill ids 23216 / 23218 / ... and the per-NPC summon cases (GetSummonPowerRateByBuff in each new case)')
add('§7', A('`329e31e0` (Add), `32baafc0`'), 'BBT 97 ChangeMaxCharacterAddResource override (Add, Ara, Rose, Luciel)', 'Rena has none (no vslot 201 / +0x324 override in ch0_vtable_slots.tsv)', 'n/a', '')
add('§7', A('`320072d0` and `3202b9a0`'), 'death-prevention pre-apply: gauge snapshot overrides (vslot 0x740 / 0x744); the row names Lire 32c852f0',
    'DISAGREES with the digest (n/a: "every class"). The row\'s per-class list has `Lire 32c852f0` = Rena\'s vslot 0x740 override (ch0_vtable_slots.tsv; class map CH1i); Rena also overrides 0x744 with 32c89230 (base 3204da60 = the Crest of Time restore; class map CH1q: it writes the natural-force gauge, 32a317d0)',
    'CH1i CH1q', '0x740 gauge snapshot = CH1i (Store/RestoreGageSnapshot as Elsword CH1q wrote under _CORE); 0x744 = CH1q (NF gauge write, restore base call kept)')
add('§7', A('`329e2340` / `329e2360`'), 'Add TryHyperModeStateChange / CanHyperMode rows', 'not Rena', 'n/a', 'OTHER')
add('§7', S('7. Buff-engine hooks'), 'AnimPlay extra skin anims, 32a3d1f0, after-image modes, bone rotation (l.243-248)', 'every user, engine', 'n/a', 'engine owners')
add('§7', S('7. Buff-engine hooks'), 'Chung InitComponent / InitPassiveSkillState readers; Add / Ain / Rose class rows (UpNowDPValue, SetPassiveSum, vslot 121)', 'not Rena', 'n/a', 'OTHER')
add('§7', A('Every class\'s vslot 510, the skill lookup'), 'every class\'s vslot 510 lookup (the Rena-named half: Phase 7d put the quick-slot prefix at the top of the eight March classes\' SpecialAttackEventProcess)',
    'see the vslot 510 row above; June body 32cac200 (override 0x7f8)', 'CH1k', 'lookup body 32cac200')
add('§7', A('`3201f850` (`CX2GUUser::SetSkillLevelStateData'), 'SetSkillLevelStateData hoisted into CX2GUUser; March copy X2GULire_ElvenRanger.cpp (row says :22954, plan says :22990); multiply by ( 3201ad10( skill ) + 1.0 )',
    'Rena copy at X2GULire_ElvenRanger.cpp:22990-23003 (the checklist row\'s :22954 is stale); June callers named for Rena: 32c886b0 = SetEquippedSkillLevel override 0x7b8 (class map CH1i)', 'CH1o',
    'lead decision: l.286 = CH1o (add the term or hoist); 32c886b0 itself is CH1i and calls it')
add('§7', A('| `32b87880` (buff Phase 9b, item 5)'), 'Add / Ain / Rose / Luciel / Ara / Elesis / Eve-only rows (summon stats via 322ece70 of Eve / Add NPC, 32011280 / 322a0180, Spec A readers of other classes)', 'Rena not listed except the rows given separately (255, 278, 280, 283, 286, 291)', 'n/a', 'OTHER')
add('§7', A('`3201ad10` (the skill-group power socket rate)'), '3201ad10 skill-group power socket rate called from class code (Eve, Add, Rose, Chung, Elsword ranges)',
    'no Rena address named; CH4b of the Rena plan re-checks "if a Rena function calls 3201ad10" (plan CH4b scope)', 'n/a', 'plan CH4b: add the term only if a Rena function calls it; none found at CH0')
add('§7', A('`3230c1a0` (`CX2Game::SetUserSummonedNPCInfo`, rewritten in June)'), 'SetUserSummonedNPCInfo summon stats (322ece70 per-case flags) for every class with a summon',
    'Rena has 4 trap summon cases; the trap summon-info setters 32c85900 / 32c86340 / 32c863a0 / 32c87860 / 32c878f0 (ch0_traps.tsv row "trap summon info", CH2d0); 32c86340 and 32c878f0 are named by this row\'s follow-up list', 'CH2d0', 'port each summon case with its follow-up (as Eve CH2a / Chung CH3b0)')
add('§7', A('June `data037_kom/Lire_Evoke_trap.src.lua`'), 'Evoke trap NPC scripts Lire_Evoke_trap(.src).lua / _Memo (INIT_COMPONENT USE_OWNERS_ENCHANT = TRUE; NPC slots 111 / 112 3275aa20 / 3275aa80)',
    'ch0_traps.tsv: Evoke trap NPC scripts owner CH2d0 (luabind.py check on every trap NPC script); June script has no USE_OWNERS_ENCHANT in Lire_ElvenRanger itself (0 hits)', 'CH2d0', 'lead decision: l.291 = CH2d0; March copies lack the key')
add('§7', A('Class readers of the tutorial flag'), 'tutorial flag read in class DamageReact', 'no Rena address named; c14c_tutorial_reads.tsv has no Rena-range row', 'n/a', '')

# 7a
add('§7a', S('7a. Effect-engine hooks'), 'AddDamageEffectModifier list and the class tag rows (Eve, Add, Rose, Ain, Luciel, Ara, Elsword)', 'no Rena tag row (grep Rena / Lire in L296-L319: none)', 'n/a', '')
# 7b
add('§7b', A('`31fdbbb0` (skill-memo entry for `APPLIED_SKILL_ID`)'), 'skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0', 'Rena\'s SetEquippedSkillFuncBySkillSlotIndex 32cb06d0 is the skill-switch head (CH1n); the Rena script has APPLIED_SKILL_ID but no APPLY_MEMO key; MemoConditionRena (RTTI 3373d738) is CH4b', 'CH1n', 'skill-switch head; also the S3d memo play-test line (CH6)')
add('§7b', A('`31fdbbb0` / `31fdbcc0` readers in class content'), '320284c0 and 3230c1a0 memo readers', 'Rose skills 31301 / 31321 and memo 22027: not Rena', 'n/a', 'S3d consumers show their lines when CH2 / CH3 bind the first memo-equipped skill (CH6 walk item)')
add('§7b', A('| `32c57660` (`+0x9688'), 'Raven Burning Nasod Hand row (32c57660, 32c43120, 32045340, AT_COMMAND)', 'Raven only', 'n/a', '')
add('§7b', A('the per-class blocks in funcmap rows `329bc560`'), 'per-class SetDamageExceptionProcess blocks: funcmap 32cad510 (Rena)',
    '32cad510 (override 0x2b8, base 32045340; March Rena has NO override; class map CH4b); June-only function: per-skill triggers on RNSI_* ids, Powerful Bowstring 23405 ignore-defence write @32cada25; follow Elsword\'s 32d1e0e0 shape; brief kickoffs/june-effect/C8c.md',
    'CH4b', 'lead decision: l.330 SetDamageExceptionProcess 32cad510 = CH4b; whole function')
add('§7b', A('D8 (completion plan)'), 'D8: Rena Powerful Bowstring 3405 -> 23405 and Sharp Kick 3400 -> 23400 re-keys; Aisha\'s Fitness replaced',
    'ch0_passive_ids.tsv: March SI_ ids 3405 / 3400 differ from June\'s RNSI_ 23405 / 23400; March\'s blocks stay until this port', 'CH4a', 'lead decision: l.331 D8 = CH4a (passive objects + readers, with the reform skills); the 23405 ignore-defence write itself is in 32cad510 (CH4b)')

# 7c
add('§7c', A('| 8 sites (`32c89310`'), 'Effect-alpha tags: 8 sites 32c89310, 32c89460, 32c895b0 (32296da0 handles)',
    'c10a_alpha_sites.tsv: 8 sites, 1 in 32c89310 (@32c893e5), 1 in 32c89460 (@32c89535), 6 in InitComponent 32c895b0 (@32c8990b .. @32c8996f), all 32296da0; class map: all three functions CH1h', 'CH1h', 'lead decision; one JuneTagEffectAlphaParticle call per site right after the creation')
add('§7c', A('| `322726f0`: the HP-rate gate'), '322726f0 HP-rate gate in GetFinalDamage for RNSI_SA_ALP_SUPERSONIC (29058); class column says Rena',
    'DISAGREES with the digest (CH4b). 29058 = RNSI_SA_ALP_SUPERSONIC is an ALP (Add\'s Lunatic Psyker line, 29xxx) id (Enum.src.lua:15170); 0 hits for SUPERSONIC in Lire_ElvenRanger.src.lua, in X2GULire_ElvenRanger.cpp and in ch0_enum_names.tsv; no Rena reader. The row\'s class column "Rena" is a checklist error', 'n/a', 'belongs to Add (Stage 7 char-add)')
add('§7e', A('lost under June\'s player'), 'unit-viewer effects lost under June\'s player: the CDC / ETK / RTR / RNW weapon-equipped gate (Rena named)', 'row says "nothing to port; listed so": RTR = Trapping Ranger, RNW = Night Watcher, accepted loss', 'n/a', 'UI sweep (Stage 9) if ever; nothing to port')
add('§7e', S('7e. Unit-viewer'), 'unit-viewer effect rows (MagicKnight, SPK, SBH, ALP, CDC, ETK/EIS, RTR, RNW: the table\'s live rows drive them)', 'done rows (C9c-2b)', 'n/a', '')
add('§7d-7g', S('7d. Damage-math'), 'damage-math hooks (7d), Rose lobby habit (7g), NPC anim-speed hook (7f), item-templet fields (7g)', 'no Rena function or key named', 'n/a', 'OTHER')

# 7h
add('§7h', A('Elsword 25 functions `32cf8230`'), 'Lire 32cb9ee0: UPDOWN_CANCEL_AFTER (+0x8ca4) up/down cancel branch in a state EventProcess',
    'DISAGREES with the digest (CH2/CH3). 32cb9ee0 lies outside Rena\'s range 32c6a8d0-32cb6c6d (it is in Arme\'s, 32cb6e00-32cf2da0) and Lire_ElvenRanger.src.lua has 0 UPDOWN_CANCEL_AFTER (ch0_keys.tsv: absent). The row\'s "Lire 32cb9ee0" is a checklist error (Arme is the Lire-family neighbour)', 'n/a', 'checklist error: stale / wrong class address')
add('§7h', A('`32024ff0` list variants'), 'EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE)',
    'script: EFFECT_SET_LIST x15, EFFECT_SET_LIST_EX x86, _FINAL x2 and _FINAL_ULTIMATE x2 (LESI_SI1_LE_HUMMING_WIND + _HYPER, phase CH2d); engine reader 32024ff0 done (Eve CH1r). Digest matched this row only through a Rena address in the 32c8 band: confirmed class-generic, no Rena address in the row', 'CH1 final', 'script data arrives with the script; Humming Wind (CH2d) is the first user of the FINAL variants')
add('§7h', A('`EffectSetToPlay` entry `+0x34`'), 'POSSIBLE_NASOD_BARRIER / RELATE_SKILL_ID fields, user slots 0x4c4 / 0x4c8, SOUND_PLAY_EX keys', 'no Rena key or site named (POSSIBLE_NASOD_BARRIER / RELATE_SKILL_ID absent in the script); SOUND_PLAY_EX x1 (LESI_RNW_DASH_XZZ_LOOP, CH1g2) is engine-read', 'n/a', '')
add('§7h', A('Class code writing `+0x3f00` directly'), '+0x3f00 writers, AddEffectSetDeleteCondition callers', 'no Rena address named', 'n/a', '')
add('§7h', A('`USEFUL_VALUE0..4` per-state float rows'), 'USEFUL_VALUE rows and getters',
    'Rena script USEFUL_VALUE0 x6 (LESI_WAIT_STATE_CHANGE CH1a, LESI_DAMAGE_BOUNDING_LANDCRASH CH1b, LESI_SI2_LCR_CRESCENT_KICK + _HYPER CH2b, LESI_SA_RTR_CALL_OF_RUIN CH3c, LESI_HA_LWS_SPIRAL_STIKE_READY CH3d); no Rena reader in c9h4f_useful_value_readers.tsv', 'CH1 final', 'rows = script data; no class reader')
add('§7h', A('`USI_WAIT_STATE_CHANGE` (June base id'), 'USI_WAIT_STATE_CHANGE registered in every class InitState', 'Rena script LESI_WAIT_STATE_CHANGE L516 (june-only, CH1a); InitState 32cadc90 @304 id 130 (ch0_state_skill.tsv)', 'CH1m CH1a', 'registration CH1m; the state\'s callbacks are CH1a')

# 7i
add('§7i', A('`32d223e0` Elsword, `32cede00` Arme'), 'SetEquippedSkillFuncBySkillSlotIndex Lire 32cb06d0: drop the snapshot call, set state byte +0x11 = 1',
    '32cb06d0 (override 0x7b4 + registrar; class map CH1n; ch0_skill_listing_rows.tsv); March X2GULire_ElvenRanger.cpp:3086 (74 case SI_*)', 'CH1n', 'lead decision: l.444 7i head = CH1n; skill cases follow their skills (CH2a-CH3d)')
add('§7i', S('7i. Skill-slot'), 'class code multiplies m_fNowStatePowerRate, never the entry\'s', 'every state port; the three power-rate functions are in the l.256 rows', 'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'fixed assignment (7i -> every skill phase)')

# 7j-7p
add('§7j', S('7j. DamageReact'), 'DamageReact steps (Elesis / Lu-Ciel / Rose hooks); Rune Slayer magic reflect', 'not Rena', 'n/a', 'OTHER')
add('§7k', S('7k. Class-set'), 'class-set callback +0x8dc0', 'no Rena writer named', 'n/a', 'unowned engine item')
add('§7l', A('| Bounding user states |'), 'bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every class InitState + their tables',
    'Rena script: LESI_DAMAGE_BOUNDING_DOWN L2832, _LANDCRASH L2841, _UP L2854 (june-only, CH1b; InitStateCommon 32ca9dc0 @1302 / 1306 / 1314 ids 120 / 121 / 122)', 'CH1p CH1m', 'hook list says CH1p (states with the script) and CH1m registers them; the states\' own functions are CH1b')
add('§7l', S('7l. Super-armour'), 'other 7l rows (Add super-armour, Lu/Ciel target, +0x9175 byte, packet-buff refusal); nothing in a UCHAR', 'not Rena; X2GULire_ElvenRanger.cpp / .h have no UCHAR state id (grep: 0 hits)', 'n/a', '7l state-id width check passes')
add('§7m', S('7m. Damage-effect die'), 'NOTIFY_TO_OWNER_WHEN_DIE (Add only)', 'not Rena', 'n/a', '')
add('§7n', A('| Rena | `32c88050`'), 'user-sync class-skill float: Rena 32c88050 (Grand Archer RANDOM_SHOT_PLEASURE 23415, +/- by up / down) via 32c86620, 32c9d420, 32ca2520; "registered by 32cb06d0"',
    'class map: 32c88050 = LESI_A_LGA_RAPID_SHOT_READY / _ATTACK FrameMoveFuture (CH2c); 32c86620 helper (CH2c); 32c9d420 = LESI_A_LGA_RAPID_SHOT_FINISH FrameMove (CH2c); 32ca2520 = LESI_SA_RGA_ACEINTHEHOLE_* FrameMove (CH3b). The row\'s skill name does not match the class map (the 23415 passive vs Rapid Shot / Ace in the Hole states); the user-sync float itself is engine-side (c3e_user_sync_fields.tsv row G)',
    'CH2c CH3b', 'the aim-float writers / readers ride the states that hold them; CH2c to confirm which state carries 23415\'s up / down keys (row text and class map disagree)')
add('§7n', S('7n. User-sync'), 'user-sync class-skill float: Eve, Chung, Elesis, Ain, Rose, Lu/Ciel rows', 'not Rena', 'n/a', '')
add('§7o', A('| Class | June states with the key | Value |'), 'SET_LIMIT_MAX_G_SPEED in <prefix>DAMAGE_BOUNDING_DOWN (-4000) and Raven\'s RRF_DASH_JUMP_COMBO_XX (-5000)',
    'script: LESI_DAMAGE_BOUNDING_DOWN L2834 = -4000 (the only use, CH1b); reader done (C3g)', 'CH1 final', 'script data, arrives with the script')
add('§7o', S('7o. Fall-speed'), 'Add / Ara / Elesis / Rose / Lu-Ciel speed rows', 'not Rena', 'n/a', '')
add('§7p', A('| All classes | `TIME_STOP_IMMUNE_STATE = TRUE`'), 'TIME_STOP_IMMUNE_STATE = TRUE in the class script',
    'Rena script 18 states: LESI_RGA_GUNGNIR_CONTROL_* x10 (CH1f2), LESI_HA_LWS_SPIRAL_STIKE_READY / _SPINING / _FLYING_ATTACK, LESI_HA_LGA_CRYOTRON_BOLT_GUIDE_SHOT / _CHARGE_SHOT / _DOWNLANDING, LESI_HA_LNW_INNOCENT_START / _END (CH3d)', 'CH1 final', 'script data; reader C3e-4 done')
add('§7p', A('| Chung | DamageReact `32c0cf90`'), 'DamageReact hit-stop sites (Chung 32c0cf90)', 'no Rena site (c3e3_stop_time_callers.tsv has no Rena-range row)', 'n/a', '')
add('§7p', A('| Elsword | March `THROW_WOODEN_PIECE_FrameMove`'), 'Elsword THROW_WOODEN_PIECE_FrameMove SetStopTime', 'Elsword only', 'n/a', 'OTHER')
add('§7p', S('7p. Hit-stop'), 'Aisha binding circle forced stop', 'not Rena', 'n/a', '')

# 7q-7u
add('§7q', S('7q. Luciel'), 'Luciel down-state getters', 'not Rena', 'n/a', '')
add('§7r', S('7r. Chung'), 'Chung cinematic-UI writes', 'not Rena', 'n/a', '')
add('§7s', A('(`X2DungeonManager.cpp:252-552` reads COMMON'), 'dungeon-registry per-class tables: X2DungeonManager.cpp:252-552 reads RENA already', 'March already reads RENA', 'n/a', '')
add('§7t', A('The class script:** every June class script sets the key'), 'SHOW_HYPER_CUT_IN_TIME in the class script',
    'Rena script 3 states (L2417 LESI_LWS_HYPER_MODE, L2501 LESI_LGA_HYPER_MODE, L2585 LESI_LNW_HYPER_MODE; CH1a); ch0_state_skill.tsv marks the three hyper-mode tables as read by hyper mode table name 31fdd490', 'CH1 final CH3d', 'script data; plan CH3d carries "the 3 SHOW_HYPER_CUT_IN_TIME states (7t)"')
add('§7t', A('June\'s `UnitTemplet.lua`** (data036)'), 'June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cutin_*_Transcendence textures',
    'ch0_unittemplet_rena.tsv: 7 Rena rows; m_HyperCutIn on 102 / 103 / 114 (HQ_Cutin_RWS / RGA / RNW_Transcendence), no m_BerserkCutIn; m_fLobbyRotationY on all 7 (30 / 30 / 25 / 1 / 30 / 20 / 10); names STR_ID_616 -> STR_ID_4783 / _4784 / _17429 for 102 / 103 / 114, all present in general.ess and both StringID_def', 'CH5 CH3d', 'rows = CH5 (BOM source, core list UnitTemplet.lua); Transcendence textures = CH3d')
add('§7t', A('| Chung | unit vslot `0xa4` override `32c24b90`'), 'unit vslot 0xa4 berserk-by-chance override (Chung 32c24b90)', 'Rena has NO 0x9c / 0xa0 / 0xa4 override (ch0_vtable_slots.tsv)', 'n/a', 'confirmed')
add('§7t', A('**Hyper active skills (from C3t-1'), 'hyper active skills: cut-in shown whatever the option says (type 13)', 'Rena: type 13 ids 23600 / 23603 (102), 23601 / 23604 (103), 23602 / 23605 (114)', 'CH3d', 'play-test: option Hide, use a hyper active -> the cut-in shows')
add('§7t', A('**Skill cut-in names (from june-gfx G4'), 'skill cut-in names -> June .gfx: June HQ_CutIn_Rena_*; March HQ_CutIn_Lena_1/2/3.dds (3 / 12 / 13 / 24), _Rena_Wind_Sneaker_1, _Grand_Archer_1, _Night_Watcher_1',
    'June 329cd890 + 32078b70 (class tables); 33 HQ_CutIn_Rena_*.gfx and 3 HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx already in March kom_contents (plan table); March cut-in table X2GULire_ElvenRanger.cpp:50-82 + the InitState index switch :1672-1715', 'CH5', 'replace s_SkillCutInSet and the switch (CH0b stages the .gfx)')
add('§7u', A('| Rena | `X2GULire_ElvenRanger.cpp`'), 'March-only effect templets the re-adoption dropped: DamageEffect_LGA_Arc_Shot, DamageEffect_Cryotron_Bolt_GUIDED_ARROW; Effectset_Rena_DiveKickBombingA/B/_B_Memo',
    'c11a_march_only_templets.tsv: all march-consumer. Callers: DamageEffect_LGA_Arc_Shot X2GULire_ElvenRanger.cpp:22010-22038 in LESI_A_LGA_ARC_SHOT_FrameMove (Arc Shot 23208, state phase CH2c); Cryotron_Bolt_GUIDED_ARROW :23274 in LESI_HA_LGA_CRYOTRON_BOLT_GUIDE_SHOT_FrameMove (hyper active 23601, CH3d); Effectset_Rena_DiveKickBombing (Dive Kick Bombing 23007, LESI_SI3_LCR_DIVE_KICK_BOMBING, CH2b)',
    'CH2c CH3d CH2b', 'DISAGREES with the digest (CH3): Arc Shot is a Sniping Ranger (13) skill = CH2c, the Guided Arrow caller is the hyper active = CH3d, Dive Kick = CH2b; take June\'s caller, drop the March names')
add('§7u', A('- Rena: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` is a June case rename'), 'Cryotron Bolt GUIDED_ARROW is a June case rename (Guided_Arrow); the Rena port takes June\'s caller and its spelling',
    'March :23274 asks the all-caps key inside LESI_HA_LGA_CRYOTRON_BOLT_GUIDE_SHOT_FrameMove (:23253); ch0_state_skill.tsv phase CH3d', 'CH3d', 'June\'s caller and spelling')
add('§7u', A('- Chung: `X2BuffTemplet.cpp:1695-1706`'), 'Chung grenade memo / Aisha / Eve / Chung March-only templets', 'not Rena', 'n/a', 'OTHER')

# 7v-7ab
add('§7v-7ab', S('7v. Lu/Ciel minor-unit'), 'Lu/Ciel and Rose rows; CUSTOM_RENDER_STATE users; basic-item pair (every class)', 'not Rena (the Rena script has no CUSTOM_RENDER_STATE)', 'n/a', '')
add('§7aa', A('`329657d0` reads the unit templet\'s *basic* items'), 'm_RavenLeftArmItemID basic-item pair (every class): 329657d0 / UpdateEquipCollisionData 31fd8c00',
    'Rena has no left-arm item (member is Raven\'s) and does not override 0x638 UpdateEquipCollisionData (ch0_vtable_slots.tsv); engine plan row (R14c)', 'n/a', 'engine plan R14c')
add('§7x', A('June\'s class scripts tag combo'), 'combo hits carry June\'s AT_COMMAND tag; adopt the script with its tags', 'Rena script AT_COMMAND 52 + AT_SPECIAL 11 (ch0_keys.tsv; March script has none); stand-in SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN stays until all played classes are ported', 'CH1 final CH1d CH1d2', 'script data (with the combo states CH1d / CH1d2 and the line combos CH1e-CH1g3)')
add('§7y', A('### 7y. GetLandPosition'), 'GetLandPosition bMonster in class code', 'w1c_land_position_callers.tsv has no Rena-range row; March X2GULire_ElvenRanger.cpp has 2 GetLandPosition calls (:15928 CreateEntangleTrap, dead in June; :22756 CreateAngerOfElfTrap, CH2d0)', 'n/a', 'CH2d0 checks the bool at 32c9e250 when it ports the trap; equivalent forwarder shape otherwise')

# 7ae
add('§7ae', A('Per family (rows / no state; first job, second job)'), 'an id of a listed type with no case plays the class\'s first state (Rena: first state of the switch = Power Attack)',
    'Rena: 267 rows over 130 ids, 260 no_state, 7 has_state (id 900 once per class); ch0_s3_baseline.txt; ch0_state_skill.tsv part 2: 130 / 130 mapped', 'CH2a CH2b CH2c CH2d CH3a CH3b CH3c CH3d CH4a',
    'each skill phase binds its ids (class 3: 14, 12 / 13 / 24: 14 each, 102 / 103 / 114: 16 each, hyper actives 6 = CH3d, passives / buff data 19 = CH4a); the skill switch default = the class first skill state: CH2a (ch0_state_skill.tsv)')
add('§7ae', A('49 rows (Chung upgrade ids'), 'rows with no June SKILL_ID name', 'Rena: 0 of 267 rows lack a June name (the 7 non-RNSI rows are id 900)', 'n/a', 'no unnamed-id task')
add('§7ae', A('June\'s types 14-16'), 'types 14-16 / 17 / 18 named in the class switch', 'Rena types 14 / 15 / 16 / 6 / 8 / 12 / 13 / 1 (ch0_s3_baseline.txt); 32cb06d0 type switch (ch0_skill_listing_rows.tsv)', 'CH1n CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'switch head CH1n; the cases ride their skills')
add('§7ae', A('Ain\'s gauge charge'), 'Ain gauge charge (32622bd0 / 32622e80)', 'Ain only', 'n/a', '')
add('§7ad', A('animation names March\'s motions have and June\'s same-named files lack'), 'stage June Motion_Lire* files with the class script; Lire\'s ComboX3 / ComboX4 / ComboX4Up / ComboX4Down / DashAttackX renamed Lire_Combo*; skill animations of Lire',
    'ch0_motion_question.tsv: June 71 / March 18 motion files, 11 class files differ; 19 dropped names in 5 files (Motion_Lire.x 5, _GA 5, _RTR 1, _SR 1, _WS 7), held for CH1 final in ch1_held_motions.tsv (placement data033, march-copy until the script lands); the rest copied by CH0b',
    'CH0b CH1 final', 'lead decision (l.758 / 760): CH0b stages June\'s Lire motions, CH1 final copies the 5 held files with the script; rerun the census after staging')
add('§7ac', A('### 7ac. CanDown'), 'CanDown call shape (Ara / Aisha / Luciel / Rena rows)', 'w1f_candown_callers.tsv has one Rena-range row (32ca111a, below); March X2GULire_ElvenRanger.cpp has 12 CanDown calls (:5790 .. :13397), forwarder shape', 'n/a', 'no change (equivalent)')
add('§7ac', A('Rena Grand Archer `LESI_RGA_GUNGNIR_READY`'), 'June-only CanDown with ( in ptr, out NULL ) in Rena Grand Archer LESI_RGA_GUNGNIR_READY @32ca111a',
    '32ca111a lies in 32ca0f80 (LESI_RGA_GUNGNIR_READY / CONTROL_IDLE / CONTROL_FIRE FrameMove, 12 states) = class map CH1f2 (digest said CH3: DISAGREES; the Gungnir states are the Sniping / Grand Archer line base states CH1f2); w1f row says "none in March"', 'CH1f2', 'port the call shape with the state (June-only skill)')
add('§7af', S('7af. Camera'), 'camera linear tracking / SUB_CAMERA sequencer', 'Rena script SUB_CAMERA0-7 / CAMERA keys x86 are plain script data; no class site named', 'n/a', 'engine')
add('§7ag', A('S5 made the engine 5 + 5'), 'pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every > 3 / -4 decode; decode slot + 1, never SAKP_Q..R',
    'Rena has its own March 4 + 4 decode: X2GULire_ElvenRanger.cpp:5482-5484 (iPressedSkillSlotIndex > 3, aNormalSkillStateIDSlotB[idx - 4]), CountUsedSkill :5390, GetSkillSlotIndexUsed :15297; 0 CurrentSkillKeyManager and 0 SAKP_ uses; X2GUUser.cpp stand-in list does not yet include UT_LIRE', 'CH1p', 'convert to June\'s A 0-4 / B 5-9 and join UT_LIRE to the June set under _LIVE (as UT_ELSWORD / UT_RAVEN)')
add('§7ag', A('- A-TC is locked because March has no transcendence flag'), 'TC slot A-5 / B-5 states and the Stage 6 skips', 'Rena transcendence 102 / 103 / 114', 'CH3d', '')
add('§7ah', A('`Static_EL_SKILL_B(_B)`'), 'Static_EL_SKILL_B(_B) / _R(_B) Elsword way of sword; UpdateElswordSkillSlotEffect', 'Elsword only', 'n/a', 'OTHER')
add('§7ai', A('S3e gave `CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID`'), 'pass June\'s skip / adjust / level / creator at every ported BFI call',
    's3e_bybfi_callers.tsv has two Rena-range rows: 32ca120e in 32ca0f20 (LESI_RNW_DASH_JUMP_XXZ_LOOP End, CH1g3) and 32ca4d58 in ReInit 32ca4a40 (CH1h); March X2GULire_ElvenRanger.cpp also calls it in its skill states', 'CH1g3 CH1h CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'each phase passes June\'s arguments at the calls it ports (the two rows above first)')
add('§7ai', A('**`CX2GUUser::JuneShowSkillNameStateString( iSkillID )`**'), 'JuneShowSkillNameStateString callers', 'Rena\'s caller is NoStateChangeActionFrameMove (vtable 0x80c, 32ca5160, class map CH1k)', 'CH1k', 'call where June does')
add('§7aj', S('7aj. Tree-UI'), 'tree-UI class pieces', 'no Rena piece', 'n/a', '')
add('§7ak', S('7ak. Skill page'), 'decide-page ACK class pieces', 'Aisha, Add only', 'n/a', '')
add('§7al', A('**New CX2GUUser hooks for every class**'), 'new CX2GUUser hooks for every class: 0x654, 0x850, 0x860, 512, UpdateEquipCollisionData 0x638',
    'Rena vtable (ch0_vtable_slots.tsv) overrides 0x800 (512) = 32c89a20 and 0x850 = 32c85c00 (both class map CH1k); not 0x654, 0x860, 0x638', 'CH1k', 'the 0x800 body is the per-class skill use / cooldown part of SpecialAttackEventProcess; 0x850 base 31fd8230 (unnamed): CH1k reads it')
add('§7am', A('| Rena | `32ca7860` |'), 'delegate-list registrations in InitPassiveSkillState: row says Rena 32ca7860 pushes A 32ca2180 (Open Mind), A 32ca2390, A 32c86790, A 32ca28b0',
    'row address is WRONG: 32ca7860 is Rena\'s Win (vslot 0x90, class map CH1h). The registrations are in InitPassiveSkillState 32ca78b0 (vslot 0x690, uncarved, CH4a); thunks pushed @32ca7d17 / @32ca7e83 / @32ca8111 / @32ca8169; class map: callbacks 32ca2180 / 32ca2390 / 32c86790 / 32ca28b0 all CH4a (called by thunks 32cb66e0 / 32cb66f0 / 32cb5dd0 / 32cb6700)', 'CH4a', 'lead decision: l.912 = CH4a; each callback whole, replace March\'s pushes with June\'s four')
add('§7am', A('**NPC-side pushes for two classes\' summons**'), 'NPC-side delegate 3275a490 (owner UT_LIRE -> Rena\'s 32ca2180); registration in CX2GUNPC::June_InitDelegateProcess (Eve ported the slot)',
    '32ca2180 is also called from outside the class (3275a506 / 3275a50f, class map note); ch0_traps.tsv "NPC-side delegate" row owner CH4a', 'CH4a', 'lead decision: l.924-925 = CH4a (Rena adds 3275a490 to June_InitDelegateProcess)')
add('§7am', A('- March\'s own pushes (Elsword, Raven, Ara, Elesis, Add)'), 'March\'s own pushes are replaced by June\'s set', 'Rena\'s March pushes (X2GULire_ElvenRanger.cpp InitPassiveSkillState :2711-3084) are replaced with June\'s four; Rena is absent from this row\'s class list', 'CH4a', 'same port')
add('§7an', A('| vslot `0x73c` | Add'), 'vslot 0x73c / 0x748 overrides (Add, Raven, Aisha, Ain, Chung)', 'Rena has no 0x73c / 0x748 / 0x428 override (ch0_vtable_slots.tsv)', 'n/a', 'confirmed')
add('§7an', A('| `GetCharacterSpecializationB()` (June `+0x6d04`) readers'), 'GetCharacterSpecializationB() readers: row lists Rena 32c85838',
    '32c85838 lies in 32c85690 (GetActualMPConsume, override 0x6a0, class map CH4b); digest said CH1q: DISAGREES. 32c85690 is also in the l.278 spec-B list', 'CH4b', 'inside the MP-consume override (CH4b names 0x6a0 / 0x6a4)')
add('§7an', A('Spec A (option member `+0x6bb0`, result `+0x6c50`) readers'), 'Spec A / Spec B readers (l.278): Rena 32ca7130, 32cb6290 (spec A), 32c85690 (spec B)',
    '32ca7130 = AttackResultByType override 0x4e0 (CH1q, NF); 32cb6290 = UpNaturalForce (CH1q, NF; June-only helper); 32c85690 = GetActualMPConsume (CH4b). Digest matched l.278 only through a Rena address: confirmed Rena addresses are in the row', 'CH1q CH4b', 'class-generic rest of the row is not Rena')
add('§7ao', A('- Rena: `ApplyHyperModeBuff` `32298590'), 'ApplyHyperModeBuff 32298590 @322985ec: unit type 3 -> 32cb50c0 count x 15.0 to vslot 0xf0, then 32cb6290( 5 ) (ADD_RENA_SYSTEM nature force)',
    '32cb50c0 = GetNaturalForceCount (CH1q); 32cb6290 = UpNaturalForce (CH1q); engine caller ApplyHyperModeBuff', 'CH1q2', 'lead decision: l.959 Rena branch = CH1q2 (engine caller); the callees are CH1q')
add('§7ao', A('| Rena | `32c9b3d0` (hyper effect start'), 'transcend: Rena hyper effect start 32c9b3d0 (vslot 49) returns at once for a transcended unit', '32c9b3d0 = HyperModeBuffEffectStart (class map override 0xc4, uncarved, CH1h)', 'CH3d CH1h', 'lead decision: l.969 the transcended return = CH3d, the function itself = CH1h')
add('§7ao', A('- its id in the emulator\'s transcend allow-list'), 'every class also brings: emulator allow-list id, LevelUpEvent TYPE row + DLG_Transcendental_Change_Guide_Lena, JuneGetHyperModeTableName cases, HQ_Cutin_*_Transcendence, hyper-class skills + TRANSCENDENTAL_SKILL_DATA, helpers 31fcac90 / 31fcad70 / 31fceab0 / 31ff1e80',
    'textures POPUP_TR_W_SNEAKER / _G_ARCHER / _N_ARCHER (June data215, missing in March: ch0b_stage_list.tsv), HQ_Cutin_RWS / RGA / RNW_Transcendence; DLG_Transcendental_Change_Guide_Lena.lua (March ships a same-name file, md5 differs); 31fdd490 hyper mode table name = CH1m', 'CH1m CH3d CH5', 'CH1m = GetHyperModeTableName; CH3d = skills + TRANSCENDENTAL_SKILL_DATA + guide + allow-list; CH5 = cut-in, pictures')
add('§7ao', A('Any class with an aura-accelerator skill'), 'ApplyAuraAcceleratorBuffToGameUnit 32028790 for a class with an aura-accelerator skill', 'Rena has no 0x7b0 override (ch0_vtable_slots.tsv)', 'n/a', '')
add('§7ap', A('- **Chung:** 0x6a8 `32bdba60`'), 'June slots CH1o2 gave a base: Lire 0x6a8 32c7bed0 (skill use 32cac5a0)',
    '0x6a8 override 32c7bed0 = ShownSkillCoolTime (class map CH1i); the skill-use call 32cac5a0 lies in 32cac200 (slot 510 lookup, CH1k)', 'CH1i CH1k', 'override CH1i, the call in the skill-use body CH1k (as Chung CH1o / CH1l3)')
add('§7ap', A('### 7ap. June slots CH1o2'), 'other 7ap slots 0x3ac / 0x3b0 / 0x56c / 0x584 / 0x65c', 'Rena vtable (ch0_vtable_slots.tsv) has none of those slots', 'n/a', 'confirmed')
add('§7aq', S('7aq. The three-orb'), 'three-orb ball on the own gauge UI (Raven, Eve rows)', 'Rena\'s own gauge is the natural-force gauge (CX2RenaMyGageUI, ch0_nf.tsv CH1q2); no three-orb ball for Rena', 'n/a', 'NF gauge = CH1q2')
add('§7aq', 'JUNE_PORT_BACKLOG_ORDER.md:236', 'JUNE_PORT_BACKLOG_ORDER.md:236 "Rena 32cb6290( 1 )" (training-game F7 gauge step, June 322ed400)',
    'RIGHT class: 322ed400 case 3 (UT_LIRE) -> push 1; call 32cb6290 = +1 natural force, not a fill (ch0_nf.tsv: X2Game.cpp:5855 dev key row, 322ed57b); replaces March\'s dev key UpNaturalForce( GetMaxNaturalForce() )', 'CH1q2', 'F7 training key (X2Game KeyProcess) with the NF gauge')
add('§7ar', S('7ar. Item class'), 'item class compatibility for classes March lacks', 'Rena classes exist in March', 'n/a', '')
add('§7as', S('7as. Upgrade-weapon'), 'upgrade-weapon glow for Lu/Ciel, Rose, Ain', 'not Rena', 'n/a', '')
add('§7at-7au', S('7at. Ara'), 'Ara add-N-stacks buff path; Ain buff-icon text hook', 'not Rena', 'n/a', '')
add('§7av', A('- **Lire:** March\'s inline `SI_P_LCR_AGILE_MOVEMENT` block'), 'class state animation rate, user vslot 0x824: Lire\'s inline SI_P_LCR_AGILE_MOVEMENT block in ResetAnimSpeed moves to the 0x824 override',
    'Rena overrides 0x824 with 32c83320 (class map CH1k, base 3208dd70); March inline block X2GUUser.cpp:35363-35366 (SI_P_LCR_AGILE_MOVEMENT)', 'CH1k', 'lead decision: l.1045 = CH1k; remove March\'s block in the same phase (divergent row ResetAnimSpeed)')

with open(HERE + 'ch0_checklist_rows.tsv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(['section', 'checklist_line', 'row', 'rena_evidence', 'phase', 'note'])
    w.writerows(R)
print(len(R))
print(collections.Counter(r[4].split()[0] if r[4] else '' for r in R).most_common(40))
print([i for i, r in enumerate(R) if '?' in r[4]])
