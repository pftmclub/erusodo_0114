#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
#include "KTDGEnvBoxMap.h"

extern CKTDGEffectStateManager* g_pEffectStateManager;

/// June engine remainder Phase R3, flag SERV_IRUHADEV_JUNE_RENDER_ENVBOX. Every function names the June address it is read from
/// ( unpacked_x2.exe, image base 31aa0000 ); the evidence rows are in june2017_re/funcmap.tsv, tag "Engine R3".

namespace
{
	const float JUNE_ENVBOX_DEFAULT_LENGTH	= 100.0f;	/// June 334355b8: BLEND_LENGTH default, and the BOX_SCALE default of the entry ctor
	const float JUNE_ENVBOX_DEFAULT_WIDE	= 3.0f;		/// June 33436700: RIM_LIGHT_WIDE default
	const float JUNE_ENVBOX_MIN_LENGTH		= 0.001f;	/// June 33431564: BLEND_LENGTH is raised to this
	const float JUNE_ENVBOX_REMAIN_EPSILON	= 0.01f;	/// June 33437810: the weight-stacking loops stop once the remaining weight is not above it
	const float JUNE_ENVBOX_DEG_TO_RAD		= 0.017453f;/// June 3344da70 (a float constant stored as a double)

	/// the June Lua getters ( FUN_32fea370 / FUN_32fe9fd0 / FUN_32fe9e40 / FUN_32feae90 / FUN_31c7e850 ) read a key of the table argument and
	/// leave the default untouched when the key is missing
	bool GetTableFloat( lua_State* L, int iTable, const char* pKey, float& fOut )
	{
		lua_getfield( L, iTable, pKey );
		const bool bOk = ( 0 != lua_isnumber( L, -1 ) );
		if( bOk )
			fOut = (float) lua_tonumber( L, -1 );
		lua_pop( L, 1 );
		return bOk;
	}

	bool GetTableInt( lua_State* L, int iTable, const char* pKey, int& iOut )
	{
		lua_getfield( L, iTable, pKey );
		const bool bOk = ( 0 != lua_isnumber( L, -1 ) );
		if( bOk )
			iOut = (int) lua_tonumber( L, -1 );
		lua_pop( L, 1 );
		return bOk;
	}

	/// June 32fe9e40 ( KLuaManager::GetValue( bool ) ): numbers only ( lua_isnumber 32fe4d70 ), value = ftol ( 32fe25f0, truncation ) != 0.
	/// A Lua boolean true / false is not a number, so the key counts as missing and the default stays ( scripts use the numeric TRUE / FALSE ).
	/// R3 review: the port first also took booleans and compared the double with 0.0; corrected to June.
	bool GetTableBool( lua_State* L, int iTable, const char* pKey, bool& bOut )
	{
		lua_getfield( L, iTable, pKey );
		const bool bOk = ( 0 != lua_isnumber( L, -1 ) );
		if( bOk )
			bOut = ( 0 != (int) lua_tonumber( L, -1 ) );
		lua_pop( L, 1 );
		return bOk;
	}

	bool GetTableString( lua_State* L, int iTable, const char* pKey, wstring& wstrOut )
	{
		lua_getfield( L, iTable, pKey );
		const bool bOk = ( 0 != lua_isstring( L, -1 ) );
		if( bOk )
			ConvertUtf8ToWCHAR( wstrOut, lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
		return bOk;
	}

	bool GetTableVec3( lua_State* L, int iTable, const char* pKey, D3DXVECTOR3& vOut )
	{
		lua_getfield( L, iTable, pKey );
		const bool bOk = ( 0 != lua_isuserdata( L, -1 ) );
		if( bOk )
			vOut = lua_tinker::read<D3DXVECTOR3>( L, -1 );
		lua_pop( L, 1 );
		return bOk;
	}

	std::string WideToAnsi( const wstring& wstr )
	{
		std::string str;
		if( wstr.empty() )
			return str;
		const int iLen = WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), (int) wstr.size(), NULL, 0, NULL, NULL );
		if( iLen > 0 )
		{
			str.resize( iLen );
			WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), (int) wstr.size(), &str[0], iLen, NULL, NULL );
		}
		return str;
	}

	/// June 31d08f90: the std::sort predicate of LoadScript's box list
	bool BoxLess( const CKTDGEnvBoxMap::EnvBox* pLeft, const CKTDGEnvBoxMap::EnvBox* pRight )
	{
		if( pLeft->m_fPriority < pRight->m_fPriority )
			return true;
		if( pLeft->m_fPriority == pRight->m_fPriority )
			return pLeft->m_iId < pRight->m_iId;
		return false;
	}
}


