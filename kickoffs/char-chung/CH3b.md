# June character Chung (Stage 7) - Phase CH3b - Skills: Deadly Chaser (111)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3a review and CH3b0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
```
Then CH0's, CH1's and the CH2 Done notes, and the `CH3b` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 111 has that classes 6 / 21 do not; Artillery Strike / Missile Shower 6019 / Quantum Ballista 6011 with checklist 7r row 1 (`CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE` StateStartFuture `32bff3d0 @32bff412-32bff420`: the three `SetFocusCamera / SetFocusCameraForce / SetApplyCinematicUI( false )` writes move there from March's READY EventProcess, no `IsMyUnit` test; JUNE_BUGS 164) and 7ac (`@32bfe8c3`, the CanDown call shape). Same method as CH2 (its kickoff's Task): each id's case with June's state functions whole under `_SKILL`, attached rows,
effect closure, the upgrade-id templet swap, raw ids for unnamed rows, "this needs Opus" for an unsettled function.
- `NUM use` lines; March vs June damage in `MODS.md`. CP949: `bytepatch.py` only. Mark rows `(Chung: CH3b)`.
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

## Amended by CH0

(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)
