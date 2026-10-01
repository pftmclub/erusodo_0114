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
  **Root-caused and fixed in data 2026-09-22, across all 4 chained Lanox
  fields:** field monster types have an `AddNPCTemplet` row (so they spawn
  with working AI) but no `AddNPCStat` row, so `m_stat` defaults to
  `MAX_HP = 0` and they resolve to 0 HP the instant `SetHardLevel` runs —
  sent back as `KILL_SELF`, not combat death. Fixed 25 of 33 distinct
  monster `NPC_ID`s spawned across `Ranox_Field_00`/`_02`/`_03.lua` (a first
  pass wrongly targeted 9 *base-name* siblings instead of the `_FD` variants
  the fields actually spawn — caught and corrected before packing) by
  staging `march2014/data037/NPCStat.src.lua` with rows transcribed verbatim
  from June2017. **8 `NUI_LOW_GEYSER_*_FD` monsters in `Ranox_Field_01.lua`
  remain unfixed and will still die on spawn** — neither March nor June has
  any stat data for the Geyser monster family, so there is nothing to
  transcribe; not a regression, a documented and currently-unsourceable gap.
  Not yet packed or play-tested; see Phase 7's own execution notes for the
  full trace, the sanity check, and a found-but-deferred 35-entry wider
  version of the same registry gap (FireTemple/BurnedForest, still locked by
  Phase 5).
  **Superseded 2026-09-26 — see `LANOX_EXTRA_NPC_STAT_PLAN.md`.** The "no
  source data" and "35 remain" findings above searched only `NPCStat.lua`.
  June2017 has a **second** NPC stat system, `data036/NewNPCStat.lua` +
  `NewNPCStatFormula.lua` (`AddExtraNPCStat` / `CalculateExtraStatValue`),
  and it covers the 8 Geyser `_FD` monsters, every remaining Lanox dungeon
  gap, and the Lanox dungeon **"phase clears while mobs are alive"** defect.
  That defect is caused by a 0-HP gate NPC dying `KILL_SELF` and satisfying
  `CC_KILL_NPC_TYPE`. The port is planned there, phase by phase.
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

**Execution notes, 2026-09-22:**

- **Bug A confirmed not packed.** Extracted the live `data034.kom` from
  `$DATA`, decompiled `DLG_Map_House.lua`, and confirmed `HI_CAMILLA_PVP_HOUSE`
  still has only the 8 pre-existing world rows in `COMMON_NPC_LOCATION` — no
  `WI_VILLAGE_RANOX` row. The 10 `WI_VILLAGE_RANOX` hits already present in
  the live archive all belong to Ranox-exclusive houses (`HI_EDEL`,
  `HI_STEEL`, `HI_FACEOFF`, `HI_SYOTA`, `HI_IGNIA`, `HI_GUILD_MARK`,
  `HI_SPA_SIGN_RANOX`, `HI_EVENT_GUILD_EDAN_VILAGE`,
  `HI_EVENT_BLESS_OF_SOLACE`, `HI_MU`), none of the 5 target houses. Repo's
  `march2014/data034/DLG_Map_House.lua` test-compiles clean via
  `elstools compile-batch -v mar-2014` (60,560 bytes) — data fix is correct
  and ready, only the repack (the user's step, per established convention —
  see `RANOX_PORT_PLAN.md` Phase 22) is outstanding.
- **Bug B built and deployed.** Re-enabled the flag (byte-level edit,
  `KTDXLIB/Always.h`, verified `file`/`git diff --stat` before and after).
  Confirmed the flag actually reaches `KTDXDeviceXSkinMesh.cpp` before
  trusting a rebuild: a temporary `#pragma message` inside the `#ifdef`,
  built KTDXLIB alone, read `RANOX_ANIM_DEGRADE_PROBE_SKINMESH ON` in the
  compiler output, then removed the probe (clean diff after removal).
  **Touching only `X2Lib/stdafx.cpp` is not enough** — both `#ifdef` call
  sites (`KTDXAnimationController.cpp`, `KTDXDeviceXSkinMesh.cpp`) are in
  `KTDXLIB`, which has its own PCH through `KTDXLIB/stdafx.cpp`; touch both.
  Full rebuild (`X2ServerProtocol` X2TOOL → `KTDXLIB` → `X2Lib` → `X2`, all
  US_SERVICE except the first) hit one **unrelated pre-existing bug**:
  `X2Lib/X2Main.cpp:454` had `wcsrchr( wszSelfPath, L'\' )` — an unescaped
  backslash character literal (`L'\'` closes the literal on the escaped
  quote, so the compiler reads past the line: `C2001: newline in constant`).
  Introduced in `0eab90f` ("Recover CP949-corrupted files, reapply Ranox
  guard + crash-log fix"), unrelated to this phase's own edits but blocking
  every build since. Fixed byte-level to `L'\\'` (confirmed 1-line diff,
  encoding intact). With that fixed, `X2ServerProtocol`/`KTDXLIB`/`X2Lib`/`X2`
  all built exit 0 (warnings only — pre-existing deprecation/PurpleSDK
  duplicate-symbol noise). Deployed `X2/US_SERVICE/x2.exe` → `$DATA/
  X2_offline.exe`, confirmed programmatically (size 18,822,656 bytes match,
  sole `X2_*.exe` present).
- **Root-cause, settled — not the same class of question the phase posed.**
  Parsed `KTDXAnimationController`'s on-disk layout by hand (`KSMHeader` →
  `KACHeader` → last `KACKeyframedAnimationSet` → its name-string offset) for
  both files, extracted from the *live* `.kom`, from this repo's own
  pre-Ranox full archive (`data013_packed.kom`), and from June2017's
  pristine, never-repacked `data013_kom_ori`. **All three copies of each
  file are byte-for-byte identical** — nothing in this project's port or
  repack pipeline truncated them. But the files' own internal metadata
  independently overruns physical EOF: `Motion_Npc_All_Camilla.x`'s last
  animation set's name string overruns by **exactly 2 bytes** — the identical
  signature already confirmed for `Motion_NPC_Ranox_SYOTA.x`/`FACEOFF.x` —
  and `Motion_Npc_All_Ariel.x`'s overruns by **1 byte**. This is a defect in
  the original KOG-exported asset itself (present since at least June2017,
  likely since March 2014 — nothing Ranox-specific about it), not a repack
  or truncation bug. `SERV_IRUHADEV_RANOX_ANIM_DEGRADE` is therefore the
  correct general fix, not a Ranox-scoped band-aid that happens to also
  help here — its comment was just scoped too narrowly when first written.
  Updated the flag's own comment in `Always.h` to record this. **No further
  C++ work is owed by this phase.**
