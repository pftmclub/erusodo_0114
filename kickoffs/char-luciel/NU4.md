# June character Lu/Ciel (Stage 8) - Phase NU4 - Creation, starter items and the select screen

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel NU4` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU2 review, NU3 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CREATE`, `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_STARTER_ITEMS` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL CREATE`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/NU4-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase NU - New-character foundation (NU1-NU4)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the NU4 row of "Phase NU". Starter rows (user, 2026-11-02): 15 rows, gear 113300-113304 plus both promotion sets 41890-41894 and
41900-41904 (`june2017_re/char_luciel/plan_starter_items.tsv`), a labelled stand-in whose comment says the live shape is ten. The stat-row
fallback never borrows another class's rows; until the user answers CH0's stat question it is the synthetic curve, logged once per class.
`tools/createchar.py` learns the seven classes (`CLASSES`, `PARENT`, `PROMO`). **The creation screen's Lu/Ciel entry is part of the gate**
(user, 2026-11-02): the slot, portrait, name, description, preview model and motions, select voice and job-preview buttons, all as NU0 item 4
maps them from June. Hand-off: open the creation screen, check each of those, create a Lu/Ciel, and see it listed
with its items (`/x2-db`), **and do not enter the game** (the class arrives in CH1).

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

## Amended by NU0

(empty until NU0 runs)
