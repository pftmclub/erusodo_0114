#include "stdafx.h"
#include ".\x2world.h"
#include "KHelpSystem.h"
#if defined(SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG) && defined(SERV_IRUHADEV_OFFLINE)
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG
#ifdef SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_ELYSION_DEBUG


#ifdef KHELP_SYSTEM_TEST // 2008-10-23

IMPLEMENT_HELP_SYSTEM( CX2World )
{
    WCHAR   wbuffer[1024];
    int     ix = 16;
    int     iy = 16;
    StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer), L"%S", __FUNCTION__ );

    if ( g_pMain && g_pData )
    {
        CX2World*   pWorld = g_pData->GetWorldManager()->GetWorldLatest();
	    g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
		    CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
        if ( pWorld )
            pWorld->Debug_RenderHelp( ix, iy );
    }//if
}//CX2World::DisplayHelp()

#endif // KHELP_SYSTEM_TEST // 2008-10-23


// constructor.
CX2World::CX2World( WorldData* pWorldData, const bool bBackgroundLoad_ /*= false*/ )
: m_bBackgroundLoad( bBackgroundLoad_ )
, m_bObjectListSorted(false)
, m_pLineMap( NULL )
, m_pLineMapFuture( NULL )
, m_bFirstLineMapFrameMove( true )
{
	m_pWorldData	= pWorldData;
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	m_bShowWorld = true;	// June ctor 327fddf0 @327fdec8
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

	m_bRenderLineMap = false;
#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
	m_fMovableLineSyncTime = 0.f;
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE
	m_LodPercent = 100;



	INIT_VECTOR3( m_LightPos, 0.0f, 0.0f, 0.0f );
#ifdef SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
	m_iCartoonTextureIndex = 0;
	m_iRimLightTextureIndex = 0;
#endif SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	m_pEnvBoxMap = NULL;
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	m_LightColor			= 0xffffffff;		
	m_FadeLightColor		= 0xffffffff;	
	m_fFadeLightTime		= 0.f;



	m_OriginColor		= 0xffffffff;
	m_FadeOriginColor	= 0xffffffff;	
	m_fFadeOriginTime	= 0.0f;


	m_NowColor			= 0xffffffff;
	m_FadeColor			= 0xffffffff;	
	m_fFadeTime			= 0.0f;

#ifdef FOG_WORLD	
    m_bFog = false;   
    m_bFogShow = false;
	m_fDensity = 0.f;
	m_fFarX = 0.f;
	m_fFarY = 0.f;
	m_fNearX = 0.f;
	m_fNearY = 0.f;
	m_FogColor = D3DXCOLOR(0xd8e3fe);
#endif


#ifdef BUBBLE_BOBBLE_TEST
	m_vLookAt	= D3DXVECTOR3( 0, 0, 0 );
	m_vEye		= D3DXVECTOR3( 0, 0, -100 );
#endif BUBBLE_BOBBLE_TEST

    m_bDistanceColor = false;

	m_pKLuabinder	= NULL;





#ifdef DEEP_WATER_DUNGEON_TEST
	m_fGravityAndSpeedRate = 1.f;
	m_fUnitAnimSpeedRate = 1.f;
#endif DEEP_WATER_DUNGEON_TEST

#ifdef WORLD_TRIGGER
	m_iTriggerCameraIndex		= -1;
	m_fDurationTriggerCamera	= 0.f;
#endif

#ifdef MODIFY_FRUSTUM
	m_fBaseCameraDistance = 0.f;
	m_fCameraDistanceStep[0] = 0.f;
	m_fCameraDistanceStep[1] = 0.f;
	m_fCameraDistanceStep[2] = 0.f;
#endif

#ifdef X2TOOL
	m_wstrWorldName = L"";
	m_wstrWorldCamera = L"";
#endif

#ifdef OPTIMIZATION_DEV_MODE
	SetRenderLineMap(true);
#endif OPTIMIZATION_DEV_MODE
}

CX2World::~CX2World(void)
{
    //{{ seojt // 2008-10-22, 10:18
    /** 월드 객체를 파괴하기 바로 전에, 월드 매니저에게 월드가 파괴된다는 것을 알려준다.
        background thread loading작업을 하면서, 게임 상태와 상관없이 현 시점의 생성된 월드를
        얻어내는 작업을 하기 위해서 처음으로 추가하다.
        - jintaeks on 2008-10-22, 10:19 */
    if ( g_pData && g_pData->GetWorldManager() )
    {
        g_pData->GetWorldManager()->OnPreDestroyWorld( this );
    }//if
    //}} seojt // 2008-10-22, 10:18

	SAFE_DELETE_KTDGOBJECT( m_pLineMap );
	SAFE_DELETE_KTDGOBJECT( m_pLineMapFuture );
#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	SAFE_DELETE_KTDGOBJECT( m_pEnvBoxMap );		/// June dtor 32803a60 releases the map ( +0x40 )
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX

#ifdef SERV_IRUHADEV_RANOX_RIM_LIGHT
	/// Iruha 2026-09-19: the override is renderer-global (CKTDGXRenderer outlives any one CX2World), so
	/// it must be cleared here or a blue rim-light tint set in Ranox's Secret Hell would follow the
	/// player out into every later map's cel-shaded units.
	if ( g_pKTDXApp && g_pKTDXApp->GetDGManager() && g_pKTDXApp->GetDGManager()->GetXRenderer() )	/// Iruha 2026-09-19: guarded like this destructor's own g_pData check above -- GetDGManager() is set NULL during app teardown (KTDXApp.cpp's stage-chain cleanup), and a world can still be getting destroyed then.
		g_pKTDXApp->GetDGManager()->GetXRenderer()->SetRimLightTexType( CKTDGXRenderer::CTT_END );
#endif SERV_IRUHADEV_RANOX_RIM_LIGHT


	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		SAFE_DELETE_KTDGOBJECT( pCKTDGSkyDome );
	}
	m_SkyDomeList.clear();

	m_ObjectList.clear();    
    
	for( int i = 0; i < (int)m_pLensFlareList.size(); i++ )
	{
		CKTDGLensFlare* pCKTDGLensFlare = m_pLensFlareList[i];
		SAFE_DELETE_KTDGOBJECT( pCKTDGLensFlare );
	}
	m_pLensFlareList.clear();

#ifdef WORLD_LAYER
    for( int i = 0; i < (int)m_LayerList.size(); i++ )
    {
        CX2WorldLayer* pWorldLayer = m_LayerList[i];
		SAFE_DELETE_KTDGOBJECT( pWorldLayer );
    }
    m_LayerList.clear();
#endif

	for ( int i = 0; i < (int)m_vecEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vecEffectBGM[i];

		if( pSound != NULL && pSound->IsPlaying() == true )
			pSound->Stop();

		SAFE_CLOSE( pSound );
	}
	m_vecEffectBGM.clear();

	for ( int i = 0; i < (int)m_vec3DEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vec3DEffectBGM[i];

		if( pSound != NULL && pSound->IsPlaying() == true )
			pSound->Stop();

		SAFE_CLOSE( pSound );
	}
	m_vec3DEffectBGM.clear();


    m_vecSyncObject.clear();

	//if( g_pKTDXApp->GetDSManager()->GetInit() == true && g_pKTDXApp->GetDSManager()->GetMP3Play() != NULL )
	//	g_pKTDXApp->GetDSManager()->GetMP3Play()->Stop();
#ifdef KEYFRAME_CAMERA
	UnLoadWorldCamera();
#endif KEYFRAME_CAMERA
}

HRESULT CX2World::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();



#ifdef WORLD_TRIGGER
	DoTriggerFrameMove( fTime, fElapsedTime );
	if( m_iTriggerCameraIndex >= 0 )
	{
		m_fDurationTriggerCamera -= fElapsedTime;
		if( m_fDurationTriggerCamera <= 0.f )
		{
			m_iTriggerCameraIndex = -1;
			m_fDurationTriggerCamera = 0.f;
		}
	}
#endif

	if( 0.f != m_fFadeTime && 
		m_FadeColor != m_NowColor )
	{
		D3DXCOLOR remainColor = m_FadeColor - m_NowColor;
		remainColor /= m_fFadeTime;
		m_NowColor += remainColor * fElapsedTime;

		m_fFadeTime -= fElapsedTime;
		if( m_fFadeTime <= 0.0f )
		{
			m_NowColor = m_FadeColor;
		}
	}


	if( m_fFadeOriginTime > 0.f )
	{
		m_fFadeOriginTime -= fElapsedTime;
		if( m_fFadeOriginTime <= 0.f )
		{
			m_fFadeOriginTime = 0.f;
			m_LightColor = m_FadeOriginColor;
		}
		else
		{
			D3DXCOLOR remainColor = m_FadeOriginColor - m_OriginColor;
			remainColor /= m_fFadeOriginTime;
			m_OriginColor += remainColor * fElapsedTime;
		}
	}



	if( m_fFadeLightTime > 0.f )
	{
		m_fFadeLightTime -= fElapsedTime;
		if( m_fFadeLightTime <= 0.f )
		{
			m_fFadeLightTime = 0.f;
			m_LightColor = m_FadeLightColor;
		}
		else
		{
			D3DXCOLOR remainColor = m_FadeLightColor - m_LightColor;
			remainColor /= m_fFadeLightTime;
			m_LightColor += remainColor * fElapsedTime;
		}
	}
#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
	/// June 32802140: the line map's OnFrameMove ( vslot 1, 31c70580: the movable lines ) after the colour fades, before the sky domes.
	/// March never ticked the line map ( its OnFrameMove only refreshed the vertices in the tools ).
	if ( NULL != m_pLineMap )
		m_pLineMap->OnFrameMove( fTime, fElapsedTime );
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE







	KTDXPROFILE_BEGIN( "SKY_DOME" );    

	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		if( pCKTDGSkyDome->GetShowObject() == true )
		{
			pCKTDGSkyDome->OnFrameMove( fTime, fElapsedTime );            
			pCKTDGSkyDome->SetColorFactor( m_NowColor );
		}		
	}
	KTDXPROFILE_END();

	KTDXPROFILE_BEGIN( "OBJECT" );

    if( m_bObjectListSorted == false )
    {
        SortOnetime();
    }//if

	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];

		if( pCX2WorldObject->GetShowObject() == true )
		{
			pCX2WorldObject->OnFrameMove( fTime, fElapsedTime );
			if( true == pCX2WorldObject->GetUseWorldColor() )
			{
                D3DXCOLOR nowColor = m_NowColor;
#if 1
                if(m_bDistanceColor)
                {                    
                    nowColor.r += 0.0923f;
                    nowColor.g += 0.1198f;
                    nowColor.b += 0.3864f;
                }

				pCX2WorldObject->SetColorFactor( nowColor );
#endif
			}
		}		
#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
		/// June 32802140 @32802785-3280283c: every object, shown or not, once the line map has records ( +0x2dc, the map's size )
		if ( NULL != m_pLineMap && false == m_pLineMap->GetMovableLineDataMap().empty() )
		{
			if ( CX2WorldObject::OT_SKIN_MESH == pCX2WorldObject->GetObjectType() )
				FrameMoveMovableLineSkinMesh( static_cast<CX2WorldObjectSkinMesh*>( pCX2WorldObject.get() ) );
			else if ( CX2WorldObject::OT_STATIC_MESH == pCX2WorldObject->GetObjectType() )
				FrameMoveMovableLineMesh( static_cast<CX2WorldObjectMesh*>( pCX2WorldObject.get() ) );
		}
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE

	}

	KTDXPROFILE_END();

#ifdef WORLD_LAYER
    KTDXPROFILE_BEGIN( "LAYER" );
    for( int i = 0; i < (int)m_LayerList.size(); i++ )
    {
        CX2WorldLayer* pCX2WorldLayer = m_LayerList[i];
        if( pCX2WorldLayer->GetShowObject() == true )
        {
            pCX2WorldLayer->OnFrameMove( fTime, fElapsedTime );
            if( true == pCX2WorldLayer->GetUseWorldColor() )
            {
                D3DXCOLOR nowColor = m_NowColor;

                if(m_bDistanceColor)
                {
                    nowColor.r += 0.0923f;
                    nowColor.g += 0.1198f;
                    nowColor.b += 0.3864f;
                }

                pCX2WorldLayer->SetColorFactor( nowColor );
            }
        }		
    }
    KTDXPROFILE_END();
