# June skill system core - Phase S7f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7f review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7f done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_MEMO_RESYNC` |
| Log tag | `[JUNE_SKILL] MEMO` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7f - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7f`.

## Task

Review Phase S7f against Ghidra (read-only):
- `CX2Room::Handler_EGS_CHANGE_SKILL_INFO_IN_ROOM_NOT` (`X2Room.cpp`) against `31ed4c80` line by line: the game-user / room-slot fallback, the packet offsets
  (+9 KStat, +0x41 KUnitSkillData, +0xa7 guild, +0x97 passives, +0xb7 notes, +0xe9 memo map), the unconditional slot set (`32895d20`, `31fd84c0` only with a user),
  and the `31ed3f90` / `31ed23a0` room loop (slot state 1 / 2 skipped).
- The identifications that routed work: `31f276b0` as the class-change NOT (S7h), `3257b610` as the select-unit ACK (not ported: packet division),
  `31d885e0`'s callers and emblem 17 (S7i), and the item-3 claim (`32890100` = `GetEqipSkillMemo`, no caller: xrefs + `.text` byte scan).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_MEMO_RESYNC --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7f -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG`:** the retire note lives in `S8-review.md`; leave it there.
