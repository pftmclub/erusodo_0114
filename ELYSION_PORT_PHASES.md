# Elysion Port (village, Field 03, SkyCrossroads / Adrian / Soless / Memory) — phased plan

> Written 2026-10-03, read-only research session, together with
> `ATLAS_PORT_PHASES.md`, and committed with it on `mods/june-buff-port`;
> Atlas Phase A0 cuts `mods/elysion-port` from that tip. **Read `ATLAS_PORT_PHASES.md` first** — it owns the shared groundwork
> (worktree, census, RE maps, level cap 99, every region enum) and its
> *Starting context*, *Fix philosophy*, *Model selection rules*, *Lanox
> lessons → gates*, *Flags* and *Exit metric* sections apply here unchanged.
> This doc repeats only what an Elysion phase needs on top.

## Context

Second half of the Atlas/Elysion port. In June, Atlas and Elysion are one
local map (`LMI_ELYSION`, 10009) with two pages; this doc owns the
**"Elysion" page** (`AddDummyString("1")`) and everything beyond Field 02:

- the **Elysion village** (`VMI_ELYSION` = 20009) and its new village systems
  (NPC relation data, new house keys, in-village warps);
- **Field 03** (40037) — a dead end back to the village;
- the local map's **scroll** between the Atlas and Elysion pages and the
  **guide window**;
- the **SkyCrossroads, Adrian, Soless and Memory** lines, which need three
  engine subsystems March lacks: **moving platforms**, the **SimpleDLG gauge
  subsystem**, and **dramatic messages**, plus ~35 more Lua bindings.

Intended outcome: from Field 02 the player walks into Elysion village (houses,
NPCs, shop, in-village warps), reaches Field 03, scrolls the local map to the
Elysion page, and can enter and clear all four lines at N/H/E in unlock order
(SkyCrossroads 83 → Adrian 84 → Soless 87 → Memory 87), with Soless/Memory
gauges, dramatic messages and moving platforms working.

## User decisions (2026-10-03) — same as the Atlas doc

1. Split by June's halves (this doc = Elysion half).
2. **Quests are out of this plan** — deferred to a later quest plan with the
   Lanox quests. June's quest keys go in verbatim and inert, with a
   `-- quest plan:` comment. **Quest-driven NPC swaps** (Yuno → Hyper Yuno →
   Nono) are ported as *mechanism* (E1) but stay dormant until quest data
   exists. The Elysion clear movie (quest 22336) is deferred.
3. Branch/worktree `mods/elysion-port` (Atlas A0). Buff-dependent phases are
   blocked on the named JUNE_BUFF phase.
4. Level cap 99 and June's ExpTable are delivered by Atlas A2 (Elysion
   monsters are level 83–99 and need NpcExpTable 81–99).

## Model, effort, review

| Phase | Item | Depends on | Kind | Model | Effort | Review |
|---|---|---|---|---|---|---|
| E0 | Preconditions, census, `ELYSION_SOLESS` decompile | Atlas A0–A4 (incl. A2 cap 99), A7 | tooling | Sonnet 5.5 | medium | none |
| E1 | Village engine | E0 (+ A1a map) | C++ (new mechanisms) | Opus 5.5 | high | required |
| E2 | Elysion village 20009 | E1, Atlas A5, A6 | data + C++ switches | Opus 5.5 | high | required |
| E3 | Field 03 (+ Field 04 enum-only check) | E2 | data | Sonnet 5.5 | high | required (Opus 5.5; escalate on gaps) |
| E4 | Local map, Elysion page (scroll, guide, filter, flags) | E0, Atlas A7 | C++ + data | Opus 5.5 | high | required |
| E5 | Moving platforms | Atlas A1b, A3 | C++ (new mechanism) | Opus 5.5 | high | required |
| E6 | SimpleDLG gauges + dramatic messages | Atlas A1b, A3 | C++ (new mechanisms) | Opus 5.5 | high | required |
| E7 | Elysion-half bindings + additional world | Atlas A1b, A3 | C++ | Opus 5.5 (Sonnet 5.5 for names A1b marks transcription-ready) | high | required |
| E8 | SkyCrossroads line (first line of the half) | E2, E4, E7, Atlas A2, A8 (per-line checklist) | data (+ C++ if gaps) | Opus 5.5 | high | required |
| E9 | Adrian line | E5, E7, E8 | data | Sonnet 5.5 if census shows 0 open gaps, else Opus 5.5 | high | required (Opus 5.5) |
| E10 | Soless line | E5–E7, E9 | data + C++ glue | Opus 5.5 | high | required |
| E11 | Memory line (finale) | E6, E7, E10, **JUNE_BUFF Phase 10** | data + C++ glue | Opus 5.5 | high | required |
| E12 | Elysion field PvP map (optional) | E3 | data | Sonnet 5.5 | high | required (Opus 5.5) |
| E13 | Asset closure + pack list (Elysion half) | E2–E12 | data | Sonnet 5.5 | high | required (Opus 5.5) |
| E14 | Build, deploy, hand-off | E13 | build | Sonnet 5.5 | medium | none |
| E15 | Play-test sweep + close-out (whole region) | E14 | verify + root-cause | Opus 5.5 | high | none |

**E1, E5, E6 and E7 are independent** and may run in parallel sessions —
each in its **own** extra worktree off `mods/elysion-port`, merged back in
the order E7 → E5 → E6 → E1 (they all append to `X2Main.cpp`'s binding block
and to the `Always.h` Atlas/Elysion block; merge order keeps the conflicts
trivial). Model rules are the Atlas doc's: Opus for anything derived from
the June binary or any mechanism March lacks; Sonnet only where A1b wrote the
method down, and a Sonnet session stops on anything unsettled; A1-level
phases at `xhigh`, everything else `high`; **no Fable, no Haiku**; record cost
in each Done note.

## Starting context (paste with any phase)

Use `ATLAS_PORT_PHASES.md` → *Starting context* verbatim (worktree
`Trunk_elysion`, branch `mods/elysion-port`, game dir, `kom_contents`
shared-staging merge rule, June tree + `june_src_fixed` only, Ghidra, elstools,
build/deploy, logs, "ask before every launch", "Do not consult"). Additions:

- **The one June Lua file not in `june_src_fixed/`:** `ELYSION_SOLESS.lua`
  (Soless boss, 105,101 B, `14JUN17/data/data037_kom/`). E0 decompiles it to
  `june2017_re/june_lua/ELYSION_SOLESS.src.lua` with
  `elstools decompile-batch` (for the `.std.luac`) then
  `python june2017_re/ljd_jun2017.py --check …`; every later phase reads that
  copy.
- **Debug warp:** `elysion` chat command (E2) → `VMI_ELYSION`.
- **Buff branch status** is in `JUNE_BUFF_PORT_PHASES.md`; check it before
  E10/E11.

## Investigation ledger — Elysion half (measured 2026-10-03; census re-verifies)

The shared ledger (IDs, end markers, offline seed, touch-list, assets, effect
sets, Lanox-side omissions) is in `ATLAS_PORT_PHASES.md`. Elysion-only facts:

### Topology and gates (June)

