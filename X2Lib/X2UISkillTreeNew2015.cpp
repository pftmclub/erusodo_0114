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
#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_UI
#include "X2GuEve.h"
#endif SERV_IRUHADEV_JUNE_CHAR_EVE_UI

namespace
{
	/// June 32207560: the skill-type background key in DLG_UI_Common_Texture_NewSkill_02.TGA
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
		case 13:	/// ST_HYPER_ACTIVE_SKILL
		case 14:	/// ST_SPECIAL_ACTIVE_GRADE_1
		case 15:	/// ST_SPECIAL_ACTIVE_GRADE_2
		case 16:	/// ST_SPECIAL_ACTIVE_GRADE_3
			return L"SKILL_BG_ACTIVE_SPECIAL";
		default:
			return L"SKILL_BG_PASSIVE";
		}
	}

#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// June 31c94970 ( CPictureData; March has no such method ): a picture with a texture and no point gets one, sized to the texture
	/// ( UIPointData::SetAutoPointByTextureSize, 31c930f0 ), and pos / size / original pos / original size follow it. With a point it does
	/// nothing. June keeps March's SetSizeAsTextureSize as 31c94870; S7b and S7e first read 31c94970 as that, which leaves a new picture
	/// with no point ( SetPos logs KEM_ERROR347 and nothing draws: play-test 2026-10-19, 385 lines )
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
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS

	/// June 31c94ac0: resize a picture around its centre ( CPictureData has no such method in March )
	void SetPictureSizeAroundCenter( CKTDGUIControl::CPictureData* pPicture_, const float fWidth_, const float fHeight_ )
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

	/// June 328914b0( id, bUpgradeSwap ): the acquired map ( user tree +0x4a8, the page in use; end sentinel +0x4ac ) holds the id, after
	/// the upgrade swap 328905b0 when the flag is set. GetUserSkillTemplet ( June 32890720 ) is the same find on the same map. S7b first
	/// read 328914b0 as IsSkillUnsealed ( the unsealed set is 32890a00, user tree +0x38 ): every slot refresh then took each skill off
	/// again ( play-test 2026-10-18 ).
	bool IsSkillAcquired( const CX2UserSkillTree& refUserSkillTree_, const CX2SkillTree::SKILL_ID eSkillID_, const bool bUpgradeSwap_ )
	{
		return NULL != refUserSkillTree_.GetUserSkillTemplet( eSkillID_, bUpgradeSwap_ );
	}

	/// June 32214fc0, one row: the value text for type letter cParamType_ ( jump tables 32215e04 / 32215e4c ). fRaw_ is the row's value
	/// ( +0 ), fNumber_ the number the e / f / h / i letters print ( value * pvp rate in the PvP pass, the value in the other ).
	/// I and L / S / l / s read the raw value in both passes ( June ); any other letter prints nothing.
	void FormatSkillDescValue( wstringstream& wstrstm_, const char cParamType_, const float fRaw_, const float fNumber_ )
	{
		WCHAR wszBuf[64] = L"";
		switch ( cParamType_ )
		{
		case 'I':	/// @32215324: a string id; March's GET_STRING carries June's "[S%1%]%2%" debug branch ( 31c31060 +0x40 )
			wstrstm_ << GET_STRING( static_cast<int>( fRaw_ ) );
			break;
		case 'L':
		case 'S':
		case 'l':
		case 's':	/// @3221545e: the float as the stream prints it
			wstrstm_ << fRaw_;
			break;
		case 'e':	/// @3221529b: a sign above zero, then "%.2f" read back with _wtof
			if ( 0.f < fNumber_ )
				wstrstm_ << L"+";
			swprintf_s( wszBuf, 64, L"%.2f", static_cast<double>( fNumber_ ) );
			wstrstm_ << _wtof( wszBuf );
			break;
		case 'f':	/// @322152fc
			swprintf_s( wszBuf, 64, L"%.2f", static_cast<double>( fNumber_ ) );
			wstrstm_ << _wtof( wszBuf );
			break;
		case 'h':	/// @32215263: a sign above zero, then the integer ( falls into 'i' )
			if ( 0.f < fNumber_ )
				wstrstm_ << L"+";
		case 'i':	/// @32215282: ftol ( 32fe25f0 ) truncates
			wstrstm_ << static_cast<int>( fNumber_ );
			break;
		default:
			break;
		}
	}

	/// June 32214fc0 ( cdecl: out, text, const char* param types, const vector<row>* rows ): count = min( strlen( types ), rows ). Pass 1
	/// replaces every "@<101 + i>" with row i's PvP number ( value * pvp rate ), pass 2 every "@<1 + i>" with its value; each match is
	/// replaced in place and the search goes on after the inserted text. "@1" also matches the start of "@10".."@19" ( June ).
	wstring ReplaceSkillDescValues( const wstring& wstrText_, const std::string& strParamTypes_,
		const vector<CX2SkillTree::SkillTemplet::NumericalRow>& vecRow_ )
	{
		wstring wstrOut = wstrText_;
		const int iCount = min( static_cast<int>( strParamTypes_.size() ), static_cast<int>( vecRow_.size() ) );
		for ( int iPass = 0; iPass < 2; ++iPass )
		{
			for ( int i = 0; i < iCount; ++i )
			{
				wstringstream wstrstmKey;
				wstrstmKey << L"@" << ( ( 0 == iPass ) ? 101 + i : 1 + i );

				const float fRaw = vecRow_[i].m_fValue;
				const float fNumber = ( 0 == iPass ) ? vecRow_[i].m_fPvpRate * vecRow_[i].m_fValue : vecRow_[i].m_fValue;
				wstringstream wstrstmValue;
				FormatSkillDescValue( wstrstmValue, strParamTypes_[i], fRaw, fNumber );

				const wstring wstrKey = wstrstmKey.str();
				const wstring wstrValue = wstrstmValue.str();
				wstring::size_type uiPos = 0;
				while ( wstring::npos != ( uiPos = wstrOut.find( wstrKey, uiPos ) ) )
				{
					wstrOut.replace( uiPos, wstrKey.size(), wstrValue );
					uiPos += wstrValue.size();
				}
			}
		}
		return wstrOut;
	}

	/// June 323511f0 ( a thiscall on MY unit ): the transcendence memo's alternate skill, 0 when none. The same June function
	/// S3d ported as the static JuneGetMyUnitTranscendenceSkillID in X2GUUser.cpp; this file needs it for the slot icons.
	int GetMyUnitTranscendenceSkillIDForUI( const int iSkillID_ )
	{
		if ( NULL == g_pData || NULL == g_pData->GetSkillTree() || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
			return 0;
		CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

		const int iBaseSkillID = static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) ) );
		std::vector<const SSkillMemoTemplet*> vecMemo;
		CX2SkillNoteManager::GetInstance()->GetMemoList( iBaseSkillID, 0, static_cast<int>( pMyUnit->GetClass() ), 0, vecMemo );

		const CX2UserSkillTree& refUserSkillTree = pMyUnit->GetUnitData().m_UserSkillTree;
		for ( std::vector<const SSkillMemoTemplet*>::const_iterator it = vecMemo.begin(); it != vecMemo.end(); ++it )
		{
			const SSkillMemoTemplet* pMemo = *it;
			if ( NULL == pMemo || false == refUserSkillTree.IsMemoActive( iBaseSkillID, pMemo->m_iType ) )
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

/// June 328ee280: the level's text ( templet +0xc0 ), out of range -> L""; then 32215e80( text, level ), which always returns 1 ( @3221611f ).
/// 32215e80: per NUMERICAL_DATA row ( +0x17c ) the level's 16 B row, index min( level - 1, count - 1 ) as unsigned ( level 0 -> the last
/// row, June ), an empty row vector -> zeros; the row's param type ( +0x2c ) appended to the type string; any row -> 32214fc0. Replaces
/// March's GetSkillEffectiveDescExpand special cases; declared in X2SkillTree.h under the flag.
const wstring CX2SkillTree::SkillTemplet::GetSkillEffectiveDescValue( int iLevel ) const
{
	const int iIndex = iLevel - 1;
	if ( 0 > iIndex || static_cast<int>( m_vecEffectiveDesc.size() ) <= iIndex )
		return L"";

	std::string strParamTypes;
	vector<NumericalRow> vecRow;
	for ( vector<NumericalData>::const_iterator it = m_vecNumericalData.begin(); it != m_vecNumericalData.end(); ++it )
	{
		NumericalRow kRow = { 0.f, 0.f, 0.f, 0.f };
		if ( false == it->m_vecRow.empty() )
		{
			unsigned int uiRow = static_cast<unsigned int>( it->m_vecRow.size() ) - 1;
			if ( static_cast<unsigned int>( iLevel - 1 ) < uiRow )
				uiRow = static_cast<unsigned int>( iLevel - 1 );
			kRow = it->m_vecRow[uiRow];
		}
		vecRow.push_back( kRow );

		for ( wstring::const_iterator itChar = it->m_wstrParamType.begin(); itChar != it->m_wstrParamType.end(); ++itChar )
			strParamTypes += static_cast<char>( *itChar );
	}

	if ( true == vecRow.empty() )
		return m_vecEffectiveDesc[iIndex];
	return ReplaceSkillDescValues( m_vecEffectiveDesc[iIndex], strParamTypes, vecRow );
}

/////////////////////////////////////////////////////////////////////////
// June's skill-type colour table ( CX2SkillTree +0xb4, filled by SkillTypeColor.lua through g_pSkillToolTipManager:AddSkillTypeColor,
// 3220baf0; read by GetSkillSubDesc through 322055c0 / 3265c7f0 )
/////////////////////////////////////////////////////////////////////////
class CX2SkillTypeColorTable
{
public:
	CX2SkillTypeColorTable() : m_bLoaded( false ) {}

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
			lua_tinker::class_add<CX2SkillTypeColorTable>( L, "CX2SkillTypeColorTable" );
			lua_tinker::class_def<CX2SkillTypeColorTable>( L, "AddSkillTypeColor", &CX2SkillTypeColorTable::AddSkillTypeColor_LUA );
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

static CX2SkillTypeColorTable s_kSkillTypeColorTable;

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
		const int iAltSkillID = GetMyUnitTranscendenceSkillIDForUI( static_cast<int>( m_eSkillID ) );
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
		/// June @328dd042..328dd066: sealed = not unsealed ( 32890a00 ), born sealed ( +0x4c ) and not acquired ( 328914b0( id, 0 ) )
		bool bIsLockSkill = false;
		if ( false == refUserSkillTree.IsSkillUnsealed( m_eSkillID ) &&
			 true == pSkillTemplet->m_bBornSealed &&
			 false == IsSkillAcquired( refUserSkillTree, m_eSkillID, false ) )
		{
			bIsLockSkill = true;
			pUISkillTree->GetDLGUISkillTree()->ChangeSequence( refSkillSlotUI.m_pStaticSkillLock, true );
		}
		refSkillSlotUI.m_pStaticSkillLock->SetShow( bIsLockSkill );

		if ( NULL == pIconSkillTemplet )
			pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		else
			pSlot->SetItemTex( pIconSkillTemplet->m_wstrIconName.c_str(), pIconSkillTemplet->m_wstrIconPieceName.c_str() );

		const bool bHave = refUserSkillTree.DoIHaveThisSkill( m_eSkillID );
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
		SetPictureSizeAroundCenter( pPicture, 50.f, 50.f );
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

#ifdef SERV_IRUHADEV_JUNE_USER_SKILL_ICON_OVERRIDE
namespace _CONST_EQUIPPEDSKILLSLOT_CH1o2_
{
	/// June 328e0aa0: its tree +0x130..+0x139 class blocks are CH4c's ( 0x1a / 0x6d ) and checklist 7aj's; the tail asks my unit's slot 0x56c
	void ApplyEquippedSkillIconOverride( const int iSkillID_, wstring& wstrIconTexName_, wstring& wstrIconTexKey_ )
	{
		if ( NULL == g_pData || NULL == g_pData->GetUIManager() || NULL == g_pData->GetUIManager()->GetUISkillTree() || NULL == g_pX2Game || NULL == g_pX2Game->GetMyUnit() )
			return;

#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_UI
		/// June @328e0b4d: the 0x7b / 0x72 / 0x73 / 0x7e blocks between Eve's two and the Add block after them are their class ports' ( 7aj )
		if ( true == g_pData->GetUIManager()->GetUISkillTree()->GetClassSkillIconSwap() && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			const CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
			if ( CX2Unit::UC_EVE_CODE_EMPRESS == eUnitClass &&
				 ( CX2SkillTree::RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION == iSkillID_ || CX2SkillTree::RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION_UPGRADE == iSkillID_ ) )
			{
				wstrIconTexName_ = L"DLG_SKILL_Hyper_Active_02.tga";
				wstrIconTexKey_ = L"SI_HA_EEP_LINK_OVERCHARGE_ILLUSION_01";
			}
			else if ( CX2Unit::UC_EVE_ELECTRA == eUnitClass &&
				 ( CX2SkillTree::RNSI_A_EVE_SPECTRO_EL_CRYSTAL == iSkillID_ || CX2SkillTree::RNSI_A_EVE_SPECTRO_EL_CRYSTAL_UPGRADE == iSkillID_ ) )
			{
				switch ( g_pData->GetUIManager()->GetUISkillTree()->GetElectraFormationType() )
				{
				case CX2GUEve::PFT_AMPLIFIER:
					wstrIconTexName_ = L"DLG_SKILL_EveActive_03.tga";
					wstrIconTexKey_ = L"SI_A_EEL_SPECTRO_EL_CRYSTAL_01";
					break;
				case CX2GUEve::PFT_SPECTRUM:
					wstrIconTexName_ = L"DLG_SKILL_EveActive_03.tga";
					wstrIconTexKey_ = L"SI_A_EEL_SPECTRO_EL_CRYSTAL_02";
					break;
				case CX2GUEve::PFT_INDUCTION:
					wstrIconTexName_ = L"DLG_SKILL_EveActive_04.tga";
					wstrIconTexKey_ = L"SI_A_EBS_SPECTRO_EL_CRYSTAL_INDUCTION";
					break;
				case CX2GUEve::PFT_FUSION:
					wstrIconTexName_ = L"DLG_SKILL_EveActive_04.tga";
					wstrIconTexKey_ = L"SI_A_EBS_SPECTRO_EL_CRYSTAL_FUSION";
					break;
				}
			}
		}
#endif SERV_IRUHADEV_JUNE_CHAR_EVE_UI

		g_pX2Game->GetMyUnit()->SkillIconOverride( iSkillID_, wstrIconTexName_, wstrIconTexKey_ );
	}
}
using namespace _CONST_EQUIPPEDSKILLSLOT_CH1o2_;
#endif SERV_IRUHADEV_JUNE_USER_SKILL_ICON_OVERRIDE

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
			bLocked = ( false == CX2UISkillTreeNew::IsMyUnitTranscended() );
		else
			bLocked = ( 0 == refUserSkillTree.GetSlotState( 1 ) );
	}
	else if ( true == m_bSlotb )
	{
		bLocked = ( false == refUserSkillTree.IsSlotUsable( 0 ) );
	}
	SetLockedSlotUI( pSlot, bLocked );

	const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot( m_iIndex, m_bSlotb );
	m_eSkillID = ( NULL != pSkillData ) ? pSkillData->m_eID : CX2SkillTree::SI_NONE;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( m_eSkillID );
	{
		const int iAltSkillID = GetMyUnitTranscendenceSkillIDForUI( static_cast<int>( m_eSkillID ) );
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
#ifdef SERV_IRUHADEV_JUNE_USER_SKILL_ICON_OVERRIDE
		/// June 328e18d0 @328e1c62: the slot's own id ( not the transcendence one ) with copies of the templet's icon strings
		wstring wstrIconTexName = pSkillTemplet->m_wstrIconName;
		wstring wstrIconTexKey = pSkillTemplet->m_wstrIconPieceName;
		ApplyEquippedSkillIconOverride( static_cast<int>( m_eSkillID ), wstrIconTexName, wstrIconTexKey );
		pSlot->SetItemTex( wstrIconTexName.c_str(), wstrIconTexKey.c_str() );
#else SERV_IRUHADEV_JUNE_USER_SKILL_ICON_OVERRIDE
		pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
#endif SERV_IRUHADEV_JUNE_USER_SKILL_ICON_OVERRIDE
		pSlot->SetDragable( true );

		const int iSkillLevel = refUserSkillTree.GetSkillLevel( m_eSkillID, true, 0, true );		/// June 328e1caf: 32890620( id, 1, 0, 1 )
		if ( iSkillLevel < 1 && CX2SkillTree::SI_NONE != m_eSkillID )
		{
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( GetSlotID(), CX2SkillTree::SI_NONE );
			return;
		}

		/// June @328e1d38: 328914b0( id, 1 ): a skill that is not acquired ( after the upgrade swap ) is taken off: 32205dc0( slot, 0 )
		/// queues the change unless the same change is already queued; S7's Handler_EGS_CHANGE_SKILL_SLOT_REQ is June's queue ( 3220e3b0 )
		if ( false == IsSkillAcquired( refUserSkillTree, m_eSkillID, true ) )
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
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
			if ( CX2SkillTree::RNSI_A_EVE_SPECTRO_EL_CRYSTAL == iSkillID || CX2SkillTree::RNSI_A_EVE_SPECTRO_EL_CRYSTAL_UPGRADE == iSkillID ||
				 true == CX2UISkillTreeNew::IsSameOrUpgradeSkill( static_cast<int>( m_eSkillID ), CX2SkillTree::RNSI_SA_ECB_THOUSANDS_OF_STARS ) )
#else
			if ( 0x6272 == iSkillID || 0x627f == iSkillID ||
				 true == CX2UISkillTreeNew::IsSameOrUpgradeSkill( static_cast<int>( m_eSkillID ), 0x61c4 ) )
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
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

#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
/// June 328df840: true hides the memorize sign; the own Aisha asks vslot 0x714, which is June's return-false stub 331a4950
static bool IsMemorizeSignBlocked( const int iSkillID_ )
{
	UNREFERENCED_PARAMETER( iSkillID_ );
	return false;
}
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

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
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
		else if ( true == bShow && SESAPT_MEMORIZE == eType )
		{
			if ( true == IsMemorizeSignBlocked( static_cast<int>( m_eSkillID ) ) )
			{
				pPicture->SetShow( false );
				return;
			}
			if ( false == pPicture->SetTex( L"DLG_UI_Common_Texture63_NEW.tga", L"AISHA_MEMORIZE_SIGN" ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );
		}
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
		pPicture->SetShow( bShow );
		return;
	}

#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	if ( false == bShow )
		return;
#else SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	if ( false == bShow || SESAPT_MEMORIZE == eType )
		return;
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

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

#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	case SESAPT_MEMORIZE:
		{
			if ( true == IsMemorizeSignBlocked( static_cast<int>( m_eSkillID ) ) )
			{
				pPicture->SetShow( false );	// June never adds this picture, so it leaks
				return;
			}
			if ( false == pPicture->SetTex( L"DLG_UI_Common_Texture63_NEW.tga", L"AISHA_MEMORIZE_SIGN" ) )
				pPicture->SetTex( L"NoAlphaImage.dds" );
			pPicture->pPoint->color = D3DXCOLOR( 1, 1, 1, 1 );
			const D3DXVECTOR2 vPos = pPicture->GetPos();
			pPicture->SetPos( D3DXVECTOR2( vPos.x - 1.f, vPos.y - 1.f ) );
			pPicture->SetSizeX( pPicture->GetSize().x + 2.f );
			pPicture->SetSizeY( pPicture->GetSize().y + 2.f );
		} break;
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

	default:
		break;
	}

	pSlot->AddPicture( static_cast<int>( eType ), pPicture );
	pPicture->SetShow( bShow );
}

