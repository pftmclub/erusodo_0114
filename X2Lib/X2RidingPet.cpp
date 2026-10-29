#include "StdAfx.h"

#ifdef RIDING_SYSTEM
/**
	@file	X2RidingPet.cpp
	@author	JHKang
	@brief	아라 클래스 구현부 정의
	@since  2013-03-26
*/

#include "X2RidingPet.h"

#pragma region CX2RidingPet
/** @class : CX2RidingPet
	@brief : 생성자
	@date  : 2013/03/27
*/
CX2RidingPet::CX2RidingPet(void)
	: m_wstrMotionName( L"" ), m_wstrChangeTex( L"" ), m_wstrAniTex( L"" ), m_wstrRidingMotion( L"" )
	, m_pMotion( NULL ), m_pAniXET( NULL ), m_pChangeXET( NULL )
	, m_bInit( false ), m_bMine( false ), m_bLoaded( false ), m_bSetData( false ), m_bInitPos( false ), m_bForceColor( false )
	, m_bForceColorChange( false ), m_bUnderWaterHead( false ), m_bIgnoreLineSpeed( false ), m_bGame( false )
	, m_fScale( 1.f ), m_fElapsedTime( 0.f ), m_fNowStateTimer( 0.f ), m_cColor( 0.f, 0.f, 0.f, 0.f )
	, m_cLineUnitColor( 0.f, 0.f, 0.f, 0.f )
{
	m_SyncData.Init();
	m_ConditionData.Init();
	m_Info.Init();

	m_StateBiMap.clear();
	m_StateDataMap.clear();

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	/// June ctor 32d35fb0 / reset 32d282f0 (offsets in X2RidingPet.h). StateChange overwrites the stored keys on every state.
	m_bFirstStateChanged	= false;
	m_wstrAnimNameAtPlayEnd	= L"";
	m_fTargetRange			= 0.f;
	m_fLimitZDepth			= 0.f;
	m_fDegreeRange			= 0.f;
	m_fValuePos				= 0.f;
	m_bApplyAnimMove		= false;
	m_fOriginalScaleTime	= -1.f;
	m_fTransformScaleTime	= -1.f;
	m_iCheckSpaciarBuff		= -1;
	m_fOnGroundOverlapTime	= 0.f;
	m_iOnGroundState		= 0;
	m_bOnGroundChanged		= false;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
	m_bUiSetHidden = false;		/// June ctor 32d35fb0: +0x4ac = 0, both vectors empty, the map empty
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
	ClearState();
}
#pragma endregion 생성자

#pragma region ~CX2RidingPet
/** @class : ~CX2RidingPet
	@brief : 소멸자
	@date  : 2013/03/27
*/
CX2RidingPet::~CX2RidingPet(void)
{
	if ( true == m_bLoaded )
	{
		SAFE_CLOSE( m_pMotion );
	}
	SAFE_CLOSE( m_pAniXET );
	SAFE_CLOSE( m_pChangeXET );
}
#pragma endregion 소멸자

#pragma region OnFrameMove
/*virtual*/ HRESULT CX2RidingPet::OnFrameMove( IN double fTime_, IN float fElapsedTime_ )
{
	#pragma region 애니메이션 생성 및 로딩
	if ( false == m_bLoaded && NULL != m_pMotion )
	{
		const CKTDXDevice::EDeviceState eStateXMesh = m_pMotion->GetDeviceState();

		if ( CKTDXDevice::DEVICE_STATE_LOADED == eStateXMesh )
		{
			m_pXSkinAnimFuture = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnimFuture->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnimFuture->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnimFuture->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

			m_pXSkinAnimFuture->GetMatrix().Move( GetPos() );
			m_pXSkinAnimFuture->GetMatrix().Scale( D3DXVECTOR3( m_fScale, m_fScale, m_fScale ) );
			m_pXSkinAnimFuture->SetShowObject( true );

			SetCenter( m_pXSkinAnimFuture->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnimFuture->GetAnimXSkinMesh()->GetBoundingRadius() );

			m_pXSkinAnimFuture->OnFrameMove( g_NowTime, 0.f );

			m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

			SetShowObject( true );

			m_pXSkinAnim->GetMatrix().Move( GetPos() );
			m_pXSkinAnim->GetMatrix().Scale( D3DXVECTOR3( m_fScale, m_fScale, m_fScale ) );
			m_pXSkinAnim->SetShowObject( true );

			SetCenter( m_pXSkinAnim->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnim->GetAnimXSkinMesh()->GetBoundingRadius() );
			
			m_pXSkinAnim->OnFrameMove( g_NowTime, 0.f );

			m_bLoaded = true;
		}
	}
	#pragma endregion

	if ( NULL == m_pMotion || NULL == m_pXSkinAnim )
		return S_OK;

	if ( false == m_bLoaded )
		return S_OK;

#ifdef SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE
	/// A pet created without any StateChange ( the owner enters a village already riding, or the mount is swapped while riding: the offline path
	/// calls ProcessRidingOn( true ) and the square unit's state does not change ) never gets an anim, so it is invisible until the owner's next
	/// state change ( the first step ). Once it has loaded, the village pet takes its wait state at once. Not June's code ( June's server sends the
	/// riding state with the unit ); with a game the user's own riding state starts the pet.
	if ( NULL == g_pX2Game && NULL != g_pTFieldGame && false == m_bFirstStateChanged )
	{
		StateChange( m_WaitState );
#ifdef SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE_DEBUG
		{
			WCHAR wszFx[160] = {0};
			StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_ENTRY_STATE first state change from OnFrameMove: wait state=%d owner=%I64d",
				static_cast<int>( m_WaitState ), static_cast<__int64>( m_UserUid ) );
			ErrorLogMsg( XEM_ERROR142, wszFx );
		}
#endif SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE_DEBUG
	}
#endif SERV_IRUHADEV_JUNE_FIX_RIDINGPET_ENTRY_STATE
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	/// June 32d2ec80 @32d2f1c6: m_bInitPos is no longer set on every frame (March's line below); it is set on the first frame after the first
	/// StateChange, and that frame runs GetUserInfo / SetPos a second time after the skin anim's OnFrameMove (June @32d2f19f).
	bool bFirstInit = false;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
		
	#pragma region 위치 및 시간 갱신
	//if ( false == m_bInitPos )
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
		bool bIsRight = true;
		vPos.y += 30.f;

		GetUserInfo( vPos, bIsRight );
		SetPos( vPos, bIsRight );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
		if ( false == m_bInitPos && true == m_bFirstStateChanged )
		{
			m_bInitPos = true;
			bFirstInit = true;
		}
#else
		m_bInitPos = true;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	}

	m_fElapsedTime = fElapsedTime_;
	m_fNowStateTimer += fElapsedTime_;
	#pragma endregion

	#pragma region 위치 갱신
	if ( true == m_bLoaded && NULL != m_pMotion && NULL != m_pXSkinAnim && ( NULL != g_pX2Game || NULL != g_pTFieldGame ) )
	{
		GetMatrix().Move( m_SyncData.m_vPos );
		GetMatrix().RotateDegree( m_ConditionData.m_vDirDegree );

		if ( true == m_bForceBillBoard )
		{
			m_pXSkinAnim->SetBillBoardType( CKTDGMatrix::BT_ALL );
		}
		else
		{
			m_pXSkinAnim->GetMatrix().RotateDegree( m_ConditionData.m_vDirDegree );
		}

		m_pXSkinAnim->GetMatrix().Move( m_SyncData.m_vPos );
		m_pXSkinAnim->OnFrameMove( fTime_, fElapsedTime_ );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
		if ( true == bFirstInit )
		{
			D3DXVECTOR3 vUserPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
			bool bUserIsRight = true;
			vUserPos.y += 30.f;

			GetUserInfo( vUserPos, bUserIsRight );
			SetPos( vUserPos, bUserIsRight );
		}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	}
	else
	{
		SetShowObject( false );
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
        if ( m_pXSkinAnim != NULL )
        {
            m_pXSkinAnim->UpdateBeforeAnimationTime();
        }
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	}
	#pragma endregion
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	PlayAnimAtPlayEnd();	/// June @32d2f3b1
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	
	return S_OK;
}
#pragma endregion 매 프레임 처리할 내용

#pragma region OnFrameRender_Prepare
/*virtual*/ RENDER_HINT CX2RidingPet::OnFrameRender_Prepare()
{
	if ( false == m_bLoaded || NULL == m_pXSkinAnim )
		return RENDER_HINT_NORENDER;

	m_pXSkinAnim->SetInstantShow( false );
	if ( !m_pXSkinAnim->GetShowObject() )
		return RENDER_HINT_NORENDER;

	CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
	CKTDGXRenderer::RenderParam* pUserRenderParam = NULL;
	
	CX2World* pWorld = NULL;

	if ( NULL != g_pX2Game )
	{
		pWorld = g_pX2Game->GetWorld();

		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
		if ( NULL != pUser && NULL != pUser->GetSkinAnimPtr() && NULL != pUser->GetSkinAnimPtr()->GetRenderParam() )
		{
			pUserRenderParam = pUser->GetSkinAnimPtr()->GetRenderParam();
		}
	}
	else if ( NULL != g_pTFieldGame )
	{
		CX2SquareUnit* pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );

		if ( NULL != pUser )
		{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
			/// June 32d2c1e0 @32d2c4b2 (C9h-3a), the village branch: the square unit's flag (+0x518 = March's GetPlanRender: June returns no render when it is set,
			/// as the test below does) is synced into the hide mirror: when they differ every UI set is shown / hidden (32d291a0 returns false only without
			/// g_pData) and the mirror takes the flag. A game has no such resync (JUNE_BUGS 110).
			{
				const bool bPlanRender = pUser->GetPlanRender();
				if ( m_bUiSetHidden != bPlanRender )
				{
					if ( true == ShowHideAllUiSets( !bPlanRender ) )
						m_bUiSetHidden = bPlanRender;
				}
			}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
			if ( true == pUser->GetPlanRender() )
				return RENDER_HINT_NORENDER;
		}

		pWorld = g_pTFieldGame->GetWorld();
	}

	if ( NULL != pWorld )
		pRenderParam->lightPos = pWorld->GetLightPos();
#ifdef SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
	if ( NULL != pWorld )
	{
		/// June 32d2c416 (R0 map, rpfill row)
		pRenderParam->iWorldCartoonTexIndex		= pWorld->GetCartoonTextureIndex();
		pRenderParam->iWorldRimLightTexIndex	= pWorld->GetRimLightTextureIndex();
	}
