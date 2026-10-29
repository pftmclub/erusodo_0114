#include "stdafx.h"
#include ".\x2eqip.h"
#include <boost/static_assert.hpp>
#include "boost_kog_extension.h"


// constructor
CX2Eqip::CX2Eqip( CX2Item* pItem, CKTDGXSkinAnimPtr pUnitXSkinAnim, int enchantLevel, bool bDummyBasic, bool bLoad,
				 CX2Unit::UNIT_TYPE eUnitType /*= CX2Unit::UT_NONE */)
//{{ robobeg : 2008-10-17
    : CKTDGObject( NULL )
//}} robobeg : 2008-10-17
{
    ASSERT( ::GetCurrentThreadId() == g_pKTDXApp->GetMainThreadID() );
//{{ robobeg : 2008-10-17
	//m_bShow = true;
//}} robobeg : 2008-10-17

//{{ seojt // 2008-12-15, 10:56
    /** @NOTE:
        리소스 쓰레드 로딩을 구현할 때, 로드된 리소스 객체의 포인터가 32비트로 표현된다고 가정한다.
        그렇지 않으면, 포인터가 유효한지를 검사하기 위해 포인터를 직접 검사하는 것은 무효invalid
        하다.
        후에, 포인터의 read/write가 atomic하다는 보장이 되지 않으면, 로드된 리소스 객체가 유효valid
        한지를 판단하기 위해 별도의 bool변수를 사용해야 할 것이다.
        - jintaeks on 2008-12-15, 10:59 */
	BOOST_STATIC_ASSERT( sizeof(CKTDGXSkinAnim*) == sizeof(int) );
	BOOST_STATIC_ASSERT( sizeof(CKTDXDeviceXSkinMesh*) == sizeof(int) );
	BOOST_STATIC_ASSERT( sizeof(CKTDXDeviceXMesh*) == sizeof(int) );
//}} seojt // 2008-12-15, 10:56


	KTDXPROFILE();


	m_bLoadedAttached = false;

	m_eObjectType = OT_CX2Eqip;

	m_bDummyBasic				= bDummyBasic;
	if ( m_bDummyBasic == true )
		m_pDummyItem = pItem;
	else
		m_pDummyItem = NULL;

	m_EnchantLevel				= enchantLevel;

	m_pItemTemplet				= pItem->GetItemTemplet();
    m_ItemID					= pItem->GetItemTemplet()->GetItemID();
	m_ItemUID					= pItem->GetUID();
	m_EqipPos					= pItem->GetItemTemplet()->GetEqipPosition();
	m_pOwnerUnit				= pItem->GetOwnerUnit();
	if( NULL != m_pOwnerUnit )
	{
		m_OwnerUnitUID				= m_pOwnerUnit->GetUID();
	}
	else
	{
		m_OwnerUnitUID				= -1;
	}


	//{{ seojt // 2009-1-12, 16:19
	m_pUnitXSkinAnim			= pUnitXSkinAnim; 
    //m_pUnitXSkinAnim = pUnitXSkinAnim->get_shared_ptr<CKTDGXSkinAnim>();
	//}} seojt // 2009-1-12, 16:19

	m_bHyperMode				= false; 
	m_bCanHyperMode				= pItem->GetItemTemplet()->GetCanHyperMode();
	m_EqipType					= pItem->GetItemTemplet()->GetUseType();

	m_Offset					= D3DXVECTOR3( 0,0,0 );
	m_fTexOffset				= 0.0f;
	if( m_pOwnerUnit != NULL && 
        m_pItemTemplet != NULL 
        )
	{
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
        m_Offset = m_pItemTemplet->GetFrameOffset( m_pOwnerUnit->GetType() );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
		switch( m_pOwnerUnit->GetType() )
		{
		default:
		case CX2Unit::UT_ELSWORD:
			m_Offset = m_pItemTemplet->m_FrameOffsetElsword;
			break;

		case CX2Unit::UT_ARME:
			m_Offset = m_pItemTemplet->m_FrameOffsetAisha;
			break;

		case CX2Unit::UT_LIRE:
			m_Offset = m_pItemTemplet->m_FrameOffsetLena;
			break;

		case CX2Unit::UT_RAVEN:
			m_Offset = m_pItemTemplet->m_FrameOffsetRaven;
			break;

		case CX2Unit::UT_EVE:
			m_Offset = m_pItemTemplet->m_FrameOffsetEve;
			break;

//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
		case CX2Unit::UT_CHUNG:
			m_Offset = m_pItemTemplet->m_FrameOffsetChung;
			break;
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

#ifdef ARA_CHARACTER_BASE
		case CX2Unit::UT_ARA:
			m_Offset = m_pItemTemplet->m_FrameOffsetAra;
			break;
#endif
		}
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	}	

	m_fAttatchNormalNowAniTime	= 0.0f;


//{{ robobeg : 2008-10-21
    m_iXSkinModelAttachment = 0;
//}} robobeg : 2008-10-21

	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	{
		m_pUnitMatrix[i]	= NULL;
		D3DXMatrixIdentity( &m_mAttachNormalMatrix[i] );

		m_pNormalXSkinAnim[i] = NULL;
		m_pHyperXSkinAnim[i] = NULL;

		m_pNormalXSkinModel[i] = NULL;
		m_pHyperXSkinModel[i] = NULL;

		m_pNormalXMesh[i] = NULL;
		m_pHyperXMesh[i] = NULL;

		m_ModelName[i] = L"";
		m_ModelNameHyper[i] = L"";
	}

	m_pNormalTexChangeXET		= NULL;
	m_pNormalMultiTexXET		= NULL;
	m_pNormalAniXET				= NULL;
	m_pNormalAniData			= NULL;

	m_pHyperTexChangeXET		= NULL;
	m_pHyperMultiTexXET			= NULL;
	m_pHyperAniXET				= NULL;
	m_pHyperAniData				= NULL;

#ifdef NEW_VILLAGE_UI
	m_bFixed					= false;
#endif

	bool bCommonItem = false;

#ifdef COMMON_ITEM_TEMPLET_TEST
	if( CX2Unit::UT_NONE != eUnitType 
//#ifndef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
//        &&
//		false == m_pItemTemplet->m_mapCommonItemModelName.empty() 
//#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
        ) // 공용 아이템
	{
        const wchar_t* pCIMName = m_pItemTemplet->GetCommonItemModelName( eUnitType );
        if ( pCIMName[0] != NULL )
        {
			bCommonItem = true;
			m_ModelName[0] = pCIMName;

			// 오현빈 // 공용 아이템 XET 적용 되도록 추가
			const wchar_t* pCIMXetName = m_pItemTemplet->GetCommonItemXETName( eUnitType );
			if( pCIMXetName[0] != NULL )
				m_ChangeTexName = pCIMXetName;
			else
				m_ChangeTexName = m_pItemTemplet->GetTextureChangeXETName();

        }
		else
		{
			bCommonItem = false;
		}
		//map<CX2Unit::UNIT_TYPE, wstring>::const_iterator it = m_pItemTemplet->m_mapCommonItemModelName.find( eUnitType );
		//if( it != m_pItemTemplet->m_mapCommonItemModelName.end() )
		//{
		//	bCommonItem = true;
		//	m_ModelName[0] = it->second;
		//}
		//else
		//{
		//	bCommonItem = false;
		//}
		// fix!!! remove 공용아이템 XET는 불필요, 대신에 attach frame name은 필요. 
		//if( true == bDone )
		//{
		//	it = m_pItemTemplet->m_mapCommonItemXETName.find( eUnitType );
		//	if( it != m_pItemTemplet->m_mapCommonItemXETName.end() )
		//	{
		//		m_ChangeTexName = it->second;
		//	}
		//	else
		//	{
		//		m_ChangeTexName = L"";
		//	}
		//}
	}
#endif COMMON_ITEM_TEMPLET_TEST

	if( false == bCommonItem )
	{
		//ASSERT( false == m_pItemTemplet->m_ModelName.empty() );  
		// note!! '분노의 반지'와 같이 렌더링 되는 메시가 없는 장착장비도 있다.

		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
		{
            m_ModelName[i] = m_pItemTemplet->GetModelName(i);
		}

		m_ChangeTexName = m_pItemTemplet->GetTextureChangeXETName();
	}
	
	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	{
		if( false == m_ModelName[i].empty() )
		{
			m_ModelNameHyper[i]			= L"Hyper_"; 
			m_ModelNameHyper[i]			+= m_ModelName[i];
		}
	}

	if( false == m_ChangeTexName.empty() )
	{
		m_ChangeTexNameHyper		= L"Hyper_";
		m_ChangeTexNameHyper		+= m_ChangeTexName;
	}
	
    const wchar_t* pAniXETName = m_pItemTemplet->GetAniXETName();
    if ( wcslen( pAniXETName ) >= 3 )
	{
        m_AniXETName				= pAniXETName; 
		m_AniXETNameHyper			= L"Hyper_";
		m_AniXETNameHyper			+= m_AniXETName;
	}
	
    const wchar_t* pAniName = m_pItemTemplet->GetAniName();
    if ( wcslen( pAniName ) >= 3 )
	{
        m_AniName					= pAniName; 
		m_AniNameHyper				= L"Hyper_";
		m_AniNameHyper				+= m_AniName;
	}
	

	m_bResourceLoadReserved = bLoad;
	//EquipResourceLoad(bLoad);
#ifdef SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET
	// June ctor 32961280: +0x15f8 = -1 early (before the templet stores), then at its tail +0x15f4 = 0 and +0x15fc / +0x1600 /
	// +0x1604 = 0. Nothing reads them in between and March's ctor has no early return, so all five are set here.
	m_fEquipEffectSetStartTime = 0.f;
	m_hEquipEffectSet = INVALID_EFFECTSET_HANDLE;
	m_afEquipEffectSetSwitchTime[0] = 0.f;
	m_afEquipEffectSetSwitchTime[1] = 0.f;
	m_afEquipEffectSetSwitchTime[2] = 0.f;
#endif SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET

}//CX2Eqip::CX2Eqip()

#ifdef SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET
// June2017 3295ca60: stop the per-equip effect set (+0x15f8) on the UI effect set (g_pData +0x9c) only when bStop and the handle is
// valid; the handle is forgotten (-1) either way. The GUUser and square miss paths pass false, so the set plays on to its own end;
// only the square's RemoveUnit stop (323f56d0) passes true. June has no CX2Eqip dtor stop (3 callers only). Both: JUNE_BUGS.md.
void CX2Eqip::ReleaseEquipEffectSet( bool bStop )
{
	if( true == bStop && NULL != g_pData && INVALID_EFFECTSET_HANDLE != m_hEquipEffectSet )
		g_pData->GetUIEffectSet()->StopEffectSet( m_hEquipEffectSet );
	m_hEquipEffectSet = INVALID_EFFECTSET_HANDLE;
}

// June2017 32961060: the per-equip effect-set timer step; true = play now. *pSel is the owner kind (0 GUUser, 1 square, 2 viewer);
// the switch roll turns it into 3 = play the switch set (the timer is NOT reset here: the caller zeroes it when it plays the switch).
// Elapsed time is the global one: June reads the protected float at g_pKTDXApp +0x2c, whose ctor value is 1/85 = m_fConstElapsedTime.
// So the first play waits START_TIME, and a set that ended replays about one frame later.
bool CX2Eqip::StepEquipEffectSetTimer( int* pSel )
{
	if( NULL == g_pData || NULL == g_pData->GetItemManager() )
		return false;

	CX2ItemManager::ConditionEquipPlayEffectSet kRecord;
	if( false == g_pData->GetItemManager()->GetConditionEquipPlayEffectSet( m_ItemID, kRecord ) )
		return false;

	// switch roll: only for a positive percent (NaN: no) and an owner kind 0..2
	if( 0.f < kRecord.m_fSwitchEffectPercent && 0 <= *pSel && *pSel < 3 )
	{
		if( m_afEquipEffectSetSwitchTime[*pSel] >= kRecord.m_fSwitchLimitTime )	// NaN on either side -> accumulate
		{
			if( static_cast<float>( rand() % 100 ) < kRecord.m_fSwitchEffectPercent )		// strict
				*pSel = 3;
		}
		else
		{
			m_afEquipEffectSetSwitchTime[*pSel] += g_pKTDXApp->GetElapsedTime();
		}
	}

	// GetEffectSetInstance 321d97c0 on g_pData +0x9c: a live set keeps the accumulator at START_TIME
	if( NULL != g_pData->GetUIEffectSet()->GetEffectSetInstance( m_hEquipEffectSet ) )
	{
		m_fEquipEffectSetStartTime = kRecord.m_fStartTime;
		return false;
	}

	// @32961233-32961244: fld acc, fld START_TIME, fcompp, TEST AH,1 (C0), JNE play. C0 is also set when unordered, so START_TIME
	// < acc strictly OR a NaN on either side plays (C9f-2 review; equal accumulates).
	if( !( kRecord.m_fStartTime >= m_fEquipEffectSetStartTime ) )
	{
		m_fEquipEffectSetStartTime = 0.f;
		return true;
	}

	m_fEquipEffectSetStartTime += g_pKTDXApp->GetElapsedTime();
	return false;
}
#endif SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET


// destructor.
CX2Eqip::~CX2Eqip(void)
{
    ASSERT( ::GetCurrentThreadId() == g_pKTDXApp->GetMainThreadID() );

//{{ robobeg : 2008-10-21

    if( m_pUnitXSkinAnim != NULL )
    {
	    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	    {
		    switch( m_iXSkinModelAttachment )
		    {
		    case 1:
			    m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );
			    break;
		    case 2:
			    m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
			    break;
		    }//switch
	    }
    }//if
    m_iXSkinModelAttachment = 0;

	//m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel );
	//m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel );

//}} robobeg : 2008-10-21



	SAFE_CLOSE( m_pNormalTexChangeXET );
	SAFE_CLOSE( m_pNormalMultiTexXET );
	SAFE_CLOSE( m_pNormalAniXET );

	SAFE_CLOSE( m_pHyperTexChangeXET );
	SAFE_CLOSE( m_pHyperMultiTexXET );
	SAFE_CLOSE( m_pHyperAniXET );


	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	{
		SAFE_DELETE_KTDGOBJECT( m_pNormalXSkinAnim[i] );
		SAFE_DELETE_KTDGOBJECT( m_pHyperXSkinAnim[i] );

		SAFE_CLOSE( m_pNormalXSkinModel[i] );
		SAFE_CLOSE( m_pHyperXSkinModel[i] );

		SAFE_CLOSE( m_pNormalXMesh[i] );
		SAFE_CLOSE( m_pHyperXMesh[i] );
	}


	if( m_bDummyBasic == true )
	{
		SAFE_DELETE( m_pDummyItem );
	}


	BOOST_TEST_FOREACH( CX2SubEquip*, pSubEquip, m_vecpSubEquip )
	{
		SAFE_DELETE_KTDGOBJECT( pSubEquip );
	}
	m_vecpSubEquip.clear();

}//CX2Eqip::~CX2Eqip()


//{{ seojt // 2008-10-15, 15:22
void CX2Eqip::PreLoad()
{
	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	{
		if( NULL != m_pNormalXSkinAnim[i] )
			m_pNormalXSkinAnim[i]->PreLoad();

		if( NULL != m_pHyperXSkinAnim[i] )
			m_pHyperXSkinAnim[i]->PreLoad();

		if( NULL != m_pNormalXSkinModel[i] )
			m_pNormalXSkinModel[i]->PreLoad();

		if( NULL != m_pHyperXSkinModel[i] )
			m_pHyperXSkinModel[i]->PreLoad();

		if( NULL != m_pNormalXMesh[i] )
			m_pNormalXMesh[i]->PreLoad();

		if( NULL != m_pHyperXMesh[i] )
			m_pHyperXMesh[i]->PreLoad();
	}

}//CX2Eqip::PreLoad()
//}} seojt // 2008-10-15, 15:22





//{{ robobeg : 2008-10-17
/*virtual*/
void    CX2Eqip::NotifyShowObjectChanged()
{
    _UpdateSkin();

	for( UINT i=0; i<m_vecpSubEquip.size(); i++ )
	{
		m_vecpSubEquip[i]->SetShowObject( GetShowObject() );
	}


#ifdef EQUIP_ATTACHED_PARTICLE_TEST
	m_AttachedParticleGroup.SetShowObject( GetShowObject() );
#endif EQUIP_ATTACHED_PARTICLE_TEST

}//CX2Eqip::SetShowObject()






// OnFrameMove 에서 RenderParam 세팅을 분리하여 OnFrameMove 이전에 실행하도록 수정
HRESULT CX2Eqip::SetRenderParam( double fTime, float fElapsedTime, CKTDGXRenderer::RenderParam* pRenderParam )
{
	if( m_pItemTemplet == NULL )
		return E_FAIL;
    DWORD   dwItemID = m_pItemTemplet->GetItemID();

    if ( pRenderParam == NULL )
        return S_OK;

	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	{
		switch( m_EqipType )
		{
		case CX2Item::UT_ATTACH_ANIM:
			{
				if( m_pNormalXSkinAnim[i] != NULL )
				{
					CKTDGXRenderer::RenderParam* pEqipRenderParam = m_pNormalXSkinAnim[i]->GetRenderParam();
					*pEqipRenderParam = *pRenderParam;                        

					int iEnchantType = GetEnchantType();
					if( iEnchantType != 0 )
					{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_UPGRADE_WEAPON_PARTICLE
						m_fTexOffset							+= fElapsedTime * 1.0f;	// March US's speed: June's eqip UV step sits in its SetRenderParam rewrite ( june-engine R6b )
#else SERV_IRUHADEV_JUNE_DIVERGE_UPGRADE_WEAPON_PARTICLE
#ifdef ADD_UPGRADE_WEAPON_PARTICLE		/// 11강 이상부터는 UV애니메이션 속도를 다르게 설정
						m_fTexOffset							+= fElapsedTime * GetUpgradeWeaponUVAnimatonSpeed();
#else  ADD_UPGRADE_WEAPON_PARTICLE
						m_fTexOffset							+= fElapsedTime * 1.0f;
#endif  ADD_UPGRADE_WEAPON_PARTICLE
#endif SERV_IRUHADEV_JUNE_DIVERGE_UPGRADE_WEAPON_PARTICLE
						pEqipRenderParam->texOffsetStage2.y		= m_fTexOffset;
						float fImpact							= m_EnchantRenderParam.fLightFlowImpact;						
						m_EnchantRenderParam					= *pEqipRenderParam;
						m_EnchantRenderParam.renderType			= CKTDGXRenderer::RT_LIGHT_FLOW_LOCAL_Z;
						m_EnchantRenderParam.fLightFlowWide		= 5.0f;
						//{{ robobeg : 2008-10-17
						if( iEnchantType == 2 )
							//}} robobeg : 2008-10-17
						{
							m_EnchantRenderParam.color				= 0xffff2222;
						}
						else if( iEnchantType == 3 )
						{
							bool bDone = false;

							for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_GOLD ); i++ )
							{
                                if( ITEM_ID_ENCHANT_TYPE3_GOLD[i] == dwItemID )
								{
									m_EnchantRenderParam.color				= 0xffffea00;
									bDone = true;
									break;
								}
							}

							if( false == bDone )
							{
								for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_BLUE ); i++ )
								{
                                    if( ITEM_ID_ENCHANT_TYPE3_BLUE[i] == dwItemID )
									{
										m_EnchantRenderParam.color				= D3DXCOLOR( 25.f/255.f, 100.f/255.f, 1.f, 1.f );
										bDone = true;
										break;
									}
								}
							}


							if( false == bDone )
							{
								for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_GREEN ); i++ )
								{
                                    if( ITEM_ID_ENCHANT_TYPE3_GREEN[i] == dwItemID )
									{
										m_EnchantRenderParam.color				= D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );
										bDone = true;
										break;
									}

								}		
							}


							if( false == bDone )
							{
								for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_YELLOW ); i++ )
								{
                                    if( ITEM_ID_ENCHANT_TYPE3_YELLOW[i] == dwItemID )
									{
										m_EnchantRenderParam.color				= D3DXCOLOR( 1.f, 1.f, 0.f, 1.f );
										bDone = true;
										break;
									}

								}
							}

							
							if( false == bDone )
							{
								for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_RED ); i++ )
								{
									if( ITEM_ID_ENCHANT_TYPE3_RED[i] == dwItemID )
									{
										m_EnchantRenderParam.color				= D3DXCOLOR( 1.f, 0.f, 0.f, 1.f );
										bDone = true;
										break;
									}
								}
							}

							if( false == bDone )
							{
								for( int i=0; i<ARRAY_SIZE( ITEM_ID_ENCHANT_TYPE3_PINK ); i++ )
								{
									if( ITEM_ID_ENCHANT_TYPE3_PINK[i] == dwItemID )
									{
										m_EnchantRenderParam.color				= D3DXCOLOR( 0.996f, 0.619f, 1.f, 1.f );
										bDone = true;
										break;
									}
								}
							}							
						}
						else
						{
							m_EnchantRenderParam.color				= 0xffffcccc;
						}

						m_EnchantRenderParam.fLightFlowImpact	= fImpact - fElapsedTime * 200.0f;
						m_EnchantRenderParam.bAlphaBlend		= true;
						m_EnchantRenderParam.srcBlend			= D3DBLEND_SRCALPHA;
						m_EnchantRenderParam.destBlend			= D3DBLEND_DESTALPHA;
						if( m_EnchantRenderParam.fLightFlowImpact < -300.0f )
							m_EnchantRenderParam.fLightFlowImpact = 300.0f;
					}

