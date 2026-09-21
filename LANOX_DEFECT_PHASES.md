# Lanox/Ranox Defect Phases

Nine defects found play-testing the ported Lanox content chapter
(`RANOX_PORT_PLAN.md`, ~22 phases; `LANOX_TROUBLESHOOTING.md`, the living
checklist that followed it). This document is self-contained — paste any one
phase section, plus the "Starting context" block below, into a fresh
conversation and continue from there without re-deriving anything already
settled here.

**Do not re-open** `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md`,
or the `mods/lanox-offline` branch.

**Fix philosophy, stated by the user:** fixes must stay as close to the
original/studio code as possible. No band-aids — no hardcoded tables, no
invented curves, no guessed values standing in for a file or a code path that
should actually be read. Where the true fix is data the user must pack
(`.lua` into a `.kom`), implement the whole feature anyway and degrade
visibly if the file isn't there yet; don't stop and wait.

---

## Starting context (paste this with any phase)

- Repo root: `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- Branch: `mods/lanox-port`
- Game directory (deploy target, logs, save):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- Client build (default `US_SERVICE`, see `CLAUDE.md` *Toolchains*):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  TRUNK="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
  DATA="F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data"
  MP="-m -p:MultiProcessorCompilation=true"
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL      -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  ```
  `touch X2Lib/stdafx.cpp` before rebuilding if any header (`Always.h`,
  `X2Define.h`, etc.) changed — the PCH silently discards header edits
  otherwise. Deploy artifact: `X2/US_SERVICE/x2.exe` (lowercase) → copy to
  `$DATA/X2_offline.exe`, confirmed programmatically (size/mtime), **then ask
  the user before launching, every time.**
- Standing constraints (`CLAUDE.md`, `LANOX_TROUBLESHOOTING.md`):
  - Every client C++ change gated behind a `SERV_IRUHADEV_*` flag declared in
    `KTDXLIB/Always.h`, house comment-block style, `#endif SERV_IRUHADEV_FOO`
    trailing token, original code preserved in `#else`.
  - CP949/ISO-8859 source files: edit at byte level (Python `rb`/`wb`), verify
    with `file <path>` + `git diff --stat` after every edit — a plain
    Read/Edit round-trip has silently re-encoded whole files to UTF-8 on this
    project before.
  - Every `.lua` must go through `elstools compile-batch -v mar-2014` before
    packing; `.src.lua`/`.std.luac` must never ship inside a `.kom`. Verify by
    extracting the **live** `.kom`, never by reading the `kom_contents/`
    staging dir — the two can legitimately disagree (packer compiles on the
    fly).
  - Never launch `X2_offline.exe` without asking first, every time.
  - `els_db.sql` in the game directory is real player-character data; never
    delete it to "start clean" without asking.
  - Server-side Lua the client needs is the user's to XOR-encrypt and pack;
    implement the full feature with a degrade-if-missing path, name the exact
    file when reporting done.
- Logs, all in the game directory, flushed per line: `offline_server.log`,
  `offline_packets.log`, `ErrorLog.txt`, `State.log`, `OpenScriptFile.log`.
  ```sh
  grep -E "UNHANDLED|EXCEPTION" "$DATA/offline_packets.log" | sort | uniq -c | sort -rn
  tail -50 "$DATA/offline_server.log"
  ```
  `CX2OfflineLog::Server` is **not** open during `CX2LocationManager` parsing
  (confirmed: only 1 of 4 expected debug lines landed in an earlier session) —
  use `ErrorLogMsg` instead for anything in `VillageParsing`, `HouseParsing`,
  or `CreateFieldNpc`.