#endif SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON

	pRenderParam->color = m_cColor;

	if ( false == m_bForceColor && ( true == m_bUnderWaterHead || true == m_bForceColorChange ) )
	{
		pRenderParam->color.r -= m_cLineUnitColor.r;
		pRenderParam->color.g -= m_cLineUnitColor.b;
		pRenderParam->color.b -= m_cLineUnitColor.b;
	}

	pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_BLACK_EDGE;

	if ( CX2GameOption::OL_MEDIUM == g_pMain->GetGameOption().GetOptionList().m_UnitDetail )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON;
	}
	else if( CX2GameOption::OL_LOW == g_pMain->GetGameOption().GetOptionList().m_UnitDetail )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
	}

	if ( 700.f < GetDistanceToCamera() )
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 1000.f * 1.f;
	}
	else
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 500.f * 1.f;
	}

	if ( NULL != g_pX2Game && NULL != pUserRenderParam )
	{
		pRenderParam->cartoonTexType	= pUserRenderParam->cartoonTexType;
		pRenderParam->renderType		= pUserRenderParam->renderType;
		pRenderParam->fOutLineWide		= pUserRenderParam->fOutLineWide;
		pRenderParam->bAlphaBlend		= pUserRenderParam->bAlphaBlend;
		pRenderParam->cullMode			= pUserRenderParam->cullMode;
		pRenderParam->bZEnable			= pUserRenderParam->bZEnable;
		pRenderParam->outLineColor		= pUserRenderParam->outLineColor;
		pRenderParam->color				= pUserRenderParam->color;
	}
	else
	{
		pRenderParam->cartoonTexType	= m_RenderParam.cartoonTexType;
		pRenderParam->renderType		= m_RenderParam.renderType;
		pRenderParam->fOutLineWide		= m_RenderParam.fOutLineWide;
		pRenderParam->bAlphaBlend		= m_RenderParam.bAlphaBlend;
		pRenderParam->cullMode			= m_RenderParam.cullMode;
		pRenderParam->bZEnable			= m_RenderParam.bZEnable;
	}

	RENDER_HINT renderHindResult = m_pXSkinAnim->OnFrameRender_Prepare();

	if ( renderHindResult.m_eHint != RENDER_HINT::NORENDER )
		m_pXSkinAnim->SetInstantShow( true );

	return renderHindResult;
}
#pragma endregion 매 프레임 렌더링 옵션 설정

#pragma region OnFrameRender_Draw
/*virtual*/ void CX2RidingPet::OnFrameRender_Draw()
{
	KTDXPROFILE();

	if( m_pXSkinAnim == NULL )
		return;

	if( m_pXSkinAnim != NULL && m_pXSkinAnim->GetShowObject() && m_pXSkinAnim->IsInstantShow() )
	{
		m_pXSkinAnim->OnFrameRender_Draw();
	}
}
#pragma endregion 매 프레임 Animation 그리기

#pragma region OnResetDevice
/*virtual*/ HRESULT CX2RidingPet::OnResetDevice()
{
	return S_OK;
}
#pragma endregion 장치 재설정

#pragma region OnLostDevice
/*virtual*/ HRESULT CX2RidingPet::OnLostDevice()
{
	return S_OK; 
}
#pragma endregion 장치 해제시

#ifdef X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
#pragma region static InitInit
/*static*/ void CX2RidingPet::InitInit( InitData& OutInit_, KLuaManager& luaManager_ )
{
	InitDevice( OutInit_.m_device, luaManager_ );
	InitMotion( OutInit_.m_device, luaManager_ );
}
#pragma endregion 

#pragma region InitData::AppendToDeviceList
void CX2RidingPet::InitData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	m_device.AppendToDeviceList( listInOut_ );
}
#pragma endregion 

#pragma region InitDeviceData::AppendToDeviceList
void CX2RidingPet::InitDeviceData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	// Texture
	BOOST_TEST_FOREACH( const wstring&, strTexture, m_vecstrReadyTexture )
	{
		g_pKTDXApp->GetDeviceManager()->AppendTexture( listInOut_, strTexture );
	}

	// skinMesh
	BOOST_TEST_FOREACH( const wstring&, strXSkinMesh, m_vecstrReadyXSkinMesh )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, strXSkinMesh );
	}

	// xmesh
	BOOST_TEST_FOREACH( const wstring&, strXMesh, m_vecstrReadyXMesh )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXMesh( listInOut_, strXMesh );
	}

	// xet
	BOOST_TEST_FOREACH( const wstring&, strXET, m_vecstrReadyXET )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, strXET );
	}
}
#pragma endregion 

#pragma region static InitDevice
/*static*/ void CX2RidingPet::InitDevice( IN InitDeviceData& OutDevice_, IN KLuaManager& luaManager_ )
{
	wstring wstrFileName;

	if( luaManager_.BeginTable( "INIT_DEVICE" ) == true )
	{
		OutDevice_.m_vecstrReadyTexture.resize( 0 );
		if( luaManager_.BeginTable( "READY_TEXTURE" ) == true )
		{
			int index = 1; 
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyTexture.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadyXMesh.resize( 0 );
		if( luaManager_.BeginTable( "READY_XMESH" ) == true )
		{
			int index = 1; 
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyXMesh.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadyXSkinMesh.resize( 0 );
		if( luaManager_.BeginTable( "READY_XSKIN_MESH" ) == true )
		{
			int index = 1; 
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyXSkinMesh.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadySound.resize( 0 );
		if( luaManager_.BeginTable( "READY_SOUND" ) == true )
		{
			int index = 1; 
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadySound.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}
		luaManager_.EndTable(); // INIT_DEVICE
	}
}
#pragma endregion 

#pragma region SetRidingPet
void CX2RidingPet::SetRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ )
{
	if ( NULL != pPetLuaState_ )
		m_LuaManager.AssignNewLuaState( pPetLuaState_, 0, false );
	else
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	m_UserUid = uUserUid_;

	if ( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit()->GetUID() == m_UserUid )
	{
		m_bMine = true;
	}

	InitSystem();
	InitDevice();
	InitMotion();
	InitPhysic();
	InitState();
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
	LoadInitEffect();	/// June 32d3cbf0 / 32d3cd10 call 32d2b0d0 right after 32d36600 (INIT_STATE) and before the motion open
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX

	if ( false == m_bLoaded && NULL == m_pXSkinAnim )
	{
		m_pMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMeshInBackground( m_wstrMotionName.c_str() );
	}

	m_bSetData = false;
}
#pragma endregion 

#pragma region SetGameRidingPet
void CX2RidingPet::SetGameRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ )
{
	m_bGame = true;

	if ( pPetLuaState_ != NULL )
		m_LuaManager.AssignNewLuaState( pPetLuaState_, 0, false );
	else
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	m_UserUid = uUserUid_;

	if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() && 
		 g_pData->GetMyUser()->GetSelectUnit()->GetUID() == m_UserUid )
	{
		m_bMine = true;
	}

	m_ConditionData.Init();
	m_SyncData.Init();

	InitSystem();
#ifdef READY_PET_DEVICES_IN_DUNGEON
	InitDevice();
#endif READY_PET_DEVICES_IN_DUNGEON
	InitMotion();
	InitPhysic();
	InitState();
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
	LoadInitEffect();	/// June 32d3cbf0 / 32d3cd10 call 32d2b0d0 right after 32d36600 (INIT_STATE) and before the motion open
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX

	if ( false == m_bLoaded && NULL == m_pXSkinAnim )
	{
		if ( NULL == m_pMotion && NULL == m_pXSkinAnim )
		{
			m_pMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_wstrMotionName.c_str() );
			ASSERT( m_pMotion );
			if ( NULL == m_pMotion )
			{
				StateLog( m_wstrMotionName.c_str() );
			}
		}

		if ( NULL != m_pMotion )
		{
			m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );
			m_pXSkinAnim->SetShowObject( false );

			SetCenter( m_pXSkinAnim->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnim->GetAnimXSkinMesh()->GetBoundingRadius() );
		}
	}

	m_bSetData = false;
}
#pragma endregion 던전 / 필드에서 탈 것 생성
#endif //X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

#pragma region SetInfo
void CX2RidingPet::SetInfo( IN const RidingPetInfo& ridingPetInfo_, IN RIDING_SPECIAL_MOVE eSpecialMove_ )
{
	m_Info = ridingPetInfo_;
	m_Info.m_eSpecialMove = eSpecialMove_;
	m_bSetData = true;
}
#pragma endregion 탈 것 정보 및 특수 이동기 값 설정

#pragma region InitDevice
void CX2RidingPet::InitDevice()
{
	wstring fileName = L"";

	if( m_LuaManager.BeginTable( "INIT_DEVICE" ) == true )
	{
		if( m_LuaManager.BeginTable( "READY_TEXTURE" ) == true )
		{
			wstring texName;
			int index = 1; 
			while( m_LuaManager.GetValue( index, texName ) == true )
			{
#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				TextureReadyInBackground( texName.c_str() );
#else	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				TextureReady( texName.c_str() );
#endif	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				index++;
			}
			m_LuaManager.EndTable();
		}

		if( m_LuaManager.BeginTable( "READY_XMESH" ) == true )
		{
			wstring xmeshName;
			int index = 1; 
			while( m_LuaManager.GetValue( index, xmeshName ) == true )
			{
#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				XMeshReadyInBackground( xmeshName.c_str() );
#else	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				XMeshReady( xmeshName.c_str() );
#endif	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				index++;
			}
			m_LuaManager.EndTable();
		}


		if( m_LuaManager.BeginTable( "READY_XSKIN_MESH" ) == true )
		{
			wstring xskinmeshName;
			int index = 1; 
			while( m_LuaManager.GetValue( index, xskinmeshName ) == true )
			{
#ifdef	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				XSkinMeshReadyInBackground( xskinmeshName.c_str() );
#else	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				XSkinMeshReady( xskinmeshName.c_str() );
#endif	X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
				index++;
			}
			m_LuaManager.EndTable();
		}

		if( m_LuaManager.BeginTable( "READY_SOUND" ) == true )
		{
			wstring soundName;
			int index = 1; 
			while( m_LuaManager.GetValue( index, soundName ) == true )
			{
#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD	
				SoundReadyInBackground( soundName.c_str() );
#else	X2OPTIMIZE_SOUND_BACKGROUND_LOAD	
				SoundReady( soundName.c_str() );
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD	
				index++;
			}
			m_LuaManager.EndTable();
		}

		m_LuaManager.EndTable(); // INIT_DEVICE
	}
}
#pragma endregion 모션, 텍스처, 사운드 파일 백그라운드 로딩 및 로딩

#pragma region InitSystem
void CX2RidingPet::InitSystem()
{
	m_RenderParam.Init();
	m_bForceBillBoard = false; 

	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		int iLayer = 0;
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_ConditionData.m_fUnitWidth, 300.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_ConditionData.m_fUnitHeight, 300.f );
		m_LuaManager.GetValue( "UNIT_LAYER", iLayer );
		SetLayer( iLayer );

		LUA_GET_VALUE( m_LuaManager, "UNIT_SCALE", m_fScale, 1.f );		

		LUA_GET_VALUE_ENUM( m_LuaManager, "RENDER_PARAM", m_RenderParam.renderType, CKTDGXRenderer::RENDER_TYPE, CKTDGXRenderer::RT_CARTOON_BLACK_EDGE );
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
		LUA_GET_VALUE( m_LuaManager, "OUT_LINE_WIDTH_SCALE", m_RenderParam.fOutLineWide, CARTOON_OUTLINE_WIDTH );
