#include "StdAfx.h"
#include ".\ktdxdevicerendertargettexture.h"


#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
/// June 31cdc5d0: + use depth ( +0x58 ), multisample type / quality ( +0x5c / +0x60 ); the rest as March
CKTDXDeviceRenderTargetTexture::CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 pd3dDevice, wstring deviceID, int width, int height, D3DFORMAT fmt /*= D3DFMT_UNKNOWN*/,
	bool bUseDepth /*= true*/, D3DMULTISAMPLE_TYPE eMultiSampleType /*= D3DMULTISAMPLE_NONE*/, DWORD dwMultiSampleQuality /*= 0*/ )
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
CKTDXDeviceRenderTargetTexture::CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 pd3dDevice, wstring deviceID, int width, int height, D3DFORMAT fmt /*= D3DFMT_UNKNOWN*/ )
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
	: CKTDXDeviceBaseTexture( deviceID, DT_RENDER_TARGET_TEXTURE )
{
	m_DeviceType				= DT_RENDER_TARGET_TEXTURE;
	
	m_D3DFormat					= fmt;
	m_Width						= width;
	m_Height					= height;

	m_pRenderTargetTexture		= NULL;
	m_pRenderTargetSurface		= NULL;
	m_pRenderTargetZSurface		= NULL;

	m_pBackupBuffer				= NULL;
	m_pBackupZBuffer			= NULL;

	m_pDisplayVertexBuffer		= NULL;
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	m_bUseDepth					= bUseDepth;
	m_eMultiSampleType			= eMultiSampleType;
	m_dwMultiSampleQuality		= dwMultiSampleQuality;
	m_pMultiSampleSurface		= NULL;
	m_bRendering				= false;
	ZeroMemory( &m_kSurfaceDesc, sizeof(m_kSurfaceDesc) );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET


//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//	m_pVB_RHW_VERTEX				= NULL;
//	m_pVB_RENDERTARGET_RHW_VERTEX	= NULL;
//	m_pVB_RENDERTARGET_VERTEX		= NULL;
//#endif

//{{ robobeg : 2011-03-18
    m_bWithoutFile = true;
//}} robobeg : 2011-03-18

 #ifdef DXUT_KTDXLIB_SAFE_ON_RESET
     if ( DXUTGetDeviceObjectsReset() == true )
 #endif
    {
	    OnResetDevice();
    }//if

}

CKTDXDeviceRenderTargetTexture::~CKTDXDeviceRenderTargetTexture(void)
{
	OnLostDevice();
}

