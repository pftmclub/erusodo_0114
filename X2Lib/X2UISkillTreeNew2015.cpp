#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_UI
/// Not a compile unit: X2UISkillTreeNew.cpp includes this file after StdAfx.h ( JUNE_SKILL_SYSTEM_PHASES.md S7b )
//////////////////////////////////////////////////////////////////////////
// June 2017's skill tree UI, CX2UISkillTreeNew2015 (RTTI 3373ca34, class block 328d98e0-328ee680, 103 functions;
// june2017_re/s0b_ui_funcmap.tsv). Ported under March's class name so that every caller of
// CX2UIManager::GetUISkillTree() compiles unchanged. March's class (X2UISkillTreeNew.cpp) is compiled out by the flag.
// Phase S7b of JUNE_SKILL_SYSTEM_PHASES.md. Memo popups and memo packets are S7c's, the page packets S7d's: their
// UICustomEventProc cases are named stubs here.
//////////////////////////////////////////////////////////////////////////

#ifdef UPGRADE_SKILL_SYSTEM_2013

#include "Offline/X2OfflineLog.h"

namespace
{
	/// June 32207560: the skill-type background key in DLG_UI_Common_Texture_NewSkill_02.TGA
	const WCHAR* JuneGetSkillTypeBGKey( const int iSkillType_ )
	{
		switch ( iSkillType_ )
		{
		case CX2SkillTree::ST_BUFF:
			return L"SKILL_BG_ACTIVE_BUFF";
		case CX2SkillTree::ST_ACTIVE:
		case CX2SkillTree::ST_RELATIONSHIP_SKILL:
			return L"SKILL_BG_ACTIVE";
		case CX2SkillTree::ST_SPECIAL_ACTIVE:
		case 13:	/// ST_HYPER_ACTIVE_SKILL
		case 14:	/// ST_SPECIAL_ACTIVE_GRADE_1
		case 15:	/// ST_SPECIAL_ACTIVE_GRADE_2
		case 16:	/// ST_SPECIAL_ACTIVE_GRADE_3
			return L"SKILL_BG_ACTIVE_SPECIAL";
		default:
			return L"SKILL_BG_PASSIVE";
		}
	}

	/// June 31c94ac0: resize a picture around its centre ( CPictureData has no such method in March )
	void JuneSetPictureSizeAroundCenter( CKTDGUIControl::CPictureData* pPicture_, const float fWidth_, const float fHeight_ )
	{
		if ( NULL == pPicture_ || NULL == pPicture_->pPoint )
			return;

		CKTDGUIControl::UIPointData* pPoint = pPicture_->pPoint;
		const float fCenterX = ( pPoint->leftTopPoint.x + pPoint->rightBottomPoint.x ) * 0.5f;
		const float fCenterY = ( pPoint->leftTopPoint.y + pPoint->rightBottomPoint.y ) * 0.5f;
		pPoint->leftTopPoint		= D3DXVECTOR2( fCenterX - fWidth_ * 0.5f, fCenterY - fHeight_ * 0.5f );
		pPoint->rightTopPoint		= D3DXVECTOR2( fCenterX + fWidth_ * 0.5f, fCenterY - fHeight_ * 0.5f );
		pPoint->leftBottomPoint		= D3DXVECTOR2( fCenterX - fWidth_ * 0.5f, fCenterY + fHeight_ * 0.5f );
		pPoint->rightBottomPoint	= D3DXVECTOR2( fCenterX + fWidth_ * 0.5f, fCenterY + fHeight_ * 0.5f );
		pPicture_->SetPos( pPoint->leftTopPoint );
		pPicture_->SetSize( D3DXVECTOR2( fWidth_, fHeight_ ) );
	}

	/// June 32890a30: the user tree holds the skill at level 1 or more ( GetSkillLevel( id, 0, 0, 1 ) > 0 )
	bool JuneDoIHaveThisSkill( const CX2UserSkillTree& refUserSkillTree_, const CX2SkillTree::SKILL_ID eSkillID_ )
	{
		return 0 < refUserSkillTree_.GetSkillLevel( eSkillID_ );
	}

	/// June 323511f0 ( a thiscall on MY unit ): the transcendence memo's alternate skill, 0 when none. The same June function
	/// S3d ported as the static JuneGetMyUnitTranscendenceSkillID in X2GUUser.cpp; this file needs it for the slot icons.
	int JuneGetMyUnitTranscendenceSkillIDForUI( const int iSkillID_ )
	{
		if ( NULL == g_pData || NULL == g_pData->GetSkillTree() || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
			return 0;
		CX2Unit* pJuneMyUnit = g_pData->GetMyUser()->GetSelectUnit();

		const int iBaseSkillID = static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) ) );
		std::vector<const SJuneSkillMemoTemplet*> vecMemo;
		CX2SkillNoteManager::GetInstance()->GetJuneMemoList( iBaseSkillID, 0, static_cast<int>( pJuneMyUnit->GetClass() ), 0, vecMemo );

		const CX2UserSkillTree& refUserSkillTree = pJuneMyUnit->GetUnitData().m_UserSkillTree;
		for ( std::vector<const SJuneSkillMemoTemplet*>::const_iterator it = vecMemo.begin(); it != vecMemo.end(); ++it )
		{
			const SJuneSkillMemoTemplet* pMemo = *it;
			if ( NULL == pMemo || false == refUserSkillTree.IsJuneMemoActive( iBaseSkillID, pMemo->m_iType ) )
				continue;
			if ( pMemo->m_iType < 0x13 || 0x14 < pMemo->m_iType )
				continue;

			const CX2SkillTree::SkillTemplet* pOwnTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
			const int iIndex = ( NULL != pOwnTemplet && 0 != pOwnTemplet->m_iDeleteSkillIDByUpgrade ) ? 1 : 0;
			const float fValue = ( iIndex < static_cast<int>( pMemo->m_vecValue.size() ) ) ? pMemo->m_vecValue[ iIndex ] : 0.f;
			return static_cast<int>( fValue );
		}
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////
// June's skill-type colour table ( CX2SkillTree +0xb4, filled by SkillTypeColor.lua through g_pSkillToolTipManager:AddSkillTypeColor,
// 3220baf0; read by GetSkillSubDesc through 322055c0 / 3265c7f0 )
/////////////////////////////////////////////////////////////////////////
class CX2JuneSkillTypeColorTable
{
public:
	CX2JuneSkillTypeColorTable() : m_bLoaded( false ) {}

	/// June 3220baf0: SKILL_TYPE ( 0 when absent ), COLOR ( L"#C000000" when absent ); the first row of a type stays
	void AddSkillTypeColor_LUA()
	{
		KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
		TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

		int iSkillType = 0;
		if ( false == luaManager.GetValue( "SKILL_TYPE", iSkillType ) )
			iSkillType = 0;
		wstring wstrColor = L"";
		if ( false == luaManager.GetValue( "COLOR", wstrColor ) )
			wstrColor = L"#C000000";
		m_mapColor.insert( std::make_pair( iSkillType, wstrColor ) );
	}

	/// June loads the file once, in the CX2SkillTree ctor ( 3221afb0 -> 32209370 ); here on the first tooltip
	const wstring* Find( const int iSkillType_ )
	{
		if ( false == m_bLoaded )
		{
			m_bLoaded = true;
			lua_State* L = g_pKTDXApp->GetLuaBinder()->GetLuaState();
			lua_tinker::class_add<CX2JuneSkillTypeColorTable>( L, "CX2JuneSkillTypeColorTable" );
			lua_tinker::class_def<CX2JuneSkillTypeColorTable>( L, "AddSkillTypeColor", &CX2JuneSkillTypeColorTable::AddSkillTypeColor_LUA );
			lua_tinker::decl( L, "g_pSkillToolTipManager", this );
			if ( false == g_pKTDXApp->LoadLuaTinker( L"SkillTypeColor.lua" ) )
				CX2OfflineLog::Server( L"[JUNE_SKILL] UI SkillTypeColor.lua did not load: skill-type labels have no colour; check data036 carries June's file" );
			else
				CX2OfflineLog::Server( L"[JUNE_SKILL] UI SkillTypeColor.lua loaded %d colour row(s)", static_cast<int>( m_mapColor.size() ) );
		}

		std::map<int, wstring>::const_iterator it = m_mapColor.find( iSkillType_ );
		return ( m_mapColor.end() != it ) ? &it->second : NULL;
	}

private:
	bool					m_bLoaded;
	std::map<int, wstring>	m_mapColor;
};

static CX2JuneSkillTypeColorTable s_kJuneSkillTypeColorTable;

/////////////////////////////////////////////////////////////////////////
// CX2SkillTreeSlotData : slot data type 1 ( vt 334c2e94 )
/////////////////////////////////////////////////////////////////////////

/// June ctor inlined in 328de040 @328de347: type 1, skill 0, new mark 0
CX2SkillTreeSlotData::CX2SkillTreeSlotData()
{
	m_eSlotDataType	= SDT_SKILL_TREE;
	m_eSkillID		= CX2SkillTree::SI_NONE;
	m_bNewSkill		= false;
}

/*virtual*/ CX2SkillTreeSlotData::~CX2SkillTreeSlotData()
{
}

/// June 328dcd50
/*virtual*/ void CX2SkillTreeSlotData::UpdateUI( CKTDGUISlot* pSlot )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetSkillTree() ||
		 NULL == g_pData->GetUIManager() ||
		 NULL == g_pData->GetUIManager()->GetUISkillTree() )
		return;

	CX2UISkillTreeNew*	pUISkillTree		= g_pData->GetUIManager()->GetUISkillTree();
	CX2Unit*			pUnit				= g_pData->GetMyUser()->GetSelectUnit();
	CX2UserSkillTree&	refUserSkillTree	= pUnit->AccessUnitData().m_UserSkillTree;

	const CX2SkillTree::SkillTemplet*		pSkillTemplet		= g_pData->GetSkillTree()->GetSkillTemplet( m_eSkillID );
	const CX2SkillTree::SkillTreeTemplet*	pSkillTreeTemplet	= g_pData->GetSkillTree()->GetSkillTreeTemplet( static_cast<int>( pUnit->GetClass() ), m_eSkillID );
	if ( NULL == pSkillTemplet || NULL == pSkillTreeTemplet || NULL == pUISkillTree->GetDLGUISkillTree() )
		return;

	/// the transcendence memo's alternate skill gives the icon ( 323511f0 )
	const CX2SkillTree::SkillTemplet* pIconSkillTemplet = NULL;
	{
		const int iAltSkillID = JuneGetMyUnitTranscendenceSkillIDForUI( static_cast<int>( m_eSkillID ) );
		if ( NULL != g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) ) )
			pIconSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) );
	}

	const map<CX2SkillTree::SKILL_ID, CX2UISkillTreeNew::SkillSlotUI>& refmapSkillSlotUISet = pUISkillTree->GetMapSkillSlotUISet();
	map<CX2SkillTree::SKILL_ID, CX2UISkillTreeNew::SkillSlotUI>::const_iterator it = refmapSkillSlotUISet.find( m_eSkillID );
	if ( it == refmapSkillSlotUISet.end() )
		return;
	const CX2UISkillTreeNew::SkillSlotUI& refSkillSlotUI = it->second;
	if ( NULL == refSkillSlotUI.m_pStaticSkillLock || NULL == refSkillSlotUI.m_pStaticSkillBG )
		return;

	/// the page being viewed is the page in use ( user tree +0x20 / +0x24 )
	USHORT usUsingPage = refUserSkillTree.GetUsingPage();
	const USHORT usSelectPage = refUserSkillTree.GetSelectPage();
	const bool bUsingPage = ( usUsingPage == usSelectPage - 1 );
	if ( 0 < usSelectPage )
		usUsingPage = usSelectPage - 1;

	if ( CX2SkillTree::ST_RELATIONSHIP_SKILL == pSkillTemplet->m_eType )
	{
		int iRelationshipLevel = 0;
		if ( NULL != g_pData->GetRelationshipManager() &&
			 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
		{
			switch ( g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType )
			{
			case SEnum::RT_SOLO:
				iRelationshipLevel = 0;
				g_pData->GetSkillTree()->UnequipRelationSkill();	/// 3220f740
				break;
			case SEnum::RT_COUPLE:
				iRelationshipLevel = 1;
				break;
			case SEnum::RT_MARRIED:
				iRelationshipLevel = 2;
				break;
			}
		}

		pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		refUserSkillTree.SetSkillLevelAndCSP( m_eSkillID, iRelationshipLevel, 0, usUsingPage );
		pSlot->SetDragable( 0 != iRelationshipLevel && true == bUsingPage );

		ShowSlotPicture( false, pSlot, STSAPT_CANLEARN );
		if ( 0 == iRelationshipLevel || false == bUsingPage )
			ShowSlotPicture( true, pSlot, STSAPT_DISABLE );
		else
			ShowSlotPicture( false, pSlot, STSAPT_DISABLE );

		refSkillSlotUI.m_pStaticSkillLock->SetShow( 0 == iRelationshipLevel );
	}
	else
	{
		/// sealed: not in the acquired map ( 32890a00 ), born sealed ( +0x4c ) and not unsealed ( 328914b0 )
		bool bIsLockSkill = false;
		if ( false == refUserSkillTree.DoIHaveThisSkill( m_eSkillID ) &&
			 true == pSkillTemplet->m_bBornSealed &&
			 false == refUserSkillTree.IsSkillUnsealed( m_eSkillID ) )
		{
			bIsLockSkill = true;
			pUISkillTree->GetDLGUISkillTree()->ChangeSequence( refSkillSlotUI.m_pStaticSkillLock, true );
		}
		refSkillSlotUI.m_pStaticSkillLock->SetShow( bIsLockSkill );

		if ( NULL == pIconSkillTemplet )
			pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		else
			pSlot->SetItemTex( pIconSkillTemplet->m_wstrIconName.c_str(), pIconSkillTemplet->m_wstrIconPieceName.c_str() );

		const bool bHave = JuneDoIHaveThisSkill( refUserSkillTree, m_eSkillID );
		pSlot->SetDragable( false );
		if ( true == bHave )
		{
			switch ( pSkillTemplet->m_eType )
			{
			case CX2SkillTree::ST_BUFF:
			case CX2SkillTree::ST_ACTIVE:
			case CX2SkillTree::ST_SPECIAL_ACTIVE:
			case 13:	/// ST_HYPER_ACTIVE_SKILL
			case 14:	/// ST_SPECIAL_ACTIVE_GRADE_1
			case 15:	/// ST_SPECIAL_ACTIVE_GRADE_2
			case 16:	/// ST_SPECIAL_ACTIVE_GRADE_3
				if ( false == refSkillSlotUI.m_bSelectSkillBlind && false == bIsLockSkill && true == bUsingPage )
					pSlot->SetDragable( true );
				break;
			default:
				break;
			}
		}
		else
		{
			ShowSlotPicture( false, pSlot, STSAPT_LEVEL );
		}

		if ( false == bHave || true == bIsLockSkill || false == bUsingPage )
			ShowSlotPicture( true, pSlot, STSAPT_DISABLE );
		else
			ShowSlotPicture( false, pSlot, STSAPT_DISABLE );
	}

	/// June-only: the "new skill" glow ( +0x10 )
	if ( true == m_bNewSkill )
		ShowSlotPicture( true, pSlot, STSAPT_NEW );
}

/// June 328dbcd0
void CX2SkillTreeSlotData::ShowSlotPicture( bool bShow, CKTDGUISlot* pSlot, SKILL_TREE_SLOT_ADD_PICTURE_TYPE eType, int val /*= -1*/ )
{
	if ( NULL == pSlot )
		return;

	CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( static_cast<int>( eType ) );
	if ( NULL != pPicture )
	{
		if ( true == bShow && STSAPT_LEVEL == eType )
		{
			wstring wstrLevelTextureName = g_pData->GetSkillTree()->GetSkillLevelTextureName( val );
			wstring wstrLevelTextureKey = g_pData->GetSkillTree()->GetSkillLevelTextureKey( val );
			if ( false == pPicture->SetTex( wstrLevelTextureName.c_str(), wstrLevelTextureKey.c_str() ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );
		}
		pPicture->SetShow( bShow );
		return;
	}

	if ( false == bShow )
		return;

	pPicture = new CKTDGUIControl::CPictureData();
	CKTDGUIControl::UIPointData* pPoint = new CKTDGUIControl::UIPointData( pSlot->GetNowPoint() );
	pPicture->SetPoint( pPoint );

	switch ( eType )
	{
	case STSAPT_DISABLE:
		pPicture->SetTex( L"HQ_ImpossibleEqip.tga" );
		pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 0.6f );
		break;

	case STSAPT_EQUIPPED:
		pPicture->SetTex( L"DLG_Common_New_Texture26.tga", L"Skill_Mount" );
		pPicture->SetFlicker( 2.0f, 1.4f, 0.2f );
		break;

	case STSAPT_SEALED:
		pPicture->SetTex( L"DLG_UI_Common_Texture02.TGA", L"lock" );
		pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );
		break;

	case STSAPT_LEVEL:
		{
			wstring wstrLevelTextureName = g_pData->GetSkillTree()->GetSkillLevelTextureName( val );
			wstring wstrLevelTextureKey = g_pData->GetSkillTree()->GetSkillLevelTextureKey( val );
			if ( false == pPicture->SetTex( wstrLevelTextureName.c_str(), wstrLevelTextureKey.c_str() ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );

			D3DXVECTOR2 vecPos = pPicture->GetPos();
			pPicture->SetPos( D3DXVECTOR2( vecPos.x, vecPos.y + 33.f ) );
			D3DXVECTOR2 vecPicturePos			= pPicture->pPoint->leftTopPoint;
			pPicture->pPoint->leftBottomPoint	= D3DXVECTOR2( vecPicturePos.x, vecPicturePos.y + 14.f );
			pPicture->pPoint->rightTopPoint		= D3DXVECTOR2( vecPicturePos.x + 25.f, vecPicturePos.y );
			pPicture->pPoint->rightBottomPoint	= D3DXVECTOR2( vecPicturePos.x + 25.f, vecPicturePos.y + 14.f );
		} break;

	case STSAPT_CANLEARN:
		pPicture->SetTex( L"DLG_Common_New_Texture26.tga", L"acquire_Skill" );
		pPicture->SetFlicker( 2.0f, 1.4f, 0.2f );
		break;

	case STSAPT_SELECT:
		{
			pPicture->SetTex( L"DLG_UI_Common_Texture_NewSkill_01.TGA", L"AbleChoice" );
			pPicture->SetFlicker( 2.0f, 1.4f, 0.2f );
			D3DXVECTOR2 vecPos = pPicture->GetPos();
			pPicture->SetPos( D3DXVECTOR2( vecPos.x + 20.f, vecPos.y ) );
			pPicture->SetSizeX( 90.f );
		} break;

	case STSAPT_NEW:	/// June-only
		pPicture->SetTex( L"DLG_UI_Common_Texture09.TGA", L"Glow" );
		pPicture->SetFlicker( 1.0f, 1.4f, 0.2f );
		JuneSetPictureSizeAroundCenter( pPicture, 50.f, 50.f );
		break;

	default:
		break;
	}

	pSlot->AddPicture( static_cast<int>( eType ), pPicture );
	pPicture->SetShow( bShow );
}

/////////////////////////////////////////////////////////////////////////
// CX2EquippedSkillSlotData : slot data type 2 ( vt 334c2ea0 )
/////////////////////////////////////////////////////////////////////////

CX2EquippedSkillSlotData::CX2EquippedSkillSlotData()
{
	m_eSlotDataType	= SDT_SKILL_EQUIP;
	m_bDataUpdated	= true;
	m_eSkillID		= CX2SkillTree::SI_NONE;
	m_iIndex		= -1;
	m_bSlotb		= false;
}

