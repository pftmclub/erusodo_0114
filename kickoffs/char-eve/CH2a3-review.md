# June character Eve (Stage 7) - Phase CH2a3 review - Mega Electronball FrameMove

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2a3 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2a3 done (its Done note and commit) and the user's play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`; temporary `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG` (this review retires it) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL registered ENSI_SI_A_EN_MEGA_ELECTRONBALL`, `EVE SKILL MEGA ...`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2a3 - Done (2026-10-27, Opus 5.5)"
```
Then the `329b1e20` row of `june2017_re/funcmap.tsv` and JUNE_BUGS 449.

## Task

Review CH2a3 against June `329b1e20` (`@329b1e20-329b2e0f`; `python june2017_re/charport_adis.py eve 329b1e20`, the listing runs on into `329b2e10`, Oberon Guard's FrameMove, which is CH2b's) and the case in `329c8a00` `@329c9225-329c92e9`. Things this phase could get wrong:
- **Registration:** slots `+0x18` Init, `+0x20` StartFuture, `+0x28` Start, `+0x30` FMF, `+0x38` FM, `+0x48` EP, `+0x50` EndFuture; the hyper table `ENSI_SI_A_EN_MEGA_ELECTRONBALL_HYPER` copies all seven, Init included; the case labels 25207 / 25219 from the jump table.
- **The shot gate:** `EventTimerOneshot( 0.23 ) || ( EventTimerOneshot( 0.48 ) && +0x978d )`; the 0.22 plane; `GetUnit()` null returns without `CommonFrameMove`.
- **MP order:** templet MP at `max( 1, level )`, 0 once anim `+0x214` > 0.3, class 19 / 109 x `+0x97b0`, then memo rate (vslot 0x75c of vslot 0x524), BBT 63 rate of 25207 (`32039a50`), `( 1 + 32358970( 0xb ) x 0.01 )` (0 stand-in); `FlushMp || +0x978d`; failure = `CreateNotEnoughMPEffect` at `Dummy2_Lhand`.
- **The rank:** only size / range scale (x1.25 / x1.125 rank 2, x1.5 / x1.25 rank 3); the power does not.
- **Open Mind** (`31fceab0( 25419 )` > 0, formation factor 0 / 1 / 3, `32007b10( 66, 25419 )` x MP) and the **upgrade proc** (`+0x178` of the running skill's templet, `32007c40( 5, skill, 1, &f )` >= `GetRandomFloat( 0 )`): argument order and the compare direction.
- **The balls:** `+0x97c0` x `GetPowerRate`; BIG = `+0x97bc > GetRandomFloat( 29 )` (main) / `( count + 29 )` (watch), x1.5 power and size; the PvP x0.33 after the main ball; the watch ball at `GetPos + 150 y`, the watch's rotation and `GetLandPos().y`; `+0xacd` = `SetInheritForceDownRate( true )`; `_Filtered` x4; `Eve_FireShot1.ogg`.
- **The two NULL guards** (templet, created ball) are the only deviations; JUNE_BUGS 449 (watch lights at the hand) is kept.
- Play-test: unit 22 `weevwe` casts Mega Electronball Enhanced (slot 7, B) at each charge rank, with and without a Dimension Link sentry up; the `EVE SKILL MEGA` lines show rank / scale / range / big / watches / second ball; no Motion Error / KEM line.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2a3 commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Retire `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG` (the `Always.h` block and the two `#ifdef` log blocks in `June_ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove`), then `tools/build.py --deploy`. Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag; hand design decisions back to the user. Write "CH2a3 - Review" into the plan doc, update the CH2a3 STATUS.md row, check the next kickoff (`CH2b0`).
