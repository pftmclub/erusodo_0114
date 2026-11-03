# June character Elsword (Stage 7) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH3d` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*`, `X2Offline*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`) |
| Review | `kickoffs/char-elsword/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH3d - Done (2026-11-03)"
```
Then CH0's Done note and the `CH3d` rows of `ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Do the CH3d row of "Phase CH3": hyper actives (type 13, 6 rows; June's nested hyper switch 0x5461-0x5465 in `32d223e0`), the 3
`SHOW_HYPER_CUT_IN_TIME` states (7t), and the transcendence rows the template lists: classes 100 / 101 / 112 on the offline transcend
allow-list, `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Elsowrd.lua` (June's spelling), checklist 7ao whole (hyper effect start
`32d17ea0`, uncarved: `charport_adis.py elsword 32d17ea0`; the four base helpers), `SPRT_HYPER_CLASS` skills and `TRANSCENDENTAL_SKILL_DATA` rows,
`GetHyperModeTableName` and `HQ_Cutin_ELK/ERS/EIS_Transcendence`, `list_TR_*` rows in `GetCharacterImageName` read from June's row pushes,
`POPUP_TR_L_KNIGHT` / `_R_SLAYER` / `_I_SWORD` present. Then `python tools/skillicons.py check <Elsword's skill-id range>` = PASS.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps. CP949 files through `bytepatch.py`. Mark rows `(Elsword: CH3d)`.
Hand-off: a transcended unit (`settranscend`) per second job, the hyper-class skills in A-5 / B-5, the transcended picture and cut-in.

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

## Amended by CH0 (2026-11-03)

- **Functions: 5** (`awk -F'\t' '$7=="CH3d"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 5).
  June addresses: `32cf4930` `32cf49d0` `32cf4ab0` `32d00be0` `32d00de0`.
- **States: 3** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_HA_ELK_GIGANTIC_SLASH`, `ESSI_HA_ERS_SHINING_RUNE_BUSTER`, `ESSI_HA_EIS_BLADE_RAIN`.
- **Skills: 6** (part 2 of `ch0_state_skill.tsv`, col 6): 21600 RNSI_HA_ELK_GIGANTIC_SLASH, 21601 RNSI_HA_ERS_SHININGRUNE_BUSTER, 21602 RNSI_HA_EIS_BLADE_RAIN, 21603 RNSI_HA_ELK_GIGANTIC_SLASH_UPGRADE, 21604 RNSI_HA_ERS_SHININGRUNE_BUSTER_UPGRADE, 21605 RNSI_HA_EIS_BLADE_RAIN_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7t L592 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cut; §7t L604-608 hyper active skills: cut-in shown whatever the option says (type 13); §7ag L807-809 TC slot A-5 / B-5 states and the Stage 6 skips; §7ao L957 transcend readers: 32d17e90 hyper effect start (vslot 19, slot 0x4c); ; §7ao L940-945 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
