# June effect completion - C3z-4 review - batch 1 bindings and the class-aware census

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4 batch 1 done (and ideally play-tested) |
| Flags | `SERV_IRUHADEV_JUNE_NPC_CREATE_NPC_REQ`, `_NPC_STORED_GUUSER`, `_GAME_LUA_INT`, `_NPC_INIT_STATE_COOL_TIME`, `_GAME_HOST_GAME_UNIT`, `_NPC_GET_DYING_START`, `_GAME_STAGE_QUERY`, `_GAME_NPC_COUNT`, `_SUBSTAGE_NPC_TEAM` (`_C3Z4_DEBUG` off) |
| Log tag | `JUNE_FX C3z-4` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4 - Done (batch 1)"
awk -F'\t' '$7=="1"' june2017_re/c3z4_bind_plan.tsv
```

## Task

Review batch 1 against June, not against the Done note:

1. **Census.** Read `june2017_re/c3z4_class_census.py`: the June class list (tokenizer of `func_strings.tsv` rows `31be67a0` /
   `31d95080`), the preprocessed March binders and `class_inh` closure, and the receiver typing (callback signatures from March's
   `lua_tinker::call` sites, getter return types read inside the declaring class, helpers typed from their call sites). Look for
   a receiver it types wrongly or a gap it cannot see; the Done note gives the typed share.
2. **Each binding.** Re-read June's body at the address in the plan TSV (`june2017_re/disasm_range.py`) and compare it with the
   March code: argument order and defaults of the five `CreateNPCReq*` wrappers (keycode from `.y` via ftol, team, AI, ally UID),
   the stored-user type gate, `InitStateCoolTime` (state id from the name, runtime vs templet index), the `GetIsLastStage`
   condition, and the March -> June `X2_STATE` translation in `GetNowStateID_LUA` (June's table: `june_src_fixed/data036_kom/
   Enum.src.lua` `X2_STATE`; March's values: `june2017_re/enumprobe.py`).
3. **The two play-test fixes.** `GetNPCUnitNumCheckTeam` (`322d4a10`) and the sub-stage NPC team: June's parser `327eb8e0`
   (`TEAM` -> NPCData `+0x5c`), the emulator's `m_cTeamNum`, and `CX2DungeonSubStage::CreateGUNPC` vs June `327ef570` (team, type-4
   ally AI). Check every other reader of the AI type (`== NAT_ALLY` sites) behaves as June's does for type 4.
4. **Hygiene.** `flagcheck --range` over the C3z-4 commits; every flag off must compile March's original behaviour.

Fix what is wrong behind the same flag (or a `_FIX` flag if June-behaviour changes), and say so in the Review note.
