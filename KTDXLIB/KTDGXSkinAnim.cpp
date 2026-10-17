#include "StdAfx.h"
#include ".\ktdgxskinanim.h"

bool CKTDGXSkinAnim::m_sbTestUpdateAnimation = true;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
// June2017 31d2f3f0: KEM 433 when a model mesh's frame count differs from the anim's, text 3344f350 / 3344f3a8. June's service
// build keeps ErrorLogMsg in memory; this build writes ErrorLog.txt ( and stdout / stderr ) on every call, and every skin part
// added to a unit, its after-images included, can log one, so only the first 20 reach the file ( the SKREF line carries the total ).
static LONG s_lJuneSkinKem433 = 0;
static void JuneSkinAnimKem433( int iAnimFrame_, int iMeshFrame_, const WCHAR* pwszMesh_, const WCHAR* pwszFile_, int iLine_ )
{
	const LONG lCount = InterlockedIncrement( &s_lJuneSkinKem433 );
	if( lCount > 20 )
		return;

	WCHAR wszLog[256] = {0,};
	if( NULL == pwszFile_ )
		StringCchPrintfW( wszLog, 256, L"Anim Frame : %d, Mesh Frame : %d, Mesh : %s", iAnimFrame_, iMeshFrame_, pwszMesh_ );
	else
		StringCchPrintfW( wszLog, 256, L"Anim Frame : %d, Mesh Frame : %d, Mesh : %s, File : %s(%d)", iAnimFrame_, iMeshFrame_, pwszMesh_, pwszFile_, iLine_ );
	ErrorLogMsg( KEM_ERROR433, wszLog );
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
// SKREF ( R12 gate ): a dead skin anim must have closed one reference per RefDevice plus one per bCloseDeviceAtDestructor mesh.
// Session totals, written from the destructor at most once per 2 s, with the device manager's live device count.
static LONG		s_lJuneSkinRefAnims = 0;
static LONG		s_lJuneSkinRefAdds = 0;
static LONG		s_lJuneSkinRefCloses = 0;
static LONG		s_lJuneSkinRefUnbalanced = 0;
static DWORD	s_dwJuneSkinRefLastLog = 0;
static int		s_iJuneSkinRefUnbalancedLines = 0;
static void JuneSkinRefLogDead( const std::wstring& wstrAnim_, int iAdds_, int iOwned_, int iCloses_ )
{
	InterlockedIncrement( &s_lJuneSkinRefAnims );
	InterlockedExchangeAdd( &s_lJuneSkinRefAdds, iAdds_ + iOwned_ );
	InterlockedExchangeAdd( &s_lJuneSkinRefCloses, iCloses_ );
	if( iAdds_ + iOwned_ != iCloses_ )
	{
		InterlockedIncrement( &s_lJuneSkinRefUnbalanced );
		if( s_iJuneSkinRefUnbalancedLines < 10 )
		{
			++s_iJuneSkinRefUnbalancedLines;
			std::string strAnim;
			ConvertWCHARToChar( strAnim, wstrAnim_.c_str() );
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKREF unbalanced anim " << strAnim.c_str() << " addref " << iAdds_ << " owned " << iOwned_ << " close " << iCloses_ << fileout;
		}
	}

	const DWORD dwNow = timeGetTime();
	if( dwNow - s_dwJuneSkinRefLastLog >= 2000 )
	{
		s_dwJuneSkinRefLastLog = dwNow;
		int iDevices = -1;
		if( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDeviceManager() )
			iDevices = g_pKTDXApp->GetDeviceManager()->JuneGetDeviceCount();
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKREF dead anims " << (int)s_lJuneSkinRefAnims << " refs " << (int)s_lJuneSkinRefAdds
			<< " closes " << (int)s_lJuneSkinRefCloses << " unbalanced " << (int)s_lJuneSkinRefUnbalanced << " kem433 " << (int)s_lJuneSkinKem433
			<< " devices " << iDevices << fileout;
	}
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG

CKTDGXSkinAnim::CKTDGXSkinAnim() 
#ifdef FIX_MESH_RENDER_BEFORE_INITINALIZE
:m_bIsProcessFrameMove(false)
#endif // FIX_MESH_RENDER_BEFORE_INITINALIZE

{
	m_eObjectType = OT_CKTDGXSkinAnim;

	m_pAnimXSkinMesh		= NULL;
	m_pAnimAniXET			= NULL;
	m_pRenderer				= g_pKTDXApp->GetDGManager()->GetXRenderer();
	m_bUseTex[0]			= true;
	m_bUseTex[1]			= true;
	m_bUseTex[2]			= true;

	m_NowTrack				= 0;
	m_NewTrack				= 1;
#ifndef X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fElapsedTime			= 0.0f;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fAnimGlobalTime		= 0.0f;
	m_fAnimCurrTime			= 0.0f;
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_fAnimCurrSpeed        = 1.0f;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fAnimBeforeTime		= 0.0f;

	m_fAnimMaxTime			= 0.0f;
	m_pAC					= NULL;

	m_AnimState				= XAS_STOP;
	m_PlayType				= XAP_LOOP;
	m_NowAnimIndex			= 0;
	m_bAnimEnd				= false;
	m_fPlaySpeed			= 1.0f;
	m_fTransitionTime		= 0.1f;
	m_NowPlayCount			= 0;
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_fAnimOneshotTimer = 0.f;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

	m_MotionMoveBack.x		= 0.0f;
	m_MotionMoveBack.y		= 0.0f;
	m_MotionMoveBack.z		= 0.0f;

	m_MotionMoveNow.x		= 0.0f;
	m_MotionMoveNow.y		= 0.0f;
	m_MotionMoveNow.z		= 0.0f;

	m_MotionOffset.x		= 0.0f;
	m_MotionOffset.y		= 0.0f;
	m_MotionOffset.z		= 0.0f;

	m_bApplyMotionOffset	= true;

	m_bUseDXMatrix			= false;
	m_pCloneFrameRoot		= NULL;

	m_FrameNum				= 1;	
    m_BillBoardType			= CKTDGMatrix::BT_NONE;
//{{ robobeg : 2008-10-16
	//m_uiPass				= 0;
//}} robobeg : 2008-10-16
	ZeroMemory( m_FrameMatrixList, sizeof(D3DXMATRIX*) * MAX_FRAME_NUM );

	m_ModelDetailPercent	= 100;

	SetIsCulled( false );

	m_pModelXSkinMeshList.reserve( 16 );
	m_pModelAniXETList.reserve( 16 );
	m_pModelAniDataList.reserve( 16 );
	m_pModelMultiTexXETList.reserve( 16 );
	m_pModelTexChangeXETList.reserve( 16 );

	//m_CollisionDataList.reserve( 32 );
	//m_AttackDataList.reserve( 32 );

	m_vecAlphaSkinMesh.reserve( 32 );
#ifdef SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
	m_vecUVAnimSkinMesh.reserve( 4 );
	m_fSMUVAccValue = 0.f;
	m_fSMUVAccU = 0.f;
	m_fSMUVAccV = 0.f;
#endif SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM

#ifdef ARA_CHARACTER_BASE
	m_vecRenderParamSkinMesh.reserve( 32 );
#endif




#ifdef SKIN_LIGHTFLOW
    m_bLightFlow                = false;
    m_lightFlowType             = CKTDGXRenderer::RT_LIGHT_FLOW_LOCAL_Y;    
    m_pLightFlow                = NULL;
    m_fFlowMin                  = 0.f;
    m_fFlowMax                  = 0.f;
    m_fFlowSpeed                = 0.f;
    m_fNowPoint                 = 0.f;
    m_fWide                     = 0.f;

    //m_BillboardType		= CKTDGMatrix::BT_NONE;    
    INIT_COLOR( m_coColor, 1, 1, 1, 1 );
    //INIT_VECTOR3( m_LightPos, 1000, 1000, 1000 );
#endif

#ifdef JIGGLEBONE_TEST
	m_bJiggleMesh = false;
	m_fTension = -1.f;
	m_iExpand = 0;
#endif

	m_bApplyMoveBone = true;
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	m_iJuneRefAdds = 0;
	m_iJuneRefOwned = 0;
	m_iJuneRefCloses = 0;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	// June2017 ctor 31d32040: the dword 0x01010000 at +0x852 leaves +0x854 and +0x855 at 1
	m_bJuneLoopTransition = true;
	m_bJuneLastTransition = true;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK

#ifdef FACE_OFF_MONSTER_HEAD_TEST
	m_bHideNoRenderable = false;
#endif FACE_OFF_MONSTER_HEAD_TEST

}

CKTDGXSkinAnim::~CKTDGXSkinAnim(void)
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	// June2017 31d39280 -> 31d31500: CleanAnimStructure( NULL, NULL ) closes every stored mesh / XET and the anim mesh / XET;
	// there is no m_vecAddedSkinMesh ( ownership moved to the reference count )
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	const std::wstring wstrJuneAnim = ( NULL != m_pAnimXSkinMesh ) ? m_pAnimXSkinMesh->GetDeviceID() : std::wstring( L"-" );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	CleanAnimStructure( NULL, NULL );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	JuneSkinRefLogDead( wstrJuneAnim, m_iJuneRefAdds, m_iJuneRefOwned, m_iJuneRefCloses );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	CleanAnimStructure();

	BOOST_TEST_FOREACH( CKTDXDeviceXSkinMesh*, pSkinMesh, m_vecAddedSkinMesh )
	{
		SAFE_CLOSE( pSkinMesh );
	}
	m_vecAddedSkinMesh.clear();
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
}


//{{ seojt // 2008-10-15, 14:48
void CKTDGXSkinAnim::PreLoad()
{
    if ( m_pAnimXSkinMesh )
        m_pAnimXSkinMesh->PreLoad();
}//CKTDGXSkinAnim::PreLoad()
//}} seojt // 2008-10-15, 14:48


#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
// June2017 31d4d2f0: the device AddRef ( InterlockedIncrement on +0x48 ); March's m_RefCount is a plain int behind RefDevice's lock
void CKTDGXSkinAnim::JuneRefDevice( CKTDXDevice* pDevice )
{
	if( NULL == pDevice )
		return;

	pDevice->RefDevice();
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	++m_iJuneRefAdds;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
}

// June2017 SAFE_CLOSE( ptr ): CloseDevice( CKTDXDevice* ) 31cdd8d0 ( March JuneCloseDeviceByPointer ) when the device manager exists
void CKTDGXSkinAnim::JuneCloseDevice( CKTDXDevice* pDevice )
{
	if( NULL == pDevice || NULL == g_pKTDXApp->GetDeviceManager() )
		return;

	g_pKTDXApp->GetDeviceManager()->JuneCloseDeviceByPointer( pDevice );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	++m_iJuneRefCloses;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
}

void CKTDGXSkinAnim::CleanAnimStructure( CKTDXDeviceXSkinMesh* pNewAnimXSkinMesh, CKTDXDeviceXET* pNewAnimAniXET )
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
void CKTDGXSkinAnim::CleanAnimStructure()
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	// June2017 31d2f9a0: the per-set release check. An odd pointer cannot be a live COM object: KEM 436 and drop the slot, no
	// Release. A small non-zero pointer ( below 0xff ): KEM 436 "crash1", drop the slot and Release the copy ResetAnimSet kept
	// in the +0x294 map. Anything else is released as before.
	for( int i = 0; i < (int)m_vAnimSetList.size(); i++ )
	{
		const UINT_PTR uAnimSet = (UINT_PTR)m_vAnimSetList[i];
		if( 0 == ( uAnimSet & 1 ) )
		{
			if( uAnimSet < 0xff )
			{
				if( 0 != uAnimSet )
				{
					std::string strAniName;
					if( NULL != m_pAnimXSkinMesh )
						ConvertWCHARToChar( strAniName, m_pAnimXSkinMesh->GetDeviceID() );
					WCHAR wszLog[ MAX_PATH ] = {0,};
					StringCchPrintfW( wszLog, MAX_PATH, L"m_vAnimSetList Release Failed crash1 index = %d Ani = %S addr = %x", i, strAniName.c_str(), (unsigned int)uAnimSet );
					ErrorLogMsg( KEM_ERROR436, wszLog );
					KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM clean crash1 index " << i << " addr " << (unsigned int)uAnimSet << fileout;

					m_vAnimSetList[i] = NULL;
					std::map<int, LPD3DXANIMATIONSET>::iterator itBackup = m_mapJuneAnimSetBackup.find( i );
					if( itBackup != m_mapJuneAnimSetBackup.end() && NULL != itBackup->second )
					{
						itBackup->second->Release();
						itBackup->second = NULL;
					}
				}
			}
			else
			{
				m_vAnimSetList[i]->Release();
				m_vAnimSetList[i] = NULL;
			}
		}
		else
		{
			std::string strAniName;
			if( NULL != m_pAnimXSkinMesh )
				ConvertWCHARToChar( strAniName, m_pAnimXSkinMesh->GetDeviceID() );
			WCHAR wszLog[ MAX_PATH ] = {0,};
			StringCchPrintfW( wszLog, MAX_PATH, L"m_vAnimSetList Release Failed index = %d Ani = %S", i, strAniName.c_str() );
			ErrorLogMsg( KEM_ERROR436, wszLog );
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM clean odd pointer index " << i << fileout;

			m_vAnimSetList[i] = NULL;
		}
	}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	for( int i = 0; i < (int)m_vAnimSetList.size(); i++ )
	{
		SAFE_RELEASE( m_vAnimSetList[i] );		
	}
	
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	m_mAnimSetList.clear();
	m_mIndexByName.clear();		
	m_mCloneFrameMap.clear();
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	m_mapJuneAnimSetBackup.clear();	// June2017 31d2f9a0: the +0x294 map goes with the three name maps
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS



	m_vAnimSetList.resize(0);
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	// June2017 31d2f9a0: one close per stored model mesh, ani XET, multi-tex XET and tex-change XET ( the ani data list holds
	// no reference ), then the five lists are emptied
	for( UINT uJune = 0; uJune < m_pModelXSkinMeshList.size(); ++uJune )
		JuneCloseDevice( m_pModelXSkinMeshList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelAniXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelAniXETList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelMultiTexXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelMultiTexXETList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelTexChangeXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelTexChangeXETList[uJune] );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	m_pModelXSkinMeshList.resize(0);
	m_pModelAniXETList.resize(0);
	m_pModelAniDataList.resize(0);
	m_pModelMultiTexXETList.resize(0);
	m_pModelTexChangeXETList.resize(0);

	
	
	CKTDXDeviceXSkinMesh::CMultiAnimAllocateHierarchy AH;
	if( m_pCloneFrameRoot != NULL )
	{		
		D3DXFrameDestroy( m_pCloneFrameRoot, &AH );
		m_pCloneFrameRoot = NULL;
	}

    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pCollision, m_CollisionDataList )
    {
        SAFE_DELETE( pCollision );
    }//BOOST_TEST_FOREACH()
	m_CollisionDataList.clear();

    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pAttack, m_AttackDataList )
    {
        SAFE_DELETE( pAttack );
    }//BOOST_TEST_FOREACH(
	m_AttackDataList.clear();

	SAFE_RELEASE( m_pAC );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	// June2017 31d2f9a0 tail: the anim mesh and anim XET become the arguments ( reference the new one, close the old one )
	if( m_pAnimXSkinMesh != pNewAnimXSkinMesh )
	{
		if( NULL != pNewAnimXSkinMesh )
			JuneRefDevice( pNewAnimXSkinMesh );
		if( NULL != m_pAnimXSkinMesh )
		{
			JuneCloseDevice( m_pAnimXSkinMesh );
			m_pAnimXSkinMesh = NULL;
		}
		m_pAnimXSkinMesh = pNewAnimXSkinMesh;
	}
	if( m_pAnimAniXET != pNewAnimAniXET )
	{
		if( NULL != pNewAnimAniXET )
			JuneRefDevice( pNewAnimAniXET );
		if( NULL != m_pAnimAniXET )
		{
			JuneCloseDevice( m_pAnimAniXET );
			m_pAnimAniXET = NULL;
		}
		m_pAnimAniXET = pNewAnimAniXET;
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
}

