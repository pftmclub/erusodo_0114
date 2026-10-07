# June effect completion - C9g-2 review - PlayByMotionType and AnimationNameTable.lua

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9g-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9g-2 done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE` |
| Log tag | `JUNE_FX ANIM_NAME` (`python tools/logs.py --grep "JUNE_FX ANIM_NAME"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9g-2 - Done" "C9g - Done"
grep -n "Effect C9g" june2017_re/funcmap.tsv
```

## Task

Review the C9g-2 code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- `CX2UnitViewerUI::PlayByMotionType` (`X2UnitViewerUI.cpp`, the block before March's original): store order vs `31fa7ef0`
  (`m_eOldMotionType`, the `+0x1dc` code, then the `31fa5c90` head: emotion reset gate `+0xf14 != 10`, group stops 0..4 / cash shop 0..2, skipped
  for 6/7/8), the unit/unit-manager test after the pre-pass, the type-0xb remap polarity (WAIT <-> WAIT1), the play types (0-2 and 8 LOOP, rest
  ONE_WAIT), the Eve sound, the literal names of 7/8/9. Re-read `31fa5c90`'s jump table for types 0/1/2/3/4/5/9 and confirm none of them stops
  a group the head did not, or plays anything with an empty custom map (the C9g-2 note says they do not).
- `CX2UnitManager::AddAnimationName_LUA` / `GetAnimationName` vs `3237a060` / `3238b0e0` / `32375ee0`: first-wins, key order (class, type),
  the 11 defaults, nil class -> 0, nil name -> empty.
- The load order in `ResetUnitManager` (`31ee6217` before `31ee6224`) and the load line in `ErrorLog.txt`.
- The square: `InitWaitMotion` vs `323f2320` / `323fa700` (what else `323fa700` assigns that March's `InitWaitMotion` also set).
- The bypassed `m_iJuneFxTriggerCode2 = 0` in the viewer's `OnFrameMove` vs `31fa8ce0 @31fa8d32-31fa8d46`.
- The 55 call-site mapping in the Done note (re-run `june2017_re/disasm_range.py` on 2-3 sites, e.g. `32511eb3`, `324e7e27`, `32d7c962`).
- Data: the four `data033` motion files are June's (md5s in the Done note), `Motion_ARA_Lobby.x` carries the one repaired byte; look for any March
  animation name a March script or hard-code still plays that these four June files lack (the Done note's length-prefixed parse found none).

Hygiene is one command: `python tools/flagcheck.py JUNE_ANIMATION_NAME_TABLE --range <first C9g-2 commit>^..HEAD --with JUNE_FX_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Write "C9g-2 - Review" into the plan doc, update the STATUS row,
and commit. After the review, the next owners are C9h-2a/2b/3 (pet viewer) and backlog Stages 10 and 12; check their kickoffs exist.
