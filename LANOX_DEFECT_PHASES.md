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
| The `SERV_IRUHADEV_FIELDNPC_EMPTY_NPCLIST_GUARD` reapplication widened behaviour | No — `git diff dcde7b3^ HEAD -- X2Lib/X2TFieldNpc.cpp` shows the `#else` arm is the verbatim original; the guard only adds `&& m_NPCList.empty() == false`. Re-checked 2026-09-22 against the *old-village* NPC reports: the guard's degrade path fires only for `HI_GUILD_MARK`/`HI_EVENT_GUILD_EDAN_VILAGE`/`HI_EVENT_BLESS_OF_SOLACE`, each of which appears exactly once in `DLG_Map_Village.lua`, inside `VMI_RANOX`'s own `HouseIDList` — it cannot fire for Elder/Bethma/Feita/Hamel/Silent Night Shelter at all, since those villages never instantiate these houses |
| A missing house block aborts `HouseParsing` (`MessageBox` + `return`, `X2LocationManager.cpp:1109-1113`) | Not triggered — every `HOUSE_ID` in every village's list has a block in `DLG_Map_House.lua` (64 blocks) |

Also, for issue 5 specifically, five more are dead (see Phase 5's own table).

### Proven root causes

- **Issue 1** (no Cobo NPCs in Lanox) → the five shared "common NPC" house
  blocks are in `VMI_RANOX`'s `HouseIDList` but have **no `WI_VILLAGE_RANOX`
  row** in their `COMMON_NPC_LOCATION` table. Luriel is Elder-only *by
  design*, not a regression. **Superseded 2026-09-22 — this was only half the
  bug.** The user has since reproduced NPCs going missing in Elder, Bethma,
  Feita, Hamel, and Silent Night Shelter too, which the Lua-row fix above
  cannot touch (those villages' rows were never missing). Root cause proven
  from a live `ErrorLog.txt` session: `SERV_IRUHADEV_RANOX_ANIM_DEGRADE`
  (`KTDXLIB/Always.h:4147`) is currently **commented out** ("Reverted
  2026-09-20: undefined pending root-cause fix"), so any NPC mesh whose
  animation-controller section fails to parse (`KEM_ERROR410`/`KEM_ERROR246`)
  fails its **entire** mesh open instead of degrading — the NPC node is still
  created but has no mesh, i.e. it's invisible. This is currently hitting
  `Motion_Npc_All_Camilla.x` and `Motion_Npc_All_Ariel.x` — not Ranox-only
  ported files — in Hamel, Elder, and Besma alike. **Not**
  `SERV_IRUHADEV_FIELDNPC_EMPTY_NPCLIST_GUARD`: its degrade path only fires
  for `HI_GUILD_MARK`/`HI_EVENT_GUILD_EDAN_VILAGE`/`HI_EVENT_BLESS_OF_SOLACE`,
  which are in `VMI_RANOX`'s `HouseIDList` only (confirmed by grep), and even
  when it fires it never gates mesh creation. See Phase 1's rewrite.
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
  **Update 2026-09-22:** the user has packed/fixed the asset problem and can
  now enter every Lanox field with no crash. New symptom found once the
  fields actually load: field monsters spawn and die immediately, suspected
  zero/undefined HP. Opened as Phase 7d — see Phase 7's rewrite.
- **Issue 8** (no signboard at Behemoth Crater's Lanox end) → not a data gap;
  no code in this build reads a direction-board table at all. The one
  in-engine "points elsewhere" mechanism (`QUEST_GUIDE` indicator) is already
  correctly fed and is merely off by default until the player opens the
  quick-quest window. **Clarified 2026-09-22:** the user means the literal
  physical signboard prop next to the field-end teleport (the "go left/go
  right" direction marker), not the quest signboard — this was already the
  correct reading; see Phase 8's rewrite for the added confirmation that
  `Field_Sander_Stage3_LineMap.lua` (Behemoth Crater's linemap) carries no
  object/prop table of any kind, only procedural collision/start-pos calls.
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

**Kickoff prompt** (only once every other phase you care about is actually
done — this is bookkeeping, not a bug fix):
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full. Phase 0 (writing that document) is already done; the only
work it still owes is folding the finished phases' results back into
LANOX_TROUBLESHOOTING.md's "Known-open items" section -- in particular
promoting the DLG_UI_BUTTON17.TGA entry from "not merged yet" to "present but
invalid data" (Phase 7c). Do this once, summarizing what each closed phase
changed; don't touch phases that are still open.
```

---

## Phase 1 — NPCs missing across villages (Luriel / Camilla / Ariel / others) [issue 1]

**Paste with:** Starting context + this section.

**Revision, 2026-09-22 — this is two separate bugs, not one.** The user has
now confirmed missing NPCs while playing Elder, Bethma, Feita, Hamel, and
Silent Night Shelter, not just Lanox. The original "Lanox-only" premise below
is **wrong as a blanket claim** — it correctly diagnosed the Ranox-side gap
(bug A), but the old-village reports are a second, unrelated bug (bug B) that
this phase's original Lua fix cannot touch. Both are documented here; do not
collapse them into one fix.

### Bug A — Ranox's shared houses had no world row (original finding, fixed in data, not yet packed/tested)

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

**This half is settled by data and still correct as far as it goes:**
`COMMON_NPC_LOCATION` rows for Elder/Besma/Altera/Peita/Velder/Hamel/Sander
are present and byte-correct for `HI_CAMILLA_PVP_HOUSE` (confirmed by reading
the live block, `DLG_Map_House.lua:880-950` — all 9 world rows present
including the Ranox one this phase added). So bug A's mechanism
(`CX2TFieldGame::AddFieldNpc`'s `bFindNpcPos` gate, `X2TFieldGame.cpp:3140-3157`)
is **not** what's hiding Camilla/Ariel in old villages — there's no missing
row for it to catch. That's what makes this bug B, below, rather than the
same bug showing up twice.

### Bug B — old-village NPCs are invisible because their mesh fails to load, proven from a live session

**Not the guard flag.** `SERV_IRUHADEV_FIELDNPC_EMPTY_NPCLIST_GUARD`
(`X2TFieldNpc.cpp:1861`, sibling in `X2TFieldNpcShop.cpp:257`) was the first
suspect named for this investigation. Ruled out on two independent grounds:

1. **Scope.** The guard's degrade path only fires for a house whose
   `NPC_ID_LIST` is empty or names an ID this build doesn't define — and the
   three houses that actually hit that (`HI_GUILD_MARK`,
   `HI_EVENT_GUILD_EDAN_VILAGE`, `HI_EVENT_BLESS_OF_SOLACE`) each appear
   exactly once in `DLG_Map_Village.lua`, inside `VMI_RANOX`'s own
   `HouseIDList` (confirmed by grep across the whole file). Elder, Bethma,
   Feita, Hamel, and Silent Night Shelter never instantiate these houses, so
   the guard's degrade branch cannot execute there at all.
2. **Effect.** Even where it does fire, the guard only sets `m_iNpcIndex =
   -1` and an empty display name — it does not gate mesh creation.
   `CreateFieldNpc` (`X2TFieldNpc.cpp:1736-1908`) opens `m_pMesh` from
   `pHouseTemplate->m_NPCMeshName` unconditionally, before the guarded block
   even runs. A degraded house still renders a mesh; it's just nameless.

**The real cause, found in the game directory's own `ErrorLog.txt`
(session `2026/9/21 22:15`–`22:22`, `$DATA/ErrorLog.txt`):**
`Motion_Npc_All_Camilla.x` and `Motion_Npc_All_Ariel.x` — the shared,
all-village Camilla/Ariel mesh, **not** a Ranox-ported file — fail their
animation-controller load (`KEM_ERROR410` then `KEM_ERROR246`,
`KTDXAnimationController.cpp:1525`/`KTDXDeviceXSkinMesh.cpp:1177`) three
separate times in that one session: once right after `MOTION_NPC_HAMEL_*`
loads (Hamel), once right after `MOTION_NPC_ELDER_HOFMANN.X` (Elder), and
once right after `MOTION_NPC_BESMA_CHACHABUK.X` (Besma/Bethma). The exact
degrade for this failure already exists —
`SERV_IRUHADEV_RANOX_ANIM_DEGRADE` — but it is currently **commented out**
(`KTDXLIB/Always.h:4147`):

```
// Reverted 2026-09-20: undefined pending root-cause fix; the #ifdef
// blocks in KTDXAnimationController.cpp / KTDXDeviceXSkinMesh.cpp stay in
// place.
//#define SERV_IRUHADEV_RANOX_ANIM_DEGRADE
```

With the flag off, `KTDXDeviceXSkinMesh.cpp:1176-1185`'s `#else` branch
returns `E_FAIL` on this failure instead of degrading to a static mesh —
which fails the **entire** X-file open, not just the animation. `m_pMesh`
comes back NULL, `CreateFieldNpc`'s `if( m_pMesh != NULL )` block never runs,
and the NPC node exists with nothing to draw: exactly "the NPC is missing,"
in every village that places Camilla or Ariel, independent of Ranox.

This flag's own comment names it as covering *"some ported June2017 motion
files... confirmed: `Motion_NPC_Ranox_SYOTA.x`, `FACEOFF.x`"* — i.e. it was
scoped to Ranox-ported files when written. `Motion_Npc_All_Camilla.x` and
`Motion_Npc_All_Ariel.x` are pre-existing, all-village assets, so either the
same 2-byte-short truncation defect (confirmed for SYOTA/FACEOFF) also
affects these two unrelated files, or a different defect happens to raise the
same error code. **Not yet distinguished — this is the next open question**,
not a settled fact; check the two files' tail bytes / size against a
known-good reference (June2017's copies, or the pre-port March originals)
before assuming it's the identical corruption.

**Fix, in two parts, same shape as Phase 7b/7a's split:**

1. **Re-enable `SERV_IRUHADEV_RANOX_ANIM_DEGRADE`** (uncomment
   `KTDXLIB/Always.h:4147`) as the immediate, already-written fix for the
   visibility symptom — `touch X2Lib/stdafx.cpp`, rebuild, redeploy. This is
   a KTDXLIB (studio-code) flag, so it stays scoped to the existing
   `#ifdef`/`#else` sites already in place; no new call sites needed.
2. **Root-cause why these two non-ported files hit the same failure** — this
   is the part the flag itself says is still owed ("pending root-cause
   fix"). Do not consider Bug B closed on step 1 alone; a degraded (static,
   unanimated) Camilla/Ariel is progress, not the fix.

**Verify:** pack Bug A's Lua fix, extract the live `.kom`, confirm bytecode +
row present. Rebuild with `SERV_IRUHADEV_RANOX_ANIM_DEGRADE` re-enabled.
Play-test: walk into Lanox — Camilla, Ariel, postbox, bank, billboard all
present; walk into Elder, Bethma, Feita, Hamel, and Silent Night Shelter —
Camilla and Ariel now appear (static pose is acceptable for this pass;
absence is not). Grep `ErrorLog.txt` for `KEM_ERROR246` after the play-test —
any remaining hits name the next mesh to root-cause.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially the "Starting context" block, the
"Investigation ledger" section (do not re-derive the 9 disproved hypotheses
or re-litigate the already-proven root causes there), and "Phase 1". Branch:
mods/lanox-port. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task: Phase 1 is two separate bugs, not one -- do not collapse them.

Bug A (Ranox's own shared houses) is already fixed in data (DLG_Map_House.lua
rows added for HI_CAMILLA_PVP_HOUSE, HI_ARIEL_EVENT_HOUSE, HI_POSTBOX,
HI_PRIVATE_BANK, HI_BILLBOARD) but not yet packed or play-tested -- confirm
it's packed (elstools compile-batch -v mar-2014, then extract the LIVE .kom
and confirm the bytes, never trust kom_contents/ staging) and verify in a
play-test.

Bug B (Elder/Bethma/Feita/Hamel/Silent Night Shelter missing Camilla/Ariel)
is a client-code fix: re-enable SERV_IRUHADEV_RANOX_ANIM_DEGRADE
(KTDXLIB/Always.h:4147, currently commented out), touch X2Lib/stdafx.cpp,
rebuild US_SERVICE in order (X2ServerProtocol X2TOOL -> KTDXLIB -> X2Lib ->
X2), redeploy, and play-test. Re-enabling the flag is NOT the whole fix --
the phase also owes a root-cause for why Motion_Npc_All_Camilla.x and
Motion_Npc_All_Ariel.x (pre-existing, all-village assets, not Ranox-ported
files) hit the same animation-controller failure as the Ranox-ported
SYOTA/FACEOFF files the flag was written for. Compare these two files' tail
bytes/size against a known-good reference (June2017's copies, or the
pre-port March originals) before assuming it's the identical 2-byte-short
truncation defect.

Verify: play-test Lanox (Camilla/Ariel/postbox/bank/billboard present) and
Elder/Bethma/Feita/Hamel/Silent Night Shelter (Camilla/Ariel now appear,
static pose acceptable for this pass). Grep ErrorLog.txt for KEM_ERROR246
afterward -- any remaining hits name the next mesh to root-cause.
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 2". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task: remove EVENT2 = TRUE / EVENT3 = TRUE and their orphaned affinity
keys (NPC_TALK_LIST0..3, Receive_*_Gift_Talk, Gift_UI_Talk,
Every_Day_Reward_Talk, Level_Up_Talk) from the ported Ranox house blocks in
DLG_Map_House.lua: HI_EDEL, HI_STEEL, HI_FACEOFF, HI_SYOTA,
HI_EVENT_GUILD_EDAN_VILAGE. Do NOT implement NMUM_EVENT2/3 -- they're entry
points to a system this build doesn't have; implementing them would be new
functionality, not a port fix. Separately, HI_IGNIA has no menu flags at all
-- determine the correct flag set from June2017's block (filtered to keys
this build actually reads) and add it.

Pack with elstools compile-batch -v mar-2014, repack, extract the LIVE .kom
and confirm the bytes (never kom_contents/ staging).

Verify: play-test -- talk to Edel (no dead Event buttons), talk to Ignia
(has its intended buttons).
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 3". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task: extract the live ShopItem.lua from its .kom and add an
AddShopItemList_LUA{ VILLIAGE_ID = VILLAGE_MAP_ID["VMI_RANOX"], ... } block
modelled byte-for-byte on the existing VMI_SANDER block, with SALE_ITEM_NPC
rows for Steel (NUI_STEEL/HI_STEEL), Faceoff (NUI_FACEOFF/HI_FACEOFF), and
Syota (NUI_SYOTA/HI_SYOTA) -- see Phase 3's table for each NPC's flags. Item
IDs come from June2017's ShopItem.lua, filtered to items that exist in this
build's ItemTemplet -- drop entries that don't resolve, never invent
replacements. Check whether VMI_BATTLE_FIELD_RANOX_REST_00 needs its own
vendor block too (the file is keyed per map, not per region).

Pack with elstools compile-batch -v mar-2014, repack, extract the LIVE .kom
and confirm the bytes (never kom_contents/ staging).

Verify: play-test -- Steel/Faceoff/Syota all list real, purchasable stock.
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 4". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task: stage data034/DLG_UI_Local_Map_NEW.lua from
.../237311/24965799/march_data/data034/ and hand-add a 9th CreateRadioButton
(AddDummyInt(LOCAL_MAP_ID.LMI_RANOX), SetCustomMsgChecked with
LMUCM_DUNGEON_GATE_SELECTED, same SetGroupID as the other 8). Do NOT copy
June2017's file wholesale -- it references LMUCM_LOCAL_MAP_ARROW_LEFT/RIGHT
and LMI_ELYSION/LMI_EVENT/LMI_SPECIAL, none of which exist in this build.
Also merge RANOX_BTN_NORMAL/OVER/DOWN into March's DLG_UI_Button20_NEW.tga
(June uses a different atlas/button naming). Phase 7's 2026-09-22 update
marked its TGA-format finding (7c, DLG_UI_BUTTON17.TGA failing
D3DXERR_INVALIDDATA) as CLOSED -- read that finding before this merge and
carry forward whatever format fix it used (bit depth / RLE / origin bit /
non-power-of-two), since this atlas merge presumably goes through the same
export path.

Pack with elstools compile-batch -v mar-2014, repack the atlas, extract the
LIVE .kom and confirm the bytes (never kom_contents/ staging).

Verify: play-test -- dungeon list shows a Lanox tab alongside Sander's. Note
(don't fix here): X2LocationManager.h:487 IsVillageField() and
X2WorldMapUI.cpp:2457 both bound VMI_RUBEN..VMI_SANDER, excluding VMI_RANOX
-- zero callers today, just log it.
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger" (Phase 5 has its own table of 5 additional dead hypotheses -- do not
redo any of those either), and "Phase 5". Branch: mods/lanox-port. Do NOT
consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task: do Phase 4 first (or confirm it's already landed) and re-observe
-- with no LMI_RANOX radio button, what the user saw as "locked" was most
likely the Lanox local-map gates (DLG_Map_Local_Ranox.lua) rather than the
dungeon-list dialog, so Phase 4's fix may make this phase evaporate. If
still locked after that: check the gate-to-dungeon wiring in
DLG_Map_Local_Ranox.lua against a working region's file (does each gate's
dungeon binding name a DUNGEON_ID this build has; is the unconditional
Ranox_SECRET_DUNGEON_LOCK sprite drawn over more buttons than just Secret
Hell's). Only then instrument: add a short-lived
SERV_IRUHADEV_RANOXDBG_DUNGEON flag logging, per Ranox DUNGEON_ID, whether
GetDungeonData returned NULL and what IsActiveDungeon decided -- use
ErrorLogMsg, not CX2OfflineLog::Server (it isn't open during this parsing
path). Do NOT "fix" GetNextDungeon -- ignoring m_vecExtraRequireDungeonID is
a pre-existing engine quirk March's own Sander chain also relies on.

Verify: re-test the dungeon tab directly after Phase 4. If still locked,
apply the wiring check and instrumentation, rebuild (touch
X2Lib/stdafx.cpp, US_SERVICE order X2ServerProtocol -> KTDXLIB -> X2Lib ->
X2), redeploy, re-test -- BurnedForest should be selectable at level >= 68.
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 6". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

Your task, both parts in the same pass or you trade one bug for another:
(1) add VMI_RANOX + its 4 fields returning zone 15, and
VMI_BATTLE_FIELD_RANOX_REST_00 returning zone 16, to the
CheckCOBOExpressTicketMapID ladder in data036/MapData.lua (follow the Sander
13/14 precedent verbatim; zone 16 is safe, every C++ consumer only compares
against 0). (2) Restore ComputeCOBOExpressTicketCost in the same file -- it's
a broken decompile (references an unassigned slot6) -- from the
un-decompiled original at KncWX2Server/ServerResource/US/MapData.lua:642+
(d = math.abs(iDesID - iSrcID)). Skipping part 2 turns the fixed refusal
into a silent free warp once part 1 makes the Lua reachable.

Pack with elstools compile-batch -v mar-2014, repack, extract the LIVE .kom
and confirm the bytecode (never kom_contents/ staging).

Verify: play-test -- Cobo Express reaches Silent Night Shelter from Lanox
town and back, for a sane non-zero cost.
```

---

## Phase 7 — Client crash entering Ashen Land [issue 7]

**Paste with:** Starting context + this section.

**Update, 2026-09-22 — 7a/7b/7c confirmed done.** The user has fixed the
asset-loading problem (7a) and can now access every Lanox field with no
crash. Treat 7a/7b/7c below as closed; do not re-open them without new
evidence. A **new** issue surfaced once the fields actually load: field
monsters spawn and die immediately. Opened as **7d**, after the original
three items.

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

**7d — field monsters spawn and die immediately (new, 2026-09-22).** Not yet
root-caused; this is the investigation trail so far, not a fix.

- Field-battle monster HP is **not** carried on the spawn request. In the
  offline emulator, `CX2OfflineBattleField::PickFromGroup`
  (`X2Lib/Offline/X2OfflineBattleField.cpp:725-761`) fills `KNPCUnitReq` with
  `m_NPCID`, `m_Level = iStandardMonsterLevel + pChosen->m_iAddLevel`,
  position, and flags — **no HP field at all**. HP has to be resolved
  client-side, from `m_NPCID` + `m_Level`, once the request becomes a live
  unit — that resolution point hasn't been located yet.
- **`CX2OfflineStatTable::GetUnitStat`
  (`X2Lib/Offline/X2OfflineStatTable.cpp:298-357`) is probably not the
  culprit even if it is in this path** — every miss case (`m_mapStat` empty,
  class not found, level not found) falls back to `SyntheticStat`, which
  always returns a large positive HP (11250 at level 1 and up). It cannot
  produce the zero/near-zero HP "dies immediately" implies, so if HP really
  does resolve through here, something upstream is producing a bad
  `iUnitClass` for these monsters specifically, not this function itself.
- The 9 field-monster types named in 7a (`MAG_CHARGER`, `MAG_NOM`,
  `RAGE_BABYFIG`, `RAGE_DRYAD`, `SMITH_LITTLEWORKER`, `SMITH_MACHINE`,
  `SMITH_RUNNER`, `SMITH_SCIENCE`, `SMITH_WORKER`) are new `NPC_UNIT_ID`
  entries added by the port (confirmed present in
  `march2014/data036/Enum.lua`). If their stat/templet rows were never
  transcribed into whatever table actually backs monster HP (client-side
  `CX2UnitManager`/`NPCUnitTemplet` data, or a server-side `StatTable.lua`
  row keyed by their `UNIT_CLASS`), that's the same "server Lua the client
  needs" shape as the rule at the top of `CLAUDE.md` — do not hardcode a
  number for these 9 monsters if that's where this lands.
- **Next steps, in order:** (1) grep `offline_server.log` /
  `offline_packets.log` for `STAT` or `WARNING` lines during a Lanox-field
  play session — `GetUnitStat`'s own `WARNING no rows for class=%d` line
  would settle whether this function is even in the path. (2) If it's not,
  find the client-side call site that turns a `KNPCUnitReq` (or its P2P
  equivalent) into a live `CX2Unit` with initialized HP — likely in
  `CX2Game`/`CX2BattleFieldGame`'s monster-creation handler — and read what
  it does when the monster's `UNIT_CLASS` has no matching row. (3) Only once
  the actual zero-HP mechanism is identified, decide whether the fix is a
  missing table row (pack it, per the server-Lua rule) or a code gap in the
  new-monster path.

**Verify (7d):** not yet actionable — no fix exists to test. Once diagnosed,
fold into a play-test the same way as 7a-c: enter a Lanox field, confirm
monsters take visible damage over more than one hit and survive contact
briefly rather than dying on spawn.

**Kickoff prompt** (scoped to 7d only — 7a/7b/7c are closed):
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 7". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Where the true fix
is server-side data the user must pack, implement the whole feature with a
degrade-if-missing path anyway. Never launch X2_offline.exe without asking
first, every time.

7a/7b/7c are CLOSED -- the user has already fixed asset loading and can
enter every Lanox field with no crash. Do not re-open them without new
evidence; this kickoff is scoped to 7d only.

Your task (7d): field monsters spawn and die immediately in Lanox fields.
Not yet root-caused. Next steps, in order: (1) grep offline_server.log /
offline_packets.log for STAT or WARNING lines during a Lanox-field play
session -- CX2OfflineStatTable::GetUnitStat's own "WARNING no rows for
class=%d" line would settle whether this function is even in the path (it
probably isn't the culprit even if it is in the path -- every miss case
falls back to SyntheticStat, which always returns a large positive HP). (2)
If it's not in the path, find the client-side call site that turns a
KNPCUnitReq (or its P2P equivalent) into a live CX2Unit with initialized HP
-- likely in CX2Game/CX2BattleFieldGame's monster-creation handler -- and
read what it does when the monster's UNIT_CLASS has no matching row. (3)
Only once the actual zero-HP mechanism is identified, decide whether the fix
is a missing table row (this is the server-Lua-the-client-needs shape --
pack it, name the exact file when reporting done, do not hardcode a number
for these 9 monsters) or a code gap in the new-monster path.

Verify: not yet actionable until diagnosed. Once a fix exists, play-test --
enter a Lanox field, confirm monsters take visible damage over more than one
hit and survive contact briefly rather than dying on spawn.
```

---

## Phase 8 — No signboard at the Lanox end of Behemoth Crater [issue 8]

**Paste with:** Starting context + this section.

**Clarified, 2026-09-22:** the user means the literal physical signboard
prop next to the field-end teleport portal — the "go left / go right"
direction marker model, not the quest-log signboard NPC (`HI_BILLBOARD`).
This was already the correct reading below; the finding is confirmed, not
revised. Added confirmation: `march2014/data040/Field_Sander_Stage3_LineMap.lua`
(Behemoth Crater's own linemap, the file Phase 9 already reads for its
`AddStartPos` line data) contains **only** procedural `g_pLineMap:` calls —
`SetLandHeight`, `SetTeamStartPos`, `AddStartPos` — no object, prop, or
decoration table of any kind, at any line. There is nowhere in this file (or
any file this build loads, per the search below) for a signboard placement
to have been dropped from. This closes off the last plausible "maybe it's a
data table after all" reading.

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 8". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

This phase is a verification step, not a code change -- there is nothing in
the data or engine to fix; do not add BATTLE_FIELD_DIRECTION_INFO rows
(nothing reads them in this build) and do not weaken the
m_bUserShowQuickQuestDLG gate to force the guide particle on (that changes
behaviour for every field in the game, not just Lanox's).

Your task: in Sander Field 04 (Behemoth Crater), toggle the quick-quest
window on and look for the QUEST_GUIDE guide particle near the field's end.
The data (BattleFieldData.lua's PMT_MOVE_TO_VILLAGE portal to VMI_RANOX at
LINE_NUMBER=124) is already confirmed correct, so if the particle appears,
record that the engine-side pointer works and the phase is closed. The
physical signboard prop itself is scene geometry in the map mesh -- an art
task the user owns, not a code or data fix; do not attempt one.

Verify: the quick-quest toggle check above. No rebuild needed.
```

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

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. Read LANOX_DEFECT_PHASES.md in the
repo root in full -- especially "Starting context", the "Investigation
ledger", and "Phase 9". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Fix philosophy: stay as close to the original/studio code as possible -- no
hardcoded tables, no invented curves, no guessed values. Never launch
X2_offline.exe without asking first, every time.

This is an offline-emulator bug, not Lanox data -- the Lua
(DLG_Map_Village.lua's Lanox west-exit block, Field_Sander_Stage3_LineMap's
AddStartPos(177,...)) is already correct and must not be changed.

Your task: in X2Lib/Offline/Handlers_Room.cpp, at both
Handler_EGS_JOIN_BATTLE_FIELD_REQ ACK builders (around line 1278 and line
1365), echo m_bNowBattleFieldPositionInfoStartPosition and
m_usBattleFieldPositionValue from kReq -- they're currently left zeroed,
which is why a start-pos key (177) gets reinterpreted as a raw line index
and resolves to NULL. This also un-breaks the portal-re-entry path, which
loses its position value the same way. Before editing, check how the
nearest existing fixes in Handlers_Room.cpp are marked (a nested
SERV_IRUHADEV_* flag, or a plain //{{ Iruha : <date> block) and match that
style -- X2Lib/Offline/ is entirely ours and already lives inside
SERV_IRUHADEV_OFFLINE, so this doesn't need its own new flag the way a
KTDXLIB studio-code fix would.

Build (touch X2Lib/stdafx.cpp if you touched any header, US_SERVICE order
X2ServerProtocol -> KTDXLIB -> X2Lib -> X2), deploy, and ask before
launching.

Verify: play-test -- take the leftmost Lanox teleport into Behemoth Crater,
confirm arrival at the field's end (near line 122), not its start.
```

---

## Verification — batching

**Batch the play-tests — each one costs a "may I launch?" round-trip.**
Updated 2026-09-22 to fold in Phase 1's Bug B and to drop 7a/7b/7c (done):

1. **Lua batch** (phases 1 Bug A, 2, 3, 4, 6): one `compile-batch` + repack +
   one play-test covering Lanox common NPCs, Edel/Ignia's dialog buttons,
   Steel/Faceoff/Syota's shops, the dungeon-list tab, the Cobo hop to Silent
   Night Shelter — and Elder, for Phase 1's scope check.
2. **C++ batch** (phases 1 Bug B — re-enable `SERV_IRUHADEV_RANOX_ANIM_DEGRADE`
   — and 9): one build/deploy/play-test — Camilla/Ariel visible (even if
   static) in Lanox, Elder, Bethma, Feita, Hamel, and Silent Night Shelter;
   and take the leftmost Lanox teleport (expect the crater's end).
3. **Asset batch** (Phase 4's atlas merge only — 7a/7c already shipped):
   repack, one play-test.

Phase 5 may need a fourth pass once its instrumentation lands, but only if
batch 1 doesn't already resolve it via Phase 4. Phase 7d has no batch yet —
it isn't diagnosed far enough to have a fix to test; add it to whichever
batch is next once step (2) of its investigation names the actual code path.
Phase 8 needs no play-test batch at all — it was closed by static reading,
confirmed rather than revised by the user's clarification.

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
visible in Lanox, **and also in Elder, Bethma, Feita, Hamel, and Silent Night
Shelter** (Phase 1, both bugs); Edel's dialog shows no dead Event buttons and
Ignia shows its intended ones; Steel/Faceoff/Syota shops list stock; a Lanox
tab appears in the dungeon list; BurnedForest is selectable at level ≥ 68;
Cobo Express reaches Silent Night Shelter; Ashen Land loads without crashing
and `ErrorLog.txt` is clean of the 18 assets (done); field monsters survive
more than one hit rather than dying on spawn (Phase 7d, still open — no
acceptance criterion until it's diagnosed further); the Behemoth Crater
quest-guide particle appears when toggled on (the physical signboard itself
is confirmed out of scope — art task, not a bug); the leftmost Lanox teleport
lands at the crater's end.