| From | Exit → target | Source |
|---|---|---|
| Field 02 (40036) | L142 → `VMI_ELYSION` pos 500 (A6 omitted it with a comment) | JF `data036_kom/BattleFieldData.src.lua:2104-2157` |
| `VMI_ELYSION` (20009) | 500 arrival from F02; 501 → F02 line 248; 516 arrival from F03; 517 → F03 line 1; 514 IsWarp; **in-village warps** 502→503, 504→505, 506→507, 508→509, 510→511, 512→513 (`InternalPlaceStartPosID` + `WaitingTime`) | JF `data034_kom/DLG_Map_Village.src.lua:991-1152` |
| Field 03 (40037) | L0 → `VMI_ELYSION` pos 516; dead end | BattleFieldData `:2158-2206` |

- F03 `REQUIRE_LEVEL` 85 / monster 88; enters and returns to `VMI_ELYSION`.
- `MapData` `VMI_ELYSION` row at `:357-370` (level 85–99, `BEFORE =
  VMI_RANOX`, inert quest keys). Cobo zone **18** for `VMI_ELYSION` and F03.
- Unlock chain continues from Tower: SkyCrossroads (`DungeonData:32268`,
  83–85) → Adrian (`32888`, 84–87) → Soless (`33120`, 87–99) → Memory
  (`33740`, 87–99), via `EXTRA_REQUIRE_DUNGEON_ID`.

### Local map (Elysion page)

- Page buttons: `DLG_Map_Local_Elysion_Back_NEW.src.lua:39-88` (`MOVE_UP`
  dummy "0", `MOVE_DOWN` "1"); hero filter `:23-37` (Hero → out of scope).
- June C++: scroll `FUN_329378a0` (+`329379d0`, `3293c1e0`, `329366a0`,
  string "Elysion_Local"), LMUCM **18006/18007**
  (`LMUCM_LOCAL_MAP_MOVE_UP/DOWN`), `OnFrameMove` `FUN_3293a960`
  (`LocalMapMoveUp/Down`); guide window `DLG_Elysion_LocalMap_Guide.lua` opened
  by `FUN_329386b0` from `OpenLocalMapDLG` (`FUN_3293c440`) for 0x2719,
  closed by LMUCM **18008** (`FUN_31be4210(0x472, +0x58, +0x5c)`); page filter
  member +0x54 (`UpdateDungeonButtons` hides controls whose dummy string ≠
  current page); event flags 0x8d4/0x8d5 via `FUN_322aa950`
  (`JUNE_UI_PARITY_PHASES.md:370-385, 401-403`). March leaves comments at
  `X2LocalMapUI.cpp:379, 800, 818, 885, 1304, 1310, 1338-1339, 1399`.
- Elysion-page titles: `MAP_TITLE_ELYSION`, `MAP_TITLE_Elysion_SkyCrossroads`,
  `MAP_TITLE_Elysion_Adrian`, `SOLESS`, `TITLE_MEMORY` in
  `DLG_Localmap_Title_01.tga` (the `_01_A` variant is only for
  `NF_DE/ES/IT`). Stars `STR_ID_40226/40227/42846/43399`.

### Village

- **Houses (12 new `HI_*`):** `HI_YUNO` 40092, `BERNARD`/`DURANDA`/`THEODOR`/
  `HUGO` 40131–40134, `BIRD_QUEST_00..04` 40135–40139,
  `ELYSION_RED/BLUE_TREE` 40145–40146; plus 9 shared houses with Elysion rows
  (`DLG_Map_House.src.lua` 8110-9365), `HI_GUILD_MARK`'s village row (Lanox
  omitted it, `M14/data034/DLG_Map_House.src.lua:4196-4197`, `4942-4944`).
- **Village NPCs (no unit scripts):** `NUI_YUNO` 10078, `BERNARD` 10143,
  `DURANDA` 10144, `THEODOR` 10145, `HUGO` 10146, `ELYSION_QUEST_BIRD0..4`
  10148–10152, `ELYSION_RED_TREE` 10194, `ELYSION_BLUE_TREE` 10195 (> March's
  `NUI_MAX_LIMIT` 10191 — A3 bumps it).
- **New `DLG_Map_House` keys:** `NPCMeshName2nd`, `AlphaBlend2ndMesh` (Yuno),
  `AlwaysShow`, `bOnlyClickable` (birds), `WAIT01_MOTION_NAME`,
  `bApplyAlphaTest`, `FRAGMENT_EXCHANGE` (trees), `TalkLoopAniNpc`.
- **NPC relation data:** JF `data034_kom/DLG_Map_House_NPC_RelationData.src.lua`
  (lines 28, 55, 84, 91, 667-824): `g_pQuestManager:AddVillageNpcRelationDataBy{Village,Quest}_LUA`
  + `VILLAGENPC_RELATIONDATA_TYPE.VRT_CHANGE`. It is the **only loader of the
  6 per-village `UI_*_ELYSION` dialogs** (data039: `UI_{ADAMS,ARIEL,BILLBOARD,
  CAMILLA,MU,RURIEL}_ELYSION`, plus `UI_NPC_BILLBOARD_ELYSION`,
  `UI_NPC_ELYSION_{RED,BLUE}_TREE`) and drives Yuno → Hyper Yuno (quest
  22227) → Nono (quest 22231).
- Loading art 0x4e29 → `HQ_LoadingVillage_Elision_01/02.tga` (JR data183).
  Music: `li_village_elysion001.ogg`, `field_elysion00*.ogg`,
  `li_field_elysion001.ogg` (JR `music/`).
- Seasonal village variants (`Elysion_Village_Festival`, `_XMAS`, `_XMAS_2`)
  are out of scope.

### Engine gaps (Elysion half; A1b assigns owners and readiness)

