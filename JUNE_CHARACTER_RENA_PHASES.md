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
| CH2a-CH2d | Skills: Rena + first jobs (3, 12, 13, 24) | Sonnet high, Opus review per series (CH2a, CH2b each; CH2c + CH2d together, user 2026-11-10), user play-test | CH1 review |
| CH2d0 | Trap summons: spawn rules, the 4 June `CreateNPCReq` sites, trap NPC scripts' binder gate, flag `_TRAP` | Opus high, Opus review | CH2c |
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

### CH1g3 - Done (2026-11-09)

All 27 class-map rows of phase CH1g3 are written: 22 `inside` (the Night Watcher accel dash and dash-jump states, June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`) and 5 `new` (`LESI_RNW_DASH_JUMP_XZ_START_EventProcess`, `_XXZ_LOOP_StateStart` / `_EventProcess` / `_StateEnd`, `_XZ_END_EventProcess`; `_CORE`, compiled in every build, referenced by nothing yet). `_LIVE` stays `//#define`; `InitState*` untouched. `X2GULire_ElvenRanger.h` (`_CORE`): 3 enum members (`LESI_RNW_DASH_JUMP_XZ_END` 279, `_XXZ_LOOP` 281, `_XXZ_END` 282; 277 and 280 were CH1g's) and 5 declarations. 27 funcmap rows `Char Rena CH1g3`; no `JUNE_BUGS` row (see below).

| June | `CX2GULire_ElvenRanger::` |
|---|---|
| `32c6b4b0` `32c6b4e0` `32c84820` `32c948e0` | `LESI_RNW_ACCEL_DASH` StateStartFuture / StateStart / FrameMoveFuture / EventProcess (all = March) |
| `32c848c0` `32c75b00` `32c6b500` `32c6b510` `32c75b50` | `..._ACCEL_DASH_JUMP` StateStartFuture / StateStart / FrameMoveFuture / StateEndFuture / EventProcess (= March) |
| `32c6b530` `32c6b560` `32c94ba0` | `..._JUMP_LANDING` StateStartFuture / StateStart / EventProcess (= March) |
| `32c84920` `32c6b580` `32c6b590` `32c75bc0` `32c6b5a0` | `..._JUMP_ATTACK_Z` StateStartFuture / FrameMove / FrameMoveFuture / EventProcess (**differs**) / StateEndFuture |
| `32c84980` `32c6b5c0` `32c6b5d0` `32c75cc0` `32c6b5e0` | `..._JUMP_ATTACK_ZZ` StateStartFuture / FrameMove / FrameMoveFuture / EventProcess / StateEndFuture (= March) |
| `32c82850` | `LESI_RNW_DASH_JUMP_XZ_START_EventProcess` (new; the XZ start and the XXZ start share it) |
| `32ca7730` `32c828a0` `32ca0f20` | `LESI_RNW_DASH_JUMP_XXZ_LOOP` StateStart / EventProcess / StateEnd (new) |
| `32c82ab0` | `LESI_RNW_DASH_JUMP_XZ_END_EventProcess` (new; the XZ end and the XXZ end share it) |

**Where June differs from March** (read from `charport_adis.py rena --phase CH1g3`):
- **21 rows equal March** and are written whole with `// June <va> ( = March )`. June inlines `GetDashSpeed` (`+0x423c` x the decoded triple at `+0x49c4`) and the hyper-mode cancels (`SKILL_CANCEL_AFTER( 0.01f )`, `TryHyperModeStateChange`); the `CanDown` call is June's 9-argument form with `bDownRadius` true, `bIsMonster` false, `bOnLine` true, `bDroppable` false = March's 4-argument forwarder. Constants as written: `2.25` / `2.45` / `2.35` (x `GetDashSpeed`), `/ 1.5`, `0.13` s, `1600`, `LINE_RADIUS * 1.5`.
- **`ACCEL_DASH_JUMP_ATTACK_Z_EventProcess` `32c75bc0`:** foot -> LANDING; the ZZ follow-up is March's combo-revamp form, flag 0 / event time 0 with the animation end next to the change-time test (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`), in place of March's live 0.25 s window (its `ADD_RENA_SYSTEM` arm lacks the animation end and stays dead).
- **New `32c82850`:** animation end -> `0x119` (id 281 `XXZ_LOOP`), else foot -> `0x11a` (id 282 `XXZ_END`), then `CommonEventProcess`. **`32c828a0` (LOOP):** foot -> `0x11a`; then only the hyper-mode branch of the skill macro is in June's listing (the `SpecialAttackEventProcess` branch is folded away by the compiler because the foot is known false there), then walk `+0x8c98`, back-walk `+0x8c94`, dash `+0x8c9c`; the C++ macro form is behaviour-identical. **`32c82ab0` (END):** animation end -> wait, then the whole chain with the skill branch. **`32ca7730` / `32ca0f20`:** `CreateTrapBlade` / `DeleteTrapBlade` (inlined) around `CommonStateStart` / `CommonStateEnd`; no `SetNoDetonation` here.
- **Id 278:** June registers the XZ chain's loop slot (id 278) with the `LESI_RNW_DASH_JUMP_XXZ_LOOP` script table (`LESI_RNW_DASH_JUMP_XZ_LOOP` is absent from the exe) and nothing in this group changes state to it; no enum member written, **CH1m decides how to register 278**.
- **Natural force:** the one `ch0_nf.tsv` row for CH1g3 is the combo-revamp block of `32c75bc0` above, not an NF line in June: no `_NF` code written.
- **No `JUNE_BUGS` row:** the quirks (`ACCEL_DASH_JUMP_StateEndFuture` ending the common state instead of the future frame's, the `m_fWalkCancelAfter` speed bonus in the accel dash start) are March's own and equal June's.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with ..._CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out (by design). OTHER = the `.h` (`_CORE`). `git diff --patience --numstat`: `.cpp` 337 / 0, `.h` 8 / 0.
- One-file `/Zs` probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) of `X2GULire_ElvenRanger.cpp`: exit 0, 0 errors with no extra defines, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check` on both: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,100,736 B, 2026-11-09 10:37:18`; `DEPLOY OK: X2_offline.exe is 26,100,736 B, dated 2026-11-09 10:37:25, md5 4db15c798e55f6fd419b35d113745583, equal to X2/US_SERVICE/x2.exe (2026-11-09 10:37:18); built at main 853b7737 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string was added.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-rena CH1g3 review` (Opus 5.5 high), then CH1h.

### CH1g3 - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** All 27 rows read against `charport_adis.py rena --phase CH1g3` (float tests by their x87 flag test, `test ah, 5` / `0x41` with `jp` / `jne`).
- **The 21 `= March` rows.** Accel dash: StartFuture `nowSpeed.x += +0x8c98` (`m_fWalkCancelAfter`); Start `CommonStateStart` + `CreateStepDust`; FrameMoveFuture `32c84820` `nowSpeed.x >= m_fRunJumpSpeedRate x triple(+0x49c4)` -> `- m_fElapsedTime x 1600`, else = the dash speed; class `0x72` (114, `UC_LIRE_NIGHT_WATCHER`) with `0.13 <= fStateTime` -> `+0x96c8 = 1`. EventProcess `32c948e0`: foot lost -> `0x15` `DASH_JUMP` y - 45; `oneZ` 0xb8, `oneX` 0xbb; skill branch `anim > 0.01` + foot + `SpecialAttackEventProcess( 1 )`, hyper branch (black hole, `oneLCtr`, count, vslot 0x560, quick slot, `+0x8d4c = 1`); `oneUp` -> 0xe8 with `nowSpeed.y = +0x330`; `TryHyperModeStateChange( 0x17 )`; facing input + animation end -> dash; the four `DASH_END` tests in March's order; `oneDown` + `CanDown( 30, ..., 1, 0, 1, 0 )` -> `DASH_JUMP`. Dash jump: StartFuture `+0x318 = +0x8c90`, x = dash speed x 2.25, `m_bDownForce = 0`; Start `GetYPos == landPosition.y` (`fucompp`, `0x44`) -> step dust; StateEndFuture `+0x318 = +0x2e4` then `CommonStateEnd` (vslot 0x7dc); EventProcess `32c75b50`: `oneUp` && `+0x95f0 == 0` -> 0x9b, else `SpecialAttackEventProcess( 2 )`; foot -> 0xe9, else `oneZ` -> 0xea. Landing `/ 1.5`, step dust; EventProcess `32c94ba0`: foot lost -> `JUMP_DOWN` y - 45, skill 0.01, double-tap / facing -> dash, walk, `pureUp` 0xf, hyper, `oneDown` + `CanDown`, animation end -> wait. Z / ZZ: x 2.45 with y = 0 / x 2.35 with y = `+0x32c` x 0.5, `+0x318` saved and restored after `CommonStateEndFuture`; FrameMove / FrameMoveFuture are the plain Common calls; ZZ EventProcess foot -> 0xe9. = the port.
- **`32c75bc0` (differs):** foot -> 0xe9; flag 0 set and (`+0x8c18 < t` or `+0x2ac`) -> 0xeb and the flag cleared; else `t < +0x8c14` and `+0x8c10 <= t` with `oneZ` sets it. = `ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_( 0, 0, ... )` (`.cpp:74`).
- **The five new rows.** `32c82850`: `m_pXSkinAnimFuture +0x2ac` -> 0x119, else foot -> 0x11a, `CommonEventProcess`. `32c828a0`: foot -> 0x11a; the skill branch survives only as a discarded `fcomp +0x8c90` (the foot test folded false), then the hyper branch with `+0x8c90`, walk `+0x8c98` (right || left -> 0xd), back-walk `+0x8c94`, dash `+0x8c9c` (vslot 0x668), each with the foot test. `32c82ab0`: animation end -> 0xd, then the same chain with the skill branch (`SpecialAttackEventProcess( 1 )`). `32ca7730`: `CreateTrapBlade` then `CommonStateStart`. `32ca0f20`: `+0x220` set clear, destroy `+0x968c`, `+0x96a4 = 0`, `CommonStateEnd`; no `+0x6890`. = the port.
- **Enum:** 279 / 281 / 282 appended after `LESI_RNW_DASH_XZZ_END` under `_CORE`, as CH1g2's; 278 has no member, CH1m decides.

**Gates.** `flagcheck ..._LIVE --with ..._CORE --range 44e2d623^..44e2d623`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` (OTHER = the `.h`, `_CORE`), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented `_LIVE` define. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows. One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No build: the review changes no code.

CH1h kickoff checked: present (Sonnet 5.5 high, depends on this review). Nothing to play-test. Next: `/x2-phase char-rena CH1h` (Sonnet 5.5 high).

### CH1h - Done (2026-11-09)

All 33 class-map rows of phase CH1h are accounted for: **25 `inside`** written (June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`; 2 are the `GetComboZ/XStateID` header inlines), **2 `new`** under `_CORE` (`ProcessAttackResultPassive`, `ProcessEradicationHit`, declared, referenced by nothing) and **6 with nothing to write** (`32c6d870` jmp thunk to base `31fca040`; `32cb34e0` an inlined STL tree step; `32cb3500` = March's `TimeLimited::OnFrameMove`; `32cb4200` = March's `FriendshipNatureData::OnFrameMove`; `32cb4270` and `32cb6c50` scalar deleting destructors). `_LIVE` stays `//#define`; the constructor `32ca2b60` is CH1k's by CH0's map (its stores for the new members are in `kickoffs/char-rena/CH1k.md`). 33 funcmap rows `Char Rena CH1h`, 7 `ch1_offsets.tsv` rows. No `JUNE_BUGS` row: nothing here is a June mistake.
**CH0 map corrections (March has all of these):** `32c89270` = `DeleteLireMajorParticle`, `32c89310` / `32c89460` = `SetRenaMajorParticleByEnum` / `SetRenaMinorParticleByEnum`, `32c95670` / `32c9e130` = `MakeRoomForEntangleTrap` / `MakeRoomForAngerOfElfTrap`, `32c9b320` = `UpNowMpPerHitOthers`, `32c85d50` = `InitDevice`, `32c895b0` = `InitComponent` (CH0 listed them as `new`).

**Where June differs from March** (read from `charport_adis.py rena --phase CH1h`):
- **`DamageReact` `32c6da50`:** the friendship-of-nature delegate also needs `damageType != DT_BUFF` ( `[edi] == 6` ); no `NEW_SKILL_2010_11` gate.
- **`ApplyRenderParam` `32c7fa40`:** the else branch sets colour alpha 1 when `!IsAlphaObject()` before the base ( `31fd4460`, stand-in `CX2GameUnit::ApplyRenderParam` ); no `RIDING_SYSTEM` gate.
- **`InitEnchantWeaponEffectUnitSpecific` `32c835c0`:** the arrow mesh index is 1 / 2 / 3 for every double / quadra / tripple master of Blaze / Frozen / Poison (15 extra damage types, ids checked against `Enum.src.lua`; March knows the three double masters).
- **`SetRenaMajor` / `MinorParticleByEnum` `32c89310` / `32c89460`:** a newly made handle goes through `TagEffectAlphaParticleHandle` ( major / minor ): 7c sites `32c893e5`, `32c89535`. **`InitComponent` `32c895b0`:** no `SetNowMp( 50 )` in the PvP case ( only `SetNaturalForceCount( 0 )`, `@32c895f4`, under `_NF` ), the six minor handles tagged ( 7c sites `32c8990b`-`32c8996f` ), `SIEGE_ANGLE` read into new `m_afSiegeAngle[4]` ( +0x97c4 ).
- **`UpNowMpPerHitOthers` `32c9b320`:** no natural force halving, no wrong-path assert; `GT_PVE` ( 4 ) counts as `GT_PVP`, dungeon / battle field x 0.8.
- **`HyperModeBuffEffectStart` `32c9b3d0`:** a transcended unit ( CX2Unit +0x3c ) or an awakening effect set ( `31fd66b0`, stand-in false ) returns before the four boost handles. CH0 routed the transcended return to CH3d ( 7ao ); it is written here with the function, CH3d only tests it.
- **Destructor `32ca42f0`:** Eradication loop effect stopped first, then March's order, then `KillMySummonedNPC( NUI_RENA_TARGETING_GUNGNIR, 3 )`; natural force lines under `_NF`. The `std::set` clear at +0x220 is a base member: not written. **ReInit `32ca4a40`:** MP floor 50 not in a PvP game; the natural force block under `_NF` ( buff at count >= 1, foot attack box x1.3, save-state, relate reset ); then the softbody tail ( `+0x405c > 0`: `SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_SELF_SOFTBODY, 1, NULL, 0, false )` = 7ai's five arguments, BTI 395's first Time finalizer set to the cool time left ) and `KillMySummonedNPC`.
- **The other 15 `inside` rows = March ( 13 functions + the two header inlines ):** InitEffect, Draw, Lose ( June's inlined `GetNowHp` = March's, 1 without a gage ), Win, COMMON_BUFF_FrameMove, InitDevice, AddUnitSlashData ( without the `UNIT_SLASH_TRACE_MANAGER_TEST` gate ), DeleteLireMajorParticle, DeleteMinorParticle, CreateMajor / MinorParticleArray, the two `MakeRoomFor*Trap` ( no ASSERT ).

**New ( `_CORE` ).** `ProcessAttackResultPassive` ( `32ca9ca0`, user vslot 0x1e8 ): `RNSI_P_RWS_PERSISTENT_MIND` learned, the hit's applied skill ( through `GetUpgradeBaseSkillID`, `32207210` ) is Reflex Magic / Nature Force and `m_fPersistentMindCoolTime` ( +0x97e8 ) <= 0: set 1.0, `ResetLeftSkillCoolTimeByBuff( ST_NONE, -1, false, BCT_FIX_VALUE, 1, true, 0 )`, the passive's buff factor. `ProcessEradicationHit` ( `32cb3200`, vslot 0x318 ): `DTT_ERADICATION` / `_EXPLOSION` hits count `m_usEradicationHitCount` ( +0x9810 ); crossing `SA_EFFECTIVE_COUNT` of `RNSI_SA_RNW_ERADICATION` plays `EffectSet_SA_RNW_Eradication_LoopFX` once ( handle +0x9818, time +0x9814 ). New file-local `TryGetAcquiredSkillAbilityValue` ( June `32007c40`, with the memo-swap argument ) and `IsTranscendedUnit` in `_CONST_RENA_CH1H_`.
**Enum names added** ( `JUNE_ENUM_NAMES`, checked against `Enum.src.lua` ): `RNSI_SA_RNW_ERADICATION` 23060, `RNSI_A_RWS_NATURE_FORCE` 23205, `RNSI_A_RWS_REFLEX_MAGIC` 23207, `RNSI_P_RWS_PERSISTENT_MIND` 23413 ( `X2SkillTree.h` ), `BTI_DEBUFF_SELF_SOFTBODY` 395, `BFI_DEBUFF_SELF_SOFTBODY` 424 ( `X2BuffFactor.h` ), `DTT_ERADICATION` 37, `DTT_ERADICATION_EXPLOSION` 38 ( `X2DamageManager.h` ).
**Routed.** CH1k: the constructor's stores ( list in its kickoff ). CH1i: `m_afSiegeAngle` is ready for `UpdateSpineAimRotation`. CH1j: `CommonFrameMove` counts `m_fPersistentMindCoolTime` down. CH1o: wire the two new virtuals ( 0x1e8 base is CH4b's, 0x318's caller is unlocated ). CH1q: the `_NF` lines written here.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_LIVE --with JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `.cpp` `PASS +711`, `.h` `PASS +12`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `flagcheck JUNE_ENUM_NAMES --worktree --files X2SkillTree.h X2BuffFactor.h X2DamageManager.h`: `RESULT: PASS` ( 3 `PASS~` ). `git diff --patience --numstat`: `.cpp` 711 / 0, `.h` 12 / 0, `X2SkillTree.h` 8 / 0, `X2BuffFactor.h` 10 / 0, `X2DamageManager.h` 6 / 0.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): `X2GULire_ElvenRanger.cpp` exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /EP` off / on: `TagEffectAlphaParticleHandle( this` 0 / 8, `GT_PVE` 0 / 1, `DT_BUFF != pDamageData->damageType` 0 / 1, `"SIEGE_ANGLE"` 0 / 1, `IsTranscendedUnit( GetUnit() )` 0 / 1, `StopEffectSet( m_hEradicationLoopEffect )` 0 / 1, `KillMySummonedNPC( ..., 3.0f )` 0 / 4, `SetNaturalForceCount( 0 )` 0 / 0 / 1 ( `_NF` ), the two `_CORE` functions 2 / 2.
- `junediverge.py --check` on the five files: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,102,272 B, 2026-11-09 11:25:01`; `DEPLOY OK: X2_offline.exe is 26,102,272 B, dated 2026-11-09 11:25:08, md5 b0237fcf9587ce45d22c3c60aa5e1823, equal to X2/US_SERVICE/x2.exe (2026-11-09 11:25:01); built at main 672250fd + 5 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string is reachable.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-rena CH1h review` ( Opus 5.5 high ), then CH1i.

### CH1h - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, with one fix to the `_CORE` helper.** I read the 33 rows against `charport_adis.py rena --phase CH1h`. The 10 differing bodies differ only as the Done note says. The "= March" rows I spot-checked match, and the 6 rows with nothing to write are as stated.
- **Fixed: `TryGetAcquiredSkillAbilityValue` ( June `32007c40` ).** June passes `322cc1a0` ( PvP or PvE, `@32007ca0` ) to `32206fb0`. The copy hardcoded `false` ( "false offline" ). It now passes `g_pX2Game->GetIsNowPVPCorrectionGame()`, as Elsword's copy does and as the junediverge idiom row requires. Nothing changes offline. Both callers are `ProcessEradicationHit`, which nothing calls yet.
- **Fixed comment: the destructor.** June's `+0x220` set clear ( `@32ca4619` ) is `DeleteTrapBlade`'s first line ( `m_SubAttackListSet.clear()`, June `32c96e50` ), inlined. It is not a base member left unwritten, so the call already covers it.
- **Destructor `32ca42f0`:** `+0x9818`, `+0x9678`, the seven minor handles `+0x9650`..`+0x9668`, the three `SAFE_DELETE_KTDGOBJECT` calls `+0x9630`..`+0x9638`, `DeleteLireMajorParticle`, then `31fefc00` + `+0x9764` ( = `DeleteLireMinorParticle`, one handle ). Then the spinning-kick mesh `+0x966c` and particles `+0x9670` / `+0x9674`, the `+0x962c` virtual delete, the trap blade, the evoke vector, prior planned blow ( stop, -1, `+0x96fc` = 0 = March's helper ), delayed firing, `+0x9600` ( 8-byte UIDs ), `+0x977c`, the NF block, and `KillMySummonedNPC( 0x823, 3.0 )`. June also zeroes `+0x9810` first; that is dead in a destructor, so it is not written.
- **ReInit `32ca4a40`:** `GetNowMp() < 50` ( `test ah, 5` `jp` ) and game type != 1 ( `GT_PVP` ) -> `SetNowMp( 50, 0 )`. The `+0x962c` inline is `ReInitFriendshipNature`. NF block: `IsMyUnit` -> sync count, buff `count >= 1`, `Lfoot` / `Rfoot` x1.3, `m_eSaveStateSkillId` = 0, relate byte `>= 10` -> 0. The `+0x405c > 0` test ( `test ah, 0x41` ) feeds vslot 0x21c with pushes `( 0x1a8, 0, 1, 0, 0 )`, which is June's ( BFI, creator, level, adjust, skip ) and equals March's `( BFI, 1, NULL, 0, false )` order. `0x18b` finalizer [0] goes through `dynamic_cast` to the time templet with the cool time left.
- **DamageReact `32c6da50`:** both percent-up tests `fldz; fcom; test ah, 1` = `<= 0`; `[edi] == 6` = `DT_BUFF` ( 7th member ). **ApplyRenderParam `32c7fa40`:** `+0x6840`, `+0x3e58`, type 3 = `RT_CARTOON`, `+0x8e9c` alpha, `+0xc74`, `+0x144` -> `+0xc44` = 1, `jmp 31fd4460` ( the stand-in base, as in Raven ).
- **InitEnchantWeaponEffectUnitSpecific `32c835c0`:** the compare tree decodes to 3003 / 4004 / 4546 / 4657 / 4768 -> 1, 3033 / 4044 / 4576 / 4687 / 4798 -> 2, 3303 / 4404 / 4846 / 4957 / 5068 -> 3, anything else -> 0. Each name's value matches `Enum.src.lua` and `X2DamageManager.h`.
- **InitComponent `32c895b0`:** with `IsMyUnit` and type 1, June runs `32a317d0( 1 )` + `32a36df0( 0 )` = `SetNaturalForceChanged( true )` + `SetNowNaturalForce( 0 )`, which is March's `SetNaturalForceCount( 0 )`. Six handles, `+0x3d0` = `UseLookPoint`, six tags `( handle, true )`. In `INIT_COMPONENT`: vslot 0x27c ( 0 ) / ( 1 ) = `AddUnitSlashData( STC_NONE / STC_RENA_NATURE_FORCE )`, then `SIEGE_ANGLE` 1-4 -> `+0x97c4`..`+0x97d0`. **SetRena*ParticleByEnum:** major tag `false` ( `@32c893d1` ), minor `true` ( `@32c89521` ).
- **UpNowMpPerHitOthers `32c9b320`:** the jump table at `32c9b3bc` reads `359 / 387 / 387 / 359`, so types 1 / 4 are plain and 2 / 3 are x0.8; `( add x 3 + hit ) [x 0.8] x rel`. **HyperModeBuffEffectStart `32c9b3d0`:** `[unit + 0x3c] != 0` or `31fd66b0` -> return before the four handles.
- **`32ca9ca0`:** base `32011b80` first ( left out until CH4b's base exists; `kickoffs/char-rena/CH1o.md` now says to add it ). `31fceab0( 0x5b75 ) <= 0` = `GetSkillLevelValue` ( only June's `int` overload is live, `cl /EP` ). `+0x5c4` -> `32207210`, 0x5aa7 / 0x5aa5, `0 < +0x97e8` returns. The 0x280 pushes decode to `( 0, -1.0, 0, 1, 1.0, 1, 0 )`, and `BCT_FIX_VALUE` = 1. `0x238( 0x5b75, 0, 1 )`, `0x224`. **`32cb3200`:** triggers 0x25 / 0x26. `32007c40( 2 = SA_EFFECTIVE_COUNT, 0x5a14, 1 )` around the `inc`. `fld limit; fcompp` with `test ah, 0x41` `jp` = `limit <= count`. A live instance returns. Then `( 1 = SA_EFFECTIVE_TIME )` into `+0x9814`. `PlayEffectSet( name, this, NULL, false, -1, -1, (1,1,1), false, 0, 0, 0, 1.0 )` = March's defaults.
- **"= March" spot-checks:** `COMMON_BUFF_FrameMove` ( `+0x24c < 0.75` + `31d28e10` = inlined `EventTimerOneshot`, camera ( 20, 0.3 ), white `ClearScreen`, 0.77 accelerator ), Win ( `+0x9714` -> `CreateTrapBlade`, 0x4b ), Draw ( 0x4b ), Lose ( 0x4c ). CH0 map corrections confirmed as the Done note lists.

**Gates.** `bytepatch.py` edit, 3 lines. `flagcheck ..._LIVE --with ..._CORE --range c30a6005^..c30a6005`: `.cpp` `PASS +711`, `.h` `PASS +12`, 3 OTHER ( `JUNE_ENUM_NAMES` ); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented-out `_LIVE` define. With the fix, `--worktree --base c30a6005^`: `PASS +712`, 0 deleted. `flagcheck JUNE_ENUM_NAMES --range`: 3 `PASS~`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check`: 0 A/B rows. One-file probe: exit 0, 0 errors off, `_LIVE`, and `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`, both before and after the fix.
`BUILD OK: X2/US_SERVICE/x2.exe 26,102,272 B, 2026-11-09 11:36:37`; `DEPLOY OK: X2_offline.exe is 26,102,272 B, dated 2026-11-09 11:36:45, md5 9a7cfefcd3c837ee20d969f4cddf3fdc, equal to X2/US_SERVICE/x2.exe (2026-11-09 11:36:37); built at main c30a6005 + 1 uncommitted code file(s).`

