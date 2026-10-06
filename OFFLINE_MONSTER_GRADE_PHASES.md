# Offline monster grade: the offline server ranks monsters like the real servers

Plan key `offline-grade` · branch `mods/june-buff-port` · flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE`
(`KTDXLIB/Always.h`; `_DEBUG` only for short-lived per-hit lines) · log tag `[OFFLINE_GRADE]`
(`offline_server.log` via `CX2OfflineLog::Server`) · kickoffs `kickoffs/offline-grade/` ·
board: the `offline-grade` block in `STATUS.md`. Created 2026-10-06.

## Context

June buff port 16-step3b2 ported the socket "damage to boss" / "damage taken from boss" terms
(June GetFinalDamage `@32286519`, user DamageReact `@32048845`). Offline they are dormant on
dungeons, because the emulator never sends a monster grade
(`KNPCUnitReq::m_cMonsterGrade`). These sockets matter for character builds. The user decided
on 2026-10-06 to fix this "the proper way": the offline server ranks monsters from the real
server's data and flow, with no client-side stand-in.

**User decisions (2026-10-06, in the planning session):**

1. **The socket test follows June's flow and is not this plan's code.** The 16-step3b2 review
   (`60d0571`, `c3e5271`) settled what June tests: unit-info `+0xdc == 1`. June writes that
   field only for an NPC ID with an ExtraNPCStat row, and the grade's source depends on the
   caller:
   - **Dungeon** (`327ef570`): NPCData `+0x44`, which `327edb60` (AddNPCData) copies from
     the packet's `m_cMonsterGrade`, and which `327ef71d` passes to the stat fetch.
   - **Battlefield:** `m_bShowBossName` (`32d528f0`) or `BossGroupId > 0` (`32d539c0`).
     This is June's own client code, already ported as `m_iExtraNPCStatMonsterGrade`, and
     it is not a stand-in.
   - `CX2Game::CreateNPC`: 0.

   This plan supplies the real server's grades. On dungeons, the grade travels through
   NPCData into the NPC's ExtraNPCStat grade, which is June's path. On battlefields and in
   rewards, the plan does what the CenterServer did.
2. **Rewards keyed on the grade get their own phase** (Phase 3): EXP/ED ×12 for a boss and ×4
   for a middle boss (an attributed NPC gets ×2/×3 first), boss ED in 10-20 piles, the
   special drops 70058 / 70057, and the battlefield factors.

## Rules

- **Offline only.** Shared-header changes stay client-side behind the flag; never ask about
  server desync. `KNPCUnitReq::m_cMonsterGrade` is already serialised with no flag
  (`CommonPacket.cpp:3151` PUT, `:3224` GET). So no `ServerDefine.h` second home is needed,
  and no packet struct changes.
- **One flag**, `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE`, defined in `KTDXLIB/Always.h`, for every
  client edit in every phase (the user's rule for this plan). The original code goes in
  `#else` wherever behaviour is replaced.
- **CP949 files** (`X2DungeonSubStage.cpp/.h`, `X2BattleFieldGame.cpp`) are edited with
  `tools/bytepatch.py` only. `X2Lib/Offline/*` is ASCII.
- **Follow the real server's flow and data.** Where the server needs data the client lacks,
  the CLAUDE.md rules apply: server Lua is the user's to pack, and the live DB beats
  `DataBase/` / `ScriptData/`. Ask; never invent a value.
- **June bugs stay verbatim.** JUNE_BUGS #52 (an NPC attacker's boss-damage result is 2.0)
  goes live for dungeon bosses with Phase 1. AI-party bots, summons and ally NPCs then deal ×2
  to a grade-1 dungeon boss that has an ExtraNPCStat row. State it in the Done note; don't
  fix it.
- **Hands off june-buff's code:** `IsJuneSocketBossGrade` (`X2GUNPC.cpp:1923`),
  `CX2GameUnit::GetFinalDamage`, `CX2GUUser::DamageReact`, and the battlefield ExtraNPCStat
  grade setters (`X2BattleFieldGame.cpp:786` / `:1000`). This plan feeds them; it does not
  edit them.

## Starting context

