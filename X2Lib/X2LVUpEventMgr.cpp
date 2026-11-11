#include "StdAfx.h"
#include ".\x2lvupeventmgr.h"
#ifdef SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
#include "KTDXWordWrap.h"
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE

CX2LVUpEventMgr::CX2LVUpEventMgr(void)
{
	m_pNowStage = NULL;
	if ( OpenScriptFile( L"LevelUpEvent.lua" ) == false )
	{
		ErrorLog( KEM_ERROR353 );
	}
}

CX2LVUpEventMgr::~CX2LVUpEventMgr(void)
{
	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	for ( mit = m_mapLVUpEvent.begin(); mit != m_mapLVUpEvent.end(); mit++ )
	{
		CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = mit->second;
		SAFE_DELETE( pLVUpEvent );
	}
	m_mapLVUpEvent.clear();
}

void CX2LVUpEventMgr::SetNowStage( CKTDXStage* pStage )
{
	m_pNowStage = pStage;

	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	for ( mit = m_mapLVUpEvent.begin(); mit != m_mapLVUpEvent.end(); mit++ )
	{
		CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = mit->second;
		if ( pLVUpEvent == NULL )
			continue;

		pLVUpEvent->SetNowStage( pStage );
	}

}

void CX2LVUpEventMgr::Reset( int nowLevel )
{
	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	for ( mit = m_mapLVUpEvent.begin(); mit != m_mapLVUpEvent.end(); mit++ )
	{
		CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = mit->second;
		if ( pLVUpEvent == NULL )
			continue;

		pLVUpEvent->Reset( nowLevel );
	}
}

void CX2LVUpEventMgr::OnFrameMove( double fTime, float fElapsedTime )
{
	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	for ( mit = m_mapLVUpEvent.begin(); mit != m_mapLVUpEvent.end(); mit++ )
	{
		CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = mit->second;
		if ( pLVUpEvent == NULL )
			continue;

		pLVUpEvent->OnFrameMove( fTime, fElapsedTime );
	}
}

bool CX2LVUpEventMgr::OpenScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pLVUpEventMgr", this );
	return g_pKTDXApp->LoadLuaTinker( pFileName );
}