void CKTDGXSkinAnim::BuildCloneFrameStructure( CKTDXDeviceXSkinMesh* pAnimXSkinMesh )
{
	KTDXPROFILE();
	
	m_mCloneFrameMap.clear();

	CKTDXDeviceXSkinMesh::CMultiAnimAllocateHierarchy AH;
	if( m_pCloneFrameRoot != NULL )
	{
		D3DXFrameDestroy( m_pCloneFrameRoot, &AH );
		m_pCloneFrameRoot = NULL;
	}
	m_pCloneFrameRoot = new CKTDXDeviceXSkinMesh::MultiAnimFrame();
	BuildCloneFrame( m_pCloneFrameRoot, pAnimXSkinMesh->GetFrameRoot() );

	m_FrameNum = 0;
	BuildCloneFrameList( m_pCloneFrameRoot );
}

void CKTDGXSkinAnim::BuildCloneFrame( CKTDXDeviceXSkinMesh::MultiAnimFrame* pCloneFrame, CKTDXDeviceXSkinMesh::MultiAnimFrame* pSrcFrame )
{
	KTDXPROFILE();

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	// June2017 31d31950: a NULL clone or source frame ends the build ( March dereferences both )
	if( NULL == pCloneFrame || NULL == pSrcFrame )
	{
		static int s_iJuneLogNullFrame = 0;
		if( s_iJuneLogNullFrame < 3 )
		{
			++s_iJuneLogNullFrame;
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM BuildCloneFrame NULL guard" << fileout;
		}
		return;
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	if( pSrcFrame->Name )
	{		
		if( 0 == strlen( pSrcFrame->Name ) )
		{
			pCloneFrame->Name	= (CHAR *) StrHeapCopy( "<no_name>" );
		}
		else
		{
			pCloneFrame->Name	= (CHAR *) StrHeapCopy( (CHAR *) pSrcFrame->Name );
		}
	}
	else
	{		
		pCloneFrame->Name	= (CHAR *) StrHeapCopy( "<no_name>" );
	}
	pCloneFrame->combineMatrix		= pSrcFrame->combineMatrix;
	pCloneFrame->fBoundingRadius	= pSrcFrame->fBoundingRadius;


//#ifdef HEAD_INVERSE_KINEMATICS_TEST
//	pCloneFrame->m_iIndex = pSrcFrame->m_iIndex;
//#endif HEAD_INVERSE_KINEMATICS_TEST


	WCHAR wszBoneName[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, pCloneFrame->Name, -1, wszBoneName, MAX_PATH );
	m_mCloneFrameMap[wszBoneName] = pCloneFrame;
	
	//wstring wstrBoneName = L"";
	//ConvertCharToWCHAR( wstrBoneName, pCloneFrame->Name );
	//ASSERT( false == wstrBoneName.empty() );
	//m_mCloneFrameMap[ wstrBoneName ] = pCloneFrame;

#ifdef JIGGLEBONE_TEST
	std::string boneName = pCloneFrame->Name;
	if( strstr(boneName.c_str(), "Jiggle_Bone_Root") == NULL && strstr(boneName.c_str(), "Jiggle_Bone_") != NULL )
	{
		pCloneFrame->m_bJiggleFrame = true;

		char cNum = boneName[12];
		pCloneFrame->m_iJiggleLevel = cNum-48;

		//float fRand = (rand() % 61) / 100.f - 0.3f;
		//fTension += fRand;
	}
#endif


	// transform siblings by the same matrix
	if( pSrcFrame->pFrameSibling )
	{
		pCloneFrame->pFrameSibling = new CKTDXDeviceXSkinMesh::MultiAnimFrame();		
		BuildCloneFrame( (CKTDXDeviceXSkinMesh::MultiAnimFrame *)pCloneFrame->pFrameSibling, (CKTDXDeviceXSkinMesh::MultiAnimFrame *) pSrcFrame->pFrameSibling );
	}
	else
		pCloneFrame->pFrameSibling = NULL;

	// transform children by the transformed matrix - hierarchical transformation
	if( pSrcFrame->pFrameFirstChild )
	{
		pCloneFrame->pFrameFirstChild = new CKTDXDeviceXSkinMesh::MultiAnimFrame();
		BuildCloneFrame( (CKTDXDeviceXSkinMesh::MultiAnimFrame *)pCloneFrame->pFrameFirstChild, (CKTDXDeviceXSkinMesh::MultiAnimFrame *) pSrcFrame->pFrameFirstChild );
	}
	else
		pCloneFrame->pFrameFirstChild = NULL;
}

void CKTDGXSkinAnim::BuildCloneFrameList( CKTDXDeviceXSkinMesh::MultiAnimFrame* pCloneFrame )
{
	m_FrameMatrixList[m_FrameNum] = &pCloneFrame->combineMatrix;
	m_FrameNum++;

	if( pCloneFrame->pFrameSibling )
	{
		// recursive call
		BuildCloneFrameList( (CKTDXDeviceXSkinMesh::MultiAnimFrame *) pCloneFrame->pFrameSibling );
	}

	if( pCloneFrame->pFrameFirstChild )
	{
		// recursive call
		BuildCloneFrameList( (CKTDXDeviceXSkinMesh::MultiAnimFrame *) pCloneFrame->pFrameFirstChild );
	}
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
// June2017 31d28f80: reference the new XET and close the old one when they differ
void CKTDGXSkinAnim::SetAnimXET( CKTDXDeviceXET* pAnimAniXET )
{
	if( m_pAnimAniXET == pAnimAniXET )
		return;

	if( NULL != pAnimAniXET )
		JuneRefDevice( pAnimAniXET );
	if( NULL != m_pAnimAniXET )
	{
		JuneCloseDevice( m_pAnimAniXET );
		m_pAnimAniXET = NULL;
	}
	m_pAnimAniXET = pAnimAniXET;
}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
void CKTDGXSkinAnim::SetAnimXET( CKTDXDeviceXET* pAnimAniXET )
{
	m_pAnimAniXET = pAnimAniXET;
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS


// @bSimple : 타격, 피격 박스가 없는 모션파일의 빠른 로딩을 위해서
bool CKTDGXSkinAnim::SetAnimXSkinMesh( CKTDXDeviceXSkinMesh* pAnimXSkinMesh, CKTDXDeviceXET* pAnimAniXET, bool bSimple /*= false*/ )
{ 
	KTDXPROFILE();

    //{{ jseop. pAnimXSkinMesh 가 NULL 인 경우가 있었음.
    ASSERT( pAnimXSkinMesh );
    //if( !pAnimXSkinMesh )
    //{
    //    return false;
    //}
    //}} jseop
	if ( NULL == pAnimXSkinMesh )
		return false;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	// June2017 31d32750 @31d32783: a mesh without an animation controller logs KEM 434 before the false ( March is silent )
	if( pAnimXSkinMesh->GetIsOnlyModelData() == true )
	{
		ErrorLog( KEM_ERROR434 );
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM SetAnimXSkinMesh mesh without an animation controller" << fileout;
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	if( pAnimXSkinMesh->GetIsOnlyModelData() == true )
		return false;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	CleanAnimStructure( pAnimXSkinMesh, pAnimAniXET );	// June2017 31d32750: stores the anim mesh / XET with references before GetCloneAC
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	CleanAnimStructure();
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS

	//SAFE_RELEASE( m_pAC );
	m_pAC = pAnimXSkinMesh->GetCloneAC();
	if( m_pAC == NULL )
	{
		ErrorLog( KEM_ERROR47 );
		return false;
	}

	ResetAnimSet();

	//D3DXMATRIX mat;
	//D3DXMatrixIdentity( &mat );

#ifndef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	m_pAnimXSkinMesh = pAnimXSkinMesh;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	if( m_pCloneFrameRoot == NULL )
	{
		BuildCloneFrameStructure( m_pAnimXSkinMesh );

		if( false == bSimple )
		{
			//{{ dmlee 2008.05.30 데미지 이펙트 이상한 위치에서 타격되는 문제 수정
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
            UpdateAnimationFrameStructure( 0.f );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
			UpdateAnimationFrameStructure();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
			//}} dmlee 2008.05.30 데미지 이펙트 이상한 위치에서 타격되는 문제 수정	
		}
	}

#ifndef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	m_pAnimAniXET = pAnimAniXET;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS

	return true;
}

#ifdef SEPARATION_MOTION
bool CKTDGXSkinAnim::MixAnim( CKTDXDeviceXSkinMesh* pAnimXSkinMesh, bool bSimple /*= false*/ )
{ 
	KTDXPROFILE();

	//{{ jseop. pAnimXSkinMesh 가 NULL 인 경우가 있었음.
	ASSERT( pAnimXSkinMesh );
	//if( !pAnimXSkinMesh )
	//{
	//    return false;
	//}
	//}} jseop

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	// June2017 31d34140 @31d34175: a motion mesh whose name is already in the +0x864 list is not mixed again ( true ). Nothing
	// clears the list, so after a second SetAnimXSkinMesh the same motion is skipped too ( JUNE_BUGS.md ).
	if( std::find( m_vecJuneMixedMotion.begin(), m_vecJuneMixedMotion.end(), pAnimXSkinMesh->GetDeviceID() ) != m_vecJuneMixedMotion.end() )
	{
		static int s_iJuneLogMixSkip = 0;
		if( s_iJuneLogMixSkip < 3 )
		{
			++s_iJuneLogMixSkip;
			std::string strMixName;
			ConvertWCHARToChar( strMixName, pAnimXSkinMesh->GetDeviceID() );
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM MixAnim skipped, already mixed: " << strMixName.c_str() << fileout;
		}
		return true;
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	if( pAnimXSkinMesh->GetIsOnlyModelData() == true )
		return false;

	if( m_pAC == NULL )
		return false;

	LPD3DXANIMATIONCONTROLLER pAnimMixer = NULL;
	LPD3DXANIMATIONCONTROLLER pAnimMixerNew = NULL;
	LPD3DXANIMATIONSET pAnimSet;

	pAnimMixerNew = pAnimXSkinMesh->GetCloneAC();
	if( pAnimMixerNew == NULL )
		return false;

	UINT cAnimSets = pAnimMixerNew->GetNumAnimationSets();
	
	HRESULT hr = m_pAC->CloneAnimationController(
		m_pAC->GetMaxNumAnimationOutputs(),
		m_pAC->GetMaxNumAnimationSets() + cAnimSets,
		m_pAC->GetMaxNumTracks(),
		m_pAC->GetMaxNumEvents(),
		&pAnimMixer);

	if(FAILED(hr))
		return false;

	SAFE_RELEASE( m_pAC );	

	m_pAC = pAnimMixer;
	pAnimMixer = NULL;

	for( UINT iAnimSet = 0; iAnimSet < cAnimSets; iAnimSet++ )
	{
		pAnimMixerNew->GetAnimationSet(iAnimSet, &pAnimSet);
		m_pAC->RegisterAnimationSet(pAnimSet);

		SAFE_RELEASE(pAnimSet);
	}
	
	SAFE_RELEASE( pAnimMixerNew );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	// June2017 31d34140 @31d34297: the name is remembered once the sets are registered
	m_vecJuneMixedMotion.push_back( pAnimXSkinMesh->GetDeviceID() );
	{
		static int s_iJuneLogMixAdd = 0;
		if( s_iJuneLogMixAdd < 3 )
		{
			++s_iJuneLogMixAdd;
			std::string strMixName;
			ConvertWCHARToChar( strMixName, pAnimXSkinMesh->GetDeviceID() );
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM MixAnim remembers " << strMixName.c_str() << " ( " << (int)m_vecJuneMixedMotion.size() << " mixed )" << fileout;
		}
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS

	//CleanAnimStructure();
	for( int i = 0; i < (int)m_vAnimSetList.size(); i++ )
	{
		SAFE_RELEASE( m_vAnimSetList[i] );		
	}
	m_mAnimSetList.clear();	

	ResetAnimSet();

	//D3DXMATRIX mat;
	//D3DXMatrixIdentity( &mat );

	//m_pAnimXSkinMesh = pAnimXSkinMesh;
	if( m_pCloneFrameRoot == NULL )
	{
		BuildCloneFrameStructure( m_pAnimXSkinMesh );
	}

	if( false == bSimple )
	{
		//{{ dmlee 2008.05.30 데미지 이펙트 이상한 위치에서 타격되는 문제 수정
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
        UpdateAnimationFrameStructure( 0.f );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		UpdateAnimationFrameStructure();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		//}} dmlee 2008.05.30 데미지 이펙트 이상한 위치에서 타격되는 문제 수정	
	}

	return true;
}
#endif


#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
// June2017 31d28fd0: really replaces ( March wrote a local copy, a no-op ), with references, and nulls the ani data slot
void CKTDGXSkinAnim::ReplaceAniXET( CKTDXDeviceXET* pModelAniXETFrom, CKTDXDeviceXET* pModelAniXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelAniXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelAniXETList[iA];
		if ( pModelAniXET == pModelAniXETFrom && pModelAniXET != pModelAniXETTo )
		{
			if( NULL != pModelAniXETTo )
				JuneRefDevice( pModelAniXETTo );
			if( NULL != pModelAniXET )
				JuneCloseDevice( pModelAniXET );
			m_pModelAniXETList[iA] = pModelAniXETTo;
			if( iA < m_pModelAniDataList.size() )
				m_pModelAniDataList[iA] = NULL;
		}
	}
}

// June2017 31d2b280: returns void. An empty list takes the XET with a reference; otherwise a different XET is referenced,
// slot 0's old XET closed and the new one stored ( the callers no longer close the old XET )
void CKTDGXSkinAnim::ReplaceMultiTexXET( int iIndex, CKTDXDeviceXET* pModelMultiTexXETTo )
{
	if( m_pModelMultiTexXETList.empty() )
	{
		if( NULL != pModelMultiTexXETTo )
			JuneRefDevice( pModelMultiTexXETTo );
		m_pModelMultiTexXETList.push_back( pModelMultiTexXETTo );
		return;
	}

	CKTDXDeviceXET* pOldXet = m_pModelMultiTexXETList[0];
	if( pOldXet != pModelMultiTexXETTo )
	{
		if( NULL != pModelMultiTexXETTo )
			JuneRefDevice( pModelMultiTexXETTo );
		if( NULL != pOldXet )
			JuneCloseDevice( pOldXet );
		m_pModelMultiTexXETList[0] = pModelMultiTexXETTo;
	}
}

// June2017 31d29070: really replaces ( March wrote a local copy, a no-op ), with references
void CKTDGXSkinAnim::ReplaceMultiTexXET( CKTDXDeviceXET* pModelMultiTexXETFrom, CKTDXDeviceXET* pModelMultiTexXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelMultiTexXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelMultiTexXETList[iA];
		if ( pModelAniXET == pModelMultiTexXETFrom && pModelAniXET != pModelMultiTexXETTo )
		{
			if( NULL != pModelMultiTexXETTo )
				JuneRefDevice( pModelMultiTexXETTo );
			if( NULL != pModelAniXET )
				JuneCloseDevice( pModelAniXET );
			m_pModelMultiTexXETList[iA] = pModelMultiTexXETTo;
		}
	}
}

// June2017 31d290f0: the same replace, with references
void CKTDGXSkinAnim::ReplaceChangeXET( CKTDXDeviceXET* pModelTexChangeXETFrom, CKTDXDeviceXET* pModelTexChangeXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelTexChangeXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelTexChangeXETList[iA];
		if ( pModelAniXET == pModelTexChangeXETFrom && pModelAniXET != pModelTexChangeXETTo )
		{
			if( NULL != pModelTexChangeXETTo )
				JuneRefDevice( pModelTexChangeXETTo );
			if( NULL != pModelAniXET )
				JuneCloseDevice( pModelAniXET );
			m_pModelTexChangeXETList[iA] = pModelTexChangeXETTo;
		}
	}
}

// June2017 31d29170: March's bounds, with references when the XET changes
void CKTDGXSkinAnim::ReplaceChangeXET( int iIndex, CKTDXDeviceXET* pModelTexChangeXETTo )
{
	if( iIndex < 0 || iIndex > (int)m_pModelTexChangeXETList.size()-1 )
		return;

	CKTDXDeviceXET* pOldXet = m_pModelTexChangeXETList[iIndex];
	if( pOldXet == pModelTexChangeXETTo )
		return;

	if( NULL != pModelTexChangeXETTo )
		JuneRefDevice( pModelTexChangeXETTo );
	if( NULL != pOldXet )
		JuneCloseDevice( pOldXet );
	m_pModelTexChangeXETList[iIndex] = pModelTexChangeXETTo;
}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
void CKTDGXSkinAnim::ReplaceAniXET( CKTDXDeviceXET* pModelAniXETFrom, CKTDXDeviceXET* pModelAniXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelAniXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelAniXETList[iA];
		if ( pModelAniXET == pModelAniXETFrom )
		{
			pModelAniXET = pModelAniXETTo;
		}
	}
}


// index번째에 XET가 있으면 교체하고, 없으면 만들어서 넣는다. return 값은 기존에 있던 xet이다.
CKTDXDeviceXET* CKTDGXSkinAnim::ReplaceMultiTexXET( int iIndex, CKTDXDeviceXET* pModelMultiTexXETTo )
{
	if( m_pModelMultiTexXETList.empty() )
	{
		m_pModelMultiTexXETList.push_back( pModelMultiTexXETTo );
	}
	else
	{
		CKTDXDeviceXET* pOldXet = m_pModelMultiTexXETList[0];
		m_pModelMultiTexXETList[0] = pModelMultiTexXETTo;
		return pOldXet;
	}

	return NULL;
}


void CKTDGXSkinAnim::ReplaceMultiTexXET( CKTDXDeviceXET* pModelMultiTexXETFrom, CKTDXDeviceXET* pModelMultiTexXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelMultiTexXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelMultiTexXETList[iA];
		if ( pModelAniXET == pModelMultiTexXETFrom )
		{
			pModelAniXET = pModelMultiTexXETTo;
		}
	}
}

void CKTDGXSkinAnim::ReplaceChangeXET( CKTDXDeviceXET* pModelTexChangeXETFrom, CKTDXDeviceXET* pModelTexChangeXETTo )
{
	for ( unsigned int iA = 0; iA < m_pModelTexChangeXETList.size(); ++iA )
	{
		CKTDXDeviceXET* pModelAniXET = m_pModelTexChangeXETList[iA];
		if ( pModelAniXET == pModelTexChangeXETFrom )
		{
			m_pModelTexChangeXETList[iA] = pModelTexChangeXETTo;
		}
	}
}

void CKTDGXSkinAnim::ReplaceChangeXET( int iIndex, CKTDXDeviceXET* pModelTexChangeXETTo )
{
	if( iIndex < 0 || iIndex > (int)m_pModelTexChangeXETList.size()-1 )
		return;

	m_pModelTexChangeXETList[iIndex] = pModelTexChangeXETTo;
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS



// @bCloseDeviceAtDestructor : 추가된 device를 skinanim객체가 지워질 때 close_device해줄지 말지. 일단은 pModelXSkinMesh만 처리 되게 만들었음.
void CKTDGXSkinAnim::AddModelXSkinMesh( CKTDXDeviceXSkinMesh* pModelXSkinMesh, CKTDXDeviceXET* pModelAniXET, 
									   CKTDXDeviceXET* pModelMultiTexXET, CKTDXDeviceXET* pModelTexChangeXET, bool bCloseDeviceAtDestructor /*= false*/
                                       , bool bAddCollisionData /*= true*/ 
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
									   , const WCHAR* pwszFile /*= NULL*/, int iLine /*= 0*/
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
									   )
{

	KTDXPROFILE();

	if( NULL == pModelXSkinMesh )
		return;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	// June2017 31d2f3f0: KEM 433 when the mesh's frame count ( +0x850 ) differs from this anim's ( +0x328 ). Then one reference per
	// stored device: the mesh unless bCloseDeviceAtDestructor ( the list then owns the caller's reference ), and every non-NULL XET.
	// No m_vecAddedSkinMesh: CleanAnimStructure / RemoveModelXSkinMesh / ClearModelXSkinMeshList close what is stored.
	if( (int)pModelXSkinMesh->GetFrameNum() != m_FrameNum )
		JuneSkinAnimKem433( m_FrameNum, (int)pModelXSkinMesh->GetFrameNum(), pModelXSkinMesh->GetDeviceID().c_str(), pwszFile, iLine );

	if( false == bCloseDeviceAtDestructor )
	{
		JuneRefDevice( pModelXSkinMesh );
	}
	else
	{
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
		++m_iJuneRefOwned;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG
	}
	if( NULL != pModelAniXET )
		JuneRefDevice( pModelAniXET );
	if( NULL != pModelMultiTexXET )
		JuneRefDevice( pModelMultiTexXET );
	if( NULL != pModelTexChangeXET )
		JuneRefDevice( pModelTexChangeXET );
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	if( true == bCloseDeviceAtDestructor )
	{
		m_vecAddedSkinMesh.push_back( pModelXSkinMesh );
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS

	m_pModelXSkinMeshList.push_back( pModelXSkinMesh );
	m_pModelAniXETList.push_back( pModelAniXET );
	m_pModelAniDataList.push_back( NULL );
	m_pModelMultiTexXETList.push_back( pModelMultiTexXET );
	m_pModelTexChangeXETList.push_back( pModelTexChangeXET );

	if( bAddCollisionData == false )
		return;

	CKTDXCollision::CollisionData* pCollisionData = NULL;
	CKTDXCollision::CollisionData* pAttackData = NULL;

    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelCollision, pModelXSkinMesh->GetCollisionDataList() )
    {
        ASSERT( pModelCollision != NULL );
		pCollisionData = new CKTDXCollision::CollisionData();
        ASSERT( pCollisionData );
        *pCollisionData = *pModelCollision;
		if( LinkCollisionDataFrame( pCollisionData ) == false )
		{
			// 오현빈 // 2013-09-13 // 
			// 기존 : LinkCollisionDataFrame 값이 false를 return하지 않도록 함으로써 강제 크래시 발생 유도
			// 변경 : 로그 남기기 위해 false를 return하는 대신 continue; 수행 하지 않도록 하여 크래시 발생 유도
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
// 			SAFE_DELETE( pCollisionData );
// 			continue;
		}
		m_CollisionDataList.push_back( pCollisionData );
    }//BOOST_TEST_FOREACH()

    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelAttack, pModelXSkinMesh->GetAttackDataList() )
    {
        ASSERT( pModelAttack != NULL );
		pAttackData = new CKTDXCollision::CollisionData();
		*pAttackData = *pModelAttack;
		if( LinkCollisionDataFrame( pAttackData ) == false )
		{
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
// 			SAFE_DELETE( pAttackData );
// 			continue;
		}
		m_AttackDataList.push_back( pAttackData );
    }//BOOST_TEST_FOREACH()


// 	CKTDXDeviceXET* pXET = pModelXSkinMesh->GetXET();
// 
// 	if( pXET != NULL )
// 	{
//         BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pXETCollision, pXET->GetCollisionList() )
// 		{
//             ASSERT( pXETCollision != NULL );
// 			pCollisionData = new CKTDXCollision::CollisionData();
//             ASSERT( pCollisionData != NULL );
// 			*pCollisionData = *pXETCollision;
// 			if( LinkCollisionDataFrame( pCollisionData ) == false )
// 			{
// 				SAFE_DELETE( pCollisionData );
// 				continue;
// 			}
// 			m_CollisionDataList.push_back( pCollisionData );
// 		}
//         BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pXETAttack, pXET->GetAttackList() )
// 		{
//             ASSERT( pXETAttack != NULL );
// 			pAttackData = new CKTDXCollision::CollisionData();
//             ASSERT( pAttackData != NULL );
// 			*pAttackData = *pXETAttack;
// 			if( LinkCollisionDataFrame( pAttackData ) == false )
// 			{
// 				SAFE_DELETE( pAttackData );
// 				continue;
// 			}
// 			m_AttackDataList.push_back( pAttackData );
// 		}
// 	}
}

void CKTDGXSkinAnim::AddAnimXSkinMesh( CKTDXDeviceXSkinMesh* pModelXSkinMesh )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	// June2017 31d2f720 ( no June caller ): NULL test, then a reference on the mesh
	if( NULL == pModelXSkinMesh )
		return;
	JuneRefDevice( pModelXSkinMesh );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
	m_pModelXSkinMeshList.push_back( pModelXSkinMesh );
	m_pModelAniXETList.push_back( NULL );
	m_pModelAniDataList.push_back( NULL );
	m_pModelMultiTexXETList.push_back( NULL );
	m_pModelTexChangeXETList.push_back( NULL );

	CKTDXCollision::CollisionData* pCollisionData	= NULL;
	CKTDXCollision::CollisionData* pAttackData		= NULL;

    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelCollision, pModelXSkinMesh->GetCollisionDataList() )
    {
        ASSERT( pModelCollision != NULL );
		pCollisionData = new CKTDXCollision::CollisionData();
        ASSERT( pCollisionData != NULL );
		*pCollisionData = *pModelCollision;
		if( LinkCollisionDataFrame( pCollisionData ) == false )
		{
			// 오현빈 // 2013-09-13 // 
			// 기존에는 LinkCollisionDataFrame 값이 false를 return하지 않도록 함으로써 강제 크래시 발생 하도록 했음.
			// 로그 남기기 false를 return 하고 continue 하지 않도록 변경거
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
			//SAFE_DELETE( pCollisionData );
			//continue;
		}
		m_CollisionDataList.push_back( pCollisionData );
    }//BOOST_TEST_FOREACH()


    BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelAttack, pModelXSkinMesh->GetAttackDataList() )
    {
        ASSERT( pModelAttack != NULL );
		pAttackData = new CKTDXCollision::CollisionData();
        ASSERT( pAttackData != NULL );
		*pAttackData = *pModelAttack;
		if( LinkCollisionDataFrame( pAttackData ) == false )
		{
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
// 			SAFE_DELETE( pAttackData );
// 			continue;
		}
		m_AttackDataList.push_back( pAttackData );
    }//BOOST_TEST_FOREACH()
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
// June2017 31d2f250: AddAnimXSkinMesh's collision half ( a copy of each collision / attack data, linked to this anim's clone
// frames, KEM 432 on a failed link ), with no list push and no reference. CX2GUUser's motion init calls it on m_pXSkinAnim.
void CKTDGXSkinAnim::JuneAddCollisionData( CKTDXDeviceXSkinMesh* pModelXSkinMesh )
{
	if( NULL == pModelXSkinMesh )
		return;

	BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelCollision, pModelXSkinMesh->GetCollisionDataList() )
	{
		ASSERT( pModelCollision != NULL );
		CKTDXCollision::CollisionData* pCollisionData = new CKTDXCollision::CollisionData();
		ASSERT( pCollisionData != NULL );
		*pCollisionData = *pModelCollision;
		if( LinkCollisionDataFrame( pCollisionData ) == false )
		{
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
		}
		m_CollisionDataList.push_back( pCollisionData );
	}

	BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pModelAttack, pModelXSkinMesh->GetAttackDataList() )
	{
		ASSERT( pModelAttack != NULL );
		CKTDXCollision::CollisionData* pAttackData = new CKTDXCollision::CollisionData();
		ASSERT( pAttackData != NULL );
		*pAttackData = *pModelAttack;
		if( LinkCollisionDataFrame( pAttackData ) == false )
		{
			ErrorLogMsg( KEM_ERROR432, pModelXSkinMesh->GetDeviceID().c_str() );
		}
		m_AttackDataList.push_back( pAttackData );
	}
}

