# June skill system core - Phase S7e - The tree's description panels: major skills and the command tab

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7e` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7b (June's tree class); not alongside S7c / S7d (all three edit `X2UISkillTreeNew2015.*`) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS` |
| Log tag | `[JUNE_SKILL] UI` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7e-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7b - Done (2026-10-18, Opus 5.5 high)" "Phase S7e - Description panels"
```
`june2017_re/s0b_ui_funcmap.tsv` rows for the addresses below.

## Task

Split out of S7b (2026-10-18, rule "plan unowned June branches without asking"): June's tree window has two panels fed by data March does not load.

- **Major-skill panel** (`328e42d0`, from the portrait builder `328e8e90`): `SkillTree_MajorSkill`, `SkillTree_Back_Black`, `Skill_Description` (the class's DESCRIPTION_STRING_ID, fitted by `328df060`),
  one `MajorSkillBack%d` static + `MajorSkillButton%d` button per SKILL_DATA row (button art = the skill icon, guide text = the row's string, line-broken to 0x172). With transcendence
  (`CX2Unit +0x3c`, Stage 6) and the 0x910 / 0x911 checks the TRANSCENDENTAL_SKILL_DATA rows replace them. Mouse-over / out: UICustomEventProc 29061 / 29062 (`328ec85b` / `328ec9b6`).
- **The data** is `SkillCommandDescriptionData.lua` (June data036, June-only): `CX2SkillTree 3220bdb0( class, isLuciel, tree +0x3c )` reads the class table ( SKILL_DATA, TRANSCENDENTAL_SKILL_DATA,
  COMMAND_DATA.MAJOR_COMBO, COMMAND_DATA_MINOR for Luciel ) into tree `+0x3c` ( four { string id, vector } groups, reset by `328ee430` ). Bring the file in with `luasrc.py adopt-june`; June reads
  it with its inline XORCRC kom loader, March uses its own shipped loader (`LoadLuaTinker` / KLuaManager, as `X2SkillNoteManager.cpp` does). The class-name key is June's `32375e30`.
- **Command tab** (`Command_Tab` radio already switches lists in S7b): `328e7ba0` reads the class table of `ComboTree.lua` (June's data036 copy; March has its own for `CX2ComboTree`, compare first)
  through `328df4c0` (INDEX_%d rows), builds the rows (`328e4bd0`), the command tooltip buttons (`328de9a0`, maps `328da200` / `328da290` at tree `+0x90` / `+0x94`), the major-command panel
  (`328e5290`, `328e8820`), the description text (`328e73f0`, `328e8cd0`), the group show/hide (`328da3c0` over tree `+0x18`) and the video popup `DLG_UI_Command_Desc_Video.lua`
  (`328e6cd0`, tree `+0x30`; already in data034). Cases 29069-29074 (`328ecb05` .. `328ecfbf`). Check the `.avi` files it names exist in March's data; if not, stage June's.
- Replace S7b's `/// S7e:` markers in `X2UISkillTreeNew2015.cpp` (SetShow, CreateSkillTree, UpdateJuneTabUI, OnMouseWheel, UICustomEventProc).

**Play-test:** open the tree on two classes: the major-skill icons and their hover text; the command tab's lists and a video popup.
