# Offline field and dungeon rewards - phased plan (`field-rewards`)

Backlog Stage 23 (`JUNE_PORT_BACKLOG_ORDER.md`, "Stage 23: offline field rewards stand-in"). Planned 2026-11-12 from the brief
`kickoffs/june-backlog/stage23-plan.md`. Plan key `field-rewards`, branch `main`, kickoffs `kickoffs/field-rewards/`.

## Context

The offline emulator pays EXP, ED and items only from the studio's server tables: `DropTable.lua` (`AddMonsterDropInfo`,
`AddBattleFieldNpcDropInfo`, `AddToGroup`) and `StaticDropTable.lua`, read by `X2Lib/Offline/X2OfflineDropTable.cpp`. Those tables are
March 2014's. Every dungeon and field June added after them has no row, so a kill there logs
`GAME -> NOTE monster N has no reward row for key K ... grants nothing` and pays nothing; the 30 % clear bonus is 30 % of 0.

There is no June server, so June's real rows do not exist anywhere. **User decision (2026-11-12):** build a proper drop table for that
content from what the client does ship: `item.kim` (which items exist, their type, grade and level) and each dungeon's entry list
(`BOSS_DROP_ITEM` in `DungeonData.lua`), split into regular-monster drops and boss drops, at fair rates.

This plan does that as a **labelled stand-in**: generated rows, appended to the two server Lua files inside one marked block, switched
by one flag, with every rate copied from the nearest real band of the studio's own table (Sander) and every item taken from June data.

## Rules

1. **Nothing is invented silently.** Every generated row traces to two cited sources: an item pool (item.kim ids, or a dungeon's
   `BOSS_DROP_ITEM`) and a rate row (`tools/data/drop_rate_sheet.tsv`, each line naming the studio row it was copied from). A value with
   no source is a `decision` for the user, written in the sheet as `source = user <date>`.
2. **Rows are generated, never hand-typed.** `tools/droptable.py gen` writes the block; a hand edit inside the block fails `check`.
3. **The studio's rows are never changed.** The block is appended; it never repeats a `(key, MonsterID)` pair or a group id the studio
   file already has (the loader's maps would overwrite it). Group ids come from the reserved range **20000-20999** (free in the server
   file today: its ids jump from 10250 to 28260).
4. **Specific keys only.** A stand-in row is keyed by its own dungeon id + difficulty or its own field id, never the wildcard key 0.
5. **No new `.lua` file** (CLAUDE.md). The rows live in the existing `KncWX2Server/ServerResource/US/DropTable.lua` and
   `StaticDropTable.lua`, both UTF-8 with BOM and CRLF: keep that byte-exact, ASCII only inside the block.
6. **Server Lua is the user's to pack** (CLAUDE.md server-Lua rule). A phase names the two files in its hand-off and proves the load from
   the log once they are packed; it never writes a loose copy into the game directory.
7. **Flag-off means the studio's table exactly.** The block is wrapped in `if DROP_STANDIN then ... end`; only the flagged client
   publishes that global, so commenting out the define removes every stand-in row with no repack.
8. **Rates are pre-boost.** `SERV_IRUHADEV_OFFLINE_DROP_BOOST` repeats the draw for every row, studio or stand-in. No stand-in rate is
   raised or lowered to allow for it.
9. Standing rules apply unchanged: one branch `main`, explicit pathspecs, no hardcoded enum literals in code (ids in a `.tsv` are data),
   no `June` in names, one-line comments, live DB over `DataBase/`.

## Starting context

- **Loader:** `X2Lib/Offline/X2OfflineDropTable.cpp`: `ReadDropBlock` (:296, fields `DungeonID` / `BattleFieldID`, `MonsterID`, `Exp`,
  `ED`, `EDProperty`, `DropItemList`, `DropGroupList`), `ReadStaticBlock` (:447), `AddToGroup_LUA` (:532), `Decide` (:550, one weighted
  pick over the accumulated list, roll 0.00-99.99), `GetNpcItemDrop` (:581, specific key then key 0), `GetNpcReward` (:743).
  `Exp` is a gate: above 0 the kill pays `NpcExpTable[npc level]` (rows 81-99 exist since cap 99). ED pays `10 + ED * npc level`.
  Henir and extra-stage rows are read and discarded on purpose (:426, :437).
