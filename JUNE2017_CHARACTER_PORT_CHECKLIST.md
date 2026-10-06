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
| **`DAMAGE_RATE_TO_SUPERAMOR`** | binary-only | 8b, **missing from 8b's resume groups** | `Luciel_Duo` ×2 |

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
| Natural force | code exists, gated off: `ADD_RENA_SYSTEM` is **off** under `US_SERVICE` | Rena (`Lire_ElvenRanger`, 77 `NATURAL_FORCE_TYPE` uses) | **a decision for you:** turn `ADD_RENA_SYSTEM` on (the studio's own Rena code, untested in this build) or leave Rena without natural force. Phase 8c's read steps aside by itself when the flag is on |
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
6. Only then copy the character's June script, and rerun `charport_scan.py` to confirm nothing new
   appeared.

## 7. Buff-engine hooks that only character code reads (from the June buff port, 4b-i part 3)

The buff port (`JUNE_BUFF_PORT_PHASES.md`, "Phase 4b-i, part 3") ports the buff side of these and
stops at the character boundary. Each item below is June character code that reads or drives a
buff-side store. Port it with the character, whole, and wire it to the store named here.

| June | Class | What it does | Buff-side store in March |
|---|---|---|---|
| `32bb22d0` / `32bb2740` (vslots 49/50, `HyperModeBuffEffectStart/End`) | Ara | Start: when `+0x9744 > 0` and the buff hyper count `< 3`, a loop over June-only members; End: writes the count back to 0 | `CX2GameUnit::GetHyperModeCountByBuff()` (June `+0x6844`). **March has both functions, June reworked them: port each whole.** |
| `32ba6cc0` (vslot 39), `32ba6d00` (vslot 147: count 1 -> state `0x294`, 2 -> `0x295`), `32badc80` region (vslot 485, 3 reads), `32baf270` (397), `32bc8be0` region (198), `32bc8030` (vslot 17, writes) | Ara | Read or write the buff hyper count | same |
| `32bf4680` (non-virtual, after vslot 465) | Chung | Skips its timer work when the buff hyper count is 3 | same |
| `329f538a` (inside the vslot-16 range) | Add | Reads the buff hyper count | same |
| `32b1e8e0` (vslot 16) | Luciel | Writes 0 at init | same |
| `3227c360` `(op, BTI, index, value)` | Rose only (`32ab2f02`, `32ab2f6d`, `32ae305b`, `32ae30ac`) | Finds the buff by BTI and calls its N-th behavior's vslot 9 (`+= v`), 10 (`*= v`) or 11 (`= v`); overrides exist on NowMp (9, 10) and ChangeStat (11) | callers only. The buff port owns `3227c360` and the three vslots and ports them dormant (`JUNE_BUFF_PORT_PHASES.md`, part 3, "Where part 3 stopped" item 5). **Ported dormant in 4b-i part 5:** call `CX2GameUnit::OperateBuffBehaviorValue( op, BTI, index, value )` (op 0/1/2 = slot 9/10/11) |
| `322a7660` `(BFI, count, level, x)` and wrapper `322a8a70` | Eve (`3299b7f9`, BFI 812), Elesis (`32b7e96c`/`32b7ebae`, 910/913), Rose, Luciel, Aisha (`32cc5c16`, BFI in a register), and `CX2GUUser` code for June-only factors 523/820/877/929 | Sets an existing buff's stack count; `3236a430` then re-runs every behavior's vslot 3 (NowMp, ChangeStat, EnchantAttackRate override it) | callers only. Every constant BFI pushed here is in Phase 2's June-only block. The buff port owns `322a7660`, `322a8a70`, `3236a430` and the vslot-3 overrides and ports them dormant (same item 5). **Ported dormant in 4b-i part 5:** call `CX2GameUnit::SetBuffAccumulationCount( BFI, count, level, x )` (`322a7660`) or `AddBuffAccumulationCount( BFI, delta, level, x )` (`322a8a70`). `x` feeds user slot 476 (`32028250`), a June-only factor adjustment that March logs and skips; June falls back to unit `+0x9274` when `x` is 0, a June-only member, so the caller must pass it |
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
| `329e2340` / `329e2360` (vslots 434 / 435) | Add | The class hook `TryHyperModeStateChange` (`31fe9a90`) asks first: when 434 is true, protected short `+0x7e84` = 0x1c, vslot 435( 0 ), awakening is consumed. Every other class uses the empty base (`31e4c550` false, `3208ddc0`) | `CX2GUUser::TryHyperModeStateChangeByClass()` / `OnHyperModeStateChangeByClass( int )` (Phase 7c, `JUNE_BUFF_AWAKE_LOCK`). **The `+0x7e84` = 0x1c write is not ported:** the member is not identified in March (it is a `KProtectedType` short; `CommonEventProcess` also writes a pending state id from `+0x8e74` into it, so it is probably the future frame's `nowState`). Identify it when porting Add and add the write to the hook branch |
| `32a31550` (vslot 344 `CanHyperMode`) and the five Add functions calling vslot 343 (`329eaa40`, `329eafe0`, `329eb210`, `329eb560`, `329eba20`) | Add | Add's own awakening test, and the Add states that call `TryHyperModeStateChange` (March's `X2GUAdd.cpp` has no `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` use) | `CX2GUUser::CanHyperMode()` is virtual for this override; `TryHyperModeStateChange` (Phase 7c) |
| Every class's vslot 510, the skill lookup June's `SpecialAttackEventProcess` (509) calls first (7c called it `NoStateChangeActionFrameMove`; corrected in Phase 7d): Add `32a20e70`, Ain `32a6dcd0`, Rose `32ad7f80`, Luciel `32b3a310` (+ slot 545 `32b3acf0`) | Add, Ain, Rose, Luciel | Starts with "if the quick-slot special item wait (`+0x8d78`) is running, return false" (no skill). **Phase 7d put this prefix at the top of the eight March classes' `SpecialAttackEventProcess`** (Elsword, Eve, Arme, EL, Ara, Chung, Raven, Lire); these four get it with their port | `CX2GUUser::IsQuickSlotSpecialItemWaiting()` (Phase 7c), flag `JUNE_BUFF_QUICKSLOT_ITEM` |
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
| Spec A (option member `+0x6bb0`, result `+0x6c50`) readers: `328eab00`, `3297ee20`, `32981130`, `329a4680`, `329e59f0` (Add `UpNowDPValue`: DP consume), `32a620e0`, `32ac8f10`, `32b14860`, `32bc9a60`, `32bed5d0`, `32c41170`, `32c43bf0`, `32c56be0`, `32ca7130`, `32cb6290`, `32cc7300`, `32ccda90`, `32ce7480`; spec B (`+0x6c64`, result `+0x6d04`): `329b3cc0`, `329b5230`, `32a01b40`, `32a59e70`, `32ac7700`, `32b14860`, `32b37910`, `32ba6ef0`, `32bf5280`, `32c491a0`, `32c50330`, `32c85690`, `32ccfe30`; generic `CX2GUUser::AttackResult @32012da2` and `CEffect::AttackResult @31f86c29` (`x *= 1 + A`, or `1 - B` when vslot `+0x674` returns +/-2) (buff Phase 16 step 1) | per class | Each class's socket specialization: the socket keys `m_fCharacterSpecializationA`/`B`, curves `CalculateLinear/Final<Class>Specialization{A,B}...` | `CX2GameUnit::GetCharacterSpecializationA()` / `B()` (members `m_AdditionalCharacterSpecializationA`/`B`, seeded and refreshed since Phase 16 under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`); vslot `+0x674` is not named |
| June `3207cd70` (ConvertAndAdd body) and the whole aggregate path (`3204f190`, `32080670`) (buff Phase 16 step 1) | Add | March's `ConvertAndAdd` halves Add's socket hyper charge and hyper time (`SERV_9TH_NEW_CHARACTER`, `fReformValue` 0.5); June has no 0.5 anywhere on the aggregate path. Whether June applies it downstream is not checked | Phase 16 drops it under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`X2SocketItem.h` ConvertAndAdd); check Add's hyper code for a June-side 0.5 when porting Add |
| `3230c1a0` (`CX2Game::SetUserSummonedNPCInfo`, rewritten in June) through the helper `322ece70`'s per-case flags (buff Phase 16-step3b3) | every class with a summon | June builds a summon's stats from the user's own stat line (`322ece70`: user `Stat` `+0xcf4`, scaled by `32071720( skill level )` and by `3344ce94`/`33472a10` when the NPC's `+0xc04` value is > 0), with per-case flags: 0x1 HP constant `3348be20`, 0x2/0x4/0x8 the NPC's own protected HP / attack / defence values (`+0xd14`, `+0xd18`/`+0xd1c`, `+0xd28`/`+0xd2c`), 0x10/0x20 the user's `+0x4074`/`+0x4068`/`+0x4064` multipliers, 0x40 the skill-ability lookups (`0xab`, `0xa1`-`0xa4` when game vslot `0x10c` returns 1 or 4, else `0x14`, `7`-`10`), 0x100 `3231dc20`, 0x200 the protected power factor `+0x8ce4`; then per-case follow-ups (`32c86340`, `32bdb250`, `323115e0` + Hell Drop effect set, `32cc98e0`, `32c878f0`, `31e4d560`, `32c11940`, `31e57960`, `32a2dec0`, ...) and the common tail (`3204e160`, `3200bec0`, `31fd05c0`, vslot `0x860`). March keeps its per-case `SetNPCStat` x `fStatScale`. The engine side (the socket skill-damage term on the power rate) is ported in 16-step3b3 | Port each class's summon cases with the class, from June's flags and follow-ups |
| Chung's Metabolism Boost in `CX2Game::UseItemSpecialAbility` (`322fbc60` HP up `@322fbe08`-`322fbe83`, HP % up `@322fd495`-`322fd510`) (buff Phase 16-step3b3) | Chung | June tests unit class `0x14`/`0x6e` (`31e4d0c0`), skill `0x6720` (32890620 level, ability `0x37`), and inlines `ability - 1.0 + 5e-8` (`3346ed68`), where March tests `UC_CHUNG_FURY_GUARDIAN`/`UC_CHUNG_IRON_PALADIN`, `SI_P_CFG_METABOLISM_BOOST`, and `CalculateIncreasingRate` (epsilon 5e-5). 16-step3b3 kept March's term and added the socket HP-recovery rate beside it | Port June's class test, skill ID and epsilon with Chung |

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
| `32a63ba0` (tag 23: `m_iIndex`); `32a64d00` (tag 17); readers of `m_iIndex` and `m_bHyper` in `32a63e80`, `32a63f60`, `32a64800`, `32a65620`, `32a65860`, `32a6a850`, `32a6a8e0`, `32a6af10`; vslot `0x248` override `32a51410` | Ain | Indexes its effects and tests whether they came from a hyper effect set | `m_iIndex`, `m_bHyper` |
| vslot `0x248` override `32b0efc0` (instance `+0x1b8` = unit `+0x988c`, a counter advanced by `32b0fa00`) | Luciel | Stamps each effect set it plays with an index that its damage effects inherit as `m_iIndex` | none: the EffectSet `m_iIndex` write and vslot `0x248` were left out of C6b (C6b kickoff, "the rest of that EffectSet block") |