void CX2LVUpEventMgr::AddLevelUpEvent_LUA()
{
//
//#ifdef LUA_TEST
//
//	CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = new CX2LVUpEventMgr::LVUpEvent();
//
//	lua_tinker::table luaTable( g_pKTDXApp->GetLuaBinder()->GetLuaState(), -1 );
//	lua_tinker::table luaTable2;
//	lua_tinker::table luaTable3;
//	lua_tinker::table luaTable4;
//
//
//
//	luaTable.get_<CX2LVUpEventMgr::LEVEL_UP_EVENT_ID>( "EVENT_ID", pLVUpEvent->m_EventID, LUEI_NONE );
//
//	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
//	mit = m_mapLVUpEvent.find( pLVUpEvent->m_EventID );
//	if ( mit != m_mapLVUpEvent.end() )
//	{
//		wstringstream wstrstm;
//		wstrstm << pLVUpEvent->m_EventID << L" : LevelUpEventID 중복됨 ";
//		ErrorLogMsg( KEM_ERROR352, wstrstm.str().c_str() );
//#ifndef _SERVICE_
//		MessageBox( g_pKTDXApp->GetHWND(), wstrstm.str().c_str(), L"레벨업 이벤트 매니져 오류", MB_OK );
//#endif
//		SAFE_DELETE( pLVUpEvent );
//		return;
//	}
//
//
//	int typeIndex = 0;
//	while( true == LUA_GET_SUFFIX_NODEFAULT_( luaTable, "TYPE", typeIndex, luaTable2 ) )
//	//while( true == LuaTableGet_<lua_tinker::table>( luaTable, "TYPE", typeIndex, luaTable2 ) )
//	{
//		typeIndex++;
//		CX2LVUpEventMgr::LVUpEventCondNReact* pLVUpEventCondNReact = new CX2LVUpEventMgr::LVUpEventCondNReact();
//
//		if( true == luaTable2.get_<lua_tinker::table>( "COND", luaTable3 ) )
//		{
//			luaTable3.get_( "LEVEL", pLVUpEventCondNReact->m_Level, 9999 );
//
//			if( true == luaTable3.get_<lua_tinker::table>( "UNIT_TYPE", luaTable4 ) )
//			{
//				int tableIndex = 1;
//				int unitTypeID = 0;
//				while( true == luaTable4.get_<int>( tableIndex, unitTypeID ) )
//				{
//					tableIndex++;
//					CX2Unit::UNIT_TYPE unitType = (CX2Unit::UNIT_TYPE)unitTypeID;
//					pLVUpEventCondNReact->m_vecCondUnitType.push_back( unitType );
//				}
//			}
//
//			if( true == luaTable3.get_<lua_tinker::table>( "UNIT_CLASS", luaTable4 ) )
//			{
//				int tableIndex = 1;
//				int unitClassID = 0;
//				while( true == luaTable4.get_<int>( tableIndex, unitClassID ) )
//				{
//					tableIndex++;
//					CX2Unit::UNIT_CLASS unitClass = (CX2Unit::UNIT_CLASS)unitClassID;
//					pLVUpEventCondNReact->m_vecCondUnitClass.push_back( unitClass);
//				}
//
//			}
//
//		}
//
//
//		if( true == luaTable2.get_<lua_tinker::table>( "REACT", luaTable3 ) )
//		{
//			if( true == luaTable3.get_<lua_tinker::table>( "DIALOG_LIST", luaTable4 ) )
//			{
//				int tableIndex = 1;
//				wstring fileName;
//				string strFileName;
//				
//				while( true == luaTable4.get_<string>( tableIndex, strFileName ) )
//				{
//					tableIndex++;
//					ConvertCharToWCHAR( fileName, strFileName.c_str() );
//					
//					pLVUpEventCondNReact->m_vecReactDialogFile.push_back( fileName );
//				}
//			}
//		}
//
//		pLVUpEvent->m_vecCondNReact.push_back( pLVUpEventCondNReact );
//	}
//
//
//#else


	CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = new CX2LVUpEventMgr::LVUpEvent();

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	LUA_GET_VALUE_ENUM(	luaManager, "EVENT_ID",	pLVUpEvent->m_EventID, CX2LVUpEventMgr::LEVEL_UP_EVENT_ID, LUEI_NONE );
	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	mit = m_mapLVUpEvent.find( pLVUpEvent->m_EventID );
	if ( mit != m_mapLVUpEvent.end() )
	{
		wstringstream wstrstm;
		wstrstm << pLVUpEvent->m_EventID << L" " << GET_STRING( STR_ID_344 ) << L" ";
		ErrorLogMsg( KEM_ERROR352, wstrstm.str().c_str() );
#ifndef _SERVICE_
		MessageBox( g_pKTDXApp->GetHWND(), wstrstm.str().c_str(), L"레벨업 이벤트 매니져 오류", MB_OK );
#endif
		SAFE_DELETE( pLVUpEvent );
		return;
	}

	
	int typeIndex = 0;

	
	while( luaManager.BeginTable( "TYPE", typeIndex ) == true )
	{
		typeIndex++;
		CX2LVUpEventMgr::LVUpEventCondNReact* pLVUpEventCondNReact = new CX2LVUpEventMgr::LVUpEventCondNReact();

		if( luaManager.BeginTable( "COND" ) == true )
		{
			LUA_GET_VALUE(	luaManager, "LEVEL",	pLVUpEventCondNReact->m_Level, 9999 );

			if( luaManager.BeginTable( "UNIT_TYPE" ) == true )
			{
				int tableIndex = 1;
				int unitTypeID = 0;
				while( luaManager.GetValue( tableIndex, unitTypeID ) == true )
				{
					tableIndex++;
					CX2Unit::UNIT_TYPE unitType = (CX2Unit::UNIT_TYPE)unitTypeID;
					pLVUpEventCondNReact->m_vecCondUnitType.push_back( unitType );
				}

				luaManager.EndTable();
			}

			if( luaManager.BeginTable( "UNIT_CLASS" ) == true )
			{
				int tableIndex = 1;
				int unitClassID = 0;
				while( luaManager.GetValue( tableIndex, unitClassID ) == true )
				{
					tableIndex++;
					CX2Unit::UNIT_CLASS unitClass = (CX2Unit::UNIT_CLASS)unitClassID;
					pLVUpEventCondNReact->m_vecCondUnitClass.push_back( unitClass);
				}

				luaManager.EndTable();
			}

			luaManager.EndTable();
		}

		if( luaManager.BeginTable( "REACT" ) == true )
		{
			if( luaManager.BeginTable( "DIALOG_LIST" ) == true )
			{
				int tableIndex = 1;
				wstring fileName;
				while( luaManager.GetValue( tableIndex, fileName ) == true )
				{
					tableIndex++;
					pLVUpEventCondNReact->m_vecReactDialogFile.push_back( fileName );
				}

				luaManager.EndTable();
			}

#ifdef ADD_PLAY_SOUND //김창한
			if( luaManager.BeginTable( "SOUND_LIST" ) == true )
			{
				int tableIndex = 1;
				wstring fileName;
				while( luaManager.GetValue( tableIndex, fileName ) == true )
				{
					tableIndex++;
					pLVUpEventCondNReact->m_vecSoundFileName.push_back( fileName );
				}

				luaManager.EndTable();
			}
#endif //ADD_PLAY_SOUND

			luaManager.EndTable();
		}

		pLVUpEvent->m_vecCondNReact.push_back( pLVUpEventCondNReact );

		luaManager.EndTable();
	}


//#endif LUA_TEST_TEST

	m_mapLVUpEvent.insert( std::make_pair( pLVUpEvent->m_EventID, pLVUpEvent ) );
}

#ifdef SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
// char-ara CHNb2: what June's guide fill ( 32713c10 ) and its 1551 / 1552 cases use
namespace _CONST_LVUP_EVENT_CHNB2_
{
	// June 32207560 ( the skill tree UI keeps the same switch file-local )
	const WCHAR* GetSkillTypeBGKey( const int iSkillType_ )
	{
		switch ( iSkillType_ )
		{
		case CX2SkillTree::ST_BUFF:
			return L"SKILL_BG_ACTIVE_BUFF";
		case CX2SkillTree::ST_ACTIVE:
		case CX2SkillTree::ST_RELATIONSHIP_SKILL:
			return L"SKILL_BG_ACTIVE";
		case CX2SkillTree::ST_SPECIAL_ACTIVE:
		case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_1:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_2:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_3:
			return L"SKILL_BG_ACTIVE_SPECIAL";
		default:
			return L"SKILL_BG_PASSIVE";
		}
	}

