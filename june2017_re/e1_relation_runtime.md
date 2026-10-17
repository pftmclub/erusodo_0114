# Atlas E1: June's village NPC relation runtime and house templet map (RE notes, atlas E1a, 2026-10-17)

Read-only RE from Ghidra and raw capstone disassembly (`june2017_re/funcdis.py`, `disasm_range.py`, `xordecrypt.py`).
E1a ported the data half: the loaders, the lookups and the applied lists live on `CX2QuestManager` behind
`SERV_IRUHADEV_ELYSION_VILLAGE`. This file carries what E1b / E1c need: the June templet layout, the relation row, the
lookups' exact rules, the TFieldGame functions (323ba5f0 / 323b0980 / 323b7ff0 / 323a9ed0 / 323b8970) and the two refresh
functions, already written and parked here because they call the TFieldGame side (E1c pastes them back).
The two NPC diffs that E1b works from are `e1_createfieldnpc_diff.md` and `e1_talkbye_diff.md`
(drafted by subagents from raw disassembly; spot-checked 325cba44..325cba91 name override, 325cc7a0 T+0x120 -> NPC +0x1d0,
325ca325 the bye step's +0x1a6 guard). "NOTES.md" in those files means this file.

Other facts settled in E1a:
- June loads `DLG_Map_House_NPC_RelationData.lua` in its startup script loader `3258c470 @3259174c`, after PVPData / PVEData.
- The replace step (323b7ff0) copies row +0x280 (Change_TalkLoopMotionName) into both T+0x53c (TALKWAIT) and T+0x504
  (TALKLOOP) and never reads row +0x2b8 (Change_TalkWaitMotionName): 323b8627..323b867d (JUNE_BUGS).
- The row's RenderOption block (+0x328..+0x368) has no constructor in June (32347110 builds only the MultiTex string; the
  templet's 323bf7e0 sets Src 5 / Dest 6 / colour 1). E1a gives the row the templet defaults (JUNE_BUGS).
- 323ba5f0's house gates: code events 2013 (house 40091 HI_EVENT_DOLL), 2081 (40129 HI_EVENT_BLESS_OF_SOLACE), 2186
  (40089 HI_EVENT_GUILD_EDAN_ROOM): the house is skipped while its event is off; 40090 HI_EVENT_GUILD_EDAN_VILAGE is
  skipped unless the premium-buff manager (g_pData+0x334) +0x5d is set, which June's PC-bang notify handler sets
  (326c7a95; March IsPremiumPC, likely); 40141 HI_SPA_SIGN_RUBEN / 40142 HI_SPA_SIGN_RANOX need code event 31.
- The per-quest refresh 32340630 has three June callers: QUEST_COMPLETE_ACK 323e57e0 (June event 0xf1), the
  ALL_COMPLETED_QUEST_COMPLETE_ACK handler 323e1710 (0x593), and 323e15e0 (June event 0x27d, a June-only quest packet:
  {int, int, 8 bytes, container}; March has no event or handler for it). Each calls RemoveUnitQuest( quest, true ) first.

## June HouseTemplet (0x618) offsets -> key (HouseParsing 324f3260; copy at 324fbbcc..)
+0 ID, +4 Type, +8 HouseName(str, from Name STR id), +0x24 NPCTextureName, +0x40 NPCTextureKey, +0x5c/60 NPCTexturePos,
+0x64/68 QuestInfoPos, +0x6c/70 EventQuestInfoPos, +0x74 TitleTextureName, +0x90 TitleTextureKey, +0xac NPCMeshName, +0xc8 NPCAniTex,
+0xe4 NPCMeshName2nd, +0x100 NPCAniTex2nd, +0x11c AlphaBlend1stMesh(b,0), +0x11d AlphaBlend2ndMesh(b,0), +0x11e NoEdge2ndMesh(b,0),
+0x120 UI file name (wstr, L"" at parse; only relation Change_UIFileName sets it), +0x13c JoinTalk sounds vec, +0x14c ByeTalk vec,
+0x15c pos, +0x168 rot, +0x174 scale, +0x180 JoinAnimSpeed, +0x184 WaitAnimSpeed, +0x188 ByeAnimSpeed, +0x18c OutlineWidthMesh?,
+0x190 byte ?, +0x194 JoinArea, +0x198 PVP +0x199 MAKING +0x19a TRAINING +0x19b FREE_TRAINING +0x19c ENCHANT +0x19d ATTRIBUTE
+0x19e REPAIR +0x19f SELL +0x1a0 GUILD +0x1a1 RANKING +0x1a2 EXCHANGE +0x1a3 PRIVATE_BANK +0x1a4 PERSONAL_SHOP +0x1a5..1a8 EVENT1-4
+0x1a9 OLD_ENCHANT +0x1aa OLD_ADVANCED_ENCHANT +0x1ab AGENCY_REG +0x1ac AGENCY_RECV, +0x1b0/+0x1cc wstr (Register/Receive talk),
+0x1e8 EVENT_NPC, +0x1ec..0x200 EVENT_NPC_TIME (yr,mo,day,hr, +0x1fc=6th, +0x200=5th), +0x214 NPC_ID_LIST vec,
+0x224 COMMON_NPC_LOCATION vec (0x20: world, pos, rot@+0x10, joinarea), +0x234 CUSTOM_NPC_LOACION vec,
+0x244 Wait +0x260 Make +0x27c Shop +0x298 Pvp +0x2b4 Quest +0x2d0 Bye +0x2ec Ranking +0x308 Exchange +0x324/340/35c (PrivBank/GuildAD/PShop?)
+0x378 ExchNewItem?, +0x394 TALK_BOX vec, +0x3a4 bCanTalkNpc(1), +0x3a5 bExchangeNewItem(0), +0x3a8 NearSound name, +0x3c4 cool, +0x3c8 dist,
+0x3cc Upgrade_Talk +0x3e8 HENIR_EQUIP_UPGRADE +0x3ec Service_Talk +0x408 GUILD_AGIT_SERVICE +0x409 GUILD_ENTER +0x40a EXTRACTION
+0x40c Trade_Talk +0x428 TRADE +0x429 PHOTOZONE +0x42a FRAGMENT_EXCHANGE +0x42b ROCKSCISSORSPAPER +0x42c TalkLoopAniNpc,
+0x430 / +0x440 Main_Talk / NPC_COMMON_TALK containers?, gift talks +0x478/+0x494..., +0x4b0 WAIT01 +0x4cc WAIT02 +0x4e8 WAIT03
+0x504 TALKLOOP +0x520 TALK +0x53c TALKWAIT +0x558 BYE +0x574 NORMAL (_MOTION_NAME), +0x590 Talk_Change_Condition rows,
+0x5a0 AlwaysShow +0x5a1 bOnlyClickable, +0x5a4 RenderOption{+0 UseRenderOption, +1 UseAlphaBlend, +4 SrcBlend, +8 DestBlend,
+0xc MultiTex wstr(0x5b0), +0x28 MultiTexSpeed vec3 (0x5cc), +0x34 Color 4f (0x5d8)}, +0x5e8 EFFECT_SET_NAME, +0x604 bMovingNPC,
+0x608 CUSTOM_MOTION_NAME vec.

## Relation row (0x388; ctor 32347110)
+0 QUEST_ID(-1) +4 TARGET_NPC_HOUSE_ID(-1) +8 type(-1) +0xc m_iUseCodeEventTriger(-1); wstr: +0x10 MeshName, +0x2c AniTex,
+0x48 MeshName2nd, +0x64 AniTex2nd, +0x80 VILLAGE_ID(-1), +0x84 AlphaBlend1stMesh("TRUE"/"FALSE"), +0xa0 AlphaBlend2nd,
+0xbc NoEdge2nd, +0xd8 UIFileName, +0xf4 Name(str, -> AddFieldNpc name arg), +0x110 HouseName -> T+8, +0x12c TextureName -> T+0x24,
+0x148 TextureKey -> T+0x40, +0x164 JoinTalk vec -> T+0x13c, +0x174 ByeTalk vec -> T+0x14c, +0x184 Wait->T+0x244, +0x1a0 ->0x260 Make,
+0x1bc ->0x27c Shop, +0x1d8 ->0x2b4 Quest, +0x1f4 ->0x2d0 Bye, +0x210 ->0x308 Exchange, +0x22c W01->0x4b0, +0x248 W02->0x4cc,
+0x264 W03->0x4e8, +0x29c ->0x520 TALK, +0x280 ->0x53c TALKWAIT, (next) ->0x504 TALKLOOP, +0x2d4 ->0x558 BYE, +0x2f0 ->0x574 NORMAL
(VERIFY key->row in loader disasm), +0x30c rot (applied if any !=0; also overwrites every COMMON_NPC_LOCATION rot),
+0x318 scale (if any != 0), +0x324 APPLY_FORCE byte, +0x328 UseRenderOption->T5a4, +0x329 UseAlphaBlend->5a5, +0x32c Src->5a8,
+0x330 Dest->5ac, +0x334 MultiTex->5b0, +0x350 MultiTexSpeed->5cc, +0x35c Color->5d8, +0x36c EFFECT_SET_NAME->5e8 (if !empty).
Strings only applied when non-empty (wcscmp with L"").
Small row (0x14, qm+0xbc, VRT_NOT_SHOW): +0 quest +4 house +8 code event +0xc ? +0x10 force byte.

## Lookups (return int; out = { vector<Row> rows; vector<Small> smalls; })
- 32340320 ByQuest(house,out): rows +0xac: gate = code==-1 || (g_pInstanceData && IsEnableCode(code)); house match;
  skip unless APPLY_FORCE==1 or quest in completed map (qm+0x34); if best.quest < row.quest: gate -> best=row,r=0 ; else if qm+0xcc
  non-empty -> best=row, r=2. Then smalls +0xbc same (best.quest < s.quest; gate -> r=1 else if +0xdc nonempty -> r=2; record s).
  r==0: out.rows.push(best), qm+0xcc.push(best). r==1: out.smalls.push(s), qm+0xdc.push(s). return r (init -1).
- 323446c0 ByVillage(village,house,out): rows with house && +0x80==village; gate -> copy r=0 (keep looping); else if +0xcc nonempty
  -> copy r=2. r==0 push both. 
- 32340060 ByQuestId(quest,out): only if quest completed; first row with +0==quest (gate r=0 / applied-nonempty r=2), break; smalls
  first match same (r=1/2) break; push lists as above.
- 32340630 per-quest refresh(quest): if g_pTFieldGame: r=ByQuestId; 0 -> replace(out.rows[0]); 1 -> remove(out.smalls[0].house).
- 32344880 code-event refresh: if g_pTFieldGame: village = loc mgr current village (324f1e10), templet 324f23a0; for each house in
  village house list (+0x20): r=ByQuest(house); if -1 r=ByVillage(village,house); 0 replace(row); 1 remove(small.house);
  2 && (qm+0xcc or +0xdc nonempty) -> restore(house, qm+0xcc).
- 323c21e0 (start of village creation): clears qm+0xcc (destroy range) and qm+0xdc.
## TFieldGame
- 323ba5f0 village creation: loc templet of current village; if qm: 323c21e0; per house: qm? ByQuest -> ByVillage; 0 replace;
  1 nothing; else plain path. Plain path: templet=GetHouseTemplet; code-event gates (2013 & 40091, 2081 & 40129, 2186 & 40089
  -> skip when event OFF for that house; g_pData+0x334 +0x5d & 40090; 40141/40142 need code 31) then AddFieldNpc(templet, L"").
- 323b0980 AddFieldNpc(templet, name): world check vs COMMON (+0x224, world id at +0) or CUSTOM (+0x234 nonempty); create
  (323bdef0), CreateFieldNpc(world TFG+0xa8, templet, name) 325ca980; push TFG+0x368; layer +0x164 = 10; AddObjectChain;
  325c9000(0); if TFG+0x624 && 32ec7230(house) -> 325ca920.
- 323b7ff0 replace(row): remove(row.house); T=new copy(GetHouseTemplet(row.house)) [323c3cf0 ctor + 323c2aa0 copy]; overrides as
  above; AddFieldNpc(T, row.name); TFG+0x444.push(T).
- 323a9ed0 remove(house): each NPC in TFG+0x368 with +0x5b0==house -> 325ca920.

## Parked for E1c: the two refresh functions (CX2QuestManager, already transcribed)

```cpp
/// June 32340630, run by the quest UI right after a completed quest is removed (QUEST_COMPLETE_ACK 323e57e0,
/// ALL_COMPLETED_QUEST_COMPLETE_ACK 323e1710; June's third caller 323e15e0 handles a June-only packet March has no event for)
void CX2QuestManager::RefreshVillageNpcRelationDataByQuest( int iQuestID_ )
{
	if ( NULL == g_pTFieldGame )
		return;

	VillageNpcRelationResult kResult;
	const int iResult = GetVillageNpcRelationDataByQuestID( iQuestID_, kResult );
	if ( 0 == iResult )
	{
		if ( false == kResult.m_vecRelationData.empty() )
			g_pTFieldGame->ReplaceVillageNpc( kResult.m_vecRelationData[0] );
	}
	else if ( 1 == iResult )
	{
		if ( false == kResult.m_vecNotShowData.empty() )
			g_pTFieldGame->RemoveVillageNpc( kResult.m_vecNotShowData[0].m_iTargetHouseID );
	}
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
	CX2OfflineLog::Server( L"[ELYDBG] VillageNpcRelation: quest %d completed -> result %d", iQuestID_, iResult );
#endif SERV_IRUHADEV_ELYSION_DEBUG
}

/// June 32344880, run at the end of Handler_EGS_UPDATE_CODE_EVENT_NOT: every house of the current village is looked up again
/// (ByQuest, then ByVillage on -1); 0 rebuilds, 1 hides, 2 restores what was applied for that house.
void CX2QuestManager::RefreshVillageNpcRelationDataByCodeEvent()
{
	if ( NULL == g_pTFieldGame || NULL == g_pData || NULL == g_pData->GetLocationManager() )
		return;

	SEnum::VILLAGE_MAP_ID eVillageID;
	D3DXVECTOR3 vPos;
	g_pData->GetLocationManager()->GetVillage( eVillageID, vPos );
	CX2LocationManager::VillageTemplet* pVillageTemplet = g_pData->GetLocationManager()->GetVillageMapTemplet( eVillageID );
	if ( NULL == pVillageTemplet )
		return;

	for ( UINT i = 0; i < pVillageTemplet->m_HouseList.size(); ++i )
	{
		if ( NULL == g_pData->GetQuestManager() )
			continue;

		const int iHouseID = static_cast<int>( pVillageTemplet->m_HouseList[i] );
		VillageNpcRelationResult kResult;
		int iResult = GetVillageNpcRelationDataByQuest( iHouseID, kResult );
		if ( -1 == iResult )
			iResult = GetVillageNpcRelationDataByVillage( static_cast<int>( eVillageID ), iHouseID, kResult );

		if ( 0 == iResult )
		{
			if ( false == kResult.m_vecRelationData.empty() )
				g_pTFieldGame->ReplaceVillageNpc( kResult.m_vecRelationData[0] );
		}
		else if ( 1 == iResult )
		{
			if ( false == kResult.m_vecNotShowData.empty() )
				g_pTFieldGame->RemoveVillageNpc( kResult.m_vecNotShowData[0].m_iTargetHouseID );
		}
		else if ( 2 == iResult && ( false == m_kAppliedVillageNpcRelation.m_vecRelationData.empty()
			|| false == m_kAppliedVillageNpcRelation.m_vecNotShowData.empty() ) )
		{
			g_pTFieldGame->RestoreVillageNpc( iHouseID, m_kAppliedVillageNpcRelation );
		}
	}
}
```
