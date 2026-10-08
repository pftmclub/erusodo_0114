---
name: x2-kim
description: Read and query item.kim (the client's item templet table, March 188 B or June 228 B records) with tools/kim.py - look up an item id, search items by name, filter by type/class/slot/level/stat/special ability/buff, show a set, compare kims. Use when the user types /x2-kim, asks "what is item <id>", "find items named ...", "which items have buff/SAT/socket X", "list Elesis weapons above level 60", "what's in set N", or whenever a check needs item data instead of guessing from ScriptData/.
---

# Query item.kim

```sh
python tools/kim.py find --name "heretic essence"                       # name substring (case-insensitive)
python tools/kim.py dump 85003830 130044                                 # every field of these ids
python tools/kim.py find --where item_type=IT_WEAPON --where unit_class=elesis_knight \
                         --where "use_level>=60" --sort use_level --desc
python tools/kim.py find --where buff_factor=481 --cols id,name,buff_factor,period_type
python tools/kim.py find --where special_ability_type=SAT_HP_PERCENT_UP --count
python tools/kim.py find --ids 130044,85003830 --cols id,name,item_type,set_name --tsv
python tools/kim.py set 306          # or: set elsword  (words from the set name)
python tools/kim.py census --field eqip_position                         # value histogram
python tools/kim.py kims                                                 # which kims exist
```

`/x2-kim <anything>` maps onto these commands: a bare number means `dump <id>`, words mean
`find --name "<words>"`, and a condition means `find --where`. Read-only, stdlib only, no build.

**Which kim.** Every single-kim command takes an optional kim as its first argument, either a path or an alias.
The default is `staged`.
- `staged` (the default): `24965799\kom_contents\data036_kom\item.kim`, what `/elstools pack` puts in data036.kom.
  It has been **June's** kim since K2 (111,428 items, md5 `a16b6f6e...`), and it is what the deployed exe reads.
- `june` is the 14JUN17 original, `march` is the March 2014 original (49,837 items), and `march_patched` is March plus the buff Phase 13 patch.
- Example: `kim.py dump march 130044`. To compare two kims: `kim.py diff march staged --ids 1,2,3` (or `--common 300 --summary`).
- `info` prints the md5. Check it against the packed archive before saying "the game has X".

**Fields.** `find --where/--cols/--sort` and `census --field` share one namespace. List it with
`kim.py census --list-fields`. Fields differ by layout: June has `stat.slot3/slot4`,
`attach_frame_6/7`, `damage_range`, `item_equipment_group` and `pvp_equip`.
- Record fields include `item_level use_level price unit_type unit_class eqip_position set_id buff_id cool_time` and others.
- Strings: `name desc desc_in_shop shop_image model_name_N ani_xet ...`.
- Flag bits and nibbles: `fashion vested can_enchant ... item_type item_grade use_type period_type shop_price_type`.
- Pools: `stat.hp stat.atk_physic ...`, `stat_rel_lv.*`, `special_ability_type`, `buff_factor`,
  `n_buff_factors`, `socket_option`, `random_socket_option`, `attached_particle`, `attached_mesh`.
- `set_name` is resolved through the set table.

**Operators.** `= != < <= > >= ~ (substring) !~`. String compares ignore case.
- Pool fields match if **any** element matches; `!=` and `!~` mean **no** element matches.
- Enum fields take names, with or without the prefix: `item_type=weapon`, `eqip_position=EP_AC_MINIME`, `unit_class=UC_ELESIS_KNIGHT`.

**Enum names never guess.** A value with no sourced name prints as a bare number, for example June `unit_class` 123+
(`unit_class=125` still filters), or `SAT` 40. Under the June layout, `unit_type` 10/11/12 is Lu-Ciel/Rose/Ain, and
`eqip_position` 26/27 is `EP_AC_MINIME`/`EP_AC_EARRING` (March's unused `EP_AC_TEMP2/3`; K2 remaps them to 17/18 in game).
`--raw` prints numbers only.

**Output.**
- The table is capped at `--limit 50` by default, and the last line always says `N matched of M scanned, showing K`.
  Raise the limit only when the full list is actually needed; `--count` prints the number alone.
- `--tsv` and `--json` keep stdout machine-readable: the summary goes to stderr, and JSON adds `<col>_name` for enum columns.
- `--width 0` stops cell truncation.
- A full-table scan takes about 3 s. `--ids` scans only those ids.

When reporting, quote the ids and names from the output and name the kim you queried. Note that `staged` is not
proof the archive in the game directory matches: `/x2-kom-packed copy` is what deploys it.
