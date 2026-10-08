# June effect completion - C9h-3a - the in-game riding pet's UI effect sets (port: riding pet + engine holder)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-3a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-3 (identify). C9h-2a's engine pieces (`+0xdd`, `321e28d0`) are in and sit beside this phase's |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX RIDINGPET_FX` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_FX"`) |
| Review | `kickoffs/june-effect/C9h-3a-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-3 - Done"
grep -n "Effect C9h-3" june2017_re/funcmap.tsv
grep -n "^321dafb0\|^321d97c0\|^321de4f0\|^321e1bf0\|^321e28d0" june2017_re/funcmap.tsv
grep -n "^| 10[789] |\|^| 110 |" JUNE_BUGS.md
```

March files:
- `X2Lib/X2RidingPet.h/.cpp`: SetRidingPet :433, SetGameRidingPet :464, StateChange :938, OnFrameRender_Prepare :200, the dtor :59 (lines after C9h-3b);
- `X2Lib/X2RidingPetManager.cpp`: NotifyShowObjectChanged :234, RemoveRidingPet :434;
- `X2Lib/X2EffectSet.h/.cpp`: PlayEffectSetByPet :3479, the C9h-2a overload, GetEffectPosition, GetCombineMatrix, GetEffectPositionByBoneName, GetUnitRotation and OnFrameMove_CreateEffect.

All are CP949, so edit them with `tools/bytepatch.py`.

## Task

Port June's riding-pet UI effect-set system **whole**, plus the engine holder it plays through. Every address, offset, default and
order is in the `Effect C9h-3` funcmap rows: transcribe them, do not re-derive.

1. **Engine: the riding-pet holder (June instance `+0xd8`, mode 3 = `ESM_COMMON`).**
   - Add the holder: a `CX2RidingPetPtr` member on `EffectSetInstance`. It is 0 in the ctor (`321df230 @321df3dd`) and released in the dtor (`321e3a80 @321e3d79`).
   - Add `CX2EffectSet::PlayEffectSet( const wstring&, CX2RidingPetPtr )` = `321e2790`. Its shape is PlayEffectSetByPet's (`m_bHyper` false, `m_fPowerRate` -1, `m_fLifeTime` -1, mode `ESM_COMMON`), with no scale write. It is always called on `g_pData->GetUIEffectSet()`.
   - Port the holder's consumers:
     - GetEffectPosition `@321dcf6b`;
     - GetCombineMatrix `@321dc81f`;
     - GetUnitRotation `@321ddf69`;
     - the create-time anim-time gate `@321eeb1e`;
     - GetEffectPositionByBoneName `@321dd7ac`. Decide its reachability as C9h-2a did for the empty-name branches: port it if it is reachable, otherwise extend the comment.
   - Map each June pet offset to a March `CX2RidingPet` member and cite it:
     - bone position `321f2e60` / `321f2eb0`;
     - skin anim `320719f0` / `32071a10`;
     - `+0x33c` (land position), `+0x390` (position), `+0x324` (rotation).
   - **Correct the C9h-2a comment** at `X2EffectSet.cpp:4945` (and the one at :6383) while you are there. `@321dd1a3` / `+0xe0` is the field NPC (C9h-5), not the riding pet.
2. **Riding-pet members.**
   - `+0x4ac`: a hide mirror (bool, ctor 0).
   - Two `vector<CX2EffectSet::Handle>`: `+0x4b0` is persistent and `+0x4c0` is per-state.
   - The reserved map `+0x4d0`: `map<int, { deleteCond, playCond, name }>`.
   - Add the helpers `32d29240` (name), `32d292b0` (find a playing set by templet name, scanning `+0x4b0` only), `32d291a0` (show/hide all, then `+0x4ac = 0` unconditionally; JUNE_BUGS 110, keep it) and `32d293e0` (stop all, then clear).
3. **Loader `32d2b0d0`** (INIT_EFFECT): call it in SetRidingPet and SetGameRidingPet **right after InitState and before the motion open**, where June's `32d3cbf0` / `32d3cd10` call it (after `32d36600`).
   - The tuple order is `name, index, playCond, deleteCond`.
   - The loop breaks at the first incomplete tuple.
   - BY_UIEFFECTSET handles are pushed into `+0x4b0` even when they are -1.
4. **StateChange** (only these blocks; C9h-3b owns the rest of the rewrite):
   - the reserved delete blocks `@32d31013` (before the user test);
   - the UI-set block `@32d35a05`: stop `+0x4c0` without clearing it (JUNE_BUGS 107), then the names at odd indices only, then `DELETE_EFFECT_SET_ON_STATE_END` chooses `+0x4c0` or `+0x4b0`;
   - the tail `32d2bad0` (PLAY_RESERVED_UI_EFFECT_SET), which runs for every state. In a game whose owner user is not found, June jumps from `@32d31193` straight to this tail and skips the UI-set block (and the village block); outside a game both run.
   - **Keep June's queued-stop outcome (JUNE_BUGS 111).** June's `321dafb0` only queues a stop and `321d97c0` does not check the queue, so a reserved set stopped by the delete blocks still counts as playing for the tail's `RESPC_NOT_PLAY` test and is skipped (then dies). March's StopEffectSet is immediate: collect the reserved handles to stop, run the tail's test treating them as still playing, and stop them after the tail (or an equivalent that gives the same result). Cite it.
5. **Visibility and lifetime.**
   - June's OnFrameRender_Prepare village branch (`32d2c1e0 @32d2c4b2`): square unit flag `+0x518` vs `+0x4ac`. Identify the March name; March's Prepare tests `GetPlanRender()`.
   - Manager NotifyShowObjectChanged (`321f5cb0`): add the `32d291a0( b )` call at the end.
   - RemoveRidingPet (`321f6190`): add the `32d293e0` call after the erase.
   - The riding-pet dtor (`32d2bdd0`) frees everything without stopping (JUNE_BUGS 109).
6. **Data.**
   - Add June's two tables to March's `Enum.lua` verbatim, as C9f-2 did for the DITT tables (`june_src_fixed/data036_kom/Enum.src.lua:17597-17604`):
     `RESERVED_EFFECT_SET_PLAY_COND { RESPC_NOT_PLAY = 1, RESPC_NONE = 0 }` and
     `RESERVED_UI_EFFECT_SET_DELETE_COND { RESDC_NONE = 0, RESDC_STATE_CHANGE = 1 }`.
   - Run `/elstools pack data036`, then ask before `/x2-kom-packed copy data036`.
   - Do **not** adopt June's riding-pet scripts here. C9h-4 owns that ordering (June moved each pet's summon and start sounds into `SOUND_PLAY_ALL_OPTION`).
7. **Logging.** Write one `JUNE_FX RIDINGPET_FX` line per loader run (pet id, reserved count, BY_UIEFFECTSET count). Lines per StateChange UI-set block that plays something (state name, count, which vector) go under `_DEBUG` only: in a village they would fire for every player's pet.

**Reach and the play-test.**
- The UI-set block runs for every state of every riding pet, the 15 March ones included, because March's scripts already carry `EFFECT_SET_LIST`.
- In a dungeon, June plays each name twice: once through the user's loader, timed, and once here, immediately (JUNE_BUGS 108). That is visible.
- The hand-off must cover summon, wait, walk and an attack:
  - in a village (sets now appear where March showed none);
  - in a dungeon (look for doubled effects; that is June's behaviour, kept);
  - hiding and unsummoning (sets stop and do not linger).

**Done gate:** `JUNE_FX RIDINGPET_FX` lines in the play-test log; the strings are in the deployed exe.
