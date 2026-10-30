/** @file : X2BuffTemplet.cpp
    @breif : 버프의 행동과 종료등을 가지고 있는 클래스 정의 파일
*/
#pragma once

#include "StdAfx.h"
#include ".\X2BuffTemplet.h"
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/// June buff port 4b-ii: June's buff code raises no popup (the user's 2026-10-04 decision: follow June, always
/// log). Every DISPLAY_ERROR in this file is collected by an active capture, else written once per buff and
/// text as a [JUNE_BUFF] line in ErrorLog.txt (CX2JuneBuffCapture::RuntimeError). Same trailing ';' as the original.
#undef DISPLAY_ERROR
#define DISPLAY_ERROR( wszError_ ) CX2BuffCapture::RuntimeError( wszError_ );
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : ParsingScript
	@brief : 버프 템플릿 스크립트을 읽어들임
	@param : 현재 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공 true, 실패 false 리턴
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
/// June buff port Phase 4a: June's CX2BuffTemplet::ParsingScript (3236a9b0), whole. Key order,
/// required keys and defaults are June's. Differences from March: IS_STORE_BUFF,
/// ACCUMULATION_DELAY_TIME, NOT_SAVE_GAGEMANAGER, IS_INSTANCE_KILL_DEBUFF,
/// BUFF_REPLACE_DETAIL_OPERATION (limit < 2) and ACCUMULATION_DECREASE_* (limit >= 2) are read,
/// and ACCUMULATION_MULTIPLIER is optional (1.0). EXP_RATE, IS_PARTY_BUFF,
/// NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER, DROP_RATE, VP_RATE and LUCKYTIMINGEVENT_* are not read by
/// June's client either (server-side keys).
bool CX2BuffTemplet::ParsingScript( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_TEMPLET_ID", m_BuffIdentity.m_eBuffTempletID,
		BUFF_TEMPLET_ID, BTI_NONE, return DISPLAY_ERROR( L"BUFF_TEMPLET_ID" ) );

	LUA_GET_VALUE_RETURN( luaManager_, "IS_WORLD_BUFF", m_bWorldBuff, false, return DISPLAY_ERROR( L"IS_WORLD_BUFF" ) );
	LUA_GET_VALUE_RETURN( luaManager_, "IS_DEBUF", m_bDeBuff, false, return DISPLAY_ERROR( L"IS_DEBUF" ) );
	LUA_GET_VALUE( luaManager_, "IS_STORE_BUFF", m_bStoreBuff, false );

	LUA_GET_VALUE_RETURN( luaManager_, "ACCUMULATION_LIMIT", m_uiAccumulationLimit, 0, return DISPLAY_ERROR( L"ACCUMULATION_LIMIT" ) );
	LUA_GET_VALUE( luaManager_, "ACCUMULATION_DELAY_TIME", m_fAccumulationDelayTime, 0.0f );
	LUA_GET_VALUE( luaManager_, "NOT_SAVE_GAGEMANAGER", m_bNotSaveGageManager, false );
	LUA_GET_VALUE( luaManager_, "IS_INSTANCE_KILL_DEBUFF", m_bInstanceKillDebuff, false );

	if ( m_uiAccumulationLimit < 2 )	/// duplicate (0) or replace (1)
	{
		int iReplaceDetailOp = BRDO_ADD_TIME;
		LUA_GET_VALUE( luaManager_, "BUFF_REPLACE_DETAIL_OPERATION", iReplaceDetailOp, BRDO_ADD_TIME );
		m_eReplaceDetailOp = static_cast<BUFF_REPLACE_DETAIL_OP>( iReplaceDetailOp );
	}
	else	/// accumulation (2 or more)
	{
		float fAccumulationMultiplier = 1.0f;
		LUA_GET_VALUE( luaManager_, "ACCUMULATION_MULTIPLIER", fAccumulationMultiplier, 1.0f );
		m_mapUnCommonMember.insert( make_pair( UCM_ACCUMULATION_MULTIPLIER, fAccumulationMultiplier ) );
		m_mapUnCommonMember.insert( make_pair( UCM_ACCUMULATION_COUNT_NOW, 0.0f ) );

		LUA_GET_VALUE( luaManager_, "ACCUMULATION_DECREASE_WHEN_FINISH", m_bAccumulationDecreaseWhenFinish, false );
		LUA_GET_VALUE( luaManager_, "ACCUMULATION_DECREASE_COUNT_WHEN_FINISH", m_uiAccumulationDecreaseCountWhenFinish, 1 );
	}

	float fRegistType = 0.0f;
	LUA_GET_VALUE_RETURN( luaManager_, "REGIST_TYPE", fRegistType, 0.0f, return DISPLAY_ERROR( L"REGIST_TYPE" ) );
	if ( 0.0f < fRegistType )
		m_mapUnCommonMember.insert( make_pair( UCM_REGIST_TYPE, fRegistType ) );

	if ( luaManager_.BeginTable( "BEHAVIOR" ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

		if ( !CX2BuffBehaviorTemplet::ParsingCombinationBehavior( luaManager_, m_vecBehaviorPtr ) )
			return false;
	}
	else
		return DISPLAY_ERROR( L"BEHAVIOR Templet doesn't exist" );

	if ( luaManager_.BeginTable( "FINALIZER" ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

		if ( !CX2BuffFinalizerTemplet::ParsingCombinationFinalizer( luaManager_, m_vecFinalizerPtr ) )
			return false;
	}
	else
		return DISPLAY_ERROR( L"FINALIZER Templet doesn't exist" );

	if ( luaManager_.BeginTable( "DISPLAYER" ) )	/// optional
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

		if ( !CX2BuffDisplayerTemplet::ParsingCombinationDisplayer( luaManager_, m_vecDisplayerPtr ) )
			return false;
	}

#ifdef BUFF_ICON_UI
	LUA_GET_VALUE( luaManager_, "BUFF_ICON_TEXTURE_NAME", m_wstIconFileName, L"" );
	m_bUseBuffIcon = ( true == m_wstIconFileName.empty() ) ? false : true ;
	if( true == m_bUseBuffIcon )
	{
		LUA_GET_VALUE( luaManager_, "BUFF_ICON_TEXTURE_KEY",  m_wstIconKeyName,  L"" );
		ASSERT( m_wstIconKeyName != L"" );

		LUA_GET_VALUE( luaManager_, "BUFF_NAME",			  m_iBuffName,		 STR_ID_2674 );
		LUA_GET_VALUE( luaManager_, "BUFF_DESC",			  m_iBuffDesc,		 STR_ID_2606 );
	}
#endif //BUFF_ICON_UI
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
bool CX2BuffTemplet::ParsingScript( KLuaManager& luaManager_ )
{
	/// 버프 아이디
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_TEMPLET_ID", m_BuffIdentity.m_eBuffTempletID, 
		BUFF_TEMPLET_ID, BTI_NONE, return DISPLAY_ERROR( L"BUFF_TEMPLET_ID" ) );

	/// 월드 여부
	LUA_GET_VALUE_RETURN( luaManager_, "IS_WORLD_BUFF", m_bWorldBuff, false, return DISPLAY_ERROR( L"IS_WORLD_BUFF" ) );

	/// 버프, 디버프 여부
	LUA_GET_VALUE_RETURN( luaManager_, "IS_DEBUF", m_bDeBuff, false, return DISPLAY_ERROR( L"IS_DEBUF" ) );

	/// 중복, 교체, 중첩 여부
	LUA_GET_VALUE_RETURN( luaManager_, "ACCUMULATION_LIMIT", m_uiAccumulationLimit, 0, return DISPLAY_ERROR( L"ACCUMULATION_LIMIT" ) );

	if ( 1 < m_uiAccumulationLimit )	/// 중첩 횟수가 정해져 있으면 (0 과 1은 중첩이 아님)
	{
		/// 중첩 배율을 읽어들임
		float fAccumulationMultiplier = 0.0f;
		LUA_GET_VALUE_RETURN( luaManager_, "ACCUMULATION_MULTIPLIER", fAccumulationMultiplier, 0, return DISPLAY_ERROR( L"ACCUMULATION_MULTIPLIER" ) );

		m_mapUnCommonMember.insert( make_pair( UCM_ACCUMULATION_MULTIPLIER, fAccumulationMultiplier ) );

		/// 현재 중첩 카운트
		m_mapUnCommonMember.insert( make_pair( UCM_ACCUMULATION_COUNT_NOW, 0.0f ) );
	}

	/// 저항 타입
	float fRegistType = 0.0f;
	LUA_GET_VALUE_RETURN( luaManager_, "REGIST_TYPE", fRegistType, 0.0f, return DISPLAY_ERROR( L"REGIST_TYPE" ) );

	/// 저항 타입이 ET_NONE(0) 이 아니면
	if ( 0.0f < fRegistType )
		m_mapUnCommonMember.insert( make_pair( UCM_REGIST_TYPE, fRegistType ) );


	/// 행동 요소 파싱
	if ( luaManager_.BeginTable( "BEHAVIOR" ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

		if ( !CX2BuffBehaviorTemplet::ParsingCombinationBehavior( luaManager_, m_vecBehaviorPtr ) )
			return false;		
	}
	else
		return DISPLAY_ERROR( L"BEHAVIOR Templet doesn't exist" );


	/// 종료 요소 파싱
	if ( luaManager_.BeginTable( "FINALIZER" ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

			if ( !CX2BuffFinalizerTemplet::ParsingCombinationFinalizer( luaManager_, m_vecFinalizerPtr ) )
				return false;		
	}
	else
		return DISPLAY_ERROR( L"FINALIZER Templet doesn't exist" );

	/// 종료 요소 파싱
	if ( luaManager_.BeginTable( "DISPLAYER" ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

			if ( !CX2BuffDisplayerTemplet::ParsingCombinationDisplayer( luaManager_, m_vecDisplayerPtr ) )
				return false;		
	}
	// Displayer는 없을 수도 있으므로 else 판단을 하지 않는다.

#ifdef BUFF_ICON_UI
	LUA_GET_VALUE( luaManager_, "BUFF_ICON_TEXTURE_NAME", m_wstIconFileName, L"" );
	m_bUseBuffIcon = ( true == m_wstIconFileName.empty() ) ? false : true ;
	if( true == m_bUseBuffIcon )
	{
		LUA_GET_VALUE( luaManager_, "BUFF_ICON_TEXTURE_KEY",  m_wstIconKeyName,  L"" );
		ASSERT( m_wstIconKeyName != L"" );

		LUA_GET_VALUE( luaManager_, "BUFF_NAME",			  m_iBuffName,		 STR_ID_2674 );
		LUA_GET_VALUE( luaManager_, "BUFF_DESC",			  m_iBuffDesc,		 STR_ID_2606 );
	}
#endif //BUFF_ICON_UI
	return true;
}
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY

/** @function : SetFactor
	@brief : 버프 요소를 받아서 버프 템플릿에 셋팅하고, 버프를 시작 시키는 함수
	@param : 버프 요소 (buffFactor_), 버프가 적용되야할 유닛(pGameUnit_)
	@return : 성공시 true, 실패시 false
*/
bool CX2BuffTemplet::SetFactor( const CX2BuffFactor& buffFactor_, CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	/// June 3236d400 @3236d400-3236d457, before the behaviors: the creator (CX2Game slot 50 with the factor's UID and
	/// !isUser; March's GetUserUnitByUID / GetNPCUnitByUID, as CX2BuffLiveCreatorFinalizer does) gives its
	/// GetPowerRate() to +0xbc; without a creator the old value stays. June also copies the UID and not-user flag
	/// to +0xb0/+0xb8, which March reads from the factor itself.
	if ( NULL != g_pX2Game )
	{
		CX2GameUnit* pCreatorGameUnit = NULL;
		if ( buffFactor_.GetIsUserUID() )
			pCreatorGameUnit = g_pX2Game->GetUserUnitByUID( buffFactor_.GetUidGameUnit() );
		else
			pCreatorGameUnit = g_pX2Game->GetNPCUnitByUID( static_cast<int>( buffFactor_.GetUidGameUnit() ) );

		if ( NULL != pCreatorGameUnit )
			m_fCreatorPowerRate = pCreatorGameUnit->GetPowerRate();
	}
	/// Phase 9c: June then stores the factor's creator UID (+0xb0/+0xb4) and !isUser (+0xb8), creator found or not
	/// (@3236d43b-3236d457). GetFactor sends them on with the power rate.
	m_uidCreator		= buffFactor_.GetUidGameUnit();
	m_bCreatorNotUser	= !buffFactor_.GetIsUserUID();
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
	/// 행동조건 셋팅
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		if ( NULL != ptrBehavior && ptrBehavior->SetFactor( buffFactor_, pGameUnit_, this ) )
			ptrBehavior->SetStart( true );	/// 정상 실행
		else
			return false;
	}

	/// 종료조건 셋팅
	if ( false == SetFactorToOnlyFinalizer( buffFactor_, pGameUnit_ ) )
		return false;

	if ( GetAccumulationLimit() > 1 )	/// 중첩 되는 것이라면 위의 항목들을 실행 후 중첩 카운트 올려줌
		IncreaseAccumulationCountNow();

	/// 표현조건 셋팅
	BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
	{
		if ( NULL != ptrDisplayer )
		{
			ptrDisplayer->StartDisplayer( pGameUnit_, this );
			ptrDisplayer->SetStart( true );
		}
	}

#ifdef BUFF_ICON_UI		
	if( true == GetUseBuffIcon() )
	{
		CX2GageUI::BuffIcon BuffIconData( buffFactor_.GetBuffTempletID(), GetIconFileName(), 
			GetIconKeyName(), GetBuffName(), GetBuffDesc() );
		CX2GageManager::GetInstance()->PushBuff( pGameUnit_->GetUnitUID(), BuffIconData, IsDeBuff() );
	}
#endif //BUFF_ICON_UI

	return true;
}

/** @function : SetFactorFromPacket
	@brief : 서버로 부터 받은 버프요소로 버프템플릿을 실행 시킴
	@param : 서버로부터 받은 버프요소(kBuffFactor_), 버프가 적용될 게임유닛(pGameUnit_)
	@return : 성공(true), 실패(false)
*/
bool CX2BuffTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	SetUniqueNum( kBuffFactor_.m_BuffIdentity.m_uiUniqueNum );

	if ( GetAccumulationLimit() > 1 )	/// 중첩 가능한 것이면
	{
		const float fAccumulationCountNow = static_cast<float>( kBuffFactor_.m_ucAccumulationCountNow );
		
		pair<MAP_UN_COMMON_MEMBER::iterator, bool> pairReturn
			= m_mapUnCommonMember.insert( make_pair( UCM_ACCUMULATION_COUNT_NOW, fAccumulationCountNow ) ); 

		/// 기존에 키값이 존재 했다면
		if ( !pairReturn.second )
			pairReturn.first->second = fAccumulationCountNow;
	}
	
	/// 행동조건 셋팅
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		if ( NULL != ptrBehavior && ptrBehavior->SetFactorFromPacketTemplateMothod( kBuffFactor_, pGameUnit_ ) )
			ptrBehavior->SetStart( true );	/// 정상 실행
		else
			return false;
	}

	/// 종료조건 셋팅
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, m_vecFinalizerPtr )
	{
		if ( NULL != ptrFinalizer && !ptrFinalizer->SetFactorFromPacketTemplateMothod( kBuffFactor_, pGameUnit_ ) )
			return false;
	}

	/// 표현조건 셋팅
	BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
	{
		if ( NULL != ptrDisplayer )
		{
			ptrDisplayer->StartDisplayer( pGameUnit_, this );
			ptrDisplayer->SetStart( true );
		}
	}
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	/// June buff port Phase 9c, June 3236c440 @3236c4f0: after the displayers June restores the creator from the
	/// factor (UID +0x28/+0x2c -> +0xb0/+0xb4, NPC flag +0x3d -> +0xb8, power rate +0x57 -> +0xbc), so a restored
	/// buff's next GetFactor sends the same creator on.
	m_uidCreator		= kBuffFactor_.m_uidMesGameUnit;
	m_bCreatorNotUser	= kBuffFactor_.m_bIsMesGameUnitNpc;
#ifdef SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
	m_fCreatorPowerRate		= kBuffFactor_.m_fCreatorPowerRate;
#endif SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

	return true;
}