- **Consumer:** `X2Lib/Offline/Handlers_Room.cpp` `Handler_EGS_NPC_UNIT_DIE_REQ` (~:3200-3360, key = dungeon id + difficulty, or the
  field id), item drop ~:3628-3680, static drop ~:3869. Existing log lines: `DROP     monster %d dropped item %d`, `DROP     static
  drop for key %d`, `GAME     -> NOTE monster %d has no reward row for key %d`.
- **Tools:** `tools/kim.py` (item.kim), `tools/docsection.py`, `tools/logs.py`, `tools/build.py --deploy`, `tools/flagcheck.py`,
  `tools/junediverge.py`, `tools/exestr.py`. Dungeon scripts are walked by `june2017_re/dungeon_npc_dump.lua` and the registry by
  `june2017_re/dungeon_registry_dump.lua`, both run through `elstools/vendor/luajit/luajit.exe` (see `dungeon_npc_census.py`,
  `dungeon_registry_census.py` for how). Never hand-type msbuild; never read this doc in full, pull sections with `docsection.py`.
- **Data:** live registry `kom_contents/data035_kom/DungeonData.lua` (BOM source, core list) and `data036_kom/DungeonEnum.lua`,
  `BattleFieldData.lua`, `Enum.lua`; field monster tables `june_src_fixed/data036_kom/<Region>_Field_0N.src.lua`; June dungeon scripts
  `june_src_fixed/data035_kom/Dungeon_*.src.lua`.
- **New in this plan:** `tools/droptable.py` (`census`, `pools`, `gen`, `check`, `simulate`), `tools/data/drop_census.tsv`,
  `tools/data/drop_item_pools.tsv`, `tools/data/drop_rate_sheet.tsv`.
- **Flag family:** `SERV_IRUHADEV_OFFLINE_DROP_STANDIN` (+ `_DEBUG`). **Log tag:** `[DROP_STANDIN]` (`offline_server.log`).

## Measurement (planning session, 2026-11-12)

**Server tables.** `DropTable.lua`: 2,127 `AddMonsterDropInfo` rows over 162 dungeon keys, 397 `AddBattleFieldNpcDropInfo` rows over
fields 40000-40025 plus key 0, 495 groups (max id 30080). `StaticDropTable.lua`: 144 dungeon keys, fields 40000-40025.

**Dungeons.** The live client registers 272 dungeon ids; 153 have server rows, 119 have none:

| Group | Keys | NPC level | Count | In scope |
|---|---|---|---|---|
| Lanox: Burned Forest, Ashes Village, Geyser, Gateway, Fire Temple, Final Hall | 33270-33322 | 70-76 | 18 | R3 |
| Elysion: Atlas Development, Atlas City, Tower | 33330-33352 | 80-84 | 9 | R4 |
| Elysion: Sky Crossroads, Adrian, Soless, Memory | 33870-33902 | 83-89 | 12 | R4 |
| Secret: Hamel common / hell, Ranox hell | 33260, 33261, 41271 | 70-99 | 3 | R5 |
| Henir | 38100 | 99 | 1 | no: the loader discards Henir rows by design (R1 decision 4) |
| Defence | 38300-38302 | - | 3 | no (R1 decision 4) |
| Retired secret N/H/E (Wally Lab, Besma, Altera), El Forest Gate, arcade, events | - | - | about 23 | no: retired or event content |
| Training, tutorial, test | - | 0 | about 50 | no: no reward by design |

Key 41270 (`UNUSED_DUNGEON_ID.DI_RANOX_SECRET_COMMON`) has 5 server rows; R0 checks whether they are that dungeon's monsters or a
March dungeon that owned the id.

