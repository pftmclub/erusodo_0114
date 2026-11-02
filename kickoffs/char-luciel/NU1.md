# June character Lu/Ciel (Stage 8) - Phase NU1 - Unit-type range (June's `UT_END`)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel NU1` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU0, char-chung Z (user, 2026-11-02: every Lu/Ciel code phase waits for it) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_UNIT_TYPE_RANGE` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL NEWUNIT`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/NU1-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase NU - New-character foundation (NU1-NU4)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the NU1 row of "Phase NU": every NU0 item-1 site (`nu0_unit_type_sites.tsv`) gets its verdict applied under the flag, March's code in `#else`.
`UT_END` takes June's value 13 without renumbering a member. A loop that would now reach `UT_ROSE` / `UT_AIN` (no templet yet) skips a type with
no templet and logs it once. The `UC_END` guards accept June's class ids as June's code does. Gate: probe compiles of every touched file with the
flag on and off; an existing character's select / village / dungeon pass shows no new log line.

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

NU0 (2026-11-02), `june2017_re/char_luciel/nu0_unit_type_sites.tsv` (55 rows: widen 15, june_body 2, leave 38; 15 of the leave rows are dead
branches by `cl /P`):
- **Script table first.** `apszScript[]` (`X2UnitManager.cpp:405-423`) has 10 entries and its `ASSERT` (:425) compiles to `((void)0)`: widening the
  loop at :427 alone reads past the table. Grow it to June's 13 (table `337341b8`, loop end `cmp esi, 0x337341ec`): index 10 `LUCIEL_DUO.LUA`,
  11 / 12 NULL (Rose / Ain not ported; log once). Getter :667 is June's `3237a680` (`cmp eax, 0xc; jbe`, else the static default).
- **Arrays** `X2UnitManager.h:3442` (June 13 x 8 B, `32385a07`): widen. `X2UnitManager.h:3444`, `.cpp:679`, `X2Item.h:208-209,347` sit in dead
  branches (`X2OPTIMIZE_*` on); the live item path already has K2's key remap: leave.
- **Loops by 12 pictures** (`X2StateDungeonRoom.cpp:2908`, `X2StatePVPResult.cpp:2258`, `X2StatePVPRoom.cpp:3814`: June `cmp ..., 0xc`): widen; the
  dialogs must hold the pictures (check, else the loop logs and skips a missing one).
- **`UC_END` guards:** June accepts classes 1..202 (`X2SocketItem.cpp:2897` = `3271a810`: `dec eax; cmp eax, 0xc9; ja`; item parse
  `X2ItemManager_Preprocessing.cpp:688` = `325bd515` `cmp eax, 0xca; jbe`, type `325bd4d8` `cmp ebx, 0xc; jbe`). June's `Enum.lua` has no `UC_END`:
  bound the test by `UC_AIN_ANGEL` (inclusive), never by a number, and keep `UC_END` itself unrenumbered. `InvalideDataCheck`
  (`X2ItemStatCalculator.cpp:195,203`, `328d6ad4` types `< 0xd`, `328d6b30` classes 100 .. `< 0x89`) only logs `XEM_ERROR148`: widen both.
- **Leave:** `X2OfflineStatTable.cpp:283` (class -> type identity for base classes 1-9: classes 10-12 are Elsword / Aisha / Lena jobs; NU4
  fixes the fallback). `X2BuffDisplayerTemplet.cpp:1257` (June's 12 keys already live, the bound is in the dead `#else`). `X2GameEdit.cpp:1643`
  (June has no class cheat dialog). `X2StateServerSelect.cpp:3393` (inside a block comment).
- **Owned elsewhere:** `PlaySoundCreateUnit` (:3985, June `32550aea` types 1..12 + voice-prefix getter `32376270`) is NU4's; `PlaySoundRevive`
  (`X2GUUser.cpp:49580`, `31fdb3f2`) reads `+0x8e90`, so its body is DU1's. NU1 owns `ChangeUnitButtonInfo` (`X2StateServerSelect.cpp:11412`): June
  tests the type 1..12, and until then a type-10 unit pops STR_ID 707 (`nu0_type_switches.tsv`).

## Amended by CH0 (2026-11-02)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §3 L155-156 New characters need a unit type, a unit class, a skill tree and server-side data; §7 L225 UT_LUCIEL and ST_SPECIAL_ACTIVE_GRADE_1-3 Enum members; §7s L583 BOSS_DROP_ITEM.LUCIEL and ENDING_SPEECH<n>.LUCIEL (101 tables), speech entries {; §7ar L988-992 GetX2UnitClassCompatibility 31ed6500: June's 31 extra switch cases (unit classes.