#else //UNIT_SCALE_COMBINE_ONE
		LUA_GET_VALUE( m_LuaManager, "OUT_LINE_WIDTH_SCALE", m_RenderParam.fOutLineWide, 1.7f );
#endif //UNIT_SCALE_COMBINE_ONE

		LUA_GET_VALUE( m_LuaManager, "ALPHA_BLEND", m_RenderParam.bAlphaBlend, false );
		if( m_RenderParam.bAlphaBlend == true )
		{
			SetAlphaObject( true );
		}

		bool bCullMode;
		LUA_GET_VALUE( m_LuaManager, "CULL_MODE", bCullMode, true );
		if( bCullMode == false )
		{
			m_RenderParam.cullMode = D3DCULL_NONE;
		}

		LUA_GET_VALUE( m_LuaManager, "Z_ENABLE", m_RenderParam.bZEnable, true );
		LUA_GET_VALUE( m_LuaManager, "Z_WRITE_ENABLE", m_RenderParam.bZWriteEnable, true );
		LUA_GET_VALUE( m_LuaManager, "FORCE_BILLBOARD", m_bForceBillBoard, false );

		CKTDGXRenderer::CARTOON_TEX_TYPE cartoonTexType = CKTDGXRenderer::CTT_NORMAL;
		LUA_GET_VALUE_ENUM( m_LuaManager, "CARTOON_TEX_TYPE", cartoonTexType, CKTDGXRenderer::CARTOON_TEX_TYPE, CKTDGXRenderer::CTT_NORMAL );
		m_RenderParam.cartoonTexType = cartoonTexType;

		D3DBLEND srcBlend = D3DBLEND_SRCALPHA;
		D3DBLEND destBlend = D3DBLEND_INVSRCALPHA;
		LUA_GET_VALUE_ENUM( m_LuaManager, "SRC_BLEND", srcBlend, D3DBLEND, D3DBLEND_SRCALPHA );
		LUA_GET_VALUE_ENUM( m_LuaManager, "DEST_BLEND", destBlend, D3DBLEND, D3DBLEND_INVSRCALPHA );
		m_RenderParam.srcBlend = srcBlend;
		m_RenderParam.destBlend = destBlend;

		LUA_GET_VALUE( m_LuaManager, "NOT_CHANGE_COLOR", m_bForceColor, false );
		float fColor = 255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_A", fColor, 255.f );
		m_cColor.a = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_R", fColor, 255.f );
		m_cColor.r = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_G", fColor, 255.f );
		m_cColor.g = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_B", fColor, 255.f );
		m_cColor.b = fColor/255.f;

		m_LuaManager.EndTable();
	}
}
#pragma endregion 탈 것 기본 정보 초기화

#pragma region InitPhysic
void CX2RidingPet::InitPhysic()
{
	if( m_LuaManager.BeginTable( "INIT_PHYSIC" ) == true )
	{
		m_LuaManager.GetValue( "RELOAD_ACCEL",		m_PhysicParam.fReloadAccel );
		m_LuaManager.GetValue( "G_ACCEL",			m_PhysicParam.fGAccel );
		m_LuaManager.GetValue( "MAX_G_SPEED",		m_PhysicParam.fMaxGSpeed );

		m_LuaManager.GetValue( "WALK_SPEED",		m_PhysicParam.fWalkSpeed );
		m_LuaManager.GetValue( "RUN_SPEED",			m_PhysicParam.fRunSpeed );
		m_LuaManager.GetValue( "JUMP_SPEED",		m_PhysicParam.fJumpSpeed );
		m_LuaManager.GetValue( "DASH_JUMP_SPEED",	m_PhysicParam.fDashJumpSpeed );

		m_LuaManager.EndTable();
	}
}
#pragma endregion 물리 수치 초기화

#pragma region InitState
void CX2RidingPet::InitState()
{
	if( m_LuaManager.BeginTable( "INIT_STATE" ) == true )
	{
		RidingPetStateData stateData;
		m_StateBiMap.clear();
		m_StateDataMap.clear();
		
		m_StateDataMap.insert( std::make_pair( RSI_NONE, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_NONE, "NONE") );

		m_StateDataMap.insert( std::make_pair( RSI_LOADING, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_LOADING, "LOADING") );

		m_StateDataMap.insert( std::make_pair( RSI_READY, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_READY, "READY") );

		m_StateDataMap.insert( std::make_pair( RSI_PLAY, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_PLAY, "PLAY") );

		m_StateDataMap.insert( std::make_pair( RSI_END, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_END, "END") );
		
		int iIndex = 1;
		while ( true == m_LuaManager.BeginTable( iIndex ) )
		{
			RidingPetStateData stateData;
			std::string strStateTableName;
			LUA_GET_VALUE_UTF8( m_LuaManager, "STATE_NAME", strStateTableName, "" );
			stateData.m_StateID = RSI_END + iIndex;

			m_StateDataMap.insert( std::make_pair( stateData.m_StateID, stateData ) );
			m_StateBiMap.insert( BiMapValue( stateData.m_StateID, strStateTableName ) );

			++iIndex;
			m_LuaManager.EndTable();
		}

		std::string strStateName = "";
		LUA_GET_VALUE_UTF8( m_LuaManager, "START_STATE", strStateName, "" );
		m_StartState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "WAIT_STATE", strStateName, "" );
		m_WaitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "HABIT_STATE", strStateName, "" );
		m_WaitHabitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "WALK_STATE", strStateName, "" );
		m_WalkState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_UP_STATE", strStateName, "" );
		m_JumpUpState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_DOWN_STATE", strStateName, "" );
		m_JumpDownState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_LANDING_STATE", strStateName, "" );
		m_JumpLandingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_STATE", strStateName, "" );
		m_DashState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_END_STATE", strStateName, "" );
		m_DashEndState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_UP_STATE", strStateName, "" );
		m_DashJumpUpState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_LANDING_STATE", strStateName, "" );
		m_DashJumpLandingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DAMAGE_FRONT_STATE", strStateName, "" );
		m_DamageFrontState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DAMAGE_BACK_STATE", strStateName, "" );
		m_DamageBackState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DYING_STATE_STATE", strStateName, "" );
		m_DyingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_Z_STATE", strStateName, "" );
		m_AttackZState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_ATTACK_Z_STATE", strStateName, "" );
		m_JumpAttackZState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_X_STATE", strStateName, "" );
		m_AttackXState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_SPECIAL_STATE", strStateName, "" );
		m_SpecialAttackState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "SIT_READY_STATE", strStateName, "" );
		m_SitReadyState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "SIT_WAIT_STATE", strStateName, "" );
		m_SitWaitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "STAND_UP_STATE", strStateName, "" );
		m_StandUpState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "SPECIAL_MOVE_STATE", strStateName, "" );
		m_SpecialMoveState = GetStateID( strStateName );

		m_LuaManager.EndTable();
	}

	for ( StateBiMap::left_const_iterator leftIter = m_StateBiMap.left.begin(); leftIter != m_StateBiMap.left.end(); ++leftIter )
	{
		m_LuaManager.MakeTableReference( leftIter->second.c_str(), leftIter->first );
	}
}
#pragma endregion 상태 테이블 초기화

#pragma region IsValidUser
bool CX2RidingPet::IsValidUser()
{
	void* pUser = NULL;

	if ( NULL != g_pX2Game )
	{
		pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );

		if ( false == static_cast<CX2GUUser*>( pUser )->IsLocalUnit() )
			return false;
	}
	else if ( NULL != g_pTFieldGame )
	{
		pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );
	}

	if ( NULL == pUser )
		return false;

	return true;
}
#pragma endregion 유저 확인

#pragma region StateChangeFutrue
void CX2RidingPet::StateChangeFuture( IN int iStateID_ )
{
	KTDXPROFILE();

	StateBiMap::left_const_iterator leftIter;
	leftIter = m_StateBiMap.left.find( iStateID_ );

	if ( leftIter != m_StateBiMap.left.end() )
	{
		string stateName = leftIter->second;
		StateChangeFuture( stateName.c_str() );
	}
}

