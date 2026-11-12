# June damage and character stat calculation - phased plan (`june-stat-calc`)

Backlog Stage 33 (`JUNE_PORT_BACKLOG_ORDER.md`, "Stage 33: June damage and character stat calculation"). Planned 2026-11-12 from the brief
`kickoffs/june-backlog/stage33-plan.md`. Plan key `june-stat-calc`, branch `main`, kickoffs `kickoffs/june-stat-calc/`.

## Context

- **The rule behind the plan (user, 2026-10-27).** Stat and damage math is June's whole calculation, never a March / June merge. A June
  seed, curve or data table read by a March formula is a bug, not a port.
- **The trigger for running it now (user, 2026-11-12).** Monsters in some secret dungeons take 0 or almost no damage. The planning session
  found the cause (see "Measurement"): June's monster defence data is already live (`NewNPCStat.lua`, ExtraNPCStat parity), but
  `CX2GameUnit::GetFinalDamage` still turns defence into a reduction with March's straight-line formula. June's defence numbers are built
  for June's curve; in March's formula they pass 100% and the hit is multiplied by 0.
- **Two tracks.**
  - **Damage track (D0-D6): runs now.** It is a playability fault on a reachable path, so the scope rule lets it run ahead of the
    character ports. D1 is the fix for the 0-damage monsters and depends on nothing.
  - **Stat-sheet and resource track (S0-S4, R1): code after the character ports** (Stages 7 and 8; user, 2026-10-27), so every
    character's numbers are tested on June's formulas at once. Its map phase S0 is read-only and can run any time.
- **`GetFinalDamage` is the model of the problem.** March's body (`X2GameUnit.cpp:2705-3999`) carries 16 `SERV_IRUHADEV_` flags from
  several plans (buff, socket parity, enchant parity, effect C2-C10, skill memo, character hooks). Each added one June block. No phase ever
  compared the whole function with June's. NPC `DamageReact` carries 23.

## Rules

1. **June's whole body.** A function this plan touches ends with June's whole body inside March's own function under the phase flag,
   March's body in `#else`. Older flags that already hold a June block inside it stay as they are; the new flag wraps only what was still
   March. A block the map marks `June` is re-read against the exe, not re-ported.
2. **The map decides, not memory.** A phase ports only rows the map marks `March`, `mixed` or `absent`. A row is marked `June` only after
   a read of the exe in this plan (the older Done note is a lead, not proof).
3. **Every reader of a number moves together.** When a phase changes how a value is produced (a curve, a seed, a unit), it lists every
   reader and refresher of that value (per hit, per event, UI, emulator) and ports them in the same phase, or proves they already agree.
4. **No tuning.** No clamp, cap, scale or "safety" value that June does not have. If June's result looks wrong (a negative defence, a
   reduction above 1), port what June does with it and add a `JUNE_BUGS.md` row. A result June never showed is a port gap: find it.
5. **Class-specific material goes to the character ports.** The per-class `SetDamageExceptionProcess` overrides, class blocks inside a
   shared function and class passives are identified here and handed to the class's plan or `JUNE2017_CHARACTER_PORT_CHECKLIST.md`.
   Inside a shared June body, an unported class's block is guarded with a one-line `stand-in: <class> not ported` comment.
6. **Numbers are proven twice.** Each formula phase runs June's own code for the same inputs (Unicorn on `unpacked_x2.exe`, or the x87
   trace of `disasm_range.py --x87` when a block cannot be emulated) against the port's expression, and reads the per-hit log from a
   play-test. The comparison script and its input table are committed under `june2017_re/statcalc/`.
7. **One debug flag for the plan:** `SERV_IRUHADEV_JUNE_STATCALC_DEBUG`. It stays on through the damage track and is retired by the D6
   review; the stat track turns it on again in S1 and Z retires it.
8. Standing rules apply unchanged: one branch `main`, explicit pathspecs, CP949 sources through `bytepatch.py`, no hardcoded enum
   literals, no `June` in names, one-line comments, `junediverge.py` gate, June bugs kept verbatim with a `JUNE_BUGS.md` row.

## Starting context