#ifdef JIGGLEBONE_TEST
					for( int i=0; i<ARRAY_SIZE( ITEM_ID_JIGGLE_TYPE1 ); i++ )
					{
						if( ITEM_ID_JIGGLE_TYPE1[i] == dwItemID )
						{						
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
							pEqipRenderParam->fOutLineWide		= CARTOON_OUTLINE_WIDTH;
#else //UNIT_SCALE_COMBINE_ONE
							pEqipRenderParam->fOutLineWide		= 1.5f;
#endif //UNIT_SCALE_COMBINE_ONE
						}
					}
#endif

					//{{ kimhc // 2010-01-12 // 외곽선 없앨 UT_ATTACH_ANIM 악세사리
					for ( int i = 0; i < ARRAY_SIZE( REAL_COLOR_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( REAL_COLOR_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;
						}
					}
					//}} kimhc // 2010-01-12 // 외곽선 없앨 UT_ATTACH_ANIM 악세사리

					for ( int i = 0; i < ARRAY_SIZE( CARTOON_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( CARTOON_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_CARTOON;
							pEqipRenderParam->fOutLineWide	= 0.0f;							
						}
					}

					// JHKang / 2012.8.29 / 알파 값 처리할 UT_ATTACH_ANIM 악세사리
					for ( int i = 0; i < ARRAY_SIZE( ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->bAlphaBlend = true;
							pEqipRenderParam->bZEnable = true;
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
							pEqipRenderParam->fOutLineWide	= 0.0f;
							pEqipRenderParam->bZWriteEnable = false;
						}
					}

#ifdef ARA_CHARACTER_BASE
					for ( int i = 0; i < ARRAY_SIZE( REAL_COLOR_ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( REAL_COLOR_ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->bAlphaBlend = true;
							pEqipRenderParam->bZEnable = true;
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;
							pEqipRenderParam->bZWriteEnable = true;
							pEqipRenderParam->srcBlend = D3DBLEND_SRCALPHA;
							pEqipRenderParam->destBlend = D3DBLEND_DESTALPHA;
						}
					}
#endif
					

					{					
						bool bAllAlphaBlend = false;
						for( int i=0; i<ARRAY_SIZE( ALL_ALPHA_ACCESSARY_ITEM_ID ); i++ )
						{
							if( ALL_ALPHA_ACCESSARY_ITEM_ID[i] == dwItemID )
							{
								bAllAlphaBlend = true;
								break;
							}
						}

						if( true == bAllAlphaBlend )
						{
							pEqipRenderParam->bAlphaBlend = true;
							SetAlphaObject( true );
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;		
							pEqipRenderParam->srcBlend		= D3DBLEND_SRCALPHA;
							pEqipRenderParam->destBlend		= D3DBLEND_DESTALPHA;
							pEqipRenderParam->cullMode		= D3DCULL_NONE;
							pEqipRenderParam->bZWriteEnable = false;
						}
					}
#ifdef DARK_DRAGON_ORA_RENDERING
					// 대만 홍콩 흑룡 기운 // 일본 흑룡 기운
					if( 60003794 == dwItemID || 60003795 == dwItemID || 70004450 == dwItemID )
					{
						pEqipRenderParam->bAlphaBlend = true;
						SetAlphaObject( true );
						pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
						pEqipRenderParam->srcBlend		= D3DBLEND_SRCALPHA;
						pEqipRenderParam->destBlend		= D3DBLEND_INVSRCALPHA;
						pEqipRenderParam->cullMode		= D3DCULL_NONE;
						pEqipRenderParam->bZWriteEnable = false;
					}
#endif DARK_DRAGON_ORA_RENDERING

#ifdef ICE_POCKET_RENDERING
					// 대만 홍콩 얼음 주머니
					if ( 70007222 == dwItemID || 70007223 == dwItemID || 70007224 == dwItemID )
					{
						pEqipRenderParam->bAlphaBlend = true;
						SetAlphaObject( true );
						pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;						
					}
#endif ICE_POCKET_RENDERING

					SetRenderParamSpecial( pEqipRenderParam );


				}

				if( m_pHyperXSkinAnim[i] != NULL && m_bHyperMode == true && m_bCanHyperMode == true )
				{
					CKTDGXRenderer::RenderParam* pEqipRenderParam = m_pHyperXSkinAnim[i]->GetRenderParam();
					*pEqipRenderParam = *pRenderParam;
					//{{ robobeg : 2008-10-17
					int iEnchantType = GetEnchantType();
					if( iEnchantType != 0 )
						//}} robobeg : 2008-10-17
					{
						m_fTexOffset							+= fElapsedTime * 2.0f;
						pEqipRenderParam->texOffsetStage2.y		= m_fTexOffset;
						float fImpact							= m_EnchantRenderParam.fLightFlowImpact;						
						m_EnchantRenderParam					= *pEqipRenderParam;
						m_EnchantRenderParam.renderType			= CKTDGXRenderer::RT_LIGHT_FLOW_LOCAL_Z;
						m_EnchantRenderParam.fLightFlowWide		= 5.0f;
						//{{ robobeg : 2008-10-17
						if( iEnchantType == 2 )
							//}} robobeg : 2008-10-17
						{
							m_EnchantRenderParam.color				= 0xffff2222;
						}
						else if( iEnchantType == 3 )
						{
							m_EnchantRenderParam.color				= 0xffffea00;
						}
						else
						{
							m_EnchantRenderParam.color				= 0xffffcccc;
						}
						m_EnchantRenderParam.fLightFlowImpact	= fImpact - fElapsedTime * 200.0f;
						m_EnchantRenderParam.bAlphaBlend		= true;
						m_EnchantRenderParam.srcBlend			= D3DBLEND_SRCALPHA;
						m_EnchantRenderParam.destBlend			= D3DBLEND_DESTALPHA;
						if( m_EnchantRenderParam.fLightFlowImpact < -300.0f )
							m_EnchantRenderParam.fLightFlowImpact = 300.0f;
					}
#ifdef JIGGLEBONE_TEST
					for( int i=0; i<ARRAY_SIZE( ITEM_ID_JIGGLE_TYPE1 ); i++ )
					{
						if( ITEM_ID_JIGGLE_TYPE1[i] == dwItemID )
						{
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
#ifdef UNIT_SCALE_COMBINE_ONE		// 해외팀 오류 수정
							pEqipRenderParam->fOutLineWide		= CARTOON_OUTLINE_WIDTH;
#else //UNIT_SCALE_COMBINE_ONE
							pEqipRenderParam->fOutLineWide		= 1.5f;
#endif //UNIT_SCALE_COMBINE_ONE
						}
					}
#endif

					//{{ kimhc // 2010-01-12 // 외곽선 없앨 UT_ATTACH_ANIM 악세사리
					for ( int i = 0; i < ARRAY_SIZE( REAL_COLOR_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( REAL_COLOR_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;
						}
					}
					//}} kimhc // 2010-01-12 // 외곽선 없앨 UT_ATTACH_ANIM 악세사리

					for ( int i = 0; i < ARRAY_SIZE( CARTOON_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( CARTOON_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_CARTOON;
							pEqipRenderParam->fOutLineWide	= 0.0f;
						}
					}

					// JHKang / 2012.8.29 / 알파 값 처리할 UT_ATTACH_ANIM 악세사리
					for ( int i = 0; i < ARRAY_SIZE( ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->bAlphaBlend = true;
							pEqipRenderParam->bZEnable = true;
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
							pEqipRenderParam->fOutLineWide	= 0.0f;
							pEqipRenderParam->bZWriteEnable = false;
						}
					}

#ifdef ARA_CHARACTER_BASE
					for ( int i = 0; i < ARRAY_SIZE( REAL_COLOR_ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( REAL_COLOR_ALPHA_BLEND_ATTACH_ANIM_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							pEqipRenderParam->bAlphaBlend = true;
							pEqipRenderParam->bZEnable = true;
							pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;
							pEqipRenderParam->bZWriteEnable = true;
							pEqipRenderParam->srcBlend = D3DBLEND_SRCALPHA;
							pEqipRenderParam->destBlend = D3DBLEND_DESTALPHA;
						}
					}
#endif

					{					
						bool bAllAlphaBlend = false;
						for( int i=0; i<ARRAY_SIZE( ALL_ALPHA_ACCESSARY_ITEM_ID ); i++ )
						{
							if( ALL_ALPHA_ACCESSARY_ITEM_ID[i] == dwItemID )
							{
								bAllAlphaBlend = true;
								break;
							}
						}

						if( true == bAllAlphaBlend )
						{
							pEqipRenderParam->bAlphaBlend = true;
							SetAlphaObject( true );
							pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
							pEqipRenderParam->fOutLineWide	= 0.0f;		
							pEqipRenderParam->srcBlend		= D3DBLEND_SRCALPHA;
							pEqipRenderParam->destBlend		= D3DBLEND_DESTALPHA;
							pEqipRenderParam->cullMode		= D3DCULL_NONE;
							pEqipRenderParam->bZWriteEnable = false;
						}
					}
#ifdef DARK_DRAGON_ORA_RENDERING
					// 대만 홍콩 흑룡 기운 // 일본 흑룡기운
					if( 60003794 == dwItemID || 60003795 == dwItemID || 70004450 == dwItemID )
					{
						pEqipRenderParam->bAlphaBlend = true;
						SetAlphaObject( true );
						pEqipRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
						pEqipRenderParam->srcBlend		= D3DBLEND_SRCALPHA;
						pEqipRenderParam->destBlend	= D3DBLEND_INVSRCALPHA;
						pEqipRenderParam->cullMode		= D3DCULL_NONE;
						pEqipRenderParam->bZWriteEnable = false;
					}
#endif DARK_DRAGON_ORA_RENDERING

#ifdef ICE_POCKET_RENDERING
					// 대만 홍콩 얼음 주머니
					if ( 70007222 == dwItemID || 70007223 == dwItemID || 70007224 == dwItemID )
					{
						pEqipRenderParam->bAlphaBlend = true;
						SetAlphaObject( true );
						pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
					}
#endif ICE_POCKET_RENDERING

					SetRenderParamSpecial( pEqipRenderParam );

				}
			}
			break;

		case CX2Item::UT_ATTACH_NORMAL:
			{
				m_AttatchNormalRenderParam = *pRenderParam;

				// 외곽선 없는 카툰처리
				{
					for ( int i = 0; i < ARRAY_SIZE( CARTOON_ACCESSARY_ITEM_ID ); ++i )
					{
						if ( CARTOON_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							m_AttatchNormalRenderParam.renderType = CKTDGXRenderer::RT_CARTOON;
							m_AttatchNormalRenderParam.fOutLineWide= 0.0f;							
						}
					}
				}				

				{
					bool bAlphaBlend = false;
					for( int i=0; i<ARRAY_SIZE( ALPHA_BLEND_ACCESSARY_ITEM_ID ); i++ )
					{
						if( ALPHA_BLEND_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							bAlphaBlend = true;
							break;
						}
					}

					if( true == bAlphaBlend )
					{
						// 우선은 static mesh 악세서리 알파는 여기서 임시로 먹이자... 브라운 썬글라스, etc.
						m_AttatchNormalRenderParam.bAlphaBlend = true;
						//{{ robobeg : 2008-10-21
						SetAlphaObject( true );
						//}} robobeg : 2008-10-21
					}
				}




				{
					// 외곽선 없는 알파 오브젝트 임시로 하드코딩
					bool bAlphaBlend = false;
					for( int i=0; i<ARRAY_SIZE( ALPHA_BLEND_REAL_COLOR_ACCESSARY_ITEM_ID ); i++ )
					{
						if( ALPHA_BLEND_REAL_COLOR_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							bAlphaBlend = true;
							break;
						}
					}

					if( true == bAlphaBlend )
					{
						m_AttatchNormalRenderParam.bAlphaBlend = true;
						SetAlphaObject( true );


						m_AttatchNormalRenderParam.renderType = CKTDGXRenderer::RT_REAL_COLOR;
					}
				}




				
				{
					// dmlee, 2009-02-25, note!! 아래의 코드는 아크엔젤 서클렛 아이템을 위해 임시로 추가된 코드, 나중에 스크립트로 옮기거나 해야됨
					bool bDestAlphaBlend = false;
					for( int i=0; i<ARRAY_SIZE( DEST_ALPHA_BLEND_ACCESSARY_ITEM_ID ); i++ )
					{
						if( DEST_ALPHA_BLEND_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							bDestAlphaBlend = true;
							break;
						}
					}

					if( true == bDestAlphaBlend )
					{
						m_AttatchNormalRenderParam.bAlphaBlend = true;
						SetAlphaObject( true );
						m_AttatchNormalRenderParam.srcBlend		= D3DBLEND_SRCALPHA;
						m_AttatchNormalRenderParam.destBlend	= D3DBLEND_DESTALPHA;
						m_AttatchNormalRenderParam.cullMode		= D3DCULL_NONE;
					}
				}

				{					
					bool bAllAlphaBlend = false;
					for( int i=0; i<ARRAY_SIZE( ALL_ALPHA_ACCESSARY_ITEM_ID ); i++ )
					{
						if( ALL_ALPHA_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							bAllAlphaBlend = true;
							break;
						}
					}

					if( true == bAllAlphaBlend )
					{
						m_AttatchNormalRenderParam.bAlphaBlend = true;
						SetAlphaObject( true );
						m_AttatchNormalRenderParam.renderType	= CKTDGXRenderer::RT_REAL_COLOR;
						m_AttatchNormalRenderParam.srcBlend		= D3DBLEND_SRCALPHA;
						m_AttatchNormalRenderParam.destBlend	= D3DBLEND_DESTALPHA;
						m_AttatchNormalRenderParam.cullMode		= D3DCULL_NONE;
						m_AttatchNormalRenderParam.bZWriteEnable = false;
						
					}
				}

				{					
					bool bAlphaCullNone = false;
					for( int i=0; i<ARRAY_SIZE( ALPHA_REAL_COLOR_CULL_NONE_ACCESSARY_ITEM_ID ); i++ )
					{
						if( ALPHA_REAL_COLOR_CULL_NONE_ACCESSARY_ITEM_ID[i] == dwItemID )
						{
							bAlphaCullNone = true;
							break;
						}
					}

					if( true == bAlphaCullNone )
					{
						m_AttatchNormalRenderParam.bAlphaBlend	= true;
						SetAlphaObject( true );
						m_AttatchNormalRenderParam.cullMode		= D3DCULL_NONE;
						m_AttatchNormalRenderParam.renderType	= CKTDGXRenderer::RT_REAL_COLOR;

					}
				}
				
				CKTDGXRenderer::RenderParam* pEqipRenderParam = pRenderParam;

				if ( NULL != pEqipRenderParam )
					SetRenderParamSpecial( pEqipRenderParam );

			} break;

		} // switch
	} // for


	return S_OK;
}//CX2Eqip::SetRenderParam()





void CX2Eqip::SetRenderParamSpecial( CKTDGXRenderer::RenderParam* pEqipRenderParam )
{
	// SF무기 예외로 외곽선 없는 카툰으로 그리기
    switch( m_pItemTemplet->GetItemID() )
	{
	case 130892:
	case 130895:
		{
			pEqipRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
		} break;	

	case 215670:
	case 215700:
	case 215760:
	case 238130:
#ifdef CARTOON_SPECIAL_RENDER_ITEM_WITH_ALPHA	
	case 70000510:	// 임규수 일본 추가 와일드 더블소드
	case 70000520:	// 임규수 일본 추가 와일드 더블소드
#endif //CARTOON_SPECIAL_RENDER_ITEM_WITH_ALPHA	
		{
			pEqipRenderParam->bAlphaBlend = true;
			SetAlphaObject( true );

			pEqipRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
		} break;
	case 232170:
	case 232180:
	case 81086:
	case 81087:
	case 81088:
	case 81089:
	case 81090:
	case 81091:
	case 81161:
	case 81162:
	case 81163:
	case 81164:
	case 81165:
	case 81166:
	case 180143:
	case 81226:
	case 81236:
	case 81246:
	case 81256:
	case 81266:
	case 81276:
#ifdef REAL_COLOR_SPECIAL_RENDER_ITEM_WITH_ALPHA
	case 60005390:
	case 60005393:
	case 84001760:
	case 90002340:	// 우정의 날개
#endif //REAL_COLOR_SPECIAL_RENDER_ITEM_WITH_ALPHA
	case 88396: /// 쉐도우 인큐버스 윙츠 ( 엘소드 )
	case 88397: /// 그레이스 페어리 윙츠 ( 아이샤 )
	case 88398: /// 그레이스 페어리 윙츠 ( 레나 )
	case 88399: /// 쉐도우 인큐버스 윙츠 ( 레이븐 )
	case 88400: /// 그레이스 페어리 윙츠 ( 이브 )
	case 88401: /// 쉐도우 인큐버스 윙츠 ( 청 )
	case 81986: /// 그레이스 페어리 라이트 윙(아라)
	case 81987:	/// 그레이스 페어리 윙츠 (아라)
	case 81955: /// 금강야차 화무일홍 망토 (아라)
	case 81962: /// 금강 천야차 염령환 망토 (아라)
	case 82003: /// 금강야차 화무일홍 망토 (엘리시스)
	case 82010: /// 금강 천야차 염령환 망토 (엘리시스)
	case 82075:	/// 아라 이블 트레이서
		{
		pEqipRenderParam->bAlphaBlend = true;
		SetAlphaObject( true );

		pEqipRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
		} break;
	}
}


HRESULT CX2Eqip::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

//{{ robobeg : 2008-10-21
	if( m_pItemTemplet == NULL )
		return E_FAIL;

    if ( !GetShowObject() )
        return E_FAIL;

//}} robobeg : 2008-10-21



	switch( m_EqipType )
	{
	case CX2Item::UT_SKIN:
		{
			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
			{
				if( NULL != m_pNormalXSkinModel[i] )
				{
                    switch( m_pItemTemplet->GetItemID() )
					{
					case 202560:
					case 202610:
						{
							m_pNormalXSkinModel[i]->SetAlphaRender(true);
						} break;
#ifdef FACE_OFF_MONSTER_HEAD_TEST
						// UnitTemplet.lua에 등록된 캐릭별 BasicFace 등록 
					case 2:
					case 9:
					case 16:
					case 29:
					case 36:
					case 44:
					case 45:
					case 50:
					case 51:
					case 57:
					case 41400:
						{
							m_pNormalXSkinModel[i]->SetNoRenderable(true);
						} break;
#endif FACE_OFF_MONSTER_HEAD_TEST

#ifdef ARA_CHARACTER_BASE
					case 41510:
					case 41640:
					case 41700:
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
					case 41736:
					case 41743:
#endif // SERV_ARA_CHANGE_CLASS_SECOND
						{
							m_pNormalXSkinModel[i]->SetUseRenderParam( true );
						} break;
#endif
					default:
						{
							m_pNormalXSkinModel[i]->SetAlphaRender(false);
						} break;
					}
				}

				if( NULL != m_pHyperXSkinModel[i] && m_bHyperMode == true && m_bCanHyperMode == true )
				{
                    switch( m_pItemTemplet->GetItemID() )
					{
#ifdef ARA_CHARACTER_BASE
					case 41510:
					case 41640:
					case 41700:
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
					case 41736:
					case 41743:
#endif // SERV_ARA_CHANGE_CLASS_SECOND
						{
							m_pHyperXSkinModel[i]->SetUseRenderParam( true );
						} break;
#endif

					case 202560:
					case 202610:
						{
							m_pHyperXSkinModel[i]->SetAlphaRender(true);
						} break;

					default:
						{
							m_pHyperXSkinModel[i]->SetAlphaRender(false);
						} break;
					}
				}
			}

#ifdef ATTACH_SKINMESH
			if( false == m_bLoadedAttached )
			{
				LoadAttached();
			}
#endif
		} break;

	case CX2Item::UT_ATTACH_ANIM:
		{
			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
			{
				if( NULL != m_pUnitMatrix[i] )
				{
					if( NULL != m_pNormalXSkinAnim[i] )
					{
						CKTDGXRenderer::RenderParam* pEqipRenderParam = m_pNormalXSkinAnim[i]->GetRenderParam();
						D3DXMATRIX localMove, matTrans, matRot, matScale;

						CX2Unit::UNIT_TYPE unitType = m_pOwnerUnit->GetType();
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                        const D3DXVECTOR3& vScale = m_pItemTemplet->GetRenderScale(unitType);
                        const D3DXVECTOR3& vRotate = m_pItemTemplet->GetRenderRotate(unitType);
						D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z );
						D3DXMatrixRotationYawPitchRoll( &matRot, vRotate.y, vRotate.x, vRotate.z );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
						D3DXMatrixScaling(&matScale, m_pItemTemplet->m_RanderScale[unitType].x, m_pItemTemplet->m_RanderScale[unitType].y, m_pItemTemplet->m_RanderScale[unitType].z );
						D3DXMatrixRotationYawPitchRoll( &matRot, m_pItemTemplet->m_RanderRotate[unitType].y, m_pItemTemplet->m_RanderRotate[unitType].x, m_pItemTemplet->m_RanderRotate[unitType].z );
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

						D3DXMatrixTranslation( &matTrans, m_Offset.x, m_Offset.y, m_Offset.z );

						D3DXMatrixMultiply( &localMove, &matScale, &matRot );
						D3DXMatrixMultiply( &localMove, &localMove, &matTrans );

						//{{ robobeg : 2008-10-24
						D3DXMATRIX  mWorld;
						D3DXMatrixMultiply( &mWorld, &localMove, m_pUnitMatrix[i] );
						m_pNormalXSkinAnim[i]->SetDXMatrix( mWorld );
						//}} robobeg : 2008-10-24

						m_pNormalXSkinAnim[i]->OnFrameMove( fTime, fElapsedTime );

					}


					if( NULL != m_pHyperXSkinAnim[i] && m_bHyperMode == true && m_bCanHyperMode == true )
					{
						CKTDGXRenderer::RenderParam* pEqipRenderParam = m_pHyperXSkinAnim[i]->GetRenderParam();
						D3DXMATRIX localMove, matTrans, matRot, matScale;

						CX2Unit::UNIT_TYPE unitType = m_pOwnerUnit->GetType();
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                        const D3DXVECTOR3& vScale = m_pItemTemplet->GetRenderScale(unitType);
                        const D3DXVECTOR3& vRotate = m_pItemTemplet->GetRenderRotate(unitType);
						D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z );
						D3DXMatrixRotationYawPitchRoll( &matRot, vRotate.y, vRotate.x, vRotate.z );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                        D3DXMatrixScaling(&matScale, m_pItemTemplet->m_RanderScale[unitType].x, m_pItemTemplet->m_RanderScale[unitType].y, m_pItemTemplet->m_RanderScale[unitType].z );
                        D3DXMatrixRotationYawPitchRoll( &matRot, m_pItemTemplet->m_RanderRotate[unitType].y, m_pItemTemplet->m_RanderRotate[unitType].x, m_pItemTemplet->m_RanderRotate[unitType].z );
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
						D3DXMatrixTranslation( &matTrans, m_Offset.x, m_Offset.y, m_Offset.z );

						D3DXMatrixMultiply( &localMove, &matScale, &matRot );
						D3DXMatrixMultiply( &localMove, &localMove, &matTrans );

						//{{ robobeg : 2008-10-24
						D3DXMATRIX  mWorld;
						D3DXMatrixMultiply( &mWorld, &localMove, m_pUnitMatrix[i] );
						m_pHyperXSkinAnim[i]->SetDXMatrix( mWorld );
						//}} robobeg : 2008-10-24
						m_pHyperXSkinAnim[i]->OnFrameMove( fTime, fElapsedTime );
					}
				}
			}



			if( false == m_bLoadedAttached )
			{
				LoadAttached();
			}
			

		} break;

	case CX2Item::UT_ATTACH_NORMAL:
		{
			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
			{
				if( NULL != m_pUnitMatrix[i] )
				{
					D3DXMATRIX localMove, matTrans, matRot, matScale;

					CX2Unit::UNIT_TYPE unitType = m_pOwnerUnit->GetType();
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    const D3DXVECTOR3& vScale = m_pItemTemplet->GetRenderScale(unitType);
                    const D3DXVECTOR3& vRotate = m_pItemTemplet->GetRenderRotate(unitType);
					D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z );
					D3DXMatrixRotationYawPitchRoll( &matRot, vRotate.y, vRotate.x, vRotate.z );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					D3DXMatrixScaling(&matScale, m_pItemTemplet->m_RanderScale[unitType].x, m_pItemTemplet->m_RanderScale[unitType].y, m_pItemTemplet->m_RanderScale[unitType].z );
					D3DXMatrixRotationYawPitchRoll( &matRot, m_pItemTemplet->m_RanderRotate[unitType].y, m_pItemTemplet->m_RanderRotate[unitType].x, m_pItemTemplet->m_RanderRotate[unitType].z );
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					D3DXMatrixTranslation( &matTrans, m_Offset.x, m_Offset.y, m_Offset.z );

					D3DXMatrixMultiply( &localMove, &matScale, &matRot );
					D3DXMatrixMultiply( &localMove, &localMove, &matTrans );

					//{{ robobeg : 2008-10-24
					D3DXMatrixMultiply( &m_mAttachNormalMatrix[i], &localMove, m_pUnitMatrix[i] );
					//}} robobeg : 2008-10-24
				}
			}
		} break;
	}