#endif

#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
	SendMovableLineSync( fElapsedTime );	/// June 32802140 @328029c3, after the object and layer loops
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE
	return S_OK;
}

HRESULT CX2World::OnFrameRender()
{
	if( true == m_bRenderLineMap )
	{
		if( NULL != m_pLineMap )
			m_pLineMap->OnFrameRender();
	}
	return S_OK;
}

HRESULT CX2World::OnResetDevice()
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	if( NULL != m_pEnvBoxMap )
		m_pEnvBoxMap->OnResetDevice();		/// June OnResetDevice 327fb6b0 @327fb6e7
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	if( NULL != m_pLineMap )
		m_pLineMap->OnResetDevice();
	
	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		if ( pCKTDGSkyDome != NULL )
			pCKTDGSkyDome->OnResetDevice(); 
	}

	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
		if ( pCX2WorldObject != NULL )
			pCX2WorldObject->OnResetDevice();
	}

#ifdef WORLD_LAYER
    for( int i = 0; i < (int)m_LayerList.size(); i++ )
    {
        CX2WorldLayer* pCX2WorldLayer = m_LayerList[i];
		if ( pCX2WorldLayer != NULL )
			pCX2WorldLayer->OnResetDevice();
    }
#endif

	return S_OK;
}

HRESULT CX2World::OnLostDevice()
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	if( NULL != m_pEnvBoxMap )
		m_pEnvBoxMap->OnLostDevice();		/// June OnLostDevice 327fb800
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
	if( NULL != m_pLineMap )
		m_pLineMap->OnLostDevice();

	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		if ( pCKTDGSkyDome != NULL )
			pCKTDGSkyDome->OnLostDevice();
	}

	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
		if ( pCX2WorldObject != NULL )
			pCX2WorldObject->OnLostDevice();
	}

#ifdef WORLD_LAYER
    for( int i = 0; i < (int)m_LayerList.size(); i++ )
    {
        CX2WorldLayer* pCX2WorldLayer = m_LayerList[i];
		if ( pCX2WorldLayer != NULL )
			pCX2WorldLayer->OnLostDevice();
    }
#endif

	return S_OK;
}

#ifdef SERV_IRUHADEV_RANOX_RIM_LIGHT
/// Iruha 2026-09-19: Ranox port Phase 15. Forwards to the renderer-wide override consulted by
/// CKTDGXRenderer::BeginRender's 3 cartoon-tex sites -- storage lives there, not on CX2World,
/// since the effect is global to every cel-shaded unit currently on screen, not just this world.
void CX2World::SetRimLightTextureIndex( int nCartoonTexIndex )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
	m_iRimLightTextureIndex = nCartoonTexIndex;	/// June 327fb220: the raw Lua int is stored on the world (+0x1a8); units copy it into RenderParam and BeginRender clamps it to 13..18
#else SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
	g_pKTDXApp->GetDGManager()->GetXRenderer()->SetRimLightTexType( (CKTDGXRenderer::CARTOON_TEX_TYPE)nCartoonTexIndex );
#endif SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
}
#endif SERV_IRUHADEV_RANOX_RIM_LIGHT

#ifdef SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON
/// June 327fb200: g_pWorld:SetCartoonTextureIndex( n ) stores the raw int on the world (+0x1a4). Units copy it into RenderParam::iWorldCartoonTexIndex; only
/// CTT_NORMAL units use it (0..3 = NORMAL / TWOTONE / USER1 / USER2, anything else 0). No June script calls it.
void CX2World::SetCartoonTextureIndex( int iCartoonTexIndex )
{
	m_iCartoonTextureIndex = iCartoonTexIndex;
}
#endif SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON

#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
/// June 327fd080: g_pWorld:SetEnvBoxMap( "File.lua" ) drops the old map, makes a new one and loads the script ( which calls g_pEnvBoxMap:SetLUT / AddEnvBox_LUA ).
/// The Lua global g_pEnvBoxMap keeps pointing at a deleted map when this is called again, as in June.
void CX2World::SetEnvBoxMap( const char* pFileName )
{
	SAFE_DELETE_KTDGOBJECT( m_pEnvBoxMap );

	wstring wstrFileName;
	if( NULL != pFileName )
		ConvertUtf8ToWCHAR( wstrFileName, pFileName );

	m_pEnvBoxMap = CKTDGEnvBoxMap::CreateEnvBoxMap();
	if( NULL != m_pEnvBoxMap )
		m_pEnvBoxMap->LoadScript( wstrFileName );
}

/// June 327fb5f0 @327fb61e-327fb689: the world render gives the env box map the camera unit's position when the renderer's env box switch ( +0x80, option
/// USE_ENV_BOX ) is on and a unit exists. The two calls of 31cda140 around it only reach the map's empty draw slot, so they are not ported.
/// The first call logs, once, that the hook is alive and that the Lua binding is in place ( the bind runs during the kom mount, whose ErrorLog lines are lost ).
void CX2World::RenderEnvBox( const D3DXVECTOR3* pvPos )
{
	static bool s_bLoggedHook = false;
	if( false == s_bLoggedHook )
	{
		s_bLoggedHook = true;
		lua_State* L = g_pKTDXApp->GetLuaBinder()->GetLuaState();
		int iClass = 0, iAdd = 0, iSetLut = 0, iWorld = 0;
		lua_getglobal( L, "CKTDGEnvBoxMap" );
		if( lua_istable( L, -1 ) )
		{
			iClass = 1;
			lua_getfield( L, -1, "AddEnvBox_LUA" );
			iAdd = lua_isfunction( L, -1 ) ? 1 : 0;
			lua_pop( L, 1 );
			lua_getfield( L, -1, "SetLUT" );
			iSetLut = lua_isfunction( L, -1 ) ? 1 : 0;
			lua_pop( L, 1 );
		}
		lua_pop( L, 1 );
		lua_getglobal( L, "CX2World" );
		if( lua_istable( L, -1 ) )
		{
			lua_getfield( L, -1, "SetEnvBoxMap" );
			iWorld = lua_isfunction( L, -1 ) ? 1 : 0;
			lua_pop( L, 1 );
		}
		lua_pop( L, 1 );
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX hook alive: Lua class CKTDGEnvBoxMap bound " << iClass << ", AddEnvBox_LUA " << iAdd << ", SetLUT " << iSetLut
			<< ", g_pWorld:SetEnvBoxMap " << iWorld << ", renderer env box switch " << (int) g_pKTDXApp->GetDGManager()->GetXRenderer()->IsEnvBoxEnable()
			<< ", map in this world " << ( NULL != m_pEnvBoxMap ? 1 : 0 ) << fileout;
	}

	if( NULL == m_pEnvBoxMap || NULL == pvPos )
		return;
	if( false == g_pKTDXApp->GetDGManager()->GetXRenderer()->IsEnvBoxEnable() )
		return;

	m_pEnvBoxMap->Render( *pvPos );
}
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
void CX2World::SetMapDetail( CX2GameOption::OptionLevel optionLevel )
{
	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		if( optionLevel == CX2GameOption::OL_LOW )
		{			
			//pCKTDGSkyDome->SetShowObject( false );
						
#ifdef FOG_WORLD
            pCKTDGSkyDome->SetFogShow( false );
#endif
		}
		else
		{
			//pCKTDGSkyDome->SetShowObject( true );
#ifdef FOG_WORLD            
            pCKTDGSkyDome->SetFogShow( true );            
#endif
		}
	}

#ifdef WORLD_LAYER
    for( int i = 0; i < (int)m_LayerList.size(); i++ )
    {
        CX2WorldLayer* pLayer = m_LayerList[i];
        if( optionLevel == CX2GameOption::OL_LOW )
        {
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_MAP_DETAIL
			// June 327fbeb0: a layer shown at start and forced stays shown at LOW
			pLayer->SetShowObject( true == pLayer->GetInitShow() && true == pLayer->GetForceShow() );
#else SERV_IRUHADEV_JUNE_DIVERGE_MAP_DETAIL
            pLayer->SetShowObject( false );
#endif SERV_IRUHADEV_JUNE_DIVERGE_MAP_DETAIL
        }
        else
        {
			if( pLayer->GetInitShow() == true )
				pLayer->SetShowObject( true );
        }
    }
#endif
    
	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];

        if( g_pSquareGame != NULL)            
        {
            if( pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_STATIC_MESH )
            {
                CX2WorldObjectMesh* pWorldMesh = (CX2WorldObjectMesh *)pCX2WorldObject.get();
                if(pWorldMesh->GetXMeshName().compare(L"Square_Moon.Y") == 0)
                {
                    pCX2WorldObject->SetShowObject( true );
                    continue;
                }
            }
        }

		if( optionLevel == CX2GameOption::OL_LOW || optionLevel == CX2GameOption::OL_MEDIUM )
		{
			if( pCX2WorldObject->GetObjectType() != CX2WorldObject::OT_STATIC_MESH || pCX2WorldObject->IsAlphaObject() == true) 
			{                
				if( pCX2WorldObject->GetForceShow() == true )
				{
					pCX2WorldObject->SetShowObject( true );
				}
				else
				{
					pCX2WorldObject->SetShowObject( false );					
				}				
			}
			else
			{
				pCX2WorldObject->SetShowObject( true );
			}
		}
		else
		{
			pCX2WorldObject->SetShowObject( true );
		}

#ifdef FOG_WORLD
        // Fog Setting
        if( optionLevel != CX2GameOption::OL_LOW)
        {            
            if( pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_STATIC_MESH )
            {
                CX2WorldObjectMesh *pWorldMesh = (CX2WorldObjectMesh *)pCX2WorldObject.get();
                pWorldMesh->SetFogShow(true);                
            }
            else if(pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_SKIN_MESH)
            {
                CX2WorldObjectSkinMesh *pWorldMesh = (CX2WorldObjectSkinMesh *)pCX2WorldObject.get();
                pWorldMesh->SetFogShow(true);
            }   
        }
        else
        {
            if( pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_STATIC_MESH )
            {
                CX2WorldObjectMesh *pWorldMesh = (CX2WorldObjectMesh *)pCX2WorldObject.get();
                pWorldMesh->SetFogShow(false);
            }
            else if(pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_SKIN_MESH)
            {
                CX2WorldObjectSkinMesh *pWorldMesh = (CX2WorldObjectSkinMesh *)pCX2WorldObject.get();
                pWorldMesh->SetFogShow(false);
            }                    
        }
#endif
	}
}

#ifdef FOG_WORLD
void CX2World::ToggleFog(bool flag)
{
    for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
    {
        CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];            

        if(flag == false)
        {
            if( pCKTDGSkyDome->GetFogShow() == true )
                pCKTDGSkyDome->SetFogShow( false );
            else
                pCKTDGSkyDome->SetFogShow( true );
        }
        else
        {
            pCKTDGSkyDome->SetFogShow( true );
        }

        

    }

    for( int i = 0; i < (int)m_ObjectList.size(); i++ )
    {
        CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
                
        if( pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_STATIC_MESH )
        {
            CX2WorldObjectMesh *pWorldMesh = (CX2WorldObjectMesh *)pCX2WorldObject.get();

            if(flag == false)
            {
                if( pWorldMesh->GetFogShow() == true )
                    pWorldMesh->SetFogShow(false);
                else
                    pWorldMesh->SetFogShow(true);
            }
            else
            {
                pWorldMesh->SetFogShow(true);
            }
            
        }
        else if(pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_SKIN_MESH)
        {
            CX2WorldObjectSkinMesh *pWorldMesh = (CX2WorldObjectSkinMesh *)pCX2WorldObject.get();

            if(flag == false)
            {
                if( pWorldMesh->GetFogShow() == true )
                    pWorldMesh->SetFogShow(false);
                else
                    pWorldMesh->SetFogShow(true);
            }
            else
            {
                pWorldMesh->SetFogShow(true);
            }
            
        }        
    }

    if(flag == false)
    {
        if(m_bFogShow == true)
        {
            if(m_bFog == true)
                m_bFog = false;
            else
                m_bFog = true;
        }
    }    
}
#endif