/// June entry ctor 31d0d100
CKTDGEnvBoxMap::EnvBox::EnvBox()
: m_iId( -1 )
, m_wstrName( L"EnvBox" )
, m_vPos( 0.f, 0.f, 0.f )
, m_vRot( 0.f, 0.f, 0.f )
, m_vScale( JUNE_ENVBOX_DEFAULT_LENGTH, JUNE_ENVBOX_DEFAULT_LENGTH, JUNE_ENVBOX_DEFAULT_LENGTH )
, m_pLut( NULL )
, m_fBlendLength( JUNE_ENVBOX_DEFAULT_LENGTH )
, m_fBlendWeight( 1.0f )
, m_fPriority( 0.f )
, m_bEnabled( true )
, m_bRimEnabled( false )
, m_vRimColor( 1.f, 1.f, 1.f )
, m_fRimWide( JUNE_ENVBOX_DEFAULT_WIDE )
, m_fRimPower( 1.0f )
, m_fRimForced( 1.0f )
{
	D3DXMatrixIdentity( &m_mat );
}

/// June 31d0cf80: matrix = RotationYawPitchRoll( yaw = rot.y, pitch = rot.x, roll = rot.z, all in degrees * 0.017453 ) * Translation( pos )
void CKTDGEnvBoxMap::EnvBox::RebuildMatrix()
{
	D3DXMATRIX matRot;
	D3DXMATRIX matTrans;
	D3DXMatrixRotationYawPitchRoll( &matRot, m_vRot.y * JUNE_ENVBOX_DEG_TO_RAD, m_vRot.x * JUNE_ENVBOX_DEG_TO_RAD, m_vRot.z * JUNE_ENVBOX_DEG_TO_RAD );
	D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	D3DXMatrixMultiply( &m_mat, &matRot, &matTrans );
}


/// June ctor 31d09b30
CKTDGEnvBoxMap::CKTDGEnvBoxMap()
: CKTDGObject( NULL )
, m_vRimColor( 0.f, 0.f, 0.f )
, m_fRimPower( 0.f )
, m_fRimWide( 0.f )
, m_fRimForced( 0.f )
, m_vWeights( 0.f, 0.f, 0.f, 0.f )
, m_vEtc( 0.f, 0.f, 0.f, 0.f )
, m_pEffect( NULL )
, m_pVB( NULL )
, m_bInPass( false )
, m_iSavedStackTop( -1 )
, m_bLoggedNoEffect( false )
{
	m_apBlendedLut[0] = NULL;
	m_apBlendedLut[1] = NULL;
	for( int i = 0; i < 2; ++i )
		m_ahTechnique[i] = NULL;
	for( int i = 0; i < 5; ++i )
		m_ahParam[i] = NULL;

	CreateQuad();	/// June 31d09c55; a failure is logged there and leaves m_pVB NULL ( OnFrameRender_Prepare then says NORENDER )
}

/// June dtor 31d09dc0
CKTDGEnvBoxMap::~CKTDGEnvBoxMap( void )
{
	for( UINT i = 0; i < m_vecBox.size(); ++i )
		SAFE_DELETE( m_vecBox[i] );
	m_vecBox.clear();

	CloseLutTextures();
	for( UINT i = 0; i < m_vecLut.size(); ++i )
		SAFE_DELETE( m_vecLut[i] );
	m_vecLut.clear();

	SAFE_RELEASE( m_pEffect );

	if( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDeviceManager() )
	{
		for( int i = 0; i < 2; ++i )
		{
			if( NULL != m_apBlendedLut[i] )
			{
				g_pKTDXApp->GetDeviceManager()->CloseDeviceByPointer( m_apBlendedLut[i] );
				m_apBlendedLut[i] = NULL;
			}
		}
	}

	ClearRendererLut();
	if( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDGManager() && NULL != g_pKTDXApp->GetDGManager()->GetXRenderer() )
		g_pKTDXApp->GetDGManager()->GetXRenderer()->SetEnvBoxRim( false, D3DXVECTOR3( 0.f, 0.f, 0.f ), 0.f, 0.f, 0.f, true );	/// +0x64 only, the colour members keep their values

	SAFE_RELEASE( m_pVB );
}

