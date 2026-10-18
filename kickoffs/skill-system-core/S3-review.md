# June skill system core - Phase S3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_REFORM` (+ `_DEBUG`) |
| Log tag | `[JUNE_SKILL] MIG` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0a - Templet and tree RE" "Open questions (planning, 2026-10-17)" "Phase S3 - The flip: June skill data and the save migration"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S3's commit(s) against the S0a / S0b map it used: the adopted closure equals S0a's tsv (sha1 = `ljconv(June)`), the load order, the v17 rung (one-way refusal, every table S0a named, SP by June's rule, defaults per class), the snapshot taken before the migrating launch, page numbers never 0 in the handlers, and the checklist section.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S3 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

## What S3 added beyond the plan text (check these too)

- **June `GetSkillLevel 32890620` whole** (`X2UserSkillTree.cpp`, amendment 9): compare with the disassembly `@32890620-3289071a`; the guild
  branch's `31d717d0` test (state 0x17 / 0x18 = June PVE states, none in March) is a comment, not code.
- **Level-0 skills are not loaded or sent** (`X2OfflineSkill.cpp` Load / GetAcquiredSkills), because June's getter reads 0 as 1 and June's
  `SetSkillLevelAndCSP 32897240` erases below 1. Confirm no other path sends a 0 row.
- **The blank-state guard** (`X2GUUser.cpp` `JuneIsBlankSlotSkillState` / `JuneLogNoStateSkill`, `StateChange`): a March safety for slot
  skills whose class code has no state (June types 14-16 hit every class's `default: return`). Not June code: judge whether it can refuse a
  state June-ported code relies on.
- **The emulator's `UNIT_CLASS` seed now merges** into Enum.lua's table (`X2OfflineLuaEnum.cpp`); before, it replaced the global and would
  have turned the 31 June classes back into nil for later loads.
- **The generated table** `X2OfflineJuneSkillAutoLearn.h` (`june2017_re/s3_autolearn_gen.py --check` must print SAME x3) and the `_DEBUG`
  `[JUNE_SKILL] MIG autolearn check` line (only generated / only loaded / require-level diffs all 0).
