# HouseParsing diff: June 2017 CX2LocationManager::HouseParsing (0x324f3260) vs March X2LocationManager.cpp:559-1134

Sources: raw disassembly `e1/house.asm` (0x324f3260..0x324fef70), plaintext/XOR key tables, direct disassembly of the callees named below, June data
`june_src_fixed/data034_kom/DLG_Map_House*.src.lua`. March: `X2Lib/X2LocationManager.cpp` / `.h` (line numbers are file lines). Flag state of March
`#ifdef`s was resolved by grepping the `#define`s (not by `cl /P`), see section 3.

Conventions
- Getters (June): gI 0x32fe9fd0 int, gS 0x32feae90 wstring, gB 0x32fe9e40 bool (number -> setne), gF 0x32fea370 float, gFi 0x32fea820 float[idx],
  gIi 0x32fea4e0 int[idx], gSi 0x32feafd0 wstring[idx], BT(name) 0x32fe9940, BT(idx) 0x32fe99b0, BT(name,n) 0x31c7b980 ("%s%d"), ET 0x32fe9a10,
  gColor 0x31c9eba0 (GetUserDefinedTypeValue<D3DXCOLOR>). Every getter is called with bLog=true (`push 1`), same as March's `LUA_GET_VALUE` default.
- Every "default" below is a store executed only when the getter returned false (`test al,al / jne` skips it), i.e. the same contract as March's
  `LUA_GET_VALUE( .., init )` (LuaCommon/KLuaManager.h:118). The VA is that store.
- STR-id keys: default id -1 (= March STR_ID_EMPTY, KTDXLIB/StringID_def1.h:7) then GET_STRING. June GET_STRING = `31c31060()` singleton, `[+0x40]` debug flag ->
  `"[S%1%]%2%"` (0x3342f1d0) else `31c30d30` GetGeneralString. That is March's `NETERROR_KOREAN_HELP` GET_STRING (KTDXStringTable.h:118-121; defined
  Always_Global.h:144), so identical. GetGeneralString(-1): `js 0x31c30d6b` returns the empty member (+0x24), March returns m_wstrEmpty (KTDXStringTable.cpp:168-171). A missing id becomes L"".
- Templet = 0x618 bytes, `new` @0x324fbba2, ctor 0x323c3cf0 @0x324fbbc1. From the copy section (0x324fbbcc..0x324fc122) on, getters write straight into the templet (ESI);
  before it they write stack temps that the copy section moves into the templet. Destinations were verified by real stack slots (default-store slot == copy-read slot,
  copy reads at pending-push depth 4/8 corrected), not by the 0x1b0 normalisation in framemap.

## 0. Frame of the function

| step | June | March |
|---|---|---|
| loop over `vecHouseID` | 0x324f32e0..0x324fef29 (`edi`=KLuaManager = arg1, vec = arg2, ret 8) | :562 |
| skip if already in `m_mapHouseTemplet` | 0x324f3315 `call 322aa950` on this+0x34 (find), `cmp [esp+0xcc],[ecx+0x38]` / `jne 0x324fef09` = continue | :566-568 same |
| `BeginTable("House")` | 0x324f333a (arg 0x334a1240); miss -> 0x324fef09 (next id, silent) | :570 same |
| `BeginTable(houseID)` | 0x324f334c (0x32fe99b0, `ebx`=id); miss -> 0x324fef31 | :573 |
| failure path | 0x324fef31..0x324fef46: `call [0x3375c888]` (MessageBoxW) hWnd=`[[0x337676e8]+4]` (presumably g_pKTDXApp->GetHWND(), unverified), text L"CX2LocationManager House OpenScript Error" (0x334a1088), caption L"Error" (0x3345828c), uType 0; then jumps to the epilogue 0x324fef4c = `return` (no EndTable("House"), remaining ids dropped) | :1125-1126 same text/caption/MB_OK, `return` |
| insert into map | 0x324fecf8..0x324fed0c: `mov ecx,[esp+0xc0]`(id), `3286eb20` (alloc 0x18-byte map node/pair key id, value = templet ptr, 0x324fecfb), `32821f20` insert on `this+0x34` (0x324fed0c). Unconditional (the earlier find already excluded duplicates). Then ET(house) 0x324fed15 | :1119 `m_mapHouseTemplet.insert( make_pair( id, pHouseTemplet ) )`, :1121 EndTable |
| tail | cleanup of ~40 temps 0x324fed1a..0x324feef4, ET("House") 0x324fef04, `inc` index / loop 0x324fef09..0x324fef29 | :1129 |

Verdict: same flow and same MessageBox as March.

## 1. Key-by-key table (June order)

