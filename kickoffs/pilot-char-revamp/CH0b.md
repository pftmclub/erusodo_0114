# June character Aisha (Stage 5 pilot) - Phase CH0b - Every June Aisha asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH0b` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_ARME_ASSET` |
| Log tag | `[JUNE_CHAR]` (`ARME DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH0 - Aisha map (read-only)" "Phase CH0b - Every June Aisha asset, whole, first"
```
Then CH0's Done note and `june2017_re/char_arme/ch0_assets.tsv` (owner Aisha, kind not Lua).

## Task

Do "Phase CH0b". The user's priority: Aisha's assets all become June's at once, with no half-March, half-June character.
1. Save a pre-CH0b `ErrorLog.txt` reference.
2. Stage every listed file with `kom.py stage-june`, byte-exact.
3. Pack each touched kom, then ask before each copy.
4. Hand over one play-test: one Aisha per job tier, a costume and a weapon worn, three skills cast, village and dungeon.

A file the list missed (seen as a missing-file line in the play-test) is staged in this phase and added to `ch0_assets.tsv`. Never keep
March's copy to make something load: a reader that breaks gets a code fix, under the flag above.

## Amended by CH0

(CH0 fills this in: totals, kom list, any reader fixes.)
