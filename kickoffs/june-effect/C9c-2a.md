# June effect completion - C9c-2a - identify June's unit-viewer effect player (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9c-2a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C9b (done 2026-10-07); can run alongside C9c-1 |
| Flag | none (no code) |
| Log tag | `[JUNE_FX]` |
| Review | none (C9c-2b's review covers it) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9b - Done"
```

Also read:
- `june2017_re/funcmap.tsv` rows tagged `Effect C9b`, plus `329585f0`, `321dfc40`, `321da420`;
- `june2017_re/c9b_unitviewer_rows.tsv`;
- March `X2Lib/X2UnitViewerUI.cpp`: `SetUnit` (~:1985), `PlayAnim` (:1857), `OnFrameMove`, `NotifyShowObjectChanged`,
  `OnFrameRender_Draw`, `Render_DrawForAvartaScreenCapture`, `PlayByMotionType`;
- March `X2Lib/X2EffectSet.h` (EffectSetData / EffectData and the `ANIMTIME_ON_START_UNITVIEWER` / `FOLLOW_UNIT_VIEWER_SCALE` members).

## Task

Identify, in Ghidra, enough of June's viewer effect player for a Sonnet port (C9c-2b). Write funcmap rows tagged `Effect C9c-2a`.

1. **Play** (`32959640`) and its helpers: `3295a080` (entry init), `3295a400` (insert), `32959970` / `32959e40` (the templet `+0x70` list:
   what it is), `31e4f5d0`.
   - Give the 0x40-byte entry layout: handle, skin anim, group, name, `+0x0c`/`+0x3c` (the "copy +0xf into +3" loop at the end of
     `32959640`), and what `+0x190` means.
   - Map each to March members by key (EffectData fields, March's `CKTDGParticleSystem` / `CKTDGXMeshPlayer` handle APIs).
2. **Per-frame update** (`329585f0`): per entry, give the bone-trace position, POSITION_OFFSET, TRACE, the `FOLLOW_UNIT_VIEWER_SCALE`
   scaling, the `ANIMTIME_ON_START_UNITVIEWER` trigger call (`321dfc40` with no instance), lifetime / erase, and the `32957980` /
   `329582b0` / `32957170` calls.
   - The hard-coded `EffectSet_SpaceMoonRabbit_motion` branch: say whether March has that set (if not, it is a dead branch to record, not port).
3. **Stop** (`329594c0`, `32959250`): what `32959250(group)` does beyond the two lists.
4. **Where the state lives.** June keeps lists `+0x194`/`+0x1a4` on a base viewer (vtable `33471140`, also used by `32e4f600`). Decide
   whether C9c-2b puts them in March's empty `CX2BaseUnitViewerUI` (inactive: `MONSTER_TROPHY` is off) or directly in `CX2UnitViewerUI`.
   Give the reason.
5. **Hard-code map.** For each March hard-coded viewer class effect (`m_hSeqHandFire`, `*SPK1/2`, `*HandElectricALP1/2`,
   `*EnergyBullet1-3`, SBH, crescent cut, ETK/CDC/RTR/RNW meshes), name:
   - the table row and June templet that replace it;
   - every March function that creates, toggles, draws or destroys it.

   That list is what C9c-2b retires under the flag.
   - Flag any March effect with **no** table row: it must stay, or there is a June gap to record.
   - Write the map to a `.tsv` (`june2017_re/c9c2a_hardcode_map.tsv`).
6. **Templet staging.** Check that `kom_contents/data036_kom/UIEffectSet.lua` decompiles cleanly with the mar-2014 tools, so the 15 June
   templets can be appended verbatim. If it does not, stop and give the user the options.

End by writing `kickoffs/june-effect/C9c-2b.md` (Sonnet: player + hooks + templets + retirement, one phase). Do not write game code.
