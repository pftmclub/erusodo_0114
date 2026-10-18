# June skill system core - Phase S7 review (the learn path)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_ACQUIRE` (also in `KncWX2Server/Common/ServerDefine.h`, `SECOND_HOMES`) |
| Log tag | `[JUNE_SKILL] ACQ` (`python tools/logs.py --grep "JUNE_SKILL] ACQ"`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7 - Skill tree UI and learn logic" "Phase S7 - Done (2026-10-18, Opus 5.5 high)"
```
Funcmap rows tagged `(S7)` in `june2017_re/funcmap.tsv`.

## Task

S7 was split (user, 2026-10-18): S7 is June's learn path, the tree UI is S7b. Review S7's commit(s):

- The finding that June never learns client-side: 0x11c has no sender, `3220df10` no caller (re-run the push / call scan, or
  spot-check it in Ghidra).
- The ports against June: `328924c0` (add + base erase), `32206d30` (level band, hyper-class skip), `328db990` + `328920d0`
  (auto-equip, A then B, no TC), `3220e3b0` / `3220e5f0` / `322105c0` (the slot-change queue: gates, dedup, 10 s input lock, pump),
  the grant handler's UI refresh (`323b0770` / `32535be0`).
- The emulator's grant is a **labelled stand-in** (June's rule is server-only): check it is announced in the log, never silent, and that
  its skips (no required level, `SPRT_HYPER_CLASS`, sealed) and the base-replacement slot move are what the Done note says.
- The routing choice (one handler in `CX2SkillTree::UIServerEventProc` instead of June's per-state copies) and the level-up call sites
  (March `CX2TFieldGame`, `CX2StateMenu`; June's third copy `325346e0`).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_ACQUIRE --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7 -- Review" into the plan doc, update the
phase's STATUS.md row, check `S7b.md`, and commit.
