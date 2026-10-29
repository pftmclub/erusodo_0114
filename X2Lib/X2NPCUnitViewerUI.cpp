#include "StdAfx.h"
#include ".\x2npcunitviewerui.h"



CX2NPCUnitViewerUI::CX2NPCUnitViewerUI(void)
: m_pXSkinMesh( NULL )
, m_vLightPosition( 300.f, 300.f, -500.f )
{
	SetAlphaObject( false );

#ifdef SERV_PET_SYSTEM
	m_RenderParam.Init();
	m_fScale = 1.f;
	m_bForceBillBoard = false;

	m_pChangeTexXET = NULL;
#endif

}

CX2NPCUnitViewerUI::~CX2NPCUnitViewerUI(void)
{
	m_pXSkinAnim.reset();
	SAFE_CLOSE( m_pXSkinMesh );

#ifdef SERV_PET_SYSTEM
	SAFE_CLOSE( m_pChangeTexXET );
#endif
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June ~CX2NPCUnitViewerUI 32e52e60: frees m_vecJuneUIEffectSetHandle without stopping the sets in it (no 321dafb0, no 32e4fde0),
	// so a UI effect set played on this viewer keeps running on the UI effect-set manager after the viewer is gone. JUNE_BUGS 105.
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
}


bool CX2NPCUnitViewerUI::ResetNPC( CX2UnitManager::NPC_UNIT_ID eNPCID )
{
	const CX2UnitManager::NPCUnitTemplet* pNPCTemplet = g_pData->GetUnitManager()->GetNPCUnitTemplet( eNPCID );
	if( NULL == pNPCTemplet )
		return false;


	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	if( false == g_pKTDXApp->LoadLuaTinker( pNPCTemplet->m_LuaFileName.c_str() ) )
	{
		ASSERT( !"NO" );
		return false;
	}
	
	if( false == g_pKTDXApp->LoadAndDoMemory( &luaManager, pNPCTemplet->m_LuaFileName.c_str() ) )
	{
		ASSERT( !"NO" );
		return false;
	}


	std::wstring motionFileName = L"";
	if( true == luaManager.BeginTable( "INIT_MOTION" ) )
	{
		LUA_GET_VALUE( luaManager, "MOTION_FILE_NAME", motionFileName, L"" );
		luaManager.EndTable(); // INIT_MOTION
	}

		
	if( true == motionFileName.empty() )
		return false;


	CKTDXDeviceXSkinMesh* pNewXSkinMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motionFileName.c_str() );
	if( NULL == pNewXSkinMesh )
		return false;

	m_pXSkinAnim.reset();
	SAFE_CLOSE( m_pXSkinMesh );
	m_pXSkinMesh = pNewXSkinMesh;

	m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
	m_pXSkinAnim->SetAnimXSkinMesh( m_pXSkinMesh );
	m_pXSkinAnim->AddModelXSkinMesh( m_pXSkinMesh, m_pXSkinMesh->GetXET() );

	ResetRenderParam();

	return true;
}

#ifdef SERV_PET_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
bool CX2NPCUnitViewerUI::ResetPet( int petId, char iLv, char cVariant /*= 0*/ )
#else SERV_IRUHADEV_JUNE_PET_VIEWER_FX
bool CX2NPCUnitViewerUI::ResetPet( int petId, char iLv )
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
{
	CX2PetManager::PetTemplet *pTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)petId );
	if( pTemplet == NULL )
		return false;

#ifdef PET_PREVIEW
#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

	//벡터 범위 초과했을 경우에 대한 예외처리 추가(크래쉬 방지)
	if( static_cast<char>(pTemplet->m_Evolution_Step_InitLuaTemplet.size()) <= iLv &&
		false == pTemplet->m_Evolution_Step_InitLuaTemplet.empty())
		iLv = 0;

#else	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

	//벡터 범위 초과했을 경우에 대한 예외처리 추가(크래쉬 방지)
	if( static_cast<char>(pTemplet->m_Evolution_Step_ScriptName.size()) <= iLv &&
		false == pTemplet->m_Evolution_Step_ScriptName.empty())
		iLv = 0;

#endif	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

#endif //PET_PREVIEW

