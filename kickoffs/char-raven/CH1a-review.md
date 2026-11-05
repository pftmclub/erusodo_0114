# June character Raven (Stage 7) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1a review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (defined by CH1a: one enum member) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1a - Done"
awk -F'\t' '$7=="CH1a"' june2017_re/char_raven/ch0_class_map.tsv
```

## Task

Review the 19 CH1a functions against June in Ghidra (`python june2017_re/charport_adis.py raven <va>...`; the listing, not the decompile, carries the `StateChange` arguments):
- The real differences from March: `32c3d3f0` DashJump EventProcess (the Blade Master `RBM_DASH_JUMP_COMBO_X` test after the Veteran Commander one, `[unit + 0x38]` = 0x73 -> state 0xfc, 0x6a -> state 0x12c, else 0xb6; the both-keys-held branch); `32c25d60` PowerLanding StartFuture (`nowSpeed.x *= 0.5`, March `/= 2`).
- Everything else was written as "= March after the macros": Z keeps the re-attack gate in Wait / Walk / JumpLanding / DashEnd (June tests `m_bReAttackZ1Right` at +0x95d0 and `m_fReAttackZ1Time` at +0x95d4). Spot-check 3 end to end (suggest `32c448a0` DashEnd, `32c44d80` PowerLanding, `32c443f0` JumpLanding's skip-point key chain): branch order, float compares (`fcomp` / `test ah` idioms), the `/ 8` and `/ 6` gates on `GetMaxAnimationTime()`.
- `RBM_DASH_JUMP_COMBO_X` is appended to `RAVEN_STATE_ID` under `_CORE` with a "June script id 300" comment (June's `Enum.lua` has no such name; CH1e owns the state, CH1m maps June's ids). Confirm 0x12c = 300 and that no live March table indexes the enum's tail.
- `june2017_re/char_raven/ch1_offsets.tsv` (seeded from Elsword's base rows, class fields dropped): confirm the six Raven class rows CH1a added (+0x95d0 / +0x95d4 / +0x95d8 re-attack and dash-jump-X timers, +0x9674 / +0x9680 Black Hole hit count / time, +0x97d4 `m_eRavenCurrentStage`) against the listings.
- Routed from the CH1a reading: June's `m_iBlackHoleHitCount` is a `KProtectedType<int>` (March plain `int`); nothing to do unless a later group reads it through the protected type.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --range <CH1a commit>^..<CH1a commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`census.cl_cmd`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-raven/CH1b.md`, commit with explicit pathspecs.