HRESULT CKTDXDeviceRenderTargetTexture::OnResetDevice()
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// June 31cdb950. The texture and its surface as March, but the format stays m_D3DFormat when one was given ( June never writes the
	/// back buffer's format back into +0x54 ); the multisample colour surface and the depth surface come from the app's surface manager
	/// ( +0x208 ), and a depth surface is made only for a target that asks for one.
	{
		OnLostDevice();

		LPDIRECT3DDEVICE9 pd3dDevice = g_pKTDXApp->GetDevice();
		if( FAILED( pd3dDevice->GetRenderTarget( 0, &m_pBackupBuffer ) ) )
		{
			string strName;
			ConvertWCHARToChar( strName, m_DeviceID.c_str() );
			ErrorLogMsg( KEM_ERROR76, strName.c_str());
			return E_FAIL;
		}
		if( FAILED( pd3dDevice->GetDepthStencilSurface( &m_pBackupZBuffer ) ) )
		{
			string strName;
			ConvertWCHARToChar( strName, m_DeviceID.c_str() );
			ErrorLogMsg( KEM_ERROR77, strName.c_str());
			return E_FAIL;
		}

		D3DSURFACE_DESC kBackupDesc;
		m_pBackupBuffer->GetDesc( &kBackupDesc );
		const D3DFORMAT eFormat = ( D3DFMT_UNKNOWN == m_D3DFormat ) ? kBackupDesc.Format : m_D3DFormat;

		if( FAILED( D3DXCreateTexture( pd3dDevice, m_Width, m_Height, 1, D3DUSAGE_RENDERTARGET, eFormat, D3DPOOL_DEFAULT, &m_pRenderTargetTexture ) ) )
		{
			SAFE_RELEASE( m_pRenderTargetTexture );
			string strName;
			ConvertWCHARToChar( strName, m_DeviceID.c_str() );
			ErrorLogMsg( KEM_ERROR78, strName.c_str());
			return E_FAIL;
		}
		if( FAILED( m_pRenderTargetTexture->GetSurfaceLevel( 0, &m_pRenderTargetSurface ) ) )
		{
			string strName;
			ConvertWCHARToChar( strName, m_DeviceID.c_str() );
			ErrorLogMsg( KEM_ERROR79, strName.c_str());
			return E_FAIL;
		}
		m_pRenderTargetSurface->GetDesc( &m_kSurfaceDesc );

		CKTDGDeviceSurfaceManager* pSurfaceManager = ( NULL != g_pKTDXApp ) ? g_pKTDXApp->GetJuneSurfaceManager() : NULL;
		/// June @31cdbe83: only with a multisample type, an app and its surface manager
		if( D3DMULTISAMPLE_NONE != m_eMultiSampleType && NULL != pSurfaceManager )
		{
			pSurfaceManager->GetRenderTargetSurface( &m_pMultiSampleSurface, m_kSurfaceDesc.Width, m_kSurfaceDesc.Height, m_kSurfaceDesc.Format,
				m_eMultiSampleType, m_dwMultiSampleQuality );
		}
		/// June @31cdbec3: the depth surface matches the multisample surface when there is one. June calls the manager unguarded and
		/// tests the void call's leftover EAX; the port guards the manager ( March guard ) and logs when no depth surface came back.
		if( true == m_bUseDepth && NULL != pSurfaceManager )
		{
			if( NULL != m_pMultiSampleSurface )
				pSurfaceManager->GetDepthStencilSurfaceFor( &m_pRenderTargetZSurface, m_pMultiSampleSurface );
			else
				pSurfaceManager->GetDepthStencilSurface( &m_pRenderTargetZSurface, &m_kSurfaceDesc );

			if( NULL == m_pRenderTargetZSurface )
			{
				string strName;
				ConvertWCHARToChar( strName, m_DeviceID.c_str() );
				ErrorLogMsg( KEM_ERROR80, strName.c_str());
				return E_FAIL;
			}
		}
		return S_OK;
	}
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	HRESULT hr;

	OnLostDevice();
	

