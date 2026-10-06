# June effect completion - C6 step 2 - port the five tail callees

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C6b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C6a (done 2026-10-06), C5 (`31f64c00` needs C5's `ADDITIONAL_ATTACK_TRIGGER_TYPE` list member) |
| Flag | `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C6b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6 - Spawner"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 10c"
```

Also read: `june2017_re/funcmap.tsv`, the rows tagged `Effect C6a` (`grep "Effect C6a" june2017_re/funcmap.tsv`).
The "C6a findings" section at the end of this file supersedes the rebuild plan's Phase 10c text where
the two disagree.

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**This session does step 2 only**, from C6a's verdicts. Under the no-partial-June-updates rule it also corrects March's existing hit and die spawners.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C6 (= rebuild plan Phase 10c steps 1-2). Read the rebuild plan's
Starting context and Phase 10c, JUNE2017_EFFECT_COMPLETION_PHASES.md (C6), funcmap.tsv. Step 1 on
Opus: identify the CEffect flags, entry fields and invoke-rate gate Phase 10c lists, verdicts into
funcmap.tsv. Step 2 on Sonnet: port the five tail callees as CEffect methods, call them from the
hit and die spawners under SERV_IRUHADEV_JUNE_SPAWN_INHERIT. Never port an offset. CP949
byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

## C6a findings (2026-10-06; rows tagged `Effect C6a` in `june2017_re/funcmap.tsv`)

These supersede the rebuild plan's Phase 10c text where they differ.

- **Who is `this`.** `31f8fdb0` and `31f64c00` run on the parent with the child as the argument. `31f649d0`, `31f65e40` and `31f65f90` run on the child: the rate is the spawning entry's `+0xb0`, and the bool is the parent's `m_bIgnoreDefence` / `m_bIgnoreStateDefence`. `31f65e40` and `31f65f90` run only when the entry's `+0xb4` / `+0xb5` is set.
- **Tail order** (row `31f810c0 @31f81984-31f81b35`):
  - In the hit and die spawners: CreateInstance, `31f8af50`, `31f8fdb0`, the existing blocks (INHERIT_BUFF_FACTOR ... max life from entry `+0x84`, then the `m_eFirstAttack`/`m_RelateSkillData` copy), then `31f649d0`, `31f64c00`, `31f65e40`, `31f65f90`.
  - Keep that order. Max life from `+0x84` runs after `31f8fdb0`, so it overwrites the `b5c` life-rate multiply.
- **Call sites: five, not two.**
  - The hit spawner (`X2DamageEffect.cpp` ~:6910) and the die spawner (~:7266).
  - The `CREATE_DAMAGE_EFFECT` loop in OnFrameMove (~:2880). June calls all five callees there: `31f8fdb0 @31f89d8b`, then `31f64c00 @31f89dbc` right after INHERIT_BUFF_FACTOR, and the other three at the end.
  - `RT_CREATE_DAMAGE_EFFECT` in AttackResult calls `31f8fdb0` only (`@31f86140`).
  - Touch-land is C7's, and it reuses these methods.
  - The no-partial rule covers the loop and RT_CREATE.
  - 7d inlined `31f8fdb0`'s INHERIT_DAMAGE branch in the loop (`:2891`). Replace that copy with the method under the new flag, keeping the old code in `#else`; do not apply it twice.
- **March names (never port an offset):**
  - Already in March, no new member needed:
    - `+0xb00` = `m_iIndex`
    - `+0xb04` = `m_iType`, not copied by `31f8fdb0`
    - `+0xb66` = `m_bInheritDamageFromMaster`
    - `+0xb32` = `m_iApplyEffectAlphaConditionByOption` (int in March, byte in June)
    - DD `+0x528/+0x529/+0x541` = `m_bIgnoreDefence` / `m_bIgnoreDefenceOnPvp` / `m_bIgnoreStateDefence`
    - DD `+0x40` = the INHERIT_BUFF_FACTOR vector
    - DD `+0x53c` = `m_eDamageTrigger`
    - CEffect `+0xd8` = `m_pDamageDataNext` plus `m_vecDamageDataNextChain`
  - New, with proposed names:
    - `+0xb44` `m_bInheritIgnoreStateDefence`
    - `+0xb45` `m_bInheritCountAsOwnerSkillHit`, with DamageData `+0x5fd` `m_bCountAsOwnerSkillHit`
    - `+0xb46` `m_bInheritBuffFactorToChild`
    - `+0xb47` `m_bInheritIgnoreDefence`
    - `+0xb5c`/`+0xb60` `m_bInheritMainEffectLifeTimeRate` / `m_fMainEffectLifeTimeRate`
    - `+0xb64` `m_bHyper`
    - `+0xb65` `m_bInheritAttackTriggerType`
    - DamageEffectData `+0xb0` `fInheritDamageRate` (default 1.0), `+0xb4`/`+0xb5` `bInheritIgnoreDefence` / `bInheritIgnoreStateDefence`
  - Ctor defaults: 0 / false.
- **Dead in the hit and die spawners.** The child's own `+0xb66` is set only by the CREATE loop, so `31f8fdb0`'s INHERIT_DAMAGE branch never fires there. That matches June.
- **Already in March, nothing to port:**
  - `+0x7a4`/`+0x7a8` are `m_eFirstAttack` / `m_RelateSkillData`.
  - The invoke-rate gate: June gates on `m_iType != DET_ARROW_OF_EXPLOSION` exactly as March does, with the same `-1` default.
- **June quirks to keep verbatim** (each gets a `JUNE_BUGS.md` row):
  1. `31f8fdb0`'s `b47` branch copies `m_bIgnoreDefence` but not `m_bIgnoreDefenceOnPvp`.
  2. `31f65e40` and `31f65f90` mark the entries 1 even when the value passed is false, so a child can lose its own Lua IGNORE_DEFENCE.
  3. `31f649d0`, `31f65e40` and `31f65f90` do not null-test the DAMAGE_DATA_NEXT entries of June's `+0xd8` vector. That vector simply has no element when an effect has no NEXT table, but March's `m_DamageEffectDataInLua.m_pDamageDataNext` is a pointer that is NULL then (`SAFE_DELETE` at X2DamageEffect.cpp ~:2033). So in March: apply to `m_pDamageDataNext` only when it is non-null, then to each `m_vecDamageDataNextChain` entry with no test. The quirk to record is only the untested chain entries. Without the `m_pDamageDataNext` check, the first ported Eve or Add skill crashes on every effect that has no NEXT table.
- **`31f64c00` needs C5.**
  - It copies values 29/30 (`DTT_RCR_TRANSCENDENCE_*`, June-only, Rose) of the parent's `ADDITIONAL_ATTACK_TRIGGER_TYPE` list, unconditionally. Then, under `b65`, it copies `m_eDamageTrigger` and the whole list.
  - The list member is C5's, and so is any June DTT enumerator. `31f64c00` itself belongs to C6b (C5's kickoff says so). Run C6b after C5. For 29/30, use C5's enumerators if C5 added them; otherwise use literals with the June Enum.lua names in a comment, since March's enum ends at 6.