#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
bool CKTDGXSkinAnim::LinkCollisionDataFrame( CKTDXCollision::CollisionData* pCollisionData )
{	
	ASSERT( NULL != pCollisionData );
	
    if( pCollisionData->m_CollisionType == CKTDXCollision::CT_SPHERE || pCollisionData->m_CollisionType == CKTDXCollision::CT_GUARD)
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = GetCloneFrame( pCollisionData->m_FrameName.c_str() );
		if( NULL == pFrame )
		{
			ASSERT( !L"GetCloneFrame Error" );
			return false;
		}

		pCollisionData->m_pCombineMatrix		= &pFrame->combineMatrix;
		pCollisionData->m_pTransformMatrix		= &pFrame->TransformationMatrix;
		pCollisionData->m_pBaseMatrix			= &pFrame->baseMatrix;
	}
	else if( pCollisionData->m_CollisionType == CKTDXCollision::CT_LINE )
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = GetCloneFrame( pCollisionData->m_FrameName.c_str() );
		if( NULL == pFrame )
		{
			ASSERT( !L"GetCloneFrame Error" );
			return false;
		}

		pCollisionData->m_pCombineMatrix		= &pFrame->combineMatrix;
		pCollisionData->m_pTransformMatrix		= &pFrame->TransformationMatrix;
		pCollisionData->m_pBaseMatrix			= &pFrame->baseMatrix;

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameEnd = GetCloneFrame( pCollisionData->m_FrameNameLineEnd.c_str() );
		if( NULL == pFrameEnd )
		{
			ASSERT( !L"GetCloneFrame Error" );
			return false;
		}

		pCollisionData->m_pCombineMatrixLineEnd		= &pFrameEnd->combineMatrix;
		pCollisionData->m_pTransformMatrixLineEnd	= &pFrameEnd->TransformationMatrix;
		pCollisionData->m_pBaseMatrixLineEnd		= &pFrameEnd->baseMatrix;
	}

	return true;
}