**Heroic dungeons.** June registers 191 `DI_*_HERO_*` rows (53 dungeons, ids from 100000, NPC level 85-87). The live client registers
none of them, so there is nothing to key rows on yet (R7 waits for the dungeons).

**Fields.** No row for Lanox `VMI_BATTLE_FIELD_RANOX_FIELD_01..04` (40029-40032) or Elysion `..._ELYSION_FIELD_01..03` (40035-40037);
40038 is an enum value with no field row. Monsters, from the field tables:

| Field | Monsters (`NUI_` prefix dropped) |
|---|---|
| 40029 | `LOW_MAG_NOM_FD`, `LOW_MAG_STONE_FD`, `LOW_MAG_CHARGER_FD`, `LOW_RAGE_BABYFIG_FD`, `LOW_RAGE_DRYAD_FD`, `LOW_SMITH_*_FD` (5), bosses `LOW_RAGE_FATHERFIG_FD_BOSS`, `LOW_SMITH_ALUMINUM_FD_BOSS` |
| 40030 | `LOW_GEYSER_*_FD` (7), boss `LOW_GEYSER_ALUMINUM_FD_BOSS` |
| 40031 | `LOW_MOUNT_*_FD` (6), boss `LOW_MOUNT_BERSERKER_BOSS` |
| 40032 | `LOW_FINALHALL_*_FD` (5), boss `LOW_FINALHALL_IGNIA_BOSS` |
| 40035 | `ELSION_F_BERMEVA`, `_PIMPIM`, `_PLINGCHU_G`, `_PLINGCHU_R`, `_PYURITA`, `_SHILEDPIM`, `_DEKALPYURITA`, `ELSION_CHEST` |
| 40036 | `ELSION_F_DECEPTOR`, `_DEFENDER`, `_LIBYA`, `_PROTO`, `_PROVER`, `_PUTER`, `_TEMPLAR`, `_HIGHTTEM`, `_NEWDEFENDER` |
| 40037 | `ELYSION_F_HERNA_BOW`, `_FEAR`, `_LANCE`, `_ORB`, `_SHIELD`, bosses `ELYSION_F_BOSS_ORB`, `_BOSS_SHIELD`, `ELYSION_NEW_CHEST` |

The raid field (`Raid_Field_00`, `NUI_FIELD_BOSS_IRON_ELTRION`) is not in this plan: raid rewards are their own system.

**Real data found.**

1. **Boss items: June's own dungeon-entry lists.** Every gap dungeon's `BOSS_DROP_ITEM` names its boss drops (`COMMON` plus one list per
   character): 121 distinct ids across the gap dungeons, all 121 present in item.kim. Lanox: three boss accessories per dungeon
   (`Kenta's`, `Sdeing's`, `Sirena's`, `Ignia's`, `Ifritan's`, `Scar's`, ids 30329-30365) and character weapons (`Crystallized ... of
   Fire`, `... of the Steel Anvil` L72; `Red / Blue Lava Demon` L78). Elysion: accessories 30426-30434, 30563-30576 and weapons `Atlas
   Dicetium` L82, `Elysion's Hernacyd` L86. Secret: materials 133537 / 133535 + set gloves / shoes (common), 154066 `Barrier's Fragment`
   + `Sacrificed Adventurer's` pieces 700009-700014 (hell). Lu/Ciel, Rose and Ain weapons are in the lists.
2. **The studio's shape for exactly this.** Behemoth Heart (33250-33252, the last real dungeon) shows how entry items become rows: the
   final boss (1411) carries groups 2127 / 2128 / 2129 = that dungeon's `BOSS_DROP_ITEM.COMMON` 30272 / 30273 / 30274 at 0.6 / 3 / 0.6
   (Normal) and 1.1 / 5.4 / 1.1 (Expert); stage bosses (1387-1389, 1409) carry two character boss weapons at 0.5 each (0.9 Expert).