#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
	KLuaManager* pLuaManager = pTemplet->m_Evolution_Step_InitLuaTemplet[iLv].m_pLuaManager;
	if ( pLuaManager == NULL )
	{
		ASSERT( !"NO" );
		return false;
	}
	KLuaManager	luaManager( pLuaManager->GetLuaState(), 0, false );
#else	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	if( false == g_pKTDXApp->LoadLuaTinker( pTemplet->m_Evolution_Step_ScriptName[iLv].c_str() ) )
	{
		ASSERT( !"NO" );
		return false;
	}
	if( false == g_pKTDXApp->LoadAndDoMemory( &luaManager, pTemplet->m_Evolution_Step_ScriptName[iLv].c_str() ) )
	{
		ASSERT( !"NO" );
		return false;
	}
#endif	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

	m_RenderParam.Init();
	m_bForceBillBoard = false;

	if( true == luaManager.BeginTable( "INIT_SYSTEM" ) )
	{
		LUA_GET_VALUE( luaManager, "UNIT_SCALE", m_fScale, 1.f );		

		LUA_GET_VALUE_ENUM( luaManager, "RENDER_PARAM", m_RenderParam.renderType, CKTDGXRenderer::RENDER_TYPE, CKTDGXRenderer::RT_CARTOON_BLACK_EDGE );
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
		m_RenderParam.fOutLineWide	= CARTOON_OUTLINE_WIDTH;
#else //UNIT_SCALE_COMBINE_ONE
		m_RenderParam.fOutLineWide	= 1.7f;
#endif //UNIT_SCALE_COMBINE_ONE

		LUA_GET_VALUE( luaManager, "ALPHA_BLEND", m_RenderParam.bAlphaBlend, false );
		if( m_RenderParam.bAlphaBlend == true )
		{
			SetAlphaObject( true );
		}
		bool bCullMode;
		LUA_GET_VALUE( luaManager, "CULL_MODE", bCullMode, true );
		if( bCullMode == false )
		{
			m_RenderParam.cullMode = D3DCULL_NONE;
		}
		LUA_GET_VALUE( luaManager, "Z_ENABLE", m_RenderParam.bZEnable, true );
		m_RenderParam.bZWriteEnable = m_RenderParam.bZEnable;			// fix!! 일단 요렇게

		LUA_GET_VALUE( luaManager, "FORCE_BILLBOARD", m_bForceBillBoard, false );

		luaManager.EndTable(); // INIT_MOTION
	}
	
	std::wstring motionFileName = L"";
	std::wstring wstrChangeTex = L"";
	if( true == luaManager.BeginTable( "INIT_MOTION" ) )
	{
		LUA_GET_VALUE( luaManager, "MOTION_FILE_NAME", motionFileName, L"" );
		LUA_GET_VALUE( luaManager, "MOTION_CHANGE_TEX_XET", wstrChangeTex, L"" );		
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
		// June 32e50de0 @32e517f1-32e51a6f, still inside INIT_MOTION: a pet evolved to step 3 (iLv after the clamp above) with a
		// non-zero variant, whose templet has 4 steps (the +0x6c vector, m_Evolution_Step_ScriptName), swaps the last characters of
		// the motion file ("N.x" -> variant 1 / 2 / else 3) and of the XET ("N.xet" -> 326cc990's suffix, else 1 / 2 / 3 .xet).
		// Each swap needs the name to be longer than the part it cuts (3 and 5). No variant byte exists in March: cVariant is 0
		// from every March caller, so this cannot fire until backlog Stage 17.
		if( 3 == iLv && 0 != cVariant && 4 == static_cast<char>( pTemplet->m_Evolution_Step_ScriptName.size() ) )
		{
			const std::wstring wstrMotionBefore = motionFileName;
			const std::wstring wstrXetBefore = wstrChangeTex;

			if( motionFileName.length() > 3 )
			{
				std::wstring wstrMotion = motionFileName.substr( 0, motionFileName.length() - 3 );
				if( 1 == cVariant )
					wstrMotion += L"1.x";
				else if( 2 == cVariant )
					wstrMotion += L"2.x";
				else
					wstrMotion += L"3.x";
				motionFileName = wstrMotion;
			}

			if( wstrChangeTex.length() > 5 )
			{
				std::wstring wstrXet = wstrChangeTex.substr( 0, wstrChangeTex.length() - 5 );
				std::wstring wstrSuffix = L"";
				if( NULL != g_pData && NULL != g_pData->GetPetManager() )
					wstrSuffix = g_pData->GetPetManager()->GetVariantXetSuffix( petId, cVariant );
				if( true == wstrSuffix.empty() )
				{
					if( 1 == cVariant )
						wstrSuffix = L"1.xet";
					else if( 2 == cVariant )
						wstrSuffix = L"2.xet";
					else
						wstrSuffix = L"3.xet";
				}
				wstrChangeTex = wstrXet + wstrSuffix;
			}

			WCHAR wszFx[512] = {0};
			StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX PET_VIEWER variant pet=%d variant=%d motion=%s->%s xet=%s->%s",
				petId, static_cast<int>( cVariant ), wstrMotionBefore.c_str(), motionFileName.c_str(), wstrXetBefore.c_str(), wstrChangeTex.c_str() );
			ErrorLogMsg( XEM_ERROR142, wszFx );
		}
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
		luaManager.EndTable(); // INIT_MOTION
	}

	SAFE_CLOSE( m_pChangeTexXET );
	CKTDXDeviceXET* m_pChangeTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wstrChangeTex.c_str() );

	if( true == motionFileName.empty() )
		return false;


	CKTDXDeviceXSkinMesh* pNewXSkinMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motionFileName.c_str() );
	if( NULL == pNewXSkinMesh )
		return false;

	m_pXSkinAnim.reset();
	SAFE_CLOSE( m_pXSkinMesh );
	m_pXSkinMesh = pNewXSkinMesh;

	m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
	m_pXSkinAnim->SetAnimXSkinMesh( m_pXSkinMesh );
	m_pXSkinAnim->AddModelXSkinMesh( m_pXSkinMesh, m_pXSkinMesh->GetXET(), NULL, m_pChangeTexXET );

	ResetRenderParam();
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e50de0 @32e51cf2-32e51e73, after ResetRenderParam (32e4f770): stop group 1 (329594c0), then INIT_EFFECT.
	// UNIT_VIEWER_EFFECT_SET_LIST[1..99] plays into group 1 (32959640( skin, name, 1 )). A missing table plays nothing.
	ViewerFxStop( 1 );
	int iGroup1 = 0;
	if( true == luaManager.BeginTable( "INIT_EFFECT" ) )
	{
		if( true == luaManager.BeginTable( "UNIT_VIEWER_EFFECT_SET_LIST" ) )
		{
			std::wstring wstrSetName = L"";
			for( int iIndex = 1; iIndex < 100; ++iIndex )
			{
				if( false == luaManager.GetValue( iIndex, wstrSetName ) )
					break;
				ViewerFxPlay( m_pXSkinAnim, wstrSetName, 1 );
				++iGroup1;
			}
			luaManager.EndTable(); // UNIT_VIEWER_EFFECT_SET_LIST
		}
		luaManager.EndTable(); // INIT_EFFECT
	}
	if( 0 < iGroup1 )
	{
		WCHAR wszFx[256] = {0};
		StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX PET_VIEWER reset=pet id=%d group1=%d uiset=0", petId, iGroup1 );
		ErrorLogMsg( XEM_ERROR142, wszFx );
	}
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX

	return true;
}
#endif