void CKTDGXSkinAnim::RemoveModelXSkinMesh( CKTDXDeviceXSkinMesh* pModelXSkinMesh )
{
	if ( pModelXSkinMesh == NULL )
		return;

	for( int i = 0; i < (int)m_pModelXSkinMeshList.size(); i++ )
	{
		if( m_pModelXSkinMeshList[i] == pModelXSkinMesh )
		{
			//충돌박스 제거

            CKTDXCollision::CollisionDataList::iterator iter, iterNext;
            for( iter = m_CollisionDataList.begin(); iter != m_CollisionDataList.end(); iter = iterNext )
            {
                iterNext = iter; iterNext++;
                CKTDXCollision::CollisionData* pCollision = *iter;
                ASSERT( pCollision != NULL );
				if( pCollision->m_pDeviceMesh == pModelXSkinMesh )
				{
					SAFE_DELETE( pCollision );
					m_CollisionDataList.erase( iter );
				}//if
            }//for

            for( iter = m_AttackDataList.begin(); iter != m_AttackDataList.end(); iter = iterNext )
            {
                iterNext = iter; iterNext++;
                CKTDXCollision::CollisionData* pAttack = *iter;
                ASSERT( pAttack != NULL );
                if ( pAttack->m_pDeviceMesh == pModelXSkinMesh )
                {
                    SAFE_DELETE( pAttack );
                    m_AttackDataList.erase( iter );
                }//if
            }//for

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
			// June2017 31d2b2f0: the same erases, closing what the skin anim referenced: the mesh after its erase, each non-NULL
			// ani / multi-tex / tex-change XET ( slot nulled ) before its erase; the ani data slot holds no reference
			if( i < (int)m_pModelXSkinMeshList.size() )
				m_pModelXSkinMeshList.erase( m_pModelXSkinMeshList.begin() + i );
			JuneCloseDevice( pModelXSkinMesh );
			if( i < (int)m_pModelAniXETList.size() )
			{
				if( NULL != m_pModelAniXETList[i] )
				{
					JuneCloseDevice( m_pModelAniXETList[i] );
					m_pModelAniXETList[i] = NULL;
				}
				m_pModelAniXETList.erase( m_pModelAniXETList.begin() + i );
			}
			if( i < (int)m_pModelAniDataList.size() )
				m_pModelAniDataList.erase( m_pModelAniDataList.begin() + i );
			if( i < (int)m_pModelMultiTexXETList.size() )
			{
				if( NULL != m_pModelMultiTexXETList[i] )
				{
					JuneCloseDevice( m_pModelMultiTexXETList[i] );
					m_pModelMultiTexXETList[i] = NULL;
				}
				m_pModelMultiTexXETList.erase( m_pModelMultiTexXETList.begin() + i );
			}
			if( i < (int)m_pModelTexChangeXETList.size() )
			{
				if( NULL != m_pModelTexChangeXETList[i] )
				{
					JuneCloseDevice( m_pModelTexChangeXETList[i] );
					m_pModelTexChangeXETList[i] = NULL;
				}
				m_pModelTexChangeXETList.erase( m_pModelTexChangeXETList.begin() + i );
			}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
			if( i < (int)m_pModelXSkinMeshList.size() )
				m_pModelXSkinMeshList.erase( m_pModelXSkinMeshList.begin() + i );
			if( i < (int)m_pModelAniXETList.size() )
				m_pModelAniXETList.erase( m_pModelAniXETList.begin() + i );
			if( i < (int)m_pModelAniDataList.size() )
				m_pModelAniDataList.erase( m_pModelAniDataList.begin() + i );
			if( i < (int)m_pModelMultiTexXETList.size() )
				m_pModelMultiTexXETList.erase( m_pModelMultiTexXETList.begin() + i );
			if( i < (int)m_pModelTexChangeXETList.size() )
				m_pModelTexChangeXETList.erase( m_pModelTexChangeXETList.begin() + i );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
			break;
		}
	}
}

