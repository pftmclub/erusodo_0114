# June 2017 Scaleform GFx sweep (unpacked_x2.exe, image base 0x31aa0000)

Method: Ghidra HTTP (read endpoints only, no writes) + raw PE byte scans/capstone in the scratchpad (pe.py, rtti.py, xdis.py, ctx.py, strs.py).
Tags: [D] = seen in Ghidra decompile or raw disassembly. [I] = inferred. Ghidra has no function carved at most of the CKTDXApp stubs, so those were read with capstone.
Globals: DAT_337676e8 = g_pKTDXApp; DAT_33775860 = g_pData; DAT_3377585c = g_pMain.

## 1. The GFx UI manager = class CKTDGGFXManager ("GG" is in the RTTI name), at [g_pKTDXApp + 0x1c8]

RTTI [D]: `.?AVCKTDGGGFXManager@@` bases = IntGFXManagerListener, IntDataBindingCallBackListener.
 - primary vtable 33446ebc (COL 3360f5b4): 3 slots, 31c485a0 (dtor), 31c43170, 31c431a0 (trivial rect copy helpers)
 - sub-object vtable 33446ea8 (COL 3360f620, offset 4): 31c48560, 31c48570, 31c45ed0
 - 31c45ed0 = the ActionScript -> C++ callback dispatcher (ExternalInterface-style). Handles: isShippingBuild, isShipping, DataStringTable, RegisterMainUI, NowCounTry, NowClientCountry, NowServiceMode, NowClientMode. [D]
Instance layout facts [D]: +0x3c renderer/HAL wrapper, +0x40 GFx loader, +0x80 IDirect3DStateBlock9 [I: created through device vtable +0xec = CreateStateBlock, used Capture(+0x10)/Apply(+0x14)], +0x98 movie wrapper (main.gfx), +0xd4 frame dt (float seconds), +0x108 "hide GFx" byte, +0x115 "ignore key input" byte (set to 1 while a GFX event scene plays, 0 at its end: 3226b130 / 3226b6c0), +0x118 set to 1 at 31aa20c3 when g_pMain+0x708 == 1.

Creation [D]:
 - 31aa1fc0 (app bootstrap; called through stub chain, RET 0xc): creates g_pKTDXApp via 31c17ed0 (@31aa2057), then g_pMain via 31e46300, then 31be3df0, then 31be4510 (@31aa20d8).
 - 31c17ed0 = CKTDXApp::CKTDXApp. At 31c19677 `push 0x120` + new, ctor 31c45cc0 (@31c196a8, arg [app+4]), result stored `mov [esi+0x1c8], eax` @31c196be. Size 0x120.
 - 31c45cc0 ctor: puts GFx state objects: ZlibSupport (vtable 33446cb8, state id 0x19), plain FileOpener (33446d18, id 8), etc.
 - 31be4510 (CKTDXApp post-init, 31aa20d8): `if ([app+0x1c8]) 31c456e0(gfx, ...)`.

Init [D]:
 - 31c456e0 = CKTDGGFXManager::Init. Calls 31c451d0 (build GFx loader + state bag), 31c465b0 (create movie wrapper -> +0x98), 31c3f3d0(movie, "main.gfx" @33446e80) (load/create movie), then reads viewport via arg vfunc 0xc0 -> 31c3f060, stores size at +0xbc..+0xd0 and timeGetTime at +0x7c. If main.gfx fails, deletes +0x98 and nulls it.
 - 31c451d0 (loader setup): states registered through loader vtable +8: r3dGFXFileOpener (vtable 33446d2c, id 8), AS2Support (33446ccc, id 0x21 -> June UI is ActionScript 2 / CLIK), FxPlayerTextClipboard (33446bd4), a Win32 object from GetDC(0) (31c451d0 @31c45490-ish, id 0x11), an IME manager (id 0x17: calls it with "ime.xml" @33446e78 and "IME.swf" @33446e70 -> 31ab7890), then font config: `31ab8360("fontconfig.font" @33446e60)`, 31c43d00 (parse/load fontconfig), 31c44ff0 + 31d21e10. Finally sets a state flag and registers with 31abe360.
 - Strings: "main.gfx" 33446e80 (xref 31c45710 in 31c456e0), "fontconfig.font" 33446e60 (xref 31c45597 in 31c451d0), "IME.swf" 33446e70 (xref 31c45570), "ime.xml" 33446e78. "fontconfig.txt": NOT FOUND (only "fontconfig.font" exists). "gfxfontlib.swf" 33435c2c is referenced only from 31adeb70 and 31adf7e0 = Scaleform library code beside "$IMECandidateListFont" 33435c14, not from app init. ".gfx" (33446e54) wstring ctor 31c451a0 <- 31e572c0 (used by 31d76b40).