void CX2RidingPet::StateChangeFuture( IN const char* stateName_ )
{
	// 	if ( false == IsValidUser() )
	// 		return;

	bool bTableOpen = false;
	std::string stateName = stateName_;
	StateBiMap::right_const_iterator rightIter = m_StateBiMap.right.find( stateName );

	if ( rightIter != m_StateBiMap.right.end() )
	{
		char cID = rightIter->second;

		m_StateData.m_StateID = m_SyncData.m_nowState = cID;
		++m_SyncData.m_StateChangeNum;

		bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
		{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			/// June 32d2c8b0 reads APPLY_RIDINGPET_ANIM_MOVE first (+0x5a1, miss 0); its user readers are Stage 17
			LUA_GET_VALUE( m_LuaManager, "APPLY_RIDINGPET_ANIM_MOVE", m_bApplyAnimMove, false );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			wstring animName;
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

			CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE ePlayType;
			LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", ePlayType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
			bool bTransition = true;
			LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			float fAnimStartTime = 0.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_START_TIME", fAnimStartTime, 0.f );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

			if( m_pXSkinAnimFuture != NULL )
			{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
				/// June 32d2c8b0: ChangeAnim( name, transition, start ) 31d33fd0
				m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), bTransition, fAnimStartTime );
#else
				if( bTransition == false )
					m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), false );
				else
					m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), true );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

				m_pXSkinAnimFuture->Play( ePlayType );
			}

			float fAnimSpeed = 1.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", fAnimSpeed, 1.f );
			if( m_pXSkinAnimFuture != NULL )
				m_pXSkinAnimFuture->SetPlaySpeed( fAnimSpeed );

			if ( NULL != g_pX2Game )
			{
				CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
				if ( NULL != pUser )
				{
					CX2GUUser::FrameData& kFutureFrameData = pUser->AccessFutureFrameData();
					//CX2GUUser::PhysicParam& pPhysicParam = pUser->GetPhysicParam();
					//std::vector<CX2GUUser::TIME_SPEED>& vecSpeedFactor = pUser->GetSpeedFatorVector();
					//std::vector<CX2GUUser::SoundPlayData>& vecSoundData = pUser->GetSoundPlayData();

					//if ( NULL != pFrameData )
					{
						LUA_GET_VALUE( m_LuaManager, "LAND_CONNECT", kFutureFrameData.stateParam.bLandConnect, true );
						m_ConditionData.m_bLandConnection = kFutureFrameData.stateParam.bLandConnect;
						bool bCanPushUnit = true;
						LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT", bCanPushUnit, true );
						kFutureFrameData.stateParam.bCanPushUnit = bCanPushUnit;
						bool bCanPassUnit = false;
						LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
						kFutureFrameData.stateParam.bCanPassUnit = bCanPassUnit;
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
						/// June 32d2c8b0 also reads the CAN_PUSH_UNIT_TIME%d / CAN_PASS_UNIT_TIME%d tables into the user (+0x8b14 / +0x8b24): C9h-4.
						/// March keeps its plain bool fields where June writes protected values.
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
#ifdef RAVEN_SECOND_CLASS_CHANGE
						/// June 32d2c8b0, after CAN_PASS_UNIT: 32d3e2f0 / 32d3e330 clear the user's Future vectors ( +0x8b14 / +0x8b24 ), then the tables are read
						/// 0-based until the first missing one: { time ( miss 0 ), can ( miss true for CAN_PUSH_UNIT_TIME, false for CAN_PASS_UNIT_TIME ) } ( C9h-4e ).
						/// A pet script with no table therefore empties the user's own vectors on every StateChangeFuture; June does the same.
						{
							std::vector<CX2GUUser::TIME_PUSHPASS>& vecPush = pUser->AccessCanPushUnitFuture();
							vecPush.clear();
							int iPushIndex = 0;
							while( m_LuaManager.BeginTable( "CAN_PUSH_UNIT_TIME", iPushIndex ) == true )
							{
								CX2GUUser::TIME_PUSHPASS canPushUnitTime;
								LUA_GET_VALUE( m_LuaManager, 1, canPushUnitTime.fTime, 0.0f );
								LUA_GET_VALUE( m_LuaManager, 2, canPushUnitTime.bCan, true );
								vecPush.push_back( canPushUnitTime );
								++iPushIndex;
								m_LuaManager.EndTable();
							}

							std::vector<CX2GUUser::TIME_PUSHPASS>& vecPass = pUser->AccessCanPassUnitFuture();
							vecPass.clear();
							int iPassIndex = 0;
							while( m_LuaManager.BeginTable( "CAN_PASS_UNIT_TIME", iPassIndex ) == true )
							{
								CX2GUUser::TIME_PUSHPASS canPushUnitTime;
								LUA_GET_VALUE( m_LuaManager, 1, canPushUnitTime.fTime, 0.0f );
								LUA_GET_VALUE( m_LuaManager, 2, canPushUnitTime.bCan, false );
								vecPass.push_back( canPushUnitTime );
								++iPassIndex;
								m_LuaManager.EndTable();
							}
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
							if ( iPushIndex > 0 || iPassIndex > 0 )
							{
								WCHAR wszFx[128] = {0};
								StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_USER_KEYS future state=%d can_push_time=%d can_pass_time=%d",
									static_cast<int>( m_StateData.m_StateID ), iPushIndex, iPassIndex );
								ErrorLogMsg( XEM_ERROR142, wszFx );
							}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
						}
#endif RAVEN_SECOND_CLASS_CHANGE
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					}
				}
			}

		}
		if( bTableOpen == true )
			m_LuaManager.EndTable();
	}
}
#pragma endregion 상태 변경_출력 프레임 전

#pragma region StateChange
void CX2RidingPet::StateChange( IN int iStateID_ )
{
	KTDXPROFILE();
	
	StateBiMap::left_const_iterator leftIter;
	leftIter = m_StateBiMap.left.find( iStateID_ );

	if ( leftIter != m_StateBiMap.left.end() )
	{
		string stateName = leftIter->second;
		StateChange( stateName.c_str() );
	}
}

void CX2RidingPet::StateChange( IN const char* stateName_ )
{
// 	if ( false == IsValidUser() )
// 		return;

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	m_bFirstStateChanged = true;	/// June 32d2f560 @32d2f5b3: before the name lookup, so a miss sets it too
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
	bool bTableOpen = false;
	std::string stateName = stateName_;
	StateBiMap::right_const_iterator rightIter = m_StateBiMap.right.find( stateName );
	
	if ( rightIter != m_StateBiMap.right.end() )
	{
		char cID = rightIter->second;

		m_StateData.m_StateID = m_SyncData.m_nowState = cID;
		++m_SyncData.m_StateChangeNum;
		
		bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
		{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			/// June 32d2f560, in June's order. TRANSFORM and the three keys after it belong to the transformer riding pets (backlog Stage 17):
			/// parsed here so the parser is June's, the consumers are left out (March has no transformer pet and no second state-id set).
			bool bTransform = false;
			LUA_GET_VALUE( m_LuaManager, "TRANSFORM", bTransform, false );	/// true -> June 32d281f0 (toggle +0x5a0): Stage 17, dropped here
			LUA_GET_VALUE( m_LuaManager, "APPLY_RIDINGPET_ANIM_MOVE", m_bApplyAnimMove, false );
			LUA_GET_VALUE( m_LuaManager, "ORIGINAL_SCALE_TIME", m_fOriginalScaleTime, -1.f );
			LUA_GET_VALUE( m_LuaManager, "TRANSFORM_SCALE_TIME", m_fTransformScaleTime, -1.f );
			LUA_GET_VALUE( m_LuaManager, "CHECK_RIDINGPET_SPACIAR_BUFF", m_iCheckSpaciarBuff, -1 );	/// > -1 -> June 32d2f3d0: Stage 17, not called here
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			wstring animName;
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			/// June 32d2f560: ONGROUND_CHANGE_STATE { OVERLAP_TIME (0), STATE_NAME } -> +0x5b8 / +0x5bc (name -> id 32d3e450 = GetStateID);
			/// absent -> 0 / 0. June also resets +0x5b0 / +0x5b4 to -1.0 here, but nothing reads them (a dead store), so March has no such members.
			if ( m_LuaManager.BeginTable( "ONGROUND_CHANGE_STATE" ) == true )
			{
				LUA_GET_VALUE( m_LuaManager, "OVERLAP_TIME", m_fOnGroundOverlapTime, 0.f );
				std::string strOnGroundStateName;
				LUA_GET_VALUE_UTF8( m_LuaManager, "STATE_NAME", strOnGroundStateName, "" );
				m_iOnGroundState = GetStateID( strOnGroundStateName );
				m_LuaManager.EndTable();
			}
			else
			{
				m_fOnGroundOverlapTime = 0.f;
				m_iOnGroundState = 0;
			}

			/// June: ANIM_NAME_AT_PLAY_END (+0x490, ""), played by CX2RidingPet::PlayAnimAtPlayEnd from OnFrameMove
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME_AT_PLAY_END", m_wstrAnimNameAtPlayEnd, L"" );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

			CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE ePlayType;
			LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", ePlayType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
			bool bTransition = true;
			LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			float fAnimStartTime = 0.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_START_TIME", fAnimStartTime, 0.f );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

			if( m_pXSkinAnim != NULL )
			{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
				/// June 32d2f560: ChangeAnim( name, transition, start ) 31d33fd0
				m_pXSkinAnim->ChangeAnim( animName.c_str(), bTransition, fAnimStartTime );
#else
				if( bTransition == false )
					m_pXSkinAnim->ChangeAnim( animName.c_str(), false );
				else
					m_pXSkinAnim->ChangeAnim( animName.c_str(), true );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

				m_pXSkinAnim->Play( ePlayType );
			}

			float fAnimSpeed = 1.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", fAnimSpeed, 1.f );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			/// June 32d2f560 @32d309e7..@32d30b87: only with a game (g_pX2Game) and the manager, ANIM_SPEED x the second float of the state's pair.
			/// Every float is 1.0 in June (no writer found), so this multiplies by 1.0; the village path gets the raw ANIM_SPEED.
			if ( NULL != g_pX2Game && NULL != CX2RidingPetManager::GetInstance() )
			{
				const int iNowState = m_StateData.m_StateID;
				const CX2RidingPetManager* pManager = CX2RidingPetManager::GetInstance();

				if ( iNowState == m_WalkState )
					fAnimSpeed *= pManager->GetAnimSpeedRate( 0 );
				if ( iNowState == m_DashState )
					fAnimSpeed *= pManager->GetAnimSpeedRate( 1 );
				if ( iNowState == m_JumpUpState || iNowState == m_JumpDownState )	/// June: 32d277f0 returns the jump down state
					fAnimSpeed *= pManager->GetAnimSpeedRate( 2 );
				if ( iNowState == m_DashJumpUpState )
					fAnimSpeed *= pManager->GetAnimSpeedRate( 3 );
				if ( iNowState == m_SpecialMoveState )
					fAnimSpeed *= pManager->GetAnimSpeedRate( 4 );
			}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			if( m_pXSkinAnim != NULL )
				m_pXSkinAnim->SetPlaySpeed( fAnimSpeed );

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
			/// June 32d2f560: stored only, defaults 1500 / 500 / 45 / 0. The reader is the user's target search 31fe2420 (C9h-4).
			LUA_GET_VALUE( m_LuaManager, "TARGET_RANGE", m_fTargetRange, 1500.f );
			LUA_GET_VALUE( m_LuaManager, "LIMIT_Z_DEPTH", m_fLimitZDepth, 500.f );
			LUA_GET_VALUE( m_LuaManager, "DEGREE_RANGE", m_fDegreeRange, 45.f );
			LUA_GET_VALUE( m_LuaManager, "VALUE_POS", m_fValuePos, 0.f );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
			/// June 32d2f560 @32d31013 (C9h-3a), before the game / user test: the reserved set delete blocks. June only QUEUES these stops (321dafb0), so a
			/// set stopped here still counts as playing for the tail below (JUNE_BUGS 111); March's StopEffectSet is immediate, so the stops are
			/// collected and run after the tail.
			std::vector< CX2EffectSet::Handle > vecPendingStop;
			DeleteReservedUiSets( vecPendingStop );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX

			if ( NULL != g_pX2Game )
			{
				CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
				if ( NULL != pUser )
				{
					CX2GUUser::FrameData& kNowFrameData = pUser->AccessNowFrameData();
					CX2GUUser::PhysicParam& kPhysicParam = pUser->AccessPhysicParam();
					std::vector<CX2GUUser::TIME_SPEED>& vecSpeedFactor = pUser->AccessSpeedFatorVector();
					std::vector<CX2GUUser::SoundPlayData>& vecSoundData = pUser->AccessSoundPlayData();
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					/// C9h-4e: what this StateChange reads from June's keys ( the log line at the end of the user block )
					float fGravityStart = 0.f;
					float fGravityEnd = 0.f;
					int iAllOption = 0;
					float fUpDown = 9999.f;
					bool bOffRiding = false;
					bool bSkillStateEnd = false;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS

					//if ( NULL != pFrameData )
					{
						bool bCanPushUnit = true;
						LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT", bCanPushUnit, true );
						kNowFrameData.stateParam.bCanPushUnit = bCanPushUnit;
						bool bCanPassUnit = false;
						LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
						kNowFrameData.stateParam.bCanPassUnit = bCanPassUnit;
					}

					D3DXVECTOR3 speed( 0.f, 0.f, 0.f );
					if( m_LuaManager.GetValue( "SPEED_X", speed.x ) == true )
						kPhysicParam.nowSpeed.x = speed.x;
					if( m_LuaManager.GetValue( "SPEED_Y", speed.y ) == true )
						kPhysicParam.nowSpeed.y = speed.y;

					LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_X", kPhysicParam.passiveSpeed.x, -1.f );
					LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_Y", kPhysicParam.passiveSpeed.y, -1.f );

					int index = 0;
					vecSpeedFactor.clear();
					while( m_LuaManager.BeginTable( "SPEED_TIME", index ) == true )
					{
						CX2GUUser::TIME_SPEED timeSpeed;

						LUA_GET_VALUE( m_LuaManager, 1, timeSpeed.vSpeed.x, 0.0f );		/// X축 속도
						LUA_GET_VALUE( m_LuaManager, 2, timeSpeed.vSpeed.y, 0.0f );		/// Y축 속도
						LUA_GET_VALUE( m_LuaManager, 3, timeSpeed.vSpeed.z, -1.0f );		/// 적용될 애니메이션 타임
						LUA_GET_VALUE( m_LuaManager, 4, timeSpeed.iFlag, 0 );			/// m_PhysicParam.nowSpeed, 1이면 +=, 2이면 = 연산을 수행함, 3이면 패시브 스피드 값 변경

						vecSpeedFactor.push_back( timeSpeed );
						++index;
						m_LuaManager.EndTable();
					}

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					/// June 32d2f560 @32d31b1b-@32d31f16, after SPEED_TIME and before INVINCIBLE: both floats are read first ( miss 0 ), then written to the user
					/// ( +0x84f8 / +0x84fc = m_vDisableGravityInScriptTime ), every state change ( C9h-4e ).
					LUA_GET_VALUE( m_LuaManager, "DISABLE_GRAVITY_TIME_START", fGravityStart, 0.f );
					LUA_GET_VALUE( m_LuaManager, "DISABLE_GRAVITY_TIME_END", fGravityEnd, 0.f );
					pUser->SetDisableGravityInScriptTime( fGravityStart, fGravityEnd );

#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					if( m_LuaManager.BeginTable( "INVINCIBLE" ) == true )
					{
						float fInvin = -1.0f;
						LUA_GET_VALUE( m_LuaManager, 1,	fInvin,	-1.0f );
						kNowFrameData.stateParam.invincibleTime.m_fX = fInvin;
						fInvin = -1.0f;
						LUA_GET_VALUE( m_LuaManager, 2,	fInvin,	-1.0f );
						kNowFrameData.stateParam.invincibleTime.m_fY = fInvin;

						if( kNowFrameData.stateParam.invincibleTime.m_fX <= 0.0f && kNowFrameData.stateParam.invincibleTime.m_fY > 0.0f )
							kNowFrameData.stateParam.bInvincible = true;
						else
							kNowFrameData.stateParam.bInvincible = false;

						m_LuaManager.EndTable();
					}
					else
					{
						kNowFrameData.stateParam.invincibleTime.m_fX	= -1.0f;
						kNowFrameData.stateParam.invincibleTime.m_fY	= -1.0f;
						kNowFrameData.stateParam.bInvincible		= false;
					}

					bool bSuperArmor = false;
					LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR",			bSuperArmor,			false );
					kNowFrameData.stateParam.bSuperArmor			= bSuperArmor;

					bool bSuperArmorNotRed = true;
					LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR_NOT_RED", bSuperArmorNotRed,		true );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
					/// June 32d2f560 @32d3262a: the key is always read, but written to the user only when SUPER_ARMOR is true
					if ( true == bSuperArmor )
						kNowFrameData.stateParam.bSuperArmorNotRed		= bSuperArmorNotRed;
#else
					kNowFrameData.stateParam.bSuperArmorNotRed		= bSuperArmorNotRed;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

					pUser->SetStateSuperArmor( bSuperArmor );

				#ifdef SUPER_ARMOR_TIME
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
                    std::vector<D3DXVECTOR2>& vecSuperArmorTime = pUser->AccessVecNowSuperArmorTime();
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
					std::vector<D3DXVECTOR2>& vecSuperArmorTime = kNowFrameData.stateParam.m_vecSuperArmorTime;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
                    vecSuperArmorTime.clear();

					int indexTimeTable = 0;
					while( m_LuaManager.BeginTable( "SUPER_ARMOR_TIME", indexTimeTable ) == true )
					{
						D3DXVECTOR2 superArmorTime;
						LUA_GET_VALUE( m_LuaManager, 1, superArmorTime.x, 0.f );
						LUA_GET_VALUE( m_LuaManager, 2, superArmorTime.y, 0.f );
						vecSuperArmorTime.push_back(superArmorTime);

						indexTimeTable++;
						m_LuaManager.EndTable();
					}
				#endif

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					/// June 32d2f560 @32d326fb-@32d33049 ( C9h-4e ): clear the user's list, SOUND_PLAY<n> through June's nation reader ( 32078fd0, the 4 fields as
					/// March ), then SOUND_PLAY_ALL_OPTION<n> ( the keys and defaults of C9h-4a's CommonStateStart block ). SOUND_PLAY_EX / STATE_SOUND_PLAY are not read
					/// here. June's player ( C9h-4a ) erases each record once it has played.
					index = 0;
					vecSoundData.resize(0);
					while( CX2GameUnit::BeginSoundPlayTable( m_LuaManager, "SOUND_PLAY", index ) == true )
					{
						CX2GUUser::SoundPlayData sndPlayData;

						LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,		-1.0f	);
						LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,		L""		);
						LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,		100		);
						LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,		false	);

						vecSoundData.push_back( sndPlayData );

						index++;
						m_LuaManager.EndTable();
					}
					iAllOption = CX2GameUnit::LoadSoundPlayAllOption( m_LuaManager, vecSoundData );
#else
					index = 0;
					vecSoundData.resize(0);
					while( m_LuaManager.BeginTable( "SOUND_PLAY", index ) == true )
					{
						CX2GUUser::SoundPlayData sndPlayData;

						LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,		-1.0f	);
						LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,		L""		);
						LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,		100		);
						LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,		false	);

						vecSoundData.push_back( sndPlayData );

						index++;
						m_LuaManager.EndTable();
					}

