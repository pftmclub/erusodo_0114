# June character Rena (Stage 7) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1d review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; two `new` helpers + two enum members), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define`; the NF lines inside the bodies) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1d - Done"
awk -F'\t' '$7=="CH1d"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1d - Review"
```

## Task

Review the 38 CH1d rows against June (`python june2017_re/charport_adis.py rena --phase CH1d --out <dir>`; the listing of an uncarved neighbour follows the function, stop at the first `int3`). Spot-check at least:
- **The cancel chain** in every EventProcess: skill (`+0x8c90`, foot test inside the March macro), walk (`+0x8c98`), back-walk (`+0x8c94`), dash (`+0x8c9c`), each with the foot test (`RENA_GROUND_*`). Rena's order is walk before back-walk (Raven's is the other way round). No Z / X cancel (`+0x8c8c`) anywhere.
- **The flagged follow-ups** (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`): change test `m_fEventTime[i].stateChange < t || animation end`, window `t < keyInputEnd && keyInputStart <= t && cond`; flag / event-time index pairs and the order in Z1 / Z2 / Z3 / X1 / X2 / X3; the class tests (`+0x38`: 12 / 13 / 24 / 102 / 103 / 114).
- **X1 / X2 ids:** X1 goes to June id 253 (`LESI_RCR_COMBO_XXZ`), X2 to id 257 (the same script table registered a second time; named `LESI_RCR_COMBO_XXZ_FROM_X2`): both with window classes 12 or 102 and `oneZ`.
- **The arrow functions** (Z4a, X1, X2, X3, X4, X4 up, X4 down): timers (0.3 / 0.24 / 0.28 / 0.38 + 0.43 / 0.431 x3), MP 9 / 4 / 6 / 8 / 9 / 9 / 9 through `GetArrowMpConsumption`, arrow names by class, X3's `degree.z += 2` / `posR.y += 10, degree.z -= 2`, the NF line positions (Z4a non-Night-Watcher branch; X4 / X4 up / X4 down), the three `ARROW_DEGREE{1,2,3}_Z` reads, not-enough-MP arguments, and the `GetMainEffect` max-life tail.
- **The two helpers** `32c85330` (`SA_MP_CONSUMPTION` 143, NF term 0.1 per point, clamp at 0) and `32c7fe20` (`SA_PROJECTILE_RANGE_REL` 31); buff 536, skill 23210, `GetSkillLevel( id, true, 0, true )`, `GetEquippedActiveSkillTemplet( id, true )`.
- **Dropped March writes:** June's Z1 and X3 FrameMoveFuture are bare `CommonFrameMoveFuture` thunks (March zeroes `nowSpeed` first).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1d commit>^..<CH1d commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1d - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1d2.md`, commit with explicit pathspecs.