Per-frame [D unless marked]:
 - CKTDXApp entry stubs in 31aa2xxx-31aa4xxx call the CKTDXApp methods: 31aa21fe -> 31be5c40 (OnFrameMove), 31aa3f66 -> 31be3800 (frame render), 31aa23a9 -> 31be3f20 (MsgProc), 31aa2160 -> 31be6510 (OnResetDevice), 31aa253c -> 31be66b0 (OnLostDevice), 31aa4b18 -> 31be1aa0 (message loop with PeekMessage), 31aa2793 -> 31be2a00 (viewport/resize).
 - FrameMove: 31be5c40 calls `[app+0x1c8]` 31c430e0 @31be64b0: dt = (timeGetTime - [+0x7c]) / 1000 -> [+0xd4].
 - Render: 31be3800 ends the scene/RT + heat-shimmer composite, then @31be3c56..31be3c68: `ecx=[app+0x1c8]; if (ecx && byte [ecx+0x108]==0) call 31c43b20`. 31c43b20 = Advance+Display: 31c3faf0(movie wrapper, dt=[+0xd4], 2) then StateBlock Capture (+0x80 vtbl 0x10), 31c3f6e0 (display via the +0x3c renderer), StateBlock Apply (+0x14). Exact Advance/Display split inside 31c3faf0/31c3f6e0 is [I].
 - Device lost: 31be66b0 -> @31be6723 31c43440: releases +0x80 state block, then if +0x3c valid and vfunc+4 true, 33213ad0 (HAL release). Reset: 31be6510 -> @31be6625 31c43e60(device): re-creates state block (device vtbl 0xec) and the HAL/viewport (14-dword viewport at +0x44).
 - Viewport/resize: 31be2a00 copies 14 dwords from arg+0x10 -> 31c430c0 (stores to gfx+0x44).

Message/input path [D]:
 - Main loop 31be1aa0: after PeekMessage (@31be1dc3) @31be1e0a `gfx.31c43ab0(hwnd,msg,wp,lp)`. 31c43ab0 = pre-filter: msg 0x100/0x101/0x201/0x202 or a mouse-class msg (32fe46a0) -> 31c435b0 (movie HandleEvent).
 - CKTDXApp::MsgProc 31be3f20: GFx mouse/cursor handler 31c44040 first (@31be3fc3); if it returns 1 and gfx not hidden (+0x108 == 1 test) the msg is swallowed (calls [0x344c0150]); then stage MsgProcs; then keyboard/char/IME handler 31c43890 (@31be406f) which returns 0 when +0x115 == 1, handles 0x100/0x101/0x104/0x105 (31c43480 + 31c432e0), 0x102 (char, 31c430a0), and IME msgs (0x281/0x282/0x286/0x290...). If it returns 1 the msg is swallowed.
 - ESC: CX2State::HandleMsgByESC 31f1e1a0 calls closeToTopZorder 31c43bb0 @31f1e271 (first GFx consumer of ESC).

