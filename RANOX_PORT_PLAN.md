# Ranox/Lanox Village + Dungeon Port Plan

Porting the "Ranox" (aka "Lanox") content chapter from a June-2017 Elsword
client into this repo's March-2014 client (`US_SERVICE`/`US_INTERNAL`).

## Do not consult

A prior attempt at this exact port (`NEW_VILLAGE_PLAN.md`,
`NEW_VILLAGE_RANOX_IDENTIFIERS.md`, and a `mods/lanox-offline` branch) was
declared a failure by the user. **Do not open, cite, or build on any of the
three.** Everything in this document was re-derived from scratch, read-only,
directly out of the two client data trees below — not from that prior work,
and not from this repo's C++ source (`X2Lib`/`KTDXLIB`/`KncWX2Server`), which
was deliberately not consulted while writing this plan either. If a phase
below turns out to need repo-side (C++) changes, that's flagged explicitly as
its own separate, later phase group — do not assume this document knows
anything about repo internals beyond what's stated.

## Source trees & tooling

- **March-2014 client** (the port target): `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents`
  — folders named `dataNNN` (no suffix), one per `dataNNN.kom`, already extracted.
- **June-2017 client** (the port source): `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data`
  — folders named `dataNNN_kom`, already extracted. The 125 Ranox `.lua`
  files in this tree were bulk-decompiled during planning research (2026-09-18)
  to `.src.lua` siblings next to each original — check whether they still
  exist before re-running the decompiler.
- **Decompiler/compiler**: `elstools/elstools.exe` in this repo.
  - `elstools.exe decompile-batch -i <file.lua>` → writes `<file>.std.luac`
    (intermediate, ignorable) and `<file>.src.lua` (readable) next to the input.
    Repeat `-i` for multiple files in one invocation.
  - `elstools.exe compile-batch -v mar-2014 -i <file.lua>` → compiles a
    plain-text `.lua` source file to March-2014-vintage KOG LuaJIT bytecode
    **in place** (overwrites the input file) — copy to scratch first if you
    need to keep the source readable. `-v jun-2014` targets the newer
    bytecode format instead (not needed for this port).
  - Verified this session: decompiling a June2017 file, then compiling it
    back with `-v mar-2014`, produces a file with the correct `0x81`
    mar-2014 bytecode header, which decompiles back to content identical to
    the original (only Lua table key *order* differs — semantically inert
    for named-key table literals). Tested on both a plain scene file and one
    with embedded functions (`RANOX_ASHESVILLAGE_GATE.lua`). **The pipeline
    works end to end.**
- **Compiling back to bytecode and packing `.kom` archives is the user's job**,
  not something any phase here should attempt. Every phase's output is plain,
  human-readable `.lua` text (or, for assets, the raw file copied byte-identical).

## Output convention — where ported/edited files go in this repo

Everything lands under `./march2014/` in this repo, tracked in git
(`.gitignore` was amended to add `!/march2014/` — an allowlist-style
gitignore that ignores everything by default, so this line is required or
the whole deliverable would be invisible to git). Two sub-conventions,
depending on whether the file already exists in the March-2014 client:

1. **Patch to an existing March2014 file** → `march2014/dataXXX/<name>.lua`,
   where `dataXXX` is **March2014's own** existing archive folder for that
   file (matching `kom_contents`'s own naming, no `_kom` suffix). This
   applies only to the small set of shared registry files being edited in
   place: `Enum.lua`, `DungeonEnum.lua`, `EnumString.lua`, `MapData.lua`,
   `WorldData.lua`, `DungeonData.lua`, `BattleFieldData.lua`,
   `PVP_Map_List.lua` — all of which live in March2014 archives `data035`
   or `data036` or `data040` (see Appendix D). The user packs this back into
   that same archive, replacing the file it already had.
2. **New file with no March2014 counterpart at all** →
   `march2014/update/dataXXX/<name>`, where `dataXXX` is the file's
   **source June2017** archive number, kept as-is even where March2014 has
   no archive of that number at all (e.g. `data217` — it simply becomes a
   new archive added to the March2014 client too, rather than being
   remapped to wherever equivalent March2014 content happens to live).
   This covers every ported village/dungeon/field/secret/PVP scene+LineMap
   script, every new monster/object unit-definition file, every new UI
   dialog, and every non-Lua asset (mesh/texture/motion/audio) — i.e. almost
   everything in this port. Example: `march2014/update/data040/Ranox_Village.lua`,
   `march2014/update/data154/<mesh/texture files>`,
   `march2014/update/data217/DLG_Map_Local_Ranox.lua`.

In both cases, save under the file's **final** name (`MapData.lua`, not
`MapData.src.lua`) — plain decompiled Lua text (or the raw asset bytes),
ready for the user's own compile/pack step.

## Key findings (from this session's research — verified, not assumed)

### The Lua data schema is compatible across the 3-year gap, not just similar

