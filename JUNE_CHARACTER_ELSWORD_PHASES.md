# June character port: Elsword (Stage 7, third character after the pilot)

Plan key `char-elsword` · branch `main` · kickoffs `kickoffs/char-elsword/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] ELSWORD <sub>`,
sub-tags `DATA`, `CORE`, `WOS`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_*` for Elsword's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-03 by the `new-plan char-elsword` session from
the Aisha plan's "Template for Stage 7" and the Chung plan (`JUNE_CHARACTER_CHUNG_PHASES.md`), the most recent worked example.

## Context

**What this plan does.** It moves Elsword (unit type `UT_ELSWORD` 1, script `Elsword_SwordMan.lua`, class `CX2GUElsword_SwordMan` in
`X2Lib/X2GUElsword_SwordMan.cpp` / `.h`, seven classes: Sword Man 1, Sword Knight 10, Magic Knight 11, Sheath Knight 22, Lord Knight 100, Rune
Slayer 101, Infinity Sword 112) from his March kit to his June 2017 kit, end to end: June script, skill states for June's tree, numerical damage,
motions, class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), the Way of the Sword gauge, UI pieces, data and his
transcendence rows.

**Why Elsword is third (stage7-plan, 2026-10-22).** Gap 20, no templet-backed June-only class, no open design decision. **One planning premise
was wrong:** the brief's "`DAMAGE_SKILL_ID` x103 in his damage data is a large table job" does not hold. The 103 rows of
`Elsword_DamageData.lua` are 46 Rose skill ids (31011-31607, `RNSI_*_RHB` / `RSR` / `RVK` / `RFY` / `RMH` / `ROM` in June's `Enum.src.lua`); the
file is a shared table whose loader is already June's (effect completion C9, `SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE`, `X2DamageManager.cpp:183`).
Elsword's script uses no `DAMAGE_DATA_KEY_NAME`. His real damage gap is the 20 numerical-only tables (§1), as for any character.

**His size.** June's class spans `0x32cf2da0`-`0x32d2763f` (Arme before it; nothing of his after `32d27620`): **308** carved functions plus
**25 uncarved vtable overrides** (code present, no Ghidra function: SpecialAttackEventProcess `32cf3290`, hyper effect start `32d17ea0`, ReInit
`32d1a920`, ...; list in `plan_june_side.md`), against Chung's 441 and Aisha's 426. June's script has **329** `ESSI_*` tables (Chung 385).
Expect about 0.75 of Chung (134 commits). The phase table names 16 CH1 sub-phases up front; Chung planned 20 and ran 33.

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28), as Chung.** June's state names are March's (`ESSI_*`), so most June
bodies have a March function of the same name: **the body goes inside that function under `_LIVE`, March's in `#else`**, and each CH1 sub-phase
proves it with a one-file probe compile that defines `_LIVE`. A June function March's class lacks is written under `_CORE` with a plain name in
the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** Third jobs (`UC_150`-`UC_186`) are out: June's `UnitTemplet` has a `UC_150` Elsword
row, it stays out. Transcendence is live (Stage 6), so his transcendence rows are in CH3d and CH6.

