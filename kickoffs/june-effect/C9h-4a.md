# June effect completion - C9h-4a - June's unit sound records, delete conditions and players (CX2GameUnit + user)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4 (identify). Not at the same time as C9h-4c or C9h-4d (all three edit `X2GUUser.cpp` / `X2GameUnit.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_UNIT_SOUND` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX UNIT_SOUND` (`python tools/logs.py --grep "JUNE_FX UNIT_SOUND"`) |
| Review | `kickoffs/june-effect/C9h-4a-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4 - Done"
grep -n "Effect C9h-4" june2017_re/funcmap.tsv
grep -P "^(SOUND|STATE_SOUND|CUSTOM_STATE_DELETE_SOUND|DELETE_CONDITION)" june2017_re/c9h4_unit_key_census.tsv
```

March: `X2Lib/X2GameUnit.h:1346` (`SoundPlayData`), `X2Lib/X2GUUser.cpp` CommonStateStart sound block (:15181), CommonFrameMove_TimedEvent's
loop (:19201), `X2Lib/X2Main.h` `GetNationFlag()`. All CP949: edit with `tools/bytepatch.py`.

## Task

Port June's sound system on `CX2GameUnit` and the user, whole. The funcmap rows give every address, offset and default.

1. **The record.** Add June's two fields to `CX2GameUnit::SoundPlayData`: a `vector<int>` of delete conditions (the ctor seeds one
   `DCS_NONE` = 0, `320795c0`) and `bool m_bLoop`. March already has the max distance and the 3D flag. The NPC and the square unit use the
   same struct, so keep it source-compatible for them (C9h-4b fills their sides).
2. **The nation-suffix reader `32078fd0`**, as a helper both this phase and C9h-4b call. It reads `g_pMain->GetNationFlag()`. Use June's
   literals 0x200 "JP", 0x400 "US" and 0x700 "BR": June's `NF_BR` is 0x700, while March's enum says 0x1000 and gives 0x700 to `NF_VN`.
   Comment the difference and keep June's value. On March's US data (`NF_US`, no `_US` tables) it resolves to March's table name.
3. **`CX2GameUnit`'s five sound-handle lists, plus the code that fills and stops them:**
   - the lists themselves (June `+0x3f20 / +0x3f30 / +0x3f40 / +0x3f50`, and the `{ sound, skill id }` pairs at `+0x3f60`);
   - the attach `3227a740`;
   - the four stoppers `32276ce0`, `32276ca0`, `32276d20`, `32276d60` (map `31d416c0`; it is likely `CKTDXDeviceSound::Stop`);
   - the skill-state check `32278200`.

   **`32276d20` (DIE) has no caller in June (JUNE_BUGS 116).** Port the function and call it nowhere.

   Before porting the raw-pointer stops, check how March owns a `CKTDXDeviceSound*` returned by `PlaySound` (release and reuse). If a
   stored pointer can dangle, that is a JUNE_BUGS row (keep June's code), unless March would crash, in which case ask the user.
4. **The user's CommonStateStart, in June's order** (`32057b80`):
   - `CUSTOM_STATE_DELETE_SOUND` (`@3205ab7f`, stop condition 4);
   - the sound block `@3205ce80`: `SOUND_PLAY` through the reader, `SOUND_PLAY_EX`, `SOUND_PLAY_ALL_OPTION`, and `STATE_SOUND_PLAY` into a
     second list (`+0x85d4`);
   - then March's `SOUND_SET` block, unchanged;
   - the skill check `32278200` at `@320606a9`. C9h-4d puts `3228a490` before it.
5. **The players.**
   - June's `31fea120` replaces March's loop in `CommonFrameMove_TimedEvent`. It plays a record once and **erases** it, uses the riding
     pet's clock while riding, and passes LOOP and 3D through.
   - The `STATE_SOUND_PLAY` player `@31ff396f`: map `32dfd2d0` and `31fdf4c0`. Add funcmap rows for what you resolve.
6. **The stop calls**, each beside the March effect-set delete it sits next to in June:
   - DamageReact `@3204a71f`;
   - the CommonFrameMove shock block `@32068a3c`;
   - CommonStateEnd `@320157fc`;
   - CommonStateStart `@3205ad3e`.

**Out of scope, each with a one-line comment at its site:**
- `EmotionFrameMove`'s riding-sit replay (`3204a860`) belongs to backlog Stage 10.
- The NPC, square-unit and event-scene sides belong to C9h-4b.
- The riding pet's writes belong to C9h-4e.

**Reach.** No March user script carries the new keys, but June's player is not inert: March replays a record on every loop of the
animation, and June erases it after the first play. A state whose animation loops (a WAIT habit, a held walk or dash) plays its
`SOUND_PLAY` once per state entry. That is June's behaviour, so keep it, and make it the play-test's main check (compare footstep and
voice sounds with the previous exe).

**Done gate:** one `JUNE_FX UNIT_SOUND` line per attach while `_DEBUG` is on (name, conditions, list), and the strings are in the
deployed exe.
