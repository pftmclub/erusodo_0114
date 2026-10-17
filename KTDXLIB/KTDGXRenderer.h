#pragma once


class CKTDGXRenderer
{
public:

		enum RENDER_TYPE
		{
			RT_REAL_COLOR = 0,			/// 자체 색상
			RT_CARTOON_BLACK_EDGE,		/// 카툰 + 검은 색 외곽선
			RT_CARTOON_COLOR_EDGE,		/// 카툰 + 다른 색 외곽선
			RT_CARTOON,					/// 카툰
			RT_ADD_CARTOON_BLACK_EDGE,	/// 카툰 + 검은 색 외곽선
			RT_ADD_CARTOON_COLOR_EDGE,	/// 카툰 + 다른 색 외곽선
			RT_ADD_CARTOON,				/// 카툰
			RT_BLACK_EDGE,				/// 검은 색 외곽선
			RT_COLOR_EDGE,				/// 다른 색 외곽선

            RT_LIGHT_FLOW_BEGIN,		/// 

			RT_LIGHT_FLOW_WORLD_X = RT_LIGHT_FLOW_BEGIN,
			RT_LIGHT_FLOW_WORLD_Y,
			RT_LIGHT_FLOW_WORLD_Z,
			RT_LIGHT_FLOW_WORLD_POINT,
			RT_LIGHT_FLOW_LOCAL_X,
			RT_LIGHT_FLOW_LOCAL_Y,
			RT_LIGHT_FLOW_LOCAL_Z,
			RT_LIGHT_FLOW_LOCAL_POINT,

            RT_LIGHT_FLOW_END = RT_LIGHT_FLOW_LOCAL_POINT,
			
			RT_CARTOON_FADE,
			RT_CARTOON_FADE_NONTEX,

