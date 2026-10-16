# June GFx runtime - Phase G5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G5 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G5 done |
| Flag | `SERV_IRUHADEV_JUNE_GFX_GATES` |
| Log tag | `[JUNE_GFX] GATE` |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase G5 - Owed gates" "Phase G0 - Port map"
```
Plus `june2017_re/g0_gfx_map.tsv` (the rows this phase used).

## Task

Review G5: the cut-in term's position among C3t-1's terms, the ESC gate order against `31f1e1a0`, and that every gate not
ported has its Stage 9 note.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_GFX_GATES --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_GFX_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line; a FAIL is a finding.
Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `build.py --deploy`); hand design decisions to the user. Write "Phase G5 -- Review" into the plan doc, update the
STATUS row, write or check the next kickoff, commit.
