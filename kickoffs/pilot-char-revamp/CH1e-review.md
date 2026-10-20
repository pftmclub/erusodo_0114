# June character Aisha (Stage 5 pilot) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1e review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1e - Done"
awk -F'\t' '$6=="CH1e"' june2017_re/char_arme/ch0_class_map.tsv      # the 50 June functions
grep -a "Char CH1e" june2017_re/funcmap.tsv                            # the 52 rows the phase wrote
python june2017_re/charport_consts.py CH1e                             # every constant, with its width
```

The `June_AVSI_*` members of CH1e are in the fifth block (`//{{ ... Phase CH1e`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1d's in the header; the six new enum members sit after `AVSI_AEM_DASH_JUMP_X3`.
**Not carved in Ghidra (read with `june2017_re/funcdis.py --func <va>`):** `32cb8b90`-`32cb8bd0`, `32cc11f0`, `32cc30b0`, `32cc34c0`, `32cc3800`, `32cc3b90`, `32cc37e0`, `32cc3b70`, `32cc6ae0`, `32cdd4f0` (its range holds a neighbour too), `32ce5f10`, `32ce6060`, `32ce6380`, `32ce6510`, `32cb7450`, `32cb74e0`, `32cb7d00`, `32cb7d30`.
`32cda840` / `32cdab20` share one bounds range (no int3 padding).

## Task

Review the 50 transcribed functions function by function. What the phase could get wrong:
1. **The cancel chains** (`32cc30b0`, `32cc34c0`, `32cc3800`, `32cc3b90`, `32cc2d50`, `32cc11f0`, `32cc1460`, `32cbf620`): the else-if order against the listing (`LAB_` gotos hide it), the `0 <= f` and `f < now` tests, the facing test of the bwalk cancel,
   `ZX` (`m_fAttackCancelAfterEx` `+0x8c88`, `oneZ` -> `GetComboZStateID`, `oneX` -> `GetComboXStateID`), and that `32cc2d50` has only the hyper half of `SKILL_CANCEL_AFTER` (no `SpecialAttackEventProcess`).
   The phase added `IsOnSomethingFuture()` to the walk / bwalk / dash / ZX cancels because the listings test `+0x7e1c` there; confirm for each.
2. **`32cc3800`** is three plain `if`s (no else) before the off-the-ground branch, comparing `+0x9784` with `m_fEventTime[ 0 ].keyInputStart` / `.keyInputEnd`; the self-transition `0xeb` on the animation end.
3. **`32cdab20` (XZZ FrameMove).** The six times are `31fd6230( 0..5 )`; the first three are inlined `EventTimerOneshot`, the last three `31e4c230` (the same function). The hit is `DAMAGE_EFFECT_ELEMENTAL_MASTER_XXZ_INVISIBLE_ATTACKBOX` with `GetPos()` / `GetRotateDegree()` twice / `GetLandPos().y`.
   Decide whether the `SPECIAL_OPTION_VALUE` stand-in (`m_vecJuneSpecialOptionValue`, `June_GetSpecialOptionValue`) is acceptable until CH1 (it is: the list is parsed in `CommonStateStart`, CH1 item 5) or needs the parser now.
4. **Fireballs.** `32ce5f10` (type 2), `32ce6060` (type 5, y + 180), `32ce6380` (type 0, y + 180), `32ce6510` (type 6): the type is the stack slot written before the `GetCloneFrame` call (`mov [esp+8], N`), the cost (`4 / 6 / 5 / 10`), the failure args.
5. **State ids** used in the hand-offs: `0xac` X4down, `0xab` X3down, `0xb3` DASH_ATTACK_X2, `0xc7` AEM_XZZ, `0xd8` XZZ_END, `0xe5 / 0xe6 / 0xe7` ZXX, `0xea / 0xeb / 0xec` ZZXX, `0x16` / `0x12` landings. If a new enum member's number is wrong, say so: CH1 numbers the ids.
6. **Constants** (`charport_consts.py CH1e`): 180.0, 1.4, 0.6, 0.8333 (qword), 0.9 (qword of the float), 1.33, 45.0, 200.0, 0.03, 0.27, 0.3, 0.34, 0.35, 0.53, 0.06, 0.1, 0.01, 3.0, 4.0, 5.0, 6.0, 10.0, 100 / 120 / 150, 40, -30.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1e commit>^..<CH1e commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1e -- Review" into the plan doc under the Done note, update the CH1e row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1f.md` still matches, and commit with explicit pathspecs.