Every repeated step is a tool (`tools/README.md`); do not hand-type msbuild, unifdef or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py OFFLINE_MONSTER_GRADE_PHASES.md "<heading>"` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --worktree --files <files>` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag OFFLINE_GRADE` (x2-check-logs) |
| Stage scripts | `<game dir>/../kom_contents/data0NN_kom/Dungeon_*.lua`; 208 of the 227 carrying `MONSTER_GRADE` are KOG LuaJIT bytecode (`1b 4b 4c 81`), so decompile them with `./elstools` (`mar-2014` is safe) into the scratchpad |
| June client | Ghidra via the `ghidra` skill (read-only); `june2017_re/funcmap.tsv` rows `323752f0`, `32286482..32286541` already carry the grade-source notes |

The game dir is `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`. Never launch the
game; ask the user.

## Established facts (verified 2026-10-06)

Three read-only sweeps during planning (server, client, emulator). A phase that builds on one
of these still opens the cited line first.

**Real dungeon flow (the grade comes from the stage script).**
- `CXSLDungeonSubStage::SubStageData::FetchNPCData` reads `MONSTER_GRADE` with
  `LUA_GET_VALUE_ENUM`, default `MG_NORMAL_NPC` (`KncWX2Server/Common/X2Data/XSLDungeonSubStage.cpp:197`).
- Its `bScriptCheck` block (`:221-246`) fails the load in two cases:
  - `HAVE_BOSS_GAGE` is true but the grade is not 1 or 2;
  - the grade is 1 without `HAVE_BOSS_GAGE`.

  `CXSLDungeon::IsScriptCheckDungeon` (`XSLDungeon.cpp:1229`) skips it for Henir, event and
  test dungeons.
- `CXSLDungeon::GetNPCData` copies the grade at `XSLDungeon.cpp:391`
  (`kNPCUnitReq.m_cMonsterGrade = (char)pNPCData->m_MonsterGrade;`). That is the live branch
  on US (`SERV_DUNGEON_RANDOM_NPC_GROUP`, ServerDefine.h:3466); `:545` is the dead `#else`.
  `:402-405` also marks the stage a boss stage on grade 1.
- Live handler: `DungeonRoom.cpp:3400`, under `SERV_BATTLE_FIELD_SYSTEM`:
  - `:3721` calls `GetNPCData`;
  - `:4061` calls `CreateMonster` (`RoomMonsterManager.cpp:75/78`: `m_bIsBoss = grade == 1`,
    and the grade is stored);
  - `:4180` broadcasts.

  GameServer only relays (`GSUserDungeon.cpp:100-105`). The commented
  `DungeonRoom.cpp:4949-4952` lines belong to an old `#else` copy of the handler.
- The server reads the **same `Dungeon_*.lua` files as the client** (CenterServer
  `config_cn_US_SERVICE.lua:40-41` adds a ClientScript path; no `Dungeon_*.lua` exists under
  `KncWX2Server`). No grade-by-NPC-ID table exists anywhere on the server: `NPCUnitTemplet` has
  no grade, and only `XSLDungeon.cpp` and `XSLBattleField.cpp` write `m_cMonsterGrade`.
- NPCs the client requests (`ERM_NPC_UNIT_CREATE_REQ`, `DungeonRoom.cpp:5803`) keep the grade
  the client sent. Other server-made NPCs keep 0.

**Real battlefield flow (the grade is hard-coded per spawn path, `XSLBattleField.cpp`).**
- Normal spawn `MakeMonsterInfo`: 0 (`:1426`). Field event monster: 0 (`:1537`).
- Middle boss `MakeMiddleBossMonsterInfo`: 2 (`:1003`).
- Boss-raid groups `GetCreateBossMonsterList`: 1 (`:724`). These are `IS_BOSS_MONSTER_GROUP`
  groups (`:322`, `:429`) on fields with `IS_BOSS_RAID_FIELD` (`XSLBattleFieldManager.cpp:228`).
- Event boss: dead on US (`SERV_BATTLEFIELD_EVENT_BOSS_INT` is not defined).
- The middle-boss list is the `SPAWN_GROUP_ID` table in `BATTLE_FIELD_MIDDLE_BOSS_INFO`
  (`:187-247`, the table read at `:195`). Without EVENT_BOSS_INT, entries with
  `SPAWN_ID > 100` also join it (`:246`).