/// June inlined in 328da900 @328daa9a: type 2, skill 0, index, bSlotB, updated
CX2EquippedSkillSlotData::CX2EquippedSkillSlotData( int index, bool bSlotb )
{
	m_eSlotDataType	= SDT_SKILL_EQUIP;
	m_bDataUpdated	= true;
	m_eSkillID		= CX2SkillTree::SI_NONE;
	m_iIndex		= index;
	m_bSlotb		= bSlotb;
}

/*virtual*/ CX2EquippedSkillSlotData::~CX2EquippedSkillSlotData()
{
}

/// June reads the slot id inline as index + ( bSlotB ? 5 : 0 ) ( 328d9cf0, 328e18d0 )
int CX2EquippedSkillSlotData::GetSlotID() const
{
	return ( true == m_bSlotb ) ? m_iIndex + 5 : m_iIndex;
}

/// June 328e1830: lock ( bLocked ) or unlock the slot
void CX2EquippedSkillSlotData::SetLockedSlotUI( CKTDGUISlot* pSlot, const bool bLocked_ )
{
	if ( false == bLocked_ )
	{
		if ( NULL != pSlot )
		{
			CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( SESAPT_DISABLE );
			if ( NULL != pPicture )
				pPicture->SetShow( false );
		}
		pSlot->SetEnable( true );
		return;
	}

	if ( CX2SkillTree::SI_NONE != m_eSkillID )
	{
		m_eSkillID = CX2SkillTree::SI_NONE;
		pSlot->SetItemTex( L"DLG_Common_New_Texture03.TGA", L"invisible" );
		pSlot->SetDragable( false );
		CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( SESAPT_LEVEL );
		if ( NULL != pPicture )
			pPicture->SetShow( false );
	}

	if ( true == pSlot->GetEnable() )
	{
		ShowSlotPicture( true, pSlot, SESAPT_DISABLE );
		pSlot->SetEnable( false );
	}
}

/// June 328e18d0
/*virtual*/ void CX2EquippedSkillSlotData::UpdateUI( CKTDGUISlot* pSlot )
{
	if ( NULL == pSlot || NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() || NULL == g_pData->GetSkillTree() )
		return;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2UserSkillTree& refUserSkillTree = pUnit->GetUnitData().m_UserSkillTree;

	/// the lock: slot 4 of a bar is the TC slot ( 3214f3d0( index ) == 3 ): A needs transcendence ( unit +0x3c ), B needs
	/// slot-state key 1 != 0; a B slot otherwise needs key 0 usable ( 32890ee0 ). Without the UI manager only the B rule runs.
	bool bLocked = false;
	if ( NULL != g_pData->GetUIManager() && 4 == m_iIndex )
	{
		if ( false == m_bSlotb )
			bLocked = ( false == CX2UISkillTreeNew::JuneIsMyUnitTranscended() );
		else
			bLocked = ( 0 == refUserSkillTree.GetJuneSlotState( 1 ) );
	}
	else if ( true == m_bSlotb )
	{
		bLocked = ( false == refUserSkillTree.IsJuneSlotUsable( 0 ) );
	}
	SetLockedSlotUI( pSlot, bLocked );

	const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot( m_iIndex, m_bSlotb );
	m_eSkillID = ( NULL != pSkillData ) ? pSkillData->m_eID : CX2SkillTree::SI_NONE;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( m_eSkillID );
	{
		const int iAltSkillID = JuneGetMyUnitTranscendenceSkillIDForUI( static_cast<int>( m_eSkillID ) );
		if ( 0 < iAltSkillID )
			pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) );
	}

	if ( NULL == pSkillTemplet )
	{
		if ( CX2SkillTree::SI_NONE != m_eSkillID )
		{
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( GetSlotID(), CX2SkillTree::SI_NONE );
			return;
		}

		pSlot->SetItemTex( L"DLG_Common_New_Texture03.TGA", L"invisible" );
		CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( SESAPT_LEVEL );
		if ( NULL != pPicture )
			pPicture->SetShow( false );
		pPicture = pSlot->GetPicture( SESAPT_MEMORIZE );
		if ( NULL != pPicture )
			pPicture->SetShow( false );
		pSlot->SetDragable( false );
	}
	else
	{
		/// 328e0aa0 ( the per-class icon override for runtime class states ) is character material: JUNE_SKILL_SYSTEM_PHASES.md S7b
		/// hands it to the character port checklist.
		pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		pSlot->SetDragable( true );

		const int iSkillLevel = refUserSkillTree.GetSkillLevel( m_eSkillID, true );
		if ( iSkillLevel < 1 && CX2SkillTree::SI_NONE != m_eSkillID )
		{
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( GetSlotID(), CX2SkillTree::SI_NONE );
			return;
		}

		/// June 328914b0 @328e1d3d: a skill that is not unsealed is taken off: 32205dc0( slot, 0 ) queues the change unless the same
		/// change is already queued; S7's Handler_EGS_CHANGE_SKILL_SLOT_REQ is June's queue ( 3220e3b0 )
		if ( false == refUserSkillTree.IsSkillUnsealed( m_eSkillID ) )
		{
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( GetSlotID(), CX2SkillTree::SI_NONE );
			return;
		}

		if ( true == pSkillTemplet->m_bShowSkillLevel )
		{
			int iLevelUpNum = 0;
			if ( CX2SkillTree::ST_RELATIONSHIP_SKILL != pSkillTemplet->m_eType )
				iLevelUpNum = refUserSkillTree.GetIncreaseSkillLevel( m_eSkillID );
			ShowSlotPicture( true, pSlot, SESAPT_LEVEL, min( iSkillLevel + iLevelUpNum, 30 ), 0 != iLevelUpNum );
		}
		else
		{
			CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( SESAPT_LEVEL );
			if ( NULL != pPicture )
				pPicture->SetShow( false );
		}

		/// in a game: Eve's El Crystal skills re-pick their icon ( 0x6272 / 0x627f, or the 0x61c4 line through 322074e0 )
		if ( NULL != g_pX2Game )
		{
			const int iSkillID = static_cast<int>( pSkillTemplet->m_eID );
			if ( 0x6272 == iSkillID || 0x627f == iSkillID ||
				 true == CX2UISkillTreeNew::JuneIsSameOrUpgradeSkill( static_cast<int>( m_eSkillID ), 0x61c4 ) )
			{
				g_pX2Game->UpdateEveElectraSkillSlotIcon();
			}
		}
		else
		{
			CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( SESAPT_MEMORIZE );
			if ( NULL != pPicture )
				pPicture->SetShow( false );
		}
	}

	/// The tail ( class type 9 / 0xb: Static_Install_Skill / Static_GCore_Skill through 328e0290, which asks the game unit's vslot 0x71c )
	/// is character material: handed to the checklist with 328e0aa0.
}

/// June 328e0460 ( type 2, the Aisha memorize sign, is character material and not built here )
void CX2EquippedSkillSlotData::ShowSlotPicture( bool bShow, CKTDGUISlot* pSlot, SKILL_EQUIPPED_SLOT_ADD_PICTURE_TYPE eType, int val /*= -1*/, bool bIsSkillLevelUpByItem /*= false*/ )
{
	if ( NULL == pSlot )
		return;

	CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( static_cast<int>( eType ) );
	if ( NULL != pPicture )
	{
		if ( true == bShow && SESAPT_LEVEL == eType )
		{
			wstring wstrLevelTextureName = g_pData->GetSkillTree()->GetSkillLevelTextureName( val );
			wstring wstrLevelTextureKey = g_pData->GetSkillTree()->GetSkillLevelTextureKey( val );
			if ( false == pPicture->SetTex( wstrLevelTextureName.c_str(), wstrLevelTextureKey.c_str() ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );
			pPicture->SetColor( true == bIsSkillLevelUpByItem ? D3DXCOLOR( 0.2f, 1.0f, 0.2f, 1.f ) : D3DXCOLOR( 1, 1, 1, 1 ) );
		}
		pPicture->SetShow( bShow );
		return;
	}

	if ( false == bShow || SESAPT_MEMORIZE == eType )
		return;

	pPicture = new CKTDGUIControl::CPictureData();
	CKTDGUIControl::UIPointData* pPoint = new CKTDGUIControl::UIPointData( pSlot->GetNowPoint() );
	pPicture->SetPoint( pPoint );

	switch ( eType )
	{
	case SESAPT_DISABLE:
		pPicture->SetTex( L"HQ_ImpossibleEqip.tga" );
		pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 0.3f );
		break;

	case SESAPT_LEVEL:
		{
			wstring wstrLevelTextureName = g_pData->GetSkillTree()->GetSkillLevelTextureName( val );
			wstring wstrLevelTextureKey = g_pData->GetSkillTree()->GetSkillLevelTextureKey( val );
			if ( false == pPicture->SetTex( wstrLevelTextureName.c_str(), wstrLevelTextureKey.c_str() ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );

			D3DXVECTOR2 vecPos = pPicture->GetPos();
			pPicture->SetPos( D3DXVECTOR2( vecPos.x, vecPos.y + 30.f ) );
			D3DXVECTOR2 vecPicturePos			= pPicture->pPoint->leftTopPoint;
			pPicture->pPoint->leftBottomPoint	= D3DXVECTOR2( vecPicturePos.x, vecPicturePos.y + 14.f );
			pPicture->pPoint->rightTopPoint		= D3DXVECTOR2( vecPicturePos.x + 25.f, vecPicturePos.y );
			pPicture->pPoint->rightBottomPoint	= D3DXVECTOR2( vecPicturePos.x + 25.f, vecPicturePos.y + 14.f );
			pPicture->SetColor( true == bIsSkillLevelUpByItem ? D3DXCOLOR( 0.2f, 1.0f, 0.2f, 1.f ) : D3DXCOLOR( 1, 1, 1, 1 ) );
		} break;

	default:
		break;
	}

	pSlot->AddPicture( static_cast<int>( eType ), pPicture );
	pPicture->SetShow( bShow );
}

/////////////////////////////////////////////////////////////////////////
// CX2UISkillTreeNew
/////////////////////////////////////////////////////////////////////////

/// June 328e76a0
CX2UISkillTreeNew::CX2UISkillTreeNew( CKTDXStage* pNowStage )
: m_iSelectedTab( 0 )
, m_pDLGCommandDesc( NULL )
, m_bShow( false )
, m_bSkillRemoveMode( false )
, m_DLGSize( 0, 0 )
, m_pDLGUISkillTree( NULL )
, m_pDLGSkillToolTip( NULL )
, m_pDLGSkillUnsealInform( NULL )
, m_pDLGSkillUnsealed( NULL )
, m_pUserSkillTree( NULL )
, m_SumDelta( 0 )
, m_pTalkBoxManager( NULL )
, m_bIsExpandSkillSlot( false )
, m_pDLGGameSkillSlot( NULL )
, m_bShowEquipSkillGuide( false )
, m_bSkillSlotSwap( false )
, m_bIsSkillUITypeA( false )
, m_iMaxSkillRowNum( 0 )
, m_pDLGBuyConfirm( NULL )
, m_bReplacedEsCounter( false )
, m_iConfirmED( 0 )
, m_pHoveredMemoButton( NULL )
, m_iNewSkillCount( 0 )
{
	m_pTalkBoxManager = CX2TalkBoxManager::CreateTalkBoxManger();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pTalkBoxManager );
	m_pTalkBoxManager->SetOverUI( true );

	/// ( stage, file, 0.07f, 0 ), SetLayer( 1 ), AddDlg, SetShow( false ), then the hot-key strings
	m_pDLGGameSkillSlot = new CKTDGUIDialog( pNowStage, L"DLG_GAME_SKILL_SLOT_NEW_10slot.lua", 0.07f, 0 );
	m_pDLGGameSkillSlot->SetLayer( XDL_NORMAL_1 );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGGameSkillSlot );
	m_pDLGGameSkillSlot->SetShow( false );
	UpdateSkillSlotString();

	m_mapSkillPosition.clear();
	m_mapSkillSlotUISet.clear();
}

/// June 328e2090
/*virtual*/ CX2UISkillTreeNew::~CX2UISkillTreeNew( void )
{
	SAFE_DELETE_DIALOG( m_pDLGUISkillTree );
	m_mapSkillSlotUISet.clear();
	m_vecSkillTabControl.clear();
	m_mapSkillPosition.clear();
	SAFE_DELETE_DIALOG( m_pDLGSkillToolTip );
	SAFE_DELETE_DIALOG( m_pDLGSkillUnsealInform );
	SAFE_DELETE_DIALOG( m_pDLGSkillUnsealed );
	SAFE_DELETE_DIALOG( m_pDLGCommandDesc );
	SAFE_DELETE_DIALOG( m_pDLGGameSkillSlot );
	SAFE_DELETE_KTDGOBJECT( m_pTalkBoxManager );
	SAFE_DELETE_DIALOG( m_pDLGBuyConfirm );
}

/// June 328d9c30
void CX2UISkillTreeNew::SetLayer( X2_DIALOG_LAYER layer )
{
	if ( NULL != m_pDLGUISkillTree )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDLGUISkillTree, layer );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGUISkillTree, false );
	}
}

/// June has no SetPosition ( march-only row ): the tree dialog stays where its script puts it. Kept for CX2UIManager's caller.
void CX2UISkillTreeNew::SetPosition( D3DXVECTOR2 vec )
{
}

HRESULT CX2UISkillTreeNew::OnFrameMove( double fTime, float fElapsedTime )
{
	return S_OK;
}

/// June 328ea4e0
bool CX2UISkillTreeNew::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( NULL == m_pDLGUISkillTree || true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGUISkillTree ) )
		return false;

	switch ( uMsg )
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		{
			for ( map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.begin(); it != m_mapSkillSlotUISet.end(); ++it )
			{
				SkillSlotUI& refSkillSlotUI = it->second;
				if ( NULL != refSkillSlotUI.m_pSlot && NULL != refSkillSlotUI.m_pSlot->GetSlotData() )
				{
					if ( CKTDGUISlot::SCS_ONDRAG == refSkillSlotUI.m_pSlot->GetState() && WM_LBUTTONUP != uMsg )
					{
						ShowSkillDesc( false );
						refSkillSlotUI.m_pSlot->SetGuideDescLT( false );
						m_bShowEquipSkillGuide = true;
						break;
					}
					m_bShowEquipSkillGuide = false;
				}
			}
			return m_pDLGUISkillTree->GetIsMouseOver();
		}

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if ( true == m_pDLGUISkillTree->GetIsMouseOver() )
		{
			g_pData->GetUIManager()->SetUILayerIndex( CX2UIManager::UI_MENU_SKILL, true );
			return true;
		}
		break;

	case WM_MOUSEWHEEL:
		ShowSkillDesc( false );
		if ( true == m_pDLGUISkillTree->GetIsMouseOver() )
		{
			OnMouseWheel( hWnd, uMsg, wParam, lParam );
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

/// June 328d9b60: the list of the open tab scrolls ( the command list is S7e's )
bool CX2UISkillTreeNew::OnMouseWheel( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( NULL == m_pDLGUISkillTree )
		return false;

	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( 0 == m_iSelectedTab ? L"SkillTreeControlList" : L"CommandControlList" ) );
	if ( NULL == pControlList || NULL == pControlList->GetYScroolBar() )
		return false;

	CKTDGUIScrollBar* pScroolBarY = pControlList->GetYScroolBar();
	m_SumDelta += static_cast<SHORT>( HIWORD( wParam ) );
	while ( abs( m_SumDelta ) >= WHEEL_DELTA )
	{
		if ( m_SumDelta < 1 )
		{
			pScroolBarY->Scroll( 1 );
			m_SumDelta += WHEEL_DELTA;
		}
		else
		{
			pScroolBarY->Scroll( -1 );
			m_SumDelta -= WHEEL_DELTA;
		}
	}
	return true;
}

/// June 328ea6d0
void CX2UISkillTreeNew::SetShow( bool val )
{
	m_bShow = val;

	if ( false == val )
	{
		/// S7e: June hides the command-description video popup here first ( 328e6cd0( false, L"Unknown", white ) )
		ShowSkillDesc( false );
		ShowUIDesc( false );
		SAFE_DELETE_DIALOG( m_pDLGSkillUnsealInform );
		m_bShowEquipSkillGuide = false;
		g_pData->GetUIManager()->UIClosed( CX2UIManager::UI_MENU_SKILL );
		if ( CX2Main::XS_PVP_ROOM == g_pMain->GetNowStateID() )
			g_pData->GetUIManager()->SetShowSkillSlot( false );
		if ( NULL != g_pMain )
			SetSkillUIType( g_pMain->GetGameOption().GetIsSkillUITypeA() );

		if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree;
			refUserSkillTree.SetSelectPage( refUserSkillTree.GetUsingPage() + 1 );
		}
		RecountJuneNewSkills();
	}
	else
	{
		if ( CX2Main::XS_PVP_ROOM == g_pMain->GetNowStateID() )
		{
			CX2StatePVPRoom* pState = static_cast<CX2StatePVPRoom*>( g_pMain->GetNowState() );
			if ( NULL != pState && true == pState->GetDisableSkillUIOpen() )
			{
				m_bShow = false;
				return;
			}
		}

		m_bSkillRemoveMode = false;
		UpdateSkillEquipedSlotUI();
		UpdateSkillTree();
		UpdateSkillPageUI();
		g_pData->GetUIManager()->UIOpened( CX2UIManager::UI_MENU_SKILL );
		if ( CX2Main::XS_PVP_ROOM == g_pMain->GetNowStateID() )
			g_pData->GetUIManager()->SetShowSkillSlot( true );
		SetSkillUIType( true );
		UpdateJuneTabUI();
	}

	if ( NULL != m_pDLGUISkillTree )
		m_pDLGUISkillTree->SetShowEnable( m_bShow, m_bShow );
}

/// June 328ed890
void CX2UISkillTreeNew::InitSkillTreeUI()
{
	SAFE_DELETE_DIALOG( m_pDLGUISkillTree );
	m_bSkillSlotSwap = false;
	m_mapSkillSlotUISet.clear();
	m_vecSkillTabControl.clear();

	m_pDLGUISkillTree = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Skill_Tree_New2015.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUISkillTree );
	m_pDLGUISkillTree->SetDisableUnderWindow( true );
	m_pDLGUISkillTree->SetShow( false );

	const D3DXVECTOR3 vDummy = m_pDLGUISkillTree->GetDummyPos( 1 );
	m_DLGSize.x = vDummy.x;
	m_DLGSize.y = vDummy.y;

	if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet() )
	{
		/// +0x8c = ( unit +0xe40 == 1 ) is Luciel's form flag: character material ( checklist )
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
		m_pUserSkillTree = &pUnit->GetUnitData().m_UserSkillTree;
		CreateSkillTree( pUnit->GetUnitTemplet()->m_UnitClass, JuneIsMyUnitTranscended() );
		InitEquippedSkillSlotData();
		UpdateJuneTabUI();
		UpdateSkillPageUI();
	}
}

/// June 328eaa50
void CX2UISkillTreeNew::CreateSkillTree( CX2Unit::UNIT_CLASS eUnitClass, bool bTranscended_ )
{
	if ( NULL == m_pDLGUISkillTree || NULL == g_pData || NULL == g_pData->GetSkillTree() )
		return;

	m_mapSkillPosition.clear();
	CreateSkillTreeSlots( eUnitClass );
	/// S7e: 328e7ba0 ( the command lists from ComboTree.lua )
	CreateSkillTreePortrait( eUnitClass, bTranscended_ );
	CreateSkillTreeLevelRows();
	/// S7e: the +0x94 command map and the +0x3c description cache are cleared here
}

