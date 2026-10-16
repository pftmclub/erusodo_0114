# June GFx runtime - Phase G2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G2 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G2 done (Done note, commits) |
| Flag | `SERV_IRUHADEV_JUNE_GFX_MANAGER` |
| Log tag | `[JUNE_GFX] MGR` |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase G2 - The GFx manager" "Phase G0 - Port map"
```
Plus `june2017_re/g0_gfx_map.tsv` (the rows this phase used).

## Task

Review G2 against the G0 map: member layout and init order, the loader states, render placement and state-block
capture / apply (render-state leaks), lost / reset / resize (no leaked D3D resource on reset), the input order and swallow rule
(`+0x115`), the opener (algo-2 and March's loose-first rule), all 14 wrappers' ActionScript names and argument order, and the
`#else` path (flag off = March exactly).

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_GFX_MANAGER --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_GFX_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line; a FAIL is a finding.
Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `build.py --deploy`); hand design decisions to the user. Write "Phase G2 -- Review" into the plan doc, update the
STATUS row, write or check the next kickoff, commit.
