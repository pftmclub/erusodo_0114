# E1a: CX2TFieldNpc talk step / bye step / helpers / UIUnitManager, June 2017 vs March

Read-only RE. Sources: raw capstone disassembly (`funcdis.py --func`), Ghidra decompiles, `c9h5_fieldnpc_onframemove_diff.md` Conventions, `NOTES.md`.
Member names: `this+` = CX2TFieldNpc, `T+` = HouseTemplet (June 0x618). "March" = `X2Lib/X2TFieldNpc.cpp` / `X2UIUnitManager.cpp`.

Globals (June): `0x33775860` g_pData, `0x3377585c` g_pMain, `0x33775864` g_pInstanceData, `0x337676e8` g_pKTDXApp, `0x33775870` g_pTFieldGame, `0x33775878` g_pChatBox.
g_pData members seen: +0x2cc UIManager, +0x3c UIUnitManager, +0x38 UnitManager, +0x5c PartyManager, +0x26c Messenger, +0x330 WorldMissionManager, +0x344 RelationshipEffectManager, +0x454 GlobalMissionManager.
g_pMain: +0x6a0 PartyUI (lazy getter 31e55230), +0x8c state id, +0x90 NowState, +0x4f8 PostBox, +0x1058 June-only sub-object.

NPC members used here (June offset = March name):
+0x198 m_pHouseTemplet, +0x1a0 m_bPlay, +0x1a1 m_bInArea, +0x1a2 m_bTalk, +0x1a3 m_bBye, +0x1a4 m_bJoinNpc, +0x1a5 m_bOpenQuestReceive,
**+0x1a6 June-only "in conversation"**, +0x1b8/+0x1c4 skin 1 / **June-only skin 2**, +0x1d0 June-only UI file name wstring (copy of T+0x120),
+0x1f4 June-only motion map<int,wstring> (0 NORMAL, 4 TALK, 6 BYE), +0x23c m_fJoinNpcTime, +0x244 m_pDlgNpcMessage, +0x248 m_pUINPC, +0x24c m_iNpcIndex,
+0x274 m_strWaitTalk, +0x428 m_bStaticMesh, +0x5a4 / +0x5a8 June-only (dword each; friendship line index and four flag bytes +0x5a8..+0x5ab), +0x5ae m_bReservedShowObject, +0x5af m_bReservedCanTalkNpc,
+0x5b0 m_NpcHouseID, +0x5fa m_bEnable, +0x5fb m_bEventNpc, +0x5fc m_bCanTalkNpc,
**+0x5d8 JoinAnimSpeed (T+0x180), +0x5dc WaitAnimSpeed (T+0x184), +0x5e0 ByeAnimSpeed (T+0x188), all June-only.**

Ids: 0x2729 = 10025 = NUI_BILLBOARD (March uses NUI_BILLBOARD under SERV_UPGRADE_TRADE_SYSTEM; the NUI_MU branch is the `#else`). 0x6edc = 28380 = STR_ID_28380. 0x8a7 = 2215 (code event, friendship). 0x9c92 = 40082 (AGIT_BANK per c9h5). 0x9c55 = 40021 = HI_POSTBOX (same value in March, X2LocationManager.h enum count).

