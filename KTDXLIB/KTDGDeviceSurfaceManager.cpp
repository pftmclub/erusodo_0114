#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
#include "KTDGDeviceSurfaceManager.h"

extern LPDIRECT3DDEVICE9	g_pd3dDevice;

/// June engine remainder Phase R4b, flag SERV_IRUHADEV_JUNE_RENDER_TARGET. Every function names the June address it is read from
/// ( unpacked_x2.exe, image base 31aa0000 ); the evidence rows are in june2017_re/funcmap.tsv, tag "Engine R4b".
/// The state-manager calls are June's 31c5a060 ( CheckDeviceFormat ), 31c5a0a0 ( CheckDepthStencilMatch ) and 31c5a0d0
/// ( CheckDeviceMultiSampleType ), whose arguments are March's CKTDGStateManager helpers.


/// June 31d01070: the stage base ctor 31d16d90, an empty list
CKTDGDeviceSurfaceManager::CKTDGDeviceSurfaceManager()
{
}

/// June 31d01130: Clear() ( 31d011c0, twice: once for the members' unwind ), free the list head, the stage base dtor 31d28560
CKTDGDeviceSurfaceManager::~CKTDGDeviceSurfaceManager()
{
	Clear();
}

/// June 31d011b0 ( vslot 4, ret 0xc )
HRESULT CKTDGDeviceSurfaceManager::OnFrameMove( double /*fTime*/, float /*fElapsedTime*/ )
{
	Purge();
	return S_OK;
}

/// June 31d00fd0 ( vslot 0x20 )
HRESULT CKTDGDeviceSurfaceManager::OnResetDevice()
{
	Clear();
	return S_OK;
}

/// June 31d00fe0 ( vslot 0x24 )
HRESULT CKTDGDeviceSurfaceManager::OnLostDevice()
{
	Clear();
	return S_OK;
}

/// June 31d00b30 ( ret 0x18 )
void CKTDGDeviceSurfaceManager::GetRenderTargetSurface( LPDIRECT3DSURFACE9* ppOut_, UINT uWidth_, UINT uHeight_, D3DFORMAT eFormat_,
													  D3DMULTISAMPLE_TYPE eMultiSample_, DWORD dwQuality_ )
{
	SAFE_RELEASE( *ppOut_ );
	if( NULL == g_pd3dDevice )
		return;

	if( D3DFMT_UNKNOWN == eFormat_ )
		eFormat_ = CKTDGStateManager::GetBackBufferDesc().Format;
	if( D3DMULTISAMPLE_NONE == eMultiSample_ )
		dwQuality_ = 0;

	for( std::list<KSurface>::iterator it = m_listSurface.begin(); it != m_listSurface.end(); ++it )
	{
		const KSurface& kSurface = *it;
		if( NULL != kSurface.m_pSurface
			&& kSurface.m_kDesc.Format == eFormat_
			&& kSurface.m_kDesc.Usage == D3DUSAGE_RENDERTARGET
			&& kSurface.m_kDesc.MultiSampleType == eMultiSample_
			&& kSurface.m_kDesc.MultiSampleQuality == dwQuality_
			&& kSurface.m_kDesc.Width == uWidth_
			&& kSurface.m_kDesc.Height == uHeight_ )
		{
			*ppOut_ = kSurface.m_pSurface;
			(*ppOut_)->AddRef();
			return;
		}
	}

	if( FAILED( CKTDGStateManager::CheckDeviceFormat( D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, eFormat_ ) ) )
		return;
	if( D3DMULTISAMPLE_NONE != eMultiSample_ )
	{
		DWORD dwQualityLevels = 0;
		if( FAILED( CKTDGStateManager::CheckDeviceMultiSampleType( eFormat_, eMultiSample_, &dwQualityLevels ) )
			|| dwQualityLevels <= dwQuality_ )
			return;
	}

	LPDIRECT3DSURFACE9 pSurface = NULL;
	if( SUCCEEDED( g_pd3dDevice->CreateRenderTarget( uWidth_, uHeight_, eFormat_, eMultiSample_, dwQuality_, FALSE, &pSurface, NULL ) )
		&& NULL != pSurface )
	{
		KSurface kSurface;
		if( SUCCEEDED( pSurface->GetDesc( &kSurface.m_kDesc ) ) )
		{
			*ppOut_ = pSurface;
			(*ppOut_)->AddRef();
			kSurface.m_pSurface = pSurface;		/// the cache keeps CreateRenderTarget's reference ( June 31d00a60 moves it in )
			m_listSurface.push_back( kSurface );
			return;
		}
	}
	SAFE_RELEASE( pSurface );
}

