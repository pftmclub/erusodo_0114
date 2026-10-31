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