3. **Regular-monster gear: the region ladder in item.kim.** Ids `10LL0C0G` (level, character, grade): Sander L64 `Demon Raider`, L68
   `Demon Subjugator` are what Sander's rows drop (groups 181-192). The same ladder continues: L72 `Radiant Fire`, L76 `Exploding
   Flame`, L80 `Steel Demon Soldier`, L82 `Atlas Commander` (ids `10801C2G`), L87 `Elysion Guard's` (`10851C2G`), each in NORMAL /
   Intermediate RARE / Advanced ELITE / Exceptional UNIQUE.
4. **Dungeon sets.** Sander's bosses drop `Fallen Behemoth` / `Demon Guard` (RARE, groups 1160-1174). Counterparts exist: `Hidden Flare`
   L72 (117561..), `Roaring Berserker` L76 (117679..), `Enchanted Diceon` L84 (117744..), `Hernacyd Guard` L89 (117907..).
5. **Consumables and craft materials.** Static rows drop the four common cooking / alchemy materials (99809 `Whole Grain Flour`, 99810
   `Ruve Herb`, 99811 `Aqua`, 99812 `Stella's Powder Mix`) plus one region pair (Sander: 99823 `Crumbly Meat`, 99824 `Cactus Stem`).
   item.kim has no later pair (99825 onward are El Gems), so R1 looks for Lanox / Elysion materials by recipe and name. Boxes (NPC 20,
   29) drop food and potion groups 6002-6008. Stones: group 4005 (Enhancement Stone Lv.6) and 4015 (Magic Stone); Lv.7-9 stones exist.

## Design: the stand-in

**Monster roles** (R0 assigns one per spawned NPC, from the script, never by name alone):

| Role | How R0 tells | Rate template (Normal; studio row it is copied from) |
|---|---|---|
| `mob` | an `NPC_GROUP` entry with none of the marks below | 33250 x 1382: Exp 1, ED 16, EDProperty 100; region NORMAL weapon 0.4, RARE weapon 0.08, NORMAL armour 0.4, RARE top / bottom / gloves / shoes 0.4 each, enhancement stones 0.6, magic stones 0.6 |
| `mob-minor` | summoned or `REWARD_RATE` below 1 | 33250 x 1415: every rate above halved |
| `mob-card` | a mob whose monster card exists in item.kim | `mob` + its card at 0.3 (33250 x 1378) |
| `stage-boss` | `GAGE_BAR` / `MONSTER_GRADE`, not the last stage | 33250 x 1387: ED 17; RARE weapon 0.53, ELITE weapon 1.23, RARE armour piece 0.11 each, ELITE armour piece 2.52 each, stone bundle (group 4090) 20, its character boss weapons 0.5 each, dungeon-set pieces 1 / 1.5 |
| `final-boss` | `HAVE_BOSS_GAGE` / `SHOW_BOSS_NAME` in the last stage, or the `CLEAR_COND` id | 33250 x 1411: the `stage-boss` gear lines; `Ancient Jewelry Box` 113276 at 0.07; entry `COMMON` items at 0.6 / 3 / 0.6; dungeon-set weapons 2 each |
| `box` | NPC 20 / 29 and June's chests | 33250 x 20: food and potion groups 30 / 30 / 20 / 20 |
| `none` | `NO_DROP`, `DROP = FALSE`, gates, traps, cut-scene actors | no row |
| `field-mob` | field table entry with `DROP = TRUE` | 40024 x 5143: 0.28 / 0.06 / 0.28 / 0.28 x4 / 0.6 / 0.6, ED 16 |
| `field-boss` | field `_BOSS` entry | 40024 x 5124: Exp 1, ED 17, no items (R1 decision 3) |

Hard and Expert use the studio's own multipliers, read from 33251 / 33252 against 33250 (Expert boss lines are x1.8).

**Item pools** (R1 fixes each list in `drop_item_pools.tsv`):

