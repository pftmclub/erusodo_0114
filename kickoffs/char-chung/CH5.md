# June character Chung (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH5` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_UI` |
| Log tag | `[JUNE_CHAR]` (`CHUNG UI`) |
| Review | `kickoffs/char-chung/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH5 - Done (2026-10-28, Sonnet 5.5)"
```
Then CH0's item 9 (UnitTemplet and cut-in findings) and `june2017_re/g4_cutin_names.tsv`.

## Task

Do "Phase CH5": Chung's 7 `UnitTemplet.lua` rows become June's (core-list BOM source; edit as text, `luasrc.py refresh`; every other
character's row stays): `m_fLobbyRotationY`, `m_BerserkCutIn` / `m_HyperCutIn` for 110 / 111 / 117, Tactical Trooper's `name` `STR_ID_17435` (if
CH0 found the string missing, keep March's id and route it). The cut-in table `X2GUChung.cpp:63-120` and the InitState index switch `:781-838`
replaced by June's names and layout under `_UI` (March's in `#else`), art staged byte-exact if CH0b missed it. Pack data036 and ask before the
copy. Hand-off: a select-screen pass of all nine March characters; Chung's skill and hyper cut-ins per class.

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

## Amended by CH0 (2026-10-31)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7t L591-593 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn / _Trans keys and t; §7t L617-622 skill cut-in names -> June .gfx; replace s_SkillCutInSet and the InitS.
- `ch0_unittemplet_chung.tsv`: June adds `m_fLobbyRotationY` (6: 10, 20: 10, 21: 20, 27: 1, 110: 20, 111: 30, 117: 25; March ignores it) and for 110 / 111 / 117 `m_BerserkCutIn` / `m_HyperCutIn` (`HQ_Cutin_{CIP,CDC,CTT}_1_Transcendence` / `_Transcendence`). Tactical Trooper `name` STR_ID_9739 -> STR_ID_17435: present (`StringID_def`, General.ess row 17435 "Tactical Trooper"). Every other key equal.
- June's cut-in set index by class (InitState `32c22540`): 6 -> 0, 20 -> 1, 21 -> 2, 27 -> 5, 110 -> 3, 111 -> 4, 117 -> 6.
- StatTable: 110 / 111 have 99 live rows each; the 19 extra are commented copies, equal (no duplicate to route).

## Amended by CH1q (2026-11-02)

- Cannon-ball readers outside the class that CH1q did not take (`ch0_cannonball.tsv` rows, June sites not located): the skill-slot fade
  `X2Game.cpp` ~15011 (`GetCannonBallCount() < m_usCBConsumption`; does June test the total with the extra balls?), the F7 refill ~5835 and
  `X2StateMenu.cpp` ~5181. `AddChungCannonBall_Cheat` / `AddChungExtraCannonBall_Cheat` (June `GameEditCommandList.lua`, unbound in March: binder
  names differ from March's `AddChungCannonBall`) only matter if the dev console runs June's list.
