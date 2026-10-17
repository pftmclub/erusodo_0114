# Atlas/Elysion port - Phase E1c - Relation runtime, entry points, OnFrameMove + the village-NPC effect set

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1c` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E1b (June's CX2TFieldNpc minus OnFrameMove); june-effect C9h-3a (done) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |
| Review | `kickoffs/atlas/E1c-review.md` (Opus 5.5; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Village engine" "Phase E1 — Split (user, 2026-10-17)" "Phase E1a — Done" "Phase E1b — Done" "Phase E1b — Review (2026-10-17)"
cat june2017_re/e1_relation_runtime.md         # lookups, TFieldGame functions, the parked refresh code
cat june2017_re/c9h5_fieldnpc_onframemove_diff.md
grep -a "Effect C9h-5" june2017_re/funcmap.tsv
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

1. **TFieldGame relation side** (whole, June addresses): village NPC creation `323ba5f0` (replaces March's `AddFieldNpc()` house
   loop, with June's code-event / PC-bang house gates), June's `AddFieldNpc( templet, name )` `323b0980`, replace `323b7ff0`
   (templet copy `323c3cf0` + `323c2aa0`, overrides incl. the TalkLoop/TalkWait June bug, owned list TFG +0x444), remove
   `323a9ed0`, restore `323b8970`; the C++ house ids June compares against (40082, 40089, 40091, 40141, 40143, 40144, 40147).
2. **Entry points:** paste back the two parked refresh functions (`e1_relation_runtime.md`, last section) and call them where
   June does: the code-event refresh `32344880` at the end of `Handler_EGS_UPDATE_CODE_EVENT_NOT` (inside its
   `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP` block), the per-quest refresh `32340630` after `RemoveUnitQuest( q, true )` in
   `CX2UIQuestNew` QUEST_COMPLETE_ACK and ALL_COMPLETED_QUEST_COMPLETE_ACK (June's third caller handles a June-only packet).
3. **OnFrameMove whole** (`325cdfa0`, 44 blocks, c9h5 diff) and the **village-NPC effect set**: `EFFECT_SET_NAME` (templet
   +0x5e8, E1a parses it), play after skin 1 is built (`@325ce1d9`), handle +0x190, stop only in the dtor; engine
   `PlayEffectSet( name, CX2TFieldNpc* )` `321e26d0` and its four readers next to C9h-3a's riding-pet branches. JUNE_BUGS rows
   for the diff file's candidates and for the raw +0xe0 pointer if the queued stop can read a freed NPC.
4. **Assets:** stage what the rows and houses name (`UI_NPC_BILLBOARD_ELYSION.lua`, `UI_BILLBOARD_ELYSION.lua`,
   `UI_NPC_BILLBOARD_SPA.lua`, `NPC_EL_BILLBOARD.X`, `EL_postbox.x`, `Motion_Npc_EL_Billboard.x`, `Shop_Ui_Board_Elysion.xet`; the
   Elysion village's own NPC assets are E2's); settle the ByVillage `VMI_SPA_RANOX` (1501, a March village) row.

**Done when:** Phase E1's "Done when" holds for steps 1, 5 and 6; in game Atlas Station's board and postbox show June's Elysion
meshes / board UI (`atlas` in chat); March villages unchanged except the June behaviour changes listed in the Done note.