- Death: `BattleFieldRoom.cpp:1783` calls `OnNpcUnitDie(..., cMonsterGrade, ...)`, and `:5804`
  adds danger × `GetMonsterTypeFactor`.

**Server reward consumers.**
- `GetMonsterTypeFactor` (`XSLUnitManager.cpp:238-269`): the attribute check comes first
  (2 on Normal/Hard and 3 on Expert, even for a boss). Then middle boss 4, boss 12, anything
  else 1.
- Dungeon kill: `DungeonRoom.cpp:6177/6182` turns that factor into `fDungeonFactor`.
  `KDropTable.cpp:1232` computes `m_iExp = iDropEXP * fContribution * fDungeonFactor * fLevelFactor`,
  with ED the same (`:1239-1244`).
- Boss ED is split into 10-20 piles instead of 2-3 (`KDropTable.cpp:1252`, keyed on `m_bIsBoss`).
- `KDropTable::NpcSpecialDropItem` (`KDropTable.cpp:1873`, `SERV_KILL_BOSS_MIDDLE_BOSS_DROP_ITEM`)
  drops 70057 at 50 % for a middle boss, 70058 always for a boss, and 70057 for the HP-ball grade.
  It is called from `DungeonRoom.cpp:6471` and `BattleFieldRoom.cpp:5270`.
- Battlefield: `GetBattleFieldMonsterTypeFactor` (`XSLUnitManager.cpp:274-293`) gives attribute
  2/3, middle boss 4, boss 12, else 1. It multiplies `fFieldEDFactor` / `fFieldEXPFactor` at
  `BattleFieldRoom.cpp:4398/5045/5930`.
- `KDungeonMonsterManager::IsAllSubStageNpcDie` (`DungeonMonsterManager.cpp:157`): if the
  sub-stage has bosses, only bosses are checked, otherwise only normal NPCs; middle bosses
  never are.

**Client.**
- The game's `FetchNPCData` (`X2Lib/X2DungeonSubStage.cpp:1577-1647`) reads `HAVE_BOSS_GAGE`
  (`:1609`), `SHOW_BOSS_NAME`, and so on, but never `MONSTER_GRADE`. Only the X2TOOL
  `FetchNPCData4Tool` reads it (`:1715`).
- The game-build `NPCData` (`X2DungeonSubStage.h:115-202`) has no grade member and **no
  constructor**. `m_MonsterGrade` and its enum are X2TOOL-only (`:46-56`, `:149`).
- `SubStageData::AddNPCData( KNPCUnitReq& )` (`:2156-2200`) does not copy `m_cMonsterGrade`.
  Its caller chain is `X2StateDungeonGame.cpp:2840` → `X2Dungeon.cpp:299`.
- `CreateGUNPC( const NPCData* )` (`:2389-2446`) sets no grade. `SwapToGameNPCUnit` (`:360`)
  calls it at `:375`, then `SetNpcHardLevel` at `:383-405`.
- `CX2GUNPC::MONSTER_GRADE` (`X2GUNPC.h:231-239`): `MG_NORMAL_NPC 0`, `MG_BOSS_NPC 1`,
  `MG_MIDDLE_BOSS_NPC 2`, `MG_DEFINITELY_DROP_HP_BALL 3`. The client registers no Lua table
  for it, so scripts use plain integers.
- `SetMonsterGrade` keeps only 2 (`X2GUNPC.cpp:32110-32116`), and its only caller is the
  battlefield constructor (`:1915`). `GetIsMiddleBosRaidNPC` has no callers. `BOSS_RAID_NPC`
  in the NPC's own Lua sets `MG_BOSS_NPC` (`:4548-4550`).
- `m_iExtraNPCStatMonsterGrade` is read only in `SetHardLevel` (`X2GUNPC.cpp:7210`): grade 1
  or 2 gives scan 50 / near range 1000, otherwise 10 / 500. The dungeon path never sets it.

**Emulator.**
- `CX2OfflineServer::BuildStageNpcData` (`X2Lib/Offline/Handlers_Room.cpp:1700`, called from
  `Handler_EGS_DUNGEON_STAGE_LOAD_REQ` `:1969`) reuses the client parser through
  `CreateDungeon( id, true )`, then copies NPCData into `KNPCUnitReq` (`:1813-1884`). The grade
  stays 0 from `Init()`. The comment at `:1886` says the scripts carry no grade, which is
  wrong: it is the parser that drops it.
- `X2OfflineBattleField.cpp` sends grade 0 from `PickFromGroup` (`:764`) and 2 from
  `MakeMiddleBoss` (`:927`).
- `AddBattleFieldData_LUA` reads the middle-boss group IDs from `BeginTable( "SET_BOSS_GROUP_NAME" )`
  (`:516-523`). But in the data that key is a string ID and the group table is `SPAWN_GROUP_ID`
  (`march2014/data036/BattleFieldData.src.lua:62-68`). The live
  `kom_contents/data036_kom/BattleFieldData.lua` bytecode is the same, with
  `SET_BOSS_GROUP_NAME` = `STR_ID_23578`. So **the middle boss probably never spawns offline**
  (`GetMiddleBossMonsterInfo` returns early). This comes from reading the code and data, not
  from a run. It contradicts june-buff `b9a9042`'s play-test target (Lanox middle bosses);
  Phase 0 task 4 settles it.
- `OnFieldNpcDie` decides "middle boss" by UID range (`Handlers_Room.cpp:814-818`), so a
  respawn kill during a live boss is charged factor 4 and clears the marker.
- No reward path reads a grade. `X2OfflineDropTable.h:100` records `fDungeonFactor = 1` as
  "room state the offline server does not model", and `GetNpcReward` is keyed on
  (key, NPC ID, level) only.

**Census so far.** 227 stage scripts under `kom_contents` contain `MONSTER_GRADE`. Only the 19
Ranox `.src.lua` are text: grade 1 ×20, 2 ×26, 3 ×9. The other 208 must be decompiled
(Phase 0).

---

## Phase 0 - Census, reach, June check

**Scope.** No code, no flag, no build. Produce the data the code phases build on, and stop for
the user if the reach is too small to be worth it.

**Run with.** Opus 5.5 / high. `/x2-phase offline-grade 0`.

**Read list.** "Starting context", "Established facts (verified 2026-10-06)", this section.

**Tasks.**
1. **Census.**
   - Decompile every stage script that carries `MONSTER_GRADE` into the scratchpad. Trust a
     file only at its live-manifest size (memory: March kom_contents extract).
   - Write `OFFLINE_MONSTER_GRADE_CENSUS.tsv` with columns: dungeon file, dungeon ID if
     known, stage / sub-stage, NPC ID, grade, `HAVE_BOSS_GAGE`, ExtraNPCStat row (y/n).
   - Report:
     - counts per grade;
     - the dungeons with no grade-1 NPC;
     - every `bScriptCheck` mismatch (excluding Henir, event and test dungeons, per
       `IsScriptCheckDungeon`);
     - the stage files with no `MONSTER_GRADE` at all.
2. **Reach.**
   - Count the grade-1 dungeon NPC IDs that have an ExtraNPCStat row: the bosses the socket
     terms will actually apply to.
   - **If that count is zero or small, stop and tell the user before Phase 1** (the plan's
     motivation would not be met).
   - Otherwise name the play-test target: a dungeon, its grade-1 boss with a row, a socket
     option with boss damage plus an item that carries it, and the dev-console commands to
     get there. Name the character and job from `els_db.sql`.
3. **June (Ghidra, read-only).**
   - Confirm the `327edb60` copy (`+0x66` → NPCData `+0x44`) and the `327ef570` / `327ef71d`
     pass.
   - Does June's dungeon `CreateGUNPC` also set `m_eMonsterGrade` / call `SetMonsterGrade`
     from NPCData `+0x44`?
   - In June, is the grade passed before the scan-rate read?
   - Does any other June reader use NPCData `+0x44`?

   Record the answers as funcmap rows.
