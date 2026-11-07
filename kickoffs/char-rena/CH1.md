# June character Rena (Stage 7) - Phase CH1 - Final: LIVE, the script, the motions, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1q review; CH0b done (any motion koms CH0 held are packed and held for this phase) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (defined here), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` |
| Log tag | `[JUNE_CHAR]` (`RENA DATA`, `RENA CORE`, `RENA NF`) |
| Review | `kickoffs/char-rena/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1 - Done (2026-11-06)" "CH1 - Review (2026-11-06)"
```
Then the Rena CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_RENA_LIVE`.
2. Adopt `Lire_ElvenRanger.lua` (`luasrc.py adopt-june`, `--dry-run` first); `luabind.py check Lire_ElvenRanger`. A `[JUNE_CHAR] RENA DATA` line logs
   the script load. Re-run `charport_scan.py` and `charport_keys.py Lire_ElvenRanger`: nothing new.
3. Run a Lua load check on every core-list file the class reads (Eve's `DamageEffect.lua` lesson).
4. Check `CX2GULire_ElvenRanger::ReservedSkillStart` is in place (CH1k) before the play-test.
5. Build, pack, ask before each copy (data036 with CH0b's held motion koms, if any), then hand over the play-test: a Rena per job tier; combos, dash,
   jump, double jump, riding, damage; natural force filling, spending and showing on the gauge as June (per decision 1); each skill cast twice (cool
   time on the slot, MP drop); the `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Rena: CH1)`. A play-test defect that needs its own work gets the next letter (`CH1r`) with a kickoff and
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

## Amended by CH0

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