/// June 328de040
void CX2UISkillTreeNew::CreateSkillTreeSlots( CX2Unit::UNIT_CLASS eUnitClass )
{
	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"SkillTreeControlList" ) );
	if ( NULL == pControlList )
		return;

	/// one skill per ( tier, index in tier ); a second skill at a taken position replaces it only when it is an upgraded skill
	/// ( +0x178 m_iDeleteSkillIDByUpgrade ) that is unsealed ( 328914b0 ). A hyper-class row ( +0x174 == 5 ) needs transcendence.
	const CX2SkillTree::SkillTreeTempletMap& refSkillTreeTempletMap = g_pData->GetSkillTree()->GetSkillTreeTempletMap( static_cast<int>( eUnitClass ) );
	for ( CX2SkillTree::SkillTreeTempletMap::const_iterator mapit = refSkillTreeTempletMap.begin(); mapit != refSkillTreeTempletMap.end(); ++mapit )
	{
		const CX2SkillTree::SKILL_ID eSkillID = mapit->first;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
		if ( NULL == pSkillTemplet )
			continue;
		if ( false == JuneIsMyUnitTranscended() && CX2SkillTree::SPRT_HYPER_CLASS == pSkillTemplet->m_eJunePowerRateType )
			continue;

		const SkillPositionKey kKey( mapit->second.m_iTier, mapit->second.m_iIndexInTier );
		map<SkillPositionKey, CX2SkillTree::SKILL_ID>::iterator it = m_mapSkillPosition.find( kKey );
		if ( it == m_mapSkillPosition.end() )
		{
			m_mapSkillPosition.insert( std::make_pair( kKey, eSkillID ) );
		}
		else if ( 0 != pSkillTemplet->m_iDeleteSkillIDByUpgrade && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
				  true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.IsSkillUnsealed( eSkillID ) )
		{
			it->second = eSkillID;
		}
	}

	for ( map<SkillPositionKey, CX2SkillTree::SKILL_ID>::const_iterator it = m_mapSkillPosition.begin(); it != m_mapSkillPosition.end(); ++it )
	{
		const int iRow = it->first.m_iRow;
		const int iCol = it->first.m_iCol;
		const CX2SkillTree::SKILL_ID eSkillID = it->second;

		SkillSlotUI UISet;
		CreateSlotUIPreset( UISet, static_cast<int>( eSkillID ), iCol );

		pControlList->InsertItem( UISet.m_pStaticSkillTypeBG,		iRow, iCol );
		pControlList->InsertItem( UISet.m_pStaticSkillBG,			iRow, iCol );
		pControlList->InsertItem( UISet.m_pStaticSkillLock,			iRow, iCol );
		pControlList->InsertItem( UISet.m_pButtonLeftMemoDesc,		iRow, iCol );
		pControlList->InsertItem( UISet.m_pStaticLeftMemoState,		iRow, iCol );
		pControlList->InsertItem( UISet.m_pButtonRightMemoDesc,		iRow, iCol );
		pControlList->InsertItem( UISet.m_pStaticRightMemoState,	iRow, iCol );
		pControlList->InsertItem( UISet.m_pSlot,					iRow, iCol );

		m_mapSkillSlotUISet.insert( std::make_pair( eSkillID, UISet ) );

		CX2SkillTreeSlotData* pSkillTreeSlotData = new CX2SkillTreeSlotData;
		if ( NULL != UISet.m_pSlot )
		{
			UISet.m_pSlot->SetID( static_cast<int>( eSkillID ) );
			UISet.m_pSlot->SetSlotData( pSkillTreeSlotData );
		}
		pSkillTreeSlotData->m_eSkillID = eSkillID;
		pSkillTreeSlotData->m_bDataUpdated = true;

		/// S7c: June moves the left memo button right by the list's +0x2b0 ( @328de384 ), gives the right one the same position, and builds
		/// both memo popups ( 328dda50( 1 ), 328dda50( 0 ) )

		if ( iRow > m_iMaxSkillRowNum )
			m_iMaxSkillRowNum = iRow;
	}

	pControlList->SetIndex( 0, 0, true, true );

	/// the page radios and buttons are shown with the skill tab ( 328e89f0 )
	const WCHAR* awszPageControl[] = { L"Skill_Page1", L"Skill_Page2", L"Skill_Page3", L"Add_Skill_Page_ED", L"Add_Skill_Page_Cash" };
	for ( int i = 0; i < ARRAY_SIZE( awszPageControl ); ++i )
	{
		CKTDGUIControl* pControl = m_pDLGUISkillTree->GetControl( awszPageControl[i] );
		if ( NULL != pControl )
			m_vecSkillTabControl.push_back( pControl );
	}
}

/// June 328db2d0
void CX2UISkillTreeNew::CreateSlotUIPreset( SkillSlotUI& UISet, int eSkillID, int iCol )
{
	if ( NULL == m_pDLGUISkillTree )
		return;

	m_pDLGUISkillTree->OpenScriptFile( L"DLG_UI_Skill_Icon_New2015.lua" );

	WCHAR buf[255] = { 0, };

	/// skill-type background ( June-only ): the type's key in DLG_UI_Common_Texture_NewSkill_02.TGA ( 32207560 )
	UISet.m_pStaticSkillTypeBG = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"g_pStatic_SkillTypeBackGround" ) );
	if ( NULL != UISet.m_pStaticSkillTypeBG && NULL != UISet.m_pStaticSkillTypeBG->GetPicture( 0 ) )
	{
		StringCchPrintf( buf, 255, L"g_pStatic_SkillTypeBackGround_%d", eSkillID );
		UISet.m_pStaticSkillTypeBG->SetName( buf );

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( eSkillID ) );
		if ( NULL != pSkillTemplet )
			UISet.m_pStaticSkillTypeBG->GetPicture( 0 )->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", JuneGetSkillTypeBGKey( static_cast<int>( pSkillTemplet->m_eType ) ) );
	}

	UISet.m_pStaticSkillBG = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"g_pStatic_SkillBackGround" ) );
	if ( NULL != UISet.m_pStaticSkillBG && NULL != UISet.m_pStaticSkillBG->GetPicture( 0 ) )
	{
		StringCchPrintf( buf, 255, L"g_pStatic_SkillBackGround_%d", eSkillID );
		UISet.m_pStaticSkillBG->SetName( buf );
	}

	UISet.m_pStaticSkillLock = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"g_pStaticSkill_Lock" ) );
	if ( NULL != UISet.m_pStaticSkillLock )
	{
		StringCchPrintf( buf, 255, L"g_pStaticSkill_Lock_%d", eSkillID );
		UISet.m_pStaticSkillLock->SetName( buf );
		UISet.m_pStaticSkillLock->SetShow( false );
		m_pDLGUISkillTree->ChangeSequence( UISet.m_pStaticSkillLock, true );
		UISet.m_pStaticSkillLock->SetOutOfControlList( true );	/// +0xc5
	}

	/// the memo buttons carry the skill id twice in their dummy ints ( S7c reads them )
	UISet.m_pButtonLeftMemoDesc = static_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"g_pButton_LeftMemoDesc" ) );
	if ( NULL != UISet.m_pButtonLeftMemoDesc )
	{
		StringCchPrintf( buf, 255, L"g_pButton_LeftMemoDesc_%d", eSkillID );
		UISet.m_pButtonLeftMemoDesc->SetName( buf );
		UISet.m_pButtonLeftMemoDesc->AddDummyInt( eSkillID );
		UISet.m_pButtonLeftMemoDesc->AddDummyInt( eSkillID );
		m_pDLGUISkillTree->ChangeSequence( UISet.m_pButtonLeftMemoDesc, true );
	}

	UISet.m_pStaticLeftMemoState = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"g_pStatic_LeftMemoState" ) );
	if ( NULL != UISet.m_pStaticLeftMemoState )
	{
		StringCchPrintf( buf, 255, L"g_pStatic_LeftMemoState_%d", eSkillID );
		UISet.m_pStaticLeftMemoState->SetName( buf );
		for ( int i = 0; i < 3; ++i )
		{
			if ( NULL != UISet.m_pStaticLeftMemoState->GetPicture( i ) )
				UISet.m_pStaticLeftMemoState->GetPicture( i )->SetShow( false );
		}
	}

	UISet.m_pButtonRightMemoDesc = static_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"g_pButton_RightMemoDesc" ) );
	if ( NULL != UISet.m_pButtonRightMemoDesc )
	{
		StringCchPrintf( buf, 255, L"g_pButton_RightMemoDesc_%d", eSkillID );
		UISet.m_pButtonRightMemoDesc->SetName( buf );
		UISet.m_pButtonRightMemoDesc->AddDummyInt( eSkillID );
		UISet.m_pButtonRightMemoDesc->AddDummyInt( eSkillID );
		m_pDLGUISkillTree->ChangeSequence( UISet.m_pButtonRightMemoDesc, true );
	}

	UISet.m_pStaticRightMemoState = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"g_pStatic_RightMemoState" ) );
	if ( NULL != UISet.m_pStaticRightMemoState )
	{
		StringCchPrintf( buf, 255, L"g_pStatic_RightMemoState_%d", eSkillID );
		UISet.m_pStaticRightMemoState->SetName( buf );
		for ( int i = 0; i < 3; ++i )
		{
			if ( NULL != UISet.m_pStaticRightMemoState->GetPicture( i ) )
				UISet.m_pStaticRightMemoState->GetPicture( i )->SetShow( false );
		}
	}

	UISet.m_pSlot = static_cast<CKTDGUISlot*>( m_pDLGUISkillTree->GetControl( L"g_pSlot_SkillTree" ) );
	if ( NULL != UISet.m_pSlot )
	{
		StringCchPrintf( buf, 255, L"g_pSlot_SkillTree_%d", eSkillID );
		UISet.m_pSlot->SetName( buf );
	}
}

/// June 328dc680: one "Lv.n" row per tier ( 1, 5, 10 ... 70, 80, 90, 95, 100 ) with a BG_LV and a LINE picture, at column 0
void CX2UISkillTreeNew::CreateSkillTreeLevelRows()
{
	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"SkillTreeControlList" ) );
	if ( NULL == pControlList )
		return;

	WCHAR buf[255] = { 0, };
	int iLevel = 1;
	int iStep = 5;
	for ( int iRow = 0; iRow < m_iMaxSkillRowNum + 1; ++iRow )
	{
		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		CKTDGUIControl::CPictureData* pPictureBG = new CKTDGUIControl::CPictureData();
		CKTDGUIControl::CPictureData* pPictureLine = new CKTDGUIControl::CPictureData();

		StringCchPrintf( buf, 255, L"Lv.%d", iLevel );
		pStatic->SetName( buf );
		m_pDLGUISkillTree->AddControl( pStatic );
		m_vecSkillTabControl.push_back( pStatic );

		pString->fontIndex	= 1;
		pString->color		= D3DXCOLOR( 1.f, 0.8f, 0.f, 1.f );
		pString->msg		= buf;
		pString->pos		= D3DXVECTOR2( -15.f, -3.f );
		pString->sortFlag	= 0;
		pString->fontStyle	= CKTDGFontManager::FS_NONE;
		pStatic->AddString( pString );

		pPictureBG->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_LV" );
		pPictureBG->SetSizeAsTextureSize();
		pPictureBG->SetPos( D3DXVECTOR2( -25.f, -11.f ) );
		pStatic->AddPicture( pPictureBG );

		pPictureLine->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"LINE" );
		pPictureLine->SetSizeAsTextureSize();
		pPictureLine->SetPos( D3DXVECTOR2( -27.f, -14.f ) );
		pStatic->AddPicture( pPictureLine );

		pControlList->InsertItem( pStatic, iRow, 0 );

		if ( iLevel < 90 )
		{
			if ( iLevel < 70 )
				iLevel = iStep;
			else
				iLevel += 10;
		}
		else
		{
			iLevel += 5;
		}
		iStep += 5;
	}

	pControlList->SetIndex( 0, 0, true, true );

	CKTDGUIControl* pHelpString = m_pDLGUISkillTree->GetControl( L"Help_String" );
	if ( NULL != pHelpString )
		m_vecSkillTabControl.push_back( pHelpString );
}

/// June 328e8e90: the class portrait ( CIT_160by160, June's type 2 ) and the class name from the unit templet's "( ... )". The major-skill
/// and command panels that follow ( 3220bdb0, 328e42d0, 328e8820 ) are S7e's. Luciel ( unit type 10, 328dcba0 ) is character material.
void CX2UISkillTreeNew::CreateSkillTreePortrait( CX2Unit::UNIT_CLASS eUnitClass, bool bTranscended_ )
{
	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"SkillTree_Character" ) );
	if ( NULL == pStatic )
		return;

	wstring wstrFileName = L"";
	wstring wstrPieceName = L"";
	if ( true == CX2Data::GetCharacterImageName( wstrFileName, wstrPieceName, eUnitClass, CX2Data::CIT_160by160 ) && NULL != pStatic->GetPicture( 1 ) )
		pStatic->GetPicture( 1 )->SetTex( wstrFileName.c_str(), wstrPieceName.c_str() );

	if ( NULL != g_pData->GetUnitManager() )
	{
		const CX2Unit::UnitTemplet* pUnitTemplet = g_pData->GetUnitManager()->GetUnitTemplet( eUnitClass );
		if ( NULL != pUnitTemplet && NULL != pStatic->GetString( 0 ) )
		{
			/// June: find( L"(" ) + 1 .. find( L")" ); with no brackets that is the whole name
			const wstring& wstrName = pUnitTemplet->m_Name;
			const wstring::size_type iOpen = wstrName.find( L"(" );
			const wstring::size_type iClose = wstrName.find( L")" );
			const wstring::size_type iFrom = iOpen + 1;
			pStatic->GetString( 0 )->msg = wstrName.substr( iFrom, iClose - iFrom );
		}
	}
}

/// June 328e7b70 ( UpdateUI( bEquipped, bSkillTree, bSkillPage ) ): March's bSP is ignored
void CX2UISkillTreeNew::UpdateUI( bool bEquipped, bool bSP /*= true*/, bool bSkillTree /*= false*/, bool bSkillPage_ /*= false*/ )
{
	if ( true == bEquipped )
		UpdateSkillEquipedSlotUI();
	if ( true == bSkillTree )
		UpdateSkillTree();
	if ( true == bSkillPage_ )
		UpdateSkillPageUI();
}

/// June 328ddf60
void CX2UISkillTreeNew::UpdateSkillTree()
{
	for ( map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.begin(); it != m_mapSkillSlotUISet.end(); ++it )
	{
		if ( NULL != it->second.m_pSlot && NULL != it->second.m_pSlot->GetSlotData() )
		{
			it->second.m_pSlot->GetSlotData()->m_bDataUpdated = true;
			/// S7c: the memo popups of the skill ( 328dda50( 1 ), 328dda50( 0 ) )
		}
	}
}

/// June 328ddff0
void CX2UISkillTreeNew::UpdateSkillTree( CX2SkillTree::SKILL_ID eSkillID )
{
	map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.find( eSkillID );
	if ( it != m_mapSkillSlotUISet.end() && NULL != it->second.m_pSlot && NULL != it->second.m_pSlot->GetSlotData() )
	{
		it->second.m_pSlot->GetSlotData()->m_bDataUpdated = true;
		/// S7c: the memo popups of the skill ( 328dda50( 1 ), 328dda50( 0 ) )
	}
}

/// June 328da2e0 ( caller 31f08290, the new-skill NOT ); June walks its 7-byte KSkillData vector
void CX2UISkillTreeNew::MarkJuneNewSkills( const std::vector<KSkillData>& vecSkillData_ )
{
	for ( std::vector<KSkillData>::const_iterator vit = vecSkillData_.begin(); vit != vecSkillData_.end(); ++vit )
	{
		map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.find( static_cast<CX2SkillTree::SKILL_ID>( vit->m_iSkillID ) );
		if ( it == m_mapSkillSlotUISet.end() || NULL == it->second.m_pSlot )
			continue;

		CKTDGUISlotData* pSlotData = it->second.m_pSlot->GetSlotData();
		if ( NULL != pSlotData && CKTDGUISlotData::SDT_SKILL_TREE == pSlotData->m_eSlotDataType &&
			 false == static_cast<CX2SkillTreeSlotData*>( pSlotData )->m_bNewSkill )
		{
			static_cast<CX2SkillTreeSlotData*>( pSlotData )->m_bNewSkill = true;
			pSlotData->m_bDataUpdated = true;
			++m_iNewSkillCount;
		}
	}
}

/// June 328da100 ( from SetShow( false ) )
void CX2UISkillTreeNew::RecountJuneNewSkills()
{
	m_iNewSkillCount = 0;
	for ( map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.begin(); it != m_mapSkillSlotUISet.end(); ++it )
	{
		if ( NULL == it->second.m_pSlot )
			continue;
		CKTDGUISlotData* pSlotData = it->second.m_pSlot->GetSlotData();
		if ( NULL != pSlotData && CKTDGUISlotData::SDT_SKILL_TREE == pSlotData->m_eSlotDataType &&
			 true == static_cast<CX2SkillTreeSlotData*>( pSlotData )->m_bNewSkill )
			++m_iNewSkillCount;
	}
}

/// June 328d98e0
void CX2UISkillTreeNew::DecreaseJuneNewSkillCount()
{
	--m_iNewSkillCount;
	if ( m_iNewSkillCount < 0 )
		m_iNewSkillCount = 0;
}

/// June 328d9fb0 ( caller 324c3873, a state-menu path ). JUNE_BUGS: the radios are set only when the command-description dialog
/// ( +0x30 ) exists, which June tests instead of the tree dialog ( +0xb4 ) whose radios it sets.
void CX2UISkillTreeNew::SelectJuneTab( const bool bSkillTab_ )
{
	m_iSelectedTab = ( true == bSkillTab_ ) ? 0 : 1;
	if ( NULL == m_pDLGCommandDesc || NULL == m_pDLGUISkillTree )
		return;

	CKTDGUIRadioButton* pRadio = static_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Tab" ) );
	if ( NULL != pRadio )
		pRadio->SetChecked( bSkillTab_ );
	pRadio = static_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Command_Tab" ) );
	if ( NULL != pRadio )
		pRadio->SetChecked( false == bSkillTab_ );
}

/// June 328e89f0: the skill tab shows SkillTreeControlList and the +0x08 controls; the command tab ( CommandControlList and its
/// group controls, 328da3c0 ) is S7e's
void CX2UISkillTreeNew::UpdateJuneTabUI()
{
	if ( NULL == m_pDLGUISkillTree )
		return;

	const bool bSkillTab = ( 0 == m_iSelectedTab );
	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"SkillTreeControlList" ) );
	if ( NULL != pControlList )
	{
		pControlList->SetShow( bSkillTab );
		pControlList->SetEnable( bSkillTab );
		pControlList->SetShowEnable( bSkillTab, bSkillTab );
		for ( vector<CKTDGUIControl*>::iterator it = m_vecSkillTabControl.begin(); it != m_vecSkillTabControl.end(); ++it )
		{
			if ( NULL != *it )
				(*it)->SetShowEnable( bSkillTab, bSkillTab );
		}
		if ( true == bSkillTab )
		{
			UpdateSkillTree();
			UpdateSkillPageUI();
		}
	}

	pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"CommandControlList" ) );
	if ( NULL != pControlList )
	{
		pControlList->SetShow( false == bSkillTab );
		pControlList->SetEnable( false == bSkillTab );
		pControlList->SetShowEnable( false == bSkillTab, false == bSkillTab );
		/// S7e: 328da3c0( show, group ) over the +0x18 command controls
	}
}

