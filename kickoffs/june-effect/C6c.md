# June effect completion - C6c - the effect-set buff list (321e3650 / 321ee480) and the SIEE 15 attach

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C6c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C6a2 (done 2026-10-07), C6b (both edit the same `OnFrameMove_CreateEffect` block; run after it). Never run alongside june-buff 16-step3b5 (both edit `X2GUUser.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`, `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL` (JUNE_BUGS 60 fix) (+ `SERV_IRUHADEV_JUNE_FX_DEBUG` for the log line) |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C6c-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6a2 - Done"
grep "Effect C6a2" june2017_re/funcmap.tsv
```

Also read:
- the funcmap rows `321ee480`, `321e3650`, `EffectSetInstance+0x160`, `321f5130`, `32d418a0`, `32d41960` and `32d40dd0`;
- JUNE_BUGS rows 59-61;
- March `X2Lib/X2GUUser.cpp`, `ProcessSocketEffectEx` case `SIEE_ALWAYS_PLAY_EFFECT_SET` (grep `321e3650`).

## Why this is its own phase

The buff port decided "follow June" for June's effect-set buff list (user, 2026-10-06), so June's socket auras work. C6a2
identified the whole of it and split it out of C6b for three reasons:
- it is a different logical change, so it gets its own flag;
- it is the only part with a data-driven caller (SIEE 15, in `X2GUUser.cpp`);
- its play-test is different.

## Task

Port June's effect-set buff list under `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`. Re-express each piece on March's classes and
never port an offset.

1. **The store.** Add `EffectSetInstance::m_vecBuffFactorList` (June `+0x160`), empty at construction.
   - Entry `BuffFactorListEntry` (June 0x18 B): `UINT m_uiBuffFactorID`, `int m_iLevel`, `vector<BuffFactorListMod> m_vecMod`.
   - Mod (June 16 B): `bool m_bFinalizer` (byte `+0`; 0 = behavior), `int m_iType` (`+4`), `int m_iOperation` (`+8`), `float m_fValue` (`+0xc`).
   - March's EffectSetInstance dtor frees it with the vector; June's dtor `321e3a80` does the same.
2. **The attach.** `CX2EffectSet::AttachBuffFactorToEffectSet( Handle, UINT uiBuffFactorID, int iLevel )` (June `321e3650`).
   - Find the instance the way March's `GetEffectSetInstance( Handle )` does. An invalid or stale handle returns silently, as June does.
   - Push `{BFI, level, empty mods}`.
   - Do not port `321e3730` / `321e3830` (Rose / Add only): they are checklist 7a's.
3. **The consumer.** `CX2EffectSet::ApplyBuffFactorListToDamageEffect( CEffect*, const vector<BuffFactorListEntry>& )` (June `321ee480`), per entry in order:
   1. `g_pData->GetBuffTempletManager()->GetBuffFactorPtr( BFI )` (June `326c32a0`).
      **June has no null test here** (JUNE_BUGS row 60). **User's answer (2026-10-07): (a), guard it.**
      Put the guard under a separate `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL`: skip the entry and log once per BFI with
      `[JUNE_FX]`. June's unguarded line goes in `#else`, so turning the fix flag off restores June exactly.
   2. `clone = factor->GetClonePtr( level )` (June `32089f60`).
   3. For each mod:
      - behavior: June `32d418a0`. Find the behavior of `m_iType` with at least 2 values; op 0 add, 1 multiply, 2 set on `value[1]`.
      - finalizer: June `32d41960` -> `32d40dd0`. Only for finalizers whose type is `m_iType` **and** BFT_TIME (1), and that have 2 or 3 values. For i = 1..n-1, op 0 add, 1 multiply, 2 set.
      - Port them as two small `CX2BuffFactor` methods over March's `BUFF_FACTOR::m_vecBuffBehaviorFactors` / `m_vecBuffFinalizerFactors` (`X2BuffFactor.h:744-745`).
      - No March data fills mods (only Rose and Add class code does), so this runs inert. Keep it anyway: the consumer must be whole.
   4. `pEffect->GetDamageData().PushBuffFactor( clone, true )` (June `321f5130` with bClear = 1).
      - **Per entry, inside the loop.** Each entry wipes the effect's own `m_vecBuffFactorPtr` and re-pushes the socket and temporary factors, so only the last entry survives (JUNE_BUGS row 59, verbatim).
      - March's PushBuffFactor clones once more; that is harmless.
      - June's `32089ff0` branch is a no-op with its `{1.0, 1.0}` rates (JUNE_BUGS row 61). Do not port it.
4. **The call.** In `OnFrameMove_CreateEffect`'s `ET_DAMAGE_EFFECT` case, inside `if( NULL != pEffect )`:
   - call the consumer with `pEffectSetInstance->m_vecBuffFactorList`, right after C6b's stamp block (June `@321ef67b-321ef689`);
   - do not add the EffectData `+0x118` call (`@321ef676`): that list is dead in June.
5. **The SIEE 15 attach** (`X2GUUser.cpp`, `ProcessSocketEffectEx` case `SIEE_ALWAYS_PLAY_EFFECT_SET`, under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`).
   - Replace the `unsupported (effect-set buff list, 321e3650)` log with `AttachBuffFactorToEffectSet( handle, m_uiBuffFactorID, m_iBuffLevel )` for every `m_vecConditionalBuff` entry with `m_bAlwaysPlayEffectSet` (June `@3202a1aa-3202a1f4`).
   - Nest it under this phase's flag, with the old log line in `#else`.
   - The flagcheck OTHER row for `X2GUUser.cpp` is expected; explain it.
6. **JUNE_FX** (under `SERV_IRUHADEV_JUNE_FX_DEBUG`, throttled): one line per attach (handle, BFI, level) and one per
   consumer run (entries, the BFI kept).

**Done gate** (write the note as `### C6c - Done` under C6; `C6c-review.md` reads that heading):
- flagcheck passes and the build deploys;
- the `[JUNE_FX]` attach line is in the deployed exe's strings;
- `MODS.md` is updated, plus the key matrix if it has a row for this (no census or matrix row tracked the effect-set buff list as of C6a2);
- funcmap rows `321ee480`, `321e3650` and `EffectSetInstance+0x160` say PORTED.

**Expected play-test:** a regression check only.
- No June set with a buff list exists in March data. The aura's set (`EffectSet_Hero_Dungeon_Aura_Buff`) is absent, so SIEE 15's set does not play and nothing attaches. No save carries Fever SIEE 13-20.
- So check that ordinary skills that create damage effects through effect sets still apply their own buffs and debuffs, for example a skill that applies a debuff on hit.
- The end-to-end aura check waits for three things: C11's re-adoption of June's effect files, june-buff Phase 13's BFIs, and a socket save edit at C11.