- June2017 reference client (source of pre-port values, use only to *read*,
  never to copy wholesale without checking against this build's enums):
  `.../237311/14JUN17/data/...`. Extract with `elstools`.

---

## Investigation ledger — what is already proven (do not redo)

### Disproved (9 dead ends)

| Hypothesis | Verdict |
|---|---|
| A village's `HouseIDList` lost its Cobo houses | No — every village block in `march2014/data034/DLG_Map_Village.lua` is byte-identical to the April-2014 sibling client's; only `VMI_RANOX` + `VMI_BATTLE_FIELD_RANOX_REST_00` were added |
| `HI_LUICHEL_HOUSE`'s block was edited by the port | No — byte-identical to April's |
| A new Ranox `HOUSE_ID` collided with an existing one | No — 78 `HI_*` in `Enum.lua`, zero numeric collisions |
| `WORLD_ID` drifted between C++ and Lua | No — 616 vs 617 entries, **0** value mismatches (only `WI_END` differs) |
| `HOUSE_ID` drifted between C++ and Lua | No — 0 mismatches; `Enum.lua` has one extra (`HI_EVENT_BLESS_OF_SOLACE`) |
| `StringID_def.lua` renumbered the string table | No — repo file is a strict **superset** of April's (30952 → 45161 entries), **0** renumbered IDs |
| New `STR_ID`s resolve to empty strings | No — `general.ess` in the game dir has **45160** lines, matching `StringID_def.lua`'s max (45159) |
| The `SERV_IRUHADEV_FIELDNPC_EMPTY_NPCLIST_GUARD` reapplication widened behaviour | No — `git diff dcde7b3^ HEAD -- X2Lib/X2TFieldNpc.cpp` shows the `#else` arm is the verbatim original; the guard only adds `&& m_NPCList.empty() == false` |
| A missing house block aborts `HouseParsing` (`MessageBox` + `return`, `X2LocationManager.cpp:1109-1113`) | Not triggered — every `HOUSE_ID` in every village's list has a block in `DLG_Map_House.lua` (64 blocks) |

Also, for issue 5 specifically, five more are dead (see Phase 5's own table).

### Proven root causes

- **Issue 1** (no Cobo NPCs in Lanox) → the five shared "common NPC" house
  blocks are in `VMI_RANOX`'s `HouseIDList` but have **no `WI_VILLAGE_RANOX`
  row** in their `COMMON_NPC_LOCATION` table. Luriel is Elder-only *by
  design*, not a regression.
- **Issue 2** (dead "Yes"/"Yes" dialog buttons) → ported Ranox house blocks
  carry June-2017 affinity-system flags (`EVENT2`/`EVENT3 = TRUE`) whose
  handlers are empty stubs in this build.
- **Issue 3** (empty NPC store) → `ShopItem.lua` has no `VMI_RANOX` block and
  was never staged under `march2014/` at all.
- **Issue 4** (no Lanox dungeon-list tab) → `DLG_UI_Local_Map_NEW.lua`
  (data034) has exactly 8 region radio buttons and was never ported.
- **Issue 5** (all Lanox dungeons locked) → real defect (level ruled out);
  narrowed to the dungeon-*list surface*, likely the same root as issue 4 —
  see Phase 5.
- **Issue 6** (Cobo Express refuses Silent Night Shelter) →
  `CheckCOBOExpressTicketMapID` in `MapData.lua` has no branch for
  `VMI_BATTLE_FIELD_RANOX_REST_00`, so its zone is 0 and the pair is refused.
- **Issue 7** (crash entering Ashen Land) → 18 missing mesh/motion assets for
  9 field-battle monster types, plus a separately-broken `DLG_UI_BUTTON17.TGA`.
- **Issue 8** (no signboard at Behemoth Crater's Lanox end) → not a data gap;
  no code in this build reads a direction-board table at all. The one
  in-engine "points elsewhere" mechanism (`QUEST_GUIDE` indicator) is already
  correctly fed and is merely off by default until the player opens the
  quick-quest window.
- **Issue 9** (leftmost Lanox teleport lands at the crater's start, not end)
  → the offline emulator drops `m_bNowBattleFieldPositionInfoStartPosition`
  from the join-battlefield ACK; a start-pos *key* is then reinterpreted as a
  raw *line index*. Not Lanox-specific — affects every village→field entry
  whose intended arrival isn't near line 0, Lanox's west door being the only
  one that currently is.

---

## Phase 0 — This document

Done by writing this file. If resuming after this phase, skip to Phase 1.
When all phases are complete, fold results back into
`LANOX_TROUBLESHOOTING.md`'s "Known-open items" — in particular promote the
`DLG_UI_BUTTON17.TGA` entry from "not merged yet" to "present but invalid
data" (Phase 7c).

---

## Phase 1 — Cobo NPCs missing in Lanox (Luriel / Camilla / Ariel) [issue 1]

**Paste with:** Starting context + this section.

**Premise correction — this is Lanox-only, not "all villages":**

- `HI_LUICHEL_HOUSE` (Luriel) appears in `VMI_ELDER` **only**, with **no
  `COMMON_NPC_LOCATION` table at all** — single-village house by design,
  unchanged from April. Luriel's absence elsewhere is original behaviour.
- `HI_CAMILLA_PVP_HOUSE` and `HI_ARIEL_EVENT_HOUSE` are in every main
  village's `HouseIDList` including `VMI_RANOX`, but their
  `COMMON_NPC_LOCATION` tables list only the 8 old-world entries
  (`WI_VILLAGE_{RUBEN,ELDER,BESMA,ALTERA,PEITA,VELDER,HAMEL,SANDER}`) — **no
  `WI_VILLAGE_RANOX` (582) row.**
- Same gap for three more shared houses also in `VMI_RANOX`'s list:
  `HI_POSTBOX` (14 worlds), `HI_PRIVATE_BANK` (8), `HI_BILLBOARD` (20).

**Confirmed at the source — this is the 3D placement path.**
`CX2TFieldGame::AddFieldNpc` (`X2Lib/X2TFieldGame.cpp:3116-3200`) walks the
village's `m_HouseList`; `:3140-3144` computes `bFindNpcPos` by matching the
current world against the house's `m_vecCommonPos`. The studio's own comment
at `:3143` (translated): *"if an NPC registered in a Village has commonPos and
the corresponding world isn't there, the NPC isn't created."* Then `:3186-3197`:

```cpp
ASSERT( true == bFindNpcPos && "dlg_map_house.lua<...> position must be specified" );
if(bFindNpcPos == true)
{
    CX2TFieldNpcPtr npcNode = CX2TFieldNpc::CreateTFieldNPC( true );
    if(npcNode != NULL) { npcNode->CreateFieldNpc( GetWorld(), pHouseTemplate ); ... }
}
```

No matching row → the NPC node is never created at all. The same gap also
costs the field-map quest mark (`X2WorldFieldMap.cpp:427-441`, the only other
`m_vecCommonPos` consumer), so one data fix repairs both surfaces.

The Lanox minimap dialog is **not** the problem —
`march2014/update/data034/DLG_UI_FIELD_MAP_Ranox_Village_NEW.lua` already
declares `CAMILLA`, `ARIEL`, `LURIEL` (bank icon), `Post`, `BOARD` and the
rest.

**Fix (data, no C++):** in `march2014/data034/DLG_Map_House.lua`, append a
`WORLD_ID.WI_VILLAGE_RANOX` row to the `COMMON_NPC_LOCATION` table of
`HI_CAMILLA_PVP_HOUSE`, `HI_ARIEL_EVENT_HOUSE`, `HI_POSTBOX`,
`HI_PRIVATE_BANK`, and `HI_BILLBOARD`, copying the row shape verbatim from the
existing `WI_VILLAGE_SANDER` row and taking coordinates from June2017's
`DLG_Map_House.lua` Ranox rows. Do not restructure the table.

**The "all villages" half of the original report is settled by data, not by
asking.** Old villages' `COMMON_NPC_LOCATION` rows are byte-identical to
April's, so there's no data cause for Camilla/Ariel to be missing in
Ruben/Elder — best explained as "missing where the player was standing," i.e.
Lanox. Do not block on this; fix the Ranox rows and make the scope check a
step of this phase's own play-test (below). If Camilla/Ariel are *still*
absent in Elder after the fix, that's a second, runtime defect — open it as
**Phase 10**, starting from `AddFieldNpc`'s other early-outs
(`m_HouseList` population in `CX2LocationManager::VillageParsing`, then
`CreateFieldNpc` returning NULL), not from the Lua.

**Verify:** pack, extract the live `.kom`, confirm bytecode + row present.
Play-test: walk into Lanox — Camilla, Ariel, postbox, bank, billboard all
present; walk into Elder — same NPCs still there (scope check).

---

## Phase 2 — Lanox NPC dialog shows two dead "Yes"/"Yes" buttons [issue 2]

**Paste with:** Starting context + this section.

Talk-window buttons are **texture-atlas image buttons** declared in
`DLG_UI_Message_Ver1.lua` (`SERV_PVP_NEW_SYSTEM` is on →
`X2TFieldNpcShop.cpp:78-82`); captions are baked into the art, not driven by a
`STR_ID`. This is a data fact, not a code branch.

Root cause: ported Ranox house blocks carry June-2017 affinity-system menu
flags. `EVENT2 = TRUE` / `EVENT3 = TRUE` appear **only** in ported blocks —
`HI_EDEL` (`DLG_Map_House.lua:4196-4197`), `HI_STEEL` (`:4320-4321`),
`HI_FACEOFF` (`:4445-4446`), `HI_SYOTA` (`:4564-4565`),
`HI_EVENT_GUILD_EDAN_VILAGE` (`:4850`) — in **zero** March-native houses.
Their companion keys (`NPC_TALK_LIST0..3`, `Receive_*_Gift_Talk`,
`Gift_UI_Talk`, `Every_Day_Reward_Talk`, `Level_Up_Talk`) are read by no C++
in this build; the handlers are empty stubs (`X2TFieldNpcShop.cpp:2023-2034`
— `case NMUM_EVENT2: { } return 2;`).

Edel (`NUI_EDEL = 10072`) offers 0 quests (confirmed via
`offline_server.log`), so the quest button is hidden too — leaving exactly the
two dead Event buttons observed.

**Fix:** remove `EVENT2 = TRUE` / `EVENT3 = TRUE` (and the orphaned affinity
keys in the same blocks) from the ported Ranox houses. Do **not** implement
`NMUM_EVENT2/3` — they are entry points to a system this build does not have;
implementing them would be new functionality, not a port fix.

**Adjacent, separate line item in the same phase:** `HI_IGNIA` (`:4621`) has
*no* menu flags at all → talking to Ignia currently yields zero buttons.
Determine the correct flag set from June's block (filtered to keys this build
actually reads) and add it.

**Verify:** pack, extract, confirm. Play-test: talk to Edel — no dead Event
buttons; talk to Ignia — has its intended buttons.

---

## Phase 3 — Lanox NPC store is empty [issue 3]

**Paste with:** Starting context + this section.

Not a packet — there is no NPC-shop request packet anywhere, and
`X2Lib/Offline/Handlers_Shop.cpp` is the *cash* shop, unrelated. The stock
list is pure client data:

`X2UIShop.cpp:1349-1357` → `CX2ItemManager::GetNpcShopItemList(currentVillageID, npcId, …)`
(`X2ItemManager.cpp:1208`, miss → `clear(); return false;`) → fallback
`GetShopItemList(currentVillageID, …)` (`:1186`, miss → `clear(); return;`,
silently). Both maps are filled by `AddShopItemList_LUA` (`:1022`) from
**`ShopItem.lua`**, opened at `X2Data.cpp:1730`. Keys: `VILLAGE_MAP_ID` for
the village-wide list, `pair<VILLAGE_MAP_ID, NPC_UNIT_ID>` for per-NPC stock
(`X2ItemManager.h:556-558`).

`ShopItem.lua` has 28 `VILLIAGE_ID` blocks (studio's own two-I typo,
transcribe verbatim) and **no `VMI_RANOX`**; the file was never staged under
`march2014/` at all.

**Fix:** extract the live `ShopItem.lua` from its `.kom`, add an
`AddShopItemList_LUA{ VILLIAGE_ID = VILLAGE_MAP_ID["VMI_RANOX"], SALE_GROUP =
…, SALE_ITEM = …, SALE_ITEM_NPC = {…} }` block modelled byte-for-byte on the
`VMI_SANDER` block, with `SALE_ITEM_NPC` rows for the three Lanox houses that
carry `SELL_NPC = TRUE`:

| NPC | ID | House block | Flags |
|---|---|---|---|
| Steel | `NUI_STEEL` = 10068 | `HI_STEEL` (`:4253`, `HT_EQUIP_SHOP`) | `SELL_NPC`/`ENCHANT`/`REPAIR_NPC`/`EXCHANGE` |
| Faceoff | `NUI_FACEOFF` = 10069 | `HI_FACEOFF` (`:4377`, `HT_ALCHEMIST_HOUSE`) | `MAKING_NPC`/`ATTRIBUTE`/`SELL_NPC`/`EXCHANGE` |
| Syota | `NUI_SYOTA` = 10070 | `HI_SYOTA` (`:4502`, `HT_ACCESSORY_SHOP`) | `SELL_NPC`/`MAKING_NPC` |

Item IDs come from June2017's `ShopItem.lua`, **filtered to items that exist
in this build's `ItemTemplet`** — drop entries that don't resolve, never
invent replacements. If the Lanox rest stop (`VMI_BATTLE_FIELD_RANOX_REST_00`)
has a vendor, it needs its own block; the file is keyed per map, not per
region.

Also verify `GetCurrentVillageID()` genuinely returns 20008 in Lanox — a
truly empty window (rather than Ruben's stock) already implies it does, since
the fallback path would otherwise have populated something.

**Verify:** pack, extract, confirm. Play-test: Steel/Faceoff/Syota all list
real, purchasable stock.

---

## Phase 4 — No Lanox tab in the dungeon list [issue 4]

**Paste with:** Starting context + this section.

The region tab strip is **not enumerated in C++**. `X2LocalMapUI.cpp:532-559`
appends controls from the dialog named by the Lua global `LOCAL_MAP_DLG`;
`:249-258` reads the clicked button's `GetDummyInt(0)` and passes it straight
through; `:387-530` iterates whatever radio buttons exist. A region has a tab
**iff** a radio button for it exists in the dialog's Lua.

March's shipped `data034/DLG_UI_Local_Map_NEW.lua` has exactly 8 buttons
(`LMI_RUBEN … LMI_SANDER`) and **is not staged under `march2014/` at all**.
Every other layer is already correct: `Enum.lua:4914` `LMI_RANOX = 10008`,
`DLG_Map_World_Elios.lua:12`, `DLG_Map_Local.lua:10`, `MapData.lua:348+`,
`X2LocationManager.h:131-144`.

**Fix (data only, no C++):**
1. Stage `data034/DLG_UI_Local_Map_NEW.lua` from
   `.../237311/24965799/march_data/data034/` and hand-add a **9th**
   `CreateRadioButton` with `AddDummyInt(LOCAL_MAP_ID.LMI_RANOX)`,
   `SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)`,
   and the same `SetGroupID` as the other 8.
2. **Do not copy June's file wholesale** — it references
   `LMUCM_LOCAL_MAP_ARROW_LEFT/RIGHT` and `LMI_ELYSION`/`LMI_EVENT`/
   `LMI_SPECIAL`, none of which exist in this build (`X2LocalMapUI.h:9-16`).
3. Merge `RANOX_BTN_NORMAL/OVER/DOWN` regions into March's
   `DLG_UI_Button20_NEW.tga` (June uses `DLG_UI_Button21.tga` /
   `Btn_RANOX_N/O/D`) — same class of work as the atlas merges tracked in
   `LANOX_TROUBLESHOOTING.md`. **Diagnose Phase 7c's TGA format problem
   first** (see Phase 7) — it's presumably the same export path, and doing
   this merge before that's understood risks a second invalid atlas.

Related latent gap, log but don't fix here: `X2LocationManager.h:487`
`IsVillageField()` bounds `VMI_RUBEN … VMI_SANDER`, excluding `VMI_RANOX`
(zero callers today); same `<= VMI_SANDER` pattern at `X2WorldMapUI.cpp:2457`.

**Verify:** pack + atlas repack, extract, confirm. Play-test: dungeon list
shows a Lanox tab alongside Sander's.

---

## Phase 5 — All Lanox dungeons locked [issue 5]

**Paste with:** Starting context + this section.

Lock state is **100% client-side** here. `SERV_NEW_EVENT_TYPES` is undefined,
so `SetDungeonStatus`/`IsDungeonEnable` (`X2DungeonManager.h:22-37`) and
`Handler_EGS_UPDATE_DUNGEON_STATUS_NOT` are compiled out — no offline handler
is needed or missing.

The only evaluator is `CX2DungeonManager::IsActiveDungeon`
(`X2DungeonManager.cpp:560-625`): templet must exist, `m_RequireDungeonID`
and every `m_vecExtraRequireDungeonID` must be cleared, and
`level >= m_MinLevel - 2`. Inactive icons are **hidden**
(`X2LocalMapUI.cpp:709 SetShowEnable(false,false)`), not padlocked — the only
lock art is the unconditional `Ranox_SECRET_DUNGEON_LOCK` sprite on the
Secret-Hell description button.

**Five hypotheses are already dead. Do not redo any of them:**

| Ruled out | Evidence |
|---|---|
| Character too low-level | `els_db.sql`: play-test character (`unitUID = 12`) is level **80** |
| A Ranox `dungeonName` STR_ID doesn't resolve | All 19 Ranox blocks' `dungeonName`/`dungeonDesc` IDs present in `march2014/data036/StringID_def.lua` (45161 defs) — **0 missing** |
| `DUNGEON_ID.DI_RANOX_*` is `nil` in Lua | `march2014/data036/DungeonEnum.lua:293-311` publishes all 19, values matching `KncWX2Server/Common/Enum/DungeonEnum.h:491-509` exactly |
| A `nil` hole truncates the parse loop | There is no loop — `DungeonData.lua` is a flat list of `g_pDungeonManager:AddDungeonData({…})` **calls**, so a bad block only drops itself |
| The region prerequisite isn't met | `unit_dungeon` for unit 12 contains `33250` = `DI_SANDER_BEHIMOSS_HEART_NORMAL` (and `33252` EXPERT) |

`DI_RANOX_SECRET_COMMON` is absent from `DungeonEnum.h` **by design** — a
port-time comment at `DungeonData.lua:14743-14755` records that only
`DI_RANOX_SECRET_HELL` is owed. Not a lead.

And the templet that should be visible has no gate at all:
`DI_RANOX_BURNDEDFOREST_NORMAL` carries `m_MinLevel = 70`, no
`requireDungeonID`, no `EXTRA_REQUIRE_DUNGEON_ID`. At level 80 with Behemoth's
Heart cleared, `IsActiveDungeon` should return true.

**So the remaining suspect is the surface, not the templet — probably the
same surface as Phase 4.** Investigation order:

1. **Do Phase 4 first and re-observe.** With no `LMI_RANOX` radio button, the
   dungeon-list dialog cannot show Ranox dungeons at all; what the user saw
   was most likely the Lanox *local map* gates
   (`march2014/update/data217/DLG_Map_Local_Ranox.lua`) instead. Phase 4's fix
   may make this phase evaporate, and it certainly changes what "locked" is
   describing.
2. **Check the gate→dungeon wiring** in `DLG_Map_Local_Ranox.lua` against a
   working region's file: whether each gate's `AddDummyInt`/dungeon binding
   names a `DUNGEON_ID` this build has, and whether the unconditional
   `Ranox_SECRET_DUNGEON_LOCK` sprite is being drawn over more buttons than
   just Secret Hell's.
3. **Only then instrument.** Add a short-lived `SERV_IRUHADEV_RANOXDBG_DUNGEON`
   flag logging, per Ranox `DUNGEON_ID`, whether `GetDungeonData` returned
   NULL and what `IsActiveDungeon` decided. Use `ErrorLogMsg`, not
   `CX2OfflineLog::Server` (see Starting Context's instrumentation note). One
   play-test then settles it.

**Do not "fix" `GetNextDungeon`** (`X2DungeonManager.cpp:718-728`), which
ignores `m_vecExtraRequireDungeonID` and so never announces the next dungeon.
March's own Sander chain (`DungeonData.lua:12053-12058`, `:13011-13016`) uses
the identical mechanism — pre-existing engine quirk; rewriting Ranox's rows to
scalar `requireDungeonID` would diverge from the studio's own data pattern.

**Verify:** after Phase 4's fix, re-test the dungeon tab directly. If still
locked, apply steps 2-3, rebuild, redeploy, re-test.

---

## Phase 6 — Cobo Express into Silent Night Shelter refused [issue 6]

**Paste with:** Starting context + this section.

Fully localized. `Handler_EGS_WARP_BY_BUTTON_REQ`
(`X2Lib/Offline/Handlers_Social.cpp:2517-2608`) refuses at `:2569-2580` when
either endpoint's **zone** is 0:

```cpp
const int iSrcZone  = pMapData->CheckCOBOExpressTicketMapID( kUnit.m_iLastPos );
const int iDestZone = pMapData->CheckCOBOExpressTicketMapID( iDestMapID );
if( kUnit.m_iLastPos == iDestMapID || 0 == iSrcZone || 0 == iDestZone )
```

There is no pair table — validity is "both zones non-zero and src != dest."
`CheckCOBOExpressTicketMapID` is a Lua **if/elseif zone ladder** in
`MapData.lua` (loaded archive-first via `X2OfflineMapData.cpp:95/123`; the
success line in the log proves it ran). The ladder ends at `VMI_RANOX → 15`
and has **no branch for `VMI_BATTLE_FIELD_RANOX_REST_00` (1007)** nor for
`VMI_BATTLE_FIELD_RANOX_FIELD_01..04` — `march2014/data036/MapData.lua:368-454`
and the clean original `KncWX2Server/ServerResource/US/MapData.lua:526-657`.

**Fix (Lua), following the Sander 13/14 precedent verbatim** — village + its
fields share a zone, the rest area gets the successor zone:

```lua
	elseif iMapID == VILLAGE_MAP_ID.VMI_RANOX
		or iMapID == VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_01
		or iMapID == VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02
		or iMapID == VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_03
		or iMapID == VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_04 then
		return 15;
	elseif iMapID == VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_REST_00 then
		return 16;
	end
```

**Zone 16 is safe to introduce** (15 was the previous maximum). Every C++
consumer — `Handlers_Social.cpp:2571-2582`, `Handlers_Inventory.cpp:741-759`,
`X2OfflineMapData.cpp:165-184` — only compares the zone against `0` and
forwards it; nothing indexes a table or bounds a loop by it.

**Fix the cost function in the same pass, or you trade a refusal for a
silently free warp.** `ComputeCOBOExpressTicketCost` at
`march2014/data036/MapData.lua:456-462` is a **broken decompile** — it
references `slot6`, which is never assigned. Once the zone rows exist,
execution reaches the cost call (`Handlers_Social.cpp:2580`), the Lua errors,
`lua_tinker::call<int>` yields 0, and the handler reads that as a free warp.
Restore the function from the un-decompiled original at
`KncWX2Server/ServerResource/US/MapData.lua:642+`
(`d = math.abs( iDesID - iSrcID )`); this gives the village↔shelter hop a
distance of 1. Scope of the decompile damage is **exactly this one
function** — an audit of every `slot<N>` in `march2014/data036/MapData.lua`
against its assignments finds `CheckCOBOExpressTicketMapID` clean.

`X2Lib/Offline/X2OfflineLuaEnumSeed.h:530-540` confirms the IDs: `1007` =
`VMI_BATTLE_FIELD_RANOX_REST_00`, `20008` = `VMI_RANOX`. Only the packed
`march2014/data036/MapData.lua` affects runtime (archive-first);
`KncWX2Server/ServerResource/US/MapData.lua` is the source-of-correct-text
copy, editing it is optional bookkeeping.

**Verify:** pack, extract, confirm bytecode. Play-test: Cobo Express reaches
Silent Night Shelter from Lanox town and back, for a sane, non-zero cost.

---

## Phase 7 — Client crash entering Ashen Land [issue 7]

**Paste with:** Starting context + this section.

Three work items. **Do 7b before 7a** — once the assets are packed the crash
disappears and the NULL-guard becomes unverifiable, i.e. you'd ship an
untested guard. Order: implement 7b → build/deploy → enter Ashen Land →
confirm `ErrorLogMsg` lines appear, the client does **not** crash, and the
affected NPCs are simply absent → *then* pack the 18 assets and confirm they
appear.

**7b — degrade instead of faulting (do this first).** A missing mesh must not
be able to crash the client. This is studio code in `KTDXLIB`, so it needs
its own `SERV_IRUHADEV_*` flag in `KTDXLIB/Always.h`, NULL-checking the
`OpenXSkinMesh` result at the fault site, logging via `ErrorLogMsg`, skipping
the NPC, keeping the original deref in the `#else`. Follow the shape of the
existing `SERV_IRUHADEV_RADIOBUTTON_FONT_DEGRADE` guard
(`KTDXLIB/KTDGUIRadioButton.cpp:251+`).

*First step of 7b is locating the deref site, not writing the guard.*
`State.log` ending at `Motion_RAGE_BABYFIG.x` tells you how far execution
got, not which pointer was dereferenced. Walk
`CKTDXDeviceManager::OpenXSkinMesh`'s callers on the battlefield
NPC-creation path and find the one that uses the result without a NULL
check; guard *that*, not the loader itself.

**7a — the missing assets (the actual cause, once 7b is verified).**
`ErrorLog.txt` lists 18 absent files while building battlefield `40029`
(`Ranox_Field_00`): 9 `MOTION_*.X` plus their 9 `NUI_LOW_*_FD_A.XET` siblings
for `MAG_CHARGER`, `MAG_NOM`, `RAGE_BABYFIG`, `RAGE_DRYAD`,
`SMITH_LITTLEWORKER`, `SMITH_MACHINE`, `SMITH_RUNNER`, `SMITH_SCIENCE`,
`SMITH_WORKER`, logged by `KGCMassFileManager::LeaveLastErrorLog` /
`CKTDXDeviceXSkinMesh::_Load` / `CKTDXDeviceXET::_Load`. First determine
whether these were never ported or ported-but-not-packed, by diffing the
June2017 `dataNNN_kom` trees against `march2014/update/dataNNN/assets.txt`;
then port/pack them.

**7c — the atlas is invalid, not merely unmerged.** `ErrorLog.txt` reports
`DLG_UI_BUTTON17.TGA` failing with `D3DXERR_INVALIDDATA` — a *stronger*
failure than `LANOX_TROUBLESHOOTING.md`'s "RANOX_ICON not merged yet" entry:
the file is present and malformed, not simply missing. Re-export
`march2014/update/datai020/DLG_UI_Button17_A.tga` in a format D3DX accepts
and repack. **Diagnose the format cause (bit depth / RLE compression / origin
bit / non-power-of-two) before starting Phase 4's atlas merge** — Phase 4
merges into `DLG_UI_Button20_NEW.tga` through what is presumably the same
export path, and without this finding it risks producing a second invalid
atlas. Compare a working March atlas's TGA header against
`DLG_UI_BUTTON17.TGA`'s byte-for-byte.

**Verify (per the ordering above):** build/deploy after 7b alone, play-test —
enter Ashen Land, confirm no crash + `ErrorLogMsg` lines in `ErrorLog.txt`.
Then pack 7a's assets + 7c's re-exported TGA, extract/confirm, play-test again
— Ashen Land loads clean, `ErrorLog.txt` gains no new entries for this
battlefield.

---

## Phase 8 — No signboard at the Lanox end of Behemoth Crater [issue 8]

**Paste with:** Starting context + this section.

**There is nothing in the data to fix; this phase is a verification step,
not a code change.** June2017's `BATTLE_FIELD_DIRECTION_INFO` /
`DIRECTION_NPC_ID` block has **zero readers** in this build:
`CBattleFieldData::ParsingScriptFile` (`X2Lib/X2BattleFieldManager.cpp:101-166`)
reads only `WORLD_ID`, `RETURN_VILLAGE_ID`, `STANDARD_MONSTER_LEVEL`,
`MAX_NUMBER_OF_MONSTER_IN_THIS_BATTLE_FIELD`, `BATTLE_FIELD_NAME`,
`BATTLE_FIELD_TEXTURE_NAME/_KEY`, `BATTLE_FIELD_PORTAL_MOVING_INFO`,
`BATTLE_FIELD_RISK_INFO`, `BATTLE_FIELD_MIDDLE_BOSS_INFO`,
`IS_BOSS_RAID_FIELD`, `RAID_FIELD_PORTAL_POSITION_INDEX`. Grepping `X2Lib/`
and `KncWX2Server/` for the key returns nothing;
`march2014/data036/BattleFieldData.lua:2021-2033` already documents the
deliberate omission. `NUI_BILLBOARD` (`X2UnitManager.h:2501`) is the village
quest board — there is no sign/guide NPC class in this build.

The physical board at a field's end is **scene geometry in the map mesh**,
not a data-placed object. The only in-engine "points elsewhere" mechanism is
the `QUEST_GUIDE` indicator, `CX2NPCIndicator::GetLinkInfo`
(`X2Lib/X2NPCIndicator.cpp:531-568`), fed from
`GetBattleFieldPortalMovingInfo` — **and its data is already correct**:
`march2014/data036/BattleFieldData.lua:1704-1712` gives SANDER_FIELD_04 a
`PMT_MOVE_TO_VILLAGE` portal with `PLACE_ID_TO_MOVE = VILLAGE_MAP_ID.VMI_RANOX`
on `LINE_NUMBER = 124`.

**Resolution, already localized:**

- `QUEST_GUIDE` **is** compiled in — `KTDXLIB/Always.h:2289` defines it
  unconditionally, so `CX2NPCIndicator` is live.
- `CX2NPCIndicator::SetShow` (`X2NPCIndicator.cpp:511-530`) forces
  `bVal = false` whenever `GetUIQuestNew()->m_bUserShowQuickQuestDLG` is
  false, and that member **initializes to `false`**
  (`X2UIQuestNew.cpp:28`). The guide particle is off until the player
  toggles the quick-quest window — original behaviour, not a regression.
- In `XS_BATTLE_FIELD`, `GetLinkInfo` (`:553-569`) reads
  `GetBattleFieldPortalMovingInfo` and places a marker at
  `GetLineData(LINE_NUMBER)->startPos + 320.f` — line 124 for
  SANDER_FIELD_04, i.e. Lanox. The data is already right.

**Step 1 (no edits):** in Sander Field 04, toggle the quick-quest window on
and look for the guide particle near the field's end. If it appears, the
engine-side pointer works and nothing in code or data is broken.

**Step 2:** the physical signboard is then **scene geometry the user owns**
— it lives in the Behemoth Crater map mesh, not in any table this client
reads. Record that as the phase outcome; there is no code fix that would be
anything but a band-aid. Do **not** add `BATTLE_FIELD_DIRECTION_INFO` rows
(nothing reads them), and do **not** weaken the `m_bUserShowQuickQuestDLG`
gate to force the particle on — that changes behaviour for every field in
the game, not just Lanox's.

**Verify:** the quick-quest toggle check above, no rebuild needed.

---

## Phase 9 — Leftmost Lanox teleport lands at the crater's start, not end [issue 9]

**Paste with:** Starting context + this section.

**This is an offline-emulator bug, not Lanox data — the fix repairs every
village→field entry, not just this one.**

The Lua is correct and must not be changed.
`march2014/data034/DLG_Map_Village.lua:857+` gives Lanox's west exit
`StartPosId = 142`, `BattleFieldStartLineIndex = 177`,
`BattleFieldId = VMI_BATTLE_FIELD_SANDER_FIELD_04`; in
`march2014/data040/Field_Sander_Stage3_LineMap.lua`,
`AddStartPos(177, …, false, 122)` resolves to line 122 — the field's **end**,
exactly where the player should arrive.

The chain:

1. `CX2StateField::MoveFromVillageToBattleField`
   (`X2Lib/X2StateField.cpp:1910-1925`) sends the two-arg
   `Handler_EGS_JOIN_BATTLE_FIELD_REQ` (`X2Lib/X2State.cpp:11278-11290`),
   which sets **`m_bNowBattleFieldPositionInfoStartPosition = true`**
   (`:11288`) — meaning "`m_iStartPosIndex` is an `AddStartPos` key, not a
   raw line index." The field→field portal overload (`:11236-11246`) sets it
   `false` at `:11245`.
2. **The bug:** `CX2OfflineServer::Handler_EGS_JOIN_BATTLE_FIELD_REQ`
   (`X2Lib/Offline/Handlers_Room.cpp:1278-1281`, same shape again at
   `:1365`) copies only `m_iBattleFieldID` and `m_iStartPosIndex`. It never
   echoes `m_bNowBattleFieldPositionInfoStartPosition` or
   `m_usBattleFieldPositionValue`, and `Initialize()`
   (`KncWX2Server/Common/CommonPacket.h:8751-8793`) has already zeroed both.
3. The client applies the zeroed values verbatim (`X2State.cpp:11338-11341`,
   dup at `:11386-11389`), so `X2BattleFieldGame.cpp:1413-1417` takes the
   `InitPosByBattleFieldPosition()` branch meant for portal re-entry.
4. That reinterprets 177 as a **line index** (`X2GUUser.cpp:2703-2730`). The
   linemap has 147 lines (max `LINE_NUM` 146), so
   `CKTDGLineMap::GetLineData(177)` returns NULL (`KTDGLineMap.h:1134-1142`),
   the whole `if` body is skipped, and `m_FrameDataFuture.syncData.position`
   is never written — the player keeps the default position, i.e. the
   field's start.

Every other exit uses a low key (Sander→Hamel Field 04 = 3, Sander→Sander
Field 01 = 1, Lanox→Ranox Field 01 = 2) that coincidentally also names a low
line near the field's beginning, which is where the player belongs anyway.
Lanox's west door is the **only** exit whose intended arrival is a field's
*end*, which is why the bug has lain dormant.

**Fix:** in `X2Lib/Offline/Handlers_Room.cpp`, at both ACK builders
(`:1278-1281` and `:1365`), echo `m_bNowBattleFieldPositionInfoStartPosition`
and `m_usBattleFieldPositionValue` from `kReq`. This also un-breaks the
portal-re-entry path, which loses its position value the same way.

**Marking: match the neighbours, don't invent a flag.** `X2Lib/Offline/` is
entirely ours and already lives inside `SERV_IRUHADEV_OFFLINE`; the
flag-per-change rule exists to keep *studio* files revertible. Before
editing, check how the nearest existing fixes in `Handlers_Room.cpp` are
marked — a nested `SERV_IRUHADEV_*`, or a plain `//{{ Iruha : <date> // …`
block — and match that style. (Phase 7b is the opposite case: `KTDXLIB` is
studio code and definitely gets its own flag.)

**Verify:** build/deploy, play-test: take the leftmost Lanox teleport into
Behemoth Crater, confirm arrival at the field's *end* (near line 122), not
its start.

---

## Verification — batching

**Batch the play-tests — each one costs a "may I launch?" round-trip.** Three
batches, not nine:

1. **Lua batch** (phases 1, 2, 3, 4, 6): one `compile-batch` + repack + one
   play-test covering Lanox common NPCs, Edel/Ignia's dialog buttons,
   Steel/Faceoff/Syota's shops, the dungeon-list tab, the Cobo hop to Silent
   Night Shelter — and Elder, for Phase 1's scope check.
2. **C++ batch** (phases 7b and 9): one build/deploy/play-test — enter Ashen
   Land (expect `ErrorLogMsg` lines, no crash) and take the leftmost Lanox
   teleport (expect the crater's end).
3. **Asset batch** (7a, 7c, and Phase 4's atlas merge): repack, one
   play-test.

Phase 5 may need a fourth pass once its instrumentation lands, but only if
batch 1 doesn't already resolve it via Phase 4.

For every batch: `elstools compile-batch -v mar-2014` → repack → **extract
the live `.kom` and confirm the bytes** (never trust `kom_contents/` staging)
→ for C++ changes, `touch X2Lib/stdafx.cpp` → build in order
`X2ServerProtocol`(X2TOOL) → `KTDXLIB` → `X2Lib` → `X2` at `US_SERVICE` →
confirm `X2/US_SERVICE/x2.exe` is newly dated → copy to
`$DATA/X2_offline.exe` (confirm programmatically) → **ask before launching**.

When confirming a packed Lua entry, check it's **compiled bytecode**, not
plaintext: a plaintext `.lua` inside a `.kom` loads only through
`DoMemoryNotEncript`, and only if it kept its UTF-8 BOM — a stripped BOM
presents as the loader's own success line simply vanishing from
`offline_server.log`, not as an error. Preserve BOM and CRLF on every
hand-edit.

**Acceptance, defect by defect:** Camilla/Ariel/postbox/bank/billboard
visible in Lanox; Edel's dialog shows no dead Event buttons and Ignia shows
its intended ones; Steel/Faceoff/Syota shops list stock; a Lanox tab appears
in the dungeon list; BurnedForest is selectable at level ≥ 68; Cobo Express
reaches Silent Night Shelter; Ashen Land loads without crashing and
`ErrorLog.txt` is clean of the 18 assets; the Behemoth Crater quest-guide
particle appears when toggled on (signboard itself is out of scope — art
task); the leftmost Lanox teleport lands at the crater's end.
