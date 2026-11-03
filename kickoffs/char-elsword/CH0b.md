# June character Elsword (Stage 7) - Phase CH0b - Every June Elsword asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH0b` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH0 (may run before char-chung Z) |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_ASSET` |
| Log tag | `[JUNE_CHAR]` (`ELSWORD DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Elsword asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH0b - Done (2026-10-31)"
```
Then CH0's Done note and `june2017_re/char_elsword/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", Chung's way (his CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_elsword.txt`.
2. `kom.py stage-june --dry-run` then the real run over `ch0b_stage_list.tsv`, byte-exact. June's motions come from June's `data199`; March's
   `data199.kom` is a different archive: stage them into the kom CH0 named.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve. **If CH0 found a dropped animation March plays,**
   pack but hold the motion koms for CH1 final and say so.
4. `python tools/skillicons.py check <Elsword's skill-id range>` = PASS; stage what it lists (shared `DLG_SKILL_Passive_*` sheets included).
5. Back up the save, then create the walk units it lacks: Sword Knight (10) and a Sword Man (1), level 99 with class costume and top gear
   (`createchar.py`). Existing: 25 jtLordKnight (100), 26 jtRuneSlayer (101), 32 MagicKnightT (11), 33 SheathKnightT (22), 34 InfinitySwordT (112).
6. Hand over one play-test: one Elsword per job tier, a costume and a weapon, three skills, village and dungeon.

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

(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)
