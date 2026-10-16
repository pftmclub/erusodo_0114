# June effect completion - C3j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3j review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_BOUNDING_STATE` |
| Log tag | `JUNE_FX C3j` (no lines: the code is dormant) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3j - Done"
grep -n "(C3j)" june2017_re/funcmap.tsv
```

## Task

Review C3j's commit against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`, `dispsweep.py`):

- **The key table** `0x334b9178` (entries 33-35 = `BOUNDING_DOWN / _LANDCRASH / _UP`) and the loop `3279dc80 @3279f1cd-3279f2ad`:
  the default name from `3275d900` applies only when the key's string is empty (`[esp+0x64]` = size), and its lookup result is 0 when absent.
- **The UP launch's new position** at the end of `CX2GUNPC::CommonStateStart`: re-match June's tail (`31e4d160`, `32275440`,
  `32071c30`, `32761b40` @`327af1d2-327af1f7`) to March's last lines, and confirm no March line after InsertBuffFactor writes nowSpeed.
- **OnFrameRender_Draw** `32768c70 @32768cd1-32768d40`: the order (table first), EndTable only when the table opened, the two gates before it
  are March's.
- **The virtuals**: slots `0x2f0 / 0x2f4` in the NPC (`0x334ba584`), user (`0x334781f4`) and base (`0x334883b4`) vtables; user `31fd7ec0`
  compares the Now index with `0x7a`, `31fd7f00` Now or Future with `0x79` (March's `USI_BOUNDING_UP / _LANDCRASH`); NPC LANDCRASH also
  tests the word `+0x8404` (March `unitCondition.nextState`, a short under the flag).
- **The trace** (Done note points 1-4): spot-check two of them, in particular that `+0x40f0 / +0x40f4` have no third writer.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_BOUNDING_STATE --range <C3j commit>^..<C3j commit>`; paste its summary lines and explain
the 13 deleted lines (C3d-3's own, inside the flag).

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). The fallback port itself is `C3j-1`, after the user's
decision; do not write it here. Write "C3j - Review" into the plan doc, update STATUS, commit.
