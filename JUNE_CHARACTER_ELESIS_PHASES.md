# June character port: Elesis (Stage 7, seventh character after the pilot; CH-N reused from Ara)

Plan key `char-elesis` · branch `main` · kickoffs `kickoffs/char-elesis/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] ELESIS <sub>`, sub-tags
`DATA`, `CORE`, `WOS`, `CLASS`, `SUMMON`, `SKILL`, `PASSIVE`, `BLOOD`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_ELESIS_*` for Elesis's class code,
`SERV_IRUHADEV_JUNE_CLASS_PATH` (Ara's CHNa) for the class-generic class-path fixes (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-11 by the `new-plan char-elesis` session from the Ara
plan (`JUNE_CHARACTER_ARA_PHASES.md`, the most recent worked example and the first CH-N run) and the Elsword plan's Way of the Sword phases.

## Context

**What this plan does.** It moves Elesis (unit type `UT_ELESIS`, script `ELESIS_KNIGHT.lua`, class `CX2GUEL` in `X2Lib/X2GUEL.cpp` / `X2GUEl.h`,
seven classes up to transcendence: Knight 8, Saber Knight 29, Pyro Knight 30, **Dark Knight 35**, Grand Master 120, Blazing Heart 121, **Crimson
Avenger 125**) from her March kit to her June 2017 kit, end to end: June script, skill states for June's tree, numerical damage, motions, class-only
engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), her class side of the Way of the Sword, her four summons, the Dark Knight / Crimson
Avenger blood hooks, UI pieces (the creation screen and the class-change dialogs included), data, her transcendence rows, and **her June-only class
path Dark Knight -> Crimson Avenger (CH-N, reused from Ara)**.

**Why Elesis is seventh (stage7-plan, 2026-10-22).** After Ara, so CH-N is reused, not written. The brief's premises hold with corrections:
the gap scan line is in "Planning evidence"; "the first live reserved-buff path (7ai)" is hers at CH1 final (Ara reaches the same path at CH2d /
CH3b); and **she summons** (the brief did not say so): four NPC ids, one of them already in March.

**Her size.** June's class spans `0x32b4e770`-`0x32b90b6f` (Lu/Ciel's thunk run ends `32b4e76f`; Ara starts `32b90b70`): **274** carved functions,
**409** thunks in two tables (T1 `32b8ecf0`-`32b900d0` 319 entries, T2 `32b905c0`-`32b90b50` 90), whose targets are her state functions: **234 of
the 409 targets have no Ghidra function**, as do **33 of her 66 vtable overrides** (list in `plan_june_side_binary.md` §2). June's script has
**305** state tables (283 `ESI_`, 22 bare `SI_`), 60 of them on the Dark Knight / Crimson Avenger line, and **261** skill-tree rows over the seven
classes (Ara 288). Expect Ara's size. **About half her functions are read from the raw listing only** (`charport_adis.py`, which every
transcription phase runs anyway): CH0 marks each class-map row `carved` / `listing-only`, and the CH1 reviews read every listing-only function.

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28).** A June body goes inside March's function of the same name under `_LIVE`,
March's in `#else`, proved by a one-file probe compile that defines `_LIVE`. A June function March's class lacks is written under `_CORE` with a
plain name in the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** June's Enum names three Elesis third jobs (`UC_ELESIS_EMPIRE_SWORD` 171,
`_FLAME_LORD` 172, `_BLOODY_QUEEN` 173) but no other table has data for them: out of scope. Transcendence is live (Stage 6), so her transcendence
rows (120 / 121 / 125) are in CH3d and CH6.