void CX2World::PlayBGM()
{
	if( true == m_BGMName.empty() )
		return;

	if( NULL == g_pKTDXApp->GetDSManager() )
		return;

	if( g_pKTDXApp->GetDSManager()->GetInit() == false || g_pKTDXApp->GetDSManager()->GetMP3Play() == NULL )
		return;

#ifdef DUNGEONSTAGE_CONTINUOUS_BGM
	if(StrCmpW(m_BGMName.c_str(), g_pKTDXApp->GetDSManager()->GetMP3Play()->GetPlayingFileName().c_str()) == 0 )
		return;
#endif DUNGEONSTAGE_CONTINUOUS_BGM

	g_pKTDXApp->GetDSManager()->GetMP3Play()->Play( m_BGMName.c_str() );
}

void CX2World::StopBGM()
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false || g_pKTDXApp->GetDSManager()->GetMP3Play() == NULL )
		return;
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	// Atlas E7: June 327fdb80 empties the player's playing name first ( @327fdbe4-327fdc0b ), so a later PlayBGM of the
	// same file is not skipped by the DUNGEONSTAGE_CONTINUOUS_BGM name test; then Stop ( 31d5ffa0 ).
	g_pKTDXApp->GetDSManager()->GetMP3Play()->ClearPlayingFileName();
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B
	g_pKTDXApp->GetDSManager()->GetMP3Play()->Stop();
}//CX2World::StopBGM()


//{{ seojt // 2008-10-24, 11:11
struct CompareObject
{
    bool operator()( const CX2WorldObjectPtr& left_, const CX2WorldObjectPtr& right_ )
    {
        return _wcsicmp( left_->GetName(), right_->GetName() ) < 0;
    }//operator()()
};//struct CompareObject


void CX2World::SortOnetime()
{
    std::sort( m_ObjectList.begin(), m_ObjectList.end(), CompareObject() );
    m_bObjectListSorted = true;

#ifdef _DEBUG // 2008-10-24
    //TRACEA( "[CX2World::SortOnetime]\r\n" );
    //UINT    uiSize = m_ObjectList.size();
    //for ( UINT ui = 0; ui < uiSize; ++ui )
    //{
    //    CX2WorldObject* pWorldObject = m_ObjectList[ui];
    //    TRACEA( "%S\r\n", pWorldObject->GetName() );
    //}//for
#endif // _DEBUG // 2008-10-24

}//CX2World::SortOnetime()
//}} seojt // 2008-10-24, 11:11


//{{ seojt // 2008-10-21, 17:27
CX2WorldObject* CX2World::FindObjectByName(const std::wstring& wstrName_)
{
    /** m_ObjectList를 sort하여 유지한 다음 binary search를 사용하도록 수정할 것
        - jintaeks on 2008-10-22, 11:12 */

    //UINT    uiSize = m_ObjectList.size();
    //for ( UINT ui = 0; ui < uiSize; ++ui )
    //{
    //    CX2WorldObject* pWorldObject = m_ObjectList[ui];
    //    if ( _wcsicmp(wstrName_.c_str(),pWorldObject->GetName() ) == 0 )
    //        return pWorldObject;
    //}//for


    UINT            uiSize = m_ObjectList.size();
    UINT            uiMid = uiSize / 2;
    UINT            uiBegin = 0;
    UINT            uiEnd   = uiSize - 1;
    //CX2WorldObject* pWorldObject = NULL;
    int             iCompareResult;

    if ( uiSize <= 0 )
        return NULL;

    while ( uiBegin <= uiEnd )
    {
        uiMid = uiBegin + (uiEnd - uiBegin) / 2;
        CX2WorldObjectPtr pWorldObject = m_ObjectList[uiMid];
        iCompareResult = _wcsicmp(wstrName_.c_str(),pWorldObject->GetName() );
        if ( iCompareResult < 0 )
            uiEnd = uiMid - 1;
        else if ( iCompareResult > 0 )
            uiBegin = uiMid + 1;
        else
            return pWorldObject.get();
    }//while

    return NULL;
}//CX2World::FindObjectByName()
//}} seojt // 2008-10-21, 17:27


bool CX2World::OpenScriptFile( const WCHAR* pFileName, KLuabinder* pKLuabinder /* = NULL */ )
{
#ifdef MODIFY_FRUSTUM
	g_pKTDXApp->GetDGManager()->SetNear(10.f);
	g_pKTDXApp->GetDGManager()->SetFar(50000.f);
	g_pKTDXApp->GetDGManager()->SetPerspectiveValue(4.f);
#endif

	KLuabinder* pRealKLuabinder = g_pKTDXApp->GetLuaBinder();


	lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pWorld", this );
	if( g_pX2Game != NULL )
	{
		lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pMajorParticle", g_pX2Game->GetMajorParticle() );
		lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pMinorParticle", g_pX2Game->GetMinorParticle() );
		lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pUIMajorParticle", g_pData->GetUIMajorParticle() );
		lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pUIMinorParticle", g_pData->GetUIMinorParticle() );
	}
	else
	{
		if( g_pData != NULL )
		{
			lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pMajorParticle", g_pData->GetUIMajorParticle() );
			lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pMinorParticle", g_pData->GetUIMinorParticle() );
			lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pUIMajorParticle", g_pData->GetUIMajorParticle() );
			lua_tinker::decl( pRealKLuabinder->GetLuaState(),  "g_pUIMinorParticle", g_pData->GetUIMinorParticle() );
		}
	}

    // 게임월드의 스크립트를 파싱해서, 게임 월드의 게임 객체들을 생성한다.
    // DoMemory()가 파싱한 스크립트 함수의 호출이 내부적으로 g_pKTDXApp->GetLuaBinder()를
    // 또 사용하고 있다는 사실을 주의하자.
    // - jintaeks on 2008-10-21, 10:04, qff
    if ( g_pKTDXApp->LoadAndDoMemory( pRealKLuabinder, pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR71, pFileName );

		return false;
    }


	//{{ seojt // 2008-10-24, 11:08
    // 메시 파일 이름으로 객체를 찾을 때, log(n)만에 검색하기 위해 
    // 메시 파일 이름으로 객체 리스트를 한 번 정렬해 둔다.
    // - jintaeks on 2008-10-24, 11:09
	SortOnetime();
	//}} seojt // 2008-10-24, 11:08



	//{{ dmlee 2009/07/28 현재는 전처리 파일을 사용하지 않아서 comment out 해둠, 나중에 사용할 수도 있음
//#ifdef BACKGROUND_LOADING_TEST // 2008-10-21
//	// 전처리된 파일이 있으면 그것을 읽어 들여서 정보를 설정한다.
//	// 전처리 파일의 이름은 "스크립트이름" + ".ppd"이다.
//	{
//		std::wstring wstrPreProcessingFile = pFileName;
//		wstrPreProcessingFile += L".ppd";
//		Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( wstrPreProcessingFile.c_str() );
//		if ( Info != NULL )
//		{
//			if ( pRealKLuabinder->DoMemoryNotEncrypt( Info->pRealData, Info->size ) == false )
//			{
//				string strFileName;
//				ConvertWCHARToChar( strFileName, pFileName );
//				ErrorLogMsg( XEM_ERROR71, strFileName.c_str() );
//
//				return false;
//			}//if
//		}//if
//	}//block
//#endif // BACKGROUND_LOADING_TEST // 2008-10-21
	//}} dmlee 2009/07/28 현재는 전처리 파일을 사용하지 않아서 comment out 해둠, 나중에 사용할 수도 있음


	SetMapDetail( g_pMain->GetGameOption().GetOptionList().m_MapDetail );

#ifdef MODIFY_FRUSTUM
	g_pKTDXApp->GetDGManager()->SetProjection( g_pKTDXApp->GetDGManager()->GetNear(), g_pKTDXApp->GetDGManager()->GetFar(), 
		g_pKTDXApp->GetDGManager()->GetPerspective(), 
		g_pKTDXApp->GetDGManager()->GetOrthoScale() );	
#endif

#ifdef X2TOOL
	m_wstrWorldName = pFileName;
#endif

	return true;
}


void CX2World::SetShowObject( bool bShow )
{
	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		pCKTDGSkyDome->SetShowObject( bShow );
	}

	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
		pCX2WorldObject->SetShowObject( bShow );
	}

	for( int i = 0; i < (int)m_pLensFlareList.size(); i++ )
	{
		CKTDGLensFlare* pObject = m_pLensFlareList[i];
		pObject->SetShowObject( bShow );
	}
	
#ifdef WORLD_LAYER
	for( int i = 0; i < (int)m_LayerList.size(); i++ )
	{
		CX2WorldLayer* pObject = m_LayerList[i];
		if( pObject->GetInitShow() == true )
			pObject->SetShowObject( bShow );
	}
#endif
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	m_bShowWorld = bShow;	// June 327fba60: the four loops above = March, then +0x1ac = bShow
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B
}

#ifdef X2TOOL
void CX2World::SetShowObjectByTool( bool bShow )
{
	SetMapDetail( g_pMain->GetGameOption().GetOptionList().m_MapDetail );

	for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
	{
		CKTDGSkyDome* pCKTDGSkyDome = m_SkyDomeList[i];
		if( pCKTDGSkyDome->m_bShowObject == true && pCKTDGSkyDome->GetShowObject() == true )
			pCKTDGSkyDome->SetShowObject( bShow );
		else
			pCKTDGSkyDome->SetShowObject( false );
	}

	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
		if( pCX2WorldObject->m_bShowObject == true && pCX2WorldObject->GetShowObject() == true )
			pCX2WorldObject->SetShowObject( bShow );
		else
			pCX2WorldObject->SetShowObject( false );		
	}

	for( int i = 0; i < (int)m_pLensFlareList.size(); i++ )
	{
		CKTDGLensFlare* pObject = m_pLensFlareList[i];
		pObject->SetShowObject( bShow );
	}

#ifdef WORLD_LAYER
	for( int i = 0; i < (int)m_LayerList.size(); i++ )
	{
		CX2WorldLayer* pObject = m_LayerList[i];
		if( pObject->GetInitShow() == true )
			pObject->SetShowObject( bShow );
	}
#endif

}
#endif

void CX2World::SetLightPos( float x, float y, float z )
{
	m_LightPos.x = x;
	m_LightPos.y = y;
	m_LightPos.z = z;
}

void CX2World::SetLightColor_LUA( float fRed, float fGreen, float fBlue )
{
	m_LightColor.r = fRed;
	m_LightColor.g = fGreen;
	m_LightColor.b = fBlue;
}

void CX2World::SetOriginColor_LUA( float fRed, float fGreen, float fBlue )
{
	m_OriginColor.r = fRed;
	m_OriginColor.g = fGreen;
	m_OriginColor.b = fBlue;

	m_FadeOriginColor = m_OriginColor;

	m_FadeColor		= m_OriginColor;
	m_NowColor		= m_OriginColor;
}

bool CX2World::SetLineMap_LUA( const char* pFileName )
{
	SAFE_DELETE_KTDGOBJECT( m_pLineMap );
	SAFE_DELETE_KTDGOBJECT( m_pLineMapFuture );


	wstring strBuf = L"";
	ConvertUtf8ToWCHAR( strBuf, pFileName );

	m_pLineMap = CKTDGLineMap::CreateLineMap();
	bool retval = m_pLineMap->LoadLineMap( strBuf );
#if defined(SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG) && defined(SERV_IRUHADEV_OFFLINE)
	// June effect C3x: a line map that fails to load leaves an empty map ( units fall, nothing spawns on a line ).
	if( false == retval )
		CX2OfflineLog::Server( L"JUNE_FX C3x line map %s failed to load", strBuf.c_str() );
#endif SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG
	
#if defined(WORLD_TOOL) || defined(X2TOOL)
	if( retval == false )
	{
		//WARNINGMSG( L"라인맵 파일 없음! 스크립트 안의 SetLineMap 항목 확인하세요." );
		strBuf = L"World_WorldTool_Default_linemap.lua";
		retval = m_pLineMap->LoadLineMap( strBuf );
		
	}
#endif WORLD_TOOL

	return retval;
}

