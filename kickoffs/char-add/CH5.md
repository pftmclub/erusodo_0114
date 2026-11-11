# June character Add (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, tree-UI pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-add CH5` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside a CH2 phase (Touches do not overlap unless CH0 says so) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_UI` (+ `..._UI_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD UI`) |
| Review | `kickoffs/char-add/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, tree-UI pieces"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CH5 - Done"   # once written; else Ara's
grep -n "877\|878\|879\|896" june2017_re/char_add/plan_checklist_rows.md   # the tree-UI rows
```

## Task

Do "Phase CH5": his 3 `UnitTemplet.lua` rows become June's and June's 33 / 36 / 123 / 126 rows are added (core-list BOM source, `luasrc.py refresh`
after; the 123 / 126 rows wait for CHNa if CH0 found the templet loader refuses them); `m_fLobbyRotationY`, `m_HyperCutIn` for 122 / 123 / 126, the
names; the cut-in table and InitState index switch replaced by June's names and layout (7t); 7ah; the tree-UI pieces (checklist l.877-879, l.896:
`328e0aa0`'s `+0x138` / `+0x139`, the install / G-Core marks `328e0290` for unit type 9, the decide-ACK refresh); his `UpdateSkillSlotUI` class pass;
the lobby particle hard-codes in `X2UnitViewerUI.*` retired where June's `AnimationNameTable` plays them (Ara's decision 2 precedent: under the unit
viewer's flag, March's code in `#else`). Not the creation screen (CH0c / CHNa). Full build; play-test: a select-screen pass of all nine March
characters with no new error, his skill and hyper cut-ins play June's art, the class names show June's strings, the install / G-Core marks show.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
