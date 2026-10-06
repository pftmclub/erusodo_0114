# June buff port - Phase 16 step 3b - the socket readers, the cool-time map, the aura socket side

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16 review and 16-data (June's OptionItemData + the Enum values, incl. SIEE 12-20, land there) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-read lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | `16-review.md` covers part 1; write `16-step3b-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Step 1 RE" "Phase 16 -- Steps 2-4 Done, part 1"
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 1c, item 4" "Phase 8 -- Done" "Phase 16 -- Review"
```

## Task

**Order (user decision 2026-10-06, 16 review): do items 7 and 8 first**, in that order, each
in its own commit; then 1-6, 9 and 10. 16-data adopted June's OptionItemData with the window open:
until 7-8 land, about 280 `+0x29c` sockets get no buff and 185 rank-linked sockets give no stats.

Part 1 seeded every June-only option member and gave `CX2GameUnit` getters for each
(`GetIncreaseDmgRateToBoss()`, `GetSocketAntiCriticalRate()`, ...). Nothing reads most of them
yet. Port the readers from step 1's "Readers of each June-only field" table, one commit per
group, each finishing its open decode item first (never assume a formula; a reader that cannot
be settled from the disassembly stays dormant with one `[JUNE_BUFF]` line and is listed as open):

1. `GetFinalDamage` terms: boss damage `@32286519` (the `bl` alternative too), low-HP enemy
   `@32285344`, low-HP self `@322853e5`, low-HP defence `@3228692c`, the piercing rates
   `@32285850`/`@32285952` (+ twins `@32285a54`/`@32285b3a`; **decode the formula past the add**),
   and the attacked-damage term `@3228538e`. Place each where June has it relative to March's
   existing multipliers.
2. User `DamageReact`: boss defence `@3204884f`, `52fc` `@3204885e` (**settle both roles** with
   the `GetFinalDamage` read), critical afflicted `@32048b97`..`@32048cb5` (**identify
   `32071ab0`**, `32070e00`, `32070de0`), anti-critical `@32048aa2` and NPC `327b1f30` (**settle the
   sign**).
3. `32275ba0`, June's rewrite of `GetAdditionalAttackPowerRateByType` (port the whole function;
   it reads socket `+0x238` and `+0x2ec`), and the summon helper `322ece70 @322ed2bd`.
4. MP cost `3203c9a0 @3203d02d` (**name its March host first**).
5. HP recovery `322fbc60` (**identify it**; callers in step 1's table).
6. Cool-time cuts: the `CX2Unit+0xe78` map (`32355d70`, filled in `SumSocketOption`), the
   factor `3234fbd0` and the apply point `3235a710` (16 call sites). **Re-check `32358770`** (its
   funcmap row does not fit the `( skill, &coolTime )` call). SA grades 14/15/16 do not exist in
   March: port the map as June has it and report the 2nd/3rd-SA cuts as dormant.
7. The `+0x29c` conditional-buff list: NPC-kill hook `322d6f20` (from `CX2Game` `3230b7b0`) and the
   HP-rate reader in `CX2GUUser::CommonStateStart @32060c3e` (**decode**).
8. Rank-linked scaling in `SumSocketOption` (`31edfe70` / `31edfeb0`; since the 16 review only the
   stat part of such a socket is left out, with a log line): decode, then replace that skip.
9. The loader keys part 1 left out (listed in its Done note): find a reader for each or record
   "no reader".
10. **BFT_AURA's socket side** (owned by Phase 16 since Phase 8): SIEE 12-20 (the enum values land in
    16-data; here, their handlers),
    `ProcessSocketEffectEx` case 15 (`32029fff`, called from user slot 484 with 0xf), the
    socket-data `EFFECT_NAME` / interval / `BUFF_FACTOR_ID` list (part 1 loads the list as
    `m_vecConditionalBuff`, `m_bAlwaysPlayEffectSet`), and `CX2GUUser::HasSocketAuraFor`'s inner
    loop (slot 181, `3204e380`).

Count the reach of each reader on the socket data the game loads after 16-data (June's
`OptionItemData.lua`; part 1 found no March socket that sets any June-only key) before building,
and say it per reader. Port June's whole function wherever it differs; cite
June addresses; byte-level CP949 edits.

## From the 16 review (2026-10-06)

- **Item 2, anti-critical:** `32273480` is not a getter. It returns
  `(prot +0x2b28 > 0 ? prot +0x2bc4 : 0) - result(+0x4e10) - (opt(+0x5a14) > 0 ? opt x 0.01 : 0)`,
  so the anti-crit option is *subtracted*, scaled by 0.01 (funcmap row corrected). Name
  `+0x2b28`/`+0x2bc4`/`+0x4e10` and port the whole function; then fix the comment on
  `CX2GameUnit::GetSocketAntiCriticalRate` (`X2GameUnit.h`), which still calls `32273480` the getter.
- **Item 5 / item 1 (boss damage):** for NPCs, June seeds HP recovery, item drop and boss damage with
  option 1.0 on top of basic 1.0 (JUNE_BUGS #46, ported by the review), so NPC results are 2.0. Keep
  that verbatim in the readers.
- **Item 8, rank-linked:** the review narrowed part 1's skip to `ConvertAndAdd` only (all three
  branches rejoin at `3204f485`). Each branch, the plain one included, also calls `32077f50`
  (= `std::vector<float>::resize(n, 0.0f)`) on user `+0x87b8` with n = the aggregate's `+0x114`
  vector count (`m_vecMpUpByAttacked`, 0x18-byte elements in June). That per-entry vector is
  June-only and not ported: decode its reader with the `+0x114` element layout.
- **Item 10, SIEE handlers:** June's loader routes SIEE 11/13/17 buff factors to the custom-use
  pointer (see `16-data.md`); 16-data ports the routing, the handlers are here.

## From 16-data (2026-10-06)

- June's `OptionItemData.lua` and the Enum are in (Lua and C++, `_SOCKET_PARITY`). SIEE 12-20 are now
  **filed** like June (`31fe4730` accepts 1-20; the caller skips the socket's tail), and SIEE 13/17
  route to the custom-use pointer. `ProcessSocketEffectEx` has no case for 12-20 yet: item 10 adds
  them. The loader logs one `[JUNE_BUFF]` line per June-only SIEE / EDT / HMET type.
- Reach on the loaded data: SIEE 15 on 180 entries, 13 on 27, 14 on 17, the rest ≤ 9; 185 rank-linked
  (120 + 65); `m_sBuffLevel` (`+0x29c`) on 280. None of the save's 13 socket IDs is in any of these.
- Not in this phase: the 18 June-only EDTs (179 entries) and `HMET_FEVER` (1 entry) belong to
  **16-step3c** (user, 2026-10-06).