**Order.** char-ara is at CH1q2 (running 2026-11-11). **CH0 runs now** (read-only, alongside anything); **CH0b after char-ara CH6's walk** (it packs
and copies koms the walk uses); **every code phase after char-ara Z** (one character's code at a time: `Always.h`, `X2GUUser.*` are shared), and CHNa
builds on Ara's `SERV_IRUHADEV_JUNE_CLASS_PATH`. Lu/Ciel stays parked until Stage 7 closes (user, 2026-11-03); Add follows Elesis.

**The state she starts from.**
- **Class code (March, live).** `X2GUEL.cpp` 9,794 lines (9,531 CRLF, 263 LF, 7 bare CR), `X2GUEl.h` 1,069 (260 CRLF, 809 LF), CP949. The whole
  class sits inside `#ifdef NEW_CHARACTER_EL` (`.cpp:4-9792`, `.h:4-1070`); no function outside it. **Every Elesis macro is ON** under `US_SERVICE`
  (`NEW_CHARACTER_EL`, `SERV_ELESIS_SECOND_CLASS_CHANGE` with about 3,240 lines, ...; `cl /P` probe, `plan_march_side.md` §2): **no dormant
  system**. `SERV_NEW_CHARACTER_EL` is ON too (`ServerDefine.h:3732`), so the emulator gives her `CAC_WSP` like Elsword; the comments that say
  "off" at `X2OfflineStatTable.cpp:292-294`, `X2OfflineClassChangeSeed.h:65-93` and `Handlers_Shop.cpp:440` are stale.
- **Way of the Sword (live March code; the shared half is already June's).** char-elsword CH1q / CH1q2 ported the shared pieces under
  `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (live): the gain rate `ApplyWayOfSwordPointGainRate`, `GetDestRageWSPGain_Elesis`,
  `CalcVigorMpUpOnAttack_Elesis`, `CX2Game::UpdateElswordSkillSlotEffect`. **Still March, hers:** the class body (`X2GUEL.cpp:3682-4046`: Get / Set
  state and point `:3684-3714`, `Start_SwordmanVigor` `:3716`, `Start_SwordmanDestruction` `:3740`, `WayOfSwordStateChange` `:3769`,
  `GetFinalDamage_WayOfSword` `:4004`; `_CONST_EL_::GALE_*` `:24-31` against June's `WayOfSword.lua` `Const_Elesis_VigorUse*` /
  `DestRageWSPGain_Elesis`), `CX2ElesisGageData` (`InitWhenGameIsOver` still resets the state, `X2GageData.cpp:1129`; June `32a34c60` does not) and
  `CX2ElesisMyGageUI` (no `RefreshWayOfSwordUI`, June vslot 0x5c `32e69e70`; `InitUI` order, June `32e6e060`). June's class side: vslots 0x674 /
  0x678 / 0x67c `32b52790` / `32b527e0` / `32b52800`, Start / End Vigor `32b74760` / `32b74930`, Start / End Destruction `32b7f0b0` / `32b74bf0`
  (both Ends also erase the `..._DARK_KNIGHT` buffs), `WayOfSwordStateChange` `32b7f370`, the state-end hooks `32b74f80` / `32b74fc0`,
  `GetFinalDamage_WayOfSword` 0x74 `32b74ed0`, `SetCriticalPercent_WayOfSword` 0x78 `32b4ef90`, `CreateGageData` 0xd0 `32b7ef90`, the gain-rate
  getters 0x720 / 0x724 / 0x72c (`+0x9714` / `+0x9718` / `+0x9720`; two stand-ins wait on 0x720: `X2DamageEffect.cpp:5543`, `X2GUUser.cpp:14837`),
  the Crest of Time pair 0x740 / 0x744 `32b6ac80` / `32b6acb0`. Phase **CH1q**, as Elsword's.
- **Summons (June 5 sites, 4 NPC ids; March 1).** `NUI_ELESIS_FIRE_BLOSSOM` 1567 (March `X2GUEL.cpp:3667`; June `32b7a058` in `32b79db0` and
  `32b746e8` in vslot 0x250 `32b74500`), `NUI_SI_SA_SBH_WALL_OF_PROMINENCE` 1635 and `_FIREKING` 1971 (`32b7a4f5` / `32b7a442` in `32b7a0a0`),
  `NUI_SI_SA_SCA_BLOOD_CUTTER` 1641 (`32b8b3b7` inside the uncarved state function `32b8b0c0`); `CanUseSpecialAttack` calls
  `CheckSummonSpecialAttack` `3202daf0`; the Fire Blossom script reads `GetRelatedSkillLevel` (the `+0x8e74` write the pilot omitted). Chung, Rena
  and Eve each needed an Opus spawn phase: hers is **CH3b0**. The same vslot pair 0x24c / 0x250 (`32b89260` / `32b74500`) holds the two writers of
  `m_fCreateEffectSetRate` (`+0x8e50`) char-elsword CH4a2 left to her: until CH3b0, `EffectSet_Elesis_SBH_Blaze_Wing_Soul_Ignition` plays at rate 0.
- **Dark Knight / Crimson Avenger blood hooks (June-only).** The hit hooks 0x4f8 `32b7beb0`, 0x4fc `32b898a0`, 0x734 `32b7bad0` (blood sucking,
  bleeding; base stubs, inert until her port: checklist l.454-455), Blood Runaway `32b87880` (BFI 462, BTI 426) and `32b734d0`, `32b7e3b0`, the
  Madness chain `32b6ff20`, the transcendence berserker chain `32b7e9a0` (BFI 913), the per-frame list 0x730 `32b816a0` over `+0x9700`. Phase
  **CH4c** (Opus), after the Crimson Avenger skills.
- **Skill system (Stage 3).** `s3_reform_ids_no_state.tsv` has her five March classes only (no 35 / 125 rows: template recipe defect 2). The offline
  auto-learn already has rows for all seven (8: 6, 29 / 30 / 35: 11 each, 120 / 121 / 125: 21 each).
- **Effect engine (June's).** `DamageEffect.lua` / `EffectSet.lua` ship merged; no `c10a` alpha site and no `march-consumer` `c11a` row is hers.
- **Motions.** June 82 Elesis names, March 9 class files (data033): 6 same-name files differ in size (`Motion_EL_Common.x` 1.4 -> 6.1 MB, `_Field`,
  `_Emotion`, `_PK`, `_GM`, `_BH`), 19 same size, **57 June-only** (`Motion_EL_DK.x`, `MOTION_EL_CA.x` in June data206, `Motion_Standard_El.X`, 54
  emotions). Whether June drops a name March's script plays decides if the 6 deploy in CH0b or with CH1 final (7ad): CH0 answers.
- **June-only class path (CH-N).** `X2Unit.h` names `UC_ELESIS_DARK_KNIGHT` 35 (`:165`) and `UC_ELESIS_CRIMSON_AVENGER` 125 (`:179`) after `UC_END`
  (= 124), so 125 fails every `< UC_END` test until Ara's CHNa lands the class-generic bounds (`X2SocketItem.cpp:2897` -> 202, `GetClassLevel`,
  the optional log sites). **Elesis's own:** `GetX2UnitClassCompatibility` arms (`X2Unit.h:2057-2123`), the third-line helpers
  (`X2Unit.h:2685-2750`, assert-only today), the gauge create (`X2GageManager.cpp:194-209` has no 35 / 125 arm while `CX2GUEL::CreateGageData`
  `:1970` and `GetELGageData` cast to `CX2ElesisGageData`: **Ara's crash, hers too**), `X2Data.cpp:2872-2892` picture rows, the lobby particles
  (`X2UnitViewerUI.cpp:630` / `:2414` / `:2530`), `X2ComboTree.cpp:302`, the class switches inside `X2GUEL.cpp` (`:213`, `:249`, `:411`, `:2747`,
  `:9581`), `tools/createchar.py`. June's route: Dark Knight quests **35569-35572** (`CJT_JOB_THIRD`, from 8 at level 15), Crimson Avenger
  **35573-35577** (from 35 at level 35); the staged `FieldQuest.lua` already has them (titles matched; CH0 confirms the ids and rewards).
  `DLG_Class_Change_Elesis_DK` / `_CA` and `DLG_1ST` / `2ND_Class_Change_Guide_Elesis.lua` are June-only.
- **Stat rows.** `StatTable.lua` has 99 rows each for 8, 29 and 30 only (29 and 30 equal 8 at every level). **Grand Master and Blazing Heart have no
  rows**: `X2OfflineStatTable.cpp` (`:355-369`) falls back to her unit type's rows, silently, so 120 / 121 play on class 8's numbers today, and 35 /
  125 would too once their `UnitTemplet` rows exist. Open decision 1 (answered 2026-11-11): CHNa writes rows for 35 / 120 / 121 / 125 by Ara's rule.
- **Creation screen (user, 2026-11-11).** March's `DLG_Unit_Create_ClassChange_Button_Elesis.lua` (1,923 B) has two buttons (Saber Knight, Pyro
  Knight); June's (5,284 B) has six. The six `SCUUCM_*_CLASS_CHANGE_BUTTON_UP` handlers are generic (`X2StateCreateUnit.cpp:778-808`,
  `SelectUpperClass` `:3397`) and March's line helpers already carry SK -> GM and PK -> BH, so **Grand Master and Blazing Heart appear with June's
  dialog (CH5)**; the third line (Dark Knight, Crimson Avenger) needs the helper arms and the stat-chart arms (**CHNa**).
- **Class-change dialog at level 15 (user, 2026-11-11).** March's `LevelUpEvent.lua` opens the old `Class_Change_Guide_Elesis.lua`; June's row for
  class 8 opens `DLG_1ST_Class_Change_Guide_Elesis.lua` (three paths), the level-35 row adds 35 and opens `DLG_2ND_...` (**CHNb**; CH0b stages them).
- **Hooks waiting on the class port.** 35 checklist rows name her and 27 class-generic rows apply (`plan_checklist_rows.md`): the pressed-index
  set (`X2GUUser.cpp:26778-26797` says "Elesis, Add not ported"; her `CountUsedSkill` `X2GUEL.cpp:3157`), `SetDamageExceptionProcess` 0x2b8
  `32b80f20` with the June-only 28416 / 28417 part (`@32b81008-32b811ef`), the delegate pushes in `InitPassiveSkillState` `32b88300` (7), 0x428
  `32b52260`, the class float writer `32b73bf0` / reader `32b7d550` (l.496, state unnamed), BFI 910 / 913 callers `32b7e96c` / `32b7ebae`,
  `32b90420` returning June state `0xbb` (March's `0xbb` is another state: by name, never the number), her tutorial test `32b74065`, the three BFI
  names missing from June's `Enum.lua` (`JUNE_BUGS` 279). **Not hers:** the Crest of Time bases and the death-prevention gate (char-add), the
  training F7 key `322ed400` (backlog Stage 2); funcmap l.1420's `32b45e0c` is in Lu/Ciel's range, and `32b901c0` `IsDashState` is a shared base.
- **Her own March patch.** `SERV_IRUHADEV_JUNE_EL_DASHJUMP_XZ_FIREBALL` (`X2GUEL.cpp:7257-7292`, June `32b770e0`): the phase that owns that state
  writes June's whole function under `_LIVE`; the old block stays in the `#else` and Z records the flag as superseded.

**Out of scope (owned elsewhere).** Third jobs; new characters (Stage 8); UI that is not hers (Stage 9); tutorials and training dungeons
(june-dungeons); NPC / boss copies of Elesis; emotion motions' behaviour (Stage 10; CH0b stages the 54 files); `FORCE_DYNAMIC_CAMERA_INDEX` x15 /
`RETAIN_CAMERA_STATE` (Stage 9); `IMMUNITY_EDT_LIST_AT_THIS_STATE` x4 (backlog Stage 2); PvP-only paths (no PvP offline); stat and damage math
(Stage 33); the jumping-event dialogs (`DLG_UI_JUMPING_*`).

## Rules

The Ara plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules"`), with Elesis's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim with a
  `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in her skill-use check.
- **`ReservedSkillStart` (user, 2026-11-05).** March's `CX2GUEL` has no override. CH1k writes `CX2GUEL::ReservedSkillStart` =
  `SpecialAttackCooldown( skillID, m_fReservedSkillMp )` then `SkillMemoMpRefund( ... )`, widening the base hook chain in `X2GUUser.h` / `.cpp` with
  her `_LIVE` flag (copy Ara's); CH1 final's play-test casts each skill twice (cool time shows, MP drops).
- **Slots by role, never by Ara's offset.** Elesis overrides 66 slots, Ara 77, 46 in common. She does **not** override 0x4e0
  (`AttackResultByType`), 0x780, 0x794, 0x68c (`InitAquiredActiveSkillState`), 0xc0 / 0xc8, 0x6a8, 0x3ac, 0x9c / 0xa0: no Ara hook is carried over
  by slot number. Every override's role is confirmed from its body (CH0 item 1).
- **Listing-only functions.** Ghidra stays read-only: a function with no Ghidra entry is read with `charport_adis.py elesis <va>` (CH0 adds
  `elesis` to its `CLASS_RANGE` as `(0x32b4e770, 0x32b90b6f)`), never by creating functions. A Sonnet phase whose function has no decompile and a
  control flow the listing does not make plain says "this needs Opus" for that function and goes on with the rest.
- **June's ids in passive code.** Every `SI_*` id March's `InitPassiveSkillState` / skill code reads is checked against June's `RNSI_*` id (March
  8000s, June 28000s). CH0 lists them; CH4a fixes what CH1 did not.
- **The checklist is the work list.** Every Elesis row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase that
  ports a row appends `(Elesis: CHn)` to it in place.
- **One deploy unit.** June's `ELESIS_KNIGHT.lua`, her June motions (held for CH1 if CH0 finds a dropped name) and the C++ that reads them go live
  together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (`//#define` in `Always.h` until then). Between phases she stays playable on
  her March kit. Classes 35 / 125 are playable only from CHNa on.
- **Flags:** `_CORE` (June functions March lacks, compiled, unreferenced), `_LIVE` (June bodies inside March's functions, the registration, the
  switch), `_WOS` (her Way of the Sword class side and gauge twins, CH1q; defined only with `_LIVE`), `_CLASS` (her 35 / 125 rows, CHNa / CHNb),
  `_SUMMON` (CH3b0), `_SKILL`, `_TRANSCEND`, `_PASSIVE`, `_BLOOD` (CH4c), `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a
  temporary `_DEBUG` per sub-phase, retired in its review. The class-generic class-path fixes are Ara's `SERV_IRUHADEV_JUNE_CLASS_PATH`: widen
  it, never copy it. New namespaces are `_CONST_EL_<phase>_` (March has `_CONST_EL_` at `X2GUEL.cpp:6-53`, used qualified).
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name; a one-line `// char-elesis CH1n` tag on
  every edited block (CLAUDE.md, 2026-11-09). `python tools/junediverge.py --list` before transcribing; `--check X2Lib/X2GUEL.cpp X2Lib/X2GUEl.h
  <other files>` = `RESULT: PASS` at every finish. Rows that name her today: 417, 420, 421, 424, 427 (Way of the Sword) and 365 (class cases).
- **Shared `CX2GUUser` helpers, never copied into the class:** `GetSkillLevel` / `GetLearnedSkillAbilityValue`
  (`SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); the seven skill-use helpers and their fields (`SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`);
  the user state key `CREATE_EFFECT_SET` (`SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET`); Elsword's Way of the Sword helpers; the earlier
  characters' `CX2GUUser` bases: a phase that needs one under its own flag widens the `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GUEL.cpp` stays 0. March's C++ enum
  `EL_STATE_ID` (`X2GUEl.h:9-226`, 130 members, no `ESI_END`) takes June's missing names with June's ids. June's `Enum.lua` names **no** Elesis
  state, so every name is resolved from the exe's state-name strings (as Rena's `ch0_state_enum_names`); June's `USI_END` is 152 (March 100). The
  NPC names for 1635 / 1641 / 1971 are registered in `X2UnitManager.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES` (CH3b0).
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces her 5 rows and adds June's 35 / 125 rows. Her class script is
  adopted whole (`adopt-june`, bytecode unless a phase must edit it) and passes `python tools/luabind.py check ELESIS_KNIGHT`; so do the four
  summon scripts (CH3b0). March's `ELESIS_KNIGHT.lua` (157,500 B bytecode, not in the ledger) is replaced, not converted.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack and copy yourself (CLAUDE.md, 2026-11-10).
- **CP949.** `X2GUEL.cpp` / `X2GUEl.h` (mixed CRLF / LF: match the neighbouring lines), `X2GUUser.*`, `X2Gage*.*`, `X2Unit.h`,
  `X2StateCreateUnit.*`, `X2DamageManager.*` are CP949: `bytepatch.py` only; `cl /P` decides every `#ifdef` branch.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** Units 20 `wewe` (8 lv7), 40 `SaberT` (29 lv70), 21 `eresisu` (30 lv77), 29 `ElesisGM` (120 lv80), 30 `ElesisBH` (121 lv80), all
  transcendence 0; no 35 / 125. Every March class has a unit, so CH0b creates none; CHNa creates 35 / 125 once the class loads. Play-test saves
  never slot a skill in slot 4 / 9 before transcendence, and learn only the ids of the bank being tested.
- **Offline reach (Raven lesson 4).** Each phase's hand-off names only checks the offline client can reach; a June path that only a server or PvP
  drives is a stand-in or a routed item, decided in the phase, not found in CH6.
- **One code phase at a time in `X2GUEL.*`** (CH1-CH4). CH5 and CHNa / CHNb may run beside CH2-CH4 when their Touches do not overlap.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "<heading>"` (`--no-sub` to skip Done notes) |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; her rows with line numbers and everything other plans routed to her: `june2017_re/char_elesis/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_elesis/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (CH0 adds `elesis`), `charport_regsim.py`, `s3_no_state.py`; Ara's builders `june2017_re/char_ara/ch0_*.py` (copy into `char_elesis/` with her range); `char_arme/ch0c_listing_check.py`; `char_eve/ch0_build_assets.py` (CH0 adds `elesis`) |
| June Lua (reading) | `june_src_fixed/data036_kom/ELESIS_KNIGHT.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`, `CharacterStatTable.src.lua`, `FieldQuest.src.lua`, `LevelUpEvent.src.lua`, `WayOfSword.src.lua`; `june_src_fixed/data034_kom/DLG_Unit_Create_ClassChange_Button_Elesis.src.lua`, `DLG_1ST` / `2ND_Class_Change_Guide_Elesis.src.lua` |
| Ghidra | `ghidra` skill; her range `0x32b4e770-0x32b90b6f`, vtable `334fa61c` (544 slots, 66 overrides, 33 uncarved), RTTI `.?AVCX2GUEL@@` (type descriptor `3373d5e4`), ctor `32b70ee0` (vtable write `32b70f3e`), dtor `32b70990`, factory `32b72430`, `MemoConditionElesis` vtable `334fa4f4` (xrefs `32b72400`, `32b642f8`), gauge data vtable `3349b184`, gauge UI vtable `3349ac34`; read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow; revert the regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` after, and only those), `python charport_keys.py ELESIS_KNIGHT` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 ELESIS_KNIGHT.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR ELESIS"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-11)

Kept in `june2017_re/char_elesis/` (digests by Sonnet subagents, spot-checked: three uncarved thunk targets and one carved override in Ghidra, the
35 / 125 enum lines, the two-button March dialog, the `LevelUpEvent` rows, the Way of the Sword dialog sizes; CH0 re-verifies anything it builds
on): `plan_charport_scan_20261111.txt`, `plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`, `plan_june_side_binary.md`,
`plan_june_side_data.md`, `plan_numerical_rows.tsv`, `plan_checklist_rows.md`.

Scans re-run 2026-11-11: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 653, june-ignores 305, handed-off 8; regenerated
`census.tsv` / matrix reverted); `charport_scan.py` (scratch `C:/tmp/cps_elesis`, seeded from Ara's finished scratch after a 258-file `cmp` because
the fresh decompile hung on `ManufactureItem.lua`; output identical to Ara's 2026-11-10 run):
`ELESIS_KNIGHT {'no NUMERICAL_DATA_INDEX': 69, 'no DAMAGE table': 20}` (the brief's gap 20 holds), no ENUM line, no KEYS line;
`s3_no_state.py --family X2GUEL.cpp --preprocess`: `rows=179 no_state=174 has_state=5` (only id 900 has a state, once per class; 8: 15, 29 / 30:
29 each, 120 / 121: 53 each). **The S3 tsv has no rows for 35 / 125** (template recipe defect 2): CH0 item 12 appends them.

| Measure | Elesis | (Ara) |
|---|---|---|
| June class range | `32b4e770`-`32b90b6f`, 274 carved; thunk tables T1 319 + T2 90 = 409 thunks, 409 distinct targets, **234 uncarved**; 66 vtable overrides (65 in range + the folded stub `32c69f70`), **33 uncarved**; 0x2b8, 0x6a0, 0x6a4, 0x428, 0xd0, 0x29c, 0x740, 0x744 overridden; 0x4e0, 0x780, 0x794, 0x68c, 0xc0 / 0xc8, 0x6a8, 0x3ac, 0x9c / 0xa0 not | 540 + 32, 77 |
| June skill-tree rows | 261 over seven classes (8: 15; 29 / 30 / 35: 29 each; 120 / 121 / 125: 53 each), **127** distinct ids (106 .. 28605; 29 / 30 / 35: `RNSI_SA` 16, `RNSI_A` 10, `RNSI_P` 2, `SI_ETC` 1; 120 / 121 / 125: `RNSI_SA` 27, `RNSI_A` 15, `RNSI_P` 8, `RNSI_HA` 2, `SI_ETC` 1); 0 unnamed; the S3 tsv lacks 35 / 125 | 288 / 139 |
| June script | `ELESIS_KNIGHT.src.lua` 20,161 lines: 305 states (283 `ESI_` + 22 bare `SI_`, all Dark Knight / Crimson Avenger skills) + 6 `INIT_*` (`MAX_HYPER_MODE_TIME` 30, `HYPER_MODE_COUNT` 1, `MP_CHARGE_RATE` 130; no way-of-sword init key; **no `PROMOTION_ITEM_ID_LIST`**: the promotion items are in `UnitTemplet`); groups DASH 50, SI 48, RIDING 27, COMBO 25, SA 23, DAMAGE 21, A 8, JUMP 6, STANDUP 6, PEPPER 5, QUICK 4, DANCE 3; by job tag: common 116, EL 19, SSK 20, SGM 38, SPK 19, SBH 31, SDK 29, SCA 30, hyper 3 (EGM / EBH / ECA) | 331 |
| Script keys | `DAMAGE_DATA` 79 (+8 `_NEXT`), `AT_COMMAND` 60, `APPLIED_SKILL_ID` 33, `TIME_STOP_IMMUNE_STATE` 3, `SHOW_HYPER_CUT_IN_TIME` 3, `SHOW_SKILL_CUT_IN_TIME` 43, `CUSTOM_RENDER_STATE` 0, `EFFECT_SET_LIST` 11 / `_EX` 103, `CREATE_EFFECT_SET` 46; class keys `WAY_OF_SWORD_TYPE` x78 (vigor 31, destruction 47; shared with Elsword), `EFFECT_SET_LIST_EX_NO_TRANS` x4, `_EX_TRANS_1` x4, `_EX_APPLIED_SKILL` x8, `EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL` x1, `RELATIVE_SKILL_LEARN_CHECK` x8, `IGNORE_STATE_DEFENCE` x2, `IGNORE_DEFENCE_DECLINE` x2; 4 `MEMO` state tables | 54 / 36 / 16 |
| Key census (`charport_keys.py ELESIS_KNIGHT`) | 248 keys: both 234; june-only **9** (six `BBT_*` / `BFT_*` damage-data keys with no reader, the camera pair (Stage 9), `IMMUNITY_EDT` (Stage 2)); neither 1 (`HEAD_IK` x9); march-only-site 4 (CH0) | 250 / 14 / 7 / 4 |
| Numerical blocks | 142 `c1a` rows keyed to 48 ids (28000-28602), all `ok` | 160 / 59 |
| Motions | June 82 / March 9 class files; 6 differ in size, 19 same size, 57 June-only (`DK`, `CA`, `Standard`, 54 emotions); names dropped: CH0 | 75 / 17 |
| Alpha-tag sites (7c) / March-only templets (7u) | **0** / **0** | 0 / 0 |
| Summons | 5 sites, 4 NPC ids (1567 in March already; 1635, 1971, 1641 June-only) | none |
| Class mechanics | Way of the Sword class side: live March code, June's twins (CH1q); blood hooks: June-only (CH4c) | force power, Specter |
| March class | `.cpp` 9,794 lines, `.h` 1,069 (`EL_STATE_ID` 130 members: common 35, Knight 17, endurance 2, SSK 16, SPK 12, SGM 26, SBH 22; no state id in a `UCHAR`); `_CONST_EL_` `:6-53`, ctor `:83`, Init `:201`, InitState `:247-310` (cut-in switch `:249-273`), InitComponent `:311`, InitPassiveSkillState `:350-659`, `SetEquippedSkillFuncBySkillSlotIndex` `:660-1551` (42 `case SI_`), SetEquippedSkillLevel `:1552-1684`, CommonFrameMove `:1704`, SpecialAttackEventProcess `:1799`, GetActualMPConsume `:1844`, GetActualCoolTime `:1929`, CreateGageData `:1970`, InitStateCommon `:2004-2743`, InitStateByUnitClass `:2744-2970`, CanUseSpecialAttack `:3012`, StateChangeSpecialAttack `:3151`, ReInit `:3248`, DamageReact `:3308`, HyperModeFrameMove `:3504`, the Fire Blossom spawn `:3667`, Way of the Sword `:3682-4046`, SetDamageExceptionProcess `:9702`; no `ReservedSkillStart`, `InitAquiredActiveSkillState`, `AttackResultByType` | 17,140 |
| June class | InitState `32b87a60` (0x7a4, wrapper) -> parts `32b81be0` (111 names) + `32b864f0` (57), base `31fd8f00`; `SetEquippedSkillFuncBySkillSlotIndex` `32b8c470` (0x7b4, four jump-table switches: 28031.., 5.., 28000.. with 55 targets, 28601..); SetEquippedSkillLevel `32b4e790` (0x7b8, uncarved, 22 id blocks); InitComponent `32b81710`; InitPassiveSkillState `32b88300` (0x690); SpecialAttackEventProcess `32b4ee20` (0x7f4, uncarved) + CanUseSpecialAttack `32b84a50` (0x7f8), 0x7fc `32b52590`, 0x800 `32b73e60`; NoStateChangeActionFrameMove `32b818b0` (+ Endurance `32b80c60`); DamageReact `32b73ff0`; HyperModeBuffEffectStart `32b74300` (0xc4, uncarved); ReInit `32b81900` (uncarved); Init `32b52560` (uncarved); InitEffect `32b4ef40`; CommonFrameMove `32b88070`; HyperModeFrameMove `32b7f090` -> `32b7b820`; the June-only hook 0x83c `32b732e0` | |
| Stat rows | `StatTable.lua` 1-99 for 8 / 29 / 30 (level 1: 360, 296, 80, 66, 11250); **none for 120 / 121 / 35 / 125** (type fallback, silent); June `CharacterStatTable` relative rows: `UT_ELESIS` `450, 100, 370, 83, 11250`, Grand Master `500, 112, 400, 90, 12250`, Blazing Heart `490, 110, 430, 92, 11750`, Crimson Avenger `510, 102, 410, 97, 12000` | 1-99 for 5 |
| Save | units 20 (8 lv7), 40 (29 lv70), 21 (30 lv77), 29 (120 lv80), 30 (121 lv80); transcendence 0; **no 35 / 125** | |
| Cut-ins | the SGM / SBH `.dds` are in March; March's table names `HQ_CutIn_Elesis_Pyro_Knight.dds` / `_Saber_Knight.dds`, which exist nowhere; `HQ_CutIn_Elesis_SCA.dds` is June-only (data044); `HQ_Cutin_EGM / EBH / ECA_Transcendence` `.gfx` already in March; June's `UnitTemplet` adds `m_fLobbyRotationY` (30 / 25 / 1 / 5 / 15 / 1 / 10) and `m_HyperCutIn` for 120 / 121 / 125 | |
| Class path | quests 35569-35572 (to 35; reward items 109999 + 293720) and 35573-35577 (to 125; 110850 + 293721); promotion items 41815-41820 (35) and 41821-41827 (125) from `UnitTemplet`; 35 uses `Motion_EL_DK.x`, 125 `Motion_EL_DK.x` + `MOTION_EL_CA.x`; both intro movies `Movie_Create_Elesis_CA.avi` (loose, already equal to June's) | 35560-35568 |
| Transcendence | `DLG_Transcendental_Change_Guide_Elesis.lua` (same size both sides) names `POPUP_TR_G_MASTER` / `_B_HEART` / `_C_AVENGER`: **none in March** (June data215, + `.TET`); `TranscendCheck` (`Handlers_Quest.cpp`) has no Elesis class | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Elesis map: class map (every thunk target and uncarved override included), state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, the Way of the Sword map, the summon map, the blood-hook map, the CH-N map (35 / 125), the motion question | Opus high | - (runs now) |
| CH0b | Every June Elesis asset, whole, first | Sonnet high, user play-test | CH0, char-ara CH6 walk |
| CH1a-CH1k | Class core transcription by state group and vslot band (12 sub-phases, below) | Sonnet high; Opus review every 2-4 phases | CH0, char-ara Z; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | the CH1a-CH1k reviews |
| CH1q | Way of the Sword: her class side and the gauge twins, flag `_WOS` | Opus high, Opus review | CH1p review |
| CH1 | Final: define `_LIVE`, adopt the script, copy any held motion koms, play-test on 8 / 29 / 30 / 120 / 121 | Opus high, Opus review, user play-test | CH1q review, CH0b |
| CH5 | UI and data: her 5 `UnitTemplet` rows June's + the 35 / 125 rows added, cut-in table, **the creation screen (Grand Master, Blazing Heart)** | Sonnet high, Opus review, user play-test | CH1 review |
| CHNa | CH-N (a): 35 / 125 load and play: her class arms, gauge create, stat rows (Open decision 1), units, **the creation screen's third line** | Opus high, Opus review, user play-test | CH5 review, char-ara CHNa review |
| CHNb | CH-N (b): the job-change route to 35 and 125 and **the level-15 / level-35 class-change dialogs** | Opus high, Opus review, user play-test | CHNa review |
| CH2a-CH2d | Skills: Knight + first jobs (8, 29, 30, 35) | Sonnet high; one Opus review per stage at CH2d; user play-test | CH1 review (CH2d: CHNb review) |
| CH3a-CH3d | Skills: second jobs (120, 121, 125), hyper actives, transcendence rows | Sonnet high (CH3d Opus); one Opus review at CH3d; user play-test | CH2d review (CH3c: CHNb review) |
| CH3b0 | Summons: the four NPC spawns and vslots 0x24c / 0x250 (the `m_fCreateEffectSetRate` writers), flag `_SUMMON` | Opus high, Opus review | CH3a; before CH3b |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives; overrides + memo; the Dark Knight / Crimson Avenger blood hooks | Opus high; one Opus review at CH4c; user play-test | CH3d review |
| CH6 | Whole-character walk and gap reruns | Sonnet medium, user walk | CH4c review, CH5 review, CHNb review |
| Z | Close-out | Sonnet medium | CH6 |

CH1-CH4 all edit `X2GUEL.*`: one at a time. CH5, CHNa and CHNb touch `UnitTemplet.lua`, `X2Unit.h`, `X2GageManager.cpp`, `X2StateCreateUnit.*`, the
emulator and the quest Lua: they may run beside a CH2 phase when the Touches columns do not overlap. If CH0's counts put a group over ~45
functions (or over ~25 where most rows are listing-only), CH0 splits it with a digit suffix (`CH1e2`) and writes the kickoff and STATUS row; it
does not merge groups. Play-test follow-ups after CH1 final get the next letters (`CH1r`).

**Review cadence (user, 2026-11-10).** CH1a-CH1k are inert transcriptions in one class file: reviewed in runs (CH1b covers CH1a-CH1b, CH1d2 covers
CH1c-CH1d2, CH1g covers CH1e-CH1g, CH1k covers CH1h-CH1k). CH1m-CH1q, CH1 final, CH5, CHNa, CHNb and CH3b0 keep their own reviews (shared
code). CH2a-CH2d have one review at CH2d, CH3a-CH3d one at CH3d, CH4a-CH4c one at CH4c: the earlier phases say "no review of its own" and keep
their `_DEBUG` define on; the stage review retires them and gives a verdict line to each incidental shared-file hunk.

## Phase CH0 - Elesis map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_elesis/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init 32b87a60 32b81be0 32b864f0 <base 31fd8f00 and the
line registrars it calls>` and `skill 32b8c470` (June's 0x7a4 `32b87a60` is a wrapper; its two parts register 111 and 57 names; `3202b110` and
`31fe22d0` are the base event states and the START / WIN / LOSE roll), then run `ch0c_listing_check.py` on it: every row must match the raw
listing; all `~` rows and std::string-named rows by hand. Map all four switches in `32b8c470` case by case (the planning resolved them by range
ends only) and the 22 id blocks of `SetEquippedSkillLevel` `32b4e790`. She has **no** second skill switch (0x68c is the base's): confirm. Ara's
builders are copied into `char_elesis/` with her range: say which. Add `elesis` to `charport_adis.py`'s `CLASS_RANGE`; confirm it reads the 234
uncarved thunk targets and the 33 uncarved overrides.

**Scope** (Ara's CH0 items, with Elesis's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32b4e770-0x32b90b6f`, **every one of the 409 thunk targets and the 33 uncarved
   overrides included**, March pair, verdict, target sub-phase, the **`inside` / `new`** column (propose the plain name for every `new` row) and a
   **`carved` / `listing-only`** column. Funcmap rows `Char Elesis CH0`. Confirm the role of each override by its body: the planning left 0x428,
   0x558, 0x670, 0x720-0x72c, 0x730, 0x83c, 0x534, 0x4ac as guesses, and the base of 0x4f8 / 0x4fc / 0x734 is the shared stub `33093ad0`.
2. **State and skill table** `ch0_state_skill.tsv`: 311 / 311 script tables, every skill id; dead-data and March-only states listed; nothing in
   `X2GUEL.*` stores a state id in a `UCHAR` (7l): confirm. Map June's tags to the jobs (`SSK` / `SGM` Saber Knight / Grand Master, `SPK` / `SBH`
   Pyro Knight / Blazing Heart, `SDK` / `SCA` Dark Knight / Crimson Avenger, `EL` Knight) and say what the 22 bare `SI_` tables and the `QUICK`
   group are and which phase each goes to; list June's ids for every state name `EL_STATE_ID` lacks or numbers differently (from the exe strings).
3. **Keys** `ch0_keys.tsv`: `EFFECT_SET_LIST_EX_NO_TRANS` / `_EX_TRANS_1` / `_EX_APPLIED_SKILL` / `EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL`,
   `RELATIVE_SKILL_LEARN_CHECK`, `IGNORE_STATE_DEFENCE`, `IGNORE_DEFENCE_DECLINE`, `WAY_OF_SWORD_TYPE`, `CREATE_EFFECT_SET` get an owner (class or
   engine; an engine reader with no March counterpart is a `CH<n>0` or a routed item); `PLAY_TYPE` / `TRANSITION` read by `32b6e0c0`; the neither
   key and the 4 march-only-site keys; `MAX_HYPER_MODE_TIME` / `HYPER_MODE_COUNT` / `MP_CHARGE_RATE` get their readers; June's
   `WayOfSword.lua` `Const_Elesis_*` keys: does June's code read them (Elsword's 14 extra keys had no reader).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md` Parts A1-A3 and
   its Part B routed items; correct funcmap l.1420's `32b45e0c` (Lu/Ciel's range) and the 0x7f4 vtable address `334fae14` -> `334fae10`).
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Elesis items incl. the 35 / 125 promotion ids
   41815-41827 and costume ids, unioned; `ch0_build_assets.py` gains `elesis`) and **effect closure** `ch0_effect_closure.tsv`
   (`charport_closure.py`, run-time-built names included; the 62 June-only effect names her code pushes are in `plan_june_side_binary.md` §6).
   **The motion kom:** name the March kom for June's data206 motions. **The motion question:** for each of the 6 differing class motions, the
   animation names March's script or class code plays that June's file lacks; if any, those files are held for CH1 final
   (`ch1_held_motions.tsv`), else CH0b copies them; the six March animations already missing (`SI_A_EL_LEAP_CRASH_*`, `SI_A_EL_RUSHING_SWORD`,
   `SI_SA_SSK_SPIRAL_BLAST`, `_VICTORIOUS_SWORD`) and the KEM 433 frame warning on her part meshes get their June files. Stage
   `POPUP_TR_G_MASTER` / `_B_HEART` / `_C_AVENGER` (+ `.TET`), `DLG_1ST` / `2ND_Class_Change_Guide_Elesis.lua`, `DLG_Class_Change_Elesis_SK` / `_PK`
   / `_GM` / `_BH` / `_DK` / `_CA` (June data217), `Class_Change_Guide_Elesis.lua`, `DLG_Unit_Create_ClassChange_Button_Elesis.lua` (owner CH5),
   `HQ_CutIn_Elesis_SCA.dds`, the four summon NPCs' scripts, meshes and templet rows (`npc_mesh_census.py`), the cut-in art CH5 needs.
6. **Unmapped helpers.** Per skill, the callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus
   `CH<n>0` sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0). CH3b0 (summons) is planned already.
7. **Way of the Sword map** `ch0_wos.tsv`: every March Way of the Sword function of the class, the gauge data and the gauge UI paired with June's
   (start from `char_elsword/ch0_wos.tsv` rows `elesis` and `shared_with_elesis=yes`, and Elsword's CH1q / CH1q2 Done notes: what June changed for
   him it changed in her twins `32a34c60`, `32e6e060`, `32e69e70`); the three gain getters 0x720 / 0x724 / 0x72c with their writers and what
   `+0x971c` scales in `32b74ed0`; the `..._DARK_KNIGHT` buff ids; `_CONST_EL_::GALE_*` against June's constants; the emulator's `CAC_WSP` answer.
   Sized for CH1q; split if over ~45 functions (CH1q2 for the gauge side).
8. **Summon map** `ch0_summon.tsv`: the five call sites with their arguments, each NPC's script / templet row / mesh presence in March, the
   `GetRelatedSkillLevel` path and the `+0x8e74` write, `CheckSummonSpecialAttack` `3202daf0`, whether June's spawn rules differ from March's
   `:3667` (count limits, owner link, position), and the whole bodies of 0x24c `32b89260` / 0x250 `32b74500`. Sized for CH3b0.
9. **Blood-hook map** `ch0_blood.tsv`: 0x4f8 / 0x4fc / 0x734 and what calls them in the base (the caller sites and arguments), `32b87880`,
   `32b734d0`, `32b7e3b0`, `32b6ff20`, `32b7e9a0`, `32b7e740`, 0x730 `32b816a0` + `32b81220` and the list at `+0x9700`, the members `+0x9748` /
   `+0x974c` / `+0x9760` named from their readers, the skills that own each. Sized for CH4c.
10. **CH-N map** `ch0_classn.tsv`: every client and emulator site with arms for 8 / 29 / 30 / 120 / 121 and none for 35 / 125 (start from
   `plan_march_side.md` §5), each marked `covered by Ara CHNa` (check what her CHNa landed under `SERV_IRUHADEV_JUNE_CLASS_PATH`; if it has not
   run, what its kickoff takes) or `Elesis`, each with June's behaviour; the quest route (35569-35577 decoded from the staged `FieldQuest.lua`,
   the reward path in `Handlers_Quest.cpp`); the `LevelUpEvent.lua` rows (level 15, level 35, transcendence); `CorrelateUnitClassTemplet`,
   `AnimationNameTable` lobby names, the creation-screen sites (`plan_march_side.md` §10); the promotion and reward items in the live `item.kim`
   (`kim.py`). Sized for CHNa / CHNb.
