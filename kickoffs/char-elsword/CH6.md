# June character Elsword (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-elsword CH6` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3d, CH4c and CH5 reviews |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD *`) |
| Review | none (the user's walk is the gate) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Planning evidence (2026-11-03)" "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH6 - Whole-character walk and gap reruns"
```
Then CH0's Done note (the S3 baseline) and Chung's CH6 Done note if it exists.

## Task

Do "Phase CH6". Back up the save first. Re-run `census.py`, `charport_scan.py` (short scratch dir), `charport_keys.py Elsword_SwordMan` and
`s3_no_state.py --family X2GUElsword_SwordMan.cpp --preprocess`; before / after table in the Done note (planning: `87 / 20`, 254 `no_state`),
every remaining `no_state` id explained. Then hand over the walk: one unit per class (CH0b's 1 / 10 units, 25 / 26 / 32 / 33 / 34), every skill
in an A and a B slot, a hyper, the Way of the Sword in vigor and destruction, Rune Slayer reflect, one dungeon per job, then back to the village;
transcended units for A-5 / B-5; and the becoming-transcended transition on un-transcended units (plan "Phase CH6": `/tc 1`, `/qdone 42030`,
`/additem 288493 1`, one per second job, guide popup picture, skill-tree rows, HUD portrait, relog). Failures go back to the owning phase's flag.

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

## Amended by CH5 (2026-11-04)

- **Select screen:** all nine March characters, then the seven Elsword classes (the lobby pose turns by class: 35 / 25 / 25 / 10 / 3 / 10 / 10). Lord Knight, Rune Slayer and Infinity Sword show their June names (`STR_ID_4641` / `4642` / `17425`).
- **Cut-in sets:** each Elsword class uses one skill cut-in in game (sets 0 / 1 / 2 / 3 = Sword Man family / Lord Knight / Rune Slayer / Infinity Sword); `[JUNE_CHAR] ELSWORD UI cut-in set <n> index <i> name <gfx>` with a `[JUNE_GFX] CUTIN play` in the same second.
- **Hyper cut-in:** `[JUNE_CHAR] ELSWORD UI hyper cut-in class <c> name HQ_Cutin_E{LK,RS,IS}_Transcendence` appears only when the awakening state sets `SHOW_HYPER_CUT_IN_TIME` (CH3d wires the three states), so the check is CH6's.
