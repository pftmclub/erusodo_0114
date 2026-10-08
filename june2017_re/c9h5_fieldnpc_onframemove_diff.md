# June CX2TFieldNpc::OnFrameMove (325cdfa0..325d0e89) vs March X2TFieldNpc.cpp:361-1591

Conventions. VAs come from a raw capstone pass (`onfm_raw.asm`). Ghidra's listing drops shared tails such as 325cef87.
- `Show(seq,b)` = the inlined `SetShowObject`: `31fae370` manager-show test, byte `+0x172`, vtable `+0x28`.
- `Seq(h)` = `31cefdd0` GetInstanceSequence. `Pos` = `31cf1a40` SetPosition. `Tex` = `31cf4680` ChangeTexForce (likely).
- Offsets are `this+` unless marked `T+` (HouseTemplet).
- UI_MENU and X2_STATE are renumbered in June. Each is named by its structural match, as "June n / March m".
- March names fixed by use: +0x198 m_pHouseTemplet, +0x1a0..1a5 m_bPlay/InArea/Talk/Bye/JoinNpc/OpenQuestReceive, +0x1a8 m_iMotionId, +0x1b8/1bc/1c0 skin/mesh/aniXET, +0x214..22c the 7 March handles (InArea, Event, NewQ1, NewQ2, DoQ, CompQ1, CompQ2), +0x23c/240 JoinNpcTime/NpcMarkTime, +0x244 m_pDlgNpcMessage, +0x248 m_pUINPC, +0x24c m_iNpcIndex, +0x270 m_bOpenedShop, +0x274..3e0 talk wstrings (Wait 274, Make 290, Shop 2ac, Quest 2e4, Bye 300, Ranking 31c, Exchange 338, PrivBank 354, GuildAD 370, PShop 38c, Register 3a8, Receive 3c4, ExchNewItem 3e0), +0x3fc m_strHouseName, +0x418 m_bFirstAnim, +0x41c m_vBasePos, +0x428 m_bStaticMesh, +0x429 m_bShowPost, +0x430..448 quest counters (NewRep, NewNorm, Event, Do, CompRep, CompNorm, FairLv), +0x46c m_vecNPCTalkBox, +0x47c/480/482 talk-box timer/enable/num, +0x484..4b0 near-sound members, +0x5ad/5ae/5af m_bLoaded/ReservedShow/ReservedCanTalk, +0x5b0 m_NpcHouseID, +0x5b4/5c0/5cc m_vPos/Rot/Scale, +0x5fa/5fb/5fc m_bEnable/EventNpc/CanTalkNpc.

## Blocks in June order

