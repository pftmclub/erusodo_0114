# June character Lu/Ciel (Stage 8) - Phase TU - Tutorial dungeon, field and world

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel TU` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_TUTORIAL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL TUTORIAL`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/TU-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase TU - Tutorial"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do "Phase TU". The core-list rows are only the tutorial's (plan Rules). `luabind.py check Dungeon_Tutorial_Luciel` = PASS before the
play-test. Hand-off: create a new Lu/Ciel and play the tutorial to its end.

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

## Amended by CH0 / NU0

CH0: (pending)

NU0 (2026-11-02), `nu0_jobchange_tutorial.tsv`:
- **June's entry diverges for every class.** `Handler_EGS_CREATE_TUTORIAL_ROOM_REQ` (June `325622e0`, caller `325712b0` inventory ACK) sends
  `DI_DUNGEON_TUTORIAL_*` (type 2 -> 0xa0c8 ... 10 -> 41240, 11 -> 0xa122, 12 -> 0xa1cc, default 0xa0be), event 0x5c. March
  (`X2StateServerSelect.cpp:7501`, copy `X2StateBeginning.cpp:4057`) sends `DI_BATTLE_FIELD_TUTORIAL_*` with no `UT_LUCIEL` case (Lu/Ciel gets
  Elsword's). Divergence row `Handler_EGS_CREATE_TUTORIAL_ROOM_REQ` (verdict `stage8`): TU ports June's whole body, every class. Check the
  other classes' `DI_DUNGEON_TUTORIAL_*` rows exist in the live `DungeonData.lua` first; a missing one is a TU gap, not a reason to keep March's.
  Emulator: `Handlers_Room.cpp:568`.
- Data: `DI_DUNGEON_TUTORIAL_LUCIEL` 41240 (`DungeonEnum.src.lua:101`) and its `DungeonData.src.lua:29877` row are missing from the live files.
  `DI_BATTLE_FIELD_TUTORIAL_LUCIEL` 39609 is in the live `DungeonEnum.lua` but June has no data row and no code reads it: not needed.
  `WI_TUTORIAL_LUCIEL_STAGE0` 633 is missing from the live `Enum.lua`, and the live `WorldData.lua` has no `WI_TUTORIAL_*_STAGE0` row for any class
  (June `WorldData.src.lua:3153`): add Lu/Ciel's.
- Files: `Dungeon_Tutorial_Luciel.lua` live (June bytecode); `Tutorial_Luciel_Stage0.lua` (15,580 B) and `_LineMap.lua` (5,332 B) June data040, new
  (CH0b); `HQ_Loading_TUTORIAL_LUCIEL_01/02.tga` not found in kom_contents (CH0b checks June's koms).
