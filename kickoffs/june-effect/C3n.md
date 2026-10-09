# June effect completion - C3n - port June's world gravity rate (live re-apply, ResetPhysicParam trim, NPC state start)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3n` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3m; **not at the same time as C3j or C3k** (same NPC CommonStateStart code) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY`, `SERV_IRUHADEV_JUNE_WORLD_RATE_TRIM` |
| Log tag | `JUNE_FX C3n` (`python tools/logs.py --grep "JUNE_FX C3n"`) |
| Review | none unless the port finds something new |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3m - Done" "C3n - port"
grep -n "(C3m)\|Effect C3m" june2017_re/funcmap.tsv
```

## Task

Port the four June deltas that C3m identified. Take each address and offset from the C3m funcmap rows, and re-read June's bytes wherever a row is unclear.

1. **`SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY`**:
   - **The setter.** `CX2World::SetGravityAndSpeedRate` (`32800800`) stops being inline in `X2World.h`. After the store, it does what June does: if `g_pX2Game`
     is valid and `GetGameType() == GT_DUNGEON`, it walks the game's unit list (`327cab80`) and calls the new `CX2GameUnit` function for `32296290` on each
     non-NULL unit.
   - **That function.** It takes a one-time backup of the whole PhysicParam, guarded by a bool that only the ctor clears. Then it sets fReloadAccel, fGAccel
     and fMaxGSpeed to backup x rate. There is no MAGIC and no `!= 1` test.
   - **Where the backup lives.** The backup and its bool are plain `CX2GameUnit` members, not PhysicParam fields. Nothing copies them back, so the C3h
     whole-struct copies must not touch them.
   - **The Lua binding.** `X2Main.cpp:3696` binds `&CX2World::SetGravityAndSpeedRate`. Check that it still binds the out-of-line function.
2. **`SERV_IRUHADEV_JUNE_WORLD_RATE_TRIM`**:
   - **`ResetPhysicParam`** (`X2GameUnit.cpp:4196-4216`) keeps only the three gravity lines. March's four speed lines go to `#else`.
   - **NPC `CommonStateStart`, accel** (`X2GUNPC.cpp:15568-15575`, the `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch). fGAccel and fDownAccel are always
     written as `( flag ? state value : m_OrgPhysicParam value ) * rate * MAGIC`.
   - **NPC `CommonStateStart`, speeds.** SPEED_X / SPEED_Y are stored unscaled. Passive speed and the heavy-monster tail stay as they are.
3. **`JUNE_BUGS.md`, two rows:**
   - **The stale backup.** A `ResetPhysicParam` with rate != 1 before the first setter call bakes a scaled value into the backup, which is then scaled again.
   - **The MAGIC mismatch.** The setter path has no 0.5 / 1.5 factor, while `ResetPhysicParam` and the NPC block do.
4. **Log.** Write one `JUNE_FX C3n` line per setter call (rate, game type, unit count). Throttling is not needed, because no shipped script calls it.
5. **Hand-off.** It is a regression play-test: a dungeon and a field, with jumps, falls and NPC launches looking as before, and no `JUNE_FX C3n` line. Offer
   a dev-console positive test only if the console can reach `g_pX2Game:GetWorld()` from raw Lua (check the binding first, do not guess). The test would be
   `SetGravityAndSpeedRate( 0.5 )` in a dungeon, so falls get slower.
