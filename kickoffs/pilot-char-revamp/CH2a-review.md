# June character Aisha (Stage 5 pilot) - Phase CH2a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2a review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2a done (its Done note, commit and the user's play-test) |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`), `[JUNE_SKILL]` (`--grep NUM`), `[JUNE_FX]` (modifier tags) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH2a - Done"
grep -a "Char CH2a" june2017_re/funcmap.tsv                                   # the 31 rows the phase wrote
awk -F'\t' '$6=="CH2a"' june2017_re/char_arme/ch0_class_map.tsv                # the 53 class-map rows
```

The `June_*` members are in the CH2a block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (`//{{ ... Phase CH2a`), the registration cases are in
`June_SetEquippedSkillFuncHead` (same file, before `default:`), the engine half is in `X2Lib/X2EffectSet.cpp` / `.h`.
**Not carved in Ghidra (read with `python june2017_re/funcdis.py --func <va>`, stop at the first `ret`; the bounds merge neighbours):** the 20 targets listed in the Done note.
`python june2017_re/disasm_range.py <start> <end>` reads an exact range.

## Task

What the phase could get wrong, in the order of risk:
1. **The registration cases vs `32cede00`** (decompile lines 177-500 and 1125-1180 plus the Meteo Shower tail `@32cee4b0-32cee7f7`): every table name (hyper tables that reuse the
   normal name: Gust Storm, Meteor Call, Binding Circle, Chain Lightning, Fire Road, Teleport), which callbacks the hyper state copies (not `Init`), the case labels (22000/22030 ...,
   22006/22038, 22007 / 22008 / 22009 only), the three Meteo Shower extra states (ids 0xd9 / 0xda / 0xdb) and the MP-rate tail of Teleport.
2. **`June_AVSI_SA_AHM_METEOSHOWER_FrameMove` (`32cdae90`) and the loop FrameMove (`32cdb5e0`)**: the fire block against March's (positions, the parity rule, `fStateTime / m_fFireGap`), the
   emit-rate block, the early `return` (no CommonFrameMove), the handle `m_ahEffectSet[EffSetID_June4]` = `+0x95f8` (constructor `32cc99d0` stores five handles from `+0x95e8`).
3. **Binding Circle `32cd57c0`**: the BFI argument order (`BFI, creator, level, adjust, skip` pushed as `0,0,1,this,0x49`), slot 94 with attack type 1, memo type 18 -> `AddForceDownGage`,
   `fScope / 500.0` in double.
4. **Chain Lightning `32cc2440` / `32cdf7f0` / `32cc23c0`**: the cancel chain order and the `1.2` s gate (`IF_SKILL_CANCEL_AFTER` is the AWAKE_LOCK form), `USI_WAIT` for the walk cancel,
   the always-memo effect set (JUNE_BUGS 382), the lock-on part, `ResetAnimSpeed` arguments of `Start`.
5. **Meteor Call `32ce2e90` / `32cc5580`** (June-only): the target filter order, `<=` on the distance, the `GetFixedDistancePositionInAllLine` argument order and the fallback, the effect-set
   arguments (custom position), the EventProcess cancel list (no BWALK).
6. **Fire Road `32cdc270` / `32cdc4a0` / `32cc0c50`** and **Teleport**: `0.24` / `0.49` gates, the early return when the unit is NULL, the dash / walk times (`0.25`, `0.638`), the Teleport `Start`
   gate (`GT_PVP` stands in for PVP-or-PVE).
7. **The modifier list**: the push order, the apply hook position (after the velocity block, before the follow-line block), tag 2 add vs multiply, the once-only log for other tags.
8. **Check the claims "= March"** for the clones the phase registered as March's members (Inits, StateStart / StateEnd of Blizzard and Meteo Shower, Teleport FrameMoveFuture / EventProcess, both CameraMoves).

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_SKILL --with JUNE_CHAR_EFFECTSET_MODIFIER --range <CH2a commit>^..<CH2a commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

The play-test result is in the Done note: check that every equipped CH2a skill has its `[JUNE_CHAR] ARME SKILL registered` line, that `logs.py --tag JUNE_SKILL --grep NUM` shows a
physic / magic equal to the tsv's x0.01 value, and that `ErrorLog.txt` has no new `Motion Error` or missing Aisha asset against `data\ErrorLog_pre_CH1.txt`.

Spot-check 3 functions directly in Ghidra / the listing (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`);
hand design decisions back to the user. Write "CH2a -- Review" into the plan doc under the Done note, update the CH2a row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH2b.md`
still matches (CH2b's skill cases go into the same `June_SetEquippedSkillFuncHead` switch, and its state functions may reuse `June_AVSI_SKILL_EventProcess`), and commit with explicit pathspecs.
