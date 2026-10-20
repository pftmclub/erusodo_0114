# June character Aisha (Stage 5 pilot) - Phase CH4 - Passives and class mechanics

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2b review (CH2 was split by CH0); not alongside CH3a / CH3b (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`, `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`, `ARME MEMO`) |
| Review | `kickoffs/pilot-char-revamp/CH4-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)" "7aj. Tree-UI class pieces June keyed to one class (from June skill system S7b)" "7ak. Skill page decide ACK class pieces (from June skill system S7d)"
```
Then the `ch4` rows of CH0's tsvs, and CH2's Done note.

## Task

Do "Phase CH4". Map each June mechanism in Ghidra before porting it (memorize: slot-data type 5 `328d9ac0`, tooltip `328e9160`, decide ACK
`32ce1770`, and the class side CH0 found). Delegate the mechanical reads, builds and log reads to subagents.
- If memorize needs a save change, snapshot `db_backup/` first. The rung is forward-only; propose it to the user before writing it.
- Edit CP949 files with `bytepatch.py` only.

## Amended by CH0

Written by CH0 (2026-10-20). Depends now read **CH2b review** (CH2 was split), and CH4 runs before or after CH3a / CH3b, never beside them.

Filters: `awk -F'\t' '$6=="CH4"' ch0_class_map.tsv` (20 functions), `awk -F'\t' 'NF==7 && $6=="CH4"' ch0_state_skill.tsv` (25 skill ids:
types 1 / 5 / 8), `awk -F'\t' '$5 ~ /CH4/' ch0_checklist_rows.tsv`.
- **Overrides:** `SetDamageExceptionProcess` `32cccc70` (vslot 0x2b8; 7b and the D8 Fitness 2413 -> Magical Staff 22409 swap, whole),
  `SetSpecificValueByBuffTempletID` `32cddec0` / `UnSet...` `32cde300` (0x24c / 0x250, Magical Makeup transform), `GetAdditionalAccelBuffTime`
  `32cc1b90` (0x7ac), `GetActualMPConsume` `32ccfe30` (0x6a0), `GetActualCoolTime` `32ce1cc0` (0x6a4, June-only override), `InitPassiveSkillState`
  `32cecd70` (0x690), the two skill-memo float hooks `32cc28b0` (0x428) and `32cc5c30` (0x73c), and their helpers.
- **Memorize:** 7ah (the 10-slot dialog's memorize statics), 7aj (slot data type 5 `328d9ac0`, `SESAPT_MEMORIZE` picture `328e0460`, mouse-in
  29059 `328ec719`, tooltip line `328e9160`; class helper `32ce1650` called from `328dfaa0`), 7ak (`32ce1770` on the decide-page ACK).
- **Buff skills (type 5):** the state is the shared `AVSI_SI_AV_COMMON_BUFF` (CH1); CH4 brings the buff data rows.

## From CH1c (2026-10-20)

- `InitPassiveSkillState` (`32cecd70`) builds the object at `+0x96ac` (`32cf2190`, 0x2c bytes: chance at `+0x10` = skill 22415 `SA_EFFECTIVE_RATE`, the refund rate = a protected float at `+0x20` = its ability `0x42`; the 22405 values sit in the container at `+0x14`)
  and sets the combo fireball element (`+0x9630`: `32ced3dc` 0, `32ced402` 2; where it becomes 1 (ice) was not found in CH1c's scan of the class range: find it), `+0x9634` (ball power), `+0x9638`, `+0x963c` / `+0x9640` (gravity). CH1c's inert `m_pJuneMpRefund` (`JuneMpRefundData { m_fChance, m_fRate }`) and
  `m_iJuneBallElement` are the stand-ins to replace; `June_FlushMpWithRefund` (`32cc2b90`) and `June_CreateComboFireBall` (`32ce4180`) read them.
