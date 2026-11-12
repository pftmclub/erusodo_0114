# June dungeons - Phase W3d - June's village NPC roster, services and talk

> Routed by W3b (2026-11-12). W3b made every village / rest-area layout, NPC position and texture match June for the houses March lists.
> What is left is who stands there and what they offer.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons W3d` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` ("Phase W3d", "W3b - Done") (branch `main`) |
| Depends on | W3b (done). One at a time with any session packing `data034` |
| Flag | none for data; a C++ gap gets its own `SERV_IRUHADEV_JUNE_*` flag |
| Log tag | `[JUNE_ENGINE] WVIL` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEONS_PHASES.md "Phase W3d - June's village NPC roster, services and talk (routed by W3b, 2026-11-12)" "W3b - Done (2026-11-12, Opus 5.5)"
```

Then `june2017_re/w3b_village_census.tsv` (rows whose `why` starts `routed W3d`).

## Task

Census, then port what is unambiguous, house by house (data034 `DLG_Map_Village.lua` / `DLG_Map_House.lua`, core-list source):
- `HouseIDList` additions June makes in every village: `HI_GUILD_MARK`, `HI_ADAMS_HOUSE`, `HI_MU`, `HI_EVENT_GUILD_EDAN_VILAGE`,
  `HI_EVENT_BLESS_OF_SOLACE`; Peita `HI_PANE_PEITA_HOUSE` and Ruben `HI_SPA_SIGN_RUBEN` (both June-only houses); Ruben gate `HI_APINK`;
  Ruben village drops `HI_POSTBOX`. For each: its assets (`w3b_asset_closure.py` covers listed houses once added), its `AddFieldNpc` gate
  (`X2TFieldGame.cpp` ~3317-3391: quest relation, code-event, EDAN / spa-sign gates), and what clicking it opens offline (guild, events).
- The 18 June-only house blocks and the June-only places `VMI_SPA_RUBEN`, `VMI_SPA_RANOX`, `VMI_GUILD_AGIT` (worlds and systems March lacks):
  identify and route (scope rule), do not half-add. With them go June's data036 `MapData.lua` `AddMapData` rows and the
  `CheckCOBOExpressTicketMapID` / `ComputeCOBOExpressTicketCost` branches for those three ids (W3c left them; March's Enum lacks two of the names).
- Per-house service keys June changes (`EXCHANGE`, `MAKING_NPC`, `OLD_ENCHANT`, `OLD_ADVANCED_ENCHANT`, `HENIR_EQUIP_UPGRADE`, `EXTRACTION`, `TRADE`,
  `SYNTHESIS`, `GUILD`, `GURA_2014_EVENT`, `EVENT2/3`) and talk tables (`NPC_TALK_LIST*`, `NPC_COMMON_TALK`, `*_Talk`, `TALK_BOX`) in ~40 houses:
  check each system and string id exists in March before taking June's value. `HI_LUTO` / `HI_APINK` keys go with their listing.
- `DLG_Map_House_1.lua` (differs) and `_2.lua` (June-only).

Stop and ask the user before adding a house whose service needs a system the offline client lacks.