#ifdef X2TOOL
bool CX2World::GetLineMapByTool( wstring wstrFileName )
{
	SAFE_DELETE_KTDGOBJECT( m_pLineMap );
	SAFE_DELETE_KTDGOBJECT( m_pLineMapFuture );

	m_pLineMap = CKTDGLineMap::CreateLineMap();
	bool retval = m_pLineMap->LoadLineMap( wstrFileName );

	if( retval == false )
	{
		//WARNINGMSG( L"라인맵 파일 없음! 스크립트 안의 SetLineMap 항목 확인하세요." );
		wstring strBuf = L"World_WorldTool_Default_linemap.lua";
		retval = m_pLineMap->LoadLineMap( strBuf );

	}

	return retval;
}
#endif

void CX2World::SetBGM_LUA( const char* pFileName )
{
	ConvertUtf8ToWCHAR( m_BGMName, pFileName );
}
void CX2World::StopAllEffectBGM()
{	
	if( true == m_vecEffectBGM.empty() )
		return;

	BOOST_FOREACH( CKTDXDeviceSound* pDeviceSound,  m_vecEffectBGM )
	{
		pDeviceSound->Stop();
	}
}
void CX2World::PlayAllEffectBGM()
{	
	if( true == m_vecEffectBGM.empty() )
		return;

	BOOST_FOREACH( CKTDXDeviceSound* pDeviceSound,  m_vecEffectBGM )
	{
		pDeviceSound->Play( true, false );
	}
}
void CX2World::AddEffectBGM_LUA( const char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );
	CKTDXDeviceSound* pDevice = g_pKTDXApp->GetDeviceManager()->OpenSound( fileName.c_str(), 10, false );
	if( NULL != pDevice )
	{
		pDevice->Play( true, false );
		m_vecEffectBGM.push_back( pDevice );
	}
}

void CX2World::Add3DEffectBGM_LUA( const char* pFileName, float x, float y, float z, float fMax3DDistance )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );
	
	CKTDXDeviceSound* pDevice = g_pKTDXApp->GetDeviceManager()->OpenSound( fileName.c_str(), 10, true, fMax3DDistance );
    if( NULL != pDevice )
    {
	    pDevice->Set3DPosition( D3DXVECTOR3(x, y, z) );

		// 배경 3D sound는 옵션이 켜져있을 때만 play 한다
		if( true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
			true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() &&
			true == g_pMain->GetGameOption().GetOptionList().m_bEnable3DSound )
		{
			pDevice->Play( true, true );
		}
    }

	if ( pDevice != NULL)
		m_vec3DEffectBGM.push_back( pDevice );
}


bool CX2World::SetWorldCamera_LUA( const char* pFileName )
{
#ifdef KEYFRAME_CAMERA
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );

#ifdef X2TOOL
	m_wstrWorldCamera = fileName;
#endif

	switch(g_pMain->GetNowStateID())
	{
	case CX2Main::XS_DUNGEON_GAME:
		{
			if(g_pX2Game != NULL)
			{
#ifdef INIT_FOCUS_UNIT
				g_pX2Game->InitFocusUnit();
#endif //INIT_FOCUS_UNIT
				return g_pX2Game->GetX2Camera()->LoadWorldCamera( fileName.c_str() );	
			}
		}
		break;

#ifdef ELSWORD_NEW_BEGINNING
	case CX2Main::XS_BEGINNING:
		{
			if(g_pBeginningGame != NULL)
				return g_pBeginningGame->GetX2Camera()->LoadWorldCamera( fileName.c_str() );	
		}
		break;
#endif ELSWORD_NEW_BEGINNING
	}
#endif KEYFRAME_CAMERA
	return false;
}
#ifdef KEYFRAME_CAMERA
void CX2World::UnLoadWorldCamera()
{
	switch(g_pMain->GetNowStateID())
	{
	case CX2Main::XS_DUNGEON_GAME:
		{
			if(g_pX2Game != NULL)
				g_pX2Game->GetX2Camera()->UnLoadWorldCamera();	
		}
		break;

#ifdef ELSWORD_NEW_BEGINNING
	case CX2Main::XS_BEGINNING:
		{
			if(g_pBeginningGame != NULL)
				g_pBeginningGame->GetX2Camera()->UnLoadWorldCamera();	
		}
		break;
#endif ELSWORD_NEW_BEGINNING
	}

	return;
}
#endif KEYFRAME_CAMERA


void CX2World::PlayEffectBGM( bool bPlay )
{
	for ( int i = 0; i < (int)m_vecEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vecEffectBGM[i];
		if ( pSound != NULL )
		{
			if( true == bPlay &&
				pSound->IsPlaying() == false )
			{
				pSound->Play( true, false );
			}
			else if ( false == bPlay && 
				pSound->IsPlaying() == true )
			{
				pSound->Stop();
			}
		}
	}
	
}



void CX2World::Play3DEffectBGM( bool bPlay )
{
	for ( int i = 0; i < (int)m_vec3DEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vec3DEffectBGM[i];
		if ( pSound != NULL )
		{
			if( true == bPlay &&
				pSound->IsPlaying() == false )
			{
				// 배경 3D sound는 옵션이 켜져있을 때만 play 한다
				if( true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
					true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() &&
					true == g_pMain->GetGameOption().GetOptionList().m_bEnable3DSound )
				{
					pSound->Play( true, true );
				}
			}
			else if( false == bPlay && 
				pSound->IsPlaying() == true )
			{
				pSound->Stop();
			}
		}
	}
}


void CX2World::SetEffectBGMVolume( float fSoundVolume )
{
	for ( int i = 0; i < (int)m_vecEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vecEffectBGM[i];
		if ( pSound != NULL )
		{
			pSound->SetVolume( fSoundVolume );
		}
	}

	for ( int i = 0; i < (int)m_vec3DEffectBGM.size(); i++ )
	{
		CKTDXDeviceSound* pSound = m_vec3DEffectBGM[i];
		if ( pSound != NULL )
		{
			pSound->SetVolume( fSoundVolume );
		}
	}

}

CX2WorldObjectParticle* CX2World::CreateObjectParticleDelay( CKTDGParticleSystem* pParticleSystem, const char* pszSequenceName, float fDelay )
{
	//CX2WorldObjectParticle* pCX2WorldObjectParticle = new CX2WorldObjectParticle( pParticleSystem, pszSequenceName );
	CX2WorldObjectParticlePtr pCX2WorldObjectParticle = CX2WorldObjectParticle::CreateWorldObjectParticle( pParticleSystem, pszSequenceName, fDelay );
	m_ObjectList.push_back( pCX2WorldObjectParticle );	
	m_bObjectListSorted = false;
	return pCX2WorldObjectParticle.get();
}

CX2WorldObjectParticle* CX2World::CreateObjectParticle( CKTDGParticleSystem* pParticleSystem, const char* pszSequenceName )
{
	//CX2WorldObjectParticle* pCX2WorldObjectParticle = new CX2WorldObjectParticle( pParticleSystem, pszSequenceName );
	CX2WorldObjectParticlePtr pCX2WorldObjectParticle = CX2WorldObjectParticle::CreateWorldObjectParticle( pParticleSystem, pszSequenceName, 0.f );
	m_ObjectList.push_back( pCX2WorldObjectParticle );	
    m_bObjectListSorted = false;
	return pCX2WorldObjectParticle.get();
}


CX2WorldObjectLightFlow* CX2World::CreateObjectLightFlow()
{
	//CX2WorldObjectLightFlow* pCX2WorldObjectLightFlow = new CX2WorldObjectLightFlow();
	CX2WorldObjectLightFlowPtr pCX2WorldObjectLightFlow = CX2WorldObjectLightFlow::CreateWorldObjectLightFlow( m_bBackgroundLoad );
	
	m_ObjectList.push_back( pCX2WorldObjectLightFlow );
    m_bObjectListSorted = false;
	g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2WorldObjectLightFlow.get() );

	return pCX2WorldObjectLightFlow.get();
}


CX2WorldObjectSkinMesh* CX2World::CreateObjectSkinMesh()
{
	//CX2WorldObjectSkinMesh* pCX2WorldObjectSkinMesh = new CX2WorldObjectSkinMesh();
	CX2WorldObjectSkinMeshPtr pCX2WorldObjectSkinMesh = CX2WorldObjectSkinMesh::CreateWorldObjectSkinMesh( m_bBackgroundLoad );
	m_ObjectList.push_back( pCX2WorldObjectSkinMesh );
    m_bObjectListSorted = false;
	g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2WorldObjectSkinMesh.get() );

	pCX2WorldObjectSkinMesh->SetLightPos( GetLightPos() );

	return pCX2WorldObjectSkinMesh.get();
}//CX2World::CreateObjectSkinMesh()


CX2WorldObjectMesh* CX2World::CreateObjectMesh()
{
#ifdef OPTIMIZATION_DEV_MODE

	return NULL;

#else

	//CX2WorldObjectMesh* pCX2WorldObjectMesh = new CX2WorldObjectMesh();
	CX2WorldObjectMeshPtr pCX2WorldObjectMesh = CX2WorldObjectMesh::CreateWorldObjectMesh( m_bBackgroundLoad );
	if ( pCX2WorldObjectMesh->GetMesh() != NULL )
		pCX2WorldObjectMesh->GetMesh()->SetLOD( m_LodPercent );

	m_ObjectList.push_back( pCX2WorldObjectMesh );
    m_bObjectListSorted = false;
	g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2WorldObjectMesh.get() );

	pCX2WorldObjectMesh->SetLightPos( GetLightPos() );

	return pCX2WorldObjectMesh.get();

#endif OPTIMIZATION_DEV_MODE
}


CKTDGSkyDome* CX2World::CreateSkyDome()
{
	//CKTDGSkyDome* pCKTDGSkyDome = new CKTDGSkyDome();
	CKTDGSkyDome* pCKTDGSkyDome = CKTDGSkyDome::CreateSkyDome();

	m_SkyDomeList.push_back( pCKTDGSkyDome );
    g_pKTDXApp->GetDGManager()->AddObjectChain( pCKTDGSkyDome );

	return pCKTDGSkyDome;
}


CKTDGLensFlare* CX2World::CreateLensFlare()
{
	//CKTDGLensFlare* pLensFlare = new CKTDGLensFlare( g_pKTDXApp->GetHWND(), g_pKTDXApp->GetDevice(),
	//									L"LensFlare_Halo.dds",
	//									L"LensFlare_01.dds",
	//									L"LensFlare_02.dds",
	//									L"LensFlare_03.dds" );

	CKTDGLensFlare* pLensFlare = CKTDGLensFlare::CreateLensFlare( g_pKTDXApp->GetHWND(), 
		L"LensFlare_Halo.dds",
		L"LensFlare_01.dds",
		L"LensFlare_02.dds",
		L"LensFlare_03.dds" );


	if( NULL != pLensFlare )
	{
		m_pLensFlareList.push_back( pLensFlare );
		g_pKTDXApp->GetDGManager()->AddObjectChain( pLensFlare );
	}

	return pLensFlare;
}

#ifdef WORLD_LAYER
CX2WorldLayer* CX2World::CreateWorldLayer()
{
    //CX2WorldLayer* pWorldlayer = new CX2WorldLayer( g_pKTDXApp->GetHWND(), g_pKTDXApp->GetDevice() );
	CX2WorldLayer* pWorldlayer = CX2WorldLayer::CreateWorldLayer( g_pKTDXApp->GetHWND() );
	pWorldlayer->SetLayerId( m_LayerList.size() );

    if( NULL != pWorldlayer )
    {
        m_LayerList.push_back( pWorldlayer );
        g_pKTDXApp->GetDGManager()->AddObjectChain( pWorldlayer );
    }

    return pWorldlayer;
}

