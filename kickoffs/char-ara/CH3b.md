# June character Ara (Stage 7) - Phase CH3b - Skills: Yama Raja (119)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH3b` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3a2. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH3d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH3b - Done"
```

## Task

Scope: the skill ids class 119 has and 7 / 31 lack (a skill's line decides its phase). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 44** (`awk -F'\t' '$7=="CH3b"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 38, new 6).
  June addresses: `32b93320` `32b933b0` `32b933e0` `32b93430` `32b93930` `32b93990` `32b939b0` `32b93aa0` `32b93b00` `32b93b80` `32b93bc0` `32b93bf0` `32b93c10` `32b93c90` `32b93e40` `32b95370`(new ASI_SA_AYR_RAKSASA_FIRE_Init) `32b9db10` `32b9e6e0` `32b9e7f0` `32b9e820` `32b9e8a0` `32b9ed30` `32ba44a0`(new ASI_SA_AYR_RESTRICTION_ENERGY_START_StateStart) `32baa7f0` `32baa830` `32baa9e0` `32baaa80` `32bacfa0` `32bad260` `32bad9a0`(new ASI_SA_AYR_RESTRICTION_ENERGY_START_EventProcess) `32bada30`(new ASI_SA_AYR_REVERSE_GRAVITY_FIELD_START_EventProcess) `32bae9d0` `32baf600` `32baf670` `32bb06f0` `32bb0740` `32bbadc0` `32bbae10` `32bc2c50` `32bc56d0`(new ASI_SA_AYR_RESTRICTION_ENERGY_START_FrameMove) `32bc92f0` `32bc93b0` `32bc9810`(new ASI_SA_AYR_RESTRICTION_ENERGY_END_FrameMove) `32bce7c0`.
- **States: 16** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_A_AYR_HIGH_SPEED_READY`, `ASI_A_AYR_HIGH_SPEED`, `ASI_SA_ALD_ENERGY_BEAD`, `ASI_SA_ALD_ENERGY_BEAD_FINISH`, `ASI_A_AYR_STEAL_SOUL`, `ASI_A_AYR_STEAL_SOUL_FINISH`, `ASI_SA_AYR_ENERGY_VOID_START`, `ASI_SA_AYR_ENERGY_VOID_LOOP`, `ASI_SA_AYR_ENERGY_VOID_END`, `ASI_SA_AYR_OPEN_THE_HELL`, `ASI_A_AYR_GAIN_GOOD_ENERGY_BALL`, `ASI_SA_AYR_RESTRICTION_ENERGY_START`, `ASI_SA_AYR_RESTRICTION_ENERGY_END`, `ASI_SA_AYR_REVERSE_GRAVITY_FIELD_START`, `ASI_SA_AYR_REVERSE_GRAVITY_FIELD_END`, `ASI_SA_AYR_RAKSASA_FIRE`.
- **Skills: 14** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27018 RNSI_SA_AYR_ENERGY_BEAD, 27019 RNSI_SA_AYR_ENERGY_VOID, 27020 RNSI_SA_AYR_STEAL_SOUL, 27045 RNSI_SA_AYR_RESTRICTION_ENERGY, 27046 RNSI_SA_AYR_REVERSE_GRAVITY_FIELD, 27056 RNSI_SA_AYR_ENERGY_BEAD_UPGRADE, 27057 RNSI_SA_AYR_ENERGY_VOID_UPGRADE, 27058 RNSI_SA_AYR_STEAL_SOUL_UPGRADE, 27065 RNSI_SA_AYR_RAKSASA_FIRE, 27212 RNSI_A_AYR_HIGH_SPEED, 27213 RNSI_A_AYR_REFLECTION, 27227 RNSI_A_AYR_GAIN_GOOD_ENERGY_BALL, 27237 RNSI_A_AYR_HIGH_SPEED_UPGRADE, 27238 RNSI_A_AYR_REFLECTION_UPGRADE.
- **Checklist rows: 8** (`awk -F'\t' '$5 ~ /(^| )CH3b( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_A_AYR_STEAL_SO; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch; L868 "BUFF_FACTOR_ID_AND_APPLY_TIME ""appears only in Elesis's sc.
- **Force-power rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH3b( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 1, unchecked 1).