#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
/////////////////////////////////////////////////////////////////////////
// CX2MemorizeSkillSlotData : slot data type 5 ( vt 334c2eac )
/////////////////////////////////////////////////////////////////////////

CX2MemorizeSkillSlotData::CX2MemorizeSkillSlotData( int index, bool bSlotb )
{
	m_eSlotDataType	= SDT_MEMORIZE;
	m_bDataUpdated	= true;
	m_eSkillID		= CX2SkillTree::SI_NONE;
	m_iIndex		= index;
	m_bSlotb		= bSlotb;
}

/// June 328e0f10: the equipped skill's icon, or the memorize list is swept ( 328df930 )
/*virtual*/ void CX2MemorizeSkillSlotData::UpdateUI( CKTDGUISlot* pSlot )
{
	if ( NULL == pSlot || NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetSkillTree() || CX2Unit::UT_ARME != g_pData->GetMyUser()->GetSelectUnit()->GetType() )
		return;

	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot( m_iIndex, m_bSlotb );
	if ( NULL != pSkillData )
	{
		m_eSkillID = pSkillData->m_eID;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( m_eSkillID );
		if ( NULL != pSkillTemplet && 0 < refUserSkillTree.GetSkillLevel( m_eSkillID, true, 0, true ) && 0 < static_cast<int>( m_eSkillID ) )
		{
			pSlot->SetItemTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
			pSlot->SetDragable( false );
			return;
		}
	}

	/// 328df930: the own unit sweeps its memorize list ( 32ce1770, refresh on )
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() && NULL != g_pX2Game->GetMyUnit()->GetUnit() &&
		 CX2Unit::UT_ARME == g_pX2Game->GetMyUnit()->GetUnit()->GetType() )
	{
		const bool bRefresh = true;
		static_cast<CX2GUArme_VioletMage*>( g_pX2Game->GetMyUnit() )->SweepMemoRise( &bRefresh );
	}
}
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

/////////////////////////////////////////////////////////////////////////
// CX2UISkillTreeNew
/////////////////////////////////////////////////////////////////////////

/// June 328e76a0
CX2UISkillTreeNew::CX2UISkillTreeNew( CKTDXStage* pNowStage )
: m_iSelectedTab( 0 )
, m_iCommandGroup( 0 )
, m_pDLGCommandDesc( NULL )
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
, m_iCommandRow( 0 )
, m_bCommandToolTip( false )
, m_bCommandMovieToolTip( false )
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
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
#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_UI
, m_bClassSkillIconSwap( false )
, m_iElectraFormationType( 0 )
#endif SERV_IRUHADEV_JUNE_CHAR_EVE_UI
, m_pHoveredMemoButton( NULL )
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
, m_pDLGMemoMsgBox( NULL )
, m_iMemoRemoveItemUID( 0 )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
	m_vecCommandGroupControl.clear();
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
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
/// June 328e6550 / 328ed890 / 328ddff0 / 328e18d0: my unit's CX2Unit +0x3c is not 0
bool CX2UISkillTreeNew::IsMyUnitTranscended()
{
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	return 0 != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iUnitClassTran;
}

#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
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
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		/// @328ea7f4: the command-description popup goes first ( 328e6cd0( false, 0, -1, L"Unknown", white ) )
		ShowCommandDescPopup( false, 0.f, -1, L"Unknown", D3DXVECTOR3( 1.f, 1.f, 1.f ) );
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		/// S7e: June hides the command-description video popup here first ( 328e6cd0( false, L"Unknown", white ) )
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
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
		RecountNewSkills();
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
		UpdateTabUI();
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
	m_vecCommandGroupControl.clear();		/// @328ed964: +0x18 cleared with +0x08

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
		/// @328edaca: +0x8c = ( unit +0xe40 == 1 ), Luciel's form flag ( the minor command group ). March's CX2Unit has no such flag and no
		/// Luciel: group 0. The flag is character material ( checklist )
		m_iCommandGroup = 0;
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
		m_pUserSkillTree = &pUnit->GetUnitData().m_UserSkillTree;
		CreateSkillTree( pUnit->GetUnitTemplet()->m_UnitClass, IsMyUnitTranscended() );
		InitEquippedSkillSlotData();
		UpdateTabUI();
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
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	CreateCommandLists( eUnitClass );			/// @328eaa96: 328e7ba0 ( the command lists from ComboTree.lua )
	CreateSkillTreePortrait( eUnitClass, bTranscended_ );
	CreateSkillTreeLevelRows();
	/// @328eaaaf..328eaaeb: the +0x90 map is cleared ( 32223c30 ), then the +0x3c block ( 328ee430 )
	m_mapCommandToolTip.clear();
	m_kSkillCommandDesc.Reset();
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// S7e: 328e7ba0 ( the command lists from ComboTree.lua )
	CreateSkillTreePortrait( eUnitClass, bTranscended_ );
	CreateSkillTreeLevelRows();
	/// S7e: the +0x94 command map and the +0x3c description cache are cleared here
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
}

/// June 328de040
void CX2UISkillTreeNew::CreateSkillTreeSlots( CX2Unit::UNIT_CLASS eUnitClass )
{
	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"SkillTreeControlList" ) );
	if ( NULL == pControlList )
		return;

	/// one skill per ( tier, index in tier ); a second skill at a taken position replaces it only when it is an upgraded skill
	/// ( +0x178 m_iDeleteSkillIDByUpgrade ) that is acquired ( 328914b0( id, 0 ) @328de1b9 ). A hyper-class row ( +0x174 == 5 ) needs transcendence.
	const CX2SkillTree::SkillTreeTempletMap& refSkillTreeTempletMap = g_pData->GetSkillTree()->GetSkillTreeTempletMap( static_cast<int>( eUnitClass ) );
	for ( CX2SkillTree::SkillTreeTempletMap::const_iterator mapit = refSkillTreeTempletMap.begin(); mapit != refSkillTreeTempletMap.end(); ++mapit )
	{
		const CX2SkillTree::SKILL_ID eSkillID = mapit->first;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
		if ( NULL == pSkillTemplet )
			continue;
		if ( false == IsMyUnitTranscended() && CX2SkillTree::SPRT_HYPER_CLASS == pSkillTemplet->m_ePowerRateType )
			continue;

		const SkillPositionKey kKey( mapit->second.m_iTier, mapit->second.m_iIndexInTier );
		map<SkillPositionKey, CX2SkillTree::SKILL_ID>::iterator it = m_mapSkillPosition.find( kKey );
		if ( it == m_mapSkillPosition.end() )
		{
			m_mapSkillPosition.insert( std::make_pair( kKey, eSkillID ) );
		}
		else if ( 0 != pSkillTemplet->m_iDeleteSkillIDByUpgrade && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
				  true == IsSkillAcquired( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree, eSkillID, false ) )
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

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// June @328de384..328de3db sets both memo buttons' offset ( vslot 0x8c, +0x1ec ) to the left one's vslot 0x90 position ( a June
		/// button virtual March lacks: the offset when set, else the normal rect's centre ) plus the list's +0x2b0 ( its x step, March's
		/// m_iScrollOffsetX ) on x. Not reproduced: the list rewrites every item's offset when it lays them out ( SetIndex below,
		/// KTDGUIControlList.cpp:186-190 ), so June's value never survives either.
		/// both memo popups ( 328dda50( 1 ) @328de3e3, 328dda50( 0 ) @328de3ee ), on the local set ( esp+0x2c ) after its copy went into
		/// the map: the map's state words stay 0 until the next UpdateSkillTree, and the select popup refreshes them first ( 328e9b30 )
		UpdateMemoPopup( UISet, true );
		UpdateMemoPopup( UISet, false );
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// S7c: June moves the left memo button right by the list's +0x2b0 ( @328de384 ), gives the right one the same position, and builds
		/// both memo popups ( 328dda50( 1 ), 328dda50( 0 ) )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

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
			UISet.m_pStaticSkillTypeBG->GetPicture( 0 )->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", GetSkillTypeBGKey( static_cast<int>( pSkillTemplet->m_eType ) ) );
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
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		SetPointByTextureSize( pPictureBG );		/// 328dc680 calls 31c94970 ( S7e correction of S7b's reading )
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		pPictureBG->SetSizeAsTextureSize();
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		pPictureBG->SetPos( D3DXVECTOR2( -25.f, -11.f ) );
		pStatic->AddPicture( pPictureBG );

		pPictureLine->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"LINE" );
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		SetPointByTextureSize( pPictureLine );		/// 31c94970
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
		pPictureLine->SetSizeAsTextureSize();
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
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
	/// @328e8f64: no SkillTree_Character skips only the portrait; the panels at 328e9100.. run either way
	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"SkillTree_Character" ) );
	if ( NULL != pStatic )
	{
		wstring wstrFileName = L"";
		wstring wstrPieceName = L"";
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
		// June 328e8e90: the fifth argument is my unit's flag
		if ( true == CX2Data::GetCharacterImageName( wstrFileName, wstrPieceName, eUnitClass, CX2Data::CIT_160by160, bTranscended_ ) && NULL != pStatic->GetPicture( 1 ) )
#else SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
		if ( true == CX2Data::GetCharacterImageName( wstrFileName, wstrPieceName, eUnitClass, CX2Data::CIT_160by160 ) && NULL != pStatic->GetPicture( 1 ) )
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
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

	/// @328e9109..328e912e: 328ee430 ( clears the +0x3c description cache ) and 3220bdb0 ( SkillCommandDescriptionData.lua ) are S7e's;
	/// 328e42d0 and 328e8820 build the major-skill / major-command panels. Their text and rows are S7e's; the group registrations below
	/// are what keeps each tab's statics off the other tab ( play-test 2026-10-18: the command background showed on the skill tab )
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// @328e9104..328e912e ( June passes its 2nd argument, my unit +0x3c, to 3220bdb0 and 328e8820 as their "minor" flag )
	m_kSkillCommandDesc.Reset();
	if ( NULL != g_pData->GetSkillTree() )
		g_pData->GetSkillTree()->LoadSkillCommandDescription( static_cast<int>( eUnitClass ), bTranscended_, &m_kSkillCommandDesc );
	CreateMajorSkillPanel( eUnitClass );
	CreateMajorCommandPanel( eUnitClass, bTranscended_ );
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	RegisterMajorSkillPanelControls();
	RegisterMajorCommandPanelControls();
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
}

/// June 328e42d0, its group half: Skill_Description ( @328e452f, after S7e's text ), SkillTree_MajorSkill ( @328e455d ) and
/// SkillTree_Back_Black ( @328e4585 ) join the skill-tab group +0x08, each only when found. The MajorSkillBack%d / MajorSkillButton%d
/// rows ( @328e4acd / @328e4ae6 ) and the description text come with S7e ( SkillCommandDescriptionData.lua ).
void CX2UISkillTreeNew::RegisterMajorSkillPanelControls()
{
	if ( NULL == m_pDLGUISkillTree )
		return;

	/// S7e: Skill_Description's text ( the class's DESCRIPTION_STRING_ID, fitted by 328df060 ) is set before it is added
	const WCHAR* awszControl[] = { L"Skill_Description", L"SkillTree_MajorSkill", L"SkillTree_Back_Black" };
	for ( int i = 0; i < ARRAY_SIZE( awszControl ); ++i )
	{
		CKTDGUIControl* pControl = m_pDLGUISkillTree->GetControl( awszControl[i] );
		if ( NULL != pControl )
			m_vecSkillTabControl.push_back( pControl );
	}
}

/// June 328e8820, its group half: Command_Back_Black ( @328e88cf ) and SkillTree_MajorCommand ( @328e88f0 ) join the command group
/// +0x18 as group 2 ( shown on the command tab whatever +0x8c is ). June's guard also needs g_pData +0x38 and two June globals
/// ( 337676e8 / 337676ec ); the rest ( 328e73f0 description, 3220d4d0 rows, 328e5290 panel, the _MINOR pass ) is S7e's.
void CX2UISkillTreeNew::RegisterMajorCommandPanelControls()
{
	if ( NULL == m_pDLGUISkillTree )
		return;

	AddCommandGroupControl( m_pDLGUISkillTree->GetControl( L"Command_Back_Black" ), 2 );
	AddCommandGroupControl( m_pDLGUISkillTree->GetControl( L"SkillTree_MajorCommand" ), 2 );
}

/// June 328ded30: a found control is pushed as { group, control } ( 3207b770 )
void CX2UISkillTreeNew::AddCommandGroupControl( CKTDGUIControl* pControl_, const int iGroup_ )
{
	if ( NULL != pControl_ )
		m_vecCommandGroupControl.push_back( std::make_pair( iGroup_, pControl_ ) );
}

/// June 328da3c0( bShow, group ): hidden -> every control off; shown -> group 2 on, the asked group on, every other group off
/// ( SetShowEnable, vslot 0x78 )
void CX2UISkillTreeNew::ShowCommandGroup( const bool bShow_, const int iGroup_ )
{
	for ( vector< std::pair<int, CKTDGUIControl*> >::iterator it = m_vecCommandGroupControl.begin(); it != m_vecCommandGroupControl.end(); ++it )
	{
		if ( NULL == it->second )
			continue;

		if ( false == bShow_ )
			it->second->SetShowEnable( false, false );
		else if ( 2 == it->first || iGroup_ == it->first )
			it->second->SetShowEnable( true, true );
		else
			it->second->SetShowEnable( false, false );
	}
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
/////////////////////////////////////////////////////////////////////////
// S7e: the description panels ( major skills, major commands ) and the command tab. The data is June's
// SkillCommandDescriptionData.lua ( CX2SkillTree 3220bdb0 ), ComboTree.lua ( 3220d4d0 / 328e7ba0 ) and CommandIconData.lua
// ( the CX2SkillTree map +0xc4 ). The panel cases 29061 / 29062 / 29069 / 29070 / 29073 / 29074 have no sender in June's code
// or data, and 29071 / 29072's buttons are never built ( the +0x90 map stays empty ): JUNE_BUGS.
/////////////////////////////////////////////////////////////////////////

namespace
{
	/// June 328e42d0 / 328e5290 / 328de9a0 build three UIPointData ( 31c9ddf0 ) per button, all at one rectangle
	CKTDGUIControl::UIPointData* NewPointAt( const float fX_, const float fY_ )
	{
		CKTDGUIControl::UIPointData* pPoint = new CKTDGUIControl::UIPointData();
		pPoint->leftTopPoint = D3DXVECTOR2( fX_, fY_ );
		return pPoint;
	}

	CKTDGUIControl::UIPointData* NewPointRect( const D3DXVECTOR2& vPos_, const D3DXVECTOR2& vSize_ )
	{
		CKTDGUIControl::UIPointData* pPoint = new CKTDGUIControl::UIPointData();
		pPoint->leftTopPoint		= vPos_;
		pPoint->rightTopPoint		= D3DXVECTOR2( vPos_.x + vSize_.x, vPos_.y );
		pPoint->leftBottomPoint		= D3DXVECTOR2( vPos_.x, vPos_.y + vSize_.y );
		pPoint->rightBottomPoint	= D3DXVECTOR2( vPos_.x + vSize_.x, vPos_.y + vSize_.y );
		pPoint->bUseTextureSize		= false;		/// +0x38 = 0
		return pPoint;
	}

	/// June 328e4bd0 / 328e5290: x after the static's last picture ( pos.x +0xc + size.x +0x14 + 2 ), or fX_ when it has none
	float NextIconX( CKTDGUIStatic* pStatic_, const float fX_ )
	{
		CKTDGUIControl::CPictureData* pLast = pStatic_->GetPicture( pStatic_->GetPictureNum() - 1 );
		if ( NULL == pLast )
			return fX_;
		return pLast->GetPos().x + pLast->GetSize().x + 2.f;
	}
}

/// June 328e42d0 ( the portrait builder 328e8e90 @328e9125 ): the major-skill panel. TRANSCENDENTAL_SKILL_DATA ( +0x00 ) replaces
/// SKILL_DATA ( +0x14 ) for a transcended unit ( my unit +0x3c ) when neither instance code 0x910 nor 0x911 is on ( June inlines
/// IsEnableCode( 0x910 ) as the +0xa18 map lookup ). Skill_Description gets the class text; each SKILL_DATA row is a
/// MajorSkillBack_%d static ( the skill-type background ) and a MajorSkillButton_%d button ( the skill icon; guide text = the
/// row's string, line-broken to 0x172 with font 1 ), 70 px apart from x 111.
void CX2UISkillTreeNew::CreateMajorSkillPanel( CX2Unit::UNIT_CLASS eUnitClass_ )
{
	if ( NULL == m_pDLGUISkillTree || NULL == g_pData->GetSkillTree() )
		return;

	/// @328e4348..328e43c1. March has no transcendence ( JuneIsMyUnitTranscended ), so the copy never runs here
	if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() && NULL != g_pInstanceData )
	{
		if ( false == g_pInstanceData->IsEnableCode( 0x910 ) && false == g_pInstanceData->IsEnableCode( 0x911 ) &&
			 true == IsMyUnitTranscended() && false == m_kSkillCommandDesc.m_kTranscendental.m_vecSkill.empty() )
			m_kSkillCommandDesc.m_kSkill = m_kSkillCommandDesc.m_kTranscendental;	/// 32714e10
	}

	/// @328e43e6..328e453b ( March's GET_STRING carries June's "[S%1%]%2%" debug branch, 31c31060 +0x40 )
	CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Skill_Description" ) );
	if ( NULL != pDescription )
	{
		const int iDescStringID = m_kSkillCommandDesc.m_kSkill.m_iDescStringID;
		pDescription->SetString( 0, GET_STRING( iDescStringID ) );
		pDescription->AddDummyInt( iDescStringID );			/// +0x1b0; 29062 reads it back
		FitDescriptionText( pDescription );
		m_vecSkillTabControl.push_back( pDescription );
	}

	CKTDGUIControl* pControl = m_pDLGUISkillTree->GetControl( L"SkillTree_MajorSkill" );
	if ( NULL != pControl )
		m_vecSkillTabControl.push_back( pControl );
	pControl = m_pDLGUISkillTree->GetControl( L"SkillTree_Back_Black" );
	if ( NULL != pControl )
		m_vecSkillTabControl.push_back( pControl );

	WCHAR buf[255] = { 0, };
	const std::vector< std::pair<int, int> >& vecSkill = m_kSkillCommandDesc.m_kSkill.m_vecSkill;
	for ( UINT i = 0; i < vecSkill.size(); ++i )
	{
		/// 32206450( id, 1 ): its level gate needs instance code 0x910 / 0x911 ( none offline ), so it is March's GetSkillTemplet( id )
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( vecSkill[i].first ) );
		if ( NULL == pSkillTemplet )
			continue;

		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		CKTDGUIButton* pButton = new CKTDGUIButton();

		m_pDLGUISkillTree->AddControl( pStatic );
		m_pDLGUISkillTree->AddControl( pButton );
		StringCchPrintf( buf, 255, L"MajorSkillBack_%d", i );
		pStatic->SetName( buf );
		StringCchPrintf( buf, 255, L"MajorSkillButton_%d", i );
		pButton->SetName( buf );

		pPicture->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", GetSkillTypeBGKey( static_cast<int>( pSkillTemplet->m_eType ) ) );
		pStatic->AddPicture( pPicture );

		pButton->SetNormalTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		pButton->SetOverTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		pButton->SetDownTex( pSkillTemplet->m_wstrIconName.c_str(), pSkillTemplet->m_wstrIconPieceName.c_str() );
		pButton->AddDummyInt( vecSkill[i].second );			/// +0x1b0; 29061 reads it
		pButton->SetGuideDesc( g_pMain->GetStrByLineBreakColor( GET_STRING( vecSkill[i].second ), 0x172, 1 ).c_str() );	/// +0x1d0, 31d43ef0

		/// @328e49ec: ( i * 70 + 111, 165 ); the button's rectangle starts 3 px right of it at y 168
		const float fX = static_cast<float>( i ) * 70.f + 111.f;
		SetPointByTextureSize( pPicture );				/// 31c94970
		pPicture->SetPos( D3DXVECTOR2( fX, 165.f ) );		/// 31c93530
		pButton->SetNormalPoint( NewPointAt( fX + 3.f, 168.f ) );
		pButton->SetOverPoint( NewPointAt( fX + 3.f, 168.f ) );
		pButton->SetDownPoint( NewPointAt( fX + 3.f, 168.f ) );

		m_vecSkillTabControl.push_back( pButton );
		m_vecSkillTabControl.push_back( pStatic );
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] UI major skills class %d: description %d, %d skill button(s)",
		static_cast<int>( eUnitClass_ ), m_kSkillCommandDesc.m_kSkill.m_iDescStringID, static_cast<int>( vecSkill.size() ) );
}