/** @function : GetAccumulationCountNow
	@brief : 현재 몇 중첩인지 얻어옴
	@return : 현재 중첩 횟수 리턴(UINT, 중첩이 안되는 경우 0을 리턴)
*/
UINT CX2BuffTemplet::GetAccumulationCountNow() const
{
	if ( !m_mapUnCommonMember.empty() )
	{
 		MAP_UN_COMMON_MEMBER::const_iterator mItr = m_mapUnCommonMember.find( UCM_ACCUMULATION_COUNT_NOW );
 		if ( m_mapUnCommonMember.end() != mItr )
 			return static_cast<UINT>( mItr->second );
	}

	return 0;
}

/** @function : IncreaseAccumulationCountNow
	@brief : 중첩이 되는 경우 중첩 횟수 1증가
*/
void CX2BuffTemplet::IncreaseAccumulationCountNow()
{
	if ( !m_mapUnCommonMember.empty() )
	{
		MAP_UN_COMMON_MEMBER::iterator mItr = m_mapUnCommonMember.find( UCM_ACCUMULATION_COUNT_NOW );
		if ( m_mapUnCommonMember.end() != mItr )
			mItr->second += 1;
	}
}

/** @function : GetRegistType
	@brief : 적용될 저항 타입을 얻어옴
	@return : 저항타입(UINT)으로 주로 CX2EnchantItem::ENCHANT_TYPE으로 변환하여 사용
*/
UINT CX2BuffTemplet::GetRegistType() const
{
	if ( !m_mapUnCommonMember.empty() )
	{
		MAP_UN_COMMON_MEMBER::const_iterator mItr = m_mapUnCommonMember.find( UCM_REGIST_TYPE );
		if ( m_mapUnCommonMember.end() != mItr )
			return static_cast<UINT>( mItr->second );
	}

	return 0;
}

/** @function : GetAccumulationMultiflier
	@brief : 중첩 가능한 경우 중첩 배율을 얻어옴
	@return : 중첩 배율(float)
*/
float CX2BuffTemplet::GetAccumulationMultiflier() const
{
	if ( !m_mapUnCommonMember.empty() )
	{
		MAP_UN_COMMON_MEMBER::const_iterator mItr = m_mapUnCommonMember.find( UCM_ACCUMULATION_MULTIPLIER );
		if ( m_mapUnCommonMember.end() != mItr )
			return mItr->second;
	}

	DISPLAY_ERROR( L"UCM_ACCUMULATION_COUNT_NOW doesn't exist" );

	return 0.0f;
}

/** @function : OnFrameMoveBehavior
	@brief : 버프템플릿의 행동 요소들의 OnFrameMove 실행
	@param : 버프가 적용된 유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveBehavior( CX2GameUnit* pGameUnit_, float fElapsedTime )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveBehavior( CX2GameUnit* pGameUnit_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		if ( NULL != ptrBehavior )
        {
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
            ptrBehavior->OnFrameMove( pGameUnit_, this, fElapsedTime );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
			ptrBehavior->OnFrameMove( pGameUnit_, this );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
        }
	}
}

/** @function : OnFrameMoveFinalizerAndCheckFinish
	@brief : 버프템플릿의 종료 요소들의 OnFrameMove 실행 및 종료여부를 판단
	@param : 버프가 적용된 유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveFinalizerAndCheckFinish( CX2GameUnit* pGameUnit_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveFinalizerAndCheckFinish( CX2GameUnit* pGameUnit_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, m_vecFinalizerPtr )
	{
		if ( NULL != ptrFinalizer )
		{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
            ptrFinalizer->OnFrameMove( pGameUnit_, fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
			ptrFinalizer->OnFrameMove( pGameUnit_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
			/// June 3236c570: with ACCUMULATION_DECREASE_WHEN_FINISH a finished finalizer only lowers the
			/// stack count. The buff keeps running if the count stays above the decrease, the finalizer
			/// can restart (only BFT_TIME can) and the behaviors re-apply at the new count.
			if ( ptrFinalizer->DidFinish( pGameUnit_ ) )
			{
				if ( IsAccumulationDecreaseWhenFinish()
					&& DecreaseAccumulationCountNow( GetAccumulationDecreaseCountWhenFinish() )
					&& ptrFinalizer->RestartForAccumulationDecrease()
					&& ReapplyBehaviorForAccumulationDecrease( pGameUnit_ ) )
				{
					RestartDisplayerForAccumulationDecrease( pGameUnit_ );
					continue;
				}
				SetDidFinish( true );
			}
#else SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
			if ( ptrFinalizer->DidFinish( pGameUnit_ ) )
				SetDidFinish( true );
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
		}
	}
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER
	/// June 3236c570 @3236c5f9: a buff that has finished runs the BFT 15 hook here; its erase runs it again
	if ( GetDidFinish() )
		DoFinishStickShake( pGameUnit_ );
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER
}


/** @function : OnFrameMoveDisplayer
	@brief : 버프템플릿의 표현 요소들의 OnFrameMove 실행
	@param : 버프가 적용된 유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveDisplayer( CX2GameUnit* pGameUnit_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffTemplet::OnFrameMoveDisplayer( CX2GameUnit* pGameUnit_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	if ( !m_vecDisplayerPtr.empty() )
	{
		BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
		{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
            ptrDisplayer->OnFrameMove( pGameUnit_, fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
			ptrDisplayer->OnFrameMove( pGameUnit_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		}
	}
}

/** @function : DoFinishBehavior
	@brief : 현재 실행되고 있는 버프가 종료될 때 템플릿의 행동요소가 종료될 때 수행되야하는 함수들을 실행
	@param : 버프가 적용중인 게임유닛(pGameUnit_)
*/
void CX2BuffTemplet::DoFinishBehavior( CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		ptrBehavior->DoFinish( pGameUnit_, this );
	}
}

/** @function : DoFinishBehavior
	@brief : 현재 실행되고 있는 템플릿의 행동요소가 종료될 때 수행되야하는 함수들을 실행
	@param : 버프가 적용중인 게임유닛(pGameUnit_)
*/
void CX2BuffTemplet::DoFinishDisplayer( CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
	{
		ptrDisplayer->DoFinish( pGameUnit_, this );
	}
}

/** @function : GetFactor
	@brief : 현재 실행되고 있는 버프의 수행요소 정보를 얻어오는 함수(서버를 통해 패킷전송하기 위해)
	@param : 정보를 담을 저장소(kBuffFactor_), 버프를 가지고 있는 게임유닛(pGameUnit_)
*/
void CX2BuffTemplet::GetFactor( OUT KBuffFactor& kBuffFactor_, const CX2GameUnit* pGameUnit_ ) const
{
	if ( !GetDidFinish() )
	{
		BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptr, m_vecBehaviorPtr )
		{
			ptr->GetFactor( kBuffFactor_.m_vecBuffBehaviorFactors );
		}

		BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptr, m_vecFinalizerPtr )
		{
			ptr->GetFactor( kBuffFactor_.m_vecBuffFinalizerFactors, pGameUnit_ );
		}

		/// 표현조건은 난입자에게 전송할 필요가 없으므로 수행하지 않는다.(m_vecDisplayerPtr)

		kBuffFactor_.m_BuffIdentity = m_BuffIdentity;

		if ( GetAccumulationLimit() > 1 )
		{
			kBuffFactor_.m_fAccumulationMultiplier = GetAccumulationMultiflier();
			kBuffFactor_.m_ucAccumulationCountNow = GetAccumulationCountNow();
		}
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
		/// June buff port Phase 9c, June 32369f50 @3236a009: last, whatever the accumulation limit, the creator that
		/// SetFactor stored: UID +0xb0/+0xb4 -> +0x28/+0x2c, NPC flag +0xb8 -> +0x3d, power rate +0xbc -> +0x57.
		kBuffFactor_.m_uidMesGameUnit			= m_uidCreator;
		kBuffFactor_.m_bIsMesGameUnitNpc		= m_bCreatorNotUser;
#ifdef SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
		kBuffFactor_.m_fCreatorPowerRate	= m_fCreatorPowerRate;
#endif SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	}
}

/** @function : SetFactorToOnlyFinalizer
	@brief : 종료조건에만 팩터 적용(중첩카운트가 최대인 이후에도 종료조건은 계속 중첩에 의해서 갱신되도록 하기 위해 사용)
	@param : 버프 요소 (buffFactor_), 버프가 적용되야할 유닛(pGameUnit_)
	@return : 성공시 true, 실패시 false
*/
bool CX2BuffTemplet::SetFactorToOnlyFinalizer( const CX2BuffFactor& buffFactor_, CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// 종료조건 셋팅
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, m_vecFinalizerPtr )
	{
		if ( !ptrFinalizer->SetFactor( buffFactor_, pGameUnit_, this ) )
			return false;
	}
	return true;
}

/** @function : GetFinalizerTempletPtrList
	@brief : 종료조건들을 얻어오는 함수
	@param : 얻어온 종료조건을 담을 함수
*/
void CX2BuffTemplet::GetFinalizerTempletPtrList( OUT vector<CX2BuffFinalizerTempletPtr>& vecFinalizerPtr_ )
{
	vecFinalizerPtr_.reserve( m_vecFinalizerPtr.size() );
	vecFinalizerPtr_ = m_vecFinalizerPtr;
}

/** @function : SetFinalizerTempletPtrList
	@brief : 종료조건들을 셋팅 하는 함수
	@param : 세팅할 종료조건
*/
void CX2BuffTemplet::ChangeFinalizerTempletPtrList( const vector<CX2BuffFinalizerTempletPtr>& vecFinalizerPtr_ )
{
	m_vecFinalizerPtr.resize( vecFinalizerPtr_.size() );
	m_vecFinalizerPtr = vecFinalizerPtr_;
}


/** @function : ReserveToFinish
	@brief : 다음 체크시 종료되도록 예약 해놓는 함수
*/
void CX2BuffTemplet::ReserveToFinish()
{
	/// 종료조건 셋팅
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, m_vecFinalizerPtr )
	{
		if ( NULL != ptrFinalizer )
			ptrFinalizer->ReserveToFinish();
	}
}

//{{ Iruha : 2026-10-03 // June buff port Phase 4a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
/** @function : DecreaseAccumulationCountNow
	@brief : June 32369e40. Lowers the stack count by uiCount_ when more than that is stacked.
	@return : true if it was lowered
*/
bool CX2BuffTemplet::DecreaseAccumulationCountNow( const UINT uiCount_ )
{
	MAP_UN_COMMON_MEMBER::iterator mItr = m_mapUnCommonMember.find( UCM_ACCUMULATION_COUNT_NOW );
	if ( m_mapUnCommonMember.end() != mItr && static_cast<float>( uiCount_ ) < mItr->second )
	{
		mItr->second -= static_cast<float>( uiCount_ );
		return true;
	}

	return false;
}

/** @function : ReapplyBehaviorForAccumulationDecrease
	@brief : June 3236a930. Every behavior re-applies itself at the lowered stack count (behavior
		vtable slot 7: false in the base, ChangeStat rescales its stat).
	@return : the last behavior's result (false when there is no unit or no behavior)
*/
bool CX2BuffTemplet::ReapplyBehaviorForAccumulationDecrease( CX2GameUnit* pGameUnit_ )
{
	bool bResult = false;
	if ( NULL != pGameUnit_ )
	{
		BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
		{
			if ( NULL != ptrBehavior )
				bResult = ptrBehavior->OnAccumulationCountDecreased( pGameUnit_, this );
		}
	}
	return bResult;
}

