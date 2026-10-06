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

2026-10-06, Sonnet 5.5 high. Flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` (the one flag, defined in `KTDXLIB/Always.h`). **Code done and deployed; play-tested 2026-10-06 (result below); reviewed (see "Phase 1 - Review").**

**What landed** (tasks 1-4):
- `X2DungeonSubStage.h`: `NPCData::m_cMonsterGrade` (a `char`; X2TOOL's `m_MonsterGrade` untouched).
- `X2DungeonSubStage.cpp`:
  - `FetchNPCData` reads `MONSTER_GRADE` (int, default 0). This is the emulator playing `XSLDungeonSubStage.cpp:197`.
  - All 5 game-build `new NPCData()` sites set the member to 0. The 2 X2TOOL sites use the constructor and were left alone.
  - `AddNPCData` copies `kNPCUnitReq.m_cMonsterGrade` (June `327edb60`).
  - `CreateGUNPC( const NPCData* )` calls `SetExtraNPCStatMonsterGrade` for every NPC (the readers test the ExtraNPCStat row themselves, as June does at `32384daa`).
- `Offline/Handlers_Room.cpp` `BuildStageNpcData`:
  - sends `kReq.m_cMonsterGrade`;
  - the wrong `:1886` comment is now in the `#else`;
  - logs one `[OFFLINE_GRADE] dungeon= stage= sub= npc= uid= grade= bossGauge=` line per graded NPC;
  - logs `[OFFLINE_GRADE] WARN scriptCheck ...` for the server's two mismatches (boss gauge with grade not 1/2; grade 1 without a gauge). It logs only and never refuses the stage, and it does not skip Henir/event/test dungeons the way the server does.
- No `SetMonsterGrade` call (June has no dungeon caller), no `X2GUNPC.*` edit.

**Order proof.** `CreateGUNPC` sets the grade right after the factory returns, before `Init()`. The only reader is `SetHardLevel`'s scan/near block (`X2GUNPC.cpp:7226`). Its dungeon caller is `SetNpcHardLevel` in `SwapToGameNPCUnit` (`X2DungeonSubStage.cpp:387`), which runs after `CreateGUNPC` returns (`:375`). `IsJuneSocketBossGrade` reads it only at damage time.

**Side effects.**
- ExtraNPCStat-row NPCs of grade 1/2 now scan at 50 / near range 1000 instead of 10 / 500.
- JUNE_BUGS #52 is live for dungeon bosses (AI-party bots, summons and ally NPCs deal x2 to a grade-1 boss with a row). Its reach column is updated.
- `m_eMonsterGrade` is unchanged.
- `X2GUNPC.h:2365` still says the dungeon grade is "not plumbed here". It is stale now; the kickoff forbids editing `X2GUNPC.*`, so the Phase 1 review may correct it.

**Hygiene.** `flagcheck --worktree --files KTDXLIB/Always.h X2Lib/X2DungeonSubStage.h X2Lib/X2DungeonSubStage.cpp X2Lib/Offline/Handlers_Room.cpp`:
`files: 2 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,232,256 B, dated 2026-10-06 14:32:54, md5 be1c9b56b9c6eb0a6bdbf092f20d074f, equal to X2/US_SERVICE/x2.exe (2026-10-06 14:32:49); built at mods/june-buff-port aab81d7 + 5 uncommitted code file(s).`
That exe was linked by another session's build with my edits already in the tree, and it also contains that session's uncommitted `X2Game.cpp` edit. The `[OFFLINE_GRADE]` strings are confirmed in the deployed exe.

**Play-test result (2026-10-06 14:59-15:05, deployed exe md5 `be1c9b56...`, logs read): PASSED, checks 1 and 2.**
The user played Ranox Burnded Forest Normal (33270) instead of the planned target; its boss `NUI_RAGE_DRYADKENTA` 1686 is grade 1 with a row (census).
- `offline_server.log:1375`: `[OFFLINE_GRADE] dungeon=33270 stage=3 sub=0 npc=1686 uid=114 grade=1 bossGauge=1`. It is the only `[OFFLINE_GRADE]` line, so no `WARN scriptCheck` lines (expected 0). Stages 0-2 carry no graded NPC.
- `ErrorLog.txt:624`: `[JUNE_BUFF] first socket boss grade: NPC 1686 grade 1, ExtraNPCStat row yes -> boss yes`.
- `ErrorLog.txt:625`: `first socket damage to boss (final damage): rate 2, 21618.6 -> 43237.1`. This is JUNE_BUGS #52 live, not the socket item: the rate is exactly 2.0, and the party's AI bots were hitting the boss in that window. The player-attacker socket check (check 3) was not done.
- The boss died `KILL_BY_USER` at 15:05:11; no UNHANDLED/EXCEPTION lines for this phase (2 `E_DISABLE_HB_CHECK_REQ` UNHANDLED at 14:59-15:00 are unrelated).
- Not exercised: the scan 50 / near 1000 change has no log line.

