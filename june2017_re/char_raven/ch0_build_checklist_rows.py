r"""char-raven CH0 item 4: build ch0_checklist_rows.tsv (Raven rows of JUNE2017_CHARACTER_PORT_CHECKLIST.md -> phase / n/a).
    python june2017_re/char_raven/ch0_numtables.py > /dev/null && python june2017_re/char_raven/ch0_build_checklist_rows.py
Line numbers are anchored by text in the CURRENT checklist (1044 lines, 2026-11-05), never copied from Elsword's file (1038 lines).
Derived from char_elsword/ch0_build_checklist_rows.py; Raven's evidence is from ch0_state_skill.tsv, ch0_class_map.tsv, ch0_keys.tsv, ch0_enum_names.tsv.
"""
import csv, json, re, collections, os
HERE = os.path.dirname(os.path.abspath(__file__)) + '/'
T = os.path.normpath(HERE + '../../') + '/'
CL = open(T + 'JUNE2017_CHARACTER_PORT_CHECKLIST.md', encoding='utf-8').read().split('\n')


def A(sub, after=1):
    for i in range(after - 1, len(CL)):
        if sub in CL[i]:
            return 'L%d' % (i + 1)
    raise SystemExit('anchor not found: ' + sub)


def sec(head):
    for i, l in enumerate(CL):
        if l.startswith(head):
            return i + 1
    raise SystemExit('heading not found: ' + head)


rows_b = list(csv.reader(open(HERE + 'ch0_s3_baseline.txt', encoding='utf-8'), delimiter='\t'))
hdr = [r for r in rows_b if r and r[0] == 'unit_class_name'][0]
data = [dict(zip(hdr, r)) for r in rows_b if r and not r[0].startswith('#') and r[0] != 'unit_class_name']
cls_of, name_of = collections.defaultdict(set), {}
for d in data:
    cls_of[int(d['skill_id'])].add(int(d['unit_class_value']))
    name_of[d['skill_name']] = int(d['skill_id'])
PH = collections.OrderedDict([(4, 'CH2a'), (16, 'CH2b'), (17, 'CH2c'), (25, 'CH2d'), (106, 'CH3a'), (107, 'CH3b'), (115, 'CH3c')])


def first_phase(sid, nm=''):
    if nm.startswith('RNSI_HA_'):
        return 'CH3d'
    return PH[min(cls_of[sid], key=lambda c: list(PH).index(c))]


R = []


def add(sec_, line, row, ev, phase, note=''):
    R.append((sec_, line, row, ev, phase, note))


# ---------------- intro
add('intro', A('(Elsword, Aisha, Rena'), 'June update of a March one (Elsword, Aisha, Rena, Raven, ...)',
    'Raven is a March class with a June update: 7 classes UC_RAVEN_FIGHTER 4, SOUL_TAKER 16, OVER_TAKER 17, WEAPON_TAKER 25, BLADE_MASTER 106, RECKLESS_FIST 107, VETERAN_COMMANDER 115', 'n/a', 'scope')
add('intro', A('Character scripts (`Elsword_SwordMan.lua`'), 'character script RAVEN_FIGHTER.lua feeds SetDamageDataFromLUA',
    'June src 352 top-level tables (6 INIT_* + 346 states: 317 mapped, 29 june-dead-data; ch0_state_skill.tsv); March data036 RAVEN_FIGHTER.lua 171,064 B bytecode; not on the core list (tools/data/lua_core_list.tsv has only Enum.lua with a Raven mention)',
    'CH1 final', 'adopt-june (June bytecode via ljconv) after the states exist; add a core-list row first only if a phase must edit the script (none so far)')

# ---------------- section 1
nt = json.load(open(HERE + 'ch0_numtables.json'))
for o in nt:
    ln, top, tl, tname, tstart, idx, app, apptop, dam = o
    nm = app[0].replace('SKILL_ID.', '')
    sid = name_of[nm]
    owners = sorted(cls_of[sid])
    add('§1', A('`RAVEN_FIGHTER` | 25 |'), 'numerical-only %s table in state %s (script L%d; NUMERICAL_DATA_INDEX = %s at L%d)' % (tname, top, tstart, idx, ln),
        'APPLIED_SKILL_ID %s %d (classes %s); no DAMAGE row in the block' % (nm, sid, ', '.join(map(str, owners))),
        first_phase(sid, nm), 'NUM use line from the play-test; first owner class %d; state %s starts at script L%d' % (min(owners), top, tl))
