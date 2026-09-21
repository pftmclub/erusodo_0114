# Lanox/Ranox Troubleshooting

Living checklist for getting Lanox fully playable (field, village, rest area,
dungeons, dungeon tab, Cobo Express, houses, PVP). Tick items off and annotate
them as they're verified or as new defects are found — this is not a
narrative, it's the reference to consult *instead of* re-deriving root causes
`RANOX_PORT_PLAN.md` and this document have already established.

**Do not re-open** `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md`,
or the `mods/lanox-offline` branch — declared a failed prior attempt, per
`RANOX_PORT_PLAN.md`'s own "Do not consult" section.

**Standing rule established 2026-09-19 (Phase 23)**: every `.lua` must be
compiled with `elstools compile-batch -v mar-2014` before packing, and
`.src.lua`/`.std.luac` decompiler byproducts must never ship inside a `.kom`
archive. 267 files across 8 archives were found packed as plain, uncompiled
text — this was the dominant reason Lanox looked "half-working" (data tables
that go through `DoMemory`'s not-encrypted fallback loaded fine; direct
`Info->pRealData` reads, like scene/world construction, silently got garbage).
If Lanox regresses after a repack, check this first: a header sweep of every
`.lua` under `kom_contents/` for the `1B 4B 4C 81` mar-2014 magic bytes.

---

## The three-layer rule

Every Lanox feature that "should just work because the value is right there"
turns out to need three separate things, and missing the second or third fails
**silently** while missing the first usually fails loudly (compile error,
`ASSERT`, or an engine-gap the port already catalogued):

1. **The C++ enum member or switch case** — e.g. `CX2LocationManager::LMI_RANOX`,
   `IsValidWarpField`'s `case SEnum::VMI_RANOX`, `HOUSE_ID::HI_EDEL`.
2. **The Lua enum value** — the matching entry in `march2014/data036/Enum.lua`
   (or `DungeonEnum.lua`), reachable by the client's Lua state.
3. **The registry-file row** that actually wires the two together for a
   specific content instance — `BattleFieldData.lua`'s portal info,
   `DLG_Map_Village.lua`'s `Village[id]` block, `WorldData.lua`'s
   `AddWorldData`, `MapData.lua`'s `AddLocalMapInfo`, `DLG_Destination*.lua`'s
   named controls.

A gap in (1) tends to be a compile error or an `ASSERT`. A gap in (2) or (3)
is a `nil` in Lua, which this codebase's `LUA_GET_VALUE` macros almost always
default away quietly — the result renders or half-renders with no log line at
all. **When something is visible but inert, or invisible with no error, look
for a missing (2) or (3) before assuming (1) is the problem.**

---

## Symptom → root-cause table

Seeded from 2026-09-19's investigation (Phases 23–29). Each entry is a *class*
of failure, not a one-off — expect it to recur as more of Lanox gets exercised.