void CKTDGEnvBoxMap::ClearRendererLut()
{
	if( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDGManager() && NULL != g_pKTDXApp->GetDGManager()->GetXRenderer() )
		g_pKTDXApp->GetDGManager()->GetXRenderer()->SetEnvBoxLut( false, NULL, NULL );		/// +0x5c = +0x60 = 0, +0x59 = 0
}

void CKTDGEnvBoxMap::CloseLutTextures()
{
	if( NULL == g_pKTDXApp || NULL == g_pKTDXApp->GetDeviceManager() )
		return;

	for( UINT i = 0; i < m_vecLut.size(); ++i )
	{
		if( NULL != m_vecLut[i] && NULL != m_vecLut[i]->m_pTexture )
		{
			g_pKTDXApp->GetDeviceManager()->CloseDeviceByPointer( m_vecLut[i]->m_pTexture );
			m_vecLut[i]->m_pTexture = NULL;
		}
	}
}

/// June 31d09900: a 4-vertex strip ( x, y, z, u, v ), FVF XYZ | TEX1, managed pool
bool CKTDGEnvBoxMap::CreateQuad()
{
	HRESULT hr = g_pKTDXApp->GetDevice()->CreateVertexBuffer( sizeof( Vertex ) * 4, 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, NULL );
	if( FAILED( hr ) )
	{
		ErrorLog( KEM_ERROR24 );
		return false;
	}

	Vertex* pVertex = NULL;
	hr = m_pVB->Lock( 0, sizeof( Vertex ) * 4, (void**) &pVertex, 0 );
	if( FAILED( hr ) )
	{
		ErrorLog( KEM_ERROR25 );
		return false;
	}

	const Vertex akVertex[4] =
	{
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
		{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f,  1.0f, 1.0f, 0.0f, 0.0f },
		{  1.0f,  1.0f, 1.0f, 1.0f, 0.0f },
	};
	memcpy( pVertex, akVertex, sizeof( akVertex ) );

	hr = m_pVB->Unlock();
	if( FAILED( hr ) )
	{
		ErrorLog( KEM_ERROR26 );
		return false;
	}
	return true;
}