/// June 31d00d20 ( ret 8 )
void CKTDGDeviceSurfaceManager::GetDepthStencilSurface( LPDIRECT3DSURFACE9* ppOut_, const D3DSURFACE_DESC* pRenderTargetDesc_ )
{
	SAFE_RELEASE( *ppOut_ );
	if( NULL == g_pd3dDevice || D3DUSAGE_RENDERTARGET != pRenderTargetDesc_->Usage )
		return;

	for( std::list<KSurface>::iterator it = m_listSurface.begin(); it != m_listSurface.end(); ++it )
	{
		const KSurface& kSurface = *it;
		if( NULL != kSurface.m_pSurface
			&& kSurface.m_kDesc.Usage == D3DUSAGE_DEPTHSTENCIL
			&& kSurface.m_kDesc.MultiSampleType == pRenderTargetDesc_->MultiSampleType
			&& kSurface.m_kDesc.MultiSampleQuality == pRenderTargetDesc_->MultiSampleQuality
			&& pRenderTargetDesc_->Width <= kSurface.m_kDesc.Width
			&& pRenderTargetDesc_->Height <= kSurface.m_kDesc.Height
			&& SUCCEEDED( CKTDGStateManager::CheckDepthStencilMatch( pRenderTargetDesc_->Format, kSurface.m_kDesc.Format ) ) )
		{
			*ppOut_ = kSurface.m_pSurface;
			(*ppOut_)->AddRef();
			return;
		}
	}

	/// June @31d00dc5: the back buffer's depth-stencil format ( state manager 3376d98c )
	const D3DFORMAT eDepthFormat = CKTDGStateManager::GetBackBufferZStencilDesc().Format;
	if( FAILED( CKTDGStateManager::CheckDepthStencilMatch( pRenderTargetDesc_->Format, eDepthFormat ) ) )
		return;
	if( D3DMULTISAMPLE_NONE != pRenderTargetDesc_->MultiSampleType )
	{
		DWORD dwQualityLevels = 0;
		if( FAILED( CKTDGStateManager::CheckDeviceMultiSampleType( eDepthFormat, pRenderTargetDesc_->MultiSampleType, &dwQualityLevels ) )
			|| dwQualityLevels <= pRenderTargetDesc_->MultiSampleQuality )
			return;
	}

	LPDIRECT3DSURFACE9 pSurface = NULL;
	if( SUCCEEDED( g_pd3dDevice->CreateDepthStencilSurface( pRenderTargetDesc_->Width, pRenderTargetDesc_->Height, eDepthFormat,
		pRenderTargetDesc_->MultiSampleType, pRenderTargetDesc_->MultiSampleQuality, TRUE, &pSurface, NULL ) )
		&& NULL != pSurface )
	{
		KSurface kSurface;
		if( SUCCEEDED( pSurface->GetDesc( &kSurface.m_kDesc ) ) )
		{
			*ppOut_ = pSurface;
			(*ppOut_)->AddRef();
			kSurface.m_pSurface = pSurface;
			m_listSurface.push_back( kSurface );
			return;
		}
	}
	SAFE_RELEASE( pSurface );
}

/// June 31d00f20 ( ret 8 )
void CKTDGDeviceSurfaceManager::GetDepthStencilSurfaceFor( LPDIRECT3DSURFACE9* ppOut_, LPDIRECT3DSURFACE9 pRenderTarget_ )
{
	SAFE_RELEASE( *ppOut_ );
	if( NULL == g_pd3dDevice || NULL == pRenderTarget_ )
		return;

	D3DSURFACE_DESC kDesc;
	if( SUCCEEDED( pRenderTarget_->GetDesc( &kDesc ) ) && D3DUSAGE_RENDERTARGET == kDesc.Usage )
		GetDepthStencilSurface( ppOut_, &kDesc );
}

/// June 31d00ff0: Release() tells whether anyone else holds the surface; if so the reference is taken back ( AddRef )
void CKTDGDeviceSurfaceManager::Purge()
{
	std::list<KSurface>::iterator it = m_listSurface.begin();
	while( it != m_listSurface.end() )
	{
		if( NULL != it->m_pSurface )
		{
			if( 0 != it->m_pSurface->Release() )
			{
				it->m_pSurface->AddRef();
				++it;
				continue;
			}
			it->m_pSurface = NULL;
		}
		it = m_listSurface.erase( it );
	}
}

/// June 31d011c0
void CKTDGDeviceSurfaceManager::Clear()
{
	for( std::list<KSurface>::iterator it = m_listSurface.begin(); it != m_listSurface.end(); ++it )
	{
		SAFE_RELEASE( it->m_pSurface );
	}
	m_listSurface.clear();
}
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