**Gate (original text).** Target: reyaa, Lanox Ashes Village Normal (33280), boss `NUI_SMITH_STAIN` 1693.
1. `offline_server.log` shows `[OFFLINE_GRADE] ... npc=1693 ... grade=1 bossGauge=1`.
2. `ErrorLog.txt` shows `[JUNE_BUFF] first socket boss grade: NPC 1693 grade 1, ExtraNPCStat row yes -> boss yes`.
3. Optional: the boss-damage socket (86322 on weapon `item_uid 810`) raising the number on the boss. It needs the user's OK for a save edit, and first confirming that `InitSocketData` sums an equipment socket of that type. Not done.

Expected `scriptCheck` warnings: 0 (census). Open questions for the user: none.

### Phase 1 - Review

2026-10-06, Opus 5.5 high, review of `ac0fe70`. **Verdict: correct, no code change.** No rebuild was
needed, so the deployed exe is still the play-tested `be1c9b56...`.

**Hygiene.** `flagcheck SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range ac0fe70^..HEAD`:
`files: 2 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`. The `PASS~` is the one blank line after the logging block's `#endif` in `Handlers_Room.cpp`.

**Checked against the sources (all match).**
- **Server.** `XSLDungeonSubStage.cpp:197` (`LUA_GET_VALUE_ENUM MONSTER_GRADE`, default 0) matches `FetchNPCData`. `:221-246` gives the two warning cases. `XSLDungeon.cpp:391` is the copy that `BuildStageNpcData` mirrors.
- **June `327edb60` (AddNPCData).** `NPCData[0x11] = (int)(char)req[+0x66]`. `+0x66` is `m_cMonsterGrade`: it sits between `m_cAllyTeam` `+0x65` and `m_usBossGaugeHPLines` `+0x67`.
- **June `327ef71d`.** `CreateGUNPC` passes `NPCData +0x44` as the fetch `32384d10`'s 3rd argument, before the factory `327b1d40` and `Init` (vtable `+0x40`).
- **The three copies of the grade.**
  - FetchNPCData read: `X2DungeonSubStage.cpp:1610-1618`.
  - BuildStageNpcData send: `Handlers_Room.cpp:1891`.
  - AddNPCData copy: `X2DungeonSubStage.cpp:2206`.
