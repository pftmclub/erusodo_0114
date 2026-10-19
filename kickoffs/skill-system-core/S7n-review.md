# June skill system core - Phase S7n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7n review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7n done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TITLE_NOTICE` |
| Log tag | `[JUNE_SKILL] EMBLEM title` (`python tools/logs.py --grep "EMBLEM title"`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7n - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7n`; `JUNE_BUGS.md` rows 348-351.

## Task

Review Phase S7n against Ghidra (read-only):
- `X2TitleManager.cpp`: `CheckNewTitle` against `32701960` whole (level gate `@327019b5`, both jump tables `3270208c` / `327020a8` and their March state mapping, the dungeon
  branch `@32701a78`, the default branch `@32701b49` with the 43200 box `@32701d75-32701dcc` (( -999, -999 ) = `33458018`, 3085 / 3086), the play-guide block `@32701e90`);
  `JunePushTitleNotice` against `326f88a0` (type 7 test `@326f8967`, sets `+0xd0` / `+0xe0`, push order); `JuneEquipTitleTen` against `327009b0`; REWARD_TITLE_NOT against
  `32700700` (`@327007c6-32700921`); UpdateMission against `32700d20` (`@32700dbb-32700ddf`, the bFirst loop `@32700e3b`); NEW_MISSION_NOT against `32701700`; TakeTitle against
  `326ff600 @326ff7c4-326ff8ac`; EQUIP_TITLE_ACK against `327032c0 @32703350`.
- `X2State.cpp`: the 3085 / 3086 cases against `31f26bfb` / `31f26c45` (switch `@31f242cb`, tables `31f27124` / `31f27020`).
- `X2EmblemManager.cpp`: `JunePushTitleNotice` against `3244adb0`; case 0x12 against `3244c4b0 @3244c71d-3244c7c5`; `JuneShowNewTitle` against `3244c2b0`; icon against
  `32448420` (`326f8400` = GetTitleModel); text against `3244bfb0` (17352 "L" / 757, the string-0 test before SetShow).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_TITLE_NOTICE --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7n -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
