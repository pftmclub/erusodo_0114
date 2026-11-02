# June character Chung (Stage 7) - Phase CH2c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2c review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2c done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`, `SUMMON`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH2c - Done (2026-11-02)"
grep -a "Char Chung CH2c" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH2c --out <short scratch dir>
```

## Task

Check CH2c against June (read-only; read the listings, not the decompiles). In order of risk:
- **Artillery Strike (shared with the Missile Shower).** READY FrameMove `32bfe730`: the NPC ids (`0x58b` = 1419 for type 2 behind `IsMyUnit`, `0x35f` = 863 for type 1 with `IsMyUnit` only as a trailing
  argument of June's `CreateNPCReq` that March lacks), the 120 / +300 / `CanDown` / +240 placement, the BFI 452 call arguments (`skip|adjust|level|creator|BFI` = `0|0|1|0|0x1c4`, checklist 7ai). READY EventProcess
  `32bfea90` (0.6 s, `IsEventGamePlayable( false )` = `3201bd60`, the 10 s test: `jnp` after `fcomp` of 10.0 against `fStateTime`). `ArtilleryStrikeControl_EventProcess` `32bfeb80`: that the held path is gated on
  `HaveThisBuff( 0x1a2 )` only, and that the x87 compare chain of the eight directions equals March's order and tests (read every `fcom` / `test ah` pair). STRIKE End `32bff430` (`SetShot( -1 )`,
  `bDirect` true), IDLE StartFuture `32bff3d0` (NPC `+0x8750` / `+0x8d34` / `+0x8e39` = `SetFocusCamera` / `SetFocusCameraForce` / `SetApplyCinematicUI`; confirm the setters write those offsets), FIRE Start
  `32c0e190`, FIRE FrameMove `32bff7f0` (the power rate really is not stored there), END EventProcess `32be3eb0`. The CANCEL state (243) is not registered by June; March's registration stays (never reached).
- **Dual Buster.** `32be2460` (flag `[0]` + `stateChange`, `m_bDualBusterLandingMpSpent` `+0x98d0`, `StateChange( 336 )` before `UpNowMp`, the base `GetActualMPConsume`), the state-336 registration (Init / StartFuture /
  FrameMove / EventProcess), the landing FrameMove `32bf42c0` (the roll order against `UpCannonBallCount`'s own full-hyper test), `32bfbe80` (two times, hyper by `GetRemainHyperModeTime`).
- **Deadly Shot `32c0b160`.** `m_usCBConsumption` really is the word at templet `+0xc`; ability types 5 / 2 = `SA_EFFECTIVE_RATE` / `SA_EFFECTIVE_COUNT`; `GetRandomFloat( 0 ) < rate`; effect `+0x84c` = `m_DamageTime`;
  `0.598` s used for both the Hit_Skill and the shell (one oneshot block in the port, June has two). `32beacc0` (walk keys give WAIT, not WALK, as June).
- **Trick Shot `32bfdd70` / `32bfe4d0`:** June slot `0xf` = the Leg Shot gun handle; the `y + 50` after the offset (x87 leftover, read the `push` shift before `fadd [esp+0x28]`); GUNFIRE's position fallback.
- **Sharpshooter `32c15340`:** swapped skill `0x659c`, `max( 1, level )`, the memo `0xf` value added to the time finalizer, `GetNowHp > 0`, the `NULL == GetUnit()` early return; StateStart / StartFuture anim speed (`+0x88fc` / `+0x8900`).
- **Registration:** the 12 labels, 26223's hyper table (the port copies the normal one; the decompile row shows FM `32bfc860` for `26220,26223` hyper), `ch1m_regcheck.py` (0 problems).
- **The "= March" rows** (funcmap `Char Chung CH2c`): confirm 4 against the listing, e.g. `32be2300`, `32be2db0`, `32bfeb30`, one direction Start.
- Dev free-skill gate: `SpecialAttackLookup` (CH1l3) covers the slot skills; confirm all six cast with no cool time / MP under `/invin 0` + dev gate.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH2c commit>^..<CH2c commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH2c - Review" into the plan doc, update the STATUS rows
(CH2d unblocks) and commit.
