# June character Aisha (Stage 5 pilot) - Phase CH1 - Deploy unit: June script, motions and the class core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (+ `SERV_IRUHADEV_JUNE_CHAR_DEBUG` if the phase logs per frame) |
| Log tag | `[JUNE_CHAR]` (`python tools/logs.py --tag JUNE_CHAR`; lines `ARME DATA` / `ARME CORE`) |
| Review | `kickoffs/pilot-char-revamp/CH1-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
```
Then CH0's Done note and the `ch1` rows of `june2017_re/char_arme/ch0_class_map.tsv`, `ch0_state_skill.tsv`, `ch0_checklist_rows.tsv`,
`ch0_assets.tsv`. Read each checklist row CH0 assigned to CH1 by its heading (`docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`).

## Task

Do "Phase CH1". The script, the motions and the C++ go live together:
1. Build the code.
2. Stage, pack and ask before each copy.
3. Hand the play-test over only once both the exe and the koms are deployed.

- `X2GUArme_VioletMage.*` and `X2GUUser.*` are CP949: edit them with `bytepatch.py` only.
- Check each `#ifdef` branch you edit with `cl /P` (`SetEquippedSkillFuncBySkillSlotIndex` carries `UPGRADE_SKILL_SYSTEM_2013` pairs).
- After the phase, append `(Aisha: CH1, <commit>)` to each checklist row it ported.
- The play-test hand-off names: an Aisha save and class, combos / dash / jump / a bounding hit, and the `ErrorLog.txt` lines to compare
  against a pre-CH1 run.

## Amended by CH0

(CH0 fills this in: tsv filters, addresses, splits.)