- **Moving platforms (E5)** — Adrian, Soless: `LINE_MOVE_TYPE.LMT_SPIN` /
  `LMT_PATROL` (`Elysion_Adrian_Stage2_linemap:1273,1299`),
  `AddMovableLineData_LUA`, `TRIGGER_LINE_ACTION_TYPE.TAT_LINE_CHANGE_MOVABLE_DATA`
  (`Elysion_Soless_Stage0_LineMap:1309`), linemap keys `DIRECT_MOVE_DATA`,
  `TARGET_MESH_NAME`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE`,
  `CAMERA_LAND_HEIGHT`, `CANDOWN`, `GetLineTypeByLineID_LUA`.
- **SimpleDLG gauges (E6):** `CreateSimpleDLG`, `DeleteSimpleDLG`,
  `GetSimpleDLG`, `SetShow_LUA`, `SetNumberInNumberStatic_LUA`
  (`_MoreDigit`, `_MoreParam`), `SetNumberInGageStatic_LUA`,
  `SetPercentValueInGageStatic_LUA`. Dialogs: `DLG_UI_ELYSION_SOLESS_AWAKEN_GAGE`
  (Soless boss), `…_HERBAON_GAGE` (`ELYSION_S_2STAGE_HERBAON:597`, Soless
  stage 2), **`…_OF_SUN_GAGE` → `ELYSION_M_SOLESS:3658` = the Memory boss**.
  Lanox deferred the same subsystem (`RANOX_PORT_PLAN.md:2989-3004`).
- **Dramatic messages (E6):** unit keys `DRAMATIC_MESSAGE0-4` +
  `IMAGE_FILE_NAME`, `BACK_COLOR`, `MESSAGE_COLOR`, `SOUND_FILE_NAME`,
  `MAX_ROW_CHAR`, `IMAGE_OFFSET_POSITION` (13 scripts);
  `DRAMATIC_MESSAGE_TYPE.DMT_LEFT/RIGHT` (`ELYSION_SOLESS_DRAMATIC:157`).
- **Bindings batch (E7):** `GetDyingStart`(b), `GetWorldObjectMeshByName`,
  `SetMovieDescriptionData`, `IsActiveMonster`(b), `FadeOriginWorldColor_LUA`,
  `LiveKeyCodeNPCNumExceptDying`, `GetCenterPositionLineIndex` (these first
  arrive with Atlas A4 — reuse), plus: `GetLiveUserUnitRandom`,
  `GetNowStateName`, `SetCameraDistance`(b, A4), `SetParticleSize`,
  `AddEffectSetDeleteCondition_LUA`, `AnimResume`, `AnimStop`(b),
  `CreateApplyAlphaMeshInstance_LUA`, `CreateApplyAlphaParticleSequence_LUA`,
  `GetNearestUnitInSpecificRange_LUA`, `GetSyncDummyValue`,
  `SetSyncDummyValue_Host`, `GetUnitID`, `SetPosion` (June typo — keep the
  name), `StartAlphaFade`, `StartFadeIn/OutCustomColor`,
  `AddStateCoolTime_LUA`, `SetStateCoolTime_LUA`,
  `FindRandomSelectUserUnitsFromRange_LUA`,
  `GetFindedRandomSelectUserUnits_LUA`, `GetRandomSelectUserUnitFromRange`,
  `Get/SetStoredGUUser`, `GetUnitIndex`(b), `IsSceneEnded`,
  `SetShowAllNPCExceptMe`, `SetShowHpSmallGage`, `SetUseExternalCondition_LUA`;
  from `ELYSION_SOLESS` bytecode (verified by byte context):
  `ChangeMotion_LUA`, `ClearStoredNPC`, `Play2DSound_LUA`,
  `PlayWorldAllSound`, `SetShowInvincible`(b), `SetShowWorld`; string-only
  (E0 confirms): `PlayAdditionalWorldAllSound`, `SetShowAdditionalWorld`.
  **Additional world:** WorldData `dataFileName_Additional` (`:3434`) —
  March's `AddWorldData_LUA` reads only 3 keys (`X2WorldManager.cpp:170-184`).
  Adrian keys: `DIRECT_MOVE_DATA`, `TARGET_MESH_NAME`;
  `MOTIONNAME_EFFECTSETNAME` (Lanox kept it inert — keep consistent).
- **Buff-owned:** `GetBuffAccumulationCountNow` (`ELYSION_M_SOLESS` = Memory
  boss; possibly `ELYSION_SOLESS`) → **JUNE_BUFF Phase 10**.
  `SOLESS_TITLE_INVINCIBLE` (BBT 101 + a custom-function class) is a **title
  item**, not dungeon content — no block.
- **Cross-script global:** `IsReadyBreakAttack` is defined in
  `ELYSION_M_SOLESS` and called from other Memory scripts — load-order
  dependency, not an engine gap.
- **June bugs to transcribe verbatim:** `NUI_ELYSION_S_CIRCLE_CTR` typo
  (`Dungeon_Elysion_Soless_{N,H,E}.src.lua:732`; nil in June too),
  `LINE_TYPE.LT_INVALID` (`ELYSION_BOSS_LINE_A:107`, undefined in June too).
  Dead scripts with no `NPCTemplet` entry: `ELYSION_M_2STAGE_GATE`,
  `ELYSION_M_HARNIE`, `ELYSION_M_SOLESS2`, `ELYSION_S_0STAGE_FLOOR` — skip.

### Files per line (June)

| Line | Dungeon scripts (data035) | Scenes + linemaps (data040) | Notes |
|---|---|---|---|
| SkyCrossroads | `Dungeon_Elysion_SkyCrossroads_{N,H,E}` | `Elysion_SkyCrossroads_{Stage0,1,2,BossStage}` + LineMaps (8) | `ELYSION_DUMMY_TITLE` placed N/H/E (`:415/569/890`), checked by `ELYSION_FAKE_OBJECT:277` — data only |
| Adrian | `…_Adrian_{N,H,E}` | `Elysion_Adrian_{Stage0..3,BossStage}` + LineMaps (10; `Stage2_linemap` lowercase) | moving platforms |
| Soless | `…_Soless_{N,H,E}` | `Elysion_Soless_{Stage0..3,BossStage}` + LineMaps, `Stage1_Hide` + LineMap, `BossStage_2Phase` (uses BossStage's LineMap) (13) | hidden stage `WI_SOLESS_STAGE1_HIDE` via `NUI_ELYSION_HIDE_STAGE_A/B` (`Soless_Normal:403`); phase-2 additional world; true/bad ending; boss script needs E0's decompile |
| Memory | `…_Memory_{N,H,E}` | `Elysion_Memory_{Stage0..3,BossStage}` + LineMaps (10) | finale, boss `NUI_ELYSION_M_SOLESS`, `CC_TIME` |

WORLD_IDs: SC 672–675, AR 706–710, SL 711–716, ME 727–731. Dungeon images:
`DLG_UI_Dungeon_Image12.dds` (new) for Soless/Memory. Loading screens
`HQ_LoadingDungeon_{Soless,Memory,…}_Pit_*` (JR data183).

---

## Phase E0 — Preconditions, census, `ELYSION_SOLESS` decompile

**Steps**
1. Confirm Atlas A0–A4 (incl. A2, level cap 99) and A7 are Done (their Done
   notes in `ATLAS_PORT_PHASES.md`); E2+ also need A5/A6, and E8–E11 need A8
   (it writes the *Per-line checklist*). Read JUNE_BUFF's Phase 10
   status.
2. Decompile `ELYSION_SOLESS.lua` (see *Starting context*); `--check` must
   pass. Confirm the string-only names (`PlayAdditionalWorldAllSound`,
   `SetShowAdditionalWorld`, `SetNumberInGageStatic_LUA`,
   `SetNumberInNumberStatic_MoreParam_LUA`, `SetUseExternalCondition_LUA`,
   `Add/SetStateCoolTime_LUA`, `GetBuffAccumulationCountNow`) are real calls;
   add any new name to A1b's `region_engine.tsv` with an owner (Opus pass if
   it needs RE).
3. Point `region_census.py` at the decompiled copy and record the
   `--region elysion` baseline per line.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the Elysion ledger and "Phase E0". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline; tell subagents the same.

Effort: medium.

Your task (Phase E0): confirm the Atlas phases E0 depends on are Done; decompile
14JUN17/data/data037_kom/ELYSION_SOLESS.lua with elstools decompile-batch +
june2017_re/ljd_jun2017.py --check into june2017_re/june_lua/; confirm its
string-only engine names and register any new one in region_engine.tsv; record
the region_census.py --region elysion baseline.

Done when: decompile passes --check; baseline recorded. Record a Done note
with the session cost, then commit on mods/elysion-port.
```

### Phase E0 — Done

*(empty)*

---

## Phase E1 — Village engine

**Steps** (`SERV_IRUHADEV_ELYSION_VILLAGE`, June addresses from A1a):
1. **NPC relation system:** bind `AddVillageNpcRelationDataBy{Village,Quest}_LUA`
   on the quest manager, store June's records, and port June's whole
   evaluation (which NPC/UI a house shows per village and per quest state).
   Load `DLG_Map_House_NPC_RelationData.lua` where June does. The ByQuest
   branch evaluates against the quest manager as June does; with no
   Elysion quests in data it yields the default NPC — **dormant, not
   stubbed** (comment names the quest plan).
2. **`DLG_Map_House` keys:** parse and apply `NPCMeshName2nd`,
   `AlphaBlend2ndMesh`, `AlwaysShow`, `bOnlyClickable`, `WAIT01_MOTION_NAME`,
   `bApplyAlphaTest`, `FRAGMENT_EXCHANGE`, `TalkLoopAniNpc` with June's
   semantics (`X2TFieldNpc*.cpp`, `HouseParsing`).
