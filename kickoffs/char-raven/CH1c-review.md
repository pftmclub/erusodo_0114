# June character Raven (Stage 7) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1c review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final); `Always.h` gained a dependency guard on `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1c - Done"
python june2017_re/charport_adis.py raven 32c4f350 32c511a0   # the two listings
```

## Task

Review the 2 CH1c class-map rows (both `inside`; CH0's `new ?` on `32c4f350` was corrected: March has `CommonHyperModeFrameMove`) against the listings:
- **`32c511a0`** = `CommonHyperModeFrameMove( 0.f, 0.1f, true )` (`push 1`, `fldz`, `fld 0.1`, `call 32c4f350`).
- **`32c4f350`**, in order: world colour (`+0xb4..0xd4`, the inline `SetWorldColor( 0xff222222 )`); timer 1 (`EventTimerOneshot`) `ShowMinorParticleHyperModeChange` with **no sound**;
  timer 2 as a separate `if`: `32011d20`, `31fd66b0` (true -> `32011f50`), `ShowMinorParticleHyperModeShock`, `UpDownCrashCamera( 30, 0.4 )`, the inline `ClearScreen`, `31fd66b0` again
  (false -> `ShowMinorParticleHyperModeTrace`), vslot 0xc0 `ApplyHyperModeBuff`, then **`31fceab0( 0x5f63 ) > 0` -> vslot 0x23c `( 0x5f63, 0, 1 )`** = `SetSkillBuffFactors( RNSI_P_RRF_OPEN_MIND, 0, true )`
  (confirm the argument order from the pushes, 0x5f63 = 24419 against June `Enum.src.lua`, and that the check runs whether or not `bSound_`), then the four sounds when `bSound_` (`Energy`, `Raven_Slash1`, `Break`, `Change`), then vslot 0x7cc `CommonFrameMove`.
- The three awakening stand-ins in `_CONST_RAVEN_CH1c_` (no-op, `false`, no-op) are Elsword's / Chung's / Eve's (funcmap `32011d20` rows; reader is Stage 12).
- `Always.h`: the new `#error ... needs SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE ( SetSkillBuffFactors )` (declared in `X2GUUser.h` under that flag).
- Probe both ways and read `cl /P` for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Write the next phase's kickoff check (`CH1d`) into the Review note.
