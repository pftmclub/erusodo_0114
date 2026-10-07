# June buff port - Phase 14b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 14b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 14b done (its Done note and commit; play-test of check 1 passed) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_INSTANCE_KILL` |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 14b — IS_INSTANCE_KILL_DEBUFF consumer" "Phase 14b — Done"
```

Also `june2017_re/funcmap.tsv` rows `31fe4150`, `320471a9`, `32049b70`, `32082710`, `322a51bb`,
`322a0ed5`, `332b3d51`. **The code is in two commits:** the `Always.h` define and every `X2GUUser.cpp`/`.h`
hunk landed in the peer's C9c-1 commit `c9476fa` (it staged whole files), and `e5bad74` has the load line
(`X2BuffTempletManager.cpp`), funcmap and the Done note. Review the range `20ecd9d..e5bad74`.

## Task

Review Phase 14b's commit against June `320462a0` (CX2GUUser::DamageReact):
- **Reader completeness.** Re-run `python june2017_re/dispsweep.py 6ba4 6ba8 6bac` and confirm
  no reader was missed (every hit is the setter, the user DamageReact, the GameUnit ctor/dtor or an
  EH funclet). Confirm slot 200 has no override in any user vtable and no other virtual call site.
- **Placement.** The set sits after the last-hit serial and before `DoCustomFunctionByBuffInDamageReact`
  (June `@320471a9`, before `32275650` / `322a88c0`); the kill sits right after March's training
  clamp, with nothing March runs between them that June does not (the SOFTBODY block is dead:
  `FIX_NOT_ACTIVE_SKILL_AT_ENCHANT_DAMAGE` is defined in `Always.h`).
- **The kill.** `SetNowHp( 0.f )` is the user override (`3202ff60`, BBT 101 gate), as June's slot 61.
- **The #else path.** Flag off = March: no member, no helper, the load line as before.
- The member lives in `CX2GUUser`, not `CX2GameUnit` as in June. Check that nothing outside the user
  DamageReact reads it in June (the sweep), so the move is behaviour-neutral.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_BUFF_INSTANCE_KILL --range 20ecd9d..e5bad74 --with SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE`
(the `--with` folds in C9c-1's flag, which shares `c9476fa`; without it the X2GUUser files show as OTHER).
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user (the open `g_pMain+0x16a` clamp question stays the user's).
Write "Phase 14b — Review" into the plan doc, update the 14b STATUS.md row, and commit.
After the review the buff port's own scope is complete; no next kickoff to write.