## 2. File opener [D]
 - Class r3dGFXFileOpener (RTTI 337267a0, vtable 33446d2c, COL 3360f2dc): slot0 31c46ad0, slot1 31c47a00 (OpenFile), slot2 33087f50, slot3 33087e70. Registered as GFx state id 8 in 31c451d0.
 - 31c47a00 (funcmap name KOM_DecodeMemberAlgo2): lower-cases the name, calls KOM_GetMemberFile (31c4c170, on [[g_pKTDXApp+0x1f4]+0x108] = the .kom mass-file manager). Flag bit 2 set -> algo-2 member decode (SHA1 seed + Blowfish ECB, tail bytes plaintext; see funcmap row), then builds an r3dScaleformMemoryFile (RTTI 33726868, ctor 31c46b20); otherwise raw member. Miss -> returns null.
 - No loose-disk fallback seen inside it [I: from decompile; the lookup 31c4c170 itself returns error 0x87 on a miss]. So the GFx files (main.gfx, *.swf, *.xml, fontconfig.font) must be .kom members; the dev_overlay loose-file route would not reach it.
 - KGCMassFileBuffer (RTTI 33726978) is also present in the same region.

## 3. GFx sound [partly D]
 - "Scaleform FMOD Update" 335fae2c: only xref 33221d35 in 33221be0 = creates the FMOD update thread (Scaleform lib code). 33221be0 is called from 33221d80 which is a vtable slot at 335fad30 of the vtable starting 335fac34 (126 slots). That vtable's ctor is 3321e4c0 (stores 335fac34 @3321e4eb), called only from 3321e709 (in 3321e680) <- 33216e20 <- 3321833a / 33218cb3, all Scaleform lib. Nothing in the app's init (31c451d0, 31c45cc0) registers an Audio state or sound renderer: NOT FOUND. [I] The linked FMOD renderer is probably unused; June's own sound path is the KTDS FMOD manager (31ceec30).
 - Imports of fmodex.dll are in the packed section (344c54e7, 344c557d ...), unrelated to GFx.

## 4. Event scenes [D unless marked]
 Two classes live side by side on g_pData:
 - g_pData+0x274 = CX2GFXEventScene (RTTI 337336b8, vtable 33486ee0: 3226d3b0 deleting dtor, 32269e40, 3226c120, 32269e00; object size 0xd8; ctor 3226aed0, created by 31edc620 @325904b9 inside the startup loader area 3258c470/3258d4b9).
 - g_pData+0x278 = CX2EventScene (the March/Lua one, RTTI 337358b0, vtable 334aba28; size 0xfc; ctor 325b79b0 created by 31edc6a0 @3259081b, then 325af4d0 binds Lua global "g_pEventScene" twice).
 - CX2GFXEventScene's own Lua binder 3226a370 (also keys g_pEventScene) has no callers and no data refs: not used [I].
 Data: "GFXEventSceneInfo.xml" 3345c414 (xref 31eed825 in 31eed7a0, called 32590869) and "eventScene.xml" 3345c3f8 (31eecc40/31eed660, called @3259085e). Row fields (strings 3345c320-3345c3dc): EventSceneDataList/EventSceneData/EventScene, questID, swfFileName, frame, soundKey, characterNameKey, textKey, wordBalloonName, SymbolData{symbolName, delayTime}. Per-scene .swf is "%s.swf" (u_%s_swf 33486f48 @3226b3d6/3226b428).
 Start: 3226b130 (CX2GFXEventScene::Play(name, mode, ...)); 3226bff0 queues it if +0x3c is empty; 3226b6c0 = End (unloadUserWidget x2, resets +0x115). 3226b130: if already playing (+4) it queues (3226c8f0); else builds "%s.swf", loads "Tutorial_Back.swf" (id 0x2712) and the scene swf with loadUserWidget 31c45af0(0x2711, name) or, when a start frame is given, loadEventSceneEndFrame 31c457e0 (0x2711, name, frame); sets +0x115 = 1; creates dialog "EventScene_BlackLayOut" (@3226b5f8, 33486ef0); at end unloadWidget(3).
 loadEventSceneEndFrame 33446e8c: xref 31c45919 in 31c457e0; its one caller is 3226b502 inside 3226b130.
 Callers of the GFX scene (sites that load g_pData+0x274): 32 sites, e.g. Lua PlayEventScene binding 327ccad0 (@327ccb0c -> 3226b130; the "PlayEventScene" Lua name is registered in 31d95080 with function ptr 327ccad0 @31de318b), quest UI 323cb2e0 (5 callers 31d8942f, 323e1bf0 (CheckBox_QuickQuest), 323e4cb0 (QuestAccept.ogg), 323e57e0, 323e5c10; it calls 3226ac80 lookup + 3226b130), 31d88c41 (DungeonClearMovie end), NPC info 327d2176 (3226a260), 322fe9ed, 31feeb4d, 3260cba6 ... The generic "is a scene playing" test 3226a010: `+4 flag || g_pData[+0x278]->325ae1e0()` (GFX flag OR Lua scene), called from 31d71829, 323ae733, 324d16b3, 324ed597, 32ab4500, 32e90645, 32f2b8f7.
 Lua CX2EventScene still runs: loader 325b6280 (SOUND_PLAY via 32078fd0) and dialogs DLG_EventScene.lua (334aba78 @325b7bd0), DLG_EventScene_BlackLayOut.lua (334aba38 @325b7c9e), "EventSceneLoadFail m_iEventSceneID = " (334ab9d8 @325b74b1); startup loader loads "EventScene.lua / EventSceneObject.lua" (xorstring 32590230). Its +0x278 users: 11 sites (31d88c6f/87, 31d8e59e, 31f0ce4d, 321ab1ff, 3226a017, 3226a1ec, 323d5b41, 32700a5b, 32deaf16, 32e2e722). I did NOT find a June caller that starts a CX2EventScene scene [not found / inferred: kept for old Lua scripts and end/cleanup].
 What decides: no runtime switch found. Entry points choose: Lua PlayEventScene and the quest UI go to the GFX scene; the Lua scene API (AddEventScene_LUA etc., 31d95c66) stays on CX2EventScene. [I]
 AS-side names for the scene adapter: "EventScene.nextScene", "EventScene.bExactFit" (33531f80/98), Lua-visible: playedEventSceneAndGetMent, getEventSceneData, playEventSceneSound, endEventScene (33531f04-33531f48), gameDataEventScene, arrEventSceneData.

