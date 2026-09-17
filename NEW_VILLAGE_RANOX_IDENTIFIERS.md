# Village RANOX — identifier table (Phase 0 output)

Decided once here per `NEW_VILLAGE_PLAN.md`'s Phase 0 exit criteria. `<NAME> = RANOX`
(the game's own internal codename for what's marketed as "Lanox" — same
codename/display-name split as `Peita`/Feita elsewhere in this tree).

Numbers shown are as observed in the newer client's `Enum.src.lua` dump (a
runtime `NAME = number` dump the user provided, hash-ordered — **not** an
authored source file and **not** something that gets merged anywhere; `Enum.h`
is C++). Treat every number below as **advisory only** for `VILLAGE_MAP_ID`/
`LOCAL_MAP_ID` (they auto-increment in this tree — see plan's *Why this is
safe*) and **collision-check-before-reuse** for `WORLD_ID`/`DUNGEON_ID`/
`NPC_UNIT_ID` (explicit literals in both builds, but this tree's free numbers
won't line up with the newer build's). The **names** are authoritative —
transcribe them verbatim, typos included (`BURNDEDFOREST`, `ASHESVILLAG_GATE`,
`ASHESVILLAG_SWITCH_GATE`, `GAYSER_BOSS`, `REGIEON_POINT`, `INCHANT`) — this
repo's "transcribe, don't tidy" rule, since Lua and the wire both reference
these exact strings.

## Village / local map / dungeon gate & lounge

| Symbol | Newer-build number | Status |
|---|---|---|
| `LOCAL_MAP_ID.LMI_RANOX` | 10008 | needed — insert after `LMI_SANDER` |
| `VILLAGE_MAP_ID.VMI_RANOX` | 20008 | needed — the village itself |
| `VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_RANOX` | 30008 | needed — **confirmed present** in the newer build |
| `VILLAGE_MAP_ID.VMI_DUNGEON_GATE_RANOX` | — | **confirmed absent** — grepped the full `Enum.src.lua` dump, no hit. Sander (the prior newest village) also has no gate map. Ranox does not use one. |
| `VILLAGE_MAP_ID.VMI_SPA_RANOX` | 1501 | needed — new sub-location, not anticipated by the base plan (a bath/hangout facility, real content in live Lanox). Own `WORLD_ID` (`WI_SPA_RANOX`) + `HI_SPA_SIGN_RANOX`/`HI_SPA_INNER_NPC_RANOX` house-interior NPCs. |

## Battle fields — IMPORTANT correction to Plan Phase 1 Step 4