            RT_END,
		};

#ifdef SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
		/// June R0 map item 4 (june2017_re/r0_renderer_contract.tsv, ramp rows; June Enum.src.lua CARTOON_TEX_TYPE): June's numbering, which the
		/// data036 Enum.lua table now carries too. Slot 19 (= CTT_END) is CartoonDamage_Raid.bmp, so the ramp array has CTT_END + 1 slots.
		enum CARTOON_TEX_TYPE
		{
			CTT_NORMAL = 0,				/// CartoonNormal.bmp
			CTT_NORMAL_TWOTONE = 1,		/// CartoonNormal_Twotone.bmp
			CTT_NORMAL_USER1 = 2,		/// CartoonNormal_User1.bmp
			CTT_NORMAL_USER2 = 3,		/// CartoonNormal_User2.bmp
			CTT_RED = 4,				/// CartoonFire.BMP
			CTT_GREEN = 5,				/// CartoonGreen.bmp
			CTT_VIOLET = 6,				/// CartoonViolet.bmp
			CTT_NORMAL_COUNTER = 7,		/// CartoonCounterLight.bmp
			CTT_DAMAGE = 8,				/// CartoonDamage.bmp
			CTT_INVINCIBLE = 9,			/// CartoonInvincible.bmp
			CTT_PETRIFIED = 10,			/// CartoonPetrified.bmp
			CTT_SKYBLUE = 11,			/// CartoonSkyBlue.bmp
			CTT_SCARLET = 12,			/// CartoonScarlet.bmp
			CTT_RIM_LIGHT_WHITE = 13,	/// CartoonRimLight_White.bmp
			CTT_RIM_LIGHT_RED = 14,		/// CartoonRimLight_Red.bmp
			CTT_RIM_LIGHT_BLUE = 15,	/// CartoonRimLight_Blue.bmp
			CTT_RIM_LIGHT_DARK = 16,	/// CartoonRimLight_Dark.bmp
			CTT_RIM_LIGHT_USER1 = 17,	/// CartoonRimLight_User1.bmp
			CTT_RIM_LIGHT_USER2 = 18,	/// CartoonRimLight_User2.bmp
			CTT_END = 19,				/// not a type; the ramp at this slot is CartoonDamage_Raid.bmp
		};
#else SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
		enum CARTOON_TEX_TYPE
		{
			CTT_NORMAL = 0,			/// CartoonNormal.bmp
			CTT_RED,				/// CartoonFire.bmp
			CTT_GREEN,				/// CartoonGreen.bmp
			CTT_VIOLET,				/// CartoonViolet.bmp
			CTT_NORMAL_COUNTER,		/// CartoonCounterLight.bmp
			CTT_DAMAGE,				/// CartoonDamage.bmp
			CTT_INVINCIBLE,			///	CartoonInvincible.bmp
			CTT_PETRIFIED,			/// CartoonPetrified.bmp
			CTT_SKYBLUE,			///	CartoonSkyBlue.bmp
			CTT_SCARLET,			/// CartoonScarlet.bmp
#ifdef SERV_IRUHADEV_RANOX_RIM_LIGHT
			CTT_RIM_LIGHT_BLUE = 10,	/// Iruha 2026-09-19: Ranox port Phase 15. No committed number anywhere in this repo (march2014/data036/Enum.lua's CARTOON_TEX_TYPE table has no such member) -- this session owns the numbering. Reuses CartoonSkyBlue.bmp, loaded already for CTT_SKYBLUE.
#endif SERV_IRUHADEV_RANOX_RIM_LIGHT
			CTT_END,
		};
#endif SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX

#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
        /// June R0 map item 2 (june2017_re/r0_renderer_contract.tsv, param rows; table 33729a58, 33 entries): June inserts 15-24 after g_fEye, so March's
        /// g_fColor .. g_iShaderIndex (15-18) become 25-28, and 29-32 are appended. A handle the loaded XRenderer.fx lacks (March's file) stays NULL and its
        /// setter skips, so this builds and runs with either file.
        enum EFFECT_PARAMETER
        {
            EP_g_mViewProj,
            EP_g_mWorldViewProj,
            EP_g_Color,
            EP_g_TexOffSet01,
            EP_g_LightPos,
            EP_g_OutLineColor,
            EP_g_LightFlowPoint,
            EP_g_fTexOffSet2X,
            EP_g_fTexOffSet2Y,
            EP_g_fOutLineWide,
            EP_g_fLightFlowImpact,
            EP_g_fLightFlowWide,
            EP_g_bFog,
            EP_g_fFog,
            EP_g_fEye,
            EP_g_mView,
            EP_g_vEyeDir,
            EP_g_iTextureOp_1,
            EP_g_iTextureOp_2,
            EP_g_bIsUseEnvBoxRimLIght,
            EP_g_vRimLightColor,
            EP_g_fRimLightPower,
            EP_g_fRimLightWide,
            EP_g_fRimLightForced,
            EP_g_bIsUseRimLight,
            EP_g_fColor,
            EP_g_fDensity,
            EP_g_amWorldPalette,
            EP_g_iShaderIndex,
            EP_g_bIsUseColorGrading,
            EP_g_bIsUseViewIndependentEdge,
            EP_g_fScreenSizeX,
            EP_g_fScreenSizeY,
            EP_END,
        };//EFFECT_PARAMETER
#else SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
        enum EFFECT_PARAMETER
        {
            // matrix
            EP_g_mViewProj,
            EP_g_mWorldViewProj, 

            // vector
            EP_g_Color,
            EP_g_TexOffSet01,
            EP_g_LightPos,
            EP_g_OutLineColor,
            EP_g_LightFlowPoint,

            //float
            EP_g_fTexOffSet2X,
            EP_g_fTexOffSet2Y,
            EP_g_fOutLineWide,
            EP_g_fLightFlowImpact,
            EP_g_fLightFlowWide,

#ifdef FOG_WORLD

            EP_FOG_BEGIN,

            //bool
            EP_g_bFog = EP_FOG_BEGIN,
            //vector
            EP_g_fFog,
            EP_g_fEye,
            EP_g_fColor,
            //float
            EP_g_fDensity,
			EP_FOG_END = EP_g_fDensity,
#endif // #ifdef FOG_WORLD
            
            EP_g_amWorldPalette,
            EP_g_iShaderIndex,
            EP_END,
        };//SKIN_EFFECT_PARAMETER
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT

		struct RenderParam
		{
			RENDER_TYPE			renderType;				/// 유닛 버프에 사용
//{{ robobeg : 2008-10-24
			//D3DXMATRIX			worldMatrix;
//}} robobeg : 2008-10-24

