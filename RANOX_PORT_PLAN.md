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

### Phase 5 findings (executed 2026-09-18)

**All 36 files landed and compile** (`compile-batch -v mar-2014`, individually
and together with the two patched registry files): 3 dungeon-difficulty
containers, 10 stage+LineMap files, and 23 monster/object unit-definition
files — the 22 named in the kickoff prompt plus `EVENT_FINALHALL_BOSS2.lua`/
`EVENT_FINALHALL_BOSS3.lua` (grouped with `EVENT_FINALHALL_BOSS.lua` under one
`NUI_EVENT_FINALHALL_BOSS{,2,3}` bullet in the prompt, but are three separate
files). None of these 23 unit-def files had pre-existing `.src.lua` siblings
from the original 125-file bulk-decompile (that pass only covered the
Appendix A inventory, which by construction excludes monster/object
definitions) — decompiled fresh this phase. `DI_RANOX_FINALHALL_{NORMAL,HARD,EXPERT}`
and all 22 `NPC_UNIT_ID`s were already in `march2014/data036/{Enum,DungeonEnum}.lua`
from Phase 1 — zero new *enum-value* work needed for the names the kickoff
prompt already knew about. Reachability was already wired by Phase 2 (both
`MapData:AddLocalMapInfo` and `DLG_Map_Local_Ranox.lua`'s `DungeonIDList`
already carry `DI_RANOX_FINALHALL_*`), so `DungeonData.lua` registration was
the only registry work this phase needed — ported verbatim from June2017,
including its `EXTRA_REQUIRE_DUNGEON_ID = { DUNGEON_ID.DI_RANOX_FIRETEMPLE_NORMAL }`
on all three difficulties (FinalHall requires FireTemple cleared first, not
the reverse — this is just what June2017's own data says; the enum value
exists from Phase 1 regardless of whether Phase 6 has landed yet).

**The `NUI_FIRE_DEVIL_IGNIA_BOSS` anomaly, resolved as planned.** All three
`Dungeon_Ranox_FinalHall_{Normal,Hard,Expert}.lua` files reference the
undefined `NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS` once each, inside a
`READY_NPC` preload list (confirmed via exhaustive grep — no other file in
this phase's set references it). All three resolved to
`NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL` with an inline
`SERV_IRUHADEV_RANOX_PORT`-tagged comment, per the plan's own Findings note.

**Two new enum-audit gaps found, outside Phase 1's four-table scope and
outside the Findings' 8-gap table** (same class Phase 4 flagged and warned
future phases to expect, from monster unit-definition files the 125-file
Appendix A audit doesn't cover):

- `BUFF_TEMPLET_ID.BTI_BUFF_DEVIL_CHAIN_BALL` and
  `BUFF_FACTOR_ID.BFI_BUFF_DEVIL_CHAIN_BALL` / `BFI_BUFF_DEVIL_IGNIA_BUFF`
  (`FIRE_DEVIL_CHAIN_BALL.lua`, `FIRE_DEVIL_IGNIA*.lua`) — missing *members*
  of tables that already existed in March2014's Enum.lua, same shape as
  Phase 4's `BFI_BUFF_MAG_STONE_ATTACK_B`. Added at June2017's own verbatim
  values (`484`, `553`, `537`), all confirmed free within their tables.
- **A more severe variant, not seen in Phases 1-4: two entire tables missing,
  not just members.** `DELETE_CONDITION_SOUND` (indexed by
  `FIRE_DEVIL_CHAIN_BALL.lua` for `.DCS_CUSTOM_STATE`, inside its
  `SOUND_PLAY_EX` entry) and `SKIN_MESH_RENDER_TYPE` (indexed by
  `FIRE_DEVIL_BERSERKER.lua`/`_INCHANT.lua`/`FIRE_DEVIL_FIGHTER.lua`/`_INCHANT.lua`
  for `.SMRT_UV_ANIM`, as `SKIN_MESH_RENDERTYPE`) don't exist anywhere in
  March2014's Enum.lua at all — confirmed absent from the whole client tree,
  not just Enum.lua, via a recursive search. Both exist as complete tables in
  June2017's Enum.lua. This is more dangerous than a missing member: since
  both references sit inside a table-constructor field assignment, indexing a
  nil global there aborts the whole file's load, not just evaluates to `nil`.
  Added both tables to March2014's Enum.lua in full, verbatim from June2017
  (not just the two referenced members), so the files load — see the tagged
  comment block at the end of `march2014/data036/Enum.lua`. **Checked whether
  this repo's C++ already reads either underlying Lua key** (not just the
  enum name, per this plan's own repo-phase instruction to check for a
  dormant C++ implementation first): neither `"SOUND_PLAY_EX"` nor
  `"SKIN_MESH_RENDERTYPE"` is parsed anywhere in `X2Lib`/`KTDXLIB` (compare
  `"SOUND_PLAY"`/`"SOUND_PLAY_LOOP"`, which are, in `X2GUNPC.cpp`; and
  `"RENDER_PARAM"`, the key this build's C++ actually reads for mesh render
  type, via `CKTDGXRenderer::RENDER_TYPE`). So **both added values are inert
  in this build**: the fix stops the load-time crash, but the sound
  delete-condition and the UV-animation render variant these files ask for
  don't actually take effect here — cosmetic/audio-only degradation, not a
  gameplay one.

**Eight unbound-method gaps, all in the newly-decompiled monster/object
files** (this phase's method-call audit — cross-checking every `:Method(`
call against `lua_tinker::class_def<...>` bindings across all of
`X2Lib`/`KTDXLIB`, not just the two top-level `.cpp` files Phase 4 checked —
`X2Lib/Offline/*.cpp` and `KTDXLIB/KTDXApp.cpp` etc. also bind methods).
Each is `SERV_IRUHADEV_RANOX_PORT`-tagged inline, same convention as Phases 2
and 4:

- `GetHostGameUnit` (5 occurrences across `FIRE_DEVIL_IGNIA.lua`,
  `FIRE_DEVIL_IGNIA_BOSS_FINALHALL.lua`, `FIRE_DEVIL_SKA.lua`,
  `FIRE_DEVIL_SNIKER.lua`, `FIRE_DEVIL_SNIKER_INCHANT.lua`) — a
  target-loss fallback, same shape as Phase 4's `GetStoredGUUser` gap;
  exists as a *different* class's C++ method (`CX2Game::GetHostGameUnit`,
  `X2Lib/X2Game.h`) but was never bound to Lua for any class. Omitted; the
  already-computed defaults (or, in `FIRE_DEVIL_SKA.lua`'s `GetUser` helper,
  a `nil` return) are kept instead of the host-unit override.
- `InitStateCoolTime` (`FIRE_DEVIL_SKA.lua`) and `ChangeMotion_LUA`
  (`FIRE_DEVIL_SKA.lua`) — same already-documented-elsewhere gap
  (`InitStateCoolTime`, from Phase 4's `RAGE_DRYADKENTA.lua`) and a new one
  respectively; both were the sole statement in their `STATE_START`/
  `STATE_END` handler, so both handlers degrade to empty (no-op) bodies.
- `GetNowStateID_LUA` and `GetIsLastStage` (`FIRE_DEVIL_BERSERKER.lua`,
  `FIRE_DEVIL_BERSERKER_BOSS.lua`, `FIRE_DEVIL_FIGHTER.lua`) — a
  "check whether this is the GateWay line's last stage before starting"
  gate, inside `..._VER_GATEWAY_{START_,}READY_STATE_START` handlers.
  **Confirmed these are GateWay-line-only state names FinalHall's own content
  never enters** (no FinalHall file references `"VER_GATEWAY"` at all) — they
  exist in these three files only because the whole unit-definition file is
  shared with Phase 8, which reuses it per this plan's own instruction not to
  re-port. `GetIsLastStage` doesn't exist anywhere in this repo's C++ at all
  (not just unbound — the concept isn't implemented); `GetNowStateID_LUA`
  isn't bound under that name for any class (`GetNowStateID`, no suffix, is
  bound, but only on `CX2GUNPC`). June2017 only skips the `StateChange_LUA`
  transition when `GetIsLastStage()` is true; since that can't be evaluated,
  all three files were changed to always transition, on the reasoning that
  risking a permanently-inactive elite/boss NPC is worse than an extra,
  harmless transition. The paired `SetFlag_LUA(9, ...)` bookkeeping is also
  dropped in all three. **This is a guess made without being able to exercise
  the real (GateWay) code path — flagged inline for Phase 8 to re-check once
  its own GateWay content lands and can actually be tested.**
- `GetIsGoNextPacketSended_LUA` and `Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ`
  (`FIRETEMPLE_REGIEON_POINT.lua`) — checked the underlying feature, not just
  the name, per this plan's repo-phase instruction: `EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ`
  **is** a real, already-wired packet in this repo —
  `CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ`
  (`X2Lib/X2DungeonGame.cpp`) sends it and the offline emulator
  (`X2Lib/Offline/Handlers_Room.cpp`) answers it — but that C++ method takes
  two `int` args and already fires automatically from `CX2DungeonGame`'s own
  per-frame update (gated on `m_bGoNextPacketSended` and a
  `TouchStartLineMapSet()` line-touch check), and was never exposed to Lua
  under this zero-arg `"Send_..."` name. Real gap, not a misnamed lookup.
  **Confirmed this does not strand FinalHall progression**: every `SUB_STAGE`
  this NPC appears in (`STAGE3`, all three difficulties) clears via the
  ordinary `CLEAR_COND0 = CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC` +
  `CLEAR_TYPE.CT_SUB_STAGE` mechanism, which is engine-driven and independent
  of this NPC's Lua. The omitted block was an additional, redundant
  "teleport players to the next line immediately once my summons die"
  convenience on top of that, not the actual gate — losing it means players
  walk to the next sub-stage's start line themselves instead of being
  auto-teleported there. The whole ~40-line block was block-commented out
  (`--[[ ]]`) rather than deleted, flagged inline for the repo/engine
  integration phase group as a candidate small wrapper binding (call the
  existing two-int-arg C++ method with the dungeon's own before/after
  sub-stage indices) if the convenience is wanted back.
- `SetMovieDescriptionData` (`EVENT_FINALHALL_BOSS.lua`, 3 call sites) —
  cutscene subtitle text timed to the boss-intro cutscene's animation. The
  cutscene itself (driven by `ANIM_NAME`/`EFFECT_SET_LIST`, both present and
  bound) is unaffected; only the on-screen caption text is lost. The
  `AnimEventTimer_LUA(...)` condition calls themselves were kept (only the
  `SetMovieDescriptionData` call inside each branch was removed), since the
  timer call may have a consumption side effect independent of its return
  value.

**Note for Phase 6 (FireTemple), as instructed**: all 23 unit-definition
files under the shared `NUI_FIRE_DEVIL_*`/`NUI_FIRETEMPLE_*`/`NUI_MOUNT_GATE`
family are now ported to `march2014/update/data037/` — do not re-port them.
Phase 6's own kickoff prompt already lists only its FireTemple-specific new
names (`NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`, `NUI_FIRE_DEVIL_CHAIN_IPLITAN`,
`NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`, `NUI_MOUNT_FIREBIRD{,_FIRETEMPLE}`), so
this requires no change there — but Phase 6 should re-run this phase's same
two audits (Enum-table-existence, not just Enum-member-existence; and
method-binding) against whatever *its own* new stage/dungeon files reference,
since both gap classes found here came from files outside the original
125-file inventory that the plan's Findings section audited.

**No gate/switch object file exists for FinalHall itself**, confirmed by
inventory search — it reuses `FIRETEMPLE_GATE.lua` and `MOUNT_GATE.lua`
(both ported this phase) rather than having its own, matching the plan's own
note to double-check rather than assume one was missing.

**Verification performed**: all 38 files (35 new/ported + the two patched
registry files) individually `compile-batch -v mar-2014` clean. A
compile→decompile round-trip was also run (same technique as Phases 2-4):
`Ranox_FinalHall_Stage0.lua` (an untouched, verbatim-ported file) came back
byte-for-byte line-identical to June2017's own decompile once sorted; the
edited `FIRE_DEVIL_SKA.lua` came back semantically identical to its own
source with the comments stripped (as expected — bytecode doesn't preserve
comments) modulo the same benign table-key-reordering and register-numbering
artifacts this plan's tooling section already documents. Every `:Method(`
call across all 36 non-registry files was checked against
`lua_tinker::class_def<...>` bindings scraped from every `.cpp` in
`X2Lib`/`KTDXLIB` including subdirectories (`X2Lib/Offline/`, etc. —
1385 unique bound names, cross-checked against a raw `class_def<` line count
to rule out multi-line binding calls or macro/token-paste names being missed;
none were).

**One low-priority, unverified cosmetic risk, noted rather than chased
further** (same non-verification precedent as Phases 3/4, which didn't check
string-table existence for their own `dungeonName`/`m_DungeonDescription`
either): FinalHall's `STR_ID_341xx`/`STR_ID_386xx`/`STR_ID_437xx` values are
well above the `STR_ID_322xx` range Phases 3/4 used. March2014's own
`data036/StringTable.lua` couldn't be grepped as plain text (still bytecode,
not decompiled this phase) to confirm these IDs resolve to real display
strings. If they don't, the affected fields (dungeon name/description,
ending-speech lines, cutscene captions) would render blank rather than error
— cosmetic only, `nil` in a table constructor doesn't crash Lua — so this
wasn't chased further, but a later phase populating/checking the string
table should be aware FinalHall's IDs are unverified.

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

