# June character Chung (Stage 7) - Phase CH3c - Skills: Tactical Trooper (117)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3c` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3b review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH3c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
```
Then CH0's, CH1's and the CH2 Done notes, and the `CH3c` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 117 has that classes 6 / 27 do not; June-only Heavy Field (`CSI_SA_CTT_HEAVY_FIELD_LOOP` `0x151`, `32c0ba60`: checklist 7r row 2 and the 7n sync float, thunk `32c256f0`). Same method as CH2 (its kickoff's Task): each id's case with June's state functions whole under `_SKILL`, attached rows,
effect closure, the upgrade-id templet swap, raw ids for unnamed rows, "this needs Opus" for an unsettled function.
- `NUM use` lines; March vs June damage in `MODS.md`. CP949: `bytepatch.py` only. Mark rows `(Chung: CH3c)`.
- Hand-off: the unit (18 Freils 117, 42 RapidT / 50 Chung99 111, 43 NovaT 110, or a CH0b unit), each skill id and its A / B slot (not 4 / 9 on an
  un-transcended unit), what June's motion and effect look like.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-10-31)

- **Functions: 27** (`awk -F'\t' '$7=="CH3c"' june2017_re/char_chung/ch0_class_map.tsv`; inside 17, new 10).
  June addresses: `32bda6d0` `32bdad40` `32bdad50` `32bdad60` `32bdad70` `32bdad80` `32bdad90` `32bdbbd0`(new CSI_SA_CTT_HEAVY_FIELD_READY_Init) `32bdbca0`(new CSI_SA_CTT_HEAVY_FIELD_READY_StateStartFuture) `32be6200` `32be6920` `32be73b0` `32be75b0` `32bf3830` `32bf48e0`(new CSI_A_CTT_FATAL_CANNON_StateStart) `32c07040` `32c0a500`(new GetUsefulTacticValue) `32c0b7f0`(new CSI_SA_CTT_HEAVY_FIELD_READY_FrameMove) `32c0b9b0`(new CSI_SA_CTT_HEAVY_FIELD_READY_EventProcess) `32c0ba60`(new CSI_SA_CTT_HEAVY_FIELD_LOOP_FrameMove) `32c0bd20`(new CSI_SA_CTT_HEAVY_FIELD_LOOP_EventProcess) `32c0c1c0`(new CSI_SA_CTT_HEAVY_FIELD_END_StateStart) `32c0f6d0` `32c10d20`(new ?) `32c12470` `32c1c840` `32c1cc70`.
- **States: 17** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_A_CHG_MARK_OF_COMMANDER`, `CSI_A_CHG_MARK_OF_COMMANDER_HYPER`, `CSI_SA_CHG_CARPET_BOMBING`, `CSI_SA_CHG_CARPET_BOMBING_HYPER`, `CSI_SA_CTT_TACTICAL_FIELD`, `CSI_SA_CTT_TACTICAL_FIELD_HYPER`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_HYPER`, `CSI_A_CTT_REMOTE_MORTAR`, `CSI_SI_SA_CTT_CANNON_STRIKE`, `CSI_SI_SA_CTT_CANNON_STRIKE_HYPER`, `CSI_A_CTT_FATAL_CANNON`, `CSI_SA_CTT_CANNON_PRISON`, `CSI_SA_CTT_BOMBING_ARTILLERRY`, `CSI_SA_CTT_HEAVY_FIELD_READY`, `CSI_SA_CTT_HEAVY_FIELD_LOOP`, `CSI_SA_CTT_HEAVY_FIELD_END`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 26023 RNSI_SA_CHG_CARPET_BOMBING, 26024 RNSI_SA_CTT_TACTICAL_FIELD_INDURANCE, 26025 RNSI_SA_CTT_ARTILLERY_STRIKE_MISSILE_SHOWER, 26026 RNSI_SA_CHG_CANNON_STRIKE, 26041 RNSI_SA_CTT_CANNON_PRISON, 26042 RNSI_SA_CTT_BOMBING_ARTILLERRY, 26058 RNSI_SA_CHG_CARPET_BOMBING_UPGRADE, 26059 RNSI_SA_CTT_TACTICAL_FIELD_INDURANCE_UPGRADE, 26060 RNSI_SA_CTT_ARTILLERY_STRIKE_MISSILE_SHOWER_UPGRADE, 26061 RNSI_SA_CHG_CANNON_STRIKE_UPGRADE, 26064 RNSI_SA_CTT_HEAVY_FIELD, 26214 RNSI_A_CHG_MARK_OF_COMMANDER, 26215 RNSI_A_CTT_AUTOMATIC_MORTAR, 26228 RNSI_A_CTT_FATAL_CANNON, 26237 RNSI_A_CHG_MARK_OF_COMMANDER_UPGRADE, 26238 RNSI_A_CTT_AUTOMATIC_MORTAR_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CTT_CANNON_STRIKE (script L; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CTT_CANNON_STRIKE_HYPER (sc; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7 L287 engine readers keyed by a March skill id where June pushes the reform ; §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7n L492 user-sync class-skill float: writer and reader 32c0ba60 (thunk 32c256f; §7r L571 June-only Heavy Field: CSI_SA_CTT_HEAVY_FIELD_LOOP (0x151) 32c0ba60 @3; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- 7r row 2 and 7n (`32c0ba60`, Heavy Field, June-only skill 26064; spawn by CH3b0). Remote Mortar `32c12470`. Mark of Commander 26214 (l.287 readers in CH4b). `CSI_SA_CTT_HEAVY_FIELD_END_StateStart` (`32c0c1c0`) is `new`.

## Amended by CH1l (2026-11-01)

- CH1l already added `CSI_SA_CTT_HEAVY_FIELD_LOOP` / `_END` (June ids 337 / 338) to `CHUNG_STATE_ID`; `CSI_SA_CTT_HEAVY_FIELD_READY` is still yours. `RNSI_SA_CTT_HEAVY_FIELD` and `RNSI_SA_CTT_DREAD_CHASE` are registered in `X2SkillTree.h`.

## Amended by CH1l4 (2026-11-01)

- CH1l4 declared `m_iCarpetBombingMaxCount` (June `+0x98a4`, ctor 0) and `m_fCarpetBombingExtraRoundPowerRate` (`+0x98a0`, ctor 1); the Carpet Bombing skill start (`32c0f854`-`32c0f88e`) writes them: rate 0, count 18, and +9 more shots (a second `32c0a230` call) with 6 or more cannon balls. `CommonFrameMove` ends the run at the count; `DoCarpetBombing` multiplies the power from shot 19 by `max( 1, rate )`. Without the start's writes the run never fires.

## Amended by CH1q (2026-11-02)

- Fatal Cannon Start `32bf48e0` = CommonStateStart, then `32c24d40( 1 )` = `SetExtraCannonBallCount( GetExtraCannonBallCount() - 1 )` (guarded since CH1q).
- **Amended by CH3b0 (2026-11-02):** Heavy Field READY (`32c0b7f0`) spawns 2525 (NPCTemplet row + `Enum.lua` member shipped by CH3b0 in data036 / data037) and the owner keeps it in `m_iHeavyFieldTargetingNPCUID` (`+0x98e8`, the LOOP `32c0ba60` reads it); Remote Mortar FrameMove `32c12470` calls `CreateRemoteMortar` (`32c10d20`, already June's: `m_vecRemoteMortarNPCUID`, cap `GetRemoteMortarMaxCount` `32c0a500`, `CHUNG SKILL remote mortar` lines); the owner link and stats are CH3b0's (`CX2Game::SetUserSummonedNPCInfo` cases under `_SKILL`, shared `CX2GUUser::SetSummonHelperInfo` = June `322ece70`). CH3b0 could not play-test (no state for the summon skill yet), so this phase's hand-off carries its check: each spawn logs `[JUNE_CHAR] SUMMON npc <id> uid <uid> skill <skill> lv <n> flags 0x<f> ...` (read with `python tools/logs.py --grep "JUNE_CHAR] SUMMON"`). Expect `npc 2525 ... skill 26064 ... flags 0x0` and `npc 1100 ... skill 26215 ... flags 0x44a`, and at the cap `remote mortar uid <u> erased ( kill 1 )`.