			CARTOON_TEX_TYPE	cartoonTexType;			/// 유닛 버프에 사용
			D3DXVECTOR3			lightPos;
			float				fOutLineWide;			/// 유닛 버프에 사용
			D3DXCOLOR			outLineColor;			/// 유닛 버프에 사용 (a, r, g, b)
			
			D3DXCOLOR			color;					/// 유닛 버프에 사용
			float				fLightFlowWide;
			float				fLightFlowImpact;
			D3DXVECTOR3			lightFlowPoint;			

			D3DXVECTOR2			texOffsetStage0;
			D3DXVECTOR2			texOffsetStage1;
			D3DXVECTOR2			texOffsetStage2;

            bool				bAlphaBlend;			/// 유닛 버프에 사용
			bool				bZWriteEnable;
			bool				bZEnable;
			D3DCULL				cullMode;
			D3DBLEND			srcBlend;
			D3DBLEND			destBlend;

#ifdef FOG_WORLD
            // added by wonpok. fog
            bool                bFog;
            bool                bFogShow;
            D3DXCOLOR           fogColor;
            float               fogNearX;
            float               fogFarX;
            float               fogNearY;
            float               fogFarY;
            float               fogDensity;
#endif
#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
			/// June R0 map item 3 (RenderParam ctor 31c8f020): two ints before bAlphaTestEnable, which moves from +0x9c to +0xa4. They carry the world's
			/// cartoon ramp index (used only for CTT_NORMAL: 0..3 = NORMAL / TWOTONE / USER1 / USER2) and rim ramp index (13..18, else 13); the
			/// per-frame fills copy them from CX2World (SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON).
			int					iWorldCartoonTexIndex;	/// June +0x9c
			int					iWorldRimLightTexIndex;	/// June +0xa0
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
#ifdef ADD_ALPHATESTENABLE
			bool				bAlphaTestEnable;
#endif

			RenderParam()
			{
				Init();
			}
			void Init()
			{
				renderType			= RT_REAL_COLOR;
//{{ robobeg : 2008-10-24
				//D3DXMatrixIdentity( &worldMatrix );
//}} robobeg : 2008-10-24

				cartoonTexType		= CTT_NORMAL;
				lightPos			= D3DXVECTOR3(0,0,0);
				fOutLineWide		= 0.0f;
				outLineColor		= 0xffffffff;

				color				= 0xffffffff;
				fLightFlowWide		= 0.0f;
				fLightFlowImpact	= 0.0f;
				lightFlowPoint		= D3DXVECTOR3(0,0,0);			

				texOffsetStage0		= D3DXVECTOR2(0,0);
				texOffsetStage1		= D3DXVECTOR2(0,0);
				texOffsetStage2		= D3DXVECTOR2(0,0);

				bAlphaBlend			= false;
				bZWriteEnable		= true;
				bZEnable			= true;
				cullMode			= D3DCULL_CCW;
				srcBlend			= D3DBLEND_SRCALPHA;
				destBlend			= D3DBLEND_INVSRCALPHA;

#ifdef FOG_WORLD
                // added by wonpok. fog
                bFog        =   false;
                bFogShow    =   true;
                fogColor    =   D3DXCOLOR(0xd8e3fe);
                fogNearX    =   0.0f;
                fogFarX     =   0.0f;
                fogNearY    =   0.0f;
                fogFarY     =   0.0f;
                fogDensity  =   0.0f;
#endif
#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
				iWorldCartoonTexIndex	= 0;
				iWorldRimLightTexIndex	= 0;
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
#ifdef ADD_ALPHATESTENABLE
				bAlphaTestEnable	= false;
#endif
			}//Init()
		};//struct RenderParam


	public:

        static const char*      GetRenderTypeTechniqueName( RENDER_TYPE eInType_ );
        static const char*      GetEffectParameterName( EFFECT_PARAMETER eInParam_ );

		CKTDGXRenderer( LPDIRECT3DDEVICE9 pd3dDevice );
		virtual ~CKTDGXRenderer(void);

