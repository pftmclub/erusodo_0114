# June character Rena (Stage 7) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3d` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*`, `X2Offline*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`), `SERV_IRUHADEV_JUNE_CHAR_RENA_TRANSCEND` |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`) |
| Review | `kickoffs/char-rena/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3d - Done" "CH3d - Review"
```
Then CH0's Done note and the `CH3d` rows of `ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Do the CH3d row of "Phase CH3": hyper actives (type 13, 6 rows; June's hyper switch in `32cb06d0`), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the
transcendence rows the template lists: classes 102 / 103 / 114 on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`),
`LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Lena.lua` (March ships a same-name file: June's replaces it), checklist 7ao whole (hyper
effect start `32c9b3d0`, uncarved; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`; l.969), `SPRT_HYPER_CLASS` skills and
`TRANSCENDENTAL_SKILL_DATA` rows, `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_RWS/RGA/RNW_Transcendence`, `list_TR_*` rows in `GetCharacterImageName`
(T6c) read from June's row pushes, `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` present. Then `python tools/skillicons.py check <Rena's skill-id range>`
= PASS.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps. CP949 files through `bytepatch.py`. Mark rows `(Rena: CH3d)`.
Hand-off: a transcended unit (`settranscend`) per second job, the hyper-class skills in A-5 / B-5 (hyper-class passives read level 0 until transcended),
the transcended picture and cut-in.

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

## Amended by CH0

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
