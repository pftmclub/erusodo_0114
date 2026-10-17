# Atlas/Elysion port - Phase E2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E2 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E2 done (its Done note and commit); best after the E2 play-test |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` (+ `SERV_IRUHADEV_CHAT_WARP_DEBUG` for the chat warp) |
| Log tag | `[ELYDBG]` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Elysion half: village" "Phase E2 — Elysion village (20009)" "Phase E2 — Done"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

E2 is almost all data. Review it against June's files (`june_src_fixed/`), the staged copies in `24965799\kom_contents\` and the live koms:
- **Verbatim checks** (diff, don't eyeball): Village[VMI_ELYSION] vs June `DLG_Map_Village.src.lua:991-1152` (only the two key renames and 517's
  two commented keys may differ); the 11 house blocks and the 7 shared Elysion rows vs June `DLG_Map_House.src.lua`; `HI_ADAMS_HOUSE` = June's
  whole block; the 11 NPCTemplet rows; WorldData 636; MapData 20009 + zone 18 (client and `ServerResource/US/MapData.lua`); the F02 exit;
  ShopItem's appended block vs June `:3389-3773`.
- **Decisions to re-test:** Adams = June's whole block (is Ranox's scene really June's, and do March villages other than Ranox list
  `HI_ADAMS_HOUSE`?); shared houses keep March's rows for March villages (March scenes); June's whole ShopItem refused (re-read
  `AddShopItemList_LUA`'s LFail01 in `X2ItemManager.cpp`); 517 commented until E3.
- **Reachability:** every `HouseIDList` element resolves in March's enum; every new house's NPC / mesh / sound / dialog is staged (census
  `--phase E2 --live` after the copy); the 12 data039 dialogs and the field-map dialog are June's ljconv bytes (`june_adopt_20261017_103941.tsv`).
- **C++:** the chat-warp hunk, and that the four A7 stand-ins (`X2LocationManager.cpp:603`, `X2StateMenu.cpp:6340`, `X2UIInventory.cpp:14120`,
  `X2WorldMapUI.cpp:495`) behave as June once 20009 has a templet.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_PORT --range <E2 commit>^..<E2 commit> --with SERV_IRUHADEV_CHAT_WARP_DEBUG --files X2Lib/X2ChatBox.cpp`.
Fix clear transcription errors under the same flag (then repack / `build.py --deploy`); hand design decisions back to the user. Write
"Phase E2 — Review" into the plan doc, update the E2 STATUS row, write `kickoffs/atlas/E3.md` if missing, and commit.