#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					if( pUser->IsFocusUnit() == true && kNowFrameData.stateParam.bResetCamera == true )
					{
						kNowFrameData.stateParam.hitCamera = 0;
						if( g_pX2Game->GetX2Camera()->GetCameraState() != CX2Camera::CS_NORMAL )
						{
							g_pX2Game->GetX2Camera()->NomalDirectCamera( pUser, g_pMain->GetGameOption().GetCameraDistance() );
						}

						int camera0Rate = 100;
						int camera1Rate = 100;
						int camera2Rate = 100;
						int ran = rand()%100 + 1;

						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA0_RATE", camera0Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA1_RATE", camera1Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA2_RATE", camera2Rate, 0 );

						if( ran < camera0Rate )
						{
							kNowFrameData.stateParam.normalCamera = 0;
						}
						else if( ran < (camera0Rate + camera1Rate) )
						{
							kNowFrameData.stateParam.normalCamera = 1;
						}
						else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
						{
							kNowFrameData.stateParam.normalCamera = 2;
						}
						else
							kNowFrameData.stateParam.normalCamera = 0;

						ran = rand()%100 + 1;
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA0_RATE", camera0Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA1_RATE", camera1Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA2_RATE", camera2Rate, 0 );
						if( ran < camera0Rate )
						{
							kNowFrameData.stateParam.hitCamera = 0;
						}
						else if( ran < (camera0Rate + camera1Rate) )
						{
							kNowFrameData.stateParam.hitCamera = 1;
						}
						else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
						{
							kNowFrameData.stateParam.hitCamera = 2;
						}
						else
							kNowFrameData.stateParam.hitCamera = kNowFrameData.stateParam.normalCamera;
					}

					float fValue = 0.f;
					LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetSkillCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "BWALK_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetBWalkCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "WALK_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetWalkCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "DASH_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetDashCancelAfter( fValue );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					/// June 32d2f560 @32d3424b-@32d343ac: UPDOWN_CANCEL_AFTER ( miss 9999.0 ) into the user ( +0x8ca4 ), after DASH and before LoadEffectSetFromScript.
					/// June's CommonStateStart clamp ( C9h-4c ) is not applied here: the riding pet's StateChange has no call to it ( C9h-4e ).
					LUA_GET_VALUE( m_LuaManager, "UPDOWN_CANCEL_AFTER", fUpDown, 9999.f );
					pUser->SetUpDownCancelAfter( fUpDown );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS

					pUser->LoadEffectSetFromScript( m_LuaManager );

					bool bDeleteEffect = false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_STATE_END", bDeleteEffect, false );
					pUser->SetDeleteEffectSetOnStateEnd( bDeleteEffect );
					bDeleteEffect= false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DAMAGE_REACT", bDeleteEffect, false );
					pUser->SetDeleteEffectSetOnDamageReact( bDeleteEffect );
					bDeleteEffect= false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DIE", bDeleteEffect,	false );
					pUser->SetDeleteEffectSetOnDie( bDeleteEffect );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
					/// June 32d2f560 @32d34b55-@32d35023: the two bytes C9h-4d added ( +0x8628 / +0x862b ), after _ON_DIE, miss false. LoadEffectSetFromScript's tail ( 4d ) has
					/// already read both keys from this same table, so these two writes repeat its values; June reads them twice too ( C9h-4e ).
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_OFF_RIDING", bOffRiding, false );
					pUser->SetDeleteEffectSetOffRiding( bOffRiding );
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_SKILL_STATE_END", bSkillStateEnd, false );
					pUser->SetDeleteEffectSetOnSkillStateEnd( bSkillStateEnd );
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
					if ( 0.f != fGravityStart || 0.f != fGravityEnd || iAllOption > 0 || 9999.f != fUpDown || true == bOffRiding || true == bSkillStateEnd )
					{
						WCHAR wszFx[256] = {0};
						StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_USER_KEYS user state=%d(%S) gravity=%g/%g sound_records=%d all_option=%d updown=%g off_riding=%d skill_state_end=%d",
							static_cast<int>( m_StateData.m_StateID ), stateName.c_str(), fGravityStart, fGravityEnd, static_cast<int>( vecSoundData.size() ), iAllOption,
							fUpDown, bOffRiding ? 1 : 0, bSkillStateEnd ? 1 : 0 );
						ErrorLogMsg( XEM_ERROR142, wszFx );
					}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
					/// June 32d2f560 @32d35029: PASSIVE_SPEED_X x the first float of manager pair 4 when the state is SPECIAL_MOVE_STATE (1.0 in June).
					/// June's block between this point and its start reads the user-side June keys that belong to C9h-4, not ported here:
					/// DISABLE_GRAVITY_TIME_START / _END, UPDOWN_CANCEL_AFTER, SOUND_PLAY_ALL_OPTION and June's SOUND_PLAY reader (32078fd0),
					/// DELETE_EFFECT_SET_OFF_RIDING / _ON_SKILL_STATE_END, CAN_PUSH/PASS_UNIT_TIME%d, and the village square-unit sound block;
					/// CUSTOM_RENDER_STATE (31fce500) is backlog Stage 2.
					if ( NULL != CX2RidingPetManager::GetInstance() && m_StateData.m_StateID == m_SpecialMoveState )
						kPhysicParam.passiveSpeed.x *= CX2RidingPetManager::GetInstance()->GetPassiveSpeedRate();
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE

				}
			}
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
			/// June @32d3110d..@32d35f00: with a game (g_pX2Game) the user block ends in a jump to the tail (@32d350db), so the village block and the state
			/// UI set block below run ONLY without a game; a game whose owner user is not found jumps to the tail from @32d31193 too. (C9h-3 and its kickoff
			/// read it the other way round; the disassembly says the state set block is village only, which makes JUNE_BUGS 108's double play a village
			/// / game split instead: in a game the state's EFFECT_SET_LIST is read once, by the user loader.)
			else
			{
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
				/// June 32d2f560: the village block 32d350e0 comes first, then the state UI set block 32d35a05 ( C9h-4e )
				VillageStateChangeSounds();
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
				StateChangeUiSets();
			}
			/// June 32d2bad0 @32d35f02, for every state: PLAY_RESERVED_UI_EFFECT_SET, then the queued stops of the delete blocks
			PlayReservedUiSets( vecPendingStop );
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
		}
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE_DEBUG
		{
			WCHAR wszFx[256] = {0};
			StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_STATE state=%d(%S) first=%d atend=%s onground=%d overlap=%g target=%g/%g/%g/%g",
				static_cast<int>( m_StateData.m_StateID ), stateName.c_str(), m_bFirstStateChanged ? 1 : 0,
				m_wstrAnimNameAtPlayEnd.empty() ? L"-" : m_wstrAnimNameAtPlayEnd.c_str(),
				m_iOnGroundState, m_fOnGroundOverlapTime,
				m_fTargetRange, m_fLimitZDepth, m_fDegreeRange, m_fValuePos );
			ErrorLogMsg( XEM_ERROR142, wszFx );
		}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE_DEBUG
		if( bTableOpen == true )
			m_LuaManager.EndTable();
	}
}