| Pool | Lanox | Elysion |
|---|---|---|
| Region gear, by dungeon level | L72 `Radiant Fire` (lv 70-73), L76 `Exploding Flame` (lv 74-76) | L80 `Steel Demon Soldier`, L82 `Atlas Commander`, L87 `Elysion Guard's` (split by level in R1) |
| Dungeon set (bosses) | `Hidden Flare` L72, `Roaring Berserker` L76 | `Enchanted Diceon` L84, `Hernacyd Guard` L89 |
| Boss items | the dungeon's own `BOSS_DROP_ITEM` | the dungeon's own `BOSS_DROP_ITEM` |
| Stones | enhancement + magic stone groups (level: R1 decision 2) | same |
| Static (craft materials) | 99809-99812 + the region pair R1 finds, else Sander's pair | same |

Secret dungeons (R5) copy the rate rows of the secret dungeons the studio table does have (33030 / 33031 and siblings), with each
dungeon's own entry items. Heroic dungeons (R7) have no studio template; their sheet is a decision of that phase.

**Fairness gate.** `droptable.py simulate <key>` computes the expected items per full clear, by pool, from the census spawn counts. A
generated dungeon passes when each pool is within 15 % of the same figure for Behemoth Heart at the same difficulty, and a field when
its per-kill expectation equals Sander Field 03's. The run prints both the one-draw and the `OFFLINE_DROP_BOOST` figures.

**Mechanism.** The generated block in each server file:

```lua
--{{ Iruha : <date> // field-rewards stand-in, generated by tools/droptable.py (OFFLINE_FIELD_REWARDS_PHASES.md) - do not hand-edit
if DROP_STANDIN then
DropTable:AddToGroup( 20000, 10720100, 12.5 )
DropTable:AddMonsterDropInfo
{
	DungeonID = 33270,
	MonsterID = 1500,
	StandIn = 1,
	...
}
end
--}}
```

Under `SERV_IRUHADEV_OFFLINE_DROP_STANDIN` the loader publishes `DROP_STANDIN` as the number 1 before the two files run (the way
`CX2OfflineLuaEnum::Publish` publishes `True`), reads the optional `StandIn` field, and logs one summary line. With the define
commented out the global is nil and Lua skips the block.

## Phase R0 - Census: every spawned monster against the reward tables

**Scope.** Build `tools/droptable.py census` and write `tools/data/drop_census.tsv`: one row per (dungeon key incl. difficulty or field
id) x spawned NPC, with NPC id and name, NPC level, stage / sub-stage, spawn count, role (table above) and the marks that decided it,
and the server-row state (`specific`, `wildcard`, `none`). Covers every dungeon the live registry registers and every field in
`BattleFieldData.lua`. Read-only: no client code, no Lua edit.

**Run with.** Sonnet 5.5 / high.

**Read list.** "Starting context", "Measurement (planning session, 2026-11-12)", "Design: the stand-in".

**Done gate.**
- Totals reproduce the planning numbers: 272 registered dungeons, 153 with rows; fields 40029-40032 and 40035-40037 with none.
- Every NPC in an in-scope key has a role; the `none` list and any `unknown` are listed in the Done note for R1.
- Key 41270's 5 server rows are identified (that dungeon's monsters or not).
- A second sheet section lists NPCs with state `none` inside dungeons that do have rows (R6's input; this is june-dungeons C3z-2b's
  question, answered here).
- Full build skipped: no play-test.

## Phase R1 - Item pools and the rate sheet (user approves)

**Scope.** Write `tools/data/drop_item_pools.tsv` (pool, region, level band, item id, name, type, grade, source) with `droptable.py
pools`, and `tools/data/drop_rate_sheet.tsv` (role x pool x difficulty -> probability, `source` = the studio row). Read the Hard and
Expert rows (33251, 33252), the Sander dungeon static rows and the secret-dungeon rows to fill the multipliers. Find the Lanox /
Elysion craft materials and monster cards in item.kim by recipe and name. Run `simulate` on one Lanox and one Elysion dungeon.

**Run with.** Opus 5.5 / high. Then the phase stops in state `decision`.