#ifdef ATTACH_SKINMESH
	for(UINT i=0; i<m_vecpSubEquip.size(); ++i)
	{
		CX2SubEquip *pSubEquip = m_vecpSubEquip[i];
		if( pSubEquip != NULL )
		{
			pSubEquip->OnFrameMove(fTime, fElapsedTime);
		}
	}
#endif

#ifdef EQUIP_ATTACHED_PARTICLE_TEST
	m_AttachedParticleGroup.OnFrameMove( fTime, fElapsedTime );
#endif EQUIP_ATTACHED_PARTICLE_TEST


	return S_OK;
}

//}} robobeg : 2008-10-17


//{{ robobeg : 2008-10-17
/*virtual*/
RENDER_HINT   CX2Eqip::OnFrameRender_Prepare()
{
    if ( m_pItemTemplet == NULL )
        return RENDER_HINT_NORENDER;

#ifdef WORLD_TOOL
	if( !GetShowObject() ) 
		return RENDER_HINT_NORENDER;
#endif

	// dmlee 2008.11.29 note!! eqip하나에 여러개의 메시가 들어가 있어도 render_hint는 첫번째 메시를 기준으로 하는 것으로 했음.

    switch( m_EqipType )
    {
    case CX2Item::UT_ATTACH_ANIM:
        {
            RENDER_HINT renderHintResult = RENDER_HINT_NORENDER;
            RENDER_HINT renderHint;
	        for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	        {
                if ( m_bHyperMode == false || m_bCanHyperMode == false )
                {
                    if ( m_pNormalXSkinAnim[i] != NULL )
                    {
                        m_pNormalXSkinAnim[i]->SetInstantShow( false );
                        if ( !m_pNormalXSkinAnim[i]->GetShowObject() )
                            continue;
                        renderHint = m_pNormalXSkinAnim[i]->OnFrameRender_Prepare();
                        if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
                        {
                            m_pNormalXSkinAnim[i]->SetInstantShow( true );
                            if ( GetEnchantType() == 0 )
                                renderHintResult.MergeRenderHint( renderHint );
                            else
                                renderHintResult = RENDER_HINT_DEFAULT;
                        }//if
                    }//if
                }
                else
                {
                    if ( m_pHyperXSkinAnim[i] != NULL )
                    {
                        m_pHyperXSkinAnim[i]->SetInstantShow( false );
                        if ( !m_pHyperXSkinAnim[i]->GetShowObject() )
                            continue;
                        renderHint = m_pHyperXSkinAnim[i]->OnFrameRender_Prepare();
                        if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
                        {
                            m_pHyperXSkinAnim[i]->SetInstantShow( true );
                            if ( GetEnchantType() == 0 )
                                renderHintResult.MergeRenderHint( renderHint );
                            else
                                renderHintResult = RENDER_HINT_DEFAULT;
                        }//if
                    }//if
                }//if.. else..
            }//for
            return renderHintResult;
        }
    case CX2Item::UT_ATTACH_NORMAL:
        for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	    {
            if( m_bHyperMode == false || m_bCanHyperMode == false )
		    {
                if ( m_pNormalXMesh[i] != NULL )
                    return RENDER_HINT_XMESH( m_AttatchNormalRenderParam.renderType
                    , m_AttatchNormalRenderParam.bFog && m_AttatchNormalRenderParam.bFogShow );
            }
            else
            {
			    if ( m_pHyperXMesh[i] != NULL )
                    return RENDER_HINT_XMESH( m_AttatchNormalRenderParam.renderType
                    , m_AttatchNormalRenderParam.bFog && m_AttatchNormalRenderParam.bFogShow );
            }//if.. else..
        }//for
        break;
#ifdef ATTACH_SKINMESH
	case CX2Item::UT_SKIN:
		{			
			for(UINT i=0; i<m_vecpSubEquip.size(); ++i)
			{
				CX2SubEquip *pSubEquip = m_vecpSubEquip[i];
				if( pSubEquip != NULL && pSubEquip->GetIsSkinMesh() == true )
				{
					return pSubEquip->OnFrameRender_Prepare();
				}
			}			
		}
		break;
#endif
    }//switch

    return RENDER_HINT_NORENDER;
}//CX2Eqip::OnFrameRender_Prepare()



/*virtual*/
void    CX2Eqip::OnFrameRender_Draw()
{
	KTDXPROFILE();

//{{ robobeg : 2008-10-17

	// 렌더링되지 않는 경우는 OnFrameRender_Prepare() 에서 막아준다.

//}} robobeg : 2008-10-17

    DWORD   dwItemID = m_pItemTemplet->GetItemID();

	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	{
		// oasis907 : 김상윤 [2011.8.30] 빅터의 고사리 주먹 악세 - 레이븐 왼팔에는 붙지 않게 하드 코딩
		if( m_pOwnerUnit != NULL &&  
			m_pOwnerUnit->GetType() == CX2Unit::UT_RAVEN &&
			i == 1 &&
			IsNoRenderNasodHandPos( dwItemID ) )
		{
			continue;
		}
		if( m_pOwnerUnit != NULL &&  
			m_pOwnerUnit->GetType() == CX2Unit::UT_RAVEN &&
			i == 0 &&
			IsNoRenderNasodArmPos( dwItemID ) )
		{
			continue;
		}

		switch( m_EqipType )
		{
		case CX2Item::UT_ATTACH_ANIM:
			{
				if( m_bHyperMode == false || m_bCanHyperMode == false )
				{
					if ( m_pNormalXSkinAnim[i] != NULL && m_pNormalXSkinAnim[i]->GetShowObject()
                        && m_pNormalXSkinAnim[i]->IsInstantShow() )
					{
						m_pNormalXSkinAnim[i]->OnFrameRender_Draw();
						//{{ robobeg : 2008-10-17
						if( GetEnchantType() != 0 )
							//}} robobeg : 2008-10-17
						{
							CKTDGXRenderer::RenderParam* pRenderParam = m_pNormalXSkinAnim[i]->GetRenderParam();
							CKTDGXRenderer::RenderParam tempRenderParam = *pRenderParam;

							*pRenderParam = m_EnchantRenderParam;
							m_pNormalXSkinAnim[i]->OnFrameRender_Draw();
							*pRenderParam = tempRenderParam;
						}					
					}
				}
				else
				{
					if ( m_pHyperXSkinAnim[i] != NULL && m_pHyperXSkinAnim[i]->GetShowObject()
                        && m_pHyperXSkinAnim[i]->IsInstantShow() )
					{
						m_pHyperXSkinAnim[i]->OnFrameRender_Draw();
						//{{ robobeg : 2008-10-17
						if( GetEnchantType() != 0 )
							//}} robobeg : 2008-10-17
						{
							CKTDGXRenderer::RenderParam* pRenderParam = m_pHyperXSkinAnim[i]->GetRenderParam();
							CKTDGXRenderer::RenderParam tempRenderParam = *pRenderParam;

							*pRenderParam = m_EnchantRenderParam;
							m_pHyperXSkinAnim[i]->OnFrameRender_Draw();
							*pRenderParam = tempRenderParam;
						}
					}
				}			

			} break;

		case CX2Item::UT_ATTACH_NORMAL:
			{
				//{{ robobeg : 2008-10-15
				//CKTDGXRenderer::RenderParam* pRenderParam = g_pKTDXApp->GetDGManager()->GetXRenderer()->GetRenderParam();
				//*pRenderParam = m_AttatchNormalRenderParam;
				//}} robobeg : 2008-10-15

				if( m_bHyperMode == false || m_bCanHyperMode == false )
				{
					if ( m_pNormalXMesh[i] != NULL )
					{
						//{{ robobeg : 2008-10-08
						//if ( SUCCEEDED( g_pKTDXApp->GetDGManager()->GetXRenderer()->BeginRender() ) )
						//                  {
						g_pKTDXApp->GetDGManager()->GetXRenderer()->OnFrameRender( m_AttatchNormalRenderParam, m_mAttachNormalMatrix[i], 
							*m_pNormalXMesh[i], m_pNormalTexChangeXET, NULL, m_pNormalAniData );
						//g_pKTDXApp->GetDGManager()->GetXRenderer()->EndRender();
						//}//if
						//}} robobeg : 2008-10-08
					}
				}
				else
				{
					if ( m_pHyperXMesh[i] != NULL )
					{
						//{{ robobeg : 2008-10-08
						//if ( SUCCEEDED( g_pKTDXApp->GetDGManager()->GetXRenderer()->BeginRender() ) )
						//{
						g_pKTDXApp->GetDGManager()->GetXRenderer()->OnFrameRender( m_AttatchNormalRenderParam, m_mAttachNormalMatrix[i], 
							*m_pHyperXMesh[i], m_pNormalTexChangeXET, NULL, m_pNormalAniData );
						//g_pKTDXApp->GetDGManager()->GetXRenderer()->EndRender();
						//              }//if
						//}} robobeg : 2008-10-08
					}
				}

			} break;

#ifdef ATTACH_SKINMESH
		case CX2Item::UT_SKIN:
			{
				for(vector<CX2SubEquip*>::iterator it = m_vecpSubEquip.begin(); it<m_vecpSubEquip.end(); ++it)
				{
					CX2SubEquip* pSubEquip = *it;

					/// 유닛 뷰어에서 드로우 되지 않는 문제 수정을 위해, 조건 해제 
					/// ( Item의 ATTATCHED_MESH 기능 중 y 파일 사용을 위한 예외 처리, 해당 함수 안에서 스킨 매시 여부를 검사하여 설정해 주고 있다. )
					//if( pSubEquip->GetIsSkinMesh() == true )
						pSubEquip->OnFrameRender_Draw();
				}
			}			
			break;
#endif
		} // switch
	} // for

	if(m_bFixed)
	{
#ifdef SUB_EQUIP_TEST
		for(vector<CX2SubEquip*>::iterator it = m_vecpSubEquip.begin(); it<m_vecpSubEquip.end(); ++it)
		{
			CX2SubEquip* pSubEquip = *it;
			pSubEquip->OnFrameRender_Draw();
		}
#endif SUB_EQUIP_TEST


#ifdef EQUIP_ATTACHED_PARTICLE_TEST
		for(vector<AttachedParticle*>::iterator itParticle = m_AttachedParticleGroup.m_vecpAttachedParticle.begin();
			itParticle < m_AttachedParticleGroup.m_vecpAttachedParticle.end();
			++itParticle)
		{
			AttachedParticle* pAttachedParticle = *itParticle;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
			
			if( true == pAttachedParticle->m_bIsMajorParticle )
			{
				pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}
			else
			{
				pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}

			if( NULL != pSeq )
			{
				CKTDGStateManager::PushStates( pSeq->GetRenderStateID() );
				pSeq->OnFrameRender_Draw();
			}
			
		}
#endif
	}
}