### Phase 6 findings (executed 2026-09-18)

**All 16 new files landed and compile** (`compile-batch -v mar-2014`, individually
and together with the patched `DungeonData.lua`): 3 dungeon-difficulty
containers, 8 stage+LineMap files, and 5 monster/mount unit-definition files.
`.src.lua` siblings for the 11 dungeon/stage/LineMap files already existed from
the original bulk-decompile; the 5 unit-definition files (outside that
125-file inventory, same as every phase since 4) were decompiled fresh this
phase. All `DI_RANOX_FIRETEMPLE_{NORMAL,HARD,EXPERT}`, `WI_RANOX_FIRETEMPLE_*`,
and every `NPC_UNIT_ID` this line references (including this phase's five new
names — `NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`, `NUI_FIRE_DEVIL_CHAIN_IPLITAN`,
`NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`, `NUI_MOUNT_FIREBIRD`,
`NUI_MOUNT_FIREBIRD_FIRETEMPLE` — and Phase 5's shared `NUI_FIRE_DEVIL_*`
family) were already in `march2014/data036/{Enum,DungeonEnum}.lua` — zero new
enum-*value* work needed. Reachability was already wired by Phase 2 (both
`MapData:AddLocalMapInfo` and `DLG_Map_Local_Ranox.lua`'s `DungeonIDList`
already carry `DI_RANOX_FIRETEMPLE_*`), so `DungeonData.lua` registration
(ported verbatim from June2017, including `EXTRA_REQUIRE_DUNGEON_ID = {
DUNGEON_ID.DI_RANOX_GATEWAY_NORMAL }` on all three difficulties — FireTemple
requires GateWay cleared first, Phase 8, not yet landed; same forward
reference shape Phase 5 already established is fine to port ahead of its
dependency landing) was the only registry work this phase needed.

**The two `Ranox_FireTemple_Stage1_LineMap.lua` engine gaps, resolved by
omission with an added finding beyond what the plan anticipated.** Checked
both `TRIGGER_ETC_ACTION_TYPE.TAT_ETC_NPC_AI_ON_OFF` and
`TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE` against the underlying C++,
not just against Lua enum tables, per this plan's own repo-phase instruction:
**both are genuinely absent from March2014's C++ enums**, not just unbound
under another name — `KTDXLIB/KTDGLineMap.h:95-104`'s `TRIGGER_LINE_ACTION_TYPE`
stops at `TAT_LINE_CHANGE_TELEPORT`, and `:106-113`'s `TRIGGER_ETC_ACTION_TYPE`
stops at `TAT_ETC_ACTIVE_LAYER` — no dormant support to wire up. **Separately
found, reading `KTDGLineMap.cpp`'s `AddTrigger_LUA` and
`KncWX2Server/Common/Lua/KLuaManager.h`'s `LUA_GET_VALUE_ENUM` macro**: a
missing enum member here would *not* have aborted the file's load even if left
in verbatim — `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE` is a plain table
index into an existing, real Lua table (`TRIGGER_LINE_ACTION_TYPE` itself,
confirmed present in `march2014/data036/Enum.lua`, just missing this one key),
which evaluates to `nil` rather than erroring, and `LUA_GET_VALUE_ENUM`
gracefully defaults a failed/nil read to `TAT_LINE_NONE`/`TAT_ETC_NONE`. This
is a different (safer) gap shape than the method-call gaps Phases 4/5 found,
which do abort on a call to a genuinely unbound method. Despite that, the
omission was still made explicit and documented rather than left implicit, to
match this port's established convention and keep the gap auditable: removed
`TRIGGER_DATA_LINE1/2/3` (the three `TAT_LINE_CHANGE_TYPE` entries, both
occurrences of the pattern, on `TRIGGER_ID` 0 and 1) and `TRIGGER_DATA_ETC2`
(the `TAT_ETC_NPC_AI_ON_OFF` entry, same two `TRIGGER_ID`s) entirely, and
emptied `TRIGGER_ID` 2 and 3's `TRIGGER_DATA0` to `{}` since their sole
content was this same NPC-AI-toggle action — both triggers are kept (still
referenced by index elsewhere in the file) but now fire as no-ops. Net effect:
the scripted line-type reclassification on lines 11/12/13/33/34/35 and the
NPC-AI on/off toggle cues (4 trigger fires) don't happen; every other trigger
sub-action in this file (mesh animation swaps, line enable/disable, camera
shake, ambient sound) is untouched and verified present in
`march2014/data036/Enum.lua`. Verified via a sorted-multiset diff against
June2017's own decompile that these omissions are the *only* content
difference (beyond the plan's already-documented benign table-key-reordering
artifact) — nothing else in the file's ~650 lines was altered. Flagged inline
(`SERV_IRUHADEV_RANOX_PORT`-tagged) for the repo/engine integration phase
group, same convention as every prior phase's omissions.

**Three more unbound-method gaps found in this phase's own unit-definition
files, outside the Findings' 8-gap table** (same class every phase since 4 has
found — files outside the original 125-file Appendix A inventory need their
own audit). All three are confined to `FIRE_DEVIL_CHAIN_IPLITAN.lua` (the
boss); the other 4 new unit-def files
(`FIRE_DEVIL_BERSERKER_FIRETEMPLE`/`FIRE_DEVIL_FIGHTER_FIRETEMPLE`/
`MOUNT_FIREBIRD`/`MOUNT_FIREBIRD_FIRETEMPLE`) came back clean on both the
enum-table-existence and method-binding audits and were ported 100% verbatim.
Each is `SERV_IRUHADEV_RANOX_PORT`-tagged inline:

- `SetActiveTrigger_LUA` (2 occurrences) — arms a LineMap trigger by index from
  the boss's own state handlers (once on its start state ending, once mid-fight
  before a special attack). Not bound anywhere in `X2Lib`/`KTDXLIB` (not just
  unbound — not declared at all, checked both trees). First occurrence sits
  alongside three bound `SetFlag_LUA`/`SetVector_LUA` calls that are kept; the
  second occurrence was the sole content (with `InitStateCoolTime`, next
  bullet) of its handler, which is now an empty no-op.
- `InitStateCoolTime` — same already-documented-elsewhere gap as Phase 4's
  `RAGE_DRYADKENTA.lua` and Phase 5's `FIRE_DEVIL_SKA.lua`; paired with the
  second `SetActiveTrigger_LUA` call above as this state's entire body.
- `DeleteNonControlDebuff` — a debuff-clear call alongside a bound
  `SetFlag_LUA` call (kept) at the start of an attack-event state. Not bound
  anywhere in `X2Lib`/`KTDXLIB`.