bool CKTDGXSkinAnim::IsInModelXSkinMesh( CKTDXDeviceXSkinMesh* pModelXSkinMesh )
{
	for( int i = 0; i < (int)m_pModelXSkinMeshList.size(); i++ )
	{
		if( m_pModelXSkinMeshList[i] == pModelXSkinMesh )
		{
			return true;
		}
	}
	return false;
}

HRESULT CKTDGXSkinAnim::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

#ifndef X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fElapsedTime = fElapsedTime;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

	if( m_AnimState == XAS_PLAYING )
	{
		m_bAnimEnd = false;
		
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
        UpdateAnimationFrameStructure( fElapsedTime );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		UpdateAnimationFrameStructure();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

		switch( m_PlayType )
		{
			case XAP_LOOP:
				if( m_fAnimCurrTime > m_fAnimMaxTime - m_fTransitionTime )
				{
					m_bAnimEnd = true;
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
					// June2017 31d2bd80 @31d2d585: the restart uses anim +0x854 ( March: the default, true )
					if( false == m_bJuneLoopTransition )
					{
						static int s_iJuneLogLoopOff = 0;
						if( s_iJuneLogLoopOff < 3 )
						{
							++s_iJuneLogLoopOff;
							KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM loop restart without transition" << fileout;
						}
					}
					ChangeAnim( m_NowAnimIndex, m_bJuneLoopTransition, 0.0f );
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
					ChangeAnim( m_NowAnimIndex );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
					m_NowPlayCount++;
				}
				break;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
			case XAP_JUNE_LOOP_LAST_TRANSITION:
				// June2017 31d2bd80 @31d2d631-31d2d650: the loop restart with the last ChangeAnim's transition ( +0x855 )
				if( m_fAnimCurrTime > m_fAnimMaxTime - m_fTransitionTime )
				{
					m_bAnimEnd = true;
					ChangeAnim( m_NowAnimIndex, m_bJuneLastTransition, 0.0f );
					m_NowPlayCount++;
					{
						static int s_iJuneLogPlayType3 = 0;
						if( s_iJuneLogPlayType3 < 3 )
						{
							++s_iJuneLogPlayType3;
							KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM play type 3 restart, transition " << (int)m_bJuneLastTransition << fileout;
						}
					}
				}
				break;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
			case XAP_ONE:
				if( m_fAnimCurrTime > m_fAnimMaxTime - m_fTransitionTime )
				{
					m_bAnimEnd		= true;
					m_AnimState		= XAS_STOP;
					m_NowPlayCount	= 1;
				}
				break;

			case XAP_ONE_WAIT:
				if( m_fAnimCurrTime > m_fAnimMaxTime - m_fTransitionTime )
				{
					m_bAnimEnd		= true;
					m_AnimState		= XAS_WAIT;
					m_NowPlayCount	= 1;
				}
				break;

		}
	}
	else if( m_AnimState == XAS_WAIT )
	{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		UpdateAnimationFrameStructure( 0.f );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		m_fElapsedTime = 0.0f;
		UpdateAnimationFrameStructure();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	}
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    else
    {
        UpdateBeforeAnimationTime();
    }
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE


#ifdef SKIN_LIGHTFLOW
    if(m_bLightFlow == true)
    {
        LightFlowFrameMove(fTime, fElapsedTime);
    }    
#endif

#ifdef FIX_MESH_RENDER_BEFORE_INITINALIZE
	if( false == GetProcessFrameMove() )
		SetProcessFrameMove(true);
#endif // FIX_MESH_RENDER_BEFORE_INITINALIZE


	return S_OK;
}


//{{ robobeg : 2008-10-16



//{{ robobeg : 2008-10-16
/*virtual*/
RENDER_HINT   CKTDGXSkinAnim::OnFrameRender_Prepare() 
{ 
    if ( m_pRenderer == NULL || m_AnimState == XAS_STOP )
        return RENDER_HINT_NORENDER;

    unsigned uSize = m_pModelXSkinMeshList.size();
    if ( uSize == 0 )
        return RENDER_HINT_NORENDER;



#ifdef SKIN_LIGHTFLOW
    if ( m_bLightFlow == true && m_pLightFlow != NULL ) 
        return RENDER_HINT_DEFAULT;
#endif // #ifdef SKIN_LIGHTFLOW

    unsigned uSkinningVerts = 0;
    for( unsigned u = 0; u < uSize; u++ )
    {
		CKTDXDeviceXSkinMesh* pXSkinMesh = m_pModelXSkinMeshList[u];

#ifdef FACE_OFF_MONSTER_HEAD_TEST
		if(m_bHideNoRenderable == true && pXSkinMesh->GetNoRenderable() == true)
			continue;
#endif FACE_OFF_MONSTER_HEAD_TEST

        ASSERT( pXSkinMesh != NULL );
        if ( uSkinningVerts < pXSkinMesh->GetMaxSkinningVertices() )
            uSkinningVerts = pXSkinMesh->GetMaxSkinningVertices();
    }//for

    return RENDER_HINT_XSKINMESH( m_RenderParam.renderType, m_RenderParam.bFog && m_RenderParam.bFogShow, uSkinningVerts );

}//OnFrameRender_Prepare()