        void    SetColor( const D3DXCOLOR& dxcInColor_ );
        void    SetRenderPassFogAndMaxSkinningVertices( bool bInFog_, unsigned uInSkinningVertices_ );

		virtual HRESULT BeginRender( const CKTDGXRenderer::RenderParam& InRenderParam_, const D3DXMATRIX* pInMatrix_ = NULL );
		virtual HRESULT NextRender();

#ifdef SERV_IRUHADEV_RANOX_RIM_LIGHT
		void    SetRimLightTexType( CARTOON_TEX_TYPE eInType_ )
		{
			/// Iruha 2026-09-19: bounds-check here, not just at the BeginRender consumption sites -- eInType_ ultimately
			/// comes from a Lua int (CX2World::SetRimLightTextureIndex), so it can't be trusted to already be in range.
			m_eRimLightOverrideTexType = ( eInType_ >= CARTOON_TEX_TYPE(0) && eInType_ < CTT_END ) ? eInType_ : CTT_END;
#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
			/// R2: superseded. The world's rim index now reaches June's path (CX2World::SetRimLightTextureIndex stores it; the units copy it into
			/// RenderParam::iWorldRimLightTexIndex; BeginRender binds the real rim ramp in stage 2), so nothing reads m_eRimLightOverrideTexType and R1's
			/// SkyBlue stand-in mapping below is bypassed.
#else SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
#ifdef SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
			/// R1 play-test 1 (2026-10-17): June's CartoonRimLight_*.bmp are 256x8 additive rim overlays (black except the far edge), not 16x1 cel ramps,
			/// so binding one in the cel stage turns units black. Until R2 binds the rim ramp in stage 2 the June way, the Ranox stand-in keeps the
			/// tint it had before R1 (CartoonSkyBlue.bmp) for CTT_RIM_LIGHT_BLUE, and the other rim types stay disabled as they were (out of range).
			if( CTT_RIM_LIGHT_BLUE == m_eRimLightOverrideTexType )
				m_eRimLightOverrideTexType = CTT_SKYBLUE;
			else if( m_eRimLightOverrideTexType >= CTT_RIM_LIGHT_WHITE && m_eRimLightOverrideTexType < CTT_END )
				m_eRimLightOverrideTexType = CTT_END;
#endif SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
		}
		CARTOON_TEX_TYPE GetRimLightTexType() const { return m_eRimLightOverrideTexType; }
#endif SERV_IRUHADEV_RANOX_RIM_LIGHT

#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
		/// June R0 map items 1-2: the option-driven switches (CX2GameOption keys, see SERV_IRUHADEV_JUNE_RENDER_RIM_OPTION) and the device caps.
		/// SetRimLight is June 31d3c210 (+0x18c, written every frame by CX2Main), SetAnisotropy June 3242ed40, SetEnvBoxEnable June 32421110
		/// (+0x80 = option USE_ENV_BOX && +0x58), SetViewIndependentEdge the same function (+0x81).
		void    SetRimLight( bool bInRimLight_ ) { m_bRimLight = bInRimLight_; }
		bool    GetRimLight() const { return m_bRimLight; }
		void    SetViewIndependentEdge( bool bInEdge_ ) { m_bViewIndependentEdge = bInEdge_; }
		void    SetEnvBoxEnable( bool bInEnvBox_ ) { m_bEnvBox = ( m_bShader30 && bInEnvBox_ ); }
		void    SetAnisotropy( int iInSamples_ );
		bool    IsShader30() const { return m_bShader30; }
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
#ifdef X2OPTIMIZE_CULLING_WORLDOBJECTMESH_SUBSET
		HRESULT OnFrameRender( const RenderParam& InRenderParam_, const D3DXMATRIX& mInWorld_, CKTDXDeviceXMesh& pXMesh, 
			CKTDXDeviceXET* pTexChangeXET = NULL, CKTDXDeviceXET* pMultiTexXET = NULL, 
			const CKTDXDeviceXET::AniData* pAniData = NULL, float fAniTime = 0, unsigned uInDrawCount = 1, std::vector<bool>* pvecDrawSubset = NULL );
#else//X2OPTIMIZE_CULLING_WORLDOBJECTMESH_SUBSET
		HRESULT OnFrameRender( const RenderParam& InRenderParam_, const D3DXMATRIX& mInWorld_, CKTDXDeviceXMesh& pXMesh, 
			CKTDXDeviceXET* pTexChangeXET = NULL, CKTDXDeviceXET* pMultiTexXET = NULL, 
			const CKTDXDeviceXET::AniData* pAniData = NULL, float fAniTime = 0, unsigned uInDrawCount = 1 );
#endif//X2OPTIMIZE_CULLING_WORLDOBJECTMESH_SUBSET