add('§1', A('`RAVEN_FIGHTER` | 25 |'), 'numerical-only DAMAGE_DATA + numerical override live (the 25 tables above)',
    'RAVEN_FIGHTER 25 numerical-only + 70 other (checklist L50; plan_charport_scan_20261105); plan_numerical_rows.tsv: 113 numerical blocks over 56 distinct ids, all result=ok, 0 zero rows',
    'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'each skill phase checks its ids reach the override (NUM use lines)')
add('§1', 'L37-105', 'blocks that index past their skill data need a JUNE_BUGS row',
    'plan_numerical_rows.tsv: 0 rows with a result other than ok (Elsword had 2)', 'n/a', 'no zero-row JUNE_BUGS entry for Raven')
add('§1', 'L37-105', 'retire SKILL_NUMERICAL_DEBUG', 'already retired by Aisha CH2a', 'n/a', '')

# ---------------- section 2
add('§2', A('| `APPLIED_SKILL_ID` |'), 'APPLIED_SKILL_ID (parsed since 8c)',
    '29 uses in RAVEN_FIGHTER (27 tables; ch0_keys.tsv): 25 numerical-only + 4 beside a DAMAGE row; engine reader done',
    'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'with the section 1 rows by owner')
add('§2', A('| `NUMERICAL_DATA_INDEX` |'), 'NUMERICAL_DATA_INDEX', '25 uses = the 25 section 1 tables (script L4171 ... L17038)',
    'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', '= the section 1 rows')
add('§2', A('| `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, `'), 'APPLY_LEARNED_SKILL_BUFF_FACTOR_ID and _RELATIVE_SKILL_ID', 'none used by RAVEN_FIGHTER (ch0_keys.tsv)', 'n/a', '')
add('§2', A('| `ADDITIONAL_ATTACK_TRIGGER_TYPE` |'), 'ADDITIONAL_ATTACK_TRIGGER_TYPE x7 in RAVEN_FIGHTER, status "to-implement" (8b group 3)',
    'STALE. Parsed: X2DamageManager.cpp:819-830 (list cleared per parse, every value pushed; under SERV_IRUHADEV_JUNE_DAMAGEDATA, Always.h:4439, live). Reader 32610fb0 (funcmap 1278: "ported, effect C5") = DamageData::IsAttackTrigger, X2DamageManager.h:1342-1350, ~60 June call sites, copy to the child effect X2DamageEffect.cpp:10509-10519. '
    'Script x7: RSI_A_RRF_HELL_DIVE (+UPGRADE), RSI_A_RF_MEGADRILL_BREAK (+UPGRADE), RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK (+UPGRADE), RSI_A_RRF_LIMIT_CRUSHER_AVOID_ATTACK',
    'CH1 final CH4b', 'data arrives with the script; consumed by 32c5c080 SetDamageExceptionProcess (CH4b: 24420 trigger 0x12, 24422 trigger 0x13) and the class IsAttackTrigger callers')
add('§2', A('| `BUFF_FACTOR_ID_ULTIMATE` |'), 'BUFF_FACTOR_ID_ULTIMATE', 'Ara only; none in RAVEN_FIGHTER', 'n/a', '')
add('§2', A('| `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP` |'), 'IGNORE_DEFENCE_RATE and _ON_PVP x2 each in RAVEN_FIGHTER, status "to-implement" (8b group 1)',
    'STALE. Parsed: X2DamageManager.cpp:750-751 (SERV_IRUHADEV_JUNE_BUFF_NEW_STAT, Always.h:4769, live; the _ON_PVP value defaults to the plain rate); consumed in CX2GameUnit::GetFinalDamage, X2GameUnit.cpp:3127-3129 (June 32284aa0, funcmap 1261: piercing factor = max( 1 - buff rate - damage-data rate, 0 )). '
    'Script x2 each: RSI_A_RF_MEGADRILL_BREAK L5070 / L5075 and RSI_A_RF_MEGADRILL_BREAK_UPGRADE L16759 / L16764',
    'CH2c', 'engine DONE; the two states belong to Mega Drill Break (skill 24203, classes 17 / 107, first owner 17 = CH2c): play-test Mega Drill Break against a defended target')
add('§2', A('| `IGNORE_DEFENCE_DECLINE` |'), 'IGNORE_DEFENCE_DECLINE', 'ELESIS_KNIGHT only; none in RAVEN_FIGHTER', 'n/a', '')
add('§2', 'L110-143', 'other DamageData keys (NATURAL_FORCE_TYPE, HIT_ADD_COMBINATION_POINT, DAMAGE_RATE_TO_SUPERAMOR)', 'none used by RAVEN_FIGHTER (ch0_keys.tsv)', 'n/a', '')
add('§2', A('`DAMAGE_SKILL_ID` (8b group 1) appears 103 times'), 'DAMAGE_SKILL_ID x103, "all in Elsword_DamageData.lua"',
    'x103 are Rose ids (see char_elsword); RAVEN_FIGHTER.src.lua has zero', 'n/a', 'Rose (Stage 8) data')

# ---------------- section 3-4
add('§3', 'L146-157', 'systems absent (numerical data, natural force, combination point, Rose weapon, Lu/Ciel duo)', 'numerical data = section 1; the rest are other characters', 'n/a', 'covered by section 1')
add('§4', 'L160-181', 'missing enum tables (CUSTOM_RENDER_STATE, DUO_TYPE, ROSE_WEAPON_TYPE)', 'plan_charport_scan_20261105.txt has no RAVEN ENUM line: no table to add', 'n/a', 'CH1 final reruns charport_scan.py after adopting the script')
add('§4', A('- **Missing members of tables March *does* have**'), 'June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID) fail silently',
    '126 distinct Raven skill ids (106, 900, 24000-24058, 24200-24239, 24400-24422, 24600-24605); ch0_enum_names.tsv: C++ enum name 5 / lookalike 30 / none 91; core Enum.lua name 59 / absent 67; 0 ids lack a June name. '
    'RAVEN_STATE_ID (ch0_state_enum_names.tsv): one C++ enum holds every June prefix; 112 registered June states are not in it',
    'CH1a CH1b CH1c CH1d CH1e CH1f CH1g CH1g2 CH2a CH2b CH2c CH2d CH3a CH3b CH3c CH3d CH4a',
    'house convention: raw id + one-line RNSI_ / state-name comment per case (as Eve); passive ids (types 1, 8) = CH4a, hyper actives (type 13) = CH3d; the SI_ ids March code reads that differ from June\'s tree ids: ch0_passive_ids.tsv')

# ---------------- section 5 keys
kc = collections.Counter(r[11] for r in csv.reader(open(HERE + 'ch0_keys.tsv', encoding='utf-8'), delimiter='\t') if r and r[11] != 'verdict')
add('§5', 'L184-198', 'per-character parser census', 'ch0_keys.tsv: 251 keys; june-only %d, neither %d, march-only-site %d, both %d' % (kc['june-only'], kc['neither'], kc['march-only-site'], kc['both']), 'CH1p', 'per-key owners follow')
add('§5', 'L184-198', 'keys FORCE_DYNAMIC_CAMERA_INDEX x15, RETAIN_CAMERA_STATE x1', 'reader 32057b80 CX2GUUser::CommonStateStart; March has no reader; March Raven script lacks both', 'n/a', 'engine camera keys (Stage 9; same as Chung / Elsword)')
add('§5', 'L184-198', 'keys BBT_CHANGE_NOW_HP_PER_SECOND x6, BBT_STUN x3, BBT_FROZEN, BBT_NEVER_MOVE, BBT_DASH_IMPOSSIBLE, BFT_TIME x10, BFT_HITTED_COUNT x3',
    'buff sub-table keys read by stringized type name (June 32d4bbf0 / 32e80450 and 32d4c750 / 32ec36f0; March X2BuffBehaviorTemplet.cpp:59-109, X2BuffFinalizerTemplet.cpp:57-59)', 'n/a', 'same as Chung / Elsword')
add('§5', 'L184-198', 'key IMMUNITY_EDT_LIST_AT_THIS_STATE x5', 'June reader 31fdaaa0 @31fdac15 (CX2GUUser per-state parse); March has none; states: WILD_CHARGE x3 (CH3b), PARRYING x2 (CH1b)', 'n/a', 'backlog Stage 2 per-state immunity lists (as Chung / Elsword)')
add('§5', 'L184-198', 'neither-side keys ANUM_SPEED, HEAD_IK x8, HEAD_IK_MAX_ANGLE, STOP_RESERVE_TIME_ATT / _DEF, STOP_TIME_ATK x8',
    'no reader on either side; ANUM_SPEED and STOP_TIME_ATK are script typos (ANIM_SPEED, STOP_TIME_ATT) that the March Raven script has too', 'n/a', 'keep verbatim; JUNE_BUGS rows for the two typos (ch0_keys.tsv)')
add('§5', 'L184-198', 'march-only-site keys BLADE_MASTERY, UNIT_SLASH_TRACE0, BUFF_FACTOR_ID_AND_APPLY_TIME, LeftDirCamear, AFTER_IMAGE, READY_SOUND',
    'no key drops a March reader\'s input: BLADE_MASTERY\'s reader is dead on both sides (BALANCE_BLADE_MASTER_20130117), the other five are read by June engine functions that March mirrors (ch0_keys.tsv)', 'n/a', '')
add('§5', 'L184-198', 'key BURNING_NASOD_HAND_POSSIBLE x24', 'June reader @32c43911 inside CommonStateStart override 32c43380 (writes +0x4108 = 2 @32c43ac4); March X2GURaven.cpp:5112', 'CH1 final CH4a', 'script data + the 24401 setter / = 2 write pair (7b L327)')
add('§5', 'L184-198', 'keys DAMAGE_DATA_THIRD x2, HYPER_MODE_COUNT, MAX_HYPER_MODE_TIME', 'DAMAGE_DATA_THIRD: reader 32c5c5a0 and both states dead in June; HYPER_MODE_COUNT NPC-only; MAX_HYPER_MODE_TIME engine (ch0_keys.tsv)', 'n/a', '')
add('§6', A('(Raven: CHn)') if '(Raven: CHn)' in '\n'.join(CL) else 'L211', 'mark rule: a character port marks the rows it did with (Raven: CHn)', 'process rule for the closing phase', 'Z', 'Z marks the checklist rows')

# ---------------- section 7 intro table
add('§7', A('Per-class code that writes the state power rate'), 'class code scales the cached state power rate (+0x7cc8): March Raven ASSIGNS m_fAttackPowerRateShadowStepThrust, m_fShadowBusterPowerRate, m_fAttackPowerRateShadowStepPunisher',
    'See the three L256 rows below. June\'s Raven class has exactly 3 functions that touch the +0x7cc8 KProtectedType<float>: 32c4b9e0 (RSI_A_RWT_BARRAGE_ATTACK_AIR FrameMove, *= 1.2, CH2d), 32c5d3f0 (RSI_A_RWT_FLAME_SWORD_FLY Start, *= 1.2, CH2a) and 32c42cd0 (dead: no call / jmp / pointer reference); every other state ported keeps June\'s base behaviour',
    'CH2a CH2d', 'a byte scan of 32c25a20-32c6a63d for [reg+0x7cc8/7ccc/7cd0] finds only those 3 functions; multiply as June does (*=, never March\'s =)')
add('§7', A('Per-class code that writes the state power rate'), 'March RSI_SHADOW_STEP_THRUST_Start X2GURaven.cpp:14727 / :14729 assigns m_fNowStatePowerRate = m_fAttackPowerRateShadowStepThrust (set in InitStateWeaponTaker :2120-2159 *= SA_DAMAGE_REL)',
    'state is march-only (ch0_state_skill.tsv: RSI_SHADOW_STEP_THRUST march-only, n/a). June\'s Shadow Step states RSI_SHADOW_STEP_Z / _Z_UPGRADE (ids 305 / 306, registered in InitPassiveSkillState 32c57660) have Init 32c266b0 / 32c27ba0 (mesh preload of Raven_SI_A_RF_Thrust_Mesh01.X only) + EventProcess 32c30ee0; no Start, no +0x7cc8 write. The damage is the numerical DAMAGE_DATA index 1 (script L4171, L4224)',
    'CH2a', 'operator: none (no June write); skill 24200 first owner class 4; the Thrust damage rate comes from the numerical table, so no class member is carried')
add('§7', A('Per-class code that writes the state power rate'), 'March RVC_SI_P_SHADOW_BUSTER_Start X2GURaven.cpp:20903 / :20905 assigns m_fNowStatePowerRate = m_fShadowBusterPowerRate (set :2566 / :2568 from SA_DAMAGE_REL)',
    'state is march-only (RVC_SI_P_SHADOW_BUSTER, n/a). June\'s counterpart RSI_SHADOW_STEP_X_UPGRADE (id 307, anim SI_P_RVC_SHADOW_BUSTER): Init 32c27350 (mesh preload), FrameMove 32c4dcc0 (creates EffectSet_Raven_Shadow_Buster once the anim time passes the threshold, then calls vslot 0x7cc), EventProcess 32c37b80; no +0x7cc8 write',
    'CH2a', 'operator: none; the state is in the Shadow Step family (ch0_state_skill.tsv phase CH2a, registration lines CH4a); March m_fShadowBusterPowerRate is not carried')
add('§7', A('Per-class code that writes the state power rate'), 'March RSI_P_RRF_SHADOW_PUNISHER_Start X2GURaven.cpp:22949 / :22951 assigns m_fNowStatePowerRate = m_fAttackPowerRateShadowStepPunisher (set :2232-2248)',
    'June has no such state: the script table RSI_P_RRF_SHADOW_PUNISHER (L15242) is june-dead-data (string absent from the exe; Ghidra strings filter PUNISHER lists no RSI_ name), and the skill SI_P_RRF_SHADOW_PUNISHER 4411 is not in June\'s 126 tree ids',
    'n/a', 'dropped with the dead table; the March assignment disappears with the state')
add('§7', A('| `32c641a0`') if False else 'L256 (d)', 'June class functions that DO write the cache: 32c4b9e0 and 32c5d3f0',
    '32c4b9e0 = RSI_A_RWT_BARRAGE_ATTACK_AIR FrameMove (March X2GURaven.cpp:17975; reads Bip01_L_Hand, EventTimer 0.1, then cache *= 1.2 via KProtectedType::operator=); 32c5d3f0 = RSI_A_RWT_FLAME_SWORD_FLY Start (March :22421 RSI_A_RWT_FLAME_SWORD_AIR_StateStart has no rate write; June adds cache *= 1.2 then reads skill 0x5e89 buff data). Both operators are fmul (qword 1.2 at 3346f488)',
    'CH2d CH2a', '32c4b9e0: skill 24212 (classes 25, 115) = CH2d; 32c5d3f0: skill 24201 Flame Sword (all 7 classes) first owner 4 = CH2a. Keep *= 1.2 exactly')
add('§7', A('Every class\'s vslot 510, the skill lookup'), 'quick-slot special-item prefix at the top of SpecialAttackEventProcess (vslot 509 / 510)',
    'Raven is one of the eight March classes that carry it (X2GURaven.cpp:5576-5581, JUNE_BUFF_QUICKSLOT_ITEM); class map: lookup body 32c58c30 (override 0x7f8, slot 510), SpecialAttackEventProcess 32c25af0 (uncarved)', 'CH1k', 'keep the prefix in June\'s 509 / 510 bodies')
add('§7', A('Every class\'s June skill-use check'), 'dev free-skill gate around the cool-time / MP / hyper-stone block of the skill-use check',
    'March block X2GURaven.cpp:5707-5715 (#ifdef TOGGLE_UNLIMITED_SKILL_USE); June skill-use virtual from 0x7f0', 'CH1k', 'JuneIsDevUnlimitedSkillUse() (standing rule 2026-10-22); CH1n checks 32c641a0 has no such block')
add('§7', A('class skill-use function sets the cooldown') if False else 'L248-250', 'class skill-use function sets the cooldown then calls ApplySkillCoolTimeRateByBuff',
    'Raven is already wired in March (X2GURaven.cpp:5844, :21045)', 'CH1k', 'rides with the slot-512 skill-use body')
add('§7', A('The same vslot 510 copies, further down'), 'riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup', 'Raven carries it (X2GURaven.cpp:5646-5651, JUNE_BUFF_RIDING_KEY_WAIT)', 'CH1k', 'keep in June\'s 510 lookup 32c58c30')
add('§7', A('`31fc8200` (`IsSummonSkill`'), 'IsSummonSkill and SetUserSummonedNPCInfo summon cases', 'Raven summons no NPC: 0 CreateNPC / NUI_ hits in X2GURaven.cpp; no summon key in the script', 'n/a', '')
add('§7', A('`329e31e0` (Add), `32baafc0`'), 'BBT 97 ChangeMaxCharacterAddResource override (Add, Ara, Rose, Luciel)', 'Raven has none (no vslot 201 override in vtable 33511454)', 'n/a', '')
add('§7', A('`320072d0` and `3202b9a0`'), 'death-prevention pre-apply: gauge snapshot overrides (vslot 0x740 / 0x744)', 'no Raven override of 0x740 / 0x744 in the checklist or in ch0_vtable_slots.tsv (the 0x73c / 0x748 pair is 7an)', 'n/a', 'confirmed')
add('§7', 'L242-246', 'AnimPlay extra skin anims, 32a3d1f0, after-image modes, bone rotation', 'every user, engine', 'n/a', 'engine owners')
add('§7', 'L247-250', 'Chung InitComponent / InitPassiveSkillState readers; Add / Ain / Rose class rows (UpNowDPValue, SetPassiveSum, vslot 121)', 'not Raven', 'n/a', 'OTHER')
add('§7', A('`329e2340` / `329e2360`'), 'Add TryHyperModeStateChange / CanHyperMode rows', 'not Raven', 'n/a', 'OTHER')
add('§7', A('Every class\'s vslot 510 lookup') if False else 'L262', 'every class\'s vslot 510 lookup (the Raven-named half: Phase 7d put the quick-slot prefix at the top of the eight March classes\' SpecialAttackEventProcess)',
    'see the vslot 510 row above', 'CH1k', 'lookup body 32c58c30')
add('§7', A('`3201f850` (`CX2GUUser::SetSkillLevelStateData'), 'SetSkillLevelStateData hoisted into CX2GUUser; March copy X2GURaven.cpp:23019; multiply by ( 3201ad10( skill ) + 1.0 )',
    'Raven copy at X2GURaven.cpp:23019 (called :23017); class map: June callers in the skill-use body and 32c58c30', 'CH1o', 'add the term or hoist (plan CH1o lists :23019 and the 3201ad10 term, section 7 l.286)')
add('§7', 'L264-292', 'Add / Ain / Rose / Luciel / Ara / Elesis / Lire / Lanox / Eve-only rows (summon stats, 32011280 / 322a0180, Spec A readers, 3201ad10 callers)', 'Raven not listed in these rows', 'n/a', 'OTHER')
add('§7', A('Class readers of the tutorial flag'), 'tutorial flag read in class DamageReact', 'no Raven address named (c14c_tutorial_reads.tsv has no Raven-range row)', 'n/a', '')

# 7a
add('§7a', 'L298-318', 'AddDamageEffectModifier list and the class tag rows (Eve, Add, Rose, Ain, Luciel, Ara, Rena, Elsword)', 'no Raven tag row', 'n/a', '')
# 7b
add('§7b', A('`31fdbbb0` (skill-memo entry for `APPLIED_SKILL_ID`)'), 'skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0', 'Raven\'s SetEquippedSkillFuncBySkillSlotIndex 32c641a0 is the skill-switch head; the Raven script has APPLIED_SKILL_ID but no APPLY_MEMO key', 'CH1n', 'skill-switch head; also the S3d memo play-test line (CH6)')
add('§7b', A('`31fdbbb0` / `31fdbcc0` readers in class content'), '320284c0 and 3230c1a0 memo readers', 'Rose skills 31301 / 31321 and memo 22027: not Raven', 'n/a', 'S3d consumers show their lines when CH2 / CH3 bind the first memo-equipped skill (CH6 walk item)')
add('§7b', A('| `32c57660` (`+0x9688` = skill 24401 learned)'), 'Burning Nasod Hand: 32c57660 (+0x9688 = skill 24401 learned); 32c43120 @32c43ac4 (+0x4108 = 2 with ChangeDamageType); gate 32045340 @320453a0; June script marks 46 attacks AT_COMMAND, March none',
    '32c57660 = InitPassiveSkillState (class map CH4a); the key read and the = 2 write are inside CommonStateStart override 32c43380 (CH1j: Common* overrides 0x7c0-0x7ec); skill 24401 = RNSI_P_ROT_BURNING_NASOD_HAND (classes 17, 107, type 8); March X2GURaven.cpp:2287-2335 / :5111-5116; on March data +0x4108 stays -1 so the gate is inert (MODS.md:2555-2559)',
    'CH4a CH1j', 'the 24401 setter (CH4a) and the = 2 write (CH1j body of 32c43380) together; the 46 AT_COMMAND tags arrive with the script (7x)')
add('§7b', A('the per-class blocks in funcmap rows `329bc560`'), 'per-class SetDamageExceptionProcess blocks: funcmap 32c5c080 (Raven)',
    '32c5c080 (override 0x2b8, base 32045340, merged with 32c5bf80; March Raven has NO override): 24417 RNSI_P_RBM_OPEN_MIND (skill type in {7, 14..16}) -> 32007b10(0xf, 24417); 24420 RNSI_P_RRF_INFINITY_RAGE + trigger 0x12 -> DT_MAGIC, DamageEffect_RRF_ExplosiveHand_Impact_NEW; 24422 RNSI_P_RVC_WIN_AND_WIN + trigger 0x13 -> vslot 0x238 -> 329cf6d0',
    'CH4b', 'whole function, follow Elsword\'s 32d1e0e0 shape; needs IsAttackTrigger (7b/L120 row) and 24417 / 24420 / 24422 (CH4a data)')

# 7c
add('§7c', 'L335-359', 'alpha-tag sites (Eve, Aisha, Add, Rena, Elsword rows; HP-rate gate 322726f0)', 'c10a_alpha_sites.tsv has no address in 32c25a20-32c6a63d', 'n/a', '')
# 7e / 7d / 7g / 7f
add('§7e', A('lost under June\'s player'), 'unit-viewer effects lost under June\'s player', 'Raven not in the row', 'n/a', '')
add('§7d-7g', 'L376-413', 'damage-math hooks (7d), Rose lobby habit (7g), NPC anim-speed hook (7f), item-templet fields (7g)', 'no Raven function or key named', 'n/a', 'OTHER')

# 7h
add('§7h', A('Elsword 25 functions') if False else 'L422', 'Raven 32c2b360: UPDOWN_CANCEL_AFTER (+0x8ca4) up/down cancel branch in a state EventProcess',
    '32c2b360 = RSI_COMBO_ZZXX EventProcess (state id 172; ch0_class_map.tsv, CH1d); March X2GURaven.cpp:1477. Script UPDOWN_CANCEL_AFTER x2: RSI_COMBO_ZZXX and RRF_COMBO_ZZZX (CH1f). Member +0x8ca4 exists (C9h-4c)',
    'CH1d CH1f', 'inside that state\'s EventProcess; the second use is the RRF combo EventProcess (find it in the class map)')
add('§7h', A('`32024ff0` list variants'), 'EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE)',
    'script: EFFECT_SET_LIST x108, EFFECT_SET_LIST_EX x12, EFFECT_SET_LIST_FINAL x5 and _FINAL_ULTIMATE x5 (RSI_SA_RST_SHOCKWAVE, RSI_SI1_SOUL_TAKER_BERSERKER_BLADE, RSI_SA_RWT_HARPOON_SPEAR, _FIRE, _FIRE_OVERHEAT); engine reader 32024ff0 done', 'CH1 final', 'script data arrives with the script')
add('§7h', A('`EffectSetToPlay` entry `+0x34`'), 'POSSIBLE_NASOD_BARRIER / RELATE_SKILL_ID fields, user slots 0x4c4 / 0x4c8, SOUND_PLAY_EX keys', 'no Raven key or site named', 'n/a', '')
add('§7h', A('Class code writing `+0x3f00` directly'), '+0x3f00 writers, AddEffectSetDeleteCondition callers', 'no Raven address named', 'n/a', '')
add('§7h', A('`USEFUL_VALUE0..4` per-state float rows'), 'USEFUL_VALUE rows and getters', 'Raven script USEFUL_VALUE0 x6 (RSI_WAIT_STATE_CHANGE, RSI_DAMAGE_BOUNDING_LANDCRASH, RSI_SA_RBM_DEADLY_BLOW, RSI_SA_RBM_SHOOTUP_GENOCIDE, RSI_SA_RRF_MAXIMUM_DIVE, RSI_SA_RRF_POWER_BOMB); no Raven reader in c9h4f_useful_value_readers.tsv', 'CH1 final', 'rows = script data; no class reader')
add('§7h', A('`USI_WAIT_STATE_CHANGE` (June base id'), 'USI_WAIT_STATE_CHANGE registered in every class InitState', 'Raven script RSI_WAIT_STATE_CHANGE L495 (june-only, CH1a); InitState 32c66fe0 @313 id 130 (ch0_state_skill.tsv)', 'CH1m CH1a', 'registration CH1m; the state\'s callbacks are CH1a')

# 7i
add('§7i', A('`32c641a0`') if False else 'L444', 'SetEquippedSkillFuncBySkillSlotIndex Raven 32c641a0: drop the snapshot call, set state byte +0x11', '32c641a0 (funcmap 2024, vslot 0x7b4); X2GURaven.cpp SetEquippedSkillFuncBySkillSlotIndex; ch0_skill.tsv 71 rows (3 switches)', 'CH1n', 'head; skill cases follow their skills (CH2a-CH3d)')
add('§7i', 'L436-437', 'class code multiplies m_fNowStatePowerRate, never the entry\'s', 'every state port; the 2 June writers are in the L256 (d) row', 'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'fixed assignment (7i -> every skill phase)')

# 7j-7p
add('§7j', 'L448-459', 'DamageReact steps (Elesis / Lu-Ciel / Rose hooks); Rune Slayer magic reflect', 'not Raven', 'n/a', 'OTHER')
add('§7k', 'L462', 'class-set callback +0x8dc0', 'no Raven writer named', 'n/a', 'unowned engine item')
add('§7l', A('| Bounding user states |'), 'bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every class InitState + their tables',
    'Raven script: RSI_DAMAGE_BOUNDING_DOWN L11903, _LANDCRASH L11912, _UP L11925 (june-only, CH1b); March Raven has no bounding state', 'CH1p CH1m', 'hook list says CH1p (states with the script) and CH1m registers them; the states\' own functions are CH1b')
add('§7l', 'L470-474', 'other 7l rows (Add super-armour, Lu/Ciel target, +0x9175 byte, packet-buff refusal); nothing in a UCHAR', 'not Raven; X2GURaven.cpp / .h have no UCHAR state id (grep: 0 hits)', 'n/a', '7l state-id width check passes')
add('§7m', 'L480-481', 'NOTIFY_TO_OWNER_WHEN_DIE (Add only)', 'not Raven', 'n/a', '')
add('§7n', 'L491-494', 'user-sync class-skill float (Rena, Eve, Chung, Elesis writers)', 'Raven not a writer or reader', 'n/a', '')
add('§7o', 'L507', 'SET_LIMIT_MAX_G_SPEED in <prefix>DAMAGE_BOUNDING_DOWN (-4000) and Raven\'s RRF_DASH_JUMP_COMBO_XX (-5000)',
    'script: RRF_DASH_JUMP_COMBO_XX L3177 = -5000, RSI_DAMAGE_BOUNDING_DOWN L11905 = -4000; reader done (C3g)', 'CH1 final', 'script data, arrives with the script (RRF_DASH_JUMP_COMBO_XX is CH1f)')
add('§7o', 'L508-531', 'Add / Ara / Elesis / Rose / Lu-Ciel speed rows', 'not Raven', 'n/a', '')
add('§7p', A('| All classes | `TIME_STOP_IMMUNE_STATE = TRUE`'), 'TIME_STOP_IMMUNE_STATE = TRUE in the class script', 'Raven script 5 states: RSI_HA_RRF_INFERNAL_ARM L5808, RSI_HA_RBM_EXTREM_BLADE_READY / _ATTACK / _ATTACK_FINISH, RSI_HA_RVC_BURNING_BUSTER (all CH3d hyper actives)', 'CH1 final', 'script data; reader C3e-4 done')
add('§7p', A('| Chung | DamageReact `32c0cf90`'), 'DamageReact hit-stop sites (Chung 32c0cf90)', 'no Raven site (c3e3_stop_time_callers.tsv has no Raven-range row)', 'n/a', '')
add('§7p', A('| Elsword | March `THROW_WOODEN_PIECE_FrameMove`'), 'Elsword THROW_WOODEN_PIECE_FrameMove SetStopTime', 'Elsword only', 'n/a', 'OTHER')
add('§7p', 'L545 L548', 'Aisha binding circle forced stop', 'not Raven', 'n/a', '')

# 7q-7u
add('§7q', 'L558-562', 'Luciel down-state getters', 'not Raven', 'n/a', '')
add('§7r', 'L571-572', 'Chung cinematic-UI writes', 'not Raven', 'n/a', '')
add('§7s', 'L579', 'dungeon-registry per-class tables: X2DungeonManager.cpp:252-552 reads RAVEN already', 'March already reads RAVEN', 'n/a', '')
add('§7t', A('The class script:** every June class script sets the key'), 'SHOW_HYPER_CUT_IN_TIME in the class script', 'Raven script 3 states (L11416 RSI_RBM_HYPER_MODE, L11522 RSI_RRF_HYPER_MODE, L11651 RSI_RVC_HYPER_MODE); all three tables are june-dead-data (string absent from the exe; ch0_state_skill.tsv)', 'CH1 final', 'script data only; the key is read by a hyper-mode state June does not register for Raven. Raven\'s June hyper cut-in comes from UnitTemplet m_HyperCutIn (CH5)')
add('§7t', A('June\'s `UnitTemplet.lua`** (data036)'), 'June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cutin_*_Transcendence textures',
    'ch0_unittemplet_raven.tsv: 7 Raven rows; m_HyperCutIn on 106 / 107 / 115 (HQ_Cutin_RBM / RRF / RVC_Transcendence), no m_BerserkCutIn; m_fLobbyRotationY on all 7; 115 name STR_ID_617 -> STR_ID_17431; every STR_ID is in general.ess', 'CH5 CH3d', 'rows = CH5 (BOM source, core list UnitTemplet.lua); Transcendence textures = CH3d')
add('§7t', A('| Chung | unit vslot `0xa4` override `32c24b90`'), 'unit vslot 0xa4 berserk-by-chance override (Chung 32c24b90)', 'Raven has NO 0xa4 override (ch0_vtable_slots.tsv); it overrides 0x9c IsFullHyperMode 32c69f60 and 0xa0 SetFullHyperMode 32c68ff0 (class map CH1h; March X2GURaven.h:483-484 inline m_bHyperAura)', 'n/a', 'plan_june_side section 9')
add('§7t', 'L604-608', 'hyper active skills: cut-in shown whatever the option says (type 13)', 'Raven: type 13 ids 24600 / 24603 / 24601 / 24604 / 24602 / 24605 (classes 107, 106, 115)', 'CH3d', 'play-test: option Hide, use a hyper active -> the cut-in shows')
add('§7t', A('| Raven |') if False else 'L623', 'skill cut-in names -> June .gfx: June HQ_CutIn_Raven_BM_01; March HQ_CutIn_Raven_Blade_Master_1.dds (no .gfx)', 'June 329cd890 + 32078b70 (funcmap 3555); March X2GURaven.cpp class cut-in table + InitState index switch', 'CH5', 'CH0b stages the .gfx files; replace s_SkillCutInSet and the switch')
add('§7u', A('| Raven | `X2GURaven.cpp`: `RAVEN_SEVEN_BURST_1/2_HYPER`'), 'March-only effect templets the re-adoption dropped: RAVEN_SEVEN_BURST_1/2_HYPER, RAVEN_MAXIMUM_CANNON_HYPER / _FLY_HYPER, RAVEN_OVER_TAKER_GUARDIAN_STRIKE[_HYPER], RAVEN_OVER_TAKER_XX_FRONT_X, EffectSet_RAVEN_VJAVELIN',
    'c11a_march_only_templets.tsv (17 Raven rows): all march-consumer in X2GURaven.cpp; June uses DamageEffect_RRF_ExplosiveHand_Impact, EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE (+ _HYPER_MEMO), EffectSet_RAVEN_VJAVELIN_LAND, Effectset_A_RWT_FLAME_SWORD_MEMO. JUNE_BUGS 200 (RRF_ExplosiveHand_Impact)',
    'CH2a CH2c CH1f CH3b', 'take June\'s caller, drop the March names: SEVEN_BURST / MAXIMUM_CANNON (skills 24003 / 24000, all classes) CH2a; GUARDIAN_STRIKE / VJAVELIN (24006 / 24004, classes 17, 107) CH2c; XX_FRONT_X is the RSI_OVER_TAKER_COMBO_XXfrontX combo (CH1f); ExplosiveHand_Impact = Infinity Rage 24420 (CH4b); Flame Sword memo effect set CH2a')
add('§7u', A('- Chung: `X2BuffTemplet.cpp:1695-1706`'), 'Chung grenade memo / Rena / Aisha / Eve / Chung March-only templets', 'not Raven', 'n/a', 'OTHER')

# 7v-7ab
add('§7v-7ab', 'L652-711', 'Lu/Ciel and Rose rows; CUSTOM_RENDER_STATE users; basic-item pair (every class)', 'not Raven (the Raven script has no CUSTOM_RENDER_STATE); the basic-item reader 329657d0 includes m_RavenLeftArmItemID (L702): see the left-arm row below', 'n/a', '')
add('§7aa', A('`329657d0` reads the unit templet\'s *basic* items'), 'm_RavenLeftArmItemID basic-item pair (every class): June reader 329657d0 / UpdateEquipCollisionData 31fd8c00 vs March arm hiding X2GUUser.cpp:41738-41760', 'engine plan row; owner named by CH0 item 8 (ch0_overheat.tsv)', 'CH4c ?', '? left-arm item owner is item 8\'s deliverable')
add('§7x', A('June\'s class scripts tag combo'), 'combo hits carry June\'s AT_COMMAND tag; adopt the script with its tags', 'Raven script 46 AT_COMMAND (March script has none: 0 in bytecode); stand-in SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN stays until all played classes are ported', 'CH1p', 'script data (with the combo states CH1d-CH1g2); pairs with the Burning Nasod Hand = 2 write (7b L327)')
add('§7y', 'L678-684', 'GetLandPosition bMonster in class code', 'w1c_land_position_callers.tsv has no Raven-range row (March X2GURaven.cpp has 2 GetLandPosition calls: check shape in CH1)', 'n/a', 'equivalent forwarder shape')

# 7ae
add('§7ae', 'L714-722', 'an id of a listed type with no case plays the class\'s first state (Raven: first state of the switch)', 'Raven: 254 rows over 126 ids, 247 no_state, 7 has_state (id 900 once per class); ch0_s3_baseline.txt', 'CH2a CH2b CH2c CH2d CH3a CH3b CH3c CH4a', 'each skill phase binds its ids (class 4: 14 rows, 16 / 17 / 25: 28 each, 106 / 107 / 115: 52 each); types 1 / 8 (passives) = CH4a')
add('§7ae', 'L728', 'rows with no June SKILL_ID name', 'Raven: 0 of 254 rows lack a June name (the 7 non-RNSI rows are id 900)', 'n/a', 'no unnamed-id task')
add('§7ae', 'L729-733', 'types 14-16 / 17 / 18 named in the class switch', 'Raven 32c641a0 listed as class material in s3c_skill_type_readers.tsv; Raven types 14 / 15 / 16, 6, 8, 13', 'CH1n CH2a CH2b CH2c CH2d CH3a CH3b CH3c', '')
add('§7ae', 'L734-740', 'Ain gauge charge (32622bd0 / 32622e80)', 'Ain only', 'n/a', '')
add('§7ad', A('animation names March\'s motions have and June\'s same-named files lack'), 'stage June Motion_Raven* files with the class script; Raven\'s OT_ComboX3Down..OT_ComboX5Down, OT_SP1a_ChargedBolt renamed',
    'r12b_anim_loss.tsv rows 41-44: Motion_Raven_OT.x lacks OT_ComboX3Down / X4Down / X5Down / OT_SP1a_ChargedBolt (the motion question, ch0_motion_question.tsv); R12b already took June\'s Motion_Raven.x', 'CH0b CH1 final', 'CH0b stages June\'s Raven motions; rerun the census after staging (CH1 final); JUNE_BUGS row for OT_SP1a_ChargedBolt in CH2c if June\'s file lacks it')
add('§7ac', 'L770-778', 'CanDown call shape (Ara / Aisha / Luciel / Rena rows)', 'w1f_candown_callers.tsv has no Raven-range row (March X2GURaven.cpp has 9 CanDown calls, forwarder shape)', 'n/a', 'no change (equivalent)')
add('§7af', 'L786-802', 'camera linear tracking / SUB_CAMERA sequencer', 'Raven script SUB_CAMERA0-7 / CAMERA keys x68 are plain script data (LeftDirCamear read by PlayLuaCamera 31fb6af0, already ported); no class site named', 'n/a', 'engine')
add('§7ag', A('S5 made the engine 5 + 5'), 'pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every > 3 / -4 decode; decode slot + 1, never SAKP_Q..R',
    'Raven has its own March 4 + 4 decode: X2GURaven.cpp:5854-5871, CountUsedSkill :5780, m_eSpecialAttackKeyPressed write :5876, GetSkillSlotIndexUsed :10842; X2GUUser.cpp:26532 stand-in lists Raven among unported classes', 'CH1p', 'convert to June\'s A 0-4 / B 5-9 and join UT_RAVEN to the June set under _LIVE (as UT_ELSWORD)')
add('§7ag', 'L807-809', 'TC slot A-5 / B-5 states and the Stage 6 skips', 'Raven transcendence 106 / 107 / 115', 'CH3d', '')
add('§7ah', A('`Static_EL_SKILL_B(_B)`'), 'Static_EL_SKILL_B(_B) / _R(_B) Elsword way of sword; UpdateElswordSkillSlotEffect', 'Elsword only', 'n/a', 'OTHER')
add('§7ai', A('S3e gave `CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID`'), 'pass June\'s skip / adjust / level / creator at every ported BFI call', 's3e_bybfi_callers.tsv has no Raven-range row; March X2GURaven.cpp calls SetBuffFactorToGameUnitByBuffFactorID / PushBuffFactor in its skill states', 'CH2a CH2b CH2c CH2d CH3a CH3b CH3c', 'each skill phase passes June\'s arguments at the calls it ports')
add('§7ai', A('**`CX2GUUser::JuneShowSkillNameStateString( iSkillID )`**'), 'JuneShowSkillNameStateString callers', 'Raven\'s caller (vtable 0x80c NoStateChangeActionFrameMove) is in the 0x7f0+ band; the class map names it', 'CH1k', 'call where June does')
add('§7aj', 'L865-881', 'tree-UI class pieces', 'no Raven piece', 'n/a', '')
add('§7ak', 'L884-887', 'decide-page ACK class pieces', 'Aisha, Add only', 'n/a', '')
add('§7al', A('**New CX2GUUser hooks for every class**'), 'new CX2GUUser hooks for every class: 0x654, 0x850, 0x860, 512, UpdateEquipCollisionData 0x638', 'Raven vtable (ch0_vtable_slots.tsv) overrides 0x800 (512) = 32c43bf0 (class map CH1k) and 0x80c = 32c25c40 (CH1k); not 0x654, 0x850, 0x860, 0x638', 'CH1k', 'the 0x800 body is the per-class skill use / cooldown part of SpecialAttackEventProcess')
add('§7am', A('| Raven | `32c57660` |'), 'delegate-list registrations in InitPassiveSkillState: Raven 32c57660 pushes A 32c3df50, C 32c4e5b0, A 32c28160; March\'s own pushes (Raven among them) are replaced',
    'class map: 32c3df50 and 32c28160 CH4a (called by thunks), 32c4e5b0 CH1o (no phased caller 32c6a2f0, "read it"); March pushes at X2GURaven.cpp:2114-2641', 'CH4a CH1o', 'each callback whole, replace March\'s pushes with June\'s three')
add('§7an', A('| vslot `0x73c` | Add'), 'vslot 0x73c / 0x748 overrides: Raven 32c3c7f0 (0x73c) and 32c3c820 (0x748, soul gain when hit = 1 + +0x95c4)', 'both uncarved, 13 insns, no calls (ch0_class_map.tsv: CH4a)', 'CH4a', 'override only; base and DamageReact @32049d8a call already ported')
add('§7an', A('| `GetCharacterSpecializationB()` (June `+0x6d04`) readers'), 'GetCharacterSpecializationB() readers Raven 32c49410 / 32c507da', 'both class-map rows; Specialization B of the Nasod rage mechanic', 'CH1q CH4c', 'CH1q (class mechanic) or CH4c (readers whole): item 7\'s rage map names which (ch0_rage.tsv)')
add('§7ao', A('- Raven: `HyperModeStart` `31ffd490`'), 'T5 branch: HyperModeStart 31ffd490, skill 24419 (0x5f63) learned while awake -> vslot 0x23c( 24419, 1, 1 )', '31ffd490 (funcmap 4643); 24419 = RNSI_P_RRF_OPEN_MIND (class 107)', 'CH3d', '')
add('§7ao', A('| Raven | `32c4e7c0` (hyper effect start'), 'transcend: Raven hyper effect start 32c4e7c0 (vslot 49) returns at once for a transcended unit', '32c4e7c0 = HyperModeBuffEffectStart (class map override 0xc4, CH1h)', 'CH3d CH1h', 'the transcended-unit return is CH3d; the function body is CH1h')
add('§7ao', 'L940-945', 'every class also brings: emulator allow-list id, LevelUpEvent TYPE row + DLG_Transcendental_Change_Guide_Raven, JuneGetHyperModeTableName cases (106 / 107 / 115), HQ_Cutin_*_Transcendence, hyper-class skills + TRANSCENDENTAL_SKILL_DATA, helpers 31fcac90 / 31fcad70 / 31fceab0 / 31ff1e80',
    'textures POPUP_TR_R_BLADE / R_RECKLESS / R_VETERAN (June data215), HQ_Cutin_RBM / RRF / RVC_Transcendence; 31fceab0 callers in the Raven range (class map)', 'CH1m CH3d CH5', 'CH1m = GetHyperModeTableName; CH3d = skills + TRANSCENDENTAL_SKILL_DATA; CH5 = cut-in, guide, pictures')
add('§7ao', A('Any class with an aura-accelerator skill'), 'ApplyAuraAcceleratorBuffToGameUnit 32028790 for a class with an aura-accelerator skill', 'Raven has no 0x7b0 override', 'n/a', '')
add('§7ap', 'L978-986', 'June slots CH1o2 gave a base: 0x3ac / 0x3b0 / 0x56c / 0x584 / 0x65c / 0x6a8', 'Raven vtable (ch0_vtable_slots.tsv) has none of those slots', 'n/a', 'confirmed')
add('§7aq', A('- **Raven:** `32e6d900`'), 'three-orb ball on the own gauge UI: 32e6d900, raven_powrUp_UI_01 at (74, 60) x resolution; March CX2RavenMyGageUI::UpdateHyperOrb uses GetUIMajorParticle() but the template is only in GameMajorParticle7.txt', 'June 32e6d900 (backlog Stage 7 hyper-ball counts: see the 32cb6290 note below)', 'CH1q CH5', 'port June\'s body (Eve did June_UpdateHyperBall 32e6a210); gauge UI is CH1q (ch0_rage.tsv)')
add('§7aq', 'JUNE_PORT_BACKLOG_ORDER.md:236', 'JUNE_PORT_BACKLOG_ORDER.md:236 "Raven 32cb6290( 1 )" (training-game F7 gauge step, June 322ed400)',
    'WRONG CLASS. 322ed400 switches on the unit type: case 3 (UT_LIRE) -> 32cb6290( 1 ); Raven is UT 4 and has no case (falls to the default return, as Aisha 2 and Eve 5). 32cb6290 lies in Rena\'s range (32c6a8d0-32cb6c6e, funcmap 32cad510 CX2GULire_ElvenRanger)', 'n/a', 'correct the backlog line: the step belongs to Rena (Lire), Raven has none')
add('§7ar', 'L998-1001', 'item class compatibility for classes March lacks', 'Raven classes exist in March', 'n/a', '')
add('§7as', 'L1004-1021', 'upgrade-weapon glow for Lu/Ciel, Rose, Ain', 'not Raven', 'n/a', '')
add('§7at-7au', 'L1024-1034', 'Ara add-N-stacks buff path; Ain buff-icon text hook', 'not Raven', 'n/a', '')
add('§7av', 'L1037-1044', 'class state animation rate, user vslot 0x824', 'Raven has no 0x824 override (ch0_vtable_slots.tsv; none listed in the checklist)', 'n/a', 'confirmed')

with open(HERE + 'ch0_checklist_rows.tsv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(['section', 'checklist_line', 'row', 'raven_evidence', 'phase', 'note'])
    w.writerows(R)
print(len(R))
print(collections.Counter(r[4].split()[0] if r[4] else '' for r in R).most_common(30))
print([i for i, r in enumerate(R) if '?' in r[4]])
