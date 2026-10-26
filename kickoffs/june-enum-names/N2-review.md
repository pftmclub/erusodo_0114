# June enum names - Phase N2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-enum-names N2 review` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Rules" "Phase N2 - Register the members" "Phase N2 - Done"
```

## Task

Review N2's commit against June's `Enum.src.lua` (271 registered members, 6 C++ enums, 19 `Enum.lua` lines). What it could get wrong:

- **A sentinel moved.** The members sit after `UC_END`, `NUI_MAX_LIMIT` and `DTT_END`, and before the closing `};` of `SKILL_ID` / `BUFF_TEMPLET_ID` / `BUFF_FACTOR_ID`. Check that no implicit-numbered member follows an inserted block (it would renumber), and that `UC_END` = 124, `NUI_MAX_LIMIT` = 10196, `DTT_END` = 5 still hold (the Done note's `enumscan` run reads them from `cl /P`).
- **A value is not June's.** Spot-check 6 members by hand against `june_src_fixed/data036_kom/Enum.src.lua` (one per enum, e.g. `RNSI_P_EBS_OPEN_MIND` 25419, `UC_AIN_APOSTASIA` 136, `BTI_BUFF_HYPER_MODE_AIN_3RD` 968, `BFI_BUFF_CODE_RECOVERY_ECB_PVP` 1119, `NUI_PVE_BLUE_KNIGHT_BASTARD_EVENT_TITLE` 4120, `DTT_ECE_REASON_PRIDE_FERDINAND_ATTACK` 44). The generator already asserted all 271 against the same file, so this checks the tsv-to-name step, not the arithmetic.
- **`Enum.lua` side effect.** The 19 names are the ones a live (shipped) `.lua` indexes and `Enum.lua` lacks (`NPCStat.lua` data037, `NewSkillTempletVer2.lua`, `Arme_VioletMage.lua`, `PVP_HERO_HINO.lua`, `NPCStat_INT.lua` datai016). Confirm adding them does not make a script load rows for an NPC / skill that has no template (a spawn crash). Not packed yet; pack + copy only after the review (ask the user before the copy).
- **`core --check` FAIL is not N2's:** `data036 ResultData_new.lua` (dated 2026-10-19, Stage 29 `june-result`) is BOM text off the core list. `ledger --check` is PASS.

Hygiene: `python tools/flagcheck.py JUNE_ENUM_NAMES --range <N2 commit>^..<N2 commit>`. Paste its `files:` / `RESULT:` lines. The 10 `eol!` lines in `X2UnitManager.h` are the anchor line's CRLF inside an LF region (the tool's rule), not a defect.

Fix clear errors under the same flag (then `tools/build.py --deploy`); write "Phase N2 - Review" into the plan doc, set the STATUS row, check that the N3 / N4 / N5a-c kickoffs still name members that now exist, and commit.