//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//
//	if(FAILED(hr = g_pKTDXApp->GetDevice()->CreateVertexBuffer( 4 * sizeof(CKTDXDeviceTexture::TEXTURE_RHW_VERTEX), 
//		D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_TEXTURE_RHW_VERTEX, 
//		D3DPOOL_DEFAULT, &m_pVB_RHW_VERTEX, NULL) ))
//	{
//		ErrorLogMsg( KEM_ERROR88, L"oops 이게 뭥미" );
//		return false;
//	}
//
//	if(FAILED(hr = g_pKTDXApp->GetDevice()->CreateVertexBuffer( 4 * sizeof(RENDERTARGET_RHW_VERTEX), 
//		D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_RENDERTARGET_RHW_VERTEX, 
//		D3DPOOL_DEFAULT, &m_pVB_RENDERTARGET_RHW_VERTEX, NULL) ))
//	{
//		ErrorLogMsg( KEM_ERROR88, L"oops 이게 뭥미2" );
//		return false;
//	}
//
//	if(FAILED(hr = g_pKTDXApp->GetDevice()->CreateVertexBuffer( 4 * sizeof(RENDERTARGET_VERTEX), 
//		D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_RENDERTARGET_VERTEX, 
//		D3DPOOL_DEFAULT, &m_pVB_RENDERTARGET_VERTEX, NULL) ))
//	{
//		ErrorLogMsg( KEM_ERROR88, L"oops 이게 뭥미3" );
//		return false;
//	}
//#endif

	//Keep a handle to the back buffer for easy swapping
	if(FAILED( hr = g_pKTDXApp->GetDevice()->GetRenderTarget( 0, &m_pBackupBuffer )))
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR76, strName.c_str());

		return E_FAIL;
	}

	//Same for the depth buffer
	if(FAILED( hr = g_pKTDXApp->GetDevice()->GetDepthStencilSurface( &m_pBackupZBuffer )))
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR77, strName.c_str());

		return E_FAIL;
	}

	D3DSURFACE_DESC desc;
	m_pBackupBuffer->GetDesc( &desc );

	if( D3DFMT_UNKNOWN == m_D3DFormat )
	{
		m_D3DFormat = desc.Format;
	}
	
	if(FAILED( hr = D3DXCreateTexture(g_pKTDXApp->GetDevice(), m_Width, m_Height, 1, 
										D3DUSAGE_RENDERTARGET, m_D3DFormat,
										D3DPOOL_DEFAULT, &m_pRenderTargetTexture)))
	{
		m_pRenderTargetTexture = NULL;

		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR78, strName.c_str());
		
		return E_FAIL;
	}

	//Get the top level surface of the target texture.
	if(FAILED( hr = m_pRenderTargetTexture->GetSurfaceLevel( 0, &m_pRenderTargetSurface )))
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR79, strName.c_str());

		return E_FAIL;
	}

	m_pBackupZBuffer->GetDesc( &desc );

	if(FAILED( hr = g_pKTDXApp->GetDevice()->CreateDepthStencilSurface(m_Width, 
															m_Height,
															desc.Format,
															desc.MultiSampleType,
															desc.MultiSampleQuality,
															TRUE,
															&m_pRenderTargetZSurface,
															NULL)))
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR80, strName.c_str());

		return E_FAIL;
	}
	return S_OK;
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
}

HRESULT CKTDXDeviceRenderTargetTexture::OnLostDevice()
{
//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//	SAFE_RELEASE( m_pVB_RHW_VERTEX );
//	SAFE_RELEASE( m_pVB_RENDERTARGET_RHW_VERTEX );
//	SAFE_RELEASE( m_pVB_RENDERTARGET_VERTEX );
//#endif

#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// June 31cdb310 ( OnLostDevice 31cdb3b0 = this, then S_OK ): texture, surface, multisample surface, saved target, depth, saved depth.
	/// June's target has no vertex buffer ( March never makes one either ).
	SAFE_RELEASE( m_pRenderTargetTexture );
	SAFE_RELEASE( m_pRenderTargetSurface );
	SAFE_RELEASE( m_pMultiSampleSurface );
	SAFE_RELEASE( m_pBackupBuffer );
	SAFE_RELEASE( m_pRenderTargetZSurface );
	SAFE_RELEASE( m_pBackupZBuffer );
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	SAFE_RELEASE( m_pRenderTargetTexture );
	SAFE_RELEASE( m_pRenderTargetSurface );
	SAFE_RELEASE( m_pRenderTargetZSurface );
	SAFE_RELEASE( m_pBackupBuffer );
	SAFE_RELEASE( m_pBackupZBuffer );
	SAFE_RELEASE( m_pDisplayVertexBuffer );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET

	return S_OK;
}