	// June 31c94970 ( the skill tree UI keeps the same helper file-local ): a picture with no point gets one sized to its texture
	void SetPointByTextureSize( CKTDGUIControl::CPictureData* pPicture_ )
	{
		if ( NULL == pPicture_ || NULL != pPicture_->pPoint || NULL == pPicture_->pTexture )
			return;

		pPicture_->pPoint = new CKTDGUIControl::UIPointData();
		pPicture_->pPoint->pUITextureData = pPicture_->pTexture;
		pPicture_->pPoint->SetAutoPointByTextureSize();
		pPicture_->pos			= pPicture_->pPoint->leftTopPoint;
		pPicture_->originalPos	= pPicture_->pos;
		pPicture_->size			= pPicture_->pTexture->texSize;
		pPicture_->originalSize	= pPicture_->size;
	}

	// June 31c9ddf0 + the two stores: a button point at one corner
	CKTDGUIControl::UIPointData* NewPointAt( const float fX_, const float fY_ )
	{
		CKTDGUIControl::UIPointData* pPoint = new CKTDGUIControl::UIPointData();
		pPoint->leftTopPoint = D3DXVECTOR2( fX_, fY_ );
		return pPoint;
	}

	// June 327127a0 cases 1551 / 1552 and 32713420: string 1 of the class skill description wrapped per character to 320
	void WrapClassSkillDescription( CKTDGUIStatic* pDescription_ )
	{
		CKTDGUIControl::UIStringData* pString = pDescription_->GetString( 1 );
		if ( NULL == pString )
			return;

		const std::wstring wstrText = pString->msg;
		pString->msg = CKTDXWordWrap::GetStrByLineBreakColorByChar( wstrText.c_str(), 320, pString->fontIndex, 0, false );
	}

	// June 32712e50: per MAJOR_COMBO row a MajorCommandBack_%d button at ( 262, y - 7 ) and a MajorCommand_%d static ( title at ( 268, y + 1 ),
	// icons from x 352 ), rows 40 px apart from y 360
	int CreateClassCommandDescription( CKTDGUIDialogType pDialog_, const std::vector<CX2SkillTree::ComboRow>& vecRow_ )
	{
		if ( NULL == pDialog_ || true == vecRow_.empty() )
			return 0;

		WCHAR buf[255] = { 0, };
		float fX = 352.f;
		float fY = 360.f;
		for ( UINT i = 0; i < vecRow_.size(); ++i )
		{
			const CX2SkillTree::ComboRow& kRow = vecRow_[i];

			CKTDGUIButton* pButton = new CKTDGUIButton();
			StringCchPrintf( buf, 255, L"MajorCommandBack_%d", i );
			pButton->SetName( buf );
			pDialog_->AddControl( pButton );
			pButton->SetNormalTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
			pButton->SetOverTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
			pButton->SetDownTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
			pButton->AddDummyInt( kRow.m_iStringID );
			pButton->SetNormalPoint( NewPointAt( 262.f, fY - 7.f ) );
			pButton->SetOverPoint( NewPointAt( 262.f, fY - 7.f ) );
			pButton->SetDownPoint( NewPointAt( 262.f, fY - 7.f ) );

			CKTDGUIStatic* pStatic = new CKTDGUIStatic();
			StringCchPrintf( buf, 255, L"MajorCommand_%d", i );
			pStatic->SetName( buf );
			pDialog_->AddControl( pStatic );

			CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
			pStatic->AddString( pString );
			pString->fontIndex	= 2;
			pString->color		= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
			pString->msg		= GET_STRING( CX2SkillTree::GetComboTitleStringID( kRow.m_iComboType ) );
			pString->sortFlag	= 0;
			pString->fontStyle	= CKTDGFontManager::FS_NONE;
			pString->pos		= D3DXVECTOR2( 268.f, fY + 1.f );

			for ( UINT j = 0; j < kRow.m_vecIcon.size(); ++j )
			{
				if ( 0 != j )
				{
					CKTDGUIControl::CPictureData* pLast = pStatic->GetPicture( pStatic->GetPictureNum() - 1 );
					if ( NULL != pLast )
						fX = pLast->GetPos().x + pLast->GetSize().x + 2.f;
				}
				g_pData->GetSkillTree()->AddCommandIconPicture( kRow.m_vecIcon[j], fX, fY, pStatic );
			}

			fX = 352.f;
			fY += 40.f;
		}
		return static_cast<int>( vecRow_.size() );
	}

	// June 32713420: the 2ND guide's skill icons ( SKILL_DATA, TRANSCENDENTAL_SKILL_DATA when transcended ), 86 px apart from ( 260, 230 ), then the command rows
	void CreateClassSkillDescription( CKTDGUIDialogType pDialog_, const int iNextUnitClass_, CX2SkillTree::SkillCommandDesc& kDesc_ )
	{
		if ( NULL == pDialog_ || NULL == g_pData->GetSkillTree() )
			return;

		if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
			 0 != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iUnitClassTran && false == kDesc_.m_kTranscendental.m_vecSkill.empty() )
			kDesc_.m_kSkill = kDesc_.m_kTranscendental;		// unlike the skill tree's 328e42d0, June tests no instance code here

		CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( pDialog_->GetControl( L"g_pStatic_ClassSkillDescription" ) );
		if ( NULL != pDescription )
		{
			const int iDescStringID = kDesc_.m_kSkill.m_iDescStringID;
			pDescription->SetString( 1, GET_STRING( iDescStringID ) );
			pDescription->AddDummyInt( iDescStringID );		// 1552 reads it back
		}

		WCHAR buf[255] = { 0, };
		int iIconCount = 0;
		const std::vector< std::pair<int, int> >& vecSkill = kDesc_.m_kSkill.m_vecSkill;
		for ( UINT i = 0; i < vecSkill.size(); ++i )
		{
			// 32206450( id, 1 ): its level gate needs instance code 0x910 / 0x911 ( none offline ), so it is March's GetSkillTemplet( id )
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( vecSkill[i].first ) );
			if ( NULL == pSkillTemplet )
				continue;

			CKTDGUIStatic* pStatic = new CKTDGUIStatic();
			CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
			CKTDGUIButton* pButton = new CKTDGUIButton();
			pDialog_->AddControl( pStatic );
			pDialog_->AddControl( pButton );
			StringCchPrintf( buf, 255, L"MajorSkillBack_%d", i );
			pStatic->SetName( buf );
			StringCchPrintf( buf, 255, L"MajorSkillButton_%d", i );
			pButton->SetName( buf );

			pPicture->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", GetSkillTypeBGKey( static_cast<int>( pSkillTemplet->m_eType ) ) );
			pStatic->AddPicture( pPicture );
			pButton->SetNormalTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
			pButton->SetOverTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
			pButton->SetDownTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
			pButton->AddDummyInt( vecSkill[i].second );		// 1551 reads it
			pButton->SetCustomMsgMouseOver( 1551 );		// June's mouse-over message; no Enum entry
			pButton->SetCustomMsgMouseOut( 1552 );		// June's mouse-out message; no Enum entry

			const float fX = static_cast<float>( i ) * 86.f + 260.f;
			SetPointByTextureSize( pPicture );
			pPicture->SetPos( D3DXVECTOR2( fX, 230.f ) );
			pButton->SetNormalPoint( NewPointAt( fX + 3.f, 233.f ) );
			pButton->SetOverPoint( NewPointAt( fX + 3.f, 233.f ) );
			pButton->SetDownPoint( NewPointAt( fX + 3.f, 233.f ) );
			++iIconCount;
		}

		// June also needs app global 337676ec here, set at start-up ( 31c18eb6 )
		pDescription = static_cast<CKTDGUIStatic*>( pDialog_->GetControl( L"g_pStatic_ClassSkillDescription" ) );
		if ( NULL != pDescription )
			WrapClassSkillDescription( pDescription );

		std::vector<CX2SkillTree::ComboRow> vecRow;
		g_pData->GetSkillTree()->LoadMajorComboRows( &kDesc_, CX2SkillTree::GetUnitClassName( iNextUnitClass_ ), false, vecRow );
		const int iRowCount = CreateClassCommandDescription( pDialog_, vecRow );

		CX2OfflineLog::Server( L"[JUNE_CHAR] CLASS GUIDE 2nd skills: next class %d, description %d, %d skill icon(s), %d command row(s)",
			iNextUnitClass_, kDesc_.m_kSkill.m_iDescStringID, iIconCount, iRowCount );
	}
}
using namespace _CONST_LVUP_EVENT_CHNB2_;

#endif SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
bool CX2LVUpEventMgr::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
	// char-ara CHNb: June 327127a0 ( cases 1551 / 1552: char-ara CHNb2 )
	switch( wParam )
	{
	case LUEMCUM_PVP_PR:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_PVP_PR );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_CHANGE_CLASS_GUIDE_FIRST:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_CHANGE_CLASS_GUIDE_FIRST );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_10LV_PR:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_LEVEL_10 );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_ELSWORD_SWORD_KNIGHT:
	case LUEMCUM_ELSWORD_MAGIC_KNIGHT:
	case LUEMCUM_AISHA_HIGH_MAGICIAN:
	case LUEMCUM_AISHA_DARK_MAGICIAN:
	case LUEMCUM_RENA_SNIPING_RANGER:
	case LUEMCUM_RENA_COMBAT_RANGER:
	case LUEMCUM_RAVEN_SWORD_TAKER:
	case LUEMCUM_RAVEN_OVER_TAKER:
	case LUEMCUM_EVE_CODE_EXOTIC:
	case LUEMCUM_EVE_CODE_ARCHITECTURE:
	case LUEMCUM_CHUNG_FURY_GUARDIAN:
	case LUEMCUM_CHUNG_SHOOTING_GUARDIAN:
	case LUEMCUM_ELSWORD_SHEATH_KNIGHT:
	case LUEMCUM_ARME_BATTLE_MAGICIAN:
	case LUEMCUM_LIRE_TRAPPING_RANGER:
	case LUEMCUM_RAVEN_WEAPON_TAKER:
	case LUEMCUM_EVE_ELECTRA:
	case LUEMCUM_CHUNG_SHELLING_GUARDIAN:
	case LUEMCUM_ARA_LITTLE_HSIEN:
	case LUEMCUM_ARA_LITTLE_DEVIL:
	case LUEMCUM_ELESIS_SABER_KNIGHT:
	case LUEMCUM_ELESIS_PYRO_KNIGHT:
	case LUEMCUM_ADD_PSYCHIC_TRACER:
	case LUEMCUM_ADD_ARC_TRACER:
	case LUEMCUM_ARA_LITTLE_SPECTER:
	case LUEMCUM_ELESIS_DARK_KNIGHT:
	case LUEMCUM_ADD_TIME_TRACER:
	case LUEMCUM_LUCIEL_CHILIARCH:
	case LUEMCUM_LUCIEL_ROYAL_GUARD:
	case LUEMCUM_ROSE_HEAVYBARREL:
	case LUEMCUM_ROSE_BLOODIA:
	case LUEMCUM_ROSE_VALKYRY:
	case LUEMCUM_ROSE_METALHEART:
	case LUEMCUM_LUCIEL_DIABLURA:
	case LUEMCUM_AIN_LOFTY_EXACTER:
	case LUEMCUM_AIN_LOFTY_ANPASSEN:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_CHANGE_CLASS_GUIDE_FIRST );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();

			// June calls the now state's ToggleCashShop ( vslot 0xac ) with no state test
			CX2StateMenu* pStateMenu = static_cast< CX2StateMenu* >( g_pMain->GetNowState() );
			if( NULL != pStateMenu )
			{
				if( NULL != g_pTFieldGame )
					g_pTFieldGame->CloseFieldName( 0.0f );

				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( (int)wParam );
				pStateMenu->ToggleCashShop();
			}
		} break;
	case LUEMCUM_SECRET_DUNGEON_OPEN_GUIDE:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_SECRET_DUNGEON_OPEN_GUIDE );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_TRANSCENDENTAL:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_CHANGE_CLASS_GUIDE_SECOND:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_CHANGE_CLASS_GUIDE_SECOND );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_1LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( CX2QuestManager::QT_CHANGE_JOB, CX2QuestManager::CJT_JOB_FIRST );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_2LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( CX2QuestManager::QT_CHANGE_JOB, CX2QuestManager::CJT_JOB_SECOND );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_3LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( CX2QuestManager::QT_CHANGE_JOB, CX2QuestManager::CJT_JOB_THIRD );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_4LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( CX2QuestManager::QT_CHANGE_JOB, CX2QuestManager::CJT_JOB_FOURTH );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_1LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
				g_pData->GetCashShop()->OpenClassChangeGuideItem( CX2Unit::UCL_FIRST );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_2LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
				g_pData->GetCashShop()->OpenClassChangeGuideItem( CX2Unit::UCL_SECOND );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_3LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
				g_pData->GetCashShop()->OpenClassChangeGuideItem( CX2Unit::UCL_THIRD );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_4LINE:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
				g_pData->GetCashShop()->OpenClassChangeGuideItem( 4 );		// line 4 ( Rose / Ain ): UNIT_CLASS_LINE has no member for it
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_TRANCENDENTAL:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( CX2QuestManager::QT_CHANGE_TRANSCENDENTAL_CLASS, CX2QuestManager::CJT_JOB_NONE );
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_TRANCENDENTAL:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
				g_pData->GetCashShop()->OpenClassChangeGuideItem( CX2Unit::UCL_NONE );
		} return true;
	case 1551:		// char-ara CHNb2: a 2ND guide skill icon's mouse-over ( 32713420 binds it; no Enum entry ): the icon's text
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>( lParam );
			if ( NULL != pControl && NULL != pControl->GetDialog() )
			{
				CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( pControl->GetDialog()->GetControl( L"g_pStatic_ClassSkillDescription" ) );
				if ( NULL != pDescription )
				{
					const int iStringID = pControl->GetDummyInt( 0 );
					pDescription->SetString( 1, GET_STRING( iStringID ) );
					WrapClassSkillDescription( pDescription );
				}
			}
		} return true;
	case 1552:		// char-ara CHNb2: its mouse-out ( no Enum entry ): the description's own text, its dummy int 0
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>( lParam );
			if ( NULL != pControl && NULL != pControl->GetDialog() )
			{
				CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( pControl->GetDialog()->GetControl( L"g_pStatic_ClassSkillDescription" ) );
				if ( NULL != pDescription )
				{
					const int iStringID = pDescription->GetDummyInt( 0 );
					pDescription->SetString( 1, GET_STRING( iStringID ) );
					WrapClassSkillDescription( pDescription );
				}
			}
		} return true;
	}
	return false;
#else SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
	switch( wParam )
	{
	case LUEMCUM_PVP_PR:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( CX2LVUpEventMgr::LUEI_PVP_PR );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
				
		}
		break;

	case LUEMCUM_CHANGE_CLASS_GUIDE_FIRST:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( CX2LVUpEventMgr::LUEI_CHANGE_CLASS_GUIDE_FIRST );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		}
		break;
	case LUEMCUM_10LV_PR:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( CX2LVUpEventMgr::LUEI_LEVEL_10 );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		}
		break;
