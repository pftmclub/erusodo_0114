# June buff port - Phase 9a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 9a review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 9a done (`b8bc0a1` code; docs `87c3a3a`, `9d70f0c`, `860804c`, `dd7a7f9`) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 1c, item 5" "Phase 9 — Custom-function buffs" "Phase 9a — Done (2026-10-05)"
```

The RE rows are the `june2017_re/funcmap.tsv` rows tagged "Buff Phase 9a". Most class overrides are not carved in Ghidra: read them with `cd june2017_re/buffre && python rawfn.py <addr>`.

## Task

Review `b8bc0a1` against June. These are the things 9a could have got wrong:

1. **Slot map.** Check the vtable order of the four new virtuals in `CX2CustomFunctionByBuff` (slots 5-8) against `buffre/vt.py CustomFunction` and the four dispatchers (`32275700`, `322757b0`, `32275860`, `3227a1d0`).
   - Check the argument order of slot 6, which is (unit, `const float*`, `DamageData*`).
   - Check that slot 8 runs before the erase.
2. **Call-site placement.**
   - User `AttackResultByType` (`320144d0`).
   - Ara's added base call (`32bceca0 @32bcece3`).
   - NPC `AttackResultByType` (`32773ee0`).
   - NPC DamageReact slot 6 (`327b480a`). Is `fFinalDamage` at that March point the same value June passes (`[esp+0x2c]`)? Is anything between it and the damage number changed in June?
3. **Creator info.**
   - `CX2BuffTemplet::SetFactor`'s creator lookup and power rate (`3236d400`).
   - The behavior SetFactor / packet info (`32e79b00`, `32e73f90`): the not-user polarity, and `m_bIsMesGameUnitNpc` = KBuffFactor `+0x3d`.
4. **Switch `3229e1a0`.** Case 56's user gate. The cool-time argument `32007b10( 0x26, 31fcf430( 0x65a3 ) )` = `SA_READY_TIME_REL` of `SI_SA_CHG_BIGBANG_STREAM`, and that June's ability numbers 5 / 38 equal March's (the skill templet parser `32216320` @32218f0b / @322193d6).
5. **Phantom Sword `32370e10`:**
   - techPoint, fHitAddMP (+0x3f8) and `m_fRateModifier` (+0x51c): confirm the field identities from June's `SimpleInit` (`31f8ee50`) order against March's.
   - Confirm that June really has no `attackType` write anywhere in that function.
6. **Bigbang Stream.**
   - The cool-time compare directions in `3236c6f0` / `3236a040`.
   - The FABS on x and z in `3236dbf0`.
   - The ULTIMATE gate being false in March.
7. **Mark of Commander `3236df00`.** The `( 1 + 3201ad10 ) x 31fd7cc0 x rate` order, and that `3201ad10` really has no March-reachable source.
8. **Pandemonium Fear `3236e2a0`.**
   - The guards (trigger 6) and the damageType cases.
   - `31fd5ef0`'s branches, as ported in `JuneGetTargetHitPos`: the 700 threshold, the strict compares, and the mode-1 fallback.
   - `32293b80`'s true/false cases with no line map.

Hygiene is one command: `python tools/flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range b8bc0a1^..b8bc0a1 --with JUNE_BUFF_DEBUG`. Paste its summary lines. Phase 11's commits also touch `Always.h` and `X2GUUser.cpp`, so do not widen the range to `..HEAD` without `--files`.

Spot-check 2-3 items directly in Ghidra (read-only). If one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`), and hand design decisions back to the user. Write "Phase 9a -- Review" into the plan doc, update STATUS (9a review row), check `kickoffs/june-buff/9b.md`, and commit with explicit pathspecs.

If the review changes code, the play-test is the 9a hand-off's four console checks (`JUNE_BUFF_PORT_PHASES.md`, "Phase 9a — Done"). Otherwise there is nothing to play-test.
