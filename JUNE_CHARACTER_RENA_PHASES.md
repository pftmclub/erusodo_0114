# June character port: Rena (Stage 7, fifth character after the pilot)

Plan key `char-rena` · branch `main` · kickoffs `kickoffs/char-rena/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] RENA <sub>`,
sub-tags `DATA`, `CORE`, `NF`, `TRAP`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_RENA_*` for Rena's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-07 by the `new-plan char-rena` session from
the Aisha plan's "Template for Stage 7" and the Raven plan (`JUNE_CHARACTER_RAVEN_PHASES.md`), the most recent worked example.

## Context

**What this plan does.** It moves Rena (unit type `UT_LIRE` 3, script `Lire_ElvenRanger.lua`, class `CX2GULire_ElvenRanger` in
`X2Lib/X2GULire_ElvenRanger.cpp` / `.h`, seven classes: Elven Ranger 3, Combat Ranger 12, Sniping Ranger 13, Trapping Ranger 24, Wind Sneaker
102, Grand Archer 103, Night Watcher 114) from her March kit to her June 2017 kit, end to end: June script, skill states for June's tree,
numerical damage, motions, class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), **natural force** (`ADD_RENA_SYSTEM`,
off in March: Open decision 1), her trap summons, UI pieces, data and her transcendence rows.

**Why Rena is fifth (stage7-plan, 2026-10-22).** Gap 12 would have put her second, but natural force needs the user's decision; it is asked at
the end of this planning session (Open decisions). The brief's premises hold: the checklist's natural-force row (l.149: "a decision for you"),
the D8 re-keys (l.330: Powerful Bowstring 3405 -> 23405, Sharp Kick 3400 -> 23400), the Rena branch of `ApplyHyperModeBuff` (l.959).

**Her size.** June's class spans `0x32c6a8d0`-`0x32cb6c6d` (Raven's last function is `32c6a620`; Arme's first override is `32cb6e00`, funcmap
row `32cb6e00-32cf2da0 (Char CH0)` "Rena ends 32cb6c6e"): **467** carved functions plus **24 uncarved vtable overrides** (list in
`plan_june_side.md` §1: SpecialAttackEventProcess `32c6a970`, InitPassiveSkillState `32ca78b0`, DamageReact `32c6da50`, hyper effect start
`32c9b3d0`, Common* `0x7c0` / `0x7d8`, ...), against Raven's 440 + 28. June's script has **346** state tables, all `LESI_` (Raven 346 under five
prefixes). Expect about Raven's size (98 commits). The phase table names 16 CH1 sub-phases up front (Raven planned 17 and ran 17).

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28), as Chung, Elsword and Raven.** Most June bodies have a March function
of the same name: **the body goes inside that function under `_LIVE`, March's in `#else`**, and each CH1 sub-phase proves it with a one-file
probe compile that defines `_LIVE`. A June function March's class lacks is written under `_CORE` with a plain name in the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** Her third jobs (`UC_LIRE_SWIFT_UPPER` 156, `_DAYBREAKER` 157, `_TWILIGHT` 158) are
out. Transcendence is live (Stage 6), so her transcendence rows are in CH3d and CH6.

