# June effect completion - C9h-4g - June-only blocks found next to USEFUL_VALUE (not started)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (a decode phase first), then type `/x2-phase june-effect C9h-4g` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4f (same two parse sites in `X2GUUser.cpp`: not at the same time as C3d-2, C3d-3, 4a, 4b, 4d) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_*` (name it when the decode says what the pieces are) |
| Log tag | `JUNE_FX <subtag>` per flag |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4f - Done"
grep -n "(C9h-4f)" june2017_re/funcmap.tsv
```

## Task

C9h-4f's decode found three June-only blocks that no STATUS row owns (rule: a June-only path gets its own phase, no question). The first two sit in the same parses as `USEFUL_VALUE`:
1. **`USEFUL_STRING` vector.** `320506f0 @32056b0e-32056bf2` (Future, vector `+0x8f40`, `+0x8f44` end, element stride `0x1c`, key `3347489c`, indexed `USEFUL_STRING0..`)
   and `32057b80 @32060dda` (Now, vector `+0x8f30`). Decode the element (`32feafd0` reads it, `31f54a10` logs `3344f1f4` on a miss), its readers (`dispsweep.py 8f40 8f44 8f30`) and port it.
2. **`CUSTOM_OFFSET_NOT_SELECT_UNIT`** (`33478e04`): `320506f0 @32056bf2-32056c4b` reads two floats into `+0x8f54` / `+0x8f58` from a plain key (`32fe9940`). Decode, find readers, port.
3. **The June-only states that read the `USEFUL_VALUE` sets** (funcmap rows `(C9h-4f)`): each class's `*_WAIT_STATE_CHANGE` (`ENSI_WAIT_STATE_CHANGE` is id `0x82` in the class registered at `329ba9c0`; the shared EventProcess is `31fc8620`) and the
   event states `0x6e..0x76` registered by `3202b110` (June Enum: `0x6e..0x71` = `USI_EVENT_SNIPER_INIT / _CONTROL_IDLE / _CONTROL_FIRE / _END`, `0x72..0x74` PROTON_BOMB, `0x75` MECA_DROP_INIT, `0x76` GUN_DANCE_INIT) (`31fc9a20`, `3200d450`, `3200d1d0`, plus `3200d3f0`, which reads no
   USEFUL_VALUE but belongs to the same family: its StateStart-shaped handler; funcmap rows corrected by the C9h-4f review: `3200d450` changes to
   `USI_EVENT_SNIPER_END` when the timer RUNS OUT, `3200d1d0` creates NPC `0x10f6` = `NUI_NEIL_THE_SNIPER_EVENT` (own unit only; the line-map distance call `31c5f090` is inferred) and sends the target `NEIL_THE_SNIPER_*` NPC to its DYING state; `3200d1d0` is also named for registration `0x76` (GUN_DANCE_INIT): re-check). They are June states March has no enum row for: design the id placement
   with `C3d-2`'s ids (`USER_STATE_ID16`), then port the handlers; the per-class registration of `*_WAIT_STATE_CHANGE` is a char-port item (checklist 7h), the shared EventProcess is ours.
   Check first whether any March script or class can reach them at all; if none can, port the engine handler and leave the registration to the class ports (no invented data).

Done means: each piece ported under its own flag with a funcmap row, or a row saying why it stays unported; June scripts are not touched.