| Symptom | Root cause | Where to look |
|---|---|---|
| A label is blank everywhere it should show text | The `STR_ID_n` is above `StringID_def.lua`'s ceiling (this build only defines up to a fixed max; anything higher is Lua `nil` → `STR_ID_EMPTY` → empty string) | `march2014/data036/StringID_def.lua` — is the ID's number `= n` present? Cross-check `general.ess` (loose file in the game dir, XOR key 16) actually has the text at that index |
| A UI element is visible but clicking does nothing | A `Get*()` call returned a valid-looking object whose **name field** is empty, and the handler early-returns silently on an empty string | `CX2StateMenu::UseWarpPopup` is the worked example: `if( strVillageName.empty() ) return;` — no popup, no packet, no log line. Check whatever populates that name (usually a `STR_ID` from cause #1 above) |
| A list is missing entries, including ones that should already work | A `nil` hole in a Lua array **truncates** a `while( GetValue( tableIndex, x ) )` parse loop — it does not skip the hole and continue | Any `HouseIDList`/`DungeonIDList`/`BattleFieldIDList`/`NPC_ID_LIST`-shaped array. Check every name in the array resolves to a real enum member, in order — the first undefined one and everything after it is silently dropped |
| No portal/warp appears at a map edge that should have one | The field's `BATTLE_FIELD_PORTAL_MOVING_INFO` table has no row for that exit, or the row exists but the linemap's matching `LINE_NUM` is still `LINE_TYPE.LT_NORMAL` instead of `LT_POTAL` | `march2014/data036/BattleFieldData.lua` for the row; the field's own `*_LineMap.lua` for the `LINE_TYPE` flag at that `LINE_NUM` |
| A warp/teleport completes server-side (ACK received, log shows the transition) but nothing visibly changes | `CX2LocationManager::IsValidWarpField()` has no `case` for the destination — `Handler_EGS_WARP_BY_BUTTON_ACK` silently no-ops if it returns false | `X2Lib/X2LocationManager.cpp`, the `IsValidWarpField` switch, hardcoded per-destination |
| A warp/teleport shows a fast "moving to village map" popup and nothing else — no crash, no map | `CX2State::Handler_EGS_STATE_CHANGE_FIELD_ACK` only fires `XGM_STATE_CHANGE` if `GetWorldID(villageId)` resolves in-range; `GetWorldID` reads `m_mapWorldMap`, populated only from `DLG_Map_Village.lua`'s `Village[id].WORLD_ID` — a **third**, easy-to-forget shared registry file, separate from the per-region `DLG_Map_Local_*.lua` | `march2014/data034/DLG_Map_Village.lua` — does `Village[VILLAGE_MAP_ID.VMI_X]` exist with a `WORLD_ID` field? |
| A ported key/field is present in the Lua but the C++ parser never reads it | The client's own field name differs from the reference (June2017) client's — June renamed some fields between versions and the port copied the new name verbatim | Confirmed instance: `OtherPlaceID`/`OtherPlaceStartLineIndex` (June) vs. `BattleFieldId`/`BattleFieldStartLineIndex` (this build) in `DLG_Map_Village.lua`'s `START_POS` entries. Diff the field *names*, not just values, against a March-native block that already works (e.g. an existing Sander entry) |
| The minimap/location state says you're somewhere but the 3D scene doesn't render | Packed `.lua` is plaintext instead of compiled bytecode (see the standing rule above) — data-table loads go through a not-encrypted fallback and still work; direct scene/world reads don't | Header-sweep every `.lua` this feature touches for the `1B 4B 4C 81` magic |
| A new C++ code path (flag-gated) seems to have no effect after a rebuild | The PCH silently discarded the header edit | `touch X2Lib/stdafx.cpp` before rebuilding, and if still no effect, grep the `.obj`/`.exe` for a literal that only exists inside the new `#ifdef` block — exit code 0 is not proof the code compiled in |

---

## Reachability checklist

Check off in order — most surfaces gate on the one above it.

- [ ] **String IDs load** — `march2014/data036/StringID_def.lua` (June2017's
      superset, adopted 2026-09-19) is packed into `data036.kom`. No visible
      proof by itself; verified indirectly by every later Lanox label showing
      real text instead of blank.
- [ ] **Lanox village renders** — from character select or `lanox` in
      chatbox (`SERV_IRUHADEV_CHAT_WARP_DEBUG`), the 3D scene appears (not
      just the minimap). If not, see "packed as plaintext" in the symptom
      table above first.
- [ ] **World Map pin** — Menu → World Map, after clearing
      `DI_SANDER_BEHIMOSS_HEART_NORMAL` at level 68+: Lanox town + 4 field
      pins + rest-stop pin are visible and clickable
      (`march2014/data034/DLG_UI_World_Map_NEW.lua`, Phase 22).
- [ ] **Behemoth Crater → Lanox portal** — stand at Sander Field 04's far
      edge (`LINE_NUM 124`), walk into it; transitions to Lanox village
      (`BattleFieldData.lua` + `Field_Sander_Stage3_LineMap.lua`, Phase 26).
- [ ] **Lanox village → its own fields** — walk out both village exits to
      Field 01 and back to Behemoth Crater; from the rest stop, to Fields 02
      and 03 (`DLG_Map_Village.lua`'s renamed `BattleFieldId` keys, Phase 27).
- [ ] **Cobo Express (B menu)** — Lanox town + rest-stop rows show icon and
      label (icon needs the atlas merges below; label needs Phase 25's
      `StringID_def.lua`), clicking selects, confirm warps
      (`CX2StateMenu::CreateWarpDest`, Phase 21 + `IsValidWarpField`
      Post-Phase-22 fix).
- [ ] **In-village warp NPC** — same as Cobo Express but via
      `CX2UIInventory::CreateWarpDest`/`DLG_Destination.lua` (Phase 29) —
      independently broken until this phase landed; check separately.
- [ ] **Dungeon tab** — all 6 Lanox lines (AshesVillage, BurnedForest,
      FinalHall, FireTemple, Geyser, GateWay) + Secret Hell listed with real
      names at Normal/Hard/Expert.
- [ ] **Each dungeon line enters and completes** — monsters spawn, clear
      reward pays out. AshesVillage first (gap-free per `RANOX_PORT_PLAN.md`
      Phase 3), then the rest.
- [ ] **Houses/NPCs render in town** — 18 `HouseIDList` entries, no longer
      truncated at `HI_EDEL` (Phase 28). 4 of the 18 (`HI_STORY_QUEST`,
      `HI_TRANSCEND_QUEST`, `HI_EVENT_GUILD_EDAN_VILAGE`,
      `HI_EVENT_BLESS_OF_SOLACE`) will render with **no associated NPC** —
      known, degrade-safe, see Known-open items.
- [ ] **PVP GateWay map** — Phase 11's content, not re-verified this session.
- [ ] **Spa** — out of scope; no infrastructure exists in this build for
      *any* village's spa, not just Lanox's. Do not attempt.

---

## Known-open items

Carried forward by name — do not re-derive, do not invent values for these:

- **`RANOX_ICON` atlas merge** — `data056`/`datai020`'s `DLG_UI_Button17`
  pair. Without it, Cobo Express and the in-village warp dialog's Lanox rows
  show a blank swatch instead of an icon. Source pixels are staged at
  `march2014/update/data056/` and `march2014/update/datai020/` — **not**
  pack-ready as-is, needs a real atlas merge (adding one region, not a file
  swap) via the studio's own content tooling. Cosmetic only, confirmed by
  reading `AddWarpList` — the icon and the click target are unrelated
  controls.
- **`TOWN_RANOX`/`FIELD_NAME_RANOX_*`/`FIELD_REST_RANOX` atlas merge** —
  `data057`'s `DLG_UI_Common_Texture71_NEW`. Same shape as above, source
  staged at `march2014/update/data057/`. Affects the World Map pin's own
  icon, not its clickability.
- **World Map pin coordinates** — ported verbatim from June2017's raw
  `D3DXVECTOR3` values; most nodes' coordinates are identical between the two
  clients' canvases but a few aren't, with no consistent transform. Flag if
  Lanox's pin renders visually wrong (over ocean/existing terrain) — would
  need per-node correction against the actual background art, not a formula.
- **Elysion forward-references, deliberately omitted**: Field 03's second
  portal + direction-NPC block (`BattleFieldData.lua`), and `HI_GUILD_MARK`'s
  `COMMON_NPC_LOCATION` row for `WI_VILLAGE_ELYSION` (`DLG_Map_House.lua`,
  Phase 28). Elysion is a future, unported content chapter — out of this
  port's scope entirely.
- **4 houses render without their NPC** — `HI_STORY_QUEST`,
  `HI_TRANSCEND_QUEST`, `HI_EVENT_GUILD_EDAN_VILAGE`,
  `HI_EVENT_BLESS_OF_SOLACE` each reference an `NPC_UNIT_ID` this build
  doesn't define (`NUI_STORY_QUEST`, `NUI_TRANSCEND_QUEST`,
  `NUI_EVENT_GUILD_EDAN_VILAGE`, `NUI_EVENT_BLESS_OF_SOLACE` — none of
  Appendix B's 135-name port list covers these; they're quest/event markers,
  not combat monsters). Confirmed degrade-safe: `HouseParsing`'s
  `NPC_ID_LIST` parse is the same truncation-on-`nil` class as `HouseIDList`,
  so it just reads zero NPCs for that house rather than erroring. Add the 4
  `NPC_UNIT_ID` values (verbatim from June2017's `Enum.lua`) in a future
  phase if these NPCs are wanted.
- **Spa** (`VMI_SPA_RANOX`) — no spa infrastructure exists in this build for
  any village. `DLG_Map_Village.lua`'s `IsSpaEnter`/`OtherPlaceID` pair for
  Lanox's spa start-pos was deliberately left with June's field names rather
  than renamed — renaming it would make the parser treat it as a real
  battlefield exit to a destination (`VMI_SPA_RANOX`) that has no
  `AddBattleFieldData` row, which is worse than the current safe no-op.
- **Hero difficulty tier** — deferred, no content files exist for it in
  either client (registry-only variant, presumably server-side scaling).
- **Server-side monster stat/drop tables** — per `CLAUDE.md`'s standing rule,
  named for the user to pull from the live DB / pack, never invented here.
- **`X2PartyUI.cpp:1940`'s field-name-icon switch has no Ranox case** —
  cosmetic (party-UI location icon), falls to a dead `default: ASSERT` in
  this NDEBUG build (gated behind `SERV_CHINA_ADVENTURE`, never defined
  anyway per Phase 20's finding — so this switch is entirely unreachable in
  this build regardless). Not worth fixing.
- **`DLG_Map_House_1.lua` is a byte-for-byte duplicate of `DLG_Map_House.lua`
  in both clients and is never loaded by any C++ call site** — confirmed by
  reading `X2LocationManager.cpp:143` (only `DLG_Map_House.lua` is
  `LoadAndDoMemory`'d). Dead file in both March2014 and June2017; don't spend
  time on it if it comes up again.

---

## How to debug the next one

1. **The `lanox` chat-warp debug tool** (`SERV_IRUHADEV_CHAT_WARP_DEBUG`,
   `X2Lib/X2ChatBox.cpp`) — type `lanox` in chat, Enter. Free warp straight to
   `SEnum::VMI_RANOX`, bypassing Cobo Express and the World Map entirely.
   Isolates "can this feature reach Lanox at all" from "is the UI for
   reaching it broken."
2. **Read both offline logs**, in the game directory, flushed per line:
   `offline_packets.log` (packet-level: `HANDLED`/`IGNORED`/`UNHANDLED`/
   `EXCEPTION`) and `offline_server.log` (lifecycle + one line per decision,
   including `WARP`/`FIELD` lines). **A clean log (zero `UNHANDLED`/
   `EXCEPTION`) is not evidence of success** — every defect found in this
   investigation produced zero log anomalies, because the failures are all
   pure client-Lua/C++ logic, not packet handling.
3. **When code-reading stalls, add a diagnostic rather than reasoning
   further**: `CX2OfflineLog::Server( L"..." )` writes an immediately-flushed
   line to `offline_server.log` from anywhere in `X2Lib`. Gate it behind a
   short-lived `SERV_IRUHADEV_*_DEBUG` flag, tag lines with a grep-able
   prefix, and remove the flag once the question is answered.
4. **Re-verify line numbers and prior claims before building on them** — this
   investigation caught its own earlier phases wrong twice in one session
   (a filename claim in Phase 20, the "cosmetic" verdicts in Phases 21/22).
   Read the actual file/line cited, don't trust the citation.
5. **Encoding discipline** — `X2LocationManager.h`, `X2UIInventory.cpp`,
   `X2ChatBox.cpp`, `Always.h` all carry non-ASCII (CP949/ISO-8859) bytes.
   Never use a text editor's plain save on these; use byte-anchored Python,
   and check `file <path>` + `git diff --stat` after every edit — a normal
   Read-then-Edit round-trip has silently re-encoded a whole file to UTF-8 on
   this project before (see `KTDXLIB/Always.h`'s history).

---

## Build/deploy verification, this pass (2026-09-19)

- `X2ServerProtocol` (X2TOOL), `KTDXLIB` (US_SERVICE) -- up to date, no
  rebuild needed this session (no header either project depends on changed
  today).
- `X2Lib` (US_SERVICE) -- full rebuild after `touch X2Lib/stdafx.cpp` (edited
  `X2LocationManager.h`). Exit code 0, 0 errors (203 pre-existing warnings,
  same `PurpleForClient*.lib` duplicate-symbol class as every prior phase's
  build). `X2/X2Lib.lib` freshly dated. Confirmed the `SERV_IRUHADEV_RANOX_PORT`
  block genuinely compiled into `X2UIInventory.obj` (Phase 29's own new call
  site for this flag) by grepping the `.obj` for the `Village_20008` UTF-16LE
  literal, rather than trusting the exit code alone.
- `X2` (US_SERVICE) -- linked clean, exit code 0, 0 errors (81 warnings, same
  `KncSecurity71MD`/Purple duplicate-symbol/missing-PDB class as always).
  `X2/US_SERVICE/x2.exe` (18,806,784 bytes) confirmed to contain both
  `Village_20008` and `Village_1007` UTF-16LE literals post-link.
- Deployed to the game directory as `X2_offline.exe`, confirmed
  programmatically (source/destination size match, mtime advanced). **Not
  launched** -- per standing rule, ask before every launch of
  `X2_offline.exe`, every time.
- **Not yet run**: the actual play-test. All of Phases 23-29's Lua output
  (the 267-file compile fix plus every new/patched file in the packing table
  below) still needs packing by the user before any of this build's fixes
  are observable in-game.

Packing precondition for this pass:

| Archive | Files |
|---|---|
| `data034.kom` | `DLG_Map_Village.lua`, `DLG_UI_World_Map_NEW.lua`, `DLG_Map_House.lua`, `DLG_Destination.lua` |
| `data036.kom` | `StringID_def.lua`, `BattleFieldData.lua`, `Enum.lua` |
| `data040.kom` | `Field_Sander_Stage3_LineMap.lua` |
| `datai014.kom` | `DLG_Destination_byButton.lua` (already patched, Phase 21 -- repack if not already done) |

Plus the two still-outstanding atlas merges (`data056`/`datai020`,
`data057`) -- cosmetic-only, not blocking reachability. And separately: the
267-file plaintext-to-bytecode fix (Phase 23) was applied directly inside the
existing packing workspace (`kom_contents/`), so it needs no separate list --
it's already reflected in whatever archives get repacked next.

---
## Follow-up session (2026-09-19, later same evening) -- packing confirmed correct, real bug found

User confirmed String IDs load (Phase 25 verified). Two new symptoms reported:
Behemoth Crater portal completes its loading bar but lands in Ruben; Cobo
Express "not working" for Lanox town and its rest area; the dungeon tab does
not show Lanox.

**First, a false lead ruled out.** Every Phase 23-29 Lua file was still
plaintext in `kom_contents/` after the user's repack -- looked exactly like
the Phase 23 bug recurring. It was not: the user's packing tool compiles
on the fly during packing, so `kom_contents/` staying plaintext is expected.
Extracted the live `.kom` archives directly with `elstools extract` and
confirmed all 8 files are genuine, correctly-sized mar-2014 bytecode inside
the actual packed archives. **Lesson for next time: always verify by
extracting the live `.kom`, never by reading the packing-workspace staging
directory** -- the two can legitimately disagree.

**Sharper statement of the `DoMemoryNotEncrypt` rule** (correcting Phase 23's
version): a plaintext `.lua` loads through `KLuaManager::DoMemoryNotEncrypt`
**only if it starts with a UTF-8 BOM** (`EF BB BF`) -- without one, the
function's own ASSERT-guarded check returns `false` immediately, with no log
line. A BOM-less plaintext file and a genuinely corrupt file fail identically.

**Root cause of the Behemoth Crater "deported to Ruben" symptom, found by
reading code, not by guessing further:**
`CX2StateBattleField::Handler_EGS_STATE_CHANGE_FIELD_REQ( uiVillageId_, usBattleFieldStartLineIndex_ )`
(`X2StateBattleField.cpp:909`) is a **virtual override** of
`CX2State::Handler_EGS_STATE_CHANGE_FIELD_REQ(int)` with a completely
different implementation -- easy to miss since the base class's 1-argument
version (which does look up `GetVillageMapTemplet`) is NOT what runs while
standing in a battlefield. The override:

```cpp
int iMapId = g_pData->GetLocationManager()->GetMapId( usBattleFieldStartLineIndex_ );
if ( iMapId <= 0 )
{
    iMapId = SEnum::VMI_RUBEN;   // <-- silent fallback, no popup
    ...
}
```

**It never reads `uiVillageId_` (the actual destination, e.g. `VMI_RANOX`) at
all.** The destination is derived entirely from `GetMapId(startPosId)`, which
looks up `m_mapVillageMap[startPosId]` (populated only inside
`VillageParsing`'s `START_POS` loop) and returns the **village that StartPos
belongs to** -- not the portal's own `PLACE_ID_TO_MOVE`. Behemoth Crater's
portal (`BattleFieldData.lua`, Phase 26) targets `POSITION_INDEX_TO_MOVE = 140`,
which is Lanox's own `StartPosId = 140` in `DLG_Map_Village.lua` -- so this
only resolves correctly if `VillageParsing( VMI_RANOX )` has already run and
inserted StartPosId 140 into `m_mapVillageMap`. If it has not, `GetMapId(140)`
returns `SEnum::VMI_INVALID` (0), and the `<= 0` check silently substitutes
Ruben -- exactly matching what was observed, with no error dialog (the one
`ASSERT`/`StateLog` in that branch fires silently in this NDEBUG build).

**This did not previously produce a visible error because the existing
per-village `MessageBox( ..., "CX2LocationManager Village OpenScript Error", ... )`
inside `VillageParsing` (X2LocationManager.cpp) is attached to the *inner*
`if( BeginTable( villageID ) )`, not the outer `if( BeginTable( "Village" ) )`
as a first read suggests** -- re-read carefully by tab-indentation level
before trusting a transcription of this function again; a wrong read of
which `if` an `else` belongs to burned real time this session.

**Cobo Express's likely-identical root cause, not yet independently
confirmed:** `Handler_EGS_WARP_BY_BUTTON_ACK` (`X2StateMenu.cpp`) calls
`pState->Handler_EGS_STATE_CHANGE_FIELD_REQ(kEvent.m_iWarpPointMapID)` --
**the same virtual call**, so while standing in a battlefield/field state,
this too resolves through `CX2StateBattleField`'s override and the same
`GetMapId`-based derivation, ignoring the intended destination. The
`offline_server.log` from this session shows `WARP button warp X -> 20008`
succeeding **four times** with `FIELD entering mapID=20008` following --
**but this is the offline emulator's own server-side bookkeeping of the
packet it received, not proof the client rendered anything.** It says the
REQUEST reached the server with the right ID (so `CX2State`'s base-class
override handled it, e.g. while standing in a *village*, not a battlefield)
-- it does not by itself tell us what happened during
`Handler_EGS_STATE_CHANGE_FIELD_ACK`'s client-side world-build gate. **Also
concrete from the same log: no warp to `1007` (Ranox rest) appears
anywhere** -- that row did not fire a request in this session, a different
failure (worth checking the `Village_1007` control and its level gate)
from whatever the town row is doing.

**Dungeon tab not investigated this session** -- separate surface,
untouched by anything in Phases 23-29; needs its own read once the warp
chain is fixed (may simply be a consequence of never actually being *in*
Lanox to see its tab, rather than a registry gap of its own).

**Instrumented rather than guessed further** (per this doc's own "how to
debug the next one" section) -- `SERV_IRUHADEV_RANOX_SCENE_DEBUG`
(`KTDXLIB/Always.h`), four `CX2OfflineLog::Server( L"[RANOXDBG] ..." )` lines,
tagged for a single `grep "[RANOXDBG]"`:

1. `X2LocationManager.cpp`, `VillageParsing`, right before
   `m_mapVillageTemplet.insert` -- logs `villageID`, `worldId`. Presence
   confirms `Village[VMI_RANOX]` parsed and what world ID it got.
2. `X2LocationManager.cpp`, `VillageParsing`'s `else` (the
   "OpenScript Error" branch) -- logs the `villageID` that failed
   `BeginTable`. **Zero-cost distinguishing test**: if this fires with
   `villageID=20008`, `Village[VMI_RANOX]` genuinely is not reachable in the
   loaded Lua at that point; if it never fires and (1) also never fires,
   `VillageParsing` is not being *called* for Ranox at all (see next line).
3. `X2LocationManager.cpp`, `LocalParsing`, right before the
   `VillageParsing` calls -- logs `localMapID`, `villageID`,
   `villageDGID`, `villageDungeonLoungeID`, `iBattleFieldRestID`. Confirms
   whether `DLG_Map_Local_Ranox.lua`'s `VillageData.ID` is even being read.
4. `X2StateBattleField.cpp`, `MoveToOtherPlace`'s `PMT_MOVE_TO_VILLAGE`
   case -- logs `GetPlaceIdToMove()`/`GetPositionIndexToMove()`, confirming
   the portal itself is feeding `140`/`VMI_RANOX` correctly (isolating the
   bug to `Handler_EGS_STATE_CHANGE_FIELD_REQ`'s override, not the portal
   data).

Built (`X2Lib` full rebuild after `touch stdafx.cpp`, then `X2`), both exit
0/0 errors; all four `[RANOXDBG]` literals confirmed present in
`x2.exe` post-link (not just post-compile). Deployed to `X2_offline.exe`,
confirmed programmatically. **Not launched.**

**One real mistake made and caught this session, worth flagging so it is not
repeated:** a byte-anchored edit meant to add a diagnostic-only `else`
branch instead produced `if {} else {} else {}` -- invalid C++ -- because the
existing `else` was mis-attributed to the wrong `if` from a first
transcription. Caught by re-deriving exact tab-indentation levels
(`len(line) - len(line.lstrip(TAB))`) before the build, not by the compiler
(the fix landed before ever invoking `cl.exe`). Do this indentation check
*before* editing any `if`/`else` chain in this codebase's C++, not after.

**Ask for the next session, in this exact order** (single clean run,
nothing else attempted, so the log isolates the answer):

1. Launch, character-select, straight into Lanox via `lanox` in chat.
2. `grep "[RANOXDBG]" offline_server.log` and paste the output. This
   alone should show whether `VillageParsing` ever succeeded for Ranox.
3. Separately: walk to Behemoth Crater and use its portal once. Then, in a
   **fresh client session** (so the log is not mixed with the above), try
   Cobo Express's Lanox row, then its rest-area row. Report exactly what
   appears on screen for each -- "nothing happens," "loading bar then back
   to the same spot," "loading bar then Ruben," etc. -- since "not working"
   covers several different, distinguishable failures in this codebase.

---
