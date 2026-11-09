# June character Rena (Stage 7) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1e review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 10 enum members, 7 new functions, the chain macros' guard), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define`; nothing in CH1e) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1e - Done"
awk -F'\t' '$7=="CH1e"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1e - Review"
```

## Task

Review the 29 CH1e rows against June (`python june2017_re/charport_adis.py rena --phase CH1e --out <dir>`; stop each listing at its first `int3`). Spot-check at least:
- **Follow-up flags and targets** (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`; event time `[0]` is `+0x8c10` / `+0x8c14` / `+0x8c18`):
  - Combat Ranger dash Z1 -> `LESI_COMBAT_RANGER_DASH_COMBO_Z2` (June id 185), dash Z2 -> `LESI_DASH_COMBO_Z2` (186); both write `m_fCanNotAttackTime` 0.2 on the animation end on a line; direction clear = flag 0 only.
  - Dash-jump Z1 -> Z2 (`0xc0`) -> Z3 (`0xc1`); landing `LESI_DASH_JUMP_ATTACK_Z_LANDING` (`0xc2`) first. `LESI_LGA_DASH_JUMP_Z` (June id 213, `32c78e40`): X (`oneX`) -> `LESI_LGA_DASH_JUMP_ZX` with the `fChangeTiming` 0.25 / window 100.0 pair, no lower bound on the window. The class map named the Combat Ranger Z1 function for this row; CH1e corrected it.
  - Double-jump attack Z: flag 0 from `oneZ` inside `( keyInputStart, keyInputEnd )` (both strict), on a line -> Z up when the flag is set else landing; off a line at the animation end `y -= LINE_RADIUS / 2`.
  - Wind Sneaker `ZZfrontZ` (`32c77370`, id 205): flag 0 -> `ZZfrontZZ` (`0xce`). **`ZZfrontZZ` (`32c77710`, 206): the animation end goes to id 208 (`ZZfrontZZZ_Landing`), no flag. `ZZfrontZZZ_Landing` (`32c77c50`, 208): the animation end -> wait, flag 0 -> id 207 (`ZZfrontZZZ`). `ZZfrontZZZ` (`32c779c0`, 207): wait at the animation end on a line.** Check the three ids against the listings; the names are the script tables'.
  - `DASH_XZ` (209): animation end on a line -> `USI_DASH_JUMP_LANDING`, flag 0 -> `DASH_XZZ` (`0xd2`); `DASH_XZZ`: end -> wait on a line, else jump-down.
- **The seven `new` functions** (`32c7feb0` `32c80200` `32c80480` `32c807c0` `32c80a40` `32c80c70` `32c80e80`): the targets are the second registration's ids (258 / 259 / 260 / 265 / 266; `LESI_RCR_COMBO_XXZ_EventProcess` is shared by 253 and 257 and always goes to 258), the foot test and `y - 45` in the three X-X-Z listings, `START`'s `IsAnimationEnd` before the foot test (`JUNE_BUGS` #564).
- **The cancel chain** in every listing that has one: skill (`+0x8c90`), walk (`+0x8c98`), back-walk (`+0x8c94`), dash (`+0x8c9c`), each with the foot test; no Z / X cancel. `ZZfrontZZ` / `ZZfrontZZZ` FrameMoveFuture: the `EventTimerOneshot( 0.3f )` / `( 0.54f )` result is dropped (March set `nowSpeed.x`).
- **Low Kick**: Init's seven preloads (`JUNE_BUGS` #563), FrameMove thunk, EventProcess (= March, 0.66 s).
- **Enum**: ten `_CORE` members (254-256, 258-260, 262, 263, 265, 266) beside the earlier 253 / 257 / 261 / 264 (`ch0_state_enum_names.tsv`; CH1m maps). **The chain macros' guard** (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`, `RENA_GROUND_*`) moved from `_LIVE` to `_CORE` because the `new` functions need them (the two deleted lines in flagcheck).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1e commit>^..<CH1e commit>` (FAIL only for the commented-out `#define`; 2 deleted lines are the guard); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1e - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1f.md`, commit with explicit pathspecs.
