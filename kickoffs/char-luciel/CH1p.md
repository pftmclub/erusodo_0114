# June character Lu/Ciel (Stage 8) - Phase CH1p - Class core: The in-class duo switch

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1p` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1o review, DU1 review, DU2 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_DUO` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL CORE` / `DATA`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH1p-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1p row: `DUO_TYPE` handling (add `DT_BOTH` 2 / `DT_KEEP` 3 to the live core-list `Enum.lua` under the plan's Lua rules, pack,
ask before copy), the minor-unit anim / speed keys, `NOT_SHOW_UNIT_BY_DUOTYPE`, the body switch and its key dialog, and combination points
(`HIT_ADD_COMBINATION_POINT`, `CP_AMOUNT`) on DU1's members. Sub-tag `DUO` lines for a switch and a point gain (throttled).

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

- **Functions: 1 to transcribe** (+ 1 thunks) (`awk -F'\t' '$6=="CH1p"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b14bc0` AddCombinationPointOnHit.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §2 L119 HIT_ADD_COMBINATION_POINT (parsed, logged) x6; §3 L151 Combination point absent; §3 L153 Lu/Ciel duo absent (no DUO_TYPE enum); §4 L166 CUSTOM_RENDER_STATE (17473) Enum table missing; §4 L167 DUO_TYPE (17515) members DT_BOTH = 2, DT_KEEP = 3; §7 L276 Spec A / B readers 32b14860 (A and B) and 32b37910 (B); §7an L927 GetCharacterSpecializationB readers Lu/Ciel 32b14950 / 32b3794d.
- `AddCombinationPointOnHit` `32b14bc0` (the `HIT_ADD_COMBINATION_POINT` consumer, a delegate InitState pushes) is this phase's; the engine reader is `32632390` @3263dfd2.
- **`Enum.lua` also needs the `CUSTOM_RENDER_STATE` table** (checklist agent; `Luciel_Demonio` uses it in 3 states and a missing table stops the file there): add it with `DT_BOTH` / `DT_KEEP`, before CH1 adopts the scripts.