/// June 328e8820 ( 328e8e90 @328e912e ): Command_Back_Black and SkillTree_MajorCommand join group 2, then the COMMAND_DATA panel
/// ( group 0 ) and, when bMinor_ ( June: my unit +0x3c, Luciel's ), the COMMAND_DATA_MINOR one ( group 1, class name + "_MINOR" )
void CX2UISkillTreeNew::CreateMajorCommandPanel( CX2Unit::UNIT_CLASS eUnitClass_, const bool bMinor_ )
{
	if ( NULL == m_pDLGUISkillTree || NULL == g_pData->GetUnitManager() || NULL == g_pData->GetSkillTree() )
		return;

	AddCommandGroupControl( m_pDLGUISkillTree->GetControl( L"Command_Back_Black" ), 2 );		/// @328e88cf
	AddCommandGroupControl( m_pDLGUISkillTree->GetControl( L"SkillTree_MajorCommand" ), 2 );	/// @328e88f0

	std::string strClassName = CX2SkillTree::GetUnitClassName( static_cast<int>( eUnitClass_ ) );
	SetCommandDescription( false );
	std::vector<CX2SkillTree::ComboRow> vecMajor;
	g_pData->GetSkillTree()->LoadMajorComboRows( &m_kSkillCommandDesc, strClassName, false, vecMajor );
	CreateMajorCommandRows( vecMajor, 0 );

	if ( true == bMinor_ )
	{
		strClassName += "_MINOR";
		SetCommandDescription( true );
		std::vector<CX2SkillTree::ComboRow> vecMinor;
		g_pData->GetSkillTree()->LoadMajorComboRows( &m_kSkillCommandDesc, strClassName, true, vecMinor );
		CreateMajorCommandRows( vecMinor, 1 );
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] UI major commands class %d: description %d, %d row(s)",
		static_cast<int>( eUnitClass_ ), m_kSkillCommandDesc.m_iCommandDescStringID, static_cast<int>( vecMajor.size() ) );
}

/// June 328e73f0( bMinor ): Command_Description carries both description ids as dummy ints 0 / 1 ( pushed again on each call ),
/// shows the asked one, is fitted ( 328df060 ) and joins group 2
void CX2UISkillTreeNew::SetCommandDescription( const bool bMinor_ )
{
	CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Command_Description" ) );
	if ( NULL == pDescription )
		return;

	const int iMajorStringID = m_kSkillCommandDesc.m_iCommandDescStringID;
	const int iMinorStringID = m_kSkillCommandDesc.m_iMinorDescStringID;
	pDescription->AddDummyInt( iMajorStringID );
	pDescription->AddDummyInt( iMinorStringID );
	const int iShownStringID = ( false == bMinor_ ) ? iMajorStringID : iMinorStringID;	/// GET_STRING does not parenthesize its argument
	pDescription->SetString( 0, GET_STRING( iShownStringID ) );
	FitDescriptionText( pDescription );
	AddCommandGroupControl( pDescription, 2 );			/// @328e7668: { 2, Command_Description }
}

/// June 328e8cd0( group ): +0x8c = group; on the command tab Command_Description shows dummy int ( group != 0 ), is fitted, and the
/// tab is refreshed ( 328e89f0 )
void CX2UISkillTreeNew::SetCommandGroup( const int iGroup_ )
{
	m_iCommandGroup = iGroup_;
	if ( 1 != m_iSelectedTab || NULL == m_pDLGUISkillTree )
		return;

	CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Command_Description" ) );
	if ( NULL != pDescription )
	{
		pDescription->SetString( 0, GET_STRING( pDescription->GetDummyInt( ( 0 != iGroup_ ) ? 1 : 0 ) ) );
		FitDescriptionText( pDescription );
	}
	UpdateTabUI();
}

/// June 328df060( static ): string 0 is line-broken to 0x13b with font 1 ( 31d43ef0 ). When that changed it: the lines are counted
/// ( "\r\n" and " \n" ), a line break right before a "#Crrggbb#CX" block becomes a space ( one line less ); 6+ lines lose the
/// doubled "\r\n" ( every one, counted once ); 5+ lines move the string up by ( lines * 5 - 15 ) * 2, a move made once and kept in
/// dummy pos 0. JUNE_BUGS: a "#C" within the text's last 11 characters never advances the scan ( June hangs there; no live
/// description text has one, checked over all 860 ids of SkillCommandDescriptionData.lua ).
void CX2UISkillTreeNew::FitDescriptionText( CKTDGUIStatic* pStatic_ )
{
	if ( NULL == pStatic_ || NULL == pStatic_->GetString( 0 ) )
		return;

	const wstring wstrMsg = pStatic_->GetString( 0 )->msg;
	wstring wstrText = g_pMain->GetStrByLineBreakColor( wstrMsg.c_str(), 0x13b, 1 );
	if ( 0 == wstrText.compare( wstrMsg ) )
		return;

	int iLineCount = 0;
	for ( wstring::size_type iPos = wstrText.find( L"\r\n", 0, 2 ); wstring::npos != iPos; iPos = wstrText.find( L"\r\n", iPos + 1, 2 ) )
		++iLineCount;
	for ( wstring::size_type iPos = wstrText.find( L" \n", 0, 2 ); wstring::npos != iPos; iPos = wstrText.find( L" \n", iPos + 1, 2 ) )
		++iLineCount;

	wstring::size_type iPos = 0;
	while ( true )
	{
		iPos = wstrText.find( L"#C", iPos, 2 );
		if ( wstring::npos == iPos )
			break;
		if ( 0 == iPos )
		{
			iPos = 2;
			continue;
		}
		if ( iPos + 0xb >= wstrText.size() )
			continue;		/// @328df23f: June jumps back to the find with the same position ( the JUNE_BUGS hang above )

		if ( L'#' == wstrText[iPos + 8] && L'C' == wstrText[iPos + 9] && L'X' == wstrText[iPos + 10] && L'\n' == wstrText[iPos - 1] )
		{
			wstrText[iPos - 1] = L' ';
			--iLineCount;
			iPos += 0xb;
		}
		else
		{
			++iPos;
		}
	}

	if ( 6 <= iLineCount )
	{
		/// @328df2cb..328df315: a CStringW copy, Replace( L"\r\n\r\n", L"\r\n" ) ( 325ab6a0 ), assigned back. CString::Replace scans
		/// left to right without overlap, which is this loop
		for ( wstring::size_type iFound = wstrText.find( L"\r\n\r\n" ); wstring::npos != iFound; iFound = wstrText.find( L"\r\n\r\n", iFound + 2 ) )
			wstrText.replace( iFound, 4, L"\r\n" );
		--iLineCount;
	}

	if ( 5 <= iLineCount )
	{
		CKTDGUIControl::UIStringData* pString = pStatic_->GetString( 0 );
		if ( 0 == pStatic_->GetDummyPosNum() )
		{
			pString->pos.y -= static_cast<float>( ( iLineCount * 5 - 15 ) * 2 );
			pStatic_->AddDummyPos( D3DXVECTOR3( 0.f, pString->pos.y, 0.f ) );	/// 3210aae0 on +0x190
		}
		else
		{
			pString->pos.y = pStatic_->GetDummyPos( 0 ).y;
		}
	}

	if ( false == wstrText.empty() )
		pStatic_->SetString( 0, wstrText.c_str() );
}

/// June 328e5290( rows, group ): per MAJOR_COMBO row a MajorCommandBack_%d button ( BG_COMMAND, guide text = the row's string ) at
/// ( 107, y - 7 ) and a MajorCommand_%d static: the combo type's title ( 32205610 ) at ( 113, y + 1 ) and the row's icons from x 197
/// ( 32207630 ); rows are 40 px apart from y 174. Both join the group.
void CX2UISkillTreeNew::CreateMajorCommandRows( const std::vector<CX2SkillTree::ComboRow>& vecRow_, const int iGroup_ )
{
	if ( NULL == m_pDLGUISkillTree || true == vecRow_.empty() || NULL == g_pData->GetSkillTree() )
		return;

	WCHAR buf[255] = { 0, };
	float fY = 174.f;
	float fX = 197.f;
	for ( UINT i = 0; i < vecRow_.size(); ++i )
	{
		const CX2SkillTree::ComboRow& kRow = vecRow_[i];

		CKTDGUIButton* pButton = new CKTDGUIButton();
		StringCchPrintf( buf, 255, L"MajorCommandBack_%d", i );
		pButton->SetName( buf );
		m_pDLGUISkillTree->AddControl( pButton );
		pButton->SetNormalTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
		pButton->SetOverTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
		pButton->SetDownTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_COMMAND" );
		pButton->AddDummyInt( kRow.m_iStringID );			/// +0x1b0; 29069 reads it
		pButton->SetGuideDesc( g_pMain->GetStrByLineBreakColor( GET_STRING( kRow.m_iStringID ), 0x172, 1 ).c_str() );
		pButton->SetNormalPoint( NewPointAt( 107.f, fY - 7.f ) );
		pButton->SetOverPoint( NewPointAt( 107.f, fY - 7.f ) );
		pButton->SetDownPoint( NewPointAt( 107.f, fY - 7.f ) );
		AddCommandGroupControl( pButton, iGroup_ );

		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		StringCchPrintf( buf, 255, L"MajorCommand_%d", i );
		pStatic->SetName( buf );
		m_pDLGUISkillTree->AddControl( pStatic );

		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex	= 2;
		pString->color		= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
		pString->msg		= GET_STRING( CX2SkillTree::GetComboTitleStringID( kRow.m_iComboType ) );
		pString->sortFlag	= 0;
		pString->fontStyle	= CKTDGFontManager::FS_NONE;
		pString->pos		= D3DXVECTOR2( 113.f, fY + 1.f );

		for ( UINT j = 0; j < kRow.m_vecIcon.size(); ++j )
		{
			if ( 0 != j )
				fX = NextIconX( pStatic, fX );
			g_pData->GetSkillTree()->AddCommandIconPicture( kRow.m_vecIcon[j], fX, fY, pStatic );
		}

		fX = 197.f;
		fY += 40.f;
		AddCommandGroupControl( pStatic, iGroup_ );
	}
}

/// June 328e7ba0( class ) ( CreateSkillTree 328eaa50 @328eaa96 ): CommandControlList gets the class's NORMAL / DASH / JUMP /
/// DASH_JUMP combo rows from ComboTree.lua ( group 0 ), and for a Luciel class ( unit type 10, 323750f0 ) the "_MINOR" table's rows
/// over the same row numbers ( group 1 ). June reads the file with its inline XORCRC loader; March's shipped loader is the same
/// KLuaManager( L, 0, true ) + LoadAndDoMemory pair its own CX2ComboTree used.
void CX2UISkillTreeNew::CreateCommandLists( CX2Unit::UNIT_CLASS eUnitClass_ )
{
	if ( NULL == m_pDLGUISkillTree || NULL == g_pData->GetUnitManager() )
		return;

	CKTDGUIContolList* pControlList = static_cast<CKTDGUIContolList*>( m_pDLGUISkillTree->GetControl( L"CommandControlList" ) );
	if ( NULL == pControlList )
		return;

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	if ( false == g_pKTDXApp->LoadAndDoMemory( &luaManager, L"ComboTree.lua" ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI ComboTree.lua did not load: the command tab stays empty; check data036 carries June's file" );
		return;
	}

	m_iCommandRow = 0;
	std::string strClassName = CX2SkillTree::GetUnitClassName( static_cast<int>( eUnitClass_ ) );
	/// 323750f0: the class's unit templet +0x10 ( unit type ) == 10. March has no Luciel ( no unit type 10 ), so this is false here
	const CX2Unit::UnitTemplet* pUnitTemplet = g_pData->GetUnitManager()->GetUnitTemplet( eUnitClass_ );
	const bool bLuciel = ( NULL != pUnitTemplet && 10 == static_cast<int>( pUnitTemplet->m_UnitType ) );

	std::vector<CX2SkillTree::ComboRow> vecNormal, vecDash, vecJump, vecDashJump;
	if ( true == luaManager.BeginTable( strClassName.c_str() ) )
	{
		LoadComboRows( luaManager, "NORMAL_COMBO", vecNormal );
		LoadComboRows( luaManager, "DASH_COMBO", vecDash );
		LoadComboRows( luaManager, "JUMP_COMBO", vecJump );
		LoadComboRows( luaManager, "DASH_JUMP_COMBO", vecDashJump );
		luaManager.EndTable();
	}
	CreateCommandRows( pControlList, vecNormal, 0 );
	CreateCommandRows( pControlList, vecDash, 0 );
	CreateCommandRows( pControlList, vecJump, 0 );
	CreateCommandRows( pControlList, vecDashJump, 0 );
	const int iRowCount = m_iCommandRow;

	if ( true == bLuciel )
	{
		m_iCommandRow = 0;
		vecNormal.clear();
		vecDash.clear();
		vecJump.clear();
		vecDashJump.clear();
		strClassName += "_MINOR";
		if ( true == luaManager.BeginTable( strClassName.c_str() ) )
		{
			LoadComboRows( luaManager, "NORMAL_COMBO", vecNormal );
			LoadComboRows( luaManager, "DASH_COMBO", vecDash );
			LoadComboRows( luaManager, "JUMP_COMBO", vecJump );
			LoadComboRows( luaManager, "DASH_JUMP_COMBO", vecDashJump );
			luaManager.EndTable();
		}
		CreateCommandRows( pControlList, vecNormal, 1 );
		CreateCommandRows( pControlList, vecDash, 1 );
		CreateCommandRows( pControlList, vecJump, 1 );
		CreateCommandRows( pControlList, vecDashJump, 1 );
	}

	pControlList->SetIndex( 0, 0, true, true );			/// 31cccd90( 0, 0, 1, 1 )
	CX2OfflineLog::Server( L"[JUNE_SKILL] UI command tab class %d ( %S ): %d combo row(s), Luciel %d",
		static_cast<int>( eUnitClass_ ), strClassName.c_str(), iRowCount, ( true == bLuciel ) ? 1 : 0 );
}

/// June 328df4c0( lua, table name, out ): the table's INDEX_0, INDEX_1 ... rows until one is missing: [1] the movie name, [2..] the
/// COMBO_TREE_ICON values; the combo type is the table name's ( 32205870 )
void CX2UISkillTreeNew::LoadComboRows( KLuaManager& luaManager_, const char* pszTableName_, std::vector<CX2SkillTree::ComboRow>& vecRow_ )
{
	if ( false == luaManager_.BeginTable( pszTableName_ ) )
		return;

	const int iComboType = CX2SkillTree::GetComboType( pszTableName_ );
	char szIndex[256] = { 0, };
	for ( int i = 0; ; ++i )
	{
		CX2SkillTree::ComboRow kRow;
		kRow.m_iComboType = iComboType;
		StringCchPrintfA( szIndex, 255, "INDEX_%d", i );
		if ( false == luaManager_.BeginTable( szIndex ) )
			break;

		luaManager_.GetValue( 1, kRow.m_wstrMovie );
		int iIcon = 0;
		for ( int j = 2; true == luaManager_.GetValue( j, iIcon ); ++j )
			kRow.m_vecIcon.push_back( iIcon );
		luaManager_.EndTable();
		vecRow_.push_back( kRow );
	}
	luaManager_.EndTable();
}

/// June 328e4bd0( list, rows, group ): a header static ( the first row's combo title, 32205610, at ( 20, 16 ) over a BG_LV picture )
/// in the current row, then one static per row ( name: STR 2632 with the row index ) with its icons from x 80 at y 15 ( 32207630 )
/// and their tooltip buttons ( 328de9a0 ), each in the next row. JUNE_BUGS: the header shares its row with the first combo row.
void CX2UISkillTreeNew::CreateCommandRows( CKTDGUIContolList* pControlList_, const std::vector<CX2SkillTree::ComboRow>& vecRow_, const int iGroup_ )
{
	if ( NULL == pControlList_ || true == vecRow_.empty() || NULL == g_pData->GetSkillTree() )
		return;

	CKTDGUIStatic* pHeader = new CKTDGUIStatic();
	const wstring wstrTitle = GET_STRING( CX2SkillTree::GetComboTitleStringID( vecRow_[0].m_iComboType ) );
	pHeader->SetName( wstrTitle.c_str() );
	m_pDLGUISkillTree->AddControl( pHeader );

	CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
	pHeader->AddString( pString );
	pString->color		= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	pString->fontIndex	= 2;
	pString->msg		= wstrTitle;
	pString->pos		= D3DXVECTOR2( 20.f, 16.f );
	pString->sortFlag	= 1;
	pString->fontStyle	= CKTDGFontManager::FS_NONE;

	CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
	pHeader->AddPicture( pPicture );
	pPicture->SetTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"BG_LV" );
	CKTDGUIControl::UIPointData* pPoint = NewPointAt( -20.f, 10.f );
	pPoint->addSize.x = 18.f;						/// +0x3c
	pPicture->SetPoint( pPoint );

	pControlList_->InsertItem( pHeader, m_iCommandRow, 0 );
	AddCommandGroupControl( pHeader, iGroup_ );

	for ( UINT i = 0; i < vecRow_.size(); ++i )
	{
		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		const wstring wstrName = GET_REPLACED_STRING( ( 0xa48, "i", static_cast<int>( i ) ) );		/// 31c325d0( 0xa48, "i", i )
		pStatic->SetName( wstrName.c_str() );
		m_pDLGUISkillTree->AddControl( pStatic );

		float fX = 80.f;
		const std::vector<int>& vecIcon = vecRow_[i].m_vecIcon;
		for ( UINT j = 0; j < vecIcon.size(); ++j )
		{
			fX = NextIconX( pStatic, fX );
			g_pData->GetSkillTree()->AddCommandIconPicture( vecIcon[j], fX, 15.f, pStatic );
			CKTDGUIButton* pToolTip = CreateCommandToolTipButton( vecIcon[j], pStatic );
			if ( NULL != pToolTip )
			{
				pControlList_->InsertItem( pToolTip, m_iCommandRow, 0 );
				AddCommandGroupControl( pToolTip, iGroup_ );
			}
		}

		pControlList_->InsertItem( pStatic, m_iCommandRow, 0 );
		++m_iCommandRow;
		AddCommandGroupControl( pStatic, iGroup_ );
	}
}

