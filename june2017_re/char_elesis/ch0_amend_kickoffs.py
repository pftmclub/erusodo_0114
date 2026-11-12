"""char-elesis CH0: append an "Amended by CH0 (2026-11-12)" section to each kickoff CH0b-CH6 (idempotent: a kickoff that has one is left alone).
    python june2017_re/char_elesis/ch0_amend_kickoffs.py [--dry]
Counts per phase: class-map functions (ch0_class_map.tsv col 7; col 9 y = carved, n = listing-only; col 5 inside / new), script states
(ch0_state_skill.tsv part 1 col 8), skill ids (part 2 col 6, the split halves by the CH0 split rule), checklist rows (ch0_checklist_rows.tsv col 5),
map rows (ch0_wos / ch0_summon / ch0_blood last column, ch0_classn col 7), plus the hand notes in NOTES (CH0 Opus verdicts).
"""
import os, re, sys, collections
HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-elesis')
D = 'june2017_re/char_elesis/'
HEAD = '## Amended by CH0 (2026-11-12)'
PHASES = ('CH0b CH1a CH1b CH1c CH1d CH1d2 CH1e CH1f CH1f2 CH1g CH1g2 CH1h CH1i CH1j CH1k CH1m CH1n CH1o CH1p CH1q CH1 CH5 CHNa CHNb '
          'CH2a CH2b CH2b2 CH2c CH2d CH3a CH3a2 CH3b0 CH3b CH3b2 CH3c CH3d CH4a CH4b CH4c CH6').split()
SPLIT_SKILL = {28202: 'CH2b2', 28203: 'CH2b2', 28204: 'CH2b2', 28205: 'CH3a2', 28206: 'CH3a2', 28222: 'CH3a2',
               28210: 'CH3b2', 28211: 'CH3b2', 28223: 'CH3b2'}
TOK = re.compile(r'CH(?:\d[0-9a-z]*|N[ab])')


def tsv(name):
    p = os.path.join(HERE, name)
    return [l.rstrip('\r\n').split('\t') for l in open(p, encoding='utf-8') if l.strip() and not l.startswith('#')] if os.path.exists(p) else []


cm = [r for r in tsv('ch0_class_map.tsv') if re.fullmatch(r'[0-9a-f]{8}', r[0])]
st_rows, sk_rows, part = [], [], 1
for l in open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8'):
    if l.startswith('# part 2'):
        part = 2
    if l.startswith('#') or not l.strip():
        continue
    r = l.rstrip('\r\n').split('\t')
    if part == 1 and r[0] != 'state' and len(r) > 7:
        st_rows.append(r)
    if part == 2 and r[0].isdigit():
        sk_rows.append(r)
cl = [r for r in tsv('ch0_checklist_rows.tsv') if r[0] != 'section']
MAPS = [('ch0_wos.tsv', -1), ('ch0_summon.tsv', -1), ('ch0_blood.tsv', -1), ('ch0_classn.tsv', 6)]

