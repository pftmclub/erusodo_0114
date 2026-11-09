# June character Rena (Stage 7) - Phase CH3b - Skills: Grand Archer (103)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH3b` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3a review. One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3b - Done" "CH3b - Review"
```
Then CH0's and CH1's Done notes, and the `CH3b` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-103 skill ids earlier classes lack; `DamageEffect_LGA_Arc_Shot` (7u); Anger of Elf `:22713` if CH0 puts it here. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH3b)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence; learn only the bank under test), each
  cast twice (cool time, MP), what June's motion and effect look like.

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

## Amended by CH0 (2026-11-07)

- **Functions: 39** (`awk -F'\t' '$7=="CH3b"' june2017_re/char_rena/ch0_class_map.tsv`; inside 21, new 18).
  June addresses: `32c6c1f0` `32c6c3a0` `32c6c460` `32c6c4d0` `32c6c4e0` `32c6c530` `32c6c550` `32c6cba0` `32c6cfc0` `32c6d040`(new LESI_SA_RGA_BLAST_ARROW_Init) `32c6d050`(new LESI_SA_RGA_CUTLASS_WIND_Init) `32c6d090`(new LESI_A_RGA_ONE_TARGET_SHOT_Init) `32c6d890`(new LESI_SA_RGA_ACEINTHEHOLE_START_Init) `32c6d8a0`(new LESI_SA_RGA_ACEINTHEHOLE_START_StateStart) `32c6d8f0`(new LESI_SA_RGA_ACEINTHEHOLE_START_StateStartFuture) `32c793f0` `32c79430` `32c7d5a0` `32c7ddb0`(new LESI_SA_RGA_BLAST_ARROW_EventProcess) `32c7e0c0`(new LESI_SA_RGA_CUTLASS_WIND_EventProcess) `32c7ed00`(new LESI_A_RGA_ONE_TARGET_SHOT_EventProcess) `32c833b0`(new LESI_SA_RGA_ACEINTHEHOLE_START_EventProcess) `32c833e0`(new LESI_SA_RGA_ACEINTHEHOLE_1ST_ATTACK_EventProcess) `32c83410`(new LESI_SA_RGA_ACEINTHEHOLE_2ND_ATTACK_EventProcess) `32c83440`(new LESI_SA_RGA_ACEINTHEHOLE_3RD_ATTACK_EventProcess) `32c84b50` `32c84c70` `32c85430`(new LESI_SA_RGA_ACEINTHEHOLE_WAIT_EventProcess) `32c85500`(new LESI_SA_RGA_ACEINTHEHOLE_WAIT_EventProcess) `32c855d0`(new LESI_SA_RGA_ACEINTHEHOLE_WAIT_EventProcess) `32c87000` `32c87030` `32c87060` `32c885a0`(new LESI_SA_RGA_ACEINTHEHOLE_WAIT_FrameMoveFuture) `32c9d7b0` `32c9e7b0` `32ca2520`(new LESI_SA_RGA_ACEINTHEHOLE_WAIT_FrameMove) `32cacda0` `32cacff0`.
- **States: 16** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SA_LGA_FREEZING_ARROW`, `LESI_SA_LGA_FREEZING_ARROW_HYPER`, `LESI_A_LGA_STIGMA_ARROW`, `LESI_A_LGA_STIGMA_ARROW_HYPER`, `LESI_SA_LGA_AERO_STRAFE`, `LESI_SA_LGA_AERO_STRAFE_HYPER`, `LESI_A_LGA_WIND_BLAST`, `LESI_A_LTR_ENTANGLE`, `LESI_A_RGA_ONE_TARGET_SHOT`, `LESI_SA_RGA_BLAST_ARROW`, `LESI_SA_RGA_CUTLASS_WIND`, `LESI_SA_RGA_ACEINTHEHOLE_START`, `LESI_SA_RGA_ACEINTHEHOLE_WAIT`, `LESI_SA_RGA_ACEINTHEHOLE_1ST_ATTACK`, `LESI_SA_RGA_ACEINTHEHOLE_2ND_ATTACK`, `LESI_SA_RGA_ACEINTHEHOLE_3RD_ATTACK`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 23015 RNSI_SA_RGA_GUNGNIR, 23016 RNSI_SA_RGA_WIND_BLAST, 23017 RNSI_SA_RGA_AERO_STRAFE, 23018 RNSI_SA_RGA_FREEZING_ARROW, 23039 RNSI_SA_RGA_BLAST_ARROW, 23040 RNSI_SA_RGA_CUTLASS_WIND, 23049 RNSI_SA_RGA_GUNGNIR_UPGRADE, 23050 RNSI_SA_RGA_WIND_BLAST_UPGRADE, 23051 RNSI_SA_RGA_AERO_STRAFE_UPGRADE, 23052 RNSI_SA_RGA_FREEZING_ARROW_UPGRADE, 23059 RNSI_SA_RGA_ACEINTHEHOLE, 23211 RNSI_A_RGA_STIGMA_ARROW, 23213 RNSI_A_RGA_ENTANGLE, 23226 RNSI_A_RGA_ONE_TARGET_SHOT, 23235 RNSI_A_RGA_STIGMA_ARROW_UPGRADE, 23237 RNSI_A_RGA_ENTANGLE_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7n L493 "user-sync class-skill float: Rena 32c88050 (Grand Archer RANDOM_SHOT_; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- Entangle 23213 (`RNSI_A_RGA_ENTANGLE`, Grand Archer in June) builds `DAMAGE_EFFECT_LENA_TRAPPING_RANGER_ENTANGLE` / `_MEMO` (`32c9e7b0`): no NPC. 7u: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` -> June's `..._Guided_Arrow` spelling, `DamageEffect_LGA_Arc_Shot` dropped.

