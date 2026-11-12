# June character Elesis (Stage 7) - Phase CH2a - Skills: Knight (8)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elesis CH2a` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Knight and the first jobs (classes 8, 29, 30, 35)" "Phase CH3 - Skills: second jobs (120, 121, 125), summons, hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH2a - Done"   # the worked example
awk -F'\t' '$7=="CH2a"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Scope: the skill ids class 8 has (a skill's line decides its phase). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` (`32b8c470`; CH0's
switch map names the case) with June's state functions whole (Ghidra and, for `listing-only` rows, `charport_adis.py elesis <va>`), inside March's
case under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SKILL` (March's in `#else`) or a new case; its block in `SetEquippedSkillLevel` `32b4e790` if it has one; its attached rows (7u names,
7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure
(a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its `WAY_OF_SWORD_TYPE` on CH1q's code. Upgrade ids reach their base
through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id. A skill whose factor
names a BFI id June's `Enum.lua` lacks (`JUNE_BUGS` 279) keeps June's nil. A function the map does not settle (new mechanism, unmapped callee, a
listing the control flow of which is not plain) is "this needs Opus": stop. Endurance Power (`32b80c60` / `32b80840`) and its memo, Sonic Assault's memo states are here.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elesis: <phase>)`.
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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).

## Amended by CH0 (2026-11-12)

- **Functions:** `awk -F'\t' '$7=="CH2a"' june2017_re/char_elesis/ch0_class_map.tsv` = 28 (16 listing-only: col 9 `n`; 16 `inside`, 12 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Script states:** `awk -F'\t' '$8=="CH2a"' june2017_re/char_elesis/ch0_state_skill.tsv` = 11.
- **Skill ids** (14): 28000, 28001, 28002, 28003, 28004, 28026, 28027, 28028, 28029, 28030, 28200, 28201, 28217, 28218.
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L52, L331, L728.
- **`ch0_wos.tsv` rows:** 2 (filter on its `proposed_phase` column).
