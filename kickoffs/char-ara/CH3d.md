# June character Ara (Stage 7) - Phase CH3d - Hyper actives and transcendence rows (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH3d` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3c. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
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


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
