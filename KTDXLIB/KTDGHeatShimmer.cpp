#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER
#include "KTDGHeatShimmer.h"

extern CKTDGEffectStateManager* g_pEffectStateManager;

/// June engine remainder Phase R4a, flag SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER. Every function names the June address it is read from
/// ( unpacked_x2.exe, image base 31aa0000 ); the evidence rows are in june2017_re/funcmap.tsv, tag "Engine R4a". June quirks kept as
/// June has them are JUNE_BUGS.md's "heat shimmer" row.


/// June 31ce5630
CKTDGHeatShimmer::CKTDGHeatShimmer( LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
: m_pEffect( NULL )
, m_hTechnique( NULL )
, m_bInPass( false )
, m_uNumPasses( 0 )
, m_uPass( 0 )
, m_iSavedStackTop( -1 )
, m_bLogged( false )
{
	for( int i = 0; i < 5; ++i )
		m_ahParam[i] = NULL;
}

/// June 31ce5660
CKTDGHeatShimmer::~CKTDGHeatShimmer()
{
	SAFE_RELEASE( m_pEffect );
}


/// June 31ce5e90: only while there is no effect yet ( @31ce5ec9 ). Kept as June ( JUNE_BUGS ): once the effect exists, a later reset
/// neither calls its OnResetDevice nor returns S_OK ( E_FAIL @31ce5feb ), so after a lost device the effect stays lost.
HRESULT CKTDGHeatShimmer::OnResetDevice()
{
	if( NULL != m_pEffect )
		return E_FAIL;

	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
	Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( L"XHeatShimmerRenderer.fx" );
	if( NULL == Info->pRealData )				/// June @31ce5f93 tests the data pointer only
	{
		ErrorLogMsg( KEM_ERROR49, "XHeatShimmerRenderer.fx" );		/// June @31ce5fac
		/// not June: logged at every reset while the file is missing ( resets are rare; a line written before ErrorLog.txt is up is lost )
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SHIMMER XHeatShimmerRenderer.fx is not in any kom yet (asset-port P4a ships it): the heat shimmer stays inactive" << fileout;
		return E_FAIL;
	}

	/// June @31ce6035: flags D3DXFX_NOT_CLONEABLE | 0x20000. Kept as June ( JUNE_BUGS ): the error buffer is never released
	LPD3DXBUFFER pErr = NULL;
	HRESULT hr = D3DXCreateEffect( g_pKTDXApp->GetDevice(), Info->pRealData, Info->size, NULL, NULL, 0x20800, NULL, &m_pEffect, &pErr );
	if( FAILED( hr ) )
	{
		ErrorLog( KEM_ERROR50 );										/// June @31ce6047
		if( false == m_bLogged )
		{
			m_bLogged = true;
			KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SHIMMER XHeatShimmerRenderer.fx failed to compile: "
				<< ( ( NULL != pErr ) ? (const char*) pErr->GetBufferPointer() : "(no message)" ) << fileout;
		}
		SAFE_RELEASE( m_pEffect );
		return hr;
	}

	ASSERT( g_pEffectStateManager != NULL );
	m_pEffect->SetStateManager( g_pEffectStateManager );		/// June @31ce6088 ( vslot 0x11c, g_pKTDXApp + 0x20c )

	m_hTechnique = m_pEffect->GetTechniqueByName( "HeatShimmerTechnique" );		/// June 337280a4
	static const char* s_apszParam[5] = { "g_mViewProj", "g_mWorldViewProj", "g_fTime", "g_fPerturbationScale", "g_color" };	/// June 337280ac ( stride 8 )
	for( int i = 0; i < 5; ++i )
		m_ahParam[i] = m_pEffect->GetParameterByName( NULL, s_apszParam[i] );
	m_pEffect->SetTechnique( m_hTechnique );

	if( false == m_bLogged )
	{
		m_bLogged = true;
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] SHIMMER effect loaded: technique " << ( NULL != m_hTechnique )
			<< ", params " << ( NULL != m_ahParam[0] ) << ( NULL != m_ahParam[1] ) << ( NULL != m_ahParam[2] ) << ( NULL != m_ahParam[3] ) << ( NULL != m_ahParam[4] )
			<< " (inactive as in June: nothing begins it)" << fileout;
	}

	return m_pEffect->OnResetDevice();		/// June @31ce60f5 ( vslot 0x118 )
}

/// June 31ce5680
HRESULT CKTDGHeatShimmer::OnLostDevice()
{
	if( NULL != m_pEffect )
		return m_pEffect->OnLostDevice();
	return S_OK;
}