NOTES = {
 'CH0b': [
  'Stage: `python tools/kom.py stage-june --jobs 8 ' + D + 'ch0b_stage_list.tsv` (6,452 rows, 28 koms). CH0 dry run: `STAGE-JUNE DRY RUN: 28 kom(s), 6452 staged (6314 new, 138 replaced, 0 identical), 0 refused`. New kom **data206** (57 June-only motions: 54 emotions, `Motion_EL_DK.x`, `MOTION_EL_CA.x`, `Motion_Standard_El.X`); `Motion_EL_Common` / `_Field` / `_Emotion` / `_PK` (no name dropped) replace March\'s in data033.',
  '**Held for CH1 final** (`' + D + 'ch1_held_motions.tsv`, kept out of the list): `MOTION_EL_GM.x` (drops `SGM_COMBO_XXDOWNX_*`, `SI_A_SGM_CRITICAL_DIVE*`) and `Motion_EL_BH.x` (drops `Combo_XZ*`, `SI_A_SBH_FIREWORKS`, `SI_SA_SBH_SCALDIC_SWORD`): March\'s script plays all ten; June renamed them into `Motion_EL_PK.x` / `Motion_EL_SK.x`.',
  'Adopt (bytecode, `/x2-adopt-june`): data034 `DLG_1ST_Class_Change_Guide_Elesis.lua`, `DLG_2ND_Class_Change_Guide_Elesis.lua` (new), `Class_Change_Guide_Elesis.lua` (replace); data217 `DLG_Class_Change_Elesis_SK` / `_PK` / `_GM` / `_BH` (replace March\'s in data034, as Ara\'s CH0b) and `_DK` / `_CA` (new). **Not** `DLG_Unit_Create_ClassChange_Button_Elesis.lua` (CH5). `luabind.py check` on each.',
  'Skill icons: `python tools/skillicons.py check 106 900 28000-28064 28200-28236 28400-28420 28600-28605` = 75 misses on 12 files today (`DLG_SKILL_ELESIS_Active_03-05`, `DLG_Elesis_SKILL_ScreenShot_02-04`, .tga + .tet), all in the list: must PASS after the pack.',
  'The four summon NPC scripts (data037) are already June\'s. Four closure names exist in no June kom (`HQ_CutIn_Ara_normal.tga`, `HQ_CutIn_Ara_Rage.tga`, `ELISIS_RIDING_RAMIEL_SWORD_TRACE.DDS`, `SA_EL_MEGA_SLASH_TRACE`): June\'s own gaps, nothing to stage. Save: every March class has a unit; 35 / 125 wait for CHNa. Pre-log `ErrorLog.txt` first; the KEM 433 frame warning on her part meshes should clear with June\'s part meshes (check).'],
 'CH1a': ['CH0 placed here by hand: `ESI_CAN_NOT_ATTACK_JUMP_DOWN` (id 186), `ESI_SWORD_WAIT` (id 187) and `PlayAnimationSwordWaitStart` `32b6e0c0` (June: fatal threshold 0.15 when `+0x95d0` > 0; `+0x95d0` unnamed, 0x29c reads it too; `PLAY_TYPE` / `TRANSITION` keys). The QUICK group is 4 `ESI_QUICK_STANDUP_ATTACK_*` states on March\'s StandUpAttack callbacks; `ESI_JUMP_ATTACK_X` / `_Z` are here (JUMP* rule).'],
 'CH1d2': ['The dash-jump XZ fireball `32b770e0` (`SERV_IRUHADEV_JUNE_EL_DASHJUMP_XZ_FIREBALL`) is an `SPK` state: CH1f writes it.'],
 'CH1f': ['CH0 split by job line: CH1f = Pyro Knight (`SPK`), CH1f2 = Blazing Heart (`SBH`). `32b770e0` (`ESI_DASH_JUMP_COMBO_SPK_XZ_FrameMove`) is here: June\'s whole function under `_LIVE`, the `SERV_IRUHADEV_JUNE_EL_DASHJUMP_XZ_FIREBALL` block stays in the `#else` (Z records it superseded).'],
 'CH1f2': ['CH0 split off CH1f (Blazing Heart `SBH` line states).'],
 'CH1g': ['CH0 split by job line: CH1g = Dark Knight (`SDK`), CH1g2 = Crimson Avenger (`SCA`). Every state name is a new `EL_STATE_ID` member with June\'s id (`' + D + 'ch0_state_enum_names.tsv`: June\'s USI_END is 152, March 100; 134 registered names missing in C++, 37 of the 79 shared ones off by 1-2 after the rebase: register by June\'s id, never by March\'s order).'],
 'CH1g2': ['CH0 split off CH1g (Crimson Avenger `SCA` line states). The review (CH1g2-review) covers CH1e-CH1g2.'],
 'CH1h': ['Overrides named by body: 0x29c `GetReduceDebuffDurationRate` `32b6fc40` (reads `+0x95d0`), 0x4ac `32b90420` returns June state 0xbb = id 187 = `ESI_SWORD_WAIT` (by name). Unnamed for this phase: 0x534 `32b90430` (base `3208e8c0` state test, then a class state-group test; name by body) and 0x558 `32b6fb80` (a gauge-data ratio, base returns 0). ReInit / InitComponent carry Way of the Sword lines (`ch0_wos.tsv`: ReInit nudges the point and ends + re-enters TOWARD outside PvP, InitComponent ends with `RefreshWayOfSwordUI`): those lines under `_WOS`. ReInit calls `32b81300` (Shadow Edge list, CH4c): declare it.'],
 'CH1i': ['0x670 `32b64290` = `IsInJobLine` (line 3 = classes 35 / 125). SetEquippedSkillLevel `32b4e790`: 22 blocks in `' + D + 'ch0_setskilllevel_blocks.tsv` (each calls vslot 0x7a8 per state id; no member write); block 21 (28009) state id 260 has no June registration name and ids 220 / 221 may share a label: resolve from the listing or say "this needs Opus".'],
 'CH1j': ['CommonFrameMove `32b88070` calls June pieces owned later: `MadnessFrameMove` `32b6ff20`, `TranscendenceBerserkerFrameMove` `32b7e9a0`, `ShadowEdgeRecordFrameMove` `32b813d0` (CH4c, under `_BLOOD`), `ApplyGrandMasterAura` `32b7e740` and `FireBlossomsProc_FrameMove` `32b734d0` (CH4a, under `_PASSIVE`): declare them `// June <va>: body in <phase>`.'],
 'CH1k': ['CH0 placed the Endurance Power states here: `ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE` (ids 205 and 207: 207 has its own Start `32b75c90` and is the one DetonationStateChange enters) and `_MEMO` (206); skills 28201 / 28218 have no switch state (they run from NoStateChangeActionFrameMove `32b818b0` -> `32b80c60`). `CheckSummonSpecialAttack` (called from `32b84a50 @32b84d19`) is already ported in the base (`X2GUUser.cpp:36995`).'],
 'CH1m': ['`' + D + 'ch0_reg.tsv` 177 registrations (`32b81be0` 111, `32b864f0` 57, wrapper 4, `3202b110` 5), `ch0c_listing_check` 177 / 177. June registers 9 names her script has no table for (`ESI_RIDING_DAMAGE_BACK`, `ESI_STANDUP_ATTACK_FRONT_NO_DOWN` / `_BACK_NO_DOWN`, `ESI_WALL_LANDING`, 5 `USI_EVENT_*`); the script defines `ESI_RIDING_DAMAGE_FRONT` twice. `ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE` is registered twice (205, 207). No berserk table; hyper tables `ESI_HYPER_MODE`, `ESI_EGM_` / `ESI_EBH_` / `ESI_ECA_HYPER_MODE`.'],
 'CH1n': ['`' + D + 'ch0_skill_switches.tsv` (287 rows): (a) 31 entries from 28031 (28031 / 28060 Juggernaut Buster, 28032 / 28061 Blazing Dance), (b) skill type 5..16, (c) 28000..28236 (237 entries, 55 targets, 96 ids register), (d) 28601..28605; compares 28600 and 900. `ch0_skill_listing_rows.tsv` 167 rows; 12 states get their hyper callbacks copied by `329cc2c0` (Ghidra shows none: the listing is right). No second skill switch (0x68c = the base `3208ddc0`). The four delegates CH0 first saw here (`32b52470`, `32b524b0`, `32b6fd50`, `32b6f820`) are InitPassiveSkillState\'s (CH4a).'],
 'CH1o': ['0x83c `32b732e0` (June-only user hook, base empty `31f3f070`): switch on the current state group (value / 100 - 164, 18 cases); reads skill 28401\'s level, adds `+0x9660` (halved for group 0xae) to `+0x7c0`: name and role here.'],
 'CH1p': ['Keys (`' + D + 'ch0_keys.tsv`): the four `EFFECT_SET_LIST_*` variants, `RELATIVE_SKILL_LEARN_CHECK`, `IGNORE_STATE_DEFENCE`, `IGNORE_DEFENCE_DECLINE` all have live engine readers (`X2GUUser.cpp:49881-49923`, `X2DamageManager.cpp:752` / `:763`, `:5273`): check her states read through them; no class reader to write.'],
 'CH1q': ['`' + D + 'ch0_wos.tsv` (84 rows, `proposed_phase` = last column): 28 code-bearing functions (18 class incl. 5 June-only: `GetWayOfSwordPointGainRate` 0x720, `GetVigorHpIntakeRate` 0x724, 0x72c, `StoreGageSnapshot` / `RestoreGageSnapshot`; 3 gauge data; 4 gauge UI incl. `RefreshWayOfSwordUI`; the gauge manager arm is CHNa\'s; 2 stand-in sites): **no split**. Start Vigor / Destruction pick the Dark Knight buffs on job line 3 (BFI 473 / 471, PvP 472); both Ends erase BTI 435 / 434 and play the `DARK_FORCE` end sets: register `BFI_BUFF_GALE_WAY_OF_SWORD_DARK_KNIGHT` 473, `BFI_BUFF_ANNIHILATION_WAY_OF_SWORD_DARK_KNIGHT` 471, `..._PVP_DARK_KNIGHT` 472, `BTI_BUFF_GALE_WAY_OF_SWORD_DARK_KNIGHT` 435, `BTI_BUFF_ANNIHILATION_WAY_OF_SWORD_DARK_KNIGHT` 434 in `X2BuffFactor.h` under `JUNE_ENUM_NAMES`. `GetFinalDamage_WayOfSword` multiplies the Destruction result by `+0x971c` (Dark Force SA_DAMAGE_REL). `+0x9714` / `+0x9718` / `+0x971c` default 1 / 0 / 1 (ctor, InitPassiveSkillState) and come from Dark Force 28410 (SA 147 / 49 / 15, CH4a). The emulator already answers `CAC_WSP` (comments at `X2OfflineStatTable.cpp:292-295` are stale). June does not read `WayOfSword.lua`\'s `Const_Elesis_*` (its constants equal March\'s `GALE_*`).'],
 'CH1': ['Stage the held `MOTION_EL_GM.x` / `Motion_EL_BH.x` (`ch1_held_motions.tsv`) with the script. Live data033 already has June\'s `Motion_EL.x`, `Motion_EL_SK.x`, `Motion_EL_Lobby.x`: the six March names they lost (`SI_A_EL_LEAP_CRASH_*`, `SI_A_EL_RUSHING_SWORD`, `SI_SA_SSK_SPIRAL_BLAST`, `_VICTORIOUS_SWORD`) are renamed in June and come right with June\'s script. Enum gate (`luabind.py check ELESIS_KNIGHT`, CH0): 4 `ENUM MISSING NAME` (`RNSI_A_SCA_BLOOD_STING`, `RNSI_A_SGM_UPPER_SLASH`, `RNSI_P_SBH_BLAZE_ABSOLU`, `RNSI_SA_SCA_CHASING_SWORD`): add June\'s members to data036 `Enum.lua`; 0 STATE lines, 0 unbound methods.'],
 'CH5': ['`' + D + 'ch0_unittemplet_elesis.tsv`: every string id is in `general.ess`; June adds `m_fLobbyRotationY` (8: 30, 29: 25, 30: 1, 120: 15, 121: 1) and `m_HyperCutIn` for 120 / 121; 35 / 125 rows are June-only (24 / 26 keys). **The 35 / 125 rows must land before CHNa**: a unit with no templet dereferences NULL in `GetType()` / `GetClass()` (`X2Unit.h:734-735`). The lobby: June\'s `Motion_EL_Lobby.x` spells `SPK_LobbyReadyAction` / `SSK_LobbyReadyAction` where `X2UnitViewerUI.cpp` plays `...Readyaction`: check the lobby call.'],
 'CHNa': ['`' + D + 'ch0_classn.tsv` (49 rows; current lines: compatibility arms `X2Unit.h:2085-2151`, Elesis helpers `:2731-2802` (only `GetThirdLine...` gains 35 / 125), gauge arm `X2GageManager.cpp:205-220`, portraits `X2Data.cpp:2925-2936` (indices 57 / 58), stat chart `:3876-3891` (June `3254ee40`: 35 / 125 = 2, 1, 1)). **Stat rows: nothing to write**: the flat `StatTable.lua` (e37c9211) already has 35 / 120 / 121 / 125. Dropped: `X2ComboTree.cpp:302` (June loads no combo tree), the lobby particles (no `SDK_` / `SCA_` rows). Covered by Ara CHNa: the socket bound, `GetClassLevel`, `CorrelateUnitClassTemplet`, class arithmetic (+3 optional bound sites). `TranscendCheck` refuses 120 / 121 / 125 / 35 today (CH3d). June\'s compat: 35 accepts item classes {8, 35}, 125 {8, 35, 125}.'],
 'CHNb': ['`' + D + 'ch0_classn_route.tsv`: 35569-35572 (class 8, level 15, `CJT_JOB_THIRD`; 35572 grants 35 + 109999 + 293720) and 35573-35577 (class 35, level 35; 35577 grants 125 + 110850 + 293721); `Handlers_Quest.cpp:586-625` already grants any class. `LevelUpEvent.lua`: June\'s level-15 row opens `DLG_1ST_...`, level-35 lists 29 / 30 / 35 and opens `DLG_2ND_...` (live: March\'s guide / no dialog). Open check: `GuideAcceptClassUpQuest` lists quests that start at Echo; 35569 starts at Lenpad, 35573 at Ariel: test the third-line Accept button.'],
 'CH2d': ['The 11 bare `SI_` Dark Knight tables are here (registered only through the skill switch). The bleeding self-damage helper `32b89470` (min(final damage, HP - 10% max HP), x0.2 under BTI 666) is first called here (10 callers in CH2d / CH3c).'],
 'CH2b': ['CH0 split by skill: CH2b = 28005 / 28006 / 28007 (special actives), CH2b2 = 28202 / 28203 / 28204.'],
 'CH2b2': ['CH0 split off CH2b: actives 28202 Rushing Sword, 28203 Power Break, 28204 Leap Crash (+ their upgrades).'],
 'CH3a': ['CH0 split by skill: CH3a = 28008-28011, 28039, 28040, 28062 (special actives), CH3a2 = 28205 / 28206 / 28222.'],
 'CH3a2': ['CH0 split off CH3a: actives 28205 Provoke, 28206 Drastic Charge, 28222 Upper Slash.'],
 'CH3b0': ['`' + D + 'ch0_summon.tsv`: five sites (all `322e67c0` or its forwarder, all behind IsMyUnit): 1567 x up to min(`+0x9788`, 3) in `32b79db0` (Scalet Rose success End, CH3b2) and on BTI 294 end in 0x250 (March `X2GUEL.cpp:3667`); 1635 / 1971 in `32b7a0a0` (Wall of Prominence FrameMove, CH3b; 1971 when `+0x97a4` = Blaze Absolu 28418 level > 0); 1641 in `32b8b0c0` (Blood Cutter, CH3c). Write each spawn under `_SUMMON` inside the owner phase\'s function. **NPC data**: 1635 / 1641 / 1971 lack `NPCTemplet.lua` rows (core list), `Enum.lua` names and `X2UnitManager.h` members (1971 has an ExtraNPCStat row only); scripts already June\'s. `+0x8e74` is written by the base `322ece70` (nothing in her class). 0x24c writes `+0x8e50` from 28063\'s ability 5 on BTI 940; 0x250 zeroes it.'],
 'CH3b': ['CH0 split by skill: CH3b = 28015-28018, 28041, 28042, 28063 (special actives), CH3b2 = 28210 / 28211 / 28223. Wall of Prominence `32b7a0a0` is here (its spawn written by CH3b0).'],
 'CH3b2': ['CH0 split off CH3b: actives 28210 Flame Sword (reserved-buff path, `[JUNE_SKILL] SBUF` lines), 28211 Scalet Rose (Fire Blossom spawn `32b79db0`, written by CH3b0), 28223 Flame Protection.'],
 'CH3c': ['The 10 bare `SI_` Crimson Avenger tables are here; Blood Cutter `32b8b0c0` spawns NPC 1641 (CH3b0 wrote it).'],
 'CH3d': ['`SI_HA_SCA_FATE_SMASHER` (bare `SI_`) is here. `TranscendCheck` (`Handlers_Quest.cpp`) refuses 120 / 121 / 125 today (error 1726): her allow-list rows; June\'s level-70 `LevelUpEvent.lua` row.'],
 'CH4a': ['`InitPassiveSkillState` `32b88300` pushes 7 delegates through the thunk-table adapters `32b8ecf0` / `32b8ed00` / `32b8ffd0` / `32b8ffe0` / `32b906d0` / `32b90ab0` / `32b90ac0` (`push edx; call; ret`): `32b52470`, `32b524b0` (Scalet Rose hit counter `+0x9788`), `32b6fd50`, `32b6f820`, `HealByBurningPhantomOnAttack` `32b7e3b0`, `32b872d0` (here) and `ApplyBloodRunawayOnAttack` `32b87880` (CH4c). It also writes the Dark Force members (`+0x9714` / `+0x9718` / `+0x971c` from 28410), Judgment (`+0x9748` level / `+0x974c` chance of 28411) and Blood Runaway (`+0x9760` cool time of 28414). `ApplyGrandMasterAura` `32b7e740` (BFI 909 / 910) and `FireBlossomsProc_FrameMove` `32b734d0` (March `:9347`) are here. Passive ids: `' + D + 'ch0_passive_ids.tsv` (55 March reads, 41 differ, 12 cross-line).'],
 'CH4b': ['0x428 `GetDefenceRateBySkill` `32b52260`, 0x6a0 / 0x6a4 (skill 8023 + memo 608111 exempt; constants = March), 0x2b8 `32b80f20` (June-only 28416 / 28417 part), `MemoConditionElesis` (vtable `334fa4f4`).'],
 'CH4c': ['`' + D + 'ch0_blood.tsv` (30 rows): 0x4f8 `OnAttackerDamageReact_Slot4F8` = Dark Force HP intake in Vigor (getter 0x724), 0x4fc `_Slot4FC` = Judgment bleeding heal (BFI 465, BTI 99, chance `+0x974c`), 0x734 `OnDefenderDamageReact` = Blood Revenge (BFI 464; `+0x9740` / `+0x9744`; **add the two base virtuals and their calls**: the June user DamageReact `320462a0` calls 0x730 @`320499f7` and 0x734 @`32049a0c`; March has neither, while 0x4f8 / 0x4fc are already called, `X2GUUser.cpp:11355`), Blood Runaway `32b87880`, Madness `32b6ff20`, transcendence berserker `32b7e9a0`, the Shadow Edge list (0x730 `32b816a0` discards its lookup: June no-op kept verbatim; `32b81220` / `32b81300` / `32b813d0`). No split (10 functions + the base hooks).'],
 'CH6': ['S3 baseline: `' + D + 'ch0_s3_baseline.txt` (261 rows, 254 no_state, 35 / 125 included). Reruns: `charport_scan.py`, `charport_keys.py ELESIS_KNIGHT` (CH0: 248 keys, 9 june-only, 1 neither, 4 march-only-site, all owned).'],
}