**Read list.** "Rules", "Measurement (planning session, 2026-11-12)", "Design: the stand-in"; R0's Done note.

**Decisions put to the user, each with a default:**
1. Boss weapons: class-blind like the studio's rows (default, no code), or only the player's own character (an emulator filter under
   its own flag; a solo player otherwise sees mostly other characters' weapons).
2. Enhancement-stone level for Lanox and for Elysion, if item.kim / `EnchantItemInfo` give no level rule (default: what the data says;
   otherwise the user picks).
3. Field bosses: EXP and ED only, as Sander's (default), or the nearest dungeon `stage-boss` gear lines.
4. Henir and the Defence dungeon stay out (default).

**Done gate.** Both sheets committed; every pool item resolves in item.kim; every rate line has a `source`; the `simulate` figures and
the four decisions are in the Done note and the hand-off; STATUS row `decision`. Full build skipped: no play-test.

## Phase R2 - Loader flag and generator

**Scope.**
- `KTDXLIB/Always.h`: `SERV_IRUHADEV_OFFLINE_DROP_STANDIN` and `SERV_IRUHADEV_OFFLINE_DROP_STANDIN_DEBUG`.
- `X2OfflineDropTable.cpp/.h`: publish `DROP_STANDIN` before the drop scripts run; read `StandIn` in `ReadDropBlock` / `ReadStaticBlock`
  and keep it on the row; count stand-in rows; one line
  `[DROP_STANDIN] loaded: %d dungeon row(s) over %d key(s), %d field row(s) over %d field(s), %d static row(s), %d group(s)`; when the
  count is 0, `[DROP_STANDIN] no stand-in rows - pack KncWX2Server/ServerResource/US/DropTable.lua and StaticDropTable.lua into
  data036.kom`.
- `Handlers_Room.cpp` under `_DEBUG`: a throttled per-kill line `[DROP_STANDIN] key %d npc %d -> exp %d ed %d item(s) %d` when the row
  is a stand-in.
- `tools/droptable.py gen` (census + pools + sheet -> the block, CRLF, ASCII, deterministic order, `--dry-run`, `--only <region>`) and
  `check` (block equals a fresh `gen`; no pair or group id shared with the studio rows; group ids inside 20000-20999; every item in
  item.kim; BOM and CRLF intact; outer probabilities of a row sum to 100 or less).

**Run with.** Sonnet 5.5 / high; review Opus 5.5 / medium (`R2-review.md`).

**Read list.** "Rules", "Starting context", "Design: the stand-in".

**Done gate.** `flagcheck` PASS for both flags, `junediverge --check` PASS, one-file probe build with 0 errors; `gen --dry-run` prints
row counts and `check` passes on the dry output; no row is written to the server Lua yet. Full build skipped: no play-test.

## Phase R3 - Lanox: six dungeons and four fields

**Scope.** `gen --only lanox`: keys 33270-33322 (18), fields 40029-40032, their static rows and groups. `check` and `simulate` every
key. Build and deploy. Hand the two server Lua files to the user to pack.

**Run with.** Sonnet 5.5 / medium. Reviewed with R4-R6 in `R6-review.md`.

**Read list.** "Rules", "Design: the stand-in", "Play-test hand-off"; R1's and R2's Done notes.

**Done gate.** `check` PASS; fairness gate PASS for all 18 keys and 4 fields (figures in the Done note); `DEPLOY OK`; `EXESTR OK` for
`[DROP_STANDIN]`; after the user's pack, the `[DROP_STANDIN] loaded:` counts equal `gen`'s; play-test: no `no reward row` line for a
Lanox key, EXP and ED rise per kill, a boss item from the entry list drops on a forced run.

## Phase R4 - Elysion: seven dungeons and three fields

**Scope.** `gen --only elysion`: keys 33330-33352 and 33870-33902 (21), fields 40035-40037 (chests take the `box` role), static rows.

**Run with.** Sonnet 5.5 / medium. Reviewed in `R6-review.md`.

**Read list.** As R3, plus R3's Done note.

**Done gate.** As R3, for the Elysion keys.

## Phase R5 - Secret dungeons

**Scope.** Hamel common / hell (33260, 33261) and Ranox hell (41271), plus 41270 if R0 found its rows are not that dungeon's. Rates from
the studio's own secret rows (33030 / 33031 and siblings), items from each dungeon's entry list. Report, without changing, any secret
dungeon whose studio rows name monsters the June script no longer spawns (R6 covers the reverse).

**Run with.** Sonnet 5.5 / high. Reviewed in `R6-review.md`.

**Read list.** "Rules", "Design: the stand-in", "Play-test hand-off"; R1's Done note.

**Done gate.** As R3, for the secret keys.

## Phase R6 - June-only monsters in dungeons that have rows

**Scope.** R0's second sheet: NPCs June added to old dungeons (the 142 of june-dungeons C3z-2, and any others) that have no specific and
no wildcard row. Each gets a row copied from a same-role sibling in the same dungeon and difficulty (source = that row). This phase
replaces june-dungeons C3z-2b.

**Run with.** Sonnet 5.5 / high; review Opus 5.5 / high (`R6-review.md`, covering R3-R6: `check`, `simulate`, `flagcheck --range`,
`junediverge --check-all`, and it retires `_DEBUG`).

**Read list.** "Rules", "Design: the stand-in", "Play-test hand-off"; R0's Done note.

**Done gate.** Census state `none` is 0 for every in-scope key; `check` PASS; one play-test smoke in one old dungeon with a June-only
monster.

## Phase R7 - Heroic dungeons

**Scope.** Blocked until the live registry holds June's `DI_*_HERO_*` rows (june-dungeons; none of the 191 is registered today). Then:
re-run `census`, build the heroic pools from their entry lists (`Heroic Gear Piece` 162392, `Heroic Gear Chest` 162393, the per-dungeon
ids 1868xx / 1885xx), propose a rate sheet (no studio template exists; nearest is secret hell) for the user's yes, `gen --only heroic`.

**Run with.** Opus 5.5 / high.

**Read list.** "Rules", "Measurement (planning session, 2026-11-12)", "Design: the stand-in"; R5's Done note.

**Done gate.** As R3, for the heroic keys, after the user approves the heroic sheet.

## Phase Z - Close

**Scope.** `MODS.md` entry (both flags, the block, how to revert: comment out the define); backlog Stage 23 marked done; the
`droptable.py` row in `tools/README.md`; STATUS block archived. Runs when R6's review is done; R7 stays as one open row if still blocked.

**Run with.** Sonnet 5.5 / medium.

**Read list.** Every Done note of this plan.

**Done gate.** `check` PASS on the committed files; `STATUS_ARCHIVE.md` holds the block. Full build skipped: no play-test.

## Play-test hand-off

Each rows phase (R3-R6) hands over, copy-ready:
1. **Pack first.** The user XOR-encrypts and packs `KncWX2Server/ServerResource/US/DropTable.lua` and `StaticDropTable.lua` into
   `data036.kom`. Until then the log says `[DROP_STANDIN] no stand-in rows`.
2. **Character.** A unit inside the region's level band (Lanox 70-76, Elysion 80-89), made or levelled with `/x2-create-char` /
   `/x2-db`; the hand-off names the unit and the exact command.
3. **Where.** One field and one dungeon of the phase, with the full dev-console line to get there (field warp ids: atlas A6 Done note,
   `FieldWarpByCommand`), `/invin 0`, and the stage command for the boss room.
4. **Look for.** EXP and ED on every kill; items on the floor within a few dozen kills; at the boss, an item from the dungeon's entry
   list (the hand-off lists the ids and names).
5. **If not.** Which log line is missing and what it means.
6. **Log check.** `python tools/logs.py --tag DROP_STANDIN`, plus `--grep "no reward row"` scoped to the phase's keys: the first must
   show the `loaded:` line with `gen`'s counts and per-kill lines, the second must be empty for those keys.
