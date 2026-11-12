# June character Elesis (Stage 7) - Phase CH3d - Hyper actives and transcendence rows (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH3d` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH3c. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SKILL` and `SERV_IRUHADEV_JUNE_CHAR_ELESIS_TRANSCEND` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS SKILL`) |
| Review | `kickoffs/char-elesis/CH3d-review.md` (Opus 5.5 high; the phase writes it when done); the CH3 stage review: covers CH3a-CH3d, retires `_SKILL_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH3 - Skills: second jobs (120, 121, 125), summons, hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # the transcendence rows block
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH3d - Done" "CH3d - Review"
```

## Task

Do CH3d: the hyper actives (type 13; the `id - 28601` switch in `32b8c470`, 5 values `RNSI_HA_SBH_ANCIENT_FIRE` .. `RNSI_HA_SCA_FATE_SMASHER_UPGRADE`,
plus the Grand Master one CH0 maps) and the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), then the **transcendence rows** for 120 / 121 / 125: the
offline allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`; today an Elesis answers the cannot-transcend error), her
`LevelUpEvent.lua` transcendence row (`june_src_fixed/data036_kom/LevelUpEvent.src.lua` L589-600) and `DLG_Transcendental_Change_Guide_Elesis.lua`;
checklist 7ao whole (`HyperModeBuffEffectStart` 0xc4 `32b74300` for a transcended unit; the four base helpers `31fcac90` / `31fcad70` / `31fceab0`
`== 1` / `31ff1e80`); her hyper-class skills (`SPRT_HYPER_CLASS`: 28416 / 28417 `RNSI_P_SBH_TRANSCENDENCE` and the Grand Master / Crimson Avenger
ones) and `TRANSCENDENTAL_SKILL_DATA` rows; the transcended side of `EFFECT_SET_LIST_EX_NO_TRANS` / `_EX_TRANS_1`; `GetHyperModeTableName`
(`31fdd490`) and `HQ_Cutin_EGM / EBH / ECA_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and her transcended pictures, read
from June's row pushes (not from another class's file); `POPUP_TR_G_MASTER` / `_B_HEART` / `_C_AVENGER` present (CH0b). Then `python
tools/skillicons.py check <her range>` = PASS. Play-test on transcended units (`settranscend`) of 120 / 121 / 125 (units 29, 30 and CHNa's level-80
125): hyper-class skills cast, slots A-5 / B-5 work, the transcended picture and cut-in show. You are on Opus: delegate the reads, probes and builds.

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

- **Functions:** `awk -F'\t' '$7=="CH3d"' june2017_re/char_elesis/ch0_class_map.tsv` = 7 (6 listing-only: col 9 `n`; 5 `inside`, 2 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Script states:** `awk -F'\t' '$8=="CH3d"' june2017_re/char_elesis/ch0_state_skill.tsv` = 3.
- **Skill ids** (6): 28600, 28601, 28602, 28603, 28604, 28605.
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L259, L331, L728, L965, L988.
- **`ch0_classn.tsv` rows:** 4 (filter on its `proposed_phase` column).
- `SI_HA_SCA_FATE_SMASHER` (bare `SI_`) is here. `TranscendCheck` (`Handlers_Quest.cpp`) refuses 120 / 121 / 125 today (error 1726): her allow-list rows; June's level-70 `LevelUpEvent.lua` row.
