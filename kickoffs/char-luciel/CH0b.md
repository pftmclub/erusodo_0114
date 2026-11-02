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

## Amended by CH0

(empty until CH0 runs)

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
