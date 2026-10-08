# June effect completion - C9h-4b - June's sound records on the NPC, the square unit and the event scene

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4a (the record, the reader and the `CX2GameUnit` lists). Can run alongside C9h-4c / C9h-4d (different files), not alongside C9h-4e |
| Flag | `SERV_IRUHADEV_JUNE_NPC_SOUND` (NPC + event scene) and `SERV_IRUHADEV_JUNE_SQUARE_SOUND` (square unit) (+ `_DEBUG` if needed) |
| Log tag | `JUNE_FX NPC_SOUND`, `JUNE_FX SQUARE_SOUND` |
| Review | `kickoffs/june-effect/C9h-4b-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4 - Done" "C9h-4a - Done"
grep -n "Effect C9h-4" june2017_re/funcmap.tsv
```

March: `X2Lib/X2GUNPC.cpp` LoadStateData's sound block (:15427 and the per-sound-set block below it) and TimedEvent's player (:20470);
`X2Lib/X2SquareUnit.cpp` StateStart (:2429), the player (:1638) and the riding-off path (:1117); `X2Lib/X2EventScene.cpp:1290`. Edit them
with `tools/bytepatch.py`.

## Task

**NPC** (`SERV_IRUHADEV_JUNE_NPC_SOUND`).
- **LoadStateData** (`32784c00 @3278dad8-3278f103`): `SOUND_PLAY` through C9h-4a's reader, plus `SOUND_PLAY_ALL_OPTION` with its eight
  fields and `DELETE_CONDITION`.
  - Keep June's order inside the per-sound-set tables.
  - June has no `SOUND_PLAY_EX` or `STATE_SOUND_PLAY` reader on the NPC side. So `FIRE_DEVIL_CHAIN_BALL`'s `SOUND_PLAY_EX` stays unread.
    Note that in a comment.
- **The TimedEvent player** (`327970d0 @327986b5`):
  - `PlaySound( name, 3D, loop )`;
  - the attach `3227a740`;
  - March's `SetMax3DDistance`;
  - then the push into NPC `+0x8f74`. Settle with `june2017_re/offsetprobe.py` whether that is March's `m_vecLoopSound`. If it is, JUNE_BUGS
    116's NPC case is masked (the die path stops the list), so update that row's "Reached?" column.
- **The stop calls:** DamageReact `@327b52a8`, CommonStateEnd `@3277f850`, CommonStateStart `@327adaa7`. Check whether the NPC reads
  `CUSTOM_STATE_DELETE_SOUND` there (funcmap rows say the user does).
- **The event scene** (`325b6280`): `SOUND_PLAY` through the reader. That is the only June change found.

**Square unit** (`SERV_IRUHADEV_JUNE_SQUARE_SOUND`).
- **StateStart** (`323fad80`): `SOUND_PLAY` through the reader, at both read sites.
- **The player** `323f17b0` replaces March :1638:
  - it uses the riding pet's clock while riding (`+0x3d6`);
  - it calls the device `PlaySound( name, loop, 3D, 1 )` (`31ce0e10`);
  - it attaches through `323f1770`, which keeps `DCS_STATE_END` only, into a new list (`+0x5ec`);
  - records are not erased.
- **The stopper** `323f1730`:
  - call it from the square riding-off path (`323f70a0 @323f7219`, after `RemoveRidingPet`);
  - free the list in the dtor, without stopping it, as June does;
  - leave the riding StateChange's call to C9h-4e.

**Out of scope:** the `CX2Pet` loader (`326ef840`). Only June's Hedgehog pets use `SOUND_PLAY_ALL_OPTION`, so it belongs to backlog Stage 17.

**Reach.** The NPC keys are in Elysion objects, the Erda NPCs and character summons, and none of them is in March, so the NPC side is
inert on March data. Elysion's own port needs it. The square-unit sounds come only from the riding pet's village block (C9h-4e), so the
play-test is a regression pass: village NPC sounds and dungeon monster sounds, as before.

**Done gate:** both flags' `JUNE_FX` lines (one per attach while `_DEBUG` is on), and the strings are in the deployed exe.