void CX2World::RemoveWordLayer(int index)
{
    if(index < 0)
    {
        for( int i = 0; i < (int)m_LayerList.size(); i++ )
        {
            CX2WorldLayer* pWorldlayer = m_LayerList[i];
            //g_pKTDXApp->GetDGManager()->RemoveObjectChain(pWorldlayer);
            //SAFE_DELETE( pWorldlayer );
			CX2WorldLayer::DeleteKTDGObject( pWorldlayer );
			
        }
        m_LayerList.clear();
    }
    else
    {
        CX2WorldLayer* pWorldlayer = m_LayerList[index];
        if(pWorldlayer != NULL)
        {
            //g_pKTDXApp->GetDGManager()->RemoveObjectChain(pWorldlayer);
            //SAFE_DELETE( pWorldlayer );
			
			CX2WorldLayer::DeleteKTDGObject( pWorldlayer );

            m_LayerList.erase(m_LayerList.begin() + index);
        }        
    }

}
#endif

void CX2World::SetLOD( int lodPercent )
{
	if ( lodPercent > 100 )
		lodPercent = 100;

	if ( lodPercent < 0 )
		lodPercent = 0;

	m_LodPercent = lodPercent;

	for ( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pObject = m_ObjectList[i];
		if ( pObject == NULL )
			continue;

		if ( pObject->GetObjectType() == CX2WorldObject::OT_STATIC_MESH )
		{
			CX2WorldObjectMesh* pMeshObject = (CX2WorldObjectMesh*)pObject.get();
			if ( pMeshObject->GetMesh() != NULL )
			{
				pMeshObject->GetMesh()->SetLOD( m_LodPercent );
			}
		}
	}
}

void CX2World::ClearObjectMesh()
{
    m_ObjectList.clear();
}


#ifdef FOG_WORLD
void CX2World::SetFogWorld(float fNearX, float fFarX, float fNearY, float fFarY, float fDensity, D3DXCOLOR fogColor)
{
	m_bFog      = true;
	m_bFogShow  = true;
	m_fNearX    = fNearX;
	m_fFarX     = fFarX;
	m_fNearY    = fNearY;
	m_fFarY     = fFarY;
	m_fDensity  = fDensity;
	m_FogColor  = fogColor;
}
#endif

CX2WorldObject* CX2World::GetSyncObject(int index)
{
    if((int)m_vecSyncObject.size() > index)
    {
        return m_vecSyncObject[index];
    }
    else
        return NULL;
}

void CX2World::FrameMoveSyncObject(float fx, float fy)
{
}


//{{ seojt // 2008-10-24, 14:06
void CX2World::Debug_RenderHelp(int ix_,int iy_)
{
    int     ix = ix_ + 16;
    int     iy = iy_ + 16;
    WCHAR   wbuffer[1024];

    if ( m_pWorldData )
    {
        // World Data 정보 출력
        //
        StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),
            L"WorldID = %i\n"
            L"WorldName = %s\n"
            L"DataFileName = %s\n",
            m_pWorldData->worldID,
            m_pWorldData->worldName.c_str(),
            m_pWorldData->dataFileName.c_str() );
	    g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
		    CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
        iy += 16 * 4;

		// Culling 정보 출력
		//
		UINT    uiNumTotalObjects   = m_ObjectList.size();
		UINT    uiNumCulled         = 0;
		std::vector<CX2WorldObjectPtr>::iterator  vitor;
		for ( vitor = m_ObjectList.begin(); vitor != m_ObjectList.end(); ++vitor )
		{
			CX2WorldObjectPtr pWorldObject = *vitor;
			if ( pWorldObject->IsCulled() )
			{
				++uiNumCulled;
			}//if
		}//for

		StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),
			L"Culled = %i / %i", uiNumCulled, uiNumTotalObjects );
		g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
		iy += 16;


		//wstringstream wstrm;
		//for( UINT i=0; i<m_ObjectList.size(); i++ )
		//{
		//	CX2WorldObject* pWorldObject = m_ObjectList[i];
		//	if( NULL == pWorldObject )
		//		continue;

		//	wstrm << L"idx: " << i << ", "
		//		<< pWorldObject->GetName() << ", " 
		//		<< L"culled: " << pWorldObject->IsCulled() << ", "
		//		<< L"visible: " << pWorldObject->GetShowObject() << ", "
		//		<< L"center: ("
		//		<< (int)pWorldObject->GetCenter().x << ", "
		//		<< (int)pWorldObject->GetCenter().y << ", "
		//		<< (int)pWorldObject->GetCenter().z << "), "
		//		<< std::endl;
		//}

		//g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wstrm.str().c_str(), D3DXCOLOR( 1, 1, 0, 1 ), 
		//	CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
		//iy += 16;

    }//if
#ifdef SHOW_LINEMAP_FILE_NAME
	if( NULL != m_pLineMap )
	{
		StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),	L"LineMap = %s\n", m_pLineMap->GetLineFullName().c_str() );
		g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
		iy += 16 * 4;
	}
#endif //SHOW_LINEMAP_FILE_NAME
}//CX2World::Debug_RenderHelp()
//}} seojt // 2008-10-24, 14:06


#if defined(WORLD_TOOL) || defined(X2TOOL)
//{{ 2009.02.25 김태완 : GetWorldObjectList에서 받은 vector을 변경시킬 수 없는 관계로
//World에서 특정 오브젝트를 삭제하는 기능 추가 : 월드툴 용
void CX2World::RemoveWorldObject(CX2WorldObject* pTargetWorldObject)
{
	for( UINT i=0; i<m_ObjectList.size(); i++ )
	{
		CX2WorldObject* pWorldObject = m_ObjectList[i].get();
		if( pWorldObject == pTargetWorldObject )
		{
			m_ObjectList[i].reset(); 
			m_ObjectList.erase( m_ObjectList.begin()+i );
			//SAFE_DELETE( pTargetWorldObject );
			break;
		}
	}
}

void CX2World::RemoveSkyDome(int index)
{
	if(index < 0)
	{
		for( int i = 0; i < (int)m_SkyDomeList.size(); i++ )
		{
			CKTDGSkyDome* pSkyDome = m_SkyDomeList[i];
			CKTDGSkyDome::DeleteKTDGObject( pSkyDome );

		}
		m_SkyDomeList.clear();
	}
	else
	{
		CKTDGSkyDome* pSkyDome = m_SkyDomeList[index];
		if(pSkyDome != NULL)
		{
			CKTDGSkyDome::DeleteKTDGObject( pSkyDome );

			m_SkyDomeList.erase(m_SkyDomeList.begin() + index);
		}        
	}

}

void CX2World::AddWorldObject4Undo(CX2WorldObjectPtr pTargetWorldObjectPtr)
{
	if( pTargetWorldObjectPtr == NULL )
		return;

	switch( pTargetWorldObjectPtr->GetObjectType() )
	{
	case CX2WorldObject::OT_STATIC_MESH:
		{
			m_ObjectList.push_back( pTargetWorldObjectPtr );
			g_pKTDXApp->GetDGManager()->AddObjectChain( pTargetWorldObjectPtr.get() );
		}
		break;
	case CX2WorldObject::OT_SKIN_MESH:
		{			
			m_ObjectList.push_back( pTargetWorldObjectPtr );
			g_pKTDXApp->GetDGManager()->AddObjectChain( pTargetWorldObjectPtr.get() );
		}
		break;
	case CX2WorldObject::OT_PARTICLE:
		{
			m_ObjectList.push_back( pTargetWorldObjectPtr );
		}
		break;
	default:
		break;
	}
}

void CX2World::DeleteWorldObject4Undo(CX2WorldObject* pTargetWorldObject)
{
	if( pTargetWorldObject == NULL )
		return;

	for( UINT i=0; i<m_ObjectList.size(); i++ )
	{
		CX2WorldObject* pWorldObject = m_ObjectList[i].get();
		if( pWorldObject == pTargetWorldObject )
		{
			g_pKTDXApp->GetDGManager()->RemoveObjectChain( pWorldObject );
			m_ObjectList.erase( m_ObjectList.begin()+i );
			break;
		}
	}
}
//}}
#endif




void CX2World::AddWorldMonster_LUA()
{
#ifdef MARIO_LIKE_BLOCK_TEST

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	WorldMonsterData worldMonsterData;
	
	LUA_GET_VALUE( luaManager, "TEAM_NUMBER",		worldMonsterData.m_iTeamNumber,		-1 );
	if( worldMonsterData.m_iTeamNumber < 0 )
		return;

	LUA_GET_VALUE( luaManager, "NPC_ID",			worldMonsterData.m_iNPCID,			-1 );
	if( worldMonsterData.m_iNPCID < 0 )
		return; 

	LUA_GET_VALUE( luaManager, "LEVEL",				worldMonsterData.m_iLevel,			-1 );
	if( worldMonsterData.m_iLevel <= 0 )
		return; 


#ifdef  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    LUA_GET_USER_DEFINED_TYPE_VALUE( luaManager, "POSITION", worldMonsterData.m_vPosition, D3DXVECTOR3(0,0,0) );
#else   X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	// lua script에 값이 없는 경우가 있으면 lua_tinker::get() 사용하면 안됨
	worldMonsterData.m_vPosition = lua_tinker::get<D3DXVECTOR3>( luaManager.GetLuaState(), "POSITION" );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	m_vecWorldMonsterData.push_back( worldMonsterData );
#endif MARIO_LIKE_BLOCK_TEST

}

#ifdef WORLD_TRIGGER
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
CX2WorldObjectSkinMesh* CX2World::GetTriggerMesh(const wstring &wstrMeshName, int iTriggerId)
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
CX2WorldObjectSkinMesh* CX2World::GetTriggerMesh(const wstring &wstrMeshName)
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
{
	for( int i = 0; i < (int)m_ObjectList.size(); i++ )
	{
		CX2WorldObjectPtr pCX2WorldObject = m_ObjectList[i];
		
		if( pCX2WorldObject->GetObjectType() == CX2WorldObject::OT_SKIN_MESH )
		{
			CX2WorldObjectSkinMesh* pWorldMesh = (CX2WorldObjectSkinMesh *)pCX2WorldObject.get();
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
			// June 327fc680: a mesh is a candidate only when its trigger id is < 0 or equals the firing trigger's id.
			if( pWorldMesh->GetTriggerID() >= 0 && pWorldMesh->GetTriggerID() != iTriggerId )
			{
				// "[JUNE_ENGINE] WORLD mesh-id reject": logged once per (trigger, mesh id, name), only when the name matched.
				static std::set<wstring> s_setRejectLogged;
				wstring wstrWant = wstrMeshName;
				wstring wstrHave = pWorldMesh->GetXSkinMeshName();
				MakeUpperCase( wstrWant );
				MakeUpperCase( wstrHave );
				if( wstrWant == wstrHave )
				{
					WCHAR wszKey[32] = {0};
					StringCchPrintfW( wszKey, ARRAY_SIZE( wszKey ), L"%d/%d/", iTriggerId, pWorldMesh->GetTriggerID() );
					const wstring wstrKey = wstring( wszKey ) + wstrHave;
					if( s_setRejectLogged.insert( wstrKey ).second == true )
					{
						std::string strName;
						ConvertWCHARToChar( strName, wstrHave.c_str() );
						KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] WORLD mesh-id reject: trigger " << iTriggerId << " mesh " << strName.c_str()
							<< " carries trigger id " << pWorldMesh->GetTriggerID() << fileout;
					}
				}
				continue;
			}
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID

			wstring wstrMeshName1 = wstrMeshName;
			wstring wstrMeshName2 = pWorldMesh->GetXSkinMeshName();
			MakeUpperCase(wstrMeshName1);
			MakeUpperCase(wstrMeshName2);
			if( pWorldMesh != NULL && wstrMeshName1 == wstrMeshName2 )
				return pWorldMesh;			
		}
	}
	return NULL;	
}