/// June 328e2590 ( OpenDLGSkillUnsealed( bOpen, skill ) ). June first offers the message ( name + STR 3847 ) to a notice system at
/// g_pData +0x290 ( 31d72cc0 ) and opens the dialog only when that is absent or declines; March has no such system, so the dialog opens.
void CX2UISkillTreeNew::OpenDLGSkillUnsealed( bool bOpen )
{
	if ( false == bOpen )
	{
		if ( NULL != m_pDLGSkillUnsealed )
			m_pDLGSkillUnsealed->SetShowEnable( false, false );
		return;
	}

	if ( NULL == m_pDLGSkillUnsealed )
	{
		m_pDLGSkillUnsealed = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Skill_Acquisition.lua", 0.07f, 0 );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSkillUnsealed );
	}
	m_pDLGSkillUnsealed->SetShowEnable( true, true );
}

/// June 32896c70 ( CX2UserSkillTree ): may the skill go into the slot? JUNE_BUGS: each equipped skill's SA 190
/// ( SA_NOT_EQUIP_WITH_SKILLID_SAME_TIME ) is compared with that same skill's id, not the incoming one, and the B loop compares the
/// target slot id with the B index 0-4 ( no +5 ).
static bool JuneCanEquipSkillToSlot( const CX2UserSkillTree& refUserSkillTree_, const int iSlotID_, const CX2SkillTree::SKILL_ID eSkillID_ )
{
	if ( NULL == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID_ ) )
		return true;

	for ( int iBar = 0; iBar < 2; ++iBar )
	{
		for ( int i = 0; i < 5; ++i )
		{
			const CX2UserSkillTree::SkillSlotData* pSlot = refUserSkillTree_.GetSkillSlot( i, 1 == iBar );
			if ( NULL == pSlot )
				continue;
			const CX2SkillTree::SkillTemplet* pEquipped = g_pData->GetSkillTree()->GetSkillTemplet( pSlot->m_eID );
			if ( NULL == pEquipped )
				continue;
			const int iValue = static_cast<int>( pEquipped->GetSkillAbilityValue( CX2SkillTree::SA_NOT_EQUIP_WITH_SKILLID_SAME_TIME, 1, false ) );
			if ( iSlotID_ != i && iValue == static_cast<int>( pEquipped->m_eID ) )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999.f, -999.f ), GET_STRING( 39765 ), g_pMain->GetNowState() );
				return false;
			}
		}
	}
	return true;
}

/// June 328e6550
bool CX2UISkillTreeNew::ProcessSkillTreeSlotRMouseUp( LPARAM lParam )
{
	CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );
	if ( NULL != pNowState && false == pNowState->GetEnableShortCutKey() )
		return true;

	CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
	CX2SkillTreeSlotData* pSlotData = static_cast<CX2SkillTreeSlotData*>( pSlot->GetSlotData() );
	if ( NULL == pSlotData || NULL == m_pUserSkillTree )
		return true;

	if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 m_pUserSkillTree->GetSkillLevel( pSlotData->m_eSkillID, true ) < 1 )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), GET_STRING( 35567 ), g_pMain->GetNowState() );
		return true;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSlotData->m_eSkillID );
	if ( NULL == pSkillTemplet || false == JuneDoIHaveThisSkill( *m_pUserSkillTree, pSlotData->m_eSkillID ) )
		return true;

	switch ( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_BUFF:
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
	case 13:	/// ST_HYPER_ACTIVE_SKILL
	case 14:	/// ST_SPECIAL_ACTIVE_GRADE_1
	case 15:	/// ST_SPECIAL_ACTIVE_GRADE_2
	case 16:	/// ST_SPECIAL_ACTIVE_GRADE_3
		{
			if ( -1 != m_pUserSkillTree->GetSkillEquippedSlotIndex( pSlotData->m_eSkillID, false ) ||
				 -1 != m_pUserSkillTree->GetSkillEquippedSlotIndex( pSlotData->m_eSkillID, true ) )
				break;

			/// the first free A slot; the TC slot ( index 4 ) counts as full unless the unit is transcended
			const int iFreeA = m_pUserSkillTree->GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, false );
			const bool bAFull = ( 4 == iFreeA ) ? ( false == JuneIsMyUnitTranscended() ) : ( -1 == iFreeA );
			if ( false == bAFull )
			{
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( iFreeA, pSlotData->m_eSkillID );
			}
			else if ( true == m_bIsExpandSkillSlot )
			{
				const int iFreeB = m_pUserSkillTree->GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, true );
				const bool bBFree = ( 4 == iFreeB ) ? m_pUserSkillTree->IsJuneSlotUsable( 1 ) : ( -1 != iFreeB );
				if ( true == bBFree )
				{
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
					g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( iFreeB + 5, pSlotData->m_eSkillID );
				}
			}
			else if ( true == m_pUserSkillTree->IsJuneSlotUsable( 1 ) )
			{
				/// B closed: only the B-TC slot ( 9 ), when its state id 0x44 holds nothing ( 32890140 )
				const CX2UserSkillTree::SkillSlotData* pBTC = m_pUserSkillTree->GetSkillSlot( 4, true );
				if ( NULL != pBTC && CX2SkillTree::SI_NONE == pBTC->m_eID )
				{
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
					g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( 9, pSlotData->m_eSkillID );
				}
			}
		} break;

	default:
		break;
	}
	return true;
}

/// June 328d9ea0
bool CX2UISkillTreeNew::ProcessEquipSlotDropped( LPARAM lParam )
{
	CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
	CX2EquippedSkillSlotData* pSlotData = static_cast<CX2EquippedSkillSlotData*>( pSlot->GetSlotData() );

	CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );
	if ( NULL != pNowState && false == pNowState->GetEnableShortCutKey() )
		return false;

	CKTDGUISlot* pDroppedSlot = pSlot->GetDroppedSlot();
	if ( NULL == pDroppedSlot || NULL == pDroppedSlot->GetSlotData() || NULL == pSlotData )
		return false;

	CKTDGUISlotData* pDroppedSlotData = pDroppedSlot->GetSlotData();
	switch ( pDroppedSlotData->m_eSlotDataType )
	{
	case CKTDGUISlotData::SDT_SKILL_TREE:
		{
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
			const CX2SkillTree::SKILL_ID eSkillID = static_cast<CX2SkillTreeSlotData*>( pDroppedSlotData )->m_eSkillID;
			if ( NULL != m_pUserSkillTree && true == JuneCanEquipSkillToSlot( *m_pUserSkillTree, pSlotData->GetSlotID(), eSkillID ) )
			{
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), eSkillID );
				return true;
			}
		} break;

	case CKTDGUISlotData::SDT_SKILL_EQUIP:
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
		g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), static_cast<CX2EquippedSkillSlotData*>( pDroppedSlotData )->m_eSkillID );
		return true;

	default:
		break;
	}
	return false;
}

/// June 328d9cf0
bool CX2UISkillTreeNew::ProcessEquipSlotRMouseUp( LPARAM lParam )
{
	CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );
	if ( NULL != pNowState && false == pNowState->GetEnableShortCutKey() )
		return true;

	CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
	CX2EquippedSkillSlotData* pSlotData = static_cast<CX2EquippedSkillSlotData*>( pSlot->GetSlotData() );
	if ( NULL == pSlotData )
		return true;

	if ( CX2SkillTree::SI_NONE != pSlotData->m_eSkillID )
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_UnEquip.ogg", false, false );
	g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), CX2SkillTree::SI_NONE );
	return true;
}

/// March API ( X2SkillTree.cpp's learn path ): June reads the user tree's level ( 32890620 )
const int CX2UISkillTreeNew::GetSkillLevelInSkillWimdow( const CX2SkillTree::SKILL_ID eSkillID ) const
{
	return ( NULL != m_pUserSkillTree ) ? m_pUserSkillTree->GetSkillLevel( eSkillID ) : 0;
}

/// June 322074e0 ( CX2SkillTree ): equal ids, the target's "[Enhanced]" skill ( map +0x24, 32205c00 ), or the target templet's
/// m_iDeleteSkillIDByUpgrade
/*static*/ bool CX2UISkillTreeNew::JuneIsSameOrUpgradeSkill( const int iSkill_, const int iTarget_ )
{
	if ( iSkill_ == iTarget_ )
		return true;
	if ( NULL == g_pData || NULL == g_pData->GetSkillTree() )
		return false;
	if ( iSkill_ == g_pData->GetSkillTree()->GetUpgradedSkillID( iTarget_ ) )
		return true;

	int iTarget = iTarget_;
	const CX2SkillTree::SkillTemplet* pTarget = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iTarget_ ) );
	if ( NULL != pTarget && 0 != pTarget->m_iDeleteSkillIDByUpgrade )
		iTarget = pTarget->m_iDeleteSkillIDByUpgrade;
	return iSkill_ == iTarget;
}

/////////////////////////////////////////////////////////////////////////
// The equip bar: S6 ( SERV_IRUHADEV_JUNE_SKILL_SLOT_UI ) and S7 ( JuneAutoEquipNewSkills ) code carried from X2UISkillTreeNew.cpp
// as it was, plus ShowUIDesc / UpdateDLGSkillUnsealed ( June match rows ) and the page UI ( March's, until S7d )
/////////////////////////////////////////////////////////////////////////

CX2EquippedSkillSlotData* CX2UISkillTreeNew::GetEquippedSlotData( int index, bool bSlotB /*= false*/ )
{
	CX2EquippedSkillSlotData* pSlotData = NULL;

	if(false == bSlotB)
	{
		switch(index)
		{
		case 0:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA1" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 1:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA2" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 2:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA3" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 3:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA4" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 4:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA5" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		default:
			break;
		}
	}
	else
	{
		switch(index)
		{
		case 0:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB1" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 1:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB2" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 2:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB3" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 3:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB4" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		case 4:
			{
				CKTDGUISlot* pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB5" );
				if(pSlot != NULL)
					pSlotData = (CX2EquippedSkillSlotData*) pSlot->GetSlotData();
			} break;
		default:
			break;
		}
	}

	return pSlotData;
}
CKTDGUISlot* CX2UISkillTreeNew::GetEquippedSlot( int index , bool bSlotB /* = false */ )
{
	if (m_pDLGUISkillTree == NULL)
		return NULL;

	CKTDGUISlot* pSlot = NULL ;

	if( index >= 5 )	/// June 328da440: 0-4 A, 5-9 B
	{
		index -= 5;
		bSlotB = true;
	}

	if(false == bSlotB)
	{
		switch(index)
		{
		case 0:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA1" );
			} break;
		case 1:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA2" );
			} break;
		case 2:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA3" );
			} break;
		case 3:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA4" );
			} break;
		case 4:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA5" );
			} break;
		default:
			break;
		}
	}
	else
	{
		switch(index)
		{
		case 0:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB1" );
			} break;
		case 1:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB2" );
			} break;
		case 2:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB3" );
			} break;
		case 3:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB4" );
			} break;
		case 4:
			{
				pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB5" );
			} break;
		default:
			break;
		}
	}

	return pSlot;
}
void CX2UISkillTreeNew::InitEquippedSkillSlotData()
{

	if(m_pDLGUISkillTree == NULL)
		return;

	for(int i=0; i<10; ++i)
	{
		bool bSlotB = (i > 4) ? true : false;
		int iSlotIndex = (i > 4) ? i-5 : i;
		CX2EquippedSkillSlotData* pData = GetEquippedSlotData(iSlotIndex, bSlotB);
		SAFE_DELETE(pData);
	}
	CKTDGUISlot* pSlot = NULL;
	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA1" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(0, false));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA2" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(1, false));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA3" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(2, false));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA4" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(3, false));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB1" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(0, true));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB2" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(1, true));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB3" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(2, true));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB4" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(3, true));
	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotA5" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(4, false));

	pSlot = (CKTDGUISlot*) m_pDLGGameSkillSlot->GetControl( L"SlotB5" );
	if(pSlot != NULL)
		pSlot->SetSlotData(new CX2EquippedSkillSlotData(4, true));

	{
		/// a missing control would leave a slot without data: say so once per dialog build
		static const WCHAR* s_awszSlot[10] = { L"SlotA1", L"SlotA2", L"SlotA3", L"SlotA4", L"SlotA5", L"SlotB1", L"SlotB2", L"SlotB3", L"SlotB4", L"SlotB5" };
		int iFound = 0;
		for( int i = 0; i < 10; ++i )
		{
			if( NULL != m_pDLGGameSkillSlot->GetControl( s_awszSlot[i] ) )
				++iFound;
		}
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI slot dialog DLG_GAME_SKILL_SLOT_NEW_10slot: %d of 10 slot controls found, slot data attached to each", iFound );
	}
}

//{{ Iruha : 2026-10-18 // June skill slot dialog ( DLG_GAME_SKILL_SLOT_NEW_10slot ), phase S6
/// June 328dfab0: one lock overlay ( the buy-guide button and the lock picture ) shown or hidden
static void JuneSetSlotLockControls( CKTDGUIDialogType pDLG_, const WCHAR* wszButton_, const WCHAR* wszLock_, const bool bLocked_ )
{
	CKTDGUIButton* pButton = (CKTDGUIButton*) pDLG_->GetControl( wszButton_ );
	if( NULL != pButton )
		pButton->SetShowEnable( bLocked_, bLocked_ );

	CKTDGUIStatic* pStaticLock = (CKTDGUIStatic*) pDLG_->GetControl( wszLock_ );
	if( NULL != pStaticLock )
		pStaticLock->SetShow( bLocked_ );
}

/// June 328dfab0: the B bar key labels ( SkillHotKeyEx ) go to alpha 1 or to the locked 0.5 ( _DAT_3343777c )
static void JuneSetSlotHotKeyAlpha( CKTDGUIDialogType pDLG_, const float fAlpha_ )
{
	CKTDGUIStatic* pStaticSkillHotKey = (CKTDGUIStatic*) pDLG_->GetControl( L"SkillHotKeyEx" );
	if( NULL != pStaticSkillHotKey )
		pStaticSkillHotKey->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, fAlpha_ ) );
}

/// June 328dfab0: a locked slot shows no cool time: clear the text and hide the fade picture of slots [ iFrom_, iTo_ )
static void JuneClearSlotCoolTime( CKTDGUIDialogType pDLG_, const bool bSlotB_, const int iFrom_, const int iTo_ )
{
	CKTDGUIStatic* pStaticCoolTime = (CKTDGUIStatic*) pDLG_->GetControl( ( true == bSlotB_ ) ? L"Static_Skill_Cool_Time_B" : L"Static_Skill_Cool_Time" );
	CKTDGUIStatic* pStaticFade = (CKTDGUIStatic*) pDLG_->GetControl( ( true == bSlotB_ ) ? L"Static_Skill_Cool_Time_Fade_B" : L"Static_Skill_Cool_Time_Fade" );
	if( NULL == pStaticCoolTime || NULL == pStaticFade )
		return;

	for( int i = iFrom_; i < iTo_; ++i )
	{
		pStaticCoolTime->SetString( i, L"" );
		if( NULL != pStaticFade->GetPicture( i ) )
			pStaticFade->GetPicture( i )->bShow = false;
	}
}

/// June 328db990: the castables among vecSkillID_ ( types 5, 6, 7, 14-16, acquired, in neither bar ) go into the free A slots, then into the
/// free B slots when slot-state key 0 is usable; the transcendence slots stay out. Each one plays Skill_Equip.ogg and queues a slot change.
bool CX2UISkillTreeNew::JuneAutoEquipNewSkills( const std::vector< int >& vecSkillID_ )
{
	/// +0xf4 = the unit's user skill tree ( set in InitSkillTreeUI 328ed890 )
	if( NULL == m_pUserSkillTree || true == vecSkillID_.empty() )
		return false;

	std::vector< int > vecEmptyA;
	std::vector< int > vecEmptyB;
	m_pUserSkillTree->GetJuneEmptySlotIndexList( vecEmptyA, false, false );
	if( true == m_pUserSkillTree->IsJuneSlotUsable( 0 ) )
		m_pUserSkillTree->GetJuneEmptySlotIndexList( vecEmptyB, true, false );

	if( true == vecEmptyA.empty() && true == vecEmptyB.empty() )
		return false;

	std::vector< int > vecCandidate;
	for( UINT i = 0; i < vecSkillID_.size(); ++i )
	{
		const CX2SkillTree::SKILL_ID eSkillID = static_cast<CX2SkillTree::SKILL_ID>( vecSkillID_[i] );

		if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
			0 >= m_pUserSkillTree->GetSkillLevel( eSkillID, false, 0, true ) )
			continue;

		const CX2SkillTree::SkillTemplet* pTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
		if( NULL == pTemplet || 0 >= m_pUserSkillTree->GetSkillLevel( eSkillID, false, 0, true ) )	/// 32890a30
			continue;

		switch( pTemplet->m_eType )
		{
		case CX2SkillTree::ST_BUFF:
		case CX2SkillTree::ST_ACTIVE:
		case CX2SkillTree::ST_SPECIAL_ACTIVE:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_1:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_2:
		case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_3:
			if( -1 == m_pUserSkillTree->GetSkillEquippedSlotIndex( eSkillID, false ) &&
				-1 == m_pUserSkillTree->GetSkillEquippedSlotIndex( eSkillID, true ) )
			{
				vecCandidate.push_back( vecSkillID_[i] );
			}
			break;

		default:
			break;
		}
	}

	UINT uiNextA = 0;
	UINT uiNextB = 0;
	for( UINT i = 0; i < vecCandidate.size(); ++i )
	{
		if( vecEmptyA.size() <= uiNextA )
		{
			if( uiNextB < vecEmptyB.size() )
			{
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( vecEmptyB[ uiNextB ] + EQUIPPED_SKILL_SLOT_COUNT,
					static_cast<CX2SkillTree::SKILL_ID>( vecCandidate[i] ) );
				++uiNextB;
			}
		}
		else
		{
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( vecEmptyA[ uiNextA ], static_cast<CX2SkillTree::SKILL_ID>( vecCandidate[i] ) );
			++uiNextA;
		}
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] ACQ auto-equip: %d given, %d candidate(s), free A %d / B %d, queued A %d / B %d",
		static_cast<int>( vecSkillID_.size() ), static_cast<int>( vecCandidate.size() ), static_cast<int>( vecEmptyA.size() ),
		static_cast<int>( vecEmptyB.size() ), static_cast<int>( uiNextA ), static_cast<int>( uiNextB ) );

	return true;
}
/// June 328dfab0 ( the lock half of UpdateSkillEquipedSlotUI ): A-TC, B-TC and B lock overlays; a skill sitting in a slot that is locked is unequipped
void CX2UISkillTreeNew::UpdateJuneSkillSlotLockUI()
{
	if( NULL == m_pDLGGameSkillSlot || NULL == m_pUserSkillTree )
		return;

	/// A-TC ( slot 4 ) is locked while CX2Unit +0x3c ( transcendence ) is 0. March has no such unit state: Stage 6 adds it, so the slot stays locked
	const bool bTranscendent = false;
	if( false == bTranscendent )
	{
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_A_TC", L"LOCK_SLOT_A_TC", true );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		JuneClearSlotCoolTime( m_pDLGGameSkillSlot, false, 4, 5 );
	}
	else
	{
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_A_TC", L"LOCK_SLOT_A_TC", false );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}

	/// B-TC ( slot 9 ): slot-state key 1 ( 32890ea0( 1 ) ) in { 1, 2 } unlocks it
	const bool bBTCLocked = ( false == m_pUserSkillTree->IsJuneSlotUsable( 1 ) );
	if( false == bBTCLocked )
	{
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_B_TC", L"LOCK_SLOT_B_TC", false );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}
	else
	{
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_B_TC", L"LOCK_SLOT_B_TC", true );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		JuneClearSlotCoolTime( m_pDLGGameSkillSlot, true, 4, 5 );

		CX2EquippedSkillSlotData* pSlotData = GetEquippedSlotData( 4, true );
		if( NULL != pSlotData && CX2SkillTree::SI_NONE != pSlotData->m_eSkillID && -1 != static_cast<int>( pSlotData->m_eSkillID ) )
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), CX2SkillTree::SI_NONE );
	}

	/// B ( slots 5-8 ): slot-state key 0 ( 32890ea0( 0 ) ) in { 1, 2 }; the emulator sends key 0 = 2, so the offline game never locks it
	const bool bBLocked = ( false == m_pUserSkillTree->IsJuneSlotUsable( 0 ) );
	if( false == bBLocked )
	{
		m_bIsExpandSkillSlot = true;
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot", L"LOCK", false );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}
	else
	{
		m_bIsExpandSkillSlot = false;
		JuneSetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot", L"LOCK", true );
		JuneSetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		JuneClearSlotCoolTime( m_pDLGGameSkillSlot, true, 0, 4 );

		for( int i = 0; i < 4; ++i )
		{
			CX2EquippedSkillSlotData* pSlotData = GetEquippedSlotData( i, true );
			if( NULL != pSlotData && CX2SkillTree::SI_NONE != pSlotData->m_eSkillID && -1 != static_cast<int>( pSlotData->m_eSkillID ) )
			{
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), CX2SkillTree::SI_NONE );
				break;
			}
		}
	}

	/// June hides the class statics that belong to the game when the dialog is shown outside one ( Elsword way of sword in the existing March block, Rose glow here )
	if( NULL == g_pX2Game )
	{
		CKTDGUIStatic* pStaticRoseGlow = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_RoseGlow_B" );
		if( NULL != pStaticRoseGlow )
			pStaticRoseGlow->SetShow( false );
		pStaticRoseGlow = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_RoseGlow" );
		if( NULL != pStaticRoseGlow )
			pStaticRoseGlow->SetShow( false );
	}

	/// one line per change of the lock state, not per call
	static int s_iLastLockState = -1;
	const int iLockState = ( true == bBLocked ? 4 : 0 ) | ( true == bBTCLocked ? 2 : 0 ) | ( false == bTranscendent ? 1 : 0 );
	if( s_iLastLockState != iLockState )
	{
		s_iLastLockState = iLockState;
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI slot locks: A-TC %s ( transcendence is Stage 6 ), B-TC %s ( key 1 state %d ), B %s ( key 0 state %d ), B expand flag %d",
			( false == bTranscendent ) ? L"locked" : L"open", ( true == bBTCLocked ) ? L"locked" : L"open", m_pUserSkillTree->GetJuneSlotState( 1 ),
			( true == bBLocked ) ? L"locked" : L"open", m_pUserSkillTree->GetJuneSlotState( 0 ), ( true == m_bIsExpandSkillSlot ) ? 1 : 0 );
	}
}