/// June 328de9a0( icon, static ): a tooltip button over the static's last picture ( or the static itself ): named
/// "SpecialButtonToopTip" + the static's name, invisible until hovered ( Bg_Command_Overlay ), dummy int = the icon's string for my
/// unit type ( 328da200 ), dummy pos = its outline colour ( 328da290 ), messages 29071 / 29072. Only when 328da200 finds a string:
/// JUNE_BUGS, June's +0x90 map is never filled, so this always returns NULL. June also clears a button byte +0x35e ( unidentified
/// in March's CKTDGUIButton; unreachable )
CKTDGUIButton* CX2UISkillTreeNew::CreateCommandToolTipButton( const int iIcon_, CKTDGUIStatic* pStatic_ )
{
	if ( NULL == pStatic_ )
		return NULL;

	const int iStringID = GetCommandToolTipStringID( iIcon_ );
	if ( iStringID < 0 )
		return NULL;

	CKTDGUIButton* pButton = new CKTDGUIButton();
	D3DXVECTOR2 vPos = pStatic_->GetPos();
	D3DXVECTOR2 vSize = vPos;
	CKTDGUIControl::CPictureData* pLast = pStatic_->GetPicture( pStatic_->GetPictureNum() - 1 );
	if ( NULL != pLast )
	{
		vSize = pLast->GetSize();
		vPos = pLast->GetPos();
	}

	wstring wstrName = L"SpecialButtonToopTip";
	wstrName += pStatic_->GetName();
	pButton->SetName( wstrName.c_str() );
	m_pDLGUISkillTree->AddControl( pButton );
	pButton->SetNormalTex( L"DLG_Common_New_Texture03.TGA", L"invisible" );
	pButton->SetOverTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"Bg_Command_Overlay" );
	pButton->SetDownTex( L"DLG_UI_Common_Texture_NewSkill_02.TGA", L"Bg_Command_Overlay" );
	pButton->AddDummyInt( iStringID );

	D3DXVECTOR3 vColor( 1.f, 1.f, 1.f );
	if ( true == GetCommandToolTipColor( iIcon_, vColor ) )
		pButton->AddDummyPos( vColor );

	pButton->SetCustomMsgMouseOver( STUCM_TOOLTIP_COMMAND_MOUSE_OVER );	/// +0x274
	pButton->SetCustomMsgMouseOut( STUCM_TOOLTIP_COMMAND_MOUSE_OUT );	/// +0x270
	pButton->SetNormalPoint( NewPointRect( vPos, vSize ) );
	pButton->SetOverPoint( NewPointRect( vPos, vSize ) );
	pButton->SetDownPoint( NewPointRect( vPos, vSize ) );
	return pButton;
}

/// June 328da200( icon ): the +0x90 entry's first m_ToolTipString row whose unit type is my unit's ( 31edf010 ) or 0; -1 when none
int CX2UISkillTreeNew::GetCommandToolTipStringID( const int iIcon_ ) const
{
	std::map< int, CX2SkillTree::CommandIconData >::const_iterator mit = m_mapCommandToolTip.find( iIcon_ );
	if ( mit == m_mapCommandToolTip.end() )
		return -1;

	int iUnitType = 0;
	if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		iUnitType = static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetType() );

	const std::vector< std::pair<int, int> >& vecString = mit->second.m_vecToolTipString;
	for ( UINT i = 0; i < vecString.size(); ++i )
	{
		if ( iUnitType == vecString[i].first || 0 == vecString[i].first )
			return vecString[i].second;
	}
	return -1;
}

/// June 328da290( icon, out ): the +0x90 entry's outline colour ( node +0x80 )
bool CX2UISkillTreeNew::GetCommandToolTipColor( const int iIcon_, D3DXVECTOR3& vColor_ ) const
{
	std::map< int, CX2SkillTree::CommandIconData >::const_iterator mit = m_mapCommandToolTip.find( iIcon_ );
	if ( mit == m_mapCommandToolTip.end() )
		return false;

	vColor_ = mit->second.m_vToolTipOutLineColor;
	return true;
}

/// June 328e6cd0( bShow, y, string id, movie, edge colour ): the DLG_UI_Command_Desc_Video.lua popup ( +0x30, created on first use
/// with layer 400, 0.07 s ). Shown: CommandDesc's text is the string ( -2 clears it ), its picture is sized to the text ( width: the
/// line-broken string or the current text, + 10; height: dummy int 0 + the text height ) with alpha 0.7 and an edge in the colour,
/// and the dialog moves to y. JUNE_BUGS: the movie name is never used ( no video plays ).
void CX2UISkillTreeNew::ShowCommandDescPopup( const bool bShow_, const float fPosY_, const int iStringID_, const wstring& wstrMovie_,
	const D3DXVECTOR3& vEdgeColor_ )
{
	if ( NULL == m_pDLGCommandDesc || false == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGCommandDesc ) )
	{
		m_pDLGCommandDesc = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Command_Desc_Video.lua", 0.07f, XDL_POP_UP );
		if ( NULL == m_pDLGCommandDesc )
			return;
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCommandDesc );
	}

	CKTDGUIStatic* pDesc = static_cast<CKTDGUIStatic*>( m_pDLGCommandDesc->GetControl( L"CommandDesc" ) );
	if ( false == bShow_ )
	{
		m_pDLGCommandDesc->SetShow( false );
		if ( NULL != pDesc )
			pDesc->SetString( 0, L"" );
		return;
	}

	CKTDGUIControl::CPictureData* pPicture = ( NULL != pDesc ) ? pDesc->GetPicture( 0 ) : NULL;
	CKTDGUIControl::UIStringData* pString = ( NULL != pDesc ) ? pDesc->GetString( 0 ) : NULL;
	if ( NULL == pPicture || NULL == pString )
	{
		m_pDLGCommandDesc->SetShow( false );
		return;
	}

	CKTDGUIDialogManager* pDialogManager = g_pKTDXApp->GetDGManager()->GetDialogManager();
	int iHeight = pDesc->GetDummyInt( 0 );
	if ( 0 <= iStringID_ )
	{
		const wstring wstrText = GET_STRING( iStringID_ );
		pDesc->SetString( 0, wstrText.c_str() );
		CKTDGFontManager::CUKFont* pFont = pDialogManager->GetUKFont( pString->fontIndex );
		if ( NULL != pFont )
			iHeight += pFont->GetHeight( wstrText.c_str() );	/// 31c3db10
	}
	else if ( -2 == iStringID_ )
	{
		pDesc->SetString( 0, L"" );
	}
	const int iScaledHeight = static_cast<int>( static_cast<float>( iHeight ) / g_pKTDXApp->GetResolutionScaleY() );	/// +0x1bc

	int iWidth = 10;
	CKTDGFontManager::CUKFont* pFont = pDialogManager->GetUKFont( pString->fontIndex );
	if ( NULL != pFont )
	{
		const wstring wstrLine = g_pMain->GetStrByLienBreak( GET_STRING( iStringID_ ),
			static_cast<int>( g_pKTDXApp->GetResolutionScaleX() * 330.0 ), 1 );		/// 31d79390, scale +0x1b8
		iWidth = static_cast<int>( static_cast<float>( pFont->GetWidth( wstrLine.c_str() ) ) / g_pKTDXApp->GetResolutionScaleX() ) + 10;	/// 31c3aa90
	}
	const wstring wstrMsg = pString->msg;
	CKTDGFontManager::CUKFont* pMsgFont = pDialogManager->GetUKFont( pString->fontIndex );
	if ( NULL != pMsgFont )
	{
		const int iMsgWidth = static_cast<int>( static_cast<float>( pMsgFont->GetWidth( wstrMsg.c_str() ) ) / g_pKTDXApp->GetResolutionScaleX() ) + 10;
		if ( iWidth < iMsgWidth )
			iWidth = iMsgWidth;
	}

	pPicture->SetSize( D3DXVECTOR2( static_cast<float>( iWidth ), static_cast<float>( iScaledHeight ) ) );	/// 31c949f0 ( GetControl_LUA's picture 0 )
	pPicture->SetSize( D3DXVECTOR2( pPicture->GetSize().x, static_cast<float>( iScaledHeight ) ) );
	pPicture->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, 0.7f ) );
	pPicture->SetEdge( false, 2, D3DXCOLOR( vEdgeColor_.x, vEdgeColor_.y, vEdgeColor_.z, 1.f ) );	/// June +0x64 colour, +0x74 2, +0x7c 0

	D3DXVECTOR2 vDialogPos = m_pDLGCommandDesc->GetPos();
	vDialogPos.y = fPosY_;
	m_pDLGCommandDesc->SetPos( vDialogPos );
	m_pDLGCommandDesc->SetShow( true );
}

/// June UICustomEventProc 328eab00, the panel cases. lParam is the sending control ( ESI ). 29061 / 29062 return true, the rest false.
bool CX2UISkillTreeNew::PanelCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>( lParam );
	switch ( wParam )
	{
	case STUCM_MAJOR_DESC_SKILL_MOUSE_OVER:		/// 328ec85b: the hovered button's string ( dummy int 0 ) into Skill_Description
		{
			if ( NULL == m_pDLGUISkillTree || NULL == pControl )
				return false;
			CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Skill_Description" ) );
			if ( NULL == pDescription )
				return false;
			pDescription->SetString( 0, GET_STRING( pControl->GetDummyInt( 0 ) ) );
		} return true;

	case STUCM_MAJOR_DESC_SKILL_MOUSE_OUT:		/// 328ec9b6: Skill_Description back to its own string ( its dummy int 0 )
		{
			if ( NULL == m_pDLGUISkillTree )
				return false;
			CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Skill_Description" ) );
			if ( NULL == pDescription )
				return false;
			pDescription->SetString( 0, GET_STRING( pDescription->GetDummyInt( 0 ) ) );
		} return true;

	case STUCM_MAJOR_DESC_COMMAND_MOUSE_OVER:	/// 328ecb05
		{
			if ( NULL == m_pDLGUISkillTree || NULL == pControl )
				return false;
			CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Command_Description" ) );
			if ( NULL != pDescription )
				pDescription->SetString( 0, GET_STRING( pControl->GetDummyInt( 0 ) ) );
		} return false;

	case STUCM_MAJOR_DESC_COMMAND_MOUSE_OUT:	/// 328ecc59: back to the shown group's description ( dummy int ( +0x8c != 0 ) )
		{
			if ( NULL == m_pDLGUISkillTree )
				return false;
			CKTDGUIStatic* pDescription = static_cast<CKTDGUIStatic*>( m_pDLGUISkillTree->GetControl( L"Command_Description" ) );
			if ( NULL != pDescription )
				pDescription->SetString( 0, GET_STRING( pDescription->GetDummyInt( ( 0 != m_iCommandGroup ) ? 1 : 0 ) ) );
		} return false;

	case STUCM_TOOLTIP_COMMAND_MOUSE_OVER:		/// 328ecd9f: the popup at the button's y with its string and colour ( dummy pos 0, else white )
		{
			if ( NULL == pControl )
				return false;
			m_bCommandToolTip = true;
			const int iStringID = pControl->GetDummyInt( 0 );
			const float fPosY = pControl->GetPos().y;
			const D3DXVECTOR3 vColor = ( 0 < pControl->GetDummyPosNum() ) ? pControl->GetDummyPos( 0 ) : D3DXVECTOR3( 1.f, 1.f, 1.f );
			ShowCommandDescPopup( true, fPosY, iStringID, L"", vColor );
		} return false;

	case STUCM_TOOLTIP_COMMAND_MOUSE_OUT:		/// 328ece68: kept open and emptied while a movie control is hovered, else closed
		m_bCommandToolTip = false;
		if ( true == m_bCommandMovieToolTip )
			ShowCommandDescPopup( true, 0.f, -2, L"Unknown", D3DXVECTOR3( 1.f, 1.f, 1.f ) );
		else
			ShowCommandDescPopup( false, 0.f, -1, L"Unknown", D3DXVECTOR3( 1.f, 1.f, 1.f ) );
		return false;

	case STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OVER:	/// 328ecf35: the control's dummy string 0 is the movie ( unused by 328e6cd0 )
		{
			if ( NULL == pControl )
				return false;
			m_bCommandMovieToolTip = true;
			const wstring wstrMovie = pControl->GetDummyString( 0 );
			ShowCommandDescPopup( true, pControl->GetPos().y, -1, wstrMovie, D3DXVECTOR3( 1.f, 1.f, 1.f ) );
		} return false;

	case STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OUT:	/// 328ecfbf
		if ( false == m_bCommandToolTip )
			ShowCommandDescPopup( false, 0.f, -1, L"Unknown", D3DXVECTOR3( 1.f, 1.f, 1.f ) );
		m_bCommandMovieToolTip = false;
		return false;

	default:
		break;
	}
	return false;
}
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS

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
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
			/// the memo popups of the skill ( 328dda50( 1 ) @328ddf8b, 328dda50( 0 ) @328ddf95 )
			UpdateMemoPopup( it->second, true );
			UpdateMemoPopup( it->second, false );
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
			/// S7c: the memo popups of the skill ( 328dda50( 1 ), 328dda50( 0 ) )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// the memo popups of the skill ( 328dda50( 1 ) @328de02c, 328dda50( 0 ) @328de035 )
		UpdateMemoPopup( it->second, true );
		UpdateMemoPopup( it->second, false );
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// S7c: the memo popups of the skill ( 328dda50( 1 ), 328dda50( 0 ) )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	}
}

/// June 328da2e0 ( caller 31f08290, the new-skill NOT ); June walks its 7-byte KSkillData vector
void CX2UISkillTreeNew::MarkNewSkills( const std::vector<KSkillData>& vecSkillData_ )
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
void CX2UISkillTreeNew::RecountNewSkills()
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

#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL
/// June 328da730 ( from 31fd0620 ): the first bar slot of SlotA1-5 / SlotB1-5 whose data holds the skill redraws
void CX2UISkillTreeNew::MarkEquipSlotUpdated( const CX2SkillTree::SKILL_ID eSkillID_ )
{
	if ( NULL == m_pDLGGameSkillSlot )
		return;

	for ( int i = 0; i < 10; i++ )
	{
		WCHAR wszSlotName[16] = L"";
		swprintf_s( wszSlotName, 16, L"%s%d", 5 <= i ? L"SlotB" : L"SlotA", ( 5 <= i ? i - 5 : i ) + 1 );
		CKTDGUISlot* pSlot = static_cast<CKTDGUISlot*>( m_pDLGGameSkillSlot->GetControl( wszSlotName ) );
		CKTDGUISlotData* pSlotData = NULL != pSlot ? pSlot->GetSlotData() : NULL;
		if ( NULL == pSlotData )
			continue;

		// June reads the id at slot data +0x0c with no type test: equip and memorize slot data both keep it there
		CX2SkillTree::SKILL_ID eSlotSkillID = static_cast<CX2EquippedSkillSlotData*>( pSlotData )->m_eSkillID;
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
		if ( CKTDGUISlotData::SDT_MEMORIZE == pSlotData->m_eSlotDataType )
			eSlotSkillID = static_cast<CX2MemorizeSkillSlotData*>( pSlotData )->m_eSkillID;
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
		if ( eSkillID_ == eSlotSkillID )
		{
			pSlotData->m_bDataUpdated = true;
			return;
		}
	}
}
#endif SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL

/// June 328d98e0
void CX2UISkillTreeNew::DecreaseNewSkillCount()
{
	--m_iNewSkillCount;
	if ( m_iNewSkillCount < 0 )
		m_iNewSkillCount = 0;
}

/// June 328d9fb0 ( caller 324c3873, a state-menu path ). JUNE_BUGS: the radios are set only when the command-description dialog
/// ( +0x30 ) exists, which June tests instead of the tree dialog ( +0xb4 ) whose radios it sets.
void CX2UISkillTreeNew::SelectTab( const bool bSkillTab_ )
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

/// June 328e89f0: the skill tab shows SkillTreeControlList and the +0x08 controls; the command tab CommandControlList and the +0x18
/// group ( 328da3c0 ). The command rows themselves are S7e's
void CX2UISkillTreeNew::UpdateTabUI()
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
		/// @328e8aea: 328da3c0( command tab, skill tab ? 2 : +0x8c )
		ShowCommandGroup( false == bSkillTab, ( true == bSkillTab ) ? 2 : m_iCommandGroup );
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
static bool CanEquipSkillToSlot( const CX2UserSkillTree& refUserSkillTree_, const int iSlotID_, const CX2SkillTree::SKILL_ID eSkillID_ )
{
	/// June @32896cd1: no templet -> 32896f5f XOR AL,AL ( false; S7b review: S7b had true )
	if ( NULL == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID_ ) )
		return false;

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
		 m_pUserSkillTree->GetSkillLevel( pSlotData->m_eSkillID, false, 0, true ) < 1 )		/// June 328e6602: 32890620( id, 0, 0, 1 )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), GET_STRING( 35567 ), g_pMain->GetNowState() );
		return true;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSlotData->m_eSkillID );
	if ( NULL == pSkillTemplet || false == m_pUserSkillTree->DoIHaveThisSkill( pSlotData->m_eSkillID ) )
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
			const bool bAFull = ( 4 == iFreeA ) ? ( false == IsMyUnitTranscended() ) : ( -1 == iFreeA );
			if ( false == bAFull )
			{
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( iFreeA, pSlotData->m_eSkillID );
			}
			else if ( true == m_bIsExpandSkillSlot )
			{
				const int iFreeB = m_pUserSkillTree->GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, true );
				const bool bBFree = ( 4 == iFreeB ) ? m_pUserSkillTree->IsSlotUsable( 1 ) : ( -1 != iFreeB );
				if ( true == bBFree )
				{
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Equip.ogg", false, false );
					g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( iFreeB + 5, pSlotData->m_eSkillID );
				}
			}
			else if ( true == m_pUserSkillTree->IsSlotUsable( 1 ) )
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
			if ( NULL != m_pUserSkillTree && true == CanEquipSkillToSlot( *m_pUserSkillTree, pSlotData->GetSlotID(), eSkillID ) )
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
/*static*/ bool CX2UISkillTreeNew::IsSameOrUpgradeSkill( const int iSkill_, const int iTarget_ )
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
static void SetSlotLockControls( CKTDGUIDialogType pDLG_, const WCHAR* wszButton_, const WCHAR* wszLock_, const bool bLocked_ )
{
	CKTDGUIButton* pButton = (CKTDGUIButton*) pDLG_->GetControl( wszButton_ );
	if( NULL != pButton )
		pButton->SetShowEnable( bLocked_, bLocked_ );

	CKTDGUIStatic* pStaticLock = (CKTDGUIStatic*) pDLG_->GetControl( wszLock_ );
	if( NULL != pStaticLock )
		pStaticLock->SetShow( bLocked_ );
}

/// June 328dfab0: the B bar key labels ( SkillHotKeyEx ) go to alpha 1 or to the locked 0.5 ( _DAT_3343777c )
static void SetSlotHotKeyAlpha( CKTDGUIDialogType pDLG_, const float fAlpha_ )
{
	CKTDGUIStatic* pStaticSkillHotKey = (CKTDGUIStatic*) pDLG_->GetControl( L"SkillHotKeyEx" );
	if( NULL != pStaticSkillHotKey )
		pStaticSkillHotKey->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, fAlpha_ ) );
}