#ifdef LINK_CASH_SHOP_WHEN_JOB_CHANGE
	case LUEMCUM_ELSWORD_SWORD_KNIGHT:
	case LUEMCUM_ELSWORD_MAGIC_KNIGHT:
	case LUEMCUM_AISHA_HIGH_MAGICIAN:
	case LUEMCUM_AISHA_DARK_MAGICIAN:
	case LUEMCUM_RENA_SNIPING_RANGER:
	case LUEMCUM_RENA_COMBAT_RANGER:
	case LUEMCUM_RAVEN_SWORD_TAKER:
	case LUEMCUM_RAVEN_OVER_TAKER:
	case LUEMCUM_EVE_CODE_EXOTIC:
	case LUEMCUM_EVE_CODE_ARCHITECTURE:
	case LUEMCUM_CHUNG_FURY_GUARDIAN:
	case LUEMCUM_CHUNG_SHOOTING_GUARDIAN:
		//	1차 추가 전직
	case LUEMCUM_ELSWORD_SHEATH_KNIGHT:
	case LUEMCUM_ARME_BATTLE_MAGICIAN:
	case LUEMCUM_LIRE_TRAPPING_RANGER:
	case LUEMCUM_RAVEN_WEAPON_TAKER:
	case LUEMCUM_EVE_ELECTRA:
	case LUEMCUM_CHUNG_SHELLING_GUARDIAN:
	case LUEMCUM_ARA_LITTLE_HSIEN:
#ifdef SERV_ARA_CHANGE_CLASS_SECOND
	case LUEMCUM_ARA_LITTLE_DEVIL:
#endif SERV_ARA_CHANGE_CLASS_SECOND
#ifdef NEW_CHARACTER_EL
	case LUEMCUM_ELESIS_SABER_KNIGHT:
	case LUEMCUM_ELESIS_PYRO_KNIGHT:
#endif NEW_CHARACTER_EL
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( CX2LVUpEventMgr::LUEI_CHANGE_CLASS_GUIDE_FIRST );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();

			CX2StateField*	pStateField	=	NULL;
			pStateField		=  static_cast< CX2StateField* >( g_pMain->GetNowState() );

			if( pStateField != NULL )
			{
				if( g_pTFieldGame != NULL )
				{
					g_pTFieldGame->CloseFieldName( 0.0f );
				}

				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( (int)wParam );
				pStateField->ToggleCashShop();
			}
		} break;
#endif LINK_CASH_SHOP_WHEN_JOB_CHANGE
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	// June 32712790 cases 1537 / 1549 / 1550
	case LUEMCUM_TRANSCENDENTAL:
		{
			CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL );
			if ( pLVUpEvent != NULL )
				pLVUpEvent->DeleteDialog();
		} break;
	case LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_TRANCENDENTAL:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetQuestManager() )
				g_pData->GetQuestManager()->GuideAcceptClassUpQuest( 9, 0 );		// 32344ab0( 9, 0 )
		} return true;
	case LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_TRANCENDENTAL:
		{
			CloseChangeClassGuides();
			if( NULL != g_pData->GetCashShop() )
			{
				// 3222a4f0( 0 ): the calling menu is 0x1d, the item 288319 ( 0x4663f ) is the one the shop opens on, then the state toggles it open
				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( 0x1d );
				g_pData->GetCashShop()->PushOpenAtItemID( 288319 );
				switch( g_pMain->GetNowStateID() )		// June's list; TRAINING / ARCADE_RESULT are March's other non-menu states
				{
				case CX2Main::XS_START_UP: case CX2Main::XS_LOGIN: case CX2Main::XS_JOIN: case CX2Main::XS_LOADING:
				case CX2Main::XS_SERVER_SELECT: case CX2Main::XS_CREATE_UNIT: case CX2Main::XS_PVP_GAME:
				case CX2Main::XS_PVP_RESULT: case CX2Main::XS_DUNGEON_RESULT:
				case CX2Main::XS_TRAINING_RESULT: case CX2Main::XS_ARCADE_RESULT:
					break;
				default:
					{
						CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
						if( NULL != pStateMenu )
							pStateMenu->ToggleCashShop();
					} break;
				}
			}
		} return true;
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	}
	return false;
#endif SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
}

#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
void CX2LVUpEventMgr::CloseChangeClassGuides()
{
	const LEVEL_UP_EVENT_ID aeEvent[3] = { LUEI_CHANGE_CLASS_GUIDE_FIRST, LUEI_CHANGE_CLASS_GUIDE_SECOND, LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL };
	for( int i = 0; i < 3; ++i )
	{
		CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = GetLVUpEvent( aeEvent[i] );
		if ( pLVUpEvent != NULL )
			pLVUpEvent->DeleteDialog();
	}
}
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
CX2LVUpEventMgr::LVUpEvent* CX2LVUpEventMgr::GetLVUpEvent( LEVEL_UP_EVENT_ID eventID )
{
	CX2LVUpEventMgr::LVUpEvent* pLVUpEvent = NULL;
	map< LEVEL_UP_EVENT_ID, LVUpEvent* >::iterator mit;
	mit = m_mapLVUpEvent.find( eventID );
	if ( mit != m_mapLVUpEvent.end() )
		pLVUpEvent = mit->second;

	return pLVUpEvent;
}

CX2LVUpEventMgr::LVUpEvent::LVUpEvent()
{
	m_LastCheckLevel = 0;
	m_EventID = LUEI_NONE;
	m_bCheckShow = false;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	m_fTranscendImageTime = 0.0f;
	m_pTranscendImage = NULL;
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
}
CX2LVUpEventMgr::LVUpEvent::~LVUpEvent()
{
	DeleteDialog();

	for ( int i = 0; i < (int)m_vecCondNReact.size(); i++ )
	{
		LVUpEventCondNReact* pLVUpEventCondNReact = m_vecCondNReact[i];
		SAFE_DELETE( pLVUpEventCondNReact );
	}
	m_vecCondNReact.clear();
}

