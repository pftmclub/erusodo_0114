#pragma once

#define D3DFVF_RENDERTARGET_RHW_VERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
#define D3DFVF_RENDERTARGET_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

class CKTDXDeviceRenderTargetTexture 
	: public CKTDXDeviceBaseTexture
{
	private:
		struct RENDERTARGET_RHW_VERTEX
		{
			float x, y, z, rhw;
			DWORD color;
			float u, v;
		};

		struct RENDERTARGET_VERTEX
		{
			D3DXVECTOR3	vPos;
			//DWORD		color;
			D3DXVECTOR2 vTex;
		};

	public:
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
		/// June 31cdc5d0 ( vtable 3344c860, 0xa0 bytes ): + use-depth, multisample type and quality. March callers keep the defaults
		/// ( a depth surface, no multisample ), which is what March's target always made.
		CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 pd3dDevice, wstring deviceID, int width, int height, D3DFORMAT fmt = D3DFMT_UNKNOWN,
			bool bUseDepth = true, D3DMULTISAMPLE_TYPE eMultiSampleType = D3DMULTISAMPLE_NONE, DWORD dwMultiSampleQuality = 0 );
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
		CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 pd3dDevice, wstring deviceID, int width, int height, D3DFORMAT fmt = D3DFMT_UNKNOWN );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET


		virtual HRESULT OnResetDevice();
		virtual HRESULT OnLostDevice();

#ifdef SERV_IRUHADEV_JUNE_RENDER_DRAW_PASS
		// June 31cdadc0: the last argument is the render pass ( 1 = DG +0x40, the additive block )
		void Draw( int nX, int nY, int nWidth, int nHeight, D3DCOLOR color = 0xffffffff, bool bPostEffectPass_ = false );
#else SERV_IRUHADEV_JUNE_RENDER_DRAW_PASS
		void Draw( int nX, int nY, int nWidth, int nHeight, D3DCOLOR color = 0xffffffff );
#endif SERV_IRUHADEV_JUNE_RENDER_DRAW_PASS
		void RenderQuadRHW( D3DCOLOR color = 0xffffffff, float fX = 0.f, float fY = 0.f, float fWidth = -1.f, float fHeight = -1.f );
		void RenderQuad( D3DCOLOR color = 0xffffffff );

		void BeginRender( bool bClear = true );
		void EndRender();
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
		/// June 31cdb3c0 ( ret 0x10 ): saves the bound target / depth, binds the multisample surface ( or the texture's ), binds this target's
		/// depth unless bKeepDepth, then clears target / depth ( + stencil on a stencil format ) with colour. March's BeginRender( bClear ) is
		/// this with ( bClear, bClear, false, 0 ).
		HRESULT BeginRender( bool bClearTarget, bool bClearDepth, bool bKeepDepth, D3DCOLOR color );
		/// June 31cdb5c0: EndRender's restore, then the result is stretched onto the back buffer ( linear when the sizes differ )
		void EndRenderToBackBuffer();

		D3DFORMAT			GetFormat() const { return m_D3DFormat; }
		D3DMULTISAMPLE_TYPE	GetMultiSampleType() const { return m_eMultiSampleType; }
		DWORD				GetMultiSampleQuality() const { return m_dwMultiSampleQuality; }
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET

		void Clear( D3DCOLOR color = 0x00000000 );

		void SetDeviceTexture( int stage = 0 );
		HRESULT CopySurface( LPDIRECT3DSURFACE9 pDestSurface );
		HRESULT CopySurfaceToBackBuffer();
		HRESULT CopyBackBufferToSurface();

		LPDIRECT3DSURFACE9 GetSurface(){ return m_pRenderTargetSurface; }
		LPDIRECT3DTEXTURE9 GetTexture(){ return m_pRenderTargetTexture; }
		int GetWidth(){ return m_Width; }
		int GetHeight(){ return m_Height; }


		bool IsOk()
		{
			if( m_pRenderTargetTexture == NULL || m_pRenderTargetSurface == NULL )
			{
				return false;
			}
			else
			{
				return true;
			}
		}

    protected:

		virtual ~CKTDXDeviceRenderTargetTexture(void);

	private:



		int						m_Width;
		int						m_Height;

		LPDIRECT3DTEXTURE9		m_pRenderTargetTexture;

		LPDIRECT3DSURFACE9		m_pRenderTargetSurface;
		LPDIRECT3DSURFACE9		m_pRenderTargetZSurface;
		
		LPDIRECT3DSURFACE9		m_pBackupBuffer;
		LPDIRECT3DSURFACE9		m_pBackupZBuffer;
		
		LPDIRECT3DVERTEXBUFFER9	m_pDisplayVertexBuffer;
		D3DFORMAT				m_D3DFormat;
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
		/// June layout: +0x4c width, +0x50 height, +0x54 format, +0x58 use depth, +0x5c / +0x60 multisample, +0x64 texture, +0x68 surface,
		/// +0x6c depth, +0x70 multisample surface, +0x74 / +0x78 the target / depth saved by BeginRender, +0x7c rendering, +0x80 surface desc
		bool					m_bUseDepth;			/// [+0x58]
		D3DMULTISAMPLE_TYPE		m_eMultiSampleType;		/// [+0x5c]
		DWORD					m_dwMultiSampleQuality;	/// [+0x60]
		LPDIRECT3DSURFACE9		m_pMultiSampleSurface;	/// [+0x70] from the surface manager; resolved into the texture by EndRender
		bool					m_bRendering;			/// [+0x7c]
		D3DSURFACE_DESC			m_kSurfaceDesc;			/// [+0x80] the texture surface's desc
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
		
//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//		LPDIRECT3DVERTEXBUFFER9	m_pVB_RHW_VERTEX;
//		LPDIRECT3DVERTEXBUFFER9	m_pVB_RENDERTARGET_RHW_VERTEX;
//		LPDIRECT3DVERTEXBUFFER9	m_pVB_RENDERTARGET_VERTEX;
//#endif
};