//}} robobeg : 2008-10-17


//{{ seojt // 2008-12-13, 19:01
// /*virtual*/ bool CX2Eqip::OnThreadLoad(DWORD dwParam_, DWORD dwParam2_)
// {
//     //CKTDGXSkinAnimPtr   m_pUnitXSkinAnim = m_ptrUnitXSkinAnim.lock();
//     //ASSERT( m_pUnitXSkinAnim != NULL );
//     //if( m_pUnitXSkinAnim == NULL )
//     //    return false;
// 
//     if( dwParam_ == CKTDXDeviceManager::THREAD_REQUEST_EQUIP_NORMAL )
//     {
//         const int iArrayIndex = (int)dwParam2_;
//         ASSERT( iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName) );
//         if( !(iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName)) )
//             return true; // 범위를 벗어난 index의 노드에 대해서 retry하지 않도록 true를 리턴한다.
// 
//         const int& i = iArrayIndex;
// 
//         ASSERT( sizeof(m_pNormalXMesh[i]) == sizeof(int) );
//         m_pNormalXMesh[i] = g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelName[i] );
// 
// 	    if( m_bCanHyperMode == true )
// 	    {
//             m_pHyperXMesh[i] = g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelNameHyper[i] );
// 	    }//if
//     	
// 
// 		if( false == m_pItemTemplet->m_AttachFrameName[i].empty() )
// 		{
//             CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
// 			pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->m_AttachFrameName[i].c_str() );
// 			if( NULL != pFrame )
// 			{
// 				m_pUnitMatrix[i] = &pFrame->combineMatrix;
// 			}//if
// 		}//if
//         return true;
//     }
//     else if( dwParam_ == CKTDXDeviceManager::THREAD_REQUEST_EQUIP_SKIN_NORMAL )
//     {
//         const int iArrayIndex = (int)dwParam2_;
//         ASSERT( iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName) );
//         if( !(iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName)) )
//             return true; // 범위를 벗어난 index의 노드에 대해서 retry하지 않도록 true를 리턴한다.
// 
//         ASSERT( sizeof(m_pNormalXSkinModel[iArrayIndex]) == sizeof(int) );
//         m_pNormalXSkinModel[iArrayIndex] = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[iArrayIndex] );
// 	    if( m_bCanHyperMode == true )
// 	    {
//             m_pHyperXSkinModel[iArrayIndex] = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[iArrayIndex] );
// 	    }//if
// 
// 	    if ( iArrayIndex == 0 && m_pNormalXSkinModel[iArrayIndex] == NULL )
// 	    {
// 		    wstring errorMSG;
// 		    errorMSG = L"- 모델 이름 : ";
// 		    errorMSG += m_ModelName[iArrayIndex];
// 
// 		    ErrorLogMsg( XEM_ERROR93, errorMSG.c_str() );
// 		    ASSERT( !"CX2Eqip::InitSkin, null normal skin model" );
// 
// 		    if ( m_bCanHyperMode == true && m_pHyperXSkinModel[iArrayIndex] == NULL )
// 		    {
// 			    wstring errorMSG2;
// 			    errorMSG2 = L"- 모델 이름 : ";
// 			    errorMSG2 += m_ModelNameHyper[iArrayIndex];
// 
// 			    ErrorLogMsg( XEM_ERROR93, errorMSG2.c_str() );
// 		    }//if
// 	    }//if
// 
// 		//{{ dmlee 2009.3.19 메시가 있는 경우에만 update 하도록
// 		if( NULL != m_pNormalXSkinModel[iArrayIndex] || 
// 			NULL != m_pHyperXSkinModel[iArrayIndex] )
// 		//}} dmlee 2009.3.19 메시가 있는 경우에만 update 하도록		
// 		{
// 			//{{ dmlee 2009.3.20 캐시템과 ED템을 동시에 장착하고 있는 경우에 ED템은 _updateSkin을 호출하지 않도록
// 			_UpdateSkin( iArrayIndex );
// 			//if( -1 != m_OwnerUnitUID &&
// 			//	NULL != m_pItemTemplet )
// 			//{
// 			//	if( true == m_pItemTemplet->GetFashion() )
// 			//	{
// 			//		_UpdateSkin( iArrayIndex );
// 			//	}
// 			//	else
// 			//	{
// 			//		if( NULL != g_pTFieldGame )
// 			//		{
// 			//			CX2SquareUnit* pFieldUnit = g_pTFieldGame->GetSquareUnitByUID( m_OwnerUnitUID );
// 			//			if( NULL != pFieldUnit &&
// 			//				pFieldUnit->GetUnitViewer() )
// 			//			{
// 			//				if( false == pFieldUnit->GetUnitViewer()->IsFashionItemEquippedAt( m_pItemTemplet->GetEqipPosition() ) )
// 			//				{
// 			//					_UpdateSkin( iArrayIndex );
// 			//				}
// 			//			}
// 			//		}
// 			//		else if( NULL != g_pSquareGame )
// 			//		{
// 			//			CX2SquareUnit* pSquareUnit = g_pSquareGame->GetSquareUnitByUID( m_OwnerUnitUID );
// 			//			if( NULL != pSquareUnit &&
// 			//				pSquareUnit->GetUnitViewer() )
// 			//			{
// 			//				if( false == pSquareUnit->GetUnitViewer()->IsFashionItemEquippedAt( m_pItemTemplet->GetEqipPosition() ) )
// 			//				{
// 			//					_UpdateSkin( iArrayIndex );
// 			//				}
// 			//			}
// 			//		}
// 			//	}
// 			//}
// 			//}} dmlee 2009.3.20 캐시템과 ED템을 동시에 장착하고 있는 경우에 ED템은 _updateSkin을 호출하지 않도록
// 		}
//         return true;
//     }
//     else if( dwParam_ == CKTDXDeviceManager::THREAD_REQUEST_EQUIP_ATTACH_ANIM_NORMAL )
//     {
//         const int iArrayIndex = (int)dwParam2_;
//         ASSERT( iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName) );
//         if( !(iArrayIndex >= 0 && iArrayIndex < ARRAY_SIZE(m_ModelName)) )
//             return true; // 범위를 벗어난 index의 노드에 대해서 retry하지 않도록 true를 리턴한다.
// 
//         const int& i = iArrayIndex;
// 
// #ifdef _DEBUG // 2008-12-14
//         //std::string strFileName;
//         //ConvertWCHARToChar( strFileName, m_ModelName[i].c_str() );
//         //BREAK( 0 == stricmp("Mesh_Raven_Base_Weapon_Sorted.x",strFileName.c_str()) );
// #endif // _DEBUG // 2008-12-14
// 
//         ASSERT( sizeof(m_pNormalXSkinModel[i]) == sizeof(int) );
//         m_pNormalXSkinModel[i] = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );
// 		ASSERT( NULL != m_pNormalXSkinModel[i] );
// 		if( NULL == m_pNormalXSkinModel[i] )
// 		{
// 			return true;
// 		}//if
// 		
// 		CKTDGXSkinAnim* pSkinAnim = NULL;
// 
//         ASSERT( sizeof(CKTDGXSkinAnim*) == sizeof(int) );
// 		pSkinAnim = CKTDGXSkinAnim::CreateSkinAnim();
// 		pSkinAnim->SetAnimXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET );
// 		pSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET );
// 		pSkinAnim->ChangeAnim( m_AniName.c_str(), false );
// 		pSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
// 		pSkinAnim->UseDXMatrix( true );
// 		m_pNormalXSkinAnim[i] = pSkinAnim;
// 
// 	    if( m_bCanHyperMode == true )
// 	    {
// 			m_pHyperXSkinModel[i]		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
// 
// 			ASSERT( NULL != m_pHyperXSkinModel[i] );
// 			if( NULL == m_pHyperXSkinModel[i] )
// 			{
// 				return true;
// 			}//if
// 
// 			pSkinAnim = CKTDGXSkinAnim::CreateSkinAnim();
// 			pSkinAnim->SetAnimXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET );
// 			pSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pHyperMultiTexXET, m_pHyperTexChangeXET );
// 			pSkinAnim->ChangeAnim( m_AniNameHyper.c_str(), false );
// 			pSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
// 			pSkinAnim->UseDXMatrix( true );
// 			m_pHyperXSkinAnim[i] = pSkinAnim;
// 	    }//if
// 
// 		if( false == m_pItemTemplet->m_AttachFrameName[i].empty() )
// 		{
// 	        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
// 			pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->m_AttachFrameName[i].c_str() );
// 			if( NULL != pFrame )
// 			{
// 				m_pUnitMatrix[i] = &pFrame->combineMatrix;
// 			}//if
// 		}//if
// 
//         return true;
//     }//if.. else..
// 
//     ASSERT( !"이 경우는 발생하지 않아야 한다. - jintaeks on 2008-12-13, 18:26" );
//     return false;
// }//CX2Eqip::OnThreadLoad()
// //}} seojt // 2008-12-13, 19:01


void CX2Eqip::ChangeHyperMode()
{
    m_bHyperMode = true;

//{{ robobeg : 2008-10-21
	//switch( m_EqipType )
	//{
	//	case CX2Item::ET_SKIN:
	//		m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel );

	//		if( m_pUnitXSkinAnim->IsInModelXSkinMesh( m_pHyperXSkinModel ) == false )
	//			m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel, m_pHyperAniXET, NULL, m_pHyperTexChangeXET );
	//		break;

	//	case CX2Item::ET_ATTACH_ANIM:
	//		break;

	//	case CX2Item::ET_ATTACH_NORMAL:
	//		break;
	//}
//}} robobeg : 2008-10-21
    _UpdateSkin();
}

void CX2Eqip::ChangeNormalMode()
{
	m_bHyperMode = false;

//{{ robobeg : 2008-10-21
	//switch( m_EqipType )
	//{
	//	case CX2Item::ET_SKIN:
	//		m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel );

	//		if( m_pUnitXSkinAnim->IsInModelXSkinMesh( m_pNormalXSkinModel ) == false )
	//			m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel, m_pNormalAniXET, NULL, m_pNormalTexChangeXET );
	//		break;

	//	case CX2Item::ET_ATTACH_ANIM:
	//		break;

	//	case CX2Item::ET_ATTACH_NORMAL:
	//		break;
	//}
//}} robobeg : 2008-10-21
    _UpdateSkin();
}

#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
// June2017 enchant glow: the stage-2 texture of the multi-tex XET by enchant level. Every June site runs the same chain:
// ChangeEnchantLevel 3295f250, the skin XET open 3295ffd0 (InitSkin 32963eb0), the attach-anim init 3295dfa0.
//   >= 13 "13Gang_Protect.tga", 9..12 "11Gang_Protect.tga" (June tests < 11, then < 9), 7..8 "Gate_Protect_G01.tga", < 7 none.
// March (ENCHANT_WEAPON_EFFECT_LEVEL_1 / _2 = 7 / 11, ADD_UPGRADE_WEAPON_PARTICLE on): 7..10 G01, >= 11 11Gang.
static const char* EnchantStage2Texture( int iEnchantLevel )
{
	if( iEnchantLevel >= 13 )
		return "13Gang_Protect.tga";	// 334c9b14; a June-only asset (June data045), june-asset-port P4a brings it
	if( iEnchantLevel >= 9 )
		return "11Gang_Protect.tga";	// 334c9ae4
	if( iEnchantLevel >= 7 )
		return "Gate_Protect_G01.tga";	// 334c9af8
	return NULL;
}

// 31d40160( "*.*" 334c9b10, texture, 7 = D3DTOP_ADD ) at every June site. One "[JUNE_ENGINE] EQSKIN" line per choice: the first 60
// per run, then every 100th; "MISSING" = the texture did not open (no glow).
static void SetEnchantStage2( CKTDXDeviceXET* pXET, int iEnchantLevel, const char* pSite, int iItemID )
{
	const char* pTexture = EnchantStage2Texture( iEnchantLevel );
	if( NULL == pTexture )
		return;
	pXET->SetMultiTexStage2( "*.*", pTexture, D3DTOP_ADD );

	static int s_iEqSkinLines = 0;
	++s_iEqSkinLines;
	if( s_iEqSkinLines <= 60 || 0 == s_iEqSkinLines % 100 )
	{
		const CKTDXDeviceXET::MultiTexureData* pStage2 = pXET->GetMultiTex( 2 );
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] EQSKIN stage2 " << pSite << " item " << iItemID << " level " << iEnchantLevel << " tex " << pTexture
			<< ( ( NULL != pStage2 && NULL != pStage2->pTexture ) ? " loaded" : " MISSING" ) << " line " << s_iEqSkinLines << fileout;
	}
}
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN




// dmlee 2009-03-19 백그라운드 로딩 스레드에서만 호출되어야 한다.
// fix!!! GetShowObject()함수를 호출할때 내부적으로 m_pManagerObject를 참조하는데 dangling pointer가 될 수 있다. 
// 이를 임시로 해결하기 위해서 m_pManagerObject를 참조하지 않도록 하였다. 
// m_pManagerObject를 shared_pointer로 만들거나, 다른 좋은 방법이 필요하다. 
//{{ robobeg : 2008-10-21
void CX2Eqip::_UpdateSkin(int iIndex_)
{
#ifdef HEAVY_LOG_TEST
	
	debugWPrintf( L"[START] _UpdateSkin( %d )", iIndex_ );
	debugWPrintf( L"unit : %s", m_pOwnerUnit->GetNickName() );
	
	CX2Item* pItem = GetItem();
	debugWPrintf( L"item : %s, %d", pItem->GetFullName().c_str(), (int) GetEqipPos() );
		
#endif HEAVY_LOG_TEST

	LastErrorLog( m_ModelName[0].c_str() );
	
    if( iIndex_ < 0 || iIndex_ >= CX2Item::MAX_MODEL_COUNT_A_ITEM )
        return;

    const int& i = iIndex_;

    //CKTDGXSkinAnimPtr   m_pUnitXSkinAnim = m_ptrUnitXSkinAnim.lock();
    //ASSERT( m_pUnitXSkinAnim != NULL );
    //if( m_pUnitXSkinAnim == NULL )
    //    return;

	//{{ dmlee 2009.3.19 note!! GetShowObject()함수를 호출할때 내부적으로 m_pManagerObject를 참조하는데 dangling pointer가 될 수 있다. 이를 임시로 해결하기 위해서 m_pManagerObject를 참조하지 않도록 하였다. 
	//if ( m_EqipType != CX2Item::UT_SKIN || !GetShowObject() )
	if( m_EqipType != CX2Item::UT_SKIN || !GetShowObjectPrivate() )
	//}} dmlee 2009.3.19 note!! GetShowObject()함수를 호출할때 내부적으로 m_pManagerObject를 참조하는데 dangling pointer가 될 수 있다. 이를 임시로 해결하기 위해서 m_pManagerObject를 참조하지 않도록 하였다. 
	{
		switch( m_iXSkinModelAttachment )
		{
		case 1:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
            if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
			m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );
			break;
		case 2:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
            if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
			m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
			break;
		}
		m_iXSkinModelAttachment = 0;
	}
	else
	{
		if ( m_bHyperMode && m_bCanHyperMode )
		{
			switch( m_iXSkinModelAttachment )
			{
			case 2:
				break;
			case 1:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
				m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );

#ifdef HEAVY_LOG_TEST
				debugWPrintf( L"remove normal skin : %s", m_pNormalXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST

			default:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, NULL, m_pHyperTexChangeXET, false, false );

				m_iXSkinModelAttachment = 2;

#ifdef HEAVY_LOG_TEST
				debugWPrintf( L"add hyper skin : %s", m_pHyperXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST

			}
		}
		else
		{
			switch( m_iXSkinModelAttachment )
			{
			case 1:
				break;
			case 2:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
				m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
#ifdef HEAVY_LOG_TEST
				debugWPrintf( L"remove hyper skin : %s", m_pHyperXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST

			default:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, NULL, m_pNormalTexChangeXET, false, false );

				if( NULL != m_pNormalXSkinModel[i] )
					m_iXSkinModelAttachment = 1;
				else
					m_iXSkinModelAttachment = 0;


#ifdef HEAVY_LOG_TEST
				if( m_pNormalXSkinModel[i] != NULL )
				{
					debugWPrintf( L"add normal skin : %s", m_pNormalXSkinModel[i]->GetDeviceID().c_str() );
				}
				else
				{
					debugWPrintf( L"add normal skin : NULL" );
				}
#endif HEAVY_LOG_TEST
			}
		}
	}//if.. else..



#ifdef HEAVY_LOG_TEST
	debugWPrintf( L"[END] _UpdateSkin( %d )", iIndex_ );
#endif HEAVY_LOG_TEST

}//CX2Eqip::_UpdateSkin()


