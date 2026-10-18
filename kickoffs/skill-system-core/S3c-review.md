# June skill system core - Phase S3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3c review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3c done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TYPE` |
| Log tag | `[JUNE_SKILL] TYPE` (`python tools/logs.py --grep "JUNE_SKILL] TYPE"`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Phase S3c - June skill types 14-18 in March's engine"
```
Then `june2017_re/s3c_skill_type_readers.tsv` (one row per June reader of a skill type, with its March reader and S3c's action).

## Task

Review Phase S3c's commit against the tsv: the five enumerator values (June `Enum.lua` `SKILL_TYPE`), the two case groups in
`X2SkillTree.cpp` (`Handler_EGS_RESET_SKILL_ACK` = June 322112d0's switch @32211750; the withdrawn `Handler_EGS_GET_SKILL_ACK` cases
(removed: dead `#else` of `UPGRADE_SKILL_SYSTEM_2013`; check nothing else S3c touched sits in a dead branch, with `cl /P` if unsure), and the TYPE census against June's data (types 14 / 15 / 16 = 372 / 467 / 309 of 2,346; none of 7, 17, 18).

Also:
- Re-run the June-side sweep (`typesweep.py` method in the Done note: every call to GetSkillTemplet 32206450 / type-by-id 32205ab0
  followed by a type compare or a jump table) and confirm no engine reader with a March counterpart was missed.
- Identify the three unidentified June-only readers in the tsv (`31fcd3d0` guide event, `32622bd0` / `32622e80` DamageManager rate).
  Route each: an existing STATUS row, the checklist, or a new phase (memory `plan-unowned-june-branches-without-asking`).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_TYPE --range <S3c commit>^..<S3c commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S3c -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
