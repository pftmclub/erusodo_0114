# June character Chung (Stage 7) - Phase CH4c - Memo and item readers

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4c` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4b review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |
| Review | `kickoffs/char-chung/CH4c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Chung lines of the checklist rows named below (`june2017_re/char_chung/plan_checklist_rows.md` gives the line numbers), the `CH4c`
rows of `ch0_*.tsv`, and Eve's matching CH4 Done note as the worked example.

## Task

Scope: `MemoConditionChung` (RTTI `.?AVMemoConditionChung@CX2GUChung@@`, JUNE_BUGS 332, `EFFECT_SET_LIST_MEMO`); 7u's grenade memo
(`X2BuffTemplet.cpp:1695-1706`: on the 20% memo-26 roll the `else` skips `HAND_GRENADE_0`) and `CHUNG_BIGBANGSTREAM_MEMO` /
`_EXPLOSION_MEMO`; Metabolism Boost in `CX2Game::UseItemSpecialAbility` (`322fbc60`, `@322fbe08`-`322fbe83` and `@322fd495`-`322fd510`: unit classes
by name for `0x14` / `0x6e`, skill `0x6720` by name, `ability - 1.0 + 5e-8` inlined). Engine files touched here (`X2BuffTemplet.cpp`, `X2Game.cpp`)
keep their own `#else`.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`.
Mark rows `(Chung: CH4c)`. Done gate: each item's effect visible in game or in a named log line.

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
