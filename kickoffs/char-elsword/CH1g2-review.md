# June character Elsword (Stage 7) - Phase CH1g2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1g2 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1g2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1g2 - Done (2026-11-04)"
awk -F'\t' '$7=="CH1g2"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 16 CH1g2 functions against June (`python june2017_re/charport_adis.py elsword --phase CH1g2 --out <dir>`; a listing can run past its function, stop at the `int3`, and `32cfe840` after `32cfe650` is another function). The branchy ones carry the risk:
- The five EventProcesses: dash X X `32cfe280` (end, flag 0 to X X X on `m_fEventTime[0].stateChange`, the `oneX` window, ground walk cancel, ground dash cancel; no skill cancel), dash X X X `32cfe440` (ground dash cancel, then `SKILL_CANCEL_AFTER( m_fWalkCancelAfter )`: JUNE_BUGS #522), new dash Z X `32cfe090` (end to WAIT on a line only, walk, dash, skill on `m_fSkillCancelAfter`), dash jump X X X `32cfe650` (end on a line, walk, dash, skill), dash jump X X `32cfe5e0` (= March), Compact Counter `32cfdcb0` (fall with `LINE_RADIUS * 1.5f`, walk cancel on a line).
- Compact Counter Start `32cfdbe0`: `fPhysic` + revenge damage, `GetSkillLevel( SI_A_ES_COUNTER, false, 0, true )` (id `0x4b2` = 1202; check the C++ value), chance `GetSkillAbilityValue( SA_EFFECTIVE_RATE, level ) * 100.0` against `GetRandomInt( 0 )`, `RT_SMALL_DAMAGE` and zero back speed, and NO memo defence time (March has `SetDefenceMemoTime( 4.f )`).
- New dash X X FrameMove `32d17550`: EventTimerOneshot 0.2 (`3346eeb8`), `IsEnoughMPForAttack( 6.0 [33439b40], Dummy2_Lhand, 0, ( 0, 0, 45.0 [3345b36c] ) )`, `EffectSet_EIS_DASH_XX` with every `PlayEffectSet` default. Check the effect set exists in the effect data (nothing runs yet; CH5 / CH1 data check).
- The three damage-only FrameMoves, the CameraMove (`CAMERA` tables 1 / 2), the two StateEnds and StartFuture (`+0x411c` = `m_bApplyMotionOffset`): re-read, confirm = March.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h`; `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1g2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1h.md`, commit with explicit pathspecs.
