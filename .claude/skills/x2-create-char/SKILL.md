---
name: x2-create-char
description: Create a character directly in the offline save (els_db.sql) with a given nickname, class and level, using tools/createchar.py. Use when the user types /x2-create-char <nickname> <class-id> <level>, or asks to "add / create a character" of some class or job for testing.
---

# Create a test character in els_db.sql

```sh
python tools/createchar.py <nickname> <class> <level>            # backs up, inserts, verifies
python tools/createchar.py <nickname> <class> <level> --dry-run  # print what it would insert
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

What it does (mirrors `CX2OfflineDB::CreateUnit`): one `unit` row on the first account, plus the
default `inventory_size` rows copied from a level-1 unit. It does **not** seed skills, hotkeys,
gear or the promotion costume - say so when you report, and tell the user to learn skills / equip in
game (or ask for a skills-seeding follow-up; see `X2OfflineSkill::SeedDefaultSkills`).

Guards (all hard stops, nothing is written): `X2_offline.exe` running; no save; account out of
slots; nickname taken or not 2-16 ASCII letters/digits; unknown class; level outside 1-80.
Every write is preceded by a full backup into `<data>/db_backup/els_db_<stamp>_pre_createchar.sql`.

Rules that still apply (CLAUDE.md): the save is real player data - never delete or edit an existing
unit with this; never launch the game afterwards (ask). Report the printed line verbatim
(`inserted unit_uid=... integrity_check: ok`) and the backup path. If the game is running, tell the
user to close it and rerun; do not kill it.
