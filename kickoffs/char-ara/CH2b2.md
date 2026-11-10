# June character Ara (Stage 7) - Phase CH2b2 - Skills: Little Hsien (28) A tier (ids >= 27200) (CH0 split of CH2b)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH2b2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2b. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
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

Scope: the skill ids class 28 has and class 7 lacks (a skill's line decides its phase). **CH0 split: this phase is the A tier, skill ids >= 27200 (CH2b has the SA tier, ids < 27200).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 28** (`awk -F'\t' '$7=="CH2b2"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 28).
  June addresses: `32b922b0` `32b92300` `32b92340` `32b92360` `32b92a60` `32b92ab0` `32b92ad0` `32b92d70` `32b92e60` `32b92f50` `32b92f90` `32b9b740` `32b9c410` `32b9cb10` `32b9cd20` `32baa560` `32bac430` `32bac6d0` `32bac7f0` `32bb7170` `32bb7490` `32bba350` `32bba6d0` `32bba730` `32bc8f40` `32bc8fb0` `32bc9170` `32bc91e0`.
- **States: 7** (`awk -F'\t' '$8=="CH2b2"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_A_ARA_TIGER_CLAW_START`, `ASI_A_ARA_TIGER_CLAW_END`, `ASI_A_ALH_ROCK_CRASH_FALLING_START`, `ASI_A_ALH_ROCK_CRASH_END`, `ASI_A_ASD_FALLING_BLOSSOM_Start`, `ASI_A_ASD_FALLING_BLOSSOM_Finish`, `ASI_A_ARA_QUICK_STAB`.
- **Skills: 8** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27202 RNSI_A_ALH_ROCK_CRASH, 27203 RNSI_A_ALH_TIGER_CLAW, 27204 RNSI_A_ALH_DRAGON_FALLING, 27205 RNSI_A_ALH_QUICK_STAB, 27223 RNSI_A_ALH_ROCK_CRASH_UPGRADE, 27229 RNSI_A_ALH_TIGER_CLAW_UPGRADE, 27230 RNSI_A_ALH_DRAGON_FALLING_UPGRADE, 27231 RNSI_A_ALH_QUICK_STAB_UPGRADE.
- **Checklist rows: 6** (`awk -F'\t' '$5 ~ /(^| )CH2b2( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_A_ARA_QUICK_ST; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L522-L526 June SPEED_TIME additions (C3l): ASI_A_ARA_QUICK_STAB SPEED_.
