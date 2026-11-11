# June character Ara (Stage 7) - Phase CH2d2 - Skills: Little Specter (34) ids 27024-27199 (CH0 split of CH2d)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2d2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2d. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
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

Scope: the skill ids class 34 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is skill ids 27024-27199 (CH2d has ids < 27024 and the states with no skill, CH2d3 the A tier).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 31** (`awk -F'\t' '$7=="CH2d2"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 7, new 24).
  June addresses: `32b94830`(new ASI_SA_ALS_SEVER_Init) `32b95030`(new ASI_SA_AA_SUPPRESSION_SPECTER_Init) `32b95120`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_Init) `32b95150`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_Init) `32b951e0`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_StateEndFuture) `32b951f0`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_StateEnd) `32ba2140`(new ASI_SA_ALS_SEVER_EventProcess) `32ba3170`(new ASI_SA_AA_SUPPRESSION_SPECTER_EventProcess) `32ba3490`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_StateStartFuture) `32ba34d0`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_EventProcess) `32ba3500`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_EventProcess) `32ba5b50`(new ASI_SA_ALS_SPECTER_OF_STORM_DEGREE_EventProcess) `32ba5e90`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_DEGREE_EventProcess) `32ba61d0`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH_FrameMoveFuture) `32ba61f0`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH_EventProcess) `32ba7c90`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH_StateStartFuture) `32baac90` `32bab500`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_DEGREE_FrameMove) `32bbd640`(new ASI_SA_ALS_SEVER_FrameMove) `32bbe550`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FrameMove) `32bbec00`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_FrameMove) `32bc0e30`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH_FrameMove) `32bc3f50`(new ASI_SA_ALS_SEVER_StateEnd) `32bc5280`(new ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH_StateStartFuture) `32bc67f0`(new ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_DEGREE_FrameMoveFuture) `32bc9750` `32bc9f50` `32bcea30` `32bcec30` `32bcf400` `32bcf450`.
- **States: 7** (`awk -F'\t' '$8=="CH2d2"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_ALS_SEVER`, `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_DEGREE`, `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH`, `ASI_SA_ALS_VERITABLE_PANDEMONIUM`, `ASI_SA_ALS_VERITABLE_PANDEMONIUM_FINISH`, `ASI_SA_AA_SUPPRESSION_SPECTER`, `ASI_SA_ALS_SPECTER_OF_STORM_DEGREE`.
- **Skills: 5** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27024 RNSI_SA_ALS_SEVER, 27025 RNSI_SA_ALS_SUPPRESSION_SPECTER, 27036 RNSI_SA_ALS_SPECTER_OF_STORM_UPGRADE (states also in CH2d), 27059 RNSI_SA_ALS_SEVER_UPGRADE, 27060 RNSI_SA_ALS_SUPPRESSION_SPECTER_UPGRADE.
- **Checklist rows: 3** (`awk -F'\t' '$5 ~ /(^| )CH2d2( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L522-L526 June SPEED_TIME additions (C3l): ASI_A_ARA_QUICK_STAB SPEED_; L664 CUSTOM_RENDER_STATE class users: Ara_Martial_Artist among th.
- Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.
- 7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).