void CX2Eqip::_UpdateSkin()
{

	KTDXPROFILE();


    //CKTDGXSkinAnimPtr   m_pUnitXSkinAnim = m_ptrUnitXSkinAnim.lock();
    //ASSERT( m_pUnitXSkinAnim != NULL );
    //if( m_pUnitXSkinAnim == NULL )
    //    return;



#ifdef HEAVY_LOG_TEST

	debugWPrintf( L"[START] _UpdateSkin()" );
	debugWPrintf( L"unit : %s", m_pOwnerUnit->GetNickName() );

	CX2Item* pItem = GetItem();
	if( pItem != NULL )
	{
		debugWPrintf( L"item : %s, %d", pItem->GetFullName().c_str(), (int) GetEqipPos() );
	}

#endif HEAVY_LOG_TEST

#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
	// June2017 CX2Eqip::_UpdateSkin() 3295ae20. Every model attaches (not only the first) when the item is in division-item group
	// 6, 0x31, 0x36 or 7 (32809f40 = IsDivisionItem on g_pData's item manager +0x230; those groups' parser is Stage 11, so today the
	// sets are empty and this reads false). June ORs two more terms that March has no data for (routed, R14 Done note):
	//   - the eqip's custom-render vector (+0x92c, 0xac each, filled by the item manager 32810450 = DITT_CUSTOM_RENDER) holds 2+ entries: Stage 11;
	//   - owner unit type 10 (UT_LU_CIEL), the ctor flag +0x15d4 clear and m_pNormalXSkinModel[1] set: character port (checklist).
	// All three adds pass the NORMAL multi-tex XET (+0x808), the hyper add too (JUNE_BUGS.md), and bAddCollisionData 0 (MOTION_COLLISION).
	bool bAllModels = false;
	if( NULL != m_pItemTemplet
		&& ( true == g_pData->GetItemManager()->IsDivisionItem( 6, m_pItemTemplet->GetItemID() )
		|| true == g_pData->GetItemManager()->IsDivisionItem( 0x31, m_pItemTemplet->GetItemID() )
		|| true == g_pData->GetItemManager()->IsDivisionItem( 0x36, m_pItemTemplet->GetItemID() )
		|| true == g_pData->GetItemManager()->IsDivisionItem( 7, m_pItemTemplet->GetItemID() ) ) )
		bAllModels = true;

	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	{
		if( m_EqipType == CX2Item::UT_SKIN && GetShowObject() )
		{
			if( m_bHyperMode && m_bCanHyperMode )
			{
				if( 1 == m_iXSkinModelAttachment )
				{
					if( m_pUnitXSkinAnim != NULL )
					{
						if( true == bAllModels )
						{
							for( int j = 0; j < CX2Item::MAX_MODEL_COUNT_A_ITEM && NULL != m_pNormalXSkinModel[j]; j++ )
								m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[j] );
						}
						else
							m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );
					}
				}
				else if( 2 == m_iXSkinModelAttachment )
					continue;

				if( m_pUnitXSkinAnim != NULL )	// line 0xe3d @3295afc8
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pNormalMultiTexXET, m_pHyperTexChangeXET, false, false );
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pNormalMultiTexXET, m_pHyperTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
				m_iXSkinModelAttachment = 2;
			}
			else if( 1 != m_iXSkinModelAttachment )
			{
				if( 2 == m_iXSkinModelAttachment && m_pUnitXSkinAnim != NULL )
					m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );

				if( m_pUnitXSkinAnim != NULL )
				{
					if( true == bAllModels )
					{
						// line 0xe6d @3295b051: from model i up to the first empty slot
						for( int j = i; j < CX2Item::MAX_MODEL_COUNT_A_ITEM && NULL != m_pNormalXSkinModel[j]; j++ )
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
							m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[j], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET, false, false );
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
							m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[j], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					}
					else	// line 0xe82 @3295b097
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
						m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET, false, false );
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
						m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
				}
				m_iXSkinModelAttachment = ( NULL != m_pNormalXSkinModel[i] ) ? 1 : 0;
			}
		}
		else
		{
			if( 1 == m_iXSkinModelAttachment )
			{
				if( m_pUnitXSkinAnim != NULL )
				{
					if( true == bAllModels )
					{
						for( int j = 0; j < CX2Item::MAX_MODEL_COUNT_A_ITEM && NULL != m_pNormalXSkinModel[j]; j++ )
							m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[j] );
					}
					else
						m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );
				}
			}
			else if( 2 == m_iXSkinModelAttachment && m_pUnitXSkinAnim != NULL )
				m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
			m_iXSkinModelAttachment = 0;
		}
	}
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	{
		if ( m_EqipType != CX2Item::UT_SKIN || !GetShowObject() )
		{
			switch( m_iXSkinModelAttachment )
			{
			case 1:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
				m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );
				break;
			case 2:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
				m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
				break;
			}
			m_iXSkinModelAttachment = 0;
		}
		else
		{
			if ( m_bHyperMode && m_bCanHyperMode )
			{
				switch( m_iXSkinModelAttachment )
				{
				case 2:
					break;
				case 1:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
					m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pNormalXSkinModel[i] );

#ifdef HEAVY_LOG_TEST
					debugWPrintf( L"REMOVE NORMAL SKIN : %s", m_pNormalXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST
				default:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, NULL, m_pHyperTexChangeXET, false, false );	// June2017 3295ae20: bAddCollisionData = 0, the motion carries the unit's collision
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, NULL, m_pHyperTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_iXSkinModelAttachment = 2;
#ifdef HEAVY_LOG_TEST
					debugWPrintf( L"ADD HYPER SKIN : %s", m_pHyperXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST
				}
			}
			else
			{
				switch( m_iXSkinModelAttachment )
				{
				case 1:
					break;
				case 2:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
					m_pUnitXSkinAnim->RemoveModelXSkinMesh( m_pHyperXSkinModel[i] );
#ifdef HEAVY_LOG_TEST
					debugWPrintf( L"REMOVE HYPER SKIN : %s", m_pHyperXSkinModel[i]->GetDeviceID().c_str() );
#endif HEAVY_LOG_TEST

				default:
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    if ( m_pUnitXSkinAnim != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, NULL, m_pNormalTexChangeXET, false, false );	// June2017 3295ae20: bAddCollisionData = 0, the motion carries the unit's collision
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, NULL, m_pNormalTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
					if( NULL != m_pNormalXSkinModel[i] )
						m_iXSkinModelAttachment = 1;
					else
						m_iXSkinModelAttachment = 0;

#ifdef HEAVY_LOG_TEST
					if( m_pNormalXSkinModel[i] != NULL )
					{
						debugWPrintf( L"ADD NORMAL SKIN : %s", m_pNormalXSkinModel[i]->GetDeviceID().c_str() );
					}
					else
					{
						debugWPrintf( L"ADD NORMAL SKIN : NULL" );
					}
#endif HEAVY_LOG_TEST
				}
			}
		}//if.. else..
	}// for
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN



#ifdef HEAVY_LOG_TEST
	debugWPrintf( L"[END] _UpdateSkin()" );
#endif HEAVY_LOG_TEST

}//CX2Eqip::_UpdateSkin()


CX2Item* CX2Eqip::GetItem()
{
	if ( m_bDummyBasic == true)
	{
		return m_pDummyItem;
	}
	
	CX2Item* pItem = NULL; 
	if( m_pOwnerUnit != NULL )
	{
		pItem = m_pOwnerUnit->GetInventory().GetItem( m_ItemUID, true );
	}	
	
	return pItem;
}//CX2Eqip::GetItem()


//#ifdef EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13
void CX2Eqip::InitSkin()
{
	KTDXPROFILE();

	//background!!!
//     if ( g_pKTDXApp->GetDeviceManager()->IsEnableThreadLoading() )
//     {
// 	    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
// 	    {
//             if( m_ModelName[i].empty() )
//             {
//                 m_pNormalXSkinModel[i] = NULL;
//             }
//             else
//             {
//                 HRESULT hr = g_pKTDXApp->GetDeviceManager()->ThreadReq_OpenDevice(
//                     this, m_ModelName[i], CKTDXDeviceManager::THREAD_REQUEST_EQUIP_SKIN_NORMAL, i );
//                 ASSERT( SUCCEEDED(hr) );
//             }//if.. else..
// 	    }//for
// 
// 	    m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
// 	    m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );	
// 
// 	    if( m_bCanHyperMode == true )
// 	    {
// 		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
// 		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
//         }//if
// 
//     }
//     else
    {
#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
		// June2017 CX2Eqip::InitSkin 32963eb0 (X2Eqip.cpp line 0xf45): the XETs first (3295ffd0, called at the top), then the models, then
		// one skin anim per hyper model. The error block and _UpdateSkin() below are shared with March's code (June logs XEM 0x276d = 93).
		// 3295ffd0: the four XETs, always (March opens the hyper pair only when m_bCanHyperMode; an empty name opens nothing in both).
		m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
		m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );
		m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
		m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
		// Then, with no item templet or for a weapon (templet +0xd5 = EP_WEAPON_HAND 2 / EP_AC_WEAPON 0x19), the enchant multi-tex XETs.
		if( NULL == m_pItemTemplet || CX2Unit::EP_WEAPON_HAND == m_pItemTemplet->GetEqipPosition() || CX2Unit::EP_AC_WEAPON == m_pItemTemplet->GetEqipPosition() )
		{
			WCHAR wszMultiTexName[256] = L"";
			StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
			m_pNormalMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
			if( NULL == EnchantStage2Texture( m_EnchantLevel ) )
			{
				if( NULL != m_pNormalMultiTexXET )
					m_pNormalMultiTexXET->DeleteMultiTexStage2();
			}
			else
				SetEnchantStage2( m_pNormalMultiTexXET, m_EnchantLevel, "InitSkin", m_ItemID );

			// The hyper name takes m_ModelNameHyper[0] (+0x724), and June stores the XET in m_pHyperTexChangeXET (+0x814 @32960287), over the
			// tex-change XET opened above (its reference is never closed), not in m_pHyperMultiTexXET (+0x818): JUNE_BUGS.md.
			StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_HYPER_%s_%dl", m_ModelNameHyper[0].c_str(), m_ItemUID );
			m_pHyperTexChangeXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
			if( NULL == EnchantStage2Texture( m_EnchantLevel ) )
			{
				if( NULL != m_pHyperTexChangeXET )
					m_pHyperTexChangeXET->DeleteMultiTexStage2();
			}
			else
				SetEnchantStage2( m_pHyperTexChangeXET, m_EnchantLevel, "InitSkinHyper", m_ItemID );
		}

		// Routed (R14 Done note), both keyed by the item templet's id: when the device manager's item map (+0x260, 31cdcc70) holds the id,
		// June stores it in each mesh (+0xa40); and when g_pData's item manager has a frame list for the id (32812f40, map +0x370), each
		// named frame (329656d0) gets two vectors (+0x108 and +0xfc; the hyper models only the first). Stage 11 owns both.
		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
			g_pMain->GetMemoryHolder()->LoadEquip( m_ModelName[i].c_str() );	// 32851d60 on g_pMain +0x510
			m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );	// 31cdef40( name, L"Bip01" ) = March's default
		}

		if( m_bCanHyperMode == true )
		{
			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
			{
				g_pMain->GetMemoryHolder()->LoadEquip( m_ModelNameHyper[i].c_str() );
				m_pHyperXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
				if( NULL == m_pHyperXSkinModel[i] )
					continue;

				// The eqip's own hyper skin anim (+0x564), as the attach-anim init builds one; March's skin path had none.
				SAFE_DELETE_KTDGOBJECT( m_pHyperXSkinAnim[i] );
				m_pHyperXSkinAnim[i] = CKTDGXSkinAnim::CreateSkinAnim();
				m_pHyperXSkinAnim[i]->SetAnimXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET );	// 31d32750( mesh, xet, 0 )
#ifdef SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
				m_pHyperXSkinAnim[i]->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pHyperMultiTexXET, m_pHyperTexChangeXET, false, false );	// @329643b5
#else SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
				m_pHyperXSkinAnim[i]->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pHyperMultiTexXET, m_pHyperTexChangeXET );
#endif SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION
				m_pHyperXSkinAnim[i]->ChangeAnim( m_AniName.c_str(), false );	// 31d33fd0( +0x85c = m_AniName, 0, 0.f ): the NORMAL name (JUNE_BUGS.md)
				m_pHyperXSkinAnim[i]->Play( CKTDGXSkinAnim::XAP_LOOP );		// 31d28d00( 0 )
				m_pHyperXSkinAnim[i]->UseDXMatrix( true );					// +0x31c = 1
			}
		}
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
	    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
	    {
		    g_pMain->GetMemoryHolder()->LoadEquip( m_ModelName[i].c_str() );		// fix!! 이 부분 필요한가?

		    m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );
	    }//for

	    m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
	    m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );	

	    if( m_bCanHyperMode == true )
	    {
		    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		    {
			    g_pMain->GetMemoryHolder()->LoadEquip( m_ModelNameHyper[i].c_str() );		// fix!! 이 부분 필요한가?

			    m_pHyperXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
		    }
    		
		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
	    }//if
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN

	    if ( m_pNormalXSkinModel[0] == NULL )
	    {
		    wstring errorMSG;
		    errorMSG = L"- 모델 이름 : ";
		    errorMSG += m_ModelName[0];

		    ErrorLogMsg( XEM_ERROR93, errorMSG.c_str() );
		    ASSERT( !"CX2Eqip::InitSkin, null normal skin model" );

		    if ( m_bCanHyperMode == true && m_pHyperXSkinModel[0] == NULL )
		    {
			    wstring errorMSG2;
			    errorMSG2 = L"- 모델 이름 : ";
			    errorMSG2 += m_ModelNameHyper[0];

			    ErrorLogMsg( XEM_ERROR93, errorMSG2.c_str() );
		    }//if

		    return;
	    }//if

        _UpdateSkin();
    }//if.. else..
}//CX2Eqip::InitSkin()
//#else // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13
////{{AFX
//void CX2Eqip::InitSkin()
//{
//	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//	{
//		g_pMain->GetMemoryHolder()->LoadEquip( m_ModelName[i].c_str() );		// fix!! 이 부분 필요한가?
//
//		m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );
//	}
//	m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
//	m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );	
//
//	if( m_bCanHyperMode == true )
//	{
//
//		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//		{
//			g_pMain->GetMemoryHolder()->LoadEquip( m_ModelNameHyper[i].c_str() );		// fix!! 이 부분 필요한가?
//
//			m_pHyperXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
//		}
//		
//		m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
//		m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
//	}
//
//	if ( m_pNormalXSkinModel[0] == NULL )
//	{
//		wstring errorMSG;
//		errorMSG = L"- 모델 이름 : ";
//		errorMSG += m_ModelName[0];
//
//		ErrorLogMsg( XEM_ERROR93, errorMSG.c_str() );
//		ASSERT( !"CX2Eqip::InitSkin, null normal skin model" );
//
//		if ( m_bCanHyperMode == true && m_pHyperXSkinModel[0] == NULL )
//		{
//			wstring errorMSG2;
//			errorMSG2 = L"- 모델 이름 : ";
//			errorMSG2 += m_ModelNameHyper[0];
//
//			ErrorLogMsg( XEM_ERROR93, errorMSG2.c_str() );
//		}
//
//		return;
//	}
//
////{{ robobeg : 2008-10-21
//	//m_pUnitXSkinAnim->AddModelXSkinMesh( m_pNormalXSkinModel, m_pNormalAniXET, NULL, m_pNormalTexChangeXET );
//    _UpdateSkin();
////}} robobeg : 2008-10-21
//}//CX2Eqip::InitSkin()
////}}AFX
//#endif // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13


//#ifdef EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13
void CX2Eqip::InitAttatchAnim()
{
	KTDXPROFILE();

	m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
	m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );

	WCHAR wszMultiTexName[256] = L"";
	StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );

	m_pNormalMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
	// June2017 3295dfa0 @3295e10c-3295e15c: below 7 stage 2 is deleted, else the June texture by level. The rest of June's
	// attach-anim init (minor-unit model, UT_ADD index remap, appearance templet, jiggle) is R14b.
	if( NULL == EnchantStage2Texture( m_EnchantLevel ) )
	{
		if( m_pNormalMultiTexXET != NULL )
			m_pNormalMultiTexXET->DeleteMultiTexStage2();
	}
	else
		SetEnchantStage2( m_pNormalMultiTexXET, m_EnchantLevel, "InitAttatchAnim", m_ItemID );
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
	if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
	{		
		if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
#ifdef ADD_UPGRADE_WEAPON_PARTICLE		/// 11강 무기 이펙트
			m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "11Gang_Protect.tga", D3DTOP_ADD );
#else ADD_UPGRADE_WEAPON_PARTICLE
			m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
#endif ADD_UPGRADE_WEAPON_PARTICLE
		else
			m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );	
	}//if
	// kimhc // 2009-12-30 // 장비 장착시 강화 장비가 아니면
	else
	{
		if( m_pNormalMultiTexXET != NULL )
			m_pNormalMultiTexXET->DeleteMultiTexStage2();
	}
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN

		//background!!!
//     if ( g_pKTDXApp->GetDeviceManager()->IsEnableThreadLoading() )
//     {
// 	    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
// 	    {
//             if( m_ModelName[i].empty() )
//             {
//                 m_pNormalXSkinModel[i] = NULL;
//             }
//             else
//             {
// 		        HRESULT hr = g_pKTDXApp->GetDeviceManager()->ThreadReq_OpenDevice(
//                     this, m_ModelName[i], CKTDXDeviceManager::THREAD_REQUEST_EQUIP_ATTACH_ANIM_NORMAL, i );
//                 ASSERT( SUCCEEDED(hr) );
//             }//if.. else..
// 	    }//for
// 
// 	    if( m_bCanHyperMode == true )
// 	    {
// 		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
// 		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
// 
// 		    WCHAR wszMultiTexName[256] = L"";
// 		    StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_HYPER_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
// 
// 		    m_pHyperMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
// 		    if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
// 		    {			
// 			    if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
// 				    m_pHyperMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
// 			    else
// 				    m_pHyperMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );			
// 		    }//if
// 	    }//if
//     }
//     else
    {
#ifdef SERV_9TH_NEW_CHARACTER // 김태환
		/// 아이템 장착 위치
		CX2Unit::EQIP_POSITION	eEquipPosition	= m_pItemTemplet->GetEqipPosition();
#endif //SERV_9TH_NEW_CHARACTER
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
		// June2017 3295dfa0: what this eqip did differently from March, for the one "[JUNE_ENGINE] EQATTACH" line at the end.
		int		iAddSlotsSkipped	= 0;		// Add owner: weapon slots with no "Weapon0<n>" frame (no model)
		bool	bHyperRemap			= false;	// hyper half: Add weapon slots 1.. opened model 0
		bool	bTraceLandPos		= false;	// an attach frame "TraceLandPos" took the clone frame root
		bool	bJiggle				= false;	// DITT 30 jiggle applied
		bool	bJiggleLost			= false;	// March's ITEM_ID_JIGGLE_TYPE1 list would jiggle, DITT 30 does not (Stage 11)
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM

		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
			//g_pMain->GetMemoryHolder()->LoadEquip( m_ModelName[i].c_str() );		// fix!! 이 부분 필요한가?
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			// June2017 3295dfa0 @3295e1a7-3295e205: when the OWNER unit (+0x1b4 -> +0xaf0 -> type) is Add (9) and the item sits on
			// EP_WEAPON_HAND / EP_AC_WEAPON (+0xd5 = 2 / 0x19), slot i gets a model only when the unit skin anim (+0x1c0) has the
			// clone frame "Weapon0<i+1>" (334c9ad0, 31d38a20); no unit skin anim = no model. No owner unit takes the normal path.
			// March opened a model and an anim in every slot. The index remap below (templet unit type, = March) is unchanged.
			// Routed (checklist 7aa, Lu/Ciel): after the model opens June builds a minor-unit anim / model pair (+0x584 / +0x5e4) from
			// the owner templet's per-position minor item (329657d0 +0xe4..+0x100, 325bb740, m_bIsForMinorUnit flags2 bit 2,
			// m_ItemIDForMinorUnit +0x30, model name 32867210), with the 3295b230 jiggle; skipped for templet unit types 9 / 0xb.
			if( NULL != m_pOwnerUnit && NULL != m_pOwnerUnit->GetUnitTemplet()
				&& CX2Unit::UT_ADD == m_pOwnerUnit->GetUnitTemplet()->m_UnitType
				&& ( CX2Unit::EP_WEAPON_HAND == m_pItemTemplet->GetEqipPosition() || CX2Unit::EP_AC_WEAPON == m_pItemTemplet->GetEqipPosition() ) )
			{
				WCHAR wszFrame[256] = L"";
				StringCchPrintfW( wszFrame, 255, L"Weapon0%d", i + 1 );
				if( m_pUnitXSkinAnim == NULL || NULL == m_pUnitXSkinAnim->GetCloneFrame( wszFrame ) )
				{
					++iAddSlotsSkipped;
					continue;
				}
			}
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM


#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			int iTargetIndex	= i;		/// 설정할 대상의 인덱스

			/// 애드의 무기일 때만, 0번 인덱스의 무기를 다른 인덱스에 모두 설정해줌
			if( CX2Unit::UT_ADD == m_pItemTemplet->GetUnitType() &&  
				( CX2Unit::EP_WEAPON_HAND == eEquipPosition || CX2Unit::EP_AC_WEAPON == eEquipPosition ) )
				iTargetIndex = 0;

			m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[iTargetIndex] );
