# June character Chung (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4a` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |
| Review | `kickoffs/char-chung/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Chung lines of the checklist rows named below (`june2017_re/char_chung/plan_checklist_rows.md` gives the line numbers), the `CH4a`
rows of `ch0_*.tsv`, and Eve's matching CH4 Done note as the worked example.

## Task

Scope: `InitPassiveSkillState` `32c16be0` and Chung's passive ids (types 1 / 8: 3 + 27 rows), their objects and readers; checklist 7am
delegate-list registrations (`32c16be0`, A `32c12910`, A `32bdba10`); the engine hooks they need (Aisha's and Eve's CH4 already ported the unit
delegate lists and the user passive hooks: reuse them). June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`.
Mark rows `(Chung: CH4a)`. Done gate: each item's effect visible in game or in a named log line.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-10-31)

- **Functions: 4** (`awk -F'\t' '$7=="CH4a"' june2017_re/char_chung/ch0_class_map.tsv`; inside 3, new 1).
  June addresses: `32bda7d0` `32bec1a0` `32c16be0` `32c24dd0`(new ?).
- **Skills: 22** (part 2 of `ch0_state_skill.tsv`, col 6): 106 RNSI_P_COMMON_HYPER_AURA, 26400 RNSI_P_CFG_METABOLISM_BOOST, 26401 RNSI_P_CFG_ELEMENTAL_DEFENCE_A, 26402 RNSI_P_CIP_IRON_WILL, 26403 RNSI_P_CFG_GUARD_MASTERY, 26404 RNSI_P_CIP_ADVANCED_METABOLISM_BOOST, 26405 RNSI_P_CSG_REMODEL_RIFLE, 26406 RNSI_P_CSG_REMODEL_MAGAZINE, 26407 RNSI_P_CDC_INTENSE_SHOWTIME, 26408 RNSI_P_CDC_TUSSLE_TECHNIQUE, 26409 RNSI_P_CDC_GAS_PISTON_SYSTEM, 26410 RNSI_P_CHG_CONVERTED_WARHEAD, 26411 RNSI_P_CHG_RELOAD_MASTERY, 26412 RNSI_P_CTT_MOBILE_ARMORED_MODE, 26413 RNSI_P_CTT_CANNONEER_WITHSTANDING, 26414 RNSI_P_CTT_REACTIVE_ARMOR, 26415 RNSI_P_CIP_OPEN_MIND, 26416 RNSI_P_CIP_GUARDIAN_FORCE, 26417 RNSI_P_CDC_OPEN_MIND, 26418 RNSI_P_CDC_ENDLESS_CHASE, 26419 RNSI_P_CTT_OPEN_MIND, 26420 RNSI_P_CTT_USEFUL_TACTIC.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7 L246 32c16be0 InitPassiveSkillState: SetPassiveMultiple( GetPassiveMultiple; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7am L897 delegate-list registrations in InitPassiveSkillState: Chung 32c16be0 p.
- **Callees this Opus phase maps itself:** `31fc94d0` CX2GUUser add-to-gauge helper: +0x93b8 += (rate > 1 ? rate-1 : rate).
- 22 passive / buff ids with no state (part 2 of `ch0_state_skill.tsv`); `31fc94d0` (called by InitPassiveSkillState `32c16be0`) is yours to map.

## Amended by CH1g (2026-10-31)

- `InitPassiveSkillState` `32c16be0` writes the protected float at `+0x98ac` (1.0, then for `RNSI_P_CSG_REMODEL_RIFLE` 26405 the `SA_DAMAGE_REL` value): CH1g added the member `m_fRemodelRifleDamageRate` (default 1) that the dash XX / Z X / Z X X shell FrameMoves multiply into their power. Port that write here, next to the `+0x96b8` / `+0x96bc` ones.

## Amended by CH1o (2026-11-02)

- `+0x9714` is `m_fBerserkChanceOnAwakening` (CH1o: member, ctor 0, read by the `IsBerserkByChance` override `32c24b90`). `InitPassiveSkillState` writes it:
  0 `@32c1757e`, then `RNSI_P_CDC_TUSSLE_TECHNIQUE` (26408, register it under `JUNE_ENUM_NAMES`; not in March's enum) level `> 0` -> `SA_EFFECTIVE_RATE` (5) `@32c175d3`
  ("Berserk Chance when Awakening" 30-60 %). `+0x9710` beside it is the same skill's `SA` `0x92` value (`@32c1756e`).
- Two delegates registered from `InitPassiveSkillState` are yours (7am): `32bdba10` via thunk `32c23090` (pointer `@32c1781d`: damage type `0x21` / `0x22`, `+8` == 2,
  skill `0x6731` learned -> vslot `0x23c( 0x6731, 0, 1 )`) and `32c12910` via thunk `32c258a0` (pointer `@32c177f1`).

## Amended by CH1p (2026-11-02)

- `COMMAND_HIT_ADD_MP_BONUS` (`m_fCommandHitAddMpBonus`, June `+0x98c4`, read by CH1k's InitComponent) has exactly one reader, `InitPassiveSkillState`
  `32c16be0` `@32c16c2c`: it multiplies the passive multiple of `BBT_CHANGE_RANGE_COMMAND_MP_GAIN_RATE` (85, `+0x68d4`, protected `+0x68ec`) and of
  `BBT_CHANGE_MELEE_COMMAND_MP_GAIN_RATE` (86, `+0x6988`, protected `+0x69a0`), then calls `SetPassiveMultiple` (`3236ccc0`). This is part of the whole
  body you write. The CH0 key row routed it to CH1p, which found no other reader in June's `.text` (dispsweep 98c4: in the class range only the ctor,
  InitComponent, the dtor and this function).

## Amended by CH1q (2026-11-02)

- `SetReduceGuardDamageAtHeavyStanceFromGuardSkill` is dead in June (`32bef230`: no call, jump or pointer; its string `335095a0` is pushed only there).
  CH1q dropped March's call in `LoadPersonalInfo`; June's `InitPassiveSkillState` has none either, so March's call at the end of the
  `CHUNG_FIRST_CLASS_CHANGE` block (`X2GUChung.cpp` ~5155 before CH1q) goes too.
- `32c16be0 @32c16f23-32c16f8f` writes Chung `+0x9864` / `+0x9860` from skill 26410 (`0x672a`): March's `CX2GUUser::m_fForceDownRelDamageRateBase` /
  `m_fCannonBallDamageUp`. The DamageEffect reader (CH1q, `_CBALL`) reads them through the March getters.

## Amended by CH2a (2026-11-02)

- Reload Cannon (26202) charges `m_ucCannonBallCountToBeUpUsingReLoadCannon` balls (default 1) and multiplies its damage by `m_fDamageRelByCannonBallMastery` (default 1). March's `InitPassiveSkillState` writes
  both from `SI_A_CC_RELOAD_CANNON` (6xxx), which June's skill tree never holds; June's `32c16be0` reads skill 26202 (`SA_CHARGE_CANNON_BALL`, `SA_DAMAGE_REL`). CH2a's play-test therefore shows 1 ball per
  Reload Cannon; the real count is yours.
- `m_fSiegeShellingDamageRel`, `m_fSiegeShellingDistance`, `m_fSiegeShellingUserBallRate`, `m_fSiegeShellingIncraseMPRel` come from the same function (March reads `SI_A_CC_SIEGE_SHELLING` 6xxx). CH2a's Siege Shelling
  reads `m_fSiegeShellingUserBallRate` (default from the ctor) and `m_fSiegeShellingDistance` (through `UpdateSpineAimRotation`); until you port the writers they stay at their ctor values.

## Amended by CH3a review (2026-11-03)

- Earthquake's landing (26007 / 26045) now applies `BFI_BUFF_LAND_DIMOLISHER` (`BTI_BUFF_LAND_DIMOLISHER` 54) as June. March's `SetSpecificValueByBuffTempletID` case for it reads
  `SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS` (6xxx), so `m_bApplyLandDemolisher`, `m_fLandDemolisherPowerRate` and the weapon pulse stay off until you port June's `32bec1a0` /
  `32bda7d0` cases (funcmap rows 6307 / 6294).
