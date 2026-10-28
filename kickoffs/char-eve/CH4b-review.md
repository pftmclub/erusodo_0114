# June character Eve (Stage 7) - Phase CH4b review - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4b review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH4b done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ `SERV_IRUHADEV_JUNE_CHAR_EVE_OVERRIDE_DEBUG`, retired here), `SERV_IRUHADEV_JUNE_SKILL_MP_DECREASE_RATE` |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH4b - Done (2026-10-28, Opus 5.5)"
```
Then the `(CH4b)` rows at the end of `june2017_re/funcmap.tsv` (CH4b added no JUNE_BUGS row).

## Task

Review the CH4b port against the listings (Ghidra for `329bc560`, `329cf6d0`, `3234d840`, `3275a120`; `python june2017_re/funcdis.py --func <va>` or `disasm_range.py` for the uncarved `329a79c0`, `32979420`, `3299b2e0`, `32028af0`, `3275f0a0`). Where it could be wrong:
- **`329bc560`.** The x87 order of the Open Mind rate `1 + n x ( v - 1 )` (`@329bc5cd-329bc5eb`), `n` = `June_GetFormationFactor` (`+0x97e4` / `+0x97f0`), DD `+0x3c` = `damage.fMagic`; vslot `0x238` argument order ( out, 25420, index 1, flag 1 ) and `PushBuffFactor( ptr, true )`; the Tesla Shock ids and `32007ce0`'s swap argument; `+0x3f0` = `fForceDownValue`.
- **`329a79c0`.** Both byte tables (`329a7bf8`: 200-206 with 204 out; `329a7c14`: 25015 / 25020 / 25030 / 25057 / 25062 -> case 0, 25201 / 25214 -> 1, 25207 / 25219 -> 2, 25235 -> 3); `+0x867c +0x38` = `GetClass()`; `+0x9c74`, `+0x97b0`; the tail `31fca8e0( id, 1 )` and `3234d840` on `g_pData +0x68 +0x90`.
- **`32979420`.** The -1.0 returns skip the clamp and the 25420 term; 25221 / 25010 replace the cool time ( SA 0x94 / 0x95 of 25416 ); the arg-3 claim (every caller March has passes 0; `31fd8ff0 @31fd9069` is the only `true`).
- **`3299b2e0`.** NPC id set, the HP test against March's `GetNowHp`, AI type / owner UID.
- **`3275a120`.** NPC vslot `0x414` = state time `+0x8408` read as March `GetStateTime()` (confirm `+0x8408` is the Now frame data's `fStateTime`), `x 10.0` then truncation, the strict `<` of `fcompp`; the registration point after `InitEtc` in `CX2GUNPC::Init`.
- **`3234d840`.** `32931920` / `32931950` really are `IsHenirDungeon` / `IsSecretDungeon`; the item id 60008066.
- **Hand-off check:** the log lines the CH4b hand-off lists.

Retire `SERV_IRUHADEV_JUNE_CHAR_EVE_OVERRIDE_DEBUG` (define and lines). Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_PASSIVE --with JUNE_CHAR_EVE_OVERRIDE_DEBUG --range <first CH4b commit>^..HEAD` and `flagcheck.py JUNE_SKILL_MP_DECREASE_RATE --range ...`; `enumscan.py --check X2Lib/X2GuEve.cpp X2Lib/X2GUNPC.cpp X2Lib/X2Unit.cpp` (no row in the CH4b lines). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. Next phase kickoff `CH4c.md` exists.