/** @function : RestartDisplayerForAccumulationDecrease
	@brief : June 3236a190. Starts every displayer again after a decrease. June then refreshes the
		buff icon with the stack count (32489810); March's gage icon shows no count, so that is not ported.
*/
void CX2BuffTemplet::RestartDisplayerForAccumulationDecrease( CX2GameUnit* pGameUnit_ )
{
	BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
	{
		if ( NULL != ptrDisplayer )
		{
			ptrDisplayer->StartDisplayer( pGameUnit_, this );
			ptrDisplayer->SetStart( true );
		}
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : SetAccumulationCountNow
	@brief : June 3236a430, reached only from CX2GameUnit::SetBuffAccumulationCount (June-only callers).
		Every behavior re-applies itself at the new count (slot 3) and is marked started when that
		succeeds; then, only if the templet keeps a stack count, the count becomes iCount_ and every
		displayer starts again. June then refreshes the buff icon with the count (+0xa8 -> 32489810),
		which is not ported, as in RestartDisplayerForAccumulationDecrease: March's icon shows no count.
*/
void CX2BuffTemplet::SetAccumulationCountNow( const int iCount_, CX2GameUnit* pGameUnit_, const CX2BuffFactor& BuffFactor_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffRuntimeScope kBuffScope( static_cast<int>( GetBuffTempletID() ) );	/// 4b-ii: names this buff in a runtime [JUNE_BUFF] line
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		if ( NULL != ptrBehavior && ptrBehavior->OnAccumulationCountSet( BuffFactor_, pGameUnit_, this, iCount_ ) )
			ptrBehavior->SetStart( true );
	}

	MAP_UN_COMMON_MEMBER::iterator mItr = m_mapUnCommonMember.find( UCM_ACCUMULATION_COUNT_NOW );
	if ( m_mapUnCommonMember.end() == mItr )
		return;

	mItr->second = static_cast<float>( iCount_ );
	BOOST_FOREACH( CX2BuffDisplayerTempletPtr ptrDisplayer, m_vecDisplayerPtr )
	{
		if ( NULL != ptrDisplayer )
		{
			ptrDisplayer->StartDisplayer( pGameUnit_, this );
			ptrDisplayer->SetStart( true );
		}
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : DecreaseDurationByShake
	@brief : June 3236c990. Every finalizer's slot 11 (DecreaseDurationByShake) on a copy of the list; only the
		Time finalizer takes time off. June's OnFrameMoveBuff (@3229d744) calls it for my user unit while
		CX2Game +0xbae is set and bumps the +0x6dd0 sync counter on true: both are Phase 8's, so nothing calls it yet.
	@return : true when any finalizer took time off
*/
bool CX2BuffTemplet::DecreaseDurationByShake()
{
	bool bDecreased = false;
	vector<CX2BuffFinalizerTempletPtr> vecFinalizerPtr;
	GetFinalizerTempletPtrList( vecFinalizerPtr );
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, vecFinalizerPtr )
	{
		if ( NULL != ptrFinalizer && ptrFinalizer->DecreaseDurationByShake() )
			bDecreased = true;
	}
	return bDecreased;
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER

/** @function : DoFinishStickShake
	@brief : June 3236a6a0. For each BFT_STICK_SHAKE finalizer of this buff: the unit's slot 257 (EndStickShake) and one more on the
		buff-change counter. June runs it when the finish is found (3236c570) and again from every erase (OnFrameMoveBuff,
		EraseBuffTempletFromGameUnitByIterator 3229d320, the clear-all 3229fb10), so one BFT 15 buff ends the shake twice (JUNE_BUGS.md)
*/
void CX2BuffTemplet::DoFinishStickShake( CX2GameUnit* pGameUnit_ )
{
	BOOST_FOREACH( CX2BuffFinalizerTempletPtr ptrFinalizer, m_vecFinalizerPtr )
	{
		if ( NULL != ptrFinalizer && BFT_STICK_SHAKE == ptrFinalizer->GetType() )
		{
			pGameUnit_->EndStickShake();
			pGameUnit_->IncreaseBuffSyncCount();	/// @3236a6cc
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
			CX2BuffCapture::Line( "BFT 15 end hook, BTI %d: stick shaking %d, count %d, buff-change counter %I64u",
				static_cast<int>( GetBuffTempletID() ), pGameUnit_->IsStickShaking() ? 1 : 0, pGameUnit_->GetStickShakeCount(), pGameUnit_->GetBuffSyncCount() );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
		}
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER
//}}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetBasicStatValue
	@brief :스탯중 기본값을 셋팅하는 함수(스크립트값 또는 월드 변수등을 적용한 값 등)
	@param : 셋팅할 값(fValue_)
*/
void CX2AdditionalStatByBuff::SetBasicStatValue( const float fValue_ )
{ 
	if ( !IsSamef( GetBasicStatValue(), fValue_ ) )
	{
		m_fBasicStatValue = fValue_;
		SetMustChangeResultValue( true ); 
	}
}

/** @function : SetOptionValue
	@brief :스탯중 소켓 옵션에 의한 값을 셋팅하는 함수
	@param : 셋팅할 값(fValue_)
*/
void CX2AdditionalStatByBuff::SetOptionValue( const float fValue_ )
{
	if ( !IsSamef( GetOptionValue(), fValue_ ) )
	{
		m_fOptionValue = fValue_;
		SetMustChangeResultValue( true ); 
	}
}

/** @function : SetPassiveSum
	@brief : 스탯을 올리는 패시브 중 0.0을 기준으로 감소시에는 마이너스(-), 증가시에는 (+) 값인 요소를 셋팅하는 함수
	@param : 셋팅할 값(fValue_)
*/
void CX2AdditionalStatByBuff::SetPassiveSum( const float fValue_ )
{
	if ( !IsSamef( GetPassiveSum(), fValue_ ) )
	{
		m_fPassiveSum = fValue_;
		SetMustChangeResultValue( true ); 
	}
}

/** @function : SetPassiveMultiple
	@brief : 스탯을 올리는 패시브 중 1.0을 기준으로 감소시에는 0.0~1.0, 증가시에는 1.0 ~ 값인 요소를 셋팅
	@param : 셋팅할 값(fValue_)
*/
void CX2AdditionalStatByBuff::SetPassiveMultiple( const float fValue_ )
{
	if ( !IsSamef( GetPassiveMultiple(), fValue_ ) )
	{
		m_fPassiveMultiple = fValue_;
		SetMustChangeResultValue( true ); 
	}
}

/** @function : UpdateResultValueByChangeType
	@brief : 변경 타입별(치환, 고정수치, 연동수치, %) 결과값 갱신
*/
void CX2AdditionalStatByBuff::UpdateResultValueByChangeType()
{
	if ( m_bitsetMustChange.any() )	/// 변경된 것이 있다면 갱신
	{
		if ( m_bitsetMustChange.test( BCT_SWAP_VALUE ) )
		{
			/// 무조건 치환요소중 제일 나중에 걸린것 적용
			if ( m_vecStatFactorArray[BCT_SWAP_VALUE].empty() )
				m_fArrayResultValueByChangeType[BCT_SWAP_VALUE] = 0.0f;
			else
			{
				CX2StatFactorByBuffPtr& ptrStatFactor = m_vecStatFactorArray[BCT_SWAP_VALUE].back();
				m_fArrayResultValueByChangeType[BCT_SWAP_VALUE] = ptrStatFactor->GetValue();
			}
		}			

		if ( m_bitsetMustChange.test( BCT_FIX_VALUE ) )
		{
			float fResultValueByChangeType = 0.0f;

			auto FuncPlus = [&fResultValueByChangeType]( CX2StatFactorByBuffPtr ptrStatFactor_ ) {
				fResultValueByChangeType += ptrStatFactor_->GetValue();
			};

			if ( !m_vecStatFactorArray[BCT_FIX_VALUE].empty() )
				for_each( m_vecStatFactorArray[BCT_FIX_VALUE].begin(), m_vecStatFactorArray[BCT_FIX_VALUE].end(), FuncPlus );

			m_fArrayResultValueByChangeType[BCT_FIX_VALUE] = fResultValueByChangeType;
		}

		if ( m_bitsetMustChange.test( BCT_RELATION_VALUE ) )
		{
			float fResultValueByChangeType = 0.0f;

			auto FuncPlus = [&fResultValueByChangeType]( CX2StatFactorByBuffPtr ptrStatFactor_ ) {
				fResultValueByChangeType += ptrStatFactor_->GetValue();
			};

			if ( !m_vecStatFactorArray[BCT_RELATION_VALUE].empty() )
				for_each( m_vecStatFactorArray[BCT_RELATION_VALUE].begin(), m_vecStatFactorArray[BCT_RELATION_VALUE].end(), FuncPlus );

			m_fArrayResultValueByChangeType[BCT_RELATION_VALUE] = fResultValueByChangeType;
		}

		if ( m_bitsetMustChange.test( BCT_PERCENT ) )
		{
			float fResultValueByChangeType = 1.0f;

			auto FuncMultiplies = [&fResultValueByChangeType]( CX2StatFactorByBuffPtr ptrStatFactor_ ) {
				fResultValueByChangeType *= ptrStatFactor_->GetValue();
			};

			if ( !m_vecStatFactorArray[BCT_PERCENT].empty() )
				for_each( m_vecStatFactorArray[BCT_PERCENT].begin(), m_vecStatFactorArray[BCT_PERCENT].end(), FuncMultiplies );

			m_fArrayResultValueByChangeType[BCT_PERCENT] = fResultValueByChangeType;
		}

		m_bitsetMustChange.reset();	/// 변경을 적용 했으므로 초기화
	}		
}

/** @function : UpdateResultValue
	@brief : 옵션수치화와 관계없는 또는 기본값이 존재하는 스탯의 결과값을 업데이트 하는 함수
*/
void CX2AdditionalStatByBuff::UpdateResultValue()
{
	if ( !m_vecStatFactorArray[BCT_SWAP_VALUE].empty() )	/// 치환요소가 있다면
	{
		/// 무조건 치환요소중 제일 나중에 걸린것 적용
		SetResultValue( m_fArrayResultValueByChangeType[BCT_SWAP_VALUE] );
	}
	else	/// 치환요소가 없다면
	{
		float fResultValue = GetBasicStatValue();
		fResultValue += GetOptionValue();
		fResultValue += m_fArrayResultValueByChangeType[BCT_FIX_VALUE];
		fResultValue += m_fArrayResultValueByChangeType[BCT_RELATION_VALUE];

		/// 패시브스킬의 합배율과 곱배율 및 버프의 percent 배율을 모두 곱함
		const float fMultiplier 
			= ( 1 + GetPassiveSum() ) * GetPassiveMultiple() * m_fArrayResultValueByChangeType[BCT_PERCENT];
		if ( fMultiplier > 0.0f )	/// 효과가 감소되는 경우에는 0보다 크고 1.0보다 작음, 증가되는 경우 1.0보다 큼
			fResultValue += GetBasicStatValue() * (fMultiplier - 1.0f);
#ifdef _IN_HOUSE_
		else
		{
			// 오현빈 // 2013-08-16
			// 0으로 만들었을 때 발생할 수 있는 오류 사항에 대한 
			// 예외처리를 위해 기획적으로 사용 할 수 없도록 제한했습니다.
			// ex ) 인스펙터의 경우 특정 구간에 무조건 데미지를 1로 주기 위해, 
			//		방어력을 높게 잡아서. 90% 감소를 시켜도 1로 피격 되는데,
			//		100% 무시가 발생하면 한 번에 많은 데미지를 줄 수 있기 때문에 제한.
			DISPLAY_ERROR(L"BattleFieldBuffFactor.lua 설정 오류.\nPERCENT 값을 0으로 설정 할 수 없습니다.");
		}
#endif // _IN_HOUSE_

		SetResultValue( fResultValue );
	}
	SetMustChangeResultValue( false );
}

/** @function : UpdateResultValue
	@brief : 옵션수치화와 관계없는 또는 기본값이 존재하는 스탯이 변경되었는지를 검사하고, 결과값을 업데이트 하는 함수
*/
void CX2AdditionalStatByBuff::CheckAndUpdateResultValue()
{ 
	if ( GetMustChangeResultValue() )
	{
		/// 각 타입별 리스트 갱신
		UpdateResultValueByChangeType();

		/// 전체값 갱신
		UpdateResultValue();
	}
}

/** @function : CreateAndInsertStatFactor
	@brief : 버프에 의해 증가되는 요소를 만들고 삽입함
	@param : 고정, 연동 수치등 변동타입(eChangeType_), 어떤 버프에 의한 변화인지 구분(BuffIdentity_), 스탯값(fStatValue_)
*/
void CX2AdditionalStatByBuff::CreateAndInsertStatFactor( const BUFF_CHANGE_TYPE eChangeType_, const KBuffIdentity& BuffIdentity_, const float fStatValue_ )
{
	UINT uiIndex = eChangeType_;
	if ( uiIndex < ARRAY_SIZE( m_vecStatFactorArray ) )
	{
		CX2StatFactorByBuffPtr ptrStatFActor 
			= CX2StatFactorByBuffPtr( new CX2StatFactorByBuff( BuffIdentity_, fStatValue_ ) );

		m_vecStatFactorArray[eChangeType_].push_back( ptrStatFActor );
		m_bitsetMustChange.set( eChangeType_ );
		SetMustChangeResultValue( true );
	}				
}

/** @function : UpdateAddtionalStatFactor
	@brief : 버프의 Identity가 일치하는 것을 찾아서 스탯 갱신
	@param : 고정, 연동 수치등 변동타입(eChangeType_), 어떤 버프에 의한 변화인지 구분(BuffIdentity_), 스탯변경값(fStatValue_)
	@return: 성공(true), 실패(false) 로 실패하면 외부에서 Create 실행
*/
bool CX2AdditionalStatByBuff::UpdateAddtionalStatFactor( const BUFF_CHANGE_TYPE eChangeType_, const KBuffIdentity& BuffIdentity_, const float fValue_ )
{
	if ( !m_vecStatFactorArray[eChangeType_].empty() )
	{
		vector<CX2StatFactorByBuffPtr>::iterator vItr
			= FindSameBuffIdentity<vector<CX2StatFactorByBuffPtr>::iterator, CX2StatFactorByBuffPtr>( m_vecStatFactorArray[eChangeType_], BuffIdentity_ );

		if ( m_vecStatFactorArray[eChangeType_].end() != vItr )
		{
			(*vItr)->SetValue( fValue_ );
			m_bitsetMustChange.set( eChangeType_ );
			SetMustChangeResultValue( true );
			return true;
		}
	}

	return false;
}

/** @function : EraseStatFactor
	@brief :버프의 Identity가 일치하는 것을 찾아서 효과 삭제
	@param : 고정, 연동 수치등 변동타입(eChangeType_), 어떤 버프에 의한 변화인지 구분(BuffIdentity_)
*/
void CX2AdditionalStatByBuff::EraseStatFactor( const BUFF_CHANGE_TYPE eChangeType_, const KBuffIdentity& BuffIdentity_ )
{
	UINT uiIndex = eChangeType_;
	if ( uiIndex < ARRAY_SIZE( m_vecStatFactorArray ) )
	{
		vector<CX2StatFactorByBuffPtr>::iterator vItr
			= FindSameBuffIdentity<vector<CX2StatFactorByBuffPtr>::iterator, CX2StatFactorByBuffPtr>( m_vecStatFactorArray[uiIndex], BuffIdentity_ );

		if ( m_vecStatFactorArray[uiIndex].end() != vItr )
		{
			m_vecStatFactorArray[uiIndex].erase( vItr );
			m_bitsetMustChange.set( eChangeType_ );
			SetMustChangeResultValue( true );
		}
	}				
}
#ifdef VERIFY_STAT_BY_BUFF
bool CX2AdditionalStatByBuff::Verify()
{
	if ( m_fBasicStatValue.Verify() &&
		m_fPassiveSum.Verify() &&
		m_fPassiveMultiple.Verify() &&
		m_fOptionValue.Verify() &&
		m_fResultValue.Verify() &&
		VerifyStatFactor() &&
		VerifyArrayResult() )
		return true;
	else
		return false;
}

bool CX2AdditionalStatByBuff::VerifyStatFactor()
{
	for ( int i = 0; i < BCT_END; i++ )
	{
		BOOST_FOREACH( CX2StatFactorByBuffPtr& ptrStatFactor, m_vecStatFactorArray[i] )
		{
			if ( NULL != ptrStatFactor && false == ptrStatFactor->Verify() )
				return false;
		}
	}

	return true;
}

bool CX2AdditionalStatByBuff::VerifyArrayResult()
{
	for ( int i = 0; i < BCT_END; i++ )
	{
		if ( false == m_fArrayResultValueByChangeType[i].Verify() )
			return false;
	}

	return true;
}

#endif // VERIFY_STAT_BY_BUFF

/** @function : PlayEffectSetImpactPointByBuff
	@brief : 타격 위치에 지정된 이펙트셋 출력
	@param : 이펙트셋을 실행시키는 게임유닛(pGameUnit_), 타격위치(vImpactPos_)
*/
void CX2EffectSetImpactPointByBuff::PlayEffectSetImpactPointByBuff( CX2GameUnit* pGameUnit_, const D3DXVECTOR3& vImpactPos_ )
{
	if ( NULL != g_pX2Game )
	{
#ifdef SERV_NEW_DEFENCE_DUNGEON // 적용날짜 : 2013-04-09
		if ( m_fNowCoolTime >= m_fCreateGap )		/// 생성 간격이 지났을 때, 생성 처리
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( m_wstrEffectSetName, pGameUnit_, 
				NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), pGameUnit_->GetPowerRate(), -1.f, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), 
				true, vImpactPos_ );
			PlaySound( pGameUnit_ );

			m_fNowCoolTime = 0.f;		/// 생성 간격 초기화
		}
#else // SERV_NEW_DEFENCE_DUNGEON
		g_pX2Game->GetEffectSet()->PlayEffectSet( m_wstrEffectSetName, pGameUnit_, 
			NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), pGameUnit_->GetPowerRate(), -1.f, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), 
			true, vImpactPos_ );
		PlaySound( pGameUnit_ );
#endif // SERV_NEW_DEFENCE_DUNGEON
	}
}

/** @function : PlayEffectSetHittedByBuff
	@brief : 피격시 지정된 이펙트셋 출력
	@param : 이펙트셋을 실행시키는 게임유닛(pGameUnit_)
*/
void CX2EffectSetImpactPointByBuff::PlayEffectSetHittedByBuff( CX2GameUnit* pGameUnit_ )
{
	if ( NULL != g_pX2Game )
	{
#ifdef SERV_NEW_DEFENCE_DUNGEON // 적용날짜 : 2013-04-09
		if ( m_fNowCoolTime >= m_fCreateGap )		/// 생성 간격이 지났을 때, 생성 처리
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( m_wstrEffectSetName, pGameUnit_,
			NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), pGameUnit_->GetPowerRate() );
			PlaySound( pGameUnit_ );

			m_fNowCoolTime = 0.f;	/// 생성 간격 초기화
		}
#else // SERV_NEW_DEFENCE_DUNGEON
		g_pX2Game->GetEffectSet()->PlayEffectSet( m_wstrEffectSetName, pGameUnit_,
			NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), pGameUnit_->GetPowerRate() );
		PlaySound( pGameUnit_ );
#endif // SERV_NEW_DEFENCE_DUNGEON
	}
}

/** @function : PlaySound
	@brief : 사운드 출력
	@param : 사운드를 실행시키는 게임유닛(pGameUnit_)
*/
void CX2EffectSetImpactPointByBuff::PlaySound( CX2GameUnit* pGameUnit_ )
{
	if ( !m_wstrSoundName.empty() )
		pGameUnit_->PlaySound( m_wstrSoundName.c_str() );
}

#ifdef SERV_NEW_DEFENCE_DUNGEON // 적용날짜 : 2013-04-09

/** @function : PlayEffectSetNormalAttackByBuff
	@brief : 일반 공격 타격시 이펙트셋 생성
	@param : 이펙트셋을 실행시키는 게임유닛(pGameUnit_), 타격위치(vImpactPos_)
*/
void CX2EffectSetImpactPointByBuff::PlayEffectSetNormalAttackByBuff( CX2GameUnit* pGameUnit_, const CX2DamageManager::ATTACK_TYPE eAttackType )
{
	if ( NULL != g_pX2Game )
	{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		/// June 3236ed60: every attack type but AT_SPECIAL (1) plays, so AT_COMMAND (2) does too.
		if ( m_fNowCoolTime >= m_fCreateGap && CX2DamageManager::AT_SPECIAL != eAttackType )
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		if ( m_fNowCoolTime >= m_fCreateGap && CX2DamageManager::AT_NORMAL == eAttackType )		/// 생성 간격이 지났고, 일반 공격일때 생성 처리
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( m_wstrEffectSetName, pGameUnit_, 
				NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), pGameUnit_->GetPowerRate(), -1.f, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
			PlaySound( pGameUnit_ );

			m_fNowCoolTime = 0.f;		/// 생성 간격 초기화
		}
	}
}

/** @function : ProcessCoolTime
	@brief : 쿨타임 갱신 함수
	@param : 프레임당 시간
*/
void CX2EffectSetImpactPointByBuff::ProcessCoolTime( float fElapsedTime )
{
	if ( m_fNowCoolTime < m_fCreateGap )
		m_fNowCoolTime += fElapsedTime;
}
#endif // SERV_NEW_DEFENCE_DUNGEON

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : CX2CustomFunctionByBuffPhantomSword
	@brief : CX2CustomFunctionByBuffPhantomSword 의 생성자
