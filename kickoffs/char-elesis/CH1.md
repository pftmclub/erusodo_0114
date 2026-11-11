# June character Elesis (Stage 7) - Phase CH1 - Deploy unit live: _LIVE, the June script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1q review, CH0b |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE`, `_WOS` defined (+ `..._DEBUG` for the load lines) |
| Log tag | `[JUNE_CHAR]` (`ELESIS DATA`, `ELESIS CORE`, `ELESIS WOS`), `[JUNE_SKILL]` (`--grep SBUF`) |
| Review | `kickoffs/char-elesis/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1 - Done" "CH1 - Review"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1 - Done (2026-11-04)"   # the Way of the Sword checks
```

## Task

Define `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (+ `_WOS`); adopt `ELESIS_KNIGHT.lua` (`/x2-adopt-june`, `--dry-run` first; March's copy is bytecode outside the ledger:
adoption replaces it) and pass `python tools/luabind.py check ELESIS_KNIGHT` (every unbound name bound whole or in `lua_unbound_known.tsv`); copy
CH0b's held motion koms (if any); pack, copy; full build and deploy; the play-test on units 20 (Knight), 40 (Saber Knight), 21 (Pyro Knight), 29
(Grand Master), 30 (Blazing Heart). Save a pre-CH1 ErrorLog first.

Done gate (plan, CH1 final): June's script and motions load (`ELESIS DATA` line); no new `KEM_ERROR` / `Motion Error` / missing-animation line
against the pre-CH1 run; combos, dash, jump, damage and riding play June's animations; the Way of the Sword gauge fills both ways, vigor and
destruction start and end as June and the state survives a stage change; Endurance Power and Flame Sword produce the `[JUNE_SKILL] SBUF parse /
frame / state end` lines (the first live reserved-buff path, 7ai); each skill cast twice shows its cool time and spends MP; `charport_scan.py` /
`charport_keys.py ELESIS_KNIGHT` show nothing new. Hand-off: dev console first, per check where / paste / look for / if not.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).

## Amended by char-ara CH1 (2026-11-11): the enum gate

`luabind.py check <script>` now also runs the ENUM and STATE gates (tool docstring). Ara's CH1 shipped with her June script reading
names the merged data036 `Enum.lua` lacked: the file died at load (`can't find 'CRS_INVISIBLE' class variable`) and her `CHAIN_SYSTEM`
popped `StateID` on every field entry. The check must print `RESULT: PASS` **before** the pack. Fix every `ENUM MISSING TABLE / NAME` by adding
June's member to `Enum.lua` (core-list source, `-- char-elesis CH1` one-line comment, then `luasrc.py refresh data036 Enum.lua`), and every
`STATE MISMATCH / NO C++ MEMBER` so the Lua value equals the C++ enum (add the C++ member first when it is missing). `enum nil in June` lines
are June's own nils: leave them.
CH0's run (2026-11-11, pre-port): 4 `SKILL_ID` names missing (`RNSI_A_SCA_BLOOD_STING`, `RNSI_A_SGM_UPPER_SLASH`, `RNSI_P_SBH_BLAZE_ABSOLU`,
`RNSI_SA_SCA_CHASING_SWORD`).
