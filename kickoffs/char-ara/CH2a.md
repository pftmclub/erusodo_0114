# June character Ara (Stage 7) - Phase CH2a - Skills: Martial Artist (7)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2a` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2a - Done"
```

## Task

Scope: the skill ids class 7 has (a skill's line decides its phase). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 48** (`awk -F'\t' '$7=="CH2a"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 48).
  June addresses: `32b911b0` `32b91200` `32b91250` `32b91260` `32b912d0` `32b91330` `32b916d0` `32b91750` `32b917e0` `32b917f0` `32b91940` `32b91a90` `32b91ae0` `32b91c30` `32b91d80` `32b92400` `32b92c90` `32b92d60` `32b9aa40` `32b9aa80` `32b9aac0` `32b9ab80` `32b9abc0` `32b9ac00` `32b9b820` `32b9c910` `32baa230` `32baa280` `32bac120` `32bac1b0` `32bac2d0` `32bac3a0` `32bae680` `32bae6f0` `32bae750` `32baf390` `32bb4a40` `32bb4db0` `32bb57a0` `32bb5a70` `32bb5b50` `32bb7e10` `32bc1ac0` `32bc1ef0` `32bc8c00` `32bc8c50` `32bc8d90` `32bce710`.
- **States: 13** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_ARA_WIND_WEDGE_START`, `ASI_SA_ARA_WIND_WEDGE_END`, `ASI_SA_ARA_PULLING_PIERCE_START`, `ASI_SA_ARA_PULLING_PIERCE_END`, `ASI_SA_ARA_FALLING_DRAGON`, `ASI_SA_ARA_TEMPEST_DANCE_START`, `ASI_SA_ARA_TEMPEST_DANCE_END`, `ASI_SA_ARA_SHADOW_KNOT_START`, `ASI_SA_ARA_SHADOW_KNOT_WAIT`, `ASI_SA_ARA_SHADOW_KNOT_CATCH`, `ASI_SA_ARA_SHADOW_KNOT_END`, `ASI_A_ARA_GAIN_FORCE`, `ASI_A_ARA_SHOULDER_BLOW`.
- **Skills: 14** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27000 RNSI_SA_ARA_TEMPEST_DANCE, 27001 RNSI_SA_ARA_PULLING_PIERCE, 27002 RNSI_SA_ARA_FALLING_DRAGON, 27003 RNSI_SA_ARA_WIND_WEDGE, 27004 RNSI_SA_ARA_SHADOW_KNOT, 27029 RNSI_SA_ARA_TEMPEST_DANCE_UPGRADE, 27030 RNSI_SA_ARA_PULLING_PIERCE_UPGRADE, 27031 RNSI_SA_ARA_FALLING_DRAGON_UPGRADE, 27032 RNSI_SA_ARA_WIND_WEDGE_UPGRADE, 27033 RNSI_SA_ARA_SHADOW_KNOT_UPGRADE, 27200 RNSI_A_ARA_GAIN_FORCE, 27201 RNSI_A_ARA_SHOULDER_BLOW, 27220 RNSI_A_ARA_GAIN_FORCE_UPGRADE, 27221 RNSI_A_ARA_SHOULDER_BLOW_UPGRADE.
- **Checklist rows: 3** (`awk -F'\t' '$5 ~ /(^| )CH2a( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch.
- **Force-power rows: 16** (`awk -F'\t' '$7 ~ /(^| )CH2a( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 10, unchecked 6).
- **Identified callees first needed here: 1** (`awk -F'\t' '$8 ~ /(^| )CH2a( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `32a31a70`.
- 48 functions (whole). The `32babba0` FP-cancel helper is CH1q's (called here).

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).

## Amended by CH1q (2026-11-11)

- `StateChangeOrFPCancel( iNextStateID_ )` is June's `32babba0` (written under `_FP`): call it where June's EventProcess calls `32babba0` after the animation-end test; a negative state id means "no state change" (read each caller's push). It does the FP-cancel gain (`MarkNowSkillFPGainReady`, `UpForcePowerByReadySkillGain`) and `StateChangeSpecialAttack` itself: do not inline March's `GetAvailableFPCancel` block beside it.
