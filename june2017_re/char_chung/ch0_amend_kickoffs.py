"""char-chung CH0 item 11: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, and write the kickoffs CH0 adds.
    python june2017_re/char_chung/ch0_amend_kickoffs.py [--dry]
Per phase: functions (ch0_class_map.tsv col 7) with inside / new, states (ch0_state_skill.tsv col 8), skills (part 2 col 6),
checklist rows (ch0_checklist_rows.tsv col 5), callees CH0 routed to an Opus helper phase (HELPER), plus the hand notes in NOTES.
Replaces the placeholder paragraph only; a kickoff whose section is already filled is left alone.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-chung')
PLACE = '(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)'
DATE = '2026-10-31'


def tsv(name):
    return [l.rstrip('\r\n').split('\t') for l in open(os.path.join(HERE, name), encoding='utf-8', errors='replace')
            if l.strip() and not l.startswith('#')]


cm = [r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn']
ss_all = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8', errors='replace').read().split('\n')
cut = next(i for i, l in enumerate(ss_all) if l.startswith('# part 2'))
states = [l.split('\t') for l in ss_all[:cut] if l and not l.startswith('#') and not l.startswith('state\t')]
skills = [l.split('\t') for l in ss_all[cut:] if l and not l.startswith('#') and not l.startswith('skill_id')]
ck = [r for r in tsv('ch0_checklist_rows.tsv') if r[0] != 'section']
ui = {r[0]: r for r in tsv('ch0_unmapped_identified.tsv') if r[0] != 'callee'}

# CH0's grouping of the 26 needs-Opus callees: helper phase -> callees (the Opus phases CH1m / CH1n / CH4a map their own)
HELPER = collections.OrderedDict([
    ('CH1d0', ('CH1d', ['31fd4290', '31fd4340'])),
    ('CH1j0', ('CH1j', ['32008660', '31fe4870', '31fe9840'])),
    ('CH1k0', ('CH1k', ['31d28b30', '3226e020', '3226f870', '31ff1950', '32497d80', '32a37130'])),
    ('CH1l0', ('CH1l', ['31fcd740', '31fd2f00', '31fe21a0', '31ffe990'])),
    ('CH1l30', ('CH1l3', ['31d70b60', '31fd7640', '3288fe90', '31f66620'])),
    ('CH2b0', ('CH2b', ['31fcf430', '329cc340', '31fc8900'])),
])
OWN = {'CH1m': ['32078bc0'], 'CH1n': ['31fdf970', '3207c1a0'], 'CH4a': ['31fc94d0']}

NOTES = {
 'CH0b': [
  '**Stage list `june2017_re/char_chung/ch0b_stage_list.tsv`: 5,093 files** (4,788 missing in March, 305 March copies; 8,518 more are already June bytes). '
  '`kom.py stage-june --dry-run`: 35 koms, 5,093 staged (4,789 new, 304 replaced, 0 identical), **0 refused**; one new kom, `data204` (54 files). '
  'Routes a / b / c unioned in `ch0_assets.tsv` (13,947 rows); owner rules keep NPC copies (25), Stage 9 UI (11), Stage 17 pet (10) and june-dungeons tutorials (181) out of the list.',
  '**Hold the class motions for CH1 final (checklist 7ad).** March\'s script plays all three animations June dropped (`ch0_motion_question.tsv`): '
  '`SI_SA_CFG_IRON_HOWLING` (gone from every June file; June\'s own `CSI_SI_SA_CFG_IRON_HOWLING_HYPER` still names it: JUNE_BUGS row in CH1e), '
  '`SI_A_CIP_SUDDEN_BURSTER` and `SI_SA_CIP_BURST_WOLF` (moved CIP -> June `Motion_CHUNG_CFG.x`). Stage and pack everything, but leave the 10 differing class '
  'motions (`Motion_CHUNG_CDC / CFG / CHG / CIP / Common / CSG / CTT / Emotion / Field / Lobby .x`) out of the copy until CH1 final; the 53 June-only emotion '
  'motions and the rest go now. Say in the hand-off which kom holds them back.',
  '**Skill icons:** `skillicons.py check` over 106, 900, 26000-26064, 26200-26238, 26400-26420, 26600-26605 = FAIL (133 skills, 22 misses) until '
  '`DLG_SKILL_ChungActive_05` (absent), `DLG_SKILL_ChungActive_04` and `DLG_Chung_SKILL_ScreenShot_04` (March `.tet` smaller than June\'s) are copied: all in the list. '
  'Transcendence passives sit on `DLG_SKILL_Passive_13` (26415-26420) and `_12` (106), already June bytes. Gate: the check PASSes after the copy.',
  '**In the list too:** `POPUP_TR_I_PALADIN / D_CHASER / T_TROOPER` (+ `.tet`, June data215, names from `DLG_Transcendental_Change_Guide_Chung`), '
  '`DDS_Chung_CTT_Skill_Normal / _Rage`, `HQ_CutIn_Chung_Basic`, `HQ_CutIn_Chung_CASH_*`. The six `HQ_Cutin_{CIP,CDC,CTT}[_1]_Transcendence.gfx` are already June bytes.',
  'Effect closure `ch0_effect_closure.tsv`: 7,263 rows, 1,945 names, 64 missing (50 sounds), 15 June never defines.',
  'The builder is `june2017_re/char_eve/ch0_build_assets.py chung` (CHARS entry `chung`, new optional keys `extra_lua` / `skill_icon_ranges`).'],
 'CH1a': ['Guard key `CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE`: March already reads it (`LoadStateIDCanBeChangedGuardState`, an `inside` helper).',
          'Cannon-ball counter calls (`32bf3e40`, `32bf5280`, `32c24c80`) are CH1q\'s: declare them with `// June <va>: body in CH1q`.'],
 'CH1b': ['June registers `CSI_RIDING_DAMAGE_BACK` with no script table (CH1c\'s riding row; JUNE_BUGS row there).'],
 'CH1c': ['Only 2 class functions: June moved the riding / pepper / dance callbacks into `CX2GUUser` (the registration rows point at `329c....` / `3207....` user thunks). '
          'CH1c writes the 2 and checks that the user-side bodies are Aisha\'s / Eve\'s ports (`X2GUUser.*`, flags `_ARME_LIVE` / `_EVE_LIVE`): widen their `#if` to `_CHUNG_LIVE`, never copy. '
          '`CSI_RIDING_DAMAGE_BACK` is registered with no script table: JUNE_BUGS row.'],
 'CH1d': ['Split by CH0 (59 functions): CH1d = `CSI_COMBO_Z*` / `CSI_COMBO_X*`; CH1d2 = dash / jump combos and `CSI_ATTACK_*`.',
          'Opus helper phase **CH1d0** runs first (`31fd4290`, 9 callers, and its neighbour `31fd4340`).',
          '`32bf3d30` (UpCannonBallCount) is CH1q\'s: declare it.'],
 'CH1d2': ['`CSI_ATTACK_DASH_X` is defined twice in June\'s script (the last table wins): JUNE_BUGS row. The same function serves `CSI_CSG_COMBO_DASH_X` / `CSI_CIP_COMBO_DASH_X` (class-gated names, `ch0_reg.tsv` `~` rows).',
           'Uses CH1d0\'s `31fd4290` mapping.'],
 'CH1e': ['`CSI_SI_SA_CFG_IRON_HOWLING_HYPER` (a CH3a2 skill state) plays `SI_SA_CFG_IRON_HOWLING`, which no June motion file has: JUNE_BUGS row when CH3a2 writes it; noted here because the `XXX_CSI_CFG_*` dead tables sit beside it.',
          'The 2 `XXX_CSI_CFG_COMBO_DASH_JUMP_ZXX*` tables and `_CSI_CFG_COMBO_XXFRONTZ` are dead data (no June registration): nothing to write.',
          '`CSI_CFG_COMBO_XXFRONTZ` / `ZZ` are registered for 20 and 110 (32c1d1d0 case 0x6e falls into 0x14).'],
 'CH1f': ['`LEAP_ATTACK_LANDING` is one state (id 228) whose table name is picked at run time (class 110 -> CIP name, else CFG name); one set of callbacks `32c23430` / `32c23440` / `32c23f80`.'],
 'CH1g': ['Split by CH0 (50 functions): CH1g = CSG dash combos; CH1g2 = X-front, dash-jump and Head Shot.'],
 'CH1g2': ['`EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE` is named by June\'s script and defined in no June EffectSet file: keep it, JUNE_BUGS row (logs `[RANOX_EFFECT_MISSING]`).',
           '`CSI_SI_A_CSG_HEAD_SHOT` (id 236) is registered by InitStateCommon\'s class-21 gate, so it is class core; its case id 26220 is `RNSI_A_CSG_HEAD_SHOT_TRANS` (no tree row, 26225 likewise).',
           'Memo lookups `31fdbcc0` at `32bfcbb0` / `32bfcfd8` / `32bfd1e7` ride with their functions.'],
 'CH1i': ['`CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT` (id 277) is registered by the class-27 gate: class core here; the passive data is CH4a.'],
 'CH1j': ['Opus helper phase **CH1j0** runs first (`32008660`, `31fe4870`, `31fe9840`).',
          '`CSI_CTT_MOBILE_ARMORED_MODE_X` (id 308) / `_Z` are registered by the class-117 gate.'],
 'CH1k': ['Opus helper phase **CH1k0** runs first (`31d28b30`, `3226e020` / `3226f870` weapon render pulse, `31ff1950`, `32497d80`, `32a37130`).',
          'InitComponent `32bf4900` (slot 0x3c) is written whole here, with its key reads: `COMMAND_HIT_ADD_MP_BONUS` (+0x98c4, consumer CH1p), `SIEGE_ANGLE` (+0x261a..d) / `MOBILE_SIEGE_ANGLE` (+0x261e..21) (consumers in the siege skill states), `EXTRA_CANNON_BALL_DAMAGE_RATE` (+0x2626 / 7, CH1q).',
          'The ctor `32bf1040` is kind `not-Chung` in the map only because its caller (`3230f409`) is outside the range: it is Chung\'s ctor and the source of the ctor / dtor offset map. Dtor body `32c0c4e0`, slot 0 `32c25880`.',
          '7p: DamageReact `32c0cf90` with the hit-stop.'],
 'CH1l': ['Opus helper phase **CH1l0** runs first (`31fcd740`, `31fd2f00`, `31fe21a0`, `31ffe990`; the last three are called only from `32bdc750`).',
          'Gauge snapshot overrides 0x740 / 0x744 (`32bed760`, `32bf4600`) and 0x688 `32bf44d0` touch the cannon-ball counter; they stay here and call CH1q\'s helpers.'],
 'CH1l2': ['CommonStateStart `32bf3990` (0x7c4) reads the June-only per-state key `USE_CANNON_BALL%d` into +0x98d8: written whole here, the consumer is CH1q.'],
 'CH1l3': ['Opus helper phase **CH1l30** runs first (`31d70b60` GetCustomizeManager, no March system: stand-in, Stage 12 owns it; `31fd7640` vslot 0x828 base predicate; `3288fe90` skill-tree slot accessor; `31f66620` for CH1l4).',
           'Slot 0x800 `32bf5130` is the one 7al hook Chung overrides.'],
 'CH1l4': ['`31f66620` (pooled alloc) is mapped by CH1l30.', 'The hyper-count-3 timer skip `32bf4680` and `32bf3f60` are CH1q\'s: declare them.'],
 'CH1m': ['InitState `32c22540` is **not carved in Ghidra** (no decompile): its 4 registrations were read from the listing (`ch0_reg.tsv` rows `@32c2277e` / `@32c22852` / `@32c2291f` / `@32c229db`: START / WIN / LOSE roll `31fe22d0`, `CSI_WAIT_STATE_CHANGE` id 130). '
          'It then sets +0x7d04 = 0xa2 and +0x4088 = 13 and the cut-in set index +0x8ac8 by class: 6 -> 0, 20 -> 1, 21 -> 2, 27 -> 5, 110 -> 3, 111 -> 4, 117 -> 6 (byte table `32c22af8`).',
          'InitStateCommon is two functions: `32c12a30` (110 rows, no gate; `USI_HYPER_MODE` id 23 named by `31fdd490` at line 737) and `32c1d1d0` (116 rows, class-gated on *(+0x867c)+0x38, two switches with fallthroughs 110 -> 20 and 117 -> 27; line ranges in `ch0_build_state_skill.py` GATES). `ch0c_listing_check.py` reproduced 188 / 188 distinct rows.',
          'June registers six names with no script table (`CSI_RIDING_DAMAGE_BACK`, `USI_EVENT_SNIPER_*` x3, `USI_EVENT_PROTON_BOMB_*` x2 from the base `3202b110`) and drops three March states (`CSI_SI_A_CDC_LEG_SHOT`, `CSI_SI_A_SIEGE_SHELLING_READY_HYPER`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL`: June wins).',
          'Skill 26032 registers `DUAL_BUSTER` a second time with id 336 (0x150) (listing, `ch0_skill_listing_rows.tsv`). `32078bc0` (unknown, called 21x by the registrars) is yours to map.'],
 'CH1n': ['`32c1f530` head: the memo switch `31fdbcc0` call at `32c1f63d`; the dirty-state batch `31fdf970` / `3207c1a0` (no March form) is yours to map.',
          'The skill rows were rebuilt from the listing (`ch0_skill_listing_emu.py` -> `ch0_skill_listing_rows.tsv`): 16 of 27 address-order guesses were wrong (callbacks belong to the END / FINAL / LANDING state registered through the shared tail at `32c2219e`).'],
 'CH1o': ['June-only overrides named by CH0 (`ch0_hand_names.tsv`): `ShownSkillCoolTime` 0x6a8 `32bdba60`, `IsNotCommonMoveState` 0x828, `GetDefenderDamageRate` 0xb8, `UpdateSpineAimRotation` 0x774, `AfterInitEquippedActiveSkillState` 0x688, `StoreGageSnapshot` / `RestoreGageSnapshot` 0x740 / 0x744; `?` rows (0x670, 0x2e8, 0x2ec, 0x824, 0x1b8, 0x748) are named from the body by the class pattern.'],
 'CH1p': ['Keys: `COMMAND_HIT_ADD_MP_BONUS` (read in InitComponent, +0x98c4); `IMMUNE_TO_SUPER_ARMOR_DOWN` / `IMMUNITY_EDT_LIST_AT_THIS_STATE` are read by `CX2GUUser` `31fdaaa0`: backlog Stage 2, not here. `HEAD_IK*`, `NOMAL_CAMERA*_RATE`, `ATTACK_ACTION_TYPE` are in no June reader (`ch0_keys.tsv`).'],
 'CH1q': ['**Cannon-ball map `ch0_cannonball.tsv` (196 rows):** 111 per-March-function rows, 18 sites (UI, gauge data, game readers, sync), 67 June-only counter touchers (state callbacks: they ride with their states\' phases and call this phase\'s helpers).',
          'Gauge at unit +0xc04, fields equal to March\'s `CX2ChungGageData`: now +0x110, max +0x11c, extra now +0x128, extra max +0x134, berserk +0x140 / +0x141, cannon-changed +0x142; now / extra set through `3231d7a0` / `3231d880`.',
          'This phase owns the counter API (`ch0_hand_phases.tsv`): `32bf3d30` up, `32bf3e40` consume, `32bf3f60` set total (%7 split), `32bf4040` set with floor, `32bf5280` use cannon (Reload.ogg), `32bf5430` start counts (`CANNON_BALL_COUNT_AT_DUNGEON_START` / `_PVP_START`; March reads one key `CANNON_BALL_COUNT_AT_DUNGEON_OR_PVP_START`), `32bf4680` hyper-count-3 timer skip, `32c24c80` / `32c24d00` / `32c24d40` / `32c24db0` setters, and `CSI_CHARGE_CANNON_BALL`.',
          'Outside the class: UI `32e6d6c0` InitUI, `32e63470` InitCannonBallUI, `32e63990` UpdateCannonBallCountUI (`DLG_Chung_Cannonball_NEW.lua`); gauge-data update `32a34720` (uncarved); DamageEffect `USE_CANNONBALL` reader `31f7aab0 @31f7c535`. `FIX_CANNON_BALL_SYNC_CALCULATION` is defined (Always.h:1993): its `#ifdef` arms at X2GUChung.cpp:6350 / 6369 are live.',
          'Doubtful pairings to settle first: GetTotalCannonBallCount -> `32bed560`, InitRemodelMagazinePassiveSkill -> `32bf4900`, CreateNotEnoughMPEffect -> `32bf3990`, Chung\'s InitWakeOrb among `32e5b310`-`32e5b8a0`. June\'s `GameEditCommandList.lua` binds `AddChungCannonBall_Cheat` / `AddChungExtraCannonBall_Cheat` (unbound in March): dev commands, port only if the dev console needs them.'],
 'CH1': ['Script: `luasrc.py adopt-june data036 Chung_Iron_Cannon.lua`. `Targeting_Chung`, `Remote_Mortar`, `Remote_Mortar_APINK` (data037) are already June\'s content (adopt dry run: identical); `luabind.py check` PASS (22 / 25 / 9 methods).',
         'Copy the class motions CH0b held back (checklist 7ad) together with `_LIVE`.'],
 'CH2a': ['Siege Shelling (26201: `CSI_SI_A_SIEGE_SHELLING_*`, 2 numerical-only tables) and Back Blast (26200) are class 6 in June\'s tree: here, not CH2c.',
          'The 7 unnamed ids (26027-26029, 26216-26219; 49 tree rows) have no SKILL_ID name in either Enum (`ch0_unnamed_ids.tsv`; 26027-26029 collide only with `XCCUM_MGR_GROUP_*`): raw id + one-line comment with the templet\'s name.'],
 'CH2b': ['Opus helper phase **CH2b0** runs first (`31fcf430` master-ultimate memo lookup, `329cc340` vslot 0x524 predicate, `31fc8900`).', 'Brutal Swing (26005, numerical-only x2) is class 20 / 110: here.'],
 'CH2c': ['**CH3b0 (summons) runs before this phase:** Quantum Ballista\'s READY FrameMove `32bfe730` spawns `NUI_TARGETING_CHUNG` / `_NOT_SHOW_OTHER_PLAYER` (also Artillery Strike\'s). 7ai L835 (BFI 452 at `32bfea69`) is in that function: here.',
          'Trick Shot `32bfdd70` replaces March\'s `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell` caller (7u).'],
 'CH2d': ['7 l.280 call sites `32bec286` (Chaos Cannon) / `32c058a0` (Big Bang Stream) ride with their states.'],
 'CH3b0': ['**Placed before CH2c** (Depends: CH1 review; CH2c waits for this review). Spawn sites (`ch0_summons.tsv`): `32bfea4d` (via `31f8b010` CreateNPCReq forwarder, 863 `NUI_TARGETING_CHUNG`) and `32bfe9ae` (via `322e67c0`, 1419 `_NOT_SHOW_OTHER_PLAYER`) in `32bfe730`; `32c0b97d` (via `322e67c0`, 2525 `NUI_TARGETING_CHUNG_HEAVY_FIELD`) in `32c0b7f0` Heavy Field READY; `32c10f99` (1100 `NUI_REMOTE_MORTAR`) in `32c10d20`, called from Remote Mortar FrameMove `32c12470`. One spawn rule, two entry points: map both.',
           '**2525 is June-only:** its NPCTemplet row (June `NPCTemplet.src.lua:21708`, LUA_FILE_NAME `Targeting_Chung.lua`), the `NPC_UNIT_ID` member in the core `Enum.lua` and the C++ enum (`X2UnitManager.h`, under `JUNE_ENUM_NAMES`) are this phase\'s.',
           'March sites: CreateNPCReq at X2GUChung.cpp:17202 / 17205 / 22611; self-destruct uses of 863 / 1419 at 286, 375, 5605, 7187, 7235, 17176 (both arms of `BALANCE_PATCH_20131107`: `cl /P`), 23731; X2Game.cpp:9488 / 18190 / 18238 / 18490; X2GUUser.cpp:26427.'],
 'CH3a': ['Split by CH0 (51 functions): CH3a = CIP-named skill states; CH3a2 = the CFG / CHG-named skills June\'s tree gives class 110.'],
 'CH3a2': ['Iron Howling (`CSI_SI_SA_CFG_IRON_HOWLING*`): June\'s `_HYPER` state plays `SI_SA_CFG_IRON_HOWLING`, which no June motion file has (CH0 motion question): JUNE_BUGS row. Its `_END` StateEnd / EventProcess (`32bd9810`, `32be13b0`) are `new`.',
           'Wonder Wall (`CSI_SA_CHG_WONDER_WALL*`) and the CFG guard states are class 110 skills in June\'s tree.'],
 'CH3b': ['7r row 1 (Artillery Strike `32bff3d0`) and 7ac (`@32bfe8c3`). `CSI_A_CDC_RAPID_CANNON_READY` is defined twice in June\'s script: JUNE_BUGS row.'],
 'CH3c': ['7r row 2 and 7n (`32c0ba60`, Heavy Field, June-only skill 26064; spawn by CH3b0). Remote Mortar `32c12470`. Mark of Commander 26214 (l.287 readers in CH4b). `CSI_SA_CTT_HEAVY_FIELD_END_StateStart` (`32c0c1c0`) is `new`.'],
 'CH3d': ['5 hyper-state functions; no transcendence-memo case in Chung\'s switch (Eve had 12). 26220 / 26225 are `_TRANS` ids with no tree row.'],
 'CH4a': ['22 passive / buff ids with no state (part 2 of `ch0_state_skill.tsv`); `31fc94d0` (called by InitPassiveSkillState `32c16be0`) is yours to map.'],
 'CH5': ['`ch0_unittemplet_chung.tsv`: June adds `m_fLobbyRotationY` (6: 10, 20: 10, 21: 20, 27: 1, 110: 20, 111: 30, 117: 25; March ignores it) and for 110 / 111 / 117 `m_BerserkCutIn` / `m_HyperCutIn` (`HQ_Cutin_{CIP,CDC,CTT}_1_Transcendence` / `_Transcendence`). Tactical Trooper `name` STR_ID_9739 -> STR_ID_17435: present (`StringID_def`, General.ess row 17435 "Tactical Trooper"). Every other key equal.',
         'June\'s cut-in set index by class (InitState `32c22540`): 6 -> 0, 20 -> 1, 21 -> 2, 27 -> 5, 110 -> 3, 111 -> 4, 117 -> 6.',
         'StatTable: 110 / 111 have 99 live rows each; the 19 extra are commented copies, equal (no duplicate to route).'],
 'CH6': ['S3 baseline (`ch0_s3_baseline.txt`, `s3_no_state.py --family X2GUChung.cpp --preprocess`): 261 rows, 254 no_state, 7 has_state.'],
}

NEW_KICKOFFS = {
 'CH1d2': ('CH1d', 'Class core: Iron Cannon dash and jump combos', 'CH1d review'),
 'CH1g2': ('CH1g', 'Class core: Shooting Guardian X-front, dash-jump and Head Shot', 'CH1g review'),
 'CH3a2': ('CH3a', 'Skills: Iron Paladin (110) CFG / CHG-named skills', 'CH3a review'),
}


def phase_block(p):
    out = []
    fn = [r for r in cm if r[6] == p]
    if fn:
        c = collections.Counter(r[4] for r in fn)
        out.append('- **Functions: %d** (`awk -F\'\\t\' \'$7=="%s"\' june2017_re/char_chung/ch0_class_map.tsv`; %s).' %
                   (len(fn), p, ', '.join('%s %d' % kv for kv in sorted(c.items()))))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[4] == 'inside' else '(%s %s)' % (r[4], r[5])) for r in fn) + '.')
    st = [r for r in states if len(r) > 7 and r[7] == p]
    if st:
        out.append('- **States: %d** (`awk -F\'\\t\' \'$8=="%s"\' june2017_re/char_chung/ch0_state_skill.tsv`): %s.' %
                   (len(st), p, ', '.join('`%s`' % r[0] for r in st)))
    sk = [r for r in skills if len(r) > 5 and r[5] == p]
    if sk:
        out.append('- **Skills: %d** (part 2 of `ch0_state_skill.tsv`, col 6): %s.' % (len(sk), ', '.join('%s %s' % (r[0], r[1]) for r in sk)))
    rows = [r for r in ck if len(r) > 4 and re.search(r'(^|\s)%s(\s|$)' % re.escape(p), r[4])]
    if rows:
        out.append('- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): ' + '; '.join('%s %s %s' % (r[0], r[1], r[2][:70]) for r in rows) + '.')
    for hp, (before, cs) in HELPER.items():
        if hp == p:
            out.append('- **Callees to map** (`ch0_unmapped_identified.tsv`): ' + '; '.join(
                '`%s` %s (%s; callers %s)' % (c, ui[c][2][:80], ui[c][3], ui[c][8][:40]) for c in cs) + '.')
        elif before == p:
            out.append('- **Depends on CH0\'s Opus helper phase %s** (%s).' % (hp, ', '.join('`%s`' % c for c in cs)))
    if p in OWN:
        out.append('- **Callees this Opus phase maps itself:** ' + '; '.join('`%s` %s' % (c, ui[c][2][:80]) for c in OWN[p]) + '.')
    for n in NOTES.get(p, []):
        out.append('- ' + n)
    return out


def helper_kickoff(hp, before, cs):
    return '\n'.join([
        '# June character Chung (Stage 7) - Phase %s - Helpers: engine callees the Sonnet phase %s needs (Opus)' % (hp, before), '',
        '| | |', '|---|---|',
        '| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung %s` |' % hp,
        '| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |',
        '| Depends on | the review of the phase before %s; one at a time with the other `X2GUChung.*` / `X2GUUser.*` phases |' % before,
        '| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |',
        '| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |',
        '| Review | `kickoffs/char-chung/%s-review.md` (Opus 5.5 high; the phase writes it when done) |' % hp, '',
        '## Read first (and nothing else from the plan doc unless you need it)', '', '```sh',
        'python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"',
        "awk -F'\\t' '$1 ~ /^(%s)/' june2017_re/char_chung/ch0_unmapped_identified.tsv june2017_re/char_chung/ch0_unmapped_callees.tsv" % '|'.join(cs),
        '```', '', '## Task', '',
        'CH0 found callees outside Chung\'s class range that no doc maps and that the Sonnet phase `%s` would stop on ("this needs Opus"). '
        'Map each in Ghidra, name it, give it a March home (an existing March function when June\'s namesake does the same; else a new one under the flag, '
        'by the owning class\'s pattern, no `June` in the name), and port its whole body (June\'s numbers and quirks verbatim, a `JUNE_BUGS.md` row each). '
        'A funcmap row per function (`Char Chung %s`) so `%s` calls it by its March name. Check first whether Aisha\'s, Eve\'s or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).' % (before, hp, before),
        ''] + ['- **`%s`** %s; CH0 grade: %s; callers %s; evidence: %s' % (c, ui[c][2], ui[c][3], ui[c][8], ui[c][4][:160]) for c in cs] + [
        '',
        'You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. '
        'Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `%s`\'s kickoff names the March symbols; '
        '`junediverge.py --check` and `enumscan.py --check` PASS.' % before, ''])


def main():
    dry = '--dry' in sys.argv
    # new split kickoffs: a copy of the base kickoff with the phase id and scope swapped
    for p, (base, scope, dep) in NEW_KICKOFFS.items():
        dst = os.path.join(K, p + '.md')
        if os.path.exists(dst):
            continue
        s = open(os.path.join(K, base + '.md'), encoding='utf-8').read()
        s = s.replace('Phase %s - ' % base, 'Phase %s - ' % p, 1).replace('char-chung %s`' % base, 'char-chung %s`' % p)
        s = s.replace('"%s"' % base, '"%s"' % p).replace('/%s/' % base, '/%s/' % p).replace('%s-review.md' % base, '%s-review.md' % p)
        s = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + scope, s, count=1, flags=re.M)
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + dep + '; one at a time with the other `X2GUChung.*` phases', s, count=1)
        s = s.replace('the class map gives phase `%s`' % base, 'the class map gives phase `%s`' % p)
        if not dry:
            open(dst, 'w', encoding='utf-8', newline='\n').write(s)
        print('new', p)
    for hp, (before, cs) in HELPER.items():
        dst = os.path.join(K, hp + '.md')
        if not os.path.exists(dst):
            if not dry:
                open(dst, 'w', encoding='utf-8', newline='\n').write(helper_kickoff(hp, before, cs))
            print('new', hp)
    for f in sorted(os.listdir(K)):
        if not f.endswith('.md') or f.endswith('-review.md'):
            continue
        p = f[:-3]
        path = os.path.join(K, f)
        s = open(path, encoding='utf-8').read()
        s2 = s.replace("'$6==\"", "'$7==\"").replace('      # CH0 amends the column number if it differs', '')
        if PLACE in s2:
            body = phase_block(p) or ['- Nothing in the CH0 maps carries this phase\'s id; the plan section is the scope.']
            s2 = s2.replace('## Amended by CH0\n\n' + PLACE, '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(body))
        elif p in NEW_KICKOFFS and '## Amended by CH0 (' not in s2:
            s2 = s2.rstrip('\n') + '\n\n## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(phase_block(p)) + '\n'
        if s2 != s:
            if not dry:
                open(path, 'w', encoding='utf-8', newline='\n').write(s2)
            print('amended', p)


if __name__ == '__main__':
    main()
