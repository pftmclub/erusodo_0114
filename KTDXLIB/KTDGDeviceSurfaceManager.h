#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
#pragma once

/// June engine remainder Phase R4b: CKTDGDeviceSurfaceManager ( RTTI .?AVCKTDGDeviceSurfaceManager@@, vtable 3344d73c, 0x40 bytes ).
/// A CKTDXStage that CKTDXApp makes at +0x208 ( ctor 31d01070, made @31c19711 ) and inserts into the stage chain before the device
/// manager. It caches the multisample colour surfaces and the depth-stencil surfaces June's render targets use ( CKTDXDeviceRenderTargetTexture
/// OnResetDevice 31cdb950 ), so two targets with the same description share one surface. Every frame ( OnFrameMove ) it frees the surfaces
/// no target holds any more; a device reset or loss empties it.
class CKTDGDeviceSurfaceManager : public CKTDXStage
{
	public:
		CKTDGDeviceSurfaceManager();											/// June 31d01070
		virtual ~CKTDGDeviceSurfaceManager();									/// June 31d01130 ( deleting dtor 31d01270 )

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );		/// June 31d011b0: Purge()
		virtual HRESULT OnResetDevice();										/// June 31d00fd0: Clear()
		virtual HRESULT OnLostDevice();											/// June 31d00fe0: Clear()

		/// June 31d00b30: a render-target surface of this size / format / multisample, shared when one is cached
		void	GetRenderTargetSurface( LPDIRECT3DSURFACE9* ppOut_, UINT uWidth_, UINT uHeight_, D3DFORMAT eFormat_,
									D3DMULTISAMPLE_TYPE eMultiSample_, DWORD dwQuality_ );
		/// June 31d00d20: a depth-stencil surface at least as big as the render target described, with its multisample
		void	GetDepthStencilSurface( LPDIRECT3DSURFACE9* ppOut_, const D3DSURFACE_DESC* pRenderTargetDesc_ );
		/// June 31d00f20: the same, for a render-target surface
		void	GetDepthStencilSurfaceFor( LPDIRECT3DSURFACE9* ppOut_, LPDIRECT3DSURFACE9 pRenderTarget_ );
		/// June 31d00ff0: drop the entries whose surface only the cache still holds
		void	Purge();
		/// June 31d011c0: release every surface and empty the cache
		void	Clear();

	private:
		struct KSurface
		{
			D3DSURFACE_DESC		m_kDesc;		/// [+0x08] of the list node
			LPDIRECT3DSURFACE9	m_pSurface;		/// [+0x28]
		};
		std::list<KSurface>		m_listSurface;	/// [+0x34] head, [+0x38] size
};
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
