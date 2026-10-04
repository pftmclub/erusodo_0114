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
- **Lead, not confirmed:** the table is probably `NewSkillTempletVer2.lua`. It is the only June script
  with `NUMERICAL_DATA1` … `NUMERICAL_DATA12` keys.
- The override's callees (`32890620`, `31fca8e0`, `320717e0` by index, `32078060` by name) are not
  identified. That is Opus-level identification work (plan, "Model and effort per phase").
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
| Combination point | absent | Lu/Ciel (`Luciel_Duo` ×6) | new-character system |
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
3. The numerical skill data system (section 1). Without it, June character damage is 0.
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
