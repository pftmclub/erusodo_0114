# June character Add (Stage 7) - Phase CH1 - Deploy unit live: _LIVE, the June script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH1` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH1q2 review, CH0b |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_LIVE`, `_DP`, `_NASOD` defined (+ `..._DEBUG` for the load lines) |
| Log tag | `[JUNE_CHAR]` (`ADD DATA`, `ADD CORE`, `ADD DP`, `ADD NASOD`) |
| Review | `kickoffs/char-add/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1 - Done" "CH1 - Review"   # the enum-gate lesson
```

## Task

Define `SERV_IRUHADEV_JUNE_CHAR_ADD_LIVE` (+ `_DP`, `_NASOD`); adopt `Add_NasodRuler.lua` (`/x2-adopt-june`, `--dry-run` first) and pass `python
tools/luabind.py check Add_NasodRuler` **including its ENUM and STATE gates** (user, 2026-11-11: every unbound name bound whole or in
`lua_unbound_known.tsv`; every missing Enum member added, `luasrc.py refresh data036 Enum.lua`; Lua state ids equal to C++); copy CH0b's held motion
koms (if any); pack, copy; full build and deploy; the play-test on units of 9 / 32 / 122 (CH0c's new Add, CH0b's units, 36 / 37). Save a pre-CH1
ErrorLog first.

Done gate (plan, CH1 final): June's script and motions load (`ADD DATA` line); no new `KEM_ERROR` / `Motion Error` / missing-animation line against
the pre-CH1 run; combos, dash, jump, damage and riding play June's animations; DP fills, spends and shows on the gauge as June; formation mode enters,
costs DP and ends as June; the nasod armor combo follows a June hit; each skill cast twice shows its cool time and spends MP or DP; `charport_scan.py`
/ `charport_keys.py Add_NasodRuler` show nothing new. Hand-off: dev console first, per check where / paste / look for / if not.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
