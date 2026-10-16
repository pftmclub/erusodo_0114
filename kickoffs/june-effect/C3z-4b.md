# June effect completion - C3z-4b - unbound Lua methods, batches 2 and 3

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4 batch 1 play-tested |
| Flag | the `flag` column of `june2017_re/c3z4_bind_plan.tsv` (one per family; reuse `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` for logs) |
| Log tag | `JUNE_FX C3z-4` (`python tools/logs.py --grep "JUNE_FX C3z-4"`); ErrorLog `can't find '<name>' class variable` |
| Review | `kickoffs/june-effect/C3z-4-review.md` covers batch 1; write `C3z-4b-review.md` when done |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4 - Done (batch 1)"
awk -F'\t' '$7=="2" || $7=="3"' june2017_re/c3z4_bind_plan.tsv
```

## Task

Port the `batch 2` and `batch 3` rows of `june2017_re/c3z4_bind_plan.tsv`, in `n_dungeons` order. Each row names June's binder
site, the bound function and what its body does (C3z-4 read them). Batch 1's code (`X2GUNPC.cpp`, `X2Game.cpp`, the
`//{{ ... C3z-4 batch 1` blocks in `X2Main.cpp`) is the worked example.

- **Rows that need more RE before code** (the notes column says so): `SetMovieDescriptionData` (a CX2State subtitle: port
  31ef7370's two members and the 31f179b0 renderer at its callers 324bb230 / 324cff10, not just the setter), `IsActiveMonster`
  (March already has `CX2GUNPC::IsActiveMonster`: check June's writers of +0x872c against March's `m_bIsActiveMonster`, then it is
  bind-only), `SetImmuneToExtraDamage` and `SetCameraDistance` (find June's reader of the member first; a setter with no consumer is a
  partial port), `ChangeMotion_LUA`, `LiveKeyCodeNPCNumExceptDying`, `GetWorldObjectMeshByName` (read the whole body).
- **vcall thunks** (`CanPassThisStage` 0x210, `GetHenirLimitLevel` 0x214, `GetLiveUserUnitRandom` 0xbc, `GetLastTouchLineIndex`
  0x12c): resolve the slot in CX2Game `3348bf94` and in CX2DungeonGame `334bb094` / CX2PVEGame `334c1f34` (users: the GUUser vtables).
  NPC scripts see a `CX2Game*`, so a dungeon override is a March `virtual` override, as `IsVotingLuto` did.
- **Batch 3** (`untyped-gap` rows): June also binds `GetIsRight` / `GetRotateDegree` on CX2GameUnit, `SetIsRight` on CX2GUUser and
  `CannotInput` on CX2DungeonGame. Check what types reach those receivers, then bind June's class.
- **Data:** add the 8 June-only summon ids C3z-4 found (Done note, 'Summon IDs March lacks') to data036 `Enum.lua` and data037
  `NPCTemplet.lua` by the C3z-2 recipe (June values, verbatim rows, mesh census), then pack and copy (ask before the copy).
- Not this phase: the `system` rows (tutorial guide, SimpleDLG: their own STATUS rows) and `june-bug` rows (JUNE_BUGS #169).

Gate: `python june2017_re/c3z4_class_census.py` lists only `system` and `june-bug` rows (and `untyped-gap` rows whose receiver is
shown to be bound). A play-test in Ranox Gateway / FinalHall, Elysion Sky Crossroads and Hamel Frozen Water Hall shows no
`can't find` line for a ported name in ErrorLog.