- **Play-tested — both bugs confirmed fixed.** User repacked `data034.kom`
  (06:14) and play-tested; `ErrorLog.txt` for that session shows zero
  `KEM_ERROR246` hits. Re-extracted the live `data034.kom` afterward and
  confirmed `HI_CAMILLA_PVP_HOUSE` now has 15 `WI_VILLAGE_RANOX` mentions
  total across the file (10 pre-existing Ranox-only houses + the 5 new rows),
  and decompiled the specific block to confirm the new row landed. Phase 1
  (Bug A + Bug B) is done.

**New minor issue found in the same play-test session, deferred —
Camilla's interact prompt doesn't appear in Lanox.** Her mesh renders
correctly (Bug B's fix), but walking up to her never shows the "press to
talk" prompt, even though the identical prompt works for her in Hamel.
Root-caused with a short-lived diagnostic (`SERV_IRUHADEV_CAMILLA_JOINAREA_
DEBUG`, since reverted — logged `CX2TFieldGame::OnFrameMove`'s
`fNpcDistance`/`m_NPCJoinArea`/`GetInArea()` for `NUI_CAMILLA` once/sec to
`ErrorLog.txt`, not `offline_server.log` — `CX2OfflineLog::Server` doesn't
cover this code path either):

- In Ranox (`world=582`), the closest the player could get was **dist=437.8**
  against a **joinArea=400** — the prompt's proximity check
  (`X2TFieldGame.cpp:1839`, `fNpcDistance <= pNpcNode->m_NPCJoinArea`) never
  passes. The player visibly stalled at ~438 for several seconds (repeated
  near-identical position samples), consistent with a collision obstacle
  (the shop counter) blocking closer approach.
- In Hamel (`world=417`) the same house's per-world row carries a `700`
  join-area override (`X2LocationManager.h:179-181`'s `m_fLocalJoinArea`,
  gated by `LOCAL_NPC_JOIN_AREA` which this build has defined) — comfortably
  covering Hamel's own approach distance.
- **Checked June2017's own `DLG_Map_House.lua` for the Ranox row we ported
  from — it carries no join-area override either** (Camilla's row is 7
  fields, no 8th; Ariel's has an explicit `0`). So this isn't a porting
  mistake relative to the source — either the original game has the same
  tight margin, or (more likely) the actual difference is in the ported
  Ranox collision/LineMap geometry around that counter, not the NPC data.
- **Deferred, per the user's own call** — not fixed this session. Two
  honest options for whoever picks this up: (a) add a measured, explicit
  per-world join-area override (empirically justified by the ~438 distance,
  e.g. `500`, matching the granularity already used elsewhere) to
  `march2014/data034/DLG_Map_House.lua`'s `HI_CAMILLA_PVP_HOUSE` Ranox row —
  quick, but the number isn't sourced from any file; or (b) root-cause
  whether Ranox's ported collision/LineMap places the counter slightly
  wrong relative to Camilla's position, which would be the more correct fix
  and might affect other interactions at that counter too (Ariel sits at
  the mirrored `-460` position behind the same counter — worth checking
  once this is picked back up). The debug flag and its `X2TFieldGame.cpp`
  instrumentation were fully reverted (verified clean `git diff`) and the
  client rebuilt/redeployed without it before this was left open.

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

**Superseded 2026-10-01 — the atlas merge below (`DLG_UI_Button20_NEW.tga`/
`.TET`) must not be redone.** `JUNE_UI_PARITY_PHASES.md` Phase 2 adopts
June's `DLG_UI_Local_Map_new.lua` wholesale, together with June's
`DLG_UI_Button21` atlas, where `BTN_RANOX_N/O/D` already live — that delivers
the Lanox tab instead. See `LANOX_REMAINING_PHASES.md` Phase 3, which cleaned
the stale `DLG_UI_Button20_NEW` entries out of
`march2014/update/data056/_manifest.json` (they were never on disk) on this
same date. The `.lua` half of this phase (the 9th `LMI_RANOX` radio button)
is still live in `data034.kom` for now — it stays live only until June-UI
Phase 2 ships, since that phase stages June's `DLG_UI_Local_Map_new.lua`
wholesale over `march2014/data034/DLG_UI_Local_Map_NEW.src.lua`, replacing
this button along with everything else in the file.

**Update 2026-10-01 (later): the lua half is superseded too.** June-UI Phase 2
has staged June's file over `march2014/data034/DLG_UI_Local_Map_NEW.src.lua`.
In June's two-page tab strip, Lanox is the 9th radio, on page 2.
The C++ paging port and the compiled lua are done (`kom_contents/data034_kom`,
12,985 B); only the `data034.kom` repack is owed. Nothing in this phase is
left to do or redo. Play-test via `JUNE_UI_PARITY_PHASES.md` "Phase 2 result".

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

**Execution notes, 2026-09-22:**

- **Lua staged.** `march2014/data034/DLG_UI_Local_Map_NEW.lua` now carries a 9th
  `CreateRadioButton` block, modeled on the sander block (not ruben — ruben's
  `RIGHT_BOTTOM y=25` is a lone outlier; every other button uses 34), with
  `RANOX_BTN_NORMAL/OVER/DOWN`, `SetGroupID(1)` (same as the other 8),
  `SetCustomMsgChecked(LMUCM_DUNGEON_GATE_SELECTED)`,
  `AddDummyInt(LOCAL_MAP_ID.LMI_RANOX)`, and the studio's own `SetName("ruben")`
  quirk preserved verbatim (all 8 existing buttons pass the literal string
  `"ruben"` to `SetName` regardless of village — transcribed, not tidied).
  Position follows the established +65 step: `BGPoint`/`RBPoint` `LEFT_TOP =
  (528, 9)`, `RBPoint RIGHT_BOTTOM = (592, 34)`, `CheckPoint LEFT_TOP = (528,
  7)`. Two things beyond the task's own description, both sourced from the
  file's existing structure rather than invented: (1) a 9th quest-notice
  picture in `g_pStatic_Quest` (`LEFT_TOP = (520, -10)`,
  `SetIndex(LOCAL_MAP_ID.LMI_RANOX)`) — the other 8 buttons each have one, so
  omitting a 9th would silently drop Ranox's quest badge; (2) the panel
  background's center/right pieces (`BG_CENTER_TOP/MIDDLE/BOTTOM`,
  `BG_RIGHT_TOP/MIDDLE/BOTTOM`) stretched by the same 65px one more button
  needs (`ADD_SIZE_X 500→565`/`510→575`, right pieces `x 551→616`) — without
  this the 9th tab would render past the dialog's own background. Test-compiled
  clean via `elstools compile-batch -v mar-2014` (10753 bytes) and
  decompiled the compiled output back to source to confirm a clean round trip
  (all 3 `RANOX` references present, nothing else changed). **Owed:** `.kom`
  packing into `data034.kom` is the user's step, per this project's established
  convention.
- **Atlas merged, not just staged.** Reverse-engineered the `.TET` sidecar
  format from `KTDXLIB/KTDXDeviceTET_Preprocessing.h/.cpp` (`KTETFormatHeader`
  + a sorted `DWORD` key-offset array + a `KTETFormatTextureUV` array + a
  length-prefixed wide-string table — `GetTexUV` binary-searches the key array,
  so it must stay sorted by wide-string comparison) and the atlas's own layout
  (columns of 65px-wide village buttons; `SANDER` occupies a second row-band
  below `RUBEN`'s column because an 8th 65px column doesn't fit the 512px
  width — `7*66=462`, `+65` would overrun to 527). Placed `RANOX_BTN_NORMAL`
  (`leftTop=(1,179)`), `_OVER` (`(1,208)`), `_DOWN` (`(1,237)`), each 58×28,
  directly below `SANDER`'s column, mirroring its own spacing convention.
  Source pixels are June2017's `data056_kom/DLG_UI_Button21.tga` keys
  `BTN_RANOX_N/O/D` (`leftTop=(1,336)/(1,365)/(1,394)`, also 58×28 — cropped
  verbatim, not rescaled to match the 65px column width; the click hit-box
  (`RBPoint`) doesn't need to match the texture's own width, and `ruben`'s
  existing 56-vs-64 mismatch shows the engine already tolerates this —
  cosmetically, expect the Lanox tab to sit ~7px narrower than its neighbours
  since the drawn quad follows the TET `rectSize`, not the 65px slot; that's
  not a placement bug worth re-opening, just June's art being narrower).
  Verified
  the coordinate space empirically before trusting it: cropped the known
  `RUBEN_BTN_NORMAL` region and rendered its alpha channel as ASCII art —
  a clean rounded-button silhouette confirmed the TET's `(x,y)` is top-left-
  origin, UI-space, and the TGA (`imageDescriptor 0x08`, bit5 clear) stores
  rows bottom-up, so cropping/pasting needs a `height-1-y` row flip; same check
  on June's 3 source crops confirmed legible button shapes before use.
  Produced `march2014/update/data056/DLG_UI_Button20_NEW.{tga,TET}` in a
  working location, appended a matching entry to that folder's `assets.txt`
  (later migrated into `_manifest.json`), and verified before staging: all 49
  of March's existing `.TET` keys round-trip byte-identical (parsed, compared
  field-by-field); the 3 new keys carry the expected rects; only TGA rows
  179–264 differ from the live archive's copy, and within those rows only
  `x∈[1,59)` changed; header (18 bytes) and 26-byte footer are byte-identical;
  file size unchanged (1,048,620 bytes). Unlike the `DLG_UI_Button17`/`_A`
  pair already sitting in `march2014/update/data056/` and `.../datai020/`
  (Phase 7c), which are explicitly *merge source only* per
  `RANOX_PORT_PLAN.md`'s note (June's Button21 atlas has a different `.TET`
  size/layout than March's, so a blind file-swap would destroy March's own
  existing swatches), this merge parsed March's real TET, preserved every
  existing entry unmodified, and only appended.

  **Correction, 2026-09-22 (later session) — this pair was never actually
  committed or packed, despite the write-up above.** `git log --all
  --full-history -- march2014/update/data056/DLG_UI_Button20_NEW*` returns
  nothing, the files are absent from `march2014/update/data056/` on disk, and
  the live `data056.kom` (extracted fresh, repacked 2026-09-22 20:42, well
  after the Phase 4 commit) still has a 49-entry `.TET`
  (`KTETFormatHeader.m_dwNumTextureUV = 49`, confirmed by parsing the header
  bytes directly) with no `RANOX_BTN_NORMAL/OVER/DOWN` keys — a UTF-16LE scan
  of the whole file found zero `RANOX` matches. The `.lua` half of Phase 4
  (the 9th `LMI_RANOX` radio button in `DLG_UI_Local_Map_NEW.lua`) **is**
  live in the current `data034.kom` (decompiled and confirmed: `AddDummyInt(
  LOCAL_MAP_ID.LMI_RANOX )` present) — only the atlas art half regressed.
  `march2014/update/data056/_manifest.json` still lists
  `DLG_UI_Button20_NEW.TET`/`.tga` as expected files that don't exist on
  disk, so whatever repacks from that manifest will skip or fail on them
  silently.

  **Verified non-blocking, not re-done this session.** `SetRBPoint` on the
  Ranox radio button sets `USE_TEXTURE_SIZE = FALSE` with an explicit
  `RIGHT_BOTTOM` (`(528,9)-(592,34)`, immediately right of Sander's
  `(463,9)-(527,34)`), so the click hit-box does not depend on the missing
  TET keys and the tab is still selectable. `SetBGPoint`/`SetNormalTex`/
  `SetOverTex`/`SetCheckedTex` do rely on `GetTexUV` succeeding — with the
  key missing, `CKTDGUIRadioButton::SetTextureKey` leaves `texSize` at its
  default `(0,0)`, so the tab draws **nothing visible**, not merely the
  wrong art. The panel-background stretch Phase 4 also added (`BG_CENTER_*`/
  `BG_RIGHT_*`, `ADD_SIZE_X` 500→565/510→575) is packed and does make room
  for it, so there is a blank clickable strip immediately right of Sander's
  tab. Redoing the atlas merge (regenerate the `.tga`/`.TET` pair, stage
  them in `march2014/update/data056/`, repack) **(superseded, see top of
  Phase 4 — do not redo this)** — it was left owed here, but it just doesn't
  block reaching Ranox's local map, and it must not be picked up now that
  June-UI Phase 2 covers it instead.
