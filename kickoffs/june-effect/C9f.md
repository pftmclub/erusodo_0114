# June effect completion - C9f - June's per-equip effect sets (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9f` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9e review (it found the system) |
| Flag | none (no code; the port phase this one plans gets its own) |
| Log tag | none |
| Review | none (identify only); this phase writes the port kickoff `C9f-2.md` and its STATUS row |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9e - Review"
```

Also read `june2017_re/funcmap.tsv` rows tagged `Effect C9e review` (6 rows, all `candidate`).

## Task

The C9e review identified `323f56d0` (the call in `CX2TFieldGame::RemoveUnit` `323af8f0 @323af9f6` that C9e left out). It is not the emotion
system. It belongs to a June-only per-equip effect-set system that no plan owns, so it is planned here (user rule 2026-10-07: an unowned
June branch gets its own phase and follows June). What the review already has:
- **Stop**: `3295ca60( bool )` stops `CX2Eqip +0x15f8` on the UI effect-set manager.
- **Lookup**: `3280b640( item id, &record )` is a map on the `g_pData +0x34` object (sentinel `+900`). The record is
  `{ int, wstring, int, wstring, +0x50, +0x54 }`.
- **Group filter**: `31fa2f20( 0x51, &vec )` lists the viewer's equips through `32807db0` (group `0x51`).
- **Play sites**: GUUser `CommonFrameMove 32063650 @32065e1a-3206603f`, square `32400620 @3240217e-3240223d`, viewer `31fa8ce0 @31fa9bdd`,
  `32961060`/`32961280`. Timers are at `+0x15f4`/`+0x1600`.
- March has none of it: no handle in `X2Eqip.h`, no map in `X2ItemManager.h`.

Identify, in Ghidra, read-only:
1. **The loader.** Find what fills the `3280b640` map, which Lua file and keys it reads, and whether that file is in June's `data036`
   (`june_src_fixed/`). The review ruled out `EFFECT_SET_NAME_BASIC` / `_TRANSCENDENTAL`: those are `CharCustomData.lua`'s hyper-effect keys,
   read by `326525d0`.
2. **The record's fields.** Name the 6 fields, what group `0x51` is (`32807db0`'s two maps), and what the `+0x198` gate on the equip is.
3. **Every play and stop site**, with the timers' meaning, the owner passed to `PlayEffectSet`, and village vs. game vs. viewer.
4. **Reach.** Which items carry an entry, and whether any is on the save (`sqlite3 els_db.sql`) or in March's item templets. If none, the
   port is inert, like C9e.

Done = funcmap rows confirmed or corrected (tag `Effect C9f`), the port list in the Done note, and `kickoffs/june-effect/C9f-2.md` + its
STATUS row. C9f-2 is the port: Sonnet 5.5 high, its own flag, with the `323f56d0` stop in `RemoveUnit` beside C9e's `StopJuneEmotionEffectSet()`.
Hand a design question to the user only if copying June would break March, for example a missing enum or a missing item-manager hook.
