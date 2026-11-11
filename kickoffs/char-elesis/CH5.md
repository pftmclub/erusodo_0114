# June character Elesis (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, the creation screen

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elesis CH5` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2 (its Touches do not overlap `X2GUEL.*` except the cut-in table: not beside a CH2 phase editing the same lines) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_UI` (+ `..._UI_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS UI`) |
| Review | `kickoffs/char-elesis/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, the creation screen"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH5 - Done" "CH5 - Review"
python tools/docsection.py june2017_re/char_elesis/plan_march_side.md --list   # then its creation-screen section (10)
```

## Task

Do "Phase CH5": Elesis's 5 `UnitTemplet.lua` rows (8, 29, 30, 120, 121) become June's (`june_src_fixed/data036_kom/UnitTemplet.src.lua` L1239 /
L1264 / L1290 / L1316 / L1452 / L1480 / L1508 are her seven rows: CH0 says which line is which class) and June's rows for 35 and 125 are added
(core-list BOM source; `luasrc.py refresh` after; every other row stays; if CH0 found the templet loader refuses a class >= `UC_END`, the 125 row
waits for CHNa); the cut-in table and the InitState index switch (`X2GUEL.cpp:249-273`) replaced by June's names and layout (7t cut-in `.gfx` names,
`HQ_Cutin_EGM / EBH / ECA_Transcendence`; March's `HQ_CutIn_Elesis_Pyro_Knight.dds` / `_Saber_Knight.dds` exist nowhere); 7ah; the lobby particle
hard-codes (`X2UnitViewerUI.cpp:630` / `:2414` / `:2530`) retired where June's `AnimationNameTable` row plays the same particles (under the unit
viewer's flag, March's code in `#else`; Ara's decision 2 is the precedent).

**Creation screen (user, 2026-11-10 and 2026-11-11: part of this port).** Today Elesis shows only Saber Knight and Pyro Knight there. Adopt June's
`DLG_Unit_Create_ClassChange_Button_Elesis.lua` (data034, six buttons; `adopt-june`, `luabind.py check`); list from Ghidra every June function the
creation state and unit viewer run for Elesis (class-preview switch, class buttons, `Movie_Create_Elesis_*` previews, the stat chart
`X2StateCreateUnit.cpp:3870-3885`, the sites of `plan_march_side.md` section 10) and port them under `_UI`. The six `SCUUCM_*_CLASS_CHANGE_BUTTON_UP`
handlers are generic (`:778-808`, `SelectUpperClass` `:3397`), so Grand Master and Blazing Heart need the dialog and whatever June's functions add.
A click on a third-line button before CHNa must be harmless: check what `SelectUpperClass` does with the assert-only third line; if it is not
harmless, hide the two third-line buttons under `_UI` until CHNa (one line each, removed by CHNa).

Pack data036 / data034 and copy them. Done gate: a select-screen pass of all nine March characters with no new error; her skill and hyper
cut-ins play June's art; the second-job names show June's strings; **on the creation screen Elesis shows Saber Knight, Grand Master, Pyro Knight
and Blazing Heart, each with its preview and movie**.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).