*/
CX2CustomFunctionByBuffPhantomSword::CX2CustomFunctionByBuffPhantomSword( const KBuffIdentity& BuffIdentity_, CX2GameUnit* pGameUnit_ ) 
	: CX2CustomFunctionByBuff( BuffIdentity_ )
	, MAGIC_PHANTOM_SWORD_DELAY( 0.2f )
	, MAGIC_PHANTOM_DAMAGE_MULTIPLIER( 1.2f )
	, MAGIC_PHANTOM_STOP_TIME_DEF( 0.033f )
	, MAGIC_MAX_COUNT_CATCH_UP( 2 )
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	, MAGIC_PHANTOM_GAIN_RATE( 0.5f )		/// June ctor 3236eee0: +0x20 = 0x3343777c (0.5)
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	, m_fThirdBladeTime( 0.0f )
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    , m_hDamageEffectThirdBlade( INVALID_DAMAGE_EFFECT_HANDLE )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	, m_pDamageEffectThirdBlade( NULL )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	, m_hThirdBladeCenterMesh( INVALID_MESH_INSTANCE_HANDLE )
	//, m_hThirdBladeSheathMesh( INVALID_MESH_INSTANCE_HANDLE )
	, m_strPhantomBladeDamageDataNameUTF8()
	, m_iPhantomBladeAttackTimeIndex( -1 )
	, m_vPhantomBladeBackSpeed( 0.0f, 0.0f )
	, m_PhantomBladeReActType( CX2DamageManager::RT_NO_DAMAGE )
{
	InitializeDamageEffectThridBlade( pGameUnit_ );
	InitializeMeshThirdBaldeCenter( pGameUnit_ );	
}



/** @function : ~CX2CustomFunctionByBuffPhantomSword
	@brief : CX2CustomFunctionByBuffPhantomSword의 소멸자
*/
/*virtual*/ CX2CustomFunctionByBuffPhantomSword::~CX2CustomFunctionByBuffPhantomSword()
{
	if ( NULL != g_pX2Game )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hThirdBladeCenterMesh );
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        if ( m_hDamageEffectThirdBlade != INVALID_DAMAGE_EFFECT_HANDLE )
            g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffectThirdBlade );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		g_pX2Game->GetDamageEffect()->DestroyInstance( m_pDamageEffectThirdBlade );
        m_pDamageEffectThirdBlade = NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	}

	m_ThirdBladeMatrixQueue.swap( std::queue<ThirdBladeMatrixPtr>() );
	m_ThirdBladeDamageDataList.swap( std::list<ThirdBladeDamageDataPtr>() );
}

/** @function : FunctionInCommonStateStart
	@brief : CommonStateStart 에서 수행될 함수
	@param : 게임유닛(pGameUnit_), 루아매니저(luaManager_)
*/
/*virtual*/ void CX2CustomFunctionByBuffPhantomSword::FunctionInCommonStateStart( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_UTF8(	luaManager_, "PHANTOM_BLADE_DAMAGE_DATA_NAME",		m_strPhantomBladeDamageDataNameUTF8,	"" );
	LUA_GET_VALUE(	luaManager_, "PHANTOM_BLADE_ATTACK_TIME_INDEX",		m_iPhantomBladeAttackTimeIndex,		-1 );

	LUA_GET_VALUE(	luaManager_, "PHANTOM_BLADE_BACK_SPEED_X",			m_vPhantomBladeBackSpeed.x,			0.f );
	LUA_GET_VALUE(	luaManager_, "PHANTOM_BLADE_BACK_SPEED_Y",			m_vPhantomBladeBackSpeed.y,			0.f );

	LUA_GET_VALUE_ENUM( luaManager_, "PHANTOM_BLADE_REACT_TYPE",		m_PhantomBladeReActType,			CX2DamageManager::REACT_TYPE,		CX2DamageManager::RT_NO_DAMAGE );

	
	if ( pGameUnit_->GetSizeOfAttackTimeList() > 0 && -1 != m_iPhantomBladeAttackTimeIndex )
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        if ( CX2DamageEffect::CEffect* pDamageEffectThirdBlade = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffectThirdBlade ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		if ( CX2DamageEffect::CEffect* pDamageEffectThirdBlade = m_pDamageEffectThirdBlade )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pDamageEffectThirdBlade->GetMainEffect();
			if( NULL != pMeshInst )			
			{
				CreateAndInsertThirdBladeData( pGameUnit_, luaManager_ );
				if ( m_ThirdBladeDamageDataList.size() == 1 )	/// kimhc // 이건 왜 이렇게 했을까..?
                {
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                    MoveDataDataToDamageEffect( pGameUnit_, pDamageEffectThirdBlade );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					MoveDataDataToDamageEffect( pGameUnit_ );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                }
			}	// if( NULL != pMeshInst )			
		}	// if ( NULL != m_pDamageEffectThirdBlade )
	}
}

/** @function : FunctionInCommonFrameMove
	@brief : CommonFrameMove 에서 수행될 함수
	@param : 게임유닛(pGameUnit_), 루아매니저(luaManager_)
*/
/*virtual*/ void CX2CustomFunctionByBuffPhantomSword::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    if ( CX2DamageEffect::CEffect* m_pDamageEffectThirdBlade = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffectThirdBlade ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	if ( NULL != m_pDamageEffectThirdBlade )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = m_pDamageEffectThirdBlade->GetMainEffect();
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCenter = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hThirdBladeCenterMesh );

		if ( NULL != pMeshInst && NULL != pMeshInstCenter )
		{
			m_fThirdBladeTime = pMeshInst->GetGlobalTime();
			ThirdBladeMatrixPtr	ptrThirdBladeMatrix( new ThirdBladeMatrix );
			if ( NULL != ptrThirdBladeMatrix )
			{
				const D3DXMATRIX* pMatrixBone =	pGameUnit_->GetCombineMatrixFromBoneName( L"Dummy1_Rhand" );
				ptrThirdBladeMatrix->PastCombineMatrix = *pMatrixBone;
				ptrThirdBladeMatrix->fPastTime = static_cast<float>( g_pKTDXApp->GetTime() ); 
				m_ThirdBladeMatrixQueue.push( ptrThirdBladeMatrix );

				const float fDelayedTime = static_cast<float>( g_pKTDXApp->GetTime() ) - MAGIC_PHANTOM_SWORD_DELAY;
				ThirdBladeMatrixPtr ptrFirstThirdBladeMatrix = m_ThirdBladeMatrixQueue.front();
				
				for ( int i = 0; i < MAGIC_MAX_COUNT_CATCH_UP; ++i )
				{
					if ( ptrFirstThirdBladeMatrix->fPastTime > fDelayedTime )
						break;
					
					m_ThirdBladeMatrixQueue.pop();
					if ( !m_ThirdBladeMatrixQueue.empty() )
						ptrFirstThirdBladeMatrix = m_ThirdBladeMatrixQueue.front();
				}
				pMeshInst->SetUseDXMatrix( ptrFirstThirdBladeMatrix->PastCombineMatrix );
				pMeshInstCenter->SetUseDXMatrix( ptrFirstThirdBladeMatrix->PastCombineMatrix );
				
				const D3DXVECTOR3& vPos = pGameUnit_->GetPos();
				pMeshInstCenter->SetPos( vPos );
				pMeshInst->SetPos( vPos );
			}
		}
		else
			m_fThirdBladeTime = 0;

		if ( !m_ThirdBladeDamageDataList.empty() )
		{
			ThirdBladeDamageDataPtr ptrThirdBladeDamageData = m_ThirdBladeDamageDataList.front();

			if ( ptrThirdBladeDamageData->m_vAttackTime.y < m_fThirdBladeTime  )		
			{
				m_ThirdBladeDamageDataList.pop_front();
				if ( !m_ThirdBladeDamageDataList.empty() )
				{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                    if ( CX2DamageEffect::CEffect* pDamageEffectThirdBlade = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffectThirdBlade ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	                if ( CX2DamageEffect::CEffect* pDamageEffectThirdBlade = m_pDamageEffectThirdBlade )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                    {
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        MoveDataDataToDamageEffect( pGameUnit_, pDamageEffectThirdBlade );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					    MoveDataDataToDamageEffect( pGameUnit_ );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                    }
				}
			}
		}
	}
}

/** @function : FunctionInCommonStateEnd
	@brief : CommonStateEnd 에서 수행될 함수
	@param : 게임유닛(pGameUnit_), 루아매니저(luaManager_)
*/
/*virtual*/ void CX2CustomFunctionByBuffPhantomSword::FunctionInCommonStateEnd( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( pGameUnit_->GetSizeOfAttackTimeList() > 0 )
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        if ( CX2DamageEffect::CEffect* m_pDamageEffectThirdBlade = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffectThirdBlade ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	    if ( NULL != m_pDamageEffectThirdBlade )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = m_pDamageEffectThirdBlade->GetMainEffect();
			if ( NULL != pMeshInst && !m_ThirdBladeDamageDataList.empty() )
			{
				ThirdBladeDamageDataPtr ptrThirdBladeDamageDataBack	= m_ThirdBladeDamageDataList.back();
				const float fInterruptedAttackTime = m_fThirdBladeTime + MAGIC_PHANTOM_SWORD_DELAY;

				if ( fInterruptedAttackTime < ptrThirdBladeDamageDataBack->m_vAttackTime.y )
					ptrThirdBladeDamageDataBack->m_vAttackTime.y = fInterruptedAttackTime;

				if ( m_ThirdBladeDamageDataList.size() == 1 )
				{
					ThirdBladeDamageDataPtr ptrThirdBladeDamageDataFront 
						= m_ThirdBladeDamageDataList.front();
					m_pDamageEffectThirdBlade->SetAttackTime( ptrThirdBladeDamageDataFront->m_vAttackTime );
				}
			}
		}
	}
}

/** @function : InitializeDamageEffectThridBlade
	@brief : 팬텀소드의 데미지 이펙트 생성
	@param : 팬텀소드를 사용하는 게임유닛(pGameUnit_)
*/
void CX2CustomFunctionByBuffPhantomSword::InitializeDamageEffectThridBlade( CX2GameUnit* pGameUnit_ )
{
	wstring wstrEffectName = L"THIRD_BLADE";
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	/// June 3236d670: THIRD_BLADE + L"_MEMO" only when 31fca9e0( 601026 ) is true. 31fca9e0 is June's
	/// GetEqippedSkillMemo and returns false unconditionally (June has no skill memos), so the name is THIRD_BLADE.
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

#ifdef ADD_MEMO_1ST_CLASS //김창한
	CX2GUUser* pUser = static_cast<CX2GUUser*>( pGameUnit_ );
	if( NULL != pUser && pUser->GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO26) == true )
		wstrEffectName += L"_MEMO";
#endif //ADD_MEMO_1ST_CLASS
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    CX2DamageEffect::CEffect*
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_pDamageEffectThirdBlade
		= g_pX2Game->GetDamageEffect()->CreateInstance( pGameUnit_, wstrEffectName.c_str(), 
		pGameUnit_->GetPowerRate(), pGameUnit_->GetPos(), pGameUnit_->GetRotateDegree(), 
		pGameUnit_->GetRotateDegree(),
		pGameUnit_->GetLandPos().y );	
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    m_hDamageEffectThirdBlade = ( m_pDamageEffectThirdBlade != NULL ) 
        ? m_pDamageEffectThirdBlade->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	if ( NULL != m_pDamageEffectThirdBlade )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = m_pDamageEffectThirdBlade->GetMainEffect();
		if ( NULL != pMeshInst )
		{
			pMeshInst->SetBoundingRadius( 0.0f );

			const D3DXMATRIX* pMatrixBone =	pGameUnit_->GetCombineMatrixFromBoneName( L"Dummy1_Rhand" );
			if ( NULL != pMatrixBone )
			{
				pMeshInst->SetUseDXMatrix( *pMatrixBone );
				pMeshInst->OnFrameMove( g_pKTDXApp->GetTime(), 0.01f );
			}
		}
	}
}

/** @function : InitializeMeshThirdBaldeCenter
	@brief : 팬텀소드의 메시 생성
	@param : 팬텀소드를 사용하는 게임유닛(pGameUnit_)
*/
void CX2CustomFunctionByBuffPhantomSword::InitializeMeshThirdBaldeCenter( CX2GameUnit* pGameUnit_ )
{
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCenter = NULL;
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	/// June 3236d920: ThirdBladeCenter_MEMO only behind 31fca9e0( 601026 ), which is always false (see above).
	/// June then hands the mesh to 32296ee0, the game option's effect alpha for other units' effects
	/// (322d4d10 -> mesh +0x4a8), a system this build does not have: not ported.
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

#ifdef ADD_MEMO_1ST_CLASS //김창한
	CX2GUUser* pUser = static_cast<CX2GUUser*>( pGameUnit_ );
	if( NULL != pUser && pUser->GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO26) == true )
	{
		pMeshInstCenter = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( pGameUnit_,  L"ThirdBladeCenter_MEMO",
			pGameUnit_->GetPos(), pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree() );
	}
	else
#endif //ADD_MEMO_1ST_CLASS
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	pMeshInstCenter = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( pGameUnit_,  L"ThirdBladeCenter",
		pGameUnit_->GetPos(), pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree() );

	if ( NULL != pMeshInstCenter )
	{
		m_hThirdBladeCenterMesh = pMeshInstCenter->GetHandle();
		pMeshInstCenter->SetBoundingRadius( 0.0f );

		const D3DXMATRIX* pMatrixBone =	pGameUnit_->GetCombineMatrixFromBoneName( L"Dummy1_Rhand" );
		if ( NULL != pMatrixBone )
		{
			pMeshInstCenter->SetUseDXMatrix( *pMatrixBone );
			pMeshInstCenter->OnFrameMove( g_pKTDXApp->GetTime(), 0.01f );
		}
	}
#ifdef SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION
	// June2017 CX2CustomFunctionByBuffPhantomSword::InitializeMeshThirdBaldeCenter 3236d920 @3236dbc6, after the mesh block (the helper takes NULL): the mesh
	// takes the unit's effect-alpha category
	if( NULL != g_pX2Game )
		g_pX2Game->TagEffectAlphaMesh( pGameUnit_, pMeshInstCenter );
#endif SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION
}

