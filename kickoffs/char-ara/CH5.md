# June character Ara (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH5` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2 (its Touches do not overlap `X2GUAra.*` except the cut-in table: not beside a CH2 phase editing the same lines) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_UI` (+ `..._UI_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA UI`) |
| Review | `kickoffs/char-ara/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH5 - Done (2026-11-09)" "CH5 - Review (2026-11-09, Opus 5.5): PASS, no code change"
```

## Task

Do "Phase CH5": Ara's 5 `UnitTemplet.lua` rows (7, 28, 118, 31, 119) become June's (`june_src_fixed/data036_kom/UnitTemplet.src.lua` L1158 / L1184 /
L1210 / L1342 / L1368) and June's rows for 34 (L1397) and 124 (L1423) are added (core-list BOM source; `luasrc.py refresh` after; every other row stays;
if CH0 found the templet loader refuses a class >= `UC_END`, the two new rows wait for CHNa); the cut-in table `X2GUAra.cpp:36-60` and the InitState
index switch `:280-310` replaced by June's names and layout (7t cut-in `.gfx` names, `HQ_Cutin_ASD / AYR / ASR_Transcendence`); 7ah; the lobby orb
handles `m_hSeqEnergyBullet1-3` (`X2UnitViewerUI.h:337-339`, `.cpp:80-82`, `:2465-2501`, checklist 7e l.368) **retired** (Open decision 2, answered 2026-11-10: under the unit viewer's flag, March's code in `#else`, as the other eight classes). Pack
data036 and copy it. Done gate: a select-screen pass of all nine March characters with no new error; Ara's skill and hyper cut-ins play June's art;
the second-job names show June's strings; the lobby shows Little Devil's orb once (`ALD_LobbyWait`).

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


## Amended by the user (2026-11-10)

- **Creation screen is part of this port.** Add to the Task: Ara's entries on the character-creation screen for classes 7 / 28 / 31 / 118 / 119: adopt `DLG_Unit_Create_ClassChange_Button_Ara.lua` (`adopt-june`, `luabind.py check`; row owner moved to `char-ara` / `CH5` in `ch0_assets.tsv`), list every June function the creation state and unit viewer run for Ara from Ghidra, port them under `_UI`. Done gate adds: open the creation screen, pick Ara, each class preview (and the `Movie_Create_Ara_*` movies) shows June's art, no new error. 34 / 124 are CHNa's.

## Amended by CH0 (2026-11-10)

- **Checklist rows: 4** (`awk -F'\t' '$5 ~ /(^| )CH5( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L368 unit-viewer ALD_LobbyWait row; March's hand-made orb (m_hSeq; L594 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and ; L620 skill cut-in names -> June .gfx: June HQ_CutIn_Ara_*; March ; L954 every class also brings: emulator allow-list id, LevelUpEven.
- **Force-power rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH5( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: same 1).
- **CH-N (class path) rows: 6** (`awk -F'\t' '$7 ~ /(^| )CH5( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- `ch0_unittemplet_ara.tsv`: 7 / 28 / 31 add `m_fLobbyRotationY` only (1 / 15 / 10); 118 / 119 add `m_fLobbyRotationY` (1 / 15), `m_HyperCutIn` (`HQ_Cutin_ASD_Transcendence` / `_AYR_`) and `m_BerserkCutIn` (`HQ_Cutin_ASD_1_Transcendence` / `_AYR_1_`); 34 (24 keys, `STR_ID_31537` / `_31539`, `Motion_Ara_LS.x`, `Movie_Create_Ara_AA.avi`) and 124 (27 keys, `STR_ID_31538` / `_31540`, `m_GameMotion2 = Motion_Ara_AA.x`) are new. All string ids are in `general.ess`. The templet loader has no `UC_END` refusal: the 34 / 124 rows go in here (CHNa needs them).
- The live data033 already holds June's `Motion_ARA_Common.x` and `Motion_ARA_Lobby.x`; they lack `ALD_lobbyWait` (June: `ALD_LobbyWait`; played at `X2SquareUnit.cpp:1079`, `X2UnitViewerUI.cpp:2173`) and `AYR_LobbyReadyAtcion` (`X2UnitViewerUI.cpp:5260`): class 31 / 119 lobby idles. Check whether those sites are live and what June plays there; fix in this phase (Ara's lobby).
