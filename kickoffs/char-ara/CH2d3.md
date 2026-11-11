# June character Ara (Stage 7) - Phase CH2d3 - Skills: Little Specter (34) A tier (ids >= 27200) (CH0 split of CH2d)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2d3` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2d2. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-ara/CH2d3-review.md` (Opus 5.5 high; the phase writes it when done); the CH2 stage review: covers CH2a-CH2d3, retires `_SKILL_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2d - Done"
```

## Task

Scope: the skill ids class 34 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is the A tier, skill ids >= 27200 (CH2d has ids < 27024 and the states with no skill, CH2d2 ids 27024-27199).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 29** (`awk -F'\t' '$7=="CH2d3"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 8, new 21).
  June addresses: `32b92c50`(new ASI_A_ARA_QUICK_PIERCING_Init) `32b92c80` `32b940d0`(new ASI_A_ALS_VIEW_BETWEEN_Init) `32b94150`(new ASI_A_ALS_VIEW_BETWEEN_FINISH_Init) `32b94290`(new ASI_A_ALS_SHAKE_CHAOS_Init) `32b94310`(new ASI_A_ALS_SHAKE_CHAOS_StateStart) `32b94340`(new ASI_A_ALS_SHAKE_CHAOS_StateStart) `32b94360`(new ASI_A_ALS_SHAKE_CHAOS_FINISH_Init) `32b94580`(new ASI_A_ALS_MANA_DRAIN_Init) `32b9c810` `32ba0680`(new ASI_A_ALS_VIEW_BETWEEN_FINISH_EventProcess) `32ba0cc0`(new ASI_A_ALS_SHAKE_CHAOS_FINISH_EventProcess) `32ba1340`(new ASI_A_ALS_MANA_DRAIN_StateStart) `32ba1370`(new ASI_A_ALS_MANA_DRAIN_EventProcess) `32bad3c0`(new ASI_A_ALS_VIEW_BETWEEN_EventProcess) `32bad450`(new ASI_A_ALS_SHAKE_CHAOS_EventProcess) `32bbcdc0`(new ASI_A_ALS_VIEW_BETWEEN_FrameMove) `32bbcff0`(new ASI_A_ALS_VIEW_BETWEEN_FINISH_FrameMove) `32bbd240`(new ASI_A_ALS_SHAKE_CHAOS_FINISH_FrameMove) `32bc3190`(new ASI_A_ALS_VIEW_BETWEEN_StateStart) `32bc3260`(new ASI_A_ALS_SHAKE_CHAOS_FrameMove) `32bc3bc0`(new ASI_A_ALS_MANA_DRAIN_FrameMove) `32bc3e40`(new ASI_A_ALS_MANA_DRAIN_StateEnd) `32bc9400` `32bc9450` `32bce810` `32bce8c0` `32bce920` `32bce9c0`.
- **States: 7** (`awk -F'\t' '$8=="CH2d3"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_A_ARA_QUICK_PIERCING`, `ASI_A_ARA_QUICK_PIERCING_TRENSCENDENCE`, `ASI_A_ALS_VIEW_BETWEEN`, `ASI_A_ALS_VIEW_BETWEEN_FINISH`, `ASI_A_ALS_SHAKE_CHAOS`, `ASI_A_ALS_SHAKE_CHAOS_FINISH`, `ASI_A_ALS_MANA_DRAIN`.
- **Skills: 8** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27214 RNSI_A_ALS_VIEW_BETWEEN, 27215 RNSI_A_ALS_SHAKE_CHAOS, 27216 RNSI_A_ALS_MANA_DRAIN, 27217 RNSI_A_ALS_QUICK_PIERCING, 27225 RNSI_A_ALS_VIEW_BETWEEN_UPGRADE, 27239 RNSI_A_ALS_SHAKE_CHAOS_UPGRADE, 27240 RNSI_A_ALS_MANA_DRAIN_UPGRADE, 27241 RNSI_A_ALS_QUICK_PIERCING_UPGRADE.
- **Checklist rows: 6** (`awk -F'\t' '$5 ~ /(^| )CH2d3( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_A_ARA_QUICK_PI; L55 numerical-only DAMAGE_DATA_NEXT table in state ASI_A_ARA_QUI; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L178 June-only members of tables March has (SKILL_ID names, ARA_S.
- Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.
- 7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).
