# June GFx runtime - Phase G4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G4 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G4 done |
| Flag | `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN` |
| Log tag | `[JUNE_GFX] CUTIN` |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase G4 - Talk cut-ins" "Phase G0 - Port map"
```
Plus `june2017_re/g0_gfx_map.tsv` (the rows this phase used).

## Task

Review G4 against the G0 map: the widget name build (`CUT_IN_TALK_L_/R_n`, `Common_CutIn_Frame`), UI id 0xc, the callers ported
and the ones logged, and the `#else` path.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_GFX_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line; a FAIL is a finding.
Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `build.py --deploy`); hand design decisions to the user. Write "Phase G4 -- Review" into the plan doc, update the
STATUS row, write or check the next kickoff, commit.