        void    DrawFrame( CKTDXDeviceXSkinMesh& kInXSkinMesh_, CKTDXDeviceXSkinMesh::MultiAnimFrame& kInFrame_
            , CKTDXDeviceXET* pTexChangeXET = NULL, CKTDXDeviceXET* pMultiTexXET = NULL
            , const CKTDXDeviceXET::AniData* pAniData = NULL, float fAniTime = 0, int iInDetailPercent = 0, bool abInUseTex_[3] = NULL
            , CKTDXDeviceXET* pAnimAniExt = NULL, const wchar_t* pwszInNowAnimName_ = NULL );

        HRESULT BeginRenderGroup( CKTDGXRenderer::RENDER_TYPE eInRenderType_ );
        HRESULT NextRenderGroup();

		virtual HRESULT	OnResetDevice();
		virtual HRESULT	OnLostDevice();

//{{ robobeg : 2008-10-28

        // 외부에서 KTDGXRenderer 의 effect 를 직접 access 하는 일이 없도록 한다.

        //D3DXHANDLE   GetHandle( EFFECT_PARAMETER eInParam_ ) const 
        //{ 
        //    return ( eInParam_ >= EFFECT_PARAMETER( 0 ) && eInParam_ < EP_END ) ? m_ahEffectParameters[ eInParam_ ] : NULL; 
        //}//GetHandle()
//}} robobeg : 2008-10-28



    private:

        void                        _CommitChanges();

#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
        /// Setters that skip a parameter the loaded XRenderer.fx does not have (March's file lacks the 14 new ones).
        void                        _JuneSetBool( EFFECT_PARAMETER eInParam_, bool bInValue_ );
        void                        _JuneSetInt( EFFECT_PARAMETER eInParam_, int iInValue_ );
        void                        _JuneSetFloat( EFFECT_PARAMETER eInParam_, float fInValue_ );
        void                        _JuneSetVector( EFFECT_PARAMETER eInParam_, const D3DXVECTOR4& vInValue_ );
        void                        _JuneSetEdgeParams();
        void                        _JuneBindCartoonRamps( const RenderParam& InRenderParam_, bool bInTypeGuard_, bool bInStoreRimIndex_, bool& bOutRimRamp_ );
        void                        _JuneSetUpperStages( CKTDXDeviceBaseTexture* pInTex1_, CKTDXDeviceBaseTexture* pInTex2_, CKTDXDeviceXET* pInMultiTexXET_ );
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT

        CKTDXDeviceBaseTexture*     _SetNowTexture( CKTDXDeviceBaseTexture* orgTex, int stage,
													CKTDXDeviceXET* pTexChangeXET, CKTDXDeviceXET* pMultiTexXET, 
													const CKTDXDeviceXET::AniData* pAniData, float fAniTime,
                                                    bool abInUseTex_[3], CKTDXDeviceXET* pAnimAniExt, const wchar_t* pwszInNowAnimName_ );


    private:

		LPD3DXEFFECT				m_pEffect;

#ifdef SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
		CKTDXDeviceTexture*			m_pCartoonTexList[CTT_END + 1];	/// June: 20 ramps, slot CTT_END = CartoonDamage_Raid.bmp
#else SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX
		CKTDXDeviceTexture*			m_pCartoonTexList[CTT_END]; 
#endif SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX

#ifdef SERV_IRUHADEV_RANOX_RIM_LIGHT
		CARTOON_TEX_TYPE			m_eRimLightOverrideTexType;	/// Iruha 2026-09-19: Ranox port Phase 15. CTT_END = disabled (sentinel); substituted for CTT_NORMAL in BeginRender's 3 cartoon-tex sites.
#endif SERV_IRUHADEV_RANOX_RIM_LIGHT

