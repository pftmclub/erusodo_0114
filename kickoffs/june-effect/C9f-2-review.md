# June effect completion - C9f-2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9f-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9f-2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET` (the viewer site also needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`) |
| Log tag | `JUNE_FX` (`python tools/logs.py --grep "JUNE_FX EQUIP_FX"`) |

Do not run at the same time as C9g: both touch `X2UnitViewerUI.cpp`.

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9f-2 - Done" "C9f - Done"
```

Also read: `june2017_re/funcmap.tsv` rows tagged `Effect C9f`, and the phase's commit (`git log --oneline --grep="C9f-2"`).

## Task: check the port against June in Ghidra (read-only), then the data

Hygiene is one command: `python tools/flagcheck.py JUNE_EQUIP_EFFECT_SET --range <C9f-2 code commit>^..HEAD`. Paste its `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check, in this order (the first ones are where a transcription slip would hide):
- **Parser `32814da0`**: the six keys and their getter types, the defaults on a failed getter, the percent clamp (`@328157c8-32815801`: > 100 -> 100,
  < 0 -> 0, NaN kept), both first-wins inserts, and the group key 0x51 after the loop. Then the dispatcher `32818480`: the skip on `m_eReadType`
  0 / 2 or a failed getter, the 55 / 85 / 89-93 targets, and that every other type (a failed `m_eType` getter included) reaches the parser.
- **Timer step `32961060`**: the switch-roll polarities (`0 < percent`; `timer >= limit` rolls, NaN accumulates; `rand() % 100 < percent` strict),
  the global elapsed time (June's `g_pKTDXApp +0x2c` protected float, ctor 1/85, ported as `GetElapsedTime()` = `m_fConstElapsedTime`), the live
  test, and `START_TIME < acc` strict.
- **GUUser** `32063650 @32065d57-32066052`: the order (record/condition first, then templet, timer, `GetShowObject`), the miss path
  (`3295ca60( false )`, `+0x15f4` and `+0x15fc` zeroed), the switch play zeroing `+0x15fc`, and the placement: live
  `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch only, between `SetRenderParam` and `OnFrameMove`. `31fd8140`: USI_WAIT, the skill-state
  test, `!(len > 1.0)`. The `+0x94d4` store at the end of `CommonFrameMove` (`@3206cee2`) and its ctor zero (`@32043273`).
- **Square** `32400620 @32402089-32402292`: the templet test before the lookup, no `GetShowObject`, owner kind 1 / `+0x1600`, the placement after
  March's `m_fCanNotInputTime` decrement (June's `param_1[400]`), the `+0x7c0` store and ctor zero (`@323f90cb`). `323f5920` and `323f56d0`.
- **Viewer** `31fa8ce0 @31fa9ab8-31fa9cb0`: placement after `UpdateTitle()` (`31f91d10 @31fa99ec`); the `UVS_FIELD` skip (`+0xf14`, confirmed at
  `SetUnit 31faa6f0 @31faa73c`); the condition chain and its unsigned `+0xfa4 > 2` (ported as `m_eOldMotionType` = June `+0xe58`, written
  together with `+0xfa4` by `31fa7ef0`); group 1; the handle never stored. `IsPlaying` `329581c0` and the filter `31fa2f20`.
- **CX2Eqip**: the five members (ctor `32961280`), `3295ca60`, and the `CX2EffectSet_EffectSetInstanceHandle` typedef repeated in `X2Eqip.h`.
- **Data** (`data036`): `Enum.lua` = March's + the 3 June tables (126 decompiled lines); `RenderDivisionItemTemplet.lua` = June's 113 blocks
  (8164 lines round trip); `UIEffectSet.lua` = 78 + 6 = 84 templets; `UIMajorXMeshPlayer.txt` + the 2 `Mesh_AC_Lowbody_Set7_Shake*` templets.
  The meshes' binaries (`AC_Lowbody_Set7_Shake.x`, `AC_Lowbody_Set7_Tail_2P.xet`) are June-only (June `data002`), noted, not staged.

Fix clear transcription errors under the same flag, then `python tools/build.py --deploy` (background). Hand design decisions back to the user.
Write "C9f-2 - Review" into the plan doc, update the C9f-2 review row in `STATUS.md`, and commit with explicit pathspecs.

## Play-test (regression only)

Nothing new is reachable (none of the 5 item ids is in March's `item.kim`). Pass = village, character select, a dungeon, back to the village,
clean exit; 0 EXCEPTION, no new `X2_*.tmp` crash bundle; `JUNE_FX EQUIP_FX_LOAD file=RenderDivisionItemTemplet.lua loaded=1 lists=5 items=5
group81=5 skipped_duplicate=0`; `ES_LOAD UIEffectSet.lua loaded=1 templets=84` (C9f-2 added 6) and `UNIT_VIEWER_FX_LOAD ... rows=18` unchanged.
