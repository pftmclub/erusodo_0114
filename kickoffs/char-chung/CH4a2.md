# June character Chung (Stage 7) - Phase CH4a2 - June's base CX2GUUser::InitPassiveSkillState

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4a2` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4a review (same functions' callers); no other phase editing `X2GUUser.cpp`'s `InitPassiveSkillState` |
| Flag | `SERV_IRUHADEV_JUNE_DIVERGE_INIT_PASSIVE_SKILL_STATE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE base`) |
| Review | `kickoffs/char-chung/CH4a2-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH4 - Passives and class mechanics" "CH4a - Done (2026-11-03)"
grep -n "CX2GUUser::InitPassiveSkillState" tools/data/june_march_divergent.tsv tools/data/june_diverge_allow.tsv
```

## Task

Found by CH4a (scope rule: wrong combat numbers on a reachable path, Chung's Elemental Defence A 26401 among them). June's base
`CX2GUUser::InitPassiveSkillState` `31fe19d0` differs from March's whole body: March reads March ids (`SI_P_CFG_ELEMENTAL_DEFENCE_A / _B`,
`SI_P_SPK_BODY_OF_FIRE`) that June's trees never hold, so every class's base passives are dead since S3. Port June's body whole inside March's
function under the flag (March's in `#else`), with its callees:
- `322718e0` (compare with March's `InitAdditionalPassive`), the five list clears (already June's, `JUNE_UNIT_DELEGATE_LIST`);
- `SkillRelatedData` `+0x4054` / `+0x4058` = 1, `+0x4060` = 0, `+0x8df0` = 0; skill 104 ( `0x68`, `SA_EFFECTIVE_RATE` ftol into `+0x4060` ) behind the
  game-type test `31edf060` / `324550a0` / `324511a0`; the guild-skill flag `+0x8ad0` from `328907e0( 0x1fd, unit +0x988 )`; 27401 ( `0x6b09`, SA 0x27,
  `rate - 1 + 5e-8` added to `+0x4058`, reset to 1 outside (0, 1] );
- the six enchant defences `+0x9258..+0x926c` from 26401 `RNSI_P_CFG_ELEMENTAL_DEFENCE_A` (SA 0x58 on all six), 28405 (`0x6ef5`, SA 0x58 on the first),
  30409 (`0x76c9`, SA 0x54 on all six); then `31fe0790` (find what it does: March's `AddEnchantStat...` / `AddExtraDamageImmuneRate...` pair?) and
  `3234b6b0` (the other June reader of 26401, `@3234b6e5` / `@3234b703`);
- `+0x92a0` = 1.0 (the anim-speed rate `ResetAnimSpeed` reads, row `ResetAnimSpeed` in the divergence table) and `+0x93b8` = 0 (CH4a placed this one
  reset under `SERV_IRUHADEV_JUNE_USER_MP_GAIN_ON_HIT`; fold it into the June body).

Register every June id by its `Enum.lua` name. When the body lands, set the divergence row's `source` to the commit and drop the
`InitPassiveSkillState` row from `june_diverge_allow.tsv`. Play-test: unit 43 (Iron Paladin, 26401 level 4) shows its element resistances in the
character window and one `CHUNG PASSIVE base` line names the six values.

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