        D3DXHANDLE                  m_ahTechniques[RT_END];
        D3DXHANDLE                  m_ahEffectParameters[EP_END];

        bool                        m_bInRenderGroup;
        bool                        m_bInBeginPass;
        UINT                        m_uNumPasses;
        UINT                        m_uCurPass;
        int                         m_iStackTop;

        bool                        m_bFog;	/// 포그 사용 여부 지정
        unsigned                    m_uMaxSkinningVertices;

#ifndef X2OPTIMIZE_SETSHADERCONSTANT
		bool                        m_bCommitChangesPending;
#endif//X2OPTIMIZE_SETSHADERCONSTANT

        CKTDGXRenderer::RENDER_TYPE m_eCurrentRenderType;
        int                         m_iCurrentShaderIndex;
        D3DXCOLOR                   m_dxcCurrentColor;

#ifdef SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
        /// June renderer members (r0_renderer_contract.tsv, renderer rows; the June offset is in brackets).
        bool                        m_bShader30;                /// [+0x58] PixelShaderVersion >= 3.0 (ctor), the DISABLE_SHADER30 macro when 0
        bool                        m_bGradingLutPresent;       /// [+0x59] CKTDGEnvBoxMap writes it (R3); stays 0 without an env box
        bool                        m_bGradingThisDraw;         /// [+0x5b] BeginRender: cartoon / real-color case && m_bEnvBox && m_bGradingLutPresent
        CKTDXDeviceBaseTexture*     m_apGradingLut[2];          /// [+0x5c] [+0x60] blended LUTs (R3 writes), bound to stage 3
        D3DXVECTOR3                 m_vEnvBoxRimColor;          /// [+0x68..+0x70] env box rim colour (R3 writes)
        float                       m_fEnvBoxRimPower;          /// [+0x74]
        float                       m_fEnvBoxRimWide;           /// [+0x78]
        float                       m_fEnvBoxRimForced;         /// [+0x7c]
        bool                        m_bEnvBox;                  /// [+0x80] option USE_ENV_BOX && m_bShader30
        bool                        m_bViewIndependentEdge;     /// [+0x81] option USE_VIEW_INDEPENDENT_EDGE, ctor 1
        bool                        m_bMipmapCap;               /// [+0x82] TextureCaps MIPMAP (shader 3 devices only)
        int                         m_iAnisotropy;              /// [+0x84] anisotropy in use, ctor 1
        int                         m_iMaxAnisotropy;           /// [+0x88] D3DCAPS9.MaxAnisotropy on a shader 3 device with the ANISOTROPY cap, else 1
        bool                        m_bRimLight;                /// [+0x18c] option RIM_LIGHT, written every frame by CX2Main
        bool                        m_bRimLightThisDraw;        /// [+0x18d] BeginRender: a rim-capable cartoon case with its ramp && m_bRimLight
        int                         m_iRimRampIndex;            /// [+0x190] rim ramp of this draw, 13..18, reset to 13 every BeginRender
        bool                        m_bRimShader;               /// the loaded XRenderer.fx has g_bIsUseRimLight (June's file); March's does not, so no rim state is bound
#endif SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT
};//class CKTDGXRenderer


#define RENDER_HINT_NORENDER    RENDER_HINT( RENDER_HINT::NORENDER )
#define RENDER_HINT_DEFAULT     RENDER_HINT( RENDER_HINT::DEFAULT )
#define RENDER_HINT_XMESH( eRenderType, bFog )  RENDER_HINT( RENDER_HINT::XMESH, eRenderType, bFog )
#define RENDER_HINT_XSKINMESH( eRenderType, bFog, uMaxSkinning ) RENDER_HINT( RENDER_HINT::XSKINMESH, eRenderType, bFog, uMaxSkinning )

struct  RENDER_HINT
{
    enum    ERenderHint
    {
       NORENDER = -1,
       DEFAULT = 0,
       XMESH = 1,
       XSKINMESH = 2,
    };//enum    ERenderHint

