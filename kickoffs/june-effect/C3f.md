# June effect completion - C3f - identify June's force-down flush in StateChange (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3f` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3c (done). Its port phase must not run with another phase that edits `CX2GUUser::StateChange` / `CommonStateStart` (C9h-4f, C3d-2) |
| Flag | none (no code); the port phase gets its own flag |
| Log tag | - |
| Review | none (its port is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3c - Review (2026-10-08, Opus 5.5 high): correct against June; no code change; June's force-down flush sits in StateChange -> C3f" "C3f - identify: June's force-down flush in StateChange"
grep -n "(C3c review)\|^31feabd0 (C3c)\|^320348c0\|^32081520" june2017_re/funcmap.tsv
grep -n "^| 131 " JUNE_BUGS.md
```

Ghidra is read-only (`ghidra` skill). Verdicts go to `june2017_re/funcmap.tsv`, tagged `Effect C3f`. `june2017_re/vtslot.py <slot> CX2GU`
resolves a slot for every class.

## Task

The C3c review found that June moved the user's force-down flush.

- **June:** `CX2GUUser::StateChange` `320348c0 @32034931-32034994` calls the gage's UpdateFlushForceDown `32a33dc0` when the state being **left**
  (Future nowState `+0x7e6c`) is vslot `0x114` / `0x118` / `0x490` (base: `0x1e` DOWN_FRONT, `0x1f` DOWN_BACK, `0x25` AIR_DOWN_LANDING).
- **March:** `ForceDownAndDetonationProcess` (`X2GUUser.cpp:40216`, `FIX_FORCE_DOWN_AND_DETONATION_BUG`, ON) runs from CommonStateStart
  (`:16110`) on **entering** DOWN_FRONT / DOWN_BACK / AIR_DOWN_INVINCIBLE / RIDING_DAMAGE_*. It also runs UpdateDamageFlushMP and clears stun,
  freeze and groggy when either flush applied.

1. **The detonation half.** Find June's counterpart of March's `CX2GageData::UpdateDamageFlushMP` (`X2GageData.cpp:162`: the `GetActivateDetonation`
   test, `UpNowMp( -charge )`, `SetActivateDetonation( false, 0 )`). Find its callers, and where June clears `m_vecStunPtr`, `m_vecFreezePtr`,
   `m_ExtraDamagePack.m_Stun` and the groggy flag after a flush. If June has no such step, say so with the evidence (xrefs, a sweep).
2. **June's CommonStateStart.** Confirm that `32057b80` has no flush call. Its only callers of `32a33dc0` are `320348c0` and `327577d0`. Note
   anything else June runs there in place of March's `ForceDownAndDetonationProcess()`.
3. **The NPC twin** `327577d0` (states `+0x4090` / `+0x4094` / `+0x4098`) against March's NPC path (`X2GUNPC.cpp:13101` `FlushForceDown`, and
   any NPC flush on state change). Find which NPC states those are and where they are set. Also (found by C3i, funcmap
   `32769240 @3276a18d-3276a1f3`): June's NPC `DamageReactStateChange` skips its whole FlushForceDown branch while nowState is one of the
   same three states; March's branch has no such test. Plan its port with the rest.
4. **Overrides.** Luciel overrides all three getters (`32b4d810` / `32b4d850` / `32b4d9d0`): record what they return. Class material goes to
   `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (standing rule D8, no question).
5. **Plan the port** (follow June, no partial update): a phase row, a flag and a kickoff. The port moves the flush to StateChange under
   that flag, with March's CommonStateStart call in `#else`. It also corrects the C3c RT_DOWN comment in `X2GUUser.cpp` ("cleared only by
   ForceDownAndDetonationProcess / its flush"), names June's location, and gives a play-test spec. The spec needs a character + job from
   `els_db.sql` and a dungeon with enemies that knock down. Expect: the gage empties when the unit stands up or leaves AIR_DOWN_LANDING, not
   when the down state starts. Failure looks like: the force-down invincible fall never comes, or comes on every knock-down.

Write a Done note and update the STATUS row.