4. **Battlefield data.**
   - Confirm the `SPAWN_GROUP_ID` loader bug against March `BattleFieldData`.
   - List the fields that have `BATTLE_FIELD_MIDDLE_BOSS_INFO`, the `SPAWN_ID > 100` rows, and
     any `IS_BOSS_RAID_FIELD` field. Does the emulator spawn raid fields at all?
5. **Reward inputs.**
   - Do items 70057 and 70058 exist in March item data? If only `ScriptData/` has them, ask
     for the live DB rows.
   - Where does the server's NPC attribute count come from, and does offline ever spawn
     attributed NPCs?
   - Does `fDungeonFactor` also reach the item lottery on the server
     (`DungeonRoom.cpp:6330-6441`)?
   - Do the offline coins (`Handlers_Room.cpp:2787`) contradict the "no ED coins" note
     (`X2OfflineDropTable.h`)?
6. **Kill-all rule.** Does `IsAllSubStageNpcDie`'s boss-only rule differ from
   `Handler_EGS_DUNGEON_KILLALLNPC_CHECK_REQ` (`Handlers_Room.cpp:2220`) in a way play can
   reach? If yes, it becomes a Phase 3 item; if no, record why.

**Done gate.**
- The census `.tsv` is committed.
- A Done note with the counts, the reach, the play-test target, and the answers to 3-6.
- `kickoffs/offline-grade/1.md`, `2.md` and `3.md` are amended with what this phase found.

### Phase 0 - Done

2026-10-06, Opus 5.5 high. No code, no flag, no build. The census is in `OFFLINE_MONSTER_GRADE_CENSUS.tsv`. The June decodes are 16 new `funcmap.tsv` rows tagged "Offline grade Phase 0", and row `323752f0` gained an addendum.

**1. Census.**
- **Source:** the 208 bytecode stage scripts in `data035_kom`, all at their live-manifest size, decompiled with `elstools decompile-batch`. They were walked like the server's `LoadNPCData` (`NPC_GROUP[i]`, `SUB_NPCj`, `NPC_GROUPn`).
- **Checks:** the Ranox decompiles match the 19 text `.src.lua` exactly. All 620 `MONSTER_GRADE =` lines and all 358 `HAVE_BOSS_GAGE = TRUE` lines are accounted for. No file has a stray global.
- **Grades over all entries:** grade 1 ×245, 2 ×275, 3 ×100; the other 22,982 entries are 0.
- **The 173 reachable files** (named in `DungeonData.lua`): grade 1 is 198 entries over 76 NPC IDs, grade 2 is 236 over 67, grade 3 is 9 over 3 (Secret Ranox Cruel Mag/Berserker).
- **Unreachable:** 35 files have no `DungeonData` row: the old El Forest, Besma Mine/Dragon Canyon, Elder Nature Cave/Halloween, Ruben/Hamel Secret and Event Halloween Day.
- **Dungeons with no grade-1 NPC:** none among the reachable graded files.
- **Files with no `MONSTER_GRADE` at all** (69): Henir, Defence Hard/Expert, Arcade, the tutorials and trainings, Newyear, Treeday, the battleships and the test dungeons. All of these are grade 0 on the server too.
- **`bScriptCheck` mismatches:** 0, whether or not Henir, event and test dungeons are excluded.

**2. Reach: not small.**
- 66 of the 76 grade-1 IDs have an ExtraNPCStat row. They cover 178 of 198 entries and 154 of 173 files.
- **No row (10 IDs):** Mushroom King (El Forest Gate), Ent, Wally 7th, the four Secret Common/Normal bosses, Ancient Bone Dragon (Common), Elsword Parasite (Common) and the Defence gate.
- **This depends on row presence, not on `EXTRA_NPC_STAT_PARITY`.** `IsJuneSocketBossGrade` tests `HasExtraNPCStat`, and June writes `+0xdc` for every row ID.
  - If Phase 1 set the grade only inside the base-only stat branch (no legacy `NPCStat.lua` row), the reach would drop to **4**: Ignia 1896, Mulsayrena 1888 and Secret Magi 2241/2242.
  - So set it whenever the row exists.