**Order.** char-raven closed 2026-11-07: CH0 runs now, CH0b after CH0, every code phase after CH0. CH1q (natural force) follows Open decision 1
(answered 2026-11-07: (a), March's dormant code as the base). Lu/Ciel stays parked until Stage 7 closes (user, 2026-11-03).

**The state she starts from.**
- **Skill system (Stage 3).** `s3_reform_ids_no_state.tsv` has 267 Rena rows (3: 15; 12 / 13 / 24: 30 each; 102 / 103 / 114: 54 each), 130
  distinct ids (106, 900, 23000-23060, 23200-23242, 23400-23417, 23600-23605, no gaps); **260 have no class state** (only 900 has one).
  **Every row has a June `SKILL_ID` name** (7ae: no unnamed-id task).
- **Effect engine (June's).** `DamageEffect.lua` / `EffectSet.lua` ship merged; **5** Rena rows in `c11a_march_only_templets.tsv`
  (`DamageEffect_Cryotron_Bolt_GUIDED_ARROW`, `DamageEffect_LGA_Arc_Shot` with a March consumer in the class; `Effectset_Rena_DiveKickBombingA` /
  `B` / `B_Memo` read by March's script; 7u); **8 alpha-tag sites** (7c, `c10a_alpha_sites.tsv` l.206-213: 6 in InitComponent `32c895b0`,
  `32c893e5`, `32c89535`).
- **Motions.** June 71 (data201 + data012), March 18 (17 `Motion_Lire*` in data033 + `Motion_Rena_Buff_NPC.X`); 7 the same size, the 11 class
  files much larger in June (`Motion_Lire.x` 4.4 -> 6.6 MB, `_Common` 1.3 -> 6.5 MB). Whether June drops names March's script plays decides if
  they deploy in CH0b or with CH1 final (7ad): CH0 answers. March's game dir has no `data201.kom`: CH0 names the target kom.
- **Natural force (dormant March code, `ADD_RENA_SYSTEM`).** Undefined under `US_SERVICE` (`cl /EP` probe, `plan_add_rena_system.md`): 187
  `#ifdef` / `#ifndef` blocks in 23 files (class 111 + 9 in `.h`; `X2DamageManager.*`, `X2DamageEffect.cpp`, `X2EffectSet.*`, `X2GUNPC.*`,
  `X2GUUser.*`, `X2Gage{Data,Manager,UI}.*`, `X2Game.cpp`, `X2Define.h`, `CommonPacket.h`). The class tail `:23576-24004` (NF functions,
  `CreateGageData`, `AttackResultByType`, `Init`, `IsHyperActiveSkill`) is all inside it; `CX2RenaGageData` / `CX2RenaMyGageUI`;
  `CommonPacket.h` adds `CAC_NATURAL_FORCE` and `KGamePlayStatus::GetNaturalForce()` (no layout change; a second `#define` in `ServerDefine.h` +
  `SECOND_HOMES` if switched on); the emulator returns `CAC_NONE` for Lire (`X2OfflineStatTable.cpp:289-313`) and has no NF save column. June's
  pieces seen so far: `ApplyHyperModeBuff` Rena branch `32cb50c0` / `32cb6290( 5 )` (l.959), `CX2GUNPC::AttackResultByType` `32773ee0` for
  non-user attackers (backlog l.1413), the first-attack / relate-skill copies June has unconditionally (l.149, C6a2 `DD+0x5a4`), RTTI
  `FriendshipNatureData@CX2GULire_ElvenRanger` `3373d6fc`. **Switching the flag on flips June-port arms already in the tree**: `X2DamageManager.h`
  `:1386` / `:1663` and `.cpp:314` / `:2202` / `:3038` / `:3506` (`m_iNaturalForceType`, June `+0x56c`), `X2GUUser.h:1410`, `X2GUUser.cpp:10893`,
  `X2DamageEffect.cpp:5217` (drops `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`'s child sequence), `X2GUNPC.cpp:11404`; it also compiles a studio "NF:"
  debug overlay (`X2GUUser.cpp:29719`) and a dev fill key (`X2Game.cpp:5855`). Phase CH1q, as Eve's Nasod Core.
- **Trap summons (live March code).** June calls `CreateNPCReq` `322e67c0` from 4 sites in her range, all uncarved (`32c95882`, `32c99a1a`,
  `32c9e42e`, `32ca11f2`); March has 21 `CreateNPCReq` sites (`NUI_ENTANGLE_TRAP_RENA`, `NUI_EVOKE_TRAP_LIRE` / `_MEMO`,
  `NUI_RENA_ANGER_OF_ELF` / `_DELAY`; Entangle `:15884`, Trap Blade `:17797`, Guide Arrow `:21554`, Anger of Elf `:22713`). June's script has no
  NPC key; the June-only key `CREATE_TRAP_BLADE` x2 has a reader `32ca81b0`. Eve and Chung each needed an Opus sub-phase for summon spawn
  rules: **CH2d0** (below), CH0 settles which skill owns each site.
- **Hooks waiting on the class port.** Checklist: about 34 Rena rows (`june2017_re/char_rena/plan_checklist_rows.md`). Rena extras: D8 re-keys
  (l.330-331: 3405 -> 23405, 3400 -> 23400; Powerful Bowstring's ignore-defence write `32cada25`, funcmap l.925); her June-only
  `SetDamageExceptionProcess` `32cad510` (funcmap l.202 / l.1289); §7 l.286 her own `SetSkillLevelStateData` (`:22990-23003`); delegate
  registrations (l.912-925); the hyper / transcendence hooks (l.959, l.969); the animation-rate row (l.1045); `32cb06d0` (l.444). June-only state
  keys `BOW_SKILL_STATE` x26 and `IMMUNE_TO_SUPER_ARMOR_DOWN` x10 (readers `32ca81b0` / `31fdaaa0`) get owners in CH0.

**Out of scope (owned elsewhere).** Third jobs; new characters (Stage 8); UI that is not Rena's own (Stage 9); creation-screen buttons (Stage 9);
tutorials and training dungeons (june-dungeons); NPC / boss copies of Rena (whichever plan adopted June's NPC Lua); emotion motions' behaviour
(Stage 10; CH0b stages the files); `FORCE_DYNAMIC_CAMERA_INDEX` x15 / `RETAIN_CAMERA_STATE` (Stage 9); `IMMUNITY_EDT_LIST_AT_THIS_STATE` (backlog
Stage 2); PvP-only dialogs and paths (no PvP offline).

## Rules

The Raven plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules"`), with Rena's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim
  with a `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in her skill-use check.
- **`ReservedSkillStart` (user, 2026-11-05).** `CX2GULire_ElvenRanger::ReservedSkillStart` = `SpecialAttackCooldown( skillID, m_fReservedSkillMp )`
  then `SkillMemoMpRefund( ... )`, and the base hook chain in `X2GUUser.h` / `.cpp` widened with Rena's `_LIVE` flag (copy Raven's). CH1k writes
  it; CH1 final's play-test casts each skill twice (cool time shows, MP drops).
- **June's ids in passive code.** Every `SI_*` id March's `InitPassiveSkillState` / skill code reads is checked against June's `RNSI_*` id,
  the D8 pair 3405 / 3400 included. CH0 lists them; CH4a fixes what CH1 did not.
- **The checklist is the work list.** Every Rena row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase that
  ports a row appends `(Rena: CHn)` to it in place.
- **One deploy unit.** June's `Lire_ElvenRanger.lua`, her June motions (if CH0 finds a name June drops, the motion koms are held for CH1) and the
  C++ that reads them go live together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define` in `Always.h` until then). Between
  phases Rena stays playable on her March kit.
- **Flags:** `_CORE` (June functions March lacks, compiled, unreferenced), `_LIVE` (June bodies inside March's functions, the registration, the
  switch), `_NF` (natural force; defined only with `_LIVE`, and per Open decision 1 it defines `ADD_RENA_SYSTEM`), `_TRAP` (trap summon spawn
  rules, CH2d0), `_SKILL`, `_TRANSCEND`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a temporary `_DEBUG` per
  sub-phase, retired in its review. New namespaces are `_CONST_RENA_<phase>_` (March already has `_CONST_RENA_` at `.cpp:6`).
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name. `python tools/junediverge.py --list`
  before transcribing; `--check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h <other files>` = `RESULT: PASS` at every finish.
- **Shared `CX2GUUser` helpers, never copied into the class:** `GetSkillLevel` / `GetLearnedSkillAbilityValue`
  (`SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); the seven skill-use helpers and their fields (`SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`);
  Aisha's, Eve's, Chung's, Elsword's and Raven's `CX2GUUser` bases: a phase that needs one under its own flag widens the `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp` stays 0.
  March's C++ enum `LIRE_ELVENRANGER_STATE_ID` (`.h:6-191`, 107 `LESI_` members) takes June's missing names with June's ids (CH0 lists them).
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces only Rena's 7 rows. Her class script is adopted whole
  (`adopt-june`, bytecode unless a phase must edit it) and passes `python tools/luabind.py check Lire_ElvenRanger`; her trap NPC scripts pass
  `luabind.py check` in CH2d0.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack yourself, ask before every copy.
- **CP949.** `X2GULire_ElvenRanger.cpp` (mixed endings: 23,856 LF, 147 CRLF), `.h` (1,864 LF, 80 CRLF), `X2GUUser.*`, `X2Gage*.*`,
  `X2DamageManager.*`, `X2DamageEffect.cpp`, `X2EffectSet.*`, `X2GUNPC.*`, `X2Game.cpp`, `CommonPacket.h` are CP949: `bytepatch.py` only;
  `cl /P` decides every `#ifdef` branch (`ADD_RENA_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, `SERV_TRAPPING_RANGER_TEST`, `RENA_SECOND_CLASS_CHANGE`,
  `UPGRADE_SKILL_SYSTEM_2013`, `NEW_SKILL_2010_11`, `BALANCE_PATCH_20120329`, the dead `RENA_SIEGE_MODE_ANGLE_TEST`).
- **Uncarved overrides.** Ghidra stays read-only: the 24 uncarved functions are read from the raw listing (`charport_adis.py rena <va>`; CH0
  adds `rena` to its `CLASS_RANGE`), never by creating functions.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** Natural force has no save column today: CH1q decides with the map whether June keeps it across a relog (no schema change unless
  June's packet carries it; a phase that needs one snapshots `db_backup/` first). Play-test saves never slot a skill in slot 4 / 9 before
  transcendence, and learn only the ids of the bank being tested.
- **Offline reach (Raven lesson 4).** Each phase's hand-off names only checks the offline client can reach; a June path that only a server
  or PvP drives is a stand-in or a routed item, decided in the phase, not found in CH6.
- **One code phase at a time in `X2GULire_ElvenRanger.*`** (CH1-CH4). CH5 may run beside CH2-CH4.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "<heading>"` (`--no-sub` to skip Done notes) |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Rena's rows with line numbers: `june2017_re/char_rena/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_rena/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (CH0 adds `rena`), `charport_regsim.py`, `s3_no_state.py`; Raven's builders `june2017_re/char_raven/ch0_*.py` (copy into `char_rena/` with the range changed); `char_arme/ch0c_listing_check.py`; `char_eve/ch0_build_assets.py` (CH0 adds `rena`) |
| June Lua (reading) | `june_src_fixed/data036_kom/Lire_ElvenRanger.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`; `DLG_Transcendental_Change_Guide_Lena.src.lua` |
| Ghidra | `ghidra` skill; Rena's range `0x32c6a8d0-0x32cb6c6d`, vtable `335185bc` (544 slots, 62 overrides, all in range, 24 uncarved), RTTI `3373d770`, ctor `32ca2b60` (vtable write `32ca2bc2`), dtor `32ca42f0`, `MemoConditionRena` RTTI `3373d738` (vtable: CH0); read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow; revert the regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` after), `python charport_keys.py Lire_ElvenRanger` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Lire_ElvenRanger.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR RENA"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-07)

Kept in `june2017_re/char_rena/` (digests by Sonnet subagents, spot-checked: the range end against funcmap's Arme row "Rena ends 32cb6c6e",
the D8 and `ApplyHyperModeBuff` checklist rows; CH0 re-verifies anything it builds on): `plan_charport_scan_20261107.txt`,
`plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`, `plan_june_side.md`, `plan_numerical_rows.tsv` (141 rows), `plan_checklist_rows.md`,
`plan_add_rena_system.md` (the natural-force census).

Scans re-run 2026-11-07: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 652, june-ignores 305, handed-off 9; regenerated
`census.tsv` / matrix reverted); `charport_scan.py` (scratch `C:/tmp/cps_rena`, output identical to Raven's 2026-11-05 run but for her line):
`Lire_ElvenRanger {'no NUMERICAL_DATA_INDEX': 65, 'no DAMAGE table': 12}` (the brief's gap 12 holds), no Lire ENUM or KEYS line;
`s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess`: `rows=267 no_state=260 has_state=7`.

| Measure | Rena | (Raven) |
|---|---|---|
| June class range | `32c6a8d0`-`32cb6c6d`, 467 carved + 24 uncarved overrides; 62 vtable overrides, all in range; 0x9c / 0xa0 / 0xa4 not overridden | 440 + 28, 57 |
| June skill-tree rows | 267 over 7 classes; 130 distinct ids; 0 unnamed | 254 / 126 / 0 |
| Rows with no class state | **260** | 247 |
| Rows by June skill type | 6: 91, 14: 49, 16: 49, 15: 41, 8: 21, 12: 7, 13 (hyper): 6, 1: 3 | |
| June script | `Lire_ElvenRanger.src.lua` 17,385 lines, data only: 346 `LESI_` states + 6 `INIT_*` (`INIT_COMMON_RANDOM_STATE` keyed by class name; `MAX_HYPER_MODE_TIME` 30, `HYPER_MODE_COUNT` 1); groups SA 43, RIDING 27, A 27, DAMAGE 21, RNW 21, COMBO 19, DASH 16, RGA 14, LWS 12, LGA 9, SNIPING 9, RCR 8, SI 8, HA 8, JUMP 7, COMBAT 7, SI1-3 20, STANDUP 6, RWS 6, RTR 6, LCR 5, LSR 5, SIEGE 5, PEPPER 5, P 5, then small groups | 346 |
| Script keys | `DAMAGE_DATA` 68 (+6 `_NEXT`, 2 `_LAST`, 1 `_THIRD`), `AT_COMMAND` 52, `APPLIED_SKILL_ID` 17, `TIME_STOP_IMMUNE_STATE` 18, `SHOW_HYPER_CUT_IN_TIME` 3, `SHOW_SKILL_CUT_IN_TIME` 14; class keys `BOW_SKILL_STATE` x26, `CREATE_TRAP_BLADE` x2, `IMMUNE_TO_SUPER_ARMOR_DOWN` x10 | 88 / 46 / 29 |
| Key census (`charport_keys.py Lire_ElvenRanger`) | 256 keys: both 241; june-only **8** (the 3 class keys above, `BBT_` / `BFT_` x2 no reader, the camera pair (Stage 9), `IMMUNITY_EDT` (Stage 2)); neither 3 (`ATTACK_ACTION_TYPE`, `HEAD_IK`, `JUMP_CANCEL_AFTER`); march-only-site 4 (`AFTER_IMAGE`, `READY_SOUND`, `UNIT_SLASH_TRACE0` / `1`) | 229 / 10 / 6 / 6 |
| Numerical blocks | 141 `c1a` rows keyed to 57 ids, all `ok` | 113 / 55 |
| Motions | June 71 / March 18; 11 class files differ in size; names dropped: CH0 | 73 / 20 |
| Alpha-tag sites (7c) | **8** (6 in InitComponent) | 0 |
| Summons | **traps**: 4 June `CreateNPCReq` sites (uncarved), 21 March | none |
| Natural force | dormant (`ADD_RENA_SYSTEM` off): 187 blocks in 23 files | live March code (rage) |
| March class | `.cpp` 24,004 lines, `.h` 1,945 (`LIRE_ELVENRANGER_STATE_ID` 107 `LESI_`); cut-in table `:50-82`, ctor `:92`, dtor `:292`, ReInit `:364`, DamageReact `:409`, InitComponent `:785`, InitStateCommon `:924-1662`, InitState `:1665` (cut-in switch `:1672-1715`), InitPassiveSkillState `:2711-3084`, `SetEquippedSkillFuncBySkillSlotIndex` `:3086-4475` (74 `case SI_*`), SetEquippedSkillLevel `:4477`, CommonFrameMove `:5087-5200`, SpecialAttackEventProcess `:5236-5518`, HyperModeFrameMove `:7488`, `SetSkillLevelStateData` `:22990-23003`, NF tail `:23576-24004` | 23,343 |
| June class | InitState `32cadc90` (0x7a4), `SetEquippedSkillFuncBySkillSlotIndex` `32cb06d0` (0x7b4), InitComponent `32c895b0`, InitPassiveSkillState `32ca78b0` (0x690, uncarved), `SetDamageExceptionProcess` `32cad510` (0x2b8), SpecialAttackEventProcess `32c6a970` (0x7f4, uncarved; parts `32cac200`, `32ca50b0`, `32c89a20`), hyper effect start `32c9b3d0` (0xc4, uncarved), DamageReact `32c6da50` (uncarved), ReInit `32ca4a40`, InitEffect `32c6a8d0` | |
| Stat rows | `StatTable.lua` 1-99 for all 7 classes, no duplicate, no gap | same |
| Save | units 12 `reyaa` (103 lv99), 35 `TrapRangerT` (24 lv70); transcendence 0; **no unit of 3, 12, 13, 102, 114** | |
| Cut-ins | 4 sets: `HQ_CutIn_Lena_1/2/3.dds` for 3 / 12 / 13 / 24, `_Rena_Wind_Sneaker_1`, `_Grand_Archer_1`, `_Night_Watcher_1`; June's 33 `HQ_CutIn_Rena_*.gfx` and 3 `HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx` already in March's kom_contents; June's `UnitTemplet` adds `m_fLobbyRotationY` (30 / 30 / 25 / 1 / 30 / 20 / 10 for 3 / 12 / 13 / 24 / 102 / 103 / 114), `m_HyperCutIn` for 102 / 103 / 114, names `STR_ID_4783` / `_4784` / `_17429` for 102 / 103 / 114 (March: `STR_ID_616` on all 7) | |
| Transcendence | `DLG_Transcendental_Change_Guide_Lena.lua` (March ships a same-name file, md5 differs); `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` none in March; March lacks `DLG_1ST` / `2ND_Class_Change_Guide_Lena.lua` | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Rena map: class map (uncarved overrides included), state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, the natural-force map, the trap map, the motion question, the `data201` target | Opus high | - |
| CH0b | Every June Rena asset, whole, first; the walk units the save lacks | Sonnet high, user play-test | CH0 |
| CH1a-CH1k | Class core transcription by state group and vslot band (11 sub-phases, below) | Sonnet high, Opus review each | CH0; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | CH1a-CH1k reviews |
| CH1q | Natural force, flag `_NF` (Open decision 1) | Opus high, Opus review | CH1p review, decision 1 |
| CH1 | Final: define `_LIVE`, adopt the script, copy any held motion koms, play-test | Opus high, Opus review, user play-test | CH1q review, CH0b |
| CH2a-CH2d | Skills: Rena + first jobs (3, 12, 13, 24) | Sonnet high, Opus review, user play-test | CH1 review |
| CH2d0 | Trap summons: spawn rules, the 4 June `CreateNPCReq` sites, trap NPC scripts' binder gate, flag `_TRAP` | Opus high, Opus review | CH2c review |
| CH3a-CH3d | Skills: second jobs (102, 103, 114), hyper actives, transcendence rows | Sonnet high (CH3d Opus), Opus review, user play-test | CH2 reviews |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4b | Passives (InitPassiveSkillState, D8 re-keys, delegates); damage exception + overrides + memo | Opus high, Opus review, user play-test | CH2 reviews (CH4b: CH3c review) |
| CH5 | UI and data: Rena's `UnitTemplet` rows, cut-in table | Sonnet high, Opus review, user play-test | CH1 review; beside CH2-CH4 |
| CH-N | Not run: Rena has no templet-backed June-only class | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5 reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH1-CH4 all edit `X2GULire_ElvenRanger.*`: one at a time. CH5 runs beside CH2-CH4. If CH0's counts put a group over ~45 functions, CH0 splits
it with a digit suffix (`CH1e2`) and writes the kickoff and STATUS row; it does not merge groups. Play-test follow-ups after CH1 final get the
next letters (`CH1r`).

## Phase CH0 - Rena map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_rena/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init <InitStateCommon> 32cadc90 <base> <line
registrars> <InitPassiveSkillState if it registers>` and `skill 32cb06d0` (find InitStateCommon from vtable `335185bc`; Raven's call is in his
CH0 Done note), then run `ch0c_listing_check.py` on it: every row must match the raw listing; all `~` rows and std::string-named rows
(`USI_HYPER_MODE`, START / WIN / LOSE roll) by hand. Map every switch in `32cb06d0` (skill type, any hyper switch, the skill switch). Raven's
builders are copied into `char_rena/` with Rena's range: say which. Add `rena` to `charport_adis.py`'s `CLASS_RANGE`; confirm it lists the 24
uncarved overrides.

**Scope** (Raven's CH0 items, with Rena's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32c6a8d0-0x32cb6c6d` **plus the 24 uncarved overrides**, March pair, verdict, target
   sub-phase, and the **`inside` / `new`** column (propose the plain name for every `new` row). Funcmap rows `Char Rena CH0`. Confirm the role of
   each override by its body, not only its vslot offset (the planning took Raven's offsets).
2. **State and skill table** `ch0_state_skill.tsv`: 352 / 352 script tables, 130 / 130 skill ids; dead-data and March-only states listed;
   confirm nothing in `X2GULire_ElvenRanger.*` still stores a state id in a `UCHAR` (7l). Map June's groups to the jobs (the planning guessed
   `LCR` / `RCR` Combat, `LSR` / `RSR` Sniping, `RTR` Trapping, `LWS` / `RWS` Wind Sneaker, `LGA` / `RGA` Grand Archer, `LNW` / `RNW` Night Watcher)
   and say what `SIEGE`, `SNIPING`, `COMBAT`, `DOUBLE`, `CHARGE`, `HA`, `P`, `SI1` / `SI2` / `SI3` are and which phase each goes to; list June's ids
   for every `LESI_` name `LIRE_ELVENRANGER_STATE_ID` lacks or numbers differently.
3. **Keys** `ch0_keys.tsv`: `BOW_SKILL_STATE` x26, `CREATE_TRAP_BLADE` x2 (reader `32ca81b0`), `IMMUNE_TO_SUPER_ARMOR_DOWN` x10 (reader
   `31fdaaa0`) get an owner (class or engine; an engine reader is a `CH<n>0` or a routed item); the 3 neither keys and the 4 march-only-site keys
   (`AFTER_IMAGE`, `READY_SOUND`, `UNIT_SLASH_TRACE0` / `1`): does June's script drop a March reader's input or does June read it elsewhere;
   `MAX_HYPER_MODE_TIME` / `HYPER_MODE_COUNT` and `DAMAGE_DATA_LAST` / `_THIRD` get their readers.
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md`; its
   class-generic rows l.258 / l.262 / l.278 / l.423 matched only through a June address: confirm). D8 (l.330-331) goes to CH4a; l.959's
   `ApplyHyperModeBuff` branch to CH1q; 7c's 8 alpha sites to the phase that owns InitComponent.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Rena items, unioned; `ch0_build_assets.py` gains
   `rena`) and **effect closure** `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). **The motion kom:** June's
   data201 motions have no March `data201.kom`: name the March kom each goes to (March's class motions are in data033). **The motion question:**
   for each of the 11 differing class motions, the animation names March's script or class code plays that June's file lacks; if any, those files
   are held for CH1 final (`ch1_held_motions.tsv`), else CH0b copies them. Stage `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` (find June's
   kom), `DLG_1ST` / `2ND_Class_Change_Guide_Lena.lua`, the trap NPC meshes / scripts (`npc_mesh_census.py`), the cut-in `.gfx` CH5 needs.
6. **Unmapped helpers.** Per skill, the callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus
   `CH<n>0` sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0).
7. **Natural-force map** `ch0_nf.tsv` (start from `plan_add_rena_system.md`): every March block paired with June's function or "absent in June",
   plus June-only pieces: `FriendshipNatureData` (RTTI `3373d6fc`), `32cb50c0` / `32cb6290` and every caller (`ApplyHyperModeBuff` `32298590
   @322985ec`), `CX2GUNPC::AttackResultByType` `32773ee0`, the gauge data / UI (June's `CX2RenaGageData` / `CX2RenaMyGageUI` vtables), the
   first-attack / relate-skill copies (C6a2 funcmap `321ef5ea-321ef632`), the `KGamePlayStatus` charm field June sends and whether June persists
   NF. For each June-port arm the flag would flip (`X2DamageManager.*` `m_iNaturalForceType`, `X2DamageEffect.cpp:5217` spawn inherit,
   `X2GUUser.*:1410` / `:10893`, `X2GUNPC.cpp:11404`) say which branch is June's. Whether June keeps the studio's "NF:" overlay and dev key.
   Sized for CH1q; split if over ~45 functions (e.g. CH1q2 for the gauge UI and emulator).
8. **Trap map** `ch0_traps.tsv`: the 4 June `CreateNPCReq` sites (`32c95882`, `32c99a1a`, `32c9e42e`, `32ca11f2`) against March's 21, each with its
   skill, NPC id (June `NUI_*` name and value), spawn arguments, and the NPC's own script / mesh; `CREATE_TRAP_BLADE`; the summoned-monster test
   (`IsSkillSummonedMonster`-style) and the master link; owners: CH2d0 for spawn rules shared by her traps, the skill phases for the rest.
9. **Shared data.** Which state names the C++ enum needs (raw id + comment, as Raven); every `SI_*` id March's passive / skill code reads with its
   June `RNSI_*` id (the Rules' passive-id check, D8 included); Rena's 7 `UnitTemplet` rows June vs March (are `STR_ID_4783` / `_4784` /
   `_17429` in our string table?).
10. **S3 census baseline:** `s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess` (260 expected) so CH6 compares like with like.
11. **Amend the kickoffs** CH0b-CH5 ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Rena checklist row has a phase or n/a;
352 / 352 tables and 130 / 130 ids mapped; the 24 uncarved overrides have rows; assets union routes a / b / c and the motion kom is named;
the motion question answered; the natural-force and trap maps exist; kickoffs amended; open questions listed.

### CH0 - Done (2026-11-07)

Read-only: no flag, no build. Outputs in `june2017_re/char_rena/`; tables live in the tsvs.

**Tooling.** Raven's builders copied with Rena's range: `ch0_build_vtable.py`, `ch0_skill_listing_emu.py` (32cb06d0's frame: this = edi, normal = esi, hyper = ebp,
id `[esp+0xac]`), `ch0_march_live_states.py`, `ch0_build_state_skill.py`, `ch0_build_state_enum_names.py`, `ch0_build_keys.py`, `ch0_build_class_map.py`,
`ch0_make_hand_names.py`, `ch0_unmapped_callees.py`, `ch0_build_enum_names.py`, `ch0_build_unittemplet.py`, `ch0_passive_ids.py`, `ch0_numtables.py`,
`ch0_build_checklist_rows.py`, `ch0_amend_kickoffs.py`; new `ch0_finalize_maps.py` (NF owners), `ch0_funcmap_rows.py`. `charport_adis.py` and
`char_eve/ch0_build_assets.py` gained `rena` (Eve / Chung / Elsword / Raven asset outputs unchanged). **Fixes:** `charport_states.py` now merges the
arms of an if / else (a slot set per arm prints `a|b`, the row is marked `?`; before, the last arm won: Rena's `LESI_DASH`, Raven's LOSE roll row 269,
whose port already has the condition); `char_arme/ch0c_listing_check.py` accepts an arm row and lists it. Rena's class-map builder: pointer scan
widened to her range (it still matched Raven's `32c2`-`32c6` bytes), every vtable target is a start (uncarved InitPassiveSkillState `32ca78b0`
follows Win's `ret 4`), and a block after a mid-function `ret` is no start (instruction check + branch target). Dead rows 129 -> 57.

**Step 0.** InitStateCommon `32ca9dc0`; InitState `32cadc90` registers every line state inline (no line registrars), InitPassiveSkillState registers
none. `charport_states.py init 32ca9dc0 32cadc90 3202b110` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **202 / 202**; one arm row (`LESI_DASH`:
class 114 takes StartFuture `32c6e0c0` / FrameMoveFuture `32c6e0f0`, the others the base pair; other arm read by hand at `32caa6fe`).
`32cb06d0`: type switch, 6-case hyper switch, skill-id switch; `ch0_skill_listing_rows.tsv` 157 rows, 130 / 130 ids (19 leave at the type switch).

**Maps.**
- `ch0_class_map.tsv`: 1,494 starts (467 carved, 1,027 uncarved); state-fn 645, thunk 626, helper 103, override 62 (24 uncarved, all with rows),
  dead 57, registrar 1. Per phase: CH1a 32, CH1b 36, CH1c 3, CH1d 38, CH1d2 36, CH1e 29, CH1f 43, CH1f2 37, CH1g 46, CH1g2 29, CH1g3 27, CH1h 33,
  CH1i 8, CH1j 10, CH1k 21, CH1m 3, CH1n 2, CH1o 5, CH1q 27, CH2a 46, CH2b 19, CH2c 39, CH2d0 8, CH2d 47, CH3a 44, CH3b 39, CH3c0 14, CH3c 46,
  CH3d 22, CH4a 15, CH4b 4. Hand routings: `ch0_hand_phases.tsv` (col 4 = the March function a row goes inside).
- **`ADD_RENA_SYSTEM` carries more than natural force:** about 90 of its 111 class blocks are the 2014 combo revamp (registrations, StateChange
  targets, new states) that June has. 27 March functions exist only under the flag (7 state callbacks such as `WalkStart` / `DashStart`, Init,
  Set/UnSetSpecificValueByBuffTempletID, AttackResultByType, CreateGageData, the NF tail): their June rows go to **CH1q** inside those functions
  (flag-off, `LESI_DASH` Start pairs with `CX2GUUser::DashStart`: CH1a must not write June's body there).
- `ch0_state_skill.tsv`: **352 / 352** (322 mapped, 24 dead data, 6 `INIT_*`), **130 / 130 ids**; 13 March-only states. No `UCHAR` state id.
  `SIEGE` / `SNIPING` -> CH1f, `COMBAT` -> CH1e, `HA` -> CH3d, `P_RNW_SEED_OF_RUIN` -> CH3c, Agile Movement (4, March StandUpRolling callbacks)
  -> CH1b, `SI1-3` by skill. `ch0_state_enum_names.tsv`: 89 shared names, all with a June id different from C++ (register by June's id).
- `ch0_keys.tsv`: `BOW_SKILL_STATE` -> CH1j + CH4b (flag `+0x981d`); `CREATE_TRAP_BLADE` -> CH1j (mesh, not a summon);
  `IMMUNE_TO_SUPER_ARMOR_DOWN` -> backlog Stage 2 (reader `31fdaaa0`, bullet amended). `DAMAGE_DATA_LAST` / `_THIRD`: Rising Falcon (CH2b),
  Furious Engage (CH3c). The 3 neither keys and 4 march-only-site keys drop no March input.
- `ch0_checklist_rows.tsv`: 142 rows, 74 phase, 68 n/a. D8 -> CH4a, l.959 -> CH1q2, 7c -> CH1h, l.358 (Add's 29058) n/a.
- Item 9: `ch0_enum_names.tsv` (8 / 130 C++ names); `ch0_passive_ids.tsv` (72 live March reads; June-id column is a title match for CH4a to
  verify; D8 3400 / 3405 live, 4 sites each); `ch0_unittemplet_rena.tsv` (`STR_ID_4783` / `_4784` / `_17429` present). S3 baseline
  `ch0_s3_baseline.txt`: 267 rows, 260 `no_state`.

**Assets.** `ch0_assets.tsv` 14,046 rows; **`ch0b_stage_list.tsv` 4,762 rows**: `STAGE-JUNE DRY RUN: 31 kom(s), 4762 staged (4459 new, 303
replaced, 0 identical), 0 refused`; new kom **data201** (54 June-only motions); differing class motions go to data033. **Motion question:**
`Motion_Lire.x`, `_GA`, `_RTR`, `_SR`, `_WS` drop names March's live script plays (`ComboX3` ..., `SR_DoubleJumpAttackX`, ...): **held for CH1**
(`ch1_held_motions.tsv`); `_Common`, `_CR`, `_Emotion`, `_Field`, `_Lobby`, `_NW` go now. `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` (data215)
in the list; `DLG_1ST` / `2ND_Class_Change_Guide_Lena.lua` by `adopt-june` (dry run 2 new). Closure 7,274 rows, 85 missing (65 in the list).
Icons: 16 misses on 3 sheets, all covered by the list except 2 pieces June's own `DLG_Lena_SKILL_ScreenShot_04.TET` lacks (CH0b: a JUNE_BUGS row).

**Natural force** (`ch0_nf.tsv`, 225 rows: CH1q 99, CH1q2 27, absent in June 24, the rest combo-revamp rows of the state phases). CH1q split:
CH1q (class side, flag, flipped arms) and **CH1q2** (gauge data / UI, gauge manager, emulator, `ApplyHyperModeBuff` / NPC `AttackResultByType` /
F7 key callers). June differs: 3.0 per hit (March 6), BFI 355 stacked per point, MP refund on consume, extra point on a +0x6c50 roll. The
"NF:" overlay and fill key are absent in June. Flipping the flag would drop `SPAWN_INHERIT`'s child sequence (`X2DamageEffect.cpp:5217`): CH1q keeps it.
`FriendshipNatureData` is live March code (CH1h), not NF.

**Traps** (`ch0_traps.tsv`). June spawns three: Evoke 1020 (`32c99690`, team args swapped), Anger of Elf 1420 (`32c9e250`), Night Watcher's Gungnir
2083 (`32ca0e60`, June-only). Entangle's NPC spawn `32c95790` is dead in June (damage effects instead). No memo (1025) / delay (1421) ids in
June. Summon-info setters (5) -> CH2d0. All trap scripts and the Gungnir script pass `luabind.py check` (0 unbound); their meshes are June's.

**Item 6.** 37 unmapped callees (`ch0_unmapped_identified.tsv`); 2 need Opus, both inside Opus phases (`31c8a5c0` CH1o, `322dea10` CH3d).
New Opus phase **CH3c0**: Night Watcher engine hook (base `31fcd700` calls `32ca6cd0` for class 114 from user, damage and NPC code; 14 fn).
`funcmap.tsv`: 34 rows `Char Rena CH0`.

**Splits and kickoffs.** CH1d -> CH1d / CH1d2, CH1f -> CH1f / CH1f2, CH1g -> CH1g / CH1g2 / CH1g3, CH1q -> CH1q / CH1q2, new CH3c0; CH2d (47)
and CH3c (46) stay whole. Every kickoff CH0b-CH5 has its "Amended by CH0" section; STATUS rows added and Depends rewired.

**Plan-doc / checklist inconsistencies.** Checklist l.912 named Win `32ca7860` for the delegate registrations and l.422 named Arme's `32cb9ee0`
for Lire: both corrected in place. Plan: CH1p owned `BOW_SKILL_STATE` / `IMMUNE_TO_SUPER_ARMOR_DOWN` (moved as above); "4 trap sites" (3 live);
the 187 `ADD_RENA_SYSTEM` blocks are not all natural force; the planning digest's vtable `33518d70` is `335185bc`; checklist l.358's "Rena" is Add.
**Drift:** `IMMUNE_TO_SUPER_ARMOR_DOWN` -> backlog Stage 2 bullet. **Open for the user:** none.

## Phase CH0b - Every June Rena asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_RENA_ASSET`). Sub-tag `DATA`.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (to the kom CH0 named; held ones packed and kept for CH1 final), body / face /
hair, equipment, textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`, trap NPC meshes; the class-change guide dialogs
(`adopt-june`, `luabind.py check`). Stage byte-exact (`kom.py stage-june`), pack each kom, ask once with the whole kom list, copy what the user
approves. Save `data\ErrorLog_pre_CH0b_rena.txt` first. Create the walk units the save lacks (classes 3, 12, 13, 102, 114 and a level-99 24,
class costume) with `createchar.py`, after a `db_backup`. Gate: `python tools/skillicons.py check 106 900 23000-23060 23200-23242 23400-23417
23600-23605` = PASS (stage what it lists; the shared `DLG_SKILL_Passive_*` sheets included; a June-side gap is a `JUNE_BUGS.md` row, as Raven's 535).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Rena per job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

### CH0b - Done (2026-11-07)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets, two guide Lua and the save only. **Copied (user approved, 2026-11-07):** `kom.py copy` x32, 32 `COPY OK`; `kom.py status`: 0 `COPY NEEDED`.
- **Reference:** `ErrorLog.txt` (211,409 B) saved as `data\ErrorLog_pre_CH0b_rena.txt` before staging.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_rena/ch0b_stage_list.tsv`: `STAGE-JUNE OK: 31 kom(s), 4762 staged (4459 new, 303 replaced, 0 identical), 0 refused, 38s`
  (dry run matched CH0). Replaced March files backed up under `24965799\kom_backup_stage_june61107_112732`. The 5 held motions (`Motion_Lire.x`, `_GA`, `_RTR`, `_SR`, `_WS`, `ch1_held_motions.tsv`) are not in the list: March's copies are untouched; CH1 final stages them.
  Differing class motions went to data033, the 54 June-only motions to new data201.
- **Lua:** `luasrc.py adopt-june data034 DLG_1ST_Class_Change_Guide_Lena.lua DLG_2ND_Class_Change_Guide_Lena.lua`: `ADOPT-JUNE OK (bytecode): 2 staged, 0 refused` (record `june2017_re/june_adopt_20261107_112754.tsv`).
  `luabind.py check`: 21 / 22 methods, 0 unbound, `RESULT: PASS`; `luasrc.py ledger --check` PASS (0 FAIL, 0 WARN); `core --check` PASS.
- **Packed:** `kom.py pack` x32 (31 + data034), 32 `PACK OK`, 0 removed entries. (A first pack run was stopped by the user after 12 koms; all 32 were re-packed from scratch.)
  Kom list to copy: data002 013 016 019 021 033 034 044 052 063 067 068 069 095 121 130 151 191 192 198 201 215 222 226 datai004 007 012 019 020 023 027 031. `kom.py status`: 32 `COPY NEEDED`.
- **Done-gate re-run:** `stage-june --dry-run` over the same list: `4762 staged (0 new, 0 replaced, 4762 identical), 0 refused`. 0 `missing` / `march-copy`.
- **Skill icons:** `skillicons.py check 106 900 23000-23060 23200-23242 23400-23417 23600-23605`: 130 skills, **2 misses**, `RESULT: FAIL` on two `NO_PIECE` rows: 23210 / 23234 `SI_A_RSR_MIND_OF_SHARPSHOOTER`
  (`DLG_Lena_SKILL_ScreenShot_04`). June's own `.TET` lacks the piece; `stage-june` reports both sheet files identical. June's gap, kept: `JUNE_BUGS.md` row 561. Icons are fine; nothing left to stage.
- **Save:** backup `db_backup\els_db_20261107_112929_pre_manual.sql`; added with `createchar.py` (lv 99, class costume + top gear +10): unit 86 ElvenRangerT (3), 87 CombatRangerT (12),
  88 SnipingRangerT (13), 89 WindSneakerT (102), 90 NightWatcherT (114), 91 TrapRanger99 (24). Existing: 12 reyaa (103 lv99), 35 TrapRangerT (24 lv70).
  (89 / 90 / 91 were created after the game was closed: `createchar` refuses while it runs.)
- **Play-test (user) is the gate** after the copy. Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_rena.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh). Class script and motions are still March's, so Rena plays as before; her new meshes / textures / sounds load.
- **Play-test and logs (user: all passed, 2026-11-07; run 11:36-11:40, after the deploy):** `ErrorLog.txt` (1,315 lines) against `ErrorLog_pre_CH0b_rena.txt`: **0** `KEM_ERROR`, **0** `Motion Error`; offline log 0 `EXCEPTION`, the 7 `UNHANDLED` are `E_DISABLE_HB_CHECK_REQ` (known). New lines are the pre log not having used Rena, all June's own or not Rena's:
  - ~330 `OpenXET` failures (missing `.XET` sidecars, every class logs them; mesh, motion and Ruben-village files); no texture or mesh the stage list should have carried.
  - 4 `OpenTexture`: `LIRE_BASIC_FACE_LOSE.TGA`, `LIRE_BASIC_FACE.TGA.TGA`, `LIRE_BASIC_FACE_NW.TGA.TGA`, `LIRE_BASIC_FACE_NW,TGA`: June's meshes name them (checked in the bytes, absent from March's), no kom has them: `JUNE_BUGS.md` row 562.
  - `MOVIE_CREATE_RENA_BASE / WS / GA / NW.LUA` `LoadDataFile` (class-creation movie previews; `.avi` and subtitle scripts are `dangling` in `ch0_assets.tsv`, movie plays on): row 562.
  - 3 Ruben-village textures (`RUBEN_VILLAGE_HERB01 / 02`, `RUBEN_TECHHOUSE_GANPAN`): not Rena's, not in her list (Stage 15 asset port's); `[LUA_SRC] ChainQuestInfo.lua ... AddGroupChainQuest` (x1, startup): Stage 4 quests, not Rena's.
- **Drift:** none new (the Ruben textures and `AddGroupChainQuest` are existing backlog stages').

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high, Opus review | Movement and slot-head states: wait (2) + wait-state-change, walk, jump (7), dash (16), double jump (3), throw item, common buff, hyper, charge |
| CH1b | Sonnet high, Opus review | Reactions: damage (21 `LESI_DAMAGE*`), stand-up (6), die (2), start (2), win / lose blocks, wall, amusement (2), frozen, transformed |
| CH1c | Sonnet high, Opus review | Riding (27 `LESI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high, Opus review | Rena ground combos (19 `LESI_COMBO*`) and the base dash / jump combos CH0 assigns |
| CH1e | Sonnet high, Opus review | Combat Ranger line (`COMBAT`, `LCR` / `RCR`) and Wind Sneaker line (`LWS` / `RWS`) |
| CH1f | Sonnet high, Opus review | Sniping Ranger line (`SNIPING`, `SIEGE`, `LSR` / `RSR`) and Grand Archer line (`LGA` / `RGA`) |
| CH1g | Sonnet high, Opus review | Trapping Ranger line (`RTR`) and Night Watcher line (`LNW` / `RNW`, 22 tables) |
| CH1h | Sonnet high, Opus review | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32c6da50` uncarved; ctor `32ca2b60` / dtor `32ca42f0` offset map; InitComponent `32c895b0` with 7c's alpha sites; the uncarved ones read from the listing) |
| CH1i | Sonnet high, Opus review | Overrides 0x600-0x7bc (InitPassiveSkillState `32ca78b0` only as a declaration: CH4a) |
| CH1j | Sonnet high, Opus review | Common* overrides 0x7c0-0x7ec (`0x7c0` / `0x7d8` uncarved), CommonFrameMove and the helpers only it reaches |
| CH1k | Sonnet high, Opus review | Skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied), SpecialAttackEventProcess `32c6a970` (uncarved) with its parts `32cac200` / `32ca50b0` / `32c89a20`, 0x810 / 0x824 / 0x834 (uncarved), and **`ReservedSkillStart`** (Rules) |
| CH1m | Opus high, Opus review | Registration: June's `InitState` `32cadc90` / `InitStateCommon`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), START / WIN / LOSE roll (June's `INIT_COMMON_RANDOM_STATE` keyed by class name), base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks Rena registers (Raven's CH1n triage first: only rows earlier characters did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32cb06d0` head (bounds, slot tables, the 7i rule, the pre-switches) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals Rena overrides; `SetSkillLevelStateData` `:22990` with the `( 3201ad10( skill ) + 1.0 )` term (§7 l.286); gaps the CH1a-CH1k reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (her decode, `CountUsedSkill`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here (`BOW_SKILL_STATE`, `IMMUNE_TO_SUPER_ARMOR_DOWN`) |
| CH1q | Opus high, Opus review | Natural force (`ch0_nf.tsv`, Open decision 1): per the answer, `_NF` defines `ADD_RENA_SYSTEM` (with its `ServerDefine.h` second home and `SECOND_HOMES` row), then June's bodies inside every March NF function that differs (class tail, `X2GUUser`, `X2DamageManager` / `X2DamageEffect` / `X2EffectSet` first-attack copies, `X2GUNPC` incl. `AttackResultByType` `32773ee0`, `CX2RenaGageData` / `CX2RenaMyGageUI`, `X2GageManager`), June-only pieces under plain names (`FriendshipNatureData`, `32cb50c0` / `32cb6290`, the `ApplyHyperModeBuff` Rena branch l.959), each flipped June-port arm resolved to June's branch, the studio overlay / dev key dropped if June has none, the emulator's `CAC_NATURAL_FORCE` case; sub-tag `NF` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `Lire_ElvenRanger.lua`; copy CH0b's held motion koms (if any); pack, ask, copy; the play-test |

**Transcription rules (CH1a-CH1k):** as Raven's (`JUNE_CHARACTER_RAVEN_PHASES.md`, "Phase CH1 - Deploy unit: June script, motions and the class
core", "Transcription rules"), with Rena's flags and her class range `0x32c6a8d0`-`0x32cb6c6d`: `inside` rows inside March's function under
`SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` with March's in `#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`;
a later group's callee declared with `// June <va>: body in <phase>`; an unmapped engine callee is "this needs Opus" (stop). June's numbers and
quirks verbatim; `m_fNowStatePowerRate` (7i) with June's operator; June's BFI arguments (7ai). **A March block under `ADD_RENA_SYSTEM` inside a
function a CH1a-CH1k phase transcribes stays as it is (dead) until CH1q:** the June body's natural-force lines are written under
`SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (an `#ifdef` inside the `_LIVE` body), never under the studio macro, so CH1q switches them on together.

**Done gate (CH1 final):** Rena (any class) loads June's script and motions; `[JUNE_CHAR] RENA DATA` logs the load; no new `KEM_ERROR` /
`Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, double jump, damage and riding play June's animations;
natural force fills, spends and shows on the gauge as June (per decision 1); each skill cast twice shows its cool time and spends MP;
`charport_scan.py` / `charport_keys.py Lire_ElvenRanger` show nothing new.

### CH1a - Done (2026-11-07)

All 32 class-map rows of phase CH1a are written, all `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`. No `new` row, so no `_CORE` define yet.
`Always.h` gets `_LIVE` and `_NF` as `//#define` (guards: `_LIVE` needs `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`; `_NF` needs `_LIVE`). New file-local namespace `_CONST_RENA_CH1A_` (after the `_CONST_RENA_` block): `IsMPChargeBlockedByGame()` = false.
36 funcmap rows `Char Rena CH1a`; `june2017_re/char_rena/ch1_offsets.tsv` is new (Raven's base rows + 16 Rena rows).

| June | `CX2GULire_ElvenRanger::` | June | `CX2GULire_ElvenRanger::` |
|---|---|---|---|
| `32c6aaa0` `32c87b90` `32c6dee0` `32c89bd0` | Wait StartFuture / FrameMoveFuture / Start / EventProcess | `32c89e80` `32c83760` | Walk / JumpReady EventProcess |
| `32c6aad0` `32c6df60` `32c8a080` | JumpUp / JumpDown / JumpLanding EventProcess | `32c6e0c0` `32c6e0f0` `32c8a2f0` | LESI_DASH StartFuture / FrameMoveFuture, Dash EventProcess |
| `32c6abb0` `32c6abc0` `32c83800` `32c6e150` `32c8a4f0` | DashEnd StartFuture / Start / FrameMoveFuture / FrameMove / EventProcess | `32c83850` `32c8a800` | DashJump / DashJumpLanding EventProcess |
| `32c6abd0` `32c83b90` `32c861c0` | DoubleJump StartFuture / FrameMoveFuture / EventProcess | `32c6abf0` `32c83c10` `32c6e1a0` | DashDoubleJump StartFuture / FrameMoveFuture / EventProcess |
| `32c8bb50` `32c8bdd0` `32c8c080` `32c8cb90` `32c6ac90` `32c8ce00` | ChargeMP StartFuture / Start / FrameMove / EventProcess / EndFuture / End | `32c6acc0` | HyperModeInit |

**Where June differs from March** (everything else is March's logic written out whole; read from `charport_adis.py rena` listings and the Ghidra decompile):
- **Wait charges MP from the state time:** `fStateTime >= 1.0` (+0x7de8) replaces `m_fChangeMPTime >= 1.0`; June neither resets nor sums `m_fChangeMPTime`. The inner test adds `g_pX2Game` and the CX2Game `+0xc20` byte (clear to enter; ChargeMP EventProcess leaves to WAIT when it is set). `+0xc20` has no March member and no June writer but its constructor (same byte as Arme's): false.
- **Both keys held** (`pureRight && pureLeft`): DashJump (as Raven: only a fresh press against the facing clamps the jump speed and picks JUMP_UP / JUMP_DOWN); DoubleJump / DashDoubleJump FrameMoveFuture (a fresh press picks the facing, the speed is set either way).
- **ADD_RENA_SYSTEM branches June runs unconditionally**, written plain (every state they name exists in March): JumpUp / JumpDown dash-jump (`GetEnableDash( EDS_END )`), both double jumps send X to `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_X` (the JJX guide only outside classes 13 / 103; DashDoubleJump has no class switch). `_NF`: WaitStart's `m_eSaveStateSkillId = SI_NONE` and relate-data `>= 10 -> 0` only.
- **ChargeMP StartFuture:** the x0.8 dungeon / battle-field scale goes through June's `32071a50` (skipped for a user with byte `+0x6894` == 1). A scan of the exe finds 7 readers and no writer, so the gate always passes = March's `SetChangeRateMp`. `ResetMPChangeRate` `31fd9e00` has the same gate (stage33 row, not mine).
- **= March after the macros:** Walk, JumpReady, JumpLanding (skip-point chain), Dash, DashEnd (split at `( max - transition ) / 8`, Night Watcher accel dash, `EventTimerOneshot( 0.4 )`), DashJumpLanding, the DoubleJump StartFutures, ChargeMP Start / FrameMove / End (strings, 8 / 15 / 10 / 5 / 200 / 85 constants checked), HyperModeInit. Hyper test = `TryHyperModeStateChange( USI_HYPER_MODE )`, `SpecialAttackEventProcess` gets `ASUT_GROUND` / `ASUT_AIR`.
- Candidates, not proven (review confirms): `322cdfa0` = `GetSlideShot()->IsPresentNow()`, `31aadcb0` = `g_pX2Game != NULL`. `LESI_WAIT_STATE_CHANGE` / COMMON_BUFF / THROW_ITEM callbacks are shared base targets (nothing in the Rena range); registration is CH1m. No `JUNE_BUGS` row: nothing here is a June defect.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_NF --worktree --files X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: 1,169 / 0 in the `.cpp`, 27 / 0 in `Always.h`.
- One-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` (no PCH): 0 errors with `_LIVE` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `RESULT: PASS` (0 allow-listed). `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,078,208 B, 2026-11-07 12:01:37`; `DEPLOY OK: X2_offline.exe is 26,078,208 B, dated 2026-11-07 12:01:44, md5 6dcf1ace179ee0392bc292fbe72a37ab, equal to X2/US_SERVICE/x2.exe`. No new string in the exe (`_LIVE` off), so no `exestr` line.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1a review` (Opus 5.5 high), then CH1b.

### CH1a - Review (2026-11-07, Opus 5.5 high)

**Verdict: pass, no code change.** No transcription error found. Every branch order, `StateChange` target, compare direction and constant I checked matches the `charport_adis.py rena` listing.
- **Read end to end against the listing:** Wait StartFuture / Start / FrameMoveFuture / EventProcess, Walk, JumpUp, JumpDown, DashJump, DoubleJump and DashDoubleJump (FrameMoveFuture + EventProcess), ChargeMP StartFuture / EventProcess / End, and ChargeMP FrameMove's constants and strings (`300 * elapsed` in radians, `55 + 30` folded to 85, 200, `-1.0` = `CreateSequence` defaults).
- **Wait `32c89bd0`:** the charge test is `fStateTime >= 1.0` (`fld1; fcomp; test ah,0x41; jp`), `Now != Max` (`test ah,0x44; jnp`), slide show off, `+0x9700` clear. A failed inner gate (emotion, `g_pX2Game`, `+0xc20`) exits without trying the down or riding arms. That matches the nested `if` inside the `else if`. `g_pX2Game` is dereferenced for the slide show before June's own NULL test: a June quirk, kept.
- **Both candidates confirmed:** `322cdfa0` = `CX2Game::GetSlideShot()`: it returns `g_pData(33775860) +0x27c`, ignores `ecx`, and is March `X2Game.cpp:16378`. The caller's byte-0 read is `IsPresentNow()` inlined (`CX2SlideShot` has no vtable; `m_bNowPresent` is its first data member). `31aadcb0` returns `arg1 != handle` = `g_pX2Game != NULL`. The `funcmap.tsv` and `ch0_unmapped_identified.tsv` rows are now `confirmed`.
- **DashJump `32c83850`:** the Z switch is 24 / 114 -> `LESI_DASH_JUMP_ATTACK_RTR_Z` (0xdf), 103 -> `LESI_LGA_DASH_JUMP_Z` (0xd5), default (including a NULL unit; March `GetUnitClass()` checks for NULL) -> `LESI_DASH_JUMP_ATTACK_Z` (0xbf). The both-keys arm, the clamp to `min( nowSpeed.y, GetJumpSpeed() )`, the `< LINE_RADIUS * 2` (60) flag tail and the `oneDown` tail all match.
- **Double jumps:** FrameMoveFuture's both-keys facing and speed match (`GetWalkSpeed` / `GetDashSpeed` = triple x `m_fRunJumpSpeedRate`). In `32c861c0`, X goes to state 0x9e for every class, and JJX is shown only outside 13 / 103. `ShowComboCommand`'s third argument is `true` in June, which is March's default (`X2ComboGuide.h:56`), so the two-argument call is exact. `32c6e1a0` reads the class but both arms are the same (no guide).
- **ChargeMP StartFuture `32c8bb50`:** `32071a50` is inlined: the x0.8 is skipped only when byte `+0x6894 == 1` on a user. **Correction to the Done note:** the field has one writer, the CX2GameUnit constructor's zero-init `mov word [esi+0x6893], bx` (ebx = 0) at `322a5120`. The object reads are at 31fd9e03, 32071a50, 3277cbad, 32c8bd8e and 32ee33c6 (the "7" in the Done note counted a stack write and a duplicate decode). The conclusion stands: always 0, the gate always passes. `ch1_offsets.tsv` and the `32071a50` row are reworded. EventProcess `32c8cb90`: `Now == Max`, the slide show, riding, then `+0xc20` -> WAIT, as written.
- **`ADD_RENA_SYSTEM` split confirmed:** June runs the JumpUp / JumpDown dash-jump arms (`GetEnableDash( 2 )`, and `EDS_END` = 2 is March's default) and the 0x9e double-jump arm with no macro; every state they name exists in March. WaitStart's `m_eSaveStateSkillId` exists only under `ADD_RENA_SYSTEM` (`.h:805`), and `m_byteRelateData` is NF data (its comment: max NF consumed), so both stay under `_NF`. `m_bIsSiegeSkill` stays plain.
- **`ch1_offsets.tsv`:** the 16 Rena rows agree with every listing read (0x9598 / 959c / 95a0 / 95a4 / 95f0 / 9620 / 96b8 / 96bc / 9700 / 9758 / 928a). WaitFrameMoveFuture's "written only when not 0" is `KProtectedType<int>::operator=` inlined, so `m_iSiegeAttackCount = 0` is exact.

**Gates.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_NF --range 1ceb9778^..1ceb9778`: `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because the flag is `//#define` (by design). `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. One-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` (census `cl_cmd`, no PCH): 0 errors with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No code changed, so no build (CH1a's `DEPLOY OK` md5 `6dcf1ace...` stands).

Nothing to play-test. Next: `/x2-phase char-rena CH1b` (Sonnet 5.5 high).

### CH1b - Done (2026-11-07)

All 36 class-map rows of phase CH1b are handled. 33 are `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`. 1 is `new` (`_CORE`, compiled, referenced only by `_LIVE` bodies). 2 need no code. `_LIVE` stays `//#define`. 36 funcmap rows `Char Rena CH1b`; 10 rows added to `june2017_re/char_rena/ch1_offsets.tsv`.

**CH0's three `new ?` helpers were March code.** `32ca6e90` = March `ClearStartOfDelayedFiringData` (now `inside`); `32cb6310` / `32cb61e0` = its lambda and the `ApplyFunctionToLiveInstances` instantiation (no code; types 2 / 4 / 6 / 10 / 12 / 14 read from the byte table at `32cb6278` = the six `DET_*_GROUND` March tests). The only truly new function is `32ca2010`.
`Always.h`: `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` defined, and `_LIVE needs _CORE` added to the guard.

| June | `CX2GULire_ElvenRanger::` | June | `CX2GULire_ElvenRanger::` |
|---|---|---|---|
| `32c6de20` `32ca9230` `32ca5290` `32c6aa40` | DieFront StartFuture / Start / FrameMove / EventProcess (also `LESI_RIDING_DIE`) | `32c6de80` `32ca9360` `32ca52d0` `32c6aa70` | DieBack StartFuture / Start / FrameMove / EventProcess |
| `32c6e5e0` `32c6e630` | DamageDownFront FrameMove / EventProcess | `32c6ea30` `32c6ea80` | DamageDownBack FrameMove / EventProcess |
| `32c6ad20` `32c6ee80` | DamageAirDownLanding Init / EventProcess | `32c6f230` `32c6f290` `32c6f2f0` `32c6f350` | StandUpRolling FrontFront / FrontBack / BackFront / BackBack EventProcess (also the four `P_LE_AGILE_MOVEMENT`) |
| `32c6ad50` `32c6adb0` `32c6ae10` `32c6ae70` | the four Rolling EndFuture | `32c6ad70` `32c6add0` `32c6ae30` `32c6ae90` | the four Rolling End |
| `32c6f3b0` `32c6f440` `32c6aed0` | StandUpAttackFront FrameMove / EventProcess / End | `32c6f490` `32c6f510` `32c6af10` | StandUpAttackBack FrameMove / EventProcess / End |
| `32ca6e90` | ClearStartOfDelayedFiringData | `32ca2010` | **new** `KillMySummonedNPC( int iNPCID_, float )` (`.h` next to `SetActiveStartOfDelayedFiring`) |

**Where June differs from March** (everything else is March's logic written out whole, read from `charport_adis.py rena --phase CH1b`):
- **Stand-up flags also fire on animation end** (DamageDownFront / Back flags 5 / 6 / 7, AirDownLanding 4 / 5 / 6): June tests `stateChange < time || IsAnimationEnd()` and runs the time-window tests before the key test. This is the expansion Chung already uses; the same macro, `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_`, is defined at the top of the Rena `.cpp` under `_LIVE`. The rest (agile gate, classes 12 / 102, `fInputStartTime`, flag writes, Back's swapped left / right) = March. The no-down roll at `+0x4060` leads to the same attack state both ways (March has it too, `// fix!!`), so no `JUNE_BUGS` row.
- **Die FrameMove adds `KillMySummonedNPC( NUI_RENA_TARGETING_GUNGNIR, 3.0f )`** after `DieFrameMove` (`32ca2010`: walks the game's units; NPC, templet id match, HP > 0, AI `NAT_ALLY` with my UID as owner; `SetNowHp( 0 )` only when the id is `0x823`). The second argument is never read; the other callers (`32ca42f0` ctor-side, `32ca4a40` ReInit) are CH1h's. Modelled on `CX2GUUser::KillOlderSummonsOfSameID` (Eve CH2b2).
- **= March after the macros:** Die Start (`DyingStart`, play speed 0.5, the Night Watcher block unconditional in June), DamageDown FrameMove (`EventTimerOneshot` 0.19 / 0.32), AirDownLanding Init, the four Rolling EventProcess (listings identical), EndFuture (`bIsRight` false / true / false / true), End (0.3 / 0.3), StandUpAttack FrameMove (Front: `EventTimer` 0.36 / 0.8, Back: `EventTimerOneshot` 0.43, both then `DamageCheck` for 0.15 < t < 0.5), `ClearStartOfDelayedFiringData`.
- Ids: March names throughout (`USI_DAMAGE_AIR_FALL`, `USI_WAIT`, `LESI_STANDUP_*`, `LESI_P_LE_AGILE_MOVEMENT_*`); June's registration ids map in CH1m. `LESI_DIE_*` stays as March registers it. The Luciel second-body future-animation accessor (`32070cf0`) is read as `m_pXSkinAnimFuture` (not in March).
- Not in this phase although the class map's CH1b state list names them: the Start / FrameMove of the damage, win / lose and wall states are base callbacks (nothing in the Rena range); CH1m registers them.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h KTDXLIB/Always.h`: `files: 0 PASS, 1 PASS~, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `--with` omitted: `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`: `RESULT: PASS`. `git diff --patience --numstat`: `.cpp` 649 / 0, `.h` 3 / 0, `Always.h` 11 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH) of `X2GULire_ElvenRanger.cpp`: 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P`: `KillMySummonedNPC` 2 lines off / 4 on, so the flag took effect.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,078,720 B, 2026-11-07 12:44:42`; `DEPLOY OK: X2_offline.exe is 26,078,720 B, dated 2026-11-07 12:44:49, md5 eea2e62875b79a16b770d31a8a085381, equal to X2/US_SERVICE/x2.exe`. `_LIVE` is off and no string was added, so no `exestr` line.

**Routed.** CH1h: ctor-side `32ca42f0` and ReInit `32ca4a40` call `KillMySummonedNPC` (the `_CORE` function is ready). CH1m: register June ids for the stand-up and agile-movement states.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1b review` (Opus 5.5 high), then CH1c.

### CH1b - Review (2026-11-07, Opus 5.5 high)

**Verdict: pass, no code change.** I found no transcription error. Every branch order, `StateChange` target, compare direction and constant I checked matches the `charport_adis.py rena` listing.
- **DamageDownFront `32c6e630`, read end to end:**
  - Not on a line: `USI_DAMAGE_AIR_FALL`, then `+0x7e40 -= 45` (= `LINE_RADIUS * 1.5`).
  - Animation end: flag 4 rolls `GetRandomInt( 0 )` (vslot 0x1c8) against `+0x4060`. Both outcomes push `0xa5`; the `jge` only skips to the shared call. Then flag 1 -> 0xa5, 2 -> 0xa1, 3 -> 0xa2, else 0x20.
  - Flags 5 / 6 / 7 (`+0x7d8a`-`+0x7d8c`): change on `stateChange < time` (`test ah,5; jnp`) or `IsAnimationEnd`, giving 0xf2 / 0xa2 (`ROLLING_FRONT_BACK`, not the agile state, = March) / 0xa5. The window test `time < keyInputEnd`, then `keyInputStart <= time` (`test ah,0x41; jp`), then agile, the key flag and class 12 / 102. The macro expands in exactly that order.
  - Key arm: `fInputStartTime < time` is strict. oneX, oneZ, then facing; the word and dword stores (`word +0x7d87`, `dword +0x7d86 = 0x10000`) decode to the flag sets the code writes.
- **DamageDownBack `32c6ea80`:** the listing differs from Front only in the state ids (0xa6 / 0xa3 / 0xa4 / 0x21 / 0xf4 / 0xa4 / 0xa6) and in the key arm's left / right test order. With facing right, pureLeft -> flag 2; with facing left, pureLeft -> flag 3. The code matches.
- **AirDownLanding `32c6ee80`:** animation end goes 1 -> 0xa5, 2 -> 0xa1, 3 -> 0xa2, else 0x20. Flags 4 / 5 / 6 give 0xf2 / 0xf3 (`AGILE_FRONT_BACK` here) / 0xa5. `oneZ || oneX` sets only flag 1. The facing arms are as written.
- **`KillMySummonedNPC` `32ca2010`:**
  - The loop calls `GetUnitNum` (vslot 0xf8) again on every pass. `+0x264 == GUT_NPC` (1). The templet test `+0x85f8 [0] == id` is skipped when the id is 0.
  - Gage `+0xc04`: when it is NULL, June skips the HP test. March's `GetNowHp()` returns 1 in that case, so the result is the same. When `+0x20` is NULL, June skips the unit, and `CX2GageData::GetNowHp()` returns 0 there, so the result is again the same.
  - AI: `+0x7c98`, then `+0x38 == NAT_ALLY` (1), then the 64-bit `+0x78` / `+0x7c` against vslot 0x54 `GetUnitUID()`. Then `0x823` (= `NUI_RENA_TARGETING_GUNGNIR`, 2083) and `SetNowHp( 0 )` (vslot 0xf4).
  - `ret 8` with only `[esp+0x1c]` read: the float argument is never read. Confirmed.
- **`ClearStartOfDelayedFiringData` `32ca6e90` = March:**
  - Unit walk: entries `+4` `m_iAttackerIndex` against `+0x268`, and `+0x10 = 0.01`. Then `ApplyFunctionToLiveInstances` (`32cb6310`, which tests the live flag at `+0xf5`) with the lambda `32cb61e0`.
  - The lambda tests the attacker observer, switches on `type - 2` (0..12) through the byte table at `32cb6278` (`00 01 00 01 00 01 01 01 00 01 00 01 00`), and calls `DestroyInstanceHandleSilently`. Index 0 is the destroy arm, so the types are 2 / 4 / 6 / 10 / 12 / 14. These are the six `DET_*_GROUND` values in `X2DamageEffect.h:92-105`.
- **Die:**
  - Start `32ca9230` / `32ca9360` (identical listings): `CommonStateStart`, `DyingStart` (vslot 0x338), play speed 0.5. Then `DeletePriorPlannedBlowEffect` inlined: stop `+0x96f8`, write -1, `+0x96fc = 0`, which is March's body. Then stop `+0x970c`, `+0x9700 = 0`, `IsMyUnit` -> `UpdateSkillSlotUI( false )` (March's default), and a tail jump into `ClearStartOfDelayedFiringData`. None of this is guarded in June.
  - FrameMove `32ca5290` / `32ca52d0`: `DieFrameMove( 0.19, 1.12 )` / `( 0.32, 0.9 )`, then `KillMySummonedNPC( 0x823, 3.0 )`.
- **StandUpAttack:**
  - Front FrameMove: `EventTimer( 0.36 ) || EventTimer( 0.8 )` (a short-circuit `je`) with an empty body.
  - Back: an inlined `EventTimerOneshot( 0.43 )` with an empty body.
  - Both: `0.15 < t < 0.5` -> `DamageCheck`.
  - EventProcess: on animation end, `JUMP_DOWN` (-45) or `WAIT`. End: invincible 0.3 / 0.3.
- **Rolling** EventProcess `32c6f230` (JUMP_DOWN -45 / WAIT): matches.
- **`ch1_offsets.tsv`:** the 10 CH1b rows (`0x7d8a`, `0x4060`, `0x9724`, `0x96f8`, `0x970c`, `0x85f8`, `0x7c98`, `0xc04`, `0x393c`, `0x268`) agree with every listing I read.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range 5090b818^..5090b818`:
  - `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`.
  - `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  - `RESULT: FAIL` only because `_LIVE` is `//#define`, which is by design. The OTHER row is the `.h` declaration under `_CORE`.
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range 5090b818^..5090b818`: `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`.
- One-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` (`june2017_re/census.py` `cl_cmd`, no PCH): 0 errors three ways (no extra defines; with `_LIVE`; with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`).
- No code changed, so there is no build. CH1b's `DEPLOY OK` (md5 `eea2e628...`) stands.

Nothing to play-test. Next: `/x2-phase char-rena CH1c` (Sonnet 5.5 high).

### CH1c - Done (2026-11-07)

All 3 class-map rows of phase CH1c are written, all `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`. No `new` row, no `_CORE` code; `_LIVE` stays `//#define`. 3 funcmap rows `Char Rena CH1c`.
CH0 listed `32c9f4f0` as `new ?`; March has `CommonHyperModeFrameMove` (`MODIFY_RIDING_PET_AWAKE`, on), so it is `inside` (class-map row corrected). The 37 riding / pepper run / dance / amusement states have no other class-range function (CH0): they are registered in CH1m with base callbacks.

| June | `CX2GULire_ElvenRanger::` | Note |
|---|---|---|
| `32c6d420` | `RidingHyperModeInit` | the six textures of `HyperModeInit` (`32c6acc0`, byte-identical listing), written out |
| `32ca7040` | `RidingHyperModeFrameMove` | `CommonHyperModeFrameMove( 0.f, 0.1f, true )` = March |
| `32c9f4f0` | `CommonHyperModeFrameMove( fTime1_, fTime2_, bSound_ )` | March + awakening stand-ins |

**Where June differs from March** (read from the listing, `charport_adis.py rena 32c6d420 32ca7040 32c9f4f0`):
- Timer 1 and timer 2 stay an `else if` (the timer-1 arm jumps to the closing `CommonFrameMove`); Raven's are two independent `if`s. Both timers are the inlined `EventTimerOneshot`; world colour `0xff222222`; timer 1 `ShowMinorParticleHyperModeChange( L"Dummy1_Rhand" )` with no sound.
- Timer 2: the awakening effect-set family `32011d20` (begin) and `31fd66b0` (present test, `32011f50` play when true) before the Shock, and `ShowMinorParticleHyperModeTrace` only when the set is absent. Stand-ins in `_CONST_RENA_CH1C_` (no-op, `false`, no-op = June's empty-name default), the same as Raven's, Chung's, Eve's and Elsword's (Stage 12 owns the reader).
- Rena has no Open Mind line and no `Raven_Slash1` sound: `Energy`, `Break`, `Change` = March. `UpDownCrashCamera( 20, 0.3 )` and the inline `ClearScreen()` = March. No `JUNE_BUGS` row.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). `git diff --patience --numstat`: `.cpp` 60 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P`: `RenaAwakeningEffectBegin` 0 lines off / 2 on, so the flag took effect.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `RESULT: PASS` (0 allow-listed). `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,078,720 B, 2026-11-07 12:56:52`; `DEPLOY OK: X2_offline.exe is 26,078,720 B, dated 2026-11-07 12:56:59, md5 3d80b388a5ba5f913a243ede92d2ef2b, equal to X2/US_SERVICE/x2.exe (2026-11-07 12:56:52); built at main 3335d9da + 1 uncommitted code file(s).` `_LIVE` is off and no string was added, so no `exestr` line.

**Routed.** CH1i: `HyperModeFrameMove` `32ca5310` calls this function with its own times (March: `0.2f, 0.75f`). CH1m: register the riding / pepper / dance states' base callbacks. Stage 12: the awakening reader behind the three stand-ins.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1c review` (Opus 5.5 high), then CH1d.

### CH1c - Review (2026-11-07, Opus 5.5 high)

**Verdict: pass, no code change.** I found no transcription error. I read all three listings (`charport_adis.py rena 32c6d420 32ca7040 32c9f4f0`) end to end.
- **`CommonHyperModeFrameMove` `32c9f4f0`:**
  - World colour: the inlined `SetWorldColor` writes 0.1333 x3 / 1.0 to `m_NowColor` (`+0xb4`) and `m_FadeColor` (`+0xc4`), then `m_fFadeTime` (`+0xd4`) = 0. That is `0xff222222` = March.
  - Both timers are the inlined `EventTimerOneshot` (`m_fAnimOneshotTimer < fTime`, `test ah,0x41; jne`, then `EventTimer`). Timer 1's arm jumps to `32c9f77f` (`CommonFrameMove`), and both failure exits fall into timer 2, so it is an `else if`.
  - Timer 1: `ShowMinorParticleHyperModeChange( L"Dummy1_Rhand" )` and no sound. March's no-argument call is the same: the parameter defaults to `L"Dummy1_Rhand"` (`X2GUUser.h:2608`).
  - Timer 2, in order: `32011d20`; `31fd66b0`, `cmp al,1` -> `32011f50`; `Shock( L"Dummy1_Rhand" )`; `UpDownCrashCamera( 20, 0.3 )`. Then the inlined `ClearScreen()` (manager `+0x1fc`: colour 1,1,1,1 and count 1, the `KTDGManager.h:133` defaults). Then `31fd66b0` again, and `Trace` only on false. Then vslot 0xc0 `ApplyHyperModeBuff`, then `bSound_ == 1` -> `Energy` / `Break` / `Change`. `PlaySound( name, 1, 0 )` matches March's defaults. There is no Open Mind line and no `Raven_Slash1`.
  - Then vslot 0x7cc `CommonFrameMove`, `ret 0xc`.
  - The three stand-ins in `_CONST_RENA_CH1C_` (no-op, `false`, no-op) match Raven's, Chung's, Eve's and Elsword's.
- **`RidingHyperModeFrameMove` `32ca7040`:** `fld 0.1` -> `[esp+4]`, `fldz` -> `[esp]`, `push 1`. That is `( 0, 0.1, true )` = March.
- **`RidingHyperModeInit` `32c6d420`:** six `TextureReadyInBackground` calls (ColorBallBlue, GroundShockWave, WhiteCircle, WhitePoint, ToKang, Energy), in the order written.
- **State coverage (`ch0_reg.tsv` x `ch0_class_map.tsv`):** the riding / pepper / dance / amusement registrations have class-range callbacks only in `LESI_RIDING_HYPER_MODE` (Init / FrameMove, CH1c) and `LESI_RIDING_DIE` (`32c6de20` / `32ca9230` / `32ca5290` / `32c6aa40`, all CH1b rows). No row is missing. The registration table has 38 such states (riding 26, pepper 5, dance 3, amusement 4), not 37. That is a count slip in CH0 / CH1c, with no effect on the map.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range 036f0b04^..036f0b04`:
  - `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`.
  - `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  - `RESULT: FAIL` only because `_LIVE` is `//#define`, which is by design.
- `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`.
- One-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` (`census.py` `cl_cmd`, no PCH): exit 0 and 0 errors three ways (no extra defines; with `_LIVE`; with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`).
- No code changed, so there is no build. CH1c's `DEPLOY OK` (md5 `3d80b388...`) stands.

Nothing to play-test. Next: `/x2-phase char-rena CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-11-09)

All 38 class-map rows of phase CH1d are written. 35 are `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`. 2 are `new` (`_CORE`, compiled, referenced only by `_LIVE` bodies): `32c85330` `GetArrowMpConsumption( fMp_ )`, `32c7fe20` `GetArrowMaxLifeTime( fMaxLifeTime_ )`. 1 needs no code: `32c7bdd0` is March's `CreateDamageEffectIndex` (CH0 said `new ?`; class-map row corrected). `_LIVE` stays `//#define`. 38 funcmap rows `Char Rena CH1d`.

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c6b0b0` `32c6b0c0` `32c6b0d0` `32c6b0e0` `32c6b0f0` `32c6b100` `32c6b110` `32c6b120` | `ComboZ1` / `Z4Up` / `X1` / `X2` / `X3` / `X4` / `X4Up` / `X4Down` FrameMoveFuture (thunks to `CommonFrameMoveFuture`) |
| `32c84340` `32c843b0` `32c84420` | `ComboZ2` / `Z3` / `Z4` FrameMoveFuture (0.3 / 0.125 / 0.28 oneshot, walk / walk / dash speed) |
| `32c6f610` `32c6fa80` `32c700b0` `32c706a0` `32c70c00` | `ComboZ1` / `Z2` / `Z3` / `Z4` / `Z4Up` FrameMove (damage windows = March) |
| `32c6f670` `32c6fae0` `32c70110` `32c70700` `32c70980` `32c70c60` | `ComboZ1` / `Z2` / `Z3` / `Z4` / `Z4a` / `Z4Up` EventProcess |
| `32c8d390` `32c8d850` `32c8dd00` `32c8e0c0` `32c8e8b0` `32c8ec70` `32c8f6d0` | `ComboZ4a` / `X1` / `X2` / `X3` / `X4` / `X4Up` / `X4Down` FrameMove (the arrows) |
| `32c70ee0` `32c712f0` `32c71700` `32c71bc0` `32c71e40` `32c720c0` | `ComboX1` / `X2` / `X3` / `X4` / `X4Up` / `X4Down` EventProcess |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1d`):
- **EventProcess = March's `ADD_RENA_SYSTEM` combo revamp** (which stays dead until CH1q), with two changes: every flagged follow-up also fires when the future animation ends (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`, the `m_fEventTime` form), and the walk / back-walk / dash cancels also need the foot on a line (`RENA_GROUND_*`; `SKILL_CANCEL_AFTER` already has it). Rena's order is skill, walk, back-walk, dash; no Z / X cancel.
- **X1 / X2** add a Combat Ranger / Wind Sneaker (12 / 102) `oneZ` follow-up: X1 to `LESI_RCR_COMBO_XXZ` (June id 253), X2 to June id 257, the same script table registered a second time (`LESI_RCR_COMBO_XXZ_FROM_X2`). CH1m maps both ids. Both are `_CORE` enum members appended after `LESI_TRANSFORMED`.
- **Z1 and X3 FrameMoveFuture** no longer zero `nowSpeed` (March does; June's are bare thunks).
- **Arrows:** MP goes through `GetArrowMpConsumption` (Z4a 9, X1 4, X2 6, X3 8, X4 / X4 up / X4 down 9; March 6 / 4 / 3-4 / 1.5-4 / 3-4 / 15 / 15) and every arrow's main effect life time through `GetArrowMaxLifeTime`. Timers 0.3 / 0.24 / 0.28 / 0.38 + 0.43 / 0.431 x3, all `EventTimerOneshot`. Names by class: Z4a Night Watcher keeps the explosion arrows, else `DamageEffect_Rena_Combo_ZZZX_Arrow`; X1 `_RSR_` (13 / 103), `_RNW_` (114), else `_Rena_Combo_X_Arrow`; X2 / X4 `_RNW_` or `_Rena_Combo_XX` / `XXXX`; X3 Night Watcher explosion arrows else `_Rena_Combo_XXX_Arrow`; X4 up / down `_Up_XXXX_` / `_Down_XXXX_` for all three arrows. X3: first arrow `degree.z += 2`, second at 0.43 s `posR.y += 10`, `degree.z -= 2`. The `CreateInstance` argument lists equal March's (checked site by site against the listing).
- **Natural force** (`_NF`, off): `m_NaturalForceType = NFT_COMBO_BOMB` on the Z4a non-Night-Watcher arrow and every X4 / X4 up / X4 down arrow; `GetArrowMpConsumption` takes 0.1 per natural force point.
- **The helpers:** `GetArrowMpConsumption`: rate 1, with the buff `BTI_BUFF_MIND_OF_MARKMAN` (536) the equipped Hunter's Steadiness (`RNSI_A_RSR_MIND_OF_SHARPSHOOTER` 23210) gives `SA_MP_CONSUMPTION`; minus 0.1 per natural force point; clamp at 0. `GetArrowMaxLifeTime`: same buff, `SA_PROJECTILE_RANGE_REL`. Both names registered under `JUNE_ENUM_NAMES` (`X2SkillTree.h`, `X2BuffFactor.h`).
- **Routed:** the same helpers are called by the Sniping Ranger double-jump attacks (`32c8aa40`, `32c8b2c0`, CH1e / f) and by dash attack X / dash-jump X1 (`32c90130`, `32c90f10`, CH1d2): they use the declared functions. March's `SnipingRangerComboX2/X3/X4` stay as they are until CH1m registers June's single set. No `JUNE_BUGS` row (no June quirk found).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files` (the four files): `files: 0 PASS, 1 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR` (OTHER = `.h` under `_CORE`, `X2SkillTree.h` / `X2BuffFactor.h` under `JUNE_ENUM_NAMES`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `git diff --patience --numstat`: `.cpp` 941 / 0, `.h` 6 / 0, `X2SkillTree.h` 5 / 0, `X2BuffFactor.h` 5 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P`: `EventTimerOneshot( 0.431f )` 0 lines off / 3 on; `NFT_COMBO_BOMB` 0 / 0 / 22 (with `_NF`).
- `junediverge.py --check` on the four files: `RESULT: PASS` (0 allow-listed). `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,078,720 B, 2026-11-09 03:54:52`; `DEPLOY OK: X2_offline.exe is 26,078,720 B, dated 2026-11-09 03:54:58, md5 a255684a375de5275f642bfec66164e9, equal to X2/US_SERVICE/x2.exe (2026-11-09 03:54:52); built at main d105cff5 + 4 uncommitted code file(s).` `_LIVE` is off and no string was added to a live path, so no `exestr` line.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1d review` (Opus 5.5 high), then CH1d2.

### CH1d - Review (2026-11-09, Opus 5.5 high)

**Verdict: pass, no code change.** I read all 38 rows against `charport_adis.py rena --phase CH1d`, each cut at its first `int3`. Cancel tests were read by their x87 flag tests: `test ah,0x41` / `jp` = `0 <=`; `test ah,5` / `jp` = `<`.
- **Cancel chain** (all 11 EventProcess): skill `+0x8c90`, with the foot test and the awakening arm inside `SKILL_CANCEL_AFTER`; then walk `+0x8c98` (foot, right or left), back-walk `+0x8c94` (foot, key against facing) and dash `+0x8c9c` (foot, double right or left), in that order. No listing reads `+0x8c8c` or `+0x8ca4`. Z4a / Z4Up / X4 / X4Up / X4Down are the same listing apart from addresses.
- **Follow-ups:** each one changes on `stateChange < t || animation end` and sets its flag on `t < keyInputEnd && keyInputStart <= t && cond`. Flag / event-time pairs, in order:
  - Z1: 1/1 class 114 X -> `LESI_COMBO_RNW_ZZX`, then 0/0 Z.
  - Z2: 0/0 class 102 Z with the key toward the facing -> `LWS_ZZfrontZ`; 2/2 class 24 or 114 Z -> `RTR_ZZZ`; 1/1 Z -> Z3; 3/3 X class 103 -> `LGA_ZZX`.
  - Z3: 2/2 up Z -> Z4Up; 0/0 Z -> Z4; 1/1 X not class 103 -> Z4a; 3/3 X class 103 -> `LGA_ZZX`.
  - X1 / X2: 0/0 X; then 1/1 class 12 or 102 Z -> ids `0xfd` (253) and `0x101` (257).
  - X3: 2/2 down X, 1/1 up X, 0/0 X.
  - Direction clears: Z2 3 flags (word + byte), Z3 4 (dword), X3 3. `m_fCanNotAttackTime`: 0.1 (Z1) and 0.2 (Z2) only.
- **FrameMove / Future:** damage windows 0.1-0.24 / 0.31-0.4 / 0.125-0.21 / 0.28-0.37 / 0.24-0.35. Future timers: 0.3 and 0.125 walk speed, 0.28 dash speed. Z1, X3 and the other six Futures are bare vslot `0x7c8` thunks.
- **Arrows:**
  - Timers 0.3 / 0.24 / 0.28 / 0.38, 0.43 (an `else if`) / 0.431 x3. MP 9 / 4 / 6 / 8 / 9 / 9 / 9 through `32c85330`.
  - Names: class 13 / 103 / 114 for X1; class 114 for the others.
  - Z4a adds 180 to `angle.y`. X3: first arrow `[esp+0x28]` z + 2; second `[esp+0x18]` y + 10 and z - 2 (no push between store and reload).
  - `+0x76c = 1` (NF) only on the Z4a combo-arrow branch and on X4 / X4Up / X4Down.
  - Night Watcher: `+0xb04` 1 + `32c7bdd0` index (delayed) / 0.
  - X4Up / X4Down keys from `xorstrings.tsv`: `ARROW_DEGREE1/2/3_Z` into `degree.z`, and `ARROW_DEGREE2_Z` for the not-enough-MP angle (fourth argument). Every other not-enough-MP call is `( pos, 0, 0, 0 )`, except Z4a's `( pos, 0, 180, 0 )`.
  - The `+0x44c` life-time tail goes through `32c7fe20` behind a `GetMainEffect` NULL test.
- **Helpers:** buff `0x218`, `GetSkillLevel( 0x5aaa, 1, 0, 1 )`, `GetEquippedActiveSkillTemplet( 0x5aaa, 1 )`, ability `0x8f` / `0x1f`; level > 0 and a templet are both required.
  - `32c85330` subtracts NF x 0.1 (the double constant is `0.1f` widened) and clamps at 0, but only when `GetUnit()` is set. The port has the same nesting.
  - June's fourth `GetSkillAbilityValue` push (0.0) is the unused slot that March's three-argument form drops (funcmap S0a).
- **Enum:** `LESI_RCR_COMBO_XXZ` / `_FROM_X2` have no `Enum.lua` value (`ch0_state_enum_names.tsv`: registration ids 253 @1527 and 257 @1543 on the same table), so they stay sequential `_CORE` members for CH1m to map.
- **Kickoff fix:** `CH1d2.md`'s Done gate said phase `CH1d`; it now says `CH1d2`, and it notes that the two arrow helpers already exist for its dash-attack X / dash-jump X1 rows.

**Gates.**
- `flagcheck ..._LIVE --with ..._CORE --range 93c78d22^..93c78d22`: `files: 0 PASS, 1 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented `_LIVE` define.
- `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH): exit 0, 0 errors with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- No code changed, so there is no build; CH1d's `DEPLOY OK` (md5 `a255684a...`) stands.

Nothing to play-test. Next: `/x2-phase char-rena CH1d2` (Sonnet 5.5 high).

### CH1d2 - Done (2026-11-09)

All 36 class-map rows of phase CH1d2 are written, every one `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. No `new` rows. `_LIVE` stays `//#define`. 36 funcmap rows `Char Rena CH1d2`. Eight `_CORE` enum members appended after `LESI_RCR_COMBO_XXZ_FROM_X2` (below).

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c6ac10` `32c6ac40` `32c6ac70` `32c6e250` `32c6e290` `32c8b820` | `DoubleJumpAttackZ` StartFuture / `Landing` StartFuture / `Landing` Start / FrameMove / EventProcess / `Landing` EventProcess |
| `32c84490` `32c6b130` `32c72340` `32c6b140` `32c6b150` `32c72f30` | `DashComboZ1` StartFuture / FrameMove / EventProcess, `DashComboZ2` StartFuture / FrameMove / EventProcess |
| `32c6b160` `32c90130` `32c731b0` `32c6b170` | `DashAttackX` StartFuture / FrameMove / EventProcess / EndFuture |
| `32c84610` `32c6b200` `32c73c40` `32c6b210` `32c846a0` `32c90c70` `32c73cb0` | `JumpAttackZ` FrameMoveFuture / FrameMove / EventProcess, `JumpAttackX` StartFuture / FrameMoveFuture / FrameMove / EventProcess |
| `32c73ce0` `32c6b230` `32c6b260` `32c73f40` | `DashJumpAttackZ` EventProcess, `DashJumpAttackZLanding` StartFuture / Start / EventProcess |
| `32c74140` `32c90f10` `32c74180` `32c744e0` `32c91780` `32c74520` `32c6b2a0` `32c91f10` `32c747d0` | `DashJumpComboX1` FrameMoveFuture / FrameMove / EventProcess, `X2` the same three, `X3` FrameMoveFuture / FrameMove / EventProcess |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1d2`; everything not listed is March's logic written out whole, with `// June <va> ( = March )`):
- **EventProcess = March's `ADD_RENA_SYSTEM` revamp** (dead until CH1q) with the animation-end follow-up (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`) and the foot test on the cancels (`RENA_GROUND_*`), as CH1d. Flag / event-time pairs:
  - Dash Z1: 2 X (class 13 / 103) `LESI_RSR_DASH_ZX`, 1 X (24 / 114) `LESI_DASH_COMBO_RTR_ZX`, 0 Z `LESI_DASH_COMBO_Z2`; the direction clear covers flags 0-1 only. `m_fCanNotAttackTime` 0.2 on the animation end. Dash Z2: cancels only.
  - Dash attack X: 0 Z (114) `LESI_RNW_DASH_XZ`. Double-jump landing: 0 Z (12 / 102) `..._DOUBLE_JUMP_ATTACK_Z_UP`, 1 X (24 / 114) `LESI_RTR_DOUBLE_JUMP_ZX` (March had flag 0 only).
  - Dash-jump X1: 0 X `X2`; 1 Z (102) 261; 2 Z (103) 269; 3 Z (114) 277. X2: 0 X `X3`; 1 Z (102) 264; 2 Z (114) 280.
- **Dash attack X / jump attack X / dash-jump X1-X3 arrows**: MP through `GetArrowMpConsumption` (dash X 4, 9 for a Night Watcher; jump X 4; X1 4; X2 6; X3 8; March paid fixed costs) and the `GetMainEffect` max-life tail through `GetArrowMaxLifeTime`. Names: dash X `DamageEffect_Rena_Dash_Combo_X_Arrow` (Night Watcher keeps the explosion arrows), jump X `DamageEffect_Rena_Jump_Combo_X_Arrow`, X1 `_RSR_` for 13 / 103 else `_Rena_Dash_Jump_Combo_X_Arrow`, X2 `_XX_`, X3 `_XXX_` (`_RNW_` for 114). Night Watcher dash-jump arrows start at `Bip01` + 30 / + 20. Timers 0.3 / 0.21 / 0.23 / 0.2 / 0.233, all `EventTimerOneshot`. `ARROW_DEGREE_Z` read per arrow (X3: `degree.z`, `-20`, `-20`; not-enough-MP `-25`).
- **Natural force** (`_NF`, off): `m_NaturalForceType = NFT_COMBO_BOMB` on each of X3's three arrows. No other CH1d2 arrow has it (March had it on X3 only, under `ADD_RENA_SYSTEM`).
- **FrameMoveFuture**: dash-jump X3 is a bare thunk (March freezes the speed below 0.3 s); jump attack X zeroes `y` for the whole first 0.26 s, `x` from 0.13 s, and the walk key only works after 0.26 s (March froze both in 0.13 - 0.26 s). Both are June differences, not a `JUNE_BUGS` row.
- **Enum** (`_CORE`, June script ids, no `Enum.lua` value; CH1e / f / g own the states, CH1m maps the ids): `LESI_RWS_DASH_JUMP_XXZ_START` 261, `LESI_RWS_DASH_JUMP_XXZ_START_FROM_X2` 264 (same table registered twice, as `LESI_RCR_COMBO_XXZ_FROM_X2`), `LESI_RSR_DASH_ZX` 267, `LESI_RGA_DASH_JUMP_XXZ` 269, `LESI_RTR_DOUBLE_JUMP_ZX` 271, `LESI_RNW_DASH_XZ` 272, `LESI_RNW_DASH_JUMP_XZ_START` 277, `LESI_RNW_DASH_JUMP_XXZ_START` 280. The CH1e / f / g kickoffs say not to add them again.
- **Not here:** `DashJumpComboX3` Start `32c6b280` is CH1q's (March: `ADD_RENA_SYSTEM` only).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`flagcheck ..._CORE` on it: `RESULT: PASS`). `git diff --patience --numstat`: `.cpp` 716 / 0, `.h` 8 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P`: `EventTimerOneshot( 0.233f )` 0 lines off / 1 on; `DamageEffect_Rena_Dash_Jump_Combo_XXX_Arrow` 0 / 1 / 1; `NFT_COMBO_BOMB` 0 / 0 / 22 (March's three X3 lines leave with the `#else`, June's three come in).
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,078,720 B, 2026-11-09 05:10:07`; `DEPLOY OK: X2_offline.exe is 26,078,720 B, dated 2026-11-09 05:10:13, md5 d712257be85ebace3ce99eb63abeefde, equal to X2/US_SERVICE/x2.exe (2026-11-09 05:10:07); built at main 98d58ac4 + 2 uncommitted code file(s).` `_LIVE` is off and no string was added to a live path, so no `exestr` line.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1d2 review` (Opus 5.5 high), then CH1e.

### CH1d2 - Review (2026-11-09, Opus 5.5 high)

**Verdict: pass, no code change.** I read all 36 rows against `charport_adis.py rena --phase CH1d2`, each cut at its first `int3`. The x87 tests read the same way as in CH1d's review.
- **Follow-ups** (`stateChange < t || animation end`; window `t < keyInputEnd && keyInputStart <= t && cond`; event time `[i]` at `+0x8c10 + 12 i`):
  - Dash Z1: flag 2 / `[2]` (`+0x8c30`), class 13 or 103 X -> `0x10b` (267); then 1 / `[1]`, class 114 or 24 X -> `0xde` (`LESI_DASH_COMBO_RTR_ZX`); then 0 / `[0]` Z -> `0xba` (Z2). The direction clear is a word write at `+0x7d85` (flags 0-1). On the animation end: off a line, `JUMP_DOWN` and y - 45; on a line, `WAIT` and `+0xde0 = 0.2` (`m_fCanNotAttackTime`). Dash Z2 and dash attack X do not write `+0xde0`.
  - Dash attack X: 0 / `[0]`, class 114 Z -> `0x110` (272).
  - Double-jump landing: dash inputs first, then 0 / `[0]` class 12 or 102 Z -> `0x9d` (Z up), then 1 / `[1]` class 24 or 114 X -> `0x10f` (271), then walk, up, down (`CanDown`, y - 45) and the animation end -> `WAIT`.
  - Dash-jump X1: 0 X -> `0xc4`; 1 class 102 Z -> `0x105` (261); 2 class 103 Z -> `0x10d` (269); 3 class 114 Z -> `0x115` (277), event time `[3]` at `+0x8c34..3c`. X2: 0 X -> `0xc5`; 1 class 102 Z -> `0x108` (264); 2 class 114 Z -> `0x118` (280). X3: landing only.
  - The class tests come in the port's order: 114 before 24 in Dash Z1, 24 before 114 in the landing, 12 before 102.
- **Cancels** (Dash Z1, Dash Z2, dash attack X): skill `+0x8c90` (foot test, then the awakening arm), walk `+0x8c98`, back-walk `+0x8c94`, dash `+0x8c9c`, each with the foot test. No `+0x8c8c`.
- **Arrows:**
  - Timers 0.3 / 0.21 / 0.23 / 0.2 / 0.233, each an inlined `EventTimerOneshot` (max-time pre-test). MP through `32c85330` with 4, 9 if class 114 / 4 / 4 / 6 / 8.
  - Dash attack X: `Dummy1_Rhand`, y - 20, `angle.y` + 180; class 114 gets `ARROW_OF_EXPLOSION_DELAY` (`+0xb04 = 1`, then `+0xb00 =` `32c7bdd0`) or `ARROW_OF_EXPLOSION` (`+0xb04 = 0`); the others get `DamageEffect_Rena_Dash_Combo_X_Arrow`. Not enough MP: `( Dummy2_Lhand, 0, 180, 0 )`. Jump attack X: `DamageEffect_Rena_Jump_Combo_X_Arrow`, `( pos, 0, 0, 0 )`.
  - Dash-jump X1 / X2: `ARROW_DEGREE_Z` (`xorstrings.tsv`: `337ae4fc` / `337ae4ec` in `32c90f10`, `337ae520` / `337ae510` in `32c91780`) into `degree.z` and as the fourth not-enough-MP argument. Class 114 starts at `Bip01` + 30 / + 20 with the `_DOWN` explosion arrows. In X1, class 13 or 103 jumps to the `_RSR_` block (`32c9146f`), everyone else gets `_Rena_`. X2 has no RSR branch (`_XX_`).
  - X3: one `ARROW_DEGREE_Z` read, then three arrows at z, z - 20, z - 40 (`fsub 20` after arrows 1 and 2). The class 114 test is repeated per arrow (`_RNW_` / `_Rena_`); the port tests once, which behaves the same. `+0x76c = 1` (NF) on each. Not enough MP: `-25` as z.
  - Every arrow has the `GetMainEffect` NULL test, then the `+0x44c` tail through `32c7fe20`.
- **FrameMoveFuture:**
  - Jump attack X: `0.26 < t` lets the walk key work (walk speed triple x `m_fRunJumpSpeedRate`); otherwise `y = 0`, and `x = 0` once `0.13 <= t`.
  - Dash-jump X3 `32c6b2a0` is a bare vslot `0x7c8` thunk. X1 / X2 zero x and y while `t < 0.5` (= March). Jump attack Z (walk key, pure down without up: flag 0 and `y -= fGAccel * dt`) = March.
- **The other rows** = March: the StartFutures (`/ 1.5`, `x 0.5` under a `GetDashSpeed` test, the `m_JumpXCount` / 0.3 pair), the Start + `CreateStepDust` pair, `DamageCheck` -> `bHit` (`+0x80cc`), `EndFuture` (flip + 180), the landing EventProcess functions, and dash-jump Z landing (dash, walk, up and the hyper-mode else after `SKILL_CANCEL_AFTER( 0.01f )`).
- **Enum:** the eight `_CORE` members follow `LESI_RCR_COMBO_XXZ_FROM_X2`. `ch0_state_enum_names.tsv` has registration rows for 261 / 267 / 269 / 271 / 272 / 277 / 280 (`32cadc90`) and no `Enum.lua` value for any of them. 264 has no row because it is the second registration of 261's table, the same case as 257. CH1e adds `_LOOP` / `_END` (262 / 263) as `new` rows. `kickoffs/char-rena/CH1e.md` already says not to add 261 / 264 again, so it is unchanged.

**Gates.**
- `flagcheck ..._LIVE --with ..._CORE --range ccc7321f^..ccc7321f`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented `_LIVE` define.
- `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH): exit 0, 0 errors with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- No code changed, so there is no build; CH1d2's `DEPLOY OK` (md5 `d712257b...`) stands.

Nothing to play-test. Next: `/x2-phase char-rena CH1e` (Sonnet 5.5 high).

### CH1e - Done (2026-11-09)

All 29 class-map rows of phase CH1e are written. 22 are `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. 7 are `new` (`_CORE`, compiled in every build, referenced by nothing yet; one block after `LESI_LWS_DASH_XZZ_EventProcess`, declared beside `GetArrowMaxLifeTime` in the `.h`). `_LIVE` stays `//#define`. 29 funcmap rows `Char Rena CH1e`; 2 `JUNE_BUGS` rows (#563, #564). **One class-map correction:** `32c78e40` (June id 213) is March's `LESI_LGA_DASH_JUMP_Z_EventProcess`, not the Combat Ranger dash-jump Z1 function the map named (`ch0_class_map.tsv` row fixed).

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c6bf40` `32c6bfa0` `32c770a0` | `LESI_SA_LCR_LOW_KICK` Init / FrameMove / EventProcess (inside) |
| `32c6e2e0` `32c6e390` | Combat Ranger `DOUBLE_JUMP_ATTACK_Z` / `_Z_UP` EventProcess (inside) |
| `32c84510` `32c72850` `32c84590` `32c72bc0` | Combat Ranger `DASH_COMBO_Z1` StartFuture / EventProcess, `Z2` StartFuture / EventProcess (inside) |
| `32c73d10` `32c73e10` `32c73f10` `32c78e40` | Combat Ranger `DASH_JUMP_COMBO_Z1` / `Z2` / `Z3` EventProcess, `LESI_LGA_DASH_JUMP_Z` EventProcess (inside) |
| `32c6c120` `32c77370` `32c776d0` `32c77710` `32c77980` `32c779c0` `32c77c50` | Wind Sneaker `ZZfrontZ` FrameMoveFuture / EventProcess, `ZZfrontZZ` FrameMoveFuture / EventProcess, `ZZfrontZZZ` FrameMoveFuture / EventProcess, `ZZfrontZZZ_Landing` EventProcess (inside) |
| `32c782f0` `32c78610` | Wind Sneaker `DASH_XZ` / `DASH_XZZ` EventProcess (inside) |
| `32c7feb0` `32c80200` `32c80480` `32c807c0` | `LESI_RCR_COMBO_XXZ` / `XXZZ` / `XXZZ_LANDING` / `XXZZZ` EventProcess (new) |
| `32c80a40` `32c80c70` `32c80e80` | `LESI_RWS_DASH_JUMP_XXZ_START` / `_LOOP` / `_END` EventProcess (new) |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1e`; the rest is March's logic written out whole with `// June <va> ( = March )`):
- **EventProcess = March's `ADD_RENA_SYSTEM` revamp** (dead until CH1q) with the animation-end follow-up (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`) and the foot test on the cancels (`RENA_GROUND_*`), as CH1d / CH1d2. Chain: skill (`+0x8c90`), walk, back-walk, dash; no Z / X cancel. Direction clear = flag 0 only (`CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 )`).
  - Combat Ranger dash Z1 -> `LESI_COMBAT_RANGER_DASH_COMBO_Z2` (June id 185), dash Z2 -> `LESI_DASH_COMBO_Z2` (186); both set `m_fCanNotAttackTime` 0.2 on the animation end on a line, jump-down at `y - 45` off it. Dash-jump Z1 -> Z2 -> Z3, each with the landing test first. Double-jump attack Z: flag 0 from `oneZ` inside `( keyInputStart, keyInputEnd )` (March's `ADD_RENA_SYSTEM` branch), `LINE_RADIUS / 2` fall at the animation end off a line.
  - `LESI_LGA_DASH_JUMP_Z` (id 213, shares the Combat Ranger Z1 script table, as March does): X -> `LESI_LGA_DASH_JUMP_ZX`, change time 0.25 s or the animation end, window 100.0 with no lower bound.
  - **Wind Sneaker front combo.** `ZZfrontZ` (205) flag 0 -> `ZZfrontZZ` (206). `ZZfrontZZ` has no follow-up flag: its animation end goes to **id 208** (`ZZfrontZZZ_Landing`). 208's animation end -> wait, its flag 0 -> **id 207** (`ZZfrontZZZ`), whose animation end on a line -> wait (March had `ZZfrontZZZ` -> landing). The names are the script tables'; CH1m registers by id. FrameMoveFuture of 206 / 207: the `EventTimerOneshot( 0.3f )` / `( 0.54f )` is tested and the March speed line (`GetDashSpeed()` / `+ 400`) is gone. `DASH_XZ` has no `> 0.3 s` condition (March's non-NF branch had).
  - **Low Kick** Init preloads seven other assets (`JUNE_BUGS` #563); FrameMove / EventProcess = March (0.66 s skip).
- **The seven `new` functions.** `LESI_RCR_COMBO_XXZ_EventProcess` (ids 253 and 257 share it): off a line `JUMP_DOWN` at `y - 45`, animation end wait, flag 0 Z -> **id 258**; `XXZZ` (254 / 258): animation end -> id 259; `XXZZ_LANDING` (255 / 259): end -> wait / jump-down, flag 0 Z -> id 260; `XXZZZ` (256 / 260): like XXZ without the flag. `RWS_DASH_JUMP_XXZ_START` (261 / 264): animation end -> id 265, on a line -> id 266, then the chain with no landing test before it; `_LOOP` (262 / 265): on a line -> id 266; `_END` (263 / 266): end -> wait. The shared EventProcess only reaches the second registration (`JUNE_BUGS` #564).
- **Enum** (`_CORE`, `X2GULire_ElvenRanger.h`, June script ids, no `Enum.lua` value): `LESI_RCR_COMBO_XXZZ` 254, `_XXZZ_LANDING` 255, `_XXZZZ` 256, `_XXZZ_FROM_X2` 258, `_XXZZ_LANDING_FROM_X2` 259, `_XXZZZ_FROM_X2` 260, `LESI_RWS_DASH_JUMP_XXZ_LOOP` 262, `_END` 263, `_LOOP_FROM_X2` 265, `_END_FROM_X2` 266 (the `_FROM_X2` names follow CH1d's 257 / 264: the same script table registered a second time). CH1m maps the ids.
- **Chain macros' guard.** The `ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_` / `RENA_GROUND_*` block's `#ifdef` / `#endif` moved from `_LIVE` to `_CORE` (the two deleted flagcheck lines), because the `new` functions are compiled with `_LIVE` off and use them.
- **Not here:** no natural-force line in CH1e's rows (`ch0_nf.tsv` col 8 `CH1e` rows are March blocks that stay dead until CH1q); `InitState*` untouched.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `2 deleted line(s)` (the guard above), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`flagcheck ..._CORE` on it: `PASS +17`). `git diff --patience --numstat`: `.cpp` 471 / 2, `.h` 17 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P` off / on: `RCR_KickW1_01.X` 0 / 1; `CX2GULire_ElvenRanger::LESI_RCR_COMBO_XXZ_EventProcess()` 1 / 1 (compiled in every build); 1,457 `CX2GULire_ElvenRanger::` mentions both ways.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,082,816 B, 2026-11-09 07:08:25`; `DEPLOY OK: X2_offline.exe is 26,082,816 B, dated 2026-11-09 07:08:32, md5 2ac4c86b2d008815ec221cabd4f7ba8a, equal to X2/US_SERVICE/x2.exe (2026-11-09 07:08:25); built at main 0101c954 + 2 uncommitted code file(s).` `_LIVE` is off and no string was added to a live path, so no `exestr` line.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1e review` (Opus 5.5 high), then CH1f.

### CH1e - Review (2026-11-09, Opus 5.5 high)

**Verdict: pass, no code change.** I read all 29 rows against `charport_adis.py rena --phase CH1e`, each cut at its first `int3`. Registrations were checked in `ch0_reg.tsv` (`32cadc90`).
- **Follow-ups** (`stateChange < t || animation end`, window `keyInputStart <= t < keyInputEnd`, event time `[0]` at `+0x8c10..18`):
  - Combat Ranger dash Z1 -> `0xb9` (185), dash Z2 -> `0xba` (186); otherwise the two listings are identical. On the animation end: off a line `JUMP_DOWN` and y - 45; on a line `WAIT` and `+0xde0 = 0.2`. The direction clear touches flag 0 only.
  - Dash-jump Z1 -> `0xc0`, Z2 -> `0xc1`, each after the landing test (`0xc2`); Z3 is the landing test only.
  - `32c78e40` is registered at id 213 with table `LESI_COMBAT_RANGER_DASH_JUMP_COMBO_Z1`, as March registers `LESI_LGA_DASH_JUMP_Z`; that confirms the class-map fix. It goes to `0xd6` on `0.25 < t || end`, window `t < 100.0`, then `oneX`. The port's `0.f <=` lower bound is behaviour-equal because animation time is never negative.
  - Double-jump attack Z: on a line, flag 0 -> `0x9d`, else `0x9f`; the animation end off a line does y - 15 (`LINE_RADIUS / 2`). `oneZ` sets the flag inside `( keyInputStart, keyInputEnd )`, both strict.
  - Wind Sneaker: `ZZfrontZ` flag 0 -> `0xce`. `ZZfrontZZ`: off a line `JUMP_DOWN`, animation end -> `0xd0` (208), no flag. `0xd0` = `32c77c50` (table `ZZfrontZZZ_Landing`): end -> wait / jump-down y - 45, flag 0 -> `0xcf` (207). `0xcf` = `32c779c0`: `t < 0.516` off a line -> jump-down; end on a line -> wait. These ids match the registrations at 206 / 207 / 208.
  - `DASH_XZ`: end on a line -> `0x16`, flag 0 -> `0xd2`. `DASH_XZZ`: end -> wait on a line, else jump-down.
- **The seven `new` functions:** the follow-ups go to `0x102` / `0x103` / `0x104` (258 / 259 / 260) and `0x109` / `0x10a` (265 / 266), which are the second registrations. Ids 253 / 257 share `32c7feb0`, 254 / 258 `32c80200`, and so on (`ch0_reg.tsv`). X-X-Z, X-X-Z-Z and X-X-Z-Z-Z test the foot first (jump-down, y - 45). START tests `IsAnimationEnd` before the foot (`JUNE_BUGS` #564). In START and LOOP the compiler drops the skill arm of the cancel because the foot test already failed on that path, and the port's `SKILL_CANCEL_AFTER` (which tests `IsOnSomethingFuture()`) behaves the same.
- **Cancels:** every listing that has a chain has skill `+0x8c90` (foot test, then the awakening arm), walk `+0x8c98`, back-walk `+0x8c94` and dash `+0x8c9c`, each with the foot test, and no Z / X cancel. In the `ZZfrontZZ` / `ZZfrontZZZ` FrameMoveFuture, the inlined `EventTimerOneshot( 0.3f )` / `( 0.54f )` is evaluated and its result dropped.
- **Other rows = March:** Low Kick Init has June's seven preloads in order (`JUNE_BUGS` #563); the FrameMove thunk; EventProcess skips at 0.66 s; Z_UP skips at 0.65 s; `ZZfrontZ` FrameMoveFuture is a thunk; both StartFutures use `nowSpeed.x > GetDashSpeed()` and then `x 0.5`.
- **Enum / guard:** the ten `_CORE` members sit between 253 / 257 / 261 / 264 in June-id order. The two lines flagcheck counts as deleted are the chain macros' `#ifdef` / `#endif` moving from `_LIVE` to `_CORE`. `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` is still guarded by `_LIVE` (CH1f amended).

**Gates.**
- `flagcheck ..._LIVE --with ..._CORE --range 844e19e9^..844e19e9`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. OTHER is the `.h` (`_CORE`).
- `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH): exit 0 and 0 errors with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- No code changed, so there is no build; CH1e's `DEPLOY OK` (md5 `2ac4c86b...`) stands.

Nothing to play-test. Next: `/x2-phase char-rena CH1f` (Sonnet 5.5 high).

### CH1f - Done (2026-11-09)

All 43 class-map rows of phase CH1f are written. 39 are `inside`: June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. 4 are `new` (`_CORE`, compiled in every build, referenced by nothing yet): the `LESI_RSR_DASH_ZX` StateStart / StateStartFuture / FrameMove / EventProcess, one block before `LESI_LGA_ZZX_FrameMove`. `_LIVE` stays `//#define`. 43 funcmap rows `Char Rena CH1f`; 2 `JUNE_BUGS` rows (#565, #566). **Two class-map corrections** (`ch0_class_map.tsv` rows fixed): `32c77f90` is the Wind Sneaker's dash X (March `LESI_LWS_DASH_X_EventProcess`, flag 1 -> `LESI_LWS_DASH_XZ`), not a second Sniping Ranger X1; `32c7bd20` is March's `SetSiegeModeCoolTime` (June looks the siege skill up by its own id), not a new helper.

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c6af50` `32c6af60` `32c87d30` | Siege wait StateStart / FrameMove / EventProcess |
| `32c87c20` `32c6f560` `32c83d00` `32c83f70` `32c84020` `32c840d0` | Siege ready StartFuture / Start / EventProcess, attack (Z) and attack X EventProcess, end EventProcess |
| `32c6afb0` `32c6aff0` `32ca5330` `32c6b030` `32c6b070` `32ca5aa0` | Siege attack Z and X StartFuture / Start / FrameMove |
| `32c7bd20` | `SetSiegeModeCoolTime` |
| `32c99a90` `32c7b1b0` `32c7b530` `32c99ec0` `32c7b570` `32c9a300` `32c7ba40` | Sniping Ranger combo X2 FrameMove / EventProcess, X3 FrameMoveFuture / FrameMove / EventProcess, X4 FrameMove / EventProcess |
| `32c6b1b0` `32c90620` `32c73500` `32c6b1c0` `32c77f90` `32c909e0` `32c73960` | dash X1 StartFuture / FrameMove / EventProcess / EndFuture, Wind Sneaker dash X EventProcess, dash X2 FrameMove / EventProcess |
| `32c74800` `32c928f0` `32c74840` `32c6b2b0` `32c92df0` `32c74a10` | dash-jump X2 FrameMoveFuture / FrameMove / EventProcess, X3 FrameMoveFuture / FrameMove / EventProcess |
| `32c83c90` `32c8aa40` `32c6e450` | double-jump attack X FrameMoveFuture / FrameMove / EventProcess |
| `32c6d500` `32c6d520` `32c9faa0` `32c810e0` | `LESI_RSR_DASH_ZX_StateStart` / `_StateStartFuture` / `_FrameMove` / `_EventProcess` (new) |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1f`; the rest is March's logic written out whole with `// June <va> ( = March )`):
- **Every arrow costs MP through `GetArrowMpConsumption` and has its own name** (Sniping Ranger `DamageEffect_RSR_*`, Wind Sneaker `_RWS_` in dash X). March live -> June MP: combo X2 1.5 -> 6, X3 3 -> 8, X4 3 -> 9, dash X1 6 -> 4, dash X2 3 -> 6, dash-jump X2 3 -> 6, dash-jump X3 12 -> 8, double-jump X 20 -> 4. Combo X4, dash-jump X3 and `RSR_DASH_ZX` arrows set the natural force combo bomb type (`_NF`). `RSR_DASH_ZX` shoots three arrows from `Dummy2_Lhand` (z, z + 15, z - 15 of `ARROW_DEGREE_Z`).
- **Siege Mode.** The skill is `RNSI_A_RENA_SIEGEMODE` (23201), the enhanced one `_UPGRADE` (23221), looked up with the upgrade swap in Ready's Start / StartFuture and `SetSiegeModeCoolTime` ( `JUNE_BUGS` #565 ). A shot ends the mode below 15 MP (Z, March 20) / 25 MP (X, March 27). The Z shot aims along `m_vSpineAimDegree` (`UpdateSpineAimRotation`, vslot 0x774; wait and Z call it), both arrows ( now + 0.12 s reserved ) go through one vector; X keeps the parabolic volley. A natural force point (`_NF`) is consumed first and swaps the arrow for `ARROW_SIEGE_Z_NATURE_FORCE` / `ARROW_SIEGE_X_NATURE_FORCE`; the weak shot ( 3 MP ) is `ARROW_SIEGE_No_MP` / `ARROW_SIEGE_ARROW_NATURE_FORCE` ( March `ARROW_LONG` ). Siege wait runs `SpecialAttackEventProcess( ASUT_GROUND )`.
- **EventProcess = March's `ADD_RENA_SYSTEM` revamp** (dead until CH1q) with the animation-end follow-up (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`) and foot-tested cancels (`RENA_GROUND_*`), as CH1d-CH1e. Order per listing: combo X2 skill / walk / back-walk / dash, X3 walk / dash / skill / back-walk (3 flags), X4 plain walk / dash, skill, foot-tested back-walk, **Z / X cancel -> wait** (March: siege wait), dash X1 skill / walk / back-walk / dash with `SpecialAttackEventProcess( ASUT_GROUND )` after the animation end, dash X2 skill / walk / dash / back-walk, Z / X -> siege wait. Dash-jump X2: flag 1 ( `oneZ`, Grand Archer ) -> id 270 ( new `_FROM_X2` member ).
- **Double-jump X EventProcess** opens the chain with the flagged X-X follow-up ( `IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`, new `_CORE` macro ); `FrameMoveFuture` of dash-jump X3 is a bare thunk ( the speed freeze is gone ).
- **Added to the headers.** `X2SkillTree.h`: `RNSI_A_RENA_SIEGEMODE` 23201, `RNSI_A_RENA_SIEGEMODE_UPGRADE` 23221 (`JUNE_ENUM_NAMES`). `X2GULire_ElvenRanger.h` (`_CORE`): `LESI_RGA_DASH_JUMP_XXZ_FROM_X2` 270, `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX` 252 (only while `ADD_RENA_SYSTEM` is off), the four `RSR_DASH_ZX` declarations, `UpdateSpineAimRotation()` ( June `32c863d0`, **body in CH1i** ) and `m_vSpineAimDegree` ( +0x97d4 ). `.cpp`: `COMBO_HIT_GROUP_VALUE` ( `_CONST_RENA_CH1F_`, the 100 `SetNowDamageRelateSkillData` takes ), the `IF_` macro.
- **Not here:** `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX` Start / FrameMove / EventProcess are CH1q's; `InitState*` untouched; nothing the plan routes elsewhere was found.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`_CORE`). `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `PASS~ +6`, `RESULT: PASS`.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P` off / on: `DamageEffect_RSR_Combo_XX_Arrow` 0 / 1 ( the `_CORE` `LESI_RSR_DASH_ZX_EventProcess` is in both ).
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,085,376 B, 2026-11-09 07:58:08`; `DEPLOY OK: X2_offline.exe is 26,085,376 B, dated 2026-11-09 07:58:14, md5 34b2c319209e55490c6bfb086d15f32d, equal to X2/US_SERVICE/x2.exe (2026-11-09 07:58:08); built at main 908ef52c + 3 uncommitted code file(s).`
- `exestr` ( the `_CORE` string is compiled in, the `_LIVE`-only one is not ): `EXESTR OK: 1 of 1 found in X2_offline.exe ... md5 34b2c319...` for `DamageEffect_RSR_Dash_Combo_ZX_Arrow` ( wide ); `EXESTR OK: 1 of 1 absent` for `DamageEffect_RSR_Combo_XX_Arrow`.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1f review` (Opus 5.5 high), then CH1f2.

### CH1f - Review (2026-11-09, Opus 5.5 high)

**Verdict: PASS with one fix.** All 43 rows were read against `charport_adis.py rena --phase CH1f`, each listing stopped at its first `int3` (`32c87c20` at `32c87d30`, `32c928f0` at its X3 tail). Float tests were read by their x87 flag tests, as in CH1d-CH1e.
- **Fix: combo X4 EventProcess `32c7ba40`.** The Z / X cancel (`+0x8c8c`, `pureZ` / `pureX`) pushes `0xa8` = `LESI_SIEGE_WAIT` (168, `ch0_state_enum_names.tsv`), as March does, and so does the animation end. CH1f had written `USI_WAIT`: the annotator's `pushes=0xd(LESI_WAIT),0(),0xa8` line carries a stale `0xd`, and the CH1f kickoff repeated it. The fix changes the line and its comment in place (bytepatch, 2 lines, LF). The funcmap row is corrected too. There is no `JUNE_BUGS` row because this is a transcription error, not a June quirk.
- **Class-map corrections confirmed.** `32c77f90`: back input clears flags 0-1 (word store); animation end first (no foot: jump down, y - 45; foot: wait); flag 1 only, `0x8c24 < t` or the end -> `0xd1` (`LESI_LWS_DASH_XZ`, 209), set in `[0x8c1c, 0x8c20)` on `oneZ`, no class test; then skill (foot, `SpecialAttackEventProcess( 1 )`), hyper, walk / back-walk / dash with foot tests. `32c7bd20` = `SetSiegeModeCoolTime`: `0x5aa1` then `0x5ab5` (`GetEquippedActiveSkillTemplet( id, 1 )`), `max( 1, GetSkillLevel( m_eID, 0, 0, 1 ) )`, vslot 0x6a4 (= March's 2-argument `GetActualCoolTime`; June's extra 0 / 1 are the unused bools), `SetSkillCoolTimeLeft( 0x5aa1 )`.
- **Siege Mode.** `+0x95ac` (protected) = `m_fMPConsumeRateSiegeArrow`, `+0x95b8` = `m_fDamageRelSiegeArrow`, `+0x95a8` / `95bc` / `95c0` / `95c4` / `95cc` / `9680` = range / power / hit count / hit gap / hit-add MP / mesh index (March's member order). The listing ends the mode when `GetNowMp() < 15 * rate` / `25 * rate`; cool time on END and on the fall.
  Z `32ca5330`: vslot 0x774 first, then the one-shot 0.1. The NF count is read at `m_pGageData +0x114`, followed by `ConsumeNaturalForce( 1 )` and `FlushMp( 15 * rate )`. Two `CreateInstance` calls are pushed into one vector, reserve 0 / -1 then 1 / 0.12, with angle = move axis = `+0x97d4` and power `GetPowerRate() * +0x95b8`. The loop then adds `+0x9614` to `backSpeed.x` and stores `+0x9618` / `961c`. The weak shot is `FlushMp( 3 )` with `1.54 * +0x95b8` and hit-add MP `+0x95cc`; the strong shots pass -1.
  X `32ca5aa0`: `DoAimingTarget( 1000, ( tan 60, -tan 30, pi / 4 ), ..., 1 )` with the result dropped (`fstp st0`). The volley is `!aim || dist <= 400` -> dir (negated when facing left) * 700, y + 300, then 5 arrows. Each arrow uses `rand( i ) % 100 * 4 - 200`, flight = dist / 3000 + `rand( i + 1 ) % 100 % 5 / 10`, gravity -1100 and 10. The weak shot is `1.154` with `GetRotateDegree()`. NF names: `ARROW_SIEGE_Z_NATURE_FORCE`, `ARROW_SIEGE_X_NATURE_FORCE`, `ARROW_SIEGE_ARROW_NATURE_FORCE` (weak), plain `SIEGE_ARROW` / `ARROW_SIEGE_X` / `ARROW_SIEGE_No_MP`, all as listing strings.
  Ready `32c87c20` / `32c6f560`: the same lookup order, `GetSkillLevel( id, 0, 0, 1 )` on the found id, `1 / SA_READY_TIME_REL (0x26)`, `ResetAnimSpeed( +0x8900, 1, 1 )` / `( +0x88fc, 1, 0 )`. Start zeroes `+0x9758` before `CommonStateStart` and sets `m_bIsSiegeSkill` unconditionally. Wait `32c87d30`: MP < 4 or count >= 5 -> END + cool time; Z / X -> 169 / 170 + count; a pure direction -> cool time then WAIT; `SpecialAttackEventProcess( 1 )` true -> cool time. Wait FrameMove `UpNowMp( -2 * elapsed )`, vslot 0x774, `CommonFrameMove`. Attack Start / StartFuture: memo `0x9337f`, `1.305` on `+0x8900` (the future speed in both = March).
- **EventProcess chains** (order and foot tests per listing; register-cached foot tests counted). X2 `32c7b1b0`: flag 0 -> 180, skill / walk / back-walk / dash. X3 `32c7b570`: flags 1 / 2 / 0 -> 182 / 183 / 181, then walk / dash / skill / back-walk. X4: plain walk and dash, skill, foot-tested back-walk, Z / X (fixed). Dash X1 `32c73500`: animation end then foot, `SpecialAttackEventProcess( 1 )`, flag 0 -> 188, flag 1 `oneZ` + class 102 -> 209, skill / walk / back-walk / dash. Dash X2 `32c73960`: end -> 168, skill / walk / dash / back-walk, Z / X -> 168. Dash-jump X2 `32c74840`: foot -> 22, flag 0 -> 198, flag 1 class 103 + `oneZ` -> 270. `RSR_DASH_ZX` `32c810e0`: end and foot -> 18, then the four foot-tested cancels. Double-jump X `32c6e450`: flagged X-X -> 252, foot -> 159, end in the air -> y - 15. Siege ready / end: = March.
- **Arrows.** All nine FrameMoves = the Done note's MP table, timers and names. Each arrow passes `+0x95cc` hit-add MP. Dash X1's switch is 13 / 103 RSR, 102 RWS, anything else nothing, after the MP flush. Double-jump X uses `-90 / -70 / -110` with the RSR name for 13 / 103. `[effect + 0x76c] = 1` appears only in X4, dash-jump X3 (x3) and `RSR_DASH_ZX` (x3). `32c9faa0`: one-shot 0.53, `Dummy2_Lhand`, y + 180, z / z + 15 / z - 15. `32c6d500`: `SetNowDamageRelateSkillData( 100 )`, which the C++ keeps under `_NF` (March declares it only with `ADD_RENA_SYSTEM`).
- **Enum / header.** 270 and 252 (`#ifndef ADD_RENA_SYSTEM`, March's own member otherwise) = `ch0_state_enum_names.tsv`; `RNSI_A_RENA_SIEGEMODE` 23201 / `_UPGRADE` 23221 = `Enum.src.lua` l.15146 / 15659.

**Gates.** `flagcheck ..._LIVE --with ..._CORE --range d72e9ad7^..d72e9ad7`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`; `RESULT: FAIL` only for the commented `_LIVE` define. The fix: `--worktree --files X2Lib/X2GULire_ElvenRanger.cpp`: `1 PASS`, `+2 -2`. Its 2 deleted lines are the replaced June-body line and comment inside `_LIVE`; March's `#else` is untouched.
`flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `PASS~ +6`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: 0 A/B rows.
One-file `/Zs` probe (`census.cl_cmd`, no PCH), after the fix: exit 0 and 0 errors in three runs: with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
`DEPLOY OK: X2_offline.exe is 26,085,376 B, dated 2026-11-09 08:17:22, md5 9fab305458cfd6de9eb4a5aa771ac568, equal to X2/US_SERVICE/x2.exe (2026-11-09 08:17:16); built at main d72e9ad7 + 1 uncommitted code file(s).`

CH1f2 kickoff checked. Its done gate said phase `CH1f` and now says `CH1f2`; its amendment lists the CH1f members it may use. Nothing to play-test (`_LIVE` off). Next: `/x2-phase char-rena CH1f2` (Sonnet 5.5 high).

### CH1f2 - Done (2026-11-09)

All 37 class-map rows of phase CH1f2 are written: 8 `inside` (June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`) and 29 `new` (`_CORE`, compiled in every build, referenced by nothing yet). `_LIVE` stays `//#define`; `InitState*` untouched. 37 funcmap rows `Char Rena CH1f2`; 2 `JUNE_BUGS` rows (#567, #568). **One class-map correction** (`ch0_class_map.tsv` fixed): `32c86890` is March's `ShowActiveSkillCutInAndLight` (now `inside`), not a new helper; `32ca7750` / `32ca9740` got their names.

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c958e0` `32c78880` / `32c95b20` `32c78be0` | Grand Archer Z-X / Z-X-X FrameMove / EventProcess (inside) |
| `32c78f10` `32c96320` `32c78f40` | dash-jump Z-X FrameMoveFuture / FrameMove / EventProcess (inside) |
| `32c6d550` `32ca02a0` `32c81340` | `LESI_RGA_DASH_JUMP_ZXX` StateStartFuture / FrameMove / EventProcess (new) |
| `32c815a0` `32ca0a90` `32c815d0` | `LESI_RGA_DASH_JUMP_XXZ` FrameMoveFuture / FrameMove / EventProcess (new) |
| `32c6d5f0` `32ca9c20` `32ca0f80` `32ca1240` `32c6d680` | Gungnir ready Init / StateStart / FrameMove / EventProcess / StateEnd (new) |
| `32ca1300` `32c82d10` | Gungnir cancel StateStart / EventProcess (new) |
| `32ca1a00` `32ca1360` | control idle StateStart / the EventProcess the nine control states share (new) |
| `32ca1bb0` `32ca9c50` `32c82f90` `32ca1cc0` | control fire StateStart / FrameMove / EventProcess / StateEnd (new) |
| `32ca1d10` `32ca1d70` `32ca1dd0` `32ca1e30` `32ca1e90` `32ca1ef0` `32ca1f50` `32ca1fb0` | the eight direction StateStarts (new) |
| `32ca9740` `32ca7750` | `PlayGungnirAttackEffect`, `EndGungnirTargeting` (new) |
| `32c86890` | `ShowActiveSkillCutInAndLight` (inside) |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1f2`; 32c81340 = 32c78be0 and the eight direction starts checked by a normalised instruction diff):
- **Grand Archer arrows and MP.** Z-X 3 -> 2 MP (through `GetArrowMpConsumption`, the Grand Archer's own `EffectSet_RGA_ZZX`), Z-X-X 18 -> 9 (three `DamageEffect_RGA_Combo_ZZXX_Arrow`, combo bomb under `_NF`), dash-jump Z-X 3 -> 2 ( `FlushMp` direct, `EffectSet_RGA_DASH_JUMP_ZX` ). New states: Z-X-X after dash-jump Z-X-X (id 268, 8 MP at 0.25 s, three `..._Dash_Jump_Combo_ZXX_Arrow` from `Dummy2_Lhand`, combo bomb) and X-X-Z (269 / 270, 3 MP at 0.492 s, `EffectSet_RGA_DASH_JUMP_XXZ`).
- **EventProcess.** Z-X: X follow-up also at the animation end, the animation end tests the foot first (jump down y - 45 / siege wait), foot-tested walk / back-walk / dash. Dash-jump Z-X, X-X-Z: landing turns the unit round (`bIsRight` flipped, `dirDegree.y + 180`) and the follow-up (X resp. Z) goes to 268, also at the animation end. Z-X-X EventProcess = Z-X-X's landing + cancels.
- **Gungnir (the shape of Chung's Artillery Strike; `NUI_RENA_TARGETING_GUNGNIR` 2083, scope buff `BTI` 537 / `BFI` 627).** Ready FrameMove creates the targeting NPC at 0.001 s ( 120 ahead, 300 up, 240 above the unit when `CanDown` finds no line ), then the scope buff; Ready EventProcess hands the camera to the NPC from 0.6 s; the control states send the NPC `SHOT_GUNGNIR_<DIR>`; the fire state keeps the NPC's spot and power rate and plays the strike effect set (`..._Gungnir_Attack`, `_Hyper` while awake) 4000 above it at 0.04 s, then `EndGungnirTargeting`.
- **Quirks kept (`JUNE_BUGS` #567, #568).** The control EventProcess compares both distances with `-750` (never true: the clamp is dead and a release always fires); its horizontal gap is an uninitialised stack float on the live path (stand-in 0: both sides may move). The 1500 test on the NPC's y stays.
- **Stand-ins (one line each in the code).** `CreateNPCReq` lacks June's trailing `IsMyUnit` argument (as Chung); June's boss-raid test `32481fa0` is false here, its effect branch is not written; `ShowActiveSkillCutInAndLight` leaves out the cut-in item override (`31fd8a50`) and the event 0x8b3 pair (CH5).
- **Added to the headers.** `X2GULire_ElvenRanger.h` (`_CORE`): 12 enum members (`LESI_RGA_DASH_JUMP_ZXX` 268, `LESI_RGA_GUNGNIR_CANCEL` 284 ... `_CONTROL_DOWN_RIGHT` 294), 29 declarations, six Gungnir members (June +0x9798 .. +0x97c0). `X2BuffFactor.h` (`JUNE_ENUM_NAMES`): `BTI_BUFF_GUNGNIR_SCOPE` 537.
- **Routed, no new plan.** The constructor's initialisers for the new members -> `CH1h.md`; `CommonFrameMove` calls `EndGungnirTargeting` -> `CH1j.md`; the cut-in pair -> `CH5.md`. The READY state's own enum member has no June id in `ch0_state_enum_names.tsv`: CH1m / CH2c.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`_CORE`). `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES --files X2Lib/X2BuffFactor.h`: `1 PASS`, `RESULT: PASS`.
- One-file `/Zs` probe (`census.cl_cmd`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h X2Lib/X2BuffFactor.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,095,616 B, 2026-11-09 09:03:36`; `DEPLOY OK: X2_offline.exe is 26,095,616 B, dated 2026-11-09 09:03:42, md5 24f50a02177102b27d4a2067d51dcb71, equal to X2/US_SERVICE/x2.exe (2026-11-09 09:03:36); built at main 964f0268 + 3 uncommitted code file(s).`
- `exestr` (a `_CORE` string is compiled in, a `_LIVE`-only one is not): `EXESTR OK: 1 of 1 found in X2_offline.exe ... md5 24f50a02...` for `EffectSet_SA_RGA_Gungnir_Loop` (wide); `EXESTR OK: 1 of 1 absent` for `DamageEffect_RGA_Combo_ZZXX_Arrow`.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1f2 review` (Opus 5.5 high), then CH1g.

### CH1f2 - Review (2026-11-09, Opus 5.5 high)

**Verdict: PASS, no behaviour change; one `JUNE_BUGS` row withdrawn, two comments corrected.** The 37 rows were read against `charport_adis.py rena --phase CH1f2`, each listing stopped at its `ret` or the next class-map start. Float tests were read by their x87 flag tests.
- **Withdrawn: `JUNE_BUGS` #568.** The control EventProcess `32ca1360` stores `fldz` to `[esp+0x28]` / `[esp+0x2c]` at `@32ca13bd` / `@32ca13c6`, right after `push 0x33722a70` (esp - 4). Once that push is popped, those slots are the later `[esp+0x24]` / `[esp+0x28]`: both gaps start at 0. The frame confirms it: `[esp+0x10]` is the 8-aligned double temp only if both callees pop their arguments. The port's 0 is June's own value. The comment at the gap locals is corrected (bytepatch, 1 line, LF), and so are the row and the funcmap row.
- **Comment fix: `LESI_LGA_DASH_JUMP_ZX_EventProcess`.** Its June comment said "Z follow-up". The code tests `oneX` (`@32c7903c`, `+0x86ef`) and is right; the comment now says X (1 line, LF).
- **Gungnir control `32ca1360`.** Both `-750` tests are dead: `@32ca150d` (`test ah, 0x41` / `jp`, so the clamp `32ca151d`-`32ca1561` runs only when the distance is -750 or less) and `@32ca19bb` (`test ah, 1` / `je`, so the release always goes to 286 FIRE). The clamp is gap X = (z diff) + (x diff), gap Y = y diff, with unit and NPC positions multiplied componentwise by the unit's dir vector (both paths). The 1500 test applies when unit y + 1500 <= NPC y and leaves gap Y = 1500.
  The picker order and gates match the listing: 291 UP_LEFT (gX >= 0 and gY <= 0, or no NPC), 292 UP_RIGHT (gX <= 0, gY <= 0), 293 DOWN_LEFT (>= 0, >= 0), 294 DOWN_RIGHT (<= 0, >= 0), 287 LEFT (gX >= 0), 288 RIGHT (gX <= 0), 289 UP (gY <= 0), 290 DOWN (gY >= 0), then 285 IDLE. Each tests `GetNowStateID()` first. The skill id is `+0x7cb0` -> `+0xc` = `GetFutureStateSkillID()` (funcmap `3208e410`). With no NPC on release: 286.
- **Gungnir states.** READY FrameMove `32ca0f80`: cut-in `( Dummy1_Rhand, 0.06, 0, 0 )`; one-shot 0.001; +-120 along the dir, then y + 300; `CanDown( pos, 30, &out, &line, 0, 1, 0, 1, 0 )` with the out position unused; on a miss, `GetPos()` + 240 on y. `CreateNPCReq( 0x823, 0, 1, pos, bIsRight, 0.3, 1, -1, team, 1, UID, 0, -1, 0, IsMyUnit, 0 )` runs behind `IsMyUnit`, then BFI 627 is pushed `skip 0 | adjust 0 | level 1 | creator 0`.
  READY EventProcess `32ca1240`: 0.6 s, then NPC `+0x8750` / `+0x8d34` = 1, `+0x8e39` = 0 with no `IsMyUnit`, then 285; otherwise `10 < t` or `!IsEventGamePlayable( 0 )` -> wait. READY Start `32ca9c20`: BTI 537, then the strike.
  Idle Start `32ca1a00`: NPC `SHOT_GUNGNIR_IDLE` (`0x33518350`); the loop effect plays only while the handle is -1, all defaults. FIRE Start `32ca1bb0`: power rate, then NPC `..._FIRE` (`0x33518364`), and the spot, dir, pending flag and UID -1 stay inside the NPC test, then the loop effect stops. FIRE FrameMove: one-shot 0.04 && pending. FIRE StateEnd: `IsMyUnit` -> `InitFocusUnit`. FIRE EventProcess = CANCEL EventProcess `32c82d10` minus the foot test and the 0.1 s attack block (normalised diff). CANCEL Start: `..._CANCEL` (`0x33518300`). The eight direction Starts are identical apart from the name.
  `PlayGungnirAttackEffect` `32ca9740`: target spot, the line-map vertical cross `( pos, false )`.y, then + 4000; the name is `_Hyper` while gage `+0xe8` > 0; `PlayEffectSet( name, this, NULL, false, +0x9798, -1, (1,1,1), true, &pos, rot, rot, 1 )`; then `EndGungnirTargeting`. `EndGungnirTargeting` `32ca7750`: power 0, pending 0, erase BTI 537, `KillMySummonedNPC( 0x823, 0 )`, stop the loop effect if it is not -1, NPC `..._FIRE`, UID -1.
- **LGA / RGA line.** `32c78f40`: the landing flips `bIsRight` and `+0x7e08` + 180 -> 18; latched flag 0 -> 268 at `+0x8c18` or the animation end; latched on `oneX` in `[0x8c10, 0x8c14)`. `32c815d0`: the same with `oneZ`. `32c6d550`: the same flip before `CommonStateStartFuture`. `32c81340` = `32c78be0`: the normalised diff is empty.
  MP and timers: Z-X 2 at 0.3 (`GetArrowMpConsumption`, `EffectSet_RGA_ZZX`); Z-X-X 9 at 0.53 (`Dummy1_Rhand`, +15 / -30); dash-jump Z-X 2 at 0.492 (direct); ZXX 8 at 0.25 (`Dummy2_Lhand`, `..._Dash_Jump_Combo_ZXX_Arrow`); XXZ 3 at 0.492. Not-enough-MP arguments are `( pos, 0, 180, z )` and `( pos, 0, 0, -90 )`.
- **Constructor (CH1h).** `32ca2b60` writes `+0x97c0` = edi (-1, from `@32ca3323`) at `@32ca3c0d` and `@32ca4210`, and `+0x97b8` = -1 at `@32ca4183`. It also writes `+0x9798`, the two vectors and `+0x97bc` = 0. `CH1h.md` carries all of them.
- **Noted, not changed.** CH1f2's `_CORE` block (`.cpp` 21592-22159) sits inside CH1f's own `_CORE` block (21352-22160). This is harmless; `CH1g.md` now says to open a fresh block after it.

**Gates.** `flagcheck ..._LIVE --with ..._CORE --range 3fe72bbc^..3fe72bbc`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. OTHER = `.h` (`_CORE`) and `X2BuffFactor.h` (`JUNE_ENUM_NAMES`: `--files` range `1 PASS`, `RESULT: PASS`).
The fix (`--worktree --files X2Lib/X2GULire_ElvenRanger.cpp`): `1 PASS`, `+2 -2`. The 2 deleted lines are the replaced comments inside the flagged blocks.
`junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`; `--check` on the fixed file: `RESULT: PASS`. `enumscan.py --check` on the class `.cpp` / `.h`: 0 A/B rows. One-file `/Zs` probe (`census.cl_cmd`, no PCH): exit 0 and 0 errors with no extra defines, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
`DEPLOY OK: X2_offline.exe is 26,095,616 B, dated 2026-11-09 09:13:41, md5 ecd83495ce44a9fcccab29e4883a68e3, equal to X2/US_SERVICE/x2.exe (2026-11-09 09:13:34); built at main 3fe72bbc + 1 uncommitted code file(s).`

CH1g kickoff checked: its gate names `CH1g`, and it depends on this review. Nothing to play-test (`_LIVE` off). Next: `/x2-phase char-rena CH1g` (Sonnet 5.5 high).

### CH1g - Done (2026-11-09)

All 46 class-map rows of phase CH1g are written: 42 `inside` (June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`) and 4 `new` (`LESI_RTR_DOUBLE_JUMP_ZX_*`, `_CORE`, compiled in every build, referenced by nothing yet). `_LIVE` stays `//#define`; `InitState*` untouched; the state enum member (271) was CH1d2's. 46 funcmap rows `Char Rena CH1g`; no `JUNE_BUGS` row (nothing June does is a bug; the one number that differs from March is below). **Class-map correction** (`ch0_class_map.tsv` fixed, 6 rows): CH0 listed `32ca6410`, `32c96e50`, `32ca69e0`, `32ca6b10`, `32ca6be0`, `32c9a5b0` as `new ?` helpers; they are March's `CreateTrapBlade`, `DeleteTrapBlade`, `SetPriorPlannedBlow`, `EndPriorPlannedBlow`, `CheckPriorPlannedBlow`, `InsertPriorPlannedBlowEffect` (now `inside`; the Night Watcher rows of CH1g2 / CH1g3 call them by name). `ch1_offsets.tsv` gains 12 members (`+0x968c` blade handle ... `+0x96f4`).

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32ca6640` `32ca9620` `32c79470` `32c96eb0` | `LESI_COMBO_RTR_ZZZ` Start / FrameMove / EventProcess (differs) / StateEnd |
| `32ca6670` `32c6c560` `32c79950` `32c96f10` | `..._ZZZ_DZ` Start / FrameMove / EventProcess (differs) / StateEnd |
| `32ca6690` `32ca9640` `32c79be0` `32c96f70` | `..._ZZZZ` Start / FrameMove / EventProcess (differs) / StateEnd |
| `32ca66c0` `32cad240` `32c79f50` `32c96fd0` | `..._ZZZZZ` Start / FrameMove (differs) / EventProcess (differs) / StateEnd |
| `32ca6700` `32ca9660` `32c84d90` `32c7a220` `32c97030` | `LESI_JUMP_ATTACK_RTR_Z` Start / FrameMove / FrameMoveFuture / EventProcess / StateEnd |
| `32ca6730` `32ca9680` `32c7a290` `32c97090` `32c6c570` | `LESI_DASH_COMBO_RTR_ZX` Start / FrameMove / EventProcess (differs) / StateEnd / StateEndFuture |
| `32ca6780` `32ca9700` `32c7a510` `32c970f0` | `LESI_DASH_JUMP_ATTACK_RTR_Z` Start / FrameMove / EventProcess (differs) / StateEnd |
| `32ca67b0` `32ca9720` `32c7a610` `32c97150` | `..._RTR_ZZ` Start / FrameMove / EventProcess / StateEnd |
| `32c7a640` `32c7a680` | `LESI_SA_RTR_FATALITY_SUCCESS` / `_FAIL` EventProcess |
| `32ca6410` `32c96e50` `32ca69e0` `32ca6b10` `32ca6be0` `32c9a5b0` | `CreateTrapBlade`, `DeleteTrapBlade`, `SetPriorPlannedBlow`, `EndPriorPlannedBlow`, `CheckPriorPlannedBlow`, `InsertPriorPlannedBlowEffect` (all = March) |
| `32c6d580` `32ca7670` `32c818d0` `32ca0cd0` | **new** `LESI_RTR_DOUBLE_JUMP_ZX` StateEndFuture / StateStart / EventProcess / StateEnd |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1g`; the 9 StateEnds, the Starts and the FrameMove thunks checked by a normalised instruction diff within each group; 14 rows equal March's body, written out whole with `// June <va> ( = March )`):
- **EventProcess = March's `ADD_RENA_SYSTEM` revamp** (dead until CH1q) with the animation-end follow-up (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`) and the foot test on every cancel (`RENA_GROUND_*`, CH1d's macros; `SKILL_CANCEL_AFTER` already carries June's foot test and awakening branch):
  - Z-Z-Z (`32c79470`): flags 0 `oneZ` + up `LESI_COMBO_Z4UP`, 1 `oneZ` `LESI_COMBO_RTR_ZZZZ`, 2 `oneX` `LESI_COMBO_Z4a`; cancels dash, walk, skill, back-walk. Z-Z-Z-Z (`32c79be0`): flag 0 `oneZ` `LESI_COMBO_RTR_ZZZZZ`; same order.
  - Z-Z-Z-down-Z `32c79950`, Z-Z-Z-Z-Z `32c79f50` (1.3 s walk exit on the Night Watcher flag, future animation only) and dash Z-X `32c7a290`: skill, walk, back-walk, dash. Dash-jump Z `32c7a510`: the Z follow-up also at the animation end.
- **Z-Z-Z-Z-Z FrameMove `32cad240`:** the Night Watcher force-down term is `down * ( hits - 4 )`; March has `hits - 5`. Written as `MIN_PRIOR_PLANNED_BLOW_ACTIVE_NUM - 1` (the helper's HP term uses the same `+ 1`).
- **`LESI_RTR_DOUBLE_JUMP_ZX`:** StateEndFuture turns the unit round, then `CommonStateEndFuture`; Start = trap blade + `CommonStateStart` (no Prior Planned Blow bookkeeping); EventProcess = animation end -> wait (no `m_fCanNotAttackTime`, no jump-down), then skill / walk / back-walk / dash; StateEnd deletes the blade before `CommonStateEnd` (the other StateEnds delete it after).
- **Equal to March:** the Prior Planned Blow trio (`SetPriorPlannedBlow` 10 s and the 5 / 14 clamp, `CheckPriorPlannedBlow` HP term, `EndPriorPlannedBlow`), `InsertPriorPlannedBlowEffect` (levels 5 / 9 / 14 and `PlayEffectSet` defaults), `CreateTrapBlade` (the Night Watcher takes `Mesh_RNW_Lire_Sword`; March's `TextureReady` is background-load-only, off), the jump attack Z walk speed (`GetWalkSpeed()` already carries `m_fRunJumpSpeedRate`, `+0x423c`), fatality success / fail.
- **Natural force:** none of the 6 `ch0_nf.tsv` rows for CH1g is an NF line in June; they are the revamp blocks above, so no `_NF` code was written and March's `ADD_RENA_SYSTEM` arms stay dead in the `#else`.
- **Structure note:** `CheckPriorPlannedBlow`'s signature and closing lines carry Korean comments or repeat, so its June copy is a whole function above March's under the same `#ifdef` / `#else` / `#endif` (the other 41 wrap the body).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`_CORE`). `git diff --patience --numstat`: `.cpp` 620 / 0, `.h` 4 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P` off / on: 1492 `CX2GULire_ElvenRanger::` lines each; `MIN_PRIOR_PLANNED_BLOW_ACTIVE_NUM - 1` 0 / 1. Flag states by `cl /P` probe: `SERV_RENA_NIGHT_WATCHER`, `SERV_TRAPPING_RANGER_TEST`, `X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD`, `BALANCE_PATCH_20131107`, `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` ON; `ADD_RENA_SYSTEM` OFF.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check` on both: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,096,640 B, 2026-11-09 09:39:47`; `DEPLOY OK: X2_offline.exe is 26,096,640 B, dated 2026-11-09 09:39:54, md5 deb36f6607dba9c0d946b2ed6877dc41, equal to X2/US_SERVICE/x2.exe (2026-11-09 09:39:47); built at main b126a821 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string was added to a live path.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1g review` (Opus 5.5 high), then CH1g2.

### CH1g - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** All 46 rows read against `charport_adis.py rena --phase CH1g` (float tests by their x87 flag test, `test ah, 5` / `0x41` with `jp` / `jne`).
- **Class-map corrections confirmed** (June = March for all six): `CreateTrapBlade` `32ca6410` (delete if `+0x968c != -1`; class `0x18` = `UC_LIRE_TRAPPING_RANGER` -> `Mesh_RTR_Lire_Sword`, else `Mesh_RNW_Lire_Sword`; `CreateInstance( this, name, GetPos(), GetRotateDegree() x2 )`, `+0x19c = 1` per-frame simulation, radius 0, `Dummy1_Rhand` matrix, `OnFrameMove( time, 0.01 )`, `SetSubAttackList`, `+0x96a4 = 1`). `SetPriorPlannedBlow` `32ca69e0`: 10.0, `++count`, `>= 5` -> insert + active, `>= 14` -> 14. `CheckPriorPlannedBlow` `32ca6be0`: `UpNowHp( max * rate * ( count - 4 ) )` (`add eax, -4`) then `EndPriorPlannedBlow` (tail jump), else the hit-count branch clears `+0x80cc` (`bHit`). `InsertPriorPlannedBlowEffect` `32c9a5b0`: levels `>= 14` / `>= 9`, `+0x96fc` compare, stop, `StringCchPrintf` into 0x46 = 70 wchars, `PlayEffectSet` defaults. `DeleteTrapBlade` `32c96e50` and `EndPriorPlannedBlow` = March. The whole-function wrap of `CheckPriorPlannedBlow` is correct (`#else` above March's commented signature).
- **Combo EventProcesses:** `32c79470` flag 0 = event time 0 (`+0x8c10..18`) `oneZ` + `pureUp` -> 0xb1 `LESI_COMBO_Z4UP`, flag 1 = time 1 `oneZ` -> 0xdc, flag 2 = time 2 `oneX` -> 0xb0; each change test `< stateChange || +0x2ac` (animation end). Cancels dash `+0x8c9c`, walk `+0x8c98`, skill `+0x8c90` (with hyper branch), back-walk `+0x8c94`, each with `+0x7e1c`. `32c79be0`: flag 0 time 0 `oneZ` -> 0xdd, same order. `32c79950`, `32c79f50`, `32c7a290`: skill, walk, back-walk, dash. `32c79f50`'s exit reads `+0x96d6` and `1.3 < future t` (`fcomp`, `test ah, 5`, `jp`) with `pureRight || pureLeft` and falls through to the chain. Fall / end: y - 45, end sets `+0xde0` (`m_fCanNotAttackTime`) 0.2 except dash Z-X (end: air -> JUMP_DOWN, else WAIT). Dash-jump Z `32c7a510`: foot -> 0xc2, else flag 0 time 0 `oneZ` -> 0xe0.
- **ZZZZZ FrameMove `32cad240`:** inlined `EventTimerOneshot` (`+0x24c` is the anim's oneshot timer), `SimpleInit`, attacker type 0, `DAMAGE_DATA_NEXT`; with `+0x96c9` and `+0x96d4`: `+0x3c4 = 2` (`RT_BIG_DAMAGE`), `force -= down * ( count - 4 )`, word `0x101` at `+0x96d5` (both flags); then `m_bPossibleHitCount` when not attacking, `CheckPriorPlannedBlow`, `CommonFrameMove`. = the port.
- **New `LESI_RTR_DOUBLE_JUMP_ZX_*`:** `32c6d580` flip `bIsRight`, `dirDegree.y + 180`, then `CommonStateEndFuture` (vslot 0x7d8). `32ca7670` blade + `CommonStateStart`. `32c818d0` future animation end -> WAIT (no `+0xde0`, no jump-down), then skill, walk, back-walk, dash with the foot. `32ca0cd0` clear / destroy / `CommonStateEnd` / `+0x96a4 = 0` (blade first), against `32c96eb0` (`CommonStateEnd` first).
- **Short rows:** `32ca9680` 1.29 oneshot, delete blade, class `0x72` = `UC_LIRE_NIGHT_WATCHER` clears `+0x96f0`; `32c84d90` walk speed and the down press (`fGAccel * m_fElapsedTime`, flag 0); `32c7a220` `+0xde4` (`m_fCanNotJumpTime`) 0.2 on flag 0; Starts `32ca6730` / `32ca67b0` (class 0x72); fatality, StateEnds, thunks = their listings.
- **Routed to CH1g2 / CH1g3** (unchanged): the Night Watcher states call the six helpers by name with June's arguments.

**Gates.** `flagcheck ..._LIVE --with ..._CORE --range d968efa2^..d968efa2`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` (OTHER = the `.h`, `_CORE`), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented `_LIVE` define. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows. One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No build: the review changes no code.

CH1g2 kickoff checked: present (Sonnet 5.5 high, depends on this review). Nothing to play-test. Next: `/x2-phase char-rena CH1g2` (Sonnet 5.5 high).

### CH1g2 - Done (2026-11-09)

All 29 class-map rows of phase CH1g2 are written: 12 `inside` (the three Night Watcher combos, June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`) and 17 `new` (`LESI_RNW_DASH_XZ*`, `_CORE`, compiled in every build, referenced by nothing yet). `_LIVE` stays `//#define`; `InitState*` untouched. `X2GULire_ElvenRanger.h` (`_CORE`): 4 enum members (`LESI_RNW_DASH_XZZ_START` / `_LOOP` / `_FULL_CHARGE_END` / `_END`, June 273-276; `LESI_RNW_DASH_XZ` 272 was CH1d2's) and 17 declarations. 29 funcmap rows `Char Rena CH1g2`; no `JUNE_BUGS` row (see below). **START / WIN / LOSE:** the kickoff's "start / win / lose" rows are shared base states (`CX2GUUser::StartEventProcess`, `WinStateStart`, `LoseStateStart`, targets outside the Rena range): no function to write; CH1m registers them.

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32ca9490` `32ca94c0` `32c75160` `32ca62f0` | `LESI_COMBO_RNW_ZZX` Start (= March) / FrameMove (= March) / EventProcess (differs) / StateEnd (= March) |
| `32ca94e0` `32ca9510` `32c754d0` `32ca6350` | `..._ZZXX` Start / FrameMove / EventProcess (differs) / StateEnd |
| `32ca9530` `32ca9550` `32c75840` `32ca63b0` | `..._ZZXXX` Start (= March) / FrameMove (differs) / EventProcess (differs) / StateEnd |
| `32ca7690` `32c81b30` `32ca0d30` | `LESI_RNW_DASH_XZ` StateStart / EventProcess / StateEnd (new) |
| `32ca76b0` `32c6d5b0` `32c81e80` `32ca0d90` | `..._XZZ_START` StateStart / StateEndFuture / EventProcess / StateEnd (new) |
| `32ca76d0` `32c82100` `32ca0df0` | `..._XZZ_LOOP` StateStart / EventProcess / StateEnd (new) |
| `32ca76f0` `32c82390` `32ca0e60` | `..._XZZ_FULL_CHARGE_END` StateStart / EventProcess / StateEnd (new) |
| `32ca7710` `32c6d5c0` `32c825f0` `32ca0ec0` | `..._XZZ_END` StateStart / StateEndFuture / EventProcess / StateEnd (new) |

**Class-map / trap-map correction.** `ch0_traps.tsv` row 4 put the Gungnir `CreateNPCReq` (`@32ca11f2`, `NUI_RENA_TARGETING_GUNGNIR`) in `32ca0e60`, so the kickoff told this phase to write the spawn. `32ca0e60` is only `DeleteTrapBlade` + `CommonStateEnd`; `32ca11f2` is inside the Gungnir ready FrameMove `32ca0f80` (`charport_adis.py rena 32ca0f80` contains it), which CH1f2 wrote (`.cpp` `CreateNPCReq( NUI_RENA_TARGETING_GUNGNIR ... )`). No spawn written here; the row now names `32ca0f80` / CH1f2 (CH2d0 keeps the spawn rule).

**Where June differs from March** (read from `charport_adis.py rena --phase CH1g2`; the three StateEnds and the XZ / XZZ Start thunks checked by a normalised instruction diff; FULL_CHARGE_END and END EventProcess are instruction-identical):
- **EventProcess = March's `ADD_RENA_SYSTEM` revamp** (dead until CH1q) with the animation-end follow-up (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`, event time 0, `oneX`) and the foot test on every cancel (`RENA_GROUND_*`, CH1d's macros). Cancel order: ZZX dash, walk, skill, back-walk; **ZZXX dash, back-walk input with the walk time (`+0x8c98`), skill, walk** (March pairs them the same way); ZZXXX skill, walk, back-walk, dash. ZZX / ZZXX / ZZXXX keep the foot-lost jump-down and the 0.2 s `m_fCanNotAttackTime` wait.
- **ZZXXX FrameMove `32ca9550`:** the damage rises by `1 + increase * ( hits - 4 )` (March `hits`), written as `MIN_PRIOR_PLANNED_BLOW_ACTIVE_NUM - 1` (as CH1g's ZZZZZ).
- **Dash XZ `32c81b30`:** a lost foot also zeroes `m_PhysicParam.nowSpeed.x`; the animation end goes to wait (no 0.2 s); the Z follow-up (`oneZ`, event time 0) to XZZ_START (also at the animation end); cancels skill, walk, back-walk, dash. **START `32c81e80`:** animation end -> LOOP. **LOOP `32c82100`:** past `m_fEventTime[0].keyInputStart` it runs the animation-end test (FULL_CHARGE_END) and then the `pureZ` release test (END) with no `else`, so a release at the last frame wins; before that time the cancel chain. **FULL_CHARGE_END / END `32c82390` / `32c825f0`:** animation end -> wait, the cancel chain.
- **`SetNoDetonation`:** LOOP Start sets it `true` after `CommonStateStart`, LOOP End `false` after `CommonStateEnd` (June `CX2GameUnit +0x6890`, the same flag Raven's states write).
- **StateEndFuture:** START = `CommonStateEndFuture`; END turns the unit round (`bIsRight` flipped, `dirDegree.y + 180`), as `LESI_RTR_DOUBLE_JUMP_ZX`.
- **Equal to March:** the Start / FrameMove / StateEnd rows of ZZX / ZZXX and ZZXXX Start (`DeleteTrapBlade` / `CreateTrapBlade` / the Prior Planned Blow flags), written whole with `// June <va> ( = March )`.
- **Natural force:** the 3 `ch0_nf.tsv` rows for CH1g2 are the revamp blocks above, not NF lines in June: no `_NF` code written.
- **No `JUNE_BUGS` row:** the ZZXX back-walk / walk-time pairing is March's own, and LOOP's two tests are consistent (release wins).

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`_CORE`). `git diff --patience --numstat`: `.cpp` 342 / 0, `.h` 21 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /P` off / on: 1510 `CX2GULire_ElvenRanger::` lines each; `MIN_PRIOR_PLANNED_BLOW_ACTIVE_NUM - 1` 0 / 2 (CH1g's one + this phase's). Flag states by `cl /P` probe: `NO_DETONATION`, `SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG`, `SERV_RENA_NIGHT_WATCHER`, `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` ON; `ADD_RENA_SYSTEM` OFF.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check` on both: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,099,712 B, 2026-11-09 10:09:42`; `DEPLOY OK: X2_offline.exe is 26,099,712 B, dated 2026-11-09 10:09:49, md5 eee4d063335706d31ada0e0c9fec81e0, equal to X2/US_SERVICE/x2.exe (2026-11-09 10:09:42); built at main 90721ebd + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string was added.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1g2 review` (Opus 5.5 high), then CH1g3.

### CH1g2 - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** All 29 rows read against `charport_adis.py rena --phase CH1g2` (float tests by their x87 flag test, `test ah, 5` / `0x41` with `jp` / `jne`).
- **Combo EventProcesses.** `32c75160` ZZX: direction change clears flags 0-2 (word + byte at `+0x7d85`); foot lost -> 0x11 `JUMP_DOWN`, y - 45; flag 0 with `< +0x8c18 || +0x2ac` -> 0xe5 `LESI_COMBO_RNW_ZZXX`, else `+0x8c10 <= t < +0x8c14` + `oneX` sets it; animation end -> `+0xde0` 0.2 then WAIT; dash `+0x8c9c`, walk `+0x8c98` (right || left), skill `+0x8c90` (foot, hyper branch), back-walk `+0x8c94`, each with `+0x7e1c`. `32c754d0` ZZXX: same head, -> 0xe6; dash, then `+0x8c98` with the back-walk input (left && right-facing / right && left-facing), skill, then `+0x8c98` with right || left. `32c75840` ZZXXX: no follow-up; skill, walk, back-walk, dash. = the port.
- **ZZXXX FrameMove `32ca9550`:** inlined `EventTimerOneshot( 0.001 )`; active -> `fPhysic *= 1 + rate * ( count + -4 )`, `+0x96d5 = 1`; else `+0x96f0 = 1`; `CheckPriorPlannedBlow` inside the `+0x96c9` block, then `CommonFrameMove`. = the port (`MIN_PRIOR_PLANNED_BLOW_ACTIVE_NUM - 1` = 4).
- **Dash XZ / XZZ EventProcesses.** `32c81b30`: foot lost -> JUMP_DOWN, `nowSpeed.x = 0`, y - 45; animation end -> WAIT (no 0.2); flag 0 time 0 `oneZ` -> 0x111 (its animation-end arm is dead behind the WAIT test, in June too); skill, walk, back-walk, dash. `32c81e80`: animation end -> 0x112 LOOP. `32c82100`: `keyInputStart < t` -> animation end -> 0x113, then `pureZ == 0` -> 0x114 (no `else`), else the cancel chain. `32c82390` / `32c825f0`: no foot test first, animation end -> WAIT, the cancel chain. = the port.
- **Start / End / EndFuture.** Starts `32ca7690` / `76b0` / `76f0` / `7710`: `CreateTrapBlade` + `CommonStateStart`; `32ca76d0` adds `+0x6890 = 1` after it; `32ca0df0` writes `+0x6890 = 0` after `CommonStateEnd`. All eight StateEnds inline `DeleteTrapBlade` (`+0x220` set clear, destroy `+0x968c`, `+0x96a4 = 0`) before `CommonStateEnd`. `32c6d5b0` = `CommonStateEndFuture`; `32c6d5c0` flips `bIsRight`, `dirDegree.y + 180`. ZZX / ZZXX Start / FrameMove and ZZXXX Start = March.
- **Class-map correction confirmed:** `32ca0e60` is the plain StateEnd above; the Gungnir `CreateNPCReq` stays CH1f2's (`32ca0f80`). START / WIN / LOSE: shared base states, nothing to write.

**Gates.** `flagcheck ..._LIVE --with ..._CORE --range ba5e3853^..ba5e3853`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` (OTHER = the `.h`, `_CORE`), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented `_LIVE` define. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows. One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No build: the review changes no code.

CH1g3 kickoff checked: present (Sonnet 5.5 high, depends on this review). Nothing to play-test. Next: `/x2-phase char-rena CH1g3` (Sonnet 5.5 high).

## Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL`.
Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 3 has it -> CH2a; else the first of 12 / 13 / 24.

| Sub-phase | Scope |
|---|---|
| CH2a | Rena (3) skills |
| CH2b | Combat Ranger (12) skills (`Effectset_Rena_DiveKickBombingA` / `B` / `B_Memo` March-only templets, 7u, if CH0 puts them here) |
| CH2c | Sniping Ranger (13) skills (`DamageEffect_Cryotron_Bolt_GUIDED_ARROW`, 7u; the Guide Arrow `:21554` if CH0 puts it here) |
| CH2d0 | **Opus.** Trap summons (`ch0_traps.tsv`): the spawn rules shared by her traps (June's `CreateNPCReq` arguments at the 4 sites, the master / summoned link, the trap's life and removal), `CREATE_TRAP_BLADE`'s reader `32ca81b0` if CH0 puts it here, every trap NPC script through `luabind.py check` (unbound names bound whole or `lua_unbound_known.tsv`), flag `_TRAP`, sub-tag `TRAP` |
| CH2d | Trapping Ranger (24) skills (Entangle `:15884`, Evoke trap, Trap Blade `:17797` states, on CH2d0's spawn rules) |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, the power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `_NF`. Upgrade ids reach their base through the
templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id. `NUM use` lines for Rena
skills; March vs June damage in `MODS.md`.

**Done gate:** every class-3 / 12 / 13 / 24 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot
casts June's state, motion and effect, deals damage, shows its cool time and spends MP; traps spawn, hit and expire as June; no missing particle /
mesh / texture line.

## Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence

**Run with:** as CH2 (CH3d on Opus: it carries engine rows). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Wind Sneaker (102) skills |
| CH3b | Grand Archer (103) skills (`DamageEffect_LGA_Arc_Shot`, 7u; Anger of Elf `:22713` if CH0 puts it here) |
| CH3c | Night Watcher (114) skills (the `RNW` / `LNW` skill states, her traps' Night Watcher variants on CH2d0's rules) |
| CH3d | **Opus.** Hyper actives (type 13, 6 rows; June's hyper switch in `32cb06d0`), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): Rena's classes on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`), her `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Lena.lua`; checklist 7ao whole (hyper effect start `32c9b3d0`, uncarved; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`; l.969); her hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_RWS/RGA/RNW_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and her transcended pictures, read from June's row pushes; `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` present |

**Done gate:** as CH2 for 102 / 103 / 114 and the hyper actives; a transcended Rena (`settranscend`) casts her hyper-class skills, slots A-5 /
B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Rena's skill-id range>` = PASS after the transcendence rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as two from the start; CH0 adds CH4c if the
memo or a passive engine piece is over ~45 functions).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32ca78b0` (vslot 0x690, uncarved) and Rena's passive ids (types 1 / 8: 3 + 21 rows), their objects and readers; **D8** (checklist l.330-331): March's Powerful Bowstring 3405 and Sharp Kick 3400 blocks replaced by June's 23405 / 23400 together with the reform skills; the delegate registrations (l.912-925) replacing March's own pushes; the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `32cad510` whole (June-only, 7b: per-skill triggers, Powerful Bowstring's ignore-defence write `32cada25`, funcmap l.925); the overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` 0x6a0 / 0x6a4, buff-templet set / unset 0x24c / 0x250, uncarved where so); `MemoConditionRena` (RTTI `3373d738`, vtable from CH0) and the memo readers; §7 l.281 power-rate socket range if a Rena function calls `3201ad10` |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); Powerful Bowstring / Sharp Kick act on June's ids; 0
Rena `no_state` rows left except passives with no state in June.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_UI`. Sub-tag `UI`.

**Scope:** Rena's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays): `m_fLobbyRotationY`,
`m_HyperCutIn` for 102 / 103 / 114, the names `STR_ID_4783` / `_4784` / `_17429`; the cut-in table `X2GULire_ElvenRanger.cpp:50-82` and the
InitState index switch `:1672-1715` replaced by June's names and layout (7t cut-in `.gfx` names); 7ah; the gauge pieces CH1q does not take.
**Done gate:** a select-screen pass of all nine March characters with no new error; Rena's skill and hyper cut-ins play June's art; the three
second-job names show June's strings.

## Phase CH-N - not run for Rena

All seven June Rena classes up to transcendence exist in March. The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara
runs it first.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Rena per class (CH0b creates 3 / 12 / 13 / 24 / 102 / 114), one
transcended per second job. Walk: every skill in an A and a B slot (each cast twice: cool time and MP), a hyper, natural force (fill, spend,
gauge, hyper buff), each trap, the passives, one dungeon per job, then **leave the game and return to the village**. Re-run `census.py`,
`charport_scan.py`, `charport_keys.py Lire_ElvenRanger` and `s3_no_state.py`; before / after numbers in the Done note. **The becoming-transcended
transition on un-transcended units (user rule, 2026-10-28):** one unit per second-job class by `/tc 1`, quest 42030 (`/qdone 42030`) and the book
288493 (`/additem 288493 1`), each showing the guide popup with that class's picture, the new skill-tree rows, the HUD portrait, and surviving a
relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Rena" section (flags, files, data, revert order: exe + data036 + motion koms
together; natural force's `ServerDefine.h` second home); no `_DEBUG` flag left defined; checklist rows marked `(Rena: CHn)`; the Aisha plan's
"Template for Stage 7" gets a Rena row (what she took against this plan); backlog Stage 7 note; the `stage7-plan` row points at the next
character (`char-ara`, the first CH-N run).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR RENA <sub>"`.

## Open decisions (for the user; asked at the end of planning; answered 2026-11-07)

1. **Natural force (`ADD_RENA_SYSTEM`).** June 2017 ships it; March has the studio's 2013-14 code dormant (187 blocks in 23 files: Rena's class,
   `X2GUUser`, the damage / effect-set first-attack copies, the NPC side, the gauge data / UI, `CommonPacket.h`). Options: **(a, recommended)**
   as Eve's Nasod Core: March's dormant code as the base. CH1q defines `ADD_RENA_SYSTEM` under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (only with
   `_LIVE`; plus the `ServerDefine.h` second home `CommonPacket.h` needs), then replaces every function that differs from June with June's whole
   body, resolves the June-port arms the flag flips to June's branch, and drops what June removed (the studio overlay / dev key if June lacks
   them). **(b)** transcribe June's natural force fresh beside the dead code, leaving `ADD_RENA_SYSTEM` off. **(c)** leave Rena without natural
   force (against the "June wins" standing rule; not recommended). (a) reuses the members, enums, packet accessor and gauge classes March already
   wired; (b) avoids the flipped arms but rewrites the same members.
   **Answered (user, 2026-11-07): (a), March's dormant code as the base**, June's whole body wherever it differs.

Settled by standing rules, not decisions: the motion koms deploy with CH1 final if June drops a name March plays (7ad); trap summons get their
own Opus sub-phase (Eve / Chung lesson); June-side icon gaps are `JUNE_BUGS.md` rows (Raven's 535).