/// June 328daed0 ( UI type B ) / 328db100 ( UI type A ): where the TC lock pictures and buy-guide buttons sit for the current swap state
void CX2UISkillTreeNew::SetOffsetPosJuneTCSlotUI( const bool bTypeA_ )
{
	if( NULL == m_pDLGGameSkillSlot )
		return;

	D3DXVECTOR2 vOffsetA( 0.f, 0.f );
	D3DXVECTOR2 vOffsetB( 0.f, 0.f );
	if( true == bTypeA_ )
	{
		if( true == m_bSkillSlotSwap )
		{
			vOffsetA.y = -47.f;
			vOffsetB.y = 47.f;
		}
	}
	else
	{
		if( true == m_bSkillSlotSwap )
		{
			vOffsetA.y = -200.f;
			vOffsetB.y = 48.f;
		}
		else
		{
			vOffsetB.y = -200.f;
		}
	}

	CKTDGUIStatic* pStaticLockA = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"LOCK_SLOT_A_TC" );
	if( NULL != pStaticLockA )
		pStaticLockA->SetOffsetPos( vOffsetA );
	CKTDGUIStatic* pStaticLockB = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"LOCK_SLOT_B_TC" );
	if( NULL != pStaticLockB )
		pStaticLockB->SetOffsetPos( vOffsetB );

	CKTDGUIButton* pButtonBuyA = (CKTDGUIButton*) m_pDLGGameSkillSlot->GetControl( L"Button_Buy_Guide_Slot_A_TC" );
	if( NULL != pButtonBuyA )
	{
		pButtonBuyA->SetOffsetPos( vOffsetA );
		if( false == bTypeA_ )
			pButtonBuyA->SetGuideDescOffsetPos( D3DXVECTOR2( 0.f, 100.f ) );
	}
	CKTDGUIButton* pButtonBuyB = (CKTDGUIButton*) m_pDLGGameSkillSlot->GetControl( L"Button_Buy_Guide_Slot_B_TC" );
	if( NULL != pButtonBuyB )
	{
		pButtonBuyB->SetOffsetPos( vOffsetB );
		if( false == bTypeA_ )
			pButtonBuyB->SetGuideDescOffsetPos( D3DXVECTOR2( 0.f, 100.f ) );
	}
}
//}}
void CX2UISkillTreeNew::UpdateSkillEquipedSlotUI()
{
	if(m_pUserSkillTree != NULL)
	{
		for(int i=0; i<10; ++i)
		{
			bool bSlotB = (i > 4) ? true : false;
			int iSlotIndex = (i > 4) ? i-5 : i;
			CX2EquippedSkillSlotData* pSlotData = GetEquippedSlotData(iSlotIndex, bSlotB);
			if(pSlotData != NULL)
			{
				pSlotData->m_bDataUpdated = true;
			}
		}

		if(m_pDLGGameSkillSlot != NULL)
		{
			UpdateJuneSkillSlotLockUI();
		}

		if( NULL == g_pX2Game && NULL != m_pDLGGameSkillSlot )
		{
			CKTDGUIStatic* pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_EL_SKILL_B_B" );
			CKTDGUIStatic* pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_EL_SKILL_R_B" );
			if( NULL != pStatic_ElSkillVigor && NULL != pStatic_ElSkillDest)
			{
				pStatic_ElSkillVigor->SetShow(false);
				pStatic_ElSkillDest->SetShow(false);
			}

			pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_EL_SKILL_B" );
			pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_EL_SKILL_R" );
			if( NULL != pStatic_ElSkillVigor && NULL != pStatic_ElSkillDest)
			{
				pStatic_ElSkillVigor->SetShow(false);
				pStatic_ElSkillDest->SetShow(false);
			}
		}

	}
}

void CX2UISkillTreeNew::ShowUIDesc( bool bShow, wstring wstr /*= L""*/, D3DXVECTOR2 pos /*= D3DXVECTOR2(0,0)*/, CX2TalkBoxManagerImp::TALK_BOX_TYPE TBT /*= CX2TalkBoxManagerImp::TBT_FROM_UP_RIGHT*/, D3DXCOLOR coTextColor /*= D3DXCOLOR(0,0,0,1)*/, D3DXCOLOR coBackColor /*= D3DXCOLOR(1, 0.96f, 0.6f, 1 )*/)
{
	m_pTalkBoxManager->Clear();

	if(bShow)
	{
		CX2TalkBoxManagerImp::TalkBox talkBox;

		talkBox.m_vPos				= pos;
		talkBox.m_GameUnitType		= CX2GameUnit::GUT_USER;
		talkBox.m_fRemainTime		= 2.0f;

		talkBox.m_wstrTalkContent	= wstr.c_str();
		talkBox.m_bTraceUnit		= false;
		talkBox.m_TalkBoxType		= TBT;
		talkBox.m_coTextColor		= coTextColor;
		talkBox.m_BackTexColor		= coBackColor;

		m_pTalkBoxManager->Push( talkBox );

	}

}

//wstring CX2UISkillTreeNew::GetExpirationDateDesc( const wstring& wstrExpirationDate )
//{
//	wstring tempDesc = L"";
//
//
//	tempDesc = GET_STRING( STR_ID_309 );

//
//	CTime cTime;
//	KncUtil::ConvertStringToCTime( wstrExpirationDate, cTime );
//
//	wstringstream wstrstm;
//

//	//tempDesc += wstrstm.str().c_str();
//	//tempDesc += L"\n";
//
//	wstrstm.str( L"" );
//	wstrstm << L"\n";
//
//	//time_t t0 = time(0);
//	__time64_t t0 = g_pData->GetServerCurrentTime();
//
//	double fDiffTime = difftime( KncUtil::TimeToInt( cTime ), (time_t)t0 );
//
//	int remainDay =  (int)(fDiffTime / ( 60 * 60 * 24 ) );
//	if ( remainDay > 0 )
//		wstrstm << remainDay << GET_STRING( STR_ID_14 ) << L" ";
//
//	int remainHour = ((time_t)fDiffTime % ( 60 * 60 * 24 )) / (60 * 60);
//	if ( remainHour < 0 )
//		remainHour = 0;
//
//	wstrstm << remainHour << GET_STRING( STR_ID_310 ) << L" ";
//
//	int remainMinute = ((time_t)fDiffTime % ( 60 * 60 )) / (60 );
//
//	if ( remainDay <= 0 )
//	{
//		if ( remainMinute <= 0 )
//			remainMinute = 0;
//		wstrstm << remainMinute << GET_STRING( STR_ID_311 ) << L" ";
//	}
//
//	tempDesc += wstrstm.str().c_str();
//
//
//	return tempDesc;
//
//}

void CX2UISkillTreeNew::UpdateDLGSkillUnsealed( CX2SkillTree::SKILL_ID eSkillID )
{
	if( m_pDLGSkillUnsealed == NULL )
		return;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
		return;

	CKTDGUIStatic* pStaticSkillSlot = (CKTDGUIStatic*) m_pDLGSkillUnsealed->GetControl( L"g_pStaticSkill_Acquisition_Slot" );
	if( NULL != pStaticSkillSlot &&
		NULL != pStaticSkillSlot->GetPicture(1) )
	{
		std::string textureName = "";
		std::string texturePieceName = "";

		ConvertWCHARToChar( textureName, pSkillTemplet->m_wstrIconName.c_str() );
		ConvertWCHARToChar( texturePieceName, pSkillTemplet->m_wstrIconPieceName.c_str() );

		pStaticSkillSlot->GetPicture(1)->SetTex_LUA( textureName.c_str(), texturePieceName.c_str() );
	}

	CKTDGUIStatic* pStaticSkillName = (CKTDGUIStatic*) m_pDLGSkillUnsealed->GetControl( L"Static_Skill_Name" );
	if( NULL != pStaticSkillName )
	{
		pStaticSkillName->GetString(0)->msg = pSkillTemplet->m_wstrName;
	}

	g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( false, false, true );
}

void CX2UISkillTreeNew::UpdateSkillCoolTime( float fElapsedTime )
{
	if( g_pData != NULL &&
		g_pData->GetMyUser() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree;

		if ( m_pDLGGameSkillSlot != NULL )
		{
			CKTDGUIStatic* pStaticCoolTime = (CKTDGUIStatic*)m_pDLGGameSkillSlot->GetControl( L"Static_Skill_Cool_Time" );
			CKTDGUIStatic* pStatic_Skill_Cool_Time_Fade = (CKTDGUIStatic*)m_pDLGGameSkillSlot->GetControl( L"Static_Skill_Cool_Time_Fade" );

			CKTDGUIStatic* pStaticSlot_A = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_SKILL_SLOT_A" );
			CKTDGUIStatic* pStaticSlot_B = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"Static_SKILL_SLOT_B" );
			for ( int i = 0; i < 5; i++ )
			{
				pStaticSlot_A->GetPicture(i)->SetColor( D3DXCOLOR(1.f,1.f,1.f,0.3f));
				pStaticSlot_B->GetPicture(i)->SetColor( D3DXCOLOR(1.f,1.f,1.f,0.3f));
			}
			if ( pStaticCoolTime != NULL && pStatic_Skill_Cool_Time_Fade != NULL )
			{
				for ( int i = 0; i < 10; i++ )
				{
					bool bSlotB = (i > 4) ? true : false;
					int iSlotIndex = (i > 4) ? i-5 : i;

					if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
						&& bSlotB == true)
					{
						break;
					}

					if( true == bSlotB )
					{
						pStatic_Skill_Cool_Time_Fade = (CKTDGUIStatic*)m_pDLGGameSkillSlot->GetControl( L"Static_Skill_Cool_Time_Fade_B" );
						if( NULL == pStatic_Skill_Cool_Time_Fade )
							break;

						pStaticCoolTime = (CKTDGUIStatic*)m_pDLGGameSkillSlot->GetControl( L"Static_Skill_Cool_Time_B" );
						if( NULL == pStaticCoolTime )
							break;
					}

					const CX2UserSkillTree::SkillSlotData* pSlotData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB);
					refUserSkillTree.SetSkillCoolTimeLeft(CX2SkillTree::SI_NONE,0.f);
					if(pSlotData != NULL)
					{
						if( NULL != pStaticSlot_A && NULL != pStaticSlot_B )
						{
							if( CX2SkillTree::SI_NONE != pSlotData->m_eID )
							{
								if( true == bSlotB )
								{
									pStaticSlot_B->GetPicture(iSlotIndex)->SetColor( D3DXCOLOR(1.f,1.f,1.f,1.f));
								}
								else
								{
									pStaticSlot_A->GetPicture(iSlotIndex)->SetColor( D3DXCOLOR(1.f,1.f,1.f,1.f));
								}
							}
						}

						if(pSlotData->m_fCoolTimeLeft > 0.0f)
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							wstringstream wstrstm;
							wstrstm << (int)( ceil(pSlotData->m_fCoolTimeLeft) );
							pStaticCoolTime->SetString( iSlotIndex, wstrstm.str().c_str() );
						}
						else
						{
							if ( pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
							{
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;
							}

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
					}
				}
			}
		}
	}
}

void CX2UISkillTreeNew::SwapSkillSlot(bool bVal, bool bForce/* = false*/)
{

	if( true == m_bIsSkillUITypeA )
	{
		SwapSkillSlotTypeA( bVal, bForce );
	}
	else
	{
		SwapSkillSlotTypeB( bVal, bForce );
	}
}

void CX2UISkillTreeNew::SetOffsetPosSkillSLotStatic(const D3DXVECTOR2& vPos_, const D3DXVECTOR2& vAddPos_/* = D3DXVECTOR2(0,0)*/)
{
	if( NULL == m_pDLGGameSkillSlot )
		return;

	SetOffsetStatic( L"Static_Skill_Cool_Time_Fade", -vPos_ -vAddPos_);
	SetOffsetStatic( L"Static_Skill_Cool_Time_Fade_B", vPos_ );
	SetOffsetStatic( L"Static_Skill_Cool_Time", -vPos_ -vAddPos_);
	SetOffsetStatic( L"Static_Skill_Cool_Time_B", vPos_ );
	SetOffsetStatic( L"Static_SKILL_SLOT_A", -vPos_ -vAddPos_ );
	SetOffsetStatic( L"Static_SKILL_SLOT_B", vPos_ );
	SetOffsetStatic( L"Static_EL_SKILL_B", -vPos_ -vAddPos_ );
	SetOffsetStatic( L"Static_EL_SKILL_B_B", vPos_ );
	SetOffsetStatic( L"Static_EL_SKILL_R", -vPos_ -vAddPos_ );
	SetOffsetStatic( L"Static_EL_SKILL_R_B", vPos_ );
	SetOffsetStatic( L"Static_RoseGlow", -vPos_ -vAddPos_ );	/// June 328dab10
	SetOffsetStatic( L"Static_RoseGlow_B", vPos_ );
}

void CX2UISkillTreeNew::SetOffsetStatic( const WCHAR* strControl_, const D3DXVECTOR2& vPos_)
{
	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGGameSkillSlot->GetControl(strControl_));
	if( NULL != pStatic )
	{
		pStatic->SetOffsetPos(vPos_);
	}
}

void CX2UISkillTreeNew::UpdateSkillSlotString()
{
	if ( NULL == m_pDLGGameSkillSlot || NULL == g_pMain || NULL == g_pMain->GetKeyPad() )
		return;

	/// June 328e1170: five key labels per bar ( SkillHotKeyEx = the B bar, SkillHotKey = the A bar ); the B labels are blank in UI type B
	{
		const GAME_ACTION aeActionEx[5] = { GAMEACTION_SKILL_EX1, GAMEACTION_SKILL_EX2, GAMEACTION_SKILL_EX3, GAMEACTION_SKILL_EX4, GAMEACTION_SKILL_EX5 };
		const GAME_ACTION aeAction[5] = { GAMEACTION_SKILL1, GAMEACTION_SKILL2, GAMEACTION_SKILL3, GAMEACTION_SKILL4, GAMEACTION_SKILL5 };

		CKTDGUIStatic* pStaticSkillHotKeyEx = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"SkillHotKeyEx" );
		CKTDGUIStatic* pStaticSkillHotKeyA = (CKTDGUIStatic*) m_pDLGGameSkillSlot->GetControl( L"SkillHotKey" );
		for( int i = 0; i < 5; ++i )
		{
			if( NULL != pStaticSkillHotKeyEx && NULL != pStaticSkillHotKeyEx->GetString(i) )
			{
				if( false == m_bIsSkillUITypeA )
					pStaticSkillHotKeyEx->GetString(i)->msg = L"";
				else
					pStaticSkillHotKeyEx->GetString(i)->msg = g_pMain->GetKeyPad()->GetKeyString( aeActionEx[i] );
			}
			if( NULL != pStaticSkillHotKeyA && NULL != pStaticSkillHotKeyA->GetString(i) )
				pStaticSkillHotKeyA->GetString(i)->msg = g_pMain->GetKeyPad()->GetKeyString( aeAction[i] );
		}
	}
}

void CX2UISkillTreeNew::SetSkillUIType( bool bIsTypeA_ )
{
	if( NULL == m_pDLGGameSkillSlot)
		return;

	m_bIsSkillUITypeA = bIsTypeA_;

	D3DXVECTOR2 vSlotPos(0,0);
	if( false == m_bIsSkillUITypeA )
	{
		vSlotPos.x = -257.f;	/// June 328e1680 ( five slots a bar ); the y is set to June's -619 below
		vSlotPos.y = -599.f;
		vSlotPos.y = -619.f;	/// June 328e1680: ( -257, -619 ) with no wide-bar rule ( user, S6 review 2026-10-18 )
	}

	bool bEnableSlotB = false;
	if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		if( true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB() )
			bEnableSlotB = true;
	}

	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGGameSkillSlot->GetControl( L"g_pStatic_Slot_Guide" ));
	if( NULL != pStatic )
	{
		if( false == m_bIsSkillUITypeA &&
			true == bEnableSlotB )
			pStatic->SetShow(true);
		else
			pStatic->SetShow(false);
	}

 	m_pDLGGameSkillSlot->SetPos( vSlotPos );

	m_pDLGGameSkillSlot->GetShow( );

	SetOffsetPosEtcSkillSlotUI();
	UpdateSkillSlotString();
	SwapSkillSlot( m_bSkillSlotSwap, true );
}

void CX2UISkillTreeNew::SetOffsetPosEtcSkillSlotUI()
{
	if( NULL == m_pDLGGameSkillSlot )
		return;

	D3DXVECTOR2 vOffsetPos(0,0);
	if( false == m_bIsSkillUITypeA )
		vOffsetPos.y = -200.f;

	CKTDGUIStatic* pStaticLock = static_cast<CKTDGUIStatic*>(m_pDLGGameSkillSlot->GetControl(L"LOCK"));
	if( NULL != pStaticLock )
		pStaticLock->SetOffsetPos(vOffsetPos);

	CKTDGUIButton* pButtonBuyGuide = static_cast<CKTDGUIButton*>(m_pDLGGameSkillSlot->GetControl(L"Button_Buy_Guide_Slot"));
		pButtonBuyGuide->SetOffsetPos(vOffsetPos);

}

