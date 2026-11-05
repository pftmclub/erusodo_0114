---
name: x2-create-char
description: Create a character directly in the offline save (els_db.sql) with a given nickname, class and level, using tools/createchar.py. Use when the user types /x2-create-char <nickname> <class-id> <level>, or asks to "add / create a character" of some class or job for testing.
---

# Create a test character in els_db.sql

```sh
python tools/createchar.py <nickname> <class> <level>            # backs up, inserts, verifies
python tools/createchar.py <nickname> <class> <level> --dry-run  # print what it would insert (incl. the item ids)
```

`/x2-create-char <nickname> <class-id> <level>` maps straight onto those three arguments. The first
argument is the **nickname**, not a unit uid: `unit_uid` is `AUTOINCREMENT`, so the save picks it
and the script prints it. If the user gave something that is clearly a uid, ask what name they want.

- `<class>` is a `CX2Unit::UNIT_CLASS` id (`120`) or a unique name fragment (`grand_master`,
  `blazing`, `uc_elesis_knight`). An ambiguous fragment lists the candidates and stops. Ids:
  basics 1-9, 1st jobs 10-32, 2nd jobs 100-122 (Elesis: 8 Knight, 29 Saber, 30 Pyro,
  120 Grand Master, 121 Blazing Heart). Table is `CLASSES` in the script (from `X2Lib/X2Unit.h`).
- `<level>` is 1-99. Exp is that level's `TOTAL_EXP` from the **June** ExpTable
  (`tools/data/june_exptable.tsv`; data036 ships June's `ExpTable.lua` since Atlas A2, and the save was
  migrated onto it at schema v16). `march_exptable.tsv` is kept only as the migration's source record.
- SP/ED default to `1`/`0` at level 1, else `600`/`1000000`; override with `--sp` / `--ed`.

What it does (mirrors `CX2OfflineDB::CreateUnit`): one `unit` row on the first account, the default
`inventory_size` rows copied from a level-1 unit, the class's free June skills, and (new) items:

- **Promotion costume, worn.** The class's **own** five-piece promotion set goes into the fashion slots 10/2/4/6/8,
  infinite (endurance -1). The ids are the `promotion*ItemID` fields of the class's row in the live
  `kom_contents/data036_kom/UnitTemplet.lua` - the same table the create screen's job preview reads
  (`CX2StateCreateUnit::EquipPromotionItemByUnitTemplet`), so Grand Master wears the Grand Master costume, not Elesis's.
  The base class's set (`PROMO` in the script, the `GBase_Item` rows) is only the fallback when the file or the row is
  missing; the dry run names the source line (`from UnitTemplet.lua row UC_...`).
- **Top-tier gear, +10, in the equip bag** (category 1, slots 0-4, not worn): a weapon plus a complete 4-piece
  armor set. `top_gear` picks from `item.kim` (`--kim`, default `staged`) the highest item_level / grade / use_level
  the job can wear: enchantable, not fashion / pc-bang / test / `[event]` / untranslated, use level <= 99.
  Today that is the **Apocalypse Type - Void** weapon of the base class (item_level 120, use 95) plus
  **Henir's Time and Space 4th Dimension** armor (item_level 120, use 90, any class). Dry-run prints the exact ids.
  A character below the use level carries them until it levels up (the tool prints a note).
- Flags: `--no-costume`, `--no-gear`, `--enchant N` (default 10), `--kim X`. Gear is **not** socketed or sealed.

It does **not** seed hotkeys or the beginner gear - tell the user to equip the bagged gear in game.

Guards (all hard stops, nothing is written): `X2_offline.exe` running; no save; account out of
slots; nickname taken or not 2-16 ASCII letters/digits; unknown class; level outside 1-80.
Every write is preceded by a full backup into `<data>/db_backup/els_db_<stamp>_pre_createchar.sql`.

Rules that still apply (CLAUDE.md): the save is real player data - never delete or edit an existing
unit with this; never launch the game afterwards (ask). Report the printed line verbatim
(`inserted unit_uid=... integrity_check: ok`) and the backup path. If the game is running, tell the
user to close it and rerun; do not kill it.