**Verification performed**: all 17 files (16 new + the patched registry file)
individually `compile-batch -v mar-2014` clean, then re-verified together in
one batch invocation after all edits landed. A compile→decompile round-trip
on the edited `Ranox_FireTemple_Stage1_LineMap.lua` (via the sorted-multiset
diff described above) confirmed the only differences from June2017's own
source are the documented omissions. The enum-table/member audit for this
phase's 5 new unit-definition files (48 distinct `TABLE.MEMBER` tokens) found
one false positive worth noting for future phases: `INIT_PHYSIC.{JUMP_SPEED,
RUN_SPEED,WALK_SPEED}` is not a global `Enum.lua` table reference at all — it's
a local table literal (`INIT_PHYSIC = { JUMP_SPEED = ..., ... }`) defined
earlier in the same file and indexed from within it. A naive `TABLE.MEMBER`
grep flags this as a missing global; check for a same-file local definition
before concluding a table is absent from `Enum.lua`. The method-binding audit
(25 distinct `:Method(` calls across the 5 files) was checked against a fresh
scrape of `class_def<...>` bindings from `X2Lib`/`KTDXLIB` (1309 unique names
this session's scrape found — a different count than Phase 5's 1385, most
likely the same multi-line-`class_def`-call underscan Phase 5's own note
already flagged as a risk, not evidence of a shrinking bind surface; the three
gaps above were independently confirmed absent by name via a direct
whole-tree `grep`, not just by missing from the scrape, so this discrepancy
doesn't put their disposition in question).

**Windows path-length note for whoever runs `compile-batch` next**: this
session's default scratchpad path (nested under
`AppData\Local\Temp\claude\<long project-derived name>\<uuid>\scratchpad\`)
combined with a `.luac.tmp` output filename hit exactly 260 characters and
made `elstools.exe compile-batch` fail with a `luajit: ...: No such file or
directory` on roughly every other file in a batch (alternating pass/fail by
index, not by file content — a red herring worth not chasing as a content bug
first). Copying the same files to a short path (`C:\tmp\...`) before compiling
fixed it immediately for every file, gap-edited or not. Not specific to this
phase's files — worth remembering for any future phase's own verification
pass, especially once a phase's working set is large enough that the default
scratch path's length becomes the binding constraint rather than content.

**Major addendum, found post-review: a third registry this plan's Output
Convention section never named, and it's load-bearing — `NPCTemplet.lua`
(`data037`).** `g_pUnitManager:AddNPCTemplet({ NPC_ID = NPC_UNIT_ID.NUI_X,
LUA_FILE_NAME = "X.lua", ... })` is what actually tells the client which
script file to run for a given `NPC_UNIT_ID` — confirmed by reading
`CX2UnitManager::AddNPCTemplet_LUA` (`X2Lib/X2UnitManager.cpp:621`, reads
`LUA_FILE_NAME` into `m_LuaFileName`) and its only two consumers,
`X2Lib/X2GUNPC.cpp:1735` and `:1864` (`CX2GUNPC`'s constructor uses
`m_templet.m_LuaFileName` to actually load the unit's script). **A ported
unit-definition file with no `AddNPCTemplet` entry exists on disk but is never
loaded for its `NPC_UNIT_ID` — the same class of silent gap this plan's own
Findings section warns about for `MASS_FILE_FIRST`/loose-file precedence, just
in a different registry.** This phase adds it retroactively for its own 10
FireTemple-referenced NPCs (`NUI_MOUNT_GATE`, `NUI_FIRE_DEVIL_BERSERKER`,
`NUI_FIRE_DEVIL_SNIKER`, `NUI_FIRE_DEVIL_JIN`, `NUI_FIRE_DEVIL_IN`,
`NUI_FIRE_DEVIL_CHAIN_IPLITAN`, `NUI_FIRETEMPLE_GATE`,
`NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`, `NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`,
`NUI_MOUNT_FIREBIRD_FIRETEMPLE`) plus `NUI_MOUNT_FIREBIRD` (plain — unused by
FireTemple's own content, confirmed by exhaustive grep, but its unit-def file
was ported this phase per the plan's own file list and Phase 8 reuses it), to
`march2014/data037/NPCTemplet.lua` — all 11 entries copied verbatim from
June2017's own `data037_kom/NPCTemplet.lua` (`MOTION`/`NAME`/`DESC` text
included), each confirmed appearing exactly once there and not colliding with
any of the base file's existing ~2100 entries. Verified compiles clean
(`compile-batch -v mar-2014`) and that the first 16929 lines are byte-identical
to a fresh decompile of March2014's own original file (i.e. purely additive).
This is an existing March2014 file being patched in place, so it follows
Output Convention rule 1 (`march2014/data037/NPCTemplet.lua`) even though the
convention section's explicit file list didn't name it — that list should be
treated as incomplete, not exhaustive; add `NPCTemplet.lua` to it.

**This same gap almost certainly affects every monster/object `NPC_UNIT_ID`
ported by Phases 3, 4, and 5 too** — none of their findings mention
`NPCTemplet.lua`, and this phase's own kickoff prompt didn't know to ask for
it either. This wasn't re-derived or fixed for those phases here (out of
scope for "port the FireTemple line" — would mean auditing and patching
`NPCTemplet.lua` for AshesVillage's 11, BurnedForest's 10, and FinalHall's 23
`NPC_UNIT_ID`s, none of which this phase touched). **Recommend a dedicated
catch-up pass — either its own phase or folded into whichever phase runs
next — that greps every already-ported `update/data037/*.lua` filename
against `march2014/data037/NPCTemplet.lua` and files an `AddNPCTemplet` entry
for any that are missing, the same way this phase just did for its own 11.**
Until that lands, AshesVillage/BurnedForest/FinalHall's monsters exist as
files but their NPCs cannot actually spawn with working AI/behavior in-game.

**Three smaller findings-completeness notes, none blocking:**

- `Dungeon_Ranox_FireTemple_Hard.lua` and `_Expert.lua` are byte-identical in
  June2017's own source (confirmed via `diff`, and both compile to the same
  8097 bytes) — ported verbatim per this plan's "transcribe, don't tidy" rule,
  noted explicitly here rather than left to look like an accidental duplicate.
- `Dungeon_Ranox_FireTemple_Normal.lua`'s `STAGE0.SUB_STAGE0` has three
  consecutive `NPC_REWARD_RATE = { ... }` table-constructor assignments (one
  each for `NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`, `NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE`,
  `NUI_FIRE_DEVIL_SNIKER`) — Lua's last-assignment-wins for a duplicate table
  key, so only the last of the three is ever live. This is June2017's own data
  (ported verbatim, correctly, per "transcribe don't tidy"), but it also means
  this plan's tooling-section claim that table-key *reordering* is
  "semantically inert for named-key table literals" doesn't extend to
  duplicate keys — a decompile→recompile round-trip on a file with this
  pattern is only guaranteed to preserve whichever duplicate the decompiler
  happened to emit last, not necessarily the original bytecode's winner. Not
  chased further (verbatim is correct either way; the *client's own* file
  already has this property), but worth a future phase knowing about if a
  reward-rate value looks wrong after a round-trip.
- FireTemple's `STR_ID_34168`/`34172` and speech-block IDs (`34078`-`34106`,
  `34137`-`34139`, `38645`-`38647`, `43789`) fall in the same unverified range
  Phase 5 flagged (`STR_ID_341xx`/`386xx`/`437xx`, above what Phases 3/4 used)
  — March2014's `data036/StringTable.lua` still hasn't been decompiled to
  confirm these resolve to real strings. Carrying Phase 5's open flag forward
  rather than re-closing it. Cosmetic only if unresolved (blank text, no
  crash).

**Appendix B correction**: its FIRETEMPLE bucket (12 names) includes
`NUI_FIRE_DEVIL_FIGHTER` (plain, non-`_FIRETEMPLE`-suffixed) alongside
`NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE`. Exhaustive grep of every file this phase
ported (dungeon defs, stage/LineMap files, and all 5 new unit-def files) found
zero references to plain `NUI_FIRE_DEVIL_FIGHTER` anywhere in FireTemple's own
content — it's a real, already-ported `NPC_UNIT_ID` (Phase 5, for FinalHall)
that Appendix B's loose "shared family" grouping over-attributed to this line
too. No action needed here since FireTemple doesn't spawn it, but noting the
bucket is approximate, not a second source to double-check against for future
phases' own completeness claims.

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

### Phase 7 findings (executed 2026-09-18)

**All 23 new files landed and compile** (`compile-batch -v mar-2014`, individually
and together with the two patched registry files): 3 dungeon-difficulty
containers, 8 stage+LineMap files, and 12 monster/object unit-definition
files. `.src.lua` siblings for the 11 dungeon/stage/LineMap files already
existed from the original bulk-decompile; the 12 unit-definition files
(outside that 125-file inventory, same as every phase since 4) were
decompiled fresh this phase. All `DI_RANOX_GEYSER_{NORMAL,HARD,EXPERT}` and
every `NPC_UNIT_ID` this line references were already in
`march2014/data036/{Enum,DungeonEnum}.lua` from Phase 1 — zero new
enum-*value* work needed. Reachability was already wired by Phase 2 (both
`MapData:AddLocalMapInfo` and `DLG_Map_Local_Ranox.lua`'s `DungeonIDList`
already carry `DI_RANOX_GEYSER_*`), so `DungeonData.lua` registration
(Normal/Hard/Expert, ported verbatim from June2017 including
`EXTRA_REQUIRE_DUNGEON_ID = { DUNGEON_ID.DI_RANOX_ASHESVILLAGE_NORMAL }` on
all three difficulties; Hero-tier entries skipped per this plan's default)
was the only *DungeonData* registry work this phase needed. `NUI_CHEST`,
referenced by several of the unit-def files, is confirmed one of the ~10
pre-existing shared NPCs the Findings section already identified — zero new
work.

**The three `Ranox_Geyser_stage0_linemap.lua` engine gaps, resolved by
omission, same technique as Phase 6.** Checked all three
(`TRIGGER_ETC_ACTION_TYPE.TAT_ETC_PLAY_EFFECTSET`, `.TAT_ETC_TALK_BOX`,
`TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE`) against the underlying C++, not
just Lua enum tables: all three are genuinely absent from March2014's C++
enums too — `KTDXLIB/KTDGLineMap.h:91-94`'s `TRIGGER_MESH_ACTION_TYPE` stops
at `TAT_MESH_CHANGE_SPEED` (no `MOVE`), and `:108-113`'s
`TRIGGER_ETC_ACTION_TYPE` stops at `TAT_ETC_ACTIVE_LAYER` (no
`PLAY_EFFECTSET` or `TALK_BOX`) — no dormant support to wire up, same verdict
as Phase 6's own two gaps in these same two tables. Same safer gap shape
Phase 6 documented (a missing member of an *existing* Lua table evaluates to
`nil`, not a load-time error, and `LUA_GET_VALUE_ENUM` defaults a failed read
to `TAT_ETC_NONE`/`TAT_MESH_NONE`) — but omitted and documented explicitly
anyway, to keep the gap auditable per this port's established convention.
Three triggers affected: `TRIGGER_ID` 0 and 2 each lost one
`TRIGGER_DATA_ETC0` (`TAT_ETC_TALK_BOX`, a work-end speech-bubble cue) while
keeping their two `TRIGGER_DATA_MESH0/1` mesh-animation-swap entries intact;
`TRIGGER_ID` 1's entire `TRIGGER_DATA0` body was all three gapped members at
once (`TAT_ETC_PLAY_EFFECTSET` ×2, `TAT_MESH_MOVE` ×1), so it collapsed to
`TRIGGER_DATA0 = {}` — the trigger is kept (still referenced by `TRIGGER_ID`
elsewhere in this file's `AddLine_LUA` calls) but now fires as a no-op.
Verified via a comments-stripped sorted-multiset diff against June2017's own
decompile that these omissions are the *only* content difference.

**A new, more severe gap class this phase found: two methods missing not
just from Lua binding but from the C++ class itself, in *scene* files, not
monster unit-definitions.** Neither the Findings' 8-gap table (enum-only) nor
any prior phase's method-binding audit (which only ever covered monster/
object unit-def files, per Phases 4-6's own notes) would have caught this —
it took extending the method-binding audit to this phase's *dungeon/stage*
files too, which no phase had done before. `Ranox_Geyser_stage0.lua` (3
occurrences) and `Ranox_Geyser_Stage1.lua` (1 occurrence) call
`g_pObjectSkinMesh:SetTriggerID(n)` and `:SetShowObject(false, true)` on a
`CX2WorldObjectSkinMesh` instance (the class behind `g_pWorld:CreateObjectSkinMesh()`,
`X2Lib/X2WorldObjectSkinMesh.h`/`.cpp`). The two methods turned out to have
different gap shapes, worth being precise about rather than lumping together
as "both nonexistent" (an earlier draft of this finding made exactly that
overgeneralization from a `head`-truncated grep before being corrected):

- `SetTriggerID` doesn't exist anywhere in this repo's C++ at all — checked
  `X2Lib`/`KTDXLIB` exhaustively (not head-limited), zero hits. Not a binding
  gap; the method itself was never implemented in this build.
- `SetShowObject` **does** exist in C++, as `CKTDGObject::SetShowObject(bool
  bShow, bool bForce = false)` (`KTDXLIB/KTDGObject.h:305`), and
  `CX2WorldObjectSkinMesh`'s inheritance chain does reach it in lua_tinker —
  `class_inh<CX2WorldObjectSkinMesh, CX2WorldObject>` and
  `class_inh<CX2WorldObject, CKTDGObject>` are both registered
  (`X2Lib/X2Main.cpp:3694`, `:3645`). But `CKTDGObject`'s own 5-entry
  `class_def<CKTDGObject>` block (`KTDXLIB/KTDXApp.cpp:1651-1655` — `SetLayer`,
  `SetForceLayer`, `SetAlphaObject`, `SetScale`, `SetOverUI`) doesn't bind it,
  so the Lua name `"SetShowObject"` resolves to nothing through this object's
  chain. Checked whether it's bound anywhere else under that name at all
  (exhaustive, not truncated): it isn't. A similar-looking pair of bindings on
  `CX2GUNPC`/`CX2GUUser` (`X2Lib/X2Main.cpp:4231`, `:4574`) map those classes'
  own `SetShowObject` C++ methods to the *different* Lua name `"SetShow"`, and
  neither class is an ancestor of `CX2WorldObjectSkinMesh` anyway.

Both still abort the whole script's load if called (same rule this port has
established throughout for any unresolved Lua method name), so all 4 call
sites were omitted and `SERV_IRUHADEV_RANOX_PORT`-tagged inline regardless of
which of the two gap shapes applies. Effect: these steel-pump/pump props now
play their `WorkLoop` animation continuously from spawn and start visible
immediately, instead of starting hidden and being armed by their paired
LineMap trigger (the same `TRIGGER_ID`s 0/2 already gap-omitted above) —
cosmetic-only, no script-load failure. One `SetTriggerID(3)` call in
`Ranox_Geyser_stage0.lua` was found to reference a `TRIGGER_ID` that doesn't
even exist in this file's own paired LineMap (`Ranox_Geyser_stage0_linemap.lua`
only defines `TRIGGER_ID` 0/1/2) — confirmed as June2017's own orphaned
reference, not something this port introduced, and transcribed verbatim per
this plan's "transcribe, don't tidy" rule rather than silently corrected.
**Flagged for the repo/engine integration phase group as a new item**: this
gap class (scene-file calls to unresolved `CX2WorldObjectSkinMesh` Lua method
names) should be added to that group's checklist alongside the 8 enum gaps,
since it wasn't discovered until this phase. `SetTriggerID` would need new
C++ (a member function plus a binding); `SetShowObject` would need only a
new `class_def<CKTDGObject>` binding line, since the C++ method already
exists.

**Three already-known method-binding gaps recurred, all
`SERV_IRUHADEV_RANOX_PORT`-tagged inline, same disposition as their prior
occurrences:**

- `GetStoredGUUser` (`GEYSER_ALUMINUM.lua`, `GEYSER_MULLOCK.lua`, one
  occurrence each) — the same target-loss-fallback gap Phase 4 first found in
  `RAGE_DRYADKENTA.lua`. Omitted the same way: the `if slot2:GetTargetUser()
  == nil then ... end` block's body is emptied, leaving `slot3` `nil` in the
  no-target case instead of falling back to a stale stored target.
- `SetMovieDescriptionData` (`EVENT_GAYSER_BOSS.lua`, 6 occurrences) — the
  same cutscene-subtitle gap Phase 5 first found in
  `EVENT_FINALHALL_BOSS.lua`. Omitted the same way: each
  `if/elseif slot2:AnimEventTimer_LUA(...) then` condition is kept (the timer
  call may have a side effect independent of its return value, per Phase 5's
  own reasoning) but its body (the `SetMovieDescriptionData` call) is emptied.
  The cutscene itself still plays via `ANIM_NAME`/`EFFECT_SET_LIST`; only the
  on-screen caption text is lost.

**A retroactive fix to already-shipped Phase 3 content, found by this
phase's own audit, not scoped to Geyser at all**: `SMITH_ALUMINUM.lua`
(AshesVillage's aluminum-golem monster, ported in Phase 3) has the exact same
unaddressed `GetStoredGUUser` call this phase found twice in its own Geyser
siblings — same function name (`*_ATTACK_B_READY_DASH_STATE_START`), same
shape, presumably the same shared monster archetype reskinned across dungeon
lines. Phase 3 predates Phase 4's discovery of the method-binding-audit
technique, so it shipped with the call still present — meaning
`SMITH_ALUMINUM.lua` has had a script-load-aborting call sitting in already-
deployed content since Phase 3. Fixed here the same way as its Geyser
siblings, tagged inline as a retroactive fix with the phase and reason noted.
**Worth a future audit pass**: since Phase 3 shipped before the method-
binding-audit technique existed, its other 10 unit-def files
(`SMITH_CART/LITTLEWORKER/MACHINE/REGIEON_POINT/RUNNER/SCIENCE/STAIN/WORKER`
plus the two gate files) were never checked against `class_def<...>`
bindings either — this phase only checked the one file its own cross-
reference (`GetStoredGUUser` search across all of `data037_kom`) happened to
surface. A dedicated pass re-running Phase 4's audit technique against all of
Phase 3's output is recommended, same shape as Phase 6's still-open
`NPCTemplet.lua` catch-up below.

**Confirmed load-bearing, same as Phase 6's own finding: this line's 12
`NPCTemplet.lua` entries were also missing and are added here.** Same
registry, same reasoning Phase 6 established (`g_pUnitManager:AddNPCTemplet`'s
`LUA_FILE_NAME` is what `CX2GUNPC` actually loads per `NPC_UNIT_ID`; a ported
unit-def file with no entry here exists on disk but never loads). All 12
entries ported verbatim from June2017's own `NPCTemplet.lua`
(`MOTION`/`NAME`/`DESC` text included) to `march2014/data037/NPCTemplet.lua`,
appended after Phase 6's own block, and confirmed not colliding with any of
the March2014 base file's pre-existing entries (checked both by `NPC_ID` and
by `LUA_FILE_NAME`, same two axes Phase 6 checked for its own 11 additions).
One verbatim-transcribed oddity worth
flagging rather than silently fixing: `NUI_GEYSER_MULSAYRENA_CALL`'s entry
carries `NUI_GEYSER_MULLOCK_CALL`'s own `MOTION`/`NAME`/`DESC` text
("Motion_GEYSER_MULLOCK_CALL.x" / "Entranced Merman") — June2017's own
copy-paste, not a porting error, kept per "transcribe, don't tidy."
**Phase 6's own catch-up recommendation is still open and still out of
scope here**: AshesVillage/BurnedForest/FinalHall's `NPC_UNIT_ID`s (Phases
3-5) still have no `NPCTemplet.lua` entries at all, so those lines' monsters
still exist as files but can't actually spawn with working AI in-game. A
dedicated catch-up pass (or folding it into whichever phase runs next) is
still recommended.

**STR_ID range flag carried forward, not re-closed**: Geyser's
`DungeonData.lua` entries use `STR_ID_38641`/`38642` and `STR_ID_43787`, the
same unverified `386xx`/`437xx` bands Phases 5 and 6 both flagged (March2014's
own `data036/StringTable.lua` still hasn't been decompiled to confirm these
resolve to real display strings). Cosmetic only if unresolved (blank ending-
speech text, no crash) — same disposition as its two prior occurrences.

**Verification performed**: all 26 files (23 new/ported + the three patched
registry files, `DungeonData.lua`/`NPCTemplet.lua`/`SMITH_ALUMINUM.lua`)
individually `compile-batch -v mar-2014` clean, then re-verified together in
one combined batch invocation after every edit landed. A comments-stripped,
sorted-multiset diff against June2017's own decompile was run on every edited
file (`Ranox_Geyser_stage0_linemap.lua`, `Ranox_Geyser_stage0.lua`,
`Ranox_Geyser_Stage1.lua`, `GEYSER_ALUMINUM.lua`, `GEYSER_MULLOCK.lua`,
`EVENT_GAYSER_BOSS.lua`, `SMITH_ALUMINUM.lua`) and confirmed the only
differences are the documented omissions, plus the benign trailing-comma-
placement and register-numbering artifacts this plan's tooling section
already documents. A sorted-multiset diff on an untouched verbatim file
(`Ranox_Geyser_Stage2.lua`, one of the stage files not affected by the
`SetTriggerID`/`SetShowObject` gap above) came back byte-identical. The
enum-table/member audit
covered both categories of file this phase touched — the 12 unit-def files
(48 distinct `TABLE.MEMBER` tokens, all present except the 3 known gaps) and
the 11 dungeon/stage/LineMap files separately (41 distinct tokens, same 3
known gaps, nothing else missing) — extending Phase 6's own note that the
125-file Appendix A inventory doesn't cover monster files to also explicitly
re-run the dungeon/stage files, which hadn't been separately re-audited since
Phase 1's original methodology (enum-*value* existence only, not enum-
*table*-member completeness) was established. The method-binding audit (56
distinct `:Method(` calls across the 12 unit-def files, 1385 unique bound
names scraped from `X2Lib`/`KTDXLIB` — matches Phase 5's own scrape count
exactly, a useful cross-session sanity check that the bind surface hasn't
drifted) found the three known-class gaps; extending that same audit to the
11 dungeon/stage/LineMap files (their own distinct `:Method(` calls) is what
surfaced the new `SetTriggerID`/`SetShowObject` gap class documented above.

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

### Phase 8 findings (executed 2026-09-18)

**All 17 new/ported files landed and compile** (`compile-batch -v mar-2014`,
individually and together with the three patched registry files, 20 total —
matching the tool's own `"total":20`): 3 dungeon-difficulty containers, 8
stage+LineMap files, and 6 monster/mount/event unit-definition files
(`EVENT_GATEWAY_BOSS`, `MOUNT_MAG_CHARGER`, `MOUNT_MAG_NOM`,
`MOUNT_MAG_STONE`, `MOUNT_POINT_STAGE_BOSS`, `SAMETEAM_IGNIA`). `.src.lua`
siblings for the 11 dungeon/stage/LineMap files already existed from the
original bulk-decompile; the 6 unit-definition files (outside that 125-file
inventory, same as every phase since 4) were decompiled fresh this phase.
`DI_RANOX_GATEWAY_{NORMAL,HARD,EXPERT}`, `WI_RANOX_GATEWAY_*`, and every
`NPC_UNIT_ID` this line references (its 6 new names plus the 6 reused ones
from Phases 5/6) were already in `march2014/data036/{Enum,DungeonEnum}.lua`
from Phase 1 — zero new enum-*value* work needed for those. Reachability was
already wired by Phase 2 (both `MapData:AddLocalMapInfo` and
`DLG_Map_Local_Ranox.lua`'s `DungeonIDList` already carry `DI_RANOX_GATEWAY_*`),
so `DungeonData.lua` registration (Normal/Hard/Expert, ported verbatim from
June2017 including `EXTRA_REQUIRE_DUNGEON_ID = { DUNGEON_ID.DI_RANOX_GEYSER_NORMAL }`
on all three difficulties — GateWay requires Geyser cleared first, continuing
the Phase 5/6/7-established sequential chain; Hero-tier entries skipped per
this plan's default) was the only *DungeonData* registry work this phase
needed.

**The one known engine gap (`TRIGGER_MESH_ACTION_TYPE.TAT_MESH_MOVE` in
`Ranox_GateWay_Stage1_LineMap.lua`), resolved by omission, same technique as
Phases 6/7.** Checked against the underlying C++, not just Lua enum tables:
genuinely absent from both — `KTDXLIB/KTDGLineMap.h:89-94`'s
`TRIGGER_MESH_ACTION_TYPE` stops at `TAT_MESH_CHANGE_SPEED` (no `MOVE`), and,
more severely than Phase 6/7's own two gaps in this same table-family, the
`TRIGGER_DATA_MESH` C++ struct (`:125-145`) **has no `MOVE_POS`-equivalent
member at all** — even a wired-up enum value would have nowhere to land a
target position. Same safe-degrade shape Phase 6/7 already documented for
this table (`KTDGLineMap.cpp:2637`'s `LUA_GET_VALUE_ENUM` defaults a
failed/nil read to `TAT_MESH_NONE`, so leaving it in verbatim would not have
aborted the file's load) — omitted and documented explicitly anyway, per this
port's established convention. Two `TRIGGER_DATA_MESH1` entries removed
(`TRIGGER_ID` 0 and 1, one each), both alongside an intact, untouched
`TRIGGER_DATA_MESH0` (`TAT_MESH_CHANGE_ANIM`) and their `TRIGGER_DATA_LINE0-3`
siblings. Verified via a comments-stripped sorted-multiset diff against
June2017's own decompile that these two removed entries are the *only*
content difference. Net effect: the two `GATEWAY_1STAGE_ROCK_ANI0{1,2}.x`
meshes (see next finding) never get relocated by their triggers; they still
receive the paired `TAT_MESH_CHANGE_ANIM` cue but stay wherever they were
spawned.

**A new, related gap this phase found: `SetIsForceNotCull`, called twice in
`Ranox_GateWay_Stage1.lua` on the same two rock meshes the gapped triggers
above were meant to move.** Extending the method-binding audit to this
phase's own stage files (the same audit class Phase 7 first ran against
scene files, finding `SetTriggerID`/`SetShowObject`) found this name doesn't
exist anywhere in this repo's C++ at all — not just unbound, checked
`X2Lib`/`KTDXLIB` exhaustively. The closest analog,
`CKTDGObject::SetIsCulled(bool)` (`KTDXLIB/KTDGObject.h:357`), is itself never
bound to Lua under any name, and has inverted semantics anyway (marks an
object as culled, not "force **not** culled") — so there's no dormant path to
wire up even by binding an existing method under this name. Both call sites
are the last statement in their block (each immediately after a bound
`SetForceShow(true)`), so both were omitted outright and
`SERV_IRUHADEV_RANOX_PORT`-tagged inline. **Confirmed cosmetically inert given
the trigger gap above**: both affected objects are spawned at `y = -50000`
(see their `SetPos` calls) specifically to be moved into position by the
now-gapped `TAT_MESH_MOVE` trigger — since that never fires either, the
objects stay hidden below the map regardless of whether their
force-render-when-offscreen flag is set. Flagged for the repo/engine
integration phase group as a new item alongside Phase 7's `SetTriggerID`/
`SetShowObject` finding: `SetIsForceNotCull` would need new C++ (there is no
existing method to bind, unlike `SetShowObject`).

**No new method-binding or enum-table gaps found in the 6 unit-definition
files beyond two already-known, recurring gap classes**, both
`SERV_IRUHADEV_RANOX_PORT`-tagged inline, same disposition as their prior
occurrences:

- `GetHostGameUnit` (`SAMETEAM_IGNIA.lua`, 2 occurrences) — the same
  unbound-method gap first found in Phase 4/5. First occurrence
  (`SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW_FRAME_MOVE`) is a "reposition relative to
  the room host" block; omitted, leaving the already-assigned
  `slot5 = slot2:GetPos()` default in place instead of the host-relative
  override. Second occurrence (a compound `and` condition gating a
  retreat-to-ready state transition once every 2 seconds) can't be partially
  evaluated since the unbound call is the first operand — omitted along with
  its whole `if`, falling through unconditionally to the
  `slot2:SetTimerRestart(0)` that follows, the same path June2017's own code
  takes whenever the condition is false.
- `SetMovieDescriptionData` (`EVENT_GATEWAY_BOSS.lua`, 3 occurrences) — the
  same cutscene-subtitle gap first found in Phase 5, recurring in every
  "_BOSS" event-cutscene file since. Each `AnimEventTimer_LUA(...)`
  `if`/`elseif` condition is kept, only the caption-text call in each body is
  dropped (all three become empty branches). The cutscene itself still plays
  via `ANIM_NAME`/`EFFECT_SET_LIST`, both bound.

`MOUNT_MAG_CHARGER.lua`, `MOUNT_MAG_NOM.lua`, `MOUNT_MAG_STONE.lua`, and
`MOUNT_POINT_STAGE_BOSS.lua` came back clean on both audits and were ported
100% verbatim.

**`NPCTemplet.lua` catch-up, continuing Phase 6/7's established practice of
closing this gap for whatever a phase itself spawns.** All 6 of this phase's
new `NPC_UNIT_ID`s needed entries (none existed). Auditing the line's 6
*reused* names from Phases 5/6 turned up 3 more still missing —
`NUI_FIRE_DEVIL_BERSERKER_BOSS`, `NUI_FIRE_DEVIL_FIGHTER`, and
`NUI_FIRE_DEVIL_IGNIA` (all plain/non-suffixed forms; the other 3 reused names
— `NUI_FIRE_DEVIL_BERSERKER`, `NUI_MOUNT_FIREBIRD`, `NUI_MOUNT_GATE` — already
had entries from Phases 5/6). All 9 entries added to
`march2014/data037/NPCTemplet.lua`, copied verbatim from June2017's own file
(`MOTION`/`NAME`/`DESC` text included), confirmed not colliding with any
pre-existing entry. **AshesVillage/BurnedForest's own `NPC_UNIT_ID`s (Phases
3/4) still lack `NPCTemplet.lua` entries at all** — this phase closes the gap
for the 9 names it itself needs, not Phase 6/7's full remaining backlog for
those two lines; a dedicated catch-up pass is still recommended and still
open.

**The `NUI_FIRE_DEVIL_IGNIA_BOSS` anomaly does not recur here**: exhaustive
grep of every file this phase ported found zero references to that name —
GateWay's own boss encounter uses plain `NUI_FIRE_DEVIL_IGNIA` (`STAGE3`'s
`READY_NPC`/`NPC_GROUP`), not the ambiguous `_BOSS` suffix Phase 5 had to
resolve for FinalHall. Nothing to fix.

**STR_ID range flag carried forward, not re-closed**: GateWay's
`DungeonData.lua` entries use `STR_ID_337xx` (dungeon name/description/ending
speech), `STR_ID_38643`/`38644`, and `STR_ID_43788` — the same unverified
`386xx`/`437xx` bands (and, newly, a `337xx` band) Phases 5-7 already flagged
(March2014's own `data036/StringTable.lua` still hasn't been decompiled to
confirm these resolve to real display strings). Cosmetic only if unresolved
(blank text, no crash) — same disposition as its three prior occurrences.

**A fourth registry, missed by every phase since Phase 2, and load-bearing:
`WorldData.lua` (`data040`).** `WORLD_ID.WI_RANOX_GATEWAY_STAGE0`
(and `_STAGE1`/`_STAGE2`/`_STAGEBOSS`) are valid integers from Phase 1's
`Enum.lua`, but nothing bound them to `Ranox_GateWay_Stage0.lua` etc. until
this phase — confirmed by reading `CX2WorldManager::CreateWorld`,
`AddWorldData_LUA`, and `GetWorldData` (`X2Lib/X2WorldManager.cpp`):
`CreateWorld(WORLD_ID)` looks the ID up in `m_WorldDataList` (populated only
by `g_pWorldManager:AddWorldData({worldName=..., dataFileName=...,
worldID=...})` calls in `WorldData.lua`) to get the `dataFileName` it then
opens — with no entry, there's no file to open for that `WORLD_ID` at all.
Same "exists on disk but is never loaded for its ID" gap class Phase 6 first
found for `NPCTemplet.lua`, just one layer up (scene files, not NPC unit
files). **Phase 2 already registered `WI_VILLAGE_RANOX` this exact way** (see
`march2014/data040/WorldData.lua`'s existing entry), so the convention was
established and known — it was simply never applied to any dungeon line's
own stage worlds in Phases 3-7 either. Added this phase's own 4 entries only,
copied verbatim from June2017's own `WorldData.lua` (including its
`dataFileName` spelling, `"Ranox_Gateway_Stage0.lua"` etc. — lowercase "way",
differing from the actual ported scene file's own "GateWay" capitalization;
kept as shipped per "transcribe, don't tidy," expected to resolve via
case-insensitive file lookup but not verified against this repo's own lookup
code). **AshesVillage/BurnedForest/FinalHall/FireTemple/Geyser's own stage
`WORLD_ID`s (Phases 3-7) are equally unregistered in this file and were not
fixed here** — none of those phases' findings mention `WorldData.lua` at all,
meaning **all 5 of those dungeon lines are non-functional in-game as things
stand**, not just missing polish. A dedicated catch-up pass (same shape as
Phase 6/7's still-open `NPCTemplet.lua` catch-up for Phases 3/4) is strongly
recommended and should be treated as higher priority than that one, since
this gap blocks a line from loading at all rather than degrading a monster's
AI.

**Verification performed**: all 21 files (17 new/ported + the four patched
registry files, `DungeonData.lua`/`Enum.lua`/`NPCTemplet.lua`/`WorldData.lua`)
individually
`compile-batch -v mar-2014` clean, copied to a short (`C:\tmp\...`) path first
per Phase 6's own path-length lesson, then re-verified together in one
combined batch invocation. A compile→decompile round-trip was run on every
edited file (`Ranox_GateWay_Stage1_LineMap.lua`, `Ranox_GateWay_Stage1.lua`,
`EVENT_GATEWAY_BOSS.lua`, `SAMETEAM_IGNIA.lua`) and confirmed the only content
differences from June2017's own decompile are the documented omissions above,
plus the same benign trailing-comma-placement and register-renumbering
artifacts this plan's tooling section and Phase 5/6 already documented (e.g.
`SAMETEAM_IGNIA.lua`'s `slot6`/`slot7` shifting after a removed assignment
upstream). A round-trip on an untouched verbatim file
(`Ranox_GateWay_Stage2.lua`) came back byte-identical (sorted). The
enum-table/member audit covered both categories of file this phase touched —
the 6 unit-def files (37 distinct `TABLE.MEMBER` tokens, only the two known
`BUFF_FACTOR_ID` gaps below) and the 11 dungeon/stage/LineMap files separately
(58 distinct tokens, only the one known `TAT_MESH_MOVE` gap) — and confirmed
Phase 6's own `INIT_PHYSIC` false-positive note still applies (local table,
not a missing global, in `EVENT_GATEWAY_BOSS.lua`/`SAMETEAM_IGNIA.lua`). The
method-binding audit (39 distinct `:Method(` calls across the 11 dungeon/
stage/LineMap files, 39 distinct calls across the 6 unit-def files — checked
separately, matching Phase 7's own extension of the audit to scene files, not
just monster files) found the `SetIsForceNotCull` gap (scene files) and the
two recurring gaps (unit-def files) documented above.

**One new `BUFF_FACTOR_ID` member gap, same class Phase 4/5 already
established**: `SAMETEAM_IGNIA.lua` references
`BUFF_FACTOR_ID.BFI_BUFF_IGNIA_ATTACK_SUPERARMOR` and `.BFI_BUFF_IGNIA_HP_MP`,
both missing from March2014's existing `BUFF_FACTOR_ID` table (the table
itself already exists, just not these two members). Added at June2017's own
verbatim values (`530`, `531`), confirmed free within that specific table by
re-checking with a comma-agnostic pattern (`grep -oE "= 53[01],?$"` scoped to
the table's own line range) — the original free-value check used a
comma-anchored pattern that silently can't see a table's *last* entry (no
trailing comma there), which happened to be exactly this table's tail in
Phase 4/5's own additions; the comma-agnostic re-check confirms the only two
matches are this phase's own new lines, so `530`/`531` are genuinely free, but
future phases should use the comma-agnostic form from the start, not repeat
this near-miss (March2014's `Enum.lua` also uses `530`/`531` in `WORLD_ID` and
`NPC_UNIT_ID`, which don't collide with `BUFF_FACTOR_ID`'s own numbering
space — different tables, not a real collision). Same inert-in-this-build
caveat Phase 4 already established for this class of gap: the buff's actual
effect depends on a separate Lua data table (`CX2BuffTemplet::ParsingScript`),
not on this enum value alone, and adding that data table is out of scope here
for the same reason it was out of scope in Phase 4.

**A retroactive completeness note, not a defect**: this phase's own
`NUI_FIRE_DEVIL_BERSERKER_BOSS`/`NUI_FIRE_DEVIL_FIGHTER`/`NUI_FIRE_DEVIL_IGNIA`
`NPCTemplet.lua` additions (above) are entries for unit-def files Phase 5
already ported to disk (`FIRE_DEVIL_BERSERKER_BOSS.lua`, `FIRE_DEVIL_FIGHTER.lua`,
`FIRE_DEVIL_IGNIA.lua`) — this phase only added their *registry* entries, it
did not re-port or re-audit the files themselves (Phase 5's own method-binding
and enum audits for those three already stand).

**A findings-completeness note, not chased further, same disposition as
Phase 6's parallel observation for FireTemple**: `Dungeon_Ranox_Gateway_Normal.lua`
has the duplicate-`NPC_REWARD_RATE`-key pattern Phase 6 flagged, more
extensively — four consecutive `NPC_REWARD_RATE = { ... }` assignments in
`STAGE0.SUB_STAGE0` (and the same four-way pattern repeated in `STAGE1`'s
three sub-stages and `STAGE2`'s two sub-stages), plus two in `STAGE3`. Lua's
last-assignment-wins means only the last of each group is ever live
(`NUI_MOUNT_FIREBIRD` at `0.875` in the four-way groups) — `NUI_MOUNT_MAG_NOM`/
`_STONE`/`_CHARGER`'s own rates are always shadowed. This is June2017's own
data, ported verbatim per "transcribe, don't tidy," not a porting error. As
Phase 6 already noted for its own instance of this pattern: this plan's
tooling-section claim that table-key *reordering* is semantically inert for
named-key table literals doesn't extend to *duplicate* keys, so the
compile→decompile round-trip performed above cannot itself confirm which
duplicate the original bytecode's winner was — only that some winner survives
the round-trip. Not chased further here (verbatim is correct regardless of
which duplicate wins, since the client's own shipped file already has this
property); `Dungeon_Ranox_Gateway_Hard.lua`/`_Expert.lua` are otherwise
identical to `_Normal.lua` except for the one `MONSTER_GRADE`/boss-variant
swap already noted, matching Phase 6's own "Hard and Expert byte-identical"
observation for FireTemple in spirit if not in exact byte count.

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

### Phase 9 findings (executed 2026-09-18)

**All 55 files landed and compile** (`compile-batch -v mar-2014`, individually
and together, `"ok":55,"failed":0`): the 3064-line `Dungeon_Ranox_Secret_Hell.lua`,
14 stage/LineMap files, 34 monster/object unit-definition files, the
fever-gauge UI (`DLG_UI_RANOX_SECRET_FEVER_GAGE.lua`), and 4 patched
registries (`DungeonData.lua`, `Enum.lua`, `NPCTemplet.lua`, `WorldData.lua`)
plus a comment-only revisit of `DLG_Map_Local_Ranox_Icon_NEW.lua`. `.src.lua`
siblings for all 14 stage/LineMap files and the dungeon-def file already
existed from the original bulk-decompile; the 34 unit-def files and the UI
file (outside that 125-file inventory, same as every phase since 4) were
decompiled fresh this phase — 32 cleanly, 2 (see below) only with a scratch
decompiler patch. `DI_RANOX_SECRET_HELL` and the 28 `NPC_UNIT_ID`s Appendix B
named were already in `march2014/data036/{Enum,DungeonEnum}.lua` from Phase
1 — zero new enum-*value* work needed for those. Reachability was already
wired by Phase 2 (`MapData:AddLocalMapInfo` and `DLG_Map_Local_Ranox.lua`'s
`DungeonIDList` both already carry `DI_RANOX_SECRET_HELL`, checked directly
rather than assumed).

**`DI_RANOX_SECRET_COMMON` is not owed anywhere — resolved, not deferred.**
Appendix C.5 flagged this as needing a Phase 9 check. June2017's
`DungeonData.lua` does carry a second `AddDungeonData` block for
`Dungeon_Ranox_Secret_Hell.lua`, but keyed off `dungeonID =
UNUSED_DUNGEON_ID.DI_RANOX_SECRET_COMMON` — a table, `UNUSED_DUNGEON_ID`,
that June2017 introduced as a graveyard for deprecated IDs across many
chapters (dozens of unrelated `DI_*` names sit in it) and that **does not
exist as a concept in March2014's own original `DungeonEnum.lua` at all**
(checked before any Phase 1 edits). Porting that block verbatim would index
a nil global the instant `DungeonData.lua` loads, aborting the load of
*every* dungeon in the game, not just Ranox's — and the block is already
`m_bForbidEnterDungeon = TRUE` in June2017's own data, i.e. dead weight even
there. Omitted outright, tagged and explained inline in
`march2014/data035/DungeonData.lua`; only the real `DUNGEON_ID.DI_RANOX_SECRET_HELL`
block was ported. `41270` is not added to `DungeonEnum.lua`.

**The LineMap question the plan flagged as unresolved is settled by direct
code, not inference.** `Ranox_Secret_Extra_Boss_Stage.lua` and
`Ranox_Secret_Extra_Stage{1,2,3}.lua` each open with an explicit
`g_pWorld:SetLineMap(...)` call reusing another stage's own LineMap
(`Extra_Boss_Stage` → `Ranox_Secret_Boss_Stage_LineMap.lua`; `Extra_Stage1/2/3`
→ `Ranox_Secret_Stage{1,2,3}_LineMap.lua` respectively) — found by grepping
each file directly, not by the March2014 precedent (`Hamel_Secret_Extra_Stage3.lua`/
`Hamel_Secret_Extra_Boss_Stage.lua`, which turn out to do the exact same thing
once checked) that motivated the search. Better than the plan's own framing
of "no LineMap needed" — they need one, and reuse an existing one by name.

**The two named engine gaps, resolved by omission, same technique as Phases
6-8:**

- `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_TYPE` in
  `Ranox_Secret_Stage2_LineMap.lua` (one occurrence, `TRIGGER_ID` 2's
  `TRIGGER_DATA_LINE1`) — the exact same C++ gap Phase 6 already found and
  cited, not re-derived (`KTDXLIB/KTDGLineMap.h:95-104` stops at
  `TAT_LINE_CHANGE_TELEPORT`). Same safe-degrade shape (a nil table-field
  read defaults to `TAT_LINE_NONE`, no load abort) but omitted and documented
  anyway. `TRIGGER_ID` 2's sibling `TRIGGER_DATA_LINE0`
  (`TAT_LINE_CHANGE_TELEPORT`) is untouched — the teleport itself still
  fires, only this line's reclassification-to-`LT_POTAL` cue is lost.
- `CARTOON_TEX_TYPE.CTT_RIM_LIGHT_BLUE` in `Ranox_Secret_Boss_Stage.lua` — a
  **different, more severe gap shape than every prior enum gap in this
  port**, confirmed by checking where the token is actually used: it's not a
  table-field read, it's the sole argument to
  `g_pWorld:SetRimLightTextureIndex(...)`, and that method **does not exist
  anywhere in this repo's C++ at all** (checked `X2Lib`/`KTDXLIB`
  exhaustively, including a bare `"RimLight"` grep — zero hits, not even
  dormant under another name). `CARTOON_TEX_TYPE` itself has no
  `CTT_RIM_LIGHT*` sibling either (its table tops out at `CTT_END = 10`) —
  this is a whole absent rendering feature, not a missing member of an
  existing one. Calling an unbound method aborts the script; the call itself
  was omitted, not just left with a missing argument. Flagged for the
  repo/engine integration phase group as a new item, same severity class as
  Phase 7/8's `SetTriggerID`/`SetIsForceNotCull` (needs new C++, not just a
  binding) — cosmetic effect: the boss stage's blue screen-edge rim-light
  tint never gets set.

**A new method-binding gap class this phase found in scene files, alongside
the one named engine gap**: `SetRimLightTextureIndex` above was found by
extending Phase 7/8's scene-file method-binding audit to this phase's own 14
stage/LineMap files (38 distinct `:Method(` calls checked); it was the only
gap in that set.

**The decompiler itself failed on 2 of the 34 unit-def files — a new class
of problem this port hadn't hit before, worked around, not routed around.**
`SECRET_MAGI_BOSS.lua` and `SECRET_SCENE_DUMMY.lua` both crashed
`elstools.exe decompile-batch` with a truncated
`assert iterator.opcode in (ins.ITERC.opcode, ins.ITERN.opcode)` traceback
(`elstools/vendor/ljd/ljd/ast/builder.py:375`). Diagnosed by patching a
**scratch copy** of the vendored `ljd` tool (never the tracked copy in this
repo) with `--catch_asserts` (which itself needed a one-line Python
3.10+-compat fix to `ljd/lua/writer.py`'s `traceback.format_exception` call
to not crash a second time) to get a best-effort decompile with an inline
error marker naming the exact failing function, and by instrumenting the
assert directly to print the actual opcode: it resolves to `USETV` (opcode
44, an upvalue-set instruction), not an ITER-family opcode at all — so this
isn't a one-line "add the missing opcode" fix, and wasn't pursued as one.
Two functions were affected, given two different dispositions rather than
one blanket treatment:

- `TableCopy()` (in both files, identical) — **reconstructed**, not
  invented, from direct bytecode evidence: the pseudo-asm for this
  prototype shows exactly "1 args, 0 upvalues, 9 slots", then `TNEW` (new
  empty table), `GGET _env["pairs"]`, and a `CALL` passing the sole argument
  to it. Two call sites in the shipped files
  (`g_arrSummonPos = TableCopy(g_arrSummonPosOrigin)` and
  `#TableCopy(slot3)`/`slot5 = TableCopy(slot3)`) both require a returned,
  `#`-countable shallow copy, which is the only behavior consistent with
  that bytecode shape. Implemented as `local copy = {} for k, v in
  pairs(t) do copy[k] = v end return copy`. A repo-wide grep (both client
  trees, `.src.lua` corpus) found no other file defining this exact
  function to copy verbatim instead.
- `SummonMonsterFrameMoveWithMaxCount()` (`SECRET_SCENE_DUMMY.lua` only) —
  **not** reconstructed; a behavior deletion, stated as such in its own
  inline comment. Unlike `TableCopy`, this is real, non-trivial gameplay
  logic (a periodic summon-cap gate, per its name and its one call site's
  arguments — `NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT`, `g_iSummomMagiGapTime`,
  `g_iSummomMagiMaxNum`), and guessing its body from the name alone would be
  inventing dungeon behavior, which this port does not do. Left as a no-op
  with the 5 parameters its call site implies, so the call doesn't abort the
  whole script with "attempt to call a nil value" — same disposition
  Phases 6-8 gave an unbound C++ method's call site, extended here to a
  whole function body for the first time. Effect: the periodic magi-minion
  summon wave this gate controlled never fires.

**A decompiler *mislabeling*, not a crash, found and corrected while auditing
`SECRET_SCENE_DUMMY.lua`**: `SECRET_SCENE_DUMMY_DYING_READY_END_FUNC`
decompiled as a bare `slot2:GetNowHP(0)` — a zero-argument getter
(confirmed real and already used elsewhere in this same file as
`slot2:GetNowHP() > 0`) called with a phantom argument and its return
discarded, i.e. a statement that provably does nothing, as the sole content
of a "_READY_END_FUNC" handler for a dying-state sequence. Corrected to
`SetNowHP(0)` — the verbatim call this same file's own
`SECRET_SCENE_DUMMY_SUMMON_TYPE3` branch already uses for the identical
"finish killing this NPC" purpose, both real, distinct, bound methods
(`X2Lib/X2Main.cpp`'s `class_def<CX2GameUnit>` block). Tagged inline as a
tooling-artifact correction, not a data "improvement."

**A major finding this phase's own audit surfaced, not named anywhere in the
plan: 7 more `NPC_UNIT_ID`s outside Appendix B's 28, each with its own real
unit-def file, found only by reading `SECRET_SCENE_DUMMY.lua`'s own script
content (a "prop" NPC's Lua, not a dungeon-def or stage file — outside the
125-file inventory Appendix B was built from).** `NUI_SECRET_SUMMON_{MAG_NOM,
FIGHTER,FIREBIRD,SNIKER,BERSERKER}` (June2017 values `2227`/`2229`/`2232`/
`2231`/`2230`) and `NUI_SECRET_MAGI_OBJECT`/`NUI_SECRET_MAGI_OBJECT_GROUB`
(`2249`/`2257`, the latter carrying June2017's own `"GROUB"` typo, kept
verbatim same as `BURNDED`/`GAYSER`) all resolved to real, distinct
`LUA_FILE_NAME`s in June2017's `NPCTemplet.lua` (`SECRET_SUMMON_MAG_NOM.lua`
etc., `SECRET_MAGI_OBJECT.lua`, `SECRET_MAGI_OBJECT_GROUB.lua` — not
reused copies of the "plain" `SECRET_MAG_NOM.lua`/etc. files), decompiled,
audited, and ported the same as the other 27. All 7 values confirmed free in
`march2014/data036/Enum.lua`'s `NPC_UNIT_ID` table and added there (in
alphabetical position, matching this table's own convention) — **this is
new enum-value work Phase 1 could not have scoped**, unlike every prior
phase's "already in Enum.lua from Phase 1" finding.

**Two more `BUFF_TEMPLET_ID`/`BUFF_FACTOR_ID` gaps, same class Phase 4/5/8
already established, found across the enum-table audit of all 34 unit-def
files**: `BFI_BUFF_RANOX_SECRET_FEVER` (`BUFF_FACTOR_ID`, June2017 value
`817`) and `BTI_BUFF_RANOX_SECRET_FEVER`/`BTI_DEBUFF_RANOX_MAGI_BOSS_SPEED_DOWN`
(`BUFF_TEMPLET_ID`, `792`/`796`) — all three confirmed free within their own
tables and appended at each table's own tail, matching Phase 4/5/8's own
convention exactly (a fresh comma-agnostic check per Phase 8's own
established lesson, not the comma-anchored one Phase 8 flagged as a
near-miss).

**14 method-binding gaps found across the full 34-file audit, most
concentrated in the fever-gauge display logic — the largest single cluster
of unbound-method gaps any phase has hit, because an entire UI subsystem is
missing, not just individual calls.** `CreateSimpleDLG`/`GetSimpleDLG`/
`SetShow_LUA`/`SetNumberInGageStatic_LUA`/`SetNumberInNumberStatic_LUA` do
not exist anywhere in this repo's C++ (a bare `"SimpleDLG"` grep across both
client trees' C++ returns zero hits) — this whole "NPC-script-opens-a-UI-dialog-
and-drives-it" mechanism is absent from this build, not unbound under
another name. Every call site gated on it was omitted (11 sites across
`SECRET_SCENE_DUMMY.lua`, `SECRET_MAGI_OBJECT.lua`); most are pure
UI-display updates with no gameplay consequence, but **one is not**: the
"count to 5" fever-trigger check in `SECRET_SCENE_DUMMY_SUMMON_TYPE3_FRAME_MOVE`
reads its live counter directly off the dialog's own number-static widget
(`slot3:GetNumber_LUA()`) with no separate state variable anywhere in this
dungeon's data — so with no dialog, there is no substitute counter to gate
on without inventing a new mechanism, which this port does not do. Omitting
that whole block means the magi-boss group-object spawn
(`CreateNPCReq_LUA(NUI_SECRET_MAGI_OBJECT_GROUB, ...)`) and the transition
into the fever buff sequence (`SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY`
and everything downstream of it) **never fire** — a substantial,
prominently-flagged behavior loss, not a cosmetic one, and named as a
repo/engine integration candidate (either a substitute counter or resolving
the SimpleDLG gap itself would restore it). The remaining 3 gap classes
recur from prior phases, same disposition each time: `SetActiveTrigger_LUA`
(8 sites across `SECRET_SCENE_DUMMY.lua`, `SECRET_MAGI_BOSS.lua`,
`SECRET_MAGI_SCENE_{A,B,C}.lua` — each just un-arms one LineMap trigger
index), `GetNowStateName` (7 sites — 3 in `SECRET_SCENE_DUMMY.lua`'s
SCENE_A/B/C summon logic, 1 in its `CF_SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF`
early-exit condition kept safe by the buff's own `STATE_TIME_OVER` timer, 5
in the newly-found `SECRET_SUMMON_*.lua` files' own fever-buff-application
checks), `GetHostGameUnit` (3 sites, the same target-loss fallback Phase 4/5
established, recurring in `SECRET_SNIKER.lua`/`SECRET_CRUEL_SNIKER.lua`/
`SECRET_SUMMON_SNIKER.lua`), `FadeOriginWorldColor_LUA` (2 sites, world-tint
fade cosmetics), `CrashAndColorChange_LUA`/`EraseAllBuffAndExtraDamage`/
`GetDyingStart` (1 site each), and `SetForceInvincible`/`SetMovieDescriptionData`
(3 and 1 sites respectively, the latter the same cutscene-subtitle gap every
`_BOSS`/cutscene file has carried since Phase 5).

**Verification performed**: all 55 files (34 unit-def + 14 stage/LineMap + 1
dungeon-def + 1 UI + 4 patched registries, plus the comment-only
`DLG_Map_Local_Ranox_Icon_NEW.lua` revisit) `compile-batch -v mar-2014` clean
individually and together (`"ok":55,"failed":0`), from a short `C:\tmp\...`
path per Phase 6's own lesson. A round-trip (compile → decompile) on the two
heaviest-edited files, `SECRET_SCENE_DUMMY.lua` and `SECRET_MAGI_BOSS.lua`,
both succeeded clean, confirming the reconstructed `TableCopy`, the
`SummonMonsterFrameMoveWithMaxCount` stub, and the `GetNowHP`→`SetNowHP`
correction all produce valid mar-2014 bytecode. The enum-table/member audit
covered the dungeon-def file (52 distinct tokens, zero gaps beyond the two
already known), the 14 stage/LineMap files (31 distinct tokens, same two),
and the 34 unit-def files (97 distinct tokens, the `BUFF_FACTOR_ID`/
`BUFF_TEMPLET_ID` gaps above plus the already-known `INIT_PHYSIC` local-table
false positive Phase 6 first flagged). The method-binding audit covered the
same three file categories separately (38/56/96 distinct `:Method(` calls
respectively, cross-checked against both a ~1300-name `class_def<...>`
scrape and, for every candidate gap, a direct exhaustive
`grep -rc "\"MethodName\""` across `X2Lib`/`KTDXLIB` to rule out the
scrape's own known multi-line-call undercount risk).

**Two registries closed beyond this phase's own scope, per this session's
own recommendation to stop re-flagging the same backlog:**

- **`WorldData.lua` closed in full for every remaining dungeon-line stage
  world, not just this phase's own 9.** Phase 8 first found and fixed this
  gap for GateWay and flagged that AshesVillage/BurnedForest/FinalHall/
  FireTemple/Geyser (Phases 3-7, 22 `WORLD_ID`s total) were equally
  unregistered and left unfixed for a future catch-up. This is the last
  dungeon-line phase in the plan and no later phase is scoped to catch it,
  so all 22 were added here alongside this phase's own 9 Secret entries (31
  total, one tagged block), transcribed verbatim from June2017's own
  `WorldData.lua` including its casing mismatches against the actual ported
  filenames (`Ranox_Geyser_Stage0.lua` vs. the Phase-7-ported
  `Ranox_Geyser_stage0.lua`; `Ranox_Gateway_Stage*.lua` vs. the
  Phase-8-ported `Ranox_GateWay_Stage*.lua`) — kept as shipped per
  "transcribe, don't tidy," same disposition Phase 8 already gave its own
  instance of this. Field/Spa/PVP `WORLD_ID`s were deliberately **not**
  included — those belong to Phases 10/11/12, not yet ported.
- **`NPCTemplet.lua` closed only for this phase's own 35 `NPC_UNIT_ID`s**
  (the 28 from Appendix B plus the 7 newly-discovered names above) — **not**
  for Phases 3-5's still-open backlog (AshesVillage/BurnedForest/FinalHall's
  own `NPC_UNIT_ID`s), which remains unrelated to this dungeon and is still
  open, same as every prior phase's own note on it. Unlike `WorldData.lua`
  (which blocks a line from loading at all), a missing `NPCTemplet.lua`
  entry only degrades one NPC's spawn behavior, so this phase closes its own
  gap without taking on that other backlog.

**A third audit axis, run after the enum-table and method-binding ones,
found no gaps but one self-inflicted regression, since corrected.** Every
prior phase's method-binding audit only ever checked `slotN:Method(`/
`g_pXxx:Method(` calls; nothing had checked *bare* global-function calls
(no `:`/`.` before the identifier) — the class Phase 2's own findings
already named as higher-severity than an enum gap ("a missing definition
would abort the whole script"). Run across all 34 unit-def files and 14
stage/LineMap files (subtracting each file's own `function NAME(`
definitions and Lua/D3DX builtins): one true bare call,
`GetDistance_LUA(...)` in `SECRET_MAGI_BOSS.lua` — confirmed bound as a
**global** function (`lua_tinker::def(L, "GetDistance_LUA", &GetDistance_LUA)`,
`KTDXLIB/KTDXApp.cpp:1738`, distinct from the `class_def<...>` bindings every
other audit this port has run checks), not a gap. Everything else that
matched was this port's own inline comments mentioning a method name in
prose without a `slotN:` prefix — a false-positive class worth remembering
for whoever runs this audit next. The one real thing this axis caught:
`SECRET_MAGI_SCENE_C.lua`'s own `GET_ALL_TRIGGER_INDEX()` call (a real,
file-local function, not a gap) was in a loop whose earlier omission
(above, the `SetActiveTrigger_LUA` fix) had deleted the whole loop rather
than matching `SECRET_MAGI_SCENE_B.lua`'s sibling disposition for the exact
same gap (keep the `GET_ALL_TRIGGER_INDEX()` call and its loop, omit only
the unbound call inside) — inconsistent treatment of the same gap across
two sibling files, corrected to match B's shape. Re-verified
`compile-batch -v mar-2014` clean after the fix.

**One completeness note on the dungeon-def file itself**: `Dungeon_Ranox_Secret_Hell.lua`
has 18 `NPC_REWARD_RATE = {...}` table-constructor assignments, checked
individually for the duplicate-key-in-one-table pattern Phase 6/8 both found
in other lines' `DungeonData`-adjacent files — none found here (each of the
18 sits in its own distinct `SUB_STAGE`, roughly one per sub-stage across
this file's 9 stage entries, not stacked as sibling duplicate keys). Nothing
to flag.

**Not chased further, matching this port's own established scope
boundary**: server-side monster stat/drop tables for this dungeon's real
monsters (as opposed to props like `NUI_SECRET_STAGE0_GATE`/`NUI_SECRET_STAGE1_CHAIN1/2`/
`NUI_SECRET_LAVA_DUMMY`/`NUI_SECRET_SCENE_DUMMY`/`NUI_SECRET_SCENE_1_0/1_1`)
are out of this port's scope for the same reason Phase 4 first established —
named as a Phase 13 dependency, not invented here.

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
3. **Resolve the two `CX2WorldObjectSkinMesh` method gaps** Phase 7 found in
   `Ranox_Geyser_stage0.lua`/`Ranox_Geyser_Stage1.lua` — `SetTriggerID` and
   `SetShowObject`. These have *different* fixes, confirm both individually
   rather than assuming they're the same gap:
   - `SetTriggerID` doesn't exist anywhere in this repo's C++ — needs a new
     member function plus a `lua_tinker::class_def<CX2WorldObjectSkinMesh>`
     binding, same shape as the 8 enum gaps' "needs real engine work" cases.
   - `SetShowObject` already exists in C++
     (`CKTDGObject::SetShowObject(bool,bool)`, `KTDXLIB/KTDGObject.h:305`,
     inherited by `CX2WorldObjectSkinMesh` via the already-registered
     `class_inh<CX2WorldObjectSkinMesh, CX2WorldObject>` /
     `class_inh<CX2WorldObject, CKTDGObject>` chain) — it's missing only a
     `lua_tinker::class_def<CKTDGObject>( L, "SetShowObject", &CKTDGObject::SetShowObject )`
     binding line (`KTDXLIB/KTDXApp.cpp`'s existing 5-entry
     `class_def<CKTDGObject>` block), no new C++ logic needed. Cheaper than
     `SetTriggerID` — verify this before assuming both need the same amount
     of work.

   A different gap class from the 8-gap table either way (method, not enum),
   so treat as separate checklist items, not folded into item 2. Also worth a
   quick check whether other, not-yet-ported dungeon lines' stage files call
   the same or similarly-absent `CX2WorldObjectSkinMesh` methods, since no
   phase before Phase 7 audited scene-file method calls at all (only
   monster/object unit-def files were checked) — Phases 3-6's own stage files
   were never re-checked against this audit either.
3b. **Resolve `SetIsForceNotCull`**, the same gap class as item 3 but found by
   Phase 8 in `Ranox_GateWay_Stage1.lua`. Unlike `SetShowObject`, there is no
   existing C++ method to bind under this name at all — checked
   `X2Lib`/`KTDXLIB` exhaustively; the closest analog,
   `CKTDGObject::SetIsCulled(bool)` (`KTDXLIB/KTDGObject.h:357`), is itself
   unbound and has inverted semantics ("force culled," not "force **not**
   culled"). Needs new C++ (a member function plus a binding), same shape as
   `SetTriggerID` above, not the cheaper `SetShowObject` shape.
3c. **Resolve `SetRimLightTextureIndex`**, the same gap class as items 3/3b
   but found by Phase 9 in `Ranox_Secret_Boss_Stage.lua`. No existing C++
   method to bind under this name either — checked `X2Lib`/`KTDXLIB`
   exhaustively, including a bare `"RimLight"` grep (zero hits, not even
   dormant under another name). `CARTOON_TEX_TYPE` also has no
   `CTT_RIM_LIGHT*` sibling member (tops out at `CTT_END = 10`) — this is a
   whole absent rendering feature, not just an unbound method, so resolving
   it means adding both a C++ method and the enum member(s) it would take,
   not a one-line binding fix.
3d. **Add `SetForceInvincible`/`EraseAllBuffAndExtraDamage` bindings and,
   separately, consider a substitute for the "SimpleDLG" NPC-to-UI-dialog
   mechanism.** Phase 9 found `SetForceInvincible`/`EraseAllBuffAndExtraDamage`
   (`SECRET_MAGI_BOSS.lua`) unbound, same class as items 3/3b/3c — checked
   exhaustively, no C++-side method exists to bind under either name. More
   significant: `CreateSimpleDLG`/
   `GetSimpleDLG`/`SetShow_LUA`/`SetNumberInGageStatic_LUA`/
   `SetNumberInNumberStatic_LUA` are entirely absent (a bare `"SimpleDLG"`
   grep across both client trees' C++ returns zero hits) — this is a whole
   subsystem (an NPC script opening and driving its own UI dialog), not a
   single method, and Secret Hell's fever-buff trigger sequence depends on
   it for its live counter with no substitute state variable anywhere in
   the ported data (see Phase 9's own findings). Resolving this for real
   gameplay (not just cosmetic UI) means either binding the whole SimpleDLG
   call surface or adding a substitute counter mechanism — bigger scope
   than items 3/3b/3c, flagged separately for that reason.
4. **Offline-emulator / world-map hookup**, since this build's
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

**Correction, Phase 9 (2026-09-18): this bucket is incomplete by 7 names.**
This 28-name list was extracted from Appendix A's 125-file inventory
(dungeon-def/stage/LineMap files), which by construction excludes
monster/object unit-definition files — the same blind spot Phase 6 first
flagged for `BUFF_FACTOR_ID`. Reading `SECRET_SCENE_DUMMY.lua`'s own script
content (Phase 9) surfaced 7 more, each with its own real unit-def file, not
a reused copy of a plain-named sibling: `NUI_SECRET_SUMMON_MAG_NOM`,
`NUI_SECRET_SUMMON_FIGHTER`, `NUI_SECRET_SUMMON_FIREBIRD`,
`NUI_SECRET_SUMMON_SNIKER`, `NUI_SECRET_SUMMON_BERSERKER`,
`NUI_SECRET_MAGI_OBJECT`, `NUI_SECRET_MAGI_OBJECT_GROUB` (the last carrying
June2017's own `"GROUB"` typo, kept verbatim). All 7 ported and registered
in Phase 9's own findings — not a new backlog.

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
