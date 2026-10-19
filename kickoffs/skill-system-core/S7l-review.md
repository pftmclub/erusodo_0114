# June skill system core - Phase S7l review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7l review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7l done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_EMBLEM_MANAGER` (+ `_DEBUG`: turn it off in this review once the play-test passed) |
| Log tag | `[JUNE_SKILL] EMBLEM` (`python tools/logs.py --tag JUNE_SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7l - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7l`.

## Task

Review Phase S7l against Ghidra (read-only):
- `X2EmblemManager.cpp`: the parser's `EXPAND_TEXTURE_FILE_NAME` block against `32449000` (after the sound loop, rows `{ name, x, y }` -> `3244f6f0`, row `+0x24` = 0, and
  the claim that the table is looked up inside an open `PLAY_SOUND_FILE_NAME_EMBLEM`); `JunePlayExpandTextureAndSound` / `JunePickEmblemSound` against `32448ce0`
  (hide test, `32447e50` with the entry's `+0x28` times, `+0x170`, `31cf71f0` position `( x, y, 0 )` and `( -1, -1 )` pairs, the one-shot erase `3244e020` / `32416450`,
  the sound pick `@32448e4d`); the June `OnFrameMove` against `3244c8c0` whole (follow loop x87 `@3244c9c7-3244ca7e`, double `334355b0` = 1.0, camera = DGManager `+0x17c`
  eye `+0x44`; cash-shop branch `@3244cabc`; the `+0x44` alive test; the state switch `0xb / 0x17`, `0xe / 0xf`, `0x15` with `32482650` vs March's
  `GetIsBossRaidCurrentField`; the pick `@3244ce15` with `31d03920`); `InitEmbelmPlayInfo` against `324485d0`; cases 5 / 7 against `3244c4b0` (`@3244c584`, `@3244c5a0-3244c6b4`).
- `X2BossRaidManager.cpp`: the gate-open emblem block against `3269f940` (row order, `33491cc8` / `33491d5c` / `33491d40`, `0x9c5a` / `0x9c5b`), the Ranox cases against `3269ece0`.
- `X2PremiumBuffManager.cpp`: the `EI_ROSE_EVENT_INT` call against `326c9030 @326c92dd`.
- data063: `RaidField_Area_0.dds` / `_1.dds` = June's bytes (`kom.py status data063`).
- Turn `SERV_IRUHADEV_JUNE_SKILL_EMBLEM_DEBUG` off (comment out its define) and rebuild once, if the play-test passed.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_EMBLEM_MANAGER --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7l -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7n), and commit.