/*virtual*/ HRESULT CX2NPCUnitViewerUI::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	if( m_pXSkinAnim != NULL )
	{
		if( true == IsManagedByDGManager() )
		{
			m_pXSkinAnim->GetMatrix().Move( GetMatrix().GetPos() );
		}

#ifdef SERV_PET_SYSTEM
		if( m_bForceBillBoard == true )
		{
			m_pXSkinAnim->SetBillBoardType( CKTDGMatrix::BT_ALL );
		}
		else
		{
			m_pXSkinAnim->GetMatrix().Rotate( GetMatrix().GetRotate() );
		}
#else
		m_pXSkinAnim->GetMatrix().Rotate( GetMatrix().GetRotate() );
#endif
		m_pXSkinAnim->GetMatrix().Scale( GetMatrix().GetScale() );

		m_pXSkinAnim->OnFrameMove( fTime, fElapsedTime );
	}
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e4f600 @32e4f75c ends with the base player update (329585f0 = CX2BaseUnitViewerUI::OnFrameMove), skin or not
	CX2BaseUnitViewerUI::OnFrameMove( fTime, fElapsedTime );
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX


	return S_OK;
}




void CX2NPCUnitViewerUI::ResetRenderParam()
{
	CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
	pRenderParam->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
	pRenderParam->outLineColor		= 0xffffffff;
	pRenderParam->lightPos			= m_vLightPosition;
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
	pRenderParam->fOutLineWide		= CARTOON_OUTLINE_WIDTH;
#else //UNIT_SCALE_COMBINE_ONE
	pRenderParam->fOutLineWide		= 1.5f;
#endif //UNIT_SCALE_COMBINE_ONE
	pRenderParam->bAlphaBlend		= false;

#ifdef SERV_PET_SYSTEM
	pRenderParam->renderType	= m_RenderParam.renderType;
	pRenderParam->fOutLineWide	= m_RenderParam.fOutLineWide;	
	pRenderParam->bAlphaBlend	= m_RenderParam.bAlphaBlend;
	pRenderParam->cullMode		= m_RenderParam.cullMode;
	pRenderParam->bZEnable		= m_RenderParam.bZEnable;
#endif
}