void CX2UISkillTreeNew::SwapSkillSlotTypeA(bool bVal, bool bForce/* = false*/)
{
	if( m_bSkillSlotSwap == bVal && false == bForce )
		return;

	m_bSkillSlotSwap = bVal;
	if( true == m_bSkillSlotSwap )
	{
		D3DXVECTOR2 vOffsetPos(0,0);
		for( int i = 0 ; i < 10; ++i )
		{
			vOffsetPos.y = ( i >= 5 ) ? 47.f : -47.f;
			if( NULL != GetEquippedSlot(i) )
			{
				GetEquippedSlot(i)->SetOffsetPos( vOffsetPos );
			}
		}
		vOffsetPos.y = 47.f;	/// June 328db100: the slot pitch is 47 ( 51 - 4 )
		SetOffsetPosSkillSLotStatic(vOffsetPos);
	}
	else
	{
		for( int i = 0 ; i < 10; ++i )
		{
			if( NULL != GetEquippedSlot(i) )
			{
				GetEquippedSlot(i)->SetOffsetPos(D3DXVECTOR2(0,0));
			}
		}

		SetOffsetPosSkillSLotStatic(D3DXVECTOR2(0,0));
	}
	SetOffsetPosJuneTCSlotUI( true );
}

void CX2UISkillTreeNew::SwapSkillSlotTypeB(bool bVal_, bool bForce_/* = false */)
{
	if( m_bSkillSlotSwap == bVal_ && false == bForce_)
		return;

	m_bSkillSlotSwap = bVal_;

	D3DXVECTOR2 vOffsetPos(0,0);
	D3DXVECTOR2 vOffsetAddPos(0,0);

	if( true == m_bSkillSlotSwap )
	{
		for( int i = 0 ; i < 10; ++i )
		{
			vOffsetPos.y = ( i >= 5 ) ? 47.f : -200.f;
			if( NULL != GetEquippedSlot(i) )
				GetEquippedSlot(i)->SetOffsetPos( vOffsetPos );
		}
		vOffsetPos.y = 47.f;	/// June 328daed0: the slot pitch is 47 ( 51 - 4 )
		vOffsetAddPos.y = 200.f;

	}
	else
	{
		for( int i = 0 ; i < 10; ++i )
		{
			if( NULL != GetEquippedSlot(i) )
				vOffsetPos.y = ( i >= 5 ) ? -150.f : 0.f;
				GetEquippedSlot(i)->SetOffsetPos( vOffsetPos );
		}

		vOffsetPos.y = -200.f;
		vOffsetAddPos.y = 200.f;
	}
	SetOffsetPosSkillSLotStatic(vOffsetPos, vOffsetAddPos);
	SetOffsetPosJuneTCSlotUI( false );
}

void CX2UISkillTreeNew::UpdateSkillPageUI()
{
	CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree;

	const USHORT usUsingPage = refUserSkillTree.GetUsingPage();
	const USHORT usOpenedPage = refUserSkillTree.GetOpenedPage();
	const USHORT usSelectPage = refUserSkillTree.GetSelectPage();
	D3DXVECTOR2 vPos( 0.f, 0.f );

	CKTDGUIRadioButton* pRadioButton_Page1 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page1" ) );
	CKTDGUIRadioButton* pRadioButton_Page2 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page2" ) );
	CKTDGUIRadioButton* pRadioButton_Page3 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page3" ) );
	CKTDGUIButton* pButton_Add_Page_ED = reinterpret_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"Add_Skill_Page_ED" ) );
	CKTDGUIButton* pButton_Add_Page_Cash = reinterpret_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"Add_Skill_Page_Cash" ) );
	CKTDGUIButton* pButton_Decide_Page = reinterpret_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"Decide_Skill_Page" ) );

	if ( pRadioButton_Page1 == NULL || pRadioButton_Page2 == NULL || pRadioButton_Page3 == NULL || pButton_Add_Page_ED == NULL
		|| pButton_Decide_Page == NULL )
		return;

	if ( refUserSkillTree.IsUsingPage() )
		pButton_Decide_Page->SetShowEnable( true, false );
	else
		pButton_Decide_Page->SetShowEnable( true, true );

	switch ( usSelectPage )
	{
	case 1:
		{
			pRadioButton_Page1->SetChecked( true );
		} break;

	case 2:
		{
			pRadioButton_Page2->SetChecked( true );
		} break;

	case 3:
		{
			pRadioButton_Page3->SetChecked( true );
		} break;
	default:
		break;
	}

	bool bPage1, bPage2, bPage3, bAdd;
	bPage1 = bPage2 = bPage3 = bAdd = false;

	switch ( usOpenedPage )
	{
	case 1:
		{
			bPage1 = bAdd = true;
			bPage2 = bPage3 = false;

			vPos = pRadioButton_Page2->GetPos();
		} break;

	case 2:
		{
			bPage1 = bPage2 = bAdd = true;
			bPage3 = false;

			vPos = pRadioButton_Page3->GetPos();
		} break;

	case 3:
		{
			bPage1 = bPage2 = bPage3 = true;
			bAdd = false;
		} break;

	default:
		{
			bPage1 = bPage2 = bPage3 = bAdd = false;
		} break;
	}

	pRadioButton_Page1->SetShowEnable( bPage1, bPage1 );
	pRadioButton_Page2->SetShowEnable( bPage2, bPage2 );
	pRadioButton_Page3->SetShowEnable( bPage3, bPage3 );

	if ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
	{
		pButton_Add_Page_ED->SetOffsetPos( vPos );
		pButton_Add_Page_ED->SetShowEnable( bAdd, bAdd );
		pButton_Add_Page_ED->SetGuideDesc( GET_STRING( STR_ID_29366 ) );
		pButton_Add_Page_ED->SetGuideDescOffsetPos( D3DXVECTOR2( 100, 0 ) );
		vPos.x += 27.f;
		pButton_Add_Page_Cash->SetOffsetPos( vPos );
		pButton_Add_Page_Cash->SetShowEnable( bAdd, bAdd );
		pButton_Add_Page_Cash->SetGuideDesc( GET_STRING( STR_ID_29367 ) );
		pButton_Add_Page_Cash->SetGuideDescOffsetPos( D3DXVECTOR2( 100, 0 ) );
	}
	else
	{
		pButton_Add_Page_ED->SetShowEnable( false, false );
		pButton_Add_Page_Cash->SetShowEnable( false, false );
	}
}

void CX2UISkillTreeNew::BuySkillPageDialog( const int iED_ )
{
	if ( g_pData && g_pData->GetMyUser() && g_pData->GetMyUser()->GetSelectUnit() )
	{
		wstringstream	wstrstm;

		m_iConfirmED = iED_;

		wstrstm << GET_REPLACED_STRING( ( STR_ID_29037, "LL", g_pMain->GetEDString( m_iConfirmED  ),
			g_pMain->GetEDString( g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED ) ) );

		m_pDLGBuyConfirm = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrstm.str().c_str(), STUCM_BUY_CONFIRM,
			g_pMain->GetNowState(), STUCM_BUY_CANCEL, L"", D3DXVECTOR2( 0, 40 ) );
	}
}

/////////////////////////////////////////////////////////////////////////
// GetSkillTitleDesc
/////////////////////////////////////////////////////////////////////////

/// June 328e5ae0 ( 4 stack args, ret 0x10, ECX unused: ( out, templet, level, mode ) ). The title: skill name ( with the active memos' names
/// in front ), then the "Lv.N" line when the templet shows its level.
void CX2UISkillTreeNew::GetSkillTitleDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iSkillLevel, IN int iDescMode_ ) const
{
	/// June 328e5b2f..328e5b6a: templet, g_pData, my user, its select unit ( +0x68 / +0x90 ), the socket item ( +0x280 ), the skill tree ( +0x50 )
	if ( NULL == pSkillTemplet ||
		 NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetSocketItem() ||
		 NULL == g_pData->GetSkillTree() )
		return;

	/// June 328e5b70..328e5ccb: mode 1 ( the tree / equipped tooltip ) honours the transcendence memo's alternate skill
	if ( 1 == iDescMode_ )
	{
		/// June 323511f0( templet id ): the alternate skill, 0 when none
		const int iAltSkillID = JuneGetMyUnitTranscendenceSkillIDForUI( static_cast<int>( pSkillTemplet->m_eID ) );
		if ( iAltSkillID < 1 )
		{
			/// June @328e5bd2: the skill's own name ( assign )
			wstrDesc = pSkillTemplet->m_wstrName;

			/// June @328e5bd7..328e5cae: the base skill ( +0x178 m_DeleteSkillIDByUpgrade, else the id ) and its active memos
			int iBaseSkillID = pSkillTemplet->m_iDeleteSkillIDByUpgrade;
			if ( 0 == iBaseSkillID )
				iBaseSkillID = static_cast<int>( pSkillTemplet->m_eID );

			std::vector<int> vecMemoID;
			g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetJuneActiveMemoList( iBaseSkillID, vecMemoID );	/// June 32891e10

			for ( std::vector<int>::const_iterator it = vecMemoID.begin(); it != vecMemoID.end(); ++it )
			{
				/// June 324550a0() -> 32453420( out, memo id ): the memo's name text
				wstring wstrMemoName = L"";
				/// S7c: 32453420 CX2SkillNoteManager memo-name text builder ( 711 lines, not ported ): wstrMemoName = CX2SkillNoteManager::GetInstance()->( 32453420 )( *it )
				if ( false == wstrMemoName.empty() )	/// S7b stand-in: June has no test; until 32453420 is ported the name is empty and nothing is prepended
				{
					/// June 31aab330 + 323e7600 + 31aa92b0: wstrDesc = ( name + L" " ) + wstrDesc
					wstrDesc = ( wstrMemoName + L" " ) + wstrDesc;
				}
			}
		}
		else
		{
			/// June @328e5b9c..328e5bc4: the alternate skill's name; no templet -> wstrDesc is left as it was
			const CX2SkillTree::SkillTemplet* pAltTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) );
			if ( NULL != pAltTemplet )
				wstrDesc = pAltTemplet->m_wstrName;
		}
	}
	else
	{
		/// June @328e5cc4: mode 2 ( the memo popup ) and any other mode: the skill's own name
		wstrDesc = pSkillTemplet->m_wstrName;
	}

	/// June @328e5cd2: 3342f1b8 = L"\n". March also wrapped the name with CWordLineHandler ( CLIENT_GLOBAL_LINEBREAK ); June does not.
	wstrDesc += L"\n";

	/// June @328e5cde: templet +0x9c == 1 ( m_bShowSkillLevel, the same byte 328e18d0 tests ). June has no "(+N)" form and no max-level word.
	if ( true == pSkillTemplet->m_bShowSkillLevel )
	{
		/// June @328e5ceb: wstrLevelTitle is built from L"" ( 3344f1f4 ) and never assigned, so the third %s is always empty ( the March
		/// STR_ID_25113 max-level text is gone ). Kept as June has it.
		wstring wstrLevelTitle = L"";

		WCHAR buf[256] = {0,};
		/// June @328e5dd1: 334c38f0 = L"%s.%d #CFF0000%s#CX\n"; GET_STRING( 0x1e9 = 489 ) is the "Lv" word
		StringCchPrintf( buf, 256, L"%s.%d #CFF0000%s#CX\n", GET_STRING( STR_ID_489 ), iSkillLevel, wstrLevelTitle.c_str() );

		wstrDesc += buf;
	}
}

/////////////////////////////////////////////////////////////////////////
// GetSkillSubDesc
/////////////////////////////////////////////////////////////////////////

/// June 328e2900 ( __thiscall, ret 0x14: ( templet, level, OUT string, mode, line count ) ). The text beside the title: "[type]", the MP /
/// DP / FP cost line and the cool time. The caller ( ShowSkillDesc 328e9160 @328e9635 ) passes max( level, 1 ) as the level.
/// The argument ORDER here is the one the header declares ( OUT first ); June's is ( templet, level, OUT ).
void CX2UISkillTreeNew::GetSkillSubDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iSkillLevel, IN int iDescMode_, IN int iLineCount_ ) const
{
	/// June 328e296a..328e2990: templet, g_pData, my user, its select unit
	if ( NULL == pSkillTemplet ||
		 NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	/// June 328e2996..328e29dc: mode 1 only; the alternate skill's templet ( 323511f0 + GetSkillTemplet( id, 1 ) ), NULL when none
	const CX2SkillTree::SkillTemplet* pAltTemplet = NULL;
	if ( 1 == iDescMode_ && NULL != g_pData->GetSkillTree() )
	{
		const int iAltSkillID = JuneGetMyUnitTranscendenceSkillIDForUI( static_cast<int>( pSkillTemplet->m_eID ) );
		pAltTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) );
	}

	WCHAR buf[256] = {0,};
	bool bIsPassive = false;	/// June local_cf9 ( [esp+0x2b] )

	/// June 328e2a0c..328e37d1: the "[type]" label. June's SKILL_TYPE numbering ( Enum.lua ) = this switch.
	switch ( static_cast<int>( pSkillTemplet->m_eType ) )
	{
	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:	/// 1
	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:		/// 2
	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:	/// 3
	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:	/// 4
	case CX2SkillTree::ST_PASSIVE:					/// 8
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40580 ) );		/// June GET_STRING( 0x9e84 )
			bIsPassive = true;
		} break;
	case CX2SkillTree::ST_BUFF:						/// 5
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40577 ) );		/// June 0x9e81
		} break;
	case CX2SkillTree::ST_ACTIVE:					/// 6
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40578 ) );		/// June 0x9e82
		} break;
	case CX2SkillTree::ST_SPECIAL_ACTIVE:			/// 7
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40579 ) );		/// June 0x9e83
		} break;
	case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:	/// 9
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40581 ) );		/// June 0x9e85
			bIsPassive = true;
		} break;
	case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:		/// 10
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40582 ) );		/// June 0x9e86
			bIsPassive = true;
		} break;
	case CX2SkillTree::ST_GUILD_PASSIVE:			/// 11
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40583 ) );		/// June 0x9e87
			bIsPassive = true;
		} break;
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:		/// 12
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40584 ) );		/// June 0x9e88
		} break;
	case 13:	/// ST_HYPER_ACTIVE_SKILL
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40585 ) );		/// June 0x9e89
		} break;
	case 14:	/// ST_SPECIAL_ACTIVE_GRADE_1
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40586 ) );		/// June 0x9e8a
		} break;
	case 15:	/// ST_SPECIAL_ACTIVE_GRADE_2
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40587 ) );		/// June 0x9e8b
		} break;
	case 16:	/// ST_SPECIAL_ACTIVE_GRADE_3
		{
			StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40588 ) );		/// June 0x9e8c
		} break;
	default:	/// 0, 17 ST_CORE, 18 ST_CORE_PASSIVE, anything else. June @328e35da: this +0x120 byte, never written inside the class block.
		{
			/// June does NOT set bIsPassive here ( March did ): these types always get the MP and cool time lines, even at 0
			if ( true == m_bReplacedEsCounter )
				StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40578 ) );	/// June 0x9e82
			else
				StringCchPrintf( buf, 256, L"[%s]", GET_STRING( 40572 ) );	/// June 0x9e7c
		} break;
	}

	/// June @328e37d1..328e37fe: ( line count < 0 ) -> the string is replaced by 33442638 = L"\r\n"; else one L"\n" per title line, so the
	/// lines sit under the right title row. wstrDesc is not cleared first.
	if ( iLineCount_ < 0 )
	{
		wstrDesc = L"\r\n";
	}
	else
	{
		for ( int i = 0; i < iLineCount_; ++i )
			wstrDesc.append( L"\n", 1 );
	}

	/// June @328e3801..328e3846: 322055c0() = the skill tree's +0xb4 map<SKILL_TYPE, colour> ( SkillTypeColor.lua ); 3265c7f0 finds m_eType
	/// ( templet +4 ) and a hit appends the colour code in front of the label
	const wstring* pTypeColor = s_kJuneSkillTypeColorTable.Find( static_cast<int>( pSkillTemplet->m_eType ) );
	if ( NULL != pTypeColor )
		wstrDesc.append( *pTypeColor );

	/// June @328e384c: append the label ( buf )
	wstrDesc += buf;

	/// June @328e3875..328e38a4: the MP cost of the BASE templet, truncated ( the inline of 32071780 over templet +0x130 ), only to gate the line
	const int iGateMPConsumption = static_cast<int>( pSkillTemplet->GetSkillMPConsumptionValue( iSkillLevel ) );

	/// June @328e38a9..328e38bd: a passive shows the MP line only when the cost is above 0; every other type always shows it
	if ( ( true == bIsPassive && 0 < iGateMPConsumption ) || false == bIsPassive )
	{
		/// June @328e38c7: the unit type is *( select unit +0xaf0 ) -> first dword
		const int iUnitType = static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetType() );

		if ( CX2Unit::UT_ADD == iUnitType )	/// June *( +0xaf0 ) == 9
		{
			/// June @328e38ec..328e3b26: the BASE templet even when an alternate skill exists. MP, then DP ( FP / 20, integer divide )
			const int iConsumeMP = static_cast<int>( pSkillTemplet->GetSkillMPConsumptionValue( iSkillLevel ) );	/// June 32071780
			const int iConsumeDP = static_cast<int>( pSkillTemplet->m_usFPConsumtion / DISPLAY_DP_RATE );			/// June word +0x10 / 20

			if ( 0 < iConsumeMP )
			{
				StringCchPrintf( buf, 256, L"\n%s%d", GET_STRING( STR_ID_2648 ), iConsumeMP );		/// June 334c3844, 0xa58 = 2648
				wstrDesc += buf;

				if ( 0 < iConsumeDP )
				{
					StringCchPrintf( buf, 256, L" / %s%d", GET_STRING( STR_ID_29866 ), iConsumeDP );	/// June 334c3834, 0x74aa = 29866
					wstrDesc += buf;
				}
			}
			else if ( 0 < iConsumeDP )
			{
				StringCchPrintf( buf, 256, L"\n%s%d", GET_STRING( STR_ID_29866 ), iConsumeDP );
				wstrDesc += buf;
			}
		}
		else if ( 11 == iUnitType )	/// June *( +0xaf0 ) == 0xb ( June UT_ROSE, not in March's UNIT_TYPE )
		{
			/// June @328e3c57..328e3b26: the alternate skill's templet when there is one. MP, then FP raw ( no / 20 )
			const CX2SkillTree::SkillTemplet* pCostTemplet = ( NULL != pAltTemplet ) ? pAltTemplet : pSkillTemplet;
			const int iConsumeMP = static_cast<int>( pCostTemplet->GetSkillMPConsumptionValue( iSkillLevel ) );
			const int iConsumeFP = static_cast<int>( pCostTemplet->m_usFPConsumtion );

			if ( 0 < iConsumeMP )
			{
				StringCchPrintf( buf, 256, L"\n%s%d", GET_STRING( STR_ID_2648 ), iConsumeMP );
				wstrDesc += buf;

				if ( 0 < iConsumeFP )
				{
					StringCchPrintf( buf, 256, L" / %s%d", GET_STRING( 38426 ), iConsumeFP );		/// June 0x961a
					wstrDesc += buf;
				}
			}
			else if ( 0 < iConsumeFP )
			{
				StringCchPrintf( buf, 256, L"\n%s%d", GET_STRING( 38426 ), iConsumeFP );
				wstrDesc += buf;
			}
		}
		else
		{
			/// June @328e3f40..328e415f: every other unit: the MP line only, from the alternate skill's templet when there is one
			const CX2SkillTree::SkillTemplet* pCostTemplet = ( NULL != pAltTemplet ) ? pAltTemplet : pSkillTemplet;
			StringCchPrintf( buf, 256, L"\n%s%d", GET_STRING( STR_ID_2648 ), static_cast<int>( pCostTemplet->GetSkillMPConsumptionValue( iSkillLevel ) ) );
			wstrDesc += buf;
		}
	}

	/// June @328e3b26 / @328e4164: the cool time, from the alternate skill's templet when there is one ( 320717b0 over templet +0x120 )
	const CX2SkillTree::SkillTemplet* pCoolTimeTemplet = ( NULL != pAltTemplet ) ? pAltTemplet : pSkillTemplet;
	const int iSkillCoolTime = static_cast<int>( pCoolTimeTemplet->GetSkillCoolTimeValue( iSkillLevel ) );

	/// June @328e4179..328e41f5: a passive shows it only when above 0; every other type always
	if ( ( true == bIsPassive && 0 < iSkillCoolTime ) || false == bIsPassive )
	{
		wstrDesc += L"\n";
		wstrDesc += GET_REPLACED_STRING( ( STR_ID_25070, "i", iSkillCoolTime ) );	/// June 0x61ee = 25070, "i"
	}
}

