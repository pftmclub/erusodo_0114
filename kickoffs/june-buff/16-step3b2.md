# June buff port - Phase 16 step 3b, part 2 - the socket readers, the cool-time map, the aura socket side

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b2` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16-step3b items 7-8 (done 2026-10-06) and their review |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-read lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b2-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Step 1 RE" "Phase 16 -- Steps 2-4 Done, part 1"
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 1c, item 4" "Phase 8 -- Done" "Phase 16 -- Review" "Phase 16-step3b -- Done, items 7-8" "Phase 16-step3b -- Review"
```

## Task

The rest of 16-step3b after items 7 and 8 (which landed separately, user order 2026-10-06), one
commit per group. Part 1 seeded every June-only option member and gave `CX2GameUnit` getters for
each. Port the readers from step 1's "Readers of each June-only field" table, each finishing its
open decode item first (never assume a formula; a reader that cannot be settled from the
disassembly stays dormant with one `[JUNE_BUFF]` line and is listed as open):

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
9. The loader keys part 1 left out (listed in its Done note): find a reader for each or record
   "no reader".
10. **BFT_AURA's socket side** (owned by Phase 16 since Phase 8): SIEE 12-20 (the enum values land in
    16-data; here, their handlers),
    `ProcessSocketEffectEx` case 15 (`32029fff`, called from user slot 484 with 0xf), the
    socket-data `EFFECT_NAME` / interval / `BUFF_FACTOR_ID` list (part 1 loads the list as
    `m_vecConditionalBuff`, `m_bAlwaysPlayEffectSet`), and `CX2GUUser::HasSocketAuraFor`'s inner
    loop (slot 181, `3204e380`). Its `@brief` (`X2GUUser.cpp`, under `_BUFF_NEW_FINALIZER`) still says "nothing is
    ever filed under 15": since 16-data, 180 SIEE 15 sockets are filed (16-data review). Rewrite it with the loop.


Already in place from items 7-8, reuse rather than redo:
- `JuneSkillCategory` (June `32070510`, a static in `X2GUUser.cpp`): item 6's slot map needs the
  same mapping (move it somewhere both can reach, under the flag).
- The SIEE 15 entries of the `+0x29c` list carry `m_bAlwaysPlayEffectSet`; item 10's case 15 and
  `HasSocketAuraFor` walk that list. Note JUNE_BUGS #48: with `+0xc` = 0 they also fire on a
  relationship skill through item 7's reader.

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
- **Item 10, SIEE handlers:** June's loader routes SIEE 11/13/17 buff factors to the custom-use
  pointer (see `16-data.md`); 16-data ports the routing, the handlers are here.

## From 16-data (2026-10-06)

- June's `OptionItemData.lua` and the Enum are in (Lua and C++, `_SOCKET_PARITY`). SIEE 12-20 are now
  **filed** like June (`31fe4730` accepts 1-20; the caller skips the socket's tail), and SIEE 13/17
  route to the custom-use pointer. `ProcessSocketEffectEx` has no case for 12-20 yet: item 10 adds
  them. The loader logs one `[JUNE_BUFF]` line per June-only SIEE / EDT / HMET type.
- Reach on the loaded data: SIEE 15 on 180 entries, 13 on 28, 14 on 18, the rest ≤ 9 (248 in all; 16-data review); 185 rank-linked
  (120 + 65); `m_sBuffLevel` (`+0x29c`) on 280. None of the save's 13 socket IDs is in any of these.
- Not in this phase: the 18 June-only EDTs (160 entries; 16-data review) and `HMET_FEVER` (1 entry) belong to
  **16-step3c** (user, 2026-10-06).

## From the 16-step3b review (2026-10-06)

- `SumSocketOption` now computes `eJuneUnitType` (June `ESI`, `@3204f28a`: this user's own unit type) once
  before the rank/plain branch and passes it to all three ConvertAndAdd calls; the plain branch is gated
  on `GetUnit()` like June (`@3204f290`). Item 6 fills the `CX2Unit+0xe78` map in the same function: put
  its code where June has it relative to that branch, and reuse the variable rather than re-reading the type.
- Items 7-8 are confirmed against June; do not reopen them. June's `+0x87b8` resize (`32077f50`) after
  every ConvertAndAdd is still open (it belongs to the 0x18-byte `m_vecMpUpByAttacked` element).