- **Phase 7c is not actually closed — correcting the 2026-09-22 update
  above.** Read Phase 7c's finding before this atlas merge, as instructed, and
  it doesn't hold up: extracted the **live** `data056.kom`/`datai020.kom`
  (mtime 2026-09-19 16:33) and both `DLG_UI_Button17.tga`/`_A.tga` are still
  the exact same malformed bytes originally reported —
  `D3DXERR_INVALIDDATA` would still fire. The phase's own hypothesis list
  (bit depth / RLE / origin bit / non-power-of-two) was also wrong; none of
  those is the defect. The real defect: the entire file (header through
  footer) is XOR'd with the same repeating 12-byte key already known from
  script encryption — `02 AA F8 C6 DC AB 47 26 EF BB 00 98`. Decrypting the
  live `DLG_UI_Button17.tga` with that key byte-for-byte recovers a textbook
  TGA — the header becomes byte-identical to `DLG_UI_Button20_NEW.tga`'s
  (`00 00 02 00 00 00 00 00 00 00 00 00 00 02 00 02 20 08`), the
  `TRUEVISION-XFILE.` footer signature appears exactly where it should, and
  the midfile bytes decode to plausible grayscale BGRA button pixels — not
  noise. The good news: **the fix already exists, staged and un-packed.**
  `march2014/update/data056/DLG_UI_Button17.tga` and
  `march2014/update/datai020/DLG_UI_Button17_A.tga` (written 2026-09-19
  16:21, 12 minutes *before* the live `.kom`s were last packed at 16:33) both
  have the clean header already — the repack at 16:33 simply didn't pick up
  this fix. This needs a re-pack of `data056.kom` and `datai020.kom` from the
  currently-staged files, not new work. My own atlas merge above targets
  `DLG_UI_Button20_NEW.tga`, a separate, already-clean file in the same
  archive — confirmed unaffected by this XOR corruption before merging into
  it.
