# June character Aisha (Stage 5 pilot) - Phase CH4c - Aisha's overrides, Meditation and the aura rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4c` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH4b review; not alongside CH4b / CH4d (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH4c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH4 - Passives and class mechanics" "CH4 split (2026-10-22, Opus)" "CH4a - Done (2026-10-22, Opus 5.5)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7b"   # use --list for the exact heading
```
Then the `ch4` rows of `june2017_re/char_arme/ch0_class_map.tsv` and `ch0_state_skill.tsv` (filters in `CH4.md`).

## Task

Port whole, each in the listing (`june2017_re/funcdis.py --func <va>`; most are not carved in Ghidra):
- `SetDamageExceptionProcess` `32cccc70` (vslot 0x2b8; checklist 7b and D8: Fitness 2413 -> Magical Staff 22409, whole);
- `GetActualMPConsume` `32ccfe30` (reads `+0x95b0` `m_fMPConsumeRateTeleport`, `+0x95bc`, `+0x97a0` under `322cc1a0`); March's override is live today;
- `GetActualCoolTime` `32ce1cc0` (June-only override, reads `+0x9638` `m_fElementalResearchRate`, which CH4a now sets: 1.0 or `SA_DECREASE_REL`);
- `GetAdditionalAccelBuffTime` `32cc1b90`;
- `Set/UnSetSpecificValueByBuffTempletID` `32cddec0` / `32cde300` with `TransformStart` `32cc91b0` and `32cdbd00` (CH2b left TransformStart March's);
- the memo float hooks `32cc28b0` (vslot 0x428) and `32cc5c30` (0x73c);
- Meditation 22203 / 22227 and March's `ADD_MEMO_1ST_CLASS` branches inside these functions, kept or removed as June has them;
- the type-5 aura rows (22202 / 22207 / 22212 and upgrades, 106) and 7ae's `ResetLeftSkillCoolTimeBySkillType( ST_SPECIAL_ACTIVE )` rule `32895390` if
  Aisha reaches it;
- find a reader of `+0x9790` / `+0x9794` (Mana Flow Overdrive, `SA_MAX` / `MIN_INCREASE_MOVEMENT_BY_MANA`) outside the Aisha module, or record none.

Record both damage numbers in `MODS.md` for any skill whose damage changes. Delegate the mechanical reads, builds and log reads to subagents.
Edit CP949 files with `bytepatch.py` only.