D = default on a miss (VA of the store). "dest" = June templet offset. M = March line(s) (read / assign) and member. All March members are `HouseTemplet::`.

| key | June getter @call | D (VA) | dest | March | verdict |
|---|---|---|---|---|---|
| Type | gI @324f33d6 | -1 (@324f33df) | +4 | :578 / :943 m_Type | same |
| (id) | `[esp+0xc0]` = id | - | +0 | :942 m_ID | same |
| Name | gI @324f3486, then GET_STRING 0x324f3493-0x324f3548 | id -1 (@324f348f) | +8 (wstring) | :581-582 / :944 m_HouseName | same |
| NPCTextureName | gS @324f36e6 | L"" (@324f36fd, 31f54a10) | +0x24 | :585 / :946 m_NPCTextureName | same |
| NPCTextureKey | gS @324f384f | L"" (@324f3865) | +0x40 | :588 / :947 m_NPCTextureKey | same |
| NPCTexturePosX | gF @324f39af | -999.0f (const 0x33458018, @324f39be) | +0x5c | :594 / :948 m_NPCTexturePos.x | same |
| NPCTexturePosY | gF @324f3afd | -999.0f (@324f3b0c) | +0x60 | :595 / :949 .y | same |
| QuestInfoPosX | gF @324f3c47 | 0.0 (@324f3c52) | +0x64 | :600 / :951 m_QuestInfoPos.x | same |
| QuestInfoPosY | gF @324f3d80 | 0.0 (@324f3d8b `fst`) | +0x68 | :601 / :952 .y | same |
| EventQuestInfoPosX | gF @324f3f22 | 0.0 (@324f3f2d) | +0x6c | :606 / :954 m_EventQuestInfoPos.x | same |
| EventQuestInfoPosY | gF @324f40b9 | 0.0 (@324f40c4) | +0x70 | :607 / :955 .y | same |
| TitleTextureName | gS @324f4264 | L"" (@324f427a) | +0x74 | :613 / :957 | same |
| TitleTextureKey | gS @324f43d2 | L"" (@324f43e8) | +0x90 | :614 / :958 | same |
| NPCMeshName | gS @324f451a | L"" (@324f4530) | +0xac | :617 / :968 m_NPCMeshName | same |
| NPCAniTex | gS @324f4641 | L"" (@324f4658) | +0xc8 | :619 / :969 m_NPCAniTex | same |
| NPCMeshName2nd | gS @324f47c2 | L"" (@324f47da) | +0xe4 | June-only | June-only |
| NPCAniTex2nd | gS @324f491b | L"" (@324f4931) | +0x100 | June-only | June-only |
| AlphaBlend1stMesh | gB @324f4aaf | false (@324f4ab8) | +0x11c | June-only | June-only |
| AlphaBlend2ndMesh | gB @324f4c35 | false (@324f4c3e) | +0x11d | June-only | June-only |
| NoEdge2ndMesh | gB @324f4d6e | false (@324f4d77) | +0x11e | June-only | June-only |
| (UI file name) | not a key: `31f54a10( +0x120, L"", 0 )` @324fbd1a | L"" always | +0x120 | June-only (only a relation row's Change_UIFileName sets it later) | June-only |
| NPCJoinTalk | BT @324f4da1; gSi idx 1.. @324f4dd4/@324f4e02 (temp L"" from @324f4daa), stop on first miss; push `31aad250`; ET @324f4e14 | none (empty vector) | +0x13c (copy @324fbd45, 31c57560) | :622-633 / :971 m_vecNPCJoinTalkSound | same |
| NPCByeTalk | BT @324f4e47; gSi @324f4e7e/@324f4eb2; ET @324f4ebf | none | +0x14c (copy @324fbd58) | :636-647 / :972 m_vecNPCByeTalkSound | same |
| NPCMeshPos | BT @324f4ee0; gFi 1/2/3 @324f4ef7/f17/f37 | 0.0 each (@324f4f02/f22/f42); table absent: NO store (stack garbage) | +0x15c/+0x160/+0x164 | :651-657 / :974 m_NPCMeshPos (uninitialised D3DXVECTOR3 when absent) | same (both undefined if the table is missing) |
| NPCMeshRot | BT @324f4f5b; gFi @324f4f72/92/b2 | 0.0 (@324f4f7d/9d/bd); absent: garbage | +0x168/+0x16c/+0x170 | :658-664 / :975 | same |
| NPCMeshScale | BT @324f4fd6; gFi @324f4fed/500d/502d | 1.0 (`fld1`, @324f4ff8/5018/5038); absent: garbage | +0x174/+0x178/+0x17c | :665-671 / :976 m_NPCMeshScal | same |
| JoinAnimSpeed | gF @324f517b | 1.0 (@324f5186) | +0x180 | June-only | June-only |
| WaitAnimSpeed | gF @324f52ba | 1.0 (@324f52c5) | +0x184 | June-only | June-only |
| ByeAnimSpeed | gF @324f53e7 | 1.0 (@324f53f2) | +0x188 | June-only | June-only |
| JoinArea | gI @324f54c4 | 500 = 0x1f4 (@324f54cd) | +0x194 | :674 / :978 m_NPCJoinArea | same |
| bCanTalkNpc | gB @324f55e3 | true (`mov byte,1` @324f55ec) | +0x3a4 | :680 / :1025 m_bCanTalkNpc (CHRISTMAS_TREE, Always.h:2819) | same |
| bExchangeNewItem | gB @324f5752 | false (@324f575b) | +0x3a5 | :686 / :1036 m_bExchangeNewItem (SERV_NEW_ITEM_SYSTEM_2013_05, ServerDefine.h:3653) | same |
| PVP_NPC | gB @324f5861 | false (@324f586a) | +0x198 | :721 / :979 m_bPvpNpc | same |
| MAKING_NPC | gB @324f595f | false (@324f5968) | +0x199 | :722 / :980 m_bMakingNpc | same |
| TRAINING | gB @324f5a34 | false (@324f5a3d) | +0x19a | :723 / :981 m_bTraining | same |
| FREE_TRAINING | gB @324f5b68 | false (@324f5b71) | +0x19b | :724 / :982 m_bFreeTraining | same |
| ENCHANT | gB @324f5c31 | false (@324f5c3a) | +0x19c | :725 / :983 m_bEnchant | same |
| ATTRIBUTE | gB @324f5d1b | false (@324f5d24) | +0x19d | :726 / :984 m_bAttribute | same |
| REPAIR_NPC | gB @324f5e1d | false (@324f5e26) | +0x19e | :727 / :985 m_bRepair | same |
| SELL_NPC | gB @324f5eed | false (@324f5ef6) | +0x19f | :728 / :986 m_bSell | same |
| OLD_ENCHANT | gB @324f5ffd | false (@324f6006) | +0x1a9 | June-only | June-only |
| OLD_ADVANCED_ENCHANT | gB @324f61b0 | false (@324f61b9) | +0x1aa | June-only | June-only |
| GUILD | gB @324f624d | false (@324f6256) | +0x1a0 | :731 / :988 m_bGuild | same |
| RANKING | gB @324f6316 | false (@324f631f) | +0x1a1 | :733 / :990 m_bRanking | same |
| EXCHANGE | gB @324f63eb | false (@324f63f4) | +0x1a2 | :736 / :994 m_bExchange | same |
| PRIVATE_BANK | gB @324f650a | false (@324f6513) | +0x1a3 | :742 / :1000 m_bPrivateBank | same |
| PERSONAL_SHOP | gB @324f663e | false (@324f6647) | +0x1a4 | :750 / :1008 m_bPersonalShop | same |
| AGENCY_TRADER_REGISTER | gB @324f6824 | false (@324f682d) | +0x1ab | :755 / :1011 m_bAgencyTraderRegister | same |
| AGENCY_TRADER_RECEIVE | gB @324f69ed | false (`bl`=0 by `xor bl,bl` @324f69da; @324f69f6) | +0x1ac | :756 / :1012 m_bAgencyTraderReceive | same |
| EVENT1 | gB @324f6aa2 | false (bl=0 @324f6a90; @324f6aab) | +0x1a5 | :764 / :1029 m_bEvent1 (NPC_EVENT_BUTTON, Always_Global.h:569) | same |
| EVENT2 | gB @324f6b57 | false (@324f6b60) | +0x1a6 | :765 / :1030 | same |
| EVENT3 | gB @324f6c0c | false (@324f6c15) | +0x1a7 | :766 / :1031 | same |
| EVENT4 | gB @324f6cc1 | false (@324f6cca) | +0x1a8 | :767 / :1032 | same |
| PHOTOZONE | gB @324f6dac | false (bl=0 @324f6da0; @324f6db5) | +0x429 | June-only | June-only |
| FRAGMENT_EXCHANGE | gB @324f6f2b | false (@324f6f34) | +0x42a | June-only | June-only |
| ROCKSCISSORSPAPER_NPC | gB @324f70f4 | false (@324f70fd) | +0x42b | June-only | June-only |
| EVENT_NPC | gB @324f71df | false (@324f71e8) | +0x1e8 | :771 / :1040 m_bEventNpc | same |
| EVENT_NPC_TIME | BT @324f7215; gFi 1..6 @324f722d,7247,7261,727b,7298,72b8; ET @324f72cb | 0.0 each (@324f7238,7252,726c,7286,72a3,72c3; the six temps are also zeroed @324f71ec-324f7211) | idx1 +0x1ec, idx2 +0x1f0, idx3 +0x1f4, idx4 +0x1f8, idx6 +0x1fc, idx5 +0x200 | :774-784 / :1041-1046 (idx1..4 Year/Month/Day/Hour, idx5 ActiveMin, idx6 WaitMin) | same (+0x1fc = WaitMin = idx6, +0x200 = ActiveMin = idx5) |
| COMMON_NPC_LOCATION | BT @324f72f6; entry BT(idx from 1) @324f730c, loop @324f7452; entry gIi 1 @324f7335 (world), gFi 2..8 @324f735d..324f741d; push `321f3690` @324f743e; ET entry @324f7447, ET table @324f7463 | world 0 (`movzx/neg/sbb/and` @324f733a-324f7343, 0 on miss), floats 0.0 (@324f7368..324f7428) | +0x224 (assign `323bf8b0` @324fc10a) | :788-811 / :1080 m_vecCommonPos (world default CX2World::WI_NONE = 0, X2World.h:29; join area under LOCAL_NPC_JOIN_AREA, Always.h:2592) | same |
| CUSTOM_NPC_LOACION (sic) | see 1.A | | +0x234 | June-only | June-only |
| NPC_ID_LIST | BT @324f7629; gIi idx 1.. @324f764e/@324f768a (temp pre-set -1 @324f7643); push `31f8b860`; ET @324f7697 | -1 temp, stop on first miss | +0x214 (`32853ff0` @324fbc8b) | :814-825 / :960 m_NPCList | same |
| Wait_Talk | gI @324f7897 | id -1 (@324f78a0) | +0x244 | :886 / :1048 m_strWaitTalk | same |
| Make_Talk | gI @324f7a8c | -1 (@324f7a95) | +0x260 | :888 / :1049 m_strMakeTalk | same |
| Shop_Talk | gI @324f7c73 | -1 (@324f7c7c) | +0x27c | :890 / :1050 m_strShopTalk | same |
| Pvp_Talk | gI @324f7e4a | -1 (@324f7e53) | +0x298 | :892 / :1051 m_strPvpTalk | same (still read) |
| Quest_Talk | gI @324f805c | -1 (@324f8065) | +0x2b4 | :894 / :1052 m_strQuestTalk | same |
| Bye_Talk | gI @324f825f | -1 (@324f8268) | +0x2d0 | :896 / :1053 m_strByeTalk | same |
| Exchange_Talk | gI @324f84ae | -1 (@324f84b7) | +0x308 | :898 / :1055 m_strExchangeTalk | same |
| Ranking_Talk | gI @324f86da | -1 (@324f86e3) | +0x2ec | :900 / :1054 m_strRankingTalk | same |
| PrivateBank_Talk | gI @324f8954 | -1 (@324f895d) | +0x324 | :904 / :1062 m_strPrivateBank | same |
| GuildAD_Talk | gI @324f8b7f | -1 (@324f8b88) | +0x340 | :911 / :1068 m_strGuildADTalk | same |
| PersonalShop_Talk | gI @324f8e09 | -1 (@324f8e12) | +0x35c | :918 / :1072 m_strPersonalShopTalk | same |
| Register_Talk | gI @324f9098 | -1 (@324f90a1) | +0x1b0 | :925 / :1014 m_strRegisterTalk | same |
| Receive_Talk | gI @324f92ca | -1 (@324f92d3) | +0x1cc | :927 / :1015 m_strReceiveTalk | same |
| Exchange_New_Item_Talk | gI @324f95b7 | -1 (@324f95c0) | +0x378 | :937 / :1057 m_strExchangeNewItemTalk | same |
| Upgrade_Talk | gI @324f980f | -1 (@324f9818) | +0x3cc | June-only | June-only |
| Service_Talk | gI @324f9a5a | -1 (@324f9a63) | +0x3ec | June-only | June-only |
| Trade_Talk | gI @324f9c79 | -1 (@324f9c82) | +0x40c | June-only | June-only |
| OutlineWidthMesh | gF @324f9ee8 | 1.2f (const 0x334a1080 = 0x3f99999a, `fld/fstp` @324f9ef1/324f9ef7) | **+0x18c** (copy `fld [esp+0xa0]` @324fbd1f) | June-only | June-only |
| bApplyAlphaTest | gB @324fa04b | false (@324fa054) | **+0x190** (copy `movzx [esp+0x3b]` @324fbd2c) | June-only | June-only |
| NPC_TALK_LIST<n> | see 1.B | | map +0x430 | June-only | June-only |
| NPC_COMMON_TALK | see 1.C | | struct +0x440 | June-only | June-only |
| TALK_BOX | BT @324fc136; entry BT(idx from 1) @324fc151 / loop @324fc3b3; MESSAGE gI @324fc21c; GET_STRING; push `31ecdf50` @324fc2ee on templet+0x394 (cleared first by `31c33790` @324fc128) | id -1 (@324fc225) -> L"" | +0x394 vector<wstring> | :1085-1098 m_vecNPCTalkBox (TALK_BOX_VILLAGE_NPC, Always.h:2489) | same |
| NPCNearSound | BT @324fc3d2; entry BT(idx from 1) @324fc3ed / @324fc479; gSi 1 @324fc407, gFi 2 @324fc42b, gFi 3 @324fc44d | L"" (@324fc419), -1.0f (const 0x334380f0, @324fc43a, @324fc45c); last entry wins as in March | +0x3a8 (wstr), +0x3c4, +0x3c8 | :1102-1115 m_wstrNearSoundFileName / m_fPlayNearSoundCoolTime / m_fPlayNearSoundDistance (ADD_PLAY_MUSIC_WHEN_VILLAGE_NPC_NEAR, Always.h:397) | same |
| HENIR_EQUIP_UPGRADE | gB @324fc642 (dest = field) | false (@324fc64b) | +0x3e8 | June-only | June-only |
| GUILD_AGIT_SERVICE | gB @324fc7eb | false (@324fc7f4) | +0x408 | June-only | June-only |
| GUILD_ENTER | gB @324fc8fe | false (@324fc907) | +0x409 | June-only | June-only |
| EXTRACTION | gB @324fc9fc | false (@324fca05) | +0x40a | June-only | June-only |
| TRADE | gB @324fcab3 | false (@324fcabc) | +0x428 | June-only | June-only |
| TalkLoopAniNpc | gB @324fcbfb | false (@324fcc04) | +0x42c | June-only | June-only |
| WAIT01_MOTION_NAME | gS @324fcd8d | L"Wait01" (0x334a1164, `31aa9200` @324fcd9d) | +0x4b0 | June-only | June-only |
| WAIT02_MOTION_NAME | gS @324fcf29 | L"Wait02" (0x334a1154, @324fcf39) | +0x4cc | June-only | June-only |
| WAIT03_MOTION_NAME | gS @324fd0c5 | L"Wait03" (0x334a1144, @324fd0d5) | +0x4e8 | June-only | June-only |
| TALKLOOP_MOTION_NAME | gS @324fd282 | L"Talkloop" (0x334a1130, lower-case l, @324fd292) | +0x504 | June-only | June-only |
| TALK_MOTION_NAME | gS @324fd3f5 | L"Talk" (0x334a1124, @324fd405) | +0x520 | June-only | June-only |
| TALKWAIT_MOTION_NAME | gS @324fd5b2 | L"TalkWait" (0x334a1110, @324fd5c2) | +0x53c | June-only | June-only |
| BYE_MOTION_NAME | gS @324fd719 | L"Bye" (0x334a1108, @324fd729) | +0x558 | June-only | June-only |
| NORMAL_MOTION_NAME | gS @324fd8b5 | L"Normal" (0x3344b6c8, @324fd8c5) | +0x574 | June-only | June-only |
| Talk_Change_Condition | see 1.D | | vector<row 0x48> +0x590 | June-only | June-only |
| AlwaysShow | gB @324fe0a3 | false (@324fe0ac) | +0x5a0 | June-only | June-only |
| bOnlyClickable | gB @324fe1eb | false (@324fe1f4) | +0x5a1 | June-only | June-only |
| CUSTOM_MOTION_NAME | see 1.E | | vector +0x608 | June-only | June-only |
| bMovingNPC | gB @324fe422 | false (@324fe42b) | +0x604 | June-only | June-only |
| RenderOption | see 1.F | | struct +0x5a4 (0x44) | June-only | June-only |
| EFFECT_SET_NAME | gS @324fecbd (dest = field) | L"" (0x3344f1f4, `31aa9200` @324feccd) | +0x5e8 | June-only | June-only |

Not read by June and compiled in March US: none (section 3).

### 1.A CUSTOM_NPC_LOACION (key is spelled so in June's data and code, 0x334a11c8)
- BT(name) @324f7493; entries BT(idx) from 1: @324f74a9, loop test @324f75e8; ET table @324f75f9. Miss of the table: nothing (vector stays empty).
- Entry layout is exactly `CommonNpcPos` (0x20 bytes: world int, pos vec3, rot vec3, join area float), built in a temp at `[esp+0x190]` and pushed with `321f3690`
  (@324f75d4, the same vector<CommonNpcPos>::push_back as COMMON): gIi idx1 @324f74cb = world (0 on miss, `movzx/neg/sbb/and` @324f74d0-324f74d9);
  gFi idx2,3,4 @324f74f3/7513/7533 = pos x,y,z; idx5,6,7 @324f7553/7573/7593 = rot x,y,z; idx8 @324f75b3 = join area; defaults 0.0 (@324f74fe, 751e, 753e, 755e, 757e, 759e, 75be).
- Destination: local vector `[esp+0x130]` (zeroed @324f7468) assigned to templet +0x234 by `323bf8b0` @324fc11d (COMMON goes to +0x224 @324fc10a).
- June data example: `CUSTOM_NPC_LOACION = { { WORLD_ID.WI_SPA_RUBEN, -3119, 1000, 2110, 0, 60, 0, 0 }, ...}` (DLG_Map_House.src.lua:9396; a row of 7 numbers is accepted, idx8 -> 0.0).
- Reader (per E1 NOTES, not re-read here): AddFieldNpc world check uses +0x224 world ids or, when +0x234 is non-empty, +0x234.

### 1.B NPC_TALK_LIST<n> (Main_Talk and the gift talks)
- Loop: `BT(name,n)` = 0x31c7b980 builds "NPC_TALK_LIST%d" (snprintf "%s%d", funcmap row 31c7b980), n starts at 0 (`mov [esp+0x50],0` @324fa077, first call @324fa07f with key 0x334a11ac), next n via
  `BT("NPC_TALK_LIST", n+1)` @324fb233 -> `je 0x324fa08c`; ends at the first missing n (jump to 0x324fb240). June data: NPC_TALK_LIST0..3 (DLG_Map_House.src.lua:61-100).
- Per n, a temp `NpcTalkList` (0x9c bytes, ctor 0x32505e20 @324fa093): `+0` vector<wstring> (zero), `+0x10` wstring, `+0x2c`, `+0x48`, `+0x64`, `+0x80` (each cap 7, empty).
  - `Main_Talk` (0x334a11a0): BT @324fa0a9 (miss -> @324fa28d), gIi idx 1.. @324fa0c9 / @324fa277 (no default; stops at first miss), each id -> GET_STRING (@324fa0d8-324fa1a8) -> push_back (`31ecdf50` @324fa1c7) into `+0`; ET @324fa288. Elements are resolved text, not ids.
  - `Receive_Like_Gift_Talk` gI @324fa459 -> `+0x10`; `Receive_DisLike_Gift_Talk` @324fa79a -> `+0x2c`; `Receive_Normal_Gift_Talk` @324faad5 -> `+0x48`; `Receive_Cash_Gift_Talk` @324fadf0 -> `+0x64`; `Level_Up_Talk` @324fb05d -> `+0x80`. Each: id default -1 (@324fa462, 324fa7a3, 324faade, 324fadf9, 324fb066), GET_STRING, `31f54a10` assign to the member (stack slots 0x9ac/0x9c8/0x9e4/0xa00/0xa1c minus struct base 0x99c).
  - ET(NPC_TALK_LIST<n>) @324fb1aa.
- Store: `std::map<int, NpcTalkList>` (red-black node 0xb0: key +0xc, value +0x10, head ctor `323c32c0` @324fa05f). pair `325d25f0` @324fb1c4 (key = n from `[esp+0x44]`), node `32506dc0` @324fb1de, insert `325d2950` @324fb1f3, temps dtor `325d2170` @324fb207/324fb224.
- Destination: map at templet +0x430 (0x10 bytes), assigned @324fc06e-324fc09f (self-check, `325d2a40` + `323c2150`). The map is empty when NPC_TALK_LIST0 is missing.
- March: nothing.

### 1.C NPC_COMMON_TALK
- Local 4-wstring struct (0x70 bytes, ctor `323be930` @324fb247, always run, so an absent table gives four empty strings); BT @324fb261 (miss -> @324fbba2).
- Four keys, each id default -1 then GET_STRING, assigned to: `After_Talk` gI @324fb626 (default @324fb62f) -> struct +0; `Every_Day_Reward_Talk` @324fb429 (@324fb432) -> +0x1c; `Reject_Gift_Talk` @324fb886 (@324fb88f) -> +0x38; `Gift_UI_Talk` @324fba98 (@324fbaa1) -> +0x54 (assign via `31aa9200` @324fbb48); ET @324fbb9d.
- Destination: `323bf5a0` @324fc0b2 copies the four strings (ret 4, `31f54910`) to templet +0x440 (+0x440 After, +0x45c Every_Day, +0x478 Reject, +0x494 Gift_UI). Element layout: 4 contiguous std::wstring. March: nothing.

### 1.D Talk_Change_Condition (key 0x334a10f0), stored from @324fdf66 into the vector at templet +0x590
- BT @324fd8d3 (miss -> @324fdfb3, nothing); rows BT(idx) from 1: @324fd8ee, loop @324fdf9d; ET row @324fdf77; ET table @324fdfae.
- Row = 0x48 bytes at `[esp+0x234]`, strings re-initialised each iteration (@324fd900-324fd92d), push_back `32506520` (elem size 0x48, magic 0x38e38e39) @324fdf6c, row dtor `323bea80` @324fdf8f.
  - `TYPE` gI @324fd9c8 -> row +0 ; default **1** (`mov [esp+0x234],1` @324fd9d1); a hit stores the value (@324fd9de-324fd9e5).
  - `STATE` gI @324fda8d -> row +4 ; default **0** (`movzx/neg/sbb/and` @324fda92-324fda99, store @324fdaa2).
  - **Filter**: `cmp [esp+0x234], ebx(=0)` / `jne 0x324fdf73` @324fdaa9-324fdab0: a row whose TYPE != 0 (including a missing TYPE, default 1) is dropped (goes straight to ET); only TYPE == 0 rows are read further and pushed. The June consumer 325ca1b0 (talkbye_diff.md) also tests `row+0 == 0`, so the test is redundant there.
  - `QUEST_ID` gI @324fdb85 -> row +8 ; default 0 (@324fdb8e).
  - `SUB_QUEST_ID` gI @324fdca9 -> row +0xc ; default 0 (@324fdcb2).
  - `MESSAGE` gI @324fdd76 -> id default -1 (@324fdd7f) -> GET_STRING -> wstring row +0x10 (`31f54a10` @324fde48).
  - `MOTION` gS @324fdf42 -> wstring row +0x2c (getter writes in place), default L"" (@324fdf59).
- No June data in the client Lua uses the key (grep over june_src_fixed: 0 hits); parser only.

### 1.E CUSTOM_MOTION_NAME (key 0x334a10dc) -> templet +0x608
- Local vector `[esp+0xe0]` (zeroed @324fe1f8-324fe206); BT @324fe21e (miss -> @324fe31c); gSi idx 1.. @324fe255/@324fe2f2 into a temp wstring (stop on first miss; no default);
  element built by `32505a00` @324fe285 = { wstring name (0x1c) + byte flag (`[esp+0x4b]` = 0, @324fe262) } 0x20 bytes, `32506ea0` push_back @324fe29d; ET @324fe303.
- Assigned to +0x608 by `323c0610` @324fe32a (runs even when the table is absent: empty vector). June data: `CUSTOM_MOTION_NAME = { "Start", "Wait", "Wait", "End" }` (DLG_Map_House.src.lua:9390). March: nothing.

### 1.F RenderOption (key 0x3348cdf4) -> templet +0x5a4 (0x44 bytes)
- Local struct `[esp+0x1ec]` built by default ctor `323bf7e0` @324fe434 **before** the table is opened: +0 byte UseRenderOption = 0, +1 byte UseAlphaBlend = 0, +4 SrcBlend = 5 (D3DBLEND_SRCALPHA), +8 DestBlend = 6 (D3DBLEND_INVSRCALPHA), +0xc wstring MultiTex = L"", +0x28 vec3 MultiTexSpeed = (0,0,0), +0x34 D3DXCOLOR Color = (1,1,1,1).
- BT @324fe44a; **miss jumps to @324feb58** (not past the copy), hit falls through ET @324feb53 to the same place. The copy `32345bb0` (@324feb66, `lea ecx,[esi+0x5a4]`, ret 4; copies bytes 0/1, dwords +4/+8, wstring +0xc via 31f54910, dwords +0x28..+0x40) therefore runs on **both** paths: no table -> the ctor defaults land in +0x5a4.
- Keys (all read inside the table):
  - `UseRenderOption` gB @324fe5a7 -> +0 ; default false (@324fe5b0).
  - `UseAlphaBlend` gB @324fe6e1 -> +1 ; default false (@324fe6ea).
  - `MultiTex` gS @324fe7bc -> +0xc ; default L"" (`31aa9200` @324fe7d1).
  - `MultiTexSpeed` BT @324fe7df, gFi 1/2/3 @324fe7f6/816/836 -> +0x28/+0x2c/+0x30 ; defaults 0.0 (@324fe801, 821, 841); ET @324fe84c.
  - `SrcBlend` gI @324fe927 -> +4 ; default 5 (@324fe930); tmp pre-zeroed @324fe856.
  - `DestBlend` gI @324fea36 -> +8 ; default 6 (@324fea3f); tmp pre-zeroed @324fe950.
  - `Color` (xor-built, use @324fea86) via `gColor` 0x31c9eba0 @324feaf0 (Lua userdata `D3DXCOLOR(r,g,b,a)`, 16-byte copy) -> +0x34..+0x40 ; default (1,1,1,1) (4x `fld1`/`fst` @324feaf9-324feb48).
- June data: DLG_Map_House_NPC_RelationData.src.lua:682-691 (MultiTex "Elysion_Hologram.XET", UseRenderOption, UseAlphaBlend, MultiTexSpeed {2,0,0.5}, SrcBlend/DestBlend D3DBLEND_*, Color D3DXCOLOR(1,0.9,0.9,0.5)); the House file itself has no RenderOption entries; the relation loader (E1a part 2) carries the same block.
- March: nothing.

## 2. Defaults summary (answers to question 2)
- All 8 `*_MOTION_NAME`: literal wstrings: Wait01 / Wait02 / Wait03 / Talkloop (sic) / Talk / TalkWait / Bye / Normal, stored with `31aa9200` (wstring::assign(const wchar_t*)), VAs in the table.
- OutlineWidthMesh 1.2f -> +0x18c; bApplyAlphaTest false -> +0x190 (both previously unmapped, now closed by real-slot matching: default-store slots [esp+0xa0] / [esp+0x3b] equal the copy-section reads @324fbd1f / @324fbd2c).
- AlwaysShow, bOnlyClickable, bMovingNPC, HENIR_EQUIP_UPGRADE, GUILD_AGIT_SERVICE, GUILD_ENTER, EXTRACTION, TRADE, TalkLoopAniNpc, PHOTOZONE, FRAGMENT_EXCHANGE, ROCKSCISSORSPAPER_NPC, OLD_ENCHANT, OLD_ADVANCED_ENCHANT: false. The last group uses `al`/`bl` as zero: `al` is the getter's false return; for PHOTOZONE etc. `bl` is zeroed by `xor bl,bl` immediately before each call (checked for all nine `bl`-store keys).
- Anim speeds 1.0f (JoinAnimSpeed/WaitAnimSpeed/ByeAnimSpeed). Every STR-id talk key: -1 -> GET_STRING -> L"". Talk_Change_Condition, NPC_COMMON_TALK, NPC_TALK_LIST, CUSTOM_*, RenderOption defaults in 1.A-1.F.

## 3. March keys June no longer reads / changed handling (answer to question 3)
- No March key that is compiled in the US build is dropped: every common key keeps its default, type, conversion and condition (table above, all "same").
- Not read by June and not compiled in March US either (so nothing is lost; per `#define` grep, not preprocessed):
  - `SHOP_CATEGORY_TYPE_LIST` (:837-852): inside `#ifndef ADD_HOUSE_ID_TO_MANUFACTURE`; the macro is defined (Always.h:2816), so March US never read it either. June has no m_ShopCategoryTypeList field.
  - `WEB_EVENT` (:747): `WEB_POINT_EVENT` is commented out (Always.h:2753).
  - `SYNTHESIS` (:760) and `Synthesis_Talk` (:932): `SERV_SYNTHESIS_AVATAR` is defined only in ServerDefine_JP.h:51.
- `Pvp_Talk`: still read by June (@324f7e4a), default -1, stored at +0x298; unchanged.
- March's `m_vecNPCTalk` (vector<NpcFieldTalk>, .h:292) is not filled by March HouseParsing; June's parser has no counterpart store either (no vector other than the ones listed above is written).
- Read-order differences are irrelevant (flat keyed lookups in one table), except that June re-opens nested tables (NPC_TALK_LIST<n>, Main_Talk, NPC_COMMON_TALK) which March has no counterpart for.

## 4. Unsettled / unverified
- `[0x337676e8]+4` as g_pKTDXApp->GetHWND() and `322aa950` as map::find are inferred from shape, not read.
- Meaning of TYPE values other than 0 in Talk_Change_Condition (rows are dropped, so they never reach a consumer).
- Whether `bLog=true` on `BT("NPC_TALK_LIST", n)` logs on the expected final miss: not read (it runs once per house, the same bLog default March's BeginTable uses).
- June's WORLD_ID.WI_NONE value (assumed 0 because the miss default stored is 0; March WI_NONE = 0).
- Consumers of the new templet fields (+0x430 map, +0x440 struct, +0x590 rows, +0x608, +0x5a4, +0x5e8, +0x18c, +0x190) are other E1a/E1b items; only the first two readers of +0x590 (325ca0b0/325ca1b0) were seen, in talkbye_diff.md.