- **March code.** `X2Lib/X2GameUnit.cpp`: `CalcLevelFactorForFinalDamage` (:1938), `GetFinalDamage` (:2705-3999; defence sum :3172-3317,
  the full-defence constants :3323-3357, the rate :3403-3418, the level-damage path :3421-3531, the damage rate :3535),
  `SetDamageExceptionProcess` (:19185), `GetAdditionalAttackPowerRateByType` (:19912). `X2Lib/X2GUNPC.cpp`: `DamageReact` (:9728-11690),
  `LevelCorrectionExtraDamage` (:11959). `X2Lib/X2GUUser.cpp`: `DamageReact` (:9211), `LevelCorrectionExtraDamage` (:12324),
  `SetDamageExceptionProcess` (~:45863). `X2Lib/X2UnitManager.cpp:1283-1313`: the extra-stat game type (1 Henir, 2 secret, 0 other) passed
  to `CalculateExtraStatValue`. `X2Lib/X2UICharInfo.cpp:1845, :1969`: the character window's June defence curve (CharInfo C3).
  Line numbers drift: find by name.
- **June code.** `GetFinalDamage` `32284aa0` (decompile and listing: `june2017_re/buffre/dec/32284aa0.c` / `.s`), NPC `DamageReact`
  `327b1f30`, user `DamageReact` `320462a0`, `SetDamageExceptionProcess` `322990e0` (user `32045340`, NPC `32759f30`), the damage-result
  hook `3229a530`, the variance roll `322788c0`, `LevelCorrectionExtraDamage` `32760210`. Existing rows:
  `grep -n -E "32284aa0|327b1f30|320462a0|322990e0|StatCalc S33" june2017_re/funcmap.tsv`.
- **June data already live.** `data036` `NewNPCStat.lua` (1,693 rows) and `NewNPCStatFormula.lua` (flags `SERV_IRUHADEV_EXTRA_NPC_STAT`,
  `_PARITY`, both on), June `item.kim` (K2), June `SocketOptionForm.lua`, June skill and buff data. Source form:
  `june_src_fixed/data036_kom/*.src.lua`.
- **Tools.** `python june2017_re/disasm_range.py <va> <va> [--x87]`, `python june2017_re/funcdis.py --func <va>`, Ghidra (skill `ghidra`,
  read-only), Unicorn 2.1.4 is installed, `tools/junediverge.py`, `tools/flagcheck.py`, `tools/bytepatch.py`, `tools/build.py --deploy`,
  `tools/logs.py --tag JUNE_STATCALC`, `tools/exestr.py`, `tools/elsdb.py`, `tools/createchar.py`. Never hand-type msbuild; never read
  this doc in full, pull sections with `docsection.py`.
- **New in this plan.** `june2017_re/statcalc/statcalc_map.tsv` (the map; seeded 2026-11-12), `june2017_re/statcalc/gfd_blocks.tsv` (D0:
  `GetFinalDamage` block by block), `june2017_re/statcalc/emu_*.py` (the comparison scripts), funcmap rows tagged `StatCalc S33`.
- **Flag family.** `SERV_IRUHADEV_JUNE_STATCALC_*`: `_DEFENCE_CURVE` (D1), `_FINAL_DAMAGE` (D2), `_HIT_PIPELINE` (D3), `_DAMAGE_REACT`
  (D4), `_EXTRA_DAMAGE` (D5), `_UNIT_STAT` (S1), `_EQUIP_STAT` (S2), `_SOCKET` (S3), `_BUFF_FACTOR` (S4), `_RESOURCE` (R1), `_DEBUG`.
  **Log tag** `[JUNE_STATCALC]` in `offline_server.log` (`CX2OfflineLog::Server`, throttled to about one line a second per pair).

## Measurement (planning session, 2026-11-12)

Read from the tree, the June exe and June's data. Nothing was run in game.

**1. March's defence formula** (`GetFinalDamage`, dungeon and field):

```
full    = 348 + 62 * attacker level                       (:3323-3357)
factor  = 1 + 0.127 * (defender level - attacker level)   when the defender is higher,
          1 - 0.065 * (attacker level - defender level)   when lower, floor 0   (CalcLevelFactorForFinalDamage, dungeon only)
rate    = defence * factor / full                          (:3409)
damage *= max( 1 - rate * frozen * skill - shock, 0 )      (:3535)
```

Straight line: at `defence >= full` the hit does nothing.

**2. June's defence formula** (`32284aa0` @32285d58-32285dc4 and @32286068-32286090):