`BATTLE_FIELD_ID`'s `BFI_*` entries in this tree's own `Enum.h` are **aliases**,
not independent numbers: `BFI_RUBEN_FIELD_01 = VMI_BATTLE_FIELD_RUBEN_FIELD_01`
(`Enum.h:292`), and a runtime range check
(`Enum.h:501`: `VMI_BATTLE_FIELD_RUBEN_FIELD_01 <= id < VMI_BATTLE_FIELD_END`)
identifies a valid battlefield ID by whether it falls inside the
`VILLAGE_MAP_ID` enum's own `40000+`/`1000+` sub-blocks. Phase 1 Step 4 as
written ("`BFI_<NAME>_<N>` per field, any unused explicit number before
`BFI_END`") would give Ranox's fields a number **outside** that range and the
runtime check would silently fail to recognize them. Correct sequence:

1. Add `VMI_BATTLE_FIELD_RANOX_FIELD_01`..`04` and `VMI_BATTLE_FIELD_RANOX_REST_00`
   to `VILLAGE_MAP_ID`, auto-incrementing right before `VMI_BATTLE_FIELD_END`
   (and the `REST_00` one before `VMI_BATTLE_FIELD_REST_END`).
2. Alias `BFI_RANOX_FIELD_01`..`04` = those same `VMI_BATTLE_FIELD_RANOX_FIELD_01`..`04`
   values in `BATTLE_FIELD_ID`, matching every existing village's pattern.

| Symbol | Newer-build number |
|---|---|
| `VMI_BATTLE_FIELD_RANOX_FIELD_01` | 40029 |
| `VMI_BATTLE_FIELD_RANOX_FIELD_02` | 40030 |
| `VMI_BATTLE_FIELD_RANOX_FIELD_03` | 40031 |
| `VMI_BATTLE_FIELD_RANOX_FIELD_04` | 40032 |
| `VMI_BATTLE_FIELD_RANOX_REST_00` | 1007 |

Note the off-by-one against `WORLD_ID`: fields are `WI_BATTLE_FIELD_RANOX_FIELD_00..03`
(0-based) but `VMI_BATTLE_FIELD_RANOX_FIELD_01..04` (1-based) — confirmed by
`BattleFieldData.src.lua:1433`, which binds `WORLD_ID.WI_BATTLE_FIELD_RANOX_FIELD_00`
to `BATTLE_FIELD_ID = VMI_BATTLE_FIELD_RANOX_FIELD_01` in the same entry.

## WORLD_ID (append, all guarded by the flag) — 43 entries

```
WI_VILLAGE_RANOX = 582
WI_SPA_RANOX = 726
WI_RANOX_GATEWAY_STAGE0 = 597
WI_RANOX_GATEWAY_STAGE1 = 598
WI_RANOX_GATEWAY_STAGE2 = 599
WI_RANOX_GATEWAY_STAGEBOSS = 600
WI_RANOX_GATEWAY_PVPMAP = 751          -- PvP variant, not covered by any file in hand yet; defer/ask
WI_RANOX_ASHESVILLAGE_STAGE0 = 587
WI_RANOX_ASHESVILLAGE_STAGE1 = 588
WI_RANOX_ASHESVILLAGE_STAGE2 = 589
WI_RANOX_ASHESVILLAGE_STAGE3 = 590
WI_RANOX_ASHESVILLAGE_STAGEBOSS = 591
WI_RANOX_BURNDEDFOREST_STAGE0 = 583
WI_RANOX_BURNDEDFOREST_STAGE1 = 584
WI_RANOX_BURNDEDFOREST_STAGE2 = 585
WI_RANOX_BURNDEDFOREST_STAGEBOSS = 586
WI_RANOX_FIRETEMPLE_STAGE0 = 601
WI_RANOX_FIRETEMPLE_STAGE1 = 602
WI_RANOX_FIRETEMPLE_STAGE2 = 603
WI_RANOX_FIRETEMPLE_STAGEBOSS = 604
WI_RANOX_FINALHALL_STAGE0 = 609
WI_RANOX_FINALHALL_STAGE1 = 610
WI_RANOX_FINALHALL_STAGE2 = 611
WI_RANOX_FINALHALL_STAGE3 = 612
WI_RANOX_FINALHALL_STAGEBOSS = 613
WI_RANOX_GEYSER_STAGE0 = 593
WI_RANOX_GEYSER_STAGE1 = 594
WI_RANOX_GEYSER_STAGE2 = 595
WI_RANOX_GEYSER_STAGEBOSS = 596
WI_RANOX_SECRET_STAGE0 = 676
WI_RANOX_SECRET_STAGE1 = 677
WI_RANOX_SECRET_STAGE2 = 678
WI_RANOX_SECRET_STAGE3 = 679
WI_RANOX_SECRET_BOSS_STAGE = 680
WI_RANOX_SECRET_EXTRA_STAGE1 = 681
WI_RANOX_SECRET_EXTRA_STAGE2 = 682
WI_RANOX_SECRET_EXTRA_STAGE3 = 683
WI_RANOX_SECRET_EXTRA_BOSS_STAGE = 684
WI_BATTLE_FIELD_RANOX_FIELD_00 = 592
WI_BATTLE_FIELD_RANOX_FIELD_01 = 605
WI_BATTLE_FIELD_RANOX_FIELD_02 = 607
WI_BATTLE_FIELD_RANOX_FIELD_03 = 608
WI_BATTLE_FIELD_RANOX_FIELD_REST_00 = 606
```
(That's 41 world/mesh scripts for dungeons+village+spa+fields, plus the PvP
variant = 42, all needing a `WI_` symbol; the Plan's parenthetical in Phase 0
item 1 — "confirm whether dungeons use distinct per-stage world maps" — is
now settled: **yes**, every dungeon stage is its own `WORLD_ID`.)

## DUNGEON_ID — 44 entries (7 dungeon lines × difficulty)

All explicit literals in `DungeonData.src.lua`; every `HERO_*` variant's
`dataFileName` points at an **already-in-hand** stage script (verified —
`HERO_NORMAL` reuses the `Expert` file, `HERO_HARD`/`HERO_EXPERT` reuse their
own tier, `HERO_HELL` and the Secret line reuse `Dungeon_Ranox_Secret_Hell.lua`).
**No additional per-instance stage scripts are needed beyond the 19 already
provided.** One oddity to carry into Phase 1: `DI_RANOX_SECRET_COMMON` is
declared under `UNUSED_DUNGEON_ID`, not `DUNGEON_ID`, in the source data —
check whether this tree even has an `UNUSED_DUNGEON_ID` enum before assuming
it needs a home.

```
DI_RANOX_GATEWAY_NORMAL / HARD / EXPERT
DI_RANOX_GATEWAY_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_ASHESVILLAGE_NORMAL / HARD / EXPERT
DI_RANOX_ASHESVILLAGE_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_BURNDEDFOREST_NORMAL / HARD / EXPERT
DI_RANOX_BURNDEDFOREST_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_FIRETEMPLE_NORMAL / HARD / EXPERT
DI_RANOX_FIRETEMPLE_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_FINALHALL_NORMAL / HARD / EXPERT
DI_RANOX_FINALHALL_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_GEYSER_NORMAL / HARD / EXPERT
DI_RANOX_GEYSER_HERO_NORMAL / HERO_HARD / HERO_EXPERT / HERO_HELL
DI_RANOX_SECRET_HELL
UNUSED_DUNGEON_ID.DI_RANOX_SECRET_COMMON   -- different enum namespace, confirm it exists here
```

## NPC_UNIT_ID — 131 references, 129 genuinely new, 2 already exist

`NUI_CHEST` and `NUI_GATE_OF_NIGHTMARE` already exist in this tree's
`X2UnitManager.h` (confirmed by grep) — they're reused generic objects, not
new Ranox content. Everything else below is new and absent from this tree
(confirmed). One anomaly: `NUI_FIRE_DEVIL_IGNIA_BOSS` (bare, no `_FINALHALL`
suffix) is referenced in three `Dungeon_Ranox_FinalHall_*` `READY_NPC` lists
but has **no `NPCTemplet.lua` row** and **no entry in the newer build's own
`Enum.src.lua` dump either** — it isn't a templet gap, the symbol itself
doesn't exist anywhere in hand. Given `NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL`
does exist fully templated right next to it, this reads as a stale/typo'd
`READY_NPC` reference in the source data rather than version skew (the other
130 cross-checked clean) — flag it to the user rather than silently dropping
or "fixing" it.

Cross-checked: of the 129 new names, all but that one already have a full
`AddNPCTemplet` row in `NPCTemplet.src.lua` (verified by grep, not assumed).
`NPCStat.lua` is still the real gap — no HP/ATK/DEF source has been provided
for any of them yet.

```
NUI_EVENT_FINALHALL_BOSS, NUI_EVENT_FINALHALL_BOSS2, NUI_EVENT_FINALHALL_BOSS3
NUI_EVENT_GATEWAY_BOSS, NUI_EVENT_GAYSER_BOSS
NUI_FIRETEMPLE_GATE, NUI_FIRETEMPLE_REGIEON_POINT
NUI_FIRE_DEVIL_BERSERKER, NUI_FIRE_DEVIL_BERSERKER_BOSS, NUI_FIRE_DEVIL_BERSERKER_BOSS_FINALHALL
NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE, NUI_FIRE_DEVIL_BERSERKER_INCHANT
NUI_FIRE_DEVIL_CHAIN_BALL, NUI_FIRE_DEVIL_CHAIN_IPLITAN, NUI_FIRE_DEVIL_CHAIN_STONE
NUI_FIRE_DEVIL_FIGHTER, NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE, NUI_FIRE_DEVIL_FIGHTER_INCHANT
NUI_FIRE_DEVIL_IGNIA, NUI_FIRE_DEVIL_IGNIA_BOSS (*anomaly, see above*), NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL
NUI_FIRE_DEVIL_IN, NUI_FIRE_DEVIL_IN_FINALHALL
NUI_FIRE_DEVIL_JIN, NUI_FIRE_DEVIL_JIN_FINALHALL
NUI_FIRE_DEVIL_SKA, NUI_FIRE_DEVIL_SNIKER, NUI_FIRE_DEVIL_SNIKER_INCHANT
NUI_GEYSER_ALUMINUM, NUI_GEYSER_GATE, NUI_GEYSER_LITTLEWORKER
NUI_GEYSER_MULLOCK, NUI_GEYSER_MULLOCK_CALL, NUI_GEYSER_MULMAI
NUI_GEYSER_MULSAYRENA, NUI_GEYSER_MULSAYRENA_CALL, NUI_GEYSER_RUNNER
NUI_GEYSER_SCIENCE, NUI_GEYSER_WORKER
NUI_LOW_FINALHALL_BERSERKER_FD, NUI_LOW_FINALHALL_FIGHTER_FD, NUI_LOW_FINALHALL_FIREBIRD_FD
NUI_LOW_FINALHALL_IGNIA_BOSS, NUI_LOW_FINALHALL_SNIKER_FD, NUI_LOW_FINALHALL_STONE_FD
NUI_LOW_GEYSER_ALUMINUM_FD_BOSS, NUI_LOW_GEYSER_LITTLEWORKER_FD, NUI_LOW_GEYSER_MULLOCK_CALL_FD
NUI_LOW_GEYSER_MULLOCK_FD, NUI_LOW_GEYSER_MULMAI_FD, NUI_LOW_GEYSER_RUNNER_FD
NUI_LOW_GEYSER_SCIENCE_FD, NUI_LOW_GEYSER_WORKER_FD
NUI_LOW_MAG_CHARGER_FD, NUI_LOW_MAG_NOM_FD, NUI_LOW_MAG_STONE_FD
NUI_LOW_MOUNT_BERSERKER_BOSS, NUI_LOW_MOUNT_BERSERKER_FD, NUI_LOW_MOUNT_CHARGER_FD
NUI_LOW_MOUNT_FIGHTER_FD, NUI_LOW_MOUNT_FIREBIRD_FD, NUI_LOW_MOUNT_NOM_FD, NUI_LOW_MOUNT_STONE_FD
NUI_LOW_RAGE_BABYFIG_FD, NUI_LOW_RAGE_DRYAD_FD, NUI_LOW_RAGE_FATHERFIG_FD_BOSS
NUI_LOW_SMITH_ALUMINUM_FD_BOSS, NUI_LOW_SMITH_LITTLEWORKER_FD, NUI_LOW_SMITH_MACHINE_FD
NUI_LOW_SMITH_RUNNER_FD, NUI_LOW_SMITH_SCIENCE_FD, NUI_LOW_SMITH_WORKER_FD
NUI_MAG_CHARGER, NUI_MAG_NOM, NUI_MAG_STONE
NUI_MOUNT_FIREBIRD, NUI_MOUNT_FIREBIRD_FIRETEMPLE, NUI_MOUNT_GATE
NUI_MOUNT_MAG_CHARGER, NUI_MOUNT_MAG_NOM, NUI_MOUNT_MAG_STONE, NUI_MOUNT_POINT_STAGE_BOSS
NUI_RAGE_BABYFIG, NUI_RAGE_DRYAD, NUI_RAGE_DRYADKENTA, NUI_RAGE_MOTHERFIG
NUI_RANOX_ASHESVILLAGE_GATE, NUI_RANOX_ASHESVILLAG_SWITCH_GATE, NUI_RANOX_BURNDEDFOREST_GATE
NUI_SAMETEAM_IGNIA
NUI_SECRET_BERSERKER, NUI_SECRET_BOSS_LAVA
NUI_SECRET_CRUEL_BERSERKER, NUI_SECRET_CRUEL_FIGHTER, NUI_SECRET_CRUEL_MAG_CHARGER
NUI_SECRET_CRUEL_MAG_NOM, NUI_SECRET_CRUEL_SNIKER
NUI_SECRET_EXTRA_MAGI_BOSS, NUI_SECRET_FIGHTER, NUI_SECRET_FIREBIRD
NUI_SECRET_IN, NUI_SECRET_JIN, NUI_SECRET_LAVA_DUMMY
NUI_SECRET_MAGI_BOSS, NUI_SECRET_MAGI_SCENE_A, NUI_SECRET_MAGI_SCENE_B, NUI_SECRET_MAGI_SCENE_C
NUI_SECRET_MAGI_SUMMON, NUI_SECRET_MAG_CHARGER, NUI_SECRET_MAG_NOM
NUI_SECRET_SCENE_1_0, NUI_SECRET_SCENE_1_1, NUI_SECRET_SCENE_DUMMY, NUI_SECRET_SNIKER
NUI_SECRET_STAGE0_CHAIN_OBJECT, NUI_SECRET_STAGE0_GATE, NUI_SECRET_STAGE1_CHAIN1, NUI_SECRET_STAGE1_CHAIN2
NUI_SMITH_ALUMINUM, NUI_SMITH_CART, NUI_SMITH_LITTLEWORKER, NUI_SMITH_MACHINE
NUI_SMITH_REGIEON_POINT, NUI_SMITH_RUNNER, NUI_SMITH_SCIENCE, NUI_SMITH_STAIN, NUI_SMITH_WORKER
NUI_TRAP_FIRE_BIG, NUI_TRAP_FIRE_SMALL
```
(reused, do not redefine: `NUI_CHEST`, `NUI_GATE_OF_NIGHTMARE`)

## File-category status (as of this Phase 0 pass)

| Category | Status |
|---|---|
| Village hub world script + line-map | done (`Ranox_Village.src.lua` + line-map; 4 seasonal reskins bonus, out of scope for now) |
| Dungeon gate map | **not applicable** — confirmed no `VMI_DUNGEON_GATE_RANOX` anywhere in the newer build's own enum dump |
| Dungeon lounge map | **still needed** — id exists (`VMI_DUNGEON_LOUNGE_RANOX`), no world script/line-map for it in hand |
| Spa sub-location | **still needed** — id exists (`WI_SPA_RANOX`/`VMI_SPA_RANOX`), no world script/line-map/house entry in hand; not in original plan checklist |
| 7 dungeon lines' world/mesh scripts (Stage0..Boss) | done, all 33 world scripts + line-maps in hand (Secret_Extra intentionally reuses base Secret line-maps — verified) |
| 7 dungeon lines' gameplay-logic scripts (STAGE_LIST/NPC_GROUP/CLEAR_COND) | done — 19 `Dungeon_Ranox_*_{Normal,Hard,Expert}.src.lua` / `Secret_Hell` scripts now in hand, cover all 44 `DungeonData.lua` entries via reuse |
| `DungeonData.lua` master registry | done — full file with 44 RANOX entries in hand |
| `BattleFieldData.lua` master registry | done — full file with RANOX entries in hand |
| 4 field monster-spawn scripts | done (`Ranox_Field_00..03.src.lua`) |
| 4 field world/mesh scripts + line-maps | **still needed** — `WI_BATTLE_FIELD_RANOX_FIELD_00..03` + `..._REST_00` (5 world IDs) confirmed required, none in hand |
| `UnitTemplet.lua` | confirmed not monster-relevant (player-class file only, zero `NUI_` hits) — resolves the plan's open question, nothing to do here for monsters |
| `NPCTemplet.lua` | done — in hand (`NPCTemplet.src.lua`, and a near-identical `NPCTemplet_LA.src.lua`); `NPCTemplet_1.src.lua` is the pre-Ranox baseline, not needed going forward |
| `NPCStat.lua` | **still needed** — not provided at all |
| Per-monster gameunit scripts (FSM/mesh) | **still needed** — none provided; ~129 of them once confirmed |
| `DLG_Map_World.lua` / `DLG_Map_World_Elios.lua` | resolved differently than planned — no new `WORLD_MAP_ID` needed; Ranox just adds `LMI_RANOX` to the existing `LocalIDList` inside `DLG_Map_World_Elios.lua`. That file as given also lists `LMI_ELYSION`, out of this plan's scope — trim on merge. |
| `DLG_Map_Local.lua` | **still needed** |
| `DLG_Map_Village.lua` | **still needed** |
| `DLG_Map_House.lua` | **still needed** |
| Binary assets (mesh `.Y`, texture `.tga`/`.dds`, audio `.ogg`) | **still needed** — ~157 Ranox-specific filenames referenced, none present in the folder or the live game directory; some referenced assets are deliberate reuses from other regions and don't need sourcing |
| World-map UI dialogs (`DLG_UI_FIELD_MAP_Ranox_*`, `DLG_UI_RANOX_SECRET_FEVER_GAGE`) | in hand, bonus — not in original plan scope, reference `.DDS` textures not yet provided |
| Cobo Express (`MapData.lua`) | optional, not requested yet |
| PvP variant (`WI_RANOX_GATEWAY_PVPMAP`) | out of scope unless requested — no file covers it |
| Buff/title/currency systems (`BTI_BUFF_RANOX_SECRET_FEVER`, `BTI_BUFF_RANOX_6TH_DUNGEON_TITLE`, etc.) | out of scope — this plan doesn't cover buffs/titles at all; visible in the enum dump but no action needed unless asked |
