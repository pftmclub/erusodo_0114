# June character Lu/Ciel (Stage 8) - Phase CH0b - Every June Lu/Ciel asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH0b` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH0 (may run before char-chung Z: assets only) |
| Flag | none (assets; `_ASSET` only if a June asset breaks a March reader) |
| Log tag | `[JUNE_CHAR]` (ErrorLog diff) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | none (user play-test: select-screen pass) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Lu/Ciel asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH0b - Done (2026-10-31)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Stage `june2017_re/char_luciel/ch0b_stage_list.tsv` byte-exact with `kom.py stage-june`, pack each touched kom, and **ask the user before each
`/x2-kom-packed copy`**. Record each replaced March file (the 4 same-name motions and anything else) in the Done note and `MODS.md`.
Gate: `python tools/skillicons.py check` over Lu/Ciel's skill ids = PASS, with the shared icon sheets included. Hand-off: a select-screen pass of all
nine March characters and an ErrorLog diff (no new missing-file line). Lu/Ciel himself cannot be seen until CH1.

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

## Amended by CH0 (2026-11-02)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ad L745 L751-755 stage June Motion_<Class>* files with the class script.
- **Stage list `june2017_re/char_luciel/ch0b_stage_list.tsv`: 14,609 files** (14,607 missing, 2 replaced: `DLG_SKILL_Buff_01.tga` / `.tet`, NO_PIECE). `kom.py stage-june` dry run: 37 koms, 0 refused; **4 new kom dirs** with no live archive (`data162` 331, `data163` 294, `datai028` 250, `datai029` 245): pack them and ask before each copy. Routes a / b / c unioned in `ch0_assets.tsv` (17,227 rows; builder `python june2017_re/char_eve/ch0_build_assets.py luciel --out june2017_re/char_luciel`, always with `--out`).
- **Motions: 127, not 125** (data208 Lu 63, data218 Ciel 64; 101 `*Emotion*`). The 4 same-name files (`Motion_CSM_Lu_LDB.x`, `Motion_Lu_LDB_Evil.x`, `Motion_CSM_Ciel_LDN.X`, `Motion_Ciel_LDN_Evil.X`) are already June's bytes in the live data208 / data218: nothing to replace. March readers: `CSM_Lu_Diabla.lua` / `CSM_Ciel_Demonio.lua` (data037 monster cards) and `GameMajorXMeshPlayer.txt` (data036, the `_Evil` meshes).
- **Skill icons:** `skillicons.py check 10000-10601` and `check 30001-30606` FAIL until the 12 sheets are copied (11 NO_TEXTURE: `DLG_Luciel_SKILL_ScreenShot_01-03`, `DLG_SKILL_LUCIEL_Active_01-05`, shared `DLG_SKILL_Passive_09 / _11 / _15`; `DLG_SKILL_Buff_01` NO_PIECE): all in the list (data063). Gate: both checks PASS after the copy.
- `POPUP_TR_NOBLESSE`, `POPUP_TR_D_LORD`, `POPUP_TR_DEMONIO` (+ `.tet`, data215) are in the list. The `HQ_Cutin_{LLD,LCN,LDN}_{LU,CIEL}_Transcendence` cut-ins are already June bytes.
- **Lua is not in the stage list** (June `0x82` bytecode goes through `adopt-june` by its owner): `Luciel_Duo` / `Luciel_Demonio` and `DLG_LUCIEL_BOUNDING_KEY.lua` CH1; `DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua`, `DLG_Luciel_CombinationPoint.lua` CH5; `DLG_Class_Change_Luciel_*` (6) and the 4 guide dialogs CHN; `DLG_Unit_Create_ClassChange_Button_Luciel` NU4; `Tutorial_Luciel_Stage0` (+ `_LineMap`) TU.
- Effect closure `ch0_effect_closure.tsv`: 13,049 rows, 3,815 names, 572 missing (557 in the list; `Ciel_Claw04.ogg`, `Luciel_Shadow_Fury04.ogg`, `WhiteImage.tga`, `__Motion_HAURES2_Ciel_LDN.X` are in no June kom; 15 templets June never defines, e.g. `LDN_Bounding_Lu_StoneFloor`): nothing to stage for those.
- About 14k of the rows are Lu/Ciel shop / costume meshes and `VoicePack_Lu_*`: the character's June assets go in whole (user rule), so they stay.

## Amended by NU0

NU0 (2026-11-02): the creation / select screen and tutorial assets (`june2017_re/char_luciel/nu0_create.tsv` `asset_names` / `asset_in_march`,
`nu0_jobchange_tutorial.tsv`), whole, through `kom.py stage-june`:
- **New:** `DLG_Unit_Background_Luciel.tga` (data051); `DLG_Unit_Create_ClassChange_Button_Luciel.lua` (data034, `adopt-june` + `luabind.py
  check`); `DLG_UI_Common_Texture_GUI_character_info_10.tga` / `.TET` (data057); `DLG_UI_Button_NEW_GUI_character_bt_03.TGA` / `.TET` (data215);
  `LucielVoice_Introduce`, `_Create01`, `_Create02`, `_Character_Select`, `_GameStart`, `_Character_Select_Nobless` `.ogg` (data162);
  `Motion_Lu_Common.x`, `Motion_Lu_Lobby.x` (data208), `Motion_Ciel_Common.x`, `Motion_Ciel_Lobby.x` (data218); `Mesh_Lu_Basic_*` + face `.XET`,
  `Mesh_Lu_Cash_*`, `Mesh_LU_Cash_AC_Lowbody` (data158); `Mesh_Ciel_Basic_*` + face `.XET`, `Mesh_Ciel_Cash_*` (data159); `SelectVoiceTable.lua`
  (data036, NU4 ports its loader); `Tutorial_Luciel_Stage0.lua`, `_LineMap.lua` (data040); `DLG_Class_Change_Luciel_*` (6, data217) and the 4
  guide dialogs (data034); `HQ_Loading_TUTORIAL_LUCIEL_01/02.tga` (not found in March: find June's kom).
- **Replace with June's** (same name): `bt_01` / `bt_02` `.TGA` / `.TET` (data215; March's TETs lack the Luciel keys), `DLG_UI_Common_Texture67.tga`
  (data057). `DLG_Unit_Create_Unit_Button_New.lua` and `Enum.lua` are core-list: NU4 edits them, not CH0b.
- Already in March: `Movie_Create_Luciel_*.avi`, `Mesh_PC_LuCiel_Stand.X`, `UIMajorXMeshPlayer.txt`, `UIEffectSet.lua`, `UIMajorParticle.txt`, the
  7 aura particle assets, `GUI_FLOW_01`, `GUI_character_01` (+ `_gray`), `AnimationNameTable.lua`.
