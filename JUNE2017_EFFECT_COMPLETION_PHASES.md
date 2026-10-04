# June2017 effect engine: completion phases (adoption to full parity)

`JUNE2017_EFFECT_REBUILD_PLAN.md` (Phases 0-11) ported the parsers and most of the runtime, then
adopted June's own effect files on 2026-10-03. This document takes the engine from *adopted* to
**complete**: on June's own effect data, the March client does what the June client does, cited to
June's binary, with nothing left on a default that June would not use.

> **2026-10-04: this plan runs on the new workflow.** Start any phase with
> `/x2-phase june-effect <phase>` in a fresh conversation, on the model `STATUS.md` names. The
> maintained kickoffs are `kickoffs/june-effect/*.md`. C1, C6, C8 and C10 are split there into an
> Opus half (`a`, identification) and a Sonnet half (`b`, code), each `b` followed by an Opus review.
> Phase state lives in `STATUS.md` (block `june-effect`). The inline kickoff prompts below are
> frozen history. Build, deploy, hygiene and log steps use `tools/` (CLAUDE.md, *Workflow automation*).
> Write each phase's Done note under its section here, at most ~60 lines.

Before 2026-10-04, each phase ran in a fresh conversation:
1. paste the rebuild plan's **Starting context**;
2. paste the phase section below;
3. paste the phase's **Kickoff prompt**.

The rebuild plan's rules all still apply:
- cite the June VA;
- re-express, never port an offset;
- a key needs two sources;
- one `SERV_IRUHADEV_JUNE_*` flag per logical change;
- CP949 byte-level edits;
- stop at the wire;
- ask before launching.

Its model and effort rules apply too: identification on Opus high, transcription on Sonnet 5 high,
then an Opus `/code-review`. The **trusted source is the June binary in Ghidra**, as before.

Branch: the current working branch (`mods/june-buff-port` as of 2026-10-04).

---

## Definition of complete

The effect engine is complete when **all** of the following hold. Each item names how it is
checked.

1. **No June effect deals less than June intends.** Every `DAMAGE_DATA*` block in June's effect
   files resolves its damage the way June does, including the numerical-skill-data override.
   Checked by a new `census.py` check over `assets_cache/june_fx.json`: 0 blocks that get damage in
   June and none in March.
2. **0 `to-implement` rows** in `census.py`, except rows the census marks `handed-off` (new status,
   C0) with the receiving port's document and phase named.
3. **Every `binary-only` row has both a parse and a consumer** in March, cited. A new census check
   covers this: a binary-only row passes only with a March parse site.
4. **Every `unsupported` row is either built, or handed off, or declined by the user**, each one
   recorded in C0's decision table. "Unsupported" must mean a system the user chose not to have, not
   one nobody built.
5. **Runtime parity on the shared paths:**
   - June's collision is used on all five hit paths (`EffectToUnit`, `UnitToUnit`, `UnitToEffect`,
     `MeshToUnit`, `CollisionDataToUnit`);
   - every `SetDamageExceptionProcess` override is compared and ported;
   - the spawner post-create tail and the touch-land runtime are ported (old Phase 10c).
6. **Every runtime branch is observed on June data**, in a `JUNE_FX` line, or recorded as
   unreachable with the data search that proves it.
7. **The June effect files March still does not open are opened, or declined:**
   `Elsword_DamageData.lua`, `UnitViewerEffectSetTable.lua`, `EffectSet_Description.lua`.
8. **Docs closed** (old Phase 12): the rebuild plan's Status summary reads "complete", `MODS.md`
   lists every flag with its revert, memory is updated, and the user has decided on
   `SERV_IRUHADEV_JUNE_FX_DEBUG`.

Not in this definition, handed to their owners (C0 records each hand-off):
- **Character code.** The 15 C++ call sites whose effect names June changed, and the 14 names
  whose character or NPC scripts June changed. These belong to the character ports
  (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`): June changed the *skill* code, not the effect engine.
- **Buffs.** The 4 nested `BUFF_FACTOR/*` rows and the June buff factors effects request
  (`BFI 512`, `653`, `1129`, ...) belong to the buff port (`JUNE_BUFF_PORT_PHASES.md`).
- **Rose.** `ROSE_WEAPON_ATTACK_TYPE` waits for a Rose character port. Parsing and logging it is
  parity until then.

---

## Where things stand (census 2026-10-04)

`python june2017_re/census.py --summary`:
- implemented 596;
- to-implement 46: 31 in 8b, 15 unassigned;
- unsupported 14;
- binary-only 17;
- june-ignores 305.

| group | rows | phase here |
|---|---|---|
| numerical skill data + applied skill ID (`NUMERICAL_DATA_INDEX/NAME`, `APPLIED_SKILL_ID` x4, `DAMAGE_SKILL_ID`) | 7 | **C1** |
| 8b "everything else" (13 keys) + `DAMAGE_DATA_KEY_NAME` + 8a's mana no-display reader | 15 | C3 |
| 8b damage math (ignore-defence x4, division x4, `CAN_ACTIVATE_NASOD_ARMOR_COMBO`) | 9 | C4 |
| 8b `ParsingBuffFactorID` family, `DAMAGE_TRIGGER_DATA` family, `TRACE_NEAREST_BONE`, group-5 binary-only | 20 | C5 |
| `InsertDamageData` (9 keys, `Elsword_DamageData.lua`), `ES.Description` (3 keys) | 12 | C9 |
| nested `BUFF_FACTOR/*` (4) | 4 | hand-off: buff port |
| effect-alpha option (4), HP-rate damage (2), combination point (1), natural force (1) | 8 | C10, per C0 |
| `ROSE_WEAPON_ATTACK_TYPE` | 1 | hand-off: Rose |
| 6a/6b binary-only (`DAMAGE_DATA_NEXT_ULTIMATE%d`, `AT_RAID_FIELD`, `HIT_EFFECT_SET%d`; already parsed) | 3 | C11 (observe) |

---

## C0 - Decisions and census plumbing (user + Sonnet, no game code)

**Status: not started.**

**Decisions for the user.** Each row needs one answer before the phase that depends on it.

| # | question | recommendation | blocks |
|---|---|---|---|
| D1 | **Answered 2026-10-04: rolled back.** The 48 live koms were restored from `kom_backup_pre_june_fx/` (hash-checked) and the 12 new koms removed; `data036.kom` again carries the merged `EffectSet.lua`/`DamageEffect.lua`. The June `dataXXX_packed.kom` files are kept. **The `kom_contents` staging was reverted the same day** (`stage_june_fx.py --revert`), so a buff-port pack of `data036` cannot bring June's effects back. Re-adopt after C1 with `--apply`, then repack. **Order (user, 2026-10-04): finish the buff port first, then this plan.** Original question: **Live data, now.** June's effect files are live and 1,915 damage blocks deal 0 damage (see C1). Roll the live koms back to the pre-adoption backup until C1 lands, or keep June's data live? | roll back: `kom_backup_pre_june_fx/` restore, 12 new koms removed. Re-adopt after C1, by re-packing from `kom_contents` (staging untouched) | everything you play-test meanwhile |
| D2 | Effect-alpha option (`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, 4 rows): build June's per-owner effect-transparency game option (UI + save), or decline? | build: small, and June data uses it 29 times | C10 |
| D3 | Natural force: March has Rena's natural-force code under `ADD_RENA_SYSTEM` (off). Turn it on (character territory: a Rena system), or hand it to the Rena character port? | hand off to the Rena port; turning on a whole character system is not an effect-engine change | C10 |
| D4 | HP-rate damage (`HP_RATE_DAMAGE_TYPE/VALUE`, 8 uses): build here? | build: self-contained in the damage path | C10 |
| D5 | Combination point (`HIT_ADD_COMBINATION_POINT`, 15 uses): identify its owner first, then decide | identify in C2 | C10 |
| D6 | `SERV_IRUHADEV_JUNE_FX_DEBUG`: keep it through C11, then remove? | keep until C11 passes | C12 |

**Census plumbing (Sonnet):**
- add the status `handed-off`, with `receiver` = document + phase, for the hand-off rows above;
- add the check "no June damage block gets damage only from a system March lacks" (item 1 of the
  definition);
- add the check "every binary-only row has a March parse site" (item 3).

**Exit:** the decisions are recorded in this table, and `census.py` runs all-PASS except the new
item-1 check, which fails until C1.

**Kickoff prompt:**
```
June2017 effect completion C0. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (Definition of
complete, C0) and the rebuild plan's Status summary. Ask me D1-D6 one by one, record my answers
in C0's table. Then extend june2017_re/census.py: a 'handed-off' status (receiver = doc+phase)
for the hand-off rows; a check that no June DAMAGE_DATA* block (assets_cache/june_fx.json) gets
damage only from a system March lacks; a check that every binary-only row has a March parse
site. Rerun, commit with my OK. No game code.
```

---

## C1 - Numerical skill data: June's damage override (BLOCKER)

**Status: not started. This is a live regression while June's data is live.**

**Why first.** June's `SetDamageDataFromLUA` overrides PHYSIC/MAGIC (x0.01), PVP_RATE, TECH_POINT
and HIT_ADD_MP from a per-skill numerical table (`FUN_32632390 @3263fe41-32640054`). It does this
when `NUMERICAL_DATA_INDEX > 0` or `NUMERICAL_DATA_NAME` is set, **and** `APPLIED_SKILL_ID` or
`DAMAGE_SKILL_ID` is set (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1). March parses and logs
the keys only (Phase 8c). Measured 2026-10-04 on June's live effect files:
- 2,095 `DAMAGE_DATA*` blocks set numerical data, and 2,085 of them also set a skill ID, so June's
  override runs.
- **1,915 have no non-zero `DAMAGE`, so in March they deal 0 damage.** By file: `DamageEffect` 1,510,
  `_Character` 394, `_Dungeon` 9, `_INT` 2.
- **492 of those did damage in the retired merged March copy**, e.g. `AIMING_EXPLOSION` (MAGIC
  5.63), `ANGER_OF_ELF_SPARK` (3.55), `CHUNG_ACELDAMA_SHELL` (PHYSIC 2.33),
  `ARA_SHADOW_RISING_IMPACT` (8). The other 1,391 are June-only templets.

**Step 1: identify (Opus, high, no code). Write each verdict into `funcmap.tsv`.**
- The override's callees: `32890620`, `31fca8e0`, `320717e0` (by index), `32078060` (by name).
- The table: the lead is June's `NewSkillTempletVer2.lua`, the only June script with
  `NUMERICAL_DATA1` ... `NUMERICAL_DATA12`. Confirm it is loaded into what the callees read, and
  how a skill's level picks the row.
- The `APPLIED_SKILL_ID` readers this depends on: the `_ULTIMATE` gate in `ParsingBuffFactorID`
  (`31fcf3e0`). Identify it here only as far as the override needs; the rest is C5.
- Compare March's `NewSkillTempletVer2.lua` with June's. If adopting June's file changes skills
  beyond numerical data, record the difference and **stop for the user**: the skill templet
  belongs to the character ports as much as to effects.

**Step 2: transcribe (Sonnet, high).** Under a new flag, `SERV_IRUHADEV_JUNE_NUMERICAL_DATA`:
- the table loader, the shipped way (`LoadDataFile` + `DoMemory`, with a degrade path that logs the
  missing file);
- the override in `SetDamageDataFromLUA`;
- `APPLIED_SKILL_ID` and `DAMAGE_SKILL_ID` stored where the override needs them. `DAMAGE_SKILL_ID`
  moves out of 8b here.

Every value this changes lands in `MODS.md`. The user packs any `.lua` this needs.

**Exit.**
- The C0 item-1 check passes: 0 blocks that get damage in June and none in March.
- A play-test with damage numbers on 3 of the 492, e.g. Rena `ANGER_OF_ELF_SPARK`, Chung
  `CHUNG_ACELDAMA_SHELL`, Aisha `AISHA_ELEMENTAL_MASTER_CYCLONE_OUTSIDE`, compared against the March
  value. June's balance may differ, and that is expected: record both.

**Kickoff prompt:**
```
June2017 effect completion C1 (numerical skill data). Read JUNE2017_EFFECT_COMPLETION_PHASES.md
(C1), JUNE2017_CHARACTER_PORT_CHECKLIST.md section 1, the rebuild plan's Starting context and
Phase 8c, june2017_re/README.md and funcmap.tsv. Branch: current.

Step 1 on Opus: identify FUN_32632390 @3263fe41-32640054's callees (32890620, 31fca8e0,
320717e0, 32078060), the table they read (lead: June NewSkillTempletVer2.lua NUMERICAL_DATA1-12)
and how skill level selects a row; write verdicts to funcmap.tsv; diff March vs June
NewSkillTempletVer2.lua and stop for me if adopting June's changes more than numerical data.
Step 2 (after /model to Sonnet): transcribe the loader and the override under
SERV_IRUHADEV_JUNE_NUMERICAL_DATA, store APPLIED_SKILL_ID/DAMAGE_SKILL_ID where it needs them.
CP949 byte-level edits; build US_SERVICE, deploy, ASK before launching. Done when census.py's
damage-regression check passes and three of the 492 named skills show damage in play.
```

---

## C2 - Identification sweep (Opus, high, no code)

**Status: not started.** Every function the rest of 8b, Phase 9 and C9 waits on, identified in one
session so that C3-C9 are pure transcription. Write each verdict into `funcmap.tsv` (`confirmed`
with evidence, or `candidate` with what is missing).

| function / question | needed by | lead |
|---|---|---|
| June counterpart of `CX2GameUnit::GetFinalDamage` | C4 (ignore-defence, division) | not in `funcmap.tsv`; find it from `EffectToUnit` `3262b6e0`'s damage call |
| `FUN_3261a150` (`DamageData +0x5b0`, divide-from-range) | C4 | 8a handoff row 14 |
| `FUN_31fcf3e0`, `FUN_3227baf0` (`ParsingBuffFactorID` family) | C5 | rebuild plan 8b group 2; do not assume the memo path by resemblance |
| June `Process_ExtraDamage` (the mana no-display reader) | C3 | by call graph from a `confirmed` caller |
| `FUN_32622220` (`DAMAGE_DATA_KEY_NAME` registration) and its reader | C3 | `@3263edee` |
| `FUN_321da350`'s helpers `32278580`, `31c1c1a0`, `31f54910` (`TRACE_NEAREST_BONE`) and their relation to `TargetMeshData` | C5 | rebuild plan 8b |
| the consumer of `DAMAGE_TRIGGER_DATA` and `ADDITIONAL_ATTACK_TRIGGER_TYPE` (`FUN_3221db80` clear, `FUN_31f8b860` push) | C5 | 8a handoff rows 33-34 |
| the owner of `HIT_ADD_COMBINATION_POINT` (DD `+0x594`) | D5 / C10 | xrefs on `+0x594` reads |
| `InsertDamageData_LUA` `32621080` and the reader of what it stores | C9 | bind table `FUN_31d95080` |
| `ES.Description` readers `32378e20`, `32680da0`; who consumes `UnitViewerEffectSetTable.lua` | C9 | |
| the eight `SetDamageExceptionProcess` `candidate` rows | C8 | `funcmap.tsv`, slot `0x2b8` |

**Exit:** every row above is `confirmed` in `funcmap.tsv`, or `candidate` with a written reason, and
C3-C10 can name their June sources without guessing.

**Kickoff prompt:**
```
June2017 effect completion C2: identification only, no code. Read
JUNE2017_EFFECT_COMPLETION_PHASES.md (C2), the rebuild plan's Starting context, Investigation
ledger and Phase 8b resume point, june2017_re/README.md and funcmap.tsv. Ghidra must be up
(health check in Starting context; stop and ask if not). Work down C2's table; write every
verdict into funcmap.tsv with evidence (call graph, decrypted strings, member writes) -- never
by resemblance. Ask before any Ghidra mutation. Commit funcmap.tsv + README notes with my OK.
```

---

## C3 - 8b, straightforward keys

**Status: not started. Model:** Sonnet 5, high, then an Opus review.

- **Keys:** `INIT_EXTRA_DAMAGE_BEFORE_LOAD`, `ADD_MEMBER_COUNT`/`_DISTANCE`,
  `BOUNDING_BACK_SPEED_X/Y`, `FORCE_BACK_SPEED`, `ADD_CHECK_DAMAGE_UNIT_NUM`, `EXCEPT_ME_HP_UP`,
  `ATTACK_ONLY_ME`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `DAMAGE_LIGHT_TIME`, `IS_AURA_BUFF`,
  `IGNORE_ATTACK_SUCCESS`, `ONYL_ATTACK_USER` (studio typo, keep), `PAUSE_NPC_AI_TIME`.
- **Also:** `DAMAGE_DATA_KEY_NAME`'s registration and reader, and 8a's carried-over
  `EDT_MANA_DAMAGE` no-display reader. Both are from C2.
- **Binary-only:** `HIT_ADD_MP_PERCENT`, `DAMAGE_RATE_TO_SUPERAMOR` (used by June's `Luciel_Duo`),
  `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `APPLIED_MEMO_ID`.
- Extend `SERV_IRUHADEV_JUNE_DAMAGEDATA`. For each key: June's parse (`FUN_32632390` site, from the
  census) **and** its consumer (xrefs on the member write inside a `confirmed` function), both cited.
- **Split rule:** if the session runs long, commit and write the resume point here.

**Exit:** these rows leave `to-implement`; the binary-only ones have a parse and a consumer; a
regression play-test (dev-console T4 shape) passes.

**Kickoff prompt:**
```
June2017 effect completion C3. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C3), the rebuild plan's
Starting context and Phase 8b (the grouping and play-test checklist), funcmap.tsv (C2's verdicts)
and the matrix. Transcribe each C3 key's June parse and June consumer, both cited, extending
SERV_IRUHADEV_JUNE_DAMAGEDATA. A consumer in a function not yet 'confirmed' is identification:
stop and flag it. CP949 byte-level edits; build, deploy, ASK before launching; update matrix
(rerun census.py) and MODS.md.
```

---

## C4 - 8b, damage math

**Status: not started. Model:** Sonnet 5, high, then an Opus review. Needs C2's `GetFinalDamage` and
`3261a150`.

- **Keys:** `IGNORE_DEFENCE_ON_PVP`, `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP`,
  `IGNORE_DEFENCE_DECLINE`, `DIVISION_NUM`, `DIVISION_OFF_TIME`, `DIVIDE_DAMAGE_FROM_RANGE`/`RANGE%d`,
  `CAN_ACTIVATE_NASOD_ARMOR_COMBO` (its system exists: `X2GUAdd`).
- **These move damage numbers.** Put them under their own flag,
  `SERV_IRUHADEV_JUNE_DAMAGE_MATH`, so they can be reverted without touching parse work. List
  every formula change in `MODS.md`.

**Exit:** the rows leave `to-implement`. A play-test compares PvE and PvP damage numbers on one
ignore-defence skill and one division skill, before and after.

**Kickoff prompt:**
```
June2017 effect completion C4 (damage math). Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C4), the
rebuild plan's Starting context and Phase 8b group 1, funcmap.tsv (GetFinalDamage and 3261a150
from C2). Transcribe parse + consumer for each C4 key under a new SERV_IRUHADEV_JUNE_DAMAGE_MATH
flag, every formula cited. CP949 byte-level edits; build, deploy, ASK before launching; record
before/after damage numbers; update matrix and MODS.md.
```

---

## C5 - 8b, buff-factor selection, damage triggers, nearest bone

**Status: not started. Model:** Sonnet 5, high, then an Opus review. Needs C2. **Coordinate with the
buff port**, which owns the buff factors these keys select: read `JUNE_BUFF_PORT_PHASES.md` first and
do not duplicate its work.

- **`ParsingBuffFactorID` family:** `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, `_RELATIVE_SKILL_ID`,
  `_RELATIVE_BUFF_TEMPLATE_ID` (binary-only, used by June's `Elsword_SwordMan`),
  `BUFF_FACTOR_ID_ULTIMATE`, `_ULTIMATE_PVP` (binary-only), `BUFF_FACTOR_ID_PVP`.
- **`DAMAGE_TRIGGER_DATA` family:** the sub-table and its `ATTACK_TRIGGER_TYPE`/`BUFF_TEMPLET_ID`/
  `POWER_RATE`/`POWER_RATE_ACCUMULATION_MULTIPLIER`; `ADDITIONAL_ATTACK_TRIGGER_TYPE`; binary-only
  `DAMAGE_TRIGGER_CONDITION`.
- **`TRACE_NEAREST_BONE`:** parsed only today; the consumer is `321da350`.

**Exit:** 8b has 0 `to-implement` and 0 unconsumed binary-only rows; the rebuild plan's Phase 8b is
marked done.

**Kickoff prompt:**
```
June2017 effect completion C5. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C5), JUNE_BUFF_PORT_PHASES.md
(what the buff port owns), the rebuild plan's Starting context and Phase 8b groups 2/3/5, funcmap.tsv.
Transcribe parse + consumer for the ParsingBuffFactorID family, the DAMAGE_TRIGGER_DATA family and
TRACE_NEAREST_BONE, extending SERV_IRUHADEV_JUNE_DAMAGEDATA; leave anything the buff port owns to it
and say where. CP949 byte-level; build, deploy, ASK before launching; mark 8b done in the rebuild
plan when census shows 0 open 8b rows; update matrix and MODS.md.
```

---

## C6 - Spawner post-create tail (old Phase 10c, steps 1-2)

**Status: identification started 2026-09-29, no code.** The full spec is the rebuild plan's Phase
10c section: the five callees, the CEffect flags to identify, the entry fields, and the invoke-rate
gate. Under the "no partial June updates" rule, this corrects March's existing hit and die spawners
as well. **Model:** Opus, high, for step 1; then Sonnet 5, high.

**Exit:** `31f8fdb0`, `31f649d0`, `31f64c00`, `31f65e40`, `31f65f90` are ported as CEffect methods
and called from the hit and die spawners, under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`, with a
`JUNE_FX` line. A play-test on a skill with hit/die child effects shows no regression.

**Kickoff prompt:**
```
June2017 effect completion C6 (= rebuild plan Phase 10c steps 1-2). Read the rebuild plan's
Starting context and Phase 10c, JUNE2017_EFFECT_COMPLETION_PHASES.md (C6), funcmap.tsv. Step 1 on
Opus: identify the CEffect flags, entry fields and invoke-rate gate Phase 10c lists, verdicts into
funcmap.tsv. Step 2 on Sonnet: port the five tail callees as CEffect methods, call them from the
hit and die spawners under SERV_IRUHADEV_JUNE_SPAWN_INHERIT. Never port an offset. CP949
byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

---

## C7 - Touch-land runtime (old Phase 10c, step 3)

**Status: not started (an earlier attempt was reverted for porting June offsets).** The spec is the
rebuild plan's Phase 10c step 3: the gate `31f83380`, `m_vVelocityBefore` on `CXMeshInstance`, and
the spawner `31f81ba0` (read in full in `ghidra/dis_31f81ba0.txt`). It needs C6's shared tail.
**Model:** Sonnet 5, high, then an Opus review.

**Exit:** `SERV_IRUHADEV_JUNE_TOUCH_LAND`; the census's `TOUCH_LAND_DAMAGE_EFFECT` runtime rows read
`done`; a throttled `JUNE_FX TOUCH_LAND` line fires in play on a June damage effect that uses it.

**Kickoff prompt:**
```
June2017 effect completion C7 (= Phase 10c step 3). Read the rebuild plan's Starting context and
Phase 10c step 3, Phase 7e's note on the reverted first attempt, funcmap.tsv, and
june2017_re/ghidra/dis_31f81ba0.txt. Port the gate and the spawner under
SERV_IRUHADEV_JUNE_TOUCH_LAND, using C6's tail; add m_vVelocityBefore to CXMeshInstance the March
way (member, ctor init, snapshot copy, getter) -- never read a June offset. CP949 byte-level;
build, deploy, ASK before launching; find a June templet with TOUCH_LAND_DAMAGE_EFFECT for the
play-test; update matrix and MODS.md.
```

---

## C8 - Hit path, second half (old Phase 9)

**Status: not started.** Phase 9 ported June's collision for `EffectToUnit` only.
**Model:** Opus, high, for the comparisons; Sonnet 5, high, for the edits.

- **Collision:** switch `UnitToUnit` (`32611970` inlines the same code), `UnitToEffect`,
  `MeshToUnit` and `CollisionDataToUnit` to June's semantics, under `SERV_IRUHADEV_JUNE_COLLISION`.
  Confirm each one's June counterpart first.
- **`SetDamageExceptionProcess` (`SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`):**
  - compare and port the eight per-class overrides in `funcmap.tsv`: Add, Elesis, Lire and Arme
    have March counterparts; Eve, Chung, Raven and Elsword are June-only;
  - port the base and user layers' June additions: the `AT_COMMAND` MP-gain multiplier, and the
    `APPLIED_SKILL_ID` hooks once C1 has the skill ID;
  - port the NPC override `32759f30`.
- **Settle T6's open question:** the far `TEST` samples (`MODS.md`, rebuild plan T6).

**Exit:**
- `MODS.md`'s "Known gap" entry is closed;
- every slot-`0x2b8` row is `confirmed` and ported, or recorded as identical to March;
- a melee and a projectile play-test show `JUNE_FX HIT` on each path.

**Kickoff prompt:**
```
June2017 effect completion C8. Read the rebuild plan's Starting context and Phase 9, MODS.md's
SERV_IRUHADEV_JUNE_COLLISION / _DAMAGE_EXCEPTION entries, funcmap.tsv slot-0x2b8 rows,
JUNE2017_EFFECT_COMPLETION_PHASES.md (C8). On Opus: confirm each remaining hit path's June
counterpart and compare the eight SetDamageExceptionProcess overrides. On Sonnet: port, citing
each. CP949 byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

---

## C9 - The June effect files March still does not open

**Status: not started. Model:** Sonnet 5, high (identification is from C2).

- **`Elsword_DamageData.lua`:** 107 `InsertDamageData_LUA` calls, 9 keys, all `unassigned` today.
  Bind `InsertDamageData_LUA` the way June does (`32621080`), store it the way June stores it,
  and port its reader from C2.
- **`UnitViewerEffectSetTable.lua`:** 18 calls. Bind and consume it per C2.
- **`EffectSet_Description.lua`:** `VERSION`/`DESC`/`NAME`. March reads it only under
  `EFFECT_TOOL`, but June's client reads it at `32378e20`/`32680da0`. Port those readers if they
  are game code; if they are tool or debug code, record that and decline.
- Stage any newly opened file with `stage_june_fx.py`, adding it to `LUA10`. **The user packs.**

**Exit:** the 12 `unassigned` `InsertDamageData`/`ES.Description` rows are implemented or declined
with a reason, and the three files appear in a `JUNE_FX` or `ES_LOAD` line.

**Kickoff prompt:**
```
June2017 effect completion C9. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C9), funcmap.tsv (C2's
verdicts for 32621080, 32378e20, 32680da0 and UnitViewerEffectSetTable's consumer), the rebuild
plan's Starting context. Bind and consume the three files June loads that March doesn't, citing
June; stage them via june2017_re/stage_june_fx.py and name every file I must pack. CP949
byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

---

## C10 - Decided absent systems

**Status: not started; scope set by C0's D2-D5.** **Model:** Opus, high, to identify; then Sonnet 5,
high.

- **D2 = build:** effect-alpha option. Port June's game option (the `AEACBO_*` enum is already in
  `Enum.lua`): the option storage, its UI, and the readers at the four parse sites.
- **D4 = build:** HP-rate damage, i.e. `HP_RATE_DAMAGE_TYPE`/`_VALUE` in June's damage path.
- **D5:** combination point, per C2's identification.
- **D3:** natural force. Only if the user chose to turn it on here.

Each gets its own flag. Rows the user declined become `declined` in the census, with the date.

**Exit:** the census has no `unsupported` row left except the handed-off and declined ones.

**Kickoff prompt:**
```
June2017 effect completion C10. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C0's decision table,
C10), the rebuild plan's Starting context and Phase 8c, funcmap.tsv. Build exactly the systems C0
says to build, each under its own SERV_IRUHADEV_JUNE_* flag, cited to June; mark declined ones in
census.py. CP949 byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

---

## C11 - Verification on June data

**Status: not started. Model:** Sonnet 5, high. A dev-console pass in the same shape as the rebuild
plan's T0-T7, against the live June data.

Observe in a `JUNE_FX` line, or prove unreachable with a data search:
- 7b's `LOT_UID_VECTOR` re-target (`UID_VECTOR_RETARGET`/`SEED_PICK`);
- 7c's die conditions;
- 4a/4b stage 2-3 keys on June data (`SYNC_SHOW_BY_CREATE_UNIT` playback, `SOUND%d`, mesh
  triggers, `TRACE_MATRIX_IN_FIELD`);
- Phase 3's `ES_EFFECT`;
- 7e's `HIT_EFFECT_SET`/`DIE_CREATE_NPC`;
- the 6a/6b binary-only rows (`DAMAGE_DATA_NEXT_ULTIMATE%d`, `AT_RAID_FIELD`, `HIT_EFFECT_SET%d`);
- every key C1-C10 added.

Also:
- fix `BOSSRAID_ABYSS_SMOG.DDS`: find the referencing file, then stage June's
  `BossRaid_Abyss_smog.dds`/`_smog2.dds`;
- a visual sample of the 178 changed particles.

**Exit:** each row has a log line or a written proof of unreachability, and `census.py` passes
every check, including C0's new ones.

**Kickoff prompt:**
```
June2017 effect completion C11 (verification). Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C11),
the rebuild plan's Dev-console verification pass (for the console toolkit) and the matrix. For
each C11 row, find a June templet that exercises it (search assets_cache/june_fx.json) and write
console steps for me; or prove no June data reaches it. ASK before launching. Afterwards read the
logs and record every result in C11 here. Fix BOSSRAID_ABYSS_SMOG via kom_contents staging.
```

---

## C12 - Close (old Phase 12)

**Status: not started. Model:** Sonnet 5, high. This is the rebuild plan's Phase 12 list, plus:
- rewrite the rebuild plan's Status summary to "complete", pointing here;
- remove `SERV_IRUHADEV_JUNE_FX_DEBUG` per D6;
- update memory.

**Exit:** every item of the Definition of complete is checked off below.

| item | evidence | done |
|---|---|---|
| 1 damage parity | census check | |
| 2 to-implement | census | |
| 3 binary-only consumed | census check | |
| 4 unsupported resolved | C0 table + census | |
| 5 runtime paths | C6, C7, C8 exits | |
| 6 branches observed | C11 | |
| 7 unopened files | C9 | |
| 8 docs | this phase | |

**Kickoff prompt:**
```
June2017 effect completion C12 (close). Read JUNE2017_EFFECT_COMPLETION_PHASES.md (Definition of
complete, C12), the rebuild plan's Phase 12, MODS.md. Do Phase 12's list, fill C12's checklist
with evidence, rewrite the rebuild plan's Status summary to 'complete', ask me before removing
SERV_IRUHADEV_JUNE_FX_DEBUG and before adding the CLAUDE.md pointer, update memory. Commit with
my OK.
```