- **Logged, not fixed (per the task):** `X2LocationManager.h:487`
  `IsVillageField()` and `X2WorldMapUI.cpp:2457` both bound
  `VMI_RUBEN..VMI_SANDER`, excluding `VMI_RANOX`. Zero callers today.
- **Owed:** pack `data034.kom` (Lua) and `data056.kom` (the Button17 re-pack
  above **only** — the Button20 atlas merge is superseded, see top of Phase 4,
  do not pack it) and play-test — dungeon list should show a Lanox tab
  alongside Sander's.

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

(Historical prompt, left verbatim. Its "Also merge RANOX_BTN_NORMAL/OVER/DOWN
into March's DLG_UI_Button20_NEW.tga" instruction is **superseded, see top of
Phase 4** — do not act on it if this prompt is ever re-pasted.)

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

**Root-caused and fixed in data, 2026-09-22 (later session) — not a logic bug
at all.** Phase 4's own 9th `LMI_RANOX` radio button *is* live (confirmed by
decompiling the live `data034.kom`), so the player could reach Ranox's local
map. The `SERV_IRUHADEV_RANOXDBG_DUNGEON` instrumentation (added this
session, `KTDXLIB/Always.h` + 4 log points in `X2Lib/X2LocalMapUI.cpp` —
region-tab level check, `GetLocalMapDLG` NULL check, `GetDungeonData` NULL
check, `IsDefenceDungeon` reroute, final `IsActiveDungeon` decision) proved
the C++ gate logic is correct: a live play-test's `ErrorLog.txt` showed
`control=Burnded_Forest dungeonID=33270 minLevel=70 unitLevel=80 isActive=1
authOperator=0` — `IsActiveDungeon` returns **true**, exactly as the static
analysis above predicted. Yet the user reported **zero** dungeon icons
rendering on the Ranox local map at all (not just BurnedForest) — "it only
renders `Ranox_Local`, no active dungeon icon, not like Sander which shows
Bonghun_temple/Screw_corridor etc."

**Real cause: the Ranox dungeon-gate icon textures were never actually
merged into their atlases — same defect class as Phase 4's tab-icon gap,
but blocking here instead of cosmetic.** `march2014/update/data217/
DLG_Map_Local_Ranox_Icon_NEW.lua`'s 8 dungeon buttons (`Burnded_Forest`,
`Ashes_Village`, `Geyser`, `Gateway`, `SantilusShip`/FireTemple,
`BehimossHeart`/FinalHall, `Ranox_Secret_Dungeon` ×2) reference texture keys
(`Ranox_Burned_Forest_N/O`, etc.) on three atlases —
`DLG_Common_Button03_NEW.tga`, `_04_NEW.tga`, `_05_NEW.tga` — and, critically,
these buttons use plain `SetNormalPoint`/`SetOverPoint` (no `SetRBPoint`
override), so with `USE_TEXTURE_SIZE` defaulting `true` and no texture UV to
size from, `CKTDGUIButton`'s rect resolves to `texSize=(0,0)`: **both the
draw and the hit-box collapse to nothing**, unlike the tab radio button whose
explicit `SetRBPoint(USE_TEXTURE_SIZE=FALSE, RIGHT_BOTTOM=...)` kept it
clickable despite missing art. Confirmed empirically from the live archive:

