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

## Amended by G0 (2026-10-19)

The scope is June's user skill cut-in (see G4.md's amendment):
- `31fd95a0` / `31fde0f0` against March's ActiveSkillShow;
- the `.gfx` name build;
- the 8 callers mapped or logged;
- CX2GUNPC left on the texture route.

## Amended by G4 (2026-10-19)

- Turn `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN_DEBUG` off (comment its define, as G2's review did) and rebuild; run flagcheck with `--with SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN_DEBUG`.
- Spot-check in Ghidra: `31fd95a0` (the `bOnlyLight` stack arg `[ebp+0x30]` @31fd95ef, the GetIsRight sense), `31d76b40` (`L_` / `R_` strings `33456858` / `3345685c`, the (0xc, frame, side, 1) / (0xc, talk, 0, 0x3c) args), `31fc7720` (member order).
- Check the play-test log for `play ui=0xc ...` lines and that no `EXCEPTION` or crash followed.
- Names: the March tables' `.dds` names are the character ports' (checklist after 7t); do not rename them here.