/** @function : CreateAndInsertThirdBladeData
	@brief : 팬텀소드의 데미지 데이타 생성 및 queue에 push하는 함수
	@param : 팬텀소드를 사용하는 게임유닛(pGameUnit_), 루아매니저(luaManager_)
*/
void CX2CustomFunctionByBuffPhantomSword::CreateAndInsertThirdBladeData( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	const D3DXVECTOR2* const pAttackTime 
		= pGameUnit_->GetAttackTimeByIndex( m_iPhantomBladeAttackTimeIndex );
	if ( NULL != pAttackTime )
	{
		ThirdBladeDamageDataPtr ptrThirdBladeDamageData( new ThirdBladeDamageData );
		if ( NULL != ptrThirdBladeDamageData )
		{
			ptrThirdBladeDamageData->m_DamageData.SimpleInit();
			ptrThirdBladeDamageData->m_DamageData.attackerType			= CX2DamageManager::AT_UNIT;
			ptrThirdBladeDamageData->m_DamageData.optrAttackerGameUnit	= pGameUnit_;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            ptrThirdBladeDamageData->m_DamageData.hAttackerEffect		= INVALID_DAMAGE_EFFECT_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			ptrThirdBladeDamageData->m_DamageData.pAttackerEffect		= NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

			if ( m_strPhantomBladeDamageDataNameUTF8.empty() )
				g_pData->GetDamageManager()->SetDamageDataFromLUA( &(ptrThirdBladeDamageData->m_DamageData), luaManager_, "DAMAGE_DATA" );
			else
				g_pData->GetDamageManager()->SetDamageDataFromLUA( &(ptrThirdBladeDamageData->m_DamageData), luaManager_, m_strPhantomBladeDamageDataNameUTF8.c_str() );
			
			const D3DXVECTOR2 vDelayedAttackTime = *pAttackTime / pGameUnit_->GetPlaySpeed();

			ptrThirdBladeDamageData->m_vAttackTime.x				= m_fThirdBladeTime + vDelayedAttackTime.x + MAGIC_PHANTOM_SWORD_DELAY; 
			ptrThirdBladeDamageData->m_vAttackTime.y				= m_fThirdBladeTime + vDelayedAttackTime.y + MAGIC_PHANTOM_SWORD_DELAY; 
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
			/// June 32370e10 @32370f96-323710e7, in its order. Where March zeroed fHitAddMP, June halves techPoint,
			/// fHitAddMP and m_fRateModifier (MAGIC_PHANTOM_GAIN_RATE); June does not force AT_SPECIAL, so the
			/// blade keeps the state's ATTACK_TYPE. The memo branch (IgnoreDefence, +0.1 critical damage) sits behind
			/// 31fca9e0( 601026 ), June's GetEqippedSkillMemo, which always returns false.
			ptrThirdBladeDamageData->m_DamageData.damage.fPhysic	*= MAGIC_PHANTOM_DAMAGE_MULTIPLIER;
			ptrThirdBladeDamageData->m_DamageData.damage.fMagic		*= MAGIC_PHANTOM_DAMAGE_MULTIPLIER;
			ptrThirdBladeDamageData->m_DamageData.fForceDownValue	= 0.f;
			ptrThirdBladeDamageData->m_DamageData.techPoint			= static_cast<int>( ptrThirdBladeDamageData->m_DamageData.techPoint * MAGIC_PHANTOM_GAIN_RATE );
			ptrThirdBladeDamageData->m_DamageData.fHitAddMP			*= MAGIC_PHANTOM_GAIN_RATE;
			ptrThirdBladeDamageData->m_DamageData.m_fRateModifier	*= MAGIC_PHANTOM_GAIN_RATE;
			ptrThirdBladeDamageData->m_DamageData.fStopTimeDef		+= MAGIC_PHANTOM_STOP_TIME_DEF;
			ptrThirdBladeDamageData->m_DamageData.backSpeed			= m_vPhantomBladeBackSpeed;
			ptrThirdBladeDamageData->m_DamageData.bArrangedFly		= true;
			ptrThirdBladeDamageData->m_DamageData.optrAttackerGameUnit	= pGameUnit_;
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
			ptrThirdBladeDamageData->m_DamageData.damage.fPhysic	*= MAGIC_PHANTOM_DAMAGE_MULTIPLIER;
			ptrThirdBladeDamageData->m_DamageData.damage.fMagic		*= MAGIC_PHANTOM_DAMAGE_MULTIPLIER;
			ptrThirdBladeDamageData->m_DamageData.fForceDownValue	= 0.f;
			ptrThirdBladeDamageData->m_DamageData.fHitAddMP			= 0.f;
			ptrThirdBladeDamageData->m_DamageData.fStopTimeDef		+= MAGIC_PHANTOM_STOP_TIME_DEF;
			ptrThirdBladeDamageData->m_DamageData.backSpeed			= m_vPhantomBladeBackSpeed;
			ptrThirdBladeDamageData->m_DamageData.bArrangedFly		= true;
			ptrThirdBladeDamageData->m_DamageData.attackType		= CX2DamageManager::AT_SPECIAL;
			ptrThirdBladeDamageData->m_DamageData.optrAttackerGameUnit	= pGameUnit_;
#ifdef ADD_MEMO_1ST_CLASS //김창한
			CX2GUUser* pUser = static_cast<CX2GUUser*>( pGameUnit_ );
			if( NULL != pUser )
			{
				if( pUser->GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO26) == true )
				{
					ptrThirdBladeDamageData->m_DamageData.m_bIgnoreDefence = true;
					ptrThirdBladeDamageData->m_DamageData.m_fApplyCriticalDamage = 0.1f;
				}
			}
#endif //ADD_MEMO_1ST_CLASS
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

			if ( CX2DamageManager::RT_NO_DAMAGE != m_PhantomBladeReActType )
				ptrThirdBladeDamageData->m_DamageData.reActType = m_PhantomBladeReActType;

			m_ThirdBladeDamageDataList.push_back( ptrThirdBladeDamageData );
		}
	}
}

/** @function : MoveDataDataToDamageEffect
	@brief : 유저의 스테이트에서 읽어들인 데미지 데이터를 데미지이펙트에 전달
	@param : 팬텀소드를 사용중인 게임유닛(pGameUnit_)
*/
void CX2CustomFunctionByBuffPhantomSword::MoveDataDataToDamageEffect( CX2GameUnit* pGameUnit_
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    , CX2DamageEffect::CEffect*   pDamageEffectThirdBlade
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    )
{
#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    CX2DamageEffect::CEffect*   pDamageEffectThirdBlade = m_pDamageEffectThirdBlade;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	/// m_ThirdBladeDamageDataList의 empty 검사, m_pDamageEffectThirdBlade NULL 체크는 caller에서 수행함
	ThirdBladeDamageDataPtr ptrThirdBladeDamageData = m_ThirdBladeDamageDataList.front();
	pDamageEffectThirdBlade->SetAttackTime( ptrThirdBladeDamageData->m_vAttackTime );

	CX2DamageManager::DamageData& kDamageData = pDamageEffectThirdBlade->GetDamageData();
	kDamageData = ptrThirdBladeDamageData->m_DamageData;
	kDamageData.attackerType = CX2DamageManager::AT_EFFECT;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    kDamageData.hAttackerEffect = pDamageEffectThirdBlade->GetHandle();
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	kDamageData.pAttackerEffect = pDamageEffectThirdBlade;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	pDamageEffectThirdBlade->SetFollowLineRight( pGameUnit_->GetIsRight() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// CX2CustomFunctionByBuffMagicalMakeUp::CX2CustomFunctionByBuffMagicalMakeUp( const KBuffIdentity& BuffIdentity_, CX2GameUnit* pGameUnit_ )
// 	: CX2CustomFunctionByBuff( BuffIdentity_ ), MAGICAL_MAKE_UP_WEAPON_ID( 40930 )
// {
// 	CX2Unit* pUnit = pGameUnit_->GetUnit();
// 	if ( NULL != pUnit )
// 	{
// 		m_ptrEquipTransform = CX2ItemManager::CreateDummyEquip( MAGICAL_MAKE_UP_WEAPON_ID, pUnit, pGameUnit_->GetSkinAnimPtr() );
// 		m_ptrEquipTransform->SetManagerObject( this );
// 		m_ptrEquipTransform->SetShowObject(false);
// 	}
// }
// 
// CX2CustomFunctionByBuffMagicalMakeUp::~CX2CustomFunctionByBuffMagicalMakeUp()
// {
// 	/// 각 유닛마다 특정 버프가 해제될 때 셋팅해줘야 하는 값(ex: 매지컬메이크업이 시전중이라는 플래그 등...)
// //	pGameUnit_->UpSetSpecificValueByBuffTempletID( m_DataBuffIdentity.m_eBuffTempletID );
// }
// 
// void CX2CustomFunctionByBuffMagicalMakeUp::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
// {
// // 	if ( NULL != m_ptrEquipTransform )
// // 	{
// // 		m_ptrEquipTransform->SetRenderParam( m_fTime, m_fElapsedTime * m_AdvanceTimeCount, m_pXSkinAnim->GetRenderParam() );
// // 		m_ptrEquipTransform->OnFrameMove( m_fTime, m_fElapsedTime * m_AdvanceTimeCount );
// // 	}	
// }

//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/** @function : FunctionInCommonStateStart
	@brief : June 3236c6f0. As March, but a state start while the cool time (June +0x2c) runs leaves the queued
			 grenades alone, and a state with a BIG_BANG_STREAM table arms the cool time (+0x30, the ctor's
			 SA_READY_TIME_REL) after EndTable.
*/
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::FunctionInCommonStateStart( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( 0.0f < m_fRemainCoolTime )
		return;

	m_vecAnimationTime.resize( 0 );
	if ( luaManager_.BeginTable( "BIG_BANG_STREAM" ) )
	{
		int iIndex = 1;
		float fValue = 0.0f;
		while ( luaManager_.GetValue( iIndex, fValue ) )
		{
			m_vecAnimationTime.push_back( fValue );
			++iIndex;
		}

		/// largest first, as March: FrameMove fires from the back
		std::sort( m_vecAnimationTime.begin(), m_vecAnimationTime.end(), std::greater<float>() );
		m_vStateStartPos = pGameUnit_->GetPos();

		luaManager_.EndTable();
		m_fRemainCoolTime = m_fCoolTime;
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::FunctionInCommonStateStart( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	m_vecAnimationTime.resize( 0 );
	if ( luaManager_.BeginTable( "BIG_BANG_STREAM" ) )
	{
		int iIndex = 1;
		float fValue = 0.0f;
		while ( luaManager_.GetValue( iIndex, fValue ) )
		{
			m_vecAnimationTime.push_back( fValue );
			++iIndex;
		}

		/// 큰것에 부터 작은 순으로 정렬
		std::sort( m_vecAnimationTime.begin(), m_vecAnimationTime.end(), std::greater<float>() );
		m_vStateStartPos = pGameUnit_->GetPos();	/// 스테이트가 시작됬을 때의 유닛 위치를 저장

		luaManager_.EndTable();
	}
}

//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/** @function : DoExtraAttack
	@brief : June 3236dbf0 (vtable slot 9). Bip01, then HAND_GRENADE_0 at GetPowerRate(); March's 20% memo grenade
			 (SMI_CHUNG_MEMO26) is gone. June's replacement, CHUNG_BIGBANGSTREAM_ULTIMATE when GetRandomFloat( 0 ) <=
			 the skill's SA_EFFECTIVE_RATE (32007c40( 5, 26019 )), is gated by 31fcf3e0( RNSI_SA_CHG_BIGBANG_STREAM ),
			 June's skill-memo map, which March lacks (ParsingBuffFactorID keeps the same gate false), so the grenade
			 is always HAND_GRENADE_0. The throw takes the absolute value of both the x and the z part of
			 GetDirVector() * 150 (FABS @3236de60 / @3236de6e; March's FIX_BIGBANG_STREAM_BUFF keeps x signed).
*/
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::DoExtraAttack( CX2GameUnit* pGameUnit_ )
{
	const D3DXVECTOR3& vBonePos = pGameUnit_->GetBonePos( L"Bip01" );
	CX2DamageEffect::CEffect* pDamageEffect
		= g_pX2Game->GetDamageEffect()->CreateInstance(
		pGameUnit_, L"HAND_GRENADE_0", pGameUnit_->GetPowerRate(), vBonePos,
		pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree(),  pGameUnit_->GetLandPos().y );
	if ( NULL == pDamageEffect )
		return;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	g_pX2Game->AddRemoveDamageEffect( pDamageEffect->GetHandle() );		/// June 32a2dec0 on g_pX2Game +0x8a4
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	g_pX2Game->AddRemoveDamageEffect( pDamageEffect );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	CKTDGXMeshPlayer::CXMeshInstance *pMesh = pDamageEffect->GetMainEffect();
	if ( NULL != pMesh )
	{
		const D3DXVECTOR3 vDir = pGameUnit_->GetDirVector() * 150.f;
		const float fDirX = ( ( fabs( vDir.x ) + fabs( vDir.z ) ) * 0.3f ) * 10.f + 100.f;
		const float fDirY = ( vDir.y * 10.f ) + 100.f;
		pMesh->SetVelocity( D3DXVECTOR3( fDirX, fDirY, 0.f ) );
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::DoExtraAttack( CX2GameUnit* pGameUnit_ )
{
	const D3DXVECTOR3& vBonePos = pGameUnit_->GetBonePos( L"Bip01" );
	CX2DamageEffect::CEffect* pDamageEffect = NULL;
#ifdef ADD_MEMO_1ST_CLASS //김창한
	CX2GUUser* pUser = static_cast<CX2GUUser*>( pGameUnit_ );
	if( NULL != pUser && pUser->GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO26 ) == true 
		&& pUser->GetRandomFloat() <= 0.2f )
		pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pGameUnit_, L"CHUNG_BIGBANGSTREAM_MEMO", pGameUnit_->GetPowerRate(), vBonePos, pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree(),  pGameUnit_->GetLandPos().y );
	else
#endif //ADD_MEMO_1ST_CLASS

	pDamageEffect 
		= g_pX2Game->GetDamageEffect()->CreateInstance( 
		pGameUnit_, L"HAND_GRENADE_0", pGameUnit_->GetPowerRate(), vBonePos,
		pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree(),  pGameUnit_->GetLandPos().y );
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    if ( pDamageEffect == NULL )
        return;
    g_pX2Game->AddRemoveDamageEffect( pDamageEffect->GetHandle() );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    g_pX2Game->AddRemoveDamageEffect( pDamageEffect );	
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	
	
	CKTDGXMeshPlayer::CXMeshInstance *pMesh = pDamageEffect->GetMainEffect();
	if ( NULL != pMesh )
	{
#ifdef FIX_BIGBANG_STREAM_BUFF
		D3DXVECTOR3 vDir = pGameUnit_->GetDirVector() * 150.f;

		float fDirX = ( (vDir.x + abs(vDir.z)) * 0.3f ) * 10.f + 100.f;
		const float fDirY = (vDir.y * 10.f) + 100.f;
#else
		const D3DXVECTOR3& vPos = pGameUnit_->GetPos();
		D3DXVECTOR3 vDir = vPos - m_vStateStartPos;
		float fDirX = ( (vDir.x + vDir.z) * 0.5f ) * 10.f + 100.f;
		const float fDirY = (vDir.y * 10.f) + 100.f;

		if ( pGameUnit_->GetIsRight() == false )
			fDirX *= -1.f;
#endif // FIX_BIGBANG_STREAM_BUFF

		pMesh->SetVelocity( D3DXVECTOR3( fDirX, fDirY, 0.f) );
	}
}


//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
/*virtual*/ void CX2CustomFunctionByBuffMarkOfCommand::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_checkElapsedTime.OnFrameMove( pGameUnit_->GetElapsedTime() );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_checkElapsedTime.OnFrameMove();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
}

//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/** @function : FunctionInCommonFrameMove
	@brief : June 3236a040. March's grenade loop, then the cool time runs down by the unit's elapsed time
			 (+0x2b0) and stops at 0.
*/
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( !m_vecAnimationTime.empty() && pGameUnit_->GetSkinAnimPtr() )
	{
		const float fNowAnimationTime = pGameUnit_->GetSkinAnimPtr()->GetNowAnimationTime();

		while ( !m_vecAnimationTime.empty() && m_vecAnimationTime.back() <= fNowAnimationTime )
		{
			DoExtraAttack( pGameUnit_ );
			m_vecAnimationTime.pop_back();
		}
	}

	if ( 0.0f < m_fRemainCoolTime )
	{
		m_fRemainCoolTime -= pGameUnit_->GetElapsedTime();
		if ( m_fRemainCoolTime <= 0.0f )
			m_fRemainCoolTime = 0.0f;
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
/*virtual*/ void CX2CustomFunctionByBuffBigBangStream::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( !m_vecAnimationTime.empty() && pGameUnit_->GetSkinAnimPtr() )
	{
		const float fNowAnimationTime = pGameUnit_->GetSkinAnimPtr()->GetNowAnimationTime();

		while ( !m_vecAnimationTime.empty() && m_vecAnimationTime.back() <= fNowAnimationTime )
		{
			DoExtraAttack( pGameUnit_ );
			m_vecAnimationTime.pop_back();
		}		
	}
}

//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
/*virtual*/ void CX2CustomFunctionByBuffMarkOfCommand::FunctionInDamageReact( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
	DoMarkOfCommand( pGameUnit_, damageData_ );
}

//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/** @function : DoMarkOfCommand
	@brief : June 3236df00 (slot 4 jumps here from 3236f370). March's guards and shells, with two changes:
			 1. a user attacker's shell power also takes the Mark of Commander skill: x GetSkillDamageRateByBuff
			    (31fd7cc0, BBT 89 x 90) and x ( 1 + 3201ad10( 26214 ) ). 3201ad10 is June's skill damage-up total
			    (the per-unit map +0x87b4, the transcendence skill, the item 215434 option); every source is a June
			    system this build lacks, so it adds 0.
			 2. March's memo shell (CHUNG_MARK_OF_COMMANDER_SHELL_MEMO by m_fMemoRate) is gone: always the shell.
			 June asks the trigger through 32610fb0( 3 ), which also searches a June-only DamageData list
			 (+0x544..+0x548); March has the one m_eDamageTrigger.
*/
void CX2CustomFunctionByBuffMarkOfCommand::DoMarkOfCommand( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	if( damageData_.m_fHpPercentUp <= 0.f &&
		damageData_.reActType != CX2DamageManager::RT_CREATE_DAMAGE_EFFECT &&
		damageData_.reActType != CX2DamageManager::RT_NO_DAMAGE &&
		damageData_.reActResult != CX2DamageManager::RT_DUMMY_DAMAGE &&
		damageData_.reActResult != CX2DamageManager::RT_SENSOR &&
		damageData_.hitType != CX2DamageManager::HT_GROUND_HIT &&
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
		false == damageData_.IsAttackTrigger( CX2DamageManager::DTT_MARK_OF_COMMANDER ) &&	/// June 32610fb0( 3 ) @3236df5b (16-step3c)
#else SERV_IRUHADEV_JUNE_DAMAGEDATA
		damageData_.m_eDamageTrigger != CX2DamageManager::DTT_MARK_OF_COMMANDER &&
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
		damageData_.m_bNoDamage == false &&
		null != damageData_.optrAttackerGameUnit &&
		m_checkElapsedTime.CheckAndResetElapsedTime() )
	{
		CX2GameUnit* pAttackerGameUnit = damageData_.optrAttackerGameUnit.GetObservable();

		D3DXVECTOR3 vDirectVectorZeroY = pAttackerGameUnit->GetDirVector();
		vDirectVectorZeroY.y	= 0.f;
		const float fConstUnitHeight = 1080.f;
		float fShellPowerRate = damageData_.damage.GetAllDamage() * damageData_.m_fDungeonRate;

		if ( CX2GameUnit::GUT_USER == pAttackerGameUnit->GetGameUnitType() )
		{
			float fSkillDamageRateByBuff = 1.0f;
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
			fSkillDamageRateByBuff = static_cast<CX2GUUser*>( pAttackerGameUnit )->GetSkillDamageRateByBuff( CX2SkillTree::SI_A_CHG_MARK_OF_COMMANDER );	/// June 26214 = RNSI_A_CHG_MARK_OF_COMMANDER
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// buff port 16-step3b7 item 1: June @3236e002 3201ad10( 0x6666 ) on the attacker user (esi), + 1.0 (334355b0) below;
			/// June's 26214 is March's SI_A_CHG_MARK_OF_COMMANDER, the key 16-step3b5's slot pass files the rate under
			const float fSkillDamageAddRate = static_cast<CX2GUUser*>( pAttackerGameUnit )->GetSkillAdditionalPowerRate( CX2SkillTree::SI_A_CHG_MARK_OF_COMMANDER );
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
			const float fSkillDamageAddRate = 0.0f;	/// 3201ad10, see the brief
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			fShellPowerRate = ( fSkillDamageAddRate + 1.0f ) * ( fSkillDamageRateByBuff * fShellPowerRate );
		}

		for ( UINT uiIndex = 0; uiIndex < m_uiDamageEffectCount; ++uiIndex )
		{
			float fRandReservTime = 0.5f;
			D3DXVECTOR3 vRotateDegree = pAttackerGameUnit->GetRotateDegree();

			if ( 0 < uiIndex )
			{
				fRandReservTime += pAttackerGameUnit->GetRandomFloat( uiIndex ) * 0.3f * uiIndex;
				fRandReservTime += pAttackerGameUnit->GetRandomFloat( pGameUnit_->GetUnitIndex() ) * 0.3f;
				vRotateDegree.z += ( pAttackerGameUnit->GetRandomFloat( pGameUnit_->GetUnitIndex() + uiIndex) * 10.f - 5.f );
			}

			D3DXVECTOR3 vShellStartPos = pGameUnit_->GetPos();
			vShellStartPos.y += fConstUnitHeight;

			if ( pAttackerGameUnit->GetIsRight() )
				vShellStartPos += ( m_fBasicPosX[uiIndex] * vDirectVectorZeroY );
			else
				vShellStartPos -= ( m_fBasicPosX[uiIndex] * vDirectVectorZeroY );

			g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerGameUnit, L"CHUNG_MARK_OF_COMMANDER_SHELL",
				fShellPowerRate * m_fAddPowerRate, vShellStartPos, vRotateDegree, vRotateDegree,
				pAttackerGameUnit->GetLandPos().y, true, fRandReservTime );
		}
	}
#endif SERV_ADD_CHUNG_SHELLING_GUARDIAN
}
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
void CX2CustomFunctionByBuffMarkOfCommand::DoMarkOfCommand( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
if( damageData_.m_fHpPercentUp <= 0.f &&
	damageData_.reActType != CX2DamageManager::RT_CREATE_DAMAGE_EFFECT &&
	damageData_.reActType != CX2DamageManager::RT_NO_DAMAGE &&
	damageData_.reActResult != CX2DamageManager::RT_DUMMY_DAMAGE &&
	damageData_.reActResult != CX2DamageManager::RT_SENSOR &&
	damageData_.hitType != CX2DamageManager::HT_GROUND_HIT &&
	damageData_.m_eDamageTrigger != CX2DamageManager::DTT_MARK_OF_COMMANDER &&
	damageData_.m_bNoDamage == false )
{
	if ( null != damageData_.optrAttackerGameUnit &&
		 m_checkElapsedTime.CheckAndResetElapsedTime() )
	{
		CX2GameUnit* pAttackerGameUnit = damageData_.optrAttackerGameUnit.GetObservable();

		D3DXVECTOR3 vDirectVectorZeroY = pAttackerGameUnit->GetDirVector();
		vDirectVectorZeroY.y	= 0.f;
		const float fConstUnitHeight = 1080.f;
		const float fShellPowerRate = damageData_.damage.GetAllDamage() * damageData_.m_fDungeonRate;

		for ( UINT uiIndex = 0; uiIndex < m_uiDamageEffectCount; ++uiIndex )
		{
			float fRandReservTime = 0.5f;
			D3DXVECTOR3 vRotateDegree = pAttackerGameUnit->GetRotateDegree();

			if ( 0 < uiIndex )
			{
				fRandReservTime += pAttackerGameUnit->GetRandomFloat( uiIndex ) * 0.3f * uiIndex;
				fRandReservTime += pAttackerGameUnit->GetRandomFloat( pGameUnit_->GetUnitIndex() ) * 0.3f;
				vRotateDegree.z += ( pAttackerGameUnit->GetRandomFloat( pGameUnit_->GetUnitIndex() + uiIndex) * 10.f - 5.f );
			}

			D3DXVECTOR3 vShellStartPos = pGameUnit_->GetPos();
			vShellStartPos.y += fConstUnitHeight;			

			if ( pAttackerGameUnit->GetIsRight() )
				vShellStartPos += ( m_fBasicPosX[uiIndex] * vDirectVectorZeroY );
			else
				vShellStartPos -= ( m_fBasicPosX[uiIndex] * vDirectVectorZeroY );

			/// 메모를 배웠으면
			const WCHAR* pwszDamageEffectName
				= ( ( 0.0f < m_fMemoRate && pAttackerGameUnit->GetRandomFloat( uiIndex + 3 ) <= m_fMemoRate ) ?
				L"CHUNG_MARK_OF_COMMANDER_SHELL_MEMO" : L"CHUNG_MARK_OF_COMMANDER_SHELL" );
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 청 스킬 개편, 김종훈
			g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerGameUnit, pwszDamageEffectName, 
			fShellPowerRate * m_fAddPowerRate, vShellStartPos, vRotateDegree, vRotateDegree,  
			pAttackerGameUnit->GetLandPos().y, true, fRandReservTime );						

#else // UPGRADE_SKILL_SYSTEM_2013 // 청 스킬 개편, 김종훈
			g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerGameUnit, pwszDamageEffectName, 
				fShellPowerRate, vShellStartPos, vRotateDegree, vRotateDegree,  
				pAttackerGameUnit->GetLandPos().y, true, fRandReservTime );				
#endif // UPGRADE_SKILL_SYSTEM_2013 // 청 스킬 개편, 김종훈								
		}
		
	}
}
#endif SERV_ADD_CHUNG_SHELLING_GUARDIAN

}
//{{ Iruha : 2026-10-05 // June buff port Phase 9a (JUNE_BUFF_PORT_PHASES.md)
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION

#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/// June unit vslot 178 (0x2c8): a user's centre is GetPos( false ) plus half its height (CX2GUUser 31fc9380);
/// every other unit answers GetPos( false ) (CX2GUNPC 322b5530). No class overrides it.
static D3DXVECTOR3 GetUnitCenterPos( const CX2GameUnit* pUnit_ )
{
	D3DXVECTOR3 vPos = pUnit_->GetPos( false );
	if ( CX2GameUnit::GUT_USER == pUnit_->GetGameUnitType() )
		vPos.y += pUnit_->GetUnitHeight( false ) * 0.5f;
	return vPos;
}

/// June CX2GUNPC vslot 269 (0x434, 327574e0): GetPos( false ) plus half the height (an NPC-only virtual).
static D3DXVECTOR3 GetNpcCenterPos( const CX2GameUnit* pNpc_ )
{
	D3DXVECTOR3 vPos = pNpc_->GetPos( false );
	vPos.y += pNpc_->GetUnitHeight( false ) * 0.5f;
	return vPos;
}

/// June 32278580 (target, OUT name, pos): the frame name of the target's collision point nearest vPos_
/// (GetPointStart, strict <, from 99999; empty without collision data). The same walk as the effect port's
/// JuneGetLockOnAimPos (X2DamageEffect.cpp), which inlines it.
static std::wstring GetNearestCollisionFrameName( const CX2GameUnit* pUnit_, const D3DXVECTOR3& vPos_ )
{
	std::wstring wstrNearestFrame;
	float fNearest = 99999.f;
	const CKTDXCollision::CollisionDataListSet& AttackListSet = pUnit_->GetCollisionListSet();
	BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
	{
		if( NULL == collisionDataList )
			continue;
		BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
		{
			if( NULL == collisionData )
				continue;
			const float fDist = GetDistance( vPos_, collisionData->GetPointStart() );
			if( fDist < fNearest )
			{
				fNearest = fDist;
				wstrNearestFrame = collisionData->m_FrameName;
			}
		}
	}
	return wstrNearestFrame;
}

/// June 32293b80 (CX2GameUnit, pos by value): true when there is no game, world or line map; false when the
/// unit's last touched line (vslot 0x12c, GetLastTouchLineIndex( true )) is out of range or has no data;
/// else whether the way from the unit (GetPos( false )) to vPos_ runs along that line's dirVector (+0x54):
/// the dot product of the two normalised vectors is above 0.
static bool IsPosAlongLastTouchLine( const CX2GameUnit* pUnit_, const D3DXVECTOR3& vPos_ )
{
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetLineMap() )
		return true;

	const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( pUnit_->GetLastTouchLineIndex( true ) );
	if ( NULL == pLineData )
		return false;

	D3DXVECTOR3 vToPos = vPos_ - pUnit_->GetPos( false );
	D3DXVec3Normalize( &vToPos, &vToPos );
	D3DXVECTOR3 vLineDir = pLineData->dirVector;
	D3DXVec3Normalize( &vLineDir, &vLineDir );
	return 0.0f < D3DXVec3Dot( &vToPos, &vLineDir );
}