- **Zero-init.** All 5 game-build `new NPCData()` sites set it. These are the only allocations, and nothing copies an NPCData by value.
- **Order.** `m_iExtraNPCStatMonsterGrade` is zeroed only in `CommonConstructor` (`X2GUNPC.cpp:2352`), not in `Init`. The setter (`:2455`) runs before `Init` (`:2463`) and before `SetNpcHardLevel` (`:387`, the only reader's caller). `CreateAndAppendGUNPC` → `CX2Dungeon::CreateStageNPCs` has no caller.
- **The client's own parse never spawns NPCs.** It runs only for `bIsNpcLoad` dungeons (`:1533`), i.e. the emulator's throwaway, so in-game NPCs get their grade from the packet, as in June.
- **`#else` paths.** The send keeps the original comment, and every other edit is insert-only.

**Coverage checks (no finding).**
- **Mid-game spawns.** June's other fetch callers all pass grade 0: `CX2Game::CreateNPC` `3230d5c9`, plus `31f0ba27`, `32e8d00a` and `32e8d0bd`. So `EGS_NPC_UNIT_CREATE_NOT` NPCs carrying no grade is June's behaviour, not a gap.
- **Other grade readers.** None changed. The client's `m_bIsBossStage` comes from clear conditions. The server's `bIsBossStage` (`XSLDungeon.cpp:402`) gates only event monsters (`DungeonRoom.cpp:3764`) and is in no packet.
- **Per-NPC versus June's per-ID grade** (the 16-step3b2 choice, now live for dungeons). I re-decompiled 163 boss stage files, and the other 10 are Ranox text. No grade-1 boss ID appears as a grade-0 `NPC_GROUP` entry or is spawned by a stage trigger, so the difference has no reach in stage data. NPC-script self-summons were not swept.
- **`bScriptCheck` scope.** The emulator warns only on shipped NPCs, after the rate roll and the SUB_NPC pick. The census walked every entry, losers included, and found 0 mismatches, so nothing is missed.

**Notes (not fixed).**
- The X2TOOL constructor (`X2DungeonSubStage.h:178`) does not set `m_cMonsterGrade`, so the Done note's "use the constructor" overstates it. X2TOOL is never built here.
- Two stale comments still say the dungeon grade is not plumbed: `X2GUNPC.h:2365` and `:3771-3780`. They are left as they are, because the code is june-buff's (hands-off) and correcting them would cost a full PCH rebuild for an identical binary.
- The member sits inside the studio's `//{{ ... //}}` SERV_ALLY_NPC block. This is cosmetic.
- The scan 50 / near 1000 change is still not exercised (it has no log line).
- **Kickoffs.** Phase 1 moved `Handlers_Room.cpp` by +8 / +39 lines. `kickoffs/offline-grade/2.md` (`:1894` → `:1902`, `:2332` → `:2371`) and `3.md` (`:2785` → `:2824`) are updated. `2.md` already says no client change on battlefields.

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

2026-10-06, Sonnet 5.5 high. Flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` (no new flag). **Code done and deployed; play-test pending.**

**What landed** (tasks 1-4):
- `X2OfflineBattleField.cpp/.h` loader: the middle-boss groups now come from the `SPAWN_GROUP_ID` table (`XSLBattleField.cpp:195`). A repeated group ID in one row is kept once with a WARN, as the server does (`:203-223`). `SET_BOSS_GROUP_NAME` is read as an int string ID (`KMiddleBoss::m_iBossNameStrID`) and only logged. The `SPAWN_ID > 100` rule (`:246`, US without EVENT_BOSS_INT) means every row joins the list, which the loader already did; it is commented and never fires (max SPAWN_ID is 19). One log line per loaded row: `[OFFLINE_GRADE] field <id> middle boss row: spawnID= rate= risk= groups= nameStrID=`.
- Grade per spawn path: normal 0 (`PickFromGroup`), middle boss 2 (`MakeMiddleBoss`), both unchanged. **Raid grade 1: not reached.** The only raid field, 40026, is never spawned offline (Phase 0 item 4), so no code.
- `Handlers_Room.cpp` + `X2OfflineServer.h`: `m_mapNpcGrade` (UID -> grade) is filled in `TrackFieldNpc`, `BuildStageNpcData` and `Handler_EGS_NPC_UNIT_CREATE_REQ` (the client-sent grade is kept). `m_setMiddleBossAlive` is the live boss set. `OnFieldNpcDie` now decides the middle boss by set membership, not UID range, and clears the marker with the last member. The factor comes from `GetMonsterTypeFactor` (attribute branch never taken offline): grade 2 = 4, grade 1 = 12, else 1.
- **Server behaviour added beyond the kickoff text:** `KBattleFieldGameManager::OnNpcUnitDie` returns before touching danger when any middle-boss member is still alive (`bIncreaseDanger = !IsRemainMiddleBoss()`, `BattleFieldRoom.cpp:1783/1833/1869`). The emulator now does the same: normal kills while a boss lives add no danger, and a multi-member boss charges factor 4 once, on its last member. Before, every kill added danger. Log line for those kills: `danger factor 1, danger stays N (middle boss still alive)`.
- No client change; June's battlefield grade setters untouched.
- `JUNE_BUGS.md` #52: battlefield reach corrected (live only since this phase; before it no middle boss ever spawned offline).

**Hygiene.** `flagcheck --worktree --files` (the 4 code files):
`files: 3 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,234,304 B, dated 2026-10-06 15:48:36, md5 a52fcf24444368be25e2c793056c9617, equal to X2/US_SERVICE/x2.exe (2026-10-06 15:48:31); built at mods/june-buff-port 8c6ef37 + 4 uncommitted code file(s).` The `[OFFLINE_GRADE]` middle-boss, row and "danger stays" strings are in the deployed exe.

**Gate (play-test, not yet done).** Log tag `[OFFLINE_GRADE]`:
1. A middle boss spawns: `field <id> middle boss NPC <id> grade 2 uid=`, preceded by the `middle boss row` lines at field load.
2. Its death logs `grade=2 danger factor 4`, once per boss set.
3. A normal kill while it lives logs `factor 1, danger stays`, and the boss marker survives.

Open questions for the user: none.

### Phase 2 - Review

2026-10-06, Opus 5.5 high, review of `e76ec1a` (play-test waived, so the code was read against the server line by line). **Verdict: Phase 2 is correct.** One older mismatch it made reachable is fixed under the flag. Two comment citations were wrong and are fixed too.

**Hygiene.** `flagcheck SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range e76ec1a^..c23f671` (Phase 2 alone):
`files: 3 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`. The `PASS~` is the blank line after the `TickField` block's `#endif`.
Review fix, `--range e76ec1a^..HEAD`: `files: 4 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` · `in this flag's files: 3 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` · `RESULT: PASS` (fix commit `069eddc`).
The deleted lines are three of Phase 2's own comment lines inside the flag, rewritten. The new `PASS~` is the blank line after the RATE roll's `#endif`.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,234,304 B, dated 2026-10-06 16:00:18, md5 9aab75296caf6cf5abc3e0bfde7da3bb, equal to X2/US_SERVICE/x2.exe (2026-10-06 16:00:13); built at mods/june-buff-port c23f671 + 2 uncommitted code file(s).` (those two files are `069eddc`). The `missed its RATE` string is in the deployed exe. No play-test is owed: on March data the roll always passes, so the behaviour is unchanged.

**Checked against the sources (all match).**
- **Loader.** `XSLBattleField.cpp:187-250`: `SPAWN_GROUP_ID` table, `buf > 0`, a repeated ID kept once. `SPAWN_ID > 100` only splits under `SERV_BATTLEFIELD_EVENT_BOSS_INT`, which is not defined, so every row joins. Client `X2BattleFieldManager.cpp:556/567` reads the same two keys.
- **March data** (29 field tables decompiled from `kom_contents/data036_kom`, plus `BattleFieldData.src.lua`):
  - 25 fields have rows (85 in total). Every field's `SPAWN_RATE` sums to 100, and every listed group exists in its field table.
  - The largest `SPAWN_ID` is 19.
  - So the emulator's `rand() % iTotalRate` matches `KLottery`, and the server's other failure paths (empty group list, missing group) are never reached.
- **Grades.**
  - `MakeMiddleBossMonsterInfo` sets 2 (`:1003`), matching `MakeMiddleBoss` (`X2OfflineBattleField.cpp:980`).
  - Normal spawns are 0 (`:817`).
  - Raid grade 1 is "not reached" (field 40026 is never spawned): accepted.
- **Live death path.** `SERV_BATTLE_FIELD_BOSS` is a top-level define (`ServerDefine.h:4257`), so the live dispatch is `BattleFieldRoom.cpp:1744-1787`.
  - `MiddleBossMonsterDieProcess` erases the member (`:6500`, `SetMiddleBossMonsterDie`) before `!IsRemainMiddleBoss()` is evaluated at `:1783`.
  - The grade comes from the stored `NPC_DATA` (`:6511`, `:6593`).
  - `KBattleFieldRoom::OnNpcUnitDie` (`:5804`) does nothing unless `bIncreaseDanger` (`:5816`), and it passes the attribute flag as `false`.
  - That is exactly erase → `empty()` → early return in `OnFieldNpcDie`. Factor table: `XSLUnitManager.cpp:238-269`, 4 / 12 / 1.
- **Client-sent grades.** Battlefield `ERM_NPC_UNIT_CREATE_REQ` (`BattleFieldRoom.cpp:1653`) keeps the request's grade through `CreateMonster`, as the dungeon handler does. So one store in `Handler_EGS_NPC_UNIT_CREATE_REQ` is right for both.
- **State.** `m_setMiddleBossAlive` and `m_iMiddleBossAlive` are reset together only in `Init()`, and the boss-group UIDs never respawn (group 0). Nothing leaves the set stale.

**Fixed (under the flag).**
- **Per-NPC RATE roll.** The server gives every NPC in a middle-boss group its own one-case `KLottery` at `RATE` (`XSLBattleField.cpp:915-917`; `Decision`, `Lottery.cpp:121-141`, is a uniform roll over [0,100]) and skips the NPC on a miss. `GetMiddleBossMonsterInfo` spawned every NPC, under a comment that cites `:770-802` (commented-out respawn code).
  - It is now rolled, with a `[OFFLINE_GRADE] ... missed its RATE` line on a miss.
  - **Latent in March:** all 148 boss groups have `RATE` 100.
  - June's `Sander_Field_02` has two 50/50 groups, so this matters once June field tables are adopted.
- **Citations.** `OnFieldNpcDie` cited the dead `#else` copies (`BattleFieldRoom.cpp:1833/1869`, `BattleFieldGameManager.cpp:80-87`). It now cites `:1783` / `:6500` and `:5804/5816`.
- **Stale comment.** The `TickField` comment ("every UID from here up belongs to this boss set") is left in place; a note inside the flag now says the set decides.

**Notes (not fixed).**
- `OnFieldNpcDie`'s leading comment still says the marker decides; the flagged lines below it say otherwise.
- Server-only failure paths are not mirrored, because the data never reaches them: a missing group makes the server drop the whole boss, while the emulator skips the group. Re-check this if field data changes.
- **Kickoff 3:** `Handlers_Room.cpp:2881` → `:2884` (+3 lines from this review). `X2OfflineDropTable.h:99-109` checked: still the stale "no coins / factors are 1.0" notes Phase 3 corrects.

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

2026-10-06, Opus 5.5 high. Flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` (no new flag). Code in `X2Lib/Offline/Handlers_Room.cpp`, note in `X2OfflineDropTable.h`, entry in `MODS.md`. **Play-tested, gate passed** (below).

**What landed** (tasks 1-8):
- **Type factor** from the UID's stored grade (`m_mapNpcGrade`): attribute first (never true offline), then 4 / 12 / 1 (`XSLUnitManager.cpp:238/:274`).
  - **Dungeon:** `fDungeonFactor` on EXP and ED (`KDropTable.cpp:1232/:1239`), right after `GetNpcReward`. The item lottery gets no factor, as on the server.
  - **Field:** EXP x `GetEXPFactor()` x factor and ED x `GetEDFactor()` x factor, one float product (`BattleFieldRoom.cpp:4403/:5050`).
  - **Order:** the server factor first, then the offline 3x boost.
- **Field middle boss ED x20.** This was not in the plan. The live dispatch (`BattleFieldRoom.cpp:1744`, `SERV_BATTLE_FIELD_BOSS`) sends a set member to `MiddleBossMonsterDieProcess` -> `BattleFieldMiddleBossDropProcess` (`:4378`). That function sets `ED = (10 + ED*lv) * 20` (`:4594-4596`). The plan's `:2016/:2217` sit in the dead `#else`.
- **Boss coins:** 10-20 for grade 1 (`m_bIsBoss`, `KDropTable.cpp:1254`); a middle boss keeps 2-3. **Gold bar:** a coin worth more than 1999 ED is 127041 (`:1176`; the client accepts it under `FIELD_BOSS_RAID`, `Always.h:582`). The pickup path uses the same function. **March's `item.kim` has no 127041 row** (127020, 70057 and 70058 are there), so while the templet is missing the coin is drawn as a gold coin, logged once (`[OFFLINE_GRADE] gold bar 127041 is not in the item data`). The value lives in `m_mapDropED`, so it is unaffected.
- **Special drop** (`NpcSpecialDropItem` grade switch, `KDropTable.cpp:1905-1931`):
  - 70057 at 50 % for grade 2, 70058 for grade 1, 70057 for grade 3. It runs after the NO_DROP gate.
  - **Not for a field middle boss:** the server's middle-boss process never calls it (correcting "Established facts").
  - A missing templet logs and skips. The mana/blood eater half (`NDS_BONUS_DROP`) is not ported, because that state never reaches `PushNpcDrop`.
- **Kill-all rule:** not ported (Phase 0 item 6). **Field boss raid** (`BattleFieldBossDropProcess`, factor 16x1.1^(lv-49)): unreachable offline, so no code.
- **Log:** `[OFFLINE_GRADE] kill uid= id= grade= factor= (dungeon|field|field middle boss, ED x20): exp A -> B -> C, ED A -> B -> C`. The three numbers are table -> server factor -> boost, logged on every paid kill (factor 1 too). There is also `[OFFLINE_GRADE] special drop: ...`.
- `X2OfflineDropTable.h`: a flagged correction under the stale note (coins are dropped; no factor reaches items). The original lines are kept.

**Hygiene.** `flagcheck --worktree --files X2Lib/Offline/Handlers_Room.cpp X2Lib/Offline/X2OfflineDropTable.h`:
`files: 1 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,244,032 B, dated 2026-10-06 16:23:48, md5 944e95e4a67be9d978884be87d222888, equal to X2/US_SERVICE/x2.exe (2026-10-06 16:23:43); built at mods/june-buff-port 6bb652d + 1 uncommitted code file(s).` (that one file is this fix). The `[OFFLINE_GRADE] kill` and gold-bar strings are in the deployed exe.

**How to see it** (log tag `[OFFLINE_GRADE]`):
- **Dungeon boss:** `grade=1 factor=12 (dungeon)`, with B = A x 12 and C = B x 3. 10-20 coins fall, and the `special drop ... item 70058` line appears.
- **Dungeon middle boss:** `grade=2 factor=4`. Item 70057 appears about half the time.
- **Normal kill:** `grade=0 factor=1`, A = B.
- **Field middle boss (optional):** `factor=4 (field middle boss, ED x20)`, and no special-drop line.

**Play-test 2026-10-06 16:40-16:47 (exe 944e95e4, run after deploy): gate passed.** Run: reyaa, Benders Cave Normal 30050.
- **Middle boss** Pporu Giant Red 28 (uid 32): `grade=2 factor=4 (dungeon): exp 114 -> 456 -> 1368, ED 186 -> 744 -> 2232`, 2 coins. The 50 % roll missed, so no 70057 this run.
- **Boss** Benders 40 (uid 45): `grade=1 factor=12: exp 114 -> 1368 -> 4104, ED 186 -> 2232 -> 6696`, 10 coins of 669, and `special drop ... item 70058`.
- **Normal kills:** all `factor=1`, base = server (114 -> 114 -> 342).
- **Logs:** no `[OFFLINE_GRADE]` errors; the only UNHANDLED is `E_DISABLE_HB_CHECK_REQ`, which is unrelated.
- **Open observation:** 70058 was sent (dropUID 211), but the client never asked to pick it up. It picked up 208-210 and 212 but not 211, and logged no "no drop viewer" box. Either the orb was out of reach when the stage cleared 5 s later, or its floor particle failed and the drop manager dropped it silently (`X2DropItemManager.cpp:93-107`). The user is asked whether an orb was visible.

**Open question for the user (proposed Phase 3b).** A field middle boss also gives 70070 (HP), 70050 (MP), a common bonus item and cubes by contribution rank (`BattleFieldRoom.cpp:4700-4790`). That comes from `FieldBonusDrop.lua`, which is server-only (`KncWX2Server/ServerResource/US/FieldBonusDrop.lua`, `BattleFieldMiddleBossDropInfo`). Porting it is a loader plus a degrade path, and the user packs the file. It is not in this phase: OK it as 3b, or drop it.

### Phase 3 - Review

2026-10-06, Opus 5.5 high. Reviewed `7c79f69` + `fee21f9`. **Correct, no code change, no rebuild** (the deployed exe 944e95e4 stands).

**Checked against the server** (each line opened):
- **Type factor.** `GetMonsterTypeFactor` (`XSLUnitManager.cpp:238-268`) and `GetBattleFieldMonsterTypeFactor` (`:274-293`) both test the attribute first, then 4 / 12 / 1 (`MTF_*`, `XSLUnitManager.h:2693-2700`). Offline never spawns an attributed NPC, so the port's grade-only switch is exact.
- **Dungeon.** `DungeonRoom.cpp:6177/6182`: `fDungeonFactor` = type factor. It reaches `NormalNpcDropItem` (`KDropTable.cpp:1563`), and Henir and ExtraStage get it the same way. `NpcDropItem` multiplies it into EXP (`:1232`) and ED (`:1239`, the live `SERV_DROP_EVENT_RENEWAL` branch) and nowhere else: no line after `:1244` up to the function's end (`:1459`, the lottery included) reads it. Contribution and level factor stay 1 offline.
- **Field.** `fFieldEXP/EDFactor` = field factor x type factor (`BattleFieldRoom.cpp:4403-4404` middle boss, `:5050-5051` normal). `BattleFieldNpcDropItem` (`KDropTable.cpp:3441-3479`) does a float product truncated once. The association differs: the server computes `ED x (F x TF)`, offline computes `(ED x F) x TF`. That is a non-finding. Offline a field TF is 1 or 4, and x4 is exact in float. TF 12 needs the field boss raid, which is unreachable offline.
- **Field middle boss.**
  - The live dispatch is `:1744` (`SERV_BATTLE_FIELD_BOSS`, ServerDefine.h:4257) -> `:1760` -> `MiddleBossMonsterDieProcess` `:6483` -> `BattleFieldMiddleBossDropProcess` `:4378`.
  - `:4594-4596`: ED = (10 + ED x lv) x 20, before the field factor, with EXP = `GetNpcExp(lv)` as on a normal kill (`:5222/:5230`).
  - No `NpcSpecialDropItem` call anywhere in `:4378-5023`, so dropping 70057 for it is right. "Established facts" was wrong there.
- **Coins.** `m_bIsBoss = grade == 1` (`RoomMonsterManager.cpp:75`, `BattleFieldMonsterManager.cpp:861`) gives [10,20] coins (`KDropTable.cpp:1254`, `:3487`); otherwise [2,3].
  - Gold bar: `1999 < nED` (`:1176`, `SERV_BATTLE_FIELD_BOSS`). The IDs agree: client `GOLD_BAR_ED_ITEM_ID` and server `EDI_GOLD_BAR` are both 127041 (`X2Define.h:1267`, `XSLItem.h:914`).
  - `FIELD_BOSS_RAID` (`Always.h:582`) is unconditional, so the client accepts the gold bar as ED (`X2ItemManager.h:447`).
  - The `fee21f9` gold-coin picture fallback is display only; the value stays in `m_mapDropED`.
- **Special drop.** `KDropTable.cpp:1905-1931` matches case for case: 50 % 70057 / 70058 / 70057. `SERV_KILL_BOSS_MIDDLE_BOSS_DROP_ITEM` (`ServerDefine.h:3236`) and `..._HP_BALL_ITEM` (`:4060`) are both live. The dungeon call (`DungeonRoom.cpp:6471`) sits inside the `m_bNoDrop == false` gate, as offline does.
- **Order.** Server factor, then the 3x boost (`SERV_IRUHADEV_OFFLINE_EXP_BOOST`), as planned.
  - Both `bFieldMiddleBoss` tests run before the only `m_setMiddleBossAlive.erase` (`OnFieldNpcDie`, `Handlers_Room.cpp:825`, called at `:2715`, after `PushNpcDrop` at `:2700`).
- **MODS.md** lists every committed file that uses the flag. `Always.h:5263` is right.

**Hygiene.** `flagcheck SERV_IRUHADEV_OFFLINE_MONSTER_GRADE --range 7c79f69^..HEAD`:
`files: 2 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
`in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
`RESULT: PASS`
- The deleted line is 7c79f69's own flagged one-liner `if( iED > 1999 ) return GOLD_BAR_ED_ITEM_ID;`, which `fee21f9` replaced inside the flag.
- PASS~ is blank lines beside the new blocks.
- The interleaved june-buff commits in the range carry no line of this flag.

**Known gap, older than this plan (user's call; not fixed).** The offline handler pays `NDS_KILL_BY_USER` and `NDS_ALLY` (`Handlers_Room.cpp:2521`, offline Phase 4 `b8bab69`). The server's `IsDropDieState` (`DungeonRoom.cpp:2576`, `BattleFieldRoom.cpp:853`) pays `KILL_BY_USER`, `KILL_SELF` and `BONUS_DROP`, not `ALLY`. With grades live:
- a boss that dies `KILL_SELF` loses its 70058 and its x12 ED offline;
- an ally kill is paid x12 where the server pays nothing;
- the mana/blood eater `BONUS_DROP` items stay unreachable.

The comment at `:2518-2520` states the opposite of the server. A fix would be its own small phase.

**Still owed by the user:** was the 70058 orb visible on the floor (Phase 3 run, dropUID 211)?
- **3b** is being implemented by a peer session as this review is written: uncommitted edits under this flag in `Handlers_Room.cpp`, `X2OfflineBattleField.*`, `X2OfflineDropTable.*` and `X2OfflineServer.h`. This review did not touch them.
- **3b's own review** must re-check `MODS.md`, which will then need `X2OfflineDropTable.cpp`.

## Phase 3b - Field middle-boss bonus items

**Scope.** `BattleFieldMiddleBossDropProcess`'s bonus items (`BattleFieldRoom.cpp:4700-4790`): 70070 (HP), 70050 (MP), the common
bonus item and the cubes, from the server-only `FieldBonusDrop.lua`. The user OK'd this on 2026-10-06 and asked for the file to go
into data036 as `FieldBonusDrop.src.lua`, with the manifest naming `FieldBonusDrop.lua` at algo 2. Run in the Phase 3 session.

### Phase 3b - Done

2026-10-06, Opus 5.5 high. Flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE`. **Play-test pending.**

**Data.**
- `kom_contents/data036_kom/FieldBonusDrop.src.lua` is byte-identical to `KncWX2Server/ServerResource/US/FieldBonusDrop.lua`, BOM and CRLF included. It compiles cleanly.
- `FieldBonusDrop.lua` is the `kom.py compile` output, and the `_manifest.json` entry is `"FieldBonusDrop.lua": "2"`.
- `PACK OK data036`: 185 entries, `+1 added, -0 removed, ~0 changed` against the live kom. Copied to the game only after the user OKs it.
- **Content:** 70 live rows (fields 40001-40032), all identical: common item 99988 x1, cube 160712 x2 (High) / x1 (Low). All four items are in March `item.kim`.

**Code.**
- `X2OfflineBattleField`: `BattleFieldMiddleBossDropInfo_LUA` is bound on `g_pBattleFieldManager`.
  - All seven keys are required, as `LUA_GET_VALUE_RETURN` drops the row on the server; a repeated (field, NPC) keeps the first (`XSLBattleFieldManager.cpp:425-461`).
  - `GetMiddleBossBonus` does an exact-key lookup, and the file loads once on the first lookup.
  - Degrade: if the file is missing, one `[OFFLINE_GRADE] 'FieldBonusDrop.lua' is not loaded` line.
- `X2OfflineDropTable::HasNpcRow` is the server's drop-row gate (`:4486`; `KDropTable.cpp:3361`). An all-zero row is not stored offline, so it reads as absent; this is noted in the header.
- **`PushNpcDrop`** takes the player's contribution, computed in the handler as damage / max HP, capped at 1.5, and 0 below 0.08 (`:4459-4467`). For a field middle boss (set member):
  - with a drop row, a bonus row and contribution > 0, it adds 70070 + 70050 + common x count + cube x **Low**;
  - the level factor is 1 offline, as for every other reward;
  - a missing templet logs and skips.
- **Server quirk kept:** `CalculatePartyRank`'s top count is `(int)(parties * 0.3)`, which is 0 for one party (`BattleFieldUserManager.cpp:1657`). So a solo player always gets the Low count: 1 cube, never 2.

**Hygiene.** `flagcheck --worktree --files` (6 offline files): `files: 2 PASS, 4 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,249,664 B, dated 2026-10-06 17:05:16, md5 307ebfa3ebe8209709dc0ff0c9c9dfed, equal to X2/US_SERVICE/x2.exe (2026-10-06 17:05:11); built at mods/june-buff-port 61b50f0 + 6 uncommitted code file(s).` (the six are this phase's). The `FieldBonusDrop.lua` and `bonus: contribution` strings are in the exe.

**How to see it** (tag `[OFFLINE_GRADE]`). Kill a field middle boss. First `'FieldBonusDrop.lua' loaded 70 row(s)`, then `field middle boss <id> bonus: contribution 0.xxx, 70070 + 70050 + 1 x 99988 + 1 x 160712`, with those four items on the floor.

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