void CX2RidingPet::StateChange_LUA( IN const char* stateName_ )
{
	//wstring stateName;
	//ConvertUtf8ToWCHAR( stateName, stateName_ );
	StateChange( stateName_ );
}
#pragma endregion 상태 변경

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_STATE
/// June 32d3cff0. The user is the in-game unit by m_UserUid; June reads the skin anim's time before its other tests and never checks it, March
/// returns false while the anim is not loaded yet.
bool CX2RidingPet::OnGroundChangeStateCheck()
{
	if ( NULL == g_pX2Game || NULL == m_pXSkinAnim )
		return false;

	CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
	if ( NULL == pUser )
		return false;

	if ( true != pUser->IsOnSomethingFuture() )
		return false;

	if ( RSI_NONE == m_iOnGroundState || RSI_END == m_iOnGroundState || -1 == m_iOnGroundState )
		return false;

	/// OVERLAP_TIME -1.0 = change at once; otherwise wait until the anim has played that long. The wait returns true too (June), so the
	/// user's event process stays skipped for the whole wait.
	if ( -1.f != m_fOnGroundOverlapTime && m_pXSkinAnim->GetNowAnimationTime() < m_fOnGroundOverlapTime )
		return true;

	m_bOnGroundChanged = true;
	pUser->CommonStateEndByRidingPet();					/// June 31fc8810 = vslot 0x7dc on the user
	StateChangeFuture( m_iOnGroundState );
	StateChange( m_iOnGroundState );
	return true;
}

/// June 32d286c0: once the main anim has ended and ANIM_NAME_AT_PLAY_END is set, play that anim (no transition, start 0, XAP_LOOP).
void CX2RidingPet::PlayAnimAtPlayEnd()
{
	if ( NULL != m_pXSkinAnim && true == m_pXSkinAnim->IsAnimationEnd() && false == m_wstrAnimNameAtPlayEnd.empty() )
	{
		m_pXSkinAnim->ChangeAnim( m_wstrAnimNameAtPlayEnd.c_str(), false, 0.f );
		m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
	}
}
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
/// June 32d2f560 village block @32d350e0-@32d35a05 ( C9h-4e ). Runs only without a game, with the village field game ( [33775870] ) and the square unit of the
/// pet's owner found ( 323a8dd0 ); either missing skips all of it. In June's order: the pet's own skin anim ( +0x238 ) oneshot timer ( +0x24c ) = 0
/// ( the kickoff said the square unit's anim; the disassembly says this ), the square's DCS_STATE_END stop ( 323f1730 ), its sound list ( +0x5dc ) cleared and
/// refilled from SOUND_PLAY_ALL_OPTION<n> only ( no SOUND_PLAY here ). The square's StateStart ( 323fad80 @323fdf97 ) clears the same list again right after the
/// pet's StateChange returns ( 323fec40 calls the pet, then StateStart ): JUNE_BUGS.
void CX2RidingPet::VillageStateChangeSounds()
{
	if ( NULL == g_pTFieldGame )
		return;

	CX2SquareUnit* pSquareUnit = g_pTFieldGame->GetSquareUnitByUID( m_UserUid );
	if ( NULL == pSquareUnit )
		return;

	if ( NULL != m_pXSkinAnim )
		m_pXSkinAnim->ResetOneshotPerformed();

	const int iRecords = pSquareUnit->RidingPetStateChangeSounds( m_LuaManager );

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
	{
		WCHAR wszFx[160] = {0};
		StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_USER_KEYS village state=%d square_records=%d",
			static_cast<int>( m_StateData.m_StateID ), iRecords );
		ErrorLogMsg( XEM_ERROR142, wszFx );
	}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS_DEBUG
}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS
#endif SERV_IRUHADEV_JUNE_RIDINGPET_STATE
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
/// June 32d2b0d0 (C9h-3a): INIT_EFFECT.RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET (names, indices 1..99) and RIDINGPET_RESERVED_UI_EFFECT_SET_LIST (name, index,
/// playCond, deleteCond tuples at 1, 5, .. 97; the loop breaks at the first incomplete tuple; a repeated index keeps the first). Once both tables are closed each
/// BY_UIEFFECTSET name plays on the UI effect set (321e2790) and its handle goes into the kept vector even when it is -1.
void CX2RidingPet::LoadInitEffect()
{
	std::vector< std::wstring > vecUiSetName;
	int iReserved = 0;

	if ( m_LuaManager.BeginTable( "INIT_EFFECT" ) == true )
	{
		if ( m_LuaManager.BeginTable( "RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET" ) == true )
		{
			for ( int iIndex = 1; iIndex < 100; ++iIndex )
			{
				std::wstring wstrName;
				if ( false == m_LuaManager.GetValue( iIndex, wstrName ) )
					break;
				vecUiSetName.push_back( wstrName );
			}
			m_LuaManager.EndTable();
		}

		if ( m_LuaManager.BeginTable( "RIDINGPET_RESERVED_UI_EFFECT_SET_LIST" ) == true )
		{
			for ( int iBase = 1; iBase < 100; iBase += 4 )
			{
				std::wstring wstrName;
				int iSetIndex = 0;
				int iPlayCond = 0;
				int iDeleteCond = 0;

				if ( false == m_LuaManager.GetValue( iBase, wstrName ) ||
					 false == m_LuaManager.GetValue( iBase + 1, iSetIndex ) ||
					 false == m_LuaManager.GetValue( iBase + 2, iPlayCond ) ||
					 false == m_LuaManager.GetValue( iBase + 3, iDeleteCond ) )
					break;

				ReservedUiSet kSet;
				kSet.m_iDeleteCond	= iDeleteCond;
				kSet.m_iPlayCond	= iPlayCond;
				kSet.m_wstrName		= wstrName;
				if ( true == m_mapReservedUiSet.insert( std::make_pair( iSetIndex, kSet ) ).second )
					++iReserved;
			}
			m_LuaManager.EndTable();
		}

		m_LuaManager.EndTable();
	}

	if ( NULL != g_pData )
	{
		for ( UINT i = 0; i < vecUiSetName.size(); ++i )
			m_vecUiSetKept.push_back( g_pData->GetUIEffectSet()->PlayEffectSet( vecUiSetName[i], CX2RidingPetPtr( this ) ) );
	}

	WCHAR wszFx[128] = {0};
	StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_FX loader info_id=%d user=%I64d reserved=%d by_uieffectset=%d",
		m_Info.m_Id, static_cast<__int64>( m_UserUid ), iReserved, static_cast<int>( vecUiSetName.size() ) );
	ErrorLogMsg( XEM_ERROR142, wszFx );
}

/// June 32d29240: the reserved set's name by index, L"" when the index is not in the map
std::wstring CX2RidingPet::GetReservedSetName( IN int iIndex_ ) const
{
	ReservedUiSetMap::const_iterator it = m_mapReservedUiSet.find( iIndex_ );
	if ( it == m_mapReservedUiSet.end() )
		return std::wstring( L"" );

	return it->second.m_wstrName;
}

/// June 32d292b0: the first kept handle whose live instance's templet name equals the reserved name of iIndex_ (the per-state vector is never scanned),
/// else -1. A set that was only queued for stopping is still found: see JunePlayReservedUiSets.
CX2EffectSet::Handle CX2RidingPet::FindReservedSet( IN int iIndex_ )
{
	if ( NULL == g_pData )
		return INVALID_EFFECTSET_HANDLE;

	const std::wstring wstrName = GetReservedSetName( iIndex_ );

	for ( UINT i = 0; i < m_vecUiSetKept.size(); ++i )
	{
		CX2EffectSet::EffectSetInstance* pInstance = g_pData->GetUIEffectSet()->GetEffectSetInstance( m_vecUiSetKept[i] );
		if ( NULL != pInstance && NULL != pInstance->m_pEffectSetData && pInstance->m_pEffectSetData->m_wstrEffectSetName == wstrName )
			return m_vecUiSetKept[i];
	}

	return INVALID_EFFECTSET_HANDLE;
}

/// June 32d291a0: SetShowObject( bShow_ ) on every live instance of both vectors, then the hide mirror is set to 0 whatever bShow_ was (@32d29229,
/// JUNE_BUGS 110) and true is returned; false without g_pData.
bool CX2RidingPet::ShowHideAllUiSets( IN bool bShow_ )
{
	if ( NULL == g_pData )
		return false;

	for ( UINT i = 0; i < m_vecUiSetKept.size(); ++i )
	{
		CX2EffectSet::EffectSetInstance* pInstance = g_pData->GetUIEffectSet()->GetEffectSetInstance( m_vecUiSetKept[i] );
		if ( NULL != pInstance )
			pInstance->SetShowObject( bShow_ );
	}

	for ( UINT i = 0; i < m_vecUiSetState.size(); ++i )
	{
		CX2EffectSet::EffectSetInstance* pInstance = g_pData->GetUIEffectSet()->GetEffectSetInstance( m_vecUiSetState[i] );
		if ( NULL != pInstance )
			pInstance->SetShowObject( bShow_ );
	}

	m_bUiSetHidden = false;
	return true;
}