3. **In-village warps:** `InternalPlaceStartPosID` + `WaitingTime` in
   `VillageParsing` and the runtime that moves the player (June's version).
4. Add the matching C++ enum for `VILLAGENPC_RELATIONDATA_TYPE` with explicit
   values equal to A3's Lua values.
5. *(Added by june-effect C9h-2e, 2026-10-08; rows are funcmap `Effect C9h-2e`.)*
   Step 1's June addresses:
   - loaders `3233b680` (ByQuest) and `32340720` (ByVillage);
   - lookups `32340320` and `323446c0`;
   - the 0x388 row (`+0` quest, `+4` house, `+0xc` code event, `+0x80`
     village), plus ByQuest's second 0x14-byte shape at `+0xbc`.

   Port all three entry points:
   - village NPC creation `323ba5f0`;
   - the per-quest refresh `32340630` (from three quest-UI handlers);
   - the code-event refresh `32344880`, called from
     `Handler_EGS_UPDATE_CODE_EVENT_NOT`'s `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`
     block on `mods/june-buff-port`.

   The TFieldGame side is `323b7ff0` (replace), `323a9ed0` (remove) and
   `323b8970` (restore).

   Reach on March data:
   - the 27 code-event rows (event 2308, quest 0) stay dormant offline;
   - the ByVillage row for `VMI_SPA_RANOX` (1501, a **March** village) swaps its
     billboard to June's `UI_NPC_BILLBOARD_SPA.lua`, which March lacks. Settle
     this row before claiming "existing villages unchanged".
6. *(Added by june-effect C9h-5, 2026-10-08; rows are funcmap `Effect C9h-5`,
   block list `june2017_re/c9h5_fieldnpc_onframemove_diff.md`.)* Step 2's key
   list is short: June's `HouseParsing` (`324f3260`) reads **56** keys March
   does not (decoded list in the C9h-5 Done note). Port June's
   `CX2TFieldNpc` with them, not only step 2's eight:
   - **`OnFrameMove` whole** (`325cdfa0`, 44 blocks, 13 June-only), plus the
     outlined talk / bye steps `325ccf60` / `325ca2f0` (diff those first;
     C9h-5 did not), `325ca0b0` (quest-conditional `MESSAGE` rows, templet
     `+0x590`), the motion map `+0x1f4` (8 `*_MOTION_NAME` keys; June's
     defaults are March's literals plus `Talkloop` / `TalkWait`), the motion
     queue `+0x204` / `325ca920`, the second skin (`NPCMeshName2nd`), the
     `MultiTex` xet, `Upgrade/Service/Trade_Talk`, the house 40082 agit-bank
     and 40129 highlight branches, `bOnlyClickable` (`323bddf0`, not a
     riding-pet getter as C9h-4 had it).
   - **The village-NPC effect set:** key `EFFECT_SET_NAME` -> templet
     `+0x5e8`; play once after the skin is built (`@325ce1d9`), handle `+0x190`
     (ctor -1, stop only in the dtor `325cd7c0`), `SetShowObject( true )`
     and nothing after. Engine: `PlayEffectSet( name, CX2TFieldNpc* )`
     `321e26d0` (instance `+0xe0`, mode `ESM_FIELD`) and its four readers:
     `GetEffectPosition @321dd1a3`, `GetCombineMatrix @321dc8af`,
     `GetEffectPositionByBoneName @321dd9dc`, the create-time anim gate
     `@321eebf3` (no `GetUnitRotation` branch), with the NPC's bone helpers
     `321f2c10` / `321f2c70`. These sit in the same switch cases as C9h-3a's
     `+0xd8` riding-pet branches (`X2EffectSet.cpp`): merge
     `mods/june-buff-port` after C9h-3a lands before porting them.
   - **Friendship branches** (gated `IsEnableCode( 2215 )`, never on
     offline): port the gate and the calls as June has them; the manager
     `32696fa0` and the sequencer `325c9e70` belong to backlog **Stage 18**.
     Whichever lands second wires the calls.
   - **Behaviour change in March villages** (June wins).
     - The quest button follows only new normal quests: `SETG` @325cebcd,
       confirmed in raw disassembly.
     - The talk mark no longer blinks: June has no `_Down.dds` literal
       (confirmed).
     - Static NPCs with a `TalkWait` anim play it while joined (per the diff;
       confirm in Ghidra).
     - The `+0x239` completable-quest call is live and its result is
       discarded. `+0x239` is set by `CX2TFieldGame` `323b5970 @323b72fa`,
       which belongs in E1 too.

     Add JUNE_BUGS rows for the diff file's candidates. Add one more for the
     effect instance's raw `+0xe0` pointer if the queued stop lets it read a
     freed NPC.
   - **Assets (E2 / E13):** `EffectSet_Elysion_NPC_Ver_ring`, `_Dyurenda_AC`,
     `_Teo_ring` (June `EffectSet.lua` and `UIEffectSet.lua`) and their
     `NPC_Elysion_*` particles; the three houses are `HI_BERNARD`,
     `HI_DURANDA`, `HI_THEODOR`.

**Done when:** census reports no unread key/binding owned by E1; build rc=0,
new literals in the exe; existing villages unchanged (their files carry none
of these keys — confirm with the census), **except** step 6's June behaviour
changes, which come from code, not keys: each one is listed in the Done note
and confirmed in the play-test, never reverted to March's *(amended by
june-effect C9h-5, 2026-10-08)*.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the Elysion ledger's "Village" section, the
A1a/A1b maps, and "Phase E1". Use the ghidra skill for June's bodies. Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E1): behind SERV_IRUHADEV_ELYSION_VILLAGE port June's NPC
relation system whole (ByVillage and ByQuest; the quest branch stays dormant
until the quest plan), the new DLG_Map_House keys, and in-village warps
(InternalPlaceStartPosID / WaitingTime), with June addresses cited.

Premises to verify first: where June loads DLG_Map_House_NPC_RelationData.lua;
that no existing March village file uses any of the new keys.

Done when: Phase E1's "Done when" holds. Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
```

### Phase E1 — Done

*(empty)*

---

## Phase E2 — Elysion village (20009)

**Steps** — same checklist as Atlas A5, for the village:
1. Scene `Elysion_Village.lua` + `Elysion_villeage_LineMap.lua` (June's
   spelling), `WorldData` 636.
2. `DLG_Map_Village` `Village[VMI_ELYSION]`: `WORLD_ID`, `HouseIDList` (12 new
   + shared; every element resolves — nil-hole check), `START_POS` 500–517
   with March key names + the in-village warp keys (E1).
3. `DLG_Map_House`: the 12 new house blocks, Elysion rows in the 9 shared
   houses, `HI_GUILD_MARK` village row (lift Lanox's omission); village NPC
   unit rows in `NPCTemplet`; `DLG_Map_House_NPC_RelationData.lua`; the data039
   `UI_*_ELYSION` dialogs; `ShopItem.lua` `VILLIAGE_ID` block (studio typo).
4. `MapData` row (inert quest keys) + Cobo zone **18** for `VMI_ELYSION` and
   F03 (client + `ServerResource/US/MapData.lua` mirror).
5. C++ cases (June's whole functions, per A1a): `IsValidWarpField`,
   `LoadFieldImage` (0x4e29 → `HQ_LoadingVillage_Elision_01/02.tga`),
   `OpenFieldName`, `GetFieldMapDlg` + `DLG_UI_FIELD_MAP_Elysion_Village_NEW`,
   camera/navi, both warp-list implementations + `Village_20009` controls,
   `IsVillageField` already widened in A3. Chat `elysion`.
6. Restore Field 02's exit `L142 → VMI_ELYSION` pos 500 (A6 left it
   commented). Town + F03 pins on the world map (A5 ported the block).
7. Music, loading art, house/NPC meshes and motions.

**Done when:** census `--line VIL` clean; build rc=0; staged + pack list.
After pack: `elysion` warp renders the village with every house and NPC;
in-village warps work; F02 → village → F02 round trip; Cobo + warp NPC rows;
shop sells; NPC UI dialogs open.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, both ledgers, Atlas "Phase A5" (the same
checklist) and "Phase E2". Also read LANOX_TROUBLESHOOTING.md's symptom table
and LANOX_DEFECT_PHASES.md Bug A (shared-house rows). Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E2): port Elysion village 20009 end to end -- scene,
WorldData, DLG_Map_Village (March key names + in-village warps), the 12 houses
and shared-house rows incl. HI_GUILD_MARK, NPC relation data and UI dialogs,
ShopItem, MapData + Cobo zone 18 (client and ServerResource mirror), June's
whole C++ region functions, the "elysion" chat warp, music/art, and restore
Field 02's village exit.

Premises to verify first: every HouseIDList element resolves; June's
START_POS 500-517 semantics against March's VillageParsing.

Done when: Phase E2's "Done when" holds. Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
```

### Phase E2 — Done

*(empty)*

---

## Phase E3 — Field 03 (+ Field 04 check)

Follow Atlas A6's field steps for Field 03 (`Field_Elysion_Stage2` + LineMap,
`Elysion_Field_03.lua`, WI 640, field-map `DLG_UI_FIELD_MAP_Elysion_03_NEW`,
pin, monsters, effects, drops). Confirm Field 04 (40038/641) is enum-only in
June (no row, scene or dialog) and leave it at that.

