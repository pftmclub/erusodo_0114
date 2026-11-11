# June character Ara (Stage 7) - Phase CH1k review (covers CH1h-CH1k)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1k review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1h, CH1h2, CH1i, CH1j and CH1k done (their Done notes and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined), `_FP` (`//#define`), `_SKILL` (`//#define`), `_SPECTER` (`//#define`), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (defined) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1h - Done" "CH1h2 - Done" "CH1i - Done" "CH1j - Done" "CH1k - Done"
awk -F'\t' '$7=="CH1h" || $7=="CH1h2" || $7=="CH1i" || $7=="CH1j" || $7=="CH1k"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review the functions of CH1h-CH1k against June (`python june2017_re/charport_adis.py ara <va>...`; the listing carries the `StateChange` arguments and the `fcomp` / `test ah` direction; `je` after `cmp byte, 1` jumps when the byte IS 1). Spot-check at least:
- **CH1k lookup `32bcfaa0`:** the order of the checks (quick-slot wait, `CommonSpecialAttackEventProcess`, templet mismatch text `0x8bae`, `m_iChosenSkillID` set before `CheckSkillUseCondition`, the two ban checks, the riding-key wait, `GetUnit()` null test), Conversion Health's three-part test (`GetTranscendenceMemoSkillID` == 0, same-or-upgrade of 27210, `CanUseConversionHealth`), the five-slots-a-bank decode (`>= 5`; the base `CommonSpecialAttackEventProcess` hands Ara March's index until CH1p), the chain block (type 2, `ResetChaining` otherwise, a missing slot reads cool time 0 and the skill's own level), the three `+0x9621` reads under `_FP`, `+0x9626` stored at `@32bd01c2`, the dev gate around the cool time / MP / force power / hyper stone block, the Moonlight Slash tail.
- **CH1k use `32ba6610` and cooldown `32bb1ff0`:** `+0x970c` / `+0x971c` stores, the chain branch's state choice, the no-stage-change branch (`SpecialAttackCooldown`, memo refund, `m_fChosenSkillMp = 0`), `StateChangeLinkFlowerSlash` before the slot state, Open Mind's block (`+0x9690` is `m_fUseForcePowerTime`, `SA_FP_UP_ABS` is ability 191, `UpForcePower` = June's inlined `+0x9644` conversion), the call order `OnSkillUse` then `ApplySkillCoolTimeRateByBuff`.
- **CH1k `32bc84b0`:** both action cases (Reflection 0x1a, heal 0x27), the `SetBuffFactorToGameUnitByBuffFactorID` argument order ( BFI, creator, level, adjust, skip ), `UseForcePower` double count against March's own counter, who writes action 0x27 (not found: routed).
- **CH1k small ones:** `32ba44e0` jump tables (checked against the exe bytes), `32b95560`, `32bab090` / `32ba62c0` entry layout (flag bytes +0 / +1 / +2, ints +4 / +8) against `32bab160` and `32bc6aa0`, `32bab220` state ids, the `X2GUUser` `ReservedSkillStart` chain.
- **CH1h-CH1j:** the checks their own Done notes name; the `m_usGain` / `m_bEnabled` / `m_bReady` / `m_iGainHitID` member renames of CH1k against CH1h's `MarkSkillFPGainHit` and CH1j's `CleanupSkillFPGainTable`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE SERV_IRUHADEV_JUNE_CHAR_ARA_FP SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER --range ae64b2a8^..<CH1k commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h`; one-file `/Zs` probes of `X2GUAra.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, and with `_LIVE` + `_FP` + `_SPECTER`. CH1k left no `_DEBUG` define (CH1h-CH1j none either). Fix clear transcription errors in place (bytepatch only). Write "CH1k - Review" into the plan doc, update the STATUS rows (CH1k done, CH1m ready), check `kickoffs/char-ara/CH1m.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1m` (Opus 5.5 high).
