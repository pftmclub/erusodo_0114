# June effect completion - C9h-3b - the riding pet's StateChange / StateChangeFuture rewrite (port: riding-pet-local keys)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-3b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-3 (identify). C9h-3a touches the same StateChange; whichever runs second rebases on the first, so do not run them at the same time |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_STATE` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX RIDINGPET_STATE` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_STATE"`) |
| Review | `kickoffs/june-effect/C9h-3b-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-3 - Done"
grep -n "Effect C9h-3" june2017_re/funcmap.tsv
```

March files: `X2Lib/X2RidingPet.h/.cpp`: StateChange :862 / :876, StateChangeFuture :775 / :789, OnFrameMove :54, the ctor :18.
The ONGROUND caller is in `X2Lib/X2GUUser.cpp` (June `320347d0 @32034858`; find March's counterpart). All are CP949, so edit them with `tools/bytepatch.py`.

## Task

Bring `CX2RidingPet::StateChange( const char* )` (`32d2f560`) and `StateChangeFuture( const char* )` (`32d2c8b0`) to June for the **riding-pet-local** keys, with
their consumers. The funcmap rows give every key, default, member and order.

Port:
- **The order and defaults of both parsers**, including the first-change flag `+0x48c` (read by OnFrameMove `32d2ec80`).
- **`ANIM_START_TIME`.** It feeds `ChangeAnim( name, transition, start )`; March's `CKTDGXSkinAnim::ChangeAnim` already takes the start time.
- **`ANIM_NAME_AT_PLAY_END`** and its player `32d286c0`, called from OnFrameMove `@32d2f3b1`.
- **`ONGROUND_CHANGE_STATE { OVERLAP_TIME, STATE_NAME }`**, its check `32d3cff0`, and that check's call from the user's land path.
- **`TARGET_RANGE` / `LIMIT_Z_DEPTH` / `DEGREE_RANGE` / `VALUE_POS`.** Store them only, with defaults 1500 / 500 / 45 / 0. Their reader is user code (`31fe2420`), and C9h-4 owns it.
- **The manager's anim-speed rates.**
  - Add the ten floats to `CX2RidingPetManager` (June ctor `321f6ec0`, all 1.0).
  - StateChange multiplies by them for the five state kinds (`32d277f0` = the third kind's getter). PASSIVE_SPEED_X uses the same pair.
  - No writer is known, so the multiply is 1.0 (June's own value). Say so in a comment.
- **The verbatim March moves** inside the user block. Keep March's plain fields where June writes protected (KProtectedType) values.
- **June's small differences in the user block:**
  - `SUPER_ARMOR_NOT_RED` is written only when `SUPER_ARMOR` is true;
  - `SUPER_ARMOR_TIME` goes into June's container. If March has no such member, keep March's vector and cite the difference.
- **The village path through `IsValidUser`** (`32d2c800`), if it differs from March :751.

Out of scope, and each needs a one-line comment where it would sit:
- **The transformer riding pets**, routed to backlog Stage 17: `TRANSFORM` (`32d281f0`), `ORIGINAL/TRANSFORM_SCALE_TIME` (`32d284d0`), `CHECK_RIDINGPET_SPACIAR_BUFF` (`32d2f3d0`; Buff Phase 10 recorded it as not ported), `APPLY_RIDINGPET_ANIM_MOVE` (user readers), the 22 state-id getters, and `32d3cf40`.
  - **Parse and store these keys** so the parser is June's. Leave the consumers out, because March has no transformer pet and no second state-id set.
- **The user-side June keys**, which belong to C9h-4. The parser reads them into locals and drops them, with a comment pointing at C9h-4:
  - `DISABLE_GRAVITY_TIME_*`, `UPDOWN_CANCEL_AFTER`, `SOUND_PLAY_ALL_OPTION`, `DELETE_EFFECT_SET_OFF_RIDING` / `_ON_SKILL_STATE_END`;
  - `CAN_PUSH/PASS_UNIT_TIME%d`, June's `SOUND_PLAY` reader, and the village square-unit sound block.
- `CUSTOM_RENDER_STATE` (backlog Stage 2).

**Reach.** No March riding-pet script carries any of these keys. The phase changes nothing visible on March data, so the play-test is a
regression pass: summon, walk, jump, attack and unsummon, in a village and in a dungeon.

**Done gate:** one `JUNE_FX RIDINGPET_STATE` line per StateChange while `_DEBUG` is on (state, anim, start time, at-end anim, onground
state); the strings are in the deployed exe.