/// June 31fd5ef0 (CX2GUUser; target, centre, mode, OUT pos; RET 0x10). The Pandemonium Fear explosion calls
/// it with ( the NPC it hit, true, 1 ). From the attacker's centre to the target's centre: the target's
/// collision frame nearest its centre and that bone's position (GetBonePos( name, out ), vslot 32). The bone
/// is the answer when it is more than 700 (0x3344ce98) from the target's centre and nearer the attacker than
/// the centre is; else the centre is. Mode 1 also needs the answer to lie the way the attacker faces along
/// its line (32293b80 == GetIsRight), and a refused bone falls back to the centre under the same test.
/// Returns false, vOut_ untouched, when nothing passes (or for a mode other than 0/1).
static bool GetTargetHitPos( const CX2GameUnit* pAttacker_, const CX2GameUnit* pTarget_, const bool bCenter_, const BYTE byMode_, OUT D3DXVECTOR3& vOut_ )
{
	if ( NULL == pTarget_ )
		return false;

	const D3DXVECTOR3 vMyPos = ( bCenter_ ? GetUnitCenterPos( pAttacker_ ) : pAttacker_->GetPos( false ) );
	const D3DXVECTOR3 vTargetPos = ( bCenter_ ? GetNpcCenterPos( pTarget_ ) : pTarget_->GetPos( false ) );
	const float fTargetDistance = GetDistance( vTargetPos, vMyPos );

	const std::wstring wstrFrameName = GetNearestCollisionFrameName( pTarget_, vTargetPos );
	D3DXVECTOR3 vBonePos( 0.0f, 0.0f, 0.0f );
	if ( true == pTarget_->GetBonePos( wstrFrameName.c_str(), vBonePos )
		&& 700.0f < GetDistance( vTargetPos, vBonePos )
		&& GetDistance( vMyPos, vBonePos ) < fTargetDistance )
	{
		if ( 0 == byMode_ )
		{
			vOut_ = vBonePos;
			return true;
		}
		else if ( 1 == byMode_ )
		{
			if ( IsPosAlongLastTouchLine( pAttacker_, vBonePos ) == pAttacker_->GetIsRight() )
			{
				vOut_ = vBonePos;
				return true;
			}
		}
		else
			return false;
	}

	if ( 0 == byMode_ )
	{
		vOut_ = vTargetPos;
		return true;
	}
	else if ( 1 == byMode_ && IsPosAlongLastTouchLine( pAttacker_, vTargetPos ) == pAttacker_->GetIsRight() )
	{
		vOut_ = vTargetPos;
		return true;
	}
	return false;
}

/** @function : FunctionInCommonFrameMove
	@brief : June 32369890: the interval timer runs on the unit's elapsed time (+0x2b0)
*/
/*virtual*/ void CX2CustomFunctionByBuffPandemoniumFear::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_checkElapsedTime.OnFrameMove( pGameUnit_->GetElapsedTime() );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_checkElapsedTime.OnFrameMove();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
}

/** @function : FunctionInDamageReact
	@brief : June 3236e2a0. Mark of Commander's guards with trigger 6 (June Enum.lua DTT_PANDEMONIUM_FEAR; March's
			 C++ enum ends at 5, its Lua at DTT_END = 6), then, once per interval, the attacker creates
			 DAMAGE_EFFECT_Pandemonium_Fear_Explosion on the hit unit: power = the hit's physic, magic or half their
			 sum by damageType (else 1.0) x value[1], at the unit's centre, re-aimed by 31fd5ef0 when a user hits an
			 NPC, rotated as the hit unit, every other CreateInstance argument at its default.
			 June's own DamageEffect sets ATTACK_TRIGGER_TYPE = DTT_PANDEMONIUM_CHAOS on the explosion, a name June's
			 Enum.lua lacks, so the trigger reads DTT_NONE there and the explosion does not block itself.
			 March's data has no DAMAGE_EFFECT_Pandemonium_Fear_Explosion yet (Phase 13), so CreateInstance makes nothing.
*/
/*virtual*/ void CX2CustomFunctionByBuffPandemoniumFear::FunctionInDamageReact( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{

	if ( 0.f < damageData_.m_fHpPercentUp
		|| CX2DamageManager::RT_CREATE_DAMAGE_EFFECT == damageData_.reActType
		|| CX2DamageManager::RT_NO_DAMAGE == damageData_.reActType
		|| CX2DamageManager::RT_DUMMY_DAMAGE == damageData_.reActResult
		|| CX2DamageManager::RT_SENSOR == damageData_.reActResult
		|| CX2DamageManager::HT_GROUND_HIT == damageData_.hitType
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| true == damageData_.IsAttackTrigger( CX2DamageManager::DTT_PANDEMONIUM_FEAR )	/// June 32610fb0( 6 ) @3236e306 (16-step3c)
#else SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| CX2DamageManager::DTT_PANDEMONIUM_FEAR == static_cast<int>( damageData_.m_eDamageTrigger )
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| true == damageData_.m_bNoDamage
		|| null == damageData_.optrAttackerGameUnit
		|| false == m_checkElapsedTime.CheckAndResetElapsedTime() )
		return;

	CX2GameUnit* pAttackerGameUnit = damageData_.optrAttackerGameUnit.GetObservable();
	if ( NULL == pAttackerGameUnit )
		return;

	float fPowerRate = 1.0f;
	switch ( damageData_.damageType )
	{
	case CX2DamageManager::DT_PHYSIC:
		fPowerRate = damageData_.damage.fPhysic;
		break;

	case CX2DamageManager::DT_MAGIC:
		fPowerRate = damageData_.damage.fMagic;
		break;

	case CX2DamageManager::DT_MIX:
		fPowerRate = ( damageData_.damage.fMagic + damageData_.damage.fPhysic ) * 0.5f;
		break;

	default:
		break;
	}

	D3DXVECTOR3 vPos = GetUnitCenterPos( pGameUnit_ );
	if ( CX2GameUnit::GUT_USER == pAttackerGameUnit->GetGameUnitType() && CX2GameUnit::GUT_NPC == pGameUnit_->GetGameUnitType() )
		GetTargetHitPos( pAttackerGameUnit, pGameUnit_, true, 1, vPos );

	fPowerRate *= m_fPowerRate;

	g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerGameUnit, L"DAMAGE_EFFECT_Pandemonium_Fear_Explosion", fPowerRate,
		vPos, pGameUnit_->GetRotateDegree(), pGameUnit_->GetRotateDegree() );
}
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}
//{{ Iruha : 2026-10-05 // June buff port Phase 9b (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
/// June Enum.lua UNIT_TYPE: March's CX2Unit::UNIT_TYPE ends at UT_ADD (9), so the Rose and Ain checks use the numbers
static const int UT_ROSE = 11;
static const int UT_AIN = 12;
/// June Enum.lua RNSI_SA_LDN_SINISTER_MIND (no March SKILL_ID)

