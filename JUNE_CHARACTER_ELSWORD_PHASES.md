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
