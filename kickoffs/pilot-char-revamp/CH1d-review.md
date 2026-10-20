# June character Aisha (Stage 5 pilot) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1d review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1d - Done"
awk -F'\t' '$6=="CH1d"' june2017_re/char_arme/ch0_class_map.tsv      # the 26 June functions
grep -a "Char CH1d" june2017_re/funcmap.tsv                            # the 28 rows the phase wrote
python june2017_re/charport_consts.py CH1d                             # every constant, with its width
```

The `June_*` members of CH1d are in the fourth block (`//{{ ... Phase CH1d`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1c's in the header; the two new enum members (`AVSI_AHM_DASH_JUMP_ZX`, `AVSI_AEM_DASH_JUMP_X3`) sit after `AVSI_AEM_ZZX`.
**Not carved in Ghidra (read with `june2017_re/funcdis.py --func <va>`):** `32cb7550`, `32cb7560`, `32cc6a50`, `32ce61f0`, `32ce6830`, `32ce69b0`. `32ce6830`, `32ce69b0` and `32ce6660` share one bounds range (no int3 padding): use `funcdis.py <va> 0 384`.

## Task

1. **Finish `32cd4830` (`June_DashComboZ2aStartFuture`), which the phase left as a declaration.** Read it whole in Ghidra. The unmapped callee is `32293f80` (a CX2GameUnit method: out pos, in pos; vslot `0x80` test, then the nearest point on the enabled lines of type other than 7 / 1).
   Map it (funcmap row, and its 8 other callers if one is already ported under another name), transcribe the function and the callee whole under the same flag, and add the body next to the others. The loop's other callees: `31c6c310` (11-argument `CanDown`, see the 7ac note in the CH1a Done note), `32293b80` (`JuneIsPosAlongLastTouchLine`), `31c7b560` (`GetDistance`). No stand-in.
2. **Review the 25 transcribed functions** function by function. What the phase could get wrong:
   - **Z1 `EventProcess` (`32cbd7b0`).** The else-if chain order against the decompile (`LAB_` gotos hide it): flag 0 -> `0xaf`, flag 1 -> `0xcc` (BM `0x17` / ADW `0x71`), flag 2 -> `0xdc` (AEM `0x68`), flag 3 -> `0xb0`, then the `0.65` key rule and the animation-end rule that follows the whole chain. Indices `m_fEventTime[ 0..3 ]` = `+0x8c10 + 12 i`, `bEventFlagList[ 0..3 ]` = `+0x7d85..+0x7d88`.
   - **`32cbe360` / `32cbe480` / `32cbe510` / `32cc4180`.** Flag and window logic, the `+0x9599` (= `m_bDashJumpZ`, the scan of the class range shows it is cleared at `32cb7538` and set at `32cd55cb`) gate, the class tests (`!= 2` is `UC_ARME_VIOLET_MAGE`, `0x68` AEM), the state ids.
   - **Fireballs.** `32ce61f0` (type 0, y + 180), `32ce6660` (type 1), `32ce6830` (type 0, z = -30), `32ce69b0` (type 7, z = -30): the type is the stack slot written before the `GetCloneFrame` call (`mov [esp+8], N`); the failure args of `CreateNotEnoughMPEffect` (`0, 180, 0` and `0, 0, -30`); `MagicFail` on the major particle system.
   - **Speeds.** `GetDashSpeed() x 1.2` (Z2), `GetWalkSpeed()` (JumpAttackZ / X), the `0.13` / `0.26` window of `32cc6c00`, the dropped speed-zero window in `32cb7550` / `32cb7560`.
   - **Constants** (`charport_consts.py CH1d`): 1000.0, 0.65, 0.55, 45.0, 0.4, 0.2, 0.03, 200.0, 0.3, 0.16, 4.0, 180.0, -30.0.
   - The Z2a `FrameMove` alpha tag (`32296ee0 @32cd4f03`).
3. If the enum choice is wrong (`AVSI_AHM_DASH_JUMP_ZX` for the shared id `0xe4`), say so: CH1 numbers the ids.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1d commit>^..<CH1d commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1d -- Review" into the plan doc under the Done note, update the CH1d row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1e.md` still matches, and commit with explicit pathspecs.
