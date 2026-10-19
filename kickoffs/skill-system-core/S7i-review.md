# June skill system core - Phase S7i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7i review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7i done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM` |
| Log tag | `[JUNE_SKILL] EMBLEM` (`python tools/logs.py --tag JUNE_SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7i - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7i`.

## Task

Review Phase S7i against Ghidra (read-only):
- The feed: `CX2SkillTree::JuneShowNewSkillOrMemoEmblem` against `31d885e0` (the `324511a0` class gate, the level read `+0xb4` XOR, the `( float id, float type, 1.0 )`
  row order = map order), `JuneGetNewSkillOrMemoByLevel` against `3220d2b0` (types 1 / 3 on the first required level, 4 on a later one, 2 per memo row, the hyper-class
  skip as in S7's `32206d30`), `JuneAddNewSkillOrMemo` against `3220b990`. The two call sites: `X2TFieldGame.cpp` (end of the my-unit block, June `323ae277` sits in
  that branch) and `X2StateMenu.cpp` (between `SetIsLevelUp` and `DisplayLevelUpEffect`, June `324744a5` tests vtable `+0x198` = IsMyUnit). Find which March state the
  third handler `325346e0` belongs to, if any.
- The emblem side in `X2EmblemManager.cpp`: `PlayEmblemEffect`'s June body against `3244b780` (texture checks, the 2nd only with a 1st, the dialog block @3244b91a,
  the ctor arguments `0.07` / `0`, the sound pick = March's), `JuneFadeEmblemDialog` against `3244c8c0` @3244cb66-3244cce2 (x87 compares: close at `t >= duration`,
  fade in while `0 <= t < FADE_IN`, fade out while `duration - FADE_OUT <= t < duration`), `JuneIsEmblemPlaying` = `32447870`, `JuneCloseEmblemDialog` = `32447760`,
  the close calls in `InitEmbelmPlayInfo` (`324485d0` @3244864c) / the cash-shop branch (@3244caf0) / before the pick (@3244cd72), case 17 = `3244c4b0` case 0x11,
  and the row helpers `32447a20` / `32448b20` / `32447aa0` / `32447c00` / `324486c0` (string ids 35571 / 35572 / 35573 / 35745, format "L", 340 px with font 1,
  the 8 px raise = double `33439cb0`).
- The data: data036 `Emblem_Manager.lua` = June's bytes (ledger / `kom.py status`), `Enum.lua` EMBLEM_ID 14-34 = June's values, data217
  `DLG_Get_New_Skill_or_Memo.lua` and data053 `DLG_Ui_Guild_01` June-exact. `luasrc.py ledger --check` and `core --check` PASS.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7i -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7l), and commit.
