# June character Ara (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH5` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2 (its Touches do not overlap `X2GUAra.*` except the cut-in table: not beside a CH2 phase editing the same lines), decision 2 |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_UI` (+ `..._UI_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA UI`) |
| Review | `kickoffs/char-ara/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH5 - Done (2026-11-09)" "CH5 - Review (2026-11-09, Opus 5.5): PASS, no code change"
```

## Task

Do "Phase CH5": Ara's 5 `UnitTemplet.lua` rows (7, 28, 118, 31, 119) become June's (`june_src_fixed/data036_kom/UnitTemplet.src.lua` L1158 / L1184 /
L1210 / L1342 / L1368) and June's rows for 34 (L1397) and 124 (L1423) are added (core-list BOM source; `luasrc.py refresh` after; every other row stays;
if CH0 found the templet loader refuses a class >= `UC_END`, the two new rows wait for CHNa); the cut-in table `X2GUAra.cpp:36-60` and the InitState
index switch `:280-310` replaced by June's names and layout (7t cut-in `.gfx` names, `HQ_Cutin_ASD / AYR / ASR_Transcendence`); 7ah; the lobby orb
handles `m_hSeqEnergyBullet1-3` (`X2UnitViewerUI.h:337-339`, `.cpp:80-82`, `:2465-2501`, checklist 7e l.368) retired or kept per Open decision 2. Pack
data036 and copy it. Done gate: a select-screen pass of all nine March characters with no new error; Ara's skill and hyper cut-ins play June's art;
the second-job names show June's strings; the lobby shows Little Devil's orb once (`ALD_LobbyWait`).

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