```
level   = vslot 0x20c of the unit stored @32284bcf ([esp+0x38]); forced to 1 when g_pX2Game +0x85d is set
rate    = defence / ( level * 39.35 + 258.65 + defence )   (doubles 33487178, 33487170)
damage *= max( 1 - rate * [esp+0x3c] * [esp+0x18], 0 )
```

A curve: it never reaches 1 for a positive defence. No game-type switch inside the block; no shock term in that statement.
The character window already prints this curve (`CX2UICharInfo::GetDamageDecrement`, June `3291eb50` / `322a8b60`), so the window and
combat disagree today.

**3. June's monster defence is built for that curve.** `NewNPCStatFormula.lua`: `DefFactor(level) = 258.65 + 39.35 * level` (the same
two constants), and defence = `k * DF * base / ( DF - (k - 1) * base ) * 1.2 for secret and Henir`, where `k` is the row's
`PHYSIC_DEF_VALUE` / `MAGIC_DEF_VALUE` and `base` is `NormalDef`, `HenirDef`, `SecretNormDef` or `SecretLutoDef` by game type. Through
June's curve that is a reduction of `k * base / ( DF + base )` before the 1.2: `k` scales the reduction, and the defence number itself
grows without limit as `k` rises. March's formula reads that number as if it were linear.

**4. The numbers** (attacker and monster at the same level, so March's level factor is 1; a higher monster makes March worse):

| Curve (game type) | Level | k | Defence | June: damage x | March: damage x |
|---|---|---|---|---|---|
| normal (0) | 70 | 1.0 | 946 | 0.76 | 0.80 |
| normal (0) | 99 | 2.0 | 3,827 | 0.52 | 0.41 |
| secret (2) | 70 | 1.0 | 3,694 | 0.45 | 0.21 |
| secret (2) | 70 | 1.5 | 11,329 | 0.21 | **0.00** |
| secret (2) | 99 | 1.0 | 5,128 | 0.45 | 0.21 |
| secret (2) | 99 | 1.5 | 15,836 | 0.21 | **0.00** |
| secret, type 3 | 70 | 1.0 | 5,702 | 0.35 | **0.00** |

Secret dungeons are the only place the port passes game type 2 (`X2UnitManager.cpp:1288`, `IsSecretDungeon`; type 3 is never passed
today, its row shows what a later fix of that question would meet): a `k = 1.0` monster
takes about a fifth, anything from about `k = 1.2` takes nothing. That is the reported fault. Normal dungeons differ by a few percent.
The 3,386 defence values in `NewNPCStat.lua`: 1.0 x 2,062; 1.1-1.3 x 850; 0.5-0.91 x 274; 1.4-1.6 x 20; 2.0-3.0 x 180.

**5. Left for the phases (not read in this session).**
- Whether June still has a level-gap factor on defence. `CalcLevelFactorForFinalDamage` has no June row; the call @322855b6
  (`3226e530`) is the extra-damage sum, not it. D1 settles it before writing.
- Which unit's level `[esp+0x38]` is (March's counterpart is the attacker's; the damage-free override to 1 matches March's attacker
  branch).
- Where June's shock (-0.1) term went, and the PvP rate.
- `k` of about 1.98 and above on the secret curve makes the Lua's denominator cross 0, so the defence value is negative. Which game
  types the 180 values of 2.0-3.0 meet, and what June's code does with a negative defence, is D1's to read (the port's ExtraNPCStat
  evaluator already falls back to the legacy stat on a non-finite result; a negative one is finite).
- The player as the defender runs the same block: incoming damage changes too. D1's play-test checks both directions.

## Design: the map and the verification

- **Map** `june2017_re/statcalc/statcalc_map.tsv`, one row per formula function, per block of a large function and per stat member.
  Columns: `area` (damage / stat / resource), `item`, `june_va`, `march_site` (`file:function`), `state` (`June` / `March` / `mixed` /
  `absent` / `unknown`), `last_flag`, `readers`, `phase`, `note`. The planning session seeded it with the rows the brief, the divergence
  plan (`tools/data/june_march_divergent.tsv`, verdict `stage33`) and the measurement name. D0 fills the damage rows, S0 the stat and
  resource rows. A phase that ports a row sets it to `June` in its own commit.
