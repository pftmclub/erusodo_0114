# June character Raven (Stage 7) - Phase CH1q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1q review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1q done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE` (`//#define` until CH1 final) + `_RAGE_DEBUG` |
| Log tag | none (nothing runs until `_LIVE` / `_RAGE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1q - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Review (2026-10-27)"        # the Nasod-core twin
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1q - Review (2026-11-04)"    # the worked example
```
Then `june2017_re/char_raven/ch1q_rage_outcome.tsv` (one verdict per `ch0_rage.tsv` row).

## Task

Check CH1q against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`, raw reads with `disasm_range.py`). In order of risk:
- **DoParrying `32c491a0`.** The roll direction at `@32c49431` (`fcompp`, `test ah,0x41`, `jz` skip = spec B > random skips the cost), the `60.0` qword compare
  `@32c494e9` (`test ah,5` / `jp`: rage < 60 -> 0), `cmp 3` / `jge` `@32c49569` (level < 3 -> 0), the `clock()` store inside the paid branch.
- **DoFrameMoveNasodBall `32c50330`.** The three rolls (`32070350`) and what a win does at each: `@32c507da` (follow lost -> `RestoreNasodBall`), `@32c50b15` / `@32c50c3d`
  (instance gone / damage time > 0 -> fall through to `RestoreNasodBall`). The order of the delay / level / follow / parry / attack-end blocks and every reset at the
  attack end. `( rage / 20.0 )` and `m_FrameDataNow.syncData.m_CannonBallCount` (`+0x8160`) for other units. `IsAttackNasodBall()` = delay or attack (`32070eb0`).
- **AddRageGuage `32c41170`**, **CanParryingState `32c3e030`** (`+0x80bc` and `+0x7e1c`), **RestoreNasodBall `32c4fd80`** (the `+0x650` erase = `hitUnitList`).
- **Gauge UI.** `32e6d900` vs the port (gates, 74 / 60 order, no owner, OverUI, the handle reset, the GetShow / hide-dialog logic), `32e681e0` (two passes),
  `32e5e450` = SetShow (vslot 0x20), the vtable `3353348c` overrides (slots 0 / 2 / 8 / 21) and that slot 21 `32e5b470` removes 6 / 5 / 3 / 2 = March's four enum
  names (`cl /EP`: `ADD_EVE_SYSTEM_2014` on, so `PGUWO_ORB_EVE` = 6).
- **"= March" spot-checks:** `CreateNasodBall` `32c4f7e0` (BEED_UP, offsets, scales), the DamageReact parry gate `@32047385`, `SetHold` `31fd29c0`.
- **Routing:** Stage 33 bullet (CEffect::AttackResult tail, NPC-attacker AttackResultByType), CH1 and CH4a kickoff amendments.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE --range <CH1q commit>^..<CH1q commit> --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE_DEBUG --files X2Lib/X2GURaven.cpp
X2Lib/X2GageUI.cpp X2Lib/X2GageUI.h KTDXLIB/Always.h` (numstat 198 / 0, 74 / 0, 7 / 0, 42 / 0; RESULT FAIL only for the commented `#define`), the same for `_RAGE_DEBUG`,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GageUI.cpp`, one-file probes of `X2GURaven.cpp` / `X2GageUI.cpp` with no flag, `_RAGE`,
`_LIVE` + `_RAGE` (0 errors) and `_LIVE` alone (stops at the `#error`). No build if the review changes no code. Then check the CH1 kickoff.
