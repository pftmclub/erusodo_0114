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
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 1c, item 4" "Phase 8 -- Done"
```

## Task

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
8. Rank-linked scaling in `SumSocketOption` (`31edfe70` / `31edfeb0`; part 1 skips such sockets
   with a log line): decode, then replace the skip.
9. The loader keys part 1 left out (listed in its Done note): find a reader for each or record
   "no reader".
10. **BFT_AURA's socket side** (owned by Phase 16 since Phase 8): SIEE 12-20 (the enum values land in
    16-data; here, their handlers),
    `ProcessSocketEffectEx` case 15 (`32029fff`, called from user slot 484 with 0xf), the
    socket-data `EFFECT_NAME` / interval / `BUFF_FACTOR_ID` list (part 1 loads the list as
    `m_vecConditionalBuff`, `m_bAlwaysPlayEffectSet`), and `CX2GUUser::HasSocketAuraFor`'s inner
    loop (slot 181, `3204e380`).

Count the reach of each reader on March's live socket data before building (part 1 found no March
socket that sets any June-only key, so most readers are dormant until the user adopts June's
`OptionItemData.lua`; say so per reader). Port June's whole function wherever it differs; cite
June addresses; byte-level CP949 edits.
