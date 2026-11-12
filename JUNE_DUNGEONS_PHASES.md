# June dungeons - phases

Created 2026-10-11 (user): the dungeon / world / map-entry ports that `june-effect` (C3y, C3z) collected are not effect-engine work,
so they run as their own plan. Engine fixes the dungeons need (NPC/game queries, intro skip, SimpleDLG) stay in
`JUNE2017_EFFECT_COMPLETION_PHASES.md`.

## Context

- Goal: every June dungeon and world that March lacks or has half of is playable offline, with June's rows, worlds, NPCs, meshes,
  map buttons and entry rules. Offline client only (shared-header changes flag-gated, server desync not a concern).
- **Phase IDs were kept** (C3y-1b ... C3z-5b, C3z-4t) so commit messages, JUNE_BUGS rows, census files and `funcmap.tsv` tags still match.
  New phases of this plan (for example the ones C3y-5 writes) take the next free ID in the same style, for example `C3y-5a`.
- Done phases that stay in june-effect as history: C3x (world load, 170 map files), C3y (census), C3y-1a (Hamel secret),
  C3z-1 (map assets), C3z-2 (old dungeon NPCs), C3z-3 (registry parity), C3z-5 (DungeonData keys), C3z-6 (relative level, poison).
- Commit message prefix: `June dungeons Phase <id>: <what>`. Log tag stays `JUNE_FX <id>` (existing code prints it).
- Where the text lives: every moved phase's task text and every earlier Done note is still in `JUNE2017_EFFECT_COMPLETION_PHASES.md`
  (section names below). **New Done notes go into this file**, under `## <id> - Done`, so the old doc stops growing.

## Rules

- All of CLAUDE.md and the standing memory rules: one `SERV_IRUHADEV_JUNE_*` flag per logical change in `KTDXLIB/Always.h`, original
  code in `#else`; June bugs kept verbatim with a `JUNE_BUGS.md` row; no new `.lua` files; June `.lua` arrives through
  `luasrc.py adopt-june`, core-list rows added by the phase that edits them; a missing June asset or key is a finding, never a hand patch.
- **Opus phases delegate the mundane work to subagents** (x2-phase skill, "When the phase is Opus").
- The port halves of C3y-1b / 1c / 2 / 3 / 4 share the core registries: one at a time.
- W3 packs `data040`, which those port halves also pack (`WorldData.lua`): one at a time with them.

## Starting context

- Tools, not hand commands: `python tools/build.py --deploy`, `flagcheck.py`, `logs.py --grep "JUNE_FX <id>"`, `kom.py`, `luasrc.py`.
- Censuses and gates: `june2017_re/c3y_unreached_dungeon_census.tsv` (+ `_world_` and `c3y_june_only_dungeon_ids.tsv`),
  `dungeon_npc_parity.tsv`, `dungeon_map_parity.py`, `dungeon_registry_census.py`, `c3z5_key_readers.tsv`.
- Method for a dungeon: DungeonEnum / DungeonData rows (data036 / data035, core) -> worlds (Enum `WORLD_ID`, data040 WorldData, files)
  -> NPC ids (`Enum.lua` + data037 `NPCTemplet.lua`, stat tables) -> meshes (`npc_mesh_census.py`) -> map button (June's icon file
  by `adopt-june`) -> emulator check. C3z-2's and C3y-1a's Done notes show it done.

## Phases

