# June character Add (Stage 7) - Phase CH2b - Skills: Psychic Tracer (32)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-add CH2b` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH2a (and the CH2b0 review if a summon is cast here). One at a time with CH1-CH4: they all edit `X2GUAdd.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH2d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Nasod Ruler and the first jobs (classes 9, 32, 33, 36)" "Phase CH3 - Skills: second jobs (122, 123, 126), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CH2b - Done"   # once written; else Ara's
```

## Task

Scope: the skill ids class 32 has that class 9 does not (a skill's line decides its phase). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's addresses), inside
March's case under `SERV_IRUHADEV_JUNE_CHAR_ADD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h UPDOWN
cancels, §7 l.256 power-rate operator, the numerical-only rows of `plan_numerical_rows.tsv`, the effect-modifier tags 7a l.306 / l.311 and alpha tags
7c l.356, `USEFUL_VALUE` readers l.427, `GetLandPosition( ..., true )` l.680, the `CanDown( NULL, &idx )` shape l.781); its effect closure (a name
found only now is staged byte-exact and added to `ch0_assets.tsv`); its DP gain / cost on CH1q's code. Upgrade ids reach their base through the
templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id. A function the map does not settle
(new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Add: <phase>)`.
- Hand-off: the unit and class, each skill id and its A / B slot (slots 0-3 / 5-8, never 4 / 9 unless the check is about transcendence; slot every
  tested skill yourself, never ask the user to swap), each cast twice (cool time, MP or DP), what June's motion and effect look like.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
