# June character Eve (Stage 7) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1e review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1e - Done"
awk -F'	' '$6=="CH1e"' june2017_re/char_eve/ch0_class_map.tsv      # 34 rows (329cc500 moved to CH1l; 32978c60 now carries its March pair)
grep -a "Char Eve CH1e" june2017_re/funcmap.tsv                      # 34 PORTED rows + 32007ce0 PORTED + 2 "not ported here"
```

The 33 state members, `June_StateChangeBoostAttackIfPossible` and the helper `June_TryGetEquippedSkillAbilityValue` are in the fifth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (it starts with `JUNE_EVE_TEMPLET_CLASS_ID`); declarations at the end of the class in `X2Lib/X2GuEve.h`, with 9 new enumerators and 2 new fields (`m_pJuneDashXFireBall` +0x9c20, `m_pJuneDashXTargetUnit` +0x9c24).

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1e --out <scratchpad dir>` and a Ghidra decompile of each address (`decompile_function`) give the facts. You decide:

- **The event-flag EventProcess functions** (DASH_COMBO_Z / X, DASH_JUMP_COMBO_Z / ZZ / X): flag order, `m_fEventTime[ i ]` index (the new arms use time 0, DASH_COMBO_X's second arm time 1), the key and class condition of every arm, and the class form per site (templet `+0xaf0 -> +4` vs unit data `+0x38`: the new flag arms of DASH_COMBO_X, DASH_JUMP_COMBO_Z / ZZ and the Nemesis test of DASH_JUMP_COMBO_X read unit data; everything else, and DASH_COMBO_Z's Architecture / Empress test, reads the templet). The jump table of `3296ea70` / `3296cf70` (classes 18 19 108 109 116 / 18 108, 26 116, 109) is in the Done note.
- **DASH_COMBO_Z FrameMove `32985bb0`**: Empress takes `EVE_AT_DASH_COMBO_Z` (March: `EVE_EP_...`) scaled by `+0x97c0` with force-down `+0x97c4`; `329cd320` is `CreateInstanceHandle`; the two particle tags sit inside the null check after `SetAxisAngle`.
- **DASH_COMBO_X / DASH_JUMP_COMBO_X FrameMoves** (`329b0c10`, `329b13f0`): `FlushMp( 4 * +0x97b0 )` (March 8), the BIG pick (`GetRandomFloat( 0 ) < +0x97bc`), the lifetime argument (1.0 in the dash X, range rate `+0x97ac` in the dash-jump X), `+0x9c20` stored before `June_AddEffectToSystem`, `+0x9c24 = 0` at 0.1 s, the EG / Nemesis branch, the five alpha tags (and that the tag follows the handle store in the dash-jump X), **JUNE_BUGS 425** (the dash-jump X BIG set keeps the small Spectrum name; the dash X does not).
- **`June_StateChangeBoostAttackIfPossible` `32978c60`**: the Electronic Move branch (`GetFutureStateSkillID() == 25223`, cost `GetActualMPConsume`), the Open Mind (25419) max count with `fiadd` + `_ftol2`, `32007ce0( 0x1b, 25029, true )` and its fall-through to 5.0, the turn, the three state pairs (219 / 220 / 221 vs 239 / 240 / 241, the id re-read each time), and the failed-MP landing. `June_TryGetEquippedSkillAbilityValue` vs `32007ce0`.
- **The 9 new enumerators** (script ids 228 229 230 236 237 239 240 241, and `ENSI_JUNE_DASH_COMBO_ZX` = 180): March's C++ values differ from June's ids; CH1m settles the mapping. The later kickoffs (CH1f / CH1g / CH1h / CH1i / CH1j2 / CH3c) are amended not to add them again.
- **The class-map correction:** `329cc500` has one caller, `InitInpuDataProcess` `32978e80`; it moved to CH1l.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1e commit>^..<CH1e commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1e -- Review" into the plan doc under the Done note, update the CH1e row in `STATUS.md`, check that `kickoffs/char-eve/CH1f.md` still matches, and commit with explicit pathspecs.