- **Play-test target:**
  - **Character:** `reyaa` (Rena / Lire, Grand Archer, Lv80).
  - **Dungeon:** Lanox Ashes Village Normal (33280; reyaa has cleared it).
  - **Boss:** `NUI_SMITH_STAIN` 1693, grade 1, with a row.
  - **Shorter alternative:** El Tree Normal, boss `NUI_BENDERS_NORMAL` 5; every character has cleared it.
  - **Primary check, no item needed:** `[JUNE_BUFF] first socket boss grade: NPC 1693 grade 1, ExtraNPCStat row yes -> boss yes`.
  - **Damage check, optional:** it needs a save edit, which is the user's call. Weapon socket option 86322 (`SDT_WEAPON_HUNTING`) gives +3 % damage to bosses. 74043 gives +10 %, but it is a title socket. reyaa's weapon is `item_uid 810`, with sockets `40120,40124,40024,40024`.

**3. June (Ghidra).**
- **`327edb60` (AddNPCData):** copies packet `+0x66` into NPCData `+0x44` as a sign-extended char. Nothing initialises `+0x44`.
- **June's client never reads a stage script's `MONSTER_GRADE`.** Its parser `327eb8e0` has 24 keys and none of them is the grade. The only `MONSTER_GRADE` key is the Hero-dungeon grade-stat binding `327defc0`, which March lacks. So the grade arrives only by packet, and offline the emulator has to play the server.
- **`327ef570` (CreateGUNPC):** always passes `+0x44` to `32384d10` (`@327ef704`). The `+0xdc` write and the scan 50 / near 1000 write inside it happen only when the NPC ID has a row (`32384daa`).
- **`SetMonsterGrade` (`327574c0`) has zero callers.** It is inlined only in the battlefield constructor. The dungeon path never writes `m_eMonsterGrade` (`+0x8f08`), so Phase 1 does not call it.
- **Order:** the grade reaches the templet first (`327ef71d`). Then come the constructor, `Init`, and `SetNPCStat` `32763950`, which reads scan/near (`+0xc8`/`+0xcc`). `SetNpcHardLevel` runs last. No other reader of `+0x44` exists.

**4. Battlefield.**
- **The loader bug is confirmed.** `X2OfflineBattleField.cpp:516` opens the scalar `SET_BOSS_GROUP_NAME` as a table. `m_vecSpawnGroupID` stays empty, the gate at `:529` drops every entry, and `GetMiddleBossMonsterInfo` returns at `:942`. **No middle boss ever spawns offline.** So june-buff 16-step3b2's "Lanox middle boss" check cannot happen before Phase 2.
- **Middle-boss tables:** every field from 40001 to 40032 has one, except Ruben 40000, the two ships 40020/40021 and the raid field 40026. Their groups are all `IS_BOSS_MONSTER_GROUP` groups in the per-field files.
- **`SPAWN_ID > 100`:** no rows; the maximum is 19. `SERV_BATTLEFIELD_EVENT_BOSS_INT` is not defined on US.
- **Raid field:** only 40026 (Iron Eltrion). Offline never spawns it: its only group is boss-flagged, so `EnsureFieldLoaded` fails. Its `VILLAGE_MAP_ID` seed is also missing, which is unverified.
- **UID-range "middle boss" test (`Handlers_Room.cpp:817`):** latent today, live once the loader is fixed. A respawn kill while the boss lives would be charged factor 4 and clear the marker.

**5. Reward inputs.**
- **70057 / 70058** exist in the live `item.kim`: "Essence of Life", the small and big HP orbs.
- **The attribute input is a bool on dungeons** (`IsAttribNpc`; `AttribNpcTable.lua` exists only on the server). It travels in `KEGS_DUNGEON_STAGE_LOAD_NOT::m_mapAttribNpcInfo`. Offline never spawns attributed NPCs, so the attribute branch is always false.
- **`fDungeonFactor` and `m_bIsBoss` never reach the item lottery** (`KDropTable.cpp:1291-1358`). They change only EXP, ED and the ED pile count. The grade reaches items only through `NpcSpecialDropItem`.
- **Offline coins:** 2-3 coins on kill (`Handlers_Room.cpp:2785`), with ED paid on pickup. So `X2OfflineDropTable.h:99-109` is stale twice: coins are dropped, and the server does not fold the factors into items. Bosses also get 2-3 coins, not 10-20.

