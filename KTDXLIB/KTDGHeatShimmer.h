#ifdef SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER
#pragma once

/// June engine remainder Phase R4a: the heat-shimmer object CKTDGManager holds at +0x384 ( 0x2c bytes, no vtable; allocated by the
/// manager ctor 31cead50 @31ceb0c2, deleted by its dtor @31ceb37c, reset / lost by the manager's 31ce98c0 / 31ce97e0 ).
/// XHeatShimmerRenderer.fx draws the frame's pass-1 objects as a perturbation of the scene render target. June never turns the
/// effect on ( option +0x34 has no setter caller, R4 RE item 1 ), so in June nothing calls BeginEffect; the object is made, reset
/// and lost exactly as June does it, and the frame render that would use it is R4b's.
class CKTDGHeatShimmer
{
	public:
		CKTDGHeatShimmer( LPDIRECT3DDEVICE9 pd3dDevice );		/// June 31ce5630 ( ret 4: the device argument is not used )
		~CKTDGHeatShimmer();									/// June 31ce5660

		HRESULT	OnResetDevice();								/// June 31ce5e90
		HRESULT	OnLostDevice();									/// June 31ce5680
		HRESULT	BeginEffect( const int& iTechnique_, const D3DXMATRIX* pmatWorld_ );	/// June 31ce5710 ( ret 8 )
		HRESULT	EndEffect();									/// June 31ce5d00
		void	CommitChanges();								/// June 31ce56a0

		bool	IsEffectLoaded() const { return NULL != m_pEffect; }

	private:
		LPD3DXEFFECT	m_pEffect;			/// [+0x00]
		D3DXHANDLE		m_hTechnique;		/// [+0x04] HeatShimmerTechnique
		D3DXHANDLE		m_ahParam[5];		/// [+0x08] g_mViewProj, g_mWorldViewProj, g_fTime, g_fPerturbationScale, g_color ( 337280ac )
		bool			m_bInPass;			/// [+0x1c]
		UINT			m_uNumPasses;		/// [+0x20]
		UINT			m_uPass;			/// [+0x24]
		int				m_iSavedStackTop;	/// [+0x28]
		bool			m_bLogged;			/// not June: the load / compile result is logged once ( [JUNE_ENGINE] SHIMMER ); a missing file logs at every reset
};
#endif SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER
