# June character Ara (Stage 7) - Phase CH2c - Skills: Little Devil (31), SA tier

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2c` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2b2, CH2c0 review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2c - Done"
```

## Task

Scope: the skill ids class 31 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is the SA tier, skill ids < 27200 (CH2c2 has the A tier).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
whole (Ghidra, the map's addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (March's in `#else`) or a new case; its attached rows
(7u names, 7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows
l.522-526); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its force-power gain / cost lines on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `SA_ALD_WOLF_Break` runs on CH2c0's add-N-stacks path.
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

- **Functions: 36** (`awk -F'\t' '$7=="CH2c"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 32, new 4).
  June addresses: `32b93460` `32b93540`(new ASI_SA_ALD_ENERGY_WAVE_StateStartFuture) `32b93570`(new ASI_SA_ALD_ENERGY_WAVE_StateStart) `32b93640` `32b93660` `32b93740` `32b93800` `32b93820` `32b93840` `32b939f0` `32b93a10` `32b93a70` `32b93d00` `32b93dc0` `32b93e20` `32b9dd20` `32b9dd60`(new ASI_SA_ALD_ENERGY_WAVE_FIRE_EventProcess) `32b9df70` `32b9e190` `32b9e4c0` `32b9eac0` `32b9ece0` `32baa890` `32baa940` `32baa990` `32baaa30` `32baaae0` `32bacb50` `32bacdb0` `32bad100` `32bae930` `32bae980` `32baea20` `32bc2900`(new ASI_SA_ALD_ENERGY_WAVE_FIRE_FrameMove) `32bc9350` `32bce770`.
- **States: 10** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_ALD_ENERGY_WAVE`, `ASI_SA_ALD_ENERGY_WAVE_FIRE`, `ASI_SA_ALD_HELL_OF_TORNADO`, `ASI_SA_ALD_HELL_OF_TORNADO_FINISH`, `ASI_SA_ALD_WOLF_SPEAR`, `ASI_SA_ALD_WOLF_SPEAR_FINISH`, `ASI_SA_ALD_PRISON_SPEAR`, `ASI_SA_ALD_PRISON_SPEAR_FINISH`, `ASI_SA_AYR_SUPPRESSION_ENERGY`, `ASI_SA_ALD_WOLF_BREAK`.
- **Skills: 10** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27013 RNSI_SA_ALD_HELL_OF_TORNADO, 27014 RNSI_SA_ALD_WOLF_SPEAR, 27015 RNSI_SA_ALD_ENERGY_WAVE, 27016 RNSI_SA_ALD_PRISON_SPEAR, 27017 RNSI_SA_ALD_SUPPRESSION_ENERGY, 27035 RNSI_SA_ALD_HELL_OF_TORNADO_UPGRADE, 27039 RNSI_SA_ALD_WOLF_SPEAR_UPGRADE, 27040 RNSI_SA_ALD_ENERGY_WAVE_UPGRADE, 27054 RNSI_SA_ALD_PRISON_SPEAR_UPGRADE, 27055 RNSI_SA_ALD_SUPPRESSION_ENERGY_UPGRADE.
- **Checklist rows: 4** (`awk -F'\t' '$5 ~ /(^| )CH2c( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L423 "EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE; Ara a; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch.
- **Force-power rows: 8** (`awk -F'\t' '$7 ~ /(^| )CH2c( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 8).
- Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.
- 7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.