/// June 31e4d0e0: the user's GetUnit()->GetUnitTemplet()->m_UnitType ([[user +0x867c] +0xaf0] -> [0]), 0 without one
static int GetUserUnitType( const CX2GUUser* pUser_ )
{
	if ( NULL == pUser_ || NULL == pUser_->GetUnit() || NULL == pUser_->GetUnit()->GetUnitTemplet() )
		return 0;
	return static_cast<int>( pUser_->GetUnit()->GetUnitTemplet()->m_UnitType );
}

/// CX2Game slot 40 (322cca00, GetUserUnitByUID) with the creator UID, as every 9b class does
static CX2GUUser* GetCreatorUser( const UidType uidCreator_ )
{
	return ( NULL != g_pX2Game ? g_pX2Game->GetUserUnitByUID( uidCreator_ ) : NULL );
}

/// Mark of Commander's and Pandemonium Fear's DamageReact guards (June 3236e2a0 / 32370430 @3237046b-32370509), with the
/// class's own damage trigger: false (no reaction) for a heal-up hit, a damage-effect or no-damage react type, a dummy or
/// sensor result, a ground hit, the trigger itself, a NO_DAMAGE hit or a hit without an attacker.
static bool CustomFunctionDamageReactGuards( const CX2DamageManager::DamageData& damageData_, const int iTrigger_ )
{
	if ( 0.f < damageData_.m_fHpPercentUp
		|| CX2DamageManager::RT_CREATE_DAMAGE_EFFECT == damageData_.reActType
		|| CX2DamageManager::RT_NO_DAMAGE == damageData_.reActType
		|| CX2DamageManager::RT_DUMMY_DAMAGE == damageData_.reActResult
		|| CX2DamageManager::RT_SENSOR == damageData_.reActResult
		|| CX2DamageManager::HT_GROUND_HIT == damageData_.hitType
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| true == damageData_.IsAttackTrigger( iTrigger_ )	/// June 32610fb0( 7 ) @323704cb (16-step3c)
#else SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| iTrigger_ == static_cast<int>( damageData_.m_eDamageTrigger )
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
		|| true == damageData_.m_bNoDamage
		|| null == damageData_.optrAttackerGameUnit )
		return false;
	return true;
}

/** @function : FunctionInDamageReact
	@brief : June 32370430. The guards with trigger 7 (June Enum.lua DTT_MOONLIGHT_RHAPSODY), then the stack count of the
			 last live BTI 439 factor whose count is above 0 (GetBuffFactor 3229fd80, KBuffFactor +0x34). With none, nothing
			 happens. Otherwise the attacker plays EffectSet_MOONLIGHT_RHAPSODY_Debuff_Explosion_Accumulation_<count> (counts
			 1-3; any other count plays nothing) at the hit's impact point (+0x460), hyper when its remaining hyper time is
			 above 0, with power = value[1] (x ( 1 + 3201ad10( 29024 ) ) for a user attacker; 3201ad10 is 0 here, Phase 9a),
			 and the debuff erases itself (3229e160). June passes PlayEffectSet one more trailing 1.0 than March's has.
*/
/*virtual*/ void CX2CustomFunctionByBuffMoonlightRhapsody::FunctionInDamageReact( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
	const int DTT_MOONLIGHT_RHAPSODY = 7;
	if ( false == CustomFunctionDamageReactGuards( damageData_, DTT_MOONLIGHT_RHAPSODY ) )
		return;

	CX2GameUnit* pAttackerGameUnit = damageData_.optrAttackerGameUnit.GetObservable();
	if ( NULL == pAttackerGameUnit )
		return;

	const D3DXVECTOR3 vImpactPoint = damageData_.impactPoint;

	int iAccumulationCount = 0;
	vector<KBuffFactor> vecBuffFactor;
	pGameUnit_->GetBuffFactor( vecBuffFactor );
	BOOST_FOREACH( const KBuffFactor& kBuffFactor, vecBuffFactor )
	{
		if ( BTI_DEBUFF_MOONLIGHT_RHAPSODY == kBuffFactor.m_BuffIdentity.m_eBuffTempletID && 0 != kBuffFactor.m_ucAccumulationCountNow )
			iAccumulationCount = kBuffFactor.m_ucAccumulationCountNow;
	}
	if ( iAccumulationCount <= 0 )
		return;

	float fPowerRate = m_fPowerRate;
	if ( CX2GameUnit::GUT_USER == pAttackerGameUnit->GetGameUnitType() )
	{
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		/// buff port 16-step3b7 item 1: June @323705a2 3201ad10( 0x7160 = 29024 ) on the attacker user (edi), + 1.0 below. 29024 is a
		/// June-only skill (no March SKILL_ID), so no equipped slot files a rate under it: 0 until that skill is ported
		const float fSkillDamageAddRate = static_cast<CX2GUUser*>( pAttackerGameUnit )->GetSkillAdditionalPowerRate( CX2SkillTree::RNSI_SA_ADE_MOONLIGHT_RHAPSODY );
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
		const float fSkillDamageAddRate = 0.0f;	/// 3201ad10( 0x7160 ), see Phase 9a
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		fPowerRate = ( fSkillDamageAddRate + 1.0f ) * fPowerRate;
	}

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
	{
		const WCHAR* pwszEffectSetName = NULL;
		switch ( iAccumulationCount )
		{
		case 1: pwszEffectSetName = L"EffectSet_MOONLIGHT_RHAPSODY_Debuff_Explosion_Accumulation_1"; break;
		case 2: pwszEffectSetName = L"EffectSet_MOONLIGHT_RHAPSODY_Debuff_Explosion_Accumulation_2"; break;
		case 3: pwszEffectSetName = L"EffectSet_MOONLIGHT_RHAPSODY_Debuff_Explosion_Accumulation_3"; break;
		default: break;
		}

		if ( NULL != pwszEffectSetName )
		{
			const bool bHyper = ( 0.0f < pAttackerGameUnit->GetRemainHyperModeTime() );
			g_pX2Game->GetEffectSet()->PlayEffectSet( pwszEffectSetName, pAttackerGameUnit, NULL, bHyper, fPowerRate, -1.f,
				D3DXVECTOR3( 1.f, 1.f, 1.f ), true, vImpactPoint );
		}
	}

	pGameUnit_->EraseBuffTempletFromGameUnit( BTI_DEBUFF_MOONLIGHT_RHAPSODY );
}

/** @function : CX2CustomFunctionByBuffServoNitro
	@brief : June 323698b0: a user unit is kept (+0x10) and, for Rose, 32a800a0( 1 ) runs on it
*/
CX2CustomFunctionByBuffServoNitro::CX2CustomFunctionByBuffServoNitro( const KBuffIdentity& BuffIdentity_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_pUser( NULL )
{
	if ( NULL != pGameUnit_ && CX2GameUnit::GUT_USER == pGameUnit_->GetGameUnitType() )
	{
		m_pUser = pGameUnit_;
		CX2GUUser* pUser = static_cast<CX2GUUser*>( pGameUnit_ );
		if ( UT_ROSE == GetUserUnitType( pUser ) )
			pUser->RoseServoNitro( true );
	}
}

/** @function : ~CX2CustomFunctionByBuffServoNitro
	@brief : June 3236f390: while the game still has the user (CX2Game 322ccf50 = IsValidUnit), BTI 721 is erased and,
			 for Rose, 32a800a0( 0 ) runs
*/
/*virtual*/ CX2CustomFunctionByBuffServoNitro::~CX2CustomFunctionByBuffServoNitro()
{
	if ( NULL == g_pX2Game || NULL == m_pUser || false == g_pX2Game->IsValidUnit( m_pUser ) )
		return;

	m_pUser->EraseBuffTempletFromGameUnit( BTI_BUFF_NITRO_SERVO_ADD_STAT_IN_AIR );
	CX2GUUser* pUser = static_cast<CX2GUUser*>( m_pUser );
	if ( UT_ROSE == GetUserUnitType( pUser ) )
		pUser->RoseServoNitro( false );
}

/** @function : FunctionInCommonStateStart
	@brief : June 32369950: a user whose next frame is in the air (+0x7e1c = IsOnSomethingFuture() false) gets
			 BFI_BUFF_NITRO_SERVO_ADD_STAT_IN_AIR (slot 135 with level 1)
*/
/*virtual*/ void CX2CustomFunctionByBuffServoNitro::FunctionInCommonStateStart( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( NULL == pGameUnit_ || CX2GameUnit::GUT_USER != pGameUnit_->GetGameUnitType() )
		return;

	if ( false == static_cast<CX2GUUser*>( pGameUnit_ )->IsOnSomethingFuture() )
#ifdef SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF
		pGameUnit_->SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_NITRO_SERVO_ADD_STAT_IN_AIR, 1, NULL, 0, false );	/// S3e: June 3236997f, no skip
#else SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF
		pGameUnit_->SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_NITRO_SERVO_ADD_STAT_IN_AIR, 1 );
#endif SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF
}

/** @function : FunctionInCommonStateEnd
	@brief : June 32369990: a user whose next frame is on a line loses BTI 721 (3229e160)
*/
/*virtual*/ void CX2CustomFunctionByBuffServoNitro::FunctionInCommonStateEnd( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( NULL == pGameUnit_ || CX2GameUnit::GUT_USER != pGameUnit_->GetGameUnitType() )
		return;

	if ( true == static_cast<CX2GUUser*>( pGameUnit_ )->IsOnSomethingFuture() )
		pGameUnit_->EraseBuffTempletFromGameUnit( BTI_BUFF_NITRO_SERVO_ADD_STAT_IN_AIR );
}

/// June 32369bd0 (the creator): 1.0 without one. Else ( GetHyperModeTime() (slot 45) == 0 ? 1.0 : 1.2 ), and when
/// RNSI_SA_LDN_SINISTER_MIND is equipped (31fca8e0, March GetEquippedActiveSkillTemplet; June swaps the id for its memo
/// first, a no-op here) times, in June's order, the templet's power rate at the learned level (< 1 -> 1; 32071720 =
/// GetSkillPowerRateValue), GetAdditionalAttackByType( AT_SPECIAL ) (32274d60), GetSkillDamageRateByBuff (31fd7cc0) and
/// ( 1 + 3201ad10 ), 3201ad10 being 0 here (Phase 9a).
static float SinisterMindPowerRate( CX2GUUser* pCreator_ )
{
	if ( NULL == pCreator_ )
		return 1.0f;

	float fRate = ( 0.0f == pCreator_->GetHyperModeTime() ? 1.0f : 1.2f );
	const CX2SkillTree::SKILL_ID eSkillID = static_cast<CX2SkillTree::SKILL_ID>( CX2SkillTree::RNSI_SA_LDN_SINISTER_MIND );
	const CX2SkillTree::SkillTemplet* pSkillTemplet = pCreator_->GetEquippedActiveSkillTemplet( eSkillID );
	if ( NULL != pSkillTemplet )
	{
		int iLevel = ( NULL != pCreator_->GetUnit() ? pCreator_->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID ) : 0 );
		if ( iLevel < 1 )
			iLevel = 1;

#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		/// buff port 16-step3b7 item 1: June @32369c98 3201ad10( 0x7560 ) on the creator (esi), + 1.0 below; June-only skill, 0 as above
		const float fSkillDamageAddRate = pCreator_->GetSkillAdditionalPowerRate( CX2SkillTree::RNSI_SA_LDN_SINISTER_MIND );
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
		const float fSkillDamageAddRate = 0.0f;	/// 3201ad10( 0x7560 ), see Phase 9a
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		fRate = pSkillTemplet->GetSkillPowerRateValue( iLevel ) * fRate;
		fRate = pCreator_->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL ) * fRate;
		fRate = pCreator_->GetSkillDamageRateByBuff( CX2SkillTree::RNSI_SA_LDN_SINISTER_MIND ) * fRate;
		fRate = ( fSkillDamageAddRate + 1.0f ) * fRate;
	}
	return fRate;
}

/** @function : ~CX2CustomFunctionByBuffSinisterMind
	@brief : June 3236f480: the seed effect set stops; while the creator user and the unit are still in the game, a last
			 DamageEffect_SA_LDN_Sinister_Mind_Seed_Damage goes off at the unit (a real hit, unlike FrameMove's)
*/
/*virtual*/ CX2CustomFunctionByBuffSinisterMind::~CX2CustomFunctionByBuffSinisterMind()
{
	if ( NULL == g_pX2Game )
		return;

	if ( NULL != g_pX2Game->GetEffectSet() )
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hSeedEffectSet );

	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	if ( NULL != pCreator && true == g_pX2Game->IsValidUnit( m_pGameUnit ) )
		CreateSeedDamage( pCreator, m_pGameUnit, false );
}

/** @function : CreateSeedDamage
	@brief : June 3236e915-3236ea22 (FrameMove) and 3236f5a7-3236f68d (dtor): the creator's
			 DamageEffect_SA_LDN_Sinister_Mind_Seed_Damage at the unit's GetPos( false ), rotated as the creator, power from
			 32369bd0, every other CreateInstance argument at its default; its physic and magic damage x the count, and
			 FrameMove's one is a dummy hit (reActType RT_DUMMY_DAMAGE, CEffect +0x224 = 7)
*/
void CX2CustomFunctionByBuffSinisterMind::CreateSeedDamage( CX2GUUser* pCreator_, CX2GameUnit* pGameUnit_, const bool bDummy_ )
{
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == pGameUnit_ )
		return;

	CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pCreator_, L"DamageEffect_SA_LDN_Sinister_Mind_Seed_Damage",
		SinisterMindPowerRate( pCreator_ ), pGameUnit_->GetPos( false ), pCreator_->GetRotateDegree(), pCreator_->GetRotateDegree() );
	if ( NULL == pEffect )
		return;

	pEffect->GetDamageData().damage.fPhysic = static_cast<float>( m_iCount ) * pEffect->GetDamageData().damage.fPhysic;
	pEffect->GetDamageData().damage.fMagic = static_cast<float>( m_iCount ) * pEffect->GetDamageData().damage.fMagic;
	if ( true == bDummy_ )
		pEffect->GetDamageData().reActType = CX2DamageManager::RT_DUMMY_DAMAGE;
}

/** @function : FunctionInCommonStateStart
	@brief : June 323697d0: the unit's state at its state start (vslot 67)
*/
/*virtual*/ void CX2CustomFunctionByBuffSinisterMind::FunctionInCommonStateStart( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( NULL != pGameUnit_ )
		m_iStateID = pGameUnit_->GetNowStateID();
}

/** @function : FunctionInCommonFrameMove
	@brief : June 3236e770. Needs the unit and the creator user. With no seed effect set yet: the level goes back to 0 and
			 EffectSet_SA_LDN_SINISTER_MIND_Seed_Plant_1 plays on the unit. Then, each time the timer is not above 0, it
			 restarts at the interval and the seed damage goes off (a dummy hit); else it runs down by the unit's elapsed time.
*/
/*virtual*/ void CX2CustomFunctionByBuffSinisterMind::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	if ( NULL == pGameUnit_ || NULL == pCreator )
		return;

	if ( INVALID_EFFECTSET_HANDLE == m_hSeedEffectSet && NULL != g_pX2Game->GetEffectSet() )
	{
		m_iSeedLevel = 0;
		m_hSeedEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_LDN_SINISTER_MIND_Seed_Plant_1", pGameUnit_ );
	}

	if ( !( 0.0f < m_fRemainDamageTime ) )
	{
		m_fRemainDamageTime = m_fDamageInterval;
		CreateSeedDamage( pCreator, pGameUnit_, true );
	}
	else
		m_fRemainDamageTime -= pGameUnit_->GetElapsedTime();
}