/////////////////////////////////////////////////////////////////////////
// GetSkillEffectiveDesc
/////////////////////////////////////////////////////////////////////////

/// June 328e5e60 ( 4 stack args, ret 0x10, ECX unused: ( out, level, templet, mode ) ). The caller is 328ee680 ( ShowSkillDescTail ), which
/// passes ( text, level, templet, mode ) from its bound context. June has NO iIncreasekillLevel argument ( March's 3rd ): the "+N" level
/// from items is not added here, the level is what the caller gives.
void CX2UISkillTreeNew::GetSkillEffectiveDesc( OUT wstring& wstrDesc, IN int iSkillLevel, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iDescMode_ ) const
{
	/// June 328e5ec0..328e5ed8: templet, g_pData, skill tree ( +0x50 )
	if ( NULL == pSkillTemplet ||
		 NULL == g_pData ||
		 NULL == g_pData->GetSkillTree() )
		return;

	/// June 322055a0( templet id ): false for 0x56b8 and 0x56c8 = RNSI_A_AISHA_TELEPORT and _UPGRADE ( june_src_fixed Enum.src.lua:14477, :16593 )
	const int iSkillID = static_cast<int>( pSkillTemplet->m_eID );
	if ( 22200 == iSkillID || 22216 == iSkillID )
		return;

	/// June @328e5eef: 31aa6560( L"\n", 1 ) = append
	wstrDesc += L"\n";

	/// June @328e5eff..328e5f50: mode 1 -> the alternate skill's templet ( 323511f0 + GetSkillTemplet ); no iAlt > 0 test here
	const CX2SkillTree::SkillTemplet* pAltTemplet = NULL;
	if ( 1 == iDescMode_ )
	{
		const int iAltSkillID = JuneGetMyUnitTranscendenceSkillIDForUI( iSkillID );
		pAltTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iAltSkillID ) );
	}

	/// June @328e5f54: the current effect, only for a learned skill
	if ( 0 < iSkillLevel )
	{
		wstrDesc += L"\n";

		/// June @328e5f68..328e5f95: 328ee280( templet, level + 1 ) = GetSkillEffectiveDescValue( level + 1 ), compared with L"" ( 3344f1f4 ).
		/// The "<current effect>" heading ( STR_ID_25071 ) is printed only when the NEXT level has a text ( June quirk: at the top level
		/// the current effect carries no heading )
		const wstring wstrNextLevelEffectiveDesc = pSkillTemplet->GetSkillEffectiveDescValue( iSkillLevel + 1 );
		if ( 0 != wstrNextLevelEffectiveDesc.compare( L"" ) )
		{
			wstrDesc += GET_STRING( STR_ID_25071 );		/// June 0x61ef = 25071
			wstrDesc += L"\n";
		}

		/// June @328e607e..328e60cf: the current level's text, from the alternate skill when there is one ( mode 1 ), else the templet
		if ( NULL != pAltTemplet && 1 == iDescMode_ )
			wstrDesc += pAltTemplet->GetSkillEffectiveDescValue( iSkillLevel );
		else
			wstrDesc += pSkillTemplet->GetSkillEffectiveDescValue( iSkillLevel );
	}

	/// June @328e60e3: the next level's preview, below the level cap ( 0x1e = MAX_LIMITED_SKILL_LEVEL ) and with a select unit
	if ( MAX_LIMITED_SKILL_LEVEL > iSkillLevel &&
		 NULL != g_pData->GetMyUser() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		/// June @328e6111..328e6128: the unit level is the xor-decoded +0xb4 / +0xb8 / +0xbc triple ( March: plain m_Level )
		const int iUnitLevel = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level;
		const int iNextLevel = ( 0 == iSkillLevel ) ? 1 : iSkillLevel + 1;

		const wstring wstrNextEffectiveDesc = pSkillTemplet->GetSkillEffectiveDescValue( iNextLevel );		/// June 328ee280( templet, next )

		/// June @328e6162: HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC is always on in June: no next text -> no preview
		if ( 0 != wstrNextEffectiveDesc.compare( L"" ) )
		{
			wstring wstrRequireLevel = L"";		/// June @328e616a: 31aa1670( L"" )

			/// June @328e6178: 3221c7b0( templet, level + 1 ) = GetSkillRequireLevelValue( level + 1 ) ( vector at templet +0x150 )
			const int iRequireLevel = pSkillTemplet->GetSkillRequireLevelValue( iSkillLevel + 1 );

			if ( iUnitLevel >= iRequireLevel )
			{
				wstrRequireLevel = GET_REPLACED_STRING( ( STR_ID_25173, "i", iRequireLevel ) );		/// June @328e6191, 0x6255 = 25173
			}
			else	/// June @328e61dc: unit level below the requirement: red, then back to the grey of the preview
			{
				wstrRequireLevel = L"#CFF0000";		/// 334599bc
				wstrRequireLevel += GET_REPLACED_STRING( ( STR_ID_25173, "i", iRequireLevel ) );
				wstrRequireLevel += L"#C878787";	/// 33486ce0
			}

			/// June @328e6241..: no STR_ID_25375 max-level branch and no SP lines ( March's 25398 ): the preview is only this
			if ( 0 < iSkillLevel )
				wstrDesc += L"\n";

			wstrDesc += L"\n#C878787";		/// June 334c3930

			wstrDesc += GET_REPLACED_STRING( ( STR_ID_25072, "L", wstrRequireLevel ) );		/// June @328e6261, 0x61f0 = 25072

			wstrDesc += L"\n";

			/// June @328e62ca..328e6347: a copy of the next text; after every "#CX" ( 3344bee0 ) the grey is put back: insert( pos + 3, L" #C878787" )
			/// ( 334c391c, 9 chars ) and the search resumes at pos + 0xc
			wstring wstrPreview = wstrNextEffectiveDesc;
			wstring::size_type iPos = wstrPreview.find( L"#CX", 0, 3 );
			while ( wstring::npos != iPos )
			{
				wstrPreview.insert( iPos + 3, L" #C878787" );
				iPos = wstrPreview.find( L"#CX", iPos + 0xc );
			}

			wstrDesc += wstrPreview;
			wstrDesc += L"#CX";		/// June @328e633b: 3344bee0
		}
	}

	/// June @328e6372: the active memos' text, mode 1 only
	if ( 1 == iDescMode_ &&
		 NULL != g_pData->GetMyUser() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

		/// June @328e63a1: the base skill ( +0x178, else the id )
		int iBaseSkillID = pSkillTemplet->m_iDeleteSkillIDByUpgrade;
		if ( 0 == iBaseSkillID )
			iBaseSkillID = iSkillID;

		std::vector<int> vecMemoID;
		pUnit->GetUnitData().m_UserSkillTree.GetJuneActiveMemoList( iBaseSkillID, vecMemoID );		/// June 32891e10

		for ( std::vector<int>::const_iterator it = vecMemoID.begin(); it != vecMemoID.end(); ++it )
		{
			/// June @328e63fd: a wstring built from L"" that is never used again ( destroyed at the loop's end ); not reproduced

			/// June 324550a0() -> 32453420( out, memo id ): the memo's name text
			wstring wstrMemoName = L"";
			/// S7c: 32453420 CX2SkillNoteManager memo-name text builder ( 711 lines, not ported )

			/// June 324550a0() -> 324513e0( out, base skill id, memo id, unit class = select unit +0x38 ): the memo's effect text
			wstring wstrMemoDesc = L"";
			/// S7c: 324513e0 CX2SkillNoteManager memo-effect text builder ( 622 lines, not ported ); its 4th argument is static_cast<int>( pUnit->GetClass() )
			const int iUnitClass = static_cast<int>( pUnit->GetClass() );
			UNREFERENCED_PARAMETER( iUnitClass );

			/// S7b stand-in: June has no test and always appends; until S7c supplies both builders the strings are empty and the append is skipped
			if ( false == wstrMemoName.empty() || false == wstrMemoDesc.empty() )
			{
				wstrDesc += L"\n\n";		/// June @328e645a: 3347e7ac
				wstrDesc += GET_REPLACED_STRING( ( 36281, "LL", wstrMemoName, wstrMemoDesc ) );	/// June 0x8db9 = 36281, "LL": ( name, effect )
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// GetSkillUnSealedDesc
/////////////////////////////////////////////////////////////////////////

/// June 328dc260 ( 2 stack args, ret 8, ECX unused: ( out, skill id ) ). The "needs item X" line for a sealed skill.
/// Differs from March: June stops at the FIRST seal-map entry ( in map order ) whose skill list holds the skill; March kept the last match.
void CX2UISkillTreeNew::GetSkillUnSealedDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SKILL_ID eSkillID ) const
{
	/// June 328dc29a..328dc2b6: g_pData, skill tree ( +0x50 ), item manager ( +0x34 )
	if ( NULL == g_pData ||
		 NULL == g_pData->GetSkillTree() ||
		 NULL == g_pData->GetItemManager() )
		return;

	/// June 328dc2bc: skill tree +0x74 = the seal map ( item id -> SealSkillItemInfo, skill list at node +0x14 )
	const std::map<int, CX2SkillTree::SealSkillItemInfo>& mapSealSkillItemTemplet = g_pData->GetSkillTree()->GetSealSkillItemTemplet();

	int iUnSealedItemID = 0;
	bool bFound = false;

	std::map<int, CX2SkillTree::SealSkillItemInfo>::const_iterator mit = mapSealSkillItemTemplet.begin();
	for ( ; mit != mapSealSkillItemTemplet.end() && false == bFound; ++mit )
	{
		/// June @328dc2db: 32079c50 copies the skill list
		const std::vector<CX2SkillTree::SKILL_ID> vecValue = mit->second.m_vecSkillID;

		for ( std::vector<CX2SkillTree::SKILL_ID>::const_iterator vit = vecValue.begin(); vit != vecValue.end(); ++vit )
		{
			if ( eSkillID == *vit )
			{
				iUnSealedItemID = mit->first;		/// June @328dc348: ebx = node +0xc; then both loops end
				bFound = true;
				break;
			}
		}
	}

	/// June @328dc36e: no item -> nothing
	if ( 0 != iUnSealedItemID )
	{
		/// June @328dc376..328dc387: 325bb740( item manager, item id ) = GetItemTemplet; NULL -> nothing
		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( iUnSealedItemID );
		if ( NULL != pItemTemplet )
		{
			/// June 32867140 = GetName(); copied into a wstring
			wstring wstrItemName = pItemTemplet->GetName();

			wstrDesc += L"#CFF0000";		/// June @328dc3d4: 334599bc, 8 chars
			wstrDesc += GET_REPLACED_STRING( ( STR_ID_25304, "L", wstrItemName ) );		/// June 0x62d8 = 25304, "L"
			wstrDesc += L"#CX\n\n";			/// June @328dc462: 3349407c, 5 chars
		}
	}
}

/// June 328ee680: the body of the closure ShowSkillDesc builds ( 328edb50 ). Line-breaks the main text, measures the title and the main
/// text with the given font, places the two divider pictures, appends the effective description, line-breaks again and stores the
/// tooltip height. iFontIndex_ is 1 first and 0 when the result is 0x300 or taller.
static int JuneLayoutSkillDescMain( CKTDGUIDialogType pDLGToolTip_, const int iFontIndex_, const int iMainDescYPos_, const int iDescWidth_,
	wstring& wstrMain_, CKTDGUIControl::UIStringData* pTitle_, CKTDGUIControl::UIStringData* pMain_,
	const CX2UISkillTreeNew* pThis_, const int iSkillLevel_, const CX2SkillTree::SkillTemplet* pSkillTemplet_, const int iDescMode_ )
{
	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( iFontIndex_ );
	const float fScaleY = g_pKTDXApp->GetResolutionScaleY();

	/// @328ee6f6: width 0x17c
	wstrMain_ = g_pMain->GetStrByLineBreakColor( wstrMain_.c_str(), 0x17c, iFontIndex_ );

	/// @328ee736: the title's y and height ( one pixel per "\n" )
	const int iTitleY = static_cast<int>( pTitle_->pos.y );
	int iCount = 0;
	for ( wstring::size_type iPos = pTitle_->msg.find( L"\n" ); wstring::npos != iPos; iPos = pTitle_->msg.find( L"\n", iPos + 1 ) )
		++iCount;
	const int iTitleHeight = static_cast<int>( static_cast<float>( pFont->GetHeight( pTitle_->msg.c_str() ) + iCount ) / fScaleY );

	/// @328ee7b4: the main text's height before the effective part ( two pixels per "\n" )
	iCount = 0;
	for ( wstring::size_type iPos = wstrMain_.find( L"\n" ); wstring::npos != iPos; iPos = wstrMain_.find( L"\n", iPos + 1 ) )
		iCount += 2;
	const int iMainHeight = static_cast<int>( static_cast<float>( pFont->GetHeight( wstrMain_.c_str() ) + iCount ) / fScaleY );

	/// @328ee818: the two dividers, ( width - 20 ) wide at x 10: under the title, and under the main text
	CKTDGUIStatic* pDevideLine = static_cast<CKTDGUIStatic*>( pDLGToolTip_->GetControl( L"g_pStatic_HorizonLine" ) );
	if ( NULL != pDevideLine &&
		 NULL != pDevideLine->GetPicture( 0 ) && NULL != pDevideLine->GetPicture( 0 )->pPoint &&
		 NULL != pDevideLine->GetPicture( 1 ) && NULL != pDevideLine->GetPicture( 1 )->pPoint )
	{
		pDevideLine->GetPicture( 0 )->SetSizeX( static_cast<float>( iDescWidth_ ) - 20.f );
		pDevideLine->GetPicture( 1 )->SetSizeX( static_cast<float>( iDescWidth_ ) - 20.f );
		pDevideLine->GetPicture( 0 )->SetPos( D3DXVECTOR2( 10.f, static_cast<float>( iTitleHeight + iTitleY ) ) );
		pDevideLine->GetPicture( 1 )->SetPos( D3DXVECTOR2( 10.f, static_cast<float>( iMainDescYPos_ + iMainHeight ) ) );
	}

	/// @328ee953: the effective description, then width 0x186
	pThis_->GetSkillEffectiveDesc( wstrMain_, iSkillLevel_, pSkillTemplet_, iDescMode_ );
	wstrMain_ = g_pMain->GetStrByLineBreakColor( wstrMain_.c_str(), 0x186, iFontIndex_ );
	pMain_->msg = wstrMain_;

	/// @328ee9db: ( font height + 65 ) and two pixels per "\n", scaled, + 0x3c
	const int iHeight = static_cast<int>( static_cast<float>( pFont->GetHeight( pMain_->msg.c_str() ) ) + 65.f );
	iCount = 0;
	for ( wstring::size_type iPos = wstrMain_.find( L"\n" ); wstring::npos != iPos; iPos = wstrMain_.find( L"\n", iPos + 1 ) )
		iCount += 2;
	return static_cast<int>( static_cast<float>( iHeight + iCount ) / fScaleY ) + 0x3c;
}

/// June 328e9160 ( bShow, skill, pos, size, bEquipped, mode ): mode 1 = the tree / equipped tooltip, 2 = the memo popup ( S7c )
void CX2UISkillTreeNew::ShowSkillDesc( bool bShow, CX2SkillTree::SKILL_ID eSkillID /*= SI_NONE*/, D3DXVECTOR2 pos /*= ( 0, 0 )*/,
	D3DXVECTOR2 size /*= ( 0, 0 )*/, bool bEquipped /*= false*/, int iDescMode_ /*= 1*/ )
{
	/// @328e91ca: DLG_UI_Skill_Desc_Video.lua ( 0.07f, layer 0x190 ), SetDisableUnderWindow( false ) ( dialog +0x190 = 0 )
	if ( NULL == m_pDLGSkillToolTip )
	{
		m_pDLGSkillToolTip = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Skill_Desc_Video.lua", 0.07f, XDL_POP_UP );
		if ( NULL == m_pDLGSkillToolTip )
			return;
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSkillToolTip );
		m_pDLGSkillToolTip->SetDisableUnderWindow( false );
	}

	/// @328e9293: mode 1 reads the transcendence memo's alternate skill ( 323511f0 ) once the skill has a templet
	const CX2SkillTree::SkillTemplet* pAltTemplet = NULL;
	if ( 1 == iDescMode_ && NULL != g_pData->GetSkillTree() && NULL != g_pData->GetSkillTree()->GetSkillTemplet( eSkillID ) )
		pAltTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( JuneGetMyUnitTranscendenceSkillIDForUI( static_cast<int>( eSkillID ) ) ) );

	if ( false == bShow )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}

	CKTDGUIStatic* pItemDesc = static_cast<CKTDGUIStatic*>( m_pDLGSkillToolTip->GetControl( L"SkillDesc" ) );
	if ( NULL == g_pData || NULL == g_pData->GetSkillTree() || NULL == g_pData->GetTitleManager() ||
		 NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet() ||
		 NULL == g_pKTDXApp || NULL == g_pKTDXApp->GetDGManager() || NULL == g_pKTDXApp->GetDGManager()->GetDialogManager() ||
		 NULL == pItemDesc )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}

	CKTDGUIControl::UIStringData* pTitleStringData	= pItemDesc->GetString( 0 );
	CKTDGUIControl::UIStringData* pMainStringData	= pItemDesc->GetString( 1 );
	CKTDGUIControl::UIStringData* pSubStringData	= pItemDesc->GetString( 2 );
	CKTDGUIControl::CPictureData* pMainPictureData	= pItemDesc->GetPicture( 0 );
	CKTDGUIControl::CPictureData* pIconPictureData	= pItemDesc->GetPicture( 1 );
	if ( NULL == pTitleStringData || NULL == pMainStringData || NULL == pSubStringData || NULL == pMainPictureData || NULL == pIconPictureData )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2UserSkillTree& refUserSkillTree = pUnit->GetUnitData().m_UserSkillTree;

	/// @328e9403: mode 1 shows the learned level ( 32890620( id, 1, 0, 1 ) ); the memo popup always level 1
	const int iSkillLevel = ( 1 == iDescMode_ ) ? refUserSkillTree.GetSkillLevel( eSkillID, true, 0, true ) : 1;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
	if ( NULL == pSkillTemplet )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}

	/// @328e944f: outside the equip bar the skill needs a row in the class tree ( mode 2 falls back to the memo's base skill through
	/// 324550a0 / 32451ff0: S7c )
	if ( false == bEquipped )
	{
		const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet =
			g_pData->GetSkillTree()->GetSkillTreeTemplet( static_cast<int>( pUnit->GetClass() ), pSkillTemplet->m_eID );
		if ( NULL == pSkillTreeTemplet )
		{
			m_pDLGSkillToolTip->SetShow( false );
			return;
		}
	}

	/// @328e94be: the icon ( the alternate skill's when there is one ), 54 x 54
	const CX2SkillTree::SkillTemplet* pIconTemplet = ( NULL != pAltTemplet ) ? pAltTemplet : pSkillTemplet;
	pIconPictureData->SetTex( pIconTemplet->m_wstrIconName.c_str(), pIconTemplet->m_wstrIconPieceName.c_str() );
	pIconPictureData->SetSize( D3DXVECTOR2( 54.f, 54.f ) );

	/// @328e9568: the title; empty -> hidden
	wstring wstrTitle = L"";
	GetSkillTitleDesc( wstrTitle, pSkillTemplet, iSkillLevel, iDescMode_ );
	if ( true == wstrTitle.empty() )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}
	pTitleStringData->msg = wstrTitle;
	pTitleStringData->pos.x = pMainStringData->pos.x + 45.f;

	int iTitleLineCount = 0;
	for ( wstring::size_type iPos = wstrTitle.find( L"\n" ); wstring::npos != iPos; iPos = wstrTitle.find( L"\n", iPos + 1 ) )
		++iTitleLineCount;

	/// @328e9630: the sub text at max( level, 1 ); empty -> hidden
	wstring wstrSub = L"";
	GetSkillSubDesc( wstrSub, pSkillTemplet, ( 0 < iSkillLevel ) ? iSkillLevel : 1, iDescMode_, iTitleLineCount );
	if ( true == wstrSub.empty() )
	{
		m_pDLGSkillToolTip->SetShow( false );
		return;
	}
	wstrSub += L"\n";
	pSubStringData->msg = wstrSub;

	/// @328e967f: the main text starts one line per title line down
	wstring wstrMain = L"";
	for ( int i = 0; i < iTitleLineCount; ++i )
		wstrMain += L"\n";

	/// @328e96b1: a born-sealed skill that is neither unsealed nor acquired names its unseal item
	if ( true == pSkillTemplet->m_bBornSealed && false == refUserSkillTree.IsSkillUnsealed( eSkillID ) &&
		 false == refUserSkillTree.DoIHaveThisSkill( eSkillID ) )
		GetSkillUnSealedDesc( wstrMain, eSkillID );

	/// @328e96e8: the common description ( the alternate skill's in mode 1 )
	wstrMain += ( NULL != pAltTemplet && 1 == iDescMode_ ) ? pAltTemplet->m_wstrMainDesc : pSkillTemplet->m_wstrMainDesc;

	/// @328e9710: Aisha ( unit type 2 ): a skill whose templet +0x170 is 3 gets "\n\n" + STR 30357 with the key of game action 0x27
	/// ( 32205af0, 31f5ee50 ). March's templet has no +0x170 field: character material ( checklist ).

	wstrMain += L"\n";

	/// @328e97ea: main y = title y + 65, width 0x1a4; the closure runs with font 1, and again with font 0 when the tooltip is 0x300 or taller
	const int iMainDescYPos = static_cast<int>( pTitleStringData->pos.y + 65.f );
	const int iDescWidth = 0x1a4;
	const wstring wstrMainCopy = wstrMain;
	int iFontIndex = 1;
	int iHeight = JuneLayoutSkillDescMain( m_pDLGSkillToolTip, iFontIndex, iMainDescYPos, iDescWidth, wstrMain, pTitleStringData, pMainStringData,
		this, iSkillLevel, pSkillTemplet, iDescMode_ );
	if ( 0x300 <= iHeight )
	{
		wstrMain = wstrMainCopy;
		iFontIndex = 0;
		iHeight = JuneLayoutSkillDescMain( m_pDLGSkillToolTip, iFontIndex, iMainDescYPos, iDescWidth, wstrMain, pTitleStringData, pMainStringData,
			this, iSkillLevel, pSkillTemplet, iDescMode_ );
	}
	pTitleStringData->fontIndex	= iFontIndex;
	pMainStringData->fontIndex	= iFontIndex;
	pSubStringData->fontIndex	= iFontIndex;

	/// @328e98d5: x centred on the slot ( half of 420 ), 0 at the left edge, 0x25c when it would pass 1024
	int iPosX = 0;
	const float fLeft = size.x * 0.5f + pos.x - 210.f;
	if ( 0.f < fLeft )
	{
		if ( 1024.f < fLeft + 420.f )
			iPosX = 0x25c;
		else
			iPosX = static_cast<int>( fLeft );
	}

	/// @328e992e: y above the slot; mode 2 keeps clear of the hovered memo button ( +0x14c ). When it does not fit above: below the slot,
	/// pulled up by what passes 0x300. Never above 0.
	int iPosY = 0;
	const int iSlotY = static_cast<int>( pos.y );
	bool bBelow = false;
	if ( 2 == iDescMode_ && NULL != m_pHoveredMemoButton )
	{
		const float fButtonY = m_pHoveredMemoButton->GetPos().y;
		if ( static_cast<float>( iSlotY - iHeight ) - fButtonY < 0.f )
			bBelow = true;
		else
			iPosY = iSlotY - static_cast<int>( fButtonY ) - iHeight;
	}
	else if ( 1 == iDescMode_ )
	{
		iPosY = iSlotY - iHeight;
		if ( iPosY < 0 )
			bBelow = true;
	}
	if ( true == bBelow )
	{
		iPosY = static_cast<int>( pos.y + size.y );
		const int iOver = 0x300 - iPosY - iHeight;
		if ( iOver < 0 )
			iPosY += iOver;
	}
	if ( iPosY < 0 )
		iPosY = 0;

	m_pDLGSkillToolTip->SetPos( D3DXVECTOR2( static_cast<float>( iPosX ), static_cast<float>( iPosY ) ) );

	/// @328e9a2e: the background 420 x height at alpha 0.7; title x = main x + 60, sub x = main x + 370 - 15
	pMainPictureData->SetSize( D3DXVECTOR2( 420.f, static_cast<float>( iHeight ) ) );
	pMainPictureData->pPoint->color = D3DXCOLOR( 1.f, 1.f, 1.f, 0.7f );
	pMainStringData->pos.y	= static_cast<float>( iMainDescYPos );
	pTitleStringData->pos.x	= pMainStringData->pos.x + 60.f;
	pSubStringData->pos.x	= pMainStringData->pos.x + 370.f - 15.f;

	m_pDLGSkillToolTip->SetShow( true );
}