**Run with:** Sonnet 5.5, high effort; required Opus 5.5 review; escalate to
Opus on any engine gap the A1b map doesn't settle.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, Atlas "Phase A6" and "Phase E3". Never
launch X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high. Stop on any engine gap the A1b map doesn't settle and log it
for an Opus pass.

Your task (Phase E3): port Field 03 the way Atlas A6 ported Fields 01/02, and
confirm Field 04 is enum-only.

Done when: census --line FLD (F03) is clean, staged with manifests, pack list
handed over. Record a Done note with the session cost, then commit on
mods/elysion-port.
```

### Phase E3 — Done

*(empty)*

---

## Phase E4 — Local map, Elysion page

**Steps** (`SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION`, June's whole functions):
1. Page filter: port the +0x54 member and `UpdateDungeonButtons`' dummy-string
   test so only the current page's controls show (titles, buttons, stars).
2. Scroll: LMUCM 18006/18007 handlers (`FUN_329378a0(id, 1/2)`), the
   `OnFrameMove` slide (`FUN_3293a960`), `MOVE_UP/DOWN` buttons un-hidden
   (A7 hid them with a comment).
3. Guide window: open `DLG_Elysion_LocalMap_Guide.lua` from `OpenLocalMapDLG`
   for `LMI_ELYSION` as June does (first-visit logic per June), close on 18008.
4. Event flags 0x8d4/0x8d5: port the gates as June has them; if the flags
   come from a June event system March lacks, parse/store/log and leave the
   default June shows when the flag is off.
5. Elysion-page TET keys (`DLG_Localmap_Title_01`: `MAP_TITLE_ELYSION`,
   `MAP_TITLE_Elysion_SkyCrossroads`, `_Adrian`, `SOLESS`, `TITLE_MEMORY`),
   dungeon images (`Image12`), star `STR_ID`s.

**Done when:** census `--line UI` clean; build rc=0; staged + pack list.
After pack: the local map scrolls between the Atlas and Elysion pages; each
page shows only its own buttons/titles; the guide opens and closes; Elysion
buttons honour the chain.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the Elysion ledger's "Local map" section,
Atlas "Phase A7" and its Done note, JUNE_UI_PARITY_PHASES.md "Function map:
CX2LocalMapUI", and "Phase E4". Use the ghidra skill. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E4): port June's Elysion local-map behaviour whole behind
SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION -- page filter (+0x54), scroll
(18006/18007 + OnFrameMove), guide window (open for LMI_ELYSION, close on
18008), event flags 0x8d4/0x8d5 -- and stage the Elysion-page TET keys and
dungeon images.

Premises to verify first: the LMUCM C++ values equal the Lua values A3 added.

Done when: Phase E4's "Done when" holds. Record a Done note (deployed branch,
session cost), then commit on mods/elysion-port.
```

### Phase E4 — Done

*(empty)*

---

## Phase E5 — Moving platforms