/// June 328dfab0: a locked slot shows no cool time: clear the text and hide the fade picture of slots [ iFrom_, iTo_ )
static void ClearSlotCoolTime( CKTDGUIDialogType pDLG_, const bool bSlotB_, const int iFrom_, const int iTo_ )
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
bool CX2UISkillTreeNew::AutoEquipNewSkills( const std::vector< int >& vecSkillID_ )
{
	/// +0xf4 = the unit's user skill tree ( set in InitSkillTreeUI 328ed890 )
	if( NULL == m_pUserSkillTree || true == vecSkillID_.empty() )
		return false;

	std::vector< int > vecEmptyA;
	std::vector< int > vecEmptyB;
	m_pUserSkillTree->GetEmptySlotIndexList( vecEmptyA, false, false );
	if( true == m_pUserSkillTree->IsSlotUsable( 0 ) )
		m_pUserSkillTree->GetEmptySlotIndexList( vecEmptyB, true, false );

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
void CX2UISkillTreeNew::UpdateSkillSlotLockUI()
{
	if( NULL == m_pDLGGameSkillSlot || NULL == m_pUserSkillTree )
		return;

	/// A-TC ( slot 4 ) is locked while CX2Unit +0x3c ( transcendence ) is 0. March has no such unit state: Stage 6 adds it, so the slot stays locked
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	const bool bTranscendent = IsMyUnitTranscended();
#else SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	const bool bTranscendent = false;
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	if( false == bTranscendent )
	{
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_A_TC", L"LOCK_SLOT_A_TC", true );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		ClearSlotCoolTime( m_pDLGGameSkillSlot, false, 4, 5 );
	}
	else
	{
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_A_TC", L"LOCK_SLOT_A_TC", false );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}

	/// B-TC ( slot 9 ): slot-state key 1 ( 32890ea0( 1 ) ) in { 1, 2 } unlocks it
	const bool bBTCLocked = ( false == m_pUserSkillTree->IsSlotUsable( 1 ) );
	if( false == bBTCLocked )
	{
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_B_TC", L"LOCK_SLOT_B_TC", false );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}
	else
	{
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot_B_TC", L"LOCK_SLOT_B_TC", true );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		ClearSlotCoolTime( m_pDLGGameSkillSlot, true, 4, 5 );

		CX2EquippedSkillSlotData* pSlotData = GetEquippedSlotData( 4, true );
		if( NULL != pSlotData && CX2SkillTree::SI_NONE != pSlotData->m_eSkillID && -1 != static_cast<int>( pSlotData->m_eSkillID ) )
			g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( pSlotData->GetSlotID(), CX2SkillTree::SI_NONE );
	}

	/// B ( slots 5-8 ): slot-state key 0 ( 32890ea0( 0 ) ) in { 1, 2 }; the emulator sends key 0 = 2, so the offline game never locks it
	const bool bBLocked = ( false == m_pUserSkillTree->IsSlotUsable( 0 ) );
	if( false == bBLocked )
	{
		m_bIsExpandSkillSlot = true;
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot", L"LOCK", false );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 1.f );
	}
	else
	{
		m_bIsExpandSkillSlot = false;
		SetSlotLockControls( m_pDLGGameSkillSlot, L"Button_Buy_Guide_Slot", L"LOCK", true );
		SetSlotHotKeyAlpha( m_pDLGGameSkillSlot, 0.5f );
		ClearSlotCoolTime( m_pDLGGameSkillSlot, true, 0, 4 );

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
			( false == bTranscendent ) ? L"locked" : L"open", ( true == bBTCLocked ) ? L"locked" : L"open", m_pUserSkillTree->GetSlotState( 1 ),
			( true == bBLocked ) ? L"locked" : L"open", m_pUserSkillTree->GetSlotState( 0 ), ( true == m_bIsExpandSkillSlot ) ? 1 : 0 );
	}
}

/// June 328daed0 ( UI type B ) / 328db100 ( UI type A ): where the TC lock pictures and buy-guide buttons sit for the current swap state
void CX2UISkillTreeNew::SetOffsetPosTCSlotUI( const bool bTypeA_ )
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
			UpdateSkillSlotLockUI();
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
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	if ( NULL != g_pX2Game )
		g_pX2Game->RefreshMemoRiseSlot();	// June 328e0ff0 @328e1097
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
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
	SetOffsetPosTCSlotUI( true );
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
	SetOffsetPosTCSlotUI( false );
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
/// June 328e6970. The radio of the select page ( my unit +0x4b8 = user tree +0x24 ) is checked ( 31caa0e0( 1, 0, 0 ) ), the radios follow the
/// opened page count ( +0x4b6 / +0x22 ). June's dialog has no Add_Skill_Page_ED and no Decide_Skill_Page: the ED button is looked up and
/// never used, and the cash button takes the place of the first closed radio. It shows in state 0xe ( XS_VILLAGE_MAP ) or 0xf ( June's
/// XS_GUILD_AGIT_GAME, which March has no state for ). Only Page1, Page2 and the cash button are tested for NULL.
void CX2UISkillTreeNew::UpdateSkillPageUI()
{
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;

	const USHORT usOpenedPage = refUserSkillTree.GetOpenedPage();
	const USHORT usSelectPage = refUserSkillTree.GetSelectPage();
	D3DXVECTOR2 vPos( 0.f, 0.f );

	CKTDGUIRadioButton* pRadioButton_Page1 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page1" ) );
	CKTDGUIRadioButton* pRadioButton_Page2 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page2" ) );
	CKTDGUIRadioButton* pRadioButton_Page3 = reinterpret_cast<CKTDGUIRadioButton*>( m_pDLGUISkillTree->GetControl( L"Skill_Page3" ) );
	CKTDGUIButton* pButton_Add_Page_Cash = reinterpret_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( L"Add_Skill_Page_Cash" ) );

	if ( NULL == pRadioButton_Page1 || NULL == pRadioButton_Page2 || NULL == pButton_Add_Page_Cash )
		return;

	switch ( usSelectPage )
	{
	case 1:	pRadioButton_Page1->SetChecked( true );	break;
	case 2:	pRadioButton_Page2->SetChecked( true );	break;
	case 3:	pRadioButton_Page3->SetChecked( true );	break;
	default:	break;
	}

	bool bPage1 = false, bPage2 = false, bPage3 = false, bAdd = false;
	switch ( usOpenedPage )
	{
	case 1:
		bPage1 = bAdd = true;
		vPos = pRadioButton_Page2->GetPos();	/// vslot 0x70
		break;

	case 2:
		bPage1 = bPage2 = bAdd = true;
		vPos = pRadioButton_Page3->GetPos();
		break;

	case 3:
		bPage1 = bPage2 = bPage3 = true;
		break;

	default:
		break;
	}

	pRadioButton_Page1->SetShowEnable( bPage1, bPage1 );	/// vslot 0x78
	pRadioButton_Page2->SetShowEnable( bPage2, bPage2 );
	pRadioButton_Page3->SetShowEnable( bPage3, bPage3 );

	if ( CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() )
	{
		pButton_Add_Page_Cash->SetOffsetPos( vPos );								/// vslot 0x50
		pButton_Add_Page_Cash->SetShowEnable( bAdd, bAdd );
		pButton_Add_Page_Cash->SetGuideDesc( GET_STRING( STR_ID_29367 ) );		/// 0x72b7, +0x1d0
		pButton_Add_Page_Cash->SetGuideDescOffsetPos( D3DXVECTOR2( 100, 0 ) );	/// vslot 0x94, _DAT_334355b8 = 100.0
	}
	else
	{
		pButton_Add_Page_Cash->SetShowEnable( false, false );
	}
}
#else SERV_IRUHADEV_JUNE_SKILL_PAGE
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
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE

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
		const int iAltSkillID = GetMyUnitTranscendenceSkillIDForUI( static_cast<int>( pSkillTemplet->m_eID ) );
		if ( iAltSkillID < 1 )
		{
			/// June @328e5bd2: the skill's own name ( assign )
			wstrDesc = pSkillTemplet->m_wstrName;

			/// June @328e5bd7..328e5cae: the base skill ( +0x178 m_DeleteSkillIDByUpgrade, else the id ) and its active memos
			int iBaseSkillID = pSkillTemplet->m_iDeleteSkillIDByUpgrade;
			if ( 0 == iBaseSkillID )
				iBaseSkillID = static_cast<int>( pSkillTemplet->m_eID );

			std::vector<int> vecMemoID;
			g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetActiveMemoList( iBaseSkillID, vecMemoID );	/// June 32891e10

			for ( std::vector<int>::const_iterator it = vecMemoID.begin(); it != vecMemoID.end(); ++it )
			{
				/// June 324550a0() -> 32453420( out, memo id ): the memo's name text
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
				const wstring wstrMemoName = CX2SkillNoteManager::GetMemoName( *it );
				/// June 31aab330 + 323e7600 + 31aa92b0: wstrDesc = ( name + L" " ) + wstrDesc, with no empty test
				wstrDesc = ( wstrMemoName + L" " ) + wstrDesc;
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
				wstring wstrMemoName = L"";
				/// S7c: 32453420 CX2SkillNoteManager memo-name text builder ( 711 lines, not ported ): wstrMemoName = CX2SkillNoteManager::GetInstance()->( 32453420 )( *it )
				if ( false == wstrMemoName.empty() )	/// S7b stand-in: June has no test; until 32453420 is ported the name is empty and nothing is prepended
				{
					/// June 31aab330 + 323e7600 + 31aa92b0: wstrDesc = ( name + L" " ) + wstrDesc
					wstrDesc = ( wstrMemoName + L" " ) + wstrDesc;
				}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
		const int iAltSkillID = GetMyUnitTranscendenceSkillIDForUI( static_cast<int>( pSkillTemplet->m_eID ) );
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
	const wstring* pTypeColor = s_kSkillTypeColorTable.Find( static_cast<int>( pSkillTemplet->m_eType ) );
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
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
	if ( CX2SkillTree::RNSI_A_AISHA_TELEPORT == iSkillID || CX2SkillTree::RNSI_A_AISHA_TELEPORT_UPGRADE == iSkillID )
#else
	if ( 22200 == iSkillID || 22216 == iSkillID )
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
		return;

	/// June @328e5eef: 31aa6560( L"\n", 1 ) = append
	wstrDesc += L"\n";

	/// June @328e5eff..328e5f50: mode 1 -> the alternate skill's templet ( 323511f0 + GetSkillTemplet ); no iAlt > 0 test here
	const CX2SkillTree::SkillTemplet* pAltTemplet = NULL;
	if ( 1 == iDescMode_ )
	{
		const int iAltSkillID = GetMyUnitTranscendenceSkillIDForUI( iSkillID );
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
		pUnit->GetUnitData().m_UserSkillTree.GetActiveMemoList( iBaseSkillID, vecMemoID );		/// June 32891e10

		for ( std::vector<int>::const_iterator it = vecMemoID.begin(); it != vecMemoID.end(); ++it )
		{
			/// June @328e63fd: a wstring built from L"" that is never used again ( destroyed at the loop's end ); not reproduced

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
			/// June 324550a0() -> 32453420( out, memo id ): the memo's name text
			const wstring wstrMemoName = CX2SkillNoteManager::GetMemoName( *it );
			/// June 324550a0() -> 324513e0( out, base skill id, memo id, unit class = select unit +0x38 ): the memo's effect text
			const wstring wstrMemoDesc = CX2SkillNoteManager::GetInstance()->GetMemoEffectDesc( iBaseSkillID, *it, static_cast<int>( pUnit->GetClass() ) );

			/// June has no test: always appended
			wstrDesc += L"\n\n";		/// June @328e645a: 3347e7ac
			wstrDesc += GET_REPLACED_STRING( ( 36281, "LL", wstrMemoName, wstrMemoDesc ) );	/// June 0x8db9 = 36281, "LL": ( name, effect )
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
static int LayoutSkillDescMain( CKTDGUIDialogType pDLGToolTip_, const int iFontIndex_, const int iMainDescYPos_, const int iDescWidth_,
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
		pAltTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( GetMyUnitTranscendenceSkillIDForUI( static_cast<int>( eSkillID ) ) ) );

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
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// June @328e9486..328e94bc: in mode 2 a skill with no row of its own is looked up through the class's transcendence memo that
		/// names it ( 32451ff0 on the id as given ); none ( <= 0 ) or no row for that skill either -> hidden
		if ( NULL == pSkillTreeTemplet && 2 == iDescMode_ )
		{
			const int iMemoSkillID = CX2SkillNoteManager::GetInstance()->GetSkillIDByTranscendenceValue( static_cast<int>( eSkillID ),
				static_cast<int>( pUnit->GetClass() ) );
			if ( 0 < iMemoSkillID )
				pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( static_cast<int>( pUnit->GetClass() ),
					static_cast<CX2SkillTree::SKILL_ID>( iMemoSkillID ) );
		}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
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
	if ( true == pSkillTemplet->m_bBornSealed && false == IsSkillAcquired( refUserSkillTree, eSkillID, false ) &&
		 false == refUserSkillTree.IsSkillUnsealed( eSkillID ) )
		GetSkillUnSealedDesc( wstrMain, eSkillID );

	/// @328e96e8: the common description ( the alternate skill's in mode 1 )
	wstrMain += ( NULL != pAltTemplet && 1 == iDescMode_ ) ? pAltTemplet->m_wstrMainDesc : pSkillTemplet->m_wstrMainDesc;

	/// @328e9710: Aisha ( unit type 2 ): a skill whose templet +0x170 is 3 gets "\n\n" + STR 30357 with the key of game action 0x27
	/// ( 32205af0, 31f5ee50 ). March's templet has no +0x170 field: character material ( checklist ).
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 CX2Unit::UT_ARME == g_pData->GetMyUser()->GetSelectUnit()->GetType() && NULL != g_pData->GetSkillTree() )
	{
		const CX2SkillTree::SkillTemplet* pMemorizeTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillTemplet->m_eID );
		if ( NULL != pMemorizeTemplet && 3 == pMemorizeTemplet->m_iWOSType )	// June +0x170 is m_iWOSType ( 32205af0 )
		{
			wstrMain += L"\n\n";
			wstrMain += GET_REPLACED_STRING( ( STR_ID_30357, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_SIT ) ) );	// June game action 0x27
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG
			CX2OfflineLog::Server( L"[JUNE_CHAR] ARME MEMO tooltip line skill %d", static_cast<int>( pSkillTemplet->m_eID ) );
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG
		}
	}
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

	wstrMain += L"\n";

	/// @328e97ea: main y = title y + 65, width 0x1a4; the closure runs with font 1, and again with font 0 when the tooltip is 0x300 or taller
	const int iMainDescYPos = static_cast<int>( pTitleStringData->pos.y + 65.f );
	const int iDescWidth = 0x1a4;
	const wstring wstrMainCopy = wstrMain;
	int iFontIndex = 1;
	int iHeight = LayoutSkillDescMain( m_pDLGSkillToolTip, iFontIndex, iMainDescYPos, iDescWidth, wstrMain, pTitleStringData, pMainStringData,
		this, iSkillLevel, pSkillTemplet, iDescMode_ );
	if ( 0x300 <= iHeight )
	{
		wstrMain = wstrMainCopy;
		iFontIndex = 0;
		iHeight = LayoutSkillDescMain( m_pDLGSkillToolTip, iFontIndex, iMainDescYPos, iDescWidth, wstrMain, pTitleStringData, pMainStringData,
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
				DecreaseNewSkillCount();
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
		UpdateTabUI();
		return false;

	case STUCM_SELECT_COMMAND_TAB:			/// 328ec84a ( returns false ); the command list is S7e's
		m_iSelectedTab = 1;
		UpdateTabUI();
		return false;

	case STUCM_TRANSFORM_SKILL_SLOT_MOUSEOUT:	/// 328eba62
		ShowSkillDesc( false );
		return true;

#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE
	case STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN:		/// 328ec719
		{
			CKTDGUISlot* pSlot = reinterpret_cast<CKTDGUISlot*>( lParam );
			if ( NULL == pSlot || NULL == pSlot->GetSlotData() || static_cast<CX2MemorizeSkillSlotData*>( pSlot->GetSlotData() )->m_eSkillID <= 0 )
				return true;
			if ( NULL == g_pX2Game || NULL == g_pX2Game->GetMyUnit() )
				return true;
			const double dRate = ( static_cast<double>( g_pX2Game->GetMyUnit()->GetCharacterSpecializationA() ) + 1.0 ) * 0.7f * 100.0;
			const wstring wstrRate = GET_REPLACED_STRING( ( 44613, "f", dRate ) );	// June then writes this over the slot object itself ( JUNE_BUGS ): dropped
			UNREFERENCED_PARAMETER( wstrRate );
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG
			CX2OfflineLog::Server( L"[JUNE_CHAR] ARME MEMO hover skill %d rate %.1f ( text dropped, JUNE_BUGS 400 )",
				static_cast<int>( static_cast<CX2MemorizeSkillSlotData*>( pSlot->GetSlotData() )->m_eSkillID ), dRate );
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG
		}
		return true;
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE

#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
	/// June's page cases: 29049 328ec068 ( all three Skill_Page radios send it ), 29052 328ec1ef, 29053 328ec3ff, 29055 328ec48e,
	/// 29056 328ec6b3. June's switch has no 29050 / 29051 / 29054 case.
	case STUCM_SKILL_PAGE1:
	case STUCM_ADD_SKILL_PAGE_ED:
	case STUCM_ADD_SKILL_PAGE_CASH:
	case STUCM_BUY_CONFIRM:
	case STUCM_BUY_CANCEL:
		return PageCustomEventProc( hWnd, uMsg, wParam, lParam );
#else SERV_IRUHADEV_JUNE_SKILL_PAGE
	/// Page cases ( S7d ): 29049 328ec068, 29052 328ec1ef, 29053 328ec3ff, 29055 328ec48e, 29056 328ec6b3. Until S7d ports June's, the
	/// page radios still run March's page code ( UpdateSkillPageUI / BuySkillPageDialog below ).
	case STUCM_SKILL_PAGE1:
	case STUCM_SKILL_PAGE2:
	case STUCM_SKILL_PAGE3:
	case STUCM_ADD_SKILL_PAGE_ED:
	case STUCM_ADD_SKILL_PAGE_CASH:
	case STUCM_BUY_CONFIRM:
	case STUCM_BUY_CANCEL:
		return SkillPageCustomEventProc( hWnd, uMsg, wParam, lParam );
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	case STUCM_SELECT_SKILL_LEFT_MEMO:			/// 328ed02c
	case STUCM_SELECT_SKILL_RIGHT_MEMO:			/// 328ed259
	case STUCM_SELECT_SKILL_ACTIVE_MEMO:		/// 328ed483
	case STUCM_SELECT_SKILL_DEACTIVE_MEMO:		/// 328ed519
	case STUCM_BUTTON_MEMO_SELECT_MOUSE_OVER:	/// 328ed5ca
	case STUCM_BUTTON_MEMO_SELECT_MOUSE_OUT:	/// 328ed64b
		return MemoCustomEventProc( hWnd, uMsg, wParam, lParam );
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// S7c ( memo ): 29065 328ed02c, 29066 328ed259, 29067 328ed483, 29068 328ed519, 29076 328ed5ca, 29077 328ed64b
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	case STUCM_MAJOR_DESC_SKILL_MOUSE_OVER:			/// 328ec85b
	case STUCM_MAJOR_DESC_SKILL_MOUSE_OUT:			/// 328ec9b6
	case STUCM_MAJOR_DESC_COMMAND_MOUSE_OVER:		/// 328ecb05
	case STUCM_MAJOR_DESC_COMMAND_MOUSE_OUT:		/// 328ecc59
	case STUCM_TOOLTIP_COMMAND_MOUSE_OVER:			/// 328ecd9f
	case STUCM_TOOLTIP_COMMAND_MOUSE_OUT:			/// 328ece68
	case STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OVER:	/// 328ecf35
	case STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OUT:		/// 328ecfbf
		return PanelCustomEventProc( hWnd, uMsg, wParam, lParam );
#else SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// S7e ( panels ): 29061 328ec85b, 29062 328ec9b6, 29069 328ecb05, 29070 328ecc59, 29071 328ecd9f, 29072 328ece68, 29073 328ecf35, 29074 328ecfbf
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// checklist ( class material ): 29059 328ec719 ( Aisha memorize slot ), 29078 328ed6a8 ( Luciel transform slot )
	default:
		break;
	}
	return false;
}



#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
/////////////////////////////////////////////////////////////////////////
// S7c: the skill memos ( traits )
/////////////////////////////////////////////////////////////////////////
namespace
{
	/// June's message boxes in the memo checks ( 3235b070 / 3235b750 / 3235be30, the ACK handlers ) sit at ( 250, 300 ) ( 334577b8 / 3344f348 )
	void MemoOKMsgBox( const wstring& wstrText_ )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), wstrText_.c_str(), g_pMain->GetNowState(), -1, -1.f );
	}

	/// June 3235b070 ( a CX2Unit thiscall on my unit; skill, memo, class ): may the memo be switched on? Each refusal shows its box.
	bool CanActivateMemo( const int iSkillID_, const int iMemoID_, const int iUnitClass_ )
	{
		const int iBaseSkillID = static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) ) );
		if ( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		{
			MemoOKMsgBox( GET_STRING( 35614 ) );		/// 0x8b1e "Unknown error has occurred."
			return false;
		}
		const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;

		/// @3235b101: the class has the row ( 32452150 )
		if ( false == CX2SkillNoteManager::GetInstance()->HasMemo( iSkillID_, iMemoID_, iUnitClass_ ) )
		{
			MemoOKMsgBox( GET_STRING( 35619 ) );		/// 0x8b23 "You can't apply a memo to this skill."
			return false;
		}

		/// @3235b119: one memo per skill: any active memo on the base ( 32891fc0 = 32891e10 not empty )
		std::vector<int> vecMemoID;
		refUserSkillTree.GetActiveMemoList( iBaseSkillID, vecMemoID );
		if ( false == vecMemoID.empty() )
		{
			MemoOKMsgBox( GET_STRING( 35620 ) );		/// 0x8b24 "A different trait has been applied..."
			return false;
		}

		/// @3235b290: the skill is acquired ( 328914b0( id, 0 ) )
		if ( false == IsSkillAcquired( refUserSkillTree, static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ), false ) )
		{
			MemoOKMsgBox( GET_STRING( 35621 ) );		/// 0x8b25 "You have yet to learn this skill."
			return false;
		}
		return true;
	}

	/// June 3235b750 ( my unit; skill, an argument it does not read ): the base skill has an active memo; else 0x8b26's box and false
	bool CanDeactivateMemo( const int iSkillID_ )
	{
		const int iBaseSkillID = static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) ) );
		std::vector<int> vecMemoID;
		g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetActiveMemoList( iBaseSkillID, vecMemoID );
		if ( false == vecMemoID.empty() )
			return true;
		MemoOKMsgBox( GET_STRING( 35622 ) );			/// 0x8b26 "There are no traits in effect."
		return false;
	}

	/// June 3235be30 ( my unit ): the UID of the first removal item ( AddRemovableMemoItemID's set, in order ) that the inventory holds
	/// ( 32869d40( id, 0, 1, 1 ): no bank-type categories, E-equip included = March's GetItemByTID( id, false, true ) ); 0 when none.
	/// An empty list shows "Unknown error" first ( 0x8b1e ).
	UidType FindMemoRemoveItemUID()
	{
		std::vector<int> vecItemID;
		CX2SkillNoteManager::GetInstance()->GetRemovableMemoItemIDList( vecItemID );
		if ( true == vecItemID.empty() )
		{
			MemoOKMsgBox( GET_STRING( 35614 ) );
			return 0;
		}
		const CX2Inventory& refInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		for ( std::vector<int>::const_iterator it = vecItemID.begin(); it != vecItemID.end(); ++it )
		{
			const CX2Item* pItem = refInventory.GetItemByTID( *it, false, true );
			if ( NULL != pItem )
				return pItem->GetUID();
		}
		return 0;
	}
}

