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

**Status: ready (C3b done 2026-10-08). Model:** Sonnet 5.5, high, then an Opus review. Kickoff `kickoffs/june-effect/C3c.md`.

- **Scope:** C3b items 1 and 3 (funcmap rows `(C3b)`, `june2017_re/c3b_damage_react_sites.tsv`). New flag `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION`
  (parse, check, the 22 sites, the 6 FORCE tests, DOWN's `SetNowForceDown`); `SERV_IRUHADEV_JUNE_DAMAGEDATA` extended for the TRANSIT reader.
- **Not with:** C9h-4f or C8b (`X2GUUser.cpp` CommonStateStart / DamageReact areas).
- **Exit:** strings in the deployed exe; a regression play-test (get knocked down, launched and dragged up in a dungeon) with 0 EXCEPTION;
  census rows `ADD_MEMBER_COUNT*` / `FORCE_BACK_SPEED` leave "parsed only".

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

## C3d-3 - port: the bounding back-speed state

**Status: blocked on C3d-2, C9h-4f and C3c. Model:** Sonnet 5.5, high, then an Opus review. Kickoff `kickoffs/june-effect/C3d-3.md`.

- **Scope:** C3b item 2: the user RT_BOUNDING case, the three `USI_BOUNDING_*` states, the five `CX2GUUser` handlers, `m_vBoundingBackSpeed`,
  the last-attacker UID / unit type (`+0x40e0` / `+0x40e8`, written next to buff 9b's `+0x40dc` serial), the registration in March's nine
  class InitStates with June's state tables in their data036 scripts (+ `Enum.lua`, packed), and the NPC half (`32769060`, `3275d900`, `3275a070`).

## C3e - identify: June's user-sync packet extension

**Status: ready (found by C3d-1, 2026-10-08). Model:** Opus 5.5, high, no code. Kickoff `kickoffs/june-effect/C3e.md`.

- **Scope:** the 15 fields June's `KDYNAMIC_UNIT_USER_SYNC::Deserialize` (`32d5be30`) reads and March's packet lacks (bit `0x400000`, and
  bits `0x10000-0x200000` / `0x1000000-0x40000000`). For each: width, the June `SyncData` member it comes from and decodes into, its
  readers, and its reach offline (the packet is sent only with UDP peers).
- **Exit:** funcmap rows `Effect C3e`, a port phase per field with a March counterpart, owners for the rest.

---

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

**Status: C8a done 2026-10-06 (see below); C8b (collision + base/user/NPC layers) queued; C8c (per-class) handed off to the character ports (D8).** Phase 9 ported June's collision for `EffectToUnit` only.
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
