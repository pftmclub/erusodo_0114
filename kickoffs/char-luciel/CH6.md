# June character Lu/Ciel (Stage 8) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-luciel CH6` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH3d, CH4c, CH5, TU reviews |
| Flag | none (fixes go back to their phase's flag) |
| Log tag | `[JUNE_CHAR]` (`LUCIEL *`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | none (user walk) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH6 - Whole-character walk and gap reruns"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do "Phase CH6". Back up the save first (`tools/elsdb.py`). The hand-off is the walk list, copy-ready.

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

## Amended by CH0 (2026-11-02)

- S3 baseline `june2017_re/char_luciel/ch0_s3_baseline.txt`: 301 tree rows, 301 no_state (March has no class), 137 distinct ids; rerun with `s3_no_state.py` (Lu/Ciel families added by CH0).
