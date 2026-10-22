---
name: x2-db
description: Query or mutate the offline save (els_db.sql, SQLite) with tools/elsdb.py - run SQL, list characters, show inventory, add/delete items (bag/stack/equip placement from item.kim), set level/ED/SP, flip settings, back up. Use when the user types /x2-db, or asks to add/give/delete an item, change a character's level/ED/SP/name/class, "check the save", "query els_db", or whenever a play-test needs save state set up. Replaces hand-written sqlite3 scripts.
---

# els_db.sql with tools/elsdb.py

```sh
python tools/elsdb.py info                      # path, schema version, row counts, is the game running
python tools/elsdb.py chars                     # uid / class / nick / level / ed / sp / item count
python tools/elsdb.py q "SELECT ..." [--tsv|--json] [--limit N]   # read-only; safe while the game runs
python tools/elsdb.py inv <unit> [--cat N]      # items with names from item.kim
python tools/elsdb.py schema [table]
python tools/elsdb.py setting [name [value]]    # cash_start, auth_level (3 = dev console), select_count.*
python tools/elsdb.py settranscend <unit> <0|1> # unit.transcendence (schema v22): June's transcended flag; needs the v22 exe to have opened the save once

# writes: backup first, one transaction, integrity_check after; add --dry-run to rehearse (rolls back, no backup)
python tools/elsdb.py addchar <nick> <class> <level>              # = createchar.py (/x2-create-char)
python tools/elsdb.py setchar <unit> --level 80 --ed 5000000 --sp 600 [--exp N] [--nick S] [--class C]
python tools/elsdb.py additem <unit> 72000:250 128005 [--enchant N]
python tools/elsdb.py additem <unit> 77 --equip [--replace]       # straight into the worn slot
python tools/elsdb.py delitem <unit> <item_uid> ...   |   delitem <unit> <item_id> --by-item-id
python tools/elsdb.py x "UPDATE unit SET ed=1 WHERE unit_uid=20"  # any SQL; also --file stmts.sql
python tools/elsdb.py backup --tag before_x | backups
```

`<unit>` is a unit_uid or a nickname. `<class>` is a UNIT_CLASS id or name fragment. Items are
`<item_id>` or `<item_id>:<count>`. Find an item id first with `/x2-kim` (`tools/kim.py find`).

## Rules

- **Writes refuse while `X2_offline.exe` runs** (the emulator holds the DB). Tell the user to close the
  game; never kill it. Reads (`q`, `chars`, `inv`, `info`) still work.
- **Save edits need no ask** (user, 2026-10-06) when a play-test needs them - the tool backs up into
  `<data>/db_backup/els_db_<stamp>_pre_<cmd>.sql` first. A refused or failed command removes its own
  backup (nothing was written). Report the printed `backup` path and the `committed | integrity_check: ok` line.
- **Never launch the game afterwards** - ask. Never delete `els_db.sql`; there is deliberately no restore
  command (restore = close the game, copy a backup over `els_db.sql`, delete `-wal`/`-shm`; only on request).
- `x` refuses `DROP/ALTER/CREATE` (the emulator migrates the schema forward itself; `--allow-ddl`) and
  `UPDATE/DELETE` without `WHERE` (`--allow-all`). Prefer `--dry-run` first on anything bulk.
- `additem` mirrors `CX2OfflineInventory::InsertItem`: bag from the kim item type (weapon/armor 1,
  accessory 2, material 3, special/skill-memo 4, quest 5, quick-slot 6, fashion 7, pc-bang 12), quantity items
  fill existing stacks then open new ones at the kim's max stack, endurance items start at the templet's
  endurance, first free slot under `inventory_size`. A full bag stops with nothing written. `IT_OUTLAY` / `IT_ETC`
  / pet / riding items have no bag in the emulator - the tool says so; `--cat N` forces one.
  `--equip` writes category 9 at the worn slot for the item's equip position (hair-fashion 0, body 1/2, leg 3/4,
  hand 5/6, foot 7/8, weapon 9/10, accessories 11-19, one-piece 20; odd real / even fashion).
  Item ids come from `staged` kim (June's, what data036 ships); an id not in it is refused (`--kim march` to override).
- `setchar --level N` also sets `exp` to that level's March `TOTAL_EXP` (same table as addchar). It does not
  grant skill points or reseed skills.
- Not covered by a subcommand (use `q`/`x`): skills (`unit_skill`), titles (`unit_title`), quests, pets,
  dungeon clears, buffs, cash balance (`account.cash_balance`). Read the table with `q` first and mirror a real row.

## Table map (schema 14; `schema <table>` is the truth)

| Table | Holds / key |
|---|---|
| `account` | one row; `unit_slots` (character cap), `cash_balance` |
| `unit` | one row per character; `unit_uid` AUTOINCREMENT, `del_date <> reg_date` = deleted; level/exp/ed/sp/title_id/csp |
| `item` | `item_uid`; (unit, category, slot) is the position; `category 9` = worn (`equipped=1`); `count`, `usage_type` (0 infinite / 1 endurance / 2 quantity), `endurance`, `enchant_level`, `sockets` TEXT |
| `inventory_size` | (unit, category) -> bag size |
| `unit_skill` | (unit, skill_id) -> level, hotkey `slot` (-1 none), `csp`; `unit_skill_unsealed`, `unit_skill_note` |
| `unit_title`, `unit_quest`, `unit_quest_complete`, `unit_subquest`, `unit_mission`, `unit_submission`, `unit_dungeon`, `unit_tc_clear` | progress, keyed by `unit_uid` |
| `unit_pet`, `unit_riding_pet`, `unit_buff` | per-unit pets / mounts / buffs |
| `cash_product` (41k rows), `cash_order`, `wish_list` | cash shop catalogue (the cash-shop tool owns it) and history |
| `settings` | name -> int: `cash_start`, `auth_level`, `select_count.<class>` |

Bag categories (`item.category`): 1 equip, 2 accessory, 3 material, 4 special, 5 quest, 6 consumable,
7 avatar, 8 bank, 9 worn, 11 quick-slot bar, 12 pc-bang, 13 pet, 14 share-bank.

## Recipes

```sh
python tools/elsdb.py q "SELECT unit_uid,nickname,level FROM unit WHERE unit_class=120 AND del_date=reg_date"
python tools/elsdb.py q "SELECT skill_id,level,slot FROM unit_skill WHERE unit_uid=29 ORDER BY skill_id"
python tools/elsdb.py q "SELECT category,COUNT(*) n FROM item WHERE unit_uid=12 GROUP BY 1"
python tools/elsdb.py x "UPDATE account SET cash_balance=999999" --dry-run
python tools/elsdb.py x "UPDATE unit_skill SET level=10 WHERE unit_uid=29 AND slot>=0"
```

When a check needs a character, name the character and job from `chars` in the hand-off (standing play-test rule).
For the DB that holds the schema's history, the emulator side is `X2Lib/Offline/X2OfflineDB.cpp` (`Migrate()` ladder).
