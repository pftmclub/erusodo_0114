# June character Chung (Stage 7) - Phase CH1 - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1q review; CH0b done (its koms copied, or held for this phase if CH0 found a dropped animation March plays) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (defined here), `_CORE`, `_CBALL` |
| Log tag | `[JUNE_CHAR]` (`CHUNG DATA`, `CHUNG CORE`, `CHUNG CBALL`) |
| Review | `kickoffs/char-chung/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1 - Done (2026-10-27)" "CH1r - Done (2026-10-27)"
```
Then the CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_CHUNG_LIVE`.
2. Adopt `Chung_Iron_Cannon.lua` (`luasrc.py adopt-june data036 Chung_Iron_Cannon.lua`, `--dry-run` first; March ships it under two spellings,
   `Chung_Iron_Cannon.lua` and `CHUNG_IRON_CANNON.lua`: CH0 says which the loader opens) and any summon script CH0 marked CH1; `luabind.py check` on
   each. A `[JUNE_CHAR] CHUNG DATA` line logs the script load. Re-run `charport_scan.py` and `charport_keys.py Chung_Iron_Cannon`: nothing new.
3. Run a Lua load check on every core-list file the class reads (Eve's `DamageEffect.lua` lesson).
4. Build, pack, ask before each copy (with CH0b's held koms if any), then hand over the play-test: a Chung per job tier; combos, dash, jump, guard,
   riding, a bounding hit, damage, cannon balls filling / spending on the gauge, awakening; the `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Chung: CH1)`. A play-test defect that needs its own work gets the next letter (`CH1r`) with a kickoff and STATUS row.

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
