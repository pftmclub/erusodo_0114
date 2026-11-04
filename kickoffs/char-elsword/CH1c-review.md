# June character Elsword (Stage 7) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1c review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1c - Done (2026-11-04)"
awk -F'\t' '$7=="CH1c"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 2 CH1c functions against June (`python june2017_re/charport_adis.py elsword 32d18d80` and `32d1b910`; the listing carries the timer constants and call order):
- `32d1b910` is `CommonHyperModeFrameMove( 0.0f, 0.1f, true )` (args read off the stack: `[esp] = 0`, `[esp+4] = 0.1`, push 1).
- `32d18d80` (CH0 listed it `new`; March has `CommonHyperModeFrameMove`, so it is `inside`): world colour `0xff222222`; timer 1 = `EventTimerOneshot` (`+0x24c < t && 31d28e10`) then `ShowMinorParticleHyperModeChange`, `Stickslash1.ogg` when `bSound_`; timer 2 is an independent `if` (not `else if`): awakening stand-ins `32011d20` / `31fd66b0` / `32011f50`, Shock, `UpDownCrashCamera( 30, 0.4 )`, `ClearScreen`, Trace only without the awakening set, vslot 0xc0 `ApplyHyperModeBuff`, `Slash` / `Break` / `Change.ogg`; ends in vslot 0x7cc `CommonFrameMove`. Confirm the inline `ClearScreen` (DG manager `+0xa8 = 1`, colour 1,1,1,1) equals March's `ClearScreen()` defaults, and that `PlaySound( name, 1, 0 )` equals March's defaults.
- Confirm no `+0x7cc8` (checklist §7 L254) touch in either function.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1c - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1d.md`, commit with explicit pathspecs.
