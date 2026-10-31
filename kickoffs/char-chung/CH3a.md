# June character Chung (Stage 7) - Phase CH3a - Skills: Iron Paladin (110)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3a` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2d review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH3a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
```
Then CH0's, CH1's and the CH2 Done notes, and the `CH3a` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 110 has that classes 6 / 20 do not. Same method as CH2 (its kickoff's Task): each id's case with June's state functions whole under `_SKILL`, attached rows,
effect closure, the upgrade-id templet swap, raw ids for unnamed rows, "this needs Opus" for an unsettled function.
- `NUM use` lines; March vs June damage in `MODS.md`. CP949: `bytepatch.py` only. Mark rows `(Chung: CH3a)`.
- Hand-off: the unit (18 Freils 117, 42 RapidT / 50 Chung99 111, 43 NovaT 110, or a CH0b unit), each skill id and its A / B slot (not 4 / 9 on an
  un-transcended unit), what June's motion and effect look like.

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

- **Functions: 17** (`awk -F'\t' '$7=="CH3a"' june2017_re/char_chung/ch0_class_map.tsv`; inside 16, new 1).
  June addresses: `32bdae80` `32bdaf00` `32bdaf90` `32bdafb0` `32bdafd0` `32bdaff0` `32bdbae0`(new CSI_SA_CIP_RAPID_GUARDIAN_Init) `32be79d0` `32be7a30` `32be7aa0` `32be7eb0` `32be7ee0` `32be7f10` `32be7f40` `32bf0c90` `32c076e0` `32c07990`.
- **States: 9** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_A_CIP_ARTILLERY_NOVA_CHARGE`, `CSI_A_CIP_ARTILLERY_NOVA_FIRE`, `CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START`, `CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN`, `CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING`, `CSI_A_CIP_LAND_SMASH`, `CSI_SA_CIP_CRACK_CRASHER`, `CSI_SA_CIP_RAGE_OF_GUARDIAN`, `CSI_SA_CIP_RAPID_GUARDIAN`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 26007 RNSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE, 26008 RNSI_SA_CIP_IRON_HOWLING, 26009 RNSI_SA_CFG_PAIN_OF_CALADBOLG, 26010 RNSI_SA_CTT_WONDER_WALL, 26037 RNSI_SA_CIP_CRACK_CRASHER, 26038 RNSI_SA_CIP_RAGE_OF_GUARDIAN, 26045 RNSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_UPGRADE, 26046 RNSI_SA_CIP_IRON_HOWLING_UPGRADE, 26047 RNSI_SA_CFG_PAIN_OF_CALADBOLG_UPGRADE, 26048 RNSI_SA_CTT_WONDER_WALL_UPGRADE, 26062 RNSI_SA_CIP_RAPID_GUARDIAN, 26206 RNSI_A_CIP_ARTILLERY_NOVA, 26207 RNSI_A_CFG_GUARD, 26226 RNSI_A_CIP_LAND_SMASH, 26230 RNSI_A_CIP_ARTILLERY_NOVA_UPGRADE, 26231 RNSI_A_CFG_GUARD_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L53 numerical-only DAMAGE_DATA table CSI_SA_CIP_CRACK_CRASHER (script L187; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7h L424 USEFUL_VALUE rows and getters; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- Split by CH0 (51 functions): CH3a = CIP-named skill states; CH3a2 = the CFG / CHG-named skills June's tree gives class 110.
