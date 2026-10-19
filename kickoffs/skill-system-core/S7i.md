# June skill system core - Phase S7i - The level-up "new skill or memo" emblem

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7i` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7 (the level-up handlers' closing block), S3b (the memo rows `32451250`) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_*` |
| Log tag | `[JUNE_SKILL] EMBLEM` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7i-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7f - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7f`, `Skill S7` (`323ae130 / 32473f70 / 325346e0`) and the `3244be60` / `3244c8c0` emblem rows.

## Task

Added by S7f (2026-10-19, unowned-branch rule). On level-up June's `31d885e0` collects the skills and memos that open at the new level
(`CX2SkillTree 3220d2b0` + `3220b990`: types 1 / 3 new skill, 4 a higher skill level, 2 a memo) and shows emblem 17
`EI_GET_NEW_SKILL_OR_MEMO` with them (`CX2EmblemManager +0x64` list via `324486b0`, `32447d70( 0x11, true )`).

1. Identify the emblem side: who reads the manager's `+0x64` list (candidates by displacement: `324483e0`, `32448420`, `32448ce0`, `3244adb0`, `3244b780`,
   `3244bb40`, `3244bfb0`, `3244c4b0`) and what it draws (skill icons, the memo variant), June's `EMBLEM_ID` 14-34 against March's 0-13, and June's parser
   `32449000` keys March lacks (`EXPAND_TEXTURE_FILE_NAME`, ...). Check whether June's `Emblem_Manager.lua` already replaced March's (same-name rule).
2. Port `31d885e0`, `3220d2b0`, `3220b990` and the emblem pieces they need, whole; add the call to every level-up handler March has (S7's `JuneAutoEquipOnLevelUp` sites).

**Play-test:** level a character across a level that opens a skill or a memo; the emblem shows the new icons.