| Phase | Scope | Text lives in (JUNE2017_EFFECT_COMPLETION_PHASES.md) |
|---|---|---|
| C3y-1b | Henir Space's 5 missing boss worlds + the other Sander secret worlds (13 worlds); play-test + review owed | "C3y-1b - port: ..." and "C3y-1b - Done" |
| C3y-1c | the Sander secret dungeon (41141, 32 NPCs, map button, BEHEMOTHSOUL effect data) | "C3y-1c - port: the Sander secret dungeon" |
| C3y-2 | June's class tutorial dungeons (9 classes; identify the entry route first) | "C3y-2 - identify, then port: ..." |
| C3y-3 | Bestera 1-4 + `DI_BESTERA_START` | "C3y-3 - identify, then port: Bestera 1-4" |
| C3y-4 | Space of Thought 1-3 + `_DUMMY`, the Special-map line | "C3y-4 - port: Space of Thought 1-3" |
| C3y-5 | identify June's route into the Hero tier (read-only), then write the port phases | "C3y-5 - identify: ..." |
| C3z-1b | June's data101 map assets (Velder / Henir); waits for june-asset-port P0 | kickoff `C3z-1b.md`; "C3z-1 - Done" |
| C3z-4t | June's `CX2TutorialGuideManager` (the 9 Battle Field tutorial dungeons via `benders_actor.lua`); scope first: report the system's size before porting | kickoff `C3z-4t.md`; "C3z-4 - Done (batch 1)" |
| C3z-5b | the DungeonData readers C3z-5 routed (DCC bits, GetLastClearDungeonID, m_fExtraValue, June-only-row keys) | kickoff `C3z-5b.md`; "C3z-5 - Done" |
| C3z-8 | June's boss-raid systems (`CX2BossRaidLongTimeEvent` enter / state UI, `CX2NewBossRaidManager` + raid phases, June's `CX2BossRaidManager`); scope first, report size and offline reach, then write port phases. Routed from skill-system-core S7o (user 2026-10-19) | kickoff `C3z-8.md`; `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7o - Done" |
| W3 | June's village world Lua: census every village world script + line map in data040, stage the closure, adopt June's (ljconv), pack, ask, copy. Moved from june-engine by its scope audit (2026-10-20), same ID; Sonnet 5.5 high | `JUNE_ENGINE_REMAINDER_PHASES.md` "Phase W3 - June village world Lua"; kickoff `W3.md` |
| W3b | full investigation of EVERY village and rest area against June after W3 (user 2026-11-12: Elder / Besma NPCs and layout do not match, textures missing): layout, NPC positions, textures, then the unambiguous fixes; Opus 5.5 high | kickoff `W3b.md`; "Phase W3b" below |
| W3c | June's field graph (village / rest / field links, BattleFieldData, MapData BEFORE / AFTER); identify, user decision, port | kickoff `W3c.md`; "Phase W3c" below |
| W3d | June's village NPC roster, services and talk (houses June lists that March does not, June-only houses / places, service + talk keys) | kickoff `W3d.md`; "Phase W3d" below |

Run with: Opus 5.5 high (C3y-5: xhigh). Kickoffs: `kickoffs/june-dungeons/<id>.md`. Board: `STATUS.md`, block `june-dungeons`.

## Phase W3b - every village and rest area against June (user 2026-11-12, after W3)

**Run with:** Opus 5.5 high, then a user play-test. Read-only census first; fixes only where the cause is proven. No code flag unless the census
finds a C++ gap (that becomes its own flagged phase, never an improvised edit). Log tag `[JUNE_ENGINE] WVIL`.

**Why.** W3 (Done note at the end of this file; MODS.md "June villages and rest areas..." and "data101 restored...") adopted June's world scripts and
line maps for villages and rest areas, staged 43 assets, restored `data101` and moved some NPC rows. The user then reported, travelling through
Elder and Besma: NPC positions and village layout do not match, some textures are missing. W3's check was too narrow, so this phase re-checks
EVERY village and rest area, not only those two: Ruben, Elder, Besma, Altera, Peita, Velder, Hamel, Sander, Ranox, Elysion and the rest areas
Elder, Besma, Altera, Peita, Velder, Hamel, Sander, Ranox, Elysion, Atlas Station (1008), plus the Arcade rest.

**Evidence already in hand (verified 2026-11-12, `ErrorLog.txt` of the run at 8:48):**
- `OpenTexture` failures for Besma village: `BESMA_VILLAGE_BRIDGE_FLOOR03`, `_ALCHEMIST_ROOF_01`, `_ALCHEMIST_BUILDING_01`, `_ALCHEMIST_COLUMN_01`,
  `_ALCHEMIST_OBJECT01`, `_ALCHEMIST_OBJECT02`, `_DISPLAY`, `_ACC_OBJECT01` (all `.dds`). March's live koms lack them, June's have them. Also
  `FIELD_ELDER_ROADTILE2.DDS`, `FIELD_DRAGON_ROAD_ROAD02_1B.DDS`, `RUBEN_BANK_CLOSE`, `RUBEN_TECHHOUSE_GANPAN`, `RUBEN_VILLAGE_HERB01` / `02` (check
  whether each is new since W3). `OpenXET` failures are noise (every mesh logs one).
- Likely cause of the textures: W3's closure followed Lua -> meshes March lacks or differs on, but never opened meshes March already had unchanged
  whose textures March lacks (June's new objects, for example the Besma Alchemist buildings). Real closure = Lua -> every mesh -> every texture
  inside the mesh (inflate `.Y` with `june2017_re/fxdata.xfile_text(lenient=True)`; method in `june2017_re/npc_mesh_census.py`: `jbytes`, `tokens`, `resolve`).
- `DLG_Map_Village.lua` `START_POS` is still March's: Elder StartPosId 20 x = -8041 (June -4780), id 22 x = -8499 (June -5071); June's Elder world is
  laid out differently, so spawn / warp-in positions no longer fit. June uses the keys `OtherPlaceID` / `OtherPlaceStartLineIndex` where March's file
  uses `BattleFieldId` / `BattleFieldStartLineIndex` (deliberate Atlas A5 choice; reader `X2Lib/X2LocationManager.cpp` ~455-490); the meaning differs
  for some rows (Besma rest 311: June `OtherPlaceID = VMI_BESMA` line 41, March `BattleFieldId = BESMA_FIELD_01` line 2; Altera 62 line 314 vs 2; Peita 72 line 312 vs 2).
- W3 synced only `COMMON_NPC_LOCATION` rows in `DLG_Map_House.lua` (9 moved, 29 added). Other per-house keys that decide where an NPC stands or where
  its mark / join area is were not diffed.
- Village `HouseIDList`s lack June's `HI_ADAMS_HOUSE`, `HI_MU`, `HI_GUILD_MARK`, `HI_EVENT_*` (those house blocks exist in March; `HI_SPA_SIGN_RUBEN`
  and `HI_PANE_PEITA_HOUSE` do not).

**Steps.**
1. **Census, all places** (`june2017_re/w3b_village_census.tsv`, one row per village / rest area / check). For each place: world script and its
   seasonal variants, line map, `WorldData.lua` row, `Village[...]` block (every key, not only `START_POS` and `HouseIDList`), every house block of its
   `HouseIDList` diffed key by key (June `june_src_fixed/data034_kom/DLG_Map_House.src.lua` and `DLG_Map_Village.src.lua` vs the staged March files),
   the minimap / map-window Lua (`DLG_Map_Village_*_Back/Front/House_Button.lua`, `DLG_Map_Local*.lua`, `MapData.lua`), and the full texture closure
   above, compared by NAME and by BYTES against March (staged and live; `data101` is restored, the staged dir equals live bar 2 empty entries).
2. **Prove the causes** before changing anything: read the log lines, diff tables and the C++ readers (`X2TFieldGame.cpp` `AddFieldNpc` ~3140-3157,
   `X2LocationManager.cpp` `VillageParsing`). June's meaning of `OtherPlaceID` / `OtherPlaceStartLineIndex` is read in Ghidra (skill `ghidra`) where the
   March reader cannot carry it.
3. **Fix what is unambiguous:** stage every June texture / mesh the closure needs (`kom.py stage-june`), June's value for every per-house and per-village
   key March's reader supports (core-list BOM source: edit, `luasrc.py refresh data034 <file>`, `ledger --check`, `core --check`), pack and copy the koms
   (`/elstools pack`, `/x2-kom-packed copy`, no ask).
4. **Route the rest:** a June key or semantic March's C++ cannot read, the missing houses, `HouseIDList` additions whose NPC assets are absent: each gets
   its own bullet in the plan or a new phase with the evidence, deferred and never dropped.
- **Gate:** census at 0 unexplained rows; the texture closure of every adopted world script at 0 missing; `ledger --check` and `core --check` PASS;
  `kom.py pack` of each touched kom shows only the intended changes against live; the Done note lists what is routed.
- **Play-test (user):** unit on the existing offline save; walk each village from the door to every edge, then each rest area: NPCs stand where their
  house mark says, nothing floats or sinks, no pink / black textures, spawn and warp-in points sit on the ground. `python tools/logs.py --grep "OpenTexture"`
  shows none of the names above.

## Phase W3c - June's field graph (routed by W3b, 2026-11-12)

**Run with:** Opus 5.5 high; identify, then a user decision, then the port. June links Elder - Besma - Altera - Peita differently: Besma's east
portal leads to the Besma rest area (line 310), and the Besma rest area leads to Besma (line 41), Peita (line 108) and Altera (line 60) instead of
Besma fields 1-3. W3b found it in `DLG_Map_Village.lua` START_POS (Besma 42 / 43, Altera 62 / 63 / March-only 67, Peita 72, Besma rest 311 / 313 /
315; `w3b_village_census.tsv` rows `routed W3c`). It goes with June's `BattleFieldData.lua` portals and world ids and `MapData.lua` `AddMapData`
BEFORE / AFTER. March's reader already carries village / rest ids as `BattleFieldId` (`X2StateField.cpp` ranges 20000-20010 / 1000-1008), so this
is a data port. **Decision (user, 2026-11-12): June's route**, after the W3b play-test showed the half-installed route (boards, Altera's
bottom path, Elder rest sign; detail in `W3c.md`). Kickoff `W3c.md`.

## Phase W3d - June's village NPC roster, services and talk (routed by W3b, 2026-11-12)

**Run with:** Opus 5.5 high, then a user play-test. The houses June lists that March does not (`HI_GUILD_MARK`, `HI_ADAMS_HOUSE`, `HI_MU`,
`HI_EVENT_GUILD_EDAN_VILAGE`, `HI_EVENT_BLESS_OF_SOLACE` in every village; June-only `HI_PANE_PEITA_HOUSE`, `HI_SPA_SIGN_RUBEN`; Ruben gate `HI_APINK`;
Ruben drops `HI_POSTBOX`), the 18 June-only houses and 3 June-only places (spa Ruben / Ranox, guild agit), and the NPC service and talk keys June
changes in ~40 houses. Each needs its assets, its `AddFieldNpc` gate and its offline system checked before June's value goes in. Kickoff `W3d.md`.

## Phase W3e - June's field worlds and field rows (routed by W3c, 2026-11-12)

**Run with:** Opus 5.5 high; identify, port what is safe, then a user play-test. W3c ported June's field graph and the route fields' assets that
do not move the ground. Left: (1) Ruben field 1 is rebuilt in June (`Field_Ruben_Stage0_Linemap.lua` 53 lines vs 116, 10 meshes, BattleFieldData
portal line 115 -> 51, 21 -> 10 monsters), but March's server spawn table `Ruben_Field_00.lua` walks lines up to 109 and June's copy is not in the tree;
(2) June's line maps for Velder field 1, Sander field 4, Lanox fields 1-4 and the Sander field 3 sign `field_sander_stage2_Guidealpa.Y`, each checked
against its spawn table and portal lines first; (3) BattleFieldData columns that are not the graph: `REQUIRE_LEVEL` on 26 rows (no March reader),
Elder fields 1 / 2 middle bosses (June has none), `MAX_JOIN_USER_COUNT`, the Velder ship `REQUIRE_DUNGEON`. Boss-raid parts (third portals in Hamel /
Sander fields, `RAID_FIELD_PORTAL_POSITION_INDEX`, the Nasod raid row, `raid_ssky.dds`) stay with C3z-8. Kickoff `W3e.md`; evidence
`june2017_re/w3c_field_closure.tsv`.

## Phase W3f - March's route on June's villages (user 2026-11-12, after W3c)

**Run with:** Opus 5.5 high, then a user play-test. **User decision (2026-11-12): June's villages and placement stay, but March's route and
March's sign boards come back, so no field is cut off.** It replaces W3c's route choice and, for the route's boards and line maps only, the
"June's same-name asset replaces March's" rule. Undo W3c's route rows (each carries `-- june-dungeons W3c` with March's value): data034
`DLG_Map_Village.lua` (Besma 42 / 43, Altera 62 / 63, Peita 72, Besma rest 311 / 313 / 315, restore Altera 67) and data036 `BattleFieldData.lua`
(Elder field 1's far portal back to the Elder rest 300). Then put March's copy back for every route board June re-labelled, from the stage backups
(`kom_backup_stage_june/20261112_*`): candidates `Besma_Village_Alpha_Text.Y`, `Field_Peita_Villeage_sign.Y`, `Field_Elder_StageRest_alpaguide.Y`,
`Field_Besma_Rest_G01_Navi.Y`, `Altera_Village_Typo.Y`, and whatever texture each names; and March's `LINEMAP_Altera_Village.lua` (backup
`20261112_083532`) for the bottom walkway to Altera field 3, once it is shown to fit June's Altera world. Keep the W3c field-change ACK fix,
the Lanox rest MapData row and the Altera / Peita field assets. Kickoff `W3f.md`.

## Order (recommendation, 2026-10-11)

1. C3y-1b play-test (user) and review; 2. C3y-1c; 3. C3y-2 / 3 / 4 one at a time (identify halves can run earlier);
4. C3z-5b best before or with them; C3z-4t (scope first) pairs with C3y-2, the class tutorials; 5. C3y-5 any time (read-only); 6. C3z-1b when P0 has restored data101.
7. C3z-8 (added 2026-10-19): its scope half is read-only and runs with anything; port halves wait for the user's look at the scope and, where they need June's
   `BattleFieldData.lua`, for atlas A6b.
8. W3 (added 2026-10-20, from june-engine): any time no other session packs `data040`.

## W3 - Done (2026-11-12, Sonnet 5.5)

Scope grew by the user's request: village / rest-area world scripts and assets to June, Lanox rest NPCs, field-name banner, Sander last-field sign.
- Census: 74 village / rest world scripts + line maps + variants; 13 differed. `june2017_re/w3_village_assets.tsv` = the staged closure (43 files); ~65 `data101` Velder meshes left for C3z-1b.
- `data034` `DLG_Map_House.lua`: 9 positions to June, 29 rows added (Lanox rest 4). `ledger --check` and `core --check` PASS.
- `data057` Fieldname sheets 1 and 6 and `data135` / `data030` sign meshes: June's.
- PACK OK + COPY OK: data024 026 027 028 029 030 031 034 040 057 110 135. Full build skipped: no code change.
- **Left:** `DLG_Map_Village.lua` `HouseIDList` / `START_POS` (June's rest areas arrive from the village; Adams / Mu / guild houses).
- **data101 (follow-up, same day):** dir restored byte-exact from the live kom (1,205 files), packed, copied (md5 3d798391); the Velder village meshes already match June except `Velder_village_ADplane.Y` (staged). Two 0-byte live entries (`Velder_Field_001/003.dds`) dropped. MODS.md has the detail.
- **Play-test owed:** every village loads, NPCs stand on the ground; Lanox rest area shows Hellen / Postbox / Billboard / Glave; entering Lanox field 1 shows the Lanox banner; Sander field 4 ends with the Lanox board.

## W3b - Done (2026-11-12, Opus 5.5)

Data only: no client source, no flag (flagcheck / junediverge not applicable; full build skipped: no code). Census `june2017_re/w3b_village_census.tsv`
(249 rows, 0 unexplained; `python june2017_re/w3b_village_census.py`), closure `june2017_re/w3b_asset_closure.tsv` (`w3b_asset_closure.py`: every
`Village[]` world incl. gates, lounges, rest areas, Atlas Station 1008, plus the Arcade and Peita rests; WorldData row -> every world `.lua` and variant ->
line map, meshes, sounds -> textures / XET / TET inside each mesh, June's copy, compared by bytes; `.lua` against ljconv(June)). New readers
`june2017_re/luatable.py` (data-only Lua tables).
- **Causes proven.** (1) Elder / Besma NPCs: W3 adopted June's worlds but six houses keep their spot in `NPCMeshPos` (no `COMMON_NPC_LOCATION`):
  Richang / Stella stood at y 3910 under June's Besma at y ~5500; Echo / Hofmann / Lenphad / Luichel at March's Elder spots. (2) Elder spawn points 20 / 22
  were March's x. (3) Textures: the closure W3 ran skipped meshes March already had and the NPC models; 52 June files were missing or older.
- **Fixed (data034 core source, `luasrc.py refresh`, `ledger --check` PASS, `core --check` PASS):** `NPCMeshPos` of the six houses, `JoinArea` of Grail /
  Hofmann / Richang, Ariel event house `NPCAniTex`; Elder StartPosId 20 / 22 x; Hamel lounge 87 -> 88 (March duplicated Velder lounge's id).
- **Staged:** `w3b_stage_assets.tsv` (52: 31 + 1 NPC model / XET / face files in data013, 6 data023, 2 data024, 4 data026, 2 data029, 3 data054, 2 new
  data056, 1 data110), `adopt-june` Elysion_Village_Festival / _XMAS (new; `luabind.py check` PASS). After the copy the closure shows 0 differ, 0 missing
  (2 `missing-both`: `Ruben_Dungeon_Gate.lua`, `Motion_NPC_Elsion_Hugo.XET`, June lacks them too). June's NPC models keep March's animation names
  (Wait01-03 / Talk / Bye); Billboard and Postbox become KSM, which March loads.
- **PACK OK + COPY OK:** data013 (md5 16766c83), 023 (a1b10494), 024 (dc4ca09b), 026 (d477b250), 029 (afbe34e5), 034 (fffab949), 040 (8ee149cd),
  054 (1aefda2f), 056 (dcb9947f), 110 (a684e35e). Each pack showed only the intended entries against live.
- **Log textures (run 2026-11-12 8:48):** `RUBEN_BANK_CLOSE`, `RUBEN_TECHHOUSE_GANPAN`, `RUBEN_VILLAGE_HERB01/02`, `FIELD_ELDER_ROADTILE2` are named by
  June's own meshes and June ships no such file (parity, kept). **Open:** the Besma Alchemist / Display / Acc-object / Bridge_Floor03 / Road02_1b textures exist only in June's data028, yet neither June's
  Besma meshes, Besma script nor March's old G01 name them; the full scan after the copies (every .x / .y / .lua / .tet / .xet of all 258 June koms
  and every live kom) found no file naming them. The play-test's `OpenTexture` grep decides whether a follow-up is needed.
- **Kept on purpose:** START_POS rows that differ only by key name (June `OtherPlaceID` = March `BattleFieldId`, atlas A5); `COMMON_NPC_LOCATION` row order.
- **Routed:** W3c (June's field graph: Besma 42 / 43, Altera 62 / 63 / 67, Peita 72, Besma rest 311 / 313 / 315 lead elsewhere in June, with
  BattleFieldData and MapData BEFORE / AFTER; a design decision); W3d (village NPC roster: Adams, Mu, guild mark, EDAN / Bless of Solace, Pane, spa sign,
  Ruben gate Apink, Ruben drops Postbox; 18 June-only houses, 3 June-only places; service and talk keys in ~40 houses; `DLG_Map_House_1/_2`);
  Stage 9 bullet (region-map dialogs `DLG_Map_Local_*`, MapData `AddLocalMapInfo`). The village minimap dialogs already equal June's.
- **Play-test (user, 2026-11-12):** Besma alchemist house still untextured; boards at Altera end / Peita start / Elder rest end wrong or blank;
  Altera bottom portal dead; Cobo list without Elder / Altera rest. **Follow-up (same day):** `fxdata.xfile_text(lenient)` stopped at the first
  MSZIP block zlib rejects (June's `Besma_Village_G01.Y` block 29 of 55), hiding half the mesh; it now pads that block and reads on. The rerun found
  the 9 Besma textures (`Alchemist_Roof_01` / `_building_01` / `_Column_01` / `_Object01` / `_02`, `Display`, `Acc_Object01`, `Bridge_Floor03`,
  `Field_Dragon_Road_Road02_1b`), staged new in data028 (backup `20261112_095755`), PACK OK + COPY OK data028 (md5 64674884). Closure 5,760 rows, 0
  differ / missing; census 0 unexplained. Boards and the Altera portal are June's route half-installed: user chose June's route -> W3c. Cobo list
  equals June's (June's dialog has no Elder / Altera rest rows).

## W3c - Done (2026-11-12, Opus 5.5)

Data only: no client source, no flag (flagcheck / junediverge not applicable; full build skipped: no code, the deployed exe is unchanged).
User decision taken before the phase: June's route. Graph read from June's `DLG_Map_Village` / `BattleFieldData` / `MapData` (`june_src_fixed`).
- **June's route (now live).** Ruben - Ruben field 1 - Elder - Elder field 1 - Besma - Besma rest - { Peita, Altera }; Peita - Peita fields 2 / 3 -
  Velder as before. Besma east (43) -> Besma rest StartPos 310; Besma rest 311 -> Besma 41, 313 -> Peita 108, 315 -> Altera 60; Altera 62 / 63 ->
  Besma rest 314; Peita 72 -> Besma rest 312; Besma west (42) -> Elder field 1 line 2; Elder field 1's far portal -> Besma StartPos 40 (was the
  Elder rest 300). March's Altera StartPos 67 (bottom walkway -> Altera field 3) removed: June has no row and its line map drops that walkway.
- **Edited (core-list source, `luasrc.py refresh`, `ledger --check` PASS, `core --check` PASS):** data034 `DLG_Map_Village.lua` (8 rows + 67 removed,
  March key names kept, a one-line `-- june-dungeons W3c` comment with March's value on each); data036 `BattleFieldData.lua` (Elder field 1 portal 2);
  data036 `MapData.lua` (June's Lanox rest `AddMapData`, MapData.lua:331-342; `AddMapData` is a no-op offline, `X2OfflineMapData.cpp:155`).
  The 20 shared `AddMapData` rows already equalled June's (BEFORE / AFTER included); the Cobo zone / cost functions equal June's but for the
  June-only `VMI_SPA_*` / `VMI_GUILD_AGIT` ids (-> W3d). After the edit every START_POS route equals June's bar Ruben 5 (spa, W3d).
- **Reader check.** Village / rest targets take `EGS_STATE_CHANGE_FIELD_REQ` (`X2StateField.cpp:2139`: 20000-20010 / 1000-1008; Besma 20002, Altera
  20003, Peita 20005, Besma rest 1001), and the "line index" there is a StartPosId (`GetStartPosLoc`, `X2StateField.cpp:2150`).
- **June quirk kept:** Besma rest 313 -> Peita StartPos 108, which neither June nor March defines; `GetStartPosLoc` returns (0, 0, 0) in June too
  (`324f1fd0`, funcmap). `JUNE_BUGS.md` #623.
- **Stranded by June's route (as in June):** Elder field 2 + Elder rest (only reach each other), Besma fields 1-3, Peita field 1, Altera fields 1-3,
  Altera rest; Cobo cannot reach them either (June's `DLG_Destination_byButton`, live since atlas A5, has no Elder / Altera rest row). Quest check (live
  `FieldQuest` / `FieldSubQuest` = June's, adopt-june dry run "identical"): 63 June sub-quests name those fields; only quest 13000 "[Field] Under
  Cover of Night" (Elder field 2) uses any, and nothing leads to it (June chains 12070 -> 13010); no save holds it (`unit_quest`). No dungeon entry
  sits in a field. So nothing reachable is stranded.
- **Field assets.** New `june2017_re/w3c_field_closure.py` (W3b's closure over every BattleFieldData world + line map): 5,199 rows, 23 differ,
  1 missing. Staged the route fields whose ground does not move: `w3c_stage_assets.tsv` (Altera field 1: `Field_Altera_Stage0_Flying_Boat_E` /
  `_Grass_D` / `_Grass_E` / `_Sky_Land_BB.Y`, data032; Peita field 2: `field_Peita_stage1_bluegate.dds`, data030); backup
  `kom_backup_stage_june/20261112_102612`.
- **Not ported, routed W3e:** Ruben field 1's June rebuild (line map 53 lines vs 116, 10 meshes, portal line 51) because March's server spawn table
  `Ruben_Field_00.lua` walks lines up to 109 and June's copy is not in the tree, so Ruben field 1 stays March's whole (its link to Elder is the same);
  June's Velder 1 / Sander 4 / Lanox 1-4 line maps and Sander 3 sign; BattleFieldData's non-graph columns (`REQUIRE_LEVEL`, Elder middle bosses,
  join counts, Velder ship gate). Boss-raid portals / rows -> C3z-8.
- **Elder rest end sign:** `Field_Elder_StageRest_alpaguide.Y` and its texture `Field_Elder_Type1.dds` ("Twin Watchtower", the Elder field 2 board)
  are byte-equal to June's (W3b closure `same`), so what shows there is June's look.
- **Census:** `w3b_village_census.py` re-run: 248 rows, 0 unexplained, 0 `routed W3c`.
- **PACK OK + COPY OK:** data030 (md5 d61905ba), data032 (604d6989), data034 (3c785a26), data036 (ff70b7d4); each pack changed only the intended
  entries. Revert: copy back `kom_backup_tools/data0{30,32,34,36}.kom.20261112_1029*`, restore the staged files from the stage backup, and undo the
  commented rows in the three Lua (`luasrc.py refresh` each).
- **Follow-up (W3c play-test, same day): code fix `SERV_IRUHADEV_JUNE_FIELD_CHANGE_ACK`.** Besma east -> Dicey Sky Road (the Besma rest, 1001) and
  Dicey Sky Road -> Besma drew only the sky and froze the unit; a full load into the rest was fine. Cause: the portal sets
  `m_bNowMovingToBattleField` (`X2StateField.cpp:2164`, June +0x22f) and `CX2StateField::OnFrameMove` returns early while it is set (`:277`); March's
  `Handler_EGS_STATE_CHANGE_FIELD_ACK` never clears it, because March's village portals only led to fields. June `32509660` clears it and sets
  `m_bEnableShortCutKey` (+0xc4, from March's layout probe) first (@325096ac / @325096b2); both are now at the top of March's handler. The rest of
  June's body equals March's: `SetVillageName` is inlined, and E1d already ported the failure-path setter `323aa1b0( -1, 3.0 )`. **Not ported:** June's
  third error branch `0x654` (quest-gated entry: reads a 4th / 5th ACK int as the quest id, string 36141, `UI_PopUp_Negative_01.ogg` box). It is the
  server's reply to the quest gate the user chose not to stand in (2026-10-19, `JUNE_QUEST_SYSTEM_PHASES.md`), so the emulator never sends it; March
  has no matching NetError. Divergence row added (`june_march_divergent.tsv`). flagcheck `--worktree` PASS (X2StateField.cpp PASS, Always.h OTHER
  = a peer's lines), junediverge PASS. My own run was blocked by char-ara CH4b's in-progress `Always.h` guard; char-ara's build `.build/logs/20261112-110130` started after this edit (10:58), compiled X2StateField.cpp with 0 errors: DEPLOY OK 2026-11-12 11:12:37, md5 66ee9277.
- **Play-test (user, 2026-11-12, exe md5 66ee9277): passed.** In-place portals draw the new place and the unit moves. Logs: portals 43 -> 1001, 315 -> 20003, 62 / 63 -> 1001, 313 -> 20005, 74 -> 40010 fired; every village / rest move one `EGS_STATE_CHANGE_FIELD_REQ` + ACK; 0 EXCEPTION, no "isepi wrong". User then chose March's route on June's villages -> W3f.

## W3f - Done (2026-11-12, Opus 5.5)

Data only: no client source, no flag (flagcheck / junediverge not applicable; full build skipped: no code, the deployed exe is unchanged; the
W3c fix `SERV_IRUHADEV_JUNE_FIELD_CHANGE_ACK` stays on). User decision (2026-11-12): June's villages, March's route and boards.
- **Route back to March (core-list source, `luasrc.py refresh` each, `ledger --check` PASS, `core --check` PASS).** data034 `DLG_Map_Village.lua`:
  the 8 `-- june-dungeons W3c` rows take March's target and line from their comment (Besma 42 -> Elder field 2 line 2, 43 -> Besma field 1 line 1;
  Altera 62 -> Besma field 3 line 2, 63 -> Altera field 1 line 1; Peita 72 -> Peita field 1 line 2; Besma rest 311 -> Besma field 1 line 2,
  313 -> Besma field 2 line 1, 315 -> Besma field 3 line 1), comments gone; Altera StartPos 67 restored (36174, 409, -193 -> Altera field 3 line 2).
  The file now equals the pre-W3c backup (`kom_backup_stage_june/20261112_w3b_data034`) bar W3b's 3 own position rows. data036
  `BattleFieldData.lua`: Elder field 1 portal 2 -> `POSITION_INDEX_TO_MOVE = 300`, `VMI_BATTLE_FIELD_ELDER_REST_00` (line 153 unchanged).
- **Boards.** Each board mesh's UV faces painted onto its textures (scratch renderer over the binary `.x`; textures from kom_contents). All five
  route boards were June's (staged by W3 / W3b) and June's names June's route, so March's copy is back, byte-exact:

  | Board | March names | June named | Restored from |
  |---|---|---|---|
  | data028 `Besma_Village_Alpha_Text.Y` | Bethagara Falls (Lv 21-22), Twin Watchtower | Dicey Sky Road, Wally's Memorial Bridge | `20261112_083749` |
  | data030 `Field_Peita_Villeage_sign.Y` | Pilgrim's Gateway, Pilgrim's Site, Feita | Dicey Sky Road, Pilgrim's Site, Feita | `20261112_083431` |
  | data027 `Field_Elder_StageRest_alpaguide.Y` | Wally's Memorial Bridge, Wally's Castle Gateway, Twin Watchtower | Bethma Village, Wally's Memorial Bridge | `20261112_083749` |
  | data029 `Field_Besma_Rest_G01_Navi.Y` | Bethma, Altera, Feita, Bethagara Falls, Toretugera, Leikiki, Pilgrim's Gateway + Lv | Bethma, Altera, Feita | `20261112_091758` |
  | data031 `Altera_Village_Typo.Y` | Toretugera Canyon (Lv 25-26), Graveyard of Purification | Dicey Sky Road | `20261112_083749` |

  Textures stay June's: the two W3b replaced (`Ruben_Dungeon_Gate_Font.dds`, `ruben_build_typo.dds`) keep March's layout (only "LV 1~9" and the
  coin differ), so March's UVs land on the same words; every other texture these meshes name is March's in both. Other replaced meshes checked for
  board textures: only `Besma_Village_Alpha_01/05/06.Y` (house-name sheet, not route) and Sander 3 (off route). June copies backed up to
  `kom_backup_stage_june/20261112_w3f_june_boards`.
- **Altera bottom walkway.** June's `LINEMAP_Altera_Village.lua` drops March's lines 28-30 (34583,480 -> 36264,409) and also trims line 3's end
  (34600 -> 34584, `CAN_DOWN = FALSE` dropped), moves line 26's start (30971 -> 30991), turns two `-2` links into `-1` and sets land height
  140 -> 0. Reported; **user chose March's whole** (2026-11-12): restored byte-exact from `20261112_083532/data040_kom` (March bytecode, md5 45112997).
  Altera field 3's way back (StartPos 68, 35812,409) sits on that walkway.
- **Census.** `w3b_asset_closure.py` re-run (5,760 rows: 5,752 same, 6 differs = the 5 boards, 2 missing-both); `w3b_village_census.py` (`GRAPH`
  note now "kept W3f: user hybrid", new `KEPT_MARCH_W3F` notes): 255 rows, 0 unexplained. Both closures (`w3b_asset_closure.py`,
  `w3c_field_closure.py`) now leave the kept-March names out of their `need_stage` lists, so a later `stage-june` run cannot undo W3f. The line
  map is not in the village closure (no world Lua names it by a `.lua` token); checked by hand above.
- **PACK OK + COPY OK** (each pack changed only its one entry): data027 (md5 1f3fad0b), data028 (ff3be268), data029 (7af719ae), data030
  (be0f60b1), data031 (9ad20a2e), data034 (fffab949), data036 (50f47529), data040 (0524a741). Revert: copy back
  `kom_backup_tools/data0{27..31,34,36,40}.kom.20261112_11452*`; in kom_contents restore the 6 files from `20261112_w3f_june_boards` and re-apply W3c's
  rows (its Done note), `luasrc.py refresh` each.
- **Kept:** the field-change ACK fix, the Lanox rest `AddMapData` row, W3c's staged Altera field 1 / Peita field 2 assets, Ruben field 1 March's.
  W3e's kickoff notes that every field is reachable again.
- **How to see it:** the play-test hand-off below (STATUS row). Log tag `[JUNE_ENGINE] WVIL`; the portal moves show as
  `EGS_JOIN_BATTLE_FIELD_REQ` (into a field) and `EGS_STATE_CHANGE_FIELD_REQ` (into a village or rest) in `offline_packets.log`.
- **Play-test 1 (user, 2026-11-12): every portal works.** Saved logs `24965799\logs_backup_w3f_20261112_120604`: 0 EXCEPTION; Elder field 1 ->
  Elder rest (1000) -> Elder field 2 -> Besma, Besma field 1 / 3, Altera fields 1 / 3 fired; Altera field 3 -> Altera landed on line 30 (the
  walkway). 2 UNHANDLED = `E_DISABLE_HB_CHECK_REQ` (old, not this phase). Two asks, done as a follow-up the same day:
  - **Walkway board missing (text only).** The signpost is a mesh of `Altera_Village_G02.Y` (395 verts, `Field_Altera_waysign.dds`, x 35793-36075);
    June's G02 drops it. Vertex diff March vs June G02: only the signpost + the step at x 34882-34979 (March) vs a crate at x 35000-35167 closing
    the ledge (June); the rest is the same geometry re-cut (120 vs 212 meshes), March also naming 6 textures June re-textured (all present).
    Restored March's G02 byte-exact (md5 baf1759e; June's copy in `20261112_w3f_june_boards`), matching March's line map.
  - **Cobo Express (B) lacked the Elder / Altera rests.** June's `DLG_Destination_byButton.lua` (datai014, core list) has no `Village_1000` /
    `Village_1002`; March's did (backup `20261009_141115`). March's two static blocks inserted at March's positions with a `-- june-dungeons W3f`
    line; `X2StateMenu::CreateWarpDest` (June `324627f0`, atlas A5) already adds a row when the control exists, and MapData's Cobo zones give
    1000 -> 3, 1002 -> 7 (`Handlers_Social.cpp` warp handler prices them). The Peita rest (1003) was in neither list.
  - `luasrc.py refresh` datai014; ledger / core check PASS; G02 added to `KEPT_MARCH` / `KEPT_MARCH_W3F`. PACK OK + COPY OK data031 (md5 1f20d388) + datai014 (cf326d55) (one entry
    each). MODS.md entry rewritten as a "MERGE MOD" note (user ask).