/// June 328dcae0 ( bRight ): the side's button loses its text, is hidden and disabled and loses its hover messages ( +0x270 / +0x274 = -1 );
/// the state icon's three pictures are hidden
void CX2UISkillTreeNew::HideMemoPopup( SkillSlotUI& UISet_, const bool bRight_ )
{
	CKTDGUIButton* pButton = ( true == bRight_ ) ? UISet_.m_pButtonRightMemoDesc : UISet_.m_pButtonLeftMemoDesc;
	CKTDGUIStatic* pStatic = ( true == bRight_ ) ? UISet_.m_pStaticRightMemoState : UISet_.m_pStaticLeftMemoState;
	if ( NULL != pButton )
	{
		pButton->SetGuideDesc( L"" );					/// +0x1d0
		pButton->SetShowEnable( false, false );			/// vslot 0x78
		pButton->SetCustomMsgMouseOver( -1 );			/// +0x274
		pButton->SetCustomMsgMouseOut( -1 );			/// +0x270
	}
	if ( NULL != pStatic )
	{
		for ( int i = 0; i < 3; ++i )
		{
			if ( NULL != pStatic->GetPicture( i ) )
				pStatic->GetPicture( i )->SetShow( false );
		}
	}
}

/// June 328dda50 ( a SkillSlotUI thiscall, bRight ): hide the side, then with my unit: the class's memo row of the slot's skill by index
/// ( 32451140: 1 = right, 0 = left ). No row -> stays hidden. Else the state ( 32455150: 0 locked below the row's level, 1 open, 2 active )
/// goes into the side's state word, the button gets the guide text and is shown, and the state's picture is shown.
void CX2UISkillTreeNew::UpdateMemoPopup( SkillSlotUI& UISet_, const bool bRight_ )
{
	HideMemoPopup( UISet_, bRight_ );
	if ( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	int& riState					= ( true == bRight_ ) ? UISet_.m_iReserved20 : UISet_.m_iReserved14;
	CKTDGUIButton* pButton			= ( true == bRight_ ) ? UISet_.m_pButtonRightMemoDesc : UISet_.m_pButtonLeftMemoDesc;
	CKTDGUIStatic* pStatic			= ( true == bRight_ ) ? UISet_.m_pStaticRightMemoState : UISet_.m_pStaticLeftMemoState;
	const CX2Unit* pUnit			= g_pData->GetMyUser()->GetSelectUnit();
	const int iUnitClass			= static_cast<int>( pUnit->GetClass() );		/// unit +0xaf0 templet +4
	const int iUnitLevel			= static_cast<int>( pUnit->GetUnitData().m_Level );
	const int iSkillID				= ( NULL != UISet_.m_pSlot ) ? UISet_.m_pSlot->GetID() : 0;	/// slot +0x3c8

	const SSkillMemoTemplet* pMemo = CX2SkillNoteManager::GetInstance()->GetMemoByIndex( iSkillID, true == bRight_ ? 1 : 0, iUnitClass );
	if ( NULL == pMemo )
		return;

	riState = CX2SkillNoteManager::GetMemoState( iSkillID, pMemo->m_iType, iUnitLevel, iUnitClass );

	if ( NULL != pButton )
	{
		std::wstringstream wstrmGuide;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = ( NULL != g_pData->GetSkillTree() ) ?
			g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID ) ) : NULL;
		if ( NULL != pSkillTemplet )
		{
			/// name + L" " + the skill's name ( templet +0x14 ) + L"\n" + the effect text ( 3344ff0c / 3342f1b8 )
			wstring wstrText = CX2SkillNoteManager::GetMemoName( pMemo->m_iType );
			wstrText += L" ";
			wstrText += pSkillTemplet->m_wstrName;
			wstrText += L"\n";
			wstrText += CX2SkillNoteManager::GetInstance()->GetMemoEffectDesc( iSkillID, pMemo->m_iType, iUnitClass );
			if ( 0 == riState )
			{
				/// locked: L"\n\n" ( 3347e7ac ) + "Can be used from Level @1." ( 0x8b1f, "i": the row's level )
				wstrText += L"\n\n";
				wstrText += GET_REPLACED_STRING( ( 35615, "i", pMemo->m_iRequireLevel ) );
			}
			wstrmGuide << wstrText;
		}

		/// open or active, a transcendence memo ( 19 / 20 ): the skill it switches to ( value[ 1 ] for an upgraded skill, 32206d00 ) goes
		/// into the button's second dummy int, and hovering shows that skill ( 29076 / 29077 )
		if ( 0 != riState && 0x13 <= pMemo->m_iType && pMemo->m_iType <= 0x14 )
		{
			bool bUpgraded = false;
			if ( NULL != g_pData->GetSkillTree() )
			{
				const CX2SkillTree::SkillTemplet* pOwnTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID ) );
				bUpgraded = ( NULL != pOwnTemplet && 0 != pOwnTemplet->m_iDeleteSkillIDByUpgrade );
			}
			const int iIndex = ( true == bUpgraded ) ? 1 : 0;
			const int iAltSkillID = ( iIndex < static_cast<int>( pMemo->m_vecValue.size() ) ) ? static_cast<int>( pMemo->m_vecValue[ iIndex ] ) : 0;
			if ( 0 < iAltSkillID )
			{
				pButton->SetDummyInt( 1, iAltSkillID );
				pButton->SetCustomMsgMouseOver( STUCM_BUTTON_MEMO_SELECT_MOUSE_OVER );	/// +0x274 = 0x7194
				pButton->SetCustomMsgMouseOut( STUCM_BUTTON_MEMO_SELECT_MOUSE_OUT );	/// +0x270 = 0x7195
			}
		}

		pButton->SetGuideDesc( wstrmGuide.str().c_str() );		/// 31f61bd0
		pButton->SetShowEnable( true, true );
	}

	/// 328d9cc0( state, 1 ): the state's picture ( MEMO_LOCK / MEMO_OPEN / MEMO_SELECT )
	if ( NULL != pStatic && NULL != pStatic->GetPicture( riState ) )
		pStatic->GetPicture( riState )->SetShow( true );
}

/// June 328e9b30 ( bRight ): a click on a memo button ( 29065 / 29066 )
void CX2UISkillTreeNew::OpenMemoSelectPopup( SkillSlotUI& UISet_, const bool bRight_ )
{
	ShowSkillDesc( false );		/// 328e9160( 0, 0, 0, 0, 0, 1 )
	if ( NULL == g_pData || NULL == g_pData->GetSkillTree() || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;
	/// @328e9bc8: in a game, a busy unit is warned and nothing opens ( 323022b0 = CheckAndWarningBusyStateNow( true ) )
	if ( NULL != g_pX2Game && true == g_pX2Game->CheckAndWarningBusyStateNow( true ) )
		return;

	UpdateMemoPopup( UISet_, bRight_ );

	const int iUnitClass	= static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
	const int iSkillID		= ( NULL != UISet_.m_pSlot ) ? UISet_.m_pSlot->GetID() : 0;
	const int iBaseSkillID	= static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID ) ) );
	const SSkillMemoTemplet* pMemo = CX2SkillNoteManager::GetInstance()->GetMemoByIndex( iBaseSkillID, true == bRight_ ? 1 : 0, iUnitClass );
	if ( NULL == pMemo )
		return;

	/// @328e9d01: the previous box goes ( 31be4210( XGM_DELETE_DIALOG, +0xa0 ) )
	g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMemoMsgBox, NULL, false );
	m_pDLGMemoMsgBox = NULL;
	m_iMemoRemoveItemUID = 0;

	const int iState = ( true == bRight_ ) ? UISet_.m_iReserved20 : UISet_.m_iReserved14;
	switch ( iState )
	{
	case 0:		/// locked: "You haven't met the requirements to open the trait option." ( 0x8b20 )
		m_pDLGMemoMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999.f, -999.f ), GET_STRING( 35616 ), g_pMain->GetNowState(), -1, -1.f );
		break;

	case 1:		/// open: the checks ( 3235b070 ), then "Would you like to apply the @1 trait?" ( 0x8b21 ) with OK = 29067
		{
			if ( false == CanActivateMemo( iSkillID, pMemo->m_iType, iUnitClass ) )
				break;
			const wstring wstrMemoName = CX2SkillNoteManager::GetMemoName( pMemo->m_iType );
			if ( true == wstrMemoName.empty() )		/// 31cda210
				break;
			m_pDLGMemoMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999.f, -999.f ),
				GET_REPLACED_STRING( ( 35617, "L", wstrMemoName ) ), STUCM_SELECT_SKILL_ACTIVE_MEMO, g_pMain->GetNowState(), -1 );
			if ( NULL != m_pDLGMemoMsgBox )
			{
				m_pDLGMemoMsgBox->AddDummyInt( iBaseSkillID );		/// 31f8b860
				m_pDLGMemoMsgBox->AddDummyInt( pMemo->m_iType );
			}
		} break;

	case 2:		/// active
		{
			if ( false == CanDeactivateMemo( iSkillID ) )
			{
				/// @328ea0e3: after 3235b750's own box June opens a second one with a hard-coded Korean literal ( 334c3b50,
				/// "Unknown error." ), not kept in +0xa0 ( JUNE_BUGS )
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999.f, -999.f ), L"\xc54c \xc218 \xc5c6\xb294 \xc5d0\xb7ec \xc785\xb2c8\xb2e4.", g_pMain->GetNowState(), -1, -1.f );
				break;
			}
			const UidType iItemUID = FindMemoRemoveItemUID();
			if ( 0 < iItemUID )
			{
				const wstring wstrMemoName = CX2SkillNoteManager::GetMemoName( pMemo->m_iType );
				if ( true == wstrMemoName.empty() )
					break;
				/// "Would you like to deactivate the @1 trait? Consumes [Tome of Amnesia]." ( 0x8b22 ) with OK = 29068
				m_pDLGMemoMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999.f, -999.f ),
					GET_REPLACED_STRING( ( 35618, "L", wstrMemoName ) ), STUCM_SELECT_SKILL_DEACTIVE_MEMO, g_pMain->GetNowState(), -1 );
				if ( NULL != m_pDLGMemoMsgBox )
				{
					m_pDLGMemoMsgBox->AddDummyInt( iBaseSkillID );
					m_pDLGMemoMsgBox->AddDummyInt( pMemo->m_iType );
					m_iMemoRemoveItemUID = iItemUID;				/// 31ed7710 ( the box's int64 dummy )
				}
				break;
			}
			/// "[Tome of Amnesia] not found." ( 0x8b9a ), not kept in +0xa0
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999.f, -999.f ), GET_STRING( 35738 ), g_pMain->GetNowState(), -1, -1.f );
		} break;

	default:
		break;
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO popup skill=%d base=%d side=%s memo=%d state=%d", iSkillID, iBaseSkillID,
		true == bRight_ ? L"right" : L"left", pMemo->m_iType, iState );
}

/// UICustomEventProc's memo cases ( June 328eab00: ESI = lParam, the control; EBP = 0; 328ec399 returns false )
bool CX2UISkillTreeNew::MemoCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case STUCM_SELECT_SKILL_LEFT_MEMO:			/// 328ed02c
	case STUCM_SELECT_SKILL_RIGHT_MEMO:			/// 328ed259
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>( lParam );
			if ( NULL == pControl )
				return false;
			const int iSkillID = pControl->GetDummyInt( 0 );		/// 31c947d0( 0 )
			if ( iSkillID <= 0 )
				return false;

			/// the slot of the skill, else of its "[Enhanced]" skill ( 32205c00, -1 when none )
			map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.find( static_cast<CX2SkillTree::SKILL_ID>( iSkillID ) );
			if ( it == m_mapSkillSlotUISet.end() && NULL != g_pData && NULL != g_pData->GetSkillTree() )
				it = m_mapSkillSlotUISet.find( static_cast<CX2SkillTree::SKILL_ID>( g_pData->GetSkillTree()->GetUpgradedSkillID( iSkillID ) ) );
			if ( it != m_mapSkillSlotUISet.end() )
			{
				OpenMemoSelectPopup( it->second, STUCM_SELECT_SKILL_RIGHT_MEMO == wParam );
				return true;
			}
			MemoOKMsgBox( GET_STRING( 35614 ) );		/// 0x8b1e
		} return true;

	case STUCM_SELECT_SKILL_ACTIVE_MEMO:		/// 328ed483: the box's dummy ints ( 0 past the end ) -> 32453060, then the box goes; returns false
	case STUCM_SELECT_SKILL_DEACTIVE_MEMO:		/// 328ed519: the same with the removal item's UID -> 324530d0
		{
			int iBaseSkillID = 0;
			int iMemoID = 0;
			if ( NULL != m_pDLGMemoMsgBox && true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGMemoMsgBox ) )
			{
				iBaseSkillID	= m_pDLGMemoMsgBox->GetDummyInt( 0 );
				iMemoID			= m_pDLGMemoMsgBox->GetDummyInt( 1 );
			}
			if ( STUCM_SELECT_SKILL_ACTIVE_MEMO == wParam )
				CX2SkillNoteManager::GetInstance()->SendActiveMemoReq( iBaseSkillID, iMemoID );
			else
				CX2SkillNoteManager::GetInstance()->SendDeactiveMemoReq( iBaseSkillID, iMemoID, m_iMemoRemoveItemUID );

			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMemoMsgBox, NULL, false );
			m_pDLGMemoMsgBox = NULL;
			m_iMemoRemoveItemUID = 0;
		} return false;

	case STUCM_BUTTON_MEMO_SELECT_MOUSE_OVER:	/// 328ed5ca: the skill in the button's second dummy int, at the button, mode 2
		{
			CKTDGUIButton* pButton = reinterpret_cast<CKTDGUIButton*>( lParam );
			if ( NULL == pButton )
				return false;
			m_pHoveredMemoButton = pButton;
			const int iSkillID = pButton->GetDummyInt( 1 );
			if ( iSkillID <= 0 )
				return false;
			ShowSkillDesc( true, static_cast<CX2SkillTree::SKILL_ID>( iSkillID ), pButton->GetPos(), D3DXVECTOR2( 0.f, 0.f ), false, 2 );	/// vslot 0x70
		} return false;

	case STUCM_BUTTON_MEMO_SELECT_MOUSE_OUT:	/// 328ed64b: only for the hovered button
		{
			CKTDGUIButton* pButton = reinterpret_cast<CKTDGUIButton*>( lParam );
			if ( NULL == pButton || pButton != m_pHoveredMemoButton )
				return false;
			ShowSkillDesc( false );
			m_pHoveredMemoButton = NULL;
		} return false;

	default:
		break;
	}
	return false;
}

/// June 328df480 ( the grant handlers 323b0770 / 324631a0 / 32535be0 / 3220ee30, after 328924c0 ): with g_pData's skill tree, each
/// granted skill ( KSkillData's short id ) through 328ded60
void CX2UISkillTreeNew::ReplaceUpgradedSkillSlots( const std::vector<KSkillData>& vecSkillData_ )
{
	if ( NULL == g_pData || NULL == g_pData->GetSkillTree() )
		return;
	for ( std::vector<KSkillData>::const_iterator it = vecSkillData_.begin(); it != vecSkillData_.end(); ++it )
		ReplaceUpgradedSkillSlot( static_cast<int>( it->m_iSkillID ) );
}

