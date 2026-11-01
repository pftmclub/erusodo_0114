# June character Chung (Stage 7) - Phase CH3a2 - Skills: Iron Paladin (110) CFG / CHG-named skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3a2` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3a review; one at a time with the other `X2GUChung.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH3a2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
```
Then CH0's, CH1's and the CH2 Done notes, and the `CH3a` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 110 has that classes 6 / 20 do not. Same method as CH2 (its kickoff's Task): each id's case with June's state functions whole under `_SKILL`, attached rows,
effect closure, the upgrade-id templet swap, raw ids for unnamed rows, "this needs Opus" for an unsettled function.
- `NUM use` lines; March vs June damage in `MODS.md`. CP949: `bytepatch.py` only. Mark rows `(Chung: CH3a)`.
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

- **Functions: 34** (`awk -F'\t' '$7=="CH3a2"' june2017_re/char_chung/ch0_class_map.tsv`; inside 27, new 7).
  June addresses: `32bd9760` `32bd97c0`(new CSI_SI_SA_CFG_IRON_HOWLING_StateStartFuture) `32bd97f0`(new CSI_SI_SA_CFG_IRON_HOWLING_StateStart) `32bd9810`(new CSI_SI_SA_CFG_IRON_HOWLING_END_StateEnd) `32bd99d0` `32bd99e0` `32bd99f0` `32bd9a00` `32bd9ed0` `32be11a0` `32be1260`(new CSI_SI_SA_CFG_IRON_HOWLING_LOOP_StateStart) `32be12c0`(new CSI_SI_SA_CFG_IRON_HOWLING_LOOP_EventProcess) `32be1390`(new CSI_SI_SA_CFG_IRON_HOWLING_LOOP_StateEnd) `32be13b0`(new CSI_SI_SA_CFG_IRON_HOWLING_END_EventProcess) `32be1ae0` `32be1ba0` `32be1e90` `32be1f70` `32be1fa0` `32be1ff0` `32be2060` `32be2270` `32be2f60` `32be6400` `32bf0af0` `32bf0b80` `32bf2f70` `32bf41c0` `32bf4200` `32bf4240` `32bf4280` `32bfbc10` `32bfbe30` `32c156c0`.
- **States: 13** (`awk -F'\t' '$8=="CH3a2"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_SI_SA_CFG_IRON_HOWLING`, `CSI_SI_SA_CFG_IRON_HOWLING_LOOP`, `CSI_SI_SA_CFG_IRON_HOWLING_END`, `CSI_SI_SA_CFG_PAIN_OF_CALADBOLG`, `CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_HYPER`, `CSI_SA_CHG_WONDER_WALL`, `CSI_SA_CHG_WONDER_WALL_HYPER`, `CSI_SI_A_CFG_GUARD_FIRST_STRIKE`, `CSI_SI_A_CFG_GUARD_READY`, `CSI_SI_A_CFG_GUARD_READY_MEMO`, `CSI_SI_A_CFG_GUARD_WAIT`, `CSI_SI_A_CFG_GUARD_END`, `CSI_SI_A_CFG_GUARD_DAMAGE`.
- Iron Howling (`CSI_SI_SA_CFG_IRON_HOWLING*`): June's `_HYPER` state plays `SI_SA_CFG_IRON_HOWLING`, which no June motion file has (CH0 motion question): JUNE_BUGS row. Its `_END` StateEnd / EventProcess (`32bd9810`, `32be13b0`) are `new`.
- Wonder Wall (`CSI_SA_CHG_WONDER_WALL*`) and the CFG guard states are class 110 skills in June's tree.

## Amended by CH1l (2026-11-01)

- CH1l already added the `CHUNG_STATE_ID` members `CSI_SI_A_CFG_GUARD_READY` (June id 298) and `CSI_SI_SA_CFG_IRON_HOWLING_LOOP` / `_END` (321 / 322): do not declare them again, only their callbacks. `RNSI_A_CFG_GUARD`, `RNSI_SA_CIP_IRON_HOWLING`, `RNSI_A_CFG_LEAP_ATTACK`, `RNSI_SA_CFG_LUNATIC_FURY` and `RNSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE` are registered in `X2SkillTree.h`. The Guard object has `m_fMpConsumption` (CH1l, June `+0x14`); its reader is the guard state.