Checked against March2014's own closest structural analog,
`Dungeon_Sander_Karuso_Village_{Normal,Hard,Expert}.lua` (see "Template
village" below): March2014 **already** supports every mechanic Ranox's
dungeon files use — flat `NPC_GROUP` arrays (not just the grouped
`NPC_GROUP0/1/2` + `NPC_GROUP_RATE` form), `TRIGGER_LIST`, `MONSTER_GRADE`,
`BOSS_GAUGE_HP_LINES`, `SHOW_BOSS_NAME`, `HAVE_BOSS_GAGE`, `FOCUS_CAMERA`,
`STOP_AT_START_STATE`, and `CLEAR_CONDITION.CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC`.
This was a genuine surprise given the age gap and is the single biggest
reason this port is tractable rather than a rewrite.

### ID collisions are essentially zero — adopt June2017's IDs verbatim

Checked every numeric value Ranox uses in June2017's `WORLD_ID`,
`NPC_UNIT_ID`, `VILLAGE_MAP_ID`, and `DUNGEON_ID` tables against every value
already assigned to something else in March2014's own copies of those same
tables (full table diff, not sampling):

- **`DUNGEON_ID`**: Ranox's Normal/Hard/Expert block is `33270`–`33322`.
  March2014's own content in that numeric neighborhood stops at `33261`
  (Sander / Hamel-secret). **Zero collisions.** Ranox's "Secret Hell"
  (`41270`/`41271`) and "Hero" tier (`100420`+ / `110200`+) ranges are
  entirely unused in March2014 too.
- **`WORLD_ID`**: 43 Ranox-named entries, **zero collisions**.
- **`VILLAGE_MAP_ID`**: 8 Ranox-named entries, **zero collisions**.
- **`NPC_UNIT_ID`**: of 145 unit IDs actually *referenced* by Ranox content
  (found by grepping every `NPC_UNIT_ID.NUI_*` token out of all 125
  decompiled files — not by filename-matching "ranox", which would have
  missed almost all of them; see Appendix B), **10 are pre-existing shared
  NPCs already identical in both clients** (reuse as-is, zero new work),
  **135 are genuinely new**, and of those, **exactly one** collides if
  adopted at its June2017 numeric value: `NUI_EDEL = 10067` collides with
  March2014's existing `NUI_EVENT_ADAMS_UI_SHOP = 10067`. Every other new ID
  can be copied verbatim.

**Practical implication**: Phase 1 is not "invent ~180 new IDs and hope
they're right" — it's "copy ~178 IDs verbatim out of June2017's own enum
files, give `NUI_EDEL` a fresh value instead of `10067`, done."

### "Hero" difficulty tier: out of scope by default

`DUNGEON_ID` has a `_HERO_{NORMAL,HARD,EXPERT,HELL}` variant for every Ranox
dungeon line, but **no separate content files exist anywhere in the June2017
tree** for it (no `Dungeon_Ranox_*_Hero*.lua`). It's a registry-only variant
— same Expert-tier stage files, a different `DUNGEON_ID`, presumably
server-side scaling. Whether March2014's engine has any concept of "Hero
mode" as a client feature is unverified and almost certainly not present —
**deferred to an optional final phase** (see Phase 12).

### Engine-gap audit: 8 missing enum members, all isolated to specific files

Every `TABLE.MEMBER`-shaped token referenced anywhere across all 125 Ranox
files was extracted and checked for existence by name in March2014's
`Enum.lua` (exhaustive, ~278 distinct references across ~28 tables — not
sampled). All but 8 line up completely:

| Missing member | File(s) it's used in | Affects phase |
|---|---|---|
| `LINE_TYPE.LT_FIRE_GROUND` | `PVP_Ranox_GateWay_LineMap.lua` | 11 (PVP) |
| `TRIGGER_ETC_ACTION_TYPE.TAT_ETC_NPC_AI_ON_OFF` | `Ranox_FireTemple_Stage1_LineMap.lua` | 6 (FireTemple) |
| `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE` | `Ranox_FireTemple_Stage1_LineMap.lua`, `Ranox_Secret_Stage2_LineMap.lua` | 6, 9 |
| `TRIGGER_ETC_ACTION_TYPE.TAT_ETC_PLAY_EFFECTSET` | `Ranox_Geyser_stage0_linemap.lua` | 7 (Geyser) |
| `TRIGGER_ETC_ACTION_TYPE.TAT_ETC_TALK_BOX` | `Ranox_Geyser_stage0_linemap.lua` | 7 |
| `TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE` | `Ranox_Geyser_stage0_linemap.lua`, `Ranox_GateWay_Stage1_LineMap.lua` | 7, 8 |
| `CARTOON_TEX_TYPE.CTT_RIM_LIGHT_BLUE` | `Ranox_Secret_Boss_Stage.lua` | 9 (Secret) |
| `DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE` | `DLG_Map_Local_Ranox_Icon_NEW.lua` | 12 (UI) |

**None of these appear in the Village, AshesVillage, BurnedForest, FinalHall,
or the open Fields** — which is why the phase order below front-loads those
clean lines. Closing these 8 gaps (confirming whether the underlying C++
feature already exists dormant, or needs adding) is repo/engine work,
covered in its own separate phase group at the end of this document — none
of it should block a first playable line.

**Known pre-existing anomaly, not something this port introduces**: Ranox
content references `NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS`, but that exact
name is **not defined anywhere in June2017's own `Enum.lua`** either — it's
a dead/typo'd reference in the source client itself. The similarly-named
`NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL` *is* defined and is almost certainly
what was meant. Whichever phase (5 or 6) hits this should resolve it as a
typo, not re-investigate it as a new mystery.

### Template village: Sander / Karuso Village

Closest existing structural analog in March2014: 6 dungeon SubAreas ×
Normal/Hard/Expert (18 files, no seasonal/secret clutter diluting the
pattern — Feita/Peita is a close runner-up with the same count but lacks
the next feature), and critically, one of its SubAreas (`Karuso_Village`)
is itself a named village-hub with its own overworld scene/LineMap/gate/UI
files — exactly the "village that is also a dungeon-line entry point" shape
Ranox needs. Reference its files whenever a schema question comes up in a
later phase, rather than re-deriving the answer:
`data035/Dungeon_Sander_Karuso_Village_{Normal,Hard,Expert}.lua`,
`data040/Sander_Karuso_Village_Stage{0,1,2,Boss}.lua` (+ `_LineMap`/`_linemap`
companions, note the inconsistent casing), `data037/Sander_Karuso_Village_Gate.lua`,
`data037/Sander_Karuso_Village_Clear_NPC.lua`.

### Monster stat/drop tables: server-side, not in this port's scope to invent

Neither client has a per-dungeon monster stat/drop table — only a generic
`CharacterStatTable.lua` exists in both. Ranox monster stats/drops are
presumably server-side, matching this repo's own documented convention
(`CLAUDE.md`) that server-owned Lua the client needs is the user's to author
and pack. This plan **names** that dependency (Phase 13) but does not invent
numbers for it.

## Phase list

Ordering rationale: get one clean, engine-gap-free vertical slice playable
first (Village + AshesVillage + world-map entry), then broaden to the other
gap-free lines, then the lines needing engine-gap workarounds, then the raid
dungeon, then fields/PVP/polish, then optional/deferred content, then the
asset sweep, then repo/engine integration as its own later group.

Every phase below ends with a ready-to-paste kickoff prompt. Paste the block
verbatim into a new conversation to start that phase with zero prior context
needed.

---

### Phase 0 — Setup (this conversation; already executed on plan approval)

Add `!/march2014/` to `.gitignore` (done); create this document (done).

---

### Phase 1 — ID/enum allocation

**Goal**: add every new identifier Ranox needs into March2014's own enum
files, using June2017's numeric values verbatim except where noted.

**Source**: June2017 `data036_kom/Enum.lua`, `DungeonEnum.lua`,
`EnumString.lua` (decompile if `.src.lua` siblings don't already exist).
**Target** (patches — use `march2014/dataXXX/`, not `update/`):
`march2014/data036/Enum.lua`, `march2014/data036/DungeonEnum.lua`,
`march2014/data036/EnumString.lua`.

**What to add** (full verbatim name+value lists in **Appendix C.5**; the
135 `NPC_UNIT_ID` names are in Appendix B):
- `NPC_UNIT_ID`: all 135 new names (Appendix B) at their June2017 numeric
  values, **except** `NUI_EDEL` — use **`10068`** (June2017's `10067`
  collides with March2014's existing `NUI_EVENT_ADAMS_UI_SHOP`; `10068` is
  confirmed free).
- `WORLD_ID`: all 43 Ranox-named entries, verbatim values — full list in
  Appendix C.5.
- `VILLAGE_MAP_ID`: all 8 Ranox-named entries, verbatim values — full list
  in Appendix C.5.
- `DUNGEON_ID` (in `DungeonEnum.lua`): the 19 Normal/Hard/Expert +
  Secret-Hell/Common entries, verbatim — full list in Appendix C.5. Skip the
  Hero-tier entries (also listed in Appendix C.5 for completeness) —
  deferred, see Phase 12.
- `EnumString.lua`: whatever display-name-string entries pair with the new
  `DUNGEON_ID`/`WORLD_ID` values in June2017's own `EnumString.lua` — check
  its structure once decompiled; it's likely a simple ID→display-string map.

**Also note while here**: the `NUI_FIRE_DEVIL_IGNIA_BOSS` anomaly above —
resolve it as `NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL` wherever it's referenced,
don't allocate a new ID for it.

**Done when**: `march2014/data036/{Enum,DungeonEnum,EnumString}.lua` contain
every new entry, and a quick diff against March2014's original confirms no
existing entry was accidentally altered.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 1" and Appendix B and C.5 (the
latter has every WORLD_ID/VILLAGE_MAP_ID/DUNGEON_ID name+value pair verbatim
-- use it directly, don't re-derive from the client trees). Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch -- that was a prior, abandoned attempt at this same
port and is not authoritative for anything.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents
(folders dataNNN, no suffix). June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data
(folders dataNNN_kom; June2017's Ranox-related .lua files may already be
decompiled to .src.lua siblings from prior research -- check first).
Decompiler: elstools/elstools.exe decompile-batch -i <file> and
compile-batch -v mar-2014 -i <file> (compiles in place -- copy to scratch
first to keep the source).

Your task: decompile June2017's data036_kom/Enum.lua, DungeonEnum.lua, and
EnumString.lua. Add every new identifier listed in RANOX_PORT_PLAN.md's
Phase 1 section, Appendix B (NPC_UNIT_ID names), and Appendix C.5
(WORLD_ID/VILLAGE_MAP_ID/DUNGEON_ID name+value pairs, verbatim -- use them
directly) to March2014's copies of those three files (decompile March2014's
data036/Enum.lua, DungeonEnum.lua, EnumString.lua first), using June2017's
numeric values verbatim except NUI_EDEL -- use 10068 (June2017's 10067
collides with March2014's existing NUI_EVENT_ADAMS_UI_SHOP; 10068 is
confirmed free). Write the full modified files to
./march2014/data036/Enum.lua, ./march2014/data036/DungeonEnum.lua, and
./march2014/data036/EnumString.lua in this repo (plain Lua text, final
filenames, not .src.lua -- these patch existing March2014 files so they use
march2014/data036/ directly, not march2014/update/).
```

---

### Phase 2 — Village hub

**Goal**: get `Ranox_Village.lua` loadable in the March2014 client and
registered on the world map.

**Source → Target**:
- `data040_kom/Ranox_Village.lua` → `march2014/update/data040/Ranox_Village.lua`
- `data040_kom/Ranox_Village_LineMap.lua` → `march2014/update/data040/Ranox_Village_LineMap.lua`
- Skip for now (deferred to Phase 12): `Ranox_Village_Festival.lua`,
  `Ranox_Village_Halloween.lua`, `Ranox_Village_XMAS.lua`,
  `Ranox_Village_XMAS_2.lua`, `Ranox_Village_luminarie.lua`,
  `Spa_Ranox*.lua` (3 files).
- Registry additions: `march2014/data040/WorldData.lua`,
  `march2014/data036/MapData.lua` (patches).
- World-map UI: `data034_kom/DLG_UI_FIELD_MAP_Ranox_Village_NEW.lua` →
  `march2014/update/data034/...`. The two world-map background files that
  sit in June2017's `data217_kom` (`DLG_Map_Local_Ranox.lua`,
  `DLG_Map_Local_Ranox_Icon_NEW.lua`) go to
  `march2014/update/data217/...` — keep June2017's own archive number, do
  **not** remap to `data034` (see Output Convention above).

**Open question this phase must answer before porting scene geometry**:
`Ranox_Village.lua` is pure `g_pWorld:Create*` scene geometry — no NPC or
portal placement was found in it during this session's research, and no
separate Ranox village-NPC placement file turned up in the 125-file
inventory either (Appendix A). **How does a player actually get from the
village into a dungeon line?** Candidates worth checking in the June2017
tree: an NPC dialog script referencing `WORLD_ID`/`DUNGEON_ID` (check the
village-portrait NPCs found in Appendix B's "VILLAGE" bucket — `NUI_EDEL`,
`NUI_FACEOFF`, `NUI_IGNIA`, `NUI_STEEL`, `NUI_SYOTA`, `NUI_MUL_MAI_EVENT` —
these look like story/quest NPCs, not monsters, and might be the entry
points); a portal object placed by a non-Ranox-named file; or an
engine-level village→dungeon list keyed off `VILLAGE_MAP_ID` that lives
outside any single Lua file. **Find and confirm the actual mechanism before
assuming "the village loads" and "you can reach AshesVillage from it" are
the same milestone** — they may not be.

**Done when**: village scene loads in-game (or in whatever verification
harness exists once repo integration lands), and the village→dungeon entry
mechanism is documented (even if it turns out to need repo-side work to
finish).

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 2". Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch -- that was a prior, abandoned attempt at this same
port and is not authoritative for anything.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents
(folders dataNNN). June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data
(folders dataNNN_kom; check for already-decompiled .src.lua siblings first).
Decompiler: elstools/elstools.exe decompile-batch -i <file> and
compile-batch -v mar-2014 -i <file> (in place -- copy to scratch first).
Phase 1 should already have landed march2014/data036/Enum.lua etc with the
new WORLD_ID/VILLAGE_MAP_ID/NPC_UNIT_ID entries this phase needs -- check
./march2014/data036/ first; if it's not there, do Phase 1 first or inline
whatever subset of it this phase needs.

Your task: (1) FIRST, investigate how a player reaches a dungeon from the
Ranox village in the June2017 client -- Ranox_Village.lua is pure scene
geometry with no NPC/portal placement found in prior research; find the real
mechanism (quest NPC? portal object in another file? something else) before
porting anything, and document what you find. (2) Port
data040_kom/Ranox_Village.lua and Ranox_Village_LineMap.lua to
./march2014/update/data040/ (skip the Festival/Halloween/XMAS/XMAS_2/
luminarie/Spa variants -- deferred). (3) Add Ranox's village entry to
March2014's data040/WorldData.lua and data036/MapData.lua, writing the full
patched files to ./march2014/data040/WorldData.lua and
./march2014/data036/MapData.lua. (4) Port
data034_kom/DLG_UI_FIELD_MAP_Ranox_Village_NEW.lua to
./march2014/update/data034/, and data217_kom/DLG_Map_Local_Ranox.lua +
DLG_Map_Local_Ranox_Icon_NEW.lua to ./march2014/update/data217/ (keep the
217 number as-is, do not remap to data034).
```

---

### Phase 2 findings (executed 2026-09-18)

**The village→dungeon mechanism, answered.** `Ranox_Village.lua` and
`Ranox_Village_LineMap.lua` are, as suspected, pure scene geometry — grepping
both for `NPC|Portal|Gate|WORLD_ID|DUNGEON_ID|CreateNPC|TRIGGER` returns
nothing (one incidental `WorldLayer_Velder_Gate.tga` texture name in the
scene file, not a gate object). Checking `Field_Ranox_Stage0.lua` and its
LineMap the same way also comes up empty, ruling out "the portal is in the
open field instead." There is no in-world portal or quest-NPC entry point
anywhere in Ranox's 125-file inventory. The real mechanism is the same
World Map / Local Map UI overlay every other chapter already uses, assembled
from three independent pieces, all now landed by this phase:

1. **`MapData:AddLocalMapInfo(LOCAL_MAP_ID.LMI_RANOX, ...)`** (in
   `march2014/data036/MapData.lua`) is the authoritative list of which
   `DUNGEON_ID`s are unlockable/enterable for the Ranox region — this is what
   the server-side/engine dungeon-entry logic actually checks, independent of
   any UI.
2. **`DungeonIDList`** in `march2014/update/data217/DLG_Map_Local_Ranox.lua`
   is the separate, parallel list of which dungeon icons are drawn on the
   region's local-map screen — a visual/UI list, not a gate. The two lists
   were ported to agree (both carry BurndedForest/AshesVillage/Geyser/
   Gateway/FireTemple/FinalHall/Secret Hell, Normal-tier-triggering only).
3. **`EnterCondition`** in that same file (`RequireUnitLevel = 68`,
   `RequireClearDungeonID = { DUNGEON_ID.DI_SANDER_BEHIMOSS_HEART_NORMAL }`)
   is the actual gate on the *region* being reachable from the world map at
   all — a player has to be level 68 and have cleared Sander's last dungeon
   before Ranox's entry even appears as clickable, which is also why Sander's
   own `AddMapData` `AFTER` field was repointed at `VMI_RANOX` in this phase
   (previously `VMI_NONE`, dead-ending the chapter chain at Sander).

In short: villages in this client are non-interactive backdrops; "walking
into a dungeon" is a UI fiction. The actual flow is world map → local map
(gated by `EnterCondition`) → dungeon icon (drawn per `DungeonIDList`,
enterable per `AddLocalMapInfo`) → room list. No repo-side (C++) change is
implicated by this — the mechanism is 100% pre-existing engine plumbing that
every other chapter already exercises; Ranox only needed its own data-table
rows.

**Enum gap not caught by Phase 1's audit**: `LOCAL_MAP_ID.LMI_RANOX` (needed
by `MapData:AddLocalMapInfo`, item 1 above) was outside Phase 1's stated
scope (`NPC_UNIT_ID`/`WORLD_ID`/`VILLAGE_MAP_ID`/`DUNGEON_ID` only) and had no
entry in `march2014/data036/Enum.lua`. Added inline this phase at June2017's
own verbatim value, `10008` — confirmed free (March2014's own `LOCAL_MAP_ID`
table only runs `LMI_RUBEN`..`LMI_SANDER`, `10000`..`10007`).