**6. Kill-all rule: not a work item.**
- The server's boss-only check can only refuse more than the emulator's unconditional `true`.
- A false answer means no clear: the hack report is `_SERVICE_`-only.
- An honest client always passes, with grade 0 or grade 1.

## Phase 1 - Dungeon grade end to end

**Scope.** Make the dungeon grade travel the real server's way, script → packet, and June's
way, packet → NPCData → NPC.

**Run with.** Sonnet 5.5 / high. `/x2-phase offline-grade 1`. Depends on Phase 0. The
june-buff 16-step3b2 review it waited on is committed (`60d0571`).

**Read list.** "Rules", "Established facts (verified 2026-10-06)", this section, "Phase 0 - Done".

**Tasks** (each behind `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE`).
1. **`KTDXLIB/Always.h`:** append the flag (house comment block).
2. **`X2Lib/X2DungeonSubStage.h`:** add a game-build member `char m_cMonsterGrade;` to
   `NPCData`. Use a `char`, never the X2TOOL-only enum, and leave the X2TOOL `m_MonsterGrade`
   alone.
3. **`X2Lib/X2DungeonSubStage.cpp`:**
   - `FetchNPCData` reads `MONSTER_GRADE` (int, default 0) next to `HAVE_BOSS_GAGE`. This is
     the emulator's copy of `XSLDungeonSubStage.cpp:197`.
   - The struct has no constructor, so initialise the member to 0 at **every**
     `new NPCData` site.
   - `AddNPCData` copies `kNPCUnitReq.m_cMonsterGrade` (June `327edb60`).
   - `CreateGUNPC( const NPCData* )` calls the existing
     `SetExtraNPCStatMonsterGrade( pNpcData_->m_cMonsterGrade )` under
     `#if defined( SERV_IRUHADEV_OFFLINE_MONSTER_GRADE ) && defined( SERV_IRUHADEV_EXTRA_NPC_STAT )`
     (June `327ef71d`). Prove it runs before the `SetHardLevel` read (`X2GUNPC.cpp:7210`).
   - Add `SetMonsterGrade` only if Phase 0 found June does it.
   - No `X2GUNPC.*` edit.
4. **`X2Lib/Offline/Handlers_Room.cpp` `BuildStageNpcData`:**
   - set `kReq.m_cMonsterGrade` from NPCData;
   - replace the wrong `:1886` comment;
   - log one `[OFFLINE_GRADE]` line per NPC with a non-zero grade at stage load (dungeon,
     stage, sub-stage, NPC ID, grade, boss gauge);
   - log a warning per `bScriptCheck` mismatch. Log only; never refuse the stage.

**Side effects to state in the Done note.**
- ExtraNPCStat-row NPCs of grade 1 or 2 now scan at 50/1000.
- JUNE_BUGS #52 is live for dungeon bosses: update its reach column.
- `m_eMonsterGrade` is unchanged unless step 3's June check said otherwise.

**Done gate.** On the Phase 0 target:
- `[OFFLINE_GRADE]` shows the boss with `grade 1`;
- `ErrorLog.txt` shows june-buff's `first socket boss grade: NPC <id> grade 1, ExtraNPCStat row yes -> boss yes`;
- the boss-damage socket raises the damage number on that boss, compared with the same skill
  on a non-boss.

### Phase 1 - Done

(not yet)

## Phase 2 - Battlefield like the CenterServer

**Scope.**
- Middle bosses actually spawn.
- Every battlefield spawn carries the CenterServer's grade.
- The emulator tells a middle boss apart by grade, not by UID.

**Run with.** Sonnet 5.5 / high. `/x2-phase offline-grade 2`. Depends on Phase 1 (both
edit `Handlers_Room.cpp`).

**Read list.** "Rules", "Established facts (verified 2026-10-06)", this section,
"Phase 0 - Done", "Phase 1 - Done".

