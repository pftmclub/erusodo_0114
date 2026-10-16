# June effect completion - C3h - June's APPLY_MOVEMENT_SPEED (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3h` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3g (found there). **Not with** a phase that edits `CX2GUUser::CommonStateStartFuture` or `CX2GUUser::PhysicProcess` (C3e-1) |
| Flag | new `SERV_IRUHADEV_JUNE_APPLY_MOVEMENT_SPEED` |
| Log tag | `JUNE_FX C3h` |
| Review | `kickoffs/june-effect/C3h-review.md` (write it when done) |

## Read first

```sh
grep -n "(C3g)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3g - Done"
```

## Task

C3g found a June user state key no phase owns. `APPLY_MOVEMENT_SPEED` is a bool:

- **Parse:** `CommonStateStartFuture` `320506f0 @32054181-3205418e` (string `3379d178`), stored as the new first byte of June's PhysicParam
  (`+0x300`; June's PhysicParam is 0x34 bytes, March's 0x30) and copied to the minor unit's PhysicParam (`+0x36c`).
- **Reader:** `PhysicProcess` `32015b30`, the x-speed decay: when the byte is set, the decay is scaled by `31e4f990()` / `+0x324` (walk speed).
  Identify `31e4f990` and every other reader of `+0x300` (`dispsweep.py 300`, then filter to the unit's functions).
- **June data:** one state, `Ara_Martial_Artist.src.lua:7334`.

1. Identify the default, every reader and what each does. Settle every x87 compare with `june2017_re/x87test.py`; verdicts into
   `june2017_re/funcmap.tsv`, tagged `Effect C3h`.
2. Port the engine side under the new flag as a plain member (do not change March's PhysicParam layout unless a reader needs it; say why).
   The minor unit's copy is Lu/Ciel's (checklist 7o): do not port it.
3. Ara's state reaches March only with the Ara port: add the row to checklist 7o if C3g's line is not enough. Play-test is a regression only.