**File found to be load-bearing but not named in the phase brief**:
`data034_kom/DLG_Map_Local_Ranox_Back_NEW.lua`. `DLG_Map_Local_Ranox_Icon_NEW.lua`
is never loaded on its own — `..._Back_NEW.lua` is what `IncludeLua("DLG_Map_Local_Ranox_Icon_NEW.lua")`s
it in, the same way March2014's own `DLG_Map_Local_Sander_Back_NEW.lua` pulls
in `..._Sander_Icon_NEW.lua`. Ported to `march2014/update/data034/` (keeping
data034, matching where its Sander equivalent already lives in this client —
this file has no data217 counterpart in June2017 either).

**Two documented, deliberate omissions, both `SERV_IRUHADEV_RANOX_PORT`-tagged
comments in the ported files, neither part of the 8-gap table**:

- `DLG_Map_Local_Ranox_Icon_NEW.lua`'s `g_pStatic_Event_01`/`02` blocks
  (Halloween "Mission Impossible" title overlays) call
  `AddDummyInt(DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE)` — this **is** the
  already-known `DLT_EVENT_DUNGEON_TITLE` gap from the Findings table,
  encountered here for the first time chronologically (Phase 9 was where the
  plan expected it to resurface). Omitted per the plan's own instruction to
  revisit in Phase 9; the two working "Mission Impossible" dungeon buttons
  that don't touch the missing enum member (`g_pButton_Event_Dummy01`/`02`,
  using only `DLT_DUNGEON` and pre-existing March2014 dungeon IDs) were kept.
- `DLG_Map_Local_Ranox_Back_NEW.lua`'s `g_pStatic_HeroMode_Filter` block is a
  new-since-March-2014 Hero-mode UI overlay with no engine support to check
  against (not one of the 8 gaps, since it's not a missing enum member —
  it's a whole absent feature). Omitted per this document's own "Hero
  difficulty tier: out of scope by default" call; March2014's own Sander
  equivalent has no such block either, confirming it postdates this client.

**Verification performed** (beyond `compile-batch -v mar-2014` succeeding on
all 8 output files, individually and together): every `TABLE.MEMBER`-shaped
token and every `:Method(`/bare-global-call token used across all 6 new/
ported files was checked against March2014's own decompiled corpus (its
`Villeage_Sander`/`Besma_Village`/`ruben_village`/`Velder_village`/
`Elder_village`/`Altera_Village` scene+LineMap files, and its
`DLG_UI_FIELD_MAP_Besma_Village_NEW`/`DLG_Map_Local_Sander*` UI files) —
zero misses once the corpus included a real popup-dialog file (`SetSize`,
`SetModal`, `SetFront`, `SetEnableMoveByDrag`, `SetDraggingRange*` etc. don't
appear in the simpler `IncludeLua`-target files, only in a standalone
`DLG_UI_FIELD_MAP_*` dialog). `IconAni` and `SetNameStrID` (both bare/method
calls, meaning a missing definition would abort the whole script rather than
silently read `nil`) were separately confirmed present and used identically
in March2014's own `DLG_Map_Local_Sander_Icon_NEW.lua`. The three base files
patched here (`data036/MapData.lua`, `data040/WorldData.lua`,
`data036/Enum.lua`'s pre-Phase-1 original) were also re-decompiled fresh
from their `.lua` bytecode into a clean scratch path and diffed
byte-identical against the `.src.lua` siblings used as this phase's base,
ruling out contamination from the abandoned prior attempt.

**One additional registration added while in `MapData.lua`**:
`CheckCOBOExpressTicketMapID` (the paid express-ticket cost lookup) gained an
`elseif slot0 == VILLAGE_MAP_ID.VMI_RANOX then return 15` branch, matching
June2017's own value (which continues this build's existing 1-per-chapter
numbering, `..., VMI_SANDER → 13/14, VMI_RANOX → 15`) verbatim. Ranox's
battlefield/spa `VILLAGE_MAP_ID` entries are intentionally not added to this
function yet since those maps aren't ported until later phases.

---

### Phase 3 — AshesVillage dungeon line (first fully playable dungeon)

**Goal**: the entry-point dungeon line, zero engine gaps — the MVP spine's
second half.

**Source → Target** (all under `march2014/update/`):
- `data035_kom/Dungeon_Ranox_AshesVillage_{Normal,Hard,Expert}.lua` → `update/data035/`
- `data040_kom/Ranox_AshesVillage_Stage{0,1,2,3,Boss}.lua` + matching
  `_LineMap.lua` (10 files total) → `update/data040/`
- `data037_kom/RANOX_ASHESVILLAGE_GATE.lua`,
  `RANOX_ASHESVILLAG_SWITCH_GATE.lua` (note the source typo — missing
  trailing "E" — keep the filename as-is, it's what the enum/dungeon file
  references) → `update/data037/`
- Monster unit-definition files for this line's 11 new `NPC_UNIT_ID`s
  (`NUI_RANOX_ASHESVILLAGE_GATE`, `NUI_RANOX_ASHESVILLAG_SWITCH_GATE`,
  `NUI_SMITH_ALUMINUM`, `NUI_SMITH_CART`, `NUI_SMITH_LITTLEWORKER`,
  `NUI_SMITH_MACHINE`, `NUI_SMITH_REGIEON_POINT`, `NUI_SMITH_RUNNER`,
  `NUI_SMITH_SCIENCE`, `NUI_SMITH_STAIN`, `NUI_SMITH_WORKER`) — find each
  one's own definition file in the June2017 tree (named after the monster,
  not "Ranox" — likely in `data037_kom` alongside the gate objects, or
  wherever this client's unit definitions conventionally live; the two gate
  files above are the known example of the shape to expect) and port it the
  same way.
- Register in `march2014/data035/DungeonData.lua` (patch).

**Done when**: all 3 difficulties compile (`compile-batch -v mar-2014`
succeeds on every file), and — once repo integration exists — the line is
reachable and playable in-game at Normal.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 3" and Appendix B. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for already-decompiled .src.lua siblings first). Decompiler:
elstools/elstools.exe decompile-batch -i <file> / compile-batch -v mar-2014
-i <file> (in place -- copy to scratch first). Phase 1's enum additions
(./march2014/data036/Enum.lua etc) and Phase 2's village should already
exist -- check first; if missing, note it and proceed with this phase's own
files regardless (they don't block on the village being finished, only on
the enum IDs existing).