void CKTDXDeviceRenderTargetTexture::BeginRender( bool bClear )
{
	KTDXPROFILE();
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// March's callers ( the env box LUT, the minimap, the UI move data, the avatar shot ... ) reach June's body; each June caller
	/// passes its own four arguments, March's one maps to ( clear, clear, own depth, transparent black )
	BeginRender( bClear, bClear, false, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	//Set up the renderTarget
	HRESULT hr;
	hr = g_pKTDXApp->GetDevice()->SetRenderTarget( 0, m_pRenderTargetSurface );
	hr = g_pKTDXApp->GetDevice()->SetDepthStencilSurface( m_pRenderTargetZSurface );

	if( bClear == true )
	{
		g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	}
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
}

void CKTDXDeviceRenderTargetTexture::EndRender()
{
	KTDXPROFILE();
#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// June 31cdb550: restore what BeginRender saved; a multisample surface is resolved into the texture's surface ( D3DTEXF_NONE ) and
	/// the texture gets AddDirtyRect( NULL ) ( vtable 0x54; R4 read it as GenerateMipSubLevels )
	{
		LPDIRECT3DDEVICE9 pd3dDevice = g_pKTDXApp->GetDevice();
		m_bRendering = false;
		pd3dDevice->SetRenderTarget( 0, m_pBackupBuffer );
		pd3dDevice->SetDepthStencilSurface( m_pBackupZBuffer );
		if( NULL != m_pMultiSampleSurface )
		{
			pd3dDevice->StretchRect( m_pMultiSampleSurface, NULL, m_pRenderTargetSurface, NULL, D3DTEXF_NONE );
			m_pRenderTargetTexture->AddDirtyRect( NULL );
		}
	}
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	//Set the renderTarget back to the normal back buffer
	HRESULT hr;
	hr = g_pKTDXApp->GetDevice()->SetRenderTarget( 0, m_pBackupBuffer );
	hr = g_pKTDXApp->GetDevice()->SetDepthStencilSurface( m_pBackupZBuffer );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
/// June 31cdb3c0 ( ret 0x10 )
HRESULT CKTDXDeviceRenderTargetTexture::BeginRender( bool bClearTarget, bool bClearDepth, bool bKeepDepth, D3DCOLOR color )
{
	KTDXPROFILE();
	LPDIRECT3DDEVICE9 pd3dDevice = g_pKTDXApp->GetDevice();

	SAFE_RELEASE( m_pBackupBuffer );
	if( FAILED( pd3dDevice->GetRenderTarget( 0, &m_pBackupBuffer ) ) )
		return E_FAIL;
	SAFE_RELEASE( m_pBackupZBuffer );
	if( FAILED( pd3dDevice->GetDepthStencilSurface( &m_pBackupZBuffer ) ) )
		return E_FAIL;

	D3DSURFACE_DESC kDepthDesc;
	m_pBackupZBuffer->GetDesc( &kDepthDesc );
	if( NULL == m_pRenderTargetSurface )
		return E_FAIL;

	pd3dDevice->SetRenderTarget( 0, ( NULL != m_pMultiSampleSurface ) ? m_pMultiSampleSurface : m_pRenderTargetSurface );
	if( false == bKeepDepth && NULL != m_pRenderTargetZSurface )
	{
		pd3dDevice->SetDepthStencilSurface( m_pRenderTargetZSurface );
		m_pRenderTargetZSurface->GetDesc( &kDepthDesc );
	}

	DWORD dwFlags = ( true == bClearTarget ) ? D3DCLEAR_TARGET : 0;
	if( true == bClearDepth )
	{
		dwFlags |= D3DCLEAR_ZBUFFER;
		switch( kDepthDesc.Format )		/// June jump table 31cdb538 / 31cdb540
		{
		case D3DFMT_D15S1:
		case D3DFMT_D24S8:
		case D3DFMT_D24X4S4:
		case D3DFMT_D24FS8:
			dwFlags |= D3DCLEAR_STENCIL;
			break;
		}
	}
	if( 0 != dwFlags )
		pd3dDevice->Clear( 0, NULL, dwFlags, color, 1.0f, 0 );

	m_bRendering = true;
	return S_OK;
}

/// June 31cdb5c0: the state manager's back buffer ( 3376e504 ) is the destination, filter linear when this target's surface size is
/// not the back buffer's ( 3376d9c4 / 3376d9c8 ), none otherwise; the multisample surface is the source when there is one
void CKTDXDeviceRenderTargetTexture::EndRenderToBackBuffer()
{
	KTDXPROFILE();
	LPDIRECT3DDEVICE9 pd3dDevice = g_pKTDXApp->GetDevice();
	m_bRendering = false;
	pd3dDevice->SetRenderTarget( 0, m_pBackupBuffer );
	pd3dDevice->SetDepthStencilSurface( m_pBackupZBuffer );

	LPDIRECT3DSURFACE9 pBackBuffer = CKTDGStateManager::GetBackBuffer();
	if( NULL == pBackBuffer )
		return;
	const D3DSURFACE_DESC& kBackBufferDesc = CKTDGStateManager::GetBackBufferDesc();
	const D3DTEXTUREFILTERTYPE eFilter = ( m_kSurfaceDesc.Width != kBackBufferDesc.Width || m_kSurfaceDesc.Height != kBackBufferDesc.Height )
		? D3DTEXF_LINEAR : D3DTEXF_NONE;
	pd3dDevice->StretchRect( ( NULL != m_pMultiSampleSurface ) ? m_pMultiSampleSurface : m_pRenderTargetSurface, NULL, pBackBuffer, NULL, eFilter );
}
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET

void CKTDXDeviceRenderTargetTexture::Clear( D3DCOLOR color )
{
	KTDXPROFILE();

	g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
}

void CKTDXDeviceRenderTargetTexture::Draw( int nX, int nY, int nWidth, int nHeight, D3DCOLOR color /* = 0xffffffff  */)
{
	KTDXPROFILE();

    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() )

	D3DXVECTOR3 pos		= g_pKTDXApp->ConvertByResolution( (float)nX, (float)nY );
	D3DXVECTOR3 size	= g_pKTDXApp->ConvertByResolution( (float)nWidth, (float)nHeight );



	float X1 = (float)pos.x - 0.5f;
	float Y1 = (float)pos.y - 0.5f;
	float X2 = (float)(pos.x + size.x);
	float Y2 = (float)(pos.y + size.y);

	float UMax = 1.0f;
	float VMax = 1.0f;

//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2
	CKTDXDeviceTexture::TEXTURE_RHW_VERTEX vertex[4];
	ZeroMemory( vertex, sizeof(CKTDXDeviceTexture::TEXTURE_RHW_VERTEX) * 4 );

	//Set up the 4 corners of a small square
	vertex[0].x = X1;     vertex[0].y = Y1;
	vertex[0].z = 1.0f;   vertex[0].rhw = 1.0f;
	vertex[0].u = 0.0f;   vertex[0].v = 0.0f;

	vertex[1].x = X1;     vertex[1].y = Y2;
	vertex[1].u = 0.0f;   vertex[1].v = VMax;
	vertex[1].z = 1.0f;   vertex[1].rhw = 1.0f;

	vertex[2].x = X2;     vertex[2].y = Y1;
	vertex[2].u = UMax;   vertex[2].v = 0.0f;
	vertex[2].z = 1.0f;   vertex[2].rhw = 1.0f;

	vertex[3].x = X2;     vertex[3].y = Y2;
	vertex[3].u = UMax;   vertex[3].v = VMax;
	vertex[3].z = 1.0f;   vertex[3].rhw = 1.0f;

	vertex[0].color = vertex[1].color = 
	vertex[2].color = vertex[3].color = color;
//#endif

	SetDeviceTexture();

//#endif //REMOVE_DRAW_PRIMITIVE_UP_2

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	BOOST_STATIC_ASSERT( D3DFVF_TEXTURE_RHW_VERTEX == D3DFVF_XYZRHW_DIFFUSE_TEX1 );
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLESTRIP, 2, vertex );
//#else
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_TEXTURE_RHW_VERTEX );
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(CKTDXDeviceTexture::TEXTURE_RHW_VERTEX) );
//#endif DYNAMIC_VERTEX_BUFFER_OPT
	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::RenderQuadRHW( D3DCOLOR color, float fX, float fY, float fWidth, float fHeight )
{
	KTDXPROFILE();
    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() );
	

	D3DVIEWPORT9 Viewport;
	g_pKTDXApp->GetDevice()->GetViewport(&Viewport);

	float X1, Y1, X2, Y2;
	if( fWidth == -1.f || fHeight == -1.f )
	{
		X1 = (float)Viewport.X;
		Y1 = (float)Viewport.Y;
		X2 = (float)Viewport.X + Viewport.Width;
		Y2 = (float)Viewport.Y + Viewport.Height;
	}
	else
	{
		X1 = fX;
		Y1 = fY;
		X2 = X1 + fWidth;
		Y2 = Y1 + fHeight;
	}

	float UMax = 1.0f;
	float VMax = 1.0f;

//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2
	RENDERTARGET_RHW_VERTEX vertex[4];
	ZeroMemory( vertex, sizeof(RENDERTARGET_RHW_VERTEX) * 4 );

	//Set up the 4 corners of a small square
	vertex[0].x = X1;     vertex[0].y = Y1;
	vertex[0].z = 1.0f;   vertex[0].rhw = 1.0f;
	vertex[0].u = 0.0f;   vertex[0].v = 0.0f;

	vertex[2].x = X1;     vertex[2].y = Y2;
	vertex[2].z = 1.0f;   vertex[2].rhw = 1.0f;
	vertex[2].u = 0.0f;   vertex[2].v = VMax;

	vertex[1].x = X2;     vertex[1].y = Y1;
	vertex[1].z = 1.0f;   vertex[1].rhw = 1.0f;
	vertex[1].u = UMax;   vertex[1].v = 0.0f;

	vertex[3].x = X2;     vertex[3].y = Y2;
	vertex[3].z = 1.0f;   vertex[3].rhw = 1.0f;
	vertex[3].u = UMax;   vertex[3].v = VMax;

	vertex[0].color = vertex[1].color = 
		vertex[2].color = vertex[3].color = color;

//#endif

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	BOOST_STATIC_ASSERT( D3DFVF_RENDERTARGET_RHW_VERTEX == D3DFVF_XYZRHW_DIFFUSE_TEX1 );
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLESTRIP, 2, vertex );
//#else   DYNAMIC_VERTEX_BUFFER_OPT
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_RENDERTARGET_RHW_VERTEX );
//	
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(RENDERTARGET_RHW_VERTEX) );
//#endif  DYNAMIC_VERTEX_BUFFER_OPT

	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::RenderQuad( D3DCOLOR color )
{
	KTDXPROFILE();
    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() )


	//HRESULT hr;
	
