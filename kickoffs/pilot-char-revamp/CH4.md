# June character Aisha (Stage 5 pilot) - Phase CH4 - Passives and class mechanics

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2 review; not alongside CH3 (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`, `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`, `ARME MEMO`) |
| Review | `kickoffs/pilot-char-revamp/CH4-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)" "7aj. Tree-UI class pieces June keyed to one class (from June skill system S7b)" "7ak. Skill page decide ACK class pieces (from June skill system S7d)"
```
Then the `ch4` rows of CH0's tsvs, and CH2's Done note.

## Task

Do "Phase CH4". Map each June mechanism in Ghidra before porting it (memorize: slot-data type 5 `328d9ac0`, tooltip `328e9160`, decide ACK
`32ce1770`, and the class side CH0 found). Delegate the mechanical reads, builds and log reads to subagents.
- If memorize needs a save change, snapshot `db_backup/` first. The rung is forward-only; propose it to the user before writing it.
- Edit CP949 files with `bytepatch.py` only.

## Amended by CH0

(CH0 fills this in.)