    ERenderHint                 m_eHint;
    CKTDGXRenderer::RENDER_TYPE m_eRenderType;
    bool                        m_bFog;
    unsigned                    m_uMaxSkinningVertices;

    RENDER_HINT( ERenderHint eInRenderHint_ = DEFAULT , CKTDGXRenderer::RENDER_TYPE eInXRendererType_ = CKTDGXRenderer::RENDER_TYPE( 0 )
        , bool bInFog_ = false, unsigned uInMaxSkinning_ = 0 )
        : m_eHint( eInRenderHint_ ), m_eRenderType( eInXRendererType_ ), m_bFog( bInFog_ ), m_uMaxSkinningVertices( uInMaxSkinning_ ) {}

    bool IsSameCategory( const RENDER_HINT& kIn_ ) const
    {
        if ( m_eHint != kIn_.m_eHint )
            return false;
        else
        {
            if ( m_eHint == XMESH || m_eHint == XSKINMESH )
                return m_eRenderType == kIn_.m_eRenderType;
            else
                return true;
        }//if.. else...
    }//IsSameCategory()


    void MergeRenderHint( const RENDER_HINT& kIn_ )
    {
        switch( m_eHint )
        {
        case NORENDER:
            *this = kIn_;
            break;
        case DEFAULT:
            break;
        default: // XMESH, XSKINMESH
            if ( kIn_.m_eHint == NORENDER )
                break;
            else if ( kIn_.m_eHint == DEFAULT || !IsSameCategory( kIn_ ) )
                *this = RENDER_HINT_DEFAULT;
            else
            {
                if ( kIn_.m_bFog )
                    m_bFog = kIn_.m_bFog;
                if ( m_uMaxSkinningVertices < kIn_.m_uMaxSkinningVertices )
                    m_uMaxSkinningVertices = kIn_.m_uMaxSkinningVertices;
            }//if.. else..   
        }//switch
    }//MergeRenderHint()

    bool operator < ( const RENDER_HINT& kIn_ ) const
    {
        if ( m_eHint < kIn_.m_eHint )
            return true;
        else if ( m_eHint > kIn_.m_eHint )
            return false;
        else
        {
            if ( m_eHint == XMESH )
            {
                if ( m_eRenderType < kIn_.m_eRenderType )
                    return true;
                else if ( m_eRenderType > kIn_.m_eRenderType )
                    return false;
                else
                {
                    if ( m_eRenderType >= CKTDGXRenderer::RT_LIGHT_FLOW_BEGIN && m_eRenderType <= CKTDGXRenderer::RT_LIGHT_FLOW_END )
                        return false;
                    else 
                        return ( !m_bFog && kIn_.m_bFog );
                }//if.. else..
            }
            else if ( m_eHint == XSKINMESH )
            {
                if ( m_eRenderType < kIn_.m_eRenderType )
                    return true;
                else if ( m_eRenderType > kIn_.m_eRenderType )
                    return false;
                else
                {
                    if ( m_eRenderType >= CKTDGXRenderer::RT_LIGHT_FLOW_BEGIN && m_eRenderType <= CKTDGXRenderer::RT_LIGHT_FLOW_END )
                        return false;
                    else if ( !m_bFog && kIn_.m_bFog )
                        return true;
                    else if ( m_bFog && !kIn_.m_bFog )
                        return false;
                    else
                        return m_uMaxSkinningVertices < kIn_.m_uMaxSkinningVertices;
                }//if.. else..
            }
            else
            {
                return false;
            }//if.. else..
        }//if.. else..
    }//operator < ()

    bool operator > ( const RENDER_HINT& kIn_ ) const
    {
        return kIn_.operator < ( *this );
    }//

    bool operator == ( const RENDER_HINT& kIn_ ) const
    {
        return !( *this < kIn_ ) && !( kIn_ < *this );
    }//

    bool operator != ( const RENDER_HINT& kIn_ ) const
    {
        return !( operator == ( kIn_ ) );
    }//

    bool operator <= ( const RENDER_HINT& kIn_ ) const
    {
        return !( kIn_ < *this );
    }//

    bool operator >= ( const RENDER_HINT& kIn_ ) const
    {
        return !( *this < kIn_ );
    }//
};//struct  RENDER_HINT