/// June 31d0b200 ( slot 6 ): the two 256 x 16 blend targets, the LUT textures, XEnvBoxRenderer.fx
HRESULT CKTDGEnvBoxMap::OnResetDevice()
{
	if( NULL == m_apBlendedLut[0] )
		m_apBlendedLut[0] = g_pKTDXApp->GetDeviceManager()->OpenRenderTargetTexture( L"@CKTDGEnvBoxMap@BlendedLUT0@", 256, 16, (D3DFORMAT) 32 );
	if( NULL == m_apBlendedLut[1] )
		m_apBlendedLut[1] = g_pKTDXApp->GetDeviceManager()->OpenRenderTargetTexture( L"@CKTDGEnvBoxMap@BlendedLUT1@", 256, 16, (D3DFORMAT) 32 );

	CloseLutTextures();
	for( UINT i = 0; i < m_vecLut.size(); ++i )
		m_vecLut[i]->m_pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( m_vecLut[i]->m_wstrName );

	if( NULL == m_pEffect )
	{
		KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
		Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( L"XEnvBoxRenderer.fx" );
		if( NULL == Info->pRealData || Info->size <= 0 )
		{
			ErrorLogMsg( KEM_ERROR49, "XEnvBoxRenderer.fx" );		/// June 31d0b50b
			if( false == m_bLoggedNoEffect )
			{
				m_bLoggedNoEffect = true;
				KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX XEnvBoxRenderer.fx is not in any kom yet (asset-port P4a ships it): the env box map stays inactive" << fileout;
			}
			return S_OK;
		}

		LPD3DXBUFFER pErr = NULL;
		HRESULT hr = D3DXCreateEffect( g_pKTDXApp->GetDevice(), Info->pRealData, Info->size, NULL, NULL, 0x20800, NULL, &m_pEffect, &pErr );	/// June 31d0b594: flags D3DXFX_NOT_CLONEABLE | 0x20000
		if( FAILED( hr ) )
		{
			ErrorLog( KEM_ERROR50 );								/// June 31d0b5a6
			if( NULL != pErr )
				KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX XEnvBoxRenderer.fx failed to compile: " << (const char*) pErr->GetBufferPointer() << fileout;
			SAFE_RELEASE( pErr );
			SAFE_RELEASE( m_pEffect );
			return S_OK;
		}
		SAFE_RELEASE( pErr );

		ASSERT( g_pEffectStateManager != NULL );
		m_pEffect->SetStateManager( g_pEffectStateManager );		/// June 31d0b629 ( vslot 0x11c, g_pKTDXApp + 0x20c )

		static const char* s_apszTechnique[2] = { "BlendLUTTechnique", "DrawEnvBoxTechnique" };		/// June 337290d4
		static const char* s_apszParam[5] = { "g_vWeights", "g_vEtcParam", "g_mMVP", "g_vCurrSize", "g_vColor" };	/// June 337290e4
		for( int i = 0; i < 2; ++i )
			m_ahTechnique[i] = m_pEffect->GetTechniqueByName( s_apszTechnique[i] );
		for( int i = 0; i < 5; ++i )
			m_ahParam[i] = m_pEffect->GetParameterByName( NULL, s_apszParam[i] );
		m_pEffect->SetTechnique( m_ahTechnique[0] );

		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX effect loaded: techniques " << ( NULL != m_ahTechnique[0] ) << ( NULL != m_ahTechnique[1] )
			<< ", params " << ( NULL != m_ahParam[0] ) << ( NULL != m_ahParam[1] ) << ( NULL != m_ahParam[2] ) << ( NULL != m_ahParam[3] ) << ( NULL != m_ahParam[4] ) << fileout;
	}

	m_pEffect->OnResetDevice();		/// June 31d0b6f6 ( vslot 0x118 )
	return S_OK;
}

/// June 31d09040 ( slot 7 )
HRESULT CKTDGEnvBoxMap::OnLostDevice()
{
	CloseLutTextures();
	if( NULL != m_pEffect )
		return m_pEffect->OnLostDevice();
	return S_OK;
}


/// June 31d0c730: the Lua global g_pEnvBoxMap ( 33773768 ), then the script; an empty name only resets the device state and sorts the ( empty ) box list
bool CKTDGEnvBoxMap::LoadScript( const wstring& wstrFileName )
{
	if( false == wstrFileName.empty() )
	{
		KLuabinder* pLuaBinder = g_pKTDXApp->GetLuaBinder();
		lua_tinker::decl( pLuaBinder->GetLuaState(), "g_pEnvBoxMap", this );

		if( false == g_pKTDXApp->LoadAndDoMemory( pLuaBinder, wstrFileName.c_str() ) )
		{
			ErrorLogMsg( KEM_ERROR20, wstrFileName.c_str() );		/// June 31d0cf5c ( FUN_31bd0f50( 0x14 ) )
			return false;
		}
	}

	OnResetDevice();
	std::sort( m_vecBox.begin(), m_vecBox.end(), BoxLess );		/// June 31d0cecd

	KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX map loaded: script '" << WideToAnsi( wstrFileName ).c_str() << "', " << (int) m_vecBox.size() << " box(es), "
		<< (int) m_vecLut.size() << " LUT(s)" << fileout;
	return true;
}


/// June 31d0a0e0 ( Lua SetLUT ): open the texture; a texture that does not open is dropped without a message ( the device manager logs it )
void CKTDGEnvBoxMap::SetLUT( const char* pFileNameUTF8 )
{
	LutEntry* pLut = new LutEntry();
	ConvertUtf8ToWCHAR( pLut->m_wstrName, pFileNameUTF8 );
	pLut->m_pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( pLut->m_wstrName );
	if( NULL == pLut->m_pTexture )
	{
		SAFE_DELETE( pLut );
		return;
	}
	m_vecLut.push_back( pLut );
}