**Steps** (`SERV_IRUHADEV_JUNE_MOVABLE_LINE`, June addresses from A1b):
1. C++ `LINE_MOVE_TYPE` enum with explicit values equal to A3's Lua table.
2. Line-map parser: June's movable-line data and keys (`DIRECT_MOVE_DATA`,
   `TARGET_MESH_NAME`, `FOLLOWER_LINE_ID`, `STOP`, `USE_ABSOLUTE_COORDINATE`,
   `CAMERA_LAND_HEIGHT`, `CANDOWN`) in `KTDGLineMap` (and its byte-identical
   server copy `XSLLineMap.h` only if June's header changed there too — check).
3. Runtime: per-frame line movement (spin/patrol), units standing on a
   moving line move with it, the attached mesh follows (`TARGET_MESH_NAME`),
   followers follow.
4. `AddMovableLineData_LUA`, `TAT_LINE_CHANGE_MOVABLE_DATA` trigger case
   (value per A1b, inside the June-renumbered trigger switch),
   `GetLineTypeByLineID_LUA`.
5. P2P sync: in-match state is peer-to-peer — check how June syncs platform
   phase between host and guests (or whether it's time-deterministic) and
   port that, not a client-local guess.

**Done when:** census reports no movable-line item open; build rc=0; a
`[ELYDBG] movable line …` first-use line in the exe; the Adrian/Soless
linemaps parse without `[ELYDBG] unsupported` lines.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the "Engine gaps" ledger, the A1b engine map
rows owned by E5, and "Phase E5". Use the ghidra skill. Never launch
X2_offline.exe without asking. Do NOT consult NEW_VILLAGE_*.md or
mods/lanox-offline.

Effort: high.

Your task (Phase E5): port June's moving-platform system whole behind
SERV_IRUHADEV_JUNE_MOVABLE_LINE -- LINE_MOVE_TYPE enum (values equal to the
Lua table), linemap keys and parser, per-frame movement incl. carried units
and attached meshes, AddMovableLineData_LUA, TAT_LINE_CHANGE_MOVABLE_DATA,
GetLineTypeByLineID_LUA, and June's P2P sync for it.

Done when: Phase E5's "Done when" holds. Record a Done note with the session
cost, then commit on mods/elysion-port.
```

### Phase E5 — Done

*(empty)*

---

## Phase E6 — SimpleDLG gauges + dramatic messages

**Steps**
1. **SimpleDLG** (`SERV_IRUHADEV_JUNE_SIMPLE_DLG`): port June's subsystem —
   create/get/delete a script-owned dialog, show/hide, number statics
   (`SetNumberInNumberStatic_LUA`, `_MoreDigit`, `_MoreParam`), gauge statics
   (`SetNumberInGageStatic_LUA`, `SetPercentValueInGageStatic_LUA`), lifetime
   tied to the game/stage as June does. Stage the three
   `DLG_UI_ELYSION_SOLESS_*_GAGE.lua` dialogs + their TET atlases. Check
   whether Lanox's deferred SimpleDLG uses (`RANOX_PORT_PLAN.md:2989-3004`,
   Secret Hell gauge) now work, and note it.
2. **Dramatic messages** (`SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE`): C++
   `DRAMATIC_MESSAGE_TYPE` enum (explicit values = Lua), unit-script keys
   `DRAMATIC_MESSAGE0-4` + sub-keys, and the display (image, back colour,
   text colour, sound, row length, image offset, left/right).

**Done when:** census reports no SimpleDLG/dramatic item open; build rc=0;
first-use `[ELYDBG]` literals in the exe; gauge dialogs and atlases staged.

**Run with:** Opus 5.5, high effort. Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the "Engine gaps" ledger, the A1b rows owned
by E6, and "Phase E6". Use the ghidra skill. Never launch X2_offline.exe
without asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E6): port June's SimpleDLG subsystem (behind
SERV_IRUHADEV_JUNE_SIMPLE_DLG) and dramatic-message system (behind
SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE) whole, with June addresses cited; stage
the three Soless/Memory gauge dialogs and their TET atlases; note whether
Lanox's deferred SimpleDLG uses now work.

Done when: Phase E6's "Done when" holds. Record a Done note with the session
cost, then commit on mods/elysion-port.
```

### Phase E6 — Done

*(empty)*

---

## Phase E7 — Elysion-half bindings + additional world

**Steps** (`SERV_IRUHADEV_ELYSION_SCRIPT_B`): every name A1b assigned to E7
(the *Bindings batch* list), receiver class verified, June address cited.
Plus the **additional world**: WorldData `dataFileName_Additional` in
`AddWorldData_LUA`, loading the second world, `SetShowAdditionalWorld`,
`PlayAdditionalWorldAllSound` (Soless phase 2). Reuse Atlas A4's bindings;
don't duplicate them.

**Done when:** census reports no E7-owned name/key open; build rc=0; new
literals in the exe.

**Run with:** Opus 5.5, high effort (Sonnet 5.5 only for names A1b marked
transcription-ready, stopping on anything else). Required review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, the "Engine gaps" ledger, the A1b rows owned
by E7, Atlas "Phase A4" and its Done note, and "Phase E7". Use the ghidra
skill. Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E7): implement every binding A1b assigned to E7 behind
SERV_IRUHADEV_ELYSION_SCRIPT_B (receiver class verified, June address cited,
reusing A4's), plus the additional-world support (dataFileName_Additional,
SetShowAdditionalWorld, PlayAdditionalWorldAllSound).

Done when: Phase E7's "Done when" holds. Record a Done note with the session
cost, then commit on mods/elysion-port.
```

### Phase E7 — Done

*(empty)*

---

## Phases E8–E11 — the four dungeon lines

Each follows Atlas's *Per-line checklist* (written by A8): dungeon scripts,
stages + linemaps, `DungeonData` rows (verbatim, Hero rows omitted with a
comment), unit scripts, `NPCTemplet`, `NPCStat`/`NewNPCStat` readiness, boss
face keys, effect sets via the June effect pipeline (merge under the
shared-staging rule), sounds, drops (unknown-item rule), offline clear and
unlock, buff census.

| Phase | Line | Line-specific | Run with |
|---|---|---|---|
| E8 | SkyCrossroads | first line of the half; `ELYSION_DUMMY_TITLE`/`FAKE_OBJECT` interplay; Herna/Emperor families | Opus 5.5, high |
| E9 | Adrian | moving platforms (E5); `SetCameraDistance`, `SetParticleSize`; emperor/laser objects | Sonnet 5.5 high if census shows 0 open gaps, else Opus 5.5 high |
| E10 | Soless | boss script from E0's decompile; hidden stage (`WI_SOLESS_STAGE1_HIDE`, `HIDE_STAGE_A/B`); phase-2 additional world (E7); `AWAKEN`/`HERBAON` gauges (E6); dramatic messages (E6); true/bad ending branches; `S_CIRCLE_CTR` typo verbatim; check `GetBuffAccumulationCountNow` use in `ELYSION_SOLESS` | Opus 5.5, high |
| E11 | Memory | finale, boss `ELYSION_M_SOLESS` (`OF_SUN_GAGE`, `IsReadyBreakAttack` global, `CC_TIME`); **blocked on JUNE_BUFF Phase 10** (`GetBuffAccumulationCountNow`) — check first, record the block and stop if not landed | Opus 5.5, high |

All four: required review (Opus 5.5).

**Done when (each):** census `--line SC|AR|SL|ME` clean (buff-owned items
only if their buff phase has landed); build rc=0 if C++ changed; staged +
manifests; pack list handed over. After pack: every difficulty enters, every
stage (incl. Soless hidden stage and phase 2) loads, gauges/messages/platforms
work, clear pays out and unlocks the next line.

### Phase E8 — SkyCrossroads

**Run with:** Opus 5.5, high effort (first line of the half). Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, both ledgers, Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix A (SkyCrossroads NUIs) and the
"SkyCrossroads" row of "Phases E8-E11". Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E8): port the SkyCrossroads line by the Per-line checklist. It is the
first line of the half: confirm E2, E4 and E7 are Done, then port the three
dungeon scripts, the 4 stages + linemaps, the SkyCrossroads unit scripts
(Herna/Emperor families, KEYOBJECT/FAKE_OBJECT/MOVE_OBJECT, the
WATERCANNON_OBJECT.LUA uppercase extension) and ELYSION_DUMMY_TITLE as data.
Add any Elysion-specific notes the checklist lacks to it.

Done when: census --line SC is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E8 -- Done
(date)" note (deployed branch, session cost), then commit on
mods/elysion-port.
```

#### Phase E8 — Done

*(empty)*

### Phase E9 — Adrian

**Run with:** Sonnet 5.5, high effort **if** `region_census.py --line AR` shows zero open engine gaps; otherwise Opus 5.5, high. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, both ledgers, Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix A (Adrian NUIs) and the
"Adrian" row of "Phases E8-E11". Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E9): port the Adrian line by the Per-line checklist. Its linemaps
use moving platforms (E5 must be Done -- Elysion_Adrian_Stage2_linemap is
lowercase), the laser/emperor objects, SetCameraDistance and SetParticleSize
(A4/E7). Reused Lanox NUI: SECRET_MAG_NOM.

Done when: census --line AR is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E9 -- Done
(date)" note (deployed branch, session cost), then commit on
mods/elysion-port.
```

#### Phase E9 — Done

*(empty)*

### Phase E10 — Soless

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, both ledgers, Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix A (Soless NUIs) and the
"Soless" row of "Phases E8-E11". Read the Soless boss from
june2017_re/june_lua/ELYSION_SOLESS.src.lua (E0's decompile -- it is not in
june_src_fixed/). Check JUNE_BUFF_PORT_PHASES.md for Phase 10's status first.
Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass. If ELYSION_SOLESS really calls GetBuffAccumulationCountNow and JUNE_BUFF
Phase 10 hasn't landed, record the block and stop.

Your task (Phase E10): port the Soless line by the Per-line checklist: the boss,
the hidden stage (WI_SOLESS_STAGE1_HIDE via HIDE_STAGE_A/B), the phase-2
additional world (E7), the AWAKEN and HERBAON gauges (E6), dramatic messages
(E6), the true/bad ending branches, and June's NUI_ELYSION_S_CIRCLE_CTR typo
transcribed verbatim with a log line. Reused Lanox NUIs: SECRET_MAG_CHARGER,
SECRET_SUMMON_MAG_NOM, SECRET_MAGI_OBJECT, SECRET_LAVA_DUMMY.

Done when: census --line SL is clean (hidden stage and phase 2 included), staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E10 -- Done
(date)" note (deployed branch, session cost), then commit on
mods/elysion-port.
```

