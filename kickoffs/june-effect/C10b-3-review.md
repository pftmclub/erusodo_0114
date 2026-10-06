# June effect completion - C10b-3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C10b-3 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C10b-3 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION_UI` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C10 - Decided"
```

Also read: the `june2017_re/funcmap.tsv` rows tagged `Effect C10a` that `kickoffs/june-effect/C10b-3.md` lists, and the
phase's commits (`git log --oneline --grep="C10b-3"`).

## Task

Review C10b-3's commits against the June binary and its kickoff: the UI stand-in matches D9, keeps June's range [0.1, 1.0], reset-to-1.0 and FLT_EPSILON apply rule, and saves through C10b-2's keys; it is labelled as a designed stand-in, not a port.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION_UI --range <first C10b-3 commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and explain every OTHER row and deleted line; a FAIL or
ERROR is a finding.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C10b-3 - Review" under C10 in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