/// June 31d0b720 ( Lua AddEnvBox_LUA ): the argument table is the top of the Lua stack
bool CKTDGEnvBoxMap::AddEnvBox_LUA()
{
	lua_State* L = g_pKTDXApp->GetLuaBinder()->GetLuaState();
	const int iTable = lua_gettop( L );
	if( !lua_istable( L, iTable ) )
	{
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] ENVBOX AddEnvBox_LUA: no table argument" << fileout;
		return false;
	}

	EnvBox* pBox = new EnvBox();

	if( false == GetTableString( L, iTable, "BOX_NAME", pBox->m_wstrName ) )
		pBox->m_wstrName = L"EnvBox";
	if( false == GetTableVec3( L, iTable, "BOX_POSITION", pBox->m_vPos ) )
		pBox->m_vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
	if( false == GetTableVec3( L, iTable, "BOX_ROTATION", pBox->m_vRot ) )
		pBox->m_vRot = D3DXVECTOR3( 0.f, 0.f, 0.f );
	if( false == GetTableVec3( L, iTable, "BOX_SCALE", pBox->m_vScale ) )
		pBox->m_vScale = D3DXVECTOR3( 0.f, 0.f, 0.f );		/// June 31d0baee: a missing key zeroes the scale ( the ctor default is not kept )

	int iLutNum = -1;
	if( false == GetTableInt( L, iTable, "LUT_NUM", iLutNum ) )
		iLutNum = -1;
	if( false == GetTableFloat( L, iTable, "BLEND_LENGTH", pBox->m_fBlendLength ) )
		pBox->m_fBlendLength = JUNE_ENVBOX_DEFAULT_LENGTH;
	if( false == GetTableFloat( L, iTable, "BLEND_WEIGHT", pBox->m_fBlendWeight ) )
		pBox->m_fBlendWeight = 1.0f;
	if( false == GetTableFloat( L, iTable, "PRIORITY", pBox->m_fPriority ) )
		pBox->m_fPriority = 0.f;
	if( false == GetTableBool( L, iTable, "BOX_ENABLED", pBox->m_bEnabled ) )
		pBox->m_bEnabled = true;
	if( false == GetTableBool( L, iTable, "RIM_LIGHT_ENABLED", pBox->m_bRimEnabled ) )
		pBox->m_bRimEnabled = false;
	if( false == GetTableVec3( L, iTable, "RIM_LIGHT_COLOR", pBox->m_vRimColor ) )
		pBox->m_vRimColor = D3DXVECTOR3( 1.f, 1.f, 1.f );
	if( false == GetTableFloat( L, iTable, "RIM_LIGHT_WIDE", pBox->m_fRimWide ) )
		pBox->m_fRimWide = JUNE_ENVBOX_DEFAULT_WIDE;
	if( false == GetTableFloat( L, iTable, "RIM_LIGHT_POWER", pBox->m_fRimPower ) )
		pBox->m_fRimPower = 1.0f;
	if( false == GetTableFloat( L, iTable, "RIM_LIGHT_FORCED", pBox->m_fRimForced ) )
		pBox->m_fRimForced = 1.0f;

	if( iLutNum >= 0 && iLutNum < (int) m_vecLut.size() )
		pBox->m_pLut = m_vecLut[iLutNum];

	pBox->RebuildMatrix();
	m_vecBox.push_back( pBox );
	pBox->m_iId = (int) m_vecBox.size() - 1;

	/// June 31d0c669-31d0c6c8: BLEND_WEIGHT is 0 for a non-positive BLEND_LENGTH and 1 for a length of 1 or more; only 0 < length < 1 keeps
	/// the scripted weight. Then the length itself is raised to 0.001.
	float fWeight = 0.f;
	if( 0.f < pBox->m_fBlendLength )
	{
		fWeight = 1.0f;
		if( pBox->m_fBlendLength < 1.0f )
			fWeight = pBox->m_fBlendWeight;
	}
	pBox->m_fBlendWeight = fWeight;
	if( pBox->m_fBlendLength < JUNE_ENVBOX_MIN_LENGTH )
		pBox->m_fBlendLength = JUNE_ENVBOX_MIN_LENGTH;
	return true;
}