/*virtual*/ RENDER_HINT CX2NPCUnitViewerUI::OnFrameRender_Prepare()
{
	KTDXPROFILE();

	if ( m_pXSkinAnim == NULL )
		return RENDER_HINT_NORENDER;

	m_pXSkinAnim->SetInstantShow( false );
	if ( !m_pXSkinAnim->GetShowObject() )
		return RENDER_HINT_NORENDER;

	RENDER_HINT renderHintResult = m_pXSkinAnim->OnFrameRender_Prepare();

	if( RENDER_HINT::NORENDER != renderHintResult.m_eHint )
		m_pXSkinAnim->SetInstantShow( true );
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e4f820: on the shown path, after the skin's own prepare, the base prepare (32957260) runs and its hint is dropped;
	// the skin's hint is the one returned
	CX2BaseUnitViewerUI::OnFrameRender_Prepare();
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX

	return renderHintResult;
}//CX2UnitViewerUI::OnFrameRender_Prepare()



/*virtual*/ void CX2NPCUnitViewerUI::OnFrameRender_Draw()
{
	KTDXPROFILE();

	if( m_pXSkinAnim == NULL )
		return; 

	if( false == m_pXSkinAnim->GetShowObject() )
		return;

	if( false == m_pXSkinAnim->IsInstantShow() )
		return; 


	m_pXSkinAnim->OnFrameRender_Draw();
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e4f8f0 @32e4f939 tail-calls the base draw (32957330) with this viewer after the skin's own draw (the kickoff and funcmap
	// had it as "no base Draw"; the disassembly reloads ECX = this). March's base draw only draws particles of an unmanaged viewer.
	CX2BaseUnitViewerUI::OnFrameRender_Draw();
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
}


HRESULT CX2NPCUnitViewerUI::OnResetDevice()
{
	if( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->OnResetDevice();
	}

	return S_OK;
}

HRESULT CX2NPCUnitViewerUI::OnLostDevice()
{
	if( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->OnLostDevice();
	}

	return S_OK;
}



void CX2NPCUnitViewerUI::PlayAnim( const WCHAR* pName, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType /*= CKTDGXSkinAnim::XAP_LOOP*/, bool bTransition /*= false */ )
{
	if ( m_pXSkinAnim == NULL )
		return;

	if( NULL != pName )
	{
		m_pXSkinAnim->ChangeAnim( pName, bTransition );

	}
	else
	{
		m_pXSkinAnim->ChangeAnim( 0, bTransition );
	}
	m_pXSkinAnim->Play( playType );
}


void CX2NPCUnitViewerUI::SetPosition( const D3DXVECTOR3& vPosition )
{
	GetMatrix().Move( vPosition );
	if( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->GetMatrix().Move( GetMatrix().GetPos() );
	}
}


