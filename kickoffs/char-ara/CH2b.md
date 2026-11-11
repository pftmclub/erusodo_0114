# June character Ara (Stage 7) - Phase CH2b - Skills: Little Hsien (28), SA tier

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2b` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2a, CH2b0 review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2b - Done"
```

## Task

Scope: the skill ids class 28 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is the SA tier, skill ids < 27200 (CH2b2 has the A tier).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
whole (Ghidra, the map's addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (March's in `#else`) or a new case; its attached rows
(7u names, 7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows
l.522-526); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its force-power gain / cost lines on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Ara: <phase>)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence; learn only the bank under test), each
  cast twice (cool time, MP), what June's motion and effect look like.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 45** (`awk -F'\t' '$7=="CH2b"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 43, new 2).
  June addresses: `32b91d90` `32b91e20` `32b91eb0` `32b91f50` `32b92590` `32b92720` `32b92810` `32b92820` `32b928d0` `32b929f0` `32b92a50`(new ASI_SA_ALH_DESCENT_DRAGON_StateStart) `32b9ac40` `32b9ad90` `32b9ae20` `32b9ae80` `32b9aee0` `32b9af30` `32b9af60` `32b9b160` `32b9b1c0` `32b9bee0` `32b9c140` `32b9c190` `32b9c1d0` `32b9c3e0` `32ba5870`(new ASI_SA_ALH_LINK_FLOWER_EventProcess) `32baa2d0` `32baa410` `32baa460` `32baa520` `32baae10` `32bac4c0` `32bae840` `32bb5bb0` `32bb5f40` `32bb6410` `32bb68e0` `32bb96d0` `32bb99d0` `32bb9ec0` `32bc2190` `32bc8df0` `32bc8e60` `32bc8ed0` `32bc9110`.
- **States: 11** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_ALH_DOUBLE_COLLISION_START`, `ASI_SA_ALH_DOUBLE_COLLISION_END`, `ASI_SA_ALH_MOONLIGHT_SLASH_READY`, `ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE`, `ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK`, `ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK`, `ASI_SA_ALH_POWER_OF_WHITER_TIGER`, `ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING`, `ASI_SA_ALH_DESCENT_DRAGON`, `ASI_SA_ASD_SUPPRESSION`, `ASI_SA_ALH_LINK_FLOWER`.
- **Skills: 10** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27005 RNSI_SA_ALH_WHITE_TIGER, 27006 RNSI_SA_ALH_DOUBLE_COLLISION, 27007 RNSI_SA_ALH_LINK_FLOWER, 27008 RNSI_SA_ALH_MOONLIGHT_SLASH, 27009 RNSI_SA_ALH_SUPPRESSION, 27034 RNSI_SA_ALH_WHITE_TIGER_UPGRADE, 27037 RNSI_SA_ALH_DOUBLE_COLLISION_UPGRADE, 27038 RNSI_SA_ALH_LINK_FLOWER_UPGRADE, 27049 RNSI_SA_ALH_MOONLIGHT_SLASH_UPGRADE, 27050 RNSI_SA_ALH_SUPPRESSION_UPGRADE.
- **Checklist rows: 9** (`awk -F'\t' '$5 ~ /(^| )CH2b( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_SA_ALH_LINK_FL; L55 numerical-only DAMAGE_DATA_NEXT table in state ASI_SA_ALH_LI; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L423 "EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE; Ara a; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch.
- **Force-power rows: 10** (`awk -F'\t' '$7 ~ /(^| )CH2b( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 3, unchecked 7).
- **Identified callees first needed here: 1** (`awk -F'\t' '$8 ~ /(^| )CH2b( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `32fe2c36`.
- **Swallow Assault is not here:** June has it as a Sakra Devanam skill (`RNSI_SA_ASD_SWALLOW_ASSAULT` 27011; March's `SI_SA_ALH_SWALLOW_ASSAULT` 7007): CH3a. June moved several skills across lines (`ch0_passive_ids.tsv` "tag-mapped" rows): the line in June's tree decides the phase, not March's name. `CalcLockOnLandingPos` `32bc0420` is not the lock-on's: CH2d. Split: CH2b = SA tier (ids < 27200), CH2b2 = A tier.

## Amended by the CH1k review (2026-11-11)

- **Moonlight Slash data grew in June** (0x28 bytes, ctor `32bd6e80`): +0x20 and +0x24 are new floats, both 1.0. The CH1k review added +0x20 as `sMoonLightSlashData::m_fEffectiveRate` (`_CORE`, `GetEffectiveRate` / `SetEffectiveRate`); `CanUseSpecialAttack` stores the skill's `SA_EFFECTIVE_RATE` there (June @32bd08c5). The Moonlight Slash states read it; +0x24 is not declared yet: name and declare it from the state that writes / reads it.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).

## Amended by CH2a (2026-11-11)

- **The shape CH2a used** (copy it): add the skill's RNSI base and upgrade ids as extra `case` labels on March's registration case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (the registration is usually March's already: compare June's `32bd09d0` case with it), June's `InitAquiredActiveSkillState` case beside March's, then each state function wrapped `#ifdef _SKILL` June `#else` March `#endif`. A June function that is only `CommonFrameMove` / the anim-end WAIT-or-JUMP_DOWN test is March's body or a one-liner.
- **Recurring June differences:** the effect sets, attack boxes and `UseForcePower` / `UpForcePower` of March's FrameMove are the state script's now (`EFFECT_SET_LIST_EX`, `USE_FORCE_POWER`); the EventProcess ends with `StateChangeOrFPCancel( next )` plus the FP-cancel window inside the `m_bFPCancelProbe` bracket; walk and dash cancels are `WALK_` / `DASH_CANCEL_AFTER_ON_LINE_`; every `GetEqippedSkillMemo( ... )` is June's always-false stub `31fca9e0` (write nothing for its branch).
- **Registration helpers** (`32bce710` ...) March lacks get a `_CreateState` function in the class's own pattern (CH2a wrote the four Shadow Knot ones next to `StateChangeOrFPCancel`).