/// June 31d09130: 0 when the point is inside the box, else the largest distance beyond a face pair; -1 for an unknown or disabled box.
/// June's three axes carry a sign flip ( x row: -m11; y row: -m22; z row: -m33 ), kept as it is.
float CKTDGEnvBoxMap::GetBoxDistance( float fX, float fY, float fZ, UINT uIndex ) const
{
	if( uIndex >= m_vecBox.size() || false == m_vecBox[uIndex]->m_bEnabled )
		return -1.0f;

	const EnvBox* pBox = m_vecBox[uIndex];
	const D3DXVECTOR3 vAxisX( -pBox->m_mat._11,  pBox->m_mat._12,  pBox->m_mat._13 );
	const D3DXVECTOR3 vAxisY(  pBox->m_mat._21, -pBox->m_mat._22,  pBox->m_mat._23 );
	const D3DXVECTOR3 vAxisZ(  pBox->m_mat._31,  pBox->m_mat._32, -pBox->m_mat._33 );
	const D3DXVECTOR3 vPoint( fX, fY, fZ );

	const D3DXVECTOR3 vFaceZ = pBox->m_vPos + vAxisZ * pBox->m_vScale.z;
	const D3DXVECTOR3 vFaceY = pBox->m_vPos + vAxisY * pBox->m_vScale.y;
	const D3DXVECTOR3 vFaceX = pBox->m_vPos + vAxisX * pBox->m_vScale.x;

	const D3DXVECTOR3 vToZ = vPoint - vFaceZ;
	const D3DXVECTOR3 vToY = vPoint - vFaceY;
	const D3DXVECTOR3 vToX = vPoint - vFaceX;
	float fDistZ = D3DXVec3Dot( &vToZ, &vAxisZ );
	float fDistY = D3DXVec3Dot( &vToY, &vAxisY );
	float fDistX = D3DXVec3Dot( &vToX, &vAxisX );

	if( fDistZ < 0.f )
		fDistZ = -( pBox->m_vScale.z * 2.0f + fDistZ );
	if( fDistY < 0.f )
		fDistY = -( pBox->m_vScale.y * 2.0f + fDistY );
	if( fDistX < 0.f )
		fDistX = -( pBox->m_vScale.x * 2.0f + fDistX );

	float fMax = fDistX;
	if( fMax < fDistY )
		fMax = fDistY;
	if( fMax < fDistZ )
		fMax = fDistZ;
	return ( 0.f < fMax ) ? fMax : 0.f;
}


/// June 31d09440: CommitChanges, then the effect state manager's deferred shader-constant flush ( PostCommitChanges )
void CKTDGEnvBoxMap::CommitChanges()
{
	if( true == m_bInPass )
	{
		m_pEffect->CommitChanges();
#ifdef X2OPTIMIZE_SETSHADERCONSTANT
		g_pEffectStateManager->PostCommitChanges();
#endif X2OPTIMIZE_SETSHADERCONSTANT
	}
}

/// June 31d094c0: technique, the pass-0 vectors, the state manager, Begin( flags 7 ), BeginPass( technique index ). Returns S_OK or a failure.
HRESULT CKTDGEnvBoxMap::BeginEffect( int iTechnique )
{
	HRESULT hr = m_pEffect->SetTechnique( m_ahTechnique[iTechnique] );
	if( FAILED( hr ) )
		return hr;

	if( 0 == iTechnique )
	{
		m_pEffect->SetVector( m_ahParam[0], &m_vWeights );
		m_pEffect->SetVector( m_ahParam[1], &m_vEtc );
	}

	const int iStackTop = CKTDGStateManager::GetStackTop();
	hr = g_pEffectStateManager->PrepareEffectState();
	if( FAILED( hr ) )
	{
		CKTDGStateManager::PopStates( iStackTop );
		return hr;
	}

	UINT uPasses = 0;
	hr = m_pEffect->Begin( &uPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESAMPLERSTATE | D3DXFX_DONOTSAVESHADERSTATE );
	if( SUCCEEDED( hr ) )
	{
		m_bInPass = false;
		hr = m_pEffect->BeginPass( iTechnique );
		if( SUCCEEDED( hr ) )
		{
			m_bInPass = true;
			m_iSavedStackTop = iStackTop;
			CommitChanges();
			return S_OK;
		}
		m_pEffect->End();
	}

	g_pEffectStateManager->RestoreEffectState();
	CKTDGStateManager::PopStates( iStackTop );
	return E_FAIL;
}