/*virtual*/
void CKTDGXSkinAnim::OnFrameRender_Draw()
{
	KTDXPROFILE();

#ifdef FIX_MESH_RENDER_BEFORE_INITINALIZE
	if( false == GetProcessFrameMove() )
		return;
#endif // FIX_MESH_RENDER_BEFORE_INITINALIZE


	CopyModelFrameStructure();

#ifdef SKIN_LIGHTFLOW
    int modelIndex = 0;
#endif

    m_vecAlphaSkinMesh.resize( 0 );
#ifdef SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
	m_vecUVAnimSkinMesh.resize( 0 );
#endif SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM

#ifdef ARA_CHARACTER_BASE
	m_vecRenderParamSkinMesh.resize( 0 );
#endif

//{{ robobeg : 2008-10-06

    for( HRESULT hr = m_pRenderer->BeginRender( m_RenderParam ); SUCCEEDED( hr ); hr = m_pRenderer->NextRender() )
    {	
		for( int i = 0; i < (int)m_pModelXSkinMeshList.size(); i++ )
		{		
			CKTDXDeviceXSkinMesh* pXSkinMesh = m_pModelXSkinMeshList[i];
            ASSERT( pXSkinMesh != NULL );
#ifdef  X2OPTIMIZE_KTDGXSKINANIM_CRAHSH_BUG_FIX
            if ( pXSkinMesh == NULL )
                continue;
#endif  X2OPTIMIZE_KTDGXSKINANIM_CRAHSH_BUG_FIX
#ifdef FACE_OFF_MONSTER_HEAD_TEST
			if(m_bHideNoRenderable == true && pXSkinMesh->GetNoRenderable() == true)
				continue;
#endif FACE_OFF_MONSTER_HEAD_TEST

#ifdef SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
			// June 31d30380: a model whose device mesh has render type >= 0 ( not alpha-render ) skips the normal pass and is
			// queued for the extra pass after the alpha pass. Only SMRT_UV_ANIM ( 3 ) is implemented; other types draw normally.
			if( 3 == pXSkinMesh->GetSkinMeshRenderType() && false == pXSkinMesh->GetAlphaRender() )
			{
				bool bQueued = false;
				for( int q = 0; q < (int)m_vecUVAnimSkinMesh.size(); ++q )
				{
					if( m_vecUVAnimSkinMesh[q].m_indexMode == i )
					{
						bQueued = true;
						break;
					}
				}
				if( false == bQueued )
				{
					Alpha_SkinMesh stUVObj = { pXSkinMesh, i };
					m_vecUVAnimSkinMesh.push_back( stUVObj );
				}
				continue;
			}
#endif SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
			for( int j = 0; j < (int) pXSkinMesh->GetDrawFrameNum(); j++ )
			{			
#ifdef SKIN_LIGHTFLOW
                if(m_bLightFlow == true && m_pLightFlow == pXSkinMesh->GetDrawFrame( j ))
                {
                    modelIndex = i;
                    continue;
                }
#endif

#ifdef ARA_CHARACTER_BASE
				if( false == pXSkinMesh->GetAlphaRender() && false == pXSkinMesh->GetUseRenderParam() )
				{
					DrawFrame( pXSkinMesh->GetDrawFrame( j ), i );
				}
				else if ( true == pXSkinMesh->GetAlphaRender() )
				{
					// alpha push
					Alpha_SkinMesh stAlphaObj = { pXSkinMesh, i };
					m_vecAlphaSkinMesh.push_back(stAlphaObj);
				}
				else if ( true == pXSkinMesh->GetUseRenderParam() )
				{
					RenderParam_SkinMesh stRenderParamObj = { pXSkinMesh, i };
					m_vecRenderParamSkinMesh.push_back( stRenderParamObj );
				}
#else
				if(pXSkinMesh->GetAlphaRender() == false)
				{
					DrawFrame( pXSkinMesh->GetDrawFrame( j ), i );
				}
				else
				{
					// alpha push
					Alpha_SkinMesh stAlphaObj = { pXSkinMesh, i };
					m_vecAlphaSkinMesh.push_back(stAlphaObj);
				}
#endif
			}
		}
	}//for
//}} robobeg : 2008-10-16

#ifdef SKIN_LIGHTFLOW
    if(m_bLightFlow == true && m_pLightFlow != NULL)
    {
//{{ robobeg : 2008-10-16

        CKTDGXRenderer::RenderParam renderParam = m_RenderParam;

        // light flow renderparam 설정
        renderParam.bAlphaBlend = true;
        renderParam.renderType = m_lightFlowType;
        renderParam.color.r		= m_coColor.r * m_RenderParam.color.r;
        renderParam.color.g		= m_coColor.g * m_RenderParam.color.g;
        renderParam.color.b		= m_coColor.b * m_RenderParam.color.b;
        renderParam.color.a		= m_coColor.a * m_RenderParam.color.a;

//{{ robobeg : 2008-10-24
//#if 0 
//        if( m_bUseDXMatrix == false )
//        {
//            D3DXMATRIX temp = GetMatrix().GetMatrix( m_BillBoardType );
//            renderParam.worldMatrix = temp;
//            //UpdateAnimationFrame( m_pAnimXSkinMesh->GetFrameRoot(), &temp, m_pCloneFrameRoot );
//        }
//        else
//        {
//            renderParam.worldMatrix = m_DXMatrix;
//            //UpdateAnimationFrame( m_pAnimXSkinMesh->GetFrameRoot(), &m_DXMatrix, m_pCloneFrameRoot );
//        }
//#endif
//}} robobeg : 2008-10-24
		//}}AFX

        //renderParam.lightPos		= m_LightPos;
        renderParam.fLightFlowImpact	= m_fNowPoint;
        renderParam.fLightFlowWide	= m_fWide;	

        for( HRESULT hr = m_pRenderer->BeginRender( renderParam ); SUCCEEDED( hr ); hr = m_pRenderer->NextRender() )
        {	
            DrawFrame(m_pLightFlow, modelIndex);
        }//for


    }    
#endif

    if( !m_vecAlphaSkinMesh.empty() )
    {
        CKTDGXRenderer::RenderParam renderParam = m_RenderParam;
        renderParam.bAlphaBlend = true;

        for( HRESULT hr = m_pRenderer->BeginRender( renderParam ); SUCCEEDED( hr ); hr = m_pRenderer->NextRender() )
        {
            for(int alphaIndex = 0; alphaIndex < (int)m_vecAlphaSkinMesh.size(); ++alphaIndex)
            {	
                Alpha_SkinMesh &stAlphaObj = m_vecAlphaSkinMesh[alphaIndex];  
            
                for( int j = 0; j < (int) stAlphaObj.m_pAlphaSkinMesh->GetDrawFrameNum(); j++ )
                {		
                    DrawFrame(stAlphaObj.m_pAlphaSkinMesh->GetDrawFrame( j ), stAlphaObj.m_indexMode);
                } //for
            } //for
        } //for
    } //if

#ifdef ARA_CHARACTER_BASE
	if( !m_vecRenderParamSkinMesh.empty() )
	{
		CKTDGXRenderer::RenderParam renderParam = m_RenderParam;
		renderParam.renderType = CKTDGXRenderer::RT_REAL_COLOR;
		renderParam.bAlphaBlend = true;
		renderParam.bZEnable = true;
		renderParam.bZWriteEnable = true;
		renderParam.cullMode = D3DCULL_CCW;
		renderParam.srcBlend = D3DBLEND_SRCALPHA;
		renderParam.destBlend = D3DBLEND_DESTALPHA;
		
		for( HRESULT hr = m_pRenderer->BeginRender( renderParam ); SUCCEEDED( hr ); hr = m_pRenderer->NextRender() )
		{
			for( int RenderParamIndex = 0; RenderParamIndex < (int)m_vecRenderParamSkinMesh.size(); ++RenderParamIndex )
			{	
				RenderParam_SkinMesh &stRenderParamObj = m_vecRenderParamSkinMesh[RenderParamIndex];

				for( int j = 0; j < (int) stRenderParamObj.m_pRenderParamSkinMesh->GetDrawFrameNum(); j++ )
				{		
					DrawFrame( stRenderParamObj.m_pRenderParamSkinMesh->GetDrawFrame( j ), stRenderParamObj.m_indexMode );
				} //for
			} //for
		} //for
	} //if
#endif
    
#ifdef SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
	// June SMRT_UV_ANIM pass ( 31d30380 ): accumulators advance once per drawn frame, wrap to 0 past 1.0 ( negative speeds
	// wrap at -1.0, the same texture phase ), texOffsetStage0.y += value, .x += speedX, .y += speedY.
	for( int uvIndex = 0; uvIndex < (int)m_vecUVAnimSkinMesh.size(); ++uvIndex )
	{
		Alpha_SkinMesh& stUVObj = m_vecUVAnimSkinMesh[uvIndex];
		CKTDXDeviceXSkinMesh* pUVMesh = stUVObj.m_pAlphaSkinMesh;
		if( NULL == pUVMesh )
			continue;

		m_fSMUVAccValue += pUVMesh->GetSMValue();
		if( m_fSMUVAccValue > 1.f || m_fSMUVAccValue < -1.f )
			m_fSMUVAccValue = 0.f;
		m_fSMUVAccU += pUVMesh->GetSMUVSpeed().x;
		if( m_fSMUVAccU > 1.f || m_fSMUVAccU < -1.f )
			m_fSMUVAccU = 0.f;
		m_fSMUVAccV += pUVMesh->GetSMUVSpeed().y;
		if( m_fSMUVAccV > 1.f || m_fSMUVAccV < -1.f )
			m_fSMUVAccV = 0.f;

		CKTDGXRenderer::RenderParam renderParam = m_RenderParam;
		renderParam.texOffsetStage0.y += m_fSMUVAccValue;
		renderParam.texOffsetStage0.x += m_fSMUVAccU;
		renderParam.texOffsetStage0.y += m_fSMUVAccV;
		renderParam.renderType = CKTDGXRenderer::RT_REAL_COLOR;
		renderParam.bAlphaBlend = pUVMesh->GetSMUVAlpha();
		renderParam.srcBlend = D3DBLEND_SRCALPHA;
		renderParam.destBlend = D3DBLEND_DESTALPHA;

		for( HRESULT hr = m_pRenderer->BeginRender( renderParam ); SUCCEEDED( hr ); hr = m_pRenderer->NextRender() )
		{
			for( int j = 0; j < (int) pUVMesh->GetDrawFrameNum(); j++ )
			{
				DrawFrame( pUVMesh->GetDrawFrame( j ), stUVObj.m_indexMode );
			}
		}
	}
#endif SERV_IRUHADEV_JUNE_ATTACH_SKIN_MESH_ANIM
//	return S_OK;
}

//}} robobeg : 2008-10-16



void CKTDGXSkinAnim::DrawFrame( CKTDXDeviceXSkinMesh::MultiAnimFrame* pModelFrame, int modelIndex )
{
	KTDXPROFILE();
#ifdef  X2OPTIMIZE_KTDGXSKINANIM_CRAHSH_BUG_FIX
    if ( modelIndex < 0 || modelIndex >= (int) m_pModelXSkinMeshList.size() || m_pModelXSkinMeshList[modelIndex] == NULL || pModelFrame == NULL )
        return;
#endif  X2OPTIMIZE_KTDGXSKINANIM_CRAHSH_BUG_FIX
    m_pRenderer->DrawFrame( *m_pModelXSkinMeshList[modelIndex], *pModelFrame, m_pModelTexChangeXETList[modelIndex]
        , m_pModelMultiTexXETList[modelIndex], m_pModelAniDataList[modelIndex], GetNowAnimationTime(), m_ModelDetailPercent
        , m_bUseTex, m_pAnimAniXET, m_NowAnimName.c_str() );
}//CKTDGXSkinAnim::DrawFrame()


void CKTDGXSkinAnim::Play( XSKIN_ANIM_PLAYTYPE playType )
{
	m_AnimState		= XAS_PLAYING;
	m_PlayType		= playType;
	m_NowPlayCount	= 0;
}

void CKTDGXSkinAnim::Wait()
{
	m_AnimState = XAS_WAIT;
}

void CKTDGXSkinAnim::Stop()
{
	m_AnimState = XAS_STOP;
}

void CKTDGXSkinAnim::Reset( float fAnimTime )
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    if ( m_pAC == NULL )
        return;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_pAC->ResetTime();
	m_pAC->AdvanceTime( fAnimTime, NULL );
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_fAnimBeforeTime = 0.f;
    m_fAnimGlobalTime = (float) m_pAC->GetTime();
	D3DXTRACK_DESC td;
	m_pAC->GetTrackDesc( m_NowTrack, &td );
    m_fAnimCurrTime = (float) td.Position;
    if ( m_fAnimCurrTime < 0.f )
        m_fAnimCurrTime = 0.f;
    m_fAnimCurrSpeed = td.Speed;
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fAnimCurrTime = fAnimTime;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
}

bool CKTDGXSkinAnim::ChangeAnim( int index, bool transition, float fStartTime )
{
	KTDXPROFILE();
	bool retval;
	LPD3DXANIMATIONSET pAS;
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	// June2017 31d2b5e0: the index is checked against the anim set vector ( signed, as June ), March indexes unchecked
	if( !( index < (int)m_vAnimSetList.size() ) )
		return false;
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	pAS = GetAnimSet(index);
	if( pAS == NULL )
	{
		return false;
	}

	WCHAR wszAnimName[ MAX_PATH ] = {0,};
	MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wszAnimName, MAX_PATH );

	m_NowAnimName = wszAnimName;
	m_NowAnimIndex = index;

	retval = ChangeAnim( pAS, transition, fStartTime );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	m_bJuneLastTransition = transition;		// June2017 31d2b5e0 @31d2b6b1
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK

	return retval;
}

bool CKTDGXSkinAnim::ChangeAnim( const WCHAR* name, bool transition, float fStartTime )
{
	KTDXPROFILE();
	bool retval; 
	LPD3DXANIMATIONSET pAS;
	wstring tempName = name;
	//MakeUpperCase( tempName );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	// June2017 31d33fd0: 31d39100 finds without inserting ( March's GetAnimSet( name ) is operator[] and leaves a NULL entry behind )
	{
		std::map<wstring, LPD3DXANIMATIONSET>::iterator itJuneAnimSet = m_mAnimSetList.find( tempName );
		pAS = ( itJuneAnimSet != m_mAnimSetList.end() ) ? itJuneAnimSet->second : NULL;
	}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	pAS = GetAnimSet(tempName.c_str());
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	if( pAS == NULL )
	{
		return false;
	}

	m_NowAnimName = tempName;
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	m_bJuneLastTransition = transition;		// June2017 31d33fd0 @31d340d4 ( ChangeAnim( pAS ) below writes the same value )
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	for( int i = 0; i < (int)m_vAnimSetList.size(); i++ )
	{
		if( pAS == m_vAnimSetList[i] )
		{
			m_NowAnimIndex = i;
			break;
		}
	}

	retval = ChangeAnim( pAS, transition, fStartTime );	

	return retval;
}

bool CKTDGXSkinAnim::ChangeAnim( LPD3DXANIMATIONSET pAS, bool transition, float fStartTime )
{
	KTDXPROFILE();
	DWORD m_NewTrack = ( m_NowTrack == 0 ? 1 : 0 );
	if( transition == false )
		m_NewTrack = m_NowTrack;

	m_MotionMoveNow.x = 0.0f;
	m_MotionMoveNow.y = 0.0f;
	m_MotionMoveNow.z = 0.0f;

	m_MotionMoveBack.x = 0.0f;
	m_MotionMoveBack.y = 0.0f;
	m_MotionMoveBack.z = 0.0f;

	m_fAnimGlobalTime	= 0.0f;
	m_fAnimCurrTime		= 0.0f;
	m_fAnimBeforeTime	= 0.0f;
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_fAnimCurrSpeed    = 1.f;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_pAC->ResetTime();

#ifdef ANIMATIONCONTROLLER_FIX
	// D3DXANIMATIONCONTROLLER 관련 수정-하형진
	m_pAC->UnkeyAllTrackEvents( 0 );
	m_pAC->UnkeyAllTrackEvents( 1 );
#else
	m_pAC->UnkeyAllTrackEvents( m_NowTrack );
	m_pAC->UnkeyAllTrackEvents( m_NewTrack );
#endif ANIMATIONCONTROLLER_FIX

	if( transition == true )
	{
		m_pAC->KeyTrackEnable( m_NowTrack, FALSE, m_fAnimGlobalTime + (m_fTransitionTime/m_fPlaySpeed) );
		m_pAC->KeyTrackSpeed( m_NowTrack, 0.0f, m_fAnimGlobalTime, (m_fTransitionTime/m_fPlaySpeed), D3DXTRANSITION_LINEAR );
		m_pAC->KeyTrackWeight( m_NowTrack, 0.0f, m_fAnimGlobalTime, (m_fTransitionTime/m_fPlaySpeed), D3DXTRANSITION_LINEAR );

		m_pAC->SetTrackAnimationSet( m_NewTrack, pAS );
		m_pAC->SetTrackEnable( m_NewTrack, TRUE );

#ifdef ANIMATIONCONTROLLER_FIX
		// D3DXANIMATIONCONTROLLER 관련 수정-하형진
		m_pAC->SetTrackSpeed( m_NewTrack, 0.f );
		m_pAC->SetTrackWeight( m_NewTrack, 0.f );
#endif ANIMATIONCONTROLLER_FIX

		m_pAC->KeyTrackSpeed( m_NewTrack, m_fPlaySpeed, m_fAnimGlobalTime, (m_fTransitionTime/m_fPlaySpeed), D3DXTRANSITION_LINEAR );
		m_pAC->KeyTrackWeight( m_NewTrack, 1.0f, m_fAnimGlobalTime, (m_fTransitionTime/m_fPlaySpeed), D3DXTRANSITION_LINEAR );

	}
	else
	{
#ifdef ANIMATIONCONTROLLER_FIX
		// D3DXANIMATIONCONTROLLER 관련 수정-하형진
		DWORD dwOldTrack = ( m_NewTrack == 0 ? 1 : 0 );
		m_pAC->SetTrackEnable( dwOldTrack, FALSE );
#endif ANIMATIONCONTROLLER_FIX

		m_pAC->SetTrackAnimationSet( m_NewTrack, pAS );
		m_pAC->SetTrackEnable( m_NewTrack, TRUE );
		m_pAC->KeyTrackSpeed( m_NewTrack, m_fPlaySpeed, m_fAnimGlobalTime, 0.00001f, D3DXTRANSITION_LINEAR );
		m_pAC->KeyTrackWeight( m_NewTrack, 1.0f, m_fAnimGlobalTime, 0.00001f, D3DXTRANSITION_LINEAR );
	}

	m_pAC->SetTrackPosition( m_NewTrack, fStartTime );

	m_fAnimMaxTime = (float)pAS->GetPeriod();

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    D3DXTRACK_DESC td;
    m_pAC->GetTrackDesc( m_NewTrack, &td );
	//m_fAnimCurrTime = (float)td.Position;
 //   if ( m_fAnimCurrTime < 0.f )
 //       m_fAnimCurrTime = 0.f;
 //   if ( m_fAnimCurrTime > m_fAnimMaxTime )
 //       m_fAnimCurrTime = m_fAnimMaxTime;
    m_fAnimCurrSpeed = (float)td.Speed;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

	for( int i = 0; i < (int)m_pModelAniXETList.size(); i++ )
	{
		CKTDXDeviceXET* pCKTDXDeviceXET = m_pModelAniXETList[i];
		if( pCKTDXDeviceXET == NULL )
			continue;

		const CKTDXDeviceXET::AniData* pAniData = pCKTDXDeviceXET->GetAniData( m_NowAnimName.c_str() );
		m_pModelAniDataList[i] = pAniData;
	}

	if( m_pAnimAniXET != NULL )
	{
		const CKTDXDeviceXET::AniData* pAniData = m_pAnimAniXET->GetAniData( m_NowAnimName.c_str() );
		if( pAniData != NULL )
		{
			if( pAniData->fTransitionTime == 0.0f )
				m_fTransitionTime = m_fAnimMaxTime / 10.0f;
			if( pAniData->fTransitionTime > m_fAnimMaxTime / 20.0f )
				m_fTransitionTime = pAniData->fTransitionTime;
			else
				m_fTransitionTime = m_fAnimMaxTime / 20.0f;
		}
		else
		{
			m_fTransitionTime = m_fAnimMaxTime / 10.0f;
		}
	}
	else
	{
		m_fTransitionTime = m_fAnimMaxTime / 10.0f;
	}	

#ifdef FIX_SKIN_ANIM_ONE_WAIT_BUG // 오현빈 // 2013-07-19
	// 0.3초 이하의 지속시간을 가진 모션을 ONE_WAIT로 설정 했을 때, 첫 프레임에서 멈추는 문제 수정
	// 오차범위로 체크하는 값이 한프레임인 0.03보다 낮아서 체크 무시 되는 경우가 발생.
	// m_fTransitionTime의 최저값을 0.03으로 수정
	m_fTransitionTime = max( m_fTransitionTime, 0.03f );
#endif // FIX_SKIN_ANIM_ONE_WAIT_BUG

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	m_bJuneLastTransition = transition;		// June2017 31d29340 @31d296fc
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK
	m_NowTrack = m_NewTrack;

	return true;
}

