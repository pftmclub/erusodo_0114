# June2017 character port: checklist

**Read this before porting any June2017 character script**, whether it is a new character (Lu/Ciel,
Rose, Ain) or a June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis,
Add).

Written 2026-09-29, after Phase 8c of `JUNE2017_EFFECT_REBUILD_PLAN.md` (commit `39405b5`).

## Why this document exists

**The effect rebuild's census does not look at character scripts.**
- `june2017_re/census.py` is the exit test of every phase of the effect plan. It reads only June's
  `DamageEffect*` and `EffectSet*` files.
- Character scripts (`Elsword_SwordMan.lua`, `RAVEN_FIGHTER.lua`, …) feed the same
  `CX2DamageManager::SetDamageDataFromLUA`. They also go through parsers the effect plan never
  touches: unit states, skill templets and the rest.
- So a phase can pass the census while a key that only character scripts use is still unread.

**Every gap below fails silently.**
- A key March doesn't parse takes its default.
- An enum *member* March lacks becomes nil, and the default is used.
- A missing enum *table* aborts the rest of the file with only `KEM_ERROR73` in the log (plan,
  Phase 8c, "What a missing enum table does").
- None of these fail to compile, and none show an error on screen. The character just plays wrong.

**Rerun the evidence, don't trust these numbers blindly.** They are from 2026-09-29.

```sh
cd june2017_re && python charport_scan.py <scratch dir>
```

It copies the June scripts into `<scratch dir>` and never writes to the June directory. It needs
`census.tsv` to be current, so run `python census.py` first if any phase has landed since.

---

## 1. Blocker: June damage numbers come from a table this build does not have

**121 of June's character `DAMAGE_DATA` tables have no `DAMAGE` table at all.** They set
`NUMERICAL_DATA_INDEX` instead.
- June's `SetDamageDataFromLUA` override (`FUN_32632390 @3263fe41-32640054`) then writes PHYSIC/MAGIC
  (×0.01), PVP_RATE, TECH_POINT and HIT_ADD_MP from a per-skill numerical table.
  - It runs when `NUMERICAL_DATA_INDEX > 0` or `NUMERICAL_DATA_NAME` is set, **and**
    `APPLIED_SKILL_ID` or `DAMAGE_SKILL_ID` is set.
- This build has neither the table nor the override. Phase 8c only parses and logs the keys.
- **Ported as-is, each of these attacks does 0 damage.** PHYSIC/MAGIC default to 0.0.

| June script | `DAMAGE_DATA*` tables whose damage comes only from the numerical table | other `DAMAGE_DATA*` tables |
|---|---|---|
| `RAVEN_FIGHTER` | 25 | 70 |
| `Elsword_SwordMan` | 20 | 87 |
| `ELESIS_KNIGHT` | 20 | 69 |
| `Chung_Iron_Cannon` | 15 | 74 |
| `Luciel_Duo` | 15 | 67 |
| `Ara_Martial_Artist` | 14 | 49 |
| `Lire_ElvenRanger` | 12 | 65 |
| `Eve_Nasod` | 3 | 25 |
| `Luciel_Demonio` | 2 | 11 |
| `Rose_Gunner` | 2 | 17 |
| `Rose_Freyja` | 2 | 2 |
| `Arme_VioletMage` | 1 | 36 |
| `Rose_StormTrooper` | 1 | 4 |

**What porting needs first:** the numerical skill data system itself.
- The override is cited above.
- **Owned here since D7 (2026-10-06).** The effect-completion plan identified it (C1a) and handed the port
  to the character ports. Its C1b brief is `kickoffs/june-effect/C1b.md`. The C1a Done note in
  `JUNE2017_EFFECT_COMPLETION_PHASES.md` has the full findings.
- **Identified (C1a, all `confirmed` in `june2017_re/funcmap.tsv`, rows tagged "Effect C1a"):**
  - The data is a field of each June skill templet: `NUMERICAL_DATA<n>` in `NewSkillTempletVer2.lua`,
    parsed by June's `AddSkillTemplet_LUA` (`32216320 @322186f5`). It is not a separate file.
    - Each `m_Value` entry is one row.
    - `m_Pvp_Rate` defaults to 1; `m_Tech_Point` and `m_Hit_Add_MP` default to 0.
    - Each side array is clamped to its last entry.
  - The override (`3263fe41` row):
    - It needs a skill the attacker has **learned** (level > 0) and an equipped or user templet.
    - The level picks the row.
    - It writes PHYSIC and/or MAGIC as value × 0.01, plus PVP_RATE, TECH_POINT and HIT_ADD_MP.
    - Its March site is just before `EndTable` in `SetDamageDataFromLUA`.
- **It depends on June's skill reform.**
  - All 1,913 numerical blocks point at reform skill IDs (21000-32605).
  - March's `Enum.lua` names them, but March's skill file has none: its IDs run 100-8409.
  - No March character has learned one.
  - So the override only works once a ported character uses June's skill templets and skill tree,
    that is, June's `NewSkillTempletVer2.lua`. That file is a different skill system: 451 of 521
    shared templets changed, plus 1,825 June-only templets.
  - Per-block evidence: `june2017_re/c1a_numerical_blocks.tsv`. Templet diff:
    `june2017_re/c1a_skilltemplet_diff.tsv`.
- **June's damage scale differs from March's** (Aceldama 9.5 vs 2.33, Aiming Shot 14.82 vs 5.63), so
  record both values in `MODS.md`.
- **Three blocks** index past their skill's data and get a zero row. They need a `JUNE_BUGS.md` row when
  ported (names in the C1a note).
- Because this changes damage values, it goes under its own flag, and every value lands in `MODS.md`.
- **The engine side is ported** (skill-system-core S4, flag `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL`, reviewed 2026-10-18). Since S3 the
  character is on June's templets, but S3 refuses presses of reform skills that have no class state. The first port that adds
  those states is the first to reach the override. Its play-test must:
  - show a `[JUNE_SKILL] NUM use ...` line (`python tools/logs.py --tag JUNE_SKILL --grep "NUM"`) whose physic / magic equal the
    tsv's x0.01 column at that level;
  - then retire the short-lived `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` (its `Always.h` block and guarded code), in the same phase.
  - (Aisha: CH2a - the first live class: states for 22000-22009 and 22200 / 22201 and their upgrade ids; NUM lines matched the tsv at level 1 on 2026-10-21 and `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` is retired.)
  - (Aisha: CH2b - 22 of the 24 ids: Dark Cloud, Aging, Death Field, Hell Stone, Magical Makeup, Super Nova, Guillotine Press, Heavy Press, Dark Road, Mana Intake, Energetic Road and their upgrade ids; Energy Drain 22213 / 22234 waits for CH2c, which maps `322f0e60`. The effect-set modifier list gained tag 1 for the upgraded Energetic Road.)
  - (Aisha: CH2c - Energy Drain 22213 / 22234: READY / SUCCESS / FAIL are June's; its target search needs June's view cone `322f0e60`, now `CX2Game::JuneIsUnitInViewCone`, which Ain's Lied Schwach also calls (Stage 8).)
  - (Aisha: CH3a - 15 of the 18 state-bearing skill cases: Blaze Step, Cyclone, Chain Burst, Plasma Cutter, Phantom Breathing, Hell Drop (+ 11 control states), Dark Burst, Circle of Elements, Frost Barrage, Petit Angkor's Millions, Ruin (+ loop / end), Icicle Spear, Abyss Field, Circle Flame (+ state 186), Petit Angkor, with their upgrade ids. Lightning Shower 22012 / 22052 (unmapped `31c69af0`) and Fire Ring / Dark Ring 22224 / 22225 (unmapped `31ff5440`, the SUB_CAMERA sequencer of 7af) wait for CH3a2; Elemental / Shadow Body have no state (CH4).)
  - (Aisha: CH3b - Energy Spurt, Distortion, Screw Driver, Impact Hammer, Air Stunner (+ END 248), Crazy Worm, Impact Zone, Worm Hole, Energy Ring and their upgrade ids; the hyper actives' Inits are March's; Distortion's summon case `June_SetDistortionSummonInfo`; Energetic Body (22215 / 22236) has no state, CH4's.)

## 2. DamageData keys June's character scripts use that March does not parse yet

Matched by exact sub-path under the `DAMAGE_DATA*` table (`charport_scan.py`, section `KEYS`).
"Owner" is the effect plan's phase for that key.

| key | census status | owner | uses (June script × count) |
|---|---|---|---|
| `APPLIED_SKILL_ID` | unsupported (parsed, logged) | 8c, done | 167: every character script except `Add_NasodRuler`, `Ain_Angel`, `Rose_Mechanic` and `Rose_Ranger` |
| `NUMERICAL_DATA_INDEX` | unsupported (parsed, logged) | 8c, done | 132: see section 1 |
| `NATURAL_FORCE_TYPE` | unsupported (parsed, logged) | 8c, done | `Lire_ElvenRanger` ×77. See section 3 |
| `HIT_ADD_COMBINATION_POINT` | unsupported (parsed, logged) | 8c, done | `Luciel_Duo` ×6 |
| `ADDITIONAL_ATTACK_TRIGGER_TYPE` | to-implement | 8b group 3 | `RAVEN_FIGHTER` ×7 |
| `BUFF_FACTOR_ID_ULTIMATE` | to-implement | 8b group 2 | `Ara_Martial_Artist` ×3 |
| `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP` | to-implement | 8b group 1 | `RAVEN_FIGHTER` ×2 each |
| `IGNORE_DEFENCE_DECLINE` | to-implement | 8b group 1 | `ELESIS_KNIGHT` ×2 |
| `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, `…_RELATIVE_SKILL_ID` | to-implement | 8b group 2 | `Elsword_SwordMan` ×1 each |
| **`APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID`** | binary-only | 8b, **missing from 8b's resume groups** | `Elsword_SwordMan` ×1 |
| **`DAMAGE_RATE_TO_SUPERAMOR`** | **implemented, June effect completion C3** (parse + the `GetFinalDamage` multiply for a super-armored defender) | 8b, **missing from 8b's resume groups** | `Luciel_Duo` ×2 |

**The two bold rows are the trap that prompted this document.**
- The census calls them `binary-only`: no June *effect* file uses them.
- Phase 8b's resume point lists only `to-implement` keys, so they were not in any group. 8b's exit
  criterion doesn't mention `binary-only` either.
- June's character scripts *do* use them. **Do not close Phase 8b without both.**
- 8b's resume groups missed nine of its 13 `binary-only` rows. These two are used; the other seven
  (`APPLIED_MEMO_ID`, `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `DAMAGE_TRIGGER_CONDITION`,
  `HIT_ADD_MP_PERCENT`, `BUFF_FACTOR_ID_ULTIMATE_PVP`) are used by no June script at all. The same is
  true of group 1's four `binary-only` rows (`DIVISION_NUM`, `DIVISION_OFF_TIME`,
  `DIVIDE_DAMAGE_FROM_RANGE`, `RANGE%d`). All of them still belong to 8b under plan rule 3; no content
  depends on them.

`DAMAGE_SKILL_ID` (8b group 1) appears 103 times, all in `Elsword_DamageData.lua`, at that file's
top level. That file is the `InsertDamageData` scope, which the census marks `unassigned`: March has
no reader for it at all. It is a separate port item.

## 3. Systems a character needs that this build lacks or has switched off