/// June 328ded60 ( skill ): a granted "[Enhanced]" skill takes over its base's tree slot ( templet +0x178 ): the slot UI set moves to the
/// new key with the slot's id and slot data rewritten, the base's two memo buttons are re-pointed ( dummy int 0 set twice, JUNE_BUGS ),
/// the slot is redrawn, and an equipped base is swapped on the bar ( 32205dc0 = the slot-change queue ). Then the skill is marked new.
void CX2UISkillTreeNew::ReplaceUpgradedSkillSlot( const int iSkillID_ )
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
	if ( NULL == pSkillTemplet )
		return;

	const int iBaseSkillID = pSkillTemplet->m_iDeleteSkillIDByUpgrade;
	map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator itBase = m_mapSkillSlotUISet.find( static_cast<CX2SkillTree::SKILL_ID>( iBaseSkillID ) );
	if ( itBase != m_mapSkillSlotUISet.end() )
	{
		SkillSlotUI kUISet = itBase->second;
		m_mapSkillSlotUISet.erase( itBase );
		if ( NULL != kUISet.m_pSlot )
		{
			kUISet.m_pSlot->SetID( iSkillID_ );												/// +0x3c8
			CKTDGUISlotData* pSlotData = kUISet.m_pSlot->GetSlotData();						/// +0x3c4
			if ( NULL != pSlotData )
			{
				static_cast<CX2SkillTreeSlotData*>( pSlotData )->m_eSkillID = static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ );	/// +0xc
				pSlotData->m_bDataUpdated = true;											/// +4
			}
		}
		m_mapSkillSlotUISet.insert( std::make_pair( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ), kUISet ) );	/// 3265c6a0

		if ( NULL != m_pDLGUISkillTree )
		{
			const WCHAR* awszFormat[] = { L"g_pButton_LeftMemoDesc_%d", L"g_pButton_RightMemoDesc_%d" };
			for ( int i = 0; i < ARRAY_SIZE( awszFormat ); ++i )
			{
				WCHAR buf[256] = { 0, };
				StringCchPrintf( buf, 256, awszFormat[i], iBaseSkillID );
				CKTDGUIButton* pButton = static_cast<CKTDGUIButton*>( m_pDLGUISkillTree->GetControl( buf ) );
				if ( NULL != pButton && iBaseSkillID == pButton->GetDummyInt( 0 ) )
				{
					/// June writes index 0 twice ( @328deecc / @328deed6 ); index 1 keeps the base id ( JUNE_BUGS )
					pButton->SetDummyInt( 0, iSkillID_ );
					pButton->SetDummyInt( 0, iSkillID_ );
					m_pDLGUISkillTree->ChangeSequence( pButton, true );
				}
			}
		}

		UpdateSkillTree( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );		/// 328ddff0

		/// @328defa7: the base on bar A ( 32890070( base, 0 ) ), else on bar B ( +5 )
		if ( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() && NULL != m_pUserSkillTree )
		{
			int iSlotID = m_pUserSkillTree->GetSkillEquippedSlotIndex( static_cast<CX2SkillTree::SKILL_ID>( iBaseSkillID ), false );
			if ( -1 == iSlotID )
			{
				iSlotID = m_pUserSkillTree->GetSkillEquippedSlotIndex( static_cast<CX2SkillTree::SKILL_ID>( iBaseSkillID ), true );
				if ( -1 != iSlotID )
					iSlotID += 5;
			}
			if ( -1 != iSlotID )
				g_pData->GetSkillTree()->Handler_EGS_CHANGE_SKILL_SLOT_REQ( iSlotID, static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );	/// 32205dc0
		}

		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO slot %d -> %d ( the upgraded skill takes its base's tree slot )", iBaseSkillID, iSkillID_ );
	}

	/// @328defe9: the new-skill mark on a tree slot ( slot data type 1, +0x10 not set yet )
	map<CX2SkillTree::SKILL_ID, SkillSlotUI>::iterator it = m_mapSkillSlotUISet.find( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
	if ( it != m_mapSkillSlotUISet.end() && NULL != it->second.m_pSlot )
	{
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
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

/// S7d replaces this: March's page cases ( X2UISkillTreeNew.cpp SKILL_PAGE_SYSTEM block ) carried as they are, minus the learn-mode
/// decide paths June's switch does not have ( 29054, 29057, 29058 ). June's 29049 ( 328ec068 ) is the one page radio case.
bool CX2UISkillTreeNew::SkillPageCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
/// June's page cases of UICustomEventProc ( 328ec068 .. 328ec719 )
bool CX2UISkillTreeNew::PageCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case STUCM_SKILL_PAGE1:		/// 328ec068: every Skill_Page radio; the page is the radio's dummy int 0 ( 1 / 2 / 3 in the dialog )
		{
			CKTDGUIRadioButton* pRadio = reinterpret_cast<CKTDGUIRadioButton*>( lParam );
			if ( NULL == pRadio || NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
				return false;

			/// in a game, a busy unit ( 323022b0( 1 ) = CheckAndWarningBusyStateNow( true ), the warning is its own ) un-checks the
			/// clicked radio ( 31caa0e0( 0, 0, 0 ) ) and eats the click
			if ( NULL != g_pX2Game && true == g_pX2Game->CheckAndWarningBusyStateNow( true ) )
			{
				pRadio->SetCheckedInternal( false, false, false );
				return true;
			}
			/// in a game, a skill-slot change still cooling down ( 3221c560 ) above level 10 ( vslot 0x20c ) eats the click; the clicked
			/// radio stays checked ( JUNE_BUGS )
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
				 false == g_pX2Game->GetMyUnit()->IsValideSlotChange() && 10 < g_pX2Game->GetMyUnit()->GetUnitLevel() )
				return true;

			/// the select page ( +0x4b8 ) takes the radio's page ( 31c947d0( 0 ) ); June keeps the old one at UI +0x2c, which no tree
			/// function reads back ( not ported ). Then the tree ( 328ddf60 ) and the page UI ( 328e6970 ) are redrawn.
			CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree;
			refUserSkillTree.SetSelectPage( static_cast<USHORT>( pRadio->GetDummyInt( 0 ) ) );
			UpdateSkillTree();
			UpdateSkillPageUI();

			if ( true == g_pMain->IsWaitingServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK ) )
				return false;
			/// the page in use ( +0x4b4, 0-based ) is the selected one: nothing to send
			if ( refUserSkillTree.GetUsingPage() == refUserSkillTree.GetSelectPage() - 1 )
				return false;
			if ( NULL == g_pData->GetSkillTree() )
				return false;
			return g_pData->GetSkillTree()->Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( refUserSkillTree.GetSelectPage() );	/// 3220f160
		}

	case STUCM_ADD_SKILL_PAGE_ED:	/// 328ec1ef: June's dialog has no button that sends it; the body is March's
		{
			/// state 0xe or 0xf ( June's XS_GUILD_AGIT_GAME, none in March ), else the notice 0x72e2 ( 29410 )
			if ( CX2Main::XS_VILLAGE_MAP != g_pMain->GetNowStateID() )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29410 ), g_pMain->GetNowState() );
				return false;
			}
			if ( true == g_pMain->IsWaitingServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK ) )
				return false;
			g_pData->GetServerProtocol()->SendID( EGS_GET_NEXT_SKILL_PAGE_ED_REQ );		/// 0x476
			g_pMain->AddServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK );
		} return true;

	case STUCM_ADD_SKILL_PAGE_CASH:	/// 328ec3ff
		{
			if ( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
				return false;
			CX2StateField* pStateField = static_cast<CX2StateField*>( g_pMain->GetNowState() );
			if ( NULL == pStateField )
				return false;
			if ( NULL != g_pTFieldGame )
				g_pTFieldGame->CloseFieldName( 0.0f );									/// 323a9780( 0 )
			g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_PRIVATE_BANK );	/// cash shop +0x31c = 0x11
#ifdef SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM
			/// June then pushes CI_EXPAND_SKILL_PAGE ( 0x1f129 = 127273 ) into the cash shop's open-at-item list ( +0x338, 321cd390 );
			/// the shop's open path then shows that item in the buy pop-up ( 3224a5d0, ported in CX2CashShop::JuneOpenAtItemList )
			g_pData->GetCashShop()->PushOpenAtItemID( 127273 );
#else SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM
			/// June then pushes CI_EXPAND_SKILL_PAGE ( 0x1f129 = 127273 ) into the cash shop's June-only open-at-item list ( +0x338,
			/// setter 321cd390, six callers ): that list is not in March ( STATUS skill-system-core S7g ), so the shop opens as March's does
#endif SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM
			pStateField->ToggleCashShop();												/// vslot 0xac
		} return true;

	case STUCM_BUY_CONFIRM:		/// 328ec48e: = March's case
		{
			if ( NULL != m_pDLGBuyConfirm )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGBuyConfirm, NULL, false );
			m_pDLGBuyConfirm = NULL;
			if ( g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED >= m_iConfirmED )
				return g_pData->GetSkillTree()->Handler_EGS_EXPAND_SKILL_PAGE_REQ();	/// 322149e0
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28961 ), g_pMain->GetNowState() );
		} return false;

	case STUCM_BUY_CANCEL:		/// 328ec6b3: = March's case
		if ( NULL != m_pDLGBuyConfirm )
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGBuyConfirm, NULL, false );
		m_pDLGBuyConfirm = NULL;
		return true;

	default:
		break;
	}
	return false;
}
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE

#endif UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
/////////////////////////////////////////////////////////////////////////
// S7e: CX2SkillTree's half of the tree panels ( declared in X2SkillTree.h under the same flag ). Kept in this file, which
// X2UISkillTreeNew.cpp compiles, so the CP949 X2SkillTree.cpp only gains the one OpenScriptFile call.
/////////////////////////////////////////////////////////////////////////

/// June 328ee430
void CX2SkillTree::SkillCommandDesc::Reset()
{
	m_kTranscendental.m_iDescStringID = -1;
	m_kTranscendental.m_vecSkill.clear();
	m_kSkill.m_iDescStringID = -1;
	m_kSkill.m_vecSkill.clear();
	m_iCommandDescStringID = -1;
	m_vecMajorCombo.clear();
	m_iMinorDescStringID = -1;
	m_vecMinorCombo.clear();
}

namespace
{
	/// June 3220bdb0 @3220c6ff..3220c97b ( and the SKILL_DATA copy ): DESCRIPTION_STRING_ID ( -1 when absent ), then SKILL_ID read in
	/// pairs from index 1 until either value of a pair is missing
	void ReadDescGroup( KLuaManager& luaManager_, const char* pszTableName_, CX2SkillTree::DescGroup& kGroup_ )
	{
		if ( false == luaManager_.BeginTable( pszTableName_ ) )
			return;

		if ( false == luaManager_.GetValue( "DESCRIPTION_STRING_ID", kGroup_.m_iDescStringID ) )
			kGroup_.m_iDescStringID = -1;

		if ( true == luaManager_.BeginTable( "SKILL_ID" ) )
		{
			int iIndex = 1;
			int iSkillID = -1;
			int iStringID = -1;
			bool bOk = luaManager_.GetValue( iIndex, iSkillID );
			while ( true == bOk )
			{
				if ( false == luaManager_.GetValue( iIndex + 1, iStringID ) )
					break;
				kGroup_.m_vecSkill.push_back( std::make_pair( iSkillID, iStringID ) );		/// 3207b770
				iIndex += 2;
				bOk = luaManager_.GetValue( iIndex, iSkillID );
			}
			luaManager_.EndTable();
		}
		luaManager_.EndTable();
	}

	/// June 3220bdb0 @3220cbb5..3220ce46 ( and the _MINOR copy ): DESCRIPTION_STRING_ID, then MAJOR_COMBO rows { name, index, string }
	void ReadCommandGroup( KLuaManager& luaManager_, const char* pszTableName_, int& iDescStringID_,
		std::vector<CX2SkillTree::MajorCombo>& vecCombo_ )
	{
		if ( false == luaManager_.BeginTable( pszTableName_ ) )
			return;

		if ( false == luaManager_.GetValue( "DESCRIPTION_STRING_ID", iDescStringID_ ) )
			iDescStringID_ = -1;

		if ( true == luaManager_.BeginTable( "MAJOR_COMBO" ) )
		{
			for ( int i = 1; true == luaManager_.BeginTable( i ); ++i )
			{
				CX2SkillTree::MajorCombo kCombo;
				if ( false == luaManager_.GetValueUtf8( 1, kCombo.m_strComboName ) )	/// 32feb1b0, "" when absent
					kCombo.m_strComboName = "";
				if ( false == luaManager_.GetValue( 2, kCombo.m_iIndex ) )
					kCombo.m_iIndex = -1;
				if ( false == luaManager_.GetValue( 3, kCombo.m_iStringID ) )
					kCombo.m_iStringID = -1;
				vecCombo_.push_back( kCombo );		/// 322201a0
				luaManager_.EndTable();
			}
			luaManager_.EndTable();
		}
		luaManager_.EndTable();
	}
}