1. **325cdfa0-325ce00a** prologue. Stores fElapsed into **+0x1f0** (June-only float), then the two timers. March 363-364. **changed: +0x1f0 is new.** Its only reader is a render-option routine at 325d11c0..12xx, which multiplies T+0x5d0.. (RenderOption colour) by it.
2. **325ce00a-325ce07a** load gate. `!m_bLoaded && m_pMesh`. Reads the device state of mesh, aniXET, **+0x1c8 mesh2nd** and **+0x1cc aniXET2nd** (key `NPCMeshName2nd`/`NPCAniTex2nd`), defaulting each to LOADED. March 365-373. **changed: all four must be LOADED.**
3. **325ce080-325ce1d9** skin 1 create: `31d28b60` DeleteKTDGObject, `31c8f1d0` CreateSkinAnim, +0x2b4=0 (SetApplyMotionOffset), `31d32750` SetAnimXSkinMesh(mesh,xet,0), `31d2f3f0` AddModelXSkinMesh, Show(m_bReservedShowObject), `31c8efc0` Move(+0x5b4), `31cfdcd0` RotateDegree(+0x5c0), `31c19c80` Scale(+0x5cc). Before each matrix op, `31c19d90` runs when skin+0xc is set (June matrix accessor, likely). March 374-385. **changed:** AddModelXSkinMesh takes multiTex = **+0x1ec** plus June-only __FILE__/__LINE__ args (527). +0x1ec is filled at 325cb354 = OpenXET(T+0x5b0), the `MultiTex` key in the RenderOption sub-table (likely).
4. **325ce1d9-325ce255** effect set. If the templet exists and `T+0x5e8` (`EFFECT_SET_NAME`) != L"" (`31c7d460`, likely wstring!=), and `g_pData+0x9c` passes: `321e26d0` PlayEffectSet(name,this), handle into **+0x190**, `321d97c0` GetInstance (likely), `321de4f0` SetShowObject(true). March: none. **June-only.**
5. **325ce255-325ce3bb** skin 2. If +0x1c8, rebuild **+0x1c4** with the step-3 sequence (mesh2/xet2, multiTex 0, line 564), using the same reserved show and pos/rot/scale. March: none. **June-only.**
6. **325ce3bb-325ce408** radius = mesh+0x87c * scale.y into +0x15c, m_bLoaded=1, and if m_bReservedCanTalkNpc then `325c8250` SetCanTalkNpc(m_bCanTalkNpc,true). March 386-394. **same.**
7. **325ce40d-325ce4cf** failure path. FAILED(3) on mesh or xet closes them via `31cdd8d0` CloseDevice (guarded on DeviceManager +0x1f4) and deletes skin 1, without nulling +0x1b8, the same as March. A FAILED mesh2/xet2 also closes +0x1c8/+0x1cc and deletes +0x1c4. March 396-402. **changed: second set added.**
8. **325ce4cf-325ce4fe** early return when `!m_pXSkinAnim || (+0x1c8 && !+0x1c4) || !m_bEnable`. March 405-409. **changed: also waits for skin 2.**
9. **325ce4fe-325ce672** event-NPC timer: `31edca50` GetServerCurrentTime, _localtime64_s, T+0x1e8 m_bEventNpc, T+0x1f0/1f4/1f8 month/day/hour, T+0x1fc m_fWaitMin, T+0x200 m_fActiveMin, then SetCanTalkNpc(1/0). March 412-446. **same.** The `% ulActiveStep` has no zero check, also in March.
10. **325ce672-325ce763** `31be3020` GetProj3DPos(m_vBasePos.y+70). InArea Pos at y-40 for **40021 HI_POSTBOX**, otherwise y-37. March 449-465. **same.**
11. **325ce763-325ce7c7** Seq(**+0x230**) Pos(x-60,y+20). +0x230 is created from particle `NpcTalkParticle` at 325c876f. March: none. **June-only.**
12. **325ce7c7** `325c8d60` InitQuestMark (likely, by position). March 468. **same.**
13. **325ce7ce-325ce8b7** house **40129 HI_EVENT_BLESS_OF_SOLACE**: Seq(**+0x234**, particle `acce_highLight_P01`), Pos(m_vPos), `31cf1b30` SetScale(3,3,1) (likely), Show(true). March: none. **June-only.**
14. **325ce8b7-325ce95b** quest-mark gate: `325c8210` GetShowObject and `31d03920` !GetHideDialog (DIALOG_SHOW_TOGGLE). March 470-484. **changed: more exclusions.**
    - March excludes only 40021 POSTBOX.
    - June excludes 40021 POSTBOX, **40082 HI_AGIT_BANK**, m_vPos == (0,0,0), !m_bCanTalkNpc, and **40143-40146** (SPA_INNER_NPC_RUBEN/RANOX, ELYSION_RED/BLUE_TREE).
    - Excluded NPCs jump to block 19. A hidden dialog skips to block 20.
15. **325ce95b-325ce9b9** SERV_EPIC_QUEST completable check. Gated by **+0x239** (June-only bool). Calls `3232fff0` GetCompletableQuest(T+0x214 m_NPCList[0]), then `32505560` destroys the vector. March 490-500. **changed: result discarded, never sets bQuest.** *(C9h-5 check: the block is live. `CX2TFieldGame` `323b5970 @323b72fa` / `@323b7303` writes +0x239 = 1 / 0 on the NPC (EDI carries +0x5b0) right after its quest-info refresh `325c81a0`, which is outside 325c8000-325d3000. The call runs and its vector is thrown away.)*
16. **325ce9b9-325ceaec** quest-mark priority: CompNorm(+0x444)->+0x228, CompRep(+0x440)->+0x22c, FairLv(+0x448)->+0x21c, NewRep(+0x430)->+0x220, Do(+0x43c)->+0x224, then Pos(x+60,y+30) and Show(true). March 504-542. **same.**
17. **325ceaec-325cebb8** event quest: +0x438>0 gives Seq(+0x218) Pos(x-50,y+20) Show. March 544-554. **same.**
18. **325cebb8-325cec2f** quest button. bQuest = `SETG` (m_nNewQuestNormal +0x434 > 0) **only**. If !`321524d0` GetShow(June 5 / March 5 QUEST_RECEIVE) && dlg && !**+0x5a8**: `32ec84e0` GetShopType(1), `32ecd610` SetShopType(1,b,0,0), `32ec90b0` SetKeyEvent. bQuest is written to **+0x238** (June-only). March 556-588. **changed** (C9h-5 confirmed in raw disassembly: `SETG bl` @325cebcd overwrites every earlier bQuest, so this is likely a deliberate June change):
    - completable, quest-mark and event-quest results no longer raise the button;
    - the +0x5a8 gate is new;
    - SetShopType takes 4 args.