### 7b. SetDamageExceptionProcess hooks that only character code drives (from June effect completion C8a)

C8b ports the base, user and NPC layers of `SetDamageExceptionProcess` (slot `0x2b8`), all dormant on March data. The per-class overrides are not ported in the effect plan: D8 (2026-10-06) hands them here, so each character port ports its class's blocks, using `kickoffs/june-effect/C8c.md` as the brief.
Each row below is what turns one of them on. The verdicts are the funcmap rows tagged `Effect C8a`.

| June | Class | What it does | What the port must bring |
|---|---|---|---|
| `32c57660` (`+0x9688` = skill 24401 learned); `32c43120 @32c43ac4` (`+0x4108` = 2 with `ChangeDamageType`) | Raven | Burning Nasod Hand's damage-type change then applies only to `AT_COMMAND` hits (the user-layer gate `32045340 @320453a0`) | the 24401 setter and the `= 2` write, together with June's `RAVEN_FIGHTER.lua`, which marks 46 attacks `AT_COMMAND`. March data marks none, so the `= 2` write alone switches the passive off |
| `31fdbbb0` (skill-memo entry for `APPLIED_SKILL_ID`) behind user vslot `0x758` `3202ac40` and the NPC override `32759f30` | every user (and summons with a live master) | June's data-driven skill memo: per memo type, scales damage, force-down, MP gain and buff factors on the hit | June's memo table. C8b stubs the lookup (plan rule 5, returns null) |
| the per-class blocks in funcmap rows `329bc560`, `32b80f20`, `32bf28f0`, `32c5c080`, `32cad510`, `32cccc70`, `32d1e0e0`, and Ara's vslot `0x780` `32bca010` | Eve, Elesis, Chung, Raven, Rena, Aisha, Elsword, Ara | Passives keyed on June reform skill IDs (`RNSI_*`) | the blocks themselves (brief: `kickoffs/june-effect/C8c.md`) plus the reform skills; needs effect C5 (`ATTACK_TRIGGER_TYPE`) |
| D8 (completion plan): Rena's Powerful Bowstring 3405 → 23405 and Sharp Kick 3400 → 23400; Aisha's Fitness 2413 block replaced by Magical Staff 22409 | Rena, Aisha | June re-keys or replaces passives March characters have now | D8 = hand off: March's blocks stay until this port, which swaps in June's versions together with the reform skills |

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
| 167 sites in 0x32965e30-0x329cf260 (`32296d60` particle x119, `32296ee0` mesh x41, `32296da0` / `32296e40` handles x7) | Eve | Tags her particles and meshes with her alpha category right after creating them | one `CX2Game` tag-helper call per site, inside the class code being ported |
| 29 sites (`32d0d660` .. `32d1ecc0`) | Elsword | as above | as above |
| 22 sites (`32ccdb70` .. `32ce0010`) | Aisha | as above | as above |
| 12 sites (`329f4350`, `329f9ff0`, `32a03b70`, `32a27360`; four of them effect-set handles via `32296f30`) | Add | as above | as above |
| 8 sites (`32c89310`, `32c89460`, `32c895b0`; `32296da0` handles) | Rena | as above | as above |
| `322726f0`: the HP-rate gate in `GetFinalDamage` returns false for `RNSI_SA_ALP_SUPERSONIC` (29058) against an NPC defender (GameUnitType 1) when it has `+0x8aeb` / `+0x8ae9` set, vslots `0x3c8` / `0x3cc` / `0x430` true, or `+0x8e80` set, or when its `+0x8f08` is 1 or 2 | Rena | Supersonic's HP-rate damage skips some NPCs | the gate's false branch, with the reform skill. C10b-1 calls `GetHpRateDamage` unconditionally until then |
