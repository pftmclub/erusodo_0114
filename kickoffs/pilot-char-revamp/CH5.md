# June character Aisha (Stage 5 pilot) - Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH5` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_UNITTEMPLET` (if CH0 says the adoption needs a reader fix), `SERV_IRUHADEV_JUNE_CHAR_ARME_UI` |
| Log tag | `[JUNE_CHAR]` (`ARME UI`) |
| Review | `kickoffs/pilot-char-revamp/CH5-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH0's `UnitTemplet.lua` decision (Done note) and the `ch5` rows of its tsvs.

## Task

Do "Phase CH5".
- **`UnitTemplet.lua`.** Every class loads it, so the play-test covers a select-screen pass of all nine March characters, not only Aisha.
- **June files.** Adopt them with `adopt-june` and stage them with `kom.py stage-june`. Pack, then ask before each copy.
- **CP949 files.** Edit them with `bytepatch.py` only.

## Amended by CH0

Written by CH0 (2026-10-20).
- **`UnitTemplet.lua` (data036, March bytecode).** Reader: `CX2UnitManager::AddUnitTemplet_LUA` (`X2UnitManager.cpp:836-911`), bound at
  `X2Main.cpp:3721`. Every key June's rows use is read (`LUA_GET_VALUE*`) or ignored (`m_fLobbyRotationY`, `m_fMinorUnitLobbyRotationY`,
  `majorUnitName` / `minorUnitName`, the other `m_MinorUnit*`). The cut-in keys `m_HyperCutIn`, `m_BerserkCutIn`, `m_MinorUnitHyperCutIn`
  and the two `_Trans` keys are read since C3t (`:894-898`).
  - `UC_150` (third-job placeholder, out of scope): `UNIT_CLASS.UC_150` is nil in March's Enum.lua, so `LUA_GET_VALUE_RETURN_ENUM` skips the row
    (`:844`, logged to cerr). It is inert with no reader fix.
  - Classes March's C++ enum does not name but Enum.lua numbers (Lu/Ciel, Rose, Ain, ...) are accepted as raw ints. A second row for a class
    is refused (`:900`).
  - **Aisha's rows:** identical items, motions and movies; June adds `m_HyperCutIn` on 104 / 105 / 113 (`HQ_Cutin_A*_Transcendence`) and
    renames 104 / 105 / 113 (`STR_ID_615` -> `STR_ID_4792` / `4793` / `17427`).
  - **Other classes change too, if adopted whole (open question to the user, asked in CH0's hand-off):** 15 second-job rows get their own
    job-name string instead of the character's (`STR_ID_614` -> `4641`, ...; every id exists in `KTDXLIB/StringID_def*.h`), and 23 rows gain
    `_Transcendence` hyper / berserk cut-in names. Those stay invisible until each class's June script is adopted (the key is read only at
    `SHOW_HYPER_CUT_IN_TIME`), but the names show at once.
- **Skill cut-in table (7t):** replace `s_SkillCutInSet` / the index switch with June's names and layout (June class function `329cd890` +
  `32078b70`); the `.gfx` files come with CH0b. `g4_cutin_names.tsv` l.4-10 lists March's 7 names (none has a June `.gfx`).
- **Hyper cut-in:** `SHOW_HYPER_CUT_IN_TIME` (3 states) arrives with the script in CH1; CH5 checks it shows for 104 / 105 / 113 once the
  UnitTemplet rows are June's.
