# June character Eve (Stage 7) - Phase CH1l5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l5 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l5 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` |
| Log tag | `JUNE_CHAR` (Aisha's skill use is live: the one Aisha play-test, if the user ran it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l5 - Done"
grep -a "Char Eve CH1l5" june2017_re/funcmap.tsv        # 7 rows
```

The members are the `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` block after `June_GetLearnedSkillAbilityValue` in `X2Lib/X2GUUser.cpp` (namespace `_CONST_GUUSER_JUNE_CH1l5_`); declarations and the six fields in the protected part of `X2Lib/X2GUUser.h`. Aisha's (`X2GUArme_VioletMage.*`) and Eve's (`X2GuEve.*`) copies sit in `#ifndef` pairs.

## Task

Review against June and against the two retired copies. **Delegate the mundane reads** (`june2017_re/charport_adis.py eve`/Ghidra dumps for `32008e40`, `31fc86e0`, `31fc93e0`, `31fd32e0`, `31fdbe70`, `3202d140`, `3202d3c0`; `git diff HEAD^ -- X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GuEve.cpp`). You decide:

- The seven bodies equal the retired Aisha copies (which equal Eve's): action ids 0x25 / 0x29 / 0x36-0x39, the 5-slot split of the state tables, the memo type test (`MI_REGENERATE_1` = June's 10), game type 4 for PVE, the `31edf8f0` bit 0xd read as false.
- The six fields are zeroed in the `CX2GUUser` constructor (scalars in the init list, arrays in the body) and Aisha's constructor still zeroes her own (compiled out with the flag, so only the base runs).
- Flag-off is the old tree (`flagcheck` PASS); nothing else in `X2GUArme_VioletMage.*` / `X2GuEve.*` reads a retired name that now resolves to a different object.
- `cl /P` of the two class `.cpp`s shows no class-local definition of the seven (the Done note holds the probe).

Hygiene: `python tools/flagcheck.py JUNE_USER_SKILL_USE_HELPERS --range <CH1l5 commit>^..<CH1l5 commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1l5 - Review" into the plan doc under the Done note, update the CH1l5 row in `STATUS.md`, check that `kickoffs/char-eve/CH1m.md` still matches, and commit with explicit pathspecs.