#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
void CX2World::DoTriggerMesh(CKTDGLineMap::TRIGGER_DATA_MESH &triggerMesh, int iTriggerId)
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
void CX2World::DoTriggerMesh(CKTDGLineMap::TRIGGER_DATA_MESH &triggerMesh)
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
{	
	triggerMesh.m_bDone = true;
	
	if( triggerMesh.m_wstrMeshName.empty() )
		return;
	
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
	CX2WorldObjectSkinMesh *pWorldMesh = GetTriggerMesh( triggerMesh.m_wstrMeshName, iTriggerId );
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
	CX2WorldObjectSkinMesh *pWorldMesh = GetTriggerMesh( triggerMesh.m_wstrMeshName );
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
	if( pWorldMesh == NULL )
		return;

	switch(triggerMesh.m_iMeshTriggerAction)
	{
	case CKTDGLineMap::TAT_MESH_CHANGE_ANIM:
		{			
			pWorldMesh->ChangePlayAnim_LUA( triggerMesh.m_wstrName, (CX2WorldObjectSkinMesh::WORLD_OBJECT_SKIN_MESH_PLAY_TYPE)triggerMesh.m_iPlayType );			
		}
		break;
	case CKTDGLineMap::TAT_MESH_CHANGE_SPEED:
		{
			pWorldMesh->SetAnimSpeed( triggerMesh.m_fAnimSpeed );
		}
		break;
#ifdef SERV_IRUHADEV_RANOX_TRIGGER_MESH_MOVE
	case CKTDGLineMap::TAT_MESH_MOVE:
		{
			pWorldMesh->SetPos( triggerMesh.m_vMovePos );
		}
		break;
#endif SERV_IRUHADEV_RANOX_TRIGGER_MESH_MOVE
	default:
		break;
	}
}
void CX2World::DoTriggerLine(CKTDGLineMap::TRIGGER_DATA_LINE &triggerLine)
{
	triggerLine.m_bDone = true;

	if( triggerLine.m_iTriggerLineIndex < 0 )
		return;

	CKTDGLineMap::LineData *pLineData = GetLineMap()->AccessLineData( triggerLine.m_iTriggerLineIndex);
	if( pLineData == NULL )
		return;

	switch(triggerLine.m_iLineTriggerAction)
	{
	case CKTDGLineMap::TAT_LINE_CHANGE_ENABLE:
		{
			pLineData->bEnable	= triggerLine.m_bEnable;
		}
		break;
	case CKTDGLineMap::TAT_LINE_CHANGE_SPEED:
		{
			pLineData->m_fSpeed = triggerLine.m_fSpeed;
		}
		break;
	case CKTDGLineMap::TAT_LINE_CHANGE_JUMPSPEED:
		{
			pLineData->m_vJumpSpeed = triggerLine.m_vJumpSpeed;
		}
		break;
	case CKTDGLineMap::TAT_LINE_CHANGE_CAMERA:
		{
			pLineData->m_iCameraIndex = triggerLine.m_iCamera;
		}
		break;
	case CKTDGLineMap::TAT_LINE_CHANGE_CANPASS:
		{
			pLineData->m_bCanPass = triggerLine.m_bEnable;
		}
		break;
	case CKTDGLineMap::TAT_LINE_CHANGE_TELEPORT:
		{
			pLineData->m_iTeleportLineIndex = triggerLine.m_iCamera;
		}
		break;
#ifdef SERV_IRUHADEV_RANOX_TRIGGER_LINE_CHANGE_TYPE
	case CKTDGLineMap::TAT_LINE_CHANGE_TYPE:
		{
			pLineData->lineType = triggerLine.m_iChangeLineType;
		}
		break;
#endif SERV_IRUHADEV_RANOX_TRIGGER_LINE_CHANGE_TYPE
#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
	case CKTDGLineMap::TAT_LINE_CHANGE_MOVABLE_DATA:
		{
			/// June 328004a0 case 8: STOP ( the +0xc byte ) to the line and its followers ( 31c6fb50 ), then the new rows if any ( 31c75410 )
			GetLineMap()->SetMovableLineStop( triggerLine.m_iTriggerLineIndex, triggerLine.m_bEnable, false );
			if ( false == triggerLine.m_vecDirectMoveData.empty() )
				GetLineMap()->SetMovableLineDirectMoveData( triggerLine.m_iTriggerLineIndex, triggerLine.m_vecDirectMoveData );
		}
		break;
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE
	default:
		break;
	}
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE
	/// June DoTriggerLine tail 328005b9-328007c0, after every action: AUTO_DISABLE_LINE marks the line ( +0x128 = 1 ) and lists every user / NPC whose
	/// land line is this line ( 31c7bb00 from GetPos( false ), radius 30, seeded with GetLastTouchLineIndex( false ); compared with LineData +0, lineIndex ).
	/// June's 31c7bb00 takes one more bool ( false for users, true for NPCs: LT_MONSTER_ROAD lines count only for NPCs ); March's GetLandPosition has none.
	if ( true == triggerLine.m_bAutoDisableLine && NULL != GetLineMap() && NULL != g_pX2Game )
	{
		pLineData->m_bAutoDisable = true;

		for ( int i = 0; i < g_pX2Game->GetUserUnitListSize(); ++i )
		{
			CX2GUUser* pUser = g_pX2Game->GetUserUnit( i );
			if ( NULL == pUser )
				continue;

			int iLandLine = pUser->GetLastTouchLineIndex( false );
#ifdef SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
			GetLineMap()->GetLandPosition( pUser->GetPos( false ), false, LINE_RADIUS, &iLandLine );	/// June @32800694: bMonster false ( W1c ); the NPC loop keeps true
#else SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
			GetLineMap()->GetLandPosition( pUser->GetPos( false ), LINE_RADIUS, &iLandLine );
#endif SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD
			if ( pLineData->lineIndex == iLandLine )
				pLineData->m_vecAutoDisableUserUID.push_back( pUser->GetUnitUID() );
		}

		for ( int i = 0; i < g_pX2Game->GetNPCUnitListSize(); ++i )
		{
			CX2GUNPC* pNPC = g_pX2Game->GetNPCUnit( i );
			if ( NULL == pNPC )
				continue;

			int iLandLine = pNPC->GetLastTouchLineIndex( false );
			GetLineMap()->GetLandPosition( pNPC->GetPos( false ), LINE_RADIUS, &iLandLine );
			if ( pLineData->lineIndex == iLandLine )
				pLineData->m_vecAutoDisableNPCUID.push_back( (int)pNPC->GetUnitUID() );
		}

		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] WORLD auto-disable: line " << triggerLine.m_iTriggerLineIndex << " marked, users listed "
			<< (int)pLineData->m_vecAutoDisableUserUID.size() << ", npcs listed " << (int)pLineData->m_vecAutoDisableNPCUID.size() << fileout;
	}
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE
}

#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
void CX2World::DoTriggerEtc(CKTDGLineMap::TRIGGER_DATA_ETC &triggerEtc, int iTriggerId)
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
void CX2World::DoTriggerEtc(CKTDGLineMap::TRIGGER_DATA_ETC &triggerEtc)
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
{
	triggerEtc.m_bDone = true;

	switch(triggerEtc.m_iEtcTriggerAction)
	{
	case CKTDGLineMap::TAT_ETC_PLAY_SOUND:
		{
			CKTDXDeviceSound* pDevice = g_pKTDXApp->GetDeviceManager()->OpenSound( triggerEtc.m_wstrActionName, 10, false );
			if( NULL != pDevice )
			{
				pDevice->Play( false, false );
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_SOUND
				// June 32800e82-32800edb: 31cdd4e0( device channel ) when the channel is not NULL.
				if( pDevice->GetChannel() != NULL )
					g_pKTDXApp->GetDeviceManager()->RegisterAutoRelease( pDevice->GetChannel() );
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_SOUND
			}
		}
		break;
	case CKTDGLineMap::TAT_ETC_DURATION_TIME:
		{
		}
		break;
	case CKTDGLineMap::TAT_ETC_CAMERA:
		{
			SetTriggerCamera(triggerEtc.m_iDummyValue01, triggerEtc.m_fDummyValue01);
		}
		break;
	case CKTDGLineMap::TAT_ETC_CAMERA_SHAKE:
		{
			if( g_pX2Game != NULL && g_pX2Game->GetX2Camera() != NULL )
			{
				if( triggerEtc.m_iDummyValue01 >= 0 )
				{
					g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( (float)triggerEtc.m_iDummyValue01, triggerEtc.m_fDummyValue01 );
				}
				else
				{
					float fGap = (float)abs(triggerEtc.m_iDummyValue01);
					g_pX2Game->GetX2Camera()->GetCamera().LeftRightCrashCamera( fGap, triggerEtc.m_fDummyValue01 );
				}
			}
		}
		break;
	case CKTDGLineMap::TAT_ETC_ACTIVE_LAYER:
		{
#ifdef WORLD_LAYER
			for(UINT i=0; i<m_LayerList.size(); ++i)
			{
				CX2WorldLayer *pWorldLayer = m_LayerList[i];
				if( pWorldLayer != NULL )
				{
					int iLayerId = pWorldLayer->GetLayerId();
					if( iLayerId >= 0 && iLayerId == triggerEtc.m_iDummyValue01 )
					{
						pWorldLayer->FadeColor( pWorldLayer->GetColor(), triggerEtc.m_fDummyValue01 );
						pWorldLayer->SetShowObject(true);
					}
				}
			}
#endif
		}
		break;
#ifdef SERV_IRUHADEV_RANOX_TRIGGER_NPC_AI_ON_OFF
	case CKTDGLineMap::TAT_ETC_NPC_AI_ON_OFF:
		{
			if( g_pX2Game != NULL )
			{
				for( int i = 0; i < g_pX2Game->GetNPCUnitListSize(); ++i )
				{
					CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnit( i );
					if( pGUNPC != NULL )
						pGUNPC->SetAIEnable( triggerEtc.m_iDummyValue01 != 0 );
				}
			}
		}
		break;
#endif SERV_IRUHADEV_RANOX_TRIGGER_NPC_AI_ON_OFF
#ifdef SERV_IRUHADEV_RANOX_TRIGGER_PLAY_EFFECTSET
	case CKTDGLineMap::TAT_ETC_PLAY_EFFECTSET:
		{
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_EFFECTSET
			// June 2017 DoTriggerEtc case 7 (328012b5-32801484, funcmap "Engine W0"): the value is "<mesh> <effect set>", split at the
			// first space (no space -> nothing). The set plays from the first user unit at the mesh's position (32802ae0 = object
			// matrix position, through its manager object: CKTDGObject::GetMatrix does the same). June's trailing float (1.0) is
			// fForceDownValueRate, which March's PlayEffectSet does not take.
			{
				const size_t uiSpace = triggerEtc.m_wstrActionName.find( L" " );
				if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL && uiSpace != wstring::npos )
				{
					const wstring wstrMeshName = triggerEtc.m_wstrActionName.substr( 0, uiSpace );
					const wstring wstrEffectSetName = triggerEtc.m_wstrActionName.substr( uiSpace + 1 );
					CX2WorldObjectSkinMesh* pTriggerMesh = GetTriggerMesh( wstrMeshName, iTriggerId );
					CX2GUUser* pUser = NULL;
					for( int iUser = 0; pUser == NULL && iUser < g_pX2Game->GetUserUnitListSize(); ++iUser )
						pUser = g_pX2Game->GetUserUnit( iUser );
					if( pTriggerMesh != NULL && pUser != NULL )
					{
						const D3DXVECTOR3 vMeshPos = pTriggerMesh->GetPos();
						g_pX2Game->GetEffectSet()->PlayEffectSet( wstrEffectSetName, pUser, NULL, false, -1.f, -1.f,
							D3DXVECTOR3( 1, 1, 1 ), true, vMeshPos, pUser->GetRotateDegree(), D3DXVECTOR3( 0, 0, 0 ) );

						std::string strValue;
						ConvertWCHARToChar( strValue, triggerEtc.m_wstrActionName.c_str() );
						KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] WORLD effectset: trigger " << iTriggerId << " played '" << strValue.c_str()
							<< "' at " << vMeshPos.x << "," << vMeshPos.y << "," << vMeshPos.z << fileout;
					}
				}
			}
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_EFFECTSET
			if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL && g_pX2Game->GetMyUnit() != NULL &&
				false == triggerEtc.m_wstrActionName.empty() )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( triggerEtc.m_wstrActionName, g_pX2Game->GetMyUnit(), NULL, false, -1.f, -1.f,
					D3DXVECTOR3( 1, 1, 1 ), true, triggerEtc.m_vEffectPosition );
			}
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_EFFECTSET
		}
		break;