/// June 3220bdb0( class, bMinor, out ) ( 328e8e90 @328e911d ): the class table of SkillCommandDescriptionData.lua ( June data036,
/// June-only ) into the tree's +0x3c block. June reads it with its inline XORCRC kom loader; March uses its shipped loader
void CX2SkillTree::LoadSkillCommandDescription( int iUnitClass_, bool bMinor_, SkillCommandDesc* pDesc_ )
{
	if ( NULL == pDesc_ )
		return;

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	if ( false == g_pKTDXApp->LoadAndDoMemory( &luaManager, L"SkillCommandDescriptionData.lua" ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI SkillCommandDescriptionData.lua did not load: the major-skill / major-command panels stay empty; check data036 carries June's file" );
		return;
	}

	const std::string strClassName = GetUnitClassName( iUnitClass_ );
	if ( false == luaManager.BeginTable( strClassName.c_str() ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI SkillCommandDescriptionData.lua has no table for class %d ( %S )", iUnitClass_, strClassName.c_str() );
		return;
	}

	ReadDescGroup( luaManager, "TRANSCENDENTAL_SKILL_DATA", pDesc_->m_kTranscendental );
	ReadDescGroup( luaManager, "SKILL_DATA", pDesc_->m_kSkill );
	ReadCommandGroup( luaManager, "COMMAND_DATA", pDesc_->m_iCommandDescStringID, pDesc_->m_vecMajorCombo );
	if ( true == bMinor_ )
		ReadCommandGroup( luaManager, "COMMAND_DATA_MINOR", pDesc_->m_iMinorDescStringID, pDesc_->m_vecMinorCombo );
	luaManager.EndTable();

	CX2OfflineLog::Server( L"[JUNE_SKILL] UI SkillCommandDescriptionData.lua class %d ( %S ): skill %d ( %d row(s) ), transcendental %d ( %d ), command %d ( %d combo(s) ), minor %d ( %d )",
		iUnitClass_, strClassName.c_str(),
		pDesc_->m_kSkill.m_iDescStringID, static_cast<int>( pDesc_->m_kSkill.m_vecSkill.size() ),
		pDesc_->m_kTranscendental.m_iDescStringID, static_cast<int>( pDesc_->m_kTranscendental.m_vecSkill.size() ),
		pDesc_->m_iCommandDescStringID, static_cast<int>( pDesc_->m_vecMajorCombo.size() ),
		pDesc_->m_iMinorDescStringID, static_cast<int>( pDesc_->m_vecMinorCombo.size() ) );
}

/// June 3220d4d0( block, class name, bMinor, out ) ( 328e8820 @328e893c / @328e8993 ): for each MAJOR_COMBO row ( +0x2c, or +0x40
/// when bMinor ) whose combo table exists in the class table of ComboTree.lua: a row with that table's combo type, the
/// MAJOR_COMBO string and the icons of its INDEX_n row ( n = the row's index; ints from [2] )
void CX2SkillTree::LoadMajorComboRows( const SkillCommandDesc* pDesc_, const std::string& strClassName_, bool bMinor_,
	std::vector<ComboRow>& vecRow_ )
{
	if ( NULL == pDesc_ )
		return;

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	if ( false == g_pKTDXApp->LoadAndDoMemory( &luaManager, L"ComboTree.lua" ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI ComboTree.lua did not load: the major-command rows have no icons; check data036 carries June's file" );
		return;
	}
	if ( false == luaManager.BeginTable( strClassName_.c_str() ) )
		return;

	const std::vector<MajorCombo>& vecCombo = ( false == bMinor_ ) ? pDesc_->m_vecMajorCombo : pDesc_->m_vecMinorCombo;
	char szIndex[256] = { 0, };
	for ( UINT i = 0; i < vecCombo.size(); ++i )
	{
		if ( false == luaManager.BeginTable( vecCombo[i].m_strComboName.c_str() ) )
			continue;

		ComboRow kRow;
		kRow.m_iComboType = GetComboType( vecCombo[i].m_strComboName );
		kRow.m_iStringID = vecCombo[i].m_iStringID;
		StringCchPrintfA( szIndex, 255, "INDEX_%d", vecCombo[i].m_iIndex );
		if ( true == luaManager.BeginTable( szIndex ) )
		{
			int iIcon = 0;
			for ( int j = 2; true == luaManager.GetValue( j, iIcon ); ++j )
				kRow.m_vecIcon.push_back( iIcon );
			luaManager.EndTable();
		}
		vecRow_.push_back( kRow );		/// 32220630
		luaManager.EndTable();
	}
	luaManager.EndTable();
}

/// June 32205870
/*static*/ int CX2SkillTree::GetComboType( const std::string& strTableName_ )
{
	if ( "NORMAL_COMBO" == strTableName_ )
		return 0;
	if ( "DASH_COMBO" == strTableName_ )
		return 1;
	if ( "JUMP_COMBO" == strTableName_ )
		return 2;
	if ( "DASH_JUMP_COMBO" == strTableName_ )
		return 3;
	return 0;
}

/// June 32205610: the combo type's title string ( 35503 normal, 35505 dash, 35504 jump, 35506 dash jump )
/*static*/ int CX2SkillTree::GetComboTitleStringID( int iComboType_ )
{
	switch ( iComboType_ )
	{
	case 0:		return 0x8aaf;
	case 1:		return 0x8ab1;
	case 2:		return 0x8ab0;
	case 3:		return 0x8ab2;
	default:	return -1;
	}
}

/// June 32375e30 ( narrows 32375530's wide name ): the unit class's table name in June's skill tree data ( "" for an unknown class ).
/// Transcribed from June's switch by class number, Luciel / Rose / Ain included ( March has no UNIT_CLASS names for them )
/*static*/ std::string CX2SkillTree::GetUnitClassName( int iUnitClass_ )
{
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
	switch ( iUnitClass_ )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:	return "ELSWORD_SWORDMAN";
	case CX2Unit::UC_ARME_VIOLET_MAGE:	return "ARME_VIOLET_MAGE";
	case CX2Unit::UC_LIRE_ELVEN_RANGER:	return "LIRE_ELVEN_RANGER";
	case CX2Unit::UC_RAVEN_FIGHTER:	return "RAVEN_FIGHTER";
	case CX2Unit::UC_EVE_NASOD:	return "EVE_NASOD";
	case CX2Unit::UC_CHUNG_IRON_CANNON:	return "CHUNG_IRON_CANNON";
	case CX2Unit::UC_ARA_MARTIAL_ARTIST:	return "ARA_MARTIAL_ARTIST";
	case CX2Unit::UC_ELESIS_KNIGHT:	return "ELESIS_KNIGHT";
	case CX2Unit::UC_ADD_NASOD_RULER:	return "ADD_NASOD_RULER";
	case CX2Unit::UC_ELSWORD_KNIGHT:	return "ELSWORD_KNIGHT";
	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:	return "ELSWORD_MAGIC_KNIGHT";
	case CX2Unit::UC_LIRE_COMBAT_RANGER:	return "LIRE_COMBAT_RANGER";
	case CX2Unit::UC_LIRE_SNIPING_RANGER:	return "LIRE_SNIPING_RANGER";
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:	return "ARME_HIGH_MAGICIAN";
	case CX2Unit::UC_ARME_DARK_MAGICIAN:	return "ARME_DARK_MAGICIAN";
	case CX2Unit::UC_RAVEN_SOUL_TAKER:	return "RAVEN_SOUL_TAKER";
	case CX2Unit::UC_RAVEN_OVER_TAKER:	return "RAVEN_OVER_TAKER";
	case CX2Unit::UC_EVE_EXOTIC_GEAR:	return "EVE_EXOTIC_GEAR";
	case CX2Unit::UC_EVE_ARCHITECTURE:	return "EVE_ARCHITECTURE";
	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:	return "CHUNG_FURY_GUARDIAN";
	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:	return "CHUNG_SHOOTING_GUARDIAN";
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:	return "ELSWORD_SHEATH_KNIGHT";
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:	return "ARME_BATTLE_MAGICIAN";
	case CX2Unit::UC_LIRE_TRAPPING_RANGER:	return "LIRE_TRAPPING_RANGER";
	case CX2Unit::UC_RAVEN_WEAPON_TAKER:	return "RAVEN_WEAPON_TAKER";
	case CX2Unit::UC_EVE_ELECTRA:	return "EVE_ELECTRA";
	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:	return "CHUNG_SHELLING_GUARDIAN";
	case CX2Unit::UC_ARA_LITTLE_HSIEN:	return "ARA_LITTLE_HSIEN";
	case CX2Unit::UC_ELESIS_SABER_KNIGHT:	return "ELESIS_SABER_KNIGHT";
	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:	return "ELESIS_PYRO_KNIGHT";
	case CX2Unit::UC_ARA_LITTLE_DEVIL:	return "ARA_LITTLE_DEVIL";
	case CX2Unit::UC_ADD_PSYCHIC_TRACER:	return "ADD_PSYCHIC_TRACER";
	case CX2Unit::UC_ADD_ARC_TRACER:	return "ADD_ARC_TRACER";
	case CX2Unit::UC_ARA_LITTLE_SPECTER:	return "ARA_LITTLE_SPECTER";
	case CX2Unit::UC_ELESIS_DARK_KNIGHT:	return "ELESIS_DARK_KNIGHT";
	case CX2Unit::UC_ADD_TIME_TRACER:	return "ADD_TIME_TRACER";
	case CX2Unit::UC_LUCIEL_CHILIARCH:	return "LUCIEL_CHILIARCH";
	case CX2Unit::UC_LUCIEL_ROYAL_GUARD:	return "LUCIEL_ROYAL_GUARD";
	case CX2Unit::UC_LUCIEL_DIABLURA:	return "LUCIEL_DIABLURA";
	case CX2Unit::UC_ROSE_BLOODIA:	return "ROSE_BLOODIA";
	case CX2Unit::UC_ROSE_HEAVY_BARREL:	return "ROSE_HEAVY_BARREL";
	case CX2Unit::UC_ROSE_METAL_HEART:	return "ROSE_METAL_HEART";
	case CX2Unit::UC_ROSE_VALKYRIE:	return "ROSE_VALKYRIE";
	case CX2Unit::UC_AIN_LOFTY_EXACTER:	return "AIN_LOFTY_EXACTER";
	case CX2Unit::UC_AIN_LOFTY_ANPASSEN:	return "AIN_LOFTY_ANPASSEN";
	case CX2Unit::UC_AIN_LOFTY_WANDERER:	return "AIN_LOFTY_WANDERER";
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:	return "ELSWORD_LORD_KNIGHT";
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:	return "ELSWORD_RUNE_SLAYER";
	case CX2Unit::UC_LIRE_WIND_SNEAKER:	return "LIRE_WIND_SNEAKER";
	case CX2Unit::UC_LIRE_GRAND_ARCHER:	return "LIRE_GRAND_ARCHER";
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:	return "ARME_ELEMENTAL_MASTER";
	case CX2Unit::UC_ARME_VOID_PRINCESS:	return "ARME_VOID_PRINCESS";
	case CX2Unit::UC_RAVEN_BLADE_MASTER:	return "RAVEN_BLADE_MASTER";
	case CX2Unit::UC_RAVEN_RECKLESS_FIST:	return "RAVEN_RECKLESS_FIST";
	case CX2Unit::UC_EVE_CODE_NEMESIS:	return "EVE_CODE_NEMESIS";
	case CX2Unit::UC_EVE_CODE_EMPRESS:	return "EVE_CODE_EMPRESS";
	case CX2Unit::UC_CHUNG_IRON_PALADIN:	return "CHUNG_IRON_PALADIN";
	case CX2Unit::UC_CHUNG_DEADLY_CHASER:	return "CHUNG_DEADLY_CHASER";
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:	return "ELSWORD_INFINITY_SWORD";
	case CX2Unit::UC_ARME_DIMENSION_WITCH:	return "ARME_DIMENSION_WITCH";
	case CX2Unit::UC_LIRE_NIGHT_WATCHER:	return "LIRE_NIGHT_WATCHER";
	case CX2Unit::UC_RAVEN_VETERAN_COMMANDER:	return "RAVEN_VETERAN_COMMANDER";
	case CX2Unit::UC_EVE_BATTLE_SERAPH:	return "EVE_BATTLE_SERAPH";
	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:	return "CHUNG_TACTICAL_TROOPER";
	case CX2Unit::UC_ARA_SAKRA_DEVANAM:	return "ARA_SAKRA_DEVANAM";
	case CX2Unit::UC_ARA_YAMA_RAJA:	return "ARA_YAMA_RAJA";
	case CX2Unit::UC_ELESIS_GRAND_MASTER:	return "ELESIS_GRAND_MASTER";
	case CX2Unit::UC_ELESIS_BLAZING_HEART:	return "ELESIS_BLAZING_HEART";
	case CX2Unit::UC_ADD_LUNATIC_PSYKER:	return "ADD_LUNATIC_PSYKER";
	case CX2Unit::UC_ADD_MASTERMIND:	return "ADD_MASTERMIND";
	case CX2Unit::UC_ARA_ASURA:	return "ARA_ASURA";
	case CX2Unit::UC_ELESIS_CRIMSON_AVENGER:	return "ELESIS_CRIMSON_AVENGER";
	case CX2Unit::UC_ADD_DIABOLIC_ESPER:	return "ADD_DIABOLIC_ESPER";
	case CX2Unit::UC_LUCIEL_DREAD_LORD:	return "LUCIEL_DREAD_LORD";
	case CX2Unit::UC_LUCIEL_NOBLESSE:	return "LUCIEL_NOBLESSE";
	case CX2Unit::UC_LUCIEL_DEMONIO:	return "LUCIEL_DEMONIO";
	case CX2Unit::UC_ROSE_CRIMSON_ROSE:	return "ROSE_CRIMSON_ROSE";
	case CX2Unit::UC_ROSE_STORM_TROOPER:	return "ROSE_STORM_TROOPER";
	case CX2Unit::UC_ROSE_OPTIMUS:	return "ROSE_OPTIMUS";
	case CX2Unit::UC_ROSE_FREYJA:	return "ROSE_FREYJA";
	case CX2Unit::UC_AIN_ARME_THAUMATURGY:	return "AIN_ARME_THAUMATURGY";
	case CX2Unit::UC_AIN_ERBLUHEN_EMOTION:	return "AIN_ERBLUHEN_EMOTION";
	case CX2Unit::UC_AIN_APOSTASIA:	return "AIN_APOSTASIA";
	case CX2Unit::UC_LUCIEL_DUO:	return "LUCIEL_DUO";
	case CX2Unit::UC_ROSE_GUNNER:	return "ROSE_GUNNER";
	case CX2Unit::UC_AIN_ANGEL:	return "AIN_ANGEL";
	default:	return "";
	}
#else
	switch ( iUnitClass_ )
	{
	case 1:	return "ELSWORD_SWORDMAN";
	case 2:	return "ARME_VIOLET_MAGE";
	case 3:	return "LIRE_ELVEN_RANGER";
	case 4:	return "RAVEN_FIGHTER";
	case 5:	return "EVE_NASOD";
	case 6:	return "CHUNG_IRON_CANNON";
	case 7:	return "ARA_MARTIAL_ARTIST";
	case 8:	return "ELESIS_KNIGHT";
	case 9:	return "ADD_NASOD_RULER";
	case 10:	return "ELSWORD_KNIGHT";
	case 11:	return "ELSWORD_MAGIC_KNIGHT";
	case 12:	return "LIRE_COMBAT_RANGER";
	case 13:	return "LIRE_SNIPING_RANGER";
	case 14:	return "ARME_HIGH_MAGICIAN";
	case 15:	return "ARME_DARK_MAGICIAN";
	case 16:	return "RAVEN_SOUL_TAKER";
	case 17:	return "RAVEN_OVER_TAKER";
	case 18:	return "EVE_EXOTIC_GEAR";
	case 19:	return "EVE_ARCHITECTURE";
	case 20:	return "CHUNG_FURY_GUARDIAN";
	case 21:	return "CHUNG_SHOOTING_GUARDIAN";
	case 22:	return "ELSWORD_SHEATH_KNIGHT";
	case 23:	return "ARME_BATTLE_MAGICIAN";
	case 24:	return "LIRE_TRAPPING_RANGER";
	case 25:	return "RAVEN_WEAPON_TAKER";
	case 26:	return "EVE_ELECTRA";
	case 27:	return "CHUNG_SHELLING_GUARDIAN";
	case 28:	return "ARA_LITTLE_HSIEN";
	case 29:	return "ELESIS_SABER_KNIGHT";
	case 30:	return "ELESIS_PYRO_KNIGHT";
	case 31:	return "ARA_LITTLE_DEVIL";
	case 32:	return "ADD_PSYCHIC_TRACER";
	case 33:	return "ADD_ARC_TRACER";
	case 34:	return "ARA_LITTLE_SPECTER";
	case 35:	return "ELESIS_DARK_KNIGHT";
	case 36:	return "ADD_TIME_TRACER";
	case 37:	return "LUCIEL_CHILIARCH";
	case 38:	return "LUCIEL_ROYAL_GUARD";
	case 39:	return "LUCIEL_DIABLURA";
	case 40:	return "ROSE_BLOODIA";
	case 41:	return "ROSE_HEAVY_BARREL";
	case 42:	return "ROSE_METAL_HEART";
	case 43:	return "ROSE_VALKYRIE";
	case 44:	return "AIN_LOFTY_EXACTER";
	case 45:	return "AIN_LOFTY_ANPASSEN";
	case 46:	return "AIN_LOFTY_WANDERER";
	case 100:	return "ELSWORD_LORD_KNIGHT";
	case 101:	return "ELSWORD_RUNE_SLAYER";
	case 102:	return "LIRE_WIND_SNEAKER";
	case 103:	return "LIRE_GRAND_ARCHER";
	case 104:	return "ARME_ELEMENTAL_MASTER";
	case 105:	return "ARME_VOID_PRINCESS";
	case 106:	return "RAVEN_BLADE_MASTER";
	case 107:	return "RAVEN_RECKLESS_FIST";
	case 108:	return "EVE_CODE_NEMESIS";
	case 109:	return "EVE_CODE_EMPRESS";
	case 110:	return "CHUNG_IRON_PALADIN";
	case 111:	return "CHUNG_DEADLY_CHASER";
	case 112:	return "ELSWORD_INFINITY_SWORD";
	case 113:	return "ARME_DIMENSION_WITCH";
	case 114:	return "LIRE_NIGHT_WATCHER";
	case 115:	return "RAVEN_VETERAN_COMMANDER";
	case 116:	return "EVE_BATTLE_SERAPH";
	case 117:	return "CHUNG_TACTICAL_TROOPER";
	case 118:	return "ARA_SAKRA_DEVANAM";
	case 119:	return "ARA_YAMA_RAJA";
	case 120:	return "ELESIS_GRAND_MASTER";
	case 121:	return "ELESIS_BLAZING_HEART";
	case 122:	return "ADD_LUNATIC_PSYKER";
	case 123:	return "ADD_MASTERMIND";
	case 124:	return "ARA_ASURA";
	case 125:	return "ELESIS_CRIMSON_AVENGER";
	case 126:	return "ADD_DIABOLIC_ESPER";
	case 127:	return "LUCIEL_DREAD_LORD";
	case 128:	return "LUCIEL_NOBLESSE";
	case 129:	return "LUCIEL_DEMONIO";
	case 130:	return "ROSE_CRIMSON_ROSE";
	case 131:	return "ROSE_STORM_TROOPER";
	case 132:	return "ROSE_OPTIMUS";
	case 133:	return "ROSE_FREYJA";
	case 134:	return "AIN_ARME_THAUMATURGY";
	case 135:	return "AIN_ERBLUHEN_EMOTION";
	case 136:	return "AIN_APOSTASIA";
	case 200:	return "LUCIEL_DUO";
	case 201:	return "ROSE_GUNNER";
	case 202:	return "AIN_ANGEL";
	default:	return "";
	}
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
}

/// June 32207110( icon, out texture, out key ): the icon's m_IconTextureName / m_IconTetName ( only when the key is not empty )
bool CX2SkillTree::GetCommandIconTexture( int iIcon_, std::wstring& wstrTexture_, std::wstring& wstrKey_ ) const
{
	std::map< int, CommandIconData >::const_iterator mit = m_mapCommandIcon.find( iIcon_ );
	if ( mit == m_mapCommandIcon.end() || true == mit->second.m_wstrIconTetName.empty() )
		return false;

	wstrTexture_ = mit->second.m_wstrIconTextureName;
	wstrKey_ = mit->second.m_wstrIconTetName;
	return true;
}

/// June 32207190( icon, out texture, out key ): the icon's m_OutLineTextureName / m_OutLineTetName ( only when the key is not empty )
bool CX2SkillTree::GetCommandOutLineTexture( int iIcon_, std::wstring& wstrTexture_, std::wstring& wstrKey_ ) const
{
	std::map< int, CommandIconData >::const_iterator mit = m_mapCommandIcon.find( iIcon_ );
	if ( mit == m_mapCommandIcon.end() || true == mit->second.m_wstrOutLineTetName.empty() )
		return false;

	wstrTexture_ = mit->second.m_wstrOutLineTextureName;
	wstrKey_ = mit->second.m_wstrOutLineTetName;
	return true;
}

/// June 32207630( icon, x, y, static ): the icon picture at ( x, y + m_fOffsetY ), sized to its texture; with an outline the icon moves
/// 2 px right and the outline picture goes at ( x, y - 2 + m_fOffsetY )
void CX2SkillTree::AddCommandIconPicture( int iIcon_, float fX_, float fY_, CKTDGUIStatic* pStatic_ ) const
{
	if ( NULL == pStatic_ )
		return;

	std::wstring wstrTexture = L"";
	std::wstring wstrKey = L"";
	if ( false == GetCommandIconTexture( iIcon_, wstrTexture, wstrKey ) )
		return;

	std::map< int, CommandIconData >::const_iterator mit = m_mapCommandIcon.find( iIcon_ );
	const float fOffsetY = ( mit != m_mapCommandIcon.end() ) ? mit->second.m_fOffsetY : 0.f;

	CKTDGUIControl::CPictureData* pIcon = new CKTDGUIControl::CPictureData();
	pIcon->SetTex( wstrTexture.c_str(), wstrKey.c_str() );
	SetPointByTextureSize( pIcon );				/// 31c94970
	pIcon->SetPos( D3DXVECTOR2( fX_, fOffsetY + fY_ ) );
	pStatic_->AddPicture( pIcon );

	wstrTexture = L"";
	wstrKey = L"";
	if ( false == GetCommandOutLineTexture( iIcon_, wstrTexture, wstrKey ) )
		return;

	pIcon->SetPos( D3DXVECTOR2( pIcon->GetPos().x + 2.f, pIcon->GetPos().y ) );
	CKTDGUIControl::CPictureData* pOutLine = new CKTDGUIControl::CPictureData();
	pOutLine->SetTex( wstrTexture.c_str(), wstrKey.c_str() );
	SetPointByTextureSize( pOutLine );			/// 31c94970
	pOutLine->SetPos( D3DXVECTOR2( fX_, ( fY_ - 2.f ) + fOffsetY ) );
	pStatic_->AddPicture( pOutLine );
}

/// June 3220f8d0 ( bound as AddCommandIconData_LUA by 31d95080 @31e2c00a ): one CommandIconData.lua row. No m_ComboTreeIcon or an
/// empty m_IconTetName adds nothing; the first row of an icon stays ( map insert )
void CX2SkillTree::AddCommandIconData_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	int iIcon = -1;
	if ( false == luaManager.GetValue( "m_ComboTreeIcon", iIcon ) || -1 == iIcon )
		return;

	CommandIconData kData;
	if ( false == luaManager.GetValue( "m_IconTextureName", kData.m_wstrIconTextureName ) )
		kData.m_wstrIconTextureName = L"DLG_UI_Common_Texture_NewSkill_02.TGA";
	if ( false == luaManager.GetValue( "m_IconTetName", kData.m_wstrIconTetName ) )
		kData.m_wstrIconTetName = L"";
	if ( true == kData.m_wstrIconTetName.empty() )
		return;

	if ( false == luaManager.GetValue( "m_OutLineTextureName", kData.m_wstrOutLineTextureName ) )
		kData.m_wstrOutLineTextureName = L"DLG_UI_Common_Texture_NewSkill_02.TGA";
	if ( false == luaManager.GetValue( "m_OutLineTetName", kData.m_wstrOutLineTetName ) )
		kData.m_wstrOutLineTetName = L"";
	if ( false == luaManager.GetUserDefinedTypeValue( "m_ToolTipOutLineColor", kData.m_vToolTipOutLineColor ) )	/// 31c7e850
		kData.m_vToolTipOutLineColor = D3DXVECTOR3( 1.f, 1.f, 1.f );
	if ( false == luaManager.GetValue( "m_fOffsetY", kData.m_fOffsetY ) )
		kData.m_fOffsetY = 0.f;

	if ( true == luaManager.BeginTable( "m_ToolTipString" ) )
	{
		for ( int i = 1; true == luaManager.BeginTable( i ); ++i )
		{
			int iStringID = -1;
			if ( false == luaManager.GetValue( 1, iStringID ) )
				iStringID = -1;
			int iUnitType = 0;
			if ( false == luaManager.GetValue( 2, iUnitType ) )
				iUnitType = 0;
			kData.m_vecToolTipString.push_back( std::make_pair( iUnitType, iStringID ) );	/// 3207b4c0
			luaManager.EndTable();
		}
		luaManager.EndTable();
	}

	m_mapCommandIcon.insert( std::make_pair( iIcon, kData ) );
}

/// June 32207dd0( L"CommandIconData.lua" ) ( OpenScriptFile 322083e0 @322092f2, after the templets and CalcLevelUpIncreaseSkillPoint,
/// before the post-process 32206050 ); g_pSkillTree was declared at OpenScriptFile's start
void CX2SkillTree::OpenCommandIconData()
{
	if ( false == g_pKTDXApp->LoadLuaTinker( L"CommandIconData.lua" ) )
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI CommandIconData.lua did not load: the command tab has no icons; check data036 carries June's file" );
	else
		CX2OfflineLog::Server( L"[JUNE_SKILL] UI CommandIconData.lua loaded %d icon row(s)", static_cast<int>( m_mapCommandIcon.size() ) );
}
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS

#endif SERV_IRUHADEV_JUNE_SKILL_TREE_UI
