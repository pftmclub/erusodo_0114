# June character Ara (Stage 7) - Phase CH2d - Skills: Little Specter (34), ids < 27024

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2d` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2c2, CHNb review (the 34 unit). One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d3 review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2d - Done"
```

## Task

Scope: the skill ids class 34 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is skill ids < 27024 and the states with no skill; no review of its own: the CH2d3 review covers it (CH2d2 has ids 27024-27199, CH2d3 the A tier).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
whole (Ghidra, the map's addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (March's in `#else`) or a new case; its attached rows
(7u names, 7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows
l.522-526); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its force-power gain / cost lines on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- Test on CHNa's 34 unit; Specter states run on CH1q2's code.
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

- **Functions: 49** (`awk -F'\t' '$7=="CH2d"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 10, new 39).
  June addresses: `32b946c0`(new ASI_SA_AA_SPECTER_OF_THUNDER_Init) `32b94750`(new ASI_SA_AA_SPECTER_OF_THUNDER_StateStart) `32b94780`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING_Init) `32b94810`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING_StateEnd) `32b948c0`(new ASI_SA_ALS_RESTRAINTS_SPECTER_Init) `32b94990`(new ASI_SA_ALS_RESTRAINTS_SPECTER_StateStart) `32b949b0`(new ASI_SA_ALS_RESTRAINTS_SPECTER_FINISH_Init) `32b94aa0`(new ASI_SA_ALS_SPECTER_OF_STORM_Init) `32b94b60`(new ASI_SA_ALS_SPECTER_OF_STORM_StateStart) `32b94b80`(new ASI_SA_ALS_SPECTER_OF_STORM_StateEnd) `32b94bc0`(new ASI_SA_ALS_SPECTER_OF_STORM_FINISH_Init) `32b94ca0`(new ASI_SA_ALS_SPECTER_OF_STORM_LANDING_Init) `32ba19b0`(new ASI_SA_AA_SPECTER_OF_THUNDER_EventProcess) `32ba1d00`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_FrameMove) `32ba1d70`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_EventProcess) `32ba1db0`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING_FrameMove) `32ba1e40`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING_EventProcess) `32ba2460`(new ASI_SA_ALS_RESTRAINTS_SPECTER_FINISH_EventProcess) `32ba2760`(new ASI_SA_ALS_SPECTER_OF_STORM_FINISH_EventProcess) `32ba27a0`(new ASI_SA_ALS_SPECTER_OF_STORM_LANDING_EventProcess) `32ba4760`(new GetMpScaledPowerRate) `32ba5ab0`(new ASI_SA_ALS_SPECTER_OF_STORM_StateStart) `32ba6220`(new ASI_SA_ALS_SPECTER_OF_STORM_LANDING_EventProcess) `32ba7c20`(new ASI_SA_ALS_SPECTER_OF_STORM_StateStartFuture) `32baac40` `32bad5e0`(new ASI_SA_ALS_RESTRAINTS_SPECTER_EventProcess) `32bad670`(new ASI_SA_ALS_SPECTER_OF_STORM_EventProcess) `32bb0790`(new ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING_StateStartFuture) `32bb09f0`(new ScaleMeshInstanceRange) `32bbd5d0` `32bbd840`(new ASI_SA_ALS_RESTRAINTS_SPECTER_FINISH_FrameMove) `32bbdae0`(new ASI_SA_ALS_SPECTER_OF_STORM_FINISH_FrameMove) `32bbdeb0`(new ASI_SA_ALS_SPECTER_OF_STORM_LANDING_StateStart) `32bc0420`(new CalcLockOnLandingPos) `32bc12e0`(new SetStoneProtractorAimArrowFade) `32bc4410`(new ASI_SA_ALS_RESTRAINTS_SPECTER_FrameMove) `32bc4670`(new ASI_SA_ALS_SPECTER_OF_STORM_FrameMove) `32bc5f80`(new FindSpecterOfThunderLockOnTarget) `32bc6820`(new CreateStoneProtractorAimArrow) `32bc94f0`(new ASI_SA_AA_SPECTER_OF_THUNDER_StateStartFuture) `32bc9540` `32bc9590` `32bc95e0` `32bc9c70`(new ASI_SA_ALS_SPECTER_OF_STORM_EventProcess) `32bc9fc0` `32bcea90` `32bceaf0` `32bd0900` `32bd0960`.
- **States: 8** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_AA_SPECTER_OF_THUNDER`, `ASI_SA_AA_SPECTER_OF_THUNDER_FIND`, `ASI_SA_AA_SPECTER_OF_THUNDER_FIND_LANDING`, `ASI_SA_ALS_RESTRAINTS_SPECTER`, `ASI_SA_ALS_RESTRAINTS_SPECTER_FINISH`, `ASI_SA_ALS_SPECTER_OF_STORM`, `ASI_SA_ALS_SPECTER_OF_STORM_FINISH`, `ASI_SA_ALS_SPECTER_OF_STORM_LANDING`.
- **Skills: 6** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27021 RNSI_SA_ALS_SPECTER_OF_STORM, 27022 RNSI_SA_ALS_RESTRAINTS_SPECTER, 27023 RNSI_SA_ALS_SPECTER_OF_THUNDER, 27036 RNSI_SA_ALS_SPECTER_OF_STORM_UPGRADE (states also in CH2d2), 27041 RNSI_SA_ALS_RESTRAINTS_SPECTER_UPGRADE, 27042 RNSI_SA_ALS_SPECTER_OF_THUNDER_UPGRADE.
- **Checklist rows: 7** (`awk -F'\t' '$5 ~ /(^| )CH2d( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L312 "CEffect +0xb18 writer 32bb0bd6 (in a function called from 3; L664 CUSTOM_RENDER_STATE class users: Ara_Martial_Artist among th; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch; L852-L855 "pass June's skip / adjust / level / creator at every ported; L868 "BUFF_FACTOR_ID_AND_APPLY_TIME ""appears only in Elesis's sc.
- **Identified callees first needed here: 4** (`awk -F'\t' '$8 ~ /(^| )CH2d( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `31cf1920` `31fac1f0` `32ec59d0` `33086a38`.
- `ASI_A_ALS_SHAKE_CHAOS` is registered at 291 and at 333 (no Enum name for 333: a number with a comment). `ASI_SA_ALS_SPECTER_OF_BLADE` / `_FINISH` are dead data. `CalcLockOnLandingPos` `32bc0420` and `FindSpecterOfThunderLockOnTarget` `32bc5f80` are here; `ASI_SA_ALS_SPECTER_OF_STORM_LANDING` EventProcess `32ba6220`.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).