CKTDXDeviceXSkinMesh::MultiAnimFrame* CKTDGXSkinAnim::GetCloneFrame( const WCHAR* name )
{
	KTDXPROFILE();
	if( NULL == name )
		return NULL;
		
	map<wstring, CKTDXDeviceXSkinMesh::MultiAnimFrame*>::iterator iter = m_mCloneFrameMap.find( name );
	if( iter != m_mCloneFrameMap.end() )
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = iter->second;
		return pFrame;
	}

	return NULL;
}



void CKTDGXSkinAnim::ResetAnimSet()
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	m_mapJuneAnimSetBackup.clear();		// June2017 31d31c10 @31d31c5c
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;

	for( int i = 0; i < (int)m_pAC->GetNumAnimationSets(); i++ )
	{
		pAS = NULL;
		m_vAnimSetList.push_back( pAS );

		hr = m_pAC->GetAnimationSet( i, &pAS );
		if( FAILED( hr ) )
			continue;

		m_vAnimSetList[i] = pAS;

		WCHAR wszAnimName[ MAX_PATH ];
		MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wszAnimName, MAX_PATH );
		//MakeUpperCase( wszAnimName );

		if( m_mAnimSetList.find( wszAnimName ) == m_mAnimSetList.end() )
		{
			m_mAnimSetList.insert( std::make_pair( wszAnimName, pAS ) );
		}
		if( m_mIndexByName.find( wszAnimName ) == m_mIndexByName.end() )
		{
			m_mIndexByName.insert( std::make_pair( wszAnimName, i ) );
		}
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
		// June2017 31d31c10 @31d31dbb-31d31dcc: ( index, anim set ) into the +0x294 backup map, no AddRef
		m_mapJuneAnimSetBackup.insert( std::make_pair( i, pAS ) );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	}

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	{
		static int s_iJuneLogBackup = 0;
		if( s_iJuneLogBackup < 3 )
		{
			++s_iJuneLogBackup;
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM ResetAnimSet backup map " << (int)m_mapJuneAnimSetBackup.size() << " sets" << fileout;
		}
	}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS
	m_NowTrack				= 0;
	m_NewTrack				= 1;

	m_fAnimGlobalTime		= 0.0f;
	m_fAnimCurrTime			= 0.0f;
	m_fAnimBeforeTime		= 0.0f;
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_fAnimCurrSpeed        = 1.f;
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_fAnimMaxTime			= 0.0f;
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_BONE_REFRESH
// June2017 31d29950: the UpdateAnimationFrameStructure frame walk without the AdvanceTime / track read. False without the anim
// mesh ( +0x190 ) or the clone root ( +0x324 ). Only caller: CXMeshInstance 31c83520, after a main-effect scale change.
bool CKTDGXSkinAnim::JuneRefreshBones()
{
	if( NULL == m_pAnimXSkinMesh )
		return false;
	if( NULL == m_pCloneFrameRoot )
		return false;

	if( m_bUseDXMatrix == false )
	{
		D3DXMATRIX temp = GetMatrix().GetMatrix( m_BillBoardType );
		UpdateAnimationFrame( m_pAnimXSkinMesh->GetFrameRoot(), &temp, m_pCloneFrameRoot );
	}
	else
	{
		UpdateAnimationFrame( m_pAnimXSkinMesh->GetFrameRoot(), &m_DXMatrix, m_pCloneFrameRoot );
	}

	static int s_iJuneLogRefresh = 0;
	if( s_iJuneLogRefresh < 3 )
	{
		++s_iJuneLogRefresh;
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SKANIM bone refresh" << fileout;
	}
	return true;
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_BONE_REFRESH
bool CKTDGXSkinAnim::EventTimer( float fTime )
{
	if( m_fAnimBeforeTime < fTime
		&& m_fAnimCurrTime >= fTime )
		return true;
	else
		return false;
}

void CKTDGXSkinAnim::SetModelDetailPercent( int detailPercent )
{
	if ( detailPercent >= 100 )
		detailPercent = 100;

	if ( detailPercent <= 0 )
		detailPercent = 0;

	m_ModelDetailPercent = detailPercent;
}





#ifdef BOOST_SINGLETON_POOL_TEST
	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CKTDGXSkinAnim)> CKTDGXSkinAnimPool;



	void* CKTDGXSkinAnim::operator new(size_t s)
	{
		return CKTDGXSkinAnimPool::malloc();
	}

	void CKTDGXSkinAnim::operator delete(void *p)
	{
		CKTDGXSkinAnimPool::free(p);
	}
#endif BOOST_SINGLETON_POOL_TEST







#ifdef SKIN_LIGHTFLOW
void CKTDGXSkinAnim::SetChildLightFlow(CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame)
{
    if(pFrame == NULL)
        return;

    if(pFrame->pMeshContainer)
    {
        m_bLightFlow = true;
        m_pLightFlow = pFrame;
        return;
    }
    else
    {
        if(pFrame->pFrameSibling)
            SetChildLightFlow((CKTDXDeviceXSkinMesh::MultiAnimFrame*)pFrame->pFrameSibling);
        
        if(pFrame->pFrameFirstChild)
            SetChildLightFlow((CKTDXDeviceXSkinMesh::MultiAnimFrame*)pFrame->pFrameFirstChild);
    }
    
}

void CKTDGXSkinAnim::SetLightFlow(wstring szName, CKTDGXRenderer::RENDER_TYPE flowType) 
{   
    if(szName.compare(L"") == 0)
        return;

    LPCSTR			tempName;    
    WCHAR			wszName[MAX_PATH] = L"";
    wstring         frameName;

    m_wstrLightFlowName = szName; 
    m_lightFlowType     = flowType;    

    m_fFlowMin = 0.0f;
    m_fFlowMax = 3000.0f;
    m_fFlowSpeed = 300.0f;
    m_fWide = 10.0f;
    m_coColor = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);

    for( int i = 0; i < (int)m_pModelXSkinMeshList.size(); i++ )
    {        
        CKTDXDeviceXSkinMesh* pXSkinMesh = m_pModelXSkinMeshList[i];
        int maxFrameNum = pXSkinMesh->GetFrameNum();

        for( int j = 0; j < maxFrameNum; j++ )
        {            
          
            CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pXSkinMesh->GetFrame(j);
#ifdef  X2OPTIMIZE_SKIN_ANIM_MESH_CRASH_BUG_FIX
            if ( pFrame == NULL )
                continue;
#endif  X2OPTIMIZE_SKIN_ANIM_MESH_CRASH_BUG_FIX
            tempName = pFrame->Name;
            MultiByteToWideChar( CP_ACP, 0, tempName, -1, wszName, MAX_PATH);
            frameName = wszName;

            if(m_wstrLightFlowName.compare(frameName.c_str()) == 0)
            {
                if(pFrame->pMeshContainer)
                {
                    m_bLightFlow = true;
                    m_pLightFlow = pFrame;
                    return;
                }
                else
                {
                    if(pFrame->pFrameFirstChild)
                        SetChildLightFlow((CKTDXDeviceXSkinMesh::MultiAnimFrame*)pFrame->pFrameFirstChild);
                }                
                return;
            }
        }
    }
}

HRESULT CKTDGXSkinAnim::LightFlowFrameMove(double fTime, float fElapsedTime)
{
    HRESULT hr = S_OK;

    m_fNowPoint += m_fFlowSpeed * fElapsedTime; 

    if( m_fNowPoint < m_fFlowMin )
        m_fNowPoint = m_fFlowMax;
    else if( m_fNowPoint > m_fFlowMax )
        m_fNowPoint = m_fFlowMin;

    return hr;
}

#endif

#if defined(WORLD_TOOL) || defined(X2TOOL)
bool CKTDGXSkinAnim::GetAnimNameList( std::vector<std::wstring>& vecAnimNameList )
{
	if( m_pAnimXSkinMesh == NULL )
	{
		return false;
	}

	LPD3DXANIMATIONCONTROLLER	pAC;
	LPD3DXANIMATIONSET			pAnimSet;
	UINT			aniNum;
	LPCSTR			szName;
	std::wstring	wstrName;
	WCHAR			wszName[128] = L"";

	pAC		= m_pAnimXSkinMesh->GetCloneAC();
	aniNum	= pAC->GetNumAnimationSets();

	for( UINT i = 0; i < aniNum; ++i )
	{
		pAC->GetAnimationSet( i, &pAnimSet );
		szName = pAnimSet->GetName();

		MultiByteToWideChar( CP_ACP, 0, szName, -1, wszName, MAX_PATH);

		wstrName = wszName;
		vecAnimNameList.push_back( wstrName );

		pAnimSet->Release();
	}

	SAFE_RELEASE( pAC);

	return true;
}

#endif

#ifdef MODIFY_AFTER_IMAGE
/** @function : ClearModelXSkinMeshList
	@brief : XSkinMeshList 초기화 함수.

			 애프터 이미지 갱신 시 비용을 줄이기 위해 만든 함수.
			 다른 곳에서 불려졌을 때의 오류를 방지하기 위해 
			 m_CollisionDataList, m_AttackDataList 체크 함
*/
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
// June2017 31d2ba80 ( only caller CKTDGXSkinAfterImage::OnFrameMove ): close every stored mesh and XET, empty the five lists,
// and delete the collision / attack data ( March only asserted that both lists were empty )
void CKTDGXSkinAnim::ClearModelXSkinMeshList()
{
	for( UINT uJune = 0; uJune < m_pModelXSkinMeshList.size(); ++uJune )
		JuneCloseDevice( m_pModelXSkinMeshList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelAniXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelAniXETList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelMultiTexXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelMultiTexXETList[uJune] );
	for( UINT uJune = 0; uJune < m_pModelTexChangeXETList.size(); ++uJune )
		JuneCloseDevice( m_pModelTexChangeXETList[uJune] );

	m_pModelXSkinMeshList.clear();
	m_pModelAniXETList.clear();
	m_pModelAniDataList.clear();
	m_pModelMultiTexXETList.clear();
	m_pModelTexChangeXETList.clear();

	BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pCollision, m_CollisionDataList )
	{
		SAFE_DELETE( pCollision );
	}
	m_CollisionDataList.clear();

	BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pAttack, m_AttackDataList )
	{
		SAFE_DELETE( pAttack );
	}
	m_AttackDataList.clear();
}
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
void CKTDGXSkinAnim::ClearModelXSkinMeshList()
{
	if( false == m_CollisionDataList.empty() ||
		false == m_AttackDataList.empty() )
	{
		ASSERT(!"ClearModelXSkinMeshList 사용 할 수 없는 상태입니다.");
	}

	m_pModelXSkinMeshList.clear();
	m_pModelAniXETList.clear();
	m_pModelAniDataList.clear();
	m_pModelMultiTexXETList.clear();
	m_pModelTexChangeXETList.clear();
}
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
#endif // MODIFY_AFTER_IMAGE