#else //SERV_9TH_NEW_CHARACTER
			m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );
#endif //SERV_9TH_NEW_CHARACTER

			if( NULL == m_pNormalXSkinModel[i] )
			{
				//ASSERT( i != 0 );
				continue;
			}//if

			ASSERT( m_pNormalXSkinAnim[i] == NULL );
			SAFE_DELETE_KTDGOBJECT( m_pNormalXSkinAnim[i] );
			m_pNormalXSkinAnim[i] = CKTDGXSkinAnim::CreateSkinAnim();
			m_pNormalXSkinAnim[i]->SetAnimXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET );
			m_pNormalXSkinAnim[i]->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET );
			m_pNormalXSkinAnim[i]->ChangeAnim( m_AniName.c_str(), false );
			m_pNormalXSkinAnim[i]->Play( CKTDGXSkinAnim::XAP_LOOP );
			m_pNormalXSkinAnim[i]->UseDXMatrix( true );

#ifdef JIGGLEBONE_TEST
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			// June2017 3295dfa0 @3295e5b0-3295e64a: jiggle when the item is in DIVISION_ITEM_TEMPLET_TYPE 30 (DITT_ITEM_ID_JIGGLE_TYPE1,
			// 32809f40), tension 1.5 (33471a1c), expand 0 (+0x848). March read GetJiggleBone()'s ITEM_ID_JIGGLE_TYPE1 list (June moved
			// the same ids into RenderDivisionItemTemplet.lua). Group 30 is filled by backlog Stage 11: until then nothing jiggles.
			if( NULL != m_pItemTemplet && true == g_pData->GetItemManager()->IsDivisionItem( 30, m_pItemTemplet->GetItemID() ) )
			{
				m_pNormalXSkinAnim[i]->SetUnitXSkinAnim( m_pUnitXSkinAnim );
				m_pNormalXSkinAnim[i]->SetJiggleMesh( true, 1.5f, 0 );
				bJiggle = true;
			}
			else if( GetJiggleBone() >= 0.f )	// log only: June has no list
				bJiggleLost = true;
#else SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			float fTension = GetJiggleBone();
			if( fTension >= 0.f)
			{
				m_pNormalXSkinAnim[i]->SetUnitXSkinAnim(m_pUnitXSkinAnim);
				m_pNormalXSkinAnim[i]->SetJiggleMesh(true, fTension);
			}
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
#endif
		}//for

	    if( m_bCanHyperMode == true )
	    {

		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );

		    WCHAR wszMultiTexName[256] = L"";
		    StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_HYPER_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );

		    m_pHyperMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
		    // June2017 3295dfa0 @3295e79c-3295e7ca: from level 7 the June texture by level; below 7 nothing (as March).
		    if( NULL != EnchantStage2Texture( m_EnchantLevel ) )
			    SetEnchantStage2( m_pHyperMultiTexXET, m_EnchantLevel, "InitAttatchAnimHyper", m_ItemID );
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
		    if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
		    {			
			    if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
#ifdef ADD_UPGRADE_WEAPON_PARTICLE		/// 11강 무기 이펙트
				m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "11Gang_Protect.tga", D3DTOP_ADD );
#else ADD_UPGRADE_WEAPON_PARTICLE
				m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
#endif ADD_UPGRADE_WEAPON_PARTICLE
			    else
				    m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );			
		    }//if
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN



		    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		    {
			    //g_pMain->GetMemoryHolder()->LoadEquip( m_ModelNameHyper[i].c_str() );

#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			    // June2017 3295dfa0 @3295e7e0-3295e801: the hyper half remaps as the normal half does (templet unit type Add on
			    // EP_WEAPON_HAND / EP_AC_WEAPON -> hyper model 0, +0x724); March's hyper loop opened slot i.
			    int iHyperIndex = i;
			    if( CX2Unit::UT_ADD == m_pItemTemplet->GetUnitType()
				    && ( CX2Unit::EP_WEAPON_HAND == m_pItemTemplet->GetEqipPosition() || CX2Unit::EP_AC_WEAPON == m_pItemTemplet->GetEqipPosition() ) )
			    {
				    iHyperIndex = 0;
				    if( 0 != i )
					    bHyperRemap = true;
			    }
			    m_pHyperXSkinModel[i]		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[iHyperIndex] );
#else SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			    m_pHyperXSkinModel[i]		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM

			    if( NULL == m_pHyperXSkinModel[i] )
			    {
				    ASSERT( i != 0 );
				    continue;
			    }//if

				ASSERT( m_pHyperXSkinAnim[i] == NULL );
				SAFE_DELETE_KTDGOBJECT( m_pHyperXSkinAnim[i] );
			    m_pHyperXSkinAnim[i] = CKTDGXSkinAnim::CreateSkinAnim();
			    m_pHyperXSkinAnim[i]->SetAnimXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET );
			    m_pHyperXSkinAnim[i]->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pHyperMultiTexXET, m_pHyperTexChangeXET );
			    m_pHyperXSkinAnim[i]->ChangeAnim( m_AniNameHyper.c_str(), false );
			    m_pHyperXSkinAnim[i]->Play( CKTDGXSkinAnim::XAP_LOOP );
			    m_pHyperXSkinAnim[i]->UseDXMatrix( true );

#ifdef JIGGLEBONE_TEST
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
				// June2017 3295dfa0 @3295e94c-3295e9e6: DITT 30 as the normal half, tension 1.5 (33471a1c), expand 4 (+0x848).
				if( NULL != m_pItemTemplet && true == g_pData->GetItemManager()->IsDivisionItem( 30, m_pItemTemplet->GetItemID() ) )
				{
					m_pHyperXSkinAnim[i]->SetUnitXSkinAnim( m_pUnitXSkinAnim );
					m_pHyperXSkinAnim[i]->SetJiggleMesh( true, 1.5f, 4 );
					bJiggle = true;
				}
				else if( GetJiggleBone() >= 0.f )	// log only: June has no list
					bJiggleLost = true;
#else SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
				float fTension = GetJiggleBone();
				if( fTension >= 0.f)
				{
					m_pHyperXSkinAnim[i]->SetUnitXSkinAnim(m_pUnitXSkinAnim);
					m_pHyperXSkinAnim[i]->SetJiggleMesh(true, fTension, 4);

				}
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
#endif
		    }//for
	    }//if


        //CKTDGXSkinAnimPtr   m_pUnitXSkinAnim = m_ptrUnitXSkinAnim.lock();
        //if( m_pUnitXSkinAnim != NULL )
        {
	        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
	        for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	        {
#ifdef SERV_9TH_NEW_CHARACTER // 김태환
				/// 애드의 무기 및 무기 악세 일 때만, 지정된 본에 무기를 붙여주자.
				if( CX2Unit::UT_ADD == m_pItemTemplet->GetUnitType() &&  
					( CX2Unit::EP_WEAPON_HAND == eEquipPosition || CX2Unit::EP_AC_WEAPON == eEquipPosition ) )
				{
					WCHAR buf[256] = {0,};

					StringCchPrintf( buf, 255, L"Weapon0%d", i + 1 );	/// Weapon01 ~ Weapon06

#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
					// June2017 3295dfa0 @3295e9f0-3295ea2c: the unit skin anim (+0x1c0) is null-checked first.
					pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( buf ) : NULL;
#else SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
					pFrame = m_pUnitXSkinAnim->GetCloneFrame( buf );
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM

					if( NULL != pFrame )
						m_pUnitMatrix[i] = &pFrame->combineMatrix;
				}
				else
#endif //SERV_9TH_NEW_CHARACTER
				{
					if( m_pItemTemplet->GetAttachFrameName(i)[0] != NULL )
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
					// Routed (checklist 7ab, Rose): June2017 3295dfa0 @3295ea4f-3295ec1e, before this default, a templet of unit type 0xb
					// (UT_ROSE; March has none) on EP_WEAPON_HAND takes attach frame 1 (328671e0( 1 )) for slots 1..4, frame 0 otherwise.
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
					{
			        	pFrame = 
	#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX          
                        ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) ) : NULL;
	#else   X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                        m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) );
	#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
						// June2017 3295dfa0 @3295ecd5-3295ed2c: an attach frame named "TraceLandPos" (334c9a84, wstring compare == 0)
						// takes the unit skin anim's clone frame root instead (skin anim +0x324 = m_pCloneFrameRoot, rebuilt by 31d32620),
						// whatever GetCloneFrame found; no unit skin anim = none.
						if( 0 == wcscmp( m_pItemTemplet->GetAttachFrameName(i), L"TraceLandPos" ) )
						{
							pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrameRoot() : NULL;
							bTraceLandPos = true;
						}
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
			        	if( NULL != pFrame )
			        	{
				        	m_pUnitMatrix[i] = &pFrame->combineMatrix;
						}//if
					}//if
		        }//if
	        }//for
#ifdef SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
		// Routed (backlog Stage 11): June2017 3295dfa0 @3295edc0-3295ee2a, after the frames: an item in DITT 72
		// (DITT_LOOP_ANIM_ITEM_ID, 32809f40) looks up its record in the item manager (32807e00, map +0x2c0), calls 3295a570( record
		// name, 1 ) = ChangeAnim( name, true ) on every normal / hyper / minor attach anim when +0x1c8 == 2, and sets +0x15f0 = 1.
		// Group 72 is empty until Stage 11, so the branch is dead here.

		// One "[JUNE_ENGINE] EQATTACH" line per eqip whose June path differed: the first 60 per run, then every 100th.
		if( 0 < iAddSlotsSkipped || bHyperRemap || bTraceLandPos || bJiggle || bJiggleLost )
		{
			static int s_iEqAttachLines = 0;
			++s_iEqAttachLines;
			if( s_iEqAttachLines <= 60 || 0 == s_iEqAttachLines % 100 )
			{
				KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] EQATTACH item " << m_ItemID << " unittype " << (int)m_pItemTemplet->GetUnitType()
					<< " pos " << (int)m_pItemTemplet->GetEqipPosition() << " addslots_skipped " << iAddSlotsSkipped
					<< " hyper_remap " << ( bHyperRemap ? 1 : 0 ) << " hyper " << ( m_bCanHyperMode ? 1 : 0 )
					<< " tracelandpos " << ( bTraceLandPos ? 1 : 0 ) << " jiggle " << ( bJiggle ? 1 : 0 )
					<< " jiggle_lost " << ( bJiggleLost ? 1 : 0 ) << " line " << s_iEqAttachLines << fileout;
			}
		}
#endif SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM
        }//if
    }//if.. else..
}//CX2Eqip::InitAttatchAnim()

//#else // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13
//
////{{AFX
//void CX2Eqip::InitAttatchAnim()
//{
//	m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
//	m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );
//
//	WCHAR wszMultiTexName[256] = L"";
//	StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
//
//	m_pNormalMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
//	if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
//	{		
//		if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
//			m_pNormalMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
//		else
//			m_pNormalMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );	
//	}
//
//
//	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//	{
//		g_pMain->GetMemoryHolder()->LoadEquip( m_ModelName[i].c_str() );		// fix!! 이 부분 필요한가?
//
//		m_pNormalXSkinModel[i]	= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelName[i] );
//
//		if( NULL == m_pNormalXSkinModel[i] )
//		{
//			ASSERT( i != 0 );
//			continue;
//		}
//
//		m_pNormalXSkinAnim[i] = CKTDGXSkinAnim::CreateSkinAnim();
//		m_pNormalXSkinAnim[i]->SetAnimXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET );
//		m_pNormalXSkinAnim[i]->AddModelXSkinMesh( m_pNormalXSkinModel[i], m_pNormalAniXET, m_pNormalMultiTexXET, m_pNormalTexChangeXET );
//		m_pNormalXSkinAnim[i]->ChangeAnim( m_AniName.c_str(), false );
//		m_pNormalXSkinAnim[i]->Play( CKTDGXSkinAnim::XAP_LOOP );
//		m_pNormalXSkinAnim[i]->UseDXMatrix( true );
//	}
//
//
//
//	if( m_bCanHyperMode == true )
//	{
//
//		m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
//		m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
//
//		WCHAR wszMultiTexName[256] = L"";
//		StringCchPrintfW( wszMultiTexName, ARRAY_SIZE(wszMultiTexName), L"MULTI_TEX_HYPER_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
//
//		m_pHyperMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszMultiTexName, true );
//		if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
//		{			
//			if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
//				m_pHyperMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
//			else
//				m_pHyperMultiTexXET->SetMultiTexStage2_LUA( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );			
//		}
//
//
//
//		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//		{
//			g_pMain->GetMemoryHolder()->LoadEquip( m_ModelNameHyper[i].c_str() );
//
//			m_pHyperXSkinModel[i]		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_ModelNameHyper[i] );
//
//			if( NULL == m_pHyperXSkinModel[i] )
//			{
//				ASSERT( i != 0 );
//				continue;
//			}
//
//			m_pHyperXSkinAnim[i] = CKTDGXSkinAnim::CreateSkinAnim();
//			m_pHyperXSkinAnim[i]->SetAnimXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET );
//			m_pHyperXSkinAnim[i]->AddModelXSkinMesh( m_pHyperXSkinModel[i], m_pHyperAniXET, m_pHyperMultiTexXET, m_pHyperTexChangeXET );
//			m_pHyperXSkinAnim[i]->ChangeAnim( m_AniNameHyper.c_str(), false );
//			m_pHyperXSkinAnim[i]->Play( CKTDGXSkinAnim::XAP_LOOP );
//			m_pHyperXSkinAnim[i]->UseDXMatrix( true );
//		}
//	}
//
//
//
//	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
//	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
//	{
//        if( m_pItemTemplet->GetAttachFrameName(i)[0] != NULL )
//		{
//#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
//            pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) ) : NULL;
//#else   X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
//			pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) );
//#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
//			if( NULL != pFrame )
//			{
//				m_pUnitMatrix[i] = &pFrame->combineMatrix;
//			}
//		}
//	}
//}//CX2Eqip::InitAttatchAnim()
////}}AFX
//#endif // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-13


//#ifdef EQUIP_BACKGROUND_LOADING_TEST // 2008-12-14
void CX2Eqip::InitAttatchNormal()
{
	KTDXPROFILE();

    //if ( g_pKTDXApp->GetDeviceManager()->IsEnableThreadLoading() )
    {
	    m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
	    m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );
	    if( m_pNormalAniXET != NULL )
		    m_pNormalAniData	= m_pHyperAniXET->GetAniData( m_AniName.c_str() );

#ifdef SERV_9TH_NEW_CHARACTER // 김태환
		/// 아이템 장착 위치
		CX2Unit::EQIP_POSITION	eEquipPosition	 = m_pItemTemplet->GetEqipPosition();
#endif //SERV_9TH_NEW_CHARACTER

		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
	#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			int iTargetIndex	= i;		/// 설정할 대상의 인덱스

			/// 애드의 무기일 때만, 0번 인덱스의 무기를 다른 인덱스에 모두 설정해줌
			if( CX2Unit::UT_ADD == m_pItemTemplet->GetUnitType() && 
				( CX2Unit::EP_WEAPON_HAND ==  eEquipPosition || CX2Unit::EP_AC_WEAPON == eEquipPosition )  )
				iTargetIndex = 0;

			m_pNormalXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelName[iTargetIndex].c_str() );
	#else // SERV_9TH_NEW_CHARACTER
			m_pNormalXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelName[i].c_str() );
	#endif // SERV_9TH_NEW_CHARACTER
		}//for


	    if( m_bCanHyperMode == true )
	    {
		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
		    if( m_pHyperAniXET != NULL )
			    m_pHyperAniData		= m_pHyperAniXET->GetAniData( m_AniNameHyper.c_str() );

			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
			{
				m_pHyperXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelNameHyper[i] );
			}//for
	    }//if

		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
			for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
			{
#ifdef SERV_9TH_NEW_CHARACTER // 김태환

				/// 애드의 무기 및 무기 악세 일 때만, 지정된 본에 무기를 붙여주자.
				if( CX2Unit::UT_ADD == m_pItemTemplet->GetUnitType() &&  
					( CX2Unit::EP_WEAPON_HAND == eEquipPosition || CX2Unit::EP_AC_WEAPON == eEquipPosition ) )
				{
					WCHAR buf[256] = {0,};

					StringCchPrintf( buf, 255, L"Weapon0%d", i + 1 );	/// Weapon01 ~ Weapon06

					pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( buf ) : NULL;

					if( NULL != pFrame )
						m_pUnitMatrix[i] = &pFrame->combineMatrix;
				}
				else
	#endif // SERV_9TH_NEW_CHARACTER
				{
					if( m_pItemTemplet->GetAttachFrameName(i)[0] != NULL )
					{
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
						pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) ) : NULL;
#else   X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
						pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) );
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX  
						if( NULL != pFrame )
						{
							m_pUnitMatrix[i] = &pFrame->combineMatrix;
						}//if
					}//if
				}//if
			}//for
		}//if
    }
	/*
	else
	{
		m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
		m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );
		if( m_pNormalAniXET != NULL )
			m_pNormalAniData	= m_pHyperAniXET->GetAniData( m_AniName.c_str() );


		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
		    m_pNormalXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelName[i] );
	    }//for


	    if( m_bCanHyperMode == true )
	    {
		    m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
		    m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
		    if( m_pHyperAniXET != NULL )
			    m_pHyperAniData		= m_pHyperAniXET->GetAniData( m_AniNameHyper.c_str() );


		    for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		    {
			    m_pHyperXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelNameHyper[i] );
		    }//for
	    }//if
    	

        //CKTDGXSkinAnimPtr   m_pUnitXSkinAnim = m_ptrUnitXSkinAnim.lock();
        //if( m_pUnitXSkinAnim != NULL )
        {
	        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
	        for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
	        {
		        if( false == m_pItemTemplet->m_AttachFrameName[i].empty() )
		        {
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
                    pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->m_AttachFrameName[i].c_str() ) : NULL;
#else   X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
			        pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->m_AttachFrameName[i].c_str() );
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
			        if( NULL != pFrame )
			        {
				        m_pUnitMatrix[i] = &pFrame->combineMatrix;
			        }//if
		        }//if
	        }//for
        }//if
    }//if.. else..
	*/
}//CX2Eqip::InitAttatchNormal()

