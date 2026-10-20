# June character Aisha (Stage 5 pilot) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1c review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1c - Done"
awk -F'\t' '$6=="CH1c"' june2017_re/char_arme/ch0_class_map.tsv      # the 30 June functions
grep -a "Char CH1c" june2017_re/funcmap.tsv                            # the 32 rows the phase wrote
python june2017_re/charport_consts.py CH1c                             # every constant, with its width (32ce5970 is merged into 32ce5780's bounds: check the address column)
```

The `June_*` members of CH1c are in the third block (`//{{ ... Phase CH1c`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1b's in the header; the new enum member `AVSI_AEM_ZZX` sits after the CH1b `_UPGRADE` pair.

## Task

Review the transcription against June, function by function. **Delegate the mundane reads** (x2-phase, *When the phase is Opus*): the Ghidra dumps of the 30 functions go to the scratchpad (`decompile_function`; **`32cbafb0` and `32ce5970`
are not carved in Ghidra, read them with `june2017_re/funcdis.py --func <va>`**), you read the parts below. What the phase could get wrong:

- **The cancel template.** Every combo `EventProcess` is an else-if chain; check the ORDER of each against the decompile (`LAB_` gotos hide it) and each compare direction from the `fnstsw` / `test ah` / `jp` triple: UPDOWN, `SKILL_CANCEL_AFTER` (the March macro, AWAKE_LOCK form, stands in for June's two blocks:
  check it is the same test set, incl. `vslot 0x7f4( 1 )` and the `+0x8d4c` write), BWALK / WALK / DASH / Z / X, flag consume (`!( now <= stateChange && !animEnd )`), window (`start <= now < end`), the last key / anim-end rule (Z1 `0.5` / `0.2`-`0.1`, Z2 `0.6` / `0.2`, Z3up `0.7`, Z4up and X3front `1.4`, Z3 `0.65`).
  Spot-check Z2 (`32cba3e0`, four flags and their classes), X2 (`32cbafb0`, listing, incl. the tail `flag2 -> m_bCanChangeState`) and X1.
- **Indices.** `m_fEventTime[ 0..3 ]` = `+0x8c10 + 12 * i`; `bEventFlagList[ 0..3 ]` = `+0x7d85..+0x7d88`; the key bytes (tables in the Done note); the state ids (`ch0_state_skill.tsv`, the `id N` in the registration column) and the class ids (0xe HM, 0xf DM, 0x17 BM, 0x68 AEM, 0x69 AVP, 0x71 ADW).
- **`32ce4180`** (`June_CreateComboFireBall`): the name table (types 0..7 x element 0 / 1 / 2, the transformed Battle Magician / Dimension Witch set, the fall-through of type 1 into 7, types 5 / 6), the power (`GetPowerRate() x ball power`, gravity swap, Magical Makeup `31fcf3e0( 22022 )` + `32007c40( 0xf, 22040 )`) and the `CreateInstance` argument order against the call at `@32ce4582`.
- **`32cc2b90`** (`June_FlushMpWithRefund`): the roll is `GetRandomFloat( 0 ) < object +0x10`; the amount is the protected float at object `+0x20` times the MP spent. Confirm `+0x10` / `+0x20` are skill 22415's `SA_EFFECTIVE_RATE` / ability `0x42` in `32cecd70 @32ced2a3-32ced2c3` (CH4 owns the object).
- **Constants** (`charport_consts.py CH1c`): timers 0.3 / 0.53 / 0.382 / 0.033 / 0.35 / 0.47 / 0.82 / 0.83 / 0.71, MP multipliers 4.0 (X1, X2) and 6.0 (X3, X4, X4 fire), `100.0` offset of X1's not-enough-MP effect, scale `x 0.5 + 1.0`.
- **The two decisions the phase made** (confirm or hand back): reading `GetUnit()->GetClass()` for June's `[ templet +0xaf0 ]->[ 4 ]`; the second-skin anim as `m_pXSkinAnimFuture` (checklist 7o).

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1c commit>^..<CH1c commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1c -- Review" into the plan doc under the Done note, update the CH1c row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1d.md` still matches, and commit with explicit pathspecs.