- **Block ledger** `june2017_re/statcalc/gfd_blocks.tsv` (D0): `GetFinalDamage` `32284aa0` cut into address ranges in June's order, each
  with what it computes, March's lines, the state and the flag. D2 works down this list; its review checks that no range is left out.
- **Per-hit log** (D1, `_DEBUG`): one line per attacker-defender pair, about once a second:
  `[JUNE_STATCALC] HIT att=<user uid or npc id> lv=<n> def=<npc id or user uid> dt=<damage type> atk=<n> defence=<n> rate=<f> march=<f> dmg=<n>`.
  `march=` is March's rate computed for the log only, so one play-test shows before and after. D2 adds `GFD` lines (each multiplier of
  the main path by name), D4 `REACT` lines (final damage, each reduction, HP change), S1 a `SHEET` block on game start.
- **June's code as the oracle.** `june2017_re/statcalc/emu_defence.py` (D1) maps `unpacked_x2.exe`, runs the straight-line ranges
  @32285d8d-32285dc4 and @32286068-32286090 with the stack slots preloaded, and compares with a Python transcription of the port's
  expression over at least 300 input sets (defence 0, negative, 1e6; level 1, 70, 99; the two multipliers at 0, 0.5, 1). Later phases add
  one script per pure function (precedent: buff Phase 16 review, `320839e0` on 360 sets).

## Phase D0 - Damage map (read-only)

**Scope.** Fill the damage rows of `statcalc_map.tsv` and write `gfd_blocks.tsv`. Read every function in June and its March counterpart:
- `GetFinalDamage` `32284aa0`, range by range, including the level-damage path @32285dc4-32286068 and the tail after @32286090 (the
  damage-type switch `32286c78`, the unit-type-100 block @322860ed, the game-type rates @32286126-3228618a, vslot `0x74`, the multiplier
  sum @3228619f-3228626e, the `AT_COMMAND` terms, boss damage, the enchant piercing term, `32275ba0`, HP-few, HP-rate, `3227c940`);
- its helpers: the variance roll `322788c0` (not ported: say whether June's data now gives it a non-zero range), `3226e530`, `3226e9a0`,
  `322a91a0`, `32273480`, `32274970`, `GetEquippingItemByEquipPos` `32869e10` (3 args in June), `ModifyFinalDamageByGameUnitType`, the
  hook `3229a530` (vslot `0x6c`, multiplies both attacks; the Rena block @3229a79c is char-rena's, the 24222 / 24057 block is Eve's);
- `SetDamageExceptionProcess` `322990e0`, user `32045340`, NPC `32759f30` (March has none), and the list of the eight class overrides
  (funcmap rows 197-204) with the plan that owns each;
- the damage section of NPC `DamageReact` `327b1f30` (from the `GetFinalDamage` call to `UpNowHp`) and of user `DamageReact`
  `320462a0`; `GetBasicReducedDamagePercent` `3201aff0`, `GetSocketReducedDamagePercentInCase` `32000320` (its June body sits in a
  June-named twin today), `322cfca0`, the training clamp;
- `LevelCorrectionExtraDamage` `32760210` and the user's twin, `GetEnchantResist` (`32274b00`, user `31fd8790`, NPC `32757c50`),
  `DamageCheck` `3262f5a0`'s prelude, and the hit dispatch gaps routed from char-raven CH1q (`CEffect::AttackResult` `31f85910` tail
  @31f885bd-31f88669; `UnitToUnit` `32625000` @3262561e).

Then size D2-D5 from the rows: split a phase that holds more than about a dozen `March` / `mixed` blocks, amend its kickoff and STATUS
row, and hand class rows to their plans. No client code.

**Run with.** Opus 5.5 / xhigh; subagents do the greps and the mechanical Ghidra pulls. Review Opus 5.5 / high (`D0-review.md`).

**Read list.** "Rules", "Starting context", "Measurement (planning session, 2026-11-12)", "Design: the map and the verification".

**Done gate.** Every address range of `32284aa0` is in `gfd_blocks.tsv` (ranges contiguous from entry to the last `ret`, checked by a
script line in the Done note); every item in the scope list has a map row with a state and a reader list; funcmap rows tagged
`StatCalc S33` for each new identification; D2-D5 kickoffs amended. Full build skipped: no play-test.

## Phase D1 - Defence curve: the 0-damage fix