//#else // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-14
//
////{{AFX
//void CX2Eqip::InitAttatchNormal()
//{
//	m_pNormalTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexName );
//	m_pNormalAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETName );
//	if( m_pNormalAniXET != NULL )
//		m_pNormalAniData	= m_pHyperAniXET->GetAniData( m_AniName.c_str() );
//
//
//	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//	{
//		m_pNormalXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelName[i] );
//	}//for
//
//
//	if( m_bCanHyperMode == true )
//	{
//		m_pHyperTexChangeXET	= g_pKTDXApp->GetDeviceManager()->OpenXET( m_ChangeTexNameHyper );
//		m_pHyperAniXET			= g_pKTDXApp->GetDeviceManager()->OpenXET( m_AniXETNameHyper );
//		if( m_pHyperAniXET != NULL )
//			m_pHyperAniData		= m_pHyperAniXET->GetAniData( m_AniNameHyper.c_str() );
//
//
//		for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
//		{
//			m_pHyperXMesh[i]			= g_pKTDXApp->GetDeviceManager()->OpenXMesh( m_ModelNameHyper[i] );
//		}//for
//	}//if
//	
//
//	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
//	for( int i=0; i< CX2Item::MAX_MODEL_COUNT_A_ITEM ; i++ )
//	{
//        if( m_pItemTemplet->GetAttachFrameName(i)[0] != NULL )
//		{
//#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
//            pFrame = ( m_pUnitXSkinAnim != NULL ) ? m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) ) : NULL;
//#else   X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
//			pFrame = m_pUnitXSkinAnim->GetCloneFrame( m_pItemTemplet->GetAttachFrameName(i) );
//#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX 
//			if( NULL != pFrame )
//			{
//				m_pUnitMatrix[i] = &pFrame->combineMatrix;
//			}//if
//		}//if
//	}//for
//
//}//CX2Eqip::InitAttatchNormal()
////}}AFX
//#endif // EQUIP_BACKGROUND_LOADING_TEST // 2008-12-14


// 캐시 무기 장착했을 때 ED 무기 강화 이펙트 보이게 만들게 하기 위한 함수
void CX2Eqip::ChangeEnchantLevel( int level )
{
	m_EnchantLevel = level;

	if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
	{
		for( int i=0; i<CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
			// 일단은 무기만 처리~
			if( NULL != m_pNormalXSkinAnim[i] )
			{
				if( NULL == m_pNormalMultiTexXET )
				{
					WCHAR wszText[256];
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MULTI_TEX_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
					m_pNormalMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszText, true );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
					m_pNormalXSkinAnim[i]->ReplaceMultiTexXET( 0, m_pNormalMultiTexXET );	// June2017 3295f336: void, the skin anim closes the old XET
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
					CKTDXDeviceXET* pOldXET = m_pNormalXSkinAnim[i]->ReplaceMultiTexXET( 0, m_pNormalMultiTexXET );
					SAFE_CLOSE( pOldXET );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
				}
			}
		}

#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
		// June2017 3295f250 @3295f348-3295f399: the June texture by level (this branch is level >= 7).
		if( NULL != m_pNormalMultiTexXET )
			SetEnchantStage2( m_pNormalMultiTexXET, m_EnchantLevel, "ChangeEnchantLevel", m_ItemID );
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
		if( NULL != m_pNormalMultiTexXET )
		{
			if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
#ifdef ADD_UPGRADE_WEAPON_PARTICLE		/// 11강 무기 이펙트
			m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "11Gang_Protect.tga", D3DTOP_ADD );
#else ADD_UPGRADE_WEAPON_PARTICLE
			m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
#endif ADD_UPGRADE_WEAPON_PARTICLE
			else
				m_pNormalMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );	
		}	
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN
	}
	else
	{
		if( m_pNormalMultiTexXET != NULL )
			m_pNormalMultiTexXET->DeleteMultiTexStage2();
	}

	if( m_bCanHyperMode == true )
	{
		if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_1 )
		{
			for( int i=0; i<CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
			{
				// 일단은 무기만 처리~
				if( NULL != m_pHyperXSkinAnim[i] )
				{
					if( NULL == m_pHyperMultiTexXET )
					{
						WCHAR wszText[256];
						StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MULTI_TEX_HYPER_%s_%dl", m_ModelName[0].c_str(), m_ItemUID );
						m_pHyperMultiTexXET = g_pKTDXApp->GetDeviceManager()->OpenXET( wszText, true );
#ifdef SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
						m_pHyperXSkinAnim[i]->ReplaceMultiTexXET( 0, m_pHyperMultiTexXET );	// June2017 3295f485: void, the skin anim closes the old XET
#else SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
						CKTDXDeviceXET* pOldXET = m_pHyperXSkinAnim[i]->ReplaceMultiTexXET( 0, m_pHyperMultiTexXET );
						SAFE_CLOSE( pOldXET );
#endif SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS
					}
				}
			}

#ifdef SERV_IRUHADEV_JUNE_EQIP_SKIN
			// June2017 3295f250 @3295f497-3295f4e8: the same choice for the hyper XET.
			if( NULL != m_pHyperMultiTexXET )
				SetEnchantStage2( m_pHyperMultiTexXET, m_EnchantLevel, "ChangeEnchantLevelHyper", m_ItemID );
#else SERV_IRUHADEV_JUNE_EQIP_SKIN
			if( NULL != m_pHyperMultiTexXET )
			{
				if( m_EnchantLevel >= ENCHANT_WEAPON_EFFECT_LEVEL_2 )
#ifdef ADD_UPGRADE_WEAPON_PARTICLE		/// 11강 무기 이펙트
				m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "11Gang_Protect.tga", D3DTOP_ADD );
#else ADD_UPGRADE_WEAPON_PARTICLE
				m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect.tga", D3DTOP_ADD );
#endif ADD_UPGRADE_WEAPON_PARTICLE
				else
					m_pHyperMultiTexXET->SetMultiTexStage2( "*.*", "Gate_Protect_G01.tga", D3DTOP_ADD );	
			}	
#endif SERV_IRUHADEV_JUNE_EQIP_SKIN
		}
		else
		{
			if( m_pHyperMultiTexXET != NULL )
				m_pHyperMultiTexXET->DeleteMultiTexStage2();
		}
	}
}

void CX2Eqip::SetOwnerUnit( CX2Unit* pUnit ) 
{ 
	m_pOwnerUnit = pUnit; 
}





// 이브 투명화 될 때 장비에 붙은 파티클 안보이게 하기 위해
void CX2Eqip::SetShowAttachedParticle( bool bShow )
{
	m_AttachedParticleGroup.SetShowObject( bShow );
}

void    CX2Eqip::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	const WCHAR* name = NULL;

	name = GetChangeTexName();
	if( name != NULL && wcslen( name ) > 3 )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, name );
	}

	for( int j=0; j<CX2Item::MAX_MODEL_COUNT_A_ITEM; j++ )
	{
		name = GetModelName(j);
		if( name != NULL && wcslen( name ) > 3 )
		{
			switch( GetEqipType() )
			{

			case CX2Item::UT_SKIN:
				g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, name );
				break;

			case CX2Item::UT_ATTACH_ANIM:
				g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, name );
				break;

			case CX2Item::UT_ATTACH_NORMAL:
				g_pKTDXApp->GetDeviceManager()->AppendXMesh( listInOut_, name );
				break;
			}
		}
	} // for(j)
	name = GetAniXETName();
	if( name != NULL && wcslen( name ) > 3 )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, name );
	}

	if( CanHyperMode() == true )
	{
		name = GetChangeTexNameHyper();
		if( name != NULL && wcslen( name ) > 3 )
		{
			g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, name );
		}

		for( int j=0; j<CX2Item::MAX_MODEL_COUNT_A_ITEM; j++ )
		{
			name = GetModelNameHyper(j);
			if( name != NULL && wcslen( name ) > 3 )
			{
				switch( GetEqipType() )
				{
				case CX2Item::UT_SKIN:
					g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, name );
					break;

				case CX2Item::UT_ATTACH_ANIM:
					g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, name );
					break;

				case CX2Item::UT_ATTACH_NORMAL:
					g_pKTDXApp->GetDeviceManager()->AppendXMesh( listInOut_, name );
					break;
				}
			}
		} // for(j) 

		name = GetAniXETNameHyper();
		if( name != NULL && wcslen( name ) > 3 )
		{
			g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, name );
		}
	}

	switch( GetEqipType() )
	{
	case CX2Item::UT_SKIN:
#ifdef	ATTACH_SKINMESH
		AppendAttachedToDeviceList( listInOut_ );
#endif	ATTACH_SKINMESH
		break;
	case CX2Item::UT_ATTACH_ANIM:
		AppendAttachedToDeviceList( listInOut_ );
		break;
	case CX2Item::UT_ATTACH_NORMAL:
		break;
	}//switch
}//CX2Eqip::AppendToDeviceList()

void CX2Eqip::EquipResourceLoad() 
{
	switch( m_EqipType )
	{
	case CX2Item::UT_SKIN:
		InitSkin();
#ifdef ATTACH_SKINMESH
		if( false == m_bLoadedAttached )
		{
			LoadAttached();
		}
#endif
		break;

	case CX2Item::UT_ATTACH_ANIM:
		{
			InitAttatchAnim();
			if( false == m_bLoadedAttached )
			{
				LoadAttached();
			}

		} break;

	case CX2Item::UT_ATTACH_NORMAL:
		InitAttatchNormal();
		break;
	}

}

void	CX2Eqip::AppendAttachedToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	if ( m_pItemTemplet == NULL )
		return;


#ifdef SUB_EQUIP_TEST
    DWORD   dwNumMD = m_pItemTemplet->GetNumAttachedMeshData();
    bool    bSkinMesh = m_pItemTemplet->GetUseType() == CX2Item::UT_SKIN;
	for( UINT i=0; i<dwNumMD; i++ )
	{

        const wchar_t*  pwszMeshName = L"";
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
        m_pItemTemplet->GetAttachedMeshData(i, &pwszMeshName);
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
        const CX2Item::AttachedMeshData& data = m_pItemTemplet->GetAttachedMeshData(i);
        pwszMeshName = data.wstrMeshName.c_str();
        bSkinMesh = data.bSkinMesh;
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

		const wstring	wstrMeshName		= pwszMeshName;	/// 스트링으로 포맷 변환
		bool			bSubEqipSkinMesh	= true;			/// 각 부분의 매시가 스킨 매시인지 여부

		/// 스킨 매시가 아니면, 일반 매시로 처리해 준다. ( Item의 ATTATCHED_MESH 기능 중 y 파일 사용을 위한 예외 처리 )
		if( 'y' == wstrMeshName.back() || 'Y' == wstrMeshName.back() )
			bSubEqipSkinMesh = false;

		if ( true == bSubEqipSkinMesh && ( true == bSkinMesh || true == IsAttachedSkinMeshForAttachAnim( m_pItemTemplet->GetItemID() ) ) )
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, pwszMeshName );
		else
			g_pKTDXApp->GetDeviceManager()->AppendXMesh( listInOut_, pwszMeshName );
	}//for
#endif SUB_EQUIP_TEST


#ifdef EQUIP_ATTACHED_PARTICLE_TEST

	// fix!! guuser쪽에 weapon particle 코드랑 합쳐서 정리해야함
	switch( m_EqipPos )
	{
	case CX2Unit::EP_AC_TITLE:			
	case CX2Unit::EP_AC_HAIR:			
	case CX2Unit::EP_AC_FACE1:			
	case CX2Unit::EP_AC_FACE2:			
	case CX2Unit::EP_AC_FACE3:			
	case CX2Unit::EP_AC_BODY:			
	case CX2Unit::EP_AC_LEG:			
	case CX2Unit::EP_AC_ARM:			
	case CX2Unit::EP_AC_RING:			
	case CX2Unit::EP_AC_NECKLESS:		
		{
			if( 
                NULL != m_pItemTemplet 
                && g_pData->GetUIMajorParticle() != NULL )
			{
                DWORD   dwNumParticles = m_pItemTemplet->GetNumAttachedParticleData();
				for( UINT j=0; j<dwNumParticles; j++ )
				{
                    const wchar_t*  pwszParticleName = L"";
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    m_pItemTemplet->GetAttachedParticleData( j, &pwszParticleName );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					const CX2Item::AttachedParticleData& particleData = m_pItemTemplet->m_vecAttachedParticleData[j];
                    pwszParticleName = particleData.wstrParticleName.c_str();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					g_pData->GetUIMajorParticle()->AppendToDeviceList( listInOut_,  pwszParticleName );
				}
			}

		} break;
	}

#endif EQUIP_ATTACHED_PARTICLE_TEST

}//CX2Eqip::AppendAttachedToDeviceList()