Your task: port the AshesVillage dungeon line, all new files under
./march2014/update/<source-archive>/ (patches to existing March2014 files,
if any needed here, go under ./march2014/<archive>/ instead -- see the plan
doc's Output Convention section). Specifically:
data035_kom/Dungeon_Ranox_AshesVillage_{Normal,Hard,Expert}.lua ->
update/data035/; data040_kom/Ranox_AshesVillage_Stage{0,1,2,3,Boss}.lua and
their _LineMap.lua companions (10 files) -> update/data040/;
data037_kom/RANOX_ASHESVILLAGE_GATE.lua and
RANOX_ASHESVILLAG_SWITCH_GATE.lua (keep the source typo in the second
filename) -> update/data037/. Then find and port the unit-definition file
for each of these 11 NPC_UNIT_IDs this line references: NUI_SMITH_ALUMINUM,
NUI_SMITH_CART, NUI_SMITH_LITTLEWORKER, NUI_SMITH_MACHINE,
NUI_SMITH_REGIEON_POINT, NUI_SMITH_RUNNER, NUI_SMITH_SCIENCE,
NUI_SMITH_STAIN, NUI_SMITH_WORKER (the two gate NPC_UNIT_IDs are the gate
files already listed above) -- each definition file is named after the
monster itself, not "Ranox", find it by grepping the June2017 tree. Finally,
add this line's entries to March2014's data035/DungeonData.lua, writing the
full patched file to ./march2014/data035/DungeonData.lua.
```

---

### Phase 4 — BurnedForest line

Same shape as Phase 3, no engine gaps. New `NPC_UNIT_ID`s (10): `NUI_MAG_CHARGER`,
`NUI_MAG_NOM`, `NUI_MAG_STONE`, `NUI_RAGE_BABYFIG`, `NUI_RAGE_DRYAD`,
`NUI_RAGE_DRYADKENTA`, `NUI_RAGE_MOTHERFIG`, `NUI_RANOX_BURNDEDFOREST_GATE`,
`NUI_TRAP_FIRE_BIG`, `NUI_TRAP_FIRE_SMALL`. Note the source's own typo
`BURNDEDFOREST` (missing an "E") in `DI_RANOX_BURNDEDFOREST_*` and the gate
object filename `RANOX_BURNDEDFOREST_GATE.lua` — keep verbatim, it's what
the dungeon file references. Stage files: `Ranox_BurnedForest_Stage{0,1,2,Boss}.lua`
(no Stage3 for this line) + LineMaps (8 files, `data040_kom`), dungeon defs
`Dungeon_Ranox_BurnedForest_{Normal,Hard,Expert}.lua` (`data035_kom`), gate
object `RANOX_BURNDEDFOREST_GATE.lua` (`data037_kom`).

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 4" and Appendix B. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enum additions exist under
./march2014/data036/ -- check first.

Your task: port the BurnedForest dungeon line the same way Phase 3 ported
AshesVillage (see RANOX_PORT_PLAN.md Phase 3 for the exact pattern -- same
file-category shape, this line just has 4 stages instead of 5, no Stage3).
Source files: data035_kom/Dungeon_Ranox_BurnedForest_{Normal,Hard,Expert}.lua,
data040_kom/Ranox_BurnedForest_Stage{0,1,2,Boss}.lua + their LineMap
companions, data037_kom/RANOX_BURNDEDFOREST_GATE.lua (keep this exact typo'd
filename, it's what the source references). New NPC_UNIT_IDs needing
unit-definition files ported: NUI_MAG_CHARGER, NUI_MAG_NOM, NUI_MAG_STONE,
NUI_RAGE_BABYFIG, NUI_RAGE_DRYAD, NUI_RAGE_DRYADKENTA, NUI_RAGE_MOTHERFIG,
NUI_TRAP_FIRE_BIG, NUI_TRAP_FIRE_SMALL (NUI_RANOX_BURNDEDFOREST_GATE is the
gate file above). All new files -> ./march2014/update/<source-archive>/;
add this line to March2014's data035/DungeonData.lua, writing the full
patched file to ./march2014/data035/DungeonData.lua.
```

---

### Phase 4 findings (executed 2026-09-18)

**All 20 files landed and compile** (`compile-batch -v mar-2014`, individually
and together with the two patched registry files): 3 dungeon-difficulty
containers, 8 stage+LineMap files, the gate object, and 9 monster
unit-definition files (`MAG_CHARGER`, `MAG_NOM`, `MAG_STONE`, `RAGE_BABYFIG`,
`RAGE_DRYAD`, `RAGE_DRYADKENTA`, `RAGE_MOTHERFIG`, `TRAP_FIRE_BIG`,
`TRAP_FIRE_SMALL` — all found in `data037_kom`, named after the monster, same
shape as Phase 3's `SMITH_*` files). All 20 also round-trip
(compile → decompile) content-identical to their June2017 source, modulo Lua
table key reordering — the same benign artifact this plan's tooling section
already documents. `DI_RANOX_BURNDEDFOREST_{NORMAL,HARD,EXPERT}` and all 10
`NPC_UNIT_ID`s were already in `march2014/data036/{Enum,DungeonEnum}.lua`
from Phase 1 — zero new enum work needed there. Reachability was already
wired by Phase 2: both `MapData:AddLocalMapInfo` and
`DLG_Map_Local_Ranox.lua`'s `DungeonIDList` already carry
`DI_RANOX_BURNDEDFOREST_*`, so this line needed no additional registry work
beyond `DungeonData.lua`. `DungeonData.lua`'s June2017 entries carry no
`EXTRA_REQUIRE_DUNGEON_ID` for BurnedForest (unlike AshesVillage, which
requires `DI_RANOX_BURNDEDFOREST_NORMAL`) — ported verbatim; this is
consistent with Phase 2's finding that region entry is gated once, by
`EnterCondition` on the region itself, not by a per-dungeon chain.

**One enum gap this phase's own scope exposed, outside Phase 1's audit**:
`BUFF_FACTOR_ID.BFI_BUFF_MAG_STONE_ATTACK_B`, referenced twice in
`MAG_STONE.lua`. The Findings section's 8-gap table only audited the 125-file
Appendix A inventory, which by construction excludes every monster
unit-definition file (they're named after the monster, not "Ranox" or a
dungeon-line name) — so this was never checked. Added to
`march2014/data036/Enum.lua`'s existing `BUFF_FACTOR_ID` table at June2017's
own verbatim value, `512` — confirmed free within that table (a couple of
*other* tables already use `512` for unrelated IDs, which don't collide).
**Phases 5–9 should expect more of this class** as they port `NUI_FIRE_DEVIL_*`,
`NUI_SECRET_*`, etc. unit-definition files — audit `BUFF_FACTOR_ID` (and any
other non-`NPC_UNIT_ID`/`WORLD_ID`/`VILLAGE_MAP_ID`/`DUNGEON_ID` table) the
same way, not just the four tables Phase 1 scoped.

**Two method-level gaps, found only by checking every `:Method(` call in the
9 monster files against this client's actual C++ bindings (`lua_tinker::class_def`
in `X2Lib`/`KTDXLIB`), not just against enum tables.** This check isn't in
Phase 1's audit or the Findings' 8-gap table at all — both are enum-only —
and it matters more than an enum gap: a call to an unbound method aborts the
whole script, per the Findings' own C++-code-reading caveat for bare/method
calls, whereas a missing *enum member* just makes that one expression `nil`.
Both gaps are confined to `RAGE_DRYADKENTA.lua` (the BurnedForest boss):

- `slot2:GetStoredGUUser()` (line ~754, the target-loss fallback in
  `RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START`) — not bound anywhere in
  `X2Lib`/`KTDXLIB`. Omitted; `slot4` now simply stays `nil` when there's no
  current target, so the reorientation block that follows is skipped instead
  of falling back to a stored target — the same behavior every other state in
  this file already has when it only checks `GetTargetUser()`.
- `slot2:InitStateCoolTime(...)` (two call sites in
  `RAGE_DRYADKENTA_COMMON_FRAMEMOVE`, both immediately before a
  `StateChange_LUA` into the same dash-attack state) — also not bound.
  Omitted; the `StateChange_LUA`/`SetFlag_LUA` calls that follow are kept, so
  the boss's phase-transition dash attack still triggers at the same HP/cooldown
  thresholds, just without whatever cooldown-priming the missing call did for
  the target state.

Both are `SERV_IRUHADEV_RANOX_PORT`-tagged inline comments in the ported file,
same convention as Phase 2's documented omissions. Re-verified
`compile-batch -v mar-2014` succeeds on the file after both edits.

**One data-table gap, noted but out of scope for this phase**: the buff
factor system doesn't key off `X2BuffFactor.h`'s C++ enum at all —
`CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID` looks the ID up at
runtime via `CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr()`, which
is itself populated by `CX2BuffTemplet::ParsingScript(KLuaManager&)` — a Lua
data table, not a fixed enum. A missing ID there degrades safely (the lookup
returns `NULL`, the function returns `false`, no crash, no script abort) but
means `BFI_BUFF_MAG_STONE_ATTACK_B`'s actual buff effect (duration, magnitude)
won't apply even with the enum value now present, unless that Lua table also
gets an entry — the same shape as this plan's Findings section already calls
out for monster stat/drop tables, and out of scope here for the same reason.

---

### Phase 5 — FinalHall line

Shares its `NUI_FIRE_DEVIL_*` monster family with FireTemple (Phase 6) —
port the shared monster unit-definitions once here, note in your work that
Phase 6 should **not** re-port them. New `NPC_UNIT_ID`s specific to this
line: `NUI_EVENT_FINALHALL_BOSS{,2,3}`, `NUI_FIRETEMPLE_GATE`,
`NUI_FIRETEMPLE_REGIEON_POINT`, `NUI_FIRE_DEVIL_BERSERKER{,_BOSS,_BOSS_FINALHALL,_INCHANT}`,
`NUI_FIRE_DEVIL_CHAIN_BALL`, `NUI_FIRE_DEVIL_CHAIN_STONE`,
`NUI_FIRE_DEVIL_FIGHTER{,_INCHANT}`, `NUI_FIRE_DEVIL_IGNIA{,_BOSS,_BOSS_FINALHALL}`
(remember the `_IGNIA_BOSS` vs `_IGNIA_BOSS_FINALHALL` anomaly from the
Findings section), `NUI_FIRE_DEVIL_IN{,_FINALHALL}`,
`NUI_FIRE_DEVIL_JIN{,_FINALHALL}`, `NUI_FIRE_DEVIL_SKA`,
`NUI_FIRE_DEVIL_SNIKER{,_INCHANT}`, `NUI_MOUNT_GATE`. Stage files:
`Ranox_FinalHall_Stage{0,1,2,3,Boss}.lua` + LineMaps (10 files, `data040_kom`),
dungeon defs `Dungeon_Ranox_FinalHall_{Normal,Hard,Expert}.lua`
(`data035_kom`). No gate/switch object file was found for this line in the
inventory — double check before assuming one is needed.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 5", the Findings section's note on
NUI_FIRE_DEVIL_IGNIA_BOSS, and Appendix B. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enum additions exist under
./march2014/data036/ -- check first.

Your task: port the FinalHall dungeon line (data035_kom/Dungeon_Ranox_FinalHall_
{Normal,Hard,Expert}.lua, data040_kom/Ranox_FinalHall_Stage{0,1,2,3,Boss}.lua
+ LineMap companions) following the same pattern as Phase 3's AshesVillage.
This line shares its NUI_FIRE_DEVIL_* monster family with the FireTemple
line (a later phase) -- port the unit-definition files for all of these now:
NUI_EVENT_FINALHALL_BOSS, NUI_EVENT_FINALHALL_BOSS2, NUI_EVENT_FINALHALL_BOSS3,
NUI_FIRETEMPLE_GATE, NUI_FIRETEMPLE_REGIEON_POINT, NUI_FIRE_DEVIL_BERSERKER,
NUI_FIRE_DEVIL_BERSERKER_BOSS, NUI_FIRE_DEVIL_BERSERKER_BOSS_FINALHALL,
NUI_FIRE_DEVIL_BERSERKER_INCHANT, NUI_FIRE_DEVIL_CHAIN_BALL,
NUI_FIRE_DEVIL_CHAIN_STONE, NUI_FIRE_DEVIL_FIGHTER, NUI_FIRE_DEVIL_FIGHTER_INCHANT,
NUI_FIRE_DEVIL_IGNIA, NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL (resolve any
reference to the undefined NUI_FIRE_DEVIL_IGNIA_BOSS as this one instead --
see the plan doc's Findings section), NUI_FIRE_DEVIL_IN,
NUI_FIRE_DEVIL_IN_FINALHALL, NUI_FIRE_DEVIL_JIN, NUI_FIRE_DEVIL_JIN_FINALHALL,
NUI_FIRE_DEVIL_SKA, NUI_FIRE_DEVIL_SNIKER, NUI_FIRE_DEVIL_SNIKER_INCHANT,
NUI_MOUNT_GATE. Note this for whoever does Phase 6 (FireTemple) so they
don't re-port the shared ones. All new files -> ./march2014/update/
<source-archive>/; register in ./march2014/data035/DungeonData.lua.
```

---

### Phase 6 — FireTemple line (has 2 engine gaps)

2 engine gaps confined to `Ranox_FireTemple_Stage1_LineMap.lua`:
`TRIGGER_ETC_ACTION_TYPE.TAT_ETC_NPC_AI_ON_OFF` and
`TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE`. Port everything else
verbatim; for the two gapped trigger entries, either resolve them (repo
phase — confirm whether the underlying C++ trigger-action handling already
exists dormant) or make a **documented, deliberate** omission of just those
trigger entries so the rest of the stage still loads. Reuses FireTemple/
FinalHall's shared `NUI_FIRE_DEVIL_*` family from Phase 5 — only new names
here: `NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`, `NUI_FIRE_DEVIL_CHAIN_IPLITAN`,
`NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`, `NUI_MOUNT_FIREBIRD{,_FIRETEMPLE}`.
Stage files: `Ranox_FireTemple_Stage{0,1,2,Boss}.lua` + LineMaps (8 files),
dungeon defs `Dungeon_Ranox_FireTemple_{Normal,Hard,Expert}.lua`.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 6", the engine-gap table in
Findings, and Appendix B. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enums and Phase 5's shared
NUI_FIRE_DEVIL_* unit-definitions already exist -- check ./march2014/ first;
if Phase 5 hasn't landed, port the FireTemple-specific new names only and
flag the shared-family dependency as still open.

Your task: port the FireTemple dungeon line
(data035_kom/Dungeon_Ranox_FireTemple_{Normal,Hard,Expert}.lua,
data040_kom/Ranox_FireTemple_Stage{0,1,2,Boss}.lua + LineMap companions).
IMPORTANT: Ranox_FireTemple_Stage1_LineMap.lua references two enum members
that do NOT exist in March2014's Enum.lua -- TRIGGER_ETC_ACTION_TYPE.
TAT_ETC_NPC_AI_ON_OFF and TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE
(confirmed absent this session via exhaustive diff, not a guess). Do not
invent values for these. Port everything else in that file verbatim, and
either research whether the underlying C++ engine feature already exists in
this repo under a different Lua binding (repo-side question, likely a
separate phase), or omit just the specific trigger-line entries using those
members and document exactly what was omitted and why, so the rest of the
stage still compiles and loads. New NPC_UNIT_IDs beyond Phase 5's shared
Fire Devil family: NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
NUI_FIRE_DEVIL_CHAIN_IPLITAN, NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
NUI_MOUNT_FIREBIRD, NUI_MOUNT_FIREBIRD_FIRETEMPLE. All new files ->
./march2014/update/<source-archive>/; register in
./march2014/data035/DungeonData.lua.
```

---

### Phase 7 — Geyser line (has 3 engine gaps)

3 engine gaps, all in `Ranox_Geyser_stage0_linemap.lua` (note the source's
own lowercase filename convention for this line — `stage0`/`linemap` not
`Stage0`/`LineMap` — keep as-is): `TAT_ETC_PLAY_EFFECTSET`,
`TAT_ETC_TALK_BOX`, `TAT_MESH_MOVE`. Same omit-or-resolve call as Phase 6.
New `NPC_UNIT_ID`s: `NUI_EVENT_GAYSER_BOSS` (note source's own typo,
"GAYSER" not "GEYSER" — keep verbatim), `NUI_GEYSER_ALUMINUM`,
`NUI_GEYSER_GATE`, `NUI_GEYSER_LITTLEWORKER`, `NUI_GEYSER_MULLOCK{,_CALL}`,
`NUI_GEYSER_MULMAI`, `NUI_GEYSER_MULSAYRENA{,_CALL}`, `NUI_GEYSER_RUNNER`,
`NUI_GEYSER_SCIENCE`, `NUI_GEYSER_WORKER`. Stage files: `Ranox_Geyser_stage0.lua`/
`_linemap.lua` (lowercase), `Ranox_Geyser_Stage{1,2,Boss}.lua` + LineMaps
(8 files total), dungeon defs `Dungeon_Ranox_Geyser_{Normal,Hard,Expert}.lua`.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 7", the engine-gap table in
Findings, and Appendix B. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first; note this line's filenames are inconsistently
lowercase -- Ranox_Geyser_stage0.lua / _linemap.lua, not Stage0/LineMap --
keep exactly as found). Decompiler: elstools/elstools.exe decompile-batch -i
<file> / compile-batch -v mar-2014 -i <file> (in place -- copy to scratch
first). Assumes Phase 1's enums already exist under ./march2014/ -- check
first.

Your task: port the Geyser dungeon line
(data035_kom/Dungeon_Ranox_Geyser_{Normal,Hard,Expert}.lua,
data040_kom/Ranox_Geyser_stage0.lua + _linemap.lua, Stage1/2/Boss.lua +
LineMaps). IMPORTANT: Ranox_Geyser_stage0_linemap.lua references three enum
members that do NOT exist in March2014's Enum.lua (confirmed via exhaustive
diff) -- TRIGGER_ETC_ACTION_TYPE.TAT_ETC_PLAY_EFFECTSET,
TRIGGER_ETC_ACTION_TYPE.TAT_ETC_TALK_BOX, and
TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE. Don't invent values -- either research
whether the C++ feature exists dormant (likely a separate repo-side phase),
or omit just those specific trigger entries and document exactly what and
why, so the rest of the file still loads. New NPC_UNIT_IDs: NUI_EVENT_GAYSER_BOSS
(keep this exact spelling, it's the source's own typo), NUI_GEYSER_ALUMINUM,
NUI_GEYSER_GATE, NUI_GEYSER_LITTLEWORKER, NUI_GEYSER_MULLOCK,
NUI_GEYSER_MULLOCK_CALL, NUI_GEYSER_MULMAI, NUI_GEYSER_MULSAYRENA,
NUI_GEYSER_MULSAYRENA_CALL, NUI_GEYSER_RUNNER, NUI_GEYSER_SCIENCE,
NUI_GEYSER_WORKER. All new files -> ./march2014/update/<source-archive>/;
register in ./march2014/data035/DungeonData.lua.
```

---

### Phase 8 — GateWay line (has 1 engine gap)

1 engine gap: `TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE` in
`Ranox_GateWay_Stage1_LineMap.lua`. Same omit-or-resolve call. New
`NPC_UNIT_ID`s: `NUI_EVENT_GATEWAY_BOSS`, `NUI_MOUNT_MAG_CHARGER`,
`NUI_MOUNT_MAG_NOM`, `NUI_MOUNT_MAG_STONE`, `NUI_MOUNT_POINT_STAGE_BOSS`,
`NUI_SAMETEAM_IGNIA` (plus reused `NUI_FIRE_DEVIL_BERSERKER{,_BOSS}`,
`NUI_FIRE_DEVIL_FIGHTER`, `NUI_FIRE_DEVIL_IGNIA`, `NUI_MOUNT_FIREBIRD`,
`NUI_MOUNT_GATE` from Phases 5/6 — don't re-port). Stage files:
`Ranox_GateWay_Stage{0,1,2,Boss}.lua` + LineMaps (8 files), dungeon defs
`Dungeon_Ranox_Gateway_{Normal,Hard,Expert}.lua` (note: dungeon-def
filenames use "Gateway", stage/scene filenames use "GateWay" — capitalization
differs between the two file categories, keep each as found).

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 8", the engine-gap table in
Findings, and Appendix B. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first; note "Gateway" in dungeon-def filenames vs
"GateWay" in stage/scene filenames -- keep each exactly as found). Decompiler:
elstools/elstools.exe decompile-batch -i <file> / compile-batch -v mar-2014
-i <file> (in place -- copy to scratch first). Assumes Phases 1, 5, 6 already
landed their enum/monster-definition work -- check ./march2014/ first.

Your task: port the GateWay dungeon line
(data035_kom/Dungeon_Ranox_Gateway_{Normal,Hard,Expert}.lua,
data040_kom/Ranox_GateWay_Stage{0,1,2,Boss}.lua + LineMap companions).
IMPORTANT: Ranox_GateWay_Stage1_LineMap.lua references
TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE, which does NOT exist in March2014's
Enum.lua (confirmed via exhaustive diff). Don't invent a value -- either
research whether the C++ feature exists dormant, or omit that specific
trigger entry and document what/why. New NPC_UNIT_IDs: NUI_EVENT_GATEWAY_BOSS,
NUI_MOUNT_MAG_CHARGER, NUI_MOUNT_MAG_NOM, NUI_MOUNT_MAG_STONE,
NUI_MOUNT_POINT_STAGE_BOSS, NUI_SAMETEAM_IGNIA (this line also reuses
NUI_FIRE_DEVIL_BERSERKER, NUI_FIRE_DEVIL_BERSERKER_BOSS,
NUI_FIRE_DEVIL_FIGHTER, NUI_FIRE_DEVIL_IGNIA, NUI_MOUNT_FIREBIRD, and
NUI_MOUNT_GATE from earlier phases -- do not re-port those definitions if
they already exist under ./march2014/). All new files ->
./march2014/update/<source-archive>/; register in
./march2014/data035/DungeonData.lua.
```

---

### Phase 9 — Secret Hell (raid) dungeon — do last among the dungeon lines

Largest and structurally distinct file (`Dungeon_Ranox_Secret_Hell.lua`,
3064 lines vs ~1100 for a normal line) — no difficulty split, additional
"extra room" stages folded into the same `STAGE_LIST`, and props (chests,
chains, gates) modeled as `NPC_UNIT_ID` entries alongside real monsters. 2
of the 8 engine gaps land here (`CTT_RIM_LIGHT_BLUE` in
`Ranox_Secret_Boss_Stage.lua`, `TAT_LINE_CHANGE_TYPE` in
`Ranox_Secret_Stage2_LineMap.lua`), plus a UI-only gap
(`DLT_EVENT_DUNGEON_TITLE` in `DLG_Map_Local_Ranox_Icon_NEW.lua`, already
ported in Phase 2 — revisit it here). New `NPC_UNIT_ID`s (28, all
`NUI_SECRET_*`): `NUI_SECRET_BERSERKER`, `NUI_SECRET_BOSS_LAVA`,
`NUI_SECRET_CRUEL_{BERSERKER,FIGHTER,MAG_CHARGER,MAG_NOM,SNIKER}`,
`NUI_SECRET_EXTRA_MAGI_BOSS`, `NUI_SECRET_FIGHTER`, `NUI_SECRET_FIREBIRD`,
`NUI_SECRET_IN`, `NUI_SECRET_JIN`, `NUI_SECRET_LAVA_DUMMY`,
`NUI_SECRET_MAGI_{BOSS,SCENE_A,SCENE_B,SCENE_C,SUMMON}`,
`NUI_SECRET_MAG_{CHARGER,NOM}`, `NUI_SECRET_SCENE_{1_0,1_1,DUMMY}`,
`NUI_SECRET_SNIKER`, `NUI_SECRET_STAGE0_CHAIN_OBJECT`,
`NUI_SECRET_STAGE0_GATE`, `NUI_SECRET_STAGE1_CHAIN{1,2}`. Stage files:
`Ranox_Secret_Stage{0,1,2,3}.lua` + LineMaps (8), `Ranox_Secret_Boss_Stage.lua`
+ LineMap, `Ranox_Secret_Extra_Boss_Stage.lua` (no paired LineMap found —
verify whether it reuses another stage's map), `Ranox_Secret_Extra_Stage{1,2,3}.lua`
(no paired LineMaps found either — same caveat). Also port
`DLG_UI_RANOX_SECRET_FEVER_GAGE.lua` (`data034_kom`) here, since it's this
dungeon's dedicated UI.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 9", the engine-gap table in
Findings, and Appendix B. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enums already landed -- check
./march2014/ first.

Your task: port the Secret Hell raid dungeon -- this is the largest and most
structurally distinct file in the whole port (data035_kom/
Dungeon_Ranox_Secret_Hell.lua, ~3064 lines, no difficulty split, chests/
chains/gates modeled as NPC_UNIT_ID entries alongside real monsters). Also
port data040_kom/Ranox_Secret_Stage{0,1,2,3}.lua + LineMap companions,
Ranox_Secret_Boss_Stage.lua + LineMap, Ranox_Secret_Extra_Boss_Stage.lua,
Ranox_Secret_Extra_Stage{1,2,3}.lua (the last 4 files may have no paired
LineMap in the source tree -- verify and note whether they reuse another
stage's LineMap or genuinely don't need one). IMPORTANT: two enum members
used here do NOT exist in March2014's Enum.lua (confirmed via exhaustive
diff): CARTOON_TEX_TYPE.CTT_RIM_LIGHT_BLUE (in Ranox_Secret_Boss_Stage.lua)
and TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE (in
Ranox_Secret_Stage2_LineMap.lua) -- don't invent values, research or omit
and document as in earlier phases. Also revisit
./march2014/update/data217/DLG_Map_Local_Ranox_Icon_NEW.lua (ported in
Phase 2) -- it references DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE, also
missing from March2014's Enum.lua, for this dungeon's special UI treatment.
Port data034_kom/DLG_UI_RANOX_SECRET_FEVER_GAGE.lua to
./march2014/update/data034/ as this dungeon's dedicated fever-gauge UI. New
NPC_UNIT_IDs (28, listed in RANOX_PORT_PLAN.md Phase 9) need unit-definition
files found and ported the same way as earlier phases. Register in
./march2014/data035/DungeonData.lua.
```

---

### Phase 10 — Open fields

**Source → Target**:
- `data036_kom/Ranox_Field_{00,01,02,03}.lua` (spawners) → `update/data036/`
- `data040_kom/Field_Ranox_Stage{0,1,2,3}.lua` + LineMaps (8 files) → `update/data040/`
- `data040_kom/Field_Ranox_rest.lua` + `_LineMap.lua` → `update/data040/`
  (skip `Field_Ranox_rest_Festival.lua` — deferred, Phase 12)
- New `NPC_UNIT_ID`s: the `NUI_LOW_*_FD` family (open-field monster tier,
  ~30 names — see Appendix B's "FIELD" bucket) plus `NUI_MUL_MAI_EVENT`.
  Some overlap with village/story NPCs (`NUI_EDEL`, `NUI_FACEOFF`,
  `NUI_IGNIA`, `NUI_STEEL`, `NUI_SYOTA`) — these may already be ported from
  Phase 2's village-entry investigation; check before re-porting.
- Register wherever WorldData.lua's field entries live (check its structure
  once decompiled — no separate FieldData registry exists in either client).

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 10" and Appendix B's "FIELD"
bucket. Do NOT consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md,
or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enums and Phase 2's village-entry
NPCs already landed -- check ./march2014/ first.

Your task: port Ranox's 4 open fields. Source files:
data036_kom/Ranox_Field_{00,01,02,03}.lua (the monster spawner scripts --
these use g_pBattleFieldManager:AddSpawnMonsterGroup calls, structurally
different from dungeon STAGE_LIST files, see the plan doc's Findings for the
schema notes) -> ./march2014/update/data036/; data040_kom/
Field_Ranox_Stage{0,1,2,3}.lua + LineMap companions (the walkable scenes the
spawners populate) -> ./march2014/update/data040/; data040_kom/
Field_Ranox_rest.lua + _LineMap.lua -> ./march2014/update/data040/ (skip
Field_Ranox_rest_Festival.lua, deferred). Port unit-definition files for the
new NUI_LOW_*_FD monster family (~30 names, full list in
RANOX_PORT_PLAN.md's Appendix B "FIELD" section) plus NUI_MUL_MAI_EVENT --
check whether NUI_EDEL/NUI_FACEOFF/NUI_IGNIA/NUI_STEEL/NUI_SYOTA were
already ported during Phase 2's village-entry investigation before
re-porting them. Register the fields wherever data040/WorldData.lua's field
entries live (there's no separate FieldData registry in either client --
inspect WorldData.lua's structure once decompiled), writing the full patched
file to ./march2014/data040/WorldData.lua.
```

---

### Phase 11 — PVP GateWay map

**Source → Target**:
- `data040_kom/PVP_Ranox_GateWay.lua` + `_LineMap.lua` → `update/data040/`
- Registry: `march2014/data036/BattleFieldData.lua`,
  `march2014/data036/PVP_Map_List.lua` (patches).
- Engine gap: `LINE_TYPE.LT_FIRE_GROUND` in the LineMap file — same
  omit-or-resolve call as earlier phases.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 11" and the engine-gap table in
Findings. Do NOT consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md,
or the mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first). Assumes Phase 1's enums already landed.