March `#ifdef` state (grep of `#define`, not preprocessor-proven except where noted): EVENT_NPC_IN_VILLAGE is only a commented-out define (Always_JP.h:151) so the March `HI_EVENT_BENDERS "Wait"` branches are NOT compiled; NPC_SOUND (KTDX.h:243), REFORM_NOVICE_GUIDE (Always.h:2197, so the March NoviceGuide lines are compiled out), QUEST_GUIDE, ADDED_RELATIONSHIP_SYSTEM, REAL_TIME_ELSWORD, CHRISTMAS_TREE, TALK_BOX_VILLAGE_NPC (Always.h); SERV_PVP_NEW_SYSTEM, SERV_PSHOP_AGENCY, SERV_UPGRADE_TRADE_SYSTEM, SERV_INSERT_GLOBAL_SERVER, SERV_EPIC_QUEST, SERV_LOCAL_RANKING_SYSTEM (ServerDefine.h), SERV_GLOBAL_MISSION_MANAGER (ServerDefine_US.h:606). DARKMOON_NPC: no define found (so March's `|| HI_DARKMOON` in SetCanTalkNpc is not compiled).

Shared helper identities (funcmap has no rows for most; names below are structural, marked "(s)"):
- 31f54910 = std::wstring::assign(const wstring&, pos, npos) (s; thiscall ret 12), 31f54a10 = wstring::assign(const wchar_t*, len) (s), 31aa92b0 = wstring operator=(wstring) (s), 32414880 = wstring dtor (funcmap), 32fe2300 = operator delete / free (s), 31c1c1a0 = wstring::compare(pos,n,ptr,n2) (funcmap evidence), 322aa950 = std::map<int,..>::find (funcmap evidence).
- 31d33fd0 = CKTDGXSkinAnim::ChangeAnim(const WCHAR*, bool transition, float start) returns bool (funcmap evidence; March `KTDGXSkinAnim.h:151`), 31d28d00 = CKTDGXSkinAnim::Play (funcmap row, confirmed), `Play(2)` = XAP_ONE_WAIT (March enum LOOP 0, ONE 1, ONE_WAIT 2), 31c8f240 = SetPlaySpeed(float) (funcmap evidence).
- 31fae370 = CKTDGObject manager-chain show test (c9h5), 32154820 = CX2UIManager::SetShowUI(bVal, bForce) (funcmap evidence: "SetShowUI @32154ced"; args here (false,true) in talk, (true,false) in bye).
- 31ce9a60 = CKTDGManager::AddObjectChain (funcmap evidence).
- 322c3d80 = PartyUI::OpenPartyMenu(bool) (s; matches March call order), 322c2930 = PartyUI::OpenPartyDLG(bool) (s), 322b9670 = OpenPartyFeverDLG(bool) (s), 322b97f0 = UpdatePartyFeverDLG (s).

---------------------------------------------------------------------------------------------------

## A. CX2TFieldNpc talk step, June 0x325ccf60..0x325cd7b3 (ret at 325cd7b3)  vs  March X2TFieldNpc.cpp OnFrameMove `if( m_bTalk == true )`, 1044-1147

June outlines March's inline block into a member called from OnFrameMove (c9h5 block 30). Entry: `ecx = this`, no args.

A1. **325ccf60-325ccfb3** prologue; `this+0x1a6 = 1` (June-only "in conversation" flag, set for every NPC including event NPCs, before any gate).
 March: none. **June-only.**

A2. **325ccfb3-325cd189** `if (!m_bEventNpc /*+0x5fb*/)` (jne at 325ccfad skips to 325cd189). March 1046-1068.
 - 325ccfb9-325ccfec: `name = m_motionMap /*+0x1f4*/[4 /*TALK, T+0x520*/]` via 325d2370 (operator[] style lookup), copied into a local wstring (31f54910).
 - 325ccff1-325cd005: `325ca1b0( 0, &name )` = quest-conditional MOTION override (June-only, see C-list). Type 0 = talk.
 - 325cd005-325cd06e skin 1: static mesh (+0x428): `ChangeAnim(name,true,0)`; if it returns false fall back to `map[0 /*NORMAL, T+0x574*/]` and ChangeAnim again. Non-static: `ChangeAnim(name,true,0)` only, no fallback.
 - 325cd06e-325cd0cf: SetPlaySpeed(skin1, `+0x5d8` JoinAnimSpeed) (inlined: skin+0x250 = speed, then skin+0x244 vfunc 0x6c), `Play(2)`.
 - 325cd0cf-325cd164 skin 2 (`+0x1c4`, only if non-null): same ChangeAnim logic, `31c8f240(+0x5d8)`, `Play(2)`.
 - 325cd164 `m_bPlay (+0x1a0) = 1`; 325cd16b-325cd189 free the local string.
 March: `HI_EVENT_BENDERS ? "Wait"` (not compiled, see above) `else if(!m_bStaticMesh) ChangeAnim(L"Talk",true) else ChangeAnim(L"normal",true); Play(XAP_ONE_WAIT); m_bPlay=true;` on `m_pXSkinAnim` only.
 **changed:** the motion name comes from the templet-fed map (key 4) instead of the literal "Talk"; June's static-mesh path tries TALK first and only then NORMAL (March uses "normal" outright); quest-MOTION override added; second skin; JoinAnimSpeed applied via SetPlaySpeed (March sets no speed). No literal strings in this block.

A3. **325cd189-325cd1af** `m_bTalk (+0x1a2) = 0; m_fJoinNpcTime (+0x23c) = 0.0; m_bJoinNpc (+0x1a4) = 1; +0x5a4 = 0; +0x5a8 = 0`.
 March 1070-1073: m_bTalk=false; m_bJoinNpc=true; m_fJoinNpcTime=0.f. **same + June-only (+0x5a4/+0x5a8 clears, friendship line index and latches).**

A4. **325cd1af-325cd218** UI hiding. `32154820(g_pData->UIManager, false, true)` = SetShowUI(false,true); `PartyUI` (g_pMain+0x6a0, lazily created by 31e55230 when null): `322c3d80(false)` OpenPartyMenu(false), `322c2930(false)` OpenPartyDLG(false); then `326662c0(31d70d90(g_pMain)(false))` = June-only (see C-list, Alarm_Button, arg 0).
 March 1081-1088 (SetShowUI(false,true); `#ifdef SERV_PVP_NEW_SYSTEM` OpenPartyMenu(false); OpenPartyDLG(false)). **same** for the first three; Alarm_Button call **June-only**.

A5. **325cd218-325cd2e0** UI unit. `if (m_pUINPC /*+0x248*/ == NULL)`: if !static: `copy = wstring(+0x1d0)` (31f54910), `m_pUINPC = UIUnitManager(g_pData+0x3c)->GetUIUnit(m_iNpcIndex /*+0x24c*/, copy)` = **32806b30( id, wstring by value )**; if static: `m_pUINPC = NULL`. Then if non-null: `unit+0x144 = 1` (SetAlphaObject(true)); SetShowObject(false) inlined (manager-chain test 31fae370, byte +0x172 cleared, vtable +0x28 called when it changed); `AddObjectChain(g_pKTDXApp->DGManager(+0x1fc), m_pUINPC)` (31ce9a60).
 March 1090-1108: same, with `GetUIUnit( m_iNpcIndex )` (1 arg). **changed: second arg = file-name override from NPC +0x1d0.** Everything else same (March's commented SetLayer/StateChange lines are comments).

A6. **325cd2e0-325cd5b6** dialog message. `if (m_pDlgNpcMessage /*+0x244*/ != NULL)` (else jump to A8 at 325cd632).
 - NPC id == 0x2729 (NUI_BILLBOARD): `wstrRemainTime = GET_STRING(28380)` (31c31060 string-table singleton; if its +0x40 flag set it builds the debug form with format L"[S%1%]%2%" at 0x3342f1d0, else `31c30d30(tbl, 0x6edc)`); `exp = g_pInstanceData(0x33775864)->GetAgencyShopExpirationDate()` (31cda490, unnamed); if non-empty: `cur = _time64(0)`, `ConvertStringToCTime` (31c55010 plain parse, funcmap evidence) and if `cur < agencyTime` and g_pMain && g_pMain->NowState(+0x90): `wstrRemainTime = NowState->GetExpirationDataDesc( g_pInstanceData->AgencyShopType(+0x208), exp )` (31f1d9a0, unnamed); `dlg->SetNpcMessageWithEtc(m_strWaitTalk /*+0x274*/, wstrRemainTime)` = 32ec99e0.
 - else: `tmp = copy(+0x274)`; `325ca0b0( 0, &tmp )` = QuestMsgOverride(0 = wait) (June-only, funcmap row "likely"); `dlg->SetNpcMessage(tmp)` = 32ec9830.
 March 1110-1146: same billboard branch (SERV_UPGRADE_TRADE_SYSTEM path, STR_ID_28380) with `SetNpcMessageWithEtc`; else `SetNpcMessage(m_strWaitTalk)`. (March's SERV_PSHOP_AGENCY outer ifdef is on; the `NUI_MU` variant is the dead `#else`.)
 **changed:** the non-billboard branch routes the wait text through QuestMsgOverride(0,...). Billboard branch **same** (June adds the STR-id debug formatting inside GET_STRING).

A7. **325cd5b6-325cd632** friendship gauge (June-only; listed, not traced). If g_pInstanceData: `322aa950( &g_pInstanceData->codeEventMap(+0xa18), key 0x8a7 )` (= IsEnableCode(2215), found && node+0x10 byte == 1) then `mgr = 32696fa0( houseId /*+0x5b0*/, &val, &flag )` (friendship manager singleton; the 3 pushed args are consumed by) `321cd500` (lookup, returns bool); if true: `32eccf60( dlg, val )`; else (or code off, or no instance data) `32ec8d40( dlg, 0 )`. Falls into A8.
 March: none. **June-only.**

A8. **325cd632-325cd752** `if (m_bStaticMesh == 1)` only:
 - `if (m_NpcHouseID == 0x9c92 /*AGIT_BANK*/ && g_pData && g_pData->UIManager) 32158070( uiMgr, 0x1a, true, 0, 0 )` = ToggleUI(June UI id 0x1a, show). 0x1a is June's numbering; bye toggles the same id off (B12).
 - `if (m_pHouseTemplet)`: `JoinTalk = T+0x13c` (vector<wstring>, element 0x1c); if non-empty: `idx = rand() % size; NpcPlaySound( JoinTalk[idx] )` = 325c9c00 (funcmap row: June rewrite of NpcPlaySound).
 March: nothing in the talk block. March plays the JoinTalk sound in OnFrameMove at :756-761 (the "joined" block), for all NPCs. June's OnFrameMove also does that (c9h5 line 48). **June-only: an extra JoinTalk sound at talk time for static-mesh NPCs, plus the agit-bank UI open.**

A9. **325cd752-325cd78c** `g_pKTDXApp->DeviceManager(+0x1f4)->PlaySound( L"Camera_ZoomIn.ogg" /*0x334acc44*/, false, false, 1 )` (31ce0e10, June 4th arg = auto-release); `uiMgr->GetUIQuestNew()->SetTalkingQuestNPC( true, m_iNpcIndex )` = 3214f020 (getter, s) then 323d0c00 (s; args (1, +0x24c) pushed before the getter and consumed by the setter, `ret 8`).
 March 1141-1146: PlaySound(L"Camera_ZoomIn.ogg", false, false); `#ifdef SERV_EPIC_QUEST` SetTalkingQuestNPC(true, npc). **same** (June adds the PlaySound 4th arg).

A-callees: 325d2370 unnamed (map<int,wstring> lookup on +0x1f4), 325ca1b0 unnamed, 31f54910 unnamed, 31f54a10 unnamed, 31d33fd0 unnamed, 31d28d00 CKTDGXSkinAnim::Play, 31c8f240 unnamed, 32154820 unnamed, 31e55230 unnamed, 322c3d80 unnamed, 322c2930 unnamed, 31d70d90 unnamed (`lea eax,[ecx+0x1058]; ret`), 326662c0 unnamed, 32806b30 unnamed, 31fae370 unnamed, 31ce9a60 unnamed (AddObjectChain per funcmap text), 31c31060 unnamed, 31c30d30 unnamed, 31aae590 / 31e71c10 / 31aaba90 / 31aaba70 / 31aadd10 unnamed (debug-prefix format path), 31cda490 unnamed, _time64 (IAT), 31c55010 unnamed (ConvertStringToCTime per funcmap text), 31f1d9a0 unnamed, 31aa92b0 unnamed, 32414880 (funcmap, wstring dtor), 32ec99e0 unnamed, 325ca0b0 (funcmap: quest-conditional NPC message, June-only), 32ec9830 unnamed, 322aa950 unnamed, 32696fa0 (funcmap: friendship manager getter), 321cd500 unnamed, 32eccf60 unnamed, 32ec8d40 unnamed, 32158070 (funcmap: CX2UIManager::ToggleUI, candidate), rand (IAT 0x344c0260), 325c9c00 (funcmap: NpcPlaySound June rewrite), 31ce0e10 (funcmap: DeviceManager::PlaySound 4-arg), 3214f020 unnamed, 323d0c00 unnamed, 32fe2300 unnamed, 32fe245c security cookie.
A-strings: L"Camera_ZoomIn.ogg", L"" (0x3344f1f4), STR 28380 (0x6edc), L"[S%1%]%2%" (0x3342f1d0, debug path only). Code event 2215 (0x8a7). House 0x9c92. NPC 0x2729.
Friendship manager calls (not traced): 32696fa0, 321cd500, 32eccf60, 32ec8d40, IsEnableCode 2215.

**Verdict A: changed** (same skeleton: motion -> flags -> hide UI -> UI unit -> message -> sound -> quest NPC; changes listed in A2, A5, A6; June-only A1, A3 tail, A4 Alarm_Button, A7, A8).

---------------------------------------------------------------------------------------------------

## B. CX2TFieldNpc bye step, June 0x325ca2f0..0x325ca919 (ret)  vs  March `if( m_bBye == true )`, 1150-1317

Note: funcdis lists 325ca2b0 inside this range; **325ca2b0..325ca2ee is a separate tiny function** (`ret 4`, bool arg stored on stack, calls `323ec810(g_pData+0x64, 0xf9, ...)` and `31d73030(g_pMain, 0xfa, 0.0)`; c9h5 names it Handler_EGS_TALK_WITH_NPC_REQ). The bye step starts at **325ca2f0**. Callers: OnFrameMove (c9h5 block 31), the dtor 325cd7c0 (first call after storing the vtable), 325ca920 and 325ca950 (when m_bJoinNpc).

B1. **325ca2f0-325ca331** prologue; **guard `if (this+0x1a6 == 0) return`** (jump to epilogue 325ca8fc); then `this+0x1a6 = 0`.
 March: none. **June-only.** Consequence: the dtor's call and the 325ca920/325ca950 calls do nothing unless the talk step ran; a pending `m_bBye` (+0x1a3) is not cleared by a skipped call.

B2. **325ca337-325ca4d5** `if (!m_bEventNpc)` (March 1152-1171).
 `name = map[6 /*BYE, T+0x558*/]` (325d2370), copied to a local; `325ca1b0( 2, &name )` quest MOTION override, type 2 = bye (June-only); skin 1: non-static -> `ChangeAnim(name,true,0)` only; static -> same, then NORMAL (`map[0]`) fallback if it returns false; `31c8f240(skin1, +0x5e0 ByeAnimSpeed)`; `Play(2)`; skin 2 (+0x1c4) identical; `m_bPlay = 1`.
 March: `!static ? ChangeAnim(L"Bye",true) : ChangeAnim(L"normal",true); Play(XAP_ONE_WAIT); m_bPlay=true;` (+ dead EVENT_NPC_IN_VILLAGE "Wait").
 **changed**, same pattern as A2 (map key 6, static tries BYE then NORMAL, quest override, ByeAnimSpeed, skin 2).

B3. **325ca4d5-325ca4e8** `mov word [this+0x1a3], 0` -> m_bBye = 0 and m_bJoinNpc = 0; `+0x5a4 = 0; +0x5a8 = 0`.
 March 1172-1173: m_bBye=false; m_bJoinNpc=false. **same + June-only clears.**

B4. **325ca4e8-325ca5bb** `#ifdef NPC_SOUND`: if m_pHouseTemplet: `ByeTalk = T+0x14c` vector (0x1c elements); if non-empty: `NpcPlaySound( ByeTalk[rand() % size] )` (325c9c00). March 1175-1190. **same** (NpcPlaySound itself is a June rewrite, see C).

B5. **325ca5bb-325ca5dd** `if (m_pUINPC) { UIUnitManager(g_pData+0x3c)->32806800( m_iNpcIndex ); m_pUINPC = NULL; }`.
 32806800 = **ClearUIUnit(int id)** (map find by id; erase via 32806460 if found). March 1192-1197: `ClearAllUIUnit(); m_pUINPC = NULL;`.
 **changed: erases only this NPC id's cached unit, not the whole map.** (The dtor still calls the clear-all 32806840 at 325cd8f3, same as March's dtor.)

B6. **325ca5dd-325ca603** `if (g_pTFieldGame->NpcShadow(+0x38)) 323a89f0() /*CloseShadow*/`; `if (g_pTFieldGame->TalkBoxManager(+0x20)) +0x198 = 1` (SetShowTalk(true) inlined). March 1199-1203 (+ NoviceGuide SetHide, compiled out by REFORM_NOVICE_GUIDE). **same.**

B7. **325ca603-325ca639** PartyUI `322c3d80(true)` OpenPartyMenu(true) (March `#ifdef SERV_PVP_NEW_SYSTEM`, 1213); then `326662c0(31d70d90(g_pMain))(true)` Alarm_Button call, arg 1. **same + June-only Alarm_Button (arg 1; talk passed 0).**

B8. **325ca639-325ca69f** `if (PartyManager(g_pData+0x5c)->myParty member count >= 2)`: `322b9670(true)` OpenPartyFeverDLG; `322b97f0()` UpdatePartyFeverDLG. March 1214-1218. **same** (count = (end-begin)/0xa0).

B9. **325ca69f-325ca6d3** `32154820(uiMgr, true, false)` SetShowUI(true); then `322c5980(PartyUI)` = **June-only**: switch on `g_pMain->state(+0x8c)` that re-opens/closes PartyMenu/DLG/Fever by state (decompile seen; internals not traced). March: only SetShowUI(true).

B10. **325ca6d3-325ca72d** WorldMission: `mgr = g_pData+0x330; if (*(byte*)mgr == 1) 32e08090( mgr+0x34, true )` (IsActiveDefenceDungeon -> UIWorldMission::SetShowTimeDlg(true)); GlobalMission: `328ad970( g_pData+0x454 ) == 1` (CheckShowGlobalMissionUI) -> `32ed3f90( *mgr, true )` (SetShowGlobalMissionDlg); Relationship: `if (g_pData+0x344) 328a4500( 0 )` (SetShowRelationshipAttachEffect(REST_SHOW_ALL = 0, X2RelationshipManager.h:278)).
 March 1226-1254. **same.**

B11. **325ca72d-325ca770** ChatBox (g_pChatBox 0x33775878): `31e9d880(false)` SetChatBoxLayerUp, `31eb4ca0` OpenChatWindow, `31e9dac0` CloseChatWindowButton, `31ea6ab0` HideChatEditBox; Messenger (g_pData+0x26c) non-null -> `325ec6e0(false)` SetHideWindow. March 1256-1262. **same.**

B12. **325ca770-325ca7c3** `if (m_NpcHouseID == 0x9c55 /*HI_POSTBOX*/ && g_pMain->PostBox(+0x4f8)) 3284b4b0(false)` SetDialogShow(false) (March 1264-1268, **same**); then **June-only**: `if (static && house == 0x9c92 && g_pData && uiMgr) 32158070( uiMgr, 0x1a, false, 0, 0 )` = close the agit-bank UI opened in A8.

B13. **325ca7c3-325ca7d3** `if (m_pDlgNpcMessage) 32ec8de0(false)` = SetShow(false). March 1271-1274. **same.**

B14. **325ca7d3-325ca817** TempInventory: `inv = 3214f110(uiMgr)`; `if (inv && inv->+0xdc > 0) 32158070( uiMgr, 0x1c, true, 0, 0 )` (0x1c = June id of UI_TEMP_INVENTORY by structure). March 1296-1300 (REAL_TIME_ELSWORD) ToggleUI(UI_TEMP_INVENTORY,true). **same** (June id renumbered). March's NoviceGuide block 1279-1293 is compiled out in both.

B15. **325ca817-325ca84b** `PlaySound( L"Camera_ZoomOut.ogg" /*0x334acb94*/, false, false, 1 )`; `uiMgr->GetUIQuestNew()->SetTalkingQuestNPC( false, 0 )` (3214f020 / 323d0c00). March 1303-1307 (default npc arg). **same** (+ PlaySound 4th arg).

B16. **325ca84b-325ca8fc** QUEST_GUIDE: `state = g_pMain+0x8c`; `<0xe` skip; `0xe..0xf` -> `if (g_pTFieldGame && TFG+0x3d8) 3293e830( indicator, true )`; `0x15` -> g_pX2Game (global 0x3372c0fc, shared_ptr style 31aad340/31aad500/31f4c3a0/31f4bb50) `+0xc78` NpcIndicator -> `3293e830(true)`; others skip. March 1308-1317: XS_VILLAGE_MAP / XS_BATTLE_FIELD. **same** (renumbered; June 0xf has no named March twin: unsettled, c9h5 also leaves it).

B-callees: 325d2370, 325ca1b0, 31f54910, 31d33fd0, 31c8f240, 31d28d00 (CKTDGXSkinAnim::Play), 31f54a10, 325c9c00 (NpcPlaySound), 32806800 unnamed (ClearUIUnit(id)), 323a89f0 unnamed (CloseShadow s), 31e55230, 322c3d80, 31d70d90, 326662c0, 328ad970 unnamed, 322b9670, 322b97f0, 32154820, 322c5980 unnamed, 32e08090 unnamed, 32ed3f90 unnamed, 328a4500 unnamed, 31e9d880 / 31eb4ca0 / 31e9dac0 / 31ea6ab0 unnamed, 325ec6e0 unnamed, 3284b4b0 (c9h5: SetDialogShow), 32158070 (ToggleUI), 32ec8de0 unnamed, 3214f110 unnamed (GetUITempInventory s), 31ce0e10 (PlaySound), 3214f020, 323d0c00, 31aad340 / 31aad500 / 31f4c3a0 / 31f4bb50 unnamed (c9h5 names the shared_ptr chain), 3293e830 (c9h5: NpcIndicator SetShow), rand, 32fe2300, 32fe245c.
B-strings: L"Camera_ZoomOut.ogg", L"" (0x3344f1f4). House 0x9c55, 0x9c92. No STR ids.
Friendship-manager calls in bye: none. (Friendship: only +0x5a4/+0x5a8 are zeroed.)

**Verdict B: changed** (guard on +0x1a6, BYE from motion map + quest override + ByeAnimSpeed + skin 2, ClearUIUnit(id) instead of ClearAllUIUnit, Alarm_Button show, 322c5980, agit-bank close; rest same).

---------------------------------------------------------------------------------------------------

## C. Helpers

All are `this = CX2TFieldNpc` thiscall.

**C1. 0x325ca920 (ret, no args)** = "disable NPC": `if (m_bJoinNpc /*+0x1a4*/ == 1) bye(325ca2f0); 325c9130(0) /*SetShowField(false)*/; m_bEnable (+0x5fa) = 0`.
 March: no counterpart as a function; the effect is `SetEnable(false)` (X2TFieldNpc.h:105, `m_bEnable = val`) plus a SetShowField(false). **June-only.** Callers (Ghidra xrefs): 323a9ed0 (remove field NPC by house), 323b0980 (AddFieldNpc when TFG+0x624 && 32ec7230(house)); also c9h5 block 37 (motion queue exhausted).

**C2. 0x325ca950 (ret 4, bool b)** = June's real **SetEnable(bool)**: `if (m_bJoinNpc == 1) bye(325ca2f0); 325c9130(b) /*SetShowField(b)*/; m_bEnable = b; 325c8ce0() /*ResetNpc*/`. Callers: 323b8970 (restore house), 32ec7280.
 March SetEnable = `m_bEnable = val` only. **changed (June much larger).**

**C3. 0x325c9130 (ret 4, bool b)** = **SetShowField(bool)**: `325c9000(b)` (SetShowObject), then for the 8 particle handles `+0x214,+0x218,+0x21c,+0x220,+0x224,+0x228,+0x22c,+0x230` -> `GetInstanceSequence(h)` (31cefdd0 on g_pTFieldGame+0x108) and set its show flag to b (manager-chain test 31fae370, byte +0x172, vtable +0x28 on change); `m_pUINPC` (+0x248) same show set to b; tail-call `m_pDlgNpcMessage->SetShow(b)` (32ec8de0).
 March `SetShowField(bool val)` (X2TFieldNpc.cpp:288-320): `SetShowObject(false)`, the 7 March handles `SetShowObject(false)`, UI NPC hide, dialog hide; **`val` is ignored, always hides.** 
 **changed:** June uses the argument (so `SetShowField(true)` shows); adds handle +0x230 (June-only handle); same call order (object, handles, UINPC, dialog). Callers: 325ca920, 325ca950, 323aa070, 323ac4e0.

**C4. 0x325c9000 (ret 4, bool b)** = **SetShowObject(bool)**: for skin 1 (+0x1b8) and **skin 2 (+0x1c4)**: set byte +0x172 = b with the manager-chain compare (31fae370) and vtable +0x28 call on a change; then `m_bReservedShowObject (+0x5ae) = b`.
 March SetShowObject (2205-2210): skin 1 only, `m_bReservedShowObject = val`. **changed: skin 2 added.** Callers: 323b0980, 325c9130, 323b5970 (x2).

**C5. 0x325c8ce0 (no args)** = **ResetNpc**: `m_bInArea (+0x1a1) = 0; dword [+0x1a2] = 0` (clears m_bTalk, m_bBye, m_bJoinNpc, m_bOpenQuestReceive; m_bPlay +0x1a0 untouched, like March's commented line; **+0x1a6 not cleared**); `if (m_pUINPC) SetShowObject(false)`; `if (m_pDlgNpcMessage) SetShow(false)`; **`+0x5a4 = 0; +0x5a8 = 0`**.
 March ResetNpc (274-286): same minus the June-only +0x5a4/+0x5a8 clears. **same + June-only.** Callers: 325ca950, 323b5970.

**C6. 0x325c8250 (ret 8, bool val, bool bForce)** = **SetCanTalkNpc(val, bForce)** (March 2414-2485):
 - `if (!bForce && m_bCanTalkNpc (+0x5fc) == val) return` (same).
 - house in {0x9c64 (40036 = HI_CHRISTMAS_TREE), 0x9c93 (40083), 0x9ccd (40141), 0x9cce (40142)}: `m_bCanTalkNpc = val; m_bReservedCanTalkNpc (+0x5af) = (bForce == 0); return`. March: CHRISTMAS_TREE (+ DARKMOON_NPC, not compiled) only. **changed: three June houses added; names of 40083/40141/40142 unverified here** (a naive March enum count read 40083 = HI_GUILD_MARK, 40142 = HI_SPA_SIGN_RANOX, ifdefs unresolved).
 - val true: skin 1 `ChangeAnim(L"Active"/*0x334ac910*/,true,0)`, `Play(2)`, `m_bPlay = 1`; skin 2 same (result of ChangeAnim not tested). March: skin 1 only.
 - val false: if a dialog named L"OkAndCancelMsgBox" (0x33457f08, via g_pKTDXApp->DGManager->DialogManager(+0x38c)) exists and is shown -> return (same). skin 1 `ChangeAnim(L"DeActive"/*0x334ac8fc*/,true,0)` and **only if it returned true** `Play(2); m_bPlay = 1`; skin 2 same. March plays unconditionally. Then `if (g_pData && uiMgr && m_bJoinNpc == 1) { 32151680(uiMgr) /*CloseAll*/; m_bBye (+0x1a3) = 1 }`. March also calls ProfileManager->CloseAll under SERV_LOCAL_RANKING_SYSTEM; June has no profile call.
 - tail: `m_bCanTalkNpc = val; m_bReservedCanTalkNpc = (bForce == 0)` (same).
 **changed** (above). Callers: 325ca980 (CreateFieldNpc), 325cdfa0 x3.

**C7. 0x325c8210 (no args, returns bool)** = **GetShowObject**: if skin 1 non-null: skin+0xc manager-chain: (31fae370 && byte +0x172) else byte +0x172; if skin 1 null: `m_bReservedShowObject (+0x5ae)`.
 March (2212-2217): `m_pXSkinAnim->GetShowObject()` else m_bReservedShowObject. **same** (June adds the manager-chain test inside the object getter, as in the other show tests).

Extra helpers named on the way:
- **0x325ca1b0 (ret 8, int type, wstring* inOut)** = quest-conditional MOTION override (June-only; unnamed in funcmap). Loop over `T+0x590` rows (0x48 bytes; the same list 325ca0b0 reads for messages): row+4 == type and row+0 == 0 and wstring at row+0x2c non-empty; `quest = g_pData+0x4c ->32322380(row+8)`; walk the quest's sub-quest list (+0x10..+0x14, 8 bytes each, flag byte +6) to the first one with flag 0; `sub = 32322350(id)`; if `sub+0x20 == 1 && sub+0x60 == m_iNpcIndex` then `*inOut = row+0x2c`. Does not break: the last matching row wins. Types seen: 0 (talk), 2 (bye). Key names (TYPE/STATE/QUEST_ID/SUB_QUEST_ID/MOTION) come from the funcmap row for 325ca0b0, not re-verified here.
- **0x325ca2b0** (separate fn, see B header).
- **0x325c9c00** NpcPlaySound (funcmap, confirmed): StopSound(+0x450), PlaySound(name,0,0,1), copy the device id into +0x450; no 3D branch (March has 3D path, X2TFieldNpc.cpp:2603-2640).
- 31d70d90 `lea eax,[ecx+0x1058]` + **326662c0 (ret 4, bool)**: `h = obj+0x78/+0x7c` (a dialog handle); `if (31d41d90(h) == 0)` (dialog alive) `ctrl = dlg->control( L"Alarm_Button" /*0x334afe88*/ )` and tail-calls `ctrl->vtable[0x60]( b )`. Talk passes 0, bye passes 1: it is a hide/show of the Alarm_Button. Which vtable method 0x60 is in June is **unverified** (March's CKTDGUIControl has SetEnable at 0x5c, GetEnable 0x60, SetShow 0x64; June's layout may differ). March: none. **June-only.**

---------------------------------------------------------------------------------------------------

## D. CX2UIUnitManager (June) vs March X2UIUnitManager.cpp

June object: `map<int, CX2UIUnitPtr>` at this+4 (head sentinel at this+8, size this+0xc); the pointer is intrusive-refcounted (count at unit+8, 31d28b60 = DeleteKTDGObject on 0). A map node's value is at node+0x10 (MSVC10 node: left, parent, right, key +0xc, value +0x10). wstring-by-value is 0x1c bytes.

**D1. 0x32806b30 GetUIUnit( this, int id, wstring fileName BY VALUE ) -> CX2UIUnit***, `ret 0x20`.
 1. `it = map.find(id)` (322aa950).
 2. Copy fileName to a temp (31f54910).
 3. `it == end` -> `CreateUIUnit(id, copy)` (32806910), return its pointer.
 4. else `32ee37a0( cached, id, copy )` returns bool "needs rebuild": **true** -> `map.erase(it)` (32806460) then `CreateUIUnit(id, copy)`; **false** -> return the cached unit (refcount net unchanged).
 5. The by-value argument is destroyed by the callee.
 March GetUIUnit(int) (X2UIUnitManager.cpp:46-60): find; if missing CreateUIUnit(id); else cached; returns `.get()`. **changed: file-name argument and rebuild-on-change.**

**D2. 0x32ee37a0 (this = the CACHED CX2UIUnit, int id UNUSED, wstring fileName by value) -> bool, `ret 0x20`** (June-only; unit is 0x360 bytes):
 - `if (unit+0x35c /*"created with a file-name override"*/ != 0)`: result = `fileName.compare( 0, fileName.size(), unit.+0x190 /*stored file name*/, unit+0x1a0 /*its size*/ )` != 0 (31c1c1a0).
 - else (`+0x35c == 0`): result = `!fileName.empty()`.
 - Returns true when the requested name differs from what the cached unit was built with. Case-sensitive exact compare. `id` is not read.
 Consequences: a unit built from the NPC templet's default (flag 0) is rebuilt as soon as ANY non-empty name is requested, even if that name equals the templet's own file name; a unit built from an override (flag 1) is reused only for the identical name; a request with an EMPTY name on a flag-1 unit compares "" with the non-empty stored name -> differs -> the unit is rebuilt from the templet default.

**D3. 0x32806910 CreateUIUnit( this, CX2UIUnitPtr* out, int id, wstring fileName BY VALUE ) -> out**, `ret 0x24`.
 - `g_pData == NULL` or `g_pData->UnitManager(+0x38) == NULL` or `GetNPCUnitTemplet(id)` (323737a0) NULL -> `*out = NULL`, return (nothing cached; March only returns an empty ptr on a NULL templet, it would crash on NULL g_pData).
 - **fileName.empty()**: unit built from `templet->m_LuaFileNameUI` (wstring at templet+0x5c): `tmp = wstring(c_str)` (31aa1670) -> `32806180(&ptr, &tmp)` = `new CX2UIUnit(0x360)` via ctor 32ee7ab0 (refcount 1). Flag +0x35c stays 0. (March: `CX2UIUnit::CreateUIUnit(templet->m_LuaFileNameUI.c_str())` with an ASSERT that the name is non-empty.)
 - **fileName non-empty**: unit built from the argument instead (`tmp = wstring(fileName.c_str())`), then **`unit+0x35c = 1`**. The templet's name is not consulted. The ctor 32ee7ab0 always stores its name argument into `unit+0x190` (31f54910 at 32ee7bb8) and clears +0x35c (32ee7d08), then calls 32ee57c0 (not traced; presumably loads the Lua).
 - `map[id] = ptr` (operator[] 32807020, old value released), `*out = ptr`.
 March CreateUIUnit(int) (23-39): same map store, no file-name argument. **changed.**

**D4. 0x32806800 ClearUIUnit( this, int id ), `ret 4`**: find, erase if present (32806460). March: none (only `ClearAllUIUnit`, `m_mapUIUnit.clear()`). **June-only.** 0x32806840 = ClearAllUIUnit (map-clear body, same as March); the dtor 325cd7c0 still calls it (325cd8f3), the bye step does not.
 Other GetUIUnit callers all pass L"": 324a58f0 (id 0x2713), 324c2a80 (0x2712), 324c5bd0 (0x2712), 325485f0 (0x2721); the manager's init 32806cf0 creates 0x2710 (NUI_ANNE) with L"". Only the field NPC passes a non-empty name.

### How the UI file name flows
HouseTemplet `T+0x120` (wstr; L"" after HouseParsing, only the relation `Change_UIFileName` row sets it, per NOTES) -> CreateFieldNpc **325ca980** at 325cc7a7 copies it unconditionally to **NPC +0x1d0** (`31f54910(this+0x1d0, T+0x120)`; `m_iNpcIndex +0x24c` is set at 325cba2c from the first element of T+0x214 NPC_ID_LIST) -> talk step A5 (325cd22d-325cd264): if `!m_bStaticMesh && m_pUINPC == NULL` it copies +0x1d0 into a by-value temp and calls `GetUIUnit( m_iNpcIndex, +0x1d0 copy )`. Bye clears with `ClearUIUnit( m_iNpcIndex )`.
- Empty +0x1d0 (the normal case): behaves like March, i.e. the NPC templet's `m_LuaFileNameUI` unit, cached per NPC id and reused.
- Non-empty: unit built from that Lua file, cached under the NPC id with flag +0x35c = 1 and the name at +0x190; a later request with another name (or empty) erases and rebuilds it, so two NPCs sharing an NPC id but different names rebuild each other's unit on every talk (the first NPC's raw `m_pUINPC` is not refcounted, so it would dangle if it were still in conversation; bye normally clears first).

---------------------------------------------------------------------------------------------------

## Unsettled / unverified
- Alarm_Button vtable slot 0x60 identity (assumed SetShow because talk=0 / bye=1; not confirmed). g_pMain+0x1058's owner class unnamed.
- 322c5980 body only skimmed (state switch over 5/9/0xc/0xd/0x12/0x18/0x19, 0xb/0x17, 0xe/0xf/0x15 ...); named "party UI refresh by state" structurally.
- June UI ids 0x1a (agit bank) and 0x1c (temp inventory) are June numbering; 0x1c = UI_TEMP_INVENTORY and 0x1a = agit bank are structural matches only.
- Houses 0x9c93 / 0x9ccd / 0x9cce in SetCanTalkNpc: numeric only.
- Row key names for 325ca1b0 (TYPE/STATE/...) taken from the funcmap row on 325ca0b0, offsets read here; row+0 == 0 condition meaning not identified.
- June state id 0xf in B16 and 3214f020/323d0c00/322aa950-style names are structural, not from funcmap.
- `32ee57c0` (end of CX2UIUnit ctor) not traced: assumed to load the Lua named by +0x190.
- SetPlaySpeed in A is inlined for skin 1 (skin+0x250 = speed, vfunc 0x6c) and called (31c8f240) for skin 2; equal behaviour assumed from the identical field writes.
