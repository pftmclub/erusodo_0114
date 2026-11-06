# June character Raven (Stage 7) - Phase CH4a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4a review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH4a done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` + temporary `_PASSIVE_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN PASSIVE` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH4a - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4a - Review"    # the worked example
```
Then the Ghidra listings of `32c57660`, `32c3c7a0`, `32c3df50`, `32c4e5b0`, `32c28160`, `31fcd640`, and the uncarved `32c3c7f0` / `32c3c820`
(`cd june2017_re && python charport_adis.py raven <va>`), plus `32012b50 @32013b7a-32013d6b` and `31f85910 @31f86a3a-31f86a76` (the two callback sites)
and `320462a0 @32049cf5` / `32012b50 @32013186` (Spiritualized Fury). Funcmap rows `(CH4a)` carry the block-by-block reading.

## Task

Check CH4a against June (read-only in Ghidra). In order of risk:
- **`InitPassiveSkillState` `32c57660` block by block:** every `32890620` push is `( id, 1, 0, 1 )`; the SA ids land on the members the Done note names
  (the `ch1_offsets.tsv` rows marked CH4a: `+0x968c` / `+0x9690` / `+0x9694`, `+0x95b8` / `+0x95c4`, `+0x96d0` / `+0x9668` ( Behavior of Mercenary,
  SA 0x7d / 0x91 and **0x10** ) against `+0x96d4` / `+0x9738` ( Winnable Technique, SA **0x7c** and 0x7d x 0.25 ): June swapped which skill grants the
  damage-taken share and the debuff-time cut, confirm by the SA ids and CH1g / CH1h's reader offsets). The resets and their order; the three force-down
  blocks ( melee `+0x4058` first ); the `+0x9734` / 24413 push condition; the Shadow Step chain ( upgrade-swapped id `328905b0`, the `+0x130` MP read,
  the five registrations' slots and SP levels, END_UPGRADE's SP 0 ); state 222 (`RSI_SHADOW_STEP`) callbacks; the lists from `3207b770`'s `ecx`
  ( `+0x66ac` attack result, `+0x66cc` frame move ).
- **The callbacks:** Surely Victory's `cmp 5` and the cool-time test ( `< 0` incl. NaN ); Reversal Soldier / Survival HP crossing ( strict tests, 0.5,
  max HP read once, `m_fOldHp` written last ); X Crash's `+0x5c4 == 24010`; Explosive Nasod Hand's level test. JUNE_BUGS 551 ( flag part withdrawn ),
  555, 556.
- **Engine:** `CX2GUUser::m_DelegateProcessInHitCreateDamageEffect` and its two call sites ( user AttackResult default case after the spawn, also when it
  fails; CEffect only with a child and a user owner, before `SetScale` ); the two Spiritualized Fury id changes.
- **Not ported on purpose:** March's Shadow Piercing / Thrust / Punisher / Air Slicer / Bleeding Slicer reads ( June has no such passive ids ); confirm
  `+0x9661` ( Punisher ) has no June writer.
- Play-test result and log lines ( `RAVEN PASSIVE init` per class, `explosive nasod hand buff`, `surely victory buff`, `reversal soldier buff`,
  `survival technique buff`, `x crash hit` ), then retire `_PASSIVE_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_PASSIVE --with JUNE_CHAR_RAVEN_PASSIVE_DEBUG --range <CH4a commit>^..<CH4a commit> --files` the seven code files,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GUUser.cpp`.
No build if the review changes no code (a `//#define` retire is a not-awaited build). Then check the CH4b kickoff (`kickoffs/char-raven/CH4b.md`).