Your task: port the Ranox GateWay PVP map. Source:
data040_kom/PVP_Ranox_GateWay.lua + PVP_Ranox_GateWay_LineMap.lua ->
./march2014/update/data040/. IMPORTANT: the LineMap file references
LINE_TYPE.LT_FIRE_GROUND, which does NOT exist in March2014's Enum.lua
(confirmed via exhaustive diff) -- don't invent a value, research whether
the underlying hazard-ground engine feature exists dormant, or omit just
that line-type usage and document what/why. Register the map in March2014's
data036/BattleFieldData.lua and data036/PVP_Map_List.lua, writing the full
patched files to ./march2014/data036/BattleFieldData.lua and
./march2014/data036/PVP_Map_List.lua.
```

---

### Phase 12 — Remaining UI + deferred/optional content

**Remaining UI** (not yet ported by an earlier phase):
- `data034_kom/DLG_UI_FIELD_MAP_Ranox_{01,02,03,04}_NEW.lua`,
  `DLG_UI_FIELD_MAP_Ranox_Rest_NEW.lua`, `DLG_Map_Local_Ranox_Back_NEW.lua`
  → `march2014/update/data034/`.

**Deferred cosmetic content** (only if still wanted after the above works):
- Village seasonal reskins: `Ranox_Village_{Festival,Halloween,XMAS,XMAS_2,luminarie}.lua`.
- `Spa_Ranox.lua`, `_LineMap.lua`, `_Snow.lua`.
- `Field_Ranox_rest_Festival.lua`.

**Hero difficulty tier** (registry-only per the Findings above — no
separate content files needed): add `DI_RANOX_*_HERO_{NORMAL,HARD,EXPERT}`
and `DI_RANOX_*_HERO_HELL` entries to `march2014/data036/DungeonEnum.lua`
(extending Phase 1's work) and whatever `DungeonData.lua` scaling entry
June2017's own `DungeonData.lua` pairs with them — check whether March2014's
engine has any client-side concept of "Hero mode" at all before assuming
this is purely a data addition; if not, this may need to move to the
repo/engine phase group instead.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 12". Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data (check
for .src.lua siblings first). Decompiler: elstools/elstools.exe
decompile-batch -i <file> / compile-batch -v mar-2014 -i <file> (in place --
copy to scratch first).

Your task, in order: (1) Port the remaining UI files not yet handled by an
earlier phase: data034_kom/DLG_UI_FIELD_MAP_Ranox_{01,02,03,04}_NEW.lua,
DLG_UI_FIELD_MAP_Ranox_Rest_NEW.lua, DLG_Map_Local_Ranox_Back_NEW.lua -> all
to ./march2014/update/data034/. (2) Ask the user whether they want the
deferred cosmetic content (village seasonal reskins Festival/Halloween/XMAS/
XMAS_2/luminarie, Spa_Ranox + its LineMap + Snow variant,
Field_Ranox_rest_Festival) ported now; if yes, port them the same way as
their non-seasonal counterparts were ported in earlier phases. (3) Ask
whether they want the "Hero" difficulty tier added -- it's registry-only (no
separate content files exist for it in June2017, same stage files as
Expert, different DUNGEON_ID) but first check whether March2014's engine has
any client-side notion of Hero-mode dungeons at all (this may turn out to
need repo/C++ work rather than being a pure Lua addition -- if so, don't do
it here, flag it for the repo/engine phase group in RANOX_PORT_PLAN.md
instead).
```

