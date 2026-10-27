# June character Eve (Stage 7) - Phase CH2c2 review - Skills: Electra (26) Sweep Rolling and Giga Stream

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2c2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2c2 done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (no debug flag in CH2c2) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2c2 - Done (2026-10-28, Sonnet 5.5)"
```
Then the 26 `Char Eve CH2c2` rows at the end of `june2017_re/funcmap.tsv` and `june2017_re/char_eve/ch0_skill.tsv` lines 35 / 74 (the two cases).

## Task

Review the CH2c2 transcriptions against the raw listings (`python june2017_re/charport_adis.py eve <va>`). Where the port could be wrong:
- **`329a1b90`** (`June_CreateGigaPlane`): the order is plane (perFrameSimulation, scale ( 1, s, s ), handle, max life, alpha tag), then the five crystals ( handle, `SetPos` from the plane's `DummyNN`, tag ), then ten `CreateSequence` ( `P02`, `P03` per dummy 02..06 ) each tagged, all ten behind `particle system != NULL && plane != NULL`. Compare the arithmetic head with March's `CreateGigaPlane` ( scale by the unit, flattened direction, axis angle `pi / 180 * z`, plane from point and normal ). It calls the March `DestroyGigaPlaneAndCrystalEffect` ( `32991ac0`, funcmap CH1l4 ).
- **`329a7250`** (Giga Stream FrameMove): the 0.5 s branch picks the three argument sets by the sign of `m_fPlaneZAngle` and `GetEnablePlane()` ( `+0x9830` ): 230 / 199.24, 230 / 60.76, 249.56 / 130, life 2.5, scale 1; the 0.9 s laser ( hyper when `GetRemainHyperModeTime() > 0` ) goes through `June_AddLaserToSystem` with 0.3 s and the `_NEXT` name three times; no handle is kept.
- **`32993c40`** (CHARGE Start): the effect handle goes to `+0x99c0` before `June_AddEffectToSystem`; the initial scale is `GetVec3ScaleByUnit()`; both memo tests are `31fca9e0` ( stub, false ).
- **`3297dcc0`** (CHARGE EventProcess): the key test is `31fce8b0( input, future state skill id )` ( `+0x7cb0` is the future state pointer ); FIRE on release, END on `EventTimer( 5.0, true )`.
- **`32994540`** (FIRE Start): `reActType +0x224 = 2`, velocity ( 400, 0, 0 ), now life from `+0x99ec`; June has no back speed.
- **`329a6f20`** (READY FrameMove): `June_CreatePlane( 0, 7.0, 340, 130, "EveProtoPuritySweepParadePlane" )` at 0.4 s.
- **Identical-listing claims:** `329737c0` = `329733b0`, `3297de10` = `3297d3f0`, `329946f0` = `329936e0` ( the normalised mnemonic and operand sequences were equal; re-diff one ).
- **Registration** ( `329c8a00` @599 / @740, `ch0_skill.tsv` ): the Sweep Rolling READY hyper table has no Init; the follow-up states 0xd3-0xd5 take the callbacks of the row ( CHARGE Start / FrameMove / EventProcess / End = `32993c40` / `32994230` / `3297dcc0` / `329944b0`, and so on ); Giga Stream's hyper table has no Init.
- **`3299b2e0`** is not CH2c2's ( its caller is `329a79c0` @`329a7b25`, CH4b ): confirm in Ghidra and check the CH4b kickoff names it.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2c2 commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2c2 - Review" into the plan doc, update the CH2c2 STATUS.md row, check the next kickoff (`CH3a0`), and commit.
