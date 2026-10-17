#ifdef SERV_IRUHADEV_JUNE_RENDER_ENVBOX
#pragma once

/// June engine remainder Phase R3: June's CKTDGEnvBoxMap ( vtable 3344da84, ctor 31d09b30, object size 0x248 ), ported whole.
/// A world script makes one with g_pWorld:SetEnvBoxMap( "File.lua" ); the script then calls g_pEnvBoxMap:SetLUT( "Lut.dds" ) and
/// g_pEnvBoxMap:AddEnvBox_LUA{ ... } to describe oriented boxes. Each frame the world asks the map to blend the colour-grading
/// LUTs and the rim-light values of the boxes the camera unit stands in (Render, 31d0a410) and hands the result to the renderer
/// ( CKTDGXRenderer +0x59 .. +0x7c ). No June script creates a map (R0 census: reach 0), so in game it stays inactive, as in June.
class CKTDGEnvBoxMap : public CKTDGObject
{
	public:
		/// June 0x20-byte LUT entry ( SetLUT, 31d0a0e0 ): the texture name and the opened texture ( +0x1c )
		struct LutEntry
		{
			wstring				m_wstrName;
			CKTDXDeviceTexture*	m_pTexture;

			LutEntry() : m_pTexture( NULL ) {}
		};

		/// June 0xb0-byte box ( entry ctor 31d0d100, AddEnvBox_LUA 31d0b720 ); the June offset is in brackets
		struct EnvBox
		{
			int				m_iId;				/// [+0x00] index at creation ( the sort tie-break )
			wstring			m_wstrName;			/// [+0x04] BOX_NAME
			D3DXVECTOR3		m_vPos;				/// [+0x20] BOX_POSITION
			D3DXVECTOR3		m_vRot;				/// [+0x2c] BOX_ROTATION ( degrees )
			D3DXVECTOR3		m_vScale;			/// [+0x38] BOX_SCALE ( half extents )
			D3DXMATRIX		m_mat;				/// [+0x44] rotation * translation, built by RebuildMatrix ( 31d0cf80 )
			LutEntry*		m_pLut;				/// [+0x84] LUT_NUM entry, or NULL
			float			m_fBlendLength;		/// [+0x88] BLEND_LENGTH
			float			m_fBlendWeight;		/// [+0x8c] BLEND_WEIGHT
			float			m_fPriority;		/// [+0x90] PRIORITY
			bool			m_bEnabled;			/// [+0x94] BOX_ENABLED
			bool			m_bRimEnabled;		/// [+0x95] RIM_LIGHT_ENABLED
			D3DXVECTOR3		m_vRimColor;		/// [+0x98] RIM_LIGHT_COLOR
			float			m_fRimWide;			/// [+0xa4] RIM_LIGHT_WIDE
			float			m_fRimPower;		/// [+0xa8] RIM_LIGHT_POWER
			float			m_fRimForced;		/// [+0xac] RIM_LIGHT_FORCED

			EnvBox();
			void RebuildMatrix();
		};

	private:
		CKTDGEnvBoxMap();
		~CKTDGEnvBoxMap( void );

	public:
		static CKTDGEnvBoxMap* CreateEnvBoxMap() { return new CKTDGEnvBoxMap(); }

		virtual HRESULT OnResetDevice();		/// June 31d0b200
		virtual HRESULT OnLostDevice();			/// June 31d09040
		virtual RENDER_HINT OnFrameRender_Prepare() { return ( NULL == m_pVB ) ? RENDER_HINT_NORENDER : RENDER_HINT_DEFAULT; }	/// June 31d09ad0; the Draw slot ( 31d09030 ) is empty

		bool	LoadScript( const wstring& wstrFileName );		/// June 31d0c730
		bool	AddEnvBox_LUA();								/// June 31d0b720, Lua: g_pEnvBoxMap:AddEnvBox_LUA{ ... }
		void	SetLUT( const char* pFileNameUTF8 );			/// June 31d0a0e0, Lua: g_pEnvBoxMap:SetLUT( "..." )
		void	Render( const D3DXVECTOR3& vPos );				/// June 31d0a410

		int		GetNumBox() const { return (int) m_vecBox.size(); }
		int		GetNumLut() const { return (int) m_vecLut.size(); }

	private:
		bool	CreateQuad();									/// June 31d09900
		float	GetBoxDistance( float fX, float fY, float fZ, UINT uIndex ) const;	/// June 31d09130
		HRESULT	BeginEffect( int iTechnique );					/// June 31d094c0
		void	EndEffect();									/// June 31d097c0
		void	CommitChanges();								/// June 31d09440
		void	ClearRendererLut();
		void	CloseLutTextures();

		struct Vertex { float x, y, z, u, v; };

		std::vector<LutEntry*>		m_vecLut;			/// [+0x1d0]
		std::vector<EnvBox*>		m_vecBox;			/// [+0x200] sorted by ( priority, id ) after LoadScript
		CKTDXDeviceRenderTargetTexture*	m_apBlendedLut[2];	/// [+0x1b0] [+0x1b4] 256 x 16 render targets
		D3DXVECTOR3					m_vRimColor;		/// [+0x1b8]
		float						m_fRimPower;		/// [+0x1c4]
		float						m_fRimWide;			/// [+0x1c8]
		float						m_fRimForced;		/// [+0x1cc]
		D3DXVECTOR4					m_vWeights;			/// [+0x1e0] g_vWeights
		D3DXVECTOR4					m_vEtc;				/// [+0x1f0] g_vEtcParam: ( first LUT of this pass, LUT count, 0, 0 )
		LPD3DXEFFECT				m_pEffect;			/// [+0x210]
		D3DXHANDLE					m_ahTechnique[2];	/// [+0x214] BlendLUTTechnique, DrawEnvBoxTechnique
		D3DXHANDLE					m_ahParam[5];		/// [+0x21c] g_vWeights, g_vEtcParam, g_mMVP, g_vCurrSize, g_vColor
		LPDIRECT3DVERTEXBUFFER9		m_pVB;				/// [+0x240]
		bool						m_bInPass;			/// [+0x230]
		int							m_iSavedStackTop;	/// [+0x23c]
		bool						m_bLoggedNoEffect;	/// not June: the effect is missing ( XEnvBoxRenderer.fx not shipped yet ) and says so once
};
#endif SERV_IRUHADEV_JUNE_RENDER_ENVBOX