| system | state here | who needs it | what porting needs |
|---|---|---|---|
| Numerical skill data | absent | every June character (section 1) | build it (section 1) |
| Natural force | code exists, gated off: `ADD_RENA_SYSTEM` is **off** under `US_SERVICE` | Rena (`Lire_ElvenRanger`, 77 `NATURAL_FORCE_TYPE` uses) | **a decision for you:** turn `ADD_RENA_SYSTEM` on (the studio's own Rena code, untested in this build) or leave Rena without natural force. Phase 8c's read steps aside by itself when the flag is on. The same flag also gates the first-attack / relate-skill copies (`DamageData::m_eFirstAttack` / `m_RelateSkillData`) in the EffectSet and every damage-effect spawner, which June has unconditionally (C6a2: funcmap `321ef5ea-321ef632`, `DD+0x5a4`) |
| Applied skill ID | absent: its June readers (`ParsingBuffFactorID`'s `_ULTIMATE` gate `31fcf3e0`; the numerical override) have no March counterpart | every character (167 uses) | comes with the numerical-data work and 8b group 2. The "absent" verdict depends on 8b identifying `31fcf3e0`; if it turns out to be March's memo path, revisit it |
| Combination point | absent | Lu/Ciel (`Luciel_Duo` ×6) | new-character system; **handed off here by effect C2/D5 (2026-10-06)**: reader `CX2GULuciel` `32b14bc0` (DD `+0x594` > 0, then `32b14860`), see `funcmap.tsv` |
| Rose weapon | absent: no `UT_ROSE`; `CX2Unit::UNIT_TYPE` ends at `UT_ADD` | Rose (every `Rose_*` script) | new-character system, plus the unit type itself |
| Lu/Ciel duo | absent (no `DUO_TYPE` enum) | Lu/Ciel, plus `CharCustomData`, `EmotionData`, `EventScene*`, `UnitViewerEffectSetTable` | new-character system |

New characters need more than this: a unit type, a unit class, a skill tree and server-side data.
All of it is outside this document, which covers only what the effect rebuild's findings reach.

## 4. Enum tables: one missing table cuts off the rest of the file

From `charport_scan.py`, section `ENUM`: June `Enum.lua` tables that March's `Enum.src.lua` and
`DungeonEnum.src.lua` lack, as indexed by character scripts. Add the whole table, with June's exact
values, before loading the script. Otherwise **every state after the first reference never loads**.

| table (June `Enum.src.lua` line) | values | characters | what is lost on today's Enum |
|---|---|---|---|
| `CUSTOM_RENDER_STATE` (17473) | `CRS_NONE = 0`, `CRS_SPECTOR_OF_THUNDER = 1`, `CRS_VERITABLE_PANDEMONIUM = 2`, `CRS_SPECTOR_WALK = 3`, `CRS_QUICKSILVER_FRENZY_BUFF = 4`, `CRS_INVISIBLE = 5`, `CRS_UNIT_FADE_WHITE_WITHOUT_WEAPON = 6`, `CRS_UNIT_FADE_WHITE = 7`, `CRS_UNIT_BLACK = 8`, `CRS_EMOTION_INVISIBLE = 9` | **Add** ×7, **Ara** ×1 (both exist in March); also `Ain_Apostasia`, `Ain_Erbluhen_Emotion`, `Luciel_Demonio`, `Rose_Ranger` | Add: top-level statements 272–328 of 328, from `ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START` on. Ara: statement 338 of 338 (`ASI_SA_ASR_FOX_PHANTOM`) |
| `DUO_TYPE` (17515) | `DT_MAJOR = 0`, `DT_MINOR = 1`, `DT_BOTH = 2`, `DT_KEEP = 3` | Lu/Ciel (`Luciel_Duo` ×390, `Luciel_Demonio` ×158) | new character |
| `ROSE_WEAPON_TYPE` (17636) | `RWT_REVOLVER = 0`, `RWT_AUTOGUN = 1`, `RWT_MUSKET = 2`, `RWT_HANDCANNON = 3`, and `RWT_END` with **no value**. As decompiled, it is a positional nil, so `ROSE_WEAPON_TYPE.RWT_END` is nil in June too. Check against the bytecode before transcribing | all five `Rose_*` scripts | new character |

- **The effect files have their own five missing tables.**
  `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`, `DAMAGE_EFFECT_DIE_CONDITION_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`,
  `HP_RATE_DAMAGE_TYPE` and `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` are listed, with values, in the
  plan's Phase 8c section. They are Phase 11's, but a character whose skills use June's
  `DamageEffect.lua` needs them loaded first.
- **The scan also flags other names that are in neither Enum file** (`INIT_PHYSIC`, `WT_XXXXX_1`,
  effect names, …). These are probably globals defined elsewhere or data quirks, not enum tables.
  Check each when that character is ported. They are not listed here because they are unverified.
- **Missing members of tables March *does* have** (for example the June `SKILL_ID`s) fail silently
  to defaults. The effect side's list is in the matrix's *Enum census*. The character side's has not
  been built yet.

## 5. What no scan here covers

- **Every non-DamageData key in a character script.** Unit-state keys, skill-templet keys, motion and
  bone tables and so on are read by June parsers nobody has mapped. The effect plan's Phase 0/1 method
  applies unchanged:
  1. identify the parser in Ghidra;
  2. decrypt its keys (`xordecrypt.py`);
  3. census them against March's preprocessed parser.

  **Do this per character port, before copying any script.** A `census.py` scope for unit scripts is
  the natural first step.
- **The files the source tokenizer rejects:** `NewSkillTempletVer2(_LA)`, the quest tables,
  `OptionItemData`, `PetTemplet` and `TitleTable`. Their bytecode *was* scanned by key name
  (`KEYS-BYNAME`), but not by path.
- **Server side.** New characters need server data and packets under `KncWX2Server/Common/`, which
  can't be built here (VS2003 is not installed). Plan rule 6 applies: stop and record it.

## 6. Order of work before the first character port

1. Finish Phase 8b, **including the two bold keys in section 2**.
2. Phase 11's enum-table additions (the effect files' five tables), plus the section 4 tables for the
   character being ported.
3. The numerical skill data system (section 1), together with June's skill templets and skill tree it
   depends on. Without it, June character damage is 0.
4. Decide on `ADD_RENA_SYSTEM` (section 3) before touching Rena.
5. A per-character parser census (section 5).
6. Only then bring in the character's June script with `python tools/luasrc.py adopt-june <kom> <Class>.lua`
   (default: June's own bytecode via `ljconv`; `--dry-run` first), and rerun `charport_scan.py` to confirm nothing new
   appeared. If the port must edit the script, first add the core-list row
   (`luasrc.py core --add <kom> <Class>.lua --origin june --why "edit: <phase> <what>"`, no question), then `adopt-june --source`
   (see `LUA_SOURCE_PHASES.md` Rule 2, skill `x2-adopt-june`). Never copy from `june_src_fixed/` or compile by hand.

- **Pilot close-out (2026-10-22).** Aisha (the Stage 5 pilot) finished: every section-7 row she owned carries an `(Aisha: CHn)` mark in place. Stage 7 characters mark their rows the same way, `(<Name>: CHn)`, in the phase that ports them. The recipe lessons and the character order are in `JUNE_CHARACTER_AISHA_PHASES.md` "Template for Stage 7" and `kickoffs/june-backlog/stage7-plan.md`.

## 7. Buff-engine hooks that only character code reads (from the June buff port, 4b-i part 3)

The buff port (`JUNE_BUFF_PORT_PHASES.md`, "Phase 4b-i, part 3") ports the buff side of these and
stops at the character boundary. Each item below is June character code that reads or drives a
buff-side store. Port it with the character, whole, and wire it to the store named here.

**Buff blockers cleared (june-buff Phase 14, 2026-10-07).** The buff engine and June's buff data are
in: every June BBT/BFT/BDT has a handler, and `data036` ships June's `BattleFieldBuffTemplet`/`Factor`
(1001 templets, 1130 factors + 69 `_INT`, 0 placeholders) and the Enum gaps (`SKILL_SLOT_TYPE`,
`UT_LUCIEL`, `ST_SPECIAL_ACTIVE_GRADE_1-3`). A June character script's `BUFF_TEMPLET_ID`/`BUFF_FACTOR_ID`
now resolves and applies; the sweep played one buff per group (stat, skill type, flag/immunity, the new
finalizers, custom functions, mob-applied, premium). What is left on the buff side for a character is
only this section's rows (the class-side hooks) and section 1's numerical data. The debug lines are
compiled out now (`SERV_IRUHADEV_JUNE_BUFF_DEBUG` off); re-enable it to see a ported class's first
apply/read lines.

| June | Class | What it does | Buff-side store in March |
|---|---|---|---|
| `32bb22d0` / `32bb2740` (vslots 49/50, `HyperModeBuffEffectStart/End`) | Ara | Start: when `+0x9744 > 0` and the buff hyper count `< 3`, a loop over June-only members; End: writes the count back to 0 | `CX2GameUnit::GetHyperModeCountByBuff()` (June `+0x6844`). **March has both functions, June reworked them: port each whole.** |
| `32ba6cc0` (vslot 39), `32ba6d00` (vslot 147: count 1 -> state `0x294`, 2 -> `0x295`), `32badc80` region (vslot 485, 3 reads), `32baf270` (397), `32bc8be0` region (198), `32bc8030` (vslot 17, writes) | Ara | Read or write the buff hyper count | same |
| `32bf4680` (non-virtual, after vslot 465) | Chung | Skips its timer work when the buff hyper count is 3 | same |
| `329f538a` (inside the vslot-16 range) | Add | Reads the buff hyper count | same |
| `32b1e8e0` (vslot 16) | Luciel | Writes 0 at init | same |
| `3227c360` `(op, BTI, index, value)` | Rose only (`32ab2f02`, `32ab2f6d`, `32ae305b`, `32ae30ac`) | Finds the buff by BTI and calls its N-th behavior's vslot 9 (`+= v`), 10 (`*= v`) or 11 (`= v`); overrides exist on NowMp (9, 10) and ChangeStat (11) | callers only. The buff port owns `3227c360` and the three vslots and ports them dormant (`JUNE_BUFF_PORT_PHASES.md`, part 3, "Where part 3 stopped" item 5). **Ported dormant in 4b-i part 5:** call `CX2GameUnit::OperateBuffBehaviorValue( op, BTI, index, value )` (op 0/1/2 = slot 9/10/11) |
| `322a7660` `(BFI, count, level, x)` and wrapper `322a8a70` | Eve (`3299b7f9`, BFI 812), Elesis (`32b7e96c`/`32b7ebae`, 910/913), Rose, Luciel, Aisha (`32cc5c16`, BFI in a register), and `CX2GUUser` code for June-only factors 523/820/877/929 | Sets an existing buff's stack count; `3236a430` then re-runs every behavior's vslot 3 (NowMp, ChangeStat, EnchantAttackRate override it) | callers only. Every constant BFI pushed here is in Phase 2's June-only block. The buff port owns `322a7660`, `322a8a70`, `3236a430` and the vslot-3 overrides and ports them dormant (same item 5). **Ported dormant in 4b-i part 5:** call `CX2GameUnit::SetBuffAccumulationCount( BFI, count, level, x )` (`322a7660`) or `AddBuffAccumulationCount( BFI, delta, level, x )` (`322a8a70`). `x` feeds user slot 476 (`32028250`), a June-only factor adjustment that March logs and skips; June falls back to unit `+0x9274` when `x` is 0, a June-only member, so the caller must pass it (Aisha: CH1i, `June_ManaflowOverdriveFrameMove` 32cc5b20, `SetBuffAccumulationCount`) |
| `31fe8130` / `31fe8050` (user `AnimPlay`) | every user | June also plays two extra skin anims (`+0x8e84`, `+0x8e88`) and clears a third state byte (`+0x8080`, which June's `AnimStop` sets; `+0x8040` is March's `bSuperArmorNotRed`, part 3 review R1). No March-equivalent path reads `+0x8080` (4b-i part 5 item 4) | the buff port plays March's two anims and clears March's two super-armor fields only |
| `32a3d1f0` (vslot 130, `GetPowerRate(BTI)`) | Ain | The only override of the BTI-taking power-rate getter that EffectSetWithDamage calls | none: March's `GetPowerRate()` takes no BTI |
| `3208e160` (unit vslot 152 = user byte `+0x8e90`), setter `3204e030`; second after-image `+0x6758` built on the skin anim `+0x8e80` (`@31feed3d`); second head frame (`31fcac20`); second `AFTER_IMAGE` table in `CommonStateStart` (`+0x852c`) | every user (`+0x8e90` copied from `CX2Unit +0xe40` at creation, `32d51e80`) | Picks a second after-image and head frame. June's `EnableAfterImage`/`DisableAfterImage` (`32275a30`/`322720f0`) take a mode: 0 = first, 1 = second, 3 = by vslot 152. The flag's meaning was not identified (4b-iii) | `CX2GameUnit::EnableAfterImage`/`DisableAfterImage`: 4b-iii ported the colour rules and always uses the first after-image. Port the mode argument and the second after-image with this system |
| `+0x7c80` "after-image on by buff": set by `3227b220`, cleared by `3227a2f0` and `31ff27c0`; read by `CommonFrameMove @32065c06` | every user | When a June-only `+0x941c` show/hide block ends, re-enables the after-image if a buff after-image is on | none (4b-iii hand-off) |
| After-image byte `+0x1d1` (saved enable): `31fd8250` saves and disables, then restores; `322727c0` sets it on both after-images; `3227a2f0` clears it on erase | every user | Suspend/restore of the after-images around a June-only state | none (4b-iii hand-off; `CKTDGXSkinAfterImage` has no such member) |
| `32bf4900` (`CX2GUChung::InitComponent`, the `INIT_COMPONENT` reader) and `32c16be0` (`CX2GUChung::InitPassiveSkillState`) | Chung | `32bf4900` reads the new key `COMMAND_HIT_ADD_MP_BONUS` (default 1.0; June's `Chung_Iron_Cannon.lua:284` sets 1.15) into protected float `+0x98c4`. `32c16be0`, right after the base call, does `SetPassiveMultiple( GetPassiveMultiple() * bonus )` on both 85 and 86. June rewrote both functions (`SIEGE_ANGLE`, `MOBILE_SIEGE_ANGLE`, `X_EXTRA_CANNON_BALL_DAMAGE_RATE`, passives for skills `0x6659`-`0x6731`); March's versions read none of it. Port Chung whole | `m_AdditionalRangeCommandMpGainRate` (85) and `m_AdditionalMeleeCommandMpGainRate` (86), Phase 5, through `SetPassiveMultiple` / `GetPassiveMultiple`. Acts only on `AT_COMMAND` hits (buff port Phase 5 review, R2) |
| `329e5b00` (`CX2GUAdd::UpNowDPValue`, vslot `0x2a4`) with its body `329e56c0` (inlines the dead `329e5180`), `329e5140`, and the formation-mode entry `32a08580` | Add | DP spend: `( 1 - m_fUsingDPValueChangeRate ) + BBT 76 result * 0.01`. DP gain: HyperCharge result + `( m_fGainDPValueChangeRate - 1 )`. Every spend is then multiplied by `1 - result(+0x6bb0)` (an additional-stat member no buff inserts into). The formation cost is `[+0x9620] * [+0x95f0]` with no decrease term, and the Fantasy Tracer refund comes from the DP actually spent. Add on this branch is incomplete; the user ports Add from June | 76: `GetHyperDurationPercent()`, already read in March's `UpNowDPValue` under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT` (review R1; superseded when Add is ported). HyperCharge: `GetHyperChargeSpeed()`. Everything else is Add-side (review R6) |
| `32a1ccb0` region, call `@32a1e0fd` (`CX2GUAdd::InitPassiveSkillState`, vslot 420) and `32a4f7a0` `@32a4fbef` (`CX2GUAin::InitPassiveSkillState`) | Add, Ain | `SetPassiveSum` on the BBT 92 (`CHANGE_MAXIMIZE`) member from skill ability `0xc4` (Add adds its own `+0x9680` running total first, through helper `32a2bbc0`) | **none yet: BBT 92 is deferred** (`JUNE_BUFF_PORT_PHASES.md`, Phase 5c Done note). When 92 lands, the member is a `CX2AdditionalStatByBuff`, so the port only calls `SetPassiveSum` on it |
| `32a7ece0` (unit vslot 121, `(DamageData*) -> float`) | Rose | Rose's override of the multiplier June's damage-variance roll `322788c0` applies to the attacker's MAXIMIZE (base `3208dd70` returns 1.0 for every other class): a skill-ability lookup keyed on `APPLIED_SKILL_ID` | **none yet: the roll and the virtual come with BBT 92**, deferred in Phase 5c |
| `32a07dc0` (Add), `32b1eef0` (Luciel), both vtable `+0x800`; `32a4df00` (Ain, non-virtual, 4 callers) and `32aad080` (Rose, non-virtual, 3 callers) | Add, Luciel, Ain, Rose | The class's skill-use function: sets the cooldown (`32893880`), then calls the cool-time helper `3235a710` (BBT 100) with the skill. The other 9 classes' copies are wired in March (Phase 6b); Add is not, because Add is ported from June | `CX2Unit::ApplySkillCoolTimeRateByBuff( skill )`, called right after the class sets the cooldown (`JUNE_BUFF_PORT_PHASES.md`, "Phase 6b -- Done") |
| `32976fe0` (non-virtual, caller `329b4ab1`) | Eve | Sets the cooldown of `RNSI_SA_ECN_PSYKER_KINESIS` (25235, a June-only Eve skill) and calls the cool-time helper `3235a710` | same |
| `31fc8200` (`IsSummonSkill`, 54 June skill ids) and the summon cases of `3230c1a0` that March lacks (NPC ids 1604, 1621, 1622, 1635, 1906, 1908, 1937, 1971, 2083, 2175, 2192, 2194, 2199, 2206-2214, 2269, 2270, 2397, 2398, 2525-2527, 4342, 4343, 5187, 5189, 5190) | Rose, Ain, Luciel, Elesis, and June's reformed skills of every class | June's skill ids are the reformed `RNSI_` numbering, so `CX2GUUser::IsSummonSkillByJune` (already ported) matches no March skill and the summon 98 cache stays 1.0. Each June summon case routes its power rate through `322ece70`, which multiplies 89 x 90 x the summon 98 cache | `CX2GUUser::GetSummonPowerRateByBuff( skill )`: multiply it into a new summon case's power rate, beside `GetAdditionalAttackByType( AT_SPECIAL )`, as Phase 6b did for March's 14 cases |
| Per-class code that writes the state power rate after `DoStateEndStart` (June: protected cache `+0x7cc8`, e.g. `32ad6ac0` multiplies it by `+0x974c`, `32b1cd20`) | every class | June's class code scales the cached rate, which already holds 89 x 90 x 98. March's Raven **assigns** it (`X2GURaven.cpp` `m_fAttackPowerRateShadowStepThrust`, `m_fShadowBusterPowerRate`, `m_fAttackPowerRateShadowStepPunisher`), which drops the buff terms for those states. Not checked in June (Phase 6b did not search the binary for these functions) | `m_NowStateData.m_fPowerRate` is the cache in March. When the class is ported, keep June's operator (`*=` or `=`) |
| `329e31e0` (Add), `32baafc0` (Ara), `32ac5e20` (Rose), `32b14cd0` (Luciel): unit vslot 201 (`+0x324`) | Add, Ara, Rose, Luciel | BBT 97 `CHANGE_MAX_CHARACTER_ADD_RESOURCE` calls it with 1.0 on the first apply. Add: DP = gage `+0x11c` x rate through vslot `0x2a4` (`UpNowDPValue`). Ara: n = (int)( max force power (gage `+0x120`) x rate ); when the June-only `CX2GUUser` protected int `+0x9644` is > 0, the force power over the max times `+0x9644` goes through unit vslot 60 (`3208dd00`), which calls gage vslot 3 with `(v, 1, 0)`, then June's `SetForcePower` (`32ba6ef0`, which adds a `+0x6d04` spend conversion March lacks); then, with skill 27417 (`RNSI_P_AYR_OPEN_MIND`) learned, vslot 143 `(27417, 0, 1)`. Rose and Luciel not decoded. Chung's override (`32bf4880`) is ported (buff port Phase 7b) | `CX2GameUnit::ChangeMaxCharacterAddResource( float )` (virtual, empty base). Override it in the class |
| `320072d0` and `3202b9a0`, called from the user `UpNowHp`/`SetNowHp` overrides (`3202fcf0`/`3202ff60`) when the HP would reach 0 | every user (skill passives) | Death prevention. `320072d0`: own unit only (vslot 103); BTI 436 or 926 (and not 673) -> force-invincible 1.0, vslot `0x744`, `+0x8e74` = 0x29, `+0x8e78` = 1, effect; skill 27409 learned with `+0x867c`->`+0xaf0` type 7 and `32bc5a50` -> the same with `+0x8e74` = 0x27; skill 23402 with the cooldown `+0x405c` <= 0 and an HP test -> force-invincible, vslot `0xf4`( 1.0 ), `+0x405c` reset, `+0x8e74` = 0x28. `3202b9a0`: own unit -> `32028ba0( 0x13, this, 0, 0 )` (a June trigger dispatcher), `+0x8e78` = 1. Before both, `UpNowHp` clears a `g_pData+0x68 -> +0x90 -> +0x8fc` vector. After the base, the own unit calls a low-HP UI `3289db00( 0xd, 0|1, -1 )`; `UpNowHp` also stores the previous HP at gage `+0x108`. **Crest of Time (buff Phase 11b):** vslot `0x744` (`3204da60`) restores the HP/MP snapshot and applies BFI 683 (`BFI_DEBUFF_TIME_BACK`, i.e. BTI 673). The snapshot is taken by vslot `0x740` (`32009780`, plus per-class gauge overrides: Add `329e2f80`, Ain `32a3cf10`, Rose `32a80110`, Luciel `32aeb410`, EL `32b6ac80`, Ara `32ba4840`, Chung `32bed760`, Lire `32c852f0`, Elsword `32d058a0`) from user slot 250 (`3201ffb0`) whenever BTI 436/926 is applied. That pre-apply hook is the call June's `SetBuffFactorToGameUnit` makes before stacking (`X2GameUnit.cpp`, "slot 250"); March has no hook there yet | `CX2GUUser::UpNowHp` / `SetNowHp` (buff port Phase 7b): put the two gates after the BBT 101 refusal and before the base call, as June orders them. Crest of Time (the common SA `RNSI_SA_COMMON_CREST_OF_TIME`) needs the slot-250 hook, the snapshot members `+0x92a4..+0x92c4` and slots `0x740`/`0x744`, ported together |
| `3204dcd0` (user slot 251, the post-apply hook after stacking; buff Phase 11b) and Ain's override `32a3d220` | Luciel, Ain | Soul Collector: when a debuff lands on a unit with BTI 462 and the `RNSI_P_LCN_TRIBUTE` (30409) HP threshold is met, one debuff is erased, BFI 523 is applied, and `RNSI_P_LCN_NOBLE_DEVOTION` (30419) values follow | none: March's `SetBuffFactorToGameUnit` has no post-apply hook (`X2GameUnit.cpp`, "slot 251") |
| `329e2340` / `329e2360` (vslots 434 / 435) | Add | The class hook `TryHyperModeStateChange` (`31fe9a90`) asks first: when 434 is true, protected short `+0x7e84` = 0x1c, vslot 435( 0 ), awakening is consumed. Every other class uses the empty base (`31e4c550` false, `3208ddc0`) | `CX2GUUser::TryHyperModeStateChangeByClass()` / `OnHyperModeStateChangeByClass( int )` (Phase 7c, `JUNE_BUFF_AWAKE_LOCK`). **The `+0x7e84` = 0x1c write is not ported.** Effect C3d-1 (2026-10-08) identified the member: `+0x7e84` is the Future frame's `syncData.nowAction` (a protected short in June; Future `nowState` is `+0x7e6c`), and 0x1c is March `UAI_RELEASE_FORMATION_MODE` (`cl` probe: 28), Add's formation release. When porting Add, add `m_FrameDataFuture.syncData.nowAction = UAI_RELEASE_FORMATION_MODE` to the hook branch (the 16-bit `nowAction` is C3d-2's `SERV_IRUHADEV_JUNE_USER_ACTION_ID16`; 28 fits either width) |
| `32a31550` (vslot 344 `CanHyperMode`) and the five Add functions calling vslot 343 (`329eaa40`, `329eafe0`, `329eb210`, `329eb560`, `329eba20`) | Add | Add's own awakening test, and the Add states that call `TryHyperModeStateChange` (March's `X2GUAdd.cpp` has no `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` use) | `CX2GUUser::CanHyperMode()` is virtual for this override; `TryHyperModeStateChange` (Phase 7c) |
| Every class's vslot 510, the skill lookup June's `SpecialAttackEventProcess` (509) calls first (7c called it `NoStateChangeActionFrameMove`; corrected in Phase 7d): Add `32a20e70`, Ain `32a6dcd0`, Rose `32ad7f80`, Luciel `32b3a310` (+ slot 545 `32b3acf0`) | Add, Ain, Rose, Luciel | Starts with "if the quick-slot special item wait (`+0x8d78`) is running, return false" (no skill). **Phase 7d put this prefix at the top of the eight March classes' `SpecialAttackEventProcess`** (Elsword, Eve, Arme, EL, Ara, Chung, Raven, Lire); these four get it with their port | `CX2GUUser::IsQuickSlotSpecialItemWaiting()` (Phase 7c), flag `JUNE_BUFF_QUICKSLOT_ITEM` (Aisha: CH1i, steps 509-512 as `June_SpecialAttackEventProcess` / `Lookup` / `Use` / `Cooldown`) |
| Every class's June skill-use check (the cool-time / MP / hyper-stone block of its vslot 510 lookup; Aisha `32cebe80`) | every class | **Dev rule, not June (user, 2026-10-22):** skip the block when `CX2GUUser::JuneIsDevUnlimitedSkillUse()` is true. It is March's in-house gate (`TOGGLE_UNLIMITED_SKILL_USE` / `#ifndef _SERVICE_`, auth `XUAL_DEV`) in one helper, so a dev account spams skills in a ported class as in March, and `_SERVICE_` turns it off with March's. No JUNE_BUGS row | flag `JUNE_DEV_UNLIMITED_SKILL` (Aisha: CH4a, `June_SpecialAttackLookup`) |
| `32d27130` (Elsword, returns `0xce`) and `32b90420` (EL, returns `0xbb`), vslot `0x4ac` | Elsword, EL | June's quick-slot item check (`322fa960`) lets a throw/install item start from this state as well as wait/walk/dash/charge-MP/dash-end. Both are June-only states (March's Elsword enum ends at `0xc1`; March's EL `0xbb` is `ESI_A_SGM_SNAP_CRASHER_GROUND`, so the number must not be copied) | `CX2GUUser::GetSpecialItemUsableExtraStateID()` (Phase 7d, base = `GetWaitStateID()`): override it with the ported state |
| The same vslot 510 copies, further down: Add `@32a211cc`, Ain `@32a6e06a`, Rose `@32ad8254`, Luciel `@32b3a61f` | Add, Ain, Rose, Luciel | Right after `CheckSkillUsePVPOfficial` (`3202cba0`) and June's two skill-ban checks (`3202d140`, `3202d3c0`): "if the riding-key skill wait (`+0x92d0`) is not −1, return false" (`329cc370`). **Phase 7e put this line after `CheckSkillUsePVPOfficial` in the eight March classes** (`CanUseSpecialAttack` for Ara and Elesis, which hold that check there); these four get it with their port | `CX2GUUser::IsRidingKeySkillWaiting()` (Phase 7e), flag `JUNE_BUFF_RIDING_KEY_WAIT` |
| `329e65c0` (user vslot 303, base `31e4c550` returns false) | Add | June's `CommonStateStart` does not count a skill use for BFT 16 (`BFT_USE_SKILL_BY_SKILL_SLOTTYPE`) while slot 303 is true; Add's override is true for state index `0x123`-`0x126` and `0x12b` (June Add states; map them to March's Add enum with the port) | `CX2GUUser::IsSkillUseCountExcludedState()` (virtual, base false; read by `CountReservedSkillUse`, June buff port Phase 8) |
| `32bd32f0 @32bd339e-32bd3454` (`CX2GUAra::GetReduceDebuffDurationRate`, Ara vtable slot 167) | Ara | After the officially-debuff test (ported in buff Phase 10): when skill 27415 (`RNSI_P_ASD_OPEN_MIND`, June-only) is learned (`31fceab0` > 0), the counter `+0x114` (`32a31a00`) has reached `+0x120` (`32a31a20`) and the buff is a debuff (templet `+0x64`), the rate is multiplied by `32007b10( 0x7c, 27415 )` | `CX2GUAra::GetReduceDebuffDurationRate`, the `SERV_IRUHADEV_JUNE_BUFF_GROUP` block |
| `32ababc0 @32abaec8` (the `SA_RBD_Moving_Shot` state; calls `3229b0a0( skill, 0, 2 )` when `329cc340` is true, else unit slot 138 `( skill, 0 )`) | Rose | Applies the skill's first buff factor at level 2 instead of the learned level | `CX2GameUnit::SetBuffFactorToGameUnitAtLevel( skill, index, level )` (June `3229b0a0`, ported dormant in the buff Phase 10 review under `SERV_IRUHADEV_JUNE_BUFF_GROUP`) |
| `32a800a0` (Servo Nitro ctor with 1, dtor with 0; buff Phase 9b) | Rose | Rose-only work when BTI 720 / 920 (Servo Nitro) starts and ends on her | `CX2GUUser::JuneRoseServoNitro( bool )`, empty until Rose |
| `32ad0e90` (ctor) / `32ad0fd0` (dtor), both `( target, count )` (buff Phase 9b) | Rose | Chain Viper (BTI 891) begins / ends on a target. The count is the target's last-hit serial + 1, or the factor's value[0] when that is above 0 | `JuneRoseChainViperBegin` / `JuneRoseChainViperEnd`; the serial is `CX2GameUnit::GetJuneLastHitAttackSerial()` |
| `32ac2c50( target, final damage, count )` (Chain Viper slot 6; buff Phase 9b) | Rose | Sees each NPC final damage on a Chain Viper target (March's value, without June's last DamageReact steps) | `JuneRoseChainViperNpcFinalDamage` |
| `32ac2d50( target, factor, count )` (Chain Viper slot 7; buff Phase 9b) | Rose | Runs when a `BTI_DEBUFF_BLEEDING` factor lands on a Chain Viper target | `JuneRoseChainViperBuffApplied` |
| `32a4ddb0( target UID, target type )` / `32a4de20()` (Moder ctor; FrameMove on the target's death and dtor; buff Phase 9b) | Ain | Marks / unmarks the target of BTI 1022 (`A_AAP_MODER_MARKING`) | `JuneAinModerMark` / `JuneAinModerUnmark` |
| `32011960( bool )`, `31fd0590( target )`, `32011ab0()`, with `31fd05c0` and user vslot `0x85c` (Summon NPC Target ctor/dtor; buff Phase 9b) | every user (a June-only `CX2GUUser` system; BTI 819 / 894 creators use it) | The ally-summon focus target: user `+0x94e0`..`+0x94ec` (UID, type) is copied into each ally summon's AI `+0x158`..`+0x164`, which June's ally AI reads; clearing it also removes the old target's 819 / 894 marks (`3229d3d0`) | `JuneClearSummonTarget` / `JuneSetSummonTarget` / `JuneApplySummonTargetToSummons`, empty; March's `CX2NPCAI` has no such fields |
| `32b87880` (buff Phase 9b, item 5) | Elesis | Blood Runaway: on a hit with the `+0x9760` gauge it applies BFI 462 at the gauge level, then erases BTI 426 (`BTI_BUFF_BLOOD_RUNAWAY`) once its stack count passes 4. The only reader of 426 in June's binary | none: 426 is a tag (June's templet has an empty CUSTOM_FUNCTION, no switch case); add the enumerator with this code |
| `31fd0311` (user), `3275a0f5` (NPC), NPC `CommonStateStart @327af3bb` (resets it to -1) (buff Phase 9b) | Luciel (unit type 10) | Read the unit's last-hit serial `+0x40dc` | `CX2GameUnit::GetJuneLastHitAttackSerial()`, written by both DamageReacts since 9b; the NPC -1 reset is not ported |
| `SocketData_DuoUnit` (vt `33478ab0`, size 0x488, built by the user ctor `@32042060` when the unit type is 10), vslots 3/4 `32091b10`/`32091b60` (bases `32091320`/`32091310`), read in `32b0fa90` (`@32b0fac4`..`32b0fbaa`) (buff Phase 16 step 1) | Luciel (Lu/Ciel) | A second per-mode pair of protected stat blocks (`+0x468`/`+0x478`) in the socket aggregate, one per form | none: March's per-unit aggregate is the plain `CX2SocketItem::SocketData` (`m_SocketData`); Phase 16 gave it June's common fields only |
| Spec A (option member `+0x6bb0`, result `+0x6c50`) readers: `328eab00`, `3297ee20`, `32981130`, `329a4680` (Eve: CH1k, the gauge gain stays under `ADD_EVE_SYSTEM_2014` for CH1q; member `m_fJuneSpecializationA`), `329e59f0` (Add `UpNowDPValue`: DP consume), `32a620e0`, `32ac8f10`, `32b14860`, `32bc9a60`, `32bed5d0`, `32c41170`, `32c43bf0`, `32c56be0`, `32ca7130`, `32cb6290`, `32cc7300`, `32ccda90`, `32ce7480`; spec B (`+0x6c64`, result `+0x6d04`): `329b3cc0`, `329b5230`, `32a01b40`, `32a59e70`, `32ac7700`, `32b14860`, `32b37910`, `32ba6ef0`, `32bf5280`, `32c491a0`, `32c50330`, `32c85690`, `32ccfe30`; generic `CX2GUUser::AttackResult @32012da2` and `CEffect::AttackResult @31f86c29` (`x *= 1 + A`, or `1 - B` when vslot `+0x674` returns +/-2) (buff Phase 16 step 1) | per class | Each class's socket specialization: the socket keys `m_fCharacterSpecializationA`/`B`, curves `CalculateLinear/Final<Class>Specialization{A,B}...` | `CX2GameUnit::GetCharacterSpecializationA()` / `B()` (members `m_AdditionalCharacterSpecializationA`/`B`, seeded and refreshed since Phase 16 under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`); vslot `+0x674` is not named (Aisha: CH1i, `32ccda90` was `32ccdae0`: `June_Vslot860`; `32ce7480`: `June_CommonStateStart`; `32cc7300` n/a) |
| June `3207cd70` (ConvertAndAdd body) and the whole aggregate path (`3204f190`, `32080670`) (buff Phase 16 step 1) | Add | March's `ConvertAndAdd` halves Add's socket hyper charge and hyper time (`SERV_9TH_NEW_CHARACTER`, `fReformValue` 0.5); June has no 0.5 anywhere on the aggregate path. Whether June applies it downstream is not checked | Phase 16 drops it under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`X2SocketItem.h` ConvertAndAdd); check Add's hyper code for a June-side 0.5 when porting Add |
| `3230c1a0` (`CX2Game::SetUserSummonedNPCInfo`, rewritten in June) through the helper `322ece70`'s per-case flags (buff Phase 16-step3b3) | every class with a summon | June builds a summon's stats from the user's own stat line (`322ece70`: user `Stat` `+0xcf4`, scaled by `32071720( skill level )` and by `3344ce94`/`33472a10` when the NPC's `+0xc04` value is > 0), with per-case flags: 0x1 HP constant `3348be20`, 0x2/0x4/0x8 the NPC's own protected HP / attack / defence values (`+0xd14`, `+0xd18`/`+0xd1c`, `+0xd28`/`+0xd2c`), 0x10/0x20 the user's `+0x4074`/`+0x4068`/`+0x4064` multipliers, 0x40 the skill-ability lookups (`0xab`, `0xa1`-`0xa4` when game vslot `0x10c` returns 1 or 4, else `0x14`, `7`-`10`), 0x100 `3231dc20`, 0x200 the protected power factor `+0x8ce4`; then per-case follow-ups (`32c86340`, `32bdb250`, `323115e0` + Hell Drop effect set, `32cc98e0`, `32c878f0`, `31e4d560`, `32c11940`, `31e57960`, `32a2dec0`, ...) and the common tail (`3204e160`, `3200bec0`, `31fd05c0`, vslot `0x860`). March keeps its per-case `SetNPCStat` x `fStatScale`. The engine side (the socket skill-damage term on the power rate) is ported in 16-step3b3 | Port each class's summon cases with the class, from June's flags and follow-ups |
| Chung's Metabolism Boost in `CX2Game::UseItemSpecialAbility` (`322fbc60` HP up `@322fbe08`-`322fbe83`, HP % up `@322fd495`-`322fd510`) (buff Phase 16-step3b3) | Chung | June tests unit class `0x14`/`0x6e` (`31e4d0c0`), skill `0x6720` (32890620 level, ability `0x37`), and inlines `ability - 1.0 + 5e-8` (`3346ed68`), where March tests `UC_CHUNG_FURY_GUARDIAN`/`UC_CHUNG_IRON_PALADIN`, `SI_P_CFG_METABOLISM_BOOST`, and `CalculateIncreasingRate` (epsilon 5e-5). 16-step3b3 kept March's term and added the socket HP-recovery rate beside it | Port June's class test, skill ID and epsilon with Chung |
| `32011280` (from user AttackResultByType `320144d0 @32014507`) and `322a0180( DamageData*, n )` (from Process_ExtraDamage, user `32034e50 @320392c3` and NPC `327995c0 @3279d465`) (buff Phase 16-step3b4 review; user 2026-10-06) | Lanox | The "Secret Ammo" socket set: BTI 794 (`0x31a`) accumulation via `322a0090`, skills 87012 / 87002, sockets 86984 / 86985 / 86994 / 86995 in the attacker's `+0x9300` map, `DamageEffect_Ranox_Secret_Ammo_5` | June's per-socket cool-time store `+0x8b98`: `CX2GUUser::GetJuneSocketCoolTime` / `SetJuneSocketCoolTime` (16-step3b4; `31fd8b70` @`320113e8`/`322a0375`, `31fe0b90` @`3201144d`/`322a03a1`). March's `AttackResultByType` comment (X2GUUser.cpp:13340) names `32011280` |
| `3201ad10` (the skill-group power socket rate) called from class code: `3297762a`, `329968a0` (Eve range), `329fdbe0` (Add range), `32ad2bb0` (Rose range), `32bec286`, `32c058a0` (Chung range), `32d076f0` (Elsword range) (buff Phase 16-step3b5) | those classes (classes by address range; the functions are not decoded) | Each reads June's skill-group power socket rate for a skill. The core callers use it as `( rate + 1.0 )` on that skill's power rate; check each class caller's use when porting it | `CX2GUUser::GetSkillAdditionalPowerRateJune( skill )` (16-step3b5); June's callers add 1.0 (Eve: CH1l `3297762a`, the Queen's Throne rate in `June_InitEquippedActiveSkillState`; `329968a0` is CH3c) |
| `32b0e670` (`CX2GULuciel`, vtable `334f06fc`) and `32badc80` (Ara, vtable `33505314`): the overrides of user vslot 485 (`+0x794`), which SumSocketOption `3204f190` calls right after the equipped-slot loop (`@3204f4df..4ed`); the base slot `33093ad0` is an empty `ret 8`, and the other 10 class vtables keep it (buff Phase 16-step3b5 review) | Lu/Ciel, Ara | Each runs the skill-group power socket slot pass `31fe14f0` for skills outside the 10 slots. Lu/Ciel: 3 skills, `0x755c..0x755e` (`RNSI_SA_LDB_*`, 30044-30046), or 30050-30052 (`LDN`) when its vslot `0x260` returns true. Ara: every skill in the list `32d3f040` builds, unless `31fca980( skill, 1 )` holds. The Ara row above (buff hyper count) also lists the `32badc80` region; port both together | Override the empty user hook that 16-step3b7 adds (June vslot 485, called by `CX2GUUser::SumSocketOption` after the slot loop) and call `CX2GUUser::AddJuneSkillAdditionalPowerRateBySlot( kSocketData, skill )` (16-step3b5) from it; do not edit `SumSocketOption` |
| `329e2410` (`CX2GUAdd::ApplyHyperModeBuff`, June form) and `32a71a90` (`CX2GUAin::ApplyHyperModeBuff`) (buff Phase 16-step3b6) | Add, Ain | Add: BFI 846 (`BFI_TRANSCENDENTAL_FORMATION_MODE`) when its `CX2Unit +0x3c` is set, else 315 (`BFI_FORMATION_MODE`); then the first learned of skills 29401 / 29406 / 29411 goes to slot `0x23c( skill, 0, 0 )`, where March applies `BFI_BUFF_LIBRARY_OF_LIMITLESS_MANA_RECOVERY` by `m_iLibraryOfLimitlessLevel`. Ain: erase BTI 966-968, apply BFI 1034, then `32083190( 0 )` and `+0x95c8 = 0`. **March has Add's function and June reworked it: port it whole.** | Keep the tail both end with: when `m_SocketData.m_vecInfoByUseHyperMode` is not empty, call `CX2GUUser::ApplyBuffByUseHyperMode()` (June `3206d480`, ported in 16-step3b6 with the per-socket cool time). Add calls it with no user test, Ain only for a user |
| `3201f850` (`CX2GUUser::SetSkillLevelStateData( templet, state )`, June base form) and its callers: user vslot 490 `32028190` (by state id) and class code `32968f20` (x9), `32c886b0`, `32ccb830`, `32bd358d`..`32bd3db2` (buff Phase 16-step3b7) | every class with skill states (March's copies: El, Add, Ara, Rena, Raven) | June hoisted March's per-class `SetSkillLevelStateData` (`X2GUEL.cpp:2977`, `X2GUAdd.cpp:2455`, `X2GUAra.cpp:2989`, `X2GULire_ElvenRanger.cpp:22954`, `X2GURaven.cpp:22987`) into `CX2GUUser` and multiplies the state power rate by ( `3201ad10( skill )` + 1.0 ) after the level / power rate / skill id. Port the term into each class copy (or hoist it like June) with the character | `CX2GUUser::GetSkillAdditionalPowerRateJune( skill )` (+ 1.0) (Aisha: CH1i, `June_SetEquippedSkillLevel` + `June_SetSkillLevelStateData`) |
| June NPC scripts `Nasod_Watch_Eve`, `SI_HA_FERDINAND` (Eve) and `SI_HA_APOCALYPSE` (Add, NPC 1604, June-only) call `SetMultiplyPowerRate_LUA( GetNumber_LUA() )`; the binding is `31e8d610` (bound `@31e0abe3`, with `ResetMultiplyPowerRate_LUA` `@31e0a802`), the writer of the NPC's protected factor `+0x8ce4` (buff Phase 16-step3b7 item 4) | Eve, Add | The summon's power factor becomes the number the summon helper stored (`3201ad10( skill )` + 1.0); June's helper first wrote its whole product there, so the script call replaces it. March has neither binding nor a separate `+0x8ce4` factor (its NPC power rate is the one `m_fDefaultPowerRate`), and March's copies of these scripts never call `GetNumber_LUA`. Port the binding and its factor with the June scripts | `m_NumberList[0]`, written by `JuneStoreSummonGroupPowerRateNumber` (`X2Game.cpp`) for NPC 384 / 519 / 1454 |
| Engine readers keyed by a March skill ID where June pushes the reform ID: Mark of Commander `0x6666` (26214) at `@3236dfe7..e002` (`X2BuffTemplet.cpp` DoMarkOfCommand, Phase 9a + 16-step3b7) and `@327742e3..32774310` (`X2GUNPC.cpp` AttackResultByType, 16-step3b7); Dimension Link 25000 and Link Overcharge Illusion 25601 at the summon helper `322ece70` (`X2Game.cpp` SetUserSummonedNPCInfo, 16-step3b3 / 3b7) (buff Phase 16-step3b7 review) | Chung, Eve | Each reads a per-skill store (`31fd7cc0` buff rate, `3201ad10` skill-group power socket rate) under the skill ID June's skill tree holds. March reads the same stores under its own IDs (`SI_A_CHG_MARK_OF_COMMANDER` = 6223, `SI_A_EN_DIMENSION_LINK` = 5201, `SI_HA_EEP_LINK_OVERCHARGE_ILLUSION` = 5601; `cl /P` probe), which is what a March slot holds today. When the class moves to June's skill templets, its slots hold 26214 / 25000 / 25601, and these readers must take June's IDs or they read 0. It also opens `SkillLevelUpSocketGroup.lua`'s groups to the class: 706 of its 722 skill IDs are June-only, 150 of them March skills under a reform ID (e.g. Chung's groups 1/7 list 26214) | `GetSkillDamageRateByBuff`, `GetSkillAdditionalPowerRateJune`, `GetSummonPowerRateBySocket` / `GetSummonPowerRateByBuff` (keys as passed) |
| June `data037_kom/Lire_Evoke_trap.src.lua` and `Lire_Evoke_trap_Memo.src.lua` (`INIT_COMPONENT USE_OWNERS_ENCHANT = TRUE`) (buff Phase 15 step 2b) | Lire (Evoke trap) | The trap NPC uses its owner's weapon enchant: NPC slots 111/112 (`3275aa20`/`3275aa80`) forward to the owner when the flag is set, so the trap's hits proc and scale with the player's attribute | `CX2GameUnit::m_kJuneUseOwnersEnchant` (read in `InitComponent`), `JuneGetWeaponEnchantExtraDamageType()` and the NPC branch of `GetWeaponEnchantData`. March's copies of both scripts lack the key, so it stays off until June's scripts are adopted with the class |
| `32a638c0` (`CX2GUAin`, user vslot `0x550` `ForceChangeHyperModeWithoutMotion`; the other 12 user classes share the base `31ff7300`) (buff Phase 16-step3c2 review) | Ain | The base function plus a `+0x9614` snapshot of `GetRemainHyperModeTime` (`31e4f890`) around the vslot `0xc0` call, zeroed after it. June's `PartyFeverAllUser` (`322d4f80`: party-fever packet and the HMET_FEVER socket) calls it through the vtable | `CX2Game::PartyFeverAllUser` calls March's non-virtual `CX2GUUser::ForceChangeHyperModeWithoutMotion`; the Ain port makes it virtual and overrides it |
| Class readers of the tutorial flag `g_pMain+0x16a` (= `CX2Main::GetIsPlayingTutorial()`; buff Phase 14c, `june2017_re/c14c_tutorial_reads.tsv`): `32978ec2` (`CX2GUEve` vslot `0x6fc`), `32a59534` (`CX2GUAin::InitComponent`), `32ab451f` (`CX2GURose` wait-habit check), `32b74065` (`CX2GUEL::DamageReact`, Iron Will), `32bb0e2c` (`CX2GUAra::InitComponent`), `32d1af6c` (`CX2GUElsword_SwordMan::DamageReact`) | Eve, Ain, Rose, Elesis (EL), Ara, Elsword | Eve: in the tutorial a render-param call uses the June-only guide manager step (`g_pMain+0x8c4 +0xe4`). Ain: tutorial -> `32a31900( 0 )` (training gives 7999.0). Rose: tutorial and a slide shot / event scene -> idle timer `+0x9610 = 0`. EL: the Iron Will branch needs `!tutorial`. Ara: tutorial -> force power 3 (`32a32640( 3 )`; March handles PvP and training only). Elsword: the special reaction needs `+0x80bc && !tutorial` | none: read `g_pMain->GetIsPlayingTutorial()` directly (March member, no store) (Eve: CH1l `32978ec2` in `June_InitInpuDataProcess`: no mask, the step reader and the InputData mask `320708f0` are June-only, backlog l.480) |
| `CX2StateCreateUnit::Handler_EGS_CREATE_UNIT_ACK` `3255b100` @`3255b5f6`/`3255b612` (buff Phase 14c) | Rose (UT `0xb`), Ain (UT `0xc`) | After a new unit is created June clears the tutorial flag (`SetIsPlayingTutorial( false )`) for Rose when instance flag `0x8df` is set and for Ain when `0x26` is set (`31ee34b0`), so these classes can skip the first tutorial | `CX2Main::SetIsPlayingTutorial` (with the 14c key latch); the two instance flags are unidentified |

### 7a. Effect-engine hooks that only character code drives (from June effect completion C6a)

C6b ports June's spawner tail: the parent-to-child copies `31f8fdb0`, `31f649d0`, `31f64c00`, `31f65e40` and `31f65f90`, under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`.
Every gate in that tail is switched on by class code only, so it does nothing until the class code below is ported.
The verdicts are the funcmap rows tagged `Effect C6a`.

| June | Class | What it does | March store (C6b names) |
|---|---|---|---|
| `321db050` / `321dad70` / `321dae90` (`CX2EffectSet::AddDamageEffectModifier`) on an effect-set handle; `321db1b0` applies the list to every damage effect the set creates | all of the below | Pushes a `{b, tag, v}` modifier. The tags that matter here: 12 = `b44` + IGNORE_STATE_DEFENCE, 13 = `b45` + DD `+0x5fd`, 14 = `b46`, 16 = `b47` + IGNORE_DEFENCE(_ON_PVP), 17 = `b65` + push onto ADDITIONAL_ATTACK_TRIGGER_TYPE, 19 = life-time rate, 20 = crit rate, 23 = `m_iIndex`. The full tag table is in funcmap row `321db1b0` | **none: the modifier list (EffectSetInstance `+0x170`) is a June-only system.** Port it with the first class that needs it; March's EffectSet applies only tag 1 (APPLY_DAMAGE_EFFECT_VELOCITY, from data) |
| `3299a9d0` (`SA_EBS_Extreme_Virtual`: tags 16, 12, 20); `329c12a0` (`31f649d0` x5, `31f65e40`/`31f65f90`); `329a3040`, `329a3760` (Thousand Star); `32968c50`, `3297bad0` (`31f65e40`/`31f65f90`) | Eve | Ignore-defence and damage-rate inheritance on her projectiles | the C6b members and methods |
| `329f3980`, `329fdb60` (Stardust Shower: tag 12); `32a047a0` (Reverse Circle / Void Field: tags 12, 13, 14); `32a060d0` (Moonlight Breaker: 12); `32a19bb0` (Flick Disc: 12); `32a0e5f0` (Quake Buster Hyper: 14); `329fc3f0` (Space Crack) and `329fd0c0` (Maximum Strike) set `b44` + IGNORE_STATE_DEFENCE directly; `32a0f830` (Conqueror) sets `b45` + DD `+0x5fd` directly; `329f10e0` (Pulse Cannon: `31f65f90`); `32a04ce0` (via `32a309a1`) is the only reader of DD `+0x5fd` (skill `0x7150` / `0x7225`) | Add | The same, plus Add's "count as skill hit" mark | same; DD `+0x5fd` = `m_bCountAsOwnerSkillHit` |
| `32abe5c0` (`SA_RMH_EX_S_Viper`: tag 12); `32ac2b40`, `32ac2ba0` (tag 12); `32ac2810` (tag 17); data values `DTT_RCR_TRANSCENDENCE_REVOLVER` / `_NOT_REVOLVER` (29/30), which `31f64c00` always copies to children | Rose | Ignore-state-defence and attack-trigger inheritance | `m_bInheritAttackTriggerType`; the DTT values need June's DAMAGE_TRIGGER_TYPE entries (section 4) |
| `32d18230` (`RunSlayer_RUNE_GUARD_FIRE`: tag 19) | Elsword | Main-effect life-time rate, inherited by children | `m_bInheritMainEffectLifeTimeRate` / `m_fMainEffectLifeTimeRate` |
| `32a63ba0` (tag 23: `m_iIndex`); `32a64d00` (tag 17); readers of `m_iIndex` and `m_bHyper` in `32a63e80`, `32a63f60`, `32a64800`, `32a65620`, `32a65860`, `32a6a850`, `32a6a8e0`, `32a6af10`; vslot `0x248` override `32a51410` (calls the base, then scales the instance by skill `0x7e97` when instance `+0x1b4`, the set's APPLIED_SKILL_ID copied in by PlayEffectSet `@321e2020`, names a type-3 skill) | Ain | Indexes its effects, tests whether they came from a hyper effect set, scales some sets | `m_iIndex`, `m_bHyper`, `JuneStampEffectSetIndex` (C6b); the instance's APPLIED_SKILL_ID copy is not in March (port it with Ain; funcmap `EffectSetInstance+0x1b4`) |
| vslot `0x248` override `32b0efc0` (instance `+0x1b8` = unit `+0x988c`, a counter advanced by `32b0fa00`); vslot `0x2e0` override `32b0efe0` (returns `+0x988c`); readers of DamageData `+0x638` `32b14db0`, `32b16810`, `32b16880`, `32b4cb80` (`>= 0` and APPLIED_SKILL_ID `> 0`, then match `{skill, index}` in unit `+0x9890`) | Luciel | Stamps each effect set it plays with an index that its damage effects carry as `m_iIndex` and DD `+0x638`, and counts hits per (skill, index) | C6b (from C6a2): `EffectSetInstance::m_iDamageEffectIndex`, `CX2GameUnit::JuneStampEffectSetIndex` (base no-op), `GetJuneDamageEffectIndex` (base -1), `DamageData::m_iJuneDamageEffectIndex` |
| `321e3650` callers `32a0e5f0` (Add), `32ab2ff0` x3 (Rose), `32ad2bb0` (`@32ad31e9` also calls `321ee480` itself); `321e3730` (with a modifier list) from Rose `32ab30ee`, `32ab31ed`; `321e3830` (one modifier) from Add `32a04a2f`, `32a04b03`, `32a06d06`; direct `321f4690` push from `32ad31b8` | Add, Rose (`32ad2bb0`: not yet attributed) | Attach buff factors (some with behavior/finalizer modifiers) to an effect set, so every damage effect it creates carries them | C6c (from C6a2): `EffectSetInstance::m_vecBuffFactorList`, `AttachBuffFactorToEffectSet`, `ApplyBuffFactorListToDamageEffect`; the with-modifier attach variants come with these classes |
| CEffect `+0xb18` writer `32bb0bd6` (in a function called from `32bbcdc0`, `32bbcff0`, `32bbd240`, `32bbd3a0`); overrides of unit vslot `0x328` (the hook every CEffect ctor calls with the new effect; base `3208ddc0` no-op) | not yet attributed (the `32bb....` range) | Sets the parent's create-damage-effect rate; `31f6ea20` then scales an RT_CREATE_DAMAGE_EFFECT child's damage, HIT_ADD_MP and TECH_POINT by it (from the C6b review) | the engine half is C7's (kickoff "Added by the C6b review" item 1): the CEffect rate member (-1) and the base vslot `0x328` virtual |

**Added by pilot-char-revamp CH2a (2026-10-21): the modifier list now exists, for tag 2 only.** Aisha's Chain Fireball is the first class that pushes a modifier (`AddDamageEffectModifier( handle, 2, m_fFireBallRangeRate, false )`, main-effect life time x range rate). `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` adds `EffectSetInstance::m_vecJuneModifier` (`+0x170`), `CX2EffectSet::AddDamageEffectModifier` (`321db050` / `321dad70`) and the apply hook in the damage-effect creation path (`@321ef6b3`). `ApplyDamageEffectModifiers` (`321db1b0`) handles tag 2; **every other tag logs once (`[JUNE_FX] effect-set modifier tag N is not ported`) and does nothing.** A class port that pushes another tag adds its `case` there, together with the member it writes (the table above).

**Added by june-effect C3 review (2026-10-08): modifier tag 15 is a base-scale modifier.** `321db1b0` tag 15 (`@321dbaa8-321dbaf4`) reads the CEffect base scale (`31f8af20`, `+0xb24..`), adds or multiplies it by the entry's v3, writes it back (`31f8aef0`), then calls SetScale `31f6ff60`. June's SetScale (`31f6fdc0`) multiplies by that base; March's `CEffect::SetScale` is absolute. When a class port first pushes tag 15, port the base member and make `CEffect::SetScale` multiply by it in the same change, or C3's `SetScale( CREATE_DAMAGE_EFFECT_NAME_SCALE )` calls (default 1,1,1) reset that child's scale to 1.0. The same dependency applies to the CEffect ctor's base step (DD `+0x5d0`, `@31f7c7f0-31f7c838`): see `kickoffs/june-effect/C8b.md`.

### 7b. SetDamageExceptionProcess hooks that only character code drives (from June effect completion C8a)

C8b ports the base, user and NPC layers of `SetDamageExceptionProcess` (slot `0x2b8`), all dormant on March data. The per-class overrides are not ported in the effect plan: D8 (2026-10-06) hands them here, so each character port ports its class's blocks, using `kickoffs/june-effect/C8c.md` as the brief.
Each row below is what turns one of them on. The verdicts are the funcmap rows tagged `Effect C8a`.

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `32c57660` (`+0x9688` = skill 24401 learned); `32c43120 @32c43ac4` (`+0x4108` = 2 with `ChangeDamageType`) | Raven | Burning Nasod Hand's damage-type change then applies only to `AT_COMMAND` hits (the user-layer gate `32045340 @320453a0`) | the 24401 setter and the `= 2` write, together with June's `RAVEN_FIGHTER.lua`, which marks 46 attacks `AT_COMMAND`. March data marks none, so the `= 2` write alone switches the passive off |
| `31fdbbb0` (skill-memo entry for `APPLIED_SKILL_ID`) behind user vslot `0x758` `3202ac40` and the NPC override `32759f30` | every user (and summons with a live master) | June's data-driven skill memo: per memo type, scales damage, force-down, MP gain and buff factors on the hit | June's memo table. C8b stubs the lookup (plan rule 5, returns null) **S3b made the lookup real; S3d (2026-10-18) ported the switch's remaining cases and the engine consumers** (`JUNE_SKILL_MEMO_*`); what stays here is the class code below |
| `31fdbbb0` / `31fdbcc0` readers in class content (found by S3d): `320284c0 @3202858e` (only caller `3202c360`; skills 31301 / 31321: memo( 31301 ) type 0x10 rescales values 1, 3, 5, 7 of each behavior holding 8+ values ( v + ( v - 1 ) x | v0 - 1 | ), then slot 0x228 for 31321) and `3230c1a0 @3230c742` (SetUserSummonedNPCInfo: memo( 22027 ) type 0xf multiplies a summon rate by v0); plus the 28 class callers of `31fdbcc0` (S3b row 3190) | the classes owning skills 31301 / 31321 / 22027 | class-specific memo effects | The memo lookups are real (S3b); port with the class's states. |
| the per-class blocks in funcmap rows `329bc560`, `32b80f20`, `32bf28f0`, `32c5c080`, `32cad510`, `32cccc70`, `32d1e0e0`, and Ara's vslot `0x780` `32bca010` | Eve, Elesis, Chung, Raven, Rena, Aisha, Elsword, Ara | Passives keyed on June reform skill IDs (`RNSI_*`) | the blocks themselves (brief: `kickoffs/june-effect/C8c.md`) plus the reform skills; needs effect C5 (`ATTACK_TRIGGER_TYPE`) (Aisha: CH4c, `32cccc70` whole) |
| D8 (completion plan): Rena's Powerful Bowstring 3405 → 23405 and Sharp Kick 3400 → 23400; Aisha's Fitness 2413 block replaced by Magical Staff 22409 | Rena, Aisha | June re-keys or replaces passives March characters have now | D8 = hand off: March's blocks stay until this port, which swaps in June's versions together with the reform skills (Aisha: CH4c, Fitness gone, Magical Staff 22409 in) |

### 7c. Effect-alpha tags that only character code applies (from June effect completion C10a)

C10b-2 ports June's effect-alpha option engine under `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION`. It brings the game
option, the category (`CX2Game::GetEffectAlphaCondition`), the EffectSet/CEffect readers, the renderers and the five
`CX2Game` tag helpers.
- Effects created through EffectSet or DamageEffect fade with no class code.
- Particles and meshes that class code creates directly fade only where June's class code calls a tag helper right
  after creating them.
- D8 hands those calls here: each character port adds its class's calls.
- Every site is a row in `june2017_re/c10a_alpha_sites.tsv` with disposition `checklist 7c`. The helpers' meaning is in
  the funcmap rows tagged `Effect C10a`.

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| 167 sites in 0x32965e30-0x329cf260 (`32296d60` particle x119, `32296ee0` mesh x41, `32296da0` / `32296e40` handles x7) | Eve | Tags her particles and meshes with her alpha category right after creating them | one `CX2Game` tag-helper call per site, inside the class code being ported (Eve: CH1a 5 of 167: `329824c0` x2, `32982c00`, `32982d30` x2; CH1e 5: `32985d93`, `32985ed8`, `329b0dbc`, `329b15a4`, `329b1765`; CH1f 2: `32984a5b`, `32984c26`; CH1j2 9: `329950fc`, `32995b6e`, `32995d38`, `32996163`, `329963da`, `329966da`, `329ada7f`, `329adaf9`, `329ae08f`; CH1k 1: `329a2a0b`; CH1l 1: `329b77ad`) |
| 29 sites (`32d0d660` .. `32d1ecc0`) | Elsword | as above | as above |
| 22 sites (`32ccdb70` .. `32ce0010`) | Aisha | as above | as above |
| 12 sites (`329f4350`, `329f9ff0`, `32a03b70`, `32a27360`; four of them effect-set handles via `32296f30`) | Add | as above | as above |
| 8 sites (`32c89310`, `32c89460`, `32c895b0`; `32296da0` handles) | Rena | as above | as above |
| `322726f0`: the HP-rate gate in `GetFinalDamage` returns false for `RNSI_SA_ALP_SUPERSONIC` (29058) against an NPC defender (GameUnitType 1) when it has `+0x8aeb` / `+0x8ae9` set, vslots `0x3c8` / `0x3cc` / `0x430` true, or `+0x8e80` set, or when its `+0x8f08` is 1 or 2 | Rena | Supersonic's HP-rate damage skips some NPCs | the gate's false branch, with the reform skill. C10b-1 calls `GetHpRateDamage` unconditionally until then |

### 7e. Unit-viewer effect rows that only character code drives (from June effect completion C9c-2b)

C9c-2b ports June's unit-viewer effect player under `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`. It retires nine of March's hand-made class
effects (MagicKnight, SPK, SBH, ALP, CDC, ETK/EIS, RTR, RNW: the table's 11 live rows now drive them). The rows below are the parts that need
a class or system this build does not have. The verdicts are in `june2017_re/c9c2a_hardcode_map.tsv`.

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `UnitViewerEffectSetTable.lua` row `ALD_LobbyWait` -> `EffectSet_ALD_LobbyWait` (3 particles, staged and inert) | Ara (Little Devil) | June renamed the anim to `ALD_LobbyWait`; March's `Motion_ARA_Common.x` and `X2UnitViewerUI.cpp` still say `ALD_lobbyWait`, and the table lookup is case-sensitive, so the row never matches. March's hand-made orb (`m_hSeqEnergyBullet1-3`: ctor, dtor, `OnFrameMove`, `PlayAnim`, `NotifyShowObjectChanged`, `OnFrameRender_Draw`, `SetUnit`) was **kept** (user 2026-10-07) | **C9g-2 (2026-10-08) adopted June's `Motion_ARA_Common.x` and `AnimationNameTable.lua`**, so the row now matches and plays the same three orb particles as the kept hard-code, which stays dead code (keyed `ALD_lobbyWait`). Still to do: retire the three handles under the player's flag (the same `#ifdef ... #else` the other eight got), with the user's OK |
| `UnitViewerEffectSetTable.lua` row `SPK_LobbyReadyAction` -> `EffectSet_SPK_LobbyWait_LobbyReadyAction` (start 0.9, staged) | Elesis (Pyro Knight) | June renamed the anim to `SPK_LobbyReadyAction`; March's `Motion_EL_Lobby.x` and `PlayByMotionType` (`X2UnitViewerUI.cpp:4996`) still say `SPK_LobbyReadyaction`, so the row never matches. Unlike the orb, March's hand-made SPK fire (which emitted on both the wait and this motion) **was retired**, so the ready action has no hand fire in the March client; the user accepted that loss (2026-10-07, C9c-2b review). The wait row works | done by C9g-2 (2026-10-08): June's `Motion_EL_Lobby.x` and the table's `SPK_LobbyReadyAction` are in, so the row matches and the ready-action fire is back |
| duo minor model: `+0xd7c` skin anim, `+0xd80-0xd82` (duo type, shown flags), the `DT_MINOR` row `LDN_LobbyWait` -> `EffectSet_LDN_Start_Ciel` (8 particles), `LDB_LobbyReadyAction` flag `+0x11d8`, and the second per-model frame move (`31fa8ce0` with its last argument 0) | Lu/Ciel | March passes duo 0 to `GetUnitViewerEffectSet` and runs one frame move; the minor model does not exist | the duo viewer model, then pass the model's duo type in `PlayAnim` and run the frame move per model |
| rows `Emotion_Sad` -> `EffectSet_Gunner_Sad` (`UT_ROSE`) and `Emotion_SitReady` / `Emotion_SitWait` / `Emotion_StandUp` -> `EffectSet_Ain_*` (`UT_AIN`), all staged | Rose, Ain | inert: no such class in March | nothing; they match once the class exists (the emotion path is `CX2GUUser::PlayEmotion`, C9c-1) |
| lost under June's player: the SBH / ALP end windows and forced emit rates (March stopped the particles at 2.95 s / 2.19 s / 4.5 s, June's `UIMajorParticle.txt` has the stop times), the CDC / ETK / RTR / RNW weapon-equipped gate, the class-change mesh outline, and the effects in the avatar screen capture | Elesis, Lunatic Psyker, Chung, Elsword, Rena | June's player has none of them | nothing to port; listed so a missing outline or capture effect is not read as a regression |

### 7d. Damage-math hooks that only character code or June's data drives (from June effect completion C4)

C4 ports the engine side under `SERV_IRUHADEV_JUNE_DAMAGE_MATH` (range rate, Nasod combo gate) and `SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY` (DIVISION window). Two June pieces are keyed on reform skill IDs this build does not have, so the character ports bring them (D8, no question).

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `329f63a0` top switch (`0x7211`, `0x7212`, `0x7220`, `0x7221` = 29201 / 29202 / 29216 / 29217: `RNSI_A_AN_PHASE_SHIFT`, `RNSI_A_AN_PULSE_BULLET` and their `_UPGRADE`) | Add (Lunatic Psyker) | `CX2GUAdd::SetNasodArmorAttackTarget` sets a target only for a hit with `CAN_ACTIVATE_NASOD_ARMOR_COMBO = TRUE` or one of those four skills. March's Add passes the two March skills of those names (`SI_A_AN_PULSE_BULLET`, `SI_A_AN_PHASE_SHIFT`) in their place; the `_UPGRADE` pair has no March skill. | the four reform IDs in the gate (`X2GUAdd.cpp`, `SetNasodArmorAttackTarget`, under `SERV_IRUHADEV_JUNE_DAMAGE_MATH`) once the reform skills land; the 52 `CAN_ACTIVATE_NASOD_ARMOR_COMBO = TRUE` lines of June's `DamageEffect*.lua` come with the DamageEffect data. Until then no other hit sets the Nasod armor target (user, 2026-10-07: follow June, accept off). |
| `320483e4` (`CX2GUUser::DamageReact`) and `327b33cf` (`CX2GUNPC::DamageReact`) | Luciel (unit type 10, `31e4d0e0`) | An attacker of unit type 10 whose DamageData has `USED_SKILL_ID` 30205 (`RNSI_A_LLC_BACKSTAB`) forces `IGNORE_DEFENCE` and `IGNORE_DEFENCE_ON_PVP` (`+0x528` / `+0x529`) on in the user DamageReact, and the NPC DamageReact (`@327b33cf`) also `IGNORE_DEFENCE_DECLINE` (`+0x534`), before the defence maths (funcmap row `320462a0 @32049412`; the exact user-side stores are to be read from `@320483e4-32048412`). | the skill-30205 test in both DamageReacts (the three DamageData members and their readers are June buff port 5c's, `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`), with the Luciel reform skills. |
| `329f63a0` body after the gate (`@329f64c1` on) | Add (Lunatic Psyker) | June rewrote the rest of `CX2GUAdd::SetNasodArmorAttackTarget` (C4 ported only the gate). The NPC-defender test reads `*(npc+0x85f8)+4 == 0` (most likely March's `NCT_BASIC == GetNPCTemplet().m_ClassType`); the used skill ID goes through the skill-memo swap `328905b0` when the attacker is a user with a unit (`+0x867c`); the FantasyTracer branch is taken on the top switch's byte (the four reform IDs) instead of a skill-ID `case`, and matches `m_vecFantasyTracerTargetInfo` (`+0x9650`) against the swapped ID; the default branch is rewritten (nearest collision frame `32278580`, bone lookup vslot `0x80`, a line-map walk) where March measures the `GetPos` distance. Found by the C4 review. | June's whole body, with Add's reform skills and the skill-memo system; read `329f63a0` end to end when porting |

### 7g. Lobby habit motion and unit-type remap that only the Rose class drives (from June effect completion C9g-2)

C9g-2 ports June's `PlayByMotionType` (`31fa7ef0`) under `SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE`. Two pieces need a class March does not have
(D8, no question). The rest of the HABIT path is the customize system (backlog Stage 12).

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `31fa7ef0` unit-type remap (ported, inert: `m_pUnit->GetUnitTemplet()->m_UnitType == 11`) | Rose (`UT_ROSE` 0xb) | in the cash shop WAIT plays as WAIT1; WAIT1/WAIT2 play as WAIT | the `UT_ROSE` enum value; the literal `11` in `PlayByMotionType` then matches |
| `31fa8ce0 @31fa9a7a` habit timer: unit type 0xb, motion type 0 (WAIT), no emotion, viewer state != 10 -> every `+0xf9c` seconds (`+0xfa0` elapsed, a float reset by `PlayByMotionType`) `PlayByMotionType( 6 HABIT, 1 )`; the first copy `@31fa99f1` is the same behind `+0xf90` (customize habit, Stage 12) | Rose | plays the idle "habit" motion on the character-select / lobby viewer; `AnimationNameTable.lua` has 9 HABIT rows, all June-only classes | the two viewer members (`+0xf9c` limit, `+0xfa0` timer), the `OnFrameMove` block, and Rose's HABIT rows (already in the staged table; they load as class 0 until `UC_ROSE` exists in Lua `UNIT_CLASS`) |

### 7f. NPC anim-speed hook that only character code drives (from June effect completion C4b)

June effect completion C4c ports the engine side. The NPC anim-speed factor (June NPC `+0x8b6c`, default 1.0) is `CX2GUNPC::m_fRoseSummonAnimSpeedRate` under `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED` (C4c, 2026-10-07; no setter yet, the Rose port adds one), and June's `CX2GUNPC::ResetAnimSpeed` (`327615d0`) multiplies it in. Its only writer is Rose class code, so the character ports bring it (D8, no question).

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `32ab0560` (`CX2GURose` vtable `334e8b5c`, slot `0x860`, argument = an NPC) | Rose | Reads a skill value: `this->32007c40( 0x2d, 31423 RNSI_P_ROM_OPEN_MIND, 1, &v )`, with `v` starting at 1.0. When that read succeeds and the NPC is one of Rose's summons (`NUI_SI_SA_RG_RX78_LAND_RUNNER` 2175 and `_UPGRADE` 2194, `NUI_SA_RMH_GALE_FORCE` 2206, `NUI_SA_RMH_EZ_8_COUNT_DOWN` 2209, `NUI_SA_ROM_ARMORED_TEMPEST` 2526), it sets the NPC's anim-speed factor to `v`. For `NUI_SA_RFY_CLAYMORE` (2398) it instead calls `SetMultiplyPowerRate_LUA( 1.15 )` and writes two of Rose's own members (`+0x994c`, `+0x9948`). | The whole slot body, with Rose, her reform passive and the summon NPCs. It writes into the member C4c adds (funcmap row `32ab0560`). |

### 7g. Item-templet fields only character code reads (from june-item-kim K0)

June-item-kim K2 ports June's 228-byte item record, so these fields exist once June's `item.kim` is in. Their consumers are
per-class, so the character ports bring them (D8, no question). Map: `june_item_kim/k0_record_map.tsv`.

| June | Class | What it is | What the port must bring |
|---|---|---|---|
| record +48 `m_ItemIDForMinorUnit` (int) | Lu/Ciel | On 429 Lu items, the id of the matching Ciel dummy item (30344 -> 200030344). | Whatever equips or shows the Ciel copy of Lu's gear (reader not located in K0). |
| record +8 flags2 bit 2 `m_bIsForMinorUnit` | Lu/Ciel | Set on 2,582 items: the Ciel dummies. | The same consumer. K2 adds only the accessor. |
| Extra accessors `32867660`..`32867960`, bool argument | Lu/Ciel | Unit type 10 maps to Extra key 10 (Lu) or 11 (Ciel) by that bool. 11 maps to 12 (Rose) and 12 to 13 (Ain). | Callers that pass the Ciel bool. K2 ports the remap with the bool defaulting to false. |
| special ability 41 `SAT_ADD_DP_PERCENT_UP` | Add | 1 item (100001054). | The ability's effect on Add's DP. |

### 7h. Unit-state readers that only class code drives (from June effect completion C9h-4)

June effect completion C9h-4a..4e port the engine side: the sound record and lists, `UPDOWN_CANCEL_AFTER` with June's
`RidingAttackZEventProcess`, and the effect-set delete conditions. The rows below are read only by class code, or select by data only
class scripts carry, so the character ports bring them (D8, no question). Funcmap rows `Effect C9h-4`.

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| Elsword 25 functions `32cf8230`..`32d06c50` (e.g. `32d01040`), Raven `32c2b360`, Lire `32cb9ee0` | Elsword, Raven, Lire | Per-class state EventProcess cancel branches. When `UPDOWN_CANCEL_AFTER` (user `+0x8ca4`, C9h-4c's member) is >= 0 and below the anim time (the second skin `+0x8e84` when `+0x8e90`), on the ground, with up or down pressed, they change state. Data: June's `RAVEN_FIGHTER` (and the riding pets C9h-4e covers). | The class states that carry these branches, with the member C9h-4c adds. |
| `32024ff0` list variants: `EFFECT_SET_LIST_EX_TRANS_1` / `_TRANS_2` / `_EX_NO_TRANS` (picked by the skill-memo entry `31fdbbb0` of `GetNowStateSkillID`, see the `31fdbbb0` row in 7), `EFFECT_SET_LIST_ULTIMATE_EX(_APPLIED_SKILL)`, `EFFECT_SET_LIST_EX_APPLIED_SKILL` (an `APPLIED_SKILL_ID` with level > 0, `31fceab0`), `EFFECT_SET_LIST_FINAL` / `_FINAL_ULTIMATE` (`32020ec0`, picked by the loader's third argument) | every class with June scripts (ELESIS_KNIGHT, Arme_VioletMage, Lire_ElvenRanger, Luciel_Duo, Ain, Ara, Add, Elsword: 23 scripts for `_FINAL`, 10 for `_FINAL_ULTIMATE`) | June's `LoadEffectSetFromScript` reads the plain lists as March does, then these, choosing by skill memo, transcendence, applied-skill level or the ultimate flag. | The selection with the skill system it needs (memo entries, transcendence, the ultimate argument from each class's caller). C9h-4d ports only the delete keys of the same function. S3f (2026-10-19): the third argument now reaches March's `CX2GUUser::LoadEffectSetFromScript` as `bJuneUltimate_` (CommonStateStart's upgraded-skill flag, `@3205ad87`; unread until this row), and the memo argument is always 0 as in June (JUNE_BUGS 332). |
| `SOUND_PLAY_EX`, `STATE_SOUND_PLAY`, `CUSTOM_STATE_DELETE_SOUND` data | Add, Elesis, Luciel, Arme, Aisha (18 / 2 / 2 June scripts) | Engine readers ported by C9h-4a; the keys live only in class scripts. | Nothing extra in code: they work once the class scripts are adopted. |
| Class code writing `+0x3f00` directly (`32ab9190`, `32ab9370`, `32b2c7d0`, `32b77b30`, `32c4a2a0`: `lea ecx,[esi+0x3f00]` + the pair push) and class callers of `AddEffectSetDeleteCondition` `3227a570` (`32ab8890`, `32aba550`, `32ac3100`, `32ac8a60`, `32b7db80` x2) | not identified (found by C9h-4d review's raw scan) | Register effect sets for June's `DCES_SKILL_STATE_END` list (C9h-4d's `m_vecJuneEffectSetToDeleteOnSkillStateEnd`) or for the delete conditions, from class state code. | The class code, calling `AddEffectSetDeleteCondition` (case 5 and the played-handle push are ported) or pushing the `{ handle, skill id }` pair. |
| `USEFUL_VALUE0..4` per-state float rows (June user `+0x8ef4` Future set and `+0x8eb8` Now set, 5 rows x 3 floats, a miss is 0; C9h-4f ports both sets, both parses and the getters `JuneGetUsefulValueFuture( row, col )` / `JuneGetUsefulValueNow( row, col )` of `CX2GUUser`, flag `SERV_IRUHADEV_JUNE_USEFUL_VALUE`) | every class with June scripts: 37 Future-set readers and 72 Now-set readers in class code (`june2017_re/c9h4f_useful_value_readers.tsv`, e.g. Future `329d7220`, `32a02f10`, `32b6ced0`; Now `3299a9d0`, `32a5c2a0`, `32ac2090`) | The row-and-column meaning is per class state (a value is a time, an offset or a speed depending on the state: read the state's June function). | The class state functions, reading the getters instead of June's raw `+0x8ef4` / `+0x8eb8` offsets; the data is already in the June class script's state table (`USEFUL_VALUE0 = { a, b, c }`). **Engine readers that are June-only states and are not here:** `31fd0260` (USI_BOUNDING_LANDCRASH, C3d-3), `31fc8620` (each class's `*_WAIT_STATE_CHANGE`), `31fc9a20` / `3200d450` / `3200d1d0` (USI_EVENT_SNIPER_*): all in C9h-4g |
| `USI_WAIT_STATE_CHANGE` (June base id `GUSI_END + 125` = 0x82; March 101 since C9h-4g, flag `SERV_IRUHADEV_JUNE_RESERVED_STATE_IDS`) registered in every class InitState (all 12, e.g. Eve `329ba9c0 @329bae2a-329bae48`: table `<prefix>WAIT_STATE_CHANGE`, only OnEventProcess = `31fc8620`), and WIN's OnEventProcess = the delegate `320718d0` (all 12 class Inits, e.g. Eve `329bac9e`) | every class; all 13 June class scripts carry the table (e.g. `ESSI_WAIT_STATE_CHANGE`: `ANIM_NAME = ""`, `USEFUL_VALUE0 = { 1.5, 0, 0 }`, `INVINCIBLE`, `RETAIN_CAMERA_STATE`) | After the WIN animation ends the unit goes to the WAIT_STATE_CHANGE state and, once its Future state time passes `USEFUL_VALUE0[0]`, to `USI_WAIT`. C9h-4g ported the EventProcess (`JuneWaitStateChangeEventProcess`), the helper and the delegate's state change (made only when the class registered the state; flag `SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE`). | In the class InitState: `JuneInitWaitStateChange( "<prefix>WAIT_STATE_CHANGE" )` (it logs `JUNE_FX C9h-4g` and registers nothing when the table is missing), and on WIN's state data `stateData.OnEventProcess = SET_CB_FUNC( CX2GUUser, JuneRandomLoseEventProcess )`. Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` before relying on it. (Aisha: CH1j, both in `June_InitState`; live in the final CH1, which checks the empty `ANIM_NAME`) (Aisha: CH1, live; empty `ANIM_NAME` accepted: `ChangeAnim( L"" )` returns false, the WIN pose holds) |

### 7i. Skill-slot re-equip snapshot call (from June effect completion C9h-6)

Every June class's `SetEquippedSkillFuncBySkillSlotIndex` (vslot `0x7b4`, slot 493) calls `31fd2af0( normal state )` before the slot
state's `Init()`. **Drop that call when porting the function.** June needs it because its running state data are pointers into the
state vector. Since skill-system-core S8 (2026-10-19, flag `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR`) March has the same model
(`m_itNowStateData` / `m_itFutureStateData` into `m_StateList`) and the setter (`CX2GUUser::JuneSnapshotRunningStateFunc`), and the
engine makes June's call itself: `JuneSnapshotBeforeSlotRebind( slot, bSlotB )` runs right before both `SetEquippedSkillFuncBySkillSlotIndex`
call sites (`ChangeEquippedSkillState`, `SetEquippedSkillFunc`), on the same normal-state entry the class picks. So the class port still
drops the call (a second one would only re-read the same delegates). A class port that adds a new caller of
`SetEquippedSkillFuncBySkillSlotIndex` calls `JuneSnapshotBeforeSlotRebind` first. Class code multiplies `m_fNowStatePowerRate`
(June `+0x7cc8`), never the entry's `m_fPowerRate`. Funcmap rows `Effect C9h-6`, `Skill S8`.

| June | Class | What the port must bring |
|---|---|---|
| `32d223e0` Elsword, `32cede00` Arme, `32cb06d0` Lire, `32c641a0` Raven, `329c8a00` Eve, `32c1f530` Chung, `32bd09d0` Ara, `32b8c470` Elesis, `32a28e00` Add; June-only `32b46580` Luciel, `32ad9b60` Rose, `32a71eb0` Ain | all 12 | Nothing for the snapshot. The same functions set state byte `+0x11` = 1 on the normal and hyper entries (`+0x10` = `m_bHyperState`). `+0x11` is an unidentified June-only `UserUnitStateData` field, so settle it when the class function is ported. Settled: `+0x11` is the slot-state byte June reads through user slot 330 (`3208e430`; CommonStateStart). March answers it with `IsJuneSkillSlotState` (`X2GUUser.cpp`), so a class port drops the store. (Aisha: CH1k) |

### 7j. DamageReact damage steps that only class code drives (from June effect completion C10d)

June's user and NPC `DamageReact` run a few class steps between the BBT 78 add and the shown-damage copy. june-effect C10e ports
the engine half: the empty virtuals (CX2GUUser `0x4f8` / `0x4fc`, CX2GameUnit `0x304`) and their call sites. The class bodies
below come with the class. Funcmap rows `Effect C10d`.

| June | Class | What the port must bring |
|---|---|---|
| `32b7beb0` (CX2GUUser vslot `0x4f8`, attacker side) | Elesis | Override of C10e's empty hook, called with `( DD, damage )` when Elesis hits a user (`320462a0 @32049626`) or an NPC (`327b1f30 @327b446d`, inside the level-damage / attacker gate). When she is not in state vslot `0x200`, vslot `0x724` > 0, DD `+0x564` is 1..3 and vslot `0x674` == 2, it creates a damage effect whose power is vslot `0x724` x damage. |
| `32b898a0` (CX2GUUser vslot `0x4fc`, attacker side) | Elesis | Override of the second empty hook, called right after `0x4f8`. Gates: not vslot `0x200`; short `+0x9748` != 0; DD attack type != `AT_SPECIAL`; the DD defender is set; a defender buff test (`3227baf0`); a random roll (vslot `0x1cc`) < float `+0x974c`. Then it creates an effect scaled by the damage. Name the members from Elesis's June scripts when ported. |
| `320462a0 @3204963d-32049674` + `32aebaf0` | Lu/Ciel (`UT_LUCIEL` 10, backlog Stage 8) | An inline step in the user `DamageReact`, with no virtual: if this unit's type is 10 and `32aebaf0( damage x 0.2 )` returns true, then damage x 0.8. `32aebaf0` returns false unless `BTI_BUFF_SHADOW_LU` 478 or `BTI_BUFF_SHADOW_CIEL` 479 is on. Otherwise it drains `+0x968c` by its argument, erases both buffs once that reaches <= 0, and returns true. The port inserts the step after C10e's `0x4f8`/`0x4fc` call and before `322cfca0`, which is June's order. |
| `32aa9c70` (CX2GameUnit vslot `0x304`, attacker side) | Rose | Override of C10e's empty hook, called with `( damage, DD )` after the HP-rate step (user) or `322cfca0` (NPC). It acts only when DD `APPLIED_SKILL_ID` == `RNSI_A_RCR_BLOOD_BULLET` (31306). k = 1, or with a DD defender and a non-zero `322a0090( 99 )` count, k = count x skill value `0x31` + 1, after which 99 is erased. Then `322780b0( skill value 0x3f x damage x k )`. |
| `320462a0 @32046a55-32046aa0` + `32d0c820` (found by the C3e-5 review) | Elsword Rune Slayer (`UC_ELSWORD_RUNE_SLAYER` 101) | June replaces part of March's NEW_SKILL_2010_11 magic-reflect roll (`X2GUUser.cpp` "UC_ELSWORD_RUNE_SLAYER == GetUnitClass()"). If DD `bCanReflexMagic` and `32d0c820()` returns true, DD `+0x3c` (the magic damage) becomes 0 and there is no roll. Otherwise March's roll runs as before. `32d0c820` returns true only when all of these hold: the defender buff test `3227baf0( 0x12 )`, skill `0x53aa` (21418) learned with templet `+0x174 == 5`, and `m_pUnit +0x3c` != 0. It then raises MP by skill value `0x2b` at the learned level (vslot `0xf0`) and creates an effect at the unit (`322d38f0`). Name the skill and the buff from June's Rune Slayer data when ported. Funcmap row `(C3e-5 review)`. |

### 7k. Class-set callback that only class code drives (from June effect completion C3)

| What | June | Notes |
|---|---|---|
| Callback at `CX2GUUser +0x8dc0` | `31fcd640( this )` runs `jmp [this + 0x8dc0]` when it is non-zero | Called after the `SetScale` of every RT_CREATE_DAMAGE_EFFECT child: `CX2GUUser::AttackResult 32012b50 @32013d66` and `CEffect::AttackResult 31f85910 @31f86a50` (random-spawn child, owner a user). Nothing in the engine writes the pointer, so it is a class hook. March has no member and the port does not call it. The class port that finds a writer adds the member and the two calls (`X2GUUser.cpp` RT_CREATE_DAMAGE_EFFECT case; `X2DamageEffect.cpp` random spawn). |

### 7l. Super-armour and bounding hooks that only class code drives (from June effect completion C3b)

| What | June | Notes |
|---|---|---|
| Add's directional super-armour call | `CX2GUAdd` DamageReact override `32a07fc0 @32a080cc`: vslot `0x81c( 0, DD )` (= `JuneIsSuperArmorAgainst`, C3c) after its own DamageReact / DamageReactStateChange calls, by reActType; false -> `32a03850` | The engine method lands with C3c. The Add port calls it where June does |
| `SUPER_ARMOR_FRONT` states | Rose_Ranger (4 states), Luciel_Duo (4) | Works once C3c is in: the key is parsed in `CommonStateStart` for every class. Nothing to add per class |
| Bounding user states | `USI_BOUNDING_DOWN / _LANDCRASH / _UP` (June `GUSI_END + 115..117`) registered in every class's own InitState (all 12; funcmap row `USI_BOUNDING_DOWN / _LANDCRASH / _UP (C3b)` lists the functions), each with its `*_DAMAGE_BOUNDING_*` state table (e.g. `Rose_Gunner.src.lua` RSI_DAMAGE_BOUNDING_*, `USEFUL_VALUE0 = { 0.16, .. }` on LANDCRASH) | Delegates: DOWN EventProcess vslot `0x5d4`, LANDCRASH EventProcess `0x5d8`, UP StateStartFuture `0x5dc` / StateStart `0x5e0` / EventProcess `0x5e4` (all `CX2GUUser` base, C3d). March's state id is a `UCHAR` and Chung's last state is already 255; June's id is 16-bit. **D10 answered (a), 2026-10-08:** C3d-2 widened the id. C3d-3 put in the engine half and `CX2GUUser::JuneInitBoundingStates( prefix )`, but no class calls it. **C3d-4 (user, 2026-10-09): every class port registers the states, March's nine included.** Each port calls `JuneInitBoundingStates( "<prefix>" )` in its class InitState where June registers them, and brings June's class script with the three `<prefix>DAMAGE_BOUNDING_*` tables inline. The first port also runs C3d-3 review's play-test legs: the registration log line (`JUNE_FX C3d-3`) and a bounding hit (Aisha: CH1j, `JuneInitBoundingStates( "AVSI_" )` in `June_InitStateCommon`; live, with the play-test legs, in the final CH1) (Aisha: CH1, live; the bounding-hit leg is not reachable offline: only player skills of Lu/Ciel, Ain and Code Empress carry RT_BOUNDING) |
| Lu/Ciel bounding-target hook | `CX2GUUser` USI_BOUNDING_UP StateStart `31fd02d0`: when the last attacker (`+0x40e0` UID, `+0x40e8` unit type 0) is a user whose CX2Unit type is `0xa` (Lu/Ciel): `32b38cb0( type, uid64, +0x40dc )`; the NPC twin `3275a070` does the same | The engine side (the members, the start) is C3d's; the Lu/Ciel port supplies `32b38cb0` (the `LSI_BOUNDING_TARGET_*` combo) |
| Pre-switch class byte `+0x9175` | `CX2GUUser::DamageReactStateChange 31feabd0 @31feacf4-31fead30`: next to March's `m_bQuickenGuardBlocking` (`+0x9173`): byte cleared, then reActResult SMALL -> class state `0x14d`, else `0x14c` (June ids) | The class that owns `+0x9175` adds the member and the branch (June order: after the `+0x9173` block, before `+0x9174`) |
| Packet-buff refusal hook | `CX2GameUnit` vslot `0x3e0( BTI )` (June base `331a4950` returns false): the first test of the per-factor packet apply `322a6d00` (and so of `SetKBuffFactorPacketToGameUnit` and the TRANSIT debuff), true skips the factor. `CX2GUAdd 329d4640` overrides it: true for BTI `0x156` (342) and `0x1b6` (438); `CX2GUAin 32a38f80` returns false; no other class overrides it | The engine side lands with C3c as `CX2GameUnit::JuneIsPacketBuffRefused( BTI )` (virtual, default false). The Add port overrides it with those two BTIs (they exist in March's `BUFF_TEMPLET_ID` only if the Add port adds them) |
| Lu/Ciel bounding-target check (from C3j) | Luciel virtual `32b34680` (Luciel vtable `0x334f06fc` slot `0x568`): for the target at `+0x96f0`, calls `JuneIsBoundingLandCrash()` (vslot `0x2f4`) and `JuneIsBoundingUp()` (`0x2f0`), then state change `0x1fe` / `0x1ff`, or clears `+0x96e8..+0x96f8` | The engine side is done (C3j: pure virtuals on `CX2GameUnit`, user `31fd7ec0` / `31fd7f00` and NPC `3275a010` / `3275a030` overrides). The Lu/Ciel port calls them. On an NPC target the result depends on C3j's fallback decision |

### 7m. Damage-effect die notify that only Add drives (from June effect completion C9h-8 review)

| What | June | Notes |
|---|---|---|
| `NOTIFY_TO_OWNER_WHEN_DIE` hook signature | `~CEffect 31f78550 @31f785c6-31f785cf`: owner vslot `0x2c4( CEffect* this )`; base `3208ddc0` (no-op, `ret 4`), only override Add `329f73a0` | March's `DieDamageEffectProc( const DamageData& )` keeps its signature; the engine side (the call moved to the dtor head) is ported as `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_NOTIFY_OWNER_FIRST`. June's Add reads only the effect's `m_DamageData` (`+0x200`) through the pointer, so the class port may switch to `CEffect*` or keep `DamageData&`; both carry the same values. |
| Add's phase-shift die handler | `CX2GUAdd` `329f73a0`: `m_eUsedSkillID` (`DD +0x590`) is `RNSI_A_AN_PHASE_SHIFT` (29201) **or** `RNSI_A_AN_PHASE_SHIFT_UPGRADE` (29216), `m_bNoDamage` (`DD +0x4dc`) is 1, and the phase-shift handle (`+0x9764`) is valid: zero `+0x9758..+0x9760`, handle = -1, then vslot `0x198` (`IsMyUnit`) -> `329f7370( this +0x9750 )` | March checks `SI_A_AN_PHASE_SHIFT` only. The Add port brings the reform IDs and the upgrade ID. |

### 7n. User-sync class-skill float that only class code drives (from June effect completion C3e)

The engine side is ported by june-effect C3e-1 (`SERV_IRUHADEV_JUNE_SYNC_CLASS_FLOAT`). That phase adds a `SyncData` float (June protected
slot `+0xa4`, plain `+0x58`, Future `+0x7ed8`, Now `+0x8178`) and user-sync bit `0x200000`. June's class code writes it on Future and reads
it on Now. Nothing in March drives it until these rows are ported. Classes are confirmed by vtable (funcmap `3200d3f0 / 3200d900 (C3e)`).

| Class | June writer (Future) | June reader (Now) | Notes |
|---|---|---|---|
| Rena | `32c88050` (Grand Archer `RNSI_P_RGA_RANDOM_SHOT_PLEASURE`, 23415: +/- by the up / down keys) | `32c86620`, `32c9d420`, `32ca2520` | Registered by `32cb06d0` (Rena vtable `33518d70`). |
| Eve | `3297d730` | `329a67b0` | Registered by `329c8a00` (Eve vtable `334cfc10`). |
| Chung | `32c0ba60` | `32c0ba60` | Thunk `32c256f0`, registered by `32c1f530` (Chung vtable `33509e30`). |
| Elesis | `32b73bf0` (StateStart: base vslot `0x7c0`, then the float = 0) | `32b7d550` | Thunks `32b906b0` / `32b909b0` from `32b864f0` (called by Elesis Init `32b87a60`). |

Rose (Neil the Sniper `RNSI_SA_RFY_NEIL_THE_SNIPER`: `3200d3f0` / `3200d900`, in base code), Ain and Lu/Ciel drive it too. They go to
backlog Stage 8 with the new characters.

### 7o. Fall-speed limit data and the minor unit's physics (from June effect completion C3g)

The engine side is ported by june-effect C3g (`SERV_IRUHADEV_JUNE_LIMIT_MAX_G_SPEED`): a state's `SET_LIMIT_MAX_G_SPEED` below 0 replaces
`fMaxGSpeed * buoyancy` as the fall cap, read at every user state start (default 1.0 = off). March's class scripts carry no such key, so the
user half does nothing until a class port brings June's state tables. Carry the key over with the tables, verbatim:

| Class | June states with the key | Value |
|---|---|---|
| All 12 classes in 7l (13 tables: Lu and Ciel have one each) | `<prefix>DAMAGE_BOUNDING_DOWN` | -4000 | (Aisha: CH1m, user reader `X2GUUser.cpp` C3g; arrives with the script in the final CH1) (Aisha: CH1, script adopted)
| Raven | `RRF_DASH_JUMP_COMBO_XX` | -5000 |
| Add | `ANSI_APT_DASH_JUMP_COMBO_ZX` | -4000 |

- **Lu/Ciel's minor unit.** June integrates a second PhysicParam (`+0x36c`; `MINOR_UNIT_PASSIVE_SPEED_X/Y`, its `APPLY_MOVEMENT_SPEED`
  copied from the main one) in step with the main one (funcmap `32015b30 @3201745a-32017861 (C3g)`). March has no minor unit; it comes
  with the Lu/Ciel port (backlog Stage 8). June builds the limit for it and then ignores it (JUNE_BUGS 142): keep that verbatim.
- **Ara's `APPLY_MOVEMENT_SPEED = TRUE`** (one state, `ASI_AIRIAL_DASH`, `Ara_Martial_Artist.src.lua:7334`; it also carries
  `PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED * 1.5`). The engine side is ported by june-effect C3h (`SERV_IRUHADEV_JUNE_APPLY_MOVEMENT_SPEED`):
  at state start, the state's `nowSpeed.x`, `PASSIVE_SPEED_X` and every `SPEED_TIME` x speed are scaled by `GetWalkSpeed() / fWalkSpeed`
  (the movement-speed buffs), and so is the state's x decay every frame. Carry the key over with Ara's June table, verbatim; it does nothing
  until then. The minor unit's half (June copies the bool to `+0x36c` and scales the minor speeds and `MINOR_UNIT_SPEED_TIME`; its decay
  uses the main unit's `GetWalkSpeed()` over the minor unit's own `fWalkSpeed`) comes with Lu/Ciel (funcmap `(C3h)` rows `3205419d`, `32015b30 @32016996`).
- **June's SPEED_TIME additions** (june-effect C3l, `SERV_IRUHADEV_JUNE_SPEED_TIME`; funcmap `(C3l)` rows). The engine side is ported: a 5th
  TIME_SPEED column (TRUE turns the entry by the unit's `dirDegree.z`, with June's `y = x cos sin`, JUNE_BUGS 153), iFlag 11 / 12 / 13
  through the user hook `JuneGetTimeSpeedVector` (June vslot `0x778`), and the `SPEED_TIME_ULTIMATE` choice (its gate stays off until the
  skill templet's `m_DeleteSkillIDByUpgrade` is ported, backlog Stage 3c). March's class scripts use none of it. Carry verbatim with the tables:
  - **Ara:** `ASI_A_ARA_QUICK_STAB` `SPEED_TIME_ULTIMATE0/1` (2 rows); `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH` `SPEED_TIME0 = { 2550, 0, 0.496, 2, TRUE }`.
  - **Elesis / Shay:** `ELESIS_KNIGHT` and `PVP_HERO_SHAY` each carry `SPEED_TIME1 = { 150, 127, 0.527, 0.8, 2 }`: iFlag reads `(int)0.8 = 0`, so the
    row never fires (the 5th value 2 reads true). Keep it as June has it.
  - **Rose (Stage 8):** `Rose_Freyja` uses iFlag 12 five times (`RSI_RVK_NITRO_MOTOR_JUMP_UP`, `_UP_FRONT` x2, `_JUMP_FRONT` x2). Rose overrides
    the hook (`32a7ab60`): copy the vector; byte `+0x977c` == 2 -> `y *= +0x8ef8`, > 2 -> `y *= +0x8efc`; then if `+0x9778 > 0` and byte
    `+0x9762` == 1 -> `x *= +0x9778`. Port it as Rose's `JuneGetTimeSpeedVector` override.
  - **Lu/Ciel (Stage 8):** the `MINOR_UNIT_SPEED_TIME(_ULTIMATE)` twins (`+0x8604`, 262 June rows), the main/minor list swap on the second body
    (`+0x8e90`, `31fe3a60`), the anim-clock override before the consumer loop (`+0x8f6d` + vslot `0x260` + `+0x8e91`, `@3206211c-32062172`)
    and the minor list's own consumer pass (`@320623c9..`).

### 7p. Hit-stop class sites and the time-stop immune state (from June effect completion C3e-3)

june-effect C3e-4 (`SERV_IRUHADEV_JUNE_HIT_STOP_LISTS`) ports the engine side:
- SetStopTime / SetStop2Time take four arguments, `( fStopTime, bDamageDataStop, pAttacker, bForce )`;
- the user keeps per-attacker lists;
- the state key `TIME_STOP_IMMUNE_STATE` is read at every user state start.

A state with the key ignores every unforced hit-stop on the player's own unit. March's class scripts carry no such key, and the class sites
below still call the 1-argument form. Carry them over with June's arguments (`june2017_re/c3e3_stop_time_callers.tsv` rows 21-23):

| Class | June site | June call |
|---|---|---|
| All classes | `TIME_STOP_IMMUNE_STATE = TRUE` in 76 June states over 16 class files (`grep -c TIME_STOP_IMMUNE_STATE june_src_fixed/data036_kom/*.src.lua`) | the state table, verbatim | (Aisha: CH1m, 12 states, reader C3e-4; arrives with the script) (Aisha: CH1, script adopted)
| Chung | DamageReact `32c0cf90` (vslot `0x60`) @32c0d18c / @32c0d19c / @32c0d1b4 | attacker `SetStopTime( 0.5, false, NULL, false )`, own `SetStopTime( 0.5, false, NULL, false )`, own `SetStop2Time( 0, false, NULL, false )` |
| Aisha (Violet Mage) | binding circle hyper FrameMove `32cd5d60` @32cd6152 (March `AVSI_SI1_AV_BINDING_CIRCLE_HYPER_FrameMove`) | `SetStopTime( 3.0, false, NULL, true )`: the exe's only forced stop, so it overrides the immune state **(Aisha CH0, 2026-10-20: n/a. `32cd5d60` has no pointer, call, jmp or thunk anywhere in the exe; June registers `AVSI_SI1_AV_BINDING_CIRCLE` for both slots with FrameMove `32cd57c0`, so June never runs this forced stop.)** |
| Elsword | March `THROW_WOODEN_PIECE_FrameMove` `SetStopTime( 1.5f )` | no June `+0x100` call loads 1.5: confirm June dropped it, then drop it too |

### 7q. Luciel's down-state getters (from June effect completion C3f)

June's force-down flush (C3f-1, `SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH`) compares the state being left with three virtual getters, and
ForceDownAndDetonationProcess compares the new state with a fourth. March's getters are non-virtual (`X2GUUser.h:1987`) and C3f-1 uses the
base ids. Luciel overrides all four; make them virtual (or test the class) when Lu/Ciel is ported.

| June vslot | Base (12 classes) | Luciel | Luciel returns |
|---|---|---|---|
| `0x114` DOWN_FRONT | `3208de60` -> `0x1e` | `32b4d810` | if `31fc9d90( 2 )` (byte `+0x8f6e` or `+0x8f6f` set): `0x1a8`, `+0x2d` when vslot `0x260` (`3208e160`, byte `+0x8e90`) is set; else `0x1e`, `+0xb4` when vslot `0x260` is set |
| `0x118` DOWN_BACK | `3208de70` -> `0x1f` | `32b4d850` | same, base `0x1f` (the `0x1a8` / `0x1d5` branch is identical for all four) |
| `0x48c` AIR_DOWN_INVINCIBLE | `3208deb0` -> `0x24` | `32b4d990` | same, base `0x24` |
| `0x490` AIR_DOWN_LANDING | `330de330` -> `0x25` | `32b4d9d0` | same, base `0x25` |

### 7r. Chung's cinematic-UI writes (from June effect completion C3o review)

June moved Chung's `SetFocusCamera / SetFocusCameraForce / SetApplyCinematicUI( false )` on the targeting NPC. Neither site changes
anything on screen (JUNE_BUGS 164; the readers need the NPC's start state), but the Chung port should match June's placement.

| Class | June site | March site | Port |
|---|---|---|---|
| Chung (Missile Shower 6019, Quantum Ballista 6011) | `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE` StateStartFuture `32bff3d0 @32bff412-32bff420` (state `0xf5`, registered `@32c1fffa`) | `CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess` (`X2GUChung.cpp`, before `StateChange( ..._CONTROL_IDLE )`) | move the three writes into a CONTROL_IDLE StateStartFuture; June has no `IsMyUnit` test there |
| Chung (June-only Heavy Field) | `CSI_SA_CTT_HEAVY_FIELD_LOOP` (`0x151`) `32c0ba60 @32c0ba9f-32c0bac2`: every frame, when vslot `0x198` is true, the same three writes, then the NPC state from a 9-way switch (`SHOT_ARTILLERY_STRIKE_*`, only when it differs) | none | comes with the skill |

### 7s. Dungeon-registry class tables only the new classes read (from June effect completion C3z-3)

Since C3z-3, June's `DungeonData.lua` rows carry per-class tables March's `AddDungeonData_LUA` has no branch for
(`X2DungeonManager.cpp:252-552` reads COMMON / ELSWORD / AISHA / RENA / RAVEN / EVE / CHUNG / ARA / ELESIS / ADD only). The data is
already live; only the readers are missing. Count per class: `june2017_re/dungeon_registry_census.tsv`, column `june_unread_class_tables` (220 shared rows since C3y-1a; counts re-taken in the C3z-3 review, 2026-10-16).

| Class | Tables | Port |
|---|---|---|
| Ain | `BOSS_DROP_ITEM.AIN` and `ENDING_SPEECH<n>.AIN` (96 tables in the shared rows) | add the `AIN` branch to both loops, mapped to Ain's `UT_` |
| Luciel | `BOSS_DROP_ITEM.LUCIEL` and `ENDING_SPEECH<n>.LUCIEL` (101 tables); the speech entries are `{ LU = STR_ID, CIEL = STR_ID }` pairs, not `{ STR_ID }` | the branch plus June's two-speaker speech read (June `327e0f80`, the `ENDING_SPEECH` block `@~1837-2001`) |
| Rose | `BOSS_DROP_ITEM.ROSE` and `ENDING_SPEECH<n>.ROSE` (93 tables) | add the `ROSE` branch |

### 7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)

Since C3t (`SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME`), `CX2GUUser::ShowHyperSkillCutInByScript` (June `32014fc0`) shows a texture at a state's
`SHOW_HYPER_CUT_IN_TIME`. It stays dormant until a port brings all three of these:
- **The class script:** every June class script sets the key in its awakening states (`ESSI_*_HYPER_MODE`, plus Ara's berserk ones), 17 files.
  No March class script sets it.
- **June's `UnitTemplet.lua`** (data036): the texture names, `m_HyperCutIn` per class (39 rows), `m_BerserkCutIn` (9),
  `m_MinorUnitHyperCutIn` (4) and the two `_Trans` keys. March's copy has none of them, so the name is `L""`. (Aisha: CH5 - her 7 rows swapped for June's in the core-list `UnitTemplet.lua`; other classes' rows stay March's until their port)
- **The `HQ_Cutin_*_Transcendence` textures** those rows name.

Class hooks (engine side ported, base bodies in `X2GUUser.h`; the class overrides are this list's):

| Class | June | Port |
|---|---|---|
| Chung | unit vslot `0xa4` override `32c24b90`: `+0x9714 > GetRandomFloat( 0 )`, a berserk roll (identify `+0x9714`) | override `JuneIsBerserkByChance` |
| Ara | user vslot `0x51c` override `32bd85f0`: now state `0x17` or `0x9b` (`ASI_BERSERK_MODE`) | override `JuneIsHyperModeState` |
| Luciel | user vslot `0x51c` override `32b4dbe0` (state `0x17` or a Luciel state: read it at `@32b4dc12`) | override `JuneIsHyperModeState` |
| Luciel | in `32014fc0`: with the second body (vslot `0x260`, `+0x8e90`, 7o) and a non-empty `m_MinorUnitHyperCutIn`, that name. Then, for unit type 10 / class `0x81` (`UC_LUCIEL_DEMONIO`): future state index (`320761a0`) `0x1c2` without the second body -> `m_HyperCutIn_Trans`, or `0x1f6` with it -> `m_MinorUnitHyperCutIn_Trans`, each only when not empty | add both branches after the berserk one (the comment above the function has the order) |

**Hyper active skills (from C3t-1, `SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION`):** June's `GetShowActiveSkillShow` (`31fd3180`) shows the cut-in
of an `ST_HYPER_ACTIVE_SKILL` state for my unit or a party member whatever the cut-in option says (Hide included). Engine side ported. It stays
dormant until a port brings June's hyper active skill templets: March's `NewSkillTempletVer2.lua` (data036) has 521 templets and none of
type 13 (`ST_HYPER_ACTIVE_SKILL`; only placeholder 903 "Specifc Hyper Active", type 0), and no save unit has an `SI_HA_*` skill. Play-test it
with the first ported hyper active: option on Hide, use it -> the cut-in shows; a normal active -> none.

**Sound sets (from C3t-2, `SERV_IRUHADEV_JUNE_SOUND_SET_PICK`):** the same three overrides above (Chung `JuneIsBerserkByChance`, Ara / Luciel
`JuneIsHyperModeState`) also pick a state's `HYPER` / `BERSERK` sound set in `CX2GUUser::CommonStateStart`, so until they land Ara's
`ASI_BERSERK_MODE` state picks as a plain state. The June class scripts bring the keys that only the June pick reads:
`USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE` (June's spelling; `ELESIS_KNIGHT`, 3 states), `SOUND_SET3_RATE` and up (`Add_NasodRuler`,
`Chung_Iron_Cannon`, `ELESIS_KNIGHT`, `Eve_Nasod`, `Rose_Ranger`), and `STATE_SOUND_PLAY<n>` inside a sound set (`Add_NasodRuler`, `ELESIS_KNIGHT`).
Engine side done; nothing to port per class beyond the overrides.

**Skill cut-in names (from june-gfx G4, `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN`):** June's user skill cut-in is a GFx widget: `ActiveSkillShow` appends `.gfx` to the name
it is given and plays it (with `Common_CutIn_Frame` and a `CUT_IN_TALK_` balloon) through the root movie. June's class tables therefore hold names **without an extension**
(`HQ_CutIn_Elsword_01`, `HQ_CutIn_Raven_BM_01`, `HQ_CutIn_Ain_AAP_hyper`), and the files are `.gfx` in data152 / data194 / data195. March's tables (`s_SkillCutInSet`, `s_SkillCutInSetFoolsDay`) name
`.dds` textures that are other names (`HQ_CutIn_Elsword_1.dds`, `HQ_CutIn_Raven_Blade_Master_1.dds`): none of the 52 has a `.gfx` (`june2017_re/g4_cutin_names.tsv`, with June's files of the same
class prefix beside each). **Each class port replaces its table with June's names** (and the set / index layout June's class function reads: June `329cd890` + `32078b70` per class), dropping the
`.dds`. Until then a class's skill cut-in shows the frame and the balloon but not the hero art. Dev console `JuneGfxCutIn( "HQ_CutIn_Elsword_01" )` (G4's `_DEBUG`, while on) shows a full one. (Aisha: CH5 - table `s_SkillCutInSet` = June `333ab660`, event 0x8b3 pair `333ab4d0`; the item override `31fd8a50` -> Stage 12.)

### 7u. March-only effect templets the June re-adoption dropped (from June effect completion C11a)

C11a (2026-10-16, user: full June re-adoption, route b) replaced data036 `DamageEffect.lua` / `EffectSet.lua` with June's own files. The merged
March copies held 50 damage effects and 37 effect sets June's files do not have; the full list is `june2017_re/c11a_march_only_templets.tsv`.
30 of them are still named by **March class code**, so those skills now play without that effect (no crash: every C++ site NULL-checks
`CreateInstance` or discards the result, and a missing effect set logs `[RANOX_EFFECT_MISSING]`). The port of each class replaces the March
caller with June's, which does not name them; until then they are a visible gap, not a bug to patch here.
| class | C++ sites (`X2Lib/`) | class Lua (data036, March bytecode) |
|---|---|---|
| Raven | `X2GURaven.cpp`: `RAVEN_SEVEN_BURST_1/2_HYPER`, `RAVEN_MAXIMUM_CANNON_HYPER`, `_FLY_HYPER`, `RAVEN_OVER_TAKER_GUARDIAN_STRIKE[_HYPER]`, `RAVEN_OVER_TAKER_XX_FRONT_X`, `EffectSet_RAVEN_VJAVELIN` | `RAVEN_FIGHTER.lua`: `DamageEffect_RRF_ExplosiveHand_Impact`, `EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE`, `..._HYPER_MEMO`, `EffectSet_RAVEN_VJAVELIN_LAND`, `Effectset_A_RWT_FLAME_SWORD_MEMO` |
| Elsword | `X2GUElsword_SwordMan.cpp`: `LORD_KNIGHT_SAND_STORM`, `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER` | - |
| Rena | `X2GULire_ElvenRanger.cpp`: `DamageEffect_LGA_Arc_Shot`, `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` | `Lire_ElvenRanger.lua`: `Effectset_Rena_DiveKickBombingA/B`, `..._B_Memo` |
| Aisha | `X2GUArme_VioletMage.cpp`: `EffectSet_Aisha_DeathField_FirstImpact`, `EffectSet_Aisha_DeathFieldBomb` | `Arme_VioletMage.lua`: `EffectSet_AHM_Blaze_Step_Damage_MEMO`, `EffectSet_Aisha_Blaze_step_MEMO` | (Aisha: CH2b - the Death Field names are June's `EffectSet_SA_ADM_DeathField` / `_Bomb` now; the Blaze Step ones are CH2a's)
| Chung | `X2GUChung.cpp`: `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell`, `EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT`; `X2BuffTemplet.cpp`: `CHUNG_BIGBANGSTREAM_MEMO` | `Chung_Iron_Cannon.lua`: `EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE` |
| Eve | `X2GuEve.cpp`: `EVE_EP_DASH_COMBO_Z` | - |
Not a class: `BOMB` (`X2GUNPCFunc.cpp`, an NPC function, and June's `Bunker.lua`) has no owner; noted in the C11a Done note.
Two notes from the C11a review (2026-10-16):
- Rena: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` is a June case rename, not a loss. June defines `DamageEffect_Cryotron_Bolt_Guided_Arrow`
  (`june2017_re/june_lua/DamageEffect.src.lua:28956`); March's `X2GULire_ElvenRanger.cpp:23238` asks for the all-caps key and the Lua table
  lookup is case-sensitive. The Rena port takes June's caller and its spelling.
- Chung: `X2BuffTemplet.cpp:1695-1706`. On the 20% memo-26 roll the `else` skips `HAND_GRENADE_0`, and the missing memo templet returns
  NULL, so the function returns (guarded; `X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE` is unconditional, `KTDX.h:328`). With memo 26 equipped, one
  grenade in five now fires nothing until the Chung port.

### 7v. Lu/Ciel minor-unit slot key (from June engine U0)

June's `CX2SlotItem` slot-data reader (vslot 11, `32e284b0`) reads `IS_FOR_MINOR_UNIT` (bool, @`32e2870a`) next to March's slot keys
(`X2SlotItem.cpp:53-65`, `LuaGetValue`). June's character-window and closet dialogs use it on Lu/Ciel's second-unit slots
(`DLG_Magical_Closet`, `_NEW_2`, `DLG_UI_CHARACTER_NEW_2/3/4`). Only the Lu/Ciel port needs it: port the key, its member and its readers
with that class. Under March data the key never matches (no minor unit). Funcmap row `32e284b0` (`Engine U0`).

### 7w. CUSTOM_RENDER_STATE class users (from June engine R0)

June reads the state key `CUSTOM_RENDER_STATE = { start, end, CRS }` with the generic parser `31fce500`, from the user state loader
(`32057b80`) and the riding-pet state loader (`32d2f560`, which copies the triple into the rider). The user render setup `31fd4460`
renders it (cases in `june2017_re/r0_renderer_contract.tsv`, `crs` rows). R5 ports the engine side. These class scripts carry the key
and come with their ports: `Add_NasodRuler` (`CRS_INVISIBLE` x6, `CRS_UNIT_FADE_WHITE_WITHOUT_WEAPON` x1), `Ara_Martial_Artist`,
`Ain_Apostasia`, `Ain_Erbluhen_Emotion`, `Luciel_Demonio`, `Rose_Ranger`, `RidingPet_Rakshasa` (all `CRS_INVISIBLE`). The `Enum` table
itself is the `CUSTOM_RENDER_STATE` row at line 152. Funcmap rows `31fce500`, `31fd4460` (`Engine R0`).
### 7x. Combo hits must carry June's AT_COMMAND tag (from Atlas E10)

June's class scripts tag combo `DAMAGE_DATA` with `ATTACK_TYPE = ATTACK_TYPE.AT_COMMAND` (e.g. `Lire_ElvenRanger`: 52 `AT_COMMAND`, 11 `AT_SPECIAL`); March's leave combos
untagged, which parses as `AT_NORMAL` in both clients (June `32632d46`-`32632d5e`). The NPC state key `INVINCIBLE_ATTACK_TYPE` (Soless Stage3's
white seal: only `AT_COMMAND` breaks it) needs the tag, so Atlas E10 ships a stand-in, `SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN`: a user's
`AT_NORMAL` hit counts as `AT_COMMAND` for that test only (`X2GUNPC.h` `GetInvincibleByAttackType`). Every class port takes June's script with
its tags; once all classes in play are ported, undefine the stand-in. Other `AT_COMMAND` readers (buff BBT 85/86/87/96, socket
`EXTRA_DAMAGE_TYPE`) already depend on the same tags.

### 7y. GetLandPosition's bMonster in class code (from June engine W1c)

June's `CKTDGLineMap::GetLandPosition( pos, bool bMonster, fRadius, pLineIndex )` (`31c7bb00`) skips `LT_MONSTER_ROAD` lines unless the bool is
true. W1c ports it with the March signature meaning `true` and gives June's `false` to the 10 shared user sites. Class code that calls it
passes the bool June passes (`june2017_re/w1c_land_position_callers.tsv`): Eve's Dimension Link states pass `true` (already March's). June adds
a call March lacks in Add's `ANSI_A_AN_PHASE_SHIFT_BLINK_StateStart` (`32a176c0` @`32a1787e`, `true`: lands the return position, March
`X2GUAdd.cpp:8224` sets it unlanded) and in an Elesis anim-event NPC create (`32b7a0a0` @`32b7a354`, `true`, NPC `0x663` / `0x7b3`). Ain
(`32a69467` `false` then `32a6989f` `true`), Rose and Luciel (all `true`) come with their classes. Lu/Ciel's minor-body block in
`CX2GUUser::PhysicProcess` (`@320194bb`, `false`) belongs with 7o.

### 7z. Lu/Ciel skin equipment attaches every model (from June engine R14)

June's `CX2Eqip::_UpdateSkin()` (`3295ae20` @`3295aed0`-`3295aef9`) attaches every model of a skin item, not only the first, when the owner unit's type is
10 (`UT_LU_CIEL`), the eqip's ctor flag (+0x15d4, the bool the model-name selector `32867880` takes with the unit type; ctor `32961280` @`3296148d`) is clear,
and the item has a second model (+0x5a8). R14 ported the function with this term as a comment only (`X2Eqip.cpp`, `SERV_IRUHADEV_JUNE_EQIP_SKIN`): the
Lu/Ciel port adds the ctor flag and the term. Related: 7v (minor-unit slot key), 7o (minor-unit physics).

### 7aa. Lu/Ciel minor-unit attach-anim models (from June engine R14b)

June's attach-anim init (`CX2Eqip` `3295dfa0` @`3295e2cf`-`3295e4f6`) builds a second anim / model pair per slot (+0x584 / +0x5e4) when the eqip has an
owner and the templet's unit type is not 9 / 0xb: the owner templet's minor-unit item for the equip position (`329657d0`, June-only unit-templet fields
+0xe4..+0x100 for EP 2 / 6..11 / 0x24), its templet (`325bb740`), and, when the worn item has `m_bIsForMinorUnit` (flags2 bit 2), the templet of its
`m_ItemIDForMinorUnit` (+0x30) instead; model name `32867210`( slot ), Set / Add with the normal XETs (collision 1), ChangeAnim( m_AniName ), the
`GetJiggleBone` (`3295b230`, DITT 30) jiggle. `3295a570` also changes this third array's anim. R14b left it as a comment (`X2Eqip.cpp`,
`SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM`). Its own 0xb remap (slots 4..7 -> 0..3) is dead code in June (the outer test excludes 0xb). Related: 7v, 7z.

### 7ab. Rose weapon attach frames (from June engine R14b)

June's attach-anim init (`3295dfa0` @`3295ea4f`-`3295ec1e`) binds a Rose weapon (templet unit type 0xb `UT_ROSE`, EP_WEAPON_HAND) to attach frame 1
(`328671e0`( 1 )) for slots 1..4 and attach frame 0 for the others, before the default per-slot frame. March has no Rose: R14b left it as a comment
(`X2Eqip.cpp`, `SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM`). The Rose port adds the branch with `UT_ROSE`.

### 7ae. June skill-tree ids with no March class state (from June skill system S3)

Since S3, data036 ships June's `NewSkillTempletVer2.lua` and every save is on June's trees (schema v17). The census
`june2017_re/s3_reform_ids_no_state.tsv` lists every tree row of the 55 classes March has (2,044 rows) with its June name, templet name, type,
master level, row / column, and whether the class family's GU file has a state for it. **1,989 rows have none.** Every reform id (21000+, `RNSI_*`) is
absent from March's C++ `SKILL_ID` enum, so no class `case` can name it. The only rows with a state are `SI_ETC_WS_COMMON_LOVE` (900), one per class.
Per family (rows / no state; first job, second job): base classes 15 / 14 each (Raven 14 / 13); first jobs 28-34 / 27-33; second jobs 52-57 / 51-56.

What a class port does with its rows:
- **Add the `RNSI_*` enumerators** (values from June's `Enum.lua`) and the state for each one in the class's `SetEquippedSkillFuncBySkillSlotIndex`
  and its Lua script. Until then, two March behaviours apply. An id whose type the class switch lists (BUFF / ACTIVE / SPECIAL_ACTIVE / HYPER)
  falls into that switch's `default:` and plays the class's first skill state (Elsword Mega Slash, Lire Power Attack, Arme Power Attack, Raven,
  Eve, Chung Detonation, Ara Wind Wedge); Elesis and Add leave it blank.
- **June's types 14-16 (`ST_SPECIAL_ACTIVE_GRADE_1..3`), 17 (`ST_CORE`) and 18 (`ST_CORE_PASSIVE`) are not in March's C++ `SKILL_TYPE`.** Every
  class switch sends them to `default: return`, so the slot state is blank. S3 logs such a slot once (`[JUNE_SKILL] MIG no-state skill`) and
  `CX2GUUser::StateChange` refuses the blank state (`SERV_IRUHADEV_JUNE_SKILL_REFORM`); MP and the cool time are still spent, as March's
  `SpecialAttackEventProcess` charges them before the state change. The port removes that path for its class by binding the state.
- `lookalike_march_name` in the tsv names March's old skill with the same suffix (e.g. `RNSI_SA_ES_MEGASLASH` 21000 vs `SI_A_ES_MEGASLASH` 1200):
  a hint for where the old state's code lives, not proof that the June skill behaves the same.
- 49 rows (Chung upgrade ids 26027-26029, 26216-26219) have no June `SKILL_ID` name; name them from the templet.
- **Since S3c (2026-10-18)** `CX2SkillTree::SKILL_TYPE` has `ST_SPECIAL_ACTIVE_GRADE_1..3` (14-16), `ST_CORE` (17) and `ST_CORE_PASSIVE` (18)
  (`SERV_IRUHADEV_JUNE_SKILL_TYPE`). A class switch names them as June's does (June's class `SetEquippedSkillFuncBySkillSlotIndex` blocks
  in `june2017_re/s3c_skill_type_readers.tsv` group 7 with 14-16). A class port that calls `ResetLeftSkillCoolTimeBySkillType( ST_SPECIAL_ACTIVE )`
  needs June's rule from `32895390`: a request for 7 matches skills of type 7, 14, 15 and 16. March's version compares exactly, and no
  non-class March caller asks for 7, so S3c left it. (Aisha: CH4c, n/a: Aisha never calls it. June's only two callers are Ain's, `32a4c37c` type 6 and `32a762e2` type 7: Stage 8.)
- **Ain's gauge charge (from the S3c review, 2026-10-18; Stage 8).** `32622bd0` (on a hit) and `32622e80` (when hit) are thiscalls on
  `g_pData +0x58`. They take the unit's (physic + magic) damage times a reaction factor. In `32622bd0` that is times the applied skill's
  `+0x130` level array, with a factor of 1.0 / x / x for types 14 / 15 / 16 and one of its own for 5. Then they feed unit vslots `0x878` / `0x86c`.
  Engine code calls them only for unit type 12 (`UT_AIN`): `CX2GUUser::AttackResult` @32013519 and `CX2GUUser::DamageReact` @32049af5.
  `CX2GUAin` `32a76f00` calls the first too. The Ain port adds both call sites to March's `AttackResult` / `DamageReact` behind its own flag.
  The constants are at `33439b24`, `3348a3a8`, `3343b240` and `33438088`. Rows: `june2017_re/s3c_skill_type_readers.tsv`.
- Every class block of June's that names a skill type is in the same tsv, with `class material` in its action column.

### 7ad. Character motions: June's renamed animations (from June engine R12b)

June engine R12b ported June's motion order (`SERV_IRUHADEV_JUNE_RENDER_MOTION_ORDER`: the main motion is the templet's `m_GameMotion0`,
`MOTION_FILE_NAME` is mixed) and took June's `Motion_Raven.x`, `Motion_CHUNG.x`, `Motion_EL.x` and `Motion_EL_SK.x` into data033 for their
collision spheres. Every other `Motion_<Class>*` file of the nine March classes is still March's (83 files; user, 2026-10-18), because June
renamed animations its class scripts play and March's scripts still use the old names. `june2017_re/r12b_anim_loss.tsv` lists the 43
animation names March's motions have and June's same-named files lack (Elsword, Arme and Eve: none). **Aisha CH0 (2026-10-20):** per file, `Motion_Arme_EM.x` does drop `EM_METEOR_SHOWER_Start` / `_Loop` / `_End`, which June moved into `Motion_Arme_HM.x`; the EM class loads both, so stage the two together.

What a class port does:
- **Stage June's `Motion_<Class>*` files** with the class script (`python tools/kom.py stage-june <list>`; the R12b list builder is the
  `^motion_(elsword|arme|lire|raven|eve|chung|ara|el|add)[_.]` pattern over `kom_contents` against June's koms). Keep script and motions in
  the same pack: Lire's `ComboX3` / `ComboX4` / `ComboX4Up` / `ComboX4Down` / `DashAttackX` became `Lire_ComboX3` ..., Elesis's
  `SI_A_EL_LEAP_CRASH_*` / `SI_A_EL_RUSHING_SWORD` became `SI_A_SSK_*` (in `Motion_EL_SK.x`), Raven's `OT_ComboX3Down` .. `OT_ComboX5Down`
  and `OT_SP1a_ChargedBolt`, Add's `APT_NA_Ready` / `APT_NA_preAttack`, plus skill animations of Add, Ara, Chung, Elesis and Lire.
- **Elesis already runs June's base and SK motions** (R12b), so these 6 March animations are missing now, accepted by the user until this
  port / skill S3: `SI_A_EL_LEAP_CRASH_Start`, `_Jump`, `_Landing`, `SI_A_EL_RUSHING_SWORD`, `SI_SA_SSK_SPIRAL_BLAST`, `SI_SA_SSK_VICTORIOUS_SWORD`
  (June deleted the last two).
- Re-run the census for the class after staging; a name still missing is a script fix, not a motion hold.
- **Elesis frame count:** June's `Motion_EL.x` (live since R12b) has 79 frames, March's `MESH_EL_*_SORTED.X` parts 80 (one unnamed
  frame; every named bone matches), so each Elesis unit logs KEM 433 `Anim Frame : 79, Mesh Frame : 80` per part (66 in the R12b
  play-test). Warning only; June's Elesis part meshes, staged with this port, end it.

### 7ac. CanDown's June call shape in class code (from June engine W1f)

June's `CKTDGLineMap::CanDown` (`31c6c310`) reads the entry line from a separate in index (NULL skips the `bOnLine` head and the no-hit fallback) and
writes the landed line to an out index. W1f ports it as a 9-arg overload, `CanDown( pos, r, pOutPos, pInIndex, pOutIndex, bDownRadius, bIsMonster,
bOnLine, bDroppable )`; March's 7-arg form forwards (index twice, `false`). Class code takes June's shape from `june2017_re/w1f_candown_callers.tsv`
(`w1f_action` = `-> checklist 7ac`):
- Ara: `SLockOnSystem::ProcessLockOn` (`3200fa20` @`3200ff98`, March `X2GUAra.cpp:5435`), `ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK` EventProcess (`@32bc204c`,
  `:11264`) and `_FINISH` StateStart (`@32bb70ff`, `:11371`) pass `NULL, &lineIndex` (March seeds `lineIndex = 0`, so its head tests line 0). June also
  calls `CanDown` (all NULL) in `ProcessLockOn`'s candidate loop (`@3200fcad`) and clears a flag when it fails; March's loop has none. June's
  `ProcessLockOn` is shared with Luciel.
- Chung Artillery Strike Ready (`@32bfe8c3`, `X2GUChung.cpp:17152`) and Aisha Hell Drop (`@32cd97ba`, `X2GUArme_VioletMage.cpp:16482`) pass the in
  index and a NULL out index; March's out local is unread, so nothing changes. Aisha's June state always creates `NUI_UFO_AISHA_DROPPER_MEMO` (0x627). (Aisha: CH3a - `June_AVSI_SA_AVP_HELL_DROP_FrameMove`; Lightning Shower's two `CanDown` calls with a NULL out index wait for CH3a2.)
- June-only helpers with `NULL, &idx`: user landing prediction `32021390` (Add / Luciel / Arme callers), target-follow prediction `3200eeb0`, line clamp
  `32294530` (Add), Ara landing helper `32bc0420`, Rena Grand Archer `LESI_RGA_GUNGNIR_READY` (`@32ca111a`); Luciel's states (`@32b2fd79`, `@32b33fb7`,
  `@32b33d09`) come with the class. (Aisha: CH1g review, `32021390` only, as `CX2GUArme_VioletMage::JuneLandingPrediction`; it steps nowSpeed.x, not y)

### 7af. Camera linear tracking and view roll that only class code drives (from June engine R13c)

R13c ports June's engine camera whole (`SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK`): `CKTDGCamera::SetTracking( ..., bLinear, fRollTo )` (`31d01b70`),
`GetLinearTracking` / `SetLinearTrackingTime` (camera `+0x90` / `+0x94`), the view roll (`+0x68`, `GetRoll`) and `CX2Camera::OnFrameMove`'s linear
gate (`31fb5800`). March's `CX2Camera` functions keep their signatures and pass June's defaults ( `false`, `0` ) (`june2017_re/r13c_settracking_callers.tsv`).
What only class code reaches:
- **`LINEARMOVING = TRUE`** sub-camera tables (`CAMERA_TYPE.CT_PARTS_LOOK_TRACKING`) exist only in `Ain_Apostasia.lua` (13) and `Ain_Erbluhen_Emotion.lua` (3)
  (data036). June's Lua camera table `31fb5920` reads it (`@31fb6123` / `@31fb63e8`) and passes it as `SetTracking`'s 9th argument through June's
  12-arg `PartsLookTrackingCamera` (`31faee30`, `a11` / `a12`) and 10-arg `NomalTrackingCamera` (`31fb3640` / `31fae910`). Ain's port extends those
  `CX2Camera` signatures (and `PlayLuaCamera`'s key reads) and passes them on.
- **The SUB_CAMERA time sequencer** `31ff5df0` (`@31ff61e9-31ff62a1`, user side; CH3a2: first written as `31ff5440`, which is `DoAimingTarget`, the function before it): picks `SUB_CAMERA%d` by elapsed time, plays it (`31fb6af0`), then sets
  the camera's step time to the overshoot (`31fae7b0` = `GetCamera().SetLinearTrackingTime( t )`). March plays SUB_CAMERA tables from class code by name
  (`X2GUArme_VioletMage.cpp:6865` etc.).
- **`TILTANGLE`** (roll, `-deg * 0.017453292`) is read by the same Lua table but appears in no June or March file: dormant.
- June's `PartsLookTrackingCameraFromPartsEye` (`31fae6d0`) takes a 5th byte for `+0x90`; its June callers pass the unit pointer's low byte
  (C3v review quirk), which is never 1 for an aligned pointer.

### 7ag. Pressed-index space, the fifth slot and the A5 / B5 states (from June skill system S5)

S5 made the engine 5 + 5 (slot ids A 0-3 + A-TC 4, B 5-8 + B-TC 9; `CX2UserSkillTree::SKILL_SLOT_*`, the wire, the gage list, `GetSlotIndexAndSlotB`). **It deliberately left the pressed-index space alone**:
`CommonSpecialAttackEventProcess`'s `iSkillSlotIndexPressed_`, `m_iPressedSkillSlotIndex`, `CountUsedSkill`, `GetPressedSkillState` and `CurrentSkillKeyManager` still use March's 0-3 (A) / 4-7 (B), because about 35
class sites (`> 3`, `-4`, `aNormalSkillStateIDSlotB[idx - 4]`) read it that way. A class port that adopts June's class code converts at that moment:
- June's combined index is A 0-4, B 5-9 (`3288fe30`: `> 4` is B, minus 5). Its pressed key reader `31fcadd0` also returns **4** (F, `GA_SKILL5`) and **9** (T, `GA_SKILL_EX5`, only while slot-state key 1 is usable).
  Change `CommonSpecialAttackEventProcess` and every `> 3` / `-4` decode of the ported class together; the `InputData` groups `oneSkill5` / `oneSkillEx5` already exist (S5 only consumes the press). (Aisha: CH1m, June `31fcadd0` ported whole as `CX2GUUser::JuneCommonSpecialAttackEventProcess`; March's reader stays for the unported classes)
- The TC slot's states `USI_SPECIAL_ATTACK_5` / `_HYPER_5` / `USI_SPECIAL_ATTACK_SLOTB_5` / `_HYPER_5` exist and the state arrays have 5 entries, but `SetEquippedSkillFunc*`, `SetEquippedSkillLevelCommon`, `CalcEquippedSkillLevelPlusAndMPCost`
  and `ChangeEquippedSkillState` skip the fifth slot, so no class `SetEquippedSkillFuncBySkillSlotIndex` ever sees it. Stage 6 (transcendence) removes those skips and gives each class its TC cases.
- A-TC is locked because March has no transcendence flag (`CX2Unit +0x3c`, June `328dfab0` / `3220e3b0`); the emulator refuses an equip into 4 / 9. Stage 6 adds the flag and sends key 1 = usable.
- `X2GuEve.cpp` (Mega Electronball, Taser Pilum, Kugelblitz) stores a **slot id** that is March's 4 + 4 (A 0-3, B 4-7), the pressed-index space: its readers are the key switches (`case 4: oneQ`), the
  `iSkillSlotIndexPressed_ =` assignments, the three `GetSkillSlotData*` decoders (`> 3`, `-4`) and the state-array lookup. S5 first moved only the three encoder loops to June's numbering; the S5 review
  (2026-10-18) put them back to March's (`if( iSlot >= 8 ) break;`) and gave the two `GetSlotIndexAndSlotB` readers a March split, because that splitter is June's 5 + 5 now. An Eve port converts the
  encoders, every reader above and the pressed index together.

### 7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)

S6 loads June's `DLG_GAME_SKILL_SLOT_NEW_10slot` and ports its generic parts (slot controls, TC lock pictures, cool-time/fade, key labels, swap offsets). The dialog also carries June's **class statics, all hidden by the file**; nothing
in March drives them, so a class port that needs one shows it and feeds it from its own unit code (June's driver is `CX2Game` `322cd5c0`-`322e07e0`, looping slots 0-9 with `3214f3d0`):
- `Static_EL_SKILL_B(_B)` / `_R(_B)` (Elsword way of sword; March's `CX2Game::UpdateElswordSkillSlotEffect` already drives them, now over 10 slots), `Static_RoseGlow(_B)` (Rose), `Static_EveRecoveryGuide` / `_Cooltime` /
  `Button_EveRecoveryToolTip`, `Static_LucielSwitchingGuide` / `g_pStatic_LucielCooltime`, `Static_AinDisparateForceGuide` / `g_pStatic_AinCooltime` / `Button_AinDisparateForce` (Ain), the memorize slots
  `Static_Memorize_Skill_Slot_TypeA1-3` / `TypeB1-3`, `Memorize_Slot1-3_TypeA/B`, `UseMemorizedSkillHotKey_A/B` (Aisha; needs `STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN` = 29059, already in Enum.lua; Aisha: CH4d, driven by `CX2Game::June_RefreshMemoRiseSlot` `322deb80` and `June_ShowMemorizeStatics` `322cf350`), and `Static_Install_Skill` /
  `Static_GCore_Skill` (10 pictures each; June toggles them per slot through `328e0290` for classes 9 and 0xb).
- `Button_V_Key_ToolTip` (the character-system V key) needs `GAGE_UI_CUSTOM_MSG.STUCM_CHARACTER_SYSTEM_TOOLTIP_ON/OFF` handling (54002 / 54003 exist in Enum.lua only), which March has no feature for.
- The TC buy-guide buttons send `SMUCM_DISABLED_TRANSCENDENTAL_SKILL_SLOT_MOUSE_UP` / `_EXPAND_..._MOUSE_UP` (208 / 209 here, June 182 / 183); `CX2StateMenu` only logs the click until Stage 6 gives them a transcendence guide.
- **Key config.** The option dialog's key rows and icons (`X2KeyPad::UpdateUI` icon switch, `X2StateOption::MappingKeyConfigIndex`, `DLG_UI_KEYSETTING` art keys `SKILL_A5` / `SKILL_B5`) have nothing for `GAMEACTION_SKILL5` / `_EX5`
  (F / T): the keys work and the HUD labels show them, but they cannot be rebound in the options. Owner: skill-system-core S6b (`kickoffs/skill-system-core/S6b.md`), not a class port.

### 7ai. Buff-factor calls by BFI, the reserved-buff hook and the 3 s skill-name string (from June skill system S3e)

S3e gave `CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID` June's three extra arguments (`322a6960`, vslot 0x21c): `( BFI, level, pCreatorUnit_ = NULL, iAdjustSkillID_ = 0, bSkipAdjust_ = true )`. March's
class calls still pass `( BFI, level )`, which keeps March's behaviour (no slot-476 memo rescale, no creator stamp). June's class code passes its own values, and most of it passes **skip 0**, so slot 476
rescales the factor by the state's reserved-buff skill id (`+0x9274`) when that skill has an active memo. Some calls pass an explicit adjust skill id (e.g. Ara `32a4dcaa` 0x7ea0) or a creator unit (e.g. `3275a587`, `32b0fccc`).
- A class port passes June's arguments at every call it ports. The map is `june2017_re/s3e_bybfi_callers.tsv`, part 2: each June call address, its function and its pushes in push order (`skip|adjust|level|creator|BFI`).
- (Aisha: CH2a - Binding Circle FrameMove `32cd57c0` calls `SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_BIND, 1, this, 0, false )` on each enemy in range; the other 7ai Aisha rows stay with their phases.)
- **User vslot 0x830** (`CX2GUUser::JuneOnStateInsertBuffFactorApplied( BFI, level )`, empty) runs after each reserved buff factor of a state is applied, in the frame apply and at state end. Only **Rose** overrides it
  (`32ad9a40`: it looks the factor up and walks a list at `+0x9818`). The Rose port adds the override.
- **`CX2GUUser::JuneShowSkillNameStateString( iSkillID )`** (June `3200ab60`, static) shows a skill's name as the state string for 3 s. Its callers are class code: `32b818b0` (skills 28201 / 28218, after `32b80c60`),
  `32bc84b0`, `32d1ddf0`, `32ccfb92`-`32ccfc91`. The port of each calls it where June does. (Aisha: CH1i, the five calls in `June_NoStateChangeActionFrameMove`; Shadow Body passes 2218, JUNE_BUGS 376)
- **First live use of the reserved-buff path.** `BUFF_FACTOR_ID_AND_APPLY_TIME` appears only in Elesis's script among user classes (Endurance Power, its memo state, Flame Sword `{ BFI 299, 0.1 }`). S3 left those states keyed on March ids
  (8217) while the save holds June's (28210), so the `[JUNE_SKILL] SBUF parse` / `frame` / `state end` lines first appear with the Elesis port. That port's play-test checks them.

### 7aj. Tree-UI class pieces June keyed to one class (from June skill system S7b)

S7b ports June's skill tree UI (`CX2UISkillTreeNew2015`). These parts of its class block only do something for one class and need that class's runtime state, so the class port adds them:
- **Equipped-slot icon overrides** `328e0aa0` (from the equipped slot's `UpdateUI` `328e18d0`): per class type (0x6d, 0x7b, 0x72, 0x73, 0x1a Eve, 0x7e, 0x24) a skill shows another icon while a class state holds
  (Eve's El Crystal index at tree `+0x134`, Add's `+0x138` / `+0x139`, the game unit's vslot 0x98 / 0x56c). March keeps only its Eve `UpdateEveElectraSkillSlotIcon` call, which S7b ports as June's tail.
- **Install / G-Core marks** `328e0290`: classes 9 and 0xb toggle `Static_Install_Skill` / `Static_GCore_Skill` per slot through the game unit's vslot 0x71c.
- **Slot data types 5 and 6** (`328d9ac0` / `328e0f10` Aisha memorize slots, ctor caller `322deb80`; `328d9b00` / `328da050` Luciel transform slot, caller `3200f380`), the equipped slot's
  `SESAPT_MEMORIZE` picture (`328e0460` case 2, `AISHA_MEMORIZE_SIGN`, `328df840` through vslot 0x714), the memorize mouse-in case 29059 (`328ec719`) and the transform-slot cases 29078 / 29079 (`328ed6a8`; 29079 only hides the tooltip, ported).
  (Aisha: CH4d, type 5 = `CX2MemorizeSkillSlotData`, the `SESAPT_MEMORIZE` picture, 29059 without June's write over the slot (JUNE_BUGS 400). Type 6 and 29078 are Luciel's: Stage 8.)
- **Luciel's portrait** `328dcba0` / `328db7a0` (unit type 10: the form flag at tree `+0x8c` from unit `+0xe40`, `DLG_UI_Common_Texture91/89/96/103_NEW` keys).
- **Aisha's memorize line** in the tooltip (`328e9160` @328e9710): unit type 2, templet `+0x170 == 3` (`32205af0`), `"

"` + STR 30357 with the key name of game action 0x27 (`31f5ee50`). March's templet has no `+0x170` field. (Aisha: CH4d. `+0x170` is March's `m_iWOSType` (`m_usWOSType` in the Lua), the field CH1i's `June_IsMemorizableSkill` reads; action 0x27 = `GAMEACTION_SIT`.)
- **Rose's sub-text costs** (`328e2900`, unit type 11: alternate templet MP and raw FP with STR 38426) are ported; they show once a Rose unit exists.
- **Luciel's command group** (from S7e): the tree's `JuneSetCommandGroup` (June `328e8cd0`, ported) is called only by the unit's form change `3234d930` (my unit's `+0x28` / `+0x2c`
  match, then `3214f050`, `328e8cd0( form )`, `328dcba0`). The class port calls it; the tree's Luciel `_MINOR` command lists (`328e7ba0`, unit type 10) and `COMMAND_DATA_MINOR` (`3220bdb0`,
  June's minor flag = my unit `+0x3c`) are ported and need March's Luciel unit type and that flag.

### 7ak. Skill page decide ACK class pieces (from June skill system S7d)

June's decide-page ACK (`3220f2b0`) has two class branches after `InitEquippedActiveSkillState( false )` (vslot 0x684), on the game unit's type (`31e4d0e0`). S7d ports the rest of the handler; the class port adds these:
- **Unit type 2 (Aisha):** `32ce1770( &true )` on the game unit (@3220f4dc), a walk over the unit's class list (`32cf1b10` / `32a78b90`). (Aisha: CH4d, `June_SweepMemoRise`.)
- **Unit type 9 (Add):** the tree UI's `328e0290( string 3347f014 )` (@3220f53a), the same install / G-Core mark refresh as 7aj.

### 7al. Class pieces CH1l found in the user base (from pilot-char-revamp CH1l)

- **Class 13 after-image toggle:** June's base of user vslot 0x850 (`31fd8230`) toggles the after-image `+0x194 / +0x1d1` for class 13 before
  `OnFrameMove`; the CH1l hook `JuneAfterImageFrameMove` keeps March's plain call in the base. Port the toggle with that class.
- **`+0x9440`, the MP a reserved skill spent** (written @32057ce2 in CommonStateStart): read only by `32b60650` / `32b63be0`. Name that class and
  write it into a CX2GUUser member before its states read it.
- **New CX2GUUser hooks for every class** (LIVE): `JuneAfterEquipSlotChange` (0x654), `JuneAfterImageFrameMove` (0x850), `JuneSummonedNPCInfo` (0x860),
  `JuneReservedSkillStart` (512), and the virtual `UpdateEquipCollisionData( vector& )` (0x638). A class port overrides the ones June's class overrides.

### 7am. Unit delegate-list registrations per class (from pilot-char-revamp CH4b)

June's five lists are March's four plus the area-hit list (`SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST`): `+0x66ac` attack result
(`InsertDelegateProcessToAttackResult`), `+0x66bc` damage react, `+0x66cc` frame move (`InsertDelegateProcessToCommonFrameMove`), `+0x66dc` damage
react with data (`InsertDelegateProcessWithDamageDataToDamageReact`), `+0x66ec` area hit (`InsertJuneDelegateProcessToAreaHit`, nobody registers).
Each class registers in its `InitPassiveSkillState` (vslot `0x690`) after the base, which clears the lists. Port a class's pushes with it, each
callback whole (the list entry is a `push edx; call <fn>; ret` thunk; the target is listed). Whole-binary scan, CH4b:

| Class | Registering function | Pushes ( list: callback ) |
|---|---|---|
| Elsword | `32d1c560` | A `32d0c4e0`, A `32d1b9d0`, C `32d08ba0`, A `32d0baf0`, A `32d0bc60`, D `32d0c180`, A `32d05af0`, A `32d1a370` |
| Rena | `32ca7860` | A `32ca2180` (Open Mind), A `32ca2390`, A `32c86790`, A `32ca28b0` |
| Raven | `32c57660` | A `32c3df50`, C `32c4e5b0`, A `32c28160` |
| Eve | none | (her summons register through the NPC, below) |
| Chung | `32c16be0` | A `32c12910`, A `32bdba10` |
| Ara | `32bcd9c0`, and `32bb0d10` (called from it) | A `32b95910`, A `32ba7e40`, A `32b95980`, A `32b959a0`; A `32baf810`, A `32baad30` |
| Elesis | `32b88300` | A `32b52470`, A `32b6fd50`, A `32b7e3b0`, A `32b524b0`, A `32b6f820`, A `32b872d0`, A `32b87880` |
| Add | `32a1ccb0` | A `32a04ce0`, A `329d4190`, A `32a01d20`, D `32a01dd0` |
| Lu/Ciel (Stage 8) | `32b43d10` (slot `0x7a4` of `334f06fc`) | A `32b14bc0`, A `32b3d380`, A `32b0fc50`, A `32b43590`, A `32b435f0`, A `32b0fce0`, A `32b177b0`, A `32b13c80`, A `32b17f90`, C `32b3e470` |
| Rose (Stage 8) | `32ad1080` | A `32aa7e90`, A `32ab41c0`, A `32ac5ce0`, A `32ab42f0`, A `32aa7e10`, A `32a7aa80`, D `32ab5720`, A `32ab1710`, A `32ab54f0`, A `32ab04d0`, A `32a7eab0`, A `32aacf00` |
| Aisha | `32cecd70` | A `32ce9f90` (Open Mind 22416; Aisha: CH4b) |

- **NPC-side pushes for two classes' summons** (`3275f0a0`, an NPC vslot): every NPC gets A `3275a120` (owner `UT_EVE`, skill 25417, MP text) and
  A `3275a490` (owner `UT_LIRE` -> Rena's `32ca2180`). Port each with its class (Eve, Rena); the registration lives in `CX2GUNPC`.
- March's own pushes (Elsword, Raven, Ara, Elesis, Add) are March's callbacks; a port replaces them with June's set above.

### 7an. User passive hooks and specialization readers per class (from pilot-char-revamp CH4c)

CH4c added two empty `CX2GUUser` virtuals under `SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS`, both called by the engine as June calls them:
- `JuneGetDefenceRateBySkill( attackType )` (vslot `0x428`, base `3208dd70` = 1.0) rates the defence term of `GetFinalDamage` (`32284aa0 @32285cf9`, user attacker only). March's inline Shadow Death block is gone.
- `JuneGetIncreaseMpRateOnDamageReact( DD )` (vslot `0x73c`, base `3208dd70`) multiplies `fIncreaseMpRate` in the user `DamageReact` (`320462a0 @32049d78`).

| Item | Classes (June override) | What the class port brings |
|---|---|---|
| vslot `0x428` | Ain `32a4d570`, EL `32b52260`, Ara `32b956a0`, Aisha `32cc28b0` (Aisha: CH4c) | the override, whole |
| vslot `0x73c` | Add `329d46b0`, Raven `32c3c7f0`, Aisha `32cc5c30` (Aisha: CH4c) | the override, whole |
| vslot `0x748`, soul gain when hit (`DamageReact @32049d8a`, times `fIncreaseSoulRate`) | Ain `32a387b0`, Chung `32beeca0`, Raven `32c3c820` | the base virtual (1.0) and its engine call, then the override; not ported yet |
| `GetCharacterSpecializationB()` (June `+0x6d04`) readers | Eve `329b407b`, Add `32a01ca2`, Ain `32a59fc5`, Rose `32ac77ab`, Lu/Ciel `32b14950` / `32b3794d`, Ara `32ba6f31`, Chung `32bf528d`, Raven `32c49410` / `32c507da`, Rena `32c85838` (Aisha `32cd0197`: CH4c) | the reading function, whole |
| `AttackResult`'s Way-of-Sword MP factor (`32012b50 @32012d98`, `@32012ee3`): `1 - SpecializationB` in Vigor / Destruction, else `1 + SpecializationA` | Elsword, Elesis | engine block in `CX2GUUser::AttackResult`, with the class |
| `GetFinalDamage @32285d0a`: unit type 11 (`UT_ROSE`) in states `0x15a` / `0x15b` scales a term by `+0x9798` | Rose (Stage 8) | the branch, with Rose's member |

### 7ao. Transcendence, per class (from june-transcend T0)

June marks a transcended unit with an int at CX2Unit `+0x3c` (T1 adds it to March's `UnitData`). The engine half is june-transcend T1-T5; Aisha is
T6. Every other class port brings its own rows below (evidence `june2017_re/transcend/t0_readers.tsv`, owner `class <Name>`).

**Closed 2026-10-24 (june-transcend Z): the engine half (T1-T6c) is done and Aisha's row is marked; the walk (T8) passed.** Engine pieces a class port reuses, not redoes: the unit flag, `/tc`, the guide popup, the items, TC slots A-5 / B-5 (type 3 glow `322d8100`), the awakening engine, the class pictures (`GetCharacterImageName` unit-slot rows: a port adds its class's `list_TR_*` rows, T6c). Every other class row below stays open for its port.

Every class also brings:
- its id in the emulator's transcend allow-list (T2; user decision 2026-10-22) and its `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_<Class>.lua`;
- its cases in `JuneGetHyperModeTableName` (June `31fdd490`, by class 100..) and its transcended hyper cut-in names (`HQ_Cutin_*_Transcendence`);
- its hyper-class skills (`SPRT_HYPER_CLASS`), their states and `TRANSCENDENTAL_SKILL_DATA` rows;
- the four base skill helpers with the flag test, if the port copies them as Aisha did: `31fcac90`, `31fcad70`, `31fceab0` (`== 1`), `31ff1e80`;
- its hyper-effect start, which returns at once for a transcended unit (June's WakeUp FX in `32298590` replaces it).

Base-code branches that june-transcend T5 found and left to the class ports (none of them is transcendence):
- Raven: `HyperModeStart` `31ffd490`, re-awakening while awake: skill 24419 (`0x5f63`) learned -> vslot `0x23c( 24419, 1, 1 )` (June_SetSkillBuffFactors).
- Rena: `ApplyHyperModeBuff` `32298590 @322985ec`: unit type 3 -> `32cb50c0` count x 15.0 to vslot `0xf0`, then `32cb6290( 5 )` (ADD_RENA_SYSTEM's
  nature force; March US compiles that system out).
- Add / Ain: `OnFrameMove` `3206eaf0 @3206fed1` counts down `+0x9330` (their hyper StateStart sets it before `31fc8c20`).
- Any class with an aura-accelerator skill: `ApplyAuraAcceleratorBuffToGameUnit` `32028790` is June-changed (targets: same team, within 490000,
  `CanApplyBuff`, users only; March also takes PvP allies and party members). It has no transcendence test (T0's row was a stack-slot match).

| Class | June readers of `+0x3c` | What the class port brings |
|---|---|---|
| Elsword | `32d17e90` (hyper effect start, vslot 19); `32d0c180` (SetEquippedSkillFunc `32d27181`); `32d0c820` (21418 Magic Coating, from DamageReact `320462a0`); `32d1b9d0` (skill `0x53ac`, `== 1`); `32d1c560` (vslot 420, three `== 1`); engine-code readers of his skills: `32063650` CommonFrameMove (21416 Fight God, `== 1`), `32610cb0` (21417 Open Mind) | each branch, whole; the two engine-code blocks go into March's `CommonFrameMove` / the caller of `32610cb0` with him |
| Aisha | `32cddcc0`, `31fcac90`, `31fcad70`, `31fceab0`, `31ff1e80` | **done, june-transcend T6 (2026-10-24):** all five were already June's since T1; passives 22412-22417, the twelve hyper-class actives, memo 22217, TC slots and cut-ins were ported by CH1-CH4 / T4 / T5; T6 adds the `[JUNE_TRANSCEND] ARME passives` log line |
| Rena | `32c9b3d0` (hyper effect start, vslot 49) | the branch |
| Raven | `32c4e7c0` (hyper effect start, vslot 49) | the branch |
| Eve | `32994710` (hyper effect start, vslot 19) | the branch |
| Chung | `32c063d0` (hyper effect start, vslot 49) | the branch |
| Ara | `32bb22d0` (hyper effect start, vslot 49); `31fdd800` (berserk table by class, only caller `32bca580`) | the branch; `31fdd800` whole (CX2GUUser code only Ara calls) |
| Elesis | `32b74300` (hyper effect start, vslot 49) | the branch |
| Add | `329e23a0` (BFI 846 formation buff instead of 315); `329e6f30`, `329e6f80` (skill-id map gates, 12 / 94 callers); `329e7210`, `329e7ca0`, `329e7d60`, `329e99a0`, `329f10e0`, `329f9380`, `32a02260`, `32a047a0`, `32a0bfd0`, `32a15580`, `32a19bb0`; `32a08150` (hyper StateStart, calls `31fc8c20`) | each branch, whole (Stage 7) |
| Lu/Ciel | `32b1f0a0` (hyper effect start, vslot 19); `32b397a0` (CommonFrameMove override, vslot 499) | the branches (Stage 8) |
| Rose | `32ab1c30` (hyper effect start, vslot 19) | the branch (Stage 8) |
| Ain | `32a599c0` (hyper StateStart, calls `31fc8c20`); `32a6a990` (states `0x3c6`-`0x3c8`); `32a71a90` (ApplyHyperModeBuff) | each branch (Stage 8) |
