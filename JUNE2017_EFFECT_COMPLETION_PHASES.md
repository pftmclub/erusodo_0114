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

---

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