/// June 328eab00: switch on wParam - 29000 ( byte table 328ed834, jump table 328ed788; june2017_re s7b stucm_cases.tsv ). A case that
/// breaks returns true; one that jumps to 328ec399 ( the default ) returns false. Cases not listed below are June's default.
bool CX2UISkillTreeNew::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case STUCM_EXIT:						/// 328ebf36
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>( lParam );
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			SetShow( false );
		} return true;

	case STUCM_BUTTON_SKILL_LEVEL_PLUS_MOUSEOVER:	/// 328eb2fe, STR 0xa67
	case STUCM_BUTTON_SKILL_LEVEL_MINUS_MOUSEOVER:	/// 328eb510, STR 0xa68
	case STUCM_BUTTON_SKILL_MASTER_MOUSEOVER:		/// 328eb736, STR 0xa69
		{
			CKTDGUIButton* pControl = reinterpret_cast<CKTDGUIButton*>( lParam );
			D3DXVECTOR2 pos = pControl->GetPos() + pControl->GetDialog()->GetPos();
			pos.x += pControl->GetWidth() / 2;
			pos.y += pControl->GetHeight() / 2.f;
			const int iStringID = ( STUCM_BUTTON_SKILL_LEVEL_PLUS_MOUSEOVER == wParam ) ? STR_ID_2663 :
								  ( ( STUCM_BUTTON_SKILL_LEVEL_MINUS_MOUSEOVER == wParam ) ? STR_ID_2664 : STR_ID_2665 );
			ShowUIDesc( true, GET_STRING( iStringID ), pos );
		} return true;

	case STUCM_BUTTON_SKILL_LEVEL_PLUS_MOUSEOUT:	/// 328eb026 ( shared with the next four )
	case STUCM_BUTTON_SKILL_LEVEL_MINUS_MOUSEOUT:
	case STUCM_BUTTON_SKILL_MASTER_MOUSEOUT:
	case STUCM_SKILL_RESET_MODE_CHANGE_MOUSEOUT:
	case STUCM_CSP_GUIDE_MOUSEOUT:
		ShowUIDesc( false );
		return true;

	case STUCM_EQUIPSLOT_DROPPED:			/// 328ebf6f
		{
			if ( NULL != g_pX2Game )
			{
				if ( CX2Game::GT_PVP == g_pX2Game->GetGameType() || true == g_pX2Game->CheckAndWarningBusyStateNow() )
					return true;
			}
			if ( NULL != g_pMain->GetNowState() )
			{
				CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
				if ( true == pX2State->CheckIsReady() )
				{
					pX2State->ShowOkAndCancelMsgForUnReadyBySkill();
					return true;
				}
				UpdateSkillEquipedSlotUI();
			}
			ProcessEquipSlotDropped( lParam );
		} return true;

	case STUCM_EQUIPSLOT_RMOUSEUP:			/// 328ebfe7
		{
			if ( NULL != g_pMain->GetNowState() )
			{
				CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
				if ( true == pX2State->CheckIsReady() )
				{
					pX2State->ShowOkAndCancelMsgForUnReadyBySkill();
					return true;
				}
			}
			if ( NULL != g_pX2Game && true == g_pX2Game->CheckAndWarningBusyStateNow() )
				return true;
			if ( true == m_bShow )
			{
				ProcessEquipSlotRMouseUp( lParam );
				return true;
			}
		} return true;

	case STUCM_EQUIPSLOT_MOUSEIN:			/// 328ebaa7
		{
			CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
			CX2EquippedSkillSlotData* pSlotData = static_cast<CX2EquippedSkillSlotData*>( pSlot->GetSlotData() );
			if ( NULL == pSlotData || CX2SkillTree::SI_NONE == pSlotData->m_eSkillID )
			{
				D3DXVECTOR2 pos = pSlot->GetPos() + pSlot->GetDialog()->GetPos();
				pos.x = pos.x + pSlot->GetSize().x * 0.5f;
				ShowUIDesc( true, GET_STRING( STR_ID_2666 ), pos, CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT );
				ShowSkillDesc( false );
				return true;
			}
			const D3DXVECTOR2 pos = pSlot->GetPos() + pSlot->GetOffsetPos() + pSlot->GetDialog()->GetPos();
			ShowSkillDesc( true, pSlotData->m_eSkillID, pos, pSlot->GetSize(), true, 1 );
		} return true;

	case STUCM_EQUIPSLOT_MOUSEOUT:			/// 328ebdc7
	case STUCM_SKILLTREESLOT_MOUSEOUT:
		ShowUIDesc( false );
		if ( NULL == m_pHoveredMemoButton )
			ShowSkillDesc( false );
		return true;

	case STUCM_SKILLTREESLOT_RMOUSEUP:		/// 328ebef4
		{
			if ( NULL != g_pMain->GetNowState() )
			{
				CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
				if ( true == pX2State->CheckIsReady() )
				{
					pX2State->ShowOkAndCancelMsgForUnReadyBySkill();
					return true;
				}
			}
			ProcessSkillTreeSlotRMouseUp( lParam );
		} return true;

	case STUCM_SKILLTREESLOT_MOUSEIN:		/// 328eb951
		{
			CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
			CX2SkillTreeSlotData* pSlotData = static_cast<CX2SkillTreeSlotData*>( pSlot->GetSlotData() );
			if ( NULL == pSlotData || CX2SkillTree::SI_NONE == pSlotData->m_eSkillID )
			{
				ShowSkillDesc( false );		/// falls into 29079's body ( 328eba62 )
				return true;
			}

			const D3DXVECTOR2 pos = pSlot->GetPos() + pSlot->GetOffsetPos() + pSlot->GetDialog()->GetPos();
			ShowSkillDesc( true, pSlotData->m_eSkillID, pos, pSlot->GetSize(), false, 1 );

			/// the new-skill glow goes once the mouse has been over it
			if ( true == pSlotData->m_bNewSkill )
			{
				CKTDGUIControl::CPictureData* pPicture = pSlot->GetPicture( CX2SkillTreeSlotData::STSAPT_NEW );
				if ( NULL != pPicture )
					pPicture->SetShow( false );
				pSlotData->m_bNewSkill = false;
				pSlotData->m_bDataUpdated = true;
				DecreaseJuneNewSkillCount();
			}
		} return true;

	case STUCM_SKILL_RESET_MODE_CHANGE:		/// 328ec04d
		m_bSkillRemoveMode = ( false == m_bSkillRemoveMode );
		UpdateSkillTree();
		return true;

	case STUCM_SKILL_RESET_MODE_CHANGE_MOUSEOVER:	/// 328eb048, STR 0xa6b
		{
			CKTDGUIButton* pControl = reinterpret_cast<CKTDGUIButton*>( lParam );
			D3DXVECTOR2 pos = pControl->GetPos() + pControl->GetDialog()->GetPos();
			pos.x += pControl->GetWidth() / 2.f;
			ShowUIDesc( true, GET_STRING( STR_ID_2667 ), pos, CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT );
		} return true;

	case STUCM_CONTROLLIST_SCROLL:			/// 328eaf5e: nothing ( March's UpdateAtMoveScrollBar / UpdateBlind are gone )
		return true;

	case STUCM_CSP_GUIDE_MOUSEIN:			/// 328eabd7, STR 0xa81 ( the button is not in June's tree dialog )
		{
			CKTDGUIButton* pControl = reinterpret_cast<CKTDGUIButton*>( lParam );
			D3DXVECTOR2 pos = pControl->GetPos() + pControl->GetDialog()->GetPos();
			pos.x += pControl->GetWidth() / 2.f;
			pos.y += pControl->GetHeight();
			if ( NULL != g_pData->GetSkillTree() && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
				const int iSPoint			= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSPoint;
				const int iCSPoint			= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint;
				const int iInitSkillPoint	= g_pData->GetSkillTree()->GetCalcInitSkillPoint( g_pData->GetSelectUnitLevel() );
				int iUsedSPoint = 0;
				int iUsedCSPoint = 0;
				refUserSkillTree.CalcUsedSPointAndCSPoint( iUsedSPoint, iUsedCSPoint );
				const int iMaxCSPoint = iSPoint + iCSPoint + iUsedSPoint + iUsedCSPoint - iInitSkillPoint;

				WCHAR wszText[32] = L"";
				StringCchPrintfW( wszText, ARRAY_SIZE( wszText ), L"(%dSP)\n", iMaxCSPoint );
				wstring wstr = GET_STRING( STR_ID_2689 );
				wstr += wszText;
				wstr += GetExpirationDateDesc( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_wstrCSPointEndDate, g_pData->GetServerCurrentTime() );
				ShowUIDesc( true, wstr, pos, CX2TalkBoxManagerImp::TBT_FROM_UP_LEFT, D3DXCOLOR( 1, 1, 1, 1 ), D3DXCOLOR( 0.97f, 0.23f, 0.06f, 1 ) );
			}
		} return true;

	case STUCM_UNSEAL_SKILL_CANCLE:			/// 328eab6f: closes +0xc4, then June's default ( false )
		if ( NULL != m_pDLGSkillUnsealInform )
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGSkillUnsealInform, NULL, false );
			m_pDLGSkillUnsealInform = NULL;
		}
		return false;

	case STUCM_BUTTON_EXIT:					/// 328ebf29: June has no learn state to confirm
		SetShow( false );
		return true;

	case STUCM_SELECT_SKILL_TAB:			/// 328ec83d ( returns false )
		m_iSelectedTab = 0;
		UpdateJuneTabUI();
		return false;

	case STUCM_SELECT_COMMAND_TAB:			/// 328ec84a ( returns false ); the command list is S7e's
		m_iSelectedTab = 1;
		UpdateJuneTabUI();
		return false;

	case STUCM_TRANSFORM_SKILL_SLOT_MOUSEOUT:	/// 328eba62
		ShowSkillDesc( false );
		return true;

	/// Page cases ( S7d ): 29049 328ec068, 29052 328ec1ef, 29053 328ec3ff, 29055 328ec48e, 29056 328ec6b3. Until S7d ports June's, the
	/// page radios still run March's page code ( UpdateSkillPageUI / BuySkillPageDialog below ).
	case STUCM_SKILL_PAGE1:
	case STUCM_SKILL_PAGE2:
	case STUCM_SKILL_PAGE3:
	case STUCM_ADD_SKILL_PAGE_ED:
	case STUCM_ADD_SKILL_PAGE_CASH:
	case STUCM_BUY_CONFIRM:
	case STUCM_BUY_CANCEL:
		return JuneSkillPageCustomEventProc( hWnd, uMsg, wParam, lParam );

	/// S7c ( memo ): 29065 328ed02c, 29066 328ed259, 29067 328ed483, 29068 328ed519, 29076 328ed5ca, 29077 328ed64b
	/// S7e ( panels ): 29061 328ec85b, 29062 328ec9b6, 29069 328ecb05, 29070 328ecc59, 29071 328ecd9f, 29072 328ece68, 29073 328ecf35, 29074 328ecfbf
	/// checklist ( class material ): 29059 328ec719 ( Aisha memorize slot ), 29078 328ed6a8 ( Luciel transform slot )
	default:
		break;
	}
	return false;
}



/// S7d replaces this: March's page cases ( X2UISkillTreeNew.cpp SKILL_PAGE_SYSTEM block ) carried as they are, minus the learn-mode
/// decide paths June's switch does not have ( 29054, 29057, 29058 ). June's 29049 ( 328ec068 ) is the one page radio case.
bool CX2UISkillTreeNew::JuneSkillPageCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case STUCM_SKILL_PAGE1:
	case STUCM_SKILL_PAGE2:
	case STUCM_SKILL_PAGE3:
		{
			if ( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
				return false;
			const USHORT usPage = static_cast<USHORT>( wParam - STUCM_SKILL_PAGE1 + 1 );
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
			pUnitData->m_UserSkillTree.SetSelectPage( usPage );
			pUnitData->m_iSPoint = pUnitData->m_UserSkillTree.GetSkillPoint( usPage );
			pUnitData->m_iCSPoint = pUnitData->m_UserSkillTree.GetCashSkillPoint( usPage );
			UpdateUI( false, true, true, true );
		} return true;

	case STUCM_ADD_SKILL_PAGE_ED:
		{
			if ( CX2Main::XS_VILLAGE_MAP != g_pMain->GetNowStateID() )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29410 ), g_pMain->GetNowState() );
				return false;
			}
			if ( true == g_pMain->IsWaitingServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK ) )
				return false;
			g_pData->GetServerProtocol()->SendID( EGS_GET_NEXT_SKILL_PAGE_ED_REQ );
			g_pMain->AddServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK );
		} return true;

	case STUCM_ADD_SKILL_PAGE_CASH:
		{
			if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				CX2StateField* pStateField = static_cast<CX2StateField*>( g_pMain->GetNowState() );
				if ( NULL != pStateField )
				{
					if ( NULL != g_pTFieldGame )
						g_pTFieldGame->CloseFieldName( 0.0f );
					g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_PRIVATE_BANK );
					pStateField->ToggleCashShop();
				}
				return true;
			}
		} return false;

	case STUCM_BUY_CONFIRM:
		{
			if ( NULL != m_pDLGBuyConfirm )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGBuyConfirm, NULL, false );
			m_pDLGBuyConfirm = NULL;
			if ( g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED >= m_iConfirmED )
				return g_pData->GetSkillTree()->Handler_EGS_EXPAND_SKILL_PAGE_REQ();
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28961 ), g_pMain->GetNowState() );
		} return false;

	case STUCM_BUY_CANCEL:
		if ( NULL != m_pDLGBuyConfirm )
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGBuyConfirm, NULL, false );
		m_pDLGBuyConfirm = NULL;
		return true;

	default:
		break;
	}
	return false;
}

#endif UPGRADE_SKILL_SYSTEM_2013
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_UI