---

### Phase 13 — Asset sweep + monster stat/drop dependency

**Goal**: catch every straggler and hand off the one dependency this port
can't self-resolve.

1. Cross-check that every one of the 135 new `NPC_UNIT_ID`s (Appendix B) has
   a ported unit-definition file somewhere under `march2014/update/`. List
   any that were missed by an earlier phase and port them now.
2. Bulk-copy the non-Lua Ranox assets, byte-identical, into
   `march2014/update/<source-archive>/`: `data154_kom` (~525 mesh/texture
   files — the largest single dump), NPC portraits/motions in
   `data013_kom`/`data153_kom`/`data181_kom`, textures in
   `data057_kom`/`data130_kom`/`data023_kom`/`data156_kom`/`data063_kom`/`data065_kom`,
   audio in `data096_kom`/`data155_kom`/`data078_kom`/`music/`, plus the
   standalone `data002_kom` (Secret-boss diorama assets),
   `data036_kom/Ranox.xml`, `eventscene/npc_ranox_png.swf`,
   `eventscene/ranox.swf`, `movie/movie_ranox_clear.avi` (see Appendix A for
   the full per-archive counts).
3. **Name** (do not attempt to build) the server-side monster stat/drop
   dependency: list every new `NPC_UNIT_ID` that's a real monster (exclude
   props like chests/gates/chains) and state plainly that neither client has
   a per-Ranox-dungeon stat/drop table — this needs a server-side Lua file
   the user authors and packs, per this repo's own documented convention.

**Kickoff prompt**:
```
Continuing the Ranox/Lanox village+dungeon port. Read RANOX_PORT_PLAN.md in
the repo root in full, especially "Phase 13" and Appendix A/B. Do NOT
consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch.

Client trees: March2014 = F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents.
June2017 = F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data.
No decompiling needed for most of this phase (it's mostly file copying), but
use elstools/elstools.exe decompile-batch for step 1's cross-check if
needed.

Your task: (1) Cross-check every one of the 135 new NPC_UNIT_IDs listed in
RANOX_PORT_PLAN.md's Appendix B has a ported unit-definition .lua file
somewhere under ./march2014/update/ already (from earlier phases) -- port
any that were missed. (2) Bulk-copy every non-Lua Ranox asset file
(byte-identical, no editing) from the June2017 tree into
./march2014/update/<same-archive-number>/ -- this is the ~525-file mesh/
texture dump in data154_kom, NPC portrait/motion assets in data013_kom/
data153_kom/data181_kom, textures in data057_kom/data130_kom/data023_kom/
data156_kom/data063_kom/data065_kom, audio in data096_kom/data155_kom/
data078_kom/music/, the data002_kom Secret-boss diorama assets,
data036_kom/Ranox.xml, eventscene/npc_ranox_png.swf, eventscene/ranox.swf,
and movie/movie_ranox_clear.avi. Full per-archive file counts are in
RANOX_PORT_PLAN.md's Appendix A. (3) Produce a short list of every new
NPC_UNIT_ID that is a real monster (not a prop like a chest/gate/chain), and
state clearly that neither client has a per-Ranox-dungeon stat/drop table --
name this as a dependency on a server-side Lua file the user needs to author
and pack (per this repo's own CLAUDE.md convention for server-owned data),
do not invent stat numbers.
```

---

## Repo/engine integration (separate phase group — start fresh, do not reuse this document's characterization of repo internals)

