# June character Chung (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-chung CH6` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3d, CH4c and CH5 reviews |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`CHUNG *`) |
| Review | none (the user's walk is the gate) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Planning evidence (2026-10-31)" "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH6 - Done (2026-10-28, Sonnet 5.5): scans PASSED, walk owed"
```
Then CH0's Done note (the S3 baseline).

## Task

Do "Phase CH6". Back up the save first. Re-run `census.py`, `charport_scan.py` (short scratch dir), `charport_keys.py Chung_Iron_Cannon`
and `s3_no_state.py --family X2GUChung.cpp --preprocess`; before / after table in the Done note, every remaining `no_state` id explained.
Then hand over the walk: one unit per class (CH0b's 6 / 20 / 21 / 27 units, 18 / 42 / 43 / 50), every skill in an A and a B slot, a hyper, guard,
cannon balls, siege mode, the summoned NPCs, one dungeon per job, then back to the village; transcended units for A-5 / B-5; and the
becoming-transcended transition on un-transcended units (plan "Phase CH6": `/tc 1`, `/qdone 42030`, `/additem 288493 1`, one per second job,
guide popup picture, skill-tree rows, HUD portrait, relog). Failures go back to the owning phase's flag.

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
