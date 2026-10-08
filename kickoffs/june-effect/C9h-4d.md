# June effect completion - C9h-4d - June's effect-set delete conditions: STAGE_END, OFF_RIDING, SKILL_STATE_END, the hyper check

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4d` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4 (identify). Not at the same time as C9h-4a / C9h-4c (same `X2GUUser.cpp` / `X2GameUnit.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_EFFECT_SET_DELETE` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX EFFECT_SET_DELETE` |
| Review | `kickoffs/june-effect/C9h-4d-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4 - Done"
grep -n "Effect C9h-4" june2017_re/funcmap.tsv
```

March: `X2Lib/X2GUUser.cpp` LoadEffectSetFromScript (:40411, the delete keys at :40636), CommonFrameMove_EffectSet (:40671),
ProcessRidingOn's off branch (:38930); `X2Lib/X2GameUnit.cpp` AddEffectSetDeleteCondition (:18548) and the DeleteEffectSetOn* functions
(:7116); `X2Lib/X2StateDungeonGame.cpp` Handler_EGS_DUNGEON_STAGE_LOAD_NOT. March's `Enum.lua` lives in `kom_contents/data036_kom/`.

## Task

1. **The parse** (the tail of `32024ff0`, in June's order). Add three June-only bytes: `DELETE_EFFECT_SET_ON_STAGE_END`,
   `DELETE_EFFECT_SET_OFF_RIDING`, `DELETE_EFFECT_SET_ON_SKILL_STATE_END`. Then add `SET_HYPER_EFFECT_SET_CHECK_HYPER_STATE`: when the now
   state data's byte `+0x10` is set (identify the March member) and `HYPER_EFFECT_SET` is false, it overwrites the hyper flag.
2. **The play site** (`32007570`). Push each played handle into three new lists: STAGE_END, OFF_RIDING, and SKILL_STATE_END as
   `{ handle, GetNowStateSkillID }` pairs (only when the id is > 0).
3. **`AddEffectSetDeleteCondition`** (`3227a570`). Add June's case 5 (user only, a skill pair).
   - June also pushes every handle into a list at `+0x3e90`, which `3228a120` walks at CommonStateEnd. Identify `3228a120`'s callee
     `321da070` and port that list too if March lacks it (funcmap row `3228a120` calls it DeleteEffectSetOnStateEnd; check).
   - `AddEffectSetDeleteCondition_LUA` (`3227a670`) belongs to the Elysion plan. Make sure the case 5 it will reach exists.
4. **The stoppers:**
   - `3228a190` (STAGE_END), at the top of `Handler_EGS_DUNGEON_STAGE_LOAD_NOT` (`324d9550`), for every user unit;
   - `3228a3b0` (OFF_RIDING), in the riding-off branch (`3204d120 @3204d330`), right after `RemoveRidingPet`;
   - `3228a490` (SKILL_STATE_END), in CommonStateStart `@320606a2`, before C9h-4a's sound check.
5. **Data:** add June's `DCES_SKILL_STATE_END = 5` to March's `DELETE_CONDITION_EFFECT_SET` table in `Enum.lua`, verbatim. Pack data036
   (`/elstools pack data036`) and **ask the user before the copy**.

**Out of scope:** the `EFFECT_SET_LIST` variants (`_EX_TRANS_1/2`, `_EX_NO_TRANS`, `_ULTIMATE_EX(_APPLIED_SKILL)`, `_EX_APPLIED_SKILL`,
`_FINAL(_ULTIMATE)`). They select by skill memo, transcendence or applied-skill level, and only class scripts carry them, so they are
character material (checklist 7h).

**Reach.**
- The new keys are only in June scripts: OFF_RIDING in 6 June-only riding pets, the hyper check in Ain_Apostasia and PVP_HERO_DEAR.
- On March data the change is inert apart from the new `+0x3e90` list, if it turns out to be new.
- Play-test: a dungeon stage change with a skill effect set running, and a riding-pet summon / unsummon, both as before.

**Done gate:** a `JUNE_FX EFFECT_SET_DELETE` line per stop call that stops something while `_DEBUG` is on; strings in the deployed exe;
`PACK OK` for data036.
