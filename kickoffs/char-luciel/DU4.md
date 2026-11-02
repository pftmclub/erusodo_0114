# June character Lu/Ciel (Stage 8) - Phase DU4 - NESI renumber and the save migration

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase char-luciel DU4` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | DU3 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_NESI_RENUMBER` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL DUO`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/DU4-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase DU - The two-body engine (DU1-DU5)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the DU4 row of "Phase DU" (user, 2026-11-02: June's numbering, forward-only). **Snapshot `db_backup/` before the first test** and
write the migration from NU0 item 7's shape, one schema step. Retire `SERV_IRUHADEV_JUNE_CHARINFO_EQUIP_GROUP` and the `IS_FOR_MINOR_UNIT`
stand-in in the same commit. Play-test: three characters' equipment, fashion and onepiece show identically before and after, and survive a relog;
Ciel's minor-unit slots hold their items.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by NU0

NU0 (2026-11-02), `nu0_equip_nesi.tsv` (`migration` rows). **Map** (checked against June `Enum.src.lua:6653-6696`): `NESI_*` 0-19 keep their
values and names; `NESI_ONEPIECE_FASHION` 20 -> 35; `NESI_END` 21 -> 43; June adds Ciel's slots 20-34, 36, 38, 40, 42
(`NESI_MINORUNIT_DEFENCE_HAIR_FASHION` = 20 ...) and the main body's `NESI_AC_MINIME` 37, `NESI_AC_EARRING` 39, `NESI_AC_RING2` 41.
`SERV_NEW_ONE_PIECE_AVATAR_SLOT` is on for US, so a March save can hold slot 20.
**Save shape** (schema v22 now, `X2OfflineDB.h:452`, rung `X2OfflineDB.cpp:1195`; the step is v23 under this flag, `#error` guard on
`SERV_IRUHADEV_JUNE_TRANSCEND_UNIT`):
1. `item.slot` where `category` = the equipped category (9): `UPDATE item SET slot = 35 WHERE category = 9 AND slot = 20`.
2. `inventory_size.size` for category 9: 21 -> 43 (`UPDATE inventory_size SET size = 43 WHERE category = 9 AND size = 21`); the loader only
   fills missing rows, so a stale 21 leaves slot 35 out of range.
No other table holds an NESI id (`unit_skill.slot`, `unit_skill_page.slot` and bag slots are not NESI). C++ in `X2Lib/Offline` is symbolic (comments
only carry numbers); `IsSuitableSlot` (`X2OfflineInventory.cpp:197-239`) needs the new slot cases. Tools: `elsdb.py` `EQUIP_SLOTS[20]` -> 35;
`createchar.py` `COSTUME_SLOTS` unchanged. Client stand-in to retire: `X2Inventory.cpp:1284`; slot hide `32e22ce0` (main {0,2,...,19,35,37,39,41}
hidden in the switching view; Ciel {20..34,36,38,40,42} shown only there).
