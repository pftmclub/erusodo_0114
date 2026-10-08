# June effect completion - C3g - port June's SET_LIMIT_MAX_G_SPEED (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3g` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3d-3 (found there). **Not with** a phase that edits `CX2GUUser::CommonStateStartFuture` / `CX2GameUnit::PhysicProcess` |
| Flag | new `SERV_IRUHADEV_JUNE_LIMIT_MAX_G_SPEED` |
| Log tag | `JUNE_FX C3g` |
| Review | `kickoffs/june-effect/C3g-review.md` (write it when done) |

## Read first

```sh
grep -n "(C3d-3)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-3 - Done"
```

## Task

C3d-3 found a June state key that no phase owns. `SET_LIMIT_MAX_G_SPEED` is a float state key:

- **User:** `CommonStateStartFuture` `320506f0 @32054734` (string `3379d12c`).
- **NPC:** `LoadStateData` `32784c00 @32787303` (string `337a9a90`).
- **Where it lands:** a protected float in the game unit at `+0x40f8` (`lea` @320548b6 in `32050230`'s neighbourhood). Readers by
  `dispsweep.py 40f8 40fc 4100`: `32015b30`, `322a0730`, `322a2700`, `3277fbf0`, `327ad4ad`.
- **March:** no reader of the key at all. `fMaxGSpeed` (`PhysicParam`, from the class script's `MAX_G_SPEED`) is the only fall-speed cap.

1. Identify the key's default and what each reader does with the value (a per-state fall-speed limit, applied where gravity is integrated?).
   Settle every x87 compare with `june2017_re/x87test.py`; verdicts into `june2017_re/funcmap.tsv`, tagged `Effect C3g`.
2. Port the whole mechanism under the new flag: the member, the user and NPC parse, the readers. No partial port.
3. June data that sets it: the 27 `*_DAMAGE_BOUNDING_DOWN` tables of `June_Bounding_States.lua` (`= -4000`, the nine March classes).
   Grep `june_src_fixed` for the other users first (`grep -rl SET_LIMIT_MAX_G_SPEED june_src_fixed`) and say how many states carry it.
4. Stage a play-test on a bounding DOWN hit (the C3d-3 staging, Mars in the El Tree, if it is still in the live data037; otherwise state
   what to use). The check: the unit's fall speed is clamped at the key's value during DOWN.
