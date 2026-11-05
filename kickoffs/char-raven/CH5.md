# June character Raven (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH5` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI` |
| Log tag | `[JUNE_CHAR]` (`RAVEN UI`) |
| Review | `kickoffs/char-raven/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Planning evidence (2026-11-05)" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH5 - Done"
```
Then `june2017_re/char_raven/plan_june_side.md` §7 and the `CH5` rows of `ch0_checklist_rows.tsv` (7t, 7ah).

## Task

Do "Phase CH5": Raven's 7 `UnitTemplet.lua` rows become June's (core-list BOM source; `luasrc.py refresh` after; every other row stays as it is):
`m_fLobbyRotationY` (4: 10, 16: 30, 17: 10, 25: 1, 106: 30, 107: 20, 115: 40), `m_HyperCutIn` `HQ_Cutin_RBM/RRF/RVC_Transcendence` for 106 / 107 /
115, the name `STR_ID_17431` for 115 (CH0 confirmed the string exists). The cut-in table `X2GURaven.cpp:61-94` and the InitState index switch
`:669-693` become June's names and layout under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI` (CP949: `bytepatch.py`). 7ah and the gauge pieces CH1q did not
take. Pack data036, ask before the copy. Hand-off: the select screen with all nine March characters, then each Raven job's skill and hyper cut-in.

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

## Amended by CH0 (2026-11-05)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7t L594 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cut; §7t L623 skill cut-in names -> June .gfx: June HQ_CutIn_Raven_BM_01; March HQ_C; §7ao L940-945 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- `ch0_unittemplet_raven.tsv`: 5 rows differ only by `m_fLobbyRotationY`; 106 / 107 / 115 add `m_HyperCutIn` `HQ_Cutin_RBM/RRF/RVC_Transcendence`; 115's name `STR_ID_617` -> `STR_ID_17431` ("Veteran Commander", present in `general.ess` and `StringID_def`). The cut-in `.gfx` are already June's bytes.
