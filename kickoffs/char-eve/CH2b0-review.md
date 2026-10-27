# June character Eve (Stage 7) - Phase CH2b0 review - Helper: the unit-cone collector

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b0 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2b0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | none (inert until CH2b calls it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b0 - Done (2026-10-27, Opus 5.5)"
```
Then the `32009180 (CH2b0)` and `32294e30 (C3c)` rows of `june2017_re/funcmap.tsv` and JUNE_BUGS 450.

## Task

Review `CX2GUUser::JuneCollectEnemyUnitsInCone` (`X2GUUser.cpp`) against June `32009180` (`@32009180-3200977d`, RET 0x20; Ghidra `disassemble_function`). Things this phase could get wrong:
- **Arguments:** stack `+0xa0` out vector, `+0xa4` count (never read), `+0xa8` max distance, `+0xac` / `+0xb0` min / max angle, `+0xb4` facing byte, `+0xb8` cone degrees, `+0xbc` invincible-skip byte.
- **Skip order and senses:** team equal skips (no team argument, unlike `32294e30`); `GUSI_DIE`; the gage test `fldz / fcomp / test ah,1` = keep only `0 < HP`; `GetInvincible` (vslot 0x180) called before the byte test.
- **Aim point and facing:** target vslot 0x3e4 ( out, my vslot 0x138( 0 ), L"" ) = `JuneGetLockOnAimPos`; `32293b80` vs vslot 0x170.
- **Yaw cone:** 0.0 / 360.0 / 180.0 / 0.5 are qwords (`charport_consts.py --fn 32009180`); `test ah,5 / jp` keeps a NaN or `>= 0` yaw as is; the gap wraps at `> 180` (`test ah,0x41`); both bounds `-h` / `h`, NaN passes.
- **Distance:** `( x*x + y*y ) + z*z` stored as float, `_CIsqrt`, minus the NPC size, kept on `<=` (NaN refuses); `std::sort` vs June `322acc40`.
- **Home:** a `CX2GUUser` member (June's address range is the user class); `friend class CX2GUUser` in `CEffect` for the aim point.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2b0 commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GUUser.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2b0 - Review" into the plan doc, update the CH2b0 STATUS.md row, check the next kickoff (`CH2b`), and commit.