- `DLG_Common_Button03_NEW.TET` (extracted from the live `data063.kom`,
  repacked 2026-09-22 20:42) has 18 keys — all pre-existing Sander/event
  content, **zero** `RANOX_*` keys, and no Ranox pixel data at all (only the
  Lua's target coordinates were ever chosen, never populated).
- `DLG_Common_Button04_NEW.tga`/`_05_NEW.tga` exist in the live archive with
  **no `.TET` sidecar at all** — any key lookup on either fails
  unconditionally.
- `march2014/update/data063/_manifest.json` already listed `Button04_NEW.tga`/
  `Button05_NEW.tga` as expected (from `RANOX_PORT_PLAN.md`'s final asset
  sweep, which did a byte-identical June2017→March copy of these two files)
  but never listed a `.TET` for either, and never staged `Button03_NEW.tga`/
  `.TET` at all.

**Fix, verified sourced rather than invented:**

- `DLG_Common_Button04_NEW.tga`/`_05_NEW.tga` in `march2014/update/data063/`
  are **byte-identical** to June2017's own copies (`cmp` confirmed). June2017's
  own `.TET` sidecars for these two files were therefore copied verbatim —
  zero computation, since the pixel layout already matches exactly. Confirmed
  `RANOX_FINALHALL_N/O`, `RANOX_FIRETEMPLE_N/O`, `RANOX_SECRET_DUNGEON_N/O`
  (Button04_NEW) and `RANOX_SECRET_DUNGEON_LOCK` (Button05_NEW) are present.
  Note `Ranox_FIRETEMPLE` has Normal/Over swapped relative to every sibling
  (`SetNormalTex(...,"_O")`, `SetOverTex(...,"_N")`) — transcribed as-is,
  studio/port data, not "corrected."
- `DLG_Common_Button03_NEW.tga`/`.TET` needed a real merge (same technique as
  Phase 4's `Button20_NEW` work). First confirmed the two atlases share a
  common ancestor: all 18 of March's existing keys have **byte-identical**
  UV rects in June's copy of the same file, and pixel content is identical
  up to the exact row where June's Ranox additions begin — so June's own
  chosen coordinates for the 8 Ranox icons already sit in March's unused
  space, no free-space search needed. Copied the 8 rects
  (`RANOX_ASHES_VILLAGE_N/O`, `RANOX_BURNED_FOREST_N/O`, `RANOX_GATEWAY_N/O`,
  `RANOX_GEYSER_N/O`) pixel-for-pixel from June's TGA into March's at the
  same coordinates (`(HEIGHT-1-y)` row-flip per the TGA's bottom-up origin,
  same as Phase 4), leaving March's original 512×512 header/footer and all
  212,764 unrelated pixel bytes untouched (verified byte-for-byte). Rendered
  `RANOX_BURNED_FOREST_N`'s merged alpha channel as ASCII art — a clean tree
  silhouette, not noise. Rebuilt the `.TET` from scratch: 18 original entries
  + 8 new ones (UVs taken verbatim from June's TET, same coordinates the
  pixel copy used), re-sorted by key (required — `GetTexUV` binary-searches
  assuming sorted order), verified by parsing the output back and confirming
  all 26 entries round-trip exactly, the 18 original entries are byte-for-byte
  unchanged, and the key order is sorted.
- All 4 files staged in `march2014/update/data063/`
  (`DLG_Common_Button03_NEW.tga`/`.TET` new, `Button04_NEW.TET`/
  `Button05_NEW.TET` new — `Button04_NEW.tga`/`Button05_NEW.tga` were already
  staged and untouched), `_manifest.json` updated to list all 6 (CRLF,
  2-space indent, no trailing newline, matching the house format). Per
  `.gitignore` (`march2014/** ... !march2014/**/*.lua !.../\_manifest.json`),
  these binary files are intentionally not tracked in git — only the
  manifest update is a real diff; this matches every other binary asset
  already in this tree.

**No C++ changes were needed or made for this half of the fix** — the
`SERV_IRUHADEV_RANOXDBG_DUNGEON` instrumentation added this session stays in
place (still short-lived/removable) since it's what proved the gate logic
was never the problem.

**Correcting Phase 4's own entry**: its "non-blocking, deferred" framing for
the world-map tab's missing `DLG_UI_Button20_NEW.tga`/`.TET` art is still
accurate **for that specific control**, because the tab is a `RadioButton`
with an explicit `SetRBPoint(USE_TEXTURE_SIZE=FALSE, ...)` override — but the
underlying mechanism (an atlas merge that was planned/referenced but never
actually produced) is the same defect that turned out to be fully blocking
here, on the plain `CKTDGUIButton`s that have no such override. The tab's own
atlas gap is still open and still cosmetic-only; don't conflate the two.

**Owed:** pack `data063.kom` from the files above (plus `data034.kom` and
`data056.kom` remain owed from Phase 4, independent of this fix) and
play-test — Ranox's local map should now show all 8 dungeon gate icons,
with BurnedForest selectable at level ≥ 68 per the confirmed `isActive=1`
log line above.

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

**Execution notes, 2026-09-22 — root-caused and fixed in data, not yet
packed/tested.**

- **Mechanism, traced end to end.** A fresh play session's `offline_server.log`
  showed the exact symptom: 26 field NPCs (`id=5154..5164`, `lv=70`) all
  `died: KILL_SELF` within 350ms of "loading complete, play start" — not combat
  damage. Zero `WARNING no rows for class=` lines, confirming
  `CX2OfflineStatTable::GetUnitStat` is not in the path, exactly as this
  phase's own step (1) predicted. The real path is entirely client-side:
  `CX2BattleFieldGame::CreateBattleFieldNpc` (`X2Lib/X2BattleFieldGame.cpp:782`)
  calls `pBattleFieldNpc->SetHardLevel( kNpcUnitData_.m_Level, true )`, which
  reads `GetNPCUnitStat()` = `m_pNPCUnitInfo->m_stat`
  (`X2Lib/X2GUNPC.h:1945`) and computes
  `HP = ( statMaxHP + hardLevel.m_fHP * level ) * fHPScale`
  (`X2Lib/X2GUNPC.cpp:7036`). `m_stat` is populated **only** by
  `CX2UnitManager::AddNPCStat_LUA` (`X2Lib/X2UnitManager.cpp:679`), called from
  Lua; with no row for a given `NPC_ID`, `m_stat` stays default-constructed —
  `m_fMaxHP = 0`, `HardLevel.m_fHP = 0` — so `HP` resolves to exactly `0`.
  `SetNowHp(0)` drives the NPC straight into a dying state; `DyingStart()`
  (`X2Lib/X2GUNPC.cpp:13221`) finds no attacker
  (`GetGameUnitWhoAttackedMe() == NULL`, since nothing hit it) and sends
  `KEGS_NPC_UNIT_DIE_REQ` with reason `NDS_KILL_SELF` — the exact line seen in
  the log.
- **Root cause: the field's own spawn table has no row in `NPCStat.lua` for
  any `NPC_ID` it spawns, the same "silent gap" class of bug
  `RANOX_PORT_PLAN.md` already named for `NPCTemplet.lua` (its own "major
  addendum"), but this is a different registry that addendum never checked.**
  `data037`'s `NPCTemplet.lua` (which supplies `LUA_FILE_NAME`, i.e. which AI
  script runs) already has entries for every relevant `NPC_ID` here — which is
  why the NPCs spawn with working AI at all (uid assigned, `ErrorLog.txt` shows
  no NULL-templet failures). `data037`'s sibling file, `NPCStat.lua` (681KB,
  1817 `AddNPCStat` blocks, confirmed via `elstools decompile-batch` against
  the **live** `data037.kom` — matches the `kom_contents` staging copy
  byte-for-byte, so no staleness) has **zero** rows for the ones this field
  actually spawns. Load order confirmed safe either way:
  `CX2Data::ResetUnitManager` → `CX2UnitManager::OpenNPCFiles`
  (`X2Lib/X2UnitManager.cpp:258-266`) opens `NPCTemplet.lua` before
  `NPCStat.lua`, and `AddNPCStat_LUA` itself bails
  (`X2Lib/X2UnitManager.cpp:697`) if the templet entry isn't there yet — so
  appending stat rows at the end of the file, after the templet is fully
  loaded, is the correct and only shape.
- **Self-correction, same session — the first pass fixed the wrong 9 IDs.**
  7a's own asset-list writeup names the 9 field monster **families**
  (`MAG_CHARGER`, `MAG_NOM`, `RAGE_BABYFIG`, `RAGE_DRYAD`,
  `SMITH_LITTLEWORKER`, `SMITH_MACHINE`, `SMITH_RUNNER`, `SMITH_SCIENCE`,
  `SMITH_WORKER`), and the first attempt at this fix added stat rows for
  exactly those 9 base-name `NPC_ID`s. That was wrong: those base names are
  BurnedForest **dungeon** monsters (still unreachable — Phase 5 has all Lanox
  dungeons locked), not what the Ashen Land field itself spawns.
  `march2014/update/data036/Ranox_Field_00.src.lua`'s own `MONSTER_ID` list
  (the field's actual spawn table, same shape as March's own
  `Sander_Field_03.lua`) names 12 **`_FD`-suffixed** siblings instead:
  `NUI_LOW_MAG_CHARGER_FD`, `NUI_LOW_MAG_NOM_FD`, `NUI_LOW_MAG_STONE_FD`,
  `NUI_LOW_RAGE_BABYFIG_FD`, `NUI_LOW_RAGE_DRYAD_FD`,
  `NUI_LOW_RAGE_FATHERFIG_FD_BOSS`, `NUI_LOW_SMITH_ALUMINUM_FD_BOSS`,
  `NUI_LOW_SMITH_LITTLEWORKER_FD`, `NUI_LOW_SMITH_MACHINE_FD`,
  `NUI_LOW_SMITH_RUNNER_FD`, `NUI_LOW_SMITH_SCIENCE_FD`,
  `NUI_LOW_SMITH_WORKER_FD` — the `_FD` suffix matches 7a's own missing-asset
  filenames (`NUI_LOW_*_FD_A.XET`) and March's own field convention
  (`Sander_Field_03.lua` spawns `NUI_ICOOS_FD`/`NUI_ANU_FD`, not base names).
  10 of these 12 match the session's own distinct `id=` values in
  `offline_server.log` (2 — the `_BOSS` pair — hadn't spawned yet in that
  session; the log's own "middle boss reserved" line shows one was queued).
  All 12 already have `NPCTemplet.src.lua` entries too (confirmed). Caught
  before packing — a stronger reviewer flagged that the base-name fix would
  have survived a full pack/deploy cycle and only failed at play-test, with
  the symptom indistinguishable from having done nothing. **Reverted the
  base-name rows; the shipped fix targets the 12 `_FD` IDs above.** The 9
  base-name rows aren't wrong data (June has real stat rows for them, and
  they'll be needed once Phase 5 unlocks BurnedForest), just not what this
  field needed — left for the deferred catch-up below rather than packed now
  ahead of any evidence they're reachable.
- **Fix (data only, no C++, no flag — same class as Phase 1 Bug A / Phase 3 /
  Phase 4's Lua fixes, not the "server Lua the client needs" rule: this is
  ordinary client `.kom` Lua with no `DoMemory` loader or degrade path).**
  Staged `march2014/data037/NPCStat.src.lua` (new file — patches an existing
  March file in place, same convention as `NPCTemplet.src.lua`) as the live
  `data037.kom`'s own `NPCStat.lua` (confirmed byte-identical prefix) plus 12
  appended `AddNPCStat` blocks, copied **verbatim** from June2017's own
  `data037_kom/NPCStat.lua` (`MAX_HP = 8620`, `HARD_LEVEL.HP = 1297.6`,
  identical for all 12 — studio's own repeated-block data, transcribed not
  tidied), one per `NPC_ID`, in the order June's file has them.
- **Sanity-checked the magnitude before trusting June's numbers** (per the
  risk that a 2014→2017 stat rescale could ship unkillable monsters instead of
  dead-on-spawn ones). Compared against `NUI_ICOOS_FD`, one of March's own
  existing Sander Field 03 spawns (`STANDARD_MONSTER_LEVEL = 68`, the field
  immediately before Ranox's own `STANDARD_MONSTER_LEVEL = 70`, and itself an
  `_FD`-suffixed field monster — the correct comparison family): March's own
  data gives it `MAX_HP = 12320`, `HARD_LEVEL.HP = 1848`, resolving to
  `12320 + 1848*68 ≈ 138,000` HP at level 68. June's numbers for the 12 Lanox
  field monsters resolve to `8620 + 1297.6*70 ≈ 99,000` HP at level 70 — same
  order of magnitude as a comparable existing March field monster, not 10× off
  in either direction. Passed; shipped as-is.
- **Verified additive and round-trip clean**, same bar as `NPCTemplet.lua`'s
  own precedent: the new file's first 962,773 bytes (49059 lines) are a byte-
  for-byte prefix match against the live `data037.kom`'s own `NPCStat.lua`;
  compiled clean via `elstools compile-batch -v mar-2014`; decompiled the
  compiled output back and confirmed all 1817 pre-existing blocks parse to the
  identical key/value content as the live file (some keys reordered by the
  decompiler — cosmetic, not semantic, same as `NPCTemplet.lua`'s prior
  finding on this), and the 12 new blocks carry the intended `NPC_ID`s with
  `MAX_HP = 8620` / `HARD_LEVEL.HP = 1297.6`.
- **Found, not fixed here — a wider version of the same registry gap.**
  Cross-referencing every `NPC_ID` in `NPCTemplet.src.lua` against the live
  `NPCStat.lua` found 242 templet entries with no stat row. Filtered against
  June's own `NPCStat.lua` to separate signal from noise: **182 are non-combat
  NPCs missing a stat row in June's client too** (village NPCs like
  `NUI_CAMILLA`/`NUI_ARIEL`/`NUI_BILLBOARD`, event props, etc. — by design,
  not a bug, since they never take damage). The remaining **60 are genuine
  port gaps** — `NPC_ID`s with a real stat row in June that never made it into
  March's `NPCStat.lua`.