//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2 

	RENDERTARGET_VERTEX vertex[6];
	//ZeroMemory( vertex, sizeof(RENDERTARGET_VERTEX) * 6 );

	vertex[0].vPos = D3DXVECTOR3(-1,1,0);
	vertex[0].vTex = D3DXVECTOR2(0,0);

	vertex[1].vPos = D3DXVECTOR3(1,1,0);
	vertex[1].vTex = D3DXVECTOR2(1,0);

	vertex[2].vPos = D3DXVECTOR3(-1,-1,0);
	vertex[2].vTex = D3DXVECTOR2(0,1);

	vertex[3].vPos = D3DXVECTOR3(1,-1,0);
	vertex[3].vTex = D3DXVECTOR2(1,1);

	vertex[4].vPos = D3DXVECTOR3(-1,-1,0);
	vertex[4].vTex = D3DXVECTOR2(0,1);

	vertex[5].vPos = D3DXVECTOR3(1,1,0);
	vertex[5].vTex = D3DXVECTOR2(1,0);

	/*
	vertex[0].color = vertex[1].color = 
		vertex[2].color = vertex[3].color = 
			vertex[4].color = vertex[5].color = color;
			*/
//#endif //REMOVE_DRAW_PRIMITIVE_UP_2

//#ifdef  DYNAMIC_VERTEX_BUFFER_OPT
	BOOST_STATIC_ASSERT( D3DFVF_RENDERTARGET_VERTEX == D3DFVF_XYZ_TEX1 );
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLELIST, 2, vertex );
//#else   DYNAMIC_VERTEX_BUFFER_OPT
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_RENDERTARGET_VERTEX );	
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, (void*)vertex, sizeof(RENDERTARGET_VERTEX) );
//
//#endif  DYNAMIC_VERTEX_BUFFER_OPT
	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::SetDeviceTexture( int stage )
{
//{{ robobeg : 2008-10-22
	//if( g_pKTDXApp->IsSameBeforeTexture( stage, m_pRenderTargetTexture ) == false )
	//{
	//	if ( FAILED(g_pKTDXApp->GetDevice()->SetTexture( stage, m_pRenderTargetTexture ) ) )
	//	{
	//		string strName;
	//		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
	//		ErrorLogMsg( KEM_ERROR81, strName.c_str() );
	//		return;
	//	}
	//	g_pKTDXApp->SetBeforeTexture( stage, m_pRenderTargetTexture );
	//}
    if ( FAILED( CKTDGStateManager::SetTexture( stage, m_pRenderTargetTexture ) ) )
    {
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR81, strName.c_str() );
    }//if
