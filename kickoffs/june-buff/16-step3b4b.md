# June buff port - Phase 16 step 3b, part 4b - the effect-set buff list (SIEE 15's aura buff)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b4b` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b4 review done. Not in parallel with 16-step3b5 (X2GUUser.cpp) or june-effect C6b (X2EffectSet.cpp, OnFrameMove_CreateEffect) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b4b-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 4" "Phase 16-step3b4 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b4" for `321e3650` and `321ee480` are the starting decode.

## Task

User decision 2026-10-06 ("follow June"): port June's effect-set buff list, so that SIEE 15's aura buff (and every other
June caller's buff) reaches the units the set's damage effects hit. Port June's whole functions where they differ, cite
June addresses, byte-level CP949 edits.

1. **The store.** `321e3650( handle, BFI, level )`: decode the handle as `GetEffectSetInstance` does, push
   `{ BFI, level, vector }` (0x18 bytes, `321f4690`) onto EffectSetInstance `+0x160`. Decode what the entry's vector holds
   (`321ee480` reads it as modifiers: `32d418a0` / `32d41960`) and who fills it.
2. **The consumer.** `321ee480( CEffect*, list )`, called twice by OnFrameMove_CreateEffect (`@321ef676` with EffectData
   `+0x118`, `@321ef689` with instance `+0x160`): per entry, clone `GetBuffFactorPtr( BFI )` at its level, apply the
   modifiers, and add it to the damage effect's damage data (`321f5130`). Find EffectData `+0x118`'s Lua key (no June
   EffectSet uses `BUFF_FACTOR`) and port both calls, because the function is one June function.
3. **The caller.** Replace 16-step3b4's `unsupported (effect-set buff list, 321e3650)` line in SIEE 15
   (`ProcessSocketEffectEx`, `@3202a1aa-1f4`) with the attach call. The class-code callers (`32a0e5f0`, `32ab2ff0` x3,
   `32ad2bb0`) go to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7 (standing rule D8).
4. **Damage side.** Check that the damage data's buff vector (`321f5130`'s target) is the one March already applies
   on hit (DamageReact's buff-factor path, the `IS_AURA_BUFF` party gate in `X2DamageManager.cpp`). If it is not, say
   what is missing before porting more.

Hand-off: the aura still needs `EffectSet_Hero_Dungeon_Aura_Buff`, its damage effect and mesh (16-step3b4c) and its buff
factor (Phase 13). If those are not staged yet, the play-test is a regression run plus the new path's log lines; say which.
