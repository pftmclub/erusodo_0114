# June character Eve (Stage 7) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1j review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1j - Done"
awk -F'	' '$6=="CH1j"' june2017_re/char_eve/ch0_class_map.tsv      # 30 rows
grep -a "Char Eve CH1j" june2017_re/funcmap.tsv                      # 29 rows (329cd320 keeps its CH1e row)
```

The members are in the tenth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (heading comment "Electra line"); declarations at the end of the class, inside `EveElectraSystem` (after `June_CreatePlane`) and two fields at the end of `EveElectraLaserData` in `X2Lib/X2GuEve.h`. No new enumerator.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1j --out <scratchpad dir>` (30 files) and a Ghidra decompile where a function object exists (`32980000`, `329a4c30`, `329a5430`, `32992440` have one; `3297ccf0` / `3297d050` / `32972b80` / `32972ed0` do not). You decide:

- **`32980000` AddLaserToSystem** against March's `AddLaserToSystem` (`X2GuEve.cpp` ~25650): 14 arguments (pEffect, TraceBone, pos, dir, velocity, propagate, width, three names, `bApply`, radius, **two trailing wstrings**), the field order (handle push, pos, dir, velocity, propagate, TraceBone, 0, 0, `bApply`, width, init rotate from the main effect `+0x5a8`, names, fire count 0, the two extra names), the three `EVE_PROTO_PURITY_LASER` defaults (the two extra have none), list push, `SetLaserGroupID( m_iLaserGroupNum )` with the 1..2 wrap, `3297a8e0` last. Check the data layout claim: June's `EveElectraLaserData` = March's + a 4-byte pad at `+0xc` + wstrings at `+0xc8` / `+0xe4` (ctor `3297b510`, copy ctor `329cdd90`). **Decide the names `m_wstrJuneName4` / `m_wstrJuneName5`**: find the reader (laser `OnFrameMove`, in a later CH1 group) and rename if it shows what they are.
- **Hyper arm in `329a4c30` / `329a5430`**: June tests `this+0xc04` (gage) non-null and the protected float at gage `+0xe8` above 0 (`GetRemainHyperModeTime() > 0.0f`); strings `EVE_COMBO_ZZZFZ_LASER_HYPER` + `EVE_PROTO_PURITY_LASER_HYPER` x3, `EVE_COMBO_XXZ_LASER_HYPER` + `EVE_COMBO_XXZ_LASER_NEXT_HYPER` x3; the normal arm passes L"" x3 (ZZZ front Z) / `EVE_COMBO_XXZ_LASER_NEXT` x3 (XXZ); both pass `true, 2.0, L"", L""`. The handle is stored **after** AddLaser and only when the effect exists (March: before, unchecked).
- **Plane / timing constants**: ZZZ front Z `CreatePlane( 0, 2.0, 240, 130 )` at 0.02, angle copy at 0.3666, laser at 0.4, MP drain `13.3333 * dt` in (0.4, 1.23333), rotate `78.3 * dt` in [0.4, 1.0) with offsets (X -8, Y 15); XXZ `CreatePlane( angle, 2.4, 220, 130 )`, 0.3, 0.23333, rotate `41.904 * dt` in [0.83333, 1.93333) with Y 2, END effect set at 1.76666, drain in (0.23333, 1.8666).
- **EventProcess x4** (`32972b80` `32972ed0` `3297ccf0` `3297d050`): no Ghidra object; read the listing: branch order, the dash Z-Z macro arm (`+0x7d85`, `+0x8c10` / `14` / `18`, `0xd2`), the four June cancels with the on-line test.
- **Dash Z-Z / Z-Z-Z**: `32991e10` (7c tag on the blade mesh, handle `+0x99f4`, effect handle `+0x99d0`), `32991f50`, `32992190`, `32992210` (handles `+0x99cc` / `+0x99d0`, blink), `32992440` (eight timers, `FlushMp( 10, 0 )`, global times), `32992f60`.
- **Helpers**: `3297a8e0` (= March `LaserAttackLineSet`), `32991940` (= March `DeleteLaserFromSystem`), `32990c50` (= March `Delete`), `329cd320` (the `CreateInstanceHandle` wrapper, March's call).
- **JUNE_BUGS**: none was written; say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1j commit>^..<CH1j commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1j - Review" into the plan doc under the Done note, update the CH1j row in `STATUS.md`, check that `kickoffs/char-eve/CH1j20.md` (helpers: slot key-held / slot page) still matches, and commit with explicit pathspecs.
