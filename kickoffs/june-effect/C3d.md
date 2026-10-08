# June effect completion - C3d - port the bounding back-speed state (BLOCKED on D10)

| | |
|---|---|
| Start | **blocked**: answer D10 first (plan doc decisions table). Model and split follow D10's answer |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | D10; C9h-4f (the USEFUL_VALUE Now set the LANDCRASH handler reads); C3c (same `DamageReactStateChange`) |
| Flag | new `SERV_IRUHADEV_JUNE_BOUNDING_STATE` |
| Log tag | `JUNE_FX C3d` |
| Review | `kickoffs/june-effect/C3d-review.md` (write it when done) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3b - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build)" "C3d - port: the bounding back-speed state (blocked on D10)"
grep -n "(C3b)" june2017_re/funcmap.tsv
```

D10's row in the decisions table (`C0 - Decisions and census plumbing`) holds the user's answer. Read it before anything else.

## Task

**User half.** These are the funcmap `(C3b)` rows `31feabd0 @31feb68f-31feb71a`, `31fd0230`, `31fd0260`, `31fc9d20`, `31fd02d0`,
`31fd0340` and `USI_BOUNDING_DOWN / _LANDCRASH / _UP`.
- **The case.** The RT_BOUNDING case in `CX2GUUser::DamageReactStateChange`. March's dead `BOUNDING_COMBO_TEST` case at
  `X2GUUser.cpp:12590` stays in `#else`.
- **The members.** `m_vBoundingBackSpeed`, plus the last-attacker UID / unit type that DamageReact writes `@32046f10` / `@32046f21`.
- **The handlers.** The five `CX2GUUser` base handlers, as virtuals. The Lu/Ciel hook `32b38cb0` is class code (checklist 7l).
- **The three `USI_BOUNDING_*` states.** In the form D10 chose.
- **Registration.** All 12 June classes register the states in their own InitState (funcmap row `USI_BOUNDING_DOWN / _LANDCRASH / _UP
  (C3b)` lists each function), with `*_DAMAGE_BOUNDING_*` tables in each class script. D10 says whether this phase adds them to
  March's nine classes and their data036 scripts (then also `Enum.lua`'s `USER_STATE_ID`, repacked) or leaves them to the ports.
- **x87 polarities.** Settle `31fd0291` (`test ah,0x41`) and `31fd0354` with `june2017_re/x87test.py`.

**NPC half.** These are the rows `32769060 @32769da7-32769e75`, `3275d900` and `3275a070`.
- **The case.** The RT_BOUNDING case in `CX2GUNPC::DamageReactStateChange` (March's dead one is `X2GUNPC.cpp:12893`).
- **The state types.** The three common-state types `DAMAGE_BOUNDING_DOWN` / `_LANDCRASH` / `_UP`. June puts them at `0x21..0x23`,
  where March's `STATE_TYPE_REACTION_STATE_NUMS` sits. Check every array sized by that count before inserting.
- **Bounding-up start.** The start in the NPC's CommonStateStart.
- **Unidentified.** Identify the three bytes `+0x872b` / `+0x8ac6` / `+0x8ac4`.

**Reach.** No March script uses RT_BOUNDING, `BOUNDING_BACK_SPEED_*` or the state names, so the phase is inert until June data is adopted.
Stage a test (a March skill's DamageEffect with `REACT_TYPE = RT_BOUNDING` and a class that registers the states) only if D10's answer
makes one possible.