## 5. Cut-ins
 - "CUT_IN_TALK_" 33456860 (xref 31d76d61) and "Common_CutIn_Frame" 33456870 (xref 31d76ba8) both in 31d76b40 (one caller 31fd9834). 31d76b40(this=UI manager with +0xd7c, uiId, ...) : when uiId == 0xc it builds the widget name "Common_CutIn_Frame" (+ ".gfx" ext helper 31e572c0) or "CUT_IN_TALK_<n>" and calls 31fc75e0(0xc, name, a, b) = SimpleWidgetPlayer.playSimpleWidget ("SimpleWidgetPlayer.playSimpleWidget" 33471d7c, clear = 33471d50 / 33471db3). So talk cut-ins are a GFx widget. [D]
 - "HQ_CutIn_Talk_L_/R_" 334b9f00/334b9f5c, "CutIn_Talk_Left/Right", "CutIn_Left/Right" are in 32772080 (one caller 32783f84), with ".dds": texture-based talk portraits. [D]
 - "HQ_CutIn_<Unit>_*" (334d45f4 ... 334f7ef0) are per-unit cut-in texture names (data). Skill cut-ins traced all go through textures: ShowHyperSkillCutInByScript 32014fc0 -> SkillCutIn(wstring) 32078b70 (512x512). GFx only touches skill cut-ins through the gate isOpenUI(0xb) in CX2GUUser::GetShowActiveSkillShow 31fd3180 (@31fd323a) [D, funcmap]. "GFX_SKILL_CUT / _FOR_MINOR / _FOR_DUO" are parsed in CX2CustomizeManager::OpenScriptFile 326525d0 (@326579dd/32657b41/32657d3c); I did not find the function that consumes them: NOT FOUND. No XOR-encrypted spelling of these strings was needed (all were plain).