- **Amended by CH1j (2026-11-09): members the Ace in the Hole states use.** `m_fAceInTheHoleTime` ( June +0x980c, declared by CH1j ): `CommonFrameMoveFuture` adds the frame time every future frame; the START / WAIT / 1ST_ATTACK StartFuture `32c6d8f0` zeroes it, the WAIT EventProcess ( `32c85430` / `32c85500` / `32c855d0` ) compares it with an ability of skill 23059. `32c6d8a0` ( the Start ): after `CommonStateStart` it zeroes +0x9808 and, for my unit, sets `m_bSkillCameraOverride` ( +0x8d90 ) = true, `m_fSkillCameraOverrideValue` ( +0x8da4 ) = 550.0 and +0x8d94 = 220.0 ( stand-in members, the reader is Stage 9's camera; declare +0x8d94 as a third stand-in ).

## Amended by CH1n (2026-11-09)

- **Slot states:** Gungnir (23015) registers `LESI_RGA_GUNGNIR_READY` ( 283 ); Ace in the Hole (23059) registers `LESI_SA_RGA_ACEINTHEHOLE_WAIT` 296 / `_1ST_ATTACK` 297 / `_WAIT_AFTER_1ST_ATTACK` 298 / `_2ND_ATTACK` 299 / `_WAIT_AFTER_2ND_ATTACK` 300 / `_3RD_ATTACK` 301 (298 / 300 use the 296 table, June `@940` / `@956`), all CH1i's `_CORE` members, by table name. Rapid Shot's `32cb5760` is March's `RapidShotData::CreateRapidShotData()` (`@32cb0e51`). The id switch takes `GetUpgradeBaseSkillID( eSkillID_ )` (`CH2a.md`, Amended by CH1n).

## Amended by CH1p (2026-11-09)

- **Held-key tests (7ag):** the three Ace in the Hole wait EventProcesses `32c85430` / `32c85500` / `32c855d0` each call `IsEquippedSkillKeyPressed( m_InputData, GetFutureStateSkillID() )` (`31fce710` `@32c8545b` / `@32c8552b` / `@32c855fb`; `CX2GUUser`, under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`) first: pressed -> StateChange to the next attack ( 0x129 / 0x12b / 0x12d ); else left / right pressed, or `m_fAceInTheHoleTime` reaching `32007b10( 1, id )`, -> `UpNowMp` with text ( `32278070`, `32007b10( 0x42, id ) * 3.0` ) and `LESI_WAIT`; then `CommonEventProcess`. The class map names all three `LESI_SA_RGA_ACEINTHEHOLE_WAIT.EventProcess`: name each by the state its table registers. No `SAKP_*` / `m_kSkillSlotKey` here.
