# June character Lu/Ciel (Stage 8) - Phase CH3a0 - Summoned NPCs: data rows for every Lu/Ciel summon

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH3a0` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2d2 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH3a0-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH3a0` row of "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
from an A and a B slot on the body that casts it; the hand-off gives the unit, class, skill id and slot, never slot 4 / 9 before
transcendence.

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

## Amended by CH0 (2026-11-02)

- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH3a`).
- **Moved from CH2a0** (CH0): the first spawn is Dread Lord's (`ch0_summons.tsv`). One spawn rule for the class (CreateNPCReq from a state FrameMove); the Garden NPC's own Lua spawns are CH3c20's.
- Scope: June's `NPCTemplet.lua` rows (core list) for 1906 `NUI_SI_SA_LLD_SUMMON_DARKNESS`, 1937 `_PVP`, 1908 `NUI_SI_SA_LCN_SHADOW_BOLT`, 2269 `NUI_SA_LDN_GARDEN_OF_HAURES`, 2270 `NUI_SA_LDN_GARDEN_OF_HAURES_ADD`; their `NPC_UNIT_ID` names in the live `Enum.lua` (core list) and 2270 in `X2UnitManager.h` (2269 is CH1a's); their motions / meshes are in CH0b's list. 1907 `NUI_SI_A_LCN_SHADOW` gets nothing (June ships no script and nothing spawns it). `luabind.py check` PASS for 1906 / 1937 / 1908 / 2270.
