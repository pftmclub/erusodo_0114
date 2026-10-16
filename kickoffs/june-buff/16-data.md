# June buff port - Phase 16 data step - adopt June's OptionItemData.lua (and the Enum it needs)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-data` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16 review |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` for any C++ enum / loader change (one new flag only if a change is logically separate) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | `16-data-review.md` (write it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Steps 2-4 Done, part 1" "Phase 16 -- Review"
```

## Task

**User decision (2026-10-06): "all June's flow and behaviour".** Adopt June's
`june_src_fixed/data036_kom/OptionItemData.src.lua` **wholesale**, as is: no merge of March's 247
IDs, which June dropped (none is in the save). This also confirms June's option-data template:
March's PERCENT-only critical / additional-defence buffs now give 0 without a socket bonus.

Part 1's load test of June's file against March's Enum (stub manager under the bundled LuaJIT)
found these names unresolved; each nil silently becomes the loader's default:
- `SOCKET_DATA_TYPE`: 80 values (`SDT_WEAPON_*`/`SDT_DEFENCE_*` absorb, amplification, counter,
  destruction, gale, growth, hunting, origin, resist, shine-*, transcend, heavyblow, mana,
  resonant, `*_PVP`, and `SDT_FIX_SKILL_DAMAGE_*`);
- `SOCKET_ITEM_EFFECT_EX`: SIEE 12-20 (`SIEE_PLAY_EFFECTSET`, `_WHEN_SUCCESS_ATTACK`,
  `_TO_VICTIM_WHEN_SUCCESS_ATTACK`, `_WHEN_DIE`, `SIEE_ALWAYS_PLAY_EFFECT_SET` (15),
  `SIEE_BUFF_WHEN_SUCCESS_ATTACK`, `SIEE_BUFF_PERIODICALLY`, `SIEE_DOWN`, `SIEE_RETURN_FROM_DEATH`);
- `HYPER_MODE_EFFECTIVE_TYPE.HMET_FEVER`;
- `EXTRA_DAMAGE_TYPE`: 18 values (`EDT_FORWARD300_*GROUND`, `EDT_COMMAND_*`,
  `EDT_DECREASE_ENEMY_RESIST`, `EDT_METEOR_STRIKE`, ...);
- 20 `STR_ID_*` values the client's `StringID_def*.h` lacks (1,689 uses).

Steps:
1. Take every value from June's `Enum.src.lua` (`june_src_fixed/data036_kom/`), never invent
   one. Add the missing Lua values to `march2014/data036/Enum.src.lua`, compile it, and stage it
   (Enum pairing hazard in "Starting context": it ships with this phase's exe).
2. C++: the matching enumerators where March's code reads the value (`SOCKET_DATA_TYPE`,
   `SOCKET_ITEM_EFFECT_EX`, `HYPER_MODE_EFFECTIVE_TYPE`, `CX2DamageManager::EXTRA_DAMAGE_TYPE`).
   For each value, say what March does with it: the UI and socket-category switches, the loader's
   extra-damage switch (an unknown EDT is dropped), `InitSocketEffectEx`. Where June has a handler
   March lacks (SIEE 12-20 apart from the aura, the new EDTs, `HMET_FEVER`), log one
   `[JUNE_BUFF]` line per type and list it. Do not port the handler here: SIEE 15 belongs to
   16-step3b, and EDT / HMET handlers need their own scope.
3. The 20 `STR_ID`s: find how June's descriptions reach the UI (`m_eDescStringID`, the string
   table). If the strings live in a data file the user packs, stage it; if they live in the
   client's string table, name the file and stop.
4. Compile June's OptionItemData (`elstools compile-batch -v mar-2014`), check the bytecode
   equals the source (the `bceq.lua` approach from part 1), stage it in both directories with
   the manifest, and re-run the load test against the new Enum: 0 unresolved names.
5. Re-count the reach on the data the game will now load: entries per June-only key, the save's
   13 socket IDs old vs new, and any June-only socket the save's items now carry.

Play-test: a dungeon run with sockets equipped, plus whatever step 2 makes visible. Log lines:
the loader's error lines, rank-linked skips (`is rank-linked`, now reachable: 185 June entries),
and step 2's per-type lines.

## From the 16 review (2026-10-06)

- **Decided (user, 2026-10-06): accept the window, run 16-data now**; 16-step3b does items 7-8 first.
  Adopting June's file makes 185
  rank-linked sockets live (120 rank, 65 PVE), whose stats stay out until 16-step3b item 8
  decodes the scaling (their enchant and skill level-up do apply since the review's fix 1), and
  about 280 `+0x29c` sockets whose buff factor is withheld (June sets the HAMEL byte for them) until
  16-step3b item 7. None of the save's 13 socket IDs is in either set; say in the Done note whether
  the save's items now carry any of them.
- The play-test log line for those sockets is now `socket %d is rank-linked (...): June's rank
  scaling is not ported, its stats are not applied (enchant and skill level-up are)`.
- SIEE 13 (`SIEE_BUFF_WHEN_SUCCESS_ATTACK`) and 17 (`SIEE_BUFF_PERIODICALLY`): June's loader routes
  their `BUFF_FACTOR_ID` to the custom-use pointer (`+0x278`) like SIEE 11 and `HMET_USE_BUFF`
  (`32726850 @3273661d..32736630`); March's HAMEL block knows only those two. When step 2 adds the
  enumerators, port that routing in the loader under `SOCKET_PARITY` (funcmap row `32726850 @32736ee9`).
- `DUNGEON_ONLY_TYPE` appears on 8 June sockets (300000-300007, `DT_SECRET`); the gate already
  handles it.