/// June 31d097c0
void CKTDGEnvBoxMap::EndEffect()
{
	m_pEffect->EndPass();
	m_pEffect->End();
	g_pEffectStateManager->RestoreEffectState();
	if( m_iSavedStackTop >= 0 )
		CKTDGStateManager::PopStates( m_iSavedStackTop );
	m_iSavedStackTop = -1;
}


/// June 31d0a410: blend the grading LUTs and the rim values of every box whose blend range holds vPos, and give the result to the renderer
void CKTDGEnvBoxMap::Render( const D3DXVECTOR3& vPos )
{
	CKTDGXRenderer* pRenderer = g_pKTDXApp->GetDGManager()->GetXRenderer();

	/// 1. the boxes whose range holds the point, with their weight ( ascending box index )
	std::vector<int>	vecInRangeIndex;
	std::vector<float>	vecInRangeWeight;
	for( UINT i = 0; i < m_vecBox.size(); ++i )
	{
		const EnvBox* pBox = m_vecBox[i];
		if( false == pBox->m_bEnabled )
			continue;

		const float fDist = GetBoxDistance( vPos.x, vPos.y, vPos.z, i );
		float fWeight = 0.f;
		if( 0.f < pBox->m_fBlendWeight * ( 1.0f - fDist / pBox->m_fBlendLength ) )
			fWeight = ( 1.0f - fDist / pBox->m_fBlendLength ) * pBox->m_fBlendWeight;

		if( 0.f <= fDist && 0.f < fWeight )
		{
			vecInRangeIndex.push_back( (int) i );
			vecInRangeWeight.push_back( fWeight );
		}
	}

	/// 2. the LUT boxes, last ( highest priority ) first, until the remaining weight is used up
	std::vector<int>	vecLutIndex;
	std::vector<float>	vecLutWeight;
	float fRemain = 1.0f;
	for( int k = (int) vecInRangeIndex.size() - 1; k >= 0 && JUNE_ENVBOX_REMAIN_EPSILON < fRemain; --k )
	{
		if( NULL != m_vecBox[ vecInRangeIndex[k] ]->m_pLut )
		{
			vecLutIndex.push_back( vecInRangeIndex[k] );
			vecLutWeight.push_back( vecInRangeWeight[k] );
			fRemain = ( 1.0f - vecInRangeWeight[k] ) * fRemain;
		}
	}

	const int iNumLut = (int) vecLutIndex.size();
	if( NULL == m_apBlendedLut[0] || NULL == m_apBlendedLut[1] || iNumLut < 1 )
	{
		pRenderer->SetEnvBoxLut( false, NULL, NULL );
	}
	else
	{
		if( 1 != iNumLut || vecLutWeight[0] < 1.0f )
		{
			/// the passes blend up to 4 LUTs ( the first pass ) or 3 plus the previous result ( the next ones ) into the two 256 x 16 targets, back and forth
			int iTarget = 1;
			int iDone = 0;
			while( iDone < iNumLut )
			{
				iTarget = ( iTarget + 1 ) & 1;

				int iChunk = 0;
				if( 0 == iDone )
					iChunk = ( iNumLut > 3 ) ? 4 : iNumLut;
				else
					iChunk = ( iNumLut - iDone > 2 ) ? 3 : ( iNumLut - iDone );

				float afWeight[4] = { 0.f, 0.f, 0.f, 0.f };
				for( int i = 0; i < iChunk; ++i )
					afWeight[i] = vecLutWeight[ iNumLut - iDone - 1 - i ];

				m_vWeights = D3DXVECTOR4( afWeight[0], afWeight[1], afWeight[2], afWeight[3] );
				m_vEtc = D3DXVECTOR4( (float) iDone, (float) iChunk, 0.f, 0.f );

				m_apBlendedLut[iTarget]->BeginRender( false );
				m_apBlendedLut[iTarget]->Clear( 0xff000000 );		/// June BeginRender( 1, 1, 0, 0xff000000 ): target and depth, opaque black

				if( NULL == m_pEffect )
				{
					/// not June ( the file ships with asset-port P4a ): without the effect the pass cannot run; the target stays black
				}
				else if( SUCCEEDED( BeginEffect( 0 ) ) )
				{
					for( int i = 0; i < iChunk; ++i )
					{
						CKTDXDeviceTexture* pLutTexture = m_vecBox[ vecLutIndex[ iNumLut - iDone - 1 - i ] ]->m_pLut->m_pTexture;
						if( NULL != pLutTexture )		/// not June: a LUT that failed to reopen is skipped instead of crashing
							pLutTexture->SetDeviceTexture( i );
					}
					if( iDone > 0 )
						m_apBlendedLut[ ( iTarget + 1 ) & 1 ]->SetDeviceTexture( 3 );

					g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
					g_pKTDXApp->GetDevice()->SetStreamSource( 0, m_pVB, 0, sizeof( Vertex ) );
					g_pKTDXApp->GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
					EndEffect();
				}

				m_apBlendedLut[iTarget]->EndRender();
				iDone += iChunk;
			}

			pRenderer->SetEnvBoxLut( true, NULL, m_apBlendedLut[iTarget] );
		}
		else
		{
			pRenderer->SetEnvBoxLut( true, m_vecBox[ vecLutIndex[0] ]->m_pLut->m_pTexture, NULL );
		}
	}

	/// 3. the rim-light boxes, again last first until the weight is used up
	std::vector<int>	vecRimIndex;
	std::vector<float>	vecRimWeight;
	fRemain = 1.0f;
	for( int k = (int) vecInRangeIndex.size() - 1; k >= 0 && JUNE_ENVBOX_REMAIN_EPSILON < fRemain; --k )
	{
		if( true == m_vecBox[ vecInRangeIndex[k] ]->m_bRimEnabled )
		{
			vecRimIndex.push_back( vecInRangeIndex[k] );
			vecRimWeight.push_back( vecInRangeWeight[k] );
			fRemain = fRemain * ( 1.0f - vecInRangeWeight[k] );
		}
	}

	const int iNumRim = (int) vecRimIndex.size();
	if( iNumRim < 1 )
	{
		pRenderer->SetEnvBoxRim( false, D3DXVECTOR3( 0.f, 0.f, 0.f ), 0.f, 0.f, 0.f, true );
	}
	else
	{
		const EnvBox* pFirst = m_vecBox[ vecRimIndex[0] ];
		m_vRimColor = pFirst->m_vRimColor;
		m_fRimWide = pFirst->m_fRimWide;
		m_fRimForced = pFirst->m_fRimForced;
		if( 1 != iNumRim || vecRimWeight[0] < 1.0f )
		{
			m_fRimPower = 0.f;		/// June 31d0ac49: the blended power starts at 0, not at the first box's power
			for( int k = iNumRim - 1; k >= 0; --k )
			{
				const EnvBox* pBox = m_vecBox[ vecRimIndex[k] ];
				const float fW = vecRimWeight[k];
				m_vRimColor.x = ( 1.0f - fW ) * m_vRimColor.x + fW * pBox->m_vRimColor.x;
				m_vRimColor.y = ( 1.0f - fW ) * m_vRimColor.y + fW * pBox->m_vRimColor.y;
				m_vRimColor.z = ( 1.0f - fW ) * m_vRimColor.z + fW * pBox->m_vRimColor.z;
				m_fRimWide = ( 1.0f - fW ) * m_fRimWide + pBox->m_fRimWide * fW;
				m_fRimPower = ( 1.0f - fW ) * m_fRimPower + pBox->m_fRimPower * fW;
				m_fRimForced = ( 1.0f - fW ) * m_fRimForced + pBox->m_fRimForced * fW;
			}
		}
		else
		{
			m_fRimPower = pFirst->m_fRimPower;
		}

		pRenderer->SetEnvBoxRim( true, m_vRimColor, m_fRimPower, m_fRimWide, m_fRimForced, false );
	}
}

#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