**Tasks** (behind the flag).
1. **`X2OfflineBattleField.cpp/.h` loader:**
   - read the middle-boss group IDs from the `SPAWN_GROUP_ID` table, and `SET_BOSS_GROUP_NAME`
     as the string ID it is (`XSLBattleField.cpp:195`, client `X2BattleFieldManager.cpp:556/567`);
   - add the US `SPAWN_ID > 100` rule (`XSLBattleField.cpp:246`);
   - log the loaded middle-boss rows per field.
2. **Grades per spawn path, as `XSLBattleField`:**
   - normal 0;
   - middle boss 2;
   - raid-field boss groups 1, only if Phase 0 found raid fields the emulator spawns;
     otherwise record "not reached" with the reason.
3. **`Handlers_Room.cpp`:**
   - Store the grade per UID next to `m_mapNpcLevel` / `m_mapNpcGroup`, like the server's
     `NPC_DATA::m_cMonsterGrade`. Include grades the client sends with
     `EGS_NPC_UNIT_CREATE_REQ`: the server keeps those.
   - `OnFieldNpcDie` decides middle boss from the stored grade, not the UID range (`:814`).
   - The danger factor comes from the server's table (attribute first, then 4 / 12 / 1).
4. **No client change.** June's battlefield ExtraNPCStat grade (`X2BattleFieldGame.cpp:786` /
   `:1000`) stays as ported.

**Done gate.**
- A battlefield middle boss spawns: `[OFFLINE_GRADE] field <id> middle boss NPC <id> grade 2`.
- Killing it logs danger factor 4 once.
- A normal kill while the boss lives logs 1, and the boss marker survives it.
- JUNE_BUGS #52's battlefield reach is corrected: offline it was latent until this fix,
  because the middle boss never spawned.

### Phase 2 - Done

(not yet)

## Phase 3 - Rewards and remaining consumers

**Scope.** The server's grade-keyed rewards (user decision 2), plus the kill-all rule if
Phase 0 found it reachable.

**Run with.** Opus 5.5 / high. `/x2-phase offline-grade 3`. Depends on Phase 2.

**Read list.** "Rules", "Established facts (verified 2026-10-06)", this section,
"Phase 0 - Done", "Phase 2 - Done".

**Tasks** (behind the flag; `X2Lib/Offline/Handlers_Room.cpp`, `X2OfflineDropTable.cpp/.h`).
1. **Dungeon:** `fDungeonFactor` = the server's `GetMonsterTypeFactor( attribute, difficulty, grade )`.
   Apply it to EXP/ED as `KDropTable.cpp:1232-1244` does, and to the item lottery if Phase 0
   found the server does.
2. **Battlefield:** `GetBattleFieldMonsterTypeFactor` into the field EXP/ED factors
   (`BattleFieldRoom.cpp:4398/5045/5930`).
3. **Order:** the server factor first, then the existing offline boost (memory: offline 3×
   EXP/drop boost).
4. **Boss ED** in 10-20 piles (`KDropTable.cpp:1252`), reconciled with Phase 0's coin answer.
5. **`NpcSpecialDropItem`:** 70057 / 70058. If an item is missing from the data, log it and
   skip it.
6. **Kill-all rule**, if Phase 0 put it here.
7. **Logging:** every applied factor gets one `[OFFLINE_GRADE]` line per kill (grade, factor,
   EXP before/after).
8. **Close-out:** a `MODS.md` entry for the flag (what it changes, how to revert).

**Done gate.**
- A dungeon boss kill logs factor 12 and EXP = base × 12 × boost.
- A middle boss logs 4.
- The special drop is seen, or its absence is logged.
- A normal kill is unchanged.

### Phase 3 - Done

(not yet)

## Reviews

`1-review`, `2-review`, `3-review`: Opus 5.5 / high. Each checks its phase against the server
citations above and the June addresses, runs the range flagcheck, fixes transcription errors
under the same flag, and hands design questions back to the user.

## Play-test hand-off

Dev console first (memory: offline auth level, `settings.auth_level = 3`, END key).
- Copy-ready one-line commands with real values from the Phase 0 target: the character and job
  from `els_db.sql`, the dungeon, the socket item.
- Per check: where / paste / what it does / look for / if not.
- End with `python tools/logs.py --tag OFFLINE_GRADE`, which is what the session reads after
  "check logs".