#endif SERV_IRUHADEV_RANOX_TRIGGER_PLAY_EFFECTSET
#ifdef SERV_IRUHADEV_RANOX_TRIGGER_TALK_BOX
	case CKTDGLineMap::TAT_ETC_TALK_BOX:
		{
#ifdef SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER
			// June 2017 DoTriggerEtc (0x32800e10) case 6 shows the box only while the dungeon game is the state.
			if( g_pMain != NULL && CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() &&
				g_pX2Game != NULL && g_pX2Game->GetTalkBoxManager() != NULL )
#else  SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER
			if( g_pX2Game != NULL && g_pX2Game->GetTalkBoxManager() != NULL )
#endif SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER
			{
				CX2TalkBoxManagerImp::TalkBox talkBox;
				talkBox.m_OwnerUnitUID		= -1;
				talkBox.m_wstrTalkContent	= GET_STRING( triggerEtc.m_iDummyValue01 );
				talkBox.m_bTraceUnit		= true;
				talkBox.m_TalkBoxType		= CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT;
#ifdef SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER
				// June sets these two too: TalkBox +0x3c (m_fRemainTime) = REMAIN_TIME, +0xa0 (m_bUseOutLine) = 1.
				talkBox.m_fRemainTime		= triggerEtc.m_fDummyValue01;
				talkBox.m_bUseOutLine		= true;
#endif SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER
#ifdef ADDED_RELATIONSHIP_SYSTEM
				talkBox.m_vTalkBoxPosition	= triggerEtc.m_vTalkBoxPosition;
#endif ADDED_RELATIONSHIP_SYSTEM
				g_pX2Game->GetTalkBoxManager()->Push( talkBox );
			}
		}
		break;
#endif SERV_IRUHADEV_RANOX_TRIGGER_TALK_BOX
	default:
		break;
	}
}

void CX2World::DoTriggerFrameMove( double fTime, float fElapsedTime )
{
	if( GetLineMap() == NULL )
		return;

	CKTDGLineMap::LINEMAP_TRIGGER *lineMapTrigger = GetLineMap()->GetLineMapTrigger();
	if( lineMapTrigger->m_vecNowTrigger.size() <= 0 || lineMapTrigger->m_vecTriggerSeg.size() <= 0 )
		return;
	
	for(UINT iActiveTrigger = 0; iActiveTrigger<lineMapTrigger->m_vecNowTrigger.size(); ++iActiveTrigger)
	{	
		bool bTriggerDataDone = true;
		int iActiveTriggerIndex = lineMapTrigger->m_vecNowTrigger[iActiveTrigger];
		CKTDGLineMap::TRIGGER_SEGMENT &triggerSeg = lineMapTrigger->m_vecTriggerSeg[iActiveTriggerIndex];

		if( triggerSeg.m_iTriggerDataIndex < 0 )
		{
			lineMapTrigger->m_vecNowTrigger.erase( lineMapTrigger->m_vecNowTrigger.begin() + iActiveTrigger );
			--iActiveTrigger;			
			continue;
		}

		CKTDGLineMap::TRIGGER_DATA &triggerData = triggerSeg.m_vecTriggerData[triggerSeg.m_iTriggerDataIndex];

		triggerData.m_fElapsedTime += fElapsedTime;

		UINT iTriggerDataIndex = 0;
		for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerMesh.size(); ++iTriggerDataIndex)
		{
			CKTDGLineMap::TRIGGER_DATA_MESH &trigger = triggerData.m_vecTriggerMesh[iTriggerDataIndex];
			if( trigger.m_bDone == false &&
				triggerData.m_fElapsedTime >= trigger.m_fTriggerTime &&
				trigger.m_iMeshTriggerAction > CKTDGLineMap::TAT_MESH_NONE )
			{
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
				DoTriggerMesh(trigger, triggerSeg.m_iTriggerId);
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
				DoTriggerMesh(trigger);
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
			}
			bTriggerDataDone = bTriggerDataDone && trigger.m_bDone;
		}
		for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerLine.size(); ++iTriggerDataIndex)
		{
			CKTDGLineMap::TRIGGER_DATA_LINE &trigger = triggerData.m_vecTriggerLine[iTriggerDataIndex];
			if( trigger.m_bDone == false &&
				triggerData.m_fElapsedTime >= trigger.m_fTriggerTime &&
				trigger.m_iLineTriggerAction > CKTDGLineMap::TAT_LINE_NONE )
			{
				DoTriggerLine(trigger);
			}
			bTriggerDataDone = bTriggerDataDone && trigger.m_bDone;	
		}
		for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerEtc.size(); ++iTriggerDataIndex)
		{
			CKTDGLineMap::TRIGGER_DATA_ETC &trigger = triggerData.m_vecTriggerEtc[iTriggerDataIndex];
			if( trigger.m_bDone == false &&
				triggerData.m_fElapsedTime >= trigger.m_fTriggerTime &&
				trigger.m_iEtcTriggerAction > CKTDGLineMap::TAT_ETC_NONE )
			{
#ifdef SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
				DoTriggerEtc(trigger, triggerSeg.m_iTriggerId);
#else SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
				DoTriggerEtc(trigger);
#endif SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID
			}
			bTriggerDataDone = bTriggerDataDone && trigger.m_bDone;
		}

		if( bTriggerDataDone == true )
		{
			InitTriggerDone(triggerData);
			lineMapTrigger->m_vecNowTrigger.erase( lineMapTrigger->m_vecNowTrigger.begin() + iActiveTrigger );
			--iActiveTrigger;			
			if( triggerSeg.m_iTriggerDataIndex < (int)triggerSeg.m_vecTriggerData.size() - 1)
			{
				++triggerSeg.m_iTriggerDataIndex;
			}
			else
			{
				if( triggerSeg.m_bLoop == true )
					triggerSeg.m_iTriggerDataIndex = 0;
				else
					triggerSeg.m_iTriggerDataIndex = -1;
			}			
		}
	}
}

void CX2World::InitTriggerDone(CKTDGLineMap::TRIGGER_DATA &triggerData, bool bDone)
{
	triggerData.m_fElapsedTime = 0;	
	
	UINT iTriggerDataIndex = 0;
	for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerMesh.size(); ++iTriggerDataIndex)
	{
		CKTDGLineMap::TRIGGER_DATA_MESH &trigger = triggerData.m_vecTriggerMesh[iTriggerDataIndex];
		trigger.m_bDone = bDone;
	}
	for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerLine.size(); ++iTriggerDataIndex)
	{
		CKTDGLineMap::TRIGGER_DATA_LINE &trigger = triggerData.m_vecTriggerLine[iTriggerDataIndex];
		trigger.m_bDone = bDone;
	}
	for(iTriggerDataIndex = 0; iTriggerDataIndex<triggerData.m_vecTriggerEtc.size(); ++iTriggerDataIndex)
	{
		CKTDGLineMap::TRIGGER_DATA_ETC &trigger = triggerData.m_vecTriggerEtc[iTriggerDataIndex];
		trigger.m_bDone = bDone;	
	}
}
#endif //WORLD_TRIGGER


#ifdef MODIFY_FRUSTUM
void CX2World::SetProjection( float fNear, float fFar, float fPerspective, float fBaseDistance, float fStepDist1, float fStepDist2, float fStepDist3 )
{
	g_pKTDXApp->GetDGManager()->SetNear( fNear );
	g_pKTDXApp->GetDGManager()->SetFar( fFar );
	g_pKTDXApp->GetDGManager()->SetPerspectiveValue( fPerspective );

	m_fBaseCameraDistance = fBaseDistance;
	m_fCameraDistanceStep[0] = fStepDist1;
	m_fCameraDistanceStep[1] = fStepDist2;
	m_fCameraDistanceStep[2] = fStepDist3;
}
#endif //MODIFY_FRUSTUM

#ifdef SERV_IRUHADEV_JUNE_WORLD_MESH_BY_NAME
/** @function : GetWorldObjectMeshByName
	@brief : June 327fcc40 ( bound @31dbab2e ): over the world objects ( +0xe8 ), the first whose type ( +0x194 ) is 1
			 ( OT_STATIC_MESH ) and whose name ( +0x19c, m_wstrObjectName: SetXMesh_LUA names it after the mesh file ) equals
			 the argument, both lower-cased ( 31c20bf0 ) before the compare; NULL when none.
*/
CX2WorldObjectMesh* CX2World::GetWorldObjectMeshByName( const char* pszName_ )
{
	wstring wstrName = L"";
	if ( NULL != pszName_ )
		ConvertUtf8ToWCHAR( wstrName, pszName_ );

	CX2WorldObjectMesh* pFound = NULL;
	for ( unsigned i = 0; i < m_ObjectList.size(); i++ )
	{
		CX2WorldObject* pObject = m_ObjectList[i].get();
		if ( NULL == pObject || CX2WorldObject::OT_STATIC_MESH != pObject->GetObjectType() )
			continue;
		if ( 0 == _wcsicmp( pObject->GetName(), wstrName.c_str() ) )
		{
			pFound = static_cast<CX2WorldObjectMesh*>( pObject );
			break;
		}
	}
#if defined( SERV_IRUHADEV_JUNE_C3Z4_DEBUG ) && defined( SERV_IRUHADEV_OFFLINE )
	static int s_iLogged = 0;
	if ( s_iLogged < 10 )
	{
		++s_iLogged;
		CX2OfflineLog::Server( L"JUNE_FX C3z-4b GetWorldObjectMeshByName %s -> %d", wstrName.c_str(), NULL != pFound ? 1 : 0 );
	}
#endif // SERV_IRUHADEV_JUNE_C3Z4_DEBUG && SERV_IRUHADEV_OFFLINE
	return pFound;
}
#endif SERV_IRUHADEV_JUNE_WORLD_MESH_BY_NAME

#ifdef SERV_IRUHADEV_JUNE_MOVABLE_LINE
#ifndef SERV_IRUHADEV_JUNE_WORLD_MESH_BY_NAME
#error SERV_IRUHADEV_JUNE_MOVABLE_LINE calls GetWorldObjectMeshByName ( June 327fcc40 ), defined under SERV_IRUHADEV_JUNE_WORLD_MESH_BY_NAME
#endif SERV_IRUHADEV_JUNE_WORLD_MESH_BY_NAME
/** June 327fca00: GetWorldObjectMeshByName's twin for OT_SKIN_MESH ( type 2 ), comparing the skin mesh file name
	( +0x1f8, m_XSkinMeshName ), both lower-cased ( 31c20bf0 ); NULL when none */
CX2WorldObjectSkinMesh* CX2World::GetWorldObjectSkinMeshByName( const char* pszName_ )
{
	wstring wstrName = L"";
	if ( NULL != pszName_ )
		ConvertUtf8ToWCHAR( wstrName, pszName_ );

	for ( unsigned i = 0; i < m_ObjectList.size(); i++ )
	{
		CX2WorldObject* pObject = m_ObjectList[i].get();
		if ( NULL == pObject || CX2WorldObject::OT_SKIN_MESH != pObject->GetObjectType() )
			continue;
		CX2WorldObjectSkinMesh* pSkinMesh = static_cast<CX2WorldObjectSkinMesh*>( pObject );
		if ( 0 == _wcsicmp( pSkinMesh->GetXSkinMeshName().c_str(), wstrName.c_str() ) )
			return pSkinMesh;
	}
	return NULL;
}