- **Second, wider pass, same session — "Lanox field" is a 4-field chain, not
  one field, and the first pass only fixed field 1 of 4.** The diagnostic
  session only walked into `Ranox_Field_00.lua`'s own zone (the entrance —
  `BATTLE_FIELD_ID = VMI_BATTLE_FIELD_RANOX_FIELD_01`). But
  `march2014/data036/BattleFieldData.src.lua` registers three more, chained by
  portal (`Field_00`'s own `LINE_NUMBER=63` portal → `Field_02`'s
  `BATTLE_FIELD_ID`, i.e. `Ranox_Field_01.lua`; that field's own portal →
  `Ranox_Field_02.lua`; that field's own portal →
  `Ranox_Field_03.lua`) — all reachable by walking the same zone forward from
  where the diagnostic session stopped, not a separate dungeon requiring
  Phase 5. Checking all 4 `march2014/update/data036/Ranox_Field_0{0..3}.lua`
  files' own `MONSTER_ID` spawn tables found **33 distinct `NPC_ID`s total**,
  all with `NPCTemplet.src.lua` rows (none would fail to spawn), but only the
  12 in `Field_00` had been fixed. Fixed 13 more from the same verbatim-from-
  June approach, same family (`MAX_HP = 8620` / `HARD_LEVEL.HP = 1297.6`,
  confirmed identical to the already-shipped 12, so the sanity check above
  covers these too without rechecking):
  - `Field_02` (`Ranox_Field_01.lua` → `Ranox_Field_02.lua`'s spawn table),
    7 IDs: `NUI_LOW_MOUNT_NOM_FD`, `_STONE_FD`, `_CHARGER_FD`, `_FIREBIRD_FD`,
    `_FIGHTER_FD`, `_BERSERKER_FD`, `_BERSERKER_BOSS`.
  - `Field_03` (`Ranox_Field_02.lua` → `Ranox_Field_03.lua`'s spawn table),
    6 IDs: `NUI_LOW_FINALHALL_FIGHTER_FD`, `_BERSERKER_FD`, `_SNIKER_FD`,
    `_FIREBIRD_FD`, `_STONE_FD`, `_IGNIA_BOSS`.
  Verified the same way as the first 12: appended to
  `march2014/data037/NPCStat.src.lua`, prefix-match confirmed against the
  live `.kom` still holds, compiled clean, round-tripped with 0 semantic
  mismatches across all 1817 pre-existing blocks. **File now carries 25 new
  rows total (12 + 13), 1842 `AddNPCStat` blocks overall.**
- **8 IDs found that this fix cannot cover — no source data exists in either
  client.** `Ranox_Field_01.lua`'s own spawn table (the field between
  `Field_00` and `Field_02`) names 8 `NUI_LOW_GEYSER_*_FD` monsters
  (`_ALUMINUM_FD_BOSS`, `_LITTLEWORKER_FD`, `_MULLOCK_CALL_FD`, `_MULLOCK_FD`,
  `_MULMAI_FD`, `_RUNNER_FD`, `_SCIENCE_FD`, `_WORKER_FD`). All 8 have
  `NPCTemplet.src.lua` rows (they spawn), but **neither March's live
  `NPCStat.lua` nor June2017's own copy has a stat row for any Geyser
  monster at all — base name or `_FD` — confirmed by direct search of both
  files.** This is not a port gap the same shape as the other 25: there is
  nowhere to transcribe from. Per this project's own rule against hardcoded
  tables, invented curves, or guessed values, **no stat rows were added for
  these 8, and none should be until a real source turns up** (a different
  client snapshot, or the user's own live-DB/asset access, per the
  `DataBase/`/`ScriptData/` rule in `CLAUDE.md` if this ever turns out to be
  server-sourced data after all — unlikely here since `NPCStat.lua` is
  ordinary client `.kom` Lua, not server data, but worth ruling out before
  calling this permanently blocked). **These 8 will still die on spawn with
  `KILL_SELF` after this fix is packed** — walking far enough into the Lanox
  field chain to reach `Ranox_Field_01.lua`'s zone will reproduce the exact
  same symptom this phase just fixed, for these 8 specifically. This is a
  known, named gap, not a regression.
  **Superseded 2026-09-26/27 — see Phase 10 and `LANOX_EXTRA_NPC_STAT_PLAN.md`.**
  This bullet only searched the legacy `NPCStat.lua`. June's second stat
  system, `NewNPCStat.lua`, has a row for all 8 — 7 confirmed spawning
  `src=extra` with no `KILL_SELF` in Phase 5's session 1 play-test, the 8th
  (`5172`) not yet observed spawning but carrying the same row as its
  siblings.
- **Found, not fixed here — the still-wider version of the same registry
  gap.** Cross-referencing every `NPC_ID` in `NPCTemplet.src.lua` against the
  live `NPCStat.lua` originally found 242 templet entries with no stat row;
  filtered against June's own `NPCStat.lua`, 182 are non-combat NPCs missing a
  stat row in June's client too (by design, not a bug) and 60 are genuine
  port gaps. This session's 25 (Field_00's 12 + Field_02/03's 13) are now
  fixed. **35 remain** — the FireTemple/BurnedForest families
  (`NUI_FIRE_DEVIL_*`, the 9 BurnedForest base names this session's first,
  reverted pass targeted, `NUI_MOUNT_FIREBIRD_FIRETEMPLE`,
  `NUI_FIRETEMPLE_GATE`/`_REGIEON_POINT`) — none reachable yet per Phase 5's
  dungeon lock, so none has produced an observed symptom. The 8 Geyser `_FD`
  IDs above are a **separate, permanently-blocked** bucket (no source data),
  not part of this 35. Recommend a dedicated catch-up pass once Phase 5 is
  closed and those dungeons are actually playable, the same shape as this
  note and as `RANOX_PORT_PLAN.md`'s own still-open `NPCTemplet.lua` catch-up
  recommendation.
  **Superseded 2026-09-26 — see Phase 10 and `LANOX_EXTRA_NPC_STAT_PLAN.md`.**
  These 35 (and the Geyser 8, and every other Lanox dungeon NPC ID with no
  legacy row) are covered by `NewNPCStat.lua`, confirmed by the plan's own
  audit: 0 non-Lanox dungeon IDs and 0 remaining Lanox dungeon IDs lack
  either a legacy or an extra row. FireTemple/the rest of BurnedForest hadn't
  been play-tested as of this note, so this is a resolved *data* gap, not yet
  a confirmed *in-game* one for those specific dungeons.
- **Owed:** `march2014/data037/NPCStat.src.lua` needs `elstools compile-batch
  -v mar-2014` (produces `NPCStat.lua`) and packing that into `data037.kom` —
  the user's step, per this project's established convention. Then play-test:
  walk the full Lanox field chain (`Field_00` → `Field_02` → `Field_03`, all
  reachable by portal from the entrance already tested), confirm every
  monster in those three fields' spawn tables takes visible damage over more
  than one hit and survives contact briefly rather than dying on spawn —
  including the four `_BOSS`/boss-tier variants
  (`NUI_LOW_RAGE_FATHERFIG_FD_BOSS`, `NUI_LOW_SMITH_ALUMINUM_FD_BOSS`,
  `NUI_LOW_MOUNT_BERSERKER_BOSS`, `NUI_LOW_FINALHALL_IGNIA_BOSS`) that may
  need a longer play session or a middle-boss spawn to trigger. **Do not
  expect `Ranox_Field_01.lua`'s zone (the Geyser-monster field) to be fixed**
  — confirm its monsters still die with `KILL_SELF` and that this matches the
  known, documented gap above, not a new regression. Grep the post-play-test
  `offline_server.log` for `KILL_SELF` — any hit **outside** the 8 known-
  blocked Geyser IDs names a `NPC_ID` this fix missed.
  **Superseded 2026-09-27 — see Phase 10.** `Ranox_Field_01.lua`'s Geyser
  monsters are fixed too, by a different mechanism (`NewNPCStat.lua` via
  `LANOX_EXTRA_NPC_STAT_PLAN.md`, not this bullet's `NPCStat.lua` rows); 7 of
  8 confirmed `src=extra` with no `KILL_SELF` in play.

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

## Phase 10 — Lanox "phase clears with mobs alive" [the Burned Forest gate defect]

**Paste with:** Starting context + this section, or just read
`LANOX_EXTRA_NPC_STAT_PLAN.md` directly — the whole fix lives there, not here.

**Status: base fix done, built, and play-tested 2026-09-27; extended
verification paused mid-way (session 1 of an unfinished checklist).**

This phase points at a separate, self-contained document,
`LANOX_EXTRA_NPC_STAT_PLAN.md`, rather than duplicating its ledger here. One
paragraph: the Lanox sub-stage "clears while other monsters are still alive"
symptom (first seen as part of issue 7's field-monster investigation, below)
turned out to be a general Lanox defect, not limited to fields. Root cause:
75 Lanox NPC IDs (gates, switch gates, traps, regen points, Geyser/Fire
Devil/Mount/event-boss/Secret monsters) have no row in either client's legacy
`NPCStat.lua` — their real stats live only in June2017's **second** NPC stat
system, `data036/NewNPCStat.lua` + `NewNPCStatFormula.lua`
(`AddExtraNPCStat`/`CalculateExtraStatValue`), which this March build never
loaded. With no row, these NPCs spawn at 0 HP and immediately self-kill
(`KILL_SELF`), which can satisfy a sub-stage's `CC_KILL_NPC_TYPE` clear
condition (a gate "dying" of 0 HP) while the sub-stage's other, legitimate
mobs are still alive. `SERV_IRUHADEV_EXTRA_NPC_STAT` (base flag) loads
June's two files and gives these 75 IDs real stats, fixing the stage-skip.
A second flag, `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` (nested inside the
base flag, on by default), extends the same formula stats to the other 772
Lanox-and-elsewhere NPC IDs that *do* have a legacy row, matching June's own
priority rule that an `ExtraNPCStat` row always wins. Both flags are defined
today; the parity keep/revert decision is not yet made (see
`LANOX_EXTRA_NPC_STAT_PLAN.md` Phase 5).

**Supersedes Phase 7d's "no source data exists" / "35 remain" findings
below** (2026-09-26/27) — those searched only the legacy `NPCStat.lua`; the
real source is `NewNPCStat.lua`. Specifically:

- **The 8 `NUI_LOW_GEYSER_*_FD` monsters are not an unsourceable, permanently
  blocked gap.** All 8 have `NewNPCStat.lua` rows. Play-tested 2026-09-27
  (`LANOX_EXTRA_NPC_STAT_PLAN.md` Phase 5, session 1): walking into
  `Ranox_Field_01.lua`'s zone, **7 of the 8** spawned this session
  (`5166,5169,5168,5171,5170,5173,5167`), all `src=extra`, zero `KILL_SELF`,
  zero `[NPC_ZERO_HP]`. The 8th, `5172`
  (`NUI_LOW_SMITH_ALUMINUM_FD_BOSS`), simply didn't spawn this session
  (spawn-rate variance, a boss-tier monster) — not yet directly observed
  spawning, but it has the same `NewNPCStat.lua` row as its 7 siblings and
  there's no reason to expect it behaves differently.
- **The "35 remain" registry gap (FireTemple/BurnedForest families:
  `NUI_FIRE_DEVIL_*`, the 9 BurnedForest base names, `NUI_MOUNT_FIREBIRD_
  FIRETEMPLE`, `NUI_FIRETEMPLE_GATE`/`_REGIEON_POINT`) is resolved
  statically, not confirmed in play.** Per `LANOX_EXTRA_NPC_STAT_PLAN.md`'s
  audit, every Lanox dungeon NPC ID that appears in any
  `Dungeon_Ranox_*.src.lua` now has either a live legacy `NPCStat.lua` row or
  a `NewNPCStat.lua` row — the 57-ID gap against legacy is fully covered by
  the 75-ID extra-only set (a superset, since it also covers non-Lanox-named
  IDs). Most of these 35 IDs sit in dungeons (FireTemple, and the rest of
  BurnedForest) that hadn't been played as of this entry — see Phase 5's
  "still open" list — so treat this as "the data gap is closed," not as "the
  in-game behavior is confirmed," until those dungeons are actually visited.
- **The "do not expect `Ranox_Field_01.lua` to be fixed" note (Phase 7d
  "Owed" bullet, and the batching/acceptance notes below) no longer holds** —
  see the Geyser bullet above.

**What's still open** (not fixed here, tracked in
`LANOX_EXTRA_NPC_STAT_PLAN.md`'s own Phase 5 and "Out of scope /
follow-ups"): the remaining Lanox dungeons/battlefield paths Phase 5's
checklist hadn't reached as of this entry (Geyser, Gateway, FireTemple,
FinalHall, Secret_Hell, every Hard/Expert tier, `path=battlefield782`), the
parity keep/revert decision, monster grade not reaching dungeon-spawned NPCs
offline (so Lanox/parity bosses get normal-monster scan values), and whether
June's own Secret-dungeon classifier (`FUN_32931950`) agrees with March's
`IsSecretDungeon` (open, interim behavior documented in the plan).

**Kickoff prompt**:
```
Continuing the Lanox/Ranox defect fixes. This defect (phase clears with
mobs alive) is tracked entirely in LANOX_EXTRA_NPC_STAT_PLAN.md, not here --
read that file's "Starting context" and whichever phase's Status line says
it's not done yet, and continue from there. Branch: mods/lanox-port.
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
batch 1 doesn't already resolve it via Phase 4. Phase 7d now has a fix
staged (`march2014/data037/NPCStat.src.lua`, 25 rows across 3 of the 4
chained Lanox fields — the 4th, `Ranox_Field_01.lua`'s Geyser monsters,
remains a documented, unsourceable gap) — fold it into the Lua batch above
(add it to `compile-batch`'s file list) rather than running it alone, since
it's the same data-only shape as batch 1's other fixes.
**Superseded 2026-09-27 — see Phase 10.** The Geyser monsters are not
permanently unsourceable; `NewNPCStat.lua` via `SERV_IRUHADEV_EXTRA_NPC_STAT`
covers them (7 of 8 confirmed in play), independent of this Lua batch.
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
and `ErrorLog.txt` is clean of the 18 assets (done); the Lanox field chain's
monsters survive more than one hit rather than dying on spawn in
`Ranox_Field_00`/`_02`/`_03.lua` (Phase 7d, 25 IDs fixed in data 2026-09-22 —
`march2014/data037/NPCStat.src.lua` needs compiling and packing and
play-testing before this criterion can be marked met) — **and**, per Phase 10,
`Ranox_Field_01.lua`'s 8 `NUI_LOW_GEYSER_*_FD` monsters too (7 of 8 confirmed
`src=extra` with no `KILL_SELF`, via `NewNPCStat.lua`, not this batch's
`NPCStat.lua` rows — no longer excepted as unsourceable); the Behemoth Crater
quest-guide particle appears when toggled on (the physical signboard itself
is confirmed out of scope — art task, not a bug); the leftmost Lanox teleport
lands at the crater's end.
