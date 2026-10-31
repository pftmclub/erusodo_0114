# June character Chung (Stage 7) - Phase CH0b - Every June Chung asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH0b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_CHUNG_ASSET` |
| Log tag | `[JUNE_CHAR]` (`CHUNG DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Chung asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH0b - Done (2026-10-26)"
```
Then CH0's Done note and `june2017_re/char_chung/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", Eve's way (her CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_chung.txt`.
2. `kom.py stage-june --dry-run` then the real run over `ch0b_stage_list.tsv`, byte-exact.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve (`data204` is a new archive). **If CH0 found a
   dropped animation March plays,** pack but hold the motion koms for CH1 final and say so.
4. Back up the save, then create the walk units it lacks: Iron Cannon (6), Fury Guardian (20), Shooting Guardian (21), Shelling Guardian (27),
   level 99 with class costume and top gear (`createchar.py`). Existing: 18 Freils (117), 42 RapidT (111), 43 NovaT (110), 50 Chung99 (111).
5. Hand over one play-test: one Chung per job tier, a costume and a weapon, three skills, village and dungeon.

A file the list missed (a missing-file line in the play-test) is staged here and added to `ch0_assets.tsv`. Never keep March's copy to make
something load.

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
