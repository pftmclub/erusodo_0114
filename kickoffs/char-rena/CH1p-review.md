# June character Rena (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1p review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`; CH1p changed no code |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1p - Done"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1p - Review"
grep -n "Rena: CH1p" JUNE2017_CHARACTER_PORT_CHECKLIST.md
```
Plus the "Amended by CH1p" sections of `kickoffs/char-rena/CH2b.md` and `CH3b.md`.

## Task

CH1p found every item already June's and changed no code. Check that verdict against June from the exe bytes (`june2017_re/disasm_range.py`,
`charport_adis.py rena`, `dispsweep.py`) and a fresh `cl /P` of `X2GULire_ElvenRanger.cpp` / `X2GUUser.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`.
Spot-check at least:
- **Pressed index.** `32c6a970` `@32c6aa13` = `m_iNowSpecialAttack = slot + 1`; `32c89a20` `@32c89bac` = `+0x8ac0` = slot + 1; `32ca50b0`'s state change is
  `StateChangeBySkillSlot`; the `_LIVE` `.i` has no March 4 + 4 decode, no `CountUsedSkill`, no `SAKP_*`; `bMarchSlotIndex` excludes `UT_LIRE` only with `_LIVE`.
- **Sweep.** `dispsweep.py 8ac0 8b58 88bc` in Rena's range `32c6a8d0-32cb6c6d` = the two writers only; no `+0x8b58` use. The `E8` scan finds
  `31fce710` only at `@32c84fd9`, `@32c8545b`, `@32c8552b`, `@32c855fb`, and `31fd7500` only at `@32c84ad9`. Confirm March's `LESI_A_LWS_BACK_KICK_EventProcess`
  tests no key, so nothing misreads before CH2b.
- **Routes.** The CH2b / CH3b amendments match the listings (`32c84ea0`; `32c85430` / `32c85500` / `32c855d0`: key -> next attack, else left / right or
  the time limit -> MP text and `LESI_WAIT`).
- **Hooks / keys.** `InitWaitStateChange` / `InitBoundingStates` once each in the `_LIVE` `.i`; `IMMUNE_TO_SUPER_ARMOR_DOWN` is not read by `32ca81b0`
  (engine `31fdaaa0`, backlog Stage 2).

Hygiene: `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, one-file `/Zs` probes of both `.cpp` off and with `_LIVE`.
Fix clear errors in place (bytepatch only). Write "CH1p - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1q.md`, commit with
explicit pathspecs.
