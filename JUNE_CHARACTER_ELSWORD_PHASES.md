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

### CH1f - Done (2026-11-04)

All 36 class-map rows of phase CH1f are written: 21 `inside` (June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`) and 15 `new` (plain names under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, compiled in every build, referenced by nothing; `InitState*` untouched, CH1m registers). 36 funcmap rows `Char Elsword CH1f` (VA set = the class map's). Read with `charport_adis.py elsword --phase CH1f`; the branchy EventProcess bodies were also compared operand by operand against the reviewed CH1d2 / CH1e listings (sibling shapes).

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32d07f40` `32cf8840` | MAGIC_KNIGHT_DASH_COMBO_Z3 StartFuture / EP (inside) | `32d0f640` `32cf8a00` `32cf3790` | MAGIC_KNIGHT_DASH_COMBO_Z3a FM / EP / EndFuture (inside) |
| `32cf97a0` `32d0fc00` `32cf97e0` | MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a FMF / FM / EP (inside) | `32cf3fa0` `32cfb900` `32d12550` `32cfb970` | ERS_XZ StartFuture / FMF / FM / EP (inside) |
| `32d128b0` `32cfbdb0` | ERS_XZZ FM / EP (inside) | `32cf3fc0` `32cf3ff0` `32cfbe10` `32d12c50` `32cfbe80` | ERS_DASH_JUMP_XZ StartFuture / EndFuture / FMF / FM / EP (inside) |
| `32d131d0` `32cfbee0` | ERS_DASH_JUMP_XZZ FM / EP (inside) | `32cf4d50` | ERS_XZ_CHARGE_StateStart (new) |
| `32d03250` `32d035d0` | EMK_DASH_JUMP_ZX / ZXX EP (new) | `32d03880` `32d19040` `32d03d00` | EMK_COMBO_XZ EP / FM, XZ_CHARGE EP (new) |
| `32d040c0` `32d19300` `32d04440` `32d195c0` | EMK_COMBO_XZZ EP / FM, EMK_COMBO_FULL_CHARGE_XZZ EP / FM (new) | `32d047c0` `32d04b80` `32d19880` | ERS_XZ_CHARGE EP, ERS_FULL_CHARGE_XZZ EP / FM (new) |
| `32d04f00` `32d05250` | ERS_COMBO_ZZZ / ZZZZ EP (new) | | |

**Where June differs from March** (everything else is March's logic written out whole):
- **Rune Slayer X Z `32cfb970` (inside):** end block with a foot-first fall (`JUMP_DOWN`, y minus 45; else animation end -> WAIT), then flag 0 on `m_fEventTime[0]` to the new `ESSI_ERS_XZ_CHARGE` (Z key), then UPDOWN / `SKILL_CANCEL_AFTER` / back-walk / walk / dash / Z / X cancels. March had a fixed 0.5 s window to `ESSI_ERS_XZZ`, a direction cancel and the ZXC skip at 0.8. **Z3a `32cf8a00`:** off the line only `y -= 45` (no state change), the walk cancel needs the foot on a line, then animation end and the ZXC skip after 1.2 straight to WAIT.
- **Rune Slayer dash jump X Z Z `32cfbee0`:** the two jump attacks use the flag macro with `|| IsAnimationEnd()` (March's macro: time only); window `t < m_fAttackCancelAfter`. **`32d131d0`:** vigor cost = `MAGIC_ERS_DASH_JUMP_XZZ_MP_COST * 0.5f` (global `337aef00`, filled at `333ac8c0`; March has the literal 3.5). **X Z Z `32d128b0`:** the XZZ effect set is also pushed into the delete-on-state-end list (+0x3ec0 = `m_vecEffectSetToDeleteOnStateEnd`, as the CH1a review of Arme found). **Z3a FM `32d0f640`:** the Rune Slayer test reads the unit templet (`IsTempletClass`).
- **New Magic Knight / Rune Slayer states.** Charge states `32d03d00` / `32d047c0`: from `stateChange` on, Z released or past `keyInputStart` goes to X Z Z before `keyInputEnd`, else to the full-charge state; before `stateChange` the cancels (no y offset on the fall). X Z `32d03880` = dash Z X's shape (flag 0 to the charge, Z key); X Z Z / full-charge EPs (`32d040c0` `32d04440` `32d04b80`) = dash X X X. Z Z Z `32d04f00` has flag 0 to Z Z Z Z and no UPDOWN / Z / X cancels; Z Z Z Z `32d05250` only the fall, end and skill / walk / dash cancels.
- **Magic Knight dash jump X `32d03250` / `32d035d0`:** the air shape has no skill branch, only the awakening half (macro `ELSWORD_AIR_AWAKE_CANCEL_AFTER`, `_CORE`, needs `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`), then **walk before back-walk**, dash, and `oneZ` / `oneX` on `m_fAttackCancelAfterEx` (+0x8c88) with the foot on a line. CH1g (`ESSI_ETK_DASH_JUMP_ZZZ`) will likely share it.
- **Magic Knight FrameMoves `32d19040` `32d19300` `32d195c0` and `32d19880`:** the MP gate is `IsEnoughMPForAttack( 0, Dummy2_Lhand, 0, 0 )` (vslot 0x6f0, March has it): cost 0, so it never fails; the real MP comes from the effect set rows (`CONSUME_MP`, Eve CH1r gate). The three EMK states reuse `m_bERS_COMBO_XZ_Rune_Created` (+0x960e): X Z sets it, X Z Z and the charge variant test and clear it (cleared whatever happens); the Rune Slayer full charge clears it only after playing. Effect sets: `EffectSet_EMK_ComboXZ` / `_ComboXZZ` / `_ComboXZZ_Charge`, `EffectSet_ERS_COMBO_FULL_CHARGE_XZZ`.
- **= March after the macros expand (no behaviour change):** Z3 StartFuture / EP, Z3a EndFuture, X2a (3), ERS X Z StartFuture / FMF / FM, X Z Z EP, dash jump X Z (5). No `JUNE_BUGS.md` row (nothing June does wrongly: the zero-cost gate is data-driven), no alpha-tag site (7c), no `+0x7cc8` (§7 L254). Checklist §7h L420 (`UPDOWN_CANCEL_AFTER`): `32cfb970` `32d03880` `32d03d00` `32d040c0` `32d04440` `32d047c0` `32d04b80` carry it; the row stays open for CH1g+.

**Added here.**
- Six June-only state ids appended to the `ESSI` enum under `_CORE` (no `= value`): `ESSI_EMK_DASH_JUMP_ZXX` (250), `ESSI_EMK_COMBO_XZ_CHARGE` (252), `ESSI_EMK_COMBO_XZZ` (253), `ESSI_EMK_COMBO_FULL_CHARGE_XZZ` (254), `ESSI_ERS_FULL_CHARGE_XZZ` (256), `ESSI_ERS_COMBO_ZZZZ` (257); 15 declarations after CH1e's. `ELSWORD_AIR_AWAKE_CANCEL_AFTER` after the CH1d helper block; `Always.h`: `_CORE needs SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` (defined at line 4847).

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Its "33 deleted / 1 non-ASCII" are diff alignment: `git diff --patience --numstat` is 1078 / 0 in the `.cpp`, 21 / 0 in the `.h`, 3 / 0 in `Always.h`, 0 non-ASCII inserted lines.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off and with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,955,840 B, dated 2026-11-04 10:32:22, md5 ebf30421907b2fe65d6831a6b7ca1a16, equal to X2/US_SERVICE/x2.exe (2026-11-04 10:32:15); built at main 3c92633c + the 3 code files of this phase`. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1f review` (Opus 5.5 high), then CH1g.

### CH1f - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 36 June listings (`charport_adis.py elsword --phase CH1f`, cut at the `int3`) against the bodies at `ab1260de`, branch by branch. For the eleven new and changed EventProcess bodies I also compared a per-function skeleton: the operands compared, the input bytes, the state pushes and the vslots.
- **Rune Slayer X Z `32cfb970`:** the fall (`JUMP_DOWN`, `-= 45`), then animation end to WAIT. Next, flag 0 to `0xff` (`ESSI_ERS_XZ_CHARGE`) on `0x8c18 < t || IsAnimationEnd()`, set by `oneZ` in `0x8c10 <= t < 0x8c14`. Then UPDOWN (`0x8ca4`, foot), skill on `0x8c90` (foot, `SpecialAttackEventProcess( 1 )`, then the awakening half), back-walk / walk / dash (`0x8c94` / `0x8c98` / `0x8c9c`, foot), and Z / X on `0x8c8c` (vslots 0x4b0 / 0x4b4). **Z3a `32cf8a00`:** off the line, only `-= 45`; then walk on `0x8c98` with the foot; then animation end; then the `_ZXC` keys with `1.2 < t` to WAIT. **Z3 `32cf8840`:** `_C` keys and `0.9`, = March.
- **Charge states `32d03d00` / `32d047c0`:** off the line, `JUMP_DOWN` with no offset. If `t >= 0x8c18`: when `pureZ` is held and `t < 0x8c10`, nothing; otherwise `t < 0x8c14` goes to X Z Z (`0xfd` / `ESSI_ERS_XZZ`), else to full charge (`0xfe` / `ESSI_ERS_FULL_CHARGE_XZZ`). If `t < 0x8c18`, the cancel tail runs. That is exactly the port's nesting.
- **Magic Knight X Z `32d03880`:** the end block, then the cancels, then flag 0 last (to the charge, `oneZ`). **X Z Z `32d040c0`, both full charges `32d04440` / `32d04b80`:** the end block and the cancels, with no flag. **Z Z Z `32d04f00`:** fall, end, flag 0 to `ESSI_ERS_COMBO_ZZZZ`, skill, the three ground cancels; no UPDOWN, no Z / X. **Z Z Z Z `32d05250`:** the same without the flag.
- **Dash jump Z X / Z X X `32d03250` / `32d035d0`:** `DASH_JUMP_LANDING` on a line. Z X then has flag 0 to `ESSI_EMK_DASH_JUMP_ZXX` (`oneX`). Both then have the awakening half on `0x8c90` (black hole, `oneLCtr`, `CanHyperMode`, quick-slot wait, `+0x8d4c = 1`), walk `0x8c98` **before** back-walk `0x8c94`, dash `0x8c9c`, and `oneZ` / `oneX` on `0x8c88` (`m_fAttackCancelAfterEx`). `ELSWORD_AIR_AWAKE_CANCEL_AFTER` matches `SKILL_CANCEL_AFTER`'s awakening half under `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`, line for line.
- **Dash jump X Z Z `32cfbee0`:** flag 0 to `0xb1` / flag 1 to X on `0x8c8c < t || IsAnimationEnd()`, set in `t < 0x8c8c`. **Dash jump X Z `32cfbe80`:** `oneZ` sets flag 0 first, then landing, then at the end `ESSI_ERS_DASH_JUMP_XZZ` or `JUMP_DOWN`. **X2a EP `32cf97e0`:** landing, then `0.59 < t` to `JUMP_DOWN`. **`32cf4d50`:** `CommonStateStart`, then `+0x960e = 1`.
- **MP costs.** June's flush is vslot 0xe4 with the cost first. Z3a / X2a FM push 6.0, X Z / dash jump X Z push 4.0 and X Z Z pushes 0 (`fldz`). These equal `MAGIC_FIRE_BALL` / `MAGIC_ERS_XZ` / `MAGIC_ERS_DASH_JUMP_XZ` / `MAGIC_ERS_XZZ` under `BALANCE_PATCH_20120329` (`Always.h:3251`), and none of them tests the sword state. **`32d131d0`:** `GetWayOfSwordState() == 2 ? [337aef00] : 0.0`. `333ac8c0` fills `337aef00` with `0.0 * 0.5`, so the cost is 0 in either state (March's vigor value is 3.5). The port's `MAGIC_ERS_DASH_JUMP_XZZ_MP_COST * 0.5f` (= 0) is exact.
- **FrameMoves.** `32d19040` (0.3): `IsEnoughMPForAttack( 0, Dummy2_Lhand, 0, 0 )`, then `EffectSet_EMK_ComboXZ` with scale `+0x96d8`, power `+0x959c`, life `*= +0x96d4` and `+0x7c = 1`; the flag `+0x960e` is 1 on success and 0 on failure. `32d19300` / `32d195c0` (0.133): flag first, then the gate; `+0x960e = 0` on every path. `32d19880` (0.01, else 0.38 stops `+0x95b4`): the flag is cleared only after the effect set plays, so it stays set when the gate fails, as the port has it. `32d128b0`: the handle goes into `+0x3eb0` and then `+0x3ec0`; 0.38 stops `+0x95b4`.
- **= March, confirmed:** Z3 SF, Z3a EndFuture, X2a FMF / FM, ERS X Z SF / FMF / FM (the `+0x198` remote-unit test around the flag), X Z Z EP, dash jump X Z SF / EndFuture / FMF / FM. The six new `ESSI` names sit at the enum end under `_CORE`; registration is CH1m's.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --range ab1260de^..ab1260de`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 EOL-mismatched`, `0 encoding change(s)`, `RESULT: PASS`. Its 33 deleted lines are the alignment the Done note explains.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1f's Done note stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1g.md` gained a note on June's flush slot. Next: `/x2-phase char-elsword CH1g` (Sonnet 5.5 high).

### CH1g - Done (2026-11-04)

All 31 class-map rows of phase CH1g are written: 29 `inside` (June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`) and 2 `new` (plain names under `_CORE`, referenced by nothing; `InitState*` untouched, CH1m registers). 31 funcmap rows `Char Elsword CH1g`. No new flag, enum member, namespace or helper: the CH1d macros cover every branch (`ESSI_ETK_DASH_JUMP_ZZZ` was already in the enum).

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cfc8e0` `32d1e200` `32cfc900` `32cf4090` | COMBO_ZZZ StateStart / FM / EP / End (inside) | `32d1e400` `32cfcb70` `32cf40b0` | COMBO_ZZZZ FM / EP / End (inside) |
| `32cf40d0` `32d1e4d0` `32cfce30` `32cf40e0` | COMBO_ZZZZZ StateStart / FM / EP / End (inside) | `32d0b730` `32d1e5a0` `32cfcfe0` `32cf4100` | COMBO_XX StateStart / FM / EP / End (inside) |
| `32d0b7d0` `32d1e7a0` `32cfd210` `32cf4120` | COMBO_XXX_LOOP StateStart / FM / EP / End (inside) | `32d1e870` `32cfd540` `32cf4150` | COMBO_XXXZ FM / EP / End (inside) |
| `32cf4170` `32d1e940` `32cfd700` `32cf4190` | COMBO_XXX_END StateStart / FM / EP / End (inside) | `32d1ea10` `32cfd7a0` `32cf41b0` | DASH_COMBO_ZZ FM / EP / End (inside) |
| `32d054d0` `32d19b80` | DASH_JUMP_ZZZ EP / FM (new, 2 declarations after CH1f's) | | |

**Where June differs from March** (everything else is March's logic written out whole):
- **EventProcesses use the Elsword ground macros.** The walk cancel needs the foot on a line (March's `WALK_CANCEL_AFTER` did not) in all eight; the flag branches are `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE` (time or animation end) and `_SET_IN_WINDOW`, where March's `ELSE_IF_STATE_CHANGE_ON_EX_` had time only.
- **Z Z Z `32cfc900`:** no direction-change cancel (March has one); `SKILL_CANCEL_AFTER( 0.01f )` sits before the flag branch. **Z Z Z Z `32cfcb70`:** keeps the direction cancel; the flag branch comes before the skill cancel. **Z Z Z Z Z `32cfce30`:** no flag, no dash.
- **X X `32cfcfe0`:** both flags on `m_fEventTime[0]` / `[1]` (`oneDoubleX` to the loop, `oneX` to the end). **X X X loop `32cfd210`:** the loop-count gate `< 3` is on the window, not on the state change. **X X X Z `32cfd540`:** walk cancel BEFORE the skill cancel (the others are the reverse). **X X X end / dash combo Z Z:** fall, end, walk cancel only.
- **FrameMoves:** June has no `EventCheck`, only `EventTimerOneshot`. Z Z Z `32d1e200`, X X `32d1e5a0` and dash combo Z Z `32d1ea10` tag the `ElswordSheath02` mesh with the effect-alpha category (`TagEffectAlphaMesh`, 7c sites `32d1e2ee` `32d1e68e` `32d1eafe`, under `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION`). Dash combo Z Z's second damage table is `DAMAGE_DATA_LAST` (string read at `335197f0`).
- **New dash jump Z Z Z (`32d054d0` / `32d19b80`):** EP lands on the foot (`USI_DASH_JUMP_LANDING`). FM at 0.296: `IsEnoughMPForAttack( 8, Dummy2_Lhand, 0, ( 0, 0, -45 ) )` (cost `3344d3e4`, rotation `33491dc8`), then `EffectSet_ETK_DASH_JUMP_ZZZ` with every `PlayEffectSet` default; the effect set must exist in the effect data (CH5/CH1 data check, nothing runs yet).
- **= March:** all 14 StateStart / StateEnd bodies (loop count `+0x962c`, `bSuperArmorNotRed` `+0x8040`, sheath handle `+0x9670`) and the five damage-only FrameMoves. The loop count is June's protected int; March's plain `int` is equal in behaviour. No `JUNE_BUGS.md` row (nothing June does wrongly), no `+0x7cc8` (§7 L254; no listing touches it). Checklist §7c: the three alpha sites above (`32d1efb2` belongs to a later group).
- Offsets used that `ch1_offsets.tsv` lacks (the review confirms and adds them): `+0x8d38` dash cancel, `+0x9624` / `+0x9628` damage change times, `+0x962c` loop count, `+0x9670` sheath handle.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (the _LIVE bodies), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `_LIVE` alone reports FAIL only because its `#define` is commented out (by design). `git diff --patience --numstat`: 486 / 0 in the `.cpp`, 2 / 0 in the `.h`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,957,376 B, dated 2026-11-04 11:02:11, md5 4a246dca62456862e75a5a70490a9b50, equal to X2/US_SERVICE/x2.exe (2026-11-04 11:02:04); built at main d8191866 + the 2 code files of this phase`. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1g review` (Opus 5.5 high), then CH1g2.

### CH1g - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 31 June listings (`charport_adis.py elsword --phase CH1g`, cut at the `int3`) against the bodies at `55871570`, branch by branch, with the `jmp` targets to confirm each else-if chain.
- **Z Z Z `32cfc900`:** fall (`JUMP_DOWN`, `-= 45`), end to WAIT, dash, then the skill half on the literal `0.01` with the foot and the awakening half, then flag 0 (`0x8c18 < t || IsAnimationEnd()`) to `0xd1`, window `0x8c10 <= t < 0x8c14` on `oneZ`, then the ground walk cancel. No direction cancel. **Z Z Z Z `32cfcb70`:** the direction cancel clears flag 0 only (`CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 )`), then fall, end, dash, flag 0 to `0xd2`, `oneZ`, skill on `0x8c90`, walk. **Z Z Z Z Z `32cfce30`:** fall, end, skill, walk.
- **X X `32cfcfe0`:** fall, end, flag 0 (`0x8c18`) to `0xd4` with `oneDoubleX` in `0x8c10`-`0x8c14`, flag 1 (`0x8c24`) to `0xd6` with `oneX` in `0x8c1c`-`0x8c20`, walk; no skill. **X X X loop `32cfd210`:** end goes to `0xd6` (X X X end); the loop-count test `< 3` (protected int `+0x962c`) sits inside the `oneX` window; flag 1 to `0xd5` on `oneZ`; skill; walk. **X X X Z `32cfd540`:** direction cancel (flag 0), fall, end, walk BEFORE skill. **X X X end `32cfd700`, dash combo Z Z `32cfd7a0`:** fall, end, walk only. `SKILL_CANCEL_AFTER` under `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` matches each skill / awakening pair line for line.
- **FrameMoves:** X X `32d1e5a0` is byte-identical to Z Z Z `32d1e200` after normalising addresses: `ElswordSheath02` (`33524310`) at 0.01 through `CreateInstance( this, name, GetPos(), GetRotateDegree(), GetRotateDegree() )`, handle to `+0x9670`, bounding radius 0, alpha tag `32296ee0` (= `TagEffectAlphaMesh`, funcmap C10b-2); handle reset at `+0x8d38`; `DAMAGE_DATA_NEXT` (`3346f050`) at `+0x9624`. Dash combo Z Z `32d1ea10` adds `DAMAGE_DATA_LAST` (`335197f0`, confirmed in the string dump) at `+0x9628`. The five damage-only ones are identical to each other.
- **`+0x8d38` is the dash cancel:** June keeps a Future cancel set (`+0x8c88` Ex .. `+0x8c9c` Dash) and a Now set in the same order (`+0x8d24` .. `+0x8d38`, funcmap `32057b80` C9h-4c); March has one set, so `m_fDashCancelAfter` is right, as March's own body has it.
- **New dash jump Z Z Z:** EP `32d054d0` lands on the foot (`0x16`). FM `32d19b80`: `EventTimerOneshot( 0.296 )` (`33523f10`); vslot `0x6f0` `IsEnoughMPForAttack( 8.0 [3344d3e4], Dummy2_Lhand position or zero, 0.0, ( 0, 0, -45.0 [33491dc8] ) )`; on success `PlayEffectSet( L"EffectSet_ETK_DASH_JUMP_ZZZ", this )` with every default (no trace, not hyper, power and life `-1`, scale `( 1, 1, 1 )`, no custom position, three zero vectors, force-down `1.0`).
- **StateStart / StateEnd (14):** Z Z Z start `CommonStateStart` then `+0x9670 = -1`; X X start count `= 0` then `CommonStateStart`; loop start count `+1`, `CommonStateStart`, `+0x8040 = 0`; X X X end start `+0x8040 = 0`; loop end `+0x8040 = 1` after `CommonStateEnd`; the rest `SetEnableAttackBox( L"Sword", true )` + `CommonStateEnd`. All = the port.
- `ch1_offsets.tsv` gains 7 rows: `+0x8c10` event times, `+0x8c88` Future cancel set, `+0x8d38` Now-set dash cancel, `+0x8040`, `+0x9624` / `+0x9628`, `+0x962c`, `+0x9670`.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --range 55871570^..55871570`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `0 EOL-mismatched`, `0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1g's Done note stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1g2.md` gained a CH1g-review amendment (its Done gate named `CH1g`). Next: `/x2-phase char-elsword CH1g2` (Sonnet 5.5 high).

### CH1g2 - Done (2026-11-04)

All 16 class-map rows of phase CH1g2 are written: 12 `inside` (June's body in March's own function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`) and 4 `new` (plain names under `_CORE`, referenced by nothing; `InitState*` untouched, CH1m registers). 16 funcmap rows `Char Elsword CH1g2`. No new flag, enum member, namespace or helper: the CH1d macros cover every branch (`ESSI_EIS_DASH_COMBO_ZX` was already in the enum).

| June | `CX2GUElsword_SwordMan::` | June | `CX2GUElsword_SwordMan::` |
|---|---|---|---|
| `32cfdbe0` `32cf4260` `32d1f130` `32d13d20` `32cfdcb0` `32cf4280` | CompactCounterGuardAttack Start / StartFuture / FrameMove / CameraMove / EventProcess / StateEnd (inside) | `32cfe280` `32cfe440` | ESSI_EIS_DASH_COMBO_XX / XXX EventProcess (inside) |
| `32cfe5e0` `32d1f2d0` `32cfe650` `32cf4490` | ESSI_EIS_DASHJUMP_COMBO_XX EP, XXX FM / EP / End (inside) | `32cf4470` `32cfe090` `32d1f200` `32d17550` | new: DASH_COMBO_ZX StateEnd / EventProcess / FrameMove, DASH_COMBO_XX FrameMove |

**Where June differs from March** (everything else is March's logic written out whole):
- **Compact Counter Start `32cfdbe0`:** no `SetDefenceMemoTime( 4.f )` (June has no `SMI_ELSWORD_MEMO5` call here); `GetSkillLevel( SI_A_ES_COUNTER, false, 0, true )`, chance `GetSkillAbilityValue( SA_EFFECTIVE_RATE, level ) * 100.0` against `GetRandomInt( 0 )` (the CH1b Revenge shape). **EventProcess `32cfdcb0`:** the walk cancel is the Elsword ground one (March's had no foot test). StartFuture / StateEnd / CameraMove / FrameMove = March.
- **Dash X X `32cfe280`:** flag 0 to X X X on `m_fEventTime[0].stateChange` (time or animation end), set inside the `oneX` window, then ground walk and dash cancels (March: `ELSE_IF_STATE_CHANGE_ON_EX_`, `WALK_CANCEL_AFTER`). **Dash X X X `32cfe440`:** ground dash cancel, then `SKILL_CANCEL_AFTER( m_fWalkCancelAfter )` (walk time, as March; JUNE_BUGS #522).
- **New dash Z X EP `32cfe090`:** animation end goes to WAIT on a line only (no jump-down); then ground walk, ground dash, `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`. **Dash jump X X X `32cfe650`:** end on a line to WAIT, ground walk, ground dash, skill cancel. **Dash jump X X `32cfe5e0`** = March.
- **New dash X X FM `32d17550`:** `EventTimerOneshot( 0.2 )` (`3346eeb8`); `IsEnoughMPForAttack( 6.0 [33439b40], Dummy2_Lhand, 0, ( 0, 0, 45.0 [3345b36c] ) )`; then `EffectSet_EIS_DASH_XX` with every `PlayEffectSet` default (the effect set must exist in the effect data; nothing runs yet). No 7c alpha site in the 16.
- Offset added to `ch1_offsets.tsv`: `+0x411c` `m_bApplyMotionOffset` (the StartFuture store).

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `git diff --patience --numstat`: 277 / 0 in the `.cpp`, 4 / 0 in the `.h`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors with `_LIVE` off and with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,958,400 B, dated 2026-11-04 11:29:58, md5 ea2e47e971666f494533a75a1ef7bdf5, equal to X2/US_SERVICE/x2.exe (2026-11-04 11:29:50); built at main 32ead863 + the 2 code files of this phase`. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1g2 review` (Opus 5.5 high), then CH1h.

### CH1g2 - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 16 June listings (`charport_adis.py elsword --phase CH1g2`, cut at the `int3` / tail `jmp`; `32cfe090` and `32cfe650` each run on into the next function at line 155) against the bodies at `7ad88d24`, branch by branch.
- **Compact Counter Start `32cfdbe0`:** `CommonStateStart`; `fPhysic = m_fRevengeAddDamage + fPhysic`; `GetSkillLevel( 0x4b2, 0, 0, 1 )`, and `0x4b2` = 1202 = `SI_A_ES_COUNTER` in the live `UPGRADE_SKILL_SYSTEM_2013` branch (`MEGASLASH = 1200` + 2; the `= 1200` at `X2SkillTree.h:776` is the dead `#else`); templet `32206450( id, 1 )` (the one-argument March call, as CH1b); `GetRandomInt( 0 )` (vslot `0x1c8`) as a double against `GetSkillAbilityValue( 5 = SA_EFFECTIVE_RATE, level ) * 100.0`, proceeding on `rand < value * 100`; then `+0x3c4 = 1` (`RT_SMALL_DAMAGE`) and `+0x77c` back speed `( 0, 0 )`. No memo call, no `SetDefenceMemoTime`. StartFuture `+0x411c = 0`; StateEnd `SetEnableAttackBox( L"Sword", true )` + `CommonStateEnd`.
- **Compact Counter EP `32cfdcb0`:** end: off the foot `JUMP_DOWN` with `y -= 45.0` (`LINE_RADIUS * 1.5f`), else WAIT; then the walk cancel on `+0x8c98` with the foot test (`ELSWORD_GROUND_WALK_CANCEL`). **CameraMove `32d13d20`:** cases 1 and 2 are tail-merged by the compiler into one `BeginTable( "CAMERA", n )` + `SUB_CAMERA0` / `SUB_CAMERA1` path; the flag is set on both exits = March.
- **Dash X X `32cfe280`:** end (fall / WAIT); flag 0 to `0xe2` on `+0x8c18 < t || IsAnimationEnd()`, clearing the flag; `oneX` sets flag 0 in `+0x8c10 <= t < +0x8c14`; ground walk (`+0x8c98`); ground dash (`+0x8c9c`, vslot `0x668`). No skill cancel. **Dash X X X `32cfe440`:** end; ground dash; skill and awakening halves both on `+0x8c98` (JUNE_BUGS #522).
- **Dash Z X EP `32cfe090`:** end goes to WAIT only on the foot (no fall branch); ground walk, ground dash, skill / awakening on `+0x8c90` (`m_fSkillCancelAfter`). **Dash jump X X X EP `32cfe650`** is byte-identical to it after normalising addresses, and both ports have the same body. **Dash jump X X `32cfe5e0`:** `oneX` flag 0, `oneZ` flag 1, on the foot `0xe3` / `0xa3` / `0xba` = March.
- **Dash X X FM `32d17550`:** inline Oneshot 0.2 (`334411a8` / `3346eeb8`); `GetCloneFrame( L"Dummy2_Lhand" )` +0xc0 position or zero; vslot `0x6f0( 6.0 [33439b40], pos, 0.0, ( 0, 0, 45.0 [3345b36c] ) )`; on true `PlayEffectSet( L"EffectSet_EIS_DASH_XX", this )` with the defaults (power `-1`, scale `( 1, 1, 1 )`, zero vectors, force-down `1.0`), the shape of CH1g's reviewed `32d19b80`. `EffectSet_EIS_DASH_XX` is in the live `data036_kom/EffectSet.lua` and in June's `EffectSet.src.lua`.
- **Damage-only FMs** `32d1f130` / `32d1f200` / `32d1f2d0` are identical to each other (Oneshot on `+0x9624`, `DAMAGE_DATA_NEXT` `3346f050`); StateEnds `32cf4280` / `32cf4470` / `32cf4490` are identical to each other. All = the port.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --range 7ad88d24^..7ad88d24`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `0 non-ASCII inserted line(s)`, `0 EOL-mismatched`, `0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1g2's Done note stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1h.md` checked: it depends on this review and needs no change. Next: `/x2-phase char-elsword CH1h` (Sonnet 5.5 high).

### CH1h - Done (2026-11-04)

All 32 class-map rows of phase CH1h are accounted for: 25 `inside` written (June's body in March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`), 5 `new` ( 4 under `_CORE`, 1 under `_LIVE` ), 2 with nothing to write ( `32d26ff0`, `32d27620` ). `_LIVE` stays `//#define`; `_CORE` was defined by CH1d. 33 funcmap rows `Char Elsword CH1h`; 16 rows added to `june2017_re/char_elsword/ch1_offsets.tsv`.
**Class-map corrections (CH0 listed these three `new ?`; March has all three):** `32d0d600` = `DeleteElswordMinorParticle`, `32d0d7a0` = `SetElswordMinorParticleByEnum`, `32d1b9d0` = `ApplyBuffToHarshSlayer` (a delegate: its callers are the CH4a registration thunks, not the destructor). `32d0d660` (`SetElswordMajorParticleByEnum`, dead in June) is written too, for its alpha-tag site `32d0d72d` (7c).

| June | `CX2GUElsword_SwordMan::` | vs March |
|---|---|---|
| `32d1ac60` DamageReact | DamageReact | differs: the auto guard block is rebuilt ( see below ); destruction rage also needs `!g_pMain->GetIsPlayingTutorial()`; the Way of the Sword gain is March's |
| `32cf2da0` | DamageReactStateChange | differs: a small hit starts `ESSI_SI_P_ESK_AUTO_GUARD_SMALL`; the Rune Slayer counter ( 21211, skill-or-upgrade ) with `RT_REVENGE` returns without the base call |
| `32d1a920` | ReInit | differs: the point moves 1 toward zero first ( `WSS_DESTRUCTION` +1, `WSS_VIGOR` -1 ) and a non-PvP game ends the mode too |
| `32d0d3f0` | InitComponent | the PvP reset runs for game type 1 or 4 ( `GetIsNowPVPCorrectionGame()` ) |
| `32d0d540` | InitEffect | adds the my-gage-UI refresh after the mode start |
| `32d1a5d0` | destructor | no `m_hSwordFire` stop, no `m_hFinalStrikeBlackHole` destroy ( June's layout has neither near `+0x95b0` ) |
| `32d17ea0` | HyperModeBuffEffectStart | the four boost handles only for an untranscended unit without an awakening set |
| `32d1b9d0` | ApplyBuffToHarshSlayer | rebuilt: Infinity Sword skill 21412, cool time, defender checked first, 21420 buff to the attacker when transcended |
| `32d1f690` / `32d0b870` | Set / UnSetSpecificValueByBuffTempletID | = March; the debuff reaction reads skill 21404 |
| `32d0d7a0` / `32d0d660` | Set Elsword Minor / Major ParticleByEnum | March plus `TagEffectAlphaParticleHandle` |
| `32cf5190` `32cf51d0` `32d06fd0` `32cf5530` `32cf55a0` `32cf56a0` `32d08590` `32d08ec0` `32d0d600` `32d17e90` `32d1b120` `32cf2e20` `32d27110` `32d27120` | Win, Draw, Lose, InitEnchant..., Append Major / Minor, COMMON_BUFF_FrameMove, Create Major / Minor ParticleArray, Delete Elsword Minor, DeleteMinorParticle, GetFinalDamage..., GetCombo Z / X StateID | = March ( June's state ids resolve to March's names ) |

**New (`_CORE`):** `IsHarshSlayerCoolTimeEnd` ( `32d059b0` ), `ChangeFinalDamageAsDefender` / `ChangeFinalDamageAsAttacker` ( `32d05b60` / `32d05b70`, empty, called by `GetFinalDamage` @32284d43 / @32284d62 with two float buffers ), `ApplyBuffToOpenMind` ( `32d0c780`, vslot 0x510, skill 21417, buff index 1 in a PvP or PvE game ). **New under `_LIVE`, not `_CORE`:** `GetSpecialItemUsableExtraStateID` ( `32d27130` returns `ESSI_SWORD_WAIT` ): it overrides a live virtual ( `X2Game.cpp:12326` reads it ), so `_CORE` would change the flag-off build.
**Not written:** `32d26ff0` ( destructor of the June-only struct at `+0x97c0`, two vectors ) and `32d27620` ( scalar deleting wrapper of the destructor body ).
**Auto guard.** June's block applies to a non-special hit that is not extra-damage, no-damage, healing, a dummy / sensor hit or a ground hit, with no hold / grab / riding / super armor / invincibility, the foot on a line, not in the Lord Knight counter, and the cool time over ( members `m_fAutoGuardElapsedTime` `+0x9764` / `m_fAutoGuardCoolTime` `+0x9770`, `_CORE`; June `-1` both from `InitPassiveSkillState`, ticked by `32d0c090`, CH1j ). A facing hit rolls `m_fAutoGuardRate`; a block restarts the timer, takes 100 % off the hit and calls `DamageReactStateChange`. Harsh Slayer has the twin pair `+0x971c` / `+0x9728` ( `32d059b0`, `32d1b9d0`, ticked by `32d0bdc0` ).
**Stand-ins:** the awakening effect-set present test is `_CONST_ELSWORD_CH1c_`'s ( false, Stage 12 ); `m_iBrutalSlayerActivatedLevel` is a plain int ( June `KProtectedType<int>` ); the gauge refresh in InitEffect flags `m_bChangedWayOfSwordState` and calls `UpdateUIElswordWSP` ( June vslot 0x5c `32e681d0`; the gauge UI is CH1q2 ). Bodies owned later are called by March's name: `End_Swordman*` / `Start_Swordman*` / `SetWayOfSwordPoint` ( CH1q ), `DoStateDestructionRage` ( `32d0f300` ), `CalcWayOfSwordPointOnDamageReact` ( `326106c0`, allow-listed until CH1q ).
**Enum names added** ( `X2SkillTree.h`, `JUNE_ENUM_NAMES`, checked against `Enum.src.lua` ): `RNSI_A_ERS_COUNTER` 21211, `RNSI_P_ELK_REDUCE_PAIN` 21404, `RNSI_P_EIS_HARSH_SLAYER` 21412, `RNSI_P_ERS_OPEN_MIND` 21417, `RNSI_P_EIS_INFINITY_POTENTIAL` 21420. Alpha-tag sites written: `32d0d72d`, `32d0d86d`.
No `JUNE_BUGS` row: nothing here is a June mistake ( `ApplyBuffToHarshSlayer` ending before any buff when the defender is gone is kept as June has it ).

**Routed.** CH1j: the two cool-time ticks `32d0bdc0` ( Harsh Slayer ) and `32d0c090` ( auto guard ) use the new members. CH1k: the constructor sets all four floats ( `32d09490` @`32d0a27f` / `@32d0a3d6` / `@32d0aa8d` / `@32d0abe4` ) and the members CH1h found at `+0x95a4`-`+0x95bc`. CH4a: `InitPassiveSkillState` sets the pairs and the rate, and registers `ApplyBuffToHarshSlayer` ( delegate `32d1b9d0` ). CH1o: wire the hooks `0x508` / `0x50c` ( `GetFinalDamage` ) and `0x510` ( `CX2GUUser::DamageReact` @320478c9 ) into the shared bases. CH1q / CH1q2: the Way of the Sword bodies and the gauge refresh named above.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `git diff --patience --numstat`: `.cpp` 681 / 0, `.h` 22 / 0, `X2SkillTree.h` 10 / 0. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `RESULT: PASS`.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors ( the `cl /P` output grows 21,163,526 to 21,179,524 B ).
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 25,958,912 B, dated 2026-11-04 12:14:42, md5 5695e737c2fe6a11e5464e07944e41e3, equal to X2/US_SERVICE/x2.exe (2026-11-04 12:14:35)`; built at main `2bb3d40f` + the 3 code files of this phase ( X2Lib 0 errors ). No `exestr` line: `_LIVE` is off.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-elsword CH1h review` ( Opus 5.5 high ), then CH1i.

### CH1h - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read the 33 June listings (`charport_adis.py elsword --phase CH1h`, cut at the `int3` / `ret`; `32d1ac60` runs on into `32d1b120`, `32cf5190` into `32cf51d0`) against the bodies at `f5aa5dbb`.
- **DamageReact `32d1ac60`:** the switch's byte table `32d1b100` sets the extra-damage flag for exactly 0x10 / 0x1a / 0x22 / 0x2f = `EDT_MANA_DAMAGE` / `EDT_DAMAGE_RATE` / `EDT_WATER_HOLD` / `EDT_ATTACK_ALL_TEAM` (counted in `X2DamageManager.h`). Then, in this order: `attackType == 1` (`AT_SPECIAL`), `m_bNoDamage`, the two `fcom` against 0 ( `0 < m_fHpPercentUp` / `0 < m_fIncreaseMP` skip ), `reActResult` 7 / 0xa ( `RT_DUMMY_DAMAGE` / `RT_SENSOR` ), `hitType` 0x1a ( `HT_GROUND_HIT` ), hold / grab / riding, the cool time ( `fcompp` cool vs elapsed, `test ah,0x41` + `jp` = skip while cool > elapsed ), skill 21206 ( `RNSI_A_ELK_COUNTER` ) or its upgrade, super armor ( vslot 0x264 ), invincible ( 0x180 ), `GetFrameData( false ) + 0x108` = foot on a line; then `+0x7e2d` ( `bAttackerFront`, C3b ) and `rand < m_fAutoGuardRate`. The block resets the elapsed time, wraps the base call in +100 / -100 and calls `DamageReactStateChange( data, 0xa2, -1 )`. Destruction rage: state -2, no super armor, `0 >= m_fDestructionRageTime`, `reActType` not 0 / 7 / 0xa, not invincible, HP > 0, foot, `g_pMain +0x16a == 0`; `reActResult` 6 / 0x11 ( `RT_REVENGE` / `RT_REFLECT` ) to the state change, else hold 0x26 + `32d0f300`, both falling through to the WSP gain ( vslot 0x198 own unit, `reActResult != 6`, `fPhysic + fMagic` = `GetAllDamage()` ). All = the port.
- **DamageReactStateChange `32cf2da0`:** `reActResult == 1` to 0xec ( small ) else 0xe8, `StateChange( id, true )`, return; skill 21211 or upgrade with `reActResult == 6` returns; else the base with all three arguments. **`IsHarshSlayerCoolTimeEnd` `32d059b0`** = `!( cool > elapsed )`.
- **ApplyBuffToHarshSlayer `32d1b9d0`:** riding, `attackType == 1`, cool time, `GetSkillLevel( 0x53a4, 1, 0, 1 )`, templet, `rand < SA_EFFECTIVE_RATE` ( `test ah,5` + `jp` ), factor count > 2, the defender optr ( `+0x470` ) before anything is applied; then Infinity Potential 0x53ac: hyper-class templet, `GetUnit() +0x3c == 1` ( 0 / 1 flag, so `0 !=` is the same ), attacker optr `+0xc`, factor 0 at the IP level onto the attacker; then factor 0 at the Harsh Slayer level onto the defender ( stack slot `[esp+0x1c]` is the defender once the `push 1` for `32d4d670` is counted ); then vslot 0x228 with 1 and 2; then the elapsed reset. Both `32d4d670` calls take ( unit, vslot 0x54 = `GetUnitUID()`, `bIsUserUID` 1 ), the March default.
- **ReInit `32d1a920`:** base, `m_fReDashJumpXTime = 0`; own unit: -2 adds 1.0, 2 subtracts 1.0; game type 1 ( `GT_PVP` ): RED and red > blue, or BLUE and red < blue, ends the mode, point 0, state 0; otherwise ( and outside PvP ) 2 ends with `End_SwordmanVigor` ( `32d0cd60` ) and state 1, -2 with `End_SwordmanDestruction` ( `32d0d240` ) and state -1 ( inlined once, `32cf5100` once ); then the sync `+0x7ec0`. `m_fDestructionRageTime = 0` last, for every unit.
- **Destructor `32d1a5d0`:** sound close, the inlined major delete ( base + one handle `+0x9798` ), minor delete, `g_pData +0x7c` hand fire, three effect-set stops `+0x95a4` / `+0x95a8` / `+0x95b8`, sheath mesh, the blasting loop ( `!= -1` ); no black hole. The member map holds: `+0x95ac` is the sound the destructor closes and `+0x95b0` is written by the dash-jump XZ FrameMove `32d12c50`, so no `m_hSwordFire` fits before `+0x95b8`.
- **InitComponent `32d0d3f0`:** type 1 or 4 ( = `GetIsNowPVPCorrectionGame()`, `X2Game.h:884` ) zeroes gauge `+0x118` and sets state 0, then the sync. **InitEffect `32d0d540`:** 2 to `Start_SwordmanVigor` ( `32d0c960` ), -2 to `Start_SwordmanDestruction` ( `32d0cf70` ), then my-gage-UI vslot 0x5c = `32e681d0`: gauge data ( `[ui+0x10]` ) `+0x110 = 1`, tail-jump `32e67df0`. The stand-in does the same through my unit's gauge data. **HyperModeBuffEffectStart `32d17ea0`:** skip on `GetUnit() && +0x3c != 0` or the awakening test, then the four handles `+0xcbc`-`+0xcc8` Right / Left / Right / Left.
- **Set / UnSet `32d1f690` / `32d0b870`:** tables `32d1f83c` / `32d0b950` decode to cases 3, 163, 164, 165 ( enchant off / on + `+0x41f8 = 1` ) and 19 ( Brutal Slayer 1 / 0 ); Set's debuff reaction: `+0x64` debuff, `+0x66` not world, skill 0x539c = 21404, factor 0; UnSet ends with `m_ucSaveDebuffCount = m_ucNumOfDeBuff` on every path and Set has no such write. Both = the port.
- **The rest:** Win / Draw ( `0x4b` on riding with a pet, else `0xb` ), Lose ( June's NULL-gauge path is `GetNowHp()`'s 1 ), COMMON_BUFF ( 0.83 / else 0.85, camera 30 / 0.4 ), the minor array ( draw counts -1 / 2 / 2 / 1 / 2 / 2 / -1, trigger wait each ), the alpha tags ( major `false` at `32d0d72d`, minor `true` at `32d0d86d` ), the hooks ( `ret 8` twice ), Open Mind ( 21417, index 1 for type 1 / 4, else 0 ), `32cf2e20`, Z / X / extra-state ids 0xa3 / 0xa8 / 0xce. All = the port.

**Routed (not a fix here).** `ApplyBuffToOpenMind` `32d0c780` first calls its empty base `31fc7ab0` ( vslot 0x510 ); March has no base yet, so CH1o adds the `CX2GUUser` virtual and the base call together ( `kickoffs/char-elsword/CH1o.md` amended ).

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --range f5aa5dbb^..f5aa5dbb`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h, JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `flagcheck JUNE_ENUM_NAMES --range f5aa5dbb^..f5aa5dbb --files X2Lib/X2SkillTree.h`: `RESULT: PASS`. The five new names match `Enum.src.lua`.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1h's Done note stands.

Nothing to play-test ( `_LIVE` is off ). `kickoffs/char-elsword/CH1i.md` checked: it depends on this review and needs no change. Next: `/x2-phase char-elsword CH1i` ( Sonnet 5.5 high ).

### CH1i - Done (2026-11-04)

All 4 class-map rows of phase `CH1i` are written: 3 `inside` ( June's body inside March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else` ) and 1 `new` under `_CORE` ( `IsInJobLine` ). One more `new` function was needed: the vslot 0x7a8 callee `SetEquippedSkillLevelStateData` ( `32028190` -> `3201f850` ), which March has no Elsword wrapper for. `_LIVE` stays `//#define`. 5 funcmap rows `Char Elsword CH1i`; 1 `ch1_offsets` row; `JUNE_BUGS` row 523.

| June | `CX2GUElsword_SwordMan::` | kind | vs March |
|---|---|---|---|
| `32cf5210` | SetEquippedSkillLevel ( vslot 0x7b8 ) | inside | differs: 8 blocks keyed by June skill id, each through `SetEquippedSkillLevelStateData`; no Air Slash block and no Defence block ( the Defence states 228-231 now belong to skill 21206 ) |
| `32d05960` | IsInJobLine ( vslot 0x670 ) | new `_CORE` | line 1 = classes 10 / 100, 2 = 11 / 101, 3 = 22 / 112; no unit = false |
| `32d08c30` | CanRidingState | inside | = March ( the ten state ids in another order, `ESSI_SWORD_WAIT` last ) |
| `32d1b720` | HyperModeFrameMove ( vslot 0x600 ) | inside | = March's `MODIFY_RIDING_PET_AWAKE` branch: `CommonHyperModeFrameMove( 0.05, 0.83, false )` |
| `32028190` | SetEquippedSkillLevelStateData ( vslot 0x7a8 ) | new `_CORE` | = Chung's body: level `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, power rate x `( GetSkillAdditionalPowerRate + 1 )` |

**SetEquippedSkillLevel, block by block** ( `if ( bChangeAll || id )`, templet `GetEquippedActiveSkillTemplet( id, true )` ): 21000 Mega Slash -> states 234 / 235; 21010 Windmill -> 190 / 191, then `m_fWindMillDuration` ( `+0x95a0` ) = `SA_EFFECTIVE_TIME` at the upgrade-swapped level; 21016 Sword Fire -> 192; 21205 Rolling Smash -> 202-205; 21211 Rune Slayer counter -> 259; 21206 Counter -> 228-231; 21035 Wind Blade upgrade -> 262 ( the templet is not tested before the call, the callee tests it ); 21058 Grand Cross -> 263-265. State names come from the enum order ( `Enum.src.lua` ESSI + the March header ): 228-231 are March's `ESSI_SI_A_ES_DEFENCE_*` four ( the slot between `ESSI_EIS_DASHJUMP_COMBO_XXX` 227 and `ESSI_SI_P_ESK_AUTO_GUARD` 232 ), 203 / 205 the Rolling Smash `_HYPER` pair ( `+84` from the Lua values, anchors 204 and 206 agree ), 263 / 264 read from `SetEquippedSkillFunc` `32d223e0` ( `FUN_31fd89b0( 0x107 / 0x108 )` ). The dirty-state batch June opens and closes the function with ( `3207c1a0` / `31fdf970` ) has no March form and is left out, as in Chung's.
**Quirk kept ( JUNE_BUGS 523 ).** A null `GetUnit()` inside the Windmill block returns from the whole function, skipping the six blocks after it. A user unit always has its unit, so it is latent.
**Enum names added.** `X2SkillTree.h` ( `JUNE_ENUM_NAMES`, values checked against `Enum.src.lua` ): `RNSI_SA_ES_MEGASLASH` 21000, `RNSI_SA_ELK_WINDMILL` 21010, `RNSI_SA_ERS_SWORD_FIRE` 21016, `RNSI_A_ELK_ROLLING_SMASH` 21205, `RNSI_SA_EMK_WIND_BLADE_UPGRADE` 21035, `RNSI_SA_ELK_GRAND_CROSS` 21058 ( 21206 and 21211 were there ). `X2GUElsword_SwordMan.h` ( `_CORE`, June script ids ): `ESSI_SI_A_MEGASLASH_MEMO_CHARGE` 234, `ESSI_SI_A_MEGASLASH_MEMO_FIRE` 235, `ESSI_ERS_COUNTER_GUARD` 259, `ESSI_SI1_ES_WIND_BLADE_ULTIMATE` 262, `ESSI_SA_ELK_GRAND_CROSS_CHARGE` / `_END` / `_FULL_CHARGE` 263-265 ( states owned by CH2a / CH2c / CH3a / CH3b ).
**Not written / routed.** The vslot 0x670 virtual keyword for `IsInJobLine` is CH1o's ( the body is plain, as CH1d declared it ). `SetEquippedSkillLevelStateData` is a class function here, not yet the `CX2GUUser` virtual: June calls it through vslot 0x7a8, CH1o decides whether to lift it. The Air Slash and Defence-skill March blocks stay only in `#else`.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `1 PASS~`, `RESULT: PASS`. `git diff --stat`: 155 insertions, 0 deletions.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 25,958,912 B, dated 2026-11-04 13:05:51, md5 de5ecba75a103f4af2b16cf60379dff7, equal to X2/US_SERVICE/x2.exe (2026-11-04 13:05:44); built at main 2c551fd7 + 3 uncommitted code file(s).` X2Lib 0 errors. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** ( `_LIVE` is off; nothing registers a June body ). Next: `/x2-phase char-elsword CH1i review` ( Opus 5.5 high ), then CH1j.

### CH1i - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read the June listings ( `charport_adis.py elsword 32cf5210 32d05960 32d08c30 32d1b720 32028190 3201f850` ) against the bodies at `aca4c26e`.
- **SetEquippedSkillLevel `32cf5210`:** eight blocks in the order 0x5208 / 0x5212 / 0x5218 / 0x52d5 / 0x52db / 0x52d6 / 0x522b / 0x5242 ( 21000 / 21010 / 21016 / 21205 / 21211 / 21206 / 21035 / 21058, names checked against `Enum.src.lua` ), each `bl == 1 || ebp == id`, templet `31fca8e0( id, 1 )` tested except before 0x106 ( `32cf5492` pushes `eax` untested ). Windmill: null `+0x867c` jumps to `32cf54fb` ( the batch end, JUNE_BUGS 523 ); `32890620( id, 0, 0, 1 )` max 1; `32206e50( 1, level, 0, 0.0 )` -> `+0x95a0` ( `SA_EFFECTIVE_TIME` = 1, the 4-arg form is March's 3-arg one, S2 / S7j ); `ebp` reloaded after. All = the port.
- **State names, from June's own registrations.** `SetEquippedSkillFunc` `32d223e0` pushes each id before `UpdateUserUnitStateData` ( `31fd89b0` ) with its table name: 190 / 191 `ESSI_SA_ESK_WINDMILL` / `_END`, 192 `ESSI_SA_EMK_SWORD_FIRE_LANDING`, 202 `ESSI_A_ELK_ROLLING_SMASH`, 204 `_LANDING`, 234 / 235 `ESSI_SI_A_MEGASLASH_MEMO_CHARGE` / `_FIRE`, 259 `ESSI_ERS_COUNTER_GUARD` ( 261 is `_END` ), 262 `ESSI_SI1_ES_WIND_BLADE_ULTIMATE`, 263-265 `ESSI_SA_ELK_GRAND_CROSS_CHARGE` / `_END` / `_FULL_CHARGE`. All 13 = the port.
- **The derived names hold.** 203 / 205: June registers no `_HYPER` state; 202, 204 and 206 ( `ESSI_SWORD_WAIT`, `InitStateCommon` ) are all Lua + 84, so Lua 119 / 121 ( `_HYPER` / `_LANDING_HYPER` ) are 203 / 205. 228-231: no push of 0xe4-0xe7 and no `DEFENCE` string anywhere in `32d23ff0` / `32d1f8f0` / `32d223e0`; `InitState` registers 226 `ESSI_EIS_DASH_COMBO_XXX`, 227 `ESSI_EIS_DASHJUMP_COMBO_XXX`, `InitStateCommon` 232 `ESSI_SI_P_ESK_AUTO_GUARD`, and March's header has exactly the four `ESSI_SI_A_ES_DEFENCE_*` between those two. June writes state data for four states it never registers ( `m_StateList[]` inserts them ), as the port does.
- **SetEquippedSkillLevelStateData `32028190` -> `3201f850`:** null templet / unit return; level `max( 1, 32890620( id, 0, 0, 1 ) )` -> `+4`, `32071720( level )` -> `+8`, id -> `+0xc`, `+8 *= 3201ad10( id ) + 1.0`. = Chung's body and the port.
- **IsInJobLine `32d05960`:** no unit reads class 0 ( false for every line ); 1 = 0xa / 0x64, 2 = 0xb / 0x65, 3 = 0x16 / 0x70, else false. `static_assert` probe: `UC_ELSWORD_*` = 10 / 11 / 22 / 100 / 101 / 112.
- **CanRidingState `32d08c30`:** 13 14 19 20 ( inlined protected state read ), then 76 77 75 81 82 206. Probe: March `USI_WAIT` / `WALK` / `DASH` / `DASH_END` = 13 / 14 / 19 / 20; `USI_RIDING_WAIT_HABIT` / `WAIT` / `WALK` / `DASH` / `DASH_END` = 74 / 75 / 76 / 80 / 81, June's minus one ( the shift the Arme CH1h row records ), so the names are right.
- **HyperModeFrameMove `32d1b720`:** `32d18d80( 0.05, 0.83, 0 )`; March's `bSound_` defaults to `false`.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --range aca4c26e^..aca4c26e`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER ( X2SkillTree.h, JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `flagcheck JUNE_ENUM_NAMES --range aca4c26e^..aca4c26e --files X2Lib/X2SkillTree.h`: `1 PASS~`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1i's Done note stands.

Nothing to play-test ( `_LIVE` is off ). `kickoffs/char-elsword/CH1j.md` checked: it depends on this review and char-chung Z ( closed ) and needs no change. Next: `/x2-phase char-elsword CH1j` ( Sonnet 5.5 high ).

### CH1j - Done (2026-11-04)

All 13 class-map rows of phase `CH1j` are written: 8 `inside` (June's body inside March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`) and 5 `new` under `_CORE`. `_LIVE` stays `//#define`. **Class-map correction:** CH0 listed `32d0b650` as `new ?`; it is March's `DestructionRageFrameMove`, so it is `inside` (the map rows now carry the names). 13 funcmap rows `Char Elsword CH1j`; 4 rows added to `june2017_re/char_elsword/ch1_offsets.tsv`.

| June | `CX2GUElsword_SwordMan::` | kind | vs March |
|---|---|---|---|
| `32cf2e80` / `32cf3210` / `32cf3220` | CommonStateStartFuture / CommonStateEndFuture / CommonStateEnd | inside | = March (a `jmp` to the `CX2GUUser` base) |
| `32cf2e90` | CommonStateStart | inside | two keys `DAMAGEDATA_CHANGE_TIME` / `...2` (default 0), base, then `m_bFirstAttackSuccess` (`+0x960f`) = 0 without the `SERV_SKILL_NOTE` guard |
| `32cf3230` | CommonRender_Prepare | inside | differs: `WSS_VIGOR` sets only `cartoonTexType` (`+0xc14`) = `CTT_SKYBLUE`, `WSS_DESTRUCTION` `CTT_SCARLET`; no `renderType` / `outLineColor` write |
| `32d0b4d0` | CommonFrameMoveFuture | inside | differs: only `m_fReDashJumpXTime` is counted down (CH1a review's routing); the sync field is `GetWayOfSwordState()` (vslot 0x674) as March (the CH0 amendment said `GetWayOfSwordPoint`: the listing calls vslot 0x674) |
| `32d0b650` | DestructionRageFrameMove | inside | differs: `IsSuperArmor()` returns; the body runs only while `0 < m_fDestructionRageTime`: super armor set, not-red cleared, then the countdown clamped at 0 (March counts first) |
| `32d1b2b0` | CommonFrameMove | inside | differs, see below |
| `32d0ba00` `32d0bdc0` `32d0beb0` `32d0bfa0` `32d0c090` | Brutal Slayer / Harsh Slayer / Sword Enchant / Short Time Strong Attack / Auto Guard `CoolTimeFrameMove` | new `_CORE` | `if ( 0 < cool && !( cool < elapsed ) ) elapsed += m_fElapsedTime` (June `KProtectedType<float>` written only when it changes) |

**CommonFrameMove `32d1b2b0` vs March.** No `NO_MORE_EFFECT_HARD_CODING_TEST` block (dead in March too). Sheath block as March. **Windmill effects:** the shock / freeze test (`+0x2480` EnchantShock, `+0x1730` Frozen, either above 0 stops the effects and the sound) now covers `USI_SPECIAL_ATTACK_1` ... `USI_SPECIAL_ATTACK_SLOTB_HYPER_5` (20 ids, the `_5` pair included) and `ESSI_SA_ESK_WINDMILL` / `ESSI_SA_ESK_WINDMILL_END`; every other state stops them (March's `_HYPER` Windmill states are gone). Read from the 142-byte table at `32d1b5b8` with a script: 0 at indices 0-19 and 140-141 only (`( +0x7cbc - +0x7cec ) / 100 - 50`, `GUSI_END` = 5), two targets `32d1b390` (test) and `32d1b44a` (stop). Tail: `WayOfSwordStateChange`, `DestructionRageFrameMove`, the weapon-effect refresh (`+0x41f8`, vslot 0x28c), the five ticks, then `m_fOpenMindRemainCoolTime` (`+0x9788`) counts down while `0 <= it`.

**New members (`_CORE`, plain floats, June `KProtectedType<float>`):** `m_fBrutalSlayerCoolTime` / `m_fBrutalSlayerElapsedTime` (`+0x96f8` / `+0x9704`), `m_fSwordEnchantCoolTime` / `m_fSwordEnchantElapsedTime` (`+0x9740` / `+0x9734`), `m_fShortTimeStrongAttackCoolTime` / `m_fShortTimeStrongAttackElapsedTime` (`+0x9758` / `+0x974c`), `m_fOpenMindRemainCoolTime` (`+0x9788`). **The skill pairing is inferred:** each pair is initialised in `InitPassiveSkillState` `32d1c560` right before the `GetSkillLevel` of skill 21410 / 21407 / 21411, and `+0x9788` takes ability 0x94 in `32d0c180` after skill 21415; the review checks it, CH4a confirms while transcribing the blocks.
No `JUNE_BUGS` row: nothing here is a June mistake.

**Routed.** CH1k: the constructor `32d09490` sets the seven new floats (kickoff amended). CH4a: confirm the four pairings, the readers `32d05a00` / `32d05a50` / `32d0baf0` / `32d0bc60` / `32d0c180` (kickoff amended). CH1q: `WayOfSwordStateChange` `32d0eeb0` keeps its March body until then.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `git diff --patience --numstat`: `.cpp` 182 / 0, `.h` 12 / 0.
- Flags resolved with `cl /P` on a probe: `NO_MORE_EFFECT_HARD_CODING_TEST`, `ELSWORD_SHEATH_KNIGHT`, `ELSWORD_WAY_OF_SWORD`, `BALANCE_RUNE_SLAYER_20130214`, `SERV_SKILL_NOTE` and `UPGRADE_SKILL_SYSTEM_2013` are all defined.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors (the `cl /P` output shrinks 21,288,225 to 21,285,840 B).
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `allow-listed: 0`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 25,959,424 B, dated 2026-11-04 13:43:24, md5 214378ad8374c908898d657f1723da87, equal to X2/US_SERVICE/x2.exe (2026-11-04 13:43:17); built at main 25f15da5 + 2 uncommitted code file(s).` X2Lib 0 errors. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1j review` (Opus 5.5 high), then CH1k.

### CH1j - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read the June listings ( `charport_adis.py elsword --phase CH1j`, plus `32d1c560` `32d0c180` `32d223e0` `32d082f0` ) against the bodies at `a63b9930`.
- **CommonFrameMove `32d1b2b0`, Windmill switch.** Both tables read from the exe with `charport_consts.read_va`: the 142 bytes at `32d1b5b8` are 0 at indices 0-19 and 140-141 only, every other byte is 1 ( `cc` padding follows ); the jump table at `32d1b5b0` is `32d1b390` ( index 0 ) / `32d1b44a` ( index 1 ). Index 0 is the shock / freeze test: `fldz`, `fcom` EnchantShock `+0x2480`, `test ah,5` / `jnp` -> stop when `0 < shock`; else `fcomp` Frozen `+0x1730`, `jp` past the stop when not `0 < frozen`. Index 1 stops both handles and the sound unconditionally. Ids: 50-69 = `USI_SPECIAL_ATTACK_1` ... `USI_SPECIAL_ATTACK_SLOTB_HYPER_5` ( `Enum.src.lua` `GUSI_END + 45 / + 64`, `GUSI_END` = 5; March's 20 members are contiguous with `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` ); 190 / 191 are `ESSI_SA_ESK_WINDMILL` / `_END` from June's own registration in `32d223e0` ( `push 0xbe` + `"ESSI_SA_ESK_WINDMILL"` @32d22973, `push 0xbf` + `"ESSI_SA_ESK_WINDMILL_END"` @32d229c5 ). The case labels are names, so March's different numbering does not matter. Handles `+0x95a4` / `+0x95a8` / sound `+0x95ac` ( `ch1_offsets.tsv` ), the sheath block ( `+0x9670`, `Dummy2_Lhand` ) and the tail order ( `32d0eeb0` WayOfSwordStateChange, `32d0b650`, `+0x41f8` -> vslot 0x28c, the five ticks, `+0x9788` ) all match the port. Open Mind: `test ah,0x41` / `jp` skips when `0 > it`, so it counts down while `0 <= it`; the `fucompp` after it is only KProtectedType's write-when-changed.
- **Five ticks `32d0ba00` / `32d0bdc0` / `32d0beb0` / `32d0bfa0` / `32d0c090`:** `fldz; fcomp cool; test ah,5; jp` -> out unless `0 < cool`; `fld elapsed; fld cool; fcompp; test ah,1; jne` -> out when `cool < elapsed`; then `elapsed += +0x2b0`. Pairs `+0x96f8/+0x9704`, `+0x9728/+0x971c`, `+0x9740/+0x9734`, `+0x9758/+0x974c`, `+0x9770/+0x9764`. = the port.
- **Cool-time pair names hold.** In `32d1c560` each pair is set to -1.0 ( `334380f0` ), then the skill's level is read and its ability 0x94 is stored in the cool member and copied into the elapsed one: `+0x96f8` after 0x53a2 = 21410 `RNSI_P_ETK_BRUTAL_SLAYER` ( @32d1ce0a ), `+0x9740` after 0x539f = 21407 `RNSI_P_ERS_SWORD_ENCHANT` ( @32d1d59e ), `+0x9758` after 0x53a3 = 21411 `RNSI_P_ETK_SHORTTIME_STRONGATTACK` ( @32d1d830 ). `32d0c180` stores ability 0x94 of 0x53a7 = 21415 `RNSI_P_ELK_OPEN_MIND` into `+0x9788` ( @32d0c4c2, through the pointer saved @32d0c1b9 ). No rename.
- **DestructionRageFrameMove `32d0b650`:** vslot 0x264( 0 ) ( = `IsSuperArmor()`, as every earlier row reads it ) returns; `0 < +0x9618` else return; protected bool `+0x7ff4` written 1 when not already; `+0x8040` = 0; `rage - elapsed` clamped at 0 and stored. = the port.
- **CommonRender_Prepare `32cf3230`:** `GetWayOfSwordState()` 2 -> `+0xc14` = 0xb, -2 -> 0xc, then the base. `+0xc10` is `renderType`: June's Violet Mage `32cc1c50` writes `RT_CARTOON_FADE` there ( March `X2GUArme_VioletMage.cpp` `m_RenderParam.renderType` ), so `+0xc14` is the next member `cartoonTexType`. `WSS_VIGOR` = 2, `WSS_DESTRUCTION` = -2, `CTT_SKYBLUE` 11, `CTT_SCARLET` 12.
- **CommonStateStart `32cf2e90`:** two `GetValue` calls into `+0x9624` / `+0x9628`, each `fldz; fstp` on a miss, the base, then `+0x960f` = `bl` ( `xor bl,bl` @32cf31ca ). `+0x960f` is `m_bFirstAttackSuccess`: the only other writers in the class are the dead memo body `32d082f0` ( memo 0x92ba9, `0.6f`, `= 1` then `== 0` test, March's Fatal Fury pattern ) and a 0 at `32d0b2e6` ( constructor, CH1k ).
- **CommonFrameMoveFuture `32d0b4d0`:** only `+0x9598` `m_fReDashJumpXTime` counted down and clamped at 0, the base, then `+0x7ec0` = vslot 0x674 `GetWayOfSwordState()` ( an int compare before the write ). Thunks `32cf2e80` / `32cf3210` / `32cf3220` jump to the three bases. = the port.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --range a63b9930^..a63b9930 --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. Worktree run: nothing to check ( clean ).
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe ( `census.cl_cmd`, `/c /Zs /EHsc`, no PCH ): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1j's Done note stands.

Nothing to play-test ( `_LIVE` is off ). `kickoffs/char-elsword/CH1k.md` checked: it depends on this review and char-chung Z ( closed ), and CH1j's amendment ( the constructor sets the seven floats ) is in it. Next: `/x2-phase char-elsword CH1k` ( Sonnet 5.5 high ).

### CH1k - Done (2026-11-04)

All 15 class-map rows of phase `CH1k` are written: 9 `inside` (June's body inside March's own `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's in `#else`; the constructor needs no `_LIVE` branch, see below) and 6 `new` under `_CORE`. `_LIVE` stays `//#define`. **Class-map corrections** (CH0 had them as `inside` or `new ?`): `32cf5760` / `32d1bd10` / `32d0d8e0` are `new` `SpecialAttackUse` / `SpecialAttackLookup` / `SpecialAttackCooldown` (Chung's names for steps 511 / 510 / 512); `32d00fe0` is March's `SetSpecificValueByEnchant`, `32d09050` is `ShowActiveSkillCutInAndLight` and `32d1dbd0` is `ProcessEnduranceSkill` (all `inside`); `32d08d20` is `IsNowStateAutoGuard`. 15 funcmap rows `Char Elsword CH1k`; 13 rows added to `ch1_offsets.tsv`. Four names are registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES`: `RNSI_A_ES_ROLLING` 21200, `RNSI_A_ES_ENDURANCE` 21201, `RNSI_A_ES_ROLLING_UPGRADE` 21217, `RNSI_A_ES_ENDURANCE_UPGRADE` 21218 (June `Enum.src.lua` 14483 / 14928 / 14411 / 14785).

| June | `CX2GUElsword_SwordMan::` | kind | vs March |
|---|---|---|---|
| `32cf3290` | SpecialAttackEventProcess | inside | June's: lookup (vslot 0x7f8), use (0x7fc) + `Send_SKILL_USE_REQ`, then the 3 / 5 use-condition give-back and `+0x88bc` = slot + 1. = Chung's `32bd8790` |
| `32d1bd10` / `32cf5760` / `32d0d8e0` | SpecialAttackLookup / Use / Cooldown | new | the three steps March had in one body, see below |
| `32d0b5a0` | SpecialAttackNoStageChange | inside | skills 21201 / 21218 set `UAI_ENDURANCE`; March's Reduce Pain case is gone |
| `32d1ddf0` / `32d1dbd0` | NoStateChangeActionFrameMove / ProcessEnduranceSkill | inside | base `32008e40` first; action 1 and 11 show the skill name (21201); the helper differs from March's |
| `32d00fe0` | SetSpecificValueByEnchant | inside | skill 21404 Reduce Pain's factor 0 instead of Induration of Revenge's |
| `32d09050` / `32d09270` | ShowActiveSkillCutInAndLight / ...ByScript | inside | event 0x8b3 cut-ins; the second is = March |
| `32cf5660` / `32d09490` | InitializeElswordMinorParticleArray / constructor | inside | = March |
| `32d08b30` / `32d08d20` / `32d08d70` | IsNotCommonMoveState / IsNowStateAutoGuard / AfterImageFrameMove | new | see below |

**Where June differs from March** (checked against the listings, `charport_adis.py elsword --phase CH1k`):
- **Lookup `32d1bd10`** (the checks March had in `SpecialAttackEventProcess`): quick-slot wait, `CommonSpecialAttackEventProcess`, equipped templet (its id must equal the slot's, else string 0x8bae), then `CheckSkillUseCondition`, `CheckSkillUsePVPOfficial`, `3202d140`, `3202d3c0`, the riding-key wait, and **the auto guard gate** (`IsNowStateAutoGuard` and the skill is not 21200 / 21217 = no skill). MP = `GetActualMPConsume`, **minus `m_iBrutalSlayerActivatedLevel` x `m_fMindOfFighterConsumeMpRate` x MP for an active skill (type 6)**, **x 0.7 in Vigor for a Vigor-active skill** (`m_iWOSType` 2). Then the dev gate (`IsDevUnlimitedSkillUse()`; vslot 0x82c is always false) around the cool time (226), MP (2549) and finality stone (26119; the training game is the only exemption here). Gone: the skill-tree-learning message (25110), the Rolling HP check (2627), `ReducePain`, `CountUsedSkill`, `SERV_SKILL_USE_SUBQUEST`'s send (now in 0x7f4).
- **Cooldown `32d0d8e0`:** `MarkSkillSlotUsed`, `UpNowMp`, the finality stone (training or photo zone), the cool time, the random-mission hook `31fcd3d0`, then `ApplySkillCoolTimeRateByBuff`. **June reads the skill level twice, drops it and asks `GetActualCoolTime( templet, 1 )`** (JUNE_BUGS 524). March's memo 22 / memo 6 cool time cuts, the Compact Counter cool time and `ST_ACTIVE` x `m_fIncreaseCoolTimeRel` are not in it (routed to CH4b, `GetActualCoolTime` `32cf4df0`).
- **ProcessEnduranceSkill:** an upgraded Endurance (`m_iDeleteSkillIDByUpgrade`, +0x178) erases every debuff first (`GetBuffFactorOnlyDebuff`); buff factor 0; Infinity Potential (21420) learned adds its factors 1 and 2 (`SetSkillBuffFactors`); no memo 6 branch. March's differs, so the `june_march_divergent.tsv` row `ProcessEnduranceSkill` was added with the replacement in this commit.
- **Constructor `32d09490`:** same defaults as March. June adds the eleven cool-time floats (-1.0, `m_fBrutalSlayerElapsedTime` 0.0) and no re-attack members, `m_hSwordFire` or `m_hFinalStrikeBlackHole`; those March members stay initialised (the March-bodied states read them). The floats and `m_bAfterImageSaved` join the init list under `_CORE`. Eight June-only members have no March name (`ch1_offsets.tsv`: `+0x96f4`, `+0x9710`, `+0x977c`, `+0x9794`, `+0x97b8`-`+0x97dc`), none has a reader yet. `+0x7d10` (`MemoCondition`, `new` of 12 bytes @32d0b47a) is CH4c's.
- **`IsNotCommonMoveState` / `AfterImageFrameMove`** override base virtuals, so they live under `_LIVE` (a `_CORE` declaration would override at once): the sword wait state (future) and `ESSI_DASH_JUMP_POWER_LANDING` (now) end the base test; the after-image is hidden in the wait and sword wait states and restored after (Arme's shape; June's second after-image `+0x6758` is not ported in March).
- **ShowActiveSkillCutInAndLight:** `EventTimerOneshot` gate, the cut-in copy of the set, then event 0x8b3 swaps in `HQ_CutIn_Elsword_CASH_21` / `_22` by `rand()` parity (names as Chung's, assets staged by CH0b); the customize cut-in override `31fd8a50` is not ported (Stage 12).

**Routed.** CH4b: the cool time / MP extras above; check `32cf4b70` for the Vigor x0.7 rule (the lookup applies it now). CH4c: the constructor's `MemoCondition`. CH1p: June's use path never calls `CountUsedSkill`. Engine (near R12c): the second after-image (`EnableAfterImage` mode 1 `32275a30`, `EraseAfterImageByBuff` `3227a2f0`); cosmetic, not a phase.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define`. `git diff --patience --numstat`: `.cpp` 369 / 0, `.h` 9 / 0, `X2SkillTree.h` 9 / 0. `flagcheck JUNE_ENUM_NAMES` on `X2SkillTree.h`: `RESULT: PASS`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): `X2GUElsword_SwordMan.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`: exit 0, 0 errors. `C_ASSERT` probe: `UAI_ENDURANCE` 1, `UAI_REDUCE_PAIN` 10, `UAI_ENDURANCE_AND_REDUCE_PAIN` 11 (June's switch values), `ST_ACTIVE` 6, `ST_HYPER_ACTIVE_SKILL` 13, `WST_VIGOR_ACTIVE` 2.
- `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h`: `allow-listed: 0`, `RESULT: PASS`; `--selftest` PASS. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 25,960,960 B, dated 2026-11-04 14:34:22, md5 007053d95c02f9a4163bb43cc32fa1dd, equal to X2/US_SERVICE/x2.exe (2026-11-04 14:34:15); built at main 60493834 + 3 uncommitted code file(s).` X2Lib 0 errors. No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` is off; nothing registers a June body). Next: `/x2-phase char-elsword CH1k review` (Opus 5.5 high), then CH1m.

### CH1k - Review (2026-11-04, Opus 5.5 high)

**Verdict: PASS, no code change.** I read all 15 June listings (`charport_adis.py elsword --phase CH1k`) against the bodies at `bba0762a`, plus Ghidra for `32216320` (the SkillTemplet loader), `333abf90` (cut-in static initialiser) and `32205af0`.
- **Lookup `32d1bd10`.** The check order is as ported: `+0x8d78` > 0 (`fcomp`, `test ah,5`, `jnp`), `CommonSpecialAttackEventProcess`, the templet id against the slot's (else 0x8bae, font 3, (1,1,1,1) / (0,0,0,1), centre), `m_iChosenSkillID` stored, `CheckSkillUseCondition( arg, +0x98 )`, `CheckSkillUsePVPOfficial`, `3202d140` / `3202d3c0` (= `CheckSkillUseNotPve` / `NotDungeon`), `329cc370`, then `IsNowStateAutoGuard` unless 0x52d0 / 0x52e1 (21200 / 21217). MP: `GetSkillLevel( id, 0, 0, 1 )` clamped to 1, vslot 0x6a0 `( id, level, 1 )` (= March's two-argument overload, which passes `true`), type 6 -> `mp - fild(+0x96a8) x +0x969c x mp` (`fmul st(1)`, `fsubp st(1)`). Then 226 when `0 < cool` (`+0x24`, `jp`), 2549 when `reserved + chosen` is not `<=` now MP (`test ah,0x41`), 26119 for type 13 outside `g_pMain +0x8c` 0x12 with `GetNumItemByTID( 130208 )` <= 0; vslot 0x82c true skips all three. The tail writes id and slot.
- **`+0x160` is `m_iWOSType`.** In `32216320` the decoded key at `3379e514` reads `m_usWOSType` (`eb d5 b0 df b6 39 1b c4 57 ae 00` xor the inline bytes) and is stored through `lea esi, [ebp+0x164]`. The loader's base is the templet minus 4: the `edi` copy writes the guild byte through `[edi+4]` after testing the type at `[edi+8]` (types 9-11), and `[ebp+0x178]` is the `m_ePowerRate` store that `32205590` reads at templet `+0x174`. So the key lands at templet `+0x160`. Aisha's memorize test `32205af0` reads the same member (map node `+0x170` = templet `+0x160`, == 3). The Vigor rule is on the right skills.
- **Cooldown `32d0d8e0`.** IsMyUnit (vslot 0x198), templet / unit null return, `MarkSkillSlotUsed( slot, 0 )`, vslot 0xf0 with `-mp`, type 13 and not `31fd0570` -> `GetItemByTID( 130208 )` -> `Handler_EGS_USE_FINALITY_SKILL_REQ`. Both `GetSkillLevel` results die: `eax` is overwritten by `mov eax, [edx+0x6a4]` @32d0d9c4 before `push 0; push 1; push edi`. JUNE_BUGS 524 holds. Then `SetSkillCoolTimeLeft( id, cool, 1 )` (= March's default `bUpgradeSwap_`), `31fcd3d0` (= `OnSkillUse`, funcmap Quest RM1), `3235a710` on the unit. = the port.
- **0x7f4 `32cf3290` / use `32cf5760`.** Saved `+0x8ea8` / `+0x8ea4` / `+0x8ea0`; a 3 / 5 templet (`+0x98`) restores all three whether or not the skill was used; `+0x88bc` = slot + 1 only when used. Use: vslot 0x810 true -> 0x800 `( +8, mp )`, 0x764 `( arg id, 1, mp )`, `+0x8eb0` = 0; false -> 0x804 `( +8 )` + `31fd32e0`; `+0x8ac4` = 1 on both paths. = the port.
- **NoStateChangeActionFrameMove `32d1ddf0`:** `32008e40` first, then the action at `+0x8124` (`movzx ax`): `dec` -> 1, `sub 9` -> 10, `dec` -> 11. The 21201 name follows 1 and 11 only. **ProcessEnduranceSkill `32d1dbd0`:** templet 21201 (null -> out), `+0x178` != 0 -> `GetBuffFactorOnlyDebuff`, erase `[elem+0x20]` (`3229e160`), `edi` zeroed again before vslot 0x228 `( templet, 0 )`. Probe: `offsetof( KBuffFactor, m_BuffIdentity )` = 0x20, `sizeof( vector<int> )` = 16. `31fceab0( 21420 )` > 0 -> vslot 0x23c `( 21420, 1, 1 )` / `( 21420, 2, 1 )`, then the effect set and sound. **SpecialAttackNoStageChange `32d0b5a0`:** 0x52d1 / +0x11 -> future action `+0x7e84` = 1.
- **SetSpecificValueByEnchant `32d00fe0`:** `+0x96f0` > 0, `GetApplyWeaponExtraDamageType()` != 0, skill tree non-null, `GetSkillTemplet( 0x539c = 21404 )`, vslot 0x228 `( templet, 0 )`. = the port.
- **Constructor `32d09490`.** Protected floats in order: `+0x96f8` -1.0 (`[esp+0x58]`, block ends `xor eax,[esp+0x58]` @32d09f57), `+0x9704` 0.0 (`fldz` @32d09f6a), `+0x9710` (unnamed), then -1.0 (`334380f0`) at `+0x971c`, `+0x9728`, `+0x9734`, `+0x9740`, `+0x974c`, `+0x9758`, `+0x9764`, `+0x9770`, `+0x977c` 0 (`mov [esp+0x60], 0`), `+0x9788` -1.0. = the init list. `+0x95bc` is written twice (@32d0951f, @32d0b168) and `+0x95c0` by `fst` @32d0b365: a handle and a float, as March's `m_hDoubleSlash` / `m_fMagicReflectRate`. `32cf5660`: 7 handles `+0x979c`-`+0x97b4` = -1 after the base.
- **IsNotCommonMoveState `32d08b30` / AfterImageFrameMove `32d08d70` / IsNowStateAutoGuard `32d08d20`:** future state `+0x7e6c` (16-bit compare) != 0xce, now state (`+0x7cbc` decode, `/100`) != 0x9a; after-image hidden at now state 13 / 0xce (`+0x194` copied to `+0x1d1`, then 0), else restored from `+0x1d1` when off. The port's `m_bAfterImageSaved` stands in for `+0x1d1` and is never cleared, the same as June; `OnFrameMove( +0x2ac, +0x2b0 )`. Auto guard: 0xe8 or 0xec-0xee. The names match June's registrations in `ch0_state_skill.tsv` (206 `ESSI_SWORD_WAIT`, 154 `ESSI_DASH_JUMP_POWER_LANDING`, 232 / 236 / 237 / 238 the four Auto Guard states, 13 the wait). March's compiled values differ (probe: 163, 108, 188 / 228 / 229 / 230; `USI_END` 106) because the port compares names and CH1m's registration sets the ids. CH1m must register by name, never by June's number.
- **Cut-in `32d09050` / `32d09270`:** index `( sub + 3 x set ) x 0x30` from `337aecc0`; `337aec60` <- `33525c18` `HQ_CutIn_Elsword_CASH_21`, `337aec90` <- `33525be4` `_CASH_22` (initialiser `333abf90`), size 512 x 512 and position 256, 512 (`334367ac` / `33435598`), = March's one-argument `SkillCutIn` default. Even `rand()` -> `_21`. `32d09270` passes `+0x8acc`. = the port.
- **Enums / signatures** (subagent greps, spot-checked): `WSS_VIGOR` 2, `WST_VIGOR_ACTIVE` 2, `USI_WAIT` 13, `RNSI_P_ELK_REDUCE_PAIN` 21404, `RNSI_P_EIS_INFINITY_POTENTIAL` 21420, `ATI_HYPER_SKILL_STONE` 130208, `EDT_NONE` 0, `XUF_DODUM_20_BOLD` 3. `USI_WAIT` and `WSS_VIGOR` also confirmed by the `C_ASSERT` probe.

**Hygiene.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --range bba0762a^..bba0762a`: `PASS` `.cpp` +369 and `.h` +9, `OTHER` `X2SkillTree.h` (belongs to `JUNE_ENUM_NAMES`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` only because `_LIVE` is a `//#define` (Always.h:11466). `flagcheck JUNE_ENUM_NAMES --range bba0762a^..bba0762a --files X2Lib/X2SkillTree.h`: `PASS~`, `RESULT: PASS`. `--worktree`: nothing changed, nothing checked.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH): exit 0 and 0 errors, both with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`.
- No build: nothing changed. The `DEPLOY OK` of CH1k's Done note stands.

Nothing to play-test (`_LIVE` is off). `kickoffs/char-elsword/CH1m.md` checked: it depends on this review and registers from `ch0_state_skill.tsv` by name. Next: `/x2-phase char-elsword CH1m` (Opus 5.5 high).

### CH1m - Done (2026-11-04)

Both class-map rows are written, both `inside`: June's bodies sit in March's own `InitState` (`32d23ff0`, vslot 0x7a4) and `InitStateCommon`
(`32d1f8f0`) under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, with March's bodies in the `#else` (still `//#define`d). Elsword has no
`InitStateByUnitClass`: June's class tests sit in `InitState` after START / WIN / LOSE, as March's do. Class map: `32d1f8f0`'s March pair corrected to `InitStateCommon`.

**How it was derived.** `june2017_re/char_elsword/ch1m_regsim.py` (Chung's, with Elsword's class registers `local_68` / `uVar4` / `iVar6` and the
21216 test switchable by `CH1M_21216=1`) walks fresh decompiles once per class; its output equals CH0's listing-checked `ch0_reg.tsv` row for row
(callbacks included) under `ch0_build_state_skill.py`'s GATES, all 7 classes. `ch1m_reg_by_class.tsv` is that output (32d1f8f0 on both 21216 paths).
`ch1m_gen.py` writes the C++: ids by table name through March's own registration of the table (else the `ESSI_` member of that name), callbacks from
`funcmap.tsv`, else CH0's `inside` member, else CH0's `pair` for a shared target (the machine-gun and magic-spear states keep March's
`CX2GUUser::MACHINE_GUN_*` / `SUMMON_MAGIC_SPEAR_*`). `ch1m_regcheck.py` parses the `_LIVE` branches per class and per 21216 path:
**1682 registrations match slot by slot, 0 problems; 154 June ids map one-to-one both ways** (`ch1m_idmap.tsv`).

**InitState `32d23ff0`:** cut-in 1 / 10 / 11 / 22 -> 0, 100 -> 1, 101 -> 2, 112 -> 3 (March's values; no default write). InitStateID is inlined
@32d24080-32d2409e with four writes only (`ESSI_MACHINE_GUN_WAIT`, `USI_WAIT` x2, `USI_DAMAGE_AIR_FLY_FRONT`, = June 280 / 13 / 13 / 40 by name), so
March's `InitStateID` gets that body under `_LIVE`: March's six `CLIFF_CLIMBING_TEST` climb writes are dropped (June registers no climb state; the
readers are the climb states' own callbacks). Then `InitStateCommon`, `InitStateCommonRandom` (3202b110 + 31fe22d0), START (Start / EventProcess / End),
WIN (`WinStateStart`, Elsword's `WinFrameMove`, `RandomLoseEventProcess`), LOSE (`LoseStateStart`), `InitWaitStateChange( "ESSI_WAIT_STATE_CHANGE" )`
@32d2446a (7h), the `ESSI_EMK_WIN2` test (`+0x960c`), five class tests, vslots 0x690 / 0x684( 0 ) / `31fd8f00`. `31fc93d0` (empty stub) is not called.
Class tests, written as one `switch( eClass )` each: 112 (`ETK_COMBO_ZZZ` .. `ZZZZZ`); 22 / 112 (`ETK_COMBO_XX` .. `XXX_END`); @660 1 / 10+100 / 11+101 /
22 / 112 (112 falls into 22 in June, written out whole); @824 1+10+22 / 11+101 / 100 / 112; 100 / 101 / 112. Per class 7 / 17 / 15 / 11 / 22 / 23 / 18
registrations incl. the four head ones. Ids come from March's own registration of the table, except the 25 June-only states (224, 236-258, 260), which take the `ESSI_` member of the table's name.

**InitStateCommon `32d1f8f0`** (119 per path, no class test): hyper id 23 = `GetHyperModeTableName( false )` (Elsword's ELK / ERS / EIS cases were
already there), FrameMove = Elsword's `HyperModeFrameMove` (vslot 0x600), End = `CX2GUUser::HyperModeEnd`. Bounding = `InitBoundingStates( "ESSI_" )`
at June's place (7l). **Id 162 @32d20f9c:** base skill level of 21216 < 1 registers `ESSI_REVENGE_ATTACK`, else the `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK`
table (March tested 1413 `SI_P_EIS_COMPACT_COUNTER`); June reads `*( +0x867c ) + 0x494` with no null test, as March. `RNSI_A_EIS_COMPACT_COUNTER = 21216`
registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES`. June's 162 compact counter has no FrameMoveFuture (March had one). `ESSI_SI_P_ESK_AUTO_GUARD*` (232 / 236-238),
`ESSI_SWORD_WAIT`, `ESSI_DESTRUCTION_RAGE`, `ESSI_WALL_LANDING`, the machine-gun states and `ESSI_LEAP` are unconditional. `ESSI_RIDING_START` = `GetRidingStartStateID()` (June 72).
March-only states June does not register are dropped (climb, link-jump, the `_HYPER` and Mega Slash states CH0 counted). The 5 `USI_EVENT_*` states of
this phase are 3202b110's, run by `InitStateCommonRandom`'s head.

**Left out, each a comment line with its owner:** phoenix run 123-126 (PvP; June names 124 / 125 `..._PHOENIX_RUN` too), dance 127-129 (Stage 10),
riding 93-96 (Stage 17; June names 93 `ESSI_RIDING_HYPER_MODE`), the empty slots `3202a7d0` (die end, in `StateChange`) and `32002f70` (Stage 17).
**JUNE_BUGS 525:** `ESSI_RIDING_DAMAGE_BACK` registered with no script table; the script defines `ESSI_RIDING_DAMAGE_FRONT` twice (Elsword's twin of 489).
`ESSI_ELK_XXXXXX` (no table) is row 521.

**The `_LIVE` gate (`Always.h`):** `#error` guards for `JUNE_ENUM_NAMES`, `JUNE_CHAR_ARME_CORE`, `JUNE_CHAR_EVE_CORE`, `HYPER_MODE_FIX`,
`JUNE_COMMON_RANDOM_STATE`, `JUNE_EVENT_STATES`, `JUNE_WAIT_STATE_CHANGE`, `JUNE_BOUNDING_STATE`; the `_LIVE` probe passes them all (each defined there, `cl /P`).
For CH1n: `june2017_re/char_elsword/ch1m_user_callbacks.tsv` lists the 157 `CX2GUUser` members the registration names (kickoff amended).
Checklist: 7h, 7l and the 7ao hyper-table bullet marked `(Elsword: CH1m)`.

**Checks.** One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`) of `X2GUElsword_SwordMan.cpp`, `_LIVE` off and on: exit 0, 0 errors each. `cl /P`: with `_LIVE`
the `.i` has `InitWaitStateChange( "ESSI_WAIT_STATE_CHANGE" )`, `InitBoundingStates( "ESSI_" )`, `GetHyperModeTableName( false )` once each; without it none.
`git diff --patience --numstat`: `X2GUElsword_SwordMan.cpp` 1421 / 0, `Always.h` 24 / 0, `X2SkillTree.h` 6 / 0.
`flagcheck.py JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 201 deleted line(s), 9 non-ASCII inserted line(s), 33 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
RESULT FAIL only for the `//#define`. The 201 / 9 / 33 are the default diff misaligning the insertion against March's identical lines (patience 1421 / 0;
the file's LF-only count 145 and high-byte count 5598 are unchanged). `flagcheck JUNE_ENUM_NAMES --worktree --files X2Lib/X2SkillTree.h`: `RESULT: PASS`.
`junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 25,960,960 B, dated 2026-11-04 15:29:53, md5 cac905c434a40028b5aac44d7c380e20, equal to X2/US_SERVICE/x2.exe (2026-11-04 15:29:47); built at main e1fcbeb4 + 3 uncommitted code file(s).`

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-elsword CH1m review` (Opus 5.5 high).

### CH1m - Review (2026-11-04)

**Verdict: passes after one fix.** Id 162's skill test dropped June's upgrade swap. June calls `32890620( 0x52e0, 1, 0, 1 )` @32d20f8c-32d20f9c.
The 4th argument runs `328905b0`, which reads the level of 21236 `RNSI_A_EIS_COMPACT_COUNTER_UPGRADE` when that skill is learned. The port passed
`( id, true )`, so the swap was off. Now `GetSkillLevel( RNSI_A_EIS_COMPACT_COUNTER, true, 0, true )` under `_LIVE`, the form of the 76 other
June call sites. The 4-argument overload is `SERV_IRUHADEV_JUNE_SKILL_TEMPLET`'s, so `Always.h` gets a 9th `_LIVE` guard for it.
`JLE` on `cmp eax, ebx` (ebx 0) = `< 1`; `*( +0x867c ) + 0x494` with no null test; `RNSI_A_EIS_COMPACT_COUNTER = 21216` = June's `Enum.src.lua`:15739.

- **Simulation.** Fresh decompiles of `32d1f8f0` (1767 lines) / `32d23ff0` (1164) through `ch1m_regsim.py`, plain and `CH1M_21216=1`, equal
  `ch1m_reg_by_class.tsv` row for row (833 / 833 / 113 / 113). They differ only at id 162 @1184. The one `UNKNOWN COND` is `piStack_70` @1121, a
  shared_ptr release after the last registration. `ch1m_regcheck.py`: `matched 1682 registrations ( both 21216 paths ), 0 problems; ids mapped 154`.
  `ch1m_idmap.tsv` is one-to-one both ways, and June 72 = `GetRidingStartStateID()`.
- **Head of `32d23ff0`, from the exe bytes.** `31fc93d0` is called first (@32d2402c), but its body is a bare `RET`, so leaving it out is right.
  Cut-in table `32d2583c` / index `32d25850`: 1 / 10 / 11 / 22 -> `+0x8ac8 = 0`, 100 -> 1, 101 -> 2, 112 -> 3, the other 105 values skip.
  Then exactly four writes: `+0x7d00 = 0x118`, `+0x7d04 = 0xd`, `+0x4088 = 0xd`, `+0x408c = 0x28`. No climb write, and no flag test before the
  calls `32d1f8f0`, `3202b110`, `31fe22d0`. START (9): slots 2 / 6 / 8 thunk to `31ffbd70` / `3201b390` / `3201b460`. WIN (0xb): 2 / 4 / 6 =
  `320004b0` / `32d07090` (Elsword) / `320718d0`. LOSE (0xc): 2 = `320007e0`. 0x82 `"ESSI_WAIT_STATE_CHANGE"`: 6 = `31fc8620`. These are the same
  shared targets as Chung's review. `ESSI_EMK_WIN2` compares the WIN table name (`[esp+0x78]`), and on a match sets `+0x960c = 1`. The tail
  @32d25740 is vslot 0x690, vslot 0x684( 0 ), `31fd8f00`.
- **Class tests.** @310 is `== 0x70`. @460 is `0x16` or `0x70`. @660 (table `32d258c0` / `32d258d8`): 1 / 10+100 / 11+101 / 22 / 112; 112
  (`32d24fab`, `EIS_DASH_COMBO_ZX`) falls into 22 (`32d24ff1`). @824 (table `32d25948` / `32d2595c`; Ghidra's listing skips these bodies, so
  capstone read them from the exe): 1+10+22 -> `DASH_JUMP_COMBO_X2` (0xb8); 11+101 -> X2 + `MAGIC_KNIGHT_..._X2a` (0xb9); 100 -> `KNIGHT_..._X2` +
  `_X3` (0xbb, EventProcess only); 112 -> `EIS_DASHJUMP_COMBO_XX` + `_XXX` (0xe3). Slots match the tsv once `320775a0` (`Init`, which clears
  +0x14..+0x5c) is taken into account. @941: `sub 0x64` -> 100, `dec` -> 101, `sub 0xb` -> 112.
- **Shared callbacks.** June's machine-gun / magic-spear targets are `31f*` `CX2GUUser` bodies, as March's `CONVERSION_VS` branch is.
  `MACHINE_GUN_WAIT_Start` `31fc7ed0` is vslot 0x7c4 (`CommonStateStart`). `SUMMON_MAGIC_SPEAR_FrameMove` `31ffba50` is the world colour
  0.1333 (= 0x22 / 255) and then the 0.55 timer: both equal March's.
- **Left out / Always.h.** Every left-out state has its owner line (phoenix 123-126, dance 127-129, riding 93-96, `3202a7d0`, `32002f70`), and
  JUNE_BUGS 525 holds. Each of the eight guards names a flag the registration calls (`HYPER_MODE_FIX` is the studio's flag).

**Hygiene.** CH1m range `flagcheck JUNE_CHAR_ELSWORD_LIVE --range bc55bd45^..bc55bd45 --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`:
`files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `201 deleted line(s), 9 non-ASCII inserted line(s),
33 EOL-mismatched inserted line(s)`, RESULT FAIL only for the `//#define`. Patience numstat confirms 1421 / 0, 24 / 0, 6 / 0, and its inserted lines
hold no non-ASCII byte. Review fix `--worktree`: `files: 2 PASS ... 0 FAIL`; `2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched`.
The 2 deleted lines are the CH1m `_LIVE` lines replaced in place. RESULT FAIL only for the `//#define`. bytepatch: LF-only 145 / high bytes 5598
unchanged. `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
One-file probes (`/c /Zs /EHsc`): `off exit 0 errors 0`, `on exit 0 errors 0`.
`DEPLOY OK: X2_offline.exe is 25,960,960 B, dated 2026-11-04 16:00:48, md5 4069ddf6ea18fda40e8a2cdb5c30ae6c, equal to X2/US_SERVICE/x2.exe (2026-11-04 16:00:41); built at main bc55bd45 + 2 uncommitted code file(s).`

**Nothing to play-test** (`_LIVE` is off). CH1n's kickoff stands as written. Next: `/x2-phase char-elsword CH1n` (Opus 5.5 high).

### CH1n - Done (2026-11-04)

**Head (`32d223e0`), inside, `_LIVE`.** June's head sits in March's own `SetEquippedSkillFuncBySkillSlotIndex` up to the id switch; March's head
and type switch are the `#else`. Read from the decompile, the listing and the exe bytes (Chung's CH1n is the template):
- Bounds `0 > slot || 4 < slot`; tables `33521204` / `3352132c` (normal A / B = 50.. / 60..) and `33521318` / `33521340` (hyper 51.. / 61..) = March's
  four arrays (read from the exe). `31fd2af0` dropped (7i), `+0x11` = `IsSkillSlotState`, hyper `0x101`; `GetEquippedActiveSkillTemplet( id, true )`.
  The batch `3207c1a0` / `31fdf970` is anti-tamper only, as Chung's.
- **Transcendence memo @32d224e4:** `31fdbcc0`, then `32206450( memo, 1 )` non-null -> straight to the end. Elsword's memo switch has **no case**, so a
  transcended skill registers nothing (Chung's had five). Checklist §7b marked.
- **Types** (jump table `32d23de0` / `32d23df0`): 5 buff = `ESSI_SI_ES_COMMON_BUFF` on **both** slots (March's hyper used `..._HYPER`; June has no
  such name here), FM `32a77a20` = vslot 0x5f0 (Elsword's override `32d08590`, CH1h), EP `31fcbd60`; 6 / 7 / 0xd-0x10 = the hyper switch, then the id
  switch; 0xc + 900 = `ESSI_THROW_ITEM`, FM / EP `329cc0d0` / `329cc170` = vslots 0x5f4 / 0x5f8; anything else returns.
- **Hyper switch:** 21600 / 21603 Gigantic Slash (Init `32cf4930`, EP `31fcc740` = `GenericSpecialActiveSkillEventProcess`), 21601 / 21604 Shining Rune
  Buster (`32cf49d0` / `32d00be0`), 21602 / 21605 Blade Rain (`32cf4ab0` / `32d00de0`). The hyper state gets the EventProcess only (March also gave SRB /
  Blade Rain's hyper the Init). `> 21600` and no case -> `@32d23c1e`; `< 21600` -> the id switch (March's until CH2-CH3). Bodies are CH3d's (kickoff amended).
- **`SetMegaSlashSkillFunc`** (new, `_CORE`; June `@32d23c1e`, the default of both June switches; there is no explicit Mega Slash case). June registers
  `ESSI_SI_A_MEGASLASH_MEMO_READY` / `_CHARGE` 0xea / `_FIRE` 0xeb there, whose bodies are CH2a's new functions, so the helper holds March's
  `ESSI_SI1_ES_MEGA_SLASH` case until CH2a (kickoff amended). Under `_LIVE` March's id-switch `default:` calls it (Chung's `SetLunaticBlowSkillFunc` shape).
- Enum names (`JUNE_ENUM_NAMES`, `X2SkillTree.h`): `RNSI_HA_ELK_GIGANTIC_SLASH`, `RNSI_HA_ERS_SHININGRUNE_BUSTER`, `RNSI_HA_EIS_BLADE_RAIN` 21600-21602
  and their `_UPGRADE` 21603-21605 (June `Enum.src.lua`, June's spelling). Types 14-16 named (§7ae marked; `ResetLeftSkillCoolTimeBySkillType` n/a).
- `Always.h`: three new `_LIVE` guards, `JUNE_SKILL_TYPE` (grades 14-16), `JUNE_SKILL_NOTE` and `JUNE_DAMAGE_EXCEPTION` (`GetTranscendenceMemoSkillID`).

**Dev free-skill gate.** `32d223e0` has no cool-time / MP check; the gate is CH1k's in `SpecialAttackLookup` (`32d1bd10`). Checklist §7 dev-gate row
and 7i marked `(Elsword: ...)`.

**Base callbacks.** Elsword's 157 (`ch1m_user_callbacks.tsv`) minus Chung's 127 triaged rows = 33, triaged in
`june2017_re/char_elsword/ch1n_user_callback_triage.tsv` (bytes via capstone; spot-checked `31fd2280`, `31ffb954`): **28 SAME, 5 DIFF**.
- **Ported, new flag `SERV_IRUHADEV_JUNE_USER_MACHINE_GUN_FACING` (defined, every class, base code):** `MACHINE_GUN_WAIT_EventProcess` (`31ffb130`),
  `_ATTACK_EventProcess` (`31ffb7f0`), `_JUMP_UP_FrameMoveFuture` (`31fd2280`), `_JUMP_DOWN_FrameMoveFuture` (`31fd2330`): both arrows held -> the arrow
  pressed this frame turns the unit (CH1n2's `UpdateFutureIsRightByArrowKeys`; `#error` guard on `JUNE_USER_BASE_STATE_BODIES`). Speed is set as June
  (dash / walk speed when any arrow is held, 0 otherwise in WAIT / ATTACK). Unreachable in this build: only `RIDE_MACHINE_GUN` (off, `InHouseEtc.h`) enters them.
- **Kept, already decided:** `RandomLoseEventProcess`'s `find` guard (Chung's CH1n).
- Same: Small / Big damage, DashJump x3 (CH1j/CH1k's `_CHAR_ARME_CORE` bodies), the rest of the machine gun, Summon Magic Spear x2.

**Checks.** One-file probes (`/c /Zs /EHsc`): `X2GUElsword_SwordMan.cpp` `_LIVE` off exit 0 / 0 errors, `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` exit 0
/ 0 errors (`cl /P`: `SetMegaSlashSkillFunc( normalStateData` 0 vs 2, so the flag took effect); `X2GUUser.cpp` exit 0 / 0 errors.
`git diff --patience --numstat`: `Always.h` 23 / 0, `X2GUElsword_SwordMan.cpp` 139 / 0, `.h` 1 / 0, `X2GUUser.cpp` 24 / 0, `X2SkillTree.h` 11 / 0.
`flagcheck JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_USER_MACHINE_GUN_FACING --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_CHAR_ELSWORD_LIVE --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `0 FAIL`, RESULT FAIL only for the `//#define` (by design).
Each: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
`junediverge.py --check` (the five code files): `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
Funcmap rows `Char Elsword CH1n` (2). No JUNE_BUGS row.
`DEPLOY OK: X2_offline.exe is 25,961,472 B, dated 2026-11-04 16:32:33, md5 8806640ca1bf33e361fe86e6822d369e, equal to X2/US_SERVICE/x2.exe (2026-11-04 16:32:26); built at main 6f5f0b12 + 5 uncommitted code file(s).`
No `exestr` line: `_LIVE` is off, and the machine-gun bodies add no string.

**Nothing to play-test** (`_LIVE` is off; the machine-gun states cannot be entered). Next: `/x2-phase char-elsword CH1n review` (Opus 5.5 high).

### CH1n - Review (2026-11-04)

**Verdict: PASS, no code change.** Read from a fresh decompile of `32d223e0` (969 lines) and the exe bytes (`june2017_re/disasm_range.py`).
- **Head.** `( int )slot < 0 || 4 < slot`; tables `33521204` / `3352132c` / `33521318` / `33521340` by `bSlotB_`; hyper `word +0x10 = 0x101`.
  `UserUnitStateData` offsets against `X2GUUser.h`: +0x10 `m_bHyperState`, +0x14 `StateInit`, +0x1c `StateStartFuture`, +0x24 `StateStart`,
  +0x34 `OnFrameMove`, +0x44 `OnEventProcess`; every slot pair the port writes is the one June writes.
- **Memo exit.** Bytes @32d224e4-32d22500: `31fdbcc0( id )`, then `32206450( memo, 1 )` on `g_pData +0x50` (skill tree), `jne 32d23daa` = the
  epilogue (`31fdf970`, return). No case, as ported (Chung's / Eve's / Aisha's form). It cannot fire for Elsword: June's `SkillNoteTemplet.src.lua`
  has 15 `MI_TRANSCENDENCE_1` / `_2` rows (skill prefixes AVP, EEG, ECE, ECB, CSG, CDC, ALD, SGM, SBH, LLD) and none names an Elsword skill, so
  `GetTranscendenceMemoSkillID` returns 0 for him. No routing needed.
- **Types.** 5 / 6, 7, 0xd-0x10 / 0xc / default = `ST_BUFF` 5, `ST_ACTIVE` 6, `ST_SPECIAL_ACTIVE` 7, `ST_RELATIONSHIP_SKILL` 12, `ST_HYPER_ACTIVE_SKILL`
  13, grades 14-16 (enum counted in `X2SkillTree.h`; `FINALITY_SKILL_SYSTEM` on). `SI_ETC_WS_COMMON_LOVE` = 900 is the live member
  (`UPGRADE_SKILL_SYSTEM_2013` branch, `Always.h:1643`; 6001 is the dead `#else`).
- **Buff / throw item.** Tails `@32d22557` / `@32d23d7c`: FM + EP on normal, the same table name and FM + EP copied to hyper, as ported. Thunks:
  `329ccd90` -> `32a77a20` (`jmp [vtbl+0x5f0]`), `329cba40` -> `31fcbd60`; `329ccda0` -> `329cc0d0` (`+0x5f4`), `329ccdb0` -> `329cc170` (`+0x5f8`).
- **Hyper switch.** `0x5460 < id`: 21601 / 21604 SRB, 21602 / 21605 Blade Rain, 21603 -> Gigantic Slash, default -> `@32d23c1e`; `== 0x5460` Gigantic
  Slash; else the id switch. Thunks `32d262f0` / `32d26300` / `32d26310` -> `32cf4930` / `32cf49d0` / `32cf4ab0` (Init), `32d26df0` / `32d26e00` ->
  `32d00be0` / `32d00de0`, `329cbfc0` -> `31fcc740` (EP). Hyper copies `+0x44` only. Table strings match. The six `RNSI_HA_*` = `Enum.src.lua`
  15508 / 15542 / 14359 / 16552 / 15039 / 16448 (21600-21605).
- **`SetMegaSlashSkillFunc`.** Both switches' `default:` jump to `@32d23c1e` (inner @106, outer @476); no explicit Mega Slash case. The helper's body
  equals March's `SI_A_ES_MEGASLASH` case line for line; `_LIVE`'s `default:` calls it and breaks (March's `default:` fell into that case). June's
  `MEMO_READY` / 0xea / 0xeb registration stays CH2a's.
- **Callbacks.** SAME spot-checks from the bytes: `MACHINE_GUN_JUMP_UP_EventProcess` `31fc7f00` (`0 >= speed.y` -> vslot 0x5c `( 0x11d, 0 )`, tail
  0x7d4) and `DashJumpEndFuture` `31fc7f80` (0x7dc = `CommonStateEnd`, then `fDownAccel = 0`) = March. DIFF bodies: `31fd2280` / `31fd2330` (both arrows
  -> one-press turns, else unchanged; speed = walk rate +0x4910) and `31ffb294` / `31ffb954` (the same, speed = dash rate +0x49c4, `fldz` -> 0 when no
  arrow) = `UpdateFutureIsRightByArrowKeys` + the ported speeds. RandomLose stays the decided stand-in.
- **Always.h.** The three `_LIVE` guards and the `JUNE_USER_BASE_STATE_BODIES` guard name the functions they need; the `_LIVE` probe compiles.

**Hygiene.** flagcheck `--range 2fa9dd2e^..2fa9dd2e --files`: `_CORE` `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`;
`JUNE_ENUM_NAMES` `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`;
`JUNE_USER_MACHINE_GUN_FACING` `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` `RESULT: PASS`;
`_LIVE` `files: 0 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`, FAIL only for the `//#define` (by design). Each: 0 deleted, 0 non-ASCII,
0 EOL-mismatched, 0 `#else`/`#endif` without the token, 0 encoding changes. `git diff --patience --numstat`: 23 / 139 / 1 / 24 / 11 added, 0 deleted.
`junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUUser.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUElsword_SwordMan.cpp` `_LIVE` off exit 0 / 0 errors, `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` exit 0 / 0 errors.
No build: the review changed no code (CH1n's `DEPLOY OK` stands). Nothing to play-test. Next: `/x2-phase char-elsword CH1o` (Opus 5.5 high).

### CH1o - Done (2026-11-04)

**Landed** (flag `_LIVE`, still `//#define`; the velocity helper under `_CORE`; 7 funcmap rows `Char Elsword CH1o`: 5 amended, 2 new). No class-map row carries CH1o (CH0's
triage); every Elsword override whose name was unsettled in `ch0_vtable_all.tsv` is owned by CH1h / CH1i / CH1k / CH1q. What CH1o owned:
- **`CX2Game::GetNearestNPCUnitUID` `322d2680` is March's, no code.** CH0's `replace` row said June keeps a unit with no gage data; March does too
  (`CX2GameUnit::GetNowHp()` returns 1 then). Read in order: gage data `+0xc04` NULL -> alive, HP gage `+0x20` NULL -> dead, else `0 < fNow`
  (`test ah,5` + `jp`), vslot 0x174 != 4 (`GUSI_DIE`), `GetDistance` `31c7b560` below the best so far (`test ah,0x41`). The divergent row now says
  "no code", and the allow row for `ESSI_LEAP_StartFuture` is removed.
- **`31f76580` writes velocity, not scale** (CH0's name was wrong). Mesh `+0x534..+0x53c` is `m_vVelocity` (funcmap row `mesh+0x54c`). Written as
  `CX2DamageEffect::CEffect::ChangeMainEffectVelocity( int, float )`, which builds ( x, 0, 0 ) for `31f75ef0` = `ChangeMainEffectVelocity( int, const D3DXVECTOR3& )`
  (`GetMainEffect`, type 0 adds, type 2 sets, any other type multiplies per component, then `SetVelocity`). The type has no Enum, so it stays a number.
  **JUNE_BUGS 526:** CH2d's caller (`32d16c00` @32d17062, Crescent Cut) passes type 1, so the effect's y and z velocity become 0. CH2d kickoff amended.
- **Hooks 0x508 / 0x50c: CH1h had the names swapped.** In `GetFinalDamage` `32284aa0`, EBP = DD `+0xc` (the attacker) and EBX = DD `+0x470` (the
  defender). Vslot 0x508 runs on a user attacker (`+0x264 == 0`) with `&fPhysicAttack` / `&fMagicAttack` (`[esp+0x28]` / `[esp+0x2c]`, from `+0x4424` /
  `+0x44d8`). Vslot 0x50c runs on a user defender with `&fPhysicDefence` / `&fMagicDefence` (`[esp+0x48]` / `[esp+0x44]`). The calls come right after
  the two stat copies and `fSharpenDamageRate = 1`, which is where March's code goes (`X2GameUnit.cpp`, after `SERV_TRAPPING_RANGER_TEST`). Elsword's
  empty bodies are renamed to match (0x508 = `ChangeFinalDamageAsAttacker`, 0x50c = `ChangeFinalDamageAsDefender`), with descriptive parameter names.
  Base `33093ad0` (`ret 8`) is empty. Of the classes mapped so far, only Elsword overrides either slot, and his overrides are empty too.
- **Hook 0x510 `ApplyBuffToOpenMind`:** base `31fc7ab0` is empty. `CX2GUUser::DamageReact` calls it @320478c6. That is inside the enchant-or-buff test,
  after `ProcessSocketEffectEx` 1 / 2 and before vslot 0x840 (`SetSpecificValueByEnchant`). In March it is inserted after the two calls at
  `X2GUUser.cpp` (live `SERV_SECRET_HELL` branch). Elsword's `_CORE` body calls the base first (`_LIVE`), as the CH1h review asked.
- **Decisions, no code:** `IsInJobLine` (0x670) and `SetEquippedSkillLevelStateData` (0x7a8) stay plain Elsword members, as Chung's CH1o decided. Only
  class code calls 0x670. 0x7a8 is the shared `CX2GUUser` function `32028190`, which Elsword does not override, and every March class keeps its own
  wrapper. So no `331a4950` base is written.

**Routed.** Stage 33 (`stage33-plan.md` amended): June's next call in `GetFinalDamage`, `this` vslot 0x6c `3229a530` ( pDamageData, &rP, &rM ) @32284d98,
multiplies both attacks and is not ported. It belongs to Stage 33's whole `GetFinalDamage` port. No other gap.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files <7>`: `files: 5 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (X2DamageEffect.*,
  _CORE), 0 FAIL, 0 ERROR`; `10 deleted line(s)` (the CH1h hook rename: 8 in the `.cpp`, 2 in the `.h`, all inside `_CORE`), `0 non-ASCII`,
  `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`. RESULT is FAIL only because `_LIVE` is a `//#define`.
  `flagcheck JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2DamageEffect.cpp X2Lib/X2DamageEffect.h`: `2 PASS`, `RESULT: PASS`.
  `git diff --patience --numstat`: X2DamageEffect.cpp 24/0, .h 4/0, X2GUElsword_SwordMan.cpp 11/8, .h 2/2, X2GUUser.cpp 3/0, X2GUUser.h 5/0, X2GameUnit.cpp 7/0.
- `junediverge.py --check <the 7 files>`: `allow-listed: 10`, `RESULT: PASS`; `--selftest`: `SELFTEST: PASS`. `enumscan.py --check` on the four `.cpp`:
  1 A row, `X2DamageEffect.cpp:1705` (`SI_SA_ES_FATAL_FURY`, from 2026-09-26 and not touched here).
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`, no PCH), with `_LIVE` off and on: X2GUElsword_SwordMan / X2GUUser / X2GameUnit / X2DamageEffect `.cpp`,
  all exit 0 with 0 errors. In the `cl /P` output, each insertion is present only with `_LIVE`: the two hook calls 0 -> 1, `ApplyBuffToOpenMind();` 1 -> 2,
  Elsword's base call 0 -> 1. The velocity helper is present in both.
- `DEPLOY OK: X2_offline.exe is 25,961,472 B, dated 2026-11-04 17:12:34, md5 bcf5730905e3ac9184f47c202224c930, equal to X2/US_SERVICE/x2.exe (2026-11-04 17:12:27);
  built at main 759d36aa + 7 uncommitted code file(s).` No `exestr` line: every call site is under the undefined `_LIVE`.

**Nothing to play-test** (`_LIVE` is off; the velocity helper has no caller until CH2d). Next: `/x2-phase char-elsword CH1o review` (Opus 5.5 high).

### CH1o - Review (2026-11-04)

**Verdict: passes with one fix** (flag `_LIVE`, still `//#define`). Read from the exe bytes (Ghidra listings, `charport_adis.py elsword 31f76580`):
- **`GetNearestNPCUnitUID` `322d2680` = March, no code (row holds).** `[esi+0xc04]` NULL -> skip to the state test (alive, March's `GetNowHp()` 1); HP gage
  `[+0x20]` NULL -> skip unit; `fldz; fcomp` the protected `fNow` (gage `+4`, March `Gage`: `bool bShow` then `KProtectedType<float> fNow`), `test ah,5; jp` keeps only
  `0 < fNow`; vslot 0x174 == 4 -> skip; `GetDistance( pos, vslot 0x138( 0 ) )`, `fcompp` best vs dist, `test ah,0x41; jne` keeps only dist < best; best starts at
  `fMaxRange` (`[esp+0x18]` on entry), `ret 0x14`. = `X2Game.cpp:16938`.
- **Hooks 0x508 / 0x50c: names and order hold.** EBP = DD `+0xc` (checked first, as March's attacker), EBX = DD `+0x470`; `[esp+0x28]` / `[esp+0x2c]` are the two
  attacks the `322788c0` rolls get (= March `DamageMaximizeRoll` 0 / 1), `[esp+0x48]` / `[esp+0x44]` the defences from `+0x46f4` / `+0x47a8` (same 0xb4 stride as the
  attacks). 0x508 pushes `&[+0x2c]` then `&[+0x28]` (args physic, magic), 0x50c pushes `&[+0x44]` then `&[+0x48]`. Each gated on `+0x264 == 0` (funcmap
  `unit+0x264` = `m_GameUnitType`, `GUT_USER = 0`). June's order: attack copies + rolls, defence copies, two `CX2Stat` + `SetStat`, `fSharpenDamageRate = 1`
  (`@32284d35`), the hooks, then vslot 0x6c (Stage 33): March's insertion is the same point.
- **Fix: vslot 0x510 has a second June caller.** Whole-`.text` sweep for vtable loads (`mov r,[r]` then `mov r,[r+disp]`): 0x508 1 (`@32284d43`), 0x50c 1
  (`@32284d62`), 0x510 **2**: `DamageReact @320478c9` and `CX2GUUser::Process_ExtraDamage` `32034e50` `@32039873`, right after its two `ProcessSocketEffectEx`
  (1 / 2) at `@32039855-3203986c`, the path every extra-damage case that lands joins (the others jump to `@3203987d`). March's twin is the `bGetExtraDamage` block
  at the end of `Process_ExtraDamage` (`X2GUUser.cpp`, `SERV_SECRET_HELL`); `ApplyBuffToOpenMind();` added there under `_LIVE`. funcmap row `31fc7ab0` amended.
  The `DamageReact` site holds: inside the `ProcessEnchant || bBuffApplied` block after both socket calls, before the separate `bApplyEnchant` -> vslot 0x840 test.
- **`ChangeMainEffectVelocity` holds.** `31f75ef0`: `GetMainEffect` NULL -> return; old = mesh `+0x534..+0x53c`; type 0 adds, type 2 copies, any other multiplies per
  component; writes `+0x534..+0x53c`, `ret 8` (March `CXMeshInstance::GetVelocity` / `SetVelocity`). `31f76580`: ( x, `fldz` 0, 0 ), ECX passed through, `ret 8`.
- **0x670 / 0x7a8 decision holds.** 0x7a8: 384 vtable loads, all in class ranges (`329690b6` Eve up to `32cf54eb` Elsword); none in engine code. 0x670 as Chung's
  review (126, all class code).

**Hygiene.** Commit `d7597ce0` (`--range d7597ce0^..d7597ce0 --files` the 7): `5 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (X2DamageEffect.*, _CORE), 0 FAIL, 0 ERROR`; 10 deleted
lines (the CH1h rename), 0 non-ASCII, 0 EOL-mismatched, 0 `#else/#endif` without the token, 0 encoding changes; RESULT FAIL only for `define: NOT FOUND`. numstat
24/0, 4/0, 11/8, 2/2, 3/0, 5/0, 7/0. The fix (`--worktree --files X2Lib/X2GUUser.cpp`): `1 PASS`, +3, same zeros, FAIL only for the `//#define`.
`junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`; `--check X2Lib/X2GUUser.cpp`: `RESULT: PASS`. `enumscan.py --check` the four `.cpp`: 1 A row,
`X2DamageEffect.cpp:1705` (`20cecda8`, not this phase). Probes (`census.cl_cmd`, `/c /Zs /EHsc`): the four `.cpp`, `_LIVE` off / on, exit 0, 0 errors; `cl /P`
counts off -> on: `ChangeFinalDamageAsAttacker( &fPhysicAttack` 0 -> 1, `ApplyBuffToOpenMind();` 1 -> 3 (both sites).
`DEPLOY OK: X2_offline.exe is 25,961,472 B, dated 2026-11-04 17:26:32, md5 e8ca4dba084f2c7342b87ea594fd7d60, equal to X2/US_SERVICE/x2.exe (2026-11-04 17:26:25); built at main d7597ce0 + 1 uncommitted code file(s).`
No `exestr` line: every site is under the undefined `_LIVE`.

**Nothing to play-test.** CH1p kickoff checked. Next: `/x2-phase char-elsword CH1p` (Opus 5.5 high).

### CH1p - Done (2026-11-04)

Flags: `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (insert-only, still `//#define`). One new `Always.h` guard: `_LIVE` needs `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, because
the new reader line keeps `UT_CHUNG` in the June set (the `_LIVE` probe compiles with it, so `_CHUNG_LIVE` is defined at that line).

**Method.** `cl /P` of `X2GUElsword_SwordMan.cpp` and `X2GUUser.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Every surviving line that reads a slot id,
the pressed index, `m_iNowSpecialAttack`, `m_eSpecialAttackKeyPressed`, `SAKP_*`, `CurrentSkillKeyManager` or `m_kSkillSlotKey` is listed and checked.

**7ag, the pressed-index space.**
- **Already June's (CH1k / CH1n):** in the `_LIVE` `SpecialAttackEventProcess` (`32cf3290`, lookup `32d1bd10`), `CommonSpecialAttackEventProcess` fills
  `iPressedSkillSlotIndex`, which goes to `m_iChosenSkillSlotIndex`, `StateChangeBySkillSlot` (0-9), `MarkSkillSlotUsed` and `m_iNowSpecialAttack = slot + 1`. CH0's sites
  (old `:4982`, the `> 3` / `- 4` decode `:5232-5249`, `CountUsedSkill` `:5152`, now `:7816`) are March's body, and none of them is in the `_LIVE` `.i`.
- **Fixed: the shared reader.** `CX2GUUser::CommonSpecialAttackEventProcess` (June `31fcadd0`) still gave Elsword March's 4 + 4, so under `_LIVE`, F would
  have returned with no slot and Q / W / E / R would have read as A-TC / B1-B3. `UT_ELSWORD` now joins the June set (`X2GUUser.cpp`, `_LIVE` / `#else` around
  Chung's block; stand-in comment lists Lire, Raven, Ara, Elesis, Add).
- **`CountUsedSkill` (CH1k routed):** June's use path (`32cf3290` -> `32d1bd10` -> `32cf5760` -> `32d0d8e0`) never calls it, and June counts through
  `MarkSkillSlotUsed` as Chung's does. March's call stays in `#else` only. No change.
- **`GetSkillSlotIndexUsed`** (`COMMON_BUFF_FrameMove`, `:18260` / `:18302`): reads the slot **state id** (`USI_SPECIAL_ATTACK_*`, per bank 0-4 + `bSlotB`), not
  the pressed index. CH1a's June body `32d08590` calls it as June does. No change.
- **No `CurrentSkillKeyManager`, `SAKP_*` or `m_eSpecialAttackKeyPressed` reader** in his class, live or dead. March's guard skill
  (`ESSI_SI_A_ES_DEFENCE_READY_StartFuture`, a `USI_SPECIAL_ATTACK_*` state switch with no `_5` case) is June-dead data (CH0 `ch0_state_skill.tsv`: string absent
  from June's exe), so it is not ported.
- **Routed: June's held-key tracker.** `dispsweep.py 8b58` finds `CX2GUUser::m_kSkillSlotKey` (Eve's `SkillSlotKey`, `_EVE_CORE`) in 10 Elsword functions, all skill
  states: Mega Slash memo `32cf3970` / `32cf39b0` (CH2a), Wind Blade `32cf3bd0` / `32cfa080` (CH2c), Windmill `32cf3da0` / `32d11fd0` and Grand Cross
  `32cf4ec0` / `32cf4f60` (CH3a), Counter Guard `32cf4e50` / `32cf4d70` (CH3b). March reads no key there. Each kickoff is amended to port June's calls whole.
- Checklist 7ag marked `(Elsword: CH1p)`.

**Hooks 7h / 7l / 7o / 7p / 7x** (CH0 rows; checked in the `_LIVE` preprocess):
- 7h: `InitWaitStateChange( "ESSI_WAIT_STATE_CHANGE" )` (`:1394`, CH1m). The shared `UPDOWN_CANCEL_AFTER` rule (`SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`, required by `_LIVE`):
  the parser is live (`X2GUUser.cpp:16608`, `:17956`, `:23612`) and 18 inline sites from CH1d2-CH1g are in the `.i`. The rest go to CH2b / CH3a-CH3c.
- 7l: `InitBoundingStates( "ESSI_" )` (`:2920`, CH1m). 7o `SET_LIMIT_MAX_G_SPEED` (reader `X2GUUser.cpp:16424`, C3g), 7p `TIME_STOP_IMMUNE_STATE` (reader `:18048`,
  C3e-4) and 7x `AT_COMMAND` (67 tags; stand-in `SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN`) are script data. They arrive with June's script in CH1 final, which marks them.
- §5 parser census: `ch0_keys.tsv` gives no June-only key an Elsword owner (camera pair Stage 9, `IMMUNITY_EDT_LIST_AT_THIS_STATE` backlog Stage 2, two buff sub-table keys n/a).

**Checks.** Probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUUser.cpp` `_LIVE` off / on, exit 0, 0 errors, 39 / 39 warnings. `X2GUElsword_SwordMan.cpp` `_LIVE` on, exit 0,
0 errors, 38 warnings. `cl /P`: the reader line with `UT_ELSWORD` only with `_LIVE`, and Chung's line only without it.
`flagcheck JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s),
0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. RESULT FAIL is only `define: NOT FOUND` (`//#define`).
`git diff --stat`: +3 / +5, 0 deletions. `junediverge.py --check X2Lib/X2GUUser.cpp`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`.
`DEPLOY OK: X2_offline.exe is 25,961,472 B, dated 2026-11-04 17:45:40, md5 8da8ac679da407e8c280845113b723f7, equal to X2/US_SERVICE/x2.exe (2026-11-04 17:45:33); built at main 23b1b0cd + 2 uncommitted code file(s).`
No `exestr` line: `_LIVE` is off. No funcmap or `JUNE_BUGS` row.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-elsword CH1p review` (Opus 5.5 high).

### CH1p - Review (2026-11-04)

**Verdict: passes, no source change** (no rebuild; the deployed `8da8ac67` stands).
- **Shared reader `31fcadd0`** (Ghidra decompile): A S D C `+0x86f7` / `fb` / `ff` / `f3` -> 0-3, `+0x872b` (oneSkill5) -> 4, T `+0x872f` -> `GetSkillSlot( 4, true )` = 9 only
  when my unit's tree (`g_pData +0x68 +0x90`) has key 1 usable (null `g_pData` skips the expand test), then `return 1` unless the slot is expanded (`+0x2cc` UI, `+0x109`),
  Q W E R `+0x8707` .. `+0x8713` -> 5-8. `X2GUUser.cpp:26529-26594` is that body; CH1p's only change is `UT_ELSWORD` in `bMarchSlotIndex` under `_LIVE`. Lookup `32d1bd10`
  calls it with its index local and stores it at `+0x8ea4` (`m_iChosenSkillSlotIndex`); use `32cf5760` goes to `StateChangeBySkillSlot` `31fd32e0`.
- **No other class gate on slot or key code in `X2GUUser.cpp`.** The remaining `UT_EVE` / `UT_CHUNG` tests are frame-sync debug text (`ReceiveFrameData` `:29316`, `:29605`),
  the pepper run and per-class asset switches. `MarkSkillSlotUsed` (0-9) and `StateChangeBySkillSlot` (`>= 5` = B) take June's space for every caller.
  `CountUsedSkill` / `GetPressedSkillState` / `m_sCurrentSkillKeyManager` (4 + 4) are reached only from unported classes (Add, Ara, Elesis, Chung's `#else`), never from Elsword.
- **Guard `_LIVE` needs `_CHUNG_LIVE`: right.** The `_LIVE` line keeps `UT_CHUNG`; without the guard, Elsword on + Chung off would give Chung June's numbering while his class
  code reads March's. `_CHUNG_LIVE` is `#define`d at `Always.h:11089`; `ARME_CORE` (the June body itself) is already required at `:11478`. A free-standing line would need a
  four-way nest for no gain.
- **Class side** (`cl /P` with `_LIVE`, lines from `X2GUElsword_SwordMan.cpp` only): `iPressedSkillSlotIndex` 2 (`:7460`, `:7462` into `CommonSpecialAttackEventProcess`),
  `m_iChosenSkillSlotIndex` 5 (`:7541` = pressed, `:7561` `StateChangeBySkillSlot`, `:7579` `MarkSkillSlotUsed`, `:7603` / `:7619` the use-condition 3 / 5 restore),
  `m_iNowSpecialAttack` 1 (`:7624` = slot + 1). No `CountUsedSkill`, `GetPressedSkillState`, `m_sCurrentSkillKeyManager`, `SAKP_`, `m_eSpecialAttackKeyPressed` or
  `m_kSkillSlotKey` survives.
- **`+0x8b58` sweep** (`dispsweep.py 8b58`): in `0x32cf2da0-0x32d2763f` 14 hits, all `lea ecx, [esi + 0x8b58]`, in exactly the 10 functions of the Done note, none uncarved.
  Each address is named once: `32cf3970` / `32cf39b0` CH2a, `32cf3bd0` / `32cfa080` CH2c, `32cf3da0` / `32d11fd0` / `32cf4ec0` / `32cf4f60` CH3a, `32cf4e50` / `32cf4d70` CH3b.
- **Hooks:** `InitWaitStateChange( "ESSI_WAIT_STATE_CHANGE" )` `:1394` and `InitBoundingStates( "ESSI_" )` `:2920` in the `.i`; `UPDOWN_CANCEL_AFTER` readers
  `X2GUUser.cpp:16608` / `:17956` / `:23612`, 7o `:16424`, 7p `:18048`. `m_fUpDownCancelAfter` sites in the class `.i`: **18** = `ch0_checklist_rows.tsv` L420 (CH1d2 1, CH1e 10, CH1f 7).

**Hygiene** (range `199e4cca^..199e4cca`, files `X2GUUser.cpp`, `Always.h`): `flagcheck JUNE_CHAR_ELSWORD_LIVE`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`,
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only for
`define: NOT FOUND` (`//#define`). `git diff --patience --numstat`: 3/0 `Always.h`, 5/0 `X2GUUser.cpp`. `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/Zs /EHsc`): `X2GUUser.cpp` 0 errors, 39 / 39 warnings off / on;
`X2GUElsword_SwordMan.cpp` 0 errors, 38 / 38; no `#error` fired. No play-test. Next: `/x2-phase char-elsword CH1q` (Opus 5.5 high).

### CH1q - Done (2026-11-04)

All 21 class-map rows were read, plus the damage-manager math, the CEffect and user AttackResult blocks and the sync field. Real June changes go under the
new `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`, a `//#define` that CH1 defines with `_LIVE` (`_LIVE` `#error`s without it; `_WOS` `#error`s without
`JUNE_ENUM_NAMES`, `JUNE_SOCKET_PARITY`, `JUNE_CHAR_EVE_SKILL`, `JUNE_EFFECT_ALPHA_OPTION`, `JUNE_DIVERGE_SKILL_LEVEL_VALUE`, `JUNE_SKILL_TEMPLET`). The
snapshot pair sits under `_CORE`. `_WOS_DEBUG` is defined, and its four `[JUNE_CHAR] ELSWORD WOS` lines sit inside `_WOS` code. Funcmap rows `Char Elsword CH1q`: 14.

| June | Site | June differs from March |
|---|---|---|
| `31f85910 @31f86bab-31f86ef0`, `32012b50 @32012d3b-32013088` | CEffect / user `AttackResult` WSP block | the gain x ( 1 - specialization B `+0x6d04` ) in vigor / destruction, else x ( A `+0x6c50` + 1.0 ) (`DE E1` = `1.0 - B`); `RNSI_P_SCA_MADNESS` 28420 learned and `SA_DECREASE_REL` ( SA `0x6a`, name table `3379e6b0` ) > HP / max HP ( HP > 0 ) -> x2. One helper `CX2GUUser::ApplyWayOfSwordPointGainRate` for both inlined copies. Elesis's branch also x her vslot `0x720` ( base `3208e1b0` = 1.0 ): stand-in comment |
| `32622600` / `32622660` | `GetDestRageWSPGain` / `_Elesis` | PvP rate in a PvP **or PvE** game (`322cc1a0` = `GetIsNowPVPCorrectionGame`) |
| `32d18cd0` | `GetFinalDamage_WayOfSword` | destruction 2.0 / 1.5 become 1.5 / 1.25 in a PvP or PvE game |
| `32d0f0c0` | `SwordWaitEventProcess` | `oneZ` with no re-attack gate (as Wait, CH1a); the rest = March |
| `32d0f300` | `DoStateDestructionRage` | the two sequences take the alpha tag `32296d60` (checklist 7c, 2 of the 29 sites) |
| `32d0c960` | `Start_SwordmanVigor` | `CalculateFinalMoveSpeedPercent( 0, level )` ( `fldz` @32d0ca32 ); no effect, the setter is empty in both builds (JUNE_BUGS 527) |
| `32d058a0` / `32d058d0` (new, `_CORE`) | `StoreGageSnapshot` / `RestoreGageSnapshot` | own unit keeps / restores `GetWayOfSwordPoint` (`m_fSnapshotWayOfSwordPoint`, June `+0x92b8`); the state is not kept; bases `32009780` / `3204da60` stand in, as Chung's |

**= March, no code:** `WayOfSwordStateChange` `32d0eeb0`, Get / Set state and point (`32cf50f0` `32cf5100` `32cf5130` `32cf5140` clamp), End vigor, Start /
End destruction, `32715040` = `CX2SocketItem::GetOptionRateCorrection`, sword wait Start / StartFuture / `PlayAnimationSwordWaitStart`, the destruction rage
thunks and EP `32cf63c0`, `CreateGageData` `32d1b810`, `CalcVigorMpUpOnAttack` / `_Elesis` / `OnDamageReact`, the loader `3261aee0` (March's 28 keys),
the sync field (+0x7ec0 written by CH1j's CommonFrameMoveFuture, +0x8160 read here). **CH0's replace row was wrong:** `CalcWayOfSwordPointOnDamageReact`
`326106c0` and `CalcWayOfSwordPoint` `326105c0` are March's live branch (`flagprobe.py`: `BALANCE_PATCH_20120329` ON, game-mode constant 1.0; CH0 read
the dead `#else`). Row 417 now says "no code"; its allow row is removed. New divergent rows (ported under `_WOS`): `GetDestRageWSPGain`, `_Elesis`,
`GetFinalDamage_WayOfSword`, `Start_SwordmanVigor`, `DoStateDestructionRage`. `RNSI_P_SCA_MADNESS = 28420` is registered in `X2SkillTree.h` (`JUNE_ENUM_NAMES`).

**Lua.** `luabind.py check WayOfSword DLG_Elsword_WayOfSword`: `0 unbound`, `RESULT: PASS`. `adopt-june --dry-run`: `WayOfSword.lua` 1 replaced,
`DLG_Elsword_WayOfSword.lua` identical. June's 14 extra keys have no reader (the MP rates stay hard-coded 3 / 4 / 2): CH1 adopts it whole (kickoff amended).

**Routed.** Not located by CH0, still March under `_WOS`: `m_bCured` / `CureExtraDamage`, `ResetAnimSpeed`'s vigor factor, `SAT_WAY_OF_SWORD_GAUGE_UP` -> CH4b;
the skill-slot Vigor x0.7 MP requirement (`X2Game.cpp:14855`) -> CH1q2; `ElswordWSPCheat` and `RenderName`'s WSP text (no June counterpart) -> CH5;
`GetActualMPConsume`'s x0.7 stays CH4b's. PHANTOM_BLADE keys are CH1g / CH3c's. Checklist §7 l.256, §7c l.352, §7an l.933 marked `(Elsword: CH1q)`.
Elesis's class side stays char-elesis's (her shared sites change with `_WOS`, as June).

**Gates.**
- flagcheck `--worktree --files` (eight code files): `_WOS --with _WOS_DEBUG _CORE`: `files: 5 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, RESULT FAIL
  only for the commented-out `#define` (OTHER = Always.h, the `_CORE` header lines, the `JUNE_ENUM_NAMES` enum line). `_CORE`: `2 PASS, 1 OTHER`, PASS.
  `_WOS_DEBUG`: `2 PASS, 1 OTHER`, PASS. `JUNE_ENUM_NAMES` (`X2SkillTree.h`): `1 PASS~`, PASS. All: `0 deleted line(s), 0 non-ASCII inserted line(s),
  0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `git diff --numstat`: +45 / +30 / +12 / +124 / +7 / +65 / +3 / +6, 0 deletions.
- `junediverge.py --check` (seven files): `allow-listed: 8`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s) remain`; the shared
  files: none in the changed lines.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`): the class, `X2GUUser.cpp`, `X2DamageEffect.cpp`, `X2DamageManager.cpp` with no flag and with `_WOS`: exit 0, 0 errors
  (8 runs); the class with `_LIVE` + `_WOS`: 0 errors; with `_LIVE` alone it stops at the new `#error` (`Always.h:11561`), which proves the probes read Always.h.
- `DEPLOY OK: X2_offline.exe is 25,961,984 B, dated 2026-11-04 18:26:30, md5 0f26d9161b030073bab0a0a259b34138, equal to X2/US_SERVICE/x2.exe (2026-11-04 18:26:23); built at main 120a29d1 + 8 uncommitted code file(s).`
  `EXESTR OK: 4 of 4 absent` (the four `_WOS_DEBUG` lines sit in `_WOS` code, which is off).

**Nothing to play-test** (`_WOS` and `_LIVE` are off). Next: `/x2-phase char-elsword CH1q review` (Opus 5.5 high), then CH1q2.

### CH1q - Review (2026-11-04)

**Verdict: PASS, no code change.** Every point was checked against the June exe (Ghidra decompiles plus raw `disasm_range.py` reads).

- **Attack-result gain** (`31f86bab-31f86ef0`, `32012d3b-32013088`): both copies have the same shape. The steps:
  - `CalcWayOfSwordPoint( [+0x764], [+0x23c] + [+0x238], state )`, then `0x674` state re-read.
  - Not -2 / 2: x ( protected `+0x6c50` + qword `334355b0` = 1.0 ). Else x ( 1.0 - protected `+0x6d04` ): bytes `DE E1` at
    `31f86c6c` / `31f86e10` / `32012de5` = FSUBRP ST(1), ST(0) = 1.0 - B.
  - `31fceab0( 0x6f04 )` > 0 and `g_pData +0x50`, then `32206450( 0x6f04, 1 )` (the project's GetSkillTemplet mapping), then
    `32206e50( 0x6a, level, 0, 0.0 )`. The 4th argument is the fallback (`[esp+0x60]` @32206f84), and 0.0 equals March's 0.
  - HP rate = 0 unless HP > `3342e84c` (0.0f), else HP / max HP. `v > rate` doubles (`fcompp`, `test ah,0x41`).
  - Then `Set( Get + gain )` and the MP factor.
  - The CEffect copy keeps the state read before the gain (`ebp`) for `32610790`. The user copy re-reads `0x674`. The port does both the same way.
  - Elesis (`[eax]==8`) multiplies by vslot `0x720` *before* the specialization, then calls `32622550` (`_Elesis`).
  - One `CX2GUUser` helper for the identical inlined part is right: the vslot `0x720` factor is the only difference and stays outside it.
  - The stand-in is right: March's `#else` has no Elesis factor, so the base 1.0 matches March. It is routed in checklist l.933 for char-elesis.
- **CH0's replace row overturned: confirmed.** `flagprobe.py BALANCE_PATCH_20120329`: `ON`.
  - `326105c0`: type const [0..5] x rate x state const, with the vigor / dest pairs [7..16] in March's order.
  - `326106c0`: DamageReact const ( +0x18 ) x [17..21], with the float argument ignored.
  - Both equal March's live `X2DamageManager.cpp:4440-4610`. No game-mode constant.
- **Ported bodies:**
  - `GetDestRageWSPGain` / `_Elesis`: `322cc1a0` -> +0x5c ( PVP_Hit ), else +0x58 ( NPC_Hit ), times +0x68 / +0x6c. The struct
    order (22 floats before NPC_Hit) agrees.
  - `GetFinalDamage_WayOfSword` `32d18cd0`: no Ghidra function, read raw.
    - State -2 required.
    - Type 4: 1.5 ( `33471a1c` ) / 2.0 ( `33437778` ).
    - Types 5 / 6: 1.25 ( `334395f0` ) / 1.5.
  - `SwordWaitEventProcess` `32d0f0c0`: the order matches the port (oneZ `+0x86eb` with no gate -> 0xa3, oneX -> 0xa8, `0x7f4( 1 )` =
    `ASUT_GROUND` (1), then dash / walk / jump-up, `0x55c( 0x17 )` hyper, down, WSS 2 / -2 test, `0x200` riding -> 0x49, else 0xd).
  - `DoStateDestructionRage`: two `32296d60` calls, one per sequence.
  - Vigor: `fldz` @32d0ca32 feeds the Lua call `CalculateFinalMoveSpeedPercent` (string `334872a0`). JUNE_BUGS 527 holds.
  - Snapshot pair: `0x198` = `IsMyUnit`. Store writes a float to `param_1[0x24ae]` = +0x92b8. Restore calls `0x67c( saved )`, then the
    base. The state is not kept.
- **"= March" spot-checks:** `WayOfSwordStateChange` `32d0eeb0` matches March's `:11238` branch for branch. That covers the thresholds
  +0x60 / +0x64, the sync field +0x8160 for other units, and the End / Start pairs with the gage +0x110 / +0x114 inline set. The other
  "= March" rows are low risk and their CH1q reads stand.
- **Routing:** CH1q2 names the `X2Game.cpp:14855` Vigor x0.7. CH4b names `m_bCured` / `CureExtraDamage`, `ResetAnimSpeed`,
  `SAT_WAY_OF_SWORD_GAUGE_UP` and `GetActualMPConsume`. CH5 names `ElswordWSPCheat` and `RenderName`'s WSP text.

**Gates** (range `ee3b2737^..ee3b2737`, the eight code files):
- `_WOS --with _WOS_DEBUG _CORE`: `files: 5 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`. The RESULT is FAIL only for the
  commented-out `#define`.
- `_CORE` (7 files; X2GUUser.cpp carries no `_CORE` line, and flagcheck cannot evaluate its old `:26915` `&&` / `||` `#if`): `RESULT: PASS`.
- `_WOS_DEBUG --with _WOS`: `RESULT: PASS`.
- `JUNE_ENUM_NAMES`: `1 PASS~`, `RESULT: PASS`.
- All runs show 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 token and 0 encoding changes. `numstat`: 45 / 30 / 12 / 124 / 7 / 65 / 3 / 6 added, 0 deleted.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`.
- `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s) remain`.
- Probes (`census.cl_cmd`, `/c /Zs /EHsc`): the four .cpp files with no flag, with `_WOS` and with `_LIVE` + `_WOS` give exit 0, 0 errors
  (12 runs). `_LIVE` alone stops at `Always.h(11561)` C1189.
- No build: no code changed, so the deployed exe is CH1q's.

**Nothing to play-test.** Next: `/x2-phase char-elsword CH1q2` (Opus 5.5 high).

### CH1q2 - Done (2026-11-04)

Every `gagedata` / `gageui` / `gagemanager` row of `ch0_wos.tsv` and rows 91 / 92 / 109 were read against June (decompiles, raw listing). The June changes
go under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (still `//#define`; it now also `#error`s without `JUNE_SKILL_SLOT_10` / `JUNE_SKILL_SLOT_UI`). Funcmap rows
`Char Elsword CH1q2`: 9. Divergent rows: 4 (all "ported under `_WOS`").

| June | Site | June differs from March |
|---|---|---|
| `32a348d0` | `CX2ElswordGageData::InitWhenGameIsOver` | the base only (`fldz` + `31e8c9d0`, = base `32a33f90`): the state and its changed flag are no longer reset after a game |
| `32e6d830` | `CX2ElswordMyGageUI::InitUI` | `InitWakeOrb` (vslot 0x54) before the dialog, as Chung's; `InitUIElswordWSP` inlined |
| `32e67df0` | `UpdateUIElswordWSP` | returns unless both statics (`+0x60`, `+0x5c`) exist; the slot-effect call also skips June state 0xf = `XS_GUILD_AGIT_GAME`, which March lacks (March's 15 is `XS_SQUARE_GAME`): comment, no test |
| `32e681d0` | new `CX2MyGageUI::RefreshWayOfSwordUI` (vslot 0x5c) | base `3353336c` slot 0x5c = `31f3f070` (`ret`): empty virtual; Elsword's sets gauge `+0x110` and calls `UpdateUIElswordWSP`. CH1h's `_LIVE` stand-in in InitEffect (`static_cast` + direct calls) is now the virtual call |
| `322cd5c0 @322cd7eb` | `CX2Game::UpdateElswordSkillSlotEffect` | March hides both B statics and breaks at the first B slot when B is off; June hides them at the B transcendence slot (`3214f3d0( i ) == 3`, `bSlotB`) when slot keys 0 and 1 are both unusable (`32890ee0`), with no break. Written with `IsSlotUsable` / `IsTranscendenceSlotIndex` |

**= March, no code:** `UpdateDataFromGameUnit` `32a34890`, ToPacket `32a323e0`, FromPacket `32a348e0` (`GetWSP` inlined), Clone `32a35e60`, SetState
`32a36310`, `CopyGageData` `32a34930` (**CH0 was wrong**: `@32a34973` sets `+0x110` = 1, as March), the UI ctor / dtor, `SetShow`, `SetShowWspParticle`,
`OnFrameMove`, `StopUIEffectElswordWS`, both Play functions (picture 4 centre, x 0.5), `UIFrameMoveElswordWSP` (March's `&&` null test kept by June),
the flicker 0.4 / 1.0 / 0.1 (`33446fa0` / `334395e0`), `CreateMyGageUI` `3248e290` (Elsword's 7 cases), the dead manager declarations (absent in both).
**Row 92** (skill-slot MP x0.7, `X2Game.cpp:14855`): June keeps it in `UpdateSkillCoolTime` `322e07e0` (type 1, vslot 0x674 == 2, templet `+0x160` == 2,
x 0.7 as a double at `3344af88`; March's `0.7f` differs by one ulp at most, kept). **CH1h's amendment:** done (the virtual).

**Routed.** `322ed400` is not a class-switch randomiser: it is June's training-game **F7** key (`KeyProcess` `322f7170 @322f7b6e`, `DIK_F7`, inside the
June 0x12 / 0x1a training block), one step of my unit's gauge for every class (Elsword: flip to the other extreme, +-4800). An every-class training-key
rework (F5 / F6 changed too): bullet in `JUNE_PORT_BACKLOG_ORDER.md` Stage 2, not ported here. Elesis's twins (`32a34c60`, `32e6e060`, `32e69e70`) show
the same changes: added to her line in `stage7-plan.md`. Checklist §7ah L827 marked `(Elsword: CH1q2 ...)`. `kickoffs/char-elsword/CH1.md` amended (one more
play-test line).

**Gates.**
- flagcheck `--worktree --files` (six code files): `_WOS --with _WOS_DEBUG _LIVE`: `files: 4 PASS, 1 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, RESULT FAIL only
  for the commented-out `#define` (as CH1q); `3 deleted line(s)` = CH1h's own `_LIVE` stand-in lines in `X2GUElsword_SwordMan.cpp`, replaced by the call;
  `0 non-ASCII`, `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`. `_WOS_DEBUG --with _WOS`: `1 PASS~, 1 DEFINE`, `RESULT: PASS`.
  `git diff --numstat`: +8 / +2 -3 / +4 / +33 / +6 / +10.
- `junediverge.py --check` (six files): `allow-listed: 13`, `RESULT: PASS`; `--selftest` PASS. `enumscan.py --check` (four `.cpp`): `0 A/B row(s) remain`.
- One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GageData.cpp`, `X2GageUI.cpp`, `X2Game.cpp`, `X2GUElsword_SwordMan.cpp` with no flag, `_WOS`, `_LIVE` + `_WOS`:
  `PROBES: 0 of 12 failed` (the `_WOS` runs pass the new `SKILL_SLOT_10` / `_UI` guards at that line).
- `DEPLOY OK: X2_offline.exe is 25,961,984 B, dated 2026-11-04 19:33:20, md5 6442d4845be72e5a9d97614a2cceedda, equal to X2/US_SERVICE/x2.exe (2026-11-04 19:33:13); built at main 1f680bf6 + 6 uncommitted code file(s).`
  `X2GageUI.cpp`'s two inserts were redone for EOL during the build; with `_WOS` off both versions compile to the same code (no exestr gate: the new line is in `_WOS` code).

**Nothing to play-test** (`_WOS` and `_LIVE` are off). Next: `/x2-phase char-elsword CH1q2 review` (Opus 5.5 high).

### CH1q2 - Review (2026-11-04)

**Verdict: PASS, no code change.** Every point was checked against the June exe (Ghidra disassembly, plus raw capstone reads through
`june2017_re/xordecrypt.py` where Ghidra has no function).

- **Skill-slot B test** (`322cd5c0`): loop `EBX` = combined index 0-9 (`cmp ebx,0xa` @322cd972), `[esp+0x10]` = `i >= 5` (bSlotB), `ESI` = slot index.
  - `3214f3d0( i )` returns 1 for i < 4, 2 for 5-8, 3 for 4 / 9, so `== 3` is exactly `IsTranscendenceSlotIndex( i )`. With bSlotB it is only i = 9.
  - The order is `== 3` (SETZ BL, no side effect), key 0, bSlotB, key 1, BL. Both keys go through `32890ee0` on `+0x68 +0x90 +0x494`, which is
    `IsSlotUsable` (state 1 or 2). The port's order differs only in where the side-effect-free index test sits.
  - Dest (`EBP`) is hidden, then vigor (`EDI`). The loop continues after `322cd858` (no break). Types 1 / 8 gate, the observer byte `+0x93` and
    the 10-slot loop match the port.
- **Refresh virtual:** base vtable `3353336c` +0x5c = `31f3f070` (`ret`). Elsword vtable `3349aba4` and Elesis vtable `3349ac34` override that slot
  with `32e681d0` / `32e69e70`. Each override sets gauge `+0x110` = 1, then jumps to its `UpdateUIElswordWSP`. Their +0x50 / +0x54 / +0x58 slots
  equal the base's.
  - InitEffect `32d0d540`: the `0x5c` jump @32d0d5ba is inside the `0x198` (IsMyUnit) test @32d0d56e, after the state switch. So a base virtual
    is right.
  - CH1h's stand-in is fully gone: no `UpdateUIElswordWSP` / `SetChangedWayOfSwordState` call is left in `X2GUElsword_SwordMan.cpp`, only
    March's commented-out lines.
- **Gauge data:**
  - `32a348d0` and `32a33f90` are the same five instructions (`fldz`, `31e8c9d0`). That is March's base
    `SetHyperModeRemainTime( 0.0f )`, so the port is right.
  - `CopyGageData` `32a34930`: base copy `32a36530`, then `+0x110` / `+0x114` / `+0x118`. `@32a34973` stores `cl` = 1 unconditionally, which is
    March's `SetChangedWayOfSwordState( true )`. CH0's note was wrong, as CH1q2 says.
- **Gauge UI:**
  - `InitUI` `32e6d830`: `InitStatusUI` `32e66e60`, vslot 0x54 (`32e5b310`, the same in base and Elsword), `OpenScriptFile`, then the inlined
    `InitUIElswordWSP` (`+0x5c` = vigor, `+0x60` = dest).
  - `UpdateUIElswordWSP` `32e67df0` tests `+0x60`, `+0x5c`, then `+0x110`. Case 0:
    - `test ah,0x41 / jp` = point >= vigor threshold (`+0x60`); `test ah,1 / jne` = point <= destruction threshold (`+0x64`).
    - Then `> 0`, `< 0`, else the centre pictures, all as March.
  - The tail (@32e68149) is reached only inside the changed block: `g_pX2Game` handle test, then state `+0x8c` != 0xe / 0xf, then a jump to
    `322cd5c0`.
  - June's Enum: `XS_VILLAGE_MAP` = 14, `XS_GUILD_AGIT_GAME` = 15, `XS_SQUARE_GAME` = 16. March: 14 = village, 15 = square. The comment-only
    handling is right.
- **No-code calls:**
  - `CreateMyGageUI` `3248e290` Elsword case = 1 / 10 / 11 / 22 / 100 / 101 / 112 (data `0x11c`, UI `0x80`), March's 7 cases.
  - Vigor x0.7 (`322e0d43`): `fmul double [3344af88]`, and that double is `0.699999988079071`, exactly `(double)0.7f`. March's `*= 0.7f` gives
    the same product. It is bit-equal, not "one ulp" (correcting CH1q2's note). Agree: no code.
- **Routing:** `JUNE_PORT_BACKLOG_ORDER.md` Stage 2 holds the F5 / F6 / F7 training-key bullet (`322ed400`). `stage7-plan.md`'s Elesis line names
  CH1q2's twins. `kickoffs/char-elsword/CH1.md`'s CH1q2 amendment is correct: no change.

**Gates** (range `4d0dbb67^..4d0dbb67`, the six code files):
- `_WOS --with _WOS_DEBUG _LIVE`: `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`. The RESULT is FAIL only for
  the commented-out `#define`.
- `_WOS_DEBUG`: `files: 0 PASS, 0 PASS~, 0 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.
- `3 deleted line(s)`: CH1h's `_LIVE` stand-in, the same 3 lines the Done note names. 0 non-ASCII, 0 EOL-mismatched, 0 token and 0 encoding
  changes. `numstat`: +8 / +2 -3 / +4 / +33 / +6 / +10.
- `junediverge.py --check-all`: `allow-listed: 181`, `RESULT: PASS`.
- `enumscan.py --check` (four `.cpp`): `0 A/B row(s) remain in 4 file(s)`.
- Probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GageData.cpp`, `X2GageUI.cpp`, `X2Game.cpp` and `X2GUElsword_SwordMan.cpp`, each with no flag,
  with `_WOS` and with `_LIVE` + `_WOS`: `PROBES: 0 of 12 failed`.
- No build: no code changed, so the deployed exe is CH1q2's.

**Nothing to play-test.** CH1 waits only on CH0b's play-test. Then: `/x2-phase char-elsword CH1` (Opus 5.5 high).

### CH1 - Done (2026-11-04)

Opus 5.5. **The deploy unit is live.** `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`Always.h:11467`) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (`Always.h:11542`)
are defined, so Elsword runs June's registration, state bodies, virtuals, skill head, pressed-slot space and Way of the Sword (CH1a-CH1q2); the shared
WoS sites change Elesis's March class too, as June does. `_WOS_DEBUG` stays on for the play-test (the review retires it). CH0b's play-test: user, "passed".
- **Script:** `luasrc.py adopt-june data036 Elsword_SwordMan.lua WayOfSword.lua`: June's files replace March's whole as `ljconv` bytecode
  (225,819 B sha1 `23db4786`; 1,759 B sha1 `fbaaf25a`; no core-list row, no edit). Record `june2017_re/june_adopt_20261104_194902.tsv`, backup
  `24965799\kom_backup_stage_june\20261104_194902`. `DLG_Elsword_WayOfSword.lua` lives in data034 and is June's bytes already (dry run: 1 identical).
  WayOfSword's 3 changed values go live (`Const_DestRageWSPGain` 1500 -> 2250, two command constants 96 -> 67.2; CH1q). The loader opens
  `ELSWORD_SWORDMAN.lua` (`X2GUUser.cpp:45680`), the kom's `Elsword_SwordMan.lua` (case-insensitive).
  `PACK OK data036` (vs live ~2 changed: the two scripts); `COPY OK data036 ... 14033385 bytes, 270 entries, md5 90445015d040da4d93887fa27031a6cc` (user approved).
- **Motions:** none held; CH0b copied data033 (11 class motions) and data199 (54 June-only).
- `luabind.py check Elsword_SwordMan WayOfSword DLG_Elsword_WayOfSword`: 0 / 0 / 17 methods, 0 unbound, `RESULT: PASS`.
- **`[JUNE_CHAR] ELSWORD DATA`** (`_LIVE`, end of `InitState`, offline_server.log): class, script, motion, state count, `wait_state_change`, as Chung's.
  `X2GUElsword_SwordMan.cpp` includes `Offline/X2OfflineLog.h` under `_LIVE` too (it was only under `_WOS_DEBUG`).
- **7h:** June's `ESSI_WAIT_STATE_CHANGE` has `ANIM_NAME = ""` (`Elsword_SwordMan.src.lua:2371`); Elsword's `CommonStateStart` (`:7118`) hands off to
  `CX2GUUser::CommonStateStart`, which takes it as for Aisha, Eve and Chung (`ChangeAnim( L"" )` false, the pose holds).
- **Effect data:** `EffectSet_ETK_DASH_JUMP_ZZZ` (CH1g) is in live data036 `EffectSet.lua`; `EffectSet_EIS_DASH_XX` was confirmed by CH1g2.
- **Lua load check:** the pack gate parsed every BOM-source `.lua` of data036. Load-time errors of the core-list files Elsword reads (`DamageEffect.lua`,
  `EffectSet.lua`) show only in `ErrorLog.txt`: the play-test compares against `data\ErrorLog_pre_CH1_elsword.txt` (212,757 B, CH0b's run 2026-11-04 04:47).
- **Scans:** `charport_keys.py Elsword_SwordMan`: 280 keys, `june-only=5 neither=5 march-only-site=8 both=262`, the plan's numbers and names
  (june-only `BBT_CHANGE_NOW_HP_PER_SECOND`, `BFT_TIME`, `FORCE_DYNAMIC_CAMERA_INDEX`, `IMMUNITY_EDT_LIST_AT_THIS_STATE`, `RETAIN_CAMERA_STATE`: owners as
  Chung's). `charport_scan.py` (census rerun first, `census.tsv` / key matrix reverted after): `no NUMERICAL_DATA_INDEX` 87 / `no DAMAGE table` 20,
  equal to `plan_charport_scan_20261103.txt`; no ENUM line, so no §4 table.
- **Checklist:** `(Elsword: CH1 ...)` on the intro (l.15), 7h list variants (l.421), 7h USEFUL_VALUE (l.425), 7o (l.507), 7p (l.546), 7t cut-in (l.590),
  7x AT_COMMAND (l.672), 7ad motions (l.757).
- **Divergence gate:** with `_LIVE` on, `junediverge` saw `ReInit`'s `GT_PVP` test (rule b). June `32d1a920` @32d1a9d9 calls vslot 0x10c and takes
  `dec eax / je`: type 1 only, so March's test is right; allow-list row added with that address.

Hygiene: `flagcheck.py JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER,
0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. `_WOS --files KTDXLIB/Always.h`: `RESULT: PASS` (same zeros). `junediverge.py --check X2Lib/X2GUElsword_SwordMan.cpp KTDXLIB/Always.h`:
`allow-listed: 3`, `RESULT: PASS`; `--check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, `_LIVE` + `_WOS` on): `X2GUElsword_SwordMan.cpp`, `X2GUUser.cpp`, `X2GageData.cpp`, `X2GageUI.cpp`,
`X2DamageManager.cpp`, `X2DamageEffect.cpp`, `X2Game.cpp`, `X2GameUnit.cpp`, `X2GUEL.cpp` exit 0, 0 errors.
Build: `DEPLOY OK: X2_offline.exe is 25,970,176 B, dated 2026-11-04 20:03:19, md5 0e9c736ff275691f447f67ffedb2e507, equal to X2/US_SERVICE/x2.exe (2026-11-04 20:03:12); built at main f7ceb11d + 2 uncommitted code file(s).`
`EXESTR OK: 3 of 3 found in X2_offline.exe, 25970176 B, dated 2026-11-04 20:03:19, md5 0e9c736ff275691f447f67ffedb2e507` (`ELSWORD DATA`, `ELSWORD WOS vigor start`, `ELSWORD WOS UI state`).

**Play-test (user) is the gate:** one Elsword per job (units 67 / 66 / 32 / 33 / 25 / 26 / 34; none has a skill in slot 4 / 9); combos, dash, jump, climb,
damage and stand-up, a bounding hit, riding where a mount exists, the gauge into vigor and destruction, awakening, a village walk; `ELSWORD DATA` per class;
ErrorLog against the pre-CH1 copy. Lord Knight's X at the end of Z-Z-Z-Z-Z is avoided (JUNE_BUGS 521). Skills are CH2-CH3's. No JUNE_BUGS row, no drift.

**Play-test (2026-11-04, run 20:19-20:34 on md5 0e9c736f): user "all test passed".** `offline_server.log`:
- `ELSWORD DATA` for all 7 classes, each `script ELSWORD_SWORDMAN.lua motion MOTION_ELSWORD.X wait_state_change 1`; states 1: 147, 10: 157, 11: 155,
  22: 151, 100: 173, 101: 165, 112: 158 (the review checks them against `ch1m_reg_by_class.tsv`); each class the same count on every load.
- `WOS`: `vigor start` x7 (anim speed x1.132, x1.200 on the second jobs), `destruction start` x3 (point -4800), `attack gain` x139 (types 1 and 4),
  `UI state` x37 reaching 2 (x7) and -2 (x3). A `vigor start` right after a load is the gauge keeping its state (`32a348d0`, CH1q2).
- **Open:** no `destruction rage` line (`DoStateDestructionRage`: a hit taken in destruction mode). The review asks for that step once before retiring `_WOS_DEBUG`.
- START / WIN / LOSE: ErrorLog's `C3e-2 roll` / `rebind` lines pick `ESSI_*_START` / `_WIN` / `_WIN2` / `_LOSE` / `_LOSE2` for every job, none `list size 0`.
ErrorLog against `ErrorLog_pre_CH1_elsword.txt`: `KEM_ERROR` 0 / 0, `Motion Error` 0 / 0, `[LUA_SRC]` 7 / 7, `lua error` 3 / 3, no missing animation.
New lines are places and assets the earlier run never opened: optional `.XET` probes (Elsword effect / mesh names), Ruben field textures, the
tutorial event-scene `.swf` / `.gfx` (quest 11010), local-map control names, `result : 47` on two sounds (as Chung's), and `HQ_CUTIN_ELSWORD_1.DDS.GFX`:
March's cut-in table name plus `.gfx`, owned by CH5 (as Chung's). `AddGroupChainQuest` unbound x1 is pre-CH1 (Chung's review). No new routing.

### CH1 - Review (2026-11-04)

Opus 5.5. **Verdict: passes.** One source change: `_WOS_DEBUG` retired (`Always.h:11582`, `//#define`); the five `ELSWORD WOS` lines go dark,
`ELSWORD DATA` stays (`_LIVE` has its own `X2OfflineLog.h` include; `X2GageUI.cpp`'s other log calls sit under their own flags and include).
- **Deploy unit:** data036 `Elsword_SwordMan.lua` 225,819 B sha1 `23db4786...` and `WayOfSword.lua` 1,759 B sha1 `fbaaf25a...`, both header `1B 4B 4C 81`,
  equal to the `june_adopt_20261104_194902.tsv` rows; no `.src.lua` sidecar. `kom.py status`: data036 and data034 `in sync`, +0 / -0 / ~0 vs live.
  data034 `DLG_Elsword_WayOfSword.lua` (sha1 `6301bbf8`) = `ljconv( June's file )`. `EXESTR OK: 1 of 1` `ELSWORD DATA` (wide x1) on md5 `0e9c736f`.
- **`ELSWORD DATA` vs `ch1m_reg_by_class.tsv`:** the tsv gives 119 common + 7 / 17 / 15 / 11 / 22 / 23 / 18 per class (126 / 136 / 134 / 130 / 141 / 142 / 137
  distinct ids, both 21216 paths). Logged - tsv = 21 + the learned-skill sub-states, exact for all 7 classes.
  - **The 21:** base and hook states (as Chung's 18 + 2) plus June's Mega Slash memo pair `0xea` / `0xeb`: every test unit has a 21000 row (level-0 rows count).
  - **The varying part** is June's `SetEquippedSkillLevel` (`32cf5210`, `bChangeAll`): `m_StateList[ id ]` inserts each listed state of a learned skill.
    Lord Knight unit 25 = +11 (Rolling Smash 21205: 4, Counter 21206: 4, Grand Cross 21058: 3); Rune Slayer unit 26 = +2 (Sword Fire 21016: 1, Rune Counter
    21211: 1); units 67 / 66 / 32 / 33 / 34 learned none of the eight. None of these states is a tsv row. The count follows the unit's skills, not its class.
- **START / WIN / LOSE:** only `C3e-2 roll` lines with list size 1 or 2; no `list size 0`, no `re-bind skipped`.
- **Way of the Sword (review run 20:50-20:53, unit 25):** `vigor start` at 4800 (x1.200), `UI state` 2 / -1 / -2 / 1, `destruction start` at -4800, then
  **three `destruction rage` lines** (-4800 -> -2550 -> -300 -> 1950, rage time 6.0; +2250 each = June's `Const_DestRageWSPGain`, CH1q), each followed by
  an `ELSWORD_ANGRY_WIND` hit; the third leaves destruction (`UI state 1 point 1950`).
- **Revenge counter:** not reachable yet. Counter 21206 is CH3a's skill, and an unported skill falls back to March's route (user). The non-zero-hit check
  moves to `CH3a.md`. Data side holds: June's `ESSI_REVENGE_ATTACK` (`Elsword_SwordMan.src.lua:12264`) has `NUMERICAL_DATA_INDEX = 1`,
  `APPLIED_SKILL_ID = RNSI_A_ELK_COUNTER`. The test save edit (21206 slotted on unit 25) was reverted after the run.
- **ErrorLog:** as the CH1 note (`KEM_ERROR` 0 / 0, `Motion Error` 0 / 0, `[LUA_SRC]` 7 / 7). Review run: `JUNE_FX unsupported APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION
  in ELSWORD_ANGRY_WIND`. A stale `_DAMAGEEFFECT_RUNTIME` line: C10b-2 applies the option (`X2DamageEffect.cpp:2632` / `:5344` / `:8178`) but left
  `:2376` logging. Routed to Stage 9 (beside the effect-alpha UI bullet), not an Elsword defect.
- **Checklist:** the 8 `(Elsword: CH1 ...)` marks are at l.16 (the note's l.15), 421, 425, 507, 546, 590, 672, 757. `charport_keys.py Elsword_SwordMan`:
  `june-only=5 neither=5 march-only-site=8 both=262`, the plan's numbers.
- **Ghidra (read-only):** `32cf5210` carries the same 8 skills and state groups as `SetEquippedSkillLevel` under `_LIVE` (21000 `0xea`/`0xeb`; 21010 `0xbe`/`0xbf`
  then the unit test and `SA_EFFECTIVE_TIME`; 21016 `0xc0`; 21205 `0xca`-`0xcd`; 21211 `0x103`; 21206 `0xe4`-`0xe7`; 21035 `0x106` with no null test; 21058
  `0x107`-`0x109`); the C++ enum keeps March's numbers (names map through CH1m / CH1i). `ReInit` `32d1a920` (uncarved, `charport_adis.py`): vslot `0x10c`
  @32d1a9d2, `dec eax / je` @32d1a9db: type 1 only, the allow-list row holds.

Hygiene:
- `flagcheck.py JUNE_CHAR_ELSWORD_LIVE --range 338d7e8f^..338d7e8f`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`.
- `_WOS` (same range): `0 PASS, 2 OTHER, 0 FAIL`, `RESULT: PASS`.
- `_WOS_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`; `RESULT: FAIL` only because the `#define` is now `//#define` (the 1 deleted line is the define).
- `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`; `--check KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
- `DEPLOY OK: X2_offline.exe is 25,968,640 B, dated 2026-11-04 21:04:39, md5 b5cd8664e4a5746a40d1c2bd3a2a26ea, equal to X2/US_SERVICE/x2.exe (2026-11-04 21:04:32); built at main 8ff11683 + 1 uncommitted code file(s).`
- `EXESTR OK: 1 of 1 found` (`ELSWORD DATA`) and `EXESTR OK: 5 of 5 absent` (`ELSWORD WOS vigor start` / `destruction start` / `destruction rage` / `attack gain` / `UI state`), same md5.

CH2a and CH5 are ready (kickoffs checked: each depends only on this review; neither reads an `ELSWORD WOS` line). Nothing to play-test for the
retirement: the next Elsword run shows no `ELSWORD WOS` line and `ELSWORD DATA` per load.

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

### CH2a - Done (2026-11-04)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, `Always.h`; `#error` without `_LIVE`, `JUNE_ENUM_NAMES`, `JUNE_SKILL_NUMERICAL`, `JUNE_CHAR_EVE_CORE`) is defined: Sword Man's seven skills run June's registrations. All 26 class-map rows are read from `charport_adis.py elsword --phase CH2a` listings (+ Ghidra decompiles): **17 written** (8 inside March's function under the flag with March in `#else`, 9 new), **9 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH2a`; `MODS.md` "Stage 7 Elsword, CH2a" has the March vs June damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cf3920` .. `32cf9960`, `32d0fe90`, `32d10020`, `32cf9920` | `ESSI_SI_A_MEGASLASH_MEMO_{READY,CHARGE,FIRE}_*` (9 new) | READY: preload 2 meshes, charge time = 0, `m_kSkillSlotKey.Set( m_iNowSpecialAttack - 1 )`, animation end -> CHARGE; CHARGE: charge time += frame time, `EffectSet_ELK_Gigantic_slash_FX1` at 0.5 s ( JUNE_BUGS 529 ), `FollowSlotSwap` then FIRE at charge >= 1.1 or key released; FIRE: at 0.094 s `EffectSet_SA_Elsword_MegaSlash_Damage` with `GetPowerRate()` ( x1.5 from a 0.5 s charge ) and the hyper flag, animation end -> JUMP_DOWN / WAIT |
| `32d101f0` | `..._UNLIMITED_BLADE_FrameMove` | one function for both states: step dust + hit-list reset at 0.166 / 0.433 / 0.666 / 0.966 ( `RT_BIG_DAMAGE` ) / 1.3, then 1.566 / 1.95 ( normal ) or 1.5 / 1.73 ( hyper: fire-wave + KuAAang particle bursts ); no cut-in call, no hard-coded final hit, no camera crash, no effect set |
| `32cf99d0` | `..._UNLIMITED_BLADE_EventProcess` | the ground back-walk / walk / dash cancels ( 7h, `ELSWORD_GROUND_*` ) |
| `32cf3a40` `32cf3b20` | `..._UNLIMITED_BLADE_Init` `..._TRIPLE_GEYSER_Init` | 2 and 5 preloads ( the hyper Init is not registered ) |
| `32cf3ad0` `32cf9d50` | `..._ASSAULT_SLASH_Start` `_FrameMove` | speed 1200 / hit gap 0.05 without the memo 2 test; FrameMove = `RT_DOWN` after 0.4 s + `CommonFrameMove` ( particles, cut-in are the script's ) |
| `32d08880` `32cfd880` | `..._ROLLING_END_StateStart` `_EventProcess` | the HP cost only while HP / max HP > 0.1; the walk cancel needs the foot on a line |
| = March | Flame Geyser Init / EP, Assault Slash Init / EP, Triple Geyser EP, Rolling StateStartFuture / FrameMove / EventProcess, Rolling End FrameMove | `funcmap.tsv` says which |

**Registration.** `SetMegaSlashSkillFunc` ( the default of both June switches ) is June's memo registration: READY on the slot's normal and hyper state ( no hyper Init ), CHARGE ( 0xea ) and FIRE ( 0xeb ) as their own states. `SetEquippedSkillFuncBySkillSlotIndex` gains the June cases for 21001 / 21027, 21002 / 21028, 21003 / 21029, 21004 / 21030 and 21200 / 21217 ( Rolling also registers its end state ) ahead of the `default:`; June's slots only ( Flame Geyser and Triple Geyser: Init + EventProcess ); where June has no `_HYPER` table the hyper state takes the normal table name. Endurance ( 21201 / 21218 ) has no state in June and keeps the default ( JUNE_BUGS 528 ). `X2SkillTree.h`: eight `RNSI_SA_ES_*` names. New members: `m_fMegaSlashChargeTime` ( June `+0x96f4` ). New macro `ELSWORD_EVENT_TIMER_ONESHOT`. Checklist rows marked `(Elsword: CH2a)` ( 51, 116, 117, 178, 254, 437, 714 ).

**Rolling fix after the play-test ( 2026-11-05 ).** The roll stood still: `m_fRollingSpeedRel` / `m_fRollingHpConsumeRel` are filled in `InitPassiveSkillState` from March's `SI_A_ES_ROLLING` ( 1205 ), never learned in this port. June's Rolling block of `32d1c560` ( `RNSI_A_ES_ROLLING` level, ability `HP_CONSUME_REL` / `SA_TELEPORT_SPEED_REL` at min( level, 10 ) ) now sits in that function under `_SKILL` ( March's block in `#else` ); the rest of the function stays CH4a's.

**Cool time / MP fix after the play-test ( 2026-11-05 ).** Only Endurance had a cool time: June's `CommonStateStart` ( `32057b80` @32057c4c-cb2 ) calls `ReservedSkillStart` when a reserved skill's state starts, and Elsword had no override ( Arme, Eve, Chung do ), so every state-change skill skipped step 512 ( MP, finality stone, cool time ) and the memo refund. `CX2GUElsword_SwordMan::ReservedSkillStart` = `SpecialAttackCooldown( skill, m_fReservedSkillMp )` + `SkillMemoMpRefund( skill, slot-state test )` under `_LIVE`, with the base declaration and call chain extended in `X2GUUser.h` / `.cpp`. Skills now cost MP and set their cool time ( June `m_SkillCoolTime`: Unlimited Blade 16 s, Assault Slash 6 s, Triple Geyser 20 s, Flame Geyser 10 s, Mega Slash 6 s, Rolling 3 s ); the dev gate ( auth 3 ) still lets you recast during it.

**Play-test (2026-11-05).** Passed after the two fixes above; the review also checks `ReservedSkillStart` and the Rolling passive block against June.

**Left to later phases ( scope rule ).** The other blocks of June's `InitPassiveSkillState` ( CH4a, routed by CH1b ). The 4 numerical-only tables ( Rolling, Unlimited Blade, Assault Slash x2 ) run through the engine; `NUM use` comes from the play-test. Upgrade ids need no templet swap here: no June body reads a skill id.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h X2Lib/X2SkillTree.h KTDXLIB/Always.h`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS` ( the `--range` line follows in the commit message check ).
- `junediverge.py --check` the three source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check 21000-21004 21026-21030 21200-21201 21217-21218`: `skills 14, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged ( `Elsword_SwordMan.lua` is CH1's ). One-file probe of `X2GUElsword_SwordMan.cpp` ( `/Zs /EHsc` ): exit 0, 0 errors. `ch1m_regcheck.py`: 0 problems.
- DEPLOY OK: X2_offline.exe is 25,973,760 B, dated 2026-11-04 21:42:05, md5 b62d02bcf39cc583f5e716ac5066a48f, equal to X2/US_SERVICE/x2.exe (2026-11-04 21:41:58); built at main 5802a810 + 4 uncommitted code file(s).
- EXESTR OK: 3 of 3 found in X2_offline.exe, 25973760 B, md5 b62d02bc ( `[JUNE_CHAR] ELSWORD SKILL memo fire`, `EffectSet_SA_Elsword_MegaSlash_Damage`, `EffectSet_ELK_Gigantic_slash_FX1` ).

**Play-test hand-off.** Save: unit 68 `SwordManT` ( Sword Man, class 1, lv 99, SP 843; backup `db_backup\els_db_20261104_213356_pre_createchar.sql` ). A Flame Geyser ( 21001 ), S Rolling ( 21200 ), D Endurance ( 21201 ), C Mega Slash ( 21000 ); Q Unlimited Blade ( 21002 ), W Assault Slash ( 21003 ), E Triple Geyser ( 21004 ); slots 4 and 9 empty. For the B bank and the upgrade ids say "swap banks" ( I run `setup2`: the enhanced ids 21028-21030 / 21026 on A S D C and 21027 / 21217 / 21218 on Q W E ).
Dev console ( END key, auth 3 ): `/invin 0`, then any dungeon with enemies in front ( `/stage 1` ). Checks, enemies near:
1. **C Mega Slash.** Tap: ready motion, then charge, then the slash within ~0.1 s of the release. Hold for 1 s: the Gigantic-Slash flash plays at 0.5 s, the slash fires by itself at 1.1 s and hits harder ( x1.5 ). If it stays in ready: `[JUNE_CHAR] ELSWORD SKILL memo` lines.
2. **D Endurance.** Plays the same Mega Slash memo ( June gives it no state, JUNE_BUGS 528 ); not a bug of the port.
3. **A Flame Geyser, E Triple Geyser, W Assault Slash.** June's motion and effect; Assault Slash dashes forward and knocks down after 0.4 s; none ends stuck ( returns to wait / fall ).
4. **Q Unlimited Blade.** Six hits, then the final blast at ~1.95 s; walk / dash cancel only on the ground. In an awakening the hyper version ends with the fire wave at ~1.73 s.
5. **S Rolling.** Plays and rolls forward ( speed rel x 1200 ), ends in the end state and costs 1% max HP ( not below 10% HP ).
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

### CH2a - Review (2026-11-05)

Opus 5.5. **Verdict: passes with one fix.** Read from `charport_adis.py elsword` listings of every CH2a address (+ Ghidra decompiles); all three CH2a commits
(`242d3864`, `bbc0e971`, `2432120b`) covered.
- **Fix:** the Rolling block of `InitPassiveSkillState` read the level without June's upgrade swap. June `32d1c560` @32d1c691 calls `GetSkillLevel( 21200, 0, 0, 1 )`
  (as the CH1m fix, and as 32d1c797 for the next block), so the line is now `GetSkillLevel( RNSI_A_ES_ROLLING, false, 0, true )` under `_SKILL`. Abilities `0x71` =
  `HP_CONSUME_REL`, `0x70` = `SA_TELEPORT_SPEED_REL` at min( level, 10 ), PvE (June's literal 0 = March's 2-argument path): as written. Unit 68 has 21200 and
  21217 both at level 1, so the passed play-test's numbers do not change; no new play-test.
- **Unlimited Blade `32d101f0`:** nine one-shots (`m_fAnimOneshotTimer < t && EventTimer( t )` = March's `EventTimerOneshot`), each `CreateStepDust` (vslot 0x7ec) +
  `hitUnitList.resize( 0 )` (+0x7f0): 0.166, 0.433, 0.666, 0.966 (+ `+0x3c4` = 2 `RT_BIG_DAMAGE`), 1.3; byte +0x10 behind +0x7cbc = 0 -> 1.566, 1.95, else 1.5, 1.73.
  At 1.73: fire wave (+0x97b4) at the position y + 10.0, land = the same y, emit 10 / 10, trigger 4; KuAAang (+0x97a8) only with +0x41f9, at `GetProj3DPos( GetPos() )`,
  z 0, 100 / 100, trigger 1 (March's own Unlimited Blade gates KuAAang on `GetShowSmallGageAndName()`, 6 sites); `CommonFrameMove` last. = port.
- **Mega Slash memo:** READY `32cf3920` (2 meshes) / `32cf3950` (+0x96f4 = 0) / `32cf3970` (`Set( m_iNowSpecialAttack - 1 )`) / `32cf9960` (end -> 0xea);
  CHARGE `32d0fe90` (CommonFrameMove, += +0x2b0, 0.5 one-shot `EffectSet_ELK_Gigantic_slash_FX1` with all defaults) / `32cf39b0` (`FollowSlotSwap`, `>= 1.1` or
  `IsKeyReleased` -> 0xeb, CommonEventProcess); FIRE `32d10020` (0.094 one-shot, vslot 0x204 x1.5 when 0.5 <= charge, `PlayEffectSet( name, this, 0, hyper byte,
  rate, -1, (1,1,1), 0, 0s, 1.0 )`). = port, argument for argument.
- **Rolling end** `32d08880`: now / max HP > 0.1 (double 0.1), cost = max x +0x9678, vslot 0xec( -cost, 0 ) only while now HP > 0 = `UpHPIfAlive`. `32cfd880`: end ->
  WAIT / JUMP_DOWN by foot, else `ELSWORD_GROUND_WALK_CANCEL` (+0x8c98). Unlimited Blade EP `32cf99d0`: back-walk (+0x8c94), walk (+0x8c98), dash (+0x8c9c), all
  foot-gated = the three 7h macros. Assault Slash Start `32cf3ad0` 1200 / 0.05, FrameMove `32cf9d50` > 0.4 -> `RT_DOWN` (3). Triple Geyser Init: 2 skin + 1 mesh + 2 tex.
- **"= March" rows:** `32cf3a10` (FlameGager.x + Land.x = March's `NO_MORE_EFFECT_HARD_CODING_TEST` branch, defined), `32cf3aa0`, `32cf9990`, `32cf9d80` (14 keys =
  `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC`, > 1.1), `32cf9f20`, `32cf41d0` (x 1200.0 of +0x9684), `32cf4200`, `32cfd840` (end + foot -> 0xd9), `32d137a0` (0.03333
  `EffectSet_ES_ROLLING`): all equal March's bodies.
- **Registration:** the 15 `ch0_skill_listing_rows.tsv` rows (32d223e0) = the port slot for slot: the hyper READY has no Init, CHARGE 234 / FIRE 235 / ROLLING_END 217 by
  id, Unlimited Blade's hyper table `_HYPER`, the other hyper tables the normal name, Flame / Triple Geyser Init + EventProcess only. No March callback in a June case.
- **`ReservedSkillStart` (fix 2):** June `32057c4c-cb2`: vslot 0x800( skill, `m_fReservedSkillMp` ), then 0x764( skill, slot-state test, mp ). Elsword's override = Eve's /
  Chung's / Arme's; its inline loop = `IsSkillSlotState` (`X2GUUser.cpp:16720`). Base declaration and call only when no other class `_LIVE` provides them.
- **`_SKILL_DEBUG` retired** (`Always.h:11616`, `//#define`): the `ELSWORD SKILL` equip / memo / rolling-end lines go dark.

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range 242d3864^..242d3864`: `2 PASS, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL`, 0 deleted, `RESULT: PASS`;
`--range bbc0e971^..bbc0e971`: `1 PASS`, `RESULT: PASS`; `_LIVE --range 2432120b^..2432120b`: `4 PASS`, `RESULT: PASS`. `git diff --patience --numstat 242d3864^ 2432120b`: 0 deletions
in all 6 files. This review's worktree: `1 PASS, 1 DEFINE, 0 FAIL`, `2 deleted line(s)` (the replaced `_SKILL` line and the retired `#define`), `RESULT: PASS`.
`junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. One-file probe (`/Zs /EHsc`, US_SERVICE defines): exit 0, 0 errors.
DEPLOY OK: X2_offline.exe is 25,972,736 B, dated 2026-11-05 04:52:23, md5 9e1bf0aac452f242972409013977ce31, equal to X2/US_SERVICE/x2.exe (2026-11-05 04:52:16); built at main 5c761eb6 + 2 uncommitted code file(s).
EXESTR OK: 1 of 1 absent in X2_offline.exe, 25972736 B, md5 9e1bf0aa ( `ELSWORD SKILL memo fire`, wide ).

### CH2b - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h` for this phase; the review retires it): Sword Knight's six skills run June's registrations. All 11 class-map rows are read from `charport_adis.py elsword --phase CH2b` listings: **5 written** (2 inside March's functions under the flag with March in `#else`, 3 new incl. the registration cases), **6 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH2b`; `MODS.md` "Stage 7 Elsword, CH2b" has the March vs June damage table. The class map called Spiral Blast's Init / EventProcess "new": March has both under the same names, so they are "= March" rows, not copies.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cf4de0` `32d05530` | `ESSI_SA_ESK_MORTAL_BLOW_FrameMove` `_EventProcess` (new) | FrameMove = `CommonFrameMove`; EventProcess: end -> JUMP_DOWN / WAIT, then the ground-only up-down ( `+0x8ca4` ), skill, back-walk, walk, dash cancels, then Z / X attack cancel ( `+0x8c8c` ) |
| `32cfa6c0` | `ESSI_SI_A_ES_KICK_Start` | force-down value = -ability `SA_FORCE_DOWN_ABS` of the equipped skill ( upgrade swap, level >= 1 ); no memo 7 power gain |
| `32cfed60` | `ESSI_A_ELK_IMPACT_SMASH_EventProcess` | dash / walk cancels need the foot on a line ( `ELSWORD_GROUND_*`, 7h ) |
| = March | Spiral Blast Init / EP, Armor Break Init / EP, Impact Smash Init, Kick EP, Double Slash EP | `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains the June cases for 21005 / 21031, 21006 / 21034, 21007 / 21043, 21202 / 21219, 21203 / 21225 and 21204 / 21226 ahead of the `default:`; the hyper state takes the normal table name ( no June `_HYPER` table ) and the callbacks June fills: Spiral Blast Init + EP, Mortal Blow FrameMove + EP, Double Slash EP, Kick Start + EP, Impact Smash Init + EP, Armor Break Init + EP. `X2SkillTree.h`: twelve `RNSI_SA_ESK_*` / `RNSI_A_ESK_*` names ( values checked against `Enum.src.lua` ). Checklist rows marked `(Elsword: CH2b)` ( 51, 116, 117, 124, 125, 178 ). `JUNE_BUGS.md` 530 ( the 21006 zero row ).

**Left to later phases ( scope rule ).** None new: the three numerical-only state tables ( Kick, Impact Smash, Armor Break ) and the damage effects run through the engine; `NUM use` comes from the play-test. Upgrade ids need no templet swap here: the only body that reads a skill id is Kick Start, which passes the swap argument itself. Memo effects ( Spiral Blast memo 9, Kick memo 7, Armor Break memo 11 ) follow June: none in C++.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the four code files: `1 PASS, 1 PASS~ ( .h: one blank line ), 1 DEFINE, 1 OTHER ( X2SkillTree.h: JUNE_ENUM_NAMES ), 0 FAIL, 0 ERROR`; `1 deleted line(s)` ( the `//#define` turned into `#define` ), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check` the 12 ids: `skills 12, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probe of `X2GUElsword_SwordMan.cpp` ( `/Zs /EHsc` ): exit 0.
- DEPLOY OK: X2_offline.exe is 25,975,296 B, dated 2026-11-05 05:15:56, md5 7154ba9fe01be3a5c92b83cd2280574b, equal to X2/US_SERVICE/x2.exe (2026-11-05 05:15:49); built at main 675cf00d + 4 uncommitted code file(s).
- EXESTR OK: 1 of 1 found in X2_offline.exe, 25975296 B, md5 7154ba9f ( `[JUNE_CHAR] ELSWORD SKILL kick: skill`, wide ).

**Play-test hand-off.** Save: unit 66 `ElswordKnight` ( Sword Knight, class 10, lv 99, SP 843; backup `db_backup\els_db_20261105_051638_pre_sql.sql` ). Slots: A Kick ( 21202 ), S Impact Smash ( 21203 ), D Armor Break ( 21204 ); Q Spiral Blast ( 21005 ), W Mortal Blow ( 21006 ), E Double Slash ( 21007 ); slots 4 and 9 empty. For the B bank and the enhanced ids ( 21219, 21225, 21226 / 21031, 21034, 21043 ) say "swap banks" ( I run the enhanced setup ).
Dev console ( END key, auth 3 ): `/invin 0`, then a dungeon with enemies in front ( `/stage 1` ). Checks, enemies near:
1. **A Kick.** June's kick motion, hits once, force-down value -50 ( the log line `kick: ... force down -50.0` ); ends in wait / fall.
2. **S Impact Smash.** Two turning hits then the swing, stun chance; on the ground a walk or dash key cancels it late in the motion; ends in wait.
3. **D Armor Break.** One strong hit, the target's defence drops ( 30%, 7 s ); ends in wait.
4. **Q Spiral Blast.** Slash then sword blast hits; any key after 2.4 s cancels to wait.
5. **W Mortal Blow.** Rocks shoot up ( new state ); late in the motion up / down, walk, dash, a skill key or Z / X cancels it on the ground; ends in wait. In an awakening the same state plays ( no hyper table ).
6. **E Double Slash.** Two-part slash; keys cancel to wait from 3.667 s.
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

### CH2b - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Every CH2b address read from `charport_adis.py elsword` listings (Kick Start also decompiled in Ghidra); commit `efe978f9`.
- **Play-test run** (06:42-06:44, after the 05:15 deploy, md5 7154ba9f): all twelve ids equipped (`equip skill` lines, both banks), Kick 21219 `level 1 force down -50.0`
  x4, `Effectset_A_ELK_IMPACT_SMASH` x4, `EffectSet_SI_SA_ESK_MORTAL_BLOW_Ultimate` x6, `EffectSet_SA_Elsword_DoubleSlash` x2, `EffectSet_SA_Elsword_SpiralBlast` x1; no
  `EXCEPTION`, 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`, not this phase).
- **Mortal Blow EP `32d05530`:** end -> JUMP_DOWN (0x11) / WAIT (0xd) by foot; then `0 <= +0x8ca4 < t`, foot, `pureUp || pureDown` -> WAIT; skill `+0x8c90` (foot,
  vslot 0x7f4 with `ASUT_GROUND` = 1) falling through to the hyper branch (black hole, `oneLCtr`, count > 0, vslot 0x560, `+0x8c90`, quick-slot wait, `+0x8d4c` = 1) =
  `SKILL_CANCEL_AFTER` under `JUNE_BUFF_AWAKE_LOCK`; back-walk `+0x8c94`, walk `+0x8c98`, dash `+0x8c9c` = the three 7h macros; Z then X on `+0x8c8c` with the foot,
  vslots 0x4b0 / 0x4b4 (the `push 0` is StateChange's bool); `CommonEventProcess` last. FrameMove `32cf4de0` = a jump to vslot 0x7cc. = port, branch for branch.
- **Kick Start `32cfa6c0`:** CommonStateStart; `31fca8e0( 0x52d2 = 21202, 1 )`; `+0x867c` non-null; `32890620( id, 0, 0, 1 )` floored at 1; `32206e50( 0x2a, level, 0,
  0.0 )` (SA 42 = `SA_FORCE_DOWN_ABS`, `X2SkillTree.cpp:612`; literal PvE 0 = `false`), `fchs` -> `+0x790`. No memo lines. = port.
- **Impact Smash EP `32cfed60`:** end; dash `+0x8c9c` (foot); walk `+0x8c98` (foot) -> WAIT; skill / hyper on `+0x8c90`. = port (`_GROUND_DASH`, `_GROUND_WALK`, `SKILL_CANCEL_AFTER`).
- **"= March" rows:** `32cf3b00` (SpiralBlast01.X), `32cf3f10` (Armor_Break_Impact01.Y), `32cf4530` (7 textures, 2 skins, 1 mesh, same order), `32cf9e50` (14 keys =
  `_ZXC`, `2.4 < t`), `32cfa740` (no foot -> JUMP_DOWN and y -= 45.0 = `LINE_RADIUS * 1.5`; 12 keys = `_C`, `0.5 < t`), `32cfadc0`, `32cfc430` (`_ZXC`, `t >= 3.667`):
  all equal March.
- **Registration:** the 12 `ch0_skill_listing_rows.tsv` rows (`32d223e0`) = the six cases slot for slot (hyper: Mortal Blow FM + EP, Kick Start + EP, the rest EP only; normal
  table names for every hyper state). March's Double Slash FrameMove (1.9 s stun removal) and StateEnd are not in June's case: June wins, as ported.
- **JUNE_BUGS 530:** 21006 has NUMERICAL_DATA1 only; `Fire_Ground` asks INDEX 2 -> June's zero row. Verbatim, engine-side, no C++. Agreed.
- **`#else`:** Kick Start and Impact Smash EP keep March's whole bodies; flag-off = base (flagcheck).
- **`_SKILL_DEBUG` retired** (`Always.h:11616`, `//#define`): the `ELSWORD SKILL` lines go dark.

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range efe978f9^..efe978f9 --files` the four code files: `1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (X2SkillTree.h:
JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`, 1 deleted line (the `//#define`), `RESULT: PASS`. `git diff --patience --numstat`: code files pure insertion (141 / 6 / 17), Always.h 1 / 1.
`junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s) remain`. One-file probe (`census.cl_cmd`, `/c /Zs
/EHsc`): exit 0, 0 errors. Build: not awaited (only change is the `_DEBUG` retirement). CH2c kickoff amended (below the CH1p block).

### CH2c - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h` for this phase; the review retires it): Magic Knight's six skills run June's registrations. All 18 class-map rows are read from `charport_adis.py elsword --phase CH2c` listings: **8 written** (7 inside March's functions under the flag with March in `#else`, 2 new incl. the registration cases), **10 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH2c`; `MODS.md` "Stage 7 Elsword, CH2c" has the March vs June damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cf3bd0` `32cfa080` | `ESSI_SI1_ES_WIND_BLADE_StateStartFuture` `_UPGRADE_EventProcess` (new) | upgraded Wind Blade (21035): StartFuture = `CommonStateStartFuture` + `m_kSkillSlotKey.Set( slot )`; EventProcess: `FollowSlotSwap`, end, then flag 0 + `stateChange < t`: MP x ( 1 - `SA_MP_CONSUME_REL` ) (June `32007c40`) paid with `UpNowMp` and `StateChange( ESSI_SI1_ES_WIND_BLADE_ULTIMATE )`; else a key press (`IsKeyPressedOnce`) inside the input window sets flag 0 |
| `32cfa270` | `ESSI_SI3_ES_STORM_BLADE_FrameMove` | 0.3 s one-shot: upgraded skill (`IsNowStateSkillUpgraded`) applies the base skill's buff factor; no cut-in, effect set or particles |
| `32d123f0` | `ESSI_SA_EMK_BIG_BURST_FrameMove` | only the 0.65 s damage effect; no memo 12 branch, no particles |
| `32cf45c0` | `ESSI_A_ES_SWORD_WAVE_Init` | six more preloads |
| `32cfef60` `32cffd60` | `ESSI_A_ES_SWORD_WAVE_EventProcess` `ESSI_SI_A_ERS_CRITICAL_SWORD_EventProcess` | dash / walk cancels need the foot on a line (7h) |
| `32cffcf0` | `ESSI_SI_A_ERS_CRITICAL_SWORD_FrameMove` | the equipped skill's templet is read with the upgrade swap |
| = March | Wind Blade Init / EP, Rising Slash Init / EP, Storm Blade Init / EP, Big Burst Init / FrameMoveFuture / EP, Critical Sword Init | `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains the June cases for 21012 / 21032, 21013, 21035, 21014 / 21048, 21207 / 21220, 21208 / 21229 and 21209 / 21230 ahead of the `default:`; the hyper state takes the normal table name and the callbacks June fills (Wind Blade 21013: Init + EP, hyper EP; 21035 also StartFuture and the callback-less state 262). March's Rising Slash / Wind Blade FrameMoves (cut-in, meshes, damage effects) are not in June's case: June wins, the script plays them. `X2SkillTree.h`: eleven `RNSI_*EMK*` names (values checked against `Enum.src.lua`; 21035 existed). `Always.h`: new `#error` for `JUNE_CHAR_CHUNG_CORE` (`IsKeyPressedOnce`). New helper `TryGetAcquiredSkillAbilityValue` in `_CONST_ELSWORD_CH2C_` (June `32007c40`, as Chung's and Arme's copies). Checklist rows marked `(Elsword: CH2c)` (51, 116, 117, 178). `JUNE_BUGS.md` 531 (the 21013 zero row).

**Judgement for the review.** The class map called the upgraded Wind Blade EP "inside" March's `ESSI_SI1_ES_WIND_BLADE_EventProcess`, but June registers two different functions (21013 -> `32cfa040` = March, 21035 -> `32cfa080`), so the second is a new function `ESSI_SI1_ES_WIND_BLADE_UPGRADE_EventProcess`. Rising Slash Init `32cf3c00` is "= March": June's memo preloads sit behind a stub that is false, and March's `GetEqippedSkillMemo` is false under `DIVERGE_SKILL_MEMO`.

**Fix after the first play-test (2026-11-05).** The logs showed the transition (`wind blade ultimate: skill 21035 level 1 mp 50.0`) but the character then stood frozen: state 262 had no EventProcess. June gives it `329cb6d0` (a jump to `31fcc350` = `CX2GUUser::GenericGroundComboEndEventProcess`, Eve CH4c); CH0's `ch0_skill.tsv` row had the callback, the registration listing had lost its register name. The ultimate state now registers `GenericGroundComboEndEventProcess`; `Always.h` `#error`s without `SERV_IRUHADEV_JUNE_GROUND_COMBO_END_EVENT`. Re-built: DEPLOY OK md5 2f267087 (25,977,344 B, 2026-11-05 08:03:18); EXESTR OK. The first play-test passed for Big Burst, Sword Wave, Critical Sword, Rising Slash, Storm Blade (+ upgraded buff factor) and the Wind Blade cast; only the ultimate needs the retest.

**Left to later phases (scope rule).** None new: the numerical-only Big Burst table and the damage effects run through the engine; `NUM use` comes from the play-test. Upgrade ids need no templet swap beyond the two swap arguments June passes itself.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the four code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `1 deleted line(s)` (the `//#define`), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check` the 12 ids: `skills 12, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probe of `X2GUElsword_SwordMan.cpp` (`/Zs /EHsc`): exit 0. The six new preloads are in `ch0_assets.tsv` (CH0b).
- DEPLOY OK: X2_offline.exe is 25,976,832 B, dated 2026-11-05 07:30:07, md5 e6c899c7f755588e9443d21f94cb87e0, equal to X2/US_SERVICE/x2.exe (2026-11-05 07:30:00); built at main cdc684b7 + 4 uncommitted code file(s).
- EXESTR OK: 3 of 3 found in X2_offline.exe, 25976832 B, md5 e6c899c7 ( `[JUNE_CHAR] ELSWORD SKILL wind blade ultimate`, `[JUNE_CHAR] ELSWORD SKILL storm blade`, `A_ES_SWORD_WAVE_BLADE_Projectile_NEW.X`, wide ).

**Play-test hand-off.** Save: unit 69 `MagicKnight99` (Magic Knight, class 11, lv 99, SP 843; backup `db_backup\els_db_20261105_072024_pre_createchar.sql`, slots set after `...072039_pre_sql.sql`). Slots: A Big Burst (21207), S Sword Wave (21208), D Critical Sword (21209); Q Rising Slash (21012), W Wind Blade (21013), E Storm Blade (21014); C and slots 4 / 9 empty. For the B bank and the enhanced ids (21220, 21229, 21230 / 21032, 21035, 21048) say "swap banks" (I run the enhanced setup).
Dev console (END key, auth 3): `/invin 0`, then a dungeon with enemies in front (`/stage 1`). Checks, enemies near:
1. **A Big Burst.** June's dash-and-punch motion, the explosion damage at ~0.65 s; any direction / attack / skill key cancels to wait from ~1.06 s; ends in wait / fall.
2. **S Sword Wave.** The sword-wave projectile flies; on the ground walk or dash cancels it late in the motion; ends in wait / fall.
3. **D Critical Sword.** Sword enchant, a buff at 0.333 s ( enhanced version: the upgraded buff ); the same ground-only cancels.
4. **Q Rising Slash.** June's slash and magic square; ends in wait ( no March flare lines: the script plays the effects ).
5. **W Wind Blade.** Wind blades and the damage; ends in wait. Enhanced ( swap banks ): hold the W key through the motion and press it again inside the window: the ultimate state starts and the MP is paid once; log `wind blade ultimate: skill 21035 ...`.
6. **E Storm Blade.** Storm blades and the damage; the enhanced one applies a buff at 0.3 s ( `storm blade: upgraded buff factor` ).
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

### CH2c - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commits `41367178` + fix `fab34725`; every address read in Ghidra (decompile + disassembly) or `charport_adis.py elsword`.
- **Play-test retest** (08:33, after the 08:03 deploy, md5 2f267087): all twelve ids equipped (`equip skill` lines, B bank 21032 / 21035 / 21048); `wind blade ultimate: skill 21035
  level 1 mp 50.0` x3, each followed by `ES_CREATE EffectSet_SA_EMK_WindBlade_Ultimate` and the projectile's `HIT`, then more skills, a cut-in and kills: no freeze. 2
  `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`, not this phase), 0 `EXCEPTION`.
- **Wind Blade upgrade EP `32cfa080`:** `31fce670` on `+0x8b58` (FollowSlotSwap); end -> 0x11 / 0xd by `+0x7e1c`; flag `+0x7d85` and `+0x8c18 < t` -> vslot 0x524, `+0x867c`,
  `32890620( id, 0, 0, 1 ) > 0`, `31fd1840( id, level, 1 )` (= the 2-argument virtual, slot 424 with true), `32007c40( 0x1b, id, 0, &rel )` (= `SA_MP_CONSUME_REL`, the helper
  equals Chung's copy), `( 1 - rel ) x mp <= vslot 0xd8` -> `StateChange( 0x106 )` then vslot 0xf0 `( -mp )`; else `+0x8c10 <= t < +0x8c14` and `31fc8900` -> flag. A missing
  ability skips the transition. The `+0x8e90` anim choice is read as `m_pXSkinAnimFuture` (CH1b). = port. **StartFuture `32cf3bd0`:** vslot 0x7c0, then `31fce6d0( +0x88bc - 1 )`. =
  port. The separate function is right: the listing registers `32cfa040` for 21013 and `32cfa080` for 21035.
- **Storm Blade FM `32cfa270`:** 0.3 one-shot; `31fca8e0( vslot 0x524, 1 )->+0x178` (= `IsNowStateSkillUpgraded`, 329cc340) around `31fca8e0( 0x5216 = 21014, 1 )`; vslot
  0x228( templet, 0 ). **Critical Sword FM `32cffcf0`:** 0.333 one-shot, `31fca8e0( 0x52d9 = 21209, 1 )`, `+0xa0 != +0xa4` (buff list non-empty), vslot 0x228. **Big Burst FM
  `32d123f0`:** 0.65 one-shot, `Dummy2_Lhand`, vslot 0x150 twice, land y `+0x80c4`, the one damage effect, defaults. All = port.
- **Sword Wave / Critical Sword EP:** the two listings are instruction-identical (address-stripped diff); dash `+0x8c9c` and walk `+0x8c98` with the foot, then
  `SKILL_CANCEL_AFTER( +0x8c90 )` (foot, then the awakening branch) = the macros. **Sword Wave Init `32cf45c0`:** the ten preloads in order.
- **"= March" rows:** the five Inits (meshes / textures and order; Rising Slash's memo pair behind the `31fca9e0` stub), the three base EPs (0x11 / 0xd, CommonEventProcess), Big
  Burst EP (14 keys, `1.06`) and FrameMoveFuture (0.2 dash speed, 0.65 `-= 1280.0` = `1600 x 0.8`): all equal March.
- **Registration:** the 15 listing rows = the seven cases slot for slot (21035 normal: Init, StartFuture, EP; hyper: StartFuture + EP). State 262 in June (`@32d22ae7-32d22aff`)
  gets only `+0x48 = 329cb6d0` (EP), so the fix is the whole registration.
- **JUNE_BUGS 531:** `DamageEffect_SA_EMK_WindBlade_Ultimate_Projectile` has `APPLIED_SKILL_ID = RNSI_SA_EMK_WIND_BLADE` and `NUMERICAL_DATA_INDEX = 2`; 21013 has one block. Agreed.
- **`#else`:** Storm Blade / Big Burst FM, Sword Wave Init / EP and Critical Sword FM / EP keep March's bodies; flag-off = base (flagcheck).
- **Forward (CH3a):** the listing lost a register name on one more state, 204 `ESSI_A_ELK_ROLLING_SMASH_LANDING`: June gives it EP `329cbfc0` (a jump to `31fcc740` =
  `GenericSpecialActiveSkillEventProcess`, CH1k), `@32d232a1`. Added to the CH3a kickoff, with a general check on CH2d's.
- **`_SKILL_DEBUG` retired** (`Always.h:11622`, `//#define`): the `ELSWORD SKILL` lines go dark.

**Gates.** flagcheck `_SKILL --with _SKILL_DEBUG --range 41367178^..fab34725 --files` the four code files: `4 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0
FAIL, 0 ERROR`, 1 deleted line (the `//#define`), `RESULT: PASS`; worktree after the retirement `1 DEFINE`, `RESULT: PASS`. `git diff --patience --numstat`: code files pure
insertion (240 / 4 / 16), Always.h 7 / 1. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s)
remain`. One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors. Build: not awaited (only change is the `_DEBUG` retirement).

### CH2d - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h` for this phase; the review retires it): Sheath Knight's six skills run June's registrations. All 19 class-map rows are read from `charport_adis.py elsword --phase CH2d` listings (+ Ghidra for the registration): **12 written** (10 inside March's functions under the flag with March in `#else`, 2 new), **7 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH2d`; `MODS.md` "Stage 7 Elsword, CH2d" has the March vs June damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cfde50` `32cfdee0` `32cfdf70` | `ESSI_SI_SA_ETK_FINAL_STRIKE` / `_SWORD_FALL` / `_CRESCENT_CUT` `_EventProcess` | instruction-identical: end -> JUMP_DOWN / WAIT, then the walk cancel with the foot on a line (7h `ELSWORD_GROUND_WALK_CANCEL`, `+0x8c98`) |
| `32cfe840` `32d005e0` `32d007e0` | `ESSI_SI_A_EIS_SPIRAL_STING` / `_FATAL_SLAP` / `ESSI_A_ETK_SWORD_SHIELD` `_EventProcess` | ground walk + dash cancels ( Spiral Sting: walk then dash; the other two dash then walk ), then `SKILL_CANCEL_AFTER` |
| `32d14100` | `ESSI_SI_SA_ETK_SWORD_FALL_FrameMove` | March's body plus: the four `_Memo` blade meshes and the four last-hit blades always play ( March: memo 15 ), nine `TagEffectAlphaMesh` sites, no 0.6 power scale; the last hit adds two `BLADE4` at y + 80 / + 160 when `IsNowStateSkillUpgraded()` ( JUNE_BUGS 532 ). Helpers `CreateSwordFallBladeMesh` / `CreateSwordFallBladeDamage` carry the eight repeated blocks |
| `32d15f80` | `..._SWORD_FALL_End` | the eight blade handles ( First-Fourth, then the _Memo four ), each destroyed only while its mesh lives |
| `32d1b740` `32d16c00` | `ESSI_SI_SA_ETK_CRESCENT_CUT_UPGRADE_FrameMove` `CreateCrescentCutEffect` (new) | the upgraded Crescent Cut: cut-in, an else-if chain 0.4 / 0.9 / 1.48 calls the helper; it plays the hit's effect set and damage effect, then scales the main effect's life by the skill's `SA_EFFECTIVE_TIME` and multiplies its velocity by `SA_MOVE_SPEED` ( `32007c40`, type 1; JUNE_BUGS 526 ) |
| `32d189a0` `32d18a60` | `ESSI_A_ETK_SWORD_SHIELD_StateStart` `_FrameMove` | the 21212 templet with the upgrade swap, level `max( 1, GetSkillLevel( id, false, 0, true ) )`, effective time from the numerical ability; no unit returns before `CommonStateStart`; the 0.68 s effect set is `Effectset_A_ETK_SWORD_SHIELD_ULTIMATE` when the state skill is upgraded |
| = March | the five Inits ( `32cf4310` `32cf4370` `32cf4400` `32cf4890` `32cf48e0` ), Crescent Cut FrameMove `32d162d0` ( its third-hit memo test is false in March under `DIVERGE_SKILL_MEMO` ), Spiral Sting FrameMove `32d1f3a0` | `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains eight June cases ahead of the `default:`: Sword Fall ( 21019 / 21033: Init, FM, EP, End ), Crescent Cut ( 21020 ) and its upgrade ( 21036, `..._UPGRADE_FrameMove` ), Final Strike ( 21021 ) and its upgrade ( 21053: table `ESSI_SI_SA_ETK_FINAL_STRIKE_UPGRADE`, the same Init / EP ), Sword Shield ( 21212 / 21221 ), Fatal Slap ( 21213 / 21233 ), Mirage Sting ( 21214 / 21234 ). Final Strike registers no FrameMove / End ( March's black-hole and cut-in lines are the script's now ); the hyper state takes the normal table name ( June has no `_HYPER` table for these ). `X2SkillTree.h`: twelve `RNSI_*ETK*` names. Checklist rows marked `(Elsword: CH2d)` ( 51, 116, 117, 178, the 7c row ). `JUNE_BUGS.md` 532 ( Sword Fall's second blade set ); 526 ( velocity y / z zeroed ) is now reachable.

**Judgement for the review.** The class map called Final Strike's upgrade state "new" with its own Init / EP; June registers the same `32cf4310` / `32cfde50` for both, so no `_UPGRADE_` copy exists. The two ability types of the Crescent Cut helper are written `SA_EFFECTIVE_TIME` ( 1 ) and `SA_MOVE_SPEED` ( 0x22 ), counted in the C++ enum as CH2c counted `SA_MP_CONSUME_REL` ( 0x1b ).

**Left to later phases ( scope rule ).** None new: Mirage Sting's two numerical-only state tables and the damage effects run through the engine; `NUM use` comes from the play-test. Upgrade ids need no templet swap beyond the swap arguments June passes itself ( Sword Shield `GetSkillLevel` / `GetEquippedActiveSkillTemplet`, `IsNowStateSkillUpgraded` ).

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the four code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `1 deleted line(s)` ( the `//#define` ), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check 21019-21021 21033 21036 21053 21212-21214 21221 21233-21234`: `skills 12, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probe of `X2GUElsword_SwordMan.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 25,975,808 B, dated 2026-11-05 09:08:43, md5 1c3ad88f89ae8d5731883b3bb44d8ee3, equal to X2/US_SERVICE/x2.exe (2026-11-05 09:08:36); built at main 212cd1be + 4 uncommitted code file(s).
- EXESTR OK: 3 of 3 found in X2_offline.exe, 25975808 B, md5 1c3ad88f ( `[JUNE_CHAR] ELSWORD SKILL sword fall`, `... sword shield`, `... crescent cut hit` ); wide: `Effectset_A_ETK_SWORD_SHIELD_ULTIMATE`, `elsword_A_swordFall_M05_Memo` 2 of 2.

**Play-test hand-off.** Save: unit 70 `SheathKnight99` ( Sheath Knight, class 22, lv 99, SP 843; backups `db_backup\els_db_20261105_085816_pre_createchar.sql`, slots set after `...085846_pre_sql.sql` ). Slots: A Sword Shield ( 21212 ), S Fatal Slap ( 21213 ), D Mirage Sting ( 21214 ), C Mega Slash ( 21000 ); Q Sword Fall ( 21019 ), W Crescent Cut ( 21020 ), E Final Strike ( 21021 ); slots 4 and 9 empty. For the enhanced ids ( 21221 / 21233 / 21234 on A S D, 21033 / 21036 / 21053 on Q W E ) say "swap banks" ( I run the enhanced setup; the skills are already learned ).
Dev console ( END key, auth 3 ): `/invin 0`, then a dungeon with enemies in front ( `/stage 1` ). Checks, enemies near:
1. **A Sword Shield.** The shield effect appears at ~0.68 s and lasts the skill's effective time ( log `sword shield: skill 21212 level 1 effective time ...` ); enhanced: the _ULTIMATE effect; ends in wait / fall.
2. **S Fatal Slap, D Mirage Sting.** June's motions and effects, damage on the hit ( Mirage Sting hits twice: the state's `DAMAGE_DATA` then `DAMAGE_DATA_NEXT` ); walk / dash cancel only on the ground.
3. **Q Sword Fall.** Sheath and blade meshes, then eight blade hits between 0.9 and 1.5 s; enhanced: two more blades stack above the last one ( log `sword fall: upgraded skill 21033, two more blades` ). Ends in wait / fall.
4. **W Crescent Cut.** Three slashes at 0.4 / 0.9 / 1.48 s; enhanced: log `crescent cut hit N: skill 21036 life x ... speed x ...` for each hit.
5. **E Final Strike.** The black hole and the blast ( the script plays them ), ends in wait; enhanced uses the _UPGRADE table.
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

### CH2d - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `3cb75440`; every address read with `charport_adis.py elsword --phase CH2d` (19 listings) plus Ghidra (`329cc340`, strings).
- **Play-test: passed (user, 2026-11-05).** Run 09:25 (exe 1c3ad88f): `sword shield: skill 21221 level 1 effective time 12.50` x4, `sword fall: upgraded skill 21033, two more
  blades`, `crescent cut hit 1 / 2 / 3: skill 21036 life x 1.30 speed x 1.30`; Mirage Sting 21214 `ES_CREATE EffectSet_EIS_SA_SPIRALSTING`, `HIT ELSWORD_SA_SPIRALSTING_M01`, then
  `DAMAGE_DATA_NEXT`. 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`), 0 `EXCEPTION`. Two `OpenXET` misses (`ELSWORD_SA_SPIRALSTING_SHEATH.XET` / `_PHANTOMSWORD.XET`): neither March nor
  June 14JUN17 ships them (only the `.X` meshes), so June misses them too; no work. `_SKILL_DEBUG` retired (`//#define`); build started, not awaited.
- **Sword Fall FM `32d14100`:** 0.01 one-shot plays `EffectSet_SECOND_BLADE`, M01 at `Bip01` + y 60 ( 0 x dir ), tagged, the position stored to `+0x9644` unconditionally; M02-M05 at 0.03333 /
  0.06666 / 0.1 / 0.15 into `+0x9650-+0x965c`, the `_Memo` four at 0.05 / 0.08 / 0.125 / 0.17 into `+0x9660-+0x966c`, each `+0x19c = 1` (per-frame simulation) and `32296ee0`;
  **no memo test** (no `31fca9e0` / memo call in the listing). Power rate = vslot 0x204 once before the hits. Hits 1.0 / 1.16666 / 1.33333 / 1.5 then 0.917 / 1.083 / 1.25 / 1.416,
  each gated on its mesh: `Dummy01` + y 42, -( -59 ) x dir, `BLADE1-4`, `DestroyInstanceHandle`. After the last hit's destroy: vslot 0x524 -> `31fca8e0( id, 1 )` +0x178 = June's
  `IsNowStateSkillUpgraded` (`329cc340`, identical), then +80 -> `BLADE4`, +80 again -> `BLADE4`. = port. **End `32d15f80`:** `+0x9650` .. `+0x966c` in order, then vslot 0x7dc. = port.
  JUNE_BUGS 532 agreed.
- **`CreateCrescentCutEffect` `32d16c00`:** GetPos + y 80, +- 130 dir; `idx - 2 == 0` -> TWO, `- 1 == 0` -> THREE, else ONE (both names); damage effect at vslot 0x204; null -> out;
  `32007c40( 1, id, 0, &v )` with `v = 1.0` -> main effect `+0x44c` x v; `32007c40( 0x22, id, 0, &v )` on the same buffer -> `ChangeMainEffectVelocity( 1, v )`. `SA_EFFECTIVE_TIME` = 1 and
  `SA_MOVE_SPEED` = 34 counted in the C++ enum (no `#if` inside). The port's copy of the buffer after the first call gives the same value. = port. **`32d1b740`:** cut-in ( 0.01, 0 ), an
  else-if chain 0.4 / 0.9 / 1.48 -> helper( 1 / 2 / 3 ), vslot 0x7cc. = port.
- **Sword Shield `32d189a0`:** stop `+0x96e8`; `31fca8e0( 0x52dc = 21212, 1 )`; null -> CommonStateStart; no unit -> `ret`; `32890620( id, 0, 0, 1 )` < 1 -> 1, else the same call
  again; `32206e50( 1, level, 0 )` -> `+0x96ec`. = port. **`32d18a60`:** 0.68 one-shot, upgraded -> `33523da8` `Effectset_A_ETK_SWORD_SHIELD_ULTIMATE`, else `33523d6c`. = port.
- **The six EventProcesses:** `32cfde50` / `32cfdee0` / `32cfdf70` end -> 0x11 / 0xd by `+0x7e1c`, then the `+0x8c98` walk cancel with the foot on a line; `32cfe840` walk then dash
  (vslot 0x668), `32d005e0` / `32d007e0` dash then walk, each then the `+0x8c90` skill cancel. = the macros.
- **"= March":** Crescent Cut FM `32d162d0` = March's three independent one-shots ( GetPos + 80, 130 ); June's third hit has no memo branch, March's resolves to `ELSWORD_CRESCENT_THREE`
  ( `GetEqippedSkillMemo` false, so `GetRandomFloat` is never called ). Inits and `32d1f3a0` per `funcmap.tsv`.
- **Registration:** the eight cases match the listing rows ( 21036 -> `32d1b740`, 21020 -> `32d162d0`, Final Strike and its upgrade share `32cf4310` / `32cfde50`, no FM / End; hyper
  rows = June's normal tables ).
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --range 3cb75440^..3cb75440 --files` the four code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER
  (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`, `1 deleted line(s)` (the `//#define`), `RESULT: PASS`. `--numstat`: `.cpp` +363 -0, `.h` +6 -0, `X2SkillTree.h` +17 -0.
  `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the `.cpp`: `0 A/B row(s) remain`. One-file probe of `X2GUElsword_SwordMan.cpp`, with and
  without `_LIVE`: exit 0, 0 errors. Retirement: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, `1 deleted line(s)`,
  `RESULT: PASS`. DEPLOY: not awaited (standing exception).

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

### CH3a - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h`; the review retires it): Lord Knight's skills run June's registrations. All 38 class-map rows are read from `charport_adis.py elsword --phase CH3a` listings (+ Ghidra for the Grand Cross helper and the registration): **22 written** (12 inside March's functions under the flag with March in `#else`, 10 new), **16 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH3a` (39, with the dead `32cfc230`); `MODS.md` "Stage 7 Elsword, CH3a" has the March vs June damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cf9f60` | `ESSI_SI3_ES_ARMAGEDDON_BLADE_FrameMove` | only the 0.01 s buff factors ( 0, and 1 first when `IsNowStateSkillUpgraded()` ) on the 21008 templet with the memo swap; no cut-in, no particles |
| `32d11cb0` `32cfa9a0` | `ESSI_SA_ESK_WINDMILL_START_FrameMove` / `_FrameMove` | `_NEW` effect sets with `bHyper` false; the buff comes from the raw 21010 templet with the swap, no `BALANCE_PATCH` gate |
| `32d11fd0` `32cf3e20` | `ESSI_SA_ESK_WINDMILL_EventProcess` / `..._FrameMoveFuture` ( new ) | past the duration the skill goes on while the key is held and mana lasts, 2.8 s at most; mana = elapsed x 100 / 2.8 per frame ( slot 0xf0 `UpNowMp` ) |
| `32d12210` `32cf3da0` | `ESSI_SA_ESK_WINDMILL_StateEnd` / `..._START_StateStartFuture` ( new ) | the end plays `EffectSet_ELSWORD_WINDMILL_LAND_END_NEW`; the start remembers the held slot ( `m_kSkillSlotKey.Set` ) |
| `32cfaa30` `32cfc5d0` | `ESSI_SA_ESK_WINDMILL_END_EventProcess` / `ESSI_SA_ELK_SAND_STORM_EventProcess` | animation end, `SKILL_CANCEL_AFTER`, ground back-walk / walk / dash cancels, ground Z / X cancels ( new macro `ELSWORD_GROUND_ATTACK_CANCEL`, `+0x8c8c` ); March's skip-key branch is gone |
| `32d05f00` | `ESSI_SA_ELK_SONIC_BLADE_CRESCENT_EventProcess` ( new ) | `GetUsefulValueFuture( 0, 0 )` ( `+0x8ef4` ) gate in the air, animation end, skill cancel, walk, dash, back-walk |
| `32cfc150` | `ESSI_A_ELK_ROLLING_SMASH_StateStartFuture` | the 21205 templet with the swap, level `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, range = `SA_TELEPORT_RANGE_ABS` ( numerical ) x 100 / duration |
| `32d0eaf0` | `RevengeWaitFrameMove` | March's body without the `SMI_ELSWORD_MEMO5` branch ( 4 s defence memo time ) |
| `32cf4e80` .. `32d1b980` | `ESSI_SA_ELK_GRAND_CROSS_*` ( 11 new ) | START ( Init is a bare `ret` ), CHARGE ( 10 mana per 0.5 s step, 5 steps = full charge, key release or mana < 10 ends it ), END / FULL_CHARGE ( 0.04 s / 0.1 s one-shot -> the helper; the two EventProcesses are instruction-identical, the second calls the first ) |
| `32d19dd0` | `CreateGrandCrossDamageEffect` ( new ) | five damage effects at pos + y 130 + offsets 0 / D / -D / ( -Dz, Dy, Dx ) / ( Dz, -Dy, -Dx ), D = 200 x dir; power rate x ( 1 + 0.1 x steps ), scale 1 + 0.2 x steps, effects 1-4 also scale the main effect's DirSpeed |
| = March | `32cf3650` `32cf3670` `32cf6290` ( Revenge ), `32cf3b70` `32cfa000` ( Armageddon ), `32cf3d70` `32cf3dd0` `32cfa910` `32cfa970` `32cf3e90` ( Windmill ), `32cfc050` ( Sonic Blade ), `32cfc120` `32cfc1f0` ( Rolling Smash ), `32d087a0` ( Sand Storm FMF ) | bodies read against March; `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains ten June cases ahead of the Mega Slash default: Armageddon Blade ( 21008 / 21044 ), Sand Storm ( 21009; 21045 uses the `_UPGRADE` table ), Windmill ( 21010 / 21046; states 190 / 191 registered; the hyper start enters the same 190, so March's `_HYPER` Windmill states stay unused ), Sonic Blade ( 21011 / 21047: EventProcess only ), Sonic Blade: Crescent ( 21037 ), Imperial Crusher ( 21038 ) and Power Smash ( 21222 ) ( both `GenericSpecialActiveSkillEventProcess`, June `329cbfc0` ), Grand Cross ( 21058; states 263-265 ), Rolling Smash ( 21205 / 21227; states 202 and 204, the CH2c-review note: 204 registers only the generic EventProcess ) and Counter ( 21206 / 21228: the Revenge wait ). `X2SkillTree.h`: eleven `RNSI_*ELK*` names. Members `m_fGrandCrossChargeTime` / `m_iGrandCrossChargeCount` ( June `+0x97b8` / `+0x97bc` ) join the class. Checklist rows marked `(Elsword: CH3a)` ( 51, 116, 117, 178, 256, 422, 427, 439, 637, 716; the 7ai BFI row is n/a: its only Elsword call `32cfc2f0` is in the dead function `32cfc230` ).

**Judgement for the review.** ( 1 ) Grand Cross's scale `1 + 0.2 x steps` and the helper's float slots: the `fstp [esp+0x3c]` at `32d19e51` runs while the `GetPos` argument is pushed, so it overwrites the slot of the step count read later by `SetScale`; Ghidra's decompile shows the same. ( 2 ) The Windmill END state shares the mana-drain FrameMoveFuture; in END the state time restarts, so it only drains when the END animation outlasts the Windmill duration ( never in practice ). ( 3 ) Sand Storm: June registers no FrameMove, so March's `LORD_KNIGHT_SAND_STORM` creation ( 7u ) is not reached on the June path; the script plays its own effect. ( 4 ) The Counter's `SMI_ELSWORD_MEMO5` branch is dropped ( June ). No `JUNE_BUGS.md` row: nothing here is a June quirk.

**Left to later phases ( scope rule ).** None new: the three numerical-only state tables ( Rolling Smash, Armageddon Blade, `ESSI_REVENGE_ATTACK` ) and the damage effects run through the engine; `NUM use` comes from the play-test. The Counter strike's hit amount ( kickoff note from CH1 ) is read in the same test.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the four code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `1 deleted line(s)` ( the `//#define` ), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the three source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h`: `0 A/B row(s) remain`. `skillicons.py check 21008-21011 21037-21038 21044-21047 21058 21205-21206 21222 21227-21228`: `skills 16, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probe of `X2GUElsword_SwordMan.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 25,985,536 B, dated 2026-11-05 10:02:21, md5 b4c119c22c516737164daa3ad015b0a0, equal to X2/US_SERVICE/x2.exe (2026-11-05 10:02:14); built at main d2cff7dd + 4 uncommitted code file(s).
- EXESTR OK: 4 of 4 found in X2_offline.exe, 25985536 B, md5 b4c119c2 ( wide: `[JUNE_CHAR] ELSWORD SKILL grand cross`, `... windmill end`, `... rolling smash`, `... armageddon blade` ); wide: `EffectSet_ELSWORD_WINDMILL_LAND_END_NEW`, `DamageEffect_SA_ELK_Grand_Cross_Final_FireBigFX_Full_Charge` 2 of 2.

**Play-test hand-off.** Save: unit 25 `jtLordKnight` ( Lord Knight, class 100, lv 99; backup `db_backup\els_db_20261105_095554_pre_sql.sql` ). All 17 ids are learned at level 1. Setup 1 ( loaded ): A Rolling Smash ( 21205 ), S Counter ( 21206 ), D Power Smash ( 21222 ), C Mega Slash ( 21000 ); Q Armageddon Blade ( 21008 ), W Sand Storm ( 21009 ), E Windmill ( 21010 ), R Sonic Blade ( 21011 ); slots 4 and 9 empty. Say "swap banks 2" for the enhanced ids ( 21227 / 21228 on A S; 21044 21045 21046 21047 on Q W E R ) and "swap banks 3" for Q Sonic Blade: Crescent ( 21037 ), W Imperial Crusher ( 21038 ), E Grand Cross ( 21058 ).
Dev console ( END key, auth 3 ): `/invin 0`, then a dungeon with enemies in front ( `/stage 1` ). Cast each skill twice: the slot icon shows the cool time and the MP bar drops ( ReservedSkillStart ). Checks, enemies near:
1. **A Rolling Smash** leaps and lands ( log `rolling smash: skill 21205 level 1 duration ... speed ...` ), the landing hits; **S Counter**: the wait state, then take a hit: the counter strike deals damage ( `NUM use` > 0, `ESSI_REVENGE_ATTACK`, CH1 note ); **D Power Smash** plays June's motion and effect.
2. **Q Armageddon Blade** ( log `armageddon blade: buff factor 0` ), **W Sand Storm** ( dash, then the storm; ends in wait / fall, cancels into walk / dash on the ground ), **R Sonic Blade**.
3. **E Windmill**: spins at once ( log `windmill: _NEW effect sets, duration ...` ); hold E past the duration: it keeps going while the mana lasts ( up to 2.8 s ), release ends it with the landing set ( log `windmill end: state time ... duration ... mp ...` ).
4. **Bank 3**: Sonic Blade: Crescent ( log `sonic blade crescent` only in the air ), Imperial Crusher, **Grand Cross**: hold E: charge steps every 0.5 s ( log `grand cross charge: step N`, mana -10 each ), release or 5 steps ends it, the five damage effects follow ( log `grand cross effects: full charge 0/1, steps N, power ..., scale ...` ).
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

**Play-test (user, 2026-11-05): passed.** Run 10:09 (exe b4c119c2): Windmill held to the 2.8 s limit and released early, Armageddon Blade 21044 `buff factor 0 and 1`, Rolling Smash 21227 `duration 0.50 speed 1860.00`; Grand Cross `start`, five charge steps (-10 mana each), `full charge 1, steps 5, power 1.50, scale 2.00`, and at 0 mana `steps 0, power 1.00, scale 1.00`; Sonic Blade: Crescent in the air at 0.93 (useful value 0.90). 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`), 0 `EXCEPTION`, `ErrorLog.txt` clean apart from `OpenXET` misses on mesh `.XET` files (review: check June ships none). One refusal, "This skill cannot be used. Please equip another skill": a save-setup error (base and enhanced ids both learned; fixed by level 0 on the enhanced ids); every skill then cast. No `NUM use` lines (the numerical debug is retired). `_SKILL_DEBUG` stays on for the review.

### CH3a - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `e4afa6f2`; every address read with `charport_adis.py elsword --phase CH3a` (38 listings) plus the registration `32d223e0`, `329cc2c0`, `32d27110` / `32d27120` raw, and Ghidra (strings, xrefs, one decompile).
- **Logs.** The game dir's logs now hold the later part of the 10:09 run (exe b4c119c2): `grand cross start` x3, `charge: step 1-5`, `effects: full charge 1, steps 5, power 1.50, scale 2.00` and two `full charge 0, steps 0, power 1.00, scale 1.00`; `armageddon blade: buff factor 0 and 1, skill 21044` x2; `sonic blade crescent: in the air at 0.93, useful value 0.90`. The Windmill / Rolling Smash lines are quoted in the Done note above. 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`), 0 `EXCEPTION`, no `NUM use`. **`OpenXET`:** 244 distinct `.XET` names missed; June's 258 koms (222,811 entries, 8,046 `.xet`) ship **none** of them, so June misses them too: no work.
- **Windmill EP `32d11fd0`:** cost = elapsed x 100.0 / 2.8 (double); END (0xbf) when `T > D + 2.8`, or `T > D` and ( `IsKeyReleased` or cost > `GetNowMp` ); otherwise the foot-gated up (jump speed, y + 30) / down (`CanDown`, y - 45) / else y speed 0, then x = dash speed x `m_fRunJumpSpeedRate` (= March's `GetDashSpeed()`) or 0, then vslot 0x7d4. = port. **FMF `32cf3e20`:** vslot 0x7c8, then `UpNowMp( -cost )` (vslot 0xf0) only for cost <= mana and `T > D`. = port. **StateEnd `32d12210`**, **START FM `32d11cb0`** ( `_NEW` / `_LAND_NEW`, `bHyper` 0, power = vslot 0x204 on the first only, -1 on the second ), **FM `32cfa9a0`** ( BTI 0x13e = 318, templet 0x5212 swap 1, factor 0 ). = port.
- **END EP `32cfaa30` = Sand Storm EP `32cfc5d0`** (instruction-identical): end -> 0x11 / 0xd, then `+0x8c90` skill, `+0x8c94` back-walk, `+0x8c98` walk, `+0x8c9c` dash, `+0x8c8c` Z (vslot 0x4b0 = `32d27110`, `ESSI_COMBO_Z1`) and X (0x4b4 = `32d27120`), all foot-gated. The `push 0` before the getter is StateChange's bool. = `ELSWORD_GROUND_ATTACK_CANCEL`.
- **Grand Cross:** START `32cf4e90` / `32cf4ec0` / `32d061a0`, CHARGE `32cf4ef0` ( t > 0.5 -> `32278070( -10 )`, ++steps, t -= 0.5 ) / `32cf4f60` ( FollowSlotSwap, vslot 0x7d4 first, then >= 5 -> 0x109, released or mana < 10 -> 0x108 ), END `32d1b930` (0.04 -> helper 0) / FULL `32d1b980` (0.1 -> helper 1). = port. **Helper `32d19dd0`:** power = vslot 0x204 x ( 0.1n + 1 ) at `+0x88`; scale 0.2n + 1 stored with GetPos's argument pushed, so it lands at `+0x38` (n's slot); the loop index is `+0x3c`, so `SetScale` and the `+0x5cc` DirSpeed multiply (funcmap row 168) read the scale; Ghidra's decompile shows the same overwrite. y + 130; D = ±dir x 200. Jump table `32d1a360` = `32d1a09b` D / `32d1a0ac` -D / `32d1a100` ( -Dz, Dy, Dx ) / `32d1a10e` ( Dz, -Dy, -Dx ), index 0 = zero. Names by Ghidra string ( `33524270` / `335241f8` .. `33523f18` ) in port order. **`CreateInstance` `32d1a26b`:** 17 values = the port's 15 + March's defaults `bInsertFront` / `bPetSkillEffect` false; rotation twice, `m_FrameDataNow` land y; null -> next. = port.
- **Sonic Blade: Crescent EP `32d05f00`:** `+0x8ef4` <= now time and off the line -> 0x11; animation end -> 0x11 / `m_CommonState.m_Wait` (`+0x4088`); skill, walk, dash, back-walk. = port.
- **Armageddon FM `32cf9f60`** (0x5210 swap; vslot 0x524 -> templet +0x178 -> factor 1, then 0 = `IsNowStateSkillUpgraded`), **Rolling Smash SSF `32cfc150`** (duration 2vy / g before the unit test, `max( 1, 32890620( id, 0, 0, 1 ) )`, ability 0x17 x 100 / duration), **Revenge Wait FM `32d0eaf0`** (0.09 / 0.7 one-shots, no 0.3 memo timer). = port.
- **"= March":** `32cfa910`, `32cf3d70`, `32cf3650`, `32cfc120` (-> 0xca), `32d087a0` (0.3 x 1.2, 0.9333 x 0.8) read against March's bodies: identical.
- **Registration:** every case = the listing rows. Two rows the tool leaves empty are filled in June: Grand Cross START's hyper state (`[edi+0x10] = 1`, then `329cc2c0` copies all nine delegates, = the port's four, the only ones set) and state 204 (`329cbfc0` at `+0x48`, the EventProcess). **`32cfc230` dead:** no Ghidra xref, no absolute dword in the June exe; 7ai n/a stands.
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --range e4afa6f2^..e4afa6f2 --files` the four code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`, `1 deleted line(s)` (the `//#define`), `RESULT: PASS`. `--numstat`: `.cpp` +638 -0, `.h` +19 -0, `X2SkillTree.h` +16 -0. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the `.cpp`: `0 A/B row(s) remain`. One-file probe of `X2GUElsword_SwordMan.cpp`, with and without `_LIVE`: exit 0, 0 errors. Retirement: `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, `1 deleted line(s)`, `RESULT: PASS`. `_SKILL_DEBUG` retired (`//#define`); build started, not awaited.

### CH3b - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h`; the review retires it): Rune Slayer's ten skills ( 16 ids ) run June's registrations. All 22 class-map rows are read from `charport_adis.py elsword --phase CH3b` listings (+ Ghidra for the registration `32d223e0` raw and the modifier switch `321db1b0`): **15 written** (4 inside March's functions under the flag with March in `#else`, 7 new, the tag-19 case and the registration cases), **11 are June = March** (no code). `funcmap.tsv` rows `Char Elsword CH3b` (24); `MODS.md` "Stage 7 Elsword, CH3b" has the March vs June damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cf3ee0` | `ESSI_SA_EMK_SWORD_FIRE_LANDING_End` | the buff templet is `GetEquippedActiveSkillTemplet( 21016, swap )`, not March's `SI_SA_EMK_SWORD_FIRE` |
| `32d09290` | `ESSI_A_ESK_PHOENIX_TALON_FrameMove` | the 1.75 s buff reads the raw 21015 templet with the swap; June registers this body for the hyper state too ( March's `_Hyper_FrameMove` is not reached ) |
| `32cfff60` | `ESSI_SI_A_ERS_RUNE_OF_FIRE_EventProcess` | dash, walk and skill cancels need the foot on a line ( 7h ) |
| `32d18230` | `ESSI_SI_A_ERS_RUNE_OF_FIRE_FrameMove` | after the life time x range percent and damage scale, `AddDamageEffectModifier( handle, 19, range percent, false )` ( checklist 7a ) |
| `321db1b0` | `CX2EffectSet::ApplyDamageEffectModifiers` case 19 | `m_bInheritMainEffectLifeTimeRate` = true at once; with a main effect `m_fMainEffectLifeTimeRate` = v and its max life time x v ( whatever the entry's b ) |
| `32cf4e50` `32d05500` | `ESSI_ERS_COUNTER_GUARD_READY_StateStartFuture` `_EventProcess` ( new ) | the ready state zeroes the hold time and remembers the held slot; animation end -> state 259 |
| `32cf4d70` | `ESSI_ERS_COUNTER_GUARD_EventProcess` ( new ) | hold time += frame time, `FollowSlotSwap`, key released or hold time > 5.0 -> `ESSI_ERS_COUNTER_GUARD_END` |
| `32d05b80` `32d068d0` | `ESSI_ERS_COUNTER_GUARD_END_EventProcess` `ESSI_SA_ERS_EXTREME_RUNE_BUSTER_EventProcess` ( new ) | instruction-identical ( address-stripped diff, 258 lines ): end -> JUMP_DOWN ( y -= 45 ) / WAIT, then the up-down cancel, skill, back-walk, walk, dash, Z, X cancels on the ground; the second calls the first |
| `32cf4ff0` `32d09310` | `ESSI_SA_ERS_EXTREME_RUNE_BUSTER_Init` `_StateStart` ( new ) | Init is a bare `ret`; Start empties the observer list ( `+0x97c4` ) and the float list ( `+0x97d4` ) and pushes `GetRandomFloat( 0..9 )` |
| = March | Sword Fire Init / EndFuture / EP, its landing EP, Phoenix Talon Init / Start / EP, Luna Blade Init / EP, Rune of Fire Init, Splash Explosion Start | `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains the June cases ahead of the Mega Slash default: Phoenix Talon ( 21015; 21049 uses the `_UPGRADE` table ), Sword Fire ( 21016 / 21050; landing state 192 registered ), Luna Blade ( 21017 / 21051 ), Splash Explosion ( 21018 / 21052: Start + `GenericActiveSkillEventProcess` ), Triple Saber ( 21039 ), Overs Geyser ( 21040 ) and Enchant Break ( 21223 ) ( all `GenericSpecialActiveSkillEventProcess` ), Extreme Rune Buster ( 21059 ), Rune of Fire ( 21210 / 21231 ) and Counter ( 21211 / 21232: ready state + states 259 / 261 ); the hyper state takes the normal table name. March's Luna Blade / Splash Explosion / Sword Fire frame code is not in June's case: June wins, the script plays it. `X2SkillTree.h`: fourteen `RNSI_*ERS*` names ( values checked against `Enum.src.lua` ). Header: `ESSI_ERS_COUNTER_GUARD_END` ( June state 261 ) joins the state enum; members `m_vecExtremeRuneBusterTargetUnit` / `m_vecExtremeRuneBusterRandom` ( June `+0x97c4` / `+0x97d4`; the readers are CH4a's ); `m_fMegaSlashChargeTime` is renamed `m_fSkillKeyHoldTime` ( June `+0x96f4`, one member for the Mega Slash charge and the guard ). `Always.h`: `#error` without `JUNE_CHAR_EFFECTSET_MODIFIER`. Checklist rows marked `(Elsword: CH3b)` ( 51, 116, 117, 178, 256, 308, 422, 439, 637, 716 ).

**Judgement for the review.** ( 1 ) The state tables list a StartFuture on state 259 too; the raw registration @32d234a5-32d23548 gives it to the ready state only ( 259: EventProcess `32cf4d70`, 261: `32d05b80` ). ( 2 ) The listing leaves Extreme Rune Buster's hyper row empty: `@32d23aa7` `329cc2c0` copies every delegate ( Init, Start, EventProcess ). ( 3 ) The two June-only vectors are only ever cleared / filled in C++; whatever reads the ten random numbers is a script binder or an uncarved function ( CH4a lists the clear at `32d270a0` ). ( 4 ) 7u: Sword Enchant is not a June skill ( 21407 is a passive without a state ), so March's `ESSI_A_ERS_SWORD_ENCHANT_*` stay unreached; no port. ( 5 ) The Phoenix Talon functions sit inside March's `SKILL_CASH_10_TEST` block ( defined, `Always.h:3103` ).

**Left to later phases ( scope rule ).** None new: Enchant Break's numerical-only state table ( 1855 magical at level 1 ) and the damage effects run through the engine; `NUM use` comes from the play-test.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the five code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 2 OTHER (X2GUElsword_SwordMan.h: JUNE_CHAR_ELSWORD_CORE; X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `7 deleted line(s)` ( the seven lines of the member rename ) plus the `//#define`, `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the four source files: `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` / `.h` / `X2EffectSet.cpp`: `0 A/B row(s) remain`. `skillicons.py check 21015-21018 21039-21040 21049-21052 21059 21210-21211 21223 21231-21232`: `skills 16, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probes of `X2GUElsword_SwordMan.cpp` and `X2EffectSet.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 25,991,680 B, dated 2026-11-05 10:54:01, md5 29a03fb1b42a7ae3e0f45c38eb4d300e, equal to X2/US_SERVICE/x2.exe (2026-11-05 10:53:55); built at main f96cb171 + 5 uncommitted code file(s).
- EXESTR OK: 2 of 2 found in X2_offline.exe, 25991680 B, md5 29a03fb1 ( wide: `[JUNE_CHAR] ELSWORD SKILL counter guard end`, `... extreme rune buster: start` ).

**Play-test hand-off.** Save: unit 26 `jtRuneSlayer` ( Rune Slayer, class 101, lv 99, SP 842; backup `db_backup\els_db_20261105_104743_pre_sql.sql` ). Slots: A Rune of Fire ( 21210 ), S Counter ( 21211 ), D Enchant Break ( 21223 ), C Mega Slash ( 21000 ); Q Phoenix Talon ( 21015 ), W Sword Fire ( 21016 ), E Luna Blade ( 21017 ), R Splash Explosion ( 21018 ); slots 4 and 9 empty. Say "swap banks 2" for the enhanced ids ( 21231 / 21232 on A S; 21049 21050 21051 21052 on Q W E R ) and "swap banks 3" for Q Triple Saber ( 21039 ), W Overs Geyser ( 21040 ), E Extreme Rune Buster ( 21059 ).
Dev console ( END key, auth 3 ): `/invin 0`, then a dungeon with enemies in front ( `/stage 1` ). Cast each skill twice: the slot icon shows the cool time and the MP bar drops. Checks, enemies near:
1. **A Rune of Fire** summons the fire rune, hits; on the ground a double-tap dash, a walk or a skill key cancels it late; ends in wait / fall. **S Counter**: hold S: the guard motion, release (or 5 s) ends it ( log `counter guard end: held N s` ); an end in the air falls; **D Enchant Break**: June's motion, magic strike; the target's elemental defence drops.
2. **Q Phoenix Talon** ( hyper version too: awaken first ), **W Sword Fire**: the sword rises, lands ( state 192 ) and the buff applies on landing; **E Luna Blade**: June's blade; any key after 1.98 s cancels to wait; **R Splash Explosion**: the explosion effect at the start.
3. **Bank 3**: Triple Saber and Overs Geyser play June's motion and end in wait; **Extreme Rune Buster**: logs `extreme rune buster: start, 10 random numbers`; the end cancels work on the ground only.
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

**Play-test (user, 2026-11-05): ran, no faults.** Run 10:56 (exe 29a03fb1, bank 1 / 2): Sword Fire x3 ( `SwordFire_Ready`, `_Fire`, `_Buff` effect sets; the buff glow is faint because June's data draws it so, user confirmed ), Phoenix Talon upgrade, Luna Blade, Splash Explosion, Rune of Fire, Counter guard end x3 ( taps: held 0.04 / 0.01 / 0.07 s ). Run 11:03 ( bank 3 ): Triple Saber x3 ( 54 `HIT` lines over the three skills below ), Overs Geyser x5, Extreme Rune Buster x10 ( `extreme rune buster: start, 10 random numbers` x10, 18 `EffectSet_SA_ERS_Extreme_Rune_Buster`, hits ). 0 `EXCEPTION`, `ErrorLog.txt` clean, 2 `UNHANDLED` ( `E_DISABLE_HB_CHECK_REQ` ), no `tag 19 not ported` line. Not seen in any log: a held Counter ( the 5 s limit ), Enchant Break, the Rune of Fire ground cancels; no `NUM use` lines ( the numerical debug is retired ). `_SKILL_DEBUG` stays on for the review.

### CH3b - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `2ccb5198`; every address read with `charport_adis.py elsword --phase CH3b` (22 listings) plus Ghidra: the registration `32d223e0` raw with every delegate thunk resolved (`disasm_range.py`), `321db1b0` raw and its jump table, `321f2a30`, state-name strings.
- **Logs.** The game dir holds the 11:03 run (exe 29a03fb1): the bank-3 equips, `extreme rune buster: start, 10 random numbers` x10, 0 `EXCEPTION`, `ErrorLog.txt` 0 lines, 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`). The 10:56 lines are quoted in the play-test note above. Held Counter, Enchant Break and the Rune of Fire ground cancels stay unseen; their code is read below.
- **Registration `32d223e0`.** The delegate slots are March's `UserUnitStateData` order (Init `+0x14/18`, StartFuture `+0x1c/20`, Start `+0x24/28`, FrameMove `+0x34/38`, EventProcess `+0x44/48`, EndFuture `+0x4c/50`, End `+0x54/58`). **Counter** @32d23493: ready state StartFuture `32d26280` -> `32cf4e50`, EP `32d26d70` -> `32d05500`, hyper copies both; `31fd89b0( 0x103, "ESSI_ERS_COUNTER_GUARD" )` EP only (`32d26290` -> `32cf4d70`), `( 0x105, "..._END" )` EP only (`32d26d80` -> `32d05b80`). **Sword Fire** @32d22bf6: Init `32cf3ea0`, EP `32cfad40`, EndFuture `32cf3ec0`, hyper copies EP + EndFuture; landing `0xc0` EP `32cfad80` + End `32cf3ee0`. **Extreme Rune Buster** @32d23a53: Init `32cf4ff0`, Start `32d09310`, EP `32d068d0`, then `329cc2c0` (all to hyper). **Phoenix Talon** @32d22b48 / `_UPGRADE` @32d22b89: Init / Start / FrameMove `32d09290` / EP `32cfc3f0`, hyper Start + FrameMove + EP, same table name. **Rune of Fire** @32d23459: Init `32cf4770`, FrameMove `32d18230`, EP `32cfff60`, hyper FrameMove + EP. = port, case for case.
- **Counter Guard.** `32cf4e50`: 0 -> `+0x96f4`, `Set( +0x88bc - 1 )`, vslot 0x7c0. `32cf4d70`: `+0x96f4 += +0x2b0`, `FollowSlotSwap`, released or `5.0 < hold` (`fcomp`, `test ah,5`, `jp` skips when 5.0 >= hold) -> 0x105. `32d05500`: animation end -> 0x103. `32d05b80`: end -> 0x11 and y -= 45.0 (= `LINE_RADIUS * 1.5`) / 0xd; up-down `+0x8ca4` (`m_fUpDownCancelAfter`) -> 0xd; skill `+0x8c90` foot-gated + awakening branch (= `SKILL_CANCEL_AFTER` under `JUNE_BUFF_AWAKE_LOCK`); back-walk, walk, dash, Z (vslot 0x4b0), X (0x4b4), all foot-gated. = port.
- **Extreme Rune Buster.** `32d09310`: `+0x97c4` copy/destroy-range clear, `+0x97d4` memmove clear, `GetRandomFloat( i )` (vslot 0x1cc) pushed for i 0..9, vslot 0x7c4. `32d068d0` vs `32d05b80`, address-stripped: identical. = port.
- **Rune of Fire / tag 19.** `32d18230`: `321db050( handle, 0x13, +0x96d4, 0 )` inside the `GetEffectSetInstance` test, after `m_fLifeTime *= +0x96d4` and `+0x7c = 1`; `+0x96d4` / `+0x96d8` = March's range / size percent (header order). **Case 19** `@321dbb95` (jump table `321dbc60` slot 18, xref `321dbca8`): `+0xb5c = 1` first; with `GetMainEffect`, `+0xb60 = v.x`, mesh `+0x44c` = `321f2a30` (main max life time) x v.x. = port. **`32cfff60`**: end -> 0x11 / 0xd (no y offset), dash `+0x8c9c`, walk `+0x8c98`, skill `+0x8c90`, all foot-gated. = port.
- **Phoenix Talon FM `32d09290`** (0.002 cut-in, 1.75 one-shot, templet `0x5217` swap 1, factor 0) and **Sword Fire Landing End `32cf3ee0`** (templet `0x5218` swap 1, factor 0, vslot 0x7dc). = port.
- **"= March":** Luna Blade EP `32cfec80` (the 15 keys of `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS`, 1.98) and Splash Explosion Start `32d135c0` (size x3, life time x range, `+0x7c`) read against March: identical. The rest stand on the listing rows.
- **Members.** `m_fSkillKeyHoldTime` is June's one `+0x96f4` (Mega Slash memo `32cf3950` / `32d0fe90` / `32cf39b0` and the guard). `m_vecExtremeRuneBusterTargetUnit` is `KObserverPtr<CX2GameUnit>` (8-byte elements as June's destroy-range), `m_vecExtremeRuneBusterRandom` floats.
- **`#else`.** The cases sit inside `#ifdef _SKILL` ahead of the `_LIVE` default; the three bodies inside March's functions keep March in `#else`; `ESSI_ERS_COUNTER_GUARD_END` is an enum member under `_CORE` (inert flag-off).
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --range 2ccb5198^..2ccb5198 --files` the five code files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 2 OTHER (X2GUElsword_SwordMan.h: JUNE_CHAR_ELSWORD_CORE; X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`, `7 deleted line(s)` (in the flag's files: the `//#define` and six `.cpp` lines of the member rename; the seventh rename line is in the `.h`, an OTHER file), `RESULT: PASS`. `--numstat`: `Always.h` +5 -1, `X2EffectSet.cpp` +14, `.cpp` +310 -6, `.h` +11 -1, `X2SkillTree.h` +19. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the `.cpp` and `X2EffectSet.cpp`: `0 A/B row(s) remain`. One-file probes of both (`/c /Zs /EHsc`): exit 0, 0 errors. Retirement: `flagcheck ... --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, `1 deleted line(s)`, `RESULT: PASS`. `_SKILL_DEBUG` retired (`//#define`); build started, not awaited.

### CH3c - Done (2026-11-05)

Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `_SKILL_DEBUG`, defined again in `Always.h`; the review retires it): Infinity Sword's ten skills ( 16 ids ) run June's registrations. All 26 class-map rows are read from `charport_adis.py elsword --phase CH3c` listings (+ the registration `32d223e0` raw, `321db1b0` and its jump table, `322b5320`): **17 written** (14 inside March's functions under the flag with March in `#else`, 3 new), **9 are June = March** (no code), plus 15 enum names, modifier tag 9 and one shared `CX2GUUser` helper. `funcmap.tsv` rows `Char Elsword CH3c` (29); `MODS.md` "Stage 7 Elsword, CH3c" has the damage table.

| June | `CX2GUElsword_SwordMan::` | June differs from March |
|---|---|---|
| `32cfd910` `32d13940` | `ESSI_SI_SA_ETK_HARSH_CHASER_StateStart` / `_FrameMove` | the range reads the swapped 21215 templet at `max( 1, level )`, no x1.1 memo bonus; the blade effect has no memo variant |
| `32cfd990` | `..._HARSH_CHASER_PULL_EventProcess` | the walk cancel needs the foot on a line ( `ELSWORD_GROUND_WALK_CANCEL` ); the pull state registers no FrameMove |
| `32d089b0` `32d1ecc0` `32cfdb30` | `CompactCounterAttackStart` / `FrameMove` / `EventProcess` | the power rate ( and the NEXT table's ) reads the swapped 21216 skill; no memo defence time; the sheath mesh is alpha-tagged ( checklist 7c ); ground-only walk cancel |
| `32d13e20` `32cfddc0` | `ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMove` / `_EventProcess` | effective time from the swapped 21024 templet; an upgraded skill adds modifier tag 9 ( -2 ) to the effect set; ground-only walk cancel |
| `32cf4450` `32d170e0` `32cfe000` | `ESSI_SI_SA_ETK_PHANTOM_SWORD_Init` / `_FrameMove` / `_EventProcess` | background load; sheath alpha-tag, buff factor 0 then 1 ( upgraded ) of the swapped 21022 templet; ground-only walk cancel |
| `32d1f470` `32cfea80` | `ESSI_SI_SA_EIS_RAGE_CUTTER_FrameMove` / `_EventProcess` | swapped 21023 templet, first buff factor pushed levelled with the skill memo's rate; ground-only walk and dash cancels |
| `32d177a0` | `ESSI_SI_SA_EIS_SWORD_BLASTING_FrameMove` | the swords are `..._UpGrade` when the state's skill is upgraded ( 21057 ) |
| `32cf44b0` `32cf5000` `32d06c50` | `ESSI_SI_SA_EIS_SWORD_BLASTING_StateEnd`, `ESSI_SA_EIS_INFINITY_STRIKE_Init` / `_EventProcess` ( new ) | the End restores the sword's attack box; Init is a bare `ret`; the EventProcess is `32d05b80`'s ( address-stripped diff: empty ) |
| `321db1b0` | `CX2EffectSet::ApplyDamageEffectModifiers` case 9 | the effect's force down ( DamageData `+0x3f0` ) plus v ( or times v ), NEXT tables take the result ( checklist 7a missed this tag ) |
| `322b5320` | `CX2GUUser::PushBuffFactorWithMemoRate` ( new, shared ) | levelled clone pushed ( clear ), then the skill's time / value memo rescales it; `RescaleBuffFactorByMemoRate` stays Arme's, `Always.h` gains the `#error` for it |
| = March | `32cf4210` `32cf4230` `32cf4240` ( Compact Counter ), `32cf42a0` `32cf42f0` `32cfdd60` ( Maelstrom Rage ), `32cfea40` ( Sword Blasting EP ), `32d13be0` `32d13cd0` ( Harsh Chaser ) | `funcmap.tsv` says which |

**Registration.** `SetEquippedSkillFuncBySkillSlotIndex` gains eleven June cases ahead of the Mega Slash default: Harsh Chaser ( 21215 / 21235; pull state 219 ), Compact Counter ( 21216 / 21236 ), Another Sword ( 21224 ), Phantom Sword ( 21022 / 21054 ), Rage Cutter ( 21023 / 21055 ), Maelstrom Rage ( 21024 / 21056, one table ), Sword Blasting ( 21025 ) and its upgrade ( 21057, own table ), Infinite Chaser ( 21041 ) and Blade Circle ( 21042 ) ( `GenericSpecialActiveSkillEventProcess` ), Infinite Strike ( 21060; hyper through `329cc2c0` ). The hyper rows were read raw: Phantom Sword / Rage Cutter copy FrameMove + EventProcess, Sword Blasting adds End, Maelstrom Rage copies StartFuture / FrameMove / EventProcess / EndFuture ( no Init ), Harsh Chaser and Compact Counter copy every delegate. `X2SkillTree.h`: fifteen names ( values checked against `Enum.src.lua` ).

**Judgement for the review.** ( 1 ) June's `GetSkillTemplet( id, 1 )` is read as the one-argument form ( as CH1 / CH3a ). ( 2 ) Tag 9 sets no inherit flag ( June has none ); `AddDamageEffectModifier` ignores a dead handle, so June's handle test is not repeated. ( 3 ) The Lua table `ESSI_SI_SA_ETK_MAELSTORM_RAGE_UPGRADE` is unused by C++: 21056 registers the base table ( June ). ( 4 ) Phantom Sword's `SetBuffFactorToGameUnit` has no null test in June ( kept ). ( 5 ) Harsh Chaser's FrameMove reads the Now set's skill-cancel member ( `+0x8d2c` ) and its EventProcess the Future set's ( `+0x8c90` ): one member in March. ( 6 ) Infinite Strike's hyper row is empty in the listing: `329cc2c0` copies Init and EventProcess ( the CH3b reading ). ( 7 ) The sheath-mesh alpha tags use `TagEffectAlphaMesh` ( `32296ee0` ).

**Left to later phases ( scope rule ).** None new: the five numerical-only state tables ( Compact Counter x2, Sword Blasting x2, Another Sword x2 ) and the damage effects run through the engine; `NUM use` comes from the play-test.

**Gates.**
- `flagcheck JUNE_CHAR_ELSWORD_SKILL --with JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files` the seven code files: `files: 5 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `1 deleted line(s)` ( the `//#define` ), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check` the five source files: `allow-listed: 10`, `RESULT: PASS`. `enumscan.py --check` the four `.cpp` / `.h` files: `0 A/B row(s) remain`. `skillicons.py check 21022-21025 21041-21042 21054-21057 21060 21215-21216 21224 21235-21236`: `skills 16, misses 0, sheets 0`, `RESULT: PASS`. `luabind`: no Lua staged. One-file probes of `X2GUElsword_SwordMan.cpp`, `X2EffectSet.cpp` and `X2GUUser.cpp` ( `/c /Zs /EHsc` ): exit 0, 0 errors.
- DEPLOY OK: X2_offline.exe is 25,998,336 B, dated 2026-11-05 11:52:50, md5 a180e7f46a613acdc76835fdb37bf283, equal to X2/US_SERVICE/x2.exe (2026-11-05 11:52:43); built at main 5294703d + 7 uncommitted code file(s).
- EXESTR OK: 5 of 5 found in X2_offline.exe, 25998336 B, md5 a180e7f4 ( wide: `[JUNE_CHAR] ELSWORD SKILL harsh chaser`, `... compact counter`, `... maelstrom rage`, `... sword blasting`, `ELSWORD_EIS_SWORDBLASTING_PHANTOMSWORD_UpGrade` ).

**Play-test hand-off.** Save: unit 71 `jtInfinitySword` ( Infinity Sword, class 112, lv 99; backups `db_backup\els_db_20261105_114333_pre_createchar.sql` and the one the slot edit took ). Setup 1 ( loaded ): A Harsh Chaser ( 21215 ), S Compact Counter ( 21216 ), D Infinite: Flurry ( 21224 ), C Mega Slash ( 21000 ); Q Phantom Sword ( 21022 ), W Rage Cutter ( 21023 ), E Maelstrom Rage ( 21024 ), R Sword Blasting ( 21025 ); slots 4 and 9 empty. Say "swap banks 2" for the enhanced ids ( 21235 / 21236 on A S; 21054 21055 21056 21057 on Q W E R ) and "swap banks 3" for Q Infinite Chaser ( 21041 ), W Blade Circle ( 21042 ), E Infinite Strike ( 21060 ).
Dev console ( END key, auth 3 ): `/invin 0`, then a dungeon with enemies in front ( `/stage 1` ). Cast each skill twice: the slot icon shows the cool time and the MP bar drops ( ReservedSkillStart ). Checks, enemies near:
1. **A Harsh Chaser** throws a blade ( log `harsh chaser: skill 21215 level 1, range 0.70` ); when it dies in the enemy or at its range, the pull state plays; walking cancels the pull on the ground only. **S Compact Counter** plays its attack state ( log `compact counter: start, skill level 1, physic damage ...`; `NUM use` > 0 for 21216 ). **D Infinite: Flurry** plays June's motion and effect ( `NUM use` for 21224 ).
2. **Q Phantom Sword**: the sheath mesh and the second blade ( log `phantom sword: buff factor 0` ). **W Rage Cutter**: the hit effect at 0.9 s ( log `rage cutter: skill 21023 level 1, buff factor pushed` ). **E Maelstrom Rage**: the storm and the camera shake at 1.0 s ( log `maelstrom rage: effective time 5.00, upgraded 0` ). **R Sword Blasting**: three swords fly out ( log `sword blasting: effect ELSWORD_EIS_SWORDBLASTING_PHANTOMSWORD`; `NUM use` for 21025 ).
3. **Bank 2** ( enhanced ): `phantom sword: buff factor 0 and 1`, `maelstrom rage: ..., upgraded 1`, `sword blasting: effect ..._UpGrade`.
4. **Bank 3**: Infinite Chaser, Blade Circle, **Infinite Strike** ( no log of its own; it ends in the wait / fall and cancels into walk / dash / Z / X on the ground ).
Log tag `[JUNE_CHAR] ELSWORD SKILL` ( `python tools/logs.py --grep "ELSWORD SKILL"` ) and `[JUNE_SKILL]` `NUM use`. Then say "check logs".

**Play-test (user, 2026-11-05): ran.** Runs 12:02 and 12:05 (exe a180e7f4), enhanced banks. Seen: `harsh chaser: skill 21235 level 1, range 0.70`, `compact counter: start, skill level 1, physic damage 1.84` x5, `phantom sword: buff factor 0 and 1`, `rage cutter: skill 21055 level 1, buff factor pushed`, `maelstrom rage: effective time 5.00, upgraded 0` ( the base 21024 was slotted, so tag 9 did not run ), `sword blasting: effect ..._UpGrade` x4; bank 3: Infinite Chaser ( `DamageEffect_SA_EIS_Inpinity_Chacer` ), Blade Circle ( `_1` / `_2` ), Infinite Strike ( Body_Attack, SwordSpearFX ) and Infinite: Flurry ( 21224, `DamageEffect_A_Compact_Counter_Continue` ) created their effects. 2 `UNHANDLED` ( `E_DISABLE_HB_CHECK_REQ` ), 0 `EXCEPTION`, `ErrorLog.txt` has no new CH3c lines. `JUNE_FX unsupported APPLIED_SKILL_ID ... applied-skill-ID system absent` x83 for these ids ( the engine's known log; no `NUM use` lines, the debug is retired ). Not seen: Maelstrom Rage upgraded ( tag 9 ) and the Infinite Strike ground cancels; 21056 is now learned on unit 71, unslotted. `_SKILL_DEBUG` stays on for the review.

### CH3c - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `82973434`; every address read with `charport_adis.py elsword --phase CH3c` (26 listings) plus Ghidra (the registration `32d223e0` decompiled, every delegate thunk resolved with `disasm_range.py`), `321db1b0` raw and its jump table.
- **Logs.** The 12:05 run (exe a180e7f4) is still in the game dir: the bank-2 / bank-3 equips (21235 21236 21224 / 21041 21042 21060 21057), `sword blasting: effect ..._UpGrade` x2, `compact counter: start, skill level 1, physic damage 1.84`, `harsh chaser: skill 21235 level 1, range 0.70`; 0 `EXCEPTION`, `ErrorLog.txt` 0 matching lines, 2 `UNHANDLED` (`E_DISABLE_HB_CHECK_REQ`). The 12:02 lines are quoted above. Tag 9 (upgraded Maelstrom Rage) and the Infinite Strike ground cancels stay unseen; their code is read below.
- **Registration `32d223e0`.** Slots in March's `UserUnitStateData` order (Init `[6]`, StartFuture `[8]`, Start `[10]`, FrameMoveFuture `[0xc]`, FrameMove `[0xe]`, EventProcess `[0x12]`, EndFuture `[0x14]`, End `[0x16]`). **Phantom Sword** (`0x521e` / `0x523e`): Init `32cf4450`, FM `32d170e0`, EP `32cfe000`, tail `32d22557` copies FM + EP. **Rage Cutter** (`0x521f` / `0x523f`): FM `32d1f470`, EP `32cfea80`, same tail. **Maelstrom Rage** (`0x5220` / `0x5240`, one table): Init `32cf42a0`, SF `32cfdd60`, FM `32d13e20`, EP `32cfddc0`, EF `32cf42f0`; hyper copies SF / FM / EP / EF, no Init. **Sword Blasting** (`0x5221`): FM `32d177a0`, EP `32cfea40`, End `32cf44b0`, tail `32d22d85` copies all three; **`0x5241`** alone takes `ESSI_SI_SA_EIS_SWORD_BLASTING_UPGRADE`, same delegates. **Harsh Chaser** (`0x52df` / `0x52f3`): Start `32cfd910`, FM `32d13940`, EP `32d13be0`, End `32d13cd0`, all copied; `31fd89b0( 0xdb, "..._PULL" )` EP only (`32d26dd0` -> `32cfd990`). **Compact Counter** (`0x52e0` / `0x52f4`): all six delegates on both. **Infinite Strike** (`0x5244`): Init `32cf5000`, EP `32d06c50`, then `329cc2c0`. **Infinite Chaser / Blade Circle / Another Sword**: EP `329cbfc0`, hyper copies EP. = port, case for case.
- **Maelstrom Rage FM `32d13e20`.** 0.1333: templet `0x5220` swap 1, `GetUnit()` NULL returns before `CommonFrameMove` (port's `return`), level `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, ability 1 (`SA_EFFECTIVE_TIME`) to `+0x968c`; `PlayEffectSet` with the power rate and the effective time; handle tested against -1, then vslot `0x524` + `GetEquippedActiveSkillTemplet( id, 1 )` + `+0x178` (= `IsNowStateSkillUpgraded`), `321db050( handle, 9, -2.0, 1 )`; 1.0: `UpDownCrashCamera( 8.0, time + 3.0 )`. = port. **Case 9** `@321db77e` (jump table `321dbc60` slot 8 = `0x321db77e`): `m_bAdd` (byte +0) ? v + `+0x5f0` : v x `+0x5f0` (v = `m_vValue.x`, +8), stored, then every `+0xd8` entry's `+0x3f0` takes it. = port (inside CH3b's `_SKILL` block in `X2EffectSet.cpp`).
- **Rage Cutter FM `32d1f470`.** 0.9: templet `0x521f` swap 1, buff vector non-empty, the hit effect, NULL `GetUnit()` returns, level as above, `322b5320( factor[0], level, 1, 31fdbfc0( id ) )` on the effect's DamageData (+0x200). `CX2GUUser::PushBuffFactorWithMemoRate` = Chung's copy line for line (`PushBuffFactor` defaults to clear + socket factors; `MI_CHECK_AND_KILL_1` / `_2` are 15 / 16). = port.
- **Compact Counter** `32d089b0` (`GetSkillLevel( 0x52e0, 1, 0, 1 )`, `GetSkillTemplet( 0x52e0, 1 )` read as the one-argument form, physic x power rate), `32d1ecc0` (0.09 particle, 0.01 `ElswordSheath02` + radius 0 + `32296ee0 @32d1efb2`, dash-cancel time clears the handle, `DAMAGE_DATA_NEXT` x the 21216 power rate, no memo), `32cfdb30` (end: 0x11 + y -= 45.0, else 0xd; walk cancel foot-gated, an else of the end test). = port. **Phantom Sword** `32d170e0`: tag `@32d17216`, factor 0 then (vslot 0x524 templet `+0x178`) factor 1, both on the `0x521e` templet. **Sword Blasting FM** `32d177a0`: the `_UpGrade` name under the same gate; the rest March's. **Harsh Chaser** `32cfd910` (`0x52df` swap 1, ability `0x1f` = `SA_PROJECTILE_RANGE_REL`, to `+0x9688`), `32d13940` (only `ELSWORD_HASHCHASER_BLADE`, no memo string), `32cfd990` (foot-gated walk, the awakening skill-cancel pair). = port.
- **"= March":** Sword Blasting EP `32cfea40` and Harsh Chaser End `32d13cd0` read against March: identical. `32d06c50` vs `32d05b80`, address-stripped with relative jumps as offsets: 258 instructions each, identical. The rest stand on the listing rows.
- **`#else`.** Every June body sits in `#ifdef _SKILL` with March in `#else`; the eleven cases sit ahead of the `_LIVE` default; the three new functions and case 9 are inside existing `_SKILL` blocks.
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG --range 82973434^..82973434 --files` the seven code files: `files: 5 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2SkillTree.h: JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`, `1 deleted line(s)` (the CH3b `//#define`, replaced by the `#define`), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`. `--numstat`: `Always.h` +5 -1, `X2EffectSet.cpp` +10, `.cpp` +486, `.h` +3, `X2GUUser.cpp` +17, `X2GUUser.h` +4, `X2SkillTree.h` +20. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the three `.cpp`: `0 A/B row(s) remain in 3 file(s) (C 0, D 13, X 3 ignored)`. One-file probes of `X2GUElsword_SwordMan.cpp`, `X2EffectSet.cpp`, `X2GUUser.cpp` (`/c /Zs /EHsc`): exit 0, 0 errors each. Retirement: `flagcheck ... --worktree --files KTDXLIB/Always.h`: `1 DEFINE`, `1 deleted line(s)`, `RESULT: PASS`. `_SKILL_DEBUG` retired (`//#define`); build started, not awaited.

### CH3d - Done (2026-11-05)

Flags `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (the two event processes; `_SKILL_DEBUG` defined again for the equip line) and new `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_TRANSCEND`
(needs `_ELSWORD_LIVE`, `_EVE_TRANSCEND`, `JUNE_TRANSCEND_CHANGE`, `JUNE_TRANSCEND_IMAGE`; `flagprobe` ON for all). MODS entry "Stage 7 Elsword, CH3d". funcmap rows `Char Elsword CH3d`.

**The 5 functions** (`ch0_class_map.tsv` phase CH3d, listings via `funcann.py`); the registrations ( 21600-21605 ) are in the head since CH1n.
- Init `32cf4930` / `32cf49d0` / `32cf4ab0`: 12 / 17 / 14 preloads, March's names in March's order. No change.
- EventProcess `32d00be0` (Shining Rune Buster) and `32d00de0` (Blade Rain), identical to each other: anim end -> `USI_JUMP_DOWN` (0x11) off the ground, else `USI_WAIT` (0xd);
  then the dash ( `+0x8c9c`, `pureDoubleRight / Left` `+0x86e0 / +0x86dc`, vslot `0x668` ) and walk ( `+0x8c98`, `+0x86de / +0x86da` ) cancels **both test `+0x7e1c` IsOnSomethingFuture**,
  then June's `SKILL_CANCEL_AFTER`, `CommonEventProcess`. Ported inside March's functions with the CH1d `ELSWORD_GROUND_DASH_CANCEL` / `_WALK_CANCEL` macros, March's two macros in `#else`.
  Gigantic Slash keeps `GenericSpecialActiveSkillEventProcess` ( `31fcc740` ).
- 7ao hyper effect start `32d17ea0` (uncarved, `charport_adis.py`): base call, skip for `unit +0x3c != 0` or the awakening set test `31fd66b0`, four handles. Equal to CH1h's port, no change.
  The four base helpers: only `31fceab0` has Elsword callers ( `32d1c560` / `32d1e0e0` / `32cf4b70` ), all CH4a. 7ag ( A-5 / B-5 pressed index ) is CH1p's.

**Transcendence rows (7ao).**
- Allow-list: `TranscendCheck` takes 100 / 101 / 112 ( one brace-less guard after Chung's, same shape ).
- `LevelUpEvent.lua` (data036, BOM source) gets `TYPE3` = June's `TYPE0` row ( `LevelUpEvent.src.lua:453-471`: LEVEL 70, the three classes, `DLG_Transcendental_Change_Guide_Elsowrd.lua`,
  `ElswordVoice_ClassUpReady01.ogg` ( data093 ) ). Appended, as Aisha / Eve / Chung were ( June's order is Elsword first ). `luasrc.py refresh`, `ledger --check` PASS. `PACK OK data036`
  ( only `LevelUpEvent.lua` changed ); the copy waits for the user's yes.
- Pictures: `GetCharacterImageName` prefixes `L_KNIGHT` / `R_SLAYER` / `I_SWORD`. June's row table ( string refs at `@33370955` `TR_L_KNIGHT_STATE`, `@33371f37` `TR_I_SWORD_FACE`,
  `@33377182` `TR_I_SWORD_LARGE`, each paired with its file ): large + gray in **Texture98** ( Texture99 has no Elsword piece, so the existing `else` serves ), MEDIUM / ICON / PARTY in Texture100,
  gauge in Texture97, but **Infinity Sword's gauge piece is `TR_I_SWORD_FACE`** ( its TET has no `_STATE` ): a class test on the `CIT_MyGage` suffix. Unit slot `list_TR_*` in `GUI_character_01`
  ( + `_gray` ). Every piece name was found in the staged TETs ( data215, data057, datai021 ).
- Already June's, verified: the guide dialog ( data034, adopted whole by june-transcend T2: `adopt-june --dry-run` 1 identical; `stage-june` says "replaced" because it compares raw June bytes ) and
  `POPUP_TR_L_KNIGHT / R_SLAYER / I_SWORD` ( data215 ); `HQ_Cutin_ELK / ERS / EIS_Transcendence.gfx` ( data195, CH0b ) and the CH5 `m_HyperCutIn` rows; `GetHyperModeTableName` ( CH1m );
  the 18 `SPRT_HYPER_CLASS` ids ( 21037-21042, 21058-21060, 21222-21224, 21415-21420 ) and the 12 `TRANSCENDENTAL_SKILL_DATA` rows: `NewSkillTempletVer2.lua` and
  `SkillCommandDescriptionData.lua` are June's bytes ( `adopt-june --dry-run`: identical ).
- **Skill icon gate:** `skillicons.py check 21000-21999`: `skills 125, misses 1, sheets 1`, `RESULT: FAIL` on exactly 21402 Wind Slicer `NO_PIECE` in `DLG_Elsword_SKILL_ScreenShot_04`,
  June's own gap (JUNE_BUGS 520, CH0b; `stage-june --dry-run`: 2 identical). Nothing to stage; the passive is CH4a's.

**Hygiene.** `flagcheck JUNE_CHAR_ELSWORD_TRANSCEND --worktree --files <4 code files>`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted,
0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `flagcheck JUNE_CHAR_ELSWORD_SKILL --worktree`: `1 PASS, 3 OTHER, 0 FAIL`, `RESULT: PASS`. The one
`Always.h` deleted line is the `//#define ..._SKILL_DEBUG` toggle. `junediverge --check` ( 5 files ): `RESULT: PASS`. `enumscan`: no new literal ( the 4 rows left are the T6 `104 / 105 / 113` cases and the `< 100` bound ).
**Deploy.** `DEPLOY OK: X2_offline.exe is 25,998,336 B, dated 2026-11-05 12:54:59, md5 2da608fe88f4ccbb7e5245d246f61e8e, equal to X2/US_SERVICE/x2.exe (2026-11-05 12:54:52); built at main 24b8fe5a + 4 uncommitted code file(s).`
`EXESTR OK: 4 of 4 found in X2_offline.exe, 25998336 B, dated 2026-11-05 12:54:59, md5 2da608fe88f4ccbb7e5245d246f61e8e` ( `R_SLAYER`, `I_SWORD`, `_FACE`, the equip debug line ).

**Play-test set-up ( save, backed up as `pre_elsword_ch3d` ).** Unit 25 `jtLordKnight` 21600 Gigantic Slash, unit 26 `jtRuneSlayer` 21601 Shining Rune Buster, unit 71 `jtInfinitySword`
21602 Blade Rain, each transcended, the hyper active in A-5 ( slot 4 ) and Grand Cross 21058 / Extreme Rune Buster 21059 / Infinite Strike 21060 moved to B-5 ( slot 9 ); upgrade ids not learned.
**In game** ( after `/x2-kom-packed copy data036` ): the plan's hand-off; log lines `ELSWORD SKILL equip skill 2160x type 13 slot 4`, `[JUNE_TRANSCEND] IMAGE class 100 / 101 / 112`.
**Play-test PASS ( user, 2026-11-05; `COPY OK data036` md5 8348bc0c ).** Logs ( run after the deploy, md5 2da608fe ): `equip skill 21600 / 21601 / 21602 type 13 slot 4` and `21058 / 21059 / 21060 type 16 slot 4 B`; `[JUNE_TRANSCEND] IMAGE` for 100 / 101 / 112: type 9 `list_TR_L_KNIGHT / R_SLAYER / I_SWORD`, type 2 Texture98 `TR_*_LARGE`, type 7 Texture97 `TR_L_KNIGHT_STATE`, `TR_R_SLAYER_STATE`, `TR_I_SWORD_FACE`; no EXCEPTION, no ErrorLog line of this phase. Not this phase: the `E_DISABLE_HB_CHECK_REQ` UNHANDLED baseline and one unbound `AddGroupChainQuest` from a June quest script. The user saw the ground-only cancels, the hyper actives in A-5 and B-5, the cut-in and the TR pictures.
**Open for the review:** the ground test on both event processes, the `_FACE` row against June's table, and the "= March" Init claims.

### CH3d - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `be8b7806`; listings via `funcann.py`, the picture rows in Ghidra (string xrefs and the paired `PUSH`es).
- **Logs.** The 12:56-13:00 run (exe 2da608fe): the six equip lines (21600 / 21601 / 21602 type 13 slot 4, 21058 / 21059 / 21060 type 16 slot 4 B), the `IMAGE` lines for 100 / 101 / 112
  (types 9 / 2 / 7, `TR_I_SWORD_FACE` on type 7), the guide dialog for 100, the three `HQ_Cutin_*_Transcendence.gfx` plays; 0 `EXCEPTION`, `ErrorLog.txt` no phase line; 4 UNHANDLED
  `E_DISABLE_HB_CHECK_REQ` and one unbound `AddGroupChainQuest` (baseline, not this phase).
- **EventProcess `32d00be0`.** Anim end ( `+0x87d0` `+0x2ac` ) -> `StateChange( +0x7e1c ? 0xd : 0x11 )`; else dash ( `+0x8c9c`, the future-anim accessor `+0x8e90 / +0x8e84`, `+0x7e1c`, `+0x86e0 / +0x86dc`,
  vslot `0x668` ) before walk ( `+0x8c98`, `+0x7e1c`, `+0x86de / +0x86da`, `StateChange( 0xd )` ); then `+0x8c90` with `+0x7e1c` and vslot `0x7f4`, else the awakening branch
  ( `IsThereEffectiveBlackHoleDamageEffect`, `+0x8723`, the encrypted hyper count, vslot `0x560`, `32070d60`, `+0x8d4c = 1`, `StateChange( vslot 0x6b8 )` ); tail vslot `0x7d4`. = `ELSWORD_GROUND_DASH_CANCEL`,
  `ELSWORD_GROUND_WALK_CANCEL` (`X2GUElsword_SwordMan.cpp:12618-12626`) and the live `SKILL_CANCEL_AFTER` ( `X2GUUser.h:113`, buff 7c's June expansion ), `CommonEventProcess`. = port.
  `32d00de0` vs `32d00be0`, address-stripped: 156 instructions each, identical.
- **Pictures.** June has every piece for all three classes: `TR_L_KNIGHT / R_SLAYER _STATE`, `TR_I_SWORD_FACE` (no `TR_I_SWORD_STATE` string exists), `_LARGE`, `_LARGE_GRAY`, `_MEDIUM`, `_ICON`, `_PARTY`,
  `list_TR_*`, `list_TR_*_GRAY`. Paired files: gauge Texture97 ( `@33371f37`, `@33370955` ), large `Texture98_NEW` ( `@33377182`, `@33375ba0` ), gray `Texture98_GRAY` ( `@3337c3cd` ), medium / icon / party
  `Texture100_NEW` ( `@3338161d`, `@3338686d`, `@3339620d` ), unit slot `GUI_character_01` / `_gray` ( `@3339b45d`, `@333a163d` ). = the port's rows, `_FACE` for 112 only.
- **Allow-list.** `TranscendCheck`: Chung's guard, then Elsword's, then March / Eve's `return` (brace-less, each an `if` of the next), so any of the 12 classes passes. `LevelUpEvent.lua` `TYPE3`
  = June's `TYPE0` (`LevelUpEvent.src.lua:453-471`) line for line.
- **"= March".** The three Inits ( `32cf4930` / `32cf49d0` / `32cf4ab0` ): 12 / 17 / 14 preloads, same names, order and loader (Texture vs XSkinMesh) as March. `32d17ea0` = CH1h's port per the Done note's listing.
- **`#else`.** Both event processes keep March's two macros in `#else`; `CIT_MyGage` keeps March's `_STATE` row in `#else`; the allow-list and prefix rows are insert-only.
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_TRANSCEND --range be8b7806^..be8b7806 --files` the four code files: `files: 2 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS`.
  `flagcheck JUNE_CHAR_ELSWORD_SKILL --with ..._SKILL_DEBUG` same range: `files: 1 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS` (the OTHER rows are the
  `_TRANSCEND` files and `Always.h`'s other blocks; `Always.h -1` = the `//#define _SKILL_DEBUG` made live). `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check` the three
  `.cpp`: `4 A/B row(s)`, all outside the CH3d lines (X2Data.cpp 104 / 105 / 113 and the `< 100` bound, pre-existing). Retirement: `flagcheck JUNE_CHAR_ELSWORD_SKILL_DEBUG --worktree --files KTDXLIB/Always.h`:
  `1 DEFINE`, `1 deleted line(s)` (the `//#define`). `_SKILL_DEBUG` retired; build started, not awaited.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32d1c560` (vslot 0x690) and Elsword's passive ids (types 1 / 8: 3 + 25 rows), their objects and readers; 7am's 8 delegate callbacks in `32d1c560`; the three `CreateAndInsertSkillAndSocketBuffFactorToList` calls (`322a25f0`; Spell Chain 21406's bool = Magic Coating 21418 learned); flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `32d1e0e0` whole (June-only, 7b: 21419 `RNSI_P_EIS_OPEN_MIND` with `ATTACK_TRIGGER_TYPE` 0xf -> `+0x510` = 1.0; 21420 `RNSI_P_EIS_INFINITY_POTENTIAL` with 0x10 -> vslot 0x238 and the `322b5320` push); the other overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style); §7 l.281 power-rate socket range (`32d076f0`); the death-prevention snapshot override `32d058a0` unless CH0 gave it to CH1q |
| CH4c | 7j Rune Slayer magic reflect: the engine branch in `320462a0` @`32046a55`-`32046aa0` (`X2GUUser.cpp`; divergent row `june_march_divergent.tsv:220`) and the class's `32d0c820` (Magic Coating 21418 as a transcended hyper-class skill: MP gain + effect), every class, own flag if the engine branch is not Elsword's; `MemoConditionElsword` (RTTI `3373d9b0`) and the memo readers; the S3d memo consumer lines that waited for his skill states (`STATUS_ARCHIVE.md` l.715) |
| CH4a2 | (added by CH4a, 2026-11-05) the engine readers of Elsword's passives: the user state key `CREATE_EFFECT_SET` (parse `32057b80` @`320606b7`, play in `CommonFrameMove` `32063650` @`32063a6c`-`32063d06` at Wind Slicer's rate `+0x8e50`, doubled by Fight God 21416 with BTI 18), and `32610cb0` (21417 Rune Slayer Open Mind); kickoff `CH4a2.md` |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); a Rune Slayer reflects magic as June, and a transcended
one with Magic Coating takes 0 magic damage with the MP gain; 0 Elsword `no_state` rows left except passives with no state in June.

### CH4a - Done (2026-11-05)

**Landed.** Commit: see `git log`, "char-elsword Phase CH4a". Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (+ temporary `_PASSIVE_DEBUG`). Files: `X2GUElsword_SwordMan.*`,
`X2GUUser.*` (one member + ctor), `X2SkillTree.h` / `X2BuffFactor.h` / `X2DamageManager.h` (names under `JUNE_ENUM_NAMES`). Read from `funcdis` / Ghidra listings of every address below.
- **`InitPassiveSkillState` `32d1c560` whole** inside March's (March's in `#else`), June's order. All `GetSkillLevel( id, 1, 0, 1 )` except 21200 / 21407 / 21411 / 21404 ( `0` ):
  Wind Slicer 21402 SA 5 -> new user `m_fCreateEffectSetRate` (`+0x8e50`, reset 0; reader = CH4a2); Rolling 21200 0x71 / 0x70 (min 10, reset 0 / 0); Strong Bone 21403 0x10 x final
  defence passive; Spell Chain 21406 factor [0] clone with bool = Magic Coating 21418 learned; Auto Guard 21401 (class 10 / 100): rate 0 / both cool times -1, then
  `SA_COOLTIME_1`, rate = SA 5 + 0.005 (double), **x2 when the game is not PvP / PvE**; Rune Mastery 21408 (class 101): range / size reset 1, fireball 1, then 0x1f / 0x20 and
  fireball 0xf **at Spell Chain's level** (JUNE_BUGS 533); Magic Resistance 21409 0x53 -> `m_fMagicReflectRate` (no class gate); Brutal Slayer 21410 (factors non-empty) ->
  A `AddComboToBrutalSlayer` + cool time; Harsh Slayer 21412 (22 / 112) -> A `ApplyBuffToHarshSlayer` (CH1h) before its templet test, cool time; Cutting Sword 21413 (112,
  2+ factors, not special-active / type 13) -> factors [0] and [1], bool false; Lightning Step 21414 walk / dash x SA_MOVE_SPEED, **anim x SA_ANI_SPEED** (June-only), C
  `LightingStepFrameMove`; Sword Enchant 21407 / Short Time Strong Attack 21411 -> A callbacks + cool times (reset -1 / -1); Reduce Pain 21404 level; Lord Knight Open Mind
  21415 and Infinity Sword Open Mind 21419 (hyper-class + transcended) -> count 0, D / A callbacks; Extreme Rune Buster 21059 (hyper-class + transcended) -> level, list clear, A.
  June drops March's Powerful Fireball, Mind of Fighter, guard skill, Intensive Attack and Indurance blocks (all stay in `#else`; their members keep their ctor values).
- **Callbacks** (thunk targets checked: 32d272b0 / 32d27160 / 32d27170 / 32d27180 / 32d26a40 / 32d27470 / 32d27640 = push edx; call; 32d26fd0 = jmp 32d08ba0):
  `AddComboToBrutalSlayer` = June `32d0c4e0` (not riding, cool time over, combo > 0, own count `+0x9710` reaches 5 -> buff, Mind of Fighter level +1 cap 5,
  `RestartBrutalSlayerCoolTime` `32d27190`); `ApplyBuffToSwordEnchant` `32d0baf0` (not AT_SPECIAL, `IsSwordEnchantCoolTimeEnd` `32d05a00`, random < SA 5 -> factor
  `GetRandomInt() % 3`); `ApplyBuffToShortTimeStrongAttack` `32d0bc60` (same, factor 0, `32d05a50`); `ApplyBuffToLordKnightOpenMind` `32d0c180` (damage react: at
  BTI 824's stack limit or in the cool time without it -> count 0; else not DT_BUFF, count +1 to the truncated SA 5 -> buff, count 0, `m_fOpenMindRemainCoolTime` =
  SA_COOLTIME_1); `ApplyBuffToInfinitySwordOpenMind` `32d05af0` (DTT 15 Conwell hits); `ApplyBuffToExtremeRuneBusterTarget` `32d1a370` (applied 21059 + DTT 42, a
  defender not in the list -> pushed, random[count % 10] > 0.67 BFI 153 / > 0.33 152 / else 168 at `+0x97c0`'s level, creator = Elsword); `LightingStepFrameMove`
  `32d08ba0` (**0.5 s** dash, March 1.0).
- **Stand-ins (one line each in code):** no game -> Auto Guard not doubled (June reads the game unchecked); an empty random list -> no debuff (June divides by zero);
  the write-only byte `+0x9794` (set 0 by `32d0c180`, no reader in the binary) is not kept.
- **Names** (`JUNE_ENUM_NAMES`): 12 skill ids 21401-21403, 21406, 21407, 21410, 21411, 21413-21415, 21418, 21419; BTI 824; BFI 152 / 153 / 168; DTT 15 / 42.
  Members: user `m_fCreateEffectSetRate`; Elsword `m_iExtremeRuneBusterSkillLevel` (`+0x97c0`), `m_iBrutalSlayerHitCount` (`+0x9710`), `m_usOpenMindHitCount` (`+0x977c`).
  CH1j's cool-time pairings confirmed by the blocks (21410 `+0x96f8` / `+0x9704`, 21407 `+0x9740` / `+0x9734`, 21411 `+0x9758` / `+0x974c`).
- **Divergence rows** (same commit): `CX2GUElsword_SwordMan::InitPassiveSkillState`, `::AddComboToBrutalSlayer`, `::LightingStepFrameMove` (all ported).
- **Drift, own phase CH4a2 (this plan; subject = his passives' only readers):** the user state key `CREATE_EFFECT_SET` (parse `32057b80` @`320606b7`, play in
  `CommonFrameMove` `32063650` @`32063a6c`-`32063d06` at `+0x8e50`'s rate, the Fight God 21416 bonus with BTI 18) and `32610cb0` (21417 Rune Slayer Open Mind).
  Without it Wind Slicer only shows its rate in the init line. Elesis writes `+0x8e50` too (`32b74500`, `32b89260`): her port. Kickoff `CH4a2.md`.
- **The 22 passive ids:** 16 read here; 106 / 21400 by the base engine; 21405 is a type-2 stat passive; 21416 / 21417 -> CH4a2; 21418 also CH4c; 21420 CH4b. No state in June.

**Hygiene.** `flagprobe.py`: `_PASSIVE`, `_PASSIVE_DEBUG` and every dependency ON. One-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUElsword_SwordMan.cpp` exit 0, 0 errors.
`flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --worktree --files` (8 files): `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 1 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`
(OTHER = the three enum headers under `JUNE_ENUM_NAMES`; the EOL row is the `#endif` before `LightingStepFrameMove`'s LF-only `}`).
`junediverge.py --check` (Elsword .cpp / .h, X2GUUser.*): `RESULT: PASS`. `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
**Range (commit `85aa602e`):** `--range 85aa602e^..85aa602e`, same 8 files: `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`; 0 deleted, 1 EOL-mismatched; `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,997,824 B, dated 2026-11-05 13:40:59, md5 18269c1218c387c05af87c734b4e912a, equal to X2/US_SERVICE/x2.exe (2026-11-05 13:40:52); built at main d3f055b6 + 8 uncommitted code file(s).`
`EXESTR OK: 4 of 4 found in X2_offline.exe, 25997824 B, dated 2026-11-05 13:40:59, md5 18269c1218c387c05af87c734b4e912a` (the `init`, `lord knight open mind`, `extreme rune buster`, `sword enchant` lines).
**Save** (backup `els_db_20261105_133049_pre_pre_ch4a.sql`): unit 26 `jtRuneSlayer` + 21409 level 4; unit 34 `InfinitySwordT` + 21414 level 3 and transcended (`settranscend 34 1`).
**In game:** two `[JUNE_CHAR] ELSWORD PASSIVE init` lines per Elsword load; `brutal slayer`, `sword enchant`, `short time strong attack`, `lord knight open mind`,
`infinity sword open mind`, `extreme rune buster`, `lightning step` lines when each fires.
**Play-test ran (user, 2026-11-05; logs 14:05-14:12 and retest 14:18-14:19 on md5 `18269c12`; 0 EXCEPTION, 0 ErrorLog lines; known `E_DISABLE_HB_CHECK_REQ` UNHANDLED, one `AddGroupChainQuest`).**
Init lines per class as the code: Lord Knight unit 25 wind slicer 0.200, final defence x0.940, auto guard 0.250 (= ( 0.12 + 0.005 ) x2 in a dungeon) cool 45; Rune Slayer unit 26 rune
x1.27 / x1.53, fireball x1.000, magic reflect 0.120, enchant cool 20, extreme rune buster 1; Infinity Sword unit 34 walk / dash x1.120, anim x1.050. Seen: `lord knight open mind` after
3 hits, three stacks then the 20 s cool time; `sword enchant` factors 0 / 2 / 0, 20 s apart; `extreme rune buster` 152 / 153 / 168 by the random, one per target per cast;
`brutal slayer` every 5 hits with the 1 s cool time, Mind of Fighter 2 -> 5 (cap); `infinity sword open mind` on Conwell hits (units 70 / 71, the user's own 99s);
`lightning step` every ~0.49 s of dash. **Not seen:** `short time strong attack` (9% per non-special hit at level 4, 2 s cool; the first run's hits were mostly skills,
the retest's Sheath Knight run lasted ~15 s / ~10 hits). Left to the review: re-read `32d0bc60` and, if it holds, one longer plain-combo run.

### CH4a - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `85aa602e`; Ghidra decompiles plus `funcdis.py --func` listings of the 11 addresses, `charport_adis.py elsword 32d08ba0`.
- **`32d1c560` block by block.** Every `32890620` push is `( id, 1, 0, 1 )`, and `( id, 0, 0, 1 )` for 21200 / 21407 / 21411 / 21404. SA ids land on the port's members: 5 on `+0x8e50` and Auto Guard, 0x71 / 0x70 on
  `+0x9678` / `+0x9684` (min 10), 0x10 on `+0x57f8`+0x18 (final defence, the same member as March's Strong Bone), 0x1f / 0x20 / 0xf on `+0x96d4` / `+0x96d8` / `+0x959c`, 0x53 on `+0x95c0`, 0x94 on each cool time
  (elapsed = cool), and 0x22 / 0x22 / 0x67 on walk `+0x4870` / dash `+0x4924` / anim `+0x47bc` (+0x18, per the Eve / Chung offset rows). The resets (0 / -1 / 1.0) come before each `GetSkillLevel`, and only the cool time is reset for Brutal Slayer.
  The class gates are 10 / 100, 101, 22 / 112 and 112. Harsh Slayer registers before its templet test. Auto Guard adds a double 0.005 ( `fadd qword [33438ad8]` ) and doubles when `322cc1a0` is false ( `@32d1cbd4` ).
  **JUNE_BUGS 533 holds:** `[esp+0x20]` is written only at `@32d1c832` (Spell Chain's level; the later `lea [esp+0x20]` sit two pushes deeper) and read at `@32d1cc8a`.
  Spell Chain: the Magic Coating bool is pushed before `32089f60` (`ret 8`) and stays on the stack as `322a25f0`'s 2nd argument, which matches the port's `( clone, bMagicCoatingLearned )`. Cutting Sword
  clones `[0]` and `[1]` with a `push 0`, after `32205510( &type )` and `type != 0xd`. The lists come from the `ecx` of `3207b770`: `+0x66ac` attack result for Brutal / Harsh / Sword Enchant /
  Short Time / Infinity Sword Open Mind / Extreme Rune Buster, `+0x66cc` common frame move for Lightning Step, `+0x66dc` damage react for Lord Knight Open Mind. All match.
- **Callbacks.** Brutal Slayer: riding, then `cool <= elapsed` (the `a < b != a == b` idiom), combo > 0, count +1, `> 4`, buff factor 0, Mind of Fighter +1 when > 0 (cap 5, `CONST_BRUTAL_SLAYER_LEVEL`),
  then `32d27190` (elapsed `+0x9704` = 0, count `+0x9710` = 0). Sword Enchant / Short Time Strong Attack: riding, unit, `attackType != 1` (AT_SPECIAL), cool time over, `( id, 0, 0, 1 )`, then
  `GetRandomFloat( 0 ) < SA 5` (`fcomp`, `test ah,5`, `jp`). The factor is `GetRandomInt( 0 ) % 3` or 0, then elapsed = 0. **`32d0bc60` holds**, so Short Time Strong Attack not showing up in the short runs is chance at 9 %.
  Lord Knight Open Mind: `322a7c30( 824 )`, or `0 <= remain` and not `HaveThisBuff( 824 )`, gives count 0. Otherwise: level, hyper-class, transcended, `*damageData != 6` (DT_BUFF), threshold = `fistp` under
  control word `| 0xc00` (truncated), `+0x9794` = 0, `count < threshold` returns, buff, count 0, cool time = SA 0x94 into `+0x9788`. Infinity Sword Open Mind: trigger `+0x53c == 0xf`.
  Extreme Rune Buster: defender, game, skill `+0x5c4 == 21059`, trigger 0x2a; UID dedupe (vslot 0x54) over `+0x97c4`, then push; `size % randomSize`, `> 0.67` -> 153, `> 0.33` -> 152, else 168.
  vslot 0x21c is `322a6960`; its June order `( BFI, creator, level, 0, 0 )` is March's ported overload `( BFI, level, creator, adjust 0, skip false )`. Lightning Step `32d08ba0`: `> 0.5`, the time reset after a fire, on a level / templet miss, and outside dash.
- **Stand-ins** (each one line in code): no game -> Auto Guard is not doubled (June dereferences the game unchecked); an empty random list returns (June divides by zero); `+0x9794` dropped (no reader).
- **Names / values.** A compile probe (`cl /Zs`, `typedef char[ ( X == v ) ? 1 : -1 ]`) checked 49 values: 11 SA, 5 ST / SPRT, 18 skill ids, 5 classes, BTI 824, BFI 152 / 153 / 168, DTT 15 / 42,
  and AT_SPECIAL = 1, DT_BUFF = 6. The 18 new names are June's `Enum.src.lua` names at June's values.
- **`#else`** keeps March's bodies whole (0 deleted lines); `m_fCreateEffectSetRate` (user `+0x8e50`, ctor 0) has no reader until CH4a2.
- **Logs** (only the 14:17-14:19 retest is still on disk; the 14:05-14:12 run's lines are quoted in the Done note above): 26 lines. 12 `init` (six pairs, each written twice 0.05 s apart, values
  equal: the base reset holds and no delegate doubles), 12 `lightning step`, 2 `brutal slayer` (Mind of Fighter 2 -> 3); 0 EXCEPTION; 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ` and one `AddGroupChainQuest` (baseline).
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_PASSIVE --with ..._PASSIVE_DEBUG --range 85aa602e^..85aa602e --files` (8 files): `files: 4 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`,
  `1 EOL-mismatched` (the LF `}` after `LightingStepFrameMove`, as in the Done note), `RESULT: PASS`; no later commit touches the flag. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`.
  `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`. Retirement: `//#define SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG`. `flagcheck JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --worktree --files KTDXLIB/Always.h`
  gives `1 DEFINE`, `1 deleted line(s)` (the `//#define`), and `RESULT: FAIL` only because the flag is no longer `#define`d (the expected result of a retirement). `junediverge.py --check KTDXLIB/Always.h`: `RESULT: PASS`.
  Build started, not awaited.
- **Optional, no log line now:** a ~1 min plain-combo run on a Sheath Knight with Short Time Strong Attack shows its buff icon now and then (9 % per non-special hit, 2 s cool time).

### CH4a2 - Done (2026-11-05)

**Landed.** Commit: see `git log`, "char-elsword Phase CH4a2". New class-agnostic flag `SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET` (needs `JUNE_TRANSCEND_UNIT`, `JUNE_SKILL_TEMPLET`,
`JUNE_SKILL_STATE_PTR`, `JUNE_ENUM_NAMES`, all checked ON with `flagprobe.py`); `_PASSIVE_DEBUG` re-opened for the log. Files: `X2GUUser.cpp` / `.h`, `X2SkillTree.h`, `X2BuffFactor.h`, `Always.h`.
Read from `disasm_range.py` listings (Ghidra fails to decompile `32063650`); rows `(CH4a2)` at the end of `june2017_re/funcmap.tsv`.
- **Members** (`X2GUUser.h`): `m_fCreateEffectSetRate` (`+0x8e50`) now under the new flag, CH4a's `_PASSIVE` line kept in its `#else`; new `m_wstrCreateEffectSetName` (`+0x8e2c`),
  `m_fCreateEffectSetTime` (`+0x8e48`), `m_fCreateEffectSetRotateDegreeZ` (`+0x8e4c`). Ctor 0 / 0 (stand-in: June's ctor leaves the two floats unset).
- **Parse** in March's `CommonStateStart` = June `32057b80` @`320606b5`-`32060736`: name = L"" first; `BeginTable( "CREATE_EFFECT_SET" )` (string `33478ecc`, whose only xref is this
  parser: no NPC reads the key, so the PvP-hero NPC scripts' copies are inert in June too) -> `[1]` name (miss L""), `[2]` time (miss 0), `[3]` (miss 0). A state without the key keeps
  the last time and `[3]` (unread, the name is empty). Placed after the skill-state-end stops and before `SPECIAL_OPTION_VALUE`, June's order. `3203a620` @`3203b66f` (the kickoff's
  "clear") is the user dtor freeing the string.
- **Play** in March's `CommonFrameMove` = June `32063650` @`32063a6c`-`32063d06`, after the buff resync and before March's `RIDING_SYSTEM` riding-wait block (June runs that at
  `@32063d06`: state index 76, `+0x89d8`, `+0x921c`). Bonus = `m_fCreateEffectSetRate` when `HaveThisBuff( BTI_SI_A_ES_ENDURANCE )` (Iron Body, 21201 / enhanced 21218 both apply BTI 18),
  Fight God 21416 templet is `SPRT_HYPER_CLASS`, `m_iUnitClassTran == 1` and `GetSkillLevel( 21416, true, 0, true ) > 0` (June's order). Then name non-empty, `GetRandomFloat( 0 ) <
  rate + bonus`, `GetXSkinAnimPtr()->EventTimerOneshot( time )` -> `PlayEffectSet( name, this, NULL, now state m_bHyperState, GetPowerRate(), -1, scale x3, false, GetPos(),
  GetRotateDegree() with z += [3], 0 )` (force-down 1.0 = the default). **The kickoff's "y offset" is the rotation's z**: June adds `+0x8e4c` to vslot `0x150` `GetRotateDegree`'s
  third float; every June row (Elsword 39, Elesis 46) has 0 there.
- **`32610cb0` (21417 Rune Slayer Open Mind):** no caller anywhere (no E8 / E9, no absolute pointer in the file, no Ghidra xref; the only other `push 0x53a9` is `32d0c780`
  `ApplyBuffToOpenMind`, ported in CH1h). Dead in June: nothing to port. (It would also return true for a non-hyper-class templet.)
- **Names** (`JUNE_ENUM_NAMES`): `RNSI_P_ELK_FIGHT_GOD` 21416, `BTI_SI_A_ES_ENDURANCE` 18 (June's `Enum.src.lua`).
- **Routed:** Elesis' two rate writers (`32b74500` @`32b7471b`, `32b89260` @`32b8930d`) -> `char-elesis` (bullet in `kickoffs/june-backlog/stage7-plan.md`, row 7); until then her
  `EffectSet_Elesis_SBH_Blaze_Wing_Soul_Ignition` rows play at rate 0. The block before June's (`@3206393c`-`32063a6c`, `+0x8e92` / `+0x8e91`, `3204df80` UpdateDuo) is Lu/Ciel's DU1.
- **Checklist:** §2 L124 and §7ao's Elsword row marked `(Elsword: CH4a2)`. No JUNE_BUGS row (no quirk with an effect: the kept time / `[3]` are unread with an empty name).

**Hygiene.** `flagcheck.py JUNE_USER_CREATE_EFFECT_SET --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --worktree --files` (5 files): `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`
(OTHER = the two enum headers under `JUNE_ENUM_NAMES`; the deleted line is `//#define SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG`, now `#define`d).
`junediverge.py --check` (the 5 files): `allow-listed: 7`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s)`.
**Range (commit `58e53890`):** `--range 58e53890^..58e53890`, same 5 files: `files: 2 PASS, 0 PASS~, 1 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; 1 deleted (the `//#define`); `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,999,360 B, dated 2026-11-05 15:00:15, md5 007afdce5353d295af8cdc4692afe75c, equal to X2/US_SERVICE/x2.exe (2026-11-05 15:00:07); built at main 732250f9 + 5 uncommitted code file(s).`
`EXESTR OK: 3 of 3 found in X2_offline.exe, 25999360 B, dated 2026-11-05 15:00:15, md5 007afdce5353d295af8cdc4692afe75c` (the `played` / `missed` lines, `CREATE_EFFECT_SET`).
**Save** (backup `els_db_20261105_145154_pre_sql.sql`): unit 25 `jtLordKnight` (transcended since CH3d, Wind Slicer 21402 level 4) + Fight God 21416 level 1 and Enhanced Iron Body
21218 level 1 in slot 7 (B-3).
**In game:** `[JUNE_CHAR] ELSWORD PASSIVE create effect set played: EffectSet_SI_P_ELK_WIND_SLICER ( ... roll r < rate 0.200 + fight god 0.000 )` / `... missed: ...` at each
combo's event time; with Iron Body up, `fight god 0.200`.
**Play-test PASS (user, 2026-11-05; run 15:30-15:31 on md5 `007afdce`; 0 EXCEPTION, 0 ErrorLog lines; known `E_DISABLE_HB_CHECK_REQ` UNHANDLED x2, one `AddGroupChainQuest`).**
Init: unit 25 class 100 wind slicer 0.200. 43 event lines over 10 states (combos 117-120, 122, 126-128, 152): bonus 0 -> 6 played / 23 missed (0.21); bonus 0.200 (Iron Body up)
-> 6 played / 8 missed (0.43); every line's played / missed agrees with its roll vs rate + bonus. The user saw the effect play.

### CH4a2 - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `58e53890`; `disasm_range.py --x87` listings `32063a40`-`32063d20`, `32060680`-`32060750`, the ctor `3203e850`-`32043400`, plus a byte scan of the exe.
- **The play** `@32063a6c`-`32063d06`: bonus 0 (`fldz`), then `3227baf0( 0x12 )` HaveThisBuff, `32206450( 0x53a8, 1 )` GetSkillTemplet, `32205590` hyper-class, `[+0x867c]+0x3c == 1`,
  `32890620( 0x53a8, 1, 0, 1 ) > 0` -> bonus = `+0x8e50`: the port's order. Name test `31c1c1a0` vs L"" (`je` skips when empty). Roll vslot `0x1cc( 0 )` is taken **before** the timer, as in the port;
  `fld rate; fadd bonus; fcompp; test ah,0x41; jne skip` plays only when `rate + bonus > roll` (= `roll < rate + bonus`). `32071d80` then `31e4c230( +0x8e48 )`, result tested after the smart pointer's release.
- **`PlayEffectSet` `321e1e60` by stack offset** (base = esp at `@32063bee`): hidden handle `+0x1e4`, name copy `+0x4b8`, this, NULL, bHyper = byte `+0x10` of the `+0x7cbc` now-state pointer
  (`UserUnitStateData::m_bHyperState`, C9h-4d row; stored at `+0x34`, pushed from `[esp+0x5c]` 40 bytes deeper), vslot `0x204` power rate, life `[0x334380f0]` = -1.0 (read from `.text`), scale `+0xe0c` x3
  (`m_fScale` = `GetScale()`), custom position 0, `GetPos( 0 )` vslot `0x138`, rotation vslot `0x150` copied to `+0x46c` with `+0x8e4c` added to its **z** (`+0x474`), direction `( 0, 0, 0 )` at `+0x1b4`, 1.0.
  `322ceae0` is `GetEffectSet`. Every argument matches March's 12-argument `PlayEffectSet` (`X2EffectSet.h:1083`, force-down default 1.0). Placement before `RIDING_SYSTEM` = June's `@32063d06` block.
- **The parse** `@320606b5`-`32060736`: `+0x8e2c` = L"" (`31f54a10`) first; `BeginTable( 33478ecc, 1 )` `cmp al,1`; `[1]` wstring miss L"", `[2]` / `[3]` float misses `fldz`; `EndTable`; nothing on
  the absent-key path. June's order is `3228a490`, `32278200` sound stop, `3200bde0`, the parse, `31fce500`, ... `SPECIAL_OPTION_VALUE` `@320608a9`: the port sits after the sound stop and before
  `SPECIAL_OPTION_VALUE` on the same table. Matches.
- **`32610cb0` dead:** no `E8` / `E9` / `0F 8x` rel32 to it and no absolute pointer anywhere in the file. Of the six `a9 53 00 00` dwords, four are `push 0x53a9` inside `32610cb0` and `32d0c780`
  (`ApplyBuffToOpenMind`, CH1h); `31de9c82` is `mov [esp+0x53a9], bl` (a stack offset) and `33078eb5` is misaligned bytes. Not ported, correctly.
- **Ctor:** `3203e850`-`32043400` writes only `+0x8e2c` (the string ctor `@32040de2`) and `+0x8e50` (`@32040e15`); `+0x8e48` / `+0x8e4c` are unset, so the 0 / 0 stand-in is right.
- **`#else` / flag off:** the members and the ctor line fall back to CH4a's `_PASSIVE` `m_fCreateEffectSetRate`; the parse and play are insert-only. **Names:** `RNSI_P_ELK_FIGHT_GOD = 21416`
  (`Enum.src.lua:15672`), `BTI_SI_A_ES_ENDURANCE = 18` (`:12891`).
- **Logs** (15:30-15:31, md5 `007afdce`): 43 lines in 9 states (117-120, 122, 126-128, 152; the Done note's "10" is one too many); bonus 0: 6 played / 23 missed (0.21), bonus 0.200: 6 / 8 (0.43);
  0 lines disagree with `roll < rate + bonus`. The debug-only `missed` branch calls `EventTimerOneshot` a second time (a read of the anim time, no state), gone with the flag.
- **Gates:** `flagcheck JUNE_USER_CREATE_EFFECT_SET --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range 58e53890^..58e53890 --files` (5 files): `files: 2 PASS, 0 PASS~, 1 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`,
  `1 deleted line(s)` (the `//#define`), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s)`.
  Retirement: `//#define SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` (bytepatch, CP949 / CRLF unchanged). Build started, not awaited.

### CH4b - Done (2026-11-05)
**Landed.** Commit: see `git log`, "char-elsword Phase CH4b". Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (new guard line: needs `JUNE_CHAR_EVE_PASSIVE` for `GetSkillBuffFactorPtr`,
`JUNE_DIVERGE_SKILL_LEVEL_VALUE`, `JUNE_USER_SKILL_LEVEL_HELPERS`, all ON by `flagprobe.py`); `_PASSIVE_DEBUG` re-opened for the log. Files: `X2GUElsword_SwordMan.cpp` / `.h`,
`X2DamageManager.h`, `Always.h`. Read from `charport_adis.py` / `funcdis.py` / `disasm_range.py` listings; rows `(CH4b)` at the end of `june2017_re/funcmap.tsv`.
- **`SetDamageExceptionProcess` `32d1e0e0`** (new override, vslot 0x2b8, under `MODIFY_SET_DAMAGE_DATA` like Eve's): base `32045340` first; Infinity Sword Open Mind 21419
  learned (`31fceab0` = `GetSkillLevelValue`) and `m_eDamageTrigger` (`+0x53c`) == `DTT_ETK_CONWELL_ATTACK` -> `m_fCriticalRate` (`+0x510`, the `CRITICAL_RATE` key) = 1.0:
  every Conwell hit is a critical. Infinity Potential 21420 with trigger 16 -> `GetSkillBuffFactorPtr( 21420, 4, true )` (vslot 0x238 `32028af0`, 0-based) pushed by `322b5320`
  ( factor, level, clear 1, rate `{ 1.0, 1.0, off }` ) = `PushBuffFactor( GetClonePtr( level ), true )`; a null factor still clears and re-adds the socket factors, as June.
  **JUNE_BUGS 534 (latent):** no June script or `.text` store sets trigger 16, and 21420 has four buff factors, so index 4 is null. New name `DTT_EIS_INFINITY_CHASER` = 16
  (`Enum.src.lua:7290`) under `JUNE_ENUM_NAMES`. Stand-in: a null damage data returns (June reads it unchecked).
- **`GetActualMPConsume` `32cf4b70`** whole inside March's: base `( id, level )` (June's bool 1 = March's path); Armageddon Blade 21008 / 21044 with Iron Body's BTI 18 up and
  Fight God 21416 learned -> `( 1 - SA_MP_CONSUMPTION( 21416 ) ) x MP` (0.2 in June's templet: 250 -> 200); Iron Body 21201 / 21218 with Fight God -> MP - 10.0 (a double
  constant `33438088`, no ability value: 60 -> 50); no clamp. June has no Vigor x0.7 or Mind of Fighter here (CH1k's `32d1bd10` applies both), so nothing is applied twice.
  March's memo 19 Sword Enchant -5 and the `SERV_BALANCE_FINALITY_SKILL_EVENT` rate stay in `#else`.
- **`GetActualCoolTime` `32cf4df0`** (new override, vslot 0x6a4): null -> 0; base; `ST_ACTIVE` (templet `+4` == 6) x `m_fIncreaseCoolTimeRel` (`+0x96e0`); < 0 -> 0. June's only
  writer of `+0x96e0` is the ctor `32d09490` @`32d09d7c` (1.0), and CH4a left March's Intensive Attack writer in `#else`, so the rate is 1.0: the cool time is the base.
  This is where CH1k's question lands: March's memo 22 / 6 -5 s and Compact Counter cool times are gone in June; only the `ST_ACTIVE` rate is kept.
- **The CH1q amendment's Way of the Sword sites (`ch0_wos.tsv` rows 83 / 84 / 87 / 95): all = March, nothing to port.** Shock end in `CommonFrameMove` `32063650`
  @`32068874`-`320688fa` (`m_EnchantShock.m_bCured` `+0x2540`), frozen end `31ff43c0` @`31ff4642` (`m_Frozen.m_bCured` `+0x17f0`): the same three state choices as
  March; June's user state ids are March's + 1 (26 / 27 / 30 / 85 / 86 vs March 25 / 26 / 29 / 84 / 85, compile probe). `CureExtraDamage` `3227d390` (funcmap 7320).
  `ResetAnimSpeed` `31ffe190` @`31ffe262`-`31ffe2aa`: Elsword in Vigor x `+0x961c` inside the item-speed branch. `UseItemSpecialAbility` `322fbc60` case 33 @`322fd29d`:
  my unit -> `SetWayOfSwordPoint( GetWayOfSwordPoint() + Value1 )`. Rows filled in `ch0_wos.tsv`.
- **Side items:** §7 L281 power-rate socket range `32d076f0` = `RevengeAttackStart`, ported in CH1b; the death-prevention snapshot `32d058a0` went to CH1q (both rows
  already marked). Checklist §7b row (L330) marked `(Elsword: CH4b)`. CH0's three CH4b functions are all done.
- **Names / values** (compile probe, 18 values): `SA_MP_CONSUMPTION` 143, skills 21008 / 21044 / 21201 / 21218 / 21416 / 21419 / 21420, `ST_ACTIVE` 6, BTI 18, DTT 15,
  `UT_ELSWORD` 1, `SAT_WAY_OF_SWORD_GAUGE_UP` 33. Numbers kept with a comment: the factor index 4 and the 10.0 MP (no Enum entry).
**Hygiene.** One-file probe (`census.cl_cmd`, `/Zs /EHsc`): `X2GUElsword_SwordMan.cpp` exit 0, 0 errors.
`flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --worktree --files` (4 files): `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`
(OTHER = `X2DamageManager.h` under `JUNE_ENUM_NAMES`; PASS~ = the blank line before the new functions; the deleted line is `//#define ..._PASSIVE_DEBUG`, now `#define`d).
`junediverge.py --check` (the 4 files): `allow-listed: 3`, `RESULT: PASS`. `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
**Range (commit `e2eb3ad5`):** `--range e2eb3ad5^..e2eb3ad5`, same 4 files: `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; 1 deleted (the `//#define`); `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 26,001,408 B, dated 2026-11-05 16:25:35, md5 a08ac02df4ad051524267eabdbee0ca4, equal to X2/US_SERVICE/x2.exe (2026-11-05 16:25:28); built at main 988cae73 + 4 uncommitted code file(s).`
`EXESTR OK: 4 of 4 found in X2_offline.exe, 26001408 B, dated 2026-11-05 16:25:35, md5 a08ac02df4ad051524267eabdbee0ca4` (the `fight god mp`, `active cool time`, `infinity sword open mind: conwell hit`, `infinity potential` lines, wide).
**Save** (backup `els_db_20261105_161635_pre_sql.sql`): unit 25 `jtLordKnight` Armageddon Blade 21044 into slot 6 (B-2; 21038 out). Unit 34 `InfinitySwordT` already has 21419 and is transcended.
**In game:** `[JUNE_CHAR] ELSWORD PASSIVE fight god mp: skill 21044 mp 250.00 -> 200.00` (Iron Body up) / `skill 21218 mp 60.00 -> 50.00`; `active cool time: skill 21218 base ... x 1.000`;
`infinity sword open mind: conwell hit critical rate 1.0` on unit 34's Conwell combos (every Conwell hit a critical). All at most once a second.

**Play-test PASS (user, 2026-11-05; run 16:30-16:33 on md5 `a08ac02d`; 0 EXCEPTION, 0 ErrorLog lines; known `E_DISABLE_HB_CHECK_REQ` UNHANDLED x3, one `AddGroupChainQuest`).** `fight god mp`: 21218 60.00 -> 50.00 (68 lines, about one a second from 16:31:04: the MP check is polled while the slot shows), 21044 250.00 -> 200.00 (9 lines, only while Iron Body was up); `active cool time`: 21218 base 15.00 x 1.000 -> 15.00 (4 casts); `infinity sword open mind: conwell hit critical rate 1.0` 9 lines (throttled; `applied skill 0` = combo hits) beside CH4a's 43 `conwell hit, buff` lines; 0 `infinity potential` lines (latent, JUNE_BUGS 534). The user saw the crits and the MP costs.

### CH4b - Review (2026-11-05)

Opus 5.5. **Verdict: passes, no code change.** Commit `e2eb3ad5`; listings `charport_adis.py elsword 32d1e0e0 32cf4b70 32cf4df0`, `funcdis.py --func 322b5320`, `disasm_range.py 32028af0 32028ba0`, `31ffe230 31ffe2b0`, `dispsweep.py 96e0 53c`.
- **`SetDamageExceptionProcess` `32d1e0e0`:** base `32045340( dd, &rate )` first; `31fceab0( 0x53ab ) > 0` and `[dd+0x53c] == 0xf` -> `fld1; fstp [dd+0x510]`: the port's Open Mind block.
  21420: `31fceab0( 0x53ac ) > 0`, `[dd+0x53c] == 0x10`, vslot `0x238( &ret, 0x53ac, 4, 1 )`; rate built at `@32d1e172`-`32d1e192` = `{ 1.0, 1.0, byte 0 }`; `322b5320( ptr, 31fceab0( 0x53ac ), 1, &rate )`.
  `322b5320`: clear -> erase `+0x40` vector and `326309f0` (= `PushSocketAndTemporaryBuffFactor`) even for a null factor; null -> return; `32089f60` levelled clone, `32d4d670` = `SetGameUnitBuffFactor`
  (attacker), push; rescale `32089ff0` only when the rate byte is 1 (here 0). March's `PushBuffFactor( GetClonePtr( level ), true )` does the same (its own plain clone of the levelled clone changes nothing).
  `32028af0` already returns a levelled clone and returns null when the factor count <= index: index 4 of four is null. JUNE_BUGS 534 holds: June's `Enum.src.lua` is the only file under
  `june_src_fixed/` naming `DTT_EIS_INFINITY_CHASER`, and no `.text` store writes an immediate 0x10 to `+0x53c`.
- **`GetActualMPConsume` `32cf4b70`:** base `31fd1840( id, level, 1 )`; `id - 0x5210 <= 0xd2`, index bytes at `32cf4c1c`: `[0]` (21008) and `[0x24]` (21044) = 0 -> `32cf4ba4`, `[0xc1]` (21201) and
  `[0xd2]` (21218) = 1 -> `32cf4be7`, rest 2 -> `32cf4c05` (decoded from the bytes under the jump table). Case 0: `HaveThisBuff( 0x12 )` before `31fceab0( 0x53a8 ) > 0`, then `32007b10( 0x8f, 0x53a8 )`,
  `DE E1` = 1 - value, x base, returned. Case 1: 21416 learned -> base - `[33438088]` (double 10.0). No clamp. March's memo 19 and event rate only in `#else`.
- **`GetActualCoolTime` `32cf4df0`:** null -> `fldz`; base `31fd1920( t, level, 0 )` (third argument unread, funcmap 7289); `[t+4] == 6` -> x `[this+0x96e0]`; `fcom 0.0; test ah,5; jp` replaces only a
  value below 0 (NaN kept), as the port's `< 0.f`. `dispsweep 96e0` in Elsword's range `32ce0000`-`32d40000`: only the ctor `@32d09d7c` (fst) and this read. March's writers (`X2GUElsword_SwordMan.cpp:5121`-`5135`)
  sit in `_PASSIVE`'s `#else` (from l.4736) and the reader `:9317` in `_LIVE`'s `#else` (from l.9087): the rate stays the ctor's 1.0.
- **Way of the Sword spot-check, `ResetAnimSpeed` `31ffe262`-`31ffe2aa`:** `[[+0x867c]+0xaf0]` type == 1 (`UT_ELSWORD`), vslot `0x674` (way-of-sword state) == 2 -> x `+0x961c`: March's
  `X2GUUser.cpp:35240`-`35248` (`WSS_VIGOR` x `GetAnimSpeedUpByVigor()`). Equal, as the Done note says.
- **`#else` / flag off:** `GetActualMPConsume`'s March body is untouched in `#else`; the two overrides and their declarations are inside `_PASSIVE` (and `MODIFY_SET_DAMAGE_DATA`); `DTT_EIS_INFINITY_CHASER = 16`
  under `JUNE_ENUM_NAMES` (`Enum.src.lua:7290`).
- **Logs** (16:30-16:33, md5 `a08ac02d`): `fight god mp` 21218 60.00 -> 50.00 x68, 21044 250.00 -> 200.00 x9; `active cool time` 21218 15.00 x 1.000 -> 15.00 x4; `infinity sword open mind: conwell hit
  critical rate 1.0` x9 beside 43 CH4a `conwell hit, buff` lines; 0 `infinity potential`. As the hand-off. (`logs.py --grep "JUNE_CHAR ELSWORD PASSIVE"` matches nothing because of the bracket; use `"ELSWORD PASSIVE"`.)
- **Gates:** `flagcheck JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --range e2eb3ad5^..e2eb3ad5 --files` (4 files): `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`,
  `1 deleted line(s)` (the `//#define`), `RESULT: PASS`. `junediverge.py --check-all`: `allow-listed: 185`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
  Retirement: `//#define SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` (bytepatch, CP949 / CRLF unchanged). Build started, not awaited. CH4c's kickoff checked: ready (CH3d review done).

### CH4c - Done (2026-11-05)
**Landed.** Commit: see `git log`, "char-elsword Phase CH4c". Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (no new dependency); `_PASSIVE_DEBUG` re-opened for the log. Files: `X2GUUser.cpp`,
`X2GUElsword_SwordMan.cpp` / `.h`, `Always.h`. Read from `disasm_range.py 320469c0 32046b40` and `charport_adis.py elsword 32d0c820 32d1b650 32cf4d00`; rows `(CH4c)` at the end of `june2017_re/funcmap.tsv`.
- **Rune Slayer reflect, `DamageReact` `320462a0` @`32046a46`-`32046aa0`** (checklist 7j, divergent row 220), June's branch inside March's `NEW_SKILL_2010_11` block, March's roll in `#else`:
  unit set and class `+0x38` == `UC_ELSWORD_RUNE_SLAYER`; DD `bCanReflexMagic` (`+0x447`) and `ApplyMagicCoating()` true -> `damage.fMagic` (`+0x3c`) = 0 and **no reflect roll**;
  otherwise March's roll (`GetRandomFloat( 0 ) <= GetMagicReflectRate()`, `fcomp; test ah,1`: NaN skips, as March) sets `bReflexMagic` / `bDamageReactReflectMagic`. Elsword-only
  (class-keyed), so `_PASSIVE`, not a `JUNE_DIVERGE_*` flag. The `RT_REFLECT` / `RT_REVENGE` choice after it (`@32046afc`-`32046b24`) and the reflect-by-buff test = March.
- **`ApplyMagicCoating` = June `32d0c820`** (new, called only from `@32046a66`): Iron Body up (`HaveThisBuff( BTI_SI_A_ES_ENDURANCE )`, 18) -> templet `RNSI_P_ERS_MAGIC_COATING`
  (21418), unit, transcended (`IsTranscendedUnit`, unit `+0x3c`), `SPRT_HYPER_CLASS` (templet `+0x174` == 5), `GetSkillLevel( 21418, false, 0, true ) > 0` -> MP `SA_MP_REGENERATION_ABS`
  (43; June's templet: 5) via `UpNowMp` (vslot `0xf0`) and `DisplayAddMPInformation( v, GetPos(), GetDirVector() )` (`322d38f0`), true. June's tooltip: "Iron Body will now absorb
  magical projectiles, negating its damage and gives MP recovery". No June quirk found, no `JUNE_BUGS` row.
- **Equal to March, nothing to port:** `GetMagicReflectRate` `32d1b650` (`m_fMagicReflectRate` `+0x95c0` x 3.0 in `GT_DUNGEON` / `GT_BATTLE_FIELD`); Magic Resistance 21409's
  writer is CH4a's `InitPassiveSkillState`; Elsword's two `RT_REFLECT` readers (his `DamageReact`, CH1's June body under `_LIVE`).
- **`MemoConditionElsword`: n/a**, as Chung's. Decide `32cf4d00` = base `31fd4450( user )` != 0 (`xor al,al`: always false), parse `32cf4d20` = `jmp 31fc8cf0`; divergence
  row `MemoCondition` (D1-1): March's `m_bApplyMemo` already gives June's answer. The ctor's `+0x7d10` object (CH1k amendment) is therefore not created.
- **S3d memo consumers:** no code. Their `[JUNE_SKILL] MEMO` lines needed `SERV_IRUHADEV_JUNE_SKILL_DEBUG`, which no longer exists in `Always.h`, so the check is visual (hand-off
  check 3); otherwise it stays a CH6 walk item (`plan_checklist_rows.md`, "S3d memo consumers").
- **Tables:** checklist §7j L458 and §7ao L963 marked `(Elsword: CH4c)`; divergent row 220 filled (write_instead, guard, source); funcmap rows `(CH4c)` x4.
**Hygiene.** One-file probes (`census.cl_cmd`, `/c /Zs /EHsc`): `X2GUElsword_SwordMan.cpp` and `X2GUUser.cpp` exit 0, 0 errors.
`flagcheck.py JUNE_CHAR_ELSWORD_PASSIVE --with JUNE_CHAR_ELSWORD_PASSIVE_DEBUG --worktree --files` (4 files): `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (the deleted line is the `//#define ..._PASSIVE_DEBUG`).
`junediverge.py --check` (Elsword .cpp / .h, `X2GUUser.cpp`): `allow-listed: 10`, `RESULT: PASS`. `enumscan.py --check X2GUElsword_SwordMan.cpp`: `0 A/B row(s)`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 26,002,432 B, dated 2026-11-05 17:00:09, md5 6586b82f723fa9c55304ec35a09df8b7, equal to X2/US_SERVICE/x2.exe (2026-11-05 17:00:01); built at main 2fcb1acb + 4 uncommitted code file(s).`
`EXESTR OK: 3 of 3 found in X2_offline.exe, 26002432 B, dated 2026-11-05 17:00:09, md5 6586b82f723fa9c55304ec35a09df8b7` (the two `magic coating` lines and `magic reflect: roll`, wide).
**Save** (backups `els_db_20261105_165141_pre_sql.sql`, `..._165201_...`): unit 26 `jtRuneSlayer` (transcended) + Magic Coating 21418 level 1 (unsealed row) and Iron Body 21201 level 1 in slot 3 (21000 out).
**In game:** `[JUNE_CHAR] ELSWORD PASSIVE magic coating: magic damage <m> -> 0, physic <p> kept` then `magic coating: mp +5.0 ( level 1 ), now <mp>` per magic projectile taken with Iron Body up
(and the "+5" MP text at Elsword); without Iron Body, `magic reflect: roll <r> <= rate 0.360, reflectable <0|1>` (rate 0.12 x3 in a dungeon) and the projectile flies back when reflectable.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI`. Sub-tag `UI`.

**Scope:** Elsword's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays, `UC_150` stays out):
`m_fLobbyRotationY`, `m_HyperCutIn` for 100 / 101 / 112, the names `STR_ID_4641` / `4642` / `17425`; the cut-in table
`X2GUElsword_SwordMan.cpp:14-44` and the InitState index switch `:873-898` replaced by June's names and layout (7t cut-in `.gfx` names); 7ah; the
dialog / gauge pieces CH1q does not take. **Done gate:** a select-screen pass of all nine March characters with no new error; Elsword's skill and
hyper cut-ins play June's art.

### CH5 - Done (2026-11-04)

- **`UnitTemplet.lua` (data036, core-list BOM source):** Elsword's 7 blocks are June's text, block-wise swap (55 blocks before and after, the other 48 untouched, BOM and CRLF kept; the keys inside the 7 blocks
  follow June's order). The semantic delta vs the old file is exactly `ch0_unittemplet_elsword.tsv`'s 13 keys: `m_fLobbyRotationY` 35 / 25 / 25 / 10 / 3 / 10 / 10 on 1 / 10 / 11 / 22 / 100 / 101 / 112, `m_HyperCutIn`
  `HQ_Cutin_ELK / ERS / EIS_Transcendence` on 100 / 101 / 112 (no `m_BerserkCutIn`), `name` `STR_ID_614` -> `STR_ID_4641` / `4642` / `17425`. `UC_150` stays out. `luasrc.py refresh data036 UnitTemplet.lua`
  REFRESHED, `ledger --check` PASS, `core --check` PASS, `luabind.py check UnitTemplet` PASS (0 unbound). File sha1 `2fd61cac`, md5 `431753f8`.
  `kom.py pack data036`: PACK OK, `vs live data036.kom: ~1 changed` = `UnitTemplet.lua` only. **`COPY OK data036`** (user yes) md5 `f017db1de0b8119f5b9a2a8fbfb3660c`, backup `kom_backup_tools/data036.kom.20261104_211841`.
- **Cut-in table (`SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI`, new at the end of `Always.h`; `#error` without `_ELSWORD_LIVE` / `_JUNE_HYPER_CUT_IN_TIME`, both top-level defines at 11467 / 6840):**
  `s_SkillCutInSet` = June `333ac120` (table `337aecc0`, 12 entries, set stride 0x90): set 0 `HQ_CutIn_Elsword_01` / `_02` / `_03` (Sword Man, Sword Knight, Magic Knight, Sheath Knight),
  set 1 `_LK_01` x3, set 2 `_RS_01` x3, set 3 `_IS_01` x3. Names carry no extension (`ActiveSkillShow` appends `.gfx`) and every entry is March's one-argument `SkillCutIn( name )` (June's `32078b70`).
  March's `.dds` table sits in `#else`. June's table holds no April-fools entry; `s_SkillCutInSetFoolsDay` stays March's in the `#else` reader only.
- **Already June's from CH1, unchanged here:** the InitState index switch (`32d23ff0`, `m_iSkillCutInSetIndex` 0 for the four Sword Man family classes, 1 / 2 / 3 for 100 / 101 / 112), the reader
  `ShowActiveSkillCutInAndLight` (`32d09050`, `cutIn[uiCutInIndex_]`) and the 0x8b3 CASH_21 / CASH_22 pair (`337aec60` / `337aec90`). CH5 only adds the log lines below.
- **Assets:** the six `HQ_CutIn_Elsword_*.gfx` are in data152 (2202 / 2206 / 2208 / 2107 / 2185 / 1788 B) and the three `HQ_Cutin_E{LK,RS,IS}_Transcendence.gfx` in data195 (2451 / 2730 / 2527 B), all June's bytes
  from CH0b; `kom.py status data152` / `data195` in sync with live. Nothing staged.
- **Logs:** `[JUNE_CHAR] ELSWORD UI cut-in set <set> index <i> name <gfx>` (class reader, after the 0x8b3 override) and `[JUNE_CHAR] ELSWORD UI hyper cut-in class <c> name <gfx>`
  (`CX2GUUser::ShowHyperSkillCutInByScript`, Elsword only).
- **Checklist rows:** 7t L592-593 and L617-623 marked `(Elsword: CH5)`. **7ah** (`UpdateElswordSkillSlotEffect`) was taken by CH1q2. **The guide dialog, `LevelUpEvent.lua` TYPE row and the `POPUP_TR_L_KNIGHT / R_SLAYER / I_SWORD`
  pictures of 7ao** are CH3d's (its kickoff names all three; the three `.tga` + `.TET` pairs are already staged in data215). No dialog or gauge piece is left for CH5.
- **Dev pieces kept (CH1q amendment):** `ElswordWSPCheat` (`X2GameEdit.cpp:773`, `X2Main.cpp:5191`) and `RenderName`'s `WSP: %d` (`X2GUUser.cpp:29563`) have no June counterpart and are shared with Elesis. They stay as dev
  tools (no code change; the WOS play-tests in CH2-CH4 use the cheat); Z lists them in `MODS.md`.
- **Not ported, routed:** the cut-in item override `31fd8a50` -> `CX2CustomizeManager`, Stage 12 bullet (as Aisha / Eve / Chung).
- **Hygiene:** `flagcheck JUNE_CHAR_ELSWORD_UI --worktree --files KTDXLIB/Always.h X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUUser.cpp`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT PASS.
  `junediverge --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUUser.cpp`: RESULT: PASS. `enumscan --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows.
  Committed `935bd9de` (CH2a's hunks in `Always.h` / `X2GUElsword_SwordMan.cpp` left unstaged): `flagcheck JUNE_CHAR_ELSWORD_UI --range 935bd9de^..935bd9de`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT PASS.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,969,152 B, dated 2026-11-04 21:23:07, md5 29e5aa7fa84dc6a534f94a1d695578ad` (the build also carried CH2a's uncommitted edits, as the build lock allows).
  `EXESTR OK: 2 of 2` (`ELSWORD UI cut-in set`, `ELSWORD UI hyper cut-in`, wide), `EXESTR OK: 6 of 6` (the six June `HQ_CutIn_Elsword_*` names), `EXESTR OK: 6 of 6 absent` (the six March `HQ_CutIn_Elsword_{1,2,3,LK_1,RS_1,IS_1}.dds`).
- **See it in game:** select screen: all nine March characters' names and models, then the seven Elsword classes (the lobby pose turns by class: 35 / 25 / 25 / 10 / 3 / 10 / 10; Lord Knight, Rune Slayer, Infinity Sword
  carry their June names). A skill cut-in on each job plays June's `HQ_CutIn_Elsword_*` art (set 0 on 1 / 10 / 11 / 22: `_01`, `_02`, `_03` by cut-in index; 1 on 100, 2 on 101, 3 on 112); an awakening hyper on 100 / 101 /
  112 shows `HQ_Cutin_E{LK,RS,IS}_Transcendence` only when the class script's awakening state sets `SHOW_HYPER_CUT_IN_TIME` (CH1's script has it in three states; the check is CH6's).

#### CH5 - Review (2026-11-05, Opus 5.5): PASS, no code change

I read each item against June in Ghidra: `333ac120` and `32d09050`. InitState `32d23ff0`'s switch targets are outside Ghidra's body, so its jump table was decoded from the exe bytes (`charport_consts.read_va` + capstone).
- **Table `333ac120`.** It builds 12 `SkillCutIn` entries at `337aecc0 + i * 0x30` (`337aecc0` .. `337aeeb0`). Strings in push order: `33525d2c` `HQ_CutIn_Elsword_01`, `5d04` `_02`, `5cdc` `_03`, then `5cac` `_LK_01` x3, `5c7c` `_RS_01` x3, `5c4c` `_IS_01` x3.
  That is the port's table, row for row. Each entry stores size `( [334367ac], [334367ac] )`, position `( [33435598], [334367ac] )` and an uninitialised z: March's one-argument `SkillCutIn( name )` (`X2GUUser.h:830`), the same constants Chung's review read.
- **Reader `32d09050`.** It reads `337aecc0 + ( index + [this+0x8ac8] * 3 ) * 0x30` (set stride 0x90), then the item override `31fd8a50` (not ported, Stage 12), then the 0x8b3 swap `337aec60` / `337aec90`. The port's log line prints after the 0x8b3 swap. That is the name `ActiveSkillShow` gets.
- **Set index (InitState `32d23ff0` @32d2403b).** `class - 1; cmp 0x6f; ja default; movzx byte [32d25850 + class-1]; jmp [32d2583c + idx*4]`. The byte map is 1 / 10 / 11 / 22 -> 0, 100 -> 1, 101 -> 2, 112 -> 3, and every other class -> 4 (the default, which keeps the value).
  The targets set +0x8ac8 = 0 (EBX), 1, 2, 3. This equals `X2GUElsword_SwordMan.cpp:1417-1436` (CH1, `_LIVE`). A NULL unit takes the default branch in the port; June has no NULL check. No index can reach past set 3.
- **UnitTemplet.** I compared block-wise on `unitClass`, with CRLF and BOM normalised only. The current, pre-phase (`kom_backup_tools/data036.kom.20261104_211841`) and June files hold 55 / 55 / 87 blocks; current and pre-phase have the same class set.
  The 7 `UC_ELSWORD_*` blocks match June's text 7 of 7. The other 48 match pre-phase 48 of 48. The file starts with a BOM, with 1465 CRLF and 0 lone LF. `luasrc.py ledger --check`: PASS (0 FAIL, 0 WARN). `core --check`: PASS. `luabind.py check UnitTemplet`: RESULT: PASS (0 unbound).
  `STR_ID_4641` / `4642` (`StringID_def2.h:2148-2149`) and `STR_ID_17425` (`StringID_def7.h:2432`) exist. The three `HQ_Cutin_E{LK,RS,IS}_Transcendence.gfx` are in data195 with the same case as the rows.
- **Dev pieces.** I agree with keeping them. `ElswordWSPCheat` and `RenderName`'s `WSP: %d` have no June counterpart. They are shared with Elesis, the CH2-CH4 WOS play-tests use the cheat, and Z lists them in `MODS.md`.
- **Routing.** `kickoffs/char-elsword/CH3d.md` names `DLG_Transcendental_Change_Guide_Elsowrd.lua`, the `LevelUpEvent.lua` TYPE row and `POPUP_TR_L_KNIGHT` / `_R_SLAYER` / `_I_SWORD`.
- **Play lines** (the 03:51-03:54 run on the deployed exe `b62d02bc`, which carries CH5). There are 13 `ELSWORD UI cut-in set 0 index 0 name HQ_CutIn_Elsword_01` lines, each with its `[JUNE_GFX] CUTIN play ui=0xc HQ_CutIn_Elsword_01.gfx` in the same second.
  Sets 1-3, indices 1 / 2 and the select-screen pass were not reached. They use the same table code and are CH6's walk, as is the hyper cut-in check.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI --range 935bd9de^..935bd9de`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatch, RESULT PASS. No later commit touches the flag (`git log -S`).
  `junediverge.py --check-all`: allow-listed 185, RESULT: PASS. `enumscan --check X2Lib/X2GUElsword_SwordMan.cpp`: 0 A/B rows (the working tree includes a peer's uncommitted edits). There was no rebuild; the deployed exe stays md5 `b62d02bc`.
- **Next.** `kickoffs/char-elsword/CH6.md` has been checked: its CH5 amendment carries the cut-in set walk and the hyper cut-in check. CH6 still waits on the CH3d and CH4c reviews.

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