**Order (user, 2026-11-03).** Lu/Ciel (Stage 8) is now lower priority and waits until the March characters are ported (Stage 7 closed),
replacing the 2026-11-02 order that put Lu/Ciel's code ahead of `char-elsword`. This plan's CH0 (read-only) and CH0b (assets) run now; every
code phase waits for char-chung Z (one class port's code at a time in the shared `X2GUUser.*` / `X2Game.cpp`).

**The state he starts from.**
- **Skill system (Stage 3).** `s3_reform_ids_no_state.tsv` has 261 Elsword rows (1: 15; 10 / 11 / 22: 29 each; 100 / 101 / 112: 53 each),
  127 distinct ids (106, 900, 21000-21060, 21200-21236, 21400-21420, 21600-21605); **254 have no class state** (only 900 has one). **Every row
  has a June `SKILL_ID` name** (7ae: no unnamed-id task, unlike Chung's 49).
- **Effect engine (June's).** `DamageEffect.lua` / `EffectSet.lua` ship merged; 2 Elsword rows in `c11a_march_only_templets.tsv`
  (`LORD_KNIGHT_SAND_STORM`, `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER`); **29 alpha-tag sites** (7c, `c10a_alpha_sites.tsv` l.236-264: 19 mesh,
  8 particle ptr, 2 particle handle, in 14 functions).
- **Motions.** June 71 `Motion_Elsword*` (data199 70 + data012), March 18 (data033 17 + data012); 11 shipped files differ,
  `Motion_Elsword_SwordMan.x` is already June's; 53 emotion files are June-only. `Motion_Elsword_IS.x` loses 1 animation name (unnamed in
  `s2_model_core.tsv`, no `r12b` row): CH0 settles it (the motion question). **June's motions live in June's `data199`, but March's game dir
  already has a different `data199.kom` (629 KB)**: CH0 decides where they are staged.
- **Way of the Sword (live in March, shared with Elesis).** `ELSWORD_WAY_OF_SWORD`: `CX2ElswordGageData` (`X2GageData.h:722-778`),
  `CX2ElswordMyGageUI` (`X2GageUI.h:444`, `.cpp:2591-3021`), `X2GageManager.h:791-798`, `CalcWayOfSwordPoint` / `CalcVigorMpUpOnAttack` and
  `m_WayOfSwordConstTable` (`X2DamageManager`, called from `X2DamageEffect.cpp:5505-5519`), ~17 class functions (`:409-529`, `:7182-7480`), the
  state carried in `syncData.m_CannonBallCount`; `WayOfSword.lua`, `DLG_Elsword_WayOfSword.lua`. June's script has `WAY_OF_SWORD_TYPE` x102.
  Phase CH1q, as Chung's cannon balls.
- **Hooks waiting on the class port.** Checklist: 29 Elsword rows (`june2017_re/char_elsword/plan_checklist_rows.md`). Elsword extras: 7j Rune
  Slayer magic reflect (engine branch `320462a0` @`32046a55`, class `32d0c820` Magic Coating 21418 transcended; divergent row
  `june_march_divergent.tsv:220`); 7b June-only `SetDamageExceptionProcess` `32d1e0e0` (21419 / 21420); 7ag his own 4+4 decode
  (`X2GUElsword_SwordMan.cpp:5232-5249`, `CountUsedSkill` `:5152`, `GetSkillSlotIndexUsed` `:12896`; `X2GUUser.cpp:26532` lists him unported);
  7h 25 UPDOWN-cancel functions; 7am 8 delegate callbacks in `32d1c560`; 7a modifier tag 19 (`32d18230`); death-prevention snapshot override
  `32d058a0` (vslot 0x740).
- **Summons: none.** No NPC key in June's script, no spawn call in June's class range, 0 `CreateNPC` / `NUI_*` in March's class. No summon phase.

**Out of scope (owned elsewhere).** New characters (Stage 8); UI that is not Elsword's own (Stage 9); the class-change guides and creation-screen
buttons (Stage 9); tutorials / `Tutorial_Elsword_*` and the tutorial dungeons (june-dungeons); `ELSWORD_NEW_BEGINNING` (a beginning-game / event
scene system, not his class; OFF, `InHouse3.h:486`, confirmed by effect C3p-2); NPC / shadow / bot copies of Elsword (whichever plan adopted
June's NPC Lua); emotion motions' behaviour (Stage 10; CH0b stages the files); `FORCE_DYNAMIC_CAMERA_INDEX` / `RETAIN_CAMERA_STATE` (Stage 9);
Elesis's own Way of the Sword class code (char-elesis; CH1q ports the shared pieces whole); Crest of Time's cast (char-add).

## Rules

The Chung plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules"`), with Elsword's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim
  with a `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in Elsword's skill-use check.
- **The checklist is the work list.** Every Elsword row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase
  that ports a row appends `(Elsword: CHn)` to it in place.
- **One deploy unit.** June's `Elsword_SwordMan.lua`, his June motions (held for CH1 if CH0 finds a dropped animation March plays) and the C++
  that reads them go live together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`//#define` in `Always.h` until then).
  Between phases Elsword stays playable on his March kit.
- **Flags:** `_CORE` (June functions March lacks, compiled, unreferenced), `_LIVE` (June bodies inside March's functions, the registration, the
  switch), `_WOS` (Way of the Sword), `_SKILL`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a temporary
  `_DEBUG` per sub-phase, retired in its review. New namespaces are `_CONST_ELSWORD_<phase>_`.
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name. `python tools/junediverge.py --list`
  before transcribing; `--check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h <other files>` = `RESULT: PASS` at every finish.
- **Shared `CX2GUUser` helpers, never copied into the class:** `GetSkillLevel` / `GetLearnedSkillAbilityValue`
  (`SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); the seven skill-use helpers and their fields (`SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`);
  Aisha's, Eve's and Chung's `CX2GUUser` bases (Chung's CH1n2 already replaced the 16 non-riding base callbacks for every class): a phase that
  needs one under its own flag widens the `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp` stays 0.
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces only Elsword's 7 rows. His class script and
  `WayOfSword.lua` / `DLG_Elsword_WayOfSword.lua` are adopted whole (`adopt-june`, bytecode unless a phase must edit one); every one passes
  `python tools/luabind.py check <NAME>`.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack yourself, ask before every copy.
- **CP949.** `X2GUElsword_SwordMan.cpp` (mixed endings: 19,270 CRLF, 145 LF), `.h` (mostly LF), `X2GUUser.*`, `X2GageData.*`, `X2GageUI.*`,
  `X2GageManager.*`, `X2DamageManager.*` are CP949: `bytepatch.py` only; `cl /P` decides every `#ifdef` branch (`ELSWORD_SHEATH_KNIGHT`,
  `SERV_ELSWORD_INFINITY_SWORD`, `ELSWORD_WAY_OF_SWORD`, `ELSWORD_SECOND_CLASS_CHANGE`, `UPGRADE_SKILL_SYSTEM_2013`, `BALANCE_PATCH_20131107`,
  `BALANCE_RUNE_SLAYER_20130214`, and the region-only `SPECIAL_ABILITY_WAY_OF_SWORD` / `GET_ELSWORD_FOCUS` / `NO_GOOD_ELSWORD_INT`).
- **Uncarved overrides.** Ghidra stays read-only: the 25 uncarved functions are read from the raw listing (`charport_adis.py elsword <va>`, as
  Eve's uncarved thunks), never by creating functions.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** No schema change expected; a phase that needs one snapshots `db_backup/` first. Play-test saves never slot a skill in slot 4 / 9
  before transcendence.
- **One code phase at a time in `X2GUElsword_SwordMan.*`** (CH1-CH4). CH5 may run beside CH2-CH4.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "<heading>"` |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Elsword's rows with line numbers: `june2017_re/char_elsword/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_elsword/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools (take a character, or copy with the range changed) | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (`CLASS_RANGE`: CH0 adds `elsword`), `charport_regsim.py`, `s3_no_state.py`; `char_chung/ch0_*.py` and `char_eve/ch0_build_*.py`, `ch0_unmapped_callees.py`, `ch0_live_lines.py`, `ch1m_regsim.py` / `ch1m_regcheck.py`; `char_arme/ch0c_listing_check.py` |
| June Lua (reading) | `june_src_fixed/data036_kom/Elsword_SwordMan.src.lua`, `WayOfSword.src.lua`, `DLG_Elsword_WayOfSword.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`; `data034_kom/DLG_Transcendental_Change_Guide_Elsowrd.src.lua` (June's spelling) |
| Ghidra | `ghidra` skill; Elsword's range `0x32cf2da0-0x32d2763f`, vtable `33522914` (544 slots, 58 overrides, all in range), RTTI `3373d9ec`, ctor `32d09490` (vtable write `32d094e4`), dtor `32d1a5d0`; read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow; revert the regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` after), `python charport_keys.py Elsword_SwordMan` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Elsword_SwordMan.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR ELSWORD"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-03)

Kept in `june2017_re/char_elsword/` (digests by Sonnet subagents, spot-checked: the ctor writes vtable `33522914` at `32d094e4`;
`Elsword_DamageData.src.lua` has 103 `DAMAGE_SKILL_ID` rows, 31011 = `RNSI_SA_RHB_HARD_FLAMETHROWER`; `32d223e0` decompiles to a ~100-case skill
switch; CH0 re-verifies anything it builds on): `plan_charport_scan_20261103.txt`, `plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`,
`plan_june_side.md`, `plan_numerical_rows.tsv` (103 rows), `plan_checklist_rows.md`.

Scans re-run 2026-11-03: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 652, june-ignores 305, handed-off 9; regenerated
`census.tsv` / matrix reverted); `charport_scan.py` (scratch `C:/tmp/cps_els`, output identical to Chung's 2026-10-31 run):
`Elsword_SwordMan {'no NUMERICAL_DATA_INDEX': 87, 'no DAMAGE table': 20}` (unchanged since 2026-10-20), no Elsword ENUM or KEYS line, no
`Elsword_DamageData` line (its key census: 107 tables, 9 keys, all `both`).

| Measure | Elsword | (Chung) |
|---|---|---|
| June class range | `32cf2da0`-`32d2763f`, 308 carved + 25 uncarved overrides; 58 vtable overrides, all in range | 441 carved, 72 overrides |
| June skill-tree rows | 261 over 7 classes; 127 distinct ids; 0 unnamed | 261 / 127 / 49 unnamed |
| Rows with no class state | **254** | 254 |
| Rows by June skill type | 6: 79, 14: 49, 16: 49, 15: 41, 8: 25, 12: 7, 13 (hyper): 6, 1: 3, 2: 2 (Magic Knight and Rune Slayer only) | |
| June script | `Elsword_SwordMan.src.lua` 18,112 lines, data only: 329 `ESSI_*` + 6 `INIT_*` (no `INIT_PERSONAL_INFO`, no `PROMOTION_ITEM_ID_LIST`), no dead `XXX_` table (groups SI 41, RIDING 27, SA 27, DAMAGE 23, ERS 18, DASH 15, ETK 14, ELK 13, A 12, EMK 11, EIS 11, SI1 11, ESK 10, COMBO 9, STANDUP 8, MACHINE 7, then small groups) | 385 |
| Script keys | `DAMAGE_DATA` 90 (+16 `_NEXT*`; **20 numerical-only**, 87 states with no `NUMERICAL_DATA_INDEX`), `AT_COMMAND` 67, `APPLIED_SKILL_ID` 25, `TIME_STOP_IMMUNE_STATE` 3, `SHOW_HYPER_CUT_IN_TIME` 3, `SHOW_SKILL_CUT_IN_TIME` 35; class keys `WAY_OF_SWORD_TYPE` x102, `PHANTOM_BLADE_*` x26 | 79 / 53 / 24 |
| Key census (`charport_keys.py Elsword_SwordMan`) | 280 keys: june-only **5**: `FORCE_DYNAMIC_CAMERA_INDEX` x15 / `RETAIN_CAMERA_STATE` x1 (reader `32057b80`, Stage 9), `BBT_CHANGE_NOW_HP_PER_SECOND` / `BFT_TIME` x3 (damage data, no reader), `IMMUNITY_EDT_LIST_AT_THIS_STATE` x4 (no reader, as Eve / Chung); neither 5 (`HEAD_IK`, `HEAD_IK_MAX_ANGLE`, `PASSIVE_SPEED`, `STOP_RESERVE_TIME_ATT` / `_DEF`); march-only-site 8 (`AFTER_IMAGE`, `BUFF_FACTOR_ID_MEMO`, `BUFF_FACTOR_RELATIVE_MEMO_ID`, `LINK_JUMP_ACCEL_X/Y/Z`, `LINK_JUMP_TIME`, `READY_SOUND`); both 262 | 249: 11 / 8 / 7 / 223 |
| Numerical blocks | 103 `c1a` rows keyed to 51 ids; 101 `ok`, 2 `index out of range: June writes a zero row` (21006 c1a l.731, 21013 l.1274) | 145, all ok |
| Motions | June 71 / March 18; 11 class motions differ; IS -1 animation name (unnamed); MK / RS change 21 of 67 named poses | 70 / 17 |
| Alpha-tag sites (7c) | **29** in 14 functions (`32d14100` x9, `32d10df0` x7) | 0 |
| March class | `.cpp` 19,415 lines, `.h` 1,652; cut-in table `:14-44`, ctor `:96`, dtor `:294`, DamageReact `:566`, InitState `:866` (cut-in switch `:873-898`), InitStateCommon `:1325-2489`, InitPassiveSkillState `:2490`, `SetEquippedSkillFuncBySkillSlotIndex` `:2886-4124` (53 `case SI_*`), SetEquippedSkillLevel `:4126`, InitComponent `:4436`, CommonFrameMove `:4808`, SpecialAttackEventProcess `:4965-5275` | 24,461 |
| June class | InitState `32d23ff0` (0x7a4), `SetEquippedSkillFuncBySkillSlotIndex` `32d223e0` (0x7b4; 94 labels / 79 `ESSI_*` + a 5-label hyper switch 0x5461-0x5465), InitComponent `32d0d3f0`, InitPassiveSkillState `32d1c560` (0x690), `SetDamageExceptionProcess` `32d1e0e0` (0x2b8, **June-only**), SpecialAttackEventProcess `32cf3290` (0x7f4, uncarved; parts `32d1bd10`, `32cf5760`, `32d0d8e0`), hyper effect start `32d17ea0` (uncarved), `MemoConditionElsword` (RTTI `3373d9b0`); berserk-by-chance 0xa4 not overridden | |
| Stat rows | `StatTable.lua` 1-99 for all 7 classes, no duplicate, no gap | 110 / 111 duplicated |
| Save | units 25 jtLordKnight (100 lv99), 26 jtRuneSlayer (101 lv80), 32 MagicKnightT (11 lv70), 33 SheathKnightT (22 lv70), 34 InfinitySwordT (112 lv70), 39 Tut14c (1 lv5); all transcendence 0; no Sword Knight (10) | |
| Cut-ins | 4 sets: `HQ_CutIn_Elsword_1/2/3.dds` for 1 / 10 / 11 / 22, `_LK_1`, `_RS_1`, `_IS_1`; June's `UnitTemplet` adds `m_HyperCutIn` `HQ_Cutin_ELK/ERS/EIS_Transcendence` (no `m_BerserkCutIn`), `m_fLobbyRotationY` on all 7 rows, names `STR_ID_614` -> 4641 / 4642 / 17425 for 100 / 101 / 112 | |
| Transcendence | `DLG_Transcendental_Change_Guide_Elsowrd.lua` (data034; March ships a same-name file), `POPUP_TR_L_KNIGHT` / `_R_SLAYER` / `_I_SWORD.tga` | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Elsword map: class map (uncarved overrides included), state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, Way of the Sword map, the motion question, the `data199` clash | Opus high | - |
| CH0b | Every June Elsword asset, whole, first | Sonnet high, user play-test | CH0 |
| CH1a-CH1k | Class core transcription by state group and vslot band (11 sub-phases, below) | Sonnet high, Opus review each | CH0, char-chung Z; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | CH1a-CH1k reviews |
| CH1q | Way of the Sword: June's gauge, sync and UI, flag `_WOS` | Opus high, Opus review | CH1p review |
| CH1 | Final: define `_LIVE`, adopt the scripts, play-test | Opus high, Opus review, user play-test | CH1q review (and CH0b) |
| CH2a-CH2d | Skills: Sword Man + first jobs (1, 10, 11, 22) | Sonnet high, Opus review, user play-test | CH1 review |
| CH3a-CH3d | Skills: second jobs (100, 101, 112), hyper actives, transcendence rows | Sonnet high (CH3d Opus), Opus review, user play-test | CH2 reviews |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives; damage exception + overrides; Rune Slayer reflect and memo readers | Opus high, Opus review, user play-test | CH2 reviews (CH4c: CH3d review) |
| CH5 | UI and data: Elsword's `UnitTemplet` rows, cut-in table | Sonnet high, Opus review, user play-test | CH1 review; beside CH2-CH4 |
| CH-N | Not run: Elsword has no templet-backed June-only class | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5 reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH0 runs now (read-only, alongside anything); CH0b follows it and may run before char-chung Z. CH1-CH4 all edit `X2GUElsword_SwordMan.*`: one at
a time. CH5 runs beside CH2-CH4. If CH0's counts put a group over ~45 functions, CH0 splits it with a digit suffix (`CH1e2`) and writes the
kickoff and STATUS row; it does not merge groups. Play-test follow-ups after CH1 final get the next letters (`CH1r`).

## Phase CH0 - Elsword map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_elsword/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init <InitState 32d23ff0> <InitStateCommon> <base>`
and `skill 32d223e0` (find InitStateCommon from vtable `33522914`; Chung's call is in his CH0 Done note), then run `ch0c_listing_check.py` on it:
every row must match the raw listing; all `~` rows and std::string-named rows (`USI_HYPER_MODE`, START / WIN / LOSE roll) by hand. Chung's
builders take a character argument or are copied into `char_elsword/` with Elsword's range: say which. Add `elsword` to `charport_adis.py`'s
`CLASS_RANGE`; confirm it lists the uncarved overrides.

**Scope** (Chung's CH0 items, with Elsword's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32cf2da0-0x32d2763f` **plus the 25 uncarved overrides** (vtable overrides,
   registrars, state callbacks incl. uncarved thunks, helpers, dead), March pair, verdict, target sub-phase, and the **`inside` / `new`** column
   (propose the plain name for every `new` row). Funcmap rows `Char Elsword CH0`. Settle the boundaries (`32cf2da0` after Arme; `32d27620` the
   last; `32d27750` is a shared helper).
2. **State and skill table** `ch0_state_skill.tsv`: 335 / 335 script tables, 127 / 127 skill ids; dead-data and March-only states listed;
   confirm nothing in `X2GUElsword_SwordMan.*` still stores a state id in a `UCHAR` (7l). Name what `MACHINE` (7), `PHANTOM_BLADE_*`, `SI1` /
   `SI2` / `SI3`, `TRANSFORMED`, `DESTRUCTION`, `REVENGE` and `SUMMON_*` are and which group each belongs to.
3. **Keys** `ch0_keys.tsv`: the 5 june-only and 5 neither keys get an owner (the camera pair is Stage 9's); the 8 march-only-site keys
   (`BUFF_FACTOR_ID_MEMO` / `_RELATIVE_MEMO_ID`, `LINK_JUMP_*`, `AFTER_IMAGE`, `READY_SOUND`): does June's script drop a March reader's input; `WAY_OF_SWORD_TYPE` and the `PHANTOM_BLADE_*` keys get their
   readers (June and March).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md`; l.140's
   "x103" note is stale: mark it so). 7c's 29 alpha sites go to the phase that owns each function; 7h's 25 UPDOWN-cancel functions likewise.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Elsword items, unioned) and **effect closure**
   `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). **The `data199` clash:** June's motions sit in June's
   `data199`, March's game dir has its own `data199.kom` (629 KB, other content): say which March kom each June motion goes to (Eve used a new
   `data203`, Chung `data204`). **The motion question:** name the animation `Motion_Elsword_IS.x` drops and whether March's script or class
   plays it; if so, CH0b holds the motion koms for CH1 final (7ad). Stage the `POPUP_TR_*` guide textures and the cut-in `.gfx` CH5 needs.
6. **Unmapped helpers.** Per skill, the callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus
   `CH<n>0` sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0).
7. **Way of the Sword map** `ch0_wos.tsv`: every March WoS function (class `:409-529` / `:7182-7480`, `CX2ElswordGageData`,
   `CX2ElswordMyGageUI`, `X2GageManager`, `X2DamageManager::CalcWayOfSwordPoint` / `CalcVigorMpUpOnAttack` / the const table, the
   `X2DamageEffect.cpp:5505-5519` caller, the `m_CannonBallCount` sync; `cl /P` says which branch is live) paired with June's or "absent in
   June", plus June-only pieces; which pieces are Elesis's too (ported whole here, her class side stays char-elesis); `WayOfSword.lua` /
   `DLG_Elsword_WayOfSword.lua` June vs March and `luabind.py check`. The death-prevention snapshot override `32d058a0` (vslot 0x740: does it
   snapshot the gauge?) goes to CH1q or CH4b. Sized for CH1q; split if over ~45 functions.
8. **Shared data.** Which `ESSI_*` / `RNSI_*` names the C++ enums need (raw id + comment, as Eve); Elsword's 7 `UnitTemplet` rows June vs
   March (`plan_june_side.md` §7; are `STR_ID_4641` / `4642` / `17425` in our string table?); the two `c1a` zero rows (21006, 21013): a
   `JUNE_BUGS.md` candidate for CH2a, not a decision.
9. **S3 census baseline:** `s3_no_state.py --family X2GUElsword_SwordMan.cpp --preprocess` (254 expected) so CH6 compares like with like.
10. **Amend the kickoffs** CH0b-CH5 ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Elsword checklist row has a phase or n/a;
335 / 335 tables and 127 / 127 ids mapped; the 25 uncarved overrides have rows; assets union routes a / b / c and the `data199` target is named;
the motion question answered; the WoS map exists; kickoffs amended; open questions listed.

### CH0 - Done (2026-11-03)

Read-only: no flag, no build. The outputs are in `june2017_re/char_elsword/`.

Tooling:
- Builders copied from Chung's, with Elsword's range and rules: `ch0_build_state_skill.py`, `ch0_skill_listing_emu.py`, `ch0_march_live_states.py`, `ch0_build_class_map.py`, `ch0_unmapped_callees.py` and `ch0_amend_kickoffs.py`.
- `ch0_build_vtable.py` is copied from Luciel's generalised builder.
- New: `ch0_make_hand_names.py`, plus `ch0_numtables.py`, `ch0_build_checklist_rows.py` and `ch0_checklist_lead_fixes.py`.
- `char_eve/ch0_build_assets.py` gained an `elsword` entry and a `name_re` key; Eve's and Chung's runs still reproduce. `charport_adis.py` gained `elsword`.
- Run order for the class map: `NO_HAND_NAMES=1` class map, then `ch0_make_hand_names.py`, then the class map again.

**Step 0.**
- InitStateCommon is **one** function, `32d1f8f0` (ids 7-286, no class gate).
- InitState `32d23ff0` is carved. It holds the class gates (`*(+0x867c)+0x38`), the START / WIN / LOSE roll and id 130.
- Command: `charport_states.py init 32d1f8f0 32d23ff0 3202b110` -> `ch0_reg.tsv`. `ch0c_listing_check.py` reproduces **180 / 180** rows with no hand rows: the roll, `USI_HYPER_MODE` and the 13 `~` rows all came from the tool.
- `skill 32d223e0` + the listing emulator: 143 rows for 127 ids (Elsword's listing exits when `32206450` is non-zero).

**Maps**
- `ch0_class_map.tsv`: 1,004 starts (308 carved, 696 uncarved).
  - By kind: 396 state-fn, 411 thunk, 58 override, 51 helper, 1 registrar, 83 dead, 4 not-Elsword.
  - All **25 uncarved overrides** have rows.
  - **503 to transcribe: 384 `inside`, 119 `new`.** 38 `new` helpers keep `?` with a hint, as Chung's 24 did; the transcribing phase names them.
  - Lead routings are in `ch0_hand_phases.tsv` (WoS functions to CH1q, the CH1o triage) and in `ch0_make_hand_names.py` SPECIAL (March pairs).
- `ch0_state_skill.tsv`: **335 / 335 tables** (284 states, 45 dead, 6 `INIT_*`, 0 unassigned) and **127 / 127 ids** (22 passives CH4a).
  - 27 March-only states are dropped (26 `*_HYPER`, `ESSI_SI1_ES_MEGA_SLASH`).
  - 7 names are registered with no table.
  - `ESSI_RIDING_DAMAGE_FRONT` is defined twice in June's script.
  - 8 names were placed by hand rule: Sword Wait / Destruction Rage CH1q; the shared knight dash combos CH1d2; Leap CH1a; Summon Magic Spear CH1n; Compact Counter guard attack CH1g2. June gates the last on **21216 `RNSI_A_EIS_COMPACT_COUNTER`**; March gates it on `SI_P_ETK_COMPACT_COUNTER`.
- `ch0_vtable_all.tsv` / `ch0_vtable_slots.tsv`: 544 slots, 58 overrides.
  - 0x670 `IsInJobLine`, 0x740 / 0x744 `StoreGageSnapshot` / `RestoreGageSnapshot`, 0x4ac `GetSpecialItemUsableExtraStateID`, 0x850 `AfterImageFrameMove`.
  - 0x508 / 0x50c / 0x510 / 0x840 stay `?` with a hint.
- `ch0_checklist_rows.tsv`: 136 rows, every one a phase or n/a (61 n/a). L140 (x103) is marked stale.
  - 7h has 24 functions, not 25.
  - 7c: 6 of the 14 functions are dead in June (no call, pointer or Ghidra xref).
- `ch0_keys.tsv`: no June-only key needs an Elsword owner (the camera pair is Stage 9's; `IMMUNITY_EDT` is backlog Stage 2's). The 8 march-only-site keys drop no March input.
  - `WAY_OF_SWORD_TYPE` is read at June `32632390` / March `X2DamageManager.cpp:289`.
  - `PHANTOM_BLADE_*` is the Sheath Knight buff (June `32371180` / `X2BuffTemplet.cpp:1227`), not WoS.
- Item 8 data:
  - `ch0_enum_names.tsv`: 4 / 127 in the C++ enum by name, 64 in the core `Enum.lua`.
  - `ch0_unittemplet_elsword.tsv`: `STR_ID_4641` / `4642` / `17425` exist. No UCHAR state id in the class (7l; the only `UCHAR` is a debuff counter).
- S3 baseline `ch0_s3_baseline.txt`: 261 rows, 254 `no_state`.

**Assets**
- `ch0_assets.tsv`: 14,522 rows, routes a / b / c.
- **`ch0b_stage_list.tsv`: 4,848 rows** (4,465 missing, 383 march-copy). Dry run: 36 koms, 0 refused, no new kom.
- `ch0_effect_closure.tsv`: 5,499 rows, 1,963 names. 63 are missing: 52 are sounds already in the list; 11 June itself does not define.
- **data199 clash:** March's `data199.kom` holds 3 CSM motions that are byte-identical to June's. June's 54 Elsword-only motions join them in **data199** (June's number). The 11 differing class motions go to **data033**.
- **Motion question:** nothing is lost. `SI_A_EIS_SPIRAL_STING` moved from `Motion_Elsword_IS.x` to `_TK.x`, and the Infinity Sword row loads `_TK.x` as `m_GameMotion1` in both March and June. **CH0b holds nothing back.**
- Skill icons: 17 misses, all in 3 listed sheets. 21402 `SI_P_ELK_WIND_SLICER` stays missing because its piece is absent from June's own `.TET` (a `JUNE_BUGS.md` row in CH0b).

**Way of the Sword** (`ch0_wos.tsv`, 111 rows: 76 paired, 15 not located, 4 absent in June, 6 June-only)
- Liveness by `cl /EP`: `ELSWORD_WAY_OF_SWORD` and `SPECIAL_ABILITY_WAY_OF_SWORD` are live.
- The Crest of Time pair (0x740 / 0x744) saves and restores only the point. It goes to CH1q.
- About 60 functions in all, so **split CH1q** (class side + damage math + attack MP factor + sync) **/ CH1q2** (gauge data, UI, manager, skill-slot effect, June-only randomiser `322ed400`).
- Elesis's class copies stay hers: brief line added to `stage7-plan.md`.
- `WayOfSword.lua`: June has 42 keys to March's 28, and 3 values changed. 14 June keys have no reader found; CH1q settles them.
- `luabind.py check` on the WoS dialogs: PASS.

**Item 6:** 250 out-of-class callees; 33 are unmapped (`ch0_unmapped_identified.tsv`: 22 confirmed, 11 candidate).
- Four need Opus. All go to phases already on Opus, so **no `CH<n>0` phase is needed**:
  - CH1o: `GetNearestNPCUnitUID` `322d2680` (replace) and June-only `31f76580`.
  - CH1q: `CalcWayOfSwordPointOnDamageReact` `326106c0` (replace) and June-only soft cap `32715040`.
- New `june_march_divergent.tsv` rows: `GetNearestNPCUnitUID` and `CalcWayOfSwordPointOnDamageReact`, each with an allow row until its phase lands. `junediverge.py --selftest` PASS.
- `funcmap.tsv`: 52 rows `Char Elsword CH0`.

**Splits and kickoffs**
- Splits (functions to transcribe): CH1d 28 + **CH1d2** 28; CH1g 31 + **CH1g2** 16; CH1q 21 class functions + engine + **CH1q2**.
- Other phases: CH1a 22, CH1b 35, CH1c 2, CH1e 30, CH1f 36, CH1h 32, CH1i 4, CH1j 13, CH1k 15, CH1m 2, CH1n 1, CH2a 26, CH2b 11, CH2c 18, CH2d 19, CH3a 38, CH3b 22, CH3c 26, CH3d 5, CH4a 18, CH4b 3, CH4c 1.
- Every kickoff CH0b-CH5 has its "Amended by CH0" section (class map col 7, state table col 8). There are 3 new kickoffs; STATUS rows were added and Depends rewired.

**Corrections to the plan:**
- The two `c1a` zero rows are CH2b (21006 Mortal Blow) and CH2c (21013 Wind Blade), not CH2a.
- Overrides 0x24c / 0x250 stay in CH1h (buff ids on class fields).
- L281 `32d076f0` is the Revenge state (CH1b).

**Drift:** the Elesis class side goes to char-elesis (brief line). Nothing needs a new plan. **Open for the user:** none.

## Phase CH0b - Every June Elsword asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_ELSWORD_ASSET`). Sub-tag `DATA`.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (to the kom CH0 named for June's `data199` set), body / face / hair, equipment,
textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`. Stage byte-exact (`kom.py stage-june`), pack each kom, ask once with the
whole kom list, copy what the user approves. Save `data\ErrorLog_pre_CH0b_elsword.txt` first. Create the walk unit the save lacks (Sword Knight
10, level 99, class costume) and a level-99 Sword Man with `createchar.py`, after a `db_backup`. **If CH0 found a dropped animation March
plays**, stage and pack here but copy the motion koms together with CH1 final. Gate: `python tools/skillicons.py check <Elsword's skill-id
range>` = PASS (stage what it lists; the shared `DLG_SKILL_Passive_*` sheets included).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Elsword per job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

### CH0b - Done (2026-11-04)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets and the save only.
- **Reference:** `ErrorLog.txt` (374,056 B, last run 2026-11-03 21:16) saved as `data\ErrorLog_pre_CH0b_elsword.txt` before any copy.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_elsword/ch0b_stage_list.tsv`: `STAGE-JUNE OK: 36 kom(s), 4848 staged (4465 new, 383 replaced, 0 identical), 0 refused, 28s`
  (the dry run matched CH0's numbers). Replaced March files are backed up under `24965799\kom_backup_stage_june\20261104_043447`.
- **Packed:** `kom.py pack` x36, 36 `PACK OK`, 0 refusals (no Lua staged). `kom.py status` showed no removed entries in any of them.
- **Copied (user approved all 36, 2026-11-04):** `kom.py copy` x36, 36 `COPY OK`: data002 012 016 017 020 021 033 044 052 057 063 067 068 069 078 093 118 130 131
  143 148 151 191 192 198 199 212 215 225 datai007 009 019 020 023 027 031. No kom held back: CH0 found nothing March plays that June drops
  (`ch0_motion_question.tsv`), so data033 (11 class motions) and data199 (54 June-only motions) went with the rest. `kom.py status`: 0 `COPY NEEDED`.
- **Done-gate re-run:** `stage-june --dry-run` over the same list: `4848 staged (0 new, 0 replaced, 4848 identical), 0 refused`. 0 `missing` / `march-copy`.
- **Skill icons:** `skillicons.py check 106 900 21000-21060 21200-21236 21400-21420 21600-21605`: 127 skills, **1 miss** (was 17), `RESULT: FAIL`.
  The one is 21402 `SI_P_ELK_WIND_SLICER`: its picture names a piece of `DLG_Elsword_SKILL_ScreenShot_04` that June's own `.TET` lacks
  (staging it again is `2 identical`). June's gap, kept: `JUNE_BUGS.md` row 520. The icon is fine. Nothing left to stage.
- **Save:** backup `db_backup\els_db_20261104_043830_pre_manual.sql`; added with `createchar.py` (lv 99, class costume + top gear +10, 4 skills on the bar):
  unit 66 ElswordKnight (Sword Knight 10), 67 ElswordMan (Sword Man 1). Existing: 25 jtLordKnight (100, lv 99), 26 jtRuneSlayer (101, lv 80),
  32 MagicKnightT (11, lv 70), 33 SheathKnightT (22, lv 70), 34 InfinitySwordT (112, lv 70).
- **Play-test (user) is the gate.** Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_elsword.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh). The class script and motions are still March's, so Elsword plays as before.
- **Drift:** none new.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high, Opus review | Movement and slot-head states: wait (2), walk, jump (6), dash (15), climb (6), charge MP, throw item (6), power attack, common buff, hyper |
| CH1b | Sonnet high, Opus review | Reactions: damage (23 `ESSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement, revenge |
| CH1c | Sonnet high, Opus review | Riding (27 `ESSI_RIDING*`), pepper run (5), dance (3), `MACHINE` (7) unless CH0 places it elsewhere |
| CH1d | Sonnet high, Opus review | Sword Man ground combos (9 `ESSI_COMBO*`, 12 `ESSI_A*`, the `KNIGHT` / `SWORD` / `MAGIC` common blocks CH0 assigns) |
| CH1e | Sonnet high, Opus review | Sword Knight line (10 `ESSI_ESK*`) and Lord Knight line (13 `ESSI_ELK*`, incl. the X-X-X combos) |
| CH1f | Sonnet high, Opus review | Magic Knight line (11 `ESSI_EMK*`) and Rune Slayer line (18 `ESSI_ERS*`) |
| CH1g | Sonnet high, Opus review | Sheath Knight line (14 `ESSI_ETK*`) and Infinity Sword line (11 `ESSI_EIS*`, `PHANTOM_BLADE_*` if CH0 puts it here) |
| CH1h | Sonnet high, Opus review | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact; ctor / dtor offset map; the uncarved ones read from the listing) |
| CH1i | Sonnet high, Opus review | Overrides 0x600-0x7bc |
| CH1j | Sonnet high, Opus review | Common* overrides 0x7c0-0x7ec, CommonFrameMove 0x7cc and the helpers only it reaches |
| CH1k | Sonnet high, Opus review | Skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied) and SpecialAttackEventProcess `32cf3290` (uncarved) with its parts `32d1bd10` / `32cf5760` / `32d0d8e0` |
| CH1m | Opus high, Opus review | Registration: June's `InitState` `32d23ff0` / `InitStateCommon`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), START / WIN / LOSE roll, base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks Elsword registers (Chung's CH1n2 triage first: only rows it did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32d223e0` head (bounds, slot tables, the 7i rule) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals Elsword overrides; gaps the CH1a-CH1k reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (his own decode `:5232-5249`, `CountUsedSkill` `:5152`, `GetSkillSlotIndexUsed` `:12896`, and his entry at `X2GUUser.cpp:26532`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; June-only script keys CH0 gives an owner here |
| CH1q | Opus high, Opus review | Way of the Sword (`ch0_wos.tsv`): June's bodies inside March's WoS functions (class, gauge data, gauge UI, damage-manager point and MP math, sync), shared pieces whole (Elesis's class side stays hers; a stand-in comment where an unported class would break), `WayOfSword.lua` / `DLG_Elsword_WayOfSword.lua` adopted with CH1 final; the vslot 0x740 snapshot override if CH0 puts it here; flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`, sub-tag `WOS` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `Elsword_SwordMan.lua` and the WoS scripts; pack, ask, copy; the play-test |

**Transcription rules (CH1a-CH1k):** as Chung's (`JUNE_CHARACTER_CHUNG_PHASES.md`, "Phase CH1", "Transcription rules"), with Elsword's flags
and his class range `0x32cf2da0`-`0x32d2763f`: `inside` rows inside March's function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` with March's
in `#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`; a later group's callee declared with
`// June <va>: body in <phase>`; an unmapped engine callee is "this needs Opus" (stop). An alpha-tag site (7c) in a function is written in the
phase that owns the function. June's numbers and quirks verbatim; `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).

**Done gate (CH1 final):** Elsword (any class) loads June's script and motions; `[JUNE_CHAR] ELSWORD DATA` logs the load; no new `KEM_ERROR` /
`Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, climb, damage, bounding and riding play June's animations;
the Way of the Sword gauge moves toward vigor and destruction and enters both modes as June; `charport_scan.py` / `charport_keys.py
Elsword_SwordMan` show nothing new.

### CH1a - Done (2026-11-04)

All 22 class-map rows of phase CH1a are written, all `inside`: June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`.
No `new` row, so no `_CORE` define yet (the first phase with a `new` row adds it, and the `#error` guard `_LIVE` then needs). `_LIVE` is in `Always.h` as `//#define`, as the kickoff says.
22 funcmap rows `Char Elsword CH1a`. `june2017_re/char_elsword/ch1_offsets.tsv` is new (seeded from Chung's, class fields dropped; the review confirms the Elsword fields).

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cf33a0` `32cf5930` `32d0db30` | WaitStartFuture / WaitStart / WaitEventProcess | `32d0dd90` `32d072a0` | WalkEventProcess / JumpReadyEventProcess |
| `32cf33d0` `32cf5960` `32d0df70` | JumpUp / JumpDown / JumpLanding EventProcess | `32d0e210` | DashEventProcess |
| `32d073a0` `32cf3470` `32cf5a40` `32d0e410` | DashEnd StartFuture / Start / FrameMove / EventProcess | `32d073e0` `32d0e6c0` | DashJump / DashJumpLanding EventProcess |
| `32cf3480` `32cf34b0` `32d0e8e0` | DashJumpPowerLanding StartFuture / Start / EventProcess | `32d119b0` `32cfa360` `32cf3ca0` `32cf3ce0` | ESSI_LEAP StartFuture / FrameMoveFuture / EventProcess / End |

**Where June differs from March** (everything else is March's logic written out whole; checked against the listings, `charport_adis.py elsword`):
- **Z needs no re-attack gate** in Wait, Walk, JumpLanding and DashEnd: June tests `oneZ` alone. March also needs `m_bReAttackZ1Right != bIsRight || m_fReAttackZ1Time <= 0`.
- **No cliff-climb branch** in JumpDown and DashJump (March's `IsOnCliffFuture() && pureUp && nowSpeed.y <= 0 -> m_CommonState.m_ClimbUp` is live in March).
- **DashJump, both keys held** (`pureRight && pureLeft`): when `oneRight` against a left facing or `oneLeft` against a right facing, the jump speed is clamped and the state goes to JUMP_UP / JUMP_DOWN; otherwise nothing. The single-key branches follow as in March.
- **Wait** enters `ESSI_SWORD_WAIT` from the way-of-the-sword state (vigor / destruction after 0.1 s), as March's live branch; the getter is vslot 0x674 (`32cf50f0` returns gage data `+0x114`), called through March's `GetWayOfSwordState()`. Its body is CH1q's.
- **= March after the macros expand:** Wait Start / StartFuture, JumpReady, JumpUp, DashEnd StartFuture / Start / FrameMove, Dash, DashJumpLanding (the Z test is a separate `if` after the off-line check, as March), DashJumpPowerLanding (all three), the four Leap functions. `CanDown` is called with March's four arguments (the engine port, `JUNE_LINEMAP_CANDOWN_LINK`, takes June's extra ones).
- **Leap StartFuture** keeps the missing early return: with no NPC it changes state and then dereferences a NULL pointer. March has the same code, so there is no `JUNE_BUGS` row. `CX2Game::GetNearestNPCUnitUID` is called by its March name (replace row `322d2680`, allow-listed in `june_diverge_allow.tsv` until CH1o).
- State ids are written by name (`ESSI_*` / `USI_*`); every name already compiles in March. Checklist §7 L254 (`+0x7cc8` power rate): no CH1a function touches it.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because the `#define` is commented out (by design).
- `git diff --patience --numstat`: 733 inserted, 0 deleted in `X2GUElsword_SwordMan.cpp`; 9 / 0 in `Always.h`.
- `cl /P` of `X2GUElsword_SwordMan.cpp`: with `_LIVE` undefined the preprocessed output equals the pre-change output (0 differing non-blank lines); with `/D SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` JumpDown has no cliff branch and DashJump has the both-keys branch.
- One-file probe (`cl /Zs`, US_SERVICE defines, no PCH): `X2GUElsword_SwordMan.cpp` with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: 0 errors each.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 04:58:46, md5 ffead37d8388881ed39809faccdcc462, equal to X2/US_SERVICE/x2.exe`; built at main `90886cfb` + the 2 code files of this phase.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1a review` (Opus 5.5 high), then CH1b.

### CH1a - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 22 June listings (`charport_adis.py elsword`) branch by branch against the `_LIVE` bodies at `4cf62863`. Every state id, branch order, constant and float compare matches.
- **The differences the Done note claims are real.** Wait / Walk / JumpLanding / DashEnd test `oneZ` alone. JumpDown `32cf5960` and DashJump `32d073e0` have no cliff branch: after the on-line and wall tests they go straight to the dash, special and Z tests. In DashJump's both-keys branch the condition is `(oneRight && !bIsRight) || (oneLeft && bIsRight)`, then it clamps to the jump speed if `nowSpeed.y > jumpSpeed` (`fcompp`, `test ah,5`), then JUMP_UP if `0 < nowSpeed.y`, else JUMP_DOWN; it does nothing otherwise. Wait's sword branch calls vslot 0x674 twice (`== 2` / `== -2`, i.e. `WSS_VIGOR` / `WSS_DESTRUCTION`), then tests `m_FrameDataNow` fStateTime (`+0x8088`) `>= 0.1` and enters `ESSI_SWORD_WAIT`. It sits after the CanDown test and before the riding test, as written.
- **The re-attack fields are March-only everywhere.** No June function reads them, and none writes them either. June's CommonFrameMoveFuture `32d0b4d0` decrements and clamps only `+0x9598` (`m_fReDashJumpXTime`); March's also decrements `m_fReAttackZ1Time`. June's ComboZ3 EventProcess `32cf6d80` does not write the side flag or the 0.2 s timer that March's does (`.cpp:8269`). SwordWait EventProcess `32d0f0c0` tests `oneZ` alone. Routed to their own phases below.
- **Spot-checks, end to end:**
  - DashEnd EventProcess `32d0e410`: now > (max - transition) * 0.125 goes to COMBO_Z1 / X1, otherwise to DASH_COMBO_Z1 / DASH_ATTACK_X (`fcompp`, `test ah,5`, `jp`). This is `GetMaxAnimationTime()` (`m_fAnimMaxTime - m_fTransitionTime`).
  - DashJumpPowerLanding EventProcess `32d0e8e0`: the `/ 6.0` gate and the nine-way inner chain match.
  - Leap FrameMoveFuture `32cfa360`: the steps run in this order: the wasted normalize, the x / z / y adds, the 0.5 · (2h / (t/2)²) · e² height, the rotations Y(π/2) and axis(°·0.01745329), then dirDegree / dirVector `+0x7e04` / `+0x7e10`. All match. Also checked: Leap EventProcess (`time <= elapsed`, `test ah,0x41`), End, and Wait Start / StartFuture.
- **Leap StartFuture `32d119b0`:** June calls `GetNearestNPCUnitUID( pos, -1, 9999.0f )`, and March's default `fMaxRange = 9999` gives the same call. Both failure checks call `StateChange( m_CommonState.m_Wait (+0x4088), false )` and then fall through. The discarded `pTargetNPC->GetPos()` call is kept. The degree clamp is 45 below 45°, otherwise `45 - (int)deg` floored at 10, the same values March gets. Kept as is; no `JUNE_BUGS` row.
- **CanDown:** June passes `( pos, 30, &land, &idx, &idx, 1, 0, 1, 0 )`. March's 4-argument wrapper reaches the same values through the `JUNE_LINEMAP_CANDOWN_SIGNATURE` defaults.
- **Offsets confirmed** and added to `june2017_re/char_elsword/ch1_offsets.tsv` (10 rows):
  - Leap block: `+0x95d8` StartPos, `+0x95e4` DirVec, `+0x95f0` VerticalSpeed, `+0x95f4` ParallelSpeed, `+0x95f8` Height, `+0x95fc` Degree (int), `+0x9600` TimeToLeap, `+0x9604` Elapsed. These are written in the header's declaration order.
  - `+0x899d` `m_bLeaping` (CX2GUUser): set to 1 in StartFuture, 0 in End.
  - `+0x9598` `m_fReDashJumpXTime`: the CommonFrameMoveFuture decrement plus the DashJump X gate.
  - Also `+0x4088`, `+0x7e04`, `+0x7e10`, `+0x8088`, vslots 0x138 / 0x674.
- **Tool quirk (no action needed):** `charport_adis` names `[eax + 0x2b0]` `m_fElapsedTime` even when `eax` is the anim pointer. There it is the anim's transition time.

**Hygiene.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range 4cf62863^..4cf62863`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only because the flag is not `#define`d (`//#define` by design).
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`.
- One-file probe (`census.cl_cmd`, `/c /Zs`, no PCH): 0 errors with `_LIVE` off and 0 with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. The `cl /P` outputs differ (21,083,638 vs 21,083,027 B), so the flag took effect.
- No code changed, so there was no build. The deployed exe is still CH1a's (`ffead37d…`).

**Routed (amended into the kickoffs):**
- CH1d: ComboZ3 EventProcess drops March's re-attack writes.
- CH1j: CommonFrameMoveFuture drops the `m_fReAttackZ1Time` decrement.
- CH1q: SwordWait EventProcess has no re-attack gate.

The members stay in the header; March's `#else` bodies still use them.

Next: `/x2-phase char-elsword CH1b` (Sonnet 5.5 high). Nothing to play-test.

### CH1b - Done (2026-11-04)

All 35 class-map rows of phase CH1b are written, all `inside`: June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`.
No `new` row, so no `_CORE` define; `_LIVE` stays `//#define`. 35 funcmap rows `Char Elsword CH1b`; 6 rows added to `june2017_re/char_elsword/ch1_offsets.tsv`.
`RNSI_A_ELK_COUNTER = 21206` and `RNSI_A_ELK_COUNTER_UPGRADE = 21228` are registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES` (June `Enum.src.lua` lines 14575 / 15504). The new member `m_fNoDownAtStandUpAttackRate` (June `+0x9684`) is declared in `X2GUElsword_SwordMan.h` under `_LIVE`. **Corrected by the CH1b review: `+0x9684` is March's `m_fRollingSpeedRel`; no new member.**

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cf57f0` `32d0da10` `32cf3360` `32cf5850` | DieFront StartFuture / Start / FrameMove / EventProcess (also `ESSI_RIDING_DIE`) | `32cf5890` `32d0daa0` `32cf3380` `32cf58f0` | DieBack StartFuture / Start / FrameMove / EventProcess |
| `32cf5a80` `32cf5ad0` `32cf5c50` `32cf5c90` `32cf5e10` | DamageDownFront FrameMove / EventProcess, DamageDownBack FrameMove / EventProcess, DamageAirDownLanding EventProcess | `32d07090` | WinFrameMove (14 win / lose states) |
| `32cf5f90` `32cf6010` `32cf6090` `32cf6110` + End `32cf34d0` `32cf3510` `32cf3550` `32cf3590` | StandUpRolling Front-Front / Front-Back / Back-Front / Back-Back EventProcess + End | `32cf6190` `32cf6210` `32cfa810` `32cfa890` + End `32cf35d0` `32cf3610` `32cf3cf0` `32cf3d30` | StandUpAttack Front / Back and Front / Back NoDown EventProcess + End |
| `32d076f0` `32cf3690` `32cf36a0` `32d0edb0` `32cf6370` | RevengeAttack Start / FrameMoveFuture / FrameMove / CameraMove / EventProcess | | |

**Where June differs from March** (everything else is March's logic written out whole; checked against the listings, `charport_adis.py elsword --phase CH1b`):
- **Stand-up attack with no knock-down** (DamageDownFront `32cf5ad0`, DamageDownBack `32cf5c90`, AirDownLanding `32cf5e10`): when the queued key was X, March rolls `GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent`. June tests `0 < +0x9684`, a float class member, with no roll: `NO_DOWN` when positive, else `ATTACK`. The member is `m_fNoDownAtStandUpAttackRate`; June's ctor `32d09490` zeroes it (CH1k) and `InitPassiveSkillState` `32d1c560` sets it from skill 21200, ability 112 (CH4a). Until those phases land it stays uninitialised, which only matters once `_LIVE` is on. **Corrected by the CH1b review: `+0x9684` is March's `m_fRollingSpeedRel`; no new member.**
- **Revenge Attack Start** `32d076f0`: the counter skill is Lord Knight's `RNSI_A_ELK_COUNTER`; with `RNSI_A_ELK_COUNTER_UPGRADE` learned the state refunds 15 MP (`UpNowMpWithText`) and uses the upgrade skill. The skill level is `31fce5c0` ( 0 without a unit, else `GetSkillLevel( id, false, 0, true )` ), written inline. The damage is **multiplied**: `fPhysic = m_fRevengeAddDamage * ( GetSkillPowerRateValue * ( GetSkillAdditionalPowerRate + 1.0 ) ) * fPhysic` (March adds `m_fRevengeAddDamage * rate`). The knock-up roll is `GetRandomInt( 0 ) < SA_EFFECTIVE_RATE * 100.0`, `RT_UP`, speed ( 1100, 1900 ), as March. The memo 5 `SetDefenceMemoTime( 4 )` branch is gone. Checklist §7 L281 (`3201ad10` called from `32d076f0`) is this function. **Watch item for CH1 final:** the multiply means June's `Elsword_SwordMan.lua` must give `ESSI_REVENGE_ATTACK` a non-zero `DAMAGE_PHYSIC`; a 0 would make the counter hit for 0.
- **Revenge Attack FrameMoveFuture** `32cf3690` is `CommonFrameMoveFuture()` alone (March's empty state-time `if` is gone).
- **Die Front / Back Start** call `DyingStart()` (vslot 0x338) and, by `GetWayOfSwordState()` ( vslot 0x674 ), `End_SwordmanVigor` ( `32d0cd60` ) for `WSS_VIGOR` or `End_SwordmanDestruction` ( `32d0d240` ) for `WSS_DESTRUCTION`; both bodies are CH1q's and March's still run until then.
- **= March after the macros expand:** the other Die functions, DamageDownFront FrameMove (`EventTimerOneshot`, as Chung), DamageDownBack FrameMove, all StandUp Rolling / Attack EventProcess and End (invincible 0.3), Revenge EventProcess / FrameMove / CameraMove, WinFrameMove. DamageDownBack keeps the `0.66` double that March has; the other two use `0.66f`. WinFrameMove's `CreateSequenceHandle` has one extra June argument (-1) that March's signature lacks.
- **Not in CH1b's functions:** the states whose callbacks are `CX2GUUser` virtuals (Groggy, Standup front / back, Small / Big, Bounding, Air, Revenge, Wall Landing) have no class-range function; CH1m registers them and CH1n owns the base bodies. START / WIN / LOSE have no function except `WinFrameMove` (CH1m registers the rest). Checklist §7 L254 (`+0x7cc8`): no CH1b function touches the state power rate.
- No `JUNE_BUGS` row: nothing here looks like a June bug.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because the `#define` is commented out (by design). Its "6 deleted line(s)" are default-diff alignment artifacts: `git diff --patience --numstat` is 701 / 0 in the `.cpp`, 3 / 0 in the `.h`, 7 / 0 in `X2SkillTree.h`.
- `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `RESULT: PASS` (PASS~: 2 marker lines).
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors. `cl /P` with `_LIVE` has `m_fNoDownAtStandUpAttackRate` 4 times and `RNSI_A_ELK_COUNTER_UPGRADE` 3; with it off, 0 and 1 (the enum), so the flag took effect.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 05:34:11, md5 16ecf5cdfe31e4e500c2029b98d106e1, equal to X2/US_SERVICE/x2.exe`; built at main `ee301193` + the 3 code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Routed.** CH1k: the ctor zeroes `m_fNoDownAtStandUpAttackRate` (`32d09490` @`32d0b2c7`, with `+0x9688` / `+0x968c` / `+0x9690` / `+0x9694` and `+0x960f`). CH4a: `InitPassiveSkillState` sets it (`32d1c560` @`32d1c785`, zeroed @`32d1c685`; skill 21200, ability 112, level capped at 10). **Corrected by the CH1b review: `+0x9684` is March's `m_fRollingSpeedRel`; no new member.** CH1q: the two `End_Swordman*` bodies. CH1 final: `ESSI_REVENGE_ATTACK` `DAMAGE_PHYSIC` in June's script (see above).

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1b review` (Opus 5.5 high), then CH1c.

### CH1b - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS with one fix.** I read all 35 June listings (`charport_adis.py elsword --phase CH1b`) against the `_LIVE` bodies at `0f38ef66`. Every state id, branch order, constant and float compare matches, except the member the stand-up test reads.
- **Fix: `+0x9684` is March's `m_fRollingSpeedRel`, not a new member.** A scan of June `.text` for the displacement finds, inside the class range, writes only in the ctor (`@32d0b2c7`, zero) and `InitPassiveSkillState` (`@32d1c685` zero, `@32d1c785` set). The set is `RNSI_A_ES_ROLLING` (21200) `GetSkillAbilityValue( SA_TELEPORT_SPEED_REL (112, s0a_templet_keys row 151), min( level, 10 ) )`, which is March's `m_fRollingSpeedRel` block (`.cpp:2678`). The fourth hit, `ESSI_SI_A_ES_ROLLING` StartFuture `32cf41d0`, is `nowSpeed.x = +0x9684 * 1200.0`, which matches March's `m_fRollingSpeedRel * 1200.f`. So June's no-down stand-up attack fires when Rolling is learned (`0 < m_fRollingSpeedRel`). The three tests now read `m_fRollingSpeedRel` (March's ctor and passive already zero and set it). The duplicate `m_fNoDownAtStandUpAttackRate` is removed, so `X2GUElsword_SwordMan.h` is back to its base bytes. The "uninitialised until CH1k / CH4a" hazard goes with it. `ch1_offsets.tsv`, the three funcmap rows, and the CH1k / CH4a routed bullets are corrected.
- **Revenge Attack Start `32d076f0`, confirmed instruction by instruction:**
  - Level of `0x52ec` (`RNSI_A_ELK_COUNTER_UPGRADE`) > 0 -> `UpNowMpWithText( 15.0f )` (`32278070`), id = upgrade; else `0x52d6` (`RNSI_A_ELK_COUNTER`). Both values are checked against June `Enum.src.lua`.
  - `GetSkillTemplet( id, 1 )` is March's one-argument form (funcmap 32206450: callers pass `( id, 1 )`).
  - `( GetSkillAdditionalPowerRate( templet +8 = m_eID ) + 1.0 )` is a double; `GetSkillPowerRateValue( level ) *` that, then stored as a float.
  - `fPhysic = m_fRevengeAddDamage * rate * fPhysic`, in that order.
  - vslot 0x1c8 (`push 0`) is `GetRandomInt( 0 )` (vtable map: 0x1cc = `GetRandomFloat`, the next declaration). It is turned into a double and compared to `GetSkillAbilityValue( 5 = SA_EFFECTIVE_RATE, level, 0, 0.0f ) * 100.0` with `test ah,0x41` / `jne`, so the knock-up fires when random < rate.
  - `+0x3c4 = 4` (`RT_UP`); `+0x77c / +0x780 = 1100 / 1900`.
  - There is no memo branch. FrameMoveFuture `32cf3690` is a jump to `CommonFrameMoveFuture`.
- **Damage-down family:**
  - AirDownLanding `32cf5e10` is byte-identical to DamageDownFront `32cf5ad0`, with the same front states.
  - DamageDownBack `32cf5c90` differs only in its state ids and in the double `0.66` (`fld time; fcomp qword 0.66; test ah,0x41; jne`).
  - Branch order at the end of the animation: flag 4 (X), then 1, 2, 3, else stand-up. Inputs past 0.66: X, Z, then facing x pureRight / pureLeft. Both match.
- **Spot-checks, end to end:**
  - WinFrameMove `32d07090`: the `SetShowObject` inline, emit 30 / 50, `Dummy2_Lhand`, `SetPosition( pos, 0, 0 )`, and `CreateSequence( this, L"MagicKnightHandFire", 0,0,0, -1,-1,-1,-1, -1.0f, true, 0.0f, false, -1 )`. All but the last argument equal March's defaults.
  - CameraMove `32d0edb0`: `BeginTable( "CAMERA", 1 / 2 )`, `SUB_CAMERA0` then `SUB_CAMERA1` (strings read at `334758f4` / `334758e8`), flag 0 set on every path.
  - DamageDownBack, as above.
- **Families diffed:**
  - The four StandUpRolling EventProcess listings are identical; ground test first, then the end of the animation.
  - The four StandUpAttack / NoDown EventProcess listings are identical; end of the animation first.
  - The eight End listings are identical: `CommonStateEnd`, vslot 0x11c `SetForceInvincible( 0.3f )`, vslot 0x120 `SetShowInvincible( 0.3f )`.
  - Die Front / Back are identical: StartFuture and Start `SetPlaySpeed( 0.5f )` inline, FrameMove `DieFrameMove( 0.19f, 1.12f )`, EventProcess `GUSI_DIE` (4).
  - The C++ bodies are pairwise identical in the same way.
- **Die Start:** vslot 0x338 = `DyingStart` and vslot 0x674 = `GetWayOfSwordState` (`ch0_vtable_all.tsv`). `== 2` -> `32d0cd60` `End_SwordmanVigor`, `== -2` -> `32d0d240` `End_SwordmanDestruction` (class map rows, CH1q).
- **Revenge watch item:** this is narrower than the Done note said. June's `ESSI_REVENGE_ATTACK` (`Elsword_SwordMan.src.lua:12373`) has no `DAMAGE_PHYSIC`; it uses `NUMERICAL_DATA_INDEX = 1` with `APPLIED_SKILL_ID = RNSI_A_ELK_COUNTER`, which `X2DamageManager.cpp:349` already loads. CH1 final confirms the counter hits for a non-zero amount in play.
- **Offsets:** the other 5 CH1b rows are confirmed as written (`+0x3d8` / `+0x3c4` / `+0x77c`, `+0x9e0`, `+0x960c` / `+0x95d4`, `+0x7d86..89`, `+0x8025` / `+0x8018`).

**Hygiene.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range 0f38ef66^..0f38ef66`: `files: 2 PASS, 0 OTHER (X2SkillTree.h = JUNE_ENUM_NAMES)`, `0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. Its "6 deleted" lines are alignment artifacts: `git show --patience --numstat` gives 701 / 0, 3 / 0 and 7 / 0. `RESULT: FAIL` only for the `//#define`.
- After the fix, `flagcheck --worktree --files` on the two class files: 2 PASS, 0 FAIL / OTHER. Its "9 deleted" lines are this review's replacements of CH1b's own inserted lines; `git diff --patience --numstat 0f38ef66^` gives `.cpp` 701 / 0 and the `.h` unchanged.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `--check` on the two class files: `RESULT: PASS`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): 0 errors with `_LIVE` off and 0 with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 07:17:25, md5 271959b3c82811864b8d0d934a1bafb0, equal to X2/US_SERVICE/x2.exe`; built at main `0f38ef66` + the two class files of this review. No `exestr` line: `_LIVE` is off.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1c.md` was checked and needs no change. Next: `/x2-phase char-elsword CH1c` (Sonnet 5.5 high).

### CH1c - Done (2026-11-04)

Both class-map rows of phase CH1c are written, both `inside`: June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`. No `new` row, no `_CORE` define; `_LIVE` stays `//#define`. 2 funcmap rows `Char Elsword CH1c`.
CH0 listed `32d18d80` as `new ?`; March has `CommonHyperModeFrameMove` (`MODIFY_RIDING_PET_AWAKE`), so it is `inside` (class-map row corrected). The 34 riding / pepper / dance / `MACHINE` states have no class-range function (CH0).

| June | `CX2GUElsword_SwordMan::` | Note |
|---|---|---|
| `32d1b910` | `RidingHyperModeFrameMove` | `CommonHyperModeFrameMove( 0.f, 0.1f, true )` = March |
| `32d18d80` | `CommonHyperModeFrameMove( fTime1_, fTime2_, bSound_ )` | March + awakening stand-ins |

**Where June differs from March** (read from the listing, `charport_adis.py elsword 32d18d80`):
- Two independent `EventTimerOneshot` tests (not `else if`), as March. World colour `0xff222222`, timer 1 `ShowMinorParticleHyperModeChange` + `Stickslash1.ogg` (when `bSound_`), timer 2 `UpDownCrashCamera( 30, 0.4 )`, `ClearScreen`, `ApplyHyperModeBuff`, `Slash` / `Break` / `Change.ogg`, then `CommonFrameMove` = March.
- New in June: the awakening effect-set family `32011d20` (begin), `31fd66b0` (present test, `Play` `32011f50` when true) before the Shock, and `ShowMinorParticleHyperModeTrace` only when the set is absent. Stand-ins in `_CONST_ELSWORD_CH1c_` (`ElswordAwakeningEffectBegin` / `SetPresent` / `Play`: no-op, no-op, false = June's empty-name default), the same as Chung's and Eve's (funcmap `32011d20` rows, Transcend S6; reader is Stage 12).
- No `+0x7cc8` touch in either function (checklist §7 L254 is not here). No `JUNE_BUGS` row.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because the `#define` is commented out (by design).
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 07:22:08, md5 7611f39a3f39088493c3478da7d57b54, equal to X2/US_SERVICE/x2.exe (2026-11-04 07:22:01)`; built at main `0e9e1aa0` + the class `.cpp`. No `exestr` line: `_LIVE` is off.

**Routed.** CH1i: `HyperModeFrameMove` `32d1b720` calls this function with its own times (March: `0.05f, 0.83f`). Stage 12: the awakening reader behind the three stand-ins.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-elsword CH1c review` (Opus 5.5 high), then CH1d.

### CH1c - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read both June listings (`charport_adis.py elsword 32d1b910 32d18d80`) against the `_LIVE` bodies at `160886b2`.
- **`32d1b910`:** `fld 0.1 -> [esp+4]`, `fldz -> [esp]`, `push 1`, `call 32d18d80` = `CommonHyperModeFrameMove( 0.f, 0.1f, true )`.
- **`32d18d80`, in order:**
  - World colour: vslot 0x90 world, then `+0xb4..0xc0` and `+0xc4..0xd0` = `0.13333, 0.13333, 0.13333, 1.0`, and `+0xd4 = 0`. That is March's inline `SetWorldColor( 0xff222222 )` (`X2World.h:947`: now colour, fade colour, fade time 0).
  - Timer 1: `fTime1_ > [anim +0x24c]` then `31d28e10` `EventTimer`. That is March's `EventTimerOneshot` (`KTDGXSkinAnim.h:274`, `m_fAnimOneshotTimer < fTime && EventTimer( fTime )`).
  - It calls `ShowMinorParticleHyperModeChange( L"Dummy1_Rhand" )`, which is March's default argument. With `SERV_9TH_NEW_CHARACTER` off, the other overload hardcodes the same bone (`X2GUUser.cpp:34395`). Then `Stickslash1.ogg` when `bSound_ == 1`.
  - Timer 2 is a separate test, not `else`. The call order is:
    1. `32011d20`
    2. `31fd66b0` (`== 1` -> `32011f50`)
    3. `ShowMinorParticleHyperModeShock( L"Dummy1_Rhand" )`
    4. `UpDownCrashCamera( 30.0f, 0.4f )`
  - Then the inline clear: `g_pKTDXApp` (`337676e8`) `+0x1fc` DG manager, `+0xac..0xb8 = 1,1,1,1`, then `+0xa8 = 1`. That equals March's `ClearScreen( num = 1, color = 0xffffffff )` (`KTDGManager.h:133`, colour first, then count).
  - Next `31fd66b0` is tested a second time; `ShowMinorParticleHyperModeTrace` runs only when it is false. Then vslot 0xc0 `ApplyHyperModeBuff` (`ch0_vtable_all.tsv` row 50) and `Slash` / `Break` / `Change.ogg` when `bSound_`.
  - Both timers fall through to vslot 0x7cc `CommonFrameMove`.
- **`PlaySound( name, 1, 0 )`** equals March's defaults (`X2GameUnit.h:2709`, `b3DSound = true, bLoop = false`).
- **Stand-ins:** `ElswordAwakeningEffectBegin` / `SetPresent` / `Play` (no-op, `false`, no-op) give June's absent-set path: no play, trace shown. That is March's behaviour. The present test is called twice, as in June.
- **`+0x7cc8`:** neither listing touches it (0 hits in the dump), so checklist §7 L254 does not apply here.

**Hygiene.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range 160886b2^..160886b2`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only for the `//#define`. `--worktree`: nothing checked, since the tree is clean.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): 0 errors with `_LIVE` off and 0 with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: there is no code change. CH1c's `DEPLOY OK` (md5 `7611f39a...`) stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1d.md` was checked and needs no change. Next: `/x2-phase char-elsword CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-11-04)

All 28 class-map rows of phase CH1d are written, all `inside`: June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`. 28 funcmap rows `Char Elsword CH1d`, plus two for what is declared here.

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cf36d0` `32cf36f0` | ComboZ1 FrameMoveFuture / FrameMove | `32cf6440` `32cf67c0` `32cf6d80` | ComboZ1 / Z2 / Z3 EventProcess |
| `32cf3700` `32cf3710` `32cf3720` `32cf3730` | ComboZ2 / Z3 / Z4 / Z4a FrameMove | `32cf7260` `32cf7340` `32d07c40` | ComboZ4 / Z4a EventProcess, ComboZ4 FrameMoveFuture |
| `32cf3740` `32cf3750` `32cf3760` | ComboX1 / X2 / X3 FrameMove | `32cf73e0` `32cf7810` `32cf7b00` `32cf7bd0` | ComboX1 / X2 / X3 / X3a EventProcess |
| `32d07cc0` `32d07d30` `32d07db0` | ComboX1 / X2 / X3 FrameMoveFuture | `32cf9050` `32cf9090` | JumpAttackZ / X EventProcess |
| `32cf37d0` `32cf37e0` `32d080f0` `32d08180` | JumpAttackZ / X FrameMove, FrameMoveFuture | | |

**Where June differs from March** (everything else is March's logic written out whole):
- **Z1:** no `TEST_MACRO_COMBO` branch. The skill, back-walk (`+0x8c94`), walk (`+0x8c98`) and dash (`+0x8c9c`) cancels need the foot on a line (`ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_` / `_DASH_`; March's macros do not).
- **Z2:** goes to Z3 unless the class is Infinity Sword (112) or Rune Slayer (101), read from the unit data (`+0x38`). Flag 1 (`m_fEventTime[0]`) takes Infinity Sword to `ESSI_ETK_COMBO_ZZZ`; flag 2 (`m_fEventTime[1]`) takes Rune Slayer to `ESSI_ERS_COMBO_ZZZ`. Then the four cancels and the pure Z / X cancels (`+0x8c8c`, `GetComboZStateID` / `GetComboXStateID`). No Sheath Knight test.
- **Z3:** no re-attack writes (CH1a review) and no direction cancel. Flag 2 is Lord Knight's front Z (unit templet class `+0xaf0 -> +4` is 100, forward key held, before 0.333 s). Flags 0 / 1 take `m_fEventTime[0]` / `[1]` to Z4 / Z4a.
- **X1:** flag 1 = Rune Slayer XZ at 0.43 s (templet class); flag 2 = Sheath Knight / Infinity Sword `ESSI_ETK_COMBO_XX` (`m_fEventTime[0]`, templet class); flag 0 = X2 at 0.43 s for the other classes; flag 3 = Magic Knight `ESSI_EMK_COMBO_XZ` (`m_fEventTime[1]`, unit data class `+0x38`, Z key).
- **X2:** no direction cancel. Flags 0 / 1 / 2 (`m_fEventTime[0]` / `[1]` / `[2]`) go to X3 (not job line 1), X3a and `ESSI_ELK_XXX` (job line 1: `IsInJobLine( 1 )`, vslot 0x670).
- **Every flagged branch** except Z1's and Z2's flag 0 also fires when the animation has ended: `|| IsAnimationEnd()` (`ELSWORD_ELSE_IF_FLAG_STATE_CHANGE`). An input branch with a window tests `now < end && start <= now` (`ELSWORD_ELSE_IF_FLAG_SET_IN_WINDOW`); the fixed-time ones (0.27, 0.366, 0.43) are written out.
- **= March after the macros expand (23 of 28 are not in the list above):** the 10 FrameMove thunks, ComboZ1 FrameMoveFuture (speed 0), ComboZ4 / X1 / X2 / X3 FrameMoveFuture (`EventTimerOneshot`; X1's 0.2 s compare is a double 0.2 in June, the same test for every float timer), JumpAttackZ / X FrameMoveFuture, and the Z4 / Z4a / X3 / X3a / JumpAttackZ / JumpAttackX EventProcess.
- `m_fCanNotAttackTime`: Z1 sets it after `StateChange`, Z2 before (as March); X1 sets it before.
- No `JUNE_BUGS.md` row (nothing June does wrongly). No alpha-tag site (7c), no `+0x7cc8` (§7 L254) and no `+0x8ca4` UPDOWN cancel (§7h) in these 28 listings (grep).

**Added here, owned by a later phase.**
- `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` is defined in `Always.h` (with the `_LIVE needs _CORE` guard). It carries `ESSI_ERS_COMBO_ZZZ` (June id 260) and `ESSI_EMK_COMBO_XZ` (251), appended to the `ESSI` enum (their states and functions are CH1f's), and the declaration `bool IsInJobLine( int )` (`32d05960`, body CH1i). CH1f / CH1i kickoffs carry the amendment.
- File-local helpers in `_CONST_ELSWORD_CH1D_`: `IsDataClass` (unit data `+0x38`; no unit is no class) and `IsTempletClass` (templet `+0xaf0 -> +4`; June does not test the unit); five `ELSWORD_*` macros before `//ESSI_COMBO_Z1`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` by design. `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `RESULT: PASS`.
- `git diff --patience --numstat`: 490 / 0 in the `.cpp`, 7 / 0 in the `.h`, 13 / 0 in `Always.h`.
- `cl /P` of `X2GUElsword_SwordMan.cpp` with `_LIVE` off: 0 differing non-blank lines against HEAD's `.cpp` (same header). With `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` the output differs in 914 diff lines and carries `IsDataClass`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. A Git Bash `/D...` argument is path-converted (`C:/Program Files/Git/D...`): pass `-D`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 08:10:07, md5 60c4b03e2b296305a2462cead504030e, equal to X2/US_SERVICE/x2.exe (2026-11-04 08:10:00)`; built at main `1e1cc6e5` + the 3 code files of this phase.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1d review` (Opus 5.5 high), then CH1d2.

### CH1d - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 28 June listings (`charport_adis.py elsword --phase CH1d`, each cut at its `int3`; `32cf7bd0` runs past its `int3` at `32cf7cad` into the next function) against the `_LIVE` bodies at `18c38988`.
- **Z1 `32cf6440`:** the direction clears, then jump down (`-= 45.0`), then `SKILL_CANCEL_AFTER`. The June form matches line for line: foot on line before vslot 0x7f4; the awakening branch is black hole, `oneLCtr`, count `> 0`, vslot 0x560 `CanHyperMode`, the time test, `32070d60`, `+0x8d4c = 1`. Then the back-walk, walk and dash cancels, each with `+0x7e1c` (`0x8c94` / `0x8c98` / `0x8c9c`). Flag 0 then goes to Z2 when `0.27 < t`. `oneZ` sets the flag when `t <= 0.27` and goes straight to Z2 when `0.27 < t <= 0.4` (double). `IsAnimationEnd` is a separate `if`: WAIT, then `+0xde0 = 0.1`.
- **Z2 `32cf67c0`:** flag 0 goes to Z3 when `0.366 < t` and the unit data `+0x38` is neither 112 nor 101. A NULL unit falls through to Z3, which `IsDataClass` matches. Flag 1 uses `m_fEventTime[0]` (`0x8c10` / `0x8c14` / `0x8c18`, which is `EventProcessTime` start / end / change): ETK ZZZ `0xd0`, set by class 112. Flag 2 uses `[1]` (`0x8c1c..0x8c24`): ERS ZZZ `0x104` = 260, set by class 101. Both flagged changes also fire on `IsAnimationEnd`. Then the skill / back-walk / walk / dash cancels and the pure Z / X cancels: `0x8c8c` = `m_fAttackCancelAfter` (`X2GUUser.h:4822`, after `m_fEventTime[10]` and `m_fAttackCancelAfterEx`), through vslots 0x4b0 / 0x4b4. Then `oneZ && t <= 0.366` sets flag 0, and `else if` animation end sets `+0xde0 = 0.1` before WAIT.
- **Z3 `32cf6d80`:** no direction clear and no re-attack writes. Jump down, then the four cancels (4 foot tests). Flag 2 goes to `ESSI_ELK_ZZZfrontZ` `0xc3` when `0.333 < t` or the animation has ended. It is set when `t < 0.333`, `oneZ`, templet `+0xaf0 -> +4 == 100` (only the templet is NULL-tested), and forward is held. Flags 0 / 1 use `[0]` / `[1]` to go to Z4 / Z4a (`oneZ` / `oneX`). The last `else if` on animation end goes to WAIT.
- **X1 `32cf73e0`:** `CANCEL_COMBO_ON_DIRECTION_CHANGE( 3 )` (word `+0x7d85` and byte `+0x7d87`; flag 3 is kept). Jump down, then `else if` animation end. Flag 1 goes to ERS XZ `0xc6` when `t > 0.43` (double, = float 0.43). It is set when `t < 0.43`, `oneZ`, templet 101. Flag 2 uses `[0]` to go to ETK XX `0xd3`, set by `oneX` with templet 22 or 112. Flag 0 goes to X2 when `t > 0.43`; it is set by `oneX` when the templet is not 22 / 112 (a NULL templet sets it). Flag 3 uses `[1]` to go to EMK XZ `0xfb` = 251, set by unit data `+0x38 == 11` then `oneZ`. Last is `_C` skip (12 keys, no Z / X) with `0.6 < t`, setting `+0xde0 = 0.2` before WAIT.
- **X2 `32cf7810`:** no direction clear. Flags 0 / 1 / 2 use `[0]` / `[1]` / `[2]` (`0x8c28..0x8c30`) to go to X3 / X3a / `ESSI_ELK_XXX` `0xc4`. X3 is set by `oneX` with vslot 0x670 `( 1 )` false, X3a by `oneZ`, and ELK XXX by `oneX` with vslot 0x670 `( 1 ) == 1`. The last `else if` on animation end goes to WAIT.
- **The other 23:** the 11 FrameMove thunks are vslot 0x7cc. Z1 FMF does `fldz` into speed x / y, then 0x7c8. Z4 FMF uses `m_pXSkinAnim` (not Future) with `0.17` and walk x `1.3`. X1 FMF sets y = 0, then dash speed at `0.2` (double prev-time test = float). X2 FMF uses `0.31` with walk x `1.5`; X3 FMF `0.3` with walk. Z4 / X3 / X3a EP: jump down, `else if` end, then skip `_ZXC` / `_C` / `_ZXC` at `0.7` / `0.7` / `0.6`. Z4a EP: Up / Down / Right / Left with `t > 0.9`, then a separate end `if`; jump down also jumps to that `if`. Jump attack Z / X EP: on landing, flag 0 sets `+0xde4 = 0.2`, then `JUMP_LANDING`. Jump attack Z / X FMF: forward gives walk speed; `pureDown && !pureUp` (Z) or `pureDown` (X) sets flag 0 and `y -= fGAccel * dt`. All of these = the port.
- **Declared here:** `ESSI_ERS_COMBO_ZZZ` / `ESSI_EMK_COMBO_XZ` are appended without `= value` (the class-internal state enum, as Chung's `CSI_*` appends; the script names states, not numbers). `IsInJobLine( int )` is a plain member for now, and CH1o decides the virtual (vslot 0x670), as the CH1i kickoff says.

**Hygiene.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range 18c38988^..18c38988`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only for the `//#define`. `_CORE` over the same range: `RESULT: PASS`. `--worktree` checks nothing, since the tree is clean.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off, and the same with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: there is no code change. CH1d's `DEPLOY OK` (md5 `60c4b03e...`) stands.

Nothing to play-test (`_LIVE` is off). In `kickoffs/char-elsword/CH1d2.md`, the Task line now names the dash scope and the Done gate names phase `CH1d2`; both were template leftovers. It also gains one line on the `ELSWORD_*` macros and the `_CONST_ELSWORD_CH1D_` helpers. Next: `/x2-phase char-elsword CH1d2` (Sonnet 5.5 high).

### CH1d2 - Done (2026-11-04)

All 28 class-map rows of phase CH1d2 are written, all `inside`: June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`. 28 funcmap rows `Char Elsword CH1d2`, plus one for the state ids declared here.

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cf7cb0` `32cf8110` | DashComboZ1 / Z2 EventProcess | `32cf90d0` `32cf9220` | DashJumpComboZ1 / Z2 EventProcess |
| `32cf8230` `32cf8770` | ESSI_KNIGHT_DASH_COMBO_Z2 / Z3 EventProcess | `32cf9440` | DashJumpComboX1 EventProcess |
| `32cf8b30` | DashAttackX EventProcess | `32d0fa10` | DashJumpComboZ3Landing EventProcess |
| `32d07e20` `32d07eb0` `32d08060` | DashComboZ2 / KNIGHT_DASH_COMBO_Z2 / DashAttackX StartFuture | `32cf3770` `32cf37f0` `32cf3800` `32cf3860` | Z2 / JumpZ1 / JumpZ2 / JumpX1 FrameMove |
| `32cf3780` | KNIGHT_DASH_COMBO_Z3 StartFuture | `32cf3810` `32cf3840` `32cf38a0` `32cf38d0` | Z3Landing / X2Landing StartFuture, Start |
| `32cf9380` `32cf9400` | DashJumpComboZ3 FrameMove / EventProcess | `32cf9530` `32cf9580` `32cf95e0` `32cf9610` `32cf3870` | X2 FrameMoveFuture / FrameMove / EventProcess, X2Landing EventProcess, X2 End |

**Where June differs from March** (the other 19 are March's logic written out whole):
- **Dash Z1 `32cf7cb0`:** four flagged follow-ups on `m_fEventTime[0..3]` (each `|| IsAnimationEnd()`): flag 0 is set by Z for every class except Sheath Knight / Infinity Sword (unit data class) and goes to `ESSI_DASH_COMBO_Z2` (March tested a fixed 0.36 s and the foot on a line); flag 1 to `ESSI_ETK_DASH_COMBO_ZZ` (Z key, classes 22 / 112); flag 2 to `ESSI_ESK_DASH_COMBO_ZChargeX` (X key, `IsInJobLine( 1 )`); flag 3 to `ESSI_EIS_DASH_COMBO_ZX` (X key, class 112). Then the end of animation and the C skip at `> 0.9` (`m_fCanNotAttackTime = 0.1`).
- **Dash Z2 `32cf8110`:** end of animation first, then flag 0 (`m_fEventTime[0]`, Z key) goes to the dash Z3 (March had no flag here).
- **Sword-Knight dash Z2 `32cf8230`:** flags 0 / 1 on `m_fEventTime[0]` / `[1]` to Z3 / Z3a (flag 1 set by X for classes 11 / 101), then the `+0x8ca4` UPDOWN cancel (checklist 7h, written inline: up or down held on a line), the skill / back-walk / walk / dash cancels, the pure Z / X cancels, and the end of animation as a separate `if`. March used fixed 0.45 windows and flag clears.
- **Dash attack X `32cf8b30`:** templet class 112 first (X flag 0 to `ESSI_EIS_DASH_COMBO_XX` at 0.63, window 0.233..0.63, C skip at `> 0.66666`); otherwise end of animation, then unit data class 100 (flag 0 on `[0]` to `ESSI_ELK_DASH_COMBO_XChargeX`, X key, skip keys + Z, 0.66666), 101 (flag 1 on `[1]` to `ESSI_ERS_XZ_CHARGE`, Z key, skip keys + X, 0.66666), else the ZXC skip at 0.6.
- **Dash jump Z1 `32cf90d0`:** flag 1 on `[0]` to `ESSI_EMK_DASH_JUMP_ZX` (X key, `IsInJobLine( 2 )`). **Z2 `32cf9220`:** flag 1 on `[0]` to `ESSI_ETK_DASH_JUMP_ZZZ` (Z key, job line 3); at the end of animation with flag 0 job line 1 goes to `ESSI_ESK_DASH_JUMP_ZZZ`, else Z3. **X1 `32cf9440`:** the Z-key flags read the templet class (11 / 101), March read the unit class.
- June's `ESSI_KNIGHT_DASH_COMBO_Z3` / `_Z3a` are March's `ESSI_DASH_COMBO_Z3` / `_Z3a` (ids 174 / 175; March's name wins). `32d0fa10`: the second dash test after the X branch is dead code in March and absent in June (not written). `CanDown` is March's 4-argument call (funcmap W1f forwarder). `32cf7cb0` runs on into `32cf8110` in the raw listing (no `int3`).
- No `JUNE_BUGS.md` row (nothing June does wrongly), no alpha-tag site (7c), no `+0x7cc8`. Checklist §7h L420 (`UPDOWN_CANCEL_AFTER`, 24 functions): `32cf8230` is done here; the other 23 are in CH1e-CH1g2 and CH2+ (their kickoffs carry the inline form).

**Added here, owned by later phases.**
- Seven June-only state ids appended to the `ESSI` enum under `_CORE` (no `= value`, as CH1d): `ESSI_ESK_DASH_COMBO_ZChargeX` (241), `ESSI_ESK_DASH_JUMP_ZZZ` (242), `ESSI_ELK_DASH_COMBO_XChargeX` (244) for CH1e; `ESSI_EMK_DASH_JUMP_ZX` (249), `ESSI_ERS_XZ_CHARGE` (255) for CH1f; `ESSI_ETK_DASH_JUMP_ZZZ` (258) for CH1g; `ESSI_EIS_DASH_COMBO_ZX` (224) for CH1g2. The CH1e / CH1f / CH1g / CH1g2 kickoffs carry an amendment.
- `Always.h`: `_LIVE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL` guard (defined at line 5880; the probe with `_LIVE` on compiles). In the `.cpp`: macro `ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH( bKey_ )` beside CH1d's, under `_LIVE`.

**Gates.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is `//#define` by design. `_CORE` on the `.h`: `files: 1 PASS`, `RESULT: PASS`.
- `git diff --numstat`: 546 / 0 in the `.cpp`, 7 / 0 in the `.h`, 3 / 0 in `Always.h`.
- `cl /P` of `X2GUElsword_SwordMan.cpp` with `_LIVE` off: 0 differing non-blank lines against HEAD's `.cpp` (same headers). With `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` the output differs and carries the new bodies.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,937,408 B, dated 2026-11-04 09:07:48, md5 09774139c9622ecc7130cc073c0f7d0e, equal to X2/US_SERVICE/x2.exe (2026-11-04 09:07:41)`; built at main `97735be0` + the 3 code files of this phase. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1d2 review` (Opus 5.5 high), then CH1e.

### CH1d2 - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 28 June listings (`charport_adis.py elsword --phase CH1d2`; `32cf7cb0` runs on into `32cf8110`, cut there) against the `_LIVE` bodies at `679a0bea`. Windows: `m_fEventTime[i]` is `0x8c10 + 0xc*i` (start / end / change); every flagged change is `change < t || IsAnimationEnd()`, every window `start <= t < end`.
- **Dash Z1 `32cf7cb0`:** flag 0 on `[0]` to `0xad` (`ESSI_DASH_COMBO_Z2`), set by `oneZ` when unit data `+0x38` is neither 22 nor 112 (a NULL unit sets it, as `IsDataClass`). Flag 1 on `[1]` to `0xd7` ETK ZZ, set by `oneZ` with data class 22 / 112 (NULL skips). Flag 2 on `[2]` to `0xf1`, vslot 0x670 `( 1 )` tested before `oneX`. Flag 3 on `[3]` to `0xe0`, data class 112 then `oneX`. Then animation end (jump down `-= 45.0`), then the `_C` keys (12, no Z / X) with `0.9 < t`, `+0xde0 = 0.1`, WAIT.
- **Dash Z2 `32cf8110`:** animation end first, then flag 0 on `[0]` to `0xae` (= March's `ESSI_DASH_COMBO_Z3`), set by `oneZ`.
- **Sword-Knight dash Z2 `32cf8230`:** flags 0 / 1 on `[0]` / `[1]` to `0xae` / `0xaf`; flag 1 set by `oneX` with data class 11 / 101. Then `0 <= +0x8ca4 < t`, foot on line, `pureUp || pureDown` to WAIT; `SKILL_CANCEL_AFTER` (`0x8c90`, vslot 0x7f4, the awakening branch); back-walk / walk / dash (`0x8c94` / `0x8c98` / `0x8c9c`); pure Z / X on `0x8c8c` through vslots 0x4b0 / 0x4b4. The end-of-animation `if` is separate (jump down with `-= 45.0`). **Z3 `32cf8770`:** end (jump down with no y offset), then `_ZXC` keys with `1.0 < t`.
- **Dash attack X `32cf8b30`:** templet `+0xaf0 -> +4 == 112` first: end, flag 0 to `0xe1` at `0.63`, set by `oneX` in `0.233 <= t < 0.63` (double `0.2329999953508377` = float `0.233f`), `_C` with `0.66666 < t`. Else end, then data class 100 (flag 0 on `[0]` to `0xf4`, `oneX`; skip keys Up Down Right Left **Z** A S D C Q W E R), 101 (flag 1 on `[1]` to `0xff`, `oneZ`; the same list with **X**), both at `0.66666`, else `_ZXC` at `0.6`. June tests the time before the keys; the port tests keys first. Nothing follows either test, so they are equal.
- **Dash jump Z1 `32cf90d0` / Z2 `32cf9220`:** `oneZ` sets flag 0 every frame; landing to `USI_DASH_JUMP_LANDING`. Flag 1 on `[0]` to `0xf9` (`oneX` then job line 2) / `0x102` (`oneZ` then job line 3). At the end with flag 0: Z2 (`0xb4`) / job line 1 to `0xf2`, else `0xb5` Z3. **X1 `32cf9440`:** `oneX` writes flags 1,0,0 (word clear); `oneZ` with templet 11 writes 0,1,0, with templet 101 writes 0,0,1; landing, then at the end `0xb8` / `0xb9` / `0xc8`.
- **Z3 landing `32d0fa10`:** jump down, double-direction dash, then `max / 6 < t` (`0x220 - 0x2b0` = `GetMaxAnimationTime()`): Z1, X1, walk, `USI_JUMP_READY`, `oneDown` with `CanDown`, end. June has no second dash test inside; March's is dead code, and the port leaves it out.
- **The other 19 equal the port.** FrameMove thunks are vslot 0x7cc; Knight Z3 SF is 0x7c0 only; the three dash SFs are byte-identical (dash speed when pressing forward); the landing SFs use `*= 0.5`, Start copies hitCamera; Z3 FM `0.26 < t < 0.37` on `m_pXSkinAnim` (camera, reset, hit); X2 FMF `EventTimerOneshot( 0.23 )` to `fMaxGSpeed`; X2 FM `0.15 < t` (no camera copy); Z3 EP bHit to `0xb6` else dash-jump landing; X2 EP to `0xba`; X2 landing EP; X2 End `UpCrashCamera( 200, -3500 )`.
- **Declared:** the seven `ESSI` appends without `= value` follow CH1d (names, not numbers). The `Always.h` UPDOWN guard is correct. CH0's scan counts 24 UPDOWN functions; the checklist's "25" is the old lead.

**Hygiene.**
- `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range 679a0bea^..679a0bea`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` (the OTHER row is the `.h`, which is `_CORE`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `RESULT: FAIL` only for the `//#define`. `--worktree` checks nothing, since the tree is clean.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH) with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors. `cl /P`: `ESSI_ERS_XZ_CHARGE` 2 / `m_fUpDownCancelAfter` 4 with `_LIVE`, 1 / 2 without, so the flag took effect.
- No build: there is no code change. CH1d2's `DEPLOY OK` (md5 `09774139...`) stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1e.md` is ready as written. Next: `/x2-phase char-elsword CH1e` (Sonnet 5.5 high).

### CH1e - Done (2026-11-04)

All 30 class-map rows of phase CH1e are written: 16 `inside` (June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`) and 14 `new` (plain names under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, compiled in every build, referenced by nothing; `InitState*` untouched, CH1m registers). 30 funcmap rows `Char Elsword CH1e`.

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cf8920` `32d07fd0` | ESSI_KNIGHT_DASH_COMBO_Z3a EventProcess / StartFuture (inside) | `32cf9670` `32cf96c0` `32cf9720` `32cf38f0` | ESSI_KNIGHT_DASH_JUMP_COMBO_X2 FrameMoveFuture / FrameMove / EventProcess / End (inside) |
| `32cf9830` | ESSI_KNIGHT_DASH_JUMP_COMBO_X3 EventProcess (inside) | `32cf3f70` `32cfaed0` | ESSI_ELK_ZZZfrontZ FrameMove / EventProcess (inside) |
| `32cf3f80` `32cfafa0` | ESSI_ELK_XXX FrameMove / EventProcess (inside) | `32cf3f90` `32cfb420` | ESSI_ELK_XXXX FrameMove / EventProcess (inside) |
| `32d180a0` `32cff880` `32d0c740` | ESSI_SI_P_ESK_AUTO_GUARD Start / EventProcess / End (inside; small shares Start / End) | `32cff8d0` `32cff920` | AUTO_GUARD_SMALL EventProcess, AUTO_GUARD_END EventProcess (END and END_SMALL) (new) |
| `32cf4d30` `32d01040` | ESSI_ESK_DASH_COMBO_ZX StateStartFuture / EventProcess (new) | `32cf4d40` `32d014c0` | ESSI_ESK_DASH_COMBO_ZXX StateStartFuture / EventProcess (new) |
| `32d017f0` `32d01b70` `32d01ba0` | ESSI_ESK_DASH_COMBO_ZChargeX, DASH_JUMP_ZZZ, DASH_JUMP_ZZZ_LANDING EventProcess (new) | `32d01f10` `32d02290` `32d02710` | ESSI_ELK_DASH_COMBO_XChargeX, _XX, _XXX EventProcess (new) |
| `32d02a90` `32d02ed0` | ESSI_ELK_XXXXX, ESSI_ELK_XXXXXX EventProcess (new) | | |

**Where June differs from March** (everything else is March's logic written out whole; checked against `charport_adis.py elsword --phase CH1e`):
- **Lord Knight X X X `32cfafa0`:** direction cancel clears flag 0, then flag 0 on `m_fEventTime[0]` goes to `ESSI_ELK_XXXX` (March: fixed 0.5 s). Then the UPDOWN cancel (`+0x8ca4`), `SKILL_CANCEL_AFTER`, back-walk / walk / dash (on a line), the Z and X cancels (`+0x8c8c`), then the end of animation. March had only the skill cancel at 0.67. **X X X X `32cfb420`:** the same chain with flag 0 to `ESSI_ELK_XXXXX` (no direction cancel), end of animation, ZXC skip keys after 1.3 s.
- **Auto Guard:** Start calls `UpNowMp( 1.0 )` (vslot 0xf0) before the effect set. EventProcess ends in `ESSI_SI_P_ESK_AUTO_GUARD_END` (off the line: `JUMP_DOWN`, y minus 45) and has no dash / walk / skill cancels (March went to WAIT). The END state's EventProcess is June's cancel chain: end -> WAIT, then dash, back-walk, walk (no foot test), then skill (A S D C Q W E R down, `SpecialAttackEventProcess( ASUT_GROUND )`); every taken branch sets `m_fCanNotAttackTime = 0.1`. Small and End-small use the same functions with `_SMALL` targets.
- **Dash Z X `32d01040` / dash X X `32d02290`:** the end-of-animation block comes first (fall, y minus 45, or WAIT), then UPDOWN, skill, back-walk, walk, dash, Z, X cancels, then flag 0 to the next state (`m_fEventTime[0]`, X key). **Dash Z X X `32d014c0`:** off the line fall, skill, walk, dash, Z, X cancels, end -> WAIT; no UPDOWN. **Dash jump Z Z Z landing `32d01ba0`:** like dash Z X without the flag part and `JUMP_DOWN` without the y offset. **X X X X X `32d02a90`:** end block, flag 0 to `ESSI_ELK_XXXXXX`, then the cancels. **Dash X X X `32d02710` and X X X X X X `32d02ed0`** are identical: end block, the cancels.
- **Charge states `32d017f0` / `32d01f10`:** off the line fall; X released, or `m_fEventTime[0].keyInputStart` reached, goes to dash Z X / dash X X; otherwise the cancels (no end block). **Dash jump Z Z Z `32d01b70`:** on a line -> landing.
- **= March after the macros expand:** Z3a (2), dash jump X2 (4) and X3, ZZZfrontZ (2), the three FrameMove thunks, the two `StateStartFuture` thunks, X2 End, Auto Guard End.
- Skill cancels use the shared `SKILL_CANCEL_AFTER` macro (June's `BUFF_AWAKE_LOCK` expansion, as CH1d2); the `+0x8e90 ? +0x8e84 : m_pXSkinAnimFuture` anim choice is read as `m_pXSkinAnimFuture` (CH1b). No alpha-tag site (7c), no `+0x7cc8` (§7 L254). Checklist §7h L420 (`UPDOWN_CANCEL_AFTER`): 10 functions here carry it (`32d01040` `32d017f0` `32d01ba0` `32d01f10` `32d02290` `32d02710` `32d02a90` `32d02ed0` `32cfafa0` `32cfb420`), the row stays open for CH1f+.
- **`JUNE_BUGS.md` row 521:** `ESSI_ELK_XXXXXX` has no script table; X X X X X has no `EVENT_PROCESS_TIME0`, so any X press during it makes the end of the animation enter a state with no data. Ported as June has it; CH1 final's play-test should avoid pressing X in X X X X X.

**Added here.**
- Ten June-only state ids appended to the `ESSI` enum under `_CORE` (no `= value`, as CH1d): `ESSI_ESK_DASH_COMBO_ZX` (239), `_ZXX` (240), `ESSI_ESK_DASH_JUMP_ZZZ_LANDING` (243), `ESSI_ELK_DASH_COMBO_XX` (245), `_XXX` (246), `ESSI_ELK_XXXXX` (247), `ESSI_ELK_XXXXXX` (248), `ESSI_SI_P_ESK_AUTO_GUARD_SMALL` (236), `_END` (237), `_END_SMALL` (238); 14 declarations beside `IsInJobLine`.
- CH1d's helper block (`IsDataClass`, `IsTempletClass`, the `ELSWORD_*` cancel macros) moved from `_LIVE` to `_CORE` (the new rows use it; `_LIVE` needs `_CORE`, so nothing changes for `_LIVE`): 2 deleted lines, `OTHER` in a `_LIVE`-only flagcheck. `Always.h`: `_CORE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL` (defined at line 5880).

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Its "27 deleted" lines are alignment: `git diff --patience --numstat` is 678 / 2 in the `.cpp` (the guard swap), 24 / 0 in the `.h`, 5 / 0 in `Always.h`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,945,088 B, dated 2026-11-04 09:49:53, md5 c095a2a43ca502220d46e25d4f1e3f9a, equal to X2/US_SERVICE/x2.exe (2026-11-04 09:49:46)`; built at main `2970ed2d` + the 3 code files of this phase. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1e review` (Opus 5.5 high), then CH1f.

### CH1e - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS after one fix.** I read all 30 June listings (`charport_adis.py elsword --phase CH1e`, cut at the `int3`) against the bodies at `f1fdcd76`, branch by branch, and compared operand sequences between sibling functions.
- **Fixed (under `_CORE`): the two charge states fall with no y offset.** In dash Z charge X `32d017f0` and dash X charge X `32d01f10`, the off-the-line branch is `StateChange( JUMP_DOWN )` and then straight to `CommonEventProcess` (no `fsub 45.0`). The port had `position.y -= LINE_RADIUS * 1.5f` in both; I removed the 2 lines. Of the 16 June functions with a `JUMP_DOWN`, only these two and the dash jump Z Z Z landing `32d01ba0` (already right) skip the offset.
- **Lord Knight X X X `32cfafa0`:** the direction cancel clears flag 0. Next comes the fall with `-= 45`, then flag 0 to `0xc5` on `[0]`: `0x8c18 < t || IsAnimationEnd()`, set by `oneX` in `0x8c10 <= t < 0x8c14`. After that, UPDOWN (`0 <= 0x8ca4 < t`, foot, Up / Down), `SKILL_CANCEL_AFTER( 0x8c90 )` (vslot 0x7f4 and the awakening branch), back-walk / walk / dash (`0x8c94` / `0x8c98` / `0x8c9c`, foot), pure Z / X on `0x8c8c` (vslots 0x4b0 / 0x4b4), and the animation end to WAIT. **X X X X `32cfb420`:** the same operand sequence minus the direction cancel, with flag 0 to `0xf7`, then the `_ZXC` keys with `1.3 < t`.
- **Dash Z X `32d01040` = dash X X `32d02290`** (the only operand that differs is the target, `0xf0` / `0xf6`): the end block (fall `-= 45` / WAIT) comes first, then UPDOWN, skill, the three ground cancels, Z, X, and flag 0 last. **Dash X X X `32d02710` = X X X X X X `32d02ed0`:** the same, with no flag. **Dash Z X X `32d014c0`:** fall, skill, the ground cancels, Z, X, end. **X X X X X `32d02a90`:** end block, flag 0 to `0xf8`, then the cancels (`JUNE_BUGS` 521 holds: `ESSI_ELK_XXXXXX` has no table). **Charge states:** after the fall comes `!pureX || 0x8c10 <= t` to `0xef` / `0xf5`, then the cancels with no end block. **Dash jump Z Z Z `32d01b70`:** on a line, go to `0xf3`.
- **Auto Guard.** Start `32d180a0`: `UpNowMp( 1.0 )` (vslot 0xf0, which `funcmap` confirms is UpNowMp), then `EffectSet_CSI_CIP_IRON_WILL`, then reduce percent `+=` guard, then `CommonStateStart`. EP `32cff880` / small `32cff8d0`: at the end, the fall `-= 45`, else `0xed` / `0xee`, with no cancels. END `32cff920`: end, then dash `0x8c9c` (no foot test), back-walk `0x8c94`, walk `0x8c98`, then skill `0x8c90` with A S D C Q W E R and `SpecialAttackEventProcess( 1 )`. Each taken branch sets `+0xde0 = 0.1`. End `32d0c740`: `CommonStateEnd`, then reduce percent `-=` guard.
- **Written = March, confirmed:** Z3a EP (`_C` keys, `0.9 < t`) and SF (forward dash speed). Dash jump X2: FMF `EventTimerOneshot( 0.23 )` to `fMaxGSpeed`, FM `0.15 < t` on `m_pXSkinAnim` with DamageCheck setting reset / hit, EP to `0xbb` / `0xa3` / `0xba`, End `UpCrashCamera( 200, -3500 )`. X3 EP: `_ZXC`, `0.68 < t`. ZZZfrontZ EP: fall first, then end, then `_C` with `0.867 < t`. The three FM thunks (vslot 0x7cc) and the two SF thunks (vslot 0x7c0).

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --range f1fdcd76^..f1fdcd76`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, 27 deleted (the alignment the Done note explains), `RESULT: PASS`. The fix with `--worktree --files X2Lib/X2GUElsword_SwordMan.cpp`: `PASS +0 -2`, `0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `--check X2Lib/X2GUElsword_SwordMan.cpp` after the fix: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `DEPLOY OK: X2_offline.exe is 25,945,088 B, dated 2026-11-04 09:56:16, md5 512dae76166864bcaeb0a8b84e592323, equal to X2/US_SERVICE/x2.exe (2026-11-04 09:56:08); built at main f1fdcd76 + 1 uncommitted code file(s)` (that file is this fix).

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1f.md` gained a note on the fall offset. Next: `/x2-phase char-elsword CH1f` (Sonnet 5.5 high).

## Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL`.
Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 1 has it -> CH2a; else the first of 10 / 11 / 22.

| Sub-phase | Scope |
|---|---|
| CH2a | Sword Man (1) skills; the two `c1a` zero rows (21006, 21013) verbatim with a `JUNE_BUGS.md` row |
| CH2b | Sword Knight (10) skills |
| CH2c | Magic Knight (11) skills, incl. his type-2 row |
| CH2d | Sheath Knight (22) skills |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI
arguments, 7p `THROW_WOODEN_PIECE`'s stop time if it is his, the 7h UPDOWN cancels CH0 put here, the numerical-only tables of `plan_june_side.md`);
its effect closure (a name found only now is staged and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's
`m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id. `NUM use` lines for Elsword skills;
March vs June damage in `MODS.md`.

**Done gate:** every class-1 / 10 / 11 / 22 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot
casts June's state, motion and effect and deals damage; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence

**Run with:** as CH2 (CH3d on Opus: it carries engine rows). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Lord Knight (100) skills (`LORD_KNIGHT_SAND_STORM`'s March-only templet, 7u) |
| CH3b | Rune Slayer (101) skills, incl. his type-2 row; the effect-set modifier tag 19 (`32d18230`, 7a) if CH0 puts it here |
| CH3c | Infinity Sword (112) skills (`PHANTOM_BLADE_*` if CH0 puts it here) |
| CH3d | **Opus.** Hyper actives (type 13, 6 rows; June's nested hyper switch 0x5461-0x5465), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): Elsword's classes on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`), his `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Elsowrd.lua` (June's name); checklist 7ao whole (hyper effect start `32d17ea0`, uncarved; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); his hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_ELK/ERS/EIS_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and his transcended pictures, read from June's row pushes; `POPUP_TR_L_KNIGHT` / `_R_SLAYER` / `_I_SWORD` present |

**Done gate:** as CH2 for 100 / 101 / 112 and the hyper actives; a transcended Elsword (`settranscend`) casts his hyper-class skills, slots A-5 /
B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Elsword's skill-id range>` = PASS after the transcendence
rows.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32d1c560` (vslot 0x690) and Elsword's passive ids (types 1 / 8: 3 + 25 rows), their objects and readers; 7am's 8 delegate callbacks in `32d1c560`; the three `CreateAndInsertSkillAndSocketBuffFactorToList` calls (`322a25f0`; Spell Chain 21406's bool = Magic Coating 21418 learned); flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `32d1e0e0` whole (June-only, 7b: 21419 `RNSI_P_EIS_OPEN_MIND` with `ATTACK_TRIGGER_TYPE` 0xf -> `+0x510` = 1.0; 21420 `RNSI_P_EIS_INFINITY_POTENTIAL` with 0x10 -> vslot 0x238 and the `322b5320` push); the other overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style); §7 l.281 power-rate socket range (`32d076f0`); the death-prevention snapshot override `32d058a0` unless CH0 gave it to CH1q |
| CH4c | 7j Rune Slayer magic reflect: the engine branch in `320462a0` @`32046a55`-`32046aa0` (`X2GUUser.cpp`; divergent row `june_march_divergent.tsv:220`) and the class's `32d0c820` (Magic Coating 21418 as a transcended hyper-class skill: MP gain + effect), every class, own flag if the engine branch is not Elsword's; `MemoConditionElsword` (RTTI `3373d9b0`) and the memo readers; the S3d memo consumer lines that waited for his skill states (`STATUS_ARCHIVE.md` l.715) |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); a Rune Slayer reflects magic as June, and a transcended
one with Magic Coating takes 0 magic damage with the MP gain; 0 Elsword `no_state` rows left except passives with no state in June.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI`. Sub-tag `UI`.

**Scope:** Elsword's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays, `UC_150` stays out):
`m_fLobbyRotationY`, `m_HyperCutIn` for 100 / 101 / 112, the names `STR_ID_4641` / `4642` / `17425`; the cut-in table
`X2GUElsword_SwordMan.cpp:14-44` and the InitState index switch `:873-898` replaced by June's names and layout (7t cut-in `.gfx` names); 7ah; the
dialog / gauge pieces CH1q does not take. **Done gate:** a select-screen pass of all nine March characters with no new error; Elsword's skill and
hyper cut-ins play June's art.

## Phase CH-N - not run for Elsword

All seven June Elsword classes up to transcendence exist in March. The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara
runs it first.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Elsword per class (CH0b creates 1 and 10), one transcended per
second job. Walk: every skill in an A and a B slot, a hyper, the Way of the Sword in vigor and destruction, Rune Slayer reflect, one dungeon per
job, then **leave the game and return to the village**. Re-run `census.py`, `charport_scan.py`, `charport_keys.py Elsword_SwordMan` and
`s3_no_state.py`; before / after numbers in the Done note. **The becoming-transcended transition on un-transcended units (user rule,
2026-10-28):** one unit per second-job class by `/tc 1`, quest 42030 (`/qdone 42030`) and the book 288493 (`/additem 288493 1`), each showing the
guide popup with that class's picture, the new skill-tree rows, the HUD portrait, and surviving a relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Elsword" section (flags, files, data, revert order: exe + data036 + motion koms
together); no `_DEBUG` flag left defined; checklist rows marked `(Elsword: CHn)`; the Aisha plan's "Template for Stage 7" gets an Elsword row (what
he took against this plan); backlog Stage 7 note; the `stage7-plan` row points at the next character (`char-raven`).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR ELSWORD <sub>"`.

## Open decisions (for the user)

None open. Asked at planning (2026-11-03): the order against Lu/Ciel. **User: Lu/Ciel waits until the March characters are ported**
(Order, above). Settled by standing rules, not decisions: the 2 `c1a` zero rows are June bugs kept verbatim (CH2a); Way of the Sword's shared
pieces are ported whole with a stand-in comment where Elesis's unported class would break (CH1q); the brief's `DAMAGE_SKILL_ID` table job does
not exist (Context). CH0 may raise one if the motion question finds a lost animation June's files no longer carry.