void CX2LVUpEventMgr::LVUpEvent::Reset( int nowLevel )
{
	m_LastCheckLevel = nowLevel;
	m_bCheckShow = false;
}

void CX2LVUpEventMgr::LVUpEvent::SetNowStage( CKTDXStage* pStage )
{
	for ( int i = 0; i < (int)m_vecDialog.size(); i++ )
	{
		CKTDGUIDialogType pDialog = m_vecDialog[i];
		if ( pDialog == NULL )
			continue;
		
		pDialog->SetStage( pStage );
	}
}

void CX2LVUpEventMgr::LVUpEvent::OnFrameMove( double fTime, float fElapsedTime )
{
	//이제 여기서 검사해서 다이얼로그 자동으로 띄워져부럴까~~~?
	CheckLevel();
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	CX2ClassChangePopup::FadeTranscendImage( m_pTranscendImage, m_fTranscendImageTime, fElapsedTime );		// June 32714ad0 -> 327108b0
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
}

void CX2LVUpEventMgr::LVUpEvent::DeleteDialog()
{
	for ( int i = 0; i < (int)m_vecDialog.size(); i++ )
	{
		CKTDGUIDialogType pDialog = m_vecDialog[i];
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pDialog, NULL, false );
	}
	m_vecDialog.clear();
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	m_pTranscendImage = NULL;		// June 32710ad0 clears +0x30, not the timer
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
}

