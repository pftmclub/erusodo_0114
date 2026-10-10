# June effect completion - C9h-4h - the "Heavenly Combat Support Kit" event states (not started)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (decode first), then type `/x2-phase june-effect C9h-4h` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4g (the ids are reserved there: `USI_EVENT_SNIPER_INIT` 88 .. `USI_EVENT_GUN_DANCE_BACK_STEP` 97, flag `SERV_IRUHADEV_JUNE_RESERVED_STATE_IDS`, Enum.lua matches) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_*` (name them when the decode says what the pieces are) |
| Log tag | `JUNE_FX <subtag>` per flag |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4h - port: the event states" "C9h-4g - Done"
grep -n "(C9h-4g)\|(C9h-4f)\|(C9h-4f review)" june2017_re/funcmap.tsv
```

## Task

June has a family of user states entered by picking up four event items; none of it is in March. C9h-4g reserved the ids (no shift needed here) and decoded the
registration and the entry (funcmap rows `3202b110 (C9h-4g)`, `3230bdd0 (C9h-4g)`, `3200d3f0 (C9h-4g)`, and the `(C9h-4f)` / `(C9h-4f review)` handler rows).

1. **Decode the rest before porting.** The handlers listed in row `3202b110 (C9h-4g)` (about 25: init / start / frame / event / end per state), the members they use
   (ctor `3203e850 @3204318f-320431b6`: `+0x947c` target NPC UID, `+0x9480` shot counter (word), `+0x9484` timer, `+0x9488`, `+0x948c`, `+0x9490` the pick-up latch),
   the NPCs they create or drive (`NUI_NEIL_THE_SNIPER_EVENT` = 4342 and its `NEIL_THE_SNIPER_*` states; the Proton Bomb / Meca Drop / Gun Dance ones), and who
   resets the `+0x9490` latch.
2. **The data.** The state tables live in June `datai015 CommonSkillState.lua` (`USI_EVENT_SNIPER_READY`, `USI_EVENT_PROTON_BOMB_READY`, `USI_EVENT_MECA_DROP`, ...).
   March has no such file and no loader: find June's loader of it and port that, and bring the file with `/x2-adopt-june` (no invented Lua). The items
   (100005740..100005743, "Heavenly Combat Support Kit (Sniper / Bomber / Quantum Bomb / Random Fire)", IT_OUTLAY) are in the staged (June) item.kim. Find how
   June drops them (a dungeon drop table, a script, a server push); if only a server did it, follow the "design missing June pieces" rule (what June does where the value
   is absent first, then a labelled stand-in proposed to the user).
3. **The entry.** `3230bdd0` is a `CX2Game` virtual in 11 game vtables (the dropped-item pick-up): find March's counterpart and add the item-id switch and the latch.
4. **Port the states** with their handlers under the reserved ids, registered in the base user InitState the way `3202b110` does (all 12 June classes call it).
5. **June quirks to keep, with JUNE_BUGS rows when ported:** registration `0x76` (`USI_EVENT_GUN_DANCE_INIT`) re-uses the table `USI_EVENT_SNIPER_READY` and
   SNIPER_INIT's frame / event / end handlers; `3200d3f0` is a StateEnd that itself calls `StateChange( USI_EVENT_SNIPER_END )` when the target NPC is gone;
   `USI_EVENT_GUN_DANCE_BACK_STEP` is registered nowhere.

Stop and ask the user before anything that needs a new server-side value, and say "this needs a higher model" if a Lua route is blocked.
Done means: each piece ported under its own flag with a funcmap row, or a row saying why it stays unported.