**Scope.** In `CX2GameUnit::GetFinalDamage`, under `SERV_IRUHADEV_JUNE_STATCALC_DEFENCE_CURVE`, replace March's defence-rate chain with
June's, whole, from the defence sum to the damage rate (@32285849-32286090), March's in `#else`:
- settle the open reads of "Measurement" item 5 first, in the exe: the level-gap factor (find June's twin of
  `CalcLevelFactorForFinalDamage` or prove there is none; if it differs or is gone, add its `june_march_divergent.tsv` row and the
  replacement in this commit), whose level is used, the shock term, the PvP rate, a negative defence;
- the rate `defence / ( level * 39.35 + 258.65 + defence )` and the damage rate `max( 1 - rate * a * b, 0 )`, with the constants as June
  holds them (doubles) and June's operand order;
- every other reader of the March constants 348 / 62 / 410 (`grep` `X2Lib`, `KTDXLIB`, `X2Lib/Offline`): list each, port or prove equal;
- the level-damage path's use of the rate, if it reads it (@32285dc4 branches on `m_bLvDamage` after the rate is stored);
- `SERV_IRUHADEV_JUNE_STATCALC_DEBUG` and the `HIT` line of "Design";
- `june2017_re/statcalc/emu_defence.py` and its result in the Done note;
- map rows set to `June`; a `MODS.md` entry.

Not in D1: any other block of `GetFinalDamage` (D2), monster stat data, the ExtraNPCStat game type 3 question (owner:
`LANOX_EXTRA_NPC_STAT_PLAN.md` follow-ups).

**Run with.** Opus 5.5 / high. Review Opus 5.5 / high (`D1-review.md`). Not beside another phase that edits `X2GameUnit.cpp`.

**Read list.** "Rules", "Starting context", "Measurement (planning session, 2026-11-12)", "Design: the map and the verification",
"Play-test hand-off".

**Done gate.** `flagcheck` PASS for both flags; `junediverge --check` PASS; the emulation agrees on every input set (or each difference
is explained as June's own behaviour with a `JUNE_BUGS.md` row); `DEPLOY OK`; `EXESTR OK` for `[JUNE_STATCALC] HIT`; play-test: in one
secret dungeon per curve the user hits a monster that took 0 before, and the log's `rate=` is below 1 with `march=` at or above 1 for
that monster; the monster's hits on the player also log.

## Phase D2 - GetFinalDamage: the rest of June's body

**Scope.** Every `gfd_blocks.tsv` range still `March`, `mixed` or `absent`, under `SERV_IRUHADEV_JUNE_STATCALC_FINAL_DAMAGE`, so the
function is June's from entry to return: block order as June runs it, the variance roll and its seed source if D0 found the range can be
non-zero, the `3229a530` call (base body; class blocks guarded), the level-damage path, the game-type and PvP / PvE rates, the
multiplier sum, `GetEquippingItemByEquipPos`'s third argument, and June-only early returns. Blocks already `June` are re-read and left.
`GFD` debug lines. One emulation script per pure helper ported.

**Run with.** Opus 5.5 / xhigh. Review Opus 5.5 / high. D0 may split it into D2a / D2b by address range.

**Read list.** "Rules", "Starting context", "Design: the map and the verification", "Phase D0 - Done", "Phase D1 - Done".

**Done gate.** `gfd_blocks.tsv` has no row left that is not `June` or `class: <plan>`; flagcheck and junediverge PASS; `DEPLOY OK`;
play-test on three characters (one physical, one magical, one with a ported transcendence): normal attack, a special active and a
level-damage skill against a normal-dungeon monster, a secret-dungeon monster and a boss, each `GFD` line's product equal to the damage
number on screen.

## Phase D3 - The hit pipeline before the formula

**Scope.** Under `SERV_IRUHADEV_JUNE_STATCALC_HIT_PIPELINE`: `SetDamageExceptionProcess` base `322990e0` (damage terms only; its MP
terms are R1's), the user's `32045340` June-only gate, the NPC override `32759f30` (new in March: master-unit forward), `DamageCheck`'s
prelude, and the two dispatch gaps (`CEffect::AttackResult` tail: the owner's on-hit sockets for a damage effect's hit; `UnitToUnit`:
`AttackResultByType` for every non-user attacker). The eight class overrides are listed with their owners, not ported.

**Run with.** Opus 5.5 / high. Review Opus 5.5 / high.

**Read list.** "Rules", "Starting context", "Phase D0 - Done", "Phase D2 - Done".

**Done gate.** Map rows `June`; gates PASS; `DEPLOY OK`; play-test: a damage effect's hit (an arrow or a fireball) fires the attacker's
on-hit socket effect, and a summoned NPC's hit on a monster logs `HIT` with the summon as attacker.

## Phase D4 - DamageReact: from the final damage to the HP change

**Scope.** Under `SERV_IRUHADEV_JUNE_STATCALC_DAMAGE_REACT`, the damage arithmetic of NPC `DamageReact` `327b1f30` and user
`DamageReact` `320462a0` between the `GetFinalDamage` call and `UpNowHp`: the reduction sums, `GetSocketReducedDamagePercentInCase`'s
June body moved into March's own function (the June-named twin goes), `322cfca0`, the order of the HP-rate overwrite, the training
clamp's June-only term, the damage number shown. Reaction, hit-stop, sound and buff code in the same functions is not this plan's.
`REACT` debug lines. The `DI_EVENT_ALCHEMY_LAB` block is ported only if that dungeon is registered by then; otherwise its row says who
owns it.

**Run with.** Opus 5.5 / high. Review Opus 5.5 / high.

**Read list.** "Rules", "Starting context", "Phase D0 - Done", "Phase D2 - Done".

**Done gate.** Map rows `June`; gates PASS; `DEPLOY OK`; play-test: for ten logged hits each way, `REACT`'s HP change equals the `GFD`
damage after the listed reductions, on a unit with damage-reduction sockets and one without.

## Phase D5 - Extra damage, enchant resist and level correction

**Scope.** Under `SERV_IRUHADEV_JUNE_STATCALC_EXTRA_DAMAGE`: `LevelCorrectionExtraDamage` NPC `32760210` and the user's twin (June's
dungeon list: adds 41280, drops the Henir branch), the extra-damage sum `3226e530`, the per-tick damage of the extra-damage types as
the map lists them, `GetEnchantResist`'s user override `31fd8790` with its six per-type floats and their writer (`31fe19d0`
@31fe1b84-31fe1d0d) and the NPC override `32757c50`. Missing EDT cases (black-out, mark list) stay with backlog Stage 36.