// note!! 일단 무기에만 UT_ATTACH_ANIM인 장비에만 붙일 수 있는 것으로 가정하고 만들었습니다. 그외 다른 장비에는 작동 안함
void CX2Eqip::LoadAttached()
{
	if( true == m_bLoadedAttached )
		return; 

#ifndef ATTACH_SKINMESH
	if( NULL == m_pNormalXSkinAnim[0] ) // fix!! 일단 0번째 메시에만 붙이는 것으로
		return; 
#endif

	m_bLoadedAttached = true;



#ifdef SUB_EQUIP_TEST

    DWORD   dwNumMD = m_pItemTemplet->GetNumAttachedMeshData();
    bool bSkinMesh = m_pItemTemplet->GetUseType() == CX2Item::UT_SKIN;
    bool bJiggle = false;
	for( UINT i=0; i<dwNumMD; i++ )
	{
        const wchar_t* pwszMeshName = L"";
        const wchar_t* pwszBoneName = L"";

#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
        m_pItemTemplet->GetAttachedMeshData(i,&pwszMeshName,&pwszBoneName,&bJiggle);
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
		const CX2Item::AttachedMeshData& data = m_pItemTemplet->m_vecAttachedMeshData[i];
        pwszMeshName = data.wstrMeshName.c_str();
        pwszBoneName = data.wstrBoneName.c_str();
        bSkinMesh = data.bSkinMesh;
        bJiggle = data.bJiggle;
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

#ifdef ATTACH_SKINMESH
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
		if( bSkinMesh == true )
		{
			if( m_pUnitXSkinAnim == NULL )
				continue;
            pFrame = m_pUnitXSkinAnim->GetCloneFrame( pwszBoneName );
		}
		else
		{
			if( m_pNormalXSkinAnim[0] == NULL )
				continue;

            pFrame = m_pNormalXSkinAnim[0]->GetCloneFrame( pwszBoneName );
			if( CX2Unit::UT_EVE == m_pItemTemplet->GetUnitType() )
			{
				if( i == 1 && NULL != m_pNormalXSkinAnim[1] )
				{
                    pFrame = m_pNormalXSkinAnim[1]->GetCloneFrame( pwszBoneName );
				}
			}
		}
#else
		// fix!! 일단은 요렇게 무기의 첫번째 메시에만 붙는 것으로 하드코딩
        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pNormalXSkinAnim[0]->GetCloneFrame( pwszBoneName );
		if( CX2Unit::UT_EVE == m_pItemTemplet->GetUnitType() )
		{
			if( i == 1 && NULL != m_pNormalXSkinAnim[1] )
			{
                pFrame = m_pNormalXSkinAnim[1]->GetCloneFrame( pwszBoneName );
			}
		}
#endif


		if( NULL != pFrame )
		{
#ifdef ATTACH_SKINMESH
			CX2SubEquip* pSubEquip = NULL;

			if( true == IsAttachedSkinMeshForAttachAnim( m_pItemTemplet->GetItemID()))
				pSubEquip = CX2SubEquip::CreateSubEquip( &pFrame->combineMatrix, pwszMeshName, true );
			else
				pSubEquip = CX2SubEquip::CreateSubEquip( &pFrame->combineMatrix, pwszMeshName, bSkinMesh );

			if( pSubEquip != NULL && bJiggle == true )
			{				
				pSubEquip->SetUnitXSkinAnim(m_pUnitXSkinAnim);
				pSubEquip->SetJiggle(true, 3);
			}
#else
			CX2SubEquip* pSubEquip = CX2SubEquip::CreateSubEquip( &pFrame->combineMatrix, pwszMeshName);
#endif			
			m_vecpSubEquip.push_back( pSubEquip );

			pSubEquip->SetManagerObject( this );
			g_pKTDXApp->GetDGManager()->AddObjectChain( pSubEquip );			

			pSubEquip->SetLayer( GetLayer() );


			// fix!! 일단 하드코딩 나중에 item_templet에서 읽어올 수 있게 추가해야함
            switch( m_pItemTemplet->GetItemID() )
			{
			default:	
				{					
					if( true == IsAttachedSkinMeshForAttachAnim( m_pItemTemplet->GetItemID()))
					{
						CKTDGXRenderer::RenderParam* pRenderParam = pSubEquip->GetRenderParam();

						if( NULL == pRenderParam )
							return; 

						switch( m_pItemTemplet->GetItemID() )
						{
						default:
							{
								pSubEquip->SetAlphaObject( true );
								pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
								pRenderParam->bAlphaBlend = true;
								pRenderParam->srcBlend	= D3DBLEND_SRCALPHA;
								pRenderParam->destBlend = D3DBLEND_DESTALPHA;
								pRenderParam->cullMode = D3DCULL_NONE;
								pRenderParam->bZWriteEnable = false;
							} break;
						case 88622:	/// 살바토르 벤투스 실론 윙 ( 엘소드 )
						case 88630:	/// 살바토르 벤투스 실론 윙 ( 아이샤 )
						case 88638:	/// 살바토르 벤투스 실론 윙 ( 레나 )
						case 88646:	/// 살바토르 벤투스 실론 윙 ( 레이븐 )
						case 88654:	/// 살바토르 벤투스 실론 윙 ( 이브 )
						case 88662:	/// 살바토르 벤투스 실론 윙 ( 청 )
						case 88670:	/// 살바토르 벤투스 실론 윙 ( 아라 )
						case 88678: /// 살바토르 벤투스 실론 윙 ( 엘리시스 )
							{
								pSubEquip->SetAlphaObject( true );
								pRenderParam->renderType		= CKTDGXRenderer::RT_REAL_COLOR;
								pRenderParam->bAlphaBlend		= true;
								pRenderParam->bAlphaTestEnable	= true;
								pRenderParam->cullMode			= D3DCULL_NONE;
							} break;
						case 82035:
						case 82051:
							{
								pSubEquip->SetAlphaObject( true );
								pRenderParam->renderType	= CKTDGXRenderer::RT_REAL_COLOR;
								pRenderParam->bAlphaBlend	= true;
							} break;
						}
					}
					else
					{
						pSubEquip->SetAlphaObject( true );
						pSubEquip->GetNormalRenderParam().renderType = CKTDGXRenderer::RT_REAL_COLOR;
						pSubEquip->GetNormalRenderParam().bAlphaBlend = true;
						pSubEquip->GetNormalRenderParam().srcBlend	= D3DBLEND_SRCALPHA;
						pSubEquip->GetNormalRenderParam().destBlend = D3DBLEND_DESTALPHA;
					}
				} break;

			case 130892: // SF 무기, 엘소드
			case 130893: // SF 무기
			case 130894: // SF 무기
			case 130895: // SF 무기
#ifdef REAL_COLOR_SPECIAL_RENDER_ITEM_WITH_ALPHA			
			case 81290:	 // 임규수 일본 추가 나소드 배틀 슈트 캐논 (청)
			case 88414: // 김석근 일본 추가 나소드 배틀 창(아라)
#endif //REAL_COLOR_SPECIAL_RENDER_ITEM_WITH_ALPHA			
			// 살바토르 솔레스
			case 131646: 
			case 131647:
			case 131648:
			case 131649:
			case 131650:
			case 180052:
			case 88120:
			case 132157:
			case 132163:
			case 132169:
			case 132175:
			case 132181:
				{					
					pSubEquip->SetAlphaObject( true );
					pSubEquip->GetNormalRenderParam().renderType = CKTDGXRenderer::RT_REAL_COLOR;
					pSubEquip->GetNormalRenderParam().bAlphaBlend = true;
					pSubEquip->GetNormalRenderParam().srcBlend	= D3DBLEND_SRCALPHA;
					pSubEquip->GetNormalRenderParam().destBlend = D3DBLEND_INVSRCALPHA;
					pSubEquip->GetNormalRenderParam().bAlphaTestEnable = true;
				} break;

			case 130896: // SF 무기, 이브
				{					
					pSubEquip->GetNormalRenderParam().renderType = CKTDGXRenderer::RT_REAL_COLOR;
				} break;

			// UT_SKIN에 붙는 attach mesh
			case 132851: // 이블 트레이서-베르드 룬 장갑(엘소드)
			case 88335: // 나소드 배틀 슈트 MK 2 상의
			case 88343:
			case 88351:
			case 88359:
			case 88367:
			case 88375:
			case 88383:
			case 88448: // 나소드 배틀 슈트 MK 2 상의 [PC방]
			case 88456:
			case 88464:
			case 88472:
			case 88480:
			case 88488:
			case 88496:
			case 88504: // 나소드 배틀 슈트 MK-B 상의
			case 88512:
			case 88520:
			case 88528:
			case 88536:
			case 88544:
			case 88552:
			case 88560:	// 나소드 배틀 슈트 MK 2 상의 (기간제)
			case 88568:
			case 88576:
			case 88584:
			case 88592:
			case 88600:
			case 88608:
				{
					if( NULL != pSubEquip )
					{
						pSubEquip->SetAlphaObject( true );
						CKTDGXRenderer::RenderParam* pRenderParam = pSubEquip->GetRenderParam();
						if( NULL != pRenderParam )
						{
							pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
							pRenderParam->bAlphaBlend = true;
							pRenderParam->srcBlend	= D3DBLEND_SRCALPHA;
							pRenderParam->destBlend = D3DBLEND_DESTALPHA;
							pRenderParam->bZWriteEnable = false;
						}
					}
				} break;
			case 181495:
			case 181496:
				{
					pSubEquip->SetAlphaObject( true );
					pSubEquip->GetNormalRenderParam().renderType = CKTDGXRenderer::RT_REAL_COLOR;
					pSubEquip->GetNormalRenderParam().bAlphaBlend = true;
					pSubEquip->GetNormalRenderParam().srcBlend	= D3DBLEND_SRCALPHA;
					pSubEquip->GetNormalRenderParam().destBlend = D3DBLEND_DESTALPHA;
					pSubEquip->GetNormalRenderParam().cullMode = D3DCULL_NONE;
					pSubEquip->GetNormalRenderParam().bZWriteEnable = false;
				} break;
//#ifdef SERV_EVE_BATTLE_SERAPH
			case 41290: // 배틀 세라프 프로모션 머리
				{
					CKTDGXRenderer::RenderParam* pRenderParam = pSubEquip->GetRenderParam();
					pSubEquip->SetAlphaObject( true );
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    if ( pRenderParam != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                    {
					    pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
					    pRenderParam->bAlphaBlend = true;
					    pRenderParam->srcBlend	= D3DBLEND_SRCALPHA;
					    pRenderParam->destBlend = D3DBLEND_DESTALPHA;
					    pRenderParam->bZWriteEnable = false;
					    pRenderParam->cullMode = D3DCULL_NONE;
                    }
				} break;
			case 41150: // 배틀 세라프 프로모션 무기
			case 81836: /// 살바토르 이벨른 문 스테프
			case 81869: /// 글레이셜 프린세스 크리스탈 무기(엘소드)
			case 81875: /// 글레이셜 프린세스 크리스탈 무기(아이샤)
			case 81881: /// 글레이셜 프린세스 크리스탈 무기(레나)
			case 81887:	/// 글레이셜 프린세스 크리스탈 무기(레이븐)
			case 81893: /// 글레이셜 프린세스 크리스탈 무기(이브)
			case 81899:	/// 글레이셜 프린세스 크리스탈 무기(청)
			case 81905:	/// 글레이셜 프린세스 크리스탈 무기(아라)
			case 88222: /// 글레이셜 프린스 크리스탈 소드(엘소드) //PC방
			case 88228: /// 글레이셜 프린세스 크리스탈 스태프(아이샤)//PC방
			case 88234: /// 글레이셜 프린세스 크리스탈 보우(레나)//PC방
			case 88240: /// 글레이셜 프린스 크리스탈 블레이드(레이븐)//PC방
			case 88246: /// 글레이셜 프린세스 크리스탈 기어(이브)//PC방
			case 88252: /// 글레이셜 프린스 크리스탈 캐논(청)//PC방
			case 88258: /// 글레이셜 프린세스 크리스탈 스피어(아라)//PC방
				{					
					pSubEquip->SetAlphaObject( true );
					pSubEquip->GetNormalRenderParam().renderType = CKTDGXRenderer::RT_REAL_COLOR;
					pSubEquip->GetNormalRenderParam().bAlphaBlend = true;
					pSubEquip->GetNormalRenderParam().srcBlend	= D3DBLEND_SRCALPHA;
					pSubEquip->GetNormalRenderParam().destBlend = D3DBLEND_DESTALPHA;
					pSubEquip->GetNormalRenderParam().cullMode = D3DCULL_NONE;
					pSubEquip->GetNormalRenderParam().bZWriteEnable = true;
					pSubEquip->GetNormalRenderParam().bAlphaTestEnable = true;
				} break;
//#endif SERV_EVE_BATTLE_SERAPH

#ifdef ADDED_RELATIONSHIP_SYSTEM
			case 183207: /// 핑크 드레스 ( 아이샤 )
			case 183208: /// 핑크 드레스 ( 레나 )
			case 183210: /// 핑크 드레스 ( 이브 )
			case 183212: /// 핑크 드레스 ( 아라 )
			case 183213: /// 핑크 드레스 ( 엘리시스 )
#ifdef SERV_NEW_ONE_PIECE_AVATAR_SLOT
			case 67006491: /// 핑크 드레스 ( 아이샤 )
			case 67006492: /// 핑크 드레스 ( 레나 )
			case 67006494: /// 핑크 드레스 ( 이브 )
			case 67006496: /// 핑크 드레스 ( 아라 )
			case 67006497: /// 핑크 드레스 ( 엘리시스 )
#endif //SERV_NEW_ONE_PIECE_AVATAR_SLOT
				{	
					pSubEquip->SetAlphaObject( true );
					if( true == pSubEquip->GetIsSkinMesh() )
					{
                        CKTDGXRenderer::RenderParam* pRenderParam = pSubEquip->GetRenderParam();
#ifdef  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                        if ( pRenderParam != NULL )
#endif  X2OPTIMIZE_X2EQIP_CRASH_BUG_FIX
                        {
						    pRenderParam->bAlphaBlend	= true;
						    pRenderParam->renderType		= CKTDGXRenderer::RT_REAL_COLOR;
						    pRenderParam->cullMode		= D3DCULL_NONE;
						    pRenderParam->bZWriteEnable	= false;
                        }
					}
					else
					{
						pSubEquip->GetNormalRenderParam().bAlphaBlend	= true;
						pSubEquip->GetNormalRenderParam().renderType		= CKTDGXRenderer::RT_REAL_COLOR;
						pSubEquip->GetNormalRenderParam().cullMode		= D3DCULL_NONE;
						pSubEquip->GetNormalRenderParam().bZWriteEnable	= false;
					}
				} break;
#endif ADDED_RELATIONSHIP_SYSTEM

			case 88615:	/// 살바토르 벤투스 소드 ( 엘소드 )
			case 88623: /// 살바토르 벤투스 완드 ( 아이샤 )
			case 88631: /// 살바토르 벤투스 보우 ( 레나 )
			case 88639: /// 살바토르 벤투스 블레이드 ( 레이븐 )
			case 88647: /// 살바토르 벤투스 기어 ( 이브 )
			case 88655: /// 살바토르 벤투스 캐논 ( 청 )
			case 88663: /// 살바토르 벤투스 스피어 ( 아라 )
			case 88671: /// 살바토르 벤투스 클레이모어 ( 엘리시스 )
				{					
					pSubEquip->SetAlphaObject( true );
					pSubEquip->GetNormalRenderParam().renderType	= CKTDGXRenderer::RT_REAL_COLOR;
					pSubEquip->GetNormalRenderParam().bAlphaBlend	= true;
					pSubEquip->GetNormalRenderParam().cullMode		= D3DCULL_NONE;
				} break;
			}
		}
	}

#endif SUB_EQUIP_TEST






#ifdef EQUIP_ATTACHED_PARTICLE_TEST

	// fix!! guuser쪽에 weapon particle 코드랑 합쳐서 정리해야함
	switch( m_EqipPos )
	{
	case CX2Unit::EP_AC_TITLE:			
	case CX2Unit::EP_AC_HAIR:			
	case CX2Unit::EP_AC_FACE1:			
	case CX2Unit::EP_AC_FACE2:			
	case CX2Unit::EP_AC_FACE3:			
	case CX2Unit::EP_AC_BODY:			
	case CX2Unit::EP_AC_LEG:			
	case CX2Unit::EP_AC_ARM:			
	case CX2Unit::EP_AC_RING:			
	case CX2Unit::EP_AC_NECKLESS:		
		{
			if( NULL != m_pItemTemplet )
			{
                DWORD   dwNumParticles = m_pItemTemplet->GetNumAttachedParticleData();
				for( UINT j=0; j<dwNumParticles; j++ )
				{
                    const wchar_t*  pwszParticleName = L"";
                    const wchar_t*  pwszBoneName = L"";
#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    m_pItemTemplet->GetAttachedParticleData( j, &pwszParticleName, &pwszBoneName );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					const CX2Item::AttachedParticleData& particleData = m_pItemTemplet->GetAttachedParticleData(j);
                    pwszParticleName = particleData.wstrParticleName.c_str();
                    pwszBoneName = particleData.wstrBoneName.c_str();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
					AttachParticle( true, pwszParticleName, pwszBoneName, D3DXVECTOR3(0, 0, 0 ) );
				}
			}

		} break;
	}
#endif EQUIP_ATTACHED_PARTICLE_TEST




	
}





#ifdef EQUIP_ATTACHED_PARTICLE_TEST


	void CX2Eqip::AttachParticle( bool bIsMajorParticle, const wstring& seqName, const wstring& boneName, D3DXVECTOR3 vOffsetPos )
	{
		if( true == seqName.empty() )
			return;

		if( true == boneName.empty() )
			return;

		if( NULL == g_pData->GetUIMajorParticle() ||
			NULL == g_pData->GetUIMinorParticle() )
			return;

		if( NULL == m_pNormalXSkinModel[0] )
			return;



		AttachedParticle* pAttachedParticle = new AttachedParticle;
		pAttachedParticle->m_bIsMajorParticle = bIsMajorParticle;
		pAttachedParticle->m_vOffsetPos = vOffsetPos;
		pAttachedParticle->m_wstrBoneName = boneName;


		if( true == bIsMajorParticle )
		{
			pAttachedParticle->m_hSeq = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  seqName.c_str(), D3DXVECTOR3( 0, 0, 0 ) );
#ifdef  X2OPTIMIZE_CULLING_PARTICLE
            CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
            if ( pSeq != NULL )
                pSeq->SetRenderCullCheck( false );
#endif  X2OPTIMIZE_CULLING_PARTICLE
		}
		else
		{
			pAttachedParticle->m_hSeq = g_pData->GetUIMinorParticle()->CreateSequenceHandle( NULL,  seqName.c_str(), D3DXVECTOR3( 0, 0, 0 ) );
#ifdef  X2OPTIMIZE_CULLING_PARTICLE
            CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
            if ( pSeq != NULL )
                pSeq->SetRenderCullCheck( false );
#endif  X2OPTIMIZE_CULLING_PARTICLE
		}


		if( INVALID_PARTICLE_SEQUENCE_HANDLE == pAttachedParticle->m_hSeq )
		{
			SAFE_DELETE( pAttachedParticle );
			return;
		}

		//CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
		//if( NULL != pSeq )
		//{
		//	pSeq->SetShowObject( GetShowObject() );
		//}

		string strBoneName = "";
		ConvertWCHARToChar( strBoneName, boneName );
		

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pNormalXSkinModel[0]->FindFrame( strBoneName.c_str() );
		if( NULL == pFrame )
		{
			if( NULL != m_pNormalXSkinModel[1] )
			{
				pFrame = m_pNormalXSkinModel[1]->FindFrame( strBoneName.c_str() );
				if( NULL == pFrame )
				{
					SAFE_DELETE( pAttachedParticle );
					return;
				}
			}
			else
			{
				SAFE_DELETE( pAttachedParticle );
				return;
			}
		}
		pAttachedParticle->m_pAttachFrame = pFrame;


		m_AttachedParticleGroup.m_vecpAttachedParticle.push_back( pAttachedParticle );
	}


	
	//////////////////////////////////////////////////////////////////////////
	CX2Eqip::AttachedParticle::AttachedParticle()
	{
		m_pAttachFrame			= NULL;
		m_wstrBoneName			= L"";
		m_bIsMajorParticle		= true;
		m_vOffsetPos			= D3DXVECTOR3(0, 0, 0);
		m_hSeq					= INVALID_PARTICLE_SEQUENCE_HANDLE;
	}


	CX2Eqip::AttachedParticle::~AttachedParticle()
	{
		if( true == m_bIsMajorParticle )
		{
			g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeq );
		}
		else
		{
			g_pData->GetUIMinorParticle()->DestroyInstanceHandle( m_hSeq );
		}
	}



	//////////////////////////////////////////////////////////////////////////
	CX2Eqip::AttachedParticleGroup::AttachedParticleGroup()
	{

	}

	CX2Eqip::AttachedParticleGroup::~AttachedParticleGroup()
	{
		for( UINT i=0; i<m_vecpAttachedParticle.size(); i++ )
		{
			AttachedParticle* pAttachedParticle = m_vecpAttachedParticle[i];
			SAFE_DELETE( pAttachedParticle );
		}
		m_vecpAttachedParticle.clear();
	}

	void CX2Eqip::AttachedParticleGroup::OnFrameMove( double fTime, float fElapsedTime )
	{
		for( UINT i=0; i< m_vecpAttachedParticle.size(); i++ )
		{
			AttachedParticle* pAttachedParticle = m_vecpAttachedParticle[i];
			CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

			if( true == pAttachedParticle->m_bIsMajorParticle )
			{
				pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}
			else
			{
				pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}

			if( NULL == pSeq )
			{
				m_vecpAttachedParticle.erase( m_vecpAttachedParticle.begin() + i );
				--i;
				continue;
			}

			// fix!! 일단은 첫번째 모델에만 붙일 수 있는 것으로 구현
			// 일단 UT_ATTACH_ANIM 타입만 구현 
			D3DXVECTOR3 vAttachPos(0,0,0);

			//switch( m_EqipType )
			//{
			//case CX2Item::UT_SKIN:
			//	{
			//		// not yet!!
			//	} break;

			//case CX2Item::UT_ATTACH_ANIM:
			//	{
					if( NULL != pAttachedParticle->m_pAttachFrame )
					{
						GetFramePos( &vAttachPos, pAttachedParticle->m_pAttachFrame );
					}
			//	} break;

			//case CX2Item::UT_ATTACH_NORMAL:
			//	{
			//		// fix!! mesh의 frame의 위치는 어떻게 가져오나?
			//		// not yet!!

			//	} break;
			//}

			vAttachPos += pAttachedParticle->m_vOffsetPos;



#ifdef EQUIP_ATTACHED_PARTICLE_TRACE_TEST
			if ( true == pSeq->GetTrace() )
				pSeq->SetPosition( vAttachPos, true, true );
			else
				pSeq->SetPosition( vAttachPos, false, false );
#else EQUIP_ATTACHED_PARTICLE_TRACE_TEST
//{{AFX
			pSeq->SetPosition( vAttachPos );
//}}AFX
#endif EQUIP_ATTACHED_PARTICLE_TRACE_TEST
		}

	}

	void CX2Eqip::AttachedParticleGroup::SetShowObject( bool bShow )
	{
		for( UINT i=0; i< m_vecpAttachedParticle.size(); i++ )
		{
			AttachedParticle* pAttachedParticle = m_vecpAttachedParticle[i];
			CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

			if( true == pAttachedParticle->m_bIsMajorParticle )
			{
				pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}
			else
			{
				pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
			}

			if( NULL != pSeq )
			{
				pSeq->SetShowObject( bShow );
			}
		}

	}
#endif EQUIP_ATTACHED_PARTICLE_TEST

void CX2Eqip::SetFixed()
{
	m_bFixed = true;

#ifdef SUB_EQUIP_TEST
	for(vector<CX2SubEquip*>::iterator it = m_vecpSubEquip.begin(); it<m_vecpSubEquip.end(); ++it)
	{
		CX2SubEquip* pSubEquip = *it;
		// DGManager Chain에서 안 그려지게 하기 위함. 직접 Render 해 준다.
		pSubEquip->SetShowObject(false);
	}
#endif


#ifdef EQUIP_ATTACHED_PARTICLE_TEST

	for(vector<AttachedParticle*>::iterator itParticle = m_AttachedParticleGroup.m_vecpAttachedParticle.begin();
		itParticle < m_AttachedParticleGroup.m_vecpAttachedParticle.end();
		++itParticle)
	{
		AttachedParticle* pAttachedParticle = *itParticle;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		if( true == pAttachedParticle->m_bIsMajorParticle )
		{
			pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
		}
		else
		{
			pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pAttachedParticle->m_hSeq );
		}
		// DGManager Chain에서 안 그려지게 하기 위함. 직접 Render 해 준다.
		if( pSeq != NULL )
		{
			pSeq->SetShowObject(false);
		}		
	}

#endif EQUIP_ATTACHED_PARTICLE_TEST
}

/** @function : ChangeAnim
	@brief : 장비의 애니메이션을 변경하는 함수\
	@param : 변경할 애니메이션 명(pwszAnimName_)
*/
void CX2Eqip::ChangeAnim( const WCHAR* pwszAnimName_, const bool bTransition_ )
{
	if ( CX2Item::UT_ATTACH_ANIM == m_EqipType )
	{
		for( int i = 0; i < CX2Item::MAX_MODEL_COUNT_A_ITEM; i++ )
		{
			if ( NULL != m_pNormalXSkinAnim[i] )
				m_pNormalXSkinAnim[i]->ChangeAnim( pwszAnimName_, bTransition_ );

			if ( m_bCanHyperMode && NULL != m_pHyperXSkinAnim[i] )
				m_pHyperXSkinAnim[i]->ChangeAnim( pwszAnimName_, bTransition_ );
		}
	}
}

#ifdef ADD_UPGRADE_WEAPON_PARTICLE
float CX2Eqip::GetUpgradeWeaponUVAnimatonSpeed()		/// 강화 아이템 UV 애니메이션 속도
{
	if( ENCHANT_WEAPON_EFFECT_LEVEL_2 <= m_EnchantLevel )	/// 11강 이상
		return 0.75f;
	else
		return 1.f;
}
#endif ADD_UPGRADE_WEAPON_PARTICLE