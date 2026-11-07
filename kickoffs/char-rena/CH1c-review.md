# June character Rena (Stage 7) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1c review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`) and `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; CH1c adds no `new` row) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1c - Done"
awk -F'\t' '$7=="CH1c"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1c - Review"
```

## Task

Review the three CH1c functions against June (`python june2017_re/charport_adis.py rena 32c6d420 32ca7040 32c9f4f0`). Spot-check at least:
- **`32c9f4f0` = `CommonHyperModeFrameMove`** (CH0 said `new ?`; March has it under `MODIFY_RIDING_PET_AWAKE`, so it is `inside`): timer 1 and timer 2 are an `else if` in Rena (the timer-1 arm jumps to the closing `CommonFrameMove`; Raven's are two independent `if`s); both are the inlined `EventTimerOneshot`; world colour `0xff222222`; timer 1 `ShowMinorParticleHyperModeChange( L"Dummy1_Rhand" )` with no sound; timer 2 order: awakening begin `32011d20`, present test `31fd66b0` `== 1` -> play `32011f50`, `Shock`, `UpDownCrashCamera( 20, 0.3 )`, the inline `ClearScreen()`, `Trace` only when the set is absent, vslot 0xc0 `ApplyHyperModeBuff`, then the `bSound_` test with `Energy` / `Break` / `Change` (no Open Mind line, no `Raven_Slash1`); vslot 0x7cc `CommonFrameMove`. The three stand-ins in `_CONST_RENA_CH1C_` match Raven's / Chung's / Eve's / Elsword's (no-op, `false`, no-op).
- **`32ca7040`** = `CommonHyperModeFrameMove( 0, 0.1, true )` (`fld 0.1` at `esp+4`, `fldz` at `esp`, `push 1`) = March.
- **`32c6d420`** = the same six `TextureReadyInBackground` calls as `32c6acc0` (CH1a `HyperModeInit`); written out in June's riding Init rather than calling `HyperModeInit()`.
- The 37 CH1c states (riding 27, pepper 5, dance 3, amusement 2) have no other class-range function (CH0): confirm no CH1c row is missing from `ch0_class_map.tsv`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1c commit>^..<CH1c commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1c - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1d.md`, commit with explicit pathspecs.