/** June 32801ab0 ( from OnFrameMove for OT_SKIN_MESH while the line map has records ), matched on the mesh file ( +0x1f8 ).
	1) every LMT_TARGET_MESH record on it ( 31c76a00 ) that is not stopped: its line centre goes to the bone ( TARGET_BONE_NAME on the
	   motion mesh, +0x220: 327f69f0 = FindFrame( name )->combineMatrix._41-_43 at +0xc0, ( 0, 0, 0 ) when absent ), and the record keeps
	   that move and the anim time ( +0x218 -> +0x214; only P2P 0x1c reads it, and guests drop type 1 ).
	2) the LMT_PATROL / LMT_SPIN record on it ( 31c74fe0, a copy ), not stopped: the mesh goes onto the line centre
	   ( USE_ABSOLUTE_COORDINATE ) or moves by the line's last move. */
void CX2World::FrameMoveMovableLineSkinMesh( CX2WorldObjectSkinMesh* pSkinMesh_ )
{
	CKTDGLineMap* pLineMap = m_pLineMap;
	if ( NULL == pLineMap || NULL == pSkinMesh_ )
		return;
	const wstring wstrMeshName = pSkinMesh_->GetXSkinMeshName();

	CKTDGLineMap::MovableLineDataMap mapTargetMesh;
	if ( true == pLineMap->GetTargetMeshMovableLineData( wstrMeshName, mapTargetMesh ) )
	{
		for ( CKTDGLineMap::MovableLineDataMap::iterator it = mapTargetMesh.begin(); it != mapTargetMesh.end(); ++it )
		{
			if ( true == it->second.m_bStop )
				continue;

			D3DXVECTOR3 vBonePos( 0.f, 0.f, 0.f );
			CKTDXDeviceXSkinMesh* pMotionMesh = pSkinMesh_->GetXSkinMotionMesh();
			if ( NULL != pMotionMesh )
			{
				std::string strBoneName;
				ConvertWCHARToChar( strBoneName, it->second.m_wstrTargetBoneName );
				const CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pMotionMesh->FindFrame( strBoneName.c_str() );
				if ( NULL != pFrame )
					vBonePos = D3DXVECTOR3( pFrame->combineMatrix._41, pFrame->combineMatrix._42, pFrame->combineMatrix._43 );
			}

			D3DXVECTOR3 vMove( 0.f, 0.f, 0.f );
			pLineMap->MoveLineCenterTo( it->first, vBonePos, vMove );
			CKTDGLineMap::MovableLineData* pData = pLineMap->GetMovableLineData( it->first );
			if ( NULL != pData )
			{
				pData->m_vLastMove = vMove;
				if ( NULL != pSkinMesh_->GetXSkinAnim() )	/// June reads +0x218 -> +0x214 with no NULL test
					pData->m_fNowTime = pSkinMesh_->GetXSkinAnim()->GetNowAnimationTime();
			}
		}
	}

	CKTDGLineMap::MovableLineData kData;
	int iLineIndex = -1;
	if ( false == pLineMap->GetMovableLineDataByMeshName( wstrMeshName, kData, iLineIndex ) || true == kData.m_bStop )
		return;

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	if ( true == kData.m_bUseAbsoluteCoordinate )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );
		if ( NULL == pLineData )
			return;
		vPos = CKTDGLineMap::GetLineCenter( pLineData );
	}
	else
	{
		if ( false == kData.m_bInitPos )	/// June @32801dc5: on the copy, so every frame ( dead in effect )
		{
			kData.m_vInitPos = pSkinMesh_->GetPos();
			kData.m_bInitPos = true;
		}
		vPos = pSkinMesh_->GetPos() + kData.m_vLastMove;
	}
	pSkinMesh_->SetPos( vPos );	/// June 321f5270
}

/** June 327fe640 ( OT_STATIC_MESH ): part 2 of the skin mesh one on the mesh file ( +0x1f8, m_XMeshName ), and June does not
	test STOP here: a stopped or finished line's mesh keeps moving by the line's last move ( JUNE_BUGS ) */
void CX2World::FrameMoveMovableLineMesh( CX2WorldObjectMesh* pMesh_ )
{
	CKTDGLineMap* pLineMap = m_pLineMap;
	if ( NULL == pLineMap || NULL == pMesh_ )
		return;

	CKTDGLineMap::MovableLineData kData;
	int iLineIndex = -1;
	if ( false == pLineMap->GetMovableLineDataByMeshName( pMesh_->GetXMeshName(), kData, iLineIndex ) )
		return;

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	if ( true == kData.m_bUseAbsoluteCoordinate )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );
		if ( NULL == pLineData )
			return;
		vPos = CKTDGLineMap::GetLineCenter( pLineData );
	}
	else
	{
		vPos = pMesh_->GetPos() + kData.m_vLastMove;
	}
	pMesh_->SetPos( vPos );	/// June 321f5270
}

/** June 328018c0 ( the end of OnFrameMove ): the host, once its 1 s timer ( +0x1b0 ) is up, broadcasts one P2P 0x1c per record:
	LMT_TARGET_MESH sends line, type and time ( June leaves the rest of its stack packet unset; zeroed here, guests drop type 1 ),
	LMT_PATROL / LMT_SPIN add the row and m_vMovedPos */
void CX2World::SendMovableLineSync( float fElapsedTime_ )
{
	if ( NULL == g_pX2Game || false == g_pX2Game->IsHost() )
		return;

	m_fMovableLineSyncTime += fElapsedTime_;
	if ( m_fMovableLineSyncTime < 1.f )
		return;
	m_fMovableLineSyncTime = 0.f;

	if ( NULL == m_pLineMap || NULL == g_pData->GetGameUDP() )
		return;

	const CKTDGLineMap::MovableLineDataMap mapData = m_pLineMap->GetMovableLineDataMap();	/// June copies the map ( 32803a80 )
	for ( CKTDGLineMap::MovableLineDataMap::const_iterator it = mapData.begin(); it != mapData.end(); ++it )
	{
		const CKTDGLineMap::MovableLineData& kData = it->second;
		KXPT_MOVABLE_LINE_SYNC kPacket;
		ZeroMemory( &kPacket, sizeof(kPacket) );
		kPacket.m_usLineIndex	= static_cast<USHORT>( it->first );
		kPacket.m_cMoveType		= static_cast<char>( kData.m_iMoveType );
		kPacket.m_fNowTime		= kData.m_fNowTime;
		if ( CKTDGLineMap::LMT_PATROL == kData.m_iMoveType || CKTDGLineMap::LMT_SPIN == kData.m_iMoveType )
		{
			kPacket.m_byNowIndex	= static_cast<BYTE>( kData.m_uiNowIndex );
			kPacket.m_vMovedPos		= kData.m_vMovedPos;
		}
		else if ( CKTDGLineMap::LMT_TARGET_MESH != kData.m_iMoveType )
			continue;
		g_pData->GetGameUDP()->BroadCast( XPT_MOVABLE_LINE_SYNC, &kPacket, sizeof(kPacket) );	/// June 33223fb0( 0x1c, &packet, 0x14 )
	}
}

/** June 32801fb0 ( CX2Game's P2P case 0x1c ): LMT_PATROL / LMT_SPIN only; the line takes the host's phase ( 31c6ffc0 ), then the
	mesh on it ( skin mesh first, 327fca00, else static, 327fcc40 ) runs its follow once more ( an extra move on that frame, June ) */
void CX2World::ReceiveMovableLineSync( const KXPT_MOVABLE_LINE_SYNC* pkSync_ )
{
	if ( NULL == pkSync_ || 1 == pkSync_->m_cMoveType || static_cast<BYTE>( pkSync_->m_cMoveType - 2 ) > 1 )
		return;
	if ( NULL == m_pLineMap )
		return;

	wstring wstrMeshName = L"";
	m_pLineMap->SyncMovableLine( pkSync_->m_usLineIndex, pkSync_->m_byNowIndex, pkSync_->m_fNowTime, pkSync_->m_vMovedPos, wstrMeshName );
	if ( true == wstrMeshName.empty() )
		return;

	std::string strMeshName;
	ConvertWCHARToChar( strMeshName, wstrMeshName );	/// June 31ecc840 ( wide -> narrow ); mesh file names are ASCII
	CX2WorldObjectSkinMesh* pSkinMesh = GetWorldObjectSkinMeshByName( strMeshName.c_str() );
	if ( NULL != pSkinMesh )
	{
		FrameMoveMovableLineSkinMesh( pSkinMesh );
		return;
	}
	CX2WorldObjectMesh* pMesh = GetWorldObjectMeshByName( strMeshName.c_str() );
	if ( NULL != pMesh )
		FrameMoveMovableLineMesh( pMesh );
}
#endif SERV_IRUHADEV_JUNE_MOVABLE_LINE
#ifdef SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY
// June effect completion C3n: CX2World::SetGravityAndSpeedRate ( June 32800800; March's was an inline store in X2World.h ). June stores the rate
// ( world +0x184 ), then, only in a GT_DUNGEON game ( GetGameType, vslot 0x10c, == 2: not battlefield 3, not PvE 4 ), dynamic_casts the game to
// CX2DungeonGame and walks its unit list ( 327cab80: +0x304..+0x308, NULL entries skipped ), calling ApplyJuneWorldGravityRate ( 32296290 ) on
// each unit, users and NPCs alike. Lua-bound ( X2Main.cpp class_def ); no March or June script calls it ( C3m ).
void CX2World::SetGravityAndSpeedRate( float val )
{
	m_fGravityAndSpeedRate = val;

	int iGameType = -1;		// log only: -1 = no game
	int iUnitNum = -1;		// log only: -1 = not re-applied ( no game, not GT_DUNGEON, or the cast failed )
	if ( NULL != g_pX2Game )
	{
		iGameType = (int) g_pX2Game->GetGameType();
		if ( CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
		{
			CX2DungeonGame* pDungeonGame = dynamic_cast<CX2DungeonGame*>( g_pX2Game );
			if ( NULL != pDungeonGame )
			{
				const vector<CX2GameUnit*>& vecUnitList = pDungeonGame->GetUnitList();
				for ( UINT i = 0; i < vecUnitList.size(); ++i )
				{
					if ( NULL != vecUnitList[i] )
						vecUnitList[i]->ApplyWorldGravityRate();
				}
				iUnitNum = (int) vecUnitList.size();
			}
		}
	}

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"JUNE_FX C3n SetGravityAndSpeedRate rate %f game type %d units %d ( -1 = not re-applied )", val, iGameType, iUnitNum );
#endif SERV_IRUHADEV_OFFLINE
}
#endif SERV_IRUHADEV_JUNE_WORLD_RATE_APPLY

#if defined( SERV_IRUHADEV_ELYSION_DEBUG ) && defined( MODIFY_FRUSTUM )
//{{ Iruha : 2026-10-17 // Atlas A13: proves the Lua SetCameraDistance call (Lanox SECRET_SCENE_DUMMY_SUMMON_TYPE3_STATE_START) reaches C++
void CX2World::SetCameraDistance_ELYDBG( float fBaseDist, float fStepDist1, float fStepDist2, float fStepDist3 )
{
	CX2OfflineLog::Server( L"[ELYDBG] SetCameraDistance call: base %.1f -> %.1f, steps %.1f %.1f %.1f", m_fBaseCameraDistance, fBaseDist, fStepDist1, fStepDist2, fStepDist3 );
	SetCameraDistance( fBaseDist, fStepDist1, fStepDist2, fStepDist3 );
}
//}}
#endif defined( SERV_IRUHADEV_ELYSION_DEBUG ) && defined( MODIFY_FRUSTUM )