/// June 31ce56a0: CommitChanges, then the effect state manager's deferred shader-constant flush ( PostCommitChanges )
void CKTDGHeatShimmer::CommitChanges()
{
	if( true == m_bInPass )
	{
		m_pEffect->CommitChanges();
#ifdef X2OPTIMIZE_SETSHADERCONSTANT
		g_pEffectStateManager->PostCommitChanges();
#endif X2OPTIMIZE_SETSHADERCONSTANT
	}
}

/// June 31ce5710: one technique ( index 0 ), time and view-projection, world-view-projection only with a world matrix, nine render
/// states, the state manager, Begin( flags 7 ), BeginPass( 0 ). g_fPerturbationScale and g_color are never set ( the .fx defaults ).
HRESULT CKTDGHeatShimmer::BeginEffect( const int& iTechnique_, const D3DXMATRIX* pmatWorld_ )
{
	if( 0 != iTechnique_ || NULL == m_hTechnique )		/// June @31ce5738 / @31ce574a
		return E_FAIL;

	HRESULT hr = m_pEffect->SetTechnique( m_hTechnique );
	if( FAILED( hr ) )
		return hr;

	m_pEffect->SetFloat( m_ahParam[2], (float) g_pKTDXApp->GetTime() );		/// June @31ce576e: app +0x80 ( double ) -> float
	D3DXMATRIX matViewProj = g_pKTDXApp->GetViewProjectionTransform();			/// June @31ce579f: app +0x15c
	m_pEffect->SetMatrix( m_ahParam[0], &matViewProj );
	if( NULL != pmatWorld_ )
	{
		D3DXMATRIX matWorldViewProj;
		D3DXMatrixMultiply( &matWorldViewProj, pmatWorld_, &matViewProj );
		m_pEffect->SetMatrix( m_ahParam[1], &matWorldViewProj );
	}

	/// June @31ce57e9-31ce5a95, in this order
	const int iStackTop = CKTDGStateManager::GetStackTop();
	CKTDGStateManager::PushRenderState( D3DRS_ALPHATESTENABLE,		FALSE );
	CKTDGStateManager::PushRenderState( D3DRS_ALPHAREF,				0x88 );
	CKTDGStateManager::PushRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER );
	CKTDGStateManager::PushRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );
	CKTDGStateManager::PushRenderState( D3DRS_ZENABLE,				FALSE );
	CKTDGStateManager::PushRenderState( D3DRS_ZWRITEENABLE,			FALSE );
	CKTDGStateManager::PushRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );
	CKTDGStateManager::PushRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
	CKTDGStateManager::PushRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

	hr = g_pEffectStateManager->PrepareEffectState();			/// June @31ce5aa0 ( 31d17860 )
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
		CommitChanges();			/// June @31ce5bb5: before BeginPass with m_bInPass false, so it does nothing ( JUNE_BUGS )
		hr = m_pEffect->BeginPass( 0 );
		if( SUCCEEDED( hr ) )
		{
			m_uNumPasses = uPasses;
			m_bInPass = true;
			m_uPass = 0;
			m_iSavedStackTop = iStackTop;
			return S_OK;
		}
		m_pEffect->End();
	}

	g_pEffectStateManager->RestoreEffectState();				/// June @31ce5bff ( 31d17d20 )
	CKTDGStateManager::PopStates( iStackTop );
	return E_FAIL;
}

/// June 31ce5d00: the next pass if there is one ( returns BeginPass's result ), else End, the state manager, the saved stack, reset
HRESULT CKTDGHeatShimmer::EndEffect()
{
	if( 0 == m_uNumPasses )
		return E_FAIL;

	if( m_uPass < m_uNumPasses )
	{
		m_pEffect->EndPass();
		++m_uPass;
		m_bInPass = false;
		if( m_uPass < m_uNumPasses )
		{
			CommitChanges();		/// June @31ce5d3d: m_bInPass is false here too
			HRESULT hr = m_pEffect->BeginPass( m_uPass );
			if( SUCCEEDED( hr ) )
			{
				m_bInPass = true;
				return hr;
			}
		}
	}

	m_pEffect->End();
	g_pEffectStateManager->RestoreEffectState();
	if( m_iSavedStackTop >= 0 )
		CKTDGStateManager::PopStates( m_iSavedStackTop );

	m_bInPass = false;
	m_uNumPasses = 0;
	m_uPass = 0;
	m_iSavedStackTop = -1;
	return S_OK;
}
#endif SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER
