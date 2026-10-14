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

**Status: done 2026-10-05 (decisions D1-D6 answered; census plumbing in `june2017_re/census_completion.py`, hooked into `census.py`).**

**Decisions for the user.** Each row needs one answer before the phase that depends on it.

| # | question | recommendation | blocks |
|---|---|---|---|
| D1 | **Answered 2026-10-04: rolled back.** The 48 live koms were restored from `kom_backup_pre_june_fx/` (hash-checked) and the 12 new koms removed; `data036.kom` again carries the merged `EffectSet.lua`/`DamageEffect.lua`. The June `dataXXX_packed.kom` files are kept. **The `kom_contents` staging was reverted the same day** (`stage_june_fx.py --revert`), so a buff-port pack of `data036` cannot bring June's effects back. Re-adopt after C1 with `--apply`, then repack. **Order (user, 2026-10-04): finish the buff port first, then this plan.** Original question: **Live data, now.** June's effect files are live and 1,915 damage blocks deal 0 damage (see C1). Roll the live koms back to the pre-adoption backup until C1 lands, or keep June's data live? | roll back: `kom_backup_pre_june_fx/` restore, 12 new koms removed. Re-adopt after C1, by re-packing from `kom_contents` (staging untouched) | everything you play-test meanwhile |
| D2 | **Answered 2026-10-05: build.** C10 builds the per-owner effect-alpha option (UI, save, the four parse sites), flag named by C10b. Original question: Effect-alpha option (`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, 4 rows): build June's per-owner effect-transparency game option (UI + save), or decline? | build: small, and June data uses it 29 times | C10 |
| D3 | **Answered 2026-10-05: hand off to the Rena port.** The census row is `handed-off`, receiver `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 3. C10 does not turn on `ADD_RENA_SYSTEM`. Original question: Natural force: March has Rena's natural-force code under `ADD_RENA_SYSTEM` (off). Turn it on (character territory: a Rena system), or hand it to the Rena character port? | hand off to the Rena port; turning on a whole character system is not an effect-engine change | C10 |
| D4 | **Answered 2026-10-05: build in C10.** Original question: HP-rate damage (`HP_RATE_DAMAGE_TYPE/VALUE`, 8 uses): build here? | build: self-contained in the damage path | C10 |
| D5 | **Answered 2026-10-06: hand off to the Lu/Ciel port** (C2: the only reader is `CX2GULuciel` `32b14bc0`); census row `handed-off`. Earlier, 2026-10-05: identify in C2, decide then. The census row stays `unsupported` with disposition `pending`; C10a asks again once C2 names the owner. Original question: Combination point (`HIT_ADD_COMBINATION_POINT`, 15 uses): identify its owner first, then decide | identify in C2 | C10 |
| D6 | **Answered 2026-10-05: keep until C11 passes;** C12 asks again whether to remove it. Original question: `SERV_IRUHADEV_JUNE_FX_DEBUG`: keep it through C11, then remove? | keep until C11 passes | C12 |
| D7 | **Answered 2026-10-06: hand C1 off to the character ports** (C1a's recommendation). Census: `Numerical skill data` and `Applied skill ID` are `handed-off`, receiver `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1, and `DAMAGE_SKILL_ID` follows them (`KEY_SYSTEM`); item 1's numerical line passes as handed off. C1b and its review are not run here. Question: C1a found that every numerical block uses a June reform skill ID (21000-32605) no March character learns, and June's `NewSkillTempletVer2.lua` is a different skill system. (a) hand off, (b) adopt June's file now, (c) dormant port? | (a) hand off | character ports |
| D8 | **Answered 2026-10-06: (a) hand off, and as a standing rule (user): anything in this plan that belongs to a character port is handed to the character ports, not ported here.** So all of C8c (every per-class `SetDamageExceptionProcess` block, Ara's `0x780` body) goes to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7b; C8c and its review are not run here. Original question (C8a): June re-keys three passives March characters have now. Rena's Powerful Bowstring (3405 -> 23405) and Sharp Kick (3400 -> 23400) are the same skills under reform IDs. Aisha's Battle Magician Fitness block (2413) is gone, and in its place Magical Staff (22409) applies the same two abilities (`SA_FORCE_DOWN_RANGE_REL`, `SA_MP_GAIN_ON_HIT_REL`). No March character learns a reform ID, so a verbatim port switches the three passives off. (a) hand the three blocks to the Rena/Aisha character ports, March's blocks stay until then; (b) port verbatim in C8c now | (a) hand off: the D7 precedent, and the port that brings the reform skills turns them on together | C8c (the Rena and Aisha parts only) |
| D9 | **Answered 2026-10-06 (user): build the engine now, and the UI later with the June UI port.** C10b-2 keeps the values in `GameOptions.lua` under June's own keys, so that port only has to wire them up. C10b-3 is handed to `JUNE_PORT_BACKLOG_ORDER.md` Stage 9 (UI sweep), with `kickoffs/june-effect/C10b-3.md` as its brief. Original question (C10a): the effect-alpha option's UI. June's option window is Scaleform GFx: `32f246d0` (`DW_GAME_SET_VALUE`, one of 39 `DW_*` GFx data wrappers) pushes `fEffectAlphaMineValue` / `fEffectAlphaMyTeamValue` / `fEffectAlphaOtherTeamValue` into a Flash movie. March has no GFx, so June's UI cannot be ported, and C10b-3 builds a designed stand-in under its own flag. (a) three sliders (my / my team / other team effects, 10-100 %) in March's option window `DLG_UI_Option_Back_New.lua` (data034), wired like `Slider_Sound_Volume` in `X2StateOption.cpp`; (b) no UI: the `GameOptions.lua` keys (C10b-2) plus a dev-console line; (c) defer the UI | (a): D2 asked for a UI, and the sound sliders are March's own pattern for a 0-1 value | C10b-3 |
| D10 | **Answered 2026-10-08 (user): (a).** A new engine phase widens March's user state id to 16 bits as June did, then the bounding port: C3d-1 (Opus, identify the widening), C3d-2 (Sonnet, port it), C3d-3 (Sonnet, the bounding state, user and NPC, with the registration in March's nine classes). Original question (C3b, 2026-10-08): June's RT_BOUNDING hit puts a user into three new user states, `USI_BOUNDING_DOWN` / `_LANDCRASH` / `_UP` (June `GUSI_END + 115..117`). They cannot be added to March as is. March's user state id is a `UCHAR` (`UserUnitStateData::stateID`, `SyncData::nowState`), and Chung's last state, `CSI_A_CIP_ARTILLERY_NOVA_FIRE`, is already **255** (`cl` probe), so three more `USI_*` entries before `USI_END` overflow it. June widened the id to 16 bits: registration `31fd89b0` reads a word into a vector state list, and checklist 7's row for `+0x7e84` has the future `nowState` as a protected short. March's own enum confirms the cap: `X2GUUser.h:436` comments out `USI_RIDING_HYPER_MODE` as "temporarily removed: more than 255 states". All 12 June classes register the three states in their own InitState, each with `*_DAMAGE_BOUNDING_*` tables in its class script. Options: **(a)** a new engine phase (Opus identifies, Sonnet ports) widens March's user state id to 16 bits as June did (`stateID`, `nowState`, the `m_StateList` key, every `UCHAR` cast and sync field; offline only). Then C3d ports the bounding case, the five handlers, the registration in March's nine class InitStates with June's state tables in their scripts, and the NPC half. The three entries before `USI_END` shift every class state id by 3; class ids are compile-time only, but `Enum.lua`'s `USER_STATE_ID` (data036) gains three rows and data036 is repacked. **(b)** hand the user half to the character ports (the registration is per class, and each class script needs its tables), with the widening as the checklist's blocker; C3d ports only the NPC half now. **(c)** park C3d whole until a character port needs it. | (a): any June class with more states than March's (Chung is already at 255) needs the widening anyway, and doing it once in the engine keeps C3d a straight port. RT_BOUNDING is in no March script, so nothing waits on it in play. | C3d |

**Census plumbing (Sonnet):**
- add the status `handed-off`, with `receiver` = document + phase, for the hand-off rows above;
- add the check "no June damage block gets damage only from a system March lacks" (item 1 of the
  definition);
- add the check "every binary-only row has a March parse site" (item 3).

**Recorded in the census (`census_completion.py`).** The decision table above is also data: `SYSTEM_DISPOSITION`
maps each absent system to build / handed-off / declined / pending, and `census.py` fails if an `unsupported` row has no
entry. The two new statuses are `handed-off` (receiver = document + phase) and `declined` (receiver = the decision and
its date). Dispositions as of 2026-10-05:
- **build:** numerical skill data and applied skill ID (C1); effect-alpha option (D2) and HP-rate damage (D4) (C10).
- **handed-off:** natural force (D3) to the Rena port and `ROSE_WEAPON_ATTACK_TYPE` to the Rose port, both
  `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 3; nested `BUFF_FACTOR/*` to `JUNE_BUFF_PORT_PHASES.md`.
- **pending:** combination point (D5), until C2 identifies its owner.
- **declined:** nothing yet.

**What the three checks do, and what they found** (census 2026-10-05):
- *Item 1* is one check line per damage source, so C1 can close its line without waiting for C10's. A block counts when
  it has no non-zero `DAMAGE` table of its own, uses the source, and the source's flag is not yet defined in
  `Always.h` **and** tested by an `#if` in `X2Lib`/`KTDXLIB`. Numerical skill data: **1,913** blocks (not the 1,915 measured
  on 2026-10-04: 2 of those 1,915 set numerical data without a skill ID, where June's override does not run either, so
  they deal 0 in June as well). **HP-rate damage: 7 more blocks that deal damage only through `HP_RATE_DAMAGE_*`**,
  which the 2026-10-04 measurement did not look for. Their line closes in C10, not C1.
- *Item 3* passes a binary-only row that has a March parse site (found by the census, or hand-cited in
  `HAND_PARSE_SITES` and verified against the preprocessed TU: `DAMAGE_DATA_NEXT_ULTIMATE%d`, `AT_RAID_FIELD`,
  `HIT_EFFECT_SET%d`), or is *owed* to C3/C4/C5/C10, which the plan makes responsible for giving it one. Owed rows are
  listed in the check line. `python june2017_re/census.py --final` (C11/C12) turns every owed row into a failure, and
  also makes items 2 and 4 strict.
- Every open row now has an owning C-phase in `owner_of()` (a new open row without one fails the census).

**Exit:** the decisions are recorded in this table, and `census.py` runs all-PASS except the two item-1 lines
(numerical skill data, closed by C1; HP-rate damage, closed by C10).

**Done note (2026-10-05, commit `66fef08`).** No game code: `flagcheck`, `build.py --deploy` and the play-test do not apply to C0.
- Census run (`--reuse-pp`, X2Lib/KTDXLIB untouched since the full run at 10:38): to-implement 39, unsupported 12,
  binary-only 17, **handed-off 3** (`NATURAL_FORCE_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`, nested `BFT_AURA`), declined 0. The
  plan's 2026-10-04 counts (46 / 14) had drifted: buff-port phases 7-7e since made three nested `BBT_*` rows `implemented`.
- `census.py` exits 1 with exactly the two item-1 lines failing (numerical 1,913 blocks, owner C1; HP-rate 7 blocks,
  owner C10); every other line PASS. `--final` additionally fails items 2, 3 and 4 until C3-C10 land, as intended.
- Two deviations from the plan text, for the user to confirm: (1) the item-1 check has a second line, HP-rate damage,
  so the census exits 1 until C10, not only until C1; (2) item 3 passes an *owed* binary-only row (14 of 17) in the
  default run, because the census defines a binary-only row as one with no March parse site found, so a strict check
  would fail all of them today. `--final` is strict.
- Files: `june2017_re/census_completion.py` (new), `census.py` (hook, `--final`, two statuses, tsv columns
  `owner`/`receiver`, matrix preamble), regenerated `census.tsv` and `JUNE2017_EFFECT_KEY_MATRIX.md` (line numbers had
  drifted since Oct 1), `june2017_re/README.md`, kickoffs C1a/C1b/C10b (census contract notes), `STATUS.md`.
- Next: `/x2-phase june-effect C1a` (Opus 5.5, high). It stays queued until the buff port is finished (D1).

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

**Status: handed off to the character ports (D7, 2026-10-06).** C1a (below) is done and its funcmap rows stand; C1b and the C1b review are not run in this plan. The port lives in `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1, together with the June skill reform it needs. ~~Not started. This is a live regression while June's data is live.~~

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
- The C0 item-1 line for numerical skill data passes: 0 blocks that get damage in June and none in March. (The
  HP-rate line is C10's.)
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

**C1a Done note (2026-10-06, Opus 5.5 high). Stopped for the user, as the kickoff asks.**
No code, so no flag, no flagcheck and no build/deploy (C2's precedent). 10 funcmap rows tagged
"Effect C1a" were added, all `confirmed`; `31fca8e0` supersedes candidate row 1214. Evidence:
`june2017_re/c1a_numerical_blocks.tsv` (one row per block) and `c1a_skilltemplet_diff.tsv`. Both
are regenerated by `c1a_numerical.py` from `skilltemplet_dump.lua` dumps, which run the bytecode
(June via `ljconv.py`), not the decompile, because ljd emits `]]` inside long strings.
- **The override** (`3263fe41` row):
  - Gate: INDEX > 0 or NAME, **and** APPLIED_SKILL_ID or DAMAGE_SKILL_ID. `USED_SKILL_ID` is not
    a trigger, so the census predicate is right and still counts **1,913**.
  - Attacker: the user, or an NPC's owner.
  - Level comes from `GetSkillLevel`, and the templet from `GetEquippedActiveSkillTemplet`
    (with the memo swap). The override skips unless level > 0 and the templet exists.
  - Writes PHYSIC and/or MAGIC (by `damageType`) = v × 0.01f, then PVP_RATE, TECH_POINT (truncated)
    and HIT_ADD_MP, each overwritten unconditionally.
  - Placed before `EndTable` and `SetDamageExceptionProcess`, so `fPowerRate` still scales the result.
    The March site is just before `luaManager.EndTable()` (X2DamageManager.cpp:605).
- **The table is not a separate file.** `NUMERICAL_DATA<n>` is a field of each skill templet, parsed
  by June's `AddSkillTemplet_LUA` (`32216320 @322186f5-32218e62`).
  - Each `m_Value` entry gives one row, and the level picks row min(level, rows).
  - `m_Pvp_Rate` defaults to 1, `m_Tech_Point` and `m_Hit_Add_MP` to 0, each clamped to its last entry.
  - So C1b's "new table loader, `LoadDataFile` + `DoMemory`" is the wrong shape. The port is an
    extension of March's `AddSkillTemplet_LUA`.
- **Why the stop.** June's file is a different skill system, not March's plus numerical data:
  - 521 shared templets, 451 of them changed (`m_eType` 232, descriptions 310, cooldown/MP/SA_ values);
  - 1,825 June-only templets and 47 keys no March templet uses;
  - an `OpenScriptFilePostProcess` call March lacks;
  - June's tree is 3,237 reform-ID rows against 123 old ones.
- **All 1,329 June templets carrying `NUMERICAL_DATA` are post-reform IDs, and none is in March's
  file.**
  - The 1,913 blocks point at 690 skill IDs, 21000-32605. March's templets are 100-8409.
  - March's shipped `Enum.lua` already names all 690, so the enum is not the gate.
  - The save (`unit_skill`, `unit_skill_unsealed`) holds 0 learned skills ≥ 20000. `GetSkillLevel`
    therefore returns 0 and June's own `jle @3263ff1e` skips the override.
  - **So a ported loader plus override changes nothing in play for March characters**, and C1's
    exit ("3 of the 492 show damage") cannot be reached as scoped.
- **Census caveat.** Item 1 turns PASS as soon as the flag is defined and `#if`-tested
  (`_flag_landed`). A dormant C1b would pass it while nothing changes in play, so item 1 is not the
  exit by itself.
- **June's scale differs** (June row 1 × 0.01 against March `DAMAGE`):

  | Skill | June | March |
  |---|---|---|
  | Aceldama | 9.5 | 2.33 |
  | Aiming Shot | 14.82 | 5.63 |
  | Eldrasil's Fury | 2.73 | 3.55 |
  | Shadow Blast | 21.45 | 8 |
- **Owed by the porting phase:** three blocks index past their skill's data, so June writes a zero
  row (0 damage, PVP_RATE 0). They need a `JUNE_BUGS.md` row:
  - `DamageEffect_ESK_Mortal_Blow_Ultimate_Fire_Ground` (21006);
  - `DamageEffect_LD_FLICK_SHOT_Projectile_Sub_Trans_Delayed_Expolsion` (30003);
  - `DamageEffect_SA_EMK_WindBlade_Ultimate_Projectile` (21013).
- The census refresh in this commit (`census.tsv`, the key matrix) is line-number drift from buff
  Phase 16 plus C2's D5 hand-off. It has no C1a content.
- **Decision for the user (C1b waits on it):**
  - (a) hand C1 to the character ports, where the skill reform lives;
  - (b) adopt June's `NewSkillTempletVer2.lua` now, which stays inert until characters learn reform IDs;
  - (c) land the parse and override dormant, and redefine C1's exit.
- **Answered 2026-10-06: (a)**, recorded as D7 in C0's table. The census numerical line now passes as
  handed off.

---

## C2 - Identification sweep (Opus, high, no code)

**Status: done 2026-10-06** (Done note at the end of this section). Every function the rest of 8b, Phase 9 and C9 waits on, identified in one
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

**Done note (2026-10-06, Opus 5.5 high).** There is no code, so there is no flag, flagcheck or build/deploy. 29 rows were added to
`june2017_re/funcmap.tsv`, each tagged `Effect C2` and carrying its evidence. No Ghidra mutation was made. Every row of the table:

| Row | Verdict |
|---|---|
| GetFinalDamage | `32284aa0`, already confirmed by the buff port; this = the defender. IGNORE_DEFENCE block `3228568a`. |
| `3261a150` | Inserts into the RANGE→rate map on DD+0x5b0. Consumer `32622780`, called from GetFinalDamage @`322857f6`. |
| `31fcf3e0`, `3227baf0` | Confirmed by buff Phase 10. The `ParsingBuffFactorID` family is **already ported** (`SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID`). |
| June `Process_ExtraDamage` | User `32034e50`, NPC `327995c0`. No-display test on `ExtraDamageData +0x101`; text via `322d39e0`. |
| `32622220` | A **lookup**, not a registration: it applies an `InsertDamageData_LUA` record by name. Moved from C3 to C9. |
| `321da350` | Stores the trace unit's nearest collision frame as an EffectSetInstance bone override (+0x130), read by `GetEffectPosition`. The three "helpers" are `32278580` and two std::wstring members. |
| Trigger family | `DAMAGE_TRIGGER_DATA`: finder `32610e60`, consumer `3227c940` (DTT 40). `ADDITIONAL_*`: reader `32610fb0`. `DAMAGE_TRIGGER_CONDITION`: no reader (candidate). |
| `HIT_ADD_COMBINATION_POINT` | Only in `CX2GULuciel` `32b14bc0` (candidate): a June-only class system (D5). |
| `InsertDamageData_LUA` | `32621080` confirmed, with its bind name. Reader `32622220`. |
| ES.Description | `32378e20` = `AddNPCTemplet`, `32680da0` = `AddNextStepPackage`. June never opens `EffectSet_Description.lua`, so **C9 declines it**. Viewer table: `31ee6100` / `3237a1f0` / `323760f0`. |
| slot `0x2b8` ×8 | Identity confirmed by vtable slot. The behaviour comparison stays with C8. |

Four findings change other phases. They are recorded in each kickoff's new "C2 findings" block, because kickoffs are the source of truth:
1. **`DIVISION_NUM`/`DIVISION_OFF_TIME` are a buff-apply gate, not damage math.** `3227a960` is the buff port's `CheckDamageDataKeyName`
   stub, misread there as a key-name index (funcmap row 1134, JUNE_BUGS #21, the comments at `X2DamageManager.cpp:600` /
   `X2DamageManager.h:1159`). The re-arm is the 8a handoff's unidentified row 66 (`32622ad0`). C4 recommends a non-`_DAMAGE_MATH` home for it.
2. **`DAMAGE_SKILL_ID` belongs to C1b** (the numerical override's skill-id fallback), not to C4.
3. **`DAMAGE_DATA_KEY_NAME` belongs to C9**, which loads the table it reads.
4. **C5's group 2 is already done** by the buff port.

User answers (2026-10-06): **D5 = port with Lu/Ciel** (handed-off, `census_completion.py`); the DIVISION gate **follows June's placement**, inside the damage-react buff apply, i.e. completing the buff port's `CheckDamageDataKeyName` stub under `SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY` (C4 kickoff).

---

## C3 - 8b, straightforward keys

**Status: done 2026-10-08 (see "C3 - Done": play-test passed; "C3 - Review": correct against June; identification left -> C3b). Model:** Sonnet 5, high, then an Opus review.

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

### C3 - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_DAMAGEDATA` extended, no new flag; play-test pending)

**What landed** (commit `51d1718`; details in `MODS.md`, "June effect completion C3"). `flagcheck JUNE_DAMAGEDATA --worktree --files <the 8 code files>`:
```
files: 6 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
`--range 51d1718^..51d1718 --with JUNE_FX_DEBUG`: `files: 6 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.

- **Scope corrections** (the kickoff predates the buff port and C2): `IS_AURA_BUFF` and `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT` were parsed and read by buff Phase 10b (`implemented` in the census); `DAMAGE_DATA_KEY_NAME` went to C9 (C2 row 1273); `ADD_MEMBER_COUNT` / `_DISTANCE` are **ExtraDamageData** keys (+0x104 / +0x108, the last two keys of the EXTRA_DAMAGE table: its `BeginTable` failure target `@326379bc` sits inside their range), not DamageData keys.
- **Parse** (`SetDamageDataFromLUA`, June's getter and default per key, cited by address; defaults from `SimpleInit 31f8ee50`, whose values differ from the parse default for none of them): `ATTACK_ONLY_ME`, `ONYL_ATTACK_USER`, `ADD_CHECK_DAMAGE_UNIT_NUM`, `FORCE_BACK_SPEED`, `BOUNDING_BACK_SPEED_X/Y`, `EXCEPT_ME_HP_UP`, `DAMAGE_LIGHT_TIME` (0.2), `IGNORE_ATTACK_SUCCESS`, `PAUSE_NPC_AI_TIME`, `DAMAGE_RATE_TO_SUPERAMOR` (1.0), `HIT_ADD_MP_PERCENT`, `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z` (1,1,1), `APPLIED_MEMO_ID`, `INIT_EXTRA_DAMAGE_BEFORE_LOAD` (a local: `31f8e430` + `ExtraDamageData::Init` before the table), `ADD_MEMBER_COUNT` / `_DISTANCE` (ED members; also in `Init()` and `Copy()`; the `Init()` reset to 0 is March's own: June's `31f8d4d0` never writes `+0x104`, so June leaves it undefined until the first parse). `DamageData` copies by `operator=`, so no hand-written copy drops a member.
- **Consumers ported:** every row in `MODS.md`'s table. The new `funcmap.tsv` rows (24, tag `(C3)`) hold the addresses, the x87 polarity of each branch, and the findings below.
  - `GetFinalDamage`: SUPERAMOR is `vslot 0x264( 0 )` = IsSuperArmor (rows 3229a669 / 32758440), PAUSE_NPC_AI_TIME is `3275b330` = `CX2GUNPC::SetTimePauseNPCAI` (same body as March's). Polarity of `fldz; fcomp; test ah,5; jp` run through `x87test.py` (mask 0x05): continues for t > 0, NaN skips.
  - The unit's `m_DamageData` sits at `this + 0x3a0` in June (`HP_PERCENT_UP` read at `+0x8a4`), which settled the user-side readers (`+0x7b0` = HIT_ADD_MP_PERCENT, `+0x98c` = DAMAGE_LIGHT_TIME).
  - `HIT_ADD_MP_PERCENT`'s action `322736f0` is new: `CX2GameUnit::JuneUpNowMpPerHitOthersPercent` ("MP +%d", blue pic char, offset 140.0). June's HP twin `32273540` multiplies by a gage factor March's `UpNowHpPerHitOthers` lacks: **not touched** (not C3's key, factor source unidentified; funcmap row).
  - `IGNORE_ATTACK_SUCCESS`'s action `31fcee10` = set `unitCondition.bHit` (future frame) on a user owner: new `CX2GUUser::JuneSetAttackSuccess`. Its sibling `31fcee40` (a `short` hit counter in the frame data, incremented for every user-owned effect hit) has no March member: not ported (C3b).
  - `CREATE_DAMAGE_EFFECT_NAME_SCALE`: three `SetScale` sites (CEffect RT_CREATE child, CEffect random spawn, `CX2GUUser::AttackResult` RT_CREATE child + random spawn). **The (1,1,1) default is not a pure no-op in principle, and is one in practice:** June's `SetScale` multiplies the main-effect scale by a per-instance base (`+0xb24`, (1,1,1) from the ctor `@31f7b35c`; the ctor itself calls `SetScale( s, s, s )` with `s` = DD `+0x5d0` = 1.0, `@31f7c7eb`), March's is absolute: equal for that base. March already runs `SetScale( entry SCALE x parent scale )` with (1,1,1) defaults on every hit / die / CREATE child, so the call adds no new class of behaviour. If the review wants it proven in play, an RT_CREATE_DAMAGE_EFFECT skill with a mesh template that carries a non-unit scale is the test.
  - `31fcd640( this )` after those SetScale calls = the class callback at `+0x8dc0` (class-set, no March member): not ported, checklist 7k.
- **Behaviour that is live in March now** (June parity; the review decides): a user-owned effect hit marks the owner's future-frame `bHit`; an invincible unit takes a DT_BUFF hit that is `ATTACK_OUR_TEAM` (March had no exemption); an NPC heal needs `GetNowHp() > 0`. Everything else is inert until June data (no March script sets the new keys).
- **Parsed and stored, no behaviour (rule 5 log once per key per templet):** `ADD_MEMBER_COUNT` / `_DISTANCE` (no reader anywhere in June's `.text`: displacement sweep for ED `+0x104/+0x108/+0x10c/+0x110` finds none in `Process_ExtraDamage`, `DamageReact_ExtraDamage`, `ProcessEnchantExtraDamage` or the manager), `BOUNDING_BACK_SPEED_X/Y` and `FORCE_BACK_SPEED`'s second consumer (`DamageReactStateChange 31feabd0`, behind `31fd86b0` = an armour-level check with no March counterpart), `APPLIED_MEMO_ID` (reader `3202ac40`, C8b, still queued: its kickoff now names the new member). -> **C3b** (Opus, identification) then a port phase.
- JUNE_BUGS 127 (ATTACK_ONLY_ME on the UID-driven paths hits nobody with the default `-1`) and 128 (EXCEPT_ME_HP_UP compares the UID's low dword). `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 2 (SUPERAMOR now engine-complete) and new 7k.
- **Census:** `census.py` ran clean (all checks PASS, `open rows by owner: C10 6, C11 3`): the 19 C3 rows are `implemented` (parsed with a March site); no row is owned by C3 any more. The regenerated matrix and `census.tsv` replace the committed ones (the diff is line-number drift in every `X2DamageManager:N` reference, as in C4); the two `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` rows (C9h-4d) that the generator reverted to `deferred` are fixed at their source (the tuple in `census.py`), and the per-key consumer notes live in `census_completion.KEY_NOTES`, so a regeneration keeps both. **8b is not marked done**: C3 closes its groups 4 and 5, but groups 1-3 are C4 / C5 (done) and `ADD_MEMBER_COUNT`, `BOUNDING_BACK_SPEED_X/Y`, `FORCE_BACK_SPEED`'s second consumer and `APPLIED_MEMO_ID` still have no behaviour (C3b, C8b).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,387,392 B, dated 2026-10-08 15:40:03, md5 4ee4c47b7f1e340c6124959d0d938449, equal to X2/US_SERVICE/x2.exe (2026-10-08 15:39:55)`. The new key strings and the three `JUNE_FX C3` log strings are in the deployed exe (checked by byte search).

**Play-test hand-off** (character `jtLordKnight`, user 25, Lord Knight, level 80; the dev console is not needed: no March data sets the new keys, so this is a regression pass over the paths that now run). ASK me before the game is launched.
1. **Where:** any dungeon. **Do:** normal attacks, then three skills (at least one that throws a projectile or a wave), get hit once by a monster, kill a few. **Look for:** damage numbers, hit-stop and hit reactions feel exactly as before; no freeze, no crash. **If not:** note the skill and say "check logs".
2. **Where:** the same run. **Do:** nothing extra. **Look for** (I read them; `python tools/logs.py --grep "JUNE_FX C3"`): one `DAMAGE_LIGHT_TIME first user DamageReact` line (DD 0.200, damageType 0..6), one `ATTACK_SUCCESS first user-owned effect hit <name>` line when one of your skills' effects connects, and one `NAME_SCALE first RT_CREATE_DAMAGE_EFFECT child` line only if a skill of yours creates a child damage effect. The other new branches need June data and print nothing.
3. **Chains:** use your normal combo and any skill whose follow-up is gated on "the attack hit". The live change can only make such a follow-up fire **more often**: look for one that now fires when only a projectile / wave connected and the melee swing missed. If you see that, say which skill; it is June's behaviour, but the review decides.
Log tag I will read: `JUNE_FX C3` (+ `0 EXCEPTION`).

**Play-test run, logs read (2026-10-08, run after the 51d1718 deploy):** 0 EXCEPTION; ErrorLog `JUNE_FX C3 DAMAGE_LIGHT_TIME first user DamageReact: DD 0.200, timer now 0.200, damageType 0`; offline_server.log `JUNE_FX C3 ATTACK_SUCCESS first user-owned effect hit ELSWORD_SONIC_BLADE`; no `NAME_SCALE` line (no RT_CREATE_DAMAGE_EFFECT child hit; that site stays unexercised). The five UNHANDLED packets are `E_DISABLE_HB_CHECK_REQ` (known, ignored). Check 3 (chains): the user reports the skills feel the same. **C3 play-test PASSED 2026-10-08.**

**Left to others (named, not changed):** C3b (identification, then its port: armour level, bounding back-speed state, ADD_MEMBER_COUNT reader, hit counter `31fcee40`); C8b (`APPLIED_MEMO_ID`'s reader); the class callback `+0x8dc0` and `UpNowHpPerHitOthers`'s gage factor (character ports).

### C3 - Review (2026-10-08, Opus 5.5 high): correct against June; one citation fixed (comment only), no behaviour change

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_DAMAGEDATA --range 51d1718^..51d1718` (C3's only code commit; 6561d1a / 2db1697 / 061c76c are docs):
`files: 6 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `0 deleted line(s), ...` / `RESULT: PASS`.
`--range 51d1718^..HEAD` (the kickoff's form): `files: 6 PASS, 2 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 1 ERROR` / `1 deleted line(s)` / `RESULT: FAIL`. Both non-PASS rows
are C9h-4d's later commits, not C3's: OTHER `15f39a5` = its `_DEBUG` diagnostic in `X2GameUnit.cpp`; ERROR `d7ab5cb` = its `EFFECT_SET_DELETE_DEBUG` toggle in
`Always.h` (the 1 deleted line), where flagcheck trips on the existing `#if defined(SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET) && !defined(SERV_IRUHADEV_JUNE_DAMAGEDATA)` guard.
The review's fix: `--worktree --files X2Lib/X2DamageManager.cpp`: `1 PASS` / `RESULT: PASS`; its 2 deleted lines are the two C3 comment lines it rewrites.
`census.py --summary`: every check PASS, `open rows by owner: C10 6, C11 3`, no row owned by C3: the Exit holds.

**Checked against June (Ghidra and the binary, read-only); every site matches the port:**
- `GetFinalDamage` `@3228676a-32286786` (vslot `0x264`( 0 ) == 1 -> x DD `+0x620`) and `@3228682a-32286879` (ST0 = the `fldz` `@32286790` that `fcom` left; runs for t > 0, NaN skips; optr `+0x470`, type 1, `3275b330`).
- `AreaHit` `@326248eb` (defender UID vs the caller's UidType argument, team block skipped) and the limit exit `@32624ae4` / `@32624c5b` (only it adds `+0x4d8`); GroundHit and AreaHit have no `+0x5fc` test, as ported.
- The `+0x50d` / `+0x5fc` tests of UnitToUnit, MeshToUnit, CollisionDataToUnit and EffectToUnit (owner `31f8b560`; the port's NULL-owner guard is March-side safety).
- User `DamageReact`: invincible reject `@3204675e` (DT_BUFF 6, RT_THROW 14, RT_DUMMY_DAMAGE 7 = March's enums), back-speed gate, DAMAGE_LIGHT_TIME `@32048377` (DT_BUFF -> 0; stored for DD >= timer; NaN keeps). NPC: gate `@327b2632`, back-speed `@327b2ec6` (a NaN frozen time takes the frozen path in both), light time `@327b3215`, heal `@327b5073-327b5171` (BL gates the HP set and the effect only; `NowHp > [3342e84c]` = 0.0).
- `CEffect::AttackResult` `@31f85a9b-31f85b36` (same game + owner block as March's); `31fcee10( val, bFuture )` = `+0x7e2c` / `+0x80cc`; `31fcee40` = the now-frame short counter (C3b). `322736f0`: max( 1, MaxMp x pct ), NaN -> 1, text int = ftol( fAdd ), `[33443958]` = 140.0. User `AttackResult` light time `@32012cb5` and HIT_ADD_MP.
- NAME_SCALE: user `+0x838` (= `0x3a0 + 0x498`) on both spawns, `31fcd640` after the RT_CREATE child only; CEffect random spawn `+0x698` behind the user-owner gate.
- Mana no-display: user `@320353d4` / `@320354b4`, NPC `@32799b91` / `@32799c90`: each skips only the text; `+0x3f90` (m_bHitedDrainMana) is still written.
- Parse defaults `[3346eeb8]` = 0.2f (DAMAGE_LIGHT_TIME), `[334380f0]` = -1.0 (ADD_MEMBER_COUNT_DISTANCE). DAMAGE_DATA_KEY_NAME: C2 handed it to C9 (funcmap row 1273), which ported it (`InsertDamageData_LUA`).
- `flagprobe.py`: DUNGEON_ITEM, HAMEL_SECRET_DUNGEON, NEW_SKILL_2010_11, ADD_MEMO_1ST_CLASS all ON, so every `#ifdef`-nested C3 site compiles.

**Finding, fixed (comment only):** MeshToUnit's ONYL_ATTACK_USER test is `@326257bb-326257cb`, not `@3262827c` (that range is vector math); UnitToUnit's is `@326252a6-326252b6`.
Corrected in `X2DamageManager.cpp` (same flag) and funcmap row 2153; the behaviour was already right. **Not rebuilt:** a comment changes no code, and the tree holds
peers' uncommitted edits (`Always.h`, `KTDXDeviceManager.cpp`, five `Offline/` files) that a build would ship. The deployed exe is `131c16eb` (md5 read from the game dir's `X2_offline.exe`, 2026-10-08 16:51, the june-item-kim K3 deploy), built after `51d1718` (15:45), so it contains C3.

**SetScale (the kickoff's question): equivalent today.** June's `31f6fdc0` multiplies by a per-instance base `+0xb24..`. Its only writers are the ctor (`@31f7b35c` = 1;
`@31f7c80a-31f7c838` = DD `+0x5d0` when IGNORE_MI_HUGE is false) and `31f8aef0`, whose one caller is modifier tag 15 (`321db1b0 @321dbad4`; funcmap row 75 relabelled: a base-scale
modifier, not a position offset). March writes neither: `+0x5d0` is memo case 2 of `3202ac40` (C8b, behind the `31fdbbb0` stub) and only tag 1 is ported. So the base is (1,1,1).
Routed to the C8b kickoff ("Added by the C3 review") and checklist 7a: whoever ports either writer ports the base and a multiplying `CEffect::SetScale` with it. The matrix
row `IGNORE_MI_HUGE` ("no writer found") is wrong on this point; it is generated, so it is named in the C8b note, not hand-edited.

**Live behaviour (the kickoff's question): June parity, kept.** The owner's future `bHit` set by a user-owned effect hit (`31fcee10( 1, 1 )`), the DT_BUFF + ATTACK_OUR_TEAM hit
on an invincible unit, and the NPC heal's NowHp > 0 test are June's code as read above; standing rule: follow June. The play-test felt no difference.

Post-commit (`e16806f`), `flagcheck --range e16806f^..e16806f`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `2 deleted line(s)` (the two rewritten comment lines) / `RESULT: PASS`.

No new play-test. Next: C3b (Opus 5.5 high, identification only).

---

## C3b - identify the armour-level check, the bounding state, ADD_MEMBER_COUNT's reader and the hit counter

**Status: done 2026-10-08 (no code). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3b.md`.

### C3b - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build)

19 new funcmap rows tagged `(C3b)` / `Effect C3b`, plus corrections to C3's rows `31fd86b0`, `31feabd0 @31feb6a7`, `31fcee40` and
`ExtraDamageData+0x104`. The per-case table of `DamageReactStateChange` is in `june2017_re/c3b_damage_react_sites.tsv`. March enum values were
settled with the compiler (an `offsetprobe`-style `cl /Zs` probe), not by counting lines. No code, so there is no flagcheck and no build; the
deployed exe is unchanged.

1. **`31fd86b0` = June's directional / typed super armour** (`JuneIsSuperArmorAgainst( bAttackerFront, DD )`).
   - The body: the super-armour list (`m_vecSuperArmorPtr`) non-empty -> true; `stateParam.bSuperArmor` false -> false. Otherwise a new per-state
     `set<int>` filters by `SUPER_ARMOR_CONDITION`: `SAC_FRONT` 0 = only against hits from the front, `SAC_NORMAL_ATTACK` 1 = not against
     `AT_SPECIAL`, `SAC_SPECIAL_ATTACK` 2 = only against `AT_SPECIAL`.
   - The parse: `CommonStateStart` reads `SUPER_ARMOR_FRONT` (inserts `SAC_FRONT` and sets `bSuperArmor`) and the `SUPER_ARMOR_CONDITION`
     int list.
   - March has neither key, and no March script sets them. With an empty set the check equals March's `IsSuperArmor()`.
   - **C3's count was wrong.** There are 22 calls, not 6. Of the 6 `FORCE_BACK_SPEED` tests, 4 follow the check and 2 follow `IsSuperArmor`
     (FLY `@31feb425`, which is buff 14c's tutorial block at `X2GUUser.cpp:12421`, and DRAG_UP `@31feb58b`).
   - FORCE's second consumer: a super-armoured unit still takes the vertical knock speed (`31feb28f`). The same happens to a front-armoured
     unit hit from the front, even without the key.
   - Two other June differences: DOWN has no `SetNowForceDown( 0 )` (March has it under `FIX_FORCE_DOWN_INITIALIZE_BUG`), and there is a
     pre-switch class block at `+0x9175` (checklist 7l). -> **C3c.**
2. **`BOUNDING_BACK_SPEED_X/Y`** feed June's RT_BOUNDING case.
   - The case writes `m_vBoundingBackSpeed` (x mirrored by facing), then `StateChange( USI_BOUNDING_DOWN )` in the air or
     `USI_BOUNDING_LANDCRASH` on the ground.
   - Five `CX2GUUser` base handlers run the states (DOWN -> LANDCRASH on landing; LANDCRASH -> UP at `USEFUL_VALUE0[1]`; UP starts with the
     stored speed, then goes to AIR_FALL / AIR_DOWN_LANDING).
   - Every June class registers the states in its own InitState (12 of 12, each with `*_DAMAGE_BOUNDING_*` tables in its script). The NPC
     twin is `32769060` with three new common-state types (`0x21..0x23`), and no June NPC script names them.
   - March's RT_BOUNDING case is dead code (`BOUNDING_COMBO_TEST` off), and no March script uses RT_BOUNDING.
   - **Blocked:** March's user state id is a `UCHAR` and Chung's last state is already 255 (cl). June widened the id to 16 bits. -> **D10**
     (user), then **C3d**.
3. **`ADD_MEMBER_COUNT` / `_DISTANCE` reader found.** It is user `Process_ExtraDamage` (`32034e50`) case `EDT_TRANSIT_DEBUFF`.
   - Flow: after `attacker->ProcessTransitDebuff( DD, this )` (as March), it collects up to N units around the attacker (`32294e30`, a June-only
     collector; the distance defaults to 1000 when not > 0). Each of those units also runs `ProcessTransitDebuff( DD, this )`.
   - C3's sweep missed it because it reads DamageData-relative `+0x154 / +0x158`: always the main `m_ExtraDamage` (JUNE_BUGS row with C3c).
   - The NPC twin is March's.
   - June's `ProcessTransitDebuff` (`322a8240`) is March's `FIX_EDT_ENCHANT` body, but applies factors one by one (`322a6d00`). -> **C3c.**
4. **`31fcee40`'s counter has no reader in June.**
   - Its getter `31fcee60` has no caller.
   - No read of unitCondition `+0xe` follows any of the 110 `GetUnitCondition` (`3208e120`) calls or the 214 inline base uses.
   - Only the increment and two state-start resets exist. Dead code: not ported, no checklist row.

**Found on the way.**
- The USEFUL_VALUE **Now set** (`+0x8eb8..+0x8ef0`, CommonStateStart `@32060cd9`) is the one the bounding LANDCRASH handler reads.
- `kickoffs/june-effect/C9h-4f.md` named only the Future set's readers. It now has an "Added by C3b" paragraph: the kickoff file is the
  source of truth, and the plan's inline copy is frozen.
- C3d depends on C9h-4f.

**Class side -> checklist 7l (new):**
- Add's `0x81c` caller `32a07fc0 @32a080cc`;
- the bounding state registration in every class (D10 decides whether C3d or the ports do it) and their `*_DAMAGE_BOUNDING_*` state tables;
- the Lu/Ciel bounding-target hook `32b38cb0`;
- the `+0x9175` block.

**Port phases:**
- **C3c** (Sonnet 5.5 high, kickoff written): items 1 and 3. Inert on March data, except DOWN's dropped `SetNowForceDown( 0 )`, which is live.
- **C3d** (kickoff written, blocked on D10 and C9h-4f): item 2.

**Open for the user:** D10 (decisions table). *D10 answered the same day: (a); C3d is split into C3d-1 / C3d-2 / C3d-3 below.*

---

## C3c - port: super-armour condition, DamageReactStateChange's gates, ADD_MEMBER_COUNT

**Status: code done 2026-10-08 (see "C3c - Done" below); play-test and Opus review owed. Model:** Sonnet 5.5, high, then an Opus review. Kickoff `kickoffs/june-effect/C3c.md`, review `kickoffs/june-effect/C3c-review.md`.

- **Scope:** C3b items 1 and 3 (funcmap rows `(C3b)`, `june2017_re/c3b_damage_react_sites.tsv`). New flag `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION`
  (parse, check, the 22 sites, the 6 FORCE tests, DOWN's `SetNowForceDown`); `SERV_IRUHADEV_JUNE_DAMAGEDATA` extended for the TRANSIT reader.
- **Not with:** C9h-4f or C8b (`X2GUUser.cpp` CommonStateStart / DamageReact areas).
- **Exit:** strings in the deployed exe; a regression play-test (get knocked down, launched and dragged up in a dungeon) with 0 EXCEPTION;
  census rows `ADD_MEMBER_COUNT*` / `FORCE_BACK_SPEED` leave "parsed only".

### C3c - Done (2026-10-08, Sonnet 5.5 high)

Flags: new `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION` (items 1-2), `SERV_IRUHADEV_JUNE_DAMAGEDATA` extended (item 3; the collector needs `_DAMAGEEFFECT_RUNTIME`
for `CEffect::JuneGetLockOnAimPos`, with an `#error` in `X2GameUnit.cpp` if it is off). Funcmap rows `(C3c)` hold every address. Inert on March data
(no script sets `SUPER_ARMOR_FRONT`, `SUPER_ARMOR_CONDITION`, `FORCE_BACK_SPEED` or `ADD_MEMBER_COUNT`) **except RT_DOWN**.

1. **Parse + check.**
   - `CommonStateStart` reads `SUPER_ARMOR_FRONT`, clears the per-state set, reads the `SUPER_ARMOR_CONDITION` int list, and FRONT true sets
     `bSuperArmor` and inserts `SAC_FRONT`, in June's order (`@320588e5-32058d38`).
   - The set lives in `StateParam` (June's `+0x8030` is inside the state param, so it is copied and cleared with it).
   - `m_bStateSuperArmor` does take FRONT's true: the one June local is compared with `+0x89fc` (`@32058f05`) and tested by the NotRed block (`@32059000`).
   - `CX2GUUser::JuneIsSuperArmorAgainst` = `31fd86b0`, non-virtual (no June class overrides it).
2. **DamageReactStateChange.** The six June cases wrap the March bodies, which stay in `#else`:
   - SMALL / BIG / DOWN / UP / FLY / DRAG_UP carry all 22 `0x81c` calls, the 6 FORCE tests and the FORCE block (`nowSpeed.y = backSpeed.y`, no state);
   - the inner re-checks that cannot fire after the gate (`@31feaf02` ...) are kept as June has them;
   - FLY carries buff 14c's tutorial term plus `&& !FORCE` (`@31feb425`), and that block's comment is corrected;
   - UP / FLY / DRAG_UP take the vertical speed before the `0x81c` check and call `ResetOnSomethingFuture` inside it.
3. **DOWN, the one live change.** June's writers of the force-down gage (`SetNowForceDown 32081520`) are unit init `31fee7a9`, `AttackResult @3201382c`, the NPC
   `3276a8be` and `UpdateFlushForceDown 32a33dd2`. Nothing moved the RT_DOWN reset elsewhere, so it is dropped under the flag: a knock-down no longer empties
   the gage (JUNE_BUGS 131).
4. **ADD_MEMBER_COUNT.**
   - `Process_ExtraDamage` `EDT_TRANSIT_DEBUFF` runs the member loop after `ProcessTransitDebuff`. Distance = DD `+0x158` when above 0, else 1000.0; the
     idiom `fldz; fcomp; test ah,5; jp` is the one `x87test.py` documents (continues only for mem > 0, NaN skips).
   - `CX2GameUnit::JuneCollectUnitsAround` = `32294e30`, whole. Argument order from the call: ( vec, count, dist, -180.0, 180.0, no facing test, same team ).
     The NPC branch is the March pattern of `X2GUAdd.cpp:7137`. The facing branch is dead for the one caller but kept (a local copy of
     `JuneIsPosAlongLastTouchLine`, because the buff file's is file-static). The count is never read (JUNE_BUGS 133); the reader uses the main extra damage (132).
   - `ProcessTransitDebuff` needs no change: the HP test is `GetNowHp() > 0` in both. June's per-element loop is `322a6fd0` over `322a6d00`, so that body
     went into March's `SetKBuffFactorPacketToGameUnit` loop (two additions: the `vslot 0x3e0` refusal hook, `CX2GameUnit::JuneIsPacketBuffRefused`, default
     false, Add overrides it for BTI 0x156 / 0x1b6 -> checklist 7l; the finalizers' `SetBuffTempletID`).
5. **Logs, census.** The rule-5 lines for ADD_MEMBER_COUNT / _DISTANCE are `#if 0`; BOUNDING's text says `bounding user states (C3d-3)`; `JUNE_FX C3c` lines
   fire once on the first super-armour refusal and on the first member loop. `census_completion.KEY_NOTES` moved for the three keys.

**Hygiene** (`flagcheck`, paste after the commit):
- `flagcheck JUNE_SUPER_ARMOR_CONDITION --range 57c5651^..57c5651 --with JUNE_TUTORIAL_MODE`: files: 1 PASS, 0 PASS~, 1 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR; in this flag's files: 0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 `#else`/`#endif` without the flag token, 0 encoding change(s); RESULT: PASS.
  The `-2` on `X2GUUser.cpp` is the two-line comment correction inside buff 14c's tutorial block (an OTHER row: `JUNE_TUTORIAL_MODE`).
- `flagcheck JUNE_DAMAGEDATA --range 57c5651^..57c5651 --with JUNE_SUPER_ARMOR_CONDITION --files <the six code files>`: 5 PASS, 1 OTHER (that comment), 0 FAIL, 0 ERROR; RESULT: PASS.
  Without `--files` the tool ERRORs on `Always.h:5380` (`#if defined(...CREATE_EFFECT_SET) && !defined(...DAMAGEDATA)`), an older line this commit does not touch; the commit's `Always.h` change is the other flag's DEFINE.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,405,312 B, dated 2026-10-08 18:26:07, md5 eeabc914b8cf2deab8f2ae39720430d8, equal to X2/US_SERVICE/x2.exe (2026-10-08 18:26:02); built at mods/june-buff-port 61d480c + 11 uncommitted code file(s).` New strings checked in the deployed exe (UTF-16): both `JUNE_FX C3c ... (first seen)` lines and `bounding user states (C3d-3)`.

**How to see it in game** (play-test hand-off in the final message): knock-down, launch and drag-up in a dungeon with 0 EXCEPTION; after a knock-down the
force-down invincible fall comes sooner than before (the gage is no longer emptied by the knock-down). The two new JUNE_FX lines do not appear on March data.
**Open for the user:** nothing; June's Add class hook goes to the Add port (checklist 7l).

### C3c - Review (2026-10-08, Opus 5.5 high): correct against June; no code change; June's force-down flush sits in StateChange -> C3f

**Hygiene.** `X2GUUser.cpp` carries three flags, so both runs treat them as one change and keep to the six code files:
- `flagcheck JUNE_SUPER_ARMOR_CONDITION --range 57c5651^..57c5651 --with JUNE_TUTORIAL_MODE JUNE_DAMAGEDATA --files <the six>`: `files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR` / `2 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. OTHER = `X2DamageEffect.h`, `X2GameUnit.cpp` (only `_DAMAGEDATA` blocks).
- `flagcheck JUNE_DAMAGEDATA`, same form: `files: 6 PASS, ... 0 FAIL, 0 ERROR` / `2 deleted line(s)` / `RESULT: PASS`.
- The 2 deleted lines are buff 14c's two comment lines, rewritten inside its own `JUNE_TUTORIAL_MODE` block.
- The kickoff's form `--range 57c5651^..HEAD` FAILs only with ERROR on the older `Always.h:5380` guard `#if defined(...CREATE_EFFECT_SET) && !defined(...DAMAGEDATA)` (as the C3 review recorded); the `c0cd5fe` / `2119f7e` OTHER rows are C9h-4e's.
- `flagprobe.py`: RIDING_SYSTEM, SEASON3_MONSTER_2010_12, EVE_SECOND_CLASS_CHANGE, CHUNG_FIRST/SECOND_CLASS_CHANGE, SERV_ELESIS_SECOND_CLASS_CHANGE, NEW_SKILL_2010_11, FIX_FORCE_DOWN_INITIALIZE_BUG, FIX_FORCE_DOWN_AND_DETONATION_BUG, BUFF_ICON_UI, FIX_EDT_ENCHANT, TEST_GROUP_GRAP, `_DAMAGEEFFECT_RUNTIME`, `_FX_DEBUG`, `_OFFLINE` all ON.

**Checked against June (Ghidra and the binary, read-only); every site matches the port:**
1. **The 22 sites** (`31feabd0`, `@31feae0b-31feb64e`). Gate: `0x81c( 0 )` and FORCE (`DD +0x3ec`) clear -> break; then `0x81c( 0 )`, or `+0x7e2d` (Future `bAttackerFront`) and `0x81c( 1 )`, -> FORCE block `31feb28f` (`nowSpeed.y = DD +0x3e0`, no state). State ids by name (`Enum.src.lua`, GUSI_END 5): `0x1a/0x1b` SMALL, `0x1c/0x1d` BIG, `0x1e/0x1f` DOWN, `0x22` AIR_SMALL (SMALL and BIG share `31feb039`), `0x23` AIR_DOWN, `0x27` AIR_UP, `0x28/0x29` AIR_FLY (FLY and DRAG_UP), `0x55` / EBX `0x56` RIDING_DAMAGE_FRONT / BACK. DOWN writes hold `+0x8be8 = 0x26` (AIR_FALL) after each state change only (not after the `@31feb1c1` break or the FORCE block); FLY writes hold `+0x8be8` and grap `+0x9110` = the state; DRAG_UP neither. UP / FLY / DRAG_UP take the speed before `0x81c( 0 )` (`@31feb330`, `@31feb499`, `@31feb5b1`) and clear `+0x7e1c` (ResetOnSomethingFuture) after the two checks. FLY's gate is vslot `0x264` + `g_pMain +0x16a` + FORCE (`@31feb425`), its x mirror `@31feb44e-31feb47a`.
2. **DOWN's dropped reset.** `SetNowForceDown 32081520` has exactly the four callers of the funcmap row. A sweep of all 1,412 calls to the protected-float writer `323101d0` (615 functions) finds no inlined write to a `+0x60` field (7 hits, all stack locals). June has no reset on the RT_DOWN path: the verdict and JUNE_BUGS 131 stand. **Finding:** `32a33dc0`'s caller `320348c0` is `CX2GUUser::StateChange` (funcmap row 1923), not ForceDownAndDetonationProcess. `@32034931-32034994`: when the Future nowState (`+0x7e6c`, the state being left) equals vslot `0x114` (`3208de60`: `0x1e` DOWN_FRONT), `0x118` (`3208de70`: `0x1f` DOWN_BACK) or `0x490` (`330de330`: `0x25` AIR_DOWN_LANDING), the gage's UpdateFlushForceDown runs. Its other caller `327577d0` is the NPC twin. March runs it from CommonStateStart (`X2GUUser.cpp:16110`, `FIX_FORCE_DOWN_AND_DETONATION_BUG` ON) on **entering** DOWN_FRONT / DOWN_BACK / AIR_DOWN_INVINCIBLE / RIDING_DAMAGE_*, together with UpdateDamageFlushMP and the stun / freeze / groggy clears. Where June put those is not found. Luciel overrides all three getters (`32b4d810` / `32b4d850` / `32b4d9d0`); the other 12 user classes use the base. This is an unowned June change: new phase **C3f** (identify). JUNE_BUGS 131 and funcmap row `31feabd0 (C3c)` are corrected, and a row `320348c0 @32034931-32034994 (C3c review)` is added. The `X2GUUser.cpp` RT_DOWN comment ("cleared only by ForceDownAndDetonationProcess / its flush") is true of the March build; C3f's port corrects it (a peer has the file dirty now). The Done note's "force-down comes sooner" expectation holds only in part until C3f's port lands.
3. **Parse** (`32057b80`): `@320588e5` SUPER_ARMOR into the local `[esp+0x28]`, stored to `+0x7ff4` when it differs; FRONT into `[esp+0x3d]`; `3260ea70` clears `+0x8030` every time; `32fe9940` / `32fea6e0` from index 1 / `32345580` / `32fe9a10` only when the table opened; FRONT -> local 1, `+0x7ff4` 1, insert 0. `@32058f05` `+0x89fc` = the local; `@32059000` NotRed tests the local. **`31fd86b0`**: `+0x42a0 != +0x42a4` -> true; `+0x7ff4` false -> false; SAC 0 and not front -> false; SAC 2 and DD and `DD +8` (attackType) != 1 (AT_SPECIAL) -> false; SAC 1 and DD and == 1 -> false; else true.
4. **Collector.** Call `@32036bda-32036c11`: ( vec, `DD +0x154`, dist, -180.0 `[33477e98]`, 180.0 `[3344d398]`, 0, 1 ). Distance `@3203686d` (0 < d keeps it; else, and NaN, 1000.0 `[33437b40]`). Skips as the row says; a unit with no gage (`+0xc04` null) passes the HP test (March `GetNowHp()` = 1). Size factor 0.25 `[33437830]`. Angle z: `@32295114` / `@3229512b` (`test ah,5; jnp`, NaN passes). Distance: `@32295177` (`test ah,0x41; jp`: keeps d <= max, NaN refuses). Member loop `@32036c40-32036c68`: non-null, != attacker, `ProcessTransitDebuff( DD, this )`. The x87 masks were read directly; `x87test.py` was not re-run (no doubt left).
5. **`322a6d00`** vs `SetKBuffFactorPacketToGameUnit`: order refusal -> find (BTI + unique) -> templet -> clone -> SetFactorFromPacket -> finalizers (vslot `0x18`, clone `+0x34`) -> counters (`+0x64`) -> `0x24c` -> **`0x3e8`** -> push -> icon. The only difference beyond the two ported additions is the slot-250 hook, which buff Phase 11b declined (`X2GameUnit.cpp:13153`; same in SetBuffFactorToGameUnit).

**Nits, left as they are (no behaviour):** the collector's `std::stable_sort` vs June's `_Sort` only reorders equal distances, and every collected unit is processed (JUNE_BUGS 133); `X2DamageManager.cpp`'s `#if 0` closes with `#endif SERV_IRUHADEV_JUNE_DAMAGEDATA` (legal, misleading).

**Not rebuilt** (no code change); the deployed exe `eeabc914` contains C3c. No new play-test.

## C3d-1 - identify: widening the user state id to 16 bits (D10 (a))

**Status: done 2026-10-08 (see the Done note below). Model:** Opus 5.5, high, no code. Kickoff `kickoffs/june-effect/C3d-1.md`.

- **Scope:** every March site that stores, casts, compares, serializes or indexes a user state id as 8 bits (`UserUnitStateData::stateID`,
  `SyncData::nowState` and its Put/Get or sync packing, the `m_StateList` key, `UCHAR` casts, arrays sized 256, Lua binds that pass a
  state id), each with June's form where June has one (`31fd89b0` registration, the future `nowState` protected short, June's
  `StateChange` `320348c0`). Also: where the three `USI_BOUNDING_*` go before `USI_END`, what the +3 shift touches (`Enum.lua`
  `USER_STATE_ID` in data036; any saved or Lua-visible class state id), and the NPC side if it shares the type.
- **Exit:** a site list (`june2017_re/c3d1_state_id_sites.tsv`), funcmap rows `(C3d-1)`, C3d-2's kickoff with the flag name.

### C3d-1 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build)

Site list `june2017_re/c3d1_state_id_sites.tsv` (19 rows), 9 funcmap rows `(C3d-1)`. March values come from the compiler: the new
`june2017_re/enumprobe.py` probes any enum or constant with the nine class headers included. Ghidra was read-only.

1. **What widens (C3d-2, `SERV_IRUHADEV_JUNE_USER_STATE_ID16`).** These sites:
   - `UserUnitStateData::stateID`;
   - the `UserUnitStateDataMap` key;
   - `SyncData::nowState`. In June it is a protected short: Future `+0x7e6c`, Now `+0x810c`; `31fe6a40`, `3203c3a0`;
   - `UnitCondition::nextState`. It is shared with the NPC, and June widened it for both: user `320348c0` writes a short at `+0x7de4`,
     NPC `32758ea0` a word. It becomes signed `short`, keeping the `-1` sentinel;
   - the two `(UCHAR)` casts in `GetGameUnitState*`;
   - `m_OldStateId`, which is write-only;
   - the sync packet's `m_ucNowState`. June's bit 1 is 2 bytes (`32d5be30`).

   The NPC's own `nowState` stays a byte in June and in March.
2. **June's container is a vector, not a map.** Registration `31fd89b0` = `UpdateUserUnitStateData( USHORT, table, data )`: an
   overflow log, then `vec[id]`. Lookups fall back to `vec[0]`, and the now-state data is a pointer whose id is recomputed. March keeps
   its `map`: `operator[]` default-inserts where the vector holds a default entry, so only the key widens.
3. **`nowAction` widened too.** The checklist row `+0x7e84` guessed `nowState`. It is Future `nowAction`, and `0x1c` is March
   `UAI_RELEASE_FORMATION_MODE` (`cl`: 28, Add). The row is corrected. C3d-2 widens `nowAction` and its packet field under a second flag,
   `SERV_IRUHADEV_JUNE_USER_ACTION_ID16`. The helpers that take a `UCHAR` action stay as they are.
4. **The +3 shift.** The three `USI_BOUNDING_*` go after `#endif //RIDING_SYSTEM`, directly before `USI_END`. That is June's relative
   order; June's intervening event states have no March counterpart.
   - Probed today, Lua = C++: `USI_END` 88, Chung's last 255, `ASI_DASH_COMBO_X` 121.
   - After the shift: 91 / 258 / 124.
   - `Enum.lua`'s `CHUNG_STATE_ID` / `ARA_STATE_ID` are `USI_END`-relative and are read by `Chung_Iron_Cannon.lua` (the guard-state
     list), `Ara_Martial_Artist.lua` and data037's `Npc_Chung_01..05`. **So C3d-2 adds the Lua rows and repacks data036, and the exe and
     kom deploy together.** C3d-3's kickoff is amended to match.
   - The literal ESSI / AVSI / LESI / RSI / ENSI tables (training dungeons) are already 44 off C++ (`ESSI_COMBO_X1` 60 vs 104). June
     carries the same literals, so they stay.
   - No class state id is saved: none in `X2OfflineDB` or the offline handlers, no replay struct, no Lua bind taking a user state.
5. **No new effect, and one test needs a new character.** The widening changes nothing in play: no id goes above 255 until the shift.
   After it, only Deadly Chaser's Rapid Cannon (256 / 257) and Iron Paladin's Artillery Nova (6210 -> 258) land above 255. The save
   has neither job, so C3d-2's test creates one.

**Found on the way.**
- June's `32d5be30` carries 15 more sync fields March lacks (bit `0x400000` and 14 more at `+0x1c..+0x75`). They are dormant offline
  (no UDP peers). New identify phase **C3e** (kickoff written).
- `X2TrainingGameUI.cpp:98/277/343` compare via `(const char)`. This already breaks any id above 127, but the training literals are
  small, so it is unchanged. June's form of that function was not located.

**Open for the user:** none.

## C3d-2 - port: the 16-bit user state id

**Status: ready (C3d-1 done 2026-10-08). Model:** Sonnet 5.5, high, then an Opus review. Kickoff `kickoffs/june-effect/C3d-2.md`.

- **Scope:** C3d-1's site list behind `SERV_IRUHADEV_JUNE_USER_STATE_ID16` (with the three reserved ids and Enum.lua's rows, data036 packed), plus `nowAction` behind `SERV_IRUHADEV_JUNE_USER_ACTION_ID16` (C3d-1 item 3); nothing else changes. **Not with:** C3c or C9h-4f (all edit `X2GUUser.h/.cpp`).
- **Exit:** builds with the flag on and off; a regression play-test across several classes (state changes, skills, riding, a dungeon
  run; Chung's top states still reachable) with 0 EXCEPTION.

### C3d-2 - Done (2026-10-08, Sonnet 5.5 high)

Flags `SERV_IRUHADEV_JUNE_USER_STATE_ID16` and `SERV_IRUHADEV_JUNE_USER_ACTION_ID16` (`KTDXLIB/Always.h`, end). Commit: see `git log` for "C3d-2".

**What landed** (site numbers = `june2017_re/c3d1_state_id_sites.tsv`):
- Rows 1-3, 5-6 (`X2GUUser.h`): `UserUnitStateData::stateID` and `m_OldStateId` -> `USHORT`; `UserUnitStateDataMap` key -> `USHORT`;
  `SyncData::nowState` -> `KProtectedType<USHORT>`; the two `GetGameUnitState*` casts -> `(USHORT)`.
- Row 4 (`X2GameUnit.h`): `UnitCondition::nextState` -> signed `short` (shared with the NPC; `-1` stays the sentinel).
- Rows 7-9 (`X2UDPPacket.h`): `m_ucNowState` -> `USHORT` (name kept); `_Serialize` / `_Deserialize` are templates, so the cpp needs no edit.
- Row 10 (own flag ACTION_ID16): `SyncData::nowAction` -> `KProtectedType<USHORT>`, `m_ucNowAction` -> `USHORT`. The `UCHAR` action helpers stay.
- Row 16: `USI_BOUNDING_DOWN / _LANDCRASH / _UP` between `#endif //RIDING_SYSTEM` and `USI_END`; data036 `Enum.src.lua` got the same rows at
  `GUSI_END + 83..85`, `USI_END` `+ 86`, compiled and packed (`PACK OK data036`; the only entry that differs from the live kom is `Enum.lua`).
- No `.cpp` edit: rows 8-9, 11-15, 17-19 are untouched, as the tsv says.

**Gate.**
- Enum probe (`enumprobe.py`, flag on): `USI_END` 91, `CSI_A_CIP_ARTILLERY_NOVA_FIRE` 258, `ASI_DASH_COMBO_X` 124, `USI_BOUNDING_DOWN` 88. = Lua (91 + 167, 91 + 33).
- Completeness, `/Zs /w14244 /w14242` on 14 TUs, flag off vs on (`june2017_re/c3d2_warndiff.py`; it comments the two defines out of `Always.h` for the
  off pass and restores it). All 14 compile in both passes (`X2UDPPacket.cpp` needs `/FIstdafx.h` alone; checked separately, 0 warnings, rc 0).
  - `cl` reported `X2GUUser.cpp` lines 35 higher with the flag on (= the lines added to `X2GUUser.h`) from the first warning on, so the file:line
    diff was matched with that offset: 18 warning sites moved by exactly +35 and are unchanged in text.
  - What is left is exactly the intended set: `X2GUNPC.cpp:7617` (row 14, short -> char), and three `nowAction` -> `UCHAR` sites
    (`X2GUUser.cpp` 24173, 24702, 41362: `IsAvaliableActionDuringDamageReact( UCHAR )`, row 10). **No missed state-id site.**
  - Open for the review: why the line numbers shift (not understood; the offset matches the header's added lines, nothing in the `.cpp` changed).
- Flag-off build: the off pass above compiled every TU; the shipped exe is flag-on.

**Hygiene** (`flagcheck --worktree --files Always.h X2GUUser.h X2GameUnit.h X2UDPPacket.h`):
```
STATE_ID16:  files: 1 PASS, 0 PASS~, 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR
             in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
             RESULT: PASS
ACTION_ID16: files: 0 PASS, 0 PASS~, 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR   (the two flags share files; OTHER = the other flag)
             in this flag's files: 0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)
             RESULT: PASS
```
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,410,432 B, dated 2026-10-08 22:21:54, md5 6db35bc54e7dbc559ea2345bdade8bdf, equal to X2/US_SERVICE/x2.exe (2026-10-08 22:21:50); built at mods/june-buff-port 50e2246 + 4 uncommitted code file(s).`

**data036 packed and copied (2026-10-08 22:34, `COPY OK`, md5 `f7a27248...`).** `kom_contents/data036_packed.kom` holds the new `Enum.lua`; the exe and the kom must reach the game together
(the exe is already deployed, so the user's yes to `/x2-kom-packed copy data036` completes it). MODS.md has the row and the "ship together" note.

**Play-test PASSED (user, 2026-10-08; logs checked 22:36-22:42 run, after deploy):** 0 EXCEPTION, UNHANDLED = 5x `E_DISABLE_HB_CHECK_REQ` (baseline), clean exit census; ErrorLog holds only the usual missing-particle noise. The eight checks in `kickoffs/june-effect/C3d-2.md` (states above 255, Chung guard list, Freils skills, Ara chain, every class, riding, a full
clear). The save rows were re-read today: RapidT 42 slot 0 = 6218, NovaT 43 slot 0 = 6210, arasss 24 = 7001 **A**, 7200 **S**, 7214 **D**, 7010 **C**.

### C3d-2 - Review (2026-10-08, Opus 5.5 high): correct against June and the site list; no code change, no build, no deploy

**Hygiene.** `flagcheck JUNE_USER_STATE_ID16 --range 30efc54^..HEAD`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR` /
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
`JUNE_USER_ACTION_ID16`, same range: `0 PASS, 0 PASS~, 0 DEFINE, 4 OTHER, 0 FAIL, 0 ERROR` / same zeros / `RESULT: PASS`.
OTHER = the two flags share `X2GUUser.h` / `X2UDPPacket.h`; `Always.h` holds both defines; `X2GameUnit.h` is STATE_ID16 only. No deleted line.

**1. Sites, case by case** (`c3d1_state_id_sites.tsv`, commit `30efc54`). Rows 1-7, 10 and 16 match their "C3d-2 action". Row 1 is `stateID` USHORT.
Row 2 is `map<USHORT, ...>`. Row 3 is `KProtectedType<USHORT>`; June's setter is `<short>`, but every June reader masks `& 0xffff`, so the value is the same.
Row 4 is a signed `short` `nextState`. Row 5 is the two `(USHORT)` casts. Row 6 is `m_OldStateId`. Row 7 is `m_ucNowState` USHORT.
Row 10 is `nowAction` and `m_ucNowAction` USHORT.
Rows 8-9, 11-15 and 17-19 are untouched: the commit edits no `.cpp`. `_Serialize` / `_Deserialize` (`X2UDPPacket.cpp:3-20`) are `sizeof(T)` templates.
`Enum.src.lua`: the C3d-2 delta against `march2014/data036/Enum.src.lua` is exactly the three `USI_BOUNDING_*` rows (`+83..85`) and `USI_END` `+86`, in June's relative order.
**2. Completeness, re-run independently.** The scan compiled **committed HEAD** `43a1733`, not the working tree, because a peer had `X2GUUser.cpp/.h` dirty. It was an export with `cl` on a short F: path (VS2010 cannot open the long scratch paths).
It ran flag-on `/Zs /w14244 /w14242` over the **127** X2Lib TUs that name a state-id expression, not the 14.
- **Filter A:** every 16 -> 8-bit conversion. It found the three `IsAvaliableActionDuringDamageReact( UCHAR )` sites (24173 / 24702 / 41362), `X2GUNPC.cpp:7617`,
  `X2StateServerSelect.cpp:10177` (gamepad map, unrelated), and one the Done note did not list: `X2GUUser.cpp:41426`. That is `GetActionSyncData`'s
  `make_pair( iIndex, nowAction )` into `map<int, UCHAR>`, reported at `<utility>(163)`. It is inside row 10's decision (the action helpers stay 8-bit; ids < 32).
  C3d-2's file:line diff could not see it, because `utility(163)` already warned in both passes for an unrelated `BONUS_TYPE` pair.
- **Filter B:** every narrowing to `char`/`UCHAR`/`BYTE` from any type whose source line names a state id. This covers `GetNowStateID()`, which returns `int` and so warns as int -> char in both passes.
  It found only NPC, pet and square/field-unit state ids (`X2GUNPC.cpp` 4789 / 5978 / 26539, `X2Pet.cpp`, `X2State.cpp`, `X2TFieldGame.cpp`), plus the intended `:7617`.
- **Filter C**, for narrowing through an intermediate variable. It reads all 127 to-8-bit warnings in `X2GUUser.cpp/.h`, `X2GameUnit.cpp` and the nine class `.cpp` by hand.
  They are `lastTouchLineIndex`, `m_SPLevel`, `nextSubState`, crit type, counts, cannon balls and local directions, plus the three action sites. **No user state id is narrowed implicitly.**
- C9h-4e's deployed exe `a0344cec` was built at `997287e`, a descendant of `30efc54`, so it does carry C3d-2.
- `X2StateArcadeResult.cpp` fails `/Zs`; it is not in `X2Lib_2010.vcxproj`, so it is dead.
- **The 35-line shift (Done note's open question).** It does not reproduce. A flag-off pass of the same HEAD copy puts all 30 shared `X2GUUser.cpp` warnings at the same lines as flag-on.
  The compiler does not shift lines with the header. The likeliest cause is `X2GUUser.cpp` changing between C3d-2's two passes (peers edit it, as one does today). The verdict is unaffected.
**3. Readers the compiler cannot see** (grep over `X2Lib`):
- Explicit byte casts: the C3d-1 rows 12 / 15, plus one the tsv missed, `X2StateDungeonGame.cpp:3952` (`char nowUserStateID = (char)GetNowStateID()`). It is dead:
  `m_bShowSkilList` is only set under `OLD_COMBO_LIST_UI`, which the `cl /P` probe settles as undefined. The tsv's row 12 now names it.
- None of the rest exists: no user-state Lua bind (only `CX2GUNPC::GetNowStateID` is bound), no state id in `Offline/` (its `GetNowStateID` hits are `g_pMain`'s X2 state), no `%c` print, no 256-array.
- **The +3 shift** turns a signed-`char` reader's 125-127 into 128-130. That matters only for the dead casts above and the training literals (row 15), so nothing changes.
- The `-1` sentinel cannot index `m_StateList[65535]`. The `nowState = nextState` copy (`X2GUUser.cpp:6604` at HEAD) runs only when `stateChangeNum` differs, both start at 0, and only `StateChange` bumps it after writing a real id.
- **Layout parity.** `X2UDPPacket.cpp` is excluded from the build and compiled inside `X2Packet.cpp` after `stdafx.h`. `X2Packet.h` is reached at `X2Main.h:138`, after `KTDX.h` (`:7`), so no TU sees the narrow structs.
**4. Lua.** `grep -ac USI_BOUNDING_DOWN` on the compiled `data036_kom/Enum.lua` = 1. `data036_packed.kom` md5 = game-dir `data036.kom` = `f7a27248...`; `kom.py status data036` reads `in sync`.
The literal tables are untouched (`ESSI_COMBO_X1` / `AVSI_COMBO_X1` 60, `LESI_COMBO_X1` 70, `RSI_COMBO_X` 63).
**5. June (Ghidra, read-only).**
- `32034a77`: `MOV word [ESI+0x7de4],DI`, then `byte [+0x7de6] = 0xff`, i.e. a 16-bit `nextState` and `nextSubState = -1`.
- `32d5be30`: bit `0x1` reads 2 bytes into `+4` and bit `0x2` reads 2 bytes into `+6`. Bit `0x4` reads 1 byte into `+8`, and bit `0x400000` reads 1 byte into `+9`, which is C3e's.
- These are the same offsets as March's widened struct after the DWORD flag.

**Finding:** none needing code. The tsv row 12 addition is documentation. The deployed exe is C9h-4e's `a0344cec`, built on top of C3d-2. It stays, and so does data036 `f7a27248`; no new play-test.

## C3d-3 - port: the bounding back-speed state

**Status: code done (2026-10-09); class tables -> character ports (C3d-4, user 2026-10-09). Model:** Sonnet 5.5, high, then an Opus review. Kickoff `kickoffs/june-effect/C3d-3.md`, review `kickoffs/june-effect/C3d-3-review.md`.

- **Scope:** C3b item 2: the user RT_BOUNDING case, the three `USI_BOUNDING_*` states, the five `CX2GUUser` handlers, `m_vBoundingBackSpeed`,
  the last-attacker UID / unit type (`+0x40e0` / `+0x40e8`, written next to buff 9b's `+0x40dc` serial), the registration in March's nine
  class InitStates with June's state tables in their data036 scripts (+ `Enum.lua`, packed), and the NPC half (`32769060`, `3275d900`, `3275a070`).

### C3d-3 - Done (2026-10-09, Sonnet 5.5 high; rewritten after the user's "no invented Lua" rule)

Flag `SERV_IRUHADEV_JUNE_BOUNDING_STATE` (`KTDXLIB/Always.h`, end; `#error` without `USER_STATE_ID16`, `USEFUL_VALUE`, `DAMAGEDATA`). funcmap rows `(C3d-3)`, JUNE_BUGS 140 / 141.
**State: the engine code is in; the three states are NOT registered by any class, so RT_BOUNDING is ignored (logged once). Nothing is play-testable until phase C3d-4 gives the classes June's tables.**

**What is in.**
- **User case.** `CX2GUUser::DamageReactStateChange`: the RT_BOUNDING case (June `31feabd0 @31feb68f-31feb71a`) replaces March's dead `BOUNDING_COMBO_TEST` case (kept in `#else`). Super armour ignores it; mirrored back speed;
  DOWN (air, `nowSpeed.y = backSpeed.y`) or LANDCRASH (ground, speed 0). It checks `m_bJuneBoundingStatesReady` first.
- **Members and handlers.** `CX2GameUnit`: `m_vJuneBoundingBackSpeed` (+0x40f0), `m_uidJuneLastAttacker` (+0x40e0), `m_eJuneLastAttackerUnitType` (+0x40e8). Five virtual `CX2GUUser::JuneBounding*` handlers; LANDCRASH's due test is
  `EventTimerOneshot( JuneGetUsefulValueNow( 0, 0 ) )` on the future anim. x87 settled: `31fd0291` = strictly above; `31fd0354` = `y < 0` falls, equal / NaN reach the landing test.
- **Last attacker** stored by both DamageReacts (user before the back-speed gate, NPC before the guard test). June's user store has no null test (JUNE_BUGS 141): March keeps one.
- **`CX2GUUser::JuneInitBoundingStates( prefix )`** registers the three states from a class's `<prefix>DAMAGE_BOUNDING_*` tables, or none (logged) when one is missing. **No class calls it yet** (the nine calls and a loader for an invented data file were removed).
- **NPC half.** Types `STATE_TYPE_DAMAGE_BOUNDING_DOWN / _LANDCRASH / _UP` before `STATE_TYPE_REACTION_STATE_NUMS`, the RT_BOUNDING case (`32769060`), the bounding-up start (`3275a070`), key `NOT_BOUNDING`. Bytes: `+0x872b` =
  `m_bNeverMove`, `+0x8ac6` = `NOT_BOUNDING`, `+0x8ac4` = zeroed by the ctor, written by no NPC code (not ported). Row `3275a070` corrected: its Lu/Ciel half compares LANDCRASH. No NPC script names the states.
- `X2DamageManager.cpp`: the "unsupported key" log for `BOUNDING_BACK_SPEED_X/Y` is skipped under the flag.

**What was removed, and why.** The first pass put June's 27 `*_DAMAGE_BOUNDING_*` tables in a NEW data036 file `June_Bounding_States.lua` and staged a RT_BOUNDING hit in data037 `Thief_Thin_Zero`. The user's rule (2026-10-09) forbids inventing Lua files, so
both are gone: the loader, the nine registration calls, the file and its manifest line. `PACK OK data036` and `PACK OK data037` now show `+0 added, -0 removed, ~0 changed` against the live archives, so there is nothing to copy. The reasoning for the file
(a decompile + recompile of `Elsword_SwordMan.lua` differs in 30,072 of 192,287 bytes) was never tested for behaviour, only for bytes (march vs march).

**The blocker (C3d-4).** June carries the three tables inline in each class script. March's nine class scripts are bytecode only. Getting the tables in means editing existing Lua: first prove a recompile equivalent (`june2017_re/ljdiff`), then
the user's go-ahead for editing the nine scripts, or another route. That needs a higher model (Opus) and a user decision; it is not decided here.

**Twins checked.** March's NPC `DamageReactStateChange_CheckOnly` RT_BOUNDING case is dead (`X2OPTIMIZE_NPC_NONHOST_SIMULATION_ANALYSIS` OFF), the `m_FlyCount++` case is `BOUNDING_COMBO_TEST` (OFF). **Open:** June's fly counter in `32045dc0`
was not located, so whether June counts RT_BOUNDING hits there is unchecked.

**Found on the way.** `SET_LIMIT_MAX_G_SPEED` (every `*_DAMAGE_BOUNDING_DOWN` carries -4000): a float state key June reads into a protected unit float (`+0x40f8`, user `@32054734`, NPC `@32787303`); March reads none. Phase **C3g**.

**Hygiene.** `flagcheck --worktree` on the changed files: PASS; the commit range pass is in the commit message of the revert. **Deploy:** see the revert commit (`DEPLOY OK` line in STATUS).

### C3d-3 - Review (2026-10-09, Opus 5.5 high; code-only, two fixes)

Commits `fea1dc9` + `7a5c9ad` checked against the June binary (Ghidra + `disasm_range.py`; operand sweeps over all of `.text`). funcmap rows `(C3d-3 review)`; new phases **C3i**, **C3j**, **C3k**.

**1. User handlers and case: correct.** State ids by name (GUSI_END = 5: `0x78/0x79/0x7a` = BOUNDING_DOWN/LANDCRASH/UP, `0x25` AIR_DOWN_LANDING, `0x26` AIR_FALL); `StateChange( id, 0 )` in the handlers, `( id, 1 )` in the case; `+0x304/+0x308` = nowSpeed; DD `+0x3e0` = `backSpeed.y` (`+0x3dc` feeds nowSpeed.x at `@32046f6f`). LANDCRASH reads the future skin anim (or `+0x8e84`), `[+0x24c]` = `m_fAnimOneshotTimer`; `EventTimerOneshot( t )` is exactly `m_fAnimOneshotTimer < t && EventTimer( t )`. **x87 re-settled by running June's own bytes** (scratch harness, `fld/fld/fcom st(1)` and `fld/fcomp m64` on concrete floats): `31fd0291` proceeds only for value > mark (equal and NaN skip); `31fd0354` falls (0x26) only for y < 0 (-0, 0, NaN go to the landing test); `[0x334350e8]` = double 0.0. x87test's trace line "`[esp]` = mark" is a tool artifact: after `fstp st(1)` the value is on top, so `31d28e10` gets the value. Do not "fix" it.

**2. NPC half: two transcription errors fixed, one gap routed.**
- Type order 0x21/0x22/0x23 confirmed (name lengths 20/25/18); guard path `+0x8b7a`/`+0x8700` = `m_bGuard`/`m_DownDamageLandGuard` (type 0x20, `enumprobe`); `31e8ae10` = March `IsOnSomething()`; facing test (own `+0x844d`) correct.
- **Fixed: `NOT_BOUNDING` is an NPC INIT key, not a state key.** Its read is in `327a0fa0` (= `InitComponent`, after `SHOW_DAMAGE_AT_IMPACT_POINT`), `@327a683d`. June has only three `+0x8ac6` operands: that read, the ctor's dword zero `32762c60`, and the bounding test. The port now reads it in `CX2GUNPC::InitComponent()`; the LoadStateData read, `FLAG_JUNE_NOT_BOUNDING`, the CommonStateStart re-read (both `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branches) and the ReInit reset are gone. March never calls `CX2GUNPC::ReInit`; its one call site is commented out, `X2Game.cpp:6238`.
- **Fixed: `+0x8ac4` is written: it is `NOT_UP_STATE`.** `327a0fa0 @327a5d52` reads a bool INIT key into it. The decoded key buffer `0x337aa208..213` = `NOT_UP_STATE` (`+0x8ac5` = `0x337aa1f4..207` = `NOT_AFFECT_BLACKHOLE`). 36 June NPC scripts set it, and they ship. The port reads it (`m_bJuneNotUpState`, after `DAMAGE_DOWN` as June does) and the RT_BOUNDING case refuses on it, in June's order. The script key sits in `INIT_COMPONENT` beside `FALL_DOWN` (e.g. data037 `ELSION_DEFENDER`), the table March's reads use. June's six other reaction cases that test it -> **C3i**. The `[ebp+0x8ac4]` reads at `322faec0` / `322fb3dc` are in `322fa960` (quick-slot item check), where `ebp` is argument 2, the **user**: the user's own `+0x8ac4` (`31fcadd0`), not `NOT_UP_STATE`. `NOT_AFFECT_BLACKHOLE` (`+0x8ac5`; 36 June scripts; one reader `31f6f0bf`; no March key) -> C3i.
- **Routed: June's InitState fallbacks** (`327992a2-fd`: DOWN else `m_DownDamageAir`, LANDCRASH else type 0x13 else `m_DownDamageAir`, UP else `m_UpDamage`) were not ported by C3d-3. Not ported here: with them, `3275a070` (unconditional in CommonStateStart) would overwrite every NPC up-launch's speed with the bounding back speed. That reading is implausible for a shipped game, so it needs a trace and then a user decision -> **C3j**. Until then an RT_BOUNDING hit on an NPC writes `nowSpeed.y` and the stored back speed, then changes no state (`StateChange_Reaction_FIRST` returns false for `state <= 0`, `X2GUNPC.cpp:8784`). That matches neither June nor March, but nothing reaches it. Two virtuals `IsBoundingUp` / `IsBoundingLandCrash` (vslots 0x2f4 / 0x2f8) have one class-code caller each -> C3j.

**3. No data: confirmed.** `grep -rn June_Bounding_States X2Lib KTDXLIB` = 0; no file in `kom_contents/data036_kom`; `PACK OK data036` / `PACK OK data037`, both `+0 added, -0 removed, ~0 changed` vs live.

**4. Last attacker: correct, and JUNE_BUGS 141 withdrawn.** Stored type = attacker `+0x264` (game-unit type), UID = vslot 0x54, on both sides. June **does** guard the user store: `@32046db2 cmp [esp+0x38],0 / je 0x32046f2d` jumps past it. `[esp+0x38]` is the attacker (DD `+0xc` observer − 0x1a0, `@320463a9-b9`) and is written nowhere else. The `@32046e90` test that the first pass read as the attacker test is on a different pointer (`31c25fa0`'s result). The X2GUUser comment is corrected; the code was already June's.

**5. Fly counter: closed.** `m_FlyCount` = `CX2GameUnit +0xdf8` (the `fidiv` sites). June never increments it: the only stores are ctor zeroes and two damage-path resets (`3206c7db`, `327bca82`). So June counts no RT_BOUNDING hit, and no other kind either. March still counts RT_UP/FLY/DRAG_UP -> **C3k** (June wins). The twins (`DamageReactStateChange_CheckOnly`, `BOUNDING_COMBO_TEST`) stay dead.

**Hygiene.** `flagcheck JUNE_BOUNDING_STATE --worktree --files X2GUNPC.cpp X2GUNPC.h X2GUUser.cpp`: 3 PASS, 0 FAIL, 0 OTHER, 0 encoding changes, RESULT PASS. The 27 deleted lines are all C3d-3's own lines inside the flag (state-key NOT_BOUNDING read, `FLAG_JUNE_NOT_BOUNDING`, the CommonStateStart re-read with its nested `#else`/`#endif X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` rows, the ReInit block, two comment lines). The other nested `X2OPTIMIZE_*` rows (`X2OPTIMIZE_NPC_NONHOST_SIMULATION` around the NPC case's StateChange; `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` in `JuneBoundingLandCrashEventProcess`) are March's own pattern copied inside the new code, both branches present.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,451,904 B, dated 2026-10-09 15:04:24, md5 48df41bfe3fc77907e41d9e086609051` (X2Lib 0 errors). No kom: nothing to pack.

**How to see it in game.** Nothing yet: no class registers the user states (C3d-4), and no shipped DamageEffect uses RT_BOUNDING. The NPC fixes change behaviour only for an RT_BOUNDING hit.

**Open for the user:** C3j's decision, once its trace is done.

## C3d-4 - decide: how March's classes get June's bounding state tables

**Status: done 2026-10-09 (decision, no code). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3d-4.md`.

### C3d-4 - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build, no kom)

**User's choice: route 2. The character ports give the classes the tables.** No C3d-5 port phase. The nine March class scripts stay unedited bytecode.

**Why (evidence shown to the user).**
- **Nothing reaches RT_BOUNDING today.** A `grep RT_BOUNDING` over `june_src_fixed/` finds only data036 `DamageEffect`, `DamageEffect_Character` and `Enum`.
  The 3,518 June NPC/dungeon scripts shipped by ljconv (`a5f7298`) have none. June's `DamageEffect.lua` is still one of the nine kept merged (D1, `c6c8c58`).
  So the C3d-3 engine code stays dormant, and stays harmless (logged once), until the June damage effects and class scripts arrive. Those come with the character ports.
- **Route 1 would be thrown away.** The character ports replace each class script with June's, which has the three tables inline. Adding them to March's copies now (the
  `luasrc.py convert` edit, Ara's open TDUP order, and a monster-script edit to stage a hit) would test a path nothing uses.
- **A wholesale swap to June's class scripts now is not a shortcut** (the user asked). Each class script pairs with that class's C++ InitState, which looks up state tables by name:
  - March C++ asks for tables June dropped: Elsword has 35 March-only top-level tables and Raven 25, mostly the `_HYPER` awakening states the June skill rework removed.
  - June adds states March has no handler for: Elsword 70, Raven 60.
  - 121 June damage tables (Elsword 20, Raven 25) get their damage only from numerical skill data (checklist 1), so they would do 0 damage.
  - `CUSTOM_RENDER_STATE` cuts off Add's and Ara's scripts (checklist 4).
  - Method: decompile March's bytecode with `luasrc.py decompile`, then compare top-level `NAME =` sets with `june_src_fixed`.
- Routes 3 (byte-level splice) and 4 were not pursued once route 2 was chosen.

**D10 (a) amended.** The 16-bit id (C3d-1/2) and the engine half (C3d-3) stay C3d's. The registration in March's nine classes moves to the character ports.
Each port calls `JuneInitBoundingStates( "<prefix>" )` in its class InitState and brings June's tables in its class script. That is checklist 7l, amended to cover all 12 classes.

**Moved with it.** C3d-3 review's play-test legs (registration lines, a staged bounding hit) go to the first character port. The code-only part of that review is unchanged.
**User owes nothing.**

## C3e - identify: June's user-sync packet extension

**Status: done (2026-10-09). Model:** Opus 5.5, high, no code. Kickoff `kickoffs/june-effect/C3e.md`.

- **Scope:** the 15 fields June's `KDYNAMIC_UNIT_USER_SYNC::Deserialize` (`32d5be30`) reads and March's packet lacks (bit `0x400000`, and
  bits `0x10000-0x200000` / `0x1000000-0x40000000`). For each: width, the June `SyncData` member it comes from and decodes into, its
  readers, and its reach offline (the packet is sent only with UDP peers).
- **Exit:** funcmap rows `Effect C3e`, a port phase per field with a March counterpart, owners for the rest.

### C3e - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build)

The per-field table is `june2017_re/c3e_user_sync_fields.tsv`. It gives each field's bit, wire position, width, plain / protected offsets,
default, June writer and reader, meaning, reach and route. There are 15 funcmap rows `(C3e)`. Three rows are corrected: C3d-1's two
syncData bases (`31fe6a40`, `3203c3a0`) and the HitCount finalizer (`32ec2300`). Ghidra was read-only.

1. **Count and width.**
   - June adds **14 bits, not 15**: 13 plus `0x400000`.
   - Bits `0x20000` / `0x40000` / `0x80000` carry one member, a second position vector.
   - Outside the kickoff's list, bit `0x40` (`lastTouchLineIndex`) is **2 bytes** in June; March has 1.
   - Bit `0x400` is 2 bytes on both sides (`SERV_9TH_NEW_CHARACTER` ON, flagprobe).
   - Packet `+0x3a..+0x3c` is filled by the sender, but no bit serializes it.
2. **Two SyncData shapes.**
   - The lists and SetData use a plain struct of 0x94 bytes.
   - The unit's Future (`+0x7e34`) and Now (`+0x80d4`) are 0x170-byte protected structs. In them, A-D are plain and E-N are
     `KProtectedType` slots.
   - The converters `32071ea0` (push) and `320829b0` (pop) copy every member.
   - C3d-1's bases `+0x7e68` / `+0x8108` are the buff counter, not the bases. The slot offsets it gave are right.
3. **Reach (the kickoff's premise corrected).**
   - SendFrameData `3203d4d0` copies the send list into the receive list **before** the peer test, as March's SendFrameData does
     (its `m_ReceiveSyncDataList.insert`, `X2GUUser.cpp:24526` at `0836013`). So every member flows Future -> Now for the local player with no peers; only Serialize / Deserialize are skipped.
   - The AI-party bots are `CX2GUNPC`, so no non-local user exists offline, and readers gated on "not my unit" never run.
   - **Local:** N; G once a class drives it; B / I (inert without a dual body); C (harmless: no June line map exceeds 243 lines,
     717 files).
   - **Wire only:** A (its attacker lookup is user-only), D, J / K, L / M.
4. **What each one is.**
   - **A** (`0x400000`, byte): StateChange's re-simulation left Future more than one state change ahead of Now. A victim reads it to
     drop that attacker from its **per-attacker hit-stop lists**, a June change to SetStopTime (`31ffe4f0` / `31ffe750`).
   - **B** + **I**: June's dual-body system (Lu/Ciel): the second body's position and the active body.
   - **C**: the 16-bit line index.
   - **D**: per-attack-type hit counters. June never syncs them (see the `32ec2300` note).
   - **E** / **F** / **H**: class members of Lu/Ciel; of Ain, Rose and Lu/Ciel; and of Ain.
   - **G** (float): a class-skill aim slot. Its drivers are Rose's Neil the Sniper (in base code) and Rena (Grand Archer Random Shot),
     Eve, Chung and Elesis; also Ain, Rose and Lu/Ciel. Each class was confirmed by vtable.
   - **J** / **K**: two-seater and custom-sit emotion.
   - **L** / **M**: damage-number skin items (Damage Custom A / B / Critical, June `item.kim`).
   - **N**: the START / WIN / LOSE variant index. June rolls it in every class Init, re-binds the state table at StateChange (Future)
     and DoStateEndStart (Now), and lets the chat box re-roll WIN / LOSE.
5. **Wire order** (for any port): 1, 2, 4, `0x400000`, 8 .. `0x8000`, `0x10000` .. `0x200000`, `0x800000`, `0x1000000`,
   **`0x20000000` before `0x2000000`**, `0x4000000`, `0x8000000`, `0x10000000`, `0x40000000`. March's BuffSyncCount (`0x800000`)
   already follows RelativePos, so June's `0x10000-0x200000` go between those two.
6. **Routes.**
   - **C3e-1** (port): C and G's engine member.
   - **C3e-2** (port): N.
   - **C3e-3** (identify): A's per-attacker hit-stop.
   - **Checklist 7n:** G's March-class drivers.
   - **Backlog:**
     - Stage 8: B, E, F, H, I, and G's Ain / Rose / Lu/Ciel drivers, Neil included.
     - Stage 12: J and K.
     - New **Stage 20**: L and M (damage-number customization).
   - **D:** not ported. Buff 4b-ii declined ATTACK_TYPE.

**Open for the user:** none.

## C3e-1 - port: June's 16-bit line index and the class-skill float slot (C, G)

**Status: done 2026-10-09 (`a39a4ce`); play-test owed. Model:** Sonnet 5.5, high, then an Opus review. Kickoff
`kickoffs/june-effect/C3e-1.md`.

- **Scope:**
  - **C:** widen `lastTouchLineIndex` to 16 bits: `SyncData`, the packet field (bit `0x40`, 2 bytes), `_EncodeUserSyncPos` /
    `_DecodeUserSyncPos`, the sender and SetData. Read June's NPC sync twin for its width, and widen it too if June did.
  - **G:** add the member, a protected float that defaults to 0, and bit `0x200000` (4 bytes) at June's wire position, plus Future /
    Now accessors for class code.
  - No March class drives G yet; checklist 7n lists its drivers.
- **Exit:** a `/w14244` warning diff, flag off vs on (as C3d-2); the wire order checked against `32d5be30`; a regression play-test.

### C3e-1 - Done (2026-10-09, Sonnet 5.5 high)

Flags `SERV_IRUHADEV_JUNE_LINE_INDEX16` (C) and `SERV_IRUHADEV_JUNE_SYNC_CLASS_FLOAT` (G), `KTDXLIB/Always.h` (end). Commit `a39a4ce`.

**What landed**
- **C, the user sync:** `SyncData::lastTouchLineIndex` and `KDYNAMIC_UNIT_USER_SYNC::m_ucLastTouchLineIndex` (name kept) are `USHORT`. Serialize /
  Deserialize are templates, so bit `0x40` is 2 bytes with no `.cpp` edit. The sender copy and SetData already assign one to the other.
- **C, the line map (not in the kickoff):** about 140 user call sites pass `&syncData.lastTouchLineIndex` to `CKTDGLineMap::CanDown` /
  `GetLandPosition`, which took `unsigned char*`. `KTDGLineMap.h` gets `unsigned short*` overloads (no default argument, so the existing
  `(int*) NULL` callers stay unambiguous). June's own `CanDown` `31c6c310` writes a 32-bit index (`uint*`).
- **G:** `SyncData::fClassSkillFloat` (`KProtectedType<float>`, 0), `m_fClassSkillFloat`, bit `0x200000` (`1 << 21`) in the enum and in `All`
  (`ReceiveFrameData` compares the accumulated flag with `All`), Serialize compare + write, Deserialize, sender copy, SetData, and
  `CX2GUUser::SetClassSkillFloat( float )` (Future) / `GetClassSkillFloat()` (Now). Nothing in March drives it (checklist 7n).
- **Wire order** against `32d5be30`: ... `0x8000` RelativePos, **`0x200000` G**, `0x800000` BuffSyncCount. Bits `0x10000`, `0x20000`-`0x80000`,
  `0x100000`, `0x400000` and the later ones stay out, with their slots free.

**June verdicts (differs from the kickoff)**
- **Encode / decode keep a `unsigned char` parameter.** June's `32006d80` (encode) and `32000f70` (decode) both test `index & 0xff`, so the position
  codec only sees the low byte. The two call sites cast (`static_cast<unsigned char>`); recorded as `JUNE_BUGS.md` #146 (latent: no June map has > 243 lines).
- **NPC twin: not widened.** `327837c0` (type `0x16`) -> `32d5c1f0` writes a single byte (`uStack_36 = (char)param_1[0x211c]`). March's
  `KDYNAMIC_UNIT_NPC_SYNC` and `NPCSyncData` stay `unsigned char`.

**Gate**
- Build: `BUILD OK: X2/US_SERVICE/x2.exe 19,492,352 B, 2026-10-09 17:37:04`, 0 errors (built when only these 6 files were dirty).
- `/Zs /w14244 /w14242`, 37 TUs, flag off vs on (`june2017_re/c3e1_warndiff.py`, per-(file, message) counts because peers' concurrent
  edits shifted line numbers between the passes). The only differences:
  - 15 sites in `X2GUUser.cpp` int -> `unsigned char` became int -> `USHORT` (4 `const int`, 11 `int`: the PhysicProcess / SetPos writers; same count both ways);
  - 3 sites in `X2GUNPC.cpp` (the NPC copies a user's index: `pCX2GUUser->GetSyncData().lastTouchLineIndex` and two `kFrameData.syncData.lastTouchLineIndex`) now
    narrow `USHORT` -> `unsigned char`. That is the intended NPC-stays-1-byte edge; harmless for <= 243 lines.
  - `X2UDPPacket.cpp` needs `/FIstdafx.h`; checked on its own (rc 0, no new warning).
  - The `KRoomUserInfo::BONUS_TYPE` row (36 vs 35) is a peer's edit, not this phase.

**Hygiene** (`flagcheck --range a39a4ce^..a39a4ce`, both flags):
```
files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR   (OTHER = X2UDPPacket.cpp: it only carries the other flag)
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,497,472 B, dated 2026-10-09 18:00:11, md5 153e7bab5aca8b64f82c514f7c837cc5, equal to X2/US_SERVICE/x2.exe (2026-10-09 18:00:06); built at mods/june-buff-port a39a4ce + 27 uncommitted code file(s).` The exe is a peer session's build (it held the build lock), so it carries this phase's code plus that peer's uncommitted files; my own 17:37 build above was the clean, phase-only compile.

**How to see it in game:** nothing changes (no June line map exceeds 243 lines; no class drives G). Regression play-test: a dungeon run with jumps across
platforms (line changes) and a village walk; `python tools/logs.py` should show 0 EXCEPTION and the usual baseline UNHANDLED set.

**Open for the user:** none.

### C3e-1 - Review (2026-10-09, Opus 5.5 high; no code change, no build)

**Verdict: PASS, no transcription errors.** All six kickoff items were checked against the June binary (Ghidra read-only) and the source at HEAD.

1. **Codec width: June confirmed, JUNE_BUGS #146 stays.** `32006d80` @32006e54 and `32000f70` @32001044 both read `MOVZX ECX, byte ptr [ESP+0x18]` (param 4), so the
   functions read only the low byte. June's own callers agree. SetData `3201f5f0` pushes `MOVZX byte [EDI+0x12]` into the decoder (@3201f62b / @3201f64b), then copies the
   full word `[EDI+0x12]` -> syncData `+0x20` (@3201f66b). SendFrameData `3203d4d0` pushes the syncData dword `[ESP+0x240]` (+0x20) to both encode calls (@3203d6e4 /
   @3203d709; the second is B, Stage 8) and stores `SI` (16 bits) into packet `+0x12` (@3203d7c1, packet base `ESP+0x20`). March's cast at both sites = June.
2. **NPC twin: 1 byte in June too.** Sender `327837c0`: `MOVZX EAX, byte [ESI+0x8470]` -> NPC encoder `327761d0` (`param_5 & 0xff`); the packet byte at `+0x16` is
   written from `byte [ESI+0x8470]` (@327838ca). `+0x8471` / `+0x8472` / `+0x8473` are separate bools, so June's NPC syncData line index is a 1-byte member. Receive:
   `32776350` (decode pos) / `327764c0` (decode Y) read their index as `MOVZX byte`. Both callers `327c8bd0` (NPC SetData) and `3277b500` read packet `+0x16` as a byte.
   So June narrows a user's 16-bit index into the NPC byte exactly as March now does. A one-TU `/w14244` pass on `X2GUNPC.cpp` puts the 3 narrowings at
   `X2GUNPC.cpp:19901` (hitter = user), `:31241` and `:31343` (non-host push-pass: `kFrameData` of the pushing user). `:19530` / `:19957` / `:25147` are NPC <- NPC and
   unchanged. `:31235` / `:31337` pass the index to `GetLineGroupIncludesLineData( int )`, which needs no narrowing.
3. **G on the wire: matches June.** Serialize `32d5c2e0`: compare `FLD [+0x2a]; FLD [+0x2a]; FUCOMPP` (a float compare, @32d5c4a1), then writes in the order
   `0x8000`, `0x80000`, `0x100000`, **`0x200000`** (@32d5c705), `0x800000`. Deserialize `32d5be30`: `0x8000`, `0x80000`, `0x100000`, **`0x200000`** (4-byte reader
   `32d5bd60`), `0x800000`. March's flag is a DWORD (`SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER`, buff Phase 8), so bit 21 survives. `ReceiveFrameData` (`X2GUUser.cpp:24893`) gets `All`
   because the receiver seeds it: `X2Game.cpp:906` (with `0x200000` now in `All`) is the only path into `ReceiveFrameData`, and the `else` branch (`:916`) only
   skips bytes. The sender diffs against a default-constructed `kLastSync` (`NULL` only on the overflow retry), so G = 0 goes on the wire only once it changes, and
   the receiver's ctor default (0) stands in until then; `m_dwRelativePos` / `dwBuffSyncCount` already work this way. `Reset()` is `*this = ctor`, and the ctor zeroes G.
   Offline never serializes a user sync (no peers; bots are NPCs), so this is wire parity only.
4. **G in SyncData: complete.** `Init` sets 0. The sender copies it (`X2GUUser.cpp:24689`, inside the per-entry loop, before both `Serialize` calls) and `SetData`
   copies it (`:36663`). Future -> Now goes through `FrameData::operator=` -> the implicit `SyncData` copy. There are no member-wise SyncData copies elsewhere: the
   `m_dwRelativePos` / `dwBuffSyncCount` writers are the sender, SetData and the RelativePos reset, which are not G's concern. **Verify: June's verifier not located; undetermined.** In June the 12-byte
   slots are March's `KProtectedType` (`m_data`, `m_verifyData`, `m_ucTableIndex`). No non-inlined SyncData member is called on Future: no `lea ecx, [reg+0x7e34]`
   anywhere. The one `lea ecx, [ebp+0x80d4]` (@3204ecce in `3204ea20`) is the Future -> Now copy `320861a0` (protected `SyncData::operator=`), which re-encodes every
   slot. No function reads the slot set together by absolute offset either: nowState `+0x7e6c` / `+0x810c`, hp `+0x7e90` / `+0x8130` and G `+0x7ed8` / `+0x8178` hit only
   class drivers, CommonFrameMove `32063650` (plain decoded reads), the converters, accessors and the debug dump `31fdfd70`. A `this`-relative sweep
   (`+0x38`/`+0x44`/... in one function) was too noisy to settle it. **Decision: G stays out of March's `SyncData::Verify`.** That is the conservative side: leaving
   it out cannot raise a false hacking flag, and the check can be added later if June's verifier is found.
5. **Line-map overloads: same body as the `unsigned char*` wrappers**, with the same const-ness (GetLandPosition const, CanDown non-const). A `USHORT*` converts
   implicitly to neither `int*` nor `unsigned char*`, so no call site can silently pick the `int*` version. Literal-NULL callers already had to cast (int* vs uchar*
   ambiguity before this phase), and the build is clean. No file outside the warndiff's 37 TUs touches `syncData.lastTouchLineIndex`; the others go through
   `GetLastTouchLineIndex()` (returns `int`).
6. **Hygiene / placement.** `flagcheck JUNE_LINE_INDEX16 --with JUNE_SYNC_CLASS_FLOAT --range a39a4ce^..a39a4ce`:
   `files: 4 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s),
   0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. In HEAD the encode cast sits inside the `for( u )` loop (`:24657-24661`), the G copy
   right after `m_dwRelativePos` (`:24688-24690`), and the SetData lines are at `:36630-36634` and `:36663-36665`.

**Note (no change):** `SYNC_CLASS_FLOAT` depends on `JUNE_BUFF_NEW_FINALIZER`. With the latter off, the field flag is a `USHORT`, bit 21 is truncated, `All` is never
reached and every user sync packet is dropped. Both flags are on and nothing toggles them separately; if that ever changes, add a guard.

**Open for the user:** none.

## C3e-2 - port: June's START / WIN / LOSE variant re-binding (N)

**Status: ready (found by C3e, 2026-10-09); not with the C3d-3 review (the nine class files), the C3f port (StateChange) or C3e-1. Model:** Sonnet 5.5, high, then an Opus review.
Kickoff `kickoffs/june-effect/C3e-2.md`.

- **Scope:**
  - The roll `31fe22d0`, from every class Init, which replaces InitStateCommonRandom's one-time binding.
  - StateChange's re-bind `31fea640` and DoStateEndStart's re-bind `@3203c449-3203c536`.
  - The LOSE delegate `+0x8abc` (`31fd65f0` case `0xc`) and the setter `31ecd040`.
  - The chat box's WIN / LOSE re-roll (`31ec2700 @31ec645c / @31ec6679`).
  - N's SyncData member and bit `0x40000000` (last on the wire).
- **Exit:** the start / win motion varies between runs (log tag `JUNE_FX C3e-2`); the chat command identified, then ported or routed.

### C3e-2 - Done (2026-10-09, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE`, `KTDXLIB/Always.h` (end; needs `JUNE_BUFF_NEW_FINALIZER`'s 32-bit field flag, `#error` otherwise). Log tag `JUNE_FX C3e-2`.
Funcmap rows `(C3e-2)`: `june2017_re/funcmap.tsv`. JUNE_BUGS 151 / 152. Backlog Stage 21 (Photozone).

**What landed** (`X2GUUser.cpp` / `.h`, `X2UDPPacket.cpp` / `.h`; no class file touched)
- **The roll `31fe22d0`:** its stack is `RET 0xc` with three OUT strings (Ghidra dropped them), i.e. March's own `InitStateCommonRandom` signature. So the
  nine class Inits needed no edit: the body is replaced under `#ifdef / #else original / #endif`. Same draws, same order; it also stores each list's
  index in `m_mapJuneCommonRandomIndex` (June `+0x8aac`, keys `USI_START` / `WIN` / `LOSE`) and clears `m_bJuneCommonRandomByChat` (`+0x8abc`).
  The class Inits then bind the rolled START / WIN names **once**, as before (`329babf1` / `329bacc3`).
- **StateChange re-bind `31fea640`** (only caller `320348c0 @320348dc`; the 3-argument StateChange has none): `JuneRebindCommonRandomStateOnStateChange`,
  first thing after the riding-pet block of `StateChange( int, bool )`. A map hit writes Future N (only if it differs); START / WIN / LOSE then re-bind to `list[ N % size ]`.
- **DoStateEndStart re-bind** (`3203c449-3203c531`): from Now nowState and Now N, before `m_NowStateData` is copied.
- **`31fd65f0`** = `JuneUpdateCommonRandomState`: LOSE's `OnEventProcess` (`+0x44` / `+0x48`) is set to `JuneRandomLoseEventProcess` while the by-chat flag is set, else cleared
  (no March LOSE registers one), then `MakeTableReference( name, state )`. June's trailing protect-registry call (`31c594b0`) has no March counterpart.
- **N in the sync:** `SyncData::ucCommonRandomIndex` (`KProtectedType<UCHAR>`, 0), `m_ucCommonRandomIndex`, bit `1 << 30` in the enum and in `All`, change test,
  Serialize / Deserialize **after** BuffSyncCount (last on the wire), sender copy, SetData. `SyncData` has no hand-written copy, so it reaches Now by struct copy.
- **The setter `31ecd040`** = `JuneSetCommonRandomStateIndex( state, index, byChat )`, public, uncalled in March.

**The kickoff's open items, answered**
- *Does June bind once in InitState before the first re-bind?* Yes. The first bind is March's; the re-binds then use the same index, so with no setter caller a re-bind
  picks the table Init bound. The roll only happens at unit creation, as in March: **the observable START / WIN variety is unchanged**, and the log is the proof.
- *The `+0x44` / `+0x48` delegate:* OnEventProcess = thunk `320718d0` -> `31fcd0b0`. When the skin anim ends (`+0x2ac`) it calls `StateChange( 0x82, false )`, then `CommonEventProcess`.
  `0x82` is June's `*_WAIT_STATE_CHANGE` (C9h-4f, not in March). June's class InitState also registers this delegate as **WIN's** OnEventProcess, always (corrects funcmap row
  `31fd8350`, which called it a frame move): not ported, it is C9h-4g. The LOSE delegate is ported and guarded (`StateChange( 0x82 )` only if a class registered it); nothing sets the flag.
- *The chat command:* handler `31ec2700` (vtable `33458f98`) compares the typed text with string `0xabeb` (44011, WIN) or `0xabec` (44012, LOSE). It is gated on
  `g_pMain +0x8c == 0x1a` = **`XS_PHOTOZONE_GAME`** (row `31d6fd60`), which March does not have. Not ported; new backlog **Stage 21: Photozone**.

**Deviations from June (both logged / in JUNE_BUGS)**
- Empty list: June divides by zero (#152); March skips the re-bind and logs it. An empty name is skipped the same way.
- A roll miss stores the list size (#151, kept verbatim).

**Hygiene** (`flagcheck --worktree --files`, 5 files)
```
files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR   (OTHER = Always.h: it also holds a peer's uncommitted C3n flags)
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,502,592 B, dated 2026-10-09 22:58:15, md5 3bb8764c4cffdf9a699924fca9efa8e7, equal to X2/US_SERVICE/x2.exe (2026-10-09 22:58:11); built at mods/june-buff-port 5463f49 + 10 uncommitted code file(s).`
A peer's build (it held the lock) compiled this worktree; my own `build.py` then found nothing to relink. Both `JUNE_FX C3e-2` format strings are in the exe (UTF-16 search). The exe carries a peer's C3n files too.

**Play-test (log read, no koms needed)**
1. Start the offline client, log in as **jtLordKnight** (Elsword, Lord Knight, level 80), enter any dungeon, clear it. Also **jtCodeEmpress** (Eve) once if time allows.
2. Say "check logs". I read `python tools/logs.py --grep "JUNE_FX C3e-2"` (ErrorLog.txt).
- Look for: per unit creation, three `roll` lines (state 9 / 11 / 12: index, list size, table); at the dungeon start a `rebind state 9` with the same index and table, and at the clear a `rebind state 11`
  (again once from StateChange, once from DoStateEndStart). Same index and table as the roll on every pair = pass. Different table = a miss (#151). Any `list is empty` line = #152.
- Not testable here: the chat commands (Photozone) and the LOSE delegate (flag never set).

**Play-test result (2026-10-10, jtLordKnight, 5 dungeon runs; `logs.py --grep "JUNE_FX C3e-2"`): PASS.** 32 lines, 0 EXCEPTION, only the baseline `E_DISABLE_HB_CHECK_REQ` UNHANDLED.
- Rolls vary run to run: WIN `ESSI_ELK_WIN` (index 0), `ESSI_ESK_WIN` (2), `ESSI_ESK_WIN2` (3); LOSE `ESSI_ELK_LOSE` (0 / 2 in one run), `ESSI_ELK_LOSE2` (1); list sizes START 3, WIN 6, LOSE 6.
- Every re-bind repeats the rolled index and table: START 9 index 0 `ESSI_ELK_START` (4 re-binds per start); at the clear (`5:15:34`) WIN 11 index 2 `ESSI_ESK_WIN`, twice (StateChange and DoStateEndStart), equal to that run's roll.
- No `list is empty` or skipped re-bind line; no roll index reached its list size (JUNE_BUGS 151 not seen for this class; 152 not hit). Other classes not rolled in this test.

**Open for the user:** none.

### C3e-2 - Review (2026-10-10, Opus 5.5 high; two code fixes, same flag)

**Verdict: correct against June except two transcription errors, both fixed** under `SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE` (`X2GUUser.cpp` only). Neither changes anything the play-test saw.

1. **Roll `31fe22d0`: matches.** Byte counters `[esp+0x11..0x13]`, incremented only on a non-match (`JL` = `rand()%100 < acc`), so a miss stores the size; draws START, WIN, LOSE;
   then map `+0x8aac`[9 / 0xb / 0xc] with `+0x8abc = 0` after each store; `RET 0xc`. 12 callers. The Eve Init (`329ba9c0`) binds **all three**: START `@329babf1`, WIN `@329bacc3`,
   **LOSE `@329bad8e`** (the Done note / funcmap said START and WIN; corrected). March's nine Inits bind all three as well, so no class file needed an edit.
2. **StateChange re-bind `31fea640`: matches.** Map hit: BL = map[state], Future `+0x7f98` written only if it differs; miss: BL = 0, no write. One caller (`320348c0 @320348dc`, right after
   the riding-pet reset). March's 3-argument StateChange makes no nested StateChange call, which matches June (no second caller, no vtable user).
3. **DoStateEndStart: matches.** `@3203c449` runs after the old state's `EndTable` (`@3203c444`): Now N low byte, `nowState & 0xffff`, before `@3203c536`. `31fd65f0` has exactly four
   callers (`31fea640` x3, `3203c531`), so June's DoStateEndStartFuture has no counterpart. A line in the block now says C3e-2 supersedes Phase 6b's "not ported" note (a scope deferral).
4. **`31fd65f0`: fix 1.** On an empty name June logs `334730a0` and **goes on** (`@31fd662b-@31fd6641`) to the LOSE delegate and `MakeTableReference`. March returned early. Now it logs
   and goes on. `MakeTableReference( "" )` is a no-op in March: `Get` reads a nil global and it returns false before `rawseti`. Clearing LOSE's `OnEventProcess` is safe: an empty `srutil::delegate0`
   has `stub_ptr == 0`, so `!= NULL` is false, the same as `Init()` leaves it, and no March class registers a LOSE `OnEventProcess` (all nine read).
   `31c594b0` (`+0x7cfc == 1`) refreshes an anti-tamper checksum of a registered block (it finds the entry and recomputes `+0x14`). Nothing reads data through it, so leaving it out is right.
5. **Delegate `31fcd0b0`: fix 2.** The body matches (`disasm_range.py`: `+0x8e90 ? +0x8e84 : +0x87d0`, byte `+0x2ac == 1` -> vslot `0x5c( 0x82, 0 )`, `jmp` vslot `0x7d4`). But the port's guard
   `m_StateList.find( 0x82 )` is always true in March: `USI_END` = 0x5b (`enumprobe.py`), so 0x82 is an ordinary class state, and a by-chat LOSE would have jumped into it.
   June's `*_WAIT_STATE_CHANGE` has no March id, so the state change is now not made (only `CommonEventProcess`); C9h-4g re-points it once the state has an id. It was unreachable either way (nothing sets the flag).
6. **N on the wire: matches.** Serialize `32d5c2e0`: change test `byte [prev+0x75] != [this+0x75]` -> `0x40000000` (`@32d5c55c`), written last (`@32d5c792`, 1 byte). Deserialize `32d5be30`:
   `0x40000000` last (`@32d5c1ba`, packet `+0x75`). The flag is a DWORD, `All` includes the bit, ctor / `Reset()` (= ctor) zero it, the sender copies it, `SetData` copies it, and Future -> Now goes through `FrameData::operator=` (whole `SyncData`).
7. **Play-test log (re-read): PASS as recorded.** 32 lines, 0 EXCEPTION. **JUNE_BUGS 151 / 152 cannot fire on March data:** all 165 START / WIN / LOSE lists in the nine
   data036 class scripts (read-only decompile) sum to exactly 100, and none is empty.
8. **Routing.** Backlog Stage 21 is correct (it depends on C9h-4g for state 0x82). **The C9h-4g kickoff lacked the WIN item:** `320718d0` is WIN's `OnEventProcess` in all 12 June class Inits
   (xrefs; `push 0xb` after it, 3 spot-checked). Added there as item 4: the engine delegate is ours, its WIN registration and the 0x82 target go with `*_WAIT_STATE_CHANGE`.

**Hygiene.** Phase range before the fix (`--range f3f13d0^..HEAD`): `files: 4 PASS, 0 PASS~, 1 DEFINE, 6 OTHER (C3n bfadac6), 0 FAIL, 0 ERROR`, `0 deleted`, `RESULT: PASS`.
Fix (`--worktree --files X2Lib/X2GUUser.cpp`): `1 PASS`, `11 deleted line(s)` = this flag's own earlier lines, `RESULT: PASS`. Post-commit (`214dec9`): `--range f3f13d0^..HEAD` `5 PASS, 1 DEFINE,
6 OTHER, 0 FAIL`, `RESULT: PASS` (the same 11 own lines); against the pre-phase base (`--base f3f13d0^ --at HEAD`): `X2GUUser.cpp PASS +141`, **`0 deleted line(s)`**, `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,502,592 B, dated 2026-10-10 05:31:24, md5 4a9c0c237b66046b655ea936d3781040, equal to X2/US_SERVICE/x2.exe (2026-10-10 05:31:18); built at mods/june-buff-port 13ed5f2 + 1 uncommitted code file(s).`
The new log string is in the exe (UTF-16), and the old one is gone.

**Play-test (regression, optional):** one jtLordKnight dungeon run: the same `roll` / `rebind` pairs as before. **LOSE cannot be play-tested:** it is entered only from
`X2PVPGame.cpp:531` -> `Lose()` with HP > 0, and the offline client has no PvP yet (user, 2026-10-10). The LOSE re-bind is settled by the code reading in item 4. **Open for the user:** none.

**Regression result (2026-10-10, jtLordKnight; exe md5 ca7fe237, a peer's build at `4720231`, which includes this fix): PASS.** 21 lines, 0 EXCEPTION, only the baseline
`E_DISABLE_HB_CHECK_REQ` UNHANDLED. Dungeon run rolled START 0 `ESSI_ELK_START`, WIN 1 `ESSI_ELK_WIN2`. It re-bound START index 0 four times at the start and WIN index 1 `ESSI_ELK_WIN2` twice at
the clear (`6:10:07`), equal to the roll. There was no `empty table name` line. An Ara roll (`ASI_AYR_*`, sizes 3 / 6 / 6, indices in range) shows a second class rolling.

## C3e-3 - identify: June's per-attacker hit-stop and the state-skip flag (A)

**Status: ready (found by C3e, 2026-10-09). Model:** Opus 5.5, high, no code. Kickoff `kickoffs/june-effect/C3e-3.md`.

- **Scope:**
  - June's SetStopTime / SetStop2Time (`31ffe4f0` / `31ffe750`): the signature `( float, ?, CX2GameUnit* attacker, bool force )`, the
    vslots, and every user and NPC caller against March's `SetStopTime( float )`.
  - The protected byte `+0x7dc8`.
  - The lists `+0x85a4` / `+0x85b4` and DoFrameMove's tick / erase `@3206fa63-3206fd42`, with what `+0x7df4` / `+0x7df8` are.
  - A's writer `@32034b37-32034b4a` and its clear `@31fe6aab`.
  - The NPC twin.
- **Exit:** funcmap rows `Effect C3e-3` and a port phase (row, flag, kickoff); class callers go to the checklist.

### C3e-3 - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build)

There are 9 funcmap rows `(C3e-3)`. The caller table, March to June, is `june2017_re/c3e3_stop_time_callers.tsv` (25 rows). Ghidra was read-only.

1. **Signatures.**
   - **User vslots and arguments.** The user's SetStopTime is vslot `0x100` and SetStop2Time is `0x104`; GetStopTime is `0x108`. Both setters take
     `( float, bool bDamageDataStop, CX2GameUnit* pAttacker, bool bForce )` (`ret 0x10`).
   - **What the user bodies do.**
     - They ignore arg 2.
     - The list entry is `{ float fTime; __int64 attackerUID }`. C3e's `?` is int64 padding.
     - The UID is -1 unless the attacker is a `GUT_USER`.
     - The early return needs `IsMyUnit`, the protected bool at `+0x7dc4` (the new state key `TIME_STOP_IMMUNE_STATE`, read in
       CommonStateStart) and `!bForce`. SetStop2Time ignores `bForce`.
     - The linemap `max` is unchanged.
   - **NPC overrides** (`32772aa0` / `32772c10`).
     - Arg 2 picks which immunity applies: `IMMUNITY_DAMAGE_DATA_STOP_TIME` (June-only, `+0x8e82`) when true, March's `IMMUNITY_TIME_STOP` when false.
     - The NPC still writes its stop time directly. It has no lists.
   - **Ain** multiplies `t` by its `+0x97ac` and forwards.
2. **Callers** (tsv).
   - Unit hit paths pass `bDamageDataStop = true`: user and NPC DamageReact and AttackResult. Everything else passes false, CEffect::AttackResult included.
   - Only StopOtherUnit (the attacker is `pNonStopUnit`) and StopAllUnit (the attacker is `pMyUnit` for the others) pass an attacker. Every other
     caller passes NULL.
   - VioletMage's binding circle is the only `bForce = true` caller.
   - Class sites go to checklist **7p**: Chung and VioletMage. Elsword's 1.5 s throw stop was not found in June: no `+0x100` call
     loads 1.5. The class port confirms it.
3. **The tick** (OnFrameMove `3206eaf0 @3206fa63-3206fd7c`). C3e's "DoFrameMove 3206d770" is InitSocketData.
   - **Where.** It runs in the IsLocalUnit block, before the stop gate.
   - **Each entry.** It counts down to 0 and stays in the list. It is erased only when its attacker, a user found by UID, has Now A set, or by
     the trim. The trim fires above 1000 entries and keeps the newest 500.
   - **The two Future fields.** `+0x7df4` / `+0x7df8` are Future fStopTime / fStop2Time (GetStopTime proves it). Each frame, each one is set to
     the newest entry's time, or decremented when its list is empty.
   - **The gate.** It no longer decrements: March's else-branch decrement is gone.
   - **A.** Set in StateChange's receive-replay loop when Future stateChangeNum > Now + 1 (signed bytes); cleared in DoStateEndStartFuture.
4. **Reach offline.**
   - **What the local player feels.**
     - Every hit-stop on the local player, taken or dealt, ends **one frame earlier**: about `ceil(t/dt) - 1` frames against March's `ceil(t/dt)`.
       The entry is ticked once before the gate sees it.
     - Nothing else changes for the player, because the newest entry gives March's "last SetStopTime wins".
     - A direct write of Future fStopTime (`UnitCondition::Init`, a FrameData copy) lasts one frame at most while an entry is above 0.
   - **The A erase never fires.**
     - NPC attackers are stored as -1.
     - The local player's StopAll / StopOther victims are NPCs, which keep no lists.
     - The bots are NPCs.
   - **The visible change is on monsters.** Counted by value from `june_src_fixed` over the shipped data037 scripts:
     - 241 set `IMMUNITY_TIME_STOP = TRUE` alone. They now do hit-stop on damage, being hit and hitting; March gave them none. For example
       Armadillo_King, Ancient_Bone_Dragon and Bater_FD_Boss.
     - 77 set both keys TRUE (summons, the Elysion bosses), so nothing changes for them.
     - None set `IMMUNITY_DAMAGE_DATA_STOP_TIME` alone.
   - **`TIME_STOP_IMMUNE_STATE` (76 June class states) is inert** until the class ports bring the keys.
5. **Found, not owned.** June's DamageReact gives a super-armored unit a short fixed hit-stop: 0.05 / 0.10 / 0.15 s by damage type, with a 0.5 s
   cooldown at `+0x68d0`, for user and NPC. -> **C3e-5** (identify, then port), straight after C3e-4: until then that call site has June's
   arguments under March's condition.

**Routes:**
- **C3e-4:** port everything above, with A.
- **C3e-5:** identify, then port, the super-armor hit-stop.
- **Checklist 7p:** the class sites.
- **Backlog Stage 8:** Ain's override.
- **Not ported:** CX2PVEGame's override and the GT_PVE team skip, which need June's PVE game.

**Open for the user:** none.

## C3e-4 - port: June's per-attacker hit-stop, the stop immunities and A

**Status: code done 2026-10-12, play-test pending (found by C3e-3, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3e-4.md`. Flag `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS`.

- **Scope:** everything C3e-3's Done note and `c3e3_stop_time_callers.tsv` route to C3e-4:
  - the 4-argument SetStopTime / SetStop2Time on CX2GameUnit / CX2GUUser / CX2GUNPC;
  - the user lists and the OnFrameMove tick that replaces March's gate decrement;
  - `TIME_STOP_IMMUNE_STATE` (user) and `IMMUNITY_DAMAGE_DATA_STOP_TIME` (NPC);
  - the June arguments at every engine caller (tsv rows 1-2, 4-9, 11-19);
  - A (SyncData byte, bit `0x400000`, 4th on the wire) with its StateChange writer and DoStateEndStartFuture clear.
- **Not with:** C3e-1 / C3e-2 (the same packet), the C3f port (StateChange), the C3g review (X2GUUser.cpp / X2GUNPC.cpp), C3e-5 (the same
  DamageReact call sites).
- **Exit:** built and deployed. The play-test shows:
  - hit-stop on an `IMMUNITY_TIME_STOP`-only boss, which March gave none;
  - none on a both-keys monster;
  - the player's own hit-stop as before.

  Write `C3e-4-review.md`.

### C3e-4 - Done (2026-10-12, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS`, plus `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS_DEBUG` (play-test lines; the review turns it off). Ghidra spot-checks:
`31ffe4f0`, `31ffe750`, the tick in `3206eaf0`, `31fe6a40`, `320348c0` and `32034c00`. All matched the C3e-3 funcmap rows.

1. **Signature.** CX2GameUnit's pure virtuals and the user / NPC overrides take `( float, bool bDamageDataStop = false, CX2GameUnit* pAttacker = NULL,
   bool bForce = false )`. There are no other overrides and no Lua binding.
2. **User.**
   - SetStopTime / SetStop2Time push `{ t, UID or -1 }` into `m_vecJuneHitStop` / `m_vecJuneHitStop2` and no longer write fStopTime.
   - The linemap max is kept.
   - The early return is `IsMyUnit && TIME_STOP_IMMUNE_STATE && !bForce`; Stop2 has no force term.
   - `TIME_STOP_IMMUNE_STATE` is read in CommonStateStart, before `DoCustomFunctionByBuffInCommonStateStart` (June `@32060990`, after the
     `@320606a9` sound stop).
   - **Deviation:** June keeps the bool at `+0x7dc4`, the tail of the Future StateParam. March keeps it as a standalone `KProtectedType<bool>`
     member. Nothing copies that slot after init, so the behaviour is the same.
3. **Tick.** `JuneTickHitStopList` runs per list in OnFrameMove's IsLocalUnit block, before the gate.
   - Trim 1000 -> 500. Each entry counts down, clamped to 0, and is kept at 0.
   - Erased when the attacker user's Now A is set; the stop time drops to 0 if that empties the list.
   - Afterwards the stop time is `back()`, or the old countdown when the list is empty.
   - Then the gate `0 < a || 0 < b`, with no decrement.
4. **NPC.** The return test is `bDamageDataStop ? IMMUNITY_DAMAGE_DATA_STOP_TIME : IMMUNITY_TIME_STOP`. The new key is read right after March's
   key (default false) and cleared in the ctor.
5. **Callers.**
   - Rows 1, 2, 6, 9 and 13 pass `true`.
   - Row 16 passes `pNonStopUnit`.
   - Row 17 passes `pMyUnit` to the non-self units (4 sites).
   - Rows 4, 5, 7, 8, 11, 12, 14, 15, 18 and 19 already make June's call through the defaults, so they are unedited.
   - Rows 3 and 10 are C3e-5's; the March condition is untouched (rows 1 and 9 have June's arguments under it). Rows 21-23 go to checklist 7p,
     row 24 to Stage 8.
6. **A.**
   - Sync bit `1 << 22`, 1 byte, serialized straight after stateChangeNum; included in `All`.
   - SyncData `bJuneStateSkipped` (Init false), the SendFrameData copy and SetData.
   - The writer is in `StateChange( int, bool )` only, after DoFrameMoveFuture and before the push. The sub-state overload `32034c00` has no test
     (new funcmap row `(C3e-4)`).
   - Cleared in DoStateEndStartFuture after the stateChangeNum store.
   - Guarded by `#error` without `JUNE_BUFF_NEW_FINALIZER` (the 32-bit flag).
7. **JUNE_BUGS 183-190:**
   - 183: entries never drop at 0.
   - 184: the newest entry wins.
   - 185: Stop2 ignores bForce.
   - 186: CEffect::AttackResult passes false.
   - 187: the NPC damage-data stop skips IMMUNITY_TIME_STOP.
   - 188: A is set in one overload only.
   - 189: remote users' lists are unbounded.
   - 190: the Future STOP_TIME sees the previous state's immune key.

**Expected in play.**
- The player's hit-stop ends one frame earlier: the entry ticks once before the gate.
- The 241 IMMUNITY_TIME_STOP-only monsters now hit-stop when struck, but only outside super armor: March's `IsSuperArmor() != true` gate stays
  until C3e-5. They also hit-stop on their own melee hits that carry a STOP_TIME_ATT; Ancient Phoru's script sets none.
- Both-keys units are unchanged.
- The A erase cannot fire offline.

**Hygiene.** `flagcheck JUNE_HIT_STOP_LISTS --with JUNE_HIT_STOP_LISTS_DEBUG --worktree`:
- `files: 8 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 3 #else/#endif without the flag token, 0 encoding change(s)`.
  The three are the `#error` guards' bare `#endif`s in Always.h, the same pattern as C3f-1.
- `RESULT: PASS`

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,575,808 B, dated 2026-10-12 05:48:11, md5 e44986c4fec29bdd4e9a34f2a74e73e1, equal to
X2/US_SERVICE/x2.exe (2026-10-12 05:48:06)`. The strings `JUNE_FX C3e-4 npc` / `user`, `TIME_STOP_IMMUNE_STATE` and
`IMMUNITY_DAMAGE_DATA_STOP_TIME` are in the exe.

**Save.** Freils (uid 18, Chung Tactical Trooper) was given skill 6225 `SI_A_CTT_AUTOMATIC_MORTAR` (spawns `NUI_REMOTE_MORTAR`, both keys) for
check 2. Backup: `db_backup/els_db_20261012_054505_pre_sql.sql`.

**How to see it in game** (log `python tools/logs.py --grep "JUNE_FX C3e-4"`):
- **Check 1** (corrected after play-test 1). Geyser Aluminum (`NUI_GEYSER_ALUMINUM` -> `GEYSER_ALUMINUM.lua`, IMMUNITY_TIME_STOP only in the shipped
  bytecode; WAIT / WALK / DAMAGE states have no super armor), Ranox Geyser Expert (33290), `/stage 2` (`Dungeon_Ranox_Geyser_Expert.src.lua:1169`).
  Expect `npc SetStopTime: GEYSER_ALUMINUM.lua ... dmgData 1 ... -> applied` and a visible hit freeze.
  - The first target was wrong. The shipped NPCTemplet binds `NUI_ANCIENT_PPORU` to `AncientPporu.lua`, which has no immunity key.
    `ANCIENT_PPORU.lua` belongs to another NPC.
  - The 220 IMMUNITY_TIME_STOP-only monsters by real binding were computed from the shipped `kom_contents/data037_kom/NPCTemplet.lua` plus a
    bytecode scan. Expect `npc SetStopTime: ANCIENT_PPORU.lua
  ... dmgData 1 ... -> applied` and a visible hit freeze.
- **Check 2.** Remote Mortar (both keys; shipped bytecode checked).
  - Its own shell has `STOP_TIME_ATT = 0` (`DamageEffect.src.lua:97670`), so its attacks log nothing in either build.
  - Only a monster striking it can show `-> blocked by IMMUNITY_DAMAGE_DATA_STOP_TIME`.
  - No line means untested, not failed. Its visible behaviour (no stop) is March's either way.
- **Check 3.** User `pushed` lines; feel unchanged; no `refused` / `tick` lines.

**Play-test 1 (2026-10-12, Freils, exe e44986c4; logs read).** 0 EXCEPTION; UNHANDLED is only the 2x `E_DISABLE_HB_CHECK_REQ` baseline; clean exit census.
- **Check 3 passed in the log:** 23 user `pushed` lines (t 0.07 / 0.133 / 0.17 / 0.25; dmgData 1 from AttackResult / DamageReact, one dmgData 0),
  no `refused`, no `tick`. Lists grow as JUNE_BUGS 183 says (117 entries by the end of the run).
- **Check 1 not tested.**
  - The Ruben swamp boss runs `AncientPporu.lua`, which has no key, so its lines were filtered out.
  - The secret-dungeon boss `SECRET_MAGI_BOSS` (TS-only) died on its first struck hit.
  - Its two `dmgData 0 -> blocked by IMMUNITY_TIME_STOP` lines (a StopAllUnit from the player's skill) show the NPC key read and the non-damage-data
    branch working as June has them.
- **Check 2 not tested:** no monster struck the mortar.
- Re-test with Geyser Aluminum above.

**Play-test 2 (2026-10-12, Freils, Ranox Geyser Expert stage 2, same exe; logs read): PASSED.** 0 EXCEPTION; UNHANDLED is only the 2x
`E_DISABLE_HB_CHECK_REQ` baseline; clean exit census.
- **Check 1 passed:** `GEYSER_ALUMINUM.lua` (keys TS 1 / DD 0) logged 46 `dmgData 1 -> applied` (t 0.067-0.35, plus a 0.10 multi-hit run). March
  returned on IMMUNITY_TIME_STOP for every one of them.
- It also logged 37 `dmgData 0 -> blocked by IMMUNITY_TIME_STOP`: the StopAllUnit / StopOtherUnit half / full pairs from Freils' skills, as June has
  them.
- **Check 3:** 59 user `pushed`, 0 `refused`, 0 `tick`.
- **Check 2:** still untested (no monster struck the mortar). Its outcome is March's by construction: both keys block every path.

**Open for the user:** none.

### C3e-4 - Review (2026-10-12, Opus 5.5 high; one code fix, same flag)

**Verdict: correct against June except one gap, fixed** under `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS` (`X2GUUser.cpp` ReInit). `_DEBUG` is off.

1. **User setters `31ffe4f0` / `31ffe750`: match.** Return on `vslot 0x198 (IsMyUnit) == 1 && protected +0x7dc4 == 1 && [esp+0x24] (bForce) == 0`. Stop2 returns without
   the bForce test. UID: NULL or `+0x264 != 0` -> `-1`, else vslot `0x54`. The 16-byte entry is pushed and fStopTime is not written. The linemap gets `max( t, lm )`.
   **Arg 2 (`[esp+0x1c]`) is never read**; the later `[esp+0x28]` / `[esp+0x24]` are a scratch reuse of arg 4's slot.
2. **Tick `@3206fa63-3206fd7c`: matches.**
   - Trim: `size > 0x3e8` -> erase( begin, end - 0x1f40 ), so the newest 500 are kept.
   - Each entry: `t - fElapsed` (`[ebp+0x10]`), and `t < 0` stores 0. An entry at 0 is kept.
   - Erase: game vslot `0xa0` (user by UID) and `[user+0x80da]` (Now A) != 0. The iterator is not advanced, and an emptied list writes 0.
   - Then `back()` at `[end-0x10]`, or the countdown clamped to 0 when the list is empty.
   - Gate `0 < +0x7df4 || 0 < +0x7df8` -> `+0x7e57 = 1`. Else 0, then `31fe6a40`, then `320347d0`. No decrement.
3. **CommonStateStart `@32060990-32060b96`: matches**, with one gap found and fixed.
   - The xor string at `3379d464` decodes to `TIME_STOP_IMMUNE_STATE`. A miss leaves 0, and the bool is stored through the checker only when it changed.
   - **Gap:** the Done note's "nothing copies that slot after init" missed ReInit. June's `StateParam::Init` `320820e0 @320825a9` sets the slot (`StateParam +0xb0`) to false, and
     `FrameData::Init` `320878e0` calls it from ReInit `32022380` in every game-type branch. That is Future `0x7d14 + 0xb0 = 0x7dc4`, the slot the setters read.
     March's standalone member was cleared only in the ctor. **Fix:** `m_bJuneTimeStopImmuneState = false` next to each of ReInit's three `m_FrameDataFuture.Init()`
     (dungeon, battle field, default). Every June branch (types 2, 3 (two paths), 4, default) inits Future, so no June path is missing in March.
     There is no visible effect today, because no March class script has the key.
   - **Other access, absolute sweep** (`dispsweep 7dc4 7dc8 7dcc 8064 8068 806c`): only the two setters and CommonStateStart. `push 0x7dc4` in `3224d0f0` is a string id.
   - **Other access, pointer-relative sweep** (`lea r,[X+0xb0]` with `[X+0xb4]` / `[X+0xb8]`): StateParam::Init, and `StateParam::operator=` `32081a30`.
     That operator runs only Now = Future (`3204ecaf`) and Dead = Now (`320878a0` from CommonStateStart `@3205e934`).
     The other protected hits are floats or ints on other objects (`32a33b20`, `32a34a70`, `3235eb20`, `320816d0`).
     Nothing reads Now's `+0x8064`, and there is no Future = Now copy. Funcmap row `(C3e-4 review)`.
4. **NPC `32772aa0` / `32772c10`: match.** `[esp+8] ? +0x8e82 : +0x8e81 == 1` -> return, then a direct `+0x8414` / `+0x8418` write (no list).
   - `IMMUNITY_DAMAGE_DATA_STOP_TIME` (`lea +0x8e82 @327aa500`, miss -> 0) is read right after `IMMUNITY_TIME_STOP` (`@327aa28a`). The note's `@327aa330` is its string-init branch.
   - The ctor clears both (`@32762952`).
   - `dispsweep 8e81 8e82` finds no other access.
5. **Callers: match `c3e3_stop_time_callers.tsv`.**
   - Rows 1, 2, 6, 9 and 13 pass `true`. Row 16 passes `pNonStopUnit` (2 sites).
   - Row 17: the self call (`pUnit == pMyUnit && fMyStopTime > 0`) keeps the defaults, and the 4 other sites pass `pMyUnit`.
   - All other unit callers use the defaults: user rows 4, 5, 7 and 8, NPC rows 11, 12 and 14, CEffect row 15, and the Stop2 helpers 18 and 19.
   - The 5 `X2GameUnit.cpp` `IsDamagedReaction` hits are effect / mesh / particle `SetStopTime`. Class rows 21-23 stay on checklist 7p (VioletMage's `bForce = true` included).
   - No new unit caller since `d180fa9`.
6. **A: matches.**
   - Deserialize `32d5be30`: bit `0x400000` 1 byte to `+0x9`, between `0x4` (`+0x8`) and `0x8`. Serialize `32d5c2e0`: `[prev+9] != [this+9]`, and a NULL prev sets the bit.
   - June's `All` is `0x7fffffff` (`322ed8dd`), which includes the bit.
   - Writer `@32034b37-32034b4a`: `movsx [+0x7e39] > movsx [+0x80d9] + 1`, after `320347d0`, before the push.
   - Clear `@31fe6aab`, right after the `+0x7e39` store. `SyncData::Init` (`320878e0`, `[+0x126]`) clears it too, as March's Init does.
7. **Play-test.** The current logs are a later 19:31 run on the same exe (e44986c4). It shows 2 user `pushed` lines, 0 `refused`, 0 `tick`, 0 EXCEPTION, the baseline UNHANDLED,
   and a clean census. The Geyser Aluminum counts (46 applied, 37 blocked, 59 pushed) come from the play-test 2 record in `0372079`, not from a fresh read.
8. **Hygiene.** Fix (`--worktree --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h`, `--with JUNE_HIT_STOP_LISTS_DEBUG`): `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL,
   0 ERROR`. `1 deleted line(s)` is the commented-out `#define ..._DEBUG`. `0 non-ASCII`, `0 EOL-mismatched`, `3 #else/#endif without the flag token` (the `#error` guards), `0 encoding change(s)`,
   `RESULT: PASS`. The post-commit range is in STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,573,760 B, dated 2026-10-12 19:52:22, md5 20926ca0efbb96b84e3dcfa9f0f44e3e, equal to X2/US_SERVICE/x2.exe (2026-10-12 19:52:17);
built at mods/june-buff-port cca7a97 + 2 uncommitted code file(s).` The exe no longer has `JUNE_FX C3e-4`; `TIME_STOP_IMMUNE_STATE` and `IMMUNITY_DAMAGE_DATA_STOP_TIME` are present.

**Play-test (regression, optional):** any dungeon run with Freils (uid 18, Chung Tactical Trooper): hit-stop feels as before, a death and revive behave as before.
There are no log lines now (DEBUG off). **Open for the user:** none.

**Regression run (2026-10-12 20:01, Freils, exe 20926ca0; logs read): PASS.** Ranox Geyser Expert (33290) was played through 3 stages with 32 monster kills. There were 0 EXCEPTION, only the 2x `E_DISABLE_HB_CHECK_REQ` UNHANDLED, and a clean exit census. No player death or revive packet was logged, so the ReInit path was not exercised in play; it rests on the code reading in item 3.

## C3e-5 - identify, then port: June's super-armor hit-stop in DamageReact

**Status: ready (found by C3e-3, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3e-5.md`. Flag
`SERV_IRUHADEV_JUNE_SUPER_ARMOR_HIT_STOP`. Runs after C3e-4.

- **Scope:**
  - User DamageReact `320462a0 @32046c7e-32046d4c` and NPC `327b1f30 @327b3160-327b31d5`.
  - The jump tables `3204a7bc` / `327b536c`, which give 0.05 / 0.10 / 0.15 s by DamageData `+0x28`.
  - The cooldown `+0x68d0`: set to 0.5, ticked in CX2GameUnit::OnFrameMove `3229ecc0 @3229f501`, written in `322a2700`.
  - The user guard: protected byte `+0x96f0` forces DamageData `+0x28 = 0x14` and fStopTimeDef = 0.
  - March's conditions on the same call: `IsSuperArmor() != true`, `RT_DUMMY_DAMAGE` and `fStopTimeDef > 0`.
- **Exit:** funcmap rows `Effect C3e-5`, then the port (no partial update), built and deployed. Play-test: a super-armored monster stops
  briefly when hit. Write `C3e-5-review.md`.

### C3e-5 - Done (2026-10-12, Opus 5.5 high)

Commit `53cb821`. Flags: `SERV_IRUHADEV_JUNE_SUPER_ARMOR_HIT_STOP`, `SERV_IRUHADEV_JUNE_DAMAGE_REACT_ORDER` (new: a June reorder this phase
found) and `SERV_IRUHADEV_JUNE_SUPER_ARMOR_HIT_STOP_DEBUG` (play-test lines, which the review turns off). Funcmap rows `(C3e-5)` hold the RE.
JUNE_BUGS 191.

1. **Conditions.**
   - DD `+0x24` = reActType, `+0x28` = reActResult, `+0x420` = fStopTimeDef, `+0x446` / `+0x447` = bCanRevenge / bCanReflexMagic.
   - User: inside the CREATE_DAMAGE_EFFECT / CREATE_EFFECT_SET case, reActResult != 7 and fStopTimeDef > 0.
   - NPC: at the general site, reActResult != 7 and != 0, with no fStopTimeDef test.
   - Then `IsSuperArmor(0) && cooldown <= 0` gives the short stop and starts the cooldown at 0.5.
   - Otherwise the stop is fStopTimeDef. **This includes a super-armored unit inside its cooldown, so the kickoff's "no second stop within 0.5 s"
     was wrong; JUNE_BUGS 191.**
   - SetStop2Time( 0 ) runs on every path. The NPC then sets `m_fCoolTime` (`+0x864c`, dispsweep-confirmed) = 0, so a super-armored NPC's AI
     cooldown is now reset by every hit.
   - The user's general hit-stop (`@3204831e`) keeps March's `IsSuperArmor() != true`.
2. **The "guard" `+0x96f0`** is `m_bCanAvoidance`, a CX2GUUser member written only by `X2GUAra.cpp`. It is March's own
   `else if( bRevenge && !bCanRevenge && m_bCanAvoidance )` with June's `GetUnitType() == UT_ARA` added (`31e4d0e0`). It is not class-owned, so
   it was ported and needs no checklist row.
3. **Tables** (user and NPC identical, index reActResult - 1):
   - 0.05: SMALL, FLIP_SAME_DIR.
   - 0.15: DOWN, BOUNDING.
   - 0.10: BIG, UP, FLY, REVENGE, GROGGY, GUARD, GRAPPLE, DRAG_UP, REFLECT.
   - No stop and no cooldown: 0, DUMMY, SENSOR, CREATE_DAMAGE_EFFECT, CREATE_EFFECT_SET, THROW, and 19 up.
   - March's REACT_TYPE numbering equals June's (`static_assert`).
4. **Cooldown `+0x68d0`.** June-only (March has nothing like it). The ctor sets 0. The tick follows CheckAndUpdateAdditionalStat:
   `if cd > 0 { cd -= dt; clamp 0 }`. There is no other June reference. Ported as `CX2GameUnit::m_fJuneSuperArmorHitStopCoolTime` plus
   `JuneDamageReactHitStop`, which both DamageReacts call.
5. **Reorder (`SERV_IRUHADEV_JUNE_DAMAGE_REACT_ORDER`, no partial update).**
   - June's user DamageReact runs the attacker-front test, the Future copy, revenge detection and the revenge / reflect / avoidance block
     *before* the CREATE_DAMAGE_EFFECT case. After the case come the drain particle, last attacker, Absorb and back speed.
   - March's places are `#ifndef`'d, and June's copies sit after the revenge block.
   - Effects:
     - CREATE_DAMAGE_EFFECT / CREATE_EFFECT_SET hits can now be countered, reflected or avoided.
     - The Future bAttackerFront is copied for those hits and for absorbed ones.
     - The last attacker is stored before Absorb.
6. **User short stop is by construction only.** The case sees reActResult 11 / 12 (`reActResult = reActType` upstream), and those have no entry.
   So a super-armored user takes no stop there (March's result), the cooldown never starts, and JUNE_BUGS 191 cannot fire for the user.

Hygiene (`flagcheck JUNE_SUPER_ARMOR_HIT_STOP --with JUNE_DAMAGE_REACT_ORDER JUNE_SUPER_ARMOR_HIT_STOP_DEBUG --worktree`):
`files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 3 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`.
- The 3 tokenless `#endif`s are Always.h's `#error` guards, in the house pattern.
- The PASS~ is the blank line before `JuneDamageReactHitStop`.
- Post-commit `--range 53cb821^..HEAD`: the same three lines (`3 PASS, 1 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted`, `0 encoding change(s)`; `RESULT: PASS`).

Deploy: `DEPLOY OK: X2_offline.exe is 19,577,344 B, dated 2026-10-12 21:02:16, md5 312c6f7ab1fcdf94cd5adb8b5ca15a3e, equal to
X2/US_SERVICE/x2.exe (2026-10-12 21:02:11)`. The `JUNE_FX C3e-5` format string is in the deployed exe.

**Play-test hand-off** (dev console `END`, `auth_level` 3; log `python tools/logs.py --grep "JUNE_FX C3e-5"`):
- **Check 1: NPC short stop and cooldown.**
  - Where: Freils (uid 18, Chung Tactical Trooper), Ranox Geyser Expert (33290). Paste `/stage 2` and `/invin 0`.
  - Do: hit Geyser Aluminum while it winds up an attack (ATTACK_A_READY / ATTACK_A / dash end) or lies down.
  - Look for: `npc GEYSER_ALUMINUM.lua ( UID n ) super armor, cooldown out: reActResult 1|2|3 ... -> short stop 0.050|0.100|0.150` with a brief
    freeze (March: none). Within 0.5 s of it, `cooldown 0.xxx left ... -> full stop` (JUNE_BUGS 191), and a fresh `cooldown out` line later.
  - If not: no lines means it was never super armored when hit. Every later line with the cooldown pinned near 0.500 means the tick is not running
    (a port gap, not row 191).
- **Check 2: reorder regression.** Same room after `/invin 0` again (toggle off). Let the Geyser mobs hit Freils several times.
  - Look for a normal flinch and knockback, 0 EXCEPTION, and UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ` baseline.
  - If there is no knockback, suspect the moved back-speed copy.

**Play-test (2026-10-12 21:08-21:11, Freils, Ranox Geyser Expert, exe 312c6f7a; logs read): PASSED.**
- Log health: 0 EXCEPTION. UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ` baseline. The census shows a clean exit.
- **Check 1 passed:** 128 `JUNE_FX C3e-5` lines, all NPC: GEYSER_ALUMINUM 82, MULLOCK_CALL 18, MULLOCK 13, SCIENCE 13, MULMAI 2.
  - Short stops: 51, all matching the table. reActResult 1 gave 0.050 (10); 2 / 4 / 5 gave 0.100 (35 / 1 / 5).
  - Full stops inside the cooldown (JUNE_BUGS 191): 77.
  - The cooldown counts down: per UID, the `left` values run 0.500 -> 0.014, and a new `cooldown out` short stop follows (e.g. UID 1000044
    @21:11:06.229). So the NPC tick runs.
  - reActResult 3 / 15 (0.150) did not occur, and no `no entry` lines were logged.
- **Check 2:** nothing in the log shows a fault, and the user reported no flinch or knockback problem.

### C3e-5 - Review (2026-10-12, Opus 5.5 high; PASS, no code fix, DEBUG off)

Verdict: **PASS.** No transcription error was found, so there is no code change beyond `_DEBUG` off. The raw disassembly comes from
`disasm_range.py --x87`; Ghidra was not needed.
1. **Hit-stop.**
   - The user's `fldz; fcomp [DD+0x420]; test ah,5; jp` falls through only when exactly C0 is set, that is `0 < fStopTimeDef`. Equal (C3)
     and NaN (C0|C2|C3) both jump to the return, as March's `> 0.f` does.
   - Both sides' `fldz; fcomp [+0x68d0]; test ah,1; jne` take the full path when `0 < cd` or the value is NaN. The port's `cd <= 0.f` is false for
     NaN as well, so the two are the same.
   - **JUNE_BUGS 191 is confirmed from the bytes:** both `IsSuperArmor(0) == false` (`je @32046d4e` / `@327b31d7`) and the cooldown running
     (`jne`) land on the one full-stop block.
   - SetStopTime is `push 0, push 0, push 1, float`, which is `( t, true, NULL, false )`. SetStop2Time( 0, true, NULL, false ) runs on every path
     that gets past the entry tests.
   - The NPC tests only `reActResult != 7` and `!= 0` (`@327b314f-327b315d`); there is no fStopTimeDef test.
   - `m_fCoolTime` (`+0x864c`) = 0 at `@327b320d`, after SetStop2Time. The table-skip `ja 327b31f4` reaches it too, as does every other path
     inside the test, which is the port's placement.
2. **Tables.** All 18 byte-index entries of `3204a7cc` / `327b537c` and the 4-entry jump tables `3204a7bc` / `327b536c` were re-read; user and
   NPC are identical. The floats are `33436704` = 0.05, `334395e0` = 0.10, `33457f04` = 0.15 and `3343777c` = 0.5. `JuneDamageReactHitStop`'s
   cases match entry for entry. March's REACT_TYPE values 0-22 were re-read (`X2DamageManager.h:230-271`, no live `#ifdef`); they equal June's.
3. **Cooldown.**
   - The ctor's `fldz @322a511e` stays on the stack through two `fst` and reaches `fstp [esi+0x68d0] @322a5169`, so the member starts at 0.
   - The tick sits after OnFrameMoveBuff `3229d620` and CheckAndUpdateAdditionalStat `32290920`. It is `fcom` / `test ah,5` (`cd > 0`), then
     `cd -= [esp+0x140]`, then clamp at `< 0`.
   - Frame arithmetic: 3 SEH pushes, `sub 0x114`, 4 registers and the cookie make `0x134`. So `fTime` is at `+0x138` (`fld qword @3229ecfb`) and
     `fElapsedTime` at `+0x140`.
   - No instruction in `3229ecc0-3229f4ff` writes `[esp+0x140]`, so June uses the raw argument, as the port does (`X2GameUnit.cpp:1092` parameter).
4. **Order** (`320462a0 @32046783-32046fb9`). The walk:
   - Cash-shop reject, then the attacker-front dot test, then the `bIsRight` flip, then ARRANGED_FLY (`+0x444`).
   - Future copy (`@32046836`), then revenge / reflect detection, then the Rune Slayer roll (`@32046a46`), then the reflect-by-buff vector
     (`+0x679c != +0x67a0`), then the old hit type kept (`@32046ac3`).
   - The revenge / reflect / avoidance block, then the case (`@32046cae`).
   - Then drain (`+0x8a58` reset, then attacker test `@32046db2`, `+0x4f9`, attacker GUT_USER, `+0x8830 == -1`), then the last-attacker store
     (`@32046f06`), then Absorb, then back speed (`@32046f3d`).
   - That is the port's flag-on order.
   - `ADD_BACKATTACK` has no place to find: it is `#define`d nowhere in this tree, so March compiles none of its 3 blocks either (its only
     readers are `X2GUUser.cpp:9953` / `:10078`, inside the same dead macro). June has no such computation.
   - The copies match March's originals line for line: the case (`RAVEN_SECOND_CLASS_CHANGE`, live, now inside the flag's `#else`, so it is
     equivalent), the drain block and the back-speed block.
   - The copy's `GRAPPLING_TEST` RT_THROW branch is dead (`InHouseEtc.h:116` is commented out). That matches June, which has no THROW branch at
     `@32046f91`.
   - UT_ARA: `31e4d0e0` = `m_pUnit ? ( templet ? templet->m_UnitType : 0 ) : 0`, compared with 7. March `GetUnitType()` = `GetUnit()->GetType()`,
     and `UT_ARA` = 7 (`X2Unit.h:29`).
   - **Done-note item 6, restated:** after the reorder the case can also see reActResult 6, 17 or 20 from the revenge block. Every one of those
     paths zeroes `fStopTimeDef` first (`@32046b03`, `@32046ca8`), so the `> 0` test skips them. The user's short stop is still reachable by
     construction only.
5. **Found (not a C3e-5 error):** June's Rune Slayer reflect check `@32046a55-32046aa0` differs from March's NEW_SKILL_2010_11 roll.
   - With `bCanReflexMagic` set and `32d0c820()` true (skill `0x53aa` learned, skill type 5, defender buff test `3227baf0( 0x12 )`, then MP from
     skill value `0x2b` and an effect at the unit), June zeroes DD `+0x3c` (magic damage) and skips the roll.
   - This is class-keyed, so it goes to the character port (checklist 7j row; funcmap `(C3e-5 review)`).
6. **Hygiene.** `flagcheck JUNE_SUPER_ARMOR_HIT_STOP --with JUNE_DAMAGE_REACT_ORDER JUNE_SUPER_ARMOR_HIT_STOP_DEBUG --range 53cb821^..HEAD`,
   before this commit:
   - `files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
   - `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 3 #else/#endif without the flag token, 0 encoding change(s)`
   - `RESULT: PASS`
   - Post-commit range: `--range 53cb821^..HEAD` (29d3e1d): `3 PASS, 1 PASS~, 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `1 deleted` (the commented `_DEBUG` `#define`), `0 encoding change(s)`; `RESULT: PASS`.
7. **`_DEBUG` off** (Always.h, commented `#define`, via `bytepatch.py`). Deploy: `DEPLOY OK: X2_offline.exe is 19,576,832 B, dated 2026-10-12 21:30:25, md5 6de22f205f993f1c5f0979439fc24e2f, equal to X2/US_SERVICE/x2.exe (2026-10-12 21:30:20)`; the `JUNE_FX C3e-5` string is gone from the exe and `JUNE_FX C3f-1` is still in it. The deploy carries C3f-1, whose code and `_DEBUG`
   are in this tree, so its pending play-test still sees its lines.

## C3f - identify: June's force-down flush in StateChange

**Status: done 2026-10-09 (no code, no build); port -> C3f-1. Model:** Opus 5.5, high, no code. Kickoff `kickoffs/june-effect/C3f.md`.

- **Scope:** June calls the gage's UpdateFlushForceDown (`32a33dc0`) from `CX2GUUser::StateChange` `320348c0 @32034931-32034994`, when the state
  being **left** is DOWN_FRONT / DOWN_BACK / AIR_DOWN_LANDING (vslots `0x114` / `0x118` / `0x490`; Luciel overrides all three). March calls
  ForceDownAndDetonationProcess (`X2GUUser.cpp:40216`, `FIX_FORCE_DOWN_AND_DETONATION_BUG`) from CommonStateStart on **entering** DOWN_FRONT /
  DOWN_BACK / AIR_DOWN_INVINCIBLE / RIDING_DAMAGE_*, together with UpdateDamageFlushMP and the stun / freeze / groggy clears. Find where June put
  the detonation flush and the clears (or show they are gone). Cover the NPC twin `327577d0` against March's NPC path (`X2GUNPC.cpp:13101`) and every class override.
- **Exit:** funcmap rows `Effect C3f`; a port phase (row, flag, kickoff) that follows June and also corrects the C3c RT_DOWN comment in `X2GUUser.cpp`;
  class overrides go to the checklist.

### C3f - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build)

10 funcmap rows `(C3f)`, 2 corrected (C3d-1's `0x114 / 0x118 / 0x490` were not "wait / charge-MP ids"; the C3c review row's "open").
Ghidra read-only, raw bytes via `disasm_range.py` (two functions are not carved), E8 sweeps of `.text` for every caller.

**The C3c review's picture was half right. June did not move ForceDownAndDetonationProcess; it moved only the force-down clear.**
1. **The detonation half is unchanged.** June's ForceDownAndDetonationProcess is `3202a9c0`, still called from CommonStateStart
   (`@3206068e`, March's position: after `32275440` / `32071c30`, before `31fe32d0` and the skill-state-end checks `@320606a2`). Same
   five entry states (DOWN_FRONT / DOWN_BACK via vslots `0x114` / `0x118`, AIR_DOWN_INVINCIBLE via vslot `0x48c` = `0x24`,
   RIDING_DAMAGE_FRONT / BACK as literals `0x55` / `0x56`). It runs UpdateDamageFlushMP (`32a33d50`, = March, its only caller) and
   the same four clears (`m_vecStunPtr +0x6554`, `m_vecFreezePtr +0x6564`, `m_ExtraDamagePack.m_Stun +0x1ee8`, `m_bChangeGroggyState
   +0x91e5`). DamageFlushMp (gage vslot `0x30`, `32a31dc0`) and FlushForceDown (`32a31e10`) are March's FIX forms.
2. **The force-down half changed in two ways.** `3202a9c0` no longer calls UpdateFlushForceDown: it only *tests* the gage flag `+0xf8`.
   When it is set, it zeroes `nowSpeed.y` (`+0x308`, June-only) and runs the clears; otherwise the clears run only when the detonation
   flushed. The flag is not cleared there and the gage is not emptied. Both happen in StateChange `320348c0 @3203492b-32034994`, on
   **leaving** DOWN_FRONT / DOWN_BACK / AIR_DOWN_LANDING (`0x25`, not AIR_DOWN_INVINCIBLE `0x24`). That block sits **before** the
   IsLocalUnit and HP / shock / frozen early returns, so a refused or remote StateChange flushes too. Kickoff item 2 resolves the
   other way: CommonStateStart does have a flush-family call (`3202a9c0`), it just no longer flushes the force-down gage.
3. **The stun gate is unchanged** (CommonFrameMove `@32067528-3206753b` = March `:17915`, both flags). Because `+0xf8` now stays set
   until the unit leaves the down state, a stun cannot turn into groggy while the unit lies down (March reopened the gate on entry).
4. **NPC.** `327577d0` is **CX2GUNPC::StateChangeForce**. Its first block flushes when the (byte) `syncData.nowState` is
   `m_DownDamageLandFront` / `LandBack` / `m_DownDamageAir` (`+0x4090` / `+0x4094` / `+0x4098`), before the bounds and cool-time tests,
   so a refused change flushes too. March's NPC empties the gage instead **every frame** while in those states (CommonFrameMove
   `X2GUNPC.cpp:18472`, compiled unconditionally; `:14878` is dead under BALANCE_PATCH_20110303). June dropped that reset: no
   SetNowForceDown caller in `327b53b0`, no inline write (C3c review sweep), and the only `+0x4090` read there (`@327b9fa8`) is a death-path
   StateChange. C3i's DRSC guard (`@3276a1a0-3276a1f3`: FlushForceDown true while in one of the three -> skip the branch) is the
   other half: June keeps the gage full through the down state and stops it re-forcing.
5. **Overrides.** Luciel overrides all **four** getters (`0x114` / `0x118` / `0x48c` / `0x490`); the other 12 classes use the base ->
   checklist 7q. March's three getters (`X2GUUser.h:1987`) are non-virtual and return the base ids.
6. **Riding.** RIDING_DAMAGE_FRONT / BACK end in RIDING_WAIT / RIDING_OFF (`X2GUUser.cpp:39591`), never in a down state, so after a
   riding force-down June keeps the flag and the full gage until the unit next leaves a down state (only AttackResult empties the value).
   The port keeps that and records it in JUNE_BUGS.

Port planned as **C3f-1** (flag `SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH`, Sonnet 5.5 high, kickoff `kickoffs/june-effect/C3f-1.md`,
review `C3f-1-review.md`). User owes nothing.

## C3f-1 - port: June's force-down flush on leaving a down state (user + NPC)

**Status: code done 2026-10-12, play-test pending. Model:** Sonnet 5.5, high. Flag `SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH` (+ `_DEBUG` for the play-test lines,
ON). Kickoff `kickoffs/june-effect/C3f-1.md`.

- **Scope:** "C3f - Done" items 1-4: the user StateChange flush and ForceDownAndDetonationProcess's test-only form; the NPC
  StateChangeForce flush, the dropped per-frame reset, and C3i's DRSC nowState guard; the C3c RT_DOWN comment; JUNE_BUGS rows.
- **Exit:** builds, flag hygiene PASS, deployed; play-test shows the gage emptied on leaving the down state (`JUNE_FX C3f-1` lines),
  0 EXCEPTION.

### C3f-1 - Done (2026-10-12, Sonnet 5.5 high; code, play-test pending)

Flag `SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH` (+ `_DEBUG`, default ON) in `KTDXLIB/Always.h`; items follow the kickoff.
1. **User StateChange** (`X2GUUser.cpp`, `StateChange( int, bool )`): after the UNIT_EMOTION block, before `IsLocalUnit()`: Future nowState
   (low 16 bits) = DOWN_FRONT / DOWN_BACK / AIR_DOWN_LANDING -> `UpdateFlushForceDown()`. Pure addition (June `320348c0 @3203492b`).
2. **ForceDownAndDetonationProcess**: June's test-only form (`3202a9c0`): bDet = UpdateDamageFlushMP; flag set -> `nowSpeed.y = 0` + the four
   clears; else clears only if bDet. March body in `#else`.
2b. **NPC CommonStateStart**: three down states + gage flag -> `nowSpeed.y = 0`, placed after March's InsertBuffFactor tail and before the
   skip-start / cinematic-UI / bounding-UP blocks (June `327ac9d0 @327af1fc`).
3. **NPC StateChangeForce**: first statement flushes in the three down states (`327577d0`). Nested in FIX_FORCE_DOWN_AND_DETONATION_BUG (the
   gage methods only exist under it; so are 2 and 2b).
4. **NPC CommonFrameMove**: per-frame `SetNowForceDown( 0.f )` block in `#else`.
5. **NPC DamageReactStateChange**: `if( <three down states> ) {} else` inserted at the top of the `FlushForceDown() == true` branch, so the
   RIDING_MONSTER / ground / air chain that follows is the `else` body (insert-only; flag off = base).
6. **C3c RT_DOWN comment** (`X2GUUser.cpp:12557`): the old line said the gage "is cleared only by ForceDownAndDetonationProcess / its flush";
   it now names June's location (StateChange `320348c0 @32034994`; ForceDownAndDetonationProcess only tests the flag). Comment only, which is the
   one deleted line flagcheck reports (OTHER = SUPER_ARMOR_CONDITION).
7. **JUNE_BUGS** rows 178-181 (riding force-down never flushed; flag set for the whole down state; refused/remote change still flushes; NPC twin
   of row 131). Row 131's wording still matches (it already names StateChange as the flush site). funcmap rows now say "Ported C3f-1".
8. **Debug lines** (offline_server.log; grep `JUNE_FX C3f-1`): `user flush`, `flag set`, `npc flush`, `npc flag set`.

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH --worktree --files ... --with _DEBUG`: X2GUNPC.cpp PASS (+60), Always.h DEFINE, X2GUUser.cpp
OTHER (+51 -1: the item 6 comment, inside SUPER_ARMOR_CONDITION); RESULT PASS, 0 non-ASCII, 0 EOL-mismatched, 0 encoding changes; the one untokened
`#endif` is the `#error` guard.
`DEPLOY OK: X2_offline.exe is 19,570,688 B, dated 2026-10-12 04:10:17, md5 5fac6f82805f3c5347e77f1c5fbbadff`; the four `JUNE_FX C3f-1` strings
are in the exe.
Open for the user: nothing. Play-test below; the review kickoff `C3f-1-review.md` already exists.

---

## C3g - port: June's SET_LIMIT_MAX_G_SPEED (the per-state fall-speed cap)

**Status: code done 2026-10-09 (see "C3g - Done"). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3g.md`.

- **Scope:** found by C3d-3. A float state key (user `320506f0 @32054734`, NPC `32784c00 @32787303`) kept in the unit's protected `+0x40f8`; identify the
  default and every reader, then port the member, both parses and the readers.

### C3g - Done (2026-10-09, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_LIMIT_MAX_G_SPEED` (`KTDXLIB/Always.h`, end; no dependency). funcmap rows `(C3g)` (8 new, row `(C3d-3)` closed), JUNE_BUGS 142, checklist 7o.

**What June does.** The unit's `+0x40f8` is a protected float, 1.0 from the ctor (`322a2700 @322a43d3`). The user reads `SET_LIMIT_MAX_G_SPEED` at **every**
state start (`320506f0 @32054734`, right after `DISABLE_GRAVITY_TIME_END`, miss = 1.0), so a state without the key turns it off again. The NPC parses it
into StateData `+0x430` (`@32787303`, after `TRANSITION`) and its CommonStateStart copies it to the unit (`327ac9d0 @327ad4a0`; the kickoff's `327a0fa0`
is the same code, Ghidra merges the two functions). Both PhysicProcess gravity clamps (user `32015b30 @32016f5e`, NPC `3277fbf0 @32780462`, per adaptive
sub-step) compute the cap as **`L < 0 ? L : fMaxGSpeed * buoyancy`**. The limit is not multiplied by buoyancy or unit scale. Both clamps use the cap, with March's `<=`.
The reverse-gravity branches do not read it.

**x87, settled by running June's bytes** (two-operand harness built on `x87test.py`: ST0 = a, compare with memory b, the same flags as `ST0` vs `ST(1)`):
the reader `fldz; fcomp L; test ah,0x41; jne` falls through (limit) for -4000 / -5000 / -0.0001 and jumps (March cap) for 0 / 1 / NaN. Clamp 1
(`@32017414`, `test ah,1; jne`) and clamp 2 (`@32017447`, `test ah,0x41; jp`) both store the cap for y <= cap and keep y for y > cap or NaN.
The setter (`fucompp; test ah,0x44; jnp`) skips only when the values are equal, so it is a plain store.

**What is in.** `CX2GameUnit::m_fJuneLimitMaxGSpeed` (1.f), `Get/SetJuneLimitMaxGSpeed`, `JuneGetMaxGSpeedCap( fBuoyancy )`. The user parse is in
`CommonStateStartFuture` after `DISABLE_GRAVITY_TIME_END`, and the user cap is in `PhysicProcess`'s two clamps. On the NPC side: `StateData::m_fJuneLimitMaxGSpeed`
(1.f) is parsed after `TRANSITION`, CommonStateStart copies it after the NOT_BOUNDING copy (the `#else` twin reads the LuaManager), and the cap is in both
`X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branches. Original code in every `#else`. Log: `JUNE_FX C3g user|npc ... SET_LIMIT_MAX_G_SPEED %.1f replaces the fall
cap ( fMaxGSpeed %.1f )` per state entry with a value below 0, the first 32 per side (offline_server.log).

**Not ported, routed.**
- **The minor unit.** `@3201745a-32017861` clamps a second PhysicParam (`+0x36c`). Its passive speeds come from `MINOR_UNIT_PASSIVE_SPEED_X/Y`, so it
  is read as Lu/Ciel's second body. March has none: checklist 7o. June builds the limit for it and then clamps with a fresh `fMaxGSpeed * buoyancy`,
  i.e. throws the limit away: JUNE_BUGS 142, verbatim.
- **`APPLY_MOVEMENT_SPEED`** (`@32054181`, the new first byte of June's PhysicParam, read by the x decay). No phase owned it -> new phase **C3h**.

**June data and reach.** 32 states in `june_src_fixed`: 13 `*_DAMAGE_BOUNDING_DOWN` (12 classes, -4000), Raven `RRF_DASH_JUMP_COMBO_XX` (-5000), Add
`ANSI_APT_DASH_JUMP_COMBO_ZX` (-4000), and 17 NPC `*_DAMAGE_AIR_FALL` (-4000: `PVP_HERO_ANNONA/ARUS/BERRY/BRIGIS/DEAR/HYSLEY/LEAH/NEJI/RIBE/SUENA/TAV/
VARNES/VINTON/WAKID`, `TIME_CHUNG1-3`). The 17 ship in data037 as June bytecode (a5f7298), but March's `NPCTemplet` (kept March's) names none of
them. March's class scripts carry no such key. **So nothing sets the key today. Both halves are dormant and the play-test is a regression check.**
The user half reaches play with the character ports (checklist 7o, after C3d-4's route 2). The NPC half reaches play when a June NPC registry names those NPCs.
No staged data (user rule: no invented Lua).

**Hygiene.** `flagcheck --worktree --files` (the 6 code files): `5 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`; 0 deleted lines; 0 encoding changes. RESULT PASS.

**Deploy.** DEPLOY OK: X2_offline.exe is 19,451,904 B, dated 2026-10-09 14:53:24, md5 6b5d4f1b4c10bf8bf1a143c77e29cd4e, equal to X2/US_SERVICE/x2.exe (2026-10-09 14:53:17); built at mods/june-buff-port fdcf60b + 6 uncommitted code file(s) (the 6 are C3g's). Strings checked in the deployed exe: `JUNE_FX C3g user` and `JUNE_FX C3g npc` (UTF-16) and `SET_LIMIT_MAX_G_SPEED` present.

**In game.** There is nothing new to see. Falls (jumps, knock-ups, monsters dropping) should feel exactly as before, and `offline_server.log` has no `JUNE_FX C3g` line.

**Play-test (2026-10-09): PASSED.** (user; exe 48df41bf, a peer's rebuild from 22dc099, which contains 525a82b; run on battlefield 40032 as reyaa, Lire Grand Archer, not the handed-off dungeon 30000: user and field-monster falls unchanged; logs read: 0 `JUNE_FX C3g` lines as expected, 0 EXCEPTION, only the baseline E_DISABLE_HB_CHECK_REQ UNHANDLED).

### C3g - Review (2026-10-09, Opus 5.5 high)

**Verdict: correct against June. No code change, so no build.** Read-only: capstone over `unpacked_x2.exe` (`disasm_range.py`, `funcdis.py`), plus a
scratch x87 harness built like `x87test.py` (June's compare bytes, run on concrete floats).

1. **Parse sites.** User `@32054734`: xorstring `3379d12c` decodes to `SET_LIMIT_MAX_G_SPEED`. The `fld1` default is stored at `@320546e4`, before the
   lazy-init branch, so the first-use and cached paths both start from 1.0. A miss stores `fld1` again (`@320548a4`). The setter (`fucompp; test ah,0x44;
   jnp`) skips only on equal, which makes it a plain store. The one branch in `320506f0` that passes the parse is `@32050753`: ProcessSocketEffectEx(20) == 20
   returns. March has the same early return (the SIEE 20 `return` under `JUNE_SOCKET_PARITY` at the head of CommonStateStartFuture), and there is no other
   `return` before the parse. NPC `@32787303`: xorstring `337a9a90` decodes to the same key, right after `NOWSTATE_SET_FLAG( 0x2b )` (TRANSITION). It is a
   plain `fstp [StateData +0x430]` with miss = `fld1`, followed by the effect-set list `resize( 0 )`, as in March.
2. **NPC copy `327ac9d0 @327ad4a0`.** `327ac9d0` has its own SEH prologue, so the Done note's correction of the kickoff's `327a0fa0` stands. Two paths avoid
   the copy. `@327aca28` returns when the skin anim (`+0x8604`) is null; March has `if ( NULL == m_pXSkinAnim ) return;`. `@327ace8f` jumps into the protector
   section (`33ea82b4`, a Themida VM entry). That jump is the body of an `if` whose three skip branches all land on the join at `327acf83`, ahead of the copy (the VM's own exit is inferred from where its junk span ends at that join, not traced).
   No other branch crosses the copy. **Stale placement:** the NOT_BOUNDING copy that the kickoff and Done note put C3g's copy after was removed by the C3d-3
   review (`9946eea`: NOT_BOUNDING is an INIT key). The copy now follows the NEVER_MOVE read in both `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branches. The two
   stores are independent, so the order is moot. The funcmap row now says so.
3. **Readers and clamps.** User `@32016f5e`: on both buoyancy paths ST0 is the compiler's carried 0.0 (`fldz` re-pushed after each call: `@320169e5`,
   `@32016a52`, `@32016f28`), so `fcomp L` compares 0 with L. NPC `@32780462`: `fld buoyancy` sits outside the sub-step loop and `fldz; fcomp L` runs every
   iteration (back edge `@327809d9` -> `32780466`). Both NPC paths leave the x87 stack balanced: the limit path pops with `fstp st0` at `@3278049d`, the other
   with `fmul`/`fstp` at `@327806c1`. The NPC clamps `@32780974` / `@327809a7` are the user's two idioms (`@32017414` / `@32017447`).
   The harness gives:
   - Reader: the limit for L = -4000, -5000 and -0.0001; March's `fMaxGSpeed * buoyancy` for -0.0, 0, 1, 4000 and NaN. March's `L < 0.f` agrees, -0.0 included.
   - Both clamps: y <= cap stores the cap; y > cap or NaN keeps y; a NaN cap keeps y.

   The limit is not scaled by buoyancy. The only `+0x40f8` reads in the two functions are user `32016f5e`, user `3201745a` (minor unit) and NPC `32780466`.
   The March clamps C3g leaves alone (`X2GUUser.cpp:21997`, `X2GUNPC.cpp:25171` / `:25203`) are `REVERSE_GRAVITY_TEST` branches.
4. **Not ported, reasons hold.** For the minor unit, the limit temp is built (`@32017476`, else `@32017625` `fmul [+0x38c]`), but both clamps (`@32017819`,
   `@32017852`) use a fresh `fld [+0x38c]; fmulp` with buoyancy, so the limit is dropped. JUNE_BUGS 142 and checklist 7o stand. `APPLY_MOVEMENT_SPEED`
   (`@32054181`, a bool into `+0x300`, then `+0x36c`) went to C3h, which has its row and kickoff.
5. **Reach.** data037 `NPCTemplet.lua` is March bytecode (`1B 4B 4C 81`, unchanged since 10-01). It names 14 `PVP_HERO_*` scripts (AMELIA, APPLE, BALAK,
   CODE_Q_PROTO_00, EDAN, LIME(_EVENT), LOW(_EVENT), NOA, PENENSIO(_EVENT), Q, SPIKA). None of them is one of the 14 June names, and it names no `TIME_CHUNG`.
   The 9 March class scripts in data036 do not carry the key (Ain, Lu/Ciel and Rose do not exist in March). A byte scan of all 192 `kom_contents` dirs finds
   the key in exactly the 17 June NPC scripts. There are no loose `.lua` overrides in the game dir. The port is dormant, as the Done note says.
6. **Hygiene.** `flagcheck JUNE_LIMIT_MAX_G_SPEED --range 525a82b^..HEAD`: `5 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; 0 deleted lines,
   0 non-ASCII inserted lines, 0 EOL mismatches, 0 encoding changes. RESULT: PASS. The 3 OTHER rows are `9946eea` (C3d-3 review, `JUNE_BOUNDING_STATE`) in
   `X2GUNPC.cpp/.h` and `X2GUUser.cpp`. The nested `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` `#ifdef/#else/#endif` lines inside C3g's NPC blocks are March's own
   adaptive-frame split (ON in US_SERVICE). C3g's copy and cap appear once in each branch, and each keeps March's original code in the flag's `#else`.

The review needs no play-test; the C3g regression play-test already passed.

---

## C3h - port: June's APPLY_MOVEMENT_SPEED (PhysicParam's new first field)

**Status: done 2026-10-09 (regression play-test passed), found by C3g. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3h.md`.

- **Scope:** a bool user state key (`320506f0 @32054181`) June stores as the new first byte of its PhysicParam (`+0x300`) and copies to the minor
  unit's (`+0x36c`). PhysicProcess's x decay scales by `31e4f990()` / walk speed when it is set. Identify every reader and port the engine side; the
  minor unit's copy is Lu/Ciel's (checklist 7o). June data: one Ara state.
- **Exit:** funcmap rows `Effect C3h`; flag `SERV_IRUHADEV_JUNE_APPLY_MOVEMENT_SPEED`; regression play-test.

### C3h - Done (2026-10-09, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_APPLY_MOVEMENT_SPEED` (`KTDXLIB/Always.h`, end). funcmap: 12 new `(C3h)` rows; the `(C3g)` candidate row is closed. Checklist 7o was updated.

**What June does.**
- **Default.** The bool is the first byte of June's PhysicParam (`+0x300`). The ctor sets it to 0 (`322a2700 @322a3184`).
- **Parse.** `CommonStateStartFuture` reads it at **every** state start (`320506f0 @32054181`; a miss stores false). The read comes after
  SPEED_X/Y, PASSIVE_SPEED_X/Y and the SPEED_TIME lists. When it is set, `@3205419d-320542b9` computes `rate = GetWalkSpeed() / fWalkSpeed`
  (`31e4f990` = March's `CX2GameUnit::GetWalkSpeed()`: the walk states store it straight into `nowSpeed.x`) and scales three things:
  - `nowSpeed.x`, always;
  - `passiveSpeed.x`, unless it is `-1` (`IsSamef`);
  - every SPEED_TIME entry's `vSpeed.x`.

  So a state's scripted x speeds follow the movement-speed buffs.
- **Decay.** `PhysicProcess` (`32015b30 @320168b8` / `@32016927`) multiplies the x decay `fReloadAccel * elapsed` by the same rate, every frame.
- **Clearers.** The only other writers are the three Org -> PhysicParam whole-struct copies: Init `322a5f90`, ResetPhysicParam `322886a0` and
  RidingOffEnd `31fcde30`. Org's byte is always 0, so each copy clears the flag.
- **Not readers.** The other `+0x300` hits (a checksum, other classes' fields, NPC AI pointer arguments, `32010440`'s pointer) are not readers.
  Each one is listed in the funcmap rows.

**x87, settled by running June's bytes.** I used a popping `fldz; fcomp` harness, because `x87test.make_func` leaks one stack slot per call and
reads "unordered" after 8 calls. The sign test is `x >= 0` (NaN negative), the positive clamp is `x <= 0 -> 0` and the negative clamp is
`x > 0 -> 0` (NaN kept in both). All three are March's own compares, so June changes only the decay term.

**What is in.**
- **The field.** `PhysicParam::bJuneApplyMovementSpeed` (last field; struct ctor sets it false). It is a struct field, **not the plain member the
  kickoff asked for**, because the whole-struct copies are its writers. March has the same three copies (`X2GameUnit.cpp:731`, `:4179`,
  `X2GUUser.cpp:38618`), so the field gets June's clears with no extra call sites. `ResetPhysicParam` also runs mid-state (`X2DungeonGame.cpp:733`).
- **The parse and rescale** are in `CommonStateStartFuture`, after PASSIVE_SPEED_Y.
- **The decay** is in PhysicProcess: an `if` before each of March's two decay lines, which stay in place as the `else`.
- **Log.** `JUNE_FX C3h user state %d: APPLY_MOVEMENT_SPEED rate ...`, the first 32 state entries that carry the key.

**Not ported, routed.**
- **The minor unit** (byte copy to `+0x36c`, its speeds, `MINOR_UNIT_SPEED_TIME`, its decay): Lu/Ciel, checklist 7o.
- **June's SPEED_TIME loader and parser** (`31fe3a60` / `31fe2740`: `SPEED_TIME_ULTIMATE`, a 5th TIME_SPEED column): no phase owned them -> **C3l**.
- **`32296290`**, a world-physics scale that backs PhysicParam up once to a June-only block at `+0x334`. It never copies it back, so the bool is
  unaffected, but it is unidentified -> **C3m**.

**June data and reach.** One state in the June files: Ara `ASI_AIRIAL_DASH` (`PASSIVE_SPEED_X = RUN_SPEED * 1.5`). March's class scripts carry
no such key, so nothing sets it today. The engine half reaches play with the Ara port (checklist 7o), and the play-test is a regression check.

**Hygiene.** `flagcheck --worktree --files` (the 3 files): `2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`; 0 deleted lines; 0 encoding changes. RESULT PASS.

**Deploy.** DEPLOY OK: X2_offline.exe is 19,456,512 B, dated 2026-10-09 16:20:16, md5 192ac6b58ea0453f94e82c3a84eb838e, equal to X2/US_SERVICE/x2.exe (2026-10-09 16:20:09); built at mods/june-buff-port 1b12baa + 7 uncommitted code file(s) (C3h's 3, plus a peer's uncommitted C3i-family flags in Always.h / X2GUNPC / X2Game / X2DamageEffect). Strings checked in the deployed exe: `JUNE_FX C3h user state` (UTF-16) and `APPLY_MOVEMENT_SPEED` present. A peer redeployed at 16:21:06 (md5 5464aeda, same size), built from this tree; it also carries both strings, so it is the exe to play-test. Build consistency: Always.h (16:04) and X2GUNPC.h (16:05) predate both X2Lib.pch (16:09) and x2.pch (16:19), so X2Lib and X2 used the same flags and layouts. Reach re-checked: no `.lua` under `24965799/kom_contents/data*_kom/` contains `APPLY_MOVEMENT_SPEED`.

**In game.** There is nothing new to see. Dashes, runs and slides should stop exactly as before, and `offline_server.log` has no `JUNE_FX C3h` line.

**Play-test (2026-10-09): PASSED.** (user; exe 10df7f3e, a peer's redeploy carrying C3h; Ruben field 40000 as arasss, Ara Yama Raja: air dash, runs; reyaa runs). Logs read: 0 `JUNE_FX C3h` lines as expected, 0 EXCEPTION, only the baseline E_DISABLE_HB_CHECK_REQ UNHANDLED, field 40000 placed 21 monsters, no `BattleFieldData` error, no monster `OpenXSkinMesh` failure. The first attempt crashed in `CX2GUNPC::Init` (NULL m_pMotion: June's `Thief_Thin_toto.lua` names `Motion_THIEF_THIN_TOTO.X`): not C3h; fixed by staging 143 June meshes/textures (`7f00a64`) and restoring March's merged `BattleFieldData.lua` (`c4c6b18`, `2830cb9`; June's needs `BFT_*` -> atlas A6b).

### C3h - Review (2026-10-09, Opus 5.5 high)

**Verdict: correct against June. No code change, so no build.** One funcmap correction (row `31e4f990 (C3h)`). Read-only: capstone over
`unpacked_x2.exe` (`disasm_range.py`, `dispsweep.py`), plus a scratch popping x87 harness (`fldz; fcomp`, June's mask/jcc bytes).

1. **Parse.** `@32053fd4` (SPEED_TIME loader) to `@32054181`: the only branch is the xorstring lazy-init (`@32053fe4`), whose two paths join at
   `@3205403f`. A miss stores `al = 0` (`@3205418a`), so the key is read at every state start, default false. The C3g review already showed that the
   function's only earlier exit (SIEE 20, `@32050753`) is mirrored in March. xorstrings: `APPLY_MOVEMENT_SPEED` has exactly one use in June
   (`3379d178`, nuses 1), with no plain or UTF-16 copy. So nothing else parses it: not the riding pet's user-key parse (`X2RidingPet.cpp:1145`), and not
   the NPC. March's placement (after PASSIVE_SPEED_X/Y; March parses SPEED_TIME before PASSIVE, June after, which are independent) is fine.
2. **Rescale** `@3205419d-320542b9`. GetWalkSpeed is inlined and rounded to float (`@320541c1`), then divided by `+0x324` and rounded to float again
   (`@320541cf`). The x87 stack trace holds through the loop (`fld st0` per element, rate left in ST0 for the minor unit, `fstp st0` at `@320542b9`).
   nowSpeed.x is always scaled, passiveSpeed.x unless `31c7b640( x, -1 )`, and each `+0x85f4` entry's `+0x0`. `31c7b640` is `|a-b| <= FLT_EPSILON`
   (`test ah,0x41`, NaN = same), which is March's `IsSamef` (`KTDXCommonFunc.h:1449`). The port matches.
   **`31e4f990` = GetWalkSpeed confirmed, but the funcmap had the two fields swapped.** June's `InitAddtionalDashAndJumpBasicStatValue` (`32272023`)
   feeds `+0x324/+0x328/+0x32c/+0x330` to the stat objects at `+0x4870/+0x4924/+0x49d8/+0x4a8c` (0xb4 bytes each). So `+0x4910` is the **walk** object's
   protected result (`+0xa0`), and `+0x423c` is the shared `m_fRunJumpSpeedRate`: dash is `+0x423c * protected +0x49c4` (94 key reads at
   `+0x49c8`, e.g. `31fd1cde`). The kickoff carried the same swap. The row is fixed; no code or comment names the two offsets.
3. **Decay** `@320168ab-32016990`. The popping harness gave identical results over 3 rounds x 8 values x 3 compares (no leak):
   - sign `x >= 0` (-0.0 positive, NaN negative);
   - positive clamp `x <= 0 -> 0`;
   - negative clamp `x > 0 -> 0` (NaN kept in both).

   These are March's `X2GUUser.cpp` compares. June's flag path computes `(fReloadAccel * fElapsed) * rate`, with rate = `31e4f990() / +0x324` rounded to float.
   The port uses the same association. The flag-off path (`@320168fe`, `@3201696b`) is March's line, so only the decay term changed.
4. **Storage.** Byte writes to `+0x300/+0x2cc` binary-wide: the unit ctor (`322a2700 @322a3184 / @322a3126`) and three other classes' functions
   (`31c91930`, `31c926b0` (own vtable `3344ac24`), `3289e740`). Every 13-dword `rep movsd` (75) was listed. The unit-side ones are exactly Init `322a5f90`,
   ResetPhysicParam `322886a0`, RidingOffEnd `31fcde30` and C3m's `+0x300 -> +0x334` backup `3229635b` (read only, never copied back). The rest are
   stack locals, `std::vector` element loops (`3221d690`, `32226154`, `32207960`) or another class's `+0x2f0`. Every `lea [reg+0x300]` outside the
   known unit / NPC ones is another class: wstring SSO tests, members with destructors, buffer pointers. March has the same three copies
   (`X2GameUnit.cpp:734`, `:4182`, `X2GUUser.cpp:38849`), no `m_OrgPhysicParam =` copy and no other whole-struct writer of the unit's struct. The field
   placement stands.
5. **Routes.** Minor unit and Ara's state: checklist 7o. `31fe3a60` / `31fe2740`: C3l kickoff. `32296290`: C3m done, port planned as C3n.
6. **Hygiene.** `flagcheck JUNE_APPLY_MOVEMENT_SPEED --range 6ed3d50^..HEAD`: `2 PASS, 0 PASS~, 1 DEFINE, 53 OTHER, 0 FAIL, 0 ERROR`; 0 deleted lines,
   0 non-ASCII inserted lines, 0 EOL mismatches, 0 encoding changes. RESULT: PASS. The 53 OTHER rows are peers' commits in the range (C3e-1, C3i, C8b,
   C10b-2 and others), none touching this flag's lines.

The review needs no play-test; the C3h regression play-test already passed.

## C3i - port: June's NOT_UP_STATE in the NPC reaction cases

**Status: ready (found by the C3d-3 review, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3i.md`.

- **Scope:** the NPC INIT key `NOT_UP_STATE` (`+0x8ac4`, read since the C3d-3 review) gates six more reaction cases in `32769240` (cited as `32769060` until C3i; `@32769b1a`, `b72`, `be9`, `c41`, `cad`, `ced`). 36 shipped June NPC scripts set it.
- **Exit:** funcmap rows `Effect C3i`; flag `SERV_IRUHADEV_JUNE_NOT_UP_STATE`; play-test on a `NOT_UP_STATE` monster.

### C3i - Done (2026-10-09, Opus 5.5 high)

**Landed** in the `June effect Phase C3i` commit. funcmap rows `(C3i)`, JUNE_BUGS 143. Four flags (one per logical change):
`SERV_IRUHADEV_JUNE_NPC_FORCE_BACK_SPEED`, `SERV_IRUHADEV_JUNE_NOT_UP_STATE`, `SERV_IRUHADEV_JUNE_NOT_AFFECT_BLACKHOLE`,
`SERV_IRUHADEV_JUNE_NO_FALL_DOWN_AT_STATE`.

**Address corrected.** June's NPC `DamageReactStateChange` starts at **`32769240`** (ret 8, called by DamageReact `@327b3253`).
`32769060` (ret 4, ends `3276923d`) is a separate function DamageReact calls at `@327b52cf`. C3b, C3d-3 and the C3d-3 review cited
`32769060`. The funcmap rows, the RT_BOUNDING comment in `X2GUNPC.cpp` and the C3i / C3j kickoffs now cite `32769240`. This note's
`@` offsets were always inside `32769240`.

**1. The six sites.** Switch table `0x3276a2b0` (index `0x3276a2d4`): UP holds `b1a` / `b72`, FLY `be9` / `c41`, DRAG_UP `cad` / `ced`.
- **`b1a` / `be9` / `cad`:** in the forced super-armour path (below). Set: the hit does nothing (no speed, no state).
- **`b72` / `c41` / `ced`:** after the guard test. Set: only the `nowSpeed.y` write is skipped (kept, not zeroed). The state still
  starts (UP `m_UpDamage`; FLY / DRAG_UP `m_FlyDamageFront` / `Back`).
- **Hosts:** three sites sat in a gate March's NPC did not have. All six June cases (SMALL incl. RT 18 / 21, BIG, DOWN, UP, FLY,
  DRAG_UP) open with the FORCE_BACK_SPEED gate C3c ported for the user only. A super-armoured NPC breaks unless DD `+0x3ec`. With it,
  speed only and no state: SMALL `y = backSpeed.y + 700`, `x -/+ GetWalkSpeed() / 3`; BIG / DOWN / DRAG_UP `y = backSpeed.y`; UP / FLY
  the m_FlyCount division. Ported under its own flag (no partial June update). The bodies after the gate are March's, checked.
- **Data:** no shipped kom sets `FORCE_BACK_SPEED`. June's DamageEffect files have 21 entries; none is in the March merge. So the gate,
  and the three tests inside it, are dormant today. The three normal-path tests are live: 36 June NPC scripts set `NOT_UP_STATE`, all
  shipped in data037.
- **Tail:** the second switch (flip-on-hit) = March. June adds a nowState guard on the FlushForceDown branch (`@3276a18d-3276a1f3`),
  routed to **C3f item 3** (kickoff amended).

**2. User side.** No `NOT_UP_STATE` reader: the operand sweep of `+0x8ac4` finds the 7 NPC tests and the user's own member only
(`m_bSpecialAttackEventProcessedAtThisFrame`, class code).

**3. NOT_AFFECT_BLACKHOLE** (`327a0fa0 @327a5f0b`, `+0x8ac5`, after NOT_UP_STATE, default false). One reader,
`OnFrameMove_BlackHoleData 31f6eea0 @31f6f0bf`. The function was ported whole. Its June deltas:
- the key;
- **`NO_FALL_DOWN_AT_STATE`**: StateData `+0x401`, a new state key, below;
- `_isnan( dist )` or `GetInvincible()` (vslot `0x180`): the unit is not moved, but a user is still marked as held;
- **not ported:** the PvE owner filter (`IsPve 322cc1c0`, game type 4, which March lacks).

The rest = March: `+0x8aa0` is siege mode (`32756e10` = SetSiegeMode); class types 100..107 = March's eight `NCT_THING_*` (enumprobe);
Valentine skips flag-OFF; the displace assigns.

**4. NO_FALL_DOWN_AT_STATE** (LoadStateData `@327960e6-3279625d`, after HINT_NO_PERFRAME_EVENT, default false; 47 June scripts).
Two readers: the black hole, and the host's resurrect loop (`Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK @3230ec81`, March
`X2Game.cpp:13373`, otherwise identical). Both ported. A raw byte search finds both; `dispsweep.py` misses `cmp byte [eax+0x401]`.

**Hygiene.** `flagcheck <flag> --worktree --files X2GUNPC.cpp X2GUNPC.h X2DamageEffect.cpp X2Game.cpp` (before the commit):
- `NPC_FORCE_BACK_SPEED`, `NOT_UP_STATE`, `NOT_AFFECT_BLACKHOLE`: `files: 0 PASS, 0 PASS~, 0 DEFINE, 4 OTHER, 0 FAIL, 0 ERROR`; `RESULT: PASS`.
OTHER rows are the four flags sharing files. The one deleted line is the RT_BOUNDING comment's address (inside
`SERV_IRUHADEV_JUNE_BOUNDING_STATE`, comment only).
After the commit, `--range 603400e^..603400e`: `NOT_UP_STATE`, `NOT_AFFECT_BLACKHOLE`, `NO_FALL_DOWN_AT_STATE` `RESULT: PASS`
(0 FAIL, 0 ERROR). `NPC_FORCE_BACK_SPEED`: `0 FAIL, 1 ERROR` = Always.h's `#if defined(..._NOT_UP_STATE) && !defined(..._NPC_FORCE_BACK_SPEED)`
dependency guard, which the tool cannot evaluate (C3d-3's `USER_STATE_ID16` guard gives the same). Checked by hand: it only `#error`s.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,456,512 B, dated 2026-10-09 16:21:06, md5 5464aeda168608003b52c89c47c6e237, equal to X2/US_SERVICE/x2.exe (2026-10-09 16:21:00); built at mods/june-buff-port 1b12baa + 7 uncommitted code file(s).`

**How to see it** (hand-off in the session): MAG_STONE in Lanox Burned Forest takes a launcher without leaving the ground.
`JUNE_FX C3i npc ... RT 4: NOT_UP_STATE: no launch speed`. A black hole (Void Princess's Aging) does not drag SMITH_STAIN (Ashes Village)
or GEYSER_MULSAYRENA: `JUNE_FX C3i blackhole ... NOT_AFFECT_BLACKHOLE`.

**Open for the user:** nothing.

**Play-test (2026-10-09, exe `5464aeda`, logs read: 34 `JUNE_FX C3i` lines, 0 EXCEPTION).**
- `NOT_UP_STATE`, normal path: `npc 1000000 ( LOW_MAG_STONE_FD.lua ) RT 5: NOT_UP_STATE: no launch speed, the fly state still starts ( nowSpeed.y 0.0 )`
  twice (Iivue's Generate Black Hole hit, field 40029).
- `NOT_AFFECT_BLACKHOLE`: `blackhole unit 1000010 type 1: NOT_AFFECT_BLACKHOLE: never pulled` (16, the cap). Unit 1000010 is the
  stage-4 boss of dungeon 33280, id 1693 = `NUI_SMITH_STAIN`.
- Invincible skip: `blackhole unit 1000015 ... not moved: invincible or NaN distance` (16), id 5157 = `NUI_LOW_RAGE_BABYFIG_FD`.
- Not exercised: RT 4 (X X X). The Lord Knight's Burned Forest run (dungeon 33270, three `NUI_MAG_STONE`) logged no C3i line.
  The RT 4 test has the same shape as the RT 5 one that fired. `NO_FALL_DOWN_AT_STATE` and the FORCE gate also logged nothing;
  neither was expected to.

### C3i - Review (2026-10-09, Opus 5.5 high)

**Verdict: the port is correct against June. No code change, no build.** Doc fixes only (below). All binary facts were re-read
from a fresh full read of `unpacked_x2.exe` (md5 `3d043b4e5cdd...`) after the F: drive dropped mid-review.

1. **Function start.** `32769240` opens `sub esp, 0x14`, returns `ret 8` (`@3276926a`, `@32769d3b`, `@3276a2ac`) and is called at
   `@327b3253`. `32769060` ends `ret 4` at `3276923d` and is called at `@327b52cf`. The funcmap rows, the RT_BOUNDING comment and the
   C3j kickoff cite `32769240`. **Missed, fixed here:** the C3i kickoff item 1 ("switch table of `32769060`") and this phase's Scope line.
   The C3b / C3d-3 sections and kickoffs keep `32769060` as history; the C3i Done note records the correction.
2. **FORCE gate.** Table `0x3276a2b0`, index `0x3276a2d4`, bound `cmp eax, 0x14`: RT 1 / 18 / 21 SMALL, 2 BIG, 3 DOWN, 4 UP, 5 FLY,
   6 REVENGE, 15 BOUNDING, 16 DRAG_UP, 22+ none. Each case: IsSuperArmor (vslot `0x264( 0 )`) and DD `+0x3ec == 0` -> switch end; IsSuperArmor
   again -> forced body -> `jmp 32769e7c` (no state call). SMALL: `y = +0x3e0 + 700.0` (`0x3344ce98`); `+0x844d == 1` -> `x -= GetWalkSpeed()
   / 3.0` (`31e4f990`, `0x33439b50`), else `+=`. BIG `327698eb` / DOWN `32769a43` / DRAG_UP `32769cba`: `y = backSpeed.y`. UP / FLY: `+0xdf8 > 2`
   -> `fidiv` by `n - 1`, else `jle 327698eb` (BIG's forced body). Port = June. **`+0x844d` = bAttackerFront:** June's landPosition.y is
   `+0x8444` (the 210.0 ground test, `+0x8468 - +0x8444`); offsetprobe puts March's bAttackerFront 9 bytes past landPosition.y (`0x75b8` /
   `0x75c1`; UnitCondition: landPosition, bHit, bAttackerFront), and June's normal bodies test `+0x844d` where March tests bAttackerFront.
3. **NOT_UP_STATE.** `b1a` / `be9` / `cad` -> `jne 32769e7c` (nothing). `b72` -> `32769ba3` (m_UpDamage `+0x8690`); `c41` / `ced` -> `32769d02`
   (Front `+0x8694` / Back `+0x8698` by `+0x844d`). The speed write is skipped, not zeroed. The port's `if( set ) log; else if( m_FlyCount > 2 )
   {..} else {..}` and DRAG_UP's `if( set ) log; else nowSpeed.y = ..` bind as June branches; the StateChange lines follow unconditionally
   and the guard branch (`+0x8b7a`) comes first in both.
4. **Black hole** `31f6eea0..31f6f3c1`. NPC order NeverMove, FALL_DOWN, `+0x401`, `+0x8ac5`, siege, `+0x4090` / `+0x4094`, class 100..107 = port.
   Range: `fld dist; fld range; fcomp st(1); test ah, 5; jp` = in range when `dist <= range` or NaN = March's `fDist > range` exit. Clamp
   `min( 1.5 - dist / range, 1 )` (`0x334396c8` = 1.5) = March. `_isnan @31f6f2a2` / GetInvincible `@31f6f2bd` -> `31f6f384` (user AddEffective)
   = port. `+0xa15` -> vslot `0x3a4( 1 )` inside the move = March's SetCanPassUnit (ARA_CHARACTER_BASE). **PvE filter:** `IsPve 322cc1c0` =
   GetGameType (vslot `0x10c`) `== 4`; March's GAME_TYPE stops at 3 (GT_BATTLE_FIELD), so the filter could never fire: not porting it
   changes nothing. flagprobe: X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE, NEW_SKILL_2010_11, ARA_CHARACTER_BASE, EVE_SECOND_CLASS_CHANGE,
   X2OPTIMIZE_NPC_NONHOST_SIMULATION ON; SERV_EVENT_VALENTINE_DUNGEON / _INT OFF.
5. **NO_FALL_DOWN_AT_STATE.** Buffer `337a9124` (xorstrings), between HINT_NO_PERFRAME_EVENT and ATTACK_POWER_RATE_FURY;
   `32fe9e40( name, StateData + 0x401, 1 )`, miss -> `bl = 0`. March's read sits in the live `#ifdef X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`
   block of LoadStateData. Raw `.text` sweep of byte `[reg + 0x401]`: readers `31f6f0b2` (`cmp , 1`) and `3230ec81` (`cmp , 0`) only;
   `32394858` / `32394ee4` are member-wise StateData copies (`+0x400`, `+0x401`, `+0x404` in a row); the rest are `[esp + 0x401]` locals.
   Resurrect loop `3230ec71-3230ecaa` = port.
6. **Routed:** the C3f kickoff item 3 names `32769240 @3276a18d-3276a1f3`. Yes.
7. **Hygiene** (`--range 603400e^..603400e`): `NOT_UP_STATE`, `NOT_AFFECT_BLACKHOLE`, `NO_FALL_DOWN_AT_STATE` `RESULT: PASS` (0 FAIL, 0 ERROR,
   0 deleted lines). `NPC_FORCE_BACK_SPEED`: `4 OTHER, 0 FAIL, 1 ERROR` = the Always.h dependency guard
   `#if defined(..._NOT_UP_STATE) && !defined(..._NPC_FORCE_BACK_SPEED)`, which only `#error`s (as the Done note). The OTHER rows are
   the four flags sharing the five files; X2GUNPC.cpp `+153 -1`.
   The one deleted line is the RT_BOUNDING comment's address, inside `SERV_IRUHADEV_JUNE_BOUNDING_STATE` (comment only).

**Data.** june_src_fixed: `NOT_UP_STATE` 36 files, `NOT_AFFECT_BLACKHOLE` 36, `NO_FALL_DOWN_AT_STATE` 47 = the Done note.
**JUNE_BUGS 143 count fixed:** Treasure_Mimic sets `NOT_UP_STATE` in **eleven** state tables (MIMIC_START2 .. MIMIC_ESCAPE;
INIT_COMPONENT closes at line 42), not ten. Row 143 and the funcmap `@32769b1a ... (C3i)` row corrected.

**Open for the user:** nothing. RT 4 (UP) is still unseen in a play-test; it has the same shape as the RT 5 test that fired.

## C3j - identify, then decide: June's NPC bounding-state fallbacks

**Status: ready (found by the C3d-3 review, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3j.md`.

- **Scope:** InitState `327992a2-fd` gives every NPC bounding states (DOWN / LANDCRASH -> `m_DownDamageAir`, UP -> `m_UpDamage`). Read literally, `3275a070` then overwrites every NPC up-launch's speed. Trace what that reading misses, port the `IsBoundingUp` / `IsBoundingLandCrash` virtuals, and hand the user the port decision.
- **Exit:** funcmap rows `Effect C3j`; a user decision; code only after it.

### C3j - Done (2026-10-10, Opus 5.5 high)

**Verdict: the trace finds nothing that stops June's overwrite.** Evidence is in funcmap rows `(C3j)`; the kickoff's four questions:
1. **`+0x8454` is nowState.** GetNowStateData indexes by it, and DoStateEndStart `32768ac0` stores nextState (`+0x8404`) into it. It does so right before CommonStateStartHost / CommonStateStart, and nothing runs after them.
2. **`+0x40f0/+0x40f4` have two writers in all of `.text`**: user DRSC and the NPC RT_BOUNDING case. No ctor stores them, no block zero covers them, and NPCs come from the CRT `operator new`, so in June they stay uninitialized until the first RT_BOUNDING hit. March's ctor zeroes them (`X2GameUnit.cpp:306`).
3. **Nothing re-applies the reaction speed.** The RT_UP case writes `nowSpeed.y`, then `327590c0` only queues the state. The state starts at the next DoStateEndStart, and DamageReact's later `327588c0` touches x only.
4. **The kickoff's premise was wrong in one point.** `3275a070` runs near the end of CommonStateStart (`@327af3aa`), after the SPEED_X / SPEED_Y block (`@327add8a`). The 663 states with SPEED_Y are therefore not exceptions: with the fallback, **every** NPC up-launch takes the bounding back speed.

June's key table confirms UP_DAMAGE = `0x14` (`+0x8690`). June data has 996 RT_UP uses (class combos included), and Pporu's UP state has no SPEED_Y. Read verbatim, June kills every monster's launch unless the monster has already taken an RT_BOUNDING hit. No client-side reason was found; the shipped game launched monsters.

**Code** (flag `SERV_IRUHADEV_JUNE_BOUNDING_STATE`; fixes to C3d-3 plus the engine pieces that do not depend on the decision):
- **INIT_STATE keys** (`3279dc80 @3279f1cd-3279f2ad`, table `0x334b9178`): the keys are `BOUNDING_DOWN / _LANDCRASH / _UP`. `DAMAGE_BOUNDING_*` is only the default **state name** a missing key falls back to (`3275d900`). C3d-3 had read `DAMAGE_BOUNDING_*` as the keys.
- **The UP launch moved** from the top of `CommonStateStart` to its end, after March's InsertBuffFactor tail, which is where June calls it. C3d-3's position let a state's own SPEED_X / SPEED_Y win; June's does not. The `0 !=` guard is kept (C3d-3's documented deviation).
- **`OnFrameRender_Draw`** (`32768c70`, NPC vslot 3): June also draws when nowState is a bounding state and the state table did not open. The C3d-3 review's sweep missed this reader. The `0 !=` guards are ours, so an NPC with no bounding state is not drawn in state 0.
- **`JuneIsBoundingUp` / `JuneIsBoundingLandCrash`**: June vslots `0x2f0 / 0x2f4`. The review's `0x2f4 / 0x2f8` was one slot off, and `32951511` is a quick-slot `0x2f8` caller, not a bounding one. They are pure on `CX2GameUnit` (June's base `31aa1d30` is a trap stub). The user override is `31fd7ec0 / 31fd7f00`. The NPC override is `3275a010 / 3275a030`, unguarded as June has it. The only caller is Luciel `32b34680`, now a checklist 7l row.

**Routed:**
- The NPC tail's force-down `nowSpeed.y = 0` (`@327af202-327af255`) goes to the **C3f-1 kickoff, new item 2b**. The kickoff was amended because the block is the NPC twin of that phase's item 2.
- The tail's flag `0x2000` / `0x8000` blocks, the world-colour tail and state type `0x30 GAME_START_STATE` go to the new phase **C3o**.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --worktree --files X2GUNPC.cpp X2GUNPC.h X2GUUser.h X2GameUnit.h`: 4 PASS, 0 FAIL, 0 OTHER, 0 encoding changes, RESULT PASS. All 13 deleted lines are C3d-3's own lines inside the flag: the three `DAMAGE_BOUNDING_*` key rows and the 10-line UP block that moved.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,505,664 B, dated 2026-10-10 05:55:29, md5 ca7fe2379d29fc9282d5a1124d0fa9ed` (X2Lib 0 errors; built with a peer's uncommitted C3l files in the tree). No kom.

**How to see it in game:** nothing should change. No March or June NPC script names a bounding key or state, and no shipped DamageEffect uses RT_BOUNDING, so every new line is dormant (ids 0). The play-test is a regression: monsters still launch, fall and render.

**Regression play-test PASSED 2026-10-10** (user; exe `ca7fe237`, logs after the deploy). The dash X still launches and Z hits react, and monsters stay visible. `offline_packets.log` has 0 EXCEPTION; its only UNHANDLED is the known `E_DISABLE_HB_CHECK_REQ` x3. No `JUNE_FX C3j` / C3d-3 RT_BOUNDING lines appear, as expected for dormant code.

**Decided by the user, 2026-10-10: (a) verbatim.** Kickoff `kickoffs/june-effect/C3j-1.md`. The options as they were put: No NPC path enters the UP id except through it: June's `+0x870c` has readers only (`32759fd0`, `3275a010`, `3275a070`, `32768c70`), and March's NPC code makes no state change to it. With the fallback, June's overwrite therefore hits only ordinary launches (RT_UP into `m_UpDamage`).
- **(a) Verbatim** (plus a JUNE_BUGS row): every monster's launch takes the bounding back speed. No shipped DamageEffect uses RT_BOUNDING, so with current data that speed stays (0,0) and **monsters never launch again**.
- **(b) Fallbacks verbatim, the speed write behind a `SERV_IRUHADEV_JUNE_FIX_*` flag** (recommended). All three ids match June everywhere: the RT_BOUNDING case, the render gate, and `JuneIsBoundingUp/LandCrash` for Lu/Ciel's monster-target check. Only the one NPC line in `CommonStateStart` that kills launches is switched off. Dropping only the UP fallback instead would make `JuneIsBoundingUp` false on a launched monster where June returns true, which changes Lu/Ciel's `32b34680` targeting.
- **(c) Leave them out** (today's state): an RT_BOUNDING hit on an NPC still changes no state.

**Post-commit hygiene** (`flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --range 308abe4^..308abe4`): `files: 4 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 13 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

### C3j - Review (2026-10-10, Opus 5.5 high)

**Verdict: no transcription error in `308abe4`; no code change.** Checked against June (capstone, Ghidra read-only):
- **Key table** `0x334b9178`: entries 33-35 are `{ BOUNDING_DOWN, 0x21 }`, `{ BOUNDING_LANDCRASH, 0x22 }`, `{ BOUNDING_UP, 0x23 }`, and 36 is `REVENGE_ATTACK` (March's order, the bounding types last in the reaction block).
  - Loop `@3279f1cd-3279f2ad`: `32feb110` reads the key with default `''`. Only when its size (`[esp+0x64]`) is 0 does `3275d900( type )` supply the name, which is `DAMAGE_BOUNDING_*` for `0x21-0x23` and `''` otherwise.
  - The map lookup (`325c7d80`) gives 0 at end, otherwise the byte `+0x28`.
  - March matches: the default applies on `empty()`, and `GetStateID` returns `GUSI_NONE` (0) when the name is absent. `X2OPTIMIZE_VERIFY_NPC_SCRIPT_CONSISTENCY` is commented out (`KTDX.h:460`), so the default names log nothing.
- **UP launch position:**
  - June's tail `@327af1d2-327af1f7` is `31e4d160` (SetIsRightNow: it sets `+0x8471` and flips the rotation by 180, the SET_DIRECTION block), then `32275440`, `32071c30`, `+0x8f1c = 0` and `32761b40( 0.0 )`. That is March's last lines in order, under `X2OPTIMIZE_ROBUST_NPC_ROTATION`.
  - The C3j block is now the function's last statement, so no March line writes nowSpeed after it.
  - Early exits match: June `@327aca28` jumps to the epilogue `327af5ea` when `+0x8604` is null, the twin of March's `NULL == m_pXSkinAnim` return. The block is therefore reached on the same paths as `@327af3aa`.
  - June's SPEED_X / SPEED_Y writes to `+0x304 / +0x308` (`@327addf5 / 327ade0d`) come before `3275a070`.
- **`OnFrameRender_Draw`** `32768c70`:
  - The two gates are March's: cash shop `+0x290 / +0xd8`, then game type 3 (`GT_BATTLE_FIELD`) and vslot `0x278` (DidReceiveFirstSyncPacket).
  - Then `32fe9b20( nowState )`, a draw (vslot `0x464`) when it opened or nowState is `+0x8704 / 08 / 0c`, and `32fe9a10` only when it opened.
  - The flag branch always uses nowState. That equals March's live branch, because `X2OPTIMIZE_NPC_NONHOST_SIMULATION` is defined (`KTDX.h:281`).
  - The `0 !=` guards are behaviour-neutral: NPC nowState starts at `GUSI_LOADING` (`X2GUNPC.cpp:3147`), and `CommonRender_Draw` reads no Lua.
- **Vslots:**
  - Slots `0x2f0 / 0x2f4`: NPC `3275a010 / 3275a030`, user `31fd7ec0 / 31fd7f00`, base `31aa1d30` in both.
  - User: `(ptr - +0x7cec) / 100` (magic `0x51eb851f`, `sar 5`). Now `== 0x7a`; Now `== 0x79` or Future (`+0x7cb0`) `== 0x79`.
  - NPC LANDCRASH: nowState or `movzx word +0x8404`. March's `nextState` is a signed short (`SERV_IRUHADEV_JUNE_USER_STATE_ID16`, `Always.h:6193`), which is equal for ids 0-255; the -1 sentinel matches neither.
- **Trace spot-checks:**
  - (2) `dispsweep 40f0 40f4`: the writers are only `31feb6d2 / de` and `32769e1b / 27`; the readers are `31fc9d20` and `3275a070`; there is no third writer.
  - (4) is confirmed by the SPEED write addresses above.

**Correction (bookkeeping):** the funcmap row `327ac9d0 @327af1d2-327af3bb (C3j)` said "-> C3f-1 item 6". It is item **2b** (item 6 is the C3c comment), and the row is fixed.

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --range 308abe4^..308abe4`): `files: 4 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `13 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 13 deletions are C3d-3's own lines inside the flag: the three `DAMAGE_BOUNDING_*` key rows and the 10-line UP block (`#ifdef`..`#endif`) that moved to the tail.

**Build:** none (no code change). C3j's regression play-test already passed. The `C3j-1` kickoff needs no change: its fills sit in InitState, and the tail and guards it re-words are as reviewed here.

## C3j-1 - port verbatim: June's NPC bounding-state fallbacks

**Status: done 2026-10-10; launch kept by a FIX flag until C3j-2 finds June's missing piece; fix play-test PASSED; C3j-2 next. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3j-1.md`; review `kickoffs/june-effect/C3j-1-review.md`.

### C3j-1 - Done (2026-10-10, Opus 5.5 high)

**What landed** (flag `SERV_IRUHADEV_JUNE_BOUNDING_STATE`, plain transcription by the user's decision; play-test lines under the new
`SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG`, ON):
- **`CX2GUNPC::InitState`**, after C3d-3's raw key reads: June `327989e0 @327992a2-327992fd`, re-read in Ghidra (each step is `TEST EAX,EAX / JNZ`
  on the value just stored). DOWN == 0 -> `m_CommonState.m_DownDamageAir` (`+0x4098`). LANDCRASH == 0 -> `m_acStateType[ STATE_TYPE_DOWN_DAMAGE_AIR_LANDING ]`
  (byte `+0x14f`, type 0x13, not a member), then == 0 -> `+0x4098`. UP == 0 -> `m_UpDamage` (`+0x8690`). Both members are filled a few lines
  above. The `GetStateID` copies at `X2GUNPC.cpp` ~7033 / 7043 are in InitState's dead `#else X2OPTIMIZE_NPC_NONHOST_SIMULATION`.
- **Consequence, kept verbatim** (JUNE_BUGS 154): every NPC with an UP_DAMAGE state now has UP = that state, so CommonStateStart's tail (C3j)
  replaces each RT_UP launch's speed with `m_vJuneBoundingBackSpeed`. That speed is (0,0) until an RT_BOUNDING hit, and no shipped DamageEffect has one,
  so **monsters no longer rise when launched**.
- **Comments reworded** (3 deleted lines, all inside the flag): the render gate (`OnFrameRender_Draw`), the RT_BOUNDING case and the CommonStateStart tail
  now say the `0 !=` guards matter only for an NPC whose DOWN_DAMAGE_AIR / UP_DAMAGE is also missing. `Always.h`'s flag block gained 3 lines on the fallbacks.
- **Debug** (offline_server.log, not ErrorLog: the NPC lines beside it, C3d-3 / C3i, use `CX2OfflineLog::Server`): `JUNE_FX C3j-1 ids` once per NPC
  script for the first 10 scripts (each id with key / fallback / 0), and `JUNE_FX C3j-1 overwrite` once per NPC unit (cap 32) when the tail fires
  (state, bounding up, UP_DAMAGE, nowSpeed before -> after).
- funcmap row `327992a2-327992fd (C3j-1)`.

**Play-test trigger checked in the data the exe loads:** March's deployed `data036 Elsword_SwordMan.lua` (the only Elsword class script, so
Lord Knight uses it; decompiled for reading only) has `ESSI_DASH_ATTACK_X` `DAMAGE_DATA.REACT_TYPE = RT_UP`. `DashEventProcess` goes to it on
`oneX` with no job branch. `ESSI_COMBO_Z1..Z3` are `RT_SMALL_DAMAGE`. jtLordKnight = uid 25, `ELSWORD_LORD_KNIGHT`, Lv80 (`elsdb.py chars`).

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --worktree --files X2Lib/X2GUNPC.cpp`): `1 PASS, 0 OTHER, 0 FAIL, 0 ERROR`;
`3 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
With `KTDXLIB/Always.h` in `--files` the main flag gives an ERROR on C3i's pre-existing `#if defined(SERV_IRUHADEV_JUNE_NOT_UP_STATE) && ...`
(line 6336), which flagcheck cannot evaluate; C3j left Always.h out for the same reason. `_DEBUG` flag (`--files X2GUNPC.cpp Always.h`):
`0 PASS, 1 DEFINE, 1 OTHER (X2GUNPC.cpp: belongs to the main flag), 0 FAIL`; `0 deleted`, `1 #else/#endif without the flag token` (the
`#endif` of its own `#error` dependency guard, the house pattern of its neighbours); `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,507,712 B, dated 2026-10-10 06:30:40, md5 4fcd8bd2097c93102d64281ba8ca1937, equal to X2/US_SERVICE/x2.exe (2026-10-10 06:30:35); built at mods/june-buff-port 689828f + 2 uncommitted code file(s)`
(the 2 are this phase's; X2Lib 0 errors). Both `JUNE_FX C3j-1` strings are in the deployed exe. No kom.

**How to see it in game:** jtLordKnight in the Elder Village field: dash X on a soldier, and it plays its hit-up animation without rising.
`overwrite` then shows the speed after = (0.0, 0.0). Z hits are unchanged.

**Play-test PASSED 2026-10-10** (user; exe `44409299`, a peer's redeploy built on `c74f63f`; logs after the deploy).
- **Check 1:** the soldiers drop straight to the ground on dash X, the June behaviour expected here. The user reported it as "failed" and it is not:
  it is the spec's expected result. The `ids` lines show UP = UP_DAMAGE by fallback, e.g. `Elder_Bridge_guard_Small.lua` / `_Big.lua`: down 23, landcrash 26, up 24.
  Five `overwrite` lines show state 24, bounding up 24 = UP_DAMAGE 24, `nowSpeed ( -1110.0, 1950.0 ) -> ( 0.0, 0.0 )`.
- **Check 2:** the Z hits react as before. `offline_packets.log` has 0 EXCEPTION; its only UNHANDLED is the known `E_DISABLE_HB_CHECK_REQ` x2.

**Correction and fix (2026-10-10, user):** "follow June" means play like June, and June's game launches monsters (user: "of course yes").
The verbatim result is therefore a **port gap, not a June bug**; JUNE_BUGS 154 is marked provisional. A re-read in Ghidra this session
confirms the pieces as ported. `3275a070 @3275a08c-3275a0a6` copies `+0x40f0/f4` into `+0x304/+0x308` with no other condition. The RT_UP
case `@32769b72-32769bb3` writes `+0x308`, then `327590c0( +0x8690 )`. `@327af3aa` is reached unconditionally. The missing piece goes to
new phase **C3j-2** (`kickoffs/june-effect/C3j-2.md`, four leads).
- **New flag `SERV_IRUHADEV_JUNE_FIX_BOUNDING_UP_SPEED` (ON):** the tail's speed copy is skipped (June's block in `#else`); the fallbacks stay.
  With `_DEBUG`, `JUNE_FX C3j-1 fix` logs once per NPC: the speed kept and what June would have written.
- **Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_FIX_BOUNDING_UP_SPEED --worktree --files X2Lib/X2GUNPC.cpp KTDXLIB/Always.h`):
  `1 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`; `0 deleted`; `1 #else/#endif without the flag token` (its `#error` guard's `#endif`); `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,511,296 B, dated 2026-10-10 09:22:04, md5 3b63d3a04848b7e8d3ca95ddd18b405d, equal to X2/US_SERVICE/x2.exe (2026-10-10 09:21:59); built at mods/june-buff-port 485b929 + 2 uncommitted code file(s)`
  (the 2 are this fix's). The `JUNE_FX C3j-1 fix` string is in the exe.
- **Fix play-test PASSED 2026-10-10** (user: "back to the original flow"; exe `3b63d3a0`). Six `JUNE_FX C3j-1 fix` lines (guards, mushrooms, a chest)
  keep the launch speed, e.g. `( -1110.0, 1950.0 ) kept, June would write ( 0.0, 0.0 )`. 0 EXCEPTION, only the known UNHANDLED. The guard's June
  script was checked too: no June NPC script names a BOUNDING key, and its UP state (`THIEF_THIN_DAMAGE_AIR_UP`) sets no speed, so the Lua is not the missing piece.

## C3j-2 - find, then port: what keeps June's NPC RT_UP launch alive

**Status: done 2026-10-10 (found and ported, FIX flag removed); play-test PASSED; C3j-2 review next. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3j-2.md`;
review (C3j-1 and C3j-2 together) `kickoffs/june-effect/C3j-2-review.md`.

### C3j-2 - Done (2026-10-10, Opus 5.5 high)

**Found: June gives every NPC three bounding states of its own.** InitState's loader `3279dc80` calls `3275f280` for types 0x21/0x22/0x23
(`@3279dd68-3279dd9d`). That happens after the INIT_STATE index walk and before NONE..END go into the name map.
- When the script's INIT_STATE has no `BOUNDING_*` key (`3275cfc0`), June inserts a default state into the name map: `DAMAGE_BOUNDING_DOWN / _LANDCRASH / _UP`, id `++counter + index + 4`, so after the script's own states.
- `@3279dda9` grows the state vector by the counter.
- The type loop's default-name lookup (C3j) then finds these states. InitState's fallbacks (C3j-1) never fire, `+0x870c` is never `m_UpDamage`, RT_UP queues `+0x8690`, and `3275a070`'s copy never touches a launch.
- March lacked only the registration: C3j's default name pointed at a state that did not exist, `GetStateID` gave 0, and C3j-1's fallback turned UP into UP_DAMAGE.

**The four leads, all ruled out with addresses (funcmap rows `(C3j-2)`):**
1. All four NPC vtables share InitState `327989e0` and CommonStateStart `327ac9d0`, and `+0x870c` has only the two InitState writers.
2. `+0x40f0/f4` has no `lea`, struct-copy or setter writer.
3. Nothing re-applies the speed on the host. The re-apply after DoStateEndStart (`3277d320 @3277dc48/@3277dc66`) is the non-host sync path. `32776e10` and `32763eb0` write no speed.
4. CommonStateStart's only early exit is at its top (`@327aca28`), and the nowState setter `31c1fc80` is immediate.

**What landed** (flag `SERV_IRUHADEV_JUNE_BOUNDING_STATE`; the original resize is in `#else`):
- **Registration** in the NPC state loader (`X2GUNPC.cpp`, after the INIT_STATE count), with the counter added to the resize.
  - Ours: an id over 127 is not registered and is logged (`JUNE_FX C3j-2: ... not registered`), because March's state ids are a signed `char`.
- **Clone** after the state-table load (`@3279f937-3279faf4` + `32761f80`).
  - Source: DOWN_DAMAGE_AIR / _LANDING / UP_DAMAGE (each if > 0, else WAIT_STATE if > 0, else START_STATE).
  - Copies the motion and its PLAY_TYPE.
  - Flags: IMMADIATE_PACKET_SEND and FLY_AI's "set" bit on; LAND_CONNECT, TRANSITION and ALLOW_DIR_CHANGE off; NEVER_MOVE from the source (LANDCRASH and UP); G_ACCEL from the source when it is above 0 (UP).
  - DOWN also gets SET_LIMIT_MAX_G_SPEED = -4000.
  - Events: DOWN lands into LANDCRASH. LANDCRASH goes to UP at MOTION_END or at ANIM_TIMER 0.2. UP takes the source's NEGATIVE_Y_SPEED / FOOT_ON_LINE_TRUE events. In LANDCRASH and UP a never-move NPC goes to SMALL_DAMAGE_LAND_FRONT.
  - Every June offset and bit was matched to a March member through the Lua key June's `32784c00` reads into it (table in funcmap `32761f80 (C3j-2)`).
- **`SERV_IRUHADEV_JUNE_FIX_BOUNDING_UP_SPEED` removed** (Always.h block and the CommonStateStart branch). June's copy runs as June has it, and now fires only when the bounding UP state is entered.
- **Comments and the `ids` debug line reworded.** The line now also prints UP_DAMAGE and the state count. **JUNE_BUGS 154 withdrawn** (a port gap, not a June bug).
- **What it changes in game:**
  - `JuneIsBoundingUp` is now false on a monster launched by RT_UP, as in June. That reverses C3j's option-(b) reasoning about Luciel `32b34680`.
  - The bounding states have no Lua table, which is why June's render gate (C3j) exists.
  - The other code that walks every state tolerates that. The per-NPC `MakeTableReference` loop (Init, ~`:7460`) gets nil and returns false without logging (`KLuaManager.cpp:457`). The type loop's reaction marking (`:4855`) flags the three as REACTION_STATE, and so does June's (`@3279f2ff`, types 6..0x23).
  - Entering a bounding state still needs an RT_BOUNDING hit, and no shipped DamageEffect has one, so the new states stay dormant.

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --worktree --files X2Lib/X2GUNPC.cpp`): `1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`27 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The 27 deleted lines are all C3j-1's own lines inside the flag: 4 fallback-comment lines, the 6-line `ids` log call, and the 17-line FIX branch (`#ifdef` .. `#else`, `#endif`).
Always.h: the FIX block (14 lines) and 3 description lines replaced, both C3j-1's.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,513,856 B, dated 2026-10-10 10:06:15, md5 1035e5efe737aa3080173f5d2842d942, equal to X2/US_SERVICE/x2.exe (2026-10-10 10:06:10); built at mods/june-buff-port 886ae18 + 2 uncommitted code file(s)` (the 2 are this phase's; X2Lib 0 errors). The `JUNE_FX C3j-2` and new `ids` format strings are in the exe; the `C3j-1 fix` string is gone. No kom.

**How to see it in game** (jtLordKnight, dash X on an Elder Bridge guard in the Elder Village field):
- The soldier rises, now with no FIX flag.
- `JUNE_FX C3j-1 ids` shows down / landcrash / up as "type table" ids at the end of the state vector, e.g. `states N` with the ids N-3..N-1, and not 23 / 26 / 24 (= UP_DAMAGE).
- No `overwrite` line appears.

**Play-test PASSED 2026-10-10** (user; exe `1035e5ef`, logs after the deploy). The soldier rises and other hits react as before.
- 10 `JUNE_FX C3j-1 ids` lines, all "type table", each at the end of the state list. For example, `Elder_Bridge_guard_Big.lua` / `_Small.lua`: down 33, landcrash 34, up 35, UP_DAMAGE 24, states 36.
- `Ruben_El_Tree_Gate.lua`: 10/11/12 of 13. `chicken_female.lua` has UP_DAMAGE 0, so its UP state takes WAIT / START as its source.
- 0 `overwrite` lines, 0 `not registered` lines, 0 EXCEPTION; the only UNHANDLED is the known `E_DISABLE_HB_CHECK_REQ` x2; clean exit.

### C3j-2 - Review (2026-10-10, Opus 5.5 high; covers C3j-1)

Reviews C3j-1 (`c74f63f`, fix `8c2c7c5`) and C3j-2 (`aac4b70`). **No code finding against June.** Read from the exe bytes
(`disasm_range.py` / `funcdis.py`; Ghidra was not running).
- **C3j-1's fallbacks** `@327992a2-327992fd`: DOWN, LANDCRASH, UP, each a `TEST / JNZ` on the value just stored. DOWN -> `+0x4098`. LANDCRASH -> type byte
  `+0x14f` (type table `+0x13c`; 0x21..0x23 at `+0x15d..f`), then `+0x4098`. UP -> `+0x8690`. Matches.
- **Registration** `@3279dd3c-3279ddb0`. ESI = first missing INIT_STATE index (March's `index`), and the counter is zeroed `@3279dd77`.
  - `3275cfc0( lua, type )`: false for type >= 0x31; else reads `0x334b9178[type]` as a string (default "") on the INIT_STATE table and tests it non-empty.
  - `3275f280` then does `++counter`; id = counter + index + 4, inserted into the name map `+0x10` before NONE..END. Resize `@3279dda9` = counter + index + 5.
  - Equal to March's `GUSI_END + ( index - 1 ) + count + 1` / `GUSI_END + 1 + ( index - 1 ) + count` (GUSI_END = 5).
- **Id-over-127 guard (ours)**: UP's id is n + 8 (n = script states), so it fires only for n >= 120.
  - The largest June NPC script has 108 states (`PVP_HERO_ARUS`, `STATE_NAME =` count over `june_src_fixed/`, 3,448 scripts).
  - For n = 120..122 the defaults would be dropped and C3j-1's fallback would fire again; March's own ids overflow from n = 123.
- **Clone** `@3279f937-3279faf4` (each source is taken only when its id is `> 0`, in this order):
  - Sources: `+0xd6/d7/d8` (types 0x12..0x14), then `+0xc6` WAIT, then `[ebx]` START (`[esp+0x2c]` is the type-array base, `@3279f2ad`).
  - Guards: `> 5` and `< size` (bytes / 0x448) on both ids, and dst `+0xb8` (anim-name length) == 0. Copies `+0xa8` / `+0xc4`.
  - `32761f80`: the -4000 at `+0x430` (`0x33476cf0`) and the 0.2 ANIM_TIMER (`0x3346eeb8`, also pushed to `+0x30`).
  - `32761f80` event lists: next states arr[0x22] / arr[0x23] / arr[6], HAVE_TARGET -1, CHECK_NEVER_MOVE_TRUE (`+0xbb`).
  - `32761f80` UP: G_ACCEL copied when `0 < src` (`test ah,5 / jp`); both event shapes. All as ported.
- **Bits, by decoding the keys** `32784c00` builds (XOR-masked; new `june2017_re/statedata_keys.py`):
  - 4 ALLOW_DIR_CHANGE, 0x1f IMMADIATE_PACKET_SEND, 0x24 LAND_CONNECT, 0x2b TRANSITION, 0x2e NEVER_MOVE.
  - 0x36 / 0x37 FLY_AI (0x36 = key present, 0x37 = value), 0x3d G_ACCEL (float getter into `+0xf8`).
  - Each matches the March flag the same key fills in LoadStateData.
- **New check: the default states never pass LoadStateData**, so both games start them from the StateData ctor.
  - June `32393dc0 @3239426b-323942ef` sets the same 13 bits as March's ctor: HINT_NO_PERFRAME_EVENT, ALLOW_DIR_CHANGE, LAND_CONNECT, CAN_PUSH_UNIT, CAN_PASS_ALLY_UNIT, APPLY_ANIM_MOVE, APPLY_MOVEBONE, GUARD_DEFENCE_FRONT / _BACK, DAMAGE_DOWN, TRANSITION, USE_MIND_FLAG, FALL_DOWN.
  - `+0x430` = 1.0, `+0x401` = 0 and `+0x414/418` = 1.0 match the June-port members' initializers.
- **Per-unit runtime**: `CX2GUNPC::InitState` (`X2GUNPC.cpp` ~6939) sizes each state's event runtime from `m_vecNPCEventProcessData` for every state, table or not.
  - Lua refs are made only for CONDITION_FUNCTION / _TABLE custom conditions; UP copies only types 3 / 5.
  - The bounding hint flag stays at the ctor default and the 0.2 hint wakes the anim timer, as for a scripted state.
- **FIX flag gone**: no `FIX_BOUNDING_UP_SPEED` in any code file; CommonStateStart's tail is June's copy, inside the main flag only.
- **Play-test lines re-read** (exe `1035e5ef`): 10 `ids` lines at the end of each list (guards 33/34/35 of 36 = 5 + 27 + 1..3; Ruben gate 10/11/12 of 13); 0 `overwrite`, 0 `not registered`, 0 EXCEPTION.

**Changed:**
- `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG` OFF (`Always.h`: `#define` commented, dated OFF line).
- Stale main-flag comment in the type loop (`X2GUNPC.cpp` ~4847, "fallbacks ... not ported") now names the C3j-2 registration (2 of C3j's own lines out, 3 in).
- funcmap rows `327992a2-327992fd (C3j)` / `(C3j-1)` marked superseded by C3j-2.
- `tools/flagcheck.py`: a flag with no `#define` (a `_DEBUG` turned OFF) no longer crashes `define_sites` (`git grep` exit 1 = no match).

**Commit `e8a427b` also carries C3k's code.** The peer staged C3k into the shared index between this review's `--cached` check and its commit; the peer's `72705f5` records "code landed in e8a427b". History was left as is.

**Hygiene** (`--range c74f63f^..e8a427b`):
- `flagcheck SERV_IRUHADEV_JUNE_BOUNDING_STATE --files X2Lib/X2GUNPC.cpp`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`;
  `30 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- The OTHER row is `e8a427b`'s `X2GUNPC.cpp` (+30 -2): this review's 3 comment lines plus C3k's FLY_COUNT blocks. With `--with SERV_IRUHADEV_JUNE_FLY_COUNT ..._DEBUG` it is still OTHER (FLY_COUNT), 0 FAIL, PASS.
- Deleted lines: 3 C3j-1 + 27 C3j-2 (both Done notes), plus this review's 2 stale comment lines, all inside the main flag.
- With `--with SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG --files X2GUNPC.cpp Always.h`: the 3 `X2GUNPC.cpp` PASS rows stand.
  - The 6 ERROR rows are all `Always.h` at C3i's `#if defined(SERV_IRUHADEV_JUNE_NOT_UP_STATE) && !defined(...)` guard, which flagcheck cannot evaluate (not a finding, per the kickoff).
  - The 48 deleted lines are the 30 above plus `Always.h`'s FIX block and description (C3j-2) and the commented `_DEBUG` `#define` (this review).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,511,808 B, dated 2026-10-10 10:43:46, md5 580feb5d11813e3a5cc9d1feb4dfb701, equal to X2/US_SERVICE/x2.exe (2026-10-10 10:43:41); built at mods/june-buff-port 198411b + 3 uncommitted code file(s).`
- The 3 files are this review's two plus the peer's uncommitted C3k (`X2GUUser.cpp`, its `X2GUNPC.cpp` / `Always.h` hunks). The peer's 10:43 build compiled both; this review's build had nothing to relink.
- In the exe: `JUNE_FX C3j-1 ids` / `overwrite` 0 times, `JUNE_FX C3j-2: %S not registered` once (DEBUG is off, the main flag is on). No kom.

**How to see it in game (regression):** jtLordKnight, Elder Village field, dash X on an Elder Bridge guard: it still rises, and `offline_server.log` has no `JUNE_FX C3j-1` line.

**Regression play-test PASSED 2026-10-10** (user; exe `bbc8eb4b`, a peer's build at `d001ec0` + uncommitted, logs after the deploy).
- That exe has the main flag on and `_DEBUG` off: `JUNE_FX C3j-1` strings 0, `C3j-2 ... not registered` 1. So 0 `JUNE_FX C3j` lines is expected, not evidence.
- Evidence of the launch: C3k's lines on the Elder Bridge guards show `backSpeed.y 1950.0, nowSpeed.y 1950.0` on every launch hit without super armour.
  The one `super armour 1` hit gives 0.0, which is C3k's expected mismatch, not a bounding overwrite.
- 0 EXCEPTION; the only UNHANDLED is the known `E_DISABLE_HB_CHECK_REQ` x5; clean exit.

## C3k - port: June stopped counting fly hits

**Status: done 2026-10-10 (see "C3k - Done"), play-test passed, reviewed 2026-10-10 (see "C3k - Review"; found by the C3d-3 review, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3k.md`.

- **Scope:** June never increments `m_FlyCount` (`+0xdf8`): no write except zeroes, so the `/( m_FlyCount - 1 )` lift damping is dead in June. March still counts RT_UP / FLY / DRAG_UP.
- **Exit:** funcmap rows `Effect C3k`; flag `SERV_IRUHADEV_JUNE_FLY_COUNT`; juggle play-test.

### C3k - Done (2026-10-10, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_FLY_COUNT` (+ short-lived `SERV_IRUHADEV_JUNE_FLY_COUNT_DEBUG`). funcmap rows `(C3k)`; the C3d-3 review row is marked ported.

**Commit:** C3k's code and docs landed inside **`e8a427b`** (the C3j-2 review's commit). C3k's hunks were staged into the shared index seconds before that session ran `git commit`; nothing was lost, and the history was not rewritten.
`flagcheck JUNE_FLY_COUNT --with JUNE_FLY_COUNT_DEBUG --range e8a427b^..e8a427b`: `1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The 2 OTHER rows are the C3j-2 review's own lines in that commit: `BOUNDING_STATE_DEBUG` off in Always.h, and its X2GUNPC.cpp type-loop comment under `SERV_IRUHADEV_JUNE_BOUNDING_STATE`.

**1. Sweep re-confirmed; June counts nothing.**
- `dispsweep df8` gives 28 hits: 11 `esp` locals, the 6 `fidiv` reads, 4 zero stores, `sub esp`, and one other class's vector (`+0xdf4/+0xdf8/+0xdfc`).
  - The vector hits: `327ca2f0` / `327cee60` / `327cef20` compute `(end - begin) / 192`, and `327cf18c` is that class's ctor (`327cef70`) zeroing `+0xdd8..+0xdfc`. The C3d-3 review did not list `327ca2f0` / `327cf18c`.
- Sub-object bases: CX2GameUnit's ctor `322a2700` stores vtables at `+0x190` / `+0x1a0`, so `0xc68` / `0xc58` were swept too. Every hit is unrelated: a scaled table, two other classes' stores, and misdecodes.
- No `lea` or add-immediate of `0xdf8` anywhere.
- Both DamageReacts (user `320462a0`, NPC `327b1f30`) were checked instruction by instruction. Every `inc`/`add` to memory is a refcount or another member, and neither function has a `+0xdf8` operand. **So June has no FORCE_FLY reset either.**
- **June never reads `FORCE_FLY` at all.** `SetDamageDataFromLUA` stores it at DamageData `+0x445` (`32632390 @3263a0fb`, bool getter `32fe9e40`; key decoded at `@3263a088`). `dispsweep 445` finds only that store and the field-by-field DamageData copy `31e9c190` (`@31e9c544`). The other hits are immediates. With the flag on, March's only reader is gone too, and the field is write-only as in June; Elsword's setter `X2GUElsword_SwordMan.cpp:10379` stays and does nothing.
- The NPC readers are in `32769240` (C3i's start; dispsweep's `fstart` prints `32769060`).

**2. June's reset rule = CommonFrameMove only.**
- `3206c7db` (user `32063650`, test `@3206c7bb`) and `327bca82` (NPC `327b53b0`, `@327bca62`) zero the count when `GetInvincible()` (vslot `0x180` = slot 96) or `IsSuperArmor()`.
- That is March's CommonFrameMove reset, already June's under `SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG` (Buff 7b). It is unchanged.

**3. Port.** In both DamageReacts (`X2GUUser.cpp` ~8212, `X2GUNPC.cpp` ~9166) the whole `bForceFly` reset + RT_UP / RT_FLY / RT_DRAG_UP count block now sits in `#else`; the `#ifdef` branch is a comment.
- The `m_FlyCount > 2` damping reads (user 5, NPC 5 incl. C3i's FORCE paths) stay as they are: dead code, as in June.
- No JUNE_BUGS row: June dropping the increment while keeping the readers is a June change (no damping), not a mistake.

**Debug line** (`JUNE_FX C3k`, NPC DamageReact after DamageReactStateChange): every RT_UP / RT_FLY / RT_DRAG_UP hit on an NPC, at most 48 lines. It gives the per-NPC hit number, reAct, fly count, super armour, backSpeed.y and nowSpeed.y.

**Hygiene** (`flagcheck JUNE_FLY_COUNT --with JUNE_FLY_COUNT_DEBUG --worktree --files X2GUUser.cpp X2GUNPC.cpp Always.h`):
`2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 1 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The one token-less `#endif` closes the DEBUG block's `#error` guard in Always.h (the house shape of every `_DEBUG` block).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,511,808 B, dated 2026-10-10 10:43:46, md5 580feb5d11813e3a5cc9d1feb4dfb701, equal to X2/US_SERVICE/x2.exe (2026-10-10 10:43:41); built at mods/june-buff-port 198411b + 3 uncommitted code file(s)` (the 3 are this phase's). The `JUNE_FX C3k` string is in the exe. No kom.

**How to see it in game** (jtLordKnight, dash X on an Elder Bridge guard, Elder Village field; the C3j-2 launcher): re-launch the same guard 3+ times in one juggle, before it lands and stands up. Each launch is as high as the first.
- **Proof is the log:** `fly count 0` on every line, and nowSpeed.y = backSpeed.y on every `super armour 0` line where the guard actually rose, hit 3 and later included.
- March would show hit 3 at backSpeed.y / 2, then / 3, but only inside one continuous juggle: an invincible or super-armour frame (e.g. a stand-up) resets the count.
- **Expected mismatches, not failures:** `super armour 1`, a guarded hit, `NOT_UP_STATE`, and a hit that changed no state all leave nowSpeed.y unwritten.

**Play-test PASSED 2026-10-10** (user: re-launches as high as the first).
- **Exe:** `bbc8eb4b`, the C3o deploy at `d001ec0`. It was built after C3k landed, and the C3k lines prove the flag is in.
- **30 `JUNE_FX C3k` lines, all RT_UP (`reAct 4`).** All 30 show `fly count 0`. All 20 `super armour 0` lines have nowSpeed.y = backSpeed.y (0 mismatches), including Elder_Bridge_guard_Big hits 3, 4, 5, 7, 8, 12 and _Small hits 3, 5-8 at 1950.0. The 10 `super armour 1` lines show nowSpeed.y 0.0, as expected.
- **Debug-counter quirk:** the per-NPC hit counter is keyed by UID, and UIDs restart from 1,000,000 per stage. So `npc 1000015` continues from a chicken to a guard. This affects only the debug counter, not the port.
- **Clean run:** 0 EXCEPTION. The only UNHANDLED lines are the known `E_DISABLE_HB_CHECK_REQ` x5. Clean exit.

**Open for the user:** none.

### C3k - Review (2026-10-10, Opus 5.5 high)

**Verdict: no transcription error in C3k's hunks of `e8a427b`; no code change.** Only `SERV_IRUHADEV_JUNE_FLY_COUNT_DEBUG` is turned off (commented out in Always.h, with an OFF line, as the C3j-2 review did). Checked against June with capstone (`dispsweep.py`, `funcdis.py`) and Ghidra (read-only).

**1. The sweep re-run.**
- `dispsweep df8`: 28 hits, the same as C3k's. There are 11 `esp` locals and 6 `fidiv` reads (user `31feb2fb` / `31feb432`; NPC `32769b27` / `b7b` / `bf6` / `c4e`).
  - The 4 zero stores: `31fab59f`, `322a59c1`, `3206c7db`, `327bca82`.
  - Also `sub esp` (`3255886e`), and the other class's `+0xdf4/+0xdf8/+0xdfc` vector: `327ca2f0`, `327cee60` / `327cef20` (reads) and `327cf18c` (its ctor `327cef70`).
  - No other `+0xdf8` write.
- `0xc58` / `0xc68`: no `inc` or memory-`add` on either.
  - **x87 float ops** (in `327b53b0`, `327c9xxx`, `327ca370`, `327cef70`, `327d1d40`, `327d4670`) cannot be the int counter. The `+0xc68` pair in `327b53b0` is an NPC float member: `ebx` is the whole NPC there, and the same function writes `[ebx+0xdf8]`.
  - **`328ba170` / `328bad50`** destroy two containers at `+0xc58` / `+0xc68` (head pointer at `+4`) of a class whose vtables are `0x334c1b34` / `+0x34`.
  - **`3331c51c` / `3331c52a` / `3331c55b`** are that class's EH unwind funclets calling the same destructor (`32698fb0`). C3k's note called these "misdecodes"; they are unwind code, and just as unrelated.
  - **`32d52fdf`** stores a call result and **`32d54853`** a ctor zero. Neither is an increment.
- **DamageReacts.**
  - NPC `327b1f30`: Ghidra's decompile has no `param_1[0x37e]` (`+0xdf8`).
  - User `320462a0`: Ghidra's decompiler fails ("Decompilation failed"). Its Ghidra disassembly (`320462a0..3204a779`, 4,891 lines) has no `0xdf8` operand, and the capstone scan of both bodies has no `+0xdf8` operand or `lea`.
  - Every memory `inc`/`add` in the two bodies is one of: a refcount, user `+0x8848` / `+0x884c`, NPC `+0xb18` / `+0xb1c` / `+0xb24` / `+0x8a84`, or jump-table bytes. So June has no `bForceFly` branch that writes the counter.
  - The one neighbour hit in each DamageReact is a read of `+0xdf4` (`@32047022`, `@327b2fd0`). It is pushed with `[ebx+0x20]` into `32610980` on `g_pX2Game+0x58`: a different member, a read, not the counter.

**2. The port.**
- Both DamageReacts (`X2GUUser.cpp:8221-8251`, `X2GUNPC.cpp:9173-9203`) keep the whole `bForceFly` reset + RT_UP / RT_FLY / (BOUNDING_COMBO_TEST) / RT_DRAG_UP count block in `#else`, byte for byte. The `#ifdef` branch is a comment only.
- Neither CommonFrameMove reset (`@3206c7db`, `@327bca82`, Buff 7b) nor any `m_FlyCount > 2` damping read is touched. They are dead code, as in June.

**3. Play-test lines** (logs.py, same run as C3k's play-test, exe `bbc8eb4b`):
- 30 `JUNE_FX C3k` lines, all `fly count 0`.
- All 20 `super armour 0` lines have nowSpeed.y = backSpeed.y. That includes Elder_Bridge_guard_Big hits 3, 4, 5, 7, 8, 12 and _Small hits 3, 5, 6, 7, 8, all at 1950.0.
- 0 EXCEPTION. The only UNHANDLED lines are the known `E_DISABLE_HB_CHECK_REQ` x5.

**Hygiene.** `flagcheck JUNE_FLY_COUNT --range e8a427b^..HEAD --with JUNE_FLY_COUNT_DEBUG`:
`files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 12 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`.
- **The 12 OTHER rows.**
  - `e8a427b` mixes C3k with the C3j-2 review, so its Always.h and X2GUNPC.cpp rows are OTHER. Read by hand:
    - C3k's X2GUNPC.cpp hunks (`@@ 9164`, `9194`, `10103`) are insert-only.
    - The file's 2 deleted lines are the C3j-2 review's `JUNE_BOUNDING_STATE` comment at ~4844.
    - Always.h's `-1` is that review's `BOUNDING_STATE_DEBUG` off.
  - The other 10 rows are peer commits: `d001ec0` (C3o: `NPC_CINEMATIC_UI`, `NPC_STATE_START_EFFECT_SET`, `WORLD_COLOR_WINDOW`) and `f07940f` (C9h-4h: `EVENT_KIT_PICKUP`, `EVENT_STATES`, `COMMON_SKILL_STATE`).
- **This review's edit:** `flagcheck --worktree --files KTDXLIB/Always.h`: `0 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `1 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 1 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The deleted line is the `#define SERV_IRUHADEV_JUNE_FLY_COUNT_DEBUG` now commented out; the token-less `#endif` is the existing `#error` guard's (house shape).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,264 B, dated 2026-10-10 12:10:02, md5 5a7d1715515022a3a12b561334810d27, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:09:56); built at mods/june-buff-port 3209e84 + 8 uncommitted code file(s).` The 8 include peers' in-progress files (C3q / C3r / C3s work), which are built in too.
- build.py re-made all three PCHs (`Always.h is newer`). `JUNE_FX C3k` is no longer in the deployed exe (UTF-16LE and ASCII searched: absent).

**How to see it in game:** nothing new. The fly-count behaviour is unchanged (play-tested in C3k), and the `JUNE_FX C3k` lines stop.

**Open for the user:** none.

## C3l - identify, then port: June's SPEED_TIME additions

**Status: done 2026-10-10 (see "C3l - Done"), play-test passed; review pending. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3l.md`.

- **Scope:** June loads the user's SPEED_TIME through `31fe3a60`. `SPEED_TIME_ULTIMATE` wins when `31fca8e0(...)` `+0x178` is set, and the parser
  `31fe2740` reads a 5th TIME_SPEED column (a bool at `+0x10`). Identify the gate and the column's reader (`CommonFrameMoveFuture` `32061bf0`),
  then port them. The `MINOR_UNIT_*` lists are Lu/Ciel's (checklist 7o).
- **Exit:** funcmap rows `Effect C3l`; flag `SERV_IRUHADEV_JUNE_SPEED_TIME`; regression play-test.

### C3l - Done (2026-10-10, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_SPEED_TIME` (`KTDXLIB/Always.h`, end). funcmap: 8 `(C3l)` rows; the two `(C3h)` rows are closed. JUNE_BUGS 153. Checklist 7o
and backlog Stage 3c updated. Review kickoff `kickoffs/june-effect/C3l-review.md`.

**What June does.**
- **Parser** `31fe2740` = March's loop plus a 5th column, a bool (`+0x10`, miss false; read as March's `GetValue( int, bool& )` reads it). It
  returns whether at least one entry was read.
- **Gate.** `31fe3a60` reads `SPEED_TIME_ULTIMATE` when `31fca8e0( Future state m_eSkillID, upgrade swap )` is non-null and its templet `+0x178`
  is non-zero. `+0x178` is the key **`m_DeleteSkillIDByUpgrade`** (`32216320 @3221851e-322186ca`; EBP there is templet - 4, calibrated against
  `m_ePowerRate` and the power-rate vector). Its non-zero value also fills the skill tree's `+0x20` old->new map (`32205c00`). So the gate means
  "the equipped version of this state's skill is an upgraded skill". An empty ULTIMATE falls back to SPEED_TIME. This corrects the older funcmap
  names for `+0x178` (memo / base / transcendence id).
- **Consumer** `CommonFrameMoveFuture 32061bf0 @320620ec-320623c7`. March's tests and erase are unchanged, and two things are new:
  - **The 5th column.** A TRUE turns the entry by the Future `dirDegree.z` (`+0x7e0c`, the slope; Now getter `3208dfd0`). It computes
    `x = cos * x` first and then `y = sin * x` from the already-turned x, so `y = x cos sin` (JUNE_BUGS 153, kept verbatim).
  - **iFlag 11 / 12 / 13.** These are cases 1 / 2 / 3 on the vector returned by user vslot `0x778`. The base (`3208e1e0`) returns it
    unchanged; only Rose overrides it (`32a7ab60`). Flags 4-10 fall to default.
- **Other SPEED_TIME parsers** (NPC `32784c00`, `326ef840`, `32d2f560`) read 4 columns.

**What is in.**
- `TIME_SPEED::bJuneRotateByDirZ` (last field, ctor false; NPC / riding-pet users default it).
- The file-static `JuneParseSpeedTimeList` and the loader in `CommonStateStartFuture` (March's loop in `#else`). The gate is a constant
  false with the reason in the comment: March has no `m_DeleteSkillIDByUpgrade` and no upgrade swap. This matches the other forced-off
  `_ULTIMATE` gates; all of them switch on with backlog Stage 3c.
- In `CommonFrameMoveFuture`: the turn before March's switch, cases 11-13 before `default`, and the virtual `CX2GUUser::JuneGetTimeSpeedVector`
  (identity).
- Log `JUNE_FX C3l user state ...` for the first 32 firings of a turn or of iFlag >= 11.

**Not ported, routed.** Rose's override, the minor lists, the `+0x8e90` swap, the second-body anim clock and the minor consumer pass go to checklist 7o.
`m_DeleteSkillIDByUpgrade`, the `+0x20` map and the upgrade swap go to backlog Stage 3c.

**June data and reach.** Counts are from `june_src_fixed` (all 25 class scripts present):
- `SPEED_TIME_ULTIMATE`: 2 rows, Ara `ASI_A_ARA_QUICK_STAB`.
- A 5th column: 1 real use (Ara `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH`, TRUE) and 2 malformed rows (Elesis, Shay; iFlag 0, never fires).
- iFlag 12: 5 rows, Rose_Freyja. iFlag 11 and 13: none.

March: no `.lua` under any `24965799/kom_contents/data*_kom` contains `SPEED_TIME_ULTIMATE`. The 9 March class scripts (decompiled) have 690 SPEED_TIME
rows, all 4 columns with iFlag 1-3, and none of them is a June adoption. **Nothing reaches play today; the play-test is a regression.**

**Hygiene.** `flagcheck --worktree --files` (the 4 files): `1 PASS, 1 DEFINE, 2 OTHER, 0 FAIL`; 0 deleted lines, 0 non-ASCII inserted lines, 0 encoding changes; RESULT PASS. The two OTHER rows are a peer's uncommitted C3j hunks (`SERV_IRUHADEV_JUNE_BOUNDING_STATE`) in `X2GUUser.h` / `X2GameUnit.h`; this commit stages only the C3l hunks. Post-commit `--range 688d41c^..688d41c`: `3 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`; 0 deleted lines; 0 encoding changes; RESULT PASS.

**Deploy.** DEPLOY OK: X2_offline.exe is 19,505,664 B, dated 2026-10-10 05:55:29, md5 ca7fe2379d29fc9282d5a1124d0fa9ed, equal to X2/US_SERVICE/x2.exe (2026-10-10 05:55:24); built at mods/june-buff-port 4720231 + 6 uncommitted code file(s) (C3l's 4, plus a peer's C3j in X2GUNPC / X2GUUser.h / X2GameUnit.h; the link came from the peer's build in the same tree, after the C3l edits). `JUNE_FX C3l user state` (UTF-16) is in the deployed exe; `SPEED_TIME_ULTIMATE` is not, as expected: the constant-false gate lets the compiler drop that read. Build consistency: Always.h, X2GameUnit.h and X2GUUser.h (05:43:49) predate X2Lib.pch (05:47:20), x2.pch (05:55:17) and the TIME_SPEED / vtable-exposed objects (X2GURaven 05:50, X2GUChung 05:50, X2RidingPet 05:52). The only older objects are sqlite3.obj (C), KTDXLIB Browser.obj (no X2Lib headers) and two orphan Offline objects not in the project.

**In game.** Nothing new to see. Dashes, skills with scripted speed bursts and slides on slopes should move exactly as before, and `offline_server.log`
has no `JUNE_FX C3l` line. Hand-off: `arasss` (Ara Yama Raja): Z Z Z, dash X, double jump forward (iFlag 2 rows) and dash, jump, Z X
(`ASI_DASH_JUMP_COMBO_ALD_ZX`, iFlag 3) on a flat and a sloped line; `python tools/logs.py --grep "JUNE_FX C3l"` must show 0 lines.

**Play-test (2026-10-10): PASSED** (user; exe ca7fe237, `arasss` unit 24 in a battle field (Elder entrance), combos / dash / jump moves as handed off). Logs read: run after the deploy, 0 `JUNE_FX C3l` lines as expected, 0 EXCEPTION, only the baseline E_DISABLE_HB_CHECK_REQ UNHANDLED (x3), clean exit with census. The per-move check is the user's report (no log names unit states).

### C3l - Review (2026-10-10, Opus 5.5 high)

**Verdict: no transcription error in `688d41c`; no code change, no build, no deploy.** The deployed exe (`ca7fe237`, play-test passed) stays the live one, and nothing new needs testing. Checked against June with capstone (`disasm_range.py`, `vtslot.py`); Ghidra was not written to.
- **Parser `31fe2740`.**
  - It clears the list (`32073340` over begin..end), then builds `"%s%d"` (`33449104`) from index 0 and calls BeginTable `32fe9940`.
  - Each pass re-zeroes a local 0x14-byte entry before reading it:

    | Column | Field | Getter | Miss value |
    |---|---|---|---|
    | 1 | x | `32fea820` | 0 |
    | 2 | y | `32fea820` | 0 |
    | 3 | z | `32fea820` | -1.0 (`334380f0` = `0xbf800000`) |
    | 4 | iFlag | `32fea4e0` | 0 |
    | 5 | bool (`+0x10`) | `32fe9ed0` | `al` (0) |

  - The entry is then pushed (`31cee980`), and EndTable is `32fe9a10`.
  - The loop exits with `al = 1` at `@31fe28cd` when the first BeginTable succeeded, and `0` at `@31fe28d1` otherwise.
  - `32fe9ed0` does pushnumber, gettable, the number test `32fe4d70` (the same test the int getter `32fea4e0` calls), tonumber, then `ftol` (`32fe25f0`) `!= 0`. That is March `KLuaManager::GetValue( int, bool& )` (`KLuaManager.cpp:696`) step for step.
- **Loader `31fe3a60`** (called `@32053fd4` with `movzx [+0x8e90]`).
  - With arg 1, SPEED_TIME goes to `+0x8604` and MINOR to `+0x85f4`; otherwise the reverse.
  - The gate is `31fca8e0( Future m_eSkillID, 1 )` non-null and `[templet + 0x178] != 0`. When it holds:
    - parse main `SPEED_TIME_ULTIMATE` (`334747d8`) into `bl`, then minor `MINOR_UNIT_SPEED_TIME_ULTIMATE` (`334747b8`) into the local flag;
    - when `bl != 0`, skip SPEED_TIME (`334747ac`);
    - when the minor flag is 0, parse `MINOR_UNIT_SPEED_TIME` (`33474794`).
  - When the gate is off, SPEED_TIME and MINOR_UNIT_SPEED_TIME are read.
  - The port's main path matches this, and the minor half stays 7o.
- **Gate key.** The xorstring `3379e4fc` decodes to exactly `m_DeleteSkillIDByUpgrade` (24 bytes, keys `@32218535-3221865b`). Its value is written to `[EBP+0x17c]`. EBP = templet - 4 holds two independent ways:
  - **From the map node.** `[esp+0x34] = [esp+0x448] + 0xc` (`@3221662c`). `[esp+0x448] / +0x44c` is the insert's pair<iterator, bool>, so EBP is the node's `_Myval`, the key, and the templet is at EBP + 4.
  - **From the key read into `[EBP+0x178]` `@32217cbb`.** The key decodes to `m_ePowerRate`. `32205590` tests templet `+0x174 == 5`, and `[EBP+0x144]` (into `32207430`) is the `+0x140` vector that `32071720` reads.

  The `+0x20` insert `@322186ca` pairs (key, `[EBP+0xc]` = templet `+0x8`), and `32205c00` reads it back (node `+0x10`, miss -1).
- **Consumer `@320620ec-320623c7`.**
  - The entry is copied to a local (`[esp+0x54..0x64]`). Like March's `TIME_SPEED timeSpeed` copy, the turned x / y never reach the vector, which is erased anyway.
  - The tests match March: iFlag `jle`, z `< 0` skips, and anim `< z` skips; a NaN skips (C0).
  - The bool test is `cmp byte, 1`.
  - The angle is `fld [+0x7e0c]`, then a float round-trip, `fmul` by the double `(float)PI` (`33439ba0`), `fdiv` 180.0 (`33438080`), and a float store `@32062217`. cos and sin each go through a float store.
  - `@32062234-38` writes x = cos·x.
  - **The tail `@32062255-32062263`, run byte for byte on a popping x87 harness:** y = sin·x′ (deg 30, x 100, table y 7 gives y 43.3013, against a true rotation's 56.06). ST0 is left holding x′ for the switch (`fstp` / `fadd` in cases 1/2/3), and TOP is balanced. **JUNE_BUGS 153 stands, and the port's `fJuneSin * timeSpeed.vSpeed.x` is it.**
  - **`+0x7e0c` is the Future `dirDegree.z`.**
    - `3208dfd0` = `fld [ecx+0x80ac]`.
    - June's own bFuture pairs are 0x2a0 apart: `GetStateTime( bFuture )` `3208e240` reads `+0x7de8 / +0x8088`, and SetAttackSuccess reads `+0x7e2c / +0x80cc`.
    - `CommonFrameMoveFuture` itself writes `fStateTimeBack +0x7dec = fStateTime +0x7de8` and adds elapsed `+0x2b0` (`@320625fb-32062619`).
    - So "Now - 0x2a0" is Future, not March's `m_FrameDataNowBefore`. June's unitCondition carries `dirDegree` at `+0x20`, while March's carries it at `+0x18`. The port reads the field by name, so the layout does not matter.
  - The cos / sin thunks are bound IAT jumps (`333bc930 / 2c`). Their names rest on `31b29850`: `2b4e` lands on both diagonal slots and `2b54` / `-2b54` off the diagonal.
- **iFlag 11 / 12 / 13.**
  - Index bytes `32063634` = `0,1,2,6,6,6,6,6,6,6,3,4,5`, and the table `32063618` maps 1 / 2 / 3 to `32062287 / 22a8 / 22bd`, 4-10 to `32062368` (pop, default), and 11 / 12 / 13 to `320622d2 / 230c / 233a`.
  - Each of 11 / 12 / 13 pops ST0 and calls vslot `0x778( &out, &local entry )`, i.e. the turned vector. Then:

    | iFlag | Applies the vector to |
    |---|---|
    | 11 | `+0x304 / +0x308` with `+=` |
    | 12 | `+0x304 / +0x308` with `=` |
    | 13 | `+0x30c / +0x310` with `=` |
  - `3208e1e0` copies 12 bytes and returns 8. The 12 user class vtables (offset 0) keep it, except `CX2GURose` (`32a7ab60`).
  - March has one `JuneGetTimeSpeedVector`, in `CX2GUUser`, so nothing shadows it. `ARA_CHANGE_CLASS_FIRST` is live (`Always.h:2068`), so March case 3 exists beside case 13.
- **Routes.** Rose's override, the minor lists, the swap, the anim clock and the minor pass are all in checklist 7o (C3l bullet). Stage 3c names the field, the `+0x20` map and the swap.
  - **Added:** `GetSkillPowerRateValue 32071720 @32071723` is a second June reader of `m_DeleteSkillIDByUpgrade`. When the field is non-zero, it reads the power rate from the `32206fb0( 0xad, level )` lookup before the `+0x140` vector.
  - It is a Stage 3c consumer, now named in the backlog line and in funcmap row `32071720 (C3l review)`.
- **Cosmetic, not fixed:** the `X2GUUser.h` comment says "checklist 7", not "7o". Fix it the next time that header is touched for code, rather than rebuilding for a comment.

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_SPEED_TIME --range 688d41c^..HEAD`): `files: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

## C3m - identify: June's world-physics scale and its PhysicParam backup

**Status: done 2026-10-09 (see "C3m - Done"; the port is C3n). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3m.md`.

- **Scope:** `32296290` backs PhysicParam up once (`+0x300` -> `+0x334`, flag `+0x368`) and scales fReloadAccel / fGAccel / fMaxGSpeed by a factor
  from global `3372c0fc` (vslot `0x90`, `+0x184`). Decide whether this is March's `GetGravityAndSpeedRate` block moved or a June-only system.
- **Exit:** funcmap rows `Effect C3m`; a Done note; a port phase if June-only behaviour exists.

### C3m - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build)

funcmap: rows `32296290 (C3m)` (was the C3h candidate, now confirmed), `32800800`, `327cab80`, `327ac9d0 @327add8a-327ade8a`, `31ffe387` (all `(C3m)`), and a C3m note on the C3h `322886a0` row.

**1. What it is.** `3372c0fc` is the `g_pX2Game` holder. Game vslot `0x90` is `GetWorld` (`3231f490`, `+0x39c`) and `+0x184` is
`CX2World::m_fGravityAndSpeedRate`, March's `DEEP_WATER_DUNGEON_TEST` rate (`X2World.h:1268`; June's world init `327fddf0` sets it and the anim rate to 1.0).
- **The verdict: March's mechanism, kept, extended and trimmed.** It was not moved. June still reads the same rate with the same MAGIC 0.5 / 1.5 (read from the exe).
- **The only caller of `32296290`** is `327cab80`. That function walks the game's unit list (`+0x304`) and is called only from June's
  `CX2World::SetGravityAndSpeedRate` (`32800800`). A raw `.text` call scan confirms both, and the only pointer to `32800800` is its Lua binding (`31db9ef4`).
- **The four June deltas:**
  - **Live re-apply (new).** The Lua setter stores the rate. Then, in a `GT_DUNGEON` game only, it re-scales every unit at once through `32296290`, from a
    one-time PhysicParam backup (`+0x334`, byte `+0x368`): reload / G accel / max G speed x rate, with no MAGIC and no `!= 1` test. March applies the rate only
    in `ResetPhysicParam` and at NPC state start.
  - **`ResetPhysicParam` trimmed.** June scales only the three gravity fields. March's Walk/Run/Jump/DashJump `*= rate` (`X2GameUnit.cpp:4212-4215`) is gone.
  - **NPC `CommonStateStart`, `SPEED_X`/`SPEED_Y`.** These are stored unscaled. March multiplies them by the rate (`X2GUNPC.cpp:15572-15575`).
  - **NPC `CommonStateStart`, `G_ACCEL`/`DOWN_ACCEL`.** These are now always written: the state value or, when absent, the Org value, x rate x MAGIC. March
    writes them only when the state sets them (`:15568-15571`). Passive speed and the come-back x4.0 are unchanged.
- **Unchanged:** `SetUnitAnimSpeedRate` (plain setter) and its one reader, `CX2GUUser::ResetAnimSpeed` (= March `X2GUUser.cpp:30141-30148`).

**2. Reach: none, in either data set.** No `.lua` calls `SetGravityAndSpeedRate` or `SetUnitAnimSpeedRate`. Searched:
- March `kom_contents`: 6,037 files.
- The studio `ClientScript` / `_ClientScript`: 4,057 files.
- June `14JUN17/data/*_kom`: all 7,343 files, plain text and bytecode, the 30 not in `june_src_fixed/` included.

So the rate stays 1.0 and the player sees nothing. At 1.0 every delta is a no-op:
- the setter is never called;
- `ResetPhysicParam` tests `!= 1`;
- the NPC Org fallback writes what March's `CommonStateEnd` already left. That is `fGAccel = Org` (`:24150`) and `fDownAccel = 0` (`:24112`). June's
  Org `fDownAccel` is only ever 0: the ctor zeroes it, `InitPhysic` has no `DOWN_ACCEL` key, and the NPC `DOWN_ACCEL` read stores into state data `+0xfc`.

The edge case: `elstools` can cut 1-255 bytes off a June algo-2 entry's tail, so the June search is not byte-exact.

**3. The port.** June-only behaviour exists and no STATUS row owns it, so it is planned as **C3n** (below; STATUS row + kickoff). It is regression-only until a
script sets the rate. **No questions for the user.**

## C3n - port: June's world gravity rate (live re-apply, ResetPhysicParam trim, NPC state start)

**Status: done 2026-10-09 (Done note below). Was: ready (found by C3m, 2026-10-09). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3n.md`. **Not with C3j / C3k** (same NPC code), nor with a
peer editing `ResetPhysicParam`.

- **Scope:** the four deltas in "C3m - Done" item 1, read from the C3m funcmap rows.
  - **Flag `SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY`:** the setter's dungeon-only tail (it stops being inline in `X2World.h`), the unit walk and `32296290`, with its
    backup block and byte as plain `CX2GameUnit` members. Not PhysicParam fields: nothing copies them back.
  - **Flag `SERV_IRUHADEV_JUNE_WORLD_RATE_TRIM`:** `ResetPhysicParam`'s three-field scale, and the NPC block's unscaled SPEED_X/Y and Org fallback for G/DOWN accel.
  - **Bugs, kept verbatim:** the stale backup and the MAGIC mismatch, each a `JUNE_BUGS.md` row.
- **Exit:** funcmap rows `Effect C3n`; both flags; a regression play-test (dungeon + field: jumps, falls, NPC launches look as before). No script sets the rate, so
  there is no positive test unless the user wants one from the dev console.


### C3n - Done (2026-10-09, Opus 5.5 high)

Flags `SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY` and `SERV_IRUHADEV_JUNE_WORLD_RATE_TRIM` (`KTDXLIB/Always.h`). funcmap rows `(C3n)` x3. JUNE_BUGS 149, 150.
June's bytes were re-read for `32800800`, `327cab80`, `32296290` and the NPC block `327ac9d0 @327add8a-327ade13`. They match the C3m rows. The NPC block is
`(value x rate) x magic`, with magic in `[esp+0x1c]`, the same as March's expression order.

**What landed.**
- **APPLY.**
  - `CX2World::SetGravityAndSpeedRate` is now out of line (`X2World.cpp`), and March's inline store is kept in `X2World.h`'s `#else`.
  - It stores the rate. Then, only in a `GT_DUNGEON` game, it does a `dynamic_cast<CX2DungeonGame*>`. If the cast succeeds, it walks `GetUnitList()` (NULL
    skipped) and calls the new `CX2GameUnit::ApplyJuneWorldGravityRate`.
  - The walk (`327cab80`) is inlined, so `X2Game.h` is untouched.
  - The apply function re-checks `g_pX2Game` and `GetWorld()`. Then it takes a one-time backup: `m_JuneWorldRateBackupPhysicParam` and
    `m_bJuneWorldRateBackupTaken`, plain members cleared only in the ctor (not `ReInit`). Then it sets reload / G / max G = backup x rate.
  - The Lua binding `X2Main.cpp:3696` is unchanged and binds the out-of-line member.
  - The setter writes one `JUNE_FX C3n` line per call (under `SERV_IRUHADEV_OFFLINE`).
- **TRIM.** `ResetPhysicParam`'s walk / run / jump / dash-jump x rate goes to `#else`. The NPC `CommonStateStart` always writes G / DOWN accel: the state
  value, else the Org value, x rate x magic. It stores SPEED_X / SPEED_Y unscaled.
- **Scope check.** March has no other reader of the rate: grep of `X2Lib` / `KTDXLIB` finds only `ResetPhysicParam` and the NPC block.

**Hygiene** (`flagcheck --worktree --files <the 6 files>`):
- `APPLY`: RESULT PASS. 2 PASS, 1 PASS~ (`X2World.cpp`: the blank separator line), 3 OTHER (Always.h; `X2GameUnit.cpp` / `X2GUNPC.cpp` carry `TRIM`). 0 deleted, 0 encoding changes.
- `TRIM`: RESULT PASS. 1 PASS (`X2GUNPC.cpp`), 5 OTHER (`APPLY`'s files). 0 deleted.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,502,592 B, dated 2026-10-09 22:58:15, md5 3bb8764c4cffdf9a699924fca9efa8e7`. Built at `5463f49` plus C3n plus a peer's uncommitted C3e-2 edits.
The UTF-16 `JUNE_FX C3n SetGravityAndSpeedRate ...` string is in the deployed exe (offset `0x10231a0`), so the APPLY side compiled in.

**How to see it in game.** It is dormant: no script sets the rate, so the regression play-test should show no change and no `JUNE_FX C3n` line. Positive test
from the dev console (END key, auth level 3), in a dungeon: `g_pX2Game:GetWorld():SetGravityAndSpeedRate( 0.5 )`. Both bindings exist: `X2Main.cpp:3931`
`GetWorld`, and the `g_pX2Game` decl in `X2GameEdit.cpp:958`. Falls get slower, and the log line shows `game type 2` (`GT_DUNGEON`) with the unit count.
`( 1.0 )` restores the backup values. The NPC SPEED/accel and ResetPhysicParam scaling apply only after a reset / state start at a rate != 1, and those
use MAGIC (JUNE_BUGS 150).

**Play-test PASSED 2026-10-10** (user, exe `3bb8764c`). The dungeon and field regression showed no change. The console positive test logged `rate 0.500000 game type 2 units 11`, then `rate 1.000000 game type 2 units 11` in the dungeon, and `rate 0.500000 game type 3 units -1` in the field (the dungeon gate holds). 0 EXCEPTION; the only UNHANDLED is the baseline `E_DISABLE_HB_CHECK_REQ`.

No questions for the user.

## C3o - identify, then port: June's NPC CommonStateStart tail and GAME_START_STATE

**Status: done 2026-10-10 (see C3o - Done), play-test passed, reviewed 2026-10-10 (see C3o - Review). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3o.md`.

- **Scope:** the June-only code after March's last CommonStateStart block (`327ac9d0 @327af25b-327af612`): the state-flag `0x2000` block, `+0x8e39` from flag `0x8000`, vslot `0x3bc` / `32776e10`, the `+0x244` stores and `CHANGE_WORLD_COLOR_DATA`. Also state type `0x30 GAME_START_STATE`. The force-down block is C3f-1's (2b) and `3275a070` is C3j's.
- **Exit:** funcmap rows `Effect C3o`; a flag per change ported; regression play-test.

### C3o - Done (2026-10-10, Opus 5.5 high)

**The tail mapped** (`327ac9d0 @327af25b-327af612`, funcmap rows `(C3o)`). June orders it: C3f-1's 2b block, then the `0x2000` block, the detonation carry (ported at the top), the `0x8000` store, `3275a070` (C3j), the `+0x40dc` reset (checklist 7l), the effect-set call and `CHANGE_WORLD_COLOR_DATA`.

1. **Flag `0x2000` (word `+8`) = June bit `0x4d`, key `DIRECTION_STATE`** (parser `32784c00 @3278bbc7`, a miss stores false). The global `0x3372c0fc` is the `g_pX2Game` holder. The block arms June's **skip-the-start** list at game `+0xb40` (bool armed, bool sent, `vector<int>` at `+0xb44`). If the list is empty, `322d8010` resets it first. Then it sets armed and pushes NPC `+0x270` (likely `GetUID()`). ESC consumes it (`322f4c60` -> `322f11d0` / event `0x4ac`): each listed NPC goes to its WAIT_STATE (`+0x4088`). It is June-only, nothing in March has it, and **77 shipped scripts** set the key, so the whole chain goes to **C3p**. Funcmap row 1064 is corrected: the arming site is this tail and the bit is `DIRECTION_STATE`.
2. **Flag `0x8000` = June bit `0x4f`, key `IS_APPLY_CINEMATIC_UI`** (`@3278a049`, a miss stores **true**). `+0x8e39` is March's `m_bIsApplyCinematicUI` (setter `31e4d4d0`).
   - Readers: only the focus-camera UI hide in `UpdateUnitPointer`, dungeon `327cea70 @327cec78 / @327cecf8` and battle field `32d530a0 @32d53249`. A raw disp32 scan over `.text` confirms the list.
   - Writers: this store, and class code at 10 sites that set a summoned NPC's focus flags (`32021256`, `32ac10ac`, `32b30048` ...; funcmap). **Ported.**
3. **vslot `0x3bc` = `GetNowAnimationTime`** (`32755be0`). `32776e10` = `CX2GUNPC::CommonFrameMove_EffectSet`, so June plays a state's effect sets once at state start. The cursor `+0x8f18` is reset earlier (`@327adab4`, March `X2GUNPC.cpp` `m_iEffectSetToPlayProgress = 0`). **The call is ported.** June's *body* differs (CONSUME_MP gate through vslot `0xe4`, 0x3c-byte entries from June's `EFFECT_SET_LIST_FINAL` loader, flag-picked delete lists). That body depends on a loader March lacks, so loader and body are one change: **C3q**. This is a deliberate exception to "no partial June updates": the call fits March's body as it is, and porting the body alone would leave a gate nothing fills.
4. **`+0x244 / +0x248` = CX2GameUnit START / END of `CHANGE_WORLD_COLOR_DATA`** (ctor -1 `@322a2928`).
   - The tail sets both to -1, then reads the table from the open state table: `START_TIME` (miss 0) and `END_TIME` (miss 9999, `3346ede8`).
   - `CommonFrameMove 327b53b0 @327b54bb-327b557d`: with START >= 0, START <= t <= END sets the world colour `D3DXCOLOR(1, 0, 0, 1)` every frame (`31e4c8f0` = `CX2World::SetWorldColor`; colour order from the x87 sequence `@327b5532-327b555e`). Outside the window, `32296be0` runs `FadeWorldColor(GetOriginColor(), 1)` and sets both times to -1.
   - **NPC side ported** (members, restore and reader on CX2GameUnit / CX2GUNPC). It is dormant: no NPC script, March or June, has the table. The user side (Ain only) goes to **C3r**.
   - JUNE_BUGS 156: a state without the table ends a running window with no fade back.
5. **GAME_START_STATE (type `0x30`) -> NPC `+0x8fac`** (InitState `@3279928e`, ctor 0 `@327633df`). **No reader in June**: raw disp32 scan, the templet slot `+0x16c` and the table entry were all checked. Nothing to port; `Raid_Waiting` sets it for nothing. June's 49-entry table also has **no `LINK_JUMP_STATE`** (the string is absent from the exe). No NPC script uses it, so there is no visible change and no port.
6. **Routed:** June's `UpdateUnitPointer` shape (`+0x8e38` gate, anim-name compares, flags `0x2000000` / `0x20000000`, the state-id jump tables) goes to **C3s**.

**Code** (`X2GUNPC.h/.cpp`, `X2GameUnit.h/.cpp`, `Always.h`; touches X2GameUnit as June has the members there):
- `SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI`: the parser reads `IS_APPLY_CINEMATIC_UI` into `FLAG_IS_APPLY_CINEMATIC_UI` after FALL_DOWN, and the tail stores it into `m_bIsApplyCinematicUI` before the bounding block. `_DEBUG` logs `JUNE_FX C3o cinematic` once per NPC that stores FALSE.
- `SERV_IRUHADEV_JUNE_NPC_STATE_START_EFFECT_SET`: `CommonFrameMove_EffectSet( GetNowAnimationTime() )` after the bounding block.
- `SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW`: the tail parse, the `CommonFrameMove` window after `DoDelegateProcessInCommonFrameMove`, and `CX2GameUnit::RestoreJuneWorldColor` + 2 members. The null checks on the game and world are ours.

**Hygiene:** `flagcheck --worktree` before the commit (C3o's files only): `SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI --with _DEBUG, _STATE_START_EFFECT_SET, _WORLD_COLOR_WINDOW`: `X2GUNPC.cpp` PASS +69, `Always.h` DEFINE +40, 0 FAIL, 0 ERROR, RESULT PASS; `_WORLD_COLOR_WINDOW`: `X2GameUnit.cpp` PASS +19, `X2GameUnit.h` PASS +7, RESULT PASS; `X2GUNPC.h` PASS +3. 0 deleted lines, 0 non-ASCII, 0 encoding changes; the one `#endif` without a token is the house `_DEBUG` `#error` guard in `Always.h`. The first run's ERROR was `X2GUNPC.cpp` checked without `--with` (the nested `_DEBUG`). Post-commit `--range` lines: see STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,530,752 B, dated 2026-10-10 11:10:39, md5 bbc8eb4bef00e8df2ad3f9752b256375, equal to X2/US_SERVICE/x2.exe (2026-10-10 11:10:34); built at mods/june-buff-port 501fc0c + 8 uncommitted code file(s).` The 3 extra files are a peer's in-progress C9h-4h (`X2GUUser.h/.cpp`, `X2Game.cpp`). `JUNE_FX C3o cinematic` and `IS_APPLY_CINEMATIC_UI` (UTF-16) and `CHANGE_WORLD_COLOR_DATA` are in the deployed exe. No kom.

**How to see it in game:**
- **Cinematic UI.** The 6 scripts that set FALSE are the Mad Behemoth boss set (Henir / Sander secret, random or absent stages) and three June Rena skill NPCs, so there is no reliable route; the play-test is a regression. If one does spawn and enters a focus-camera state, the UI stays on screen, and the log has its `JUNE_FX C3o cinematic` line.
- **Effect sets.** A monster's time-0 effect sets now start one frame earlier: nothing to see.
- **World colour.** Dormant.

**Regression play-test PASSED 2026-10-10** (user, Tree of El; exe `bbc8eb4b`, logs after the deploy): 0 EXCEPTION, the only UNHANDLED is the baseline `E_DISABLE_HB_CHECK_REQ` x5. 16 `JUNE_FX C3o cinematic` lines, all `state 4` (chicken, Thief_Thin_Zero, Ruben_El_Tree_Gate). State 4 is `GUSI_DIE`: ids <= `GUSI_END` never pass LoadStateData, so their flags come from the StateData ctor alone, and June's (`32393dc0 @3239426b-323942e9`: word 0 bits 2/4/6/7/11/12/13/25/26, word +4 bits 36/37/41/43, word +8 none) leaves bit `0x4f` at 0, as March's does. A dying NPC therefore stores FALSE in June too. The member is read only in the focus-camera branch, so nothing changes on screen. The code ran on every NPC and no scripted state stored FALSE.

### C3o - Review (2026-10-10, Opus 5.5 high)

**No code finding against June.** Re-derived from the bytes (`funcdis.py`, `disasm_range.py --x87`, `statedata_keys.py`, `dispsweep.py`):
- **Parser** `32784c00`: `statedata_keys.py` gives `@32789e76` bit `0x1a` FALL_DOWN, `@3278a04d` bit `0x4f` IS_APPLY_CINEMATIC_UI, `@3278a145` bit `0x50` FURY_MODE, `@3278bbcb` bit `0x4d` DIRECTION_STATE. A miss stores 1 for `0x4f` (`@3278a03f mov [esp+0x14], 1`) and 0 for `0x4d` (`@3278bbbe` stores `bl`, zeroed `@3278bba2`).
- **Tail** `@327af38e-327af5e5`: `[flags+8] & 0x8000` -> `+0x8e39`, then `3275a070`, then the `+0x40dc` reset when bit 0 is clear (checklist 7l), then vslot `0x3bc` -> `32776e10`, then `CHANGE_WORLD_COLOR_DATA`. The cursor `+0x8f18 = 0` is at `@327adab4`, after the CUSTOM_STATE_DELETE_SOUND call `@327adaa7`, which is March's `m_iEffectSetToPlayProgress = 0` position (`X2GUNPC.cpp` CommonStateStart, after ChangeAnim / Play, so `GetNowAnimationTime()` is the new state's).
- **World colour parse:** both -1 (`334380f0` = -1.0), BeginTable( `33478ee0` "CHANGE_WORLD_COLOR_DATA", 1 ) on `+0x2b4`. The keys were decoded by hand: `0x337aa59c` ^ (fb 20 c5 8b 47 57 12 11 b0 6f) = `START_TIME`, and `0x337aa594` ^ (20 c5 8b 47 57 12 11 b0) = `END_TIME`. Miss: `fldz` and `3346ede8` = 9999.0. March's `m_LuaManager` holds the state table there (`DoStateEndStart` wraps CommonStateStart in `BeginTableByReference`).
- **Window** `327b53b0 @327b54bb-327b557d`: `fldz; fcomp START; test ah,0x41; jp` skips when START < 0 or NaN; `fld START; fcomp t; test ah,0x41; jp` restores when START > t; `fld END; fcompp; test ah,1; jne` restores when END < t. The port's `0 <= START`, `START <= t && t <= END`, else restore is the same, NaN included.
- **Colour:** `fld1` -> r; `0x334387b0` (1/255) x `0x334350e8` (0.0) -> g, b; the remaining `fld1` -> a. So `D3DXCOLOR( 1, 0, 0, 1 )`. This is a hand trace plus the constants; the `--x87` trace confirms the pop order (it does not evaluate `fmul`). `x87test.py` was not run because it covers compare idioms only.
- **Restore** `32296be0`: only when START >= 0, world `+0xc4..+0xd0` = `+0x90..+0x9c`, `+0xd4` = 1.0, both -1. `31e4c8f0` writes `+0xb4`, `+0xc4` and `+0xd4` = 0, which is March's SetWorldColor. March's layout (`m_OriginColor` / `m_FadeOriginColor` / `m_fFadeOriginTime` / `m_NowColor` / `m_FadeColor` / `m_fFadeTime`) gives the same offsets.
- **`+0x8e39`:** `dispsweep.py 8e39` returns exactly the Done note's list. Dungeon `327cea70`: `@327cec78` comes after the `m_bFocusCamera` (`+0x8750`) / start-state / flag `0x20000000` gate, and `@327cecf8` after the state-id compare. Battle field `32d530a0 @32d53249` is reached from `@32d53122` (state flag `0x20000000`) after the focus handle `+0x2e4` is set. These are March's two `GetApplyCinematicUI()` readers (`X2DungeonGame.cpp:2372`, `X2BattleFieldGame.cpp:1080`).

**Checked further: the class writers (no visible change).** March writes `false` once, in Chung's `CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess`; Missile Shower 6019 and Quantum Ballista 6011 both use this state. The store now overwrites it at the NPC's next state start:
- the user's CONTROL_IDLE StateStart calls `StateChange( "SHOT_ARTILLERY_STRIKE_IDLE", true )`;
- Targeting_Chung has no `IS_APPLY_CINEMATIC_UI`, so the state start stores TRUE.

June does the same. It moved the write into CONTROL_IDLE's StateStartFuture (`32bff3d0 @32bff420`, registered `@32c1fffa`, state `0xf5`), and the StateStart (`32bff370`) then calls `3275da30( "SHOT_ARTILLERY_STRIKE_IDLE", 1 )`, which has no same-state skip. That is JUNE_BUGS 164.

Nothing changes on screen in either game. Both readers run only while the NPC is in its start state (`SHOT_ARTILLERY_STRIKE_START`) or in a state with `FOCUS_CAMERA_AT_CURRENT_STATE`, which Targeting_Chung never sets. The later states take the focus-force branch, which shows the UI. The relocation and June-only Heavy Field (`32c0ba60`, `CSI_SA_CTT_HEAVY_FIELD_LOOP` `0x151`, writes every frame) go to checklist **7r**. The June event NPCs (sniper, proton bomb) set FALSE in their own states.

**Fix:** `SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI_DEBUG` is OFF (`Always.h`).

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI --range d001ec0^..HEAD --with _DEBUG, _STATE_START_EFFECT_SET, _WORLD_COLOR_WINDOW`: `X2GUNPC.cpp` PASS +69, `X2GUNPC.h` PASS +3, `Always.h` DEFINE +40, 7 OTHER (peers' C9h-4h files, and `X2GameUnit.*`, which belong to `_WORLD_COLOR_WINDOW`), 0 FAIL, 0 ERROR, 0 deleted, RESULT PASS. `_WORLD_COLOR_WINDOW` on `d001ec0`: `X2GameUnit.cpp` / `.h` PASS, RESULT PASS. The one tokenless `#endif` is the house `_DEBUG` `#error` guard.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,776 B, dated 2026-10-10 12:20:24, md5 79ce7ab690266b5af16669106078cd29, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:20:19); built at mods/june-buff-port bf3c61e + 5 uncommitted code file(s).` The 4 other files are peers' in-progress C3s / C3q. `JUNE_FX C3o cinematic` is absent from the deployed exe; `IS_APPLY_CINEMATIC_UI` and `CHANGE_WORLD_COLOR_DATA` (ASCII) are present. No kom. Save: Freils (uid 18) got Missile Shower 6019 on slot C (backup `db_backup/els_db_20261010_120107_pre_sql.sql`).

**Regression play-test PASSED 2026-10-10** (user; exe `79ce7ab6`, logs after the deploy): 0 EXCEPTION, the only UNHANDLED is the baseline `E_DISABLE_HB_CHECK_REQ` x2, no `JUNE_FX C3o` line (`_DEBUG` off). Freils cast Missile Shower three times (`JUNE_FX MP_CRASH CDC_CHUNG_ARTILLERY_STRIKE_SHELL`, ErrorLog 646-684, 13:14:45 / 13:14:53 / 13:15:15); no HUD loss reported.

## C3p - port: June's skip-the-start (ESC) system and DIRECTION_STATE

**Status: done 2026-10-11, play-test PASSED (see C3p - Done; found by C3o). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3p.md`, review `kickoffs/june-effect/C3p-review.md`.

- **Scope:** the whole June-only chain: parser bit `0x4d` `DIRECTION_STATE`, the NPC CommonStateStart arming block (`327ac9d0 @327af25b-327af360`), game `+0xb40` (armed, sent, NPC id list), `322d8010` / `322f4c60` / `322f11d0` / `324cf360`, event `0x4ac` (the emulator answers it if the offline party path can reach it), the CX2State `+0x1ec / +0x1f0` setter `31ef7370`.
- **Visible:** 77 shipped NPC scripts set the key (dungeon openings). **Exit:** ESC skips an opening in game; funcmap rows `Effect C3p`.

### C3p - Done (2026-10-11, Opus 5.5 high)

**The chain** (11 funcmap rows `(C3p)`; flag `SERV_IRUHADEV_JUNE_SKIP_START`, play-test lines `_DEBUG` on). 81 June scripts set the key (`june_src_fixed`; the kickoff's 77 was low): openings, boss intros (`EVENT_*`, `Event_NPC_*`, START state, WAIT_STATE = `*_DYING`), tutorials, Elysion.
1. **Arm** (CommonStateStart `@327af25b`, before C3o's cinematic store): a DIRECTION_STATE state pushes the NPC's `GetUID()` (`+0x270`; game vslot `0xa8` = `GetNPCUnitByUID` `322ccac0` compares it) and sets armed; an empty list is reset first (`322d8010`).
2. **Hint while armed** (`32305be0`, the last call of CX2Game's render `3230aa60` = the end of March `CX2Game::OnFrameRender`): string 31002 "Press ESC key to end the video footage." (31003 "Waiting for party members..." once sent), font 2 = `XUF_DODUM_15_BOLD`, (970, 740), white, FS_SHELL, DT_RIGHT. Found by the raw `+0xb40..+0xb48` sweep; the kickoff did not know it.
3. **ESC** (`CX2State::HandleMsgByESC` = `31f1e1a0`, vslot `0x1e8`; `0x1ec` = `HandleMsgByESCImp`): armed and not sent -> sent, then solo `322f11d0`; true skips Imp. **Skip** (`322f11d0`): every user's start-motion delay (`+0x92cc`) to 0 unless it is -1, each listed NPC `StateChange( GetWaitStateID(), false )`, list cleared. The walk is over a copy (an ending DIRECTION state resets the list inside StateChange; June survives on cached begin / end).
4. **End of a DIRECTION_STATE state** (CommonStateEnd `@3277f863-3277f9b7`, not in the kickoff): disarm, every NPC's `m_bShowBossName` = false (`322d8070`; `+0x8ae9` proven by the `+0x8ae9 / +0x8aea / +0x8aeb` slow-motion test `@327ce3c4`), my unit's `m_fCanNotInputTime` (`+0xde8`, proven by CommonFrameMoveFuture's timer order) = 0, reset. JUNE_BUGS 176.
5. **`31ef7370( 0x1b, -1 )` is not VK_ESCAPE**: it is `SetMovieDescriptionData( 27, -1 )`, the subtitle clear (string 27 is empty). The setter is C3z-4b's (uncommitted while C3p ran); C3p calls it in the reset and the skip under a nested `#ifdef SERV_IRUHADEV_JUNE_MOVIE_DESCRIPTION`, so it compiles in when that flag does.
6. **Party path (event `0x4ac`) not built.** The emulator never forms a party (`Handlers_Social.cpp` answers every party request "no party"; the AI-party bots are NPCs), so `m_vecPartyMemberData.size() > 1` is never true offline. The branch sets sent and logs one line. June sends and receives the same id (dispatcher `324dbc20` case `0x4ac` -> `324cf360`); the name is XOR-built.
7. Not ported: June's other `HandleMsgByESC` gates (state `+0x210`, tutorial guide manager, chat box): they belong to June's ESC rewrite, not this chain. The extra June `OutTextXY` argument has no March parameter.

**Code:** `Always.h` (flag + `#error` needing `SERV_IRUHADEV_JUNE_BUFF_START_DELAY`), `X2GUNPC.h` (`FLAG_DIRECTION_STATE`), `X2GUNPC.cpp` (parser, arm, end), `X2Game.h/.cpp` (members, ctor reset, 6 methods, render call), `X2State.cpp` (ESC hook). Peers (C3z-4b) had uncommitted edits in all six files; this commit carries only C3p's hunks.

**Hygiene:** `flagcheck --worktree --files` (6 files): 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 untokened, 0 encoding changes, RESULT PASS (6 OTHER = the peer's C3z-4b hunks in the same files). Post-commit `--range`: see STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,570,176 B, dated 2026-10-11 15:45:42, md5 a3cf19b767317fc5cd4460e33b4e7277, equal to X2/US_SERVICE/x2.exe (2026-10-11 15:45:36); built at mods/june-buff-port 478322f + 21 uncommitted code file(s).` The others are the peer's in-progress C3z-4b. `JUNE_FX C3p arm / esc / skip / end` (UTF-16) and `DIRECTION_STATE` are in the deployed exe. The first build failed on `GET_STRING( a ? b : c )` (the macro does not parenthesise its argument); fixed with a local id. No kom.

**How to see it in game:** Underground Waterway (`DI_ELDER_BELOW_PASSAGE_NORMAL` 30120), stage 3 (STAGE2, the boss room): `NUI_EVENT_NPC_BENDERS_ELDER` (NPC_GROUP, START state `DIRECTION_STATE`, WAIT = DYING) with the boss Benders Nasod (`STOP_AT_START_STATE`, `SHOW_BOSS_NAME_DELAY_TIME` 9). The hint shows bottom right during the intro; ESC ends it.

**Play-test PASSED 2026-10-11** (user, reyaa; exe `a3cf19b7`, logs after the deploy, 0 EXCEPTION, only the baseline `E_DISABLE_HB_CHECK_REQ` x2, clean shutdown). Played Sander secret and Ranox Gateway instead of Underground Waterway:
- **ESC skip, two NPCs** (offline_server 564-574): `arm` `EVENT_SANDER_SECRET_3STAGE1_HENIR` + `EVENT_SANDER_SECRET_3STAGE2` (list 2) -> `esc: armed 1 sent 0` -> both `state 6 -> WAIT_STATE 7` -> two `end` lines.
- **ESC skip, Gateway boss intro** (1260-1264, C3z-7's gate): `arm` `EVENT_GATEWAY_BOSS` -> ESC 0.5 s later -> `state 6 -> WAIT_STATE 7` -> `end`; the event NPC died `KILL_SELF` (its DYING state) and Ignia fought (hits on 1000014 / 16 / 17 from 16:17:54).
- **No ESC** (973-978): Gateway intro armed at 16:16:47 and ended on its own at 16:16:57 (`end`, input unlocked); later ESC presses (`armed 0`) went to March's normal ESC menu.
- `0 user start-motion delay(s) zeroed` in all three: these rooms have no start-motion wait (delay already -1). `80 NPC boss-name flags` is the NPC list size.

## C3q - port: June's NPC effect-set list (EFFECT_SET_LIST_FINAL, CONSUME_MP)

**Status: done 2026-10-10 (see C3q - Done; found by C3o), regression play-test passed. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3q.md`, review `kickoffs/june-effect/C3q-review.md`.

- **Scope:** June's `CX2GUNPC::CommonFrameMove_EffectSet` body (`32776e10`: 0x3c-byte entries, CONSUME_MP gate through vslot `0xe4`, hyper bit `0x200000`, delete lists by flags `0x8000 / 0x4000 / 0x10000 / 0x20000`) together with the `EFFECT_SET_LIST_FINAL` block of the state loader `32784c00` that fills it.
- **Data:** 10 shipped PvP-hero NPC scripts (`PVP_HERO_*`, not the AI-party `CSM_PVP_HERO_*`); offline has no PvP, so a regression play-test unless a reachable spawn is found.

### C3q - Done (2026-10-10, Opus 5.5 high)

**What June does** (funcmap rows `(C3q)`):
1. **Loader** `32784c00 @32787475-32787d29`. It reads `EFFECT_SET_LIST` (pairs), then `_EX` (tables), then **`EFFECT_SET_LIST_FINAL`**, and appends all three to the same list (state `+0x1e4`). It then sorts the list with `327c8b30`, a stable sort on the start time (= March's `operator<`). **Nothing wins:** a state with several lists plays all of them, and a tie keeps the order LIST, EX, FINAL. BIX, EOR and ZENITH carry the same effect set in both EX and FINAL, so June plays it twice. That is data, so it gets no bug row.
   - FINAL keys are `sprintf_s( "%s%d", "EFFECT_SET", n )` from **n = 0**. All 10 scripts start at `EFFECT_SET0`.
   - Each entry: `EFFECT_NAME` (a miss gives L""), `START_TIME` (miss 0), `CONSUME_MP` (miss -1), and `DELETE_CONDITION` = { ints }.
   - The reset `32079980` seeds the delete conditions with `{ DCES_NONE }`; EX entries get no seed. When the table is present, it clears the seed (`3221db80`) and refills it.
   - An empty name ends the whole FINAL list (`@32787984`). FINAL has no `START_TIME >= 0` filter (LIST and EX have one): JUNE_BUGS 163, latent.
2. **Entry** = 0x3c bytes. `+0x30` = CONSUME_MP: -1 from all three ctors (`32079750` / `320797f0` / `32079890`) and the reset. `+0x34` (byte) and `+0x38` (int) are read by neither the NPC loader nor the player. They look like the user parser `3200b030`'s `IS_LEARN_SKILL_CHECK` / `RELATE_SKILL_ID`, so they belong to the checklist (the user `EFFECT_SET_LIST_FINAL` row) and are not ported.
3. **Player** `32776e10`: the same as March's `CommonFrameMove_EffectSet` except for the gate `@32776f10-32776f39`.
   - `+0x30 > 0` and `vslot 0xe4( +0x30, 0 ) != 1` skip the entry, and the cursor still advances (`@327770b5`).
   - Checked equal to March: the flag bits (`statedata_keys.py`: `0x4000` STATE_END, `0x8000` DAMAGE_REACT, `0x10000` DIE, `0x20000` ON_CUSTOM_STATE, `0x200000` HYPER), the push order, the `PlayEffectSet 321e1e60` arguments (March's defaults plus June's trailing 1.0), the `+0xc4` animation-timing clear and `3227a570`.
4. **vslot `0xe4` = `CX2GameUnit::FlushMp`** (`3208dc70`). It calls the gage's slot `0x2c` (`32a31910`): `( fMp, IsMyUnit(), 0 )`. That is March's `CX2GageData::FlushMp` (fails when MP < cost, else `UpNowMp( -cost )`).
   - `UpNowMp` `322b33d0` has a June-only accounting tail (gage `+0x100` / `+0x104`, counter `32676150( 9 )`). It runs only when `IsMyUnit()` is true. For an NPC that is `32756c10` = `xor al, al`, so March's `FlushMp( cost )` is exact.

**Code** (flag `SERV_IRUHADEV_JUNE_NPC_EFFECT_SET_LIST`; `X2GameUnit.h`, `X2GUNPC.cpp`, `Always.h`):
- `EffectSetToPlay::m_fJuneConsumeMP` = -1 in the 3 live ctors. User entries keep -1, so the user player is unchanged.
- `LoadStateData`: FINAL's indexed tables are added to the reserve count, and the FINAL block sits after EX, before the sort.
- `CommonFrameMove_EffectSet`: `if ( c > 0.f && false == FlushMp( c ) ) continue;` inside the start-time branch.
- No `_DEBUG` flag: no reachable data could fire it (below).

**Reachability: none.** `EFFECT_SET_LIST_FINAL` is in exactly the 10 `data037` `PVP_HERO_*` scripts (kom_contents grep). None can spawn in March: no `NUI_PVP_HERO_BIX` (etc.) in `X2UnitManager.h` or `Enum.lua`, and March's `NPCTemplet.lua` names none of them. The play-test is therefore a regression check only. It cannot exercise the new loop or the gate, and the review (`C3q-review.md`) is the only line-by-line check of them.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_NPC_EFFECT_SET_LIST --worktree --files X2GUNPC.cpp X2GameUnit.h Always.h`: `X2GameUnit.h` PASS +12. `X2GUNPC.cpp` and `Always.h` show OTHER only from peers' uncommitted hunks (`_WORLD_COLOR_WINDOW`, C3r / C3s / C3o-review defines). 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 encoding changes, RESULT PASS. The commit stages only C3q's hunks (HEAD-based blobs). Post-commit `--range`: see STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,776 B, dated 2026-10-10 12:20:24, md5 79ce7ab690266b5af16669106078cd29, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:20:19); built at mods/june-buff-port 3eb8ef0 + 8 uncommitted code file(s).` The other uncommitted files are peers' C3r / C3s / C3o-review work. `EFFECT_SET_LIST_FINAL` is in the deployed exe (2 hits), and `X2GUNPC.obj` is newer than the source. No kom.

**How to see it in game:** nothing new can be seen. Regression: monsters' effect sets (Tree of El, any dungeon) play as before, and the log shows 0 EXCEPTION.

**Regression play-test PASSED 2026-10-10** (user, Tree of El through auto-party, so the AI heroes loaded their EX lists through the changed loader; exe `79ce7ab6`, run after the deploy, clean exit): 0 EXCEPTION, the only UNHANDLED is the baseline `E_DISABLE_HB_CHECK_REQ` x4, and no log line names `EFFECT_SET_LIST_FINAL`, `DELETE_CONDITION` or `CONSUME_MP`.

### C3q - Review (2026-10-14, Opus 5.5 high; no transcription errors, no code change, no build)

Commit `d31391a` was checked line by line against June (`disasm_range.py`, raw exe reads). The code is unchanged and the exe stays `79ce7ab6`. Every kickoff item matches:
- **Keys:** the three XOR'd keys decode to `EFFECT_NAME` (`337a9a84`), `START_TIME` (`337a9a78`) and `CONSUME_MP` (`337a9a6c`). The plain strings are `33476e64` = `EFFECT_SET_LIST_FINAL`, `33476e18` = `EFFECT_SET`, `33449104` = `%s%d` and `33476588` = `DELETE_CONDITION`. `sprintf_s( buf, 0xff, ... )` runs from n = 0 (`ebp` = 0, `@327877d4`) and again after `inc ebp` (`@32787c78`). The port's `StringCchPrintfA` into a 256-byte buffer is equivalent.
- **Reset:** `32079980` sets the name to L"", `+0x1c` to 0, `+0x30` to -1 (`334380f0` reads -1.0), the conditions to `{ 0 }` and `+0x38` to 0, which equals the port's fresh default-ctor entry per iteration. June instead reuses one entry (`32079750` before the loop, the reset at the top of each pass). The pushed copies are the same.
- **Miss defaults:** START_TIME miss is `fldz` (`@32787a8c`) and CONSUME_MP miss is -1 (`@32787b95`). Both getters are `32fea370` (float). The name getter `32feae90` converts with CP_UTF8 (`0xfde9`), as March's `KLuaManager::GetValue( const char*, wstring& )` does.
- **Empty name:** `31c1c1a0` = `compare( 0, size, L"", 0 )`, zero -> `@32787caf`. That path runs EndTable (EFFECT_SETn), then `@32787cc4` EndTable (FINAL), the same as the port's `EndTable; break;` plus the outer EndTable.
- **DELETE_CONDITION:** BeginTable, then `3221db80` (clear), then `32fea6e0( edi = 1.., &int )` pushes until a miss, then EndTable. March's EX loop uses the same getter shape (`GetValue( int, UINT& )`), so the port matches.
- **Push:** `320840c0 @32787c5a` with no time filter (JUNE_BUGS 163 stands).
- **EndTable balance on every path:** an EFFECT_SET0 miss -> `@32787cc4` (FINAL only); an empty name -> n + FINAL; normal -> DELETE_CONDITION (when opened), n at `@32787c8f`, and FINAL once the next BeginTable misses (`@32787cad -> @32787cc2`). The port balances the same way on all four paths.
- **Order and sort:**
  - The reserve `@327874c0-32787503` adds the FINAL count with the same counter `32fe9dc0` that EX uses (= March's `GetNumIndexedTables`).
  - LIST, EX and FINAL are appended in that order (`3278775c` closes EX, `32787775` opens FINAL).
  - `327c8b30` runs on a non-empty list only. Its insertion step compares `[+0x1c]` with `fcompp` / `test ah, 0x41` (strict <) and copies `+0x30 / +0x34 / +0x38`. March's `stable_sort` uses the implicit copy, which carries `m_fJuneConsumeMP`.
- **Gate `@32776f10-32776f39`:**
  - `fld [+0x30]; fcomp 0.0; test ah, 0x41; jne play`. So CONSUME_MP <= 0 or NaN plays the entry.
  - Otherwise the gate calls `[vtbl + 0xe4]( c, 0 )`. `cmp al, 1; jne @327770b5`, and `@327770b5` = `inc [esi + 0x8f18]`. That skips the play, the four delete-list pushes and `3227a570`, which is exactly the port's `continue` in the `for` that does `++m_iEffectSetToPlayProgress`.
  - The start-time test before the gate is March's (`0 == t || now >= t`; a NaN breaks).
- **Dispatch:** all four vtables that hold the NPC's `IsMyUnit` `32756c10` (bases `334b8084 / 334b8504 / 334b8984 / 334ba584`) have slot `0xe4` = `3208dc70` and slot `0x204` = `32759630`.
  - `3208dc70` calls gage slot `0x2c`. All seven gage vtables that hold `UpNowMp 322b33d0` have `0x2c` = `32a31910`, so no gage class overrides it.
  - `32a31910`: a null MP gage or now < cost fails, otherwise `UpNowMp( -cost )` and true. That is `X2GageData.cpp:95` `CX2GageData::FlushMp` (March's NPC gage is the base class: `CX2GameUnit::CreateGageData`, not overridden by `CX2GUNPC`).
- **Struct:** `m_fJuneConsumeMP` is -1 in all three ctors. Its only readers are the FINAL loader and the NPC player (`X2GUNPC.cpp:5586`, `:33632`), so the user player is unchanged. June's `(name, time)` ctor `320797f0` leaves `+0x34` unset, but nothing reads it; it is not ported.
- **Rest of `32776e10`:** flag words `0x4000 / 0x8000 / 0x10000 / 0x20000 / 0x200000`, push order `+0x3eb0` (DAMAGE_REACT), `+0x3ec0` (STATE_END), `+0x3ed0` (DIE), `+0x3ef0` (CUSTOM), the `+0xc4 = 0` clear on a nonzero start, then `3227a570( handle, entry + 0x20 )`.
  - Those are all March's.
  - `321e1e60`'s trailing 1.0 is the June-only `fForceDownValueRate` default (funcmap `321e1e60`, Phase 4a). It belongs to March's existing call, not to C3q's diff.

**Hygiene:** flagcheck `--range d31391a^..d31391a`: 2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL-mismatched, RESULT PASS. `git log -S` shows that no later commit adds or removes the flag token; `..HEAD` scans every peer commit and timed out at 580 s.

**User owes nothing.** The new loop and gate stay unreachable in March (the 10 `PVP_HERO_*` scripts cannot spawn).

## C3r - port: June's user CHANGE_WORLD_COLOR_DATA window

**Status: done 2026-10-10 (see "C3r - Done"; found by C3o). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3r.md`, review `C3r-review.md`.

- **Scope:** the user side of C3o's world-colour window: `CX2GUUser::CommonStateStart 32057b80 @32060286-320603b2`, the reader `3204f190 @3204fb99`, `31fd3260` and its caller `32063650 @3206cdb9-3206cdcb`. Same flag as C3o. Dormant (only Ain's class script has the table).

### C3r - Done (2026-10-10, Opus 5.5 high)

**Mapped** (funcmap rows `(C3r)`; Ghidra, `xordec2.py`, a raw rel32 scan of `.text`):
1. **Parse** `32057b80 @32060286-32060497`. The keys around it are `CHANGE_WORLD_COLOR` (`+0x927c`, March `m_bChangeWorldColor`), then `SHOW_SKILL_CUT_IN_TIME` (`+0x9278`), then the table, then `SHOW_HYPER_CUT_IN_TIME` (`+0x9280`, June-only -> **C3t**).
   - **The kickoff's `[ebx]` question:** `ebx = edi + 0x9278` (`@32060255`), so the `@32060290` store is `SHOW_SKILL_CUT_IN_TIME`'s miss default of -1, which March's `LUA_GET_VALUE( ..., -1.f )` already does.
   - The table parse is the NPC's: both times -1, START miss 0 (`@320603b0`), END miss 9999 (`@3206048b`).
2. **`31fd3260`** (one caller, `32063650 @3206cdb9`) = `m_bChangeWorldColor` (`@31fd3266`), else START >= 0 and START <= t <= END. t is the Now anim's time: `32071d80` gives `+0x87d4` = `m_pXSkinAnim`, or June's second-body anim `+0x8e88` while `+0x8e90` is on (not in March).
3. **The caller** replaces March's `if( m_bChangeWorldColor ) ChangeWorldColorByHyperMode();`: true -> `32001460`, false -> `32296be0`.
   - `32001460` is March's `ChangeWorldColorByHyperMode`, constant for constant: `0xffff0000` in hyper mode, else `0x22 / 255` grey.
   - So **the user window tints by hyper mode**, where the NPC's (C3o) is fixed red.
4. **`32001380` = `GetShowCutInAndChangeWorldColor`**:
   - Game types 1-2 return true, and 3-5 take the battle-field test. March's enum ends at 3.
   - June adds a `GetMyUnit()` NULL test (`@320013f5`) before the distance test (640000 = `LIMIT_DISTANCE_SQ`).
5. **`3204f190 @3204fb99` is not world colour.** C3o's scan matched an offset.
   - EBX there is the socket templet: the same EBX reads `+0x27c` (`m_eHyperModeEffectiveType`) a few lines later.
   - So `+0x240 / +0x244 / +0x248` are March's `m_iSummonNpcId / m_fSummonNpcRate / m_fSummonNpcCoolTime`, pushed onto `m_vecSummonNpcSocket` (`+0x8d80`).
   - June's one addition there is a dungeon-only gate for NPC ids `0x1017 / 0x1018`, routed to **C3u**.
6. **C3o missed a third restore caller:** NPC `CommonStateEnd` `3277ee10` ends in `jmp 0x32296be0` (`@3277fbe0`) on every path.
   - The raw scan finds `32296be0` at exactly 3 sites: user CommonFrameMove, NPC CommonFrameMove, NPC CommonStateEnd.
   - The user's `CommonStateEnd` still fades back itself (June `32015250 @3201572f-3201574f`, March after `TRANSITION`).
   - So **JUNE_BUGS 156 is withdrawn**: a state cut short inside its window fades back on both sides.

**Code** (`X2GUUser.h/.cpp`, `X2GUNPC.cpp`, `Always.h`):
- **`SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW`** (C3o's flag):
  - the table parse after `SHOW_SKILL_CUT_IN_TIME`;
  - `CX2GUUser::IsJuneChangeWorldColorNow` (`31fd3260`; the anim null check is ours);
  - the CommonFrameMove gate, with March's line in `#else`;
  - `RestoreJuneWorldColor()` as the last statement of `CX2GUNPC::CommonStateEnd`.
- **`SERV_IRUHADEV_JUNE_CUTIN_MY_UNIT_GUARD`** (new): the battle-field case of `GetShowCutInAndChangeWorldColor`, with March's in `#else`.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW --with SERV_IRUHADEV_JUNE_CUTIN_MY_UNIT_GUARD --worktree` (C3r's files):
- `X2GUUser.cpp` PASS +56, `X2GUUser.h` PASS +3, `X2GUNPC.cpp` PASS +6, `Always.h` OTHER (the define, plus a peer's line).
- 0 FAIL, 0 ERROR, 0 deleted, 0 non-ASCII, 0 tokenless `#else` / `#endif`, RESULT PASS.
- Post-commit (`--range 268627e^..HEAD`, same `--with`): `files: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,776 B, dated 2026-10-10 12:20:24, md5 79ce7ab690266b5af16669106078cd29, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:20:19); built at mods/june-buff-port 3eb8ef0 + 8 uncommitted code file(s).` The exe was linked by a peer's build at 12:20 from this tree, after C3r's edits (`X2GUUser.obj` 12:14 > `X2GUUser.cpp` 11:58, `X2GUNPC.obj` 12:20), so it carries C3r; the uncommitted files include peers' C3q / C3s work. No kom.

**How to see it in game:**
- **Ain's window:** dormant, since Ain is not in March.
- **The replaced branch (regression):** every March `CHANGE_WORLD_COLOR = TRUE` state now runs through the June gate. Examples: Elesis's special actives (e.g. Grand Master `ESI_SA_SGM_SONIC_BLADE`), Add's SAs, Ara's, and every class's hyper active.
  - The world goes dark grey during the skill, or red while awakened, and fades back when the state ends, as before.
- **NPC state-end fade:** dormant, since no NPC script has the table.

**Regression play-test PASSED 2026-10-10** (user; ElesisGM, Grand Master, Sonic Blade: dungeon normal grey, dungeon awakened red, battle field grey, each fading back at skill end). Logs: run 12:39-12:43 after the deploy on exe `79ce7ab6`, 0 EXCEPTION, the only UNHANDLED is the baseline `E_DISABLE_HB_CHECK_REQ` x4, clean-exit census. C3r writes no log lines; the verdict is visual.

No questions for the user.

## C3s - identify, then port: June's UpdateUnitPointer

**Status: done 2026-10-10; regression play-test pending (found by C3o). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3s.md`, review `C3s-review.md`.

- **Scope:** June's dungeon `327cea70` and battle-field `32d530a0` focus-camera walks against March's `UpdateUnitPointer` (`+0x8e38`, anim-name compares, state flags `0x2000000` / `0x20000000`, the cinematic-UI hide by main-state id). One flag per logical change.

### C3s - Done (2026-10-10, Opus 5.5 high)

**Identified** (funcmap rows `(C3s)`). Both June functions return bool and start with the base `CX2Game::UpdateUnitPointer` (`322d5f70`), returning 1 when it does.

1. **`+0x8f8c` is not an animation name. It is a narrow `std::string` read from `FORCE_CINEMATIC_STATE`** in INIT_COMPONENT: `327a0fa0 @327ab5fe`, right after `BUFF_GROUP_ID`; a miss assigns `""`. `3275aaf0` = `GetStateID( string )`, `+0x8454` = now-state, `327d65d0` = "string not empty". Only June `Secret_Mad_Trock_Summoner` sets it (`"MAD_TROCK_SUMMONER_START"`), and no NPCTemplet, March or June, names that script.
2. **Dungeon `327cea70`** = March's `MODIFY_DUNGEON_STAGING` walk, except where a `FORCE_CINEMATIC_STATE` is set:
   - `CanNotInputAndDisalbeNPCAI( now == GetStateID( name ) )` replaces the stop-at-start test, whether or not `m_bStopAtStartState` (`+0x8e38`) is set.
   - The same compare replaces **both** focus conditions (`m_bFocusCamera` + start state, and `FOCUS_CAMERA_AT_CURRENT_STATE`).
   - Without the key it is March's code.
3. **Members and keys check out against March:**
   - The flags `0x2000000` (word `+4`) and `0x20000000` (word `0`) are `STOP_AT_CURRENT_STATE` (bit `0x39`) and `FOCUS_CAMERA_AT_CURRENT_STATE` (bit `0x1d`), the same as March's `NOWSTATE_FLAG` getters.
   - `+0x8e38` is `m_bStopAtStartState`: cleared in CommonStateEnd and set by CreateGUNPC, as in March.
   - `+0x8750` is `m_bFocusCamera` and `+0x8d34` is `m_bFocusCameraForce`; the revert to my-unit is March's.
   - `322cef70` is `CanNotInputAndDisalbeNPCAI`.
4. **The state-id jump tables are March's `IsInheritStateMenu`, inlined.** The 26 bytes give false for the same nine March states, plus June-only `PVE_RESULT` / `PHOTOZONE_LOBBY` / `PHOTOZONE_GAME`, and `g_pMain +0x90 -> +0x3e8` is `SetShowUI`. No change.
5. **Battle field `32d530a0`** = March with `MODIFY_DUNGEON_STAGING` and `SERV_BATTLEFIELD_MIDDLE_BOSS` on (flagprobe: both ON). `+0x8e80` is `m_bIsMainBoss`, and the `+0x8c4` bytes 0 / 1 are `CinematicUI::m_bStart` / `m_bEnd`, so the check is `GetIsCinematicUIActive`. **New:** June returns before the base call when `GetMyUnit()` is NULL. March's only direct call (`X2BattleFieldGame.cpp`, the leave handler) runs with a my-unit, so only the virtual calls made while your unit is still loading change. Host setup does not depend on those calls: once your unit exists, `UpdateMyGameUnit` (`X2Game.cpp:2322`) calls `SetHostGameUnit` / `UpdateHostGameUnit` itself, and a host change calls it from `X2State.cpp`.
6. **Routed:**
   - The base `322d5f70`: a timed second focus unit (`+0x2f0`..`+0x301`), the bool signature and the derived early returns, including `X2PVPGame.cpp`. -> **C3v**.
   - June's `ShowBossNameAtStart` `327ef160`: the forced-state boss intro, several boss names, game type 4. -> **C3w** (whole).
   - The seven keys June reads after `FORCE_CINEMATIC_STATE` (`NOT_CHECK_INDICATOR` ...) are Atlas A1b / A4's unit-script keys.

**Code** (`X2GUNPC.h/.cpp`, `X2DungeonGame.cpp`, `X2BattleFieldGame.cpp`, `Always.h`):
- `SERV_IRUHADEV_JUNE_FORCE_CINEMATIC_STATE`: `m_strJuneForceCinematicState` + `GetJuneForceCinematicState` / `HasJuneForceCinematicState`, the parse after `BUFF_GROUP_ID`, and the dungeon walk (the stop-at-start block and the focus condition, March's in `#else`).
- `SERV_IRUHADEV_JUNE_BF_FOCUS_NEEDS_MY_UNIT`: the battle-field guard before `CX2Game::UpdateUnitPointer()`.

**Hygiene:** `flagcheck --worktree --files` (C3s's files only):
- `_FORCE_CINEMATIC_STATE`: `X2DungeonGame.cpp` PASS +25, `X2GUNPC.h` PASS +9. `X2GUNPC.cpp` OTHER: the peers' uncommitted C3q / C3r edits; C3s adds 6 lines there.
- `_BF_FOCUS_NEEDS_MY_UNIT`: `X2BattleFieldGame.cpp` PASS +6.
- `Always.h` OTHER: the peers' defines plus these two.
- Both flags: RESULT PASS, 0 deleted lines in their files, 0 non-ASCII, 0 EOL mismatches, 0 untokened `#else` / `#endif`.
- Post-commit `--range` lines: see STATUS (commit `752136f`).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,776 B, dated 2026-10-10 12:20:24, md5 79ce7ab690266b5af16669106078cd29, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:20:19); built at mods/june-buff-port 085ddf9 + 8 uncommitted code file(s).` The other files are the peers' uncommitted C3q / C3r edits. No kom.

**How to see it in game:** nothing new can be seen. The one script with the key is not spawned by any templet, so this is a regression play-test.
- Dungeon openings with a focus camera (stop-at-start, boss focus) should behave as before. Test with Elder Druid Forest (Normal): its boss `NUI_ANGER_FOREST_ENT` sets `FOCUS_CAMERA` and `STOP_AT_START_STATE`, which runs the restructured empty-string path.
- A battle field should still focus your character and the field boss.

**Play-test 1 (2026-10-10): crash entering Elder Druid Forest. Not C3s.** The crash bundle resolves to `CX2DungeonGame::StageLoading+0x899`,
which reads `[NULL+0x34]`: `m_pWorld->GetLineMap()` at `X2DungeonGame.cpp:895` with `m_pWorld` NULL.
- The adopted June scripts name June's reworked worlds (`WI_ELDER_DARK_FOREST_*`), and March has none of them, so CreateWorld returns NULL
  without logging -> **C3x**.
- A census found 128 such world IDs in 73 June dungeon scripts. Only 7 are registered: Druid Forest x3, Benders Cave x3 (boss stage) and
  Henir Space (Elysion / Atlas / Sander boss stages).
- **Monster assets (user, 2026-10-10: copy them all).** `npc_mesh_census.py --all-npc` scanned all 3,364 unit scripts and wrote
  `june2017_re/npc_mesh_need_all.tsv`: 1,666 June files, about 1.17 GB.
  - `kom.py stage-june` placed them: 1,666 new, 0 replaced, 0 refused, in 62 koms (14 of them new archive numbers, all inside the
    `SERV_IRUHADEV_KOM_FILE_COUNT_226` mount range).
  - `data118_kom` first got its missing `blur_16.dds` back from the live kom.
  - All 62 PACK OK, each `-0 removed` against the live archive. Copy waits for the user's OK.
- Re-test C3s in Elder Wally's Castle (Normal), whose maps are identical to June's. (The Roof's stages 1-2 are among the 137 reworked maps, see C3x.)

## C3v - identify, then port: June's CX2Game::UpdateUnitPointer

**Status: done 2026-10-12, play-test owed (see C3v - Done). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3v.md`.

- **Scope:** June's base `322d5f70` (a timed second focus unit `+0x2f0`..`+0x301`, CanNotInput while it holds, two camera calls, returns bool) against March `X2Game.cpp` (`m_fFocusTime` only); the bool signature and `if( base() ) return true;` in every override (dungeon, battle field after C3s's guard, `X2PVPGame.cpp` = June caller `328ba013`); the writers of the timed focus.

### C3v - Done (2026-10-12, Opus 5.5 high)

**Identified** (funcmap rows `(C3v)`, 13 rows):
- **`+0x2ec` is a small struct, June's replacement for `m_fFocusTime`.** It has a vtable `0x3348a4b8` and holds a focus handle `+0x2f0`, a time `+0x2f8`, a distance `+0x2fc` (-1 means the option's distance) and two bools `+0x300` / `+0x301`. Its ctor is `32311680`, operator= `323108d0`, Reset `32311720`.
- **The writer is June's `SetTimerFocusUnit` `322d63f0`** (vslot 0x88). It takes 5 arguments (the two bools are new) and stores them, and it no longer points the camera itself: only `OnFrameMove( 0, 0 )` is left.
- **`322d5ef0` is `UpdateHostGameUnit`**, the same as March.
- **The base `322d5f70` keeps the camera on the timed unit until the time runs out.** It calls `NomalDirectCamera` every frame, or `NomalTrackingCamera` when `+0x301` is set. It returns 1 while the hold lasts and on the frame it ends. When the focus moves to another unit first, it cancels and returns 0. Every path ends in `ResetFocusUnit`.
  - March's block, by contrast, put the focus back on my unit the very next frame.
- **The overrides:**
  - Dungeon `327cea70`: returns 1 on every path.
  - PvP `328ba010` (RTTI `CX2PVPGame`): returns 1 on every path.
  - Battle field `32d530a0`: returns 0 only on C3s's no-my-unit guard.
  - In all three, base true returns immediately and skips the NPC walk.
- **Callers:** every June caller passes `false, false`, except the absolute-monster setup `3277cf10` (3.0 s, true, true). That setup is not ported, because the data is server-sent. So in the port the CanNotInput branch and the tracking-camera branch are unreachable.
  - The dungeon sites are March's lines 1719 / 1724 / 1812-1817 / 1909 (800 = `0x3344cc20`, 1.5 = `0x33471a1c`).
  - The extra sites (`CX2PVPGameAmusementItem 32d58e90`, the `32ee` / `32ef` modes) are PvP only.
- **Not ported:**
  - The unit's `+0x240` byte (1 while held). It has no June reader; `31e4df40` is a `CX2SquareUnit` getter.
  - June's extra camera arguments. `NomalDirectCamera`'s 7th is never read. `NomalTrackingCamera`'s last two are unread here and only reachable through the absolute monster. March's calls are used for both.

**Code** (`SERV_IRUHADEV_JUNE_TIMER_FOCUS_UNIT`; one flag, because a bool override over a void base does not compile):
- `X2Game.h/.cpp`:
  - `JuneTimerFocusUnit m_kJuneTimerFocusUnit`. `m_fFocusTime` stays, unused.
  - `SetTimerFocusUnit( ..., bool bCanNotInput = false, bool bTrackingCamera = false )` with June's body.
  - `bool UpdateUnitPointer()` with June's body after `UpdateHostGameUnit`.
- Dungeon / battle field / PvP `.h/.cpp`:
  - `bool UpdateUnitPointer()`: the C3s guard in the battle field, then `if( base ) return true;`, then `JuneUpdateUnitPointerNPCWalk()`, then `return true`.
  - The walk is March's body, moved verbatim. Its base call is in `#else`.
- `SERV_IRUHADEV_JUNE_C3V_DEBUG` (short-lived): `JUNE_FX C3v arm / expire / cancel` lines in `offline_server.log`.
- `JUNE_BUGS.md` row 182: cancel and expiry always call `CanNotInputAndDisalbeNPCAI( false )`; a time-0 arm lingers until the focus moves.

**Hygiene:** `flagcheck JUNE_TIMER_FOCUS_UNIT --worktree --files` (the 8 X2Lib files + Always.h):
- 8 PASS, 1 OTHER (Always.h, the debug flag's define). RESULT PASS.
- 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 untokened `#else` / `#endif`, 0 encoding changes.
- `_C3V_DEBUG`: RESULT PASS, same zeros.
- Post-commit `--range` lines: see STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,572,224 B, dated 2026-10-12 04:23:13, md5 73840b31d475ee7d27dc842c14d65bc4, equal to X2/US_SERVICE/x2.exe (2026-10-12 04:23:08); built at mods/june-buff-port b4fa118 + 9 uncommitted code file(s).` The 9 files are C3v's own. `JUNE_FX C3v` is in the exe (3 hits). No kom.

**How to see it in game:** kill a dungeon boss as the last kill, e.g. Elder Wally's Castle (Normal). The camera should hold on the dying boss at distance 800 for 1.5 s, then return to you. The log should show `JUNE_FX C3v arm: unit type 1, time 1.50, distance 800`, then `expire`. Regression: stage openings with a focus-camera boss still lock input and show the boss.

**Play-test (2026-10-12, Elder Wally's Castle Normal, jtLordKnight):** `offline_server.log` shows one `JUNE_FX C3v arm: unit type 1, time 1.50, distance 800, can-not-input 0, tracking 0` (04:26:52.071) and one `expire: focus back to my unit ( 1 )` (04:26:55.898), with no `cancel`. Arm to expire took 3.8 s of real time for a 1.5 s timer that counts `m_fElapsedTime`. The likely cause is the last-kill slow motion slowing the game clock, but that is not traced; the review checks it. The user confirmed on screen that the close-up held on the boss and then returned, and that the boss-intro input lock is unchanged. The battle-field check was skipped (the user's call: a field boss is a hassle to spawn). The only UNHANDLED packets are `E_DISABLE_HB_CHECK_REQ` x2, which predate this phase.

## C3w - port: June's ShowBossNameAtStart

**Status: done 2026-10-14, play-test PASSED 2026-10-14; review next. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3w.md`.

- **Scope:** June `CX2DungeonSubStage::ShowBossNameAtStart 327ef160` whole: game type 2 or 4, the `FORCE_CINEMATIC_STATE` gate (C3s's member) that bypasses `m_bShowBossName` and its delay, several boss names through `327cc7c0` with a counter `<= 7`, the danger particles and the sub-boss branch.

### C3w - Done (2026-10-14, Opus 5.5 high)

**Identified** (funcmap rows `(C3w)`; raw disasm of `327ef160`, `327cc7c0`, the ctor `327cef70`, `Release` `327d2240`):
1. **`327cc7c0` = `CX2DungeonGame::ShowBossName( const WCHAR* tex, const int& n )`.** It returns when `n` is outside 0..7. Otherwise it
   creates the sequence `L"BOSS_NAME" + n` with March's `D3DXVECTOR3` defaults, calls `ChangeTexForce( tex )` and stores the handle in slot
   `n` (`+0xcf8 + 4n`).
   - March's single `m_hBossName` is an array of 8 in June. The ctor sets each slot to -1 and `Release` destroys each valid one.
   - `+0xd18` is the next member (the wave id, `327d0870`), so June keeps no other boss-name handle.
2. **The gate.** An NPC gets the boss intro when (`m_bShowBossName` and the delay < sub-stage time) **or** `FORCE_CINEMATIC_STATE` is set
   (`std::string::compare` with `""` at `@327ef30e`).
   - With a forced state, the intro also needs now-state == `GetStateID( name )`. On a mismatch the NPC is skipped, sub-boss branch included.
   - Without the intro, the sub-boss branch is March's.
   - Both particles, their arguments and the stores into `+0x68` / `+0x6c` match March. The one exception is June's 14th `CreateSequence`
     argument, -1, pushed at every call. March has no such parameter.
3. **Several names.** A per-call counter goes to `327cc7c0` while it is `<= 7`, and goes up only when a name was shown.
   - A ninth boss, or one with no texture, still gets the danger particles and sets `m_bBossNameWasShown`.
   - The loop never breaks (March's). Entry, `0.01f` and the caller's place are unchanged.
4. **Game type 4** is June's `GT_PVE`: ported as a literal 4 (precedent `X2PremiumBuffManager.cpp:547`). The dungeon game comes from a
   `dynamic_cast` as in June (`31e94080`), with a NULL check June lacks. The check is unreachable here.

**Data:** March's `GameMinorParticle.txt` has only `BOSS_NAME`, and without `BOSS_NAME0..7` every boss name would vanish.
- June's 8 blocks were inserted byte-exact (4,455 B, CRLF) right after `BOSS_NAME`, as in June's file.
- June's `BOSS_NAME` equals March's, and its keys are March's.
- The original is in `24965799/kom_backup_c3w/20261014_193716/`. The 10-12 full particle-file replace (`97df3a0`) is undone on disk and was
  **not** re-run.
- `PACK OK data036` (`~1 changed: GameMinorParticle.txt`). **The copy waits for the user.**

**Census** (scratch `census.py`, June `june_src_fixed`): 14 NPC groups in 5 scripts reach the several-names path, all `STAGE3 / SUB_STAGE0`.
- Behimoss Heart Normal / Hard / Expert, groups 0-1 (Icoos + Anu, Anu + Eris; group 2 = Waldomenation alone).
- Karuso Village Hard / Expert (two bosses each).
- `FORCE_CINEMATIC_STATE` is still only in `Secret_Mad_Trock_Summoner`, which no templet spawns, so the forced path cannot be seen in game.

**Code** (`X2DungeonSubStage.cpp`, `X2DungeonGame.h/.cpp`, `Always.h`):
- `SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START`: the function (March's in `#else`), the overload, the slot array with its ctor and `Release`
  loops, and a `JUNE_FX C3w ShowBossName: particle sequence ... not found` log line when a `BOSS_NAMEn` is missing.
  - `Always.h` `#error`s without `_FORCE_CINEMATIC_STATE`.
- Short-lived `SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START_DEBUG` (**on**; off in the review): one `JUNE_FX C3w` line per banner,
  sub-boss and nameless boss.

**Hygiene:** `flagcheck --worktree --files` (the 4 files):
- `_SHOW_BOSS_NAME_AT_START`: `X2DungeonGame.h` PASS +11, `X2DungeonGame.cpp` PASS~ +56 (one blank line before the ctor `#ifdef`),
  `X2DungeonSubStage.cpp` OTHER (the DEBUG lines; with `--with _DEBUG`: PASS +99), `Always.h` OTHER.
- RESULT PASS, 0 deleted, 0 non-ASCII, 0 EOL mismatches, 0 untokened `#else` / `#endif`, 0 encoding changes.
- Post-commit `--range`: see STATUS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,578,880 B, dated 2026-10-14 19:47:41, md5 352a3ca53af47a61d5401e37bb487cc3, equal to X2/US_SERVICE/x2.exe (2026-10-14 19:47:36); built at mods/june-buff-port b69fec6 + 5 uncommitted code file(s).` The fifth file, `X2GUUser.cpp`, is a peer's uncommitted C3t-1 edit. The exe holds `JUNE_FX C3w banner` / `JUNE_FX C3w ShowBossName`. **Ship exe and data036 together:** without the new kom every boss name is missing (and logged).

**How to see it in game:** the session hand-off. Behimoss Heart Normal (33250), `/stage 3`: two boss names stacked at once (or one for
Waldomenation), sliding in about 100 px further left than March's single banner. Log tag: `JUNE_FX C3w`.

**Play-test (2026-10-14, reyaa, Karuso Village Hard 33231 `/stage 3`; `data036` copied first, `COPY OK` md5 07b97181): PASSED.**
- `offline_server.log`: `banner 0 npc 1218 tex HQ_BOSS_NAME_KARUSO_FIGHTER_BOSS.dds` and `banner 1 npc 1216 tex HQ_BOSS_NAME_KARUSO_ASSASSIN_BOSS.dds`
  in the same call (19:58:49.806 / .814, sub-stage time 0.01, delay -1), then `banner 0 npc 1219 tex HQ_BOSS_NAME_KARUSO.dds` (Battle Master).
- No `ShowBossName: particle sequence ... not found` line; 0 EXCEPTION; UNHANDLED only the baseline `E_DISABLE_HB_CHECK_REQ`; clean exit.

## C3t - identify, then port: June's user SHOW_HYPER_CUT_IN_TIME

**Status: ready (found by C3r, 2026-10-10). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3t.md`.

- **Scope:** the June-only state key `SHOW_HYPER_CUT_IN_TIME` (`32057b80` @32060505 -> user `+0x9280`, miss -1) and its reader (`32063650 @3206ce0a-3206ce42`, user vslot `0x838` ( time, false ) when not -1). Data: every June class script (hyper actives); none in March, so dormant until the character ports.

### C3t - Done (2026-10-10, Opus 5.5 high)

**Mapped** (funcmap rows `(C3t)`; Ghidra, `vtslot.py`, `xorstrings.tsv`, a raw `+0x951c` displacement scan):
1. **Slot `0x838` = `32014fc0`** on all 12 June user classes, overridden by none. It is a hyper cut-in shown from the script. The kickoff's
   "hyper actives" is wrong: the 17 files set the key in their **awakening** states (`ESSI_*_HYPER_MODE`, Ara's berserk ones too).
   - When the now anim's one-shot timer passes the time (= `EventTimerOneshot`), and `GetShowCutInAndChangeWorldColor` and `GetShowActiveSkillShow`
     are true: `SkillCutIn( name )` (March's one-argument constructor), then `ActiveSkillShow( L"Dummy1_Rhand", ..., bHyper = false, bOnlyLight )`.
   - The name comes from the **unit templet**, not the class script: `m_HyperCutIn`, or `m_BerserkCutIn` when user `+0x951c` is set and it is not
     empty. Then two Lu/Ciel branches (second body, `UC_LUCIEL_DEMONIO` state `0x1c2 / 0x1f6`) pick `m_MinorUnitHyperCutIn` / the `_Trans` names.
2. **The templet keys** (June `AddUnitTemplet_LUA 3237aa40`, last five, miss `L""`). The parser's base is the map node + 0xc (`unitType` at
   `esi+4`), so its `+0x2e0..+0x350` are the reader's `+0x2dc..+0x34c`. March's data036 `UnitTemplet.lua` has none of them.
3. **`+0x951c`** is set in CommonStateStart `@3205e0d0`, right before `BeginTable( "SOUND_SET" )`:
   `( GetHyperModeCount > 2 (unsigned) || vslot 0xa4 ) && vslot 0x51c`.
   - vslot `0xa4` is June-only (base false; Chung's `32c24b90` rolls `+0x9714` against a random float). It is **not** `IsFullHyperMode` (`0x9c`).
   - vslot `0x51c` is "now state `0x17` = `USI_HYPER_MODE`"; Ara adds `0x9b` = `ASI_BERSERK_MODE`, and Luciel overrides it too.
4. **Routed, not ported here:**
   - June's `GetShowActiveSkillShow` (`31fd3180`) is a different body (three option cases, a `31fcef00() == 5` case, a hide override) -> **C3t-1**.
   - June's sound-set pick around the flag (`USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE`, a `SOUND_SET<n>_RATE` loop, a HYPER / BERSERK choice
     that reuses vslots `0x51c / 0xa4`) differs from March's `ARA_CHARACTER_BASE` block -> **C3t-2**.

**Code** (`SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME`; `X2GUUser.h/.cpp`, `X2Unit.h`, `X2UnitManager.cpp`, `Always.h`):
- `m_fShowHyperCutInTime` (ctor -1) and its parse after C3r's `CHANGE_WORLD_COLOR_DATA` table; the reader after March's `SHOW_SKILL_CUT_IN_TIME` call.
- `virtual ShowHyperSkillCutInByScript` (after `ShowActiveSkillCutInAndLightByScript`), with the anim / unit / templet null checks marked as ours.
- `m_bJuneBerserkHyperCutIn` (ctor false) and its set before `SOUND_SET`; base virtuals `JuneIsBerserkByChance` (false) and `JuneIsHyperModeState`.
- Five `UnitTemplet` members and their parse after `introMovieFIleName`.
- The class overrides and both Lu/Ciel branches -> checklist **7t** (no second body, no unit type 10 in March: leaving them out is reach-equal).

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME --worktree --files` (C3t's 5 files): 4 PASS (`X2GUUser.cpp` +56, `X2GUUser.h` +14,
`X2Unit.h` +10, `X2UnitManager.cpp` +9), `Always.h` OTHER (the define plus a peer's uncommitted C3z-6 defines; only C3t's block was staged). Post-commit (`--range 547670a^..HEAD`): `files: 4 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Commit `547670a`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,551,232 B, dated 2026-10-10 23:14:04, md5 ed368352133232b457e071dc84410437, equal to X2/US_SERVICE/x2.exe (2026-10-10 23:13:59); built at mods/june-buff-port 57beb50 + 10 uncommitted code file(s).` The uncommitted files are C3t's and a peer's C3z-6 work. `SHOW_HYPER_CUT_IN_TIME`, `m_BerserkCutIn` and `m_MinorUnitHyperCutIn_Trans` are each in the deployed exe once. No kom.

**How to see it in game:** nothing, by design: no March class script sets `SHOW_HYPER_CUT_IN_TIME` and March's `UnitTemplet.lua` names no cut-in.
Dormancy checked against what ships: a byte scan of 6,392 `.lua` (every `24965799\kom_contents\data*_kom\` file, June bytecode included, plus
loose ones under the game `data\`; no `dev_overlay\`) finds the key 0 times.
Regression surface: every CommonStateStart now runs the flag test and every CommonFrameMove one `IsSamef`, so awakening (`USI_HYPER_MODE`)
should look and sound as before. No play-test owed; C3t writes no log lines.

No questions for the user.

## C3t-1 - identify, then port: June's GetShowActiveSkillShow

**Status: done 2026-10-14, play-test PASSED 2026-10-14 (found by C3t, 2026-10-10). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3t-1.md`, review `C3t-1-review.md`.

- **Scope:** June `CX2GUUser::GetShowActiveSkillShow 31fd3180`, which gates every skill cut-in: three option cases (March has two), a
  `31fcef00() == 5` case, and a false override from two globals. Includes June's option values against March's `CX2GameOption` cut-in setting.

### C3t-1 - Done (2026-10-14, Opus 5.5 high)

**Mapped** (funcmap rows `(C3t-1)`; Ghidra, `dispsweep.py`, `xorstrings.tsv`):
1. **June's option is a bitmask, not March's enum.** `g_pMain +0x274` = `CX2GameOption +0xf4` (OpenScriptFile `324214a0` runs on `g_pMain + 0x180`
   @3259affa): `SKILL_CUTIN_SHOW` is read as a raw int (miss 1), saved with `%d`. June's GFx option page has two checkboxes, `mySkillCutScene` and
   `teamSkillCutScene`, synced by `32ede000` as `+0x274 & 1` / `& 2`. So 1 = mine, 2 = party only, 3 = mine or party, 0 = none. June's Lua
   `SKILL_CUTIN_SHOW_STATE` (0 / 1 / 2) and its `DLG_UI_Option_Etc` radios are March's, unchanged and unused by June's GFx page. June's
   `DefaultGameOptions.lua` sets 3; March loads no such file (left to the asset port).
2. **`31fcef00`** = the now state's skill category: `m_pUnit`, skill tree, `GetNowStateSkillID` (vslot `0x524`) != 0, then
   `GetSkillTemplet( id, 1 )` -> `32070510`. 5 = SKILL_TYPE 13 = `ST_HYPER_ACTIVE_SKILL`. Its two `GetSkillLevel` calls discard their results.
3. **The hide override** (`@31fd3228`) has two June-only sources. `[g_pKTDXApp+0x1c8]` is the GFx UI manager; `31c44710( 0xb )` invokes
   `"isOpenUI"`, i.e. GFx UI `0xb` is open. `g_pMain +0x80a` = "a movie is playing" in June's movie table (`+0x7f8`: `m_wstrMovieFileName`,
   `m_eMovieCategory`, `m_iQuestID`, ...; parser `31d8cb90`, set by `31d7a150` / `31d884f0`, cleared @324dadc2). March has neither: no GFx (D9),
   and its `m_bPlayIntroMovie` is a one-shot server-select flag.

**Code** (`SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION` + `_DEBUG`; `X2GUUser.cpp` `GetShowActiveSkillShow`, `Always.h`): June's body, March's in `#else`.
- March's enum, radios and save stay. The value is mapped by name: `SCSS_ONESELF` -> 1, `SCSS_PARTY_MEMBER` -> 3 (March's meaning),
  `SCSS_HIDE` -> 0. June's 2 (party only) is ported but **not reachable** from March's UI.
- June's three cases, then the hyper rule: a `ST_HYPER_ACTIVE_SKILL` state of my unit or a party member shows its cut-in whatever the option.
  March's `GetSkillTemplet( id )` stands for `( id, 1 )`, whose level gate needs `IsEnableCode( 0x911 / 0x910 )` (no code event offline).
- The two hide terms are left out with a comment; they are routed to the backlog's Scaleform/GFx row (`JUNE_PORT_BACKLOG_ORDER.md`).
- `_DEBUG`: one `JUNE_FX C3t-1` line per call (unit, my, option, June value, now skill, hyper, result).

**Dormant part:** March's `NewSkillTempletVer2.lua` has 521 templets and none of type 13, and no save unit has an `SI_HA_*` skill.
The hyper rule therefore waits for the character ports (checklist 7t, new paragraph). For every value March can reach, flag-on equals March.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION --worktree --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h`:
`files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`. `Always.h` is OTHER because it holds the `_DEBUG` block and the peer's uncommitted
C3w defines; only C3t-1's lines are staged. `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted
line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Commit `5021fe4`. Post-commit
(`--range 5021fe4^..HEAD --with SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION_DEBUG`): `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 1 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The `tok!` is the bare `#endif` of the `_DEBUG`-needs-`SERV_IRUHADEV_OFFLINE` guard in `Always.h` (C3e-5's shape).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,578,880 B, dated 2026-10-14 19:47:41, md5 352a3ca53af47a61d5401e37bb487cc3, equal to X2/US_SERVICE/x2.exe (2026-10-14 19:47:36); built at mods/june-buff-port b69fec6 + 5 uncommitted code file(s).`
That is C3t-1 plus the peer's C3w work. `JUNE_FX C3t-1` is in the deployed exe once. No kom.

**How to see it in game:** cut-ins look as before. The log lines prove the mapping: with option One-self, `option 0 -> june 1`; with Hide,
`option 2 -> june 0`, `show 0`. Every line should read `hyper 0`. The party branch shows only if an AI-party bot counts as a party member.

**Play-test PASSED (2026-10-14, exe 352a3ca5, run 19:55-19:57):** reyaa (uid 12) used Aero Strafe (3014) in a dungeon under each option:
`option 0 -> june 1 ... show 1`, `option 1 -> june 3 ... show 1`, `option 2 -> june 0 ... show 0`, all `hyper 0`, `my 1`. 0 EXCEPTION, baseline
UNHANDLED (2x `E_DISABLE_HB_CHECK_REQ`), clean exit. No bot line appeared (the party branch was not exercised).

No questions for the user.

## C3t-2 - identify, then port: June's state sound-set pick

**Status: ready (found by C3t, 2026-10-10). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3t-2.md`.

- **Scope:** June `32057b80 @3205dc33-@3205e4c5` and on: `USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE` (roll kept in `+0x6dcc`), the
  `SOUND_SET<n>_RATE` loop, and the HYPER / BERSERK sound choice (reuses C3t's `JuneIsHyperModeState` / `JuneIsBerserkByChance`), against March's
  `ARA_CHARACTER_BASE` block in `CX2GUUser::CommonStateStart`.

### C3t-2 - Done (2026-10-14, Opus 5.5 high)

**Mapped** (funcmap rows `(C3t-2)`; Ghidra disassembly of `32057b80`, `xordec2.py`, `vtslot.py`, a raw `+0x6dcc` displacement scan):
1. **The roll** (`@3205db96-@3205df1a`): `+0x6dcc` is a **CX2GameUnit** int (ctor `322a2700 @322a5381` = 0). `USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE`
   false or missing -> vslot `0x1c8( 0 ) % 100 + 1`. Vslot `0x1c8` is `3208e0b0` on every user class: the random table at `+0x80f8` + offset,
   which is March's `GetRandomInt` (March used `rand()`). The roll is stored back either way.
2. **The rate loop** (`@3205df30-@3205e081`): `"SOUND_SET" + n + "_RATE"`, with the rates summed. The first n with `roll < sum` wins, and the first
   missing key ends the loop with n left where it is. There is no fallback to set 0.
   - June data: 1,497 states have rates summing to 100 and no spare table, so a roll of 100 plays no set sound (**JUNE_BUGS 192**).
   - The 95 states that sum below 100 without an "else" set look like June's design, not a bug.
3. **The table** (`@3205e10f-@3205e70a`): when `SOUND_SET<n>` opens, **both** lists are cleared (`+0x85c4` and `+0x85d4`).
   - HYPER = `USE_HYPER_SOUND && ( remain > 0 || vslot 0x51c )`.
   - BERSERK (wins over HYPER) = `USE_BERSERK_SOUND && ( IsFullHyperMode && remain > 0 || ( count > 2 || vslot 0xa4 ) && vslot 0x51c )`.
   - Inside the chosen table: `SOUND_PLAY<n>` through the nation reader `32078fd0`, then `STATE_SOUND_PLAY<n>` into `+0x85d4`.
4. **The NPC's CommonStateStart** (`327ac9d0 @327ae165-@327ae564`) has the same roll in front of March's own 3-rate pick -> routed to **C3t-3**
   (`X2GUNPC.cpp` is outside this row). Its key is not decoded yet, and what is open on the NPC's Lua manager is not settled (see the kickoff).

**Code** (`SERV_IRUHADEV_JUNE_SOUND_SET_PICK` + `_DEBUG`; `X2GUUser.h/.cpp`, `Always.h`; needs `_JUNE_HYPER_CUT_IN_TIME` and `_JUNE_UNIT_SOUND`, `#error`-guarded):
- `m_iJuneSoundSetRandomValue` (ctor 0) is on `CX2GUUser`, following row `322a51bb`'s precedent.
- There are three `#ifdef` islands in the `ARA_CHARACTER_BASE` block, and C3t's berserk-flag lines stay between them at June's `@3205e0d0`. March's block is intact in each `#else`.
- **Debug:** one `JUNE_FX C3t-2` line per state start of my unit that has a rate key or opens a set (capped at 400).
- **Census (what reaches March today; byte scan of `24965799\kom_contents`):**
  - Not in any shipped script: `USE_PREVIOUS_...`, `SOUND_SET3_RATE` and up, `STATE_SOUND_PLAY`.
  - `USE_BERSERK_SOUND`: Ara (16 states, e.g. `ASI_COMBO_ZZZ`) and 8 Shadow Ara / Annona NPC scripts.
  - `USE_HYPER_SOUND`: Add, and Chung (14 states, e.g. `CSI_A_CIP_ARTILLERY_NOVA_CHARGE`, rate 100).
  - June-only keys: checklist **7t**.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_SOUND_SET_PICK --worktree --files` (3 files):
- `X2GUUser.cpp` +134 PASS, `X2GUUser.h` +3 PASS.
- `Always.h` OTHER (its own define block, +28).
- 0 deleted lines and 0 encoding changes. The tok! lines are the three bare `#else` and the `#endif`s after `#error` (house precedent).
- `RESULT: PASS`.
- Post-commit (`--range ae12ab4^..HEAD --with _DEBUG`): `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 6 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Commit `ae12ab4`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,579,392 B, dated 2026-10-14 20:26:04, md5 03a0cae7d02e700dc3e28f67f22207f1, equal to X2/US_SERVICE/x2.exe (2026-10-14 20:26:00); built at mods/june-buff-port 2ecd89c + 3 uncommitted code file(s).` The three are C3t-2's own. `JUNE_FX C3t-2`, `USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE` and `SOUND_SET%d_RATE` are in the deployed exe. No kom.

**How to see it in game:** the hand-off below. Voices should sound as before. The log shows the roll, the set, and the branch (NORMAL / HYPER / BERSERK).
Expected differences from March:
- On a roll of 100, Ara's Z-Z-Z (`50 + 50`) and Chung's Nova charge (`100`) play no voice. March played set 0.
- A state with a sound set now also drops the top-level `STATE_SOUND_PLAY` records. No March script has any.

No questions for the user.

## C3t-3 - identify, then port: June's NPC sound-set roll

**Status: ready (found by C3t-2, 2026-10-14). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3t-3.md`.

- **Scope:** June `327ac9d0 @327ae165-@327ae564`: the NPC's roll (`+0x6dcc`, a 41-byte key read on the Lua manager `+0x2b4`, NPC vslot `0x1c8` =
  `32755a00`) in front of March's NPC 3-rate pick (`X2GUNPC.cpp`, `ARA_CHARACTER_BASE`), under C3t-2's flag. First settle the key and what a
  live Lua read returns in March's `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` CommonStateStart.

## C3u - port: June's summon-NPC socket gate

**Status: ready (found by C3r, 2026-10-10). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3u.md`.

- **Scope:** `CX2GUUser::SumSocketOption 3204f190 @3204fb89-3204fc1d`: March's summon-NPC socket push, plus June's gate: NPC ids `0x1017 / 0x1018` only in `GT_DUNGEON` (game vslot `0x10c` == 2).
---

## C3x - port: June's maps for the June dungeon scripts

**Status: ready (found by the C3s play-tests, 2026-10-10). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3x.md`.

- **Scope:** the adopted June dungeon scripts run on March's maps. `june2017_re/world_census.py` (writes `world_census.tsv`, June bytecode
  ljconv'd before comparing) covers the 374 worlds they use: 235 are identical, **137 were reworked by June** and 4 are missing.
  - The 137: 126 line maps and 61 world scripts, i.e. 162 files, 6 of them only in June. 11 world IDs are renamed to June file names.
    142 dungeon scripts sit on them, and their layouts are wrong.
  - The 4 missing: `WI_ELDER_DARK_FOREST_STAGE_0 / _1 / _BOSS_STAGE` and `WI_ELDER_BENDERS_STAGE_BOSS`. Entering them crashes,
    because CreateWorld returns NULL into StageLoading.
  - Work: add the IDs to Enum.lua; add and rename rows in WorldData.lua; `adopt-june` the map scripts and line maps; stage their assets;
    log a NULL CreateWorld.
  - Henir Space's 5 boss-stage worlds go to atlas / elysion.

### C3x - Done

**2026-10-10, code + data; play-test owed.** Flag `SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG` (log only). Commits `b3039ef` and a follow-up (35 line maps).

- **Users first:** every `.lua` naming a reworked world is June bytecode (146 scripts), apart from 7 registries / map dialogs
  (Enum, EnumString, WorldData, DungeonData, 3 `DLG_Map_*`), so swapping the maps moves no March-only dungeon.
- **Enum.lua** (data036, BOM source): `WI_ELDER_DARK_FOREST_STAGE_0 / _1 / _BOSS_STAGE` = June's 575-577 (free in March).
  **33 collided:** June renamed March's `WI_ELDER_BENDERS_CAVE_STAGE2_1` to `WI_ELDER_BENDERS_STAGE_BOSS`. The user chose June
  parity: the new name is added at 33, the old one is kept as an alias, and the old name has no other user.
  `CTT_RIM_LIGHT_BLUE = 10` was added, the C++ value under `SERV_IRUHADEV_RANOX_RIM_LIGHT` (June's Lua has 15).
  June's `Ranox_Secret_Boss_Stage.lua` passes it; nil read as 0 and did not abort. No C++ names the world IDs (`WI_END` = 752).
- **WorldData.lua** (data040): converted (T0 from its sidecar, new core row `edit: june-effect C3x`), then given June's rows:
  - the 11 renamed files and Valley of Abyss's missing `.lua`;
  - world 33 now points at `Elder_Benders_Cave_Boss_Stage.lua`; March's row is kept as a comment;
  - the 3 Druid Forest rows.

  Kept, because June lacks them: `WI_RANOX_SECRET_EXTRA_STAGE1-3`, which June's `Dungeon_Ranox_Secret_Hell.lua` uses.
  Also kept as-is: the 157 June rows whose IDs March lacks. Row diff: March 591, June 748; 12 changed rows, all taken.
- **Map files:** `adopt-june` staged 170 files into data040: the census's 162 plus the 4 missing worlds' scripts and line maps.
  - 24 new, 146 replaced, 0 refused; 34 sidecars superseded. Run record: `june2017_re/june_adopt_20261010_133232.tsv`.
  - Six were Lanox March-compat edits; June's calls are bound now (`SetTriggerID`, `SetShowObject`, `SetIsForceNotCull`,
    `SetRimLightTextureIndex`, `SetCartoonRender`; the `SERV_IRUHADEV_RANOX_*` flags probe ON).
- **The census missed 35 line maps.** Its bytecode tokenizer trimmed at most 3 characters, and some names glue onto the constant
  before them (`SetBGM.X_LineMap.lua`). A source-text pass over every world's `SetLineMap("...")` in `june_src_fixed` found 31 that
  differ from June and 4 that March lacks (June's `Altera_Battle_Air_Ship_Stage1-4_LineMap.lua`, named by the adopted June scripts).
  All 35 are adopted (`june2017_re/june_adopt_20261010_134556.tsv`). `world_census.py` now tries every suffix and reads
  WorldData.lua as source. Re-run: **378 worlds, 0 differ from June**; 4 march-only (Air Ship s6, Dragon Road 4: June dropped them,
  only old events use them). 8 of the 205 adopted files pack to the same sizes as live, so the copy may not change them.
- **Binding audit:** all 65 methods in the 170 June sources are bound. The only gap in the enum tables was the CTT member above.
  - Geyser stage 0's restored `TAT_ETC_PLAY_EFFECTSET` rows carry `MOTIONNAME_EFFECTSETNAME`, which March does not read.
  - An empty `EFFECT_NAME` is skipped, so the rows are inert, as Lanox kept them (Elysion's plan says the same).
- **Assets:** `npc_mesh_census.py --worlds` is a new mode that also takes `.ogg` / `.wav`.
  - It inflates MSZIP meshes (`xof 0303bzip0032`, `fxdata.xfile_text`); without that, the `.Y` textures are invisible.
  - Result: 363 files (36 `.x`, 295 `.Y`, 32 `.dds`; no sounds), in `june2017_re/world_asset_need.tsv`, all new.
    They are staged into data026 / 028 / 029 / 031 / 032 / 101.
  - **data101 is not packed:** its kom_contents dir lacks 1,071 of the live entries (the known corrupt extract).
    Its 2 files (Velder Bridge Hope 3-1 `River_Wave`, `Wall01`) wait for that dir's repair.
  - BGM: `Music063_Stage.ogg` (Druid Forest 0-1) is missing from `data\music\`; June has it at `14JUN17\data\music\`.
- **Packed:** data026 / 028 / 029 / 031 / 032 / 036 / 040, `PACK OK`.
  - 125 sampled live entries in data026-032 decode equal to their dir files.
  - The data028-032 "changed" entries are recompression only: same raw size and algo, first pack by the tools.
  - data036 warns of 30 newer `.src.lua` sidecars that are not C3x's (present before this phase).
- **C++:** `CX2WorldManager::CreateWorld` logs a world ID with no WorldData row (NULL return), a script that fails, and a world
  that sets no line map. `CX2World::SetLineMap_LUA` logs a line map that fails to load. The crash stays (user: log only).
- Hygiene: `flagcheck --worktree --files` 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatches; the
  2 untokened lines are the inner `#endif SERV_IRUHADEV_OFFLINE`. RESULT: PASS. `ledger --check` and `core --check` PASS.
- `DEPLOY OK: X2_offline.exe is 19,532,288 B, dated 2026-10-10 13:41:38, md5 b9b1fdca0d1be39d48fbc8f4c9f8c44c`
  (both log strings are in the exe).
- **Copied (user OK, 2026-10-10 13:49):** `COPY OK` for data040 (1210 entries, md5 26aa6d05), data036 (192, 7bda0c1c), data026 (1268),
  data028 (993), data029 (1694), data031 (1106) and data032 (1310). `Music063_Stage.ogg` is in `data\music\` (md5 46525597, = June's).
- **Not here:**
  - Henir Space boss-stage worlds: `WI_ADRIAN_BOSSSTAGE` and `WI_SKYCROSSROADS_BOSSSTAGE` go to elysion;
    `WI_ATLASDEVELOPMENT_BOSSATGE` and `WI_TOWER_BOSSATGE` go to atlas.
  - `WI_SANDER_SECRET_BOSS_STAGE` and the 66 June-only dungeons no registry reaches go to the new **C3y** (no owner;
    `DI_SANDER_SECRET_HELL` is June-only).
  - Valley of Abyss's June file is in no March kom; no dungeon uses it (field boss raid).
  - C3s's `--all-npc` census never inflated MSZIP meshes (373 of 812): added to C3s's review kickoff.

### C3x - play-test 1 (2026-10-10 13:54): two gaps -> C3z

The user reports two problems, and both are outside what C3x ported:
- Map layouts still look wrong.
- Elder Druid Forest has no monsters and clears itself.

The causes were measured, not guessed:
- **Druid Forest (30070) is empty.** The offline emulator parsed its stages (10 / 13 / 3 NPC cases) but spawned 1 / 1 / 0, the box
  `NUI_BOX` only. The 6 monster ids are June-only (`NUI_ANGER_FOREST_ENT` / `_FAIRY` / `_KNIGHT`, `NUI_CURSE_WISP`,
  `NUI_FOREST_DARK_MAGICIAN`, `NUI_ELDER_DRUID_FOREST_GATE`, June 1608-1614). March's merged `Enum.lua` and `NPCTemplet.lua` (data037,
  kept merged) lack them, so they read nil. Wally Castle Roof (30110) spawned its 14.
- **Layouts:** C3x staged only the assets March *lacks*. 221 of the 1,062 names that the 205 adopted world scripts use exist in both
  builds with different bytes (Battle Air Ship `s1_ground1.y`: 14 KB March vs 70 KB June). March's old geometry draws over June's line
  maps. List: `june2017_re/world_asset_collide.tsv` (script `world_asset_collide.py`; textures not closed yet).

User, 2026-10-10: give the old dungeons their own phases and update them to what June has. That is **C3z** below.

## C3y - census: June-only dungeons no registry reaches, and the Sander secret boss world

**Moved 2026-10-11 (user):** the open content ports of this census (C3y-1b, 1c, 2, 3, 4, 5, C3z-1b, C3z-4t, C3z-5b) now run as the `june-dungeons` plan
(`JUNE_DUNGEONS_PHASES.md`, `kickoffs/june-dungeons/`, same IDs). Their sections and Done notes below stay here as history; new Done notes go
into `JUNE_DUNGEONS_PHASES.md`.

**Status: done 2026-10-11 (census + plan; no C++, no flag, no build, no pack). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y.md`.

- **Scope:** the dungeon scripts in kom_contents that no March `DungeonData` row reaches, plus June's `DungeonData` ids March lacks
  (C3z-3 routed 251 here), plus `WI_SANDER_SECRET_BOSS_STAGE`. Hand each to its owner; unowned work gets a phase row.

### C3y - Done

**2026-10-11, census and plan only.** No C++, no flag, no build, no kom touched, so flagcheck and deploy do not apply.

- **Tool:** `june2017_re/c3y_unreached_dungeon_census.py`. It runs the registries (both DungeonEnums, both `WORLD_ID` tables, both
  DungeonData, both WorldData) in the KOG luajit through `lua_registry_dump.lua`, scans each script's `WI_` names, and re-derives the
  script set. That set equals `dungeon_npc_parity.tsv`'s `C3y:` exceptions: no mismatch lines. It writes three files:
  - `c3y_unreached_dungeon_census.tsv`: one row per script (ids, worlds, gaps, unresolved NPCs, origin, owner);
  - `c3y_unreached_world_census.tsv`: one row per world they name (both enums, both WorldData rows, files, line maps);
  - `c3y_june_only_dungeon_ids.tsv`: one row per June DungeonData id March does not register.
- **66 -> 110 scripts.** C3x's count predates C3z-3. The re-measure gives 110: **56 registered only by June** and **54 registered by
  nobody**. 109 of them are June's own bytecode already. Of the 251 June-only ids, 81 point at those scripts and 170 at covered
  scripts (169 are Hero variants of existing dungeons). No June dungeon id collides with a March value.
- **Owners** (per id; the TSVs carry the per-row owner):

  | Owner | What | Ids |
  |---|---|---|
  | atlas A8 / A9 / A10 | AtlasDevelopment / AtlasCity / Tower N-H-E | 9 (matches `ATLAS_PORT_PHASES.md:314`) |
  | elysion E8-E11 | SkyCrossroads / Adrian / Soless / Memory N-H-E | 12 |
  | backlog Stage 19 | event dungeons (list in that stage) | 13 |
  | backlog Stage 8 | `Dungeon_Tutorial_{Luciel,Rose,Ain}` | 3 |
  | **C3y-1a** | Hamel secret (`UNUSED:DI_HAMEL_SECRET_COMMON` 33260, `DI_HAMEL_SECRET_HELL` 33261), `UNUSED:DI_RANOX_SECRET_COMMON` 41270 | 3 |
  | **C3y-1b / 1c** | Sander secret: its 9 worlds (1b), the dungeon `DI_SANDER_SECRET_HELL` 41141 + `UNUSED:..._COMMON` 41140 (1c) | 2 |
  | **C3y-2** | June's class tutorial dungeons, 9 March classes (41150-41290) | 9 |
  | **C3y-3** | Bestera 1-4 + `DI_BESTERA_START` (150000-150040) | 5 |
  | **C3y-4** | Space of Thought 1-3 + `_DUMMY` (41480-41510) | 4 |
  | **C3y-5** | the Hero tier: 159 `*_HERO_NORMAL/HARD/EXPERT`, 30 `*_HERO_HELL`, `DI_HERO_DUMMY_COMMON/HELL` | 191 |

- **54 scripts are dead in both builds** (neither DungeonData names them; a grep of March's kom_contents, `june_src_fixed` and
  X2Lib's `.cpp` finds no other referrer): old El Forest / Besma Mine / Dragon Canyon / Nature Cave / Elder Halloween sets, Henir
  2011 / 2013 / 2016, Ruben secret, the Velder / Hamel battle ships, 3 training scripts, `En_Time1-3`, 7 datai030 events. No row.
- **Hero is not "unreachable in June".** The Atlas / Elysion "Out of scope" lines say so, but the source they cite
  (`JUNE_UI_PARITY_PHASES.md` "Phase 4 findings") proves only that the *Mode Button* is unreachable. It also says June's real route
  into Hero content "still needs its own investigation", and the user plans to port Hero later ("User decisions" item 3). So it gets
  C3y-5 (identify first), per the unowned-branch rule.
- **Henir Space already names the 5 missing boss worlds.** The live, covered `Dungeon_Henir_Space.lua` (June bytecode, Special map)
  puts stage 22 on `WI_SANDER_SECRET_BOSS_STAGE` and stages 32-35 on atlas / elysion's 4. Each stage's `NEXT_STAGE` names its own
  index, so the run order is decided elsewhere; whether a run reaches them is not proven. That is why Sander's worlds are split out
  as C3y-1b, ahead of the dungeon.
- **WorldData must be keyed by `worldID`.** June names `WI_EVENT_SCHOOL_EVENT_STAGE0`'s row "WI_TUTORIAL_EL_STAGE0" and the Eve
  tutorial's "WI_TUTORIAL_EVE_STAGE0 " (trailing space). `world_census.py` (C3x) keys by name. Re-checked by id: its only other
  miskey, `WI_VELDER_SECRET_BOSS_STAGE`, has a world script and line map byte-equal to June's, so C3x missed nothing.
- **One world-id clash, harmless:** June's `WI_PVE_BATTLE_WORLD_ISLAND_OF_ERDA` = 573 = March's Lua `WI_END`, named only by the dead
  `Dungeon_Pve_Battle_World_Island_of_Erda.lua`. C++ `WI_END` (752) bounds village / field worlds only (`X2State.cpp:6797` and three
  more), not dungeon stages, so Space of Thought's 771-773 are not blocked by it.
- **Running together:** C3y-1a and C3y-1b share no file; the identify halves of C3y-2 / 3 / 5 run with anything; the port halves
  of C3y-1b / 1c / 2 / 3 / 4 edit the same core registries (data036 Enum / DungeonEnum, data035 DungeonData, data040 WorldData,
  data037 NPCTemplet) and run one at a time.
- **Routing notes added:** STATUS rows C3y-1a..C3y-5 + kickoffs; backlog Stage 19's "still to identify" now lists the 13 event ids;
  the atlas / elysion blocks point at the census.

**Decided (user, 2026-10-11): the 4 atlas / elysion Henir boss worlds are ported in june-effect** (`WI_ATLASDEVELOPMENT_BOSSATGE`,
`WI_TOWER_BOSSATGE`, `WI_SKYCROSSROADS_BOSSSTAGE`, `WI_ADRIAN_BOSSSTAGE`), world-only, in C3y-1b next to Sander's, so no Henir Space
stage names a world March lacks. Atlas A8 / A10 and Elysion E8 / E9 reuse them; their dungeons stay with atlas / elysion.

## C3y-1a - port: Hamel secret dungeon rows + the Ranox secret common row

**Status: done 2026-10-11, play-test owed (see "C3y-1a - Done"). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y-1a.md`.

- **Scope:** March already ships `Dungeon_Hamel_Secret_{Common,Hell}.lua` (June bytecode), the 9 Hamel secret worlds (561-569) and
  `DI_HAMEL_SECRET_HELL` 33261 in DungeonEnum, with 0 unresolved NPCs. Missing:
  - June's 2 DungeonData rows (data035 `DungeonData.lua`, core) and `UNUSED:DI_RANOX_SECRET_COMMON` 41270's row;
  - the map button: June's `DLG_Map_Local_Hamel_Icon_NEW.lua` (data217 in June) has `HamelSecret`; March's data034 copy omitted it in
    UI parity Phase 3. Default: adopt June's file (June asset wins); editing March's sidecar'd copy needs the user's OK first;
  - UI assets: `HQ_LoadingDungeon_HARMEL_SECRET1/2.tga`, `DLG_UI_Dungeon_Image09.dds`, button texture keys; entry item 700015 in item.kim.
- Check `m_eDefaultDungeonLoungeWorldID = WI_HAMEL_SECRET_STAGE_BOSS` (no such June enum name; June's world is `..._BOSS_STAGE`):
  keep June's value verbatim (JUNE_BUGS row) unless the strict load test fails.

### C3y-1a - Done

**2026-10-11, Opus 5.5 high. Data only: no C++, no flag, no build.** flagcheck and deploy do not apply.

- **DungeonData** (data035, core BOM source; `luasrc.py refresh`; ledger and core checks PASS):
  - June's 3 June-only rows are added verbatim from `june_src_fixed`, between `--{{ ... June effect C3y-1a ... June's row` / `--}}` markers, in June's order:
    - 33260 `UNUSED_DUNGEON_ID.DI_HAMEL_SECRET_COMMON` and 33261 `DI_HAMEL_SECRET_HELL`, after Behimoss Heart Expert;
    - 41270 `UNUSED_DUNGEON_ID.DI_RANOX_SECRET_COMMON`, before Ranox Secret Hell.
  - The Ranox port Phase 9 note ("not ported, UNUSED_DUNGEON_ID does not exist") is superseded, since C3z-3 added that table. It says so in the marker.
- **Gate:**
  - `dungeon_registry_census.py`: 220 shared ids, **220 SAME** (217 + these 3). STR_ID missing: none.
  - Strict test as written: `FAIL run (stopped after row 229): missing enum member WORLD_ID.WI_HAMEL_SECRET_STAGE_BOSS`.
    - That is the test's own rule (it puts an erroring `__index` on every numeric table), not the client's.
    - `WORLD_ID` is a plain table in `Enum.lua:1730`, and nothing in X2Lib or KTDXLIB sets a metatable on it, so the key reads nil.
    - June ships the same name against an Enum that also lacks it.
    - The test now takes `allow=TABLE.MEMBER` for one named, kept June name, and prints `ALLOW` when that name is read.
    - With `allow=WORLD_ID.WI_HAMEL_SECRET_STAGE_BOSS`: **`OK 251 rows, 10 titles, 0 fail(s)`**; it was 248 before.
- **The lounge-world name is kept verbatim (JUNE_BUGS row 170).**
  - The lounge world becomes `WI_NONE`, as it already is for 100 of the 251 rows.
  - Its only reader, `X2Main.cpp:5529`, runs only when the return village is a dungeon lounge.
- **Map button: the user chose to adopt June's file (2026-10-11).**
  - March's `DLG_Map_Local_Hamel_Icon_NEW.src.lua` was June's file minus `HamelSecret`, `SecretDungeonDesc` and `TITLE_SECRET`, and nothing else.
  - `adopt-june` put June's ljconv bytecode (11,641 B) in data034 and moved the sidecar to `kom_backup_stage_june\20261011_103645`.
  - Record: `june2017_re/june_adopt_20261011_103645.tsv`.
  - This reverses the 2026-10-09 core-Lua rollback for this one file only.
  - No C++ is needed: `UpdateDungeonButtons` / `SecretDungeonDesc` (`X2LocalMapUI.cpp:1375`, `:1418`) are keyed by the button's dungeon id and the control name `SecretDungeonDesc`, not by village.
- **Assets** (`stage-june`, byte-exact June; backup `kom_backup_stage_june\20261011_103536`):
  - `DLG_Localmap_Title_01.tga` / `.TET` (June data151, staged into data063, March's only copy, 77 keys) replaces Lanox Phase 2's merge.
    - With March's art, `TITLE_HARMEL_SECRET` showed "Sander's Ordeal"; June's art reads "Temple of Trials".
    - Forecast: all 58 keys March's source scripts read are in June's TET. The bytecode-only readers were checked too:
      the Hamel file is now June's, and `DLG_UI_Local_Peita_Icon_NEW.lua` reads only `TITLE_PEITA_01-06` and `TITLE_HENIR`.
    - `Title_Holloween` is blank in June's art. June's own 7 village `Icon_NEW` files read it the same way, so that is June parity.
    - `TITLE_ERDA_ISLAND` has no reader. `TITLE_SANDER_SECRET` is now there for C3y-1c.
  - `DLG_UI_Dungeon_Image09.dds` (data065): June's copy; same TET.
  - Already equal to June, so not staged: `HQ_LoadingDungeon_HARMEL_SECRET1/2.tga`, the `DLG_Common_Button03/05_NEW` atlases (keys `HARMEL_SECRET_DUNGEON_NORMAL/_OVER/_LOCK`).
- **Emulator:** nothing keys on a fixed secret-id list.
  - The secret paths test `DT_SECRET` / `DM_SECRET_HELL` (`Handlers_Room.cpp:1952`, `:2026`, `:2232`).
  - The only id list is Altera's extra-stage count (`:2124`).
- **Items:** 113277 (Luto's Secret Key), 700015 (Secret Dungeon Entry Permit) and the 9 `BOSS_DROP_ITEM` ids are all in the live item.kim.
- **Packs and copies:**
  - `PACK OK` data034 (~1), data035 (~1), data063 (~2), data065 (~1).
  - `COPY OK data034 ... md5 60c59a7764eb8754433e550ed3c9ab8e`
  - `COPY OK data035 ... md5 c719492b6e65d2fbb7b3ec05c2f96fcb`
  - `COPY OK data063 ... md5 0ded68660adc53eaa02114f6bb791e71`
  - `COPY OK data065 ... md5 c1d561445849e652421be6f4ccf41c45`
- **Save:** Freils (unit 18) was given 2x 700015. Backup: `db_backup\els_db_20261011_103730_pre_additem.sql`.
- **Play-test owed** (Freils):
  - the Hamel map shows the secret button and title;
  - a Hamel Secret Hell room starts and stage 0 loads;
  - Ranox Secret Hell still resolves. It is registered after both insertions.
  - `auth_level` 3 shows every button enabled, so this proves the button and the room, not the lock logic.
- **Log lines read** (nothing writes `JUNE_FX C3y-1a`; this phase is data only):
  - `[RANOXDBG] ... dungeonID=41271 pDungeonData=OK` in ErrorLog;
  - the room-start line for 33261 and `using its own level band` in `offline_server.log`;
  - 0 EXCEPTION, and no `XEM_ERROR2` / `can't find` / `attempt to index` line.
- **Play-test 1 PASSED (2026-10-11, run 16:11, exe `1ca6415` deploy, koms of this phase live; logs read).**
  - reyaa (Lire Grand Archer, unit 12, level 80) played it, not Freils.
  - Ranox Secret Hell: ErrorLog shows `[RANOXDBG] control=Ranox_Secret dungeonID=41271 pDungeonData=OK`, and room 1002 (41271) started. So the file loads past both insertions.
  - Hamel Secret Hell: room 1001 started (`dungeonID=33261 ... mode=4`).
    - Stage 0 loaded 21 NPCs over 2 sub-stages; the emulator used the 70-99 band, base 85.
    - Kills paid EXP / ED. The room was left after stage 0.
  - 0 EXCEPTION. 2 UNHANDLED, both `E_DISABLE_HB_CHECK_REQ`, which is not this phase's.
  - No `XEM_ERROR2`, `can't find` or `attempt to index` line.
  - The Henir run (38100, stages 22 / 32-35) belongs to C3y-1b.
  - **Gap found:** 12x `Temple of Trials_LocalQuest` control not found.
    - Cause: data034 `DLG_Map_Local_Hamel.lua` (March merged copy) left `DI_HAMEL_SECRET_HELL` out of `DungeonIDList`, so the map never built that dungeon's quest-mark static.
    - The lookup (`UdateLocalMapQuestDesc`, `X2LocalMapUI.cpp:2073`) is null-checked, so it was harmless.
    - June's file differs only by that entry. **The user chose to adopt it** (2026-10-11).
    - `adopt-june`, sidecar to `kom_backup_stage_june\20261011_162133`, record `june_adopt_20261011_162133.tsv`, `PACK OK data034` (~1), `COPY OK data034 ... md5 558e9f4d5e7825cccdc118be0762134b`.
    - Copy and play-test 2 are owed: the Hamel map with no `_LocalQuest` line.
- **Play-test 2 PASSED (2026-10-11, run 16:23, after the 16:21 data034 copy; logs read).**
  - No `Temple of Trials_LocalQuest` line, and the Hamel map works.
  - reyaa ran Hamel Secret Hell 33261 through stages 0, 2, 1, 3 and 4, then left (no clear attempted).
  - 0 EXCEPTION. 3 UNHANDLED (`E_DISABLE_HB_CHECK_REQ` x2, `EGS_REPORT_HACK_USER_NOT`), none this phase's.
  - Seen, not this phase's: 1x `[EXTRA_NPC_STAT] non-finite result (statType=1 level=1 dungeon=33261)`, the open NaN hazard of `LANOX_EXTRA_NPC_STAT_PLAN.md` Phase 2. It fell back to the legacy stats.
  - **Phase closed.**

## C3y-1b - port: Henir Space's 5 missing boss worlds + the other Sander secret worlds

**Status: done 2026-10-11, play-test PASSED (2nd run), review owed (routed by C3y; the 4 atlas / elysion boss worlds added by the user the same day). Model:** Opus 5.5,
high. Kickoff `kickoffs/june-effect/C3y-1b.md`.

- **Scope:** 13 worlds, world-only (no dungeon rows, no NPCs). All June ids are free in March.
  - Sander: `WI_SANDER_SECRET_STAGE0-3 / BOSS_STAGE / EXTRA_STAGE1-3 / EXTRA_BOSS_STAGE` = June 614-622 (Henir Space stage 22 names
    the boss one).
  - Henir Space stages 32-35: `WI_ATLASDEVELOPMENT_BOSSATGE` 645, `WI_TOWER_BOSSATGE` 654, `WI_SKYCROSSROADS_BOSSSTAGE` 675, `WI_ADRIAN_BOSSSTAGE` 710
    (`Elysion_<name>_BossStage.lua` + `_LineMap.lua`; the dungeons that also use them stay with atlas A8 / A10 and elysion E8 / E9).
  - data036 `Enum.lua` (core) + data040 `WorldData.lua` (core) rows; 13 world scripts + 13 line maps by `adopt-june`.
  - Assets (world-script level; MSZIP meshes not inflated yet, so a lower bound): Sander 130 named / **112 missing** (73 `.Y`, 33 `.x`,
    6 `.ogg`); the 4 boss worlds 102 named / **93 missing** (77 `.Y`, 7 `.x`, 9 `.ogg`). Run `npc_mesh_census.py --worlds` after staging,
    as C3x did, and check overlap with june-asset-port before staging.
  - **Binding audit:** the 4 Elysion world scripts may call Elysion-half methods March lacks (elysion E5 moving platforms, E7
    "additional world"). An unknown binding aborts the rest of the chunk. Port a missing binding here if it is small and world-only;
    otherwise stop and report which call needs which E-phase.
- Gate: the C3x CreateWorld log shows no missing row for the 13; Henir Space stages 22 and 32-35 load if a run reaches them.

### C3y-1b - Done

**2026-10-11, Opus 5.5 high. Data + one binding.** Flag `SERV_IRUHADEV_JUNE_WORLD_ANIM_WAIT`. Review: `kickoffs/june-effect/C3y-1b-review.md`.

- **Enum.lua** (data036, core source): the 13 `WORLD_ID` rows at June's values (614-622, 645, 654, 675, 710), in a `--{{ ... C3y-1b` block
  at the end of `WORLD_ID`. Each value and name was re-checked as free in March before writing.
  **WorldData.lua** (data040, core source): June's 13 rows verbatim, picked by `worldID` and appended in a block.
  `luasrc.py refresh` both; `ledger --check` and `core --check` PASS.
- **Map files:** `adopt-june data040` staged the 13 world scripts and their 13 line maps (each script's `SetLineMap` names exactly its own),
  all 26 new, as June bytecode. Record: `june2017_re/june_adopt_20261011_103439.tsv`.
- **Binding audit** (per class, with each binding's `#ifdef` resolved in `Always.h`): 55 receiver:method pairs and 27 enum members.
  Every one is bound and defined except **`SetAnimWait`**, called once in all of June's Lua: `Sander_Secret_Stage3.lua:320`, on its
  last skin mesh (X07, `WOSMPY_ONE_WAIT`), with only `SetTriggerID(0)` after it.
  - June: binder `31d95080` name use `31dbffdd` -> `327f6990` = `if ( m_pXSkinAnim ) m_pXSkinAnim->Wait()`. `31d28d30` sets +0x2a4 = 2.
    Its siblings `31d28d00` (1 + play type) and `31d28d40` (0) are March's `Play` / `Wait` / `Stop` (`KTDGXSkinAnim.cpp:1077-1092`).
    Ported as an inline member next to `SetTriggerID` plus a `class_def` in `X2Main.cpp`. funcmap row `327f6990`.
  - Purpose: X07 holds still until line-map trigger 0 (`TAT_MESH_CHANGE_ANIM`, mesh named `..._X07.x`) plays it. `GetTriggerMesh` compares
    names upper-cased (`X2World.cpp`), so the case difference is harmless.
  - The 4 Elysion boss-stage scripts need nothing from elysion E5 / E7.
- **Assets:** `npc_mesh_census.py --worlds` (MSZIP inflated) found 408 June files: 154 `.y`, 40 `.x`, 196 `.dds`, 3 `.tga`, 15 `.ogg`.
  The plan's 205 was a lower bound. List: `june2017_re/c3y1b_world_asset_need.tsv` (the script's default `world_asset_need.tsv` is C3x's
  and was restored). A re-run after staging needs 0. No overlap: june-asset-port has not started (P0), and C3y-1a used
  data034 / 035 / 063 / 065. `stage-june` put all 408 in as new: data135 151, data136 6, data146 6, data180 236, **data166 1**,
  **data182 8**. The last two are new archives inside the mounted range (`X2Main.cpp:799-845`).
- **BGM:** the worlds name 6 tracks missing from `data\music\`: `Music072_Stage`, `Music073_Boss`, `Music075_Boss`, `Music079_Boss`,
  `LI_Boss_Elysion001`, `LI_Boss_Elysion002` (all in June's `14JUN17\data\music\`).
  june-asset-port P9 (`music/`): once copied, these 6 are already present, so do not count them as missing.
- **Packed:** `PACK OK` data036 (~1 Enum.lua), data040 (+26, ~1 WorldData.lua), data135 (+151), data136 (+6), data146, data166 (new),
  data180 (+236), data182 (new).
  - data146 also shows ~28 `FIELD_BOSS_IRON_ELTRION_*.ogg`. That is recompression only: the raw size and algo equal live, the dir files
    are the 2026-09-22 extract, and it is this tool's first pack of data146.
- **Gate:** `world_census.py`: 391 worlds, 0 differ from June; all 13 `same` (26 rows added to `world_census.tsv`).
- Hygiene: `flagcheck --worktree --files`: 2 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatches. RESULT: PASS.
  After commit `9c7bd65`, `--range 9c7bd65^..9c7bd65`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the token, 0 encoding change(s)`; `RESULT: PASS`.
  The exe was built at `9a8c6c0` plus these 3 files; `9a8c6c0` and `9ef8b63` (C3y-1a) are data / docs only, so the exe matches `9c7bd65`.
- `DEPLOY OK: X2_offline.exe is 19,555,328 B, dated 2026-10-11 10:48:05, md5 64fa2a98ba49ba5c6fc3645b1af303d2` (`SetAnimWait` is in the exe).
- **Copied (user OK, 2026-10-11 ~10:55):** `COPY OK` data036 (md5 aaa392f1), data040 (ef1358bf), data135 (70afdc24), data136 (b147083c),
  data146 (b6ddb3f4), data166 (new, 4582d4d8), data180 (49d1cf63), data182 (new, 0539c736). The 6 BGM files are in `data\music\`, md5 = June's.
- **Play-test 1 (2026-10-11 10:52-10:55, reyaa): crash at `/stage 22`.** The world loaded (State.log `Stage Start : 22`, no
  `JUNE_FX C3x` line). CrashRpt `X2_offline.xml`: AV at `0x32e6cd0a` = `CX2GUNPC::OnFrameRender_Prepare` +0x76a (X2.map), the
  `*pSubRenderParam = *pRenderParam` copy at `X2GUNPC.cpp:25269`. The attach skin mesh is NULL because `MAD_BEHIMOSS_SOUL_Skin.x`
  (`SECRET_MAD_BEHEMOTH_SOUL_HENIR.lua` `ATTACH_SKIN_MESH0`) is in neither build. June skips the block when `OpenXSkinMesh` fails
  (`327a901e`, funcmap row). The Ranox port's `ATTACH_SKIN_MESH0` kept a SubEquip with no anim. **Fix:** flag
  `SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_SKIP_MISSING` drops it and logs `JUNE_FX C3y-1b attach skin mesh ... not found` (insert-only,
  inside the Ranox block). A census of June's 3 attach-skin-mesh names finds only this one missing, used by the 3 Soul scripts. JUNE_BUGS row 171.
  flagcheck `--worktree --files`: 1 PASS, 1 DEFINE, 0 FAIL, 0 deleted. RESULT: PASS (also `--range d015581^..d015581`).
  `DEPLOY OK: X2_offline.exe is 19,570,176 B, dated 2026-10-11 15:45:42, md5 a3cf19b767317fc5cd4460e33b4e7277` (built at `1ca6415`;
  the shared tree first had to wait for C3p / C3z-4b to compile).
- **Play-test 2 PASSED (2026-10-11 16:11-16:16, reyaa, exe a3cf19b7):** State.log `Stage Start` 22 (16:13:58), 32, 33, 34, 35, each with
  `Sub Stage Start : 0`; no crash (the only `%TEMP%\X2_offline.xml` is play-test 1's). `offline_server.log` `[16:13:59.116] JUNE_FX C3y-1b
  attach skin mesh MAD_BEHIMOSS_SOUL_Skin.x not found: skipped, as June does`. No `JUNE_FX C3x` world / line-map line and no
  `lua error while loading` in ErrorLog. **Routed to C3y-1c (the Behemoth soul NPC, not the worlds):** ErrorLog `OpenTexture` fails
  `BEHEMOTHSOUL_HAND_TRACE01.DDS` / `BEHEMOTHSOUL_HAND.TGA` (June data002, absent in March), and 6 `[RANOX_EFFECT_MISSING]
  EffectSet_BEHEMOTHSOUL_*` names (June's `EffectSet.src.lua` has 18 `EffectSet_BEHEMOTHSOUL` lines, March's 0).
- **Not provable here:** only `Dungeon_Sander_Secret_Hell` (C3y-1c) reaches Sander's 8 non-boss worlds, so `SetAnimWait` and those
  8 worlds wait for C3y-1c's play-test. Henir Space (38100) reaches stages 22 and 32-35, but its run order is decided elsewhere,
  so the check uses `/stage`.

## C3y-1c - port: the Sander secret dungeon

**Status: queued (needs C3y-1b; C3y-1a sets the pattern). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y-1c.md`.

- **Scope:** `DI_SANDER_SECRET_HELL` 41141 into data036 `DungeonEnum.lua` (core; 41140 `_COMMON` is already in March's
  `UNUSED_DUNGEON_ID`); June's 2 DungeonData rows (data035, core); 32 NPC ids into `Enum.lua` + `NPCTemplet.lua` (data037), as C3z-2:
  all 32 have June templets, and their unit scripts are already staged; meshes (`npc_mesh_census.py`); stat tables.
  DropTable is server-only, so they give 0 EXP / ED (C3z-2's open question applies).
- Map button: June's `DLG_Map_Local_Sander_Icon_NEW.lua` has `SanderSecret` + `_Desc` + `TitleBG`, which March's data034 copy omits
  (UI parity Phase 3). Default: adopt June's file, as C3y-1a. Texture keys `SANDER_SECRET_DUNGEON_NORMAL/_OVER` (`DLG_Common_Button04_NEW.tga`), `TITLE_SANDER_SECRET`
  (`DLG_Localmap_Title_01`, June data151), `HQ_LoadingDungeon_Sander_SECRET1/2.tga`, `DLG_UI_Dungeon_Image10.dds`; entry items
  700015 / 113277 in item.kim.
- The emulator: C3z-6's secret-dungeon path (relative level, `DT_SECRET`); check nothing keys on a fixed list of secret ids.

## C3y-2 - identify, then port: June's class tutorial dungeons

**Status: ready (routed by C3y, 2026-10-11). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y-2.md`.

- **Scope:** `Dungeon_Tutorial_{Elsword,Aisha,Lena,Raven,Eve,Chung,Ara,EL,Add}.lua` (41150-41290; Luciel / Rose / Ain are Stage 8).
  One world each (624-632, all June-only, free in March), 5-6 `NUI_TUTORIAL_*` NPCs each, `m_NPCLevel` 1, `requireSpirit` 1.
- **Identify first:** who sends a player into `DI_DUNGEON_TUTORIAL_<class>` in June (character creation, a server packet, the June-only
  `CX2TutorialGuideManager`), and how it meets March's own tutorial auto-connect (fixed in MARCH_2014_MIGRATION Phase 3B) and
  june-buff 14c's `SERV_IRUHADEV_JUNE_TUTORIAL_MODE` (`c14c_tutorial_reads.tsv`). Stop and report the route before porting if it
  replaces March's new-character flow.

## C3y-3 - identify, then port: Bestera 1-4

**Status: ready (routed by C3y, 2026-10-11). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y-3.md`.

- **Scope:** `Dungeon_Bestera1-4.lua`; `DI_BESTERA_START` 150000 (the Besma map button, row on `Dungeon_Bestera1.lua`) and
  `DI_BESTERA1-4` 150010-150040. Levels 35-70, `m_NPCLevel` 1, entry item 78988, `Min/MaxAdequateMemberCount` 3 / 4 (C3z-5's keys).
  8 worlds (736-743, free), 25-29 June-only `NUI_BES_*` / `NUI_TERA_*` NPCs per script.
- **Identify first:** June's `IsActiveDungeon` (`327da190`) has Bestera handling (UI parity "Known follow-ups"); the Besma icon's
  Bestera control was omitted in UI parity Phase 3.

## C3y-4 - port: Space of Thought 1-3

**Status: ready (routed by C3y, 2026-10-11). Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3y-4.md`.

- **Scope:** `Dungeon_Space_of_Thought_1-3.lua`, `DI_SPACE_OF_THOUGHT_1-3` 41480-41500 and `_DUMMY` 41510 (the Special-map id;
  its line in data217 `DLG_Map_Local_Special.lua`, core, is commented out by UI parity Phase 3). Worlds 771-773 (free).
  30 June-only `NUI_PVP_HERO_*` NPCs (the AI party uses `NUI_CSM_PVP_HERO_*`, a different set). Levels 40-99, `m_NPCLevel` 40.
- `m_iLimitPlayCount` 3 on the `_DUMMY` row is a server-fed play limit: design it as June does with no server (design-missing rule).

## C3y-5 - identify: June's route into Hero dungeons (read-only)

**Status: ready (routed by C3y, 2026-10-11). Model:** Opus 5.5, xhigh. Kickoff `kickoffs/june-effect/C3y-5.md`.

- **Scope:** 191 June-only ids: 159 `DI_*_HERO_NORMAL/HARD/EXPERT` (100000-100522), 30 `DI_*_HERO_HELL` (110xxx),
  `DI_HERO_DUMMY_COMMON/HELL` (120000 / 120010). 169 sit on scripts March already covers, 22 on atlas / elysion's.
- The Mode Button is not the route (UI parity Phase 4 findings: no callers). Find the real one in Ghidra (`DL_HERO`, the hero flag
  readers, `IsActiveDungeon` `327da190`'s hero branch, the third icon dummy), then write the port phases. No code in this phase.

## C3z - old dungeons on June's data (user, 2026-10-10)

**Status: planned. Model:** Opus 5.5, high. The dungeon scripts and maps are June's since C3x; the data they stand on is still March's.
June wins at the same filename and in behaviour (standing rules), so each phase replaces March's copy with June's and merges only
where March's C++ forces it (the registries are kept merged). **Every dungeon** (user, 2026-10-10: "make sure to check all dungeon"): each phase's gate is a per-dungeon census TSV, not the sampled play-test.

- **C3z-1 - June's map assets at the same filename (layout).** Kickoff `kickoffs/june-effect/C3z-1.md`.
  - Replace the 221 colliding meshes and the textures they name, closed over inflated meshes, June bytes wherever June's differ.
  - Re-run the collision census to 0, then pack and copy. Touches the asset koms only.
- **C3z-2 - June's NPCs for the old dungeons.** Kickoff `C3z-2.md`.
  - Census every `NUI_*` that the dungeon scripts name against March's `Enum.lua` `NPC_UNIT_ID` and `NPCTemplet.lua`.
  - Add June's ids and templet rows; check their unit scripts, meshes and any C++ bound.
  - Gate: Druid Forest spawns its monsters. Touches data036 `Enum.lua` and data037 `NPCTemplet.lua`.
- **C3z-3 - June's dungeon registry rows for the old dungeons.** Kickoff `C3z-3.md`.
  - `DungeonData.lua` (data035, kept merged) and `DungeonEnum.lua`: take June's rows (stage lists, limits, levels, names) for every
    dungeon March and June share; record each March-only consumer that blocks a row.
  - Census first, then the edit. Not at the same time as C3z-2 (both pack data036).
- **C3z-4 - Lua methods adopted June scripts call and March never binds** (found by C3z-1). Kickoff `C3z-4.md`; batch 2 / 3
  `C3z-4b.md`; the June systems behind them: C3z-4f (the summoner packet field), C3z-4t (tutorial guide), C3z-4s (SimpleDLG).
- **C3z-7 - skipping a boss intro cinematic** (user, 2026-10-10). Kickoff `C3z-7.md`: prove June's path, then port it.
- **C3z-5 - June's DungeonData fields March's client loader does not read** (found by C3z-3). Kickoff `C3z-5.md`.
  - June's `AddDungeonData_LUA` (`327e0f80`) reads keys March's lacks: `Min/MaxAdequateMemberCount`, `FixMaximumDifficulty`,
    `DUNGEON_CHECK_CONDITION`, `m_SuitableLevelFixValue`, `m_HardMin/MaxLevel`, `m_HardRequireItemLevel`, `m_fExtraValue`, plus
    (June-only rows today) `additionalTicket`, `m_bShowNpcKillCount`, `m_bNoDarkBackGroundBy60S`, `m_iRequireCompleteQuestID`,
    `m_bAprilFoolsDungeon`. Port each reader and its consumers, or record June's no-system answer.
- **C3z-6 - the emulator's server-side DungeonData semantics** (found by C3z-3). Kickoff `C3z-6.md`.
  - Relative-level monsters: the emulator bases them on `m_MaxLevel` (`Handlers_Room.cpp:1827`); the server clamps the party's
    average level into [`m_MinLevel`, `m_MaxLevel`], +5 in secret-hell mode (`CenterServer/DungeonRoom.cpp:3656-3710`).
    June's Secret Hell band 70-99 now spawns level-99+ monsters.
  - Server-only keys no client reads: `m_bSwitchingWithEventInfo`, `m_bForbidEnterDungeon`, `requireLevel`, the poison limits,
    `m_bArcadeMode`; and the server half of `DUNGEON_CHECK_CONDITION`.

### C3z-1 - Done (2026-10-10, Opus 5.5 high; data only, no C++, no flag, no build; 11 koms + 17 music copied)

**Gate tool:** `june2017_re/dungeon_map_parity.py`.
- Scope: every dungeon script in kom_contents (354), its worlds (378), their world scripts and line maps (730), and every asset those
  name, closed over June's meshes (MSZIP inflated). 10,342 assets after the port.
- Writes `dungeon_map_parity.tsv` (the gate, 1 row per dungeon), `dungeon_map_asset.tsv` (1 row per asset) and
  `dungeon_map_replace.tsv` (still to stage: **0**).

**Tokenizer fixed in all 3 censuses** (also `world_asset_collide.py` and `npc_mesh_census.py`). Bytecode constants sit back to back,
so a greedy match swallowed `A.Y-B.Y-C.Y` and resolved only C: 539 direct names were invisible to C3x. A trim is now taken only where
the byte before it is LuaJIT's length (`len + 5`); a blind suffix had turned `Peita_Heart_Floor.tga` (in neither build) into June's
`floor.tga`.

**What changed** (staging and removal both backed up under `kom_backup_stage_june\`):
- **Staged (`20261010_142454`): 404 June files**, 365 replaced and 39 new, 0 refused. Record: `c3z1_stage_plan.tsv`.
  - By kom: 023: 1, 024: 75, 025: 35, 026: 57, 027: 21, 028: 85, 029: 53, 030: 17, 031: 17, 032: 18, 110: 25.
- **Removed (`20261010_142520_c3z1_drop`; user OK 2026-10-10): 306 meshes** from data028 (142) and data029 (164).
  - These are pieces June merged away (Besma Mine / Lake / Dragon Road; e.g. Mine s1 `bc1` rebuilt, `bc2`-`bc5` gone). June's
    world scripts still name them and they fail to load (JUNE_BUGS #166).
  - March's copies drew over June's rebuilt meshes. March's `CX2WorldObjectMesh::SetXMesh_LUA` logs `XEM_ERROR109` and skips the
    draw; render and `RayHit` check the mesh, as June does. List: `c3z1_june_dropped.tsv`.
- **Loose music: 17 BGM** in `data\music\` replaced by June's re-encodes (Hamel, Velder secret, Sander, Ruben tutorial; about half
  the size). Backup `kom_backup_tools\loose\20261010_142700\`.

**Other users** (`c3z1_users.py` -> `c3z1_users.tsv`; every .lua and inflated .x/.y in kom_contents). June wins, so these change:
- 135 March-side files name a changed name. All but 4 are files June dropped and no dungeon world reaches: old Besma
  Lake / Dragon Road / Mine2 / Dragon Nest world scripts and meshes, Altera Below Tunnel floors, `Altera_Battle_Air_Ship_s1-s4.lua`.
- **Reachable: one.** `Besma_Dragon_Road_Stage4.lua` (KidDay event, a March-only world) loses 15 of its removed meshes.
- Village/field meshes now drawn with June's textures: `Besma_Village_G01/G02.Y`, `Field_Besma_Rest_G01_Left/Right.Y`,
  `Field_Ruben_G02_Mesh.Y`. Also `Elder_Hell_Extra_Stage1.lua`, which no dungeon reaches.

**Gate** (`dungeon_map_parity.tsv`): 354 rows, 330 read 0/0. The 24 that do not all name `data101-dir`, none unnamed:
- Velder Gate / Market / Palace Entrance / Bridge Hope / Bridge Burning / Third Dwelling / Secret, Henir Space x4, Hamel Waterway,
  Sander Santilus Ship, and 5 Ranox dungeons with 1 shared texture each.
- data101's kom_contents dir lacks 1,071 live entries, and 134 of its files are stubs smaller than live (`..._junk02.y`: 111 B, live
  5,657 B), so it is never packed. Its 104 differing or missing names wait in `c3z1_data101_pending.tsv`; some only differ because
  of the stubs. Its 799 live-only entries cannot be compared until
  june-asset-port P0 restores the dir; that follow-up is **C3z-1b**.
- KidDay reads `wf march-only=4`: its 2 event worlds (Air Ship s6, Dragon Road 4) are worlds June dropped.
- `world_asset_collide.py` over all 728 world files: 89 DIFF, **all data101** (`c3z1_world_asset_collide.tsv`).
- `npc_mesh_census.py --worlds` over the same list: 2 needed, **both data101** (`c3z1_world_asset_need.tsv`).
- C3x's own `world_asset_*.tsv` records are kept as they were.

**Packs and copies.** `PACK OK` for all 11. Every one of the 404 entries decodes equal to June's bytes.
- The first-time packs of data024 / 025 / 027 / 030 / 110 show 570 / 360 / 415 / 507 / 1,138 extra `~changed`, all recompression:
  the same raw size and algo, and 440 sampled entries decode equal to live. One data024 file (`Ruben_Ruin_Of_Elf_S1_alpa1.Y`) has the
  same raw and packed size in both builds, so it does not show as `~changed`.
- data023 still warns of 13 manifest keys with no file; they predate this phase.
- **`COPY OK`** (user OK 2026-10-10):
  - data023 md5 `fad26f15`, 024 `82a7debd`, 025 `93ba7838`, 026 `ce0eb4b5`, 027 `c9314c2d`;
  - 028 `c4e98d83` (852 entries) and 029 `eae9a4c9` (1536 entries), both `--allow-removed`;
  - 030 `c826ed4d`, 031 `318161fa`, 032 `0eccdecd`, 110 `b0bad26b`;
  - 17 music files `COPY OK` (md5 = June's).
- No C++ changed, so there is no flagcheck and no build; the deployed exe is unchanged.

**Play-test:** the kickoff's three maps, plus Besma Mine stage 1, where the removed pieces were. Logs:
- 0 EXCEPTION.
- Expected: `XEM_ERROR109` / `XEM_ERROR110` (LOD) / `KEM_ERROR136` lines naming a file in `c3z1_june_dropped.tsv` (June logs the
  same), and Velder Bridge Hope 3-1's 4 data101 meshes (`River_Wave`, `Wall01`, `Back_Gate`, `Gate_Window`; C3z-1b).
- Any other name is new. A missing mesh fails once per load: `SetLOD(bool)`'s reload (`X2WorldObjectMesh.cpp:352`) has no caller, and
  `OnFrameMove` closes a FAILED mesh without retrying.

### C3z-1 - play-test 1 (2026-10-10 14:40): crash at Air Ship stage 1 -> 199 NPC assets

- **Crash:** Battle Air Ship stage 0 crashed during sub-stage loading. The CrashRpt bundle has an access violation at `0x32e77ac8`,
  which is `CX2GUNPC::Init+0x88` (`X2.map`, base `0x32830000`): `m_pMotion` was NULL.
  - The last lines before it are `Airship_Soldier_RepairMan.lua` and
    `OpenXSkinMesh` failing on `MOTION_ALTERA_AIRSHIP_SOLDIER_REPAIRMAN.X`.
  - That is one of the June ids C3z-2 added. June ships the mesh (`data009`, strict decode ok); March had none.
- **Cause:** C3s's `--all-npc` census used the old greedy tokenizer, so it missed names glued onto the constant before them. With
  this phase's fix, `npc_mesh_census.py --all-npc` found **199 June-only NPC files**: 54 `Motion_*` meshes, the rest models and
  textures. List: `june2017_re/c3z1_npc_mesh_need.tsv`.
- **Fix:** staged all 199, all new and 0 refused (plan `c3z1_npc_stage_plan.tsv`). First, all 29 target dirs were checked equal to
  their live koms (names and sizes).
  - `PACK OK` for all 29. Only data117 shows extra changes: 117 `~changed`, all recompression (each decodes equal to live).
  - Copied with the user's OK: data002 / 006-009 / 012 / 013 / 017 / 023 / 033 / 043 / 100 / 107 / 111 / 116 / 117 / 136 / 140 / 147 /
    165 / 180 / 181 / 184 / 189 / 211 / 212 / 225 / i002 / i027. All 29 are `in sync` (live md5 = packed).
  - Re-run: `--all-npc` needs **0**.

### C3z-1 - play-test 2 (2026-10-10): gates stay invincible -> IsVotingLuto bound; 37 more unbound methods -> C3z-4

- **Symptom (user):** on Battle Air Ship and Wally Castle Roof the gate stays invincible after the room is cleared. Not every
  dungeon has the problem.
- **Cause:** ErrorLog repeats `can't find 'IsVotingLuto' class variable` (>1,000 times). Ten June gate scripts start
  `CF_SMALL_GATE_*_WAIT` with `if slot1:IsVotingLuto() == true then return false end` and only then check `LiveActiveNPCNum() == 0`
  to leave `..._WAIT_INVINCIBLE`. March never bound that method, so the call aborts and the gate never changes state.
  - The ten: `SMALLGATE_WALLY_CASTLE_A`, `SMALLGATE_BESMA_AIR_SHIP`, `_DRAGON_ROAD`, `_LAKE`, their `_POST_` pillars,
    `LatticeGate_BelowPassage` and `Velder_market_gate`.
  - Gates whose condition does not ask (`SmallGate_Red`, `SMALLGATE_BENDERS_CAVE_A`, ...) work, which is why only some dungeons
    are affected.
  - **58 dungeons reach them:** Altera Air Ship / Nasod Factory / Plain / Plain Recycle / Secret, Besma Air Ship / Dragon Canyon /
    Dragon Road / Lake / Lake Night / Secret, Elder Below Passage / Wally Center / Lab / Roof / Secret / New Year, KidDay, and
    Velder Market / Secret.
- **June (RE):**
  - The binder `31d95080 @31dda25e` registers `IsVotingLuto` on `CX2Game`, after `InitDLG_RutoVoteSystem`, as the vcall thunk
    `31e4e9c0`, which calls this->vslot `0x244`.
  - `CX2Game 330e7c10` returns true. `CX2DungeonGame` and `CX2PVEGame` (`327c94a0`; vtables `334bb094` / `334c1f34`) return
    `+0xdd1 || +0xdd2 || +0xdd3`.
  - The ctor `327cef80` zeroes those bytes (`@327cf464`). Only June's Ruto vote system writes them (`327caa70`, `327cc19d`,
    `327d28c7` / `327d28dc`, `327d4149`), and March has no vote system. Not ported; the answer offline is June's with no vote
    running: false.
- **Port:** `SERV_IRUHADEV_JUNE_IS_VOTING_LUTO`:
  - `CX2Game::IsVotingLuto` is a virtual returning true (`X2Game.h`), bound in `X2Main.cpp`.
  - `CX2DungeonGame` overrides it over `m_abJuneVotingLuto[3]`, zeroed in the ctor.
  - The game reaches Lua as `g_pX2Game` (`X2GUNPC.cpp:23509`, a `CX2Game*`), so the override is the one that runs.
  - `SERV_IRUHADEV_JUNE_IS_VOTING_LUTO_DEBUG` (short-lived) logs `JUNE_FX C3z-1 IsVotingLuto %d, LiveActiveNPCNum %d <id:script ...>`
    once a second.
- **Census of the whole class:** `june2017_re/c3z1_unbound_reach.py` -> `c3z1_unbound_reach.tsv`.
  - It covers every method called (`x:Method(`) by a script a registered dungeon reaches (unit scripts, world files, the dungeon
    script) that no March string literal spells. March has no macro binders.
  - It also scans each dungeon's June source for its `NUI_` names, because C3z-2's `read_by` column is cut at about 400 characters.
  - Result: 2,363 scripts, **37 unbound methods left** after `IsVotingLuto`. Both runtime `can't find` names are in it
    (`IsVotingLuto`, `SetMovieDescriptionData`).
  - Routed to **C3z-4**. `GetHostGameUnit` and `GetDyingStart` already exist in March's C++ and only need binding; the rest need
    June's bodies or June's no-system answer.
- Hygiene: `flagcheck --worktree --files` (with `_DEBUG`): 3 PASS, 1 PASS~ (a blank line before the `X2DungeonGame.h` block), 1 DEFINE,
  0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatches; RESULT: PASS.
- `DEPLOY OK: X2_offline.exe is 19,533,312 B, dated 2026-10-10 15:15:55, md5 f8f23b2b0c7e9af175999d3f1e9c52c3` (`IsVotingLuto` and the
  `JUNE_FX C3z-1` log string are in the exe).

### C3z-1 - play-test 3 (2026-10-10 15:20): PASSED

- **User:** the phase's checks passed, and the Air Ship crash is gone.
- **Logs** (exe `f8f23b2b`, run after the deploy):
  - 114 `JUNE_FX C3z-1` lines, all `IsVotingLuto 0`. Live counts ran from 12 down to 1, then the gate stopped asking: it left its wait
    state at 0.
  - No `can't find 'IsVotingLuto'` line, no new CrashRpt bundle, clean exit.
- **New in the run:** one `can't find 'CreateNPCReq_LUA'`. March binds it on `CX2Game` only; June also binds it on `CX2GUNPC`. The
  name-only census cannot see that, so C3z-4's kickoff now starts with a class-aware census.
- `SERV_IRUHADEV_JUNE_IS_VOTING_LUTO_DEBUG` is off (commented out in `Always.h`).
  `DEPLOY OK: X2_offline.exe is 19,532,288 B, dated 2026-10-10 15:38:16, md5 d60e4b9fbfb265962cd94a2ca7ed2a2f`; `IsVotingLuto` is still in the exe,
  and the debug string is gone.

### C3z-2 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data036 `Enum.lua` + data037 `NPCTemplet.lua` packed and copied)

**Census tool.** `june2017_re/dungeon_npc_census.py` (with `dungeon_npc_dump.lua` and `lua_registry_dump.lua`).
- It runs all 343 dungeon scripts in kom_contents through the vendored KOG luajit, which loads `1B 4B 4C 81` bytecode as-is.
- It walks them the way the client does: `STAGE_LIST` / `STAGE<n>` / `SUB_STAGE<n>`, `New_LoadNPCData` (`NPC_GROUP_RATE` + `NPC_GROUP<N>`, else `NPC_GROUP`, `SUB_NPC<i>`), and the `CLEAR_COND<c>.NPC_ID` lists.
- It adds a `NUI_` token scan of each script and the summon closure through the NPCs' unit scripts.
- It runs the registries (Enum, NPCTemplet, NPCStat, NewNPCStat, DungeonData) the same way.
- It reproduces the emulator's Druid Forest parse: 10 / 13 / 3 entries.
- It writes `june2017_re/dungeon_npc_census.tsv` (per name), `dungeon_npc_parity.tsv` (the gate) and `dungeon_npc_add.tsv` (empty now).

**Covered scripts.** 233 of the 343: those March's DungeonData registers, plus those June registers under a dungeon id March also has. All of the latter are March-registered already, so C3z-3 adds none.

**Finding: 142 June-only ids missing from March's `NPC_UNIT_ID`.** All of them read nil, so their entries never spawned and their clear conditions emptied.
- 96 are named by the covered scripts, and the summon closure adds 46 more through their unit scripts.
- What they are:
  - Druid Forest's 1608-1614.
  - Besma Lake Night's two gates (in `CLEAR_COND`).
  - The story-stage event NPCs of 14 dungeons (1605-1646, 1726-1727).
  - Air Ship's `AIRSHIP_SOLDIER_SLESHU` and the Below Tunnel boss.
  - Every `_HENIR` boss in Henir Space, plus their summons (1728-2009, 10154-10167).
  - The Elysion boss parts.

**Edits.**
- **`Enum.lua`** (BOM source, byte-level, CRLF kept): June's 142 values appended in an Iruha block.
  - No collision with March's Lua values.
  - No collision with the C++ `CX2UnitManager::NPC_UNIT_ID`, resolved with `cl /P` (2,023 members, implicit ones included).
  - None of the 142 names is a C++ member.
- **`NPCTemplet.lua`:** `luasrc.py convert` (T0 from the sidecar), then June's 142 rows appended verbatim, in June's order.
  - By value they equal June's own `data037` bytecode (ljconv'd and executed).
  - The 2,026 March rows are unchanged.
  - Every `CLASS_TYPE` resolves in March's `NPC_CLASS_TYPE`, and every row has `IS_NPC` (`AddNPCTemplet_LUA` returns false otherwise).
- **Unit scripts and meshes:** all 142 unit scripts are staged, and no `.x` they name is missing.
  - They are also in the live koms (`kom.live_names()`).
  - `npc_mesh_census.py --all-npc` (textures included) needs 0 files.
- **Stats:** 125 of the 142 have an ExtraNPCStat (`NewNPCStat.lua`) row and 17 an `NPCStat.lua` row; none has neither.
- **Bookkeeping:** `luasrc.py refresh` on both files; `ledger --check` PASS, `core --check` PASS (the NPCTemplet core row is now `source`).

**C++: nothing to change.**
- `NUI_MAX_LIMIT` (10191) bounds only an item's summon NPC, and only to log it (`X2ItemManager_Preprocessing.cpp:1501`). The highest new id is 10167.
- The templet map is keyed by value, and `FetchNPCData` casts.

**Gate (`dungeon_npc_parity.tsv`).** 2,510 sub-stages: **2,130 PASS, 380 EXC, 0 FAIL.**
- EXC: 330 "C3y: June-only dungeon", 50 "C3y: registry-less script".
- No covered sub-stage is left with only a box or a gate.

**Hand-off lines.**
- `PACK OK data036` / `PACK OK data037` (each `~1 changed`).
- `COPY OK data036: ... md5 920afa6ae9545708dc31c46db19bd7c7`.
- `COPY OK data037: ... md5 42bc49305f49b1402d4b8f47c6b2ccdf`.
- Exe unchanged.

**Open, for the user.**
1. **Drop and EXP.** `KncWX2Server/ServerResource/US/DropTable.lua` is keyed by dungeon + monster id and has no row for any of the 142; no June server exists.
   - The emulator pays EXP and ED only on a row with `Exp > 0`, so these monsters give 0 EXP and 0 ED. The dungeon's static drop still comes.
   - No rows were invented. A stand-in (own flag, e.g. a June monster borrowing the dungeon's old March rows) is a design call: say if you want it planned.
2. `NUI_EVENT_GLITER_LANCE_GREAT` is 4035 in March and 4190 in June. Only the June-only `Dungeon_Event_Alchemy_Lab` uses it -> C3y. Not renumbered.
3. The 24 `NUI_PVE_*` (Island of Erda) have no March templet; that dungeon is registry-less in March -> C3y.
4. Ten names June's own scripts read exist in no Enum (`READY_NPC` typos) -> `JUNE_BUGS.md` #165, kept.
5. **For Atlas / Elysion:** the Elysion ids added here (`NUI_ELSION_MAYA`, `NUI_ELYSION_BOSS_ORB` / `_SHIELD`, `NUI_ELYSION_OBJECT_HERBAON1` / `2`, the `_HENIR` parts) now exist in March's Enum and NPCTemplet. Do not add them twice.

**In game.**
- The `[EXTRA_NPC_STAT] loaded 1693 rows (skipped: N nil NPC_ID` line drops from 835 to **709**.
- Elder Druid Forest spawns its monsters and no longer clears itself. Exact checks are in the hand-off.

**Play-test PASSED (user, 2026-10-10; run 14:47-14:55, after the 14:22 kom copy; logs read).** Character: reyaa (Lire Grand Archer, unit 12).
- **Druid Forest (30070).**
  - Spawns: stage 0 `+NPC` ids 1608 / 1609 / 1610 / 1611 / 1614 (10 entries); stage 1 has 13; boss stage has the Ent 1613 (BOSS) and 2× 1611.
  - Every one died to KILL_BY_USER, and the stage advanced only after gate 1614 died. Then `30070 CLEARED`.
- **Besma Lake Night (30200).** Gates 1633 / 1634 spawned in sub-stage 1 of stages 0 and 1 and were killed. Then `30200 CLEARED`.
- **Startup line:** `[EXTRA_NPC_STAT] ... skipped: 709 nil NPC_ID` (was 835). 0 EXCEPTION.
- **As expected:** the June monsters gave 0 EXP and 0 ED (open item 1). The boss still drops through the special-drop path.
- The `IsVotingLuto` / `SetMovieDescriptionData` Lua errors in this run are C3z-1 / C3z-4 scope (`65ef74f`), not this phase.

### C3z-3 - Done (2026-10-10, Opus 5.5 high; no C++, no flag, no build; data035 + data036 + data065 packed and copied)

**Gate:** `june2017_re/dungeon_registry_census.py` (driver `dungeon_registry_dump.lua`) runs March's staged files and June's own
bytecode (junekom + ljconv) in the KOG luajit and diffs every `AddDungeonData` table per dungeon id.
- 217 ids registered by both (the 4 `*_SECRET_COMMON` count: June registers them through `UNUSED_DUNGEON_ID`).
- Before (`dungeon_registry_census_before.tsv`): 155 differ, 62 equal. **After (`dungeon_registry_census.tsv`): 217 SAME.**
- Names: every shared `dungeonName` is the same `STR_ID`, so no dungeon was renamed. Every June `STR_ID` exists in March's
  `StringID_def.lua` and `General.ess`. All 175 item ids in June's lists are in the live item.kim. The room titles are equal.
- Stage lists and time limits are not in `DungeonData` (stage lists are in the dungeon scripts, June's since C3x).
  No shared `m_fTimeLimit` differs.

**What June changed in the 155** (all now live):
- `BOSS_DROP_ITEM` (129) and `ENDING_SPEECH` (53): new lists. That is the party UI's boss-item list (`X2PartyUI.cpp:2065`).
- `requireDungeonID` dropped in 92 rows: Hard no longer needs Normal, Expert no longer needs Hard (`IsActiveDungeon`,
  `X2DungeonManager.cpp:603`). `GetNextDungeon`'s "new dungeon" list shrinks to match.
- The 4 Secret Hells (33031 / 33041 / 33051 / 33191):
  - levels 50-70 become 70-99, item level 85 becomes 90, entry item 700015;
  - no `EXTRA_REQUIRE_DUNGEON_ID`, and no `IS_HELL_MODE`;
  - Elder's image is now `Image06` / `Dungeon_Image_Elder_Secret`.
- Henir (38100): 80-99, item level 120, `m_NPCLevel` 99, not relative.
- Besma Lake's extra requirement is emptied. 14 event / defence rows gain `DUNGEON_CHECK_CONDITION`.
- `IS_HELL_MODE`: June's loader never reads it (54 keys: the 47 XOR-guarded ones in `xorstrings.tsv`, matching the 47 guard
  blocks in `327e0f80`, plus 7 plain). March still reads it, and June's data now sets it on the Elder / Besma `*_SECRET_COMMON`
  rows. That is a no-op in March: the `<name>_Icon_Info` readers (`X2LocalMapUI.cpp:1486` / `1548` / `1569`) sit in the `#else` of
  `SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS` (no such string in the exe), and `CanGoOneSecretDungeonAtLeast` has no caller.
- **Monster levels (live now):** Henir is a fixed `m_NPCLevel` 99 (June's design), and the Secret Hells' relative base is the
  emulator's `m_MaxLevel` (99). No crash path was found:
  - EXP clamps to NpcExpTable's last row, level 80 (`X2OfflineDropTable.cpp:710`);
  - NPC stats are formulas (`CalculateExtraStatValue` takes the level as a number);
  - buff factors index by the buff's level and return 0 past the end.
  - Difficulty is not June's until C3z-6.

**Edits** (`luasrc.py convert` T0 for both registries; `refresh`; ledger and core PASS):
- `DungeonData.lua`: each of the 155 blocks is replaced in place by June's block (`june_src_fixed`), between `--{{ Iruha ... June's
  row` / `--}}` markers. March's 31 own rows stay. June's 251 June-only rows are not added (C3y).
- `DungeonEnum.lua`: June's `UNUSED_DUNGEON_ID` (62) appended verbatim; its 59 March names keep equal values. `DI_END` is now 41530,
  June's value (was 41120); no Lua reads it.
- `Enum.lua`: June's `DUNGEON_CHECK_CONDITION` (16) appended. Without it the script stops at row 86, and every later row is lost.
- Both tables are equal by value to June's own bytecode.
- **Strict load test** (`dungeon_registry_strict.lua`): March's real Enum / DungeonEnum / StringID_def, and any unknown global or
  enum member raises. Result: `OK 248 rows, 10 titles, 0 fail(s)`.
  - Controls: without the new Enum table the script stops at row 86; without `UNUSED_DUNGEON_ID` it stops at row 172. data035 and
    data036 ship together.
- `DLG_UI_Dungeon_Image06.dds` (data065): June's copy staged; it differs from March's in 2,860 DXT5 bytes, with the same TET.

**Packs and copies:**
- `PACK OK` data035 (`~1`), data036 (`~2`), data065 (`~1`).
- `COPY OK data035 ... md5 780396af690c95b4cd7f40097dac2597`
- `COPY OK data036 ... md5 9ee02c70935f7949dc0ab9ac7622d6b5`
- `COPY OK data065 ... md5 093b1ef0cf210567517908fd9b66d8f1`

**Routed, not done here:**
- C3z-5: the June client fields March's loader skips.
- C3z-6: the emulator's relative monster level, which now spawns level-99+ monsters in the Secret Hells, and the server-only keys.
- Checklist 7s: the AIN / LUCIEL / ROSE tables in `BOSS_DROP_ITEM` / `ENDING_SPEECH`.

**Play-test PASSED (user, 2026-10-10; run 16:20-16:27 after the 16:05 / 16:07 copies; logs read).**
- reyaa (Lire Grand Archer, unit 12) played Ranox Geyser (33290) through stages 0-3. Every Ranox button resolves
  (`[RANOXDBG] ... pDungeonData=OK`), so the file loaded past row 172.
- Freils (Chung Tactical Trooper, unit 18, level 72) started Elder Secret Hell (33031) by quick start without the Elder Expert
  clears, and stage 0 loaded (36 NPCs).
  - The emulator logged `using its own level band (99)`, as predicted (C3z-6).
- Wally Roof Expert being selectable was the user's own check. No room was started for it, so no log line covers it.
- No `DungeonData` / `XEM_ERROR2` / `can't find` / `attempt to index` line. 0 EXCEPTION.
- 4 UNHANDLED: 3 are `E_DISABLE_HB_CHECK_REQ` and 1 is `EGS_SKIP_CONTINUE_DUNGEON_NOT`; neither is this phase's.
- Geyser's 0 EXP rows come from DropTable (no Ranox monster rows), not this phase.

### C3z-4 - Done (batch 1) (2026-10-10, Opus 5.5 high; 7 flags + `SERV_IRUHADEV_JUNE_C3Z4_DEBUG`)

**Census, class-aware** (`june2017_re/c3z4_class_census.py`, committed `3d94334`).
- June side: the two binder string rows `31be67a0` / `31d95080`, read as class then methods; 135 classes.
- March side: the six binder files under `cl /P` (dead `#ifdef`s drop out), with `class_inh` walked; 122 Lua classes, 569 class gaps
  (`c3z4_class_gap.tsv`).
- Receiver typing per call site:
  - Callbacks have the script kind's signature: unit scripts are `(CKTDXApp, CX2Game, CX2GUNPC)`, dungeon triggers are
    `(CKTDXApp, CX2DungeonGame, CX2DungeonSubStage)`.
  - Helpers take their types from their call sites. Getter return types are read inside the declaring class.
  - 68,795 of 95,316 reached sites typed (72%). An untyped site of a name June binds on a class March lacks is kept as `untyped-gap`.
- Before batch 1: 65 rows / 59 methods. After: **45 rows / 45 methods** (`c3z4_unbound.tsv`).
- Every reached name the name-only census had (incl. both live ErrorLog names) is in it. `CreateNPCReq_LUA` on CX2GUNPC was the gap
  that census missed.
- Triage of every row: `june2017_re/c3z4_bind_plan.tsv` (June use site, bound fn, body summary, flag, batch).

**Batch 1 (this commit):**

| Flag | Lua methods (class) | June | Dungeons |
|---|---|---|---|
| `_NPC_CREATE_NPC_REQ` | CreateNPCReq_LUA / _LUA2 / Elite / Ally, PushCreateNPCReq_Lua (CX2GUNPC) | `327790e0`, `327791c0`, `32779ad0`, `32779bc0`, `32779cb0` | 113 / 4 / 13 / 4 / 82 |
| `_NPC_STORED_GUUSER` | Get/SetStoredGUUser (CX2GUNPC) | `32759660`, `31e57960` | 37 / 26 |
| `_GAME_LUA_INT` | Get/SetInt_LUA (CX2Game) | `31e4de10`, `31e4ddf0` | 19 / 13 |
| `_NPC_INIT_STATE_COOL_TIME` | InitStateCoolTime (CX2GUNPC) | `3275d9d0` | 15 |
| `_GAME_HOST_GAME_UNIT` | GetHostGameUnit (CX2Game), bind-only | `31aa6280` | 13 |
| `_NPC_GET_DYING_START` | GetDyingStart (CX2GUNPC), bind-only | `31e4ce00` | 12 |
| `_GAME_STAGE_QUERY` | GetIsLastStage, GetNowStateID_LUA (CX2Game) | `322f36e0`, `322cf9b0` | 12 / 12 |

- **NPC create requests.** Each one is March's `CX2Game::CreateNPCReq`, which keeps June's host gate, called with the NPC's
  `GetUID()` as ally UID. March only reads that UID for `NAT_ALLY`, and an NPC UID finds no user, so `Ally` summons get no owner
  user, as in June.
  - June's extra packet field (the summoner's templet unit ID, `+0x85f8[0]`) is not carried (user, 2026-10-10): row C3z-4f.
  - June's NPC `PushCreateNPCReq_Lua` creates at once, with no push list (JUNE_BUGS #168).
- **Game ints.** Ten ints (`+0x7e0`), zeroed in the ctor `32306600`, with no bounds check (JUNE_BUGS #167). June's DungeonGame
  writes `[0]` only for `DI_EVENT_2017_TREASURE`, which March lacks.
- **`GetNowStateID_LUA`.** June inserted `XS_GUILD_AGIT_GAME = 15`, and the scripts compare compiled literals
  (berserker: `== 18`, June's `XS_TRAINING_GAME`; March's is 17, by `enumprobe`).
  - March's state is returned under June's number, by name. The four arcade states return -1, a labelled stand-in.
- **`InitStateCoolTime`.** Sets `runtime[id].m_fStateElapsedTime` to the state's `STATE_COOL_TIME`, so the state is ready at once.
  The stored-user observer is cleared only by June's ctors and dtor, so a default-constructed member is enough.
- Not bound, by design: 6 names no June binder has, so June fails the same way (JUNE_BUGS #169). The tutorial guide (3) goes to row
  C3z-4t and SimpleDLG (8) to row C3z-4s. 22 ports plus 5 untyped-gap rows go to C3z-4b (kickoff `C3z-4b.md`).

**Hygiene.** `flagcheck --worktree --files` on all 8 flags: RESULT PASS, 0 deleted, 0 non-ASCII, 0 EOL mismatches (378 lines
inserted, 6 files; the OTHER rows are the sibling batch-1 flags sharing the files).
After the commit, `flagcheck --range 6788262^..6788262 --files <the 6 code files>`: RESULT PASS for all 8 flags.

`DEPLOY OK: X2_offline.exe is 19,537,408 B, dated 2026-10-10 16:50:29, md5 c1bcf39df31a318dca435b0b52092e87, equal to X2/US_SERVICE/x2.exe (2026-10-10 16:50:24); built at mods/june-buff-port 7482b9e + 6 uncommitted code file(s).`
The 7 method names and the `JUNE_FX C3z-4` log strings are in the exe.

**How to see it.**
- Ranox Fire Temple Normal: no gap left. The berserker / fighter scripts read the game ints, last-stage and state ID; ska /
  smith stain call `InitStateCoolTime`; the ignia / sniker fallbacks call `GetHostGameUnit`.
- Sander Den of Trock Normal: trock summoners call `CreateNPCReq_LUA` / `PushCreateNPCReq_Lua` and Get/SetStoredGUUser. One
  `can't find 'GetNPCUnitNumCheckTeam'` (C3z-4b) is expected.
- Log: `offline_server.log` `JUNE_FX C3z-4 ...` (first 10-30 calls per method). ErrorLog has no `can't find` for a batch-1 name.
- Not exercised by these two dungeons: `GetDyingStart` (Elysion / Henir / Ranox Secret Hell scripts) and `GetHostGameUnit`
  (the last fallback of `GetUser`, after the target and a random user are both nil; no log line). Both are bound and in the exe.
- **Summon IDs March lacks.** The newly bound NPC calls name 176 NPC ids. 8 exist only in June's Enum: `NUI_ELSION_PIMPIM`,
  `NUI_ELSION_SUMMON2`, `NUI_SUMMON_SAND_HAND` (+`_EXTRA`), `NUI_TARANVASH_THE_SHINE_FAKE_A_HENIR` / `_B_HENIR`, `NUI_THIEF_FAT_HENIR` /
  `_THIN_HENIR`. In March the nil id arrives as 0, `CX2Game::CreateNPC` finds no templet and creates nothing, so there is no crash.
  Their Enum + NPCTemplet rows (the C3z-2 recipe) go to C3z-4b. The other 6 have rows and are inside C3z-1's `--all-npc` mesh closure.

### C3z-4 - play-test 1 (2026-10-10 20:06-20:48): inconclusive, the batch-1 paths were not reached

- Played Ranox Fire Temple Normal (33310: stage 0 for 52 s, then `/stage 3` for 30 s) and Sander Den of Trock Normal (33220:
  `/stage 2` first room only, then `/stage 3` for 49 s).
- 0 EXCEPTION, no crash, no `can't find` line in ErrorLog, and also no `JUNE_FX C3z-4` line: `EGS_NPC_UNIT_CREATE_REQ` 0 times.
  - The berserkers (`GetNowStateID_LUA` / `GetIsLastStage` on spawn) are in stages 1 / 2, which were skipped.
  - The iplitans and the chief died before `ATTACK_B_EVENT_FIRE` / `HP_DRAIN` / `SA_A` / `SA_B`.
  - The summon caves are in Den stage 2's second room, which was never opened.
- Re-test: `/stage 1` in Fire Temple; Den `/stage 2` past the first room, and `/stage 3` waiting out the chief under `/invin`.

### C3z-4 - play-test 2 (2026-10-10 21:12-21:21): chief summons and stores work; caves blocked by GetNPCUnitNumCheckTeam -> bound

- Den of Trock stage 3 (reyaa, unit 12): the chief's `CHIEF_OF_TROCKTA_SA_B` calls `CreateNPCReq_LUA( NUI_TROCKTA_TRAP 1227 )` five
  times (`summoner=1000089 team=2 ai=0 keycode=-1`) and HP_DRAIN calls `SetStoredGUUser( user 12 )` three times. 0 EXCEPTION.
- Den of Trock stage 2, room 2 (user: "no summons"): every `TROCK_SUMMON_CAVE*_SUMMON_*_FRAME_MOVE` asks
  `slot1:GetNPCUnitNumCheckTeam( team ) <= N` before its `CreateNPCReq_LUA`, so the batch-2 method stopped the summon (5 `can't find`).
  The hand-off was wrong to expect summons there. Fix: `SERV_IRUHADEV_JUNE_GAME_NPC_COUNT` binds June `322d4a10` (@31dd13bf) now:
  it counts every NPC in the list whose team equals the argument, with no other test.
- Fire Temple: no berserker line, because Fire Temple names `NUI_FIRE_DEVIL_BERSERKER` only in `READY_NPC` (preload) and never
  spawns it. The C3z-1 reach counts `READY_NPC` too. Plain berserkers and fighters spawn in **Ranox Gateway stage 3** (and Henir).
- `flagcheck SERV_IRUHADEV_JUNE_GAME_NPC_COUNT --worktree --files` (4 files): 2 PASS, 1 PASS~, 1 DEFINE, 0 FAIL, 0 deleted; RESULT PASS.
  `DEPLOY OK: X2_offline.exe is 19,538,432 B, dated 2026-10-10 21:32:59, md5 5445212b23053255f72f6735753edb1c` (the method name and its log string are in the exe).

### C3z-4 - play-test 3 (2026-10-10 21:47-21:54, reyaa unit 12): PASSED for summons, stored user, game ints, cool time; Gateway still owed

- 0 EXCEPTION, no crash, **no `can't find` line at all** in ErrorLog (play-test 2 had 5 for `GetNPCUnitNumCheckTeam`).
- **Den of Trock stage 2, room 2 (33220, 21:48).** The caves ask `GetNPCUnitNumCheckTeam team=2` (10 logged, `-> 10`), then
  `CreateNPCReq_LUA` with npc ids 1203-1207 (`NUI_RASH_*_SUMMON`, e.g. 1205 `NUI_RASH_BOMB_TROCK_SUMMON`), 30 logged. The trocks appear.
- **Ranox Ash Village (33280, 21:51-21:54, stages 0-4).** The smiths' `SetInt_LUA` [0] / [1] / [2] / [4] (20 logged) and an
  `InitStateCoolTime` (npc 1000113, state 13 of 44). The user saw the workers spawn. Their `CreateNPCReq_LUA` lines are not in the log:
  the debug caps (30 creates, 10 counts per launch) were already used up in Den of Trock.
- Fire Temple stages 3 and 1 (21:49-21:51): no C3z-4 line (the iplitans did not reach `ATTACK_B_EVENT_FIRE`); not a failure.
- **Still not exercised:** `GetNowStateID_LUA` / `GetIsLastStage` (spawn of a plain berserker: Ranox Gateway stage 3),
  `PushCreateNPCReq_Lua`, `GetStoredGUUser`, `GetHostGameUnit`, `GetDyingStart`. Gateway was not played.

### C3z-4 - fix: a dungeon NPC's TEAM (user, 2026-10-10: Gateway's Ignia counts as an enemy, the stage never clears)

- Ranox Gateway stages 0-2 spawn `NUI_SAMETEAM_IGNIA` with `ALLY_NPC = TRUE, TEAM = TEAM_NUM.TN_RED`.
  - June's sub-stage NPC parser `327eb8e0` reads TEAM right after ALLY_NPC (`@327ec9a9-327eca04`), into NPCData `+0x5c`, 2 when absent.
  - March's client and server parsers never read it, and the emulator left `KNPCUnitReq::m_cTeamNum` at its default 2 (TN_MONSTER).
    Ignia arrived as a monster, so `CC_KILL_ALL_ACTIVE_NPC` (`LiveActiveNPCNum`: TN_MONSTER, active) never reached 0.
- Fix `SERV_IRUHADEV_JUNE_SUBSTAGE_NPC_TEAM`:
  - `FetchNPCData` reads TEAM (default TN_MONSTER) into `NPCData::m_iJuneTeam`, and `AddNPCData` copies it from a packet.
  - The emulator's sub-stage spawn (`Handlers_Room.cpp`) sends it as `m_cTeamNum`.
  - `JUNE_FX C3z-4 substage npc <id> team <t> ally <a>` is logged for every non-monster team.
- Scope: 8 June dungeon scripts set TEAM (91 entries): the 3 Gateway difficulties (Ignia, 9) and June's PVE red / blue scripts (not
  reachable in March). flagcheck (4 files): 3 PASS, 1 DEFINE, 0 FAIL, 0 deleted; RESULT PASS.
  `DEPLOY OK: X2_offline.exe is 19,538,432 B, dated 2026-10-10 22:10:13, md5 d63e075e6a77d586d386429e25bc505b`.

### C3z-4 - play-test 4 (2026-10-10 22:17-22:20, Ranox Gateway Normal 33300): berserker queries PASSED; Ignia team applied

- `JUNE_FX C3z-4 substage npc 1897 team 0 ally 1` at stage 0's spawn: `NUI_SAMETEAM_IGNIA` now spawns on TN_RED.
  The run went from stage 0 to `/stage 3` after 70 s, so no stage was cleared with Ignia alive (stage 0's clear condition is
  `CC_KILL_NPC_TYPE`; stages 1 / 2 use `CC_KILL_ALL_ACTIVE_NPC`, the one Ignia blocked).
- Stage 3 (the Ifritan room): four berserkers log `GetNowStateID_LUA march 11 -> june 11` and `GetIsLastStage 1` at spawn; the game
  int `[0]` is set to 1 and then -1. Sub-stage clear and `EGS_END_GAME` at 22:19:55-22:20:05: the dungeon finished.
- ErrorLog: one `can't find 'SetMovieDescriptionData'` (the boss intro subtitle, C3z-4b), nothing else. 0 EXCEPTION.

### C3z-4 - play-test 5 (user, 2026-10-10): Ignia still an enemy (red minimap dot, homing arrows) -> the client-side create

- The emulator's packet was right (team 0), but the client builds sub-stage NPCs itself in `CX2DungeonSubStage::CreateGUNPC`
  (June `327ef570`), and March hard-coded `TN_MONSTER` / `NAT_NORMAL` there.
- June `@327ef72d-327ef74b`: team = NPCData `+0x5c`, AI = `ALLY_NPC` (`+0x52`) ? 4 : 0.
  - The ctor `327af620` builds type 4 exactly as type 1 (the ally AI `32f0e3d0`, owner from the caller: none here).
  - It then stores 4 in the AI's type (`+0x38`), so the `== NAT_ALLY` user-summon tests skip it.
- Port (same flag `SERV_IRUHADEV_JUNE_SUBSTAGE_NPC_TEAM`, original call in `#else`):
  - `CreateGUNPC( 0, m_iJuneTeam, uid, info, ally ? NAT_ALLY : NAT_NORMAL, NULL, scale )`, then `SetAIType( 4 )` for an ally.
  - March's ally AI is NULL-owner safe (`RallyToAlly` returns early, `SetAllyGameUnit` ignores NULL).
  - Log: `JUNE_FX C3z-4 substage CreateGUNPC npc <id> team <t> ally <a> ai <type>`.
- flagcheck (2 files) PASS. `DEPLOY OK: X2_offline.exe is 19,538,432 B, dated 2026-10-10 22:34:45, md5 ffba1cb74f1e2605f0181890c49cf55a`.

### C3z-4 - play-test 6 (2026-10-10 22:37-22:41): PASSED; `_C3Z4_DEBUG` off

- Ranox Gateway Normal: `substage CreateGUNPC npc 1897 team 0 ally 1 ai 4` at stages 0 and 1. The user confirmed Ignia is
  friendly (no red minimap dot, homing arrows ignore her) and the rooms clear with her alive; sub-stage clears through stage 3.
  ErrorLog: only `can't find 'SetMovieDescriptionData'` (C3z-4b). 0 EXCEPTION.
- `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` commented out (OFF note in `Always.h`).
  `DEPLOY OK: X2_offline.exe is 19,536,896 B, dated 2026-10-10 22:51:28, md5 b8896722bee85d3bcb481d6f8b1d4df4`.
- Batch 1 is done. Not exercised in game: `PushCreateNPCReq_Lua`, `GetStoredGUUser`, `GetHostGameUnit`, `GetDyingStart` (rare
  boss moves / fallbacks); the review reads them against June.

### C3z-4 - Review (2026-10-11, Opus 5.5 high): one finding fixed (stage allies with an `IsAllyNPCID` id stay type 1); the rest matches June

**Census** (`c3z4_class_census.py`, re-run with a fresh `cl /P`): the committed TSVs come back unchanged, 44 rows. Every March
unit-script push site (`X2GUNPC.cpp` 14474-24765, `X2GUNPCAI.cpp:81`) is `(g_pKTDXApp, g_pX2Game, this)`, as the script assumes. No
receiver is typed wrongly. Two blind spots; neither adds a row:
- 18 reached scripts have no `june_src_fixed` source. 16 are bytecode in kom_contents, and a name scan finds only `GetIsRight` /
  `SetIsRight` / `GetRotateDegree` / `PlaySound_LUA` (Spriggan Small, Henir Three Way) and `CannotInput` (the 9 Battle Field
  tutorials), all already untyped-gap rows (C3z-4b batch 3). `altera_field_stage2.lua` and `test_ver2_field1.lua` are not on disk.
- Upvalue receivers (`uvN`) stay untyped. An indented nested `function` is read as part of its parent's body (4 Wally armor files;
  their one gap name is already a row).

**Bindings against June** (`disasm_range.py`):
- The five `CreateNPCReq*` wrappers pass `322e67c0` the same arguments in the same order as March's `CreateNPCReq`: (id, level,
  active, pos, right, delay, nodrop, keycode, team, AI, ally UID, elite, ally team -1, 0, 0, summoner templet).
  - `_LUA`: keycode -1, team 2, AI 0. `_LUA2`, `Elite` and `Ally`: delay `.x`, keycode = ftol `.y` (`32fe25f0`).
  - `Elite` takes elite from argument 8. `Ally` takes team from argument 8 and uses AI 1. `PushCreateNPCReq_Lua` takes the keycode
    from argument 8, with team 2 and AI 0.
- Ally owner: June's `CX2Game::CreateNPC` (`3230d2b0` @3230d3b4) looks it up only for AI 1 with ally UID != -1, through
  `GetUserUnitByUID` (vslot 0xa0). An NPC UID finds no user, as in March.
- Stored user: both accessors gate on `GUT_USER` (0) and use the observer at `+0x8d64`.
- `InitStateCoolTime`: June writes runtime row `+0` (36-byte rows), March's `m_fStateElapsedTime`, the first member. The index is
  `GetStateID`'s, and March's map value is the `m_vecStateData` position (`X2GUNPC.cpp:4796`).
- `GetIsLastStage`, `GetNowStateID_LUA` (states 0-14 same, 15-19 +1, arcade -1, 24->21, 25->22, 26->27) and
  `GetNPCUnitNumCheckTeam` (team `+0x278` == argument, no life test) match June.

**Sub-stage team** (`327eb8e0`: the XOR'd key decodes to `TEAM`, default 2 -> `+0x5c`; `327ef570`: AI = `+0x52` ? 4 : 0, no owner)
matches June. **Finding, fixed:**
- June's ctor `327af620` runs `IsAllyNPCID` (`327573e0`: uid != 0 -> type 1, team 0) before its AI switch, so an ally-id NPC
  sent as type 4 is built as a plain type 1. The port wrote 4 after the factory returned, which undid that.
- Affected: Santilus Ship's `NUI_MOTOR` (stage 0 sub-stage 1, every difficulty), Defence's `NUI_DEFENCE_CRYSTAL` /
  `NUI_GOD_GATE_ALLY`, and the Valentine box maker / cup cake killer. With the flag off, March's ctor made them NAT_ALLY. Every
  `== NAT_ALLY` reader skipped them, so this was a C3z-4 regression against both March and June.
- Fix (same flag `SERV_IRUHADEV_JUNE_SUBSTAGE_NPC_TEAM`): `CreateGUNPC` passes 4, and the ctor gets June's `case 4`: the ally AI
  with the caller's owner, then `SetAIType( 4 )` and `SetRenderLv( false )`, as type 1 does (@327b0943-327b095f).
  - Nothing reads the AI type during construction (the ctor tail, `CommonConstructor` 2205-2806), so Gateway Ignia is unchanged.
- **No June code distinguishes type 4.** All 50 June AI-type compares after a `+0x7c98` load test 0-3, and the register operands
  hold 0 or 1. The exe's three `cmp [r+0x38], 4` are not NPC AI: engine `31d4e5dd`, UI `32514c80`, and one misdecoded. Keeping 4
  is therefore June's behaviour: every `== NAT_ALLY` test skips it.

**Hygiene.**
- The fix, `flagcheck --worktree --files` (3 files): 2 PASS, 1 OTHER; RESULT PASS. The 5 deleted lines are the old post-factory
  `SetAIType` lines, inside the flag's own `#ifdef`.
  Post-commit, `--range 52dd9a1^..52dd9a1`: 2 PASS (X2GUNPC.cpp +13, X2DungeonSubStage.cpp +4 -5), 1 OTHER; RESULT PASS.
- C3z-4, `--range 6788262^..19a72fd`: every flag RESULT PASS, 0 FAIL. Batch-1 files show OTHER per flag because the flags share
  files, so they were checked as groups with `--with`:
  - all 10 flags: X2GUNPC.cpp/.h and X2Main.cpp PASS/PASS~ (only `//{{ //}}` markers outside);
  - the game flags: X2Game.cpp/.h 2 PASS, 2 PASS~;
  - `_SUBSTAGE_NPC_TEAM`: 4 PASS.
- `DEPLOY OK: X2_offline.exe is 19,552,768 B, dated 2026-10-11 05:42:19, md5 f30f41b1143f590c05237abdc5183c1b, equal to X2/US_SERVICE/x2.exe (2026-10-11 05:42:15); built at mods/june-buff-port 17a518e + 10 uncommitted code file(s).`
  The 10 files include a peer's C3z-5 WIP. The `substage CreateGUNPC` debug string is in the exe.
- `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` is **on** for one play-test; it goes off after. The fix is funcmap rows `327af620 @327b0943`,
  `3230d2b0 @3230d3b4` and `327573e0`. No JUNE_BUGS row, no open question.

### C3z-4 - review play-test (2026-10-11 05:57-06:05, reyaa unit 12): PASSED; `_C3Z4_DEBUG` off

- Santilus Ship Normal (33240): `substage CreateGUNPC npc 1390 team 2 ally 1 ai 1`. The motor stays type 1. `team 2` is the script's
  TEAM that the line prints; the ctor's `IsAllyNPCID` override still puts it on TN_RED. The room's summoners log
  `CreateNPCReq_LUA ... keycode=-1 team=2 ai=0`.
- Ranox Gateway Normal (33300): `npc 1897 team 0 ally 1 ai 4` at stages 0 and 1, unchanged. The user confirmed both checks.
- ErrorLog: 0 C3z-4 lines, 0 EXCEPTION. The run used a peer's deploy built from `e67eff2`, so it includes the fix.
- `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` is commented out (OFF note in `Always.h`). `DEPLOY OK: X2_offline.exe is 19,554,816 B, dated 2026-10-11 06:15:02, md5 ef4e77810987f7d2715a4131f47d6c52` (b41b0a1 + Always.h); the `JUNE_FX C3z-4` strings are gone from the exe.

### C3z-4b - Done (2026-10-11, Opus 5.5 high; 21 flags + `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` on for the play-test)

**Gate.** `c3z4_class_census.py`: 45 rows -> **18**. What is left: the `system` rows (tutorial guide 3 -> C3z-4t, SimpleDLG 8 -> C3z-4s), the 6 `june-bug` rows (JUNE_BUGS #169), and `PlaySound_LUA` (untyped-gap; its receiver is bound on CX2GameUnit, the tsv's expected leftover).

**Batch 2 (22 rows) + batch 3 (4 rows).** Each body is June's (addresses in the code comments and `Always.h`).
- **Bind-only, after checking June's members against March's:**
  - `IsActiveMonster`: June's 5 writers of +0x872c match March's 5 `m_bIsActiveMonster` writers.
  - `SetCameraDistance`: June's SetProjection writes the same 4 floats.
  - `GetUnitType` (GUUser).
  - `GetLastTouchLineIndex` on GUUser (slot 0x12c = March's override).
  - Batch 3: `GetIsRight` / `GetRotateDegree` on CX2GameUnit (vslots 0x170 / 0x150).
- **Ported:**
  - NPC: `GetNearestSiegeModeNpcUID`, `ChangeMotion_LUA`, `SetShowAllNPCExceptMe` (= `SetShowGage` + `SetShowObject`), `GetNowStateName`, `SetImmuneToExtraDamage` (June +0x4104 = `m_bImmuneToExtraDamage`, whose readers March has).
  - Game: `LiveKeyCodeNPCNumExceptDying`, `FadeOriginWorldColor_LUA`, `GetLiveUserUnitRandom` (vslot 0xbc, no override), and `GetIsGoNextPacketSended_LUA` / `Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ` (new `CX2DungeonGame::JuneSendGoNextReq` = June 327c9fa0).
  - Unit / user: `Play2DSound_LUA` (June's sound core with 3D off) and, in batch 3, `SetIsRight( bIsRight, bFuture )` on GUUser.
  - World, line map, effect: `GetWorldObjectMeshByName`, `GetCenterPositionLineIndex`, `SetEnableLinebyPos`, `CEffect::GetMainEffectPos`.
  - Batch 3: `CX2DungeonGame::CannotInput` (= the slide-shot body).
- **Henir:** `CanPassThisStage` / `GetHenirLimitLevel` are virtual, true / 0, with a CX2DungeonGame override (327cad60 / 327c9ff0). June's level-limit map is filled only by `HenirData:AddHenirLevelLimit`, and nothing in June's data calls it, so it always answers true / 0 (JUNE_BUGS #172). CX2HenirData is not ported.
- **Subtitle:** `SetMovieDescriptionData` stores the string ID and seconds on the now `CX2State`. `CX2State::RenderMovieDescription` (June 31f179b0) runs last in the dungeon-game and battle-field render passes:
  - `XUF_HEADLINE_30_NORMAL`, centred at (512, 620), white with a black shell.
  - It counts down by the state's elapsed time.
  - All 188 string IDs the scripts pass exist in the deployed `General.ess`.

**Readings used** (for the review):
- unit vslot 0x174 = `GetGameUnitState` (!= GUSI_DIE).
- `+0xc04 -> +0x20` = HP gage (`GetNowHp`; no gage data counts).
- `+0x8aa0` = `GetSiegeMode`.
- line `+0x18` = `bCanDown`, centre = (start + end) * 0.5.
- world object `+0x194 / +0x19c` = type / `m_wstrObjectName` (`SetXMesh_LUA` names it after the mesh file).
- `31e8a990` = `GetNowStateData` (the state-start replay in `ChangeMotion_LUA`).
- `LiveKeyCodeNPCNumExceptDying` is a new flag (`_GAME_KEYCODE_NPC_COUNT`) rather than the tsv's reuse of `_GAME_NPC_COUNT`, so batch 1's define stays insert-only.

**Data.** The 8 summon ids from the C3z-4 note were added by the C3z-2 recipe:
- data036 `Enum.lua`: June's values; no clash with the Lua table or the C++ `NPC_UNIT_ID` (`cl /P`, 2,023 members).
- data037 `NPCTemplet.lua`: June's 8 rows verbatim, in June's order (JUNE_BUGS #175 for the copy-paste rows).
- `npc_mesh_census.py --all-npc`: 0 files needed. `luasrc.py refresh` both; `ledger --check` / `core --check` PASS.
- `PACK OK data036` (~1 changed: Enum.lua), `PACK OK data037` (~1 changed: NPCTemplet.lua). Copy owed (user OK).

**Hygiene.**
- `flagcheck <flag> --worktree --with JUNE_C3Z4_DEBUG <the other 20> --files <21 code files>`: all 21 flags RESULT PASS, 0 FAIL.
- The only deleted line is the `C3Z4_DEBUG` toggle.

`DEPLOY OK: X2_offline.exe is 19,570,176 B, dated 2026-10-11 15:45:42, md5 a3cf19b767317fc5cd4460e33b4e7277, equal to X2/US_SERVICE/x2.exe (2026-10-11 15:45:36); built at mods/june-buff-port ec2b2df + 21 uncommitted code file(s).`
All 17 new method names and the `JUNE_FX C3z-4b` log strings are in the exe. The link at 15:45 was C3p's session (its build break in X2Game.cpp held this deploy up); `build.py` confirmed the bytes.

**JUNE_BUGS** #172 (Henir map empty), #173 (siege query keeps 32 bits of the UID), #174 (ChangeMotion always builds an after image), #175 (NPCTemplet copy-paste rows).

**Not play-testable here.** Elysion Sky Crossroads is not registered in March's DungeonData; that is the Atlas / Elysion plan, not started. Its Elysion scripts (laser / Maya / key objects) wait for that plan.

**After the commit.** `flagcheck <flag> --range 1ca6415^..1ca6415 --with JUNE_C3Z4_DEBUG <the other 20> --files <the 21 code files>`: all 21 flags RESULT PASS. `COPY OK data036: ... md5 a4824040b031ac30bfa7eb44330bbd25`, `COPY OK data037: ... md5 4f7c52457a9d3ef6bb6cb513b792259d` (user OK, 2026-10-11).

### C3z-4b - play-test 1 (2026-10-11 16:11-16:18, reyaa unit 12, exe `a3cf19b7`): partly reached

- Played Hamel secret 33261, 41271, Henir Space 38100 (stages 0, 22, 32-35) and Ranox Gateway twice.
- **Subtitle calls fire.** Gateway boss intro (event npc 1905), `offline_server.log:975-977`: `SetMovieDescriptionData npc 1905 string 33875 / 33876 / 33877`. Whether it rendered is the user's to confirm. The second Gateway run's intro was skipped (npc 1905 `KILL_SELF` about 1 s in; C3p's ESC skip), so it made no calls.
- **ErrorLog: 0 `can't find` lines** for the whole run, Henir stage 22 (Mad Behemoth Soul: `GetUnitType` / `GetMainEffectPos`) included. 0 EXCEPTION.
- **Not reached:** Final Hall 33320 (`ChangeMotion_LUA`, the GO_NEXT pair) and Frozen Water Hall 33180 (`SetShowAllNPCExceptMe`); both still owed.

### C3z-4b - play-test 2 (2026-10-11 16:24-16:31, reyaa unit 12, exe `a3cf19b7`): Final Hall PASSED

- **Ranox Final Hall Normal 33320**, `CLEARED in 242s` (`offline_server.log:2665`), 0 `can't find`, 0 EXCEPTION.
- **GO_NEXT:** stage 3 regeneration points advanced on their own, `Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ -> 1` x3 (1544 / 1682 / 1817).
- **Subtitles:** the boss intro (`EVENT_FINALHALL_BOSS` npc 1925) called `SetMovieDescriptionData` with 34176 / 34174 / 34175 (1842-1851).
- **ChangeMotion_LUA:** `npc 1924 motion Motion_FIRE_DEVIL_SKA_Change.x mesh 1 anim 1` (2069); Ska then fought on for 63 s and died to the user (2626).
- Still owed: Frozen Water Hall 33180 (`SetShowAllNPCExceptMe`) and the user's word that the subtitles render on screen.

**Play-test PASSED (user, 2026-10-11: "all works").** The subtitles render on screen (Gateway / Final Hall boss intros). Frozen Water Hall 33180 (`SetShowAllNPCExceptMe`) rests on the user's word: no run after 16:31 is in the logs. `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` stays on until the review, which turns it off.

### C3z-4b - Review (2026-10-11, Opus 5.5 high): one fix (GO_NEXT anti-hack gate); `_C3Z4_DEBUG` off

**Fix, under `SERV_IRUHADEV_JUNE_GAME_GO_NEXT`: script GO_NEXT reported the player as a hacker.**
- `JuneSendGoNextReq` calls March's `Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ`. Under `CHECK_SUB_STAGE_GO_NEXT` (`Always.h:3226`, unconditional), that handler treats `m_bCheckSubStageGoNext == false` as a hack (`X2DungeonGame.cpp`, handler body). It then sends `EGS_REPORT_HACK_USER_NOT`, sets `hackingUserType = HUT_AGREE_HACK_USER` (which suppresses later hack reports for the session) and calls `SendHackInfo3`.
- March's two inline senders (frame move, `SubStageOpen`) arm the flag first; the port did not.
- June's handler (vslot 0x280 = `327d0370`, raw disassembly) has no check: it sends `0xb4` with the now stage's byte `+8`, adds the `0xb5` wait and returns 1.
- The fix arms the flag before the call, as March's inline sites do. This is a port gap, not a June bug: no JUNE_BUGS row.
- Play-test 2's log is overwritten, so whether the report fired cannot be shown; the code path is unconditional.

**Checked and correct:**
- **Spot-checks in Ghidra:**
  - `ChangeMotion_LUA` (`32761cd0`, disassembly): order close / open "Bip01" / anim released twice / clear `+0x200` and `+0x210` / `3275c950` / state data `31e8a990` replay.
    The anim speed is `flag & 4 ? +0x37c : +0x8b60`. The after image is `new 0x1d4`, `(anim, 5, 0xe)` with `XL_EFFECT_0 == 14`, grey `3346efb0`, `AddMesh(+0x8600)`, built whenever the anim exists (#174).
  - `327c9fa0`: `+0xc44 -> +4 -> +8` into vslot 0x280, `+0xc4c = 1`, then `327f3b20` with the argument; callers `322f1550` (false) and the frame move `327d38e0`.
  - Subtitle `31f179b0`: font id 6, (0x200, 0x26c) = (512, 620), white, style 2 = `FS_SHELL`, black outline, line space 1, `DT_CENTER`, the render arg passed through.
    The countdown runs only when the argument is 0, clamped at `<= 0`. March's single false call per frame equals June.
- **GO_NEXT queries:** the `static_cast<CX2DungeonGame*>` is safe because only `CX2DungeonGame` sets `GT_DUNGEON` (BattleField and PvP set their own types). June's extra game-handle check (`+0x3c`) has no March equivalent.
- **Henir (#172):** no June `.lua` in any extracted `14JUN17` kom contains `AddHenirLevelLimit` (byte grep; control string `AddNPCTemplet` found). Its only xref is the binder `328b0940`, so the map is empty.
- **Data:** the 8 summon ids (1833, 1834, 1851, 1852, 1968, 2013, 2097, 2121) equal June in `Enum.lua` with no clash in the `NUI_` table. The 8 `NPCTemplet.lua` rows are byte-equal to `june_src_fixed`'s, in June's order.
- **Mapped readings** (`GUSI_DIE` = vslot 0x174, HP gage, siege `+0x8aa0`, `TN_MONSTER`, `bCanDown`, world object type / name) are taken as the Done note states. They were not re-derived, and the play-test exercised the counters without a miscount.

**Hygiene.**
- **Whole C3z-4b range:** `flagcheck <flag> --range 1ca6415^..HEAD --with JUNE_C3Z4_DEBUG <the other 20> --files <the 21 code files>`, run before the fix. All 21 flags `RESULT: PASS`, 0 FAIL, 0 ERROR.
  - The only deleted line is the `//#define SERV_IRUHADEV_JUNE_C3Z4_DEBUG` toggle in 1ca6415.
  - The OTHER rows are the sibling flags of this batch, plus peers' `365baf9` / `afefdb5` (`_JUNE_ATTACH_SKIN_MESH_ANIM`, `_GEYSER_SUREYNA_DEBUG`).
- **The fix:** `flagcheck SERV_IRUHADEV_JUNE_GAME_GO_NEXT --worktree --files X2Lib/X2DungeonGame.cpp KTDXLIB/Always.h`: X2DungeonGame.cpp PASS (+5), 0 deleted. Always.h OTHER (+2 -1: this review's `_C3Z4_DEBUG` OFF toggle). `RESULT: PASS`.

`DEPLOY OK: X2_offline.exe is 19,569,664 B, dated 2026-10-11 22:10:47, md5 a57a63e6de57163a99b5046a49cdc5d8, equal to X2/US_SERVICE/x2.exe (2026-10-11 22:10:42); built at mods/june-buff-port 727de03 + 2 uncommitted code file(s).` The `JUNE_FX C3z-4b` strings are gone from the exe (debug off).

**Play-test (review fix):** Ranox Final Hall 33320 once more. In `offline_packets.log`, `EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ` should be HANDLED (the stage 3 regeneration points advance on their own) and `EGS_REPORT_HACK_USER_NOT` should not appear anywhere.
**Review play-test PASSED (2026-10-12 03:52-03:58, exe `a57a63e6`, run after the deploy).** Final Hall 33320 `CLEARED in 220s` (`offline_server.log:1675`). `EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ` HANDLED x7 (`offline_packets.log:126-476`), each with its ACK and ALL_NOT. `REPORT_HACK` / `HACKSHIELD` lines: 0. 0 EXCEPTION; the only UNHANDLED is `E_DISABLE_HB_CHECK_REQ` x2, which this phase did not touch.

### C3z-4f - Done (2026-10-12, Opus 5.5 high): findings only, no client reader; nothing ported

**Verdict.** June's client writes the summoner field and never reads it, so it is an input to June's server only. No code changed: no
flag `#define`, no flagcheck, no build or deploy. The kickoff's step 3 has no branch for this case, so the port (below) is the user's call.

**The field (kickoff step 1).**
- June's `KNPCUnitReq` is `#pragma pack(1)`, **0x81 bytes**. That size is the stride of every `vector<KNPCUnitReq>` loop (`32130370`,
  `3230dcc0`, `32d53d40`) and the size of the embedded req in `320d5960`.
- The summoner value is a dword at **+0x7d**. It is the struct's last field: `+0x7d + 4 = 0x81`.
- The create core `322e67c0` stores its last argument there (`@322e6973` loads `[ESP+0x124]`, `@322e6995` stores it to struct +0x7d). The
  five wrappers pass the summoner's templet unit ID (`+0x85f8[0]`); every other caller passes 0.
- **Name: not recoverable.** The element Get `320ce400` jumps into VM-protected code (`33cb347a` -> `33854fb3`), and no reflection or
  log string names any `KNPCUnitReq` field. If it is ever ported, the name is ours: `m_iSummonerNPCID` (summoner templet NPC ID).
- **The 8 bytes before it** are two June-only ints. `AddNPCData 327edb60` copies `+0x75` to NPCData `+0x7c` and `+0x79` to `+0x80`. The
  Lua stage parser `327eb8e0` fills the same two NPCData slots from the XOR'd keys `QUEST_ID_FOR_CREATE` / `QUEST_ID_FOR_DO_NOT_CREATE`
  (`xorstrings.tsv:5402-5403`, decoded again here). Both default to 0.
  - **No shipped script sets either key.** There are 0 hits in two corpora: every `.lua` in June's own extract of all 258 koms
    (`14JUN17\data\*_kom`, where 376 files carry `NPC_ID` and 32 carry `SIEGE_MODE`, so the keys would show), and every `.lua` in
    `24965799\kom_contents`. This is a presence check; LuaJIT keeps string constants as plain bytes.
  - The quest-gated stage-NPC create is therefore dead in June's data. It is noted here and nothing more (skip-non-important rule).
    March lacks both keys.

**Reader sweep (kickoff step 2).** Every route out of the element Get was walked. The handler-side sweep was done by a Sonnet subagent;
the sink copy and both xref lists were spot-checked by hand.
- `320ce400` has exactly two callers: `320d5960`, a single embedded req, and `32130370`, the vector loop. `32138b00` has exactly two
  callers: `320e4830` and `320e4d90`.
- Handlers, and what each reads at element `+0x7d`:
  - `3230dcc0`, the CREATE_NOT handler: dispatch on +0x6b, then `CreateNPC 3230d2b0`. No read.
  - `3230e190`, 0x86-byte elements: no read.
  - `3230e3a0`, through `3230fa20` -> `320d5960`: reads the req plus the packet's +0x81 dword, not +0x7d.
  - `324b96a0` / `324b98b0` (dispatcher `324bf402` / `324bf343`) and `324ba090` (battlefield) all reach the sinks `32d53020` -> `32d528f0`
    and `32d53d40`.
    - The only copy out of the element is `32d555b0`, which copies field by field into a 0x68-byte node. Its highest read is +0x71
      (`@32d555f6`).
    - No 0x81-byte block copy anywhere.
  - `327edb60` (AddNPCData) and `32d539c0`: no read.
- Every one of the five June `CreateNPC` callers takes its arguments by value, and none of them includes +0x7d.

**Rows.** `june2017_re/funcmap.tsv` gains three C3z-4f rows: `322e67c0`, the +0x7d slot / `320ce400`, and the `QUEST_ID_FOR_*` NPCData
slots.

**User's choice (not done).** Port it for struct parity anyway. That is 4 small edits behind `SERV_IRUHADEV_JUNE_NPC_SUMMONER_FIELD`, which
lives in `Always.h` and also in `ServerDefine.h` plus `SECOND_HOMES`:
- the field and its `Init`;
- Put / Get;
- the five C3z-4 wrappers fill it;
- the emulator echoes it.

There would be no change in behaviour, since nothing reads it. Say the word and it becomes C3z-4f-2; otherwise the row stays closed.

### C3z-6 - Done (2026-10-10, Opus 5.5 high; 2 flags)

**`SERV_IRUHADEV_OFFLINE_RELATIVE_NPC_LEVEL`**: the March server's relative monster level, in the emulator (`Handlers_Room.cpp`).
- **Start** (`OnDungeonStartGame`, called at both dungeon start points):
  - The base is the party's average level, frozen at dungeon start (`DungeonRoom.cpp:780`): `GetAverageUserLevel(5)`
    (`RoomUserManager.cpp:6309-6352`) skips a member more than 5 below the highest and uses integer division.
  - Henir challenge takes 1 (`:787-798`; `SERV_HENIR_RENEWAL_2013` is off). An average of `<= 0` becomes `g_iMaxLevel`
    (`:842-852`, standing in for `GetLimitsLevel`).
  - **"Party" offline** is the player plus the AI bots: `MakeRoomSlots` puts them in the room's slots, so they are the server's
    `UT_GAME` users. `MakePartyBots` gives them the player's level, so the average is the player's level either way.
- **Stage load** (`RelativeMonsterLevelForStage`, `:3656-3712`):
  - `DT_SECRET` clamps into [`m_MinLevel`, `m_MaxLevel`], then adds +5 in `DM_SECRET_HELL`. An `ST_EXTRA` stage adds
    +5 + the extra-stage level instead.
  - A defence dungeon (`CASE_DEFENCE_DUNGEON`, ID without difficulty) clamps to the minimum only. Nothing is clamped after the +5.
  - The base applies only to a relative dungeon (`XSLDungeon.cpp:374-376`); the script's per-NPC `LEVEL` is still added.
  - **Mode offline:** the room's `m_cDungeonMode`, as sent. The local map and party manager send `DM_SECRET_HELL` (4) for every
    secret dungeon under `SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS`.
- **Extra-stage level** comes from server Lua `SecretDungeonData.lua` (`SetSecretDungeonExtraStageNpcLevel( 2 )`, line 6).
  - It is loaded the shipped way, borrowing `g_pDungeonManager` and handing it back. Its other two calls are bound and unused
    (extra-stage entry odds; offline the client's secret-stage pad decides).
  - **Not packed yet:** the cited live value 2 stands in, and the log says so.
  - All 5 Secret Hell scripts have an `ST_EXTRA` stage, so this branch is live.
- **Was** (`#else`): `m_MaxLevel`, else `m_MinLevel`. That was 99 for June's 70-99 Secret Hells.
- **Expected:** Freils (unit 18, level 72) in Elder Secret Hell: base 77 (72 + 5), 79 on the extra stage. A level-80 character
  gets 85 / 87.

**`SERV_IRUHADEV_OFFLINE_SECRET_POISON`**: the server's poison timer.
- `m_fPoisonTimeLimit` / `m_fExtraStagePoisonTimeLimit` are read into the client's `DungeonData` (`X2Dungeon.h`,
  `X2DungeonManager.cpp`; the server's reads are `XSLDungeonManager.cpp:173-174`).
- The emulator sends:
  - `EGS_DUNGEON_EFFECT_REMAINING_TIME_NOT` after `EGS_PLAY_START_NOT` (`:1072-1078`);
  - `EGS_DUNGEON_EFFECT_TIME_OUT_NOT` (`DDT_POISON`) once, on the 3 s play-status beat, after the limit (`:167-186`);
  - the extra limit on Altera's first `ST_EXTRA` load (`:4147-4166`).
- **Only Altera Secret Hell sets one:** 720 s, extra stage 900 s. The client applies the poison to Altera ids only
  (`X2DungeonGame.cpp:2497-2519`).
- **Deviation:** `:4156-4157` (the play-time limit reset) is not mirrored on `m_dwPlayStartTick`. Offline's limit is 19999 s,
  and that tick also times the result screen. The poison clock restarts instead.

**Server-only keys, verdicts** (`june2017_re/dungeon_registry_census.tsv` lists none of them as a June client read):
- `requireLevel`: the March server's reader is commented out (`XSLDungeonManager.cpp:107`). Only 4 test rows set it, all 0.
  Nothing.
- `m_bForbidEnterDungeon` (the 4 `*_SECRET_COMMON` rows, unreachable `UNUSED_DUNGEON_ID`s) and `m_bArcadeMode` (3 arcade rows):
  no March server reader exists, and June's server is not available. Nothing.
- `m_bSwitchingWithEventInfo` (KidDay x4, Tree Day x6). Live, it gates entry by the event schedule (`GSUserParty.cpp:1094`,
  `GSUserFunction.cpp:10480` / `10810`) and drives `EGS_UPDATE_DUNGEON_STATUS_NOT` (`:17940`). The emulator sends no status
  packet, so the client (`IsDungeonEnable`) treats them as enabled. **Not ported, open question below.**
- `DUNGEON_CHECK_CONDITION`, server half: March's server reads no `DCC_*`.
  - The 13 `DCC_EVENT_DUNGEON` rows are exactly March's hard-coded `CXSLDungeon::IsEventDungeon` list (`XSLDungeon.cpp:1103-1135`:
    KidDay 4, Tree Day 6, Defence 3), so June moved a March ID list into data.
  - Per member:
    - `DCC_EVENT_DUNGEON` = `IsEventDungeon` by ID. Its consumers are spirit (`UserSpiritManager.cpp:82`) and the stat
      "revision" (`GSUserFunction.cpp:11698`, `12848`). The emulator does not mirror the stat revision, and that gap
      predates June (open question 2).
    - `DCC_NOT_DECREASE_SPIRIT`: offline spirit is pinned full (`FillSpirit`), so there is nothing to do.
    - `DCC_ONLY_GET_EL_EXP`: no March reader. Its nearest March behaviour is the `bRevisionDungeon == false` gate on the
      socket-option / title EXP-ED bonus (`GSUserFunction.cpp:11706-11727`), and the emulator applies no such bonus at all
      (no reader in `X2Lib/Offline/`). Nothing to gate.
    - `DCC_NOT_USE_QUICK_SLOT`, `DCC_NOT_SUMMON_RIDING_PET` (Henir): the client half, C3z-5.

**Hygiene** (commit `6a06565`, `--range 6a06565^..6a06565`; the compound `#if defined(A) || defined(B)` blocks need `--with`):
- `RELATIVE_NPC_LEVEL --with SECRET_POISON`: 0 PASS, 2 PASS~, 1 DEFINE, 1 OTHER (Handlers_Field's block is POISON), 0 FAIL,
  0 ERROR. RESULT: PASS.
  - This run leaves out `X2Dungeon.h` / `X2DungeonManager.cpp`: they hold POISON-only changes, and their existing
    `#if defined(X2TOOL) || defined(SERV_IRUHADEV_OFFLINE_NPC_GROUP_RATE)` is a line flagcheck cannot evaluate.
- `SECRET_POISON --with RELATIVE_NPC_LEVEL` (all 6 code files): 2 PASS, 3 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR.
  RESULT: PASS.
- Both runs: 0 deleted lines, 0 non-ASCII, 0 untokened `#else` / `#endif`, 0 encoding changes. `X2Dungeon.h` and
  `X2DungeonManager.cpp` were edited with bytepatch (CP949).
- `DEPLOY OK: X2_offline.exe is 19,551,232 B, dated 2026-10-10 23:22:04, md5 f29f14935e30771ee21f0a17645a8577` (built at
  `3e66b8a` + this phase's files; it includes C3t). The `JUNE_FX C3z-6` and `scales monsters to the party: base` strings
  are in the exe.

**Open questions (user):**
1. Event dungeons (KidDay, Tree Day) with no event running. Live, they are refused (`ERR_PARTY_22`). Offline they stay
   enterable, as before this phase. Port the gate (they become unenterable offline), or keep them open?
2. The live server plays the 13 event / defence dungeons with "revised" stats (`CalculateDungeonUnitStat( bRevisionDungeon )`,
   `GSUserFunction.cpp:11698-11704`, `12848`). Offline they run at full stats; that has been true since the emulator existed.
   Plan a phase to port it, or leave it?

**Answers (user, 2026-10-11): "skip any non important stuff for now".** Both are left as they are: event dungeons stay open
offline, and no stat-revision phase is planned.

### C3z-6 - play-test (2026-10-10 23:29-23:33, Freils unit 18, level 72, 3 bots): PASSED

- Elder Secret Hell (33031, mode 4):
  - `start: average level 72 (4 of 4 member(s) counted, mode 4)`
  - `scales monsters to the party: base 77`, band 70-99. The user confirmed the monster levels.
- Altera Secret Hell (33051, mode 4):
  - base 77;
  - `poison countdown 720s` sent after play start.
- 0 EXCEPTION. The 4 UNHANDLED (`E_DISABLE_HB_CHECK_REQ`, `EGS_SKIP_CONTINUE_DUNGEON_NOT`) are not this phase's.
- Not exercised:
  - an `ST_EXTRA` stage (base 79, the `SecretDungeonData.lua` note);
  - the poison timeout itself (12 min);
  - the extra-stage poison restart.

### C3z-5 - Done (2026-10-11, Opus 5.5 high; 5 flags)

**Gate:** every key has a ported reader or a routing row, per key in `june2017_re/c3z5_key_readers.tsv` (June offset,
default, accessor, every June reader, the March counterpart, verdict).
- June's readers come from June's own accessors (`327da980`, `327da920`, `327da9f0`, `327db250`, `327d9df0`), a dataflow scan
  from `GetDungeonData` (`327da150`, 54 callers), and the 23 callers of g_pData `31edf8f0`. That function is the only path to
  the DCC bitset: `327e6d20` has one caller.
- Earlier ports had left "June-only DungeonData field, omitted" comments at these sites. Those comments now sit above the
  ported code; they are kept, because edits are insert-only.

**Loader + members** (`X2Dungeon.h`, `X2DungeonManager.cpp`; June `327e0f80`). Defaults read from the disassembly:
- MinAdequateMemberCount / MaxAdequateMemberCount: -1.
- FixMaximumDifficulty: false.
- m_SuitableLevelFixValue: 2. m_iRequireCompleteQuestID: 0.
- m_HardRequireItemLevel: -1. m_HardMinLevel / m_HardMaxLevel: 0.
- DUNGEON_CHECK_CONDITION: a dword, cleared, then bit N for each listed 0 <= N < 15.

**The five flags:**
- **`SERV_IRUHADEV_JUNE_DUNGEON_ADEQUATE_MEMBERS`:** `GetAdequateMemberCount` (327da980). The party UI's `need_user` becomes
  "min" or "min~max", red while the party is below the minimum, else green (`322be5c0` @322be93d; no party counts as 1).
- **`SERV_IRUHADEV_JUNE_DUNGEON_FIX_MAX_DIFFICULTY`:** `GetMaxDifficulty` / `IsFixMaximumDifficulty` / `GetDefaultDifficulty`
  (327da920 / 327da9f0 / 327db250). Its consumers:
  - UpdatePartyDLG switches the party to the top difficulty, both before the sum and after the Ruben fallback
    (`322c3078`, `322c312d`);
  - UpdateDifficultyButton hides NORMAL / HARD / EXPERT (`322b6600`);
  - the local-map click starts at `GetDefaultDifficulty` (`3293d6d6` / `3293d717`);
  - MakeDungeonNameString drops the difficulty suffix (`327ddb60`).
- **`SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL`:** two consumers.
  - IsActiveDungeon (`327da190`) uses `m_MinLevel - m_SuitableLevelFixValue` instead of `- 2`, and also needs
    `m_iRequireCompleteQuestID` (GetUnitCompleteQuest). Its other branches are commented in place: the local-map templet
    quest (+0x70), the Atlas event keys 2260 / 2261, and Hero / Bestera.
  - The party-list join check (`322ca040`) uses the same slack.
- **`SERV_IRUHADEV_JUNE_HENIR_HARD`:** in Henir challenge, the item level comes from +0x64 (UpdatePartyDLG `322c31d9` and
  SelectDungeonDifficulty `322c1826`). At a non-normal difficulty, need_level shows +0x94 / +0x98 (UpdateAdequateLevel
  `322beb90`).
- **`SERV_IRUHADEV_JUNE_DUNGEON_CHECK_CONDITION`:** `IsDungeonCheckCondition` (327d9df0). DCC 11 (NOT_USE_QUICK_SLOT)
  replaces the KidDay ID list in the dungeon-game state ctor (`324d9f80` @324dac97; the list is kept in `#else`), and it is
  added after the Henir test in SetShowUI (`32154820` @32154ced).

**Visible now:**
- 50 Expert rows set FixMaximumDifficulty, from Ruben's Ruins of Elf / Swamp through Ranox, so those dungeons play at Expert
  only: no difficulty boxes, no suffix, and "1~4" recommended.
- Ruben's El Tree sets FixMaximumDifficulty on its normal-only row, so it stays Normal and shows "1".
- The 4 Secret Hells (m_SuitableLevelFixValue 0) open at m_MinLevel exactly (70), not 68.
- KidDay already hid the quick slot, so that is unchanged.
- **DCC 11 can't be play-tested offline.** No local-map or location lua lists a KidDay dungeon, and no dev-console command
  enters a dungeon by ID. Its proof is the exe strings and the flagcheck lines.

**Routed to C3z-5b** (kickoff `C3z-5b.md`):
- DCC 8 (riding pet; live on Henir, 5 sites);
- DCC 2, whose lists equal March's IsEventDungeon today, so nothing changes yet;
- DCC 1 / 9 / 10 / 12 / 13 / 14 and `322c4708` (June-only rows only);
- June's GetLastClearDungeonID (`32350560`) and the UpdatePartyDLG fallback;
- `m_fExtraValue` (a Henir-practice NPC stat scale in `32384d10`, stat not identified);
- additionalTicket, m_bShowNpcKillCount, m_bAprilFoolsDungeon, m_bNoDarkBackGroundBy60S.

DCC 0 and 3-7 have no client reader (C3z-6's server-half verdicts).

**Hygiene** (worktree, `--files` the 7 code files, each flag `--with` the other four):
- FIX_MAX_DIFFICULTY: 4 PASS, 1 PASS~, 2 OTHER (the DCC flag's files), 0 FAIL, 0 ERROR; RESULT: PASS.
- CHECK_CONDITION: 4 PASS, 1 PASS~, 2 OTHER (X2LocalMapUI = FIX_MAX; X2PartyUI = the other new flags), 0 FAIL; RESULT: PASS.
- Both: 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 untokened `#else` / `#endif`, 0 encoding changes. Every file was
  edited with bytepatch (CP949).
- Post-commit, `--range da63fb2^..da63fb2`, the same `--with` sets. FIX_MAX_DIFFICULTY and CHECK_CONDITION each give 4 PASS,
  1 PASS~, 1 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR, 0 deleted lines, 0 encoding changes; RESULT: PASS.

**Deploy:**
- `DEPLOY OK: X2_offline.exe is 19,556,352 B, dated 2026-10-11 05:53:10, md5 1c28ab8727eb2aeec0bdf7fd85cb9eb1` (built at
  `e67eff2` + this phase's files; `--pch force`).
- The first deploy (md5 f30f41b1) linked a peer's build that had a stale PCH, and none of the new strings were in it.
- The strings `JUNE_FX C3z-5 need_user`, `JUNE_FX C3z-5 party dungeon`, `FixMaximumDifficulty`, `DUNGEON_CHECK_CONDITION`
  and `m_SuitableLevelFixValue` are in the exe.

**Log:** `logs.py --grep "JUNE_FX C3z-5"`.
- `need_user 1~4 for dungeon N (party of K, green)` is the reliable line.
- `party dungeon N fixed to its top difficulty 2 (was 0)` appears only when the stored party data held a lower difficulty.
  For example, the first party-window open with a dungeon kept from before this build. A local-map pick already starts at
  Expert (GetDefaultDifficulty), so it logs nothing.

### C3z-5 - play-test (2026-10-11 05:57-06:05, reyaa unit 12, lv 80): PASSED

The user confirmed the checks, and the logs agree.
- The run used the deployed exe (md5 1c28ab87).
- 0 EXCEPTION. The 2 UNHANDLED are `E_DISABLE_HB_CHECK_REQ`, not this phase's.
- `party dungeon 33300 fixed to its top difficulty 2 (was 0)`: the stored Gateway pick was raised to Expert.
- 32 `need_user` lines across Ruben, Elder, Sander and Ranox:
  - `1~4` green on every Expert row (e.g. 30082, 30142-30212, 33202-33312);
  - `1` green on El Tree (30000).
- Both dungeon starts were Expert, from Quick Start with no difficulty boxes: `dungeonID=33240 dif=2` (Santilus Ship) and
  `dungeonID=33300 dif=2` (Gateway).
- Henir challenge (90-99, item level 140) was the user's visual check; no log line covers it.
- DCC 11 was not exercised (it can't be reached offline; see the Done note).

### C3z-7 - skipping a boss intro cinematic (user, 2026-10-10)

**Status: planned. Model:** Opus 5.5, high. Kickoff `kickoffs/june-effect/C3z-7.md`. The user asked for June's boss-intro skip.
It is not the event-scene talk skip (March has ESC at `X2EventScene.cpp:336`), not the epic quest scene skip
(`DLG_Skip_Epic_Scene_Guide`, `UQNCM_SKIP_EPIC_QUEST`), and not the dungeon clear movie (`DLG_Dungeon_Clear_Movie`).

**What a boss intro is (June data).**
- An event NPC spawned in the boss room (`NPC_GROUP`): 26 June `EVENT_*` scripts in data037 have the shape, e.g. `EVENT_GATEWAY_BOSS`
  (Ranox Gateway stage 3), `Event_NPC_Battle_Air_Ship` (Altera Battle Air Ship), `EVENT_FINALHALL_BOSS`, `EVENT_GAYSER_BOSS`.
- `INIT_AI NO_BRAIN`. Its `START` state plays the cutscene animation with `FOCUS_CAMERA_AT_CURRENT_STATE`, `STOP_AT_CURRENT_STATE`,
  a parts camera (`CT_PARTS_LOOK_DIRECT`), a cutscene sound and `SetMovieDescriptionData` subtitles (C3z-4b).
- `WAIT_STATE` is its `DYING` state, whose start function puts the real boss in its start state (`FIRE_DEVIL_IGNIA_START`) and moves
  the event NPC away. So "skip" most likely means: end `START` early and run `DYING`.

**Leads in June (not yet proven).**
- No June string names a boss-intro skip. `BG_ESC_SKIP` (`dlg_ui_button20.tga`) is used by `DLG_EventScene` and
  `DLG_Dungeon_Clear_Movie` only, and `IsSkipKeyPressed` (CX2Game, `@31dc90c7`) only by tutorial / 2017 event / Elysion gate scripts.
- State keys: `FORCE_CINEMATIC_STATE` (`327a0fa0 @327ab496`), `IS_APPLY_CINEMATIC_UI` (`32784c00 @32789ecf`).
  Binds: `SetApplyCinematicUI` (CX2GUNPC, `31e4d4d0`), `CannotInput` (CX2DungeonGame, `327d20b0`), `PlayWorldCamera` /
  `EndWorldCamera` (CX2Main `31d828d0` / `31d82970`, CX2Game `322cbe70`).
- To trace:
  - the readers of the NPC state flags `STOP_AT_CURRENT_STATE` / `FOCUS_CAMERA_AT_CURRENT_STATE` (March `X2GUNPC.cpp:5736-5741`);
  - June's dungeon-game key reads (`DIK_ESCAPE` / space / Z) and what they change while such a state runs;
  - June's CinematicUI (March `CX2Game::CinematicUI`, `X2Game.h:332`, `MODIFY_DUNGEON_STAGING`).
- March: no skip of any kind for these intros (no skip reader in `X2DungeonGame.cpp` / `X2StateDungeonGame.cpp` / `X2GUNPC.cpp`).

**Rules for the phase.** June's path, cited; if June has none, stop and propose a stand-in (design-missing-june-pieces), never
build one unasked. Test only where the event NPC actually spawns (`NPC_GROUP`, not `READY_NPC`).

## C4 - 8b, damage math

**Status: done 2026-10-07 (see "C4 - Done" and "C4 - Review" below; Fury mode found there -> C4b/C4c). Model:** Sonnet 5, high, then an Opus review. Needs C2's `GetFinalDamage` and
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

### C4 - Done (2026-10-07, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_DAMAGE_MATH`, plus `SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY` extended)

**What landed** (commit `c1c74e2` (code) and the follow-up commit (the `BeginTable` arity below, docs`; details in `MODS.md`, "C4: damage math"). C2's decisions applied: the `IGNORE_DEFENCE*` keys were already June buff port Phase 5c's (parse + both DamageReacts + `GetFinalDamage` block `3228568a`), `DAMAGE_SKILL_ID` is C1's, and `DIVISION_NUM` / `DIVISION_OFF_TIME` are buff-apply gating, so they follow the user's 2026-10-06 decision and sit under `SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY`, not `_DAMAGE_MATH`.
- **`DIVIDE_DAMAGE_FROM_RANGE`** (`_DAMAGE_MATH`). Parse in `SetDamageDataFromLUA` (`32632390 @3263575f-32635b0f`): `DIVIDE_DAMAGE_FROM_RANGE = { RANGE0 = { RANGE, DAMAGE_RATE }, RANGE1 = ... }`. The sub-tables are named `"%s%d"` (`31c7b980`) from index **0** and the loop ends at the first missing one; `RANGE` defaults to 99999, `DAMAGE_RATE` to 1.0; the map insert keeps the first of two equal `RANGE`s (`3261a150`). Consumer `CEffect::JuneApplyDivideDamageFromRange` (= `32622780`), called in `GetFinalDamage` between the two damage locals and `ModifyFinalDamageByGameUnitType`, as June's `@322857f6` sits before the attacker's hook; it changes the locals only, never `damage`. dist = effect position (`m_Pos`) to the defender's aim point (`JuneGetLockOnAimPos`, June slot `0x3e4`; user = `GetPos`, base = the zero vector); rate = value of the last `RANGE` not above dist (default 1.0); physic and magic are scaled only for rate **< 1.0**.
- **`CAN_ACTIVATE_NASOD_ARMOR_COMBO`** (`_DAMAGE_MATH`). Parse as a bool (default false). Gate in `CX2GUAdd::SetNasodArmorAttackTarget` (`329f63a0 @329f64a5`): C2's open "local" is a switch at the function top on `USED_SKILL_ID` over 29201 / 29202 / 29216 / 29217 (June reform `RNSI_A_AN_PHASE_SHIFT` / `_PULSE_BULLET` and their `_UPGRADE`). This build has no reform IDs, so the two March skills of those names pass instead; the rest goes to checklist 7d. **User decision (2026-10-07): follow June and accept that Add's other hits set no Nasod armor target until DamageEffect data is adopted** (March data has 0 uses of the key, June's has 52, all Add's). One-shot log `JUNE_FX NASOD_COMBO gate (first seen)`.
- **DIVISION window** (`_BUFF_REACT_APPLY`). `CheckDamageDataKeyName` (a stub from buff Phase 10, wrongly named) became `JuneCheckDivisionWindow` (`3227a960`): with `DIVISION_NUM >= 1` the unit keeps a map (attacker UID, `DIVISION_NUM`) -> seconds; a present key refuses the factor, an absent one is inserted with `DIVISION_OFF_TIME`. Tick `JuneTickDivisionWindow` (`322749f0`) at the end of `CX2GameUnit::OnFrameMove`, re-arm `JuneRearmDivisionWindow` (`32275da0`) from the end of `SetDamageDataFromLUA` through `32622ad0`'s unit walk (`322ccf50` is only "pointer is in the unit list", so the port iterates `GetUnitList()` directly). Parse: `DIVISION_NUM` int default 0, `DIVISION_OFF_TIME` float default 0.
- **x87 polarities settled** (harness `c4_x87.py`, same technique as `x87test.py`; funcmap rows `Effect C4`): the walk stops at the first `RANGE` with dist < `RANGE` (NaN takes every entry), the multiply needs rate < 1.0 strictly (1.0, above 1.0 and NaN do nothing), the tick erases at time <= 0 (NaN keeps). JUNE_BUGS 87 (DIVISION window shut by a later-rejected factor / the first factor of a hit) and 88 (first duplicate `RANGE` wins, rate >= 1.0 ignored).
- **June-only items routed, not ported:** the hard-coded `USED_SKILL_ID == 30205` (Luciel, `RNSI_A_LLC_BACKSTAB`) force of `IGNORE_DEFENCE*` in both DamageReacts (`@320483e4`, `@327b33cf`) and the four reform IDs -> checklist 7d. Found, not owned: an unidentified NPC-attacker block in `GetFinalDamage @32285757-322857e8`, just before the DIVIDE call (funcmap, status `candidate`; needs Opus identification).

**Hygiene.** `flagcheck JUNE_DAMAGE_MATH --worktree --files <the 8 code files>`:
```
files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS        (--range c1c74e2^..c1c74e2; the 4 OTHER rows are the REACT_APPLY lines in X2DamageManager.*/X2GameUnit.*)
```
`flagcheck JUNE_BUFF_REACT_APPLY` on the same files: `RESULT: PASS`, 1 deleted line in `X2GameUnit.h` (the renamed declaration) and 8 in `X2GameUnit.cpp` (the old `CheckDamageDataKeyName` stub, replaced by the real functions; both are buff Phase 10's own flag lines, an OTHER row).

**Deploy:** ``DEPLOY OK: X2_offline.exe is 19,307,008 B, dated 2026-10-07 15:08:32, md5 ca2f1a7e98760f0bbff0c623e8e54e70, equal to X2/US_SERVICE/x2.exe (2026-10-07 15:08:27); built at mods/june-buff-port c1c74e2 + 7 uncommitted code file(s).` (the 7 are a peer's C9c-2b files plus this follow-up). The first build failed on a stale PCH (a header edit landed inside another session's PCH compile) and on a missing `Offline/X2OfflineLog.h` include in `X2GUAdd.cpp`; `--pch force` fixed the first. All four log strings and the four key names are in the deployed exe (narrow and UTF-16).`
**Census** (`census.py`): `census.py` after the follow-up: `CAN_ACTIVATE_NASOD_ARMOR_COMBO`, `DIVISION_NUM`, `DIVISION_OFF_TIME`, `DIVIDE_DAMAGE_FROM_RANGE` and `RANGE%d` are all `implemented` (the last two only after `BeginTable( "DIVIDE_DAMAGE_FROM_RANGE" )` / `BeginTable( "RANGE", i )` replaced my first `, false` spelling: the census reads a 2nd argument as an index, and the log flag is unused in `KLuaManagerProxy`); no row is open for C4. Still open: C3 19, C10 6, C11 3. The one FAIL line is C10's HP-rate item, unchanged. Matrix and `census.tsv` regenerated.

**Exit.** The plan's exit asked for before/after damage numbers on one ignore-defence skill and one division skill. They cannot be recorded: no June or March script uses `DIVISION_NUM`, `DIVISION_OFF_TIME`, `DIVIDE_DAMAGE_FROM_RANGE` or `RANGE%d`, and the ignore-defence keys did not change (buff 5c). The play-test is therefore a regression check (damage numbers unchanged in a dungeon) plus the Nasod gate line for an Add.

**How to see it in game:** nothing changes for existing data, so the check is a regression one. Play a dungeon with any character (NWatcher, Night Watcher, level 70, is the one used for the effect tests): damage numbers and kills behave as before, no crash. The only visible change is Add's Nasod armor targeting: an Add (create one with `/x2-create-char AddTest 32 40`, Psychic Tracer) logs `JUNE_FX NASOD_COMBO gate (first seen) ...` in `offline_server.log` on the first hit and no longer sets the Nasod armor target on normal hits. No line for DIVISION or `DIVIDE_DAMAGE_FROM_RANGE` is expected: no data uses them. Log tag `JUNE_FX` (`python tools/logs.py --tag JUNE_FX`).

**Open for the review:** (1) the Nasod gate's March-skill mapping; (2) the re-arm's list walk; (3) the unidentified NPC block before DIVIDE.

### C4 - Review (2026-10-07, Opus 5.5 high)

**Verdict: correct against June, with one init deviation fixed** (flag `SERV_IRUHADEV_JUNE_DAMAGE_MATH`, `X2DamageManager.h` only).
Checked in Ghidra (read-only) and in the exe bytes:
- **DIVIDE parse** (`32632390 @3263575f-32635b0f`): March's `KLuaManager::BeginTable( name, i )` formats `"%s%d"`, so `RANGE0`..; defaults read from the exe: `33441c60` = 99999.0, `FLD1`; insert `3261a150` is the parse's only caller (`@32635ae5`).
- **Consumer `32622780`**: empty-map / game / damage-effect-manager / defender / `GetInstance( +0x14, live )` early-outs; dist from `CEffect +0x100` to vslot `0x3e4` (empty name) = `JuneGetLockOnAimPos`; walk `TEST AH,5 / JNP` = stop at the first `dist < RANGE` (NaN takes all); multiply `FCOM 1.0 (334355b0) / JP` = rate < 1.0 only; both locals by the same rate. The call site `@322857f6` follows the locals' load and June's Fury block and precedes the attacker hook (`+0x3ac`/`+0x3b0`), as ported. The definition is already inside `_DAMAGEEFFECT_RUNTIME` (`X2DamageEffect.cpp:8647`), so its guard matches the declaration's.
- **PvE / PvP:** none of the three consumers (`32622780`, `329f63a0`'s gate, `3227a960`) tests the game type; the only PvP split is the ignore-defence rates (buff 5c, unchanged by C4).
- **Nasod gate** (`329f63a0`): top switch on `+0x590` over `0x7211/0x7212/0x7220/0x7221`, gate `+0x570 != 1 && byte != 1` -> function end. March's body is all inside its `if`, so the early `return` is equivalent. Judgement (1) confirmed (the March skills stand in for the reform IDs); (2) is the user's 2026-10-07 decision; June's body after the gate is rewritten (memo swap `328905b0`, FantasyTracer on the switch byte, new default targeting): Add's class code -> checklist 7d row (added).
- **DIVISION:** `3227a960` (null / `< 1` / no live attacker -> true; find -> false; insert `+0x61c` -> true), tick `322749f0` (erase at `<= 0`, NaN kept; `CX2GameUnit::OnFrameMove` has one return in both builds, June ticks `@3229fab5` near the end), re-arm `32275da0`, loop `32622ad0` and its call `@32640608` (args: attacker UID via vslot `0x54`, `*[esp+0xbc]` = `+0x618`, `*[esp+0xb8]` = `+0x61c`; only when `31e55680` says the attacker is live). Judgement (3) confirmed: the body changes no list, so iterating `GetUnitList()` directly is equivalent.
- **Fixed: DamageData reset.** June's `SimpleInit` (`31f8ee50`) resets `+0x618/+0x61c` (as ported) but not `+0x570` or the range map, `Init` (`31f8f550`) neither, and the ctor (`31f8f620`) builds the map empty and never writes `+0x570`. C4 had put `= false` and `.clear()` in `SimpleInit`. Now: nothing in `SimpleInit`; the ctor sets the bool false (stand-in for June's undefined byte; every parse rewrites it, default false `@3263575b`). JUNE_BUGS 91. Funcmap's JUNE_BUGS citations corrected (79 -> 87, 80 -> 88).

**Found, not owned -> new phases C4b (identify, Opus) + C4c (port, Sonnet).** The NPC block at `GetFinalDamage @32285757-322857e8` is June's raid-boss **Fury mode**: attacker NPC (`dynamic_cast<CX2GUNPC*>`, type descriptors read from the exe), NPC byte `+0x8fb7` == 1, current state `FURY_MODE` (bitset bit 80, `322afe40`), rate = StateData `+0x414` = `ATTACK_POWER_RATE_FURY` (parse `32784c00 @327963cb`), each damage local multiplied when nonzero. March has no Fury key; June uses it in raid scripts only. Not a DamageData key and not per-class, so neither C4's nor a character port's: per the standing rule it gets its own phases (kickoffs `C4b.md`, `C4c.md`). Funcmap row now `confirmed` + 5 rows (`Effect C4 review`).

**Hygiene.** `flagcheck --range c1c74e2^..HEAD` before the fix: `_DAMAGE_MATH` `files: 4 PASS, 0 PASS~, 1 DEFINE, 10 OTHER, 0 FAIL, 0 ERROR`, `2 deleted line(s)` (C4's own `BeginTable` respelling), `RESULT: PASS`; `_BUFF_REACT_APPLY` `files: 1 PASS, 0 PASS~, 0 DEFINE, 14 OTHER, 0 FAIL, 0 ERROR`, `1 deleted line(s)` (in `X2GameUnit.h`, the renamed declaration; the `X2GameUnit.cpp` -8 is buff Phase 10's own stub and call site), `RESULT: PASS`. OTHER rows: each flag's lines in the other flag's files, plus the peer's C9c-2b commit `c71621a` inside the range. The fix (`--worktree --files X2Lib/X2DamageManager.h`): `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `2 deleted line(s)` (the two `SimpleInit` resets C4 itself added), `RESULT: PASS`. After the commit (`540186c`), `--range c1c74e2^..HEAD`: `files: 5 PASS, 0 PASS~, 1 DEFINE, 29 OTHER, 0 FAIL, 0 ERROR`, `4 deleted line(s)` (the 2 `BeginTable` respellings + the 2 resets), `RESULT: PASS` (the extra OTHER rows are the peer's 14c commits `6c9b694`/`d5f442f` in the range).
**Census:** all ten C4 rows (`IGNORE_DEFENCE*`, the Nasod key, `DIVISION_*`, `DIVIDE_DAMAGE_FROM_RANGE`, `RANGE%d`) `implemented`; `census.tsv` unchanged.
**Deploy** (`--pch force`: a peer build had compiled the PCH 72 s after the header edit): `DEPLOY OK: X2_offline.exe is 19,311,616 B, dated 2026-10-07 15:52:52, md5 006df66ef6e6d81a204f6f6580f8e624, equal to X2/US_SERVICE/x2.exe (2026-10-07 15:52:47); built at mods/june-buff-port d5f442f + 1 uncommitted code file(s).` (the one file is this fix).
**In game:** nothing visible (no data uses the range or DIVISION keys; the Nasod gate is unchanged). No new play-test: C4's regression play-test stands. C10b-1 is unblocked. JUNE_BUGS rows 87-88 were numbered after a peer's uncommitted 79-86.

### C4b - Done (2026-10-07, Opus 5.5 high; identification only, no code)

**June's NPC Fury mode, end to end.** All of it is in `june2017_re/funcmap.tsv`, 14 rows tagged `Effect C4b`. No row of the system is left at `candidate`. Read-only Ghidra, capstone sweeps (`dispsweep.py`, `funcdis.py`, `vtslot.py`) and an `offsetprobe.py` sizeof probe.
- **Settled first.**
  - `31e8a990` is March's `GetNowStateData()`: in-range index, else 0.
  - The `EBP` at `@327964b2` is the parser's own `StateData&`, the same one the `FURY_MODE` store writes. `[esp+0x1c]` is the first argument. Every float store to that slot follows a `push`.
  - `32784c00` = March `CX2GUNPC::LoadStateData( StateData&, InitData&, KLuaManager& )`.
- **Parse.**
  - `FURY_MODE`: a bool, default false, `NOWSTATE_SET_FLAG` bit 0x50. It comes after `FALL_DOWN` and June's `IS_APPLY_CINEMATIC_UI`, and before `ENABLE_ATTACK_BOX`.
  - `ATTACK_POWER_RATE_FURY` (StateData `+0x414`) and `ANIM_SPEED_RATE_FURY` (`+0x418`) are read only inside a `FURY_DATA` sub-table. That table name is plaintext in the exe.
  - Each rate defaults to 1.0, both on a missing key (`fld1`) and in the StateData ctor `32393dc0`.
  - June has 83 flags, March 76 (`FLAG_MAX`, probed). June's `FALL_DOWN` is 26 where March's is 24, so the enums do not line up. The port appends a new `FLAG_FURY_MODE`.
  - All five names are confirmed from two sources: `xorstrings.tsv` and the June Lua plaintext.
- **The Fury byte `+0x8fb7`.**
  - Its only writers are the common ctor (cleared) and `SetFuryMode(bool)` `31e4d7c0`. Its readers are `GetFuryMode()` `31e4d7d0`, `GetFinalDamage` and `ResetAnimSpeed`. Both binds are confirmed by xorstring and plaintext.
  - Nothing server-driven, no HP threshold, no reset on Init, ReInit or state change. Raid Lua turns it on and off: Red Dragon P2 by position and a timer, Eltrion MK2 by a time table.
- **Readers.**
  - **Damage:** in `GetFinalDamage`, each nonzero damage local is multiplied by `+0x414`, as the C4 review had it. Re-traced.
  - **Animation:** the NPC's `ResetAnimSpeed` `327615d0`. When the byte and the flag are set, the base speed (`m_fAnimSpeed`, or the timed-event speed) is **replaced** by a nonzero `+0x418`. It is not multiplied. The born rate, the buff rate and the comeback x4 still apply on top.
  - Nothing else reads bit 80, `+0x414` or `+0x418`: the scan covered every `GetNowStateData` call and every `and reg, 0x10000`. There is no UI, effect or sound tied to Fury.
- **No partial update: June's `ResetAnimSpeed` changed in four more ways, all in C4c's port list.**
  1. It is a `CX2GameUnit` virtual (slot `0x3f8`). The base and `CX2GUUser` versions are empty.
  2. It no longer calls `m_AdditionalAnimSpeed.UpdateResultValue()`. Instead, June's `CheckAndUpdateAdditionalStat` (`32290920`) runs the update only when the must-change flag is set, then calls `ResetAnimSpeed()`. Slot `0x3fc` is a no-op in every unit class.
  3. A factor `+0x8b6c`, default 1.0. Its only writer is `CX2GURose` slot `0x860`, from the passive `RNSI_P_ROM_OPEN_MIND` on Rose's summons. That writer goes to checklist **7f** (D8).
  4. A factor `+0x8fdc`, default 1.0. Its only writer is the `CreateGUNPC` tail `327ef570`, from Hero dungeon `Add_HeroDungeon_ExtraStatForException`. March has no hero dungeons, so the writer is not ported, the same call as buff 16-step3b's decision 1. Only the member and its multiply are ported.
- **Reach.** Only five June raid scripts use the keys: `RAID_ELTRION_MK2_PHASE1`, `_PHASE3`, `_PHASE3_BG`, `_PHASE3_BH` and `Raid_Red_Dragon_Phase2`. No March or adopted script does. The play-test therefore needs a staged test NPC (C4c kickoff).

**Port list for C4c.** Flag `SERV_IRUHADEV_JUNE_NPC_FURY_MODE`; the kickoff has the detail.
- `X2GUNPC.h`/`.cpp`:
  - `FLAG_FURY_MODE`;
  - the `StateData` members `m_fAttackPowerRateFury` and `m_fAnimSpeedRateFury` (1.0);
  - the two parse sites;
  - `m_bFuryMode` (cleared in `CommonConstructor`), `SetFuryMode` and `GetFuryMode`;
  - two anim factors, 1.0 in `CommonConstructor`;
  - June's whole `ResetAnimSpeed` body.
- `X2GameUnit.h`/`.cpp`:
  - a virtual empty `ResetAnimSpeed()`;
  - the `CheckAndUpdateAdditionalStat` anim block;
  - the `GetFinalDamage` multiply, before `JuneApplyDivideDamageFromRange`.
- `X2Main.cpp`: the two binds.
- `KTDXLIB/Always.h`.

**JUNE_BUGS candidates for C4c:**
- Fury anim replaces the base speed rather than multiplying it.
- A zero anim rate is skipped, but the attack rate is never tested (0 zeroes the damage).
- NaN is stored or multiplied.
- `SetFuryMode` does not refresh the speed until the next `ResetAnimSpeed`.
- The byte survives every state, death and ReInit.
- The Hero-table walk's last match wins (recorded, not ported).

**Hygiene, deploy, play-test:** none. This phase has no code and no build.

### C4c - Done (2026-10-07, Opus 5.5 high)

**Landed** (`7090255`; flags `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` + `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`, both new): kickoff items 1-7 as listed there, June addresses cited at each site.
- `X2GUNPC.h/.cpp`: `FLAG_FURY_MODE` appended before `FLAG_MAX` (after the ARA sound flags, which index by +1/+2); StateData `m_fAttackPowerRateFury` / `m_fAnimSpeedRateFury` (1.0); `FURY_MODE` parse after `FALL_DOWN`; `FURY_DATA` at the end of `LoadStateData`, after `#endif X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`; `m_bFuryMode` (cleared in `CommonConstructor` only), `SetFuryMode` / `GetFuryMode` (+ `JuneGetFuryAttackPowerRate`, the gate as a public helper so `X2GameUnit.cpp` needs no `StateData`); `m_fRoseSummonAnimSpeedRate` / `m_fMultiplicationAnimSpeed` (1.0, no writer: checklist 7f / C4d, both updated with the names); June's whole `ResetAnimSpeed` (March body in `#else`).
- `X2GameUnit.h/.cpp`: `virtual void ResetAnimSpeed() {}`; `CheckAndUpdateAdditionalStat` updates the anim stat only on must-change and then calls it (March line in `#else`); the `GetFinalDamage` multiply between the locals' load and `JuneApplyDivideDamageFromRange`. `X2Main.cpp`: the two binds.

**Pre-checks.**
1. `cl /EP` probe on an `stdafx.h` TU (US_SERVICE defines): `X2OPTIMIZE_NPC_NONHOST_SIMULATION=ON`, `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE=ON`.
2. `CX2GUNPC::OnFrameMove` reaches `CX2GameUnit::OnFrameMove`, which runs `OnFrameMoveBuff` then `CheckAndUpdateAdditionalStat`, for host and non-host. Its three early returns are a battlefield NPC before its first sync packet, `NOT_RENDER_NPC_GAME_EDIT`'s hide, and the adaptive frame skip (`E_FAIL` on in-between frames, which only defers a whole frame). Must-change survives a skipped frame.
3. Added: dropping `UpdateResultValue()` is safe only if every anim-stat write raises must-change. All four setters (`SetBasicStatValue`, `SetOptionValue`, `SetPassiveSum`, `SetPassiveMultiple`, `X2BuffTemplet.cpp:789-835`) and the factor insert/update set it. Init's 0 -> 1.0 basic value therefore triggers the update.
4. March's `ResetAnimSpeed` had no other caller in buff code. Consequence (June's behaviour): **an NPC slow or haste now changes its play speed at once**; in March it waited for the next state start or timed event.

**Deviations.** Log lines are `JUNE_FX FURY ...` in `offline_server.log` (`CX2OfflineLog::Server`, unbracketed like C10b-1), not the kickoff's `[JUNE_FX]` in ErrorLog: read them with `logs.py --grep "JUNE_FX FURY"`. Lines: `set` (every call), `dmg` (about 1/s), `anim` (first 200). JUNE_BUGS 94-98.
**Census:** it tracks DamageEffect / EffectSet / DamageData roots only (`census.tsv` roots DD/DE/DS/ES/ESD), so the four NPC state keys have no rows to flip.

**Hygiene** (`--worktree --files` the 6 code files): `_NPC_FURY_MODE --with JUNE_NPC_ANIM_SPEED`: `files: 1 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS` (OTHER = `X2GameUnit.h`, ANIM_SPEED only). `_NPC_ANIM_SPEED` alone: `files: 1 PASS, 0 PASS~, 0 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS` (OTHER = the Fury lines in the same files). After the commit (`7090255`), `--range 7090255^..HEAD`: the same two summaries, identical counts, both `RESULT: PASS`.
**Deploy** (`--pch force`, new virtual in `X2GameUnit.h`): `DEPLOY OK: X2_offline.exe is 19,314,688 B, dated 2026-10-07 19:23:05, md5 fd7ff8e5d20ddff57b422479a9fca33e, equal to X2/US_SERVICE/x2.exe (2026-10-07 19:23:00); built at mods/june-buff-port ee76e7c + 6 uncommitted code file(s).` (the 6 = this phase's). Exe holds `SetFuryMode`, `GetFuryMode`, `FURY_DATA`, both rate keys and the three `JUNE_FX FURY` formats.

**Test staging (revert after the play-test; the review kickoff has the steps).** Mars (`NUI_THIEF_THIN_ZERO`, 13 of the El Tree's spawns, dungeon 30000), `kom_contents/data037_kom/Thief_Thin_Zero.src.lua` (decompiled, three edits marked `C4c TEST STAGING`): `START` gets `LUA_STATE_START_FUNC` -> `slot2:SetFuryMode(true)` (June's call form); `THIEF_THIN_ZERO_ATTACK` (melee slash, attack box) gets `FURY_MODE = TRUE`, `FURY_DATA = { 2.0, 2.0 }`. Original bytecode: `kom_contents_backup_june_fx_c4c/data037_kom/Thief_Thin_Zero.lua` (md5 `ab2aad2e`). `PACK OK data037` (`~1 changed: Thief_Thin_Zero.lua`). Round trip: the compiled staged `.lua` decompiles to the original's lines plus the three edits (same line multiset; the rest is key order inside table constructors). `COPY OK data037: ...\data\data037.kom  9613520 bytes, 1915 entries, md5 5f6be6939a8c23cd4423279835e6a7cd` (user yes; backup `kom_backup_tools/data037.kom.20261007_192829`). Pairing: this data037 needs an exe at or after `7090255` (older ones do not bind `SetFuryMode`), and every El Tree run shows a Fury Mars until the review reverts it.

**In game** (El Tree, any character): Mars's slash plays at 2x (`ATTACK` only; walk/wait normal) and hits about 2x; `JUNE_FX FURY set` once per Mars, `anim ... rate=2 v=2` (more when buffed), `dmg ... rate=2 phys a->2a`. Regression: in another dungeon, BFI 628 (`MOTION_SLOWDOWN_WIND_BLAST`, anim x0.15, 10 s) slows monsters at once and they recover after 10 s; no FURY line.

**Play-test PASSED 2026-10-07** (exe fd7ff8e5, data037 md5 5f6be693; logs read, 0 EXCEPTION). El Tree twice (Tut14c, jtRuneSlayer): one `FURY set ... on=1` per Mars at stage start, about 24 `anim ... rate=2 v=2` (the slash visibly 2x), 4 `dmg ... rate=2 phys 1->2`. Most 2x slashes did not hit: not the port. The NPC hit test is a strict per-frame point sample (`start < anim time < end`), March `CommonFrameMove_TimedEvent` and June `327970d0` alike (decompiled: the `+0x214` anim-time test, no crossing check), so the staged 2.0 rate shrinks Mars's 0.067 window to about 33 ms real and frames step over it; June's own Fury rate is 1.2. March behaviour, so no JUNE_BUGS row. Regression (user, earlier launch, Ruben's second dungeon): BFI 628 on every monster slowed them at once, mid-attack, and they recovered after about 10 s.

### C4c - Review (2026-10-07, Opus 5.5 high)

**Verdict: PASS, no code change.** Deployed exe fd7ff8e5 = HEAD code (`git log 7090255..HEAD` touches no code path; game-dir md5 re-read), nothing to rebuild.

**Checked against June** (raw disassembly with `june2017_re/disasm_range.py`: `327615d0` has no Ghidra function; Ghidra's own server was up):
- **Parse.**
  - `@3278a12f-3278a145`: a GetValue miss leaves al = 0, so the default is false, then `32386970( 0x50, v )`.
  - `@32796366-3279670f`: `BeginTable == 1`, then two `GetValue(float)` calls, each with `fld1` on a miss (`@32796551`, `@32796707`), then `EndTable`.
  - Port: `FURY_MODE` sits after `FALL_DOWN` with default false; `FURY_DATA` sits after `#endif X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` and inside `X2OPTIMIZE_NPC_NONHOST_SIMULATION` (the C4c probe found that one ON); the ctor sets both members to 1.0.
- **GetFinalDamage** `@32285757-322857f6`:
  - The gate is `+0x264 == 1`, then `dynamic_cast`, then byte `+0x8fb7 == 1`, then `322afe40`. The rate is `GetNowStateData()+0x414` and is never tested.
  - Each local is multiplied only when nonzero: `fucomp` vs 0, then `test ah,0x44 / jnp`, so 0 is skipped and NaN is multiplied.
  - The DIVIDE call `32622780` comes next.
  - The port matches; `pAttacker` is already NULL-checked at `X2GameUnit.cpp` GetFinalDamage's top.
- **ResetAnimSpeed** `327615d0`:
  - Starts with the skin null return. `IsSamef` (`test ah,0x41`) picks `+0x8b70` or `+0x8b74`.
  - Fury: `+0x8fb7 == 1`, the bit `0x10000` test, `+0x418`, then `fldz / fucompp / jnp`. A nonzero rate is **stored** over v (not multiplied); 0 is popped.
  - Then, in order: `x +0x8b64`, `x` the protected `+0x485c`, `x +0x8b6c`, `x +0x8fdc`, comeback `x 4.0`, `SetPlaySpeed`.
  - There is no `UpdateResultValue` at entry. The port matches in order, and its `#else` is March's body byte-intact (flagcheck 0 deleted).
- **CheckAndUpdateAdditionalStat** `@322909a2-322909fd`:
  - If `+0x486c == 1`: the level is read (vslot `0x20c`), the still-set test runs, then `3236cd90` + `320839e0` (that pair = March `CheckAndUpdateResultValueWithOptionData`, `X2BuffTemplet.h`), then vslot `0x3f8`, then `0x3fc(1)`.
  - The port's `if ( must-change ) { CheckAndUpdate...WithOptionData; ResetAnimSpeed(); }` is equivalent.
- **Anim-stat writers.**
  - Every write to `m_AdditionalAnimSpeed` in X2Lib raises must-change: the 4 setters (`X2BuffTemplet.cpp:789-835`), factor insert/update/erase (`:970/:990/:1014`), `InitAdditionalPassiveMultipleAndSum` (`SetPassiveMultiple`/`SetPassiveSum`), and the NPC's own `SetPassiveMultiple`/`SetOptionValue` (`X2GUNPC.cpp` ~`:18736/:18752`).
  - No accessor returns the object by reference, and nothing else calls `UpdateResultValue()`.
  - So the gated update cannot leave the result stale or at its ctor 0.
- **Overrides.** `ResetAnimSpeed()` is overridden only by `CX2GUNPC`. `CX2GUUser`'s 4-argument overload only hides the name.
- **JUNE_BUGS 94-98:** each matches the disassembly above. #94: replace, not `*=`. #95: the attack rate is untested. #96: NaN passes both `jnp` tests. #97: `31e4d7c0` only stores. #98: the byte has no resetter.

**Hygiene** (`--range 7090255^..HEAD`):
- `_NPC_FURY_MODE --with JUNE_NPC_ANIM_SPEED`: `files: 1 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. OTHER = `X2GameUnit.h`, which holds the ANIM_SPEED virtual only.
- `_NPC_ANIM_SPEED`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / same zero line / `RESULT: PASS`. The 5 OTHER are the Fury lines in the same files plus `Always.h`'s other define.

**Staging reverted.**
- Copied `kom_contents_backup_june_fx_c4c/data037_kom/Thief_Thin_Zero.lua` back (md5 `ab2aad2e`) and deleted the staged `.src.lua`. The backup folder is kept.
- `PACK OK data037` (`~1 changed: Thief_Thin_Zero.lua`), 9,613,442 B, md5 `c7c0c3b01eb2efb631a8b664aba8c9e1`. That is **byte-identical** to the pre-staging live kom (`kom_backup_tools/data037.kom.20261007_192829`), so no new play-test is needed.
- `COPY OK data037: ...\data\data037.kom  9613442 bytes, 1915 entries, md5 c7c0c3b01eb2efb631a8b664aba8c9e1` (user yes; backup of the staged kom `kom_backup_tools/data037.kom.20261007_194417`). El Tree's Mars is back to March; any exe pairs with it.


### C4d - Done (2026-10-07, Opus 5.5 high, user OK; flag `SERV_IRUHADEV_JUNE_HERO_NPC_ANIM_SPEED`)

**June's Hero-dungeon NPC anim speed, all three pieces, dormant.** Funcmap rows tagged `Effect C4d` (4 new; row `327ef570 @327ef852` updated to "ported").
- **Bind body found: `327db2c0`** (pushed @31dc41db, name use @31dc3faa). It walks the argument table's rows from index 1. Each row is **appended first** (`327e7dd0` = `resize( size + 1 )`) and then filled, so there is no dedupe and nothing clears the list. Keys: `DUNGEON_ID` -> `+4` (int, 0 on a miss), `NPC_ID` -> `+8` (int, 0), `MULTIPLICATION_ANIM_SPEED` -> `+0` (float, 1.0). The layout is confirmed twice: key lengths 10/6/25 in the bind, and the tail's `+4`/`+8`/`+0` reads.
- **Storage:** `CX2DungeonManager::HeroDungeonExtraStatForException` and `m_vecHeroDungeonExtraStatForException` (June `g_pData +0x44`, vector `+0x5c`).
- **Bind:** `Add_HeroDungeon_ExtraStatForException_LUA` (void, as June). It is bound in `X2Main.cpp` next to `AddDefaultRoomTitle_LUA`.
- **`CreateGUNPC` tail:** at the end of `CX2DungeonSubStage::CreateGUNPC`, after the last setter, as June. It needs `_JUNE_NPC_ANIM_SPEED` too.
  - The gate is `JuneIsHeroDungeon( room dungeon ID )`. June's `g_pData +0x70` is March's `m_pDungeonRoom`. The offsetprobe puts March's room at +0x8c and its DungeonManager at +0x60; June has them at +0x70/+0x44, the same 0x2c apart. `m_DungeonID` sits at +0xdc in both.
  - The walk compares the dungeon ID first, then `GetNPCTemplet().m_nNPCUnitID` (June `[+0x85f8]`), and sets the NPC's factor through the new `CX2GUNPC::SetMultiplicationAnimSpeed`.
  - It never breaks, so the last match wins (JUNE_BUGS **99**, latent).
  - June has no NULL test on the room, and the port has none either. Both callers run only inside a dungeon game. Its one creation site is `CX2StateDungeonGame::CreateGame`, and for every mode (tutorial and training included) `CX2DungeonGame::GameLoading` -> `DungeonLoading` already reads the room unguarded to build the dungeon, before any NPC spawns. The only in-state `DeleteDungeonRoom` (`X2StateDungeonGame.cpp:2653`) is inside `#if 0`.
- **`JuneIsHeroDungeon` is shared:** it is now `CX2DungeonManager::JuneIsHeroDungeon` (June `32931be0`). `X2PartyUI.cpp`'s static calls it under the new flag, and its own copy is kept in `#else`.
- **Not staged, and the file would not load in March:**
  - `HeroDungeonData_Client.lua` is not in March's data.
  - Its line 1 calls `Add_HeroDungeon_AbsoluteMonsterTeleport`, and none of the four sibling `Add_HeroDungeon_*` binds exists in March, so the script would stop there.
  - March's `DungeonEnum` has no `DI_*_HERO_*` and `Enum.lua` no `NUI_ELSION_*` / `NUI_ELYSION_BOSS_*`.
  - June's loader (`3258c470 @3258e244`) is not ported. No STATUS row owns Hero-dungeon mode; buff 16-step3b decision 1 left it dormant.
- **Log (not June):** once per run, at the first dungeon NPC: `JUNE_FX HERO_ANIM ... rows=0 ... dungeon=<id> hero=0` in ErrorLog.txt.

**Hygiene:** worktree flagcheck PASS (`_HERO_NPC_ANIM_SPEED`: 5 PASS, 1 PASS~ blank line, 1 DEFINE, 0 deleted; `_JUNE_NPC_ANIM_SPEED` on the two compound-guarded files: 2 PASS). Committed `2cf5fef`; range flagcheck `2cf5fef^..2cf5fef`: 5 PASS, 1 PASS~, 1 DEFINE, 0 FAIL, 0 deleted, RESULT PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,316,736 B, dated 2026-10-07 20:06:05, md5 aa9143e1f31906afc9b30beae800065c` (built at `23106ff` + this phase's 7 files). The exe holds the wide `JUNE_FX HERO_ANIM` string and the bind name.
**In game:** nothing changes. Run dungeon 30000; ErrorLog.txt gets one `JUNE_FX HERO_ANIM` line with `rows=0`, `dungeon=30000` and `hero=0` (read it with `python tools/logs.py --grep "JUNE_FX HERO_ANIM"`; `--tag JUNE_FX` matches nothing).
**Also:** MODS.md (row + section), JUNE_BUGS 99.
**Play-test PASSED 2026-10-07** (El Tree 30000; logs read, run after the aa9143e1 deploy): exactly one `JUNE_FX HERO_ANIM ... rows=0 ... dungeon=30000 hero=0` (`CreateGUNPC`, `X2DungeonSubStage.cpp:2496`), 0 EXCEPTION, clean exit.

---

## C5 - 8b, buff-factor selection, damage triggers, nearest bone

**Status: code done 2026-10-06 (Sonnet 5.5 high), Opus review and play-test pending; see "C5 - Done" below. Model:** Sonnet 5, high, then an Opus review. Needs C2. **Coordinate with the
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

### C5 - Done (2026-10-06, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_DAMAGEDATA`, no new flag)

**What landed** (commit `ba39843`; pure insertions; details in `MODS.md`, "damage triggers and nearest bone"). `flagcheck --range ba39843^..ba39843`: `files: 3 PASS, 4 PASS~, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s)`, `RESULT: PASS`.
- **Parse** in `SetDamageDataFromLUA`: `ADDITIONAL_ATTACK_TRIGGER_TYPE` (list, zeros kept), `DAMAGE_TRIGGER_DATA` (positional sub-tables; defaults type 0, BTI 0, POWER_RATE 1.0, MULT 0.0), `DAMAGE_TRIGGER_CONDITION` (int, -1, rule-5 log). Both vectors are emptied on every parse, as June does (`@3263d39c`, `@3263d40f`), so `DAMAGE_DATA_NEXT` does not inherit them.
- **Readers** on `DamageData`: `JuneIsAttackTrigger` (= `32610fb0`), `JuneFindDamageTriggerData` (= `32610e60`).
- **`3227c940`** = `CX2GameUnit::JuneApplyMultipleDamageByDebuff`, called in `GetFinalDamage` right after the `<= 0` clamp, as June's `@322869b2` follows `@32286995`.
  - Polarity read from the operands (`fld MULT; fcomp 0.0`, constants `334350e8` = 0.0 and `334355b0` = 1.0): the formula runs for MULT > 0 and n > 0. `x87test.py` was not used: it covers the reversed `fldz; fcom mem` idiom, not this one.
- **`321da350`** = `EffectSetInstance::JuneSetTraceNearestBone`, called in `CEffect::AttackResult` after the DELAYTIME block with the main-effect mesh position (`ebp` at `@31f8679d` is the `GetMeshInstance` result; mesh `+0x3f0` is `CXMeshInstance::m_vPos`, funcmap row 91). Readers patched in the **ESM_GAME** branch of `GetEffectPosition` (`@321dd270`) and `GetEffectPositionByBoneName` (`@321dda93`).
  - **The override applies to the trace target**: the observable at instance `+0x14` is ctor arg 3 (`@321df2b7`). C2's row said "trace unit (+0xd4)"; `+0xd4` is the **pet** slot (mode 3, `ESM_COMMON`).
  - **June reads the name in all three modes** (ESM_GAME `@321dd270`/`@321dda93`, ESM_FIELD `@321dd01e`/`@321dd84e`, ESM_COMMON `@321dceaa`/`@321dd6d5`). Only the ESM_GAME reads are ported: `AttackResult` passes a game unit, so only an ESM_GAME instance can carry the name. An earlier draft of the code comments cited the mode-3 addresses; fixed in a second commit (6 comment lines replaced, the only deletions).
  - **No pooled-instance reset is needed**: `EffectSetInstance` is created per `PlayEffectSet`; the member is a plain `std::wstring`.
- **The `ParsingBuffFactorID` family is not touched**: buff Phase 10 owns it (`SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID`, `X2DamageManager.cpp` ~`:4260-4390`), and the census shows all six keys `implemented`.

**Hygiene** (`flagcheck JUNE_DAMAGEDATA --worktree --files <the 7 code files>`):
```
files: 3 PASS, 4 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
With `--with JUNE_FX_DEBUG`, `X2EffectSet.cpp` shows `tok! 1`: one `#endif` of that family with no token. It is not in this diff; without `--with` it is 0.

**Deploy** (after the comment-only second commit): `DEPLOY OK: X2_offline.exe is 19,264,000 B, dated 2026-10-06 22:34:24, md5 055976c6a5a28ddfa546f51ef22189a9, equal to X2/US_SERVICE/x2.exe (2026-10-06 22:34:19); built at mods/june-buff-port a9ac1df + 1 uncommitted code file(s).` The three log strings (`trigger_parse`, `trigger_data`, `trace_nearest_bone`) and the three key names were found in the first deployed exe, which has the same code.

**Census** (`census.py`): all 8 C5 rows (`ADDITIONAL_ATTACK_TRIGGER_TYPE`, `DAMAGE_TRIGGER_DATA` and its four entry keys, `DAMAGE_TRIGGER_CONDITION`) and `TRACE_NEAREST_BONE` are `implemented`; "open rows by owner" lists no C5. **8b is not marked done**: C3 19, C4 5, C9 11 rows are still open (the one FAIL line is C10b-1's HP-rate item, unchanged). Funcmap rows 1181, 1275-1278, 1280 carry the port notes.

**How to see it in game: nothing yet.** No March `data036` script sets any of these keys (`march2014/data036/*.src.lua` has none), so the change is inert until a June DamageEffect that does is adopted, and the play-test is a regression check on `GetFinalDamage` and the effect-set hit path. Staging a test templet (one DamageEffect with `DAMAGE_TRIGGER_DATA` for a bleeding debuff, and one with `TRACE_NEAREST_BONE`) is the user's decision; offered, not done.

**Left to others (named, not changed):**
- The three buff-port callers still compare `m_eDamageTrigger` only: `DoMarkOfCommand` (`X2BuffTemplet.cpp` ~`:1814`, `:1880`), Pandemonium Fear (`:2087`) and `JuneCustomFunctionDamageReactGuards` (`:2160`). They diverge from June (`32610fb0`) once a script fills the list. Replace them with `damageData_.JuneIsAttackTrigger( n )`, a one-line change each (buff port).
- About 57 per-class call sites of `32610fb0` go to the character ports (checklist 7b already names C5 as their prerequisite).
- `31f64c00` (child copy of the list) is C6b's. Its kickoff now names the member and the reader.

**Open for the review:**
1. `CX2GameUnit::JuneGetNearestCollisionFrameName` is a third copy of `32278580`'s walk (the others: the file-`static` in `X2BuffTemplet.cpp`, and the inline loop in `CEffect::JuneGetLockOnAimPos`). I put it on the unit and did not touch buff-port code; folding the other two is an OTHER row.
2. Settled in this session, for the record: the early `return fFinalDamage;` inside `GetFinalDamage` (the fixed-damage / level path) is June's `JMP 32284b01 @32286063` (the epilogue), which also skips `@322869b2`, so the new call sits on the same paths as June's.
3. The stale comment above `m_bTraceNearestBone` in `X2DamageManager.h` (and the one at `X2DamageManager.cpp` ~`:176`) still says "parsed only". Left as is to keep the diff insert-only; the review may rewrite them.

### C5 - Review (2026-10-07, Opus 5.5 high)

**Verdict: correct against June; one flag-nesting fix and comment fixes landed (commit below), no behaviour change.**

**Checked in Ghidra (read-only):**
- `3227c940`: the argument order (`&damage`, then DamageData as ECX of `32610e60`), the polarity (`fcomp 0.0; test ah,0x41; jnz` skips on MULT <= 0 or NaN) and the formula `(PR - 1) * (n - 1) * MULT + PR` all match. Call `@322869b2` comes right after the clamp, EDI = GetFinalDamage's `this`.
- The call site `@31f8678b-31f867ad` matches:
  - `[ESP+0x24]` is the trace unit: 0 unless `+0x6c0` (`CREATE_EFFECT_SET_TRACE_UNIT`), then `+0x670`'s observable = `optrDefenderGameUnit` (`@31f86173-31f8618f`), as March's `pTraceUnit`.
  - EBP is restored from `[ESP+0x20]` on every path. That is `GetMeshInstance( +0x1e4 )` (`@31f85bb8`), i.e. `pMeshInst`.
- `321da350` (templet `+0x80`, list `+0x1c/+0x20`, unit, `L""` miss, assign `+0x130`) and `32278580` (the unit's `+0x204` set, `GetPointStart`, strict `<` from `33441c60`) match the port. All three copies of the walk are identical.
- `32610fb0` (`+0x53c` first, then `+0x544..+0x548`) and `32610e60` (stride 16 over `+0x554..+0x558`, NULL on a miss) match the two readers.
- `GetEffectPosition @321dd270`:
  - `+0x140` is the `+0x130` wstring's size. It is tested inside the `31e55680(+0x14)` validity check, then vslot `0x80` is called.
  - All branches converge at `@321dd51b` = `SetEffectSetOffset` (`321d9820`), as the port's `else` chain does.
  - **Open item (the observable) settled:** June's own BONE_NAME branch (`@321dd2bd`) uses the same `EBP = EDI+0x14` that March reads as `m_optrTraceTargetGameUnit`.

**Fixed (flag `SERV_IRUHADEV_JUNE_DAMAGEDATA`):**
- **Cross-flag compile dependency.** Both new `CX2GameUnit` declarations sat inside `#ifdef SERV_IRUHADEV_JUNE_BUFF_BINDING` (`X2GameUnit.h:3021`), while their definitions and callers needed only `JUNE_DAMAGEDATA`. Undefining `BUFF_BINDING` alone would not compile. The fix:
  - `JuneGetNearestCollisionFrameName`'s declaration now sits in its own `DAMAGEDATA` block after `#endif BUFF_BINDING`.
  - `JuneApplyMultipleDamageByDebuff`'s definition and its `GetFinalDamage` call are now nested in `BUFF_BINDING`, because they need `GetBuffAccumulationCountNow`.
- **Open item 3:** the two "parsed only / not ported" comments (`X2DamageManager.h:1267`, `X2DamageManager.cpp:196`) now name the consumer.

**Hygiene:**
- Range, before the fix (`--range ba39843^..HEAD`): `files: 4 PASS, 4 PASS~, 0 DEFINE, 4 OTHER, 0 FAIL, 0 ERROR`, `6 deleted line(s)`, `RESULT: PASS`. The 4 OTHER rows are peer commit `6d304a1` (`SERV_IRUHADEV_JUNE_ENCHANT_PARITY`, buff Phase 15) inside the range. The 6 deleted lines are `a86d69c`'s own comment fix.
- The fix, `--worktree --files` on the 4 files: `files: 4 PASS, 0 OTHER, 0 FAIL, 0 ERROR`, `10 deleted line(s)`, `RESULT: PASS`. The 10 lines are this flag's own: 8 comment lines rewritten, 2 declaration lines moved.
- Range after the commit: see STATUS.

**Census:** unchanged by the review. All 8 C5 rows and `TRACE_NEAREST_BONE` are `implemented`. Open rows by owner: `C3 19, C4 5, C9 11` (+ C10 6, C11 3), none C5. 8b stays open on C3/C4/C9 only, as the Done note says.

**Settled:**
- **Open item 1 (fold the three walks): declined.** The copies are identical to June and to each other. Folding would add an OTHER row in buff-port code for no behaviour.
- **The three buff-port trigger compares:** the Done note's list was wrong on one line. `X2BuffTemplet.cpp:1880` is March's own code in `#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` (`:1869`). The real sites are `:1814`, `:2087` and `:2160`; June calls `32610fb0` with 3, 6 and 7 there (`@3236df5b`, `@3236e306`, `@323704cb`). This is written into `kickoffs/june-buff/16-step3c.md`, which owns the change. It is not done here, to keep out of the buff port's file.

**New, not C5's keys (for the user):** June's `RT_CREATE_EFFECT_SET` case has a branch March lacks, `31f861e2-31f8672a`, before the PlayEffectSet. Funcmap row `31f85910 @31f861e2-31f8672a`.
- It applies with a trace unit, when `32481fa0` is true (main `+0x8c == 0x15`, then `3266cdc0`) and the trace unit's vslot `0x54` UID equals that of the unit reached through `3266cb90`/`31e56700`.
- It finds the nearest live NPC with ID `0x621` = `NUI_FIELD_BOSS_COLLISION_BOX_1` and plays the set with other arguments (three PlayEffectSet variants).
- March's `X2_STATE` 21 is `XS_ARCADE_GAME`, not `XS_BATTLE_FIELD` (25), so June's state 0x15 is not settled.
- It does not touch `321da350`'s inputs. Under the no-partial rule this case is now part-June. Proposed owners: C6a2 identifies it, C6b ports it (C6b already rewrites AttackResult's RT_CREATE). **The user decides.**

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,267,072 B, dated 2026-10-07 05:58:47, md5 87310d3ce9e3c44cc59f5093352cc332, equal to X2/US_SERVICE/x2.exe (2026-10-07 05:58:40); built at mods/june-buff-port a3e1eda + 4 uncommitted code file(s).` (the 4 are this review's files)

**Play-test:** none new. The changes are a declaration move, a nesting that is a no-op in the shipped build (both flags on), and comments. The 2026-10-07 regression pass stands.

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

### C6a - Done (2026-10-06, `2456285`, Opus 5.5 high; no code, no flag, no build)

**What landed:**
- 30 verdicts in `june2017_re/funcmap.tsv`, all tagged `Effect C6a`.
  - 5 rows upgraded in place from candidate to confirmed: `31f8fdb0`, `31f649d0`, `31f64c00`, `31f65e40`, `31f65f90`.
  - Row `321db1b0` gains its full 23-tag table.
  - 24 new rows: one per CEffect / DamageData / DamageEffectData offset, the modifier adders, the entry ctor `31f8bf00`, the invoke-rate gate, the tail order, and unit vslot `0x248`.
- The C6b kickoff now carries the findings, and C6b depends on C5 (see "Changes to C6b").
- The checklist's section 7a lists the class code that drives the tail.

**Answers to Phase 10c's step-1 questions:**
- **`+0xb00` / `+0xb04`** are March's own `m_iIndex` / `m_iType` (`SERV_RENA_NIGHT_WATCHER`).
  - Same defaults (-1 / DET_NONE), same member order, same CheckDamageEffectType `32273980`.
- **`+0xb32`** is APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION. **`+0xb66`** is March's `m_bInheritDamageFromMaster` (7d).
- **`+0xb44`-`b47`, `+0xb5c`/`b60`, `+0xb64`, `+0xb65`** are June-only gate flags, and nothing sets them except class code.
  - Mostly through June's EffectSet "damage-effect modifier" list (`321db050`/`321dad70` -> `321db1b0` tags 12/13/14/16/17/19).
  - Directly by Add, Eve, Rose, Elsword and Ain skill functions.
  - In March they are absent (rule 5), so the tail runs inert until the characters are ported.
  - `+0xb64` is the one exception: the EffectSet copies it from `EffectSetInstance::m_bHyper`.
- **`+0xb5c @31f7f98b` is a ctor init** (BL = 0, fldz), not the destination of RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT. The guess in Phase 10c was wrong.
- **Entry fields.** `+0xb0` is a float, default 1.0. `+0xb4`/`+0xb5` are bools, default 0. All come from the DamageEffectData ctor `31f8bf00`.
  - No loader writes any of them.
  - Only `31f649d0` / `31f65e40` / `31f65f90` (and class code calling them) change them.
- **DamageData.** `+0x5a4`/`+0x5a8` are `m_eFirstAttack` / `m_RelateSkillData`, confirmed by four independent sites.
  - The spawners' `+0x7a4`/`+0x7a8` copy is already in March.
  - `+0x5fd` is a June-only bool with no Lua key; its only reader is Add.
- **Invoke-rate gate:** identical to March (`m_iType != DET_ARROW_OF_EXPLOSION`) in both the hit and touch-land spawners. No hit-spawner change.

**Changes to C6b (`kickoffs/june-effect/C6b.md`, the source of truth):**
- **Five call sites, not two.**
  - Hit, die, the OnFrameMove CREATE_DAMAGE_EFFECT loop (all five callees), and RT_CREATE_DAMAGE_EFFECT (`31f8fdb0` only).
  - The no-partial rule covers them all.
  - 7d's inlined INHERIT_DAMAGE copy at `:2891` is replaced, not doubled.
- **C6b now depends on C5.** `31f64c00` needs C5's ADDITIONAL_ATTACK_TRIGGER_TYPE list member.
- **The EffectSet `m_bHyper` write** (`@321ef6d5`) joins C6b, which adds `X2EffectSet.cpp` to its Touches.
- **Three June quirks to keep verbatim, each with a JUNE_BUGS row:** `b47` copies only `+0x528`, the entry marks are always set to 1, and there is no null test on DAMAGE_DATA_NEXT chain entries.
  - March's `m_pDamageDataNext` can be NULL, so C6b must test it. The kickoff says so; without the test it crashes.

**Open (for the user):**
- The rest of EffectSet `OnFrameMove_CreateEffect @321ef639-321ef66d` is not ported anywhere yet:
  - `m_iIndex` from instance `+0x1b8`, stamped by unit vslot `0x248` (base no-op; Luciel/Ain override). With it, every EffectSet-made effect of a March class gets index 0 instead of -1.
  - two unidentified CEffect members, `+0x838` and `+0x7e0`;
  - `321ee480` x2;
  - the runtime modifier list.
- Options: a small identification phase, or leaving it to the Luciel/Ain ports.
- **Decided 2026-10-06 (user):** a small identification phase, C6a2 (`kickoffs/june-effect/C6a2.md`, Opus, no code). It runs before C6b and adds anything data- or engine-driven to C6b's scope.

**Hygiene:** docs and TSV only. `funcmap.tsv` stays ASCII / LF, 4 fields per row. No game file changed.

### C6a2 - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build)

**What landed:**
- 20 new rows and 6 updated rows in `june2017_re/funcmap.tsv`, all tagged `Effect C6a2`.
- C6b's kickoff gains a "C6a2 findings" section.
- A new phase, **C6c**, gets `kickoffs/june-effect/C6c.md` and `C6c-review.md`.
- JUNE_BUGS rows 59-61; checklist sections 3 and 7a.

**Verdicts on `321ee600 @321ef5e6-321ef6d8`:**
- **`@321ef5ea-321ef632`** is March's `SetLockOn( pEffect, 0, LockOnType )` (default modulus 30) followed by the first-attack / relate-skill copy, inside the GUT_USER branch.
  - **Correction to C6a:** the copy, the DamageData members and every spawner copy sit under `ADD_RENA_SYSTEM`, which is **off** (`Always.h:238`). So "already in March" holds for the source only; nothing is compiled.
  - It goes with checklist section 3's `ADD_RENA_SYSTEM` decision (D3). Port nothing here.
  - SetLockOn touches neither velocity nor follow-line, so March's different block order is harmless.
- **`@321ef639-321ef66d`** are three unconditional stamps from the instance:
  - `m_iIndex` and new DD `+0x638` (`m_iJuneDamageEffectIndex`, Luciel's per-(skill, index) hit key) both come from instance `+0x1b8`. Unit vslot `0x248` writes `+0x1b8`; the base is a no-op and the ctor sets 0.
  - DD `+0x5e0`, which is buff 9b's `m_iJuneAttackerStateID`, comes from instance `+0x15c` = the owner's `GetNowStateID()` at PlayEffectSet (ctor -1).
  - These overwrite what SetDamageExceptionProcess wrote at CreateInstance.
  - DD `+0x638` also has SDEP (vslot `0x2e0`, base -1), SetDamageDataNext and NEXT-parse writers. Its only reader is Luciel.
  - **All of it goes to C6b**, under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`. It is the same logical change as the tail, because `31f8fdb0` copies `m_iIndex` and `m_bHyper` to children.
- **The `m_iIndex` stamp is safe in March** (index 0 instead of -1), so it is ported as the kickoff planned:
  - Rena's indices are `unitIndex*100 + 1..100`;
  - `GetInstanceByIndex` is fed only by her `DET_*_GROUND` effects;
  - nothing compares a CEffect index with -1.
- **Loader `@321ebf45/55` writes EffectData `PLAY_TYPE`.** It is unrelated to the instance's `+0x15c`.
- **`321ee480` x2 is the effect-set buff list.**
  - **EffectData `+0x118` is dead in June:** the ctor and dtor touch it, nothing writes it, and it has no key. No member and no call.
  - **Instance `+0x160`** is filled by `321e3650`. Its callers are SIEE 15 (data) and class code. The class-only variants `321e3730` (Rose) and `321e3830` (Add) carry behavior/finalizer modifiers.
  - Per entry, the consumer runs: GetBuffFactorPtr (**no null test**), a clone at level, the modifiers (`32d418a0` / `32d41960`->`32d40dd0`), then `PushBuffFactor( clone, true )`.
  - **Port: C6c**, flag `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`, with the SIEE 15 attach moved there from C6b.
  - It gets its own phase and flag because it is a different change with its own data caller (`X2GUUser.cpp`) and its own play-test. It runs after C6b, which edits the same block.
- **`321db1b0` on instance `+0x170`** is class code only and stays in checklist 7a. **`m_bHyper` `@321ef6d5`** is C6b's (unchanged).
- **The vslot `0x10c` naming is not a conflict.** It is GetNowStateID (`3208ec80`); rows 614/733 test it against GUSI_DIE, which is a state ID.

**June quirks** (JUNE_BUGS):
- **59:** clear-per-entry, so only the last entry survives. Verbatim.
- **60:** no null test on GetBuffFactorPtr. The user's call; see below.
- **61:** the uninitialised Rate byte. A harmless no-op, not ported.

**Decided by the user (asked and answered 2026-10-07):**
- **Q (JUNE_BUGS 60).** June `321ee480` dereferences NULL when a set's buff list names a BFI that does not exist. June's own data never does this. March lacks June's aura BFIs until june-buff Phase 13.
- **(a) Recommended:** guard behind `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL`: skip the entry and log once, with June's line in `#else`.
- **(b) Verbatim:** a crash once a June aura set with a missing BFI plays.
- **Answer (user, 2026-10-07): (a), the guard.** C6c implements it.

**Also noted for june-buff:** June's NEXT parse resets `m_iJuneAttackSerial` (+0x5e8) to -1 (`31f79c20 @31f79ce3`); Phase 9b does not. SDEP restamps it whenever there is an attacker.

**Hygiene:** docs, kickoffs and TSV only. `funcmap.tsv` is still ASCII / LF with 4 fields per row. No game file changed.

---

### C6b - Done (2026-10-07, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`, `SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET`)

**What landed** (`df22a13`, then `6382401` for one log line; `X2DamageEffect.cpp/.h`, `X2EffectSet.cpp/.h`, `X2GameUnit.cpp/.h`, `X2DamageManager.h`, `Always.h`):
- **The five callees, as CEffect methods** (end of the `DAMAGE_EFFECT_WORK` block): `JuneInheritFromParent` (31f8fdb0), `JuneApplyInheritDamageRate` (31f649d0), `JuneCopyAttackTriggerToChild` (31f64c00), `JuneSetIgnoreDefenceDown` (31f65e40), `JuneSetIgnoreStateDefenceDown` (31f65f90), plus `JuneSpawnTail` (the hit/die order) and two list helpers.
- **State:** 8 CEffect gate fields (all false/0; only class code sets them), `m_bHyper`; `DamageEffectData` gets `fInheritDamageRate` (1.0), `bInheritIgnoreDefence`, `bInheritIgnoreStateDefence` (also in its `operator=`); `DamageData` gets `m_bCountAsOwnerSkillHit` and `m_iJuneDamageEffectIndex` (-1 in SimpleInit).
- **Call sites, five:**
  - hit and die spawners: 31f8fdb0 first in `if( pEffect != NULL )`, the tail after the first-attack copy;
  - CREATE loop: 31f8fdb0 and 31f64c00 after the INHERIT_DAMAGE block, the other three at the end. The 7d inline damage copy is in `#else`, so it is not applied twice;
  - RT_CREATE_DAMAGE_EFFECT: 31f8fdb0 only. The live branch is the `#else //ADD_RENA_SYSTEM` one, which discarded the pointer.
- **Effect set stamps** (C6a2): `m_iDamageEffectIndex` (0) and `m_iOwnerStateIDOnPlay` (-1) on the instance, two base virtuals on `CX2GameUnit`, the `PlayEffectSet` (game-unit overload) stamps, the `OnFrameMove_CreateEffect` stamps and `m_bHyper`, the SDEP tail, the NEXT copy and the NEXT-parse reset. `MODIFY_SET_DAMAGE_DATA`, `LOCAL_ROTATE_EFFECT`, `UPGRADE_RAVEN` and `SERV_RENA_NIGHT_WATCHER` are all ON (`cl /EP` probe), so every one of these sites compiles in.
- **RT_CREATE_EFFECT_SET** (own flag): branch on the trace-unit pointer. With one: LIFTTIME applies and the field-boss retarget gate runs (state, the eight raid field IDs; March has no raid object, so the boss unit is a stub that returns NULL and logs once, and the nearest-box search is ported behind it). Without one: only with the main-effect mesh, the set plays at its position, `GetRotateDegree()` and `GetDirVec()` (= 31c82e40: Ghidra shows the same (1,0,0) rotated by rotation + local rotation), life -1. June's trailing 1.0 has no March parameter.

**Corrections to the kickoff / C6a text found while porting:**
- `SetDamageDataNext`'s copy site is `SetDamageDataNextByChainIndex` (the live body); the `:9649` one is the dead `#else`.
- The child's CREATE entries sit in `m_vecCreateDamageEffect` until `SetDamageEffect` moves them to `m_DamageEffectDataList`, so the list helper walks both. It also walks the touch-land vector (parse-only, exists), so C7 has no list work to redo.
- The first NEXT table resets `+0x638`, NEXT2.. do not (`@31f79ce9` vs the loop's `@31f79e07`): JUNE_BUGS 77.
- **Open for june-buff:** June's NEXT parse also resets `m_iJuneAttackSerial` (+0x5e8) to -1 (`@31f79ce3`, and every numbered entry `@31f79e07`); Phase 9b does not. Left alone.
- JUNE_BUGS 74-78 added (b47 without PvP, always-1 entry marks, untested chain entries, NEXT2 index, LIFTTIME only with a trace unit). The key matrix has no key to flip; its 7d INHERIT_DAMAGE note now names the method.

**Hygiene** (`flagcheck`): SPAWN_INHERIT `--range df22a13^..HEAD`: `files: 6 PASS, 0 PASS~, 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`, 2 deleted lines. Those 2 are `6382401` rewriting C6b's own debug line, not base code; net `--base df22a13^ --at HEAD`: `5 PASS, 2 OTHER, 0 FAIL`, **0 deleted**, 0 non-ASCII, 0 EOL-mismatched, 0 encoding change, `RESULT: PASS`. The OTHER rows are `Always.h` (the two `#define`s) and `X2DamageEffect.cpp/.h` (they carry both flags). CREATE_EFFECT_SET `--range`: 0 FAIL, 0 deleted, `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,297,792 B, dated 2026-10-07 11:00:38, md5 fb3456427143e9fd978bbdc639cdf327, equal to X2/US_SERVICE/x2.exe (2026-10-07 11:00:34)`. The strings `SPAWN_INHERIT`, `SPAWN_TAIL`, `SPAWN_INHERIT_STAMP`, `CREATE_EFFECT_SET` are in the exe. Another session (C9) ran `build.py` once while this was in the tree, so an earlier exe (md5 `adda229e`) held step 1 only.

**How to see it** (`python tools/logs.py --tag JUNE_FX`; lines are `JUNE_FX CE_HIT <effect> <key> ...` and `ES_CREATE`, at most one a second per key):
- `SPAWN_INHERIT`: every child from a hit, die, CREATE and RT_CREATE_DAMAGE_EFFECT spawn: the gate flags (all 0 in March), `hyper`, `index` (the child's `m_iIndex` after the copy).
- `SPAWN_TAIL`: every hit/die child, with the entry rate (1.000) and the entry flags (0).
- `SPAWN_INHERIT_STAMP`: every effect-set damage effect, `index=0 ownerState=<state> hyper=<0|1>`.
- `CREATE_EFFECT_SET`: `path=trace_unit` or `no_trace_unit` per RT_CREATE_EFFECT_SET hit.

**Play-test result (2026-10-07, logs read): passed.** 26 `SPAWN_INHERIT` and 26 `SPAWN_TAIL` lines, gates 0, rate 1.000; the index runs unchanged down Delayed Firing (5, 9, 10, 14) and Explosion Trap (7) chains, basic arrows stay -1, one boom per index, 0 exceptions; the user confirmed the later trap (index 8) exploded. `CREATE_EFFECT_SET` was not reached.

**Play-test** (the check that can fail: `m_iIndex` now reaches hit and CREATE children). Character `NWatcher` (Rena, Night Watcher, class 114, level 70, unit 31, 600 SP, no skills learned; created for this test, backup `db_backup/els_db_20261007_104747_pre_createchar.sql`). The dev console cannot reach it: the children come from real hits.
- Night Watcher's basic arrows are `ARROW_OF_EXPLOSION` (the delay variant after Delayed Firing): hit monsters with X/Z combos and the dash attack, expect one explosion per arrow.
- Delayed Firing is skill 3227 and Explosion Trap 3225 (active `SKILL_ID` block): learn both in the skill window, put them on quick slots, use them.
- Log: `JUNE_FX CE_HIT ... SPAWN_INHERIT ... index=` should show the arrow's index on its boom child. A boom that fires twice, or never, is the failure.
- Optional, rare: `EffectSet_RAVEN_NUCLEAR` (Children's Day Nuclear Sphere item) and `EffectSet_SP_B_MASK` (`DAMAGE_EFFECT_TROCKTA_SP_B`) are the no-trace path.

### C6b - Review (2026-10-07, Opus 5.5 high; also covers C6a2's identification)

**Verdict: C6b is correct against June. No code changed, so there was no build or deploy; the deployed exe is still fb345642.**
**One C6a identification gap was found**, which led to a re-diff of every spawner window. Three engine gaps and one C3 consumer came out of it. All are identified and routed below (funcmap rows tagged `Effect C6b review`).

**Checked in Ghidra (read-only):**
- **The five callees, re-read in full:**
  - `31f8fdb0`: branch for branch. b5c scales the **child's** main effect (ECX = EDI); b46 copies parent to child.
  - `31f649d0`: the FLT_EPSILON gate, own damage plus every NEXT table, and entries die/CREATE/hit/touch-land only when rate >= 0.
  - `31f64c00`: 29/30 are pushed onto the child (`EBP+0x744`).
  - `31f65e40` and `31f65f90`: own PvP flag too, untested NEXT entries, entries always set to 1.
- **The callers of each callee:**
  - Every engine caller maps to a ported March site. Touch-land `31f81ba0` is C7's.
  - The `329xxxxx` callers are class code (checklist 7a).
- **Entry ownership:** the entry lists are parsed per instance, and the hit and die spawners work on copies. The rate multiply cannot leak, and `SetDamageEffect` moves and clears the CREATE list, so nothing is counted twice.
- **C6a2 stamps** (`321ee600 @321ef639-321ef66d`):
  - CEffect `+0xb00`, `+0x838` (= DD `+0x638`) and `+0x7e0` (= DD `+0x5e0`) are written unconditionally; `m_bHyper` comes from `+0x20` at `@321ef6d8`.
  - **PlayEffectSet:** the vslot `0x248` argument is `PUSH ESI` at `@321e1ffa`, scheduled early (base `RET 0x4`), and `+0x15c` is stored last at `@321e20ce`.
  - None of the six other overloads (`321e2120`, `2340`, `25d0`, `26d0`, `2790`, `28d0`) writes either.
- **The four DD `+0x638` writers:**
  - SimpleInit `@31f8f47b`;
  - SDEP `@3229971e`, after the serial wrap; June's early-outs skip it, as March's returns do;
  - the NEXT copy `@31f79b87`;
  - the NEXT reset `@31f79ce9`, where NEXT2.. reset only `+0x5e8` (`@31f79e07`, JUNE_BUGS 77).
- **C6a2's three negative verdicts hold:**
  - `ADD_RENA_SYSTEM` is OFF (`flagprobe.py`; `RIDING_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, `X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE` are ON).
  - EffectData `+0x118` has no writer: `321f4690`'s four callers push onto instance `+0x160` (`321e3650`, `3730`, `3830`) or onto a local vector (`32ad31b8`).
  - `m_iIndex` 0 cannot reach Rena: `GetInstanceByIndex` is fed only by `StartOfDelayedFiringData::m_iArrowIndex`, which is filled from `DET_*_GROUND` effects' `GetIndex()`, and `CreateDamageEffectIndex` returns `unitIndex*100 + 1..100`, never 0.
- **The kickoff's JUNE_FX item** (rate, gates, index) is split over `SPAWN_TAIL` (entry rate and flags) and `SPAWN_INHERIT` (gates, `hyper`, `index`). C6b's play-test log shows 26 of each.
- **RT_CREATE_EFFECT_SET:**
  - The branch is on `[ESP+0x24]` = flag ? defender : 0, which is March's `pTraceUnit`. LIFTTIME applies on the trace path only.
  - The gate is `32481fa0` (state `0x15`) plus `3266cdc0` (`0x9c61/62/67-6c` = 40033, 40034, 40039-40044). The stub returns NULL.
  - The no-trace argument list matches the port exactly; June's trailing 1.0 is extra. March's `GetDirVec` = `31c82e40`.
- **`PlayEffectSet`'s `pGameUnit`** is NULL-tested at `X2EffectSet.cpp:3185`, before both new unit calls.

**Deviation, not fixed:** the effect-set stamp block sits just inside `#ifdef RIDING_SYSTEM` (`X2EffectSet.cpp:1098`), not before it as the kickoff said. `RIDING_SYSTEM` is ON, so the behaviour is identical; it would only matter if that flag were ever turned off.

**Correction to C6a** (row `31f85910 @31f860f2-31f8614d`): "RT_CREATE_DAMAGE_EFFECT calls `31f8fdb0` only" holds for the five callees. But June also calls `SetScale( DD +0x498 )` and `31f6ea20` on that child. The re-diff that followed turned up:
- **C3** (kickoff note): `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z` (DD `+0x498`). Its three SetScale consumers are RT_CREATE, the CREATE_DAMAGE_EFFECT_RATE spawn `@31f86a76`, and the user AttackResult. The (1,1,1) default runs on every child, and it is not proven to be a no-op.
- **C7** (kickoff "Added by the C6b review"; all three do nothing on March data, while June's DamageEffect uses them):
  1. `31f6ea20` plus CEffect `+0xb18` (-1, class-set) plus base vslot `0x328`. Flag `_SPAWN_INHERIT`.
  2. The CREATE loop's inlined INHERIT_CRITICAL adds (`@31f89d52`; row `31f8af50`'s "not the CREATE loop" is wrong; 14 June uses).
  3. The die spawner's INHERITLOCKONDATA consumer (`@31f83110`; the key is parsed and never read; 12 June uses; the matrix row says `implemented` and is parse-only).
     - It goes through June's `LockOnData::operator=` `31f8c280`, which copies every member except `m_setPreLockOnTarget`.
     - March's implicit copy carries that set. That also affects the CREATE loop's existing lock-on copy (`@31f89e2b`), which C7 switches to the June copy.
- **Checklist 7a:** the `+0xb18` writer `32bb0bd6` and the vslot `0x328` overrides.

**Hygiene** (`flagcheck --range df22a13^..HEAD`):
- SPAWN_INHERIT: `files: 6 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`, `in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`.
- CREATE_EFFECT_SET: `files: 0 PASS, 0 PASS~, 0 DEFINE, 9 OTHER (other flags), 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`.
- **OTHER rows:**
  - `Always.h` carries both defines. Its `tok! 1` is the `#endif` of the two `#if defined(...) #error` dependency guards, which carry no flag token.
  - `X2DamageEffect.cpp/.h` carry both flags.
  - The other seven files only mention SPAWN_INHERIT.
- The 2 deleted lines are `6382401` rewriting C6b's own debug line.
- Census unchanged, as expected: C6b had no key to flip.

**No new play-test** (no code change). The C6b play-test stands.

---

### C6c - Done (2026-10-08, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`, `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL`)

**What landed** (`Always.h`, `X2EffectSet.h/.cpp`, `X2BuffFactor.h`, `X2GUUser.cpp`; the flags need `EFFECTSET_RUNTIME` + `ENCHANT_PARITY`, with `#error` guards):
- **Store:** `EffectSetInstance::m_vecBuffFactorList` (June `+0x160`) of `BuffFactorListEntry { BFI, level, vector<BuffFactorListMod> }`; `BuffFactorListMod { bFinalizer, type, op, value }`. Empty at construction, freed with the instance.
- **Attach:** `CX2EffectSet::AttachBuffFactorToEffectSet( Handle, BFI, level )` (`321e3650`). An invalid or stale handle returns silently. Rose / Add's `321e3730` / `321e3830` are not ported (checklist 7a).
- **Consumer:** `ApplyBuffFactorListToDamageEffect( CEffect*, list )` (`321ee480`), called in `OnFrameMove_CreateEffect` right after C6b's stamp block, on the instance list only (EffectData `+0x118` is dead in June, no call). Per entry: factor by BFI, clone at the entry's level, modifiers, `PushBuffFactor( clone, true )` inside the loop (only the last entry survives: JUNE_BUGS 59, verbatim).
- **Modifiers:** behavior = the existing `JuneSetBehaviorValue` (`32d418a0`, reused); finalizer = new `CX2BuffFactor::JuneSetFinalizerValue` (`32d41960` + `32d40dd0`: type match **and** BFT_TIME, 2 or 3 values, index 0 skipped, op 0 add / 1 multiply / 2 set).
- **JUNE_BUGS 60 (user: guard):** an unknown BFI is skipped and logged once per BFI (`JUNE_FX BUFF_LIST skipped BFI n`, ErrorLog.txt); `#else` keeps June's unguarded line. JUNE_BUGS 61 not ported (no-op).
- **SIEE 15:** `ProcessSocketEffectEx` attaches every `m_bAlwaysPlayEffectSet` entry to the set it plays; the old "unsupported" log is in `#else`, nested under the `SOCKET_PARITY` case.
- **JUNE_FX** (`JUNE_FX_DEBUG`, offline_server.log, throttled 1/s): `ES_CREATE <set> - BUFF_LIST_ATTACH bfi= level= list_size=` and `ES_CREATE - - BUFF_LIST_APPLY entries= kept_bfi=`.
- Docs: `MODS.md` row and section; funcmap rows `321ee480`, `321e3650`, `EffectSetInstance+0x160`, `32d41960`, `32d40dd0` say PORTED; `32d418a0` notes the reuse.

**Build note:** the first compile failed (`g_pData->GetBuffTempletManager()` does not exist; the manager is `CX2BuffTempletManager::GetInstance()`, as the SIEE 16 code already uses). Fixed before any commit.

**Hygiene** (`flagcheck --worktree --files`):
```
files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,381,760 B, dated 2026-10-08 15:05:33, md5 bf609c7b49a69376a2ba4fb4c4be874f, equal to X2/US_SERVICE/x2.exe (2026-10-08 15:05:28); built at mods/june-buff-port 0b33379 + 6 uncommitted code file(s).` The exe holds the strings `BUFF_LIST_ATTACH`, `BUFF_LIST_APPLY` and `BUFF_LIST skipped BFI` (UTF-16, one each). The build also carries a peer's uncommitted `KTDXDeviceManager.cpp`.

**How to see it in game:** nothing reaches the list in March data (no `EffectSet_Hero_Dungeon_Aura_Buff`, no Fever SIEE 13-20 socket in any save), so this is a regression check: skills that make damage effects through effect sets must still apply their own buffs and debuffs, and `grep "BUFF_LIST" offline_server.log ErrorLog.txt` must print nothing. The end-to-end aura check waits for C11 (June effect files), june-buff Phase 13 (the BFIs) and a socket save edit.

**Play-test PASSED 2026-10-08** (run 16:30-16:37 on exe `40b97943`, which contains C6c; user: all checks passed): `jtLordKnight` used all seven equipped skills (Windmill, Sonic Blade, Double Slash, Flame Geyser, Endurance, Armageddon Blade, Fatal Fury) in a dungeon, no visible change; `BUFF_LIST` lines: 0 in ErrorLog.txt, 0 in offline_server.log; 0 EXCEPTION, clean exit, no crash bundle. Next: C6c review.

**Open questions:** none.

### C6c - Review (2026-10-08, Sonnet 5.5 pass, then redone independently on Opus 5.5 high; no code change, no build; exe `40b97943` stays deployed)

**Verdict: C6c is correct against June. No transcription finding.** Checked in the disassembly (`june2017_re/funcdis.py`, read-only):
- **`321ee480` consumer** (`@321ee4dd-321ee58e`): entry = 0x18 B {BFI +0, level +4, mods begin/end +8/+0xc}; mod = 0x10 B {kind byte +0, type +4, op +8, value +0xc}. Per entry: `31ef6990` + `326c32a0` (GetBuffFactorPtr) with **no null test**, `32089f60` clone at `[entry+4]`, then the mod loop, then `321f5130` on effect `+0x200` (DamageData) with bClear = 1 **inside the loop**. Port: same order, `PushBuffFactor( ptrClone, true )` per entry (JUNE_BUGS 59, verbatim); the clone is at the entry's level; March's copy ctor deep-copies `BUFF_FACTOR`, so the template is never modified.
- **Mod kinds not swapped:** kind byte 0 -> `32d418a0( type, value, op )` (`JuneSetBehaviorValue`), anything else -> `32d41960` (`JuneSetFinalizerValue`). Arg order type / value / op matches both pushes.
- **`32d41960` double gate:** `cmp ebp,eax` (type == arg) then `dec eax; jne` (type == 1) -> `type == eType_ && type == BFT_TIME`; `BFT_TIME` is 1 in March's enum (`BFT_NONE = 0`). Port identical.
- **`32d40dd0`:** count = `(end-begin)>>2` of the entry's value vector; `> 3` or `<= 1` returns (so 2 or 3 only); loop starts at 0 with `test eax,eax; je` skipping index 0; op 0 add, 1 multiply, 2 set, else nothing. Port identical.
- **Instance list only:** `@321ef676` (EffectData `+0x118`) is dead in June (C6a2) and not called; `@321ef689` (instance `+0x160`) is the one call, placed right after C6b's stamp block, before June's `321db1b0` neighbours, which touch neither DamageData factors nor each other. The call sits inside `#ifdef RIDING_SYSTEM` like the stamps (ON; recorded, not moved).
- **`321e3650` / SIEE 15** (`@3202a1aa-3202a1f4`): June skips on handle -1 (`@3202a1a5`), loops `+0x29c` entries (0x10 B), tests byte `+9` (`m_bAlwaysPlayEffectSet`), calls `321e3650( handle, [+0], [+4] )` = (BFI, level). Port: INVALID handle `continue`s before the loop, same test, same args; `GetEffectSetInstance` returns NULL for a stale handle, as the decode does. The old "unsupported" line sits in `#else`, inside the `SOCKET_PARITY` case.
- **JUNE_BUGS 60 guard (user: guard):** a null `GetBuffFactorPtr` skips the entry, logs once per BFI, `#else` keeps June's unguarded line. One consequence: BFI 0 (which `GetBuffFactorPtr` returns null for silently) is also skipped and logged once; June would crash on it. No change.
- No June offset is ported (store is a typed `std::vector<BuffFactorListEntry>`).
- **Opus pass, added checks:**
  - **`32d418a0`, reused from buff 15-2b:** the first behavior with type == arg and >= 2 values gets value[1] += / *= / = (op 0/1/2), then returns; any other op also returns; a type match with < 2 values moves on to the next behavior. `JuneSetBehaviorValue` does the same.
  - **`32089f60`** is new 0x70, refcount 0, `320844a0( src data, level )`, then the identity `+0x68/+0x6c` is copied. That is March's `CX2BuffFactor( rhs, uiLevel )` (deep copy plus `m_DataBuffIdentity`).
  - **`321e3650`:** handle -1 returns. It checks the top 2 bits against the set id `+0x40`, the low word < the slot count, a non-null slot and the stamp word. Then it pushes {BFI, level, empty mod vector}. March's `GetEffectSetInstance` (`DecomposeHandle`, index, stamp) rejects the same handles.
  - **`321f5130`:** with bClear set, the old factors are erased and `326309f0` (PushSocketAndTemporaryBuffFactor) runs. Then `32d4d670( attacker, -1, -1, 1 )` = March's `SetGameUnitBuffFactor( attacker, iCreatorUID = -1, bIsUserUID = true )` defaults (`ADD_LIVE_CREATOR_FINALIZER` is ON). Then a push. March's extra clone carries the modified data over unchanged.
  - **Instance lifetime:** March creates every `EffectSetInstance` with `new` (`X2EffectSet.cpp:3238/3338/3445/3517`) and deletes it; nothing pools or resets it, so the list always starts empty.
  - **Order:** March runs the list after its velocity / IS_RIGHT blocks and before SetLockOn. June runs SetLockOn, the stamps, the list, then `321db1b0` and IS_RIGHT. None of those neighbours reads or writes `m_vecBuffFactorPtr`, and nothing else in `X2EffectSet.cpp` does either, so the order is equivalent.

**Hygiene** (`flagcheck --range 496cd83^..496cd83`):
```
EFFECTSET_BUFF_LIST alone: files: 4 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 1 ERROR   RESULT: FAIL
  (the ERROR is Always.h:6050, the FIX flag's own `#if defined(FIX) && !defined(BUFF_LIST)` #error guard: the tool cannot evaluate it, the same class as C6b/C7's)
with --with SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL: files: 4 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR   RESULT: PASS
  in this flag's files: 0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)
FIX_EFFECTSET_BUFF_NULL alone: files: 0 PASS, 0 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR   RESULT: PASS   (its code lives in X2EffectSet.cpp's nested block; counted under the closure)
```
Net 0 deleted lines. The two flags are checked as a closure because the FIX flag cannot exist without the first.

**Not verifiable by play-test:** nothing in March data reaches the list (no aura set, no Fever socket), so the end-to-end path (attach, apply, the BFI null guard) waits for C11 + june-buff Phase 13 + a socket save edit. The regression test already passed on `40b97943`.

**Open questions:** none. Next in this plan: `/x2-phase june-effect C8b` (Sonnet 5.5 / high) or `C9h-4e`; both kickoffs exist.

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

### C7 - Done (2026-10-07, Opus 5.5 high (kickoff named Sonnet 5.5; the user chose to continue); flag `SERV_IRUHADEV_JUNE_TOUCH_LAND`, plus `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` and `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME` extended)

**What landed** (`8973e18`; `KTDGXMeshPlayer.h/.cpp`, `X2DamageEffect.h/.cpp`, `X2GameUnit.h`, `Always.h`):
- **Mesh** (`TOUCH_LAND`): `CXMeshInstance::m_vVelocityBefore` (June +0x54c): ctor 0 (`@31c8d718`), snapshot `= m_vVelocity` in OnFrameMove's X2OPTIMIZE block before `m_fAniTimeBefore` (June `@31c87cac`, the same block, after the per-frame-simulation return), `GetVelocityBefore()`. `GetGravity()` added; m_Gravity had only a setter. No June offset is read.
- **Gate** (`TOUCH_LAND`): in `CEffect::OnFrameMove` right after the die check, `GetGravity().y < 0 && GetVelocityBefore().y < 0 && GetVelocity().y > 0` -> `JuneTouchLandDamageEffect()` (31f83380). No once-flag. June's `GetLandPosition` probe (`@31f8a2a4-31f8a2e3`) is not repeated: its results are unread and March's `GetLandPosition` is `const`. 31f83380's scratch copy (31f8e750, a funcmap candidate) is not observable, since the spawner and tail only read entries. March therefore spawns from `m_vecTouchLandDamageEffect` directly.
- **Spawner** `CreateTouchLandDamageEffect` (31f81ba0), every block of `dis_31f81ba0.txt` in June order:
  - guard chain, invoke-rate skip (March's hit test), rotation = `m_AngleDegree` under APPLY_ROTATE_ORIGIN else 0, position = `m_BeforePos`, rotated/plain offset;
  - `CreateInstance( m_fLandPos as is, force-down = inherit ? m_fForceDownRate : 1 )`, crit adds, `JuneInheritFromParent`, buff factor, life time, hit damage rate, `SetScale( entry * m_vHitDamageScale )`, `SetScaleByUnit( m_vHitDamageScaleByUnit )`, `SetInheritForceDownRate`, `SetMaxLifeTime` (a plain store, as June's +0x44c write), first-attack (`ADD_RENA_SYSTEM`, off), `JuneSpawnTail`.
- **C6b-review gap 1** (`SPAWN_INHERIT`): `m_fJuneCreateDamageEffectRate` (+0xb18, -1 after LINK_EFFECT_RATE), then `pGameUnit->JuneOnDamageEffectCreated( this )` (new base virtual, vslot 0x328, no-op), `JuneApplyCreateDamageEffectRate` (31f6ea20) after `JuneInheritFromParent` in RT_CREATE_DAMAGE_EFFECT. The `(int)` truncation is kept: JUNE_BUGS 103.
- **Gap 2** (`DAMAGEEFFECT_RUNTIME`): the CREATE loop's inlined crit adds, right before its `JuneInheritFromParent`.
- **Gap 3** (`DAMAGEEFFECT_RUNTIME`): `JuneAssignLockOnData` (31f8c280: swap the destination's `m_setPreLockOnTarget` out, implicit assign, swap back). It is used by the die spawner's INHERITLOCKONDATA (after the first-attack block, before the tail; no type kept or tested) and by the CREATE loop's copy (`#else` keeps March's `=`). **The loop change is live in March:** a lock-on CREATE child no longer inherits the parent's hit list.

**Bookkeeping:** funcmap rows 31f83380, 31f81ba0, mesh+0x54c, 31f6ea20, CEffect+0xb18, 3208ddc0, 31f88790 @31f89d52, 31f82210 @31f83110 and 31f8c280 are tagged `Effect C7: PORTED`. `census.py`: the 7e TOUCH_LAND rows are now `C7 done` (APPLY_ROTATE and PASSIVE_PARTICLE_* are `-`: 31f81ba0 never reads them), a new `DE.DIE_DAMAGE_EFFECT INHERITLOCKONDATA` runtime row, and the INHERIT_CRITICAL rows name the CREATE loop and the touch-land spawner. The matrix is regenerated (census.py: every check PASS, matrix and census.tsv rewritten; the rest of the matrix diff is re-cited X2DamageEffect line numbers). MODS.md has a table row and a C7 section.

**Hygiene** (`flagcheck --worktree`): TOUCH_LAND `files: 2 PASS, 3 OTHER, 0 FAIL`; SPAWN_INHERIT `1 PASS, 4 OTHER, 0 FAIL`; DAMAGEEFFECT_RUNTIME `1 PASS, 4 OTHER, 0 FAIL`. Each has 0 deleted lines, 0 non-ASCII, 0 EOL-mismatched and 0 encoding changes, `RESULT: PASS`. The OTHER rows are files that carry the other two flags. All three stripped together (`--with`): `3 PASS, 1 PASS~, 1 OTHER, 0 FAIL`. The PASS~ is one blank separator line after the new `JuneAssignLockOnData` `#endif`. Range `--range 8973e18^..HEAD`: TOUCH_LAND `2 PASS, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS` (its one bare `#endif` is the `#error` guard's, the house pattern). SPAWN_INHERIT and DAMAGEEFFECT_RUNTIME: `1 PASS, 4 OTHER, 0 FAIL, 1 ERROR`, 0 deleted. Each ERROR is an Always.h dependency guard the tool cannot evaluate (`#if defined(..TOUCH_LAND) && !defined(..SPAWN_INHERIT)` at 5628; C6b's own guard at 5362), not a code file.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,333,120 B, dated 2026-10-07 20:49:08, md5 e59da04f4d517917130377696b3f3fd0, equal to X2/US_SERVICE/x2.exe (2026-10-07 20:49:02); built at mods/june-buff-port 762aae6 + 6 uncommitted code file(s).` (the six are this phase's, committed as 8973e18). The strings `TOUCH_LAND_GATE`, `CREATE_DAMAGE_EFFECT_RATE`, `CREATE_DAMAGE_EFFECT_INHERIT`, `INHERITLOCKONDATA`, `CREATE_LOCKON_COPY` are in the exe.

**How to see it** (`python tools/logs.py --grep "JUNE_FX CE_HIT"`; offline_server.log, at most one line a second per effect and key):
- `TOUCH_LAND_GATE entries=0 gravity.y=… velocityBefore.y=… velocity.y=…`: any March damage effect whose main mesh bounces off land, e.g. the `Bomb`/`Apple` meshes (`BOMB`, `APPLE`, `POISON_POCKET`), Chung's `HandGrenade1` (`CHUNG_ELASTIC_BOMB`), Raven VC's napalm / back-slide grenades, `NasodDrillerDynamite*`. This is the only C7 line March data can produce. It proves `m_vVelocityBefore` works.
- `TOUCH_LAND`, `CREATE_DAMAGE_EFFECT_RATE`, `CREATE_DAMAGE_EFFECT_INHERIT`, `INHERITLOCKONDATA` cannot fire on March data (0 users; the rate is class-set). Seeing one is a finding.
- `CREATE_LOCKON_COPY parentPreLockOn=N childKept=0`: a lock-on CREATE child whose parent had already hit someone. This is the live change.

**Play-test (2026-10-07, logs read): regression passed.** The run used exe e59da04f. 44 `JUNE_FX CE_HIT` lines, all `SPAWN_INHERIT` / `SPAWN_TAIL`: arrow boom, Delayed Firing (index 1, 4, 7 carried from DELAY to GROUND to BOOM), Explosion Trap (fly, ground, bomb) and Trapping Ranger entangle chains. Gates 0, rate 1.000, 0 EXCEPTION, 3 UNHANDLED (the known `E_DISABLE_HB_CHECK_REQ`). None of the C7 lines appeared. For `TOUCH_LAND`, `CREATE_DAMAGE_EFFECT_RATE`, `CREATE_DAMAGE_EFFECT_INHERIT` and `INHERITLOCKONDATA` that is expected. `TOUCH_LAND_GATE` and `CREATE_LOCKON_COPY` were not reached: nothing bounced, and no lock-on CREATE child spawned from a parent that had already hit. Those two stay unobserved; optional for the review.

**Open for the user:** none. The kickoff's "find a June templet with TOUCH_LAND_DAMAGE_EFFECT" has no March answer: June's only user is Rose's `DamageEffect_A_RMH_KS_83_Grenade`, and its owner is the character port or June-data re-adoption (C11).

### C7 - Review (2026-10-07, Opus 5.5 high)

**Verdict: correct against June, with one transcription fix.** The fix is NaN polarity in `31f6ea20`, under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`. It changes nothing on March data.

**Checked against June** (raw disassembly via `disasm_range.py` / Ghidra, read-only):
- **Gate** `@31f8a2e8-31f8a38b`: `0 > gravity.y`, `0 > velocityBefore.y` (`test ah,0x41; jne`), `0 < velocity.y` (`test ah,5; jp`). All strict, and NaN skips, as ported. No once-flag.
- **Mesh** `31c87b80 @31c87c8e-31c87cc4`: the `+0x534..+0x53c -> +0x54c..+0x554` snapshot sits in the `m_fAniTimeBefore` block, after the per-frame-simulation return. March places it the same way, and no June offset is read. `X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION` is ON (`flagprobe.py`).
- **Spawner** `31f81ba0` (`dis_31f81ba0.txt`), traced block by block:
  - guard chain; invoke-rate test (`+0xb04` != 0 = type != `DET_ARROW_OF_EXPLOSION` (0); random > `+0x720` skips);
  - `+0x74` angle `+0x118` into both angle vectors; position `+0x10c`;
  - `+0x75` rotated offset (the z branch's direction `(-dz, 0, dx)` is March's swap) or the plain offset;
  - CreateInstance's 17 arguments `(owner, name, +0x1d8, pos, rot, axis, +0x130, 0, -1, forceDown, 1, -1, -1, 0, -1, 0, 0)`: March's overload with its own defaults;
  - then `31f8af50`, `31f8fdb0`, buff factor, life clock `+0x448`, rate `+0x724` > 0, scale `entry x +0xa5c`, `SetScaleByUnit(+0xaa0)` (= `m_vHitDamageScaleByUnit`: `31f76100` writes `+0xaa0`/`+0xaac`), `+0xacd`, max life `+0x84` > 0, first-attack, tail.
  - Every guarding macro is ON except `ADD_RENA_SYSTEM` (OFF, as in the hit and die spawners).
- **Ctor hook** `@31f7f612-31f7f62b`: `+0xb18 = -1.0`, then the ctor-argument owner's vslot `0x328`, when that owner is non-null.
- **RT_CREATE** `@31f86140-31f86148`: `31f6ea20` runs unconditionally after `31f8fdb0`.
- **CREATE loop** `@31f89d52-31f89d82`: the crit adds sit between SetScale and `31f8fdb0`. They are inside C7's existing `DAMAGEEFFECT_RUNTIME` block (`#ifdef` at cpp :2918).
- **Lock-on copy** `@31f89e02-31f89e34`: both types non-zero; the child's type is saved, then operator=, then the type is restored.
- **Die copy** `@31f83110-31f83126`: entry `+0x9c` == 1, then operator=. There is no type test or keep, and it sits between first-attack and the tail.
- **`31f8c280`**: copies `+0x0..+0x3c` (not the padding at `+0x14`), **skips `+0x40..+0x4f`** (the set), then copies `+0x50..+0xc0` with the wstring at `+0x8c`. `JuneAssignLockOnData`'s swap/assign/swap-back is exactly that.
- **`31f6ea20`** body: the type switch, HIT_ADD_MP `*=`, TECH_POINT `*= ftol(rate)`, with ST0 = the rate on both paths. JUNE_BUGS 103 is present.

**Fix:** the guard (`@31f6ea3a-31f6ea49`: `fcom` rate vs 0, `test ah,0x41`, `jnp return`) returns on `< 0` and `== 0` only. **A NaN rate goes on.** This was checked by JIT-running the exact x87 bytes (scratch harness after `x87test.py`): -1 and 0 return; 0.5, 1 and NaN proceed. C7 wrote `false == ( rate > 0.f )`, which returns on NaN. It is now `m_fJuneCreateDamageEffectRate <= 0.f` (X2Lib has no `FloatingPointModel`, so `/fp:precise` keeps NaN false). funcmap row `31f6ea20` is corrected.

**Hygiene:**
- Range `8973e18^..HEAD`, single flag: TOUCH_LAND `files: 2 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`. Its one bare `#endif` is the `#error` guard's.
- SPAWN_INHERIT and DAMAGEEFFECT_RUNTIME, single flag: `1 PASS, 4 OTHER, 1 ERROR`, `RESULT: FAIL`. Each ERROR is an Always.h `#error` dependency guard (`:5628`, `:5362`). A single-flag strip of either one cannot compile by design, so they are checked over the dependency closure instead:
  - SPAWN_INHERIT `--with JUNE_TOUCH_LAND`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s) ... 2 #else/#endif without the flag token` (the two guards' bare `#endif`s); `RESULT: PASS`.
  - DAMAGEEFFECT_RUNTIME `--with JUNE_SPAWN_INHERIT JUNE_TOUCH_LAND`: `files: 1 PASS, 1 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s)`; `RESULT: PASS`. The PASS~ is C7's blank line after `JuneAssignLockOnData`.
  - This is a known limit: any later range that touches Always.h gets the same single-flag ERROR for these flags. Use the closure.
- Worktree after the fix: SPAWN_INHERIT `--files X2Lib/X2DamageEffect.cpp` `1 PASS, 0 FAIL, 0 ERROR`. Its `1 deleted line(s)` is C7's own replaced condition line inside the flag's block. `RESULT: PASS`.
- `census.py` moved as the Exit says: the TOUCH_LAND runtime rows read `C7 done`, with the INHERITLOCKONDATA row added. The matrix rows are corrected.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,333,120 B, dated 2026-10-07 21:13:12, md5 4438a92347b2041b5dce62e89116ec5c, equal to X2/US_SERVICE/x2.exe (2026-10-07 21:13:07); built at mods/june-buff-port a8758a3 + 1 uncommitted code file(s).` The one file is this fix, committed with this note.

**Exit, third leg: not met.** "A `JUNE_FX TOUCH_LAND` line fires in play on a June damage effect that uses it" cannot be met on March data. June's only user is Rose's `DamageEffect_A_RMH_KS_83_Grenade`. The leg, plus the still-unobserved `TOUCH_LAND_GATE` and `CREATE_LOCKON_COPY` lines, is carried to `kickoffs/june-effect/C11.md` ("Added by the june-effect C7 review"). The user may instead stage a temporary touch-land entry now (the C4c precedent); that is their call.

**Play-test:** none required; the fix is inert (rate -1). The optional check: throw a bomb or Chung's Elastic Bomb and look for `TOUCH_LAND_GATE entries=0`.

**Play-test (2026-10-07, logs read): the optional check passed.**
- Run on exe 4438a923 (started after the deploy); 0 EXCEPTION; 2 UNHANDLED, both the known `E_DISABLE_HB_CHECK_REQ`.
- 48 `JUNE_FX CE_HIT` lines: 17 `TOUCH_LAND_GATE`, 18 `SPAWN_INHERIT`, 13 `SPAWN_TAIL`.
- Every gate line is `entries=0 gravity.y=-2000.0` with `velocityBefore.y < 0 < velocity.y`, on `CHUNG_ELASTIC_BOMB` (e.g. -671.6 -> 560.2) and `HAND_GRENADE_0` (e.g. -1400.0 -> 700.0). So `m_vVelocityBefore` and the gate work on real bounces.
- None of `TOUCH_LAND`, `CREATE_DAMAGE_EFFECT_RATE`, `CREATE_DAMAGE_EFFECT_INHERIT`, `INHERITLOCKONDATA` or `CREATE_LOCKON_COPY` appeared, as expected on March data.
- What is still open, carried to C11: the `TOUCH_LAND` spawn on June data and `CREATE_LOCKON_COPY`.

---

## C8 - Hit path, second half (old Phase 9)

**Status: C8a done 2026-10-06; C8b done 2026-10-09 (play-test passed except UnitToEffect; review PASS, comment-only citation fix); C8c (per-class) handed off to the character ports (D8).** Phase 9 ported June's collision for `EffectToUnit` only.
**Model:** Opus, high, for the comparisons; Sonnet 5, high, for the edits.

- **Collision:** switch `UnitToUnit` (`32611970` inlines the same code), `UnitToEffect`,
  `MeshToUnit` and `CollisionDataToUnit` to June's semantics, under `SERV_IRUHADEV_JUNE_COLLISION`.
  Confirm each one's June counterpart first.
- **`SetDamageExceptionProcess` (`SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`):**
  - compare and port the eight per-class overrides in `funcmap.tsv`: Add, Elesis, Lire and Arme
    have March counterparts; Eve, Chung, Raven and Elsword are June-only;
  - port the base and user layers' June additions: the `AT_COMMAND` MP-gain multiplier, and the
    `APPLIED_SKILL_ID` hooks, reading `DamageData::m_iAppliedSkillID` (stored since Phase 8c; D7 took C1
    out of this plan, so do not wait for it). Port them as June has them, even where they stay inert for
    March skill IDs;
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

### C8a - Done (2026-10-06, Opus 5.5 high; no code, no flag, no build)

**What landed:** 24 funcmap rows tagged `Effect C8a`. Rows `329bc560`…`32d1e0e0` move from candidate to confirmed, each with its verdict. 16 rows are new: the hit paths, the user-layer items, the helpers, and `322a2700`, which is the `CX2GameUnit` **ctor** (row 901 had it as the dtor). Checklist section 7b is new.

**Hit paths: all four switch to June's math.** In every pair test, each normalize is followed by the ×0.5 halfway impact. March's `LineToSphere` follows its normalize with a dot product instead. The advance branch (`31c7b640`) exists only in EffectToUnit.

| path (March site) | June | shape | guard |
|---|---|---|---|
| UnitToUnit (`CollisionDataCheckFromUnitToUnit` `:3979`) | `32611970` | 3 × listSet×listSet (attack `+0x214`, sub `+0x224`, pair subs); fresh flags each; returns on a hit or on guard-only | yes |
| UnitToEffect (`:2688-2700`) | `326413c0` | 3 × listSet × the effect's list | yes |
| MeshToUnit (`:2827`) | `32625670` | list×listSet: EffectToUnit's template, advance off (83% aligned) | yes |
| CollisionDataToUnit (`:3061`) | `32628890` | defender set (A) × manual datum (B), March's order; halfway from A | **no** (as March) |

**C8b port list** (Sonnet; depends on C8a only):
1. The four paths above, under `SERV_IRUHADEV_JUNE_COLLISION`. Generalise `JuneEffectCollisionDataCheck` to take an attack list set, or loop it, with advance off. Add a listSet × datum variant without the guard. March's call stays in the `#else`. COLLIDE/DIFF lines gain a path tag. The traced `TEST` line gains the owner UID and the effect's age (T6).
2. `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`, user layer `32045340`:
   - the `+0x4108` member, set to 2 in the ctor (`@322a5bf7`) and to -1 in `ResetDamageType` (`@32c438a5`);
   - the gate `@320453a0`: -1, or equal to `attackType`.
   Inert on March data: only Raven's 24401 path writes 2, and that path belongs to the Raven port (checklist 7b).
3. The user vslot `0x758` hook `3202ac40`, as June has it. `31fdbbb0` is a rule-5 stub: it returns null and logs `JUNE_FX unsupported skill memo` once. Cases whose DD offset has no March member are skipped, with a comment naming the offset. Call order in the user layer: Raven block, then `0x758`, then (the socket copy, not ported), then `0x780`.
4. The NPC override `32759f30`: base, then the master's `0x758` (row 196's conditions).
5. An empty `CX2GUUser` vslot `0x780` hook `(DD, fPowerRate)`, called last. Only Ara overrides it (Ara port, D8).
- **Done already, do not re-add:**
  - AT_COMMAND 85/86 (`_JUNE_BUFF_NEW_STAT`);
  - `GetChargeMpRateByBuff` (`_NEW_SKILL`);
  - the state and serial tail (`_CUSTOM_FUNCTION`).
- **Not ported:**
  - DD `+0x638` = vslot `0x2e0` (always -1; reader unknown);
  - the socket copy `@32045458` to DD `+0x3b4`, which belongs to **june-buff 16-step3c** (owner of the 18 June-only EDTs; item added to its kickoff): it carries only June EDT 150/159, which 16-data drops.

**C8c port list: handed off to the character ports by D8 (2026-10-06); kept as their brief.** Each port needs C5 (`ATTACK_TRIGGER_TYPE` `+0x53c`, `32610fb0`). Port the blocks citing the addresses in each class's funcmap row:
- Eve `329bc560`; Chung `32bf28f0`; Raven `32c5c080`; Elsword `32d1e0e0`;
- Elesis `@32b81008-32b811ef`;
- Rena's additions (all but Bowstring and Sharp Kick);
- Aisha's Cyclone (22011/22051);
- Ara's vslot `0x780` `32bca010`.

Every gate is a June reform skill (`RNSI_*`, absent from March's enum). The helpers are in the new rows: vslot `0x238` `32028af0`, PushBuffFactor `329cf6d0`/`322b5320`, ability getters `32007ce0`/`32007c40`, `32610e90` (`32d40dd0` still to decode).
- **Identical to March, nothing to port:**
  - Add `329e4a50` (`ProcessBodyOfTranscendence`);
  - Elesis's Way of the Sword `@32b80f33-32b81001`.
- **D8:** Rena's Bowstring/Sharp Kick and Aisha's Fitness → Magical Staff.

**T6, far TEST samples: not settled; the 2026-10-03 log is overwritten.**
- Ruled out: the near group's fireball.
  - x stays at 12818-12842 across samples at least 1 s apart, while a live fireball moves about 500 a second.
  - y is 1263-1526, about 1300 below the land height 2788 where every shot crash-lands (`MP_CRASH`).
- Left:
  - a second charger on a lower, deeper layer (the throttle keys on the effect name, so chargers mix);
  - or a mesh that outlives its crash-landing.
- C8b's trace (owner UID, age) and its hand-off ("how many Mag Chargers?") settle it.

**C8b exit, a trigger for each path** (March sites):
- MeshToUnit: Aisha `EvasionStandupAttack` / `ComboX3down` (`AT_MESH`, `X2GUArme_VioletMage.cpp:8036/9279`);
- CollisionDataToUnit: Aisha `ComboX3front` (`:9074`), Raven `COMBO_ZZZX` (`X2GURaven.cpp:7945`);
- UnitToEffect: melee on an enemy's `ATTACKED_BY_UNIT` effect (221 templets in `DamageEffect.src.lua`);
- UnitToUnit: any melee.

### C8b - Done (2026-10-09, Sonnet 5.5 high; code, play-test owed)

**What landed** (flags `SERV_IRUHADEV_JUNE_COLLISION` extended, `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` new; MODS.md entries "C8b"):
- **Four hit paths** on `EffectToUnit`'s pair math, advance off (`X2DamageManager.cpp`). `JuneEffectCollisionDataCheck` is untouched; three small drivers sit under it:
  - UnitToUnit `32611970` and UnitToEffect `326413c0`: the attacker's three instances (attack set, sub set, pair sub sets), one run per attack list. The result equals June's single pass (a guard seen first is what a later hit reports anyway). UnitToEffect wraps the effect's list in a one-list set.
  - MeshToUnit `32625670`: the attacker anim's list x the defender's set, `pAttackerAnim != NULL` kept.
  - CollisionDataToUnit `32628890`: defender set (A) x the manual datum (B). The type reported is B's: `MOV EBX,[EBX+0x20]` @3262b23e is DD `+0x1c`'s type, as March's. No guard call; the caller maps `CT_GUARD` to `m_bGuard`. Run as defender lists x a one-datum set (identical result even for a `CT_GUARD` B: the first hit point, type `CT_GUARD`).
  - Trace: `JUNE_FX HIT UNIT_TO_UNIT | UNIT_TO_EFFECT | MESH_TO_UNIT | COLLISION_DATA_TO_UNIT COLLIDE | DIFF` (March's test run on the same pair); `EffectToUnit`'s lines gain `path=EFFECT_TO_UNIT`, TEST gains `owner=<uid> age=<now>/<max>` (T6).
- **`+0x4108`**: `CX2GameUnit::m_iJuneChangeDamageAttackType` (ctor 2, `ResetDamageType` -1) and the user-layer gate `@320453a0-320453b5`; `SetHitNasodWeapon( false )` stays before it. Inert: Raven's reset leaves -1.
- **`0x758` hook** `CX2GUUser::JuneApplySkillMemoToDamageData` (`3202ac40`, all cases re-read from the raw disassembly) and the rule-5 stub `GetJuneSkillMemoEntry` (`31fdbbb0`: returns none, logs `JUNE_FX unsupported skill memo` once). Skipped, each with a comment: `+0x4f8`, `+0x5d0`, `+0x5dc`, `+0x5e4`, cases 0xf/0x10. The `APPLIED_MEMO_ID` test reads C3's `m_iAppliedMemoID`; C3's "unsupported" log for that key is compiled out under the new flag.
- **Call order** in the user layer, user attackers only (June's early-outs `@32045359-3204538c` skip both calls): Raven block, `0x758`, the socket copy (already in under `_SOCKET_EDT`), `0x780`. **`0x780`** = empty `JuneDamageExceptionClassHook( DD, fPowerRate )`, last.
- **NPC `32759f30`**: `CX2GUNPC::SetDamageExceptionProcess` = the base, then the owner's `0x758` (live user owner, CX2Unit and unit templet set, type != 0).
- **No damage number moves**: every `fPowerRate` multiply is unchanged. UnitToUnit is live on every melee hit, so hit/miss and the impact point can change on line x sphere pairs (as Phase 9 did for effects).
- C3 review note: `+0x5d0` is not mapped and the CEffect scale base is not ported, so nothing resets a scaled child; the switch cannot run behind the stub anyway.
- **JUNE_BUGS 144** (memo cases 5/6 fall into the shared tail with v1) and **145** (June reads an NPC owner's `+0x867c` with no user test; the port skips a non-user owner, the one deviation).

**Hygiene:** `flagcheck JUNE_DAMAGE_EXCEPTION --with JUNE_COLLISION --worktree --files <my files>`: RESULT PASS, 6 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 `#else`/`#endif` without the token. The 6 deleted lines are Phase 9's own TEST/COLLIDE/DIFF trace lines (inside the flag and `_JUNE_FX_DEBUG`), rewritten for the path tag, owner and age.
**Deploy:** DEPLOY OK: X2_offline.exe is 19,497,472 B, dated 2026-10-09 18:00:11, md5 153e7bab5aca8b64f82c514f7c837cc5, equal to X2/US_SERVICE/x2.exe (2026-10-09 18:00:06); built at mods/june-buff-port a39a4ce + 27 uncommitted code file(s) (peers' work is in that exe too). The six trace strings and `JUNE_FX unsupported skill memo` are in the exe.

**How to see it in game** (full spec in the session hand-off; log read with `python tools/logs.py --grep "JUNE_FX HIT"`). Place: Ruben field (40000), console `END`, `auth_level` 3. Setup: `/invin 0` (invincible, no MP cost). Corrections to the C8a/kickoff trigger list, found while writing the spec:
1. UnitToUnit: `/CM "LOW_LIZARDMAN_WARRIOR" 0 70` (Lizardman Commander), any melee hit -> `UNIT_TO_UNIT COLLIDE`.
2. UnitToEffect: `/CM "LOW_LIZARDMAN_SHAMAN" 0 70` (Lizardman Healer, state `LOW_LIZARDMAN_SHAMAN_MAGIC_MISSILE`), melee its `LIZARDMAN_SHAMAN_MAGIC_MISSILE` projectile -> `UNIT_TO_EFFECT COLLIDE`. **The kickoff's `data037 Lizardman_Shaman.lua` does not use that templet** (it uses `DamageEffect_LIZARDMAN_SHAMAN_ATTACK_A`); the users are `Low_Lizardman_Shaman`, `BES_SHAMAN`, `Lizardman_Shaman_Dark`.
3. MeshToUnit: `jtVoidPrincess` (uid 28, Aisha / Void Princess lv 80): X, X, then X with Down held = `AVSI_DARK_MAGICIAN_COMBO_X3down` (`AT_MESH`, `X2GUArme_VioletMage.cpp:9365`). **`ComboX3down` / `EvasionStandupAttack` are not the Void Princess's states** (the former is registered for Violet Mage / High Magician / Elemental Master only).
4. CollisionDataToUnit: the same character, X, X, then X with forward held = `ComboX3front` (`:9074`, Void Princess is in its class list); or `ravbe` (uid 23, Raven / Reckless Fist lv 67): Z, Z, Z, then X before the third swing ends = `RSI_COMBO_ZZZX` (`X2GURaven.cpp:7945`).
5. T6: `/CM "MAG_CHARGER" 0 70` in Ruben field; the `TEST` lines now carry `owner` and `age`. Ask: how many Mag Chargers were on screen?

**Play-test 2026-10-09: PASSED, except UnitToEffect (skipped by the user).** Logs read (`JUNE_FX HIT`): `UNIT_TO_UNIT` COLLIDE for `jtVoidPrincess` (uid 28) and `ravbe` (uid 23); `MESH_TO_UNIT` x2 (Void Princess, `AVSI_DARK_MAGICIAN_COMBO_X3down`); `COLLISION_DATA_TO_UNIT` for both characters; 0 EXCEPTION. T6: 60 Mag Charger `TEST` lines, one owner (1000027), age <= 1.03 of 2.00, hitbox centre = mesh position on every sample, so the far samples did not reproduce with one charger. **`UNIT_TO_EFFECT` never fired:** the Lizardman Healer would not throw its missile (invisible, 1 s life, 700 units/s), and the console `BOMB` spawn did not work, so the user skipped the check as too conditional. The path is verified by code only; the C8b review must read it against `326413c0` (and may use the `UNIT_TO_UNIT` DIFF lines below, which run the same instance driver). **DIFF lines:** 3 x `june=0 march=1` and 2 x `june=1 march=0` on `UNIT_TO_UNIT` (both directions, player and monster attackers); the review checks they are line x sphere cases.

### C8b - Review (2026-10-09, Opus 5.5 high)

**Verdict: PASS. The logic is correct against June. One comment-only fix:** the memo-switch citations for cases 0xd and later in `X2GUUser.cpp` were wrong. The case bodies were right.

**Hit paths (Ghidra + raw disassembly):**
- **UnitToUnit `32611970`:** the attacker sets `+0x214`, then `+0x224` (the flags are re-zeroed @326146b2), then the pair sets. Each instance has one guard call (`32640770` @326145b1). It returns on a hit (`[esp+0x34]`) or on guard-only (@32614646). The port's per-attack-list driver gives the same result.
- **UnitToEffect `326413c0`:** the same three instances (`+0x214` @3264171d, `+0x224` @326442fc) against the effect's list, with 3 guard calls. Read by code only, as the C8b note asked; it shares its driver with UnitToUnit.
- **MeshToUnit `32625670`:** one instance, 4 halfway multiplies, guard present.
- **CollisionDataToUnit `32628890`:**
  - A = the defender datum (ESI, from unit `+0x204`), B = the manual datum (EBX). A is tested for `CT_LINE` first, then B, as in the template.
  - **line A x sphere B: the halfway point starts from B's centre.** `[esp+0x9c]` is written from EBX `+0x74..` @3262a529-3262a630; the impact is built @3262a913-3262a96c. **sphere x sphere starts from A** (`[esp+0x34]`, ESI's point @3262903a; @3262b1a9-3262b1fc). Both match `JuneEffectCollisionDataCheck`.
  - The C8a table's "halfway from A" holds only for sphere x sphere and sphere x line.
  - There is no guard call. The first hit stores B's type (@3262b23e) and sets `m_bGuard` for type 2 (@3262b26b), as the port claims.
  - The line x line `0.5` (@32629a13) is the parallel-segment midpoint, which March has too (`KTDXCollision.h:2035`).

**SetDamageExceptionProcess:**
- **User layer `32045340`:**
  - **Gate:** `+0x4108` (@320453a0) is -1 or `== DD+8`, after `SetHitNasodWeapon(false)` (`+0x8b62` @320453a6).
  - **Calls:** 0x758 is called on the attacker for both gate outcomes (@32045402); the socket copy follows; 0x780 is called on `this` (@32045488).
  - **Early-outs:** they jump past both calls (@32045496).
- **`+0x4108`:** the ctor writes 2 (@322a5bf7) and Raven's inlined reset writes -1 (@32c438a5). March's only `ChangeDamageType` caller (`X2GURaven.cpp:5111-5115`) resets first, so the gate is inert.
- **0x758 `3202ac40`:**
  - The jump table was decoded from the binary (byte index @3202b0ec, targets @3202b0b8): 1/4 -> acd9, 2 -> ad03, 5 -> ad23, 6 -> ad39, 8 -> ad49, 0xc -> ad6d, 0xd -> ad7d, 0xf -> af87, 0x10 -> afbc, 0x11 -> aff1, 0x12 -> b088, 0x16 -> b0a2, any other type -> return.
  - Cases 5 and 6 enter the tail at acdd with v1; v2 is 0 when the entry has two values or fewer. Every body matches the port.
  - **Fixed (comments only):** the citations for 0xd (`-af84`, not `-af17`), 0xf/0x10, 0x11, 0x12 and 0x16 pointed at the middle of case 0xd.
- **NPC `32759f30`:** the base runs, then the `+0x8d5c` owner's `+0x867c` and `+0xaf0` are checked, `[templet] != 0` (= `m_UnitType != UT_NONE`), then 0x758 with EDI = the DD (@32759fc4). The port matches, with one difference: it skips a non-user owner (JUNE_BUGS 145).
- **0x780:** an empty virtual, called last.
- **Nothing added twice:** C8b does not touch AT_COMMAND 85/86, `GetChargeMpRateByBuff`, the custom-function tail or the 16-step3c socket copy. The copy sits between 0x758 and 0x780, where June has it.
- **JUNE_FX HIT lines:** all four paths have COLLIDE and DIFF lines, and EffectToUnit's lines carry `path=` / `owner` / `age`.

**UNIT_TO_UNIT DIFF lines (judged by code; the logs are gone):** a later play-test rewrote `ErrorLog.txt` at 19:45, and it now has 0 `JUNE_FX HIT` lines, so the 5 DIFF lines can't be re-read. Line x line and sphere x sphere are the same test in both. Only the two clamped pair tests can disagree:
- **`june=1 march=0`:** a sphere behind the segment start. March's `LineToSphere` rejects a projection < 0; June clamps it to the start point.
- **`june=0 march=1`:** a sphere just past the segment end. March checks the perpendicular distance to the infinite line, and its only bound is `|start->c| - r <= L` (`KTDXCollision.h:1542`). June measures to the clamped end point. Example: centre at (L + 0.9 rsum, 0.9 rsum) is a March hit and a June miss.

Both directions are the expected line x sphere cases.

**T6 far samples: not reproduced.** In C8b's run, one charger (owner 1000027) logged 60 `TEST` lines, and the hitbox sat on the mesh in every sample. This review closes T6 as not reproduced, and the trace stays compiled in.

**census.py:** `census.py --summary` exit 0, every check PASS, `census.tsv` unchanged. Open rows: C10 6, C11 3, none for C8. `APPLIED_MEMO_ID` stays `implemented`: C8b only compiled out C3's unsupported log; the key is still parsed.

**Hygiene (`flagcheck --range 735ccc4^..HEAD`, both PASS):**
- **JUNE_COLLISION:** `files: 0 PASS, 37 OTHER, 0 FAIL, 0 ERROR` / `0 deleted` / `RESULT: PASS`.
  - `X2DamageManager.cpp` is OTHER on its own: the same commit also adds DAMAGE_EXCEPTION's `#ifndef` (the C3 log).
  - With `--commits 735ccc4 --with JUNE_DAMAGE_EXCEPTION JUNE_FX_DEBUG` it is `PASS +222 -6`, plus a DEFINE row; RESULT PASS.
  - The 6 deleted lines are Phase 9's TEST/COLLIDE/DIFF trace lines, rewritten with the path tag, owner and age.
- **JUNE_DAMAGE_EXCEPTION:** `files: 6 PASS, 4 PASS~, 1 DEFINE, 30 OTHER, 0 FAIL, 0 ERROR` / `82 deleted, 1 non-ASCII, 0 EOL, 3 tok, 0 encoding` / `RESULT: PASS`.
  - The 82 deleted lines are b69d2a2's moves (+N/-N in four files).
  - The non-ASCII line is a moved studio line in `X2GUUser.h` (the `NOT_CANCEL_BBT_STUN_BY_HIT` comment).
  - `tok! 3` are the `#endif`s of the `#if defined && !defined / #error` dependency guards in `Always.h`.
  - Each PASS~ is one blank line after a closing `#endif` (735ccc4) or a pure move (b69d2a2).
- **This review's fix:** `--worktree --files X2Lib/X2GUUser.cpp`: `PASS +5 -5`, 0 non-ASCII, 0 EOL. The 5 deleted lines are C8b's wrong citations.

**Deploy:** DEPLOY OK: X2_offline.exe is 19,497,472 B, dated 2026-10-09 21:57:01, md5 35778ad9d417af7da1ebea6a2472b784, equal to X2/US_SERVICE/x2.exe (2026-10-09 21:56:56); built at mods/june-buff-port 4d0549a + 1 uncommitted code file(s) (this review's `X2GUUser.cpp`). This exe is the first one built from HEAD after b69d2a2 rewrote the committed blobs; the C8b note's 18:00 exe predates that fix-up.

**User owes:** nothing; no play-test is needed (comments only). UnitToEffect stays verified by code only.

---

## C9 - The June effect files March still does not open

**Status: code done 2026-10-07 (play-test owed); the UnitViewer half is not ported, see the Done note and C9b. Model:** Sonnet 5, high (identification is from C2).

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

### C9 - Done (2026-10-07, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE`)

**What landed** (details in `MODS.md`, "June effect completion C9"; pure insertions). The flag needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`.
- **`Elsword_DamageData.lua`**: `CX2DamageManager` is now bound in Lua (`X2Main.cpp`, with `InsertDamageData_LUA` = June `32621080`),
  the ctor loads the file after `OpenWayOfSwordScript` (`OpenJuneDamageDataScript` = `32620940`, from `32622080` @`326221da`), and
  `SetDamageDataFromLUA` applies the record right after the `DAMAGE_DATA_KEY_NAME` read (`JuneApplyDamageDataRecord` = `3261a1a0` +
  `3264a5d0`, call @`3263edee`).
- **Record, settled from `32621080`** (the two roles C2 left open): `rec[0]` is the damage type, computed (PHYSIC <= 0 -> 1 with MAGIC kept;
  both > 0 -> 2; else 0 with MAGIC zeroed), `rec[5]` is `DAMAGE_DOWN_POINT` -> `fForceDownValue`. Each June `DamageData` offset was
  checked against its key's store in `32632390` (funcmap rows tagged `Effect C9`), and March members are used by key. First entry wins on a
  duplicate name; an empty name is skipped. `DAMAGE_SKILL_ID` is copied when non-zero as June does (skill reform stays handed off, D7).
- **Order**: the apply sits after every March key and before the (handed-off) NUMERICAL_DATA override, as June's. No key June reads after
  `3263edee` stores into a record field.
- **Staged**: `june2017_re/stage_june_fx.py` gained `--only NAME` (the full plan is the rolled-back Phase 11 set and must not be applied
  wholesale; `stage_plan.tsv` is untouched, the plan goes to `stage_plan_only.tsv`). `Elsword_DamageData` is in `LUA10`.
  `--only Elsword_DamageData.lua --apply` added exactly one file to `kom_contents/data036_kom/` and one manifest key.
- **Packed**: `python tools/kom.py pack data036` -> `PACK OK data036` (189 entries, `+1 added, -0 removed, ~0 changed`, `+ Elsword_DamageData.lua`).
  **Not copied to the game yet** (asks the user first).
- **Census** (`census.py`, 2026-10-07): the 8 `InsertDamageData` rows (+ the `InsertDamageData` top row) are `implemented`; the 3 `ES.Description`
  rows are `declined`; the only FAIL is the pre-existing HP-rate item (owner C10). `unassigned` is now 0 outside the nested buff parser.

**Play-test fix (2026-10-07, after `509f15c`).** With the file packed, the client closed silently at loading, with no crash dump and no
log line. A `DD_TRACE` build located it: the first call from the file entered `InsertDamageData_LUA` and died in the first
`LUA_GET_VALUE`. The cause was the loader running the file through `KLuaManager( L, 0, true )`, which creates a **new Lua thread**.
The handler's `KLuaManager( GetLuaState() )` then read the main state's stack instead of the call's table, and the error escaped
every pcall. A probe of the same call made from C++ on the main state worked. The loader now uses `g_pKTDXApp->LoadLuaTinker`, as
`CX2EffectSet::OpenScriptFile` does for its callbacks. The trace flag was removed.
Re-test passed (exe md5 `03e657f7`, `113a3a9`): `ErrorLog.txt` has `JUNE_FX DAMAGE_DATA_LOAD Elsword_DamageData.lua loaded=1 rows=107
skipped_empty_name=0 skipped_duplicate=0`. The client reached a dungeon with 0 EXCEPTION, and data036.kom is live (`COPY OK`, md5 `569113df`).

**Declined: `EffectSet_Description.lua`.** June's client never names the file (C2): `32378e20` is `AddNPCTemplet_LUA`, `32680da0` is
`AddNextStepPackage_LUA`, and the census matched their `DESC`/`NAME`/`VERSION` keys by name only. March reads it only under `#ifdef EFFECT_TOOL`.

**Not done: `UnitViewerEffectSetTable.lua` (the viewer half).** The plan's Exit does not count it, and it is identification, not
transcription: `31fa3010` (20+ call sites from `323fad80`) looks the motion name up (`323760f0`) and pushes a 0x30-byte request into a vector
that another function drains; it also sets `this+0x11d8` for `LDB_LobbyReadyAction`. The bulk loader that calls `31ee6100`
(`32597410`) is not in funcmap, so even where March should call the loader is unconfirmed. Porting the bind and the table with no consumer
would be inert, so the file is **not staged and not bound**. Proposed new phase **C9b** (Opus, no code): identify the viewer play path
(`31fa3010`/`31fa8ce0`/`32043380`, the queue and its drain, `32597410`) and decide whether March's `CX2UnitViewerUI::PlayAnim` can take it.
`DUO_TYPE`, `UNIT_TYPE.UT_NONE` and `UNIT_CLASS.UC_NONE` already exist in March's `Enum.lua`.

**Hygiene.** `flagcheck JUNE_DAMAGE_DATA_TABLE --worktree --files <my 4 files>`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The two OTHER rows are a **peer session's** uncommitted C6b edits (`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`) in `Always.h` and `X2DamageManager.h`; I committed only my hunks.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,297,280 B, dated 2026-10-07 10:45:46, md5 adda229ec56700e76e92cf1b7e8022e3, equal to X2/US_SERVICE/x2.exe (2026-10-07 10:45:42)`
(the build also contained the peer's uncommitted C6b edits).

**How to see it in game.** After `data036.kom` is copied and the game starts, `ErrorLog.txt` has
`JUNE_FX DAMAGE_DATA_LOAD Elsword_DamageData.lua loaded=1 rows=N ...` (expected N close to 107 minus duplicate names). Only three June
blocks carry `DAMAGE_DATA_KEY_NAME`, all in files March does not run today: `GetSA_RVK_Freezing_Bullet_Explosion( slot0 )` and a fixed
name (the "high-mobility cast, outside calc" record) in `DamageEffect.lua`, and the "Mecha Drop explosion" record in `DamageEffect_INT.lua`.
Both fixed names are in the table. So in a March play-test `offline_server.log` will normally show **no** `DAMAGE_DATA_KEY` line: an
absent `apply` line is not a failure. The load line in `ErrorLog.txt` is the proof for this phase.
Open question for the user: none; C9b is proposed above.

### C9 - Review (2026-10-07, Opus 5.5 high): no finding, no code change

**Play-test read first.** `ErrorLog.txt` (run 12:00:04, exe md5 `03e657f7` = the `113a3a9` deploy): `JUNE_FX DAMAGE_DATA_LOAD
Elsword_DamageData.lua loaded=1 rows=107 skipped_empty_name=0 skipped_duplicate=0`, 0 EXCEPTION. June's source has 107
`DAMAGE_KEY_NAME`s and none repeats, so 107 is the full table.

**Checked against June (Ghidra, read-only).**
- `32621080` `InsertDamageData_LUA`: the 9 keys were decoded by length and getter (string / float `32fea370` / int `32fe9fd0`), and each
  maps to the record slot C9 uses. Defaults: PVP 1.0, the rest 0. The two int keys (TECH_POINT, ATTACK_TYPE) are masked to 0 when the read
  fails, which settles census.tsv's two `unchecked-default` rows: March's 0 is June's. The empty name is skipped, and so is a name already
  in the map (`326411f0` find != `+0x4c00` end), so the first entry wins. The type branch is the same as March's:
  PHYSIC <= 0 -> 1, MAGIC kept, PHYSIC 0; MAGIC > 0 -> 2; else 0, MAGIC 0.
- `3264a5d0` apply: `+0`, `+0x538`, `+0x38`, `+0x3c`, `+0x3f0`, `+0x3f8` (protected, written when different; March's unconditional
  write gives the same value), `+0x34`, `+0x5c4` only when non-zero, `+0x8`. This matches `JuneApplyDamageDataRecord` field for field.
  `32622220` seeds the out-record with PVP 1.0 and applies only on a hit.
- Call site `3263edee`: inside `SetDamageDataFromLUA`, after `DAMAGE_DATA_KEY_NAME`, only when the length is non-zero. March: X2DamageManager.cpp:633.
- **Clobber check.** Every June store to a record offset after `3263edee` (`3263ff9c`/`ffa7` physic/magic, `ffb7` PVP, `ffc9` tech,
  `ffd8` add-MP) is inside the `3263fe41`..`32640054` block, which is gated on NUMERICAL_DATA / APPLIED_SKILL_ID (handed off, D7).
  Between the parse start and the apply, March modifies none of the record's fields. Its only `fPowerRate` use is `m_ExtraDamage`.
  After the apply, the only March writer is `SetDamageExceptionProcess` (`MODIFY_SET_DAMAGE_DATA` is defined), June's per-unit post-step.
- `32620940` loader: it clears the map, then reads `L"Elsword_DamageData.lua"`. The `LoadLuaTinker` fix (`113a3a9`) is right: the
  handler's `KLuaManager` reads the call's table off the main state.
- ES.Description decline: not re-checked in Ghidra. It rests on C2's identification (`32378e20` = AddNPCTemplet_LUA,
  `32680da0` = AddNextStepPackage_LUA). Census: 9 `InsertDamageData` rows `implemented`, 3 `ES.Description` rows `declined`, as Exit says.

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE --range 509f15c^..HEAD`):
`files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 8 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The 2 deleted lines are `113a3a9` replacing this flag's own `KLuaManager( L, 0, true )` and `LoadAndDoMemory` lines from
`509f15c`, not studio code. All 8 OTHER rows are `df22a13`, the peer's C6b commit (`SPAWN_INHERIT` / `CREATE_EFFECT_SET`) inside the range.

**Deploy:** none. Nothing changed, and the tree holds a peer's uncommitted `Always.h` / `X2GageManager.cpp` / `X2GameUnit.cpp` edits.
The deployed exe is still `03e657f7`, the one the play-test passed on. No new play-test. Next: C9b.

### C9b - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build)

**Verdict: port it, as C9c-1 / C9c-2a / C9c-2b below.** The table is June's data-driven replacement for every class effect March
hard-codes in `CX2UnitViewerUI`. All 13 live rows reproduce effects March already draws by hand. Funcmap: 12 new rows tagged `Effect C9b`,
and 3 rows corrected in place (`31ee6100`, `31fa3010`, `329585f0`). Per-row reach is in `june2017_re/c9b_unitviewer_rows.tsv`.

**1. Loader.** `32597410` is June's `CX2StateStartUp` load body (`X2StateStartUp.cpp:239-240` order). **Correction:** `31ee6100` is
**`CX2Data::ResetUnitManager`**; after `OpenDataScriptFile( CorrelateUnitClassTemplet.lua )` (`32376f10`) it opens `SelectVoiceTable`,
`AnimationNameTable`, then `UnitViewerEffectSetTable.lua` @`31ee6224`. March counterpart: `X2Data.cpp:1824`, after Correlate at `:1835`.

**2. `31fa3010` = `CX2UnitViewerUI::PlayAnim( name, playType, bTransition, which )`.** Its callers are the 9 `LobbyDownLanding` sites
(`X2StateCreateUnit`) and `323fad80`. `which` 0/1/2 = main skin anim `+0x1e8` / duo minor `+0xd7c` / both. After the anim it clears the
request vector `+0x11a0`, stops group-0 viewer effects (`329594c0`), looks the motion up (`323760f0`; 0 = any type/class, 2 = any duo,
first row wins) and, on a hit, pushes a 0x30 request `{ set name, UNIT_TYPE, UNIT_CLASS, start time, loop, 0 }`. No hard-coded particles.
- **Drain:** `31fa8ce0`, the per-model frame move inside `OnFrameMove` (`31faa550`). A request plays once the anim time reaches its start
  time (0 = at once), via `32959640( skin anim, request, group 0 )`, and is then erased.
- **Re-arm:** when the anim time is exactly 0.0 (the loop wrapped), `loop = true` rows are queued again.

**3. The other two callers.** `31fa8ce0` is the re-arm above. `32043380` = **`CX2GUUser::PlayEmotion`** (`X2GUUser.cpp:28817`):
- it looks up the emotion motion, plays the set at once on `g_pData +0x9c` with the unit as owner, keeps the handle at `+0x943c`, and tags
  it via `32296f30` (C10a site 32043cf0, "with host"); start time and loop are not used;
- only Rose/Ain rows match, and neither class exists in March, so the path is inert;
- `+0x9c` = `m_UIEffectSet`: the `_INT` opens in `32597410` go to `+0x9c` and then `+0xe0`, March's UI-then-Game order
  (`X2Data.h:567-568`), and all 15 sets live only in June's `UIEffectSet.src.lua`.

**4. March has both hooks but no player.** June's viewer does not call `CX2EffectSet::PlayEffectSet`; it runs its own player on a base
viewer class: `32959640` plays (`ET_MESH_PLAYER_UI_MAJOR` / `ET_PARTICLE_UI_MAJOR` into lists `+0x194` / `+0x1a4`, plus the templet's
`+0x70` list), `329585f0` updates each frame (bone trace, `FOLLOW_UNIT_VIEWER_SCALE`, `ANIMTIME_ON_START_UNITVIEWER` triggers: the runtime
owner of those "owner open" census rows), `329594c0` / `32959250` stop.

March's `CX2BaseUnitViewerUI` is empty and `PlayEffectSet` takes only a game unit, square unit or pet as owner, so C9c ports the player.
June's own viewer effects (`31fa52b0`, `31fa4ec0`, `31f91b70`, `323fad80`, `32e50de0`) also use it; they are out of scope.

**Reach.**
- **13 rows are live** once staged. March's `UIEffectSet.lua` has none of the 15 sets; the two SBH sets are only in the game
  `EffectSet.lua`. Every particle and mesh those 13 name is already in March's `UIMajorParticle.txt` / `UIMajorXMeshPlayer.txt`.
- **4 rows are inert by class** (Rose/Ain). **1 is inert by duo** (`LDN_LobbyWait`).
- **March's hard-code:** handles created in `SetUnit` (`:1985`) and used in `PlayAnim`, `OnFrameMove`, `NotifyShowObjectChanged`,
  `OnFrameRender_Draw`, `Render_DrawForAvartaScreenCapture` and `PlayByMotionType`. The handles are `m_hSeqHandFire`, `*SPK1/2`,
  `*HandElectricALP1/2`, `*EnergyBullet1-3`, SBH, crescent cut, and the ETK/CDC/RTR/RNW meshes.
- None of these names is a plain string in June code (only `MagicKnightHandFire`, in GU SwordMan `32d07090`). Upgrade, enchant and
  emblem particles stay.

**D8 (no question):** the duo minor model (`+0xd7c`/`+0xd80-0xd82`), the `DT_MINOR` row and `LDB_LobbyReadyAction` (`+0x11d8`) go to
`JUNE2017_CHARACTER_PORT_CHECKLIST.md`. March always passes duo 0.

**Port list.**
- **C9c-1 (Sonnet), new flag:**
  - bind `AddUnitViewerEffectSet`, with the map, first-wins insert and lookup (June's three gates);
  - load the file in `ResetUnitManager` after Correlate, under the new flag, not inside `REFORM_SKILL_NOTE_UI`. `OpenDataScriptFile` uses
    `LoadLuaTinker` on the main state (C9's failure was a new Lua thread);
  - add a `JUNE_FX` load-count line, then stage and pack the file;
  - port the `PlayEmotion` consumer (inert). The `32296f30` tag goes in only if C10b-2 has landed it; otherwise it is a note for C10b-2.
- **C9c-2a (Opus, no code):** identify the player's helpers (`3295a080`, `3295a400`, `32959970`, `32959e40`, `32959250`, `32957980`,
  `329582b0`, `32957170`, the `+0x70` list) and `329585f0`'s per-entry update; map each hard-coded site to its row.
- **C9c-2b (Sonnet), one phase so nothing regresses in between:**
  - the player, and the `PlayAnim` / `OnFrameMove` hooks in June's order;
  - stage the 15 templets verbatim into March's `UIEffectSet.lua` (decompile, append, compile; never the rolled-back wholesale replace);
  - retire the hard-code with `#else` intact.

  Check: the exact-0.0 re-arm may queue `SBH_LobbyWait` / `ALP_LobbyWait` twice on the first frame. Keep June's order; add a `JUNE_BUGS`
  row if it reproduces.

Open question for the user: none.

---

### C9c-1 - Done (2026-10-07, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE`; play-test PASSED 2026-10-07)

**What landed** (details in `MODS.md`, "June effect completion C9c-1"; pure insertions; the load line needs `SERV_IRUHADEV_JUNE_FX_DEBUG`):
- **Table + bind** (`X2UnitManager.h/.cpp`, `X2Main.cpp`): `AddUnitViewerEffectSet` = `3237a1f0`. Seven arguments exceed lua_tinker's member
  functor (6), so the handler is no-arg and reads the Lua stack (1 = manager, 2..8 = motion, UNIT_TYPE, UNIT_CLASS, set name, start, loop, DUO).
  First row wins. A wrong-typed argument drops the row (`skipped_bad_args`); June's typed reads would give 0 = "any", which would widen the row.
- **Lookup** `GetUnitViewerEffectSet` = `323760f0`, gates in June's order (type, class, duo != 2 and != row). Class -> type uses the class's templet
  `m_UnitType` where June calls `31eca1d0` (a class switch; same mapping for March's classes; the review spot-checks it).
- **Load** in `CX2Data::ResetUnitManager` (`X2Data.cpp`) after the Correlate open, outside `REFORM_SKILL_NOTE_UI`, via `OpenDataScriptFile`
  (`LoadLuaTinker`, main state). June's `SelectVoiceTable.lua` / `AnimationNameTable.lua` opens in between are **not ported**.
- **`PlayEmotion`** (`X2GUUser.cpp`, after `m_ePlayedEmotion = eEmotionId`, before the sound, as `32043380`): lookup with the emotion motion
  name, the unit's class, duo 0; on a hit `g_pData->GetUIEffectSet()->PlayEffectSet( name, this )`, handle kept in `m_hJuneEmotionEffectSet`
  (`+0x943c`). **Inert in March** (only Rose/Ain rows match; not tested).
  **For C10b-2:** June then calls `32296f30( handle, category 1, by pointer )` at `32043cf5`. The helper is not in March yet, so the tag is not
  ported; the site is the end of the `if( INVALID_EFFECTSET_HANDLE != hJuneEffectSet )` branch in `PlayEmotion`.
- **Data**: `UnitViewerEffectSetTable` added to `LUA10` in `stage_june_fx.py`; `--only UnitViewerEffectSetTable.lua --apply` staged one file and one
  manifest key. Enum check: `UT_ROSE` = 11, `UT_AIN` = 12, `DT_MAJOR` 0, `DT_MINOR` 1 are in `Enum.src.lua` and in the compiled `Enum.lua`.
  `PACK OK data036` (190 entries, `+1 added, -0 removed, ~0 changed`); `COPY OK data036` (user said "copy"; md5 `62318ebb`, backup
  `kom_backup_tools/data036.kom.20261007_133401`).

**Hygiene.** `flagcheck JUNE_UNIT_VIEWER_EFFECT_TABLE --with JUNE_FX_DEBUG --worktree --files <my 7 files>`:
`files: 5 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS` (`X2UnitManager.h`'s 23 inserted lines were first LF next to an LF anchor; converted to CRLF to match the file's majority).
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,283,456 B, dated 2026-10-07 13:52:04, md5 d8917e2f820962fe11434f6936778693, equal to X2/US_SERVICE/x2.exe (2026-10-07 13:51:59)`;
`UNIT_VIEWER_FX_LOAD`, `UnitViewerEffectSetTable.lua` (UTF-16) and `AddUnitViewerEffectSet` are in the deployed exe. The build also contained a peer's
uncommitted `X2BuffTempletManager.cpp` (14b) edit, which is not in my commit.

**How to see it in game.** Start the game (data036.kom is already copied), reach the login screen, close it, say "check logs":
`python tools/logs.py --tag JUNE_FX` should show `JUNE_FX UNIT_VIEWER_FX_LOAD UnitViewerEffectSetTable.lua loaded=1 rows=18 skipped_duplicate=0 skipped_bad_args=0`, with 0 EXCEPTION.
No visible change: the viewer consumer is C9c-2b and the `PlayEmotion` path is inert.
**Play-test PASSED 2026-10-07** (exe d8917e2f, run 13:57-13:59, after deploy): `ErrorLog.txt` line 144 `JUNE_FX UNIT_VIEWER_FX_LOAD UnitViewerEffectSetTable.lua loaded=1 rows=18 skipped_duplicate=0 skipped_bad_args=0` (`X2UnitManager.cpp`, `OpenJuneUnitViewerEffectSetTable`), 0 EXCEPTION, clean exit with the census; C9's `DAMAGE_DATA_LOAD ... rows=107` still loads. `tools/logs.py --tag JUNE_FX` printed no ErrorLog lines (its filter wants the bracketed form `[JUNE_FX]`; these lines are unbracketed), so read it with `grep UNIT_VIEWER ErrorLog.txt`. The only UNHANDLED packet is `E_DISABLE_HB_CHECK_REQ` (not this phase).
Open question for the user: none.

### C9c-1 - Review (2026-10-07, Opus 5.5 high): one finding fixed (June's stop side of the PlayEmotion handle), one hygiene note

**Play-test read first.** The Done note's run (exe `d8917e2f`): `JUNE_FX UNIT_VIEWER_FX_LOAD UnitViewerEffectSetTable.lua loaded=1 rows=18
skipped_duplicate=0 skipped_bad_args=0`, 0 EXCEPTION. `skipped_bad_args=0` means every argument of all 18 rows had its Lua type. The
port's one deliberate difference (a wrong-typed row is dropped where June's typed read would give 0 = "any") therefore cannot change any row.

**Checked against June (Ghidra, read-only).**
- `3237a1f0`: 7 args (motion `char*`, type, class, set `char*`, float, bool, duo). The node is filled `+0x44..+0x54` as the port's struct.
  Insert only when `31e58a40` find == end, so first row wins. June converts with `MultiByteToWideChar(CP_ACP)` and the port with UTF-8.
  All 36 names are ASCII, so the result is identical.
- `323760f0`: miss on absent key; `+0x44 != 0 && != 31eca1d0(class)`; `+0x48 != 0 && != class`; `duo != 2 && != +0x54`; then copy out
  `+0x28` / `+0x4c` / `+0x50`. The order is the port's.
  - `31eca1d0` agrees with March's `UNIT_CLASS` enum for every class March has, across all 9 types. Spot-checked: 1/10/11/22/100/101/112 -> 1;
    7/28/31/118/119 -> 7; 9/32/122 -> 9.
  - The port's templet `m_UnitType` gives the same value. A class with no templet gives 0, as June's `default`.
- `31ee6100`: `32376f10` opens Correlate (`3345be48` @`31ee61f5`), then `32379690`, then SelectVoice, AnimationName, and UnitViewer
  (`3345bdb0` @`31ee6224`). The port opens UnitViewer right after Correlate, outside `REFORM_SKILL_NOTE_UI`, via `OpenDataScriptFile`
  (`LoadLuaTinker`, main state). Correct. `32379690` is also not ported, alongside the two opens the Done note lists.
- `32043380`: the motion arg is `wstrEmotionType` (the same string June tests for `empty()`). The class is `GetUnit() +0x38`; the duo is
  `+0x8e90 == 1` (March 0). The handle is stored only when `!= -1` (@`32043ccc`), as the port does. The `32296f30` tag @`32043cf0` is
  named in the Done note. C10b-2 has not landed (queued), so the tag stays out.

**Finding (fixed here, same flag): the stop side of `+0x943c` was not ported.** `dispsweep 943c 9438` found 5 more June sites.
`+0x9438` is the **emotion id** that played the set (`param_2` @`32043cda`), not "its manager"; the funcmap row is corrected.
- `31fcf490` @`32043b64`, before the unit-manager check: emotion = 0, `StopEffectSet( +0x943c )`. Ported at the top of the PlayEmotion
  block, gated only on `g_pData`.
- On a hit: also store `eEmotionId` (`@32043cda`).
- `CX2GUUser::OnFrameMove` @`3206ed75` (= `31fcf4d0`, which has no xrefs): if the handle is valid and the emotion != `m_ePlayedEmotion` (`+0x89d8`),
  clear and stop. Ported right after `CX2GameUnit::OnFrameMove`, before `IncrementLocalRelativePosTimers` (`31fc8b90`, confirmed).
- `~CX2GUUser` `3203a620` (13 class-dtor callers) @`3203afb7-afd1`, before the stick-shake prompt @`3203aff3`: clear and stop. Ported at
  the dtor head, before Phase 8's stick-shake block.
- ctor `@32041dcf`: `+0x9438 = 0` -> `m_eJuneEmotionEffectSetEmotion = ET_NONE` (0).

March's `StopEffectSet( Handle& )` returns at once on `INVALID_EFFECTSET_HANDLE`, so the two paths that now run live (every emote and
every user dtor) are no-ops. `GetUIEffectSet()` is `&m_UIEffectSet`, which lives as long as `g_pData`. June's second `321dafb0`
argument (0) has no March counterpart (one-arg signature).
Funcmap: rows `31fcf490`, `31fcf4d0`, `3203a620` added; `32043380` corrected.
Out of scope, unowned: the June-only `32355a50` emotion branch (`+0x9434`, and the `+0x93dc` / `+0x93ec` vectors in OnFrameMove and
the dtor). See the open question.

**Hygiene.**
- Fix (`flagcheck ... --worktree --files X2GUUser.cpp X2GUUser.h`): `files: 2 PASS`, `0 deleted line(s)`, `RESULT: PASS`.
- C9c-1's commit alone (`--range c9476fa^..c9476fa`): `files: 4 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`.
- The 3 OTHER rows are 14b's `SERV_IRUHADEV_JUNE_BUFF_INSTANCE_KILL` code: the `Always.h` define, `X2GUUser.cpp` +55, `X2GUUser.h` +9.
  `c9476fa` committed it whole; 14b's own commit `e5bad74` came after. The Done note's "not in my commit" is wrong for those hunks.
  History is not rewritten. Both flags are on and both phases passed their play-tests.
- Tool note: `logs.py --tag JUNE_FX` misses the unbracketed `JUNE_FX ...` lines; read them with `grep UNIT_VIEWER ErrorLog.txt`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,283,968 B, dated 2026-10-07 14:20:42, md5 f7b368060e27e455819fa517b6b50522, equal to X2/US_SERVICE/x2.exe (2026-10-07 14:20:37)`. Built on `227cb9b` plus this fix and nothing else uncommitted.

**Play-test (regression; the new paths are inert apart from the invalid-handle stops).** Pass means `rows=18` still loads, the run
has 0 EXCEPTION and exits cleanly with no `X2_*.tmp` crash bundle, after emotes in a village and a dungeon entered and left.
**Regression play-test PASSED 2026-10-07** (exe `f7b36806`, run 14:23:23-14:24:34, after deploy): State.log shows village -> dungeon ->
village; ErrorLog line 144 `JUNE_FX UNIT_VIEWER_FX_LOAD ... loaded=1 rows=18 skipped_duplicate=0 skipped_bad_args=0`; 0 EXCEPTION, clean exit
(census present), no new `X2_*.tmp`. Emotes leave no packet offline; the user confirmed the emote check passed. The only
UNHANDLED packet is `E_DISABLE_HB_CHECK_REQ` (not this phase).

Open question for the user: should June's other emotion effect branch (`32355a50`: a set per emotion, kept at `+0x9434`, stopped in
OnFrameMove and the dtor, with the `+0x93dc` / `+0x93ec` handle vectors) get its own phase? No plan row owns it.
**Answered 2026-10-07:** yes, follow June -> **C9d** (Opus, identification, `kickoffs/june-effect/C9d.md`), then a Sonnet port C9e.

---

### C9c-2a - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build)

**Landed** (`47ed7ed` + follow-up):
- 19 funcmap rows tagged `Effect C9c-2a`;
- C9b's `329585f0` row corrected in place: `Bip01_Footsteps` is a USE_LAND_POSITION override; dead entries are **not** erased; the set is
  `..._motionFX`;
- the per-site map `june2017_re/c9c2a_hardcode_map.tsv`;
- the C9c-2b kickoff.

**1. Play `32959640`** pushes 0x40-byte entries and creates nothing:
- fields: `+0` EffectData, `+4` skin (ref-counted), `+8` group, `+0x0c` trace-target mesh, `+0x10` set name, `+0x2c` copy of
  MESH_TRIGGER_DATA_LIST, `+0x3c` own handle;
- only UI-major meshes and particles are taken; templet `+0x70` (`m_vecSoundData`) goes to a pending-sound list;
- `+0x190` = "shown": set to 1 by Play, never cleared.

**2. Per frame `329585f0`** = `CX2BaseUnitViewerUI::OnFrameMove`. It traces, then creates (`32957980`), then plays sounds (`329582b0`).
- **Create:** once START_TIME is reached, at bone + POSITION_OFFSET, with the viewer as owner. ROTATE goes to `SetRotateDegree` (meshes)
  or `m_AddRotateRel` (particles). SCALE goes to `m_ScaleFactor` when USE_EFFECT_SCALE_IN_UNIT_VIEWER is set, except in `XS_PVP_ROOM`.
- **Trace:** TRACE is never read, so every entry follows its bone. FOLLOW_UNIT_VIEWER_SCALE copies `GetMatrix().GetScale()`.
- **Triggers:** `321dfc40` runs with a NULL set and two viewer codes, using ANIMTIME_ON_START_UNITVIEWER.
- **Draw** `32957330`: `PushStates` + `OnFrameRender_Draw` on every shown entry, without testing DG membership.
  - March chains every effect at creation; June un-chains only the particles of an unmanaged viewer.
  - So a managed viewer's effects are reached twice. March's `m_bFixed` meshes already are.
- **SpaceMoonRabbit** (`..._motionFX`) is June-only: dead in March.

**3. Stop.** `329594c0(group)` destroys one group's meshes and particles. `32959250` stops sounds (group 5 = all).
The viewer dtor stops groups 0..4; the base dtor stops group 1 and then every sound.

**4. State goes in `CX2BaseUnitViewerUI`.** June's RTTI and vtable `3347113c` hold the player; June's NPC viewer uses it too. March's NPC
viewer already derives from the empty base and overrides 3 of the 4 virtuals; the 4th becomes a no-op. C9c-2b re-parents
`CX2UnitViewerUI` insert-only.

**5. Map: 11 live rows, not 13.**
- `SPK_LobbyReadyAction` has no March motion and nothing plays it.
- `ALD_LobbyWait` is June's renamed anim; March's is `ALD_lobbyWait`, and the map is case-sensitive.
- Nine hard-codes retire. Enchant, upgrade, emblem and APINK effects have no row and stay.
- Lost under June: the SBH/ALP end windows and forced emit rates, the meshes' weapon gate and outline, and effects in the avatar capture.

**6. Templets.** The staged `UIEffectSet.lua` round-trips clean with mar-2014: same bytecode size, only key order differs, 52 sets.

**Quirks for C9c-2b's JUNE_BUGS** (inert on the 15 sets):
- cumulative TRACE_MATRIX offset;
- no null test on the sound skin;
- dead link store `@32957bfc`;
- one land particle per dirty flag.

**Decisions (user, 2026-10-07):**
- (a) **keep the ALD orb hard-code**, stage its templet, add a checklist §7 row;
- (b) **guard** the null TOGGLE_VISIBILITY write on the viewer trigger path, with a JUNE_BUGS row;
- (c) **adopt June's whole `UIMajorParticle.txt`** and re-append the 10 March-only sequences. March drove `MagicKnightHandFire` (no emit
  rate) and `SBH_LobbyWait01-03` / `ADD_LP_LobbyWait01-02` (never stop) from code.

**Left for C9c-2b:** June's `SetHideSelectUnitWeapon` counterpart, `31cf4890`'s March equivalent, mesh `+0x15c` and seq `+0x415`.
Nothing was built, so there are no hygiene or deploy lines.

---

### C9c-2b - Done (2026-10-07, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`; play-test pending)

**Landed** (this commit, `git log --grep C9c-2b`; details in `MODS.md`, "June effect completion C9c-2b"):
- **Player on the base viewer** (`X2BaseUnitViewerUI.h/.cpp`, June's layout): the entry (`+0x00..+0x3c`), the four vectors, `+0x190` show,
  `+0x1d4` land-dirty, `+0x1d8/+0x1dc` trigger codes; Play `32959640`, Stop `329594c0` / `32959250`, create pass `32957980`, per-entry update
  `329585f0` (three trace branches, FOLLOW_UNIT_VIEWER_SCALE, show, triggers, land), sound pass `329582b0`, Prepare `32957260`, Draw `32957330`
  (meshes, then particles; `PushStates` + draw), `NotifyShowObjectChanged` `32957760`, ctor `31fae120`, dtor `31fae1a0`, land position `32957170` with
  the lowest-bone helper `31d2b080` (frame tree walk).
- **`CX2UnitViewerUI`** re-parented under the base (insert-only `#ifdef`). `PlayAnim` `31fa3010`: clear queue, stop group 0, `GetUnitViewerEffectSet`
  (duo 0), queue. `OnFrameMove` `31fa8ce0`: codes + base update at entry (before the skin frame move), re-arm `@31fa9271` and drain `@31fa93f6`
  after it, base update again after the equip loop. Base calls from Notify / Prepare / Draw; dtor stops groups 0..4; `SetPositionOnScr` sets `+0x1d4`.
- **Trigger variant** `JuneRunViewerMeshTriggers` (`X2EffectSet.cpp`, `321dfc40 @viewer`): first non-zero folded code, ANIMTIME_ON_START_UNITVIEWER,
  STOP_TRACING skipped, EFFECT_SET_TOGGLE_VISIBILITY_TIMER skipped and logged once (user decision), JUNE_BUGS 86.
- **Retired hard-code** (`#ifdef ... #else original #endif`): the `SetUnit` creation of the MagicKnight, SPK, SBH, ALP, CDC, ETK/EIS, RTR and RNW
  handles, and the `PlayAnim` emit / `m_bShow*` / crescent-cut blocks for them. The Little Devil orb is untouched. Every other site of a retired
  handle (`OnFrameMove`, Notify, Prepare, Draw, avatar capture, `SetHideSelectUnitWeapon`) tests the handle first, so it is a no-op: not edited.
- **Data** (`data036`): `UIEffectSet.lua` = March's 52 sets + the 15 June blocks (37,072 B compiled, `kom.py status` was in sync first);
  `UIMajorParticle.txt` = June's file (2,042,192 B = the June kom entry; braces balanced) + the **10 March-only sequences** before its last brace:
  `CHUNG_HOWLING01/02`, `CHUNG_STEEL_EDGE02`, `Nasod_King_SpellAttack_Splitlight06`, `PET_PETTE_BERTHE_STEB_3_Attack_C`, `..._Attack_C_02`,
  `Pulsewave_Summon_Boss_SpecialAttackB01/02`, `ShockWave_Summon_Boss_SpecialAttackB`, `UI_Eve_Party_Event_StartEffect` (2,049,998 B, 2729 sequences).
  `PACK OK data036` (`~2 changed`: those two). Every June-only key in that file (AniXet, ChangeTexXet, CullMode, FinalParticleList, MeshRenderType,
  TraceParticleList, WireFrame) is already parsed by `KTDGParticleSystem.cpp`. The orb's `SA_AYR_ORB_KI_Main_S_P01` lost March's `Zenable = false`.
- **Docs**: JUNE_BUGS rows 79-86; checklist 7e (ALD orb, duo model, Rose / Ain rows, what June's player loses); MODS; review kickoff (re-verifies C9c-2a).

**Open / recorded, not decided here**
- `SetHideSelectUnitWeapon` has no June counterpart that I could identify (GhidraMCP was not running). Its four callers (`X2UICharInfo.cpp:1230`,
  `X2UIWatch.cpp:646`, `X2Community.cpp:11914`, `X2GULire_ElvenRanger.cpp:252`) run right after a unit is set, in a wait / down-landing motion; June creates
  nothing at `SetUnit` and the table meshes live only from the ready-action `PlayAnim` to the next one. The review checks this.
- `31cf4890` (draw of a particle's child sequences) is not ported: March's `CreateSequence` chains child sequences itself, so only the DG manager draws them.
- Mesh `+0x15c`, sequence `+0x415` / `+0x45a`: unidentified or write-only, not ported. `EffectSet_SpaceMoonRabbit_motionFX`: June-only, dead.
- Deliberate differences from June: a set missing from the UI effect set is logged and skipped (June dereferences it); the sound pass is skipped
  without a skin (June faults, JUNE_BUGS 80); a failed create is retried every frame as June does, with its log line written once.
- Draw ownership (C9c-2a): a managed viewer's effects are reached by both the DG manager and the viewer's Draw. Kept as June has it; if the
  play-test shows doubled (brighter) effects, add a JUNE_BUGS row. C9c-2a's `31f99f70`, `viewer+0x1d8`, `31fa4980` candidates are the review's.

**Hygiene.** `flagcheck JUNE_UNIT_VIEWER_EFFECT_PLAYER --with JUNE_FX_DEBUG --worktree --files <6 code files>`:
`files: 1 PASS, 4 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (the PASS~ are the blank separator lines I inserted next to the flag blocks).
(the 2 `tok!` are existing `#endif` / `#else` lines of JUNE_FX_DEBUG blocks in `X2EffectSet.cpp` (1200, 3698), not in this phase's insertion).
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,307,008 B, dated 2026-10-07 14:56:39, md5 e30f79f8e0e04878595f252bd72bfd2d, equal to X2/US_SERVICE/x2.exe (2026-10-07 14:56:34); built at mods/june-buff-port 5354ad3 + 13 uncommitted code file(s).` Another session's `build.py --deploy --pch force` linked this exe from the shared tree; a later C4 deploy replaced it with **md5 ca2f1a7e** (15:08:32, head c1c74e2), which I re-checked still holds all four strings (my files included: `UNIT_VIEWER_FX_PLAY`, `UNIT_VIEWER_FX_CREATE`, `UNIT_VIEWER_FX_TRIGGER` and the `retried every frame` text are in it, UTF-16). **Data:** `COPY OK data036: ...data036.kom 6624522 bytes, 190 entries, md5 e931a22ccc58a4655e0882b732d0b69c` (user said yes; backup `kom_backup_tools/data036.kom.20261007_151116`). Another session's C4 edits (`X2DamageEffect`, `X2DamageManager`, `X2GameUnit`, `X2GUAdd`) were in that build too; they are committed since as `c1c74e2`.

**How to see it in game** (hand-off in the reply): the 11 live rows by class; the log lines are `JUNE_FX UNIT_VIEWER_FX_PLAY <set> group=0 meshes=<n>
particles=<n> sounds=<n>` and `UNIT_VIEWER_FX_CREATE <set> mesh|particle <name> bone=<bone>` in `ErrorLog.txt` (unbracketed: `grep JUNE_FX ErrorLog.txt`).
Test characters added to the save (backups in `db_backup/`): `MagicKnightT` (uid 32), `SheathKnightT` (33), `InfinitySwordT` (34), `TrapRangerT` (35), `LunaticPsykerT` (36), `LittleDevilT` (38, for the kept orb),
all level 70 and without gear or skills. The save already had `Freils` (Deadly Chaser), `eresisu` (Pyro Knight), `ElesisBH` (Blazing Heart), `NWatcher` (Night Watcher).
**Play-test 1 (user, 2026-10-07):** 8 of 11 rows fine (SPK, SBH, ALP wait, CDC, orb, ...). Three findings, fixed the same day:
- **ETK/EIS second sword, RTR and RNW swords: black texture only.** Those three meshes use `RT_CARTOON_BLACK_EDGE` (CDC's gun is `RT_REAL_COLOR`, which was fine).
  Cause: the base viewer's Draw (a port of `32957330`) drew them a second time outside the DG manager's render-hint setup. March chains every mesh at
  creation, so the manager already draws them. Fix (same flag): meshes are never drawn by the viewer; particles only when the viewer is unmanaged (when the
  create pass un-chained them). JUNE_BUGS row 92. Not verified in game yet.
- **Magic Knight fire looked wrong.** June's `MagicKnightHandFire` is a different particle (texture `EMK_Fire001.dds`, children `MagicKnightHandFire_P01/P02`
  with `blur_16.dds`); both textures exist only in June's `data118` (asset closure: june-only, missing). Copied a minimal `data118.kom`: the live kom + those two
  files, nothing else (`COPY OK data118 ... 2745078 bytes, 191 entries, md5 c6027dcd`, user said yes; backup `%TEMP%\kom_backup_tools\data118.kom.20261007_171132`).
  The repo's staged `data118_kom` still holds 97 other staged June files (+3 changed) that were never packed; `EMK_Fire001.dds` was added there too.
- **Lunatic Psyker "add 3d" missing:** user said skip.
Deploy after the fix: `DEPLOY OK: X2_offline.exe is 19,312,640 B, dated 2026-10-07 17:11:10, md5 0eb0990bcd9df99e0ee30aaf894dbec1, equal to X2/US_SERVICE/x2.exe (2026-10-07 17:11:05); built at mods/june-buff-port fc01407 + 1 uncommitted code file(s).`
**Play-test 2 PASSED (user, 2026-10-07 17:13-17:14, exe 0eb0990b, after the data118 copy):** user said the fixes are good. Log read (`ErrorLog.txt`, run after deploy):
`UNIT_VIEWER_FX_LOAD ... loaded=1 rows=18`; `UNIT_VIEWER_FX_PLAY` + `CREATE` for `EffectSet_RNW_LobbyReadyAction` (mesh, `Dummy1_Rhand`), `EffectSet_RTR_LobbyReadyAction` (mesh, `Bip01_L_Hand`),
`EffectSet_ETK_LobbyReadyAction` twice (1 mesh + 4 crescent-cut particles each) and `EffectSet_EMK_LobbyWait_LobbyReadyAction` (`MagicKnightHandFire`); 0 `NOT PLAYED` / `NOT CREATED`;
no particle parse message; 0 EXCEPTION; only UNHANDLED is `E_DISABLE_HB_CHECK_REQ` (not this phase). No two back-to-back `SBH_LobbyWait` PLAY lines (JUNE_BUGS 85 stays "not counted"; this run did not play SBH).
The first play-test (before the fix) covered SPK, SBH, ALP, CDC and the orb ("as expected"). The unrelated noise in the log (`MOTION_*_EMOTION_BBABBABBA.X`, `final_delete_unit`) is older.

Open question for the user: none. **User owes:** play-test.

---

### C9c-2b - Review (2026-10-07, Opus 5.5 high): five transcription fixes (all inert in March today), one C9c-2a verdict corrected (SPK), JUNE_BUGS 82 retracted, groups 1-4 -> new phase C9g

Checked against June in Ghidra (read-only): every function the kickoff lists, plus `31fa52b0`, `31f9a860`, `31fa89b0`, `31faa150` and the 200 functions
at `31f9xxxx-31faxxxx` (the `SetHideSelectUnitWeapon` search). funcmap: 3 rows corrected, 3 added (`Effect C9c-2b review`).

**C9c-2a re-verified**
- `viewer+0x1d8` = `m_ePlayedEmotion`: **confirmed**. June's PlayEmotion `31fa52b0` gates on `+0xe5c` exactly as March gates on `m_ePlayedEmotion` (`:6454-6459`).
- `31f99f70` = the viewer's move: **confirmed**. June's `SetPositionOnScr` is `31f9a860`, which calls it; seven other June sites call it directly. March
  moves viewers by hand at those sites, so only `SetPositionOnScr` sets the dirty byte. Inert: no particle of the 15 table sets uses land.
- `31fa4980`: **likely** June's `DeleteEnchantWeaponEffect`. It also stops group 0 on an equip refresh. Not ported -> **C9g** (below).
- **11 live rows: the count holds, one reason was wrong.** March *has* the Pyro Knight motion: `SPK_LobbyReadyaction` (lower-case a, data033
  `Motion_EL_Lobby.x`), and `PlayByMotionType` plays it (`:4996`). June renamed it `SPK_LobbyReadyAction` (data206), the same case as ALD. So
  March's hand-made SPK fire was live on the ready action, and retiring it lost that. **User 2026-10-07: accept the loss.** Recorded in checklist
  7e and in `c9c2a_hardcode_map.tsv`. `ALD_lobbyWait` / `ALD_LobbyWait` confirmed in both motion files.
- Draw ownership: as C9c-2b's play-test fix (JUNE_BUGS 92). Particles of a managed viewer are drawn once, by the DG manager, not twice as in June.

**C9c-2b against the binary.** These match June: Stop / stop-sounds, the create pass, the update's three branches, `FOLLOW_UNIT_VIEWER_SCALE`
and show, the sound pass, Prepare, Notify, ctor/dtor, `PlayAnim` `31fa3010`, the inline trigger (`-1, true, 0`), the armed test, and
`ANIMTIME_ON_START_UNITVIEWER`. The deliberate differences hold: a create that fails is retried every frame, and a set without a skin plays no
sound (JUNE_BUGS 80). "June dereferences a missing set" was wrong: June tests it (`@329596a4`) and skips silently, as the port does (the comment
is fixed). Fixed under the same flag:
1. **Land refresh** (`329585f0 @32958b1f-32958b31`). June copies `+0x1d4` once, before the particle loop, so it refreshes every land particle
   that frame. The port stopped after the first. **JUNE_BUGS 82 is retracted** (not a June bug).
2. **Play's tail `@3295987e` was missing.** It runs on every call, even when nothing is pushed (no skin `@32959680`, no set `@32959694`, no
   templet `@329596a4`). It re-links `BONE_TRACE_TARGET_MESH` targets over every entry. Now `JuneViewerFxLinkTraceTargets()`.
3. **Target-mesh branch (c).** June moves nothing unless the bone's frame is found on the target's skin; the port called `GetBonePos` anyway
   (meshes and particles).
4. **Viewer trigger** (`321dfc40 @321dfd9d`). June searches `BOUND_MOTION_TYPE_LIST` only with a non-zero code. The port fired a trigger bound
   to 0 with no emotion playing.
5. **Frame-move order** (`31fa8ce0`). The first base update is now June's first statement (`@31fa8d32-31fa8d4c`): before the `m_pUnit` test,
   `EmotionFrameMove` and the avatar-sound stop. The re-arm, the drain and the second update now follow the equip loop and the render-param
   block, as in June (`@31fa9271`, `@31fa93f6`, `@31fa9639`). Behaviour is unchanged today: the equip loop touches neither the anim time nor
   the entries.

Items 2 and 3 matter only for `BONE_TRACE_TARGET_MESH` sets (C9e's three emotion sets, inert). Item 4 matters only for trigger lists (none in the
15 sets). Item 1 matters only for land particles (none).

**`SetHideSelectUnitWeapon`: no June counterpart, the no-op stands.** No viewer-range function switches on those classes (the only `0x6f` hit,
`31f9d570`, is an item-id switch). Every March caller runs `PlayByMotionType` -> `PlayAnim` first, and that stops group 0.
**Retired sites:** only creation and the `m_bShow*` / crescent-cut writes are behind `#else`; every other site tests the handle. The orb is untouched.
**Data:** `UIMajorParticle.txt`: 2,729 sequences, no duplicates, braces balance. The last sequence is `CHUNG_STEEL_EDGE02` (not `CHUNG_HOWLING01`).
The 17:13 run logged no particle parse error (only the old `GameMajorParticleNN` probes). The orb's `ZEnable = FALSE` loss is confirmed (June has none).
`UIEffectSet.lua` = 78 templets in the log (52 + 15 + C9e's 11). JUNE_BUGS 79-81 and 83-86 are real as written; 85 stays not counted.
**Guard flags** (`flagprobe`): `MODIFY_EFFECT_SET_TRACE_BONE`, `FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE`, `FIX_ICE_HEATER_EVENT` and `RIDING_SYSTEM` are all ON.

**New phase C9g** (standing rule, unowned June branch): June's viewer groups 1-4 and their stops. That covers `31fa4980` (stops 0..4 on an equip
refresh), `PlayByMotionType` `31fa5c90` / `31fac190` (stops 0..4, or 0/1/2, on a motion change; `+0xf14` / `+0xf74` unidentified) and the
sit-emotion stops `31fa5140` / `31fa5180`. Kickoff `C9g.md`.

**Hygiene.** `flagcheck JUNE_UNIT_VIEWER_EFFECT_PLAYER --with JUNE_FX_DEBUG --range c71621a^..HEAD`: `files: 6 PASS, 5 PASS~ (blank/marker only), 1 DEFINE, 31 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 88 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The 88 deletions are this flag's own lines, rewritten by `c2029da` and by this review (the moved frame-move blocks). Against the base `c71621a^`, all five files show 0 deleted lines (`git diff --numstat`).
The 31 OTHER rows are peer commits in the range (`4ca07e7` C10b-1 and others). The 4 `tok!` are existing `JUNE_FX_DEBUG` `#else`/`#endif` lines in `X2EffectSet.cpp`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,313,152 B, dated 2026-10-07 17:52:02, md5 a87b307beb07ca847b5a1382f153d4b9, equal to X2/US_SERVICE/x2.exe (2026-10-07 17:51:57); built at mods/june-buff-port 9a8e26f + 4 uncommitted code file(s).` (the four are this review's)
**Regression play-test PASSED** (user, 2026-10-07 17:55-17:57, exe a87b307b; logs read, run after deploy): `UNIT_VIEWER_FX_LOAD ... rows=18`;
PLAY + CREATE for `EffectSet_ETK_LobbyReadyAction` twice (1 mesh + 4 crescent cuts), `EffectSet_EMK_LobbyWait_LobbyReadyAction` (`MagicKnightHandFire`),
`EffectSet_CDC_LobbyReadyAction` (both mini-guns), `EffectSet_SPK_LobbyWait_LobbyReadyAction` (P02 at once, P01 0.1 s later as its START_TIME says);
0 `NOT PLAYED` / `NOT CREATED`, 0 EXCEPTION, clean exit (census present); the only UNHANDLED is `E_DISABLE_HB_CHECK_REQ` (not this phase).

---

### C9d - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build)

**Verdict: two systems, not one.** The per-emotion effect set (`32355a50`) is effect wiring and is ported as **C9e**, inert. Everything
that would make it reachable is **June's emotion system**, which March lacks; that goes to the user (open question below).
Funcmap: 22 rows tagged `Effect C9d`. Ghidra was read-only.

**1. The ids.** June's `EMOTION_TYPE` (`Enum.lua`) runs to `ET_END = 65`; March's ends at `ET_EMOTION_AVATAR7 = 15`. Ids 0-15 are equal
(flagprobe: `CRAYONPOP_SECOND_EMOTION`, `CRAYONPOP_EMOTION_WITH_MUSIC` ON). The 11 sets belong to `0x1c-0x20` = AVATAR15-19 (Keroro),
`0x23` = AVATAR22, `0x26` = AVATAR25 (Twice), `0x27`/`0x28` = AVATAR26/27 (Xmas box), `0x2f`/`0x30` = AVATAR31/32 (Maid).
- **None of the 11 is in June's `EmotionData.lua`.** That file holds 20 entries: AVATAR1-7, 13, 29, 30 and 33-41. The 11 are code-only:
  - `3234cb00` (June's `IsAvatarEmotion`) falls back to a hard list;
  - `323fef90` has a hard sound switch.

  Porting `EmotionData.lua` alone would not reach them.
- **Where June's emotions come from:** `AddEmotionData` -> `32375250` (`GetEmotionData`, unit-manager map `+0x108`), with
  `CHANGE_EMOTION_CONDITION` (set items, items), motions, sounds and `DLG_ChatBox_Emotion_List.lua`. March has none of it: no Lua
  `EMOTION_TYPE` beyond a 4-entry Phase 11 stub, no file, no bind.

**2. Callers.**
- `323fef90` = `CX2SquareUnit::PlayEmotion` (March `X2SquareUnit.cpp:3616`): plays into `+0x7ac` through the square-unit overload `321e2120`.
- `31fa52b0` = `CX2UnitViewerUI::PlayEmotion` (March `:6269`): plays through the C9c-2b viewer player `32959640( skin anim, name, group 1 )`.
  This corrects C9b's "June's own viewer effect".

Both have March counterparts, so both are in C9e.

**3. The `+0x9434` lifecycle (GUUser).**
- **PlayEmotion** `@32043a33-32043b62`, just before C9c-1's `31fcf490`: if `+0x9434` is valid, stop it. Then play the set with every
  default on `g_pData +0x9c` and store the handle unconditionally (no `32296f30` tag).
- **OnFrameMove** `@3206eb64-3206ed70` switches on `m_ePlayedEmotion` (jump table decoded in funcmap):
  - sit and cash-sit ids stop the set;
  - the 11 (plus 41) keep it;
  - every other id stops it unless EmotionData gives a non-empty `UI_EFFECT_SET`. With no EmotionData in March, that case is June's NULL
    path: always stop.
- **Dtor** `@3203af9f`; **ctor** `@32041dc9` (= -1).
- **Square:** play `@323ff8c5`. It stops on every `StateStart` (`323fad80 @323fbd70`), in the dtor (`323f7760 @323f7b36`), and in
  `CX2TFieldGame::RemoveUnit` (`323af8f0`, via `323f4640`).
  - There is no stop before the play, and nothing leaks: March's `StateChange` (`:3721`) runs `StateStart` synchronously before the play.

**4. `+0x93dc` / `+0x93ec` / `+0x940c` are a second June-only system: equipped-item sit effects.**
- `3203e1b0` reads a `CX2Unit +0xa44` map (key `3264c980`: sit 7, cash1 8, cash2 13, cash3 14) and the item templet's per-mode effect list
  (`32651ab0`). The insert site was not found; March has neither the map nor the field.
- The drains in OnFrameMove, the dtor and PlayEmotion, and the viewer's group-1/2 block (`@31fa5645-31fa5983`), belong to it, not to C9e.
- `3204a860` re-runs mode 5 from CommonFrameMove.

**5. Assets.** All 11 templets are in June's `UIEffectSet.src.lua` (7 also in June's `EffectSet`); none are in March.
- Their particles resolve in the staged `UIMajorParticle.txt`, which C9c-2b replaced with June's today (re-check after it commits).
- 5 mesh players are June-only: `Mesh_Emotion_2015XMas_Box_Ready`/`_End`, `Emotion_2016Maid_Kettle`/`_Teacup`/`_Duster`
  (`2015xmas_Box_*.x`, `Emotion_2016Maid_*.X`).

**Port list (C9e, Sonnet, new flag, inert):** see `kickoffs/june-effect/C9e.md`. It covers the `32355a50` helper, the GUUser branch,
OnFrameMove case, dtor and ctor, the square play and its 3 stops, the viewer play (after C9c-2b), and staging the 11 templets plus the 5
mesh-player entries.

**Open question for the user:** June's emotion system is a missing system that no backlog stage owns. It covers EMOTION_TYPE 16-64,
`EmotionData.lua` + bind + `GetEmotionData`, the data-driven `IsAvatarEmotion`, cash sits, emotion items, the sit-item effects in §4,
motions, sounds and the chat-box emotion list. Where should it go?
- (a) its own backlog stage (recommended);
- (b) Stage 8, which already lists `EmotionData` for Lu/Ciel;
- (c) Stage 9 UI sweep.

**Answered 2026-10-07: (a).** It is `JUNE_PORT_BACKLOG_ORDER.md` Stage 10 (stage details + Opus planning prompt).

---

### C9e - Done (2026-10-07, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET`; inert, regression play-test pending)

**Landed** (this commit, `git log --grep C9e`; details in `MODS.md`, "June effect completion C9e"). Everything is June verbatim and **inert**: March's
`EMOTION_TYPE` ends at 15, the 11 June ids start at `0x1c`, so no emote ever matches. Nothing logs.
- **Helper `32355a50`**: `CX2Unit::GetJuneEmotionEffectSetName( int )`, inline in `X2Unit.h`, June's 11 ids (`0x1c-0x20` Keroro, Kururu, Giroro, Dororo, Tamama;
  `0x23` GirlFriend_Heart; `0x26` Twice; `0x27`/`0x28` 2015XMas_Box_Start/End; `0x2f`/`0x30` 2016Maid_Man/_girl) with the enum names in comments, `L""` otherwise.
- **GUUser `+0x9434`** (`m_hJuneEmotionSetEffectSet`): ctor -1; `PlayEmotion` right before C9c-1's block (stop the old handle, play with every default, store
  unconditionally, no `32296f30` tag); `OnFrameMove` right after `CX2GameUnit::OnFrameMove` (June's switch on `(int)m_ePlayedEmotion`: stop 1-3 / 25-27 / 54-59, keep
  28-32 / 35 / 38-41 / 47 / 48, default stop = June's `GetEmotionData == NULL` path); dtor head before C9c-1's stop.
- **SquareUnit `+0x7ac`** (`m_hJuneEmotionEffectSet`): ctor -1; `PlayEmotion` between the sound-name switch and the sound open (`CX2SquareUnitPtr( this )` overload,
  no stop before the play; intrusive pointer confirmed: `X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR` is defined at `KTDX.h:340`); `StateStart` stop after the PLAY_SPEED read;
  dtor stop; new public `StopJuneEmotionEffectSet()` (= `323f4640`) called from `CX2TFieldGame::RemoveUnit` before the list erase.
- **Viewer**: `CX2UnitViewerUI::PlayEmotion` calls `JuneViewerFxPlay( m_pXSkinAnim, name, 1 )` after March's gates (nested under `..._VIEWER_EFFECT_PLAYER`).
  **Correction to C9c-2b**: its `JuneViewerFxPlay` lacked June's first test (`32959640 @32959680`: `TEST EBX,EBX; JZ 3295987e`, a NULL skin anim returns), so I added
  `if( NULL == pXSkinAnim_ ) return;` under this flag. Group 1 is stopped by the viewer dtor (C9c-2b) and by June's sit-item block (missing emotion system).
- **Data** (`data036`): `UIEffectSet.lua` = C9c-2b's sets + the 11 templets appended verbatim from June's source (`UIEffectSet.src.lua` appended, `kom.py compile`);
  `UIMajorXMeshPlayer.txt` = March's + the 5 entries (`Mesh_Emotion_2015XMas_Box_Ready`/`_End`, `Emotion_2016Maid_Kettle`/`_Teacup`/`_Duster`) inserted before the closing brace.
  All keys parse (`LifeTime`, `LightPos` included); the 26 particles of the 11 sets resolve in the committed `UIMajorParticle.txt`.
  `PACK OK data036: ... 190 entries, 6625437 bytes, ~2 changed (UIEffectSet.lua, UIMajorXMeshPlayer.txt)`. **Copied (user said yes): `COPY OK data036: ...data036.kom 6625437 bytes, 190 entries, md5 9f7546891c60793f7216767a52ad2398`, backup `kom_backup_tools/data036.kom.20261007_170807`.**
- **Not ported, by the kickoff**: the `+0x93dc` / `+0x93ec` / `+0x940c` sit-item vectors and every `EmotionData` path (the missing emotion system, Stage 10);
  `323f34a0` / `323f6fb0( 0 )`; June's skin-anim gate around the `StateStart` stop (inert either way); **`323f56d0`** (called in `RemoveUnit` beside `323f4640`,
  not identified: the review identifies it).
- **Not staged**: the 5 meshes `2015xmas_Box_Ready.x`, `2015xmas_Box_End.x`, `Emotion_2016Maid_Kettle.X`, `_Teacup.X`, `_Duster.X` ship with the emotion system. The mesh entries
  name them, but nothing plays them.
- **Open for the review**: the GUUser play sits inside March's `if( m_pXSkinAnim != NULL )` because C9c-1's block does (kickoff: "immediately before C9c-1's block"); the
  review checks whether June has `@32043a33` under a skin-anim test.

**Hygiene.** `flagcheck JUNE_EMOTION_EFFECT_SET --worktree --files <9 code files>`: `files: 7 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`
(the PASS~ is the one blank line I put after the helper in `X2Unit.h`). Range (`--range 9e86741^..9e86741`): `files: 7 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; same `in this flag's files` line (all zeros); `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,313,152 B, dated 2026-10-07 17:01:48, md5 f1966238bff0202979961b5cf774df05, equal to X2/US_SERVICE/x2.exe (2026-10-07 17:01:44); built at mods/june-buff-port 2847079 + 9 uncommitted code file(s).`
`--pch force` (Always.h, X2Unit.h, X2GUUser.h, X2SquareUnit.h are all in the PCH). **Proof the code is in the exe** (nothing logs): all 11 set names are in `x2.exe` as UTF-16 (1 hit each; the helper is inline, so they only
reach the exe if a call site compiled with the flag), and `X2.map` holds `CX2SquareUnit::StopJuneEmotionEffectSet`.

**How to see it in game.** You cannot: it is inert. The regression play-test is the review's: emote in the village, in the character select and in a dungeon, village -> dungeon -> village,
clean exit. 0 EXCEPTION, no new `X2_*.tmp`, the C9c line `UNIT_VIEWER_FX_LOAD ... rows=18` unchanged. Once `data036` is copied the 11 templets load with the other sets (`EffectSet` load lines unchanged).

**Regression play-test PASSED 2026-10-07** (exe `f1966238`, run 17:09:27-17:10:44, after the `data036` copy; logs read): State.log login -> server select -> village -> dungeon -> village; ErrorLog `JUNE_FX ES_LOAD UIEffectSet.lua loaded=1 templets=78` (67 + the 11) and `UNIT_VIEWER_FX_LOAD ... rows=18 skipped_duplicate=0 skipped_bad_args=0` unchanged; 0 EXCEPTION, clean exit (census + `SHUTDOWN clean`), no new `X2_*.tmp` (the newest is from 2026-10-03). The only UNHANDLED packet is `E_DISABLE_HB_CHECK_REQ` (not this phase). The `GameMajorParticle24+` KEM_ERROR136 lines are old and unrelated. Emotes leave no packet offline, so the user's emote check is the evidence that they did not fault.

### C9e - Review (2026-10-07, Opus 5.5 high): two findings fixed (GUUser placement, incl. C9c-1's block; square StateStart gate), `323f56d0` -> new phase C9f

Checked against June in Ghidra (read-only) and the exe bytes (`disasm_range.py`, the jump table read from `unpacked_x2.exe`).
- **Hygiene** (`flagcheck JUNE_EMOTION_EFFECT_SET --range 9e86741^..HEAD`): `files: 7 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  The OTHER row is the peer commit `c2029da` (C9c-2b play-test 1, `..._VIEWER_EFFECT_PLAYER`) in `X2BaseUnitViewerUI.cpp`. It does not touch C9e's lines.

**Correct as ported**
- **`32355a50`**: the 11 ids and the 11 UTF-16 strings match. Each name occurs once in June's `UIEffectSet.src.lua`. `0x1c-0x20` = Keroro, Kururu,
  Giroro, Dororo, Tamama.
- **GUUser `OnFrameMove`**: decoded from the bytes (targets `32070188` = `3206eba6`/`3206ebc6`/`3206ed75`/`3206ebdd`).
  - Stop: 1-3, 25-27, 54-59. Keep: 28-32, 35, 38-41, 47, 48. Default: everything else, including 0 and > 59 (the `JA`).
  - The default path skips only the `+0x93dc`/`+0x93ec` drains, then stops `+0x9434` when `GetEmotionData` is NULL (`@3206ed63`). This matches the port.
- **The play**: it plays only when the name is not empty (`31cda210` against `L""`) and `g_pData +0x9c` is set. It stops a valid old handle first and stores the new one.
- **Square**:
  - play `@323ff8c5-323ff93e` (`321e2120( name, this )`, no stop first, stored into `+0x7ac`);
  - dtor `@323f7b36`;
  - `323f4640` (its `323f34a0( 0 )` is the sit-item system).
- **Viewer** `@31fa5995-31fa59d2` (`32959640( 31fac3c0(), name, 1 )` on a non-empty name).
- **NULL skin anim** (`32959680`): June jumps to the shared tail `3295987e`, not to an empty return. That tail re-links the trace-target mesh
  (entry `+0xc` = `+0x3c`) when an entry's EffectData `+0xfc` == 1. C9c-2b's port returns early on its other two exits (`3295987e` too), so
  the early return is equivalent. Only the code comment "does nothing for it" is loose. It is left alone: a peer is editing that file.
- **Data**:
  - `UIEffectSet.src.lua` = 78 templets, all unique: the 67 first, then the 11 in the helper's order, each identical to June's block. None of
    the 11 replaced an existing templet.
  - `UIMajorXMeshPlayer.txt` = March's (`march_data/data036`) plus one 104-line insertion (the 5 templets) before the closing `}`, CRLF kept.
  - The 26 `ET_PARTICLE_UI_MAJOR` names all resolve in `UIMajorParticle.txt`, and each of the 5 mesh names is there once.

**Finding 1 (fixed, both flags): June plays the per-emotion set and C9c-1's table set before its skin-anim test.**
- June's skin-anim test is `32071d80 @32043d02` with `JZ @32043d25`, and the anim change comes after it.
- The per-emotion block (`@32043a33-32043b62`) and C9c-1's block (`31fcf490 @32043b64` stop + table play `@32043b69-32043cf5`) both run
  before it, right after the guards.
- The port had both inside March's `if( m_pXSkinAnim != NULL )`. Both now sit after the STANDUP guard and before that test, in June's order,
  each under its own flag. C9c-1's comment says so.
- Flags off, the function is now identical to the March base. The moved separator line was dropped, so the extra blank HEAD had after
  `m_ePlayedEmotion = eEmotionId;` is gone.
- Effect today: none (inert ids; a GUUser always has a skin anim).

**Finding 2 (fixed): the square `StateStart` stop is gated in June.**
- The gate is `@323fbca2-323fbcb7`: `m_pUnitViewer` (`+0x2ec`) and `GetXSkinAnim()` (`31fac3c0`). It is now ported verbatim.

**`323f56d0` identified: not ported, planned as C9f.**
- It walks the leaving unit's viewer equips (`31fa2f20( 0x51 )`). For each equip found in an item-id map (`3280b640`, `g_pData +0x34`), it stops
  the equip's own effect set (`3295ca60`, `CX2Eqip +0x15f8`).
- That is a June-only **per-equip effect-set** system. It is played from GUUser `CommonFrameMove @32066027` and from square `32400620 @32402225`,
  with timers. It is not the emotion system.
- March has no handle and no map, and no STATUS row or backlog stage owns it. So it is **C9f** (Opus, identify: the loader is not found yet),
  then C9f-2 (port).
- Funcmap: 6 rows `Effect C9e review`, all `candidate`.

**Hygiene of the fixes** (`--worktree --files X2Lib/X2GUUser.cpp X2Lib/X2SquareUnit.cpp`): RESULT PASS. `X2SquareUnit.cpp` PASS (+3 -2:
the 2 deleted lines are this flag's own old comment line and condition). `X2GUUser.cpp` OTHER, because the move spans two flags. Its 49
deleted lines are the two moved blocks. With both flags stripped, the worktree equals HEAD, minus HEAD's extra blank.
Range (`--range b3ed2a3^..b3ed2a3`):
- `JUNE_EMOTION_EFFECT_SET`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `2 deleted line(s)` (the square lines above), every
  other count 0; `RESULT: PASS`.
- `JUNE_UNIT_VIEWER_EFFECT_TABLE`: `files: 0 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; all counts 0; `RESULT: PASS`.

The OTHER rows are this commit's two files, and each holds both flags' changes.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,313,152 B, dated 2026-10-07 17:40:43, md5 30a7d90c9eea6c7b5d5c2b80f463d20b, equal to X2/US_SERVICE/x2.exe (2026-10-07 17:40:38); built at mods/june-buff-port 2751f9c + 5 uncommitted code file(s).`
The 5 are the 2 files of this review plus a peer's C10b-1 work in progress (`Always.h`, `X2GameUnit.cpp/.h`, and one hunk in `X2GUUser.cpp` at `DamageReact`), so that work is in this exe too.

**Play-test (regression):** an emote in the village, at character select and in a dungeon; village -> dungeon -> village; clean exit.
Pass = 0 EXCEPTION, no new `X2_*.tmp`, and `UNIT_VIEWER_FX_LOAD ... rows=18` plus `ES_LOAD UIEffectSet.lua loaded=1 templets=78` unchanged.
**Regression play-test PASSED 2026-10-07** (exe `30a7d90c`, run 17:47:19-17:49:20; logs read):
- State.log: login -> server select -> battle field -> village -> dungeon -> village.
- ErrorLog: `ES_LOAD UIEffectSet.lua loaded=1 templets=78` and `UNIT_VIEWER_FX_LOAD ... rows=18 skipped_duplicate=0 skipped_bad_args=0`
  unchanged. The one `UNIT_VIEWER_FX_PLAY` line (`EffectSet_RNW_LobbyReadyAction`, character select) is C9c-2b's.
- 0 EXCEPTION, clean exit (census + `SHUTDOWN clean`), no new `X2_*.tmp` (the newest is from 2026-10-03). The 2 UNHANDLED are
  `E_DISABLE_HB_CHECK_REQ`, not this phase.

### C9f - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build): June's per-equip effect sets = `DITT_CONDITION_EQUIP_PLAY_EFFECT_SET`, one slice of a missing system

Read-only, in Ghidra and the exe bytes. 17 funcmap rows are tagged `Effect C9f`: the 6 C9e-review rows are confirmed or corrected, and 11 are new.
The full semantics are in those rows; this note keeps the map.

**What the system is.** "CEPE" is one of June's 112 `DIVISION_ITEM_TEMPLET_TYPE`s (81). March has **none** of that system: no
`AddDivisionItemTemplet_LUA`, no `DITT_*`, no `+0x230` / `+0x380` maps on `CX2ItemManager`.
- **Loader.** June's `ResetItemManager` (`31edda90`) loads `RenderDivisionItemTemplet.lua` (113 blocks) and `DivisionItemTemplet.lua` (24).
  - Each block goes to the dispatcher `32818480`. It skips `m_eReadType` 0/2. It sends `m_eType` 55, 85 and 89-93 to their own targets, and every other type to a chain of six parsers.
  - `32814da0`, one of the six, reads `m_EffectSetList%d` on every block.
  - June's only CEPE block is in `RenderDivisionItemTemplet.lua`: `m_EffectSetList0-4`, items 289162, 289165, 30572, 291516 and 291518.
- **Record**: `{ m_eCondition, EFFECT_SET_NAME, START_TIME, SWITCH_EFFECT_SET_NAME, SWITCH_EFFECT_PERCENT (clamped 0-100), SWITCH_LIMIT_TIME }`.
  - It goes into the item-id map `+0x380` and into the group map `+0x230[81]`. Both inserts keep the first entry.
  - The key names are XOR-encrypted in the exe. Decoded by hand, they match the Lua exactly.
- **Conditions** (`CONDITION_EQUIP_PLAY_EFFECT_SET`): 1 WAIT, 2 NOT_MOVE, 3 ALWYAS (June's spelling).
  - GUUser `31fd8140`: WAIT = state `USI_WAIT` (13). NOT_MOVE = not in a skill state, and moved <= 1.0 since last frame (`+0x94d4`).
  - Square `323f5920`: WAIT = `GetStateID("WAIT")`. NOT_MOVE = moved <= 1.0 (`+0x7c0`).
  - Viewer: motion type <= 2 (the three waits).
- **Equip state** (`CX2Eqip`, June ctor `32961280`):
  - `+0x15f4` start accumulator;
  - `+0x15f8` effect-set handle;
  - `+0x15fc` / `+0x1600` / `+0x1604` switch timers, one per owner kind (GUUser / square / viewer).
  - The key is `m_ItemID` (`+0x1a0`), the displayed templet's id. The gate is `m_pItemTemplet` (`+0x198`).
- **Timer step `32961060`.**
  - The first play waits `START_TIME`, and a set that ended replays about a frame later.
  - Once a switch timer reaches `SWITCH_LIMIT_TIME`, each play rolls `rand()%100 < SWITCH_EFFECT_PERCENT` for the switch set. The caller zeroes the timer when the switch set plays.
  - It uses the global elapsed time.
- **Play sites** (all on `g_pData +0x9c`, the UI effect set, in dungeons too):
  - GUUser `CommonFrameMove`, in the `m_ViewEqipList` loop before each equip's `OnFrameMove`;
  - square `OnFrameMove`, with the equips from `m_pUnitViewer` filtered by group 81;
  - viewer `31fa8ce0`, through June's viewer player, group 1. The handle is never stored. The site is skipped for `UVS_FIELD` viewers, so a square unit does not play the set twice.
- **Stops**: `RemoveUnit` -> `323f56d0` stops (true). The GUUser/square miss paths release the handle without stopping it (false). There is no equip-dtor stop.

**Reach: inert in March.**
- None of the 5 ids is in March's `item.kim`. Control: all 280 item ids in the save, and the beginner-gear ids, are found there as LE ints.
- None is on the save or in `cash_product`.
- None of the 6 sets is in March's staged `UIEffectSet.src.lua`.
- The proof for C9f-2 is therefore a load line with the right counts, plus a regression play-test.

**What blocks loading June's file in March.** A scan of every global `RenderDivisionItemTemplet.src.lua` touches:
- `RENDER_TYPE`, `CARTOON_TEX_TYPE`, `D3DCULL`, `D3DBLEND`, `D3DXVECTOR3`, `D3DXCOLOR`, `TRUE` and `FALSE` exist in March.
- **`DIVISION_ITEM_TEMPLET_TYPE` (81 members used), `CONDITION_EQUIP_PLAY_EFFECT_SET` and `READ_TYPE` do not.** The first block would raise
  "attempt to index nil", and the rest of the file would not run.

**Port list (C9f-2, `kickoffs/june-effect/C9f-2.md` has the details):**
- the item-manager binding, the dispatcher shell, parser `32814da0`, the two maps, and the load line;
- `CX2Eqip`'s five members, `3295ca60` and `32961060`; the viewer group filter `31fa2f20`;
- the GUUser, square and viewer condition checks and play blocks, plus the two last-position members;
- `323f56d0` in `RemoveUnit`, beside C9e's `StopJuneEmotionEffectSet()`;
- the data (the three enum tables, the file, the 6 sets) and three latent JUNE_BUGS rows (first-wins group map; the miss path releases without stopping; no equip-dtor stop).

**Not ported: backlog Stage 11** (`JUNE_PORT_BACKLOG_ORDER.md`, standing rule: an unowned June system gets its own stage). It holds the rest of the
division system:
- the dispatcher's other targets and the five other parsers;
- `DivisionItemTemplet.lua` / `CoboRenderDivisionItemTemplet.lua`;
- the `DITT_LOOP_ANIM_ITEM_ID` (72) block that precedes CEPE in GUUser's loop (`3295c960`).

**The missing enum tables are not a question: there is precedent.** Effect Phase 11 appended 7 new June tables verbatim to March's
`Enum.src.lua`, with no flag (MODS.md, "June2017 effect engine: enum data + loader-list gaps (Phase 11)"). C9f-2 does the same for
`DIVISION_ITEM_TEMPLET_TYPE`, `CONDITION_EQUIP_PLAY_EFFECT_SET` and `READ_TYPE`. None of the three names exists in March.

C9f-2 (Sonnet 5.5 high) starts after the running C4c commits, because both touch `X2Main.cpp`.

### C9f-2 - Done (2026-10-07, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET`; inert, regression play-test pending)

**Landed** (`dde622a`; MODS.md "June effect completion C9f-2"). June verbatim, **inert**: none of the 5 item ids is in March's `item.kim`.
- **Item manager**: `AddDivisionItemTemplet_LUA` (`32818480`), bound once on `CX2ItemManager` in `X2Main.cpp`. It sits outside
  `#ifndef X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING`, which is ON (`AddItemTemplet` there is dead code). June has no proxy bind: `32818480` has one
  xref (`31db330e`), and every push to that `class_def` (`31e75970`) is a `328xxxxx` item-manager method. The kickoff's "one binding per proxy" does
  not match June. The dispatcher skips `m_eReadType` 0/2 or a failed getter, and returns for 55/85 and 89-93 (Stage 11 comments). Every other type
  runs `32814da0`, the only chain parser ported. The parser keeps both first-wins inserts, the 0x51 group key and the clamp.
  `GetConditionEquipPlayEffectSet` (`3280b640`), `IsDivisionItem` (`32807db0`). Loaded in `ResetItemManager` after `ItemConvertTable.lua`.
- **`CX2Eqip`**: June's five members (ctor tail), `3295ca60`, `32961060`. The global elapsed time is `g_pKTDXApp->GetElapsedTime()`: June's
  `+0x2c` protected float is initialised to 1/85 in ctor `31c17ed0`, which is March's `m_fConstElapsedTime`. `X2EffectSet.h` is included after
  `X2Eqip.h`, so `X2Eqip.h` repeats the handle typedef with a forward-declared tag.
- **GUUser** (`31fd8140`, play `@32065d57-32066052`, last position `+0x94d4` ctor 0 `@32043273` / stored at the end of `CommonFrameMove`
  `@3206cee2`). The play block is in the live `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` branch only (ON per `cl /P`, `june2017_re/flagprobe.py`),
  between `SetRenderParam` and `OnFrameMove`.
- **Square** (`323f5920`, play `@32402089-32402292`, `+0x7c0` ctor 0 `@323f90cb` / tail store). June's order: HANABI, `323f17b0`,
  `param_1[400]` = `m_fCanNotInputTime` decrement, June-only sit-wait blocks, CEPE. So the block goes after `MODIFY_LINEMAP_JUMPUP_IN_VILLAGE`.
  `StopJuneEquipEffectSets` (`323f56d0`) is in `RemoveUnit` after C9e's stop. C9e's comment there ("323f56d0 is not identified") is left as is,
  with a note under it.
- **Viewer**: `GetJuneDivisionEquips` (`31fa2f20`), and `JuneViewerFxIsPlaying` (`329581c0`) on `CX2BaseUnitViewerUI`. The play block goes after
  `UpdateTitle()`, since `31f91d10` (title-id compare) is `UpdateTitle`. It is skipped for `UVS_FIELD`: `+0xf14` is `m_eUnitViewerState`, because
  `SetUnit` `31faa6f0` stores `eState` there `@31faa73c` and state 3 loads the Field motion. June's `+0xfa4` is written together with `+0xe58` =
  `m_eOldMotionType` by `31fa7ef0`, and June `PlayEmotion` tests `+0xe58 == 4` (READY). Only June's random-wait path zeroes `+0xfa4` alone, and only
  for types 1-2, which pass the `<= 2` test either way. The test is an unsigned `JA`.
- **JUNE_BUGS 100-102** (all latent): first-wins group map; the miss path releases without a stop; no equip-dtor stop.

**Data** (`data036`; backups in the session scratchpad):
- `Enum.lua` gets June's `DIVISION_ITEM_TEMPLET_TYPE`, `READ_TYPE` and `CONDITION_EQUIP_PLAY_EFFECT_SET`, verbatim.
- `RenderDivisionItemTemplet.lua` is June's file verbatim (113 blocks), with a new manifest entry `"2"`. All 95 enum references resolve.
- `UIEffectSet.lua`: 78 + 6 = **84** templets.
- `UIMajorXMeshPlayer.txt` gets `Mesh_AC_Lowbody_Set7_Shake` / `_2P` before the closing brace. March's parser knows all their keys.
- The 29 particles already resolve. The two meshes' binaries (`AC_Lowbody_Set7_Shake.x`, `AC_Lowbody_Set7_Tail_2P.xet`) exist only in June's
  `data002` and are not staged (inert path).
- Round trips (MODS "Phase 11" method):
  - `Enum`: the old bytecode equals a fresh compile of the old source, and the decompile is old + 126 lines exactly.
  - `UIEffectSet`: the same check, old + 390 lines exactly.
  - `RenderDivisionItemTemplet`: the decompile has the same 8164 lines as June's source.
- `PACK OK data036: ...data036_packed.kom` 191 entries, 6644810 bytes; vs live: `+1 added` (`RenderDivisionItemTemplet.lua`), `~3 changed` (`Enum.lua`, `UIEffectSet.lua`, `UIMajorXMeshPlayer.txt`). Copied (user said yes): `COPY OK data036: ...\data\data036.kom  6644810 bytes, 191 entries, md5 00106631ffc3cf71a4b89c07f7c6b3fe`, backup `kom_backup_tools/data036.kom.20261007_202656`.

**Hygiene** (`--worktree --files` the 16 code files): `files: 13 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
Range (`--range dde622a^..dde622a`): the same three lines, `RESULT: PASS`.
The two PASS~ are the blank line after each new function block.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,329,024 B, dated 2026-10-07 20:24:08, md5 f433332e91f796e75dca57a9a1242f23, equal to X2/US_SERVICE/x2.exe (2026-10-07 20:24:03); built at mods/june-buff-port b045c37 + 16 uncommitted code file(s).` (the 16 = this phase's). The exe holds `RenderDivisionItemTemplet.lua` and `EQUIP_FX_LOAD` (UTF-16) and `AddDivisionItemTemplet_LUA`.

**Play-test (regression):** village, character select, a dungeon, back to the village, clean exit.
Pass = 0 EXCEPTION and no new `X2_*.tmp`, with these log lines:
- `JUNE_FX EQUIP_FX_LOAD file=RenderDivisionItemTemplet.lua loaded=1 lists=5 items=5 group81=5 skipped_duplicate=0`;
- `ES_LOAD UIEffectSet.lua loaded=1 templets=84`;
- `UNIT_VIEWER_FX_LOAD ... rows=18` unchanged.

**Regression play-test PASSED 2026-10-07** (exe `f433332e`, run 20:31:18-20:35:24; logs read): State.log login -> server select -> village -> dungeon -> result -> village; ErrorLog `JUNE_FX EQUIP_FX_LOAD file=RenderDivisionItemTemplet.lua loaded=1 lists=5 items=5 group81=5 skipped_duplicate=0`, `ES_LOAD UIEffectSet.lua loaded=1 templets=84`, `UNIT_VIEWER_FX_LOAD ... rows=18 skipped_duplicate=0`, no `XEM_ERROR9`; 0 EXCEPTION, clean exit (census present), no new `X2_*.tmp` (newest 2026-10-03). The 2 UNHANDLED are `E_DISABLE_HB_CHECK_REQ`, not this phase.

Next: `C9f-2 review` (Opus 5.5 high).

### C9f-2 - Review (2026-10-07, Opus 5.5 high): two transcription fixes (both inert), everything else matches June

Read-only in Ghidra and with `june2017_re/disasm_range.py` (raw bytes) against `dde622a`. Both fixes are under `SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET`.

**Fixed:**
1. **Dispatcher `32818480`: types 55/85 and 89-93 do reach the parser.** The jump-table targets are `328186a7` = `32816d60( table )` and
   `328186b5` = `328161d0( table, m_eType )`. Each ends in `jmp 328186d1`, which is the chain after its first call (`32814170`). So they
   replace only that first call and then run `32810700`, `328144f0`, **`32814da0`**, `32817470` and `32815af0`. C9f read them as separate
   exits, and the port `return`ed. The returns are gone, so every block that passes the `m_eReadType` gate is parsed.
   The load counts do not change: in June's file only block 95 (type 81) has `m_EffectSetList`, and block 0 is type 85.
2. **Timer step `32961060` @`32961233-32961244`: NaN polarity.** The code is `fld acc; fld START_TIME; fcompp; test ah,1; jne play`. C0 is
   also set when the compare is unordered, so a NaN on either side **plays**. The port's `START_TIME < acc` accumulated instead, and its
   comment said so. It is now `!( START_TIME >= acc )`. Equal values still accumulate.
- The funcmap rows `32818480` and `32961060` are corrected (tagged `C9f-2 review`).
- The C9f-2 kickoff's "types 55, 85 and 89-93 -> a comment" wording was the source of fix 1. That kickoff is consumed, so it is left as is.

**Confirmed against June:**
- **Parser `32814da0`.** Six keys and their getters (int / wstring / float). Defaults on a failed getter: 0 / L"".
- **Clamp** @`328157c8-32815801`. `FCOM 100.0; TEST AH,0x41; JNZ` sends below, equal and NaN to the low test. That test is
  `FCOMP 0; TEST AH,5; JP keep`, so only a value below 0 becomes 0, and NaN is kept.
- **Item and group inserts.** The `m_eItemID` loop runs 1..n until a getter fails. The item-id map is first-wins (`32825df0` / `32825c30`).
  The group key is `0x51` @`328159e9`, inserted after the loop only when the set is non-empty, first-wins.
- **Dispatcher.** `m_eReadType` 0 / 2 or a failed getter skips the block. A failed `m_eType` getter reads as 0 (`neg/sbb/and`).
  The byte table @`32818748` gives 55/85 -> 0, 89-93 -> 1, all other types -> 2.
- **`32961060`.** The roll needs `0 < percent` (`TEST AH,5; JP`; NaN gives no roll) and `0 <= sel < 3`.
  It rolls when `timer >= limit`, and a NaN accumulates. The switch fires on `rand()%100 < percent` (strict).
- **Elapsed time.** It is the global `+0x2c/+0x30/+0x34` protected float, which is March's `GetElapsedTime()` = `m_fConstElapsedTime`
  (1/85, `KTDXApp.cpp:337`).
- **GUUser.**
  - Order `@32065d57-32066052`: record, `31fd8140`, templet, `32961060`, `GetShowObject` (`31fae370`), then the play.
  - Miss path: `3295ca60( 0 )`, `+0x15f4 = 0`, `+0x15fc = 0`. A switch play zeroes `+0x15fc`.
  - Placement: `32963c80` is called @`32065bb4`, before the `3295c960` block and before `OnFrameMove` @`32066083`.
  - `31fd8140`: USI_WAIT = 13 (`GUSI_END` 5 + 8), skill vslot `0x524`, `!(len > 1.0)`, and the 3/0 cases via `dec`/`jne`.
- **Square** `@32402089-32402292`. The templet test comes before the lookup. There is no `GetShowObject` test. Sel is 1, the timer is
  `+0x1600`, and `+0x7c0` is stored after the loop. `323f5920` byte-compares `m_StateID` with `GetStateID("WAIT")` and does no skill test.
  `323f56d0` stops only on a record hit with a templet (`3295ca60( 1 )`).
  - Recorded deviation: June calls `31fa2f20` on `m_pUnitViewer` without a NULL test (`324020a8`); the port guards it.
- **Viewer** `@31fa9ab8-31fa9cb0`. The `+0xf14 == 3` skip, then record, `329581c0` (`EFFECT_SET_NAME`, rec+4), and the condition chain
  `<= 0` / `<= 2 && +0xfa4 JA 2` / `== 3`. Sel is 2, the group is 1, the switch zeroes `+0x1604`, and the handle is never stored.
  - June calls `31fa7ef0` (PlayByMotionType) right before this block (@`31fa9ab3`). March reads the `m_eOldMotionType` left by its own
    PlayByMotionType. That is C9g-2's rewrite; this review changes nothing there.
- **`CX2Eqip`.** March has one ctor and no early return. The five members, `3295ca60` (`bStop` and handle != -1, then -1) and the typedef
  were checked.
- **Data (`data036` staged).**
  - `Enum.src.lua` has June's three tables verbatim: 114 + 6 + 6 = 126 lines, one definition each.
  - `RenderDivisionItemTemplet.src.lua` is byte-identical to June's (113 blocks).
  - `UIEffectSet.src.lua` has 84 templets; the 6 CEPE sets are June's chunks verbatim.
  - `UIMajorXMeshPlayer.txt` has both `Mesh_AC_Lowbody_Set7_Shake*` templets.
  - No repack: the data is unchanged.

**Hygiene.** `--worktree --files X2Lib/X2ItemManager.cpp X2Lib/X2Eqip.cpp`: `files: 2 PASS`, `16 deleted line(s)`, `RESULT: PASS`.
The 16 deleted lines are this flag's own `dde622a` lines, replaced. Range `dde622a^..HEAD`: `files: 15 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 6 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 16 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 16 deleted lines are the same replaced flag lines (in `4ad8395`); the 6 OTHER are peer commit `8973e18` (C7: `JUNE_TOUCH_LAND` and others), not this flag.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,333,120 B, dated 2026-10-07 21:06:56, md5 5e72f88f9fc71cdfc79cc93778e6a84a, equal to X2/US_SERVICE/x2.exe (2026-10-07 21:06:50); built at mods/june-buff-port 9ba5b91 + 2 uncommitted code file(s).` (the 2 = this review's)

**Play-test (regression only; nothing new is reachable).** Village, character select, a dungeon, back to the village, clean exit.
Expect 0 EXCEPTION, no new `X2_*.tmp`, and the same lines as C9f-2:
`EQUIP_FX_LOAD ... loaded=1 lists=5 items=5 group81=5 skipped_duplicate=0`, `templets=84`, `rows=18`.

**Regression play-test PASSED 2026-10-07** (run 21:18:37-21:20:41; logs read). The exe was `4438a923`, a later redeploy built at `a8758a3` + 1 uncommitted peer file, so it contains this review's fixes.
- State.log: login -> server select -> village -> dungeon -> village -> server select.
- ErrorLog: `JUNE_FX EQUIP_FX_LOAD file=RenderDivisionItemTemplet.lua loaded=1 lists=5 items=5 group81=5 skipped_duplicate=0`, `ES_LOAD UIEffectSet.lua loaded=1 templets=84`, `UNIT_VIEWER_FX_LOAD ... rows=18 skipped_duplicate=0`.
- 0 EXCEPTION, clean exit (census present), no new `X2_*.tmp` (newest 2026-10-03). The 2 UNHANDLED are `E_DISABLE_HB_CHECK_REQ`, not this phase.

---

### C9g - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build): groups 1-4 are June's customize system; PlayByMotionType is a June rewrite -> C9g-2

Read-only in Ghidra. 30 funcmap rows `Effect C9g`; new rows correct `31fa4980` (**not** DeleteEnchantWeaponEffect), `31fa5c90` (not
PlayByMotionType) and `32651ab0` (not the item manager). Census: `june2017_re/c9g_animation_name_census.tsv` (script `.py` beside it).

**Who plays each group** (every `32959640` caller):
- **Group 0:** the effect table (C9c-2b, ported).
- **Group 1:**
  - ported: the per-emotion sets and CEPE (C9e, C9f-2);
  - Stage 10: EmotionData `UI_EFFECT_SET` (`31fa52b0 @31fa5a0a`);
  - Stage 12: the custom per-motion sets (`31f968a0`, played by `31fa5c90` and square StateStart `323fad80`) and the custom
    SIT_READY sets;
  - C9h: the pet / riding-pet `INIT_EFFECT` lists (`ResetPet` `32e50de0`, `ResetRidingPet` `32e51ea0`).
- **Group 2:** the custom SIT_WAIT sets (Stage 12).
- **Groups 3-4:** the customize preview dialogs `3224b340` / `3224d0f0` / `32d77220` (Stage 12).

**The stops.**
- `31fa4980` = June's customize reset. It clears six custom motion names, three sit arrays and two maps, then stops 0..4. Callers
  are `ApplyCustomize(1)` `31fa89b0` and the preview UI.
- June keeps March's enchant particles (`31f9eeb0`): nothing of `m_hSeqWeapon_*` was replaced.
- `31fa5140`/`31fa5180` replay the sit sets (Stage 12).
- The dtor's 0..4 stop is already ported (`X2UnitViewerUI.cpp:288`).

**PlayByMotionType = `31fa7ef0`, a rewrite.** It first calls the customize pre-pass `31fa5c90`.
- **The pre-pass head is data-independent:**
  - `m_ePlayedEmotion = ET_NONE` unless the viewer state is June's 10;
  - except for HABIT/SIT_READY/SIT_WAIT, it stops groups 0..4 (0..2 in `UVS_CASHSHOP`).
- `+0xf14` = `m_eUnitViewerState`; `+0xf74` = the length of the custom WAIT string.
- **Names come from `GetAnimationName` `32375ee0`** over `AnimationNameTable.lua`, with March's defaults as fallback. The table also
  replaces the square's lobby-wait switch (`323f2320`).
- **The enum is renumbered:** HABIT 6, SIT 7-9, WAIT_DELETE 6 -> 10.
- **WAIT1/WAIT2 loop** in June; March plays them once (`X2Room.cpp:1704/1709`).
- **The Eve level-up sound is kept.**
- **The second argument `bTransition` is not always 1:** literal 0 at two sites and a register at about eight (funcmap row);
  C9g-2 maps them first.

**Census** (384 rows):

| Rows | Verdict |
|---|---|
| 160 | classes missing from March's Lua `UNIT_CLASS`; they load as `UC_NONE` (expect `inserted=230 skipped_duplicate=154`) |
| 133 | same name as March |
| 82 | same name, play type changes |
| 2 | new BM rows (motion present) |
| 7 | June renamed the motion |

The 7 renamed rows are `PR_LobbyDownLanding`, `ALD_LobbyWait` x3, `AYR_LobbyReadyAction` and `SSK_`/`SPK_LobbyReadyAction`; C9g-2 adopts
4 June motion files. As a result, June's table plays the ALD orb, so the kept hard-code goes dead, and SPK's ready-action fire returns.
The user hears this at C9g-2's hand-off.

**Reach.**
- Groups 2-4 have none in March data: 13 of the 1,270 custom ids are in March's `item.kim`, none in the save.
- The user will adopt June's `item.kim`, which has 1,066 of them, so Stage 12 is real data.

**Routing:**
- **C9g-2** (Sonnet 5.5 high, flag `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE`);
- **C9h** (Opus 5.5 high, identify);
- new **backlog Stage 12**, the character customize system, which takes over C9d's sit-effect bullets from Stage 10.

---

### C9g-2 - Done (2026-10-07/08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE`; play-test pending)

**Landed** (MODS.md "June effect completion C9g-2"). June's `PlayByMotionType` `31fa7ef0` over `AnimationNameTable.lua`, whole function:
- **Enum** (`X2UnitViewerUI.h`): `UVUMT_NONE -1`, WAIT..LEVEL_UP 0-5, `HABIT 6`, `SIT_READY 7`, `SIT_WAIT 8`, `STANDUP 9`, `WAIT_DELETE 10`. Every
  caller names the enum (no number is stored or compared), so the renumber is safe. `PlayByMotionType( type, bool bTransition = true )`.
- **Table** (`X2UnitManager`): bind `AddAnimationName` (`3237a060`, `X2Main.cpp`), map keyed (class, type), **first row wins** (`3238b0e0` frees the
  new node on an equal key), a nil class reads 0 and is inserted (June's typed reads), `GetAnimationName` (`32375ee0`, defaults verbatim, no class-0
  fallback). Loaded in `ResetUnitManager` before the C9c-1 table (`31ee6217` before `31ee6224`). A script rerun of the load against March's
  `UNIT_CLASS` (56 entries) predicts `rows=384 inserted=230 skipped_duplicate=154 class_none=160`, as the kickoff expected.
- **`PlayByMotionType`**, June's order: `m_eOldMotionType`, `m_iJuneFxTriggerCode2` (7 -> 2, 8 -> 1), the `31fa5c90` head (`m_ePlayedEmotion = ET_NONE`
  unless state 10; group stops 0..4, cash shop 0..2, skipped for types 6/7/8), then the unit tests, the Rose remap (type 0xb, inert), the name, `PlayAnim`
  (0-2 LOOP, 8 LOOP, rest ONE_WAIT), Eve's level-up sound, 7/8/9 with their literal names. Customize jump table / custom WAIT shortcut: cited, Stage 12.
- **Square**: `InitWaitMotion` takes `GetAnimationName( class, WAIT )` (`323f2320`; `323fa700` calls it when the customize test `323f92f0` is false).
  The 7/8/9 names `323fa700` also sets are Stage 10.
- **C9c-2b's per-frame `m_iJuneFxTriggerCode2 = 0`** is bypassed under this flag: June's frame move (`31fa8d32-31fa8d46`) writes only `+0x1d8`.

**Call-site mapping (corrects the funcmap row).** All 55 June callers pass `bTransition` 1 except `32d7c962` (the customize preview, Stage 12).
The "literal 0" sites were the *type* (the thiscall's last push); `PUSH 0` at `3251248a`, `324e361c`, `32532461`, `32228c65`, `324e7e27/63` is type WAIT with
the 1 pushed before the branch; the register sites hold 1 (`MOV EBX,1` / `MOV EBP,1`); `325648d3` pushes type 0xa (WAIT_DELETE, = March
`X2StateServerSelect.cpp:7102`) or 3. So March's default `true` **is** June's behaviour; no March caller needs an argument.

**Identified, not ported** (funcmap rows `Effect C9g-2`):
- `+0xfa0` = float idle (HABIT) timer, `+0xf9c` its limit; `31fa8ce0 @31fa99f1-31fa9ab8` plays HABIT (type 6) behind `+0xf90` (customize) or unit
  type 0xb (Rose). No March member -> Stage 12 / checklist 7g. `+0xfa4` = type copy, read only by the customize preview `32228af0`.
  `+0xe60` = byte set only by PlayEmotion's customize path (`@31fa5c30`), cleared here. `+0xd82` = duo shown.
- `31fa5c90` returns false for type >= 10 or -1 (`XOR AL,AL @31fa634a`), so WAIT_DELETE plays `LobbyLose`; `SERV_UNIT_WAIT_DELETE` is on for US
  (`ServerDefine_US.h:48`), as in March. No JUNE_BUGS row (no June quirk ported).
- `SelectVoiceTable.lua` (opened between Correlate and this table) is still unported; C9c-1's "not ported here" comment is stale for AnimationNameTable.

**Data.**
- `data036`: `Enum.src.lua` + June's `UNIT_VIEWER_UI_MOTION_TYPE` (decompile = old + 14 lines exactly); `AnimationNameTable.lua` (June source verbatim,
  decompile = source, 384 rows) + manifest `"AnimationNameTable.lua": "2"`. `PACK OK data036` 192 entries, `+1 added, ~1 changed` (Enum.lua).
- `data033`: June's `Motion_EL_Lobby.x` (data206), `Motion_ARA_Lobby.x`, `Motion_ARA_Common.x` (data205), `Motion_EVE_Common.x` (data203) replace March's
  (sizes 2007749 / 1466552 / 6054910 / 6062982; algo 0 as before). Names re-checked with a length-prefixed parse: March-only names are only the
  renamed ones (`PR_LobbyDownlanding`, `ALD_lobbyWait`, `AYR_LobbyReadyAtcion`, `SSK_/SPK_LobbyReadyaction`). **Wholeness gate:** 3 files extract at
  the manifest size; **`Motion_ARA_Lobby.x` was 1 byte short** (last name field: length 5, `Wait` present; elstools `-v jun-2017` identical; raw entry
  not decodable here). The user chose "append `1`" (2026-10-08): it is the only repaired file, = `Wait1` as `Motion_EL_Lobby` ends. Backups of March's files:
  `%TEMP%/c9g2_backup033`, `c9g2_backup036`. `PACK OK data033`; sizes equal the live kom except the 4 (the other 122 show `~` from recompression).
- `COPY OK` (user said yes): data033 md5 `b939e2ba...` (backup `data033.kom.20261008_042838`), data036 md5 `f709405e...` (backup `data036.kom.20261008_042837`).

**Hygiene** (`--worktree --files` the 8 code files): `files: 7 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,323,904 B, dated 2026-10-07 22:11:03, md5 fee6eb1fb848f26aaf249c2e0ef2f83f, equal to X2/US_SERVICE/x2.exe (2026-10-07 21:58:49); built at mods/june-buff-port 2931339 + 8 uncommitted code file(s).`
`ANIM_NAME_LOAD`, `AnimationNameTable.lua` (UTF-16) and `AddAnimationName` are in the exe.

**The user hears this.**
- With June's `ALD_LobbyWait` the table row plays the same three orb particles as March's kept orb hard-code (`X2UnitViewerUI.cpp:2090`, keyed `ALD_lobbyWait`);
  that hard-code now never fires. Not removed (user's earlier call).
- The SPK ready-action fire (lost in C9c-2b) comes back through the table. WAIT1/WAIT2 now loop (`X2Room.cpp:1704/1709`).
- One data file is repaired by a byte: `Motion_ARA_Lobby.x`.

**How to see it.** Play-test below, then "check logs": `python tools/logs.py --grep "JUNE_FX ANIM_NAME"` should show
`loaded=1 rows=384 inserted=230 skipped_duplicate=154 class_none=160`, 0 EXCEPTION, no missing-motion line for the 7 renamed names.
**Play-test 2026-10-08 (partial):** exe `fee6eb1f`, runs 04:55 and 04:58; ErrorLog line 157 `JUNE_FX ANIM_NAME_LOAD ... loaded=1 rows=384 inserted=230 skipped_duplicate=154 class_none=160`; State.log reached `XS_SERVER_SELECT` then `XS_VILLAGE_MAP`; 0 EXCEPTION, clean exit, no new crash bundle, no missing-motion line. Room ready/unready and the level-up result were **not played** (user waived them, 2026-10-08): they use the same `PlayByMotionType` path as character select (types 0-2, 4, 5), so the review checks them by reading code.

Review: `kickoffs/june-effect/C9g-2-review.md`.

### C9g-2 - Review (2026-10-08, Opus 5.5 high): one transcription fix (HABIT's table branch, inert in March); everything else confirmed

Ghidra was read-only. Fixes are under the same flag, `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE`; funcmap row `Effect C9g-2 review`.

**Fixed: HABIT (type 6) was missing `31fa5c90`'s table branch.** The C9g-2 note said every jump-table case of `31fa5c90` is customize-only. Case 6 is not.
- With no custom habit (`+0xf90 == 0`, always in March), it calls `GetAnimationName( class, 6 )` itself (`@31fa5fae`).
- A non-empty name plays `PlayAnim( name, ONE_WAIT, bTransition 0 )` (`@31fa5ff1`) and returns true, so `31fa7ef0` plays nothing more.
- An empty name falls through to `31fa7ef0`'s `L""` with the caller's bTransition.
- The port played the name with the caller's bTransition (true). Now `case UVUMT_HABIT` plays with `false` when the name is non-empty.
- It is unreachable today: no March caller passes HABIT, and no March class has a HABIT row. Its future callers are Stage 12 and checklist 7g.

**Confirmed against Ghidra:**

| What | Result |
|---|---|
| `31fa7ef0` stores | `+0xfa0`, `+0xe58`, `+0xfa4`, then `+0x1dc` (7 -> 2, 8 -> 1) |
| Unit / unit-manager test | after the pre-pass |
| Rose remap (`+0xaf0[0] == 0xb`) | WAIT -> WAIT1 only in state 1 (`UVS_CASHSHOP`); WAIT1/2 -> WAIT |
| Play types | 0-2 and 8 LOOP (0); 5, 7, 9 and default ONE_WAIT (2) |
| `PlayAnim`'s 4th argument | 2 (both duos), which is March's main model |
| Eve sound | `+0xe54` (`31d414d0( 0, 0 )`) |
| Literals | `Emotion_SitReady` / `SitWait` / `StandUp` |

- **`31fa5c90` head:** it resets `+0xe5c` unless state 10 and clears `+0xe60`. The group stops (cash shop 0..2, else 0..4, plus `+0xd82 = 1`) run before the `type > 9` exit, so WAIT_DELETE (10) still stops. Jump table `31fa6370`:
  - types 1/2/4/5 return false at once;
  - 0/3 stop only group 1 (already stopped by the head) and find nothing in the empty map;
  - 7/8/9 return false on an empty sit slot;
  - nothing else stops or plays.
- **Table:**
  - `3237a060` takes typed args (nil reads 0 / NULL) and converts with `MultiByteToWideChar( CP_ACP )`. The port uses UTF-8, which is equivalent: all 48,873 bytes of the source are ASCII.
  - `3238b0e0` frees the new node on an equal key, so the first row wins.
  - `32375ee0` keys (class, type), and its 11 defaults are verbatim (6 = `L""`).
  - Class: June reads `CX2Unit +0x38`, the port `GetClass()`. They are the same value, because every `m_pUnitTemplet` assignment uses `m_UnitData.m_UnitClass` (`X2Unit.cpp:55/215/333-342`).
- **Load order:** `@31ee6217` comes before `@31ee6224`. Play-test load line: `rows=384 inserted=230 skipped_duplicate=154 class_none=160`.
- **Frame move:** `31fa8ce0` (1,217 instructions) writes `+0x1d8` only, at `@31fa8d46`. The only other writer of `m_iJuneFxTriggerCode2` is PlayByMotionType, so bypassing the per-frame zero is right.
- **Square:** `323f2320` stores `GetAnimationName( class, 0 )` to `+0x64c`, guarded only by the unit manager. Each of the 51 cases in March's switch assigns only `m_wstrLobbyWaitMotionName` and nothing else. The only name the table changes is `ALD_lobbyWait` -> `ALD_LobbyWait`. Every March class is listed, so no class picks up the new default. `323f9680` and the sit loop are Stage 10/12.
- **Call sites:** `32511eb3` is `push 1` ... `push 5`. `324e7e27/63` push ebx = 1, then type 0. `32d7c962` pushes 0, then type = ebx. The mapping holds.

**Data.**
- The four `data033` files equal June's extracts (md5 EL_Lobby `e28a2874`, ARA_Common `8ff0af55`, EVE_Common `d12da42e`). ARA_Lobby is June's 1,466,551 bytes plus the one appended `1`.
- A strict length-prefixed name diff of March's originals (`%TEMP%/c9g2_backup033`) against them finds exactly the five renamed names March-only.
- Nothing else plays the old spellings:
  - **C++:** only the `#else` branches and the viewer's two `wcscmp` tests.
  - **4,851 compiled March Lua scripts:** none of the five strings appears.
  - **Two face `.XET`s:** they name `ALD_lobbyWait` (Ara) and `SSK_`/`SPK_LobbyReadyaction` (EL). XET lookup is `_wcsicmp` (`KTDXDeviceXET_Preprocessing.cpp:65`, `X2OPTIMIZE_TET_XET_PREPROCESSING` on), so the faces still match June's spellings.

**Stale comment, not changed:** `X2UnitViewerUI.cpp:2030` (C9c-2b's flag) still says the ALD row "never matches". With June's `ALD_LobbyWait` it now does, as the C9g-2 hand-off said.

**Hygiene** (`--range 0a2336d^..HEAD --with JUNE_FX_DEBUG`): `files: 8 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
The one deleted line in the flag's files is this flag's own Stage-12 comment, which was rewritten.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,339,776 B, dated 2026-10-08 06:12:03, md5 c49f362852f888c3efaf987cd62a9a4b, equal to X2/US_SERVICE/x2.exe (2026-10-08 05:45:54); built at mods/june-buff-port e9cfdca + 9 uncommitted code file(s).` The 05:26 attempt failed in C9h-2a's then-uncommitted files. Deploying was then held until june-item-kim K2's koms were live (its uncommitted code reads June's `item.kim`). `X2UnitViewerUI.obj` (05:44:57) is newer than the edit (05:17:12). The exe also carries K2's uncommitted code.

**Next owners.** C9h-2a / 2b / 3 have kickoffs. Stage 10 has its inline prompt in `JUNE_PORT_BACKLOG_ORDER.md`. Stage 12 now has `kickoffs/june-backlog/stage12-plan.md`.
**Play-test 2026-10-08 (regression) PASSED:** exe `c49f3628`, run 06:15-06:19. ErrorLog line 157: `JUNE_FX ANIM_NAME_LOAD ... loaded=1 rows=384 inserted=230 skipped_duplicate=154 class_none=160`. `UNIT_VIEWER_FX_PLAY EffectSet_ALD_LobbyWait` played through the table, so the Little Devil's lobby anim name is June's `ALD_LobbyWait`. States went through `XS_SERVER_SELECT`, `XS_VILLAGE_MAP` and `XS_BATTLE_FIELD`. No missing lobby-motion line, 0 EXCEPTION, clean exit, no new crash bundle. Out of scope, not this phase's: 3 `E_DISABLE_HB_CHECK_REQ` UNHANDLED and the `MOTION_*_EMOTION_*` open misses.

---

### C9h - Done (2026-10-07, Opus 5.5 high; no code, no flag, no build): the pet viewer sets are a viewer + engine slice; the reserved sets are the in-game riding pet's -> C9h-2a/2b/3

Read-only in Ghidra. 27 funcmap rows `Effect C9h`. These rows correct C9g in two places:
- `ResetRidingPet` does **not** stop group 1 itself;
- `ResetPet` takes **3** arguments (`RET 0xc`).

**1. The June diff, key by key.**
- **Unchanged against March:** the INIT_SYSTEM and INIT_MOTION keys and their defaults (outline 1.2 = `CARTOON_OUTLINE_WIDTH`), the level clamp, and `OpenXSkinMesh(..., L"Bip01")`. March's `m_pChangeTexXET` shadowing is March's own and June has it too.
- **ResetPet `32e50de0`:**
  - A third `char` argument, the pet **variant**. When `iLv == 3`, the variant is non-zero and the templet has 4 steps, the motion file becomes `...N.x` and the XET becomes `...N.xet`. A hard-coded suffix from `326cc990` takes over the XET in two cases: pet 39 uses `H.xet`, and pet 134 with variant 1-2 uses `2.xet`.
  - After ResetRenderParam it stops group 1, then plays `INIT_EFFECT.UNIT_VIEWER_EFFECT_SET_LIST` into group 1.
- **ResetRidingPet `32e51ea0`:**
  - `RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST` plays into group 1.
  - `RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET` plays on the **UI effect set** through the new overload `321e28d0 (name, skin)`. That overload sets the June instance flag `+0xdd`, and the handles go to a new viewer vector at `+0x2a8`.
- **The `+0xdd` flag:**
  - It moves a set to screen space, read from the viewer bone, in GetEffectPosition `@321dd0e0`.
  - A particle created on such a set gets OverUI, `PT_2D_PLANE` and ResolutionConvert (`@321f2522`).
- **The viewer's new members:** IsAnimExist `32e4fe30`, a handle stop `32e4fde0`, and a SetScale `32e4fcf0` that also scales the handles. Ctor and dtor change to match.

**2. The reserved UI effect sets belong to the in-game `CX2RidingPet`, not the viewer.**
- `32d2b0d0` (INIT_EFFECT) loads the 4-tuples into a map at `+0x4d0`.
- June's riding-pet StateChange `32d2f560` does three things:
  - it stops every playing entry whose delete condition is `RESDC_STATE_CHANGE` (1), then the state's `DELETE_RESERVED_UI_EFFECT_SET` indices;
  - it plays a per-state UI-set list into `+0x4b0` or `+0x4c0`;
  - it calls `32d2bad0`, which plays `PLAY_RESERVED_UI_EFFECT_SET`. With `RESPC_NOT_PLAY` (1), an entry that is already playing is not replayed.
- The enums are `RESERVED_EFFECT_SET_PLAY_COND {NONE 0, NOT_PLAY 1}` and `RESERVED_UI_EFFECT_SET_DELETE_COND {NONE 0, STATE_CHANGE 1}`. **March's `Enum.lua` has neither.**
- The same StateChange carries 24 more June-only keys (funcmap `32d2f560`). That makes it a riding-pet engine rewrite -> **C9h-3**.

**3. `32228c90` = March `CX2CashShop::ResetPetViewer`, and its only caller `32234b10` = `PetPreview`.** June rewrote both:
- a `type != -1` guard;
- the clear and the group-1 stop come first;
- the 1.4 scale for pets 35-36;
- `Wait_Transform`;
- the position is the `+0x51c` vector plus June offsets, with a second offset pair under Stage 12's preview flag `3379fce4`;
- PetPreview flips a coin between a pet and a riding pet, then runs a pre-equip-list hide.

March's three no-argument hides became inline blocks in June: SetOpen `3223d680`, UpdateDLGPetViewerUI `3223c5d0`, and `322383c0` (whose March counterpart is unconfirmed). Pet-info clears are at `32902000`, `32902700` and `328fce40`. -> **C9h-2b**.

**4. Reach.**
- **March data:** 0. None of the 8 June scripts that carry these keys is in March (Santagirl, Jet Ski, Lord of Inferno and `_2P`, Raul Puppet, Cat Maru, Dog Kkogi, Pporu Rickshaw), and March's enums lack all 8 ids.
- **June preview items for those pets:** 16, all in June's `item.kim` (1 in March's). After Stage 13 they preview an id that March cannot resolve; `ResetPet` / `ResetRidingPet` return false, as in June.
- **March pets that do reach:** 39 and 134 (the variant branch, which waits on the variant byte); 4 preview items, 4 in June's `item.kim` and 2 in March's.
- **Pets 35-36** (the 1.4 scale) have no preview row in June's `PetPreView.lua`.

**Routing.**
- **C9h-2a** (Sonnet 5.5 high, flag `SERV_IRUHADEV_JUNE_PET_VIEWER_FX`): the viewer, the `321e28d0` overload, `+0xdd` and its three consumers, and `326cc990`.
- **C9h-2b** (Opus 5.5 high, flag `SERV_IRUHADEV_JUNE_PET_PREVIEW`): `ResetPetViewer` + `PetPreview`, the hide sites, and the pet-info clears.
- **C9h-3** (Opus 5.5 high, identify): the riding-pet engine rewrite.
- **Backlog Stage 17** (new): the pet variant byte (a June `KPetInfo` field at `+0xca`, the in-game pet `326e5cf0`, and the save) and the 8 June-only pets and riding pets.
- **JUNE_BUGS 104-106.**

### C9h-2a - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_PET_VIEWER_FX`; inert, regression play-test PASSED 2026-10-08)

**Landed** (MODS.md "June effect completion C9h-2a"). Whole port of the viewer and engine slice; nothing in March data reaches it, so nothing visible changes except the base viewer's player now also running for pet / riding-pet previews.
- **`CX2NPCUnitViewerUI`**: handle vector (June `+0x2a8`; the typedef is repeated in the header because `X2EffectSet.h` is included after it), dtor frees it without stopping (JUNE_BUGS 105);
  the three base hooks (`OnFrameMove` `329585f0` after the skin block, `Prepare` `32957260` on the shown path, hint dropped, `Draw` `32957330`); `IsAnimExist`; `JuneStopUIEffectSets` (= `32e4fde0`); `SetScale` that also scales live handles;
  `ResetPet( id, lv, cVariant = 0 )` with the variant branch, the group-1 stop and `UNIT_VIEWER_EFFECT_SET_LIST`; `ResetRidingPet` (viewer list into group 1, no stop; by-UI list through the new overload). Load lines `JUNE_FX PET_VIEWER reset=<pet|riding> id= group1= uiset=` and `... variant ...` go to ErrorLog.txt, only when something played.
- **Engine**: `CX2EffectSet::PlayEffectSet( const WCHAR*, CKTDGXSkinAnimPtr )` (`321e28d0`), instance flag `m_bPlayedOnUnitViewer` (`+0xdd`), consumers: `GetEffectPosition` (ESM_FIELD, no square unit), `GetEffectPositionByBoneName`, particle creation (OverUI, `PT_2D_PLANE`, resolution convert).
  `CX2PetManager::JuneGetVariantXetSuffix` (`326cc990`); `CKTDGXSkinAnim::JuneIsAnimSetExist` (`KTDGXSkinAnim.h`, outside the kickoff's touch list).

**Corrections to C9h (kickoff, funcmap, JUNE_BUGS fixed in this commit).**
1. `OnFrameRender_Draw` `32e4f8f0` does call the base draw: it reloads ECX = this and JMPs to `32957330` (`@32e4f939`). The kickoff said "no base Draw".
2. `32e4fde0` stops a stack copy of each handle (`MOV ECX,[ESI]` -> `[ESP+0xc]`): the vector entries are **not** set to -1 and stay stale. JUNE_BUGS 104 and funcmap row `32e4fde0` reworded.
3. `321dafb0` is June's deferred `StopEffectSet( Handle&, bool )` queue push (`32116c20` = `vector<pair<int,bool>>::push_back` on `this+0x30`, drained by `321e3ee0`). Ported as March's immediate `StopEffectSet`, as C9c-1, C9d and C9f already did.
4. `31d34110` / `31d39100` = a find-based by-name anim lookup (NULL on a miss, no insert). March's `GetAnimSet( name )` is `operator[]` and inserts, so `JuneIsAnimSetExist` is find-based.
5. The `+0xdd` branch in `GetEffectPosition` still runs the `+512`, `(y-384)*-1`, `z = 0` transform when both bone names are empty (vPos is (0,0,0) there); no `SetEffectSetOffset` on either path. The `GetEffectPositionByBoneName` consumer is reachable (non-empty `<base>_GROUPCHILD_<n>` names) and ported; only its empty-name riding-pet fallback stays out (comment extended).
   Doubles read from `unpacked_x2.exe`: `3347e350` 512.0, `3347e348` 384.0, `33436c90` -1.0.

**Evidence.** `+0x328` is `m_pPetManager`: June's own `ResetPet` calls `GetPetTemplet` (`326cbcf0`) through `[g_pData+0x328]` (`@32e50e35-32e50e44`); March's offset is `0x298` (probe), which is only March's layout. `+0x6c` = `m_Evolution_Step_ScriptName` and `+0x7c` = `m_Evolution_Step_InitLuaTemplet` (March probe `0x6c` / `0x7c`). SetScale members: a vec3 + bool at June `+0x70/+0x7c`, March `0x64/0x70` (`m_vScale`, `m_bDamageEffectScale`).
`PlayEffectSet` hands the handle back through a hidden out pointer (`KHandleType` is not POD); the three args are out, name, skin.

**Hygiene** (`--worktree --files` the 8 code files): `files: 5 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` (the OTHER row is `Always.h`: another session's K2 flags sit beside mine);
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 26 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` (the 26 are ResetPet's group-1 block, inserted into an LF stretch of a mixed-EOL file); `RESULT: PASS`.
**Build:** `BUILD OK: X2/US_SERVICE/x2.exe 19,339,776 B, 2026-10-08 05:45:54`. `JUNE_FX PET_VIEWER reset=pet`, `reset=riding` and `variant` are in the exe (UTF-16).
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,339,776 B, deployed 2026-10-08 06:12:03, md5 c49f362852f888c3efaf987cd62a9a4b, equal to X2/US_SERVICE/x2.exe (05:45:54), HEAD e9cfdca.` It went out with june-item-kim K2's deploy once its koms were live (it also contains K2's uncommitted item.kim code).
**A tooling trap hit once:** a bytepatch spec with a repeated `@@ file` section for the same file drops the earlier sections' edits silently. The first apply lost most of this phase's edits; flagcheck still said PASS (it checks what is there) and the compile caught it. One `@@ file` per file.

**Reach (named for the play-test).** March's `PetPreView.lua` (data036) pet previews: 500010 (Ancient Pporu), 500150 (Mystic), 500020 (Tree Knight), 90002050 / 90002070 (Sheriff, pet 134, the variant branch's pet); riding-pet previews 550000 (Mobi), 550010 (Koucika), 550020 (Ancient Pporu riding), 550030 (Scorpion R), 269731. Pet 39 has no March preview row.

**How to see it.** Regression only: open the cash shop, preview one pet and one riding pet; open pet info, view the pet and the riding pet. They look as before; ErrorLog.txt has no `JUNE_FX PET_VIEWER` line (nothing in March plays a viewer list); 0 EXCEPTION.

**Play-test 2026-10-08:** exe `c49f3628`, run 06:15-06:19; 0 EXCEPTION, no `JUNE_FX PET_VIEWER` line, cash shop opened 3 times, pet lists loaded; the user confirmed previews and pet-info looked as before.

Review: `kickoffs/june-effect/C9h-2a-review.md`.

### C9h-2a - Review (2026-10-08, Opus 5.5 high): no finding, no code change

**Play-test read first:** C9h-2a's regression run (exe `c49f3628`, 0 EXCEPTION, no `JUNE_FX PET_VIEWER` line) was read and recorded in the Done note above.

**Checked against June (Ghidra read-only, plus literals read from `unpacked_x2.exe`).**
- **The variant branch, `32e50de0 @32e517f1-32e51a6f`.** Stack offsets are normalised to the ESP at `@32e517f1`.
  - Locals: `+0x17` is the clamped iLv (written `@32e50e40/71`), `+0x118` is cVariant, `+0x40` is the petId copy (`@32e50e31`), `+0x70` is the motion string, `+0x8c` is the XET string (OpenXET's argument `@32e51aa9`), and `+0x54`, `+0xa8` and `+0xc4` are scratch strings.
  - Tests: the templet's `+0x6c` size uses stride 0x1c and is compared as `AL == 4`. The guards are `len > 3` and `len > 5`. `substr(0, len-3)` and `substr(0, len-5)` are each moved into a scratch string with `31aa92b0` (= `operator=(&&)`), so the motion scratch cannot leak into the XET.
  - Literals: `334b58ec/f4/fc` are `1.x`/`2.x`/`3.x`, and `334b58d4`/`334b5198`/`334b58e0` are `1.xet`/`2.xet`/`3.xet`. `31aa9200` is assign, not append.
  - The suffix is used when `31c1c240(L"") != 0`, i.e. when it is non-empty. The suffix call needs `g_pData` and `+0x328`.
  - Everything matches `X2NPCUnitViewerUI.cpp:184-223`.
- **`326cc990`**: pet 0x27 gives `H.xet` (`334b518c`). Pet 0x86 with `(byte)(v-1) <= 1` gives `2.xet`. Anything else gives `L""`. Matches.
- **ResetRidingPet, `32e51ea0 @32e52ad1-32e52ce0`.** The whole function has no `329594c0` call. `BY_UIEFFECTSET` (`33525eac`) is collected first, then `RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST` (`33531ab0`) plays into group 1. After INIT_EFFECT closes, each name goes through `321e28d0`, gated on `g_pData`, and a handle other than -1 is pushed to `+0x2a8`. Matches.
- **`321e28d0`.** A null templet or a null skin gives -1. `321e1bf0` is called with (templet, skin, 0, 0, null square ptr, 0 pet). `+0xdd = 1` is stored on the returned instance (`@321e29de`). Matches `PlayEffectSet(name, skin)`.
- **Reach of the consumers (not checked by C9h-2a).** A no-owner instance gets mode 2 in June: the ctor `321df230` takes the `@321df570` else. March does the same: `X2EffectSet.cpp:4651` gives `ESM_FIELD` (= 2). June's switch `@321dce8f` sends 2 to `321dd013`, the case that holds both viewer branches. So both position consumers run.
- **The `+0xdd` consumers.**
  - **`@321dd0e0`**: BONE_NAME `+0x40`, then `+0x9c`. Both empty keeps vPos (0,0,0). `31d38a20` gives the frame translation, or 0 without a frame. Then `+512.0`, `(y-384.0)` is rounded to float and multiplied by `-1.0`, and z = 0. The jump to `321dd531` is the plain return (no `321d9820` offset).
  - **`@321dd926`**: same transform. Found/not-found goes to `bBoneFound`, the miss zeroes x and y, and the result reaches `321ddd58`, which is the return without an offset.
  - **`@321f2522`**: EDI is the new sequence (`[EDI+0x430]` is its handle `@321f246a`). `+0x170 = 1`, `+0x1c0 = 1` (`PT_2D_PLANE` = 1 in March's enum), `+0x2f4 = 1`.
  - All three match.
- **Frame hooks.**
  - `32e4f600` jumps over the skin block when there is no skin (`@32e4f60a`, to `@32e4f748`) and always ends in `329585f0`.
  - `32e4f820` calls `32957260` only on the shown path, and returns the skin's hint.
  - `32e4f8f0 @32e4f939` reloads ECX = this and JMPs to `32957330`, which confirms C9h-2a's correction 1.
- **Handle stop / SetScale / IsAnimExist.**
  - `32e4fde0` copies `[ESI]` to `[ESP+0xc]` and stops that copy, never erasing it (correction 2 confirmed).
  - `32e4fcf0` writes `+0x70..+0x78` and `+0x7c = 0`. The ctor `321df230 @321df32e-33f` has the order `+0x6c` / `+0x70` / `+0x7c`, which is March's `m_fPowerRateScale`, `m_vScale`, `m_bDamageEffectScale` (`X2EffectSet.h:715-719`).
  - `31d39100` returns 0 on a miss and the map value on a hit, so a found NULL entry also returns false. `JuneIsAnimSetExist` does the same.
  - **One deliberate deviation:** `IsAnimExist` adds a `NULL == pName` guard. June would build a wstring from the pointer. The only caller passes a literal, so the guard is kept.
- **`+0xe0` (`@321dd1a3`, `@321dd9dc`)** is not ported, which is right; it belongs to C9h-3.

**Hygiene** (`flagcheck JUNE_PET_VIEWER_FX --range e9cfdca^..HEAD`):
`files: 5 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 9 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 26 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`.
- **The 2 bare `#endif`s** (`tok! 2` on the DEFINE row) close the two `#if defined(...) #error` dependency guards under the `#define` in `Always.h`. They are not flag blocks.
- **All 9 OTHER rows are `f1549a6`**, K2's `JUNE_ITEM_KIM` / `KIM_LAYOUT_CHECK` commit inside the range. Each has +0 lines for this flag.
- **The 26 EOL lines** are ResetPet's group-1 block. It sits after `ResetRenderParam();`, which is an LF line in a mixed file (419 CRLF / 218 LF). The 0 deleted lines show that no existing line ending changed.

**Deploy:** none. Nothing changed, and the tree holds a peer's uncommitted C9h-2b edits (`Always.h`, `X2CashShop.cpp`, `X2UIPetInfo.cpp`). The deployed exe is still `c49f3628`, which the regression play-test passed on. No new play-test.

Next: C9h-2b (Opus 5.5 high) and C9h-3 (Opus 5.5 high); both kickoffs exist.

### C9h-2b - Done (2026-10-08, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_PET_PREVIEW`; play-test PASSED 2026-10-08)

15 funcmap rows `Effect C9h-2b`. The code is in `X2CashShop.cpp` and `X2UIPetInfo.cpp`, and the flag is in `Always.h`. The flag `#error`s without `SERV_IRUHADEV_JUNE_PET_VIEWER_FX`.

**What landed.**
- **`ResetPetViewer` = June `32228c90`:**
  - it does nothing for type -1, and the UI-set and group-1 stops come first;
  - pets go through `ResetPet( id, 3, 0 )`; the scale is 0.5 for a riding pet, 1.4 for pets 35-36, else 1.0. `PUI_STINKO_HATS_ON` / `PUI_EBONY_HATS_ON` = 35 / 36 by a `cl` `static_assert` probe, with a negative control. `3344f240` is the float 1.4;
  - `Wait_Transform` falls back to `Wait` (XAP_LOOP = 0, transition true);
  - the position is **June's `(260,-30,0)`**.
- **`PetPreview` = June `32234b10`:**
  - both lookups run and are tested with `< 1`;
  - `rand()` runs only when both hit, and an odd result picks the riding pet;
  - the trailing hide fires when the previewed item has left the pre-equip list.
- **The three no-argument `ResetPetViewer()` calls** became June's inline hides (stops, hide, no previewed item): SetOpen `@3223e56d`, UpdateDLGPetViewerUI `@3223c763`, and take-off-all `322383c0 @322385b0`. Under June's guard a call left behind would hide nothing.
- **Pet info:** Select_Tab `32902000 @32902243` and SetShow's close branch `32902700 @32903116` stop both viewers' UI sets and group 1, the riding viewer first. `:1183` gets a comment: the variant byte is Stage 17, and the variant stays 0.

**Decisions and findings.**
- **Position (user, 2026-10-08): June verbatim.**
  - `+0x51c` is written only as zero (the ctor `@32253ef3`, SetOpen `@3223e1a3`), so June's preview stands at `(260,-30,0)`.
  - June re-laid its dialog: SetOpen puts the character at `+0x51c + (315,-5030,200)`. March's `ResetUnitViewer` keeps it at `(-355,-5001,200)`, with the pet at `(-270,0,0)`.
  - Asked as "copying June breaks March"; the user chose June's offset. In March's dialog the preview therefore appears about 615 units right of the character, not beside it.
- **`32234380` = June's `CheckPreEqipItem`:** June's InsertPreEquipItem `3224cb20` calls it where March's does. June's two lists (map `+0x2c8`, key 0/1) differ only for June's unit type 10 (`+0x528`, SetOpen `@3223d9d2`), so it is ported as March's `CheckPreEqipItem` plus June's templet-null `false`.
- **Corrections to C9h:**
  - `3224d0f0` is June's **Scaleform** cash-shop handler (`uMsg 0x467` from the GFx callbacks `32e3ebe0` / `32e3ec10` / `32e425f0`), not a customize preview and not March's `UICustomEventProc`.
  - `322383c0` is its take-off-all (messages 15 / 19); its hide is the one March's `CSCUM_TAKE_OFF_ALL_ITEM` gets.
  - `DAT_3379fce4` is the Scaleform customize-preview layout flag (Stage 12) and stays false.
- **Routed:**
  - `328fce40 @328fd0d8`: pet info's riding view, under `IsEnableCode( 2155 )` plus byte `+0xd1` of the June-only singleton `324943c0`. This session first named that singleton `CX2RidingPetManager`; C9h-3, committed meanwhile, corrected the name and routed the block to backlog **Stage 17**.
  - June's preview lookup `32808770` (CTime windows, a code event, two ids; `g_pCashItemManager:AddPetPreViewData`) -> new **C9h-2c** (identify).

**Hygiene** (`--worktree --files` Always.h, X2CashShop.cpp, X2UIPetInfo.cpp): `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. After committing, `--range 9687d8c^..HEAD` gives the same three lines (2 PASS, 1 DEFINE, RESULT: PASS).
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,342,848 B, dated 2026-10-08 07:18:11, md5 1a00382efe082ae2816ae801f0f2f07d, equal to X2/US_SERVICE/x2.exe (2026-10-08 07:17:55); built at mods/june-buff-port 464a972 + 3 uncommitted code file(s).` The 3 files are this phase's. `JUNE_FX PET_PREVIEW item=%d`, `... hide item=` and `Wait_Transform` are in the exe (UTF-16).

**How to see it** (cash shop, then pet info):
- **A pet item** (500010 Ancient Pporu, 500020 Tree Knight, 500150 Mystic): the pet shows toward the right side of the shop window, away from the character (June's spot), playing `Wait`. Click it again and it hides.
- **A riding-pet item** (550000 Mobi, 550010 Koucika): the riding pet shows there at half scale.
- **A non-pet item:** nothing changes, and a pet that is still previewed stays.
- **Take off all, or close and reopen the shop:** the preview hides.
- **Pet info, switching between the pet and riding-pet tabs:** the viewers show as before.
- **ErrorLog.txt:** one `JUNE_FX PET_PREVIEW item=<n> pet=<n> riding=<n> pick=<pet|riding|none>` line per equip click; a `hide item=` line when a previewed item leaves the pre-equip list.

**Play-test 2026-10-08: PASSED** (user, all checks). Exe `1a00382e`, run 07:23-07:25, 0 EXCEPTION; the 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are unrelated to this phase. 15 `JUNE_FX PET_PREVIEW` lines:
- `pick=riding`: 550000, 550010, 550030;
- `pick=pet`: 90002100 (pet 137), 84001272 (96), 500600 / 500610 / 500620 (28-30);
- `pick=none`: non-pet items;
- one `hide item=550010`: the next riding item (550021) took its pre-equip slot, so June's trailing hide fired.

Review: `kickoffs/june-effect/C9h-2b-review.md`.

### C9h-2b - Review (2026-10-08, Opus 5.5 high): no finding, no code change

**Play-test read first:** the PASSED run above (exe `1a00382e`, 0 EXCEPTION, 15 `JUNE_FX PET_PREVIEW` lines).

**Checked against June (Ghidra read-only, literals read from `unpacked_x2.exe`).**
- **ResetPetViewer `32228c90`.**
  - The guard is `+0xec` non-null and `type != -1`, and the -1 path does nothing at all. Then come `32e4fde0` and `329594c0(1)`, before the item test (`JS` on the id, then `+0xf0 == item`).
  - The type test is `DEC EAX; JZ` (1 = `PPT_RIDING_PET`, `X2ItemManager.h:15`). Pets call `32e50de0( id, 3, 0 )`, and `0x23..0x24` load `3344f240` = 1.4f. The riding scale `3343777c` is 0.5f, and other pets get `FLD1`.
  - The order is SetScale `32e4fcf0`, the inline show, `IsAnimExist( 33482ab4 "Wait_Transform" )`, PlayAnim( name or `33474724 "Wait"`, 0, 1 ), then `+0x170 = 1`, then SetPosition. The else branch is the inline hide with `+0xf0 = -1`.
  - `33733604/08/0c` = (260,-30,0) and `33733610/14/18` = (-230,-200,0), read-only (2 readers each: `32228c90` and `3224d0f0`).
  - **`+0x51c`, scanned across the whole `.text`:** the cash shop's only writers are its ctor `322533c0 @32253ef3` and SetOpen `@3223e1a3`, and both store zero.
    - Two non-cash-shop ctors (`31cbb9d0`, `31e46300`) also store zero there.
    - The 9 `LEA [reg+0x51c]` sites are other classes, where `+0x51c` is an object, not a float vector. One is `32306600`, a base ctor that stores vtable `33441d84` there. The others read a container head from `[+0x51c+4]`.
    - None of them is reached from the cash shop's ctor (vtable `334843b4`, base `32e1c600`).
    - **So the preview stands at June's (260,-30,0), as decided.**
- **PetPreview `32234b10`.**
  - It returns when `g_pData+0x34` is null. Both lookups run, and both are tested with `JLE` (`< 1`).
  - `rand()` (`@32234b4e`) runs only when both hit, and a non-zero `% 2` picks riding. A miss on both gives ( -1, item, -1 ).
  - The trailing hide checks `+0xf0 != -1` and `!32234380`, then the viewer. It runs SetShowObject(false), `32e4fde0`, group 1, then `+0xf0 = -1`. Matches.
- **`32234380` = March `CheckPreEqipItem` + the templet-null test: confirmed, with a sharper reason.**
  - For every unit other than type 10, June keys the list by `322551c0(templet)`: bit `+8>>2`, with the `+0x528` override.
  - June's InsertPreEquipItem `3224cb20` inserts with that same key, both the forced and the toggled path. So "in its own list" asks the same thing as March's single-list find. `SAVE_CASH_SHOP_PRE_EQUIP_ITEM_LIST` is off in US (`InHouse5.h:788`, commented).
  - `+0x528` is non-zero only for type 10. SetOpen stores 0 `@3223d9df` on every other unit. A second writer, `3224c250`, toggles it only under type 10; it is called from the Scaleform handler `@3224eef1` and from `@3224c852`.
  - That corrects the Done note's "the two lists differ only for type 10": they differ by item for every unit, and the conclusion stands. Two funcmap rows were added.
- **The three hides** (SetOpen `@3223e56d`, UpdateDLGPetViewerUI `@3223c763`, take-off-all `@322385b0`) all run `32e4fde0`, group 1, hide, `+0xf0 = -1`.
  - Take-off-all pushes `EBP`, which is set to 1 `@3223854b` and is callee-saved up to `@322385c5`.
  - `grep ResetPetViewer(` finds no no-argument call left.
  - **Correction to the Done note:** a call left behind would *not* "hide nothing". March's header default type is `PPT_PET` (0), so it passes the guard, runs the stops and hides. That is harmless, because none is left.
- **Pet info.** Select_Tab `@32902236-3290226f` and SetShow `@32903116-32903148` both stop `+0xb8` before `+0x3c`.
  - The members were identified by June's own use rather than a March sizeof walk: `+0x3c` receives `ResetPet` (`328f0aa0 @328f0be3`) and `+0xb8` receives `ResetRidingPet` (`328f74e0 @328f75f2`). So they are the pet viewer and the riding viewer.

**Hygiene** (`flagcheck JUNE_PET_PREVIEW --range 9687d8c^..HEAD`):
`files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`.
The DEFINE row's +23 lines are the comment block, the `#define`, and the `#if defined(...) / #error / #endif` dependency guard on `JUNE_PET_VIEWER_FX`.

**Deploy:** none, since nothing changed. The deployed exe is still `1a00382e`, which passed the play-test. No new play-test is needed.

Next: C9h-2c (Opus 5.5 high, identify; its kickoff exists).

---

### C9h-2c - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): June's preview table is March's plus dated / event rows nobody uses; the port is its loader -> C9h-2d

19 funcmap rows `Effect C9h-2c` (one upgraded: `32808770`).

**Headline.** June's lookup equals March's on June's own data.
- All 393 rows of June's `PetPreView.lua` pass 2-3 arguments (283 pet with the default type, 10 explicit `PPT_PET`, 100
  `PPT_RIDING_PET`). No row uses the second id, the dates or the code event.
- With those fields empty, every matching row pushes its pet, and the result is `vec[ rand() % size ]`, or `-1` on a miss.
  That is March's `GetPreviewIDByItemID` (`X2ItemManager.cpp:4304`). `rand()` survives.

**Map.**
- **Object.** The item manager (`g_pData+0x34`, ctor `3281f490`). `g_pCashItemManager` is March's own Lua name for it
  (`X2ItemManager.cpp:320`), and June loads `PetPreView.lua` from `ResetItemManager` `31edda90`, as March does.
  - Row: `32822b60`, 0x4c bytes: item, pet, type, second id, start/end `wstring`, code event.
  - Vector: `+0x1cc`.
- **Loader `32812ce0`.** Bound as `AddPetPreViewData`; June builds the name XOR'd, and C9h-2c decoded it from
  `31db13af..` plus the keys `@31db143a..`.
  - Arguments: (item, pet, type, second id, a5, a6).
  - Both strings given: they are the dates. Only a5 given: code event = `atoi(a5)`.
  - It skips `item <= 0 || pet <= 0` silently: no popup, no latch, no templet check.
- **Lookup `32808770`.** Each date or event row either adds its pet, or, once its window or event is on, swaps in the
  second id. The pet is removed first, the swapped id is added once, and an out-of-window row falls back to the pet only if
  neither id is already there. The branch table is in the funcmap row.
  - "Now" is `CX2Data::GetServerCurrentTime` `31edca50` (= March `X2Data.cpp:2614`).
  - The dates parse through `31c55270`: `ConvertStringToCTime`, then a shift of (`CX2Data+0x470` - `_timezone`).
    - `+0x470` is June-only. Its one writer is `31edce90`, fed by the 3rd `__int64` of June's wider `EGS_CURRENT_TIME_ACK`
      (handler `32571b80`, Get `320a2930`). June's ctor `31eef470` never writes it.
    - Offline it has no source, so it needs a stand-in: an open question for the user, below.
  - The event test `32822650` is a June-only `std::set<int>` at `g_pInstanceData+0xa94`, not March's `IsEnableCode` map
    `+0xa18`. June's `EGS_UPDATE_CODE_EVENT_NOT` handler `31f00970` fills it, together with `+0xa18` and a second set `+0xaa4`.
    - Get `320de0f0`: the map, then the two sets.
    - March's `ClientPacket_Global.h` is reached before `Always.h`, so the port's flag needs the `ServerDefine.h` second home.
    - The emulator never sends that packet, so the set stays empty.

**Data.**
- 152 of June's 393 rows name a pet March's `Enum.lua` lacks: 85 pet rows (44 names) and 67 riding rows (36 names). The
  names are listed by the C9h-2c census; they are backlog Stage 17's.
- 241 rows resolve to the same values in both enums.
- All 332 distinct items are in the live `item.kim` (June's, since K2).
- June's file is a superset of March's preview items.
- `datai015.kom`'s `PetPreView_1.lua` is a different entry name. March opens only `PetPreView.lua` (`X2Data.cpp:1784`), so
  data036's copy is the one that loads.
- **Hazard for adopting it on March's loader:** under `_IN_HOUSE_` (on in US_SERVICE), the first row that resolves to 0
  pops a box and sets `static bHasInvalidData`, and every later row is then rejected. June's loader has no such block, which
  is why the port takes June's loader.

**Routed.**
- Self-contained on March's item manager -> new port phase **C9h-2d** (Sonnet 5.5 high, flag
  `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`): loader, lookup, the two externals (set `+0xa94` with June's handler and packet
  sets; the `+0x470` time-zone member), June's `PetPreView.lua`. Kickoff `kickoffs/june-effect/C9h-2d.md`.
- The same row type also backs June's **ice-heater preview** rewrite: loader `3281a0d0` (`AddIceHeaterItemData`, map
  `+0x1b0`) and lookup `328136f0` (set `+0xaa4`). That is not the pet preview, and it has no owner, so it goes to backlog
  **Stage 16** as one line. All 9,545 of June's IceHeaterPreView rows pass 4 arguments.

**Open question (user):** the stand-in for June's server UTC offset (`CX2Data+0x470`).
- Proposed: `_timezone`. The shift is then 0, so dates parse as in March.
- The alternative is 0, which shifts every date by `-_timezone`.
- Either way it is inert on June's data.

and that answer. Next: `/x2-phase june-effect C9h-2d` on Sonnet 5.5 high.

---

### C9h-2d - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`; play-test PASSED 2026-10-08)

June's pet-preview table, whole. 7 funcmap rows `Effect C9h-2d`; JUNE_BUGS #112.

**What landed** (flag also `#define`d at the end of `ServerDefine.h`, in `SECOND_HOMES`):
- **Row**: `PREVIEW_PET_DATA` = June's 0x4c row (second id, start / end `wstring`, code event).
- **Loader `32812ce0`**: 6 arguments, no `_IN_HOUSE_` popup / latch. Each skipped row logs `JUNE_FX PET_PREVIEW_LOOKUP skip item= pet=`.
  One `JUNE_FX PET_PREVIEW_LOOKUP loaded= skipped=` line per load (X2Data.cpp, after `PetPreView.lua`).
- **Lookup `32808770`**: re-decompiled, not taken from the funcmap row. A parse failure on the second-id-less path pushes nothing; both bounds are strict; the swap erases the pet only when the second id is absent; `rand() % size`, `-1` on a miss.
- **Externals**: `CX2InstanceData` sets `+0xa94` / `+0xaa4`; `KEGS_UPDATE_CODE_EVENT_NOT` Put/Get = map, set, set; the handler copies all three. `CX2Data::GetServerUtcOffset()` (int member, sign-extended) and the shifted parse (`31c55270`: `time += offset - _timezone`).
- **Handler**: June has none of March's NEW_YEAR_2014 / CHECK_POWER blocks, so the flag-on path drops them (inert offline: nothing sends the packet).
  June's tail, `32344880()` plus the state virtual at `+0x100`, is **not** ported: it is an unidentified code-event list refresh -> **C9h-2e** (identify; kickoff written, STATUS row added).
- **Data**: June's `PetPreView.src.lua` copied verbatim, compiled, `data036` packed (`PACK OK`; `kom.py status`: only `PetPreView.lua` changed, nothing foreign) and copied (`COPY OK`, md5 `10fba833`, backup `kom_backup_tools/data036.kom.20261008_083402`).

**`+0x470` stand-in** (user: "follow June's"): June's ctor never writes it, so offline it is never set. The port initialises it to **0**
(the kickoff's second option): every parsed date shifts by `-_timezone`, not March's unshifted parse. Inert today (no row has a date).

**Hygiene**: `flagcheck --worktree --files <10 code files>`:
- `files: 8 PASS, 0 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 5 #else/#endif without the flag token, 0 encoding change(s)` (the 5 are bare `#else`)
- `RESULT: PASS`

**Deploy**: `DEPLOY OK: X2_offline.exe is 19,348,992 B, dated 2026-10-08 08:20:24, md5 affec3722e7bead5d6f914dbc7a60989, equal to X2/US_SERVICE/x2.exe (2026-10-08 08:20:19); built at mods/june-buff-port 4567c75 + 16 uncommitted code file(s).`
The exe also holds a peer's uncommitted riding-pet work (X2RidingPet*, X2GUUser*); the new strings are in it (UTF-16 search).

**Reach census** (step 6; `scratchpad` script, March templets = names present in `PetTemplet.lua` / `RidingPetTemplet.lua`):
- 393 rows: 152 unresolved on March's enums (85 pet + 67 riding) -> skipped, Stage 17's. 241 resolve: 199 have a March templet (174 pet, 25 riding), **42 have none** (34 pet, 8 riding: Oberon custom, Grim Reaper, Hatchling `_F`, Mystic `_F`/`BLUE`, ...).
- Against March's old 115-row file: 136 of the 241 are new, 94 of them with a templet. 10 March rows are gone from June's file (269731's three riding picks, 500060 / 500070 / 500110 and 84001272 / 84001273 move to the `_F` variants): June wins.
- The 42 no-templet rows are survivable: `ResetPet` and `ResetRidingPet` return false on a missing templet (X2NPCUnitViewerUI.cpp:89, :502), so the viewer shows nothing; no stop condition.
- June-new items that are also **in the offline cash shop**: 550040 / 550041 / 550042 (Ice / Dark / Red Hameling riding pets, products 346-348, category 63), all with a March templet.

**How to see it**: cash shop, riding-pet category, preview a Hameling (below). `ErrorLog.txt` after startup: per load 152 `skip` lines and `loaded=241 skipped=152`.

**Play-test (2026-10-08, logs read)**: 152 `skip` lines (all `pet=0`) and one `loaded=241 skipped=152` line (the session loaded the item manager once), 0 EXCEPTION, no popup.
`PetPreView` picks: 550042/41/40 -> riding 7/6/5 (Red / Dark / Ice Hameling, new), 84001272/73 -> pets 98/99 (`PUI_PANDA_STICK_RED_F` / `BLUE_F`, June's change), 500610 / 90002060 / 90002100 -> pets 29 / 136 / 137 as before, 550030 -> riding 4.

**Next**: `/x2-phase june-effect C9h-2d review` on Opus 5.5 high. Also `C9h-2e` (identify, Opus 5.5 high).

### C9h-2d - Review (2026-10-08, Opus 5.5 high): no finding, no code change, no deploy

**Play-test read first:** the PASSED run above (exe `affec372`, 152 `skip` lines, `loaded=241 skipped=152`, 0 EXCEPTION).

**Checked against June (Ghidra read-only, literals read from `unpacked_x2.exe`).**
- **Lookup `32808770`, re-decompiled; every branch matches `GetPreviewIDByItemID` (`X2ItemManager.cpp`).**
  - Row copy: `[2] == type && [0] == item`. "Now" = `31edca50`, taken only on the `code == 0` path.
  - `code == 0`, `[3] < 1`: `31c1c240` is `wstring::compare( const wchar_t* )` (strlen, then `31c1c1a0( 0, size, s, n )`), and `3344f1f4` reads `00 00` (`L""`).
    - So `JZ` = equal = empty: either date empty -> push the pet (`@32808abd`, `@32808ad6`, ECX = `+0x10` then `+0x2c`).
    - Otherwise: a failed parse skips; `start < now` and `now < end` (both `CTime::operator<`) push.
  - `code == 0`, `[3] >= 1`: no empty test. In window: `find([3])` misses -> `find([1])`, erase on a hit (`325f3d20`), push `[3]`. Out of window or a failed parse: push `[1]` when neither is present. This equals `JunePreviewUseSecond` / `UsePet`.
  - `code != 0`: `DAT_33775864` (= `g_pInstanceData`, confirmed by the handler's `+0xa18`) is null, or `32822650` misses -> `[3] < 1` skip, else the fallback; a hit -> `[3] < 1` push, else the swap. Matches.
  - Tail: empty vector -> `OR EAX,-1` (`@32808bce`, and the head's empty-table `JZ`). Otherwise `@32808bda` `rand()`, `XOR EDX,EDX; DIV ESI` (unsigned) -> `vec[ rand() % size ]`. Matches.
- **Loader `32812ce0`.** Item and pet are tested with `JLE` (signed `<= 0` -> silent skip). `a5 && a6` -> both converted by `31c335e0`.
  - `31c335e0` = `toWideString( p, -1 )`: `MultiByteToWideChar( 0 = CP_ACP, ... )`, `-1` drops the NUL, the same as `KncUtil.h:222`.
  - Otherwise both are set to `L""` (`31f54a10( 3344f1f4 )`). Code starts at `XOR EAX,EAX` and becomes `atoi( a5 )` only when `a5 && !a6`. Matches.
  - **Which string is the start:** a5 is assigned to `[ESP+0x44]`, which becomes the 5th argument of `32822b60`. The row ctor copies its 5th argument to `+0x10`, and the lookup parses `+0x10` and tests it as `start < now`. So a5 is the start date, as ported.
  - The argument order into the row is (item, pet, type, second, a5, a6, code), the same as `PREVIEW_PET_DATA`'s ctor.
- **`31c55270`.** On a parse failure it returns false with no shift; otherwise `time += (__int64)offset - (__int64)_timezone` (`CDQ; SUB; SBB; ADD; ADC`). `31edcea0` = `MOV EAX,[ECX+0x470]; CDQ`. Both match (`CTimeSpan( GetServerUtcOffset() - lTimeZone )`, an `__int64` int member).
- **Packet `320de0f0`.** The map (tag 0x18) at `+0`, then a set (0x16) at `+0x10`, then a set (0x16) at `+0x20`. This matches Put/Get (map, A, B).
- **Handler `31f00970`.** Under `g_pInstanceData != 0` it assigns the map to `+0xa18`, set A to `+0xa94` and set B to `+0xaa4`, then `32344880()` and the vtable `+0x100`. Nothing else: no NEW_YEAR / CHECK_POWER blocks, so dropping them is right. The tail is not ported, the Done note says so, and STATUS has row C9h-2e (kickoff `C9h-2e.md` exists).
- **Missing Lua arguments.** `KNCSDK/Src/lua_tinker.cpp` `read<const char*>` = `lua_tostring` (nil -> NULL), `read<int>` = `lua_tonumber` (0), and `lua2enum` = `(T)(int)lua_tonumber` (0 = `PPT_PET`).
- **Data.** `kom_contents/data036_kom/PetPreView.src.lua` is byte-identical to `june_src_fixed`'s. The live `data036.kom` against the pre-copy backup `data036.kom.20261008_083402` (`elstools inspect`): +0 added, -0 removed, changed `PetPreView.lua` only. `kom.py status data036`: in sync.
- **The `+0x470` init** (`X2Data.cpp`, in the ctor) is outside every `#if`. The item manager's two counters are not set in its ctor, but they are zeroed before the only `PetPreView.lua` load, so this is not a finding.

**Hygiene**: `flagcheck JUNE_PET_PREVIEW_LOOKUP --range 7ba2f9d^..HEAD`:
- `files: 8 PASS, 0 PASS~ (blank/marker only), 2 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 5 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`
- The 4 OTHER rows are peers' commits inside the range: `10ff360` (X4, `SERV_IRUHADEV_KOM_LONG_NAMES`: Always.h + KGCMassFileManager.h/.cpp) and `f1efeaa` (C9h-3b, `RIDINGPET_STATE_DEBUG` off, Always.h +1/-1).
- The 5 lines without the flag token are bare `#else` (X2ItemManager.cpp 2, .h 2, X2State.cpp 1). Every `#endif` carries the token.

**Next**: `C9h-2e` (identify, Opus 5.5 high).

---

### C9h-2e - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): the handler's tail is two June systems with owners elsewhere -> Elysion E1, backlog Stage 9

There are 17 funcmap rows tagged `Effect C9h-2e`. The `32344880` row corrects C9h-2d's. The binding names were decoded from the XOR-built strings in `31d95080`.

**1. `32344880` = the quest manager re-evaluating village NPC relation data.**
- `ECX` is `g_pData(33775860)+0x4c`, the quest manager (`+0x50` is the skill tree). The kickoff's `33775860` is `g_pData`, not a class.
- `33775870` is `g_pTFieldGame`; the function does nothing outside a village.
- It takes the current village from the location manager (`g_pData+0x268`: `324f1e10`, `324f23a0`). For each house id in that village it looks the house up in June's relation rows: ByQuest `32340320` first, then ByVillage `323446c0`.
- It then replaces (`323b7ff0`), removes (`323a9ed0`) or restores (`323b8970`) the field NPC.
- **The 0x388 row:** `+0` QUEST_ID, `+4` TARGET_NPC_HOUSE_ID, `+0xc` `m_iUseCodeEventTriger` (-1 = no gate; otherwise March's `IsEnableCode` test on instance map `+0xa18`), `+0x80` VILLAGE_ID.
- **Where the rows come from:** `g_pQuestManager:AddVillageNpcRelationDataByQuest_LUA` (`3233b680`) and `...ByVillage_LUA` (`32340720`), in `data034` `DLG_Map_House_NPC_RelationData.lua`.
  - ByQuest has 36 rows: 3 change, 6 not-show (likely the second 0x14-byte shape at `+0xbc`), and 27 code-event rows, all event 2308 with quest 0.
  - ByVillage has 11 rows.
- **March has none of this.** It is the NPC relation system that **Elysion E1** already plans to port whole; that plan has not started and has no worktree yet.
- E1 needs three entry points, not one:
  - village NPC creation `323ba5f0`;
  - the per-quest refresh `32340630`, called from quest-UI handlers `323e15e0`, `323e1710` and `323e57e0`;
  - this tail.

**2. Vtable slot `+0x100`: June-only.**
- In `CX2State` (vtable `3346ac84`) slot `+0x100` is the empty `31f3f070`. Its neighbours `+0xfc` and `+0x104` are the empty `3208ddc0`.
- March's `CX2State` and `CX2StateMenu` have no virtual for event buttons or class-up notices, so June inserted all three slots. They have no March name.
- Every `CX2StateMenu` state overrides `+0x100` with `3245d3a0`, which refreshes June's code-event menu icons:
  - **drop:** icons and popups whose event has turned off are destroyed (maps `+0x44c` and `+0x45c`);
  - **add:** icons for events that are on and within the unit's level range are added through `+0xfc` (`324637d0`).
- The icon table is `g_pData+0x584`, filled from `data036` `EventButtonIconInfo.lua` (29 rows) by `EventButtonAddSystem:AddEventButtonIconInfo` (`31ee94a0`) and `AddEventButtonIconUsingState` (`31ee39f0`).
- `+0x104` (`3245abd0`) is a June-only class-up notice button, recorded as a candidate.
- March has no counterpart; its menu hard-codes `IF_EVENT_ENABLED` blocks.

**Reach for a March player: none today.**
- `m_mapEnableCodeEnum`'s only setter is this handler, and the offline emulator never sends the packet. So no code event is ever on, and the handler itself never runs.
- Even with a ported tail, the 27 code-event NPC rows and all 29 event icons stay dormant.
- The ungated rows reach players through E1's other two entry points, not through this tail.
  - E1 has to settle one of them: the ByVillage row for `VMI_SPA_RANOX` is a **March** village (1501), and it swaps the billboard to June's `UI_NPC_BILLBOARD_SPA.lua`, which March lacks.

**Routing (no port kickoff and no new STATUS row; both systems now have owners):**
- **Tail -> Elysion E1.**
  - `ELYSION_PORT_PHASES.md` Phase E1 step 5 was added, outside its frozen inline kickoff.
  - Backlog Stage 1 has a hand-off line: add the call inside C9h-2d's flag-on handler block, behind E1's flag; whichever branch merges second wires it.
- **Virtual `+0x100` (the event-icon system) -> backlog Stage 9 (UI sweep)**, new bullet: check the `STATE_MENU_UI_CUSTOM_MSG` ids and the 29 dialogs against March first.

No JUNE_BUGS row: nothing here is ported.

---

### C9h-3 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): the riding pet's UI sets port now; its StateChange splits three ways -> C9h-3a/3b/4/5, Stage 17

Read-only in Ghidra. There are 43 funcmap rows tagged `Effect C9h-3`. They correct C9h in four places:
- the riding-pet holder is EffectSetInstance **`+0xd8`** (mode 3, `ESM_COMMON`);
- `+0xe0` is a `CX2TFieldNpc*` (mode 2);
- there are **two** vectors, `+0x4b0` and `+0x4c0`, not three; C9h's `+0x4b4` is the end pointer;
- `32d2f560` is `StateChange( const char* )`, not the int overload.

They also correct C9h-2b's `324943c0`: it is not `CX2RidingPetManager`, which is `321f7130` (RTTI).

**1. The UI effect-set system** (-> **C9h-3a**, Sonnet, `SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX`).
- **Members.** `+0x4ac` is a *hide* mirror. `+0x4b0` holds the persistent handles: INIT `BY_UIEFFECTSET`, the reserved sets, and state sets without delete-on-end. `+0x4c0` holds the per-state handles. The map `+0x4d0` is `map<int, {deleteCond, playCond, name}>`.
- **Loader.** `32d2b0d0` runs at the end of SetRidingPet and SetGameRidingPet.
- **StateChange's own blocks.** Reserved deletes run first (delete cond 1, then `DELETE_RESERVED_UI_EFFECT_SET`). At the end comes the riding pet's own `EFFECT_SET_LIST`, played on the UI set through `321e2790`, then `32d2bad0` (`PLAY_RESERVED_UI_EFFECT_SET`).
- **Visibility and lifetime.** Visibility follows the square unit's `+0x518` in the village Prepare and the manager's NotifyShowObjectChanged. RemoveRidingPet stops everything. The dtor frees without stopping.
- **Engine.** `321e2790` is PlayEffectSetByPet's shape on a riding pet. Its consumers are GetEffectPosition, GetCombineMatrix, ByBoneName, GetUnitRotation, the create-time anim gate and the dtor.
- **Data.** June's two RESERVED enums go into March's `Enum.lua` verbatim.

**2. The rest of StateChange / StateChangeFuture.**
- **Verbatim March moves.** ANIM, push/pass, speeds, SPEED_TIME, INVINCIBLE, SUPER_ARMOR (NOT_RED is now written only under SA), cameras, the four cancel-afters and the three delete flags. June's protected-value writes stay plain March fields.
- **Riding-pet-local, new** (-> **C9h-3b**, Sonnet, `SERV_IRUHADEV_JUNE_RIDINGPET_STATE`):
  - `ANIM_START_TIME`;
  - `ANIM_NAME_AT_PLAY_END` (OnFrameMove `32d286c0`);
  - `ONGROUND_CHANGE_STATE` (`32d3cff0`, called from the user's land path);
  - the manager's ten anim-speed rates (ctor 1.0, no writer found);
  - the stored `TARGET_RANGE` / `LIMIT_Z_DEPTH` / `DEGREE_RANGE` / `VALUE_POS`;
  - the first-change flag `+0x48c`.
- **User / square-unit side, unowned** (-> **C9h-4**, Opus identify):
  - `DISABLE_GRAVITY_TIME_*`, `UPDOWN_CANCEL_AFTER`;
  - the `SOUND_PLAY_ALL_OPTION` family (user `+0x85c4`, square unit `+0x5dc`) and June's `SOUND_PLAY` reader;
  - `DELETE_EFFECT_SET_OFF_RIDING` / `_ON_SKILL_STATE_END`, `CAN_PUSH/PASS_UNIT_TIME%d`;
  - the riding-pet target search `31fe2420`.
  - The same keys live in June's `CommonStateStart` and `LoadEffectSetFromScript`.
- **Transformer riding pets** (-> **backlog Stage 17**): `TRANSFORM` (`32d281f0`), the 22 state-id getters, the scale times (`32d284d0`), `TRANSFORM_VILLAGE` (`32d3cf40`), `APPLY_RIDINGPET_ANIM_MOVE` and `CHECK_RIDINGPET_SPACIAR_BUFF`. The kickoff's "already ported" was wrong: Buff Phase 10 recorded it as *not* ported. In June data only Bumblebee and Optimus Prime use them.
- **`CUSTOM_RENDER_STATE`** (`31fce500` -> user `+0x8e68`) -> backlog Stage 2.

**3. Other routes.**
- The `+0xe0` field-NPC holder (`321e26d0`, `CX2TFieldNpc::OnFrameMove` `325cdfa0`) -> **C9h-5** (Opus identify).
- C9h-2b's pet-info riding block (`328fce40 @328fd0d8`) is gated on `IsEnableCode( 2155 )`, which the offline emulator never sends. Its `+0xd1` byte belongs to the unnamed singleton `324943c0` and has no writer found -> Stage 17.

**4. Reach.**
- June has 57 riding-pet scripts; March has 15, and **all 15 are in June**.
- The 15 March pets gain only `SOUND_PLAY_ALL_OPTION` from June's versions. That is each pet's summon and start sounds, *moved* out of `SOUND_PLAY`. So adopting June's scripts before C9h-4's reader exists silences them.
- Every other new key is in the 42 June-only pets (Stage 17). For example, the reserved / viewer keys are in 7, `TRANSFORM` in 2 and the target keys in 2.
- But the UI-set block (C9h-3a) runs for **every** state of every riding pet, because March's scripts already carry `EFFECT_SET_LIST`:
  - in a village, sets appear where March showed none;
  - in a dungeon, June plays each name twice (user loader timed, riding pet immediately; JUNE_BUGS 108).

**JUNE_BUGS 107-111** (per-state vector never cleared; the double play and skipped start times; dtor without stop; the hide mirror zeroed by show/hide; a reserved set stopped (queued) in the same StateChange is skipped by `NOT_PLAY` and dies).

Kickoffs written: `C9h-3a.md`, `C9h-3b.md`, `C9h-4.md`, `C9h-5.md`; STATUS rows added. C9h-3a and C9h-3b both edit StateChange, so run them one after the other. C9h-4 and C9h-5 can run alongside either.

### C9h-3a - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX` + `_DEBUG` (off), log `JUNE_FX RIDINGPET_FX`; play-test pending)

The in-game riding pet's UI effect sets are ported whole, with the engine holder they play through. Files: `X2EffectSet.h/.cpp`, `X2RidingPet.h/.cpp`, `X2RidingPetManager.cpp`, `Always.h`, `data036` `Enum.lua`. Read-only Ghidra; funcmap rows tagged `Effect C9h-3a` (6), JUNE_BUGS 108 corrected and 118 added.

**A correction of C9h-3 (the kickoff said the opposite).** The riding pet's own state set block (`@32d35a05`: stop `+0x4c0`, `EFFECT_SET_LIST` names, play on the UI set) runs **only without a game**:
- the in-game user block (`@32d3110d..@32d350db`) ends in `JNZ @32d350aa` / `JMP @32d350db`, both to the tail `@32d35f00`;
- the only jumps into `@32d35a05` are the village block's guards (`@32d350e8`, `@32d35105`, `@32d35154`) and its fall-through.
- So a game plays a state's `EFFECT_SET_LIST` once (the user loader `32024ff0`), not twice. JUNE_BUGS 108 is re-scoped to what is real: in a village the names are read at odd indices only (start times skipped). The reserved delete blocks and the tail `32d2bad0` still run for every state. In `CX2RidingPet::StateChange` that is an `else` on `if ( NULL != g_pX2Game )`.

**What landed**
- **Engine holder.** `EffectSetInstance::m_pRidingPetHolder` (a `CX2RidingPetPtr`, released by the member dtor) and `CX2EffectSet::PlayEffectSet( name, CX2RidingPetPtr )` = `321e2790` (power rate -1, life time -1, `ESM_COMMON`, no scale). Consumers:
  - `GetEffectPosition @321dcf6b`: RIDING_PET_BONE_NAME bone when the pet's skin anim is loaded, else land position (`m_ConditionData.m_vLandPosition`, pet `+0x33c`), else `GetPos()` (`+0x390`); no `SetEffectSetOffset` on any of the three;
  - `GetCombineMatrix @321dc81f`;
  - `GetEffectPositionByBoneName @321dd7ac` (reachable: the GROUP_NAME loop passes a non-empty name; it reads the EffectData's bone, not the caller's);
  - `GetUnitRotation @321ddf69` (`m_vDirDegree`, `+0x324`);
  - the create-time anim gate `@321eeb1e`.
  - The C9h-2a comments at `X2EffectSet.cpp:4945` / `:6383` got correction lines (insert-only: `+0xe0` is the field NPC, C9h-5).
- **Riding pet.** Members: hide mirror `+0x4ac`, kept vector `+0x4b0`, per-state vector `+0x4c0`, reserved map `+0x4d0`. Helpers `32d29240` / `32d292b0` / `32d291a0` / `32d293e0`. Loader `32d2b0d0` (INIT_EFFECT, called after InitState in both `SetRidingPet` and `SetGameRidingPet`; one log line per run). StateChange: reserved delete blocks `@32d31013` before the game test, the village-only state set block, the tail for every state. Village `OnFrameRender_Prepare`: square unit `+0x518` = `GetPlanRender` (identified by behaviour: June returns the no-render hint when it is set) synced into the mirror. Manager: `NotifyShowObjectChanged` ends with the show / hide, `RemoveRidingPet` stops everything after the erase. The dtor stops nothing (JUNE_BUGS 109).
- **June's queued stop (JUNE_BUGS 111) is emulated:** the handles stopped by the delete blocks are collected, the tail treats them as still playing (`RESPC_NOT_PLAY` skips a name whose set was just stopped), and they are stopped after the tail.
- **Data.** `Enum.src.lua` got `RESERVED_EFFECT_SET_PLAY_COND` and `RESERVED_UI_EFFECT_SET_DELETE_COND` verbatim from June; `kom.py compile` rebuilt `Enum.lua`; `kom.py pack data036` = `PACK OK` (192 entries, only `Enum.lua` changed vs the live kom). **Not copied: asking the user** (nothing March reads needs it yet; June's riding-pet scripts are C9h-4e's).

**Reach on March data.** The 15 March riding pets already carry `EFFECT_SET_LIST` (summon `EffectSet_Riding_Pet_Summon`, attacks, dashes). So in a **village** their sets now play on the UI effect set where March showed none; in a game nothing changes except the loader's one log line (no March pet has `INIT_EFFECT`). The tail and delete blocks are inert (no March pet has reserved lists).

**Hygiene** (`flagcheck JUNE_RIDINGPET_UI_FX --with JUNE_RIDINGPET_UI_FX_DEBUG --worktree --files <6 code files>`): `files: 4 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (X2EffectSet.cpp: the two correction comment lines sit inside PET_VIEWER_FX blocks), 0 FAIL, 0 ERROR`; the one deleted line is an unrelated blank line a peer removed from `Always.h`, not in the committed blob; `RESULT: PASS`. Range `53d4f10^..53d4f10`: `files: 4 PASS, 0 PASS~, 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,367,936 B, dated 2026-10-08 10:12:57, md5 3f3dd8bfb30e3a439059a47a72f97dcc, equal to X2/US_SERVICE/x2.exe (2026-10-08 10:12:22); built at mods/june-buff-port 9f23a3a + 6 uncommitted code file(s).` The strings `JUNE_FX RIDINGPET_FX loader`, `RIDINGPET_RESERVED_UI_EFFECT_SET_LIST`, `PLAY_RESERVED_UI_EFFECT_SET` and `DELETE_RESERVED_UI_EFFECT_SET` are in the deployed exe.

**Play-test (character `reyaa`, lv 80, riding pets 4, 5 and 7 in the save).** No dev console needed.
1. Village: summon a riding pet. **Look for:** the summon effect (`EffectSet_Riding_Pet_Summon`) around the pet, and attack / dash effects when the pet performs them; they should follow the pet and vanish when it stops or is unsummoned.
2. Village, optional: anything that hides your character (`CX2SquareUnit::SetShowObject( false )`, which the square unit forwards to the pet manager: an NPC cutscene, a full-screen scene) and shows it again. **Look for:** the pet's sets hidden with it and back with it; no stray effect left at the pet's last position after unsummon. Not reachable on demand offline: skip it if nothing hides the character.
3. Dungeon or battlefield: summon, walk, attack. **Look for:** nothing new (June does not play the riding pet's list on the UI set in a game); no effect lingering after unsummon.
- **If a set floats at the origin or behind the pet,** that is the holder position branch: tell me the pet and the state.
- **Logs:** `python tools/logs.py --grep "JUNE_FX RIDINGPET_FX"` shows one `loader` line per summon (`reserved=0 by_uieffectset=0` expected on March data), 0 EXCEPTION.

**Open for the user:** `/x2-kom-packed copy data036` (say "copy data036"; it only adds the two Enum tables).
### C9h-3a - Review (2026-10-08, Opus 5.5 high): no transcription finding, no code change, no deploy

**Play-test:** run before this review (STATUS: 5 loader lines, `reserved=0 by_uieffectset=0`, 0 EXCEPTION; data036 copied 10:17). `info_id=-1` in those lines is log-only: `m_Info` is set by `SetInfo` (`X2RidingPet.cpp:547`) after the loader runs, and June's loader logs nothing. Left as is.

**Checked against June (Ghidra, read-only).**
- **Game / village split (`32d2f560`).** A scan of every jump in the function finds only the village block's three guards (`@32d350e8`, `@32d35105`, `@32d35154`) entering `[@32d35a05, @32d35f00)`, plus the fall-through from `@32d35a01`. There are no indirect jumps. The user block ends in `JNZ @32d350aa` / `JMP @32d350db`, both to `@32d35f00`; a missing user exits at `@32d31193`. The village block is entered only from the four game-test guards `@32d31114..@32d31150`. So the `else` on `if ( NULL != g_pX2Game )` is right, and JUNE_BUGS 108 stays as re-scoped. The tail runs before the state table's `EndTable` in both builds.
- **State set block.** The stop loop queues each `+0x4c0` handle through a copy (`321dafb0(&copy, 0)`). `EFFECT_SET_LIST` (`33476f88`) is read at 1, 3 .. 99 and breaks at the first non-string. The XOR-built key decodes to `DELETE_EFFECT_SET_ON_STATE_END` (default false). A handle of -1 is not pushed; a valid one goes to `+0x4c0` or `+0x4b0` by that key, then `321de4f0( +0x4ac == 0 )`. Matches.
- **Loader `32d2b0d0`.** It reads `INIT_EFFECT`, then BY_UIEFFECTSET (1..99, break), then RESERVED tuples at `base` / `+1` / `+2` / `+3` = name, index, playCond, deleteCond. Bases are 1, 5 .. 97; the loop breaks at the first incomplete tuple. The node is key `+0xc`, deleteCond `+0x10`, playCond `+0x14`, name `+0x18`. `32899920` is a unique map insert (it frees the node on a duplicate key), so the first tuple for an index wins. The plays run after both `EndTable`s, gated on `g_pData + 0x9c` (the decompiler's `local_94` is a stack-slot mix-up). Every handle is pushed into `+0x4b0` with no -1 test (JUNE_BUGS 118). The only callers are `32d3cbf0 @32d3cc86` and `32d3cd10 @32d3ce41`, each right after `32d36600` (INIT_STATE); the call order is the same as March's.
- **Tail `32d2bad0`, finder `32d292b0`, name `32d29240`, queued stop.**
  - The tail skips an index only when playCond `+0x14` is 1 and the finder returns a handle. It keeps a name when it compares unequal to `L""`. A handle of -1 is not pushed; a valid one goes to `+0x4b0` and gets show = `+0x4ac == 0`.
  - The finder scans `+0x4b0` only, matching the templet (`+0x80`) name at `+0x10`. It writes -1 on a miss.
  - `321dafb0` only pushes `{handle, bool}` onto the effect set's `+0x30` vector and sets the caller's copy to -1. `321d97c0` checks only the handle's index and stamp, with no queue test. So June's queued stop does let the tail find a set that was just stopped, and emulating it (JUNE_BUGS 111) is right.
  - Delete blocks `@32d31013`: per map node, the finder's handle is stopped when deleteCond `+0x10` == 1. `DELETE_RESERVED_UI_EFFECT_SET` (`33525fa0`) is read as ints and breaks at the first miss.
  - The only callers of `321e2790`, `32d2bad0`, `32d292b0` and `32d29240` are the ones the port has.
- **Engine holder.**
  - `321e2790` matches: a null templet or a null pet gives -1, then `321e1bf0` with no owners. Afterwards `+0xd8` = pet (a counted ref, the old one released), `+0x60` = `+0x68` = -1.0 (`334380f0`), mode 3 (`ESM_COMMON`), `+0x20` = 0, and no scale.
  - `GetEffectPosition @321dcf6b`: the bone is used only if the name at `+0xac` is non-empty and `320719f0` passes. `320719f0` is loaded `+0x1bc` and anim `+0x238`, which is March's `GetXSkinAnim()`. Otherwise `+0xcb` selects `+0x33c`, else `+0x390`. All exits go straight to `@321dd531`, with no offset.
  - `GetCombineMatrix @321dc81f` is the raw `32071a10`, frame `+0x9c`, `+0x90`.
  - `ByBoneName @321dd7ac` follows the same shape with `321f2eb0`; `bBoneFound` is zeroed at entry in both builds.
  - `GetUnitRotation @321ddf69` returns `+0x324`.
  - Gate `@321eeb1e`: the effect is skipped while anim time `+0x214` < start + delay; with no anim it falls through.
  - Completeness: in `CX2EffectSet::OnFrameMove` `321f28f0` and its four callees, `+0xd8` is read only at the gate.
- **Members.** `RidingPetCondition` (bool, float, float, dirDegree, dir, landPosition) puts `+0x324` / `+0x330` / `+0x33c` in order.
- **Visibility.**
  - Prepare `@32d2c4b2`: when `g_pTFieldGame` is set and the square unit is found, `+0x518` is compared with `+0x4ac`. On a difference `32d291a0( !flag )` runs, and if it returns true, `+0x4ac` = flag. Then the no-render return. The March block sits in the same branch.
  - `32d291a0` shows / hides both vectors, then zeroes `+0x4ac` (JUNE_BUGS 110).
  - `32d293e0` queues stops on both vectors, then `resize(0)` (`32a2d590`) on each.
  - `321f5cb0` ends with `32d291a0( bVal )`. Its `32d2e090` call on a show is March's existing `InitPos`.
  - `321f6190` calls `32d293e0` after the erase.
  - The dtor `32d2bdd0` calls neither a stop nor `32d293e0` (JUNE_BUGS 109).
  - `CX2RidingPetPtr( this )` is safe: `X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR` (`KTDX.h:340`) makes it an intrusive pointer.
- **Data.** Both tables in `kom_contents/data036_kom/Enum.src.lua:12460-12467` are verbatim from `june_src_fixed/data036_kom/Enum.src.lua:17597-17604`. The live `data036.kom` (md5 `d67cc28e`, the same file as `data036_packed.kom`) differs from the pre-copy backup `kom_backup_tools/data036.kom.20261008_101736` only in `Enum.lua` (331,868 -> 332,019 B); both have 192 entries.

**Hygiene** (`flagcheck JUNE_RIDINGPET_UI_FX --range 53d4f10^..HEAD --with JUNE_RIDINGPET_UI_FX_DEBUG`): `files: 4 PASS, 0 PASS~ (blank/marker only), 2 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- The X2EffectSet.cpp OTHER row is the two insert-only correction comments inside `PET_VIEWER_FX` blocks.
- The four `8ec9968` OTHER rows are a peer's offline-emulator commit (riding pet into the dungeon), under other flags.
- The one deleted line is `1eb9563`, a peer's removal of a CRLF-only blank line in `Always.h`, outside this flag's block.

Next: `/x2-phase june-effect C9h-4b` (Sonnet 5.5 high), ready.

### C9h-3b - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_RIDINGPET_STATE` + `_DEBUG`, log `JUNE_FX RIDINGPET_STATE`)

The riding pet's `StateChange` / `StateChangeFuture` now follow June (`32d2f560` / `32d2c8b0`) for the riding-pet-local keys. Files: `X2RidingPet.h/.cpp`, `X2RidingPetManager.h/.cpp`, `X2GUUser.h/.cpp`, `Always.h`. Read-only Ghidra plus capstone; funcmap rows `Effect C9h-3b`.

**What landed**
- **Parsers.** `ANIM_START_TIME` feeds `ChangeAnim( name, transition, start )` in both functions; `APPLY_RIDINGPET_ANIM_MOVE` is read first in the Future parser; `TARGET_RANGE` / `LIMIT_Z_DEPTH` / `DEGREE_RANGE` / `VALUE_POS` are stored (1500 / 500 / 45 / 0; getters for C9h-4).
- **`ANIM_NAME_AT_PLAY_END`** and its player `PlayAnimAtPlayEnd` (`32d286c0`), the last statement of the loaded block of OnFrameMove.
- **`ONGROUND_CHANGE_STATE`** + `OnGroundChangeStateCheck` (`32d3cff0`). Its caller is June's `DoFrameMoveFuture` (`320347d0 @32034858`), not a land path: true skips the state's event process. `CommonStateEnd` is protected in March, so `CX2GUUser::CommonStateEndByRidingPet()` wraps it. The pet flag `+0x620` is set by the check and cleared at the top of `CX2GUUser::StateChange( int, bool )` (`320348c0`).
- **Manager rates.** Ten floats, all 1.0 (no June writer). ANIM_SPEED x the *second* float of the state's pair, only with `g_pX2Game`; PASSIVE_SPEED_X x the *first* float of pair 4, placed after DELETE_EFFECT_SET_ON_SKILL_STATE_END (`@32d35029`). The pair-to-state map was decoded from June's XOR-built INIT_STATE strings (funcmap `32d36600`): pair 0 WALK, 1 DASH, 2 JUMP_UP or JUMP_DOWN, 3 DASH_JUMP_UP, 4 SPECIAL_MOVE. **This corrects funcmap row `321f6ec0`** (it said wait / walk-type).
- **Transformer keys parsed and stored**, consumers left out (Stage 17): `TRANSFORM` (local), `ORIGINAL_SCALE_TIME`, `TRANSFORM_SCALE_TIME`, `CHECK_RIDINGPET_SPACIAR_BUFF`.
- **User block:** `SUPER_ARMOR_NOT_RED` written only under `SUPER_ARMOR`. `SUPER_ARMOR_TIME` needed nothing: March's `AccessVecNowSuperArmorTime()` is that container.
- **First-change flag (`+0x48c`).** Set before the name lookup (a miss sets it too), never cleared. OnFrameMove no longer sets `m_bInitPos` every frame; it sets it on the first frame after the first StateChange and runs GetUserInfo / SetPos a second time after the skin anim's frame move (`32d2ec80 @32d2f19f`).
- **`IsValidUser`** (`32d2c800`): same as March :751 (including the unchecked dereference). No edit.

**Not ported, with reasons**
- **`32031e60`** (the `+0x620` arm of `DoFrameMoveFuture`: riding-pet cancel-after checks, then CommonEventProcess) reads C9h-4's cancel-after members and calls `3202f460`. Routed to **C9h-4 item 7** (kickoff edited); March falls through to the state's own `OnEventProcess`. Only June's Optimus_Prime reaches it. This is the advisor-backed correction of the kickoff, which said "the user's land path".
- The user-side keys (gravity, UPDOWN, SOUND_PLAY_ALL_OPTION, OFF_RIDING / SKILL_STATE_END deletes, CAN_*_TIME) are a comment at the end of the user block, not dead local reads.
- `+0x5b0` / `+0x5b4` are dead stores in June; not stored.

**Not inert on March data.** The first-change gate changes every pet: before its first StateChange `m_bInitPos` stays false, so the manager's `InitPos` path (NotifyShowObjectChanged) runs its body, and the first frame after the first StateChange repositions the pet once more. Everything else is inert on March data (no script carries the new keys; the rates are 1.0).

**JUNE_BUGS 113-115** (ONGROUND wait skips the event process on every frame; the manager pair's dead float; NOT_RED keeps the old value). Hygiene (worktree, only my files): `files: 1 PASS, 5 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags: Always.h), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 5 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS` (the 5 are the original `#else` / `#endif` lines of replaced blocks). Range line: see the commit.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,348,992 B, dated 2026-10-08 08:20:24, md5 affec3722e7bead5d6f914dbc7a60989, equal to X2/US_SERVICE/x2.exe (2026-10-08 08:20:19); built at mods/june-buff-port 4567c75 + 16 uncommitted code file(s).` The exe was linked by a peer's build that already contained these edits; `JUNE_FX RIDINGPET_STATE` is in it. The build also contains the peer's uncommitted C9h-2d code.

**Play-test (regression pass):** dev console not needed. In a village: summon a riding pet, walk, jump, dash, unsummon, hide / show. In a dungeon: the same plus an attack. Look for: the pet appears at the right place on summon (the first-change gate), nothing freezes, one `JUNE_FX RIDINGPET_STATE` line per state change in `ErrorLog.txt` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_STATE"`). **Review:** `kickoffs/june-effect/C9h-3b-review.md`.

### C9h-3b - Review (2026-10-08, Opus 5.5 high): no finding, no code change, no deploy

**Play-test read first:** the PASSED run (187 `JUNE_FX RIDINGPET_STATE` lines, Red Hameling, 0 EXCEPTION; exe `6877fb3f` from 4bd51f2 still has `_DEBUG` on, f1efeaa's off lands with the next build).

**Checked against June (Ghidra / capstone read-only, literals read from `unpacked_x2.exe`).**
- **Slot order.** Decoded the XOR strings before each store in the INIT_STATE loader `32d36600`: `+0x4ec` = `WALK_STATE` (`@32d37459`), `+0x4fc` = `DASH_STATE` (`@32d37a14`), `+0x534` = `SPECIAL_MOVE_STATE` (`@32d390fe`). `32d277f0` returns `+0x4f4` (JUMP_DOWN) and does not touch EDX, which the caller reuses.
- **Pairs.** `@32d30a3f..@32d30b87`: five independent ifs (WALK, DASH, JUMP_UP or JUMP_DOWN, DASH_JUMP_UP, SPECIAL_MOVE) multiply ANIM_SPEED by the second float (`+0x90/+0x98/+0xa0/+0xa8/+0xb0`); the first goes to a dead slot. Each slot reads the `+0x550..` transform set when `+0x5a0` is set, which March never sets (TRANSFORM is dropped, Stage 17). PASSIVE_SPEED_X (`+0x30c`, read after SPEED_X/Y, before `+0x310`) x `+0xac` = the first float of pair 4 (`@32d35055`). Matches.
- **Gate (`32d2ec80`).** `+0x1c0` = `m_bInitPos`, `+0x48c` = the first-change byte; the test sits after the first GetUserInfo / SetPos (`DAT_33439c68` = 30.0). The second pair runs only in the (game or village) branch, after the skin anim's OnFrameMove (`+0x238` vslot 1). `32d286c0` is the last call before the return (only xref `@32d2f3b1`). StateChange's `@32d2f5a6` is `if( !+0x48c ) +0x48c = 1`, so a plain set is the same.
- **`PlayAnimAtPlayEnd` (`32d286c0`).** `+0x2ac` = `m_bAnimEnd` (after `m_PlayType` `+0x2a8`, which `Play` `31d28d00` writes), a non-empty test on `+0x490`, `ChangeAnim( name, false, 0 )`, `Play( 0 = XAP_LOOP )`. Matches.
- **Check (`32d3cff0`).** `334380f0` = -1.0. Equal -> change; else `FCOMPP` time vs overlap with `TEST AH,5; JP`: time < overlap -> return 1 (wait), time >= overlap -> change. `+0x214` = `m_fAnimCurrTime` (anchored by `+0x224` = `m_NowAnimIndex`, which `ChangeAnim` `31d33fd0` writes). ECX of `31fc8810` is still the user from `@32d3d02e`. `31fc8810` is a `jmp [vtbl+0x7dc]` thunk, so June dispatches virtually, the same as the wrapper's `CommonStateEnd()`. Then `32d2f480` (-> `32d2c8b0` StateChangeFuture), then `32d3cb10` (-> `32d2f560` StateChange). Matches.
- **User hooks.** `320347d0 @3203484e`: the pet object (`+0x683c`) non-null, GetRidingOn not tested, true skips both event paths. `320348c0 @320348d0` clears `+0x620` as the first statement. The `+0x620` arm comment is in place, and C9h-4c owns it. The rest of `320348c0` keeps March's two `DoFrameMoveFuture` re-simulation loops (`@32034b32`, `@32034bb2`; March :7862, :7893).
- **Keys and defaults** (`func_strings.tsv` order = the port's order; default stores at each getter): APPLY 0, ORIGINAL / TRANSFORM_SCALE_TIME -1.0, CHECK_..._SPACIAR_BUFF -1 (`> -1` -> `32d2f3d0`), ONGROUND table absent -> `+0x5b8` 0, `+0x5bc` 0 (EBX = 0 from `@32d3003c`), OVERLAP_TIME 0, STATE_NAME "" -> `32d3e450`, PLAY_TYPE 2, TRANSITION true, ANIM_START_TIME 0 and passed in both ChangeAnim branches, TARGET_RANGE 1500 / LIMIT_Z_DEPTH 500 / DEGREE_RANGE 45 / VALUE_POS 0. NOT_RED `@32d3262a`: written only when SUPER_ARMOR == 1. Spellings match `june_src_fixed/data037_kom` (Optimus_Prime ONGROUND, 34 files ANIM_START_TIME).
- **Init.** June's reset `32d282f0` (only caller: the ctor `@32d3655e`) zeroes `+0x5b8/+0x5bc/+0x620`, the same as the port's ctor values (a capstone sweep of `.text` finds one call each to `32d282f0`, `32d286c0`, `32d3cff0`); March's `ClearState` has the ctor as its only caller too.
- **Not ported on purpose.** `IsValidUser` (`32d2c800`, raw disassembly) = March :796, unchecked dereference included. `+0x5b0/+0x5b4`: inside `32d27000..32d40000` only stores (the reset, StateChange, the ctor); readers through other classes were not swept (389 `.text` hits of the offset, mostly other classes).
- **JUNE_BUGS 113-115** describe what the code above does.

**Unowned June branch -> C9h-6.** `320347d0` calls user delegates `+0x9544/+0x9548` before OnFrameMoveFuture and `+0x954c/+0x9550` before OnEventProcess. They are a snapshot of the state's delegates taken by `31fd2af0` (11 class-side callers, gated on `GetNowStateSkillID() > 0`). March has neither. They are not riding-pet code, and no row owned them, so they get the identify phase **C9h-6** (`kickoffs/june-effect/C9h-6.md`).

**Hygiene**: `flagcheck JUNE_RIDINGPET_STATE --range d36b1b1^..HEAD --with JUNE_RIDINGPET_STATE_DEBUG`:
- `files: 1 PASS, 5 PASS~ (blank/marker only), 2 DEFINE, 13 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 5 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`
- The deleted line is f1efeaa's `#define ..._DEBUG`, now commented out (DEFINE row). The 13 OTHER rows are peers' commits `7ba2f9d` (C9h-2d) and `10ff360` (X4). The 5 lines without the token are this flag's own bare `#else` (X2GUUser.cpp 1, X2RidingPet.cpp 4); the Done note's "original #else/#endif" was wrong.

**Also:** `C9h-3a.md` line numbers updated for C9h-3b's inserts. **Next**: C9h-3a (Sonnet 5.5 high), or the identify phases C9h-2e / C9h-5 / C9h-6 (Opus 5.5 high).

### C9h-4 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): the June keys are a sound system, a cancel-after, three delete conditions -> C9h-4a..4e

Read-only work in Ghidra and capstone. Results:
- 37 funcmap rows tagged `Effect C9h-4`;
- a census, `june2017_re/c9h4_unit_key_census.tsv` (script `c9h4_unit_key_census.py`; March is counted from the bytecode string constants
  of `kom_contents`, June from `june_src_fixed`);
- JUNE_BUGS 116.

**Corrections to C9h-3 and C9h-3b**
- **Four "June-only" keys are March's.** March already parses `DISABLE_GRAVITY_TIME_*` and `CAN_PUSH/PASS_UNIT_TIME` (X2GUUser.cpp:14328,
  :14270, :14611), with the same consumers. Only the riding pet writing them is June's.
  - June's gravity window (`32061bf0 @3206263f`) differs in one thing: its clock is the second skin anim `+0x8e84` when `+0x8e90` is set.
    Checklist 7, line 224 already owns that.
- **`32031e60` is in March.** It is `CX2GUUser::RidingAttackZEventProcess` (state 0x58 in all 12 classes, thunk `329cb460`), and
  `DoFrameMoveFuture` also calls it on the `+0x620` arm. C9h-3b's row said "none in March".
- **The square unit's sound list** is stopped only by the riding pet: StateChange's village block, and riding-off `323f70a0`.
- **The `VALUE_POS` reader**, which C9h-3 did not find, is `32005520` (via `320322e0`).

**1. Sound** (the core) -> **C9h-4a** (CX2GameUnit + user), **C9h-4b** (NPC, square unit, event scene).
- **The record.** June's record is March's `SoundPlayData` plus a delete-condition list (seeded `DCS_NONE`) and LOOP; it is 0x44 bytes.
- **Three readers fill it:** `SOUND_PLAY`, `SOUND_PLAY_EX` (user only) and `SOUND_PLAY_ALL_OPTION`. `STATE_SOUND_PLAY` fills a second
  list, and `CUSTOM_STATE_DELETE_SOUND` stops one condition.
- **Delete conditions.** The attach `3227a740` sorts sounds by `DELETE_CONDITION_SOUND` into five `CX2GameUnit` lists. Five stoppers
  drain them: damage react, state end, custom state, the skill-id change, and DIE (`32276d20`). **The DIE stopper is never called**
  (JUNE_BUGS 116).
- **The nation reader.** June's `SOUND_PLAY` reader `32078fd0` tries `SOUND_PLAYn_<JP|US|BR>` first, keyed by
  `CX2Main::m_dwNationFlag`. March already has that member, and its US config sets `NF_US`, so no stand-in is needed and the reader
  resolves to March's name. June renumbered `NF_BR` to 0x700.
- **The user player** `31fea120` erases each record after it plays. March already plays a record once per state entry (CommonStateStart
  resets the oneshot timer, X2GUUser.cpp:14596), so this differs only where class code resets the timer mid-state (Chung, 12 sites).
- **Reach.** March's scripts carry none of the new keys. The only exception is `FIRE_DEVIL_CHAIN_BALL`'s `SOUND_PLAY_EX`, which no NPC
  reader reads, in June or in March. June's riding pets (51 of 57, including all 15 of March's) carry `SOUND_PLAY_ALL_OPTION`, and so do
  26 NPC / summon scripts (Elysion, Erda, Rose and Ain).

**2. `UPDOWN_CANCEL_AFTER`** -> **C9h-4c**. Default 9999.0. The engine reader is June's `RidingAttackZEventProcess` (up/down -> RIDING_WAIT;
its cancel tests are March's: time >= 0 and below the riding pet's anim time). ~27 class EventProcess readers (Elsword 25, Raven 1, Lire 1) go to checklist 7h. Its neighbour
`USE_COMMON_CANCEL_AFTER` (10 June scripts) is unmapped, and C9h-4c identifies it.

**3. `LoadEffectSetFromScript` (`32024ff0`)** -> **C9h-4d**.
- **Bytes.** Eight delete bytes at `+0x8624..+0x862b`. Three are June's: STAGE_END, OFF_RIDING and SKILL_STATE_END.
- **The hyper check.** `SET_HYPER_EFFECT_SET_CHECK_HYPER_STATE` is June's too.
- **Where each one stops:**
  - STAGE_END in the dungeon state's `Handler_EGS_DUNGEON_STAGE_LOAD_NOT` (`324d9550`; the packet layout matched);
  - OFF_RIDING in the riding-off branch (`3204d120`);
  - SKILL_STATE_END in CommonStateStart (`3228a490`).
- **Attach and data.** The attach `3227a570` gains case 5. March's `Enum.lua` lacks `DCES_SKILL_STATE_END = 5`.
- **The list variants** (`_EX_TRANS_*`, `_ULTIMATE_EX*`, `_APPLIED_SKILL`, `_FINAL*`) select by skill memo, transcendence or
  applied-skill level -> checklist 7h (D8).
- **Already in March:** `HYPER_EFFECT_SET`, `EFFECT_SET_LIFE_TIME`, `_ON_CUSTOM_STATE` and `CUSTOM_STATE_..._ON_DAMAGE_REACT`.

**4. The riding pet's user / square writes** -> **C9h-4e**. It runs after C9h-4a..4d and after C9h-3a (same StateChange).
- It replaces C9h-3b's end-of-block comment with June's writes, including the village sound block `32d350e0`.
- It then adopts June's scripts for March's 15 riding pets. **Ordering constraint:** not before both sound readers exist. June moved each
  pet's summon and start sounds from `SOUND_PLAY` to `SOUND_PLAY_ALL_OPTION`, so earlier adoption silences them.

**Routed elsewhere**
- **Backlog Stage 17:**
  - `3202f460`, June's `RidingPetSpecialAttackEventProcess` rewrite (transformed state sets and the target search `31fe2420`; every riding
    EventProcess calls it);
  - the `VALUE_POS` reader `32005520`;
  - the `APPLY_RIDINGPET_ANIM_MOVE` readers (`+0x5a1`: Bumblebee and Optimus_Prime);
  - the `CX2Pet` `SOUND_PLAY_ALL_OPTION` reader `326ef840` (the Hedgehog pets).
- **Backlog Stage 10:** `EmotionFrameMove`'s riding-sit sound replay (`3204a860`).
- **Elysion plan:** `AddEffectSetDeleteCondition_LUA` is already on its list, and needs C9h-4d's case 5. The Elysion lasers' sounds need
  C9h-4b.

Kickoffs written: `C9h-4a.md` .. `C9h-4e.md`, with STATUS rows. C9h-4a, 4c and 4d share `X2GUUser.cpp`, so run them one at a time; 4b can
run alongside 4c or 4d.

### C9h-4a - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_UNIT_SOUND` + `_DEBUG` (on for the play-test), log `JUNE_FX UNIT_SOUND`; play-test pending)

Files: `X2GameUnit.h/.cpp`, `X2GUUser.h/.cpp`, `Always.h`. Funcmap rows `Effect C9h-4a`. JUNE_BUGS 117.

**What landed**
- **Record.** `SoundPlayData` gains `m_vecDeleteCondition` (ctor seeds `DCS_NONE`) and `m_bLoop`; `CX2GameUnit::DELETE_CONDITION_SOUND` enum (values as `Enum.lua`).
- **Nation reader `32078fd0`** = `CX2GameUnit::JuneBeginSoundPlayTable`: flag 0 or 1000 plain, else `<name><n>_<JP|US|BR>` (0x200 / 0x400 / 0x700) then the plain name. On March US (`NF_US` 0x400) it probes `_US`, always misses, falls back.
- **Lists, attach, stoppers.** Five lists on `CX2GameUnit`, `AttachSoundDeleteCondition` (`3227a740`), `StopSoundOn{DamageReact,StateEnd,Die,CustomState}`, `StopSoundOnSkillStateEnd`. DIE is ported and called nowhere (JUNE_BUGS 116). `GetNowStateSkillID` is `CX2GUUser`-only in March, so callers pass it in.
- **Dangling check (kickoff step 3).** `PlaySound` opens the device (ref +1) and nothing closes it, and March's NPC already stores and `Stop()`s such pointers: no dangling, no JUNE_BUGS row.
- **CommonStateStart**: `JuneClearStateSounds` early; `CUSTOM_STATE_DELETE_SOUND` after the effect-set one; June's sound block in June's order: `SOUND_PLAY` (reader), `SOUND_PLAY_EX` (rows `{name, time, conditions...}`, no seed), `SOUND_PLAY_ALL_OPTION<n>` (8 keys, `DELETE_CONDITION` replaces the seed: `31ecace0` is `erase(all)`), `STATE_SOUND_PLAY<n>` into a second list; March's `SOUND_SET` block follows unchanged; `StopSoundOnSkillStateEnd` at the end.
- **Players.** `JunePlaySoundRecords` (`31fea120`: erases a record once played, also when `PlaySound` returned null; keeps it when not due, roll lost, or only-if-my-unit elsewhere) and `JunePlayStateSoundRecords` (never erases, 3D on, loop off, no attach, keeps the handle). Both replace March's loop in `CommonFrameMove_TimedEvent` (March's loop stays in the `#else`). Due test = March's `EventTimerOneshot` (June's `+0x24c` is `m_fAnimOneshotTimer`).
- **Stop calls.** DamageReact, the shock-enchant block, CommonStateEnd (before `DeleteEffectSetOnStateEnd`), CommonStateStart.

**Corrections to C9h-4 (found here)**
- **A fifth stopper, `32009ae0`**, which C9h-4 and the kickoff missed: it clears the `STATE_SOUND_PLAY` records and `Stop()`s their handles (`+0x85e4`). Callers: CommonStateStart `@32057de1`, CommonStateEnd `@320153c7`, ReInit `@32022455`, the dtor `@3203a68b`. Ported as `JuneClearStateSounds` at all four.
- **The `STATE_SOUND_PLAY` player is not a copy of `31fea120`** (see above); `31fdf4c0` is the reserved-buff apply, not sound; `+0x9108` is `m_bGrap` (`31fd2982` clears it with `m_bDisableGravity`, as `SetGrap(false)`), so June's `break` equals March's `continue`.
- **June's DamageReact guard is wider than March's:** it also skips `reActResult` 0, 0xb, 0xc. The sound stop uses June's guard; March's `DeleteEffectSetOnDamageReact` keeps its own (effect-set half: C9h-4d).

**Not ported:** `EmotionFrameMove`'s replay (Stage 10), NPC / square / event scene (C9h-4b), the riding pet's writes (C9h-4e), `3228a490` (C9h-4d), `USE_DIFFERENT_SOUND_WHEN_IN_SPECIAL` (JP-only; absent from June's US player).

**Hygiene** (`flagcheck JUNE_UNIT_SOUND --with ..._DEBUG --worktree --files ...`):
```
files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
(The 1 deleted line is a peer's uncommitted blank line in `Always.h`, not in my commit. The PASS~ is two blank lines I inserted; the two tokenless lines are `#else`.)

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,360,256 B, dated 2026-10-08 09:42:40, md5 6310e571063e8f58de27c566d33f6019, equal to X2/US_SERVICE/x2.exe (2026-10-08 09:42:34); built at mods/june-buff-port 28ce76c + 5 uncommitted code file(s).` The `JUNE_FX UNIT_SOUND` strings and the six keys are in the exe. A comment-only edit followed the build.

**Limit of the play-test:** no March script carries the new keys, so every attach is `DCS_NONE` and the stoppers never find a handle. `_DEBUG` writes `attach ... list=0` per sound played; the stop lines and `state_play` need June data (4e's riding pets, or a hand-made script). Only the player's erase can change what a March script does (Chung's mid-state timer resets).

**Open for the user:** none. **User owes:** the play-test below.

### C9h-4a - Review (2026-10-08, Opus 5.5 high): no transcription finding, one stale comment fixed (comment only), no deploy

**Play-test:** passed before this review (STATUS; logs read, `_DEBUG` off in `dc9610c`). The deployed exe (10:12:57, md5 `3f3dd8bf`, a peer's later build) has the keys and no `JUNE_FX UNIT_SOUND` string, so `_DEBUG` is off there.

**Checked against June (Ghidra / capstone read-only, literals from `unpacked_x2.exe`).**
- **ALL_OPTION keys** (`@3205d1cc-3205da78`, XOR buffers `0x3379d61c..0x3379d65c`). All eight decoded: `SOUND_NAME` (wstring getter `32feae90` -> rec `+0x4`), `PLAY_TIME` (`32fea370` float -> `+0x0`), `RATE` (`32fe9fd0` int -> `+0x20`), `ONLY_IF_MY_UNIT` (`32fe9e40` bool -> `+0x24`), `MAX_SOUND_DISTANCE` (float -> `+0x28`), `ENABLE_3D_SOUND` (bool -> `+0x2c`), `LOOP` (bool -> `+0x40`); `DELETE_CONDITION` is plaintext (`33476588`).
  - The decoder printed `ONLY_IF_MY_UNITU`. That is an artifact: `XOR BL,BL` stores the terminator at `+0x5ff`.
  - Defaults on a miss: "", -1.0 (`334380f0`), 100, false (BL = 0), -1.0, true, false. June's data carries six of the eight names (in 40 `june_src_fixed` files); `MAX_SOUND_DISTANCE` and `ENABLE_3D_SOUND` appear only in the binary.
- **DELETE_CONDITION replaces the seed.** `31ecace0` is `erase( begin, end )` on rec `+0x30` (`@3205d9d2`), followed by `GetValue( 1.. )` and push_back. EndTable runs only when the table opened. Matches.
- **SOUND_PLAY_EX** (`@3205cff8-3205d1cc`).
  - Rows are 1-based; `[1]` is the name, `[2]` the time (default 0), `[3..]` the conditions.
  - Per row, `31ecace0` clears the condition vector. `32079650( name, time, &conds )` builds the record (rate 100, onlyIfMyUnit 0, distance -1, 3D 1, loop 0) and copies the row's conditions with no seed.
  - An empty name jumps to the single EndTable at `@3205d19b`, which leaves `SOUND_PLAY_EX` open (JUNE_BUGS 117). Matches.
- **Other readers.** `SOUND_PLAY` goes through `32078fd0` (flag 0 / 1000 plain, `"%s%d_%s"` with `JP` / `US` / `BR` at 0x200 / 0x400 / 0x700, plain fallback; AL returned). `STATE_SOUND_PLAY` (`@3205da78-3205db96`) clears `+0x85d4`, reads plain names, defaults -1 / "" / 100 / false. Anchor: June runs `ATTACK_TIME`, then `32005780`, then the block; March runs `ATTACK_TIME`, then `StopTime_StateStart()`, then the block.
- **Stop sites.**
  - DamageReact `@3204a6f7-3204a71f`: one guard (7, 0x16, 0, 0xb, 0xc) for both `3228a270` and `32276ce0`. The port gates the sound on all five.
  - Shock block `@32068a3c`: after `3228a270` / `3228a420`.
  - CommonStateEnd: `32009ae0 @320153c7`, then `3228a200`, `32276ca0 @320157fc`, `3228a120`.
  - CommonStateStart: `32009ae0 @32057de1`; `CUSTOM_STATE_DELETE_SOUND` (bool, miss false) -> `32276d60` right after `3228a420`; `3228a490`, `32278200 @320606a9`.
  - ReInit `@32022455` (after the `+0x85c4` clear) and the dtor `@3203a68b`.
  - Attach jump table `3227a808`: 1 `+0x3f20`, 2 `+0x3f30`, 3 `+0x3f40`, 4 `+0x3f50`, 5 `+0x3f60` (user, skill > 0). Each stopper reads its own list.
  - Order differences that change nothing:
    - June's CommonStateEnd runs the reserved-buff loop and a skin-anim call before `32009ae0`;
    - June runs four unmapped calls before `32278200`, where March runs `DoCustomFunctionByBuffInCommonStateStart` / `ApplyDrainHpBuff` after it.
    - Neither touches the sound lists: only the players attach.
- **DIE `32276d20`.** Ghidra has no function there. A raw scan found 0 `E8` / `E9` targets and 0 absolute dwords pointing at it, so JUNE_BUGS 116 stands.
- **Players.**
  - `31fea120`: due = oneshot `+0x24c` < t and `31d28e10` (March's `EventTimerOneshot`), or the riding pet's anim (`+0x683c`, vslot 0x200).
    - The roll is `rand % 100 < rate`. Only-if-my-unit uses vslot 0x198 (`31ff1f30` = IsMyUnit) with 3D off.
    - Then `32276220( name, 3D, loop )` (param 3 sets the 3D position, param 4 is the play loop), attach if non-null, and erase in both cases. A failed roll or another unit's record is kept.
  - STATE player (`@31ff396f`): `m_bGrap` skips the record, `(name, 1 or 0, 0)`, pushes the handle (null too), never erases.
  - vslot 0x524 = `3208e410` GetNowStateSkillID.
- **`+0x9108` = `m_bGrap`.** June `31fd2920` is March's `SetGrap` line for line: count <= 0 or false clears `+0x84f3` (`m_bDisableGravity`) and `+0x9108`; otherwise HP > 0 runs StateChange( `+0x9110` GetGrapStateID ), zeroes nowSpeed, and sets `+0x9108`.
- **Regression.** Every reader of the user's `m_vecSoundPlayData` only clears it or appends to it: CommonStateStart, ReInit, and the riding pet's StateChange `X2RidingPet.cpp:1184`, through `AccessSoundPlayData`. No code indexes it or caches its size, so the erase is safe. On March data the only visible change is Chung's mid-state replay.

**Finding (comment only).** `X2GUUser.cpp:17638`, inside `SERV_IRUHADEV_JUNE_ENCHANT_PARITY` (`6322917`), said June's `32276ce0` list was "not ported". C9h-4a ported it, so the line now says so. A bytepatch replace of one line: encoding and EOL unchanged; `flagcheck JUNE_ENCHANT_PARITY --worktree --files X2Lib/X2GUUser.cpp` PASS, with 1 deleted line (the replaced comment). It does not change the binary, so nothing is rebuilt.

**Hygiene** (`flagcheck JUNE_UNIT_SOUND --range 5dc4944^..HEAD --with JUNE_UNIT_SOUND_DEBUG`):
- `files: 3 PASS, 1 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`
- Explanations:
  - The deleted line is `dc9610c`'s `#define ..._DEBUG`, now commented out (the DEFINE row). The Done note blamed a peer's blank line.
  - The PASS~ 2 are the blank lines after the early-clear `#endif` and after the `CUSTOM_STATE_DELETE_SOUND` `#endif`.
  - The 2 tokenless lines are the bare `#else` in CommonStateStart and in CommonFrameMove_TimedEvent.

**C9h-4b kickoff:** checked; it reads this Done note for the shipped names. One line added: its flags need `SERV_IRUHADEV_JUNE_UNIT_SOUND` on, and the NPC passes skill id 0 to the attach. **Next:** C9h-4b, C9h-4c or C9h-4d (Sonnet 5.5 high; 4c and 4d one at a time).

### C9h-4c - Done (2026-10-08, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`, `_COMMON_CANCEL`, `_CANCEL_CLAMP` + the two `_DEBUG` (on for the play-test); logs `JUNE_FX UPDOWN_CANCEL`, `JUNE_FX CANCEL_CLAMP`; play-test run, logs read: 17 attack_z lines, 0 clamp lines, 0 EXCEPTION)

Files: `X2GUUser.h/.cpp`, `Always.h`. Funcmap rows `Effect C9h-4c`.

**Corrections to the kickoff (found in Ghidra / capstone)**
- **`USE_COMMON_CANCEL_AFTER` is already mapped:** a byte at `+0x8ca0`, read only by `CommonStateStartFuture` `320506f0 @3205650d` (miss false). Its one consumer is a block at the top of June's `CommonEventProcess` `31fe7610 @31fe7618-31fe7902` (funcmap Buff 7c said "not ported"). Engine, so ported (flag `_COMMON_CANCEL`).
- **The clamp (not in the kickoff, not inert).** After the keys are read, `320506f0 @320568d3-32056a1d` and `CommonStateStart` `32057b80 @3205fa38-3205fb5f` raise every cancel-after float to `max( v, 0.1 )` (`fcom` of 0.1 against v, `test ah,0x41; jne keep`; NaN kept). Eight floats: ATTACK_EX, ATTACK, SKILL, BWALK, WALK, DASH, UPDOWN, SKILL_CANCEL_AFTER_FLY. A March script that writes 0 or a negative value (March's `>= 0` guards mean "-1 = off") now cancels at 0.1 s. Own flag so the user can drop it; `_DEBUG` logs each value it changes.
- **June has two member sets.** Future (`+0x8c88..+0x8ca8`, read by `32031e60` and the class code) and Now (`+0x8d24..+0x8d40`, written by `CommonStateStart`). March has one, written by both parses; kept. UPDOWN is read in both June parses (Now `+0x8d3c`); `USE_COMMON` only in the Future one.
- **`3202f460( 1 )`** = `RidingPetSpecialAttackEventProcess`; March's default argument is `ASUT_GROUND` (= 1), so the port calls it as March's macro does (Stage 17 owns June's rewrite).
- **New June-only block** after the clamp (`@32056a15`): `USEFUL_VALUE<n>` float table into `+0x8ef4..+0x8f2c`, read by class code and 3 engine functions. Unowned -> new row **C9h-4f** (kickoff written).

**What landed**
- `m_fUpDownCancelAfter` (9999.f, ctor `3203e850 @32042df9`), `m_bJuneUseCommonCancelAfter`, `SetUpDownCancelAfter` (for C9h-4e), both parses.
- `RidingAttackZEventProcess`: the UPDOWN `else if` after WALK (up or down pressed, riding pet anim time above the member) -> `USI_RIDING_WAIT`. Everything else in `32031e60` was March's already.
- `DoFrameMoveFuture`: pet object exists, ONGROUND check false, pet `+0x620` set -> `RidingAttackZEventProcess()` in any state, in place of the state's OnEventProcess (replaces C9h-3b's fall-through).
- `CommonEventProcess` block, before everything else, when the byte is set: `IF_SKILL_CANCEL_AFTER` (June's own expansion: skill needs ground, then the awakening branch), BWALK / WALK -> `USI_WAIT` (June state 13), DASH -> `StateChangeDashIfPossible` (vslot `0x668`), ATTACK Z / X -> `GetComboZStateID` / `GetComboXStateID` (vslots `0x4b0` / `0x4b4`); each also needs `IsOnSomethingFuture()` (`+0x7e1c`). The first hit skips the rest of the block only.
- `JuneClampCancelAfters( stateId, bFuture )`, called after the last read in both parses.
- Not ported: June's second-skin clock `+0x8e84` (checklist 7 line 224), class UPDOWN readers (checklist 7h), the riding pet's own write of the key (C9h-4e).

**Hygiene** (`flagcheck JUNE_UPDOWN_CANCEL --with ..._UPDOWN_CANCEL_DEBUG _COMMON_CANCEL _CANCEL_CLAMP _CANCEL_CLAMP_DEBUG --worktree --files ...`):
```
files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,376,128 B, dated 2026-10-08 11:45:51, md5 8875aac0449cb6d513070fc3bc30a675, equal to X2/US_SERVICE/x2.exe (2026-10-08 11:45:46); built at mods/june-buff-port 0220a38 + 3 uncommitted code file(s).` Both `JUNE_FX UPDOWN_CANCEL` and `JUNE_FX CANCEL_CLAMP` are in the deployed exe.

**How to see it in game (play-test hand-off is in the reply):** inert on March data except the clamp. `JUNE_FX UPDOWN_CANCEL attack_z ...` prints the four cancel-after values per RIDING_ATTACK_Z entry; `JUNE_FX CANCEL_CLAMP ...` prints one line per value the clamp changed (state id, key, old value), so one session shows whether March data uses values below 0.1.

**Decided:** the clamp (`SERV_IRUHADEV_JUNE_CANCEL_CLAMP`) is kept (user, 2026-10-08: it follows June). Play-test: 17 riding `attack_z` lines and a 30 s Lord Knight field run, 0 clamp lines in both.

### C9h-4c - Review (2026-10-08, Opus 5.5 high): no transcription finding; `_DEBUG` flags off; the clamp's March reach measured (35 states, all identical in June)

**Checked against June (Ghidra / capstone, read-only).**
- **The clamp is `max( v, 0.1 )`, NaN kept, eight floats in each parse.**
  - The first float: `fld v; fld qword 0.1; fcom st(1)`, then `test ah,0x41; jne keep`. Kept when 0.1 <= v or unordered.
  - The other seven: `fcomp st(2)` against the double, then `test ah,5; jp keep` (PF set when C0 is clear, or when C0 and C2 are both set).
  - The last float, FLY (`[ebx]` = `+0x8ca8` / `+0x8d40`): `jnp` keeps the 0.1. The stored 0.1 is the float `334395e0`.
  - Future set: `+0x8c90, 8c94, 8c98, 8c9c, 8c8c, 8c88, 8ca4, 8ca8`. Now set: `+0x8d2c, 8d30, 8d34, 8d38, 8d28, 8d24, 8d3c, 8d40`.
  - Comparing a float against the double 0.1 gives the same result as the port's `v < 0.1f`: 0.1f is the nearest float to 0.1, and the next float down is below 0.1.
- **The Now set** (`dispsweep 8d24..8d40`). Outside the parse and the ctor, there are two float readers:
  - (a) `31ff92d0` (THROW_ITEM FrameMove). It copies Now WALK / DASH (`+0x8d34` / `+0x8d38`) into the throw start velocity `+0x8d6c` / `+0x8d70`. March does the same with its single member (`X2GUUser.cpp:26885`, `m_vThrowStartVelocity = { m_fWalkCancelAfter, m_fDashCancelAfter }`). Every THROW_ITEM state, in March and in June, uses WALK 1200..1700 and DASH 60..350, so the clamp never touches the velocity.
  - (b) Elsword class code (`32d13940`, `32d18490`, `32d1e200..32d1ecc0`), reading Now SKILL / DASH. That is checklist 7h class code, and March's class code reads the same single member.
  - The other hits are other classes (NPC `327b53b0` and the rest), or byte members at the same offset in other objects.
  - So nothing makes March's one-member design differ from June's two sets.
- **`32031e60`.** The anim end (`+0x2ac`) goes to 0x4c. Otherwise GetRidingOn (vslot 0x200) runs the chain, and its false branch goes to 0x4a. That is March's if / else-if order.
  - The chain: SKILL `+0x8c90` -> `3202f460( 1 )`; DASH `+0x8c9c` (`+0x86e0 / 86dc`) -> vslot 0x668; WALK `+0x8c98` (`+0x86de / 86da`); UPDOWN `+0x8ca4` (`+0x86e6 / 86e2`). WALK and UPDOWN share the 0x4c exit with the anim end, so `USI_RIDING_WAIT` is correct whatever the numbering. Then vslot 0x7d4.
  - Each test is `t >= 0` (`NAN || (0<t)==(t==0)` skips) and the pet anim's `+0x214` > t.
  - March's InputData (`X2GameUnit.h:170`) puts pureUp at `+0x86e2` and pureDown at `+0x86e6`, so the input bytes match.
- **`3202f460`'s argument.** Templet condition 1 requires `param_2 == 1`; condition 2 always passes. `param_2 == 2` picks state 0x59, otherwise 0x58. So 1 = ground = March `ASUT_GROUND` (`ASUT_NONE` 0, `GROUND` 1, `AIR` 2). March's default argument matches.
- **The `DoFrameMoveFuture` arm** (`@3203484e-32034877`). The order is: `+0x683c` non-null, then `32d3cff0`; true skips the event process. Otherwise, `+0x620` set -> `32031e60`; otherwise the state's delegate runs (`+0x9550`). The port matches.
- **The `CommonEventProcess` block** (`@31fe7618-31fe7902`).
  - Every branch tests `t >= 0` with `fldz; fcom(p) t; test ah,0x41; jp`, then `time > t` with `fcompp; test ah,5; jp`, then ground `+0x7e1c == 1`. Only the awakening branch skips the ground test, which matches the macro.
  - SKILL: vslot `0x7f4( 1 )`; `== 1` ends the block.
  - Awakening: `322731a0` false, `+0x8723`, HyperModeCount > 0, vslot 0x560, `32070cf0` time, `32070d60` false. Then `+0x8d4c = 1` and vslot 0x6b8 -> StateChange.
  - BWALK: pureLeft && bIsRight `+0x7e56`, or pureRight && !bIsRight. BWALK and WALK both go to `StateChange( 0xd, 0 )`; March's `USI_WAIT` = GUSI_END (5) + 8 = 13.
  - DASH: vslot 0x668. Z / X: the same `+0x8c8c`, pureZ `+0x86ea` / pureX `+0x86ee`, vslots 0x4b0 / 0x4b4.
  - Every exit lands on `31fe7902`, the `+0x8e78` death gate that the port has right after the block.

**The clamp's reach on March data (new, not a defect).** March's data036 class scripts, decompiled with `ljd_fixed.py`, carry 843 cancel-after assignments.
- None is negative. 35, in 7 classes, are below 0.1 (0, 0.001 or 0.033): Add, Ara, Chung, Elesis and Eve combos and modes, and Raven's RBM / RVC / Weapon Taker combos.
- **All 35 carry the same value in June's scripts**, and June clamps them too. In those states the cancel opens at 0.1 s, as it does in June. The kept clamp is therefore parity, not drift.
- The play-tests logged 0 clamp lines because none of these states was entered. Elsword has none.
- Only the 9 class scripts carry the keys (a `grep -l CANCEL_AFTER` over `kom_contents/data036_kom`).
- Some class code uses the members as something other than a time:
  - Lire: `nowSpeed.x +=` WALK (`X2GULire_ElvenRanger.cpp:13169`), and `fReloadAccel =` SKILL (`:13291`, `:13411`, `:13452`);
  - Elsword: EventTimer( DASH ), and the HarshChaser / PiercingSword range formulas on SKILL.
  - Both classes have no value below 0.1, so the clamp does not reach any of these uses.
- Table: `june2017_re/c9h4c_cancel_clamp_census.tsv`.

**Changes:** `SERV_IRUHADEV_JUNE_UPDOWN_CANCEL_DEBUG` and `SERV_IRUHADEV_JUNE_CANCEL_CLAMP_DEBUG` are commented out in `Always.h` (bytepatch; encoding and EOL unchanged). No code change.

**Hygiene** (`flagcheck JUNE_UPDOWN_CANCEL --with _UPDOWN_CANCEL_DEBUG _COMMON_CANCEL _CANCEL_CLAMP _CANCEL_CLAMP_DEBUG --range 6310cab^..HEAD`):
```
files: 0 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
The OTHER row is `87439b3`, C9h-4b's `_DEBUG` lines in `Always.h`. The worktree run on this review's edit is PASS with 2 deleted lines: the two `#define ..._DEBUG` lines, now commented out.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,372,032 B, dated 2026-10-08 13:10:00, md5 5f0e337202f98351309359a3771dbdac, equal to X2/US_SERVICE/x2.exe (2026-10-08 13:09:55); built at mods/june-buff-port 15bc580 + 7 uncommitted code file(s).`
- The 7 files are this review's `Always.h` lines plus a peer's in-progress C9h-4d: `SERV_IRUHADEV_JUNE_EFFECT_SET_DELETE` + `_DEBUG`, in X2GUUser, X2GameUnit, X2GUNPC and X2StateDungeonGame.
- A scan of the exe: `JUNE_FX UPDOWN_CANCEL` and `JUNE_FX CANCEL_CLAMP` are absent, in both UTF-16 and narrow form. `UPDOWN_CANCEL_AFTER` and `USE_COMMON_CANCEL_AFTER` are present.

**Next kickoff:** `C9h-4d.md` was checked; nothing here changes it. `C9h-4f.md` already places `USEFUL_VALUE` after the clamp.

### C9h-6 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): the snapshot exists because June's running state data are pointers; March copies them by value, so a port would never change anything -> no port, routing only

There are 23 funcmap rows tagged `Effect C9h-6`. The sweep of `[reg + 0x9538..0x956c]` found 69 hits, a few of them `mov reg, imm` decode artefacts.

**1. The members.** The snapshot is five delegate pairs (8 bytes each), using March's `UserUnitStateData` layout:
- **Future side** (from the `+0x7cb0` pointer, cleared together): `+0x9544` OnFrameMoveFuture (`+0x2c`), `+0x954c` OnEventProcess (`+0x44`), `+0x9554` StateEndFuture (`+0x4c`).
- **Now side** (from `+0x7cbc`, cleared together): `+0x955c` OnFrameMove (`+0x34`), `+0x9564` StateEnd (`+0x54`).
- The ctor zeroes all five: `3203e850 @32041ff6`, through `32073d20`.
- **Setter:** `31fd2af0` is the only writer of real values.
- **Consumers.** Each one prefers the snapshot when it is non-null, else the state's own delegate, else the Common* vslot:
  - `320347d0` DoFrameMoveFuture: `@3203480e`, `@32034879`;
  - `31fe6a40` DoStateEndStartFuture: StateEndFuture;
  - `31fd6b80` DoFrameMove: OnFrameMove;
  - `3203c3a0` DoStateEndStart: StateEnd.
- **Clears** are inline: the Future triple in `31fe6a40`, the Now pair at `3203c3a0 @3203c907`. Both run after the new state pointer is stored and before StateStart(Future).
- **Unreferenced:** `31fd2bc0` (a clear) and `31fcd030` (call-by-id 3..7) have no call, no pointer and no xref. The kickoff's "`31fe6d0a` clears all six" is wrong: that address is inside `31fe6a40` and clears only the Future triple.
- **Neighbours, not part of the snapshot:** `+0x9538/+0x953c` is an int64 unit UID from the type-0x16 list (`31f23660`, `31f09710`, `31fceba0`; unowned, not swept further). `+0x9540` (word) and `+0x956c` (byte) are separate members.

**2. The gate and the callers.**
- `31edf580` takes ECX = **`g_pData`** (`33775860`), not `g_pMain`. It calls skill tree `+0x50` -> `322074e0( now skill, slot's old skill )`, a same-skill test:
  - a direct match; or
  - the skill-tree map `+0x20` (`322aa950`, `node+0x10`); or
  - templet `+0x178` (the redirect field Phase 6b named).
- All **12** callers (not 11) are slot 493 of each class vtable (RTTI), i.e. `SetEquippedSkillFuncBySkillSlotIndex`:
  - 9 March classes: Elsword, Arme, Lire, Raven, Eve, Chung, Ara, Elesis, Add;
  - 3 June-only classes: Luciel, Rose, Ain.
- The setter runs on the slot's normal state, before that entry's `Init()`.
- Corrected starts: Add is `32a28e00` (Ghidra carves `32a288d0`). Luciel is `32b46580` (the scan's `32b43d10` is Luciel slot 489).

**3. Why June has it, and why March does not need it (item 4).**
- June's `+0x7cb0/+0x7cbc` point *into* the state vector (`+0x7cec`, 100-byte entries): `31fe6a40` stores `base + index*100`. So the class's `Init()` + rebind of a slot entry rewrites the delegates of the state that is **running**. The snapshot keeps the five delegates that run mid-state until the next state change.
- March copies by value: `X2GUUser.cpp:6585`, `:6644`, through `operator=` (`X2GUUser.h:1024`), which copies all five delegates.
- Checks:
  - no March code writes a delegate into `m_NowStateData` / `m_FutureStateData` (only `m_fPowerRate`, in class code);
  - the one mid-state `m_StateList` reader, `GetPressedSkillState` (`:37357`), reads the *pressed* slot.
- So a ported snapshot would always equal the copy, and the port is **inert by construction**.

**4. Reach.** A slot is re-equipped mid-state through `ChangeEquippedSkillState`. The callers are the skill-slot ACK (`X2SkillTree.cpp:1795`, `:1808`), the skill page (`:3976`), another user's slot change (`X2Room.cpp:1670`), and `X2GUUser.cpp:6043`. March is already safe on every one of these paths, so nothing changes in game.

**Residual June-only differences.** A snapshot port would not reproduce them; only June's pointer model would:
- (a) a snapshotted *null* delegate falls through to the new entry's delegate;
- (b) the running state then reports the new entry's skill id, hyper byte, `+0x11` byte and OnCameraMove;
- (c) a second re-equip of the same slot in the same state passes the gate on the *new* skill and overwrites the snapshot.

March keeps the old values in all three cases, which is the behaviour June's snapshot is aiming for.

**Routing (no port kickoff, no new STATUS row):**
- **Class side -> checklist 7i** (D8): drop the `31fd2af0` call when porting each `SetEquippedSkillFuncBySkillSlotIndex`, and settle state byte `+0x11` there.
- **Engine side -> backlog Stage 3, new sub-step 3f**: decide whether to adopt June's pointer model for the running state data. Only that reproduces (a)-(c); if adopted, the snapshot is ported with it.
- No JUNE_BUGS row: nothing here is ported.

### C9h-4d - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_EFFECT_SET_DELETE` + `_DEBUG` (on for the play-test); log `JUNE_FX EFFECT_SET_DELETE`; play-test run, logs read: new exe, 0 EXCEPTION, 0 phase lines; data036 copied)

June's three effect-set delete conditions, the hyper check and the played-handle list are ported. Files: `X2GameUnit.h/.cpp`, `X2GUUser.h/.cpp`, `X2GUNPC.cpp`, `X2StateDungeonGame.cpp`, `Always.h`, `data036` `Enum.lua`. Ghidra read-only; 8 funcmap rows `Effect C9h-4d`; JUNE_BUGS 123-124; key matrix: the two `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` rows flipped from "deferred".

**Correction to C9h-4 / C9h-4a / C9h-4b.** `3228a120` is **not** March's `DeleteEffectSetOnStateEnd`; that is `3228a200` (walks `+0x3ec0`, stops each handle).
- `3228a120` walks `+0x3e90`, the handle of every played effect set, and calls `321da070` on each live instance. `321da070` marks every effect of the instance as created when the templet has `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` (`EffectSetData+0x50`).
- That makes `+0x3e90` the runtime of the key that the effect rebuild's Phase 4a parsed and deferred ("the writer is not located"). The writer is `3227a570`, ported here.
- The "( = DeleteEffectSetOnStateEnd )" comments in the `JUNE_UNIT_SOUND` and `JUNE_NPC_SOUND` blocks (`X2GUUser.cpp:20974`, `X2GUNPC.cpp:24038`) are stale. They are C9h-4a / 4b's code, so they were not edited. June's order is `3228a200`, sound stop, `3228a120`; March's sound stop sits before `DeleteEffectSetOnStateEnd()`. The lists are disjoint, so the order does not matter.

**What landed (all behind the flag, `#else` untouched).**
- **Enum:** `DCES_SKILL_STATE_END = 5` in `CX2GameUnit` and in `Enum.lua` (June's line and position, after `DCES_CUSTOM_STATE`).
- **Parse** (`CX2GUUser::LoadEffectSetFromScript`, the tail):
  - three bytes `DELETE_EFFECT_SET_ON_STAGE_END`, `_OFF_RIDING`, `_ON_SKILL_STATE_END`;
  - `SET_HYPER_EFFECT_SET_CHECK_HYPER_STATE`, read into the hyper flag when `m_NowStateData.m_bHyperState` and `HYPER_EFFECT_SET` is false.
  - The key name was decoded from June's XOR buffer. `+0x10` of the now-state pointer `+0x7cbc` is `m_bHyperState` (`GetNowStateSkillID` `3208e410` reads `+0xc` of the same pointer).
- **Play site** (`CommonFrameMove_EffectSet`): three new lists, STAGE_END, OFF_RIDING, `{ handle, skill id }` pairs when the id is above 0.
- **`AddEffectSetDeleteCondition`:** every handle is pushed once per condition into the played list before the switch (JUNE_BUGS 123); case 5 is user-only, id above 0.
- **Stoppers:**
  - STAGE_END at the top of `DungeonStageLoadNot`, for each user unit (June's only caller);
  - OFF_RIDING after `RemoveRidingPet` in `ProcessRidingOn`'s off branch;
  - SKILL_STATE_END at the end of `CommonStateStart`, just before C9h-4a's `StopSoundOnSkillStateEnd`.
- **`JuneStopCreateNewEffectOnStateEnd`** right after `DeleteEffectSetOnStateEnd()` in the user's **and the NPC's** `CommonStateEnd` (the NPC play site feeds the same list; without its consumer the list would grow forever).
- **Not ported:** the `+0x93da = 0` store after `3228a3b0` (not an effect-set member; the review checks it). The CommonStateStart clears are as June: it clears only `+0x3eb0` / `+0x3ec0` (disassembled `32057b80 @3205ad95`), so the new lists survive state changes.
- **June queues the stops (`321dafb0`), March stops at once**, as every earlier port. `3228a490` keeps June's guards (game, effect set, user).

**Reach on March data: inert.** The three delete keys and the hyper-check key are June-only (C9h-4's census). `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`: a grep of `EffectSet*.src.lua` under `kom_contents` found nothing, but that file-name pattern was not checked against the real names, so March data is **not proven** free of it; the review re-checks. The only thing that runs is the played-handle push and its drain at every state end (a `resize( 0 )`). The `_DEBUG` lines print only when something is stopped or marked, so a March play-test prints none. A June effect set with the key (after the asset port) is the live path: `stop_create` lines.

**Hygiene** (`flagcheck JUNE_EFFECT_SET_DELETE --with ..._DEBUG --worktree --files <7 code files>`): files: 4 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (Always.h: the new block's comment and define), 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 `#else`/`#endif` without the token, 0 encoding change; RESULT: PASS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,374,592 B, dated 2026-10-08 13:19:24, md5 779e41742c4649b0ea5f7b54fce70e96, equal to X2/US_SERVICE/x2.exe (2026-10-08 13:19:19)`. The strings `JUNE_FX EFFECT_SET_DELETE %s n=%d extra=%d unit=%d` and the four keys are in the exe.

**Data:** `Enum.src.lua` got `DCES_SKILL_STATE_END = 5`; `kom.py compile data036` = `COMPILE OK`; `kom.py pack data036` = `PACK OK data036 ... 192 entries, 10655716 bytes ... ~1 changed: Enum.lua`. **Copied 2026-10-08 13:21** (`COPY OK`, md5 `cb5625f6`; backup `kom_backup_tools/data036.kom.20261008_132140`). **Play-test (Lord Knight, jtLordKnight): `logs.py` = run after deploy, 0 EXCEPTION, 0 `JUNE_FX EFFECT_SET_DELETE` lines, as expected on March data** (2 unrelated `E_DISABLE_HB_CHECK_REQ` UNHANDLED lines, not this phase's).

**How to see it in game.** Nothing visible changes on March data. Regression play-test: a dungeon stage change with a skill effect set running, a riding-pet summon / unsummon, a few skills and state changes; expect no change, 0 EXCEPTION, and no `JUNE_FX EFFECT_SET_DELETE` line.

### C9h-4d - Review (2026-10-08, Opus 5.5 high): four transcription findings fixed under the same flag; the port is **live on March data** now; `_DEBUG` kept on for one play-test

**Confirmed in Ghidra / capstone (read-only):**
- `3228a120` / `3228a200` / `321da070` as the Done note says. `EffectSetInstance` ctor `321df230`: `+0x80` = `m_pEffectSetData`; the next `vector<bool>` has its words at `+0x84` and `_Mysize` at `+0x94` (`+0x90` allocator) = `m_vecEffectInstanceCreated`, March's next member.
- `3227a570`: jump table `3227a650` in case order. 1..4 -> `+0x3eb0` / `+0x3ec0` / `+0x3ed0` / `+0x3ef0` (March's four lists). The `+0x3e90` push comes before the switch, once per condition. Case 5: `+0x264 == 0`, `jle` on vslot `0x524`.
- Parse tail: `xordecrypt.py 0x32024ff0`, 14 buffers ok. Store order `+0x8624` STAGE_END, `+0x8625` STATE_END, `+0x8626` DAMAGE_REACT, `+0x8627` DIE, `+0x8628` OFF_RIDING, `+0x8629` CUSTOM, `+0x862a` CUSTOM..DAMAGE_REACT, `+0x862b` SKILL_STATE_END.
- Hyper check `@32027e17`: now-state pointer `+0x7cbc` -> `+0x10 == 1` and `[+0x862c] == 0`. March's `DoStateEndStart` copies the state into `m_NowStateData` (`X2GUUser.cpp:6656`) before `StateStart` -> the parse, so the port reads the new state, as June does.
- Stoppers (raw `E8`/`E9` scan of `.text`, no absolute refs): `3228a190` only `@324d966b`, `3228a3b0` only `@3204d330`, `3228a490` only `@320606a2`, `3228a120` `@32015803` / `@3277f857`. `3228a490` = the port (guards, keep-if-same-id, stop + erase).
- `+0x93da`: a June-only user byte copied from `RidingPetTemplet +0x15c` at riding-on (`@3204d232`) and cleared at riding-off. March's `SetRidingOn` does not cover it. Not this phase's -> backlog Stage 17 (readers in the funcmap row).
- Reach of `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`: March `kom_contents/data036_kom/EffectSet.src.lua` 0 (all `*.lua` under `kom_contents` 0). June `june_src_fixed/data036_kom/EffectSet.src.lua` 2: `EffectSet_DashJumpComboXAutoGun_Last`, `..._SilverBullet_Last` (Chung).
- Data: live `data036.kom` md5 `cb5625f6` (= the copy). vs `kom_backup_tools/data036.kom.20261008_132140`: 192 = 192 entries, changed only `Enum.lua`; `DCES_SKILL_STATE_END = 5` after `DCES_CUSTOM_STATE = 4`.
- Handles carry a generation stamp (`ComposeHandle( wIndex, m_wStamp )`, `X2EffectSet.cpp:3247`), so stale handles in the lists stop nothing.

**Findings (all fixed under `SERV_IRUHADEV_JUNE_EFFECT_SET_DELETE`).**
1. **STAGE_END defaults to 1.** Parse miss `@32026b04: mov byte [edi], 1` (every other key stores 0) and ctor `@32042300` (word 1 at `+0x8624`). Ctor and parse now default to `true`. Every effect set a user plays is stopped at the next dungeon stage load unless its state says `DELETE_EFFECT_SET_ON_STAGE_END = FALSE`. JUNE_BUGS 124 is now live (the list grows outside dungeons).
2. **`3228a270` (= `DeleteEffectSetOnDamageReact`) has a second block, not ported.** After the DAMAGE_REACT stops:
   - the conditions are user, `GetNowStateSkillID() != 0`, and vslot `0x414( false ) < 0.5`;
   - it stops every live `+0x3e90` handle and does not clear the list;
   - vslot `0x414` `3208e240` reads `+0x8088` (Now) or `+0x7de8` (Future); vslot `0x124` = `GetUnitCondition` returns `+0x8084`, so this is `fStateTime`.

   New `JuneStopPlayedEffectSetOnEarlyDamageReact`, called at the end of March's `DeleteEffectSetOnDamageReact` (all three callers; the NPC is skipped inside).
3. **The user DamageReact guard** (`@3204a6f7-3204a714`) also skips reActResult 0 / 0xb / 0xc for the whole effect-set half (`3228a270`, `32276ce0`, `3228a420`). C9h-4a applied it to the sound stop only and routed the rest here (funcmap row 2000). Now three `&&` lines in March's `if` (`X2GUUser.cpp:10618`).
4. **The play site leaves the function** when SKILL_STATE_END is set and the skill id is <= 0 (`@32007837 jle` -> epilogue `320078f1`): no `AddEffectSetDeleteCondition`, no erase, so the entry replays every frame. Ported as `return;`, JUNE_BUGS 126 (June-only key; inert on March data).
- The `_DEBUG` stop count now counts live instances (with STAGE_END on, the list is mostly ended handles). Comment fixes: call addresses `@32015803` / `@3277f857`, and C9h-4a / 4b's "( = DeleteEffectSetOnStateEnd )" now sits on `3228a200`.
- Routed: class code that pushes `+0x3f00` directly or calls `3227a570` -> checklist 7h (new row); the riding pet's `+0x8628` / `+0x862b` writes (`32d2f560 @32d34d43` / `@32d35023`) are already in `C9h-4e.md`.

**Hygiene.** `flagcheck JUNE_EFFECT_SET_DELETE --with ..._DEBUG --worktree --files <4 code files>`: 2 PASS, 2 OTHER (comment-only fixes inside C9h-4a / 4b's `JUNE_UNIT_SOUND` / `JUNE_NPC_SOUND` blocks), 0 FAIL; 2 deleted lines = C9h-4d's own flagged lines replaced (vs the pre-C9h-4d base: insert-only, see the range run in the commit message), 0 non-ASCII, 0 EOL mismatch, 0 encoding change; RESULT: PASS.

**Deploy.** My own `build.py --deploy` failed in a peer's then-uncommitted C6c (`X2EffectSet.cpp(3722)`), so the commit `0b33379` holds only this review's hunks (shared files staged via `update-index`). **Deployed by the peer's build:** `last_deploy.json` = X2_offline.exe 19,387,392 B, 2026-10-08 15:55:05, md5 `944238b6875156658a9df2050292872d`, head `51d1718` (contains `0b33379`; also C6c and C3, plus the uncommitted `KTDXDeviceManager.cpp`). The strings `damage_react_all`, `stage_end`, `skill_state_end`, `stop_create` and the `JUNE_FX EFFECT_SET_DELETE` format are in the deployed exe.

**Play-test owed** (`_DEBUG` still on; turn it off after the logs), character `jtLordKnight` (Lord Knight, unit 25), no console: (1) a multi-stage dungeon, a skill right before the next stage loads -> `JUNE_FX EFFECT_SET_DELETE stage_end n=.. unit=25`; (2) get hit within 0.5 s of starting a skill without super armor -> its effects cut, `damage_react_all n=.. extra=<skill id>`; (3) a riding-pet summon / unsummon and a few skills as before, 0 EXCEPTION.

**Play-test 2 (2026-10-08 16:30-16:37, exe `40b97943`, `_DEBUG` on; user: all checks passed; logs read): (1) and (3) confirmed, (2) has no log evidence.**
- (1) One `stage_end` line per stage load for every unit that played: `jtLordKnight` (unit 25) `n=0 extra=0` at 16:31:14 and `n=0 extra=2` at 16:32:28; `AddTest` (37) and `reyaa` (12) `n=0 extra=0`. The list fills (Endurance pressed at the exit), but no set was still live at a stage load, so a live stop was not observed.
- (2) **0 `damage_react_check` lines.** That line prints on every DamageReact a user takes in a state with a skill id != 0, before the 0.5 s test, so no hit inside a skill state reached the effect-set half (Endurance = `SI_A_ES_ENDURANCE` 1203, key Q, no SUPER_ARMOR / INVINCIBLE in `ESSI_SI_A_ES_ENDURANCE`, set at time 0). Either the hits landed outside the state, the reaction was 0 / 0xb / 0xc, or the state carries skill id 0. The user reported the glow cut on a hit; the log cannot confirm it. For the review to settle.
- (3) `reyaa` summon / unsummon in village and dungeon: no `off_riding` line (expected: no March pet uses OFF_RIDING), no lingering effect.
- 0 EXCEPTION, clean exit, no crash bundle; 4 UNHANDLED = `E_DISABLE_HB_CHECK_REQ` (unrelated).
- `_DEBUG` is now off in `Always.h` (committed with these results; the deployed exe `40b97943` still has it on, the next build drops it). A retest of (2) needs it back on.
- **User decision 2026-10-08: accept as is** (no retest of (2); the in-game observation stands). C9h-4d is closed; C9h-4e is unblocked.

### C9h-4b - Done (2026-10-08, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_NPC_SOUND` + `SERV_IRUHADEV_JUNE_SQUARE_SOUND`, each + `_DEBUG` on for the play-test; logs `JUNE_FX NPC_SOUND`, `JUNE_FX SQUARE_SOUND`; play-test PASSED, logs read; `_DEBUG` off)

Files: `X2GUNPC.h/.cpp`, `X2SquareUnit.h/.cpp`, `X2EventScene.cpp`, `Always.h`. 12 funcmap rows `Effect C9h-4b`, JUNE_BUGS 119-121 (+ row 116's "Reached?" updated).

**What landed**
- **NPC** (`_NPC_SOUND`, needs C9h-4a's `_UNIT_SOUND`):
  - the nation reader in LoadStateData (default set and the SOUND_SET tables; SOUND_PLAY_LOOP keeps the plain one);
  - `SOUND_PLAY_ALL_OPTION`, appended to set 0 after the SOUND_SET tables;
  - two June state flags, appended before `FLAG_MAX` (`STOP_SOUND_AT_STATE_END` = June bit 0x4e, `CUSTOM_STATE_DELETE_SOUND` = bit 0x13), their key names decoded from the XOR buffers;
  - the TimedEvent player: `PlaySound( name, 3D, LOOP )`, the attach (skill id 0), `SetMax3DDistance`, then the push into a new member list;
  - the stop calls: DamageReact (June's NPC guard is March's), CommonStateEnd, CommonStateStart on `CUSTOM_STATE_DELETE_SOUND`.
- **The `+0x8f74` question, answered:** it is **not** March's `m_vecLoopSound`. The SOUND_PLAY_LOOP loop under the main loop pushes into `+0x8f5c` (= March's list); `+0x8f74` is a June-only played-sound list, pushed by every played record (NULL handles too), stopped + cleared in ReInit `3277c2d0` and the dtor `3279fb60`, and in CommonStateEnd `3277ee10` stopped only under `STOP_SOUND_AT_STATE_END` but cleared at every state end. `3275cf80` (stop + clear) has no caller. `offsetprobe` reports March's layout only, so the call was made from the loop structure and a dispsweep of every `+0x8f74` access. A dying NPC's DCS_DIE sound is therefore stopped at the dtor only if it started in the state current at destruction; an earlier state's sound has left the list unstopped (JUNE_BUGS 116 updated, 120). Ported as `m_vecJunePlayedSound` + `JuneStopPlayedSounds`.
- **Event scene** (`_NPC_SOUND`): the nation reader on its SOUND_PLAY loop, the only change.
- **Square unit** (`_SQUARE_SOUND`): the reader; June's player `323f17b0` as `JunePlaySquareSoundRecords` (own anim, or the riding pet's while riding and the pet has an anim; record LOOP and ENABLE_3D_SOUND passed through; a handle whose record lists `DCS_STATE_END` goes into a new list); the stopper `JuneStopSoundOnStateEnd` after `RemoveRidingPet` in ProcessRidingOn's off branch. The dtor needs nothing (the vector frees itself, as June's dtor only frees).

**Corrections to C9h-4:** the default SOUND_PLAY records go to StateData `+0x204`, not `+0x2a4` (that is `m_vecSetSoundPlayLoopValue`); the two "read sites" per reader are one rotated loop; the NPC reads `CUSTOM_STATE_DELETE_SOUND` (once, into a state flag).

**Not ported:** June's device `PlaySound` 4th argument (1; acts only on a freshly opened device, `31cdd4e0` registers a callback), KTDXLIB's signature is unchanged (**corrected by the review:** it is June's sound auto-release, which can free a device whose handle the square still holds; leaving it out is the safe side -> C9h-7). The riding pet's call of the square stopper (C9h-4e). The `CX2Pet` `SOUND_PLAY_ALL_OPTION` reader (Stage 17).

**Behaviour on March data (the play-test is a regression pass, not "as before")**
- **NPC: not inert.** Every March monster sound is pushed; the sounds of the state current at the monster's dtor / ReInit are `Stop()`ped (the list is cleared at every state end; JUNE_BUGS 119). Sound devices are shared per file name, so a removed monster can cut the same sound on another unit. That is June's behaviour; expect it in dungeons with many same-type mobs.
- Attach lists stay `DCS_NONE`; the per-condition stoppers find nothing. The two new state flags are never set (no March script has the keys).
- Square: no March `SQUARE_*.lua` carries `SOUND_PLAY` (5 files checked, bytecode scan), so the player has no records; only the empty stopper runs at every dismount. The 3D default would change a square sound from non-3D to 3D, once a script has one.

**Hygiene** (`flagcheck JUNE_NPC_SOUND --with ..._DEBUG --range HEAD^..HEAD` on `7ac67a6`, same for `JUNE_SQUARE_SOUND`):
```
NPC:    files: 3 PASS, 0 PASS~, 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR
        in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)
        RESULT: PASS
SQUARE: files: 2 PASS, 0 PASS~, 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR
        in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)
        RESULT: PASS
```
(The OTHER rows are the other flag's files; the 4 / 2 tokenless lines are the `#else`s. `Always.h` was staged with only this phase's two blocks: a peer's uncommitted blocks stayed out of the commit.)

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,374,592 B, dated 2026-10-08 11:35:50, md5 a88b65111c050b4ff189cb037a7db6bf, equal to X2/US_SERVICE/x2.exe (2026-10-08 11:35:46); built at mods/june-buff-port bcb3b6a + 8 uncommitted code file(s).` Another session's `build.py --deploy` ran first and linked my edits in (my run reported "nothing needed relinking"); the exe also carries that session's uncommitted C9h-4c work. The strings `JUNE_FX NPC_SOUND`, `JUNE_FX SQUARE_SOUND`, `STOP_SOUND_AT_STATE_END` and `CUSTOM_STATE_DELETE_SOUND` are in it.

**Play-test** (character `reyaa`, user 12, level 80, three riding pets; no console commands needed):
1. **Dungeon.** Enter any dungeon, kill five or more monsters, change stage once. Sounds as before, except a sound may cut when a corpse disappears. No crash on stage change or on leaving.
2. **Village.** Summon a riding pet, ride, dismount. Village sounds as before.
3. Any dungeon intro scene plays as before.
Then say "check logs"; I read `JUNE_FX NPC_SOUND` (play / stop / clear lines, capped at 400) and `JUNE_FX SQUARE_SOUND` (one `stop state_end n=0` per dismount). Then turn both `_DEBUG` flags off.

**Logs (2026-10-08 12:45-12:49, exe 8875aac0 = this code + peers' work):** 0 EXCEPTION; NPC `play ... n=1`, `clear state_end n=1..2` and one `stop dtor n=1` on offline monster units 1000001-4; SQUARE `stop state_end n=0` at each of 6 dismounts (units 12, 18); no crash on stage change. The only UNHANDLED is the known `E_DISABLE_HB_CHECK_REQ`. Both `_DEBUG` flags turned off afterwards (comment-only `Always.h` change).

Review: `kickoffs/june-effect/C9h-4b-review.md` (Opus 5.5 high).

### C9h-4b - Review (2026-10-08, Opus 5.5 high): no transcription finding, no code change, no deploy; the device `PlaySound` 4th argument is June's sound auto-release, not neutral -> C9h-7

**Checked against June (Ghidra / capstone, read-only). Everything matches the port.**
- **The two state keys, re-decoded by hand** (I emulated the initializer and the XOR, without the C9h-4b script):
  - `@3278d8e0-3278dab7` = `STOP_SOUND_AT_STATE_END`, set `( 0x4e )`. CommonStateEnd `@3277f9bc` tests `[flags+8] & 0x4000`, the same bit.
  - `@3278929c-32789495` = `CUSTOM_STATE_DELETE_SOUND`, set `( 0x13 )`. CommonStateStart `@327ada97` tests `[flags] & 0x80000`, the same bit.
- **`+0x8f74`** (a fresh `dispsweep 8f74 8f78 8f7c`). The NPC accesses are exactly the Done note's list: the ctors, the push, ReInit, the dtor (stop `@327a01a3`, free `@327a07a3`), CommonStateEnd, and `3275cf80` (0 callers).
  - The other hits are user-class layouts and SEH unwind funclets.
  - The SOUND_PLAY_LOOP loop pushes into `+0x8f5c`, and the die block touches only `+0x8f5c`.
- **Where each stop sits.** Every stop is in June's order relative to its neighbours (DamageReact, CommonStateEnd, CommonStateStart, ReInit, dtor; the funcmap rows give the addresses).
  - March runs its staging block earlier than June does, so the `+0x8f74` stop still comes after it.
  - June zeroes `+0x8f18` after the CUSTOM_STATE stop and the port zeroes it before. The two do not interact.
- **The NPC player:** `PlaySound( name, 3D, LOOP )`, then the attach even for a NULL handle (March's attach does the same, and the stoppers skip NULL), then SetMax3DDistance, then the push.
- **ALL_OPTION:** the 7 keys decoded, record offsets `+4 / +0 / +0x20 / +0x24 / +0x28 / +0x2c / +0x40`, defaults `"" / -1 / 100 / false / -1 / true / false`, DELETE_CONDITION erased then read, set 0 only. Identical to the user's reader.
- **The square player** `323f17b0`: the due test, the riding pet's clock (pet UID `+0x198` = `m_UnitUID`; it applies only when the pet and its anim both exist), the roll, the my-unit gate and the device arguments all match. So does the stopper's position after RemoveRidingPet (`@323f7219`).

**The finding (a design question, not a transcription error): the 4th argument is June's sound auto-release.**
- **How it works in June.** When the call has just opened the device, argument 1 registers FMOD `Channel::setCallback( 31cdcca0 )` through `31cdd4e0`.
  - At channel end, `31cdcbb0` queues the device if its count is 1 and its channel is the one that ended.
  - The sound manager's frame move `31cde8d0` then runs `31cdd8d0` on each queued device: erase it from the map, then the deleting dtor.
- **What that means for the square.** A finished square sound can be freed while `+0x5ec` still holds its handle. The riding-off `Stop()` then runs on freed memory (JUNE_BUGS 122, latent).
- **Leaving the argument out is the safe side, not a neutral one.** March has no channel callback (KTDXLIB has no `setCallback`), so the port's handles stay valid, and a device stays cached. The Done note's "Not ported" line is corrected.
- **Reach in June.** 174 of the 242 `31ce0e10` call sites pass 1, 9 pass 0, and the rest pass a register. The only other caller of `31cdd4e0` is `32800e10`.
  - The unit `PlaySound` `32276220` opens its device through `31cdf4a0` and never registers the callback.
  - So C9h-4a's unit lists and the NPC `+0x8f74` list are not exposed (JUNE_BUGS 116 / 119 unchanged).
- **Routing.** No STATUS row owned this, so it goes to **C9h-7** (planned). Whether to port it is the user's decision: copied verbatim, it exposes every March handle list filled from the device manager's `PlaySound`.

**Regression, from the C9h-4b logs.** As expected: the NPC attaches only `DCS_NONE` (`play ... n=1`); the played-sound list logs `clear state_end` / `stop dtor`; the square logs `stop state_end n=0` per dismount and no `play` line.

**Hygiene** (`flagcheck --range 7ac67a6^..HEAD --with ..._DEBUG`, one run per flag):
```
NPC:    files: 3 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR
        in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)
        RESULT: PASS
SQUARE: files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 8 OTHER (other flags), 0 FAIL, 0 ERROR
        in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)
        RESULT: PASS
```
- **OTHER rows:** the sibling flag's files; `6310cab` (C9h-4c's commit, inside the range); `Always.h`.
- **Deleted lines:** only `87439b3`'s `-2/+2`, which comment out the two `_DEBUG` defines.
- **Tokenless lines:** the `#else`s.

**What changed:** no code, so no build and no deploy. 6 funcmap rows tagged `Effect C9h-4b review` (row `323f17b0 (C9h-4b)` corrected); JUNE_BUGS 122; a STATUS row and `kickoffs/june-effect/C9h-7.md`. C9h-4e stays blocked: C9h-4d is not done.

, and the C9h-7 decision (port June's sound auto-release, or keep March's cached devices).

### C9h-4e - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS` + `_DEBUG` (on for the play-test), log `JUNE_FX RIDINGPET_USER_KEYS`; play-test pending)

Files: `X2RidingPet.h/.cpp`, `X2GUUser.h`, `X2GameUnit.h/.cpp`, `X2SquareUnit.h/.cpp`, `Always.h`; data: 14 `RidingPet_*.lua` in `data037`. Ghidra / capstone read-only; 8 funcmap rows `(C9h-4e)`; JUNE_BUGS 129-130.

**What landed (all behind the flag; C9h-3b's comment is left in place, the new code sits at June's positions)**
- **User block (game).** In June's order, from `32d2f560`'s call list and its XOR-decoded keys:
  - `DISABLE_GRAVITY_TIME_START` / `_END` after SPEED_TIME and before INVINCIBLE (both read first, miss 0, then written to the user: every pet state change resets the window);
  - the sound list: clear, `SOUND_PLAY<n>` through C9h-4a's nation reader, then `SOUND_PLAY_ALL_OPTION<n>` (`CX2GameUnit::JuneLoadSoundPlayAllOption`; keys and defaults checked in both June copies);
  - `UPDOWN_CANCEL_AFTER` (miss 9999.0) after DASH, before `LoadEffectSetFromScript`;
  - `DELETE_EFFECT_SET_OFF_RIDING` / `_ON_SKILL_STATE_END` after `_ON_DIE`. These repeat what C9h-4d's `LoadEffectSetFromScript` tail already read from the same table; June reads them twice.
- **StateChangeFuture.** After CAN_PASS_UNIT: the user's Future push / pass vectors (`+0x8b14` / `+0x8b24`, March `m_vecCanPushUnit` / `m_vecCanPassUnit`) are cleared (`32d3e2f0` / `32d3e330`, disassembled), then refilled from `CAN_PUSH_UNIT_TIME<n>` / `CAN_PASS_UNIT_TIME<n>`.
- **Village block `32d350e0`** (no game only, before C9h-3a's state UI sets): the pet's anim oneshot timer reset, the square's DCS_STATE_END stop (`323f1730`), the square's list cleared and refilled from `SOUND_PLAY_ALL_OPTION<n>` only.

**Corrections (found here)**
- **The oneshot reset is on the pet's own anim, not the square unit's** (kickoff and funcmap row `32d2f560 @32d3110d-32d35a05` said the square's): `ebp` is the pet (`[ebp+0x43c]` = owner uid) and `[ebp+0x238]` is `m_pXSkinAnim` (`32d286c0` calls ChangeAnim on it). C9h-4b's square player times its records on that anim while riding, so this is the one that matters.
- **June's village block never survives** (JUNE_BUGS 129): every village pet StateChange comes from the square's `StateChange` `323fec40` (pet first, then `StateStart` `323fad80`, which clears `+0x5dc` at `@323fdf97`) or from inside StateStart before that clear. Ported verbatim. **Pet sounds are heard only in a game**, as in March (March's village path never played them either).
- **JUNE_BUGS 130:** a pet script without CAN_PUSH / PASS tables (all of March's) empties the user's Future vectors on every StateChangeFuture. Latent: no `RIDING_*` state in June or March class scripts has the tables.
- **Reach of the user keys.** Scan of `june_src_fixed`: 35 scripts, 380 `*RIDING_*` state blocks; none carries `DISABLE_GRAVITY_TIME_*`, `CAN_PUSH/PASS_UNIT_TIME`, `UPDOWN_CANCEL_AFTER` or `SOUND_PLAY_ALL_OPTION`. So the gravity / push / pass resets hit empty values, and UPDOWN is written 9999.0 (no March class code reads it).

**The scripts (14 of March's 15 riding pets)**
- **Source and compile.** June's `RidingPet_*.src.lua` from `june_src_fixed/data037_kom/`, copied byte-for-byte, then `kom.py compile data037` (`COMPILE OK ... 'ok': 14`). Each compiled `.lua` has the same size and string set as June's original bytecode (`junekom.py extract`; header byte 0x82 -> 0x81); no hand edit.
- **Resolve check.** The keys June adds (`SOUND_PLAY_ALL_OPTION0/1`, `SOUND_NAME`, `PLAY_TIME`, `ENABLE_*_COLLISION_BOX`, `DASH_JUMP_LANDING_STATE` in the Oberon variants) are either read here or ignored by March's parser. Every dotted name and bare global the scripts use is a string constant somewhere in March's script corpus. `ENABLE_USER_COLLISION_BOX` / `ENABLE_RIDING_PET_COLLISION_BOX` have no March reader (inert).
- **Assets.** Every file name the 14 scripts reference exists in `kom_contents`, except four `.ogg` the March scripts already referenced and lacked (`RidingPet_Ice_Hameling_DashEnd`, `RidingPet_Scorpion_DashJumpLanding`, `RidingPet_Scorpion_Dash_End`, `RidingPet_Nasod_ScooterAngel_AttackZ_Shot`).
- **Visible deltas of adopting June's whole scripts** (decompiled March originals vs June, flattened and compared per key; expected June behaviour, not bugs):
  - all 14: `CAN_PUSH_UNIT` / `CAN_PASS_UNIT` are gone (defaults push true, pass false), so in `ATTACK_Z` and `DYING` monsters can now push the riding unit, and `DYING` no longer passes through them; **Scorpion's `PASS_DASH` no longer passes through monsters** (June replaced it with the collision-box keys March cannot read, backlog Stage 17);
  - all 14: `LAND_CONNECT = FALSE` is gone from the `DASH` state(s) (default true: a dash follows the ground line);
  - the START sounds move from `SOUND_PLAY` to `SOUND_PLAY_ALL_OPTION` with the same names and times; Blue / Golden / Sweetblossom Mobi swap the attack sounds for colour-specific ones (`RidingPet_Blue_*`, `_Red_*`, `_Pink_*`, all in the koms) and add Hit sounds; Ice Hameling adds `PunchAttack6.ogg` in its attacks;
  - the three Oberon 650s add a `JUMP_ATTACK_Z` projectile effect set (`EffectSet_Oberon650_*Jump_Attack_Z_Projectile`) that is defined only in June's `EffectSet.lua`: March ignores the missing name (`PlayEffectSet` returns invalid), so the projectile is absent until June's `EffectSet` is adopted;
  - a byte-substring check of every other new literal against March's Lua / txt corpus found nothing else missing.
- **`RidingPet_GodBeast_Fire` is NOT adopted (user decision pending).** June's script needs `Motion_Riding_GodBeast_Fire.x`, `Riding_GodBeast_Fire.tga` and 13 `RidingPet_Godbeast_Fire_*.ogg` (June's `data140` / `data141`, not mounted by March); March's own script is a Red Hameling reskin. Adopting it now would load a missing mesh. After the asset port stages those files (P3), copy the June `.src.lua`, `kom.py compile data037`, `pack`, `copy`. The review kickoff checks it.
- **Pack.** `PACK OK data037 ... 1915 entries, 9613937 bytes`; entry diff against the live kom = exactly the 14 files (`~14 changed`, 0 added, 0 removed). **Copy: waiting for the user's yes and for the new exe (the old exe does not read ALL_OPTION on the pet, so a copy first would silence the summon sounds).**

**Hygiene** (`flagcheck JUNE_RIDINGPET_USER_KEYS --with ..._DEBUG --worktree --files <8 code files>`): files: 7 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 1 `#else` without the token (the original sound loop), 0 encoding changes; RESULT: PASS.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,405,312 B, dated 2026-10-08 18:26:07, md5 eeabc914b8cf2deab8f2ae39720430d8, equal to X2/US_SERVICE/x2.exe (2026-10-08 18:26:02); built at mods/june-buff-port 61d480c + 11 uncommitted code file(s).` Another session's `build.py --deploy` linked my edits in first (my own X2 step failed on that session's then-uncommitted C3c code, which it fixed; my files compiled in X2Lib); the exe also carries that C3c work (now `57c5651`). The strings `JUNE_FX RIDINGPET_USER_KEYS user state` / `village` / `future` and the keys `UPDOWN_CANCEL_AFTER` / `SOUND_PLAY_ALL_OPTION` are in it.

**Play-test data.** `els_db.sql` (backup `db_backup/els_db_20261008_180843_pre_c9h4e_ridingpets.sql`): reyaa (unit 12, level 80) now owns riding pet ids 1-7 and 11-17: Nasod Mobi (1), Koucika (2), Ancient Pporu (3), Scorpion (4), Ice / Dark / Red Hameling (5 / 6 / 7), Oberon 650 (11), Ancient Pporu Naver (12), Oberon Black Skull (13) / Angel Heart (14), Bluestorm / Goldenfire / Sweetblossom Mobi (15 / 16 / 17).

**How to see it in game.** With the new exe and the new `data037.kom`: in a dungeon, summon each pet. The START state plays `<Pet>_Start.ogg` (0.01-0.4 s) and `RidingPet_Summon.ogg` (0.001 s), each once, no loop: the same sounds the old scripts played through `SOUND_PLAY`. `ErrorLog.txt` gets `JUNE_FX RIDINGPET_USER_KEYS user state=... all_option=2` per summon. In a village the pets sound as before (none), with `JUNE_FX RIDINGPET_USER_KEYS village state=... square_records=2` per pet state change.

**C9h-4e play-test (2026-10-08): check 1 passed (6 pets, `all_option=2` each); check 2 passed, plus one bug: a mount swapped without dismounting, then a character change, leaves the pet invisible until the owner moves.** Logs: those pets have a `RIDINGPET_FX loader` line and no `village state=5` line (no StateChange at creation); my village block did not run then, so it is not 4e's code. Fix `SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE` (+ `_DEBUG` on): a loaded village pet with no StateChange takes its wait state from OnFrameMove (log `JUNE_FX RIDINGPET_ENTRY_STATE`). Not proven to be the cause until retested. `DEPLOY OK: X2_offline.exe is 19,406,848 B, dated 2026-10-08 20:26:43, md5 0d6a90cdb76816011c9abd48bf4e77b9`.

### C9h-4e - Review (2026-10-08, Opus 5.5 high): no transcription finding; JUNE_BUGS 129 narrowed (a transformer-only June exception); data037 round trip proven; both `_DEBUG` flags off (committed, not deployed)

**Code, checked against June** (capstone over `32d2f560` / `32d2c8b0`, keys from `xordecrypt.py 0x32d2f560`, Ghidra xrefs):
- **Order in the user block = the port:** SPEED_TIME, gravity (`@32d31b1b` / `@32d31d40`), INVINCIBLE (`@32d31f05`), SUPER_ARMOR / _NOT_RED / _TIME, the sound list (clear `+0x85c4` `@32d326fb`, SOUND_PLAY via `32078fd0`, ALL_OPTION `@32d32827`), cameras, SKILL / BWALK / WALK / DASH, UPDOWN (`@32d3424b`), `LoadEffectSetFromScript` (`@32d343b5`), ON_STATE_END / ON_DAMAGE_REACT / ON_DIE, OFF_RIDING / ON_SKILL_STATE_END.
- **Miss defaults and stores match:**
  - gravity: both reads miss to `fldz`; both are read first, then START goes to `+0x84f8` and END to `+0x84fc`;
  - UPDOWN: miss `[3346ede8]` = 9999.0, stored to `+0x8ca4`, no clamp call;
  - OFF_RIDING: miss = `bl` after `xor bl, bl` (false), stored to `+0x8628`;
  - SKILL_STATE_END: miss = `al` 0, stored to `+0x862b`.
- **Sound loops match:**
  - SOUND_PLAY: time -1, name `L""` (`3344f1f4`), rate 100 (`ebp = edi + 0x64` with `edi` 0), only-if false.
  - ALL_OPTION, both copies (user `@32d32853-@32d330d3`, village `@32d35163-@32d359fb`) = `JuneLoadSoundPlayAllOption`: NAME `""`, PLAY_TIME -1, RATE 100, ONLY_IF false, MAX_DISTANCE -1, 3D true, LOOP false.
  - DELETE_CONDITION, in both copies: erase all (`@32d3302e` / `@32d35951`), then read from 1 until a miss.
- **Village block `@32d350e0` = the port:**
  - It is reached from the game test `@32d31114-@32d31150` only when the protected game pointer `[3372c0fc]` is unset.
  - `ebp` is still `this` (`@32d2f59b`), so `[ebp+0x238] -> +0x24c = 0` is the pet's own anim (= `ResetOneshotPerformed`).
  - The owner uid is the 8 bytes at `+0x438`, and `ecx = [33775870]` for `323a8dd0`.
  - Then `323f1730`, clear `+0x5dc`, refill. Nothing in the block uses a game.
- **Future `32d2c8b0 @32d2d52f-@32d2d763` matches:**
  - `32d3e2f0` / `32d3e330` run on the user;
  - table names are `"%s%d"` from index 0;
  - push rows `{ time miss 0, can miss 1 }` go to `+0x8b14`, pass rows `{ 0, miss 0 }` to `+0x8b24`, which are March's Future `m_vecCanPushUnit` / `m_vecCanPassUnit` (`CommonStateStartFuture`).

**JUNE_BUGS 129: the verdict stands, the claim is narrowed** (46 callers of `32d3cb10`).
- **The square's 8 are as the Done note says:** `323fec40` and 7 in `323fad80`.
- **The rest are game-only:**
  - `3249d150` / `324bd250` / `324d1de0` call the user's StateChange `32043380`, then `32071c10` (GetRidingPet);
  - `32a08150` is Add's StateStart;
  - the other ~33 are `CX2GUUser` methods, and `32d3cff0` comes from the user's StateChange `320347d0`.
- **New: a transformer-only exception.**
  - `32d3cf40` (TRANSFORM_VILLAGE / _REVERSE_VILLAGE, pet type `+0x350` 0x20 / 0x21 only) is called from the square's state request `323fef90 @323ff0c7` (state 0x18), outside the square's StateChange.
  - The request usually goes on to the square's StateChange `@323ff597`, which wipes the list again. On its early exits (`+0x794`, `323f3090` refusing, `[g+0x1f1]`, `3234e610` null) the refill survives.
  - Unreachable in March (no transformer pet, Stage 17).
  - Row 129 and funcmap row `323fec40` updated; new funcmap row `323fef90 @323ff0c7`.

**Data:**
- **Live kom = the packed one:** the live `data037.kom` (md5 `1f3126cf`) is the packed kom, extracted with elstools.
- **The 14 `RidingPet_*.lua` are June's scripts:**
  - their size and string set equal June's original (`junekom.py extract`), and the header goes 0x82 -> 0x81;
  - the bytes differ only by the opcode numbering;
  - the live bytecode decompiles to the same lines as `june_src_fixed/data037_kom/*.src.lua` (a multiset match for all 14; only the table-hash key order differs);
  - no `.src.lua` is in the kom.
- **`RidingPet_GodBeast_Fire` is still March's.** Its 15 assets are not staged (asset port P3 is `waiting (P2)`), so it stays held, and P3's kickoff carries it.

**`SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE`** (added after this kickoff was written):
- not June's code; its own flag; retest passed; flagcheck PASS (below);
- it keys on C9h-3b's first-change gate `m_bJuneFirstStateChanged`;
- whether the pre-C9h-3b exe showed the pet in this case was never tested;
- **June's client has no starter here either.** June's square riding-on `323f70a0` (on branch) calls only CreateRidingPet `321f71b0`, with no square or pet StateChange. None of the 46 callers of `32d3cb10` is the creation / load path. June relies on the square's next StateChange, as March does, so there is no June call to port instead;
- **User decision 2026-10-08: follow June.** The flag is now OFF (`#define` commented in `Always.h`, code left in place). A pet created without a StateChange (mount swapped without dismounting, then a character change) stays invisible until the owner's next state change, as in June. `DEPLOY OK: X2_offline.exe is 19,410,432 B, dated 2026-10-08 22:55:46, md5 a0344cecee3b64668b89e2b6e1db3466` (built at `997287e`, after C3d-2 landed; carries both `_DEBUG` flags off too: neither `JUNE_FX RIDINGPET_USER_KEYS` nor `JUNE_FX RIDINGPET_ENTRY_STATE` is in the exe).

**`_DEBUG` off:** `SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG` and `SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE_DEBUG` are commented out in `Always.h` (bytepatch, 2 lines). **No build, no deploy.** The tree holds the peer C3d-2's uncommitted 16-bit state id, which shifts every class state id by 3 and must reach the game together with its data036. The deployed exe (`e2c02f4f`, a peer's 21:58 deploy) still prints both tags; the next deploy carries the switch. No behaviour change, so no play-test.

**Hygiene** (`--range c0cd5fe^..HEAD`, `--files` = the 8 code files / `X2RidingPet.cpp Always.h`; `Always.h`'s other +26 lines are the peer's C3d-2 defines, not staged):
```
JUNE_RIDINGPET_USER_KEYS (+_DEBUG):    files: 7 PASS, 0 PASS~, 2 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR; 3 deleted, 0 non-ASCII, 0 EOL-mismatched, 1 #else/#endif without the token, 0 encoding changes; RESULT: PASS
JUNE_FIX_RIDINGPET_ENTRY_STATE (+_DEBUG): files: 1 PASS, 0 PASS~, 2 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR; 3 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the token, 0 encoding changes; RESULT: PASS
```
- **The 3 deleted lines are peers'** (C9h-4f's `_INJECT` removal in `8b56f68`, and its `_DEBUG` switch in `037fce2`).
- **The `#else` without the token** is the original sound loop (see the Done note).

### C9h-4f - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_USEFUL_VALUE` + `_DEBUG` and the temporary `_INJECT` (both now OFF); log `JUNE_FX USEFUL_VALUE`; play-test PASSED 2026-10-08, logs read)

Files: `X2GUUser.h/.cpp`, `Always.h`. Funcmap rows `(C9h-4f)` (8), reader census `june2017_re/c9h4f_useful_value_readers.tsv`, checklist row in 7h.

**June's block, decoded** (`320506f0 @32056a15-32056b0e`, `32057b80 @32060cd9-32060dd5`, identical shape)
- 15 floats are zeroed (5 rows x 3; the kickoff said 14): Future `+0x8ef4..+0x8f2c`, Now `+0x8eb8..+0x8ef0`.
- `BeginTable( 33478e24 = "USEFUL_VALUE", i, true )` for i = 0, 1, ..: values 1 / 2 / 3 into row i (a miss writes 0.0), `EndTable`. The first missing table ends the loop. A table that opens at i == 5 is closed unread (`jge`), so only five rows exist. This is March's own `EVENT_PROCESS_TIME` loop idiom (`BeginTable( name, index )` appends `%d`).
- **Engine readers are all June-only states, none reachable in March:** `31fc8620` = EventProcess of each class's `*_WAIT_STATE_CHANGE` (state time > Future[0][0] -> `StateChange( 0xd )`), `31fc9a20` = `USI_EVENT_SNIPER_CONTROL_FIRE` 0x70, `3200d450` and `3200d1d0` (Now) = `USI_EVENT_SNIPER_*` handlers registered by `3202b110`, `31fd0260` = bounding landcrash (C3d-3). **`3200d3f0` in the kickoff is the sweep's wrong start** (no int3 padding): it is the previous function and does not read the set.
- 109 class-code readers (37 Future, 72 Now) go to the character ports: checklist 7h row (getters named there).

**What landed**
- `m_afJuneUsefulValueFuture[5][3]` / `m_afJuneUsefulValueNow[5][3]` (ctor-zeroed: ours, June's ctor leaves them), getters `JuneGetUsefulValueFuture / Now( row, col )`.
- `JuneParseUsefulValue( table, stateId, bFuture )`, called right after the C9h-4c clamp in `CommonStateStartFuture` (Future set) and `CommonStateStart` (Now set).
- Not ported, on purpose: the handlers above (their states do not exist in March; porting a function nothing registers is dead code). They are written up in funcmap and go to **C9h-4g** (new, kickoff written), together with two June-only keys found in the same parses that no row owned: `USEFUL_STRING` (`+0x8f40` / `+0x8f30` vectors, stride 0x1c) and `CUSTOM_OFFSET_NOT_SELECT_UNIT` (`+0x8f54 / +0x8f58`).
- **C3d-3's dependency on this phase is met** (the Now set and `JuneGetUsefulValueNow` exist).
- No JUNE_BUGS row: nothing June does wrong here.

**Hygiene** (`flagcheck JUNE_USEFUL_VALUE --with JUNE_USEFUL_VALUE_DEBUG --worktree --files KTDXLIB/Always.h X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`, before commit):
```
files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,409,920 B, dated 2026-10-08 21:14:23, md5 cba1f25c47209563fa926321ba914ac2, equal to X2/US_SERVICE/x2.exe (2026-10-08 21:14:18); built at mods/june-buff-port 920eafe + 7 uncommitted code file(s).` That build also carried a peer's uncommitted C9h-8 work (`KTDXDeviceSound`, `X2DamageEffect`); the peer's own deploy 20 s later (21:14:44, md5 `561222164aeae847...`, same size) replaced it, and that exe contains this phase's code too (strings re-checked in the deployed file). Strings in the exe: `USEFUL_VALUE` and wide `JUNE_FX USEFUL_VALUE` once each.

**Play-test (3 runs, logs read).** Runs 1 and 2 used the dev console (`ESSI_WAIT.USEFUL_VALUE0 = ...`, success message) and printed nothing: **each unit runs its script in its own Lua environment** (`m_LuaManager( L, 0, true )`, script via `LoadAndDoMemory( &m_LuaManager, ... )` at `X2GameUnit.cpp:4079`), so the console's global `ESSI_WAIT` is not the unit's table. That was a wrong test spec of mine, not a port defect: a probe (run 2, `_DEBUG`) showed the helper running for unit 25 state 13, Future and Now, `rows=0`. Run 3 used a temporary `_INJECT` hook (`m_LuaManager.DoString` in the unit's own thread, once, at the first Elsword wait start): `inject done unit=25`, then on every wait start `future` and `now` `state=13 rows=2 row0=1.5,2.5,3.5` and `inject check ... row1=4,5,6` (12 wait starts, 0 EXCEPTION). So the parse reads both sets, three floats per row, two rows, and stops at the first missing table. Run 3's exe was `814c9f96`; `_DEBUG` and `_INJECT` were then set off in `Always.h` (code kept under the dead flags: the review deletes the `_INJECT` blocks and the probe).
**Final deploy (flags off):** `DEPLOY OK: X2_offline.exe is 19,408,896 B, dated 2026-10-08 21:58:30, md5 e2c02f4fbebeb5bcf818ba7c4cc99071, equal to X2/US_SERVICE/x2.exe (2026-10-08 21:58:25); built at mods/june-buff-port 559c114 + 2 uncommitted code file(s).` Wide `JUNE_FX USEFUL_VALUE` and `ESSI_WAIT.USEFUL_VALUE0` in the exe: 0 each.

**Next:** review `kickoffs/june-effect/C9h-4f-review.md` (Opus 5.5 high), then C9h-4g.

### C9h-4f - Review (2026-10-08, Opus 5.5 high): no transcription finding in the code; three funcmap reader notes corrected; dead `_INJECT` / probe code deleted; no build, no deploy

**Code, checked against June** (`disasm_range.py`, Ghidra strings):
- **Row count and shape: exact.** `320506f0 @32056a15-32056b0e` and `32057b80 @32060cd9-32060dd5` both zero 15 floats (`+0x8ef4..+0x8f2c` / `+0x8eb8..+0x8ef0`), then call `31c7b980( 33478e24, i, 1 )`. `31c7b980` = `BeginTable( name, index )` (`StringCchPrintf` with `33449104` = `"%s%d"`, as March's `KLuaManagerProxy.cpp:84`). Each loop reads values 1 / 2 / 3 into row i through `32fea820` (GetValue by integer key); a miss writes `fldz`. The loop ends at the first table that does not open. `cmp i, 5; jge` -> `EndTable` with no read. The raw bytes at `33478e24` are `USEFUL_VALUE\0`, with no trailing character. `JuneParseUsefulValue` does exactly this.
- **Call sites: correct.** Future is right after `JuneClampCancelAfters( .., true )`; Now is right after the Now clamp.
  - No table is left open across either call. Every `BeginTable` before them in `CommonStateStartFuture` / `CommonStateStart` is a self-closing `if` or `while`, and run 3 read the injected rows through both sets.
  - The one exception is June's own JUNE_BUGS 117: an empty-name `SOUND_PLAY_EX` row leaves that table open, so June then reads the Now set inside `SOUND_PLAY_EX`. That is ported verbatim and needs no new row.
- **Readers: two of four notes were wrong** (`funcmap.tsv`, notes only):
  - State names are from June's `Enum.lua` (`GUSI_END` = 5): `0xd` = `USI_WAIT`, `0x6e..0x71` = `USI_EVENT_SNIPER_INIT / _CONTROL_IDLE / _CONTROL_FIRE / _END`, `0x7a` = `USI_BOUNDING_UP`. `0x72..0x76` are not sniper states (PROTON_BOMB / MECA_DROP / GUN_DANCE), and `0x6e` is INIT, not READY (C3d-1 row corrected).
  - `31fc8620` was right: state time > Future[0][0] -> `USI_WAIT`.
  - `31fc9a20` was right: state time >= Future[0][0] -> `_CONTROL_IDLE` when signed byte `+0x9480 > 0`, else `_END`.
  - **`3200d450` had the test inverted.** It changes to `0x71` when the target `+0x947c` is null **or the accumulated `+0x9484` > Future[0][0]** (the timer ran out). Only otherwise do the key bytes run: `+0x86eb` -> `dec +0x9480`, `0x70`; `+0x86ef` -> `0x71`.
  - **`3200d1d0` was misread** (`likely` -> `confirmed`). `322e67c0` is CX2Game's CreateNPC packet fill, and `0x10f6` = 4342 = `NUI_NEIL_THE_SNIPER_EVENT` in June's Enum, so it is an **NPC id**. Vslot `0x198` is the own-unit test, not a target test. The `+0x947c` target is the Neil the Sniper NPC, sent to `NEIL_THE_SNIPER_DYING` through `3275da30`. The `Now[0][0]` distance goes through `31c5f090`; that it is the line map's `GetFixedDistancePosition` is inferred from the argument shape and marked so.
  - **`3200d3f0` is a separate function** (int3 padding before it, none between it and `3200d450`). It reads no USEFUL_VALUE, but it is the same sniper family's StateStart-shaped handler (target NPC -> `NEIL_THE_SNIPER_IDLE`, else `0x71`). It has a new funcmap row, and it is added to C9h-4g's handler list.
  - `31fd0260` (C3d-3) reads `+0x8eb8` = `JuneGetUsefulValueNow( 0, 0 )`. C3d-3's kickoff now says so, 0-based.
- **Cleanup (dead-flag deletion only).**
  - Deleted from `X2GUUser.cpp`: both `SERV_IRUHADEV_JUNE_USEFUL_VALUE_INJECT` blocks and the run-2 probe `_DEBUG` block (35 lines).
  - Deleted from `Always.h`: the `_INJECT` define and its two comment lines.
  - The `iRow > 0` `_DEBUG` line stays for when June scripts are adopted, and its `#ifdef`/`#endif` pair was checked with `bytepatch show`.
- **No build, no deploy.** Every deleted line sat under a flag that was off, so the preprocessed source, and the exe, are unchanged. A deploy now would also have shipped the peer C3d-2's uncommitted 16-bit state-id code, which must reach the game together with its data036.

**Hygiene** (`flagcheck JUNE_USEFUL_VALUE --with JUNE_USEFUL_VALUE_DEBUG --worktree --files KTDXLIB/Always.h X2Lib/X2GUUser.cpp`, before commit; `Always.h`'s +26 lines are the peer's C3d-2 defines, not staged here):
```
files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 38 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
The 38 deleted lines are the dead lines above, all added inside this phase. Range (`flagcheck ... --range b48c9e6^..HEAD`, where the code landed through a peer's commit; 3 commits b48c9e6 / 037fce2 / 8b56f68):
```
files: 4 PASS, 0 PASS~ (blank/marker only), 3 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR
in this flag's files: 39 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
RESULT: PASS
```
39 deleted = the 38 above + 037fce2's `#define _DEBUG` -> `//#define` (switch-off). Net over the range: insertion only.

**Next:** C9h-4g (Opus 5.5 high, kickoff updated), not at the same time as C3d-2 / C3d-3.

### C9h-4g - Done (2026-10-10, Opus 5.5 high; flags `SERV_IRUHADEV_JUNE_USEFUL_STRING`, `_NOT_SELECT_UNIT_OFFSET`, `_RESERVED_STATE_IDS`, `_WAIT_STATE_CHANGE` + `SERV_IRUHADEV_JUNE_C9H4G_DEBUG` (on for the play-test); log `JUNE_FX C9h-4g`; data036 Enum.lua repacked; play-test PASSED 2026-10-10, logs read)

Files: `X2GUUser.h/.cpp`, `Always.h`, data036 `Enum.src.lua` (folded into BOM-source `Enum.lua`). Funcmap rows `(C9h-4g)` (13), JUNE_BUGS 155, checklist 7h row, backlog Stage 8 lines.

**Decoded** (details in the funcmap rows)
- **USEFUL_STRING**: a `vector<wstring>` per parse (Future `+0x8f40`, Now `+0x8f30`): clear, then tables `USEFUL_STRING0, 1, ..` with value `[1]`. A miss assigns `L""` (the kickoff's "logs on a miss" was wrong: `3344f1f4` is an empty string). There is no row cap. The getter is `32a78fa0` ( index, bFuture ); its 15 callers are all Ain class code. A static `ParseUsefulString( table, vector )` (`31fea4d0`) is Ain-only too.
- **CUSTOM_OFFSET_NOT_SELECT_UNIT** (2 floats `+0x8f54/58`) and **USE_NOT_SELECT_MOTION_DIFFERENCE** (bool `+0x8f5c`, a miss reads true; the key name is XOR-built on the stack, decoded positionally): both parses write ONE set (JUNE_BUGS 155). Their only reader is PhysicProcess's dual-body branch (`32015b30 @32019f9b`, Lu/Ciel).
- **The event states 0x6e..0x76** ("Heavenly Combat Support Kit": Sniper / Bomber / Quantum Bomb / Random Fire). The base InitState part `3202b110` registers 9 states with about 25 handlers; all 12 classes call it. The tables are in June `datai015 CommonSkillState.lua`, a file March lacks. The only entry is the dropped-item pick-up `3230bdd0`: items 100005740..3 (in the staged June kim) -> `StateChange`, latch `+0x9490`. A first read of the ids as 99940204.. was a hex slip, caught before any conclusion. **Live in June, too big for this phase -> C9h-4h** (kickoff written).
- **USI_WAIT_STATE_CHANGE** is a June *base* id (`GUSI_END + 125` = 0x82). Each class registers it with only an OnEventProcess (`31fc8620`). Its one entry is the WIN / LOSE delegate `31fcd0b0` (the only `StateChange( 0x82 )` in June).
- Not entries: the `3220b740` sites push the same small numbers into a special-ability name table.

**User decision (2026-10-10): one shift now.** Eleven ids are reserved in June's order: the 10 `USI_EVENT_*` before `USI_BOUNDING_*`, and `USI_WAIT_STATE_CHANGE` after them. June's Phoenix (PvP amusement) and Dance ids in between get no March id.

**What landed**
- USEFUL_STRING: `JuneParseUsefulString` (both parses, right after USEFUL_VALUE) and `JuneGetUsefulString( index, bFuture )`.
- NOT_SELECT: `JuneParseNotSelectUnitOffset` (both parses, after USEFUL_STRING); ctor zeroes as June (`@320431e2`); getters for Stage 8. The PhysicProcess branch goes to backlog Stage 8.
- Reserved ids: `USI_EVENT_SNIPER_INIT` 88 .. `USI_EVENT_GUN_DANCE_BACK_STEP` 97, `USI_BOUNDING_*` 98..100, `USI_WAIT_STATE_CHANGE` 101, `USI_END` 102 (was 91).
  - Gate (`enumprobe.py`, = Lua): Chung's last state `CSI_A_CIP_ARTILLERY_NOVA_FIRE` 269, `ASI_DASH_COMBO_X` 135, `USI_WAIT` 13.
  - data036 `Enum.src.lua` has the same rows (`GUSI_END + 83..97`). `kom.py compile` folded it (tier EDIT).
  - That compile also folded 30 other T0 sidecars. They were restored to bytecode (`luasrc.py restore`), and their ledger / core rows were put back to HEAD. Only Enum is new source (core row -> `source`). `ledger --check` and `core --check` PASS.
  - PACK OK: vs the live kom only `Enum.lua` changed.
- WAIT_STATE_CHANGE:
  - `JuneWaitStateChangeEventProcess` = `31fc8620`: Future state time > `USEFUL_VALUE` Future[0][0] -> `USI_WAIT`, then CommonEventProcess.
  - `JuneInitWaitStateChange( table )`: the class-port call. A missing table registers nothing and logs once.
  - The delegate `JuneRandomLoseEventProcess` makes June's `StateChange( 0x82 )` again, to the new id, only when the class registered it. That guard is March safety, not June.
  - No March class registers the state, so nothing changes in play (checklist 7h row).

**Hygiene** (`flagcheck <flag> --with JUNE_C9H4G_DEBUG --worktree --files Always.h X2GUUser.h X2GUUser.cpp`):
```
USEFUL_STRING / NOT_SELECT_UNIT_OFFSET / WAIT_STATE_CHANGE: files: 0 PASS, 0 PASS~, 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR
  in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
  RESULT: PASS
RESERVED_STATE_IDS: files: 0 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (other flags), 0 FAIL, 1 ERROR   RESULT: FAIL
```
OTHER = the four flags share the three files. RESERVED_STATE_IDS's one ERROR is `Always.h` line 6511, WAIT_STATE_CHANGE's dependency guard
`#if defined(..WAIT_STATE_CHANGE) && !defined(..RESERVED_STATE_IDS)`, which flagcheck cannot evaluate (the same limit C3j-1 recorded for C3i's guard): not a finding. 0 deleted lines (insert-only).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,511,808 B, dated 2026-10-10 07:03:04, md5 444092992fed518aa5615fdec4fe5a55, equal to X2/US_SERVICE/x2.exe (2026-10-10 07:03:00); built at mods/june-buff-port c74f63f + 3 uncommitted code file(s).` Strings in the exe: the three wide `JUNE_FX C9h-4g` formats and the three narrow keys, once each.
**data036:** `COPY OK data036: ...\data\data036.kom  10650176 bytes, 192 entries, md5 fe54e8bdda6e4e8cbf2a449aa4ec3173` (user's yes, 2026-10-10; backup `kom_backup_tools\data036.kom.20261010_085357`). The exe and this kom go together: either alone puts every class state id 11 off between C++ and Lua.

**Trap for the next data036 Lua edit:** `kom.py status data036` still says `stale .src.lua: compile`. A plain `kom.py compile data036` would fold the 30 T0 sidecars again (the sidecars are not core-list files: restore them, as here). The route for a future Enum edit is the text `Enum.lua` itself + `luasrc.py refresh data036 Enum.lua`; `Enum.src.lua` is now a stale copy of it.

**Play-test facts (enumprobe):** only Chung crosses 255 after the shift (last 269; the other classes top out at 239). Tactical Trooper `CSI_CTT_COMBO_DASH_X` is 262 (was 251), Rapid Cannon loop / final 267 / 268, Artillery Nova fire 269, `CSI_COMBO_XX` 126, Wolf Spear 212, Wolf Break 214.

**Play-test PASSED (user, 2026-10-10; run 09:29-09:36, after the deploy):** all 7 checks (Chung guard XX, Freils dash X 262 / Bigbang / Aceldama, Rapid Cannon 267/268, Artillery Nova 269, Ara Wolf Spear -> Wolf Break, every class's common states + one skill, a full clear). Logs: 12 `USEFUL_STRING ... rows=0` and 12 `NOT_SELECT ... offset=0,0 motion_difference=1` lines (the miss default is true, as June), no `USI_WAIT_STATE_CHANGE` line (no class registers it), 0 EXCEPTION, UNHANDLED = 8x `E_DISABLE_HB_CHECK_REQ` (baseline), clean-exit census.

**Open for the user:** none. Next: `/x2-phase june-effect C9h-4g review` (Opus 5.5 high); C9h-4h after it.

### C9h-4g - Review (2026-10-10, Opus 5.5 high): no code finding; `SERV_IRUHADEV_JUNE_C9H4G_DEBUG` off

Checked against June in Ghidra (read-only). No transcription error, so there is no code change besides the debug flag.
- **USEFUL_STRING** (`320506f0 @32056b0e-32056bf2`, `32057b80 @32060dda-32060e5e`):
  - Matches. Future clears by erase( begin, end ) and Now by `31c33790`.
  - `BeginTable( "USEFUL_STRING", i, 1 )` from 0 = March `BeginTable( name, index )` (`"%s%d"`, `KLuaManagerProxy.cpp:84`).
  - Per row: `resize( i + 1 )`, then `GetValue( 1, &row[i], 1 )` into a wide string (stride `0x1c`). A miss assigns `L""` (`3344f1f4`, length 0), which equals the resized empty row. Then EndTable.
  - The first miss ends the loop, with no EndTable and no cap.
  - Getter `32a78fa0`: `bFuture ? +0x8f40 : +0x8f30`, `cmp size, index; ja` (unsigned) -> a copy, else empty = `JuneGetUsefulString`.
- **NOT_SELECT** (`@32056bf2-32056edd`, `@32060e5e-32061149`):
  - `fldz` into `+0x8f58` and `+0x8f54`, `BeginTable( "CUSTOM_OFFSET_NOT_SELECT_UNIT" )`, values 1 / 2 (a miss writes 0.0), EndTable.
  - Then the bool into `+0x8f5c`; a miss writes 1.
  - Positional decrypt re-run on both stack builds. Future (`3379d004..23`, the last three bytes stored at pushed-stack offsets) and Now (`3379d444..63`, byte order 0-28, 30, 29, 31) both give `USE_NOT_SELECT_MOTION_DIFFERENCE`.
  - Both parses write the same three members (JUNE_BUGS 155 stands).
- **Reserved ids:** `enumprobe.py` gives `USI_WAIT` 13, `USI_EVENT_SNIPER_INIT` 88, `_GUN_DANCE_BACK_STEP` 97, `USI_BOUNDING_DOWN` 98 / `_UP` 100, `USI_WAIT_STATE_CHANGE` 101, `USI_END` 102, Chung `CSI_A_CIP_ARTILLERY_NOVA_FIRE` 269, `ASI_DASH_COMBO_X` 135.
  - data036 `Enum.lua` (BOM) has `GUSI_END(5) + 83..96`, and `USI_END` + 97 = 102.
  - June `Enum` order (`+105..114`, BOUNDING `+115..117`, PHOENIX / DANCE `+118..124`, WAIT_STATE_CHANGE `+125`) is kept with the gap closed.
- **The literal-id census, re-checked:** the shift moves nothing that keys on a literal.
  - `X2TrainingGameUI.cpp:98/277/343` compares `GetNowStateID()` (int) with `(const char) iStateID`. June keeps the cast (`32832910 @328333a7 / @328335e8`, `MOVSX byte`).
  - Its ids come from the 31 `Dungeon_Training_*` scripts (data035, bytecode). They use 66 names, all from the old classes' legacy *literal* Lua tables (`ELSWORD_SWORDMAN_STATE_ID`, `ARME_VIOLEMAGE_`, `LIRE_ELVENRANGER_`, `RAVEN_`; e.g. `ESSI_COMBO_X1 = 60`).
  - Those literals do not follow the C++ enum (`ESSI_COMBO_X1` is 118 in C++ now, 104 in the original March). All 66 are 16..92: below 128, so the cast is a no-op.
  - The literals below 88 hit base ids the shift did not move. 92 (`LESI_DASH_JUMP_COMBO_X3`) was the never-entered class base state before and is a reserved event id now.
  - Nothing overrides those tables at runtime.
    - Across every `kom_contents` `.lua`, the four table names appear only in data036 `Enum.lua` (`ESSI_BASE = 6`) and the training scripts.
    - No class script or X2Lib / KTDXLIB code writes them.
    - The old classes' C++ `*_WALK` / `_DASH` / `_JUMP_UP` exist only as comments, so they resolve to `USI_*`.
  - So the training's match result is unchanged by C3d-2 and C9h-4g. The mismatch itself (Lua 16 vs `USI_WALK` 14) is the studio's and predates every port. C3d-1's "the training literals are small" holds, for this reason.
  - No numeric state literal in `X2Lib` `StateChange( n )` / `GetNowStateID() == n`.
- **WAIT_STATE_CHANGE:**
  - `31fc8620`: `fld +0x8ef4` (Future [0][0]), `fld +0x7de8` (Future `fStateTime`), `fcompp`, `test ah, 0x41`, `jnz`. So the state changes only when time > value; equal and unordered stay. Then `StateChange( 0xd = USI_WAIT, 0 )` and `jmp` vslot `0x7d4`.
  - Delegate `31fcd0b0`: `+0x87d0` is `m_pXSkinAnimFuture` (AnimStop order) and `+0x2ac` is its anim-end byte. June's `+0x8e90 ? +0x8e84` dual-body branch is dropped, as in C3b / C3d-3 (Stage 8).
  - The registered-state guard is labelled as March safety in the code and the funcmap row.
- **Debug:** `SERV_IRUHADEV_JUNE_C9H4G_DEBUG` commented out (`Always.h`, 1 line). `JuneInitWaitStateChange`'s "NOT registered" line stays on by design: once per table, only when a class port calls it.

**Hygiene** (`flagcheck <flag> --with JUNE_C9H4G_DEBUG --range 4580737^..HEAD`):
```
USEFUL_STRING / NOT_SELECT_UNIT_OFFSET / WAIT_STATE_CHANGE: files: 0 PASS, 0 PASS~, 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR   RESULT: PASS
RESERVED_STATE_IDS: 3 OTHER, 0 FAIL, 2 ERROR   RESULT: FAIL  (both = Always.h:6511, WAIT_STATE_CHANGE's dependency guard, in 4580737 and 8c2c7c5's range; flagcheck limit, as the Done note)
  in each flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 encoding change(s)
```

**Worktree** (`--with JUNE_C9H4G_DEBUG --worktree --files KTDXLIB/Always.h`): USEFUL_STRING / NOT_SELECT / WAIT_STATE_CHANGE PASS (1 DEFINE).
- RESERVED: the same guard-line ERROR.
- The 1 deleted line in each = the debug `#define`, now commented out.
- WAIT_STATE_CHANGE's 2 token-less `#endif`s = its dependency guards (4580737).
- The debug flag cannot be checked by itself any more: no active `#define` is left for flagcheck's `git grep`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,510,784 B, dated 2026-10-10 09:53:16, md5 88482eeb6515b3a2512b50d1f25aa2a4, equal to X2/US_SERVICE/x2.exe (2026-10-10 09:53:11); built at mods/june-buff-port 3957194 + 1 uncommitted code file(s).`
- PCH was re-touched for X2Lib and X2.
- Wide strings in the deployed exe: `JUNE_FX C9h-4g USEFUL_STRING` 0, `NOT_SELECT` 0, `USI_WAIT_STATE_CHANGE registered:` 0, `NOT registered` 1 (not behind the debug flag).
- data036 is unchanged (md5 fe54e8bd stays).

**Regression play-test PASSED (user, 2026-10-10; run 10:13-10:16).** The exe was a peer's later deploy (10:06:15, md5 1035e5ef, built at 886ae18 + 2 uncommitted files, so it includes this review). Logs: 0 `JUNE_FX C9h-4g` lines, 0 EXCEPTION, UNHANDLED = 2x `E_DISABLE_HB_CHECK_REQ` (baseline), clean exit.

**Open for the user:** none. Next: C9h-4h (`/x2-phase june-effect C9h-4h`, Opus 5.5 high).

### C9h-4h - port: the event states (Heavenly Combat Support Kit)

Scope: June's user states `USI_EVENT_SNIPER_INIT .. USI_EVENT_GUN_DANCE_INIT` (June 0x6e..0x76; March ids 88..96 reserved by C9h-4g), registered by the base
InitState part `3202b110` and entered by the dropped-item pick-up `3230bdd0` (items 100005740..100005743). Decode the ~25 handlers, the members `+0x947c..+0x9490`,
the NPCs (`NUI_NEIL_THE_SNIPER_EVENT` 4342, ...), June's loader of `datai015 CommonSkillState.lua` (adopt the file, no invented Lua) and how the items drop; then port.
Run with: Opus 5.5 high. Read list: this section, "C9h-4g - Done", funcmap rows `(C9h-4g)` / `(C9h-4f)`. Done gate: each piece under its own flag with a funcmap row,
or a row saying why not; the June quirks named in the kickoff get JUNE_BUGS rows.

### C9h-4h - Done (2026-10-10, Opus 5.5 high; flags `SERV_IRUHADEV_JUNE_COMMON_SKILL_STATE`, `_EVENT_STATES`, `_EVENT_KIT_PICKUP`; log `JUNE_FX C9h-4h` ( offline_server.log ); datai015 CommonSkillState.lua adopted + packed)

Files: `X2GUUser.h/.cpp`, `X2Game.cpp`, `X2GameUnit.cpp`, `Always.h`; datai015 `CommonSkillState.lua` (June ljconv bytecode, 9044 B). Funcmap rows `(C9h-4h)` (17), JUNE_BUGS 157-162,
dependency census `june2017_re/c9h4h_event_state_deps.tsv`.

**User decisions (2026-10-10):** port the C++ now, inert; **no stand-in drop** (June's client never drops the kits: item ids 100005740..3 appear in the June exe only in the
pick-up switch, negated compare `@3230bf49`; a server dropped them).

**Decoded** (every handler; details in the funcmap rows)
- **Loader:** `32286cb0` is the base `CX2GameUnit::InitSystem`; after the unit script it loads `CommonSkillState.lua` into every unit's Lua state (users and NPCs).
- **Registration** `3202b110` (slots re-verified): 9 states, 25 delegates; the 8 that are only a Common* body stay NULL (March falls back to the same).
- **Sniper / Random Fire:** at anim 0.001 the old NPC dies and NPC 4342 is requested `Now USEFUL_VALUE0[0]` (500) ahead (0.3 s delay, NAT_ALLY, my team).
  - From anim 0.6 the NPC takes the camera, and CONTROL_IDLE starts.
  - The arrow keys aim. The value goes into C3e-1's synced `fClassSkillFloat`, and the NPC plays `NEIL_THE_SNIPER_<dir>`.
  - One Z fires (10 shots; 1 for Random Fire, which plays `EffectSet_SA_RBD_Gun_Dance_EVENT`). One X, 10 s, or no NPC ends it.
  - END kills the NPC, resets the camera (`InitFocusUnit`, game vslot 0x8c) and opens the latch.
- **Proton Bomb:** the same shape with NPC 4343, at `m_fEventTime[0].keyInputStart` (500) ahead. Z/X fires from 0.3 s, and it fires anyway at 3 s. The attack set plays
  800 back along the dir.
- **Meca Drop:** a Lua-driven skill state (the damage comes from its table). Its event is the skill-cancel set plus ATTACK_EX Z/X.
- **`3201bd60( false )`** is "game still running": in a battle field, NPCs loaded and not at a portal; otherwise `!m_bGameEnd && !m_bLastKillCheck`.
- **NPC side:** the `+0x8750 / +0x8d34 / +0x8e39` bytes are `SetFocusCamera` / `SetFocusCameraForce` / `SetApplyCinematicUI` (June Lua binds). `SetUserSummonedNPCInfo` stores the UIDs
  (`3230c1a0`, no stat set).
- **Latch `+0x9490`:** it is closed by the pick-up and opened by SNIPER_END's end and PROTON fire's end. MECA_DROP's event opens it every frame (JUNE_BUGS 162).
- **JUNE_BUGS:**
  - 157: GUN_DANCE uses the sniper table and handlers.
  - 158: SNIPER_INIT's StateEnd calls StateChange.
  - 159: BACK_STEP is never registered.
  - 160: the aim offset block can never run (distance <= -750).
  - 161: a second bomb kills the NEW bomb.
  - 162: the MECA latch.

**What landed**
- `_COMMON_SKILL_STATE`: `CX2GameUnit::InitSystem` loads the file after the script block. A miss logs once.
- `_EVENT_STATES`:
  - `JuneInitEventStates` runs at the top of `InitStateCommonRandom`, both variants. Every class calls it right after InitStateCommon, as June's 12 classes call `3202b110`.
  - The 25 handlers and the 6 members (ctor: UIDs -1, the rest 0) are in.
  - The event-NPC cases sit before the summon switch in `SetUserSummonedNPCInfo`.
  - March safety, labelled in the code:
    - a missing table registers none of the states;
    - an NPC id with no templet is not requested.
    - Both are logged once.
- `_EVENT_KIT_PICKUP`: `Handler_EGS_GET_ITEM_NOT` calls `JuneStartEventKit( m_iItemID )` after the HP / DIE return. Any picker counts, AI-party bots included (June).
- **Inert in play:**
  - No drop exists.
  - The NPCs (no `NPCTemplet` / `Enum` rows), the five `*_EVENT` effect sets (June `EffectSet_INT.lua`) and 26 of the 41 preload files are June assets March lacks.
  - The asset port brings them; NPC templet rows -> asset port / core-list registries.

**Hygiene** (`flagcheck <flag> --worktree --files Always.h X2GUUser.h X2GUUser.cpp X2Game.cpp X2GameUnit.cpp`):
```
COMMON_SKILL_STATE: files: 0 PASS, 0 PASS~, 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 1 ERROR
EVENT_STATES:       files: 0 PASS, 0 PASS~, 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 1 ERROR
EVENT_KIT_PICKUP:   files: 0 PASS, 0 PASS~, 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR   RESULT: PASS
  in this flag's files (all three): 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)
```
- **OTHER:** the three flags share these files, and X2GameUnit.cpp also carries C3o's WORLD_COLOR_WINDOW.
- **The 2 ERRORs** are `Always.h`'s dependency-guard `#if defined(..) && !(..)` lines (`EVENT_STATES` needs `RESERVED_STATE_IDS`, `USEFUL_VALUE`, `SYNC_CLASS_FLOAT`, `COMMON_SKILL_STATE` and `BUFF_AWAKE_LOCK`). flagcheck cannot evaluate those lines; they are not findings.
- Insert-only.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,530,752 B, dated 2026-10-10 11:10:39, md5 bbc8eb4bef00e8df2ad3f9752b256375, equal to X2/US_SERVICE/x2.exe (2026-10-10 11:10:34); built at mods/june-buff-port d001ec0 + 5 uncommitted code file(s).`
- C3o's peer build compiled these edits; mine found nothing to relink.
- Strings in the exe: the 5 `JUNE_FX C9h-4h` formats, `USI_EVENT_SNIPER_READY`, `NEIL_THE_SNIPER_DYING`, `PROTON_BOMB_UP_LEFT`, `EffectSet_SA_RHB_Proton_Bomb_Attack_EVENT`.

**datai015:** `PACK OK datai015` (`kom_contents\datai015_packed.kom`). Against the live kom: +1 `CommonSkillState.lua`. The other 17 "changed" entries have equal uncompressed sizes; only their zlib output differs.
`COPY OK datai015: ...\data\datai015.kom  113122 bytes, 24 entries, md5 d7c0f00afc09ffb0e6831bea67d3de80` (user's yes, 2026-10-10; backup `kom_backup_tools\datai015.kom.20261010_112639`).

**Regression play-test PASSED (user, 2026-10-10; run 11:35-11:39, after the deploy and the datai015 copy):** dungeon with AI party, full clear, other classes, battle field. Logs: exactly one `JUNE_FX C9h-4h event states registered: SNIPER_INIT 88 .. GUN_DANCE_INIT 96` (offline_server.log:133), no `CommonSkillState.lua NOT loaded`, no event NPC / kit lines (nothing enters the states), 0 EXCEPTION, UNHANDLED = 5x `E_DISABLE_HB_CHECK_REQ` (baseline), clean-exit census.

**Open:** C3o's commit d001ec0 left funcmap row `322f11d0` with its last clause doubled ("; no March CX2Game member" x2). It is left as committed; this phase only appended rows.

### C9h-4h - Review (2026-10-10, Opus 5.5 high): two NaN-direction compares fixed (`>=`), no other finding

Read-only against June (`disasm_range.py`, Ghidra decompile of `322e67c0`). Every item the kickoff lists was walked in the raw disassembly, and the constants were read from the exe.
Fixes are under `SERV_IRUHADEV_JUNE_EVENT_STATES`, inside C9h-4h's own block in `X2GUUser.cpp`; flag-off is unchanged.

**Findings (fixed)**
- **`31fc9a20` (SNIPER CONTROL_FIRE event):** `fcompp` (state time vs Future[0][0]) + `test ah,1; jne skip` changes state only when the compare is ordered and not below.
  - The port's `!( t < v )` also fired on a NaN state time. It is now `t >= v`.
- **`3200e260` (PROTON CONTROL_IDLE event):** the keyInputEnd compare is `fcompp` + `test bl(1),ah; jne aim`, so it fires on `t >= keyInputEnd` ordered.
  - The port's `false == ( t < end )` also fired on NaN. It is now `t >= end`.
- Neither can trigger in practice (a state time is never NaN); the kickoff asks for exact NaN behaviour.
- **Comment only:** `JuneEventCreateNpc` said "the same argument list as JuneDieCreateNpc's". The full picture: June passes 16 arguments, and March's 14 match June's first 14.
  - June then adds the IsMyUnit byte (1 here), which lets a non-host past `322e67c0`'s host gate, and a 0 packet field.
  - March keeps its `IsHost` gate. Offline the local player is always host, so nothing differs. Funcmap row `3200d1d0 (C9h-4h)` says the same.

**Checked, matches June**
- **Aim classifier** `3200d450 @3200d6e0-3200d859`:
  - nine branches in order 1, 3, 7, 9, 4, 6, 2, 8, else 5;
  - each branch's x / y sign test (`test ah,1` = `>= 0`, `test ah,0x41; jp` = `<= 0`) and keys (pureUp `+0x86e2` / Down `+0x86e6` / Left `+0x86da` / Right `+0x86de`, = March InputData);
  - constants 1.0 .. 9.0 read from the exe. The 7 branch's inline compare against qword 7.0 (`334729f8`) is the same store.
  - The store (`fucompp` + `test ah,0x44; jnp`) writes on unequal or NaN, which is the port's `!( == )`.
  - The `-750` block (`33476834` = -750.0) runs only when the distance is `<= -750`, i.e. never (JUNE_BUGS 160). The 1500 step is `NPC.y >= my.y + 1500`.
- **Proton aim body** `3200e2f1-3200e6c1` = `3200d450`'s: identical constants, key offsets and the `+0x7ed8` store through `323101d0`. Only stack offsets and one `mov ecx, edi` differ. Its exit runs InputData.Init, then CommonEventProcess.
- **Jump tables** `3200db78` / `3200e9b8` (strings read): 1 UP_LEFT, 2 UP, 3 UP_RIGHT, 4 LEFT, 5 / default IDLE, 6 RIGHT, 7 DOWN_LEFT, 8 DOWN, 9 DOWN_RIGHT = `s_apszJuneEvent*AimState`; `_ftol` truncation, `dec; cmp 8; ja`.
- **x87 compares:** `320211d0` / `320212b0` anim `+0x214 >= (double)0.6f` (the port's double `0.6` is equivalent for every float), then `5 < t` or `!3201bd60( false )`;
  `3200d450` `Future[0][0] < timer` strict; `3200e260` `keyInputStart <= t`; `3200d1d0` / `3200e040` `0.001 > +0x24c` = `EventTimerOneshot`. Shots: `jg` = signed; X2Lib has no `/J`.
- **Cancel windows** `31fcfb70` / `31fcfde0`: each window is `0 <= after` (`test ah,0x41; jp`), then anim strictly above it (`test ah,5; jp`), then on the ground (`+0x7e1c`).
  - Keys: WALK = pureRight / pureLeft. BWALK = (pureLeft && bIsRight) || (pureRight && !bIsRight) (`+0x7e56`). DASH = pureDoubleRight `+0x86e0` / pureDoubleLeft `+0x86dc` -> vslot 0x668.
  - ATTACK_EX `+0x8c88`: oneZ, then oneX, each with its own window. Meca's anim end goes to WAIT on the ground, else JUMP_DOWN (0x11).
  - The SKILL / awakening block is C9h-4c's proven `SKILL_CANCEL_AFTER` expansion.
- **Proton fire start** `3200e9e0`: +180 (qword `33438080`) to rot.y only when vslot 0x170 (isRight) is false; dir.y = 0.
  - Offset = dir * -800 (qword `33476ac8`), added facing right and subtracted facing left. Hyper = gage `+0xe8 > 0`.
  - The set is `EffectSet_SA_RHB_Proton_Bomb_Attack_EVENT` with (1,1,1), -1.0, custom position, then pos / rot / dir. Sniper fire `3200dba0`: dir.y = 0 only, and `+0x9488 == 1` -> Gun Dance set.
- **NPC create** `@3200d36f` / `@3200e1e8`: (`0x10f6` / `0x10f7`, 0, 1, pos, isRight, 0.3f, 1, -1, team `+0x278`, 1 = NAT_ALLY, my UID, 0, -1 = TN_NONE, 0 = NCT_NONE, IsMyUnit, 0); see the comment fix above.
  - The old sniper keeps its UID at INIT. The old bomb's UID goes to -1, and `327573b0` = an IsHost query (NPC vslot 0x19c, result unused) + SetNowHp( 0 ).
- **`3201bd60( false )`:** no game -> false; GT 3 (= `GT_BATTLE_FIELD`): `+0xc74` and not near a portal; else `+0x748 != 2` and not `+0x3b9` (argument false). Phase 1 has no March counterpart (labelled).
- **`3230c1a0 @3230cd79-3230ce5e`:**
  - 4342: the owner keeps the UID, power 1.0.
  - 4343: when the old UID is still found, the camera bytes and SetNowHp( 0 ) go to the **new** NPC (`edi`; JUNE_BUGS 161).
  - New UID, power 1.0, and `SetShowObject( false, true )` (push 1, push 0) only when `GetMyUnit()` exists and the teams differ.
- **Latch `+0x9490`:** closed only at `@3230bf78` after a kit matched (table `3230c034`: 100005740 -> 0x6e, 41 -> 0x75, 42 -> 0x72, 43 -> 0x76); opened by `3200dff0` (before
  CommonStateEnd), `3200ed60` (after it) and `31fcfde0 @31fd00ef` on every exit; the port orders all three the same way.
- **Also re-read:** `31fc99c0` / `31fc9cf0` (shots 10 / 1, Gun Dance 0 / 1, timer 0), `31fc9a80`, `31fc9aa0` (WAIT 0xd), `3200d3f0`, `3200df80`, `3200e6e0`, the three preload lists.
- **Not a finding:** June picks the second skin anim (`+0x8e84/+0x8e88`) when `+0x8e90` is set. That is the June-only dual body, read as March's single anim as in C3b / C9h-4 (checklist 7).

**Hygiene** (`flagcheck <flag> --range <first>^..HEAD`, after the commit):
```
review commit 3eb8ef0 alone (EVENT_STATES): 1 PASS (X2GUUser.cpp +7 -3), 0 FAIL, 0 ERROR; RESULT: PASS
f07940f^..HEAD COMMON_SKILL_STATE: 1 PASS, 4 OTHER, 0 FAIL, 2 ERROR, 1 deleted
f07940f^..HEAD EVENT_STATES:       1 PASS, 9 OTHER, 0 FAIL, 4 ERROR, 4 deleted
f07940f^..HEAD EVENT_KIT_PICKUP:   0 PASS, 20 OTHER, 0 FAIL, 0 ERROR; RESULT: PASS
  0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the token, 0 encoding change(s) in all three
```

- The two range FAILs come from ERROR rows only: the `Always.h` dependency-guard `#if defined(..) && !(..)` lines, which flagcheck cannot evaluate (the known limit), re-reported for each later commit that touched `Always.h`.
- The deleted lines: 3 are this review's replaced lines inside C9h-4h's own `#ifdef` block (flag-off unchanged). 1 is 085ddf9 (C3k review) commenting out `FLY_COUNT_DEBUG`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,531,776 B, dated 2026-10-10 12:20:24, md5 79ce7ab690266b5af16669106078cd29, equal to X2/US_SERVICE/x2.exe (2026-10-10 12:20:19); built at mods/june-buff-port bf3c61e + 5 uncommitted code file(s).`
- A peer's build compiled the fix: `X2GUUser.obj` is 12:14:40, after the edit at 11:58:51. Mine found nothing to relink. No new strings (one compare operator per site, plus comments).

**Play-test (regression only; nothing reaches these states):** one dungeon run. Logs: one `JUNE_FX C9h-4h event states registered` line, no `CommonSkillState.lua NOT loaded`, 0 EXCEPTION.

**Regression play-test PASSED (user, 2026-10-10; run 13:13-13:16 on the deployed md5 79ce7ab6, dungeon to result screen, clean exit):** one `JUNE_FX C9h-4h event states registered: SNIPER_INIT 88 .. GUN_DANCE_INIT 96` line (offline_server.log:146), no `NOT loaded` line, 0 EXCEPTION, UNHANDLED = 2x `E_DISABLE_HB_CHECK_REQ` (baseline).

### C9h-5 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): the village-NPC effect set is one key in June's `CX2TFieldNpc` rewrite -> Elysion E1 step 6, backlog Stage 18

Read-only in Ghidra. There are 23 funcmap rows tagged `Effect C9h-5`. The 44-block `OnFrameMove` diff is in `june2017_re/c9h5_fieldnpc_onframemove_diff.md`. No C9h-5b: this is not an effect-engine slice.

**1. The key.**
- NPC `+0x198` is March's `m_pHouseTemplet`. June's `HouseParsing` (`324f3260`) writes `EVENT_NPC` to templet `+0x1e8` (`@324fbf54`, then the start/wait/active floats in March's order). It writes **`EFFECT_SET_NAME`** to `+0x5e8` (`@324febb4`, `L""` on a miss) through the same templet register.
- The file is `data034` `DLG_Map_House.lua`. It is the only house file June opens (`32503150`). `_1` carries the same three rows.
- **Reach: 3 houses in June, all Elysion.** `HI_BERNARD` plays `EffectSet_Elysion_NPC_Ver_ring`, `HI_DURANDA` plays `_Dyurenda_AC`, and `HI_THEODOR` plays `_Teo_ring`. All are `Bip01_Head` with `TRACE_MATRIX`, in June's `EffectSet.lua` and `UIEffectSet.lua`.
- **March: 0.** No `DLG_Map_House*` in the March kom extract carries the key, and March's `EffectSet.lua` has none of the three sets.

**2. Readers of instance `+0xe0`** (sweep over `321d9000-321f8000`):
- `GetEffectPosition @321dd1a3` (known from C9h-3);
- `GetCombineMatrix @321dc8af`: NPC skin `GetCloneFrame( BONE_NAME )`, frame `+0x90`;
- `GetEffectPositionByBoneName @321dd9dc`: `321f2c70`, else NPC `+0x5b4` = `m_vPos`;
- the create-time anim gate `@321eebf3`: NPC skin `+0x214`, as the riding pet's.

There is no `GetUnitRotation` branch. The dtor does not touch the raw pointer. The other sweep hits are `EffectData` floats and `g_pData+0xe0`.

**3. Handle lifetime.**
- The ctor `325d13e0` sets `+0x190 = -1`.
- `OnFrameMove @325ce1d9` plays the set once, after the first skin is built, then calls `SetShowObject( true )` (`321de4f0`) and nothing more. The set ignores the NPC's show state.
- The **dtor** `325cd7c0` is the only stop: `321dafb0( &+0x190, false )`, which only queues.

**4. `OnFrameMove` changed throughout** (44 blocks: 13 June-only, about 25 June-only members). The second skin, `MultiTex`, a motion-name map from 8 `*_MOTION_NAME` keys (June's defaults equal March's literals, plus `Talkloop` / `TalkWait`) and a motion queue are new. So are the quest-conditional messages (`325ca0b0`), `Upgrade/Service/Trade_Talk`, the house 40082 / 40129 branches and `bOnlyClickable`. The talk and bye blocks were moved into `325ccf60` and `325ca2f0`, not yet diffed. The friendship branches are gated on `IsEnableCode( 2215 )` (`@325cec47`).
- **Corrects C9h-4:** `323bddf0` is `CX2TFieldNpc` reading templet `bOnlyClickable` (`+0x5a1`), not a riding-pet getter. Its funcmap row and the Stage 17 bullet are fixed.
- **Checked in raw disassembly** (the diff was drafted from the decompile):
  - The quest button follows only `m_nNewQuestNormal` (`SETG` @325cebcd).
  - June has no `_Down.dds` talk-mark literal.
  - The draft's "dead `+0x239`" was wrong. `CX2TFieldGame` `323b5970 @323b72fa` sets it, so block 15 runs and discards its result. The diff file is corrected; 23 rows now, not 22.
  - March's `EffectSet.lua` is bytecode with plaintext constants, so the grep was valid.

**The 56 June-only `HouseParsing` keys** (use VAs in the funcmap / the diff file):
- **mesh and render:** `NPCMeshName2nd`, `NPCAniTex2nd`, `AlphaBlend1stMesh`, `AlphaBlend2ndMesh`, `NoEdge2ndMesh`, `OutlineWidthMesh`, `bApplyAlphaTest`, `UseRenderOption`, `UseAlphaBlend`, `MultiTex`, `SrcBlend`, `DestBlend`, `Color`, `EFFECT_SET_NAME`;
- **motion:** `JoinAnimSpeed`, `WaitAnimSpeed`, `ByeAnimSpeed`, `TalkLoopAniNpc`, `WAIT01/02/03`, `TALKLOOP`, `TALK`, `TALKWAIT`, `BYE`, `NORMAL` `_MOTION_NAME`;
- **services:** `OLD_ENCHANT`, `OLD_ADVANCED_ENCHANT`, `PHOTOZONE`, `FRAGMENT_EXCHANGE`, `ROCKSCISSORSPAPER_NPC`, `HENIR_EQUIP_UPGRADE`, `GUILD_AGIT_SERVICE`, `GUILD_ENTER`, `EXTRACTION`, `TRADE`;
- **talks:** `Upgrade_Talk`, `Service_Talk`, `Trade_Talk`, `Receive_{Like,DisLike,Normal,Cash}_Gift_Talk`, `Level_Up_Talk`, `Every_Day_Reward_Talk`, `After_Talk`, `Reject_Gift_Talk`, `Gift_UI_Talk`;
- **message rows:** `TYPE`, `STATE`, `QUEST_ID`, `SUB_QUEST_ID`, `MOTION`;
- **flags:** `AlwaysShow`, `bOnlyClickable`, `bMovingNPC`.

**Routing (no port kickoff and no new STATUS row; all three have owners):**
- **Elysion E1, new step 6.** E1 already owns `X2TFieldNpc` and `HouseParsing`, and the no-partial rule makes `OnFrameMove` one port. Step 6 covers:
  - the whole function and its keys;
  - the effect set with its engine overload and four readers;
  - a merge note: the `+0xe0` branches sit next to C9h-3a's `+0xd8`, so merge after C9h-3a lands;
  - the behaviour change in March villages (June wins);
  - the asset list for E2 / E13.
  - The backlog Stage 1 hand-off line was added.
- **Friendship manager and sequencer -> new backlog Stage 18.** Whichever of E1 and Stage 18 lands second wires the calls.
- No JUNE_BUGS row: nothing is ported. E1 records the diff file's five candidates when it ports them.

### C9h-7 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build): June's sound auto-release is one unref per opened device; user chose **(b) port verbatim** -> C9h-7a

**How it works in June** (funcmap rows `Effect C9h-7`):
- The device `PlaySound` `31ce0e10` has two branches.
  - **Found** (the device is already in the map): `LoadDevice` + `Play`, with no reference taken and no callback. This is March's branch.
  - **Open**: `OpenSound( name, 10, b3D, -1.f )` (+1 ref), `Play`, then `31cdd4e0( m_pChannel )` only when arg 4 is 1.
- At the channel's END, the FMOD callback `31cdcca0` calls `31cdcbb0`. It queues the first sound device whose current `m_pChannel` (`+0x60`) is the ended channel. The device manager's `OnFrameMove` `31cde8d0` then calls June's `CloseDevice( CKTDXDevice* )` `31cdd8d0` on each queued device: **one unref**, then erase + delete only at zero.
- **Corrections to the C9h-4b review.**
  - `[dev+4] == 1` is `m_DeviceType == DT_SOUND`, not a ref count. The count is `+0x48`.
  - The release is an unref, not a forced free: a device that someone else opened survives it.
  - March does have a channel callback (`KTDXDeviceSound.cpp:607`), but its body is empty and it is compiled out (`CHECK_VOICE_IN_SLIDESHOT`).
- **Nothing else touches `+0x240`.** The only other accessors are the ctor `31ce2520` and the dtor `31ce2220` (dispsweep).
- **There is no game-code window between the queue and the delete.**
  - June's stage chain (app ctor `31c17ed0`) is `CKTDSManager` (FMOD update) first and the device manager fifth, with only engine stages between them.
  - March has the same order (`KTDXApp.cpp:439-451`).
  - A replay changes `m_pChannel`, so a stopped-then-replayed device is not queued.
- **The other `31cdd4e0` caller** is `32800e10`: June's `CX2World` world-event timeline. Its type-1 event plays a sound and does not keep the handle. The system is June-only -> backlog Stage 2.

**Census** (`june2017_re/c9h7_playsound_census.tsv`, script `c9h7_playsound_census.py`, corrects the review's 174 / 9): 242 call sites.
- 214 pass 1 and 24 pass 0.
- 4 pass a register the script could not resolve (`3250f680` x3, `32e2b608`). All four drop the handle.
- The 4th parameter has no recoverable default: every site pushes all four arguments.
- **The 24 zero sites** = 13 + 3 + 2 + 6:
  - 13 are March's cross-frame keep-sites: 9 create-unit voices (`+0x718`), `PlaySoundSelect` (`+0x490`), the TFieldNpc near sound (`+0x4b0`), the wedding duration sound and the emblem vector.
  - 3 are June-only voices (Luciel, Rose, Ain).
  - 2 are the Lua wrappers `31be4734` / `31be4894`.
  - 6 drop the handle: `X2_Button_Mouse_Up`, `Count`, `LevelUpBGM`, `Go`, `NewStage_Open`, `UseItem`.

**March audit** (`june2017_re/c9h7_playsound_handle_audit.tsv`): 16 March call sites store the handle. June passes 0 at each one that keeps it across frames, with two exceptions:
- **The square** (`+0x5ec`, JUNE_BUGS 122). Latent: the list stays empty on March data until C9h-4e.
- **`CX2GUAdd::PlayLoopSound`** `329e2660` (`m_pLoopSound` / `+0x9608`, arg 1). **JUNE_BUGS 125.**
  - `bLoop` is ignored, so the "loop" sound is a one-shot clip.
  - Neither March nor June preloads `Add_Effect05` / `Add_Effect10` / `ADD_PULSE_BULLET_CHARGE_01` (June's Add `READY_SOUND` is empty).
  - So a state that outlasts its clip calls `Stop()` on freed memory. The clips are 2.44 / 4.20 / 5.11 s; the state lengths were not measured, so reach is "not counted".

Other notes:
- **The receiver sweep came back clean.** Every other `->PlaySound(` receiver is a game unit, whose `PlaySound` is an `OpenSound` wrapper. The device manager hands out a sound only through `OpenSound` (which takes a reference) or `PlaySound`; there is no lookup without a reference.
- Lua cannot hold a handle: the only bindings are the two `void` app wrappers.
- `NpcPlaySound` is a June rewrite (`325c9c00`: stop the previous sound by name, 2D only, keeps the name) -> Elysion E1 step 6.

**Unowned, routed** (plan-unowned rule): June's damage-effect sound play `31f6eca0`.
- March opens with `OpenSound` and never closes. June uses `PlaySound( name, LOOP, THREE_DIMENSIONAL_SOUND, 1 )`, plus a `{ FMOD sound, channel }` list at `CEffect +0xb34` for `DELETE_WHEN_EFFECT_END`.
- Those keys are parsed by `JUNE_DAMAGEEFFECT_PARSE` but never consumed -> **C9h-8**.

**The user's decision (2026-10-08): (b) port verbatim, in a follow-up row (C9h-7a).**
- The square and Add keep June's argument 1 and June's bug (JUNE_BUGS 122 / 125: Port = verbatim, C9h-7a).
- C9h-7a's default choice: the new parameter defaults to **true** (June's majority), and March passes 0 exactly where June does. So no class file needs an edit.

**What changed:** no code, no build, no deploy. 21 funcmap rows `Effect C9h-7`, census + audit `.tsv`, JUNE_BUGS 125 (+ 122's Port cell), STATUS rows C9h-7 / 7a / 8, kickoffs `C9h-7a.md` and `C9h-8.md`, backlog Stage 2 line (world-event timeline).

then C9h-7a.

### C9h-7a - Done (2026-10-08, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE` + `_DEBUG` (on for the play-test); log `JUNE_FX SOUND_RELEASE`; play-test pending)

**What landed** (15 code files; `KTDXLIB/Always.h` defines both flags):
- `CKTDXDeviceManager::PlaySound( name, bLoop, b3D, bJuneAutoRelease_ = true )` (`31ce0e10`). The default `true` is **ours** (June has none). In the **open** branch only, after `Play`, `FMOD_Channel_SetCallback( pSound->GetChannel(), JuneSoundAutoReleaseCallback )` (`31cdd4e0` / `31cdcca0`) when the flag is true and the channel is not NULL. The found branch is March's: no reference, no callback.
- Callback: END (type 0, first enumerator) -> `JuneQueueAutoRelease` (`31cdcbb0`: first `DT_SOUND` device whose `m_pChannel` is the ended one, push, break, under `m_DeviceLock`) -> `return FMOD_OK`. `GetChannel()` is a flag-gated getter in `KTDXDeviceSound.h`.
- Drain in `OnFrameMove` (`31cde8d0`): **every frame**, outside the first-frame gate (June's `jne @31cde8ea` skips only `ProcessForegroundQueue`). Close each queued device, then `clear()`.
- Close = `JuneCloseDeviceByPointer` (`31cdd8d0`), **not** March's `CloseDevice( name )` (that one deletes at a count <= 0 and does not compare the map pointer). Count <= 0 returns; > 1 one decrement under `m_csDeviceState` (June: lock-free CAS; March's `m_RefCount` is a plain int); else `UnrefDevice` under `m_DeviceLock`, find by device ID, erase only if the map holds this pointer, delete inside the lock. `31c4a0d0( this+0x108 )` (unidentified) left out.
- IAT `007a0458` = the only target of the `31d4d4e0` thunk; its import is the FMOD Ex ordinal Ghidra calls `setCallback`. The call takes (channel, callback) = the C API `FMOD_Channel_SetCallback` of March's `fmod.h`. It links (the call had never been compiled before). Threading: `FMOD_System_Update` runs only in `KTDSManager.cpp:195` (main thread), no lock on the drain, as in June.

**Census corrections** (`c9h7_playsound_census.tsv`): the 3 unresolved `3250f680` registers are `ebp = 0` (the same register is the false `bLoop` / `b3D` at the site; March counterparts are `(false, false)`). The 4th, `32e2b608` (`MagicEnchant_Success`, June-only), is `ebx = 1`. So June has **27 zero sites** and 215 one-sites (not 24 / 214).

**Per-site arguments (final): `false` passed at 26 March call sites**, every other site keeps the default `true`.

| June zero site | March site(s) edited |
|---|---|
| 12 create-unit voices `32552860..` | `X2StateCreateUnit.cpp` x9 (Luciel / Rose / Ain: no March class) |
| `3255f240` PlaySoundSelect | `X2StateServerSelect.cpp:12106` (`ADD_PLAY_SOUND`, no macro) |
| `325cdfa0` near sound | `X2TFieldNpc.cpp:1553` |
| `326ba0c0` duration sound | `X2WeddingManager.cpp:219` (non-duration branch :230 keeps 1, as June) |
| `3244c8c0` emblem | `X2EmblemManager.cpp:432` |
| `31be4734` / `31be4894` Lua wrappers | `KTDXApp.cpp` `PlaySound2D_LUA` / `PlaySound_LUA` |
| `324c7ce0` X2_Button_Mouse_Up (square MsgProc: talkbox click, then `ToggleUI`) | `X2StateSquareGame.cpp:354` (`X2UIManufacture.cpp:295` keeps 1: June has no such site) |
| `327f4210` Go (SubStageGo* particle start) | `X2DungeonStage.cpp:333` (`X2PlayGuide.cpp:1077` keeps 1) |
| `3250f680` Count / ResultStamp / LevelUpBGM (= `CX2StateDungeonResult::OnFrameMove`) | `X2StateDungeonResult.cpp` :490, :681, :1176, :1509, :1521 (Arcade / Training results keep 1: no June site) |
| `3293b5e0` NewStage_Open | `X2LocalMapUI.cpp` :1393, :1594 |
| `32952930` UseItem | `X2UIQuickSlot.cpp` :903, :2405 (**review: :2405 is June's `32951f70`, which passes 1; fixed, so 25 sites, not 26**) |
| `32e2b608` MagicEnchant_Success | none (June-only) |

- `X2StateDungeonResult.cpp:520` (`Count.ogg`, true) sits inside a `/* ... */` block (dead code), so it was **not** edited (`flagcheck` cannot evaluate a directive inside a comment).
- Reach note: every one of these sounds that `SoundReady` preloads takes the found branch anyway, so the argument only matters for a non-preloaded open.
- The square (`+0x5ec`, JUNE_BUGS 122) and Add (`PlayLoopSound`, JUNE_BUGS 125) keep the default 1 and June's use-after-free; no guard, no class file touched. JUNE_BUGS 122 / 125 Port cells updated.

**Hygiene** (`flagcheck JUNE_SOUND_AUTO_RELEASE --with ..._DEBUG --worktree --files <15 files>`): files: 13 PASS, 1 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 `#else`/`#endif` without the flag token, 0 encoding change(s); RESULT: PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,376,640 B, dated 2026-10-08 14:46:13, md5 9cc6fcd7e0348b979ffc53d52f01a90d, equal to X2/US_SERVICE/x2.exe`. Strings in the exe: `JUNE_FX SOUND_RELEASE ` x2, ` deleted=` x1.
**Not changed:** no kom, no data. 5 funcmap rows `Effect C9h-7a`.

**How to see it in game:** the play-test hand-off below. A non-preloaded sound that is played and ends gives `queue <NAME>` then `close <NAME> ref=1 deleted=1`. Every later play of it reloads the file (a small load hitch is possible; none was measured yet).

**Post-deploy fix (found by the 15:58 play-test, in exe `944238b6` / `40b97943`, committed with the play-test results).** The drain was unreachable: the block sat after `CKTDXDeviceManager::OnFrameMove`'s `return S_OK;`. The return now follows the `#endif` (vs the pre-C9h-7a base the file is insert-only: `flagcheck --base aa905d0^` PASS~, 0 deleted). Also, under `_DEBUG`: `JuneQueueAutoRelease` returns after the first match and logs `end-nomatch` when no device owns the ended channel; the open branch logs `open <NAME> fmod=<FMOD_Channel_SetCallback result> channel=<0|1> ref=<arg 4> deleted=<callback registered>`. Worktree flagcheck (both flags, `KTDXDeviceManager.cpp`): PASS, 4 deleted = own flagged lines.

**Play-test PASSED 2026-10-08** (run 16:30-16:37, exe `40b97943`; user: all checks passed; logs read). Five sounds ran the whole chain `open ... fmod=0 channel=1 ref=1 deleted=1` -> `queue` -> `close <NAME> ref=1 deleted=1`: `SKILL_EQUIP`, `SKILL_UNEQUIP`, `QUESTREWARD`, `CAMERA_ZOOMIN`, `CAMERA_ZOOMOUT` (the 15:58 run also did `UI_QUICKQUEST_CHECK`, `QUESTCOMPLETE_ELSWORD`). 0 EXCEPTION, clean exit, no crash bundle.
- **Add check (`AddTest`, Psychic Tracer, X X Z Z + Z taps, 3 times): no crash, but JUNE_BUGS 125's path was not reached.** `ADD_EFFECT05.OGG` (and `ADD_EFFECT10.OGG`) logged one `open` with `fmod=-1 channel=0`: the first play returned no channel, so no callback was registered, and every later play took the found branch (no callback). The device was never released, so `StopLoopSound` never touched freed memory.
- **For the review:** many first opens show `fmod=-1 channel=0` (game-start / character-select voices, `QUESTACCEPT`, `PORTAL`, `SKILL_REFRESH`, the Add clips), and such a sound is then never auto-released for the session. Check whether June's open branch behaves the same (Play returning before a channel exists on a fresh load), or whether this is a March difference.

### C9h-7a - Review (2026-10-08, Opus 5.5 high): one transcription error fixed (the defence-dungeon quick-slot ACK passes 1); `_DEBUG` off

**Engine vs June: all four pieces match.**
- **Callback `31cdcca0`:** `cmp [esp+8],0` (END), then `31cdcbb0` on `[g_pKTDXApp]+0x1f4`, `xor eax,eax`, `ret 0x10`. The port's added NULL guard on the app and the manager changes nothing.
- **Queue `31cdcbb0`:** lock `+0xc4`, map walk, `[dev+4]==1`, dynamic cast, `[+0x60]==channel`, push `+0x240`, break.
- **Close `31cdd8d0`:** NULL or count <= 0 returns. At count >= 2, CAS `31d4d310` returns when it takes.
  - **A failed CAS falls through** to the locked `InterlockedDecrement` `31d4d300` and erases + deletes at zero (find by `+8`, node `+0x24` == device). That is June's only way for the "> 1" branch to end in an erase: another thread must change the count between the read and the CAS.
  - In March every `m_RefCount` writer (`RefDevice` / `UnrefDevice`, `KTDXDevice.cpp:170-186`) holds `m_csDeviceState`. So the port's locked read-and-decrement cannot fail, and the result is the same.
- **Drain `31cde8d0`:** `jne @31cde8ea -> 31cde8fb` skips only `ProcessForegroundQueue`; the close loop and the clear run every frame. This matches the port after the post-deploy fix.

**Census re-trace:**
- **`3250f680`'s 4 `ebp` sites are proven 0**, no longer inferred from March's arguments.
  - `3251029b`, `3251094c`, `325130c5` and `325131b2` are reached only from cases 3 / 9 / 20 / 24 of the state switch `3250fc56` (`[+0x30c]-1`, table `325138e8`).
  - The only `ebp` write before that dispatch is `xor ebp,ebp @3250fc01`. A backward walk over direct edges finds no `ebp` write between any case entry and its call; the `inc ebp` blocks belong to other cases.
- **`32e2b608`:** `mov ebx,1 @32e2b41b`, no other write (`lea ebx,[ebx]` is a no-op).
- **`324c7ce0` = the square-game `MsgProc`** = `X2StateSquareGame.cpp:354`, confirmed by structure: WM_LBUTTONDOWN, PShop talkbox hit `324c8260`, guest check `+0x31`, the sound, then `ToggleUI( 0x12 )`.
  - The Done note's "June has no such site" for `X2UIManufacture.cpp:295` is loose: June's other `X2_Button_Mouse_Up` site, `32e03200` (not identified), passes 1. The March site keeps 1 either way.
- **`327f4210` = `CX2DungeonStage::StartGoEffect`** (`SubStageGo*`) and **`3289d090` (1) = `CX2PlayGuide::StartGoEffect`** (`SubStageGoRight_UI`): confirmed.
- **`3293b5e0` -> `X2LocalMapUI.cpp` :1393 / :1594: confirmed by structure** (it was mapped from the sound name before).
  - The sound follows `GetShow` / `GetEnable` (vslots +0x6c / +0x64), the newly-opened test `3293d940` and the drop-down `32935910`. That is March's `UpdateDungeonButtons`.
  - The two March sites are the two `#ifdef` versions of that function.
- **`32952930` -> `X2UIQuickSlot.cpp` was half wrong; fixed.** June has three `UseItem.ogg` handlers. The quick slot's packet switch `32952e40` sends:
  - `0x110` and `0x656` to `32952930`, which passes 0;
  - `0x3eb` to `32951f70`, which passes 1;
  - `0x5d2` to `32951d10`, which passes 1.
  - **Which ACK is which:** `KeyEventProcess` `32951670` sends `0x3ea` and waits for `0x3eb` (`3294ea00`) when `+0x100` is set (March `GetIsSummonCardSlot()`); otherwise it sends `0x10f` and waits for `0x110` (`3294e290`). In March's preprocessed enum, `EGS_USE_QUICK_SLOT_REQ/ACK` = `0x119/0x11a` and `EGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_REQ/ACK` = `0x3f1/0x3f2`.
  - So `32952930` = `Handler_EGS_USE_QUICK_SLOT_ACK` (:903, 0, right), and **`32951f70` = `Handler_EGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_ACK` (:2409), which passes 1**.
  - **Fix:** `X2UIQuickSlot.cpp:2410` now passes `true` explicitly, with a comment, under the same flag. March passes `false` at **25** sites, not 26.
- **Missed-site sweep:** every stored `GetDeviceManager()->PlaySound(` result in X2Lib / X2 / KTDXLIB (assignment, including Add's split `m_pLoopSound =`; `return`; `push_back`; argument) is one of the audit's 16 sites. No other receiver stores a device-manager handle.

**Play-test (16:30 run) and `channel=0`:**
- Five sounds ran `open` -> `queue` -> `close ref=1 deleted=1`. Every close was `ref=1`, so the count > 1 branch was never exercised in game. 0 EXCEPTION, no crash.
- **Add (JUNE_BUGS 125):** no crash; the path was not reached. Reported only, no fix.
- **`fmod=-1 channel=0` behaves the same in June; it is not a port defect.**
  - `Play` returned before `FMOD_System_PlaySound`: there is no `KEM_ERROR374` in the log.
  - June's `Play` `31d414d0` has the same early returns: DS init `+0x34`, mute `+0x36`, state `+0x44 != 2`, system `+0x58`, sound `+0x5c` after `_CheckLoadFMODSound`. June's `31cdd4e0` registers only on a non-NULL channel.
  - So in June too, a sound whose first (open) play gets no channel keeps that open's reference for the whole session, as March did before the port.
  - The cause is runtime state, not the file. `QUESTCOMPLETE_ELSWORD` / `UI_QUICKQUEST_CHECK` got a channel in the 15:58 run. In the 16:30 run, every `channel=0` line is before 16:36 and every later open has a channel.
  - Candidate (unconfirmed): `PLAY_PROMOTION_MOVIE` (`Always.h:361`) mutes sound on entering server select (`X2StateServerSelect.cpp:293`) until the intro-movie block restores it.

**What changed:**
- `X2UIQuickSlot.cpp:2410` (`true`).
- `Always.h`: `_DEBUG` commented out, and the description now names the defence-dungeon exception.
- funcmap: 5 rows `Effect C9h-7a review`.
- No kom, no data.
- No new JUNE_BUGS row: `0x656` sharing `32952930` is June's dispatch, not a bug.

**Hygiene** (`flagcheck JUNE_SOUND_AUTO_RELEASE --with ..._DEBUG --worktree --files KTDXLIB/Always.h X2Lib/X2UIQuickSlot.cpp`): 1 PASS, 1 DEFINE, 0 FAIL; 3 deleted lines = our own (the `Always.h` description line and the `_DEBUG` `#define`, now commented, and the replaced flagged `false` line); 0 non-ASCII, 0 EOL-mismatched, 0 encoding change; RESULT: PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,386,368 B, dated 2026-10-08 17:06:44, md5 fa2af5980bce6d2b08c0cac56b49b795, equal to X2/US_SERVICE/x2.exe (2026-10-08 17:06:39)`. `JUNE_FX SOUND_RELEASE` strings in the exe: 0 (`_DEBUG` off).
**Play-test:** none owed (the fixed site is the defence-dungeon summon-card ACK, not reachable offline). next C9h-8.

### C9h-8 - Done (2026-10-08, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND` + `_DEBUG` (on for the play-test), log `JUNE_FX DE_SOUND`; data036 LOOP rows set to June's FALSE (user); play-test pending)

**June's damage-effect sound, mapped** (funcmap rows `Effect C9h-8`, 11):
- **The play `31f6eca0`.** It matches March except at the play itself. The timing tests (`USE_GLOBAL_TIME` vs mesh `+0x3e0`, otherwise the skin `EventTimer`), `rand() % 100 < iRate`, and the erase whether played or not are all unchanged.
- **What June changed:**
  - It calls the device `PlaySound( name, LOOP, THREE_DIMENSIONAL_SOUND, 1 )` instead of `OpenSound` + `Play()`.
  - With `DELETE_WHEN_EFFECT_END` it pushes `{ m_pChannel +0x60, m_pSound +0x5c }` into `+0xb34`.
  - It then calls `31d41710( pos )`.
  - It drops `AddSound`. The `CLOSE_SOUND_TEST` map at `+0x8e4` is never fed in June: only the ctor and dtor touch it, so the dtor's `CloseSound` walks an empty map.
- **Corrections:**
  - The pair is **channel first**; the C9h-7 row said sound, channel.
  - March's open was not "never closed": `CLOSE_SOUND_TEST` (`Always.h:2869`) closed it by name in the dtor.
- **The stopper.** It is only in `~CEffect` `31f78550 @31f785d1`, which runs before `CloseSound` and on every pooled free. For each entry with a channel:
  - `FMOD_Channel_GetCurrentSound` fails: `Stop` anyway.
  - It succeeds and the sound matches: `Stop`.
  - It succeeds with a different sound: skip.
  - Then the list is emptied.
- **`31d41710` is a June-only device method.** Its one caller is this play (E8 scan). With a channel, it stores the position, calls `Update3DPosition`, and on success `FMOD_Channel_SetPaused( false )`; `31d6293a` resolves to fmodex RVA `0x96100`. Without a channel it stores nothing. June keeps March's store-only `Set3DPosition` as `31d41070`. The difference matters: called after the play, March's version would leave the channel at the device's previous position.
- **June's `Update3DPosition` `31d411c0`** adds `result : %d, File : %s` to `KEM_ERROR381`. `+0x70` is June's own copy of the ctor's file name, the same value as `m_DeviceID`.

**Port** (insert-only, March in `#else`):
- `X2DamageEffect.cpp`: the play (`PlaySound`, push, `JuneSet3DPositionAndUpdate`, no `AddSound`) and the dtor stopper.
- `X2DamageEffect.h`: `m_vecJuneDeleteWhenEffectEndSound`.
- `KTDXDeviceSound.h/.cpp`: `GetSound()`, `JuneSet3DPositionAndUpdate` (June `31d41710`), and the `Update3DPosition` error text.
- `Always.h`: the flag, with an `#error` unless `JUNE_SOUND_AUTO_RELEASE` and `JUNE_DAMAGEEFFECT_PARSE` are on.
- JUNE_BUGS **134** (raw FMOD handles; stop on error) and **135** (a LOOP sound's device reference is never released).

**Reach** (census of the Lua sources):
- **March `DamageEffect.src.lua`:** 1,154 sounds; no `DELETE_WHEN_EFFECT_END` or `THREE_DIMENSIONAL_SOUND`; **13 `LOOP = TRUE`** (Aisha EM XZ, Eve EP dash ZZ / ZZZ / jump ZX, Add pylon ×2, PvP bots).
- **June:** the same 13 are `FALSE`; 38 sounds set `DELETE_WHEN_EFFECT_END` (Ain, Rose KS-83, Add NasodArmor XXX, Eve Atomic Shield); 5 `LOOP = TRUE`, all with DELETE.
- **Under June's code, March's 13 would loop forever and stack**, so this is a break. **User 2026-10-08: use June's FALSE.** The 13 rows in `kom_contents/data036_kom/DamageEffect.src.lua` now read `LOOP = FALSE -- C9h-8 ... Restore TRUE if the flag is off`; they were recompiled (`COMPILE OK data036`) and packed.
  - `PACK OK data036` reports `~0 changed` because it compares sizes, and the constant swap keeps the size. By md5 the packed kom is `ad48d728…` and the live one `cb5625f6…`.
  - Until C11 the list and stopper are unreachable on March data. The visible change is the play path, plus 3D placement applied after the play.

**Hygiene** (`flagcheck JUNE_DAMAGEEFFECT_SOUND --worktree --files` the 5 code files): 2 PASS, 1 PASS~ (one blank line), 2 OTHER (`Always.h` peers' blocks; `X2DamageEffect.cpp` = the `_DEBUG && OFFLINE` log blocks), 0 FAIL, 0 deleted, 0 non-ASCII, 0 encoding change; RESULT: PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,409,920 B, dated 2026-10-08 21:14:44, md5 561222164aeae84747f51182fad1f869, equal to X2/US_SERVICE/x2.exe (2026-10-08 21:14:39); built at mods/june-buff-port 920eafe + 7 uncommitted code file(s)` (the 7 include a peer's C9h-4f `X2GUUser` work). The 4 `JUNE_FX DE_SOUND` / `result : %d, File : %s` strings are in the exe. data036: `COPY OK data036 ... 10655716 bytes, 192 entries, md5 ad48d728211d0345fad4b1100377d7ea` (user yes; backup `kom_backup_tools/data036.kom.20261008_211615`).
**Play-test PASSED 2026-10-08** (run 21:22, exe `56122216`): jtCodeEmpress, EP dash Z / ZZ / ZZZ and jump ZX, user: each sound plays once, nothing drones.
- `offline_server.log`: 108 `JUNE_FX DE_SOUND … play` lines, all `loop=0 3d=1 del=0`; 0 `push` / `stop` / `skip` lines (March data has no DELETE_WHEN_EFFECT_END); 0 EXCEPTION.
- 79 lines `dev=1 channel=1`, among them `Eve_EP_Dash_Combo_ZZ` / `_ZZZ` / `Eve_EP_Jump_Combo`, the three formerly `LOOP = TRUE` sounds.
- 29 lines `dev=0`: `Eve_Dash_Combo_ZZ.ogg` (13) / `Eve_Dash_Combo_ZZZ.ogg` (16) are in no March kom (only the `Eve_EP_` variants are), so `ErrorEnum(351)` comes from `PlaySound`. June's data names the same files. This is a studio data gap that predates the port; March's `OpenSound` failed on them too.
next: C9h-8 review.

### C9h-8 - Review (2026-10-08, Opus 5.5 high): the play, stopper and device method match June; two verbatim June quirks added, the owner notify moved to the dtor head; `_DEBUG` stays on for one re-test

**Checked against June, no finding** (read-only disasm; funcmap rows `Effect C9h-8` and `C9h-8 review`):
- **The play `31f6eca0`:**
  - `PlaySound( name, +0x25 LOOP, +0x27 3D, 1 )`.
  - Pair stores: `+0x5c` at `[esp+0x1c]` before the `push edx`, `+0x60` at the same slot after it, so `.first` = channel.
  - The push is gated on `+0x26`; `31d41710( mesh +0x3f0 )` runs for every non-null device.
  - The erase runs for a failed rate roll (`jge 31f6ee17`) and for a null device.
  - Offsets `0x20` / `0x24..0x27` are confirmed by the parse's key buffers: DELETE (22 chars) -> `+0x26`, THREE_DIMENSIONAL (23) -> `+0x27`.
- **The stopper `@31f785d1-31f78646`:**
  - Null channel -> skip; GetCurrentSound fails -> Stop; match -> Stop; different sound -> skip; then the list is emptied.
  - It runs before `CloseSound @31f7864c`.
  - dispsweep `b34 b38 b3c`: the CEffect hits are only the ctor, the dtor and the push.
- **`31d41710`:** no channel returns before the store; `SetPaused( 0 )` only when `Update3DPosition` returns 1.
  - `333bcf14` -> `61e16100` = fmodex RVA `0x96100`, the only export there: `FMOD_Channel_SetPaused` (export table read directly).
- **`31d411c0`:** the buffer is 0x200 bytes (the port's `WCHAR[256]` is the same size).
  - `+0x70` is assigned from the ctor's by-value fileName `@31d419af`, the same argument the base ctor `31d40f00` gets `@31d41953` = `m_DeviceID`.
- **Data:** the 13 `LOOP = FALSE -- C9h-8` rows equal June's for the same templet, `SOUND<n>` and file.
  - The live `data036.kom` has been repacked since C9h-8 (C3d-2: md5 `f7a27248…`, 22:12).
  - Its `DamageEffect.lua` entry (extracted) is byte-identical (md5 `83230a62…`) to `kom_contents/data036_kom/DamageEffect.lua`, which is newer than the edited `.src.lua`.
- **Play-test:**
  - C9h-8's run: 108 `play` lines, all `loop=0`.
  - The later C3d-2 run (22:36): 186 `play` lines, `loop=1` none, 0 push / stop / skip, 0 EXCEPTION.

**Found, ported under the same flag:**
- **JUNE_BUGS 138: June never copies THREE_DIMENSIONAL_SOUND.**
  - The assign `31f8c600`, the uninitialized copy `31f8ce80` and LoadSoundData's inlined push `@31f74821-31f7489b` all copy only up to `+0x26`.
  - So every list entry's `+0x27` is whatever byte the storage held, and the play passes it as the 3D flag. The 6 Ain rows that set the key never get their value in June.
  - Port: `SoundData::operator=` skips the field under `JUNE_DAMAGEEFFECT_SOUND` (copy in `#else`). The `_DEBUG` play line now prints the raw byte (`3d=<n>`).
- **JUNE_BUGS 139: June passes the `+0x70` wstring *object* to `%s`** (`rep movsd` ×7, `add esp, 0x28`).
  - Port: `m_DeviceID` itself, not `.c_str()`.
  - Error path only (a failed `Set3DAttributes`).

**Found, new flag `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_NOTIFY_OWNER_FIRST`** (no-partial rule: same function):
- June's `~CEffect` starts with `NOTIFY_TO_OWNER_WHEN_DIE` (`+0xb13`, `@31f78584-31f785cf`), owner vslot `0x2c4`, before the stopper. March calls `DieDamageEffectProc` late in the dtor.
- The port moves the call to the head; the late block is in `#else`.
- June's argument is the `CEffect*`. Add (`329f73a0`, the only override) reads only its `m_DamageData` `+0x590` / `+0x4dc`, so March's `DamageData&` carries the same values.
- The signature and Add's second ID (`RNSI_A_AN_PHASE_SHIFT_UPGRADE` 29216) -> **checklist 7m** (D8).
- `#error` unless `NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE` (`Always.h:415`) is on.

**Hygiene** (`--worktree --files`):
- `JUNE_DAMAGEEFFECT_SOUND` (+ `_DEBUG`): 2 PASS, 1 OTHER (`X2DamageEffect.cpp` also holds the new flag).
- `NOTIFY_OWNER_FIRST`: 2 OTHER (`Always.h` peers' blocks; `X2DamageEffect.cpp` other flags).
- 0 FAIL, 0 non-ASCII, 0 encoding change; RESULT PASS both.
- The 2 deleted lines are C9h-8's own: the debug `3d=` argument and the `c_str()` argument.
- Range (`--range c3a723c^..HEAD`, both flags + `_DEBUG`): 5 PASS, 1 PASS~, 2 DEFINE, 21 OTHER, 0 FAIL; RESULT PASS.
  - Of the 5 deleted lines, 3 are peers' `Always.h` define switch-offs (riding-pet `_DEBUG` / `ENTRY_STATE`) and 2 are the C9h-8 lines this review replaced.
- Compiled form: `KTDXDeviceSound.obj` passes `m_DeviceID` as `rep movs` ×7 from `this+8`, a bitwise copy with no `basic_string` copy constructor, the same as June.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,411,968 B, dated 2026-10-08 23:19:46, md5 e2d031c8eba284e2b0188652816f4a24, equal to X2/US_SERVICE/x2.exe (2026-10-08 23:19:41); built at mods/june-buff-port 23c917a + 10 uncommitted code file(s)`.
- A peer's build compiled this tree at 23:10-23:13, after these edits (obj files newer than the sources); this run relinked nothing.
- The 10 files include the peers' in-progress `X2GUNPC` / `X2GUUser` / `X2Game` / `X2GameUnit` and the socket-cooltime block in `Always.h`.

**Notify move not play-testable yet** (user 2026-10-09: Add is not playable before its port): the only March row with `NOTIFY_TO_OWNER_WHEN_DIE` is `DamageEffect_Add_Phase_Shift`, and the only non-empty owner hook is Add's, so nothing reaches the moved call until the Add port (checklist 7m), which verifies it.

**Re-test PASSED 2026-10-09** (run 04:50, exe `e2d031c8`, jtCodeEmpress):
- 24 `play` lines, all `loop=0 dev=1 channel=1`, 0 push / stop / skip, 0 EXCEPTION.
- Covered: the AT XXZ4 chain, XX↓X bomb, X fireball, EP jump ZX (one of the 13 former LOOP rows; one play each).
- Raw `3d=` bytes 0 ×5, 55 ×8, 70 ×3, 83 ×8, never 1: JUNE_BUGS 138 as June has it.

**`_DEBUG` off** (`//#define`, 2026-10-09). `DEPLOY OK: X2_offline.exe is 19,411,456 B, dated 2026-10-09 05:03:10, md5 b37daed281a37d29f3cae865ff548250, equal to X2/US_SERVICE/x2.exe (2026-10-09 05:03:05); built at mods/june-buff-port 0cc7042 + 1 uncommitted code file(s)` (the 1 = this `Always.h` change). The exe has 0 `JUNE_FX DE_SOUND` strings and 1 `result : %d, File : %s`. **C9h-8 closed.**

## C10 - Decided absent systems

**Status: C10a done 2026-10-06 (identification, no code; Done note below). Building is split into C10b-1 (HP-rate),
C10b-2 (effect-alpha engine) and C10b-3 (effect-alpha UI: handed to the June UI port by D9).** **Model:** Opus, high, to identify; then
Sonnet 5, high.

- **D2 = build:** effect-alpha option. Port June's game option (the `AEACBO_*` enum is already in
  `Enum.lua`): the option storage, its UI, and the readers at the four parse sites.
- **D4 = build:** HP-rate damage, i.e. `HP_RATE_DAMAGE_TYPE`/`_VALUE` in June's damage path.
- **D5:** combination point, per C2's identification.
- **D3:** natural force. Only if the user chose to turn it on here.

Each gets its own flag. Rows the user declined become `declined` in the census, with the date.

**Exit:** `python june2017_re/census.py --final` has no failing line for the decided systems: every `unsupported` row is
built (its flag named in `census_completion.py`'s `SYSTEM_DISPOSITION` and defined in `Always.h`, tested by an `#if`),
handed-off or declined, and the HP-rate item-1 line passes. (Built systems stay `unsupported` in the census: code under a
`SERV_IRUHADEV_JUNE_*` flag is deliberately not evidence of a system.)

**Kickoff prompt:**
```
June2017 effect completion C10. Read JUNE2017_EFFECT_COMPLETION_PHASES.md (C0's decision table,
C10), the rebuild plan's Starting context and Phase 8c, funcmap.tsv. Build exactly the systems C0
says to build, each under its own SERV_IRUHADEV_JUNE_* flag, cited to June; mark declined ones in
census.py. CP949 byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

**C10a - Done note (2026-10-06, Opus 5.5, no code).** `flagcheck`, `build.py` and a play-test do not apply (the C0/C1a
precedent). 43 funcmap rows are tagged `Effect C10a`; they landed in peer commit `1d511ed` (june-buff 16-step3b),
which staged `funcmap.tsv` while this session was appending. The 265 call sites of June's alpha-tagging helpers, with a
disposition each, are in `june2017_re/c10a_alpha_sites.tsv`. D3 and D5 were already handed off, so C10 builds two systems.

*HP-rate damage (D4) goes to **C10b-1**, flag `SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE`.*
- `32273e80` is a new `CX2GameUnit::GetHpRateDamage( float* pOut, DamageData* )`:
  - type 1 (`HRDT_NOW_HP`): `*pOut = GetNowHp() * HP_RATE_DAMAGE_VALUE`;
  - type 2 (`HRDT_MAX_HP`): `*pOut = GetMaxHp() * HP_RATE_DAMAGE_VALUE`;
  - it writes only when the gage and its HP gage exist and now HP > 1.0 (strictly), otherwise `*pOut` is left alone;
  - it **overwrites** the damage; it does not add to it.
- It has two call sites:
  - `GetFinalDamage`, on `optrDefenderGameUnit`. It runs after the `(1 - this+0x7374)` multiply and before the
    `DTT_MULTIPLE_DAMAGE_BY_DEBUFF` consumer, then clamps the damage to >= 0.
  - `CX2GUUser::DamageReact`, after the additional defence/attack block (BBT 78).
- A user defender is therefore overwritten twice, and the second write discards whatever DamageReact took off in
  between; an NPC defender is overwritten once. Kept verbatim; C10b-1 owes the `JUNE_BUGS.md` row.
- The `GetFinalDamage` gate `322726f0` is false only for `RNSI_SA_ALP_SUPERSONIC` (29058) against some NPCs. That is
  Rena reform-skill material, so it goes to checklist 7c under D8; in March the call is unconditional.
- **Test vehicle.** No deployed templet uses HP-rate, and March's `Enum.lua` has no `HP_RATE_DAMAGE_TYPE` table
  (Phase 8c's Phase 11 handoff). C10b-1 stages one templet using raw ints (`TYPE = 2`, `VALUE = 0.1`).

*Effect-alpha option (D2) goes to **C10b-2** (engine, flag `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION`) and **C10b-3**
(UI, flag `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION_UI`, after D9).*
- **Storage** is on `CKTDXApp`, so KTDXLIB's renderers can read it:
  - three floats (my / my team / other team), default 1.0;
  - the setter clamps to [0.1, 1.0];
  - the getter returns 1.0 for categories 0 and 1.
- **Load and save.** `GameOptions.lua` carries `MY_EFFECT_ALPHA`, `MY_TEAM_EFFECT_ALPHA` and
  `ANOTHER_TEAM_EFFECT_ALPHA` (the decryptor and the save format agree), default 1.0.
- **Category** (`322d4d10`), from the effect's owner:
  - my unit or my summon -> 2;
  - same team -> 3;
  - otherwise 4;
  - 0 without a my-unit, an owner or `m_pGameEdit`.
  - A user override forces 1 (no alpha) in USI_START, USI_WIN and USI_LOSE, or USI_RIDING_WAIT_HABIT and
    USI_RIDING_WAIT while riding. June numbers these 9/11/12/75/76; translate by name, not by number.
- **Readers.**
  - The EffectSet factory and `OnFrameMove_CreateEffect`, which hands the category to the damage effect, mesh or
    particle.
  - CEffect: the ctor (0 -> category), OnFrameMove (main mesh, every frame), `SetParticleData`, and `AttackResult`
    (the hit effect set, an unconditional overwrite).
- **Render.**
  - The mesh `OnFrameMove`, plus its two slash traces.
  - The particle mesh-draw and vertex paths. The blend rule is `31ceefb0`.
  - Owned NPCs: their body and `CX2GameUnit::Weapon` fade. Users return category 0, so a user's body and weapon never do.
- **Lua binds.** `CreateApplyAlphaParticleSequence_LUA` and `CreateApplyAlphaMeshInstance_LUA` have 49 uses in June.
  March's deployed `SMITH_CART.lua` already calls the first one, so that call fails today because March does not bind it.
- **Call sites.**
  - C10b-2 ports 16 common ones, plus 3 in june-buff files (the two buff displayers and PhantomSword).
  - 238 are class sites (Eve 167, Elsword 29, Aisha 22, Add 12, Rena 8). They go to checklist 7c.
  - 8 are CX2GUUser sites that sit in June-only blocks. Each is ported with its host.
- **Not ported:**
  - `31f64040`, which is dead in June;
  - `SetEffectAlpha_LUA`, which is an EffectSet fade, not this option.
- **Test on March data.** In `GameOptions.lua`, `MY_EFFECT_ALPHA = 0.3` fades my effects and
  `ANOTHER_TEAM_EFFECT_ALPHA` the monsters' effects.
  - The offline AI-party bots spawn as `NAT_ALLY` with my UID. `CreateNPC` then gives them me as owner
    (`SetOwnerGameUnit` under `DUNGEON_RANK_NEW`; C10b-2 confirms with the preprocessor).
  - So they count as my summons: their effects, bodies and weapons all fade with `MY_EFFECT_ALPHA`, as June does for
    summons.
  - `MY_TEAM_EFFECT_ALPHA` then has nothing to show in solo offline play.

*Size.* D2 called this small. It touches CKTDXApp, GameOption, CX2Game, user, NPC, Weapon, CEffect, EffectSet, mesh,
particle, slash trace and two buff displayers, hence the three-way split.
*Census.* C10b-1 registers its flag in `census_completion.py` (`DAMAGE_SOURCES` and `SYSTEM_DISPOSITION`); C10b-2
registers the alpha flag in `SYSTEM_DISPOSITION`.
*D9, answered the same day:* engine now; the UI goes to the June UI port (backlog Stage 9). C10b-3 is not run here.

**C10b-1 - Done (2026-10-07, Opus 5.5 high, user OK'd Opus for a Sonnet phase; flag `SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE`; play-test PASSED 2026-10-07).**
Commit `4ca07e7`. `CX2GameUnit::GetHpRateDamage` = June `32273e80` (type 1 now HP, 2 max HP, times the value; only while now HP > 1.0;
overwrites). March's `CX2GageData::GetNowHp` returns 0.0 without an HP gage, so `gage && now > 1.0` is both of June's existence checks.
- *GetFinalDamage* (Ghidra, `32284aa0`): the my-HP-few multiply is `@322868d1..32286968`, the HP-rate site `@3228696a..32286990` (the
  funcmap's `32286930` start is inside the multiply), and March's existing `if( <= 0 ) = 0` is June's clamp `@32286995..322869aa`. The call
  sits between them, on the DamageData's defender, unconditionally (the SUPERSONIC gate `322726f0` is checklist 7c, D8).
- *CX2GUUser::DamageReact*: June runs the provoke zero, the attacker-class hooks, `322cfca0`, an execute block, then HP-rate `@32049727`,
  then the attacker's vslot `0x304` and the shown-damage copy (`@3204975b`). March has only the first of those, so the call goes
  right before `fFinalDamageToShow`: the number shown is the post-write value, as June. Double overwrite kept -> JUNE_BUGS 93.
- *Not owned by any row* (standing rule -> new phase **C10d**, Opus, identify): June's DamageReact steps between the BBT 78 block and the
  HP-rate call (`0x6d4` / `0x4f8` / `0x4fc` attacker hooks, a class-0xa multiply, `322cfca0`'s x0.5, the `+0xbbc` execute block, the
  attacker's `0x304`), user and NPC side. Kickoff `kickoffs/june-effect/C10d.md`.
- *Parity (kickoff item 5):* March drops no zero-damage hit before `GetFinalDamage` (no gate reads `damage.fPhysic/fMagic`; the only
  gate is a missing `REACT_TYPE` -> `RT_NO_DAMAGE`). All 7 of June's HP-rate-only blocks set a damaging `REACT_TYPE` (2/4/5), so on
  March they deal their HP-rate damage, not 0.
- *Census:* `census_completion.py` names the flag in `DAMAGE_SOURCES` and `SYSTEM_DISPOSITION`; `census.py`: `PASS definition item 1,
  HP-rate damage ...: 0 of 7948 June DAMAGE_DATA* blocks get damage only from it` (matrix regenerated in the same commit).
- *Test vehicle:* TEMPORARY edit of `ARROW_OF_EXPLOSION` (Night Watcher's basic arrow) in `kom_contents/data036_kom/DamageEffect.src.lua`:
  `HP_RATE_DAMAGE_TYPE = 2`, `HP_RATE_DAMAGE_VALUE = 0.1` inside `DAMAGE_DATA`, `PHYSIC = 0.5` kept, marked `C10b-1 TEST` with the restore
  steps. The staged source compiled byte-identical to the staged bytecode before the edit. `PACK OK data036` (~1 changed:
  `DamageEffect.lua`); `COPY OK data036` (md5 f040c412, user yes). Its boom (`ARROW_OF_EXPLOSION_BOOM`) has its own DAMAGE_DATA: one line per arrow hit.
  **Revert the edit + repack after the play-test.** The console `jC` route was not used (never proven; T3 crashed in that path).
- *Log:* `JUNE_FX hp-rate def=<user|npc> type value now max damage <before> -> <after>` in `offline_server.log`, first 60 writes
  (`JUNE_FX_DEBUG`); unbracketed, so read it with `python tools/logs.py --grep "JUNE_FX hp-rate"`.

Hygiene: `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted
line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` /
`RESULT: PASS` (`--range 4ca07e7^..4ca07e7`).

`DEPLOY OK: X2_offline.exe is 19,313,152 B, dated 2026-10-07 17:40:43, md5 30a7d90c9eea6c7b5d5c2b80f463d20b, equal to X2/US_SERVICE/x2.exe
(2026-10-07 17:40:38); built at mods/june-buff-port 2751f9c + 5 uncommitted code file(s).` (a peer's build compiled these edits; the
UTF-16 string `JUNE_FX hp-rate def=%s` is in the deployed exe).

**How to see it in game:** NWatcher (Night Watcher, class 114, Lv 70) hits a monster with basic X/Z arrows: each arrow hit takes 10 % of
the monster's max HP, and the log line's `->` value equals `max x 0.1`. The number on screen can differ: the NPC's own DamageReact
still acts on the damage after `GetFinalDamage`.

**Play-test result (2026-10-07, logs read): passed.** Run 17:47-17:49 on exe 30a7d90c, 0 EXCEPTION, clean exit. Four
`JUNE_FX hp-rate def=npc type=2 value=0.100` lines, each `->` exactly max HP x 0.1: `max=5224.0 damage 479.8 -> 522.4`, then three on
`max=4608.0` (`452.1 / 450.9 / 474.7 -> 460.8`). The arrow edit was reverted afterwards: the recompiled `DamageEffect.lua` is
byte-identical to the pre-test one, `PACK OK`, `COPY OK data036` (user yes) md5 9f754689 = C9e's, so the live kom is the pre-test archive.

**C10b-1 - Review (2026-10-07, Opus 5.5 high): PASS, no code findings, nothing rebuilt.** All checks ran read-only in Ghidra against `4ca07e7`.
- *GetHpRateDamage `32273e80`*: matches instruction for instruction. Type 1 calls `31e4f900` (GetNowHp), type 2 calls `31e551a0` (GetMaxHp);
  both multiply by DD `+0x5f8`. The gate reads the protected now-HP at HP gage `+4` (the same field `31e4f900` reads) with `fld1/fcomp`,
  `test ah,5`, `jp`: a write only for now > 1.0, never on equal or NaN. It overwrites and has no other branch. March's
  `CX2GageData::GetNowHp` returns 0.0 without an HP gage (`X2GageData.h:200`), so the March gate equals June's two null checks.
- *Callers*: `xrefs_to 32273e80` = `@32049727` and `@32286990` only. No NPC or class-override site exists, so JUNE_BUGS 93's "NPC written once" holds.
- *GetFinalDamage*: `@3228696a` is reached from every exit of the my-HP-few block (`@322868e9/32286901/3228692a/32286966`). The
  defender comes from DD `+0x470` (`31e55340`), then gate `322726f0` (checklist 7c, row present), call `@32286990`, then clamp
  `@32286995` (0 unless 0 < damage). The March block is at function scope, directly after `#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY`. The clamp
  differs only for NaN: June writes 0, March's `<= 0` keeps NaN. That clamp is March's own code, not this flag's.
- *DamageReact*: June's order is BBT 78 add, provoke zero `@32049565`, attacker-Add vslot `0x6d4` `@320495ca` (Add `329e25b0` = March's
  `SERV_9TH_NEW_CHARACTER` `SetStasisfieldHitValue`), Elesis `0x4f8/0x4fc` (by value; only `CX2GUEL` overrides), class-0xa multiply, `322cfca0`,
  execute block, HP-rate `@32049727`, then vslot `0x304` (only Rose overrides it, `32aa9c70`; empty for User/Add) and the shown copy `@3204975b`.
  March's call sits after the stasis block and before `fFinalDamageToShow`, which is June's order for the steps March has. Enclosing gate:
  March `X2GUUser.cpp:9297` `IsDamagedReaction && !m_bNoDamage` = June `@320483a4..320483bc` (`3226de80`, DD byte `+0x4dc`).
- *JUNE_BUGS 93*: the row is correct. It now also names the three June-only steps the second write discards (C10d) and the xref proof.
- *Census*: `PASS definition item 1, HP-rate damage ...: 0 of 7948 June DAMAGE_DATA* blocks get damage only from it`. The 0 counts blocks
  whose only source has *not landed*; the 7 HP-rate-only blocks are covered by the flag. The regenerated matrix/tsv changed only line numbers
  from peers' edits and were left uncommitted.
- *Test vehicle*: no `C10b-1`/`HP_RATE_DAMAGE` line is left in `kom_contents/data036_kom/DamageEffect.src.lua` (only studio `DRAIN_HP_RATE`); the live
  `data036.kom` md5 is `9f7546891c60793f7216767a52ad2398`, the pre-test archive.

Hygiene (`--range 4ca07e7^..HEAD`): `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR` /
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag
token, 0 encoding change(s)` / `RESULT: PASS`. Both OTHER rows are peer commit `b3ed2a3` (C9e review: the emotion blocks were moved in
`X2GUUser.cpp`, 5 lines in `X2SquareUnit.cpp`, under `JUNE_EMOTION_EFFECT_SET` / `JUNE_UNIT_VIEWER_EFFECT_TABLE`). It has no `HP_RATE` lines, and its
-49 are that move, not this flag's. Not checked: a user defender in play (the play-test produced only `def=npc` lines). The code path was verified by reading instead.

**C10d - Done (2026-10-08, Opus 5.5 high; identification, no code).** There is no flag, build or play-test, as in C10a. The work was read in
Ghidra, in raw bytes and with `vtslot.py`. 15 funcmap rows are tagged `Effect C10d`. Each step between the BBT 78 add and the
shown-damage copy is listed here with its owner, in June's order.

*User `DamageReact` (`320462a0`).* Every step runs at function scope, because the level-damage skip `@3204940c` lands on the provoke zero.

| June | Step | March | Owner |
|---|---|---|---|
| `@32049565` | provoke zero | has it (`SERV_ELESIS_SECOND_CLASS_CHANGE`) | - |
| `@32049572` | attacker Add vslot `0x6d4` = stasis hit value (Add `329e25b0`) | has it (`SetStasisfieldHitValue`) | - |
| `@320495d8` | attacker-user vslots `0x4f8` / `0x4fc( DD, dmg )`. CX2GUUser-only slots, empty in User; only Elesis overrides them | no | empty virtuals + call -> **C10e**; bodies -> checklist 7j |
| `@3204963d` | `UT_LUCIEL` (10) shadow shield: `32aebaf0( dmg x 0.2 )` true -> dmg x 0.8 (BTI 478/479, pool `+0x968c`) | no | checklist 7j (Lu/Ciel) |
| `@32049678` | `322cfca0`: x0.5 in **PvP** game type with PvP room `PGT_AMUSEMENT_ITEM` (8), for attack types != `AT_COMMAND` | no; inert on March data | **C10e** |
| `@3204968f` | attacker in the `CX2Game +0xbbc` list -> dmg = own max HP x 0.1, plus 2 s force/show invincible | no | backlog **Stage 19** (`DI_EVENT_2017_TREASURE`) |
| `@32049727` | HP-rate | C10b-1 | - |
| `@3204972c` | attacker vslot `0x304( dmg, DD )`. CX2GameUnit slot, empty for User and NPC; only Rose overrides it | no | empty virtual + call -> **C10e**; Rose body -> checklist 7j |

*NPC `DamageReact` (`327b1f30`).*
- The level-damage skip `@327b421b` and the attacker-invalid skip `@327b42aa` both jump to `327b4484`, so the Add `0x6d4` and Elesis
  `0x4f8`/`0x4fc` calls sit inside that gate, as March's NPC stasis call does.
- Then, at function scope:
  - socket **2000145** (`@327b4484`), the cool-time cut on hit -> **C10e**:
    - user flag `+0x7ca8`, set in `SumSocketOption`;
    - a 3 % roll (`< 0.03`, strict), against 1 % in the text;
    - a 0.5 s internal cool-down `+0x7cac`;
    - `ResetLeftSkillCoolTimeByBuff( ST_ACTIVE, -5.0, false, BCT_FIX_VALUE, 1.0, true, 0 )`;
  - the `DI_EVENT_ALCHEMY_LAB` block (`@327b452f`) -> **Stage 19**;
  - `322cfca0` -> **C10e**;
  - attacker `0x304` -> **C10e**;
  - slot 6 (already ported).

*Corrections to the buff 9a review's list* (`JUNE_BUFF_PORT_PHASES.md` 3424-3433). That text is frozen, so the corrections live here:
- `322cfca0` is PvP, not "dungeon mode `+0xd8 == 8`". The gate is `GetGameType() == GT_PVP`, and `g_pData +0x6c` is the PvP room.
- "`+0x7ca8` buff apply" is socket 2000145's skill cool-time cut.
- "NPC 41350" is the dungeon `DI_EVENT_ALCHEMY_LAB`. At or below 20 % HP the damage becomes 3e9. Above 20 %, a branch the 9a list
  missed heals the NPC by 25 % of its max HP and sets its attack x1.5.
- The user "execute" block is the `DI_EVENT_2017_TREASURE` manager: a hit from a listed NPC costs 10 % of max HP. It is not an execute.

*Owners.*
- **C10e** (Sonnet 5.5 high; kickoff `kickoffs/june-effect/C10e.md`; flags `SERV_IRUHADEV_JUNE_DAMAGE_REACT_HOOKS` and
  `..._SOCKET_COOLTIME_ON_HIT`): the three empty hooks with their call sites, `322cfca0`, and socket 2000145's five sites.
- **Checklist 7j** (new): the Elesis `32b7beb0` / `32b898a0` bodies, the Lu/Ciel step plus `32aebaf0`, and the Rose `32aa9c70` body.
- **Backlog Stage 19** (new, "June event dungeons"): both `DI_EVENT_*` blocks, the `+0xbbc` manager (`32d5c830`..) and `CX2GUKiddayNPC`.
  March has neither dungeon ID; its highest is 41311.

*Open for the user:* nothing. One crash case is decided in the C10e kickoff. June reads a null attacker at `327b449c`, and March
keeps the null check; C10e owes a JUNE_BUGS row for it.

**C10e - Done (2026-10-08, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_DAMAGE_REACT_HOOKS` + `SERV_IRUHADEV_JUNE_SOCKET_COOLTIME_ON_HIT`; play-test PASSED 2026-10-09).**
Every step cites June by address. The test is on the socket half; the three hooks and `322cfca0` are inert on March data.

*Hooks (flag `..._DAMAGE_REACT_HOOKS`).*
- `CX2GUUser::OnAttackerDamageReact_Slot4F8` / `_Slot4FC( DD, damage )` are empty virtuals, June vslots `0x4f8` / `0x4fc` (body `33093ad0`; only Elesis overrides, checklist 7j).
  - User `DamageReact`: after the `SERV_9TH_NEW_CHARACTER` stasis block, at function scope (`@320495d8-3204963b`): attacker is a user with a unit.
  - NPC `DamageReact`: inside the `m_bLvDamage == false` / attacker-non-null block, right after March's stasis call. Ghidra confirmed `@327b4434-327b4482` falls through into `@327b4484`, and the two skips `@327b421b` / `@327b42aa` land on `327b4484`.
- `CX2GameUnit::OnDamageDoneToDefender_Slot304( damage, DD )` is an empty virtual, June slot `0x304`, called on the attacker. User: after the C10b-1 HP-rate block, before `fFinalDamageToShow` (`@3204972c`). NPC: after `322cfca0`, before `DoCustomFunctionByBuffInNpcFinalDamage` (`@327b4800`).
- `CX2Game::JunePvpAmusementHalveDamage( DD, float* )` = `322cfca0`: DD and `g_pData` set, `GetGameType() == GT_PVP`, PvP room `GetPVPGameType() == 8` (`JUNE_PGT_AMUSEMENT_ITEM`, June `Enum.lua:736`), `attackType != 2` (`JUNE_AT_COMMAND`, June `ATTACK_TYPE`) -> `*= 0.5f`. Both are plain `int` constants inside the function: `AT_COMMAND` exists in March only under `SERV_IRUHADEV_JUNE_BUFF_ENUM`, and March's `PVP_GAME_TYPE` ends at `PGT_TAG`. User: after the hooks, before the HP-rate block (`@32049678`). NPC: before the `0x304` call (`@327b47a4`).

*Socket 2000145 (flag `..._SOCKET_COOLTIME_ON_HIT`).* Public `CX2GUUser::m_bJuneSocketCoolTimeOnHit` (+0x7ca8) and `m_fJuneSocketCoolTimeOnHitCool` (+0x7cac).
- ctor sets false / 0.0 (`@3203e928`).
- `UpdateSocketDataAndEnchantData` clears the bool between `ClearAllDelegateProcess` and `InitSocketData` (`@3204b5a0`).
- `SumSocketOption` sets it when `kSocketData.m_ID == 2000145`, just before the cool-time cut block (`@3204fff5-32050002`).
- `OnFrameMove` ticks the cool-down by `m_fElapsedTime` (June `+0x2b0`, same `+0x2b0` as C4's division window) and clamps at 0, before the start-motion block (`@3207001b-32070053`).
- Consumer in NPC `DamageReact`, function scope after the gate, before `322cfca0`: user attacker with a unit and the bool set -> roll `GetRandomFloat( 0 )` on the **attacker** (vslot `0x1cc` on `EDI`) `< 0.03` as a **double** (`3344f248`), cool-down not above 0 -> `ResetLeftSkillCoolTimeByBuff( ST_ACTIVE, -5.0f, false, BCT_FIX_VALUE, 1.0f, true, 0 )`, then cool-down = 0.5. Pushed arguments `@327b4507-327b451d` read back as 6 / -5.0 / 0 / 1 / 1.0 / 1 / 0, which are `ST_ACTIVE` = 6 and `BCT_FIX_VALUE` = 1 in March.
- `JUNE_BUGS.md` **136** (code 3 % vs text 1 %, kept) and **137** (June's no-null-test read at `327b449c`; March keeps its null test).
- Log (`JUNE_FX_DEBUG`, `offline_server.log`): `JUNE_FX socket-cooltime flag set user=... socket=...` (first 20) and `JUNE_FX socket-cooltime roll user= roll= cooldown= proc=0|1` (every proc, plus the first 5 misses).

Hygiene (`flagcheck --worktree --files <my 7 files>`): `DAMAGE_REACT_HOOKS`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 4 OTHER (the socket flag's lines in the same files), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. `SOCKET_COOLTIME_ON_HIT`: `0 PASS, 4 OTHER, 0 FAIL` / same zeros / `RESULT: PASS`. Range (`--range 19584d0^..19584d0`): the same counts, `RESULT: PASS` for both flags (the 4 OTHER rows are each flag's lines in the other's files).

`DEPLOY OK: X2_offline.exe is 19,411,968 B, dated 2026-10-08 23:19:46, md5 e2d031c8eba284e2b0188652816f4a24, equal to X2/US_SERVICE/x2.exe (2026-10-08 23:19:41); built at mods/june-buff-port 23c917a + 10 uncommitted code file(s).` Both `JUNE_FX socket-cooltime` strings are in the exe (UTF-16, 1 hit each).

**Test vehicle (a save edit, backed up).** Accessory `100002826` "Elsword's Mysterious El Lord Chronicle" (sockets `1601,1501,5211,2000145`, `EP_AC_WEAPON`, use level 0) now sits in the Accessory bag of **jtLordKnight** (unit 25, Elsword's Lord Knight, Lv 80), item_uid 1336. Backup: `db_backup/els_db_20261008_c10e_pre_socket_accessory.sql`. To undo: `delete from item where item_uid = 1336`. `100002827..33`, `100003886/7` are the same item for the other classes.

**Play-test (a 3 % roll needs many hits).** jtLordKnight: equip the accessory from the Accessory tab (double-click it; it is an `EP_AC_WEAPON` accessory), enter any dungeon with a monster that survives many hits, cast **Endurance** (active, slot 4 in the save) first so its cool-down is visible, then hit with normal attacks and Sonic Blade. Expect about one proc per 33 hits: the active skills' timers jump down 5 s at once. Log: `python tools/logs.py --grep "JUNE_FX socket-cooltime"` -> one `flag set` line at the unit load, at least one `proc=1`.

**Play-test result (2026-10-09, logs read): passed.** Run 05:05-05:09 on the deployed exe, 0 EXCEPTION, clean exit (the 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are not this phase's). jtLordKnight, accessory 100002826 equipped:
- `JUNE_FX socket-cooltime flag set user=25 socket=2000145` at each unit load (05:06:27, and twice at 05:08:26-27).
- Rolls: five misses logged, all `> 0.03` (0.5475 / 0.4603 / 0.5745 / 0.6661 / 0.7183), then three `proc=1`: `0.0069` (05:08:42), `0.0284` (05:09:31), `0.0112` (05:09:34). Each had `cooldown=0.00`, and the last two procs are 3 s apart, past the 0.5 s window.
- The in-game 5 s drop on the active skills' timers was the user's check ("test passed").
*Open for the user:* nothing. Review kickoff: `kickoffs/june-effect/C10e-review.md`.

**C10e - Review (2026-10-09, Opus 5.5 high): PASS. Two edge-case transcription fixes and one comment fix, all under `..._SOCKET_COOLTIME_ON_HIT`. Rebuilt.**
Every check below was read-only in Ghidra against `19584d0`; the constants were read from `unpacked_x2.exe`.
- *User hooks*: the order matches June exactly:
  - `0x6d4` (`@320495c4`), then the attacker gate (`31e55680` valid, `+0x264 == 0`, unit `+0x867c`);
  - then `0x4f8` and `0x4fc`, each pushed as ( EBX = DD, damage ) (`@32049613-3204963b`);
  - then Lu/Ciel (not ported), `322cfca0( DD, &damage )` `@3204968a`, the 2017 event block (not ported), HP-rate `@32049727`;
  - then the attacker `0x304( damage, ESI = DD )`, skipped only for a null attacker (`@3204972c-32049750`), and the shown copy `@3204975b`.
- *NPC hooks*: inside the gate: `0x6d4` `@327b442c`, then `0x4f8` / `0x4fc` with ( EBX = DD, damage ) (EBX is DD: `[EBX+0x57c]` is the `m_bLvDamage` skip). Both skips land on `327b4484`.
  At function scope, in this order: socket `@327b4484-327b452f`, ALCHEMY_LAB (Stage 19), `322cfca0` `@327b47cd`, `0x304( damage, DD )` `@327b4800`, slot 6 `@327b480a`. March has the same order.
- *`322cfca0`*: matches instruction for instruction:
  - DD and `g_pData` null tests, then `GetGameType` vslot `0x10c` `DEC EAX` (= `GT_PVP` 1, March `X2Game.h:61`);
  - PvP room `g_pData +0x6c` null test, then `+0xd8 == 8`;
  - `DD +8 == 2` -> return, then `FMUL double 3342ebc8` = 0.5.
  - March's `GetPVPRoom()` is a plain `return m_pPVPRoom`. March's `NULL != g_pX2Game` guards at both call sites are an addition: June calls through the game getter with no test. Harmless, because DamageReact runs only in a game.
- *Socket 2000145*:
  - ctor `@3203e928`: byte = BL, and BL = 0 (`XOR EBX,EBX` `@3203e8a2`); float = 0.0.
  - Sum `@3204fff5`: `[EBX+4] == 0x1e8511`. It is the last per-socket step before the cool-time cut, after June's `+0x896c` push, and March has it inside the same loop.
  - Tick `@3207001b`: runs only when `0 < cool` (NaN skips); subtracts `+0x2b0`; clamps when the new value is `< 0`. Same as March.
  - Consumer: roll first, cool-down second (March also rolls on every hit). The pushes read back as ( 6, `33476cd8` = -5.0f, 0, 1, 1.0, 1, 0 ), with `ST_ACTIVE` = 6 and `BCT_FIX_VALUE` = 1 (`X2BuffFactor.h:206`). The cool-down is set to `3343777c` = 0.5f.
- **Fixed (`X2GUNPC.cpp:10321`)**:
  - (1) `3344f248` is the double `0.029999999329447746`, which is `0.03f` widened, not `0.03`. March's `fJuneRoll < 0.03` let a roll equal to `0.03f` proc where June does not, so it now reads `< 0.03f`.
  - (2) June's cool-down test is `FLDZ; FCOMP [+0x7cac]; test ah,1; jnz skip`, so a NaN skips. `false == ( 0.f < cool )` let a NaN proc, so it now reads `0.f >= cool`.
  - Neither case occurs in play, so the play-test result stands.
- **Comment fix (`X2GUUser.cpp:30500`)**: June's reset `@3204b5a0` sits *before* `3207bfe0`. `3207bfe0` is `ClearAllDelegateProcess`: it clears 5 vectors to March's 4, and the extra one is June's. March's placement was already right. The comment, and the Done note's "between ClearAllDelegateProcess and InitSocketData", had the order backwards.
- *JUNE_BUGS 136 / 137*: both correct. 137: `@327b449c` `XOR EAX,EAX` then `CMP [EAX+0x264]`.
- *Log* (run 05:05-05:09, exe b37daed2 = C9h-8 review deploy, which contains `19584d0`): re-read with `logs.py`; the lines match the Done note. 0 EXCEPTION, clean exit.

Hygiene, worktree (`--files X2GUNPC.cpp X2GUUser.cpp`):
- `SOCKET_COOLTIME_ON_HIT`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `4 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The 4 deleted lines are this flag's own C10e lines, replaced by the fix.
- `DAMAGE_REACT_HOOKS`: `0 PASS, 2 OTHER (the socket flag's fix), 0 FAIL`, `0 deleted`, `RESULT: PASS`.
- Range `19584d0^..HEAD` (after commit `f7d1a2d`):
  - `DAMAGE_REACT_HOOKS`: `files: 3 PASS, 0 PASS~, 0 DEFINE, 11 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
  - `SOCKET_COOLTIME_ON_HIT`: `files: 2 PASS, 0 PASS~, 0 DEFINE, 12 OTHER, 0 FAIL, 0 ERROR` / `4 deleted line(s)` (f7d1a2d replacing 19584d0's own lines), others 0 / `RESULT: PASS`.
  - OTHER rows:
    - each flag's lines in the other's files (19584d0, f7d1a2d);
    - Always.h (both defines);
    - peer commits `2f78eb0` / `d269580` (C9h-8 review: Always.h, KTDXDeviceSound.cpp, X2DamageEffect.h/.cpp), which have no lines of either flag.

`DEPLOY OK: X2_offline.exe is 19,411,456 B, dated 2026-10-09 05:19:24, md5 ef524c3e71fc40c32af8f5c2f0a27613, equal to X2/US_SERVICE/x2.exe (2026-10-09 05:19:19); built at mods/june-buff-port 61e768f + 2 uncommitted code file(s).`

*Owed:* nothing (the optional regression run was skipped, user 2026-10-09). Next: C10b-2 (`kickoffs/june-effect/C10b-2.md`).


---

**C10b-2 - Done (2026-10-09, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION`; log `JUNE_FX alpha` under `SERV_IRUHADEV_JUNE_FX_DEBUG`; play-test pending).**
June's effect-alpha option engine on March; the option UI stays with the June UI port (D9). 26 client files, 18 funcmap rows `Effect C10b-2`, JUNE_BUGS 147-148.
- *Storage / file:* `CKTDXApp` three floats (default 1.0), `SetEffectAlpha` clamps [0.1, 1.0] and stores 2 / 3 / 4, `GetEffectAlpha` gives 1.0 for 0 / 1. `GameOptions.lua`: `MY_EFFECT_ALPHA`, `MY_TEAM_EFFECT_ALPHA`, `ANOTHER_TEAM_EFFECT_ALPHA` read before the table closes, written last in it in June's layout.
- *Category (`322d4d10`):* `CX2Game::GetEffectAlphaCondition`: user mine = `IsMyUnit`, NPC mine = owner UID == my UID; mine 2, team differs 4, else 3; user override = 1 in `USI_START` and the win / lose states (riding: the two riding-wait states). funcmap corrects the earlier "vslot 50 = GetEffectAlphaCondition" rows: vslot 50 is the unit lookup.
- *Readers:* EffectSet instance byte (templet key, else owner category) + per-EFFECT hand-off (the key counts only as 1) to damage effect / mesh / particle; CEffect ctor, `OnFrameMove` (main mesh, every frame), `SetParticleData`, `AttackResult` (hit set, unconditional). The four rule-5 "unsupported" log lines are retired under the flag (old lines in `#else`).
- *Render:* mesh `OnFrameMove` (blend SRCALPHA / INVSRCALPHA unless INVSRCCOLOR / DESTALPHA, outline + colour alpha), both slash traces, particle mesh draw, particle vertex path (not RHW).
- *Owned NPCs:* `CX2GUNPC::SetOwnerGameUnit` stores the owner's category; NPC body (after the skin's Prepare, as June) and `CX2GameUnit::Weapon` fade. **AI party: `cl /P` (flagprobe) shows `DUNGEON_RANK_NEW`, `X2OPTIMIZE_UNITTYPE_BUG_FIX`, `GIANT_UNIT_GIANT_EFFECT_TEST`, `SERV_TRAPPING_RANGER_TEST` ON, so `SetOwnerGameUnit` is compiled and the bots (NAT_ALLY owned by my unit) are category 2: their bodies, weapons and effects fade with `MY_EFFECT_ALPHA`. That is verbatim June (summons).**
- *Helpers / binds:* five tag helpers, `CreateApplyAlphaParticleSequence_LUA` / `CreateApplyAlphaMeshInstance_LUA` (bound on `CX2Game`; fixes SMITH_CART.lua's call). Sites ported: CEffect/EffectSet readers, delayed-firing emoticons (`32298eb0`), NPC timed-event particle (`327975f9`), PhantomSword mesh, weapon-particle displayer, buff effect-set displayer (creator category), PlayEmotion unit-viewer set (`32043cf0`, set in place).
- *Not ported (host absent in March, ported with their host):* ApplyHyperModeBuff's TRANScendence sets (`32298c5b`), `3203e1b0` sit-item sets (`3203e56d` / `3203e754`), `3204b5e0` (PARANOIA / AMUSEMENT_ITEM_PRESS), `32011f50`, `32028790`, PlayEmotion's EmotionData path (`320443c5`); `31f64040` (dead), the 238 class sites (checklist 7c), `SetEffectAlpha_LUA`.
- *June quirks kept verbatim:* JUNE_BUGS 147 (slash-trace alpha compounds every frame), 148 (displayer overwrites with the creator's 0).
- Census: `census_completion.py` names the flag for "Effect-alpha option".
`BUILD OK` x2.exe 19,497,472 B; `DEPLOY OK: X2_offline.exe is 19,497,472 B, dated 2026-10-09 18:00:11, md5 153e7bab5aca8b64f82c514f7c837cc5, equal to X2/US_SERVICE/x2.exe (2026-10-09 18:00:06); built at mods/june-buff-port 11d0f38 + 27 uncommitted code file(s).` The strings `JUNE_FX alpha set=%s`, `MY_EFFECT_ALPHA` and `CreateApplyAlphaParticleSequence_LUA` are in the deployed exe.
**How to see it in game:** `GameOptions.lua` in the game dir now has `MY_EFFECT_ALPHA = 0.3` and `ANOTHER_TEAM_EFFECT_ALPHA = 0.3` (backup `db_backup/GameOptions.lua.pre_C10b2`). With jtLordKnight (class 100, Lv 80) in a dungeon with the AI party: my skill effects, the monsters' effects, the bots' bodies / weapons / effects are faded; my own body and weapon are not. After a clean exit `GameOptions.lua` still holds `0.300000` for both keys.
**Open:** none for the user except the play-test.
**Play-test finding (user, 2026-10-09): the AI-party bots' bodies faded (June's summon rule for NPCs owned by my unit).** New flag `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_PARTY_BOT_AS_PLAYER` (user chose: skills follow `MY_TEAM_EFFECT_ALPHA`): a bot (`IsOfflinePartyBotUID`) is never "mine", so its effects take the team category (3, or 4 if its team differs), and its body and weapon take category 0 (never fade). Off = June. `DEPLOY OK: X2_offline.exe is 19,497,472 B, dated 2026-10-09 19:30:07, md5 ff5316e79158e2efa2d24269528de2eb, equal to X2/US_SERVICE/x2.exe (2026-10-09 19:30:03); built at mods/june-buff-port b30d3d7 + 6 uncommitted code file(s).` Re-test: with the current `GameOptions.lua` (my 0.3, team 1.0) the bots' skills stay solid and their bodies do not fade; set `MY_TEAM_EFFECT_ALPHA = 0.3` and their skills fade.
Commit `bd6e73d`. Hygiene (`flagcheck --range bd6e73d^..bd6e73d`, with `JUNE_FX_DEBUG`): `files: 25 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 5 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The 5 tokens: 3 bare `#endif` closing the `#error` dependency checks in Always.h (house style) and 2 `#endif SERV_IRUHADEV_JUNE_FX_DEBUG` of the debug line.

**C10b-2 - Review (2026-10-09, Opus 5.5 high): PASS, no code findings, nothing rebuilt.** Commits `bd6e73d` (engine), `4378554` (AI-party flag), against
the kickoff and the June binary (Ghidra read-only). The deployed exe ff5316e7 (play-test PASSED) stands.
- *Category `322d4d10`*: vslot `0x1a8` "is mine" -> 2; else the protected team (`+0x278`) vs my unit's -> 4 if different, 3 if equal; then the owner's
  vslot `0x41c`. The whole block runs only with my unit, an owner and `g_pMain +0x43c`, as March. NPC "mine" (`3276b2b0`) = owner UID == my UID, with -1
  without an owner. That is March's `GetOwnerGameUnitUID()`.
- *User override `31fd8350`*: state id == 9, or (riding ? 75 : 11), or (riding ? 76 : 12) -> 1. March pairs `USI_WIN`/`USI_RIDING_WAIT_HABIT` and
  `USI_LOSE`/`USI_RIDING_WAIT` the same way.
- *Effect-set tag `32296f30`*: bByPointer -> `g_pData +0x9c`, else `322ceae0` = `g_pData +0xe0`. In March's `CX2Data` these are `m_UIEffectSet` (right after
  `m_pGameMinorXMeshPlayer` `+0x98`) and `m_GameEffectSet`. The port's `GetUIEffectSet()` / `GetEffectSet()` match; byte `+0x1bc` = the category if non-zero.
- *Lua binds*: the Lua-facing shape is June's thunk `31e59970` (bMajor, unit, char* name, pos, emitRate, trigger). It calls `322f2660` with emit.x / .y,
  drawCount -1, `(int)trigger.x` (ftol `@31e59aa0`), trigger.y, bCullCheck 1, latency 0.0, bNextFrame 0. `322f2660`'s trailing -1 is a June-only
  CreateSequence argument. That is March's `CreateSequence_LUA` call exactly; `g_pData +0x84` / `+0x88` = game major / minor particle. The mesh thunk
  `31e59b10` passes its args through as the port does. June scripts call both as (bool, unit, name, pos, v2, v2) / (bool, unit, name, pos, v3, v3, layer).
- *Slash trace `31d39320` @31d39402*: `GetEffectAlpha( +0x268 ) * stored alpha`, then the `- elapsed / +0x1e0` fade, then stored back. JUNE_BUGS 147 holds.
- *Buff effect-set displayer `32eb9c70` @32eba1f8-32eba2cb*: the byte starts at 1. Unless the skip flag `[esp+0x17]` is set, it becomes
  `322d4d10( g_pX2Game vslot 0xc8( uid +0xb0/+0xb4, +0xb8 == 1 ) )`. Then every handle's instance not already 1 takes it. vslot `0xc8` = `322d80a0`
  scans the unit list for UID and `GameUnitType (+0x264) == arg`, so `+0xb8` (March `m_bJuneCreatorNotUser`, i.e. `m_bIsMesGameUnitNpc`) selects
  GUT_NPC. The port's NPC / user lookup split is equivalent.
- *Render resets (March side)*: the NPC body (`*pRenderParam = m_RenderParam`), the weapon (copy of the owner's unfaded `m_RenderParam`) and the mesh (per-field
  copy from the templet) are all rewritten every frame before the multiply. Nothing compounds, and a weapon fades once, not squared.
- *Rows*: every `C10b-2` / `C10b-2 checks` / `C10b-2 (june-buff file)` row of `c10a_alpha_sites.tsv` is ported or recorded as host-absent (`32298c5b`,
  `3203e56d` / `3203e754`, funcmap row 2322), plus the `32043cf0` exception. No `with host` or `class code` row was ported. Census: `SYSTEM_DISPOSITION`
  names the flag (`census_completion.py:49`). The NPC timed-event (`327975f9`), delayed-firing (`322990b8/cc`), PhantomSword and the readers match their funcmap rows.

Hygiene (`--range bd6e73d^..HEAD`): `files: 27 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 4 OTHER (other flags), 0 FAIL, 1 ERROR` / `in this flag's
files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 3 #else/#endif without the flag token, 0 encoding change(s)` /
`RESULT: FAIL`.
- The ERROR is `4378554`'s Always.h `#if defined(..._PARTY_BOT_AS_PLAYER) && !defined(..._EFFECT_ALPHA_OPTION)` `#error` dependency guard. It is the
  documented tool limitation (C6b / C4 precedent), not a code finding. Over the dependency closure (`--with ..._PARTY_BOT_AS_PLAYER
  SERV_IRUHADEV_JUNE_FX_DEBUG`): `27 PASS, 2 DEFINE, 4 OTHER, 0 FAIL, 0 ERROR`, `0 deleted`, `RESULT: PASS`. The bot flag alone (`4378554^..4378554`):
  `2 PASS, 1 DEFINE, 0 FAIL`, `RESULT: PASS`.
- The 4 OTHER rows are peer `b69d2a2` (C8b fix-up, `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`, X2GUNPC.cpp / X2GUUser.cpp/.h / X2GameUnit.h, +/-82 block moves).
  None of them touches this flag's lines.
- The 3 tokens are the bare `#endif`s of `bd6e73d`'s own Always.h dependency guards (house style).
*Owed:* nothing. C10b-3 is handed off (D9); next in the plan: C11 (blocked on C2-C10, see STATUS).

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
