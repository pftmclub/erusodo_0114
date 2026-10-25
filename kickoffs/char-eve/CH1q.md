# June character Eve (Stage 7) - Phase CH1q - Nasod Core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1q` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1p review (decision 1 answered 2026-10-25: (a), March's dormant code as the base) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (+ `..._NCORE_DEBUG`); it runs only with `_LIVE` |
| Log tag | `[JUNE_CHAR]` (`EVE NCORE`), seen from CH1 final |
| Review | `kickoffs/char-eve/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Context" "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Open decisions (for the user; asked at the end of planning)"
```
Then CH0's Done note and `june2017_re/char_eve/ch0_nasod_core.tsv`.

## Task

Port the Nasod Core as decision 1 says: maneuver core, maneuver gauge, `ENSI_PARRYING_SMALL / _PARRYING / _PARRYING_BLINK`, `ENSI_BERSERK_MODE`, `DTT_EVE_MANEUVER_ATTACK`, the gauge-UI orb (`CX2EveMyGageUI`, `PGUWO_ORB_EVE`), the `X2GUUser` virtuals (`AddManeuverGauge`, `IsWaitManeuverCore`, `SetAttackManeuverCore`), the NPC side in `X2GUNPC.cpp`. Every function June has is June's whole body; what June removed goes; the parrying states register through CH1m's June registration.
- Decision (a), the user's answer: define `ADD_EVE_SYSTEM_2014` / `FIX_ADD_EVE_SYSTEM_2014` only inside `#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` in `Always.h`; prove with `cl /P` what else the two macros switch on (other classes, `X2GUUser` layout, the gauge manager), then replace each body that differs from June under the NCORE flag.
- Over ~45 functions in the map: stop after the first half and write `CH1q2`.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). Done gate: the build passes; the Done note lists every `ch0_nasod_core.tsv` row with its outcome (June body / March = June / dropped). The play-test comes with CH1 final.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