/// June 32d293e0: stop every handle of both vectors (through copies) and clear them
void CX2RidingPet::StopAllUiSets()
{
	if ( NULL == g_pData )
		return;

	for ( UINT i = 0; i < m_vecUiSetKept.size(); ++i )
	{
		CX2EffectSet::Handle hHandle = m_vecUiSetKept[i];
		g_pData->GetUIEffectSet()->StopEffectSet( hHandle );
	}
	m_vecUiSetKept.clear();

	for ( UINT i = 0; i < m_vecUiSetState.size(); ++i )
	{
		CX2EffectSet::Handle hHandle = m_vecUiSetState[i];
		g_pData->GetUIEffectSet()->StopEffectSet( hHandle );
	}
	m_vecUiSetState.clear();
}

/// June StateChange @32d31013-@32d3110d: (1) every reserved set whose delete condition is RESDC_STATE_CHANGE and that is playing, (2) the sets named by
/// DELETE_RESERVED_UI_EFFECT_SET[1..99]. The handles stay in the kept vector (JUNE_BUGS 107); they are only collected here.
void CX2RidingPet::DeleteReservedUiSets( OUT std::vector< CX2EffectSet::Handle >& vecPendingStop_ )
{
	if ( NULL == g_pData )
		return;

	for ( ReservedUiSetMap::const_iterator it = m_mapReservedUiSet.begin(); it != m_mapReservedUiSet.end(); ++it )
	{
		const CX2EffectSet::Handle hHandle = FindReservedSet( it->first );
		if ( INVALID_EFFECTSET_HANDLE != hHandle && RESDC_STATE_CHANGE == it->second.m_iDeleteCond )
			vecPendingStop_.push_back( hHandle );
	}

	if ( m_LuaManager.BeginTable( "DELETE_RESERVED_UI_EFFECT_SET" ) == true )
	{
		for ( int iIndex = 1; iIndex < 100; ++iIndex )
		{
			int iSetIndex = 0;
			if ( false == m_LuaManager.GetValue( iIndex, iSetIndex ) )
				break;

			const CX2EffectSet::Handle hHandle = FindReservedSet( iSetIndex );
			if ( INVALID_EFFECTSET_HANDLE != hHandle )
				vecPendingStop_.push_back( hHandle );
		}
		m_LuaManager.EndTable();
	}
}

/// June StateChange @32d35a05-@32d35f00 (no game only, see StateChange): every handle of the per-state vector is stopped through a copy and NOT cleared
/// (JUNE_BUGS 107); then the state's own EFFECT_SET_LIST names (odd indices 1, 3 .. 99: June skips the start times at the even ones) play on the UI effect
/// set through the riding pet holder (321e2790), into the per-state vector when DELETE_EFFECT_SET_ON_STATE_END is true and into the kept vector otherwise,
/// each shown unless the pet is hidden.
void CX2RidingPet::StateChangeUiSets()
{
	if ( NULL == g_pData )
		return;

	CX2EffectSet* pUIEffectSet = g_pData->GetUIEffectSet();

	for ( UINT i = 0; i < m_vecUiSetState.size(); ++i )
	{
		CX2EffectSet::Handle hHandle = m_vecUiSetState[i];
		pUIEffectSet->StopEffectSet( hHandle );
	}

	std::vector< std::wstring > vecSetName;
	if ( m_LuaManager.BeginTable( "EFFECT_SET_LIST" ) == true )
	{
		for ( int iIndex = 1; iIndex < 100; iIndex += 2 )
		{
			std::wstring wstrName;
			if ( false == m_LuaManager.GetValue( iIndex, wstrName ) )
				break;
			vecSetName.push_back( wstrName );
		}
		m_LuaManager.EndTable();
	}

	bool bDeleteOnStateEnd = false;
	LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_STATE_END", bDeleteOnStateEnd, false );

	for ( UINT i = 0; i < vecSetName.size(); ++i )
	{
		const CX2EffectSet::Handle hHandle = pUIEffectSet->PlayEffectSet( vecSetName[i], CX2RidingPetPtr( this ) );
		if ( INVALID_EFFECTSET_HANDLE == hHandle )
			continue;

		if ( true == bDeleteOnStateEnd )
			m_vecUiSetState.push_back( hHandle );
		else
			m_vecUiSetKept.push_back( hHandle );

		CX2EffectSet::EffectSetInstance* pInstance = pUIEffectSet->GetEffectSetInstance( hHandle );
		if ( NULL != pInstance )
			pInstance->SetShowObject( false == m_bUiSetHidden );
	}

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX_DEBUG
	if ( false == vecSetName.empty() )
	{
		WCHAR wszFx[160] = {0};
		StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_FX state=%d names=%d vector=%s kept=%d state_vec=%d",
			static_cast<int>( m_StateData.m_StateID ), static_cast<int>( vecSetName.size() ), bDeleteOnStateEnd ? L"state" : L"kept",
			static_cast<int>( m_vecUiSetKept.size() ), static_cast<int>( m_vecUiSetState.size() ) );
		ErrorLogMsg( XEM_ERROR142, wszFx );
	}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX_DEBUG
}

/// June 32d2bad0 (every state, after the blocks above): PLAY_RESERVED_UI_EFFECT_SET[1..99]; an index is skipped when its playCond is RESPC_NOT_PLAY and
/// a set of its name is already in the kept vector, a name that is empty (an index not in the map) is skipped too; then every collected name plays on the
/// UI effect set, its handle goes into the kept vector and the instance is shown unless the pet is hidden. No handle is kept per index: "already playing"
/// is found by templet name (32d292b0). Last the stops of the delete blocks run: June's are queued, so up to here a set stopped by them still counted as
/// playing and a NOT_PLAY reserved set stopped this state was skipped, then died (JUNE_BUGS 111).
void CX2RidingPet::PlayReservedUiSets( IN std::vector< CX2EffectSet::Handle >& vecPendingStop_ )
{
	if ( NULL != g_pData )
	{
		std::vector< std::wstring > vecSetName;

		if ( m_LuaManager.BeginTable( "PLAY_RESERVED_UI_EFFECT_SET" ) == true )
		{
			for ( int iIndex = 1; iIndex < 100; ++iIndex )
			{
				int iSetIndex = 0;
				if ( false == m_LuaManager.GetValue( iIndex, iSetIndex ) )
					break;

				int iPlayCond = RESPC_NONE;
				ReservedUiSetMap::const_iterator it = m_mapReservedUiSet.find( iSetIndex );
				if ( it != m_mapReservedUiSet.end() )
					iPlayCond = it->second.m_iPlayCond;

				if ( RESPC_NOT_PLAY == iPlayCond && INVALID_EFFECTSET_HANDLE != FindReservedSet( iSetIndex ) )
					continue;

				const std::wstring wstrName = GetReservedSetName( iSetIndex );
				if ( false == wstrName.empty() )
					vecSetName.push_back( wstrName );
			}
			m_LuaManager.EndTable();
		}

		CX2EffectSet* pUIEffectSet = g_pData->GetUIEffectSet();
		for ( UINT i = 0; i < vecSetName.size(); ++i )
		{
			const CX2EffectSet::Handle hHandle = pUIEffectSet->PlayEffectSet( vecSetName[i], CX2RidingPetPtr( this ) );
			if ( INVALID_EFFECTSET_HANDLE == hHandle )
				continue;

			m_vecUiSetKept.push_back( hHandle );

			CX2EffectSet::EffectSetInstance* pInstance = pUIEffectSet->GetEffectSetInstance( hHandle );
			if ( NULL != pInstance )
				pInstance->SetShowObject( false == m_bUiSetHidden );
		}

#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX_DEBUG
		if ( false == vecSetName.empty() )
		{
			WCHAR wszFx[160] = {0};
			StringCchPrintfW( wszFx, ARRAY_SIZE(wszFx), L"JUNE_FX RIDINGPET_FX state=%d reserved_played=%d kept=%d pending_stop=%d",
				static_cast<int>( m_StateData.m_StateID ), static_cast<int>( vecSetName.size() ), static_cast<int>( m_vecUiSetKept.size() ),
				static_cast<int>( vecPendingStop_.size() ) );
			ErrorLogMsg( XEM_ERROR142, wszFx );
		}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX_DEBUG

		for ( UINT i = 0; i < vecPendingStop_.size(); ++i )
		{
			CX2EffectSet::Handle hHandle = vecPendingStop_[i];
			pUIEffectSet->StopEffectSet( hHandle );
		}
	}

	vecPendingStop_.clear();
}
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
#ifdef X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
#pragma region static InitMotion
/*static*/
void CX2RidingPet::InitMotion( InitDeviceData& OutDevice_, KLuaManager& luaManager_ )
{
	if( luaManager_.BeginTable( "INIT_MOTION" ) == true )
	{		
		std::wstring	wstrName;

		if ( luaManager_.GetValue( "MOTION_FILE_NAME", wstrName ) == true )
			OutDevice_.m_vecstrReadyXSkinMesh.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_CHANGE_TEX_XET", wstrName ) == true )
			OutDevice_.m_vecstrReadyXET.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_ANI_TEX_XET", wstrName ) == true )
			OutDevice_.m_vecstrReadyXET.push_back( wstrName );

		luaManager_.EndTable();
	}
}
#pragma endregion 
#endif //X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD

#pragma region InitMotion
void CX2RidingPet::InitMotion()
{
	if( m_LuaManager.BeginTable( "INIT_MOTION" ) == true )
	{		
		LUA_GET_VALUE( m_LuaManager, "MOTION_FILE_NAME", m_wstrMotionName, L"" );		
		LUA_GET_VALUE( m_LuaManager, "MOTION_CHANGE_TEX_XET", m_wstrChangeTex, L"" );
		LUA_GET_VALUE( m_LuaManager, "MOTION_ANI_TEX_XET", m_wstrAniTex, L"" );		
		LUA_GET_VALUE( m_LuaManager, "RIDING_MOTION", m_wstrRidingMotion, L"" );

		m_LuaManager.EndTable();
	}

	SAFE_CLOSE( m_pChangeXET );
	m_pChangeXET = g_pKTDXApp->GetDeviceManager()->OpenXET( m_wstrChangeTex.c_str() );
	SAFE_CLOSE( m_pAniXET );
	m_pAniXET = g_pKTDXApp->GetDeviceManager()->OpenXET( m_wstrAniTex.c_str() );

}
#pragma endregion  모션 설정

