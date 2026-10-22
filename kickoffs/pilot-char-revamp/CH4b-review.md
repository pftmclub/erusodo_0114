# June character Aisha (Stage 5 pilot) - Phase CH4b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4b review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH4b done (its Done note and commit) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST` (+ `_DEBUG`, temporary), `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` (the 22416 tail) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL delegate run`, `ARME SKILL passive init ... open mind`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)" "CH4b - Done (2026-10-22, Opus 5.5)"
grep -a "Char CH4b" june2017_re/funcmap.tsv
grep -a "^| 397 " JUNE_BUGS.md
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7am. Unit delegate-list registrations per class (from pilot-char-revamp CH4b)"
```

The code: the `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST` blocks in `X2Lib/X2GameUnit.h` / `.cpp`, `X2GUUser.cpp` (InitPassiveSkillState),
`X2DamageManager.cpp` (AreaHit tail), `X2GUNPC.cpp` (AttackResult), and the CH4b lines in the `//{{ ... Phase CH4a` block at the end of
`X2Lib/X2GUArme_VioletMage.cpp` (`June_OpenMindAttackResult`, the 22416 tail).

## Task

Review CH4b's commit against June, in the listing:
1. **The list map.** Offsets `+0x66ac` / `+0x66bc` / `+0x66cc` / `+0x66dc` / `+0x66ec` = March's four members plus the new one; runners `322752b0`
   (RET 8, `edx` = arg) and `32275280` (RET 4) = March's `DoDelegateProcessWithDamageData` / `DoDelegateProcess` (empty test, null-entry skip).
2. **All 11 run sites** (`xrefs_to` both runners): each site's list, argument and position against March's call (CEffect `@31f86a8b`, user
   AttackResult `@3201399d`, NPC `@3276a7a1` / `@3276a83e` (the NAT_NORMAL branch, `m_pAI +0x38 == 0`), both DamageReacts, both CommonFrameMoves,
   AreaHit `@32624f7b`: inside `pAttacker != NULL`, after the drain-mana block, whatever the hit count).
3. **The clears:** `31fe19d0` (five `326df550( 0 )`) and `3207bfe0` (five), and that no June code pushes `+0x66ec` (CH4b's whole-binary scan).
4. **`32ce9f90`:** the gates (`31fceab0`, DD `+0x470`, `32610fb0( 0x11 )`), the clone level, `32d4d670( defender, -1, -1, 1 )` (JUNE_BUGS 397), vslot
   `0x224`, and the push in `32cecd70 @32ced5fc`-`32ced623`. The port adds a templet / empty-vector guard June does not have.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST --range <CH4b commit>^..<CH4b commit> --with SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST_DEBUG`
and `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE --range <CH4b commit>^..<CH4b commit>`; paste the `files:` / `in this flag's
files:` / `RESULT:` lines. The `Always.h` ERROR row is the `_DEBUG` `#error` guard; the one deleted line is the stale CH4a comment above
`June_InitPassiveSkillState`.

If the play-test passed, retire `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST_DEBUG` (comment out its define, build, deploy).
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH4b -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH4c.md`, and commit.