void CX2NPCUnitViewerUI::SetScale( const D3DXVECTOR3& vScale )
{	
	GetMatrix().Scale( vScale );
	if( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->GetMatrix().Scale( GetMatrix().GetScale() );
	}
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e4fcf0: every live UI effect set this viewer played also takes the scale, and no longer follows the damage-effect
	// scale (m_vScale = scale, m_bDamageEffectScale = false: June +0x70..+0x78 / +0x7c, ctor 321df230 order m_fPowerRateScale,
	// m_vScale, m_bDamageEffectScale; March probe 0x5c / 0x64 / 0x70). A stale handle answers NULL and is skipped.
	if( NULL != g_pData )
	{
		for( size_t i = 0; i < m_vecUIEffectSetHandle.size(); ++i )
		{
			CX2EffectSet::EffectSetInstance* pInstance = g_pData->GetUIEffectSet()->GetEffectSetInstance( m_vecUIEffectSetHandle[i] );
			if( NULL != pInstance )
			{
				pInstance->m_vScale = vScale;
				pInstance->m_bDamageEffectScale = false;
			}
		}
	}
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
}

#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
// June 32e4fe30: false without a skin anim, else the skin anim's has-animation lookup (31d34110 -> 31d39100, no insert on a miss)
bool CX2NPCUnitViewerUI::IsAnimExist( const WCHAR* pName )
{
	if( m_pXSkinAnim == NULL || NULL == pName )
		return false;

	return m_pXSkinAnim->IsAnimSetExist( pName );
}

// June 32e4fde0: with g_pData, each handle of the vector goes to the UI effect-set manager's stop (321dafb0, called as thiscall on
// g_pData +0x9c). June stops a COPY of the handle: the vector entry keeps its (now stale) value, and the vector is never erased
// (JUNE_BUGS 104). June's 321dafb0 queues the stop in the manager (+0x30, drained by DeleteEffect 321e3ee0); March has the
// immediate StopEffectSet( Handle& ), which earlier phases (C9c-1, C9d, C9f) already use for every 321dafb0 call.
void CX2NPCUnitViewerUI::StopUIEffectSets()
{
	if( NULL == g_pData )
		return;

	for( size_t i = 0; i < m_vecUIEffectSetHandle.size(); ++i )
	{
		CX2EffectSet::Handle hCopy = m_vecUIEffectSetHandle[i];
		g_pData->GetUIEffectSet()->StopEffectSet( hCopy );
	}
}
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
#ifdef RIDING_SYSTEM
bool CX2NPCUnitViewerUI::ResetRidingPet( int RidingPetId )
{
	CX2RidingPetManager::RidingPetTemplet *pTemplet = CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(RidingPetId) );
	if( pTemplet == NULL )
		return false;


//#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
	KLuaManager* pLuaManager = pTemplet->m_InitLuaTemplet.m_pLuaManager;
	if ( pLuaManager == NULL )
	{
		ASSERT( !"NO" );
		return false;
	}
	KLuaManager	luaManager( pLuaManager->GetLuaState(), 0, false );