#pragma region ClearState
void CX2RidingPet::ClearState()
{
	m_StartState = m_WaitHabitState = m_WaitState = m_WalkState = m_JumpUpState = m_JumpDownState = m_JumpLandingState =
		m_DashState = m_DashEndState = m_DashJumpUpState = m_DashJumpLandingState = m_DamageFrontState = m_DamageBackState =
		m_DyingState = m_AttackZState = m_JumpAttackZState = m_AttackXState = m_SpecialAttackState = m_SitReadyState =
		m_SitWaitState = m_StandUpState = m_SpecialMoveState = RSI_NONE;
}
#pragma endregion 기본 상태들 값 초기화

#pragma region GetUserInfo
void CX2RidingPet::GetUserInfo( IN D3DXVECTOR3 &vPos, IN bool &bIsRight_ )
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_SADDLE_POS
	// June 32d2d7e0: the saddle is kept relative to the rider ( bone - rider position ); the rider's draw adds its position back
	vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
	bIsRight_ = true;

	if ( -1 >= m_UserUid )
		return;

	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) - pUser->GetPos() );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );

			m_pXSkinAnim->GetMatrix().Scale( pUser->GetScaleByUnit() );
		}
	}
	else if ( NULL != g_pTFieldGame )
	{
		CX2SquareUnit* pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) - pUser->GetPos() );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );	// stand-in: June also clears square-unit byte +0x794 here ( @32d2d8bc ), member not identified
		}
	}
#else SERV_IRUHADEV_JUNE_DIVERGE_SADDLE_POS
	vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
	bIsRight_ = true;

	if ( -1 >= m_UserUid )
		return;

	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );

			m_pXSkinAnim->GetMatrix().Scale( pUser->GetScaleByUnit() );
		}
	}
	else if ( NULL != g_pTFieldGame )
	{
		CX2SquareUnit* pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );
		}
	}
#endif SERV_IRUHADEV_JUNE_DIVERGE_SADDLE_POS
}
#pragma endregion 안장 위치 전달, 현재 위치 구하기

#pragma region SetPos
void CX2RidingPet::SetPos( IN D3DXVECTOR3& vPos_, IN bool bIsRight_ )
{
	KTDXPROFILE();

	CKTDGLineMap* pLineMap = NULL;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetWorld() )
	{
		pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	}
	else if ( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetWorld() )
	{
		pLineMap = g_pTFieldGame->GetWorld()->GetLineMap();
	}

	if ( NULL == pLineMap )
		return;

	if ( false == pLineMap->GetAnyEnabledLine() )
		return;

	D3DXVECTOR3 startPos;
	D3DXVECTOR3 dirVec( 0.f, 0.f, 0.f );

	startPos = vPos_;
	m_SyncData.m_bIsRight = bIsRight_;

	int iLineIndex = -1;
	D3DXVECTOR3 vLandPos = startPos;
#ifdef SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
	vLandPos = pLineMap->GetLandPosition( startPos, false, LINE_RADIUS, &iLineIndex );	/// June SetPos 32d2db50 @32d2dca1: bMonster false
#else SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
	vLandPos = pLineMap->GetLandPosition( startPos, LINE_RADIUS, &iLineIndex );
#endif SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
	pLineMap->IsOnLine( startPos, LINE_RADIUS + 10, &startPos, &iLineIndex, bIsRight_ );

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );

	if ( NULL == pLineData )
	{
		ASSERT( !"CX2RidingPet::SetPos, null linedata" );
		return;
	}

	if ( true == m_SyncData.m_bIsRight )
	{
		dirVec.x = pLineData->dirDegree.x;
		dirVec.y = pLineData->dirDegree.y;
	}
	else
	{
		dirVec.x = pLineData->dirDegree.x;
		dirVec.y = pLineData->dirDegree.y + 180.f;
	}

	//m_SyncData.m_vPos			= startPos;
	m_SyncData.m_vPos			= vPos_;
	m_ConditionData.m_vDirDegree	= dirVec;
	m_ConditionData.m_vDir			= pLineData->dirVector;
	m_ConditionData.m_vLandPosition	= vLandPos;
	m_SyncData.m_ucLastTouchLineIndex	= iLineIndex;
}
#pragma endregion 현재 위치 및 회전 값 얻기

#pragma region InitPos
bool CX2RidingPet::InitPos()
{
	if ( true == m_bInitPos )
		return true;

	D3DXVECTOR3 vPos;
	bool bIsRight;
	GetUserInfo( vPos, bIsRight );

	if ( false == m_bInit )
	{
		CKTDGLineMap* pLineMap = NULL;

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetWorld() )
		{
			pLineMap = g_pX2Game->GetWorld()->GetLineMap();
		}
		else if ( NULL != g_pTFieldGame )
		{
			pLineMap = g_pTFieldGame->GetWorld()->GetLineMap();
		}

		if ( NULL == pLineMap )
			return false;

		if ( false == pLineMap->GetAnyEnabledLine() )
			return false;

		int iLineIndex = -1;
		D3DXVECTOR3 vStartPos = vPos;
		D3DXVECTOR3 vLandPos = vStartPos;

#ifdef SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
		vLandPos = pLineMap->GetLandPosition( vStartPos, false, LINE_RADIUS, &iLineIndex );	/// June InitPos 32d2e090 @32d2e1dc: bMonster false
#else SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
		vLandPos = pLineMap->GetLandPosition( vStartPos, LINE_RADIUS, &iLineIndex );
#endif SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
		pLineMap->IsOnLine( vStartPos, LINE_RADIUS, &vStartPos, &iLineIndex, true );

		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );

		if ( NULL == pLineData )
			return false;
	}

	vPos.y += 30.f;
	SetPos( vPos, bIsRight );
	m_bInitPos = true;

	return true;
}
#pragma endregion 위치 초기화

#pragma region CommonCameraFrameMove
void CX2RidingPet::CommonCameraFrameMove()
{
	KTDXPROFILE();

	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
		if ( NULL != pUser )
		{
			CX2GUUser::FrameData& kNowFrameData = pUser->AccessNowFrameData();
			CX2GUUser::FrameData& kFutureFrameData = pUser->AccessFutureFrameData();

			//if ( NULL == pFrameData || NULL == pFrameDataFuture )
			//	return;
			//
			int iCameraIndex = kNowFrameData.stateParam.normalCamera;
	
			if( true == kNowFrameData.unitCondition.bHit )
				iCameraIndex = kNowFrameData.stateParam.hitCamera;

		#ifdef CAMERA_SCRIPTING_TEST
			g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = D3DXVECTOR3( 0, 0, 0 );
		#endif CAMERA_SCRIPTING_TEST
	
			bool bTableOpen = false;

			bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
			{
				if( this->m_LuaManager.BeginTable( "CAMERA", iCameraIndex ) == true )
				{
					std::vector< float > vecSubCameraTiming;
					if( this->m_LuaManager.BeginTable( "SUB_CAMERA_TIMING" ) == true )
					{
						float fSubCameraTiming = 0.f;
						for( int i=1; ; i++ )
						{
							if( true == this->m_LuaManager.GetValue( i, fSubCameraTiming ) )
							{
								vecSubCameraTiming.push_back( fSubCameraTiming );
							}
							else
							{
								break;
							}
						}
						this->m_LuaManager.EndTable(); // SUB_CAMERA_TIMING
					}

					if( true == vecSubCameraTiming.empty() )
					{
						g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, this->m_LuaManager, 1 );
					}
					else
					{
						bool bDone = false;
						for( int i=0; i<(int)vecSubCameraTiming.size(); i++ )
						{
							float fTiming = vecSubCameraTiming[i];

							if( m_pXSkinAnim->GetNowAnimationTime() < fTiming )
							{
								char tableName[64] = "";
								StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", i );
								g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, this->m_LuaManager, tableName );
								bDone = true;
								break;
							}
						}

						if( false == bDone )
						{
							char tableName[64] = "";
							StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", (int)vecSubCameraTiming.size() );
							g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, m_LuaManager, tableName );
						}
					}
				
					this->m_LuaManager.EndTable(); // CAMERA
				}
				else
				{

				float eyeDistance = 0;
				const CKTDGLineMap::CameraData* pCameraData = g_pX2Game->GetWorld()->GetLineMap()->GetCameraData(
					kFutureFrameData.syncData.lastTouchLineIndex );

				if( NULL != pCameraData )
				{
		#ifdef CHECK_CAMERA_INFO
			#ifdef RIDING_SYSTEM
					pUser->SetIsLineMapCamera( true );
			#else //RIDING_SYSTEM
					m_bIsLineMapCamera = false;
			#endif //RIDING_SYSTEM
		#endif //CHECK_CAMERA_INFO
					switch( pCameraData->m_eType )
					{
					case CKTDGLineMap::LCT_ABSOULTE_POSITION:
						{
							g_pX2Game->GetX2Camera()->GameLineCamera( pUser, pCameraData->m_vEye, pCameraData->m_vLookAt,
								pCameraData->m_bFocusUnit, 0.4f );			
						} break;
					case CKTDGLineMap::LCT_RELATIE_POSITION:
						{
		#ifdef CAMERA_SCRIPTING_TEST
							g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = pCameraData->m_vRelativeEye;
							g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_fSpeed = pCameraData->m_fCameraRepositionSpeed;
		#endif CAMERA_SCRIPTING_TEST
							g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(), 200,
								pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );
						} break;

					default:
						{
							g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(),
								200, pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );

						} break;
					}
				}
				else
				{
		#ifdef CHECK_CAMERA_INFO
			#ifdef RIDING_SYSTEM
					pUser->SetIsLineMapCamera( true );
			#else //RIDING_SYSTEM
					m_bIsLineMapCamera = false;
			#endif //RIDING_SYSTEM
		#endif //CHECK_CAMERA_INFO
		#ifdef SERV_CHUNG_TACTICAL_TROOPER
					if( pUser->GetGameCameraOffset().bEnable == true )
					{
						g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance() + 
							pUser->GetGameCameraOffset().fCameraDistance,
							200.f + pUser->GetGameCameraOffset().fHeight,
							pUser->GetCameraAngleDegree() + pUser->GetGameCameraOffset().fAngleDegree,				
							eyeDistance + pUser->GetGameCameraOffset().fEyeDistance,
							0.f + pUser->GetGameCameraOffset().fLookatDistance,
							0.3f + pUser->GetGameCameraOffset().fTrackingTime,
							0.0f + pUser->GetGameCameraOffset().fTrackingAtTime );

					}
					else
						g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(), 200,
						pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );
		#else
					g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(), 200, m_fAngleDegree, eyeDistance, 0.f, 0.3f );
		#endif SERV_CHUNG_TACTICAL_TROOPER
					}
				}
			}
			if( bTableOpen == true )
				m_LuaManager.EndTable();
		}
	}
}
#pragma endregion 카메라 파싱 및 변경
#endif //RIDING_SYSTEM