/** @function : FunctionInAttackResultByType
	@brief : June 3236e490 (the seeded unit's own hit; slot 5). Only the first hit of the state the unit started last counts
			 (DamageData +0x5e0 = the attacker's state, from SetDamageExceptionProcess). Below the full count the count goes
			 up, and when count / ( max / 3 ) passes the level (June never stores the level, so every counted hit past the
			 first step replays it) the seed effect set is swapped for Seed_Plant_2 (level 1) or Seed_Plant_3. At the full
			 count the unit gets BFI_DEBUFF_SINISTER_MIND_ARMOR_BREAK from the creator (slot 136, user, level 1).
			 June divides by max / 3 without a check, which is 0 below a max of 3: a divide-by-zero crash, ported as June
			 has it (Phase 9c, JUNE_BUGS #33). A DEBUG line names the max just before the divide.
*/
/*virtual*/ void CX2CustomFunctionByBuffSinisterMind::FunctionInAttackResultByType( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
	if ( m_iStateID != damageData_.m_iAttackerStateID )
		return;

	if ( m_iCount < m_iMaxCount )
	{
		++m_iCount;
		const int iStep = static_cast<int>( static_cast<double>( m_iMaxCount ) / 3.0 );
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		if ( 0 == iStep )	/// Phase 9c: the last line a crash bundle's ErrorLog.txt holds before June's divide
			CX2BuffCapture::Line( "Sinister Mind: max count %d gives a 0 step; June divides by it at 3236e516 (JUNE_BUGS #33): divide by zero next", m_iMaxCount );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
		const int iLevel = m_iCount / iStep;	/// June 3236e506-3236e516: idiv, no zero check
		if ( m_iSeedLevel < iLevel && NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hSeedEffectSet );
			m_hSeedEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( ( 1 == iLevel
				? L"EffectSet_SA_LDN_SINISTER_MIND_Seed_Plant_2" : L"EffectSet_SA_LDN_SINISTER_MIND_Seed_Plant_3" ), pGameUnit_ );
		}
	}
	else if ( NULL != pGameUnit_ )
		pGameUnit_->SetBuffFactorToGameUnitByBuffFactorIDAndCreatorID( BFI_DEBUFF_SINISTER_MIND_ARMOR_BREAK, m_uidCreator, true, 1 );

	m_iStateID = -1;
}

/** @function : CX2CustomFunctionBySummonNpcTarget
	@brief : June 3236f6b0: with the creator user in the game, its June-only summon-target calls run on it:
			 32011960( true ) (clear the old target and its 819/894 marks), 31fd0590( unit ) (aim at this unit),
			 32011ab0() (hand the target to every ally summon)
*/
CX2CustomFunctionBySummonNpcTarget::CX2CustomFunctionBySummonNpcTarget( const KBuffIdentity& BuffIdentity_, const UidType uidCreator_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_uidCreator( uidCreator_ ), m_pGameUnit( pGameUnit_ )
{
	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	if ( NULL == pCreator )
		return;

	pCreator->ClearSummonTarget( true );
	pCreator->SetSummonTarget( m_pGameUnit );
	pCreator->ApplySummonTargetToSummons();
}

/** @function : ~CX2CustomFunctionBySummonNpcTarget
	@brief : June 3236f7c0: 32011960( false ) on the creator user, if it is still in the game
*/
/*virtual*/ CX2CustomFunctionBySummonNpcTarget::~CX2CustomFunctionBySummonNpcTarget()
{
	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	if ( NULL != pCreator )
		pCreator->ClearSummonTarget( false );
}

/// the creator user when it is a Rose (the check every Chain Viper hook makes), else NULL
CX2GUUser* CX2CustomFunctionByChainViper::GetRoseCreator() const
{
	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	return ( UT_ROSE == GetUserUnitType( pCreator ) ? pCreator : NULL );
}

/** @function : CX2CustomFunctionByChainViper
	@brief : June 3236f890. For a Rose creator: the count is the unit's last-hit serial (+0x40dc) + 1. A value[0] of 0 or
			 less is overwritten with it, in the factor June was handed (a JUNE_BUGS row); a value[0] above 0 becomes the
			 count instead. Then Rose's 32ad0e90( unit, count ).
*/
CX2CustomFunctionByChainViper::CX2CustomFunctionByChainViper( const KBuffIdentity& BuffIdentity_, const KBuffBehaviorFactor& kBuffBehaviorFactor_,
	const UidType uidCreator_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_uidCreator( uidCreator_ ), m_pGameUnit( pGameUnit_ ), m_iCount( 0 )
{
	CX2GUUser* pCreator = GetRoseCreator();
	if ( NULL == pCreator || NULL == m_pGameUnit )
		return;

	m_iCount = m_pGameUnit->GetLastHitAttackSerial() + 1;
	KBuffBehaviorFactor& kWritableFactor = const_cast<KBuffBehaviorFactor&>( kBuffBehaviorFactor_ );
	if ( false == kWritableFactor.m_vecValues.empty() )
	{
		const int iValue = static_cast<int>( kWritableFactor.m_vecValues[0] );
		if ( iValue <= 0 )
			kWritableFactor.m_vecValues[0] = static_cast<float>( m_iCount );
		else
			m_iCount = iValue;
	}

	pCreator->RoseChainViperBegin( m_pGameUnit, m_iCount );
}

/** @function : ~CX2CustomFunctionByChainViper
	@brief : June 3236f9f0: Rose's 32ad0fd0( unit, count )
*/
/*virtual*/ CX2CustomFunctionByChainViper::~CX2CustomFunctionByChainViper()
{
	CX2GUUser* pCreator = GetRoseCreator();
	if ( NULL != pCreator )
		pCreator->RoseChainViperEnd( m_pGameUnit, m_iCount );
}

/** @function : FunctionInNpcFinalDamage
	@brief : June 3236fae0 (slot 6): Rose's 32ac2c50( unit, final damage, count ). In March the final damage lacks June's
			 last DamageReact steps (Phase 9a review, item 2).
*/
/*virtual*/ void CX2CustomFunctionByChainViper::FunctionInNpcFinalDamage( CX2GameUnit* pGameUnit_, IN const float* pfFinalDamage_, IN CX2DamageManager::DamageData* pDamageData_ )
{
	CX2GUUser* pCreator = GetRoseCreator();
	if ( NULL != pCreator && NULL != pfFinalDamage_ )
		pCreator->RoseChainViperNpcFinalDamage( m_pGameUnit, *pfFinalDamage_, m_iCount );
}

/** @function : FunctionInSetBuffFactor
	@brief : June 3236fb90 (slot 7): a BTI_DEBUFF_BLEEDING factor (+0x28 == 99) applied to the unit calls Rose's
			 32ac2d50( unit, factor, count )
*/
/*virtual*/ void CX2CustomFunctionByChainViper::FunctionInSetBuffFactor( CX2GameUnit* pGameUnit_, IN const CX2BuffFactor& buffFactor_ )
{
	if ( BTI_DEBUFF_BLEEDING != buffFactor_.GetBuffTempletID() )
		return;

	CX2GUUser* pCreator = GetRoseCreator();
	if ( NULL != pCreator )
		pCreator->RoseChainViperBuffApplied( m_pGameUnit, buffFactor_, m_iCount );
}

/** @function : FunctionInDamageReact
	@brief : June 323697f0: a command hit (attackType 2) on the debuffed unit gets +1.0 on DamageData +0x5d4, which both
			 DamageReacts add to the critical rate
*/
/*virtual*/ void CX2CustomFunctionByAtomicBlaster::FunctionInDamageReact( CX2GameUnit* pGameUnit_, IN CX2DamageManager::DamageData& damageData_ )
{
	if ( CX2DamageManager::AT_COMMAND == damageData_.attackType )
		damageData_.m_fAddCriticalRate += 1.0f;
}

/** @function : CX2CustomFunctionByAttackMe
	@brief : June 3236fc50: the creator is looked up as an NPC first, then as a user (CX2Game slot 50 with not-user 1,
			 then 0); an NPC unit's AI then targets it (3275b390: AI +0x3c observer = the creator; March SetTargetGameUnit).
			 The class keeps no other state and its dtor is the base one, so the target is not reset when the debuff ends.
*/
CX2CustomFunctionByAttackMe::CX2CustomFunctionByAttackMe( const KBuffIdentity& BuffIdentity_, const UidType uidCreator_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_uidCreator( uidCreator_ ), m_pGameUnit( pGameUnit_ )
{
	if ( NULL == g_pX2Game || NULL == m_pGameUnit )
		return;

	CX2GameUnit* pCreator = g_pX2Game->GetNPCUnitByUID( static_cast<int>( m_uidCreator ) );
	if ( NULL == pCreator )
		pCreator = g_pX2Game->GetUserUnitByUID( m_uidCreator );

	if ( NULL != pCreator && CX2GameUnit::GUT_NPC == m_pGameUnit->GetGameUnitType() )
	{
		CX2GUNPC* pNpc = static_cast<CX2GUNPC*>( m_pGameUnit );
		if ( NULL != pNpc->GetNPCAI() )
			pNpc->GetNPCAI()->SetTargetGameUnit( pCreator );
	}
}

/** @function : CX2CustomFunctionByGeistHerrschaft
	@brief : June 3236fd90. With the unit and the creator user in the game: the creator's DamageEffect_Debuff_Geist_Herrschaft
			 at the unit's bone nearest its GetPos( false ) (32278580, then GetBonePos, slot 33), rotated as the creator,
			 power = the creator info's power rate, every other CreateInstance argument at its default. Its handle is kept
			 and its lock-on unit is this one (LockOnData +0x18 UID for a user, +0x20 NPC UID otherwise).
*/
CX2CustomFunctionByGeistHerrschaft::CX2CustomFunctionByGeistHerrschaft( const KBuffIdentity& BuffIdentity_, const UidType uidCreator_,
	const float fCreatorPowerRate_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_fPowerRate( fCreatorPowerRate_ ), m_uidCreator( uidCreator_ ), m_pGameUnit( pGameUnit_ )
	, m_hDamageEffect( INVALID_DAMAGE_EFFECT_HANDLE )
{
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == m_pGameUnit )
		return;

	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	if ( NULL == pCreator )
		return;

	const std::wstring wstrFrameName = GetNearestCollisionFrameName( m_pGameUnit, m_pGameUnit->GetPos( false ) );
	const D3DXVECTOR3 vBonePos = m_pGameUnit->GetBonePos( wstrFrameName.c_str() );
	CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pCreator, L"DamageEffect_Debuff_Geist_Herrschaft",
		m_fPowerRate, vBonePos, pCreator->GetRotateDegree(), pCreator->GetRotateDegree() );
	if ( NULL == pEffect )
		return;

	m_hDamageEffect = pEffect->GetHandle();
	if ( CX2GameUnit::GUT_USER == m_pGameUnit->GetGameUnitType() )
		pEffect->GetLockOnData().m_LockOnUnitUID = m_pGameUnit->GetUnitUID();
	else
		pEffect->GetLockOnData().m_LockOnNPCUID = static_cast<int>( m_pGameUnit->GetUnitUID() );
}

/** @function : ~CX2CustomFunctionByGeistHerrschaft
	@brief : June 32370000: the damage effect is destroyed (DestroyInstanceHandle, not silently)
*/
/*virtual*/ CX2CustomFunctionByGeistHerrschaft::~CX2CustomFunctionByGeistHerrschaft()
{
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffect, false );
}

/** @function : FunctionInCommonFrameMove
	@brief : June 323700c0: while the effect lives, a missing unit or one at 0 HP or less destroys it
*/
/*virtual*/ void CX2CustomFunctionByGeistHerrschaft::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( INVALID_DAMAGE_EFFECT_HANDLE == m_hDamageEffect || NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
		return;

	if ( NULL == pGameUnit_ || pGameUnit_->GetNowHp() <= 0.0f )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffect, false );
}

/// June 32370150: the creator user when it is an Ain, else NULL
CX2GUUser* CX2CustomFunctionByModer::GetAinCreator() const
{
	CX2GUUser* pCreator = GetCreatorUser( m_uidCreator );
	return ( UT_AIN == GetUserUnitType( pCreator ) ? pCreator : NULL );
}

/** @function : CX2CustomFunctionByModer
	@brief : June 32370210: for an Ain creator, its 32a4ddb0( unit UID, unit type ), and the unit is marked (+0x10)
*/
CX2CustomFunctionByModer::CX2CustomFunctionByModer( const KBuffIdentity& BuffIdentity_, const UidType uidCreator_, CX2GameUnit* pGameUnit_ )
	: CX2CustomFunctionByBuff( BuffIdentity_ ), m_bMarked( false ), m_uidCreator( uidCreator_ ), m_pGameUnit( pGameUnit_ )
{
	if ( NULL == m_pGameUnit )
		return;

	CX2GUUser* pAin = GetAinCreator();
	if ( NULL == pAin )
		return;

	pAin->AinModerMark( m_pGameUnit->GetUnitUID(), static_cast<int>( m_pGameUnit->GetGameUnitType() ) );
	m_bMarked = true;
}

/** @function : ~CX2CustomFunctionByModer
	@brief : June 32370300: a marked unit is unmarked through the Ain creator (32a4de20)
*/
/*virtual*/ CX2CustomFunctionByModer::~CX2CustomFunctionByModer()
{
	if ( false == m_bMarked )
		return;

	CX2GUUser* pAin = GetAinCreator();
	if ( NULL != pAin )
	{
		pAin->AinModerUnmark();
		m_bMarked = false;
	}
}

/** @function : FunctionInCommonFrameMove
	@brief : June 323703b0: a marked unit that is gone or at 0 HP or less is unmarked through the Ain creator (32a4de20);
			 without the creator the mark stays
*/
/*virtual*/ void CX2CustomFunctionByModer::FunctionInCommonFrameMove( CX2GameUnit* pGameUnit_, KLuaManager& luaManager_ )
{
	if ( false == m_bMarked )
		return;

	if ( NULL != pGameUnit_ && 0.0f < pGameUnit_->GetNowHp() )
		return;

	CX2GUUser* pAin = GetAinCreator();
	if ( NULL != pAin )
	{
		pAin->AinModerUnmark();
		m_bMarked = false;
	}
}

/** @function : FunctionInErase
	@brief : June 3236d3f0 (slot 8, when the buff is erased): 32082790( unit, 1 ) reserves the unit's death, which
			 CX2GUUser::OnFrameMove carries out next frame (an NPC's flag is never read)
*/
/*virtual*/ void CX2CustomFunctionByBuffDeathSign::FunctionInErase( CX2GameUnit* pGameUnit_ )
{
	if ( NULL != pGameUnit_ )
		pGameUnit_->SetReservedDeath( true );
}
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
//}}

#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
// 행동 불능 디버프 인가? 특정 BBT 를 가지고 있으면 참을 반환
// Ex. 블레이징 하트, 불꽃 개화
bool CX2BuffTemplet::IsNonControlDebuff ()
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
	/// June 32369cc0: only a debuff counts
	if ( !IsDeBuff() )
		return false;
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
	BOOST_FOREACH( CX2BuffBehaviorTempletPtr ptrBehavior, m_vecBehaviorPtr )
	{
		switch ( ptrBehavior->GetBuffBehaviorType() )
		{
		case BBT_DASH_IMPOSSIBLE :
		case BBT_ATTACK_IMPOSSIBLE :
		case BBT_REVERSE_LEFT_RIGHT :
		case BBT_CHANGE_UNIT_SCALE :
		case BBT_STUN :
		case BBT_FROZEN :
		case BBT_NEVER_MOVE :
		case BBT_SKILL_ATTACK_IMPOSSIBLE :
		case BBT_ZX_ATTACK_IMPOSSIBLE :
		case BBT_Z_ATTACK_IMPOSSIBLE :
		case BBT_X_ATTACK_IMPOSSIBLE :
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
		case BBT_JUMP_IMPOSSIBLE :		/// June 32369cc0 adds BBT 104
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
			return true;
			break;

		default :
			break;
		}
	}
	return false;
}
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편