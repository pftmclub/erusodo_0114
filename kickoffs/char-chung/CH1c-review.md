# June character Chung (Stage 7) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1c review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1c - Done (2026-10-31)"
awk -F'\t' '$7=="CH1c"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1c is two functions, so read both in full against June (`python june2017_re/charport_adis.py chung --phase CH1c --out <scratch dir>`):
- `CommonHyperModeFrameMove` `32c09b60` (`inside`): the three `EventTimer` tests (`t > +0x24c && EventTimer` = March's active `EventTimerOneshot`), the t1 branch (`ShowMinorParticleHyperModeChange` only), the t2 order (awakening begin / present / play, Shock, `UpDownCrashCamera( 30, 0.4 )`, `ClearScreen`, Trace only when no effect set, `ApplyHyperModeBuff` vslot 0xc0, the three sounds when `bSound_`), the empty t3 branch (JUNE_BUGS 490), the `CommonFrameMove` vslot 0x7cc tail. The static-guard call in the prologue (`call [344c0044]`, also in Aisha's `32ce12c0`) is a compiler artefact and is not ported. Confirm the world colour (0.1333 x3, 1.0) is `0xff222222` and the inlined `ClearScreen` is March's.
- `RidingHyperModeFrameMove` `32c113e0` (`inside`): `( 0, 0.11, 0.1, true )` is March's call.
- The awakening stand-ins (`_CONST_CHUNG_CH1c_`, `ChungAwakeningEffect*`): same shape as Aisha's and Eve's; say whether Stage 6 (transcendence) should replace all three classes' copies with one `CX2GUUser` member when it ports `31fd66b0` (funcmap row 4588).
- The user-side check the kickoff asked for: the riding and pepper-run callbacks are `CX2GUUser` bodies that no Aisha / Eve flag touches (March's, as Eve's CH1c review found); Chung overrides none of vslots `0x58c`-`0x5b4` / `0x610`-`0x624`; dance (3) and phoenix-run (1) callbacks and `CSI_RIDING_SPECIFIC` / `_ATTACK_X_READY(_2)` / `_END` have no March function and stay out of CH1m's registration (owners: Stage 10, PvP, Stage 17). Confirm the CH1m kickoff says so.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1c commit>^..<CH1c commit>` (FAIL only because the `#define` is commented out); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with and without `/D SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`june2017_re/census.py` `cl_cmd`, see the CH1a review). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1c - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1d0.md` (Opus helpers `31fd4290` / `31fd4340`), commit with explicit pathspecs.
