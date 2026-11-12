# June dungeons - Phase W3c - June's field graph (village / rest-area / field links)

> Routed by W3b (2026-11-12). June 2017 links the Elder - Besma - Altera - Peita stretch differently from March: some village portals lead
> to a rest area or another village instead of a field. W3b kept March's links (playable) and left this as a decision.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons W3c` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` ("Phase W3c", "W3b - Done") (branch `main`) |
| Depends on | W3b (done). One at a time with any session packing `data034`, `data036` or `data040` |
| Flag | none for data; a C++ gap found here gets its own `SERV_IRUHADEV_JUNE_*` flag |
| Log tag | `[JUNE_ENGINE] WVIL` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEONS_PHASES.md "Phase W3c - June's field graph (routed by W3b, 2026-11-12)" "W3b - Done (2026-11-12, Opus 5.5)"
```

Then `june2017_re/w3b_village_census.tsv` (rows whose `why` starts `routed W3c`).

## Task

Identify, then decide with the user, then port June's field graph for the places W3b found:
- `DLG_Map_Village.lua` START_POS rows whose June `OtherPlaceID` differs from March's `BattleFieldId` (not only the key name): Besma 42 / 43,
  Altera 62 / 63 (and March-only 67), Peita 72, Besma rest 311 / 313 / 315. June: Besma east -> Besma rest line 310; Besma rest -> Besma
  line 41, Peita line 108, Altera line 60.
- June's `BattleFieldData.lua` (data036, `june_src_fixed/data036_kom/BattleFieldData.src.lua`) portals (`PLACE_ID_TO_MOVE`) and world ids per field
  (June's ELDER_FIELD_02 uses world `WI_BATTLE_FIELD_ELDER_FIELD_01`), and `MapData.lua` `AddMapData` BEFORE / AFTER rows (June 23, March 18).
- What reads them offline: `X2Lib/Offline/X2OfflineBattleField.cpp` (field join, portals, `GetIsRight`), `X2StateField.cpp` ~1928 / 2138-2160
  (village / rest ids 20000-20010 / 1000-1008 already take the `EGS_STATE_CHANGE_FIELD_REQ` path), quests that name the fields.

**Decision taken (user, 2026-11-12, after the W3b play-test): June's route.** Port it whole: no need to ask again. Stop only if the port
would strand a quest or dungeon that has no June replacement (say which). The census rows become `same as June`.

What the W3b play-test showed (the half-installed June route; each must be right after this phase):
- Altera village end and Peita village start: boards read "Dicey Sky Road" (June's Besma rest destination), while March still sends you to the
  Altera / Peita fields.
- Altera village end: June's line map (`LINEMAP_Altera_Village.lua`, adopted by W3) drops the bottom walkways x 34583 -> 36264 (y 480 -> 409) and
  the portal there (March START_POS 67 -> `VMI_BATTLE_FIELD_ALTERA_FIELD_03` line 2): the platform is drawn but cannot be walked on.
- Elder rest area end: W3 replaced `Field_Elder_StageRest_alpaguide.Y` with June's (one sign plane, `Besma_Village_Milestone.dds`), so the end
  board shows no text: check it against June's route there (June's `ELDER_FIELD_02` uses world `WI_BATTLE_FIELD_ELDER_FIELD_01`).
- Cobo Express (B): June's `DLG_Destination_byButton.lua` has no row for the Elder (1000) or Altera (1002) rest areas, and March's list now matches
  June's; confirm the warp table (`MapData.lua` `CheckCOBOExpressTicketMapID`) and list still fit once the route is June's.
