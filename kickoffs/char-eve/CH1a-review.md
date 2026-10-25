# June character Eve (Stage 7) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1a review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1a - Done"
awk -F'\t' '$6=="CH1a"' june2017_re/char_eve/ch0_class_map.tsv      # the 44 June functions
grep -a "Char Eve CH1a" june2017_re/funcmap.tsv                      # the 44 rows the phase wrote
```

The 44 `June_*` members are at the end of `X2Lib/X2GuEve.cpp` (after the last `#endif // ADD_EVE_SYSTEM_2014`), declared at the end of `X2Lib/X2GuEve.h`.

## Task

Review the transcription against June, function by function. **Delegate the mundane reads** (x2-phase, *When the phase is Opus*): the annotated listings of the 44 functions come from one command, `python june2017_re/charport_adis.py eve --phase CH1a --out <scratchpad dir>` (state-id names, member names from `char_eve/ch1_offsets.tsv`, vslot names, float / double values, wide strings next to each instruction); you read the parts below. What the phase could get wrong:

- **Functions Ghidra has no function for** (read from the listing only): `32965fa0`, `32966070`, `32981910` (the WALK EventProcess runs on from `32981680`: no int3 pair between them, so `charport_adis.py eve 32981680` prints both), `32982a00`, `329cc0d0`, `329cc170`. Re-read `32981910` and `32982a00` in full.
- **Branch structure the decompiler hides.** `32977810` (dash jump: the both-keys-held block turns the unit on the key pressed this frame, then jumps or falls), `32977d30` (jump dash: `m_bDownForce` goes straight to the power landing, no resets), `3297ae70` (jump down: the Queen's Throne flag `+0x96b4` repeats the double-tap test), `32982790` (dash jump landing: the Recovery test is an `else if` of the airborne test, the key chain after it is a separate `if`, as in March), `32981eb0` (dash: the Battle Seraph boost dash waits 0.5 s, March 1.0 s).
- **Compare directions.** `fcomp m; test ah,1; jne` skips when `0 < m`; `test ah,5; jp` skips when not `0 < m`; `test ah,0x41; jp` skips when `0 > m`. Check `0.0f < nowSpeed.y` (`3297acf0`, `32977810`), `nowSpeed.y >= 0.0f` (`3297ae70`, `3297b1f0`), `nowSpeed.y <= 0.0f` (`3297b070`), `GetJumpSpeed() < nowSpeed.y` (`32977d30`), and the Recovery test (`0.0f < +0x9c64` fails it; `GetNowMp() < +0x9c6c` fails it).
- **Constants.** 0.65 `334c9ca8` (float, = `s_fMaxDashJumpTime`), 1.85 `334cccd0` and 1.0 `334355b0` (doubles), 0.5 `3342ebc8`, -0.8 `334c9e50`, 45.0 `33472998`, 30.0 `33439c68`, 60.0 `33457f50`, 90.0 `33471350`, 40.0 `33439c70`, 15.0 `33449130`, 0.125 `33439ba8`, 0.01 `33437810`, 0.4 `33446fa0`. Re-read two.
- **Members named by use, not by a March twin:** `+0x9c60` (`m_iJuneAddAirDashCount`), `+0x9c64` (`m_fJuneCodeRecoveryCoolLeft`), `+0x9c6c` (`m_fJuneCodeRecoveryConsumeMp`), `+0x96b4` (`m_sEGQueensThrone.m_bEnable`), `+0x99b4` (the Photon Booster float). Confirm each from a writer (`329c47e0`, `char_eve/ch0_keys.tsv`) or an `offsetprobe.py` run on `CX2GUEve` (include `X2GuEve.h` in the probe source: `stdafx.h` alone gives an incomplete type).
- **The ability type `0x80`** in `32976e90` is read as `SA_AIR_MOVE_COUNT` (March index 127 + 1, June's numbering is March's + 1 from the pilot's `SA_COOLTIME_1 = June 148` on). Confirm with an Electra / Battle Seraph skill row that carries the ability.
- **JUNE_BUGS 421** (the dash-jump family stops clearing `m_bDownForce`): confirm it is June's own and not a decompiler loss; the three StartFuture bodies are tiny, read them.
- **Open item for the user / CH1k:** `June_GetSkillLevel` and `June_GetLearnedSkillAbilityValue` are Eve-local copies of the pilot's (`31fceab0`, `32007b10`). Decide whether CH1i-level phases of later characters should get a shared `CX2GUUser` home first (an Opus helper phase), or each class keeps its copy.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1a commit>^..<CH1a commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1a -- Review" into the plan doc under the Done note, update the CH1a row in `STATUS.md`, check that `kickoffs/char-eve/CH1b.md` still matches, and commit with explicit pathspecs.
