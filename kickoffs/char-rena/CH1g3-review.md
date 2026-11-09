# June character Rena (Stage 7) - Phase CH1g3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1g3 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g3 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; 22 `inside` bodies), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 5 new `LESI_RNW_DASH_JUMP_XZ_*` / `XXZ_*` functions, their 5 declarations, 3 enum members) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1g3 - Done"
awk -F'\t' '$7=="CH1g3"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Review"
```

## Task

Review the 27 CH1g3 rows against June (`python june2017_re/charport_adis.py rena --phase CH1g3 --out <dir>`; every listing runs past its function: stop at the first `ret` / `jmp edx` that is followed by `int3`; `32ca0f20` runs into `32ca0f80`). Spot-check at least:
- **The 21 accel-dash / dash-jump rows the Done note calls `= March`:** read each listing against the March body kept in `#else` (the dash speed is the inlined `GetDashSpeed` = `m_fRunJumpSpeedRate` x the decoded triple at `+0x49c4`; `2.25` / `2.45` / `2.35` / `1.5` / `0.13` / `1600` as written; `CanDown` June's 9-argument call = March's 4-argument forwarder `bDownRadius` true, `bIsMonster` false, `bOnLine` true).
- **`LESI_RNW_ACCEL_DASH_JUMP_ATTACK_Z_EventProcess` `32c75bc0` (the one `differs` row):** foot -> LANDING; flag 0 / event time 0: change time passed or animation end -> `ACCEL_DASH_JUMP_ZZ` (`0xeb`) and the flag cleared; else inside `keyInputStart <= t < keyInputEnd` with `oneZ` sets the flag. March's live arm is the 0.25 s window; its `ADD_RENA_SYSTEM` arm lacks the animation end.
- **The five new rows:** `32c82850` (animation end -> `0x119` = id 281 `XXZ_LOOP`, foot -> `0x11a` = id 282 `XXZ_END`), `32c828a0` (foot -> `0x11a`; the skill branch of `SKILL_CANCEL_AFTER` is folded away by the compiler in June because the foot is known false, which the C++ macro form reproduces; then hyper, walk `+0x8c98`, back-walk `+0x8c94`, dash `+0x8c9c`), `32c82ab0` (animation end -> wait, the full cancel chain with the skill branch), `32ca7730` / `32ca0f20` (`CreateTrapBlade` / `DeleteTrapBlade` inlined; no `SetNoDetonation` here).
- **Enum:** `LESI_RNW_DASH_JUMP_XZ_END` (279), `_XXZ_LOOP` (281), `_XXZ_END` (282) added under `_CORE`; id 278 (the XZ chain's loop slot, registered with the XXZ_LOOP script table; `LESI_RNW_DASH_JUMP_XZ_LOOP` is absent from June's exe) has no member: CH1m decides how to register it.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1g3 commit>^..<CH1g3 commit>` ( FAIL only for the commented-out `#define` ); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1g3 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1h.md`, commit with explicit pathspecs.