#ifdef SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
// char-ara CHNb2: June 32713c10, run on each dialog CheckLevel opens ( after the transcendence init, as June 327141e0 )
void CX2LVUpEventMgr::LVUpEvent::InitClassChangeGuide( CKTDGUIDialogType pDialog_ )
{
	if ( NULL == pDialog_ )
		return;

	switch ( m_EventID )
	{
	case LUEI_CHANGE_CLASS_GUIDE_FIRST:
		{
			// stand-in: GetControl, not June's GetStatic_LUA, whose missing-control popup March's other 1ST guides would raise
			CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( pDialog_->GetControl( L"g_pStatic_String_ClassDescription" ) );
			if ( NULL == pDescription )
			{
				CX2OfflineLog::Server( L"[JUNE_CHAR] CLASS GUIDE 1st: no g_pStatic_String_ClassDescription ( a March guide ), nothing wrapped" );
				return;
			}
			int iWrapped = 0;
			for ( int i = 0; i < 4; ++i )
			{
				CKTDGUIControl::UIStringData* pString = pDescription->GetString( i );
				if ( NULL == pString )
					continue;
				const std::wstring wstrText = pString->msg;
				pString->msg = CKTDXWordWrap::GetStrByLineBreak( wstrText.c_str(), 180, pString->fontIndex, NULL );
				++iWrapped;
			}
			CX2OfflineLog::Server( L"[JUNE_CHAR] CLASS GUIDE 1st: %d class description(s) wrapped to 180", iWrapped );
		} break;

	case LUEI_CHANGE_CLASS_GUIDE_SECOND:
		{
			if ( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() || NULL == g_pData->GetUnitManager() )
				return;

			const CX2Unit::UnitTemplet* pUnitTemplet = g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet();
			const int iUnitClass = ( NULL != pUnitTemplet ) ? static_cast<int>( pUnitTemplet->m_UnitClass ) : CX2Unit::UC_NONE;
			const int iUnitType = ( NULL != pUnitTemplet ) ? static_cast<int>( pUnitTemplet->m_UnitType ) : CX2Unit::UT_NONE;
			const int iLine = GetUnitClassLineByUnitClass( iUnitClass );
			const int iNextUnitClass = GetUnitClassByTierAndLine( iUnitType, GetUnitClassTierByUnitClass( iUnitClass ) + 1, iLine );
			const CX2Unit::UnitTemplet* pNextTemplet = g_pData->GetUnitManager()->GetUnitTemplet( static_cast<CX2Unit::UNIT_CLASS>( iNextUnitClass ) );
			if ( NULL == pNextTemplet )
			{
				CX2OfflineLog::Server( L"[JUNE_CHAR] CLASS GUIDE 2nd: class %d line %d has no next class templet ( %d ), guide left as scripted", iUnitClass, iLine, iNextUnitClass );
				return;
			}

			CKTDGUIStatic* pCharacterImage = static_cast<CKTDGUIStatic*>( pDialog_->GetControl( L"g_pStatic_CharacterImg" ) );
			CKTDGUIButton* pAccept = static_cast<CKTDGUIButton*>( pDialog_->GetControl( L"g_pButton_Accept_Quest1" ) );
			CKTDGUIButton* pBuy = static_cast<CKTDGUIButton*>( pDialog_->GetControl( L"g_pButton_Buy_CashItem1" ) );
			const int iLineIndex = iLine - CX2Unit::UCL_FIRST;
			if ( NULL != pCharacterImage && NULL != pAccept && NULL != pBuy && 0 <= iLineIndex && iLineIndex < 4 )		// June's switch: lines 1-4
			{
				CKTDGUIControl::CPictureData* pPicture = pCharacterImage->GetPicture( iLineIndex );
				if ( NULL != pPicture )
					pPicture->SetShow( true );
				pAccept->SetCustomMsgMouseUp( LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_1LINE + iLineIndex );
				pBuy->SetCustomMsgMouseUp( LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_1LINE + iLineIndex );
			}

			CKTDGUIStatic* pClassName = static_cast<CKTDGUIStatic*>( pDialog_->GetControl( L"g_pStatic_String_ClassName" ) );
			if ( NULL != pClassName && NULL != pClassName->GetString( 0 ) )
				pClassName->GetString( 0 )->msg = pNextTemplet->m_Name;

			CX2OfflineLog::Server( L"[JUNE_CHAR] CLASS GUIDE 2nd: class %d line %d -> next class %d, buttons %d / %d", iUnitClass, iLine, iNextUnitClass,
				LUEMCUM_CHANGE_CLASS_GUIDE_QUEST_ACCEPT_1LINE + iLineIndex, LUEMCUM_CHANGE_CLASS_GUIDE_BUY_CASH_ITEM_1LINE + iLineIndex );

			if ( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() && NULL != g_pData->GetSkillTree() )
			{
				CX2SkillTree::SkillCommandDesc kDesc;
				g_pData->GetSkillTree()->LoadSkillCommandDescription( iNextUnitClass, false, &kDesc );		// stand-in: June's minor flag is CX2Unit +0xe40 ( Luciel, not ported )
				CreateClassSkillDescription( pDialog_, iNextUnitClass, kDesc );
			}
		} break;

	case LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL:
		{
			CKTDGUIButton* pClose = static_cast<CKTDGUIButton*>( pDialog_->GetControl( L"ButtonClass_Change_X" ) );
			if ( NULL != pClose )
				pClose->SetShow( false );		// June button vslot 0x68 ( 31cacc00 ) with 0
		} break;

	default:
		break;
	}
}
#endif SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
void CX2LVUpEventMgr::LVUpEvent::CheckLevel()
{
	if ( m_bCheckShow == true )
		return;

	if ( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

		for ( int i = 0; i < (int)m_vecCondNReact.size(); i++ )
		{
			CX2LVUpEventMgr::LVUpEventCondNReact* pLVUpEventCondNReact = m_vecCondNReact[i];
			if ( pLVUpEventCondNReact == NULL )
				continue;

			if ( pLVUpEventCondNReact->m_Level > m_LastCheckLevel &&
				pLVUpEventCondNReact->m_Level <= pUnit->GetUnitData().m_Level )
			{
				m_bCheckShow = true;

				bool bCheck = false;

				for ( int i = 0; i < (int)pLVUpEventCondNReact->m_vecCondUnitType.size(); i++ )
				{
					CX2Unit::UNIT_TYPE unitType = pLVUpEventCondNReact->m_vecCondUnitType[i];
					if ( unitType == CX2Unit::UT_NONE )
					{
						bCheck = true;
						break;
					}

					if ( pUnit->GetUnitTemplet() != NULL &&
						pUnit->GetUnitTemplet()->m_UnitType == unitType )
					{
						bCheck = true;
						break;
					}
				}

				if ( bCheck == false )
				{
					for ( int i = 0; i < (int)pLVUpEventCondNReact->m_vecCondUnitClass.size(); i++ )
					{
						CX2Unit::UNIT_CLASS unitClass = pLVUpEventCondNReact->m_vecCondUnitClass[i];
						if ( unitClass == CX2Unit::UC_NONE )
						{
							bCheck = true;
							break;
						}

						if ( pUnit->GetUnitTemplet() != NULL &&
							pUnit->GetUnitTemplet()->m_UnitClass == unitClass )
						{
							bCheck = true;
							break;
						}
					}
				}
				
				

				if ( bCheck == true )
				{
					DeleteDialog();

					for ( int i = 0; i < (int)pLVUpEventCondNReact->m_vecReactDialogFile.size(); i++ )
					{
						wstring dialogFileName = pLVUpEventCondNReact->m_vecReactDialogFile[i];
						CKTDGUIDialogType pDialog = new CKTDGUIDialog( g_pMain->GetNowState(), dialogFileName.c_str() );
						g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

						m_vecDialog.push_back( pDialog );
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
						// June 327141e0: a transcendence guide file shows my class's picture ( 32711490 name test, 32710710 init )
						if( CX2ClassChangePopup::IsTranscendGuideName( dialogFileName.c_str() ) )
							CX2ClassChangePopup::InitTranscendGuide( pDialog, static_cast<int>( pUnit->GetClass() ), false, m_pTranscendImage, m_fTranscendImageTime );
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
#ifdef SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
						InitClassChangeGuide( pDialog );		// char-ara CHNb2: June 327141e0 calls 32713c10 right after 32710710
#endif SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE
					}

#ifdef ADD_PLAY_SOUND //김창한
					if( !pLVUpEventCondNReact->m_vecSoundFileName.empty() && NULL != g_pKTDXApp->GetDeviceManager() )
					{
						int iSize = pLVUpEventCondNReact->m_vecSoundFileName.size();
						int iRandom = RandomInt() % iSize;

						g_pKTDXApp->GetDeviceManager()->PlaySound(pLVUpEventCondNReact->m_vecSoundFileName[iRandom].c_str(), false, false );
					}
#endif //ADD_PLAY_SOUND

					break;
				}
			}
		}

		
		m_LastCheckLevel = g_pData->GetSelectUnitLevel();
	}
}