CH1i kickoff checked: present ( Sonnet 5.5 high, depends on this review, carries CH1h's `m_afSiegeAngle` amendment ). Nothing to play-test. Next: `/x2-phase char-rena CH1i` ( Sonnet 5.5 high ).

### CH1i - Done (2026-11-09)

All 8 class-map rows of phase CH1i are accounted for: **2 `inside` written** (June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`), **3 `new`** (`StoreGageSnapshot` and `UpdateSpineAimRotation` under `_CORE`, `ShownSkillCoolTime` a virtual under `_LIVE` as Chung's), **3 `inside` rows with nothing to write** (= March). `_LIVE` stays `//#define`. 8 funcmap rows `Char Rena CH1i`, 4 `ch1_offsets.tsv` rows. No `JUNE_BUGS` row: nothing here is a June mistake.
**CH0 map correction:** `32c881f0` is March's `SetSiegeModeEffectiveValue` ( listed `new` ).

| June | `CX2GULire_ElvenRanger::` | vs March |
|---|---|---|
| `32c6a920` | InitEquippedActiveSkillState ( 0x684 ) | = March: jmp to the base `31fd8f60`; nothing to write |
| `32c85970` | CanRidingState ( 0x628 ) | = March: a jump table over `( state - 13 )`; its ten zero entries ( 13 / 14 / 19 / 20 / 75 / 76 / 77 / 81 / 82 / 160 ) are WAIT, WALK, DASH, DASH_END, the five RIDING states and `LESI_CHARGE_MP` ( 160 ); nothing to write |
| `32ca5310` | HyperModeFrameMove ( 0x600 ) | = March: `CommonHyperModeFrameMove( 0.2, 0.75 )`; nothing to write |
| `32c881f0` | SetSiegeModeEffectiveValue | differs: Siege Mode's enhanced skill ( 23221 ) is the templet once learned ( `GetSkillLevel( id, true, 0, true ) > 0` ), else the base 23201; June's `GetSkillLevel( 23201 )` in the else arm is discarded and not written; the two abilities are March's |
| `32c886b0` | SetEquippedSkillLevel ( 0x7b8 ) | differs: 12 blocks and 4 loops keyed by June's `RNSI_` ids ( below ), 39 `SetEquippedSkillLevelStateData` sites; March's block per skill keyed by the `SI_` ids; no `ADD_RENA_SYSTEM` block in it |
| `32c7bed0` | ShownSkillCoolTime ( 0x6a8 ) new | Eradication ( upgrade base id `RNSI_SA_RNW_ERADICATION` ) shows 0 once `SA_EFFECTIVE_COUNT <= m_usEradicationHitCount`; else the argument |
| `32c852f0` | StoreGageSnapshot ( 0x740 ) new | own unit only; the base `32009780` ( Crest of Time HP / MP snapshot ) is not ported ( stand-in, as Chung's and Elsword's ); the natural force count into new `m_iSnapshotNaturalForceCount` ( +0x92b4 ), under `_NF` |
| `32c863d0` | UpdateSpineAimRotation ( 0x774 ) new | `GetNearestTargetAimDegree( SIEGE_ANGLE 0..3 )` into `m_vSpineAimDegree`; an all-zero aim copies `GetRotateDegree()` into the member only; the bone test ( `FLT_EPSILON < |y|` or `|z|` ), the yaw gap and `SetRotateBone( axis -1, 0, -1 )` / `EraseRotateBone( Bip01_Spine1 )` use the local aim; `m_fRotateBoneDegree` = local z. Twin of Chung's, without its distance factor |

**SetEquippedSkillLevel, block by block** ( `if ( bChangeAll || id )`; the callee is `SetEquippedSkillLevelStateData`, null templet = no-op as June's `3201f850` / vslot 0x7a8 ): 23207 Reflex Magic -> 199; 23009 Sharpfall -> 201 / 202; 23213 Entangle -> `m_MaxEntangleTrapCount` ( ability `SA_PROJECTILE_FIRE_COUNT` ), `m_fEntangleTrapAttackDuration` ( `SA_EFFECTIVE_TIME` ), memo `SMI_RENA_MEMO12` +1 ( `GetEqippedSkillMemo` is already June's `false` ); 23008 Violent Attack -> 203 / 204; 23202 Rising Falcon -> 227; 23212 Rapid Shot -> 240 / 241; 23216 Anger of Elf -> `m_MaxAngerOfElfTrapCount`; Siege loop ( 23201, 23221 ) -> 167-171 + `SetSiegeModeEffectiveValue( level )`; Hyper loops ( 23600 / 23603 -> 246 / 247, 23601 / 23604 -> 248 / 249, 23602 / 23605 -> 251 ); 23011 Slide Kick -> 216; 23204 Low Kick -> 295; 23015 Gungnir -> 283-294; 23059 Ace in the Hole -> 296-301; 23060 Eradication -> 302. Levels are `max( 1, GetSkillLevel( id, false, 0, true ) )` ( June's 4-argument form ). The ability reads use March's two-argument form ( `bPvp` false ). June's dirty-state batch ( `3207c1a0` / `31fdf970` ) is left out, as in Raven's, and the callee lacks the `+ 1.0` power-rate term ( CH1o ).
**Two ids are inferred, the review checks them:** `0xa7` = 167 = `LESI_SIEGE_READY` and `0x11b` = 283 = `LESI_RGA_GUNGNIR_READY`. Neither is registered by a literal id ( the READY states are slot states made in `32cb06d0` ); across the registered ids 120-302 ( `ch0_reg.tsv` + the listing's `reg` rows ) the only single gaps are 167 and 283, one before the siege WAIT run and one before the Gungnir CANCEL run, and 167 = March member 115 + 52 like WAIT / ATTACK / ATTACK_X / END ( 116-119 + 52 ). The ace states 298 / 300 are `UpdateUserUnitStateData( 0x12a / 0x12c, "LESI_SA_RGA_ACEINTHEHOLE_WAIT" )` ( the 296 table registered again ).
**Enum names added.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES`, values checked against `Enum.src.lua` ): `RNSI_SA_RWS_VIOLENT_ATTACK` 23008, `_SHARPFALL` 23009, `_SLIDE_KICK` 23011, `RNSI_SA_RGA_ACEINTHEHOLE` 23059, `RNSI_A_RCR_RISING_FALCON` 23202, `RNSI_A_RCR_LOW_KICK` 23204, `RNSI_A_RSR_RAPID_SHOT` 23212, `RNSI_HA_RWS_SPIRAL_STRIKE` 23600, `RNSI_HA_RGA_SHOOTER_DESTINY` 23601, `RNSI_HA_RLW_INNOCENT` 23602, and the three `_UPGRADE` 23603-23605. `X2GULire_ElvenRanger.h` ( `_CORE`, `LIRE_ELVENRANGER_STATE_ID`, after the earlier phases' members ): `LESI_SA_LCR_SHARPFALL_HYPER_LANDING` 202, `LESI_RGA_GUNGNIR_READY` 283, `LESI_SA_LCR_LOW_KICK` 295, `LESI_SA_RGA_ACEINTHEHOLE_WAIT` 296 / `_1ST_ATTACK` / `_WAIT_AFTER_1ST_ATTACK` / `_2ND_ATTACK` / `_WAIT_AFTER_2ND_ATTACK` / `_3RD_ATTACK` 301, `LESI_SA_RNW_ERADICATION_ATTACK` 302 ( C++ values follow the enum; registration is by script name, CH1m ). Rising Falcon's state keeps March's `LESI_SI_A_LE_RISING_FALCON_LANDING` ( June's table `LESI_A_LE_RISING_FALCON_LANDING` is the same state ).
**Routed.** CH1k: the constructor zeroes `m_iSnapshotNaturalForceCount` ( +0x92b4 ). CH1m: register the ten new names ( list in its kickoff ). CH1o: wire `StoreGageSnapshot` ( 0x740 ) and `ShownSkillCoolTime` ( 0x6a8 ), the `+ 1.0` term covers all 39 writes. CH1q: the `_NF` line in `StoreGageSnapshot` ( and 0x744's restore ). The checklist's `(Rena: CHn)` marks stay unwritten, as in CH1a-CH1h ( `ch0_checklist_rows.tsv` rows L258 and L987 go to this phase ).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_LIVE --with JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `.cpp` `PASS~ +216` ( 2 `//{{` marker lines outside the flag ), `.h` `PASS +15`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~ +17`, `RESULT: PASS`. `git diff --patience --numstat`: `.cpp` 216 / 0, `.h` 15 / 0, `X2SkillTree.h` 17 / 0.
- One-file probe ( `census.cl_cmd` flags, `/c /Zs /EHsc`, no PCH; `-D` not `/D` ): `X2GULire_ElvenRanger.cpp` exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /EP` off / on / on+NF: `RNSI_A_RENA_SIEGEMODE_UPGRADE, true, 0, true` 0 / 1 / 1, `SetEquippedSkillLevelStateData(` 8 / 42 / 42, `LESI_SA_RGA_ACEINTHEHOLE_WAIT_AFTER_1ST_ATTACK );` 0 / 1 / 1, `ShownSkillCoolTime(` 2 / 4 / 4, `m_iSnapshotNaturalForceCount = GetNaturalForceCount` 0 / 0 / 1, `UpdateSpineAimRotation()` 3 / 5 / 5.
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,102,784 B, 2026-11-09 12:08:41`; `DEPLOY OK: X2_offline.exe is 26,102,784 B, dated 2026-11-09 12:08:49, md5 cdf158a2cd19f7a785e327f590494136, equal to X2/US_SERVICE/x2.exe (2026-11-09 12:08:41); built at main c80ca139 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string is reachable.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). CH1j kickoff checked: present ( Sonnet 5.5 high, depends on this review ). Next: `/x2-phase char-rena CH1i review` ( Opus 5.5 high ), then CH1j.

### CH1i - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** I read the 8 rows against `charport_adis.py rena --phase CH1i`. Every body matches June as the Done note says.
- **SetEquippedSkillLevel `32c886b0`:** the skill tests run in the kickoff's order ( 0x5aa7 ... 0x5a14 ), and the hyper loop vectors are built from 0x5c30 / 0x5c33, 0x5c31 / 0x5c34 and 0x5c32 / 0x5c35. The 39 state pushes ( 199; 201 / 202; 203 / 204; 227; 240 / 241; 167-171; 246 / 247; 248 / 249; 251; 216; 295; 283-294; 296-301; 302 ) all resolve by name through `ch0_skill_listing_rows.tsv` / `ch0_reg.tsv` to the C++ members used. 227 is March's `LESI_SI_A_LE_RISING_FALCON_LANDING`, which March already registers as `"LESI_A_LE_RISING_FALCON_LANDING"` ( `.cpp:3124` ). In the siege loop, the level comes from `GetSkillLevel( m_eID, 0, 0, 1 )` floored at 1, and five vslot 0x7a8 calls come before `SetSiegeModeEffectiveValue( level )`. The Entangle templet is null-tested. Ability 0x1e goes through `_ftol2` into the protected int at `+0x95d4`, and ability 1 goes into the float at `+0x95d0`. Memo `0x93384` = 603012 = `SMI_RENA_MEMO12` adds 1. Anger of Elf: ability 0x1e goes into `+0x95f4`. `GetSkillAbilityValue` takes a literal `0` for PvP, so March's two-argument form is right here.
- **Inferred ids confirmed.** 167: `Enum.src.lua:7774` has `LESI_SIEGE_READY = 59` immediately before `LESI_SIEGE_WAIT = 60` ( 168 - 60 = 108; 59 + 108 = 167 ). March's own `SetEquippedSkillLevel` writes `LESI_SIEGE_READY` in the same place ( `.cpp:5345` ). 283: a skipdata sweep of the class range `32c6a8d0`-`32cb6c6d` finds `push 0x11b` / `push 0xa7` only at the two `SetEquippedSkillLevel` sites. That fits a READY slot state ( registered through the slot head ) sitting right before its first registered state, which is the siege pattern. I accept `LESI_RGA_GUNGNIR_READY`.
- **Member layout:** `+0x95ac` ( protected float, 12 B ) -> `+0x95b8` ... `+0x95d0` / `+0x95d4` ( protected int ) -> `+0x95f4` -> `+0x9610` follows March's declaration order ( `.h:706-728` ): `m_fMPConsumeRateSiegeArrow`, `m_fDamageRelSiegeArrow`, five fields, `m_fEntangleTrapAttackDuration`, `m_MaxEntangleTrapCount`, vector, `m_iDoubleJumpCount`, `m_MaxAngerOfElfTrapCount`, vector, `m_fSiegeDamageByPassive`. `SA_` values from `X2SkillTree.h:2588`: `EFFECTIVE_TIME` 1, `EFFECTIVE_COUNT` 2, `DAMAGE_REL` 0xf, `MP_CONSUME_REL` 0x1b, `PROJECTILE_FIRE_COUNT` 0x1e.
- **SetSiegeModeEffectiveValue `32c881f0`:** if `GetSkillLevel( 0x5ab5, 1, 0, 1 ) > 0`, June uses `GetSkillTemplet( 0x5ab5 )`. Otherwise it calls `GetSkillLevel( 0x5aa1 )`, discards the result, and uses `GetSkillTemplet( 0x5aa1 )`. Then it applies `0xf x +0x9610 -> +0x95b8` and `0x1b -> +0x95ac`, the last only when the value changes ( the `KProtectedType` assign ).
- **UpdateSpineAimRotation `32c863d0`:** the aim comes from `32008660( +0x97c4..+0x97d0 )` and is written into `+0x97d4..dc`. The all-zero test ( `test ah, 0x44` x3 ) copies vslot 0x150 into the member only. `eps < |0 - y|` and `eps < |0 - z|` read the local aim. The yaw gap is `rot.y - local y`; `360 <= gap -> 360 - gap` and `gap <= -360 -> gap + 360`, stored as float after each step ( the same wrap as March's Chung, so no `JUNE_BUGS` row ). `SetRotateBone( 1, Bip01_Spine1, axis ( -1, 0, -1 ), degree ( gap, 0, local z ) )`; `+0x8a68` = local z on both paths. The angles are passed as they are.
- **ShownSkillCoolTime `32c7bed0`:** `32207210` = `GetUpgradeBaseSkillID` ( `X2SkillTree.h:3614` ) `== 0x5a14`. `32007c40( 2, id, 1, &limit )` matches the helper's `( type, id, bMemoSwap, out )`. `fld limit; fcompp; test ah, 0x41; jp` returns 0 when `limit <= count`; the two differ only on NaN. **StoreGageSnapshot `32c852f0`:** vslot 0x198 = own unit ( funcmap `32009780` ), then base call, then gauge `+0x114` -> `+0x92b4` ( `_NF` ).
- **"= March":** `InitEquippedActiveSkillState` = `jmp 31fd8f60` ( base call ). `HyperModeFrameMove` = `CommonHyperModeFrameMove( 0.2, 0.75 )` ( March's `MODIFY_RIDING_PET_AWAKE` arm ). `CanRidingState`: decoding the byte table at `32c859c4` gives zero entries for exactly 13 / 14 / 19 / 20 / 75 / 76 / 77 / 81 / 82 / 160.
- **Names:** the 13 `RNSI_` values match `Enum.src.lua` ( lines 14490-15953 ). The 10 `LESI_` members and the 13 ids compile once each ( a duplicate would fail the probe ).

**Gates** ( range `b9148088^..b9148088` ). `flagcheck ..._LIVE --with ..._CORE`: `.cpp` `PASS~ +216`, `.h` `PASS +15`, `X2SkillTree.h` OTHER; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only for the commented-out `_LIVE` define. `flagcheck JUNE_ENUM_NAMES`: `PASS~ +17`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check`: 0 A/B rows. One-file `/Zs` probe: exit 0 and 0 errors with flags off, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No build: no code changed, so CH1i's `DEPLOY OK` ( md5 `cdf158a2...` ) stands.

CH1j kickoff checked: present ( Sonnet 5.5 high, depends on this review ). Nothing to play-test. Next: `/x2-phase char-rena CH1j` ( Sonnet 5.5 high ).

### CH1j - Done (2026-11-09)

All 10 class-map rows of phase `CH1j` are accounted for: **4 `inside` written** (June's body in March's own `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's in `#else`), **3 `new`** (`_CORE`), **3 `inside` rows with nothing to write** (= March). `_LIVE` stays `//#define`. 10 funcmap rows `Char Rena CH1j`, 9 `ch1_offsets.tsv` rows. No `JUNE_BUGS` row: nothing here is a June mistake (`m_fPersistentMindCoolTime` runs on below 0 without a clamp, as June; harmless).

| June | `CX2GULire_ElvenRanger::` | kind | vs March |
|---|---|---|---|
| `32c6a930` / `32c6a940` / `32c6a950` | CommonStateStartFuture / CommonStateEndFuture / CommonRender_Prepare | inside | = March: a `jmp` to the base ( `320506f0` / `31fdd8e0` ) and the base call that returns the hint ( `31fd07b0` ); nothing to write |
| `32c87990` | CommonFrameMoveFuture | inside, written | one new first line: `m_fAceInTheHoleTime` ( +0x980c ) `+=` the frame time; the rest is March's ( the NF sync under `_NF` ) |
| `32ca5060` | CommonStateEnd | inside, written | the base; +0x981c set -> `DeleteTrapBlade`; `m_bBowSkillState` set and Random Shot Pleasure ( 23415 ) learned -> `SetSkillBuffFactors( 23415, 1, true )` |
| `32ca81b0` | CommonStateStart | inside, written | see below |
| `32ca8bf0` | CommonFrameMove | inside, written | see below |
| `32ca2aa0` | ResetEradicationLoop | new | hit count 0, stop the loop effect set ( callers: CommonFrameMove, the constructor and ReInit ) |
| `32c6d800` / `32c83280` | GetPredatorDamageMultiple / GetPredatorDamageStage | new | Predator ( 23417 ): the multiple of stage 0..3 ( `SA_SKILLDAMAGE_MULTIPLE_01..04` = 0x73-0x76, 1.0 elsewhere ); the stage a roll falls in ( running sum of `_01_RATE.._03_RATE` = 0x77-0x79, roll `<` sum, else 3 ). The abilities' values were checked by a `cl` probe ( 115-118, 119-122 ) |

**CommonStateStart `32ca81b0`**, June's order (the five xor key tables read by their addresses and sizes: 15 / 22 / 23 / 23 / 17 bytes = `BOW_SKILL_STATE`, `DAMAGEDATA_CHANGE_TIME` / `1` / `2`, `CREATE_TRAP_BLADE`): `BOW_SKILL_STATE` into `m_bBowSkillState` ( +0x981d ); the three change times ( +0x9698 / +0x969c / +0x96a0, miss 0 ); +0x981c cleared, `CREATE_TRAP_BLADE` read into it, set -> `CreateTrapBlade`; **under `_NF`** a state with `m_SPLevel > 0` whose skill id differs from `m_eSaveStateSkillId` passes `SetNowDamageRelateSkillData( templet +0x10 )` unless the type is `ST_ACTIVE` ( templet +0x10 is the low byte of `m_usFPConsumtion`: every Rena skill row has 1 / 2 / 3 or 0 ), then saves the id ( March's MP-derived count and MP charge are not in June ); the base; Sharp Kick ( 23400 ) scales `fForceDownValue` by `SA_FORCE_DOWN_MELEE_REL` on a melee non-special hit; Hunter's Ability ( 23408 ) by `SA_FORCE_DOWN_REL` while `m_bUseTrapBlade`; **new:** Predator learned and the state skill's upgrade base is Seed of Ruin ( Pole ) / Rage Fungus / Call of Ruin: `GetPredatorDamageStage( GetRandomFloat( 0 ) )`, `m_fNowStatePowerRate *=` the multiple ( checklist §7 l.256: June's `*=` ), `m_bPredatorCritical` = stage 0; **new:** my unit clears the camera stand-ins unless the state skill is Ace in the Hole ( 23059 ). June's dead empty local `std::wstring` in the Predator block is not written.

**CommonFrameMove `32ca8bf0`**, June's order: the base; `m_usRandomRollCount` ( +0x981e ) = 0; the Eradication loop time counts down and at `<= 0` becomes 0 and calls `ResetEradicationLoop`; **new** an upgrade of Reflex Magic ( 23207 ) / Mind of Sharpshooter ( 23210 ) ( `GetEquippedActiveSkillTemplet( id, true )` with `m_iDeleteSkillIDByUpgrade != 0` ) sets `bSuperArmor` true and `bSuperArmorNotRed` false; the NF lines ( non-own unit syncs the count, `UpdateNaturalForceEffect` ) under `_NF`; MP charge A / B / C, friendship `OnFrameMove`, trap blade matrix and the prior planned blow timer = March; **new** `EndGungnirTargeting()` ( CH1f2's, routed ) unless the base id is Gungnir ( 23015 ); `m_fPersistentMindCoolTime -=` the frame time ( CH1h's member, no clamp ); **new** every value of the +0x97f4 map `-=` the frame time.

**New members** ( `_CORE`, `.h` ): `m_bBowSkillState` +0x981d, `m_bCreateTrapBlade` +0x981c, `m_fAceInTheHoleTime` +0x980c ( reset by Ace in the Hole's StartFuture `32c6d8f0`, read by its wait states: CH3b ), `m_bPredatorCritical` +0x97f0, `m_usRandomRollCount` +0x981e ( `32c7af80` counts it: CH4a ), `m_mapRageFungusTargetLockTime` +0x97f4 ( **inferred type** `std::map< std::pair< int, UidType >, float >`: key compare order in `32cb57b0` is the game unit type then the 64-bit uid, the value sits at node +0x20, `32c86790` sets 1.0 on a Rage Fungus hit target and `32c85b20` erases the expired ones ), and two stand-ins `m_bSkillCameraOverride` +0x8d90 / `m_fSkillCameraOverrideValue` +0x8da4 ( June's `CX2GUUser` camera group; Ace in the Hole's start sets 1 / 550.0 and +0x8d94 = 220.0; **no reader found** ). Enum names ( `X2SkillTree.h`, `JUNE_ENUM_NAMES`, values from `Enum.src.lua`; March's `SI_` ids for the same skills are 3400-range, so June's ids need their own names ): `RNSI_SA_RNW_CALL_OF_RUIN` 23022, `_SEED_OF_RUIN_POLE` 23041, `_RAGE_FUNGUS` 23042, `RNSI_P_RCR_SHARP_KICK` 23400, `RNSI_P_RTR_HUNTERS_ABILITY` 23408, `RNSI_P_RGA_RANDOM_SHOT_PLEASURE` 23415, `RNSI_P_RNW_PREDATOR` 23417. `Always.h`: three more `_LIVE` guards ( `EVE_PASSIVE` for `SetSkillBuffFactors`, `ENUM_NAMES`, `DIVERGE_SKILL_LEVEL_VALUE`; all three `ON` by a `cl /P` probe that includes `stdafx.h` ).

**Drift, routed (scope rule).** The skill-camera group has no reader: Stage 9's camera bullet in `JUNE_PORT_BACKLOG_ORDER.md` now names the group and the Rena writers. **Routed to later phases (kickoffs amended):** CH1k the constructor stores ( 6 members, the `ResetEradicationLoop` call @32ca42ab ); CH1o the sweep `32c85b20`; CH3b the Ace in the Hole members; CH4a the readers and the `32c86790` filler.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_LIVE --with JUNE_CHAR_RENA_CORE JUNE_CHAR_RENA_NF --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `.cpp` `PASS~ +296` ( 2 `//{{` marker lines outside the flag ), `.h` `PASS +11`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is commented out. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`. `git diff --numstat`: `.cpp` 296 / 0, `.h` 11 / 0, `X2SkillTree.h` 11 / 0, `Always.h` 9 / 0.
- One-file probe ( `census.cl_cmd` flags, `/c /Zs /EHsc`, no PCH; `-D` ): `X2GULire_ElvenRanger.cpp` exit 0, 0 errors with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. `cl /EP` off / on / on+NF: `"BOW_SKILL_STATE"` 0 / 1 / 1, `m_fAceInTheHoleTime +=` 0 / 1 / 1, `m_mapRageFungusTargetLockTime.begin()` 0 / 1 / 1, `SetSkillBuffFactors( ...RANDOM_SHOT_PLEASURE` 0 / 1 / 1, `m_bSkillCameraOverride = false` 0 / 1 / 1, `SetNowDamageRelateSkillData( static_cast<Byte>` 0 / 0 / 1, `UpdateNaturalForceEffect();` 0 / 0 / 2 ( one is March's dead call ), `ResetEradicationLoop()` 2 / 3 / 3 ( the `_CORE` definition and declaration in every build ).
- `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,103,296 B, 2026-11-09 13:05:59`; `DEPLOY OK: X2_offline.exe is 26,103,296 B, dated 2026-11-09 13:06:06, md5 7b0303264da7b32c3b5d451cd91b128f, equal to X2/US_SERVICE/x2.exe (2026-11-09 13:05:59); built at main 0a415667 + 4 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and no string is reachable.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). CH1k kickoff amended ( Sonnet 5.5 high, depends on this review ). Next: `/x2-phase char-rena CH1j review` ( Opus 5.5 high ), then CH1k.

### CH1j - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** I read the 10 rows against `charport_adis.py rena --phase CH1j`. Every body matches June as the Done note says. One June quirk was found in the inferred map type: JUNE_BUGS 569.
- **CommonStateStart `32ca81b0`:** a script replayed the five init-then-xor blocks from the listing and decoded `BOW_SKILL_STATE` ( 15 ), `DAMAGEDATA_CHANGE_TIME` ( 22 ), `..._TIME1` ( 23 ), `..._TIME2` ( 23 ) and `CREATE_TRAP_BLADE` ( 17 ), in that order. The stores are +0x981d, then +0x9698 / +0x969c / +0x96a0 ( `.h:798-802` order; each miss `fldz`; the bool misses store false ), then +0x981c cleared before its read. `_NF`: `m_SPLevel` is a signed byte `> 0`, the id is compared with +0x9758, `GetSkillTemplet( id, 1 )`, `[t+4] != 6`, and `movzx [t+0x10]` goes into `31fe28f0`. The id is saved even when the templet is null, as written. March's `SkillTemplet` has `m_eType` +4 and `m_usFPConsumtion` +0x10 ( after `m_bGuildSkill`, `m_eID`, `m_usCBConsumption`, `m_usFPGainCount` ). June's `ST_ACTIVE = 6` ( `Enum.src.lua:7134` ). June's row for Rage Fungus has `m_usFPConsumtion = 2` ( "Use up to 2 NF" ). Sharp Kick: `GetSkillLevel( 0x5b68, 1, 0, 1 )`, then the templet, `+0x3cc != 0`, `+0x3a8 != 1` ( `AT_SPECIAL` = 1 ), then ability 0x28 `fmul` +0x790. Hunter's Ability: the `+0x96a4 == 1` test comes first, then level, templet and 0x27. Predator: `31fceab0` = `GetSkillLevelValue`, then `32207210` = `GetUpgradeBaseSkillID` in { 0x5a01, 0x5a02, 0x59ee }, then vslot 0x1cc `( 0 )`, stage, multiple. The protected +0x7cc8 is written only when the product differs ( the `*=` ). +0x97f0 is set to 0 and then to 1 on stage 0, so `= ( 0 == stage )`. The `std::wstring::assign( L"" )` and its free are dead. Tail: vslot 0x198, then `!= 0x5a13` ( 23059 ) clears +0x8d90 ( byte ) and +0x8da4 ( 0.f ).
- **`32c83280` / `32c6d800`:** `32007b10( type, 0x5b79 )` = `GetLearnedSkillAbilityValue`. The multiples are 0x73-0x76, `default` returns `fld1`. The rates are 0x77 / 0x78 / 0x79. The compares are `fcompp` / `fcomp st(1)` / `fcomp qword`, each `test ah, 0x41`. `jne` / `je` gives `roll < sum` for stages 0 / 1 / 2, else 3. The third sum is rounded to float into the arg slot before the compare, the same as the C++ `float`. `32c83320` is the next function.
- **CommonFrameMove `32ca8bf0`:** the base comes first, then +0x981e = 0 ( stored between the `fcom` and its test ). Eradication: `0 < t`, `t -= dt`, `t <= 0` sets 0 and calls `32ca2aa0`. Super armor: `GetEquippedActiveSkillTemplet( id, 1 )`, `[t+0x178] != 0` ( the field `GetUpgradeBaseSkillID` reads: `m_iDeleteSkillIDByUpgrade` ), the base id `- 0x5aa7`, `- 3`. The protected +0x7ff4 is written to 1 only if it is not already 1, then +0x8040 = 0. NF: `!IsMyUnit` calls `32a317d0( 1 )` + `32a36df0( +0x8160 )` = March's `SetNaturalForceCount` ( `SetNaturalForceChanged( true )`, `SetNowNaturalForce` ). The +0x9754 test followed by `SetNaturalForceEffect` = `UpdateNaturalForceEffect`. MP charge, friendship ( +0x962c `OnFrameMove( m_fTime, dt )` ), the trap blade ( `+0x374 = 1`, 16 dwords from frame +0x90 = `SetUseDXMatrix` ) and the prior planned blow ( `0 < t`, `t <= 0` ) = March. `!= 0x59e7` calls `EndGungnirTargeting`. +0x97e8 `-=` dt ( no clamp ). The map walk starts at header +0x97f8, steps `[esi]` to the header with `node+0x20 -= dt`, and increments with `322a8c30`.
- **The inferred map type.** `32cb57b0` is `find` ( lower_bound `32cb4290`, then `!( key < node )` ), and the filler `32c86790` builds the key as { `GetUnitUID()` ( vslot 0x54, 64-bit ) at +0, the target's `m_GameUnitType` ( +0x264 ) at +8 }. So June's key is uid-first in memory, and the compare is type first ( `jl` ), then the signed 64-bit uid ( `jl` / `jg` high, `jb` low ). **Both functions lack the reverse type test:** June's `<` is `type < type' || uid < uid'`, which is not a strict weak order once the types differ. In PvE every Rage Fungus target is `GUT_NPC`, so the compare reduces to uid order. There, `std::pair< int, UidType >` gives the same equality and the same iteration order. The port keeps the pair, and JUNE_BUGS 569 records why June's compare is not copied ( a broken compare is undefined behaviour in `std::map` ). The value at node +0x20 fits the VC node layout for a 16-byte key. The CH4a kickoff now says how to build the key.
- **CommonFrameMoveFuture `32c87990`:** `+0x980c += dt` comes first. +0x959c: `0 < t` fails, so it stores 0 ( `fcom` / `test 0x41` ). +0x95a4: `< 0` sets 0 and `m_JumpXCount` = 0. `bFootOnLine == 1` with `m_iDoubleJumpCount` gives `IsOnSomethingFuture`. The NF sync is protected +0x7ec0 <- gage +0x114. The tail calls the base. **CommonStateEnd `32ca5060`:** the base, then +0x981c and `DeleteTrapBlade`, then +0x981d, `31fceab0( 0x5b77 ) > 0` and vslot 0x23c `( 0x5b77, 1, 1 )` = `SetSkillBuffFactors( iSkillID_, iIndex_, bCheckLearned_ )`. **`32ca2aa0`:** the word +0x9810 = 0. `push 0` + `push &+0x9818` are `321dafb0`'s ( `StopEffectSet( Handle&, bool )` ) arguments; `GetEffectSet` takes none. `call [344c0044]` is the static-guard artefact ( Chung CH note ), not ported.
- **"= March":** `32c6a930` / `32c6a940` are `jmp`s to the base, and March's bodies ( `.cpp:5578`, `:6100` ) are the base call. `32c6a950` returns the base's hint, as March does ( `.cpp:6141` ).
- **Names:** the 7 new `RNSI_` values ( 23022 / 23041 / 23042 / 23400 / 23408 / 23415 / 23417 ) and the 4 used from earlier phases ( 23059 / 23207 / 23210 / 23015 ) match `Enum.src.lua`. The camera stand-ins and the CH1k / CH1o / CH3b / CH4a amendments say what the Done note says.

**Gates** ( range `50d202f2^..50d202f2` ). `flagcheck ..._LIVE --with ..._CORE ..._NF`: `Always.h` `PASS +9`, `.cpp` `PASS~ +296`, `.h` `PASS +11`, `X2SkillTree.h` OTHER; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the `_LIVE` define is commented out. `flagcheck JUNE_ENUM_NAMES`: `X2SkillTree.h` `PASS~ +11`, same zero counts. `RESULT: FAIL` comes from one `ERROR` on `Always.h:10175` ( a compound `#if` from `70db3b35c`, 2026-10-26, outside this commit ). `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check` on the class files: `0 A/B row(s)`. The one-file `/c /Zs /EHsc` probe returned 0 errors in all three runs: off, with `_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. No build: the review changes no code.

CH1k kickoff checked: present ( Sonnet 5.5 high, depends on this review ). Nothing to play-test. Next: `/x2-phase char-rena CH1k` ( Sonnet 5.5 high ).

### CH1k - Done (2026-11-09)

All 21 class-map rows of phase `CH1k` plus `ReservedSkillStart` are accounted for: **15 written** (`inside`: `SpecialAttackEventProcess`, `SpecialAttackNoStageChange`, `NoStateChangeActionFrameMove`, `GetSkillUseCondition` ( listed `new` ), the constructor; `new`: the lookup / use / cooldown ( `_CORE` ), `TryStateChangeEradicationAttack` ( `_CORE` ), three file-local passive helpers ( `_CORE` ), `IsNotCommonMoveState` / `AfterImageFrameMove` / `GetStateAnimSpeedRate` / `ReservedSkillStart` ( `_LIVE`, they override base virtuals )) and **6 equal to March** ( `32c86af0`, `32c9ac00`, `32c9af70`, `32cb5b70`, `32cb5cb0`, `32cb5ee0` ). `_LIVE` stays `//#define`. 22 funcmap rows `Char Rena CH1k`, 9 `ch1_offsets.tsv` rows. **JUNE_BUGS 570** ( Delayed Fire: lookup and use test the legacy id 3227, the skill is 23217 ).

| June | `CX2GULire_ElvenRanger::` | kind | vs March |
|---|---|---|---|
| `32c6a970` | SpecialAttackEventProcess | inside | June's: lookup, use + `Send_SKILL_USE_REQ`; the **templet's** use condition 3 / 5 ( as Elsword's ) gives the chosen values back |
| `32cac200` / `32ca50b0` / `32c89a20` | SpecialAttackLookup / Use / Cooldown | new | the checks, the use and the spending March had in one body |
| `32ca2af0` | TryStateChangeEradicationAttack | new | Eradication ( base 23060 ) with `SA_EFFECTIVE_COUNT` <= hit count: `ResetEradicationLoop`, state `LESI_SA_RNW_ERADICATION_ATTACK` |
| `32c6d480` | GetSkillUseCondition | inside | Fatality ( 3015 ) is always `ASUT_AIR` ( March: only with memo 22 ) |
| `32c6d700` / `760` / `790` | IsPersistentMindAirSkill / IsRandomShotPleasureAirSkill / IsPredatorAirSkill | new | upgrade base ids: Crescent Kick ( + upgrade ) and Violent Attack / Backjump Shot ( + upgrade ) / Rosebush Trap, Seed of Ruin, Delayed Fire, Call of Ruin |
| `32c87ac0` | SpecialAttackNoStageChange | inside | the **upgrade base id** 23217 ( March: the id itself ) with Delayed Fire active sets action `UAI_START_OF_DELAYED_FIRING` |
| `32ca5160` | NoStateChangeActionFrameMove | inside | base `32008e40`, action 0x10 as March but the cool time goes to 23217, **new action 0x28**: buff factors 0 and 1 of skill 23402 ( `_CONST_RENA_CH1K_::UAI_SOFT_BODY_BUFF_FACTORS` ) |
| `32c83320` | GetStateAnimSpeedRate ( 0x824 ) | new `_LIVE` | `SA_ANI_SPEED` of skill **23402** for classes 12 / 102 ( March read 3401 inline in `ResetAnimSpeed`; that block is skipped under `_LIVE` ) |
| `32c85a60` / `32c85c00` | IsNotCommonMoveState / AfterImageFrameMove | new `_LIVE` | base test less 6 states ( June 154 / 155 / 160 / 231-233 ); after-image suspended in `USI_WAIT` / `LESI_CHARGE_MP` ( second after-image not ported ) |
| `32ca2b60` | constructor | inside | `_CORE` stores for CH1f2 / CH1h / CH1i / CH1j members + `m_bAfterImageSaved`; `_LIVE` `ResetEradicationLoop()`, `m_fDashJumpDownAccelRate` 1.1, `m_fDashJumpSpeedRate` 1.3 |

**Where June differs from March:** the lookup uses `ShownSkillCoolTime` for the cool-time test ( Eradication ), `CheckSummonSpecialAttack`, no skill-tree-learning message ( 25110 ), no `CountUsedSkill`, no infinity-essence term; MP by the 2-argument virtual ( = June's call with `true` ). Cooldown: Siege Mode and Delayed Fire ( by base id ) never set their cool time here; a cool time of -1.0 ( `IsSamef` ) is not set; the memo cool-time cuts are gone ( CH4b ). Use: with Delayed Fire active and id 3227 nothing happens ( JUNE_BUGS 570 ).
**Equal to March, checked:** `EntireExplosion` ( the `FIX_DELAYED_FIRING_DELAY_ADD_DAMAGE` branch ), `CreateEntireExplosionDataVector` ( offset table, READY types 1 -> 7 / 3 -> 8, 0.01 + 0.1 s ) and its ground walk ( switch cases 2 / 4 / 6 / 10 / 12 / 14 = the six `DET_*_GROUND` values by `enumprobe` ), the cut-in call ( `Dummy1_Rhand`, `+0x8acc` ), `32cb5ee0` ( the map's default constructor ).
**Constructor, unmapped:** `+0x973c` = 7 and the protected `+0x9768` = 5.5 have no member; routed to CH1q ( kickoff amended ). Named stores equal March ( `m_hTrapBlade`, change times, `m_bIsSiegeSkill`, prior planned blow and delayed firing groups, throw values 0.6666 / ( 40, 130, 0 ) / ( 1600, 200, 0 ) ); `+0x9610` 1.0 and `+0x9768` 5.5 are matched to March's members by value ( inferred ).
**Other files:** `X2GUUser.h` / `.cpp`: `ReservedSkillStart` declaration and call chains widened with Rena's `_LIVE`; the `CommonSpecialAttackEventProcess` stand-in no longer treats `UT_LIRE` as March's slot index space; `ResetAnimSpeed` skips March's Agile Movement block under `_LIVE`. `Always.h`: Rena `_LIVE` now needs `ARME_CORE`, `ARME_LIVE`, `CHUNG_LIVE`, `ELSWORD_LIVE`, `SKILL_TEMPLET`, `USER_SKILL_USE_HELPERS` ( all defined above it; the `_LIVE` probe passes ). `X2SkillTree.h` ( `JUNE_ENUM_NAMES`, values from `Enum.src.lua` ): `RNSI_SA_RCR_CRESCENT_KICK` 23005, `_UPGRADE` 23031, `RNSI_A_RENA_BACKJUMP_SHOT` 23200, `_UPGRADE` 23220, `RNSI_A_RTR_ROSEBUSH_TRAP` 23215, `RNSI_A_RNW_DELAYED_FIRE` 23217, `RNSI_A_RNW_SEED_OF_RUIN` 23219, `RNSI_P_RWS_SOFTBODY` 23402 ( 3227 uses March's `SI_A_LNW_DELAYED_FIRING` ).
**Routed ( kickoffs amended ):** CH1m ( registers nothing; ids by name ), CH1o ( do not repeat the `ResetAnimSpeed` change ), CH1q ( two stores ), CH4a ( find the writer of action 0x28; 23402 vs 3401 ). Open for the review: June's 0x28 writer is unknown, so the soft body action never fires until found.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_LIVE --with JUNE_CHAR_RENA_CORE JUNE_CHAR_RENA_NF --worktree --files` ( Lire `.cpp` / `.h`, `X2GUUser.cpp` / `.h`, `Always.h` ): `PASS +428` / `+9` / `+22` / `+13` / `+18`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `flagcheck JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `PASS~ +12`, `RESULT: PASS`.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH, `-D` ): `X2GULire_ElvenRanger.cpp` exit 0 with `_LIVE` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`; `X2GUUser.cpp` exit 0 off and with `_LIVE`. `cl /EP` off / on: `SpecialAttackLookup` 4 / 5, `m_fDashJumpSpeedRate = 1.3f` 0 / 1, `UAI_SOFT_BODY_BUFF_FACTORS` 1 / 2, `GetStateAnimSpeedRate` 2 / 4 ( Lire ), `SI_P_LCR_AGILE_MOVEMENT` 3 / 1, `UT_LIRE != GetUnit()->GetType()` 0 / 1 ( `X2GUUser.cpp` ).
- `junediverge.py --check` ( the five code files ): `allow-listed: 7`, `RESULT: PASS`. `enumscan.py --check` ( Lire `.cpp` / `.h` ): `0 A/B row(s)`.
- `BUILD OK: X2/US_SERVICE/x2.exe 26,105,856 B, 2026-11-09 13:57:06`; `DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-11-09 13:57:13, md5 2d68e720f440229516a62b9bbb6d3805, equal to X2/US_SERVICE/x2.exe (2026-11-09 13:57:06); built at main 742470a1 + 6 uncommitted code file(s).` No `exestr` line: `_LIVE` is off.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-rena CH1k review` ( Opus 5.5 high ), then CH1m.

### CH1k - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** I read the 21 rows and `ReservedSkillStart` against `charport_adis.py rena --phase CH1k`. A subagent ran the gates.
- **Lookup `32cac200`:** the order matches the code. Quick-slot wait `+0x8d78` ( `test ah, 5` / `jnp` ), then `CommonSpecialAttackEventProcess` ( slot data `[esp+0x3c]`, index `[esp+0x40]` = -1 ), templet `( id, 1 )`, id != slot -> 0x8bae. `+0x8ea0` is stored @32cac426. Delayed Fire: `0xc9b` + `+0x9700` == 1 + argument != 3 -> 1 ( `ASUT_ANYTIME` 3 -> `ASUT_GROUND` 1 ). `GetSkillUseCondition`, then 23413 / 23415 / 23417 ( `31fceab0` = `GetSkillLevelValue` ) with their helpers -> 2 ( `ASUT_AIR` ). Then `CheckSkillUseCondition`, `3202daf0`, `3202cba0`, `3202d140`, `3202d3c0`, riding wait `329cc370`, `GetSkillLevel( id, 0, 0, 1 )` clamped to 1, vslot 0x6a0 `( id, level, 1 )`, vslot 0x82c ( `bSkillFree`, the same as Raven's ). `ShownSkillCoolTime( slot +0x24, id )` > 0 -> 226. Reserved + chosen > now MP -> 2549 ( `test ah, 0x41` ). Type 13, not training, stone `0x1fca0` <= 0 -> 26119. Success writes the id and `[esp+0x40]`. The `PushText` order is font 3, string, white, black outline, `DT_CENTER`, 1.0, 1.0.
- **Helpers:** `32c6d700` { 23005, 23031, 23008 }, `32c6d760` { 23200, 23220 }, `32c6d790` { 23215, 23219, 23217, 23022 }, all by upgrade base ( `32207210` ). `32c6d480`: 3015 -> 2, else templet `+0x98`. The enum values match the code's names.
- **Event process `32c6a970`:** saves `+0x8ea8` / `+0x8ea4` / `+0x8ea0`. Used = lookup true and id != 0, then 0x7fc `( id, 1 )` and `Send_SKILL_USE_REQ`. A templet `+0x98` of 3 / 5 restores all three. Used: `+0x88bc` = slot + 1. Not used: only the id is restored.
- **Use `32ca50b0`:** vslot 0x810 true -> 0x800 `( templet id, chosen MP )`, 0x764 `( argument id, 1, MP )`, MP 0, `+0x8ac4`. Otherwise the skip is `+0x9700` == 1 and the **argument** == `0xc9b` ( JUNE_BUGS 570 confirmed @32ca5125 and @32cac42c ). Then 0x804, `32ca2af0`, else `31fd32e0`, then `+0x8ac4`. June has no null test on the templet, and neither does the port.
- **Eradication `32ca2af0`:** base 23060, `32007c40( 2 = SA_EFFECTIVE_COUNT, base, 1, &limit )`. `fcompp` of limit against count with `test ah, 0x41` / `jp` goes on only when limit <= count, then `ResetEradicationLoop` and state 0x12e ( June 302 = `LESI_SA_RNW_ERADICATION_ATTACK` by `ch0_state_enum_names.tsv` ).
- **Cooldown `32c89a20`:** `IsMyUnit`, templet / `GetUnit` null, slot mark `( slot, 0 )`, `UpNowMp( -mp )`, stone, level clamp. Base 0x5aa1 / 0x5ab1 skips the cool time. Vslot 0x6a4 is skipped when `IsSamef( cool, -1.0 )`. Siege is 0x5aa1 / 0x5ab5 on the templet id, with `+0x96b8`. Then `3235a710`, `+0x8ac0` = slot + 1, `31fcd3d0`. It has Raven's shape.
- **`32ca5160`:** base `32008e40`. Action ( `+0x8124` = `m_FrameDataNow`, as March's switch ) 0x10: riding, `+0x9700`, `EntireExplosion`, stop `+0x970c`, cool time 0x5ab1 = `+0x9708` ( March's `m_fStartOfDelayedFiringCoolTime` by layout ), then 0, false, `IsMyUnit` -> slot UI. Action 0x28: templet 0x5b6a, vslot 0x228 `( t, 0 )` then `( t, 1 )`. **`32c87ac0`:** base 0x5ab1 + `+0x9700` -> future action 0x10 ( March writes `m_FrameDataFuture` too ). `UAI_START_OF_DELAYED_FIRING` = 16 by `enumprobe`.
- **`32c83320`:** `GetSkillLevel( 0x5b6a, 1, 0, 1 )`, class `+0x38` 12 / 102, `GetSkillAbilityValue( 0x67 = SA_ANI_SPEED, level, 0 )`, default 1.0. **`32c85a60`:** base, then not 154 / 155 / 160 / 231 / 232 / 233 = `LESI_DOUBLE_JUMP`, `_DASH_DOUBLE_JUMP`, `_CHARGE_MP`, `_RNW_ACCEL_DASH`, `_JUMP`, `_JUMP_LANDING`. **`32c85c00`:** in 13 / 160, enabled -> saved = 1, disabled. Otherwise disabled -> enabled = saved. The saved byte is never cleared, in June or in the port. Then `OnFrameMove( +0x2ac, m_fElapsedTime )`. March's `Enable` / `Disable` only write `m_bEnable`. The second after-image `+0x6758` stays a stand-in.
- **"= March" rows:** `32c86af0` = `L"Dummy1_Rhand"`, `+0x8acc`, time, `bOnlyLight`. In `32c9ac00` I traced the offset table store by store: it equals March's nine rows ( row 4 z -50, row 8 z +50 ). READY types are 1 -> 7 / 3 -> 8 ( `DET_*` by `enumprobe` ), count reset after 8, 0.01 + 0.1. `32cb5b70` jump table ( `type - 2`, 13 bytes ) = cases 2 / 4 / 6 / 10 / 12 / 14.
- **Constructor:** every `_CORE` store matches: `edi` = -1 for `+0x97b8` / `+0x97c0` / `+0x9818`, floats and vectors 0, `+0x97bc` / `+0x97f0` / `+0x981c` 0, `+0x9810` / `+0x981e` 0. `ResetEradicationLoop` @32ca42ab, 1.1 `+0x8fb8`, 1.3 `+0x8fbc`, `+0x973c` = 7, `+0x9768` = 5.5.
- **Not exact in the Done note:** `+0x92b4`, `+0x8d90` and `+0x8da4` are not stored by `32ca2b60`. They sit in the `CX2GUUser` region, which June's user constructor zeroes ( CH1j header comments ), so the port's 0 in the Lire constructor has the same value. I left the code comment alone. "Two unmapped stores" leaves out the zero vector `+0x978c`-`+0x9794` and the deferred `+0x97e0` / `+0x97e4` / `+0x97ec` / `+0x9804` / `+0x9808` ( the CH1k kickoff's "declare when read" ). **`+0x978c` is routed to CH1q** ( kickoff amended ).
- **X2GUUser:** the `ReservedSkillStart` declaration and call chains give exactly one for any `_LIVE` combination. The stand-in set adds `UT_LIRE`. `ResetAnimSpeed`'s Agile Movement block is skipped under `_LIVE`. `ReservedSkillStart` is Raven's body.

Gates. `flagcheck JUNE_CHAR_RENA_LIVE --with ..._CORE ..._NF --range 59b63f1c^..59b63f1c`: `PASS` `Always.h` +18, Lire `.cpp` +428, `.h` +9, `X2GUUser.cpp` +22, `.h` +13 ( `X2SkillTree.h` OTHER = `ENUM_NAMES` ). `0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because `_LIVE` is `//#define`. `JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `PASS~ +12`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check` ( Lire `.cpp` / `.h` ): `0 A/B row(s)`. Probe `/c /Zs /EHsc`: Lire `.cpp` and `X2GUUser.cpp` exit 0 / 0 errors, both off and with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. No code change, so no build. The deployed exe is CH1k's ( `DEPLOY OK` md5 `2d68e720f440229516a62b9bbb6d3805` ).
CH1m kickoff checked: the CH1k amendment is there. Nothing to play-test. Next: `/x2-phase char-rena CH1m` ( Opus 5.5 high ).

### CH1m - Done (2026-11-09)

All three class-map rows are `inside`: June's bodies sit in March's own `InitStateCommon` (`32ca9dc0`) and `InitState` (`32cadc90`, vslot
0x7a4) under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's bodies in the `#else` (still `//#define`d). `32c96d60` is March's `StateInitTrapBlade`
( = March, nothing to write ). Class map corrected: InitState's March pair is `.h:613 InitState()` ( CH0 had it as new ), 32ca9dc0's is `InitStateCommon`.

**How it was derived.** `june2017_re/char_rena/ch1m_regsim.py` ( Raven's, with Rena's two class reads: unit `+0x38` and templet `+0xaf0` -> `+4`,
folded into one class expression; null tests of the unit / templet taken as not null ) walks fresh decompiles once per class ( 3 12 13 24 102 103 114 ).
It equals CH0's listing-checked `ch0_reg.tsv`: InitStateCommon 89 / 89 sites, InitState 108 / 108 distinct registrations ( CH0's `~` rows are the
per-arm view of the shared calls ). `ch1m_reg_by_class.tsv` is that output. `ch1m_gen.py` writes the C++: Rena has no line registrars, so the
per-class sequences are aligned on the registrations every class makes alike, and the rest is cut at June's nine class tests ( decompile @306 / @857 /
@1031 / @1095 / @1160 / @1220 / @1254 / @1345 / @1384 ) into one `switch( eClass )` each, one case per group of classes. Ids: the `June script id N`
member CH1d-CH1i wrote, else March's registration of the table, else the member of that name. `ch1m_spec.py` makes the bytepatch spec.
`ch1m_regcheck.py` parses the `_LIVE` branches per class ( read with `_NF` on ): **992 registrations match slot by slot, 0 problems; 172 June ids map
one-to-one both ways** ( `ch1m_idmap.tsv` ).

**InitState `32cadc90`:** cut-in 3 / 12 / 13 / 24 -> 0, 102 -> 1, 103 -> 2, 114 -> 3 ( no default write ), sub-index 12 -> 1, 13 -> 2, else 0 ( = March ).
`InitStateID` is inlined ( `+0x4088` = 0xd, `+0x7d04` = 0xa0 = `USI_WAIT` / `LESI_CHARGE_MP` ): March's is the same, so it is called. `31fc93d0` ( empty
stub ) is not called. Then `InitStateCommon`, `InitStateCommonRandom` ( 3202b110 + 31fe22d0; its five `USI_EVENT_*` states are this phase's ), START
( Start / EventProcess / End ), WIN ( `WinStateStart`, `RandomLoseEventProcess` ), the `LESI_RNW_WIN2` compare @32cadfac -> byte `+0x9714` =
`m_bShowTrapBladeToWinMotion` ( = March ), LOSE ( `LoseStateStart` ), `InitWaitStateChange( "LESI_WAIT_STATE_CHANGE" )` @32cae131 ( 7h ), the nine class
blocks, then vslot 0x690, 0x684( 0 ), `31fd8f00`, `StateInitTrapBlade()` ( @32cb0480, every class ) and, under `_NF`, `SetNaturalForceBuff( true )`
when `0 < GetNaturalForceCount()` ( `32c85200( 1 )` @32cb04a6; March's `>= NATURAL_FORCE_BUFF` stays in the `#else` ). Per class 61 / 69 / 62 / 67 /
75 / 65 / 75 registrations incl. the four head ones.

**InitStateCommon `32ca9dc0`** ( 89 per class ): hyper id 23 = `GetHyperModeTableName( false )` ( `31fdd490( ..., 0 )` @898; Rena's `LESI_LWS` / `LGA` /
`LNW_HYPER_MODE` cases were there ), FrameMove = Rena's `HyperModeFrameMove` ( vslot 0x600 ), End = `CX2GUUser::HyperModeEnd` ( 0x604 not overridden ).
One class arm: `LESI_DASH` StartFuture / FrameMoveFuture are Rena's for 114, the user base for the others ( @532 ). Bounding = `InitBoundingStates(
"LESI_" )` at June's place ( 7l ). Low Kick ( 295 ) is registered here first ( @97 ).

**Ids.** 213 is `LESI_LGA_DASH_JUMP_Z` ( the Combat Ranger Z1 table, as March ); the other registration of that table ( 191 ) resolves to
`LESI_DASH_JUMP_ATTACK_Z` by its own table. **Id 278** ( CH1g3 left it to CH1m ): June registers the XZ chain's loop slot with the
`LESI_RNW_DASH_JUMP_XXZ_LOOP` table and enters it nowhere; registered as June does under a new `_CORE` member `LESI_RNW_DASH_JUMP_XZ_LOOP` ( `.h`, after 302 ).

**`_NF` slots.** Seven June callbacks are March functions that exist only under `ADD_RENA_SYSTEM` ( CH0's `inside (ADD_RENA_SYSTEM)` rows ): `WalkStart`,
`DashStart` ( `#else` `CX2GUUser::DashStart` ), `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX_StateStart` / `_FrameMove` / `_EventProcess`,
`DashJumpComboX3StateStart`, `LESI_LGA_ZZXX_StateStart`. Each sits under a nested `#ifdef SERV_IRUHADEV_JUNE_CHAR_RENA_NF`; CH1q writes their bodies
( `CH1q.md` amended ).

**Left out, each a comment line with its owner:** phoenix run 123-126 ( PvP ), dance 127-129 ( Stage 10 ), riding 93-96 ( Stage 17 ), the empty slots
`3202a7d0` ( die end, in `StateChange` ) and `32002f70` ( Stage 17 ). **JUNE_BUGS 571:** `LESI_RIDING_DAMAGE_BACK` is registered with no script
table; the script defines `LESI_RIDING_DAMAGE_FRONT` twice ( lines 11083 / 11090; Raven's twin, row 542 ).

**The `_LIVE` gate (`Always.h`):** six `#error` guards added ( Raven's registration set Rena lacked ): `JUNE_CHAR_EVE_CORE`, `HYPER_MODE_FIX`,
`JUNE_COMMON_RANDOM_STATE`, `JUNE_EVENT_STATES`, `JUNE_WAIT_STATE_CHANGE`, `JUNE_BOUNDING_STATE`; all defined ( `cl /P` flag probe ) and the `_LIVE` probe passes them.
For CH1n: `ch1m_user_callbacks.tsv` lists the 131 `CX2GUUser` members the registration names; all are on Raven's list except `RandomLoseEventProcess`
( every class uses it ). Slot states 167 / 202 / 283 / 295-302 are `32cb06d0`'s ( `CH1n.md` amended ). Checklist: 7h, 7l and the 7ao hyper-table bullet marked `(Rena: CH1m)`.

**Checks.** One-file probes of `X2GULire_ElvenRanger.cpp` ( `/c /Zs /EHsc` ), `_LIVE` off and on: `exit 0 errors 0` each. `cl /P` off / on:
`InitWaitStateChange( "LESI_WAIT_STATE_CHANGE" )` 0 / 1, `InitBoundingStates( "LESI_" )` 0 / 1, `GetHyperModeTableName( false )` 0 / 1, the WIN2 compare 0 / 1,
`LESI_RNW_DASH_JUMP_XZ_LOOP;` 0 / 1, `LESI_RCR_COMBO_XXZ_EventProcess` 2 / 4, `LESI_RNW_DASH_XZ_StateStart` 2 / 3. Every registered table is a top-level
table of June's script except `LESI_RIDING_DAMAGE_BACK` ( row 571 ).
`git diff --patience --numstat`: `X2GULire_ElvenRanger.cpp` 1737 / 0, `.h` 1 / 0, `Always.h` 18 / 0. bytepatch: CRLF 171 / 80 / +18 ( guards only ), high bytes unchanged ( 21398 / 4576 / 38738 ).
`flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only for the `//#define`.
`flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS`, 0 deleted, `RESULT: PASS`.
`junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check` ( `.cpp`, `.h` ): `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-11-09 14:38:22, md5 77fdb81177f2f6ab809c427f0ffb0f85, equal to X2/US_SERVICE/x2.exe (2026-11-09 14:38:15); built at main 226f5096 + 3 uncommitted code file(s).`

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-rena CH1m review` (Opus 5.5 high).

### CH1m - Review (2026-11-09, Opus 5.5)

**Pass, no code change.** Commit `a295ca5a` read against fresh Ghidra decompiles and listings of `32ca9dc0` / `32cadc90` and the exe bytes.
- **Simulation.** `ch1m_regsim.py` over the fresh decompiles = `ch1m_reg_by_class.tsv` row for row ( 1097 rows, `diff` empty ); the only `UNKNOWN COND`
  is the NF count test @1705. Both folded class reads are the unit class: unit `+0x38` is what the cut-in switch tests against 3 / 12 / ... / 114, and
  templet `+4` is `m_UnitClass` ( `X2Unit.h:331-332`, `+0` = `m_UnitType`, funcmap 31e4d0e0 ). `ch1m_regcheck.py`: `matched 992 registrations, 0 problems; ids mapped 172`.
- **Head ( exe bytes ).** Jump table `32cb0558` / bytes `32cb056c`: 3 / 12 / 13 / 24 -> `mov [esi+0x8ac8], ebx(0)`, 102 -> edi(1), 103 -> edx(2), 114 -> 3,
  the other 105 bytes -> `32cadd1d` ( no write ). Sub-index 12 -> 1, 13 -> 2, else 0. `+0x4088` = 0xd, `+0x7d04` = 0xa0 = March `InitStateID`. `31fc93d0` is a bare `RET`.
- **START / WIN / LOSE.** The second `31fe22d0` OUT string ( `esp+0x78` ) is the one registered as 0xb ( WIN, @32cadf8b ); the compare @32cadfac takes
  `[esp+0x88]` after four pushes = the same string, against `"LESI_RNW_WIN2"` ( `3351a25c` ), and sets byte `+0x9714`. 0x82 @32cae131 with
  `"LESI_WAIT_STATE_CHANGE"` ( `3351a244` ). `InitStateCommonRandom` runs the event states first ( `JUNE_EVENT_STATES`, funcmap 3202b110 C9h-4h ).
- **Class tests ( listing ).** 32cae14b 12/102; 32caead1 24/114 ( templet ); 32caeefd 13/103 ( templet ); table `32cb05dc` 12,102 / 24,114 / rest;
  32caf2ec 13,103 / 102 / rest; 32caf488 24/114; table `32cb0650` 12,102 / 103 / 24,114 / rest; 32caf84a 13/103; 32caf982 102 / 103 / 114 / rest.
  The nine C++ `switch( eClass )` blocks group the classes in exactly these sets, in this order.
- **Ids.** `ch1m_idmap.tsv`: 172 ids, 172 enumerators. 213 = `LESI_LGA_DASH_JUMP_Z` ( table `LESI_COMBAT_RANGER_DASH_JUMP_COMBO_Z1` ); the eight
  shared-table pairs each keep their own member. **278**: `PUSH 0x116` @32cb037c takes `335198ec` = `LESI_RNW_DASH_JUMP_XXZ_LOOP`, and the exe has no
  `LESI_RNW_DASH_JUMP_XZ_LOOP` string, though June's script defines that table ( line 7931 ). The member name is the script's own; registering it with the
  XXZ table is June's quirk, kept: **JUNE_BUGS 572** ( the phase left it unrecorded ).
- **InitStateCommon.** Id 23 = `31fdd490( ..., 0 )` @898 = `GetHyperModeTableName( false )`. `32adf320` = `jmp [eax+0x600]`, Rena vtable `335185bc`
  +0x600 = `32ca5310` ( Rena `HyperModeFrameMove` ); `329cc0e0` = `jmp [eax+0x604]`, Rena +0x604 = `3201b8e0` = user vtable `334781f4` +0x604 ( base
  `HyperModeEnd` ). `LESI_DASH` arm 114 only ( templet test @531 ). Bounding at June's place ( after dance 127-129, before `LESI_THROW_ITEM` ); 93-96,
  123-126, 127-129 left out with owner comments.
- **`_NF`.** The seven callbacks sit under nested `_NF` guards. Only `DashStart` has a March flag-off pair ( `CX2GUUser::DashStart`, = March's `#else //ADD_RENA_SYSTEM` ).
- **Tail.** `0x690` / `0x684( 0 )` / `31fd8f00` / `32c96d60` / `32c85200( 1 )` when `0 <` the count = lines 3883-3889 ( same as Raven's ).
  `StateInitTrapBlade` is unconditional, as in June ( March: `SERV_TRAPPING_RANGER_TEST` + `X2OPTIMIZE_...` ).
- **JUNE_BUGS 571** confirmed: the script has `LESI_RIDING_DAMAGE_FRONT` at 11083 and 11090, and no `LESI_RIDING_DAMAGE_BACK`.

Gates. `flagcheck JUNE_CHAR_RENA_LIVE --range a295ca5a^..a295ca5a --files X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h`: `files: 2 PASS` ( `Always.h` +18,
`.cpp` +1737 ), `0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only
for the `//#define`. `JUNE_CHAR_RENA_CORE --files .h`: `files: 1 PASS`, `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`.
`enumscan.py --check` ( `.cpp`, `.h` ): `0 A/B row(s)`. Probe `/c /Zs /EHsc` of the Lire `.cpp`: exit 0 / 0 errors, off and with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`.
No code change, so no build; the deployed exe is CH1m's ( md5 `77fdb81177f2f6ab809c427f0ffb0f85` ). `CH1n.md` checked: the CH1m amendment is there.
Nothing to play-test. Next: `/x2-phase char-rena CH1n` ( Opus 5.5 high ).

### CH1n - Done (2026-11-09)

**Head (`32cb06d0`), inside, `_LIVE`.** June's head sits in March's own `SetEquippedSkillFuncBySkillSlotIndex` up to the id switch; March's head and
type switch are the `#else` (`_LIVE` still `//#define`d). Raven's CH1n is the template; read from the decompile and the exe bytes:
- Bounds `0 > slot || 4 < slot`; tables `335153c0` / `335153e8` (normal A / B, 50.. / 60..) and `335153d4` / `335153fc` (hyper 51.. / 61..) = March's
  `_CONST_GUUSER_` arrays (PE bytes). `31fd2af0` dropped (7i), `+0x11` = `IsSkillSlotState`, hyper `0x101`; `GetEquippedActiveSkillTemplet( id, true )`.
- **Transcendence memo @32cb07cd:** `31fdbcc0`, then `32206450( memo, 1 )` non-null -> the epilogue. No case, as Raven's.
- **Types** (bytes `32cb3004` = {0,1,1,3,3,3,3,2,1,1,1,1}, jump `32cb2ff4`): 5 buff = `LESI_SI_LE_COMMON_BUFF` / `_HYPER` (FM vslot 0x5f0 = Rena's
  `32c849f0` `COMMON_BUFF_FrameMove`, EP `31fcbd60`); 6 / 7 / 0xd-0x10 = the hyper switch, then the id switch; 0xc + 900 = `LESI_THROW_ITEM`, vslots 0x5f4 /
  0x5f8 (not overridden); anything else returns.
- **Base id (Rena differs from Raven).** Both June switches take `32207210( id )` = `GetUpgradeBaseSkillID` (`SERV_IRUHADEV_JUNE_SKILL_TEMPLET`), not the raw id.
- **Hyper switch** (`0x5c30 < base`: 0x5c31 / 0x5c34 Cryotron, 0x5c32 / 0x5c35 Innocent, 0x5c33 -> Spiral, default `@32cb2eef`; `== 0x5c30` Spiral; else the id
  switch): `RNSI_HA_RWS_SPIRAL_STRIKE` / `_UPGRADE`, `RNSI_HA_RGA_SHOOTER_DESTINY` / `_UPGRADE`, `RNSI_HA_RLW_INNOCENT` / `_UPGRADE` (CH1i's names). All 28
  thunks resolve to March's callbacks in March's slots (Spiral READY Init / Start / EP, SPINING Start / FM / EP / End, FLYING Init / FM / EP; Cryotron GUIDE
  Init / Start / FM / EP, CHARGE EP, DOWNLANDING EP; Innocent START Init / EP, hyper EP only, END Init / Start / EP / End). Sub-states 0xf6 / 0xf7 / 0xf8 /
  0xf9 / 0xfb by table name. Bodies stay CH3d's (kickoff amended).
- **`SetPowerAttackSkillFunc`** (new, `_CORE`; June `@32cb2eef`, the default of both switches: hyper base ids above 23600 with no case, and the id switch
  default @911): `LESI_SI_LE_POWER_ATTACK1` on both slots with thunk targets `32c84740` / `32c86b60` / `32c74a40` (hyper `32c847b0` / `32c86b90` / `32c74d50`)
  = March's `default:` case; under `_LIVE` March's `default:` calls it. Bodies CH2a's.
- **`32cb5760`** (CH0's "new ?") = March's `RapidShotData::CreateRapidShotData()` (0x14 bytes, `+4` = 0x99 `LESI_BASE`); only caller the Rapid Shot case
  @32cb0e51. Nothing to port; class-map row corrected.
- **Slot states** the CH1m amendment named (167, 202, 283, 295-302) are made by id-switch cases, not by the head: routed by name to the owning skill
  phases (CH2a Siege, CH2b Low Kick, CH3a Sharpfall, CH3b Gungnir / Ace in the Hole, CH3c Eradication; kickoffs amended). CH2a's amendment also carries
  the base-id switch for the id switch.
- `Always.h`: three new `_LIVE` guards: `JUNE_SKILL_TYPE`, `JUNE_SKILL_NOTE`, `JUNE_DAMAGE_EXCEPTION` (`JUNE_ENUM_NAMES`, `JUNE_SKILL_TEMPLET` were there).

**Dev free-skill gate.** `32cb06d0` has no cool-time / MP check; the gate is CH1k's `IsDevUnlimitedSkillUse()` in `SpecialAttackLookup` (`32cac200`,
`X2GULire_ElvenRanger.cpp`). Checklist marked `(Rena: CH1n)`: dev-gate row (as CH1k), §7b memo row, the 7i head row, the `+0x11` row, types 14-16.

**Base callbacks.** `june2017_re/char_rena/ch1n_triage.py` joins every callback target outside Rena's range in `ch1m_reg_by_class.tsv` (157 VAs) with
Raven's, Chung's and Elsword's CH1n triages by June VA -> `ch1n_user_callback_triage.tsv`: 130 found (62 SAME; DIFF: 16 ported for every class, 5 kept,
47 Stage 17), 27 `n/a` (14 riding 93-96 / empty slots and the die StateEnd, 3 phoenix, 3 dance: left out by CH1m; 5 bounding + 1 wait-state-change: the
shared helpers; 1 vslot 0x600 thunk = Rena's `HyperModeFrameMove`). `RandomLoseEventProcess` `31fcd0b0` = Chung's row. Nothing new, nothing to widen:
the four `X2GUUser` sites gated on Rena's `_LIVE` are the pressed-index stand-in, ResetAnimSpeed and ReservedSkillStart, none a registered callback.

**Checks.** One-file probes of `X2GULire_ElvenRanger.cpp` (`census.cl_cmd`, `/c /Zs /EHsc`): `_LIVE` off `exit 0 errors 0`, `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`
`exit 0 errors 0` (the three guards pass). `cl /P` off / on: `SetPowerAttackSkillFunc( normalStateData` 0 / 2, `GetUpgradeBaseSkillID( eSkillID_ )` 0 / 1,
`RNSI_HA_RWS_SPIRAL_STRIKE:` 0 / 1, `GetTranscendenceMemoSkillID( eSkillID_ )` 0 / 1; the helper's definition 1 / 1.
`git diff --patience --numstat`: `X2GULire_ElvenRanger.cpp` 187 / 0, `.h` 1 / 0, `Always.h` 10 / 0. bytepatch: high bytes unchanged (21398 / 4576 / 38738).
`flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h KTDXLIB/Always.h`:
`files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only for the `//#define`.
`flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS` (the OTHER is the `.cpp`'s `_LIVE` lines).
`junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check` (`.cpp`, `.h`): `0 A/B row(s)`.
Funcmap rows `Char Rena CH1n` (3). No JUNE_BUGS row.
`DEPLOY OK: X2_offline.exe is 26,105,856 B, dated 2026-11-09 15:11:52, md5 f266e292f4921dc126f0386b92bd2fb3, equal to X2/US_SERVICE/x2.exe (2026-11-09 15:11:45); built at main 8ab92198 + 3 uncommitted code file(s).`
The build ran before `8ab92198` (full build only for play-test phases); the phase-tag comments (`// char-rena CH1n`, same rule commit) were added after it,
comment-only, and re-proven by both probes (`exit 0 errors 0`) and flagcheck (same tallies). No `exestr` line: `_LIVE` is off, the helper adds no string.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-rena CH1n review` (Opus 5.5 high).

### CH1n - Review (2026-11-09, Opus 5.5)

**Verdict: PASS, no code change.** Read from a fresh decompile of `32cb06d0` (1482 lines) and the exe bytes (`june2017_re/disasm_range.py`; tables and
strings from the PE sections).
- **Head.** `js` / `cmp esi, 5; jae` = `slot < 0 || 4 < slot`. Tables by `bSlotB_`: `335153c0` {50..58} / `335153e8` {60..68} normal, `335153d4` {51..59} /
  `335153fc` {61..69} hyper = `_CONST_GUUSER_`. `31fd2af0( normal )` (7i snapshot) dropped; vtbl[0] = `Init()`; `byte +0x11 = 1` normal (= the static
  `IsSkillSlotState`, `X2GUUser.cpp:16814`), `word +0x10 = 0x101` hyper. `31fca8e0( id, 1 )` = `GetEquippedActiveSkillTemplet( id, true )`, null -> epilogue.
  `3207c1a0` / `31fd88f0` / `31fdf970` = the anti-tamper batch around `m_StateList[]`, as ported.
- **Memo exit @32cb07cd.** `31fdbcc0( id )`, `test; je` past, then `32206450( memo, 1 )` on `g_pData +0x50`, `jne 32cb2fc1` = the epilogue. The C++ is the
  house form of Raven / Chung / Elsword.
- **Types.** `[templet +4] - 5`, `cmp 0xb; ja` exit; bytes `32cb3004` = {0,1,1,3,3,3,3,2,1,1,1,1}, jump `32cb2ff4` = buff `32cb0810` / active `32cb0868` /
  throw `32cb2f5a` / exit `32cb2fc1`: 5 buff; 6, 7, 0xd-0x10 active; 0xc throw; 8-11 exit (`ST_RELATIONSHIP_SKILL` = 12, `ST_HYPER_ACTIVE_SKILL` = 13,
  grades 14-16, `X2SkillTree.h:26-56`). Buff: strings `3351ae50` / `3351ae30` = `LESI_SI_LE_COMMON_BUFF` / `_HYPER`, FM `329ccd90` -> `32a77a20`
  (`jmp [vtbl+0x5f0]`, Rena's `32c849f0` = `COMMON_BUFF_FrameMove`), EP `329cba40` -> `31fcbd60`, hyper copies. Throw: `cmp [id], 0x384` on the raw id,
  `335192b8` = `LESI_THROW_ITEM`, FM `329ccda0` -> `329cc0d0` (`+0x5f4`), EP `329ccdb0` -> `329cc170` (`+0x5f8`), copied to hyper.
- **Base id.** `32cb0870 call 32207210( id )` feeds both the `cmp 0x5c30` chain and the id switch (`sub 0x59d8; cmp 0xe3; ja 32cb2eef`):
  `GetUpgradeBaseSkillID`. The C++ switches the hyper part on it; the id switch stays March's raw-id switch until CH2 (CH2a's amendment says so).
- **Hyper switch.** `jg` -> `base - 0x5c31; cmp 4; ja 32cb2eef`, table `32cb31e0` = {Cryotron, Innocent, Spiral, Cryotron, Innocent}; `je` -> Spiral
  `32cb2bf1`. `RNSI_HA_*` 23600-23605 = June `Enum.src.lua` and `X2SkillTree.h:1822-1827`. Strings `3351a384` / `3351a360` / `3351a338` / `3351a310` /
  `3351a2e8` / `3351a2c0` / `3351a2a4` / `3351a288` = the eight tables the C++ names; sub-states 0xf6 / 0xf7 / 0xf8 / 0xf9 / 0xfb pushed with them. Every
  slot offset (normal / hyper +0x18 / +0x28 / +0x38 / +0x48, stack struct `[esp+0x1c]` +0x18 / +0x28 / +0x38 / +0x48 / +0x58 after `320775a0`) and all 28
  thunk targets equal the C++ (each target -> the March callback of that name in `ch0_class_map.tsv`). Spiral / Cryotron set hyper `byte +0x10 = 1` again
  (the C++ repeats it); Innocent's hyper slot copies only +0x44 / +0x48.
- **`SetPowerAttackSkillFunc`.** Both defaults (`ja` @32cb0893, `ja` @32cb2be4) land on `@32cb2eef`; `3351a26c` = `LESI_SI_LE_POWER_ATTACK1`, normal +0x30 /
  +0x38 / +0x48 -> `32c84740` / `32c86b60` / `32c74a40`, hyper -> `32c847b0` / `32c86b90` / `32c74d50`. The hyper three are separate June functions,
  byte-identical to the normal ones as March's `_HYPER` twins are; the helper = March's `default:` line for line.
- **`32cb5760`.** Only xref @32cb0e51 (Rapid Shot case); `new 0x14`: +0 0, +4 `0x99` (`LESI_BASE`), +8 0, +0xc NULL, +0x10 0.0f = March's
  `RapidShotData()` init list (`X2GULire_ElvenRanger.h:402`). Nothing to port.
- **Callbacks.** `ch1n_triage.py` re-run reproduces `ch1n_user_callback_triage.tsv` byte for byte: 157 rows, 62 SAME / 68 DIFF (16 ported, 5 kept, 47 Stage 17)
  / 27 `n/a`.
- **Always.h.** The guards name what the head uses: `GetEquippedActiveSkillTemplet( id, bool )` (`X2GUUser.h:2701`, `JUNE_SKILL_TEMPLET`),
  `GetTranscendenceMemoSkillID` (`X2GUUser.h:3109`, inside `JUNE_DAMAGE_EXCEPTION` then `JUNE_SKILL_NOTE`), the grades (`JUNE_SKILL_TYPE`).

**Bookkeeping fix.** `ch0_class_map.tsv` rows `32c847b0` / `32c86b90` / `32c74d50` named the normal March functions; now the `_HYPER` ones (March
snapshot lines 3167-3169), as the C++ already had.

**Hygiene.** flagcheck `--range 6e8b5f0c^..6e8b5f0c --files`: `_LIVE --with _CORE` `files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`,
FAIL only for the `//#define` (by design); `_CORE` `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`. Each: 0 deleted,
0 non-ASCII, 0 EOL-mismatched, 0 `#else`/`#endif` without the token, 0 encoding changes. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`.
`enumscan.py --check` (`.cpp`, `.h`): `0 A/B row(s)`. One-file probes (`/c /Zs /EHsc`): `_LIVE` off `exit 0 errors 0`, on `exit 0 errors 0`.
Full build skipped: no play-test (the review changed no code; CH1n's `DEPLOY OK` stands).

**Nothing to play-test** (`_LIVE` is off). CH1o's kickoff stands as written (Amended by CH0 / CH1j / CH1k checked). Next: `/x2-phase char-rena CH1o` (Opus 5.5 high).

### CH1o - Done (2026-11-09)

**Landed** (flag `_LIVE`, still `//#define`; `_CORE` for the new helper; 14 funcmap rows tagged `Char Rena CH1o`). Files: `X2GULire_ElvenRanger.cpp` / `.h`,
`X2GameUnit.h`, `X2Game.cpp`, `X2GUNPC.cpp`, `X2GUUser.cpp`, `X2DamageEffect.cpp`, `KTDXLIB/Always.h`. Every block carries `char-rena CH1o`.
- **Vslot 0x318 is March's `DoProcessWhenKillEnemy` (corrects CH1h).** Vtables read from the PE: `CX2GameUnit` 334883b4 / `CX2GUUser` 334781f4 / `CX2GUNPC`
  334ba584 + 0x318 = `3208ddc0` (empty, = March's base); Rena + 0x318 = `32cb3200`. A `[r+0x318]` call sweep finds three callers: `CX2Game::ProcessIKillThisNpc`
  `322cf060 @322cf0ee`, its PvP twin `ProcessIKillThisUser` `322cf100 @322cf195`, and `CX2GUUser::DyingStart` `32014810 @320149eb` (= March's attacker call).
  The argument is the **killed unit**: `32cb3200` copies its `+0x763c` DamageData (`32cb6dd0`) and tests the Eradication triggers on that copy. CH1h's
  `_CORE` `ProcessEradicationHit( DamageData* )` is now the `_LIVE` override `DoProcessWhenKillEnemy( const CX2GameUnit* )`, body unchanged after the copy.
- **The killing-blow DamageData, engine side (all `_LIVE`).** `+0x763c` is a June-only `CX2GameUnit` member (ctor `322a0c72`, dtor `322a2700`):
  `m_DamageDataWhoAttackedMe` + `GetDamageDataWhoAttackedMe()`. June stores it right after `SetGameUnitWhoAttackedMe` (`32076140`) in the NPC
  DamageReact (`@327b4cb6` user attacker, `@327b4e95` NPC attacker) and on the user DamageReact's owner-credit path (`@3204a1e5`), and its inline
  `ResetGameUnitWhoAttackedMe` also runs `DamageData::Init` (`31f8f550`; six inlined sites). Ported at March's X2GUNPC.cpp:11457 / :11486,
  X2GUUser.cpp:11887 and the inline reset. June's gate on the user store is `!IsPve` where March tests the ally team (PvP only; JUNE_BUGS 573: a direct
  user-on-user kill keeps no DamageData). June's `ProcessIKillThisNpc` / `User` hand over `GetNPCUnitByUID( m_nDieNPCUID )` / `GetUserUnitByUID(
  m_KilledUserUnitUID )` where March passes NULL: June's whole bodies under `_LIVE` (Ara's override ignores the argument). Three `june_march_divergent.tsv`
  rows (the two handlers, the reset), source "ported under SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE".
- **Vslot 0x1e8 is a `CX2GameUnit` slot** (base `3208ddc0` empty; `CX2GUUser` overrides it with `32011b80`, routed to backlog "attack-result hooks").
  New `CX2GameUnit::ProcessAttackResultPassive( DamageData* ) {}` and its three June call sites, each right after the attack-result delegate run:
  CEffect `31f86a92` (owner), user `320139a4`, NPC `3276a7a8` (the NAT_ALLY branch only, as June) = X2DamageEffect.cpp:5444, X2GUUser.cpp:15243,
  X2GUNPC.cpp:14821. Rena's override moved to `_LIVE`, `virtual`, and calls `CX2GUUser::ProcessAttackResultPassive` first (`@32ca9cc9`; reaches the
  empty base until the user body lands; backlog note added).
- **`SetSkillLevelStateData` = June's `3201f850` under `_LIVE`** (Raven's CH1o form): null templet / unit return, `max( 1, GetSkillLevel( id, false, 0,
  true ) )`, power rate, id, x `( GetSkillAdditionalPowerRate( id ) + 1.f )`. Rena keeps vslot 0x7a8 `32028190`; all 39 `SetEquippedSkillLevel` writes get the term.
- **The five base-called helpers.** `32c83580` / `32c6dbd0` / `32c6dd10` / `32c6ddc0` = March's static `AppendEffectToDeviceList` / `AppendMajorParticleToDeviceList`
  / `AppendComponentToDeviceList` / `AppendDeviceToDeviceList` (same names, order, tests; bases `31fef4d0` / `31fede50` / `31fca170`, Raven's `32c3d1c0` case):
  no code. `32c85b20` = new `_CORE` `GetRageFungusTargetLockCount()`: erase every lock not above 0 (NaN too), return the size as a word. Its only caller
  is unit vslot 0x6c `3229a530 @3229a79c` (attacker type 3, skill 23042: count <= `SA_EFFECTIVE_COUNT` -> both attacks x `SA_DAMAGE_REL`), which
  char-elsword CH1o routed whole to Stage 33: `stage33-plan.md` now names the Rena block and the helper.
- **`31c8a5c0`** = `CKTDGXMeshPlayer::AppendToDeviceList( list, wstring, priority )`, = March including its inverted empty test (a named templet never
  appends; StateInitTrapBlade stays a no-op in both). No code.
- **Routed items closed.** `ShownSkillCoolTime` (0x6a8): base virtual and X2Game caller live under `JUNE_USER_SHOWN_SKILL_COOL_TIME`; `Always.h` guard
  added under Rena's `_LIVE`. `StoreGageSnapshot` (0x740): its base `32009780` is the Crest of Time engine, routed whole to `char-add`; stage7-plan row 8
  now lists Rena's override. `UpdateSpineAimRotation`: by name, nothing to wire. `AfterImageFrameMove` (0x850): base caller exists (X2GUUser.cpp:21316).
  `ResetAnimSpeed`: not repeated (CH1k).

**Routed (scope rule).** Stage 33: `3229a530` with Rena's block. Backlog "attack-result hooks": `32011b80` as `CX2GUUser`'s 0x1e8 override. char-add:
Rena's `StoreGageSnapshot`. Other `+0x763c` readers: none beyond `32cb3200` (the other sweep hits are the six resets).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_LIVE --with JUNE_CHAR_RENA_CORE --worktree --files` the 8: `files: 7 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER, 0 FAIL,
  0 ERROR`; `7 deleted line(s)` (inside `_CORE`: CH1h's 0x1e8 / 0x318 declarations and the 0x318 head), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif`
  without the token, 0 encoding changes; RESULT FAIL only for `define: NOT FOUND` (`//#define`). numstat: .cpp 41 / 5, .h 3 / 2, X2Game 18 / 0,
  X2GameUnit.h 11 / 0, X2GUNPC 9 / 0, X2GUUser 6 / 0, X2DamageEffect 3 / 0, Always.h 3 / 0.
- `junediverge.py --check` the 7 code files: `allow-listed: 18`, `RESULT: PASS`. `enumscan.py --check` Rena .cpp, X2Game, X2GUNPC, X2GUUser,
  X2DamageEffect: 1 A row, `X2DamageEffect.cpp:1705` (an older log throttle's `>= 1000` ms read as `SI_SA_ES_FATAL_FURY`, not this phase); none in the new lines.
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`) of Rena .cpp, X2Game, X2GUNPC, X2GUUser, X2DamageEffect, `_LIVE` off / on: exit 0, 0 errors each.
  `cl /P` off -> on: Rena `ProcessAttackResultPassive(` 0 -> 4, `DoProcessWhenKillEnemy(` 1 -> 3, `+ 1.f ) * stateData_` 0 -> 1; X2Game the two June
  calls 0 -> 1 each, March's `( 0 )` 2 -> 0; X2GUNPC store 0 -> 2, call 0 -> 1; X2GUUser store 0 -> 1, call 0 -> 1; X2DamageEffect call 0 -> 1.
- Full build skipped: no play-test.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-rena CH1o review` (Opus 5.5 high).

### CH1o - Review (2026-11-09)

**Verdict: PASS with one fix** (flag `_LIVE`, still `//#define`; the fix is in CH1o's `_CORE` helper). Read from the exe bytes (capstone over
`unpacked_x2.exe`, `charport_adis.py rena`, vtables from the PE sections):
- **Fix: `32c85b20` keeps a NaN lock.** `fldz; fcomp [node+0x20]; test ah,1; jne keep`: C0 is set on `0 < value` and on unordered, so June erases only
  `value <= 0`. CH1o's `0.f < value` erased a NaN too (the Done note's "NaN too"). Now `if ( it->second <= 0.f ) erase( it++ ); else ++it;`; the return is
  the word at `+0x97fc` (map size). Comments and the funcmap row corrected. No reachable NaN (the timers only lose frame time); kept for exactness.
- **0x318.** `CX2GameUnit` 334883b4 / `CX2GUUser` 334781f4 / `CX2GUNPC` 334ba584 + 0x318 = `3208ddc0` (`ret 4`); Rena 335185bc + 0x318 = `32cb3200`.
  `dispsweep 318`, `mov r,[r+0x318]` then `call r`: exactly `322cf0ee`, `322cf195`, `320149eb`. `32cb3200`: arg NULL -> return; `mov ecx, edi; call 32cb6dd0`
  (twice, a temporary copy per trigger test; one const reference is the same read); `32cb6dd0` = `add ecx, 0x763c` + DamageData copy ctor `31e9cb70`;
  triggers 0x25 / 0x26 = `DTT_ERADICATION` 37 / `_EXPLOSION` 38; `SA_EFFECTIVE_COUNT` (0x5a14 = Eradication) before / after `inc word [+0x9810]` with
  `test ah,0x41; jp` = `!( limit > count )`; loop effect only when the instance is gone, `SA_EFFECTIVE_TIME`, `EffectSet_SA_RNW_Eradication_LoopFX` = the C++.
- **`+0x763c`.** `dispsweep 763c`: the three stores `@3204a1e5` / `@327b4cb6` / `@327b4e95` each follow `call 32076140` and `push` the DamageData argument
  into `31e9c190` (assignment); user side behind `322cc1c0` = `CX2Game::IsPve` (`jne` skip), after `SetGameUnitWhoAttackedMe( GetOwnerGameUnit() )`
  (`32759f00`); NPC side: the user-attacker branch (`[attacker+0x264] == 0`) and the observer-attacker branch. The six `31f8f550` calls follow the
  `+0xd80 / +0xd84` observer reset (= the widened inline reset; March has seven callers, the seventh `X2GameUnit.cpp:295` in the unit's own init, harmless).
  Ctor `322a0c72`; `332b3e85` / `332b4a35` are EH unwind funclets (`jmp ~DamageData`), not readers. JUNE_BUGS 573 holds.
- **Handlers.** `322cf060`: game vslot 0x98 (GetMyUnit) twice, `+0x867c`, `[pkt+9]` 64-bit != -1, vslot 0x54 UID = `+9/+0xd`, `push [pkt]` -> game vslot
  0xa8; `322cf100`: UID = `+4/+8` or `+0xc/+0x10`, `push +0x18, +0x14` -> vslot 0xa0. CX2Game 3348bf94 + 0xa0 = `322cca00` (UID compare, `ret 8`) =
  `GetUserUnitByUID`, + 0xa8 = `322ccac0` (`[npc+0x270]`, `ret 4`) = `GetNPCUnitByUID`; March `KEGS_*_DIE_NOT` offsets agree. Three divergent rows hold.
- **0x1e8.** `CX2GameUnit` / `CX2GUNPC` + 0x1e8 = `3208ddc0`, `CX2GUUser` = `32011b80`, Rena = `32ca9ca0`. Call sites `31f86a92`, `320139a4`, `3276a7a8`
  each follow `322752b0( +0x66ac, dd )` and push the same `dd`; Rena's `call 32011b80 @32ca9cc9` comes first. The sweep's other four `[r+0x1e8]` calls
  (`31f0d981` in a state frame move `31f0d390`, three in `322ff710`) push nothing: another class's slot, not this one.
- **`SetSkillLevelStateData`** = `3201f850` (Raven's review bytes); Rena 0x7a8 = `32028190`; the C++ matches. **Helpers**: `32c6dd10` (six minor names),
  `32c6ddc0` (three `Lire_ChargeSoul*.X`), `32c83580` (`32c6dbd0` + `ImpactTickBlue` + `3226f1c0`) = March `:1209` / `:7409` / `:814` in order.

**Hygiene.** Commit range `05d5f430^..05d5f430`, the 8 files: `files: 7 PASS, 1 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `7 deleted line(s)` (the
CH1h `_CORE` declarations / head), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for
`define: NOT FOUND` (`//#define`). Fix `--worktree` on the two Rena files: `files: 2 PASS`, `5 deleted line(s)` (the replaced helper lines, inside `_CORE`),
same FAIL only. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check` the 5 `.cpp`: 1 A row, the known
`X2DamageEffect.cpp:1705`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`) of the 5 `.cpp` off / with `_LIVE`: exit 0, 0 errors x10; Rena `.cpp` re-probed after
the fix, off / on: 0 errors. Full build skipped: no play-test.

**Nothing to play-test.** CH1p kickoff checked. Next: `/x2-phase char-rena CH1p` (Opus 5.5 high).

### CH1p - Done (2026-11-09)

**No code change.** Flags `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (still `//#define`) / `_CORE` untouched: every CH1p item was already June's from an earlier sub-phase.
Checks: `cl /P` of `X2GULire_ElvenRanger.cpp` / `X2GUUser.cpp` off and with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`census.cl_cmd`); June side
`dispsweep.py 8ac0 8b58 88bc` and an `E8` call scan of Rena's range `32c6a8d0-32cb6c6d` for the key helpers.

**7ag, the pressed-index space: June's since CH1k / CH1n.**
- **Class side.** `SpecialAttackEventProcess` under `_LIVE` = June `32c6a970`: lookup `32cac200` keeps the pressed index (`m_iChosenSkillSlotIndex`),
  use `32ca50b0` -> `StateChangeBySkillSlot( slot )`, cooldown `32c89a20` -> `MarkSkillSlotUsed( slot, false )` and `m_eSpecialAttackKeyPressed = slot + 1`
  (`+0x8ac0` `@32c89bac`), `m_iNowSpecialAttack = slot + 1` (`+0x88bc` `@32c6aa13`). March's 4 + 4 decode (`iPressedSkillSlotIndex > 3`, `[idx - 4]`) and
  `CountUsedSkill` are only in the `#else` (`.cpp:8590`, `:8682-8699`). In the `_LIVE` `.i` (class bodies): `CountUsedSkill` 0, `SAKP_` 0,
  `CurrentSkillKeyManager` / `GetPressedSkillState` / `m_kSkillSlotKey` 0, `m_eSpecialAttackKeyPressed` 1 (the writer). The four slot-B table reads left are
  June's: the slot head's `bSlotB_` select (CH1n) and an index-free slot-state membership loop.
- **Shared reader.** `CommonSpecialAttackEventProcess` (June `31fcadd0`, called `@32cac27e`): `UT_LIRE` already joined the June set under `_LIVE`
  (CH1k, `X2GUUser.cpp:26723`; `.i`: `bMarchSlotIndex` excludes `UT_LIRE` only with `_LIVE`).
- **June side.** In Rena's range the only `+0x8ac0` / `+0x88bc` touches are the two writers above; `+0x8b58` (`SkillSlotKey`) none. `GetSkillSlotIndexUsed`
  (`31fd7500`, = March) is called once (`@32c84ad9`, `COMMON_BUFF_FrameMove` `32c849f0`, CH1h, reads the state id). June's four held-key tests
  `IsEquippedSkillKeyPressed` (`31fce710`) sit in states March tests no key in: Back Kick `32c84ea0` `@32c84fd9` (March's
  `LESI_A_LWS_BACK_KICK_EventProcess` has none) and Ace in the Hole's three wait functions `32c85430` / `32c85500` / `32c855d0` (new). Nothing misreads
  before their skill phases: routed with the states, **CH2b** and **CH3b** kickoffs amended ("Amended by CH1p").
- Checklist 7ag marked `(Rena: CH1p, no code change ...)`.

**Hooks 7h / 7l / 7o / 7p / 7x.** 7h `InitWaitStateChange( "LESI_WAIT_STATE_CHANGE" )` and 7l `InitBoundingStates( "LESI_" )` are CH1m's (in the `_LIVE`
`.i` once each, absent flag-off; checklist rows already marked). 7o `SET_LIMIT_MAX_G_SPEED` (script x1, `LESI_DAMAGE_BOUNDING_DOWN` -4000; reader
`X2GUUser.cpp:16560`), 7p `TIME_STOP_IMMUNE_STATE` (script x18; reader `:18197`) and 7x `AT_COMMAND` / `AT_SPECIAL` (script x63; user gate ported by
Raven CH1p) are script data that CH1 final brings with `Lire_ElvenRanger.lua`.

**June-only keys.** None owned here (CH0 amendment): `BOW_SKILL_STATE` and `CREATE_TRAP_BLADE` were ported by CH1j inside `CommonStateStart` `32ca81b0`
(funcmap `Char Rena CH1j`); `IMMUNE_TO_SUPER_ARMOR_DOWN` (script x10) is read only by the engine `31fdaaa0` (from `CX2GUUser::CommonStateStart`
`@32057e68`), not by `32ca81b0`: backlog Stage 2 bullet (`JUNE_PORT_BACKLOG_ORDER.md:246`). §5 census row stays as CH0 left it.

**Hygiene.** No code file changed, so no flagcheck. `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GUUser.cpp`: `allow-listed: 7`,
`RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`), `_LIVE` off / on:
`X2GULire_ElvenRanger.cpp` 0 errors, 38 / 38 warnings; `X2GUUser.cpp` 0 errors, 39 / 39. Full build skipped: no play-test. No funcmap or `JUNE_BUGS` row.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-rena CH1p review` (Opus 5.5 high).

### CH1p - Review (2026-11-09)

**Verdict: PASS, no code change** (flags `_LIVE`, still `//#define`, and `_CORE` untouched). Two kickoff amendments corrected. Read from the exe bytes
(`june2017_re/disasm_range.py` over `unpacked_x2.exe`, `dispsweep.py`, an `E8` scan) and fresh `cl /P` / `/Zs` probes (`census.cl_cmd`):
- **Pressed index.** `32c6a970`: saves `+0x8ea0` / `+0x8ea4` / `+0x8ea8`; vslot `0x7f8` (lookup) true and `+0x8ea0` != 0 -> vslot `0x7fc`( id, 1 ) and
  `32006b30`( id ); `31fca8e0`( id, 1 ) templet `+0x98` == 3 / 5 (`ASUT_ANYTIME` / `ASUT_ANYTIME_EXCEPT_AIR`) restores all three; used -> `+0x88bc` =
  `+0x8ea4` + 1 `@32c6aa13`, else `+0x8ea0` restored `@32c6aa22`. = the `_LIVE` body (`.cpp:8407-8436`) line for line. `32c89a20`: `31fc86e0`( `+0x8ea4`, 0 )
  `@32c89a6f` (`MarkSkillSlotUsed`), `+0x8ac0` = `+0x8ea4` + 1 `@32c89bac`, then `31fcd3d0` (random-mission hook).
- **`_LIVE` `.i`, class segments only** (on / off): `CountUsedSkill` 0 / 1, `SAKP_` 0 / 0, `CurrentSkillKeyManager` / `GetPressedSkillState` / `m_kSkillSlotKey`
  0 / 0, `m_eSpecialAttackKeyPressed` 1 / 1 (the writer), `iPressedSkillSlotIndex` 3 / 13 (on: the lookup's declare, out-arg, `m_iChosenSkillSlotIndex =`;
  no 4 + 4 decode), `InitWaitStateChange` / `InitBoundingStates` 1 / 0 each. `X2GUUser.cpp` `CommonSpecialAttackEventProcess`: `bMarchSlotIndex` adds
  `UT_LIRE != ...` only with `_LIVE`; flag-off has no `UT_LIRE` in the function.
- **Sweep.** `dispsweep.py 8ac0 8b58 88bc` (raw-byte scan, so the uncarved functions are covered): in `32c6a8d0-32cb6c6d` only `32c89bac` and `32c6aa13`;
  no `+0x8b58`. `E8` scan of the range: `31fce710` at `@32c84fd9` / `@32c8545b` / `@32c8552b` / `@32c855fb` only, `31fd7500` at `@32c84ad9` only; no
  `E9` thunk to either anywhere in `.text`. March's `LESI_A_LWS_BACK_KICK_EventProcess` (`.cpp:32273-32294`, flag-independent) tests no skill key
  (cancel macros, any-key -> `USI_WAIT` after 0.66), so nothing misreads before CH2b.
- **Routes, corrected.** CH3b: the three Ace in the Hole waits match (key -> 0x129 / 0x12b / 0x12d; left `+0x86da` / right `+0x86de` or `+0x980c` reaching
  `32007b10( 1, id )` -> `32278070` and `LESI_WAIT`; tail-jump vslot `0x7d4`), but the MP refund is `32007b10( 0x42, id )` x 3 / x 2 / x 1 (`fmul [33439b50]`
  = 3.0 `@32c854be`, `fadd st0,st0` `@32c8558e`, unscaled `@32c8565e`), not x 3.0 in all three: amendment fixed. CH2b: the key test sits **inside** the
  `+0x8c10` / `+0x8c14` window, and the amendment lacked user vslot `0xe8`( `32007b10( 0x8f, id, 0 )` ) -> `+0x7d85` = 1 and the later StateChange `0x127`
  (time past `+0x8c18` or animation `+0x2ac`): added.
- **Hooks / keys.** As above, 7h / 7l once each in the `_LIVE` `.i`. `IMMUNE_TO_SUPER_ARMOR_DOWN` stays engine `31fdaaa0` (backlog Stage 2), not CH1j's `32ca81b0`.

**Hygiene.** No code file changed, so no flagcheck. `junediverge.py --check-all`: `allow-listed: 197`, `RESULT: PASS`. `enumscan.py --check
X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. Probes (`/c /Zs /EHsc`), `_LIVE` off / on: `X2GULire_ElvenRanger.cpp` 0 errors, 38 / 38 warnings;
`X2GUUser.cpp` 0 errors, 39 / 39. Full build skipped: no play-test.

**Nothing to play-test.** CH1q kickoff checked. Next: `/x2-phase char-rena CH1q` (Opus 5.5 high).

### CH1q - Done (2026-11-09)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`, still `//#define`, now with two homes; June bodies under `_LIVE`, the new helper under `_CORE`;
15 funcmap rows `Char Rena CH1q`; every block tagged `char-rena CH1q`). Files: `X2GULire_ElvenRanger.cpp` / `.h`, `X2Define.h`, `X2DamageEffect.cpp`,
`X2GUUser.cpp`, `KTDXLIB/Always.h`, `KncWX2Server/Common/ServerDefine.h`, `tools/flagcheck.py`. Per-row verdicts: `june2017_re/char_rena/ch1q_nf_outcome.tsv`
(99 map rows: 80 = March, 9 June bodies, 1 June value, 1 June by CH1m, 7 flag-off arms that drop, 1 partly June).
- **Flag plumbing (decision 1 = (a)).** `_NF` defines `ADD_RENA_SYSTEM` in Always.h (after its `_LIVE` check, plus `#error`s for `SPAWN_INHERIT`,
  `MP_CHANGE_TEXT`, `SKILL_STATE_BUFF`) and in a `ServerDefine.h` second home, because `CommonPacket.h` (`CAC_NATURAL_FORCE`, `GetNaturalForce`) is read
  before Always.h; `SECOND_HOMES` row added. Both stay commented until CH1 final (CH1 kickoff amended: toggle both, `--pch force`).
- **Baseline.** Before any edit, a `/Zs` probe with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM` of the 10 NF files (class, `X2GUUser`, `X2DamageManager`,
  `X2DamageEffect`, `X2EffectSet`, `X2GUNPC`, `X2Game`, `X2GageData`, `X2GageManager`, `X2GageUI`) gave 0 errors: March's dormant code compiles as is.
- **June bodies (inside March's `ADD_RENA_SYSTEM` functions, March in `#else`).**
  `AttackResultByType` `32ca7130`: only `NFT_SPECIAL_ACTIVE` takes the skill path (March also took `NFT_ACTIVE`, which now charges like a combo on its
  first hit); full = count < gauge `+0x120`; 3 hits per point. `ConsumeNaturalForce` `32c9f7a0`: Open Mind MP refund `UpNowMpWithText( n x +0x97e0 )`,
  nothing taken in full hyper mode (vslot 0x9c; Rena = base false), always `SetNaturalForceBuff( false )` (JUNE_BUGS 574). `UpNaturalForce` `32cb6290`:
  +1 when `GetCharacterSpecializationA()` (+0x6c50) beats `GetRandomFloat( 0 )`. `SetNaturalForceBuff` `32c85200`: erase BTI 333, then BFI 355 once per
  point (`( BFI, 1, NULL, 0, false )`; the bool is only the gauge mode). `Set` / `UnSetSpecificValueByBuffTempletID` `32c85260` / `32c85290`: also
  `BTI_TRANSCENDENTAL_HYPER_MODE`; drop at count < 1 (March < 3). `AdjustDamageDataBeforeDamageReact` `32c6d4a0`: empty (no RT_BIG_DAMAGE override).
  `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX_FrameMove` `32c8b2c0`: CH1f's X shape, 6 MP, `DamageEffect_RSR_Double_Jump_Combo_XX_Arrow` x3.
- **= March (switched on as they are):** CreateGageData, Init (foot boxes 1.3 = `33472a10`), SetNaturalForceCount, GetNaturalForceBuff, the relate
  Check / CheckCombo / Delete (`32cb5d10` = the `std::vector` copy constructor, no port), SetNaturalForceEffect, UpdateNaturalForceEffect, the six state
  callbacks CH1m registered, IsFullNaturalForce (inlined in June), IsHyperActiveSkill. `GetNaturalForceCount` / `IsEmptyNaturalForce` / `UpNaturalForce`
  lose `__forceinline` under `_NF` (.h and .cpp): June has them out of line for engine callers (CH1q2).
- **New.** `RestoreGageSnapshot()` (`32c89230`, user vslot 0x744; my unit -> `SetNaturalForceCount( m_iSnapshotNaturalForceCount )`; stand-in: base
  `3204da60` not ported, as Chung's / Elsword's). Member `m_fOpenMindMpPerNaturalForce` (+0x97e0, ctor 0 @32ca4252; written by InitPassiveSkillState
  `32ca78b0` from Open Mind's SA 0x4f: CH4a kickoff amended, with `+0x97e4` = SA 5).
- **Engine rows.** `X2Define.h`: `NATURAL_FORCE_VALUE_PER_HIT` 3 under `_NF` (June 3.0 @32ca73fd; 6 in `#else`). `X2DamageEffect.cpp`
  RT_CREATE_DAMAGE_EFFECT: the `_NF` arm keeps both June sequences (`31f85910 @31f860fd-31f86148`: first-attack / relate copy, SetScale, InheritFromParent,
  ApplyCreateDamageEffectRate); flipping alone would have dropped SPAWN_INHERIT's. `X2GUUser.cpp`: the studio "NF:" / "NFP:" overlay goes under `_NF`
  (absent in June). The first-attack sets, the effect / effect-set copies, `SetNowDamageRelateSkillData` `31fe28f0`, the user resets (`3204ea20`
  @3204f0f2 and three more), NPC-die cleanup (`3230bc70`-`3230bca7`, before the mission gate: independent) and the summon copy (`3230c3a0`) = March;
  the flipped `#ifndef ADD_RENA_SYSTEM` arms (`m_iNaturalForceType`, vslot 0x300 calls, `X2GUUser.h` virtual, DamageReact hit counts) are equivalent.
- **Members routed by CH1k:** `+0x973c` (ctor and dtor store 7, no reader in Rena's range) and `+0x978c` (ctor zero vector, no reader) are not natural
  force and stay unnamed; `+0x9768` readers `32c7f350` / `32c88330` / `32c9ece0` are Spiral Strike's (CH1k's inference).

**Hygiene.** `flagcheck _NF --worktree --files <7 files>`: 3 PASS, 2 DEFINE, 2 OTHER (`_LIVE` / `_CORE` blocks), 0 FAIL; 0 deleted, 0 non-ASCII inserted,
0 EOL mismatches; RESULT FAIL only for the commented `//#define`. `_LIVE` on the class files: 2 OTHER, 0 FAIL (same). `junediverge --check` (5 code files):
`RESULT: PASS` (allow-listed 8). `enumscan --check X2GULire_ElvenRanger.cpp`: 0; the one row in `X2DamageEffect.cpp` (:1705) predates this phase.
Probes (`/c /Zs /EHsc`): off: class, `X2GUUser`, `X2DamageEffect`, `X2DamageManager`, `X2GageData` 0 errors; `_LIVE` + `_NF`: the 10 NF files +
`X2GameUnit.cpp` 0 errors; `_NF` alone stops at the Always.h `#error` (C1189). `cl /P` with `_LIVE` + `_NF`: `CAC_NATURAL_FORCE` x2 in `X2GUUser.i`, no
`NFP: %f`, no `__forceinline` GetNaturalForceCount, `NATURAL_FORCE_VALUE_PER_HIT = 3`. Full build skipped: no play-test.

**Routed.** CH1q2 (kickoff amended): dev fill key `X2Game.cpp:5855` (June's F7 key `322ed57f` differs), NPC `AttackResultByType` `32773ee0` (now called for
every non-user attacker), `ApplyHyperModeBuff`, emulator `CAC_NATURAL_FORCE` and the save column, and the gauge contract the class code relies on
(+0x114 count, +0x120 max, empty <= 0). CH4a: Open Mind's two members. Checklist rows L118 / L149 / L258 / L278 marked `(Rena: CH1q)`.

**Nothing to play-test.** Next: `/x2-phase char-rena CH1q review` (Opus 5.5 high).

### CH1q - Review (2026-11-09)

**Verdict: PASS, no code change** (flags `_NF` and `_LIVE` still `//#define`, `_CORE` untouched). Read from the exe bytes (`charport_adis.py rena`,
`funcdis.py`, Rena vtable `335185bc`) and fresh `cl /P` / `/Zs` probes (`census.cl_cmd`, US_SERVICE):
- **AttackResultByType `32ca7130`.** `+0x56c` (`m_NaturalForceType`): `js` out, `jle 2` -> combo path, `jne 3` out, so only `NFT_SPECIAL_ACTIVE` (3) takes
  the skill path and `NFT_ACTIVE` (2) charges as a combo (enum `X2DamageManager.h:677-682` = June). Skill path: vslot `0x420` (`32c87780` Check) `== 1`,
  count `jle` 0 out, `+0x5a9` byte, vslot `0x424` (`32c86740` Delete), `BOOM_SKILL` with the default args (`CreateInstance` defaults = June's pushes), min( count,
  byte ), `+0x23c` / `+0x238` x n, ConsumeNaturalForce, return (no `+0x5a4` store). Combo path: `+0x5a4 == 1` (`FAC_FIRST_ATTACK`); count < gauge `+0x120`;
  `< 10` or `== 100` and `32c858c0` (CheckDamageRelateComboData) -> charge + 1; 3.0 `@32ca73fd` `test ah, 0x41; jp` = charge >= 3; combo bomb = type 1 and
  count > 0, relate `< 10` direct, else Check true -> Delete, false -> skip both; charge - 3.0; inlined UpNaturalForce `@32ca75e2` (+1 when `+0x6c50` >
  vslot `0x1cc` roll); `+0x5a4 = -1` `@32ca765b` on every combo-path exit past the gate. = the `_LIVE` body line for line.
- **Helpers.** ConsumeNaturalForce `32c9f7a0`: `fldz; fcomp +0x97e0; test ah, 5; jp` = 0 < m -> `32278070`( n x m ); vslot `0x9c` (slot 39 IsFullHyperMode)
  = `31e4c550` (false) in Rena's vtable and March has no override (`X2GameUnit.h:2323` only); `32a317d0`(1) + `32a36df0`( count - n ) = March's
  SetNaturalForceCount (changed `+0x112`, clamp 0..max = `X2GageData.cpp` SetNowNaturalForce); buff( false ). UpNaturalForce `32cb6290` = the inlined copy.
  SetNaturalForceBuff `32c85200`: `32a317b0` (`+0x110` buff mode), `3229e160`( 333 ), loop i < count re-read: vslot `0x21c` (`322a6960`) args ( 0x163, 0, 1,
  0, 0 ) = June ( BFI, creator, level, adjust, skip ) = March's ( `BFI_BUFF_NATURAL_FORCE` 355, 1, NULL, 0, false ). Set `32c85260` / UnSet `32c85290`: 5 /
  0x336 (`BTI_HYPER_MODE` / `BTI_TRANSCENDENTAL_HYPER_MODE` 822), UnSet count `< 1` -> mode false + `32281440`( 333 ). `32c6d4a0` = `ret 4`. `32c89230` (vslot
  `0x744`): vslot `0x198` = `31ff1f30` IsMyUnit, `+0x92b4` through the same gauge pair, `jmp 3204da60` (stand-in as noted). `32cb50c0` / `32cb50e0` = count /
  count <= 0.
- **XX FrameMove `32c8b2c0`** vs X `32c8aa40`: 6.0 (X 4.0), no class test (X picks by class `0xd` / `0x67`), `DamageEffect_RSR_Double_Jump_Combo_XX_Arrow`
  x3 at -90 / -70 / -110, stores arrow 1 backSpeed then rates, arrows 2 / 3 rates then backSpeed, `Dummy2_Lhand` not-enough-MP at -90, CommonFrameMove. = port.
- **Equal rows confirmed:** XX StateStart `32c6ac20` (SetNowDamageRelateSkillData( 100 ), CommonStateStart), CreateGageData `32ca7060` (my unit -> copy
  `32a37e00` else default `32498a30`), Init `32c883d0` (base, my unit -> CannonBallCount sync, SI_NONE, relate >= 10 -> 0, Lfoot / Rfoot 1.3), SetNaturalForceEffect
  `32c9f830` (stop, 5-case Loop table, PlayEffectSet), NPC-die `3230bc70-3230bca7` (owner `32759f00`, vslot `0x420` then `0x424` with NPC `+0x8f6c`).
- **RT_CREATE_DAMAGE_EFFECT `31f860fd-31f86148`:** child null -> straight to `31f6ea20`; else `+0x7a4` / `+0x7a8` copies (DamageData +0x5a4 / +0x5a8),
  SetScale( `+0x698` ), `31f8fdb0`, `31f6ea20`. The port calls InheritFromParent on a null child too, which returns at once (`X2DamageEffect.cpp:10433`): equal.

**Flag plumbing.** `_NF` alone: `Always.h(12016) : fatal error C1189` ("needs `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`"). `_LIVE` + `_NF` `.i`: `CAC_NATURAL_FORCE` x2
in `X2GUUser.i` (enum + the `m_cCharAbilType` use), `NFP: %f` 0, `__forceinline` on GetNaturalForceCount 0, `NATURAL_FORCE_VALUE_PER_HIT = 3` x1 per `.i`, `= 6` 0;
the RT_CREATE_DAMAGE_EFFECT arm keeps one CreateInstance (copy, SetScale, InheritFromParent, ApplyCreateDamageEffectRate in June's order).

**Hygiene.** `flagcheck _NF --range 60bdb407^..60bdb407 --files <7>`: 3 PASS, 2 DEFINE, 2 OTHER (`_CORE` / `_LIVE`), 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL;
RESULT FAIL only for the commented `//#define`. `junediverge.py --check-all`: allow-listed 197, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`:
0 A/B rows. Probes (`/c /Zs /EHsc`), off and `_LIVE` + `_NF`: `X2GULire_ElvenRanger.cpp`, `X2GUUser.cpp`, `X2DamageEffect.cpp` 0 errors each. Full build
skipped: no play-test.

**Nothing to play-test.** CH1q2 kickoff checked (its "Amended by CH1q" section is current). Next: `/x2-phase char-rena CH1q2` (Opus 5.5 high).

### CH1q2 - Done (2026-11-09)

**Landed** (flag `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`, still `//#define`; new temporary `SERV_IRUHADEV_JUNE_CHAR_RENA_NF_DEBUG`, `//#define`, `#error`s
without `_NF`; every block tagged `char-rena CH1q2`; 6 funcmap rows `Char Rena CH1q2`). Files: `X2GageUI.cpp`, `X2GameUnit.cpp`, `X2Game.cpp`,
`X2GageData.cpp` (debug line only), `Offline/X2OfflineStatTable.cpp`, `KTDXLIB/Always.h`. Per-row verdicts: `june2017_re/char_rena/ch1q2_nf_outcome.tsv`
(the 27 CH1q2 map rows + the two June-only pieces: 2 June bodies, 1 June value, 1 drop, 1 routed, the rest = March). Read from the exe bytes (`funcdis.py`).
- **Gauge data `CX2RenaGageData` = March.** Layout +0x110 / +0x111 / +0x112 bytes, +0x114 now / +0x120 max (KProtectedType), size 0x12c, ctor max 5;
  SetNow `32a36df0` clamps 0..max; Update `32a34f20` = base + SetNow + SetMax( 5 ) + the two bytes; InitWhenGameIsOver `32a34f80` = base
  (SetHyperModeRemainTime( 0 )) + mode false; packet get `32a32500` (count as char) / set `32a34fa0` (type 5 ? count : 0, SetMax( 5 ), SetNow, changed);
  Copy / operator= / Clone = March. `CommonPacket.h` (`CAC_NATURAL_FORCE` 5, `GetNaturalForce`) = March; the gauge manager case `3248e531` = March.
- **Gauge UI.** `CX2RenaMyGageUI::InitUI` `32e6e190` differs: June calls InitWakeOrb (vslot 0x54) before opening `DLG_Rena_NaturalForce.lua` (the order
  Chung's and Elsword's InitUI took). June's body under `_NF`, March in `#else`; `june_march_divergent.tsv` row added. Ctor, OnFrameMove `32e71970`,
  UpdateNaturalForceUI `32e5ec10` = March.
- **NPC `AttackResultByType` `32773ee0` NF branch (`@3277452b-32774732`) = March** line for line (FAC_FIRST_ATTACK, NFT_ACTIVE or NFT_SPECIAL_ACTIVE, owner
  Check / not empty / Delete, min( count, relate byte ), Consume, BOOM_SKILL at the hit unit's Bip01 or GetPos, rate 1.0, damage x n).
- **ApplyHyperModeBuff Rena branch (June-only, checklist 7ao).** `32298590 @322985df-32298632`: user whose unit type is UT_LIRE: count > 0 -> `UpNowMp(
  count x 15.0 )` (vslot 0xf0 = `3208dd00`, double `33449130`, no MP text), then `UpNaturalForce( 5 )` (written `MAX_NATURAL_FORCE_VALUE`). Added first
  in `CX2GameUnit::ApplyHyperModeBuff` under `_NF`, before the transcendence test (Rena has no override: her vslot 0xc0 = base).
- **Dev F7 fill key dropped under `_NF`.** June's KeyProcess `322f7170` has no auth-level F7 fill; its F7 is the training step `322ed400` (Rena
  `32cb6290( 1 )`), which `JUNE_PORT_BACKLOG_ORDER.md` Stage 2 already holds for every class. `#ifdef _NF` comment `#else` March `#endif`.
- **Emulator.** `CharAbilTypeOf` gets `UT_LIRE -> CAC_NATURAL_FORCE` under `_NF` (as the live server's `GSGameDBThread.cpp:989-993`, which loads the count
  from `P_GUnitPlayInfo_SEL`, so the live game keeps it across a relog). The save already has a generic `abil_count` column written from every
  play-status update (`Handlers_Field.cpp:279`): no schema change, no save edit, no `db_backup` needed.
- **Already done elsewhere:** `FriendshipNatureData` (CH1h, live March code), `32cb50c0` / `32cb6290` out of line (CH1q), the flipped arms (CH1q).
- **`_NF_DEBUG` lines** (for CH1 final's play-test; CH1 kickoff amended): `RENA NF gauge UI` (InitNaturalForceUI), `RENA NF gauge: count n / max` (on change),
  `RENA NF from packet` (SetMyPlayStatusFromPacket), `RENA NF awakening` (ApplyHyperModeBuff).

**Hygiene.** `flagcheck _NF --worktree --files <6>`: 1 PASS (`X2Game.cpp`), 1 PASS~ (`X2OfflineStatTable.cpp`, one blank line), 4 OTHER (Always.h comments;
the `_NF_DEBUG` nests in `X2GageData` / `X2GageUI` / `X2GameUnit`), 0 FAIL, 0 ERROR; `_NF_DEBUG`: 1 PASS, 1 DEFINE, 4 OTHER (the `_NF` blocks), 0 FAIL.
Both: 0 deleted, 0 non-ASCII inserted, 0 EOL mismatches; RESULT FAIL only for the commented `//#define`s. `junediverge --check <5>` and `--check-all`:
`RESULT: PASS`. `enumscan --check <5>`: 0 A/B rows. Probes (`/c /Zs /EHsc`) of the 5 code files: off, `_LIVE` + `_NF`, and `_LIVE` + `_NF` + `_NF_DEBUG`:
0 errors each; `_NF_DEBUG` alone stops at its `#error` (C1189). `cl /P` with `_LIVE` + `_NF`: InitUI = InitStatusUI, InitWakeOrb, OpenScriptFile;
`iNaturalForceCount * 15.0` x1 in `X2GameUnit.i`; `CAC_NATURAL_FORCE;` x1 in `X2OfflineStatTable.i`; the fill key's `UpNaturalForce(` gone from `X2Game.i`.
Full build skipped: no play-test.

**Routed.** The training F7 step `322ed400` stays with backlog Stage 2 (Rena case listed there). Checklist row l.963 (7ao) marked `(Rena: CH1q2)`.

**Nothing to play-test.** Next: `/x2-phase char-rena CH1q2 review` (Opus 5.5 high).

### CH1q2 - Review (2026-11-09)

**Verdict: PASS, no code change** (`_NF`, `_NF_DEBUG`, `_LIVE` still `//#define`). Read from the exe bytes (`funcdis.py`, `charport_consts.read_va`) and fresh
`cl /P` / `/Zs` probes (`census.cl_cmd`, US_SERVICE):
- **InitUI `32e6e190`.** `32e66e60` (InitStatusUI), vslot `0x54` (Rena vtable `3353363c` + 0x54 = `32e5b310` = `CX2MyGageUI::InitWakeOrb`: removes pictures
  6 / 5 / 4 / 3 of `StaticPVPGameStateWakeOrb`, March has no Rena override), `31cd5910`( `335347e8` = `DLG_Rena_NaturalForce.lua` ), `31cd3f20`( `335322c8` =
  `g_pStatic_NF_Gage_Back` ), NULL test, vslot `0x68`( 1 ) = SetShow( true ) = the `_NF` arm in that order (InitNaturalForceUI = the inlined tail). `.i`: InitStatusUI,
  InitWakeOrb, OpenScriptFile, InitNaturalForceUI.
- **ApplyHyperModeBuff `32298590 @322985cd-32298632`.** `+0x264 != 0` -> `32298cd0` (NPC path, Transcend T5); `+0x867c` unit, `+0xaf0` templet, type `== 3`;
  `32cb50c0` (gauge `+0xc04` -> `+0x114`) count, `jle` skip; `fild` x `33449130` (15.0) -> float -> vslot `0xf0` (`335185bc` + 0xf0 = `3208dd00` UpNowMp, one
  arg); `push 5; call 32cb6290` whatever the count; then the transcendence part. Rena's vslot `0xc0` = `32298590` (no override). = the `_NF` block.
- **Equal rows confirmed:** SetNowNaturalForce `32a36df0` (max < n -> max, n < 0 -> 0, else n; KProtectedType), SetMyPlayStatusFromPacket `32a34fa0` (base
  `32a33fa0`, `+0x15 == 5` ? `+0x16` : 0, SetMax( 5 ) `32a36ef0`, SetNow, `+0x112` = 1), InitWhenGameIsOver `32a34f80` (`31e8c9d0`( 0.0 ) = SetHyperModeRemainTime,
  `+0x110` m_bNFBuffMode = 0), UpdateNaturalForceUI `32e5ec10` (changed test / clear, `g_pStatic_NF_Gage`, picture i shown if count > i, SetShow( true )), NPC
  `32773ee0 @3277452b-32774732` (`+0x5a4 == 1`, type 2..3, owner, vslot `0x420` / IsEmpty `32cb50e0` / byte `+0x8f6d` read before vslot `0x424`, min, Consume
  `32c9f7a0`, GetPos or hit unit `Bip01`, `NATURAL_FORCE_EXPLOSION_BOOM_SKILL` 1.0, `+0x23c` / `+0x238` x n) = `X2GUNPC.cpp:32462-32500`.
- **Dev key.** KeyProcess `322f7170` F7 (0x41) sites: `@322f7802` (chat key, string id 0x7d92), `@322f7b6e` (training step `322ed400`), `@322f7ba8` (item 0x8bc
  gate -> `32894490`( 1 )); none fills natural force. `.i`: March's fill call gone; only Rena's fill is inside the `_NF` arm.
- **Emulator.** `UT_LIRE -> CAC_NATURAL_FORCE` = `GSGameDBThread.cpp:989-993`; `Handlers_Field.cpp:280` saves `m_iCharAbilCount` to `abil_count`,
  `X2OfflineServer.cpp:1224-1225` reads it back with the type: no schema change.

**Hygiene.** `flagcheck _NF --range 9f1b971a^..9f1b971a --files <6>`: 1 PASS, 1 PASS~ (one blank line), 4 OTHER (Always.h comments, the `_NF_DEBUG` nests), 0 FAIL;
`_NF_DEBUG`: 1 PASS, 1 DEFINE, 4 OTHER, 0 FAIL; both 0 deleted, 0 non-ASCII, 0 EOL; RESULT FAIL only for the commented `//#define`s. `junediverge.py --check-all`:
allow-listed 197, `RESULT: PASS`. `enumscan.py --check <5>`: 0 A/B rows. Probes (`/c /Zs /EHsc`) of the 5 code files: off, `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`,
and + `_NF_DEBUG`: 0 errors each; `_NF_DEBUG` alone: `Always.h(12040) : fatal error C1189`. Full build skipped: no play-test.

**Nothing to play-test.** CH1 kickoff checked (its "Amended by CH1q2" section is current). Next: `/x2-phase char-rena CH1` (Opus 5.5 high).

### CH1 - Done (2026-11-09)

Opus 5.5. **The deploy unit is live.** `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`Always.h:11938`), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`Always.h:12013` and its second home
`ServerDefine.h:4792`; defines `ADD_RENA_SYSTEM`) and `_NF_DEBUG` (`Always.h:12037`, for the play-test; the review retires it) are defined, so Rena runs June's
registration, state bodies, virtuals, skill head, pressed-slot space and natural force (CH1a-CH1q2). Every `#ifndef` guard in the `_LIVE` / `_NF` blocks names a flag
`#define`d earlier in `Always.h` (none lives only in `ServerDefine.h`, Raven's KTDXLIB trap).
- **Script:** `luasrc.py adopt-june data036 Lire_ElvenRanger.lua` (dry run first): June's file replaces March's whole as `ljconv` bytecode, 209,576 B sha1 `028e14dc`,
  no core-list row, no sidecar. Record `june2017_re/june_adopt_20261109_180855.tsv`, backup `24965799\kom_backup_stage_june\20261109_180855`.
  `luabind.py check Lire_ElvenRanger`: 0 methods, 0 unbound, `RESULT: PASS`.
- **Motions:** `kom.py stage-june june2017_re/char_rena/ch1_held_motions.tsv`: `Motion_Lire.x` (6,614,767 B), `_GA` (2,757,337), `_RTR` (2,966,118), `_SR` (3,135,823),
  `_WS` (2,739,968) replace March's in data033 (5 replaced; backup `kom_backup_stage_june\20261109_180909`).
- **Packed / copied (user approved):** `PACK OK data036` (~1 `Lire_ElvenRanger.lua`), `PACK OK data033` (~5 motions);
  `COPY OK data036 ... 14051967 bytes, 270 entries, md5 7b59ffcad6e3e1158a354ec069b7c4c7`, `COPY OK data033 ... 178842869 bytes, 165 entries, md5 8a858fe781945edadc938f67827c6a1f`.
- **`[JUNE_CHAR] RENA DATA`** (`_LIVE`, end of June's `InitState`, `X2GULire_ElvenRanger.cpp:3897`, offline_server.log): class, script, motion, state count,
  `wait_state_change`, as Raven's. `X2OfflineLog.h` is included under `_LIVE` (`:3-6`). No `RENA CORE` line: nothing in the class core needs one beyond `RENA DATA`.
- **7h:** June's `LESI_WAIT_STATE_CHANGE` has `ANIM_NAME = ""` (`Lire_ElvenRanger.src.lua:517`); Rena's `CommonStateStart` (`:7548`) hands off to
  `CX2GUUser::CommonStateStart`, the path the five earlier classes take.
- **`ReservedSkillStart`** (CH1k) is live: the `.i` with `_LIVE` + `_NF` has the base virtual and `CX2GULire_ElvenRanger::ReservedSkillStart` (`:8354`).
- **Lua load check:** `luasrc.py core --check` `CORE CHECK PASS: 0 FAIL, 0 WARN`, `ledger --check` `LEDGER CHECK PASS`. Core files Rena reads: `Enum.lua`, `UnitTemplet.lua`,
  `NPCTemplet.lua` (source), `DamageEffect.lua`, `EffectSet.lua` (June bytecode). Load-time errors show only in `ErrorLog.txt`: reference
  `data\ErrorLog_pre_CH1_rena.txt` (1,046,346 B, the log of the last pre-CH1 run, 2026-11-08 19:09) and, for Rena loads, `ErrorLog_pre_CH0b_rena.txt`.
- **Scans:** `charport_keys.py Lire_ElvenRanger`: 256 keys, `june-only=6 neither=3 march-only-site=4 both=243`; CH0 had 241 / 8 / 3 / 4: the two class keys CH1p gave
  readers (`BOW_SKILL_STATE`, `DAMAGEDATA_CHANGE_TIME*`) moved to both; the 6 left are CH0's (`BBT_` / `BFT_` no reader, camera pair Stage 9, `IMMUNITY_EDT` and
  `IMMUNE_TO_SUPER_ARMOR_DOWN` Stage 2). `charport_scan.py` (census rerun first: 26 PASS, 0 FAIL; `census.tsv` / key matrix reverted after): `no NUMERICAL_DATA_INDEX` 65 / `no DAMAGE table` 12, the whole body identical to `char_rena/plan_charport_scan_20261107.txt`. Nothing new.
- **junediverge, now that `_LIVE` is on:** two `GT_PVP` hits in Rena's `_LIVE` bodies (CH1h's `ReInit` / `InitComponent`; `--check-all` never saw them while `_LIVE` was off).
  June tests type 1 only at both (`32ca4a40` @32ca4ac9 vslot 0x10c `cmp eax, 1 / jz`; `32c895b0` @32c895f4 `dec eax / jnz`): two `june_diverge_allow.tsv` rows, no code change.
- **Checklist:** `(Rena: CH1 ...)` at l.16, 423, 427, 509, 548, 592, 674, and `(Rena: CH0b / CH1 ...)` at l.760. l.120 (`ADDITIONAL_ATTACK_TRIGGER_TYPE`): Rena's script has none.
- `MODS.md`: "Stage 7 Rena, CH1" (flags, the two replaced assets and their backups, revert order).

Hygiene: `flagcheck.py JUNE_CHAR_RENA_LIVE --worktree --files X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h KncWX2Server/Common/ServerDefine.h`: `1 PASS, 0 PASS~, 0 DEFINE,
2 OTHER, 0 FAIL, 0 ERROR`; 0 deleted / non-ASCII / EOL / encoding; `RESULT: PASS` (the OTHERs are the define flips). `_NF`: `0 PASS, 1 DEFINE, 2 OTHER, 0 FAIL`, `RESULT: PASS`;
`_NF_DEBUG`: `3 OTHER, 0 FAIL`, `RESULT: PASS`. `junediverge.py --check X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h`: `allow-listed: 5`, `RESULT: PASS`; `--check-all`:
`allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`. One-file probes (`/c /Zs /EHsc`, all three flags on): 12 files
(`X2GULire_ElvenRanger`, `X2GUUser`, `X2GageUI`, `X2GageData`, `X2GageManager`, `X2DamageManager`, `X2DamageEffect`, `X2EffectSet`, `X2GUNPC`, `X2Game`, `X2GameUnit`,
`Offline/X2OfflineStatTable`) exit 0, 0 errors.
Build (`--pch force`): `DEPLOY OK: X2_offline.exe is 26,148,864 B, dated 2026-11-09 18:19:39, md5 034256dd8f3a8d5c34d5269001f808ac, equal to X2/US_SERVICE/x2.exe (2026-11-09 18:19:30); built at main 9f62c3a7 + 3 uncommitted code file(s).`
`EXESTR OK: 4 of 4 found in X2_offline.exe, 26148864 B, dated 2026-11-09 18:19:39, md5 034256dd8f3a8d5c34d5269001f808ac` (`RENA DATA` wide x1, `RENA NF gauge` x2,
`from packet` x1, `awakening` x1).

**Play-test (user) is the gate:** units 86 (3), 87 (12), 88 (13), 91 (24), 89 (102), 12 (103), 90 (114), none transcended, skills in slots 0-3 only; combos, dash, jump,
double jump, a bounding hit, damage and stand-up, riding, awakening; natural force from combo first hits, its gauge, spending, the awakening refund and a relog; `RENA DATA`
per class; ErrorLog against the references. Skills are not tested here (Raven's precedent, user 2026-11-06: an unported skill cannot be cast); the two-casts check
(cool time, MP, `ReservedSkillStart`) is CH2a's first hand-off.
**Play-test (2026-11-09, run 19:24-19:32 on md5 034256dd): user "all test passed".** `offline_server.log`:
- `RENA DATA` for all 7 classes, each `script LIRE_ELVENRANGER.lua motion MOTION_LIRE.X wait_state_change 1`; states 3: 169, 12: 177, 13: 172, 24: 175, 102: 187,
  114: 184, 103: 193 (the review checks them against `ch1m_reg_by_class.tsv`).
- `RENA NF`: `gauge UI ... g_pStatic_NF_Gage_Back shown` and `from packet: type 5` on every entry; the count climbs 0 -> 5 by combo hits (class 3: 1 at 19:25:17 ... 5 at
  19:25:48) and drops on spending (5 -> 4, 3 -> 2, 1 -> 0 on every class); the last entry (back to unit 86, left at 4) reads `from packet: type 5, count 4 -> 4` and the
  gauge shows `4 / 5`: the count survives a relog through `abil_count`.
  **Open:** no `RENA NF awakening` line (no awakening with points held in this run); the review asks for that step once before retiring `_NF_DEBUG`.
  **Debug-line flaw:** `RENA NF gauge` logs every frame, not only on a change (16,4xx lines; CH1q2 meant "every count change"); harmless, it goes with `_NF_DEBUG`.
- START / WIN / LOSE: ErrorLog's `C3e-2 roll` / `rebind` lines pick `LESI_*_START` / `_WIN` / `_LOSE` tables for every job, list sizes 1-6, `list size 0` 0,
  `re-bind skipped` 0.
ErrorLog against `ErrorLog_pre_CH1_rena.txt` / `ErrorLog_pre_CH0b_rena.txt`: `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 11 / 7, `lua error` 3 / 4 / 3,
no missing animation. The new line groups come from places, gear and effects those runs never opened: optional `.XET` probes (202 groups: Rena motion / costume / effect
meshes), `result : N` on three Rena sounds (as Raven's), `HQ_CUTIN_LENA_N.DDS.GFX` (March's cut-in table plus `.gfx`, CH5's), `LIRE_BASIC_FACE_NW` textures (CH0b's
`JUNE_BUGS` 562), Elysion `[ELYDBG]` / local-map control lines, `JUNE_FX MP_CRASH` traces. Nothing broke in play; no new routing. `AddGroupChainQuest` unbound x1 is pre-CH1.

### CH1 - Review (2026-11-09)

Opus 5.5. **Verdict: passes.** One source change: `_NF_DEBUG` retired (`Always.h:12037`, `//#define`); the `RENA NF` lines go dark, `RENA DATA` stays
(`X2OfflineLog.h` is included under `_LIVE`).
- **Deploy unit:** data036 `Lire_ElvenRanger.lua` 209,576 B sha1 `028e14dc...` header `1B 4B 4C 81` = `june_adopt_20261109_180855.tsv` row; no `.src.lua` sidecar.
  `stage-june --dry-run ch1_held_motions.tsv`: `5 staged (0 new, 0 replaced, 5 identical)`. `kom.py status data036 data033`: both in sync, `+0 / -0 / ~0` vs live
  (data036's `[stale .src.lua: compile]` is older non-Rena sidecars, as Raven's). `exestr.py "RENA DATA" --form wide`: `EXESTR OK: 1 of 1` (wide x1), md5 `034256dd`.
- **State counts, exact for all 7 units:** C++ registrations on the class path + 4 hook states (3 `DAMAGE_BOUNDING` from `InitBoundingStates( "LESI_" )`, 1
  `LESI_WAIT_STATE_CHANGE`) + 9 `USI_EVENT_*` + 20 slot states + 1 key 0 + learned-skill sub-states (`SetEquippedSkillLevel` `_LIVE` -> `SetEquippedSkillLevelStateData`
  `m_StateList[ id ]`, only ids the class path did not register): 3: 135+4+9+20+1+0 = 169; 12: 143+4+9+20+1+0 = 177; 13: 136+4+9+20+1+2 = 172 (Rapid Shot 23212);
  24: 141+4+9+20+1+0 = 175; 102: 149+4+9+20+1+4 = 187 (Reflex Magic, Violent Attack x2, Slide Kick finish); 103: 139+4+9+20+1+20 = 193 (Rapid Shot 2, Gungnir 12,
  Ace in the Hole 6); 114: 149+4+9+20+1+1 = 184 (Eradication). `ch1m_regcheck.py` (scratch copy, no write): `matched 992 registrations, 0 problems`; tsv rows per class
  = C++ + 4 hooks + 11 `LEFT` (June Common riding / phoenix / dance). Raven's "C++" term held the 4 hooks; here they are counted apart, same totals.
- **START / WIN / LOSE:** `list size 0` 0, `re-bind skipped` 0; `C3e-2 roll` lines pick `LESI_START` / `LESI_WIN` / `LESI_LOSE2` ...
- **Natural force (decision 1 = (a)):** gauge UI line on every entry; the count climbs one point per three first hits and drops on spending; relog `from packet: type 5,
  count 4 -> 4`. **Awakening (second run 19:53-19:55, asked once, unit 12 class 103):** `RENA NF awakening: count 1, MP +15, count after 5` = June's `32298590` (`count x 15.0`,
  then `UpNaturalForce( 5 )`, capped at 5). The gauge then reads 5 -> 4 -> 2 -> 3 as played.
- **ErrorLog** (first run, against `ErrorLog_pre_CH1_rena` / `_pre_CH0b_rena`): `KEM_ERROR` 0 / 0 / 0, `Motion Error` 0 / 0 / 0, `[LUA_SRC]` 7 / 11 / 7 (TutorSystem,
  ChainQuestInfo `AddGroupChainQuest`), `lua error` 3 / 4 / 3; no missing animation. New Rena lines are unopened content only: two lobby-ready `JUNE_FX UNIT_VIEWER_FX_CREATE`
  (`RNW` / `RTR` sword-ready meshes), `LIRE_BASIC_FACE_NW` textures (JUNE_BUGS 562), four `HYPER_MESH_LIRE_*` `.XET` probes. Second run: `KEM_ERROR` 0, `Motion Error` 0,
  `[LUA_SRC]` 7. `offline_packets.log`: `EXCEPTION` 0; `UNHANDLED` only `E_DISABLE_HB_CHECK_REQ` (pre-existing). Nothing to route.
- **Ghidra spot-checks:** `ReInit` `32ca4a40`: `GetNowMp() < 50.0` (`test ah, 5` + `jp`), then vslot 0x10c `cmp eax, 1 / jz` -> `SetNowMp( 50 )` only outside PvP; then
  `MakeRoomForEntangleTrap( 0, 1 )` `32c95670`, friendship reset (`+0x962c`: byte +8 = 1, float +0x10 = 0), `MakeRoomForAngerOfElfTrap( 0, 1 )` `32c9e130`, charge `+0x9744`
  = 0, own unit -> count sync, `32c85200( 0 < count )` = the port in order. `InitComponent` `32c895b0` @32c895f4: own unit (vslot 0x198), `dec eax / jnz` (type 1 only) ->
  gauge `+0x112 = 1` (`32a317d0`) + `32a36df0( 0 )` = `SetNaturalForceCount( 0 )` (`:34683`, changed flag + count). Both `june_diverge_allow.tsv` rows (l.20-21) confirmed.
- **Checklist / scans:** `(Rena: CH1 ...)` at l.16, 423, 427, 509, 548, 592, 674 and `(Rena: CH0b / CH1 ...)` at l.760. `charport_keys.py Lire_ElvenRanger`:
  `june-only=6 neither=3 march-only-site=4 both=243` = CH1's. `luabind.py check Lire_ElvenRanger`: 0 unbound, `RESULT: PASS`; `core --check` / `ledger --check` PASS.
- **Kickoffs:** CH2a matches (it carries the two-casts / `ReservedSkillStart` check). CH5's line refs were stale (CH1m-CH1q moved them): it now names the cut-in table
  `:160-197` and the live `_LIVE` InitState switch `:2845-2854` (March's `#else` copy `:3915-3928`).

Hygiene: `flagcheck.py JUNE_CHAR_RENA_LIVE --range 21a4c2e0^..21a4c2e0`: `1 PASS, 2 OTHER, 0 FAIL`, `RESULT: PASS`; `_NF`: `1 DEFINE, 2 OTHER, 0 FAIL`, `RESULT: PASS`;
`_NF_DEBUG`: `3 OTHER, 0 FAIL`, `RESULT: PASS`. `_NF_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, 1 deleted line (the define, now `//#define`), `RESULT: FAIL`
only for that. `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s)`.
Build: started after the define change, not awaited (debug-retire rule, user 2026-11-03).
CH2a and CH5 are ready.

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

### CH2a - Done (2026-11-09)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `_SKILL_DEBUG`, `Always.h:12051` / `:12063`; `#error` without `_LIVE`, `JUNE_ENUM_NAMES`, `JUNE_SKILL_NUMERICAL`, all three defined earlier in the file) is defined: Rena's seven class-3 skills run June's registrations (before this every one fell into the Power Attack default). All 46 class-map rows are read from `charport_adis.py rena --phase CH2a` listings: **18 functions written** (inside March's function under the flag, March in `#else`), **28 are June = March** (no code). `funcmap.tsv` rows `Char Rena CH2a` (46); `MODS.md` "Stage 7 Rena, CH2a" has the damage table; `JUNE_BUGS` 575.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6b2e0` `32c6b410` `32c6b620` `32c6b820` `32c6b8b0` `32c6bc30` | `LESI_SI1_LE_PIERCE_ARROW_Init`, `_SI3_LE_PHOENIX_STRIKE_`, `_SI1_LE_ASSAULT_KICK_`, `_SI2_LE_PERFECT_STORM_`, `_SI3_LE_AERO_TORNADO_`, `_SI_A_LE_BACKJUMP_SHOT_Init` | June's own preload lists (15 / 11 / 6 / 5 / 7 / 5 names; all in `ch0_assets.tsv`, `june-bytes`) |
| `32c86bc0` `32c86bf0` `32c86c20` `32c86c50` `32c86e20` `32c86e50` | `..PIERCE_ARROW_FrameMove` (+ `_HYPER`), `PHOENIX_STRIKE_`, `ASSAULT_KICK_`, `AERO_TORNADO_FrameMove` (+ `_HYPER`) | cut-in and `CommonFrameMove` only: the arrow, particles, meshes and damage effects moved to the state's effect sets (`EffectSet_SA_Rena_*`, all in the shipped `EffectSet.lua`); Assault Kick's memo test calls June's always-false stub `31fca9e0` and is not written |
| `32c94e20` | `LESI_SI_A_LE_BACKJUMP_SHOT_FrameMove` | fires at 0.361 s ( March 0.4 ) from `Dummy2_Lhand` ( March `Dummy1_Rhand` ); `ARROW_BACKJUMP` x `m_nBackShotCount` ( z -45 / -45 -30 / -15 -30 -45 ), `GetPowerRate()` with no arrow-rate factor |
| `32c768c0` | `LESI_SI_A_LE_BACKJUMP_SHOT_Start` | base templet 23200, else the upgrade 23220 ( both with the upgrade swap ); count = ability 30 at `max( 1, GetSkillLevel( id, 0, 0, 1 ) )` |
| `32c74a40` `32c74d50` | `LESI_SI_LE_POWER_ATTACK_EventProcess` (+ `_HYPER`) | walk, back-walk, dash and Z / X cancels need the foot on a line ( `RENA_GROUND_*`, new `RENA_GROUND_ZX_CANCEL_EX` ) |
| `32c76070` `32c76280` | `LESI_SI2_LE_PERFECT_STORM_EventProcess` (+ `_HYPER`) | skill, then dash and walk cancels with the foot test ( March's `DASH_` / `WALK_CANCEL_AFTER` ) |
| = March | FMF x10, CameraMove x7, EndFuture, EventProcess x6 ( Pierce Arrow, Phoenix, Assault Kick, Aero x2, Backflip ), Power Attack FM x2, Perfect Storm FM x2 | `funcmap.tsv` says which; Perfect Storm's FM is March's live part under `BALANCE_PATCH_20131107` |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex`: the second skill switch is `switch( GetUpgradeBaseSkillID( eSkillID_ ) )` under `_LIVE` ( June `32207210`; March's `switch( eSkillID_ )` in `#else` ), with seven cases ahead of the CH1n default ( both the base and the upgrade id of each skill are labels ): Rail Stinger ( table `LESI_SI1_LE_PIERCE_ARROW`, hyper `_HYPER` ), Perfect Storm ( one table, own hyper functions ), Aero Tornado ( `..._HYPER` table and functions ), Assault Kick and Phoenix Strike ( one table and one function set for both slots ), Backflip Shot ( Init / Start / FMF / FM / EP / EndFuture on both ) and Siege Mode ( `LESI_SIEGE_READY`, CH1f's functions ). No hyper row has an Init. `X2SkillTree.h`: ten `RNSI_SA_RENA_*` names ( 23000-23004, 23026-23030 ). New macro `RENA_GROUND_ZX_CANCEL_EX` ( June `32c74a40 @32c74c81` ).

**Checklist** `(Rena: CH2a ...)`: §1 `Lire_ElvenRanger` row ( 0 numerical-only state tables here ), §4 l.178, the l.717 / 729 type rows, §7i l.440 ( no cached-rate use ). §7ai ( BFI arguments ): none of the 18 functions calls it; 7h / `NF` rows: the two `ch0_nf` rows are the Power Attack cancel lines, written unconditionally ( March's `ADD_RENA_SYSTEM` block stays dead ).

**Findings, none blocking.** (1) March's `*_HYPER_*` duplicates of Assault Kick, Phoenix Strike, Pierce Arrow ( except FrameMove ), Aero EP stay unregistered and dead. (2) Backflip Shot's state table also lists `EffectSet_A_Rena_BackjumpShot` while C++ still fires `ARROW_BACKJUMP`: both are June's, kept. (3) Rail Stinger's script sets DASH_CANCEL_AFTER 0.783 but its EventProcess `32c75060` has no cancel lines, so the dash cancel never opens ( JUNE_BUGS 575, kept ); Perfect Storm's keys ( 1.15 / 1.08 / 1.15 ) are read by its EventProcess; Assault Kick, Phoenix Strike, Aero Tornado and the Power Attack default set none.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with JUNE_CHAR_RENA_SKILL_DEBUG --worktree --files X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `1 DEFINE, 2 OTHER, 0 FAIL` ( `.cpp` OTHER = my id-switch header sits in `_LIVE`, `X2SkillTree.h` = `JUNE_ENUM_NAMES` ); with `--with JUNE_CHAR_RENA_LIVE` on the `.cpp`: `1 PASS` ( +374 / 0 deleted ), `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three code files: `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. One-file probe ( `/c /Zs /EHsc` ): `X2GULire_ElvenRanger.cpp` exit 0, 0 errors.
- `DEPLOY OK: X2_offline.exe is 26,148,864 B, dated 2026-11-09 20:37:44, md5 7ced4bc276190c4e8b4cb3bf4b8bbf56, equal to X2/US_SERVICE/x2.exe (2026-11-09 20:37:37); built at main 941c7994 + 3 uncommitted code file(s).`
- `EXESTR OK: 1 of 1 found in X2_offline.exe, 26148864 B, md5 7ced4bc2` ( wide `RENA SKILL equip skill` ).

**Play-test hand-off.** Unit 86 `ElvenRangerT` ( Rena, class 3, lv 99 ), set 1 is already slotted: A Backflip Shot ( Enhanced, 23220 ), S Rail Stinger ( Enhanced, 23026 ), D Perfect Storm ( Enhanced, 23027 ), C Aero Tornado ( Enhanced, 23028 ). For Assault Kick ( Enhanced 23029 ), Phoenix Strike ( Enhanced 23030 ) and Siege Mode ( 23201 ) say "swap" and I move them in. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: motion and effects, the slot shows a cool time after the first cast, the MP bar drops, a second cast during the cool time is refused ( unless the dev gate lets it through ). Then `python tools/logs.py --grep "RENA SKILL"` ( four `equip` lines ) and say "check logs".

**Play-test (2026-11-09, runs 20:40-20:42 and 20:44-20:45 on md5 7ced4bc2): user "all passed"** for both sets on unit 86 `ElvenRangerT` (cool time, MP and the second-cast refusal on screen as handed off). Set 1 ( A 23220, S 23026, D 23027, C 23028 ) and set 2 ( A 23029, S 23030, D 23201 ): `RENA SKILL equip` lines slot 0-3 / 0-2 with states 49 / 51 / 53 / 55 and 49 / 51 / 53. Casts seen: Backflip Shot ( `EffectSet_A_Rena_BackjumpShot`, ARROW.OGG, a hit at 20:41:12 ), Rail Stinger x3 ( `DamageEffect_A_Rena_RailStinger_Square`, cut-in 01 ), Perfect Storm x2 ( the MEMO effect set and damage effects, cut-in 02 ), Aero Tornado x4 ( `DamageEffect_SA_Rena_AeroTornado`, cut-in 03 ), Phoenix Strike x2 ( `DamageEffect_SA_Rena_PhoenixPhoenixStrike_Square` ), Assault Kick x2 ( 1st and 2nd hit effect sets ); Siege Mode has no effect-set line ( its states are CH1f's ). ErrorLog: `KEM_ERROR` 0, `Motion Error` 0; new lines only optional `.XET` probes ( `RENAASSAULTKICK_M1_01 / 02`, `M2_01 / 02`, `PHOENIXSTRIKE_MAGICP01 / M01`, `RENA_PERFECT_STORM_MEMO_*`, `LENANORMALARROWGROW` ) and `result : 47` on `ARROW.OGG`. **Not played:** the hyper ( awakened ) casts of the five attacks and the base ids 23000-23004 / 23200 ( the review's walk or CH6 ); `_SKILL_DEBUG` is retired by the review.

### CH2a - Review (2026-11-09, Opus 5.5): PASS, `_SKILL_DEBUG` retired, no other code change

Read from `charport_adis.py rena --phase CH2a` listings (46 functions), plus June's exe bytes (`disasm_range.py`) and Ghidra for the registration `32cb06d0`.
- **Registration.** `@32cb0868`: `push [esp+0xac]; call 32207210; cmp 0x5c30 / sub 0x59d8; ja 32cb2eef` = `switch( GetUpgradeBaseSkillID( id ) )`, default = CH1n's Power Attack. Ghidra `@32cb08b9` .. `@32cb0b38` and `@32cb1517` / `@32cb156c`:
  callbacks and table names equal `ch0_skill_listing_rows.tsv` row for row. Rail Stinger's hyper copies FMF / Camera / EP and sets its own FM `32cb6000`; Assault Kick and Phoenix copy all four; Perfect Storm and Aero Tornado set their own hyper four.
  Correction to the Done note: "no hyper row has an Init" holds for the five attacks and Siege; **Backflip Shot's hyper row does set Init** (`[ebp+0x18] = 32cb3d30`), as the port does. The 14 ids equal June's `Enum.src.lua`.
  March's own cases left in the switch are labelled with March ids (3000 / 3200 / 2000 ...), not June's, so switching on the base id moves no other skill: the class 12 / 13 / 24 skills fall into the default as before.
- **Five "= March" claims, all confirmed.** Assault Kick EP `32c75d80` (the 14 keys of `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC`, 1.3), Aero Tornado EP `32c76490` (2.8; `32c76560` byte-equal), Perfect Storm FM `32c86dc0`
  (cut-in `Dummy1_Rhand` 0.06 index 1, CommonFrameMove = March's live part), Backflip Shot FMF `32c76960` (0.4 <= t <= 0.71), Phoenix Strike camera `32c947c0` (camera 1, 0.1 / 0.8, three sub cameras, `m_pXSkinAnim`).
- **Ground cancels.** `32c74a40`: anim end, `SKILL_CANCEL_AFTER` (June's foot test is in the live `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` macro), walk `+0x8c98`, back-walk `+0x8c94`, dash `+0x8c9c`, Z then X `+0x8c88`, each `bFootOnLine`;
  June's two Z / X branches equal the port's one branch with the inner `if` (nothing follows but `CommonEventProcess`). `32c76070`: skill, dash, walk (4 foot tests); `32c76280` byte-equal.
- **Backflip Shot.** `32c94e20`: `EventTimerOneshot( 0.361 )`, `GetCloneFrame( Dummy2_Lhand )` translation, count 1 / 2 / 3 (else nothing) with z -45 / -45 -30 / -15 -30 -45, `CreateInstance` tail `false, -1, [+0x4050], [+0x95a8], [+0x95c0], [+0x95c4], [+0x9680], [+0x95cc]`
  = the arrow tail CH1d already uses (`GetForceDownValueRateOfRenaArrowAttack()` ...), back speed `+= [+0x9614]`. `32c768c0`: 23200 then 23220 (swap 1), `GetUnit()` test, `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, ability `0x1e` (= the loader's `SA_PROJECTILE_FIRE_COUNT`, `s0a_templet_keys.tsv:68`).
- **FrameMoves / Init lists.** The six FrameMoves' cut-in arguments equal June (0.06 / 0 x2, 0.001 / 2, 0.06 / 0, 0.19 / 2 x2); Assault Kick's dropped memo branch only writes `+0x790` when `31fca9e0` (returns 0, funcmap 1182) is true.
  The six Init lists equal June name for name, in order and by loader (Texture / XMesh / XSkinMesh), incl. Aero Tornado's second `AeroTornado01.X`.
- **Logs.** Second run: 3 `RENA SKILL equip` lines (23029 / 23030 / 23201, states 49 / 51 / 53), `[JUNE_SKILL] SLOT` rows as handed off, 0 EXCEPTION. No damage lines in this build; the damage is the data's (`MODS.md` table).
  `AddGroupChainQuest` unbound x1 is a quest script's, not this phase's.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --range d8e0eab8^..d8e0eab8`: 1 DEFINE, 2 OTHER, 0 FAIL, 0 deleted, RESULT: PASS (adding `--with ..._RENA_LIVE`: `.cpp` PASS +374; Always.h then reports a
  tool ERROR on CH5's `#if defined( ..._RENA_UI ) && ...` line, the same file read with LIVE forced, not a defect). `junediverge.py --check-all`: allow-listed 202, RESULT: PASS. `enumscan --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows.
- **Change.** `KTDXLIB/Always.h:12063` `//#define SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (bytepatch, CP949 / CRLF kept); `flagcheck ..._SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: 1 DEFINE, 1 deleted (the `#define` itself), 0 encoding change, "not #defined" FAIL as a retire expects;
  `junediverge --check KTDXLIB/Always.h`: RESULT: PASS. Full build started, not awaited (debug-retire rule); it stands in for the one-file probe.
- **Next.** CH6 walks the hyper casts and base ids (`kickoffs/char-rena/CH6.md` "Amended by CH2a review"). `kickoffs/char-rena/CH2b.md` is current (its "Amended by CH2a" re-defines `_SKILL_DEBUG`).

### CH2b - Done (2026-11-09)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (CH2a's; `Always.h:12055` adds a second `#error`: `..._USEFUL_VALUE`, `..._USER_SKILL_LEVEL_HELPERS`, both defined earlier) and `_SKILL_DEBUG` re-defined (`Always.h:12067`). The seven class-12 skills now run June's registrations (before this every one fell into the Power Attack default). All 19 class-map rows read from `charport_adis.py rena --phase CH2b` listings: **11 functions written** (inside March's function under the flag, March in `#else`), **8 are June = March** (no code), plus one engine helper (below). `funcmap.tsv` rows `Char Rena CH2b` (20); `MODS.md` "Stage 7 Rena, CH2b" has the damage table.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6b950` `32c6ba20` `32c6ba70` `32c6c710` `32c6c860` `32c6cf80` | `LESI_SI1_LCR_SPINNING_KICK_Init`, `_SI2_LCR_CRESCENT_KICK_`, `_SI3_LCR_DIVE_KICK_BOMBING_`, `LESI_A_LE_RISING_FALCON_`, `LESI_A_LWS_BACK_KICK_`, `LESI_A_LWS_ASSAULT_IMPACT_Init` | June's own preload lists ( 15 / 5 / 13 / 9 / 7 / 4 names; all `june-bytes` in `ch0_assets.tsv` ) |
| `32c86e80` `32c86eb0` | `..SPINNING_KICK_FrameMove`, `..CRESCENT_KICK_FrameMove` | cut-in ( `Dummy1_Rhand` 0.1 / 0, 0.14 / 1 ) and `CommonFrameMove` only: meshes, particles, damage effects and the memo test moved to the state's effect sets |
| `32c76700` | `LESI_SI2_LCR_CRESCENT_KICK_EventProcess` | in the air past `GetUsefulValueFuture( 0, 0 )` ( script `USEFUL_VALUE0` 1.9 ) the state falls ( `<`, foot off a line ); then the 14 keys past 2.5 s |
| `32c7d3a0` | `LESI_A_LWS_ASSAULT_IMPACT_EventProcess` | dash and walk cancels need the foot on a line ( `RENA_GROUND_DASH_` / `_WALK_CANCEL` ), then `SKILL_CANCEL_AFTER` |
| `32c84ea0` | `LESI_A_LWS_BACK_KICK_EventProcess` | CH1p's held-key test: inside `m_fEventTime[0]`'s window ( script `EVENT_PROCESS_TIME0` 0.2 / 100 / 0.62 ) the Enhanced skill ( templet `m_iDeleteSkillIDByUpgrade != 0` ) pressed again after a hit ( `GetAttackSuccess( true )` ) spends `SA_MP_CONSUMPTION` once ( `FlushMpOnce` ) and sets flag 0; the flag past `stateChange` or at animation end -> Low Kick ( `0x127` ); then ground dash, walk, `SKILL_CANCEL_AFTER`; March's Z / X skip branch is gone |
| = March | Spinning Kick FMF `32c6ba10` + EP `32c76630`, Dive Kick EP `32c76800` + FM `32c86ee0` ( cut-in `Bip01_R_Foot` 0.14 / 2 ), Rising Falcon FMF `32c6c790` + EP `32c7ae20` + FM `32cad390` ( strings `DAMAGE_DATA_NEXT` / `_LAST` ), Landing EP `32c7a6c0` | `funcmap.tsv` says which |

**New engine piece ( needs Opus's read ).** Back Kick's `vslot 0xe8` is `3208dcb0` ( identified here from the listing, not in the map ): `+0x7638` set -> false; else `FlushMp( mp )` and, on success, set the flag. A byte search of June's `.text` finds the flag's writers: `32050759` ( `CommonStateStartFuture`, after the socket early return ) and `322a5eb2` ( constructor ); the only reader is `3208dcb0`. Ported as `CX2GUUser::FlushMpOnce` + `m_bMpFlushedThisState` ( `X2GUUser.h` / `.cpp`, `_SKILL`; the member stays in `CX2GUUser` as `m_bInstanceKill` did ), cleared in `CommonStateStartFuture`, initialised in the constructor. No other user calls it in this phase.

**Registration.** Seven cases added to the id-switch block ( both ids of each skill are labels ): Spinning Kick ( one table, FMF / FM / EP shared by the hyper state ), Crescent Kick and Dive Kick ( `_HYPER` table name, normal functions; Dive Kick has no FMF ), Rising Falcon ( hyper without Init ), Back Kick, Low Kick ( CH1e's Init / FM / EP, no StateStart: March's `LESI_SA_LCR_LOW_KICK_StateStart` is unreferenced ) and Assault Impact. `X2SkillTree.h`: ten `RNSI_*` names ( 23006, 23034, 23007, 23043, 23222, 23203, 23228, 23229, 23206, 23231 ). Templet-swap check: the March readers of these ids ( `SetEquippedSkillLevel`, Rising Falcon / Low Kick ) were already June's ( CH1i ); the `SI_A_LCR_LOW_KICK` reads in the unreferenced StateStart stay dead.

**Checklist** `(Rena: CH2b ...)`: §1 `Lire_ElvenRanger` ( 7 numerical-only tables: Spinning Kick 2, Rising Falcon 3, Assault Impact 2 ), §2 l.116 / 117, §4 l.178, §7 l.256, §7h l.427, §7i l.440, §7u l.638 ( the `Effectset_Rena_DiveKickBombing*` names have no caller in June's FrameMove ), §7ae l.729. §7ai: none of the 11 functions calls BFI. NF rows: none for CH2b. **No JUNE_BUGS row** ( no June quirk found in the 19 functions ). Found, not ported: skill icon `23210` ( Sniping Ranger, CH2c ) has `NO_PIECE` ( `DLG_Lena_SKILL_ScreenShot_04` ); CH2c stages it.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --worktree --files` the five code files: `3 PASS, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` ( `X2SkillTree.h` OTHER = `JUNE_ENUM_NAMES` ); with `--with ..._RENA_LIVE` on the three code files `3 PASS`; `1 deleted line(s)` = `//#define _SKILL_DEBUG` re-defined; `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the five files ): `allow-listed: 12`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. `luabind.py check Lire_ElvenRanger`: 0 unbound, PASS. `skillicons.py check` 23005-23007, 23031-23043, 23202-23206, 23222, 23228-23231: PASS ( 23210 is CH2c's ).
- `DEPLOY OK: X2_offline.exe is 26,150,400 B, dated 2026-11-09 21:29:56, md5 cd5d508149cdbac4ab9f7b7fbc607226, equal to X2/US_SERVICE/x2.exe ( 2026-11-09 21:29:49 ); built at main 72f21095 + 5 uncommitted code file(s).`
- `EXESTR OK: 2 of 2 found in X2_offline.exe, 26150400 B, md5 cd5d5081` ( wide `RENA SKILL flush mp once`, `RENA SKILL equip skill` ); a second `EXESTR OK: 3 of 3` for three new preload names.

**Play-test hand-off.** Save set up ( backup `db_backup/els_db_20261109_212247_pre_sql.sql` ): unit 87 `CombatRangerT` ( Rena, class 12, lv 99 ), set 1 slotted: A Rising Falcon ( Enhanced 23222 ), S Spinning Kick ( Enhanced 23034 ), D Crescent Kick ( Enhanced 23031 ), C Dive Kick Bombing ( Enhanced 23043 ). Set 2 ( say "swap" and I move it in ): A Assault Impact ( 23231 ), S Back Kick ( Enhanced 23228, now learned ), D Low Kick ( 23229 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: motion and effects, a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. Crescent Kick: stay on a line ( the air fall only fires off a line after 1.9 s ). **Back Kick chain ( set 2 ):** cast S, hit an enemy, press S again before the kick ends: Low Kick follows, MP drops a second time, one `flush mp once` log line. Then `python tools/logs.py --grep "RENA SKILL"` ( four `equip` lines per set ) and say "check logs".

**Play-test (user 2026-11-10): passed.** Logs: 7 `RENA SKILL equip` lines (states 49 / 51 / 53 / 55 bank A, 59 / 61 / 63 bank B); effect sets seen for Spinning Kick (51, 3), Crescent Kick (53, 4), Dive Kick Bombing (55, 4 sets), Rising Falcon Landing (182, 2), Back Kick (61, 16), Low Kick (63, 4; 251, 5 chained); **5 `flush mp once 20.00` lines** for the Back Kick chain, 5 Low Kick chain sets; ErrorLog 0 KEM_ERROR / Motion Error; packets 0 EXCEPTION. Assault Impact has no effect set in the log (user: cast passed). Not CH2b's: `AddGroupChainQuest` unbound (quest script), `APPLIED_SKILL_ID` stand-in lines.

### CH2b - Review (2026-11-10, Opus 5.5): PASS, `_SKILL_DEBUG` retired, no other code change

Read from `charport_adis.py rena --phase CH2b` listings (19 functions), June's exe bytes (`disasm_range.py`) and Ghidra xrefs.
- **Registration.** The seven cases equal `ch0_skill_listing_rows.tsv` row for row: Spinning Kick one table, hyper without Init; Crescent and Dive Kick `_HYPER` table names, normal functions, no hyper Init (Dive Kick no FMF);
  Rising Falcon hyper without Init; Back Kick, Low Kick and Assault Impact hyper rows copy Init too. Spot-check `@32cb0b67` / `@32cb0b89`: both Back Kick rows push the same name `3351ad4c`. The 14 ids equal June's `Enum.src.lua`.
- **Eight "= March" claims, all confirmed.** Spinning Kick FMF `32c6ba10` and Rising Falcon FMF `32c6c790` (thunks to vslot `0x7c8`), Spinning Kick EP `32c76630` (14 keys, 1.4), Dive Kick EP `32c76800` (animation end only),
  Dive Kick FM `32c86ee0` (`Bip01_R_Foot` 0.14 / 2, then CommonFrameMove = March's live part under `BALANCE_PATCH_20131107`), Rising Falcon EP `32c7ae20` (end on a line -> `0xe3`), FM `32cad390` (`DAMAGE_DATA_NEXT` `3346f050` / `_LAST` `335197f0`),
  Landing EP `32c7a6c0` (two `GetValue`, direction-change test, end, 8-key ASDC after SKIP_TIME).
- **FrameMoves.** `32c86e80` / `32c86eb0`: `ChangeWorldColorByHyperMode`, cut-in `Dummy1_Rhand` 0.1 / 0 and 0.14 / 1, CommonFrameMove: equal.
- **Crescent Kick EP `32c76700`.** End; `[+0x8ef4] < t` (`test ah, 5; jp`) and foot off -> JUMP_DOWN; the 14 keys and `2.5 < t` -> WAIT: equal.
- **Assault Impact EP `32c7d3a0`.** End; dash `+0x8c9c`, walk `+0x8c98` (-> WAIT), skill `+0x8c90`, each with the foot test, then the hyper tail of `SKILL_CANCEL_AFTER`; no Z / X: equal.
- **Back Kick EP `32c84ea0`.** End; flag 0 set and (`stateChange < t` or end) -> `0x127`, flag cleared; else `keyInputStart <= t < keyInputEnd` and templet (`31fca8e0( id, true )`) `+0x178 != 0` and `31fce710` and `GetFrameData( true ) +0x118` (= `bHit`, future base `+0x7d14`)
  and vslot `0xe8`( `32007b10( 0x8f, id )` ) -> flag set, skip the cancels; with the flag set before `stateChange` June re-runs the window test, as the macro's `else if` does. Then ground dash, walk, skill: equal.
- **`FlushMpOnce` `3208dcb0`.** Body = June's `FlushMp` `3208dc70` inline (`GetGageData()` slot `0x2c`( mp, `IsMyUnit()`, 0 ), arg 2 unused) behind `+0x7638`; the port's `FlushMp` under `JUNE_DIVERGE_GAGE_TALLY` is that call. Writers confirmed: `32050759` (after the socket
  return, where the port clears it) and `@322a5eb2`, which is inside **`CX2GameUnit::CX2GameUnit` `322a2700`**, not the user ctor; Ghidra lists `3208dcb0` in 18 unit vtables (slot after FlushMp's). So June's is a `CX2GameUnit` virtual. Keeping it in `CX2GUUser` is accepted:
  the only reset is user-side, no NPC caller exists, and behaviour is identical. `funcmap.tsv` row corrected; the `X2GUUser.h` comment ("user vslot") is left as written.
- **Preload lists.** The six `Init` lists equal June name for name, in order and by loader (15 / 5 / 13 / 9 / 7 / 4); all 37 names are `june-bytes` in `ch0_assets.tsv`.
- **Logs.** 7 `RENA SKILL equip` lines (23222 / 23034 / 23031 / 23043 bank A, 23231 / 23228 / 23229 bank B), 5 `flush mp once 20.00` lines, one per chained cast; 0 KEM_ERROR / Motion Error; 0 EXCEPTION. No damage lines in this build (the `JUNE_FX` APPLIED_SKILL_ID lines are the known stand-in); damage is the data's (`MODS.md`).
- **Hygiene.** `flagcheck ..._RENA_SKILL --with ..._SKILL_DEBUG --range b2eca553^..b2eca553`: 3 PASS, 1 DEFINE, 1 OTHER (`X2SkillTree.h` = `JUNE_ENUM_NAMES`), 0 FAIL, 1 deleted (the re-defined `#define`), RESULT: PASS (with `--with ..._RENA_LIVE` the three code files PASS; Always.h's known tool ERROR on CH5's `#if defined` line).
  `junediverge.py --check-all`: allow-listed 202, RESULT: PASS. `enumscan --check` the Rena cpp and `X2GUUser.*`: 2 B rows, both `X2GUUser.h:2555-2556` (`NUI_NEIL_THE_SNIPER_EVENT` / `NUI_PROTON_BOMB_EVENT`), not CH2b's lines.
- **Change.** `KTDXLIB/Always.h:12067` `//#define SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (bytepatch, CP949 / CRLF kept); `flagcheck ..._SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: 1 DEFINE, 1 deleted, 0 encoding change, "not #defined" FAIL as a retire expects;
  `junediverge --check KTDXLIB/Always.h`: RESULT: PASS. Full build started, not awaited (debug-retire rule); it stands in for the one-file probe.
- **Next.** `kickoffs/char-rena/CH2c.md` "Amended by CH2b review": re-define `_SKILL_DEBUG`, stage icon 23210's `NO_PIECE` sheet, `FlushMpOnce` is available.

### CH2c - Done (2026-11-10)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (CH2a's; `Always.h:12059` adds a third `#error`: `..._SYNC_CLASS_FLOAT`, `..._USER_BONE_ROTATION`, `..._USER_COPY_STATE_CALLBACKS`, `..._SKILL_SLOT_10`, all defined earlier) and `_SKILL_DEBUG` re-defined (`Always.h:12071`). The seven class-13 skills now run June's registrations (before this every one fell into the Power Attack default). All 39 class-map rows read from `charport_adis.py rena --phase CH2c` listings: **26 functions written** (19 inside March's function under the flag, March in `#else`; 7 new), **13 are June = March** (no code). `funcmap.tsv` rows `Char Rena CH2c` (39); `MODS.md` "Stage 7 Rena, CH2c" has the damage table.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6b720` `32c6bb20` `32c6be30` `32c6c970` `32c6ca70` `32c6d690` | `LESI_SI2_LE_GUIDED_ARROW_Init`, `LESI_SI3_LSR_CRAZY_SHOT_Init`, `LESI_SA_LSR_WINDWARD_Init`, `LESI_A_LSR_SHOOTING_MAGNUM_Init`, `LESI_A_LGA_RAPID_SHOT_READY_Init`, new `LESI_A_LSR_MIND_OF_SHARPSHOOTER_Init` | June's own preload lists ( 14 / 21 / 14 / 5 / 8 / 6 names; all `june-bytes` in `ch0_assets.tsv` ) |
| `32c86d60` `32c86d90` `32c86f10` `32c86f40` | Guided Arrow ( normal, hyper ), Crazy Shot ( normal, hyper ) `FrameMove` | cut-in ( 0.06 / 1, 0.06 / 1, 0.16 / 2, 0.1 / 0 ) and `CommonFrameMove` only: arrows, particles and damage effects moved to the state's effect sets |
| `32c953d0` | `LESI_SA_LSR_WINDWARD_FrameMove` | `EffectSet_SA_RSR_WindWard`, unhyper in both states ( the hyper state takes the normal functions ), time = ability 1, size = ability `0x20` x3 |
| `32c9c340` | `LESI_A_LSR_SHOOTING_MAGNUM_FrameMove` | four `DamageEffect_RSR_Shooting_Magnum` arrows, no memo branch |
| `32c9cc10` | `LESI_A_LGA_ARC_SHOT_FrameMove` | five `DamageEffect_RGA_Arc_Shot` arrows ( +30 .. -30 ); the Enhanced skill ( templet `+0x178` ) adds -45 and an absolute 45 |
| `32c7c470` `32c7c670` `32c7cbe0` `32c7c990` | Shooting Magnum, Arc Shot, Rapid Shot finish and attack `EventProcess` | dash and walk cancels need the foot on a line ( `RENA_GROUND_*_CANCEL` ); Rapid Shot attack keeps March's 1.2 / 2.9 s end test |
| `32c7c870` | `LESI_A_LGA_RAPID_SHOT_READY_StartFuture` | the held key also covers the fifth slot of each bar ( `pureSkill5` / `pureSkillEx5` ), no default assert |
| `32c9d250` `32c9d420` | Rapid Shot attack / finish `FrameMove` | `DamageEffect_RSR_Rapid_Shot` ( arrow `z += aim` ); the finish state first runs the aim back to 0 ( Now class-skill float -50 above 3, +50 below -3 ) |
| new: `32c6cae0` `32c88050` `32c86620` `32c86720` | `LESI_A_LGA_RAPID_SHOT_READY_StateStart` / `_FrameMoveFuture` / `_FrameMove`, `UpdateRapidShotAimRotation` | aim angle ( member `m_fRapidShotAimAngle`, +0x97ec ): with `RNSI_P_RGA_RANDOM_SHOT_PLEASURE` learned up / down write ±50 into the Future class-skill float ( C3e-1 engine piece ), the helper adds float x elapsed, clamps ±40 and turns `Bip01_Spine1`; the attack and finish states take the same FMF |
| new: `32c831f0` `32c83220` | `LESI_A_LSR_MIND_OF_SHARPSHOOTER_EventProcess` / `_FrameMove` | end -> wait ( no foot test ); `CommonFrameMove`, then at 0.71 s `SetBuffFactorToGameUnit( templet 23210, 0 )` ( user slot 138 ) |
| = March | `32c6b7e0` `32c6b7f0` `32c6b800` `32c6b810` ( thunks ), `32c75ed0` / `32c75fa0` ( Guided Arrow EP, 14 keys past 1.8 s ), `32c76840` / `32c76880` ( Crazy Shot EP ), `32c76dd0` `32c76e30` `32c6bef0` ( Wind Wall Start / EP / End ), `32c6c9c0` ( Arc Shot Init ), `32c7c950` ( ready EP ) | `funcmap.tsv` says which |

**Registration.** Eight cases added to the id-switch block ( both ids of each skill are labels ): Guided Arrow ( hyper table `_HYPER`, March's `_HYPER_` functions ), Wind Wall ( hyper = normal Start / FM / EP / End, no Init ), Crazy Shot ( hyper table `_HYPER`, no Init ), Shooting Magnum and Arc Shot ( hyper copies Init / FM / EP ), Hunter's Steadiness ( new state; hyper = `CopyStateCallbacks( hyper, normal )` = June's `329cc2c0` call ), Rapid Shot ( ready with 6 callbacks, hyper copies them; the attack / finish states `@32cb0ded` / `@32cb0e33` carry `LESI_A_LGA_RAPID_SHOT_READY_FrameMoveFuture`; `RapidShotData` as March ). `X2SkillTree.h`: twelve `RNSI_*` names ( 23012 / 23013 / 23014 / 23032 / 23035 / 23048 / 23208 / 23209 / 23223 / 23233 / 23234 / 23236 ). Templet-swap check: the March readers of these ids ( `SetEquippedSkillLevel`, the Mind of Sharpshooter readers ) were already June's ( CH1i / CH1k ); the new FMs read the equipped templet by the base id with `true` ( upgrade swap first ).

**Checklist** `(Rena: CH2c ...)`: §1 / §2 l.116 / 117 ( no state-table numerics here: the damage effects' blocks run through the engine ), §4 l.178 ( 12 names ), §7 l.256 / l.440 ( no function writes the power rate ), §7h l.427, §7n l.493 ( answered: **Rapid Shot** ( 23212 ) carries 23415's up / down keys: writer = the FMF, readers = the three FMs ), §7u l.638 ( June's Arc Shot caller is `DamageEffect_RGA_Arc_Shot`; the March name is dropped; Guided Arrow's `Cryotron_Bolt_GUIDED_ARROW` is the hyper active, CH3d ), §7ae l.729 ( 14 ids ). §7ai: no BFI call in the 26 functions ( `SetBuffFactorToGameUnit( templet, 0 )` is not one ). NF rows: none for CH2c.

**Found, not ported / routed.** Skill icon `23210` / `23234` ( image piece `SI_A_RSR_MIND_OF_SHARPSHOOTER` in `DLG_Lena_SKILL_ScreenShot_04` ): **June's own TET lacks it** ( `stage-june` reports the sheet identical, 2 of 2 ): JUNE_BUGS row 576, `tools/data/skillicons_june_gaps.tsv` + `skillicons.py` prints `KNOWN_JUNE_GAP` ( the CH2b review's "stage it" cannot close it ). Rapid Shot's aim ( 23415 ) is a class-103 passive: not reachable on a class-13 save, so the hand-off does not test it ( CH3b ).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --worktree --files` the four code files: `2 PASS, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` ( `X2SkillTree.h` OTHER = `JUNE_ENUM_NAMES` ); `1 deleted line(s)` = `//#define _SKILL_DEBUG` re-defined; `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( the four files ): `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. `skillicons.py check 23012-23048 23208-23236`: `misses 0, known June gaps 2`, PASS.
- `DEPLOY OK: X2_offline.exe is 26,155,008 B, dated 2026-11-10 05:08:25, md5 7bbd4b5d241e8b2952347f7e7bc152db, equal to X2/US_SERVICE/x2.exe ( 2026-11-10 05:08:19 ); built at main 95fc9388 + 4 uncommitted code file(s).`
- `EXESTR OK: 6 of 6 found in X2_offline.exe, 26155008 B, md5 7bbd4b5d` ( wide `EffectSet_SA_RSR_WindWard`, `DamageEffect_RSR_Rapid_Shot`, `DamageEffect_RGA_Arc_Shot`, `DamageEffect_RSR_Shooting_Magnum`, `Lire_Uplight01.Y`, `RENA SKILL equip skill` ).

**Play-test hand-off.** Save set up ( backup `db_backup/els_db_20261110_045721_pre_sql.sql` ): unit 88 `SnipingRangerT` ( Rena, class 13, lv 99 ). Set 1 slotted: A Guided Arrow ( Enhanced 23032 ), S Wind Wall ( 23035 ), D Crazy Shot ( 23048 ), C Arc Shot ( 23223 ). Set 2 ( say "swap" and I move it in ): A Shooting Magnum ( 23233, now learned ), S Hunter's Steadiness ( 23234 ), D Rapid Shot ( 23236 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice, enemies in front: motion and effects ( Guided Arrow: three homing arrows; Wind Wall: the wind effect set; Crazy Shot: the big shot; Arc Shot: seven arrows in a fan; Shooting Magnum: four arrows ), a cool time on the slot after the first cast, the MP bar drops, a second cast during the cool time is refused. **Hunter's Steadiness:** the buff icon appears about 0.7 s into the cast and the next skill's MP cost is lower. **Rapid Shot:** hold its key: the ready motion, a burst of arrows for about 1.2 to 2.9 s, release -> the finish shot; try jumping out of it. Then `python tools/logs.py --grep "RENA SKILL"` ( four `equip` lines in set 1, three in set 2 ) and say "check logs".

**Play-test logs (2026-11-10, "check logs").** Exe md5 `7bbd4b5d`, run after the deploy. 7 `RENA SKILL equip` lines ( 23032 / 23035 / 23048 / 23223 bank A states 49 / 51 / 53 / 55; 23233 / 23234 / 23236 bank B states 59 / 61 / 63 ). Effect sets / damage effects seen per skill: Guided Arrow ( state 49, `EffectSet_SA_RSR_GuidedArrow`, lock-on seeds, hit ), Wind Wall ( 51, `EffectSet_SA_RSR_WindWard`, hits ), Crazy Shot ( 53, hits ), Arc Shot ( 55, 7 `DamageEffect_RGA_Arc_Shot` in one cast, hit ), Shooting Magnum ( 59, 4 arrows, hit ), Hunter's Steadiness ( 61, `EffectSe_A_RSR_Mind_Of_Shapshooter` arrow + hits ), Rapid Shot ( 63, bursts of `DamageEffect_RSR_Rapid_Shot` about every 0.18 s, hits ). ErrorLog: 0 KEM_ERROR / Motion Error / missing mesh or particle; packets 0 EXCEPTION. Not CH2c's: `ARROW.OGG` / `SONIC_ARROW.OGG` 3D-sound update `result : 47` ( 60 lines; any arrow ), `AddGroupChainQuest` unbound ( quest script ), the `APPLIED_SKILL_ID` stand-in lines. Not in any log ( the user's eyes ): Hunter's Steadiness's buff icon and cheaper next skill, cool times and MP bar.

### CH2d0 - Done (2026-11-10)

Opus 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP` (`Always.h:12099`; `#error` unless `_LIVE`, `_CORE`, `JUNE_CHAR_CHUNG_SKILL`, `JUNE_ENUM_NAMES`, `JUNE_USER_SKILL_LEVEL_HELPERS`, `JUNE_SKILL_TEMPLET`, `JUNE_DIVERGE_SKILL_LEVEL_VALUE`, all defined earlier) and a temporary `_TRAP_DEBUG` (`Always.h:12114`). Every block carries `// char-rena CH2d0`.

**Verdict.** June's trap rules sit in three places: the spawn calls (owner's client, not the host), `CX2Game::SetUserSummonedNPCInfo` `3230c1a0` (stats, power rate, the owner's trap list), and the NPC-die hook `3230b7b0` (list erase). **CH0's trap map was wrong on two points:** June keeps `NUI_EVOKE_TRAP_LIRE_MEMO` (1025) and `NUI_RENA_ANGER_OF_ELF_DELAY` (1421) (`32c99690` pushes `0x401`, `32c9e250` pushes `0x58d`; both are in June's `Enum.lua` and have `3230c1a0` cases), and Evoke's "swapped" team arguments are March's own global-server call (`eTeam TN_NONE`, `eAllyTeam` = team). `ch0_traps.tsv` corrected.

| June | Where | June differs from March |
|---|---|---|
| `32c99690` | `CreateEvoke` (inside) | `IsMyUnit()` gate (March `IsHost()`); memo trap 1025 when `IsEquippedSkillUpgraded( GetNowStateSkillID() )` (June inlines `31fca8e0( id, 1 )->+0x178`; March: memo 14); one call, level 1 while awakened (= March) |
| `32c9e250` | `CreateAngerOfElfTrap` (inside) | `IsMyUnit()` gate; **key code = `(int) GetLearnedSkillAbilityValue( SA_EFFECTIVE_TIME, GetNowStateSkillID() )`**: `AngerOfElfTrapRena.lua` / `...DelayRena.lua` end the trap when `GetKeyCode() <= GetTimerElapsedTime( 0 )`, so March's -1 ended it at once; delay id kept |
| `32c95790` | `CreateEntangleTrap` (inside) | `IsMyUnit()` gate only (no live June caller: Entangle is damage effects in June, CH3b) |
| `32c86340` | `AddEntangleTrap` (inside) | related skill level = `RNSI_A_RGA_ENTANGLE` (23213) at `GetSkillLevel( id, false, 0, true )` |
| `32c878f0` `32c87860` `32c863a0` `32c85900` | `InsertEvokeNpc`, `DeleteEvokeNPCUID`, `AddAngerOfElfTrap`, `DeleteAngerOfElfTrapUID` | = March (`32cb5f60` = the vector's `erase`) |
| `3230c1a0` 0x3fc / 0x401, 0x58c / 0x58d | `X2Game.cpp` cases (March in `#else`) | `SetSummonHelperInfo( ..., RNSI_A_RTR_EVOKE / _ANGER_OF_ELF, SHF_FIXED_HP, rate )`, rate = Hunter's Ability (23408) `SA_DAMAGE_REL` when learned (`31fceab0` / `32007b10`); then the owner's list; no `else SetNowHp( 0 )` |
| `3230c1a0` 0x1c9 | case 457 | helper flags 0 (`RNSI_A_RGA_ENTANGLE`), `AddEntangleTrap`, hold time from the owner (`+0x95d0` -> NPC `+0x8cd0`) |
| `3230c1a0` 0x823 | **new** case 2083 | helper flags 0 (`RNSI_SA_RGA_GUNGNIR`), owner `+0x97b8` = `m_iGungnirTargetNPCUID` via new `SetGungnirTargetNPCUID`: **nothing wrote it before**, so CH1f2's Gungnir aim states could never find their NPC |
| `322ece70` | `CX2GUUser::SetSummonHelperInfo` | gains June's rate argument (`fRate_ = 1.f`), multiplied after the socket term; `GetLearnedSkillAbilityValue` made public under the flag (June's `3230c1a0` calls it) |
| `3230b7b0` @3230bb7c-3230bc08 | NPC-die hook | = March (owner `UT_LIRE` -> erase by uid); no change |

Jump-table targets read from the exe (`0x823 -> 3230cbdf`, `0x3fc / 0x401 -> 3230c6c4`, `0x58c / 0x58d -> 3230c868`, `0x1c9 -> 3230c3ff`). The trailing `IsMyUnit` / `0` arguments of June's `322e67c0` are not in March's `CreateNPCReq` (as Chung CH3b0 / Rena CH1f2). Master link: the ally UID + `NAT_ALLY` (March's `SetUserSummonedNPCInfo` gate), Gungnir 2083 already unique per owner through `KillOlderSummonsOfSameID` (Eve CH2b2); 1020 / 1420 are not in that map (June caps them through `CreateEvoke`'s count and `MakeRoomForAngerOfElfTrap`).

**Trap NPC scripts.** `luabind.py check Lire_Evoke_trap Lire_Evoke_trap_Memo AngerOfElfTrapRena AngerOfElfTrapDelayRena EntangleTrapRena Rena_Targeting_Gungnir`: 11 / 11 / 11 / 11 / 5 / 17 methods, 0 unbound, `RESULT: PASS`. `adopt-june --dry-run`: all six `same` as June (data037). `USE_OWNERS_ENCHANT` is read (`X2GameUnit.cpp:4638`, slots 111 / 112 ported earlier). Emulator: `Handler_EGS_NPC_UNIT_CREATE_REQ` copies the whole req, key code included.

**Sites that stay with the skill phases.** CH2d: the Evoke states (`32ca6810` / hyper `32ca6860`, which calls `CreateEvoke` instead of March's six inline calls), Anger of Elf's FrameMove `32ca6fb0`, `EntireExplosion` (March `:21072`, reads the delay id; June keeps 1421, so CH2d checks June's body). CH3b: Entangle (damage effects) and the Gungnir equip case (the spawn `32ca11f2` is CH1f2's). CH3c: Night Watcher's Evoke / Anger of Elf hyper rows.

**Play-test: none.** Every trap is cast by a skill with no case yet (class-24 ids are CH2d's, Gungnir's CH3b's; saves use June ids), and the dev console's `CreateNPCReqAlly` passes owner -1, so `SetUserSummonedNPCInfo` never runs. CH2d's hand-off carries the checks; `_TRAP_DEBUG` stays on for it (CH2d's review retires it).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_TRAP --worktree --files` the six files: `4 PASS, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR` (`Always.h` = the two defines; `X2GULire_ElvenRanger.cpp` = `_TRAP_DEBUG` lines); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `_TRAP_DEBUG` (`Always.h`, Rena `.cpp`, `X2Game.cpp`): `RESULT: PASS` (`X2GUUser.cpp` makes that run stop on an unevaluable Chung `#if`, a tool limit; it has no `_TRAP_DEBUG` line).
- `junediverge.py --check` (5 code files): `allow-listed: 21`, `RESULT: PASS`. `enumscan.py --check` (3 `.cpp`): `0 A/B row(s) remain`.
- One-file `/Zs` probes (`census.cl_cmd`, no PCH): `X2GULire_ElvenRanger.cpp`, `X2Game.cpp`, `X2GUUser.cpp` 0 errors (the first run's two C2248 on the protected helper fixed); `/EP`: 4 `RENA TRAP` lines in the Rena `.cpp`.
- Full build skipped: no play-test.

### CH2d0 - Review (2026-11-10)

Opus 5.5. **Passed, no code change.** Commit `d98d771d` checked against June (`charport_adis.py rena 32c99690 32c9e250 32c95790 32c86340`; Ghidra read-only `3230c1a0`, `322ece70`; jump tables read from `unpacked_x2.exe`).

- **Evoke `32c99690`.** Count loop = June (`while size >= MAX_COUNT`: first trap `SetNowHp( 0 )` via vslot 0xf4, `erase( begin )`). Memo choice: `31fca8e0( vslot 0x524, 1 )->+0x178` = `m_iDeleteSkillIDByUpgrade` (`X2SkillTree.h:3285`), so `IsEquippedSkillUpgraded( GetNowStateSkillID() )` (`X2GUUser.cpp:7032`) is exact -> 0x401 / 0x3fc. Level: `fldz; fcomp [gage +0xe8]; test ah, 5; jp` = 1 only when the hyper time > 0 (= March's `REGULATE_EVOKE_HYPER_MODE` test). Pushes (both branches): arg 9 team `-1` = `TN_NONE`, arg 10 AI 1 = `NAT_ALLY`, arg 13 ally team = team, arg 15 = vslot 0x198 (`IsMyUnit`, not in March's `CreateNPCReq`).
- **Anger of Elf `32c9e250`.** Key = `_ftol2( 32007b10( 1, vslot 0x524 ) )`, `SA_EFFECTIVE_TIME` = 1 (`X2SkillTree.h:2656`), the cast = truncation. `+0x9700` -> 0x58d else 0x58c. Level = byte `+0x10` of the now-state pointer `+0x7cbc` = `m_bHyperState` = `IsHyperState()` (= March). `KNPCUnitReq::m_KeyCode` is a `char` (`CommonPacket.h:3816`): June's `SA_EFFECTIVE_TIME` is 7 (23216) and 10 (23239), well under 128.
- **Entangle `32c95790` / `32c86340`.** Level 0, key -1, team, `IsMyUnit` gate; `GetSkillLevel( 0x5aad = 23213, 0, 0, 1 )` -> NPC `+0x8e74`. = port.
- **`3230c1a0` cases.** Jump tables read from the exe: `0x1c9 -> 3230c3ff` (range base 0x180), `0x3fc / 0x401 -> 3230c6c4` (0x35f), `0x58c / 0x58d -> 3230c868` (0x58c), `0x823 -> 3230cbdf` (0x823). Flags 0 / 1 / 1 / 0. The rate = 1.0, or `32007b10( 0xf, 0x5b70 )` when `31fceab0( 0x5b70 ) > 0`; `SA_DAMAGE_REL` = 15, and `GetSkillLevelValue` under `JUNE_DIVERGE_SKILL_LEVEL_VALUE` is June's `31fceab0` (March's `SKILL_ID` overload is in `#else`, so no overload trap). Owner gate `31e4d0e0 == 3` = `UT_LIRE`; then `32c86340` + hold time `+0x95d0 -> +0x8cd0`, `32c878f0` (`InsertEvokeNpc`), `32c863a0` (`AddAngerOfElfTrap`), `+0x97b8 = npc +0x270` (`m_iGungnirTargetNPCUID`, `X2GULire_ElvenRanger.h:1912`, read at `.cpp:26446`-`26590`). The skill ids passed match June's `31fcf520` map (`summon_npc_skill_map.tsv`: 457 -> 23213, 1020 / 1025 -> 23218, 1420 / 1421 -> 23216, 2083 -> 23015).
- **`322ece70` rate.** funcmap row "@322ed2ae..322ed3ae": `(1 + socket) x rate x (3201ad10 + 1) x 31fd7cc0 x 31fea430 x GAA`; the port's `GetSummonPowerRateBySocket * fRate_ * GetSummonPowerRateByBuff * GAA` puts the rate in the same product.
- **Access change.** `GetLearnedSkillAbilityValue` is declared once under `_TRAP` (public) and once under `#ifndef _TRAP` (protected); Aisha's and Eve's copies are compiled out under `USER_SKILL_LEVEL_HELPERS`. Access alone changes no lookup, so no caller's meaning changes.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP --range d98d771d^..d98d771d --with ..._TRAP_DEBUG`: `files: 5 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `git log -S` after `d98d771d`: none. `junediverge.py --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan.py --check` (6 files): 2 rows, both pre-existing `X2GUUser.h:2555-2556` (`NUI_NEIL_THE_SNIPER_EVENT` / `NUI_PROTON_BOMB_EVENT`, blame `09107672`), not CH2d0's.

`_TRAP_DEBUG` stays defined: CH2d's play-test reads its lines, CH2d's review retires it. Full build skipped: no play-test (no code change, no probe needed). CH2d is ready; its kickoff's "Amended by CH2d0" section is accurate as written.

### CH2d - Done (2026-11-10)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (no new define; `_SKILL_DEBUG` and CH2d0's `_TRAP_DEBUG` stay on for the play-test). All 47 class-map rows read from `charport_adis.py rena --phase CH2d` listings (each listing runs on into the next function: read to the `int3`). **21 function bodies written inside March's** (June under the flag, March in `#else`), **26 are June = March** (no code). 7 cases (both ids each) added to the id-switch block; `X2SkillTree.h` gains 11 `RNSI_*` names (23019 / 23020 / 23021 / 23033 / 23036 / 23053 / 23214 / 23224 / 23238 / 23239 / 23241; 23215 / 23216 / 23218 existed). Every block carries `// char-rena CH2d`. `funcmap.tsv` rows `Char Rena CH2d` (48); `MODS.md` "Stage 7 Rena, CH2d" has the damage table.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6b6a0` `32c6b6e0` `32c6b700` `32c86d00` `32c86d30` `32c75e50` `32c75e90` | Humming Wind `Init`, `FrameMoveFuture`, `FrameMove`, `EventProcess` ( normal, hyper ) | June's 4-name preload; FMF = `CommonFrameMoveFuture` only ( March sets the dash speed / stops the fall ); FM = world colour, cut-in 0.06 / 0, `CommonFrameMove` ( particles and damage effects moved to the effect set ); EP = end -> `JUMP_DOWN` / `WAIT`, no skip-key cancel |
| `32ca6810` `32ca6860` | Evoke `FrameMove` ( normal, hyper ) | `CreateEvoke( 1 )` at 0.25 s ( CH2d0 ); no cut-in, no world colour |
| `32c97210` | Explosive Trap `FrameMove` | Hunter's Ability ( 23408 ) `SA_DAMAGE_REL` through `GetSkillLevel( id, 0, 0, 1 )`; no fake traps; Enhanced skill: `SA_EFFECTIVE_RATE` ( `32007c40`, swap ) `>= GetRandomFloat( 0 )` throws a second trap, `rotate.z += 15`; delay mode keeps the index / type 5 |
| `32c6c690` `32c7aae0` | Karma `StateStart`, `EventProcess` | no memo animation speed; end test + `SKILL_CANCEL_AFTER` + `RENA_GROUND_WALK / BWALK / DASH_CANCEL` |
| `32c98360` `32c987f0` `32c7b060` | Fatality `FrameMove` ( normal, hyper ), `SetFatalityData` | damage effect set always `_MEMO`; rating / power from 23019 at the swapped level and the swapped templet ( March read the old ids: level 0 = FAIL, power 0 ) |
| `32c7d160` | Eldrasil's Fury `EventProcess` | dash / walk cancels need the foot ( `RENA_GROUND_*` ), then `SKILL_CANCEL_AFTER` |
| `32c6cf50` `32c6cf60` `32c6cf70` `32c7d360` `32c9e440` | Briar Trap `StartFuture`, `FrameMoveFuture`, `EndFuture`, `EventProcess`, `FrameMove` | no gravity switches or 0.36 s timer; `FrameMove`: Hunter's Ability, `ROSEBUSH_TRAP_FLY_MEMO` ( + `_Upgrade` when Enhanced ), a 20 % second one at `pos.x += 100`, no memo test |
| = March | 26 functions | Init / Start / End / thunks of every state, Explosive Trap FMF / EP, Fatality Start / EP / End / futures ( `32ca68b0` keeps March's `m_fNowStatePowerRate *=` ), Eldrasil's Fury Init / Start / FM / End, Evoke Init / Start / End, Karma FM ( March's empty 1.0 s timer ): the funcmap rows say which |

**Registration** ( `32cb06d0`, rows `@32cb1328` Humming Wind, `@32cb2269` Fatality, `@32cb21d5` Karma, `@32cb2108` Explosive Trap, `@32cb10e7` Briar Trap, `@32cb107b` Eldrasil's Fury, `@32cb2076` Evoke, each with its hyper row ): Karma's hyper uses the normal FrameMove ( one June function ), Fatality's hyper FM is `_HYPER_FrameMove`, Evoke's EventProcess is `GenericActiveSkillEventProcess` ( June `31fcc9d0`, already shared ), Humming Wind's hyper has no Init. `m_bHyperState` is set centrally, so Eldrasil's Fury's March `m_bHyperState = true` line is not copied.

**Divergence row.** `SetFatalityData` ( `32c7b060`; the class map listed it new ) gets its `june_march_divergent.tsv` row, `replace`, ported in this commit.

**Checklist** `(Rena: CH2d ...)`: §1 l.116 / 117 ( none: damage effects run through the engine ), §4 l.178 ( 11 names ), §7 l.256 / l.440 ( only Fatality's Start multiplies the rate = March ), §7h l.427 ( no USEFUL_VALUE read ), §7u l.638 ( dropped March names ), §7ae l.729 ( 14 ids ). §7ai: no BFI call in the 47 functions ( the listings' callee census has none ). NF rows: none for CH2d ( `ch0_nf.tsv` has no class-24 row ). §7 l.423: Humming Wind's `EFFECT_SET_LIST_FINAL` rows come with the adopted script ( CH1 ); no class code.

**Answered from CH2d0.** `EntireExplosion` ( `32c9af70` ) = March ( CH1k's funcmap row: counts NPC 1421, cap 6 ). Trap Blade: `CreateTrapBlade` / `DeleteTrapBlade` = March ( CH1g ); its reader `32ca81b0` is CH1j's. Entangle is damage effects in June: CH3b.

**Found, not ported.** Nothing new. Fatality's always-`_MEMO` damage set is June's own ( its `EffectSet.lua` has the `_MEMO` sets ): kept, no JUNE_BUGS row ( a change, not a bug ).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --worktree --files` ( `X2GULire_ElvenRanger.cpp`, `X2SkillTree.h` ): `1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( both files ): `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. `skillicons.py check` ( the 16 class-24 ids ): `misses 0, known June gaps 0`, PASS. `luabind` not run: no new `.lua`.
- `/Zs` probe of the cpp ( `census.cl_cmd`, no PCH ): 0 errors; `/P` shows the new bodies live.
- `DEPLOY OK: X2_offline.exe is 26,157,568 B, dated 2026-11-10 07:49:56, md5 46081fb61e162d5a29307244fdeb871c, equal to X2/US_SERVICE/x2.exe ( 2026-11-10 07:49:49 ); built at main 921d7587 + 2 uncommitted code file(s).`
- `EXESTR OK: 5 of 5 found in X2_offline.exe, 26157568 B, md5 46081fb6` ( wide `VERITABLE_PANDEMONIUM_T02.dds`, `SCA_CRIMSON_BREAK_P01.Y`, `ROSEBUSH_TRAP_FLY_MEMO`, `_Upgrade`, `RENA SKILL equip skill` ).

**Play-test hand-off.** Save set up ( backup `db_backup/els_db_20261110_073856_pre_sql.sql` ): unit 91 `TrapRanger99` ( Rena, class 24, lv 99 ). Set 1 slotted ( bank A ): A Evoke Enhanced ( 23241 ), S Eldrasil's Fury Enhanced ( 23239 ), D Explosive Trap Enhanced ( 23224 ), C Briar Trap Enhanced ( 23238 ). Set 2 ( say "swap" ): bank B A Humming Wind ( 23036 ), S Fatality ( 23033 ), D Karma ( 23053 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice with enemies near: motion and effects, a cool time on the slot after the first cast, MP drops, a cast during the cool time is refused. **Evoke / Eldrasil's Fury:** the trap stays for its time and hits ( Eldrasil's Fury about 10 s ), it does not vanish at once. **Explosive Trap:** one fan of flying bombs, sometimes a second one turned 15 degrees. **Briar Trap:** thorn shots, sometimes a second one to the side. **Humming Wind:** the wind effect set around you, no dash. **Fatality:** a rated spark ( damage is not 0 ). **Karma:** the spark, walk / dash cancel on the ground. Then `python tools/logs.py --grep "RENA SKILL"` ( four `equip` lines in set 1, three in set 2 ) and `--grep "RENA TRAP"` ( per trap cast `spawn npc 1025` / `1420` or `1421`, `linked npc ... owner`, the `SUMMON` line, `removed ... uid` when it ends ) and say "check logs".

**Play-test logs (2026-11-10, "check logs").** Exe md5 `46081fb6`, run after the deploy, unit 91. 7 `RENA SKILL equip` lines ( bank A 23241 / 23239 / 23224 / 23238 states 49 / 51 / 53 / 55; bank B 23053 / 23036 / 23033 states 59 / 61 / 63 ). Effects seen: Evoke ( `EVOKE_*`, `EffectSet_LIRE_EVOKE_TRAP`, hit, then `_MEMO_DYING` ), Eldrasil's Fury ( `ANGER_OF_ELF_SPARK`, `Effectset_A_RNW_ANGER_OF_ELF_Damage` ), Explosive Trap ( 22 `EXPLOSION_TRAP_FLY` ), Briar Trap ( 13 `ROSEBUSH_TRAP_FLY_MEMO_Upgrade` ), Humming Wind ( `HummingWind_FX` / `_Damage` ), Fatality ( `EffectSet_FATALITY_SPARK_LUCKY` + `..._DAMAGE_MEMO`: rating and power non-zero ), Karma ( `KARMA_SPARK`, 73 ). **Traps:** per cast `RENA TRAP spawn npc 1025` ( Evoke, memo trap, skill 23241 ) / `1420` ( Eldrasil's Fury, `key 10` > 0 ), `linked ... owner 91`, `[JUNE_CHAR] SUMMON ... flags 0x1`. Both trap kinds detonate by proximity as June's scripts say ( `DISTANCE_TO_TARGET_NEAR = 80` ): Evoke died 0.7 s after its hit, Eldrasil's Fury hit at 0.3 s and ended 4.0 s after spawn ( its `DYING_READY` timer ); the 10 s wait for a trap nobody approaches was not exercised. **No `RENA TRAP removed` line:** `Handler_EGS_NPC_UNIT_DIE_NOT` ( `X2Game.cpp:9420` ) found no NPC by the time the die packet arrived ( the hook sits inside `if( pCX2GUNPC != NULL )`, = March and June's `3230b7b0` ); a stale uid is erased by `MakeRoomForAngerOfElfTrap` ( null NPC ) and Evoke's list holds weak pointers, so nothing breaks; for the review. ErrorLog: 0 Motion Error / missing mesh or particle for these skills ( the run's `OpenXET` / texture lines are the Ruben village and other classes' files: `RCR_REVOLVERFIRE_P1`, `ELSWORD_SA_RAGECUTTER_ATTACKSPHERE` ); packets 0 EXCEPTION; no `NUM use` lines ( the numerical rows print only on a miss ). Not in any log ( the user's eyes ): cool times, the MP bar, Karma / Eldrasil's Fury cancels.

### CH2d - Review (2026-11-10, Opus 5.5): PASS, covers CH2c + CH2d; `_SKILL_DEBUG` and `_TRAP_DEBUG` retired, no other code change

Commits `d39764ba` (CH2c) and `5824de6f` (CH2d) checked against June (`charport_adis.py rena --phase CH2c / CH2d` and `32cb06d0`; table names and the `32cb3xxx`-`32cb6xxx` callback thunks decoded from `unpacked_x2.exe`).

**CH2d.**
- **Registration.** All 7 class-24 cases = June: table names (`LESI_SI1_LE_HUMMING_WIND` / `_HYPER`, `LESI_SA_RTR_FATALITY`, `LESI_SA_RTR_KARMA`, `LESI_A_RTR_EXPLOSION_TRAP`, `LESI_A_RNW_ROSEBUSH_TRAP`, `LESI_A_RNW_ANGER_OF_ELF`, `LESI_SA_RTR_EVOKE`), each normal slot's thunk target and each hyper copy (Humming Wind's hyper: no Init; Karma's hyper: Start / FM / EP / End copied, one FM; Fatality's hyper FM `32c987f0`; Anger of Elf's shared copy block `32cb0f69` includes Init; Evoke's EP `329cba60` -> `31fcc9d0`).
- **Explosive Trap `32c97210`.** `GetSkillLevel( 0x5b70, 0, 0, 1 )`, templet by id, `SA_DAMAGE_REL` = 15; `y += 200`; second trap: `31fca8e0( GetNowStateSkillID(), 1 )->+0x178` then `32007c40( 5 = SA_EFFECTIVE_RATE, id, 1, &f )` and `f >= GetRandomFloat( 0 )`; delay mode = `CreateDamageEffectIndex()` (`+0x9718` wrap at 100, unit index x 100) and type 5 = `DET_EXPLOSION_TRAP`; `z += 15` after each. = port.
- **Briar Trap `32c9e440`** = port (`_MEMO` + `_Upgrade` on `+0x178`; second shot when `GetRandomFloat( 0 ) <= 0.2`, `x += 100`). Thunks `32c6cf50 / 60 / 70` = the common calls only; EP `32c7d360` = end on the ground -> `JUMP_LANDING`, no timer.
- **Fatality.** `SetFatalityData` `32c7b060`: rating 3 / power 0 first; level of 0x59eb; templet of `31fcf430` (= `GetUpgradeSwappedSkillID`, funcmap row 7019); abilities 0x77-0x7a; `sum > random` picks the rank; power 0x73 + rank. FM `32c98360`: the push_back takes `[esp+0x44]` = the first `PlayEffectSet`'s return slot (stack walked: first call's out-arg = base+0x44, second's = base+0x48) = the damage set, as ported; `32c987f0` differs only in the SEH handler and jump-table bytes.
- **Cancels.** Karma EP `32c7aae0`: end, then 0x8c90 / 0x8c98 / 0x8c94 / 0x8c9c = `SKILL_CANCEL_AFTER`, `RENA_GROUND_WALK`, `_BWALK`, `_DASH` (the macros are `else if` chains, as June's jumps); Start `32c6c690` = `CommonStateStart`. Eldrasil's Fury EP `32c7d160`: dash, walk, skill = port.
- **Humming Wind** (`32c6b6a0` 4 names, FMF thunk, FM 0.06 / 0, EP) and **Evoke FM** (`CreateEvoke( 1 )` at 0.25 s): = port; each hyper twin is byte-identical to its normal function.
- **"= March" spot-checks.** `32c971b0` (CommonStateEnd + `DeleteTrapBlade` inline: `+0x220` clear, `m_hTrapBlade`, `m_bUseTrapBlade`), `32ca68b0` (CreateTrapBlade, SetFatalityData, `m_fNowStatePowerRate *=`), `32ca6fb0` (CreateAngerOfElfTrap at 0.25 s): = March. `32c87090` (Karma FM) has no 1.0 s timer where March keeps an empty one: same behaviour, left.

**CH2c** (the `CH2c-review.md` list).
- **Registration.** The 7 class-13 rows = port: Guided Arrow (hyper `_HYPER` FMF / FM / Camera / EP), Wind Wall (hyper copy from `+0x24`: no Init), Crazy Shot (hyper FM / EP, no Init), Shooting Magnum and Arc Shot (hyper copies Init + FM / EP), Hunter's Steadiness (`329cc2c0` `CopyStateCallbacks`), Rapid Shot (ready 6 callbacks, hyper copies; attack / finish ids 0xf0 / 0xf1 carry FMF `32c88050`, FM `32c9d250` / `32c9d420`, EP `32c7c990` / `32c7cbe0`).
- **Bodies.** Hunter's Steadiness `32c83220` (CommonFrameMove, then at 0.71 s vslot 0x228 `SetBuffFactorToGameUnit( templet of 0x5aaa, 0 )`), Wind Wall `32c953d0` (`max( 1, level )`, ability 0x20 x3, ability 1, gravity back on at 1.0 s), Arc Shot `32c9cc10` (+30 then -15 x4; on `+0x178` -15 and an absolute 45), Rapid Shot FMF / helper / attack / finish (`0x5b77`; Future-slot write; `+0x8178` Now float x elapsed, clamp ±40, `Bip01_Spine1`; finish 0.08 s / ±3 / 0.054 s) = port. June's FMF skips the write when the value is unchanged; the port writes the same value: same result.
- **Preload lists.** The six June-side lists = port name for name and in order.

**Play-test (CH2d run).** 7 `RENA SKILL equip` lines, 4 `RENA TRAP spawn` + 4 `linked` + 4 `SUMMON` (1025 x2, 1420 x2); 0 Motion Error / KEM_ERROR; packets 0 EXCEPTION. The open CH2d0 question is answered: no trap vanishes at once (proximity detonation per June's scripts). No `removed` line: the die hook = June `3230b7b0` and sits inside `pCX2GUNPC != NULL`; nothing breaks (weak pointers / null-NPC erase), no change. The run's `OpenXET` lines on Rena skill meshes (`RENA_A_EVOKE_DUMMY.XET`, `A_RNW_THORNS_TRAP.XET`, ...) are not missing assets: `CKTDXDeviceXMesh::XMeshProxy::LoadXET` (`KTDXDeviceXMesh.cpp:1382`) tries an optional same-name `.XET` beside every mesh.

**Hygiene.** `flagcheck JUNE_CHAR_RENA_SKILL --with _SKILL_DEBUG --range d39764ba^..d39764ba`: `2 PASS, 1 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL`, 1 deleted ( the re-define ), `RESULT: PASS`; `--range 5824de6f^..5824de6f`: `1 PASS, 1 OTHER, 0 FAIL`, 0 deleted, `RESULT: PASS` (the joined range `d39764ba^..5824de6f` FAILs only on CH2d0's `_TRAP` hunk in `X2GUUser.cpp`, another flag's). `flagcheck JUNE_CHAR_RENA_TRAP --with _TRAP_DEBUG --range d98d771d^..d98d771d`: `5 PASS, 1 DEFINE, 0 FAIL`, `RESULT: PASS`. `junediverge --check-all`: `allow-listed: 202`, `RESULT: PASS`. `enumscan --check X2Lib/X2GULire_ElvenRanger.cpp`: `0 A/B row(s) remain`. `/Zs` probe of the cpp (`census.cl_cmd`): 0 errors.

**Change.** `KTDXLIB/Always.h:12071` `//#define SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` and `:12114` `//#define SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG` (bytepatch; CP949 / CRLF kept, 2 + / 2 -). `flagcheck JUNE_CHAR_RENA_SKILL --with` both `--worktree --files KTDXLIB/Always.h`: `0 FAIL`, `RESULT: PASS`; `junediverge --check KTDXLIB/Always.h`: `RESULT: PASS`. Full build started, not awaited (debug-retire rule). CH3a re-defines `_SKILL_DEBUG` for its play-test (`CH3a.md`, Amended by CH2d review); CH4a is ready too, one at a time with CH3a (same file).

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

### CH3a - Done (2026-11-10)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (no new define; `_SKILL_DEBUG` is defined again for the play-test and stays on until the CH3b review retires it). All 44 class-map rows read from `charport_adis.py rena --phase CH3a` listings (each listing runs on into the next function: read to the `int3`). **21 function bodies written** (12 inside March's under the flag, March's in `#else`; 9 new: Sharpfall's start and landing start, the Init of the four new states, the EventProcess of three of them), **23 are June = March** (no code). 10 cases ( 16 ids ) added to the id-switch block; `X2SkillTree.h` gains 11 `RNSI_*` names (23010 / 23037 / 23038 / 23044-23047 / 23058 / 23225 / 23230 / 23232; 23008 / 23009 / 23011 / 23205 / 23207 existed). Every block carries `// char-rena CH3a`. `funcmap.tsv` rows `Char Rena CH3a` (45); `MODS.md` "Stage 7 Rena, CH3a" has the damage table; `JUNE_BUGS.md` 577-579.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6bca0` `32c6bd00` `32c6bfb0` `32c6c150` `32c6c2e0` `32c6c410` | Reflex Magic / Sharpfall / Violent Attack ready / Slide Kick / Nature Force / Airelinna `Init` | June's preload lists ( 5 / 18 / 13 / 11 / 8 / 5 names; Violent Attack's two later Inits = March ) |
| `32c6bdf0` `32c87ec0` | `LESI_SA_LCR_SHARPFALL_StateStart`, `..._LANDING_StateStart` ( new ) | Start height kept in `m_fSharpfallStartPosY` ( +0x9804 ); the Enhanced skill scales the cached state rate by 1 + `SA_DAMAGE_REL` x min( fall, `SA_SCOPE_ABS` ) / `SA_SCOPE_ABS` ( both landing states ) |
| `32c76d50` | Sharpfall hyper `EventProcess` | lands in `LESI_SA_LCR_SHARPFALL_HYPER_LANDING` ( 202 ), registered with the normal landing's callbacks |
| `32c769f0` | Reflex Magic `EventProcess` | skill, walk, back-walk, dash cancels need the foot ( `RENA_GROUND_*` ); March's `ADD_RENA_SYSTEM` ones do not |
| `32c96910` | Slide Kick `EventProcess` | only a living hit unit ( HP > 0, not `GUSI_DIE` ) passes through; the end refunds `GetActualMPConsume` x `SA_MP_RETAKE_ABS` ( March: any hit, 50 % of the slot's MP ) |
| `32c6c1e0` | Slide Kick `StateEnd` | keeps the effect handle ( JUNE_BUGS 577 ) |
| `32c6c350` | Nature Force `StateStart` | templet of 23205 with the upgrade swap; the hyper form shares it |
| `32c96aa0` | Airelinna `FrameMove` | no effect set at 0.1 s; at 0.7333 s the bolt of the swapped templet; Enhanced: `SetScale( SA_PROJECTILE_SIZE )`, no life time ( JUNE_BUGS 578 ) |
| `32c6d020` `32c6d030` `32c6d080` `32c6d880` | Spinning Crescent / Gyro Kick / Maximum Pain / Tempest `Init` ( new ) | empty |
| `32c7d790` `32c7daa0` `32c7e9f0` | Spinning Crescent / Gyro Kick / Maximum Pain `EventProcess` ( new ) | the three are the same instructions: end -> `JUMP_DOWN` / `WAIT`, `SKILL_CANCEL_AFTER`, `RENA_GROUND_BWALK / WALK / DASH / ATTACK_CANCEL` ( pure Z / X at +0x8c8c ) |
| = March | 23 functions | the rest of the Violent Attack, Sharpfall, Slide Kick states ( FrameMove, cancel, pass-through, landing ), Reflex Magic ready: the funcmap rows say which |

**Registration** ( `32cb06d0`, rows in the `32cb15ca`-`32cb28f9` range, the hyper row of a single-state skill is the shared `@32cb0c01` = `CopyStateCallbacks` ): Violent Attack registers only the states 203 / 204 ( no hyper-named states, so March's `_HYPER` ones stay unused under June ); Slide Kick's finish state keeps the generic special-skill event process ( June lists no callback ); Tempest's EventProcess is the shared `GenericActiveSkillEventProcess`. The landing states take their skill id and level from CH1i's `SetEquippedSkillLevel` ( checked: Sharpfall 201 / 202, Violent Attack 203 / 204, Slide Kick finish ). Upgrade swap: `GetEquippedActiveSkillTemplet( id, true )` / `IsEquippedSkillUpgraded( GetNowStateSkillID() )` / `GetFutureStateSkillID()` read the upgrade through its base; no other term reads an `SI_*` id.

**Checklist** `(Rena: CH3a ...)`: §1 l.56 ( 17 tables through the engine ), l.116 / 117, §4 l.178 ( 11 names ), §7 l.256 / l.440 ( only `32c87ec0` multiplies the cached rate ), §7h l.427 ( no USEFUL_VALUE read ), §7u l.638 ( Airelinna's `EffectSet_AIRELINNA` dropped ), §7ae l.729 ( 16 ids ). §7ai: no BFI call in the 44 functions ( `SetBuffFactorToGameUnit( templet, index )` takes the templet's factor list ). NF row ( `ch0_nf.tsv`, Reflex Magic's `ADD_RENA_SYSTEM` cancels ): replaced by June's `RENA_GROUND_*` set.

**Found, not ported.** Nothing new. `item_Summon_Dummy_Mesh01.X` ( Airelinna's Init ) is in no kom: JUNE_BUGS 579, kept.

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --worktree --files` ( `X2GULire_ElvenRanger.cpp`, `.h`, `X2SkillTree.h`, `Always.h` ): `2 PASS, 0 PASS~, 1 DEFINE ( Always.h: the re-defined debug line ), 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `1 deleted line ( the //#define turned into #define ), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( both files + `X2SkillTree.h` ): `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp / .h`: `0 A/B row(s) remain`. `skillicons.py check` ( the 16 ids ): `skills 16, misses 0, known June gaps 0`, PASS. `luabind.py check Lire_ElvenRanger`: 0 unbound.
- `/Zs` probe of the cpp ( `census.cl_cmd`, no PCH ): 0 errors.
- `DEPLOY OK: X2_offline.exe is 26,167,296 B, dated 2026-11-10 09:16:52, md5 2f3b9635c128569caa03e7047f8107ea, equal to X2/US_SERVICE/x2.exe (2026-11-10 09:16:45); built at main 23b9b6a6 + 4 uncommitted code files.`
- `EXESTR OK: 5 of 5 found in X2_offline.exe, 26167296 B, md5 2f3b9635` ( wide `RENA SKILL slide kick mp refund`, `RENA SKILL sharpfall landing skill`, `RENA SKILL equip skill`, `AIRELINNA_ATTACK_TWO`, `ViolentAttackM1_02.X` ).

**Play-test hand-off.** Save set up ( backup `db_backup/els_db_20261110_090934_pre_sql.sql` ): unit 89 `WindSneakerT` ( Rena, class 102, lv 99 ). Set 1 slotted ( upgrade ids; the bases are level 0 ): bank A A Reflex Magic Enhanced ( 23232 ), S Nature Force Enhanced ( 23230 ), D Maximum Pain ( 23225 ); bank B Q Sharpfall Enhanced ( 23045 ), W Violent Attack Enhanced ( 23044 ), E Slide Kick Enhanced ( 23047 ), R Airelinna Enhanced ( 23046 ). Set 2 ( say "swap" ): bases at level 1, upgrades 0: A Reflex Magic ( 23207 ), S Nature Force ( 23205 ); B Q Sharpfall ( 23009 ), W Violent Attack ( 23008 ), E Slide Kick ( 23011 ), R Airelinna ( 23010 ). Set 3 ( say "swap" again ): B Q Spinning Crescent ( 23037 ), W Gyro Kick ( 23038 ), E Tempest ( 23058 ). Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice with enemies near: motion and effects, a cool time on the slot after the first cast, the MP bar drops, a cast during the cool time is refused. **Sharpfall:** cast it from a jump ( the farther it falls the harder an Enhanced one hits ) and it lands in its landing state; **Violent Attack:** the ready, hop and landing states play in order; **Slide Kick:** slides, passes through a hit enemy, finishes with a kick; a miss ends in the cancel state and refunds MP ( "MP+ n" text ); **Reflex Magic:** the kick then a walk / dash cancel on the ground only; **Nature Force:** a buff icon at the start; **Airelinna:** the bolt flies at the end of the cast ( a bigger one when Enhanced ); **Spinning Crescent / Gyro Kick / Tempest / Maximum Pain:** the state plays and ends in WAIT ( air: fall ). Then `python tools/logs.py --grep "RENA SKILL"` ( `equip skill` lines per id, `sharpfall landing skill` and `slide kick mp refund` ) and say "check logs".

**Play-test (2026-11-10).** Logs checked for set 1 ( upgrade ids, unit 89 ): `equip skill` x7, Sharpfall Enhanced `fall 317 / 0 / 289` -> multiple x1.33 / 1.00 / 1.30, Slide Kick pass-through effect sets and a miss with `mp refund 75`, Violent Attack ready / attack / landing sets, Reflex Magic, Nature Force, Airelinna ( `AIRELINNA_ATTACK_TWO` ) and Maximum Pain effect sets; 0 Motion Error. The set 2 / 3 runs are not in the logs ( the save was overwritten once by a game still open: close the game before a `swap` ). **The user reports all the skills passed**, base ids and sets 2 / 3 included; cool time, MP bar and cancels are by the user's eye.

### CH3b - Done (2026-11-10)

Sonnet 5.5. Flag `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (no new define; `_SKILL_DEBUG` stays on until the CH3b review, which also covers CH3a, retires it). All 39 class-map rows read from `charport_adis.py rena --phase CH3b` listings (each listing runs on into the next function: read to the `int3`). **29 function bodies written** (10 inside March's under the flag, March's in `#else`; 19 new: the Stigma Arrow hyper FrameMove, the Init and EventProcess of Blast Arrow / Cutlass Wind / One Target Shot, and Ace in the Hole's 12), **10 are June = March** (no code). 10 cases ( 16 ids ) added to the id-switch block; `X2SkillTree.h` gains 13 `RNSI_*` names ( 23016-23018, 23039, 23040, 23049-23052, 23211, 23226, 23235, 23237 ); `X2GULire_ElvenRanger.h` two stand-in / state members. Every block carries `// char-rena CH3b`. `funcmap.tsv` rows `Char Rena CH3b` ( 40 ); `MODS.md` "Stage 7 Rena, CH3b" has the damage table; `JUNE_BUGS.md` 580.

| June | `CX2GULire_ElvenRanger::` | June differs from March |
|---|---|---|
| `32c6c1f0` `32c6c3a0` `32c6c460` `32c6cba0` | Freezing Arrow / Stigma Arrow / Aero Strafe / Wind Blast `Init` | June's preload lists ( 19 / 8 / 8 / 10 names ) |
| `32c9d7b0` | Wind Blast `FrameMove` | one `DamageEffect_SA_RSR_WindBlast_Arrow` at 1.0 s ( after `CommonFrameMove` ); its lock-on is the nearest target within 0 - 1200 ( `322d7670`: unit uid, else NPC uid; random seed = `GetRandomInt` ) ( JUNE_BUGS 580 ) |
| `32cacda0` `32cacff0` | Stigma Arrow `FrameMove`, hyper ( new ) | `DamageEffect_A_RGA_Stigma_Arrow` ( `_Hyper` ); the swapped 23211 templet; the first buff factor goes through `PushBuffFactorWithMemoRate` |
| `32c87060` `32c6c530` | Aero Strafe `FrameMove`, hyper | no `AERO_STRAFE` damage effect ( the skill's effect sets carry it ); the hyper form has no cut-in light |
| `32c9e7b0` `32c7d5a0` | Entangle `FrameMove`, `EventProcess` | Hunter's Ability ( 23408 ) applies with no PvP test; the dash / walk / skill cancels need the foot on a line |
| `32c7ddb0` `32c7e0c0` `32c7ed00` | Blast Arrow / Cutlass Wind / One Target Shot `EventProcess` ( new ) | the three are `32c7d790`'s instructions ( CH3a ): end -> `JUMP_DOWN` / `WAIT`, `SKILL_CANCEL_AFTER`, `RENA_GROUND_BWALK / WALK / DASH / ATTACK_CANCEL` |
| `32c6d8a0` `32c6d8f0` | Ace in the Hole start: `StateStart`, `StateStartFuture` ( new ) | `m_fAceInTheHoleAimAngle` ( +0x9808, new member ) = 0; my unit sets the three camera stand-ins ( 550.0 / 220.0, reader = Stage 9 ); the future start zeroes `m_fAceInTheHoleTime` ( only the start state registers it ) |
| `32c833b0` `32c833e0` `32c83410` `32c83440` | Ace in the Hole start / 1st / 2nd / 3rd attack `EventProcess` ( new ) | animation end -> wait / wait-after-1st / wait-after-2nd; the 3rd -> `JUMP_DOWN` / `WAIT` |
| `32c85430` `32c85500` `32c855d0` | Ace in the Hole three wait `EventProcess` ( new ) | the skill key ( `IsEquippedSkillKeyPressed` ) shoots the next arrow; left / right, or `m_fAceInTheHoleTime` >= `SA_EFFECTIVE_TIME`, gives back `SA_MP_RETAKE_ABS` x 3 / 2 / 1 ( `UpNowMpWithText` ) and ends in `USI_WAIT` |
| `32c885a0` `32ca2520` | Ace in the Hole `FrameMoveFuture`, `FrameMove` ( new ) | class-skill float = +-`SA_DEGREE_Z` on up / down; the angle integrates it, clamped to `SA_DEGREE_MAX`, `SetRotateBone( Bip01_Spine1 )`; at 0.2 s the state's effect set ( Fire1 / Fire2 / Final ), local rotation ( 0, 0, angle ) |
| = March | 10 functions | Freezing Arrow's two FrameMoves, Aero Strafe's CameraMove x2 / EndFuture / FrameMoveFuture x2 / EventProcess x2, Entangle's Init: the funcmap rows say which |

**Registration** ( `32cb06d0` ): Gungnir `@32cb1877` / `@32cb18c1` + the 11 states `@32cb193b`-`@32cb1b92` ( functions are CH1f2's; before this nothing registered them ), Wind Blast `@32cb0eb4`, Aero Strafe `@32cb1f90` ( the hyper form takes the normal EndFuture ), Freezing Arrow `@32cb1e5c`, Stigma Arrow `@32cb1f29` / `@32cb1f5d`, Entangle `@32cb2413`, Blast Arrow `@32cb276d`, Cutlass Wind `@32cb27a2`, One Target Shot `@32cb2876`, Ace in the Hole `@32cb292a` + six states `@32cb29c8`-`@32cb2b19` ( 298 and 300 register the table of 296 ). A single-state skill's hyper row is the shared `@32cb0c01` = `CopyStateCallbacks`. Upgrade swap: the Stigma FrameMoves read the templet with `GetEquippedActiveSkillTemplet( 23211, true )`; Ace's terms read `GetFutureStateSkillID()` / `GetNowStateSkillID()`; Hunter's Ability is read by its own id.

**Checklist** `(Rena: CH3b ...)`: §1 l.56 ( 25 effects ), l.116 / 117, §4 l.178 ( 13 names ), §7 l.256 / l.440 ( none of the 39 touches the state rate ), §7h l.427, §7n l.493 ( Ace's float ), §7u l.638 ( `DamageEffect_LGA_Arc_Shot` was CH2c's; Aero Strafe's `AERO_STRAFE`, Stigma's `RENA_STIGMA_ARROW` are the `#else` ), §7ae l.729 ( 16 ids ). §7ai: no BFI call in the 39 functions ( the Stigma buff factor is pushed into the damage effect's data, not into a unit ). NF: `ch0_nf.tsv` has no CH3b row ( Ace's NF cost is the skill's data ).

**Found, not ported.** Nothing new. The kickoff's "Anger of Elf `:22713` if CH0 puts it here": CH0 put it in CH2d ( done ). Entangle's trap function stays as CH2d0 left it ( no live caller ).

**Gates.**
- `flagcheck JUNE_CHAR_RENA_SKILL --with ..._SKILL_DEBUG --worktree --files` ( `X2GULire_ElvenRanger.cpp`, `.h`, `X2SkillTree.h` ): `2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h = JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` ( both files + `X2SkillTree.h` ): `allow-listed: 5`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check` ( the 16 ids ): `skills 16, misses 0, known June gaps 0`, PASS. `luabind.py check Lire_ElvenRanger`: 0 unbound.
- `/Zs` probe of the cpp ( `census.cl_cmd`, no PCH ): 0 errors ( 38 warnings, all in existing headers ).
- `DEPLOY OK: X2_offline.exe is 26,184,704 B, dated 2026-11-10 10:09:06, md5 d4cbd8acc23c040cfc064cddd5fbf82f, equal to X2/US_SERVICE/x2.exe (2026-11-10 10:08:59); built at main 361d04e8 + 3 uncommitted code files.`
- `EXESTR OK: 8 of 8 found in X2_offline.exe, 26184704 B, md5 d4cbd8ac` ( wide `RENA SKILL wind blast lock on`, `ace in the hole fire`, `ace in the hole mp refund`, `stigma arrow buff factor`, `entangle power rate`, `EffectSet_SI_SA_RGA_AceInTheHole_Fire1`, `DamageEffect_SA_RSR_WindBlast_Arrow`, `DamageEffect_A_RGA_Stigma_Arrow_Hyper` ).

**Play-test hand-off.** Save set up ( backups `db_backup/els_db_20261110_095829_pre_ch3b_pre_addchar.sql`, `..._095901_pre_sql.sql` ): new unit 92 `GrandArcherT` ( Rena, class 103, lv 99 ). Set 1 slotted ( upgrade ids; the bases are level 0 ): bank A A Stigma Arrow Enhanced ( 23235 ), S Entangle Enhanced ( 23237 ), D One Target Shot ( 23226 ); bank B Q Wind Blast Enhanced ( 23050 ), W Aero Strafe Enhanced ( 23051 ), E Freezing Arrow Enhanced ( 23052 ), R Gungnir Enhanced ( 23049 ). Set 2 ( say "swap" ): bases at level 1, upgrades 0: A Stigma Arrow ( 23211 ), S Entangle ( 23213 ), D One Target Shot; B Q Wind Blast ( 23016 ), W Aero Strafe ( 23017 ), E Freezing Arrow ( 23018 ), R Gungnir ( 23015 ). Set 3 ( say "swap" again ): B Q Blast Arrow ( 23039 ), W Cutlass Wind ( 23040 ), E Ace in the Hole ( 23059 ). Close the game before a `swap`. Dev console ( END key ): `/invin 0`, then `/stage 1`. Each skill twice with enemies near: motion and effects, a cool time on the slot after the first cast, the MP bar drops, a cast during the cool time is refused. **Stigma Arrow:** an arrow that marks the hit enemy ( log `stigma arrow buff factor` ); **Entangle:** the seed is thrown after its wind-up and bursts ( log `entangle power rate`, higher with Hunter's Ability learned ); **One Target Shot / Cutlass Wind / Blast Arrow:** one shot, then walk / dash / Z / X cancels on the ground only ( Blast Arrow: a small black hole at the arrow ); **Wind Blast:** after the wind-up one arrow flies to the nearest enemy ( log `wind blast lock on npc uid N` ); **Aero Strafe:** the back-flip with the shots ( no cut-in light in hyper ); **Freezing Arrow:** an ice arrow and its burst; **Gungnir:** hold the skill key: an aim marker appears ( `python tools/logs.py --grep "SUMMON npc 2083"` ), the arrow keys move it, releasing the key fires; **Ace in the Hole:** the start, then the same skill key shoots each of three arrows ( last one hardest ), up / down tilts the aim ( log `ace in the hole fire ... angle N` x3 ), and stopping early ( left / right, or wait out its time ) shows "MP+ n" ( log `mp refund n, k shot(s) left` ). Then `python tools/logs.py --grep "RENA SKILL"` and say "check logs".

**Play-test (2026-11-10, unit 92 set 1; logs read).** The user reports eight of the ten skills pass ( Stigma Arrow, Entangle, One Target Shot, Aero Strafe, Freezing Arrow and the rest of set 1 ). **Wind Blast:** the arrow chases but never explodes; the log has `wind blast lock on npc uid 1000011` and no `CE_HIT` for the arrow ( the other skills log theirs ). Lead: `X2DamageManager.cpp:2920` ( `m_bIsOnlyTargetAttack` ) casts every defender, NPCs included, to `CX2GUUser` and needs its unit uid to equal the lock-on unit uid, so an NPC lock ( NPC uid ) is never hit; June's template has `ONLY_TARGET_ATTACK = TRUE`. **Gungnir:** stays in its first animation; no `Gungnir_Loop` effect set, so `LESI_RGA_GUNGNIR_CONTROL_IDLE` was never entered; `READY_EventProcess` waits for the aim NPC link ( CH2d0's case in `SetUserSummonedNPCInfo`, whose debug line is retired ), cause unknown. Both go to **CH3b2** ( `kickoffs/char-rena/CH3b2.md`, Opus: shared engine file plus a diagnosis build ).

### CH3b2 - Done (2026-11-10)

Opus 5.5 high. Fixes the two CH3b play-test failures. Kickoff `kickoffs/char-rena/CH3b2.md`. Own review (`kickoffs/char-rena/CH3b2-review.md`): engine file and core-list Lua keep their own review under the 2026-11-10 cadence rule; the CH3 skill review at CH3d (`CH3d-review.md`, covers CH3a-CH3d; the CH3b review was folded into it, user 2026-11-10) only reads this phase's play-test result. (The kickoff's "folds into the CH3b review" predated that rule.)

**Wind Blast never hit: engine, new flag `SERV_IRUHADEV_JUNE_ONLY_TARGET_ATTACK_NPC`** (`Always.h`, end).
- June `3262b6e0` @3262ba99-3262bb4f (`EffectToUnit`, right after the `ONLY_RELATIONSHIP` test, same place as March's): when `LockOnData.m_bIsOnlyTargetAttack` (+0x9a0) is set it switches on the defender's type (+0x264): a user is skipped unless `GetUnitUID()` (vslot 0x54) = `m_LockOnUnitUID` (+0x938/+0x93c); an NPC is skipped unless `GetUID()` (+0x270) = `m_LockOnNPCUID` (+0x940); any other type is skipped. March `static_cast`ed every defender to `CX2GUUser` and compared the unit UID only, so a locked-on NPC never passed.
- June's NPC branch then (@3262baca-3262bb22), in a boss-raid field (`32481fa0`), retargets to the NPC its follow-target UID names (`32757490( 0 )` = March's `FIELD_BOSS_RAID` `m_sFollowTargetUnitData`, layout matches) when that NPC is a boss (vslot 0x3c8). Stand-in, one-line comment: `32481fa0` is false in this build (the existing precedent in `X2GULire_ElvenRanger.cpp` / `X2GUArme_VioletMage.cpp`).
- `X2DamageManager.cpp` `EffectToUnit`, inside `LOCK_ON_USER_ONLY_ON`, March's block in `#else`. The only reader of `m_bIsOnlyTargetAttack` (grep).
- The lock-on writes were already June's: CH3b's Wind Blast block calls `SetLockOnNPCUID` and `CX2DamageEffect::SetLockOnTargetUID` writes only the UID field (no `m_bLockOnUserUnit`), as June `32c9d7b0` @32c9d9ac-@32c9d9e1 does. No change there.

**Gungnir stayed in its ready state: data, not code.**
- The CH3b logs show all three casts spawned the targeting NPC on the emulator (`spawning 1 NPC(s)` 1000026 / 27 / 28) but never a `[JUNE_CHAR] SUMMON npc 2083` line (unconditional in `SetSummonHelperInfo`), and `OpenScriptFile.log` never opened its script: the client never built the NPC. `NUI_RENA_TARGETING_GUNGNIR` (2083) existed in C++ (CH2d0) but in neither `data036 Enum.lua` nor `data037 NPCTemplet.lua`, so `GetNPCUnitInfo( 2083 )` is NULL and both create paths refuse it. `LESI_RGA_GUNGNIR_READY_EventProcess` = June `32ca1240` (re-read): with no NPC it waits for the 10 s timeout, which is what the user saw.
- Added (core-list BOM source, ledger refreshed): `NUI_RENA_TARGETING_GUNGNIR = 2083,` in `Enum.lua`'s `NPC_UNIT_ID` beside the other port-added names; June's `AddNPCTemplet` row verbatim (`june_src_fixed/data037_kom/NPCTemplet.src.lua:18660`; script `Rena_Targeting_Gungnir.lua`, already June's bytecode in data037; mesh `CDC_AS_ROCKON_DUMMY.X`, March data117, the same mesh and `Normal` anim as Chung's working targeting NPC) at the end of `NPCTemplet.lua` in a `char-rena CH3b2` block.
- `luabind.py check Rena_Targeting_Gungnir`: 17 methods, 0 unbound, PASS. `luasrc.py ledger --check` PASS, `core --check` PASS.
- `PACK OK data036`, `PACK OK data037`; `COPY OK data036` md5 d42346f0 (backup `kom_backup_tools\data036.kom.20261110_110459`), `COPY OK data037` md5 5eea4f8e (backup `..._110500`).
- Found, not ported (March engine, not June): in a battle field an NPC whose templet is missing never finishes `AddNPCUnitUIDResource`, and the field's in-order create queue (`CX2BattleFieldGame::OnFrameMove`, front only) then stalls every later field NPC. Moot once every summoned id has a row; noted only.

**Hygiene.**
- `flagcheck.py SERV_IRUHADEV_JUNE_ONLY_TARGET_ATTACK_NPC --worktree`: 1 PASS (`X2DamageManager.cpp` +19), 1 DEFINE, 0 deleted, 0 FAIL, `RESULT: PASS`.
- `junediverge.py --check X2Lib/X2DamageManager.cpp KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2DamageManager.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,184,704 B, dated 2026-11-10 11:16:01, md5 60d22c2cb5a3f7f747c958b9b3034753, equal to X2/US_SERVICE/x2.exe (2026-11-10 11:15:54); built at main 2e297802 + 2 uncommitted code file(s).` `EXESTR OK: 2 of 2` ( wide `JUNE_FX CE_HIT`, `[JUNE_CHAR] SUMMON npc` ).
- funcmap rows `3262b6e0 @3262ba99-3262bb4f`, `32757490`, `32ca1240`. No JUNE_BUGS row (no June quirk). `MODS.md` "Stage 7 Rena, CH3b2".

**How to see it in game.** Wind Blast's arrow explodes on the enemy it chases. Gungnir: hold the skill key, the aim mark appears and the arrow keys move it, release fires.

**Play-test (2026-11-10 11:18-11:19, unit 92 set 1, deployed md5 60d22c2c; logs read).** Pass.
- Wind Blast: 5 casts, `wind blast lock on npc uid` 1000011 / 1000000 / 1000011 / 1000001 ( one -1: no target in range at the cast, the in-range retarget found one ); 6 `CE_HIT DamageEffect_SA_RSR_WindBlast_Arrow SPAWN_INHERIT child=..._Arrow_Explosion` and each `HIT ..._Arrow_Explosion COLLIDE defender=1`.
- Gungnir: 2 casts, `spawning 1 NPC(s)` 1000026 / 1000027 each followed by `[JUNE_CHAR] SUMMON npc 2083 uid ... skill 23049` and `ES_CREATE EffectSet_SA_RGA_Gungnir_Loop ... ownerState=232` ( control idle reached ); the spear summoner's `DamageEffect_SA_RSR_Gungnir_Spear` / `_Explosion` / `_FX` hit NPCs 1000021, 1000013, 1000002.
- `ErrorLog.txt`: 0 Motion Error / KEM_ERROR; packets 0 EXCEPTION ( 2 known `E_DISABLE_HB_CHECK_REQ` UNHANDLED ).

### CH3b2 - Review (2026-11-10)

Opus 5.5 high. Kickoff `kickoffs/char-rena/CH3b2-review.md`. **Verdict: pass, no code change.**

- **`EffectToUnit` vs June `3262b6e0` @3262ba99-3262bb4f (Ghidra, read-only).** Pass. EDI = the damage effect: `+0x9a0` = `m_bIsOnlyTargetAttack`, `+0x938/+0x93c` = `m_LockOnUnitUID` (`UidType`, what `GetLockOnUnitUID()` returns), `+0x940` = `m_LockOnNPCUID` (`int`). Switch on defender `+0x264`: 0 (`GUT_USER = 0`) -> vslot 0x54 int64 compare = `pDefender->GetUnitUID()` (virtual on `CX2GameUnit`, `X2GameUnit.h:2252`); 1 (`GUT_NPC`) -> int32 `+0x270` compare = `CX2GUNPC::GetUID()` (`(int)m_UnitUID`); any other type -> `3262f54e` (skip). Placed right after the `ONLY_RELATIONSHIP` block (@3262ba35-3262ba93), as in June. March's block intact in `#else`.
- **Stand-in** (@3262baca-3262bb22). Confirmed. `32481fa0` is true only when the game state is 0x15 and `3266cdc0( field )` matches 0x9c61 / 0x9c62 / 0x9c67-0x9c6c = fields 40033 / 40034 / 40039-40044, which this build lacks (funcmap `32481fa0 (CH2a)`). `32757490` returns `+0x8ee0` (int64) when `+0x8f04` is set and `+0x8f05` == the argument: March `FollowTargetUnitBone` = `UidType` +0x0, `wstring` +0x8 (0x1c B), `m_bUseFollowTargetUnitBone` +0x24, `m_bIsUser` +0x25. The funcmap row holds.
- **Readers.** `m_bIsOnlyTargetAttack`: written only by `ONLY_TARGET_ATTACK` in `X2DamageEffect.cpp:8529` / `:8735`, read only at `X2DamageManager.cpp:2920`. June `DamageEffect.src.lua` sets it on 5 effects: `DamageEffect_SA_RSR_WindBlast_Arrow` (46237), `DamageEffect_RESTRAINTS_SPECTER_Finish_Damage` (111169), `DAMAGE_EFFECT_PROTECT_GUARD_MAGIC_EX` / `_PHYSIC_EX` (257274 / 257341), `DAMAGE_EFFECT_SHADOW_EX` (259373). Their user defenders keep March's compare; an NPC defender now matches by the lock-on NPC UID, as in June. No PvP-only path changes.
- **Data.** Pass. `data036 Enum.lua:2507` `NUI_RENA_TARGETING_GUNGNIR = 2083,` = June `Enum.src.lua:3402`. The `char-rena CH3b2` block at `data037 NPCTemplet.lua:21495-21505` = June `NPCTemplet.src.lua:18661-18668`, all 7 fields identical (only the `--{{ --}}` markers added); BOM + CRLF kept. `ledger --check` PASS (29 rows), `core --check` PASS (27 rows), `luabind.py check Rena_Targeting_Gungnir` 17 methods, 0 unbound, PASS.
- **Rena's other summoned NPC ids.** All 6 have an `Enum.lua` row and an `NPCTemplet.lua` row: 2083, `NUI_ENTANGLE_TRAP_RENA` 457, `NUI_EVOKE_TRAP_LIRE` 1020, `_MEMO` 1025, `NUI_RENA_ANGER_OF_ELF` 1420, `_DELAY` 1421. No miss.
- **Play-test lines** re-read in the logs: `CE_HIT DamageEffect_SA_RSR_WindBlast_Arrow SPAWN_INHERIT child=..._Arrow_Explosion`, `[JUNE_CHAR] SUMMON npc 2083 uid 1000027 skill 23049`, `ES_CREATE EffectSet_SA_RGA_Gungnir_Loop ... ownerState=232`.

**Hygiene.** `flagcheck.py SERV_IRUHADEV_JUNE_ONLY_TARGET_ATTACK_NPC --range 1cb6fa14^..1cb6fa14`: 1 PASS (`X2DamageManager.cpp` +19), 1 DEFINE, 0 deleted, 0 non-ASCII, 0 FAIL, `RESULT: PASS`; later commits naming the flag: `e077384f` only (docs, no code). `junediverge.py --check-all`: allow-listed 202, `RESULT: PASS`. `enumscan.py --check X2Lib/X2DamageManager.cpp`: 0 A/B rows. One-file `/Zs` probe of `X2DamageManager.cpp` (`census.cl_cmd`, no PCH): exit 0, 0 errors. Full build skipped: no play-test (CH3b2's `DEPLOY OK` md5 `60d22c2c` stands). No debug define to retire.

**Next:** CH3c0 (Night Watcher engine hook), kickoff `kickoffs/char-rena/CH3c0.md` checked, now ready.

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

### CH5 - Done (2026-11-09)

- **`UnitTemplet.lua` (data036, core-list BOM source):** Rena's 7 blocks are June's text, block-wise swap (55 blocks before and after, the other 48 untouched, BOM and CRLF kept). Semantic delta vs the old file = exactly `ch0_unittemplet_rena.tsv`
  (13 keys): `m_fLobbyRotationY` 30 / 30 / 25 / 1 / 30 / 20 / 10 on 3 / 12 / 13 / 24 / 102 / 103 / 114, `m_HyperCutIn` `HQ_Cutin_RWS / RGA / RNW_Transcendence` on 102 / 103 / 114, `name` `STR_ID_616` -> `STR_ID_4783` / `_4784` / `_17429`
  ("Wind Sneaker", "Grand Archer ", "Night Watcher"). `luasrc.py refresh` REFRESHED, `ledger --check` PASS, `core --check` PASS, `luabind.py check UnitTemplet` PASS (0 unbound).
  `kom.py pack data036`: PACK OK, `vs live data036.kom: ~1 changed` = `UnitTemplet.lua` only. **`COPY OK data036`** (user yes) md5 `6807d7a65129e53dd89dddede49a95c8`, backup `kom_backup_tools/data036.kom.20261109_201125`.
- **Table (`SERV_IRUHADEV_JUNE_CHAR_RENA_UI`, end of `Always.h`; `#error` without `_RENA_LIVE` / `_JUNE_HYPER_CUT_IN_TIME`):** `s_SkillCutInSet` = June `333aad30` (table `337ae6e8`, 12 entries, stride 0x30, strings by xref): set 0 `HQ_CutIn_Rena_01` / `_02` / `_03`
  (3 / 12 / 13 / 24), set 1 `_WS_01` x3, set 2 `_GA_01` x3, set 3 `_NW_01` x3. No extension, March's one-argument `SkillCutIn( name )`; March's `.dds` table sits in the `#else`. The 0x8b3 pair `333aaba0`: `337ae688` = `HQ_CutIn_Rena_CASH_21`, `337ae6b8` = `_CASH_22`.
- **Reader `ShowActiveSkillCutInAndLight` (June `32c86890`):** CH1f2's `_LIVE` body now has a nested `_UI` arm: copy of the set entry, the 0x8b3 swap (`31e5a860( 0x8b3 )`, even `rand()` -> CASH_21, read from the listing), the RENA UI log. The item override `31fd8a50` stays unported (Stage 12, as the other classes).
- **Already June's, unchanged:** the InitState set-index switch (CH1m: sets 0 / 1 / 2 / 3, sub index 1 / 2 / 0) and `ShowActiveSkillCutInAndLightByScript`.
- **Assets:** `HQ_CutIn_Rena_*.gfx` (8 incl. CASH) in data152 and `HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx` in data195, June's bytes from CH0b. Nothing staged.
- **Logs:** `[JUNE_CHAR] RENA UI cut-in set <set> index <i> name <gfx>` (after the 0x8b3 swap) and `[JUNE_CHAR] RENA UI hyper cut-in class <c> name <gfx>` (`CX2GUUser::ShowHyperSkillCutInByScript`, Rena only).
- **Checklist:** 7t L594, L620-625 and the 7ao hyper cut-in bullet marked `(Rena: CH5)`; funcmap rows `333aad30`, `333aaba0`, `32c86890` updated. **7ah** is Elsword-only (n/a, CH0); the gauge pieces were CH1q2's. The guide dialog, `LevelUpEvent.lua` TYPE row and the `POPUP_TR_*` pictures (already in data215) are CH3d's. No dialog or gauge piece is left for CH5.
- **Not ported, routed:** the cut-in item override `31fd8a50` -> Stage 12. `CH6.md` amended with the cut-in set walk. Review kickoff `kickoffs/char-rena/CH5-review.md` written.
- **Hygiene:** `flagcheck JUNE_CHAR_RENA_UI --worktree --files KTDXLIB/Always.h X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GUUser.cpp`: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL mismatch, 0 token-less #else/#endif, RESULT: PASS.
  `junediverge --check X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GUUser.cpp`: allow-listed 12, RESULT: PASS. `enumscan --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows (the `0x8b3` event id has no Enum entry, as in Raven).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 26,148,864 B, dated 2026-11-09 20:18:29, md5 79a341b4023401beef268cc17d536584`. `EXESTR OK: 10 of 10` (the two log tags and the eight June names `HQ_CutIn_Rena_01/02/03/WS_01/GA_01/NW_01/CASH_21/CASH_22`, wide), `EXESTR OK: 6 of 6 absent` (March's `HQ_CutIn_Lena_1/2/3`, `_Rena_Wind_Sneaker_1`, `_Grand_Archer_1`, `_Night_Watcher_1` `.dds`).
- **See it in game:** select screen with all nine March characters, then Rena's classes: the lobby pose turns by class (30 / 30 / 25 / 1 / 30 / 20 / 10), and 102 / 103 / 114 are named "Wind Sneaker" / "Grand Archer" / "Night Watcher". A skill cut-in plays June's art: `HQ_CutIn_Rena_01..03` on 3 / 12 / 13 / 24, `_WS_01` on 102, `_GA_01` on 103, `_NW_01` on 114.
  The awakening hyper on 102 / 103 / 114 shows `HQ_Cutin_R{WS,GA,NW}_Transcendence` only when the class script's awakening state sets `SHOW_HYPER_CUT_IN_TIME` (CH6's walk). Rena's skills are June's only from CH2a: a March-kit cast that reaches `ShowActiveSkillCutInAndLight` is the test.
- **Play-test (user, 2026-11-09 20:22-20:24, exe `79a341b4`): passed.** Select screen with all nine March characters and Rena's classes OK (lobby poses, second-job names). `offline_server.log` has 11 `[JUNE_CHAR] RENA UI cut-in` lines, each with a
  `[JUNE_GFX] CUTIN play ui=0xc <gfx>` in the same second: set 0 index 0 `HQ_CutIn_Rena_01` x2, set 2 `_GA_01` x3, set 3 `_NW_01` x3, set 1 `_WS_01` x3. ErrorLog 0 `Motion Error`, 0 `KEM_ERROR`. Outside CH5, no routing from here: 5 `E_DISABLE_HB_CHECK_REQ` UNHANDLED
  and `AddGroupChainQuest` unbound (quest file). Sets 0 indices 1 / 2 and the hyper cut-ins (102 / 103 / 114) are CH6's walk.

### CH5 - Review (2026-11-09, Opus 5.5): PASS, no code change

I read each item against June in Ghidra (`333aad30`, `333aaba0`, `32c86890`, `32cadc90`) and decoded the targets Ghidra leaves out from the exe bytes (`charport_consts.read_va` + capstone).
- **Table `333aad30`.** 12 `SkillCutIn` at `337ae6e8` .. `337ae8f8`, stride 0x30. Strings in push order: `3351afc8` `HQ_CutIn_Rena_01`, `afa4` `_02`, `af80` `_03`, `af58` `_WS_01` x3, `af30` `_GA_01` x3, `af08` `_NW_01` x3 (lengths 0x10 / 0x13, read back from the exe).
  Each entry stores size `( [334367ac] x2 )`, position `( [33435598], [334367ac] )`: March's one-argument `SkillCutIn( name )`. The port's `s_SkillCutInSet` is this table, row for row.
- **Pair `333aaba0`.** `3351aedc` `HQ_CutIn_Rena_CASH_21` -> `337ae688`, `3351aeb0` `_CASH_22` -> `337ae6b8` = `s_SkillCutInSetEvent8b3[0]` / `[1]`.
- **Reader `32c86890`.** Gate `[anim+0x24c] < t`, `31d28e10` (EventTimerOneshot), `32001380`, `31fd3180`. Copy of `337ae6e8 + ( index + [this+0x8ac8] * 3 ) * 0x30`. Item override `31fd8a50` (not ported, Stage 12).
  Then `[33775864] && 31e5a860( 0x8b3 )`: `rand() AND 0x80000001`, zero (even) -> `337ae688` CASH_21. Then `31fde0f0( bone, name, size, pos, IsHyperState, only light )`. The port matches in that order (`IF_EVENT_ENABLED` = the same NULL test + `IsEnableCode`, as Raven's).
  `ShowActiveSkillCutInAndLightByScript` (uncarved `32c86af0`) pushes `3345bb58` `Dummy1_Rhand` and `[this+0x8acc]`, then calls the reader: March's body.
- **Set index (InitState `32cadc90` @32cadcdb).** `class - 3; cmp 0x6f; ja; movzx byte [32cb056c + class-3]; jmp [32cb0558 + idx*4]`. The byte map sends 3 / 12 / 13 / 24 -> 0, 102 -> 1, 103 -> 2, 114 -> 3, the other 105 values -> 4 (default, value kept).
  Targets 0-3 store +0x8ac8 = EBX (0), EDI (1), EDX (2), 3. Sub index +0x8acc: 12 -> 1, 13 -> 2, else 0. This equals CH1m's switch (`_LIVE`, InitState). No index can reach past set 3.
- **UnitTemplet.** Blocks split on `AddUnitTemplet`, keyed on `unitClass`, BOM / CRLF normalised. Current, pre-phase (`elstools extract` of `kom_backup_tools/data036.kom.20261109_201125`) and June: 55 / 55 / 87 blocks; current and pre-phase have the same keys in the same order.
  The 7 `UC_LIRE_*` blocks equal June's text 7 of 7; the other 48 equal pre-phase 48 of 48. Key delta = the 13 of `ch0_unittemplet_rena.tsv`. BOM, 1485 CRLF, 0 lone LF.
  `ledger --check`: LEDGER CHECK PASS (0 FAIL, 0 WARN). `core --check`: CORE CHECK PASS. `luabind.py check UnitTemplet`: RESULT: PASS (0 unbound).
  `HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx` are in data195 (same case as the rows). `STR_ID_4783` / `_4784` (`StringID_def2.h:2290-2291`) and `STR_ID_17429` (`StringID_def7.h:2436`) are defined.
- **Routing.** `kickoffs/char-rena/CH3d.md:25-28` names the guide dialog, the `LevelUpEvent.lua` TYPE row and the `POPUP_TR_*` pictures. 7ah n/a (Elsword-only); gauge = CH1q2.
- **Logs (play-test 20:22-20:24, exe `79a341b4`).** 11 `[JUNE_CHAR] RENA UI cut-in` lines (sets 0 x2, 2 x3, 3 x3, 1 x3, all index 0), 11 `JUNE_GFX` CUTIN plays, 0 `Motion Error`, 0 `KEM_ERROR`, 0 EXCEPTION.
  Note: `logs.py --grep "JUNE_CHAR RENA UI"` matches nothing (the tag is bracketed); grep `"RENA UI"`.
- **Left as is.** The CH1f2 one-line comment above the reader body (`not ported: ... the event 0x8b3 pair, with CH5's cut-in table`) is now stale: CH5 ported the pair. Comment only; not rewritten here (the file carries a peer's uncommitted CH2a edits).
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_UI --range 5a08ae95^..5a08ae95`: 2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT: PASS; no later commit touches the flag (`git log -S`).
  `junediverge.py --check-all`: allow-listed 202, RESULT: PASS. `enumscan --check X2Lib/X2GULire_ElvenRanger.cpp`: 0 A/B rows (working tree incl. a peer's CH2a edits). Full build skipped: no play-test (no code change; deployed exe stays `79a341b4`).
- **Next.** `kickoffs/char-rena/CH6.md` "Amended by CH5" walks sets 0-3, indices 0-2 and the hyper cut-ins on 102 / 103 / 114. CH6 still waits on CH3d and CH4a-b reviews.

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
