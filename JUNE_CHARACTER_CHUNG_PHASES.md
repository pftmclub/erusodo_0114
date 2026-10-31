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