def counts(ph):
    rows = [r for r in cm if len(r) > 6 and r[6] == ph and r[1] not in ('thunk', 'dead', 'not-Elesis')]
    if not rows:
        return None
    lo = sum(1 for r in rows if len(r) > 8 and r[8] == 'n')
    ins = sum(1 for r in rows if r[4] == 'inside')
    return len(rows), lo, ins, len(rows) - ins


def skills_of(ph):
    out = []
    for r in sk_rows:
        p = SPLIT_SKILL.get(int(r[0]), r[5])
        if p == ph:
            out.append(r[0])
    return out


def section(ph):
    L = [HEAD, '']
    c = counts(ph)
    if c:
        L.append('- **Functions:** `awk -F\'\\t\' \'$7=="%s"\' %sch0_class_map.tsv` = %d (%d listing-only: col 9 `n`; %d `inside`, %d `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).' % ((ph, D) + c))
    ns = sum(1 for r in st_rows if r[7] == ph)
    if ns:
        L.append('- **Script states:** `awk -F\'\\t\' \'$8=="%s"\' %sch0_state_skill.tsv` = %d.' % (ph, D, ns))
    sk = skills_of(ph)
    if sk:
        L.append('- **Skill ids** (%d): %s.' % (len(sk), ', '.join(sk)))
    cr = [r for r in cl if len(r) > 4 and ph in TOK.findall(r[4])]
    if cr:
        L.append('- **Checklist rows** (`%sch0_checklist_rows.tsv` col 5): %s.' % (D, ', '.join(sorted({r[1] for r in cr}, key=lambda x: int(re.sub(r'\D', '', x) or 0)))))
    for name, col in MAPS:
        rr = [r for r in tsv(name)[1:] if ph in TOK.findall(r[col] if len(r) > abs(col) else '')]
        if rr:
            L.append('- **`%s` rows:** %d (filter on its `proposed_phase` column).' % (name, len(rr)))
    for n in NOTES.get(ph, []):
        L.append('- ' + n)
    if len(L) == 2:
        L.append('- Nothing changes this phase\'s scope.')
    return '\n'.join(L) + '\n'


def main():
    dry = '--dry' in sys.argv
    for ph in PHASES:
        p = os.path.join(K, ph + '.md')
        if not os.path.exists(p):
            print('MISSING kickoff', ph)
            continue
        s = open(p, 'rb').read().decode('utf-8')
        if HEAD in s:
            print('skip (amended)', ph)
            continue
        nl = '\r\n' if '\r\n' in s else '\n'
        add = section(ph).replace('\n', nl)
        if dry:
            print('=== ' + ph)
            print(add)
            continue
        if not s.endswith(nl):
            s += nl
        open(p, 'wb').write((s + nl + add).encode('utf-8'))
        print('amended', ph)


main()