//#endif	//X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

	m_RenderParam.Init();
	m_bForceBillBoard = false;

	if( true == luaManager.BeginTable( "INIT_SYSTEM" ) )
	{
		LUA_GET_VALUE( luaManager, "UNIT_SCALE", m_fScale, 1.f );		

		LUA_GET_VALUE_ENUM( luaManager, "RENDER_PARAM", m_RenderParam.renderType, CKTDGXRenderer::RENDER_TYPE, CKTDGXRenderer::RT_CARTOON_BLACK_EDGE );
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
		m_RenderParam.fOutLineWide	= CARTOON_OUTLINE_WIDTH;
#else //UNIT_SCALE_COMBINE_ONE
		m_RenderParam.fOutLineWide	= 1.7f;
#endif //UNIT_SCALE_COMBINE_ONE

		LUA_GET_VALUE( luaManager, "ALPHA_BLEND", m_RenderParam.bAlphaBlend, false );
		if( m_RenderParam.bAlphaBlend == true )
		{
			SetAlphaObject( true );
		}
		bool bCullMode;
		LUA_GET_VALUE( luaManager, "CULL_MODE", bCullMode, true );
		if( bCullMode == false )
		{
			m_RenderParam.cullMode = D3DCULL_NONE;
		}
		LUA_GET_VALUE( luaManager, "Z_ENABLE", m_RenderParam.bZEnable, true );
		m_RenderParam.bZWriteEnable = m_RenderParam.bZEnable;			// fix!! 일단 요렇게

		LUA_GET_VALUE( luaManager, "FORCE_BILLBOARD", m_bForceBillBoard, false );

		luaManager.EndTable(); // INIT_MOTION
	}

	std::wstring motionFileName = L"";
	std::wstring wstrChangeTex = L"";
	if( true == luaManager.BeginTable( "INIT_MOTION" ) )
	{
		LUA_GET_VALUE( luaManager, "MOTION_FILE_NAME", motionFileName, L"" );
		LUA_GET_VALUE( luaManager, "MOTION_CHANGE_TEX_XET", wstrChangeTex, L"" );		
		luaManager.EndTable(); // INIT_MOTION
	}

	SAFE_CLOSE( m_pChangeTexXET );
	CKTDXDeviceXET* m_pChangeTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wstrChangeTex.c_str() );

	if( true == motionFileName.empty() )
		return false;


	CKTDXDeviceXSkinMesh* pNewXSkinMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motionFileName.c_str() );
	if( NULL == pNewXSkinMesh )
		return false;

	m_pXSkinAnim.reset();
	SAFE_CLOSE( m_pXSkinMesh );
	m_pXSkinMesh = pNewXSkinMesh;

	m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
	m_pXSkinAnim->SetAnimXSkinMesh( m_pXSkinMesh );
	m_pXSkinAnim->AddModelXSkinMesh( m_pXSkinMesh, m_pXSkinMesh->GetXET(), NULL, m_pChangeTexXET );

	ResetRenderParam();
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	// June 32e51ea0 @32e52ad1-32e52ce0, after ResetRenderParam: there is NO group-1 stop in this function (JUNE_BUGS 106; the cash
	// shop stops group 1 before calling, the pet-info caller 328f74e0 does not). INIT_EFFECT.RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET[1..99]
	// is collected first, then RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST[1..99] plays into group 1, and once the tables are closed each
	// collected name plays on the UI effect set through the skin overload (321e28d0), its handle going into m_vecJuneUIEffectSetHandle.
	std::vector<std::wstring> vecUiSetName;
	int iGroup1 = 0;
	if( true == luaManager.BeginTable( "INIT_EFFECT" ) )
	{
		std::wstring wstrSetName = L"";
		if( true == luaManager.BeginTable( "RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET" ) )
		{
			for( int iIndex = 1; iIndex < 100; ++iIndex )
			{
				if( false == luaManager.GetValue( iIndex, wstrSetName ) )
					break;
				vecUiSetName.push_back( wstrSetName );
			}
			luaManager.EndTable(); // RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET
		}
		if( true == luaManager.BeginTable( "RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST" ) )
		{
			for( int iIndex = 1; iIndex < 100; ++iIndex )
			{
				if( false == luaManager.GetValue( iIndex, wstrSetName ) )
					break;
				ViewerFxPlay( m_pXSkinAnim, wstrSetName, 1 );
				++iGroup1;
			}
			luaManager.EndTable(); // RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST
		}
		luaManager.EndTable(); // INIT_EFFECT
	}
	int iUiSet = 0;
	if( NULL != g_pData )
	{
		for( size_t i = 0; i < vecUiSetName.size(); ++i )
		{
			CX2EffectSet::Handle hUiSet = g_pData->GetUIEffectSet()->PlayEffectSet( vecUiSetName[i].c_str(), m_pXSkinAnim );
			if( INVALID_EFFECTSET_HANDLE != hUiSet )
			{
				m_vecUIEffectSetHandle.push_back( hUiSet );
				++iUiSet;
			}
		}
	}
	if( 0 < iGroup1 || 0 < iUiSet )
	{
		WCHAR wszFx[256] = {0};
		StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX PET_VIEWER reset=riding id=%d group1=%d uiset=%d", RidingPetId, iGroup1, iUiSet );
		ErrorLogMsg( XEM_ERROR142, wszFx );
	}
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX

	return true;
}
#endif //RIDING_SYSTEM