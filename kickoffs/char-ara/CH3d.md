# June character Ara (Stage 7) - Phase CH3d - Hyper actives and transcendence rows (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH3d` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3c2. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` and `SERV_IRUHADEV_JUNE_CHAR_ARA_TRANSCEND` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`) |
| Review | `kickoffs/char-ara/CH3d-review.md` (Opus 5.5 high; the phase writes it when done); the CH3 stage review: covers CH3a-CH3d, retires `_SKILL_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # the transcendence rows block
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH3d - Done (2026-11-10)" "CH3d - Review (2026-11-10)"
```

## Task

Do CH3d: the hyper actives (type 13; June's hyper switch in `32bd09d0`) and the 6 `SHOW_HYPER_CUT_IN_TIME` states (7t), then the **transcendence
rows** for 118 / 119 / 124: the offline allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp:1411`; today Ara answers NetError 1726), her
`LevelUpEvent.lua` TYPE rows and `DLG_Transcendental_Change_Guide_Ara.lua`; checklist 7ao whole (hyper effect start `32bb22d0` returns at once when
transcended; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`); her hyper-class skills (`SPRT_HYPER_CLASS`) and
`TRANSCENDENTAL_SKILL_DATA` rows; `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_ASD / AYR / ASR_Transcendence`; `list_TR_*` rows in
`GetCharacterImageName` (T6c) and her transcended pictures, read from June's row pushes (not from another class's file); `POPUP_TR_S_DEVANAM` / `_Y_RAJA` /
`_ASURA` present (CH0b). Then `python tools/skillicons.py check <Ara's range>` = PASS. Play-test on transcended units (`settranscend`) of 118 / 119 / 124:
hyper-class skills cast, slots A-5 / B-5 work, the transcended picture and cut-in show. You are on Opus: delegate the reads, probes and builds.

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

- **Functions: 8** (`awk -F'\t' '$7=="CH3d"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 6, new 2).
  June addresses: `32b92fc0` `32b93ec0` `32b95290`(new ASI_HA_AA_NINE_TAILED_FOX_Init) `32b9cf50` `32b9ed80` `32ba3b80`(new ASI_HA_AA_NINE_TAILED_FOX_EventProcess) `32baab30` `32baace0`.
- **States: 3** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_HA_ASD_THOUSANDS_BLOSSOMS`, `ASI_HA_AYR_ENERGY_CANNON`, `ASI_HA_AA_NINE_TAILED_FOX`.
- **Skills: 6** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27600 RNSI_HA_ASD_THOUSANDS_BLOSSOMS, 27601 RNSI_HA_AYR_ENERGY_CANNON, 27602 RNSI_HA_ASR_NINE_TAILED_FOX, 27603 RNSI_HA_ASD_THOUSANDS_BLOSSOMS_UPGRADE, 27604 RNSI_HA_AYR_ENERGY_CANNON_UPGRADE, 27605 RNSI_HA_ASR_NINE_TAILED_FOX_UPGRADE.
- **Checklist rows: 9** (`awk -F'\t' '$5 ~ /(^| )CH3d( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L592 "SHOW_HYPER_CUT_IN_TIME in the class script (""plus Ara's be; L594 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and ; L607 hyper active skills: cut-in shown whatever the option says (; L713-L745 an id of a listed type with no case plays the class's first ; L814 TC slot A-5 / B-5 states and the Stage 6 skips; L954 every class also brings: emulator allow-list id, LevelUpEven; L959 hyper-effect start returns at once for a transcended unit; L978 transcend Ara: 32bb22d0 hyper effect start (vslot 49) branch.
- **CH-N (class path) rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH3d( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).
- **Hyper split.** June's `0x6bd0 < id` switch (27601 / 27604 `32baab30`, 27602 / 27605 `32baace0`, 27603 and `== 0x6bd0` Thousands Blossoms inline, default `@32bd271e` = the Wind Wedge default) goes in the slot head's active arm, where CH1n left the comment `June's hyper split ( 27600 and up ) is CH3d's`.