#### Phase E10 — Done

*(empty)*

### Phase E11 — Memory

**Run with:** Opus 5.5, high effort. Required review (Opus 5.5). **Blocked on JUNE_BUFF Phase 10.**

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, both ledgers, Atlas's "Per-line checklist",
the buff-dependency and unknown-item rules, Appendix A (Memory NUIs) and the
"Memory" row of "Phases E8-E11". FIRST check JUNE_BUFF_PORT_PHASES.md: if Phase 10
(GetBuffAccumulationCountNow) hasn't landed, record the block in this doc and
stop. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline; tell subagents
the same.

Effort: high. If the census shows an engine gap the A1b map doesn't settle,
stop on it and log it for an Opus pass.

Your task (Phase E11): port the Memory line by the Per-line checklist -- the
finale: boss ELYSION_M_SOLESS (the OF_SUN gauge from E6, the
IsReadyBreakAttack global other Memory scripts call -- respect the load order,
CC_TIME clear condition), the memory/knight/summon families and dramatic
messages.

Done when: census --line ME is clean, staged with manifests, pack
list handed over (live-kom check after I pack). Record a "Phase E11 -- Done
(date)" note (deployed branch, session cost), then commit on
mods/elysion-port.
```

#### Phase E11 — Done

*(empty)*

---

## Phase E12 — Elysion field PvP map (optional)

Mirror Lanox Phase 11 (`RANOX_PORT_PLAN.md` Phase 11 findings):
`PVP_Elysion_Field` + LineMap (WI 752), `PVP_Map_List` row
(`MAP_SELECT_ELISION`; lift `M14/data036/PVP_Map_List.src.lua:116-117`'s
omission), map-select art. `WINNER_Adrian_PVPMAP` (757, tournament) stays
out.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, RANOX_PORT_PLAN.md Phase 11 findings, and
"Phase E12". Never launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E12): port the Elysion field PvP map (WI 752,
MAP_SELECT_ELISION) the way Lanox Phase 11 ported GateWay.

Done when: staged with manifests, pack list handed over. Record a Done note
with the session cost, then commit on mods/elysion-port.
```

### Phase E12 — Done

*(empty)*

---

## Phase E13 — Asset closure + pack list (Elysion half)

Same method as Atlas A11, over everything E1–E12 staged (mesh-internal
textures, TET keys, truncation, mount order); one consolidated pack list.

**Run with:** Sonnet 5.5, high effort. Required review (Opus 5.5).

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
and ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) --
both "Starting context" sections, Atlas "Phase A11" and "Phase E13". Never
launch X2_offline.exe without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E13): close the Elysion half's asset set exactly as Atlas A11
did for its half and produce one consolidated pack list.

Done when: census --region elysion shows no missing assets or keys and the
pack list is in the doc. Record a Done note with the session cost, then commit
on mods/elysion-port.
```

### Phase E13 — Done

*(empty)*

---

## Phase E14 — Build, deploy, hand-off

Same as Atlas A12, plus the Elysion reachability checklist.

**Run with:** Sonnet 5.5, medium effort. No review.

```
Continuing the Atlas/Elysion port, Elysion half. Read ELYSION_PORT_PHASES.md
(worktree Trunk_elysion, branch mods/elysion-port) -- "Phase E14" and
"Verification" -- and Atlas "Starting context". Never launch X2_offline.exe
without asking. Do NOT consult
NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: medium.

Your task (Phase E14): build all four projects as US_SERVICE from the
worktree (PCH touched), prove every Elysion-port flag's literal is in x2.exe,
deploy as X2_offline.exe (size/mtime checked; record that it is the
mods/elysion-port build), and hand me the pack list and the Elysion
reachability checklist.

Done when: deployed and handed off. Record a Done note with the session cost,
then commit on mods/elysion-port.
```

### Phase E14 — Done

*(empty)*

---

## Phase E15 — Play-test sweep + close-out (whole region)

Root-cause every checklist failure (short-lived `[ELYDBG]` diagnostics, scope
log checks to the phase's own lines); `MODS.md` entries for every flag;
troubleshooting rows; complete *Deferred to the quest plan* in both docs.

**Run with:** Opus 5.5, high effort. No review.

```
Continuing the Atlas/Elysion port. Read ELYSION_PORT_PHASES.md and
ATLAS_PORT_PHASES.md (worktree Trunk_elysion, branch mods/elysion-port) in
full, plus LANOX_TROUBLESHOOTING.md. Never launch X2_offline.exe without
asking. Do NOT consult NEW_VILLAGE_*.md or mods/lanox-offline.

Effort: high.

Your task (Phase E15): I have packed and played the Elysion reachability
checklist; I'll paste results and logs. Root-cause each failure with
short-lived [ELYDBG] diagnostics, fix, rebuild, hand off again. Then close out
the whole region: MODS.md entries for every flag, troubleshooting rows, and
both docs' "Deferred to the quest plan" sections.

