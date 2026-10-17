# Skill DLG census, June vs March (S0b, read-only)

Name filter: case-insensitive 'skill' and prefix DLG_/UI_. Kom contents: June via tools/junekom.py list (all June koms), March via kom_contents/*_kom dirs.

June: 51 .lua (all in june_src_fixed), plus 178 .tga / 175 .tet / 1 .dds assets. March: 39 .lua, plus 113 .tga / 110 .tet / 1 .dds.
March has no skill DLG .lua that June lacks. DLG_Guide_Skill_Popup.lua is listed in June data001.kom and also in a June data001_packed.kom (second June archive in the data dir; March has only data001_kom). DLG_UI_Skill_Desc_Video.lua is referenced by March C++ (X2UISkillTreeNew.cpp:3625) only under #ifdef MOVIE_TEST, and the file is not in March contents.

| name | June kom | March kom | in june_src_fixed | in March | March C++ loader (first 3 hits, file:line) |
|---|---|---|---|---|---|
| DLG_Command_Skill.lua | data034 | data034 | yes | yes | X2Lib/X2UIComboTree.cpp:299 |
| DLG_Game_Skill_Slot.lua | data034 | data034 | yes | yes | X2Lib/X2Game.cpp:443 |
| DLG_GAME_SKILL_SLOT_NEW.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTree.cpp:752, X2Lib/X2UISkillTreeNew.cpp:1003 |
| DLG_GAME_SKILL_SLOT_NEW_10slot.lua | data034 | not found | yes | no | - |
| DLG_GAME_SKILL_SLOT_NEW_EVENT.lua | data034 | not found | yes | no | - |
| DLG_Get_New_Skill_or_Memo.lua | data217 | not found | yes | no | - |
| DLG_Guide_Skill_Popup.lua | data001,data001_packed | data001 | yes | yes | none found |
| DLG_Guild_Skill_Icon.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1784 |
| DLG_Guild_Skill_Info_Icon.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTreeInfo.cpp:1140 |
| DLG_Help_Skill.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTree.cpp:1424, X2Lib/X2UISkillTreeNew.cpp:2098 |
| DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua | data034 | not found | yes | no | - |
| DLG_Note_Of_Skill.lua | data034 | data034 | yes | yes | X2Lib/X2UIManager.cpp:3811 |
| DLG_Note_Of_Skill_New.lua | data034 | data034 | yes | yes | X2Lib/X2UIManager.cpp:3809 |
| DLG_Reset_Skill_Window.lua | data034 | data034 | yes | yes | none found |
| DLG_RIDING_SKILL_SLOT.lua | data034 | data034 | yes | yes | X2Lib/X2RidingPetSkillSlot.cpp:16 |
| DLG_Skill_Acquisition.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:2791, X2Lib/X2UIGuildSkillTreeInfo.cpp:1882, X2Lib/X2UISkillTree.cpp:3884 |
| DLG_Skill_Help.lua | datai014 | datai014 | yes | yes | none found |
| DLG_Skill_Icon.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTree.cpp:2708 |
| DLG_Skill_Info_Popup.lua | data034 | data034 | yes | yes | none found |
| DLG_Skill_Purchase_Window.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:2066, X2Lib/X2UIGuildSkillTree.cpp:2070, X2Lib/X2UIGuildSkillTreeInfo.cpp:1420 |
| DLG_Skill_Tree_Arrow_HeadTail.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1926, X2Lib/X2UIGuildSkillTreeInfo.cpp:1280, X2Lib/X2UISkillTree.cpp:2874 |
| DLG_Skill_Tree_Arrow_HeadTail_Left.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1972, X2Lib/X2UIGuildSkillTreeInfo.cpp:1326, X2Lib/X2UISkillTree.cpp:2920 |
| DLG_Skill_Tree_Arrow_HeadTail_Right.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1968, X2Lib/X2UIGuildSkillTreeInfo.cpp:1322, X2Lib/X2UISkillTree.cpp:2916 |
| DLG_Skill_Tree_Arrow_Line.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1948, X2Lib/X2UIGuildSkillTreeInfo.cpp:1302, X2Lib/X2UISkillTree.cpp:2896 |
| DLG_Skill_Tree_Arrow_Line_Horizonal.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1996, X2Lib/X2UIGuildSkillTreeInfo.cpp:1350, X2Lib/X2UISkillTree.cpp:2944 |
| DLG_Skill_Tree_Main.lua | data034 | data034 | yes | yes | none found |
| DLG_SkillCutIn_SocketSelect.lua | data034 | not found | yes | no | - |
| DLG_SkillCutIn_SocketSelect_Control.lua | data034 | not found | yes | no | - |
| DLG_SkillTree_Back.lua | data034 | data034 | yes | yes | none found |
| DLG_SkillTree_Front.lua | data034 | data034 | yes | yes | none found |
| DLG_SkillTree_Navi.lua | data034 | data034 | yes | yes | none found |
| DLG_SkillTree_Skill_Slot.lua | data034 | data034 | yes | yes | none found |
| DLG_SkillTree_Study_Window.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Erda_Skill_Interface.lua | data034 | not found | yes | no | - |
| DLG_UI_Guide_Skill.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Guide_Skill02.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Guide_Skill_Note01.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Guide_Skill_Note02.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Guild_Skill_Tree.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:1011, X2Lib/X2UIGuildSkillTree.cpp:1044, X2Lib/X2UIGuildSkillTreeInfo.cpp:617 |
| DLG_UI_Guild_Skill_Tree_Info.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTreeInfo.cpp:654 |
| DLG_UI_Guild_Skill_Tree_New.lua | data034 | not found | yes | no | - |
| DLG_UI_Guild_Skill_Tree_New_2.lua | data217 | not found | yes | no | - |
| DLG_UI_Skill_Choice.lua | data034 | data034 | yes | yes | none found |
| DLG_UI_Skill_Desc.lua | data034 | data034 | yes | yes | X2Lib/X2UIGuildSkillTree.cpp:2127, X2Lib/X2UISkillTreeNew.cpp:3627 |
| DLG_UI_Skill_Desc_Video.lua | data034 | not found | yes | no | X2Lib/X2UISkillTreeNew.cpp:3625 |
| DLG_UI_Skill_Icon_New.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTreeNew.cpp:3109 |
| DLG_UI_Skill_Icon_New2015.lua | data034 | not found | yes | no | - |
| DLG_UI_Skill_Tree.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTree.cpp:1518 |
| DLG_UI_Skill_Tree_New.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTreeNew.cpp:2187 |
| DLG_UI_Skill_Tree_New2015.lua | data034 | not found | yes | no | - |
| DLG_UI_Skill_Tree_Portrait.lua | data034 | data034 | yes | yes | X2Lib/X2UISkillTreeNew.cpp:5028 |

June-only .lua (not in March): DLG_GAME_SKILL_SLOT_NEW_10slot.lua, DLG_GAME_SKILL_SLOT_NEW_EVENT.lua, DLG_Get_New_Skill_or_Memo.lua, DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua, DLG_SkillCutIn_SocketSelect.lua, DLG_SkillCutIn_SocketSelect_Control.lua, DLG_UI_Erda_Skill_Interface.lua, DLG_UI_Guild_Skill_Tree_New.lua, DLG_UI_Guild_Skill_Tree_New_2.lua, DLG_UI_Skill_Desc_Video.lua, DLG_UI_Skill_Icon_New2015.lua, DLG_UI_Skill_Tree_New2015.lua

March .lua with no C++ loader hit in X2Lib/KTDXLIB/X2 (by filename text): DLG_Guide_Skill_Popup.lua, DLG_Reset_Skill_Window.lua, DLG_SkillTree_Back.lua, DLG_SkillTree_Front.lua, DLG_SkillTree_Navi.lua, DLG_SkillTree_Skill_Slot.lua, DLG_SkillTree_Study_Window.lua, DLG_Skill_Help.lua, DLG_Skill_Info_Popup.lua, DLG_Skill_Tree_Main.lua, DLG_UI_Guide_Skill.lua, DLG_UI_Guide_Skill02.lua, DLG_UI_Guide_Skill_Note01.lua, DLG_UI_Guide_Skill_Note02.lua, DLG_UI_Skill_Choice.lua

## Slot dialog control diff (names from SetName; March from bytecode strings)
March DLG_GAME_SKILL_SLOT_NEW (data034, bytecode): Static_SKILL_SLOT_A/B, SlotA1-4, SlotB1-4, SkillHotKey, SkillHotKeyEx, Static_Skill_Cool_Time(+_B, _Fade, _Fade_B), Static_EL_SKILL_B(+_B), Static_EL_SKILL_R(+_R_B), LOCK, Button_Buy_Guide_Slot, g_pStatic_Slot_Guide. It does not SetName the dialog itself.
June DLG_GAME_SKILL_SLOT_NEW (same name, data034): 47 names; new vs March: Static_RoseGlow(+_B), Static_EveRecoveryGuide/Cooltime, Button_EveRecoveryToolTip, Static_LucielSwitchingGuide, g_pStatic_LucielCooltime, Static_Memorize_Skill_Slot_TypeA1-3/TypeB1-3, Memorize_Slot1-3_TypeA/TypeB, UseMemorizedSkillHotKey_A/B, Static_Install_Skill, Static_GCore_Skill, and SetName("DLG_GAME_SKILL_SLOT_NEW").
June DLG_GAME_SKILL_SLOT_NEW_10slot: 57 names = June NEW plus SlotA5, SlotB5, Button_Buy_Guide_Slot_A_TC/B_TC, LOCK_SLOT_A_TC/B_TC, Static_AinDisparateForceGuide, g_pStatic_AinCooltime, Button_AinDisparateForce, Button_V_Key_ToolTip. (No March C++ reference to 10slot or SlotA5/SlotB5.)
March C++ asks for Static_Skill_Cool_Time (X2UISkillTreeNew.cpp:4642) and SlotA1.. (X2UISkillTree.cpp:2015+).

## Tree dialogs
March: DLG_UI_Skill_Tree_New.lua (X2UISkillTreeNew.cpp:2187): g_pStaticSkill, g_pStatic(_)HorizonLine1/2, SkillTreeControlList, CommandSkill, InitializeSkill, Help, _exit, Cancel, Decide, g_pStaticText, g_pStaticSPBox, Button_Notice_Notice_SP, g_pStaticBlind, Notice, Skill_Page1-3, Add_Skill_Page_Cash, Decide_Skill_Page.
June DLG_UI_Skill_Tree_New.lua (data034, same name): same 20 plus Event_Skill_Page (STUCM_SKILL_EVENT_PAGE).
June DLG_UI_Skill_Tree_New2015.lua (data034, June-only variant, 20 names): SkillTree_Back_White/Black, SkillTree_MajorSkill, Skill_Description, SkillTreeControlList, Help_String, Skill_Page1-3, Add_Skill_Page_Cash, Command_Back_Black, Command_Description, SkillTree_MajorCommand, CommandControlList, g_pStaticText, Skill_Tab, Command_Tab, SkillTree_Character, Help, exit. Paired with DLG_UI_Skill_Icon_New2015.
Other June trees (also in March, no loader hit): DLG_Skill_Tree_Main (1 name) + DLG_SkillTree_Back/Front(55 names)/Navi/Skill_Slot/Study_Window, DLG_Reset_Skill_Window, DLG_UI_Skill_Choice (all use SKILL_TREE_UI_CUSTOM_UI_MSG; the table exists in March Enum.lua).

## June-only widget/binding names
Methods: all 100 distinct :Method names used by the 51 June skill DLGs appear as quoted strings in March X2Lib/KTDXLIB C++ (bindings in KTDXApp.cpp), including CreateNavigation, CreateControlList, CreateRadioButton, CreateSlider, CreateCheckBox, CreateListBox, SetCloseCustomUIEventID, SetGuideDescByString, SetTex2. Free functions: only D3DXCOLOR/D3DXVECTOR2/D3DXVECTOR3 (present). No June-only widget type or method found.
Enum tables/members missing from March C++ and March data036 Enum.lua (all other referenced ones exist):
- tables: PVE_GAME_UI_CUSTOM_MSG, UI_TOOLTIP_TYPE (June Enum.lua has both; skill DLG users: 10slot, Erda_Skill_Interface)
- SKILL_TREE_UI_CUSTOM_MSG: STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN (NEW, NEW_10slot, NEW_EVENT), STUCM_SKILL_EVENT_PAGE (Tree_New), STUCM_SELECT_SKILL_TAB / STUCM_SELECT_COMMAND_TAB (Tree_New2015), STUCM_SELECT_SKILL_LEFT_MEMO / RIGHT_MEMO (Icon_New2015), STUCM_TRANSFORM_SKILL_SLOT_MOUSEIN / MOUSEOUT (LUCIEL_TRANSFORM_SKILL_SLOT) - in June Enum.lua; STUCM_RESET_LEFT_SELECT_SKILL / RIGHT / SELECT_SKILL_CLOSE / SELECT_SKILL_DECIDE (UI_Skill_Choice) - not in June Enum.lua either (June C++ side)
- STATE_MENU_UI_CUSTOM_MSG: SMUCM_DISABLED_TRANSCENDENTAL_SKILL_SLOT_MOUSE_UP, SMUCM_DISABLED_TRANSCENDENTAL_EXPAND_SKILL_SLOT_MOUSE_UP (10slot)
- GAGE_UI_CUSTOM_MSG: STUCM_CHARACTER_SYSTEM_TOOLTIP_ON/OFF (10slot)
- STATE_UI_CUSTOM_MSG: SUCM_CUSTOM_ITEM_SOCKET_SELECT_APPLY/CLOSE/UPGRADE (SkillCutIn_SocketSelect)
Caveat: member check is a text search of identifiers in C++ and March Enum.lua; March Enum.lua is the live merged file, and bytecode string scans give names but not structure.