Not started during this planning pass, and none of the content phases above
depend on it to produce their file output — but nothing above is actually
*playable* until this lands. Each of these should be its own
fresh-conversation phase that explores the current repo state itself (this
plan intentionally avoided opening `X2Lib`/`KTDXLIB`/`KncWX2Server` while
researching, so trust nothing here beyond "these are the things that will
need doing," not the specifics of how):

1. **Sync the new enum IDs** from `march2014/data036/{Enum,DungeonEnum}.lua`
   into `KncWX2Server/Common/Enum/{Enum.h,DungeonEnum.h}` (or wherever the
   current repo actually keeps the C++ mirror of these — verify, don't
   assume the filenames from a prior attempt).
2. **Resolve the 8 engine gaps** (Findings table above) — for each, first
   confirm whether the C++ implementation already exists and just isn't
   exposed via a matching `Enum.lua` entry (cheap to check, might mean zero
   C++ work), before assuming new engine code is needed.
3. **Offline-emulator / world-map hookup**, since this build's
   `SERV_IRUHADEV_OFFLINE` mode may need dungeon-clear and village-entry
   handling wired for Ranox's new `WORLD_ID`/`DUNGEON_ID` values the same
   way it's wired for existing content — investigate current repo state
   fresh rather than assuming a shape.

**Kickoff prompt** (for whenever this group starts):
```
Starting the repo/engine integration phase group for the Ranox/Lanox
village+dungeon port. Read RANOX_PORT_PLAN.md in the repo root in full
first -- it documents the client-data-side work (already done or in
progress) and, in its final section, the repo-side work still needed. Do NOT
consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch -- that was a prior, abandoned attempt at this same
port. Unlike the rest of this port's planning, THIS phase group should
explore this repo's actual current C++ source (X2Lib, KTDXLIB, KncWX2Server)
directly -- the plan doc deliberately did not, so treat every repo-specific
claim in it as "needs verifying," not as fact.

Your task: (1) find where this repo currently mirrors client-side
WORLD_ID/DUNGEON_ID/NPC_UNIT_ID enum values on the C++/server side, and add
the new Ranox entries from ./march2014/data036/Enum.lua and DungeonEnum.lua
there too, following this repo's existing SERV_IRUHADEV_ flag convention
(see CLAUDE.md) for any client-code change this requires. (2) For each of
the 8 engine gaps listed in RANOX_PORT_PLAN.md's Findings section
(LT_FIRE_GROUND, TAT_ETC_NPC_AI_ON_OFF, TAT_LINE_CHANGE_TYPE,
TAT_ETC_PLAY_EFFECTSET, TAT_ETC_TALK_BOX, TAT_MESH_MOVE, CTT_RIM_LIGHT_BLUE,
DLT_EVENT_DUNGEON_TITLE), determine whether this repo's C++ already
implements the underlying feature and it's just missing from March2014's
Enum.lua binding, or whether real engine work is needed -- report findings
before implementing anything speculative. (3) Investigate what this build's
offline emulator (X2Lib/Offline/, see CLAUDE.md) needs for Ranox's new
village/dungeon content to work under SERV_IRUHADEV_OFFLINE, following the
same "play until something breaks, grep UNHANDLED in offline_packets.log"
loop CLAUDE.md documents for other offline-mode work.
```

---

## Appendix A — Full June2017 Ranox file inventory (125 `.lua` files)

Grouped by category and containing `data0NN_kom` archive. All 125 were
bulk-decompiled this session; `.src.lua` siblings should already exist next
to each.

**Village** (`data040_kom`, 7): `Ranox_Village.lua`, `Ranox_Village_Festival.lua`,
`Ranox_Village_Halloween.lua`, `Ranox_Village_LineMap.lua`,
`Ranox_Village_XMAS.lua`, `Ranox_Village_XMAS_2.lua`, `Ranox_Village_luminarie.lua`.

**Village sub-area "Spa"** (`data040_kom`, 3): `Spa_Ranox.lua`,
`Spa_Ranox_LineMap.lua`, `Spa_Ranox_Snow.lua`.

**Field / open world** (`data036_kom` + `data040_kom`, 15):
`Ranox_Field_00.lua`, `Ranox_Field_01.lua`, `Ranox_Field_02.lua`,
`Ranox_Field_03.lua` (all `data036_kom`); `Field_Ranox_Stage0.lua` /
`_LineMap.lua`, `Field_Ranox_Stage1.lua` / `_LineMap.lua`,
`Field_Ranox_Stage2.lua` / `_LineMap.lua`, `Field_Ranox_Stage3.lua` /
`_LineMap.lua`, `Field_Ranox_rest.lua`, `Field_Ranox_rest_Festival.lua`,
`Field_Ranox_rest_LineMap.lua` (all `data040_kom`).

**Dungeon difficulty containers** (`data035_kom`, 19): six lines ×
Normal/Hard/Expert — `Dungeon_Ranox_AshesVillage_{Normal,Hard,Expert}.lua`,
`Dungeon_Ranox_BurnedForest_{Normal,Hard,Expert}.lua`,
`Dungeon_Ranox_FinalHall_{Normal,Hard,Expert}.lua`,
`Dungeon_Ranox_FireTemple_{Normal,Hard,Expert}.lua`,
`Dungeon_Ranox_Gateway_{Normal,Hard,Expert}.lua`,
`Dungeon_Ranox_Geyser_{Normal,Hard,Expert}.lua`, plus
`Dungeon_Ranox_Secret_Hell.lua` (19th).

**Dungeon stage + LineMap pairs, by line** (`data040_kom`, 52):
- AshesVillage (10): `Stage{0,1,2,3,Boss}.lua` + LineMaps
- BurnedForest (8): `Stage{0,1,2,Boss}.lua` + LineMaps (no Stage3)
- FinalHall (10): `Stage{0,1,2,3,Boss}.lua` + LineMaps
- FireTemple (8): `Stage{0,1,2,Boss}.lua` + LineMaps
- GateWay (8): `Stage{0,1,2,Boss}.lua` + LineMaps
- Geyser (8): `stage0.lua`/`_linemap.lua` (lowercase!), `Stage{1,2,Boss}.lua` + LineMaps

**Secret dungeon** (`data040_kom`, 14): `Ranox_Secret_Boss_Stage.lua` /
`_LineMap.lua`, `Ranox_Secret_Extra_Boss_Stage.lua` (no paired LineMap
found), `Ranox_Secret_Extra_Stage{1,2,3}.lua` (no paired LineMaps found),
`Ranox_Secret_Stage{0,1,2,3}.lua` + LineMaps (8 files).

**PVP** (`data040_kom`, 2): `PVP_Ranox_GateWay.lua`, `_LineMap.lua`.

**Gate/switch trigger objects** (`data037_kom`, 3):
`RANOX_ASHESVILLAGE_GATE.lua`, `RANOX_ASHESVILLAG_SWITCH_GATE.lua` (typo,
missing trailing "E" — keep verbatim), `RANOX_BURNDEDFOREST_GATE.lua` (typo,
"BURNDED" not "BURNED" — keep verbatim).

**UI/Dialog/World-map** (`data034_kom` + `data217_kom`, 10):
`DLG_Map_Local_Ranox_Back_NEW.lua`, `DLG_UI_FIELD_MAP_Ranox_{01,02,03,04}_NEW.lua`,
`DLG_UI_FIELD_MAP_Ranox_Rest_NEW.lua`, `DLG_UI_FIELD_MAP_Ranox_Village_NEW.lua`,
`DLG_UI_RANOX_SECRET_FEVER_GAGE.lua` (all `data034_kom`);
`DLG_Map_Local_Ranox.lua`, `DLG_Map_Local_Ranox_Icon_NEW.lua` (both `data217_kom`).

**Non-Lua assets, by archive** (counts of files matching `*ranox*`):
`data154_kom` 525 (mesh/texture — largest dump), `data096_kom` 80 (voice
lines), `data155_kom` 62 (voice/audio), `data057_kom` 62 (world-map UI art),
`data130_kom` 51 (texture), `data023_kom` 48 (village/spa meshes),
`data013_kom` 46 (NPC portraits/motions), `data153_kom` 25, `data078_kom` 20
(ambience audio), `data002_kom` 11 (Secret-boss diorama), `data156_kom` 10,
`music/` 7 (BGM), `data181_kom` 4, `data063_kom` 4 (raid/field art),
`eventscene/` 2 (`.swf`), `data065_kom` 2, `movie/` 1 (`.avi`). Plus
standalone: `data036_kom/Ranox.xml`.

## Appendix B — New `NPC_UNIT_ID`s (135), grouped by dungeon line/field

Extracted by grepping every `NPC_UNIT_ID.NUI_*` token out of all 125
decompiled Ranox files (not by filename — most of these names don't contain
"Ranox" at all). A name can appear in more than one bucket where multiple
lines share a monster family (e.g. Fire Devil monsters span FinalHall,
FireTemple, and GateWay).

**Reuse as-is — already identical in both clients, zero new work** (10):
`NUI_ADAMS`, `NUI_ARIEL`, `NUI_BILLBOARD`, `NUI_CAMILLA`, `NUI_CHEST`,
`NUI_GATE_OF_NIGHTMARE`, `NUI_GLAVE`, `NUI_HELLEN`, `NUI_LURIEL`, `NUI_MU`.

**The one ID collision** — adopt at a fresh value, not June2017's `10067`:
`NUI_EDEL` (collides with March2014's existing `NUI_EVENT_ADAMS_UI_SHOP`).

**VILLAGE** (17 — story/quest NPCs and this line's own objects): `NUI_EDEL`,
`NUI_FACEOFF`, `NUI_IGNIA`, `NUI_MUL_MAI_EVENT`, `NUI_RANOX_ASHESVILLAGE_GATE`,
`NUI_RANOX_ASHESVILLAG_SWITCH_GATE`, `NUI_SMITH_ALUMINUM`, `NUI_SMITH_CART`,
`NUI_SMITH_LITTLEWORKER`, `NUI_SMITH_MACHINE`, `NUI_SMITH_REGIEON_POINT`,
`NUI_SMITH_RUNNER`, `NUI_SMITH_SCIENCE`, `NUI_SMITH_STAIN`,
`NUI_SMITH_WORKER`, `NUI_STEEL`, `NUI_SYOTA`.

**ASHESVILLAGE** (11): `NUI_RANOX_ASHESVILLAGE_GATE`,
`NUI_RANOX_ASHESVILLAG_SWITCH_GATE`, `NUI_SMITH_ALUMINUM`, `NUI_SMITH_CART`,
`NUI_SMITH_LITTLEWORKER`, `NUI_SMITH_MACHINE`, `NUI_SMITH_REGIEON_POINT`,
`NUI_SMITH_RUNNER`, `NUI_SMITH_SCIENCE`, `NUI_SMITH_STAIN`, `NUI_SMITH_WORKER`.

**BURNEDFOREST** (10): `NUI_MAG_CHARGER`, `NUI_MAG_NOM`, `NUI_MAG_STONE`,
`NUI_RAGE_BABYFIG`, `NUI_RAGE_DRYAD`, `NUI_RAGE_DRYADKENTA`,
`NUI_RAGE_MOTHERFIG`, `NUI_RANOX_BURNDEDFOREST_GATE`, `NUI_TRAP_FIRE_BIG`,
`NUI_TRAP_FIRE_SMALL`.

**FINALHALL** (24): `NUI_EVENT_FINALHALL_BOSS`, `NUI_EVENT_FINALHALL_BOSS2`,
`NUI_EVENT_FINALHALL_BOSS3`, `NUI_FIRETEMPLE_GATE`,
`NUI_FIRETEMPLE_REGIEON_POINT`, `NUI_FIRE_DEVIL_BERSERKER`,
`NUI_FIRE_DEVIL_BERSERKER_BOSS`, `NUI_FIRE_DEVIL_BERSERKER_BOSS_FINALHALL`,
`NUI_FIRE_DEVIL_BERSERKER_INCHANT`, `NUI_FIRE_DEVIL_CHAIN_BALL`,
`NUI_FIRE_DEVIL_CHAIN_STONE`, `NUI_FIRE_DEVIL_FIGHTER`,
`NUI_FIRE_DEVIL_FIGHTER_INCHANT`, `NUI_FIRE_DEVIL_IGNIA`,
`NUI_FIRE_DEVIL_IGNIA_BOSS` (undefined anomaly, see Findings — use
`_FINALHALL` variant), `NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL`,
`NUI_FIRE_DEVIL_IN`, `NUI_FIRE_DEVIL_IN_FINALHALL`, `NUI_FIRE_DEVIL_JIN`,
`NUI_FIRE_DEVIL_JIN_FINALHALL`, `NUI_FIRE_DEVIL_SKA`, `NUI_FIRE_DEVIL_SNIKER`,
`NUI_FIRE_DEVIL_SNIKER_INCHANT`, `NUI_MOUNT_GATE`.

**FIRETEMPLE** (12): `NUI_FIRETEMPLE_GATE`, `NUI_FIRE_DEVIL_BERSERKER`,
`NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`, `NUI_FIRE_DEVIL_CHAIN_IPLITAN`,
`NUI_FIRE_DEVIL_FIGHTER`, `NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`,
`NUI_FIRE_DEVIL_IN`, `NUI_FIRE_DEVIL_JIN`, `NUI_FIRE_DEVIL_SNIKER`,
`NUI_MOUNT_FIREBIRD`, `NUI_MOUNT_FIREBIRD_FIRETEMPLE`, `NUI_MOUNT_GATE`.

**GATEWAY** (12): `NUI_EVENT_GATEWAY_BOSS`, `NUI_FIRE_DEVIL_BERSERKER`,
`NUI_FIRE_DEVIL_BERSERKER_BOSS`, `NUI_FIRE_DEVIL_FIGHTER`,
`NUI_FIRE_DEVIL_IGNIA`, `NUI_MOUNT_FIREBIRD`, `NUI_MOUNT_GATE`,
`NUI_MOUNT_MAG_CHARGER`, `NUI_MOUNT_MAG_NOM`, `NUI_MOUNT_MAG_STONE`,
`NUI_MOUNT_POINT_STAGE_BOSS`, `NUI_SAMETEAM_IGNIA`.

**GEYSER** (12): `NUI_EVENT_GAYSER_BOSS` (source's own typo, keep verbatim),
`NUI_GEYSER_ALUMINUM`, `NUI_GEYSER_GATE`, `NUI_GEYSER_LITTLEWORKER`,
`NUI_GEYSER_MULLOCK`, `NUI_GEYSER_MULLOCK_CALL`, `NUI_GEYSER_MULMAI`,
`NUI_GEYSER_MULSAYRENA`, `NUI_GEYSER_MULSAYRENA_CALL`, `NUI_GEYSER_RUNNER`,
`NUI_GEYSER_SCIENCE`, `NUI_GEYSER_WORKER`.

**SECRET** (28): `NUI_SECRET_BERSERKER`, `NUI_SECRET_BOSS_LAVA`,
`NUI_SECRET_CRUEL_BERSERKER`, `NUI_SECRET_CRUEL_FIGHTER`,
`NUI_SECRET_CRUEL_MAG_CHARGER`, `NUI_SECRET_CRUEL_MAG_NOM`,
`NUI_SECRET_CRUEL_SNIKER`, `NUI_SECRET_EXTRA_MAGI_BOSS`, `NUI_SECRET_FIGHTER`,
`NUI_SECRET_FIREBIRD`, `NUI_SECRET_IN`, `NUI_SECRET_JIN`,
`NUI_SECRET_LAVA_DUMMY`, `NUI_SECRET_MAGI_BOSS`, `NUI_SECRET_MAGI_SCENE_A`,
`NUI_SECRET_MAGI_SCENE_B`, `NUI_SECRET_MAGI_SCENE_C`, `NUI_SECRET_MAGI_SUMMON`,
`NUI_SECRET_MAG_CHARGER`, `NUI_SECRET_MAG_NOM`, `NUI_SECRET_SCENE_1_0`,
`NUI_SECRET_SCENE_1_1`, `NUI_SECRET_SCENE_DUMMY`, `NUI_SECRET_SNIKER`,
`NUI_SECRET_STAGE0_CHAIN_OBJECT`, `NUI_SECRET_STAGE0_GATE`,
`NUI_SECRET_STAGE1_CHAIN1`, `NUI_SECRET_STAGE1_CHAIN2`.

**FIELD** (39, the `_FD` open-field tier plus story NPCs also seen in
fields): `NUI_EDEL`, `NUI_FACEOFF`, `NUI_IGNIA`,
`NUI_LOW_FINALHALL_BERSERKER_FD`, `NUI_LOW_FINALHALL_FIGHTER_FD`,
`NUI_LOW_FINALHALL_FIREBIRD_FD`, `NUI_LOW_FINALHALL_IGNIA_BOSS`,
`NUI_LOW_FINALHALL_SNIKER_FD`, `NUI_LOW_FINALHALL_STONE_FD`,
`NUI_LOW_GEYSER_ALUMINUM_FD_BOSS`, `NUI_LOW_GEYSER_LITTLEWORKER_FD`,
`NUI_LOW_GEYSER_MULLOCK_CALL_FD`, `NUI_LOW_GEYSER_MULLOCK_FD`,
`NUI_LOW_GEYSER_MULMAI_FD`, `NUI_LOW_GEYSER_RUNNER_FD`,
`NUI_LOW_GEYSER_SCIENCE_FD`, `NUI_LOW_GEYSER_WORKER_FD`,
`NUI_LOW_MAG_CHARGER_FD`, `NUI_LOW_MAG_NOM_FD`, `NUI_LOW_MAG_STONE_FD`,
`NUI_LOW_MOUNT_BERSERKER_BOSS`, `NUI_LOW_MOUNT_BERSERKER_FD`,
`NUI_LOW_MOUNT_CHARGER_FD`, `NUI_LOW_MOUNT_FIGHTER_FD`,
`NUI_LOW_MOUNT_FIREBIRD_FD`, `NUI_LOW_MOUNT_NOM_FD`, `NUI_LOW_MOUNT_STONE_FD`,
`NUI_LOW_RAGE_BABYFIG_FD`, `NUI_LOW_RAGE_DRYAD_FD`,
`NUI_LOW_RAGE_FATHERFIG_FD_BOSS`, `NUI_LOW_SMITH_ALUMINUM_FD_BOSS`,
`NUI_LOW_SMITH_LITTLEWORKER_FD`, `NUI_LOW_SMITH_MACHINE_FD`,
`NUI_LOW_SMITH_RUNNER_FD`, `NUI_LOW_SMITH_SCIENCE_FD`,
`NUI_LOW_SMITH_WORKER_FD`, `NUI_MUL_MAI_EVENT`, `NUI_STEEL`, `NUI_SYOTA`.

## Appendix C.5 — Full `WORLD_ID`, `VILLAGE_MAP_ID`, `DUNGEON_ID` lists (verbatim June2017 values, zero collisions confirmed)

**`WORLD_ID`** (43 — add to `march2014/data036/Enum.lua`'s `WORLD_ID` table):
```
WI_VILLAGE_RANOX = 582,
WI_RANOX_BURNDEDFOREST_STAGE0 = 583,
WI_RANOX_BURNDEDFOREST_STAGE1 = 584,
WI_RANOX_BURNDEDFOREST_STAGE2 = 585,
WI_RANOX_BURNDEDFOREST_STAGEBOSS = 586,
WI_RANOX_ASHESVILLAGE_STAGE0 = 587,
WI_RANOX_ASHESVILLAGE_STAGE1 = 588,
WI_RANOX_ASHESVILLAGE_STAGE2 = 589,
WI_RANOX_ASHESVILLAGE_STAGE3 = 590,
WI_RANOX_ASHESVILLAGE_STAGEBOSS = 591,
WI_BATTLE_FIELD_RANOX_FIELD_00 = 592,
WI_RANOX_GEYSER_STAGE0 = 593,
WI_RANOX_GEYSER_STAGE1 = 594,
WI_RANOX_GEYSER_STAGE2 = 595,
WI_RANOX_GEYSER_STAGEBOSS = 596,
WI_RANOX_GATEWAY_STAGE0 = 597,
WI_RANOX_GATEWAY_STAGE1 = 598,
WI_RANOX_GATEWAY_STAGE2 = 599,
WI_RANOX_GATEWAY_STAGEBOSS = 600,
WI_RANOX_FIRETEMPLE_STAGE0 = 601,
WI_RANOX_FIRETEMPLE_STAGE1 = 602,
WI_RANOX_FIRETEMPLE_STAGE2 = 603,
WI_RANOX_FIRETEMPLE_STAGEBOSS = 604,
WI_BATTLE_FIELD_RANOX_FIELD_01 = 605,
WI_BATTLE_FIELD_RANOX_FIELD_REST_00 = 606,
WI_BATTLE_FIELD_RANOX_FIELD_02 = 607,
WI_BATTLE_FIELD_RANOX_FIELD_03 = 608,
WI_RANOX_FINALHALL_STAGE0 = 609,
WI_RANOX_FINALHALL_STAGE1 = 610,
WI_RANOX_FINALHALL_STAGE2 = 611,
WI_RANOX_FINALHALL_STAGE3 = 612,
WI_RANOX_FINALHALL_STAGEBOSS = 613,
WI_RANOX_SECRET_STAGE0 = 676,
WI_RANOX_SECRET_STAGE1 = 677,
WI_RANOX_SECRET_STAGE2 = 678,
WI_RANOX_SECRET_STAGE3 = 679,
WI_RANOX_SECRET_BOSS_STAGE = 680,
WI_RANOX_SECRET_EXTRA_STAGE1 = 681,
WI_RANOX_SECRET_EXTRA_STAGE2 = 682,
WI_RANOX_SECRET_EXTRA_STAGE3 = 683,
WI_RANOX_SECRET_EXTRA_BOSS_STAGE = 684,
WI_SPA_RANOX = 726,
WI_RANOX_GATEWAY_PVPMAP = 751,
```
(Note the gap between `613` and `676`, and again to `726`/`751` — these are
not contiguous in June2017 either; adopt verbatim, don't try to compact them.)

**`VILLAGE_MAP_ID`** (8 — add to `march2014/data036/Enum.lua`'s
`VILLAGE_MAP_ID` table):
```
VMI_BATTLE_FIELD_RANOX_REST_00 = 1007,
VMI_SPA_RANOX = 1501,
VMI_RANOX = 20008,
VMI_DUNGEON_LOUNGE_RANOX = 30008,
VMI_BATTLE_FIELD_RANOX_FIELD_01 = 40029,
VMI_BATTLE_FIELD_RANOX_FIELD_02 = 40030,
VMI_BATTLE_FIELD_RANOX_FIELD_03 = 40031,
VMI_BATTLE_FIELD_RANOX_FIELD_04 = 40032,
```

**`DUNGEON_ID`** (19 Normal/Hard/Expert/Hell entries — add to
`march2014/data036/DungeonEnum.lua`'s `DUNGEON_ID` table; the Hero-tier
entries below are listed for completeness but deferred per Phase 12/Findings):
```
DI_RANOX_ASHESVILLAGE_NORMAL = 33280,
DI_RANOX_ASHESVILLAGE_HARD = 33281,
DI_RANOX_ASHESVILLAGE_EXPERT = 33282,
DI_RANOX_BURNDEDFOREST_NORMAL = 33270,
DI_RANOX_BURNDEDFOREST_HARD = 33271,
DI_RANOX_BURNDEDFOREST_EXPERT = 33272,
DI_RANOX_FINALHALL_NORMAL = 33320,
DI_RANOX_FINALHALL_HARD = 33321,
DI_RANOX_FINALHALL_EXPERT = 33322,
DI_RANOX_FIRETEMPLE_NORMAL = 33310,
DI_RANOX_FIRETEMPLE_HARD = 33311,
DI_RANOX_FIRETEMPLE_EXPERT = 33312,
DI_RANOX_GATEWAY_NORMAL = 33300,
DI_RANOX_GATEWAY_HARD = 33301,
DI_RANOX_GATEWAY_EXPERT = 33302,
DI_RANOX_GEYSER_NORMAL = 33290,
DI_RANOX_GEYSER_HARD = 33291,
DI_RANOX_GEYSER_EXPERT = 33292,
DI_RANOX_SECRET_HELL = 41271,
```
(`DI_RANOX_SECRET_COMMON = 41270` also exists in June2017 — likely the
non-Hell/matchmaking-lobby entry paired with `SECRET_HELL`; check
`DungeonData.lua`'s structure in Phase 9 to see if both are needed.)

**Deferred (Hero tier, Phase 12 — do not add in Phase 1)**:
`DI_RANOX_ASHESVILLAGE_HERO_{NORMAL,HARD,EXPERT}` = `100430`/`100431`/`100432`,
`_HERO_HELL` = `110210`; `DI_RANOX_BURNDEDFOREST_HERO_*` = `100420`/`100421`/`100422`,
`_HELL` = `110200`; `DI_RANOX_FINALHALL_HERO_*` = `100470`/`100471`/`100472`,
`_HELL` = `110250`; `DI_RANOX_FIRETEMPLE_HERO_*` = `100460`/`100461`/`100462`,
`_HELL` = `110240`; `DI_RANOX_GATEWAY_HERO_*` = `100450`/`100451`/`100452`,
`_HELL` = `110230`; `DI_RANOX_GEYSER_HERO_*` = `100440`/`100441`/`100442`,
`_HELL` = `110220`.

**`NUI_EDEL` replacement value** (resolved, not left as an exercise):
March2014's own `NPC_UNIT_ID` table's highest value in the `100xx` band is
`10067` (`NUI_EVENT_ADAMS_UI_SHOP`). **Use `10068`** for `NUI_EDEL` — confirmed
free (not assigned to anything in March2014's `Enum.lua`).

## Appendix C — Engine-gap table

See "Engine-gap audit" under Findings above for the full table (8 entries)
— not repeated here to avoid drift between two copies.

## Appendix D — March2014 registry files needing new entries

| File | Archive | Size (decompiled or raw) |
|---|---|---|
| `MapData.lua` | `data036` | 14,705 bytes (raw) |
| `WorldData.lua` | `data040` | 80,497 bytes (raw) / 2751 lines decompiled |
| `DungeonData.lua` | `data035` | 158,465 bytes (raw) / 13,290 lines decompiled |
| `BattleFieldData.lua` | `data036` | 25,570 bytes (raw) / 1773 lines decompiled |
| `BattleFieldServerData.lua` | `data036` | 958 bytes (likely not needed — server-side tuning, no Ranox-specific numbers known) |
| `PVP_Map_List.lua` | `data036` | 2,521 bytes (raw) / 115 lines decompiled |
| `Enum.lua` | `data036` | 8971 lines decompiled |
| `DungeonEnum.lua` | `data036` | 294 lines decompiled |
| `EnumString.lua` | `data036` | (not yet decompiled — do so in Phase 1) |

## Appendix E — Template village reference (Sander / Karuso Village)

March2014 files to consult whenever a schema question comes up, rather than
re-deriving the answer:

- `data035/Dungeon_Sander_Karuso_Village_{Normal,Hard,Expert}.lua` (1914,
  ~2000, 2176 decompiled lines respectively)
- `data040/Sander_Karuso_Village_Stage{0,1,2,Boss}.lua` + `_LineMap`/`_linemap`
  companions (casing is inconsistent in the source, same as several Ranox
  files — not an error, just this client's own inconsistency)
- `data040/Sander_Karuso_Village_Stage_Camera.lua`
- `data037/Sander_Karuso_Village_Gate.lua`, `Sander_Karuso_Village_Clear_NPC.lua`
- `DLG_UI_FIELD_MAP_Sander_Village_NEW.lua` (`data034`) and
  `DLG_UI_MINIMAP_Sander_Village.lua` — the latter lives in `datai014` (the
  CN-variant locale archive series, not `data034`) — note this one lives in
  a different archive series than the rest.