## 6. Other GFx consumers
 31c3f150 = Scaleform GFx::Value::Invoke-style wrapper (this+0x68 object interface, +0x6c type, +0x70 data) [D]. All 14 direct callers are inside the manager itself (31c43150, 31c43bb0, 31c44530, 31c445d0, 31c44670, 31c44710, 31c44860, 31c44940, 31c44a20, 31c44ae0, 31c44bc0, 31c44ca0, 31c457e0, 31c45af0) = 14 distinct AS names: clearFocus, closeToTopZorder, openUI, unloadWidget, unloadUserWidget, isOpenUI, moveToTopZorder, changeCharacter, showOkPopup, ShowUI, HideUI, isVisibleI(UI) [string is "isVisibleI" at 33446c9c], loadEventSceneEndFrame, loadUserWidget. [D]
 External callers of those wrappers (E8 only, 44 call-site groups; ids are the pushed arg):
   openUI x12: ids 0xe @31d70c11, 3 @31d8f359/32477372/3249ca80/32523e56, 0xb @32228478/3223dd01, 7 @3239d441/3239d6d7, 0xa @324b1081, 8 @324cf4e2, 9 @3252a0c7
   unloadWidget x15: ids 0xe, 8, 3, 0xb, 7, 0xa, 9
   isOpenUI x19: ids 3, 8, 0xb, 0xa, 9
   moveToTopZorder x1 (id 6 @31fc4075), ShowUI(6) @3223d6c3 and HideUI(6) @3223d6dc both in CX2CashShop::SetOpen 3223d680, showOkPopup x1 @3284efbd, closeToTopZorder x1 @31f1e271, unloadUserWidget x2 (3226b6c0), loadUserWidget x2 + loadEventSceneEndFrame x1 (3226b130).
   Caller groups (approx function start, funcmap name where it has one): 31d70c00, 31d70c20, 31d8f1d3, 31ea2b70, 31f06e60, 31f1e1a0 (CX2State::HandleMsgByESC), 31fc4060, 31fc413c, 31fd3180 (GetShowActiveSkillShow), 32228460, 32228480, 3223d680 (CX2CashShop::SetOpen), 3223d6f0, 3223dde3, 3226b404, 3226b610, 3226b6c0, 3239d3ae, 3239d66e, 324659a0, 32465e88, 324668ba, 32470ad9, 324772a4, 3249c9d0, 324ac1d0, 324ac1f0, 324ac26a, 324b106c, 324b6460, 324c8c20, 324cf3d0 (dungeon/training vslot 0xd4), 324cf4bf, 324cf536, 32507d40, 325089b0, 3251d440, 3251d460, 3251d4c3, 3251d531, 32523dc0, 32529f90, 3284ef97, 32935651.
 Beyond the manager: the rest of the UI uses Value/data-binding paths, not 31c3f150:
   - SimpleWidgetPlayer: 31fc75e0 (3 callers in 31d76b40).
   - 13 Adapter/CallbackHandler pairs (RTTI): CLIKDataBinding, CX2SimpleWidgetPlayer, CX2GFXCommonEvent, CX2GFXBingo, CX2TutorialGuide, CX2UICashShop, CX2GFXEventScene, CX2UIProfession, CX2UIPvEResult, CX2UIDungeonResult, CX2UIPvPResult, CX2StateOption, CX2StateMenu; plus ~35 DW_* data-wrapper classes (funcmap says 39) and CX2GFXCashShop slots, CX2UINewUserEvent, CX2TalkBoxManagerUI, CX2UIQuestNew, etc.
   - AS "scope.member" path strings (242 in 33470000-33570000): cashShop 172 (updatePreviewWishItem, showSearchResult, ... preEquip*), GameResult 20, bingo 11, option 5 (updateSound, updateRecord, updateGraphic ...), SimpleWidgetPlayer 3, tutorialGuide 2 (showGuide/hideGuide), EventScene 2, UIProfession 2 + ProfessionalJob 1, UIPvEResult 2, UIDungeonResult 2.
   - UI ids: openUI/isOpenUI ids seen: 3, 6 (ShowUI/HideUI, cash shop), 7, 8 (result UI per funcmap), 9, 0xa, 0xb (checked by skill cut-in gate), 0xc (Common_CutIn_Frame via SimpleWidgetPlayer), 0xe.
