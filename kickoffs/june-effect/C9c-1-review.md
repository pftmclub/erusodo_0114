# June effect completion - C9c-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9c-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9c-1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE` (with `SERV_IRUHADEV_JUNE_FX_DEBUG` for the load line) |
| Log tag | `[JUNE_FX]` (`python tools/logs.py --tag JUNE_FX`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9c-1 - Done" "C9b - Done"
```

Also read: `june2017_re/funcmap.tsv` rows `3237a1f0`, `323760f0`, `323760f0 @detail`, `32043380`, and the phase's commits (`git log --oneline --grep="C9c-1"`).

## Task

Review C9c-1 against the June binary:
- `AddUnitViewerEffectSet_LUA` (`X2UnitManager.cpp`) against `3237a1f0`: argument order and types, first-row-wins insert, and the one
  deliberate difference (a wrong-typed argument drops the row and is counted in `skipped_bad_args`; June's typed reads would give 0).
  Say whether that difference can matter for the 18 rows (it cannot if every enum name resolves; the load line shows `skipped_bad_args=0`).
- `GetUnitViewerEffectSet` against `323760f0`: the three gates and their order. June maps class -> type with `31eca1d0`; the port uses the
  class's templet `m_UnitType`. Spot-check `31eca1d0`'s switch against March's templets for the classes March has (one class per type is enough).
- The loader in `CX2Data::ResetUnitManager` against `31ee6100` @`31ee6224` (after Correlate, outside `REFORM_SKILL_NOTE_UI`, main Lua state).
- `PlayEmotion` against `32043380` @`32043b69-32043cf5`: lookup arguments, owner on the UI effect-set manager, handle kept at `+0x943c`.
  The `32296f30` tag is deliberately **not** ported (C10b-2's helper): confirm the Done note names the site, and if C10b-2 has landed
  since, port the tag here under the same flag.
- Play-test result: the `UNIT_VIEWER_FX_LOAD ... loaded=1 rows=18 skipped_duplicate=0` line, 0 EXCEPTION.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE --with JUNE_FX_DEBUG --range <first C9c-1 commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row and deleted line.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "C9c-1 - Review" under the phase in the plan,
update the STATUS.md rows (C9c-1 review done; C9c-2b stays blocked on C9c-2a), and commit. Never launch X2_offline.exe; ask the user.
