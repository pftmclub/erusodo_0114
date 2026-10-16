# June GFx runtime - Phase G3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G3 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G3 done |
| Flag | `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE` |
| Log tag | `[JUNE_GFX] SCENE` |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase G3 - Event scenes" "Phase G0 - Port map"
```
Plus `june2017_re/g0_gfx_map.tsv` (the rows this phase used).

## Task

Review G3 against the G0 map: the XML loader's keys and which files it reads, Play / queue / End / IsPlaying (IsPlaying covers
both scene classes), the end-frame callback, the input block set / clear, text and voice resolution, every reader G0 marked G3
(none missed, none extra), and the quest-UI callers left alone.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_GFX_EVENTSCENE --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_GFX_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line; a FAIL is a finding.
Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `build.py --deploy`); hand design decisions to the user. Write "Phase G3 -- Review" into the plan doc, update the
STATUS row, write or check the next kickoff, commit.