11. **Shared data.** Which state names the C++ enum needs; every `SI_*` id March's passive / skill code reads with its June `RNSI_*` id; her 7
   `UnitTemplet` rows June vs March (are the string ids in our string table?); the dash-jump fireball state's phase.
12. **S3 census baseline:** `s3_no_state.py --family X2GUEL.cpp --preprocess` with the 35 / 125 rows (`--append-missing`, as Ara's CH0) so CH6
   compares like with like.
13. **Amend the kickoffs** CH0b-CH5, CHNa, CHNb ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Elesis checklist row has a phase or n/a; 311 / 311
tables and every skill id mapped; every thunk target and uncarved override has a row; assets union routes a / b / c and the motion kom is named;
the motion question answered; the Way of the Sword, summon, blood-hook and CH-N maps exist; kickoffs amended; open questions listed.

## Phase CH0b - Every June Elesis asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_ELESIS_ASSET`). Sub-tag `DATA`. After char-ara CH6's walk.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (to the kom CH0 named; held ones packed and kept for CH1 final), body / face /
hair, equipment (35 / 125 promotion sets included), textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`, the summon NPCs' meshes,
the 54 emotion motions; the class-change dialogs and guides (`adopt-june`, `luabind.py check`; `LevelUpEvent.lua` is CHNb's). Stage byte-exact
(`kom.py stage-june`), pack each kom, copy each (CLAUDE.md, 2026-11-10). Save `data\ErrorLog_pre_CH0b_elesis.txt` first. Gate: `python
tools/skillicons.py check <Elesis's skill-id range from CH0>` = PASS (stage what it lists; the shared `DLG_SKILL_Passive_*` sheets included; a
June-side gap is a `JUNE_BUGS.md` row).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Elesis per March job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log (the KEM 433 frame warning gone); a costume, a weapon and three skills look right.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high | Movement and slot-head states: wait + wait-state-change, walk, jump (6), dash basics, throw item, common buff (`COMMON_BUFF_FrameMove` 0x5f0 `32b6def0`), hyper + the three job hyper states, charge, the `QUICK` group (4) |
| CH1b | Sonnet high, Opus review (covers CH1a-CH1b) | Reactions: damage (21 `ESI_DAMAGE*`), stand-up (6), die, start, win / lose / draw blocks (0x90 / 0x94 / 0x98), wall, amusement, frozen, transformed |
| CH1c | Sonnet high | Riding (27 `ESI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high | Ground combos with no job tag (`ESI_COMBO*`) |
| CH1d2 | Sonnet high, Opus review (covers CH1c-CH1d2) | Dash and dash / jump combos with no job tag (`ESI_DASH*`), incl. the dash-jump XZ fireball `32b770e0` if CH0 puts it here |
| CH1e | Sonnet high | Saber Knight / Grand Master lines (`SSK`, `SGM` combo and dash-combo states, and the non-skill states CH0 assigns to them) |
| CH1f | Sonnet high | Pyro Knight / Blazing Heart lines (`SPK`, `SBH`) |
| CH1g | Sonnet high, Opus review (covers CH1e-CH1g) | Dark Knight / Crimson Avenger lines (`SDK`, `SCA`; all June-only: every row is `new`, and every state name is a new `EL_STATE_ID` member) |
| CH1h | Sonnet high | Overrides below vslot 0x600: ctor `32b70ee0` / dtor `32b70990` offset map, Init `32b52560`, ReInit `32b81900`, InitEffect `32b4ef40`, InitComponent `32b81710` (its Way of the Sword calls under `_WOS`), DamageReact `32b73ff0` (her tutorial test `32b74065`), HyperModeBuffEffectStart 0xc4 `32b74300`, the particle arrays 0x540 / 0x548 / 0x54c, the state-id getters 0x4ac / 0x4b0 / 0x4b4 (by name, never the number), DetonationStateChange 0x4d4 `32b73e10`, 0x534 `32b90430`, 0x558 `32b6fb80`; the hit hooks 0x4f8 / 0x4fc / 0x734 only as declarations (CH4c) |
| CH1i | Sonnet high | Overrides 0x600-0x7bc: HyperModeFrameMove `32b7f090` -> `32b7b820`, CanRidingState 0x628 `32b70130`, 0x670 `32b64290`, SetEquippedSkillLevel `32b4e790` (22 id blocks); InitPassiveSkillState `32b88300`, 0x6a0 / 0x6a4, 0x29c, 0x2b8 only as declarations (CH4) |
| CH1j | Sonnet high | Common* overrides 0x7c0-0x7e8 (`32b70180`, `32b70220`, `32b724b0`, `32b88070`, `32b4edc0`; 0x7d8 / 0x7dc are tail jumps to the base) and the helpers only CommonFrameMove reaches (`32b6e050`, `32b7ac20`, the effect-set stops `32b7b5f0` / `32b7b6c0` / `32b7b760`, the debuff cleanse `32b87520`, the aimed effect mesh `32b813d0`, `32b7e580`); its Way of the Sword call under `_WOS`, its blood calls under `_BLOOD` |
| CH1k | Sonnet high, Opus review (covers CH1h-CH1k) | Skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied): SpecialAttackEventProcess `32b4ee20`, CanUseSpecialAttack `32b84a50` (its `CheckSummonSpecialAttack` call under `_SUMMON`), 0x7fc `32b52590`, 0x800 `32b73e60`, NoStateChangeActionFrameMove `32b818b0` + Endurance `32b80c60` / `32b80840`, SpecialAttackNoStageChange 0x810 `32b72550`, IsValideSkillRidingOn 0x818 `32b4ef10`, 0x828 `32b70290`, 0x834 `32b70d10` -> `32b70af0`, and **`ReservedSkillStart`** (Rules) |
| CH1m | Opus high, Opus review | Registration: June's InitState wrapper `32b87a60` and its parts `32b81be0` / `32b864f0` against March's `InitState` / `InitStateCommon` / `InitStateByUnitClass`, the hyper-mode table names (`GetHyperModeTableName` `31fdd490`), START / WIN / LOSE roll (`31fe22d0`), base event states (`3202b110`), `ESI_WAIT_STATE_CHANGE`, the June-only `EL_STATE_ID` members CH1a-CH1k declared, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks she registers (Ara's CH1n triage first: only rows earlier characters did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32b8c470` head (bounds, slot tables, the 7i rule, the pre-switches; June drops the snapshot call and sets `+0x11`: `plan_checklist_rows.md` item 14) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals she overrides (0x83c `32b732e0`, 0x670, the rest CH0 gives this phase); `SetSkillLevelStateData` if she has one in June; gaps the CH1a-CH1k reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (`UT_ELESIS` joins June's set at `X2GUUser.cpp:26778-26797`; her decode, `CountUsedSkill` `X2GUEL.cpp:3157`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here (the four `EFFECT_SET_LIST_*` variants, `RELATIVE_SKILL_LEARN_CHECK`, `IGNORE_STATE_DEFENCE`, `IGNORE_DEFENCE_DECLINE`); the reserved-buff path (7ai, checklist l.870-871) checked for her ids |
| CH1q | Opus high, Opus review | Way of the Sword (`ch0_wos.tsv`): June's bodies inside every March function of the class that differs (Start / End Vigor, Start / End Destruction with the Dark Knight buff ids, `WayOfSwordStateChange` `32b7f370`, the state-end hooks, `GetFinalDamage_WayOfSword` `32b74ed0`, `SetCriticalPercent_WayOfSword` `32b4ef90`, the gain getters 0x720 / 0x724 / 0x72c replacing the two stand-ins), `CX2ElesisGageData::InitWhenGameIsOver` `32a34c60`, `CX2ElesisMyGageUI` `InitUI` `32e6e060` and `RefreshWayOfSwordUI` `32e69e70`, the snapshot pair 0x740 / 0x744 as `StoreGageSnapshot` / `RestoreGageSnapshot` (the Crest bases stay unported, as Elsword's); divergence rows 417 / 420 / 421 / 424 / 427 settled; sub-tag `WOS` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE` (+ `_WOS`); adopt `ELESIS_KNIGHT.lua`; copy CH0b's held motion koms (if any); pack, copy; the play-test on 8 / 29 / 30 / 120 / 121, incl. the `[JUNE_SKILL] SBUF` lines of the reserved-buff path (Endurance Power, Flame Sword) |

**Transcription rules (CH1a-CH1k):** as Ara's (`JUNE_CHARACTER_ARA_PHASES.md`, "Phase CH1 - Deploy unit: June script, motions and the class
core", "Transcription rules"), with Elesis's flags and her class range `0x32b4e770`-`0x32b90b6f`: `inside` rows inside March's function under
`SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` with March's in `#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CORE`; a
later group's callee declared with `// June <va>: body in <phase>`; an unmapped engine callee is "this needs Opus" (stop). June's numbers and
quirks verbatim; `m_fNowStatePowerRate` (7i) with June's operator; June's BFI arguments (7ai). **Way of the Sword, summon and blood lines inside a
function a CH1a-CH1k phase transcribes** are written under `_WOS` / `_SUMMON` / `_BLOOD` (an `#ifdef` inside the `_LIVE` body) when they call a June
piece CH1q / CH3b0 / CH4c has not written yet; March's live Way of the Sword calls stay March's until CH1q.

**Done gate (CH1 final):** Elesis (8 / 29 / 30 / 120 / 121) loads June's script and motions; `[JUNE_CHAR] ELESIS DATA` logs the load; no new
`KEM_ERROR` / `Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, damage and riding play June's animations; the Way
of the Sword gauge fills both ways, vigor and destruction start and end as June, and the state survives a stage change; each skill cast twice
shows its cool time and spends MP; `charport_scan.py` / `charport_keys.py ELESIS_KNIGHT` show nothing new.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, the creation screen

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ELESIS_UI`. Sub-tag `UI`.

**Scope:** her 5 `UnitTemplet.lua` rows become June's and June's rows for 35 and 125 are added (BOM source, `luasrc.py refresh` after; every other
row stays; if CH0 found the templet loader refuses a class >= `UC_END`, the 125 row waits for CHNa): `m_fLobbyRotationY`, `m_HyperCutIn` for
120 / 121 / 125, the names; the cut-in table and the InitState index switch (`X2GUEL.cpp:249-273`) replaced by June's names and layout (7t
cut-in `.gfx` names; March's two missing `.dds` names go); 7ah; the lobby particle hard-codes (`X2UnitViewerUI.cpp:630` / `:2414` / `:2530`)
retired where June's `AnimationNameTable` row plays the same particles (Ara's decision 2 is the precedent: under the unit viewer's flag, March's
code in `#else`). **Creation screen (user, 2026-11-10 and 2026-11-11: a char-port item, not Stage 9).** Adopt June's
`DLG_Unit_Create_ClassChange_Button_Elesis.lua` (six buttons; `adopt-june`, `luabind.py check`) and port every June function the creation state
and unit viewer run for Elesis (class-preview switch, class buttons, `Movie_Create_Elesis_*` previews, the stat chart `X2StateCreateUnit.cpp:3870-3885`),
listed from June first: **Grand Master and Blazing Heart get their entries here**. A click on a third-line button before CHNa must be harmless
(check `SelectUpperClass`; if it is not, the third-line buttons are hidden under `_UI` until CHNa).

