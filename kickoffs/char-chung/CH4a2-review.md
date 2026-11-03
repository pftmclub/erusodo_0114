# June character Chung (Stage 7) - Phase CH4a2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4a2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4a2 done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_DIVERGE_INIT_PASSIVE_SKILL_STATE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE base`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH4a2 - Done (2026-11-03)"
grep -n "InitPassiveSkillState\|AddExtraDamageImmuneRate\|UpdateEnchantStatFromPassiveSkill\|^GetEnchantResist" tools/data/june_march_divergent.tsv
```

## Task

Check CH4a2 against June (read-only; read the listings, not only the decompiles). In order of risk:
- **The base `InitPassiveSkillState` `31fe19d0`** against `X2GUUser.cpp`'s June branch: statement order, the GetSkillLevel arguments (`1,0,1` for 104 /
  27401 / 26401, `0,0,1` for 28405 / 30409), each SA type (5, 0x27, 0x58, 0x54), the Get Chance `x 100.0` ftol (`3342f360` is a double), the 27401 clamp
  (`rate - 1 + 5e-8` then 1.0 outside (0, 1]), the guild flag reset before `328907e0`, and the 104 gate (`31edf060` / `324550a0` / `324511a0`, JUNE_BUGS 516).
- **The offsets -> members**: `+0x4058` / `+0x4054` / `+0x4060` (SkillRelatedData melee / range / no-down), `+0x8df0` = `m_fAddAccuaracyPercent` (only
  reader `326407b0` in the hit-rate function `32610a00`), `+0x8ad0` guild flag, `+0x9258..+0x926c` = `m_EnchantStatFromPassiveSkill` (getter `31fc79e0`),
  `+0x92a0` the new `m_fPassiveAnimSpeedRate` (ctor `@32041c49`), `+0x93b8`.
- **`31fe0790`** = `AddExtraDamageImmuneRateFromPassiveSkillToImmuneData`: the nine (SA, EDT) pairs, `31fe0730` = March's `AddExtraDamageImmuneData`.
- **`31fd8790`** = the new `CX2GUUser::GetEnchantResist` override: passive float + buff resist per type, then the map; base `32274b00`. Confirm no March
  caller relied on `AddEnchantStatFromPassiveSkillToDefenceEnchantData` (only `IsImmuneToEnchant` reads the map directly, and only above the cap).
- **`3234b6b0`** = `CX2Unit::UpdateEnchantStatFromPassiveSkill` (unit's own tree `+0x494`); caller `32358df0` adds it like March.
- **`322718e0`** = March's `InitAdditionalPassive` (the CH4a2 subagent map: same 54 members; 14 of them named from `X2GameUnit.h` comments only).
- **Play-test results** (units 43 / 21): `CHUNG PASSIVE base` values, the character window's resistances, `ErrorLog.txt`.

Retire `SERV_IRUHADEV_JUNE_DIVERGE_INIT_PASSIVE_SKILL_STATE_DEBUG` (comment out its `#define`) if the logs were read.
Hygiene: `python tools/flagcheck.py JUNE_DIVERGE_INIT_PASSIVE_SKILL_STATE --with JUNE_DIVERGE_INIT_PASSIVE_SKILL_STATE_DEBUG --range <CH4a2 commit>^..<CH4a2 commit> --files KTDXLIB/Always.h X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h X2Lib/X2SkillTree.h X2Lib/X2Unit.cpp`,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2Unit.cpp` (no row inside the CH4a2 blocks). Write "CH4a2 - Review" into the plan
doc, update the STATUS row and commit.
