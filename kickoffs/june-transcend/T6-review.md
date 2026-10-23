# June transcendence - Phase T6 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T6 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T6 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND` |
| Log tag | `[JUNE_TRANSCEND] ARME` (`python tools/logs.py --grep "JUNE_TRANSCEND] ARME"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T6 - Aisha transcended" "T6 - Done"
grep -n "Transcend T6" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ao. Transcendence, per class (from june-transcend T0)"
```

## Task

T6 found no unported Aisha code (the Done note lists why); the review settles whether that is true. Check against June in Ghidra (read-only):
- **Readers.** `31fcac90`, `31fcad70`, `31ff1e80` test `!= 0`, `31fceab0` tests `== 1`, `32cddcc0` returns at `32cddeb6`; the March copies in
  `X2GUArme_VioletMage.cpp` (`June_SetSkillBuffFactors`, `June_GetLearnedSkillTemplet`, `June_GetPassiveSkillAbilityValue`, `June_GetSkillLevel`,
  `June_HyperModeBuffEffectStart`) match. `31fdd490`'s class cases 104 / 105 / 113 name `AVSI_AEM/AVP/ADW_HYPER_MODE`.
- **No missed reader.** Scan `32cede00`, `32cecd70`, the Aisha vtable overrides (RTTI `3351d0b4`) for any other read of `CX2Unit +0x3c`, or of a
  skill in 22412-22417, 22041-22046, 22068-22070, 22224-22226, that the March port lacks. T0's list is `june2017_re/transcend/t0_readers.tsv`.
- **The play-test lines.** The `[JUNE_TRANSCEND] ARME passives` values against June's skill data (`june_src_fixed/data036_kom/NewSkillTempletVer2`
  rows 22412-22417); the `registered` lines for the twelve hyper-class actives; the memo 22217 line; the awakening table and cut-in.
- **`June_SetEquippedSkillFuncHead`** (`32cede00`): the `31fdbcc0` branch registers only 22217 and nothing for another transcended skill.

- **Retire the two temporary log lines** T6's play-test added under the same flag (`ARME info text` in `June_PushInfoText`, `ARME cast lookup` in `June_SpecialAttackLookup`), then build and deploy.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND --range <T6 commit>^..<T6 commit>`; paste its `files:` / `in this flag's files:` /
`RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T6 - Review" into
the plan doc, update the T6 row of `STATUS.md`, check that `kickoffs/june-transcend/T8.md` still reads right, and commit.