//}} robobeg : 2008-10-22
}

HRESULT CKTDXDeviceRenderTargetTexture::CopySurface( LPDIRECT3DSURFACE9 pDestSurface )
{
	KTDXPROFILE();

	HRESULT hr;

	hr = g_pKTDXApp->GetDevice()->StretchRect( m_pRenderTargetSurface, NULL, pDestSurface, NULL, D3DTEXF_LINEAR );
	if ( hr != S_OK )
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR82, strName.c_str() );
	}
	return hr;
}

HRESULT CKTDXDeviceRenderTargetTexture::CopySurfaceToBackBuffer()
{
	KTDXPROFILE();

	HRESULT hr;

#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// Not in June. June's target saves the bound target at BeginRender, not at reset, so the back buffer March meant here is the
	/// state manager's ( the surface March's reset saved )
	hr = g_pKTDXApp->GetDevice()->StretchRect( m_pRenderTargetSurface, NULL, CKTDGStateManager::GetBackBuffer(), NULL, D3DTEXF_LINEAR );
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	hr = g_pKTDXApp->GetDevice()->StretchRect( m_pRenderTargetSurface, NULL, m_pBackupBuffer, NULL, D3DTEXF_LINEAR );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET

	if ( hr != S_OK )
	{
		switch( hr )
		{
		case D3DERR_INVALIDCALL:
			{
				wstring errorMSG;
				errorMSG = L"D3DERR_INVALIDCALL ";
				errorMSG += m_DeviceID;
				ErrorLogMsg( KEM_ERROR83, errorMSG.c_str() );
			}
			break;
		}
	}
	return hr;
}

HRESULT CKTDXDeviceRenderTargetTexture::CopyBackBufferToSurface()
{
	KTDXPROFILE();

	HRESULT hr;

	if( g_pKTDXApp->GetCannotStretchRect() == true )
	{
		return E_FAIL;
	}

#ifdef SERV_IRUHADEV_JUNE_RENDER_TARGET
	/// Not in June ( see CopySurfaceToBackBuffer )
	hr = g_pKTDXApp->GetDevice()->StretchRect( CKTDGStateManager::GetBackBuffer(), NULL, m_pRenderTargetSurface, NULL, D3DTEXF_LINEAR );
#else SERV_IRUHADEV_JUNE_RENDER_TARGET
	hr = g_pKTDXApp->GetDevice()->StretchRect( m_pBackupBuffer, NULL, m_pRenderTargetSurface, NULL, D3DTEXF_LINEAR );
#endif SERV_IRUHADEV_JUNE_RENDER_TARGET

	if ( hr != S_OK )
	{
		switch( hr )
		{
		case D3DERR_INVALIDCALL:
			{
				wstring errorMSG;
				errorMSG = L"D3DERR_INVALIDCALL ";
				errorMSG += m_DeviceID;
				ErrorLogMsg( KEM_ERROR84, errorMSG.c_str() );
			}
			break;
		}
	}
	return hr;
}
