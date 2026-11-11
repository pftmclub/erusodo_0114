# June character Lu/Ciel (Stage 8) - Phase CH1 - Class core: Final: Lu/Ciel goes live

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1p review, NU4 review, CH0b |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_DUO` defined |
| Log tag | `[JUNE_CHAR]` (`LUCIEL CORE` / `DATA`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH1-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1 row: define `_LIVE` and `_DUO`, add the `UT_LUCIEL` cases to `CX2Game::CreateGUUser` (`X2Game.cpp:21276`) and the intruder
copy (`:2141-2201`), adopt `Luciel_Duo.lua` and `Luciel_Demonio.lua` (`adopt-june`, bytecode; `luabind.py check` both = PASS), copy the motion
koms CH0b held, and add the `[JUNE_CHAR] LUCIEL DATA` line at the end of InitState. Play-test (the plan's CH1 row): create, enter the village,
walk, jump, dash, combo, swap bodies, enter a dungeon, relog. Write `CH1-review.md` and amend CH2a / DU3 / CHN / TU from what the play-test shows.

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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 per-character parser census; §7h L421 EFFECT_SET_LIST variants; §7h L423 SOUND_PLAY_EX x5, CUSTOM_STATE_DELETE_SOUND x5, STATE_SOUND_PLAY x0; §7h L424 USEFUL_VALUE0..4 rows and getters; §7o L506-507 SET_LIMIT_MAX_G_SPEED -4000 in <prefix>DAMAGE_BOUNDING_DOWN (13 tables; Lu and C; §7p L545 TIME_STOP_IMMUNE_STATE = TRUE in the class script; §7t L586-589 SHOW_HYPER_CUT_IN_TIME in the class script; §7x L664 combo hits carry June AT_COMMAND tag; adopt the script with its tags.
- Adopt `DLG_LUCIEL_BOUNDING_KEY.lua` (data034) with the scripts: Init 0x40 `32b1e8e0` loads it at `32b1ea23`. `luabind.py check` all three.

## Amended by char-ara CH1 (2026-11-11): the enum gate

`luabind.py check <script>` now also runs the ENUM and STATE gates (tool docstring). Ara's CH1 shipped with her June script reading
names the merged data036 `Enum.lua` lacked: the file died at load (`can't find 'CRS_INVISIBLE' class variable`) and her `CHAIN_SYSTEM`
popped `StateID` on every field entry. The check must print `RESULT: PASS` **before** the pack. Fix every `ENUM MISSING TABLE / NAME` by adding
June's member to `Enum.lua` (core-list source, `-- char-luciel CH1` one-line comment, then `luasrc.py refresh data036 Enum.lua`), and every
`STATE MISMATCH / NO C++ MEMBER` so the Lua value equals the C++ enum (add the C++ member first when it is missing). `enum nil in June` lines
are June's own nils: leave them.
Run of 2026-11-11 (pre-port): `DUO_TYPE.DT_BOTH` / `DT_KEEP` and `SKILL_ID.RNSI_A_LLD_BLUFF` missing.