**Run with.** Opus 5.5 / high. Review Opus 5.5 / high.

**Read list.** "Rules", "Starting context", "Phase D0 - Done".

**Done gate.** Map rows `June`; gates PASS; `DEPLOY OK`; play-test: poison, fire and frozen ticks on a monster and on the player match
the log's computed tick; a resist-socketed unit shows the lower tick.

## Phase D6 - Damage smoke on every ported character, track review

**Scope.** No new code. One play-test sheet per ported character (a geared level-99 unit with sockets and one without): normal, command,
special active, hyper active and level-damage hits against a normal monster, a secret monster and a boss, and hits taken. The review
reads every `HIT` / `GFD` / `REACT` line against the map, runs `flagcheck --range` over D1-D5 and `junediverge --check-all`, and
retires `SERV_IRUHADEV_JUNE_STATCALC_DEBUG` (build not awaited).

**Run with.** Sonnet 5.5 / high for the sheets; the review Opus 5.5 / high.

**Read list.** "Rules", "Play-test hand-off", the Done notes of D1-D5.

**Done gate.** Every character's sheet passes or its failure is routed (a class block to its port, a formula row back to a D phase);
no damage row in the map is `March` or `mixed`.

## Phase S0 - Stat-sheet and resource map (read-only)

**Scope.** The stat and resource rows of the map: base stats by level, `SetKStat` `323571c0` (two 7-value sets), `SetGameStat`
`322814a0`, `ResetMaxHP` / `ResetMaxMP` `31fd2810`, June `Stat` [9] / [10], `GetEqipStat` `3234e210`, `CX2Item::GetStat` `328842f0`,
`GetSetIDNPartsNum` `32350cb0`, `GetEnchantStat` `32358df0`'s group argument, the option-data template `320839e0`, `SumSocketOption`
`3204f190`, `ResetIncreaseSkillLevelBySocket` `3235a080`, the `IsSocketOptionFormLoaded()` paths, titles, pets, passives, the buff stat
members (`32270dd0`, `32271540`, `322718e0`, `32290950`), the three `SetFactor` finalizers, `32358970`; MP on hit and on hit taken,
`ResetMPChangeRate` `31fd9e00`, `AbsorbEffectAttackProcess` `32274660`, the awakening gauge, regen, `GetActualMPConsume`, the six
`GetSkillMpDecreaseRate` sites. Size S1-S4 and R1, amend their kickoffs.