**Done gate:** a select-screen pass of all nine March characters with no new error; her skill and hyper cut-ins play June's art; the second-job
names show June's strings; on the creation screen Elesis shows Saber Knight, Grand Master, Pyro Knight and Blazing Heart, each with its preview.

## Phase CH-N - Elesis's June-only class path: Dark Knight (35) and Crimson Avenger (125)

The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N" as Ara's Z rewrites it; Ara ran it first. **What Elesis reuses** (Ara's CHNa,
flag `SERV_IRUHADEV_JUNE_CLASS_PATH`): the socket-loader bound, `GetClassLevel`, the optional log sites, the June `FieldQuest.lua`, the quest
reward path that accepts any class. **What is hers** (flag `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CLASS`): everything in `ch0_classn.tsv` marked
`Elesis`. Sub-tag `CLASS`.

| Sub-phase | Scope |
|---|---|
| CHNa | **The class loads and plays** (`ch0_classn.tsv`): **the gauge create first** (`X2GageManager.cpp:194-209` gets the 35 / 125 arms: without them her class casts a plain gauge to `CX2ElesisGageData`); `GetX2UnitClassCompatibility` arms (`X2Unit.h:2057-2123`, June `31ed6500`, divergence row 365); the third-line arms of `GetFirstLine...` / `GetSecondLine...ClassTypeByUnitClassType` and their callers (`X2Unit.h:2685-2750`); the class switches inside `X2GUEL.cpp` (`:213`, `:249`, `:411`, `:2747`, `:9581`); `X2Data.cpp:2872-2892`, `X2ComboTree.cpp:302`; `tools/createchar.py` (`CLASSES` / `PARENT`); **stat rows** for 35 / 120 / 121 / 125 per Open decision 1 (server Lua `KncWX2Server/ServerResource/US/StatTable.lua`: the user packs it; until then the loader's type fallback, now logged); `Motion_EL_DK.x` / `MOTION_EL_CA.x` reach the unit; **the creation screen's third line** (Dark Knight, Crimson Avenger: helper arms, stat-chart arms, previews, `Movie_Create_Elesis_CA`); the units: `createchar.py` a 35 (level 20), a 125 (level 40) and a level-80 125. Play-test: both load in the village and a dungeon, combos and the Way of the Sword gauge work, the job name shows, a socketed weapon keeps its socket, the creation screen shows all six entries. |
| CHNb | **The job-change route and the class-change dialogs** (`ch0_classn.tsv`): June's quests 35569-35572 (to 35) and 35573-35577 (to 125) with their rewards through `Handlers_Quest.cpp`; **`LevelUpEvent.lua`'s level-15 row opens `DLG_1ST_Class_Change_Guide_Elesis.lua` (three paths) and its level-35 row adds 35 and opens `DLG_2ND_Class_Change_Guide_Elesis.lua`** (core-list rule if it must be edited); `DLG_Class_Change_Elesis_DK` / `_CA` and June's copies of `_SK` / `_PK` / `_GM` / `_BH` (staged by CH0b); `X2OfflineClassChangeSeed.h` parent rows only if June's cash route needs them (no Elesis row today means "do not refuse"); `luabind.py check` on each dialog. Play-test: a level-15 Knight sees the three-path dialog, takes 35569-35572 (`/qdone`) and becomes Dark Knight; a level-35 Dark Knight sees the second dialog, takes 35573-35577 and becomes Crimson Avenger; a second level-15 Knight still reaches Saber Knight and Pyro Knight from the same dialog; all survive a relog. |

**Done gate:** a 35 and a 125 reached both by job change and by `createchar.py` load, fight and relog; the gauge, socket and stat paths log no
refusal; no other class changed (a select-screen pass of all nine March characters).

## Phase CH2 - Skills: Knight and the first jobs (classes 8, 29, 30, 35)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), one Opus review at CH2d (covers CH2a-CH2d), user play-test per phase. Flag
`SERV_IRUHADEV_JUNE_CHAR_ELESIS_SKILL`. Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 8 has it -> CH2a; else the first of 29 / 30 / 35.

| Sub-phase | Scope |
|---|---|
| CH2a | Knight (8) skills (15 rows; Endurance Power `32b80c60` and its memo, Sonic Assault's memo states) |
| CH2b | Saber Knight (29) skills |
| CH2c | Pyro Knight (30) skills |
| CH2d | Dark Knight (35) skills (after CHNb: tested on a 35 unit; the bare `SI_*_SDK_*` script tables); CH2 stage review |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its block in `SetEquippedSkillLevel` `32b4e790`
if it has one; its attached rows (7u names, 7ai BFI arguments, 7h UPDOWN cancels, the power-rate operator, the numerical-only tables of
`plan_numerical_rows.tsv`); its effect closure (a name found only now is staged and added to `ch0_assets.tsv`); its `WAY_OF_SWORD_TYPE` on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A skill whose factor names one of the three BFI ids June's `Enum.lua` lacks (`JUNE_BUGS` 279) keeps June's nil, with the row cited.
`NUM use` lines for her skills; March vs June damage in `MODS.md`.

**Done gate:** every class-8 / 29 / 30 / 35 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot casts
June's state, motion and effect, deals damage, shows its cool time and spends MP; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (120, 121, 125), summons, hyper actives, transcendence

**Run with:** as CH2 (CH3b0 and CH3d on Opus); one Opus review at CH3d (covers CH3a-CH3d; CH3b0 keeps its own). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Grand Master (120) skills (the Dignity aura `32b7e740` with BFI 910 passing `x`, checklist 7ai) |
| CH3b0 | **Opus.** Summons (`ch0_summon.tsv`), flag `_SUMMON`: vslots 0x24c `32b89260` / 0x250 `32b74500` whole (the `m_fCreateEffectSetRate` writers `@32b8930d` / `@32b7471b` and the Fire Blossom spawn `@32b746e8`), `32b79db0` (Fire Blossom), `32b7a0a0` (Wall of Prominence 1635 / Fire King 1971), the spawn call of `32b8b0c0` (Blood Cutter 1641; the state body is CH3c's), `CheckSummonSpecialAttack` `3202daf0`, the `+0x8e74` related-skill-level write, the three NPC enum names, `luabind.py check` on the four NPC scripts and their templet rows; own review |
| CH3b | Blazing Heart (121) skills (Wall of Prominence and Fire Blossoms on CH3b0's spawns; Soul Ignition's effect-set rate) |
| CH3c | Crimson Avenger (125) skills (after CHNb; Blood Cutter on CH3b0's spawn; the bare `SI_*_SCA_*` tables) |
| CH3d | **Opus.** Hyper actives (type 13; the 28601.. switch in `32b8c470`), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): 120 / 121 / 125 on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`), her `LevelUpEvent.lua` transcendence row + `DLG_Transcendental_Change_Guide_Elesis.lua`; checklist 7ao whole (`HyperModeBuffEffectStart` `32b74300` for a transcended unit; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); her hyper-class skills (`SPRT_HYPER_CLASS`: 28416 / 28417 `RNSI_P_SBH_TRANSCENDENCE` and the SGM / SCA ones) and `TRANSCENDENTAL_SKILL_DATA` rows; the `EFFECT_SET_LIST_EX_NO_TRANS` / `_TRANS_1` keys' transcended side; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_EGM / EBH / ECA_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and her transcended pictures, read from June's row pushes; `POPUP_TR_G_MASTER` / `_B_HEART` / `_C_AVENGER` present; CH3 stage review |

**Done gate:** as CH2 for 120 / 121 / 125 and the hyper actives; each summon appears, acts and leaves as June's script says (the hand-off lists the
`ES_CREATE` and NPC state lines); a transcended Elesis of each second job (`settranscend`) casts her hyper-class skills, slots A-5 / B-5 work, the
transcended picture and cut-in show; `python tools/skillicons.py check <her skill-id range>` = PASS after the transcendence rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, one Opus review at CH4c (covers CH4a-CH4c), user play-test (three engine pieces, planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32b88300` (vslot 0x690) whole on June's ids, her passive ids, their objects and readers, its 7 attack-result delegate pushes replacing March's own (checklist l.920 / l.930); the June-only hook 0x83c `32b732e0`; 0x29c `32b6fc40` (the `GetReduceDebuffDurationRate` shape); the class float writer `32b73bf0` / reader `32b7d550` (l.496); the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | Overrides and memo: `SetDamageExceptionProcess` 0x2b8 `32b80f20` whole (the June-only 28416 / 28417 part `@32b81008-32b811ef`), `GetActualMPConsume` 0x6a0 `32b7ebe0` and `GetActualCoolTime` 0x6a4 `32b7ee10` with the memo reader `31fca9e0`, 0x428 `32b52260` (l.942), 0x558 `32b6fb80`; `MemoConditionElesis` (vtable `334fa4f4`, `32b51980` / `32b519a0`) and the four `MEMO` states' readers; what Elsword's CH4b took for the shared Way of the Sword leftovers (`CureExtraDamage`, `ResetAnimSpeed`'s vigor factor, `SAT_WAY_OF_SWORD_GAUGE_UP`) checked in her class |
| CH4c | **Blood hooks** (`ch0_blood.tsv`), flag `_BLOOD`: the hit hooks 0x4f8 `32b7beb0`, 0x4fc `32b898a0`, 0x734 `32b7bad0` and their base call sites (the hooks are inert stubs today, l.454-455), Blood Runaway `32b87880` (BFI 462 registered with it, BTI 426 erased above 4 stacks) and `32b734d0`, `32b7e3b0`, Madness `32b6ff20`, the transcendence berserker chain `32b7e9a0` (BFI 913 passing `x`), the per-frame list 0x730 `32b816a0` / `32b81220`; tested on 35 / 125 units; CH4 stage review |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); blood sucking heals and bleeding ticks on a Dark Knight;
0 Elesis `no_state` rows left except passives with no state in June.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Elesis per class (8 / 29 / 30 / 35 / 120 / 121 / 125), one transcended
per second job. Walk: every skill in an A and a B slot (each cast twice: cool time and MP), a hyper, the Way of the Sword (fill both ways, vigor,
destruction, a stage change), each summon, blood sucking and Blood Runaway on 35 / 125, the passives, one dungeon per job, both job changes to
35 / 125 and one to 29 from the level-15 dialog (CHNb's quests on fresh units), the creation screen's six entries, then **leave the game and return
to the village**. Re-run `census.py`, `charport_scan.py`, `charport_keys.py ELESIS_KNIGHT` and `s3_no_state.py`; before / after numbers in the Done
note. **The becoming-transcended transition on un-transcended units (user rule, 2026-10-28):** one unit per second-job class (120, 121, 125) by
`/tc 1`, quest 42030 (`/qdone 42030`) and the book 288493 (`/additem 288493 1`), each showing the guide popup with that class's picture, the new
skill-tree rows, the HUD portrait, and surviving a relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Elesis" section (flags, files, data, revert order: exe + data036 + motion koms
together; `SERV_IRUHADEV_JUNE_EL_DASHJUMP_XZ_FIREBALL` recorded as superseded; any user-packed `StatTable.lua` rows); the stale "flag off" comments
about `SERV_NEW_CHARACTER_EL` corrected if a phase touched those files; no `_DEBUG` flag left defined; checklist rows marked `(Elesis: CHn)`; the
Aisha plan's "Template for Stage 7" gets an Elesis paragraph (what she took against this plan; what CH-N's second run took); backlog Stage 7 note;
the `stage7-plan` row points at the next character (`char-add`).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR ELESIS <sub>"`.

## Open decisions (for the user; asked at the end of planning)

1. **Stat rows for Dark Knight (35) and Crimson Avenger (125).** Ara's answer (2026-11-10) followed Ara's own rows, where her second jobs had a
   level 1 of their own. Elesis's evidence is different: `StatTable.lua` has rows for 8 / 29 / 30 only, and **Grand Master and Blazing Heart have
   none**: they play on class 8's rows through the loader's type fallback today. **(a, recommended)** leave it so: 35 and 125 take the same
   fallback as 120 / 121 (no server Lua to write or pack; CHNa only adds the missing log line), and June's whole stat calculation stays Stage 33's.
   **(b)** Ara's rule for all three second jobs: write rows for 120, 121 and 125 with level 1 = 0.8 x June's relative row (Grand Master
   `400, 320, 89.6, 72, 12250`; Blazing Heart `392, 344, 88, 73.6, 11750`; Crimson Avenger `408, 328, 81.6, 77.6, 12000`, in the column order Ara's
   118 uses: CHNa checks it) and levels 81-99 from `cap99_gen.py`, as a labelled stand-in; you pack `StatTable.lua`. It changes Grand Master's and
   Blazing Heart's numbers too. **(c)** rows for 125 only: Crimson Avenger alone would differ from her sister classes.
   **Answered (user, 2026-11-11): (b), Ara's rule for all three second jobs.** CHNa writes the rows as a labelled stand-in: 35 = class 8's 99 rows
   verbatim (as 29 / 30); 120, 121 and 125 = class 8's rows with level 1 = 0.8 x June's relative row and the top levels regenerated by
   `cap99_gen.py` (check the column order and the regenerated level range against Ara's 118 / 119 rows first); the user packs `StatTable.lua`.
   Grand Master's and Blazing Heart's numbers change with it: `MODS.md` records before / after.

Settled by standing rules, not decisions: the motion koms deploy with CH1 final if June drops a name March plays (7ad); Ghidra stays read-only
and listing-only functions are read with `charport_adis.py`; the lobby particle hard-codes are retired where June's table plays them (Ara's
decision 2); June-side icon gaps and the three nil BFI names are `JUNE_BUGS.md` rows; the 54 emotion motions are staged, their behaviour is
Stage 10; third jobs 171-173 are out of scope.
