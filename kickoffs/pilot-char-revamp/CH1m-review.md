# June character Aisha (Stage 5 pilot) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1m review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (reader, parse, Arme fixes), `SERV_IRUHADEV_JUNE_INPUT_SIT_KEY` (InputData Sit group) |
| Log tag | none (inert until the final CH1) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1m - Done (2026-10-21, Opus)"
```

## Task

Review CH1m's commit against June's listings and the March code:
- **`CX2GUUser::JuneCommonSpecialAttackEventProcess`** against `31fcadd0`: guard order (`322731a0`, `+0x8ac4`, `3226e2e0` = `IsCanNotIntput`), the A keys and F (4, no lock test), T (9, `32890ee0( 1 )` and the `g_pData` null path), the expand gate, Q/W/E/R 5-8 (else-if chain). Agree or overturn the design call (June's reader for Aisha only; March's reader for the rest).
- **The Sit key:** `0x27` = GAMEACTION_SIT (KeyProcess `322f7170 @322f7fa4`, `31f5da70`), the group's placement in X2Game.cpp (after the skill keys, both swap branches), the double-byte quirk (JUNE_BUGS 381), and `JUNE_ARME_INPUT_8726` = `pureSit` against `32cc7071` / `32cc7200`.
- **`June_SelectSkillSlotKind`** against `32cc7e60` and `3288fff0`: `oneSit` (`+0x8727`), index -1 / first match / B + 5.
- **`SPECIAL_OPTION_VALUE`** against `@320608a9-3206092e`: clear, values 1..n, placement before `TIME_STOP_IMMUNE_STATE`; Arme's getter on the user list.
- **The script-key verdicts and routes** (Stage 2 immunity lists, Stage 9 camera keys, checklist row 397): check each address cited.
- A `cl /Zs` probe with LIVE defined. Write the next kickoff note in `CH1.md` if one is needed.
