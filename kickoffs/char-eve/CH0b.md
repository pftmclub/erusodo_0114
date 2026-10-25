# June character Eve (Stage 7) - Phase CH0b - Every June Eve asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH0b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_EVE_ASSET` |
| Log tag | `[JUNE_CHAR]` (`EVE DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Eve asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH0b - Done (2026-10-20)"
```
Then CH0's Done note and `june2017_re/char_eve/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", the pilot's way (Aisha's CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_eve.txt`.
2. `kom.py stage-june --dry-run --plan-out ...` then the real run over `ch0b_stage_list.tsv`, byte-exact.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve. **If CH0 found a dropped animation March plays,**
   pack but hold the motion koms for CH1 final and say so.
4. Hand over one play-test: one Eve per job tier (the save has units 19 / 27 Empress, 22 Nasod, 41 Electra; add others with
   `/x2-create-char` if CH0 asks), a costume and a weapon, three skills, village and dungeon.

A file the list missed (a missing-file line in the play-test) is staged here and added to `ch0_assets.tsv`. Never keep March's copy to make
something load.

## Amended by CH0

(CH0 fills this: the list, the koms, cross-kom moves, readers a June asset could break, the held koms if any.)