- **`+0xb64` writer (EffectSet).**
  - `OnFrameMove_CreateEffect @321ef6d5-321ef6d8` sets the damage effect's `m_bHyper` from `EffectSetInstance::m_bHyper`.
  - It sits right after the `USE_IS_RIGHT_WHEN_CREATE` block that Phase 4a ported (`X2EffectSet.cpp` ~:1088). Port it with the copy, so `X2EffectSet.cpp` is in Touches.
- **The rest of that EffectSet block is out of scope for C6b. Ask the user; do not port it.** Three parts:
  - `@321ef639-321ef66d`: `m_iIndex` and an unidentified `+0x838` from instance `+0x1b8`, which unit vslot `0x248` stamps (base no-op; Luciel and Ain override it). It also writes an unidentified `+0x7e0`.
  - `321ee480` x2: not identified.
  - The runtime modifier list `321db1b0` (instance `+0x170`): only class code fills it.
- **Expected play-test.**
  - No March code sets any gate flag (all setters are Add, Eve, Rose, Elsword and Ain class code: see the checklist section 7 addendum), and every entry's rate is 1.0. So the play-test can only show that the tail runs and changes nothing.
  - **The one check that can fail:** `m_iIndex` now reaches hit, CREATE and RT_CREATE children, and Rena Night Watcher looks up her effects by index (`CheckDamageEffectType`, arrow detonation). Play a Night Watcher with Arrow of Explosion, Delayed Firing and Explosion Trap, the skills whose effects have hit children. Look for detonations behaving as before, and nothing triggering twice. That is June parity, but it is the one place a regression would show.
  - Two things do change, in both cases the same as June:
    - Hit and CREATE children now inherit the parent's `m_iIndex`, as die children already did.
    - The `m_bHyper` flag is carried down.
  - The `JUNE_FX` line should print the entry rate, the four gate flags and the index per spawn, throttled.