19. **325cec31-325ceddd** friendship mark on +0x230 (June-only).
    - Gate: g_pInstanceData && `31e5a860` IsEnableCode(**2215**, unnamed in June Enum) && `32696fa0` friendship-manager singleton (0x610 bytes) `321cd500` lookup(houseID, &val, &flag).
    - Effect: Pos(x-50,y+20). If !flag, Tex(`Npc_Talk.dds`) and Show. Else if `325d1d70` (val >= mgr+0x490), Tex(`Npc_FriendShip_Max.dds`) and Show. Else Show(false).
    - March: none.
20. **325ceddd-325cf25a** in-area display (March 595-678), **changed**. Branch on m_bInArea && GetShowObject:
    - JoinNpc: hides InArea, quest, event **and +0x230**.
    - Else, when !`323bddf0` and (!event || canTalk): `323bddf0` = `T+0x5a1`, key `bOnlyClickable`. The mark timer at 0.5/1.0 s picks `One_Use_Up.dds` for 40021/40028/**40082/40145/40146** and `Npc_Talk_UP.dds` otherwise. **All three timer branches use the Up texture; `*_Down.dds` never appears.** (C9h-5 confirmed: June's exe has no `_Down.dds` talk-mark literal anywhere, and each `_Up` literal has one reference: @325cef88 / @325ceff8.) Then Show(InArea, canTalk), Show(quest/event, true), and **40147 HI_SPA_PPORU** hides InArea again.
    - Else branch: shows InArea only if canTalk && !bOnlyClickable.
21. **325cf25a-325cf2bd** not in area: Seq(+0x214) Show(false). March 679-686. **same.**
22. **325cf2bd-325cf6b6** joined for more than 1.2 s (March 688-767), **changed**. Gate gains **+0x1a6** (June-only "in conversation" bool: set in 325ccf60, cleared in 325ca2f0).
    - m_pUINPC: Show, +0x164=0x17 (SetLayer, XL_LENS_FLARE = March 0x17), `324a7ce0` GetWaitStateID, `31aa9150` StateChange (likely), plus June-only `32ee3860` (unidentified).
    - Dialog: `32ec8de0` SetShow(true). NUI 10025 (NUI_BILLBOARD) takes the agency remaining-time path (STR 28380, `32ec99e0` SetNpcMessageWithEtc(+0x274)); otherwise SetNpcMessage(**`325ca0b0` QuestMsgOverride(0, copy of +0x274)**). Then SetKeyEvent.
    - m_bOpenedShop=0, join sound from T+0x13c via `325c9c00` NpcPlaySound, `325ca2b0` Handler_EGS_TALK_WITH_NPC_REQ.
    - `325ca0b0` = June-only: replaces the text with a `MESSAGE` entry from T+0x590 (TYPE/STATE/QUEST_ID/SUB_QUEST_ID/MESSAGE/MOTION list) when STATE matches and the player's quest condition matches m_iNpcIndex.
23. **325cf6b6-325cf873** postbox 40021 (March 769-816), **same**. UI ids map June 0x1f / March 25 INVEN; June 4,0x13,0x17,0x18,0x24 / March 4,15,19,20,29 (TRADE, CHARINFO, QUEST_NEW, QUEST, PARTY). Callees: `3284b4b0` SetDialogShow, `3214ed80` CloseAllNPCDlg. The send-window test reads `31d41c90(postbox+0x144)+0x6e` (likely).
24. **325cf873-325cfaaa** house **40082 HI_AGIT_BANK**. Copy of block 23 against UIManager+0x5c, using `3214f1b0` getter, `32150800` create and vtable+0x20 SetShow, with **+0x42a** (June-only bool) in place of m_bShowPost. March: none. **June-only.**
25. **325cfaaa-325cfed4** shop-message loop. Gate +0x1a6 && m_pUINPC shown.
    - Each open UI: SetNpcMessage(`325ca0b0`(state, default)) when dlg exists, then m_bOpenedShop=1.
    - SHOP(J6/M6) 3,+0x2ac; MANUF(J7/M7) 4,+0x290; PRIVBANK(J0x11/M13) 5,+0x354; EXCHANGE(J0x10/M12) 6,+0x338; GUILD_BOARD(J0xe/M10) 7,+0x370; PSHOP_BOARD(J0xd/M9) 8,+0x38c.
    - PERSONAL_SHOP(J0x12/M14) agency state +0xdc 4/5/6 -> Register/Register/Receive (no dlg null check, as in March). RANKING(J0xf/M11) +0x31c. QUEST_RECEIVE(5) or QUEST_NEW(J0x17/M19) -> m_bOpenQuestReceive and QuestMsgOverride(1,+0x2e4).
    - March 828-956. **changed: QuestMsgOverride is inserted, and every enum id is renumbered.**
26. **325cfed4-325cff7c** cursor (`g_pMain+0x90`->+0x64). State 0x12 (March XCS_EXCHANGE_NEW_ITEM = 18) -> +0x3e0. **June-only:** cursor state 0x1a (likely gift) && friendship-manager+0x4a4 -> SetNpcMessage(**+0x588**), `32679e10`(0), **+0x5ab**=1; when the cursor leaves 0x1a, +0x274 and +0x5ab=0. March 957-969. **changed.**
27. **325cff7c-325d0204** bye reset. **+0x5a8** forces opened. If opened && !this-frame && !+0x5a8: `32ec8ef0` SetHouseName(+0x3fc), billboard agency or QuestMsgOverride(2,+0x300 Bye), m_bOpenedShop=0. March 971-1021. **changed.**
28. **325d0204-325d02a2** June UIs 9, 0xb, 0xa (no March twin; inside the four ids inserted after MANUFACTURE) -> SetNpcMessage(**+0x4b4** `Upgrade_Talk` / **+0x4d0** `Service_Talk` / **+0x4ec** `Trade_Talk`, via T+0x3cc/3ec/40c, slot-matched), m_bOpenedShop=1. **June-only.** These run after block 27 and never set the local opened flag, so every frame does bye then June message (see quirks).
29. **325d02a2-325d0326** QUEST_GUIDE NpcIndicator SetShow(false) (`3293e830`). June states 14-15 -> g_pTFieldGame+0x3d8; June 21 -> g_pX2Game (shared_ptr via `31aadcb0`/`31f4c3a0`/`31f4bb50`)+0xc78. March 1023-1041: VILLAGE_MAP=14 / BATTLE_FIELD=24 by March's own enum. **same** (renumbered; 15 is likely a June-new village-type state).
30. **325d0326-325d0336** m_bTalk -> `325ccf60`. The March body (1044-1147) is outlined and **changed**:
    - sets +0x1a6;
    - TALK motion from map +0x1f4 key 4, on both skins;
    - SetPlaySpeed(+0x5d8 JoinAnimSpeed);
    - friendship and AGIT_BANK (40082) branches.
31. **325d0336-325d0346** m_bBye -> `325ca2f0`. March 1150-1317, outlined and **changed** (clears +0x1a6, BYE motion key 6, needs its own diff).
32. **325d0346-325d0356** +0x5a8 -> `325c9e70`, the friendship multi-line talk sequencer. It uses +0x5a4 line index, +0x5a9 latch, +0x534/+0x550 talks and `326878c0`. **June-only.**
33. **325d0356-325d037c** +0x5aa -> SetNpcMessage(**+0x56c**), clear. **June-only.**
34. **325d037c-325d03bc** talk-box re-enable after 10 s. March 1319-1330. **same.**
35. **325d03bc-325d03ee** m_pUINPC->OnFrameMove. March 1332-1333. **changed: needs +0x1a6.**
36. **325d03ee-325d0427** `32ed06f0` dlg OnFrameMove when JoinNpc && !static. March 1335-1338. **same.**
37. **325d0427-325d07bb** idle motion when !m_bPlay && !static (March 1340-1412), **changed**. `31c8f240` SetPlaySpeed(**+0x5dc** WaitAnimSpeed, T+0x184) first.
    - Event NPC: `Wait01`/`Active_Wait01` literals (same as March).
    - Otherwise m_iMotionId is renumbered: 1 = map[1] plus talk box (March 0), 2/3/10 = map[id] via `325d24b0`, default = map[1] via `325d2370`, **11 = first unplayed entry in +0x204 vector** (wstring + played flag, 0x20 each, filled from T+0x608), or `325ca920` when exhausted (bye, `325c9130`(0), m_bEnable=0).
    - `31d33fd0` ChangeAnim(name,1,0) and `31d28d00` Play(2).
    - Skin 2 repeats this, but ids outside {1..3,10} use map[1].
    - Map **+0x1f4** = std::map<int,wstring> from T: 1 WAIT01 (+0x4b0), 2 WAIT02 (+0x4cc), 3 WAIT03 (+0x4e8), 4 TALK (+0x520), 10 TALKLOOP (+0x504), 5 TALKWAIT (+0x53c), 6 BYE (+0x558), 0 NORMAL (+0x574).
38. **325d07bb-325d0945** static mesh: SetPlaySpeed(+0x5dc). If `31d34110` HasAnim(map[5]) && JoinNpc -> map[5] Play(0 LOOP), else map[0] Play(2), then the same for skin 2. March 1413-1417 (`Normal`). **changed.** m_bPlay is still never set (March quirk kept).
39. **325d0945-325d0aa6** static-mesh talk box (`323151a0` ctor, `32e90410` Push, `32310930` dtor, distance +0x148 < 3800). March 1419-1449. **same.**
40. **325d0aa6-325d0af4** skin OnFrameMove (vtable+4). March 1451. **changed: +0x1c4 too.**
41. **325d0af4-325d0b6b** m_bFirstAnim: `325d1ac0` GetHeadBonePos, or m_vPos+210. March 1453-1465. **same.**
42. **325d0b6b-325d0c55** anim end (+0x2ac) && !static (March 1467-1500), **changed**.
    - Event NPC: 1. Otherwise: if **+0x1a6 && T+0x42c** (`TalkLoopAniNpc`) -> 10; else rand%9: 0 -> 2<->1, 1 -> 3<->1, else 1 (the March table shifted +1).
    - Non-empty +0x204 -> 11. If !HasAnim(map[id]) && queue empty -> 1.
43. **325d0c55-325d0e5d** near sound: `31c7b5c0` dist², `31ce0e10` PlaySound, `31d41070` Set3DPos, `31d41750` SetMax3D, `31d410f0` IsPlaying, `31d41140` SetVolume, `31cdd1b0` StopSound. March 1535-1588. **same.**
44. **none in June**: March 1503-1533 (`_IN_HOUSE_` head scale) is compiled out of the June service build. **March-only.**

## Summary

- **June-only members: about 25.**
  - +0x190, +0x1a6, +0x1c4/1c8/1cc, +0x1ec, +0x1f0, +0x1f4 map, +0x204/208 vector, +0x230, +0x234, +0x238, +0x239, +0x42a.
  - Talks +0x4b4/4d0/4ec, +0x534/550/56c/588. +0x56c/+0x588 come from T+0x478/0x494; their keys are untied, likely the gift-talk group.
  - +0x5a4/5a8/5a9/5aa/5ab, +0x5d8/5dc.
- **June-only templet fields: 13.** 0x42c, 0x4b0..0x574 (8 motion names), 0x590, 0x5a1, 0x5b0, 0x5e8, 0x608, 0x184 (plus 0x180).
- **Callees with no March counterpart:**
  - `321e26d0`, `321d97c0`, `321de4f0` (effect set);
  - `325ca0b0` QuestMsgOverride; `325d2370`/`325d24b0` motion-map operator[];
  - `32696fa0`, `321cd500`, `325d1d70`, `32679e10`, `325c9e70` (friendship);
  - `325ca920` motion-queue end; `3214f1b0`/`32150800` agit-bank UI; `323bddf0` bOnlyClickable; `32ee3860`.
- **JUNE_BUGS candidates:**
  1. The quest button ignores completable, marked and event quests (block 18; confirmed, likely deliberate). The +0x239 completable check runs but its result is discarded (block 15; corrected by C9h-5: not dead).
  2. June UIs 9/0xb/0xa run SetHouseName, the bye message and the June message every frame (block 28). Not checked in play.
  3. The talk-mark blink is gone: Up only, the 0.5/1.0 s timer is dead (block 20).
  4. Skin 2 plays Wait01 while skin 1 plays a queued motion (block 37).
  5. Minor: the `g_pData+0x9c` test can never fail (block 4).
