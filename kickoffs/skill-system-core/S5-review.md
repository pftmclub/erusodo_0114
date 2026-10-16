# June skill system core - Phase S5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S5 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S5 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (+ `SECOND_HOMES` in `tools/flagcheck.py`) |
| Log tag | `[JUNE_SKILL] SLOT` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S5 - Slots 4+4 -> 5+5"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S5's commit(s) against the S0a / S0b map it used: both structs' Put/Get order against S0b's spec and the `cl /P` proofs in both projects; every `s0b_slot_sites.tsv` row ticked (grep `EQUIPPED_SKILL_SLOT_COUNT`, `> 3`, `< 4`, `[4]` again); `SlotIndexToSlotType` values; TC locked; BBT 98 / BFT 16 see slot types; the v18 rung and its snapshot; flagcheck with both homes.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S5 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