Done when: every checklist item passes or is recorded with a cause and owner.
Record a Done note with the session cost, then commit on mods/elysion-port.
```

### Phase E15 — Done

*(empty)*

---

## Verification (end to end, Elysion half)

- [ ] Field 02 → village → Field 02 round trip; `elysion` chat warp renders
      the village (scene, music, loading image).
- [ ] All village houses and NPCs present (no truncated `HouseIDList`); the
      birds/trees/Yuno render with their new house keys; in-village warps
      move the player after `WaitingTime`; NPC UI dialogs open; shop sells.
- [ ] Village → Field 03 → back; F03 monsters spawn, fight, drop.
- [ ] Cobo Express + warp NPC list the village (zone 18); world-map town and
      F03 pins; M-key map follows the character.
- [ ] Local map: scroll Atlas ↔ Elysion page; only the current page's
      buttons/titles/stars show; guide window opens and closes.
- [ ] SkyCrossroads → Adrian → Soless → Memory unlock in order and enter at
      N/H/E; every stage loads (Soless hidden stage, phase-2 world).
- [ ] Adrian/Soless moving platforms carry the player and stay in sync with a
      party/bot.
- [ ] Soless `AWAKEN`/`HERBAON` and Memory `OF_SUN` gauges show and update;
      dramatic messages display.
- [ ] Clears pay out (levels 83–99) and unlock the next line.
- [ ] `grep -E "UNHANDLED|EXCEPTION" offline_packets.log` — none new;
      `ErrorLog.txt` has no new `KEM_ERROR*` for region files.

## Out of scope (with the reason)

- **Hero tier**, **seasonal variants** (`Elysion_Village_Festival`, `_XMAS`,
  `_XMAS_2`, rest-area variants), **Henir NPCs**, **CSM cards**,
  **`WINNER_Adrian` PvP map**, **dungeon lounge 30009**, **spa** — see the
  Atlas doc for reasons.
- **Everything quest-related**, including quest-driven NPC swaps' quest data
  (the mechanism is ported in E1), the Elysion clear movie (quest 22336,
  `Movie_Elysion_Clear.avi`, `datai015` subtitles), EventScene/GFx scenes
  (`Elysion.xml`, `GFXEventSceneInfo.xml`, `elysion*.swf`) →
  `ATLAS_PORT_PHASES.md` *Deferred to the quest plan*.

## Critical files (in addition to the Atlas doc's list)

- **Lua:** `march2014/data034/DLG_Map_House_NPC_RelationData.src.lua`,
  `march2014/data036/{ShopItem,PVP_Map_List}.src.lua`,
  `march2014/update/data034/{DLG_Elysion_LocalMap_Guide,DLG_UI_ELYSION_SOLESS_*_GAGE,DLG_UI_FIELD_MAP_Elysion_*}.lua`,
  `march2014/update/data039/UI_*_ELYSION.lua`.
- **C++:** `X2Lib/X2TFieldNpc*.cpp`, `X2Lib/X2LocationManager.cpp`
  (`HouseParsing`, `VillageParsing`), `X2Lib/X2QuestManager.cpp` (relation
  binding), `X2Lib/X2LocalMapUI.{h,cpp}`, `KTDXLIB/KTDGLineMap.{h,cpp}`,
  `X2Lib/X2World.cpp` (triggers), `X2Lib/X2WorldManager.cpp`
  (`AddWorldData_LUA`), `X2Lib/X2GUNPC.cpp` (dramatic messages),
  `X2Lib/X2Main.cpp` (bindings), the SimpleDLG owner A1b identifies.
- **Tools:** `june2017_re/june_lua/ELYSION_SOLESS.src.lua`.

## Appendix A — Elysion-half NPC_UNIT_IDs (June values)

- **SkyCrossroads (15):** `ELYSION_HERNA_ORB` 2260, `HERNA_BOW` 2261,
  `HERNA_LANCE` 2262, `HERNA_FEAR` 2263, `BOSS_ORB` 2264, `BOSS_SHIELD` 2265,
  `KEYOBJECT` 2266, `FAKE_OBJECT` 2267, `MOVE_OBJECT` 2268,
  `KEYOBJECT_TRIGGER` 2295, `WATERCANNON_OBJECT` 2296 (file extension is
  uppercase `.LUA`), `SKYCROSS_EVENT0` 2297, `SKYCROSS_BOSS_EVENT` 2298,
  `SKY_DUMMY` 2323, `DUMMY_TITLE` 2333.
- **Shared:** `ELYSION_HERNA_SHIELD` 2259 (SC + fields); `WALL_DUMMY1` 2331
  (SC + AR); `NEW_CHEST` 2332 (SC + AR + SL + fields); `EMPEROR_SHIELD` 2299,
  `EMPEROR_ORB` 2300 (AR + SL).
- **Adrian (34):** `ELYSION_HAMMER_PANG` 2271, `SING_PANG` 2272, `BOMB_PANG`
  2273, `PUPANG` 2274, `NEW_DEKAL` 2275, `BOSS_HERBAON` 2276, `DUMMY_HERBAON`
  2277, `L_H_LASER` 2278, `L_V_LASER` 2279, `R_H_LASER` 2280, `R_V_LASER`
  2281, `EMPEROR_BOW` 2301, `EMPEROR_LANCE` 2302, `2STAGE_LASER_A/B/C`
  2304–2306, `3STAGE_LASER_A/B/C` 2307–2309, `WALL_DAMAGE` 2310,
  `DAMAGE_OBJECT` 2311, `GAUGE_OBJECT` 2312, `GAUGR_DUMMY` 2313,
  `STAIRS_DUMMY` 2314, `ADRIAN_GATE` 2315, `ADRIAN_EVENT1` 2316,
  `ADRIAN_EVENT2` 2317, `ADRIAN_BOSS_EVENT` 2318, `OBJECT_HERBAON1` 2319,
  `OBJECT_HERBAON2` 2320, `ADRIAN_EVENT3` 2324, `3STAGE_BACK` 2328,
  `WALL_DUMMY0` 2330, `DAMAGE_OBJECT_STAGE2_SUB_STAGE2` 2335.
- **Soless (45):** `ELYSION_IMPRINT_SHIELD/ORB/BOW/LANCE` 2349–2352,
  `COMMANDER` 2353, `DIA` 2354, `SPHERE` 2355, `S_IMPRINT_SHIELD/ORB/BOW/LANCE`
  2356–2359, `S_COMMANDER` 2360, `S_DIA` 2361, `S_SPHERE` 2362, **`SOLESS`
  2363**, `SOLESS_EVENT1` 2364, `SOLESS_EVENT1_A` 2365, `SOLESS_DUMMY` 2368,
  `S_MAYA` 2369, `S_0STAGE_LASER1-4` 2370–2373, `S_DAMAGE_OBJECT` 2374,
  `S_CIRCLE_OBJECT` 2375, `S_CIRCLE_CTRL` 2376, `S_WALL_OBJECT` 2377,
  `S_COMMAND_3STAGE` 2378, `S_SKILL_3STAGE` 2379, `S_COMMAND_BOSS` 2380,
  `S_SKILL_BOSS` 2381, `S_3STAGE_OBJECT` 2382, `S_2STAGE_HERBAON` 2383,
  `S_DISION_OBJECT` 2384, `S_2STAGE_SUVEILLAN` 2385, `S_3STAGE_END` 2386,
  `BOSS_LINE_A` 2387, `HIDE_STAGE_A` 2388, `HIDE_STAGE_B` 2389,
  `S_0STAGE_GATE` 2390, `S_1STAGE_END` 2391, `S_0STAGE_START` 2392,
  `S_2STAGE_END` 2402, `IMPRINT_DYING` 2403, `SOLESS_DRAMATIC` 2404.
- **Memory (30):** `ELYSION_M_S_KNIGHT/LANCE/SWORD/MAGE` 2405–2408,
  `M_EL_KNIGHT/LANCE/MAGE` 2409–2411, `M_MEMORY_1..5` 2412–2416, `M_SOLESS`
  2417, `M_BOSS_EVENT` 2418, `M_0STAGE_START` 2419, `M_0STAGE_GATE` 2420,
  `M_1STAGE_START` 2421, `M_1STAGE_GATE` 2422, `M_3STAGE_START` 2423,
  `M_EL_DEF` 2424, `M_2STAGE_SUMMON` 2427,
  `M_S_{KNIGHT,SWORD,LANCE,MAGE}_SUMMON` 2428–2431, `M_0STAGE_GATE_2` 2432,
  `M_0STAGE_GATE_3` 2433, `M_1STAGE_START_1` 2434, `M_2STAGE_END` 2435,
  `M_3STAGE_END` 2436.
- **Village (12, no unit scripts):** `YUNO` 10078, `BERNARD` 10143,
  `DURANDA` 10144, `THEODOR` 10145, `HUGO` 10146, `ELYSION_QUEST_BIRD0-4`
  10148–10152, `ELYSION_RED_TREE` 10194, `ELYSION_BLUE_TREE` 10195.
- Reused Lanox NUIs in these lines: `SECRET_MAG_NOM` 2220 (AR),
  `SECRET_MAG_CHARGER` 2221, `SECRET_SUMMON_MAG_NOM` 2227,
  `SECRET_MAGI_OBJECT` 2249, `SECRET_LAVA_DUMMY` 2248 (SL).
