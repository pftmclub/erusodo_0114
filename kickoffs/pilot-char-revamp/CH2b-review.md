# June character Aisha (Stage 5 pilot) - Phase CH2b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH2b review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2b done (its Done note, commit and the user's play-test) |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`), `[JUNE_FX]` (modifier tags) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH2b - Done"
grep -a "Char CH2b" june2017_re/funcmap.tsv                                   # the 16 rows the phase wrote
awk -F'\t' '$6=="CH2b"' june2017_re/char_arme/ch0_class_map.tsv                # the class-map rows
```

The `June_*` members are in the CH2b block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (`//{{ ... Phase CH2b`), the registration cases are in `June_SetEquippedSkillFuncHead`
(same file, after the CH2a cases), tag 1 of the modifier list is in `X2Lib/X2EffectSet.cpp`. Not carved in Ghidra (read with `python june2017_re/funcdis.py --func <va>`): `32cb7600 32cb7850
32cb7990 32cb79c0 32cb79e0 32cb7b00 32cb7fa0 32cb8d60 32cb8e00 32cb8e90 32cc5840 32cc95b0 32cccc20`.

## Task

What the phase could get wrong, in the order of risk. **The Ghidra decompile of several of these functions reuses stack slots: confirm values from the listing.**
1. **Death Field `32cd7bf0`**: the bomb set's `m_fDelayTime` is the death-sentence time (the decompile shows range^2; the listing at `@32cd7f25-32cd7f97` and `@32cd8124` reads the slot `[esp+0x14]` the buff time was written to). BFI argument order, `480` range squared, slot 94 with attack type 1, no `IsValidUnit`, hyper = the state's `m_bHyperState`, the `0.8` / `1.4` oneshots (`double` compare of the same float).
2. **Super Nova `32cdcef0`** against March's function: no invincibility test, `IsMyUnit` for non-NPC units, `( width + height ) * 0.5`, `<= 700`, the clamps, instance `+0x64` = `SetPowerRateScale`.
3. **Mana Intake `32cd81b0` / `32cd83c0`**: ability `0x2c` as the effect set's power rate (not the life time), the unit-NULL early return, the sound call `32276220( name, 1, 0 )`.
4. **Dark Road `32ce33b0` / Energetic Road `32ce35e0`**: power rate -1, life = ability 1; the upgrade block (`GetNowStateSkillID`, templet `+0x178`, `32007c40( 0x22, 0x56c3, 1, &out )`, modifier tag 1 with `b = 0`). Tag 1 in `X2EffectSet.cpp` against `321db1b0` case 1 (`@321db216-321db340`).
5. **Hell Stone `32cc6d70` / `32cc6de0`** and the one-line change to `June_EndHellStoneWindow` (the upgrade swap June passes): JUNE_BUGS 384 (both starts run the tail).
6. **Magical Makeup `32cc7d80` / `32cc95b0` / `32cc11a0`**: the buff erase loop (`3227baf0` / `3229e160`), `+0x96c8` = 1.3, `+0xe10` = `SetUseWorldColor`, the class test (March's `GetClass()` stands for `+0xaf0`'s class), `TransformStart` is March's (June's `32cc91b0` is CH4's).
7. **Registration vs `32cede00`** (decompile lines 674-720, 877-960, 1233-1300): case labels (22014 + 22039, 22015, 22016, 22017, 22022 + 22040, 22023, 22024, 22026, 22206 + 22219, 22208, 22211 + 22220), table names, which callbacks the hyper copies (Dark Cloud / Aging / Hell Stone / Mana Intake none of Init), the Guillotine LOOP / END states.
8. **Check the claims "= March"** (Inits, Death Field Start / End, Guillotine START EventProcess, the generic EventProcess clones) with `funcdis.py`.
9. Energy Drain (22213 / 22234) is deliberately not registered: check it still falls to the default path and that `CH2c.md` names `322f0e60` correctly.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARME_SKILL --with JUNE_CHAR_EFFECTSET_MODIFIER --range <CH2b commit>^..<CH2b commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines.

Play-test: units `ArmeDM` (Dark Magician) and `ArmeBM` (Battle Magician) in the save; check each equipped skill has its `[JUNE_CHAR] ARME SKILL registered` line, no new `Motion Error`, and that damage lands (kills in `offline_server.log`).

Spot-check 3 functions directly in the listing; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH2b -- Review" into the plan doc under the Done note, update the CH2b row in `STATUS.md`, and commit with explicit pathspecs.
