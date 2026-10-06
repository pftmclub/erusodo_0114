# June buff port - Phase 16 step 3b, part 4 - BFT_AURA's socket side (SIEE 12-20)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b4` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b2 (done); 16-step3b3 done first (both edit X2GUUser.cpp and X2SocketItem.cpp/.h; never in parallel) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b4-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 4" "Phase 8 -- Done" "Phase 16-step3b -- Done, items 7-8" "Phase 16-step3b2 -- Done"
```

## Task

Item 10 of 16-step3b (owned by Phase 16 since Phase 8), split off by 16-step3b2. Port June's whole
function wherever it differs, cite June addresses, byte-level CP949 edits. Count the reach on June's
`OptionItemData.src.lua` (SIEE 15 on 180 entries, 13 on 28, 14 on 18, the rest <= 9; 16-data review).

1. SIEE 12-20 handlers: `CX2GUUser::ProcessSocketEffectEx` (June `32028ba0`; March `X2GUUser.cpp`, which stops at
   `SIEE_ACTIVE_BUFF` 11). The enum values and the filing (`31fe4730`, 1-20) landed in 16-data; SIEE 11/13/17 route
   their buff factor to the custom-use pointer (16-data). Port each June case 12-20.
2. Case 15 `SIEE_ALWAYS_PLAY_EFFECT_SET` (`32029fff`, called from user slot 484 = `InitSocketData`'s frame caller
   with 0xf, `@3207012a`): for each socket ID in `map[15]` whose data has an `EFFECT_NAME` list (`+0x1fc`), the
   interval check (`m_fIntervalTime` `+0x1e8` against unit slot 115 and `31fd8b70( id ) > k`), play each effect set
   (`322ceae0` -> `321e1e60`) and attach every `+0x29c` entry with flag byte 9 to it (`321e3650`), then restart the
   cooldown (`31fe0b90( id, +0x1f8 )`). Confirm `EffectSet_Hero_Dungeon_Aura_Buff` exists in the data March loads;
   if it does not, degrade with one `[JUNE_BUFF]` line.
3. `CX2GUUser::HasSocketAuraFor` (slot 181, `3204e380`): the inner loop over `+0x29c` (entries with byte 9 == 1 whose
   factor's BTI matches). Its `@brief` (`X2GUUser.cpp`, under `_BUFF_NEW_FINALIZER`) still says "nothing is ever filed
   under 15"; since 16-data 180 SIEE 15 sockets are filed. Rewrite it with the loop, and retire Phase 8's
   `unsupported (Phase 16)` line.
4. Reuse, do not redo: the `+0x29c` list (`m_vecConditionalBuff`, with `m_bAlwaysPlayEffectSet`), item 7's reader
   and JUNE_BUGS #48 (with `+0xc` = 0 an SIEE 15 entry also fires on a relationship skill).

Hand-off: the aura needs an SIEE 15 socket on the player's gear; none of the save's 13 sockets has one.
If no console route reaches it, say so and propose how the user gets such an item (do not invent one).
