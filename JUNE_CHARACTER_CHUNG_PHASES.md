# June character port: Chung (Stage 7, second character after the pilot)

Plan key `char-chung` · branch `main` · kickoffs `kickoffs/char-chung/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] CHUNG <sub>`, sub-tags
`DATA`, `CORE`, `CBALL`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_CHUNG_*` for Chung's class code,
`SERV_IRUHADEV_JUNE_CHAR_*` for class-agnostic pieces (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7
(`JUNE_PORT_BACKLOG_ORDER.md`, brief `kickoffs/june-backlog/stage7-plan.md`) · written 2026-10-31 by the `new-plan char-chung` session from
the Aisha plan's "Template for Stage 7" and the Eve plan (`JUNE_CHARACTER_EVE_PHASES.md`), the most recent worked example.

## Context

**What this plan does.** It moves Chung (unit type `UT_CHUNG` 6, script `Chung_Iron_Cannon.lua`, class `CX2GUChung` in `X2Lib/X2GUChung.cpp` /
`.h`, seven classes: Iron Cannon 6, Fury Guardian 20, Shooting Guardian 21, Shelling Guardian 27, Iron Paladin 110, Deadly Chaser 111, Tactical
Trooper 117) from his March kit to his June 2017 kit, end to end: June script, skill states for June's tree, numerical damage, motions,
class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7au), the cannon-ball system, UI pieces, data and his transcendence rows.

**Why Chung is second (stage7-plan, 2026-10-22).** Gap 15 (numerical-only attacks), no templet-backed June-only class, no open decision.
This session confirmed it: March has **no dormant Chung system** (no `ADD_*_2014` counterpart of Eve's Nasod Core; the only commented-out 2014
defines in `Always.h` are Eve's), and `ChangeMaxCharacterAddResource` is already June's (`X2GUChung.cpp:461`, June `32bf4880`, flag
`SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG`). No decision is open (end of this doc).

**His size.** June's class spans `0x32bd86f0`-`0x32c25a1f` (Ara before, Raven after): **441** carved functions, against Eve's 643 (854 once the
uncarved thunks were counted, 802 to transcribe) and Aisha's 426 (560 real). Expect about Aisha's function count, but **more states**: June's
script has 385 `CSI_*` tables (Eve 295, Aisha 313), 117 of them `CSI_SI_*` skill states. Eve took about 167 commits; plan Chung at about 0.8 of
that. The phase table names 20 CH1 sub-phases up front (Eve planned 17 and ran 29).

**First character under the no-`June`-prefix rule (CLAUDE.md, 2026-10-28).** Aisha and Eve wrote inert `June_*` members in CH1a-CH1l and the
`june-march-divergence` plan merged them afterwards. Chung never writes one. June's state names are March's (`CSI_*`), so most June bodies
have a March function of the same name: **the body goes inside that function under `_LIVE`, March's in `#else`**, and each CH1 sub-phase
proves it with a one-file probe compile that defines `_LIVE`. A June function March's `CX2GUChung` lacks is written under `_CORE` with a plain
name in the class's pattern (`CSI_<STATE>_FrameMove`, `Get...`). This moves part of the wiring into CH1a-CH1l, so CH1m-CH1o are smaller.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** Third jobs (`UC_150`-`UC_186`) are out; the s3 tsv has no Chung row above 117.
Transcendence is live (Stage 6 closed 2026-10-24), so his transcendence rows are in CH3d and CH6.

**The state he starts from.**
- **Skill system (Stage 3).** Saves are on June's trees. `s3_reform_ids_no_state.tsv` has 261 Chung rows (6: 15; 20 / 21 / 27: 29 each;
  110 / 111 / 117: 53 each), 127 distinct ids (106, 900, 26000-26064, 26200-26238, 26400-26420, 26600-26605); **254 have no class state**
  (only 900 has one). **49 rows have no June `SKILL_ID` name** (checklist 7ae l.728).
- **Effect engine (June's).** June's `DamageEffect.lua` / `EffectSet.lua` ship merged; 5 Chung rows in `c11a_march_only_templets.tsv` (7u).
- **Motions.** March 17 `Motion_CHUNG*` (data033 16 + data012), June 70 (data204 69 + data012); 10 shipped class motions differ, `Motion_CHUNG.x`
  is already June's. `s2_model_core.tsv`: **CIP drops 2 animation names, CFG drops 1**; `r12b_anim_loss.tsv` names one (`SI_SA_CFG_IRON_HOWLING`
  in `Motion_CHUNG_CFG.x`), the two CIP ones are unnamed. CH0 settles it (the motion question).
- **Cannon balls.** Live in March (`CX2ChungGageData`, `CCannonBallUI` `X2GageManager.h:555`, `CX2ChungMyGageUI` `X2GageUI.h:406`,
  `PGUWO_ORB_CHUNG`, `FIX_CANNON_BALL_SYNC_CALCULATION`). June adds `CANNON_BALL_COUNT_AT_DUNGEON_START` / `_PVP_START` (reader `32bf5430`),
  `COMMAND_HIT_ADD_MP_BONUS` (`32bf4900`, checklist §7 l.246) and the hyper-count-3 timer skip `32bf4680` (l.236). Phase CH1q.
- **Hooks waiting on the class port.** Checklist: 38 rows name Chung (19 sections), 11 sections apply to every class
  (`june2017_re/char_chung/plan_checklist_rows.md`). Chung extras: 7r (cinematic-UI writes, Artillery Strike and June-only Heavy Field), 7ae
  (49 unnamed ids), 7t (berserk-by-chance `32c24b90`), 7n (sync float `32c0ba60`), 7p (hit-stop in DamageReact `32c0cf90`).
- **Summons.** June's script names no NPC. March's class spawns `NUI_TARGETING_CHUNG`, `NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER` (10 sites
  each) and `NUI_REMOTE_MORTAR` (1); June's `Targeting_Chung.lua` (data037) is newer than March's. Eve's summons took three Opus sub-phases
  (CH2b2-CH2b4), so CH0 lists every summon site in June's class and the plan carries an Opus summon phase (CH3b0).

**Out of scope (owned elsewhere).** New characters (Stage 8); UI that is not Chung's own (Stage 9); `DLG_1ST/2ND_Class_Change_Guide_Chung`,
`DLG_UI_JUMPING_EVENT_CHUNG`, `DLG_Unit_Create_ClassChange_Button_Chung` (Stage 9); `Tutorial_Chung_*` / `Tutotial_Chung_Dp*` and the tutorial
dungeons (june-dungeons); `PET_Mystic_CHUNG_3` (Stage 17); NPC / shadow / bot copies (`Npc_Chung_*`, `*_SHADOW_CHUNG*`, `TIME_CHUNG*`,
`PVP_BOT_CHUNG`, `APINK_ANGEL_06_CHUNG`: whichever plan adopted June's NPC Lua, check the ledger); emotion motions' behaviour (Stage 10; CH0b
stages the files); `FORCE_DYNAMIC_CAMERA_INDEX` / `RETAIN_CAMERA_STATE` (Stage 9, as for Eve).

## Rules

The Aisha plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules"`), with Chung's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim
  with a `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in Chung's skill-use check.
- **The checklist is the work list.** Every Chung row in §1-§7au is ported by a phase here or marked n/a by CH0 with the reason. The phase that
  ports a row appends `(Chung: CHn)` to it in place (checklist l.211).
- **One deploy unit.** June's `Chung_Iron_Cannon.lua`, his June motions (held for CH1 if CH0 finds a dropped animation March plays) and the C++
  that reads them go live together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (in `Always.h` as `//#define` until then).
  Between phases Chung stays playable on his March kit.
- **Flags:** `_CORE` (June functions March lacks, compiled but unreferenced), `_LIVE` (June bodies inside March's functions, the registration,
  the switch), then `_CBALL` (cannon balls), `_SKILL`, `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), and a temporary
  `_DEBUG` per sub-phase, retired in its review. New namespaces are `_CONST_CHUNG_<phase>_`.
- **Names (CLAUDE.md, 2026-10-28).** No `June` in any identifier, no Ghidra address or phase tag in a name; the June address goes in the
  one-line comment. `python tools/junediverge.py --list` before transcribing; `--check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h <other files>` =
  `RESULT: PASS` at every phase's finish. A March function June code calls whose June namesake differs is replaced at its definition
  (`tools/data/june_march_divergent.tsv` row in the same commit).
- **Shared `CX2GUUser` helpers, never copied into the class:** skill level `GetSkillLevel` / `GetLearnedSkillAbilityValue` (June `31fceab0` /
  `32007b10`, flag `SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`); skill use `UserNoStateChangeActionFrameMove`, `MarkSkillSlotUsed`,
  `ReserveSkillUse`, `StateChangeBySkillSlot`, `SkillMemoMpRefund`, `CheckSkillUseNotPve`, `CheckSkillUseNotDungeon` and the fields
  `m_iChosenSkillID`, `m_iChosenSkillSlotIndex`, `m_fReservedSkillMp`, `m_fChosenSkillMp`, `m_abSkillSlotUsed`, `m_abHyperSkillSlotUsed`
  (flag `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`). Aisha's and Eve's CX2GUUser bases (dash-jump rates, slot key-held helpers, the
  user virtuals of `X2GUUser.h` under `_ARME_LIVE` / `_EVE_LIVE`) are reused; a Chung phase that needs one under its own flag widens the
  `#if` instead of copying it.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id with an Enum name is written by name, registered under a
  `SERV_IRUHADEV_` flag with June's value if C++ lacks it; `enumscan.py --check X2Lib/X2GUChung.cpp` stays 0. **The 49 unnamed rows of 7ae
  stay raw ids with a one-line comment giving the templet's skill name**: the checklist's "name them from the templet" predates the rule, and a
  name June's `Enum.lua` does not have is an invented name (CH0 checks each id against `Enum.src.lua` and the live `Enum.lua` first).
- **Shared Lua per character.** `UnitTemplet.lua` is core-list BOM source: CH5 replaces only Chung's 7 rows. His class script and skill NPC
  scripts are adopted whole (`adopt-june`, bytecode unless a phase must edit one); every one passes `python tools/luabind.py check <NAME>`.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack yourself, ask before every copy.
- **CP949.** `X2GUChung.cpp` (pure CRLF), `X2GUChung.h` (mixed line endings), `X2GUUser.*`, `X2GageUI.*`, `X2GageManager.*` are CP949:
  `bytepatch.py` only; `cl /P` decides every `#ifdef` branch (`CHUNG_SECOND_CLASS_CHANGE`, `SERV_ADD_CHUNG_SHELLING_GUARDIAN`,
  `SERV_CHUNG_TACTICAL_TROOPER`, `BALANCE_DEADLY_CHASER_20130214`, `UPGRADE_SKILL_SYSTEM_2013`, `SKILL_CANCEL_BY_HYPER_MODE`, the balance-patch
  pairs).
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** No schema change expected. A phase that needs one snapshots `db_backup/` first. Play-test saves never slot a skill in slot 4 / 9
  before transcendence.
- **One code phase at a time in `X2GUChung.*`** (CH1-CH4). CH5 may run beside CH2-CH4.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "<heading>"` |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Chung's rows with line numbers: `june2017_re/char_chung/plan_checklist_rows.md` |
| Planning evidence | `june2017_re/char_chung/plan_*` (below); CH0 writes `ch0_*` beside them |
| Eve's tools (take a character or copy with the range changed) | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (`CLASS_RANGE` has `eve` only: CH0 adds `chung`), `charport_regsim.py`, `s3_no_state.py`; `char_eve/ch0_build_*.py`, `ch0_unmapped_callees.py`, `ch0_live_lines.py`, `ch1m_regsim.py` / `ch1m_regcheck.py`; `char_arme/ch0c_listing_check.py` |
| June Lua (reading) | `june_src_fixed/data036_kom/Chung_Iron_Cannon.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`; `data037_kom/Targeting_Chung.src.lua` |
| Ghidra | `ghidra` skill; Chung's range `0x32bd86f0-0x32c25a1f`, vtable `3350967c` (544 slots, 72 overrides in range), ctor `32bf1040`, dtor body `32c0c4e0`; read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (slow), `python charport_keys.py Chung_Iron_Cannon` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Chung_Iron_Cannon.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR CHUNG"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-10-31)

Kept in `june2017_re/char_chung/` (digests written by Sonnet subagents, spot-checked: the ctor writes vtable `3350967c` at `32bf1094`, and
`32c1f530` decompiles to a 118-case skill switch; CH0 re-verifies anything it builds on): `plan_charport_scan_20261031.txt`,
`plan_charport_keys.txt` / `.tsv`, `plan_march_side.md`, `plan_june_side.md`, `plan_numerical_rows.tsv` (145 rows), `plan_checklist_rows.md`.

Scans re-run 2026-10-31: `census.py` 26 PASS, 0 FAIL, 0 `to-implement` rows (implemented 651, june-ignores 305, handed-off 10; regenerated
`census.tsv` reverted); `charport_scan.py` (scratch `C:\tmp\cps`) Chung lines identical to the 2026-10-25 run:
`Chung_Iron_Cannon {'no NUMERICAL_DATA_INDEX': 74, 'no DAMAGE table': 15}`, no Chung ENUM or KEYS line (the only diff is nine field-boss / loading
scripts no longer skipped).

| Measure | Chung | (Eve) |
|---|---|---|
| June class range | `32bd86f0`-`32c25a1f`, 441 carved functions, 72 vtable overrides in range | 643 carved, 854 in range |
| June skill-tree rows | 261 over 7 classes; 127 distinct ids; 49 rows unnamed | 261 / 127 |
| Rows with no class state | **254** | 254 |
| Rows by June skill type | 6: 95, 14: 47, 15: 41, 16: 35, 8: 27, 12: 7, 13 (hyper): 6, 1: 3 | |
| June script | `Chung_Iron_Cannon.src.lua` 19,376 lines, data only: 385 `CSI_*` + 7 `INIT_*` + `PROMOTION_ITEM_ID_LIST` + 2 `XXX_CSI_CFG_*` (groups SI 117, SA 28, RIDING 27, CSG 21, DAMAGE 21, A 21, COMBO 20, CTT 18, CIP 16, CFG 14, CDC 13, CHG 13, STANDUP 8) | 295 |
| Script keys | `DAMAGE_DATA` 79 (**15 numerical-only**, `plan_june_side.md` §3), `AT_COMMAND` 53, `APPLIED_SKILL_ID` 24, `TIME_STOP_IMMUNE_STATE` 14, `SHOW_HYPER_CUT_IN_TIME` 3, `SHOW_SKILL_CUT_IN_TIME` 15, `CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE` 1 | 28 / 3 |
| Key census (`charport_keys.py Chung_Iron_Cannon`) | 249 keys: june-only **11** / neither 8 / march-only-site 7 / both 223. June-only: `CANNON_BALL_COUNT_AT_DUNGEON_START` / `_PVP_START` (reader `32bf5430`), `COMMAND_HIT_ADD_MP_BONUS` (`32bf4900`), `IMMUNE_TO_SUPER_ARMOR_DOWN` x11 (`31fdaaa0`), `SIEGE_ANGLE` / `MOBILE_SIEGE_ANGLE` (no reader found), `IMMUNITY_EDT_LIST_AT_THIS_STATE` x46 (no reader, as Eve), the two camera keys (Stage 9), `BBT_CHANGE_NOW_HP_PER_SECOND` / `BFT_TIME` (damage data). Neither: `EXTRA_CANNON_BALL_DAMAGE_RATE`, `USE_CANNON_BALL0`, `NOMAL_CAMERA0-2_RATE`, `HEAD_IK`(`_MAX_ANGLE`), `ATTACK_ACTION_TYPE` | 239: 16 / 1 / 8 / 214 |
| Numerical blocks | 145 `c1a` rows keyed to 58 Chung ids, all `ok` | 209 |
| Motions | June 70 / March 17; 10 class motions differ; CIP -2, CFG -1 animation names (one named: Iron Howling) | Eve: none lost |
| Alpha-tag sites (7c) | **0** (`c10a_alpha_sites.tsv`) | 167 |
| March class | `X2GUChung.cpp` 24,461 lines, `.h` 1,946; cut-in table `:63-120`, ctor `:131`, InitState `:779` (cut-in switch `:781-838`), InitStateCommon `:890`, `SetEquippedSkillFuncBySkillSlotIndex` `:3184-4831` (79 `case SI_*`), InitComponent `:5181`, SpecialAttackEventProcess `:5815`; own 4+4 pressed-index decode `:5923`, `:6064-6081`, encode `:6087` / `:6090` (7ag) | 34,813 |
| June class | `SetEquippedSkillFuncBySkillSlotIndex` `32c1f530` (vslot 0x7b4), InitState `32c22540` (0x7a4), InitComponent `32bf4900`, `SetDamageExceptionProcess` `32bf28f0` (**June-only**, 7b), SpecialAttackEventProcess `32bd8790`, InitPassiveSkillState `32c16be0`, berserk-by-chance `32c24b90` (vslot 0xa4, 7t), hyper effect start `32c063d0` (7ao), `MemoConditionChung` (RTTI `3373d650`) | |
| Stat rows | `StatTable.lua` 1-99 for 6 / 20 / 21 / 27 / 117; **110 and 111 list levels 81-99 twice** (118 rows each; CH0 item 9) | all 99 |
| Save | units 18 Freils (117 lv99), 42 RapidT (111 lv70), 43 NovaT (110 lv70), 50 Chung99 (111 lv99), all transcendence 0; none of 6 / 20 / 21 / 27 | |
| Cut-ins | `X2GUChung.cpp:63-120`, 7 sets (Paladin / Chaser / Trooper `DDS_Chung_*`, the rest `HQ_CutIn_Chung_*`); `g4` names `HQ_CutIn_Chung_Gura / _Rage / _normal` have no June `.gfx`; June's `UnitTemplet` adds `m_BerserkCutIn` / `m_HyperCutIn` (`HQ_Cutin_CIP/CDC/CTT[_1]_Transcendence`) | |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Chung map: class map, state / skill table, checklist rows, assets + effect closure, keys, registration listing check, unmapped helpers, summon sites, cannon-ball map, the motion question, the 49 unnamed ids, stat duplicates | Opus high | - |
| CH0b | Every June Chung asset, whole, first | Sonnet high, user play-test | CH0 |
| CH1a-CH1l4 | Class core transcription by state group and vslot band (15 sub-phases, below) | Sonnet high, Opus review each | CH0; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals; 7ag + hooks + June-only keys) | Opus high, Opus review each | CH1a-CH1l4 reviews |
| CH1q | Cannon balls: June's cannon-ball system and gauge, flag `_CBALL` | Opus high, Opus review | CH1p review |
| CH1 | Final: define `_LIVE`, adopt the script, play-test | Opus high, Opus review, user play-test | CH1q review (and CH0b) |
| CH2a-CH2d | Skills: Iron Cannon + first jobs (6, 20, 21, 27) | Sonnet high, Opus review, user play-test | CH1 review |
| CH3b0 | Chung's summoned NPCs (targeting NPC, remote mortar): spawn rule, `Targeting_Chung.lua`, binder check; CH0 moves it before the first skill phase that spawns one | Opus high, Opus review, user play-test | CH1 review |
| CH3a-CH3d | Skills: second jobs (110, 111, 117), hyper actives, transcendence rows | Sonnet high, Opus review, user play-test | CH2 reviews |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4c | Passives; damage exception + overrides; memo and item readers | Opus high, Opus review, user play-test | CH2 reviews |
| CH5 | UI and data: Chung's `UnitTemplet` rows, cut-in table | Sonnet high, Opus review, user play-test | CH1 review; beside CH2-CH4 |
| CH-N | Not run: Chung has no templet-backed June-only class | - | - |
| CH6 | Whole-character walk + gap reruns | Sonnet medium, user walk | CH3, CH4, CH5 reviews |
| Z | Close-out | Sonnet medium | CH6 |

CH0 runs now (read-only, alongside anything). CH0b follows it. CH1-CH4 all edit `X2GUChung.*`: one at a time. CH5 runs beside CH2-CH4.
If CH0's counts put a group over ~45 functions, CH0 splits it with a digit suffix (`CH1g2`) and writes the kickoff and STATUS row; it does not
merge groups. Play-test follow-ups found after CH1 final (Eve's CH1r / CH1s) get the next letters.

## Phase CH0 - Chung map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_chung/`.

**Step 0 (the pilot's recipe fixes).** Build the registration map with `charport_states.py init <InitState 32c22540> <InitStateCommon> <base>`
and `skill 32c1f530` (find InitStateCommon from vtable `3350967c`; Eve's call was `init 329b8030 329ba9c0 3202b110`), then run
`ch0c_listing_check.py` on it: every row must match the raw listing; all `~` rows and std::string-named rows (`USI_HYPER_MODE`, START / WIN /
LOSE roll) by hand. Eve's builders take a character argument or are copied into `char_chung/` with Chung's range: say which. Add `chung` to
`charport_adis.py`'s `CLASS_RANGE`.

**Scope** (Eve's CH0 items, with Chung's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in `0x32bd86f0-0x32c25a1f` (vtable overrides, registrars, state callbacks incl. uncarved
   thunks, helpers, dead), March pair, verdict, **target sub-phase from the table below**, and a column **`inside` / `new`**: `inside` = a March
   `CX2GUChung` function of the same role exists (the June body goes inside it under `_LIVE`), `new` = March lacks it (plain name under
   `_CORE`; propose the name). Funcmap rows `Char Chung CH0`. Settle the boundary functions (`32c25880`-`32c25a1f`; Raven's first override
   `32c25a20` is not carved).
2. **State and skill table** `ch0_state_skill.tsv`: 395 / 395 script tables, 127 / 127 skill ids; dead-data and March-only states listed.
   Chung's last March state is already 255 (checklist 7l; June ids are 16-bit, widened by june-effect C3d-2): confirm nothing in
   `X2GUChung.*` still stores a state id in a `UCHAR`.
3. **Keys** `ch0_keys.tsv` (`charport_keys.py Chung_Iron_Cannon`): each of the 11 June-only keys becomes a CH1p / CH1q / skill item or gets an
   owner; find readers for `SIEGE_ANGLE`, `MOBILE_SIEGE_ANGLE`, `IMMUNITY_EDT_LIST_AT_THIS_STATE`, and check the 8 `neither` keys
   (`EXTRA_CANNON_BALL_DAMAGE_RATE`, `USE_CANNON_BALL0` are cannon-ball keys: CH1q).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7au row -> phase or n/a with reason (start from `plan_checklist_rows.md`; l.254 is
   not Chung's). 7r row 1 (Artillery Strike, `32bff3d0`) and 7ac (`@32bfe8c3`) go to the CDC skill phase; 7r row 2 and 7n (`32c0ba60`, Heavy
   Field) to CTT; 7u's grenade memo (`X2BuffTemplet.cpp:1695-1706`) and l.278 Metabolism Boost to CH4c.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Chung items, unioned; `ch0_build_assets.py`) and
   **effect closure** `ch0_effect_closure.tsv` (`charport_closure.py`, run-time-built names included). **The motion question:** name the two
   animations `Motion_CHUNG_CIP.x` drops and the one `Motion_CHUNG_CFG.x` drops (`SI_SA_CFG_IRON_HOWLING` per `r12b`), and whether March's
   script or `X2GUChung.cpp` plays them (or June moved them to another file, as Eve's Atomic Blaster). If March plays one, CH0b holds the motion
   koms for CH1 final (checklist 7ad) and its kickoff says so. Stage the class's `POPUP_TR_*` guide textures (names in
   `DLG_Transcendental_Change_Guide_Chung.lua`) and the cut-in `.gfx` CH5 needs in the same list.
6. **Unmapped helpers.** For every skill, list the callees outside the class range that funcmap does not map (`ch0_unmapped_callees.py`). Each
   gets an Opus `CH<n>0` sub-phase before the Sonnet phase that needs it (kickoff + STATUS row written by CH0).
7. **Summon sites.** Every `CreateNPC`-style call in June's class (and its NPC id), March's `NUI_TARGETING_CHUNG*` / `NUI_REMOTE_MORTAR` sites,
   June's `Targeting_Chung.lua` vs March's (is it already June's in data037: check the ledger), `luabind.py check Targeting_Chung` and every other
   summoned NPC's script. Place CH3b0 before the first skill phase that spawns one; split it if there is more than one spawn rule.
8. **Cannon-ball map** `ch0_cannonball.tsv`: every March cannon-ball function (class, `CX2ChungGageData`, `CCannonBallUI`, `CX2ChungMyGageUI`,
   `X2GageManager.cpp`, `FIX_CANNON_BALL_SYNC_CALCULATION` sites; `cl /P` says which branch is live) paired with its June function or "absent in
   June", plus June-only pieces (`32bf5430`, `32bf4680`, the `DLG_Chung_Cannonball_NEW` dialog if June reads it). Sized for CH1q; split if over
   ~45 functions.
9. **Shared data.** Which `RNSI_*` / `CSI_*` names the C++ enums need (raw id + `/// RNSI_` comment, as Eve); **the 49 unnamed rows of 7ae**:
   look each id up in June's `Enum.src.lua` and the live `Enum.lua` (expect none; then raw id + comment, Rules); Chung's 7 `UnitTemplet` rows June
   vs March (`plan_june_side.md` §7: `m_fLobbyRotationY`, the two transcendence cut-in keys, the Tactical Trooper name `STR_ID_17435`: is the
   string in our string table?); **the StatTable duplicates** for 110 / 111 levels 81-99: equal or not, and which row `X2OfflineStatTable` keeps.
   If they differ, route it (server-Lua rule: the user packs) rather than deciding here.
10. **S3 census baseline:** `s3_no_state.py --family X2GUChung.cpp --preprocess` (254 expected) so CH6 compares like with like.
11. **Amend the kickoffs** CH0b-CH5 ("Amended by CH0" sections: filters with real column numbers, addresses, counts, any split).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Chung checklist row has a phase or n/a;
395 / 395 tables and 127 / 127 ids mapped; assets union routes a / b / c; the motion question answered; the cannon-ball and summon maps exist;
kickoffs amended; open questions listed.

### CH0 - Done (2026-10-31)

Read-only, no flag, no build. Outputs in `june2017_re/char_chung/`. Builders copied from Eve's with Chung's range (`ch0_build_state_skill.py`,
`ch0_build_class_map.py`, `ch0_build_vtable.py`, `ch0_march_live_states.py`, `ch0_unmapped_callees.py`) plus new `ch0_skill_listing_emu.py` and
`ch0_amend_kickoffs.py`; `char_eve/ch0_build_assets.py` gained a `chung` entry (Eve's run still reproduces); `charport_adis.py` has `chung`.

**Step 0.** InitStateCommon is two functions: `charport_states.py init 32c12a30 32c1d1d0 3202b110` -> `ch0_reg.tsv`; `ch0c_listing_check.py`
reproduces **188 / 188** distinct rows. InitState `32c22540` is **not carved in Ghidra**, so its 4 registrations (START / WIN / LOSE roll, `CSI_WAIT_STATE_CHANGE`)
were read from the listing by hand (rows `@<call>`), as were the `USI_HYPER_MODE` std::string row and the 9 `~` rows (class gate *(+0x867c)+0x38 in `32c1d1d0`,
fallthroughs 110 -> 20, 117 -> 27). `skill 32c1f530` -> `ch0_skill.tsv`; the listing emulator then corrected **16 of 27** address-order guesses (callbacks belong
to the END / FINAL / LANDING state registered through the shared tail `32c2219e`) and filled 15 tables the decompile names only through a variable.

**Maps**
- `ch0_class_map.tsv`: 1,396 starts in `32bd86f0-32c25a1f` (442 carved, 954 uncarved): 557 state-fn, 558 thunks, 72 overrides, 71 helpers, 2 registrars,
  127 dead (no reference in the exe; spot-checked), 9 not-Chung (incl. the ctor `32bf1040`, whose caller is outside: it is Chung's, CH1k). **702 to transcribe: 593 `inside`, 109 `new`**
  (names in `ch0_hand_names.tsv`; 24 stay `?` with a hint, the transcribing phase names them by the class pattern). Boundary: `32c25880` slot-0 dtor, `32c258b0`-`32c25a00` thunks, `32c25a10` not Chung's.
- `ch0_state_skill.tsv`: **395 / 395 tables** (30 dead data, 8 non-state, 0 unassigned), **127 / 127 ids** (22 passives CH4a, 900 = `CSI_THROW_ITEM`).
  3 March-only states dropped (`CSI_SI_A_CDC_LEG_SHOT`, `..._SIEGE_SHELLING_READY_HYPER`, `..._ARTILLERY_STRIKE_CANCEL`); 6 names registered with no table
  (`CSI_RIDING_DAMAGE_BACK`, 5 base `USI_EVENT_*`); `CSI_ATTACK_DASH_X` and `CSI_A_CDC_RAPID_CANNON_READY` defined twice in June's script (JUNE_BUGS rows in CH1d2 / CH3b).
- `ch0_vtable_all.tsv` (544 slots) / `ch0_vtable_slots.tsv` (72 overrides; every slot the plan named confirmed). `ch0_checklist_rows.tsv`: 123 rows, every one a phase or n/a (46 n/a).
- `ch0_keys.tsv` (249): readers found for `SIEGE_ANGLE` / `MOBILE_SIEGE_ANGLE` / `EXTRA_CANNON_BALL_DAMAGE_RATE` (InitComponent `32bf4900`) and `USE_CANNON_BALL%d`
  (CommonStateStart `32bf3990`): the census had the last two as `neither`. `HEAD_IK*`, `NOMAL_CAMERA*`, `ATTACK_ACTION_TYPE` are in no June reader. Immunity keys -> Stage 2 (`31fdaaa0`, CX2GUUser).
- Assets: `ch0_assets.tsv` 13,947 rows (routes a / b / c), **`ch0b_stage_list.tsv` 5,093** (4,788 missing, 305 March copies), dry run 35 koms, 0 refused, new kom `data204`.
  `ch0_effect_closure.tsv` 7,263 rows, 1,945 names, 64 missing. `skillicons.py check` FAILs until `DLG_SKILL_ChungActive_04 / _05` and `DLG_Chung_SKILL_ScreenShot_04` are copied (in the list).
  `POPUP_TR_I_PALADIN / D_CHASER / T_TROOPER` staged.
- **The motion question:** three animations are lost, and March's script plays all three. `SI_A_CIP_SUDDEN_BURSTER` and `SI_SA_CIP_BURST_WOLF` moved CIP -> June `Motion_CHUNG_CFG.x`;
  `SI_SA_CFG_IRON_HOWLING` is in no June file, yet June's own `_HYPER` state plays it (JUNE_BUGS row, CH3a2). **CH0b holds the 10 differing class motions for CH1 final** (7ad).
- Summons (`ch0_summons.tsv`): 4 June spawn sites (`32bfe730` x2, `32c0b7f0`, `32c10d20`), entry points `31f8b010` / `322e67c0`; June-only NPC **2525 `NUI_TARGETING_CHUNG_HEAVY_FIELD`**
  (no NPCTemplet row, no enum here). `Targeting_Chung` / `Remote_Mortar` already June's content; `luabind.py check` PASS. **CH3b0 moves before CH2c** (Quantum Ballista spawns first).
- Cannon balls (`ch0_cannonball.tsv`, 196 rows): gauge +0xc04 equals March's `CX2ChungGageData`; CH1q owns the counter API (11 helpers via `ch0_hand_phases.tsv`, 15 functions) and the UI outside the class.
- Item 9: the "49 unnamed" are 7 ids x 7 classes (26027-26029, 26216-26219), named in neither Enum (`ch0_unnamed_ids.tsv`): raw id + comment. `ch0_enum_names.tsv`: 10 / 127 in the C++ enum by name,
  60 in the core Enum.lua. UnitTemplet (`ch0_unittemplet_chung.tsv`): June adds `m_fLobbyRotationY` and 110 / 111 / 117 cut-in keys; `STR_ID_17435` exists ("Tactical Trooper").
  StatTable: no duplicate (the 19 extra rows per 110 / 111 are commented copies, equal).
- S3 baseline (`ch0_s3_baseline.txt`): 261 rows, 254 no_state. Item 2: state ids are 16-bit already (`SERV_IRUHADEV_JUNE_USER_STATE_ID16`); the `UCHAR` sites in `X2GUUser.h` are its dead `#else`.
- Item 6: 294 out-of-class callees, 115 unmapped; `ch0_unmapped_identified.tsv`: 68 confirmed, 33 candidate, 14 unknown; 26 need Opus.

**Splits and new phases** (functions to transcribe): CH1d 37 + **CH1d2** 21; CH1g 20 + **CH1g2** 30; CH3a 17 + **CH3a2** 34. Opus helper phases **CH1d0, CH1j0, CH1k0, CH1l0, CH1l30, CH2b0**
(the other needs-Opus callees go to CH1m / CH1n / CH4a, already Opus). Others: CH1a 29, CH1b 27, CH1c 2 (riding is CX2GUUser code), CH1e 12, CH1f 27, CH1h 22, CH1i 15, CH1j 41, CH1k 38,
CH1l 16, CH1l2 9, CH1l3 13, CH1l4 6, CH1m 3, CH1n 2, CH1o 24, CH1q 15, CH2a 41, CH2b 39, CH2c 43, CH2d 42, CH3b 38, CH3c 27, CH3d 5, CH4a 4, CH4b 3.
Every kickoff CH0b-CH6 has its "Amended by CH0" section (filters: class map col 7, state table col 8); 9 new kickoffs; STATUS rows added and Depends rewired.
**Corrections to the plan:** Siege Shelling (26201) and Back Blast are class 6 (CH2a, not CH2c); Head Shot is class core (CH1g2, its id 26220 is a `_TRANS` id); 7ai L835 is CH2c; 7al: Chung overrides only slot 0x800.

**Drift** (scope rule): CustomizeManager (`31d70b60`) is Stage 12's (CH1l30 writes a stand-in); immunity keys stay on the Stage 2 bullet; June's `AddChungCannonBall_Cheat` dev commands noted in CH1q.
Nothing needs a new plan. `funcmap.tsv`: 110 rows `Char Chung CH0`.
**Open:** the doubtful cannon-ball pairings and Chung's InitWakeOrb variant (CH1q kickoff); 24 `?` helper names. Next: CH0b (Sonnet) and CH1a may start; they may run side by side.

## Phase CH0b - Every June Chung asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (then
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_ASSET`). Sub-tag `DATA`.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv`: motions (June's are in `data204`, not in the game dir yet: a new kom, as Eve's `data203`;
`X2Main.cpp` mounts `data001-226`), body / face / hair, equipment, textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`. Stage byte-exact
(`kom.py stage-june`), pack each kom, ask once with the whole kom list, copy what the user approves. Save `data\ErrorLog_pre_CH0b_chung.txt`
first. Create the walk units the save lacks (classes 6, 20, 21, 27; level 99, class costume) with `createchar.py`, after a `db_backup`.
**If CH0 found a dropped animation March plays**, stage and pack here but copy the motion koms together with CH1 final.

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Chung per job tier, village and dungeon, no new `KEM_ERROR` /
`Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

### CH0b - Done (2026-10-31)

Sonnet 5.5. No C++, no flag (CH0 found no March reader a June asset breaks). Assets and the save only.
- **Reference:** `ErrorLog.txt` (263,310 B, last run 2026-10-31 08:56) saved as `data\ErrorLog_pre_CH0b_chung.txt` before any copy.
- **Staged:** `kom.py stage-june --jobs 8 june2017_re/char_chung/ch0b_stage_list.tsv`: `STAGE-JUNE OK: 35 kom(s), 5093 staged (4789 new, 304 replaced, 0 identical), 0 refused, 31s`
  (the dry run matched CH0's numbers). Replaced March files are backed up under `24965799\kom_backup_stage_june\20261031_110724`.
- **Packed:** `kom.py pack` x35, 35 `PACK OK`, 0 refusals (no Lua staged).
- **Copied (user approved the 33, 2026-10-31):** `kom.py copy` x33, 33 `COPY OK`: data002 013 016 017 019 020 021 044 052 063 067 069 107 109 117 130 148 151 159 185
  191 192 198 204 215 222 226 datai006 007 019 020 023 031. `data204` is a new archive (June's 53 emotion motions); `X2Main.cpp` mounts `data001-226`.
- **Held back, packed in `kom_contents`:**
  - **data033** (`~10 changed` = `Motion_CHUNG_CDC / CFG / CHG / CIP / Common / CSG / CTT / Emotion / Field / Lobby .x`): CH1 final copies it with `_LIVE`
    (March's script plays the three animations June dropped, `ch0_motion_question.tsv`).
  - **data101**: `kom.py status` shows `+2 added, -1070 removed, ~747 changed`. The 1,070 are March entries never restored into `kom_contents`
    (june-asset-port P0 step 2; a copy would delete them) and the +2 / ~747 come from other sessions' staging. CH0b's only row there is
    `Velder_Gate_Stage2_Wall_BlueTile01.dds` (a Velder tile, not Chung's). It ships with P0's restore. Not touched by `--allow-removed`.
- **Done-gate re-run:** `kom.py stage-june --dry-run` over the same list: `5093 staged (0 new, 0 replaced, 5093 identical), 0 refused`. 0 `missing` / `march-copy`.
- **Skill icons:** `skillicons.py check 106 900 26000-26064 26200-26238 26400-26420 26600-26605`: 133 skills, **2 misses** (was 22), `RESULT: FAIL`.
  The two are 26406 `SI_P_CSG_REMODEL_MAGAZINE` and 26413 `SI_P_CTT_CANNONEER_WITHSTANDING`: their skill-tree picture names a piece of
  `DLG_Chung_SKILL_ScreenShot_04` that June's own `.TET` (identical bytes, 10 pieces) does not have. June's gap, kept: `JUNE_BUGS.md` row 488.
  Their icons are fine. Nothing to stage.
- **Save:** backup `db_backup\els_db_20261031_111102_pre_manual.sql`; added with `createchar.py` (lv 99, class costume + top gear +10): unit 62 ChungIron
  (Iron Cannon 6), 63 ChungFury (Fury Guardian 20), 64 ChungShoot (Shooting Guardian 21), 65 ChungShell (Shelling Guardian 27). Existing: 18 Freils (117),
  42 RapidT (111, lv 70), 43 NovaT (110, lv 70), 50 Chung99 (111).
- **Play-test (user) is the gate.** Log tag `[JUNE_CHAR]` has no lines (no code); the check is `ErrorLog.txt` against `ErrorLog_pre_CH0b_chung.txt`
  (`KEM_ERROR`, `Motion Error`, missing texture / mesh). The class script and motions are still March's, so Chung plays as before.
- **Logs checked (user play-test passed, 2026-10-31, run 11:19-11:22):** `KEM_ERROR` 0 and `Motion Error` 0 (pre run 0 / 0); `offline_packets.log` 0 EXCEPTION. 142 failed names the pre run lacks, none a regression: ~120 are `.XET` probes whose `.X` / `.Y` is in `kom_contents` (the engine's fallback; the pre run had 37 `_SORTED.XET`), ~80 of them Ruben village; loose `eventscene` probes (`PC_CHUNG_PNG.SWF`, `QUEST_11000_CHUNG_END`; the files exist loose, the pre run had 11); `MESH_CHUNG_BASIC_FACE_ANGRY5.TGA` and `RUBEN_TECHHOUSE_GANPAN.DDS` are in no June kom (`ch0_assets` `dangling`, June's own); `DLG_UI_GUILD_02.TGA` is June's guild UI (Stage 9), not Chung's.
- **Drift:** none new. The data101 restore is june-asset-port P0's.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high, Opus review | Movement, guard and slot-head states: wait, walk, jump (4), dash (4), charge MP, throw item, power attack, guard (3) with the guard-state list key, common buff, hyper |
| CH1b | Sonnet high, Opus review | Reactions: damage (21 `CSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement |
| CH1c | Sonnet high, Opus review | Riding (27 `CSI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high, Opus review | Iron Cannon ground combos (20 `CSI_COMBO*`, 4 `CSI_ATTACK*`) |
| CH1e | Sonnet high, Opus review | Fury Guardian line (14 `CSI_CFG*` + the 2 `XXX_CSI_CFG_*` dead tables) |
| CH1f | Sonnet high, Opus review | Iron Paladin line (16 `CSI_CIP*`) |
| CH1g | Sonnet high, Opus review | Shooting Guardian line (21 `CSI_CSG*`) |
| CH1h | Sonnet high, Opus review | Deadly Chaser line (13 `CSI_CDC*`) |
| CH1i | Sonnet high, Opus review | Shelling Guardian line (13 `CSI_CHG*`) |
| CH1j | Sonnet high, Opus review | Tactical Trooper line (18 `CSI_CTT*`) |
| CH1k | Sonnet high, Opus review | Overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32c0cf90` with the 7p hit-stop; ctor / dtor offset map) |
| CH1l | Sonnet high, Opus review | Overrides 0x600-0x7bc |
| CH1l2 | Sonnet high, Opus review | Common* overrides 0x7c0-0x7ec except CommonFrameMove |
| CH1l3 | Sonnet high, Opus review | Skill-use overrides from 0x7f0 and their helpers (the seven `CX2GUUser` skill-use helpers are called, never copied) |
| CH1l4 | Sonnet high, Opus review | CommonFrameMove 0x7cc and the helpers only it reaches |
| CH1m | Opus high, Opus review | Registration: June's `InitState` `32c22540` / `InitStateCommon`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), START / WIN / LOSE roll, base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The CX2GUUser base callbacks Chung registers; `SetEquippedSkillFuncBySkillSlotIndex` `32c1f530` head (bounds, slot tables, the 7i rule) + the dev free-skill gate |
| CH1n2 | Opus high, Opus review | (split from CH1n) The non-riding `CX2GUUser` base callbacks whose June body differs from March's (`ch1n_user_callback_triage.tsv`, 16 rows), every class, own flag |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`; June-only and changed user virtuals Chung overrides (7ap 0x6a8 `32bdba60`, 7t 0xa4 `IsBerserkByChance` `32c24b90` with `+0x9714`, 0x9c / 0xa0 `32c24b50` / `32c24b60`); gaps the CH1a-CH1l4 reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (Chung's own 4+4 decode `:5923`, `:6064-6081` and the encoders `:6087` / `:6090`, every reader, together); hooks 7h / 7l / 7o / 7p / 7x; June-only script keys (`COMMAND_HIT_ADD_MP_BONUS`, `IMMUNE_TO_SUPER_ARMOR_DOWN`, `SIEGE_ANGLE` if CH0 finds a reader) |
| CH1q | Opus high, Opus review | Cannon balls (`ch0_cannonball.tsv`): June's bodies inside March's cannon-ball functions, `CANNON_BALL_COUNT_AT_DUNGEON_START` / `_PVP_START` (`32bf5430`), the hyper-count-3 timer skip `32bf4680`, the gauge UI and orb; flag `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL`, sub-tag `CBALL` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE`; adopt `Chung_Iron_Cannon.lua` and any summon script CH0 marks CH1; pack, ask, copy; the play-test |

**Transcription rules (CH1a-CH1l4):** each June function whole. **`inside` rows** (CH0's column): June's body inside March's function under
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March's body in `#else`, `#endif SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`; the phase compiles `X2GUChung.cpp` once
with `_LIVE` defined (one-file probe, `cl /c` with `/D`) and pastes the result. **`new` rows:** a plain name in the class's pattern under
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, compiled in every build. A class-range callee owned by a later group is declared with
`// June <va>: body in <phase>`; an unmapped engine callee or unexplained mechanism is "this needs Opus" (stop). June's numbers and quirks
verbatim; class code multiplies `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai). `junediverge.py --check` on the class files = PASS.

**Done gate (CH1 final):** Chung (any class) loads June's script and motions; `[JUNE_CHAR] CHUNG DATA` logs the load; no new `KEM_ERROR` /
`Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, guard, damage, bounding and riding play June's animations;
cannon balls fill, spend and show on the gauge as June; `charport_scan.py` / `charport_keys.py Chung_Iron_Cannon` show nothing new.

### CH1a - Done (2026-10-31)

All 29 class-map rows of phase CH1a are written: 28 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`) and 1 `new`
(`JumpUpStart`, June `32bdbcd0`, under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`; CH0 proposed `CSI_JUMP_UP_StateStart`, renamed to the class pattern: **CH1m registers `JumpUpStart`**).
Both flags are in `Always.h`: `_CORE` defined, `_LIVE` as `//#define` with nested `#error` guards (needs `_CORE`, `JUNE_ENUM_NAMES`, `JUNE_DIVERGE_SKILL_LEVEL_VALUE`).
`RNSI_P_CIP_OPEN_MIND = 26415` registered in `X2SkillTree.h` under `JUNE_ENUM_NAMES` (June `Enum.lua`). 29 funcmap rows `Char Chung CH1a`; Chung's offset table is `june2017_re/char_chung/ch1_offsets.tsv` (seeded from Eve's).

| June | Function (`CX2GUChung::`) | June | Function |
|---|---|---|---|
| `32bdca80` `32bdcac0` `32bdcaf0` `32bf5de0` | WaitStartFuture / WaitStart / WaitFrameMoveFuture / WaitEventProcess | `32bf60e0` `32beb1b0` | WalkEventProcess / JumpReadyEventProcess |
| `32bdbcd0` (new) `32bef2d0` `32bef450` `32bf62f0` | JumpUpStart / JumpUpEventProcess / JumpDownEventProcess / JumpLandingEventProcess | `32bf65e0` | DashEventProcess |
| `32beb260` `32bdcb10` `32bf67e0` | DashEndStartFuture / DashEndFrameMove / DashEndEventProcess | `32bd8930` `32bdcb50` `32bd8960` `32bef6f0` `32bf6ae0` | DashJumpStartFuture / Start / EndFuture / EventProcess / DashJumpLandingEventProcess |
| `32bd8980` `32bd89b0` `32bf6d30` | PowerLandingStartFuture / PowerDownLandingStart / PowerDownLandingEventProcess | `32bd8a50` `32bf2b50` | GuardStart / GuardEventProcess |
| `32bdcfc0` `32c03880` | GuardAttackZEventProcess / FrameMove | `32bd8a60` `32bf7250` `32bdd1d0` | GuardAttackXInit / FrameMove / EventProcess |

**Where June differs from March** (everything else is March's logic, written out whole):
- **Wait**: the cannon-ball charge test needs no emotion playing at all (`m_ePlayedEmotion == ET_NONE`, +0x89d8); March only refuses SITWAIT / SITREADY.
- **Down-force latch**: June has a second byte at +0x8bf9 next to `m_bDownForce` (+0x8bf8). New member `m_bDownForcePressedInState` (CX2GUChung, `_CORE`, ctor false): a Down press in JUMP_UP / DASH_JUMP sets both; without a press `m_bDownForce` is cleared each frame unless the latch is set; `JumpUpStart` and `DashJumpStart` reset it. JUMP_DOWN's Z / X air attacks clear `m_bDownForce` before the state change. June keeps the byte in the CX2GUUser region; it stays on the Chung class until another class needs it.
- **Guard / Guard X**: June inlines `CanUseCannon` with a third clause, skill level of `RNSI_P_CIP_OPEN_MIND` (26415) >= 1 (March: full hyper mode or balls left); Guard X frame move calls `UseCannon( 1 )` only below that level (`UseCannon` body `32bf5280` is CH1q's). Guard X event process: walk and dash cancels after 0.7 also require `IsOnSomethingFuture()`.
- **DashJump** event process: the both-keys branch tests `oneRight` / `oneLeft` against the facing before the jump-state clamp.
- Identical to March after the macros expand (checked against the listing): Walk, JumpReady, JumpLanding, Dash, DashEnd (3), DashJumpStartFuture / EndFuture / Landing, PowerLanding (3), Guard start, Guard Z (2), Guard X init.
- State ids are written by name; the `CSI_*` numbers differ from June's raw ids by design (script tables bind names through `MakeTableReference`); every name used compiles. `0xb9` / `0xbd` / `0x11e` resolved through `ch0_state_skill.tsv` (`CSI_COMBO_DASH_JUMP_Z`, `CSI_ATTACK_DASH_JUMP_X`, `CSI_SI_A_CHG_CHAOS_CANNON_WAIT`).
- Checklist §7 L253 (`+0x7cc8` power rate): no CH1a function touches it. No `JUNE_BUGS` row: nothing here looks like a June bug.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files ...`: `RESULT: PASS` (0 deleted lines, 0 non-ASCII inserted, 0 `#else`/`#endif` without the token). `_LIVE` reports FAIL only because its `#define` is commented out
(as the kickoff says); `git diff --patience` shows 903 inserted lines, 0 deleted in `X2GUChung.cpp`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`cl /Zs`, US_SERVICE defines, no PCH, Boost / DXSDK include dirs from the vcxproj): `X2GUChung.cpp` with `_LIVE` off and with `/D SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: 0 errors each.
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 11:47:15, md5 f7acf6cde643e4eba96325b552df9230`, equal to `X2/US_SERVICE/x2.exe`; built at main `3db0b96a` + the 4 code files of this phase.

**Nothing to play-test** (`_LIVE` is off; nothing registers `JumpUpStart`). Next: `/x2-phase char-chung CH1a review` (Opus 5.5 high), then CH1b.

### CH1a - Review (2026-10-31, Opus 5.5)

**Verdict: pass, no code change.** All 29 functions were read against June's annotated listings (`charport_adis.py chung <va>...`), not
only the three suggested. Branch order, `StateChange` ids, the x87 compare directions and every constant match.
- **The six differences hold.** (1) Wait: `[+0x89d8] == 0` is `ET_NONE == GetNowEmotionId()`; vslot 0x9c is `IsFullHyperMode` (Chung `32c24b50` =
  gage data +0x140, `GetBerserkMode()`). (2) Latch: JumpUp sets both bytes in one `word 0x101` store, JumpDown's oneDown sets only +0x8bf8, and
  JumpDown's Z / X clear `m_bDownForce` before `StateChange`. (3) Guard: X takes `GUARD_ATTACK_Z` only when not full hyper, count `setg` 0 is false
  and `31fceab0( 26415 ) <= 0` (`GetSkillLevelValue` is the `replace` row, hence `_LIVE`'s `#error` on `JUNE_DIVERGE_SKILL_LEVEL_VALUE`). (4) Guard X
  frame move: the same gate, then `UseCannon( 1 )` only at level <= 0. (5) Guard X event process: walk (to `USI_WAIT`) and dash after 0.7 re-test
  `bFootOnLine`. (6) DashJump: the both-keys branch, the `GetJumpSpeed()` clamp and `0 < nowSpeed.y` match.
- **Spot checks end to end:** DashEnd `32bf67e0` (`now > ( max - transition ) * 0.3`, the oneRight / oneLeft-with-facing dash order),
  DashJump `32bef6f0` (event flag 0 re-arm at `nowSpeed.y <= 0`, ending at the `ret` @32befa7f; the code after it is the next function),
  PowerDownLanding `32bf6d30` (0.06 / 0.13333 / max/6; Z only for Iron Paladin, a nested time test).
- **`JumpUpStart`** stays the name. CH1m's kickoff now says to register it, and the class map's `proposed_name` reads `JumpUpStart`.
- **+0x8bf9 stays on `CX2GUChung`.** A census of June's `.text` finds 4 readers or writers, all Chung's (`32bdbcd0` / `32bdcb50` / `32bef2d0` /
  `32bef6f0`). The other two byte hits are misaligned decodes (inside `cmp ecx, 0x8b`). +0x8bf8 is used by 57 functions, the only word-wide
  store being Chung's JumpUp, so no other class sets the latch.
- **Hygiene.** `flagcheck JUNE_CHAR_CHUNG_CORE --range db34715a^..db34715a`: `RESULT: PASS` (1 PASS, 3 OTHER: `_LIVE` / Always.h /
  `JUNE_ENUM_NAMES`; 0 deleted). `_LIVE` range: FAIL only because its `#define` is still commented out (by design). `git diff --patience` and
  `--histogram`: 903 / 0 in `X2GUChung.cpp` (the default diff's -22 is alignment). `junediverge.py --check-all`: `RESULT: PASS` (158
  allow-listed). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows. `cl /Zs X2GUChung.cpp /DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
- **No build.** No client file changed. The deployed exe is still CH1a's (`X2_offline.exe` md5 `f7acf6cde643e4eba96325b552df9230` = `X2/US_SERVICE/x2.exe`).
- **Tool and data fixes (no client code).** `charport_adis.py --phase` now finds the `phase` column by its header (Chung's map has it in
  column 7, so the old index printed nothing for every Chung phase). `char_chung/ch1_offsets.tsv` was a byte copy of Eve's: her 19 class
  fields (0x95bc-0x9c74, e.g. +0x96b4 labelled `m_sEGQueensThrone`) are replaced by 12 Chung fields and 2 vslots read here (+0xc04 gage data,
  +0x89d8, +0x8bf9, +0x8d4c, +0x9624 / +0x9628 cannon-ball wait timer, +0x96b4 / +0x96c8 / +0x96d0, +0x971c, +0x9740, +0x9794; vslots 0x9c, 0x1cc).
- No `JUNE_BUGS` row and no drift.

**Nothing to play-test.** Next: `/x2-phase char-chung CH1b` (Sonnet 5.5 high).

### CH1b - Done (2026-10-31)

All 27 class-map rows of phase CH1b are written: 26 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`) and 1 `new`
(`EraseTargetingNPC`, June `32c04140`, CH0's name, under `_CORE`, declared in `X2GUChung.h`). 27 funcmap rows `Char Chung CH1b`. No new `Always.h` define (`_CORE` / `_LIVE` are CH1a's; `_LIVE` stays `//#define`).
`NUI_TARGETING_CHUNG_HEAVY_FIELD = 2525` registered in `X2UnitManager.h` under `JUNE_ENUM_NAMES` (June `Enum.src.lua` line 4030); 863 and 1419 already had March names.

| June | `CX2GUChung::` | June | `CX2GUChung::` |
|---|---|---|---|
| `32bdc940` `32c0dd90` `32bd88f0` `32bdc9a0` | DieFront StartFuture / Start / FrameMove / EventProcess (also `CSI_RIDING_DIE`) | `32bdc9e0` `32c0de40` `32bd8910` `32bdca40` | DieBack StartFuture / Start / FrameMove / EventProcess |
| `32c04140` (new) | EraseTargetingNPC | `32bdcba0` `32bdcc40` | DamageSmall / DamageBig EventProcess |
| `32bdcd90` `32bdcde0` `32bedb80` `32bedd30` `32bedee0` | DamageDownFront / Back FrameMove, DamageDownFront / Back / AirDownLanding EventProcess | `32bdce20` `32bd89d0` `32bdcea0` `32bd8a10` | StandUpRolling EventProcess / End, StandUpAttack EventProcess / End |
| `32c02ee0` `32c033b0` | StandUpAttackFront / Back FrameMove | `32bd8e00` `32bf8c50` `32bdf400` `32bd8e50` `32bd8e90` | StandUpCannonAttack Init / FrameMove / EventProcess / End / EndFuture |

**Where June differs from March** (everything else is March's logic written out whole; checked against the listings):
- **Die Start (both):** June calls `EraseTargetingNPC( id, 3.0 )` for 863 `NUI_TARGETING_CHUNG`, 1419 `_NOT_SHOW_OTHER_PLAYER`, 2525 `_HEAVY_FIELD` with no `CHUNG_SECOND_CLASS_CHANGE` guard; March self-destructs the first two after 3 s with a countdown particle. June's helper walks the units, keeps NPCs of the id with hp > 0 whose ally AI follows this unit, and kills them with `SetNowHp( 0 )` (NPC vslot `0xf4`); the 3.0 is never read. The other June callers (`32c08010` / `32c0c4e0` / `32c0ca60`) belong to later phases, which keep March's `SetSelfDestructSummonedNPC` until then.
- **Damage Small / Big:** June tests only `GetGameUnitStateFuture() == USI_DAMAGE_SMALL_FRONT` (`_BIG_FRONT`) and `SpecialAttackEventProcess( ASUT_GROUND )`; March also needs `SMI_CHUNG_MEMO12` equipped and a non-PvP game (`ADDITIONAL_MEMO`). The rest, incl. the 200-unit gap, the 0.93 key-cancel of Big and the `-45` y shift, is March.
- **StandUpCannonAttack EventProcess:** the walk and dash cancels after 0.833 also require `IsOnSomethingFuture()` (March's macros do not, same as CH1a Guard X); the skill and awakening cancels are the `SKILL_CANCEL_AFTER` expansion; June reads the alternate future animation (`+0x8e90 ? +0x8e84 : m_pXSkinAnimFuture`, `32070cf0`) as `m_pXSkinAnimFuture` (as Aisha, CH1d).
- **= March after the macros expand** (March's active `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch is `EventTimerOneshot`, June's `t > +0x24c && EventTimer`): the four Die functions that are not Start, DamageDown (all five), StandUpRolling / Attack (EventProcess, End), StandUpAttackFront / Back FrameMove (0.58 / 0.60 and 0.45 / 0.47 Land Demolisher sets), StandUpCannonAttack Init / FrameMove / End / EndFuture. June inlines `CanUseCannon` (not full hyper -> balls > 0), the same as March's.
- **Not in CH1b's functions:** the states whose callbacks are `CX2GUUser` virtuals (Damage Groggy, Standup front / back, the three `BOUNDING_*`, Air Small / Down / Down Invincible / Fall / Up / Fly, Revenge, Wall Landing, Amusement Phoenix Run End) have no class-range function. The three bounding callbacks are thunks to `31fd0230` / `31fd0260` / `31fd02d0`; CH1m registers them, CH1n owns the base bodies. Checklist §7 L253: no CH1b function touches `+0x7cc8`.
- No `JUNE_BUGS` row: nothing here looks like a June bug. `m_fLandDemolisherPowerRate` is used as is.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2UnitManager.h`: `RESULT: PASS` (1 PASS, 2 OTHER: `_LIVE` / `JUNE_ENUM_NAMES`; 0 deleted, 0 non-ASCII, 0 EOL mismatch, 0 `#else`/`#endif` without the token); `_LIVE` reports FAIL only because its `#define` is commented out. `git diff --patience --numstat`: 586 / 0 in `X2GUChung.cpp`, 3 / 0 `X2GUChung.h`, 6 / 0 `X2UnitManager.h`.
`junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2UnitManager.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,865,216 B, dated 2026-10-31 15:37:45, md5 1510294a014ae442e8899798907d2a5e`, equal to `X2/US_SERVICE/x2.exe`; built at main `2e0aa199` + the 3 code files of this phase (X2Lib build 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states or calls `EraseTargetingNPC`). Next: `/x2-phase char-chung CH1b review` (Opus 5.5 high), then CH1c.

### CH1b - Review (2026-10-31, Opus 5.5)

**Verdict: pass after one structural fix.** I read all 27 functions against June's listings (`charport_adis.py chung --phase CH1b`). Branch
order, `StateChange` ids, x87 compare directions and constants match. The transcription had no logic errors.
- **Fix: `32c04140` is June's `SetSelfDestructSummonedNPC`, not a new function.** It has March's signature (`( id, fDeleteTime )`, `ret 8`), the
  same loop (GUT_NPC, templet id, hp > 0, ally AI owned by this UID, switch on the id) and March's callers: destructor `32c0c4e0`, ReInit `32c0ca60`,
  artillery reset `32c08010`, Die Start `32c0dd90` / `32c0de40`. Under the no-copy rule (CLAUDE.md, 2026-10-28) June's body now sits inside
  March's `CX2GUChung::SetSelfDestructSummonedNPC` under `_LIVE` (March in `#else`). `EraseTargetingNPC` and its `_CORE` declaration are
  gone (`X2GUChung.h` is byte-equal to `03ec44d1^` again), and both Die Starts call `SetSelfDestructSummonedNPC( id, 3.f )`. June's version
  drops March's `GetSelfDestruction()` skip, `StartSelfDestruction( fDeleteTime )` and the `CountDown3` particle, kills with `SetNowHp( 0 )`
  (NPC vslot 0xf4) and adds 2525. `june_march_divergent.tsv` row `CX2GUChung::SetSelfDestructSummonedNPC` (replace, ported under `_LIVE`).
  The class map, `ch0_hand_names.tsv` and funcmap rows `32c04140` / `32c0dd90` now carry the name.
- **The other callers follow June when `_LIVE` turns on** (March's destructor, ReInit, CommonFrameMove, ARTILLERY_STRIKE_READY and
  ResetArtilleryStrikeData sites). This is the rule's "unported callers follow June", and their own June bodies land in CH1k / CH1l / CH1l4. The
  `0x333bc990( 0x33722a70 )` call in each loop pass belongs to June's `g_pX2Game` handle fetch (it also appears in `32c033b0`), not to the logic.
  `GetNowHp()` returns 1 when there is no gage data, which matches June's skip of the hp test.
- **Damage Small / Big** (`32bdcba0` / `32bdcc40`): the state test is `GetGameUnitStateFuture()` (vslot 0x4f4) == 0x1a / 0x1c, then
  `SpecialAttackEventProcess( 1 )`. There is no memo or PvP test, the gap is `200 < gap`, the shift is `y -= 45.0`, and Big's skip-point keys are exactly
  `ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC` (14 keys, then `0.93 < t`).
- **StandUpCannonAttack EventProcess** (`32bdf400`): anim end -> `JUMP_DOWN` (+ y shift) / `WAIT`. After 0.833 come the skill cancel with
  `bFootOnLine`, then the hyper branch in `_BUFF_AWAKE_LOCK`'s `SKILL_CANCEL_AFTER` order (black hole, oneLCtr, count > 0, `CanHyperMode`
  vslot 0x560, time, quick-slot wait, `m_bHyperModeStartedByInput`), then walk -> `WAIT` and dash -> `StateChangeDashIfPossible`, both on
  `bFootOnLine`, then `ELSE_IF_ARROW_ASDC_PRESSED_AFTER` (no Z / X).
- **Spot checks end to end:** `32bedb80` (+0x7d86..+0x7d89 = `bEventFlagList[1..4]`; the anim-end order, the oneZ / oneX word stores and the
  facing-mirrored rolling flags all match); `32bf8c50` (state from `(+0x7cbc - +0x7cec) / 100` = `GetNowStateID()`, BIG when `rand <= prob`,
  `UseCannon( 1 )` after the switch, default included); `32c033b0` (oneshot `+0x24c < t`, `m_bApplyLandDemolisher`, -120 / +120 by facing,
  `m_fLandDemolisherPowerRate`, `true`, target, rotate degree).
- **Bounding states:** there are 5 callbacks, not 3 (DOWN EP, LANDCRASH EP, UP StartFuture / Start / EP: thunks to vslots 0x5d4-0x5e4). Chung
  overrides none of them, and the bodies are already `CX2GUUser`'s (C3d-3, `SERV_IRUHADEV_JUNE_BOUNDING_STATE`), so CH1n has nothing to write.
  The CH1m kickoff now says `InitBoundingStates( "CSI_" )`.
- **Hygiene.** `flagcheck JUNE_CHAR_CHUNG_CORE --range 03ec44d1^..03ec44d1`: `RESULT: PASS` (1 PASS, 2 OTHER, 0 deleted). Worktree
  (`--files X2GUChung.cpp X2GUChung.h`): `RESULT: PASS`. Its 3 deleted lines are CH1b's own `EraseTargetingNPC` declaration; against `03ec44d1^`
  the .cpp is 582 / 0 and the .h has no change. `_LIVE` reports FAIL only because its `#define` is still commented out (by design). `junediverge.py
  --check-all`: `RESULT: PASS` (158 allow-listed); `--check X2GUChung.cpp X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
  One-file probes (`census.cl_cmd`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors
  (which also proves the two-argument `BALANCE_PATCH_20131107` signature is the live one).
- `DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 15:58:37, md5 8077f072334beba92f4bb7525d4338f9`, equal to `X2/US_SERVICE/x2.exe`; built at main `03ec44d1` + the 2 code files of this review. No `exestr` line: `_LIVE` is off and the review adds no string to the flag-off exe.
- No `JUNE_BUGS` row and no drift.

**Nothing to play-test.** Next: `/x2-phase char-chung CH1c` (Sonnet 5.5 high).

### CH1c - Done (2026-10-31)

Both class-map rows of phase CH1c are written, both `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`). No new `Always.h` define (`_CORE` / `_LIVE` are CH1a's; `_LIVE` stays `//#define`). 2 funcmap rows `Char Chung CH1c`.

| June | `CX2GUChung::` | Notes |
|---|---|---|
| `32c09b60` | CommonHyperModeFrameMove | March's hyper-mode body with June's changes below |
| `32c113e0` | RidingHyperModeFrameMove | `CommonHyperModeFrameMove( 0, 0.11, 0.1, true )` = March |

**Where June differs from March** (the timers are March's active `EventTimerOneshot`; June's `t > +0x24c && EventTimer` is the same test):
- **No voice line.** March plays `ChungVoice_BoostPower.ogg` at the third timer (full hyper only) and in the second timer's branch when the two times are equal. June has neither: its third `else if` keeps the timer test with an empty body (JUNE_BUGS 490, kept as an empty branch).
- **Awakening effect-set family.** At the second timer June calls `32011d20`, then `31fd66b0` (present?) and `32011f50` (play) before the Shock particle, and skips the Trace particle when the set is present. They are `CX2GUUser` members March lacks (the unit's awakening set name, +0x9578); stand-ins in `_CONST_CHUNG_CH1c_` (`ChungAwakeningEffectBegin` / `SetPresent` / `Play`: nothing plays, present = false, June's empty-map default), the same shape as Aisha's and Eve's. Order: Begin, Play, Shock, `UpDownCrashCamera( 30, 0.4 )`, `ClearScreen`, Trace (if no set), `ApplyHyperModeBuff` (vslot 0xc0), then Energy / Break / Change when `bSound_`.
- **Same as March:** the world colour (0.1333 x3, 1.0 = `0xff222222`), the first timer (`ShowMinorParticleHyperModeChange` only), the inlined `ClearScreen`, the `CommonFrameMove` tail (vslot 0x7cc). The prologue's `call [344c0044]` is the compiler artefact Aisha's `32ce12c0` also has; not ported.
- Checklist §7 L253 (`+0x7cc8` power rate): neither function touches it. 7ai (BFI arguments): none.

**The user-side check.** CH0 expected the riding / pepper-run callbacks to be Aisha's or Eve's ports to widen. They are not: every riding, pepper-run and hyper-end target (`31fc....`-`3203....`, `329cc....`) is a `CX2GUUser` member whose March body no `_ARME_` / `_EVE_` flag touches (X2GUUser.cpp:30858 `PEPPER_RUN_*`, :44839 `Riding*`); `RidingHyperModeEnd` (vslot 0x608) is `virtual` under `_ARME_CORE` for every class, with the Eve-owned `SERV_IRUHADEV_JUNE_DIVERGE_RIDING_HYPER_MODE_END` body. So nothing is widened and nothing is copied. Chung overrides none of vslots `0x58c`-`0x5b4` and `0x610`-`0x624` (`ch0_vtable_slots.tsv`). Eight states have no March callbacks (`CSI_AMUSEMENT_ITEM_PHOENIX_RUN`, `CSI_DANCE_Z/X/WAIT`, `CSI_RIDING_SPECIFIC`, `_ATTACK_X_READY(_2)`, `_END`): owners PvP / Stage 10 / Stage 17 (`JUNE_PORT_BACKLOG_ORDER.md`); the CH1m kickoff now says to leave them out as Eve did. **JUNE_BUGS 489:** `CSI_RIDING_DAMAGE_BACK` has no script table and `CSI_RIDING_DAMAGE_FRONT` is defined twice (CH0's candidate; Chung's twin of 379 / 422).

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --worktree --files X2Lib/X2GUChung.cpp`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 FAIL`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out (by design). `git diff --patience --numstat`: 54 / 0 in `X2GUChung.cpp`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 16:08:03, md5 9c67ab96686d36e54704e755d7eb1e8a`, equal to `X2/US_SERVICE/x2.exe`; built at main `5504e2ec` + the 1 code file of this phase. No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

No drift. **Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1c review` (Opus 5.5 high), then CH1d0.

### CH1c - Review (2026-10-31, Opus 5.5)

**Verdict: pass, one comment fixed.** I read both functions against June's listings (`charport_adis.py chung --phase CH1c`).
- **`32c09b60` CommonHyperModeFrameMove.** Each timer is `fld [anim+0x24c]; fld fTimeN; fcom; test ah,0x41; jne skip` followed by `EventTimer`: run when
  `fTimeN > +0x24c`. That is March's `EventTimerOneshot` (`KTDGXSkinAnim.h:272`, `m_fAnimOneshotTimer < fTime && EventTimer( fTime )`). A failed test
  or a failed `EventTimer` falls through to the next timer, so the `else if` chain is right. The world colour is three `0.13333334` and one `fld1` into a
  `D3DXCOLOR`, stored at `+0xb4` / `+0xc4` with `+0xd4 = 0`. That is March's inline `SetWorldColor` (`m_NowColor`, `m_FadeColor`, `m_fFadeTime`) with
  `0xff222222` (0x22 / 255). The inlined `ClearScreen` writes DGManager `+0xa8 = 1` and four `1.0` values at `+0xac`, which is March's
  `ClearScreen( 1, 0xffffffff )` default. The t1 branch builds `L"Dummy1_Rhand"` for `ShowMinorParticleHyperModeChange`, which is March's default argument.
  The t2 order (Begin, present? -> Play, Shock `L"Dummy1_Rhand"`, `UpDownCrashCamera( 30, 0.4 )`, ClearScreen, Trace only when not present, vslot 0xc0
  `ApplyHyperModeBuff`, then Energy / Break / Change as `PlaySound( name, true, false )` when `bSound_`) matches the port. The t3 branch is `EventTimer` and
  nothing else (JUNE_BUGS 490). The tail is vslot 0x7cc. `call [344c0044]` is the static guard, not ported. Chung's June body never calls `31febf50` (the
  Dark Dragon hyper-mode item set that Eve's calls), so there is no stand-in for it here.
- **`32c113e0` RidingHyperModeFrameMove**: `( 0, 0.11, 0.1, true )` = March.
- **Fix:** the stand-in comment named "Stage 6" and an "empty-map default". The Stage 6 planning moved the family to **Stage 12**
  (`JUNE_PORT_BACKLOG_ORDER.md` L363; funcmap 4588 / 4589: `+0x9578` is the set-name wstring). The line now reads `( Stage 12 ), June's empty-name default`.
- **Stand-ins:** Aisha (`_CONST_ARME_CH1b_` `AwakeningEffect*`, plus the `HyperModeBuffEffectStart` reader at `X2GUArme_VioletMage.cpp:29404`), Eve
  (`EveAwakeningEffect*`) and Chung (`ChungAwakeningEffect*`) have the same shape. June's three are `CX2GUUser` members, so whichever phase ports them should
  replace every class copy with one `CX2GUUser` trio. That is Stage 12, not Stage 6: Stage 6 ports only `Effectset_TRANScendence_WakeUp_FX`. The L363
  bullet now says so.
- **User-side check confirmed.** `ch0_vtable_slots.tsv` has no Chung override in `0x58c`-`0x5b4` or `0x610`-`0x624`. `PEPPER_RUN_READY_EventProcess`
  (`X2GUUser.cpp:30858`) and `RidingStartStartFuture` (:44839) are March `CX2GUUser` bodies. `RidingHyperModeEnd` carries
  `SERV_IRUHADEV_JUNE_DIVERGE_RIDING_HYPER_MODE_END` (:47473). The CH1m kickoff's "Amended by CH1c" leaves out the 8 states with owners
  (PvP / Stage 10 / Stage 17) and registers `CSI_RIDING_DAMAGE_BACK` as June does (JUNE_BUGS 489).
- **CH1d0 lead:** `+0x24c` is `m_fAnimOneshotTimer`, so `31fd4290` / `31fd4340` look like March's `CX2GUUser::ClearAnimEventTimerOneshotNow` / `Future`
  (`X2GUUser.cpp:48871` / `:48879`, `ResetOneshotPerformed` on the unit's and the riding pet's anim). The kickoff now says so. CH1d0 confirms the offsets.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --range 94b7fdc9^..94b7fdc9`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 FAIL`, `0 deleted line(s), 0
non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `--worktree --files
X2Lib/X2GUChung.cpp`: `1 PASS`, with 1 deleted line (the replaced comment, inside the flag). Both runs are RESULT FAIL only because the `#define` is commented out.
`junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows. One-file probes (`census.cl_cmd` /
`cl_env`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors.
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 16:14:29, md5 ee90d3201b67ed8e296be4d5eb46c5b7, equal to X2/US_SERVICE/x2.exe (2026-10-31
16:14:22); built at main 94b7fdc9 + 1 uncommitted code file(s).` Nothing to play-test. Next: `/x2-phase char-chung CH1d0` (Opus 5.5 high).

## CH1d0 - Done (2026-10-31)

**Result: no code change.** Both helpers are March functions; CH1d calls them by name.

| June | March | June's body |
|---|---|---|
| `31fd4290` | `CX2GUUser::ClearAnimEventTimerOneshotNow()` (`X2GUUser.cpp:48871`) | `+0x24c` (`m_fAnimOneshotTimer`) = 0 on `+0x87d4` (`m_pXSkinAnim`), on `+0x8e88`, and on the riding pet (`+0x683c`) now anim `+0x238` (getter `32071a10`) |
| `31fd4340` | `CX2GUUser::ClearAnimEventTimerOneshotFuture()` (`:48879`) | the same on `+0x87d0` (`m_pXSkinAnimFuture`), `+0x8e84`, and the pet's future anim `+0x23c` (`32071a30`) |

- **Now vs Future:** `+0x87d4` is the now anim (funcmap rows `32071d80` C3r / R12b), `+0x87d0` the future one (`32070cf0`); March's `CX2RidingPet`
  declares `m_pXSkinAnim` before `m_pXSkinAnimFuture` (`X2RidingPet.h:537-538`), matching `+0x238` / `+0x23c`. The kickoff's lead was right.
- **The one difference:** June also clears `+0x8e88` / `+0x8e84`, the second skin-anim pair that `InitMotion` loads only for unit type 10 (funcmap R12,
  row `31fe5860`): Lu/Ciel's dual body. For Chung both are NULL, so March's body does exactly what June's does. Recorded as two `out` rows in
  `tools/data/june_march_divergent.tsv` (same treatment as `AnimPlay` / `AnimStop`), and appended to backlog Stage 8's dual-body bullet
  (`JUNE_PORT_BACKLOG_ORDER.md`). No `JUNE_BUGS` row: nothing quirky.
- **Callers (Ghidra xrefs):** `31fd4290` from Chung `32bf1000` `32bf75b0` `32bf7cd0` `32bfb6e0` `32bfc310` `32bfd2b0` `32c11e10` `32c12660` `32c07cba` and
  user `32022380` / `32057b80`; `31fd4340` from Chung `32beba58` and user `32022380` / `320506f0`. March calls Now 13 times, Future 3 times.
- funcmap rows `31fd4290` / `31fd4340` (`Char Chung CH1d0`); `kickoffs/char-chung/CH1d.md` "Amended by CH1d0" names the March symbols.

**Gates.** No client source changed, so no flag, flagcheck, enumscan or build applies (last deploy: CH1c review, md5 ee90d320).
`junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`: `RESULT: PASS` (allow-listed 4); `--list` reads the two new rows. Nothing to play-test.
Next: `/x2-phase char-chung CH1d0 review` (Opus 5.5 high).

## CH1d0 - Review (2026-10-31)

**Verdict: pass, nothing changed.** The CH1d0 note's mapping, offsets and `out` verdict hold.
- **Bodies (Ghidra).** `31fd4290`: `FLDZ` / `FST` to `+0x24c` on `+0x87d4` and on `+0x8e88` (each NULL-tested), then, if the riding pet `+0x683c` is set,
  the smart-pointer getter `32071a10` (returns `+0x238`, refcount `+8` up / down through `31d28b60`) and the same store on it. `31fd4340` is the same on
  `+0x87d0`, `+0x8e84` and getter `32071a30` (`+0x23c`). March `X2GUUser.cpp:48872-48886` does `ResetOneshotPerformed()` (`m_fAnimOneshotTimer = 0.f`,
  `KTDGXSkinAnim.h:298`) on `m_pXSkinAnim` / `m_pXSkinAnimFuture` and `m_ptrRidingPet->GetXSkinAnimPtr()` / `GetXSkinAnimFuturePtr()`
  (`X2RidingPet.h:424` / `:426`, members `:537` / `:538` in that order = `+0x238` / `+0x23c`). Live: `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` is defined (`KTDX.h:342`).
- **Second pair NULL for Chung.** `InitMotion` `31fe5860` writes `+0x8e80` / `+0x8e84` / `+0x8e88` only inside `if ( unit type == 10 )` (`@31fe5f2x`, unit
  templet `+0x867c` -> `+0xaf0`); June `Enum.src.lua` `UNIT_TYPE`: `UT_LUCIEL = 10`, `UT_CHUNG = 6`. The `out` rows (divergent.tsv l.390-391) stand; no
  `replace` row or body is owed.
- **Caller spot-check.** `32bf75b0` = March `CX2GUChung::ComboZZZXCannonBallShotFrameMove` (`X2GUChung.cpp:10678`): one-shot 0.001 event, play count
  `+0x248` < 5, cannon balls > 1 or full hyper (vslot `0x9c`), `EffectSet_CSI_COMBO_ZZZX_CANNON_BALL_SHOT`, `UseCannon( 1 )`, `TRACE_START0` / `STR_Kwa`,
  then on anim end (`+0x2ac`) `31fd4290`, where March calls `ClearAnimEventTimerOneshotNow()` (`:10704`), then `CommonFrameMove` (vslot `0x7cc`).
- **Callers.** Ghidra xrefs equal the note's lists (11 to `31fd4290`, 3 to `31fd4340`); March has 13 / 3 call sites, matching the tsv rows.
- **Bookkeeping.** `kickoffs/char-chung/CH1d.md` l.62-63 names both March symbols; backlog Stage 8 dual-body bullet (`JUNE_PORT_BACKLOG_ORDER.md`
  l.416-420) carries the second-body reset.

**Gates.** `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). No source change, so no flagcheck or build (last deploy: CH1c review, md5 ee90d320).
Nothing to play-test. Next: `/x2-phase char-chung CH1d` (Sonnet 5.5 high).

### CH1d - Done (2026-10-31)

All 37 class-map rows of phase CH1d are written, all `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`). No `new` row, no new `Always.h` define (`_LIVE` stays `//#define`). 38 funcmap rows `Char Chung CH1d` (the 37 + `32beaf10`). JUNE_BUGS 491.

| Group | June -> `CX2GUChung::` |
|---|---|
| Init (preload, `= March`) | `32bd8ab0` ZZZXCannonBallShotInit, `32bd8b20` ZZZXFinishInit, `32bd8b90` XXInit, `32bd8bc0` XXXInit, `32bd8c10` XXZXCannonBallShotInit, `32bd8c60` XXZXFinishInit |
| StartFuture (`= March`) | `32bd8b00` ZZZXCannonBallShot, `32bd8b70` ZZZXFinish |
| Z line EventProcess | `32bdd420` Z, `32bdd5e0` ZZ, `32bdd7e0` ZZZ, `32bdda60` ZZZZ, `32beb2a0` ZZZXReady, `32beb330` ZZZXCannonBallShot, `32bddc20` ZZZXFinish, `32bdddf0` ZZdownZ, `32bddf30` ZZdownZZ |
| X line EventProcess | `32bde0f0` X, `32bde240` XX, `32bde470` XXX, `32beb450` XXZ (also `CSI_CHG_COMBO_XZ`), `32beb5b0` XXZXCannonBallShot, `32bde640` XXZXFinish |
| FrameMove | `32c00ec0` Z, `32c01070` ZZ, `32c01220` ZZZ, `32c014d0` ZZZZ, `32c03b30` ZZdownZ, `32c01780` ZZdownZZ, `32c01930` X, `32c1bad0` XX, `32bf75b0` ZZZXCannonBallShot, `32bf77c0` ZZZXFinish, `32bf7ae0` XXX, `32bf7cd0` XXZXCannonBallShot, `32bf7ef0` XXZXFinish |
| Helper | `32bf5360` ShowOnomatopoeiaParticle (`X2GUChung.h:601`) |

**Where June differs from March** (the other 28 are March's logic written out whole; the `SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER` / `ELSE_IF_ARROW_ASDC_PRESSED_AFTER` macros already expand to June's code, `+0x24c` < t && `EventTimer` = March's `EventTimerOneshot`, June reads the alternate future animation `+0x8e90 ? +0x8e84 : +0x87d0` as `m_pXSkinAnimFuture`):
- **ZZ `32bdd5e0`:** the Z Z X flag [3] (`CSI_CFG_COMBO_ZZX`) is set for Iron Paladin (110) only; March also takes Fury Guardian (20). Deadly Chaser (111) -> [4], Tactical Trooper (117) -> [5] with its 0.83 window first (all as March). **JUNE_BUGS 491.**
- **ZZZ `32bdd7e0`:** the walk cancel after 0.5 is the opposite-direction one (`CAN_WALK_CANCEL`, March: any direction) and both it and the dash cancel need `IsOnSomethingFuture`; `IF_SKILL_CANCEL_AFTER( 0.5 )` leads; no `IsAnimationEnd` in the cancel chain.
- **ZZZXFinish `32bddc20`, XXZXFinish `32bde640`:** the any-direction walk cancel and the dash cancel also need `IsOnSomethingFuture` (March's `WALK_CANCEL_AFTER` / `DASH_CANCEL_AFTER` do not), written out as CH1a did for Guard X.
- **XX `32bde240`:** a branch March lacks. Flag `bEventFlagList[2]` (`+0x7d87`): while it is set, once t > `m_fEventTime[0].stateChange` or the animation ends, `StateChange( CSI_CFG_COMBO_XXFRONTZ )`; it is set when `m_fEventTime[0].keyInputStart <= t < keyInputEnd`, job line 1 (`0x670( 1 )`: classes 20 / 110), `oneZ` and a forward direction key; both paths end in `CommonEventProcess()` at once. `oneZ` sets the X X Z flag only while [2] is off. `m_fEventTime[0]` (`+0x8c10` / `14` / `18`) is CX2GUUser's `EVENT_PROCESS_TIME` table, already in March.
- **Floats:** every compare constant is a float literal widened to double (0.42, 0.4, 0.45, 0.69, 0.607, 1.033, 0.566, 1.1, 0.766, 1.233, 0.6, 0.54, 0.9, 1.533, 0.433, 1.066, 0.3, 0.2, ...), read out of the exe with the same values as March's `f` literals.

**Declared for later phases.** `CX2GUChung::IsInJobLine( int )` (`32beaf10`, vslot 0x670; classes 20 / 110 -> 1, 21 / 111 -> 2, 27 / 117 -> 3) is declared in `X2GUChung.h` under `_CORE` with no body: CH1l owns it (its kickoff says so), and the `_LIVE` build does not link until it lands. `CSI_CFG_COMBO_XXFRONTZ` (June script id 323) is the last enumerator of `CHUNG_STATE_ID`, under `_CORE`; CH1e owns the state (kickoff amended). `UpCannonBallCountAtThisAnimationTime` (`32bf3d30`) is March's, called by name; its June body is CH1q's. `CanUseCannon` is June's inline (as CH1b).
Checklist §7 L253 (`+0x7cc8` power rate): none of the 37 functions touches it. 7ai (BFI arguments): none.

**Gates.** `GIT_CONFIG ... diff.algorithm=patience flagcheck JUNE_CHAR_CHUNG_LIVE --worktree --files X2Lib/X2GUChung.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. (Git's default Myers diff mis-aligns the 1,024 inserted lines and prints `+1059 -35 nonascii 6`; `git diff --patience --numstat` is `1024 / 0` in `X2GUChung.cpp` and `6 / 0` in `X2GUChung.h`. The review's flagcheck needs the same env.) `flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.h`: `RESULT: PASS`.
`junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH, `cl /P` shows `IsInJobLine( 1 )` once with `_LIVE` and not without): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 17:00:50, md5 0f6d97b3b85073e929a545c2bb19d735`, equal to `X2/US_SERVICE/x2.exe`; built at main `65c33d7f` + the 2 code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

No drift beyond the two declarations above. **Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1d review` (Opus 5.5 high), then CH1d2.

### CH1d - Review (2026-10-31, Opus 5.5)

**Verdict: pass, one comment fixed.** I read the functions where June differs, plus four `= March` ones, against `charport_adis.py chung --phase CH1d` (all 37 listings are carved there, including the four the kickoff expected only in `funcdis.py`).
- **ZZ `32bdd5e0`.** The walk cancel (`pureRight && !bIsRight || pureLeft && bIsRight` = `CAN_WALK_CANCEL`) clears word `+0x7d85` ([0] [1]), word `+0x7d88` ([3] [4]) and byte `+0x7d8a` ([5]), and leaves [2] alone. `t <= 0.83 && oneX && class 0x75` sets [5] and clears [0] [1] [3] [4]. Otherwise `t <= 0.45`: oneZ + pureDown sets [1], oneZ alone sets [0], and both clear [3]. Then `oneX && 0x6e` sets [3] and clears [0] [1] [4] [5], and `oneX && 0x6f` sets [4] and clears [0] [1] [3] [5]. After that comes the chain [1] 0xad, [0] 0xa8, [3] 0xc0, [4] 0xd2, [5] 0x12c. The anim-end WAIT and 0.1 sit inside the ground branch, because June's JUMP_DOWN path jumps straight to vslot 0x7d4. All of this matches the port. **Fix:** the line comment said flag [3] is set for "Iron Paladin and Deadly Chaser", but Deadly Chaser sets [4]. It now reads "Iron Paladin only" (comment only, inside `_LIVE`).
- **ZZZ `32bdd7e0`.** `0.4 <= t <= 0.69` sets [0] or [1]. `t > 0.69` branches to 0xa9 / 0xaa. The cancel chain is: vslot 0x7f4 with `bFootOnLine` for `t > 0.5`, then the awake-lock hyper branch (`+0x8d4c = 1`, vslot 0x6b8), then the opposite-direction walk with ground, then the dash with ground. The anim end is tested after the chain, never inside it. This matches the port.
- **ZZZXFinish `32bddc20` (0.2) / XXZXFinish `32bde640` (0.3).** The walk is any direction, and both the walk and the dash need `bFootOnLine`. XXZXFinish tests the anim end before the cancel chain, inside the ground branch, and the port does the same. Both match.
- **XX `32bde240`.** The block runs while [2] (`+0x7d87`) is set. If `+0x8c18` (stateChange) `< t` or the anim has ended, it calls `StateChange( 0x143, false )`, clears [2] and goes to vslot 0x7d4. Inside the window (`+0x8c10` keyInputStart `<= t`, `t < +0x8c14` keyInputEnd), vslot 0x670 is called with 1, then `oneZ` and the forward direction (`pureRight && bIsRight || pureLeft && !bIsRight`) set [2] and go to 0x7d4. `EventProcessTime` is `keyInputStart, keyInputEnd, stateChange` (`X2GUUser.h:1227`). For `0.4 <= t <= 0.8`, `oneZ && ![2]` sets [1] and `oneX` sets [0]; for `t > 0.8`, [1] goes to 0xb2 and [0] to 0xb1. This matches the port. 0x143 = 323, the script id in the enumerator's comment.
- **Spot-checks (`= March`).** I checked four line by line. `32bf75b0` ZZZXCannonBallShot FrameMove: `0.001` oneshot, play count `< 5` (`MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE(5)`), `count > 1 || IsFullHyperMode`, the effect set, `32bf5280( 1 )`, then `L"TRACE_START0"` (read at `3344ab00`) / `L"STR_Kwa"`, and the anim end calls `31fd4290`. `32bf77c0` ZZZXFinish FrameMove: `IsFullHyperMode || count > 0` = `CanUseCannon`, `STR_Kwang`, else `MISFIRE`. `32bdddf0` ZZdownZ: `0.566 <= t < 1.1` sets the flag, `t >= 1.1` branches to 0xae, and for `t >= 0.766` the dash comes before the opposite-direction walk to `CSI_WALK`. Its anim-end check sits outside the ground branch, because June's JUMP_DOWN path jumps to it, and the port does the same. `32bf5360` ShowOnomatopoeiaParticle: `+0x41f9`, `GetWeaponBonePos( 0, name, 0 )`, `GetProj3DPos`, then the minor particle `CreateSequence`. All four are March's.
- **Declarations.** `CSI_CFG_COMBO_XXFRONTZ` and `IsInJobLine( int )` are under `_CORE` (`X2GUChung.h:284` / `:604`). The CH1e and CH1l kickoffs carry their "Amended by CH1d" notes, and the JUNE_BUGS 491 row is accurate.
- **CH1d2 kickoff** is amended: its Done gate said "phase `CH1d`", and it now carries this phase's reading notes.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --range c350eb8a^..c350eb8a` (patience diff): `X2GUChung.cpp +1024 PASS`, `X2GUChung.h OTHER (_CORE)`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `--worktree --files X2Lib/X2GUChung.cpp`: `1 PASS`, with 1 deleted line (the replaced comment, inside the flag). Both runs are RESULT FAIL only because the `#define` is commented out. `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows. One-file probes (`census.cl_cmd` / `cl_env`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors.
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 17:14:20, md5 c18511d881f6b165ed2b1fb439abce3f, equal to X2/US_SERVICE/x2.exe (2026-10-31 17:14:13); built at main c350eb8a + 1 uncommitted code file(s).` Nothing to play-test. Next: `/x2-phase char-chung CH1d2` (Sonnet 5.5 high).

### CH1d2 - Done (2026-10-31)

All 21 class-map rows of phase CH1d2 are written, all `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`). No `new` row, no new `Always.h` define (`_LIVE` stays `//#define`). 21 funcmap rows `Char Chung CH1d2`. JUNE_BUGS 492.

| Group | June -> `CX2GUChung::` |
|---|---|
| Init (preload, `= March`) | `32bd8cb0` AttackDashXInit, `32bd8d40` ComboDashJumpZZLandingCannonBallShotInit, `32bd8da0` AttackDashJumpXInit |
| EndFuture / FrameMoveFuture (`= March`) | `32bd8d00` AttackDashXEndFuture, `32bd8df0` AttackDashJumpXFrameMoveFuture, `32beb6d0` AttackJumpZ, `32beb760` AttackJumpX |
| StartFuture | `32bee7c0` AttackDashJumpX (also `CSI_CTT_DASH_JUMP_COMBO_XX`) |
| EventProcess | `32bde810` ComboDashZ, `32bdea80` AttackDashX, `32bdec60` AttackJumpZ, `32bdeca0` AttackJumpX, `32bdece0` ComboDashJumpZ, `32bedef0` ComboDashJumpZZ, `32bdf040` ComboDashJumpZZLanding, `32bdf160` ...LandingCannonBallShot, `32befa80` AttackDashJumpX |
| FrameMove | `32bf80e0` AttackDashX, `32bf8840` AttackDashJumpX, `32c02c30` ComboDashJumpZZLanding, `32c03cf0` ...LandingCannonBallShot |

**Where June differs from March** (the other 13 are March's logic written out whole; `SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER` / `ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER` already expand to June's code, `+0x24c` timer = `EventTimerOneshot`, `+0x8e90 ? +0x8e84 : +0x87d0` = `m_pXSkinAnimFuture`):
- **Open Mind (skill 26415 `RNSI_P_CIP_OPEN_MIND`, `GetSkillLevelValue` = June `31fceab0`).** `AttackDashXFrameMove` `32bf80e0` and `AttackDashJumpXFrameMove` `32bf8840`: learned -> the effect set plays with no `UseCannon` (dash X plays the Iron Paladin set `EffectSet_CSI_CIP_ATTACK_DASH_X` for every class, no onomatopoeia); else `CanUseCannon()` as March. `AttackDashJumpXStartFuture` `32bee7c0` skips the 1/4 speed factor when it is learned (`false == CanUseCannon() && level <= 0`).
- **`AttackDashXEventProcess` `32bdea80`:** the X X branch (`CSI_CSG_COMBO_DASH_XX`, 0.48) tests class 111 only, March also took Shooting Guardian (21): **JUNE_BUGS 492**. Iron Paladin 110 (Z, 0.53333 -> `CSI_CIP_COMBO_DASH_XX`) and Tactical Trooper 117 (X, 0.53333 -> `CSI_CTT_DASH_COMBO_XX`) as March; the Deadly Chaser dash cancel (`m_fDashCancelAfter`, else-if after the two time tests) also needs `IsOnSomethingFuture`. The switch in the frame move (jump table `32bf87cc` / `32bf87dc`): classes 21 and 111 CSG, 110 CIP, 117 CTT, else the plain set (= March).
- **`AttackDashJumpXEventProcess` `32befa80`:** the 0.36 X branch (`CSI_CSG_COMBO_DASH_JUMP_X`) is class 111 only (**JUNE_BUGS 492**); for every other class two windows: flag [0] / `m_fEventTime[0]`, class 110 + X -> `CSI_CIP_COMBO_DASHJUMP_XX`; flag [1] / `m_fEventTime[1]`, class 117 + X -> `CSI_CTT_DASH_JUMP_COMBO_XX` (each fires on `stateChange < t` or anim end, then clears its flag). In the air, X starts `CSI_ATTACK_JUMP_X` for classes 6 / 20 / 27 only (March: 110 and 117 too, not 21); the combo guide `JX` is shown for every class. Landing, wall landing, `JZ`, the down-force dive as March.
- **`ComboDashJumpZEventProcess` `32bdece0`:** replaces March's 0.3 s window with four flag + `m_fEventTime[0..3]` pairs (`+0x8c10`-`+0x8c3c`, stride 12, `keyInputStart` / `keyInputEnd` / `stateChange`): [0] Z -> `CSI_COMBO_DASH_JUMP_ZZ`; [1] X + job line 1 -> `CSI_CFG_COMBO_DASH_JUMP_ZX`; [2] X + job line 3 -> `CSI_CHG_DASH_JUMP_COMBO_ZX`; [3] X + job line 2 -> `CSI_CSG_COMBO_DASH_JUMP_ZX_READY`. One else-if chain; a flag whose time has not come falls through to the window tests.
- **`ComboDashZEventProcess` `32bde810`, `...LandingCannonBallShotEventProcess` `32bdf160`:** the any-direction walk cancel (to `USI_WAIT`) and the dash cancel also need `IsOnSomethingFuture` (March's `WALK_CANCEL_AFTER` / `DASH_CANCEL_AFTER` do not); the arrow-key test does not.
- **Floats:** 0.5, 0.3, 0.13, 0.01, 0.15, 0.48, 0.53333, 0.58, 0.566, 0.25, 0.1, 0.2 are float literals widened to double in June's compares (read from the exe); `0.36` is a double, as in March.

**Declared for later phases.** `CSI_CIP_COMBO_DASHJUMP_XX` (June script id 325, CH1f), `CSI_CTT_DASH_JUMP_COMBO_XX` (328, CH1j) and `CSI_CSG_COMBO_DASH_JUMP_ZX_READY` (333, CH1g2) are appended to `CHUNG_STATE_ID` under `_CORE` with no `= value`, like CH1d's `CSI_CFG_COMBO_XXFRONTZ`. The C++ values are March's append order (a probe printed `CSI_CFG_COMBO_XXFRONTZ` = 274, `CSI_COMBO_DASH_JUMP_ZZ` = 140, `CSI_CSG_COMBO_DASH_XX` = 155), not June's script ids; CH1m maps them. The CH1f / CH1g2 / CH1j kickoffs carry an "Amended by CH1d2" note. `IsInJobLine` stays declared only (CH1l). `UseCannon` / `CanUseCannon` / `ShowOnomatopoeiaParticle` are March's.
Checklist §7 L253 (`+0x7cc8` power rate): none of the 21 touches it. 7ai (BFI arguments): none. No drift beyond the three enumerators.

**Gates.** `GIT_CONFIG ... diff.algorithm=patience flagcheck JUNE_CHAR_CHUNG_LIVE --worktree --files X2Lib/X2GUChung.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out (`+554`). `flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.h`: `1 PASS`, `RESULT: PASS`.
`junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH; from Git Bash the define is `-DSERV_...`, a leading `/D` is rewritten to a path): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors (`cl /P`: `EffectSet_CSI_CIP_ATTACK_DASH_X` 2 lines on, 1 off; `RNSI_P_CIP_OPEN_MIND` 7 on, 1 off).
`DEPLOY OK: X2_offline.exe is 25,864,704 B, dated 2026-10-31 17:46:42, md5 f7afb96b6a00a768d8f234c1b2d68410, equal to X2/US_SERVICE/x2.exe (2026-10-31 17:46:35)`; built at main `7213a084` + the 2 code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1d2 review` (Opus 5.5 high), then CH1e.

### CH1d2 - Review (2026-10-31, Opus 5.5)

**Verdict: pass, nothing changed.** I read every function where June differs, plus three `= March` ones, against `charport_adis.py chung --phase CH1d2`. All 21 are carved there.
- **AttackDashX EventProcess `32bdea80`.** Class `0x6f` only: if `![0] && t <= 0.48` (double `0.47999998927` = `(double)0.48f`), `oneX` sets [0]. Otherwise, if `t >= 0.48` and [0] is set, it goes to `0xc9`. Otherwise it tries the dash cancel: `m_fDashCancelAfter` (`+0x8c9c`) `>= 0`, `t > it`, `bFootOnLine`, `pureDoubleRight || pureDoubleLeft`, vslot 0x668. Class `0x6e`: `oneZ` and 0.53333 go to `0x101`. Class `0x75`: `oneX` goes to `0x12e`. The anim end goes to JUMP_DOWN (`-45.0`) or WAIT, and the function ends in vslot 0x7d4. This matches the port, and JUNE_BUGS 492 is accurate.
- **AttackDashX FrameMove `32bf80e0`.** A `0.3` oneshot. `31fceab0( 0x672f ) > 0` builds `L"EffectSet_CSI_CIP_ATTACK_DASH_X"` (length 0x1f), plays it and jumps to CommonFrameMove, with no `32bf5280` and no onomatopoeia. Otherwise `IsFullHyperMode || count > 0` (March's `CanUseCannon`). I read the jump table from the exe: targets `32bf82db` / `839f` / `848a` / `8557`, and its index bytes give `{21: 0, 110: 1, 111: 0, 117: 2}`, all other classes 3. That means CSG, CIP, CTT, else the default, then `32bf5280( 1 )`, `L"TRACE_START0"` / `L"STR_Kwang"`, else `MISFIRE`. This matches the port.
- **AttackDashJumpX StartFuture `32bee7c0` / FrameMove `32bf8840`.** StartFuture returns early when the cannon is usable or when Open Mind's level is `> 0`. Otherwise it scales the `+0x85f4` vector (stride 0x14, `x` and `y`) by `0.25`. FrameMove: a `0.13` oneshot; Open Mind plays `EffectSet_CSI_ATTACK_DASH_JUMP_X` without `32bf5280`; otherwise the cannon branch plays it and calls `32bf5280( 1 )`; else `MISFIRE`. Both match the port.
- **AttackDashJumpX EventProcess `32befa80`.** Class `0x6f` with `0.36 <= t` (a double) and `oneX` goes to `0xcb`. Otherwise this chain runs: [0] (`+0x7d85`) with `+0x8c18 < t` or the anim ended goes to `0x145` and clears [0]. Inside the `+0x8c10 <= t < +0x8c14` window, class `0x6e` and `oneX` set [0]. [1] (`+0x7d86`) with `+0x8c24` goes to `0x148`. Inside the `+0x8c1c` / `+0x8c20` window, class `0x75` sets [1]. On ground, down-force goes to `0x9a` with `ShowComboCommand( L"A", L"AXD", true )`, and otherwise to `0x16`. With `t > 0.58`: `SpecialAttackEventProcess( 2 = ASUT_AIR )`, then the wall (`oneUp || oneDoubleUp`, gravity off, `0xa1`), then `oneZ` to `0xb7` + `JZ`, then `oneX` to `0xb8` for classes 6 / 0x14 / 0x1b only, with `JX` shown for every class. `oneDown` sets `nowSpeed.y = 2 * fMaxGSpeed` and the down-force. This matches the port.
- **ComboDashJump Z EventProcess `32bdece0`.** On ground it goes to `0x16`. Otherwise four pairs, in this order: [0] `0x8c10/14/18` with `oneZ` -> `0xba`; [1] `0x8c1c/20/24` with `oneX` + vslot 0x670( 1 ) -> `0xc3`; [2] `0x8c28/2c/30` + 0x670( 3 ) -> `0x10e`; [3] `0x8c34/38/3c` + 0x670( 2 ) -> `0x14d`. In each pair the flag side is tested before its window, and every hit ends the chain. This matches the port.
- **ComboDash Z `32bde810` / ZZLanding CannonBallShot `32bdf160` EventProcess.** The skill cancel (`t > 0.5` / `0.566`, `bFootOnLine`, vslot 0x7f4( 1 = ASUT_GROUND ), the awake-lock hyper branch) comes first. Then the any-direction walk with ground goes to `0xd`, the dash with ground goes to vslot 0x668, and the 14-key arrow test without ground goes to `0xe` (Z) or `0xd` (landing). The anim end (WAIT with `m_fCanNotAttackTime` `+0xde0` = `0.1`, or JUMP_DOWN `-45`) sits outside the chain for Z and is the chain's last else for the landing. Both match the port.
- **Spot-checks (`= March`).** `32beb760` AttackJumpX FrameMoveFuture: `walk triple * m_fRunJumpSpeedRate` = `GetWalkSpeed()`, and the down key subtracts `fGAccel * m_fElapsedTime` and sets [0]. `32bedef0` ComboDashJumpZZ: `CanUseCannon` -> `0xbc`, else `0xbb`. `32bdf040` ZZLanding: no ground -> `0x16`, `SKILL_CANCEL_AFTER( 0.01 )`, then the anim end -> WAIT `0.1`. I also skimmed `32c02c30` / `32c03cf0`: the `0.01` oneshot, `+-120` / `+-100`, `m_fLandDemolisherPowerRate` `+0x96d0`, and in the second one the `0.15` cannon ball shot. All match March.
- **Declarations.** The three enumerators sit at `X2GUChung.h:285-287` inside the `_CORE` block, after `CSI_CFG_COMBO_XXFRONTZ`. The CH1f / CH1g2 / CH1j "Amended by CH1d2" notes are there. The CH1e kickoff gains a "Checked by the CH1d2 review" note (the `CSI_CFG_COMBO_DASH_JUMP_ZX` entry from `32bdece0`).

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --range c747a3bb^..c747a3bb` (patience diff): `X2GUChung.cpp +554 PASS`, `X2GUChung.h OTHER (_CORE)`, `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched inserted, 0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only because the `#define` is commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`, same range: `X2GUChung.h +3 PASS`, `RESULT: PASS`. `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). `enumscan.py --check X2GUChung.cpp X2GUChung.h`: 0 A/B rows. One-file probes (`census.cl_cmd`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors. No build: no source changed (the deployed exe is CH1d2's `f7afb96b`). Nothing to play-test. Next: `/x2-phase char-chung CH1e` (Sonnet 5.5 high).

### CH1e - Done (2026-10-31)

All 12 class-map rows of phase CH1e are written: 10 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`), 2 `new` (`CFGComboXXFrontZEventProcess` `32be9210`, `CFGComboXXFrontZZEventProcess` `32be9600`, under `_CORE`, declared in `X2GUChung.h`, referenced by nothing). `_LIVE` stays `//#define`; no new `Always.h` define. 12 funcmap rows `Char Chung CH1e`. No JUNE_BUGS row (nothing June shows is a bug; the dead `|| IsAnimationEnd` in `32be9210` after the animation-end branch is kept as written).

| June | `CX2GUChung::` | kind | vs March |
|---|---|---|---|
| `32bd8ed0` | CFGComboDashJumpZXXStart | inside | = March (CommonStateStart) |
| `32bdf650` | CFGComboZZXEventProcess | inside | walk / dash cancel (1.16) need ground; macros already expand to June's hyper branch |
| `32bdf820` | CFGComboDashZEventProcess | inside | walk / dash cancel (0.5) need ground |
| `32bdfaf0` | CFGComboDashZZEventProcess | inside | = March |
| `32bdfba0` | CFGComboDashZZZEventProcess | inside | walk / dash cancel (0.633) need ground |
| `32bdfe00` | CFGComboDashJumpZXEventProcess | inside | rewritten: Z and X are `m_fEventTime[0]` / `[1]` windows with flags [0] / [1] (`+0x8c10`-`+0x8c24`), no anim-end branch, no 0.45 X |
| `32be0030` | CFGComboDashJumpZXXEventProcess | inside | ground contact alone -> `CSI_CFG_COMBO_DASH_JUMP_ZXXLanding` (March also needed the anim end) |
| `32be0060` | CFGComboDashJumpZXXLandingEventProcess | inside | walk / dash cancel (0.3) need ground |
| `32be9210` | CFGComboXXFrontZEventProcess | new | flag [0] + `m_fEventTime[0]` Z window -> `CSI_CFG_COMBO_XXFRONTZZ`; cancels from `m_fSkillCancelAfter` / `m_fBWalkCancelAfter` / `m_fWalkCancelAfter` / `m_fDashCancelAfter` / `m_fAttackCancelAfter` (`+0x8c90`-`+0x8c8c`), all ground-gated |
| `32be9600` | CFGComboXXFrontZZEventProcess | new | anim end, then the same five cancels |
| `32c01be0` | CFGComboDashZFrameMove | inside | = March; June also uses it for `CSI_CFG_COMBO_XXFRONTZ` and `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` (CH1f/CH1m register it) |
| `32c1be30` | CFGComboZZXFrameMove | inside | = March (0.53/0.61/0.71/0.8/0.94 effects, `DAMAGE_DATA_SECOND` 0.633, `DAMAGE_DATA_FINAL` 0.84; narrow strings read from the exe) |

New enumerator `CSI_CFG_COMBO_XXFRONTZZ` (June script id 324) appended at the end of the `_CORE` block (earlier enumerator values unchanged). The new functions are named after the class's pattern (`CFGCombo*EventProcess`), not CH0's `CSI_*_EventProcess` proposal (CH1a did the same for `JumpUpStart`). The 2 `XXX_CSI_CFG_COMBO_DASH_JUMP_ZXX*` tables and `_CSI_CFG_COMBO_XXFRONTZ` are dead data (CH0 found no June registration): nothing written.
Reading notes: `+0x8e90 ? +0x8e84 : +0x87d0` = `m_pXSkinAnimFuture`; `32070cf0` / `32070d60` are inside the `SKILL_CANCEL_AFTER` macro (JUNE_BUFF_AWAKE_LOCK); `0x4b0` / `0x4b4` = `GetComboZStateID` / `GetComboXStateID`. Float literals are float widened to double in June's compares. Checklist §7 L253 (`+0x7cc8`): none of the 12 touches it. 7ai: none. No drift.

**Gates.** `GIT_CONFIG ... diff.algorithm=patience flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.h`: `1 PASS`, `RESULT: PASS`; the same flag on the `.cpp` and `JUNE_CHAR_CHUNG_LIVE` report the file as `OTHER` (the file carries both flags; `+497`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`); LIVE's RESULT FAIL is only the commented-out `#define`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors.
`DEPLOY OK: X2_offline.exe is 25,866,240 B, dated 2026-10-31 20:32:49, md5 f67db16912cab018d01614b914151db0, equal to X2/US_SERVICE/x2.exe (2026-10-31 20:32:42)`; built at main `341b4bf7` + the 2 code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1e review` (Opus 5.5 high), then CH1f.

### CH1e - Review (2026-10-31, Opus 5.5)

**Verdict: pass, nothing changed.** I read all 12 against `charport_adis.py chung --phase CH1e` (all carved there).
- **Dash-jump Z X EventProcess `32bdfe00`.** On ground: `m_bDownForce` -> `0x9a`, else `0x12`. Otherwise this chain runs: [0] (`+0x7d85`) with `+0x8c18 < t` or the anim end -> `0xb7` and [0] cleared; `oneZ` inside `+0x8c10 <= t < +0x8c14` sets [0]; [1] (`+0x7d86`) with `+0x8c24 < t` or the anim end -> `0xc4` and [1] cleared; `oneX` inside `+0x8c1c <= t < +0x8c20` sets [1]. Every taken branch, landing included, lands on `32bdffd1`, which clears `m_bDownForce`. Then `oneDown` sets the down-force and clears both flags (one `word` store). Down-force with `t > 0.6` (double `(double)0.6f`) sets `nowSpeed.y = fMaxGSpeed + fMaxGSpeed`. This matches the port.
- **Dash-jump Z X X `32be0030`.** `bFootOnLine` alone -> `0xc5`, then vslot 0x7d4. This matches the port.
- **X X front Z `32be9210` / Z Z `32be9600` (new).** The anim end goes to JUMP_DOWN `-45` or WAIT `0.1`. In `32be9210` it is followed by [0] with `+0x8c18 < t` or the (dead) anim end -> `0x144` and [0] cleared, and then `oneZ` in the `+0x8c10` / `+0x8c14` window sets [0]. Then come the five cancels: `+0x8c90` through the expanded `SKILL_CANCEL_AFTER` (`>= 0`, `t >`, ground, vslot 0x7f4( 1 ), awake-lock hyper branch); `+0x8c94` back-walk; `+0x8c98` walk; `+0x8c9c` dash (vslot 0x668); `+0x8c8c` `pureZ` -> vslot 0x4b0 and `pureX` -> 0x4b4. Every cancel is `>= 0`, `t >` and `bFootOnLine`. The offsets are `X2GUUser.h:4755-4759` in order (`m_fAttackCancelAfter` `0x8c8c` first). Both functions match the port.
- **ZZX `32bdf650` / dash Z `32bdf820` / dash ZZZ `32bdfba0` / ZXX landing `32be0060`.** The thresholds are 1.16 / 0.5 / 0.633 / 0.3. Each has the macro's skill and hyper pair, then the walk (`pureRight || pureLeft`, ground) and the dash (ground). Dash Z and ZZZ add the 14-key arrow test with no ground check -> `0xe`. ZZX keeps its anim end as a separate `if` after the chain. ZXX landing's anim end is WAIT `0.1` only. In dash Z, `t < 0.38` with `oneZ` sets [0] as the chain's last else, and `0.38 <= t` with [0] -> `0xc1` sits outside the chain (the anim-end and hyper exits jump to it). All four match the port.
- **Spot-checks (`= March`).** `32bd8ed0` is CommonStateStart only. `32bdfaf0` dash ZZ: anim end; else `0.6 < t < 0.87` (doubles of the floats), where `oneZ` sets [0]; else [0] -> `0xc2`. `32c1be30` ZZX FrameMove: Land Demolisher oneshots at 0.53 / 0.61 / 0.71 / 0.8 / 0.94, each gated on `+0x96c8` (5 gates). Right-facing uses `+70`, else `-70`. HORIZONTAL takes `true, vTargetPos` and REVERSE takes `false, GetPos()`, with power `+0x96d0`. Then come the ungated 0.633 / 0.84 damage tables. I read the narrow strings from the exe: `0x33509584` = `DAMAGE_DATA_SECOND`, `0x3350dc70` = `DAMAGE_DATA_FINAL`. All match March.
- **Declarations.** `CSI_CFG_COMBO_XXFRONTZZ` is the last enumerator of the `_CORE` block (`X2GUChung.h`). The two new functions are declared under `_CORE` after `CFGComboDashJumpZXXLandingEventProcess`. CH1f's "Amended by CH1e" note is accurate; the CH1f kickoff gains a "Checked by the CH1e review" note (the cancel-member offsets).

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --range 16490f1e^..16490f1e` (patience diff): `X2GUChung.cpp +497 OTHER (_CORE)`, `X2GUChung.h +0 OTHER (_CORE)`, `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched inserted, 0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only because the `#define` is commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`, same range: `X2GUChung.h +6 PASS`, `.cpp OTHER (_LIVE)`, `RESULT: PASS`. `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). `enumscan.py --check X2GUChung.cpp X2GUChung.h`: 0 A/B rows. One-file probes (`census.cl_cmd`, `/Zs /EHsc`): `_LIVE` off exit 0 / 0 errors; `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0 / 0 errors. No build: no source changed (the deployed exe is CH1e's `f67db169`). Nothing to play-test. Next: `/x2-phase char-chung CH1f` (Sonnet 5.5 high).

### CH1f - Done (2026-10-31)

All 27 class-map rows of phase CH1f are written: 25 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`), 2 `new` under `_CORE` (`CIPComboDashJumpXXEventProcess` `32be9930`, `CIPComboDashJumpXXLandingEventProcess` `32be9960`, declared in `X2GUChung.h`, referenced by nothing). `_LIVE` stays `//#define`; no new `Always.h` define. 27 funcmap rows `Char Chung CH1f`. No JUNE_BUGS row. Read from `charport_adis.py chung --phase CH1f` (fifteen of these are thunks or stubs Ghidra has no function for).

| Group | June -> `CX2GUChung::` | vs March |
|---|---|---|
| Init / End / FrameMove stubs (15) | `32bda3a0`-`32bda4c0`: PowerDownLandingZ / ZZ, DashXX / XXX / XXXX / XXXZ | = March (`ret`, `CommonStateEnd` vslot `0x7dc`, `CommonFrameMove` vslot `0x7cc`, the five preloads of `32bda3c0`) |
| EventProcess | `32be4500` PowerDownLandingZZ, `32be4710` DashXX, `32be4810` DashXXX | = March |
| EventProcess | `32be42c0` PowerDownLandingZ | the any-direction walk cancel and the dash cancel after 0.6 also need ground |
| EventProcess | `32be48e0` DashXXXX (1.1333), `32be4b40` DashXXXZ (0.633) | skill / hyper, then walk (`USI_WAIT`) and dash cancels need ground, then the 14-key test -> `USI_WALK` (no ground test) |
| EventProcess (new) | `32be9930` DashJumpXX | `bFootOnLine` alone -> `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` (0x146) |
| EventProcess (new) | `32be9960` DashJumpXXLanding | anim end, then the five cancel members `+0x8c90` / `94` / `98` / `9c` / `8c`, each ground-gated (same shape as CH1e's `CFGComboXXFrontZZEventProcess`) |
| FrameMove | `32c01fd0` DashXXXX, `32c02280` PowerDownLanding, `32c02530` PowerDownLandingZ, `32c02920` PowerDownLandingZZ | = March: Land Demolisher effect sets at 0.62 / 0.1333 / 0.35 + 0.51, cannon effect + `UseCannon( 1 )` at 0.335 (no Open Mind clause) |

In June the `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` form is the only one (`EventTimerOneshot`), so the `_LIVE` branches use it. New enumerator `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` (June script id 326) is appended to the `_CORE` block of `CHUNG_STATE_ID`, like CH1d2's. Names are in the class pattern (`CIPComboDashJumpXX...`), not CH0's proposals. Checklist §7 L253 (`+0x7cc8`): none of the 27 touches it (`grep 7cc8` over the listings is empty). 7ai: none. No drift. The CH1m kickoff gains an "Amended by CH1f" note (register the two new functions).

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h` (patience diff): `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (the .h, _CORE), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`: `1 PASS`, `RESULT: PASS`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors; `cl /P` shows both new functions and the enumerator in both.
`DEPLOY OK: X2_offline.exe is 25,867,264 B, dated 2026-10-31 21:04:50, md5 5c90647edf0fa0c07e346521952e3457, equal to X2/US_SERVICE/x2.exe (2026-10-31 21:04:43)`; built at main `0510b763` + the 2 code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1f review` (Opus 5.5 high), then CH1g.

### CH1f - Review (2026-10-31, Opus 5.5)

**Verdict: pass, nothing changed.** I read all 27 against `charport_adis.py chung --phase CH1f`.
- **Stubs `32bda3a0`-`32bda4c0` (15).** Each is a bare `ret`, a `jmp` through vslot `0x7dc` (CommonStateEnd) or `0x7cc` (CommonFrameMove), or `32bda3c0`'s five preloads (three `CHUNG_Cannon_Effect0N.X` meshes, `CenterLight_Gray01.dds`, `Steam_Bp.dds`). None does more. This matches the port.
- **PowerDownLandingZ `32be42c0`.** Off ground -> `0x11` and `-45`. On ground, `0.2 <= t <= 0.6` with `oneZ` sets [0] and skips the chain. Otherwise the chain runs (all `t > 0.6`): skill + hyper (the macro), walk (`bFootOnLine`, `pureRight || pureLeft`) -> `0xd`, dash (`bFootOnLine`) -> vslot 0x668, then [0] -> `0x100`. The anim end (WAIT, `0.1`) is inside the on-ground branch. This matches the port. `SKILL_CANCEL_AFTER` opens with `else if`, so it chains onto the window `if` the way June does.
- **DashXXXX `32be48e0` (1.1333) / DashXXXZ `32be4b40` (0.633).** On anim end -> JUMP_DOWN or WAIT, and the function exits. Otherwise: skill + hyper, walk (ground) -> `0xd`, dash (ground), then the 14-key test (arrows, Z X A S D C Q W E R) -> `0xe` with no ground test. The macro's leading `else` makes the chain the anim-end `if`'s else, as in June. Both match the port.
- **DashJumpXX `32be9930` / landing `32be9960` (new).** `bFootOnLine == 1` -> `0x146`. In the landing, the anim end exits. Otherwise the five cancels run, each `>= 0`, `t >` and `bFootOnLine`: `+0x8c90` skill (macro), `+0x8c94` back-walk -> `0xd`, `+0x8c98` walk -> `0xd`, `+0x8c9c` dash, `+0x8c8c` `pureZ` -> vslot 0x4b0 and `pureX` -> 0x4b4. Both match the port.
- **Spot-checks (`= March`).** `32be4500` ZZ: off ground -> `0x11`. On ground with `t > 0.6`, the order is skill + hyper, dash, walk (`CAN_WALK_CANCEL`) -> `0xe`, then 12 keys (no Z / X) -> `0xd`. The anim end sits outside the ground test. `32be4810` DashXXX: anim end; else `0.6 < t < 0.87`, where `oneX` sets [0] and else `oneZ` sets [1]; else [0] -> `0x103`, then [1] -> `0x104`. `32be4710` DashXX: `t >= 0.8` gives the walk cancel only; else `t < 1.0` with `oneZ` sets [0]; then `t >= 1.0` with [0] -> `0x102`. `32c02530`: 0.35 HORIZONTAL (`false, GetPos()`), then 0.51 VERTICAL at `-220` (right) / `+220`, both gated on `+0x96c8`. `32c02920`: 0.335, with `CanUseCannon` inlined (`IsFullHyperMode() || count > 0`) -> CANNON + `32bf5280( 1 )`, else MISFIRE. There is no Open Mind clause. `32c02280` 0.1333 `-125` / `+125` and `32c01fd0` 0.62 `+50` / `-50` also hold. All match March.
- **Standing stand-ins, unchanged.** The hyper branch's alternate future-animation accessor `32070cf0` (Luciel second body) is read as `m_pXSkinAnimFuture`, as in every earlier CH1 phase. `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` takes its value from the append order; CH1m reconciles the ids.
- **Checklist section 7 L253.** `grep 7cc8` over the 27 listings is empty. None of them touches it.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --range fd128801^..fd128801` (patience diff): `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched inserted, 0 #else/#endif without the flag token, 0 encoding change(s)`. RESULT is FAIL only because the `#define` is commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`, same range: `X2GUChung.h +6 PASS`, `RESULT: PASS`. `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 158). `enumscan.py --check X2GUChung.cpp`: 0 A/B rows. One-file probes (`census.cl_cmd`, `/Zs /EHsc`): with `_LIVE` off, exit 0 / 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, exit 0 / 0 errors. No build: no source changed (the deployed exe is CH1f's `5c90647e`). Nothing to play-test. CH1g's kickoff gains a "Checked by the CH1f review" note. Next: `/x2-phase char-chung CH1g` (Sonnet 5.5 high).

### CH1g - Done (2026-10-31)

All 20 class-map rows of phase CH1g are written: 9 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`), 11 `new` under `_CORE` (declared in `X2GUChung.h`, referenced by nothing). `_LIVE` stays `//#define`; no new `Always.h` define. 20 funcmap rows `Char Chung CH1g`. No JUNE_BUGS row. Read from `charport_adis.py chung --phase CH1g` plus Ghidra for `32c15990`.

| June | `CX2GUChung::` | vs March |
|---|---|---|
| `32bd8fc0` / `32bd9020` | `CSGComboDashXXInit` / `XXXInit` | = March (six preloads each) |
| `32be08b0` / `32be0b10` | `CSGComboDashXXEventProcess` / `XXXEventProcess` | = March (1.1 X window; walk, dash, skill / walk, dash; the ground tests June repeats sit inside the ground `else`) |
| `32bf5060` | `SetHandleChungMajorMeshByEnum` | = March |
| `32bf9790` / `32bf9c10` / `32bf97b0` | `CSGComboDashXXEnd` / `XXXEnd` / `XXXFrameMove` | = March (slots 4 / 5; 0.48 cannon shot, `GetNowPlayCount() < 5`) |
| `32c18950` | `CSGComboDashXXFrameMove` | differs: the 0.533 / 0.93 shots cost `FlushMp( 4 x Open Mind MP rate )` ( `GetSkillLevelValue( RNSI_P_CDC_OPEN_MIND ) > 0` then `GetLearnedSkillAbilityValue( SA_MP_CONSUME_REL, ... )`, else 1 ), the shell power is `GetPowerRate() x m_fRemodelRifleDamageRate`, and the head shot goes through the effect overload |
| `32c15990` | `SetExtraDamageMiniGun( CEffect*, float )` (new) | June's argument is the CEffect: null test, `m_ExtraDamage.Init()`, then for a rate above 0 `m_DamageTime + 1` ( `+0x84c`, funcmap `31f76010` / `31f85910`: `+0x850` is `m_DamageTimeNow` ) and `EDT_HEAD_SHOT`, time 5, level 0, rate = pure rate |
| `32be9c90` / `32bea080` | `CSGComboDashZEventProcess` / `ZXEventProcess` (new) | `m_fEventTime[0]` window with X, then `CSI_CSG_COMBO_DASH_ZX` / `_ZXX_READY`; anim end -> WAIT (Z also sets `m_fCanNotAttackTime` 0.1); the five ground-gated cancels (CH1e / CH1f shape) |
| `32bea460` / `32bea4c0` / `32be8ef0` | `ZXXReadyEventProcess` / `ZXXEventProcess` / `ZXXEndEventProcess` (new) | ready: off ground -> JUMP_DOWN, anim end -> `_ZXX`. ZXX: X window [0] loops `_ZXX`, from `m_iCountbyCsgComboDashZXXMiniGun >= 5` goes to `CSI_SI_A_CSG_HEAD_SHOT`; Z window [1] (`+0x8c1c/20/24`) -> head shot; anim end -> `_ZXX_END`. End: five cancels, no `m_fCanNotAttackTime`; also `CSI_CHG_DASH_JUMP_COMBO_ZXX`'s EventProcess |
| `32c15c00` / `32c163f0` / `32c0a8d0` | `ZXFrameMove` / `ZXXFrameMove` / `ZXXReadyFrameMove` (new) | mini-gun meshes into slots 0x11 / 0x12 (`Mesh_CSI_CDC_COMBO_ZZXX_Mini_Gun_L/R` in ZX, `Mesh_CSI_CDC_COMBO_Mini_Gun_L/R` in ZXX and ready), `Bip01_L/R_Hand` attach, shots from `Gun_R/L_Dummy03` at 0.378 / 0.567 (ZX) and 0.036 / 0.164 (ZXX), `FlushMp( 2 x rate )`, shell `..._DASH_ZX_FIRE_Mini_Gun_Shell`, `GetPos( false )` while the mesh is gone, not-enough-MP effect with degrees 0 / 0 / 0; ZXX counts a loop, ready resets the count at 0.123 |
| `32c0abc0` / `32c0ac00` | `ZXXEndStateStart` / `ZXEnd` (new) | `CommonStateStart` / `CommonStateEnd`, then both slots released; `ZXEnd` is the End of ZX, ZXX_READY and ZXX |

**What the header gained** (all `_CORE`): enumerators `CSI_CSG_COMBO_DASH_ZX` / `_ZXX_READY` / `_ZXX` / `_ZXX_END` (script ids 329-332, CH1m reconciles), `CHUNG_MAJOR_MII_CSG_COMBO_DASH_ZX_MINI_GUN_L` / `_R` before `CHUNG_MAJOR_MII_END` (June slots 0x11 / 0x12), members `m_fRemodelRifleDamageRate` (June `+0x98ac`, default 1: the rifling remodel passive's `SA_DAMAGE_REL`, `InitPassiveSkillState` `32c16be0` writes it, CH4a) and `m_iCountbyCsgComboDashZXXMiniGun` (`+0x9894`), the declarations. `X2SkillTree.h`: `RNSI_P_CDC_OPEN_MIND` = 26417 under `SERV_IRUHADEV_JUNE_ENUM_NAMES` (June `Enum.src.lua`). Divergent row `CX2GUChung::SetExtraDamageMiniGun` added, replaced by the overload (`ported under SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`).

**Routed, not ported** (scope rule): June sets the head shot only in `32c15990` (the byte pattern occurs once in the exe), so the X-front, dash-jump and Head Shot shells carry none: CH1g2 / CH1h drop March's calls there (`CH1g2.md`). Slot order after 0x0f differs from March's: CH1k. `+0x98ac` writer: CH4a. Registration list: `CH1m.md`. Shared ZXX_END EventProcess: `CH1i.md`. Checklist §7 L253 (`+0x7cc8`): none of the 20 writes it (`grep 7cc8` over the listings is empty); 7ai: none.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h` (patience diff): `files: 0 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. `flagcheck JUNE_CHAR_CHUNG_CORE` (cpp, h, SkillTree.h): `1 PASS, 2 OTHER`, `RESULT: PASS`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors (`cl /P` with it on shows `m_fRemodelRifleDamageRate` in the dash XX FrameMove).
`DEPLOY OK: X2_offline.exe is 25,876,480 B, dated 2026-10-31 21:51:46, md5 67b7ac4917ebc7bca94bb2255c681d3a, equal to X2/US_SERVICE/x2.exe (2026-10-31 21:51:39)`; built at main `9638ba07` + the code files of this phase (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1g review` (Opus 5.5 high), then CH1g2.

### CH1g - Review (2026-10-31, Opus 5.5)

**Verdict: the code passes and is unchanged. One routing note was wrong and is corrected in `CH1g2.md` and `CH1h.md`.** I read all 20 functions against `charport_adis.py chung --phase CH1g`, plus Ghidra for `32c15990`, its xrefs and `InitPassiveSkillState` `32c16be0`.
- **Dash XX FrameMove `32c18950`.** The 0.01 oneshot sets `+0x9718` (`m_bIncreaseCritical`) and puts the mini gun into slot 4 with `+0x19c` (per-frame simulation). The `Dummy01` position is zero when the frame is missing. The 0.533 shot and the `else if` 0.93 shot each start the rate at 1 and take `32007b10( 0x1b, 0x6731 )` only when `31fceab0( 0x6731 ) > 0`. They then call vslot `0xe4` (FlushMp) with `rate x 4.0`. The shell power is vslot `0x204` (GetPowerRate) x the protected float `+0x98ac`. The 23 `CreateInstance` arguments are pos, angle and move axis, land 0, reserve false, -1, 1, and `+0x96b8` (`m_fIncreaseRangeSubWeaponShot`, the range passive's other write) as the life-time rate, then the defaults. Next comes `32c15990( pEffect, +0x97d0 )`, then Fire1 / Fire2. When MP is short, the effect is (pos, 0, 0, rotate z). This matches the port.
- **`+0x98ac` / `+0x97d0`.** `32c16be0` resets `+0x98ac` to 1. For `0x6725` = `RNSI_P_CSG_REMODEL_RIFLE` it writes SA `0x1f` to `+0x96b8`, SA `0x32` to `+0x96bc` and SA `0xf` to `+0x98ac`. It writes `+0x97cc` from `0x6726` (March's `m_fCannonBallExtraChargeRate`), so the next float, `+0x97d0`, is `m_fMiniGunHeadShotRate` (same order in March's header). The Done note's wording (CH4a writes `+0x98ac`; the slot numbering is a CH1k hand-off) stands.
- **`32c15990`.** Its argument is the CEffect: a null test, then a pre-clear of June-only fields `+0x254` / `+0x258` (no March counterpart), then `31f8d4d0` (ExtraDamageData::Init) at `+0x250`. For a rate above 0 (`test ah, 0x41 / jnp`), it does `inc [+0x84c]` (`m_DamageTime`), then type `0x26` (`EDT_HEAD_SHOT` = 38 in June's `Enum.src.lua`), `+0x25c` 5.0, `+0x2b0` 0 (`m_iLevel` by layout), and rate / pure rate `+0x28c` / `+0x298`. `mov [reg+0x250], 0x26` occurs only here. This matches the port.
- **Correction: June calls `32c15990` from 9 functions (18 sites), not 4.** Ghidra xrefs: dash ZX `32c15c00`, ZXX `32c163f0`, dash XX `32c18950`, X-front fire `32c181f0`, dash-jump X `32c18f30`, dash-jump XX `32c19710` (CH1g2), and CDC X `32c19f30`, XZ `32c1a480`, ZZXX `32c1ae60` (CH1h). The class map's "dead" `32c15e60` / `32c185b0` are mid-function addresses inside `32c15c00` / `32c181f0`. So those shells **do** carry the head shot. CH1g2 and CH1h call the CEffect overload there and drop March's call only where the listing has none (the Head Shot skill's own shells).
- **New EventProcess.** Dash Z `32be9c90`: anim end -> JUMP_DOWN (`-45`) / WAIT + `0xde0` 0.1. [0] (`+0x8c18` or the anim end, kept dead) -> `0x149`. The X window is `+0x8c10 <= t < +0x8c14`. Then come the five ground-gated cancels (`+0x8c90` skill + hyper, `+0x8c94` back-walk, `+0x8c98` walk, `+0x8c9c` dash, `+0x8c8c` Z -> vslot 0x4b0 / X -> 0x4b4). ZX `32bea080` is the same shape with `0x14a` and no `0xde0`. Ready `32bea460`: off ground -> JUMP_DOWN, else anim end -> `0x14b`, nothing else. ZXX `32bea4c0`: anim end -> `0x14c`. [0] goes to `0xec` when `+0x9894 >= 5` (`jl` -> `0x14b`). X window [0], then [1] (`+0x8c24`) -> `0xec`, then the Z window `+0x8c1c` / `+0x8c20`, then the five cancels. End `32be8ef0`: anim end, then the five cancels, no `0xde0`. All match the port.
- **New FrameMoves / Start / End.** ZX `32c15c00`: 0.001 sets `+0x9718`, `ZZXX_Mini_Gun_L` / `_R` at the hand bones -> slots `0x11` / `0x12` with `+0x19c`. The 0.378 shot (`Gun_R_Dummy03`) and the 0.567 shot (`Gun_L_Dummy03`) are independent `if`s. Each starts from `GetPos( false )` (vslot 0x138), costs `FlushMp( rate + rate )`, fires `..._DASH_ZX_FIRE_Mini_Gun_Shell` x `+0x98ac` with `32c15990( +0x97d0 )` and Fire1, and makes the not-enough-MP effect at 0 / 0 / 0. ZXX `32c163f0` is the same at 0.036 / 0.164 with `COMBO_Mini_Gun_L` / `_R`, and `inc [+0x9894]` heads its 0.001 block. Ready `32c0a8d0` makes the 0.123 meshes and then resets `+0x9894` whether or not they were created. `32c0abc0` = CommonStateStart + both slots; `32c0ac00` = CommonStateEnd + both slots. All match the port.
- **Spot-checks (`= March`).** `32bf97b0`: 0.01 mesh -> slot 5 (no per-frame flag). At 0.48, if `GetNowPlayCount() < 5`, then cannon count `>= 1` or vslot 0x9c -> CANNON_BALL_SHOT + `32bf5280( 1 )`, else MISFIRE. `32be08b0`: off ground -> JUMP_DOWN; `t > 1.1` with `oneX` -> `0xca`; anim end -> WAIT + 0.1, the function exits; walk / dash / skill each gated on `bFootOnLine`. `32bf5060`: for -1 with a live slot, DestroyInstanceHandle( copy ), then store. Inits `32bd8fc0` / `32bd9020`, Ends `32bf9790` / `32bf9c10` also hold. All `= March`.
- **Checklist section 7 L253.** `grep 7cc8` over the 20 listings is empty.

**Gates** (range `86ecb41e^..86ecb41e`, patience diff). `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE`: `files: 0 PASS, 1 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR` and `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched inserted, 0 #else/#endif without the flag token, 0 encoding change(s)`. RESULT is FAIL only because the `#define` is commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`: `X2GUChung.h +24 PASS`, `RESULT: PASS`. `junediverge.py --check-all`: `RESULT: PASS` (allow-listed 160). `enumscan.py --check X2GUChung.cpp X2GUChung.h`: 0 A/B rows. One-file probes (`census.cl_cmd`, `/Zs /EHsc`): with `_LIVE` off, exit 0 / 0 errors; with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, exit 0 / 0 errors. No build: no source changed (the deployed exe is CH1g's `67b7ac49`). Nothing to play-test. Next: `/x2-phase char-chung CH1g2` (Sonnet 5.5 high).

### CH1g2 - Done (2026-10-31)

All 30 class-map rows of phase CH1g2 are written: 26 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`; 27 March functions because the dash-jump X / X X / landing Ends are one June function) and 4 `new` under `_CORE`, plus a 5th `new` for the second Head Shot frame move (`32bfc860`, see below). `_LIVE` stays `//#define`; no new `Always.h` define. 30 funcmap rows `Char Chung CH1g2`. JUNE_BUGS 493-496. Read from `charport_adis.py chung --phase CH1g2` (listings cut at the next non-`dead` class-map address).

| June | `CX2GUChung::` | vs March |
|---|---|---|
| `32bd8ee0` `32bd8f00` `32bd8f60` `32bd9080` `32bd90e0` `32bd9140` `32bd9c10` | the seven `Init`s (pull, fire, X X, dash-jump X, X X, landing, Head Shot) | = March (same preloads) |
| `32be0200` `32bf9100` `32bf92d0` | pull-sub-weapon EventProcess / FrameMove / End | = March (slot 1) |
| `32be0270` `32bf92f0` | X-front fire EventProcess / End | = March (1.02 Z window, cancels 1.2 / 0.43; the ground tests repeat the ground `else`) |
| `32c181f0` | X-front fire FrameMove | differs: three shots (0.01 / 0.36 / 0.72) at `FlushMp( 4 x Open Mind rate )`, power x `+0x98ac`, head shot through the CEffect `SetExtraDamageMiniGun`, per-frame simulation |
| `32be0630` `32bf9310` `32bf9770` | X-front X X EventProcess / FrameMove / End | = March (0.9 cancels; 0.44 cannon shot; slot 3) |
| `32be0c30` `32be0cb0` | dash-jump X / X X EventProcess | = March (landing, X flag, 0.4 window, count < 4, Down speed x 2) |
| `32bf9f50` | landing EventProcess | = March |
| `32c18f30` | dash-jump X FrameMove | differs: two mini guns (left `Mesh_CSI_CSG_COMBO_DASH_JUMP_X_Mini_Gun`, right `Mesh_CSI_CDC_COMBO_Mini_Gun_R`), attach with `SetPos`, shots 0.207 (left `Dummy01`) / 0.338 (right `Gun_R_Dummy03`) at 4 x rate, x `+0x98ac`, head shot |
| `32c19710` | dash-jump X X FrameMove | differs: CDC left / right mini guns, shots 0.013 / 0.127 (`Gun_L/R_Dummy03`), only the right one counts, a short shot stores 4 (JUNE_BUGS 493) |
| `32bf9c70` | landing FrameMove | differs: the landing mesh in both hands, no per-frame flag, no `SetPos` |
| `32bf9c30` | dash-jump X / X X / landing End | differs: one function frees both hand slots (`CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_MINI_GUN_L` / `_R`, June 7 / 6) |
| `32be2ad0` | Head Shot EventProcess | differs: skill / walk / dash cancels at 0.933 need `bFootOnLine` (no outer ground branch), the arrow cancel does not |
| `32bfcc80` | Head Shot FrameMove (inside) | differs: transcendence memo of the current skill == 26225 adds 0.15 to the shell's head shot rate; `32007c40( SA_EFFECTIVE_RATE )` chance fires a second shell at +-30 / +10; no CHUNG_MEMO8 shell |
| `32bfc860` | `CSI_SI_A_CSG_HEAD_SHOT_DefaultFrameMove` (new) | the frame move InitState registers for state 236 (memo of 26208 -> 26220, no second shell); the equipped skill replaces it with `32bfcc80`. CH1m registers it (`CH1m.md` amended) |
| `32bdb7f0` `32bea970` `32bee360` `32c0ac40` | `CSGComboDashJumpZXReadyStateStart` / `...ReadyEventProcess` / `...EventProcess` / `...FrameMove` (new) | ready: CommonStateStart + loop count 0; ZX: ground -> `USI_DASH_JUMP_LANDING`, animation end -> again while (full hyper mode or a cannon ball) and count < 6; FrameMove: at 0.1 a cannon ball thrown ahead (random z flip, z - 45, `Bip01` +- dir x 35, effect set, `UseCannon( 1 )`, `STR_Kwang`) or a misfire (count 6), then count + 1 (JUNE_BUGS 494) |

**What the headers gained** (all under existing flags): `CSI_CSG_COMBO_DASH_JUMP_ZX` (script id 334, appended to the `_CORE` state block), `CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_MINI_GUN_L` / `_R` (`_CORE`), 5 declarations, and in `X2SkillTree.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES` `RNSI_A_CSG_HEAD_SHOT` 26208, `_TRANS` 26220, `_TRANS_UPGRADE` 26225 (June `Enum.src.lua`; March's own `SI_A_CSG_HEAD_SHOT` is 6214). A file-local `TryGetAcquiredSkillAbilityValue` (June `32007c40`, same body as Eve's) sits in `_CONST_CHUNG_CH1g2_` under `_CORE`.

**Unsure, for the review**: the cannon-ball FrameMove's random block (`32c0ac40` @32c0ad0c-@32c0ad4b) was read from the stack traffic: the `% 10` roll is never used (kept for the random state), and which of `z` / `-z` gets the `- 45` is the part to re-read. The mini-gun slot numbering stays a CH1k hand-off.

**Routed, not ported**: `EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE` has no June definition (JUNE_BUGS 496, script data). The Head Shot memo MP/skill readers are CH4c. `+0x98ac` writer: CH4a. Registration: `CH1m.md`.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h` (patience diff): `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. `flagcheck JUNE_CHAR_CHUNG_CORE`: `RESULT: PASS`; `JUNE_ENUM_NAMES` (X2SkillTree.h): `RESULT: PASS`. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,879,552 B, dated 2026-10-31 22:55:46, md5 2b64b17ff322d45ac51bdfcd4a4cfb6e, equal to X2/US_SERVICE/x2.exe (2026-10-31 22:55:39)`; built at main `bf1f5c44` + the code files of this phase (X2Lib, X2 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1g2 review` (Opus 5.5 high), then CH1h.

### CH1g2 - Review (2026-11-01, Opus 5.5)

**Verdict: one defect, fixed (one line, `_CORE`). Everything else matches June.** I read all 30 functions plus `32bfc860` against `charport_adis.py chung --phase CH1g2`, with each listing cut at the next class-map address.
- **Fixed: the dash-jump Z X cannon ball tilt (`32c0ac40`).** The `% 10` roll *is* read. `GetRotateDegree()`'s z goes to `[esp+0x1c]` @32c0ad04. The roll's float is stored over that same slot @32c0ad2a (`fstp [esp+0x20]` after one `push 0`). So the angle is z = ±(`GetRandomInt() % 10`) − 45, with x and y from `GetRotateDegree()`, and the same vector is passed as both angle and move angle. The transcription negated the rotation's z and left the roll in an unused local. Now `vAngle.z = (float)( GetRandomInt() % 10 )` sits before the `GetRandomFloat() < 0.5` flip. This resolves the CH1g2 "unsure" item. The rest of the block matches: `Bip01` ± `GetDirVector()` × 35, all 23 `CreateInstance` arguments at their defaults, the effect set, `UseCannon( 1 )`, `STR_Kwang`, and misfire 6 then + 1 (JUNE_BUGS 494 stands).
- **Dash-jump slots.** The X `32c18f30`, X X `32c19710` and landing `32bf9c70` FrameMoves call `SetHandleChungMajorMeshByEnum` only with 7 (left) and 6 (right). The End `32bf9c30` frees exactly those two. X: `+0x3f0` `SetPos` on both hands, `+0x19c` per-frame simulation, z set (not added) to −35, shots 0.207 `Dummy01` / 0.338 `Gun_R_Dummy03`, both through one tail (head shot @32c196d5, Fire1). X X: 0.013 `Gun_L_Dummy03` / 0.127 `Gun_R_Dummy03`, Fire2, only the right shot `inc [+0x96c0]`, and both short-of-MP paths share the store of 4 @32c19cba (JUNE_BUGS 493). Landing: one mesh in both hands, no `+0x19c`, no `+0x3f0`.
- **X-front fire `32c181f0`.** `+0x9718` is set at 0.01, and the shots at 0.01 / 0.36 / 0.72 use `4 × Open Mind`, `× +0x98ac`, and `+0x96b8` as the 11th argument. The 0.01 shell jumps into the 0.72 tail (@32c1858f → @32c188fe), so it gets the head shot @32c18911 and Fire1. Two call sites cover three shells, and the result is Fire1 / Fire2 / Fire1 as transcribed.
- **Head Shot.** EventProcess `32be2ad0`: walk (`pureRight` or `pureLeft`, plain) and dash both need `bFootOnLine` at > 0.933. The skill cancel is the `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` macro, and the arrow cancel → `USI_WALK` is ungated. FrameMove `32bfcc80`: there is no `+0x9718` write, so dropping March's `m_bIncreaseCritical` is right, and there is no MEMO8 shell. Memo `31fdbcc0( vslot 0x524 )` == 0x6671 (26225) adds 0.15 to both shells. The chance test `fld f; fcompp; test ah,1` skips when f < rand, so `GetRandomFloat() <= f` is correct. `32007c40( 5, skill, 1, &f )`. The second shell sits at x ± 30, y + 10. `32bfc860` uses memo of 0x6660 == 0x666c. The values 26208 / 26220 / 26225 match `Enum.src.lua`:14970 / 15248 / 15460.
- **New Z X ready / loop** (`32bdb7f0`, `32bea970`, `32bee360`): `+0x9894` = 0, ground → `USI_DASH_JUMP_LANDING`, animation end → Z X while (full hyper or cannon > 0) and count < 6. All match.
- **Claimed `= March` spot-checks pass.** `32be0270`: 1.02 / `oneZ`, `JUMP_DOWN` −45, a 1.2 chain (skill / walk / dash / arrow ASDC), then a 0.43 skill / dash chain, and any cancel jumps to the animation-end test, the same as March's `else if` chain. `32bf9310`: play count < 5, cannon ≥ 1 or full hyper, effect set + `UseCannon( 1 )`, or misfire. `32bf9f50`: Z / X (class 111 → CDC X) / dash / walk / up +30 / hyper / down −45 / skip keys. The repeated `bFootOnLine` tests inside the ground `else` are redundant. `m_FrameDataFuture.unitCondition.bFootOnLine` is written only by the physics pass (`X2GUUser.cpp`:25543), never by `StateChange`.

**Gates.** CH1g2 range `de236d44^..de236d44` (patience): `_LIVE`+`_CORE`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL`, `0 deleted line(s) … 0 encoding change(s)`, FAIL only for the commented-out `#define`; `_CORE`: `RESULT: PASS`; `JUNE_ENUM_NAMES`: `RESULT: PASS`. The fix: `flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp`: `PASS +1 -1`, `RESULT: PASS`. The one deleted line is CH1g2's own inserted line, so against base the file is still insert-only. `junediverge.py --check-all`: `RESULT: PASS` (160 allow-listed); `--check X2Lib/X2GUChung.cpp`: `RESULT: PASS`. `enumscan.py --check`: 0 A/B rows. One-file probes: `_LIVE` off rc 0, 0 errors; `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` rc 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,879,552 B, dated 2026-11-01 02:44:11, md5 89a59628b368312d96f7180c3192a7e7, equal to X2/US_SERVICE/x2.exe (2026-11-01 02:44:04); built at main de236d44 + 1 uncommitted code file(s)` (the fix). There is no `exestr` line because the fix adds no string.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-chung CH1h` (Sonnet 5.5 high).

### CH1h - Done (2026-11-01)

All 22 class-map rows of phase CH1h are written, all `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`); no `new` row, no new member, enumerator, state id or `Always.h` define. `_LIVE` stays `//#define`. 22 funcmap rows `Char Chung CH1h`. Read from `charport_adis.py chung --phase CH1h` (listings cut at the next non-`dead` class-map address).

| June | `CX2GUChung::CDCCombo...` | vs March |
|---|---|---|
| `32bd9ee0` `32bd9f40` `32bd9f70` `32bd9fd0` `32bda020` `32bda080` | `XInit` `XXInit` `XZInit` `XZZInit` `ZZXXInit` `ZZXXXInit` | = March (7 / 3 / 7 / 5 / 7 / 13 preloads, same names) |
| `32be30c0` `32be31f0` `32be3320` | `XEventProcess` `XXEventProcess` `XZEventProcess` | = March (0.865 to X X / X Z; walk cancel clears both flags, 1.282 / 1.584 window to X X Z / X X X; 1.3 input / 1.458 branch to X Z Z, cancels 1.074 / 1.495, then `m_fDashCancelAfter` / `m_fSkillCancelAfter`) |
| `32be3730` `32be3960` `32be3a30` `32be3cb0` | `XZZEventProcess` `ZZXEventProcess` `ZZXXEventProcess` `ZZXXXEventProcess` | = March (skill 0.9 / arrow 1.44, dash cancel only on X Z Z; 0.521 to Z Z X X; 1.5 / 1.9 to Z Z X X X with the walk / dash / skill members) |
| `32bfd690` `32bfd6b0` `32bfda10` | `XEnd` `XZEnd` `ZZXXEnd` | = March (frees June slots 9 / 10 + 11 / 13 + 14) |
| `32bfd6f0` `32bfda50` `32c1ba00` | `XZZFrameMove` `ZZXXXFrameMove` `XXFrameMove` | = March (0.666 and 0.594 cannon ball or misfire, the latter with `GetPowerRate()` in the effect set; 1.232 `DAMAGE_DATA_NEXT` table, `UpCannonBallCountAtThisAnimationTime( 1.232f, 1 )`) |
| `32c19f30` | `XFrameMove` | differs: the 0.599 shot costs 4 x the Open Mind rate (`RNSI_P_CDC_OPEN_MIND`), power x `m_fRemodelRifleDamageRate` (`+0x98ac`), head shot through the CEffect `SetExtraDamageMiniGun` (`32c15990`) |
| `32c1a480` | `XZFrameMove` | differs: shots 0.632 (left) / 0.965 (right) / 1.332 (left), each 4 x rate, x `+0x98ac`, head shot |
| `32c1ae60` | `ZZXXFrameMove` | differs: 5 left shots (0.6 step 0.25), 4 right (0.67 step 0.25) and the 1.67 last shell, each 2 x rate, x `+0x98ac`, head shot; a short shot passes 23 as the effect's angle z |

No function of the phase reads `+0x7cc8` (`m_fNowStatePowerRate`, checklist §7 L253): nothing to scale here. The head shot of `32c15990` is on the three FrameMoves above (CH1g review amendment); the `+0x9718` write at 0.599 / 0.001 stays behind March's own `SKILL_BALANCE_20110728` as in CH1g2. `UpCannonBallCountAtThisAnimationTime` is March's, called by name (its June body `32bf3d30` is CH1q's, as CH1d).

**Mini gun handle slots.** June frees slot 9 (X), 10 / 11 (X Z), 13 / 14 (Z Z X X); March's enumerators for the same guns are one higher (10, 11, 12, 14, 15). The bodies use March's names with the June slot in the comment; `CH1k.md` is amended to settle the array against June's numbering.

**Checked against the preprocessed file.** `cl /P` with `_LIVE` off and on, 22 bodies compared token-wise: 17 identical, 2 differ only by an unused local (`iExtraChargeCannonBall`, X X FrameMove) or a bare block (Z Z X X EventProcess), 3 differ exactly as the table says. No JUNE_BUGS row: nothing in the 22 functions is a June mistake (the redundant ground tests inside the `IsOnSomethingFuture` else are treated as in CH1g2).

**Gates.** `GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. `junediverge.py --check X2Lib/X2GUChung.cpp`: `RESULT: PASS` (allow-listed 2). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,879,552 B, dated 2026-11-01 03:13:10, md5 dd88ab2a63d4b2474aabfb06abba8b39, equal to X2/US_SERVICE/x2.exe (2026-11-01 03:13:03); built at main 2a9098bb + 1 uncommitted code file(s)` (X2Lib 0 errors). No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1h review` (Opus 5.5 high), then CH1i.

### CH1h - Review (2026-11-01, Opus 5.5)

**Verdict: the code passes and no source changed.** I read the kickoff's functions against `charport_adis.py chung --phase CH1h`, each listing cut at the next class-map address.
- **X FrameMove `32c19f30`.** The 0.001 block creates `Mesh_CSI_CDC_COMBO_X_Mini_Gun` with `+0x19c` = 1 and writes slot 9 (@32c1a039). `+0x95ec` is read for the hand matrix and for `Gun_L_Dummy03`. 0.466 creates `Mini_Gun_Hit` at `Bip01_L_Hand` with `GetPowerRate()`. At 0.599, `+0x9718` = 1 is set before the MP test (@32c1a29e). The Open Mind rate comes from `31fceab0( 0x6731 )` > 0, then `32007b10( 0x6731, 0x1b )`. The shot then flushes 4 × the rate (vslot 0xe4, second argument 0) and fires `Shell_Big` at the gun position with power `GetPowerRate()` × decrypted `+0x98ac`. Its 11th argument is `+0x96b8`. After it come the head shot `32c15990( effect, +0x97d0 )` and Fire1. The short-of-MP effect is at the gun position (`[esp+0x24]` / `[esp+0x28]` / `ebx`), with 0, 0 and the rotation's z.
- **X Z FrameMove `32c1a480`.** `+0x9718` is set in the 0.001 block. Slots are 0xa left and 0xb right, read back as `+0x95f0` / `+0x95f4` (`Gun_L_Dummy03` / `Gun_R_Dummy03`). The left gun position is `[esp+0x18..0x20]` and the right is `[esp+0x30]` / `[esp+0x34]` / `ebx`. The shot reads `+0x44` above them, so 0.632 fires left (@32c1a9ca), 0.965 right (@32c1abc3) and 1.332 left (@32c1ada7). Each misfire uses the same gun as its shot. All three are `Shell`, 4 × rate, × `+0x98ac`, head shot, Fire1.
- **Z Z X X FrameMove `32c1ae60`.** Slots are 0xd and 0xe, `+0x95fc` / `+0x9600`. The left gun is at `[esp+0x34]` / `[esp+0x38]` / `ebx` and the right at `[esp+0x1c..0x24]`. The loops are 0.6 step 0.25 while `<= (double)1.62f` (5 left shots) and 0.67 step 0.25 while `<= 1.43` (a double literal, 4 right shots), then the 1.67 `Last_Shell` on the right. Each costs 2 × rate (`fadd st0, st0`), × `+0x98ac`, head shot, and passes 23.0 as the misfire's z. The right loop and the last shell call `31f7ff60`, the common entry that `31f8ff00` forwards to, with the same arguments. The unused `GetRotateDegree()` @32c1b1bf is March's `vDegree_MiniGun_Shell` (z forced to 23), so the literal 23.f is the same.
- **`= March` EventProcesses pass line by line.** X Z `32be3320`: `oneZ` from 1.3, then `CSI_CDC_COMBO_XZZ` from 1.458. Then one chain: skill > 1.074, walk > 1.495 → `USI_WAIT`, dash > 1.074, arrow ASDC > 1.074 → `USI_WALK`, animation end. Last come `m_fDashCancelAfter` (`+0x8c9c`) and `m_fSkillCancelAfter` (`+0x8c90`), in the same `else if` chain. Z Z X X `32be3a30`: `oneX` from 1.5, then from 1.9 with the flag → `CSI_CDC_COMBO_ZZXXX`. After the animation-end test come the walk (`+0x8c98`), dash (`+0x8c9c`) and skill (`+0x8c90`) member cancels. X X `32be31f0`: the walk cancel clears both flags with one word store (@32be3217). In 1.282 ≤ t ≤ 1.584, Z wins over X. When t > 1.584, the Z flag goes to `CSI_COMBO_XXZ` (0xb2), else the X flag to `CSI_COMBO_XXX` (0xb1).
- **Mini gun slots.** The Ends free 9 (`32bfd690`), 10 / 11 (`32bfd6b0`) and 13 / 14 (`32bfda10`). No CH1h listing names slot 12 or `+0x95f8`. The `CH1k.md` amendment is right.
- **X X FrameMove `32c1ba00`.** At 1.232: `BeginTableByReference`, `SimpleInit`, `+0x3a4` = 0, `optrAttackerGameUnit` = this, `SetDamageData( 0x3346f050 )`, `EndTable`. Then `32bf3d30( 1.232f, 1 )` is called every frame, the same as March's `UpCannonBallCountAtThisAnimationTime( 1.232f, 1 )`.
- The CEffect overload `SetExtraDamageMiniGun` returns on NULL, so the unchecked `CreateInstance` results are safe.

**Gates.** CH1h range `3cdb218b^..3cdb218b` (patience), `_LIVE --with _CORE`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; the only FAIL is the commented-out `#define`. `junediverge.py --check-all`: `RESULT: PASS` (160 allow-listed). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows. One-file probes (`/Zs /EHsc`, no PCH): `_LIVE` off exit 0, 0 errors; `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0, 0 errors. No build: nothing changed, so CH1h's `DEPLOY OK` md5 dd88ab2a stands.

**Nothing to play-test** (`_LIVE` is off). `CH1i.md` checked, no amendment needed. Next: `/x2-phase char-chung CH1i` (Sonnet 5.5 high).

### CH1i - Done (2026-11-01)

All 15 class-map rows of phase CH1i are written: 14 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`) and 1 `new` (`CHG_ComboDashJumpZXXFrameMove`, June `32c11ad0`, under `_CORE`, declared in `X2GUChung.h`, referenced by nothing; CH1m registers it). One new enumerator, `CSI_CHG_DASH_JUMP_COMBO_ZXX` (June script id 327, `Enum.src.lua` `USI_END + 175`), appended to the `_CORE` block of the state enum. `_LIVE` stays `//#define`; no new `Always.h` define. 15 funcmap rows `Char Chung CH1i`. Read from `charport_adis.py chung --phase CH1i`.

| June | `CX2GUChung::` | vs March |
|---|---|---|
| `32be52a0` `CHG_ComboXEventProcess` | differs: the Z (to `CSI_CHG_COMBO_XZ`, flag [0], `m_fEventTime[0]`) and X (to `CSI_CHG_COMBO_XX`, flag [1], `m_fEventTime[1]`) flags also fire when the future animation ends, not only past `stateChange`; walk cancel `m_fWalkCancelAfter` |
| `32be54f0` `CHG_ComboDashZEventProcess` | differs: the Z flag (to `CSI_CHG_DASH_COMBO_ZZ`) also fires at the animation end |
| `32be5680` `CHG_ComboDashZZEventProcess` | differs: the X flag (to `CSI_CHG_DASH_COMBO_ZZX`) also fires at the animation end; walk / dash / skill cancels (`+0x8c98` / `+0x8c9c` / `+0x8c90`) as March |
| `32be5960` `CHG_ComboDashJumpZXEventProcess` | differs: the animation end is tested first (foot: `USI_WAIT`, else `USI_JUMP_DOWN` and y - 45); else X in the `m_fEventTime[0]` window sets the flag and it branches to `CSI_CHG_DASH_JUMP_COMBO_ZXX` at `stateChange` or the end (March has no third hit) |
| `32be5fe0` `32bf3270` `32bf3350` `32bf34c0` | = March (`WITHSTANDING_WAIT` / `ComboXX` / `ComboDashZZX` / `WITHSTANDING_SIEGE` EventProcesses: animation end -> `USI_WAIT` or `m_iSiegeShellingCount = 0` + `CSI_SI_A_SIEGE_SHELLING_READYLOOP`; no foot -> `USI_JUMP_DOWN`) |
| `32bf4440` `32bf4470` | = March (`ComboXFrameMove` 0.432 / `ComboDashZZFrameMove` 0.633: `UpCannonBallCountAtThisAnimationTime( t, 1 )`) |
| `32c042c0` `32c048b0` | = March (X X: aim 0.55, shot or misfire 0.666, `CHG_COMO_XX_EXPLOSION` 0.69; dash Z Z X: 0.415 / 0.581 / 0.585 / 0.834) |
| `32c04ed0` | = March (both `WITHSTANDING` states: count up 0.533 with `m_iChargeCannonBall`, `CHG_WITHSTANDING` at 0.364, power x `m_fWithStandingPowerRate`) |
| `32c0e1f0` | = March (dash jump Z X: 0.202 bomb for 3 MP, else the magic-fail particle) |
| `32c11ad0` (new) | byte-identical to `32c0e1f0` except the jump targets: the dash jump Z X X state throws the same bomb at 0.202 |

`CSI_CHG_COMBO_XZ` has no function of its own in June (`ComboXXZEventProcess`, CH1d); `CSI_CHG_DASH_JUMP_COMBO_ZXX` shares `32be8ef0` with `CSI_CSG_COMBO_DASH_ZXX_END` (`CSGComboDashZXXEndEventProcess`, CH1g). No function of the phase reads `+0x7cc8` (checklist §7 L253). The June flag clause `stateChange < now || IsAnimationEnd()` is written out in each body: March's `ELSE_IF_STATE_CHANGE_ON_EX_` macro has no animation-end term, so it is not used in these four. `PlaySound`, `UseCannon`, `UpCannonBallCountAtThisAnimationTime` and `CommonFrameMove` are called by March's names (the June bodies `32276220`, `32bf5280`, `32bf3d30` are other phases'). No JUNE_BUGS row: nothing in the 15 is a June mistake (the `|| IsAnimationEnd()` in the dash jump Z X flag branch is dead after the end test, kept as June has it).

**Checked against the preprocessed file.** `cl /P` with `_LIVE` off and on: `bCanChangeStateComboDashJumpZXX` 0 / 4 hits (the June path is compiled in only with `-D`), `CHG_ComboDashJumpZXXFrameMove` 2 / 2 (declaration + definition in both, `_CORE`).

**Gates.** `GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR` (the `.h` OTHER is the `_CORE` enumerator and declaration; `flagcheck JUNE_CHAR_CHUNG_CORE --files X2Lib/X2GUChung.h`: PASS, `+3`); `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is still commented out. `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `RESULT: PASS` (allow-listed 2). `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,880,576 B, dated 2026-11-01 04:47:17, md5 f97e46e828b5ed276d0c818f1e0e09df, equal to X2/US_SERVICE/x2.exe (2026-11-01 04:47:10); built at main 8267d632 + 2 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1i review` (Opus 5.5 high), then CH1j0.

### CH1i - Review (2026-11-01, Opus 5.5)

**Verdict: the code passes and no source changed.** I read all 15 bodies against `charport_adis.py chung --phase CH1i`, each listing cut at the next class-map address.
- **The four EventProcesses that differ.** Each starts with `+0x2ac` on `m_pXSkinAnimFuture` → `USI_WAIT` (`32be5960`: the foot test inside it, `USI_JUMP_DOWN` with y - 45, else `USI_WAIT`), then a separate foot test. The flag clause is flag `== 1` and (`stateChange` < now, `test ah,5 / jnp`, **or** `+0x2ac != 0`). The key window is now < `keyInputEnd` (`ah,0x41 / jne`) and `keyInputStart` <= now (`ah,0x41 / jp`). `m_fEventTime[0]` = `+0x8c10 / 14 / 18` and `[1]` = `+0x8c1c / 20 / 24`. X `32be52a0`: flag [0] `+0x7d85` with `oneZ` → `0x10b` `CSI_CHG_COMBO_XZ`, then flag [1] `+0x7d86` with `oneX` → `0x10a` `CSI_CHG_COMBO_XX`, then the walk cancel. Dash Z `32be54f0`: [0] `oneZ` → `0x10c`, then walk. Dash Z Z `32be5680`: [0] `oneX` → `0x10d`, then walk `+0x8c98`, dash `+0x8c9c` (vslot `0x668`) and skill `+0x8c90` (`SKILL_CANCEL_AFTER`'s expansion with black hole, `oneLCtr` and hyper), one `else if` chain (each change jumps to `CommonEventProcess`). Dash jump Z X `32be5960`: [0] `oneX` → `0x147` = 327, `USI_END` (152) + 175. Its `|| +0x2ac` is dead after the end test, as the Done note says. The extra `bFootOnLine == 1` in June's walk / dash tests is redundant in the `else` of the foot test. The alternate future animation `+0x8e90 ? +0x8e84 : +0x87d0` is read as `m_pXSkinAnimFuture`, as in every CH1 phase.
- **X X FrameMove `32c042c0`.** 0.55 aims with `DoAimingTarget( 800, ( tan 60°, -tan 10°, 45° ), +0x9690, +0x9694, +0x96a0, false )` (the vector built at `[esp+0x68..0x70]`). Then × 180 / π, and `+0x8a64` = 1 / `+0x8a68` = -deg when the degree is >= 0.001 or <= -0.001. 0.666: aiming and (`GetGageData()` count `>= 1` (`jge`) or vslot `0x9c` `IsFullHyperMode`) → `AIMMING_SHOT` at the rotation's z, else `_MISFIRE`, aim and bone cleared, `Chung_CannonGun_Trigger.ogg`. Then `UseCannon( 1 )` (`32bf5280`) when `GetRandomFloat()` > `+0x9730` (`fcomp` / `ah,0x41 / jne`) = `m_fSiegeShellingUserBallRate < GetRandomFloat()`. 0.69: `CHG_COMO_XX_EXPLOSION` with power × `+0x972c` when aiming, then the bone is cleared (@32c04865) either way.
- **Dash Z Z X FrameMove `32c048b0`.** Same shape at 0.415 / 0.581 / 0.585, with the bone cleared only at 0.834 (@32c04e8b). The misfire clears it too (@32c04d0b). 0.415 and 0.834 compare `+0x24c` to the double-widened float, which is still `EventTimerOneshot`. Both FrameMoves equal March's `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch.
- **Other `= March` bodies.** `32be5fe0` (end → `USI_WAIT`, then foot). `32bf3270` (foot first, else end → `m_iSiegeShellingCount = 0`, `0x110`). `32bf3350` / `32bf34c0` (end → count 0 + `0x110`, then foot). `32bf4440` / `32bf4470` (`32bf3d30( 0.432f / 0.633f, 1 )`). `32c04ed0` (`32bf3d30( 0.533f, +0x9748 )`, 0.364 `CHG_WITHSTANDING` at y + 100, × `+0x9744`). `32c0e1f0`: 0.202, `FlushMp( 3.0 )` → `CHG_DASHJUM_ZX_BOMB` from `Dummy2_Lhand`'s y and `AddRemoveDamageEffect`. Otherwise the magic-fail particle at the clone frame with rotation z - 50, emit 100 / 100, trigger 1, axis = add rotate = the angle.
- **New row `32c11ad0`.** A normalized diff against `32c0e1f0` differs only in the 13 jump targets. `32be8ef0` is the class map's shared EventProcess for `CSI_CHG_DASH_JUMP_COMBO_ZXX` and `CSI_CSG_COMBO_DASH_ZXX_END` (CH1g's `CSGComboDashZXXEndEventProcess`). The C++ enumerator takes its value from the `_CORE` append order, and CH1m reconciles the ids.
- No CH1i listing reads `+0x7cc8` (checklist §7 L253). No drift.

**Gates.** CH1i range `ec500f11^..ec500f11` (patience), `_LIVE --with _CORE`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`. The `.h` OTHER is `_CORE`. `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. The only FAIL is the commented-out `#define`. `junediverge.py --check-all`: `RESULT: PASS` (160 allow-listed). `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: 0 A/B rows. One-file probes (`/Zs /EHsc`, no PCH): `_LIVE` off exit 0, 0 errors; `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` exit 0, 0 errors. No build: nothing changed, so CH1i's `DEPLOY OK` md5 f97e46e8 stands.

**Nothing to play-test** (`_LIVE` is off). `CH1j0.md` checked, no amendment needed. Next: `/x2-phase char-chung CH1j0` (Opus 5.5 high).

### CH1j0 - Done (2026-11-01)

The three helpers CH1j needs are `CX2GUUser` code, ported whole under three shared flags (compiled, live, inert): nothing fills the new
map until Chung's spine aim runs. funcmap rows `Char Chung CH1j0` (6). JUNE_BUGS 497, 498.

| June | March home | What it is |
|---|---|---|
| `31fe9840` | `CX2GUUser::SetRotateBone( bEnable, wstrBoneName, vAxis, vDegree )` | `m_mapRotateBone[name] = RotateBoneData( ... )`; RET 0x38 |
| `31fe4870` | `CX2GUUser::EraseRotateBone( wstrBoneName )` | `m_mapRotateBone.erase( name )`; RET 0x1c |
| `32008660` | `CX2GUUser::GetNearestTargetAimDegree( fMaxUpDegree, fMaxDownDegree, fYawRangeDegree, fMaxDistance )` | the aim angles to the nearest enemy in range; vec3 by hidden pointer, RET 0x14 |

**The bone map** (flag `SERV_IRUHADEV_JUNE_USER_BONE_ROTATION`). June added `std::map<wstring, RotateBoneData>` at `+0x8a6c`, right after
March's `m_bRotateBone` / `m_fRotateBoneDegree` (`+0x8a64` / `+0x8a68`). The value is 0x38 bytes: bool, bone name, axis vec3 and degree vec3
(ctor `320799f0`, Set `32079aa0`). Two more June functions make the map do anything, so they came with it:
- `ApplyRotateBoneMap` (`31fdb4e0`): called by `OnFrameMove` `@320657e4`, after March's `m_bRotateBone` block and before the
  `m_ViewEqipList` loop. For each enabled, named entry it does `GetCloneFrame` and then x, y, z in turn: when `FLT_EPSILON` < |axis| and
  |degree|, `RotateBone( frame, axis on that axis only, degree )`.
- `CommonStateStart` (`32057b80 @32057e01`) clears the map after `m_bRotateBone = false`.
Chung's callers (`32befec0` / `32bf00c0`) pass `Bip01_Spine1`, axis (-1, -1, 0), degree (yaw gap, -pitch, 0), and erase the entry when both
aim angles are within `FLT_EPSILON` of 0.

**`RotateBone` diverges** (`31fd93e0`, flag `SERV_IRUHADEV_JUNE_DIVERGE_ROTATE_BONE`, a `replace` row in `june_march_divergent.tsv`). It
matches March except:
- a frame named `Dummy4_LWeapon01` multiplies onto `m_pMotion->FindFrame( "Bip01_L_Finger1" )->combineMatrix`, with no NULL test (JUNE_BUGS
  498: the port returns instead of crashing);
- it repeats the child update on the second body `+0x8e88`. That is Lu/Ciel's dual body, NULL for Chung, and left as a stand-in comment
  (same treatment as CH1d0).
March's one caller (Chung's `m_bRotateBone` on `Bip01_Spine1`) gets identical results.

**The aim helper** (flag `SERV_IRUHADEV_JUNE_USER_AIM_TARGET`, needs `_DAMAGEDATA` and `_CHAR_ARME_CORE`, each guarded by `#error`).
`32008660` walks `g_pX2Game`'s units from my centre (vslot 0x2c8: `GetPos(false)` plus half the height for a user). It keeps a unit that:
- is valid, on another team, and has HP > 0;
- if an NPC: is `NCT_BASIC`, is not `GetInvincible`, and gets a hit position from `31fd5ef0`; if a user: passes
  `IsPosAlongLastTouchLine == GetIsRight`;
- is no further than the nearest so far (starting at `fMaxDistance`);
- has `GetDirVecToDegree` z in [-down, up] and a yaw gap within ±range x 0.5.
It returns those angles; constants (qword) 0 / 360 / 180 / 0.5. The negative-yaw fold is `360 - y` (JUNE_BUGS 497). `31fd5ef0` needed a
`CX2GUUser` home: `CX2GUUser::GetTargetHitPos` is the body Buff 9a wrote as a file-static in `X2BuffTemplet.cpp` (re-read in Ghidra, same
reading), written on `CX2GameUnit::GetNearestCollisionFrameName` / `IsPosAlongLastTouchLine`. The buff static stays untouched.

**For CH1j:** `ApplySpineAimRotation` (`32bf00c0`) calls `GetNearestTargetAimDegree`, then `SetRotateBone` / `EraseRotateBone`
(`L"Bip01_Spine1"`), and finally sets `m_fRotateBoneDegree` = the pitch. `kickoffs/char-chung/CH1j.md` "Amended by CH1j0" lists the symbols.
`32befec0` (CH1l) uses the same three.

**Branches seen, not ported:** the other June callers of the map and aim helpers (`32aac830`, `32aac940`, `32ab4a70`, `32b15680`,
`32bc0e30`, `32c863d0`, `32c86620`, `32ca2520`, `32aef530`, `32af59c0`) belong to their own classes' ports and need nothing more here.

**Gates.**
- `flagcheck USER_BONE_ROTATION --with USER_AIM_TARGET DIVERGE_ROTATE_BONE --worktree` (patience): `files: 1 PASS, 1 PASS~, 1 DEFINE,
  0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s)`, `RESULT: PASS`. The
  PASS~ is the blank line after the new `.cpp` block.
- `junediverge.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h`: `RESULT: PASS` (allow-listed 4). `--open-rows` does not list `RotateBone`.
- `enumscan.py --check`: the two rows left are `X2GUUser.h:2550-2551` (`NUI_NEIL_THE_SNIPER_EVENT` / `NUI_PROTON_BOMB_EVENT`). They are old
  and untouched; the new code has none.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUUser.cpp` exit 0, 0 errors; `X2GUChung.cpp` exit 0, 0 errors. A flag
  probe shows all three flags on.
- `DEPLOY OK: X2_offline.exe is 25,887,232 B, dated 2026-11-01 05:33:35, md5 d78cfd9fb8b09a7f74dc1dc4a169b65d, equal to X2/US_SERVICE/x2.exe (2026-11-01 05:33:28); built at main 5256502e + 3 uncommitted code file(s).`
  The build started before the `#ifdef SERV_IRUHADEV_JUNE_USER_AIM_TARGET` wrap around the `Always.h` guards; that wrap is preprocessor-only and changes nothing while the flags are on.
- No `exestr` line: there is no new string to show.

Nothing to play-test (the map stays empty and the aim helper is unreferenced). Next: `/x2-phase char-chung CH1j0 review` (Opus 5.5 high).

### CH1j0 - Review (2026-11-01, Opus 5.5)

**Verdict: the code passes and no source changed.** I read all six June bodies in Ghidra, both asm and decompile.
- **`32008660` -> `GetNearestTargetAimDegree`.** The skip order matches the port: `IsValidUnit` (`322ccf50`), then team `+0x278`, then HP. For HP,
  gage `+0xc04` NULL passes, hp gage `+0x20` NULL skips, and a unit passes only when `0 <` hp. March's `GetNowHp()` gives the same answers (1 with
  no gage, 0 with no hp gage). Next is my centre / its centre (vslot `0x2c8`). For an NPC (`+0x264 == 1`): `+0x85f8 +4` (`m_ClassType`) `!= 0`,
  vslot `0x180` (`GetInvincible()`, slot 96) and `31fd5ef0( npc, 1, 1, &centre )` skip it. Any other type takes the
  `32293b80( centre ) == vslot 0x170` test. Arguments: `[esp+0x5c]` = up, `+0x60` = down, `+0x64` = yaw range, `+0x68` = max distance.
  Out = the `31c19930` angles. Every x87 compare equals the port's C++ `<`, NaN included:
  distance `test 0x41 / jz` (NaN passes), pitch `-down` and `up` (`test 5 / jnp`, NaN passes), both yaw folds `test 5 / jp` (`360 - y` only for
  y < 0, JUNE_BUGS 497 at @32008920 / @32008955), the 180 fold `test 0x41 / jnz`, and the ±range x 0.5 window (NaN passes). Constants 0 / 360 / 180 / 0.5
  (`charport_consts.py`).
- **`31fe9840` / `31fe4870` / `31fdb4e0`.** Value = bool `+0`, name `+4`, axis `+0x20`, degree `+0x2c` (ctor `320799f0` and Set `32079aa0`
  both write all four), which puts the map node's value at `+0x28`. Erase = `equal_range` + erase. Apply skips a disabled entry and a name equal to `L""`.
  Each axis needs `FLT_EPSILON < |axis.c|` (`test 5 / jp`) and `|degree.c| > FLT_EPSILON` (`test 0x41 / jnz`); NaN skips both, as in the port. The calls build
  (ax,0,0) / (0,ay,0) / (0,0,az). Call site: the join of both `m_bRotateBone` exits (`jnz` @32065745, `jz` @32065799 -> @320657e2) =
  March's `CommonFrameMove` line after that block. Clear: `3207c850` on `+0x8a6c` right after `+0x8a64 = 0` (`CommonStateStart`).
- **`31fd93e0` -> `RotateBone`.** Guards `+0x87cc` / `+0x87d4` / `fucompp test 0x44 / jnp` (= `0.f ==`, NaN goes on). Radians = qword
  0.01745329238 = `(float)( D3DX_PI / 180.f )`. `std::string == "Dummy4_LWeapon01"`; `FindFrame( "Bip01_L_Finger1" )` and the child
  `FindFrame` both on `+0x87cc` (`m_pMotion`); the multiply is rotation x that frame's `+0x90`, copied into `pFrame +0x90`. The second
  `UpdateAnimationFrame_Public` has ECX = `+0x8e88`. The `#else` is March's body, unchanged (0 deleted lines).
- **`31fd5ef0` -> `GetTargetHitPos`.** It matches the buff static line by line (700 qword, `test 0x41 / jnz`, my-bone `< target` `test 5 / jp`,
  modes 0 / 1 / other). **The target-centre branch is right for an NPC:** NPC vslot `0x434` = `327574e0` = GetPos( false ) + vslot
  `0x18c`( false ) x 0.5 on y, the same body as the user's `0x2c8` (`31fc9380`). The user's slot `0x434` is `32034c00`, a state-change-like
  function, not a position. All three June callers (`32008660`, `32008a20`, `3236e2a0`) reach it with an NPC only (`+0x264 == 1` before
  the call), so the port computing a centre for a user target is unreachable. Noted, no JUNE_BUGS row.

**Lu/Ciel material found (routed, checklist §7 row after `3208e160`):** `ApplyRotateBoneMap` takes its frame from `32071d80`
(`+0x8e90 ? +0x8e88 : +0x87d4`). June's `m_bRotateBone` block also rotates `Bip01_Spine1` on `+0x8e88` (@32065792-320657dd). Add to that
RotateBone's second child update. Chung's second pair is NULL (CH1d0), so all three read as March's single body here.

**CH1j kickoff:** "Amended by CH1j0" checked against `32bf00c0`. The arguments are `+0x9878` / `+0x987c` / `+0x9880` / `+0x9884 x +0x97a4`, the result
goes to `+0x9888`, Set fires when either |y| or |z| > `FLT_EPSILON` (else Erase), and `+0x8a68` = z. Two fixes: `CommonFrameMove`, not
`OnFrameMove`; degree x = `GetRotateDegree().y - aim.y`, with June's folds (>= 360 -> 360 - v, <= -360 -> v + 360), axis (-1, -1, 0).

**Gates.**
- `flagcheck JUNE_USER_BONE_ROTATION --range 7da8b1f1^..7da8b1f1 --with JUNE_USER_AIM_TARGET JUNE_DIVERGE_ROTATE_BONE` (patience):
  `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted
  line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`.
- `enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h`: the old `X2GUUser.h:2550-2551` rows only (09107672, untouched).
- `X2GUUser.cpp` one-file probe (`census.cl_cmd`, `/c /Zs /EHsc`): exit 0, 0 errors.
- No build (CH1j0 `DEPLOY OK` md5 `d78cfd9f` stands; nothing changed in source).

**Nothing to play-test.** Next: `/x2-phase char-chung CH1j` (Sonnet 5.5 high).

### CH1j - Done (2026-11-01)

All 41 class-map rows of phase CH1j are written: 36 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`) and 5 `new` under `_CORE` (declared in `X2GUChung.h`, referenced by nothing; CH1m registers). `_LIVE` stays `//#define`. 41 funcmap rows `Char Chung CH1j`. Read from `charport_adis.py chung --phase CH1j`. Shared bits: `RNSI_P_CTT_OPEN_MIND = 26419` (`X2SkillTree.h`, `JUNE_ENUM_NAMES`); under `_LIVE` a macro `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` (June's flag clause, as CH1i wrote out) and `_CONST_CHUNG_CH1J_::MOBILE_MODE_X_COOLTIME`; under `_CORE` five members (`m_fSpineAimMaxUpDegree` +0x9878, `...MaxDownDegree` +0x987c, `...YawRangeDegree` +0x9880, `...MaxDistance` +0x9884, `m_vSpineAimDegree` +0x9888; their writer, the constructor, is CH1k) and `enum CANNON_BULLET_TYPE`.

| June | `CX2GUChung::` | vs March |
|---|---|---|
| `32bece90` / `32bec410` / `32bec950` | `CTT_MobileArmoredModeLoop` / `LoopFoward` / `LoopBackward` EventProcess | differs: MP drain x `SA_MP_CONSUME_REL` of the open mind passive (26419); the four flags (UNREADY, X, forward, backward / loop) also fire at the animation end; `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )` added |
| `32be6fe0` | `...ReadyEventProcess` | differs: the X flag also fires at the animation end |
| `32be6c50` | `CTT_ComboZZXEventProcess` | differs: the Z Z X X flag also fires at the animation end |
| `32bdac90` | `...XStateStartFuture` | differs: X cool time 0.8 (March 1.1) |
| `32bf07b0` | `...LoopFrameMove` | differs: calls `ApplySpineAimRotation` |
| `32c108d0` | `...XFrameMove` | differs: at 0.033333 spine aim, one extra cannon ball, `CreateCannonBulletDamageEffect( type 2, power x +0x97a8 )`; no `DoAimingTarget`, no 0.066666 explosion |
| `32bda860` (new) | `CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD_StateStartFuture` | also the BACKWARD StartFuture; open mind: `passiveSpeed.x` (unit +0x30c) x `SA_MOVE_SPEED` |
| `32bf00c0` (new) | `ApplySpineAimRotation` | `GetNearestTargetAimDegree` -> `m_vSpineAimDegree`; `SetRotateBone` / `EraseRotateBone( Bip01_Spine1 )` as CH1j0 amended; `m_fRotateBoneDegree` = pitch |
| `32c09e40` (new) | `CreateCannonBulletDamageEffect( type, power )` | 0 / 1 / 2 = `..._By_Siege` / `_Finish` / `_By_Mobile_Siege` from `ATTACK_SPHERE1_Front01` / `ATTACK_SPHERE3_Back01`; y - 15; rotation = the aim (unit rotation + aim when the aim is 0); main effect max life x max( 1, range ); `SetLockOn` |
| `32bf02c0`, `32c0a5b0` (new) | `CSI_CTT_DASH_JUMP_COMBO_XX_EventProcess` / `_FrameMove` | landing as the dash jump (down force -> `CSI_POWER_DOWN_LANDING`), air actions after 0.58 (no X branch); 0.202 shot costs a cannon ball (`EffectSet_CSI_CTT_DASH_JUMP_COMBO_XX`, Kwang), else `EffectSet_CSI_SI_A_CHUNG_MISFIRE` |
| the other 29 | Start / End / StartFuture of the mode states, Unready / Z / X EventProcess, Ready / Unready / Forward / Backward / Z FrameMove, Z Z X / Z Z X X / dash X X bodies | = March (written in the `_LIVE` branch beside the March body, `/// June <va> ( = March )`) |

**Stand-ins and open items (for the review).**
- Loop FrameMove: June calls `ApplySpineAimRotation` only while `[g_pMain +0x1a0] == 0`. That is a dword in the `CX2GameOption` (+0x180; data block +4, block +0x1c, zeroed by `3242fbd0`; June's block order differs from March's `OptionList`), not mapped. The port calls it always (default 0); comment marks it.
- `GetExtraCannonBallCount()` is a June inline that reads 0 unless the unit is class 117 with `+0x97a0` set; called here by March's name (cannon balls, CH1q). The Loop / Forward / Backward key `oneDown || count <= 0` and the X shot depend on it only for non-Tactical-Trooper units, which never reach these states.
- June also writes `gage +0x142 = 1` when the X shot spends the ball (inline `SetExtraCannonBallCount`); March's `SetExtraCannonBallCount( n )` is called (CH1q).
- The three damage-effect names `DAMAGE_EFFECT_Cannon_Ball_Bullet_By_Siege` / `_Finish` / `_By_Mobile_Siege` live in June's DamageEffect data, adopted at CH1 final (not checked here).
- No JUNE_BUGS row: nothing in the 41 is a June mistake (the fold quirk of `ApplySpineAimRotation` is row 497).

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h` (patience): `files: 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is commented out.
- `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `allow-listed: 2`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: 0 A/B rows.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors. `cl /P`: `_CONST_CHUNG_CH1J_` 0 / 3 hits (off / on), `CreateCannonBulletDamageEffect` 2 / 3.
- `DEPLOY OK: X2_offline.exe is 25,890,816 B, dated 2026-11-01 07:48:57, md5 860a8d7ade910de83163b19535dab522, equal to X2/US_SERVICE/x2.exe (2026-11-01 07:48:51); built at main c72085b6 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off, so the phase adds no string to the exe.

**Nothing to play-test** (`_LIVE` is off; nothing registers these states). Next: `/x2-phase char-chung CH1j review` (Opus 5.5 high), then CH1k0.

### CH1j - Review (2026-11-01, Opus 5.5)

**Verdict: the code passes with one correction: the stand-in gate is replaced** (`X2GUChung.cpp` Loop FrameMove, inside `_LIVE`, +3 / -2).
I read every function from `charport_adis.py chung --phase CH1j` against the port.
- **The open item: `g_pMain +0x1a0` is `GetOptionList().m_UnitDetail`.** `3377585c` = g_pMain (funcmap). June has 16 readers of `+0x1a0`. The
  unit-viewer one (`31ff0745`) writes render type 3 for 1 and 0 for 2, which is `RT_CARTOON` for `OL_MEDIUM` and `RT_REAL_COLOR` for `OL_LOW` (March
  `X2UnitViewerUI.cpp:447-454`). The riding-pet one (`32d2c49f`, `OnFrameRender_Prepare`) matches March `X2RidingPet.cpp:293-299` the same way.
  So June `32bf07dc` is `m_UnitDetail == CX2GameOption::OL_HIGH`, the same test March's own siege aim makes (`X2GUChung.cpp:26943`).
  The port now calls `ApplySpineAimRotation` only under that test. Other Chung readers: `32be5b14` (in `32be5a80`) and `32c0e949` (in
  `32c0e530`, a `CreateCannonBulletDamageEffect` caller), for the phases that port them. The X FrameMove (`32c108d0`) calls it ungated, as the port does.
- **Loop / Forward / Backward EventProcess** (`32bece90` / `32bec410` / `32bec950`). MP drain: `31fceab0( 26419 ) > 0` (`GetSkillLevelValue`), then
  `elapsed x -3.0 (qword) x 32007b10( 0x1b = SA_MP_CONSUME_REL, 26419 )`, then `GetNowMp() <= 0` -> UNREADY.
  Keys: Loop and Backward store forward at `+0xa`, Forward stores it at `+0xb`. The flag tests read them back right: Loop FWD / BWD, Forward BWD / `!fwd` -> LOOP,
  Backward FWD / `!bwd` -> LOOP. Flags `0x7d85 + i`, event times `0x8c10 + 12i` [start, end, change], order 2, 1, 3, 4, all in one else-if chain.
  Change: `change < now` (`test 5 / jnp`) `|| +0x2ac`; key: `now < end && start <= now` (`test 0x41`). `SKILL_CANCEL_AFTER( +0x8c90 )` = the
  AWAKE_LOCK expansion. The flag-2 condition `oneDown || !(class 117 && +0x97a0 && count > 0)` is the port's `oneDown || GetExtraCannonBallCount() <= 0`.
- **The cannon-ball guard (kickoff item).** `+0x97a0` = `m_bMobileArmoredModeEnable`. June's inline is March's `GetExtraCannonBallCount()` plus
  a Tactical Trooper / mode-enabled test. These states are reached only by a TT with the mode enabled, so the March call gives the same value
  there. CH1q owns the inline.
- **Member layout (June differs from March).** June drops `m_iExtraCannonBallReloadCount` from this block. `+0x97a4` = Distance
  (`32c17079`: from SA 0x1f), `+0x97a8` = DamageRel (SA 0xf), `+0x97ac` = step dust, `+0x97b0` / `+0x97b4` = X / Z cool. The port's names hold:
  the X shot's power is `GetPowerRate() x m_fMobileArmoredModeDamageRel`, and the spine distance and mobile range read `m_fMobileArmoredModeDistance`.
  `+0x9728` = `m_fSiegeShellingDistance` (`32c16d91`, SA 0x1f).
- **Ready EP / Z Z X EP / X StartFuture.** The animation-end term is on flag 0 (`0x8c18`); cool `<= 0` (`fcom` / `test 1`, NaN refuses).
  DASH / BWALK / SKILL cancels are in June's order. Z Z X falls back to `m_fCanNotAttackTime` 0.1 -> WAIT. X cool is float 0.8.
- **X FrameMove.** EventTimerOneshot 0.033333, then ApplySpineAimRotation, then `count >= 1` (EffectSet + local rotate (0, 0, 10), gage `+0x142` = 1 +
  set count - 1, `+0x97a1` = true, `CreateCannonBulletDamageEffect( 2, ... )`), else misfire + `Chung_CannonGun_Trigger.ogg`. Last come the
  Z / X cool decrements and CommonFrameMove. June never clears `+0x97a1` at the start (March did); the port follows June.
- **New rows.** `32bda860`: SA 0x22 = `SA_MOVE_SPEED` x `+0x30c`. `32bf00c0`: args up / down / yaw / `+0x9884 x +0x97a4`; erase unless
  `eps < |y| || eps < |z|`; both folds `test 0x41`; axis (-1, -1, 0), degree (gap, -z, 0); `+0x8a68` = z on both paths. `32c09e40`: types 0 / 1 / 2,
  names, bones, y - 15, rotation test `|x|`, `|y|` and `31c7b640` (IsSamef); clamp `range < 1 -> 1` (NaN kept); max life `+0x44c` x range;
  `SetLockOn( e, 0, LOT_NONE, 30 )`. `32bf02c0`: landing, `0.58 < now`, ASUT_AIR / wall / Z. `32c0a5b0`: 0.202, `IsFullHyperMode || count > 0`,
  `EffectSet_CSI_CTT_DASH_JUMP_COMBO_XX`, `UseCannon( 1 )`, `TRACE_START0` / `STR_Kwang`, else MISFIRE.
- **Spot checks of the "= March" bodies:** Z FM `32c104d0` (0.01, 170 / 22, 3.0, +10, 100), dash X X FM `32c06b00` (0.8, 10 / 30 / 45 deg, 800, 0.8333, 0.866, 0.3) and
  Z Z X FM `32c0ff80` (0.4333, 3.0, +50, -50, 100) all equal the port.
- No JUNE_BUGS row: nothing here is a June mistake.

**Gates.**
- flagcheck range (`60801ef4^..60801ef4`, patience): `1 PASS, 2 OTHER, 0 FAIL, 0 ERROR`; 0 deleted lines.
- flagcheck worktree (this fix): `X2GUChung.cpp PASS +3 -2`. The 2 deleted lines are CH1j's own stand-in inside `_LIVE`.
- RESULT FAIL in both runs only because the `#define` is commented out.
- `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`.
- `enumscan.py --check X2GUChung.cpp`: 0 rows (the 315 rows in `X2SkillTree.h` are its own enum declarations).
- Probes: `_LIVE` off exit 0 / 0 errors, `_LIVE` on exit 0 / 0 errors.
- `DEPLOY OK: X2_offline.exe is 25,890,816 B, dated 2026-11-01 08:18:18, md5 25c79d0fc2fac9c05b22db674a5bc9a4, equal to X2/US_SERVICE/x2.exe (2026-11-01 08:18:10); built at main 60801ef4 + 1 uncommitted code file(s).`
  No `exestr` line: `_LIVE` is off.

**Nothing to play-test** (`_LIVE` off). Next: `/x2-phase char-chung CH1k0` (Opus 5.5 high).

### CH1k0 - Done (2026-11-01)

All six callees are March functions that already do what June's do on every member March has, so nothing changed in source and no flag
was added. funcmap rows `Char Chung CH1k0` (8: the six, plus the two June-only emblem creators below). No JUNE_BUGS row: no June quirk is
kept, because nothing is transcribed.

| June | March home (CH1k calls it by this name) | Why no port |
|---|---|---|
| `31d28b30` | `CKTDGObject::IsRecursivelyManagedByDGManager()` (`KTDGObject.cpp:153`) | June loops up `+0xc` (`m_pManagerObject`) while `+0x178` == manager `+0x130`; March's `CONVERSION_VS` body compares the same handle with `GetRenderObjectChainEnd()` and recurses |
| `3226e020` | `CX2GameUnit::SetEnableWeaponRenderEffect( bool )` (`X2GameUnit.cpp:13243`) | `+0x416c` = arg; false zeroes the six times `+0x4170..+0x4184` and `+0x4188` (`PWCI_NONE` = 0) |
| `3226f870` | `CX2GameUnit::FadeInWeaponRenderEffect( start, end, fChangeTime, fDuration )` (`:13167`) | the `m_fPulseTimeLeft <= fDuration` gate is March's `NEW_HENIR_TEST` early return (on); same stores |
| `31ff1950` | `CX2GUUser::NotifyShowObjectChanged()` (`X2GUUser.cpp:15436`) | see below |
| `32497d80` | `CX2ChungGageData()` (`X2GageData.h:620`) | 0x144 bytes: four `KProtectedType<int>` = 0, three bools false |
| `32a37130` | `CX2ChungGageData( const CX2ChungGageData& )` (`:626`) | the same seven members copied |

**Layout of the weapon render block** (`3226f900` Pulse, `3226f870` Fade, `3226e020`): `+0x416c` bool, `+0x4170` fade time left, `+0x4174` /
`+0x4178` fade change time (left), `+0x417c` pulse duration, `+0x4180` period, `+0x4184` pulse time left, `+0x4188` colour id, `+0x418c` /
`+0x419c` start / end colour. That is March's member order.

**`NotifyShowObjectChanged` (`31ff1950`).** June does March's steps in March's order: `GetShowObject`, the `CX2GameUnit` base (`322845c0`),
the shadow (`+0xcb8`), vslot 0x394 = `SetShowEnchantWeaponParticle` (`32000490`: base, then `SetShowEnchantedWeaponEffectAtHand`;
`ENCHANT_BUG_TEST` on), the head marker (`+0x87f8`, game major), the title emblem (`+0x87fc`, UI major), hold centre and circle (`+0x8818` /
`+0x881c`, game major = March's `X2OPTIMIZE_CONFUSE_DIFFERENT_PARTICLE_SYSTEMS_BUG_FIX` branch, on), `DisableAfterImage` when hidden, then
`m_ViewEqipList` and the pet manager (`g_pData +0x328`, `326cb840`). June also does three things that need systems March does not have:
- `+0x8808`, a second title emblem: made by `31fd54b0` from a June-only string at title templet `+0x60` (title manager `g_pData +0x28c`).
- `+0x8810`, the `PC_ROOM_EMBLEM` sequence, made by `31fcfde0`.
- `DisableAfterImage( true, 1 )`, after `( true, 3 )`: the second after-image. That is Lu/Ciel's second body (Buff 4b-iii, checklist s7).
March never creates the two handles, so on March's members its body already gives June's result. A June body with three stand-ins would
compile to the same code, so no `june_march_divergent.tsv` row: the row is for a March function that does something different.

Flag states (`cl /P` probe through `stdafx.h`): `NEW_HENIR_TEST`, `CHUNG_SECOND_CLASS_CHANGE`,
`X2OPTIMIZE_CONFUSE_DIFFERENT_PARTICLE_SYSTEMS_BUG_FIX`, `ENCHANT_BUG_TEST`, `X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE`, `CONVERSION_VS` and
`SERV_PET_SYSTEM` are all on.

**For CH1k:** `32bee680` calls the base, then on `IsRecursivelyManagedByDGManager()` adds (`31ce9a60`) or removes (`31ce9860`) each
`+0x9648` equip = March's Chung body. `32bdc700` calls the base, then
vslots 0x63c / 0x634 with `GetShowObject()`. `32bda7d0` / `32c0ca60` call `SetEnableWeaponRenderEffect`. `32c0feb0` is March's
`CreateGageData` (`IsMyUnit`, `GetMyGageData`, copy or default ctor). `3226f870`'s caller `32c007e0` is CH2b's. `kickoffs/char-chung/CH1k.md`
"Amended by CH1k0" lists them.

**Routed, not ported:** the two June-only emblems go to **Stage 9** (a bullet in `JUNE_PORT_BACKLOG_ORDER.md`). Both are made by June
code March lacks and are also read in `3203d160` and `CommonFrameMove` `32063650`. The second after-image is already Lu/Ciel's (checklist s7).

**Gates.**
- No client source changed: no flagcheck, no build. CH1j review's `DEPLOY OK` md5 `25c79d0f` stands for this tree.
- `junediverge.py --check X2Lib/X2GUUser.cpp X2Lib/X2GameUnit.cpp X2Lib/X2GageData.h KTDXLIB/KTDGObject.cpp` (the homes): `allow-listed: 6`,
  `RESULT: PASS`.
- `enumscan.py --check X2Lib/X2GageData.h KTDXLIB/KTDGObject.cpp`: `0 A/B row(s) remain in 2 file(s)`.

**Nothing to play-test** (no code changed). Next: `/x2-phase char-chung CH1k0 review` (Opus 5.5 high).

### CH1k0 - Review (2026-11-01, Opus 5.5)

**Verdict: passes; nothing changes in source.** All six callees are March functions that already do what June's do. One bookkeeping
correction (below). I read each function in Ghidra (decompile and, for the gates, the disassembly).
- **`31ff1950` = `NotifyShowObjectChanged`.** June's order is March's: `GetShowObject` (`31fae370`: `+0xc` chain, `+0x172`), the base
  `322845c0`, shadow `+0xcb8`, vslot 0x394, then the handles head marker `+0x87f8` (game major), emblem `+0x87fc` (`g_pData +0x7c`, UI major),
  hold centre / circle `+0x8818` / `+0x881c` (game major `322ceaa0`), then `SetShowObject( bShow )` on each, `+0x8808`, then `+0x8810`. Then
  `DisableAfterImage`, the `+0x8834` equip loop (vslot 0x28, intrusive release `31d28b60`), and the pet manager `g_pData +0x328` with
  `GetUnitUID` (vslot 0x54) and `bShow` (`326cb840`). This is March's `X2GUUser.cpp:15436` body.
- **The second after-image is not Chung's.** `322720f0( 1, 3 )`: mode 3 takes `+0x6758` only when vslot 0x260 (`3208e160`, byte `+0x8e90`) is
  set, else `+0x6754`. `( 1, 1 )` takes `+0x6758`. The user init `31fee090` builds `+0x6758` only when `+0x8e88` (the second skin anim, unit
  type 10) is set. For Chung both calls reduce to March's `DisableAfterImage( true )`. No divergence row is needed.
- **`+0x8808` / `+0x8810`.** Swept with `dispsweep.py 8808 8810 6758`. Writers: the ctor `3203e850` (init), `31fd54b0` (create `+0x8808`),
  `31fd58c0` (destroy `+0x8808`, new funcmap row) and `31fcfde0` (create `+0x8810`). Readers: `31ff1950`, `CommonFrameMove` `32063650` and
  the dtor `3203a620`, plus `3203d160` for `+0x8810` only. **Correction:** the Done note and backlog bullet said `3203d160` reads both. It reads
  only `+0x8810`; the bullet in `JUNE_PORT_BACKLOG_ORDER.md` (Stage 9) is fixed. None of these writers exists in March.
- **`3226e020` / `3226f870`.** The stores land in March's member order (`X2GameUnit.h:5127-5140`): `+0x416c` bool, `+0x4170` fade left,
  `+0x4174` change time, `+0x4178` change left, `+0x417c` / `+0x4180` / `+0x4184` pulse duration / period / left, `+0x4188` colour id,
  `+0x418c` / `+0x419c` start / end. The Fade gate is `fld [+0x4184]; fld dur; fcom; test ah,5; jnp return`. It returns only when `dur < left`,
  and runs the body on equal or NaN. That is March's `if( m_fPulseTimeLeft > _fDuration ) return;`, NaN included (Ghidra's `<=` hides this).
- **`31d28b30`.** The June ctor `31d28a20` stores `g_pKTDXApp +0x1fc` (`GetDGManager`) `+0x130` into `+0x178`, as March's `KTDGObject.cpp:68`
  `m_hKTDGManagerHandle = GetRenderObjectChainEnd()`. `+0xc` is `m_pManagerObject` (`31fae370`). The loop is March's recursion.
- **`32497d80` / `32a37130`.** Four `KProtectedType<int>` at `+0x110` / `+0x11c` / `+0x128` / `+0x134` (12 bytes each), default-built
  (ctor) or decoded from rhs and re-encoded (copy). Bools: `+0x140..+0x142` = 0 (ctor) or copied from rhs (copy). That is the
  `X2GageData.h:620-632` initializer lists. June's `KProtectedType` has a registry `3376cdd0` and a vtable that March's lacks. That is the
  anti-cheat type, not this class, so it is out of scope.
- **`kickoffs/char-chung/CH1k.md` "Amended by CH1k0"** names the March symbols correctly. CH1k is ready.

**Gates.** No source change: no flagcheck, no build (CH1j review `DEPLOY OK` md5 `25c79d0f` stands). `junediverge.py --check-all`:
`allow-listed: 160`, `RESULT: PASS`. Nothing to play-test. Next: `/x2-phase char-chung CH1k` (Sonnet 5.5 high).

### CH1k - Done (2026-11-01)

All 38 class-map rows of phase CH1k are written, plus the ctor map: 33 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`; `_LIVE` stays `//#define`) and 5 `new` under `_CORE` (declared in `X2GUChung.h`, referenced by nothing; one of them, `32bdc890`, is not new: see below). 39 funcmap rows `Char Chung CH1k`. Read from `charport_adis.py chung --phase CH1k`; Ghidra for the key strings (obfuscated Lua keys decoded with a scratch decoder).

| June | `CX2GUChung::` | vs March |
|---|---|---|
| `32c0cf90` | `DamageReact` | differs: Iron Will 7p hit stop (attacker and unit `SetStopTime( 0.5, false, NULL, false )`, `SetStop2Time( 0 )`); guard-damage branch also needs `!GetInvincible()`; dash Z X mini gun handles (slots 0x11 / 0x12) cleared at the end for every reAct type |
| `32c0ca60` | `ReInit` | differs: Heavy Field NPC 2525 removed; PvP / PvE (type 1 / 4, `GetIsNowPVPCorrectionGame()`) start with the PvP count (`+0x9680`), other games raise to the dungeon count (`+0x9674` = March's `m_ucBasicCannonBallCount`) |
| `32bf4900` | `InitComponent` | differs: PvP / PvE take `m_ucPvpStartCannonBallCount` (March 3 in PvP), training 3; reads `SIEGE_ANGLE` (4), `MOBILE_SIEGE_ANGLE` (4 = CH1j's spine aim limits), `EXTRA_CANNON_BALL_DAMAGE_RATE` (2), `COMMAND_HIT_ADD_MP_BONUS` (default 1) |
| `32c063d0` / `32c06750` | `HyperModeBuffEffectStart` / `End` | differs: boost handles only for a non-transcended unit without an awakening effect set; cannon balls refilled at the start (March: end); end sets `m_fCannonBallGaugeTimer = -100` |
| `32beeed0` | `ApplyRenderParam` | differs: Chaos Cannon states (`IsNowStateChaosCannon`) take the guard outline; else branch = CX2GUUser `31fd4460`, stand-in `CX2GameUnit::ApplyRenderParam` |
| `32c0c4e0` | dtor | differs: also removes NPC 2525 |
| `32beaf70` | `GetComboXStateID` (header) | differs: Deadly Chaser -> `CSI_CDC_COMBO_X` |
| `32bdc890` | `CalculateHpPercentUpRateByDamageDataHpUp` | the class map's "new GetMetabolismBoostValue" is March's function; body `= March`, skill id by June name |
| `32bed3d0`, `32bed7e0`, `32bed840`, `32beeb90`, `32beec40` | `GetDefenderDamageRate`, `GetDamageRateOnCommanderMarkedDefender`, `GetDamageRateOnExcitingCrashDefender`, `IsNowStateChaosCannon`, `GetOpenMindScaledValueInGuardState` | `new` under `_CORE`, plain names by role; callers are GetFinalDamage / GetAdditionalAttackPowerRateByType (Stage 33) |
| the other 22 | Init, Win, Lose, Draw, InitEffect, CreateMinor/MajorParticleArray, InitEnchant..., COMMON_BUFF_FrameMove, NotifyShowObjectChanged, NotifyDGManagerChanged, OnFrameMove, GetCriticalValue, DeleteMinorParticle, ApplyHyperModeBuff, ApplyRenderParamOnEmptyRenderParamByBuff, CreateGageData, DeleteMajorMesh, InitPromotionEquipList, LoadPromotionItemID, IsThisStateCanBeChangedToGuardStateOnHit, GetComboZStateID | = March (copy in the `_LIVE` branch, `/// June <va> ( = March )`); `ChangeMaxCharacterAddResource` is already June's (Buff 7b) and untouched |

**Members and the ctor map.** New `_CORE` members (ctor values = June's): `m_ucPvpStartCannonBallCount` (`+0x9680`, 0; June splits March's one start count into dungeon `+0x9674` and PvP `+0x9680`, Lua keys `CANNON_BALL_COUNT_AT_DUNGEON_START` / `_AT_PVP_START`, written by LoadPersonalInfo `32bf5430`, CH1l), `m_afSiegeAngle[4]` (`+0x9868`), `m_afExtraCannonBallDamageRate[2]` (`+0x9898`, 1), `m_fCommandHitAddMpBonus` (`+0x98c4`, 1), `m_fCannonBallGaugeTimer` (`+0x98b8`, -1). CH1j's spine aim members were not ctor-set: the ctor zeroes them and `InitComponent`'s MOBILE_SIEGE_ANGLE writes them (the header comment "set by the constructor" is stale). `june2017_re/char_chung/ch1k_ctor_map.tsv` lists all 163 offsets June's ctor writes (`+0x9598` .. `+0x98ec`) with width, value, ctor address and the March member where I could pair it; rows with an empty last column belong to the phase that reads that member (their names come with their consumers, no member is invented here). RNSI names registered (`JUNE_ENUM_NAMES`): `RNSI_SA_CDC_EXCITING_CRASH` 26040, `RNSI_P_CFG_METABOLISM_BOOST` 26400, `RNSI_P_CTT_REACTIVE_ARMOR` 26414, `RNSI_P_CTT_USEFUL_TACTIC` 26420. New namespace `_CONST_CHUNG_CH1K_` (`IsTranscendedUnit` through `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT`; the awakening-set stand-in is CH1c's, forward-declared).

**Open items (for the review).**
- **Needs Opus:** `GetOpenMindScaledValueInGuardState` (`32beec40`, unit vslot 0x1b8): June reads a protected float at `CX2GameUnit +0x567c` (base `3208e290`); that member and the vslot's caller are not identified in March, so the port takes the base value as an argument (stand-in).
- June dereferences the defender optr of `32bed7e0` / `32bed840` without a null test; the port tests it (a crash otherwise, no JUNE_BUGS row).
- The `ApplyRenderParam` else branch, `HyperModeBuffEffectStart`'s awakening and transcended gates are stand-ins already used by Eve / CH1c.
- `SetStopTime( 0.5, false, NULL, false )` compiles in this build (the 4-argument overload is the active one).

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `9 deleted line(s)` (flagcheck's own alignment of the LoadPromotionItemID copy: `git diff --patience` on the cpp shows 897 insertions, 0 deletions), `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because the `#define` is commented out.
- `junediverge.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `allow-listed: 2`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: 0 A/B rows.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
- `DEPLOY OK: X2_offline.exe is 25,891,328 B, dated 2026-11-01 09:28:10, md5 c47b1572aee03fdbb1e8c8469204450c, equal to X2/US_SERVICE/x2.exe (2026-11-01 09:28:03); built at main 1f33e82c + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off, so the phase adds no string to the exe.

**Nothing to play-test** (`_LIVE` is off; the flag-off tree adds only unused `_CORE` members and functions). Next: `/x2-phase char-chung CH1k review` (Opus 5.5 high), then CH1l0.

### CH1k - Review (2026-11-01, Opus 5.5)

**Verdict: passes, and the one stand-in is resolved.** I read every CH1k row against `charport_adis.py chung --phase CH1k` (and Ghidra or
`disasm_range.py`), and compile-probed 34 enum values (`enumprobe.py`).
- **`DamageReact` `32c0cf90` = port.** Iron Will gate: class 110, reActType not 0 / 7 / 10, not invincible, HP > 0, foot on line, chance > random;
  the chance doubles in CFG guard wait / damage and in dungeon / field. The hit stop is vslot 0x100 `( 0.5, 0, 0, 0 )` on the attacker unless
  `AT_EFFECT`, the unit gets 0.5, and its second stop (vslot 0x104) gets 0: March's one-argument calls with the same values. Reduce +100 / +80
  (plus guard) around `CX2GUUser::DamageReact`. Be-shot ball: time <= 0, random <= rate, 2 s, then June's inlined not-berserk +1, which is
  `UpCannonBallCount( 1, true )`. Iron Will turns DOWN / UP / FLY / DRAG_UP into BIG_DAMAGE. Guard branch: `bAttackerFront && !GetInvincible()`
  (`@32c0d4de`). The slot 0x11 / 0x12 handles are cleared after every branch, including the invincible `CSI_GUARD` path.
- **Enum shifts are harmless.** March's Chung states are June's minus 0x2e (the common-state shift), for example `CSI_GUARD` 0x75 / 0xa3,
  `CFG_GUARD_WAIT` 0xb7 / 0xe5, Chaos Cannon 0xef..0xfa / 0x11d..0x128, `CSI_COMBO_Z` / `_X` / `CSI_CDC_COMBO_X` 0x78 / 0x81 / 0xa0. The order
  is the same, so the names are right. The mesh enumerators `CSG_COMBO_DASH_ZX_MINI_GUN_L` / `_R` (March 18 / 19) are June slots 0x11 / 0x12 by
  name (CH1 header comments). `XS_TRAINING_GAME` is 17 in March; June compares 0x12, which three confirmed funcmap rows name.
- **`ReInit` `32c0ca60` = port.** It turns full hyper off and calls the base. Then targeting NPCs 863 / 1419, land demolisher (colour 2,
  `+0x96cc`), Heavy Field 2525. Game type 1 or 4 takes team kills (`+0x4a4` red, `+0x4a8` blue, `32bf4040`) or the PvP count `+0x9680`;
  otherwise the count is raised to `+0x9674`. Then the sync (Tactical Trooper total), the counters, and the Chaos Cannon effect and sound.
- **`InitComponent` `32bf4900` = port.** Type 1 / 4: `+0x140` / `+0x141` false, count `+0x9680`, Ex 0, changed; training: 3. The table names
  follow from the string layout `33509f4c` / `f6c` / `f80` (EXTRA_CANNON_BALL_DAMAGE_RATE / MOBILE_SIEGE_ANGLE / SIEGE_ANGLE), offsets
  `+0x9898` / `+0x9878` / `+0x9868`. I decoded `COMMAND_HIT_ADD_MP_BONUS` again from the 24-byte XOR at `337adba0`; default 1, `+0x98c4`.
- **Hyper mode `32c063d0` / `32c06750`, `ApplyRenderParam` `32beeed0` = port.** Start: unit `+0x3c`, the `31fd66b0` stand-in, the refill from
  gage `+0x11c`. End: `-100` at `+0x98b8` after the changed flag; March's End refill is gone. Render: `+0x6840`, state 0xe7, Chaos time, `32beeb90`;
  colour `( 0, 245/255, 1, 1 )` = `0xff00f5ff`. START..END in March's enum is the same 12 states.
- **The three damage-rate `_CORE` bodies = port.** `32bed3d0`: `GetSkillLevel( 26414, true, 0, true )`; hyper count 1 / 2 / else gives SA
  0x88 / 0x89 / 0x8a; `1 - v`; tactical field `+0x41f4` < 1. `32bed7e0`: 26420, BTI 62, SA 0x1c. `32bed840`: 26040, HT 0x21 / 0x22, AT 2,
  `GetSkillBuffTempletID( 26040, 0, true )`, SA 0x39. `31fceab0` / `32007b10` are March's `GetSkillLevelValue` / `GetLearnedSkillAbilityValue`.
  June calls `HaveThisBuff` on a null defender with no test; the port's null test stays, because June's way would crash.
- **Dtor `32c0c4e0`, both combo ids, `DeleteMajorMesh` = port.** Correction to the Done note: June frees 19 slots, but March's loop covers
  `CHUNG_MAJOR_MII_END` = 22, not "one to one". The extra slots are March meshes June dropped, and freeing them is harmless.
- **`= March` spot checks pass:** `GetCriticalValue` (`+0x4dfc`, `+0x9718`, `+0x96bc`), `IsThisStateCanBeChangedToGuardStateOnHit` (the 12-byte
  list `+0x9638`, the window, the guard point), `CalculateHpPercentUpRateByDamageDataHpUp` (classes 20 / 110, 26400, SA 0x37, `v - 1 + 5e-8`),
  `Init`. RNSI 26040 / 26400 / 26414 / 26420 (and 26415 `RNSI_P_CIP_OPEN_MIND`) match June's `Enum.lua`.
- **Ctor map.** Most blank rows are continuation words (vector tails, array elements, handle slots); the rest stay with the reading phases.
  Filled: `+0x9718` = `m_bIncreaseCritical`.

**`32beec40` is no longer a stand-in.** `dispsweep.py 567c` finds `+0x567c` only in the base `3208e290` and in `32beec40`. It is
`m_AdditionalChargeMpHitted` (`+0x55dc`) `+0xa0` = its `m_fResultValue`. Evidence: `320870a0` = `CheckAndUpdateAdditionalChargeMpHitted` gates on
`+0x568c` = `m_bMustChangeResultValue`, and `32070c30` = `SetBasicStatValueChargeMpHitted`. So unit vslot 0x1b8 is `CX2GameUnit::GetChargeMpHitted`,
virtual in June. Its only callers are `CX2GUUser::DamageReact` `@32049f5c` / `@32049fb9`, `UpNowMp( GetChargeMpHitted() )` = `X2GUUser.cpp:11497` / `11509`.
- `X2GameUnit.h`: `GetChargeMpHitted` is virtual under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`; the `#else` is March's line.
- `X2GUChung.*`: `GetOpenMindScaledValueInGuardState( float )` becomes `virtual float GetChargeMpHitted() const` (`_CORE`) with June's body:
  base x SA_MP_GAIN_GET_HIT_REL of 26415 while the guard test holds. It uses a `const_cast` because March's guard test is non-const. It stays
  inert until `_LIVE` makes the base virtual.
- Rows: `june_march_divergent.tsv` `GetChargeMpHitted` (replace, ported under `_LIVE`), funcmap `32beec40` / `3208e290`.

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2GameUnit.h`:
  `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`. `7 deleted line(s)` are CH1k's own stand-in lines; `0 non-ASCII inserted`,
  `0 EOL-mismatched`, `0 #else/#endif without the flag token`, `0 encoding change(s)`. RESULT FAIL only because `_LIVE` is `//#define`.
- `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2GameUnit.h`: 0 A/B rows.
- Probes (`/Zs /EHsc`, no PCH), all exit 0 with 0 errors: `X2GUChung.cpp` as in the tree; `X2GUChung.cpp` with `-D..._LIVE`; `X2GUUser.cpp` with `-D..._LIVE`.
- `DEPLOY OK: X2_offline.exe is 25,891,328 B, dated 2026-11-01 09:53:59, md5 05b833e84d40a542d33d854ef1ba91ea, equal to X2/US_SERVICE/x2.exe
  (2026-11-01 09:53:52); built at main 802a550b + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off, so no string is added.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-chung CH1l0` (Opus 5.5 high).

### CH1l0 - Done (2026-11-01)

No source change and no flag: one helper is March's own function, one is dead in June, and two belong to June's equip-view rewrite, which is
Lu/Ciel's (scope rule; routed to Stage 8). funcmap rows `Char Chung CH1l0` (15); `june_march_divergent.tsv` 3 rows, verdict `stage8`
(the `stage33` shape: informational, no replacement here). No JUNE_BUGS row: nothing is transcribed.

| June | March home (CH1l calls it by this name) | Verdict |
|---|---|---|
| `31fcd740` | none | **dead**: base of a June-only vslot 0x688 (`SetNowMp( GetMaxMp() )`, then +1 awakening orb when `GetHyperModeCount()` equals the protected int `+0x8154` and is < 1) |
| `31fe21a0` | `CX2GUUser::RemoveEquipFromViewList( CX2EqipPtr, int )` (`X2GUUser.cpp:35764`) | **same**: erase at index, weapon index 0 / 1 for `EP_WEAPON_HAND` / `EP_WEAPON_SECOND` (2 / 0x25), erase + delete the matching `m_vecpWeapon` entries |
| `31fd2f00` | `CX2GUUser::GetAddItemAndRemoveItemToViewList` (`:41897`) | **differs, Stage 8**; for a non-Lu/Ciel unit the result is March's except Stage 16's expiry test |
| `31ffe990` | `CX2GUUser::AddEquipToViewList( CX2Item* )` (`:35650`) | **differs, Stage 8**; June: `( item, slotId, bForceBasic )`, Ciel copies, CreateEquip via vslot 0x570 |

**vslot 0x688 is never called.** A byte scan of June's exe for `call` / `jmp [reg+0x688]` (incl. vcall thunks and SIB forms) and
`mov reg, [reg+0x688]` finds 13 hits, all data members or `esp` locals. The slot's only callers are the base calls in Chung's override
`32bf44d0` (then gage `+0x142` = 1, `SetNowCannonBallCount( max )`) and Add's `329e2770`, which nothing reaches. So `32bf44d0` (CH0's "new
AfterInitEquippedActiveSkillState") is dead June code: CH1l marks it n/a, and Add's port has nothing to transcribe there.

**The equip-view set** (`32bdc750`'s three callees). Chung's override is March's body: `GetAdd...` false -> false; remove the found equip;
add the item. The only new thing is a third argument, `slotId`, passed through to `AddEquipToViewList( item, slotId, 0 )`. It comes from
vslot 0x644's June signature, which the base `3201b930` (rewritten too) and every override share. What June changed is Lu/Ciel's two bodies:
- per-form tables: fashion is unit `+0xb7d` `[pos*3 + (equip +0x15d4 != 0)]`; `IsNormalEquipInThePosition` (`31fac260`) reads `+0xbf2` per form for
  positions 16-29 (form = unit `+0xe40`);
- Ciel copies built from the templet's `m_ItemIDForMinorUnit` (`+0x30`) when the unit type is 10 (`31fac140`), and Ciel slot ids `0x14`-`0x2a`
  picking form 1;
- skin anim per form, vslot 0x570 (`31fd48a0`: `+0x87d4` or Ciel's `+0x8e88`), and "( pos, form ) already shown" `31fd63c0`.
These need June's CX2Unit form tables, Ciel's skin anim and the minor-unit item ids, none of which March has. For any other class the
form is 0 and the result is March's. The remaining differences, which do apply to Chung, go to Stage 8 with the set:
- CreateEquip always gets the unit type, plus two arguments (cf. the `CreateDummyEquip` replace row);
- `GetEquippingItemByEquipPos` takes a third argument (`stage33` row);
- the usable test `328825c0` and enchant `32882540` (the Stage 16 parts);
- there is no `INIT_COMPONENT` read.
Bullet added to `JUNE_PORT_BACKLOG_ORDER.md` Stage 8 ("June's equip-view rewrite").

**For CH1l.** `UpdateEquipViewListInfo` (`32bdc750`): leave March's body, with no `_LIVE` block. It calls `GetAddItemAndRemoveItemToViewList` /
`RemoveEquipFromViewList` / `AddEquipToViewList` by March's names; Stage 8 adds `slotId` to every override at once. `32bf44d0`: n/a (dead). The
kickoff has "Amended by CH1l0".

**Gates.** No source changed, so there is nothing to flagcheck and no build; the CH1k deploy (`DEPLOY OK` md5 `05b833e8`) stands.
`junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`.

**Nothing to play-test.** Next: `/x2-phase char-chung CH1l0 review` (Opus 5.5 high).

### CH1l0 - Review (2026-11-01, Opus 5.5)

**Verdict: passes. All four verdicts hold, nothing changes in source, and CH1l can start.** Three readings in the notes were wrong and are
corrected below. None of them moves anything out of Stage 8.
- **`31fcd740` is dead (holds).** I re-scanned `14JUN17\data\unpacked_x2.exe` below raw `0x1cbbe00` for disp32 `0x688`: 111 hits. None is
  `FF /2` or `FF /4` (`call` / `jmp [reg+0x688]`, including SIB forms). The 8 loads from a register other than `esp` (`31d80d17`..`31d81094`,
  `31d943d8` and `32fdfd35`, plus `ebp` at `32193fca`) and the `add [esi+0x688]` at `31d81075` read `this` members: `+0x684` / `+0x688`
  vector pairs. The `add ecx, 0x688` lines at
  `332db6af` / `332dbc56` are SEH unwind funclets, and there is no `add reg, 0x688` before an indirect call. A raw rel32 scan finds only three
  `call` / `jmp` to the three bodies: Chung's base call `32bf44d3`, Add's base call `329e2773`, and `32b90da0` (`jmp 31fcd740`). That `jmp` is
  the thunk Ara's vtable holds in slot 0x688 (`3350599c`), so it is not a caller. Thirteen RTTI-resolved vtables hold slot 0x688. No member
  pointer or Lua binding could reach the slot either: either one would need a vcall thunk (`jmp [eax+0x688]`), and none exists. So `32bf44d0`
  is still n/a.
- **`31fe21a0` = March `RemoveEquipFromViewList` (holds).** It reads the position at `+0x1b0` and erases `m_ViewEqipList` at the index. It maps
  2 -> 0 and 0x25 -> 1: `EP_WEAPON_HAND` and `EP_WEAPON_SECOND` by count in `X2Unit_PreHeader.h`. On an `m_iWeaponIndex` match it erases with
  memmove, calls `DeleteKTDGObject` (`31d28b60`) and does `--i`, then re-tests against the new size. It releases the by-value smart pointer
  (`+8` refcount, `31d28b60` at 0).
- **`32bdc750` = March's body + `slotId` (holds).** At entry plus 0x1c, args 1 / 2 / 3 are `[esp+0x20]` / `+0x24` / `+0x28`. The calls are
  `31fd2f00( &ptr, &add, remove, &index )`, then `31fe21a0( ptr by value, index )` and `31ffe990( add, slotId, 0 )`. The function ends with
  `RET 0xc` on both paths.
- **The equip-view set goes to Stage 8 (holds).** No residue changes what Chung shows on any playable path:
  - *CreateEquip always gets the unit type:* **no visible difference** (correction). June's `CX2Eqip` ctor `32961280` sets the unit type to 0
    unless the displayed templet's `m_dwFlags & 0xf000 == 0x1000`. That field is `m_UseCondition` (bits 12-15), and 1 is `UC_ANYONE`. June
    moved March's `UC_ANYONE` gate from `AddEquipToViewList` into the ctor. A class-locked item therefore gets March's model, never
    `GetCommonItemModelName( type )`.
  - *"No `INIT_COMPONENT` read":* **not a difference** (correction). March's read sits under `#ifndef FIX_SLASH_TRACE`, and `FIX_SLASH_TRACE`
    is ON (`cl /EP` probe through `stdafx.h`).
  - *Usable test `328825c0`:* its first clause is **not an expiry test** (correction to the S7k reading). Templet `+6 & 0xf` is `ITEM_TYPE`, and
    1 / 2 is weapon / defence. Templet `+4` bit 0 is `FLAG_BIT_FASHION`. `32882540 < 0` is a negative enchant level. Together that is March's
    `CX2Item::IsDisabled()` (`ITEM_RECOVERY_TEST`, ON by probe), read through June's enchant getter. The getter returns 7 or special ability
    0x28 for a PvP-equip templet (bit 7) or a type-0x4d division item, and `+0x18` otherwise. The endurance clause is March's.
    `32809f40( 0x4d, id )` with `+0xc4` / `+0x48d` stays unknown (Stage 16).
  - *What Chung can see:* a broken item is possible offline, because enchanting can break one (`ERR_ENCHANT_RESULT_04`). If Chung takes off a
    fashion item that sits over a broken normal item, June shows the basic model and March shows the broken item. Stats are dropped in both
    (`CX2Unit::EqipAbility`). A PvP-equip weapon also shows a +7 glow in June. Both cases are cosmetic and apply to every class, which is not
    playability under the scope rule (crash, softlock, progression, combat numbers). They stay with the set in Stage 8, and the bullet now names
    them.
  - *`GetEquippingItemByEquipPos( pos, false, 0 )`:* slot class 0 covers every non-Ciel equip slot (`stage33` row), so Chung gets the same item.

**Corrected rows.** `funcmap.tsv`: `328825c0 (S7k)`, `31fd2f00`, `31ffe990` and `31fcd740` are reworded, and `32b90da0` is added (Ara slot
0x688 thunk). `june_march_divergent.tsv`: the `GetAddItemAndRemoveItemToViewList` and `AddEquipToViewList` rows are reworded (verdict
`stage8` unchanged). The `JUNE_PORT_BACKLOG_ORDER.md` Stage 8 bullet is updated. `kickoffs/char-chung/CH1l.md` "Amended by CH1l0" is
checked and correct as written (14 of 16 rows; `32bdc750` stays March, `32bf44d0` n/a).

**Gates.** No source changed: no flagcheck, no build (the CH1k review's `DEPLOY OK` md5 `05b833e8` stands). `junediverge.py --check-all`:
`allow-listed: 160`, `RESULT: PASS`. Nothing to play-test. Next: `/x2-phase char-chung CH1l` (Sonnet 5.5 high).

### CH1l - Done (2026-11-01)

14 of the 16 class-map rows are written. The other two are CH1l0's verdicts: `32bdc750` keeps March's body with no block, `32bf44d0` is dead (n/a). 8 are `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March in `#else`; `_LIVE` stays `//#define`), 5 are `new` under `_CORE` (declared in `X2GUChung.h`, referenced by nothing) and 1 is an override of an existing virtual, so it waits for `_LIVE`. Read from `charport_adis.py chung --phase CH1l`; `SetEquippedSkillLevel` ids and states mapped through `ch0_enum_names.tsv` / `ch0_reg.tsv` / `ch0_skill.tsv`.

| June | `CX2GUChung::` | kind | vs March |
|---|---|---|---|
| `32bdbe50` | `SetEquippedSkillLevel` | inside | differs: 18 cases keyed by June skill id (`RNSI_*`), `GetEquippedActiveSkillTemplet( id, true )`. New: Siege Shelling Z finish, Iron Howling loop / end, Guard ready / ready memo, Heavy Field loop / end, Dual Buster upgrade state (when the equipped templet's `m_iDeleteSkillIDByUpgrade` is set). Gone: Detonation, Gigantic Impact, Sudden Burster, Burst Wolf, old Land Demolisher, Leg Shot. The Guard block also writes `SA_MP_CONSUMPTION` (default -4) into the guard object |
| `32bef140` | `ReplaceWeapon` | inside | differs: no `GetItem()` test, the old equip's own enchant level and `GetItemTemplet()` |
| `32bf0a40` | `SetShowViewList` | inside | = March; stand-in: user `+0x93da` (the riding pet's hide-equipment byte that forces `bShow_` false) is not ported, already routed to Stage 17 (`JUNE_PORT_BACKLOG_ORDER.md`, it lists `32bf0a43`) |
| `32bd8880` `32beeaa0` `32bf0530` `32bf0670` `32c0def0` | `UpdateEquipCollisionData` `CanRidingState` `SetShowPromotionEquips` `SetShowOriginalViewList` `HyperModeFrameMove` | inside | = March (CanRidingState tests the same ten states in another order; the two show loops inline `CKTDGObject::SetShowObject`, equal to `KTDGObject.h`; HyperModeFrameMove is March's `MODIFY_RIDING_PET_AWAKE` branch) |
| `32beaf10` | `IsInJobLine` | new | job line 1 / 2 / 3 = classes 20 / 110, 21 / 111, 27 / 117, no unit false. Stays a plain member; CH1o decides the `CX2GUUser` virtual (vslot 0x670, base returns false) |
| `32beeca0` | `GetIncreaseSoulRateOnDamageReact` | new | vslot 0x748 (checklist 7an, "soul gain when hit"): 1.0, or `SA_SOUL_ABS` of `RNSI_P_CIP_OPEN_MIND` while `IsThisStateCanBeChangedToGuardStateOnHit()`. The base virtual (returns 1.0) and the `DamageReact @32049d8a` call are CH4b's |
| `32bed760` `32bf4600` | `StoreGageSnapshot` `RestoreGageSnapshot` | new | vslot 0x740 / 0x744: keep and give back the cannon ball counts (members `m_iSnapshotCannonBallCount` / `Ex` = June `+0x92c0` / `+0x92c4`; the extra count only for class 117 in mobile armored mode). Stand-in: the Crest of Time bases `32009780` / `3204da60` (HP / MP snapshot, Buff Phase 11b) are not ported, so the base call is left out |
| `32befec0` | `UpdateSpineAimRotation` | new | vslot 0x774 (base `31f3f070` is an empty stub): `ApplySpineAimRotation`'s twin with the `SIEGE_ANGLE` limits, range x `m_fSiegeShellingDistance`, and the other bone axis ( -1, 0, -1 ) / ( gap, 0, pitch ) against ( -1, -1, 0 ) / ( gap, -pitch, 0 ); no `JUNE_BUGS` row (two bodies of one design) |
| `32bf0480` | `ScaleHead` | `_LIVE` override | vslot 0x66c: an SD one-piece avatar scales `Bip01_Spine1` outside the hyper mode, else `Bip01_Head`. `CX2GUUser::ScaleHead( float, float )` is already June's virtual (`DIVERGE_SCALE_HEAD`), so a `_CORE` override would change Chung now; it sits under `_LIVE` and `DIVERGE_SCALE_HEAD` (the body is `31fd64a0` body 0, Lu/Ciel's second body is a stand-in). Reads `m_bSDOnePieceAvatar` through the new `CX2GUUser::IsSDOnePieceAvatar()` |

**Registered here.** `RNSI_*` x15 in `X2SkillTree.h` (`JUNE_ENUM_NAMES`: June `26001` .. `26211`; the other three `SetEquippedSkillLevel` ids and `RNSI_P_CIP_OPEN_MIND` were already there). Six `CHUNG_STATE_ID` members under `_CORE` (June script ids `298` `CSI_SI_A_CFG_GUARD_READY`, `321` / `322` `CSI_SI_SA_CFG_IRON_HOWLING_LOOP` / `_END`, `336` `CSI_SI_SA_CSG_DUAL_BUSTER_UPGRADE`, `337` / `338` `CSI_SA_CTT_HEAVY_FIELD_LOOP` / `_END`); their states belong to CH3a2 / CH3c / CH2c. `CFuryGuardianGuardSkill::m_fMpConsumption` (June `+0x14`, ctor -4 from `32c24400`). `IsSDOnePieceAvatar()` in `X2GUUser.h` under `DIVERGE_SCALE_HEAD`. All 28 values (19 skill ids incl. `RNSI_P_CIP_OPEN_MIND`, 3 `SA_*`, 6 unit classes) were compile-probed against June's numbers.

**Open items (for the review).**
- **State `336`.** June registers table `CSI_SI_SA_CSG_DUAL_BUSTER` a second time under id 336 (skill 26032 slot); its C++ name is unknown. I named the member `CSI_SI_SA_CSG_DUAL_BUSTER_UPGRADE`; CH1m (which maps `32078bc0` and the registrars) may rename it. `kickoffs/char-chung/CH1m.md` and `CH2c.md` are amended.
- **Base virtuals.** The user virtuals for 0x670 / 0x740 / 0x744 / 0x748 / 0x774 are CH1o / CH4b; until then the five `_CORE` functions are plain members and the two Crest of Time bases stay a stand-in.
- **`_LIVE` dependency.** When CH1 defines `_LIVE`, Always.h must also require `SERV_IRUHADEV_JUNE_DIVERGE_SCALE_HEAD` (the `ScaleHead` override sits under both; without it the override silently drops).
- The `m_ucPvpStartCannonBallCount` header comment says LoadPersonalInfo `32bf5430` is "CH1l"; it is CH1q's (CH1 table).

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES JUNE_DIVERGE_SCALE_HEAD --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h X2Lib/X2GUUser.h`: `1 PASS, 1 PASS~, 2 OTHER, 0 FAIL`, `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because `_LIVE` is `//#define`. Own-flag runs: `JUNE_DIVERGE_SCALE_HEAD` on `X2GUUser.h` PASS, `JUNE_ENUM_NAMES` on `X2SkillTree.h` PASS~. `git diff --numstat`: 402 + 27 + 20 + 1 insertions, 0 deletions.
- `junediverge.py --check` on the four files: `allow-listed: 2`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: 0 A/B rows.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors.
- `DEPLOY OK: X2_offline.exe is 25,892,864 B, dated 2026-11-01 14:08:56, md5 2ff27838351bb8bcc4412b780ea54a2e, equal to X2/US_SERVICE/x2.exe (2026-11-01 14:08:49); built at main 0364128b + 4 uncommitted code file(s).` No `exestr` line: `_LIVE` is off, so the phase adds no string to the exe.

**Nothing to play-test** (`_LIVE` is off; the flag-off tree adds only unused `_CORE` members, enum names and functions). Next: `/x2-phase char-chung CH1l review` (Opus 5.5 high), then CH1l2.

### CH1l - Review (2026-11-01, Opus 5.5)

**Passes, with two fixes, both `_LIVE`-only.** Read against `charport_adis.py chung --phase CH1l` and Ghidra (`32028190`, `3201f850`, `32206e50`, `32206fb0`, `32c24400`).

**Fixed.**
- **`SetEquippedSkillLevelStateData` was not "= March".** Vslot 0x7a8 `32028190` writes the state slot (`+ id x 100`) through `3201f850`, which takes the level from `GetSkillLevel( id, 0, 0, 1 )` (upgrade swap) and multiplies the power rate by `GetSkillAdditionalPowerRate( id ) + 1` (`3201ad10`; Arme / Eve already port it). March's Chung wrapper had neither. June's body now sits inside `CX2GUChung::SetEquippedSkillLevelStateData` under `_LIVE`, March's in `#else`. New `june_march_divergent.tsv` row `SetEquippedSkillLevelStateData` (verdict `replace`, ported under `_LIVE`); funcmap `32028190` row corrected. All 111 call sites are in `SetEquippedSkillLevel`.
- **The Guard block's level** is `32890620( id, 0, 0, 1 )` (`@32bdc0cb`, pushes `1, 0, 0`). CH1l called `GetSkillLevel( id )` (swap false); it is now `GetSkillLevel( id, false, 0, true )`.

**Holds.**
- `SetEquippedSkillLevel` `32bdbe50`: 18 blocks in June's order, ids 26201 .. 26064, each equal to `Enum.src.lua`. All 51 pushed state ids map by name through `ch0_reg.tsv` / `ch0_skill_listing_rows.tsv`, including `0x111`-`0x113` (X, Z finish 274, X finish 275), `0x12a` / `0x12b` Guard ready / ready memo, `0x141` / `0x142` Iron Howling, `0x151` / `0x152` Heavy Field, and `0xf1` = 241 `ARTILLERY_STRIKE_READY` (March enum order READY / FIRE / CANCEL / END). Guard: templet null skips to the next block, unit null returns from the function (`@32bdc0b3` -> `32bdc4f5`); `100 - (int)( SA_DAMAGED_REL x 100 )` -> guard `+0x10`; `TryGetSkillAbilityValue( SA_MP_CONSUMPTION 0x8f )` with -4 in -> `+0x14` only on success. Ctor `32c24400` (0x18 B: ref, slot 0x99, key, bool, `+0x10` int 0, `+0x14` -4.0) confirms both members. `32206e50`'s fourth stack dword (0.0) is unused (`RET 0x10`, three params read).
- State 336 keeps `CSI_SI_SA_CSG_DUAL_BUSTER_UPGRADE`: June's Enum has `RNSI_SA_CSG_DUAL_BUSTER_UPGRADE = 26032`, the skill that registers id 336. `+0x178` = `m_DeleteSkillIDByUpgrade` (funcmap C3l). `CH1m.md` amended.
- `ReplaceWeapon` `32bef140`: no `GetItem()` test; enchant `+0x194` / templet `+0x198` (`m_pItemTemplet`, CX2Eqip ctor C9f) read before the reset. `SetShowViewList` `32bf0a40`: `+0x93da` is the only difference, and `JUNE_PORT_BACKLOG_ORDER.md` L969-973 lists `32bf0a43`.
- `IsInJobLine`: 1 -> 0x14 / 0x6e, 2 -> 0x15 / 0x6f, 3 -> 0x1b / 0x75, else or no unit false. `GetIncreaseSoulRateOnDamageReact`: `31fceab0( 26415 ) > 0` and the guard test, then `32007b10( 0xaa = SA_SOUL_ABS 170, 26415 )`.
- Gauge snapshots: `IsMyUnit` (vslot 0x198), counts at `+0x92c0` / `+0x92c4`, `+0x142` = `m_bCannonBallChanged`. Leaving out the base call is safe while nothing calls the two functions. The bases clear `+0x92a4..+0x92c4`, so `CH4b.md` now says the clear must reach Chung's two members.
- `UpdateSpineAimRotation` `32befec0`: `( eps < |y| ) || ( eps < |z| )` (both `test ah,5 / jnp`); gap = rot.y - aim.y, `360 <= gap` -> `360 - gap`, `gap <= -360` -> `+360`; axis ( -1, 0, -1 ), degree ( gap, 0, z ); `+0x8a68` = aim z; `+0x9868..+0x9874` / `+0x9728` = `m_afSiegeAngle` / `m_fSiegeShellingDistance` (ctor map). The twin `32bf00c0` keeps ( -1, -1, 0 ) / ( gap, -z, 0 ).
- `ScaleHead` `32bf0480`: `+0x8fc0 == 1 && !IsFullHyperMode()` -> `Bip01_Spine1`, else `Bip01_Head`; `31fd64a0( bone, head, face, 0 )`, the same inline as the base `CX2GUUser::ScaleHead` (`DIVERGE_SCALE_HEAD`).
- `= March` spot checks: `HyperModeFrameMove` (0.05 / 0.2 / 0.3, bSound 0), `SetShowPromotionEquips` (inline `SetShowObject`, then `ReplaceWeapon` when shown and `+0x1b0 == 2`), `CanRidingState` (the ten ids).

**Gates.**
- CH1l commit: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES JUNE_DIVERGE_SCALE_HEAD --range 7fa748b7^..7fa748b7`: `1 PASS, 1 PASS~, 2 OTHER, 0 FAIL`, `0 deleted line(s) ... 0 encoding change(s)`; RESULT FAIL only for the `//#define`. `junediverge --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan --check X2GUChung.*`: 0 A/B rows. Probes: `_LIVE` off exit 0 / 0 errors; on exit 0 / 0 errors.
- Review fix: `flagcheck JUNE_CHAR_CHUNG_LIVE --with ... --worktree --files X2Lib/X2GUChung.cpp`: `PASS worktree X2Lib/X2GUChung.cpp +17 -1`, `1 PASS, 0 FAIL`, `1 deleted line(s)` (the Guard line inside CH1l's own `_LIVE` block, replaced in place), `0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only for the `//#define`. `junediverge --check X2Lib/X2GUChung.cpp`: `allow-listed: 2`, `RESULT: PASS`. `enumscan`: 0 A/B rows. Probes: off exit 0 / 0 errors, on exit 0 / 0 errors.
- `DEPLOY OK: X2_offline.exe is 25,892,864 B, dated 2026-11-01 15:29:43, md5 b23f7e943ce9293c639624f23a48397e, equal to X2/US_SERVICE/x2.exe (2026-11-01 15:29:36); built at main 7fa748b7 + 1 uncommitted code file(s).`

Nothing to play-test (`_LIVE` off). `CH1l2.md` checked and amended (decompile every user helper before calling it equal; read each `GetSkillLevel` fourth argument). Next: `/x2-phase char-chung CH1l2` (Sonnet 5.5 high).

### CH1l2 - Done (2026-11-01)

All 9 class-map rows are written: 6 `inside` (June body in March's own `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March's in `#else`; `_LIVE` stays `//#define`) and 3 `new` that need no source. Read from `charport_adis.py chung --phase CH1l2`; the jump table and byte index of `CommonStateStart` were decoded from the exe (`32bf3c4c` / `32bf3c54`).

| June | `CX2GUChung::` | kind | vs March |
|---|---|---|---|
| `32bd8740` `32bd8750` `32bd8760` | `CommonStateStartFuture` `CommonStateEndFuture` `CommonStateEnd` | inside | = March: bare jumps to `320506f0` / `31fdd8e0` / `32015250` (March: `KTDXPROFILE` + the same base call) |
| `32bd8770` | `CommonRender_Prepare` | inside | = March: calls `31fd07b0` and returns the hidden `RENDER_HINT` pointer |
| `32bf2a40` | `CommonFrameMoveFuture` | inside | differs: base first, then `m_FrameDataFuture.syncData.iClassSyncInt` (June `+0x7ec0`) = now count, plus extra x 7 for class 117 with `m_bMobileArmoredModeEnable` (compare, then assign). March wrote `m_CannonBallCount`; June never does |
| `32bf3990` | `CommonStateStart` | inside | differs: after the base, `GetNowStateSkillID()` through `GetUpgradeBaseSkillID` (`32207210`) switches on 12 skill ids; a new raw id re-applies Open Mind (`SetSkillBuffFactors( 26417, 1, true )`, vslot 0x23c) and is kept in `m_iPrevStateSkillID` (`+0x98ec`). Then guard refill (not in `CSI_GUARD` = June 163), used cannon ball count 0, melee force-down x `m_fForceDownRelAtMeleeAttack`, passive critical rate, `m_bIncreaseCritical` false, and the new `USE_CANNON_BALL<n>` rows into `m_vecUseCannonBall` (`+0x98d8`, `{ float, int }`; consumer CH1q) |
| `32c24470` `32c24770` `32c24860` | - | new | std template instantiations behind `push_back` of the 8-byte rows; no source of their own |

**Registered here.** `RNSI_*` x8 in `X2SkillTree.h` (`JUNE_ENUM_NAMES`: June `26013` `26018` `26032` `26039` `26063` `26209` `26223` `26227`); the other 5 names the switch and the Open Mind call spell were already there. All 13 values and `UC_CHUNG_TACTICAL_TROOPER == 117` were compile-probed. Members `m_iPrevStateSkillID`, `m_vecUseCannonBall` under `_CORE`.

**Found.**
- `32c24c80` (class map: CH1q, `SetExtraCannonBallReloadCount`, a role guess) is the protected-int setter of `m_CannonBallSystem`'s used count (`this` = `+0x9624`, int at `this + 8`) = March's `SetUsedCannonballCountAtThisState`; CommonStateStart calls it with 0. `32bf3990` is `CommonStateStart`, not CH1q's `CreateNotEnoughMPEffect`. Both corrected in `ch0_class_map.tsv` and the CH1q kickoff.
- `32bed560` (June's total cannon ball count) is not March's `GetTotalCannonBallCount`: no x7, full hyper reads the max. It was not called; row `GetTotalCannonBallCount` in `june_march_divergent.tsv` (replace, ported in CH1q). `JUNE_BUGS.md` #499: a `USE_CANNON_BALL<n>` row that fails to read skips `EndTable`.
- `+0x8b10` = the passive critical rate (read by `31fd19b0`, written here from `+0x9814`); `+0x3cc` / `+0x790` = `m_bMeleeAttack` / `fForceDownValue`. Stated, not re-proven in Ghidra beyond that.
- **For CH1l4:** with `_LIVE`, nothing writes `m_CannonBallCount` any more; `CommonFrameMove` (Now side) must read `iClassSyncInt`. Kickoff amended.

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because `_LIVE` is `//#define`. `git diff --numstat`: 92 + 2 + 13 insertions, 0 deletions.
- `junediverge.py --check` on the three files: `allow-listed: 2`, `RESULT: PASS`. `enumscan.py --check X2GUChung.*`: 0 A/B rows.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH; from Git Bash pass `-D`, not `/D`, which MSYS turns into a path): `X2GUChung.cpp` with `_LIVE` off: exit 0, 0 errors; with `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: exit 0, 0 errors (`cl /P` confirms the June body is the selected branch: `USE_CANNON_BALL` 1 vs 0).
- `DEPLOY OK: X2_offline.exe is 25,892,864 B, dated 2026-11-01 16:00:00, md5 a10300c83fd36116ca9edf4bb3f73b01, equal to X2/US_SERVICE/x2.exe (2026-11-01 15:59:53); built at main 271885e5 + 3 uncommitted code file(s).`
- `EXESTR OK: 1 of 1 absent in X2_offline.exe` (`USE_CANNON_BALL`, flag-off build carries no June string).

**Nothing to play-test** (`_LIVE` off). Next: `/x2-phase char-chung CH1l2 review` (Opus 5.5 high), then `CH1l30` (Opus).

### CH1l2 - Review (2026-11-01)

**Passes, no source change.** All 9 rows hold against June's listings (`charport_adis.py chung --phase CH1l2`, the exe bytes read with `charport_consts.read_va`).
- **`CommonStateStart` `32bf3990`.** The jump table `32bf3c4c` = { `32bf39e5` case, `32bf3a02` default }; the byte index `32bf3c54` (217 entries from 26011) has 0 at exactly 26011 26013 26018 26032 26039 26063 26208 26209 26220 26223 26225 26227. All 12 names and `RNSI_P_CDC_OPEN_MIND` carry June's `Enum.src.lua` values. The switch runs on `32207210`'s result and the compare with `+0x98ec` uses the raw `GetNowStateSkillID()` (`edi`); every path, out-of-range included, stores the raw id, as the port does. `SetSkillBuffFactors` gets pushes `1, 1, 0x6731` = ( 26417, 1, true ). Chung's vtable slot 0x23c is the base `31fcac90` (`ch0_vtable_all.tsv`), so calling March's non-virtual by name is right.
- **Guard.** The Now state index (protected `+0x7cbc`, ( ptr - `+0x7cec` ) / 100) is compared with 0xa3 = `CSI_GUARD` (`ch0_state_skill.tsv` id 163). `+0x9614` = `+0x9618`, clamped at 0, = `ChargeGuardPointRemain()` (`SetGuardPointRemain` clamps; ctor map: remain / max).
- **Members.** `31fd19b0` adds `+0x8b10` to the critical value (`fadd [esi+0x8b10]`), so it is the passive critical rate. `+0x9718` = `m_bIncreaseCritical`, `+0x96b0` = `m_fForceDownRelAtMeleeAttack` (ctor map). `+0x3cc` / `+0x790` are the same statement as March's own body. June's order (critical rate, then the flag) is kept.
- **`USE_CANNON_BALL<n>`.** The inlined key is `char[255] = {0}` + `"%s%d"` (`33449104`) on `"USE_CANNON_BALL"` (`33509efc`) with n before the increment, then `BeginTable( key, true )`. That is March's `KLuaManagerProxy::BeginTable( name, index )` (`KLuaManagerProxy.cpp:84`) byte for byte. `[1]` float and `[2]` int start at 0 and are read with log on. A failed read jumps to the next key without `EndTable` (JUNE_BUGS #499 holds). Clear = `end = begin` (the move loop is dead: `cmp ecx, ecx`).
- **`CommonFrameMoveFuture` `32bf2a40`.** Base first; class 0x75 and `+0x97a0 == 1` -> extra `+0x128` x 7 + now `+0x110`, else now. It compares with the protected `+0x7ec0` and assigns through the protected-type operator = the KProtectedType `iClassSyncInt` (`X2GUUser.h:911`, Future). `GetCannonBallCount` / `GetExtraCannonBallCount` read gage now / now-ex.
- **Bare jumps.** `32bd8740` / `8750` / `8760` jump to `320506f0` / `31fdd8e0` / `32015250`. `32bd8770` calls `31fd07b0` with the hidden pointer and returns it (`ret 4`).
- **Routing.** `32bed560` = ( IsFullHyperMode vslot 0x9c ? gage `+0x11c` : `+0x110` ) + ( 0x75 and `+0x97a0` ? `+0x128` : 0 ), no x7, as the divergence row says. The CH1l4 and CH1q amendments are right. One caveat was added to CH1q: 32bed560 is March's `GetTotalCannonBallCount` namesake only by the class map, so CH1q proves it from June's callers before replacing.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range acec8aab^..acec8aab`: `2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (X2SkillTree.h, JUNE_ENUM_NAMES), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. RESULT FAIL only because `_LIVE` is `//#define`. `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2GUChung.*`: 0 A/B rows. One-file probes of `X2GUChung.cpp`, `_LIVE` off and `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: both exit 0, 0 errors. No build: no source change, so `DEPLOY OK` md5 `a10300c8` stands. Nothing to play-test. Next: `CH1l30` (Opus 5.5 high); its kickoff is checked.

### CH1l30 - Done (2026-11-01)

**Four helpers mapped; two are March's own functions, one needs no home until Stage 12, and one shared base was finished.** funcmap rows
`Char Chung CH1l30:` (7). The CH1l3 and CH1l4 kickoffs name the March symbols.

| June | March | How |
|---|---|---|
| `31fd7640` | `CX2GUUser::IsNotCommonMoveState()` (X2GUUser.cpp, `_ARME_CORE`) | already ported by Arme CH1i; **completed** (below) |
| `3288fe90` | `CX2UserSkillTree::GetCBConsumption( iSlotIndex, bSlotB )` = March | slot +0x84 / +0x278 - row bases +0x48 / +0x23c = SkillSlotData +0x3c = `m_usCBConsumption`; bound `> 4` = 5 slots (`JUNE_SKILL_SLOT_10`) |
| `31f66620` | `CX2DamageEffect::DamageEffectData::operator new` = March | boost singleton pool `31f8c540` (requested 0xb8 = June's DamageEffectData, ctor `31f8bf00`), critical section, free list `+0x18`, grow `31c5b8c0` |
| `31d70b60` | none (CX2CustomizeManager, Stage 12) | `LEA EAX,[ECX+0x9e4]`. Chung's only use is `32bf0800`'s cut-in item override @32bf08b4-@32bf0929; CH1l3 leaves it out with Arme's / Eve's one-line comment |

- **`IsNotCommonMoveState` was short two tests.** June's body ends with vslot 0x520 (@31fd786b) and 0x578 (@31fd787b); either true -> false. Arme's
  transcription stopped at the riding range. Added under the same flag (`_ARME_CORE`, X2GUUser.h / .cpp, +2 / +3 lines):
  `virtual bool IsWaitState()` = June base `3208e710` (Now == `GetWaitStateID()`; only Rose `32ae37f0` overrides it, adding state 0x9a) and
  `virtual bool IsClassCommonMoveState()` = June base `31e4c550` (false; only Luciel `32b15340` overrides it: states 424-433 / 469-478). Both slots are
  the base in every March class's June vtable (Chung `3350967c` included), so the result is unchanged for every class in the game today.
- Re-read of the eleven getters: vslots 0x438 / 0x43c-0x45c = wait, walk 0xe, dash 0x13, dash end 0x14, dash jump 0x15, dash jump landing 0x16, jump ready
  0xf, up 0x10, down (`3208de10`), landing 0x12, plus charge MP `+0x7d04`: all match March's names in the body. The CH0 row's "protected counts" was wrong:
  `+0x7cbc` is the Now state-data pointer (index = ( ptr - `+0x7cec` ) / 100, `3208ec80`).
- `3288fe90`'s only caller is `32c17830` (@32c17e8a), not `32bf5130` (CH0's "to confirm"). `31f66620`'s only out-of-line caller is `32c058a0` (CH1l4).

**Gates.** `flagcheck JUNE_CHAR_ARME_CORE --worktree --files X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h`: `2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched`; `RESULT: PASS`. `junediverge.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h`:
`RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.*`: 2 rows, both pre-existing (`NPC_UNIT_ID` 4342 / 4343 at X2GUUser.h:2550-2551), none in this diff.
No new enum literal, no JUNE_BUGS row (no quirk). `DEPLOY OK: X2_offline.exe is 25,892,864 B, dated 2026-11-01 16:42:59, md5 558cbabb`
(built at main 989ec7a0 + these 2 files).

**Routed.** `31fd78a0` (OnFrameMove @3206fa18 zeroes user float `+0x91b4` unless waiting, charging MP or `IsWaitState()`) joins the backlog bullet
"June's idle timer" (same `+0x91a8`-`+0x91b8` block); the bullet's stale `JuneIsNotCommonMoveState` name now points at the March one.

**Play-test:** none. Nothing new runs (both added tests are false for every March class); CH1l3 is next after the review.

### CH1l30 - Review (2026-11-01)

**Passes, no source change.** All four readings hold against Ghidra and the exe bytes.
- **`31fd7640`.** Ten vslots (0x438-0x45c) plus member `+0x7d04`, each compared with the Now index, then the ten riding ids, then 0x520 / 0x578; any hit -> false.
  Base getters from the bytes: 0x43c `31b052e0` = 0xe, 0x440 = 0x13, 0x444 = 0x14, 0x448 = 0x15, 0x44c = 0x16, 0x450 = 0xf, 0x454 = 0x10, 0x458 `3208de10` =
  riding ? 0x4f : 0x11, 0x45c = 0x12, 0x438 `3208ddd0` = riding ? 0x4c : `+0x4088`. Same order and names as March's body. Only Luciel overrides
  0x438-0x45c, so writing March's `USI_*` constants is right for every March class. Riding: March `USI_RIDING_WAIT_HABIT`..`_DASH_JUMP_LANDING` = 74..83
  (`cl -Zs` probe, contiguous, `RIDING_SYSTEM` on). June uses 0x4b..0x54, one higher, the same +1 as June's riding wait 0x4c and jump down 0x4f (March 75 / 78).
  The name mapping holds.
- **0x520 / 0x578.** `3208e710` = `sete` on Now == vslot 0x438; `31e4c550` = `xor al,al`. `vtslot.py`: only Rose `32ae37f0` (0x520) and Luciel `32b15340`
  (0x578) differ from the base. A mov+call / `call [r+disp]` scan of `.text` finds exactly @31fd786b and @31fd7910 (0x520) and @31fd787b (0x578).
  `IsWaitState` / `IsClassCommonMoveState` occur nowhere else in X2Lib / KTDXLIB, so no March class's function is overridden or hidden.
- **`3288fe90`.** Index `< 0` or `> 4` -> 0. Otherwise it reads the protected value at +0x84 / +0x278 + 100 x index; the row helper `3288fe00` puts the bases
  at +0x48 / +0x23c, so the field is row +0x3c. The caller @32c17e8a splits the slot inline (> 4 -> B, -5) and `movzx`es the result (16-bit). March's
  `SkillSlotData` (`VERIFY_STAT_BY_BUFF`, `NEW_CHARACTER_CHUNG`, `ARA_CHARACTER_BASE` all on) has `m_usCBConsumption` at 0x3c, row size 84 (June's is 100).
  Rows are only reached by name through March's `GetCBConsumption`, so the different row size changes nothing.
- **`31f66620`.** `31f8c540` sets up a critical section, requested size 0xb8, next size 0x20. Enter / pop free list +0x18 (else grow `31c5b8c0`) / Leave.
  The caller pushes 0xb8; ctor `31f8bf00` writes no further than +0xb6. March's operator new (X2DamageEffect.cpp:10933) = `DamageEffectDataPool::malloc()`
  under `BOOST_SINGLETON_POOL_TEST` (defined).
- **`31d70b60`.** `LEA EAX,[ECX+0x9e4]; RET`, called only at @32bf08dc, inside the `31fd8a50() > 0` block @32bf08b4-@32bf0929 (which ends `JMP 32bf0956`).
  The `<= 0` path @32bf092b and the event 0x8b3 check @32bf0956 are as the CH1l3 kickoff says. The stand-in comment matches Arme's
  (X2GUArme_VioletMage.cpp:28769) and Eve's (X2GuEve.cpp:37603).

**Gates.** `flagcheck JUNE_CHAR_ARME_CORE --range e61bc4e8^..e61bc4e8`: `2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 7 #else/#endif without the flag token, 0 encoding change(s)`.
The seven untokened `#else` / `#endif` lines are already in the two files; CH1l30 added none. `RESULT: PASS`. `junediverge.py --check-all`:
`allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.*`: the two pre-existing `NPC_UNIT_ID` rows (X2GUUser.h:2550-2551) only. No build
(no source change; `DEPLOY OK` md5 558cbabb stands). CH1l3 kickoff: one clarifying line added (the `<= 0` path runs unconditionally).

### CH1l3 - Done (2026-11-01)

All 13 class-map rows are written. `_LIVE` stays `//#define`. funcmap rows `Char Chung CH1l3:` (12); read from `charport_adis.py chung --phase CH1l3`, the jump table `32bed6a0` / byte index `32bed6a8` from the exe.

| June | `CX2GUChung::` | kind | Notes |
|---|---|---|---|
| `32bd8790` | `SpecialAttackEventProcess` | inside | lookup, use (`Send_SKILL_USE_REQ` always), the 3 / 5 use-condition values come back; March body in `#else` |
| `32bd8860` `32bd8870` | `SpecialAttackNoStageChange` / `NoStateChangeActionFrameMove` | inside | false (= March) / bare jump to `UserNoStateChangeActionFrameMove` |
| `32c17830` `32bee490` `32bf5130` | `SpecialAttackLookup` / `Use` / `Cooldown` | `_CORE` | June-only functions (vslots 0x7f8 / 0x7fc / 0x800), Arme's and Eve's shape; the class map's "inside" is these three as pieces of the one March function |
| `32bee3e0` | `TryGuardReadyStateChange` | `_CORE` | Guard 26207 / 26231 from a damage state: `CSI_SI_A_CFG_GUARD_READY_MEMO`; no ball, not berserk: `CSI_SI_A_CFG_GUARD_READY` |
| `32bed5d0` | `CanPayCannonBallConsumption` | `_CORE` | Fatal Cannon 26228 reads the extra count; five skills read the total, the rest the now count |
| `32bed560` | `GetTotalCannonBallCount` | inside | June's body (berserk = max, + extra in class 117's mobile mode, no x 7); divergence row done (below) |
| `32bed8c0` | `GetStateAnimSpeedRate` | `_CORE` | user vslot 0x824 (sole caller `ResetAnimSpeed` `31ffe2b0`); Intense Showtime x acquired value of 26061 / 26234-26235 |
| `32beebd0` | `IsNotCommonMoveState` | `_CORE` | base `31fd7640`, then false in `CSI_CHARGE_CANNON_BALL` / `CSI_POWER_DOWN_LANDING` |
| `32bf0800` `32bf0cc0` | `ShowActiveSkillCutInAndLightByBerserkMode` / `...ByScript` | inside | event 0x8b3 swap kept (`CASH_21` / `CASH_22`, data `s_SkillCutInSetEvent8b3` under `_LIVE`); customize override left out with Arme's comment (Stage 12) |

**Beyond the rows (needed for the skill use to run under `_LIVE`).**
- `CX2GUChung::ReservedSkillStart` (override, `_LIVE`) = Arme's / Eve's: `32057b80` @32057c4c calls vslot 0x800, so the reserved MP is spent at state start. `X2GUUser.h` / `.cpp` got a third `#ifndef` block so the hook exists when only `_CHUNG_LIVE` is defined (7 + 7 lines).
- Registered `RNSI_*` x9 in `X2SkillTree.h` (`JUNE_ENUM_NAMES`): 26036 26061 26200 26203 26228 26231 26233 26235 26407; the rest were there. States 298 / 299 = `CSI_SI_A_CFG_GUARD_READY` / `_MEMO` (`USI_END` 152 + 146 / 147), 26 / 28 = `USI_DAMAGE_SMALL_FRONT` / `_BIG_FRONT`, 162 / 154 = `CSI_CHARGE_CANNON_BALL` / `CSI_POWER_DOWN_LANDING`.
- Members `m_bGuardStartedFromDamageReact` (`+0x985c`, set by 32bee3e0, read by GUARD_END / GUARD_DAMAGE: CH3a2) and `m_bDualBusterLandingMpSpent` (`+0x98d0`, written in DUAL_BUSTER_LANDING, cleared by skill 26032: CH2c), both ctor false.
- **Divergence row `GetTotalCannonBallCount` done here, not in CH1q**: June's callers `32bedf60` / `32bedfc0` / `32bee020` compare it with 1 / 2 and `IsFullHyperMode` exactly as March's `CSI_A_CDC_RAPID_CANNON_*` sites (cpp ~32689 / 32729 / 32767), which settles the namesake CH1l2's review asked CH1q to prove. March's x 7 formula now sits only in the `#else`.
- `JUNE_BUGS.md` #500 (Rumble Shot upgrade 26233 outside the Gatling range of `GetStateAnimSpeedRate`).

**Routed.**
- **CH1o:** add the user virtual 0x824 (base 1.0) to `CX2GUUser`, call it from `ResetAnimSpeed` as June does and drop March's Intense Showtime block there (`GetStateAnimSpeedRate` is written, called by nothing). Kickoff amended.
- The cut-in set content and the 0x8b3 names are CH5's (GFx names without extension); the `ShownSkillCoolTime` override for Chung (`32bdba60`) stays CH1o.

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --worktree --files <5 files>`: `3 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 14 #else/#endif carrying other flags' tokens, 0 encoding change(s)`; RESULT FAIL only because `_LIVE` is `//#define`. The `_CORE` view: `1 PASS, 1 PASS~, 3 OTHER`, `0 #else/#endif without the flag token`, `RESULT: PASS`.
- `junediverge.py --check` on the 5 files: `allow-listed: 6`, `RESULT: PASS`. `enumscan.py --check X2GUChung.*`: 0 A/B rows.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH) of `X2GUChung.cpp` and `X2GUUser.cpp`, `_LIVE` off and `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: all four exit 0, 0 errors (`cl /P`: the June path is selected, `HQ_CutIn_Chung_CASH_21` 1 vs 0).
- `DEPLOY OK: X2_offline.exe is 25,895,936 B, dated 2026-11-01 17:33:32, md5 cc063003fed99c73d05595d2407ed564` (built at main 88727edf + these files). `EXESTR OK: 2 of 2 absent` (`USE_CANNON_BALL`, `HQ_CutIn_Chung_CASH_21`: the flag-off build carries no June string).

**Nothing to play-test** (`_LIVE` off). Next: `/x2-phase char-chung CH1l3 review` (Opus 5.5 high), then `CH1l4`.

### CH1l3 - Review (2026-11-01)

**Passes, no source change.** All 13 rows checked against `charport_adis.py chung --phase CH1l3` (read-only).
- **`32c17830`.** Order as written: `+0x8d78 > 0` -> false (`fldz; fcomp; test ah,5; jnp`) = `IsQuickSlotSpecialItemWaiting`; `CommonSpecialAttackEventProcess`
  (unit `+0x494`, slot -1); templet id != slot id -> 0x8bae; `+0x8ea0` = id before `CheckSkillUseCondition`; summon / PVP official / `3202d140` NotPve /
  `3202d3c0` NotDungeon / `329cc370` riding wait; `+0x97a2` (`m_bInStateMobileArmoredMode`, CH1k ctor map) -> only 26200; vslot 0x4f4 == 26 / 28 -> only 26207;
  level max( 1, `GetSkillLevel( id, 0, 0, 1 )` ); vslot 0x6a0 -> `+0x8eb0`; 0x82c true skips to success. Then cool time (slot `+0x24` protected) through 0x6a8 `> 0` -> 226;
  `NowMp < +0x8ea8 + +0x8eb0` -> 2549; `> 4` / `- 5` split, `movzx`, 32bed5d0 -> 10225; type 13 and not training, `GetNumItemByTID( 130208, 0, 0, 1, 0 )` <= 0 -> 26119.
  Success writes `+0x8ea0` / `+0x8ea4`. `PushText( 3, str, white, black, 1, 1.f, 1.f )` decoded from the stack copies (both colours, both orders).
- **`32bd8790` / `32bee490` / `32bf5130`.** Saved `+0x8ea0` / `+0x8ea4` / `+0x8ea8`, restored for use condition 3 / 5 (`ASUT_ANYTIME` / `_EXCEPT_AIR`, probed);
  `+0x88bc` = slot + 1 (`m_iNowSpecialAttack`) only when used, else `+0x8ea0` alone restored. `+0x8ac0` (`m_eSpecialAttackKeyPressed`) is 32bf5130's.
  32bee490: tables `33507b00` / `7b14` / `7b28` / `7b3c` = Normal A, Hyper A, Normal B, Hyper B, hyper while gage `+0xe8` > 0, `StateChange( id, false )`:
  the shared `StateChangeBySkillSlot` (31fd32e0, `GetRemainHyperModeTime() > 0`). `+0x8ac4` = 1 always; 26032 clears `+0x98d0`.
  32bf5130's extra arguments are March's defaults (`GetItemByTID` ( tid, 0, 1 ), `SetSkillCoolTimeLeft` true, the 0x6a4 third 0 as Eve's).
- **`32bed5d0`.** Jump table `32bed6a0` = { 32bed609 total, 32bed620 now }; byte index `32bed6a8` has 0 at +0, +5, +0x10, +0x28, +0xb7 = 26020 26025 26036 26060
  26203. 26228 -> class 117 and `+0x97a0` ? cost <= `+0x128` : cost <= 0; berserk true; three exits as written. **`32bed560`:** `+0x110` / `+0x11c` / `+0x128`.
  The namesake proof stands as the Done note gives it.
- **`32bed8c0`.** `GetSkillTemplet( 26407, 1 )`, `GetSkillAbilityValue( 0x67, level, 0 )` = March's 2-argument overload (June `32206e50` with bPvp false);
  rate is assigned, not multiplied. `bFuture` -> `+0x7cb0` -> `+0xc` (= `GetFutureStateSkillID`), else vslot 0x524 (`GetNowStateSkillID`, funcmap 3208e410).
  `== 26061 || 26233 < id <= 26235` (JUNE_BUGS #500 holds). **`32bee3e0`:** 26207 / 26207 + 0x18; 0x12b with `+0x985c` = 1; `setg` on the now count -> 0x12a.
- **`32bf0800` / `32bf0cc0`.** cutIn[0] at `337adca8 + 0x90 x +0x8ac8`, cutIn[1] at `+0x30`; event 0x8b3 `rand() % 2 == 0` -> `337adc48` (CASH_21); hyper argument =
  now state data byte `+0x10` = `IsHyperState()`; `ActiveSkillShow` order (name, file, vSize, vPosition, hyper, bOnlyLight) from the stack.
- **State ids.** The C++ values are March's (probe: `USI_DAMAGE_SMALL_FRONT` 25, `_BIG_FRONT` 27, `CSI_SI_A_CFG_GUARD_READY` 286, `_MEMO` 252,
  `CSI_CHARGE_CANNON_BALL` 116, `CSI_POWER_DOWN_LANDING` 108); June's 26 / 28 / 298 / 299 / 162 / 154 are the same states in June's numbering (CH0 state map,
  `X2GUChung.h` "June script id" comments; March's own Chung guard code tests `USI_DAMAGE_SMALL_FRONT` there). The names are right; never compare raw June ids.
  `ST_HYPER_ACTIVE_SKILL` 13, `SA_ANI_SPEED` 103, `XUF_DODUM_20_BOLD` 3, `UC_CHUNG_TACTICAL_TROOPER` 117 probed. All 14 `RNSI_*` names match June's `Enum.src.lua`.
- **`ReservedSkillStart`.** One declaration and one call for every combination of `_ARME_LIVE` / `_EVE_LIVE` / `_CHUNG_LIVE`; body = Arme's.

**Gates.** `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range dba0f93d^..dba0f93d`: `3 PASS, 1 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL,
0 ERROR`; `0 deleted, 0 non-ASCII, 0 EOL-mismatch, 14 #else/#endif without the flag token, 0 encoding change(s)` (the 14 are the nested other-flag closers the
Done note lists); RESULT FAIL only because `_LIVE` is `//#define`. `_CORE` view: `1 PASS, 1 PASS~, 3 OTHER`, `0 #else/#endif without token`, `RESULT: PASS`.
`junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.*`: `0 A/B row(s)` (D 1 ignored). One-file probes
(`census.cl_cmd`, `/Zs /EHsc`, no PCH) of `X2GUChung.cpp` and `X2GUUser.cpp`, with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`: all four exit 0, 0 errors.
No build (no source change; `DEPLOY OK` md5 cc063003 stands). CH1l4 kickoff checked, unchanged; CH1l4 is ready.

### CH1l4 - Done (2026-11-01)

All 6 class-map rows are written. `_LIVE` stays `//#define`. funcmap rows `Char Chung CH1l4:` (6); read from `charport_adis.py chung --phase CH1l4` and the Ghidra decompiles, every constant and member offset checked in the listing.

| June | `CX2GUChung::` | kind | vs March |
|---|---|---|---|
| `32c0d570` | `CommonFrameMove` | inside | June body under `_LIVE`: no summoned-NPC life timer; Now cannon ball count from `iClassSyncInt` (`32bf3f60`); carpet bombing ends at `+0x98a4`; Chaos Cannon time also ends by the Dash Z X X count (JUNE_BUGS #501); artillery fire waits `SA_DELAY` (skill 26014, zero for Missile Shower); Bombard Service marks are a map (JUNE_BUGS #502); the state's `USE_CANNON_BALL` rows call `UseCannon` inside their frame and are erased; `+0x98d4` counts down |
| `32c058a0` | `DoCarpetBombing` | inside | a shot when `count x 0.2 <= time` (March waits 1 s more), `y + 2500` with the old y as the landing y (March: random y + 2000), power x (1 + `GetSkillAdditionalPowerRate`) with skill 26023, x `max( 1, +0x98a0 )` from shot 19; the small shells are March's |
| `32c08010` | `ResetArtilleryStrikeData` | inside | also erases buff 418 and self-destructs NPCs 863 / 1419; `m_fSummonedLifeTime` is not reset |
| `32c080f0` | `DoArtilleryStrikeFire` | inside | Missile Shower: gap > 0.15, `(rand % 8 - 4) x 100` along the last shot direction, y + 4000, reset at 15 shells; Quantum Ballista: y + `SA_FALL_HEIGHT` of 26014, no memo variant |
| `32c09390` `32c09ad0` | `DoSateliteRain` / `SateliteRainFrameMove` | = March | read in full: same tick 0.2, big shell 0.5 s before the summoner ends, Bomb1-4 counts 1 / 2 / 4 / 3 and random keys (June's `( r * 5 - 250 ) * 2` = March's `( r - 50 ) * 10`), power rate on each effect. `32c09ad0` is `SateliteRainFrameMove` with `ResetSateliteRain` inlined (CH0's name was wrong; `ch0_hand_names.tsv` fixed). No source |

**Beyond the rows.**
- `CX2GUUser::GetSkillNumericalByName` (June `32007910`) moved up from `CX2GUEve` (flag `SERV_IRUHADEV_JUNE_USER_NUMERICAL_BY_NAME`, defined; Eve's decl and body are `#ifndef`-ed out): Chung reads `SA_DELAY` / `SA_FALL_HEIGHT` through it, as Eve's note planned.
- Registered `RNSI_SA_CHG_CARPET_BOMBING` = 26023 (`X2SkillTree.h`) and `BTI_BUFF_ARTILLERY_STRIKE_SCOPE` = 418 (`X2BuffFactor.h`), both `JUNE_ENUM_NAMES`.
- Members under `_CORE`: `m_iCarpetBombingMaxCount` (`+0x98a4`), `m_fCarpetBombingExtraRoundPowerRate` (`+0x98a0`), `m_mapBombardServiceEffect` (`+0x97f8`), `m_fShootingBindCoolTimeLeft` (`+0x98d4`); `CannonBallGaugeTimerFrameMove()` (`32bf4680`) declared, **body is CH1q's**: `_LIVE` will not link until it exists (CH1q kickoff amended). CH3c / CH3b kickoffs name the writers of the new members.
- JUNE_BUGS #501, #502.

**Checked, not changed.** `+0x9108` is `m_bGrap` (funcmap); `+0x8a48` was read as `m_bHold` from the pair of tests, not matched to a name. `32007910` / `3201ad10` / `31fca8e0` / `32890620` map to `GetSkillNumericalByName` / `GetSkillAdditionalPowerRate` / `GetEquippedActiveSkillTemplet( id, true )` / `GetSkillLevel( id, false, 0, true )`. The decompiler garbled the `CreateInstance` arguments; the stack pushes were traced by hand (all defaults but the landing y).

**Gates.**
- `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --worktree --files X2GUChung.cpp X2GUChung.h X2SkillTree.h X2BuffFactor.h`: `2 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token`; RESULT FAIL only because `_LIVE` is `//#define`. `flagcheck JUNE_USER_NUMERICAL_BY_NAME --worktree --files X2GUUser.* X2GuEve.* Always.h`: `4 PASS, 1 DEFINE`, `RESULT: PASS`.
- `junediverge.py --check` on the 8 files: `allow-listed: 25`, `RESULT: PASS`. `enumscan.py --check X2GUChung.*`: `0 A/B row(s)`.
- One-file probes (`census.cl_cmd`, `/Zs /EHsc`, no PCH) of `X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `X2GUUser.cpp`, `X2GuEve.cpp`: all exit 0, 0 errors (`cl /P`: the June path is selected, `SA_FALL_HEIGHT` 1 vs 0).
- `DEPLOY OK: X2_offline.exe is 25,895,936 B, dated 2026-11-01 19:11:30, md5 443cb60ba33692d5902f8a1f42b24be2, equal to X2/US_SERVICE/x2.exe` (built at main 799ae6d0 + these files; X2Lib stage 523 s).
- `EXESTR OK: 2 of 2 absent in X2_offline.exe` (`SA_FALL_HEIGHT`, `SA_DELAY`: the flag-off build carries no June string).

**Nothing to play-test** (`_LIVE` off). Next: `/x2-phase char-chung CH1l4 review` (Opus 5.5 high), then `CH1m`.

### CH1l4 - Review (2026-11-01)

**Passes with one fix:** the Bombard Service container's shape. All 6 rows checked against `charport_adis.py chung --phase CH1l4` and the Ghidra decompiles (read-only).
- **`32c0d570`.** Block order as written. Vslot 0x198 (own unit) false -> `32bf3f60( +0x8160 )`, the XOR-decoded `KProtectedType` = Now `iClassSyncInt`. Promotion equip loop
  (`SetRenderParam` / vslot 4), Iron Will `+0x96d4` (`> 0` subtract, else 0), shot cancel (vslot 0xa8 on `+0x96e4`, `> 1.0` s, `+0x8a48 == 1 || +0x9108 == 1`
  -> `StateChange_LUA( .., true )`) and `+0x973c` are March's own code (its `GetHold()` / `GetGrap()` / `GetShot()`). Carpet bombing `+0x976c` and
  `+0x9760 < +0x98a4`, else time / count / flag cleared. Chaos Cannon: HP from gage `+0x20` (`<= 0` -> 0), tick, `+0x9894 >= 10` -> 0, `>= 8` and `32bed560 <= 1`
  -> 0 (JUNE_BUGS #501 holds), `<= 0` stops `+0x9798` and the `+0x979c` sound (the sound test sits outside the `g_pX2Game` test, as ported). Mobile armored
  `+0x97a2` / `+0x97b8`. Artillery: type `+0x97d4 == 1` (`CAST_MISSILE_SHOWER`, enum value 1) skips `SA_DELAY` ( 26014, upgrade swap true ), `delay < elapsed`
  then gap += elapsed. `USE_CANNON_BALL` loop: `first <= anim +0x214` (`m_fAnimCurrTime`) or `anim + elapsed < first` -> next, else `32bf5280( second )` and the
  tail shifts down 8; `+0x98d4` subtracts while `> 0`, no clamp.
- **The fix.** June's container is not a unit -> handle map: the object starts at `+0x97f4` (erase `this`), nodes carry 12 bytes (`isnil` at `+0x19`), and the
  filler `32c08970` inserts { distance float, unit index, -1 } through `32c242c0`, which never tests for an equal key (a `multimap`). CommonFrameMove passes
  `+0x10` (unit index) to vslot 0xb0 (`GetUnit`) and stops `+0x14`. `m_mapBombardServiceEffect` is now `std::multimap< float, std::pair< int, Handle > >`
  and the loop reads `second.first` / `second.second`. June also tests the **next** node (`@32c0db97`, decompile `piVar13[4]`) while erasing the current
  one, so its last test reads `end()`'s head node; the port tests the node it erases (undefined behaviour otherwise; the map is never filled, #502 amended).
- **`32c058a0`.** `count x 0.2 <= time` (`test ah, 0x41; jp`), `count % 9`, 2400 / 2000 / 375, y + 2500 and the old y as `fLandPos` (stack traced: the y copy at
  frame `+0x20` is the 13th argument), `GetEquippedActiveSkillTemplet( 26023, true )`, NULL unit returns before the count, level `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`,
  `( 3201ad10 + 1 ) x rate`, `count >= 18` x `( +0x98a0 >= 1 ? it : 1 )`. The other 9 `CreateInstance` arguments are March's defaults. Small shells as March.
- **`32c08010` / `32c080f0`.** Reset clears `+0x97d4`, the protected `+0x97dc`, `+0x97f0`, `+0x9854`, `+0x97ec`, erases BTI 0x1a2 ( 418 ), self-destructs
  0x35f / 0x58b ( 863 / 1419, June `Enum.src.lua` ) at 0.f, and nothing else (no summoned life time). Fire: `type == 1` -> `0.15 < gap`, `( vslot 0x1c8( count ) % 8 - 4 ) x 100` along `+0x96f4`,
  y + 4000, gap 0, `SHELL_GROUND`, count + 1; `>= 15` -> reset (outside the gap test). `type == 2` -> y + `SA_FALL_HEIGHT`( 26014, true ), `QUANTUM_BALLISTA_GROUND_SHELL`
  with `fLandPos` 0, reset.
- **`32c09390` / `32c09ad0`.** Spot-checked: 0.5 / 850 / 0.2, the Bomb3 loop key steps by 6 to 25 (4 shells) and Bomb4 by 7 to 22 (3), `SetPowerRate` at `+0x1d8`.
  The "= March" verdict holds.
- **`GetSkillNumericalByName`.** `CX2GUUser`'s body is byte-for-byte Eve's (Eve's `.cpp` body and `.h` declaration both under `#ifndef`).

**Gates.** CH1l4 commit (`--range d1ddcf8f^..d1ddcf8f`): `_LIVE` view `2 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL`, `0 deleted, 0 #else/#endif without the flag token`,
RESULT FAIL only because `_LIVE` is `//#define`; `_CORE` view `1 PASS, 3 OTHER`, `RESULT: PASS`; `JUNE_USER_NUMERICAL_BY_NAME` `4 PASS, 1 DEFINE`, `RESULT: PASS`.
Review fix (`--worktree --files X2GUChung.cpp X2GUChung.h`): `2 PASS, 0 FAIL`; the 6 deleted lines are CH1l4's own lines inside its `_LIVE` / `_CORE` blocks;
RESULT FAIL only for the `//#define`. `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2GUChung.*`: `0 A/B row(s)`.
One-file probes (`census.cl_cmd`, `/Zs /EHsc`) of `X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `X2GUUser.cpp`, `X2GuEve.cpp`: exit 0, 0 errors.
`DEPLOY OK: X2_offline.exe is 25,895,936 B, dated 2026-11-01 19:33:28, md5 a40ed149c9992c0a12c0c143aa7a8a0e, equal to X2/US_SERVICE/x2.exe` (main d1ddcf8f + the 2 fix files; X2Lib 536 s, PCH rebuilt).

**Nothing to play-test** (`_LIVE` off). CH1m kickoff checked, unchanged; CH1m is ready.

### CH1m - Done (2026-11-01)

All 3 class-map rows are written, all `inside`: June's bodies sit in March's own `InitState` (`32c22540`), `InitStateCommon` (`32c12a30`) and
`InitStateByUnitClass` (`32c1d1d0`) under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, with March's bodies in the `#else` (still `//#define`d).
CH0's class map paired `32c1d1d0` with `InitStateCommon`; March splits the class-gated part out as `InitStateByUnitClass`, so it goes there (map row fixed).

**How it was derived.** `june2017_re/char_chung/ch1m_regsim.py` (Eve's, with Chung's class tests) walks fresh decompiles of `32c12a30` / `32c1d1d0`
once per class; `ch1m_reg_by_class.tsv` is that output plus the four listing rows of the uncarved `32c22540` (CH0's `ch0_reg.tsv`) per class.
`ch1m_gen.py` writes the C++ from it: ids by table name through March's own registration of the table (else the `CHUNG_STATE_ID` member of that
name, else the id another table of the same June id took), callbacks from `funcmap.tsv`, checked against the headers. `ch1m_regcheck.py` parses the
`_LIVE` branches per class: **814 registrations match slot by slot, 0 problems; 163 June ids map one-to-one both ways** (`ch1m_idmap.tsv`).

**InitState `32c22540`** (read from the listing): cut-in jump table `32c22ad8` / bytes `32c22af8` give 0 / 6 -> 0, 20 -> 1, 21 -> 2, 27 -> 5, 110 -> 3,
111 -> 4, 117 -> 6: March's values, no `ASSERT` on `UC_NONE`. `+0x7d04 = 0xa2` / `+0x4088 = 0xd` are March's `InitStateID` inlined. Calls `32c12a30`,
`3202b110`, `32c1d1d0`, roll `31fe22d0`; the C++ calls `InitStateByUnitClass` before `InitStateCommonRandom`, whose head runs the event states
(different ids, same result). START Start / EventProcess / End, WIN Start + `RandomLoseEventProcess` (`320718d0`), LOSE Start,
`InitWaitStateChange( "CSI_WAIT_STATE_CHANGE" )` (7h), then vslots 0x690 / 0x684( 0 ) / `31fd8f00`. **No WIN2 / LOSE2 flag tests** (Eve has them; Chung has none).

**InitStateCommon `32c12a30`** (110, no gate): hyper id 23 = `GetHyperModeTableName( false )` (`31fdd490`; Chung's CIP / CDC / CTT cases were already
there), FrameMove = Chung's `HyperModeFrameMove` (vslot 0x600), End = `CX2GUUser::HyperModeEnd` (vslot 0x604). Bounding = `InitBoundingStates( "CSI_" )`
at June's place (7l). `CSI_JUMP_UP` Start = `JumpUpStart`; `CSI_RIDING_START` = `GetRidingStartStateID()` (June 72); `CSI_RIDING_DAMAGE_BACK` registered
as June (JUNE_BUGS 489). `CSI_CHARGE_CANNON_BALL` takes March's names (CH1q writes June's bodies into them).
**June uses `CX2GUUser`'s callback where March used Chung's own:** `DamageBigStart` (same body) and `DashJumpFrameMoveFuture` (June's base `31fd2520`;
Chung's March copy added 1.6x dash speed in the facing direction, so June's dash jump differs from March's once `_LIVE` is on).
*Corrected by the review:* June's base keeps the 1.6x (`m_fDashJumpSpeedRate`, base ctor `@32040f15`; Chung's ctor `32bf1040` leaves +0x8fbc alone) and
adds one case, both arrow keys held. That case is the only difference from March.

**InitStateByUnitClass `32c1d1d0`:** June's two switches; the first (20 joins the default's tail, 27 / 117 jump to `LAB_32c1f0af` with 0x115 / 0x134)
and the second (fallthroughs 110 -> 20, 117 -> 27) are written out whole per class: 6 / 20 / 21 / 27 / 110 / 111 / 117 = 7 / 14 / 22 / 12 / 23 / 27 / 23.
June 181 = `CSI_COMBO_DASH_Z` for all four dash-Z tables (`CSI_CSG_COMBO_DASH_Z` has no March registration). March's tables are a subset of June's.

**Left out, each a comment line with its owner:** phoenix run 123-126 (PvP; June passes the RUN table for 124 / 125 too), dance 127-129 (Stage 10),
riding 93-96 (Stage 17), the empty slots `3202a7d0` (die end, in `StateChange`) and `32002f70` (Stage 17). **Routed to CH2a** (kickoff amended):
Back Blast 335 (`@1543`, only when 26200 is equipped, then vslot 0x7a8) and the shared siege-shelling StateStart `32bda6c0` (4 states registered with
March's FrameMove / EventProcess now). `32078bc0` = `UserUnitStateData`'s constructor (funcmap). **JUNE_BUGS:** none new.

**The `_LIVE` gate (`Always.h`):** `#error` guards for every shared flag the registration needs (`JUNE_CHAR_ARME_CORE`, `JUNE_CHAR_EVE_CORE`,
`HYPER_MODE_FIX`, `JUNE_COMMON_RANDOM_STATE`, `JUNE_EVENT_STATES`, `JUNE_WAIT_STATE_CHANGE`, `JUNE_BOUNDING_STATE`) and CH1l's `JUNE_DIVERGE_SCALE_HEAD`;
the probe with `_LIVE` defined passes them all, which proves each defined at that line.
For CH1n: `june2017_re/char_chung/ch1m_user_callbacks.tsv` lists the 127 `CX2GUUser` members the registration names, with their June targets.
Checklist: 7h, 7l and the 7ao hyper-table bullet marked `(Chung: CH1m)`.

**Checks.** One-file probes (`census.cl_cmd`, `/Zs /EHsc`) of `X2GUChung.cpp`, `_LIVE` off and on: exit 0, 0 errors each. `cl /P`: with `_LIVE` the
`.i` has `InitWaitStateChange( "CSI_WAIT_STATE_CHANGE" )`, `InitBoundingStates( "CSI_" )`, `GetHyperModeTableName( false )` and
`"CSI_CSG_COMBO_DASH_ZXX_END"` once each; without it none. `git diff --patience --numstat`: `X2GUChung.cpp` 1812 / 0, `Always.h` 25 / 0.
`flagcheck.py JUNE_CHAR_CHUNG_LIVE --worktree --files X2Lib/X2GUChung.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 276 deleted line(s), 10 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
RESULT FAIL only for the `//#define`. The 276 / 10 are the default diff misaligning the long insertion against March's identical lines (patience and histogram: 1812 / 0).
`junediverge.py --check X2Lib/X2GUChung.cpp KTDXLIB/Always.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 25,895,936 B, dated 2026-11-01 20:04:21, md5 ba6bf95b0e041a96956a1f7db693146e, equal to X2/US_SERVICE/x2.exe (2026-11-01 20:04:14); built at main 7604fe38 + 2 uncommitted code file(s).`
After the build only the three `#else` lines gained the house `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` token (no compiled change; probes rerun, 0 errors).

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-chung CH1m review` (Opus 5.5 high).

### CH1m - Review (2026-11-02)

**Verdict: passes, no source change.** One fix, to the Done note only: June's dash jump keeps the 1.6x (correction above).

- **Simulation.** Fresh Ghidra decompiles of `32c12a30` / `32c1d1d0` through `ch1m_regsim.py` reproduce the 898 non-listing rows of
  `ch1m_reg_by_class.tsv` exactly (line endings aside); no `UNKNOWN COND`. The overrides hold: `+0x867c == 0` only zeroes the class (the unit
  exists at `InitState`), every `piVar != 0` block is a shared_ptr release (lock xadd, vslot +4 / +8, no registration), and `iVar4` at `@1543` is
  `31fca8e0( 0x6658 = 26200, 1 )`, Back Blast's own gate. `ch1m_regcheck.py`: `matched 814 registrations, 0 problems; ids mapped 163`.
- **Gates of `32c1d1d0`, from the exe bytes.** Switch 1 (bytes `32c1f44c`, table `32c1f430`): 20 / 21 / 27 / 110 / 111 / 117 each have a slot, the
  other 92 values take default `32c1eb25`. 20 ends at `code_r0x32c1f09d` (`push 0x3350e0b8` "CSI_ATTACK_DASH_X"), which default also reaches. 27 and 117
  jump to `32c1f0af` with 0x115 / 0x134. 110 / 111 / 21 break with `CIP_` / `CSG_` / `CSG_COMBO_DASH_X` into the 0xb6 tail (Init / FrameMove /
  EventProcess / EndFuture). Switch 2 (bytes `32c1f4c8`, table `32c1f4b0`): 21 and 111 share a slot; 110 (`f0e5`) falls into 20 (`f14a`), 117 (`f1a9`)
  into 27 (`f1ef`); the rest go to `32c1f409`. The tsv per class gives 182 = `ATTACK_DASH_X` (6, 20, 27), `CSG_` (21, 111), `CIP_` (110), `CTT_` (117).
- **`32c22540` (capstone sweep, not carved).** Its byte table `32c22af8` (0, 6 -> 0; 20 -> 1; 21 -> 2; 27 -> 3; 110 -> 4; 111 -> 5; 117 -> 6) indexes
  `32c22ad8`, which gives cut-in 0 / 1 / 2 / 5 / 3 / 4 / 6, as in the port; no `ASSERT`. Then `+0x7d04 = 0xa2`, `+0x4088 = 0xd` and the calls `32c12a30`,
  `3202b110`, `32c1d1d0`, `31fe22d0`. The registrations: START (9) slots 2 / 6 / 8 = `31ffbd70` / `3201b390` / `3201b460`, WIN (0xb) 2 / 6 =
  `320004b0` / `320718d0`, LOSE (0xc) 2 = `320007e0`, 0x82 "CSI_WAIT_STATE_CHANGE" slot 6 = `31fc8620`. stateData sits at `esp+0x14` and slot n is
  the pair at +0x14 + 8n. There is no flag test between the registrations, and the tail is vslot 0x690, vslot 0x684( 0 ), `31fd8f00`.
  Order: June's `32c1d1d0` runs after the event states, the port's before them. That is harmless: the event states are `USI_*` below `USI_END`
  and every Chung id is `CSI_BASE` (`USI_END + 1`) or above, so neither set overwrites the other.
- **Ids.** `ch1m_idmap.tsv` is one-to-one both ways (no duplicate on either side). June 72 = `GetRidingStartStateID()` (`USI_RIDING_START`);
  June 181 = `CSI_COMBO_DASH_Z` under all four dash-Z tables.
- **Callbacks from `CX2GUUser`.** `DamageBigStart` is `CommonStateStart(); CreateStepDust();` in both classes. Neither member is virtual, so
  `SET_CB_FUNC( CX2GUUser, ... )` binds the base and not Chung's hiding copy.
- **Left out / routed.** Each left-out state has its comment line with an owner. CH2a's kickoff carries Back Blast 335 (`@1543`-`@1549`, vslot 0x7a8)
  and `32bda6c0` on the four siege-shelling states, plus the `LEFT` / `EMPTYOK` edits to `ch1m_regcheck.py`.
- **`Always.h`.** Each `#error` names the flag that declares what the registration calls: `ARME_CORE` (`StartStateStart`, `DamageBigStart`,
  `DashJumpFrameMoveFuture`, `GetHyperModeTableName`), `HYPER_MODE_FIX` (`HyperModeStartFuture` / `HyperModeEnd`), `COMMON_RANDOM_STATE`
  (`RandomLoseEventProcess`), `EVENT_STATES`, `WAIT_STATE_CHANGE` and `BOUNDING_STATE`. `EVE_CORE` (HyperModeEnd body) and `SCALE_HEAD` are each
  used in the `.cpp`. The `_LIVE` probe compiles, so all of them are defined at that line.

**Hygiene.** `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range 52d2db77^..52d2db77 --files X2Lib/X2GUChung.cpp KTDXLIB/Always.h`:
`files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `276 deleted line(s), 10 non-ASCII inserted line(s)`,
RESULT FAIL only for the `//#define`. The misalignment noted in the Done note is confirmed: `git diff --patience --numstat` gives `1812 0` / `25 0`, and
the patience diff's inserted lines contain no non-ASCII byte. `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GUChung.cpp`: `0 A/B row(s)`. One-file probes of `X2GUChung.cpp` (`/Zs /EHsc`): `off exit 0 errors 0`, `on exit 0 errors 0`.
No build (no source change; `DEPLOY OK` ba6bf95b stands). **Nothing to play-test.** CH1n kickoff: line ref and callback table added.

### CH1n - Done (2026-11-02)

**Head (`32c1f530`), inside, `_LIVE`.** June's head sits in March's own `SetEquippedSkillFuncBySkillSlotIndex` up to the id switch; March's head
and type switch are the `#else`. Read from the decompile and the exe bytes:
- Bounds `0 > slot || 4 < slot`; tables `33507b00` / `33507b28` (normal A / B = 50.. / 60..) and `33507b14` / `33507b3c` (hyper 51.. / 61..) = March's
  four arrays; `31fd2af0` dropped (7i), `+0x11` = `IsSkillSlotState`, hyper `0x101`. `GetEquippedActiveSkillTemplet( id, true )`.
- **Batch `3207c1a0` / `31fdf970`:** begin takes the state vector `+0x7cec` and the protect byte `+0x7cfc` into a `std::set<ushort>`; the accessor
  `31fd88f0` records each id; the end re-protects each recorded entry (`31c594b0`). Anti-tamper only, as Aisha's CH1l found: nothing to port.
- **Transcendence memo** (`31fdbcc0`, then `GetSkillTemplet( memo )` must exist): 26030 / 26052 Shooting Star (READY, ATTACK 0xe9, END 0xea),
  26220 Head Shot with `CSI_SI_A_CSG_HEAD_SHOT_DefaultFrameMove` (`32bfc860`), 26221 / 26235 Rapid Cannon (READY, LOOP 0x13e, FINAL 0x13f), 26225 Head
  Shot with its full FrameMove (`32bfcc80`); any other memo registers nothing. **CH0's "no transcendence-memo case in Chung's switch" was wrong.**
- **Types:** 5 buff (`CSI_COMMON_BUFF` both, FM vslot 0x5f0, EP `31fcbd60`); 6 / 7 / 0xd-0x10: 26601 / 26604 Outrage Strike, 26602 / 26605 Satellite
  Rain, 26600 / 26603 Doom Striker (EP `31fcc740` = `GenericSpecialActiveSkillEventProcess`), `> 26600` Lunatic Blow, else the id switch; 0xc + 900 throw
  item (vslots 0x5f4 / 0x5f8); anything else returns.
- **`SetLunaticBlowSkillFunc`** (new, `_CORE`; June `@32c220ae`, the default of both June switches) = March's Lunatic Blow case (START / _HYPER,
  EXPLOSION 0xd7, EXPLOSION_FINISH 0xd8). Under `_LIVE` March's id-switch `default:` (which falls into Detonation) calls it instead.
- Every callback the head names is a March `CX2GUChung` member of the same name (class map column 4); their June bodies are CH1g2 (Head Shot, done),
  CH2a (Lunatic Blow), CH3b (Shooting Star, Rapid Cannon), CH3d (hyper actives). The id switch stays March's until CH2-CH3.
- Enum names (`JUNE_ENUM_NAMES`, `X2SkillTree.h`): `RNSI_SA_CSG_SHOOTING_STAR_TRANS(_UPGRADE)` 26030 / 26052, `RNSI_A_CDC_GATLING_SHOT_TRANS` 26221,
  `RNSI_HA_CIP_DOOM_STRIKE` / `_CDC_OUTRAGE_STRIKE` / `_CTT_SATELLITE_RAIN` 26600-26602 and their `_UPGRADE` 26603-26605 (June `Enum.src.lua`).
- `32c24400` is not a std template: it is March's `CFuryGuardianGuardSkill::CreateFuryGuardianGuardSkill`, called once in the Guard case (CH2b); map row fixed.

**Dev free-skill gate.** `32c1f530` has no cool-time / MP check; the gate is CH1l3's in `SpecialAttackLookup` (`32c17830`, `X2GUChung.cpp`
`IsDevUnlimitedSkillUse()`). Checklist §7 dev-gate row and 7i marked `(Chung: ...)`.

**Base callbacks (127, `ch1m_user_callbacks.tsv`).** None needs an `#if` widened: no body is gated on `_ARME_LIVE` / `_EVE_LIVE`; Aisha's and Eve's
ported bases (START, WIN / LOSE, DamageBigStart, DashJumpFrameMoveFuture, HyperModeEnd, RandomLose) are under `_CORE` flags, so every class has them.
A triage of every June body against March's live C++ (`june2017_re/char_chung/ch1n_user_callback_triage.tsv`; spot-checked: `31fd23e0`,
`31fcc150`, `3201b460`, `31ffd490`, `31fcbe80`) gives **58 SAME, 69 DIFF**. Too large and not Chung's own, so it is split (scope rule):
- **CH1n2 (new, this plan, Opus):** the 16 non-riding rows with a real June difference, for every class under their own flag: Walk / Jump / Dash
  FrameMoveFuture (both arrows held -> facing from oneRight / oneLeft, `31fd23eb`), DashStartFuture / DashJumpLandingStartFuture (CommonStateStartFuture
  first), DamageAirSmallEventProcess (airborne skill use, condition 6, `31fcc169`), StartEnd (`3201b460`), HyperModeStart (`31ffd55a`, `31f4b1e0`),
  PEPPER_RUN x5, THROW_ITEM FrameMove / EventProcess / StateEnd.
- **Kept, already decided:** StartStateStart's PvP round particles (no PvP offline), StartEventProcess's `3200ac50` (Stage 19), HyperModeEnd /
  RidingHyperModeEnd's Add stand-in, RandomLose's `find` guard.
- **Stage 17:** the other 48 `Riding*` rows (bullet added in `JUNE_PORT_BACKLOG_ORDER.md`).

**Checks.** One-file probes of `X2GUChung.cpp` (`/Zs /EHsc`, US_SERVICE defines): `_LIVE` off exit 0 / 0 errors, `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`
exit 0 / 0 errors. `git diff --patience --numstat`: `X2GUChung.cpp` 245 / 0, `X2GUChung.h` 3 / 0, `X2SkillTree.h` 14 / 0.
`flagcheck JUNE_CHAR_CHUNG_CORE --worktree --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_CHAR_CHUNG_LIVE --files X2Lib/X2GUChung.cpp`: FAIL only for the `//#define` (by design). Each: `0 deleted line(s), 0 non-ASCII inserted line(s),
0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `junediverge.py --check X2Lib/X2GUChung.cpp
X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`: `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`: 0 A/B rows. Funcmap rows `Char Chung CH1n` (7). No JUNE_BUGS row.
`DEPLOY OK: X2_offline.exe is 25,896,960 B, dated 2026-11-02 04:27:43, md5 062caaaf0f70f793b0fd6c8d5d146d6c, equal to X2/US_SERVICE/x2.exe (2026-11-02 04:27:36); built at main 9fc55a44 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and the flag-off exe gains no string.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-chung CH1n review` (Opus 5.5 high), then CH1n2.

### CH1n - Review (2026-11-02)

**Verdict: passes, no source change** (no rebuild; the deployed `062caaaf` stands).

- **Head, against the listing** (`32c1f530`): bounds `0 > slot || 5 <= slot` before `3207c1a0`; tables from the exe bytes `33507b00` = 50..58,
  `33507b14` = 51..59, `33507b28` = 60..68, `33507b3c` = 61..69 (March's four arrays); `31fd2af0` on the normal only (dropped, 7i); normal `+0x11 = 1`,
  hyper word `+0x10 = 0x101`; `31fca8e0( id, 1 )`; memo `31fdbcc0( id )` then `32206450( memo, 1 )`, a NULL templet falls to the type switch.
- **Memo switch** (`memo - 26030`, `JA` past 0xcd, bytes `32c22264` / targets `32c22250`): 26030 / 26052 -> 0, 26220 -> 1, 26221 / 26235 -> 2,
  26225 -> 3, the rest -> `32c2221d` (nothing). Delegates are 8 bytes from `+0x14` (Init, StartFuture, Start, FMFuture, FM, Camera, EP, ...); every
  `LAB_` thunk resolved through its `E9`: Shooting Star READY Init / FM / EP `32bd9b70` / `32c0df80` / `32be28b0`, hyper copies FM + EP; ATTACK 0xe9
  Start / FM / EP `32bd9b80` / `32bfc310` / `32beb940`; END 0xea EP `32be28e0` (shared tail `32c22198` sets only the EP `this`). Head Shot 26220 Init
  `32bd9c10`, FM `32bfc860` (= `_DefaultFrameMove`, CH1g2), EP `32be2ad0`, hyper the same table, FM + EP; 26225 the same with FM `32bfcc80`.
  Rapid Cannon READY StartFuture / Start / EP `32bdb130` / `32bf3900` / `32bee140` on both, hyper `+0x10 = 1`; LOOP 0x13e Init / SF / FM / EP
  `32bdb160` / `32bdb1b0` / `32c079f0` / `32bee210`; FINAL 0x13f SF / FM / EP `32bdb1d0` / `32c07cf0` / `32be8360` (tail `32c22190`). = the port.
- **Type switch** (`type - 5`, bytes `32c22344` / targets `32c22334`): 5 buff (FM thunk `329ccd90` -> `32a77a20` = `jmp [eax+0x5f0]`, EP `31fcbd60`);
  6 / 7 / 13-16 actives; 12 throw item; 8-11 and the rest nothing. March's enum: ST_BUFF 5, ACTIVE 6, SPECIAL_ACTIVE 7, RELATIONSHIP 12, HYPER_ACTIVE 13,
  GRADE_1-3 14-16. Actives: `> 26600` (table `32c2252c`: 26601 / 26604 Outrage `32bdb610` + EP `31fcc740`, 26602 / 26605 Satellite Rain Init / EP / FM
  `32bdb550` / `32be8bb0` / `32c08fa0`, 26603 Doom Strike, else `32c220ae`), `== 26600` Doom Strike Init `32bdb6a0` + EP `31fcc740`, else the id switch
  (default `32c220ae`). Throw item: `id == 900` (`SI_ETC_WS_COMMON_LOVE`, the live value), thunks `329ccda0` / `329ccdb0` = vslots 0x5f4 / 0x5f8.
- **`@32c220ae` = `SetLunaticBlowSkillFunc`** = March's Lunatic Blow case (`X2GUChung.cpp:5658`): START FM / EP `32bf0a90` / `32bedf60`, `_HYPER`
  copies both; EXPLOSION 0xd7 Init / FM / EP `32bd9160` / `32c11e10` / `32beb7f0`; FINISH 0xd8 `32bd91d0` / `32bfa2c0` / `32be0d50`.
- **Call site:** March's head and type switch are the `#else`; under `_LIVE` the id switch's `default:` calls the helper and Detonation keeps its own
  case; the id switch ends the function (`X2GUChung.cpp:7191`), so the head's early returns skip nothing. `stateID` kept as March sets it (June's Init
  zeroes `+4` and passes the raw id to MakeTableReference: same table, same id).
- **Enum names:** the 9 values equal `Enum.src.lua`; no non-`RNSI_` `SKILL_ID` member has an explicit value in 20000-26999, each value appears once.
- **Triage** (`ch1n_user_callback_triage.tsv`): counts reconcile (non-riding 36 SAME / 20 DIFF = 16 CH1n2 + 4 kept; riding 22 / 49 = 48 Stage 17 +
  RidingHyperModeEnd kept; 58 / 69). Re-read against the listings: Walk / Dash FMF (both-keys block), DashStartFuture / DashJumpLandingStartFuture
  (Common first), DamageAirSmallEventProcess (`vslot 0x7f4( 6 )` airborne), THROW_ITEM_EventProcess (foot test, y -= 30), PEPPER_RUN_JUMP_UP_SSF
  (`+0x8fc1` skip), JUMP_UP_FMF, PEPPER_RUN_END_FM (`31f54a10` on `+0x93bc`), StartEnd / HyperModeStart / THROW_ITEM FM / StateEnd evidence at the
  cited addresses: all DIFF as written. SAME re-read: HyperModeStartFuture, DamageAirSmallStartFuture (`+0x80cd` is in `m_FrameDataNow`, base `0x7fb4`),
  DAMAGE_GROGGY_EventProcess, THROW_ITEM_StateStart, PEPPER_RUN_END_EventProcess: all SAME. The 5 kept rows are the Done note's. `CH1n2.md` lists the 16.
- `32c24400` = `CreateFuryGuardianGuardSkill` (class map row) as fixed in CH1n.

**Hygiene** (`--range d2c23a9b^..d2c23a9b`). `JUNE_CHAR_CHUNG_CORE`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
`JUNE_CHAR_CHUNG_LIVE`: `files: 0 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: FAIL` (`the flag is not #defined anywhere tracked`, by design).
`JUNE_ENUM_NAMES --files X2Lib/X2SkillTree.h`: `files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
Each: `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
`git diff --patience --numstat`: 245 / 0, 3 / 0, 14 / 0. `junediverge.py --check-all`: `allow-listed: 160`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp`:
`0 A/B row(s) remain in 1 file(s)`. Probes of `X2GUChung.cpp` (`/Zs /EHsc`, US_SERVICE defines): `_LIVE` off exit 0 / 0 errors, on exit 0 / 0 errors.

User owes nothing. Next: `/x2-phase char-chung CH1n2` (Opus 5.5 high).

### CH1n2 - Done (2026-11-02)

**Flag `SERV_IRUHADEV_JUNE_USER_BASE_STATE_BODIES`** (defined, every class; guards: `_CHAR_ARME_CORE`, `_CHAR_EVE_PASSIVE`, `_DIVERGE_SKILL_LEVEL_VALUE`,
`_ENUM_NAMES`). June's body inside each March `CX2GUUser` member, March in `#else`. Each row re-read against the listing; funcmap rows `Char Chung CH1n2` (22).
- **Facing with both arrows held** (`31fd23eb`, inlined 6 times in June): new `UpdateFutureIsRightByArrowKeys()` (both held -> `oneRight` / `oneLeft`
  decides; false when none held). Walk (`31fd23e0`: speed or 0), Jump (`31fd2470`: speed only when held), Dash (`31fd262e`: after March's speed test),
  PEPPER_RUN / JUMP_UP / JUMP_DOWN FrameMoveFuture (`31fd1c90` / `31fd1d80` / `31fd1e60`).
- **Dash starts** (`31fcbe80`, `31fcbe40`): `CommonStateStartFuture` before the pet. June's pet getters `32d27830` / `32d27810` pick `+0x560` / `+0x55c`
  while pet `+0x5a0` is set: stand-in lines, March's `m_DashState` / `m_JumpLandingState` (Stage 17 bullet).
- **DamageAirSmallEventProcess** (`31fcc150`): airborne -> `SpecialAttackEventProcess( ASUT_DAMAGE )`. Every class's lookup reaches the shared
  `CheckSkillUseCondition` (Aisha CH1k's case: only `ASUT_DAMAGE` / `_EXCEPT_AIR` templets pass); March classes run `CommonSpecialAttackEventProcess` first.
- **StartEnd** (`3201b460`): gate = March's tutorial port; room `+0xd8 == 6` is the room's first own member (`m_PVPGameType` in CX2PVPRoom, `m_DifficultyLevel`
  in CX2DungeonRoom, whose `+0xdc` is the dungeon id), so it is `PGT_WINNER` of the PvP room: no Fight.ogg, no particle. Game type 5 (no name) blocks the
  sound. PvP -> `GameFightNew`. Dungeon (stand-in: June's GT_PVE too) -> `Valentine2017_GameStart` for `DI_EVENT_2017_VALENTINE`, `GameStart` unless defence,
  `ClearScreen( 10 )`; stand-ins: event object `+0xbd8` (`32070f40`), sends 0x67e / `AddServerPacket( 0x67f, 60 )` (Valentine, Treasure), Fools' Day sub-stage
  `+0x64` (Stage 19 bullet). March's empty `CommonStartStateEnd` and the party-bonus particle (off) are not in June.
- **HyperModeStart** (`31ffd490`): in the `+0x8fc3` branch, `RNSI_P_RRF_OPEN_MIND` (24419) learned -> `SetSkillBuffFactors( 24419, 1, true )` (vslot 0x23c).
  `31f4b1e0` @`31ffd61c` = ServerProtocol send of event 0x73d with sub-quest 203630 ("Use Awakening 2 times"): stand-in (Stage 4 bullet).
- **Pepper run:** `+0x8fc1` = new `m_bOnJumpUpLine` (ctor false; June's ctor zeroes it @`32040f1e`); JUMP_UP StateStartFuture (`31fd1d20`) skips its body and
  FrameMoveFuture (`31fd1d80`) runs only CommonFrameMoveFuture while it is set; UT_EVE test null-safe on the templet. Its writer is June's
  `PhysicProcess_Jumping` (`31fd6ce0`, a reworked jump-pad handler): divergent row + allow row, ported by **new CH1n3**. Until then the flag stays false.
  PEPPER_RUN_END_FrameMove (`31fd98b0`): clearing `+0x93bc` is a stand-in line; the named-motion system behind it (`31fdbaf0`, `31fdbb70`, Lua `32747090`) is a
  Stage 10 bullet.
- **THROW_ITEM:** EventProcess (`31fcbc10`) foot test (airborne -> `USI_JUMP_DOWN`, y -= 30). FrameMove (`31ff92d0`, tables `31ffaa38` value-1 / `31ffaa70` from
  1021) gains 19 cases: Lofty 14, Nasod 1024 and Glaves Axe 1026 (no velocity, like the spears), explosions 1027-1031, Soul Sealstone 1032, face-off spheres
  1033-1038, Perkisas 1039, Luriel 1040-1042. StateEnd (`31ffb087`) gains Lofty. The triage's "WOODEN_SPEAR is March-only" was wrong (case 1, `dec eax`).
  Every `SPECIAL_ITEM_*` effect is in March's koms (`DamageEffect.lua`, `DamageEffect_INT.lua`). Second-body anim (`+0x8e84`): stand-in line.
- **Enum names** (`JUNE_ENUM_NAMES`, June `Enum.src.lua` / `DungeonEnum.src.lua`): `RNSI_P_RRF_OPEN_MIND` 24419; SST `LOFTY_SPHERE` 14, `NASOD_SPHERE` 1024,
  `GREEN` / `PURPLE` / `BLUE` / `RED` / `GOLD_EXPLOSION` 1027-1031, `PERKISAS_BREATH` 1039 (1026, 1032-1038, 1040-1042 have no name: numbers with a comment);
  `PGT_WINNER` 6; `DI_EVENT_2017_VALENTINE` 41440, `DI_EVENT_2017_FOOLSDAY` 41460.

**Checks.** One-file probe of `X2GUUser.cpp` (`census.cl_cmd`, `/Zs /EHsc`): exit 0, 0 errors. `git diff --patience --numstat`: Always.h 24 / 0,
X2GUUser.cpp 262 / 0, X2GUUser.h 6 / 0, X2Item.h 10 / 0, X2SkillTree.h 6 / 0, X2PVPRoom.h 3 / 0, DungeonEnum.h 6 / 0.
`flagcheck JUNE_USER_BASE_STATE_BODIES --worktree`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`;
`JUNE_ENUM_NAMES` (4 headers): `files: 2 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`; each `0 deleted line(s),
0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. `junediverge.py --check` (7 files):
`allow-listed: 6`, `RESULT: PASS` (new allow rows: StartEnd's GT_PVP = June `322cc180` type 1; `PhysicProcess_Jumping` -> CH1n3). `enumscan.py --check
X2Lib/X2GUUser.cpp`: `0 A/B row(s)`. No JUNE_BUGS row.
`DEPLOY OK: X2_offline.exe is 25,902,080 B, dated 2026-11-02 05:28:04, md5 f179e5e90c14c477f3c57d420ee13501, equal to X2/US_SERVICE/x2.exe (2026-11-02 05:27:57); built at main 6a920926 + 7 uncommitted code file(s).`
`EXESTR OK: 4 of 4 found in X2_offline.exe, 25902080 B, dated 2026-11-02 05:28:04, md5 f179e5e90c14c477f3c57d420ee13501` (`GameFightNew`, `Valentine2017_GameStart`,
`SPECIAL_ITEM_LOFTY_SPHERE`, `SPECIAL_ITEM_LURIEL_BUFF_SHPERE_SPEED_ACCEL`, wide).

**Play-test** (Aisha or Eve, every class has it): both-arrow facing on walk / jump / dash, a damage-condition skill while hit in the air, throw items
(Green Fire Cracker `100002331`, Pesop's Wind Orb `100003449`, Perkisas's Crystal Ball `100003918`; no June item carries Lofty 14) thrown on the ground and
in the air, a dungeon start (Fight.ogg, GameStart). Pepper run is unchanged until CH1n3 (the jump-up flag stays false). Next: the play-test, then
`/x2-phase char-chung CH1n2 review` (Opus 5.5 high); CH1n3 and CH1o follow the review, one at a time (both touch `X2GUUser.*`).
**Play-test passed (user, 2026-11-02):** all 6 checks. Logs on md5 f179e5e9 (run 06:29-06:33, after deploy): 0 EXCEPTION, 2 UNHANDLED (`E_DISABLE_HB_CHECK_REQ`,
known); the one unbound name, `AddGroupChainQuest` in `ChainQuestInfo.lua`, is quest-system's (Q0a / Q1), not this phase's.

### CH1n2 - Review (2026-11-02)

**Verdict: passes, no source change** (no rebuild; the deployed `f179e5e9` stands). Every body was re-read against `charport_adis.py chung <va>`.
- **StartEnd** (`3201b460`): CommonStateEnd first; gate vslot 0x198 / `g_pMain +0x16a` / `+0x8b61`. Room `+0xd8` is the PvP game type: June's own
  StartStateStart tests `+0xd8 == 0xa` (`PGT_BEST_OF_N_GAMES`) @`31ffbe86` before `L"PVP_Best_OF_N_Round"`. A dungeon room's `+0xd8` (difficulty 0-2)
  or a battle-field room's (field id) is never 6, so the port's PvP-room-only test is the same. Fight.ogg: type 5 or a null room still plays, as in
  June. PvP: null room or winner -> no particle; `GameFightNew` then `m_bStarted` and return. Dungeon (type 2 or IsPve): `+0xc44` -> `[0]` -> `+0x10` id;
  `32070f40` true -> GameFightNew, then straight to the defence test (stand-in holds). 0xa1e0 / 0xa1fe / 0xa1f4 = 41440 / 41470 / 41460. `IsDefenceDungeon`,
  `GameStart`, `ClearScreen( 10, (1,1,1,1) )` = March's default colour. `m_bStarted = 1` on every path (@`3201b8d3`). March's CommonStartStateEnd and
  party-bonus particle are not in June.
- **HyperModeStart** (`31ffd490`): gauge > 0 and `+0x8fc3` (= `GetCanChangeHyperModeInHyperModeState`) -> count, then `31fceab0( 0x5f63 ) > 0` ->
  vslot 0x23c `( 0x5f63, 1, 1 )`; `+0x8fc3` false skips both, as the port's `#ifdef SUPERPOSITION` block does. `+0x9176` / `+0x8d4c` cleared, then
  `31f4b1e0( 0x73d, 203630 )` (stand-in).
- **THROW_ITEM_FrameMove** (`31ff92d0`): `> 0x3f2` -> `31ffaa70` (value - 0x3fd, 22 entries), `== 0x3f2` -> `EVENT_SPECIAL_ITEM_LIGHT_SPEAR`, else
  `dec eax` -> `31ffaa38` (1..14; case 1 = `SPECIAL_ITEM_WOODEN_SPEAR`, string read from the exe). The velocity tail `31ffa9b2` is taken by 3-10, 12-14
  and 1027-1042. 1-2, 11, 1010 and 1021-1026 skip it; 1023 / 1025 are empty. Every string and tail matches the port. June has no 1011: the US
  defines agree (`EVENT_ICICLE_THROW_ITEM` BR only; `EXESTR OK` for the light spear and fire cracker, the icicle spear absent). StateEnd `31ffb0fc`
  (value - 3, 12 entries): 3-8, 13 anger, 14 Lofty @`31ffb087`, 9-12 nothing = the port. Lofty 14 and the 8 SST names = `Enum.src.lua`.
  EventProcess `31fcbc10`: airborne `StateChange( 0x11 )` and then y -= 30; on foot `m_Wait`. `IsOnSomethingFuture()` = `bFootOnLine` (`+0x7e1c`).
- **Facing:** walk `31fd23eb`, jump `31fd247b`, dash `31fd262e` (after the speed test), pepper run `31fd1ca4` / `31fd1da4` / `31fd1e77`. All six are the
  same block. When both keys are held, the speed is set even with neither one-key, so the helper returns true. Walk: no arrow -> `fldz`. Jump: no
  arrow -> speed untouched.
- **Pepper run:** `+0x8fc1 == 1` returns from StartFuture and leaves FrameMoveFuture with only CommonFrameMoveFuture. The templet at unit `+0xaf0` is
  null-safe and `[0]` = `m_UnitType`: 5 -> 2300. y += 30 = `LINE_RADIUS`. The June ctor zeroes the word `+0x8fc0` @`32040f1d`. The PEPPER_RUN_END
  stand-in holds.
- **Dash starts** `31fcbe80` / `31fcbe40`: CommonStateStartFuture first, then GetRidingOn (dash only), `+0x683c` pet, `32d27830` / `32d27810` (stand-ins).
  `nowSpeed.x = 0` / `*= 0.5`. **DamageAirSmallEventProcess** `31fcc150`: foot -> `StateChange( 0x12 )`, else vslot 0x7f4 `( 6 )`, then
  CommonEventProcess. Each class passes the condition through to `CheckSkillUseCondition`, which takes `ASUT_DAMAGE` under `_ARME_CORE`. That holds
  for Elsword, Ara, Lire, Raven, El and Add directly, and for Aisha, Eve and Chung (`_LIVE`) via SpecialAttackLookup. Lire's Night Watcher rewrites
  any non-ANYTIME condition to GROUND for Delayed Firing alone. That is March class code for Lire's own port (Stage 7), not a CH1n2 change.

**Checks.** `flagcheck JUNE_USER_BASE_STATE_BODIES --range 7da7ff34^..7da7ff34`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 4 OTHER (other flags),
0 FAIL, 0 ERROR`, `RESULT: PASS`. The 4 OTHER are the enum headers (`JUNE_ENUM_NAMES`). Same range for `JUNE_ENUM_NAMES` over the four headers: `files: 2 PASS,
2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`. Both show 0 deleted, non-ASCII, EOL or encoding lines. `git diff
--patience --numstat`: no code deletions. `junediverge.py --check-all`: `allow-listed: 162`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B
row(s)`. The one-file probe of `X2GUUser.cpp` gives 0 errors with `_LIVE` both off and on. Play-test (6 checks, 0 EXCEPTION) judged a pass. CH1n3 and CH1o
are ready, one at a time (both touch `X2GUUser.*`); the `CH1n3.md` kickoff was checked.

### CH1n3 - Done (2026-11-02)

**Flag `SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE`** (defined, every class; guards: `_USER_BASE_STATE_BODIES`, `_LINEMAP_CANDOWN_SIGNATURE`). June's `CX2GUUser::PhysicProcess_Jumping`
(`31fd6ce0`, read from the listing) inside March's function, March in `#else`. funcmap row `31fd6ce0` now `ported`.
- **Entry** = March's: foot on line (`+0x7e1c` = `IsOnSomethingFuture()`), line from `lastTouchLineIndex`, key stop `+0xe4 > 0` -> `SetCanNotInputTime_LUA` (`+0xde8`).
- **Types 4 / 5** (`LT_JUMP_UP_REL` / `_ABS`, June blocks `31fd7022` / `31fd6d86`, merged into one case): `m_bOnJumpUpLine = true` *before* the state tests. The four
  tests are vslots 0x114 / 0x118 / 0x494 / 0x490, which return 0x1e / 0x1f / 0x26 / 0x25 (`USI_DAMAGE_DOWN_FRONT` / `_DOWN_BACK` / `_AIR_FALL` / `_AIR_DOWN_LANDING`,
  vtable `334781f4`), the same set as March. The down-drop test is vslot 0x458 = `GetJumpDownStateID()` (Diverge D0c-1: same as March). `CanDown` `31c6c310` with
  ( pos, 30, &landPosition, &index, &index, 1, 0, 1, 0 ) = March's `unsigned short*` overload; y -= 33 (`LINE_RADIUS * 1.1f`).
- **Repeat-touch guard** (`31fd6eec` / `31fd719e`): `g_pMain +0x8c == XS_DUNGEON_GAME` (11) and key stop > 0. `[0x344c0178]` = `winmm!timeGetTime` (import table).
  A new line (`LineData +0 lineIndex`) stores id and time. The same line clears the input lock when key stop < 0.001 x elapsed (`test ah, 5; jp`), then stores the time.
  New members `m_iLastJumpUpLineIndex` (`+0x88f0`) and `m_dwLastJumpUpLineTouchTime` (`+0x88f4`); ctor -1 / 0 (`@3204259a`).
- **REL:** direction `+0xe0` (2 `LDO_RIGHT` / 1 `LDO_LEFT`) on both paths. Its second key-stop write is only outside the guard (`31fd72eb`), as March's has it.
  **ABS:** x flipped when facing left, no direction. Both: `m_bSteppedJumpLinemap`, `HighJump.ogg` (args ( name, true, false ) = March's defaults), `m_vAddJumpSpeed`.
- **Other foot lines:** `+0x8fc1 = 0`, `+0x88f0 = -1`, `+0x88f4 = 0` (June's `default`, which March lacks). The CH1n2 pepper-run skip now goes live: on a pad,
  PEPPER_RUN_JUMP_UP keeps the pad's own jump.
- Divergence: `june_march_divergent.tsv` row marked `ported under`, `june_diverge_allow.tsv` row removed. No JUNE_BUGS row.
- **Routed (scope fence):** Lu/Ciel overrides the four damage getters (`32b4d810` / `32b4d850` / `32b4d9d0`); March has no such virtuals. Stage 8 bullet
  (`JUNE_PORT_BACKLOG_ORDER.md`). No other callee differs (`PlaySound` is already June's under `_DIVERGE_PLAY_SOUND`).

**Checks.** `cl /P` of `X2GUUser.cpp`: June branch live, 9-arg CanDown present. One-file probe (`/Zs /EHsc`): exit 0, 0 errors. `git diff --stat`: Always.h +17,
X2GUUser.cpp +82, X2GUUser.h +4, no deletions. `flagcheck JUNE_USER_JUMP_UP_LINE --worktree`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags),
0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
`junediverge.py --check` (3 files): `allow-listed: 7`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s)`.
`DEPLOY OK: X2_offline.exe is 25,902,080 B, dated 2026-11-02 07:06:23, md5 26d1965e5fea78a2d7411f542b2d1868, equal to X2/US_SERVICE/x2.exe (2026-11-02 07:06:16); built at main 2d243c46 + 3 uncommitted code file(s).`
`EXESTR OK: 1 of 1 found in X2_offline.exe, 25902080 B, dated 2026-11-02 07:06:23, md5 26d1965e5fea78a2d7411f542b2d1868` (`HighJump.ogg`, wide).

**Play-test** (any class; Aisha or Eve): Garpai Rock (Sander, `DI_SANDER_GARPAI_ROCK_NORMAL` 33210; stages 1-3 have key-stop pads of 1-2 s) for the pad, the
hold-down drop, the double touch and a pepper run (Hot Pepper `130507`). Dry Sander (33200) for the direction pads. Next: the play-test, then
`/x2-phase char-chung CH1n3 review` (Opus 5.5 high); CH1o follows the review.
**Play-test passed (user, 2026-11-02):** all checks. Logs on md5 26d1965e (run 07:10-07:14, after deploy; dungeon 33210 played 07:11-07:13): 0 EXCEPTION,
2 UNHANDLED (`E_DISABLE_HB_CHECK_REQ`, known), clean exit. Not this phase's: `DLG_UI_LOCAL_MAP` lacks a "Garpai Rock" control (local-map UI); `AddGroupChainQuest` (quest-system).

### CH1n3 - Review (2026-11-02)

**Verdict: passes, no source change** (no rebuild; the deployed `26d1965e` stands). Re-read against `charport_adis.py chung 31fd6ce0` (456 lines).
- **Entry:** `+0x7e1c == 1` (`IsOnSomethingFuture()`), index in range, line non-null; key stop `> 0` (`test ah, 0x41`) -> `+0xde8` (`SetCanNotInputTime_LUA`).
  `[ebx+4] - 4` -> `31fd7022` = type 4 (`LT_JUMP_UP_REL`, has the direction), `- 1` -> `31fd6d86` = type 5 (`LT_JUMP_UP_ABS`, has the x flip).
  Other types: `+0x88f0 = -1`, `+0x88f4 = 0`, `+0x8fc1 = 0`; off-line / null line touches nothing. Port = June.
- **Both types:** `+0x8fc1 = 1` first (`31fd6d92` / `31fd702a`). Base slots (vtable `334781f4`): 0x114 `3208de60` = 0x1e, 0x118 `3208de70` = 0x1f,
  0x494 `3208dec0` = 0x26, 0x490 `330de330` = 0x25 = `USI_DAMAGE_DOWN_FRONT` / `_DOWN_BACK` / `_AIR_FALL` / `_AIR_DOWN_LANDING` (`ch1m_idmap.tsv`); each
  match exits with nothing else. 0x458 `3208de10` = riding ? 79 : 17 = March's `GetJumpDownStateID()`. `vtslot.py`: all 12 other user classes, Chung
  included, use these bases; only `CX2GULuciel` overrides (see the routing fix below).
- **CanDown** `31c6c310`: ( pos, 30.0, &landPosition, &idx, &idx, 1, 0, 1, 0 ), index written back = March's `unsigned short*` overload -> `int*` forwarder
  under `_LINEMAP_CANDOWN_SIGNATURE` (`KTDGLineMap.h:1299`). True -> y -= 33.0 (`LINE_RADIUS * 1.1f` folds to 33.0f) and return. False -> the guard test.
- **Guard** (`31fd6eec` / `31fd719e`): `g_pMain +0x8c == 0xb` and key stop `> 0`; `[0x344c0178]` called first. Same id: (elapsed, unsigned via the
  2^32 fix-up) x 0.001f vs key stop, `test ah, 5; jp` skips on `>=` / unordered, else `+0xde8 = 0`; time stored on both paths. New id -> `31fd7254`
  stores id and time. Port = June; order (time read, direction, `+0x88ec`, HighJump.ogg ( 1, 0 ), `+0x88e0..e8`, id) = June.
- **REL:** direction `+0xe0` 2 / 1 through vslot 0x170 `GetIsRight` in both paths; second key-stop write only at `31fd72eb` (non-guard). **ABS:** no
  direction, no second write, x negated when `bIsRight != 1` in both paths (guard `31fd6f5f`, non-guard `31fd6fe2`). Port = June.
- **Ctor:** `or ebp, -1` @`3204249e`, `+0x88f0 = ebp` @`3204259a`, `+0x88f4 = ebx` (= the `bl` stored into `+0x88ec` false) = -1 / 0.
- **Routing fix (doc only):** Lu/Ciel overrides four damage slots, not three (`32b4da10` for 0x494 was missing), and 0x458 `GetJumpDownStateID`
  (`32b4d5b0`; non-virtual in March). Stage 8 bullet corrected (`JUNE_PORT_BACKLOG_ORDER.md`). Scope fence held: only `31fd6ce0`'s body changed.

**Hygiene:** `flagcheck JUNE_USER_JUMP_UP_LINE --range 3628840d^..3628840d`: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL,
0 ERROR`, `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
`git diff --patience --numstat`: Always.h 17/0, X2GUUser.cpp 82/0, X2GUUser.h 4/0. `junediverge.py --check-all`: `allow-listed: 163`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`. Probe (`census.cl_cmd`, `/Zs /EHsc`) of `X2GUUser.cpp`: `_LIVE` off and on, exit 0, 0 errors.
**Play-test:** passed (pad, hold-down drop, double touch, pepper run; 0 EXCEPTION on `26d1965e`). Next: `/x2-phase char-chung CH1o` (Opus 5.5 high).

### CH1o - Done (2026-11-02)

**Landed** (flag `_LIVE`, still `//#define`; members under `_CORE`; listings `charport_adis.py chung --phase CH1o`, all 24 rows read):
- **0x6a8 `ShownSkillCoolTime` `32bdba60`:** Chung override; `RNSI_SA_CDC_SHOOTING_BIND` (0x65b7) shows 0 while `m_fShootingBindCoolTimeLeft` (+0x98d4) > 0.
  The base and its `UpdateSkillCoolTime` call are Eve CH1o2's; Chung's own skill-use call (`32c17b82`) is CH1l3's `SpecialAttackLookup` line.
- **0xa4 `IsBerserkByChance` `32c24b90`:** `m_fBerserkChanceOnAwakening > GetRandomFloat( 0 )`. **`+0x9714` identified:** Tussle Technique's "Berserk Chance when
  Awakening" (`RNSI_P_CDC_TUSSLE_TECHNIQUE` 26408, `SA_EFFECTIVE_RATE`), written only by `InitPassiveSkillState` (0 `@32c1757e`, value `@32c175d3`): new member
  (`_CORE`, ctor 0 `@32bf1a6b`); the writer is CH4a's (kickoff amended). Until then the roll is always false. Always.h: `_LIVE` now `#error`s without
  `JUNE_USER_SHOWN_SKILL_COOL_TIME` / `JUNE_DIVERGE_BERSERK_BY_CHANCE` (both bases).
- **User vslot 0x824 (CH1l3 amendment):** base `CX2GUUser::GetStateAnimSpeedRate( bFuture )` returns 1.0 (`3208dd70`); `ResetAnimSpeed` calls it `@31ffe2b0`
  in place of March's Intense Showtime block (`#else`). Chung's `_CORE` body (`32bed8c0`) becomes the override. June's only 0x824 caller is `ResetAnimSpeed`.
  **The rest of June's `ResetAnimSpeed` differs per class:** divergent row + allow row `ResetAnimSpeed` (`31ffe190`), checklist **7av** (Lire's Agile Movement
  override, Add / Ain `+0x92a0` rate, Lu/Ciel dual-body future anim).
- **`InitRemodelMagazinePassiveSkill` `32bf40e0`:** June reads `RNSI_P_CSG_REMODEL_MAGAZINE` 26406 (registered, `JUNE_ENUM_NAMES`), level `( id, 1, 0, 1 )`,
  `SA_EXPAND_CB` -> gauge `SetMaxCannonBallCount` (`32a36ba0`); March read `SI_P_CSG_REMODEL_MAGAZINE` 6406.
- **= March, written under `_LIVE` (house pattern):** `AppendMajorParticleToDeviceList` `32bdc520` (7 names, `0x334b7718` = `DamageImpactRingRed`),
  `InitializeMajorMeshArray` `32bdc600` (June's 19 handle slots are spread over March's 21-entry `CHUNG_MAJOR_MII_END`), `LoadStateIDCanBeChangedGuardState`
  `32bf0ce0` (default 0x99 = `CSI_BASE`, x 0 / y 100). **= March, no edit (header one-liners):** `IsFullHyperMode` `32c24b50` (gauge +0x140), `SetFullHyperMode`
  `32c24b60` (+0x141 = 1, +0x140 = arg), `CCannonBallSystem()` `32c24930` (0.5 / 0 / 0), `SetBasicCannonBallCount` `32c24e50` (+0x9674).
  **CH0 names corrected:** `32c24e50` = dungeon-start setter (CH0: `SetCannonBallCountToBeUpUsingReLoadCannon`), `32c24ed0` = `SetPvpStartCannonBallCount`
  (+0x9680, CH1l's; CH0: `SetBasicCannonBallCount`); both are called only by `LoadPersonalInfo` `32bf5430` (CH1q).

**Decisions (no CH1o2):** June calls user vslots 0x670 (`IsInJobLine`, 135 sites) and 0x774 (`UpdateSpineAimRotation`) only from class code on the class
itself, never from engine code, so both stay plain Chung members (no `CX2GUUser` base). 0x740 / 0x744 are called only by the Crest of Time hook `3201ffb0`
and death prevention `320072d0`: CH4b (amended), with 0x748. 0x688 stays dead (CH1k0).

**Routed (kickoffs amended):** CH4a: `+0x9714` / `+0x9710` writers, delegates `32bdba10` (thunk `32c23090`) and `32c12910` (thunk `32c258a0`) registered by
`InitPassiveSkillState`. CH4c: `32bdb760` = `MemoConditionChung` vtable slot 1 (RTTI `3361d48c`). CH3b: `32bfeb80` (Artillery Strike control helper,
caller `32bff350`). **n/a:** `32c23600` / `32c245a0` / `32c246f0` (std `vector<12-byte>` instantiations), `32c242c0` / `32c24520` (callees of the dead
`32c08970`, JUNE_BUGS #502), `32c24b30` / `32c24b40` (dtor adjustor thunks, secondary vtables `+0x1a0` / `+0x190`). Checklist: L260 / L263 / 7ap / 7t marked.

**Hygiene.** `flagcheck JUNE_CHAR_CHUNG_LIVE --worktree --files <6>`: `3 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII,
0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT FAIL only because `_LIVE` is `//#define`. `_CORE` view of the three code
files: `3 OTHER`, `RESULT: PASS` (Always.h's 6 lines sit in the `_LIVE` block). `JUNE_ENUM_NAMES`: `1 PASS~`, `RESULT: PASS`. `git diff --patience --numstat`: Always.h
6/0, X2GUChung.cpp 79/0, X2GUChung.h 5/0, X2GUUser.cpp 6/0, X2GUUser.h 3/0, X2SkillTree.h 6/0. `junediverge.py --check-all`: `allow-listed: 163`, `RESULT: PASS`.
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/Zs /EHsc`): `X2GUChung.cpp` and `X2GUUser.cpp`, `_LIVE`
off and on, exit 0, 0 errors. `DEPLOY OK: X2_offline.exe is 25,902,080 B, dated 2026-11-02 07:55:15, md5 34c683ba6006503b49155c46e71a52ee` (main 38940bf1 + these 6 files). No exe string: every change is under the undefined `_LIVE`. No play-test: nothing runs until CH1.
Next: `/x2-phase char-chung CH1o review` (Opus 5.5 high).

### CH1o - Review (2026-11-02)

**Verdict: passes, no source change** (no rebuild; the deployed `34c683ba` stands). Listings: `charport_adis.py chung --phase CH1o` (24) and `chung 31ffe190`.
- **`ResetAnimSpeed` `31ffe190`:** extra rate (`!= 1` -> x), then the item block (`bSpeedUpByItem == 1`: protected rate, world-mission `+0x330` x 0.01 unless game
  type 1, then Elsword vigor `+0x961c`); its `jne` lands on `@31ffe2b0`: `[vtbl + 0x824]( bFuture )`, x. Then `+0x88f8` floor 0.1 and x, world `+0x188` rate, `+0x92a0` x,
  future anim `+0x8e90 ? +0x8e84 : m_pXSkinAnimFuture`. The `_LIVE` call sits exactly there; March's Lire block stays after it (products commute) on `int iSkillLevel = 0`.
  The routed rest (`+0x92a0`, the dual body, Lire's override) is everything else that differs. **Sweep:** of 12 `[r + 0x824]` loads in the exe, only `@31ffe2b0` is a
  vtable load; the other 11 (`31d309f2`..`31d30f2e`, `322de71c`, `3296079c`, `3303747f`/`ab`, `3303d4b6`) read data members (vector / string fields).
- **`ShownSkillCoolTime` `32bdba60`:** ( cool `[esp+4]`, id `[esp+8]` ), `ret 8`; `0x65b7` = 26039 = `RNSI_SA_CDC_SHOOTING_BIND`; `fldz; fcom [+0x98d4]`, `test ah,5; jnp`
  taken only for C0 alone (0 < member) -> returns the 0 on ST0; equal / greater / unordered -> the cool time. Port = June.
- **`IsBerserkByChance` `32c24b90`:** vslot 0x1cc `GetRandomFloat( 0 )` first, then `fld [+0x9714]`: `fcompp` compares member (ST0) with roll; `test ah,0x41; jne` -> false on
  `<=` / unordered, so true when member > roll. Port = June. Writers `@32c1757e` / `@32c175d3` stay CH4a's.
- **`InitRemodelMagazinePassiveSkill` `32bf40e0`:** `GetSkillLevel( 0x6726, 1, 0, 1 )` (pushes reversed), `> 0`; `GetSkillTemplet( id, 1 )` = March's one-argument form
  (UPGRADE_SKILL_SYSTEM_2013); `GetSkillAbilityValue( 0x66, level, 0, 0.0f )`: `32206e50` is `ret 0x10`, the 4th argument is the miss default (`fld [esp+0x60]`), 0 = March's
  `return 0.f`, bPvp 0 = the two-argument getter; `_ftol2`; gauge `[+0xc04]` -> `32a36ba0` sets the protected int at `+0x11c` (`ret 4`) = `SetMaxCannonBallCount`. No else
  branch in June either. Port = June.
- **Verdict rows hold:** `32c24b50` (`+0x140`), `32c24b60` (`+0x141 = 1`, `+0x140 = arg`) = March. `32bdc520` 7 names (`[g_pData+0x84]` null test = `GetGameMajorParticle`),
  `32bdc600` 19 x -1 from `+0x95c8`, `32bf0ce0` (default `0x99`, `[2]` 0, `[3]` 100.0), `32c24930` (0.5 / 0 / protected 0) = March. `32c24e50` (+0x9674) / `32c24ed0` (+0x9680)
  are byte setters with only `LoadPersonalInfo` callers, CH0 names corrected as the note says. Routed and n/a rows as listed; nothing to add.
- **0x670 / 0x774 decision holds:** 126 vtable loads of 0x670, all in class code (`329d..` to `32cf..`; Chung's 4 at `32bde360` / `32bdee8e` / `32bdef64` / `32bdf01b`, its combo
  handlers, `ecx = this`); `324ef57c` is a member read `[ebp + 0x670]`. 0x774: 5 loads, `32be5b25`, `32c0e566`, `32c0ea11` (Chung, `mov ecx, esi`) and `32c6af88` / `32ca535b`
  (other classes' own frame moves on `esi = this`, which reach the empty base `31f3f070`). No engine caller, so no base virtual is needed for Chung.

**Hygiene** (range `1e2bf4ce^..1e2bf4ce`): `flagcheck JUNE_CHAR_CHUNG_LIVE`: `3 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched,
0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only for `define: NOT FOUND` (`//#define`). `_CORE --files` the four class/user files: `4 OTHER, 0 FAIL`,
`RESULT: PASS`; `ENUM_NAMES --files X2SkillTree.h`: `1 PASS~`, `RESULT: PASS` (whole-range runs trip on Always.h only: the `_LIVE` guard lines and an old compound `#if` at
:10175 flagcheck cannot parse). Both new `#error` guards' flags are defined above the block (Always.h:10235, :10448). `git diff --patience --numstat`: code 6/0, 79/0, 5/0, 6/0, 3/0,
6/0. `junediverge.py --check-all`: `allow-listed: 163`, `RESULT: PASS`. `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`. Probes
(`census.cl_cmd`, `/Zs /EHsc`): `X2GUChung.cpp` and `X2GUUser.cpp`, `_LIVE` off and on (`/D`), exit 0, 0 errors, warnings equal off / on (38 / 39). No play-test.
Next: `/x2-phase char-chung CH1p` (Opus 5.5 high).

### CH1p - Done (2026-11-02)

Flags: `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (insert-only, `//#define`); one `_CORE` comment fixed. No new flag, no new `Always.h` guard (`_LIVE` already
needs `_ARME_CORE` and `_EVE_CORE`, which hold the reader and `SkillSlotKey`).

**Method.** I ran `cl /P` on `X2GUChung.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` and listed every surviving line that reads a slot id, the pressed index,
`m_iNowSpecialAttack` or `m_eSpecialAttackKeyPressed`. Each is checked against June's listing.

**7ag, the pressed-index space.**
- **Already June's** (CH1l3 / CH1n): `SpecialAttackLookup` (`32c17830`: calls `31fcadd0` `@32c178ab`, decodes `> 4` / `- 5` for `GetCBConsumption` `@32c17e7f`), the encoders
  (`m_iNowSpecialAttack = slot + 1`, `SpecialAttackCooldown`'s `m_eSpecialAttackKeyPressed = slot + 1`), `StateChangeBySkillSlot` / `MarkSkillSlotUsed` (0-9),
  `SetEquippedSkillFuncBySkillSlotIndex`'s head (per-bank 0-4). The CH0 sites `:5923`, `:6064-6081`, `:6087`, `:6090` are March's `SpecialAttackEventProcess`, all in `#else`.
- **Fixed: the shared reader.** `CX2GUUser::CommonSpecialAttackEventProcess` (June `31fcadd0`) still handed every class but Eve / Aisha March's 4 + 4. So under
  `_LIVE`, Q (4) would have decoded as A-TC, and W / E / R as B1-B3. `UT_CHUNG` now joins the June set (`X2GUUser.cpp`, `_LIVE` / `#else`).
- **Fixed: `CurrentSkillKeyManager` (March 4 + 4) fed June's 0-9.** June's Artillery Nova charge (`32bdaf00`, `32be7a30`) and Rapid Cannon (`32bdb130`, `32bee210`)
  use `+0x8b58` = `CX2GUUser::m_kSkillSlotKey` (Eve CH1j20: `Set` `31fce6d0`, `FollowSlotSwap` `31fce670`, `IsKeyReleased` `31fc89a0`). The 6 call sites now use it
  under `_LIVE`. Both StartFutures are then June's whole body. The rest of the EventProcesses is CH3a / CH3b's (kickoffs amended).
- **Fixed: Artillery Strike's control switch** (`ArtilleryStrikeControl_EventProcess`, June `32bfeb80`, which was CH0's unnamed "helper"). June switches on `value - 1`
  over 0-9 (table `32bff328`: A S D C, F `+0x872a`, Q W E R, T `+0x872e`), the same table as `IsKeyReleased`, so the March `SAKP_*` switch became a local
  `SkillSlotKey` decode. June's held-key gate `HaveThisBuff( BTI_BUFF_ARTILLERY_STRIKE_SCOPE )` (418, `@32bfeded`) and the rest of the body go to **CH2c**, which owns
  the only caller `32bff350`. The class map row moved there from CH3b.
- **No change, routed:** G.U.A.R.D. ready StartFuture (`32be1e90`) reads the slot state id, not the pressed index; June has the TC cases and March falls back to A
  (TC locked until Stage 6): CH3a2 kickoff. `CountUsedSkill` (4 + 4) is only in March's `#else`; June counts through `MarkSkillSlotUsed`.
- **Lead, not drift:** `SkillSlotKey` is June's version of March's `CurrentSkillKeyManager` (same `+0x8b58` role, 5 + 5). Each later class port moves its users over
  with the pressed index (checklist 7ag line added). The March struct goes away once no class reads 4 + 4.

**Hooks 7h / 7l / 7o / 7p / 7x** (CH0 rows, all confirmed in the `_LIVE` preprocess): 7h `InitWaitStateChange( "CSI_WAIT_STATE_CHANGE" )`, WIN `RandomLoseEventProcess`
(CH1m), the `EFFECT_SET_LIST*` / `USEFUL_VALUE` rows come with the script (MEMO reader CH4c, Nova's `USEFUL_VALUE` readers CH3a). 7l `InitBoundingStates( "CSI_" )` (CH1m).
7o `SET_LIMIT_MAX_G_SPEED` and 7p `TIME_STOP_IMMUNE_STATE` are script data that June's script brings in final CH1, and their readers are done. The 7p DamageReact hit-stop
(`SetStopTime( 0.5 )` x2, `SetStop2Time( 0 )`) is CH1k's. 7x: the 53 `AT_COMMAND` tags come with the script.

**June-only keys.** `SIEGE_ANGLE` / `MOBILE_SIEGE_ANGLE`: read by CH1k's InitComponent; readers `32befec0` (vslot 0x774, CH1o) and the spine-aim members.
`COMMAND_HIT_ADD_MP_BONUS` (`+0x98c4`): `dispsweep 98c4` over June's `.text` finds, in the class range, only the ctor, InitComponent, the dtor and
`InitPassiveSkillState` `32c16be0` `@32c16c2c`. That is the one consumer: it multiplies the passive multiples of BBT 85 / 86 (range / melee command MP gain) before
`SetPassiveMultiple`. It is part of CH4a's whole body (kickoff amended; the member comment now names it). `IMMUNE_TO_SUPER_ARMOR_DOWN`: Stage 2 (CH0).

**Checks.** Probes (`census.cl_cmd`, `/Zs /EHsc`): `X2GUChung.cpp` and `X2GUUser.cpp`, `_LIVE` off and on, exit 0, 0 errors, warnings equal (38 / 39).
`flagcheck JUNE_CHAR_CHUNG_LIVE --worktree --files X2GUChung.cpp X2GUUser.cpp`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched,
0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only for `define: NOT FOUND` (`//#define`). `_CORE --files X2GUChung.h`: `1 PASS`, `1 deleted line(s)` (the
member comment, replaced inside `_CORE`), `RESULT: PASS`. `git diff --patience --numstat`: 30/0, 1/1, 5/0. `junediverge.py --check X2GUChung.cpp X2GUChung.h X2GUUser.cpp`:
`RESULT: PASS`. `enumscan.py --check X2GUChung.cpp X2GUUser.cpp`: `0 A/B row(s) remain`.
`DEPLOY OK: X2_offline.exe is 25,902,080 B, dated 2026-11-02 08:32:55, md5 3f412a248afcbc78dfc0f68f2621ea71, equal to X2/US_SERVICE/x2.exe (2026-11-02 08:32:48); built at main 2afae8d1 + 3 uncommitted code file(s).` No `exestr` line: `_LIVE` is off and the phase adds no string to the flag-off exe.
Funcmap: 3 rows `Char Chung CH1p` (`32bfeb80`, `32bdaf00`, `32bdb130`). No `JUNE_BUGS` row.

**Nothing to play-test** (`_LIVE` is off). Next: `/x2-phase char-chung CH1p review` (Opus 5.5 high).

### CH1p - Review (2026-11-02)

**Verdict: passes, no source change** (no rebuild; the deployed `3f412a24` stands). Listings: `charport_adis.py chung 32bfeb80 31fc89a0 32bdaf00 32be7a30 32bdb130 32bee210 32bdb1b0 32c16be0`.
- **No reader left behind.** `_LIVE` preprocess of `X2GUChung.cpp`: the only survivors are June's 0-9 space (`SpecialAttackEventProcess` 32bd8790 `m_iNowSpecialAttack = slot + 1`,
  `SpecialAttackLookup` -> `CommonSpecialAttackEventProcess`, `SpecialAttackUse` / `Cooldown`: `StateChangeBySkillSlot`, `MarkSkillSlotUsed`, `m_eSpecialAttackKeyPressed = slot + 1`)
  and the six converted sites. No `m_sCurrentSkillKeyManager`, `SAKP_`, `CountUsedSkill` or `GetPressedSkillState` survives; the one `- 4` is Artillery Strike's random scatter
  (`% 8 - 4`). `X2GUUser.cpp` under `_LIVE`: `CountUsedSkill` / `GetPressedSkillState` (4 + 4) are definitions with no caller in either file; the `+/- 4` is inside
  `CurrentSkillKeyManager`, which Chung no longer uses; the rest are resets to 0. Elsewhere only `X2TrainingGameUI.cpp:303` (`GetNowSpecialAttack() > 0`) and the Lua getter.
- **Shared reader:** `UT_CHUNG` joins the June set only under `_LIVE`; the `#else` is CH1m's line unchanged.
- **SkillSlotKey:** `32bdaf00` = CommonStateStartFuture, `Set( [+0x88bc] - 1 )` on `+0x8b58`; `32bdb130` the same plus `+0x411c = 0` (March's `m_bApplyMotionOffset = false`);
  `32be7a30` `FollowSlotSwap` @32be7a3a, `IsKeyReleased` @32be7a70; `32bee210` @32bee21a / @32bee32b. LOOP's StartFuture `32bdb1b0` is CommonStateStartFuture plus `+0x411c = 0`,
  no `Set`, = March's. All six sites match.
- **Artillery Strike control `32bfeb80`:** `[+0x8ac0] - 1`, `cmp 9; ja` -> not-held path `32bfebb5`; table `32bff328` decodes to A S D C (`+0x86f6` / `fa` / `fe` / `f2`), F `+0x872a`,
  Q W E R (`+0x8706` .. `+0x8712`), T `+0x872e`, each `cmp pure, 0; jne` held. That is `IsKeyReleased` `31fc89a0`'s order (`+0x1c` `+0x20` `+0x24` `+0x18` `+0x50` / `+0x2c` .. `+0x38` `+0x54`).
  `+0x872a` / `+0x872e` = base `+0x50` / `+0x54` = `pureSkill5` / `pureSkillEx5` (June's `+0x4c` is Sit; Eve CH1j20 mapping, checked against `31fcadd0`'s `+0x872b` = oneSkill5).
  March's struct has them at 76 / 80, which is why the port goes by name. Out of range: our `IsKeyReleased` returns true (not held) = June's `ja`. Held path @32bfede8 starts
  `HaveThisBuff( 0x1a2 )`: routed to CH2c (kickoff amended in CH1p).
- **`COMMAND_HIT_ADD_MP_BONUS`:** `32c16be0` @32c16c2c decodes the protected float `+0x98c4` (`[+0x98c8] ^ &, [+0x98cc] ^`) and multiplies the passive multiples at `+0x68d4` (`+0x68ec`) and
  `+0x6988` (`+0x69a0`) before `SetPassiveMultiple` `3236ccc0`. `dispsweep 98c4`: in the class range ctor `32bf1040`, InitComponent `32bf4900`, dtor `32c0c4e0`, and this function;
  outside it Add (`329faa80`, `32a02960`) and Rose (`32ac3f70`, `32ac8a60`, `32ad1080`) own members, an EH-unwind funclet `3333acd0`, and misaligned `add [ecx+0x98c4]` noise.
- **Added to CH3a's kickoff:** `32be7a30`'s release test compares the anim time with `+0x8ef4` = `GetUsefulValueFuture( 0, 0 )` (C9h-4f), where March has `0.7f`.

**Hygiene** (range `e34b3fb4^..e34b3fb4`): `flagcheck JUNE_CHAR_CHUNG_LIVE`: `2 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (X2GUChung.h = _CORE), 0 FAIL, 0 ERROR`, `0 deleted line(s), 0 non-ASCII,
0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`, RESULT FAIL only for `define: NOT FOUND` (`//#define`). `_CORE`: `1 PASS, 2 OTHER, 0 FAIL`, 1 deleted
line (the member comment), `RESULT: PASS`. `git diff --patience --numstat`: 30/0, 1/1, 5/0. `junediverge.py --check-all`: `allow-listed: 163`, `RESULT: PASS`. `enumscan.py --check
X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`: `0 A/B row(s) remain`. Probes (`census.cl_cmd`, `/c /Zs /EHsc`): both files, `_LIVE` off and on, exit 0, 0 errors, warnings equal (38 / 39).
No play-test. Next: `/x2-phase char-chung CH1q` (Opus 5.5 high).

### CH1q - Done (2026-11-02)

All 15 class-map rows are written, plus the cannon-ball sites outside the class. Class bodies sit inside March's functions under
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (March in `#else`), and one new function sits under `_CORE`. The outside sites use the new
`SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL`, a `//#define` that CH1 defines together with `_LIVE` (`_LIVE` `#error`s without it). Chung stays on his March kit
until then. `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL_DEBUG` is defined; its `[JUNE_CHAR] CHUNG CBALL` lines (start / gain / spend / gauge refill / rebirth)
sit in the `_LIVE` bodies (the refill line in the `_CORE` function, uncalled until `_LIVE`). Funcmap rows `(CH1q)`, 12; divergence rows, 6 (`replace`, ported); checklist §7 l.236 marked.

| June | `CX2GUChung::` | June differs from March |
|---|---|---|
| `32bf3d30` | UpCannonBallCountAtThisAnimationTime | = March (UpCannonBallCount inlined); CH0 had paired it with UpCannonBallCount |
| `32bf5280` | UseCannon | a roll `GetCharacterSpecializationB() > GetRandomFloat( 0 )` (`+0x6d04`, Eve's DoParrying read) keeps the balls |
| `32bf3e40` | ConsumeCannonBallCount | an empty count with hyper time left arms `m_fCannonBallGaugeTimer` (`+0x98b8`) = 0.3 |
| `32bf4680` (new, `_CORE`) | CannonBallGaugeTimerFrameMove | ticks the timer; at -10..0, in the awakening, buff hyper count != 3, count 0 and no skill state, sets 1 ball, timer -100 |
| inline (`32bed560`, `32c24d40`) | GetExtraCannonBallCount | 0 unless class 117 is in the mobile armored mode |
| `32c24d00` | SetExtraCannonBallCount (header) | the same class 117 / mode guard |
| `32bf3f60` | SetTotalCannonBallCount | = March's `FIX_CANNON_BALL_SYNC_CALCULATION` arm (still % 7) |
| `32bf4040` | ResetCannonBallCountWhenRebirthInPVPGame | refills to the PvP start count (`+0x9680`), not the basic (dungeon) count |
| `32bf5430` | LoadPersonalInfo | keys `CANNON_BALL_COUNT_AT_DUNGEON_START` -> basic, `_AT_PVP_START` -> PvP start (March: one key); no `SetReduceGuardDamageAtHeavyStanceFromGuardSkill` call |
| `32bdcf20` `32bdcf70` `32bf4150` `32bf6f20` | ChargeCannonBall Start / StartFuture / FrameMove / EventProcess | = March, written under `_LIVE` |

`32c24db0` = SetCannonBallCount and `32c24c80` = SetUsedCannonballCountAtThisState are March's header one-liners (no edit). `32c24d40` is
`SetExtraCannonBallCount( GetExtraCannonBallCount() - n )` (CH0 said it duplicated `32c24d00`); its callers are CH2a / CH3c (kickoffs amended).
`ChangeMaxCharacterAddResource` `32bf4880` still matches the buff-port body. `FIX_CANNON_BALL_SYNC_CALCULATION`, `GAGEDATA_NULLCHECK`,
`FIX_CHUNG_GAGE_UI_UPDATE_BUG2`, `UPGRADE_SKILL_SYSTEM_2013`, `GUARD_BUG_TEST` and the Chung class flags are all ON (`flagprobe.py`).

**Outside the class (`_CBALL`).**
- `CX2ChungGageData` (vtable `3349b064`): the play status packs the extra balls x 13 (`32a32390` / `32a347e0`; March 7) and FromPacket sets the extra
  max before the extra count. `UpdateDataFromGameUnit` `32a34720` copies no berserk-changed flag (no March reader). The setters `3231d7a0` / `3231d880` /
  `32a36ba0` / `32a36c20`, `InitWhenGameIsOver` and `CopyGageData` = March. The emulator stores the packed int as-is: a Tactical Trooper save with
  extra balls may decode once with the new factor after CH1.
- `CX2ChungMyGageUI::InitUI` `32e6d6c0` (vtable `335333fc`): InitWakeOrb (vslot 0x54 = `32e5b3a0`, = March) runs before the
  `DLG_Chung_Cannonball_NEW.lua` open (March already opens that dialog). InitCannonBallUI `32e63470` and UpdateCannonBallCountUI `32e63990` = March.
- DamageEffect `USE_CANNONBALL` (`31f7aab0 @31f7cae4`, the only `+0xae8` reader): the owner must be unit type `UT_CHUNG`, there is no
  `fUseCannonBallDamageRel` factor (1.0 here anyway), and `m_fDecreaseForceDown` = `GetForceDownRelDamageRateBase()` with no x rate (March:
  rate x base). `+0x9860` / `+0x9864` are March's `m_fCannonBallDamageUp` / `m_fForceDownRelDamageRateBase` (writer `32c16be0`, skill 26410).

**Dropped (June removed it).** `SetReduceGuardDamageAtHeavyStanceFromGuardSkill` (`32bef230`) has no caller in June. Its March call in
`InitPassiveSkillState` goes in CH4a (kickoff amended).

**Routed.** `EXTRA_CANNON_BALL_DAMAGE_RATE` readers `32c0a230` / `32bfa860` -> CH2a. The skill-slot fade on cannon balls (`X2Game.cpp` ~15011), the F7
refill, the StateMenu site and June's `AddChungCannonBall_Cheat` binders -> CH5. CH1 defines `_CBALL` with `_LIVE` (kickoff amended).
No JUNE_BUGS row: nothing here reads as a June mistake. The % 7 / x 13 split is June's (class sync vs play status).

**For the review.** The debug flag stays until CH1's review, because its lines can only appear once `_LIVE` is on. Confirm the mode-entry callers of
`SetExtraCannonBallCount( GetMaxCannonBallCountEx() )` set `m_bMobileArmoredModeEnable` first under the new guard.

**Gates.**
- flagcheck `--worktree --files` (the six code files): `_LIVE` `files: 1 PASS, 1 PASS~, 0 DEFINE, 4 OTHER, 0 FAIL`; `_CBALL` `4 PASS, 1 PASS~, 1 OTHER,
  0 FAIL`. Both report RESULT FAIL only for the commented-out `#define`. `_CORE` and `_CBALL_DEBUG` (with Always.h): RESULT PASS. All four: `0 deleted
  line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  `git diff --numstat`: +225 / +9 / +9 / +4 / +11 / +25 / +34 (Always.h), 0 deletions.
- `junediverge.py --check` (six files): `allow-listed: 8`, `RESULT: PASS`; `--check-all`: `allow-listed: 163`, `RESULT: PASS`.
  `enumscan.py --check`: 1 row, `X2DamageEffect.cpp:1705` `SI_SA_ES_FATAL_FURY` (old code, not touched).
- One-file probes (`cl /Zs /EHsc`, no PCH), `X2GUChung.cpp` / `X2GageData.cpp` / `X2GageUI.cpp` / `X2DamageEffect.cpp`, with `_LIVE` + `_CBALL` off and on:
  exit 0, 0 errors (8 runs). `_LIVE` without `_CBALL` stops at the new `#error` (Always.h:11230), which proves the probes read Always.h.
- `DEPLOY OK: X2_offline.exe is 25,902,592 B, dated 2026-11-02 09:23:18, md5 d4f966c927f9f4c1ce40f3df7fb67092, equal to X2/US_SERVICE/x2.exe (2026-11-02 09:23:11); built at main fd01385e + 7 uncommitted code file(s).`
  `EXESTR OK: 4 of 4 absent` (the spend / gain / start / rebirth lines, `_LIVE` bodies). The gauge refill line is in the exe (`EXESTR OK: 1 of 1 found`):
  it sits in the `_CORE` `CannonBallGaugeTimerFrameMove`, which nothing calls until CH1's `_LIVE` CommonFrameMove.

**Nothing to play-test** (`_LIVE` and `_CBALL` are off). Next: `/x2-phase char-chung CH1q review` (Opus 5.5 high), then CH1.

## Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL`.
Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 6 has it -> CH2a; else the first of 20 / 21 / 27.

| Sub-phase | Scope |
|---|---|
| CH2a | Iron Cannon (6) skills |
| CH2b | Fury Guardian (20) skills |
| CH2c | Shooting Guardian (21) skills (siege mode: `CSI_SI_A_SIEGE_SHELLING_*`, `SIEGE_ANGLE` if CH0 placed it here) |
| CH2d | Shelling Guardian (27) skills |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI
arguments, 7b blocks if CH0 puts them here, the numerical-only tables of `plan_june_side.md` §3); its effect closure (a name found only now is
staged and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check
the templet swap for every term that reads the id (the Aisha CH3b review defect). `NUM use` lines for Chung skills; March vs June damage in
`MODS.md`.

**Done gate:** every class-6 / 20 / 21 / 27 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot
casts June's state, motion and effect and deals damage; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence

**Run with:** as CH2. Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3b0 | **Opus.** Chung's summoned NPCs (CH0 item 7): how June spawns and drives the targeting NPC (`NUI_TARGETING_CHUNG`, `_NOT_SHOW_OTHER_PLAYER`) and `NUI_REMOTE_MORTAR`; `Targeting_Chung.lua` adopted (or confirmed June's) and `luabind.py check` PASS; the summon buff / death path if June has one. CH0 moves it before the first skill phase that spawns one |
| CH3a | Iron Paladin (110) skills |
| CH3b | Deadly Chaser (111) skills; Artillery Strike / Missile Shower / Quantum Ballista with 7r row 1 (`32bff3d0`, the three cinematic writes move into the CONTROL_IDLE StateStartFuture; JUNE_BUGS 164) and 7ac (`@32bfe8c3`) |
| CH3c | Tactical Trooper (117) skills; June-only Heavy Field (`CSI_SA_CTT_HEAVY_FIELD_LOOP` `32c0ba60`, 7r row 2, the 7n sync float) |
| CH3d | Hyper actives (type 13, 6 rows), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows** (template, Stage 6 close-out): Chung's classes on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`), his `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Chung.lua`; checklist 7ao whole (hyper effect start `32c063d0`, the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); his hyper-class skills (`SPRT_HYPER_CLASS`) and `TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_CIP/CDC/CTT_Transcendence` names; `list_TR_*` rows in `GetCharacterImageName` (T6c) and his transcended pictures, read from June's row pushes (Eve's 160by160 pair was in `DLG_UI_Common_Texture99`); `POPUP_TR_*` present |

**Done gate:** as CH2 for 110 / 111 / 117 and the hyper actives; a transcended Chung (`settranscend`) casts his hyper-class skills, slots A-5 /
B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Chung's skill-id range>` = PASS after the transcendence
rows (the shared `DLG_SKILL_Passive_*` sheets included).

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, Opus review, user play-test (each a different engine piece: planned as three from the start).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` `32c16be0` and Chung's passive ids (types 1 / 8: 3 + 27 rows), their objects and readers; 7am delegate-list registrations (`32c16be0`, A `32c12910`, A `32bdba10`); flag `_PASSIVE` |
| CH4b | `SetDamageExceptionProcess` `32bf28f0` whole (June-only, 7b; `RNSI_P_CIP_IRON_WILL` 26402), the other overrides CH0 finds (`GetActualMPConsume` / `GetActualCoolTime` style); 7an soul gain when hit (vslot 0x748 `32beeca0`) and the specialization-B reader `32bf528d`; the death-prevention pre-apply override `32bed760` (§7 l.255); the per-skill stores of §7 l.280 / l.287 (`3201ad10` socket rate, `31fd7cc0` buff rate) |
| CH4c | Memo and item readers: `MemoConditionChung` (JUNE_BUGS 332, `EFFECT_SET_LIST_MEMO`), 7u's grenade memo (`X2BuffTemplet.cpp:1695-1706`, `HAND_GRENADE_0`) and `CHUNG_BIGBANGSTREAM_MEMO` / `_EXPLOSION_MEMO`, Metabolism Boost in `CX2Game::UseItemSpecialAbility` (§7 l.278, unit classes `0x14` / `0x6e`, skill `0x6720`) |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); 0 Chung `no_state` rows left except passives with no
state in June.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_CHUNG_UI`. Sub-tag `UI`.

**Scope:** Chung's 7 `UnitTemplet.lua` rows become June's (BOM source, `luasrc.py refresh` after; every other row stays): `m_fLobbyRotationY`,
`m_BerserkCutIn` / `m_HyperCutIn` for 110 / 111 / 117, the Tactical Trooper name `STR_ID_17435`; the cut-in table `X2GUChung.cpp:63-120` and the
InitState index switch `:781-838` replaced by June's names and layout (`g4`: June has no `.gfx` for `HQ_CutIn_Chung_*`, CH0 names June's
candidates); the dialog / gauge pieces CH1q does not take. **Done gate:** a select-screen pass of all nine March characters with no new error;
Chung's skill and hyper cut-ins play June's art.

## Phase CH-N - not run for Chung

All seven June Chung classes up to transcendence exist in March. The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N"; Ara runs
it first.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Chung per class (CH0b created 6 / 20 / 21 / 27), one transcended
per second job. Walk: every skill in an A and a B slot, a hyper, guard, cannon balls filling and spending, siege mode, the summoned NPCs, one
dungeon per job, then **leave the game and return to the village** (Eve's slot-glow lesson). Re-run `census.py`, `charport_scan.py`,
`charport_keys.py Chung_Iron_Cannon` and `s3_no_state.py`; before / after numbers in the Done note. **The becoming-transcended transition on
un-transcended units (user rule, 2026-10-28):** one unit per second-job class by `/tc 1`, quest 42030 (`/qdone 42030`) and the book 288493
(`/additem 288493 1`), each showing the guide popup with that class's picture, the new skill-tree rows, the HUD portrait, and surviving a relog.
A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Chung" section (flags, files, data, revert order: exe + data036 + motion koms
together); no `_DEBUG` flag left defined; checklist rows marked `(Chung: CHn)`; the Aisha plan's "Template for Stage 7" gets a Chung row (what
he took, what the no-`June`-prefix rule changed in CH1); backlog Stage 7 note; the `stage7-plan` row points at the next character
(`char-elsword`).

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste /
what it does / look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR CHUNG <sub>"`.

## Open decisions (for the user)

None at planning (2026-10-31). Two items that look like decisions are settled by standing rules: the 49 unnamed upgrade ids stay raw ids with a
comment (no-enum-literal rule: no invented names), and March's cannon-ball code is the base June's bodies go into (it is live, not dormant, so
the Eve decision does not arise). CH0 may raise one if the StatTable duplicates for 110 / 111 differ or the motion question finds a lost
animation June's files no longer carry.