**Run with.** Opus 5.5 / xhigh. Review Opus 5.5 / high. Any time (read-only).

**Read list.** "Rules", "Starting context", "Design: the map and the verification"; the backlog's Stage 33 section.

**Done gate.** Every item above has a row with a state and readers; the S and R kickoffs are amended. Full build skipped.

## Phases S1-S4, R1 - Stat sheet and resource gain (after Stages 7 and 8)

Each phase: the map's rows for its area, June's whole bodies under its flag, a `SHEET` or per-event debug line, one emulation script per
pure function, an Opus review, and a play-test on every character (level 99, with and without sockets).

| Phase | Area | Flag | Main files |
|---|---|---|---|
| S1 | unit stat core: `SetKStat`, `SetGameStat`, `ResetMaxHP` / `ResetMaxMP`, `Stat` [9] / [10] | `_UNIT_STAT` | `X2Stat.*`, `X2Unit.*`, `X2GameUnit.*`, `X2GUUser.*` |
| S2 | equipment, set items, enchant stat | `_EQUIP_STAT` | `X2Unit.*`, `X2Item.*`, `X2ItemStatCalculator.*` |
| S3 | sockets and options: `320839e0`, `SumSocketOption`, socket skill levels | `_SOCKET` | `X2SocketItem.*`, `X2GameUnit.*`, `X2GUUser.*`, `X2Unit.*` |
| S4 | buff finalizers `SetFactor` x3, buff stat members re-read | `_BUFF_FACTOR` | `X2BuffFinalizer*`, `X2GameUnit.*` |
| R1 | MP / awakening gain, regen, MP cost, MP change rate, absorb | `_RESOURCE` | `X2GameUnit.*`, `X2GUUser.*`, `X2GageData.*`, class MP blocks |

**Run with.** Opus 5.5 / high each, Opus review each. **Depends.** S0 review, Stage 7 and Stage 8 closed, D6 review. One at a time
(shared files). S0 may split or merge them.

## Phase Z - Close

**Scope.** `MODS.md` complete, the map has no `March` / `mixed` / `unknown` row, `tools/data/june_diverge_allow.tsv` holds no `stage33`
reason, the backlog's Stage 33 marked closed, memory updated, the STATUS block archived.

**Run with.** Sonnet 5.5 / medium. **Done gate.** `junediverge --check-all` PASS; full build skipped unless Z changed code.

## Play-test hand-off

Each code phase hands over, copy-ready (dev-console lines in full):
1. **Unit.** Nickname, class and level of the test unit, made or set with `/x2-create-char` / `/x2-db`; skills slotted by the phase in
   slots 0-3 and 5-8.
2. **Where.** The dungeon id and difficulty, the console line to enter it, the stage that spawns the monster, the monster's NPC id and
   name. A secret-dungeon check names one monster with `k = 1.0` and one with `k >= 1.2` from `NewNPCStat.lua`.
3. **Do.** Which attack, how many hits, and for incoming damage which monster to stand in front of (`/invin 0`).
4. **Look for.** The damage numbers on screen (a range), and that the monster's HP bar moves.
5. **If not.** Which log line is missing or which field is off, and what that means.
6. **Log check.** `python tools/logs.py --tag JUNE_STATCALC`: the lines the phase's gate names, by monster id.

## Open questions for the user

None blocks D0 or D1.
1. **Which dungeon and monsters showed the 0 damage?** D1 tests one secret dungeon per curve either way; naming yours puts them in the
   hand-off first.
2. **Normal dungeons shift by a few percent** once D1 lands (table above: June is slightly harder at `k = 1.0`, easier at `k = 2.0`),
   and so does damage taken. That is June's balance; D1 ports it as is.
3. **`320839e0` (the option-data rewrite)** was left unported by an earlier decision (buff Phase 5c). S3 ports it under this plan's
   rule unless you say that decision stands.
