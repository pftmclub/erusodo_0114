# June character Elsword (Stage 7) - Phase CH1 - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1q2 review; CH0b done (its koms copied: CH0 found no dropped animation, nothing is held) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (defined here), `_CORE`, `_WOS` |
| Log tag | `[JUNE_CHAR]` (`ELSWORD DATA`, `ELSWORD CORE`, `ELSWORD WOS`) |
| Review | `kickoffs/char-elsword/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1 - Done (2026-11-02)" "CH1 - Review (2026-11-02)"
```
Then the CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_ELSWORD_LIVE`.
2. Adopt `Elsword_SwordMan.lua`, `WayOfSword.lua` and `DLG_Elsword_WayOfSword.lua` (`luasrc.py adopt-june`, `--dry-run` first; CH0 says
   which spelling the loader opens); `luabind.py check` on each. A `[JUNE_CHAR] ELSWORD DATA` line logs the script load. Re-run
   `charport_scan.py` and `charport_keys.py Elsword_SwordMan`: nothing new.
3. Run a Lua load check on every core-list file the class reads (Eve's `DamageEffect.lua` lesson).
4. Build, pack, ask before each copy (with CH0b's held koms if any), then hand over the play-test: an Elsword per job tier; combos, dash,
   jump, climb, riding, a bounding hit, damage, the Way of the Sword moving to vigor and destruction and entering both modes, awakening; the
   `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Elsword: CH1)`. A play-test defect that needs its own work gets the next letter (`CH1r`) with a kickoff and
  STATUS row.

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

## Amended by CH0 (2026-11-03)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): intro L14 character script Elsword_SwordMan.lua feeds SetDamageDataFromLUA; §7h L421 EFFECT_SET_LIST_* variants; Elsword among scripts using _FINAL / _FINA; §7h L425 USEFUL_VALUE rows and getters; §7t L590 SHOW_HYPER_CUT_IN_TIME in the class script; §7ad L750 stage June Motion_Elsword* files with the class script.
- No motion kom is held for this phase: CH0 found no lost animation (CH0b copies data033 / data199). Adopt `Elsword_SwordMan.lua`, `WayOfSword.lua` (see CH1q on its 14 unread keys) and `DLG_Elsword_WayOfSword.lua`.

## Amended by CH1q (2026-11-04)

- Define `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` together with `_LIVE` (`_LIVE` `#error`s without it). It also changes Elesis's March class at the shared
  sites (attack-result gain, `GetDestRageWSPGain_Elesis`), as June does.
- `WayOfSword.lua`: June's 14 extra keys have no reader (`3261aee0` reads March's 28; the MP rates stay the hard-coded 3 / 4 / 2): adopt it whole as
  bytecode (CH1q dry run: 1 replaced). Its 3 changed values go live with it (`Const_DestRageWSPGain` 1500 -> 2250, the two command constants 96 -> 67.2).
  `DLG_Elsword_WayOfSword.lua` is already identical to June's. `luabind.py check WayOfSword DLG_Elsword_WayOfSword`: PASS.
- Play-test lines (`_WOS_DEBUG`): `[JUNE_CHAR] ELSWORD WOS vigor start`, `destruction start`, `destruction rage`, `attack gain` (own unit, once a second).
