/**@file : X2BuffBehaviorTemplet.cpp
   @breif : 버프의 행동을 정의하는 Templet 파일
*/

#pragma once

#include "StdAfx.h"
#include ".\X2BuffBehaviorTemplet.h"
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/// June buff port 4b-ii: June's buff code raises no popup (the user's 2026-10-04 decision: follow June, always
/// log). Every DISPLAY_ERROR in this file is collected by an active capture, else written once per buff and
/// text as a [JUNE_BUFF] line in ErrorLog.txt (CX2JuneBuffCapture::RuntimeError). Same trailing ';' as the original.
#undef DISPLAY_ERROR
#define DISPLAY_ERROR( wszError_ ) CX2BuffCapture::RuntimeError( wszError_ );
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

#define CASE_BEHAVIOR_TEMPLET( class_, type_ ) \
case type_: \
	{ \
		CX2BuffBehaviorTempletPtr ptrBehaviorTemplet = class_::CreateBuffBehaviorTempletPtr(); \
		if ( NULL != ptrBehaviorTemplet && ptrBehaviorTemplet->ParsingBehaviorTemplateMethod( luaManager_, STRINGIZEA2(type_) ) ) \
		{ \
			ptrBehaviorTemplet->SetType( type_ ); \
			vecBuffBehaviorTempletPtr_.push_back( ptrBehaviorTemplet ); \
		} \
		else \
			return DISPLAY_ERROR( STRINGIZE2(type_) ); \
	} break; \

/** @function : ParsingCombinationBehavior
	@brief : 버프의 행동이 어떤종류로 구성되어 있는지 알아내고, 구성된 타입별로 파싱
	@param : 읽어들이고 있는 루아스크립트의 루아매니저(luaManager_), 파싱성공한 BehaviorTempletPtr을 담을 vector(vecBuffBehaviorTempletPtr_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*static*/ bool CX2BuffBehaviorTemplet::ParsingCombinationBehavior( KLuaManager& luaManager_, OUT vector<CX2BuffBehaviorTempletPtr>& vecBuffBehaviorTempletPtr_ )
{
	vector<UINT> vecBehaviorType;

#ifdef EXCEPTION_BUFF_FACTOR_VER2
	bool bExceptionCase = true;
#endif // EXCEPTION_BUFF_FACTOR_VER2

	/// Behavior가 어떤 행동요소들의 조함으로 이루어 졌는지 얻어옴
	if ( CX2BuffTempletManager::GetCombination( luaManager_, vecBehaviorType ) )
	{
		vector<UINT>::iterator vItr = vecBehaviorType.begin();
		while ( vecBehaviorType.end() != vItr )		/// 조합에 포함된 행동요소들을 파싱
		{
#ifdef EXCEPTION_BUFF_FACTOR_VER2
			if( static_cast<BUFF_DISPLAYER_TYPE>( *vItr ) != BBT_REFLECT_MAGIC )
			{
				bExceptionCase = false;
			}
#endif // EXCEPTION_BUFF_FACTOR_VER2

			switch ( static_cast<BUFF_BEHAVIOR_TYPE>( *vItr ) )
			{
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeNowHpPerSecondBehaviorTemplet, BBT_CHANGE_NOW_HP_PER_SECOND )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeNowMpPerSecondBehaviorTemplet, BBT_CHANGE_NOW_MP_PER_SECOND )
			CASE_BEHAVIOR_TEMPLET( CX2BuffReverseLeftRightBehaviorTemplet, BBT_REVERSE_LEFT_RIGHT )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_DASH_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_ATTACK_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MAX_HP )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_PHYSIC_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MAGIC_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_PHYSIC_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MAGIC_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ANIM_SPEED )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_WALK_RUN_SPEED )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_JUMP_POWER )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ACCURACY )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_AVOIDANCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_CRITICAL_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_CRITICAL_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ANTI_CRITICAL_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_CRITICAL_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ADDITIONAL_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ADDITIONAL_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffCreateBuffFactortBehaviorTemplet, BBT_CREATE_BUFF_FACTOR )
			CASE_BEHAVIOR_TEMPLET( CX2BuffAbsorbEffectAttackBehaviorTemplet, BBT_ABSORB_EFFECT_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHARGE_MP_HIT_MELEE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHARGE_MP_HIT_RANGE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHARGE_MP_HITTED )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeHyperModeBehaviorTemplet, BBT_CHANGE_HYPER_MODE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_SUPER_ARMOR )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_MELEE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_RANGE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FINAL_DEFENCE_MULTIPLIER )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_RANGE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffResetSkillCoolTimeBehaviorTemplet, BBT_RESET_SKILL_COOL_TIME )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeUnitScaleBehaviorTemplet, BBT_CHANGE_UNIT_SCALE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_BLAZE_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_WATER_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_NATURE_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_WIND_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_LIGHT_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_DARK_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffCustomFunctionBehaviorTemplet, BBT_CUSTOM_FUNCTION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_DRAIN_HP_NORMAL_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffEffectSetWithDamageBehaviorTemplet, BBT_EFFECT_SET_WITH_DAMAGE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffStunBehaviorTemplet, BBT_STUN )
			CASE_BEHAVIOR_TEMPLET( CX2BuffFinishOtherBuffDebuffBehaviorTemplet, BBT_FINISH_OTHER_BUFF_DEBUFF )
			CASE_BEHAVIOR_TEMPLET( CX2BuffFreezeBehaviorTemplet, BBT_FROZEN )
			CASE_BEHAVIOR_TEMPLET( CX2BuffCanPassUnitBehaviorTemplet, BBT_CAN_PASS_UNIT )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_ATTACK_ALL_TEAM )
			CASE_BEHAVIOR_TEMPLET( CX2BuffNeverMoveBehaviorTemplet, BBT_NEVER_MOVE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MAX_MP )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeAttackByTypeBehaviorTemplet, BBT_CHANGE_ATTACK_BY_TYPE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ENCHANT_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffDoubleAttackBehaviorTemplet, BBT_DOUBLE_ATTACK )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeAttackSphereScaleBehaviorTemplet, BBT_CHANGE_ATTACK_SPHERE_SCALE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_DOWN_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffAddSkillLevelBehaviorTemplet, BBT_ADD_SKILL_LEVEL )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_SKILL_ATTACK_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_ZX_ATTACK_IMPOSSIBLE )
	#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
			CASE_BEHAVIOR_TEMPLET( CX2BuffReflectMagicBehaviorTemplet, BBT_REFLECT_MAGIC )
	#endif // SERV_ARA_CHANGE_CLASS_SECOND
	#ifdef FIX_SKILL_BALANCE_AISHA_LENA //JHKang
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeConsumeMpRate, BBT_CHANGE_CONSUME_MP_RATE )
	#endif //FIX_SKILL_BALANCE_AISHA_LENA
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
			// Z 키 사용 X 키 사용 불가, 버프 타입 추가
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_Z_ATTACK_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_X_ATTACK_IMPOSSIBLE )
			
			// 각 속성 별 인챈트 확률 증가
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_WATER_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_NATURE_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_WIND_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_LIGHT_ATTACK_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeEncahntAttackRateBehaviorTemplet, BBT_CHANGE_ATTRIBUTE_DARK_ATTACK_RATE )
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편

#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_HYPER_CHARGE_SPEED )
#endif //SERV_ADD_LUNATIC_PSYKER
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
			/// June buff port Phase 5: ChangeStat class for the seven new types (June 32e80450 factory)
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_HYPER_DURATION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_RANGE_COMMAND_MP_GAIN_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MELEE_COMMAND_MP_GAIN_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MELEE_COMMAND_DAMAGE_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_RANGE_COMMAND_DAMAGE_RATE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_NPC )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_USER )
			/// June buff port Phase 5b (June 32e80450 factory): 106/107 use the ChangeStat class, 93 its _HaveBuffOwner subclass (32e86740),
			/// 79 and 80-84 their own classes (32e86e90, 32e86c00)
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACKED )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_RANGE_ATTACKED )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner, BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeNowForceDownPerSecondBehaviorTemplet, BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStateByConditionBehaviorTemplet, BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStateByConditionBehaviorTemplet, BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStateByConditionBehaviorTemplet, BBT_CHANGE_PHYSIC_DEFENCE_BY_CONDITION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStateByConditionBehaviorTemplet, BBT_CHANGE_MAGIC_DEFENCE_BY_CONDITION )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStateByConditionBehaviorTemplet, BBT_CHANGE_MAX_HP_BY_CONDITION )
			/// June buff port Phase 5c (June 32e80450 factory): 78 uses the ChangeStat class, 88 its own class (vt 33535558); 92 deferred
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER )
			/// June buff port Phase 16: 92 is the ChangeStat class too (June factory case 92 @32e83509 builds 32e866f0, as 78 does)
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeStatBehaviorTemplet, BBT_CHANGE_MAXIMIZE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIgnoreDefenceBehaviorTemplet, BBT_IGNORE_DEFENCE )
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
			/// June buff port Phase 6 (June 32e80450 factory): 99 and 100 are byte-identical in June, so they share one class here
			CASE_BEHAVIOR_TEMPLET( CX2BuffDoubleAttackBuffBehaviorTemplet, BBT_DOUBLE_ATTACK_BUFF )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeSkillDamageBehaviorTemplet, BBT_CHANGE_SKILL_DAMAGE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_ID )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChargeMpHitBySkillIDBehaviorTemplet, BBT_CHARGE_MP_HIT_BY_SKILL_ID )
			CASE_BEHAVIOR_TEMPLET( CX2AddDamageOfSkillSlotType, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeRateBySkillTypeBehaviorTemplet, BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeRateBySkillTypeBehaviorTemplet, BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet, BBT_CHANGE_SKILL_COOL_TIME_SPEED )
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG
			/// June buff port Phase 7 (June 32e80450 factory): 104 is an identity type (June CreateAndInsertBuffIdentity 3227ad60), the others have their own classes
			CASE_BEHAVIOR_TEMPLET( CX2BuffIgnoreStateGuardBehaviorTemplet, BBT_IGNORE_STATE_GUARD )
			CASE_BEHAVIOR_TEMPLET( CX2BuffImmuneBuffDebuffBehaviorTemplet, BBT_IMMUNE_BUFF_DEBUFF )
			CASE_BEHAVIOR_TEMPLET( CX2BuffImmuneNonControlDebuffBehaviorTemplet, BBT_IMMUNE_NON_CONTROL_DEBUFF )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_JUMP_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffImmuneEDTBehaviorTemplet, BBT_IMMUNE_EDT )
			/// June buff port Phase 7b (June 32e80450 factory): 74, 77, 101, 102 and 110 are identity types (32e866b0, like 104); 97 has its own class (32e86c90)
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_DETONATION_IMPOSSIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_INVINCIBLE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet, BBT_CHANGE_MAX_CHARACTER_ADD_RESOURCE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_IGNORE_HP_DECREASE )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_DETONATION_IMPOSSIBLE_GAGE_RESET )
			CASE_BEHAVIOR_TEMPLET( CX2BuffIdentityBehaviorTemplet, BBT_AWAKE_IMPOSSIBLE )
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG

			case BBT_EMPTY:	/// 행동을 지정하지 않겠다고 명시적으로 해야함
				return true;
#ifdef EXCEPTION_BUFF_FACTOR_VER2
#ifndef SERV_ARA_CHANGE_CLASS_SECOND
			case BBT_REFLECT_MAGIC:
				++vItr;
				continue;
#endif SERV_ARA_CHANGE_CLASS_SECOND
#endif //EXCEPTION_BUFF_FACTOR_VER2
				break;

			default:
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
				return CX2BuffCapture::UnsupportedType( L"BBT", *vItr );	/// June buff port Phase 3: the [JUNE_BUFF] line names the type
#else SERV_IRUHADEV_JUNE_BUFF_LOADER
				return DISPLAY_ERROR( L"Unknown Behavior Templet Type" );	/// false 리턴
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
				break;
			}

			++vItr;
		}
	}

	if ( vecBuffBehaviorTempletPtr_.empty() 
#ifdef EXCEPTION_BUFF_FACTOR_VER2
		&& !bExceptionCase
#endif //EXCEPTION_BUFF_FACTOR_VER2
		)
	{
		return DISPLAY_ERROR( L"BEHAVIOR Templet is empty" );
	}
	else
		return true;
}

/** @function : ParsingBehaviorTemplateMethod
	@brief : 각 파생클래스에서 구현된 ParsingBehavior를 똑같은 전후 처리 사이에서 호출하기 위한 템플릿메소드
	@param : 읽어들이고 있는 루아스크립트의 루아매니저(luaManager_), 파싱성공한 BehaviorTempletPtr을 담을 vector(vecBuffBehaviorTempletPtr_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
bool CX2BuffBehaviorTemplet::ParsingBehaviorTemplateMethod( KLuaManager& luaManager_, const char* pszTableName_ )
{
	if ( luaManager_.BeginTable( pszTableName_ ) )
	{
		BOOST_SCOPE_EXIT( (&luaManager_) ) {
			luaManager_.EndTable();
		} BOOST_SCOPE_EXIT_END

		return ParsingBehavior( luaManager_ );
	}
	else
		return DISPLAY_ERROR( pszTableName_ );	
}

/** @function : SetFactorFromPacketTemplateMothod
	@brief : 패킷으로 부터 받은 버프 정보로 각 행동의 SetFactorFromPacket을 실행 시키는 템플릿 메소드
	@param : 서버로 부터 받은 버프정보(kBuffFactor_), 적용할 게임유닛(pGameUnit_)
	@return : 성공(true), 실패(false)
*/
bool CX2BuffBehaviorTemplet::SetFactorFromPacketTemplateMothod( const KBuffFactor& kBuffFactor_, CX2GameUnit* pGameUnit_ )
{
	BOOST_FOREACH( const KBuffBehaviorFactor& kFactor_, kBuffFactor_.m_vecBuffBehaviorFactors )
	{
		if ( GetType() == kFactor_.m_uiType )
		{
			SetFactorFromPacket( kBuffFactor_, kFactor_, pGameUnit_ );
			return true;
		}	
	}

	return DISPLAY_ERROR( L"SetFactorFromPacket Error" );
}

/** @function : ParsingBehavior
	@brief :초당 HP가 증감되는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeNowHpPerSecondBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	float fTimeInterval = 0.0f;	/// 몇초간격으로 HP가 변경되는가
	LUA_GET_VALUE_RETURN( luaManager_, "TIME_INTERVAL", fTimeInterval, 0.0f, return DISPLAY_ERROR( L"TIME_INTERVAL" ) );
	m_CheckElapsedTimeForInterval.SetTargetTime( fTimeInterval );

	LUA_GET_VALUE_RETURN( luaManager_, "DRAW_TEXT", m_bDrawText, false, return DISPLAY_ERROR( L"DRAW_TEXT" ) );
	return true;
}

/** @function : OnFrameMove
	@brief : interval 타이머 Update, HP 변경
	@param : HP가 변경되야 하는 게임유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowHpPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowHpPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_CheckElapsedTimeForInterval.OnFrameMove( fElapsedTime_ );	/// Interval 타이머 진행
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove();	/// Interval 타이머 진행
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_delegateChangeHpByTypeFunc( pGameUnit_ );		/// HP 변경
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	/// June 32e798d0. The restack is the multiplier formula for every change type: June has no BCT_PERCENT
	/// branch (March's ADJUST_BUFF_ACCMULATION_CALCULATE), and both failures return false without an error.
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	const BUFF_CHANGE_TYPE eChangeType
		= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] );
	if ( eChangeType != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	SelectDelegate();

	const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;
	const float fNewValueOrPercent = m_delegateGetValueOrPercentPerSecondFunc( pBehaviorFactor, fRegist );

	if ( DidStart() )
	{
		const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
		if ( uiAccumulationCountBefore > 0 )
		{
			const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
			const float fOldValueOrPercent = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
			m_fResultValue = ( abs(fNewValueOrPercent) > abs(fOldValueOrPercent) ? fNewValueOrPercent : fOldValueOrPercent );
			m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );
		}
		else
			m_fResultValue = ( abs(fNewValueOrPercent) > abs(m_fResultValue) ? fNewValueOrPercent : m_fResultValue );
	}
	else
		m_fResultValue = fNewValueOrPercent;

	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		const BUFF_CHANGE_TYPE eChangeType 
			= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] );

		if ( eChangeType == m_eChangeType )
		{
			/// 변경타입(CHANGE_TYPE)에 따라 수행 되야할 함수를 선택
			SelectDelegate();
			/// 저항을 무시하는 것도 아니고 영향을 받을 저항이 있는 경우 저항값을 얻어옴

// 			const float fRegist = ( BuffFactor_.GetIgnoreRegistForRate() ? 
// 				0.0f : pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE );

			const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;

			// 현재 효과의 적용값
			const float fNewValueOrPercent = m_delegateGetValueOrPercentPerSecondFunc( pBehaviorFactor, fRegist );

			if ( DidStart() )	/// 중첩 또는 교체라면
			{
				/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
				const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
				if ( uiAccumulationCountBefore > 0 )	/// 중첩 이라면
				{
#ifdef ADJUST_BUFF_ACCMULATION_CALCULATE
					if( eChangeType == BCT_PERCENT )
					{
						//바뀐 공식은 첫 효과가 1중첩이 된 것이라고 생각하고 작성됨.
						//따라서 uiAccumulationCountBefore에 1을 더해줌.
						m_fResultValue = 1 + ( ( fNewValueOrPercent - 1.0f ) * ( uiAccumulationCountBefore + 1 ) );
						if( m_fResultValue < 0.0f )
						{
							//m_fREsultValue값이 0보다 작지 않도록 값들을 계산해서 적어야 한다.(기획파트에서 정함)
							DISPLAY_ERROR( L"m_fResultValue error" );
							m_fResultValue = max( m_fResultValue, 0.0f );
						}
					}
					else
#endif //ADJUST_BUFF_ACCMULATION_CALCULATE
					{		
						// 중첩되었을 때 적용할 배율
						const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
						// 이전 효과의 적용값
						const float fOldValueOrPercent = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
						/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용
						m_fResultValue = ( abs(fNewValueOrPercent) > abs(fOldValueOrPercent) ? fNewValueOrPercent : fOldValueOrPercent );
						/// 중첩 적용
						m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );	/// 현재의 중첩횠수 - 1을 곱해줘야 하는 Before는 현재-1과 같으므로 그냥 곱한다.
					}
				}
				else	/// 교체 라면
					m_fResultValue = ( abs(fNewValueOrPercent) > abs(m_fResultValue) ? fNewValueOrPercent : m_fResultValue );
			}
			else	/// 중첩, 교체가 아니라면(첫시작 또는 중복)
			{
				m_fResultValue = fNewValueOrPercent;
			}
			return true;
		}
		else
			return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE MISS MATCH" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeNowHpPerSecondBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowHpPerSecondBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 초당 Hp를 증감 시키는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fResultValue = kFactor_[0];
	SelectDelegate();
}

/** @function : SelectDelegate
	@brief : 초당 변경되야하는 타입에 따라 실행해야하는 함수를 선택함
	@param : 행동요소의 포인터(pBehaviorFactor_)
*/
void CX2BuffChangeNowHpPerSecondBehaviorTemplet::SelectDelegate()
{
	switch ( m_eChangeType )
	{
	case BCT_FIX_VALUE:			/// 값에 의한 변경이면 해당 델리게이트 셋팅
		{
			m_delegateChangeHpByTypeFunc = DelegateChangeHpByType::from_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::ChangeHpByValue>( this );
			m_delegateGetValueOrPercentPerSecondFunc = DelegateGetValueOrPercentPerSecond::from_const_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetFixValuePerSecond>( this );
		} break;
	case BCT_RELATION_VALUE:	/// 공격력에 의한 변경이면 해당 델리게이트 셋팅
		{
			m_delegateChangeHpByTypeFunc = DelegateChangeHpByType::from_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::ChangeHpByValue>( this );
			m_delegateGetValueOrPercentPerSecondFunc = DelegateGetValueOrPercentPerSecond::from_const_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetValuePerSecond>( this );
		} break;

	case BCT_PERCENT:	/// Percent에 의한 변경이면 해당 델리게이트 셋팅
		{
			m_delegateChangeHpByTypeFunc = DelegateChangeHpByType::from_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::ChangeHpByPercent>( this );
			m_delegateGetValueOrPercentPerSecondFunc = DelegateGetValueOrPercentPerSecond::from_const_method<CX2BuffChangeNowHpPerSecondBehaviorTemplet, &CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetPercentPerSecondByPercent>( this );
		} break;

	default:
		DISPLAY_ERROR( L"ChangeType Invalid" );
		break;
	}
}

/** @function : ChangeHpByValue
	@brief : 값에 의한 HP 변경
	@param : HP가 변경될 유닛(pGameUnit_)
*/
void CX2BuffChangeNowHpPerSecondBehaviorTemplet::ChangeHpByValue( CX2GameUnit* pGameUnit_ )
{
	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
	{
#ifdef FIX_RECOVERY_HP
		if( pGameUnit_->GetNowHp() <= 0 )
			return;
#endif // FIX_RECOVERY_HP

		pGameUnit_->UpNowHp( m_fResultValue );
		if ( m_bDrawText )
			DrawText( pGameUnit_, m_fResultValue );
	}
}

/** @function : ChangeHpByPercent
	@brief : %에 의한 HP 변경
	@param : HP가 변경될 유닛(pGameUnit_)
*/
void CX2BuffChangeNowHpPerSecondBehaviorTemplet::ChangeHpByPercent( CX2GameUnit* pGameUnit_ )
{
	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
	{
#ifdef FIX_RECOVERY_HP
		if( pGameUnit_->GetNowHp() <= 0 )
			return;
#endif // FIX_RECOVERY_HP

		const float fDeltaValue = pGameUnit_->GetMaxHp() * m_fResultValue;
		pGameUnit_->UpNowHp( fDeltaValue );
		if ( m_bDrawText )
			DrawText( pGameUnit_, fDeltaValue );
	}
}

/** @function : GetValuePerSecond
	@brief : 값에 의해 적용될 초당 변경량 얻어옴
	@param : 저항률(fRegist_)
	@return : 변경량(float)
*/
float CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetFixValuePerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const
{	
	/// 수치 * 저항
	return (*pBehaviorFactor_)[FO_MULTIPLIER_OR_VALUE] * ( 1.0f - fRegist_ );
}

/** @function : GetValuePerSecond
	@brief : 값에 의해 적용될 초당 변경량 얻어옴
	@param : 저항률(fRegist_)
	@return : 변경량(float)
*/
float CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetValuePerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const
{	
	/// 수치 * 배율 * 저항
	return (*pBehaviorFactor_)[FO_ATTACK_VALUE] * (*pBehaviorFactor_)[FO_MULTIPLIER_OR_VALUE] * ( 1.0f - fRegist_ );	/// 공격력 * 배율
}

/** @function : GetPercentPerSecondByPercent
	@brief : 값에 의해 적용될 초당 변경 % 얻어옴
	@param : 저항률(fRegist_)
	@return : 변경률(float)
*/
float CX2BuffChangeNowHpPerSecondBehaviorTemplet::GetPercentPerSecondByPercent( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const
{
	/// 배율 * 저항
	return (*pBehaviorFactor_)[FO_PERCENT] * ( 1.0f - fRegist_ );
}

/** @function : DrawText
	@brief : HP의 변화량을 출력하는 함수
	@param : 게임유닛(pGameUnit_), 변화량(fDeltaValue_)
*/
void CX2BuffChangeNowHpPerSecondBehaviorTemplet::DrawText( CX2GameUnit* pGameUnit_, const float fDeltaValue_ )
{
	const UINT uiSize = 20;
	WCHAR wstrText[uiSize] = {0, };
	D3DXVECTOR3 numPos = pGameUnit_->GetHeadBonePos();
	numPos.y += -50.0f;

	if ( fDeltaValue_ > 0.0f )
	{
		StringCchPrintf( wstrText, uiSize, L"HP +%d", static_cast<int>( fDeltaValue_ ) );
		if( NULL != g_pData->GetPicCharRed() )
			g_pData->GetPicCharGreen()->DrawText( wstrText, numPos, pGameUnit_->GetDirVector(), CKTDGPicChar::AT_CENTER );
	}
	else
	{
		StringCchPrintf( wstrText, uiSize, L"HP %d", static_cast<int>( fDeltaValue_ ) );
		if( NULL != g_pData->GetPicCharRed() )
			g_pData->GetPicCharRed()->DrawText( wstrText, numPos, pGameUnit_->GetDirVector(), CKTDGPicChar::AT_CENTER );
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : 초당 MP 증감이 이루어지는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeNowMpPerSecondBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	float fTimeInterval = 0.0f;	/// 몇초간격으로 Mp가 변경되는가
	LUA_GET_VALUE_RETURN( luaManager_, "TIME_INTERVAL", fTimeInterval, 0.0f, return DISPLAY_ERROR( L"TIME_INTERVAL" ) );
	m_CheckElapsedTimeForInterval.SetTargetTime( fTimeInterval );

	LUA_GET_VALUE_RETURN( luaManager_, "DRAW_TEXT", m_bDrawText, false, return DISPLAY_ERROR( L"DRAW_TEXT" ) );	
	return true;
}

/** @function : OnFrameMove
	@brief : interval 타이머 Update, Mp 변경
	@param : Mp가 변경되야 하는 게임유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove( fElapsedTime_ );	/// Interval 타이머 진행
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove();	/// Interval 타이머 진행
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_delegateChangeMpByTypeFunc( pGameUnit_ );		/// Mp 변경
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e7c380: there is no ADJUST_BUFF_ACCMULATION_CALCULATE branch (as NowHp, part 1). Every change type
/// restacks with the multiplier formula and is never clamped at 0. Delegates are bound only for FIX and
/// PERCENT (the inlined SelectDelegate), and June raises no popups.
/*virtual*/ bool CX2BuffChangeNowMpPerSecondBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeNowMpPerSecondBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeNowMpPerSecondBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	if ( BCT_FIX_VALUE == m_eChangeType || BCT_PERCENT == m_eChangeType )
		SelectDelegate();

	const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;
	const float fNewValueOrPercent = GetValueOrPercentPerSecond( pBehaviorFactor, fRegist );

	if ( DidStart() )
	{
		const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
		if ( uiAccumulationCountBefore > 0 )
		{
			const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
			const float fOldValueOrPercent = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
			m_fResultValue = ( abs(fNewValueOrPercent) > abs(fOldValueOrPercent) ? fNewValueOrPercent : fOldValueOrPercent );
			m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );
		}
		else
			m_fResultValue = ( abs(fNewValueOrPercent) > abs(m_fResultValue) ? fNewValueOrPercent : m_fResultValue );
	}
	else
		m_fResultValue = fNewValueOrPercent;

	return true;
}

/** @function : OnAccumulationCountSet
	@brief : June 32e7c680 (behavior slot 3). Re-applies the value at a stack count set from outside
			 (CX2BuffTemplet::SetAccumulationCountNow): the change type must match, the FIX/PERCENT delegate
			 is bound again, the value is (1 - resist) * v[1], and once started with a stack count it is
			 scaled by 1 + multiplier * (count - 1). Unlike SetFactor there is no larger-absolute pick.
*/
/*virtual*/ bool CX2BuffChangeNowMpPerSecondBehaviorTemplet::OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeNowMpPerSecondBehaviorTemplet::OnAccumulationCountSet: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeNowMpPerSecondBehaviorTemplet::OnAccumulationCountSet: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	if ( BCT_FIX_VALUE == m_eChangeType || BCT_PERCENT == m_eChangeType )
		SelectDelegate();

	const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;
	float fValueOrPercent = GetValueOrPercentPerSecond( pBehaviorFactor, fRegist );
	if ( DidStart() && 0 < pBuffTemplet_->GetAccumulationCountNow() )
		fValueOrPercent = ( pBuffTemplet_->GetAccumulationMultiflier() * static_cast<float>( iCount_ - 1 ) + 1.0f ) * fValueOrPercent;

	m_fResultValue = fValueOrPercent;
	return true;
}

/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::AddValueByOperation( const float fValue_ )
{
	m_fResultValue += fValue_;		/// June 32e893a0 (slot 9)
}

/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::MultiplyValueByOperation( const float fValue_ )
{
	m_fResultValue *= fValue_;		/// June 32e89430 (slot 10)
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeNowMpPerSecondBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		const BUFF_CHANGE_TYPE eChangeType 
			= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] );

		if ( eChangeType == m_eChangeType )
		{
			/// 변경타입(CHANGE_TYPE)에 따라 수행 되야할 함수를 선택
			SelectDelegate();
			/// 저항을 무시하는 것도 아니고 영향을 받을 저항이 있는 경우 저항값을 얻어옴

// 			const float fRegist = ( BuffFactor_.GetIgnoreRegistForRate() ? 
// 				0.0f : pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE );

			const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;

			// 현재 효과의 적용값
			const float fNewValueOrPercent = GetValueOrPercentPerSecond( pBehaviorFactor, fRegist );

			if ( DidStart() )	/// 중첩 또는 교체라면
			{
				/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
				const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
				if ( uiAccumulationCountBefore > 0 )	/// 중첩 이라면
				{
#ifdef ADJUST_BUFF_ACCMULATION_CALCULATE
					if( eChangeType == BCT_PERCENT )
					{
						//바뀐 공식은 첫 효과가 1중첩이 된 것이라고 생각하고 작성됨.
						//따라서 uiAccumulationCountBefore에 1을 더해줌.
						m_fResultValue = 1 + ( ( fNewValueOrPercent - 1.0f ) * ( uiAccumulationCountBefore + 1 ) );

						if( m_fResultValue < 0.0f )
						{
							//m_fREsultValue값이 0보다 작지 않도록 값들을 계산해서 적어야 한다.(기획파트에서 정함)
							DISPLAY_ERROR( L"m_fResultValue error" );
							m_fResultValue = max( m_fResultValue, 0.0f );
						}
					}
					else
#endif //ADJUST_BUFF_ACCMULATION_CALCULATE
					{
						// 중첩되었을 때 적용할 배율
						const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
						// 이전 효과의 적용값
						const float fOldValueOrPercent = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
						/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용
						m_fResultValue = ( abs(fNewValueOrPercent) > abs(fOldValueOrPercent) ? fNewValueOrPercent : fOldValueOrPercent );
						/// 중첩 적용
						m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );	/// 현재의 중첩횠수 - 1을 곱해줘야 하는 Before는 현재-1과 같으므로 그냥 곱한다.
					}
				}
				else	/// 교체 라면
					m_fResultValue = ( abs(fNewValueOrPercent) > abs(m_fResultValue) ? fNewValueOrPercent : m_fResultValue );
			}
			else	/// 중첩, 교체가 아니라면(첫시작 또는 중복)
			{
				m_fResultValue = fNewValueOrPercent;
			}
			return true;
		}
		else
			return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE MISS MATCH" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeNowMpPerSecondBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeNowMpPerSecondBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowMpPerSecondBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 초당 MP를 증감 시키는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeNowMpPerSecondBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fResultValue = kFactor_[0];
	SelectDelegate();
}

/** @function : SelectDelegate
	@brief : 초당 변경되야하는 타입에 따라 실행해야하는 함수를 선택함
	@param : 행동요소의 포인터(pBehaviorFactor_)
*/
void CX2BuffChangeNowMpPerSecondBehaviorTemplet::SelectDelegate()
{
	switch ( m_eChangeType )
	{
	case BCT_FIX_VALUE:	/// 공격력에 의한 변경이면 해당 델리게이트 셋팅
		{
			m_delegateChangeMpByTypeFunc = DelegateChangeMpByType::from_method<CX2BuffChangeNowMpPerSecondBehaviorTemplet, &CX2BuffChangeNowMpPerSecondBehaviorTemplet::ChangeMpByValue>( this );
		} break;

	case BCT_PERCENT:	/// Percent에 의한 변경이면 해당 델리게이트 셋팅
		{
			m_delegateChangeMpByTypeFunc = DelegateChangeMpByType::from_method<CX2BuffChangeNowMpPerSecondBehaviorTemplet, &CX2BuffChangeNowMpPerSecondBehaviorTemplet::ChangeMpByPercent>( this );
		} break;

	default:
		DISPLAY_ERROR( L"ChangeType Invalid" );
		break;
	}
}

/** @function : ChangeMpByValue
	@brief : 값에 의한 Mp 변경
	@param : Mp가 변경될 유닛(pGameUnit_)
*/
void CX2BuffChangeNowMpPerSecondBehaviorTemplet::ChangeMpByValue( CX2GameUnit* pGameUnit_ )
{
	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
	{
		pGameUnit_->UpNowMp( m_fResultValue );
		if ( m_bDrawText )
			DrawText( pGameUnit_, m_fResultValue );
	}
}

/** @function : ChangeMpByPercent
	@brief : %에 의한 Mp 변경
	@param : Mp가 변경될 유닛(pGameUnit_)
*/
void CX2BuffChangeNowMpPerSecondBehaviorTemplet::ChangeMpByPercent( CX2GameUnit* pGameUnit_ )
{
	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
	{
		const float fDeltaValue = pGameUnit_->GetMaxMp() * m_fResultValue;
		pGameUnit_->UpNowMp( fDeltaValue );
		if ( m_bDrawText )
			DrawText( pGameUnit_, fDeltaValue );
	}
}

/** @function : GetValueOrPercentPerSecond
	@brief : 값에 의해 적용될 초당 변경 수치 or %를 얻어옴
	@param : 저항률(fRegist_)
	@return : 변경률(float)
*/
float CX2BuffChangeNowMpPerSecondBehaviorTemplet::GetValueOrPercentPerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const
{
	/// 배율 * 저항
	return (*pBehaviorFactor_)[FO_VALUE_OR_PERCENT] * ( 1.0f - fRegist_ );
}

/** @function : DrawText
	@brief : MP의 변화량을 출력하는 함수
	@param : 게임유닛(pGameUnit_), 변화량(fDeltaValue_)
*/
void CX2BuffChangeNowMpPerSecondBehaviorTemplet::DrawText( CX2GameUnit* pGameUnit_, const float fDeltaValue_ )
{
	const UINT uiSize = 20;
	WCHAR wstrText[uiSize] = {0, };
	D3DXVECTOR3 numPos = pGameUnit_->GetHeadBonePos();
	numPos.y += -50.0f;

	if ( fDeltaValue_ > 0.0f )
	{
		StringCchPrintf( wstrText, uiSize, L"MP +%d", static_cast<int>( fDeltaValue_ ) );
		if( NULL != g_pData->GetPicCharBlue() )
			g_pData->GetPicCharBlue()->DrawText( wstrText, numPos, pGameUnit_->GetDirVector(), CKTDGPicChar::AT_CENTER );
	}
	else
	{
		StringCchPrintf( wstrText, uiSize, L"MP %d", static_cast<int>( fDeltaValue_ ) );
		if( NULL != g_pData->GetPicCharRed() )
			g_pData->GetPicCharRed()->DrawText( wstrText, numPos, pGameUnit_->GetDirVector(), CKTDGPicChar::AT_CENTER );
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffReverseLeftRightBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	float fTimeInterval = 0.0f;	/// 몇초 간격으로 좌우가 반전 되는가?
	LUA_GET_VALUE_RETURN( luaManager_, "TIME_INTERVAL", fTimeInterval, 0.0f, return DISPLAY_ERROR( L"TIME_INTERVAL" ) );

	m_CheckElapsedTimeForInterval.SetTargetTime( fTimeInterval );
	return true;
}

/** @function : OnFrameMove
	@brief : interval 타이머 Update 및 좌우반전 갱신
	@param : 좌우반전이 갱신되야 하는 게임유닛(pGameUnit_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffReverseLeftRightBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffReverseLeftRightBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    m_CheckElapsedTimeForInterval.OnFrameMove( fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
		pGameUnit_->ToggleReverseLeftRightByBuff( pBuffTemplet_->GetBuffIdentity() );
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffReverseLeftRightBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
		if ( !DidStart() )	/// 중첩 적용 안함
		{
			// 교체 적용 안함(교체도 효과가 달라지는게 아니기 때문에 적용할 필요 없음)
			pGameUnit_->CreateAndInsertReverseLeftRightByBuff( pBuffTemplet_->GetBuffIdentity() ); /// 좌우반전 인스턴스 생성 및 적용
		}	
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffReverseLeftRightBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffReverseLeftRightBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffReverseLeftRightBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffReverseLeftRightBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 좌우반전을 적용시키는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffReverseLeftRightBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	pGameUnit_->CreateAndInsertReverseLeftRightByBuff( kBuffFactor_.m_BuffIdentity );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffReverseLeftRightBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseReverseLeftRightByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffIdentityBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
		if ( !DidStart() )	/// 중첩 적용 안함
		{
			// 교체 적용 안함(교체도 효과가 달라지는게 아니기 때문에 적용할 필요 없음)
			pGameUnit_->CreateAndInsertBuffIdentity( GetType(), pBuffTemplet_->GetBuffIdentity() ); /// 공격불가 인스턴스 생성 및 적용
		}
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffAttackImpossibleBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffIdentityBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffIdentityBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffIdentityBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 공격(콤보, 스킬)을 못하게 하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffIdentityBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	pGameUnit_->CreateAndInsertBuffIdentity( GetType(), kBuffFactor_.m_BuffIdentity );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffIdentityBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseBuffIdentity( GetType(), pBuffTemplet_->GetBuffIdentity() );		
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief :스탯(이동속도, 크리티컬확률등등 포함)을 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeStatBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_RELATION_TYPE", m_eRelationType, 
		BUFF_RELATION_TYPE, BRT_END, return DISPLAY_ERROR( L"BUFF_RELATION_TYPE" ) );

	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffChangeStatBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
			if ( BCT_RELATION_VALUE != m_eChangeType || STATIC_CAST_FLOAT_TO_ENUM( BUFF_RELATION_TYPE, (*pBehaviorFactor)[FO_RELATION_TYPE] ) == m_eRelationType )
			{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
				/// June 32e78410 @32e784b8: vslot 15 with the applier's UID (factor +0x30/+0x34), after both type checks
				SetBuffOwnerUID( BuffFactor_.GetUidGameUnit() );
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
				/// 저항을 무시하는 것도 아니고 영향을 받을 저항이 있는 경우 저항값을 얻어옴

// 				const float fRegist = ( BuffFactor_.GetIgnoreRegistForRate() ? 
// 					0.0f : pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE );

				const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;

				const float fNewValue = GetValue( pBehaviorFactor, pGameUnit_, fRegist );
				if ( DidStart() )	/// 중첩 또는 교체라면
				{
					/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
					const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
					if ( uiAccumulationCountBefore > 0 )	/// 중첩 이라면
					{
						// 중첩되었을 때 적용할 배율
						const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();

						if ( IsSamef( fAccumulationMultiflier ) )
							return DISPLAY_ERROR( L"AccumulationMultiflier must not be 0" );

						switch ( m_eChangeType )
						{
						case BCT_FIX_VALUE:
						case BCT_RELATION_VALUE:
							{
								// 이전 효과의 적용값	/// 첫 중첩이면(uiAccumulationCountBefore <= 1) 기존의 적용값을 old 값으로 그대로 사용
								const float fOldValue =
									(uiAccumulationCountBefore <= 1) ? m_fResultValue : ( m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) ) );

								/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용 (마이너스의 경우에도 효과가 큰것)
								m_fResultValue = ( abs(fNewValue) > abs(fOldValue) ? fNewValue : fOldValue );
								/// 중첩 적용
								/// 현재의 중첩횟수 - 1 한 값을 을 곱해줘야 하는 uiAccumulationCountBefore가 이미 현재중첩횟수-1과 같으므로 그냥 곱한다.
								m_fResultValue += m_fResultValue * fAccumulationMultiflier * uiAccumulationCountBefore;	
							} break;

						case BCT_PERCENT:
							{
#ifdef ADJUST_BUFF_ACCMULATION_CALCULATE
								//바뀐 공식은 첫 효과가 1중첩이 된 것이라고 생각하고 작성됨.
								//따라서 uiAccumulationCountBefore에 1을 더해줌.
								m_fResultValue = 1 + ( ( fNewValue - 1.0f ) * ( uiAccumulationCountBefore + 1 ) );

								if( m_fResultValue < 0.0f )
								{
									//m_fREsultValue값이 0보다 작지 않도록 값들을 계산해서 적어야 한다.(기획파트에서 정함)
									DISPLAY_ERROR( L"m_fResultValue error" );
									m_fResultValue = max( m_fResultValue, 0.0f );
								}
#else //ADJUST_BUFF_ACCMULATION_CALCULATE
								// 이전 효과의 적용값	/// 첫 중첩이면(uiAccumulationCountBefore <= 1) 기존의 적용값을 old 값으로 그대로 사용
								const float fOldValue =
									(uiAccumulationCountBefore <= 1) ? m_fResultValue : ( m_fResultValue / pow( fAccumulationMultiflier, static_cast<float>(uiAccumulationCountBefore - 1) ) );

								/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용 (1.0 보다 작은 경우 0.8 배 보다 0.7 배가 더 많은 효과를 주는 것이므로 0.7을 선택)
								m_fResultValue = ( abs(fNewValue - 1.0f) > abs(fOldValue - 1.0f) ? fNewValue: fOldValue );
								/// 중첩 적용
								/// 현재의 중첩횟수 - 1 한 값을 을 곱해줘야 하는 uiAccumulationCountBefore가 이미 현재중첩횟수-1과 같으므로 그냥 곱한다.
								m_fResultValue *= pow( fAccumulationMultiflier, static_cast<float>( uiAccumulationCountBefore ) );
								//m_fResultValue += 1.0f;
#endif //ADJUST_BUFF_ACCMULATION_CALCULATE
							} break;

						default:	/// 치환인 경우 행동요소는 중복 수행 안함
							break;
						}
					}
					else	/// 교체 라면
						m_fResultValue = ( abs(fNewValue) > abs(m_fResultValue) ? fNewValue : m_fResultValue );

					pGameUnit_->UpdateAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
				}
				else	/// 중첩, 교체가 아니라면(첫시작 또는 중복)
				{
					m_fResultValue = fNewValue;
					pGameUnit_->InsertAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
				}
				return true;
			}
			else
				return DISPLAY_ERROR( L"MISS MATCH BUFF_RELATION_TYPE" );
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeStatBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStatBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeStatBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeStatBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fResultValue = kFactor_[0];
	pGameUnit_->InsertAdditionalStat( GetType(), m_eChangeType, kBuffFactor_.m_BuffIdentity, m_fResultValue );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeStatBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity() );		
}

//{{ Iruha : 2026-10-03 // June buff port Phase 4a (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
/** @function : OnAccumulationCountDecreased
	@brief : June 32e73500 (ChangeStat vtable slot 7). Rescales the applied stat from the stack count
		before the decrease to the count now: FIX/RELATION values by now/before, PERCENT values on their
		distance from 1.0. "Before" is now + ACCUMULATION_DECREASE_COUNT_WHEN_FINISH (now + 1 without the flag).
	@param : unit the buff is on (pGameUnit_), its templet (pBuffTemplet_)
	@return : false without a unit or templet, else true
*/
/*virtual*/ bool CX2BuffChangeStatBehaviorTemplet::OnAccumulationCountDecreased( CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	if ( NULL == pGameUnit_ || NULL == pBuffTemplet_ )
		return false;

	const float fCountNow = static_cast<float>( pBuffTemplet_->GetAccumulationCountNow() );
	const float fCountBefore = fCountNow + ( pBuffTemplet_->IsAccumulationDecreaseWhenFinish() ?
		static_cast<float>( pBuffTemplet_->GetAccumulationDecreaseCountWhenFinish() ) : 1.0f );

	switch ( m_eChangeType )
	{
	case BCT_FIX_VALUE:
	case BCT_RELATION_VALUE:
		m_fResultValue = m_fResultValue / fCountBefore * fCountNow;
		break;

	case BCT_PERCENT:
		m_fResultValue = ( m_fResultValue - 1.0f ) / fCountBefore * fCountNow + 1.0f;
		break;

	default:
		break;
	}

	pGameUnit_->UpdateAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
	return true;
}
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : OnAccumulationCountSet
	@brief : June 32e786d0 (behavior slot 3). The change type must match; v = GetValue at the resist.
			 Not started: count * v, inserted. Started: with no stack count, count * v; with one, a zero
			 multiplier returns false, FIX/RELATION take count * v, PERCENT 1 + (v - 1) * count clamped at 0,
			 SWAP keeps its value; then the stat is updated (3226fd70, so BBT 6 gets no HP rescale here).
*/
/*virtual*/ bool CX2BuffChangeStatBehaviorTemplet::OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet::OnAccumulationCountSet: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet::OnAccumulationCountSet: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	const float fRegist = pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE;
	const float fNewValue = GetValue( pBehaviorFactor, pGameUnit_, fRegist );

	if ( !DidStart() )
	{
		m_fResultValue = static_cast<float>( iCount_ ) * fNewValue;
		pGameUnit_->InsertAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
		return true;
	}

	if ( 0 == pBuffTemplet_->GetAccumulationCountNow() )
		m_fResultValue = static_cast<float>( iCount_ ) * fNewValue;
	else
	{
		if ( IsSamef( pBuffTemplet_->GetAccumulationMultiflier() ) )
			return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet::OnAccumulationCountSet: ACCUMULATION_MULTIPLIER is 0" )	/// 4b-ii: June returns false here silently; logged once

		switch ( m_eChangeType )
		{
		case BCT_FIX_VALUE:
		case BCT_RELATION_VALUE:
			m_fResultValue = static_cast<float>( iCount_ ) * fNewValue;
			break;

		case BCT_PERCENT:
			m_fResultValue = ( fNewValue - 1.0f ) * static_cast<float>( iCount_ ) + 1.0f;
			if ( m_fResultValue < 0.0f )
				m_fResultValue = 0.0f;
			break;

		default:		/// SWAP keeps its value
			break;
		}
	}

	pGameUnit_->UpdateAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
	return true;
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
//}}

/** @function : GetValue
	@brief : ChangeType 에 따라 각기 다른 공식으로 변경값을 얻어내는 함수
	@param : 행동요소(pBehaviorFactor_), 버프가 적용된 게임유닛(pGameUnit_), 저항(fRegist_)
	@return : 변경값(float)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e764f0: as March, except that the option-data level is clamped at 90 (cmp eax,0x5a @32e7655d). The
/// option types are June's byte table 32e7664c (11-16, 20, 21, 72), transcribed literally.
float CX2BuffChangeStatBehaviorTemplet::GetValue( const KBuffBehaviorFactor* pBehaviorFactor_, CX2GameUnit* pGameUnit_, const float fRegist_ ) const
{
	const float fValue = (*pBehaviorFactor_)[FO_VALUE];

	switch ( m_eChangeType )
	{
	case BCT_SWAP_VALUE:
		return fValue;

	case BCT_FIX_VALUE:
		switch ( GetType() )
		{
		case BBT_CHANGE_ANIM_SPEED:
		case BBT_CHANGE_WALK_RUN_SPEED:
		case BBT_CHANGE_JUMP_POWER:
		case BBT_CHANGE_ACCURACY:
		case BBT_CHANGE_AVOIDANCE:
		case BBT_CHANGE_CRITICAL_RATE:
		case BBT_CHANGE_ADDITIONAL_ATTACK:
		case BBT_CHANGE_ADDITIONAL_DEFENCE:
		case BBT_CHANGE_HYPER_CHARGE_SPEED:
			{
				int iLevel = static_cast<int>( pGameUnit_->GetUnitLevel() );
				if ( iLevel > 90 )
					iLevel = 90;
				const UINT uiLevel = static_cast<UINT>( iLevel );
				return fValue * ( 1.5f * (uiLevel - 1) + 9.5f ) * (1.0f - fRegist_);
			}

		default:
			return fValue * (1.0f - fRegist_);
		}

	case BCT_RELATION_VALUE:
		return fValue * (*pBehaviorFactor_)[FO_MULTIPLIER] * (1.0f - fRegist_);

	case BCT_PERCENT:
		if ( fValue >= 1.0f )
			return fValue;
		else if ( fValue > 0.0f )
			return 1.0f - ( (1.0f - fValue) * (1.0f - fRegist_) );
		else
			return fValue;

	default:
		return 0.0f;
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
float CX2BuffChangeStatBehaviorTemplet::GetValue( const KBuffBehaviorFactor* pBehaviorFactor_, CX2GameUnit* pGameUnit_, const float fRegist_ ) const
{
	const float fValue = (*pBehaviorFactor_)[FO_VALUE];

	switch ( m_eChangeType )
	{
	case BCT_SWAP_VALUE:
		{
			return fValue;
		} break;

	case BCT_FIX_VALUE:
		{
			if ( MustUseOptionData() )
			{
				const UINT uiLevel = pGameUnit_->GetUnitLevel();
				return fValue * ( 1.5f * (uiLevel - 1) + 9.5f ) * (1.0f - fRegist_);	/// 옵션 수치화 스크립트에 적용된 연산
			}
			else
			{
				return fValue * (1.0f - fRegist_);
			}
		} break;

	case BCT_RELATION_VALUE:
		{
			return fValue * (*pBehaviorFactor_)[FO_MULTIPLIER] * (1.0f - fRegist_);
		} break;

	case BCT_PERCENT:
		{
			/// 디버프라고 생각되는 값의 경우 (fValue <1.0f && fValue >0.0f)
			/// fRegist_ 적용			
			if ( fValue >= 1.0f )
				return fValue;
			else if ( fValue > 0.0f )
				return 1.0f - ( (1.0f - fValue) * (1.0f - fRegist_) );
			else
				return fValue;
		} break;

	default:
		return 0.0f;
		break;
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : MustUseOptionData
	@brief : 옵션 수치화를 사용해야 하는 행동인지를 얻어옴
	@return : 옵션 수치를 사용해야 하면 true, 그렇지 않으면 false
*/
bool CX2BuffChangeStatBehaviorTemplet::MustUseOptionData() const
{
	switch ( GetType() )
	{
	case BBT_CHANGE_ANIM_SPEED:
	case BBT_CHANGE_WALK_RUN_SPEED:
	case BBT_CHANGE_JUMP_POWER:
	case BBT_CHANGE_ACCURACY:
	case BBT_CHANGE_AVOIDANCE:
	case BBT_CHANGE_CRITICAL_RATE:
	case BBT_CHANGE_ADDITIONAL_ATTACK:
	case BBT_CHANGE_ADDITIONAL_DEFENCE:
#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
	case BBT_CHANGE_HYPER_CHARGE_SPEED:
#endif //SERV_ADD_LUNATIC_PSYKER
		return true;
		break;
		
	default:
		return false;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffCreateBuffFactortBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	/// June 32e860a0. NPCs get the created factor too (March inserted for users only), at the templet's stack
	/// count; a user with SKILL_ID takes that skill's level instead. APPLY_TO_SPECIAL_ATTACK is kept and sent;
	/// June hands it to the unit's list entry, which CX2DamageManager reads (326309f0).
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffCreateBuffFactortBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	m_uiBuffFactorId = static_cast<UINT>( (*pBehaviorFactor)[FO_BUFF_FACTOR_ID] );
	if ( 0 == m_uiBuffFactorId )
		return DISPLAY_ERROR( L"CX2BuffCreateBuffFactortBehaviorTemplet::SetFactor: BUFF_FACTOR_ID is 0" )	/// 4b-ii: June returns false here silently; logged once

	if ( DidStart() )
		pGameUnit_->EraseTemporaryBuffFactor( pBuffTemplet_->GetBuffIdentity() );

	m_uiSkillId = static_cast<UINT>( (*pBehaviorFactor)[FO_SKILL_ID] );
	CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( m_uiBuffFactorId );

	UINT uiLevel = pBuffTemplet_->GetAccumulationCountNow();
	if ( 0 == uiLevel )
		uiLevel = 1;

	if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );
		if ( NULL == pUser->GetUnit() )
			return DISPLAY_ERROR( L"CX2BuffCreateBuffFactortBehaviorTemplet::SetFactor: the user has no CX2Unit" )	/// 4b-ii: June returns false here silently; logged once

		if ( 0 != m_uiSkillId )
		{
			const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SKILL_ID( m_uiSkillId ) );
			uiLevel = ( iSkillTempletLevel < 1 ? 1 : static_cast<UINT>( iSkillTempletLevel ) );
		}
	}

	m_bApplyToSpecialAttack = ( 2 < pBehaviorFactor->m_vecValues.size() && 0.0f < (*pBehaviorFactor)[2] );

	if ( NULL != ptrBuffFactor )
		pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, pBuffTemplet_->GetBuffIdentity(), uiLevel, m_bApplyToSpecialAttack );
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffCreateBuffFactortBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		m_uiBuffFactorId = static_cast<UINT>( (*pBehaviorFactor)[FO_BUFF_FACTOR_ID] );
		if ( 0 < m_uiBuffFactorId )
		{
			if ( DidStart() )	/// 기존에 작동 중인 것이었으면 기존의 효과 삭제
				pGameUnit_->EraseTemporaryBuffFactor( pBuffTemplet_->GetBuffIdentity() );

		#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
			m_uiSkillId = static_cast<UINT>( (*pBehaviorFactor)[FO_SKILL_ID] );
			if ( 0 > m_uiSkillId )
				return DISPLAY_ERROR( L"SKILL_ID doesn't exist" );
		#endif //UPGRADE_SKILL_SYSTEM_2013

			CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( m_uiBuffFactorId );
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
			if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
			{
				CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );
				
				if ( NULL == pUser->GetUnit() )
					return false;

				const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SKILL_ID( m_uiSkillId ) ) );	/// 스킬 레벨
				pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, pBuffTemplet_->GetBuffIdentity(), iSkillTempletLevel );

				return true;
			}
#else //UPGRADE_SKILL_SYSTEM_2013
			pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, pBuffTemplet_->GetBuffIdentity() );
#endif //UPGRADE_SKILL_SYSTEM_2013
			return true;
		}
		else
			return DISPLAY_ERROR( L"BUFF_FACTOR_ID doesn't exist" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffCreateBuffFactortBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffCreateBuffFactortBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffCreateBuffFactortBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffCreateBuffFactortBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	/// June 32e7d930: [BFI, SKILL_ID, 1.0 / -1.0]; the two new values are appended after March's one
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_uiBuffFactorId ) );
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_uiSkillId ) );
	BehaviorFactor.m_vecValues.push_back( m_bApplyToSpecialAttack ? 1.0f : -1.0f );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffCreateBuffFactortBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_uiBuffFactorId ) );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 다른 유닛에게 지정된 버프 또는 디버프를 적용할 수 있는 행동 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffCreateBuffFactortBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	/// June 32e862c0: SKILL_ID and the special-attack flag come from the packet (a March sender omits them: 0 / false),
	/// the level is the packet's stack count or 1, and a user's SKILL_ID level replaces it
	m_uiBuffFactorId = static_cast<UINT>( kFactor_[0] );
	m_uiSkillId = static_cast<UINT>( kFactor_[1] );
	m_bApplyToSpecialAttack = ( 2 < kFactor_.m_vecValues.size() && 0.0f < kFactor_[2] );

	CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( m_uiBuffFactorId );

	UINT uiLevel = ( 0 == kBuffFactor_.m_ucAccumulationCountNow ? 1 : kBuffFactor_.m_ucAccumulationCountNow );
	if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );
		if ( NULL == pUser->GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SKILL_ID( m_uiSkillId ) );
		uiLevel = ( iSkillTempletLevel < 1 ? 1 : static_cast<UINT>( iSkillTempletLevel ) );
	}

	if ( NULL != ptrBuffFactor )
		pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, kBuffFactor_.m_BuffIdentity, uiLevel, m_bApplyToSpecialAttack );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffCreateBuffFactortBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_uiBuffFactorId = static_cast<UINT>( kFactor_[0] );
	CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( m_uiBuffFactorId );
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );
				
		if ( NULL == pUser->GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SKILL_ID( m_uiSkillId ) ) );	/// 스킬 레벨

		pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, kBuffFactor_.m_BuffIdentity, iSkillTempletLevel );
	}
#else //UPGRADE_SKILL_SYSTEM_2013
	pGameUnit_->CreateAndInsertTemporaryBuffFactor( *ptrBuffFactor, kBuffFactor_.m_BuffIdentity );
#endif //UPGRADE_SKILL_SYSTEM_2013
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffCreateBuffFactortBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseTemporaryBuffFactor( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	/// June 32e768a0: the value is index 1, where June's level slice (32d47620) puts it after a 1.0
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( DidStart() )
	{
		if ( pBuffTemplet_->GetAccumulationCountNow() > 1 )
			m_fAbsorbValue += (*pBehaviorFactor)[1];
		else
			m_fAbsorbValue = (*pBehaviorFactor)[1];

		pGameUnit_->UpdateAbsorbEffectAttack( pBuffTemplet_->GetBuffIdentity(), m_fAbsorbValue );
	}
	else
	{
		m_fAbsorbValue = (*pBehaviorFactor)[1];
		pGameUnit_->CreateAndInsertAbsorbEffectAttack( pBuffTemplet_->GetBuffIdentity(), m_fAbsorbValue );
	}
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( DidStart() )	/// 중첩 또는 교체
		{
			const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
			
			if ( uiAccumulationCountBefore > 1 )	/// 중첩 이라면
				m_fAbsorbValue += (*pBehaviorFactor)[FO_ABSORB_VALUE];
			else
				m_fAbsorbValue = (*pBehaviorFactor)[FO_ABSORB_VALUE];

			pGameUnit_->UpdateAbsorbEffectAttack( pBuffTemplet_->GetBuffIdentity(), m_fAbsorbValue );
		}
		else	/// 첫효과 또는 중복
		{
			m_fAbsorbValue = (*pBehaviorFactor)[FO_ABSORB_VALUE];
			pGameUnit_->CreateAndInsertAbsorbEffectAttack( pBuffTemplet_->GetBuffIdentity(), m_fAbsorbValue );
		}
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffAbsorbEffectAttackBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffAbsorbEffectAttackBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffAbsorbEffectAttackBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffAbsorbEffectAttackBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
	
	BehaviorFactor.m_vecValues.push_back( m_fAbsorbValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 이펙트를 흡수하여 MP로 전환하는 행동 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	/// June 32e76a00 reads index 1 too, though GetFactor (32e7da30, unchanged) sends one value: a peer's copy gets 0.
	/// That is June's own behaviour, transcribed as is (flagged for the Phase 14 play-test).
	m_fAbsorbValue = kFactor_[1];
	pGameUnit_->CreateAndInsertAbsorbEffectAttack( kBuffFactor_.m_BuffIdentity, m_fAbsorbValue );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fAbsorbValue = kFactor_[0];
	pGameUnit_->CreateAndInsertAbsorbEffectAttack( kBuffFactor_.m_BuffIdentity, m_fAbsorbValue );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffAbsorbEffectAttackBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseAbsorbEffectAttack( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

// /** @function : SetFactor
// 	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
// 	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
// */
// /*virtual*/ bool CX2BuffChangeMpHitBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
// {
// 	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
// 
// 	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
// 	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
// 	{
// 		/// 저항을 무시하는 것도 아니고 영향을 받을 저항이 있는 경우 저항값을 얻어옴
// 		const float fRegist = ( BuffFactor_.GetIgnoreRegistForRate() ? 
// 			0.0f : pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE );
// 		// 현재 효과의 적용값
// 		const float fNewMultiplier = (*pBehaviorFactor)[FO_MULTIPLIER] * (1.0f - fRegist);
// 
// 		if ( DidStart() )	/// 중첩 또는 교체라면
// 		{
// 			/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
// 			const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
// 			if ( uiAccumulationCountBefore > 0 )	/// 중첩 이라면
// 			{
// 				// 중첩되었을 때 적용할 배율
// 				const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
// 				// 이전 효과의 적용값
// 				const float fOldMultiplier = ( m_fMultiplier - 1.0f ) / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
// 
// 				/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용
// 				m_fMultiplier 
// 					= ( abs(fNewMultiplier - 1.0f) > abs(fOldMultiplier) ? fNewMultiplier - 1.0f : fOldMultiplier );
// 				/// 중첩 적용
// 				m_fMultiplier *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );	/// 현재의 중첩횠수 - 1을 곱해줘야 하는 Before는 현재-1과 같으므로 그냥 곱한다.
// 				m_fMultiplier += 1.0f;
// 			}
// 			else	/// 교체 라면
// 				m_fMultiplier = ( abs(fNewMultiplier) > abs(m_fMultiplier) ? fNewMultiplier : m_fMultiplier );
// 			
// 			pGameUnit_->UpdateAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity(), m_fMultiplier );
// 		}
// 		else	/// 첫효과 또는 중복
// 		{
// 			m_fMultiplier = fNewMultiplier;
// 			pGameUnit_->InsertAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity(), m_fMultiplier );
// 		}
// 		return true;
// 	}
// 	else
// 		return DISPLAY_ERROR( L"CX2BuffChangeMpHitBehaviorTemplet GetBehaviorFactor didn't work" );
// }
// 
// /** @function : GetClonePtr
// 	@return : CX2BuffBehaviorTempletPtr
// */
// /*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeMpHitBehaviorTemplet::GetClonePtr() const
// {
// 	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeMpHitBehaviorTemplet( *this ) );
// }
// 
// /** @function : GetFactor
// 	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
// 	@param : 버프요소 인스턴스(factor_)
// */
// /*virtual*/ void CX2BuffChangeMpHitBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
// {
// 	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
// 	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
// 
// 	BehaviorFactor.m_vecValues.push_back( m_fMultiplier );
// }
// 
// /** @function : SetFactorFromPacket
// 	@brief : 패킷으로 부터 받은 버프 정보로 타격시 MP 회수량을 증가하는 행동 템플릿을 셋팅
// 	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
// */
// /*virtual*/ void CX2BuffChangeMpHitBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
// {
// 	m_fMultiplier = kFactor_[0];
// 	pGameUnit_->InsertAdditionalStat( GetType(), BCT_SWAP_VALUE, kBuffFactor_.m_BuffIdentity, m_fMultiplier );
// }
// 
// /** @function : DoFinish
// 	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
// 	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
// */
// /*virtual*/ void CX2BuffChangeMpHitBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
// {
// 	pGameUnit_->EraseAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity() );
// }
// 
// //////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// /** @function : SetFactor
// 	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
// 	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
// */
// /*virtual*/ bool CX2BuffChangeMpHittedBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
// {
// 	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
// 
// 	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
// 	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
// 	{
// 		/// 저항을 무시하는 것도 아니고 영향을 받을 저항이 있는 경우 저항값을 얻어옴
// 		const float fRegist = ( BuffFactor_.GetIgnoreRegistForRate() ? 
// 			0.0f : pGameUnit_->GetEnchantResist( static_cast<CX2EnchantItem::ENCHANT_TYPE>( pBuffTemplet_->GetRegistType() ) ) / CX2EnchantItem::EAR_MAX_VALUE );
// 		// 현재 효과의 적용값
// 		const float fNewMultiplier = (*pBehaviorFactor)[FO_MULTIPLIER] * (1.0f - fRegist);
// 
// 		if ( DidStart() )	/// 중첩 또는 교체라면
// 		{
// 			/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
// 			const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
// 			if ( uiAccumulationCountBefore > 0 )	/// 중첩 이라면
// 			{
// 				// 중첩되었을 때 적용할 배율
// 				const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
// 				// 이전 효과의 적용값
// 				const float fOldMultiplier = ( m_fMultiplier - 1.0f ) / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
// 
// 				/// 이전 효과와 현재 효과 중 절대값 수치가 더 높을 것을 적용
// 				m_fMultiplier 
// 					= ( abs(fNewMultiplier - 1.0f) > abs(fOldMultiplier) ? fNewMultiplier - 1.0f : fOldMultiplier );
// 				/// 중첩 적용
// 				m_fMultiplier *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );	/// 현재의 중첩횠수 - 1을 곱해줘야 하는 Before는 현재-1과 같으므로 그냥 곱한다.
// 				m_fMultiplier += 1.0f;
// 			}
// 			else	/// 교체 라면
// 				m_fMultiplier = ( abs(fNewMultiplier) > abs(m_fMultiplier) ? fNewMultiplier : m_fMultiplier );
// 
// 			pGameUnit_->UpdateAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity(), m_fMultiplier );
// 		}
// 		else	/// 첫효과 또는 중복
// 		{
// 			m_fMultiplier = fNewMultiplier;
// 			pGameUnit_->InsertAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity(), m_fMultiplier );
// 		}
// 		return true;
// 	}
// 	else
// 		return DISPLAY_ERROR( L"CX2BuffChangeMpHittedBehaviorTemplet GetBehaviorFactor didn't work" );
// }
// 
// /** @function : GetClonePtr
// 	@return : CX2BuffBehaviorTempletPtr
// */
// /*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeMpHittedBehaviorTemplet::GetClonePtr() const
// {
// 	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeMpHittedBehaviorTemplet( *this ) );
// }
// 
// /** @function : GetFactor
// 	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
// 	@param : 버프요소 인스턴스(factor_)
// */
// /*virtual*/ void CX2BuffChangeMpHittedBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
// {
// 	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
// 	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
// 
// 	BehaviorFactor.m_vecValues.push_back( m_fMultiplier );
// }
// 
// /** @function : SetFactorFromPacket
// 	@brief : 패킷으로 부터 받은 버프 정보로 피격시 회수되는 MP량을 증감시키는 행동 템플릿을 셋팅
// 	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
// */
// /*virtual*/ void CX2BuffChangeMpHittedBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
// {
// 	m_fMultiplier = kFactor_[0];
// 	pGameUnit_->InsertAdditionalStat( GetType(), BCT_SWAP_VALUE, kBuffFactor_.m_BuffIdentity, m_fMultiplier );
// }
// 
// /** @function : DoFinish
// 	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
// 	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
// */
// /*virtual*/ void CX2BuffChangeMpHittedBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
// {
// 	pGameUnit_->EraseAdditionalStat( GetType(), BCT_SWAP_VALUE, pBuffTemplet_->GetBuffIdentity() );		
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : OnFrameMove
	@brief : 보여주기용 각성 시간을 갱신
	@param : 각성중인 유닛(pGameUnit_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	/// June 32e7dae0: the gage's remaining time (if positive) is taken back first, so a gage-side change wins
	if ( 0.0f < m_fRemainTime )
	{
		if ( 0.0f < pGameUnit_->GetRemainHyperModeTime() )
			m_fRemainTime = pGameUnit_->GetRemainHyperModeTime();

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		m_fRemainTime -= fElapsedTime_;
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		m_fRemainTime -= g_pKTDXApp->GetElapsedTime();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		if ( m_fRemainTime <= 0.0f )
			m_fRemainTime = 0.0f;

		pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
		pGameUnit_->HyperModeBuffEffectOnFrameMove();
		/// At 0 June calls 32296cb0, which stops two effect-set handles (unit +0x9f4/+0x9f8) that only
		/// June code creates. March never creates them, so there is nothing to stop.
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( 0.0f < m_fRemainTime )
	{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
        m_fRemainTime -= fElapsedTime_;
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		m_fRemainTime -= g_pKTDXApp->GetElapsedTime();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
		m_fRemainTime = max( m_fRemainTime, 0.0f );
		pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
// robobeg : frame move 호출 빈도에 따라 이펙트가 달라질 수도 있다. --;
		pGameUnit_->HyperModeBuffEffectOnFrameMove();
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeHyperModeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	/// June 32e7dbb0. Unit slots: 40 SetFullHyperMode, 41 IsFullHyperMode, 49 HyperModeBuffEffectStart.
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeHyperModeBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( 0 == m_uiHyperModeCount )
	{
		m_uiHyperModeCount = static_cast<UINT>( (*pBehaviorFactor)[FO_HYPER_MODE_COUNT] );
	}

	/// June checks the orbs on every SetFactor, not only the first (the count != 0 branch jumps to the
	/// CMP [ESI+0x10],2 at 32e7dc30); slot 41 is asked only when the count is 2 or less
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_BERSERK_BY_CHANCE
	/// June @32e7de0a: unit vslot 0xa4 ( berserk by chance, false but for Chung ), not IsFullHyperMode
	if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount || pGameUnit_->IsBerserkByChance() )
#else
	if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount || pGameUnit_->IsFullHyperMode() )
#endif SERV_IRUHADEV_JUNE_DIVERGE_BERSERK_BY_CHANCE
		pGameUnit_->SetFullHyperMode( true );

	/// June then calls 32b4dd50( count, or 1 ) (@32e7dc69 / @32e7de36), before slot 49: the buff-driven hyper count
	/// (June +0x6844). Not March's SetHyperModeCount, which is the gage's orb count.
	pGameUnit_->SetHyperModeCountByBuff( static_cast<int>( 1 > m_uiHyperModeCount ? 1 : m_uiHyperModeCount ) );

	m_fRemainTime = m_fRemainTime + (*pBehaviorFactor)[FO_HYPER_MODE_TIME];
	pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
	pGameUnit_->HyperModeBuffEffectStart();
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeHyperModeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( 0 == m_uiHyperModeCount )	/// 재각성 등이 아닌 경우에만 갱신(3각성 중 1카운트로 재각성 했을 때  3각성이 1각성으로 변경되지 않도록!)
			m_uiHyperModeCount = max( m_uiHyperModeCount, static_cast<UINT>( (*pBehaviorFactor)[FO_HYPER_MODE_COUNT] ) );

		if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount )
			pGameUnit_->SetFullHyperMode( true );

		m_fRemainTime = m_fRemainTime + (*pBehaviorFactor)[FO_HYPER_MODE_TIME];
		pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
		pGameUnit_->HyperModeBuffEffectStart();
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeHyperModeBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeHyperModeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeHyperModeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_uiHyperModeCount ) );
	BehaviorFactor.m_vecValues.push_back( m_fRemainTime );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 각성 행동 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	/// June 32e7ddb0: wire layout unchanged, [count, remaining time]
	m_uiHyperModeCount = static_cast<UINT>( kFactor_[0] );
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_BERSERK_BY_CHANCE
	/// June @32e7de0a: unit vslot 0xa4 ( berserk by chance, false but for Chung ), not IsFullHyperMode
	if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount || pGameUnit_->IsBerserkByChance() )
#else
	if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount || pGameUnit_->IsFullHyperMode() )
#endif SERV_IRUHADEV_JUNE_DIVERGE_BERSERK_BY_CHANCE
		pGameUnit_->SetFullHyperMode( true );

	/// June then calls 32b4dd50( count, or 1 ) (@32e7dc69 / @32e7de36), before slot 49: the buff-driven hyper count
	/// (June +0x6844). Not March's SetHyperModeCount, which is the gage's orb count.
	pGameUnit_->SetHyperModeCountByBuff( static_cast<int>( 1 > m_uiHyperModeCount ? 1 : m_uiHyperModeCount ) );

	m_fRemainTime = kFactor_[1];
	pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
	pGameUnit_->HyperModeBuffEffectStart();
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_uiHyperModeCount = static_cast<UINT>( kFactor_[0] );
	if ( static_cast<UINT>(CX2GageUI::PGUWO_ORB3) < m_uiHyperModeCount )
		pGameUnit_->SetFullHyperMode( true );

	m_fRemainTime = kFactor_[1];
	pGameUnit_->SetRemainHyperModeTime( m_fRemainTime );
	pGameUnit_->HyperModeBuffEffectStart();
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	/// June 32e7de90: as March, then 32b4dd50( 0 ) after slot 50 (@32e7ded7)
	CX2GageData* pGageData = CX2GageManager::GetInstance()->GetMyGageData();
	if ( NULL != pGageData )
		pGageData->SetHyperModeRemainTime( 0.0f );

	pGameUnit_->SetRemainHyperModeTime( 0.0f );
	pGameUnit_->HyperModeBuffEffectEnd();
	pGameUnit_->SetHyperModeCountByBuff( 0 );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffChangeHyperModeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GageData* pGageData = CX2GageManager::GetInstance()->GetMyGageData();
	if ( NULL != pGageData )
		pGageData->SetHyperModeRemainTime( 0.0f );

	pGameUnit_->SetRemainHyperModeTime( 0.0f );
	pGameUnit_->HyperModeBuffEffectEnd();
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : OnFrameMove
	@brief : 스킬 쿨타임 초기화
	@param : 버프에 걸리 게임유닛(pGameUnit_), 버프의 템플리정보(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : OnFrameMove
	@brief : June 32e85fe0. Hands both skill-ID lists to the unit (slots 161/162), then resets through
			 slot 160 every frame, or on the first frame only with APPLY_ONCE. DRAW_TEXT floats the
			 change over the unit when a slot changed (June 32e857d0, DrawText below).
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( !DidStart() )
		return;

	pGameUnit_->SetResetCoolTimeIgnoreSkillID( m_vecIgnoreSkillID );
	pGameUnit_->SetResetCoolTimeForceSkillID( m_vecForceSkillID );

	if ( true == m_bApplyOnce && true == m_bApplied )
		return;

	m_bApplied = true;
	const bool bChanged = pGameUnit_->ResetLeftSkillCoolTimeByBuff( static_cast<CX2SkillTree::SKILL_TYPE>( m_eSkillTypeToResetCoolTime ),
		m_fCoolTime, m_bForceSet, m_eChangeType, m_fCoolTimeRate, m_bIsRelativeAllSkill, m_iSkillSlotType );

	if ( true == m_bDrawText && true == bChanged )
	{
		DrawText( pGameUnit_, m_eChangeType, m_fCoolTime, m_fCoolTimeRate );		/// @32e86090: +0x14, +0x18, +0x1c
	}
}

/** @function : DrawText
	@brief : June 32e857d0. Floats the cool-time change in PicCharYellowSmall at the head bone
			 (unit vslot 63 = GetHeadBonePos, vslot 91 = GetDirVector), centred, no gap time.
			 STR 38799 "COOLDOWN" and 38800 "SEC"; StringCchPrintfW into 20 wide chars (31aa55f0).
			 SWAP "%s = %d%s"; FIX "%s +%d%s", or "%s %d%s" below 0 (a NaN takes "+"); any other change
			 type draws an empty string, as in June. The value is truncated to int (_ftol2). The rate is
			 passed and unused, as in June. June's string-ID debug view ("[S%1%]%2%") is not ported.
*/
void CX2BuffResetSkillCoolTimeBehaviorTemplet::DrawText( CX2GameUnit* pGameUnit_, const BUFF_CHANGE_TYPE eChangeType_, const float fCoolTime_, const float /*fCoolTimeRate_*/ ) const
{
	WCHAR wszText[20] = { 0, };
	switch ( eChangeType_ )
	{
	case BCT_SWAP_VALUE:
		StringCchPrintf( wszText, _countof( wszText ), L"%s = %d%s", GET_STRING( 38799 ), static_cast<int>( fCoolTime_ ), GET_STRING( 38800 ) );
		break;

	case BCT_FIX_VALUE:
		if ( fCoolTime_ < 0.0f )
			StringCchPrintf( wszText, _countof( wszText ), L"%s %d%s", GET_STRING( 38799 ), static_cast<int>( fCoolTime_ ), GET_STRING( 38800 ) );
		else
			StringCchPrintf( wszText, _countof( wszText ), L"%s +%d%s", GET_STRING( 38799 ), static_cast<int>( fCoolTime_ ), GET_STRING( 38800 ) );
		break;

	default:
		break;
	}

	if ( NULL != g_pData && NULL != g_pData->GetPicCharYellowSmall() && NULL != pGameUnit_ )
		g_pData->GetPicCharYellowSmall()->DrawText( wszText, pGameUnit_->GetHeadBonePos(), pGameUnit_->GetDirVector(), CKTDGPicChar::AT_CENTER, 0.0f );
}

/** @function : SetFactor
	@brief : June 32e76aa0. The factor's change type must equal the templet's. SWAP and FIX take the
			 value as a cool time, RELATION and PERCENT as a rate (PERCENT only within 0..1, stored
			 before the check). June raises no popup on any failure.
*/
/*virtual*/ bool CX2BuffResetSkillCoolTimeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffResetSkillCoolTimeBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( static_cast<int>( (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != static_cast<int>( m_eChangeType ) )
		return DISPLAY_ERROR( L"CX2BuffResetSkillCoolTimeBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	switch ( m_eChangeType )
	{
	case BCT_SWAP_VALUE:
	case BCT_FIX_VALUE:
		m_fCoolTime = (*pBehaviorFactor)[FO_VALUE];
		return true;

	case BCT_RELATION_VALUE:
		m_fCoolTimeRate = (*pBehaviorFactor)[FO_VALUE];
		return true;

	case BCT_PERCENT:
		m_fCoolTimeRate = (*pBehaviorFactor)[FO_VALUE];
		return ( 0.0f <= m_fCoolTimeRate && m_fCoolTimeRate <= 1.0f );

	default:
		return DISPLAY_ERROR( L"CX2BuffResetSkillCoolTimeBehaviorTemplet::SetFactor: unknown BUFF_CHANGE_TYPE" )	/// 4b-ii: June returns false here silently; logged once
	}
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffResetSkillCoolTimeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffResetSkillCoolTimeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : June 32e7dee0: the type only, no values (as March). June has no SetFactorFromPacket override.
*/
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : ParsingBehavior
	@brief : June 32e78e90. BUFF_CHANGE_TYPE and SKILL_TYPE are required (no popup); RELATIVE_ALL_SKILL,
			 FORCE_SET, DRAW_TEXT, SKILL_SLOT_TYPE, the IGNORE_SKILL_ID / FORCE_SKILL_ID lists and
			 APPLY_ONCE are optional.
*/
/*virtual*/ bool CX2BuffResetSkillCoolTimeBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	int iChangeType = 0;
	if ( false == luaManager_.GetValue( "BUFF_CHANGE_TYPE", iChangeType ) )
	{
		m_eChangeType = BCT_SWAP_VALUE;
		return false;
	}
	m_eChangeType = static_cast<BUFF_CHANGE_TYPE>( iChangeType );

	if ( false == luaManager_.GetValue( "SKILL_TYPE", m_eSkillTypeToResetCoolTime ) )
	{
		m_eSkillTypeToResetCoolTime = 0;
		return false;
	}

	LUA_GET_VALUE( luaManager_, "RELATIVE_ALL_SKILL", m_bIsRelativeAllSkill, false );
	LUA_GET_VALUE( luaManager_, "FORCE_SET", m_bForceSet, false );
	LUA_GET_VALUE( luaManager_, "DRAW_TEXT", m_bDrawText, false );
	LUA_GET_VALUE( luaManager_, "SKILL_SLOT_TYPE", m_iSkillSlotType, 0 );

	if ( true == luaManager_.BeginTable( "IGNORE_SKILL_ID" ) )
	{
		int iIndex = 1;
		int iSkillID = -1;
		while ( true == luaManager_.GetValue( iIndex, iSkillID ) )
		{
			m_vecIgnoreSkillID.push_back( iSkillID );
			++iIndex;
		}
		luaManager_.EndTable();
	}

	if ( true == luaManager_.BeginTable( "FORCE_SKILL_ID" ) )
	{
		int iIndex = 1;
		int iSkillID = -1;
		while ( true == luaManager_.GetValue( iIndex, iSkillID ) )
		{
			m_vecForceSkillID.push_back( iSkillID );
			++iIndex;
		}
		luaManager_.EndTable();
	}

	LUA_GET_VALUE( luaManager_, "APPLY_ONCE", m_bApplyOnce, false );
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( DidStart() )
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환

#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
		pGameUnit_->ResetLeftSkillCoolTimeBySkillType( static_cast<CX2SkillTree::SKILL_TYPE>( m_eSkillTypeToResetCoolTime ), m_fCoolTime, false, m_eChangeType, m_fCoolTimeRate, m_bIsRelativeAllSkill );
#else // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME
		pGameUnit_->ResetLeftSkillCoolTimeBySkillType( static_cast<CX2SkillTree::SKILL_TYPE>( m_eSkillTypeToResetCoolTime ), m_fCoolTime, false, m_eChangeType );
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
		
#else // UPGRADE_SKILL_SYSTEM_2013
		pGameUnit_->ResetLeftSkillCoolTimeBySkillType( static_cast<CX2SkillTree::SKILL_TYPE>( m_eSkillTypeToResetCoolTime ) );
#endif // UPGRADE_SKILL_SYSTEM_2013
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffResetSkillCoolTimeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
			switch ( m_eChangeType )
			{
			case BCT_FIX_VALUE:
			case BCT_RELATION_VALUE:
				{
					m_fCoolTime = (*pBehaviorFactor)[FO_VALUE];
					return true;

				} break;
#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
			case BCT_PERCENT :
				{
					m_fCoolTimeRate = (*pBehaviorFactor)[FO_VALUE];

					if ( m_fCoolTimeRate < 0 || m_fCoolTimeRate > 1 ) 
						return DISPLAY_ERROR( L"MISS MATCH BCT_PERCENT" );

					return true;			
				} break;
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
			
			default:
				{
#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME				// 김종훈, 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
					return DISPLAY_ERROR( L"BCT_FIX_VALUE or BCT_RELATION_VALUE or BCT_PERCENT" );
#else  // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME
					return DISPLAY_ERROR( L"BCT_FIX_VALUE or BCT_RELATION_VALUE" );
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈, 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
				} break;
			}
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
#endif // UPGRADE_SKILL_SYSTEM_2013
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffResetSkillCoolTimeBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffResetSkillCoolTimeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffResetSkillCoolTimeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffResetSkillCoolTimeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
}

/** @function : ParsingBehavior
	@brief : 버프템플릿 스크립트로 부터 초기화 할 스킬 타입을 읽어들임
	@param : 루아매니터(luaManager_)
	@return : 성공시 true, 실패시 false
*/
/*virtual*/ bool CX2BuffResetSkillCoolTimeBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );
#endif // UPGRADE_SKILL_SYSTEM_2013

	LUA_GET_VALUE_RETURN( luaManager_, "SKILL_TYPE", m_eSkillTypeToResetCoolTime, 0, return DISPLAY_ERROR( L"SKILL_TYPE") );

#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
	LUA_GET_VALUE ( luaManager_, "RELATIVE_ALL_SKILL", m_bIsRelativeAllSkill, 0 );	
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
	return true;
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffChangeUnitScaleBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( !DidStart() )	/// 처음 적용
		{
			pGameUnit_->CreateAndInsertChangeUnitScaleByBuff( pBuffTemplet_->GetBuffIdentity(), m_vScale );
		}
#ifdef POSSIBLE_ADD_CHANGE_SCALE_BUFF_VALUE
		else				/// 중첩 또는 교체
		{
			pGameUnit_->UpdateChangeUnitScaleByBuff( pBuffTemplet_->GetBuffIdentity(), m_vScale,
													pBuffTemplet_->GetAccumulationLimit());
		}
#endif POSSIBLE_ADD_CHANGE_SCALE_BUFF_VALUE

		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeUnitScaleBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeUnitScaleBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeUnitScaleBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeUnitScaleBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 유닛의 스케일을 변경하는 행동 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeUnitScaleBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( !DidStart() )
		pGameUnit_->CreateAndInsertChangeUnitScaleByBuff( kBuffFactor_.m_BuffIdentity, m_vScale );
}

/** @function : ParsingBehavior
	@brief : 유닛의 스케일을 변경하는 행동 템플릿을 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeUnitScaleBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN( luaManager_, "SCALE_X", m_vScale.x, 0.0f, return DISPLAY_ERROR( L"SCALE_X" ) );
	LUA_GET_VALUE_RETURN( luaManager_, "SCALE_Y", m_vScale.y, 0.0f, return DISPLAY_ERROR( L"SCALE_Y" ) );
	LUA_GET_VALUE_RETURN( luaManager_, "SCALE_Z", m_vScale.z, 0.0f, return DISPLAY_ERROR( L"SCALE_Z" ) );

	/// 스케일이 원래대로 돌아갈 때 정해진 시간간격과 방법을 사용할 것인가
	LUA_GET_VALUE_RETURN( luaManager_, "USE_TIME_TO_RETURN", m_bUseTimeToReturn, false, return DISPLAY_ERROR( L"USE_TIME_TO_RETURN" ) );

	return true;
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeUnitScaleBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	if ( DidStart() )
		pGameUnit_->EraseChangeUnitScaleByBuff( pBuffTemplet_->GetBuffIdentity(), m_bUseTimeToReturn );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffCustomFunctionBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( !DidStart() )
		{
			m_kBuffBehaviorFactor = *pBehaviorFactor;
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
			/// June 32e79b00: the third argument is { !factor +0x38, factor +0x30 (the creator SetGameUnitBuffFactor
			/// stored), templet +0xbc (the creator's power rate, CX2BuffTemplet::SetFactor) }
			KCustomFunctionCreatorInfo kCreatorInfo;
			kCreatorInfo.m_bNotUser				= !BuffFactor_.GetIsUserUID();
			kCreatorInfo.m_uidCreator			= BuffFactor_.GetUidGameUnit();
			kCreatorInfo.m_fCreatorPowerRate	= pBuffTemplet_->GetCreatorPowerRate();
			pGameUnit_->CreateAndInsertCustomFunctionByBuff( pBuffTemplet_->GetBuffIdentity(), *pBehaviorFactor, kCreatorInfo );
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
			pGameUnit_->CreateAndInsertCustomFunctionByBuff( pBuffTemplet_->GetBuffIdentity(), *pBehaviorFactor );
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
		}

		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffCustomFunctionBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffCustomFunctionBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffCustomFunctionBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffCustomFunctionBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( m_kBuffBehaviorFactor );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 CustomFunction을 유닛에 적용
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffCustomFunctionBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	/// June 32e73f90: the creator info from the KBuffFactor: the UID (+0x28/+0x2c = m_uidMesGameUnit), not-user
	/// (+0x3d = m_bIsMesGameUnitNpc) and the creator's power rate (+0x57 = m_fJuneCreatorPowerRate, Phase 9c). June's
	/// templet GetFactor (32369f50) writes all three from the creator its SetFactor stored.
	if ( !DidStart() )
	{
		KCustomFunctionCreatorInfo kCreatorInfo;
		kCreatorInfo.m_bNotUser				= kBuffFactor_.m_bIsMesGameUnitNpc;
		kCreatorInfo.m_uidCreator			= kBuffFactor_.m_uidMesGameUnit;
#ifdef SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
		kCreatorInfo.m_fCreatorPowerRate	= kBuffFactor_.m_fCreatorPowerRate;
#endif SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE
		pGameUnit_->CreateAndInsertCustomFunctionByBuff( kBuffFactor_.m_BuffIdentity, kFactor_, kCreatorInfo );
	}
#else SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
	if ( !DidStart() )
		pGameUnit_->CreateAndInsertCustomFunctionByBuff( kBuffFactor_.m_BuffIdentity, kFactor_ );
#endif SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffCustomFunctionBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	if ( DidStart() )
		pGameUnit_->EraseCustomFunctionByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffEffectSetWithDamageBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( NULL != g_pX2Game && !DidStart() )
		{
			m_bNotUse = ( 0.0f < (*pBehaviorFactor)[0] ? true : false );
			m_fPowerRate = pGameUnit_->GetPowerRate();	/// 사용 당시의 PowerRate 저장
			if ( !m_bNotUse )	/// FACTOR에서 사용하지 않겠다고 지정한것이 아니면
				PlayEffectSet( pGameUnit_ );
		}
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffEffectSetWithDamageBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/CX2BuffBehaviorTempletPtr CX2BuffEffectSetWithDamageBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffEffectSetWithDamageBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/void CX2BuffEffectSetWithDamageBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( ( true == m_bNotUse ? 1.0f : 0.0f ) );
	BehaviorFactor.m_vecValues.push_back( m_fPowerRate );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 데미지를 이펙트셋을 실행
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/void CX2BuffEffectSetWithDamageBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_bNotUse = ( 0.0f < kFactor_[0] ? true : false );
	m_fPowerRate = kFactor_[1];
	if ( !m_bNotUse )	/// FACTOR에서 사용하지 않겠다고 지정한것이 아니면
		PlayEffectSet( pGameUnit_ );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/void CX2BuffEffectSetWithDamageBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	if ( NULL != g_pX2Game && DidStart() )
	{
		BOOST_FOREACH( CX2EffectSet::Handle& hHandle, m_vecHandleEffectSet )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( hHandle );
		}
	}
}

/** @function : ParsingBehavior
	@brief : 유닛의 스케일을 변경하는 행동 템플릿을 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/bool CX2BuffEffectSetWithDamageBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	int iIndex = 1;

	wstring wstrEffectSetName;
	while( luaManager_.GetValue( iIndex, wstrEffectSetName ) )
	{
		m_vecWstrEffectSetName.push_back( wstrEffectSetName );
		m_vecHandleEffectSet.push_back( INVALID_EFFECTSET_HANDLE );
		wstrEffectSetName.resize(0);
		++iIndex;
	}

	if ( 1 != iIndex )	
		return true;
	else
		return false;
}

/** @function : PlayEffectSet
	@brief : 벡터에 있는 이펙트 셋들을 실행
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_)
*/
void CX2BuffEffectSetWithDamageBehaviorTemplet::PlayEffectSet( CX2GameUnit* pGameUnit_ )
{
	if ( NULL != g_pX2Game && !DidStart() )
	{
		UINT uiSize = m_vecWstrEffectSetName.size();
		for ( UINT uiIndex = 0; uiIndex < uiSize; ++uiIndex )
		{
			m_vecHandleEffectSet[uiIndex] 
			= g_pX2Game->GetEffectSet()->PlayEffectSet( m_vecWstrEffectSetName[uiIndex], pGameUnit_,
				NULL, ( 0.0f < pGameUnit_->GetRemainHyperModeTime() ), m_fPowerRate, -1.0f);//, pGameUnit_->GetScaleByUnit() );
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffStunBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
		if ( !DidStart() )	/// 중첩 적용 안함
		{
			// 교체 적용 안함(교체도 효과가 달라지는게 아니기 때문에 적용할 필요 없음)
			pGameUnit_->CreateAndInsertStunByBuff( pBuffTemplet_->GetBuffIdentity() ); /// 좌우반전 인스턴스 생성 및 적용

#ifdef NOT_CANCEL_BBT_STUN_BY_HIT		/// 적용 즉시 그로기 상태로 변경 ( CommonFrameMove에서 처음 적용시엔 무조건 그로기 상태 변경하도록 설정 )
			if( CX2GameUnit::GUT_USER ==  pGameUnit_->GetGameUnitType() )
			{
				CX2GUUser *pUser = static_cast<CX2GUUser*>( pGameUnit_ );
				if( NULL != pUser )
					pUser->SetChangeGroggyState( true );
			}
#endif NOT_CANCEL_BBT_STUN_BY_HIT
		}	
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffStunBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/CX2BuffBehaviorTempletPtr CX2BuffStunBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffStunBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/void CX2BuffStunBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 데미지를 이펙트셋을 실행
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/void CX2BuffStunBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	pGameUnit_->CreateAndInsertStunByBuff( kBuffFactor_.m_BuffIdentity );

#ifdef NOT_CANCEL_BBT_STUN_BY_HIT		/// 적용 즉시 그로기 상태로 변경 ( CommonFrameMove에서 처음 적용시엔 무조건 그로기 상태 변경하도록 설정 )
	if( CX2GameUnit::GUT_USER ==  pGameUnit_->GetGameUnitType() )
	{
		CX2GUUser *pUser = static_cast<CX2GUUser*>( pGameUnit_ );
		if( NULL != pUser )
			pUser->SetChangeGroggyState( true );
	}
#endif NOT_CANCEL_BBT_STUN_BY_HIT
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/void CX2BuffStunBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseStunByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffFinishOtherBuffDebuffBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		EraseBuffTempletFromGameUnit( pGameUnit_, BuffFactor_.GetBuffTempletID() );
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffFinishOtherBuffDebuffBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffFinishOtherBuffDebuffBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffFinishOtherBuffDebuffBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffFinishOtherBuffDebuffBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 특정 버프들을 종료 시킴
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
///*virtual*/ void CX2BuffFinishOtherBuffDebuffBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
// {
// 	EraseBuffTempletFromGameUnit( pGameUnit_ );
// }

/** @function : ParsingBehavior
	@brief : 종료 시킬 버프의 템플리ID를 루아스크립트로 부터 읽어들인다
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffFinishOtherBuffDebuffBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	int iIndex = 1;

	UINT uiBuffTempletID = 0;
	while( luaManager_.GetValue( iIndex, uiBuffTempletID ) )
	{
		m_vecBuffTempletIDToBeFinished.push_back( static_cast<BUFF_TEMPLET_ID>( uiBuffTempletID ) );
		++iIndex;
	}

	if ( 1 != iIndex )	
		return true;
	else
		return false;
}

/** @function : EraseBuffTempletFromGameUnit
	@brief : 특정 버프가 있는 경우 종료시킨다
	@param : 버프에 걸린 게임 유닛(pGameUnit_), 삭제에서 제외할 BuffTempletID로 자기자신을 삭제하는 경우를 방지하기 위해 자신의 BuffTempletID를 넣어준다.
*/
void CX2BuffFinishOtherBuffDebuffBehaviorTemplet::EraseBuffTempletFromGameUnit( CX2GameUnit* pGameUnit_, const BUFF_TEMPLET_ID eExceptBuffTempletID_ )
{
	BOOST_FOREACH( const BUFF_TEMPLET_ID eBuffTempletID_, m_vecBuffTempletIDToBeFinished )
	{
		if ( eExceptBuffTempletID_ != eBuffTempletID_ )
			pGameUnit_->EraseBuffTempletFromGameUnit( eBuffTempletID_ );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffFreezeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		/// 중첩이 가능하다면 현재 얼마나 중첩되어 있는지를 얻어냄 (첫 효과 or 중첩이 안되면 0 이다)
		if ( !DidStart() )	/// 중첩 적용 안함
		{
			// 교체 적용 안함(교체도 효과가 달라지는게 아니기 때문에 적용할 필요 없음)
			pGameUnit_->CreateAndInsertFreezeByBuff( pBuffTemplet_->GetBuffIdentity() ); /// 좌우반전 인스턴스 생성 및 적용
			m_fAnimCurrentTime = pGameUnit_->GetNowAnimationTime();
			pGameUnit_->AnimStop();
		}	
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffStunBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/CX2BuffBehaviorTempletPtr CX2BuffFreezeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffFreezeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/void CX2BuffFreezeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fAnimCurrentTime );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 데미지를 이펙트셋을 실행
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/void CX2BuffFreezeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fAnimCurrentTime = kFactor_[0];
	pGameUnit_->SetNowAnimationTime( m_fAnimCurrentTime );
	pGameUnit_->AnimStop();
	//pGameUnit_->CreateAndInsertFreezeByBuff( kBuffFactor_.m_BuffIdentity );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e76d10: the animation resumes (with its own play type) only while the unit lives: no gage data, or
/// NowHp > 0 (a null HP gage reads 0, so it takes the other path, as in June), and not GUSI_DIE. Otherwise June
/// plays XAP_ONE_WAIT. The down transition is March's.
/*virtual*/void CX2BuffFreezeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseFreezeByBuff( pBuffTemplet_->GetBuffIdentity() );

	pGameUnit_->SetStateToNowAnimation( CKTDGXSkinAnim::XAS_PLAYING );

	CX2GageData* pGageData = pGameUnit_->GetGageData();
	if ( ( NULL == pGageData || 0.0f < pGageData->GetNowHp() ) && CX2GameUnit::GUSI_DIE != pGameUnit_->GetGameUnitState() )
		pGameUnit_->AnimPlay();
	else
		pGameUnit_->AnimPlayByPlayType( CKTDGXSkinAnim::XAP_ONE_WAIT );

	if( true == m_bIsDown && CX2GameUnit::GUT_USER == pGameUnit_->GetGameUnitType())
	{
#ifdef RIDING_SYSTEM
		if ( true == pGameUnit_->GetRidingOn() )
			pGameUnit_->StateChange( CX2GUUser::USI_RIDING_DAMAGE_FRONT, true );
		else
#endif // RIDING_SYSTEM
			pGameUnit_->StateChange( CX2GUUser::USI_DAMAGE_DOWN_FRONT, true );
	}
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/void CX2BuffFreezeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseFreezeByBuff( pBuffTemplet_->GetBuffIdentity() );

	pGameUnit_->SetStateToNowAnimation( CKTDGXSkinAnim::XAS_PLAYING );

	pGameUnit_->AnimPlay();

	if( true == m_bIsDown && CX2GameUnit::GUT_USER == pGameUnit_->GetGameUnitType())		/// 버프 종료시 다운시킬지 설정 ( NPC인 경우, 다운 시키지 않는다. )
	{
#ifdef RIDING_SYSTEM
		/// 탈것 탑승중이면, 탈것 데미지 스테이트로 전환
		if ( true == pGameUnit_->GetRidingOn() )
			pGameUnit_->StateChange( CX2GUUser::USI_RIDING_DAMAGE_FRONT, true );
		else
#endif // RIDING_SYSTEM
			pGameUnit_->StateChange( CX2GUUser::USI_DAMAGE_DOWN_FRONT, true );
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffFreezeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
void CX2BuffFreezeBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( DidStart() )
		pGameUnit_->AnimStop();
}

/** @function : ParsingBehavior
	@brief : 동작이 멈추는 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffFreezeBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN( luaManager_, "IS_DOWN", m_bIsDown, false, return DISPLAY_ERROR( L"IS_DOWN" ) );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e74260: also saves the unit's can-pass flag (unit vslot 68 with false: the now frame). It does so on
/// every SetFactor, with no started guard, so a restack saves the buff's own value: June's, transcribed.
/*virtual*/ bool CX2BuffCanPassUnitBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffCanPassUnitBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( NULL != pGameUnit_ )
		m_bCanPassUnitBeforeBuff = pGameUnit_->GetCanPassUnitNow();
	return true;
}

/** @function : DoFinish
	@brief : June 32e72890: restores the can-pass flag SetFactor saved (unit vslot 233). March had no DoFinish.
	@param : unit the buff was on (pGameUnit_), its templet (pBuffTemplet_)
*/
/*virtual*/ void CX2BuffCanPassUnitBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	if ( NULL != pGameUnit_ )
		pGameUnit_->SetCanPassUnit( m_bCanPassUnitBeforeBuff );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffCanPassUnitBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffCanPassUnitBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffCanPassUnitBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffCanPassUnitBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffCanPassUnitBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : OnFrameMove
	@brief : 매 프레임 SetCanPassUnit 지정
	@param : 버프에 걸리 게임유닛(pGameUnit_), 버프의 템플리정보(pBuffTemplet_)
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffCanPassUnitBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffCanPassUnitBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( DidStart() )
		pGameUnit_->SetCanPassUnit( m_bCanPassUnit );
}

/** @function : ParsingBehavior
	@brief : 종료 시킬 버프의 템플리ID를 루아스크립트로 부터 읽어들인다
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffCanPassUnitBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN( luaManager_, "CAN_PASS_UNIT", m_bCanPassUnit, 
		false, return DISPLAY_ERROR( L"CAN_PASS_UNIT" ) );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : ParsingBehavior
	@brief : June 32e7a4a0. IS_GRAVITY is required: without it the parse fails before the other
			 keys are read (June raises no popup). CAN_DOWN, OFFSET_POSITION (a D3DXVECTOR3) and
			 DELETE_BUFF_WHEN_DIE are optional and default to false / zero.
*/
/*virtual*/ bool CX2BuffNeverMoveBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	if ( false == luaManager_.GetValue( "IS_GRAVITY", m_bIsGravity ) )
	{
		m_bIsGravity = false;
		return false;
	}

	LUA_GET_VALUE( luaManager_, "CAN_DOWN", m_bCanDown, false );
	LUA_GET_USER_DEFINED_TYPE_VALUE( luaManager_, "OFFSET_POSITION", m_vOffsetPosition, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	LUA_GET_VALUE( luaManager_, "DELETE_BUFF_WHEN_DIE", m_bDeleteBuffWhenDie, false );
	return true;
}

/** @function : SetFactor
	@brief : June 32e744c0. The anchor is the unit's future position plus OFFSET_POSITION. June hands
			 the store a constant false for "can down" (3227abb0 writes +0x1d = 0), so CAN_DOWN does
			 not reach it; that is CreateAndInsertNeverMoveByBuff's own default here.
*/
/*virtual*/ bool CX2BuffNeverMoveBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffNeverMoveBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( !DidStart() )
	{
		m_vPosition = pGameUnit_->GetPos( true ) + m_vOffsetPosition;
		pGameUnit_->CreateAndInsertNeverMoveByBuff( pBuffTemplet_->GetBuffIdentity(), m_vPosition, m_bIsGravity );
	}
	return true;
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffNeverMoveBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffNeverMoveBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : June 32e7e330. March's four values (position, gravity), then CAN_DOWN and
			 OFFSET_POSITION appended, so the existing order is unchanged on the wire.
*/
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_vPosition.x );
	BehaviorFactor.m_vecValues.push_back( m_vPosition.y );
	BehaviorFactor.m_vecValues.push_back( m_vPosition.z );
	BehaviorFactor.m_vecValues.push_back( ( true == m_bIsGravity ? 1.0f : 0.0f ) );
	BehaviorFactor.m_vecValues.push_back( ( true == m_bCanDown ? 1.0f : 0.0f ) );
	BehaviorFactor.m_vecValues.push_back( m_vOffsetPosition.x );
	BehaviorFactor.m_vecValues.push_back( m_vOffsetPosition.y );
	BehaviorFactor.m_vecValues.push_back( m_vOffsetPosition.z );
}

/** @function : SetFactorFromPacket
	@brief : June 32e76e70. Values 4..7 are optional: a 4-value packet leaves CAN_DOWN false and the
			 offset untouched. With 7 or more values June adds the received offset to the received
			 position, although GetFactor already sent the offset position (transcribed as June has it).
*/
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_vPosition.x = kFactor_[0];
	m_vPosition.y = kFactor_[1];
	m_vPosition.z = kFactor_[2];
	m_bIsGravity  = ( 0.0f < kFactor_[3] ? true : false );
	m_bCanDown    = ( 0.0f < kFactor_[4] ? true : false );

	if ( 7 <= kFactor_.m_vecValues.size() )
	{
		m_vOffsetPosition.x = kFactor_[5];
		m_vOffsetPosition.y = kFactor_[6];
		m_vOffsetPosition.z = kFactor_[7];
		m_vPosition += m_vOffsetPosition;
	}

	pGameUnit_->CreateAndInsertNeverMoveByBuff( kBuffFactor_.m_BuffIdentity, m_vPosition, m_bIsGravity );
}

/** @function : OnFrameMove
	@brief : June 32e77000 (March has no override). With DELETE_BUFF_WHEN_DIE the anchor is released
			 once the unit has no HP left (June: gage data +0xc04 present and HP <= 0) or is in
			 GUSI_DIE (slot 67, GetGameUnitState). The buff itself keeps running.
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
	if ( NULL == pGameUnit_ || false == m_bDeleteBuffWhenDie )
		return;

	const CX2GageData* pGageData = static_cast<const CX2GameUnit*>( pGameUnit_ )->GetGageData();
	if ( ( NULL == pGageData || 0.0f < pGageData->GetNowHp() )
		&& CX2GameUnit::GUSI_DIE != pGameUnit_->GetGameUnitState() )
		return;

	pGameUnit_->EraseNeverMoveByBuff( pBuffTemplet_->GetBuffIdentity() );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef NEVER_MOVE_GRAVITY_APPLY
/** @function : ParsingBehavior
	@brief : 중력이 적용되는가를 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffNeverMoveBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN( luaManager_, "IS_GRAVITY", m_bIsGravity, false, return DISPLAY_ERROR( L"IS_GRAVITY" ) );

	return true;
}
#endif //NEVER_MOVE_GRAVITY_APPLY


/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffNeverMoveBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( !DidStart() )
		{
			m_vPosition = pGameUnit_->GetPos(true);
#ifdef NEVER_MOVE_GRAVITY_APPLY
			pGameUnit_->CreateAndInsertNeverMoveByBuff( pBuffTemplet_->GetBuffIdentity(), m_vPosition, m_bIsGravity );
#else
			pGameUnit_->CreateAndInsertNeverMoveByBuff( pBuffTemplet_->GetBuffIdentity(), m_vPosition );
#endif //NEVER_MOVE_GRAVITY_APPLY
		}
		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffNeverMoveBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffNeverMoveBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffNeverMoveBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_vPosition.x );
	BehaviorFactor.m_vecValues.push_back( m_vPosition.y );
	BehaviorFactor.m_vecValues.push_back( m_vPosition.z );
#ifdef NEVER_MOVE_GRAVITY_APPLY
	BehaviorFactor.m_vecValues.push_back( ( true == m_bIsGravity ? 1.0f : 0.0f ) );
#endif //NEVER_MOVE_GRAVITY_APPLY
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 행동 템플릿 실행
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_vPosition.x = kFactor_[0];
	m_vPosition.y = kFactor_[1];
	m_vPosition.z = kFactor_[2];
#ifdef NEVER_MOVE_GRAVITY_APPLY
	m_bIsGravity  = ( 0.0f < kFactor_[3] ? true : false );
#endif //NEVER_MOVE_GRAVITY_APPLY

#ifdef NEVER_MOVE_GRAVITY_APPLY
	pGameUnit_->CreateAndInsertNeverMoveByBuff( kBuffFactor_.m_BuffIdentity, m_vPosition, m_bIsGravity );
#else
	pGameUnit_->CreateAndInsertNeverMoveByBuff( kBuffFactor_.m_BuffIdentity, m_vPosition );
#endif //NEVER_MOVE_GRAVITY_APPLY
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffNeverMoveBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseNeverMoveByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : 타입에 따른 공격력 수치를 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/** @function : ParsingBehavior
	@brief : June 32e745b0: BUFF_CHANGE_TYPE, required (no popup)
*/
/*virtual*/ bool CX2BuffChangeAttackByTypeBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	int iChangeType = 0;
	if ( false == luaManager_.GetValue( "BUFF_CHANGE_TYPE", iChangeType ) )
	{
		m_eChangeType = BCT_SWAP_VALUE;
		return false;
	}

	m_eChangeType = static_cast<BUFF_CHANGE_TYPE>( iChangeType );
	return true;
}

/** @function : SetFactor
	@brief : June 32e77070. BCT_PERCENT only, and no popups. The first application stores the value, the
			 attack type and ADDITIONAL_ATTACK_TYPE (factor value 3) on the unit; a restack keeps the larger
			 magnitude, scales it by the accumulation multiplier (NowHp's formula, 32e798d0) and replaces the
			 unit's entry.
*/
/*virtual*/ bool CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( static_cast<int>( (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != static_cast<int>( m_eChangeType ) || BCT_PERCENT != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	const float fNewValue = (*pBehaviorFactor)[FO_VALUE];

	if ( !DidStart() )
	{
		m_fResultValue = fNewValue;
		m_eAttackType = static_cast<CX2DamageManager::ATTACK_TYPE>( static_cast<int>( (*pBehaviorFactor)[FO_ATTACK_TYPE] ) );
		m_iAdditionalAttackType = static_cast<int>( (*pBehaviorFactor)[3] );
		pGameUnit_->CreateAndInsertChangeAttackByType( pBuffTemplet_->GetBuffIdentity(), m_eAttackType, m_fResultValue, m_iAdditionalAttackType );
		return true;
	}

	const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
	if ( uiAccumulationCountBefore > 0 )
	{
		const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
		const float fOldValue = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
		m_fResultValue = ( abs(fOldValue) < abs(fNewValue) ? fNewValue : fOldValue );
		m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );
	}
	else
		m_fResultValue = ( abs(m_fResultValue) < abs(fNewValue) ? fNewValue : m_fResultValue );

	m_eAttackType = static_cast<CX2DamageManager::ATTACK_TYPE>( static_cast<int>( (*pBehaviorFactor)[FO_ATTACK_TYPE] ) );
	m_iAdditionalAttackType = static_cast<int>( (*pBehaviorFactor)[3] );
	pGameUnit_->EraseChangeAttackByType( pBuffTemplet_->GetBuffIdentity() );
	pGameUnit_->CreateAndInsertChangeAttackByType( pBuffTemplet_->GetBuffIdentity(), m_eAttackType, m_fResultValue, m_iAdditionalAttackType );
	return true;
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeAttackByTypeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeAttackByTypeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : June 32e7e440: March's three values, then ADDITIONAL_ATTACK_TYPE appended
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eAttackType ) );
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_iAdditionalAttackType ) );
}

/** @function : SetFactorFromPacket
	@brief : June 32e74740: needs all four values (a three-value packet from a March client is ignored)
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( kFactor_.m_vecValues.size() < 4 )
		return;

	m_eChangeType			= static_cast<BUFF_CHANGE_TYPE>( static_cast<int>( kFactor_.m_vecValues[FO_CHANGE_TYPE] ) );
	m_fResultValue			= kFactor_.m_vecValues[FO_VALUE];
	m_eAttackType			= static_cast<CX2DamageManager::ATTACK_TYPE>( static_cast<int>( kFactor_.m_vecValues[FO_ATTACK_TYPE] ) );
	m_iAdditionalAttackType	= static_cast<int>( kFactor_.m_vecValues[3] );

	pGameUnit_->CreateAndInsertChangeAttackByType( kBuffFactor_.m_BuffIdentity, m_eAttackType, m_fResultValue, m_iAdditionalAttackType );
}

/** @function : DoFinish
	@brief : June 32e747b0 (as March)
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseChangeAttackByType( pBuffTemplet_->GetBuffIdentity() );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeAttackByTypeBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
			if ( BCT_PERCENT == m_eChangeType )	/// 일단 무조건 배율
			{
				/*if ( DidStart() )
				{*/
					m_fResultValue = (*pBehaviorFactor)[FO_VALUE];
					m_eAttackType = STATIC_CAST_FLOAT_TO_ENUM( CX2DamageManager::ATTACK_TYPE, (*pBehaviorFactor)[FO_ATTACK_TYPE] );

					pGameUnit_->CreateAndInsertChangeAttackByType( pBuffTemplet_->GetBuffIdentity(), m_eAttackType, m_fResultValue );

					return true;
				//}
			}
			else
				return DISPLAY_ERROR( L"ONLY BCT_PERCENT" );
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeAttackByTypeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeAttackByTypeBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eAttackType ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( kFactor_.m_vecValues.size() >= static_cast<UINT>( FO_END ) )
	{
		m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
		m_fResultValue	= kFactor_.m_vecValues[FO_VALUE];
		m_eAttackType	= STATIC_CAST_FLOAT_TO_ENUM( CX2DamageManager::ATTACK_TYPE, kFactor_.m_vecValues[FO_ATTACK_TYPE] );

		pGameUnit_->CreateAndInsertChangeAttackByType( kBuffFactor_.m_BuffIdentity, m_eAttackType, m_fResultValue );
	}
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeAttackByTypeBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseChangeAttackByType( pBuffTemplet_->GetBuffIdentity() );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : 소켓 속성 공격 발동 확률을 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeEncahntAttackRateBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	// 버프 타입에 대해 지정 가능하도록 추가
	LUA_GET_VALUE_ENUM ( luaManager_, "ENCHANT_ATTRIBUTE_TYPE", m_eEnchantAttributeType, 
		BUFF_ENCHANT_ATTRIBUTE_TYPE, BEAT_ALL );
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e77370. The attribute per type is March's (table 32e775c4). When the buff has already started, June
/// restacks instead of inserting a second entry: with a stack count it erases (32279c70) and re-inserts at
/// VALUE * ( 1 + multiplier * count ), a zero multiplier failing; without one it keeps the larger absolute
/// value. No resist is applied, and June raises no popups.
/*virtual*/ bool CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	switch ( GetType() )
	{
	case BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_BLAZE;	break;
	case BBT_CHANGE_ATTRIBUTE_WATER_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_FROZEN;	break;
	case BBT_CHANGE_ATTRIBUTE_NATURE_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_POSION;	break;
	case BBT_CHANGE_ATTRIBUTE_WIND_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_PIERCING;	break;
	case BBT_CHANGE_ATTRIBUTE_LIGHT_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_SHOCK;	break;
	case BBT_CHANGE_ATTRIBUTE_DARK_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_SNATCH;	break;
	default:	break;
	}

	if ( BCT_PERCENT != m_eChangeType && BCT_FIX_VALUE != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE is neither FIX nor PERCENT" )	/// 4b-ii: June returns false here silently; logged once

	const float fNewRate = (*pBehaviorFactor)[FO_VALUE];
	if ( !DidStart() )
	{
		m_fRate = fNewRate;
		pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
		return true;
	}

	const UINT uiAccumulationCountNow = pBuffTemplet_->GetAccumulationCountNow();
	if ( 0 < uiAccumulationCountNow )
	{
		const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
		if ( IsSamef( fAccumulationMultiflier ) )
			return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor: ACCUMULATION_MULTIPLIER is 0" )	/// 4b-ii: June returns false here silently; logged once

		pGameUnit_->EraseChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_eEnchantAttributeType, m_eChangeType );
		m_fRate = fNewRate * ( 1.0f + fAccumulationMultiflier * uiAccumulationCountNow );
		pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
	}
	else if ( abs( fNewRate ) > abs( m_fRate ) )
	{
		pGameUnit_->EraseChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_eEnchantAttributeType, m_eChangeType );
		m_fRate = fNewRate;
		pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
	}
	return true;
}

/** @function : OnAccumulationCountSet
	@brief : June 32e775e0 (behavior slot 3). The change type must match; the attribute is set per type as
			 in SetFactor, and only FIX and PERCENT go on. Not started: insert v[1]. Started with a stack
			 count: a zero multiplier returns false, else erase and insert v[1] * (1 + multiplier * (count - 1)).
			 Started with no count: erase and insert v[1] (no larger-absolute pick, unlike SetFactor).
*/
/*virtual*/ bool CX2BuffChangeEncahntAttackRateBehaviorTemplet::OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::OnAccumulationCountSet: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::OnAccumulationCountSet: BUFF_CHANGE_TYPE MISS MATCH" )	/// 4b-ii: June returns false here silently; logged once

	switch ( GetType() )
	{
	case BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_BLAZE;	break;
	case BBT_CHANGE_ATTRIBUTE_WATER_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_FROZEN;	break;
	case BBT_CHANGE_ATTRIBUTE_NATURE_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_POSION;	break;
	case BBT_CHANGE_ATTRIBUTE_WIND_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_PIERCING;	break;
	case BBT_CHANGE_ATTRIBUTE_LIGHT_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_SHOCK;	break;
	case BBT_CHANGE_ATTRIBUTE_DARK_ATTACK_RATE:	m_eEnchantAttributeType = BEAT_SNATCH;	break;
	default:	break;
	}

	if ( BCT_PERCENT != m_eChangeType && BCT_FIX_VALUE != m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::OnAccumulationCountSet: BUFF_CHANGE_TYPE is neither FIX nor PERCENT" )	/// 4b-ii: June returns false here silently; logged once

	const float fNewRate = (*pBehaviorFactor)[FO_VALUE];
	if ( DidStart() )
	{
		if ( 0 < pBuffTemplet_->GetAccumulationCountNow() )
		{
			const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
			if ( IsSamef( fAccumulationMultiflier ) )
				return DISPLAY_ERROR( L"CX2BuffChangeEncahntAttackRateBehaviorTemplet::OnAccumulationCountSet: ACCUMULATION_MULTIPLIER is 0" )	/// 4b-ii: June returns false here silently; logged once

			pGameUnit_->EraseChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_eEnchantAttributeType, m_eChangeType );
			m_fRate = ( fAccumulationMultiflier * static_cast<float>( iCount_ - 1 ) + 1.0f ) * fNewRate;
		}
		else
		{
			pGameUnit_->EraseChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_eEnchantAttributeType, m_eChangeType );
			m_fRate = fNewRate;
		}
	}
	else
		m_fRate = fNewRate;

	pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
	return true;
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ bool CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;


	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
			// BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE 등 특정 속성만 강화 시키는 BBT 는
			// m_eEnchantAttributeType 값만 수정해서 넣는다.
			// 후에 확률 계산 때 해당 속성에 대해서만 체크하게 됨
			switch ( GetType() )
			{
			case BBT_CHANGE_ATTRIBUTE_BLAZE_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_BLAZE;
				break;
			case BBT_CHANGE_ATTRIBUTE_WATER_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_FROZEN;
				break;
			case BBT_CHANGE_ATTRIBUTE_NATURE_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_POSION;
				break;
			case BBT_CHANGE_ATTRIBUTE_LIGHT_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_SHOCK;
				break;
			case BBT_CHANGE_ATTRIBUTE_DARK_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_SNATCH;
				break;
			case BBT_CHANGE_ATTRIBUTE_WIND_ATTACK_RATE :
				m_eEnchantAttributeType = BEAT_PIERCING;			
				break;
			}
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편


			if ( BCT_PERCENT == m_eChangeType ) /// 일단 무조건 배율
			{
				/*if ( DidStart() )
				{*/
					m_fRate = (*pBehaviorFactor)[FO_VALUE];
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
					// 공격 타입 및 변경 조건에 대해 추가
					pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
#else // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편					
					pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate );
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편					

					return true;
				//}
			}
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
			// FIX_VALUE 까지 확장, 인챈트 속성 발동 확률이 0 이 넘어야지 적용된다.
			else if ( BCT_FIX_VALUE == m_eChangeType )		
			{
				m_fRate = (*pBehaviorFactor)[FO_VALUE];
				// 공격 타입 및 변경 조건에 대해 추가
				pGameUnit_->CreateAndInsertChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity(), m_fRate, m_eEnchantAttributeType, m_eChangeType );
				return true;
			}
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
			else
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
				return DISPLAY_ERROR( L"ONLY BCT_PERCENT OR BCT_FIX_VALUE" );
#else // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
				return DISPLAY_ERROR( L"ONLY BCT_PERCENT" );
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet GetBehaviorFactor didn't work" );
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeEncahntAttackRateBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeEncahntAttackRateBehaviorTemplet( *this ) );

}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeEncahntAttackRateBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );

	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fRate );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/// June 32e74b50: needs [change type, rate]. It erases by identity, attribute and change type, then inserts with
/// the templet's attribute (no per-type override on this path) and the change type. March's packet path used
/// the 2-argument insert and lost both.
/*virtual*/ void CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( kFactor_.m_vecValues.size() < 2 )
		return;

	m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
	m_fRate = kFactor_.m_vecValues[FO_VALUE];

	pGameUnit_->EraseChangeEnchantAttackRate( kBuffFactor_.m_BuffIdentity, m_eEnchantAttributeType, m_eChangeType );
	pGameUnit_->CreateAndInsertChangeEnchantAttackRate( kBuffFactor_.m_BuffIdentity, m_fRate, m_eEnchantAttributeType, m_eChangeType );
}
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
/*virtual*/ void CX2BuffChangeEncahntAttackRateBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( kFactor_.m_vecValues.size() >= static_cast<UINT>( FO_END ) )
	{
		m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
		m_fRate = kFactor_.m_vecValues[FO_VALUE];

		pGameUnit_->CreateAndInsertChangeEnchantAttackRate( kBuffFactor_.m_BuffIdentity, m_fRate );
	}
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeEncahntAttackRateBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseChangeEnchantAttackRate( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : 소켓 속성 공격 발동 확률을 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffDoubleAttackBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffDoubleAttackBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
			if ( BCT_PERCENT == m_eChangeType ) /// 일단 무조건 배율
			{
				/*if ( DidStart() )
				{*/
					m_fRate = (*pBehaviorFactor)[FO_VALUE];

					pGameUnit_->CreateAndInsertDoubleAttackByBuff( pBuffTemplet_->GetBuffIdentity(), m_fRate );

					return true;
				//}
			}
			else
				return DISPLAY_ERROR( L"ONLY BCT_PERCENT" );
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffDoubleAttackBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffDoubleAttackBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffDoubleAttackBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );

	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fRate );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffDoubleAttackBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( kFactor_.m_vecValues.size() >= static_cast<UINT>( FO_END ) )
	{
		m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
		m_fRate = kFactor_.m_vecValues[FO_VALUE];
		
		pGameUnit_->CreateAndInsertDoubleAttackByBuff( kBuffFactor_.m_BuffIdentity, m_fRate );
	}
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffDoubleAttackBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseDoubleAttackByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffChangeAttackSphereScaleBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( !DidStart() )
		{
			BOOST_FOREACH( const AttackSphereNameAndScale& attackSphereNameAndScale, m_vecAttackSphereNameAndScale )
			{
				float fScale = pGameUnit_->GetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName );
				fScale += attackSphereNameAndScale.m_fScale;

				if ( fScale < 0.0f )	/// fScale이 마이너스면 정상이 아니라고 판단하여
					fScale = 1.0f;		/// 1.0 (기본값)으로 셋팅

				pGameUnit_->SetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName, fScale );
			}
		}

		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeUnitScaleBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : ParsingBehavior
	@brief : 어택박스 크기를 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeAttackSphereScaleBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	int iIndex = 1;
	while ( luaManager_.BeginTable( iIndex ) )
	{
		AttackSphereNameAndScale attackSphereNameAndScale;

		LUA_GET_VALUE( luaManager_, "ATTACK_SPHERE_NAME", attackSphereNameAndScale.m_wstrAttackSphereName, L"" );
		LUA_GET_VALUE( luaManager_, "SCALE", attackSphereNameAndScale.m_fScale, 1.0f );

		m_vecAttackSphereNameAndScale.push_back( attackSphereNameAndScale );
		++iIndex;

        luaManager_.EndTable();
	}

	return true;
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeAttackSphereScaleBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeAttackSphereScaleBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeAttackSphereScaleBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeAttackSphereScaleBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	/// 오작동 할 것으로 예상 됨... // 김현철 // 2013-07-18
	if ( !DidStart() )
	{
		BOOST_FOREACH( const AttackSphereNameAndScale& attackSphereNameAndScale, m_vecAttackSphereNameAndScale )
		{
			float fScale = pGameUnit_->GetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName );
			fScale += attackSphereNameAndScale.m_fScale;

			if ( fScale < 0.0f )	/// fScale이 마이너스면 정상이 아니라고 판단하여
				fScale = 1.0f;		/// 1.0 (기본값)으로 셋팅

			pGameUnit_->SetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName, fScale );
		}
	}
	
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeAttackSphereScaleBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	BOOST_FOREACH( const AttackSphereNameAndScale& attackSphereNameAndScale, m_vecAttackSphereNameAndScale )
	{
		float fScale = pGameUnit_->GetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName );
		fScale -= attackSphereNameAndScale.m_fScale;

		if ( fScale < 0.0f )	/// fScale이 마이너스면 정상이 아니라고 판단하여
			fScale = 1.0f;		/// 1.0 (기본값)으로 셋팅

		pGameUnit_->SetSphereAttackBoxScale( attackSphereNameAndScale.m_wstrAttackSphereName, fScale );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffAddSkillLevelBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( !DidStart() )	/// 일단 중첩등 허용 안함
		{
			m_iAddSkillLevel = static_cast<int>( (*pBehaviorFactor)[FO_SKILL_LEVEL] );
			pGameUnit_->CreateAndInsertAddSkillLevelByBuff( pBuffTemplet_->GetBuffIdentity(), m_iAddSkillLevel );
			pGameUnit_->UpdatePassiveAndActiveSkillState();
		}

		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffAddSkillLevelBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffAddSkillLevelBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffAddSkillLevelBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffAddSkillLevelBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_iAddSkillLevel ) );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffAddSkillLevelBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_iAddSkillLevel = static_cast<int>( kFactor_.m_vecValues[FO_SKILL_LEVEL] );
	pGameUnit_->CreateAndInsertAddSkillLevelByBuff( kBuffFactor_.m_BuffIdentity, m_iAddSkillLevel );
	pGameUnit_->UpdatePassiveAndActiveSkillState();
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffAddSkillLevelBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseAddSkillLevelByBuff( pBuffTemplet_->GetBuffIdentity() );
	pGameUnit_->UpdatePassiveAndActiveSkillState();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환

/** @function : ParsingBehavior
	@brief : 타입에 따른 공격력 수치를 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffReflectMagicBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffReflectMagicBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) == m_eChangeType )
		{
			if ( BCT_PERCENT == m_eChangeType )	/// 무조건 배율
			{
				if ( DidStart() )	/// 기존에 작동 중인 것이었으면 기존의 효과 삭제
					pGameUnit_->EraseReflectMagicByBuff( pBuffTemplet_->GetBuffIdentity() );

				/// 효과 설정
				m_fResultValue = (*pBehaviorFactor)[FO_VALUE];

				pGameUnit_->CreateAndInsertReflectMagicByBuff( pBuffTemplet_->GetBuffIdentity(), m_fResultValue );

				return true;
			}
			else
				return DISPLAY_ERROR( L"ONLY BCT_PERCENT" );
		}
		else
			return DISPLAY_ERROR( L"MISS MATCH BUFF_CHANGE_TYPE" );
	}
	else
		return DISPLAY_ERROR( L"CX2BuffChangeStatBehaviorTemplet GetBehaviorFactor didn't work" );
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffReflectMagicBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffReflectMagicBehaviorTemplet( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffReflectMagicBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 스탯을 변경하는 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffReflectMagicBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
	m_fResultValue = kFactor_.m_vecValues[FO_VALUE];

	pGameUnit_->CreateAndInsertReflectMagicByBuff( kBuffFactor_.m_BuffIdentity, m_fResultValue );
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffReflectMagicBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseReflectMagicByBuff( pBuffTemplet_->GetBuffIdentity() );
}

#endif // SERV_ARA_CHANGE_CLASS_SECOND

#ifdef FIX_SKILL_BALANCE_AISHA_LENA //JHKang
/** @function : ParsingBehavior
	@brief : 타입에 따른 공격력 수치를 변경 시키는 버프 행동을 정의한 템플릿을 루아로 부터 읽어들이는 함수
	@param : 읽어들이려는 스크립트의 루아매니저(luaManager_)
	@return : 파싱 성공시 true, 실패시 false 리턴
*/
/*virtual*/ bool CX2BuffChangeConsumeMpRate::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, 
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ) );

	return true;
}

/** @function : SetFactor
	@brief : BuffFactor에 담겨진 값으로 Templet 셋팅
	@param : 버프요소(BuffFactor_), 버프를 가지고 있을 게임유닛(pGameUnit_), 기존에 가지고 있던 또는 새로운 버프템플릿 포인터(pBuffTemplet_)
*/
/*virtual*/ bool CX2BuffChangeConsumeMpRate::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// June 32e7cef0: the value is a rate, 1 + v (restack: += v), for users only. June looks the skill level up and
	/// ignores it; it has no MP lookup and no change-type test. The reader is CX2GUUser::GetSkillRateByBuffBehavior.
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( false == BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeConsumeMpRate::SetFactor: GetBehaviorFactor didn't work" )	/// 4b-ii: June returns false here silently; logged once

	m_uiSkillId = static_cast<UINT>( (*pBehaviorFactor)[FO_SKILL_ID] );

	if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );

		if ( NULL == pUser->GetUnit() )
			return DISPLAY_ERROR( L"CX2BuffChangeConsumeMpRate::SetFactor: the user has no CX2Unit" )	/// 4b-ii: June returns false here silently; logged once

		if ( DidStart() )
			m_fResultValue += (*pBehaviorFactor)[FO_VALUE];
		else
			m_fResultValue = (*pBehaviorFactor)[FO_VALUE] + 1.f;
	}

	return true;
#else
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;

	/// 버프가 가지고 있는 여러 요소 중 현재 클래스에 해당 하는 정보를 얻어옴
	if ( BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
	{
		float fMPConsume = 0.f;
		m_uiSkillId = static_cast<UINT>( (*pBehaviorFactor)[FO_SKILL_ID] );
		
		if ( 0 > m_uiSkillId )
			return DISPLAY_ERROR( L"SKILL_ID doesn't exist" );

		if ( pGameUnit_->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			CX2GUUser* pUser = reinterpret_cast<CX2GUUser*>( pGameUnit_ );

			if ( NULL == pUser->GetUnit() )
				return false;

			const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SKILL_ID( m_uiSkillId ) ) );
						
			if ( DidStart() )
			{
				m_fResultValue = m_fResultValue + m_fInitMp * (*pBehaviorFactor)[FO_VALUE];
			}
			else
			{	/// 중첩, 교체가 아니라면(첫시작 또는 중복)
				m_fInitMp = pUser->GetActualMPConsume( CX2SkillTree::SKILL_ID( m_uiSkillId ), iSkillTempletLevel );
				m_fResultValue = m_fInitMp + m_fInitMp * (*pBehaviorFactor)[FO_VALUE];
			}
		}

		return true;
	}
	else
		return DISPLAY_ERROR( L"CX2BuffCreateBuffFactortBehaviorTemplet GetBehaviorFactor didn't work" );
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
}

/** @function : GetClonePtr
	@return : CX2BuffBehaviorTempletPtr
*/
/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeConsumeMpRate::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeConsumeMpRate( *this ) );
}

/** @function : GetFactor
	@brief : 템플릿 인자들 중 필요 한 것들을 factor로 변환
	@param : 버프요소 인스턴스(factor_)
*/
/*virtual*/ void CX2BuffChangeConsumeMpRate::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();
	
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_uiSkillId ) );
	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @function : SetFactorFromPacket
	@brief : 패킷으로 부터 받은 버프 정보로 다른 유닛에게 지정된 버프 또는 디버프를 적용할 수 있는 행동 템플릿을 셋팅
	@param : 서버로 부터 받은 행동요소(kFactor_), 적용할 게임유닛(pGameUnit_)
*/
/*virtual*/ void CX2BuffChangeConsumeMpRate::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_eChangeType	= STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, kFactor_.m_vecValues[FO_CHANGE_TYPE] );
	m_uiSkillId		= static_cast<UINT>( kFactor_[FO_SKILL_ID] );
	m_fResultValue	= kFactor_.m_vecValues[FO_VALUE];
}

/** @function : DoFinish
	@brief : 버프가 종료됬을 때 정리해야하는 행동들을 처리
	@param : @param : 해당 버프에 걸렸던 게임유닛(pGameUnit_), 버프템플릿(pBuffTemplet_)
*/
/*virtual*/ void CX2BuffChangeConsumeMpRate::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseTemporaryBuffFactor( pBuffTemplet_->GetBuffIdentity() );
}
#endif //FIX_SKILL_BALANCE_AISHA_LENA

//{{ Iruha : 2026-10-04 // June buff port Phase 5b (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 79: CX2BuffChangeNowForceDownPerSecondBehaviorTemplet (June create 32e86e90)
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : June 32e755a0. TIME_INTERVAL is required: June returns false when it is absent (AL checked at both rets).
*/
/*virtual*/ bool CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	float fTimeInterval = 0.0f;
	LUA_GET_VALUE_RETURN( luaManager_, "TIME_INTERVAL", fTimeInterval, 0.0f, return DISPLAY_ERROR( L"TIME_INTERVAL" ); );
	m_CheckElapsedTimeForInterval.SetTargetTime( fTimeInterval );
	return true;
}

/** @function : OnFrameMove
	@brief : June 32e756f0 and its tick 32e89800: the timer runs, and when the sum has reached the target it is cleared and the
		force-down gage rises by the value (gage data of the unit +0xc04, Gage::Increase( v, 0 ) @31f8de20). TIME_INTERVAL 0 therefore
		ticks every frame, and the value is applied per tick, not per second (June's data note; transcribed as is).
		June does not test for missing gage data; the port does, as a crash guard.
*/
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
/*virtual*/ void CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
{
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove( fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	m_CheckElapsedTimeForInterval.OnFrameMove();
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

	if ( m_CheckElapsedTimeForInterval.CheckAndResetElapsedTime() )
	{
		CX2GageData* pGageData = pGameUnit_->GetGageData();
		if ( NULL != pGageData )
		{
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
			static bool s_bSeenTick = false;
			if ( !s_bSeenTick )
			{
				s_bSeenTick = true;
				CX2BuffCapture::Line( "first read BBT 79: force down gage %g + %g (interval %g)", pGageData->GetNowForceDown(), m_fResultValue, m_CheckElapsedTimeForInterval.GetTargetTime() );
			}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
			pGageData->UpNowForceDown( m_fResultValue );
		}
	}
}

/** @function : SetFactor
	@brief : June 32e855f0: March's CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor (the multiplier restack, no BCT_PERCENT branch) with the
		resist fixed at 0 and no popups. The change type must equal BCT_FIX_VALUE; the new value is v[1] (0 when the factor has one value only).
*/
/*virtual*/ bool CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" );

	if ( static_cast<int>( (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != static_cast<int>( m_eChangeType ) )
		return DISPLAY_ERROR( L"CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::SetFactor: BUFF_CHANGE_TYPE MISS MATCH" );

	const float fNewValue = ( 1 < pBehaviorFactor->m_vecValues.size() ? (*pBehaviorFactor)[FO_VALUE] : 0.0f );

	if ( DidStart() )
	{
		const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
		if ( uiAccumulationCountBefore > 0 )
		{
			const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
			const float fOldValue = m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) );
			m_fResultValue = ( abs(fNewValue) > abs(fOldValue) ? fNewValue : fOldValue );
			m_fResultValue *= ( 1 + fAccumulationMultiflier * uiAccumulationCountBefore );
		}
		else
			m_fResultValue = ( abs(fNewValue) > abs(m_fResultValue) ? fNewValue : m_fResultValue );
	}
	else
		m_fResultValue = fNewValue;

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	{
		static bool s_bSeenApply = false;
		if ( !s_bSeenApply )
		{
			s_bSeenApply = true;
			CX2BuffCapture::Line( "first apply BBT 79 (force down per second): interval %g, value %g", m_CheckElapsedTimeForInterval.GetTargetTime(), m_fResultValue );
		}
	}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowForceDownPerSecondBehaviorTemplet( *this ) );
}

/** @brief : June 32e7eaa0: [ m ] */
/*virtual*/ void CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @brief : June 32e85790: m = v[0] (0 when the factor is empty); nothing else (no unit store). */
/*virtual*/ void CX2BuffChangeNowForceDownPerSecondBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fResultValue = kFactor_[0];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 80-84: CX2BuffChangeStateByConditionBehaviorTemplet (June create 32e86c00)
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : June 32e75790. BUFF_CHANGE_TYPE and BUFF_VALUE_CALCULATE_TYPE are both required (AL checked).
*/
/*virtual*/ bool CX2BuffChangeStateByConditionBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType,
		BUFF_CHANGE_TYPE, BCT_SWAP_VALUE, return DISPLAY_ERROR( L"BUFF_CHANGE_TYPE" ); );

	LUA_GET_VALUE_RETURN_ENUM( luaManager_, "BUFF_VALUE_CALCULATE_TYPE", m_eValueCalculateType,
		BUFF_VALUE_CALCULATE_TYPE, BVC_RALLY_OF_HERO, return DISPLAY_ERROR( L"BUFF_VALUE_CALCULATE_TYPE" ); );

	return true;
}

/** @function : GetValue
	@brief : June 32e77f40 (unit and resist unused). With BVC_RALLY_OF_HERO (the class's calculate type, +0x14 == 0):
		v[3] + v[4] * ( 4.0 - v[5] ), where v[5] is the user count the SGU appended; a missing element reads 0. Any other
		calculate type gives 0. It is exact only for a two-element VALUE table, which all 25 June factors carry.
*/
float CX2BuffChangeStateByConditionBehaviorTemplet::GetValue( const KBuffBehaviorFactor* pBehaviorFactor_ ) const
{
	if ( BVC_RALLY_OF_HERO != m_eValueCalculateType )
		return 0.0f;

	return (*pBehaviorFactor_)[FO_VALUE_0] + (*pBehaviorFactor_)[FO_VALUE_1] * ( 4.0f - (*pBehaviorFactor_)[FO_USER_NUM] );
}

/** @function : SetFactor
	@brief : June 32e78860: March's CX2BuffChangeStatBehaviorTemplet::SetFactor, the ADJUST_BUFF_ACCMULATION_CALCULATE branch, with three
		differences: (1) a change type that differs from the class's, or BCT_RELATION_VALUE, returns false (June has no relation path),
		(2) the value comes from GetValue above, (3) no popups: a zero accumulation multiplier returns false. The resist
		( GetEnchantResist / 500 ) is computed by June and never used. The stat is written to the store of the type
		(80/81/82/83/84 -> the stores of 7/8/9/10/6, see CX2GameUnit::InsertAdditionalStat).
*/
/*virtual*/ bool CX2BuffChangeStateByConditionBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return DISPLAY_ERROR( L"CX2BuffChangeStateByConditionBehaviorTemplet::SetFactor: GetBehaviorFactor didn't work" );

	if ( STATIC_CAST_FLOAT_TO_ENUM( BUFF_CHANGE_TYPE, (*pBehaviorFactor)[FO_CHANGE_TYPE] ) != m_eChangeType || BCT_RELATION_VALUE == m_eChangeType )
		return DISPLAY_ERROR( L"CX2BuffChangeStateByConditionBehaviorTemplet::SetFactor: MISS MATCH BUFF_CHANGE_TYPE" );

	const float fNewValue = GetValue( pBehaviorFactor );
	if ( DidStart() )	/// stack or replace
	{
		const UINT uiAccumulationCountBefore = pBuffTemplet_->GetAccumulationCountNow();
		if ( uiAccumulationCountBefore > 0 )
		{
			const float fAccumulationMultiflier = pBuffTemplet_->GetAccumulationMultiflier();
			if ( IsSamef( fAccumulationMultiflier ) )
				return DISPLAY_ERROR( L"CX2BuffChangeStateByConditionBehaviorTemplet::SetFactor: AccumulationMultiflier must not be 0" );

			switch ( m_eChangeType )
			{
			case BCT_FIX_VALUE:
				{
					const float fOldValue =
						(uiAccumulationCountBefore <= 1) ? m_fResultValue : ( m_fResultValue / ( 1 + fAccumulationMultiflier * (uiAccumulationCountBefore - 1) ) );

					m_fResultValue = ( abs(fNewValue) > abs(fOldValue) ? fNewValue : fOldValue );
					m_fResultValue += m_fResultValue * fAccumulationMultiflier * uiAccumulationCountBefore;
				} break;

			case BCT_PERCENT:
				{
					m_fResultValue = 1 + ( ( fNewValue - 1.0f ) * ( uiAccumulationCountBefore + 1 ) );
					if( m_fResultValue < 0.0f )
					{
						DISPLAY_ERROR( L"CX2BuffChangeStateByConditionBehaviorTemplet::SetFactor: m_fResultValue error" );
						m_fResultValue = max( m_fResultValue, 0.0f );
					}
				} break;

			default:	/// a replace keeps the value for the other change types
				break;
			}
		}
		else	/// replace
			m_fResultValue = ( abs(fNewValue) > abs(m_fResultValue) ? fNewValue : m_fResultValue );

		pGameUnit_->UpdateAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
	}
	else	/// first apply, or a duplicate
	{
		m_fResultValue = fNewValue;
		pGameUnit_->InsertAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity(), m_fResultValue );
	}

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	{
		static std::set<int> s_setSeen;
		if ( s_setSeen.insert( static_cast<int>( GetType() ) ).second )
			CX2BuffCapture::Line( "first SetFactor BBT %d (by condition): change type %d, calculate type %d, v[3..5] = %g %g %g, value %g",
				static_cast<int>( GetType() ), static_cast<int>( m_eChangeType ), static_cast<int>( m_eValueCalculateType ),
				(*pBehaviorFactor)[FO_VALUE_0], (*pBehaviorFactor)[FO_VALUE_1], (*pBehaviorFactor)[FO_USER_NUM], m_fResultValue );
	}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeStateByConditionBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStateByConditionBehaviorTemplet( *this ) );
}

/** @brief : June 32e7eb90: [ m ] */
/*virtual*/ void CX2BuffChangeStateByConditionBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fResultValue );
}

/** @brief : June 32e77ef0: m = v[0], then InsertAdditionalStat( type, change type, identity, m ) */
/*virtual*/ void CX2BuffChangeStateByConditionBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fResultValue = kFactor_[0];
	pGameUnit_->InsertAdditionalStat( GetType(), m_eChangeType, kBuffFactor_.m_BuffIdentity, m_fResultValue );
}

/** @brief : June 32e75720: EraseAdditionalStat( type, change type, identity ) */
/*virtual*/ void CX2BuffChangeStateByConditionBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseAdditionalStat( GetType(), m_eChangeType, pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 93: CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner (June create 32e86740)
//////////////////////////////////////////////////////////////////////////////////////////////////////

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner( *this ) );
}

/** @function : GetFactor
	@brief : June 32e7d7f0: [ m, uid & 0xffff, ( uid >> 16 ) & 0xffff, ( uid >> 32 ) & 0xffff, uid >> 48 ], ChangeStat's [ m ] first.
*/
/*virtual*/ void CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	CX2BuffChangeStatBehaviorTemplet::GetFactor( vecFactors_ );

	vector<float>& vecValues = vecFactors_.back().m_vecValues;
	const unsigned __int64 ui64Owner = static_cast<unsigned __int64>( m_uidBuffOwner );
	vecValues.push_back( static_cast<float>( ui64Owner & 0xffff ) );
	vecValues.push_back( static_cast<float>( ( ui64Owner >> 16 ) & 0xffff ) );
	vecValues.push_back( static_cast<float>( ( ui64Owner >> 32 ) & 0xffff ) );
	vecValues.push_back( static_cast<float>( ui64Owner >> 48 ) );
}

/** @function : SetFactorFromPacket
	@brief : June 32e76710: ChangeStat's insert ( m = v[0] ), then the owner UID is rebuilt from v[4], v[5], v[6] and v[7] (low word first).
		JUNE INDEX BUG, TRANSCRIBED VERBATIM: GetFactor above writes the UID words at indices 1-4, this reads 4-7, so a peer ends up with a
		wrong owner (v[4] is the top word, v[5..7] are past the end of a five-value factor and read as 0 here; June reads past the end).
		Harmless: the UID's only consumer is June's owner aggregator 3227c620, which nothing calls.
		Phase 5c correction (5b review D1): June does not read past the end either. 32e76710 size-tests each of v[4..7] and reads
		0.0 for a missing one, exactly like KBuffBehaviorFactor::operator[] here, so a peer gets owner = uid >> 48 (the top word).
*/
/*virtual*/ void CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	CX2BuffChangeStatBehaviorTemplet::SetFactorFromPacket( kBuffFactor_, kFactor_, pGameUnit_ );

	m_uidBuffOwner = static_cast<UidType>( static_cast<int>( kFactor_[4] ) )
		| ( static_cast<UidType>( static_cast<int>( kFactor_[5] ) ) << 16 )
		| ( static_cast<UidType>( static_cast<int>( kFactor_[6] ) ) << 32 )
		| ( static_cast<UidType>( static_cast<int>( kFactor_[7] ) ) << 48 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 88: CX2BuffIgnoreDefenceBehaviorTemplet (June vt 33535558, ctor 32e890b0), Phase 5c
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : June 32e7d260. GetBehaviorFactor failing returns false (no popup in June). On the first apply only, the rate is v[0]
		(0.0 when the factor is empty) and the unit gets one ignore-defence entry for this buff (June 32282390). A restack changes
		nothing and still returns true.
*/
/*virtual*/ bool CX2BuffIgnoreDefenceBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	if ( !DidStart() )
	{
		m_fIgnoreDefenceRate = ( pBehaviorFactor->m_vecValues.empty() ? 0.0f : (*pBehaviorFactor)[0] );
		pGameUnit_->InsertIgnoreDefence( pBuffTemplet_->GetBuffIdentity(), m_fIgnoreDefenceRate );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		static bool s_bSeenApply = false;
		if ( !s_bSeenApply )
		{
			s_bSeenApply = true;
			CX2BuffCapture::Line( "first apply BBT 88 (ignore defence): BTI %d, rate %g", static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<float>( m_fIgnoreDefenceRate ) );
		}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	}
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffIgnoreDefenceBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffIgnoreDefenceBehaviorTemplet( *this ) );
}

/** @brief : June 32e7ec40: [ rate ] */
/*virtual*/ void CX2BuffIgnoreDefenceBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_fIgnoreDefenceRate );
}

/** @brief : June 32e7d3f0: rate = v[0] (0.0 when empty), then the insert, with no first-apply test. */
/*virtual*/ void CX2BuffIgnoreDefenceBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_fIgnoreDefenceRate = ( kFactor_.m_vecValues.empty() ? 0.0f : kFactor_[0] );
	pGameUnit_->InsertIgnoreDefence( kBuffFactor_.m_BuffIdentity, m_fIgnoreDefenceRate );
}

/** @brief : June 32e75b00: EraseIgnoreDefence( identity ) (June 3227a8c0) */
/*virtual*/ void CX2BuffIgnoreDefenceBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseIgnoreDefence( pBuffTemplet_->GetBuffIdentity() );
}
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
//}}

//{{ Iruha : 2026-10-04 // June buff port Phase 6 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL

/// June gates 89, 90, 91, 98 (erase), 99, 100 and 103 on the unit type (+0x264 == 0 is GUT_USER) and then calls CX2GUUser members.
static CX2GUUser* BuffNewSkillGetUser( CX2GameUnit* pGameUnit_ )
{
	if ( NULL != pGameUnit_ && CX2GameUnit::GUT_USER == pGameUnit_->GetGameUnitType() )
		return static_cast<CX2GUUser*>( pGameUnit_ );

	return NULL;
}

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
/// One "[JUNE_BUFF] first apply" line per type (step 8 of the shared checklist).
static bool BuffNewSkillFirstApply( const int iBehaviorType_ )
{
	static std::set<int> s_setSeen;
	return s_setSeen.insert( iBehaviorType_ ).second;
}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG

/// The { rate, skill IDs... } blocks of BBT 90 and 91 out of their factor [ rate, id1..idn, -1.0 ]*. A value of exactly -1.0 (June 334380f0)
/// closes a block, the first value after a close is the rate and every later one is a skill ID; a trailing block without its -1.0 is dropped.
/// June never clears its skill-ID vector (or its rate) between blocks (SetFactor 32e849c0 / 32e84d70, packet 32e84b90 / 32e84f40: the vector
/// is zeroed once before the loop and the block is COPIED into the member at each -1.0), so block k lists the IDs of blocks 1..k, and with
/// the getter's last-match rule every listed ID gets the last block's rate. Reproduced as June does it (Phase 6 review); June's own 90 / 91
/// data has one block per factor, where nothing differs.
static void BuffReadSkillIDRateBlocks( const KBuffBehaviorFactor& kFactor_, OUT vector<CX2SkillIDRateByBuff::SkillIDRateBlock>& vecBlock_ )
{
	vecBlock_.clear();

	CX2SkillIDRateByBuff::SkillIDRateBlock kBlock;
	bool bExpectRate = true;
	for ( vector<float>::const_iterator vItr = kFactor_.m_vecValues.begin(); kFactor_.m_vecValues.end() != vItr; ++vItr )
	{
		if ( -1.0f == *vItr )
		{
			vecBlock_.push_back( kBlock );
			/// no reset of kBlock here: June keeps the IDs (Phase 6 review, see above)
			bExpectRate = true;
		}
		else if ( bExpectRate )
		{
			kBlock.m_fRate = *vItr;
			bExpectRate = false;
		}
		else
			kBlock.m_vecSkillID.push_back( static_cast<int>( *vItr ) );
	}
}

/// [ rate, id1..idn, -1.0 ] per block (GetFactor of 90 and 91, June 32e7ef50 / 32e7f2e0)
static void BuffWriteSkillIDRateBlocks( const vector<CX2SkillIDRateByBuff::SkillIDRateBlock>& vecBlock_, OUT KBuffBehaviorFactor& kFactor_ )
{
	for ( vector<CX2SkillIDRateByBuff::SkillIDRateBlock>::const_iterator vItr = vecBlock_.begin(); vecBlock_.end() != vItr; ++vItr )
	{
		kFactor_.m_vecValues.push_back( vItr->m_fRate );
		for ( vector<int>::const_iterator vIdItr = vItr->m_vecSkillID.begin(); vItr->m_vecSkillID.end() != vIdItr; ++vIdItr )
			kFactor_.m_vecValues.push_back( static_cast<float>( *vIdItr ) );
		kFactor_.m_vecValues.push_back( -1.0f );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 75: CX2BuffDoubleAttackBuffBehaviorTemplet (June vt 33535070), Phase 6
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : ParsingBehavior
	@brief : June 32e75360. BUFF_CHANGE_TYPE, 0 (BCT_SWAP_VALUE) when absent. March's BBT 56 class fails the templet instead.
*/
/*virtual*/ bool CX2BuffDoubleAttackBuffBehaviorTemplet::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE_ENUM( luaManager_, "BUFF_CHANGE_TYPE", m_eChangeType, BUFF_CHANGE_TYPE, BCT_SWAP_VALUE );
	return true;
}

/** @function : SetFactor
	@brief : June 32e77be0. No factor: false. The change type must match the templet's and be BCT_PERCENT, else false with no popup.
		rate = v[1] (0.0 when absent). A restack on a started behavior erases this buff's entry first, then the entry is inserted
		(June 32281b00 on the unit's own BBT 75 list). March's 56 class inserts again without erasing.
*/
/*virtual*/ bool CX2BuffDoubleAttackBuffBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	const BUFF_CHANGE_TYPE eChangeType = static_cast<BUFF_CHANGE_TYPE>( static_cast<int>( (*pBehaviorFactor)[FO_CHANGE_TYPE] ) );
	if ( eChangeType != m_eChangeType || BCT_PERCENT != eChangeType )
		return false;

	const float fRate = (*pBehaviorFactor)[FO_VALUE];

	if ( DidStart() )
		pGameUnit_->EraseDoubleAttackBuffByBuff( pBuffTemplet_->GetBuffIdentity() );

	m_fRate = fRate;
	pGameUnit_->CreateAndInsertDoubleAttackBuffByBuff( pBuffTemplet_->GetBuffIdentity(), m_fRate );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( 75 ) )
		CX2BuffCapture::Line( "first apply BBT 75 (double attack buff): BTI %d, rate %g", static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), m_fRate );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffDoubleAttackBuffBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffDoubleAttackBuffBehaviorTemplet( *this ) );
}

/** @brief : June 32e7e9f0: [ (float)change type, rate ] */
/*virtual*/ void CX2BuffDoubleAttackBuffBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_eChangeType ) );
	BehaviorFactor.m_vecValues.push_back( m_fRate );
}

/** @brief : June 32e754f0: with at least two values, change type = v[0], rate = v[1], then insert (no erase first). */
/*virtual*/ void CX2BuffDoubleAttackBuffBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	if ( static_cast<UINT>( FO_END ) <= kFactor_.m_vecValues.size() )
	{
		m_eChangeType = static_cast<BUFF_CHANGE_TYPE>( static_cast<int>( kFactor_[FO_CHANGE_TYPE] ) );
		m_fRate = kFactor_[FO_VALUE];

		pGameUnit_->CreateAndInsertDoubleAttackBuffByBuff( kBuffFactor_.m_BuffIdentity, m_fRate );
	}
}

/** @brief : June 32e75540: erase this buff's entry (June 32279d20) */
/*virtual*/ void CX2BuffDoubleAttackBuffBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseDoubleAttackBuffByBuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 89: CX2BuffChangeSkillDamageBehaviorTemplet (June vt 33535378), Phase 6
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : June 32e7a9e0. At the stack cap (limit >= 2 and count now >= limit) nothing changes and it returns true. No factor or not a
		user: false. First apply: the { SKILL_TYPE, rate } pairs of the factor go to the unit's list (InsertSkillDamage, June 31fe3b30,
		only when the buff is not there yet). A restack scales the new rates by the accumulation multiplier and either replaces the
		pairs (limit 1) or adds into the matching skill type as old += new - m (June's arithmetic, not a plain sum), then
		UpdateSkillDamage (June 31fe3c10: erase the buff's entry, insert again).
*/
/*virtual*/ bool CX2BuffChangeSkillDamageBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const UINT uiLimit = pBuffTemplet_->GetAccumulationLimit();
	if ( 2 <= uiLimit && uiLimit <= pBuffTemplet_->GetAccumulationCountNow() )
		return true;

	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) || NULL == pUser )
		return false;

	const UINT uiSize = pBehaviorFactor->m_vecValues.size();

	if ( !DidStart() )
	{
		for ( UINT uiIndex = 0; uiIndex + 1 < uiSize; uiIndex += 2 )
			m_vecSkillTypeRate.push_back( CX2SkillTypeRateByBuff::SkillTypeRate( static_cast<int>( (*pBehaviorFactor)[uiIndex] ), (*pBehaviorFactor)[uiIndex + 1] ) );

		pUser->InsertSkillDamage( pBuffTemplet_->GetBuffIdentity(), m_vecSkillTypeRate );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		if ( BuffNewSkillFirstApply( 89 ) )
			CX2BuffCapture::Line( "first apply BBT 89 (skill damage by skill type): BTI %d, %d pair(s), first type %d rate %g",
				static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<int>( m_vecSkillTypeRate.size() ),
				m_vecSkillTypeRate.empty() ? 0 : m_vecSkillTypeRate.front().first, m_vecSkillTypeRate.empty() ? 0.0f : m_vecSkillTypeRate.front().second );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
		return true;
	}

	float fMultiplier = 1.0f;
	if ( 2 <= uiLimit )
	{
		fMultiplier = pBuffTemplet_->GetAccumulationMultiflier();
		if ( IsSamef( fMultiplier ) )
			fMultiplier = 1.0f;
	}

	vector<CX2SkillTypeRateByBuff::SkillTypeRate> vecNew;
	for ( UINT uiIndex = 0; uiIndex + 1 < uiSize; uiIndex += 2 )
		vecNew.push_back( CX2SkillTypeRateByBuff::SkillTypeRate( static_cast<int>( (*pBehaviorFactor)[uiIndex] ), (*pBehaviorFactor)[uiIndex + 1] * fMultiplier ) );

	if ( 1 == uiLimit )
		m_vecSkillTypeRate = vecNew;
	else
	{
		/// June 32e7a9e0 matches every new pair against the pairs held BEFORE this restack only: an unmatched pair goes to a temporary list
		/// that is appended after the loop (Phase 6 review; appending inline let a later pair of the same factor merge into it).
		vector<CX2SkillTypeRateByBuff::SkillTypeRate> vecAppend;
		for ( vector<CX2SkillTypeRateByBuff::SkillTypeRate>::const_iterator vItr = vecNew.begin(); vecNew.end() != vItr; ++vItr )
		{
			vector<CX2SkillTypeRateByBuff::SkillTypeRate>::iterator vFindItr = m_vecSkillTypeRate.begin();
			while ( m_vecSkillTypeRate.end() != vFindItr && vFindItr->first != vItr->first )
				++vFindItr;

			if ( m_vecSkillTypeRate.end() != vFindItr )
				vFindItr->second += vItr->second - fMultiplier;
			else
				vecAppend.push_back( *vItr );
		}
		m_vecSkillTypeRate.insert( m_vecSkillTypeRate.end(), vecAppend.begin(), vecAppend.end() );
	}

	pUser->UpdateSkillDamage( pBuffTemplet_->GetBuffIdentity(), m_vecSkillTypeRate );
	return true;
}

/** @function : OnAccumulationCountDecreased
	@brief : June slot 7, 32e7c020. Re-derives the pairs for the stack count now (after ACCUMULATION_DECREASE_WHEN_FINISH lowered it),
		otherwise BTI_BUFF_HEAVYFIREARMS_MASTERY would end at its first timeout instead of decaying. With c = count now and
		m = the accumulation multiplier (1.0 when it is 0): base = ( rate + c*m ) / ( c*m + 1.0 ) per pair; for c > 1 the result is
		base with ( base*m - m ) added c-1 times, else base itself. Then UpdateSkillDamage.
	@return : false without a unit, a templet or a user; else true
*/
/*virtual*/ bool CX2BuffChangeSkillDamageBehaviorTemplet::OnAccumulationCountDecreased( CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL == pBuffTemplet_ || NULL == pUser )
		return false;

	const UINT uiCount = pBuffTemplet_->GetAccumulationCountNow();
	float fMultiplier = pBuffTemplet_->GetAccumulationMultiflier();
	if ( IsSamef( fMultiplier ) )
		fMultiplier = 1.0f;

	const float fCountMultiplier = static_cast<float>( uiCount ) * fMultiplier;

	vector<CX2SkillTypeRateByBuff::SkillTypeRate> vecBase;
	for ( vector<CX2SkillTypeRateByBuff::SkillTypeRate>::const_iterator vItr = m_vecSkillTypeRate.begin(); m_vecSkillTypeRate.end() != vItr; ++vItr )
		vecBase.push_back( CX2SkillTypeRateByBuff::SkillTypeRate( vItr->first, ( vItr->second + fCountMultiplier ) / ( fCountMultiplier + 1.0f ) ) );

	vector<CX2SkillTypeRateByBuff::SkillTypeRate> vecResult = vecBase;
	if ( 1 < uiCount )
	{
		for ( UINT uiTimes = 1; uiTimes < uiCount; ++uiTimes )
		{
			for ( UINT uiIndex = 0; uiIndex < vecResult.size(); ++uiIndex )
				vecResult[uiIndex].second += vecBase[uiIndex].second * fMultiplier - fMultiplier;
		}
	}

	m_vecSkillTypeRate = ( 1 < uiCount ? vecResult : vecBase );
	pUser->UpdateSkillDamage( pBuffTemplet_->GetBuffIdentity(), m_vecSkillTypeRate );
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeSkillDamageBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillDamageBehaviorTemplet( *this ) );
}

/** @brief : June 32e7ecf0: [ skill type, rate ] per pair (the type as a float) */
/*virtual*/ void CX2BuffChangeSkillDamageBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	for ( vector<CX2SkillTypeRateByBuff::SkillTypeRate>::const_iterator vItr = m_vecSkillTypeRate.begin(); m_vecSkillTypeRate.end() != vItr; ++vItr )
	{
		BehaviorFactor.m_vecValues.push_back( static_cast<float>( vItr->first ) );
		BehaviorFactor.m_vecValues.push_back( vItr->second );
	}
}

/** @brief : June 32e7af60: rebuild the pairs from [ type, rate ]*, and insert them when the unit is a user (no erase first). */
/*virtual*/ void CX2BuffChangeSkillDamageBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_vecSkillTypeRate.clear();
	const UINT uiSize = kFactor_.m_vecValues.size();
	for ( UINT uiIndex = 0; uiIndex + 1 < uiSize; uiIndex += 2 )
		m_vecSkillTypeRate.push_back( CX2SkillTypeRateByBuff::SkillTypeRate( static_cast<int>( kFactor_[uiIndex] ), kFactor_[uiIndex + 1] ) );

	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->InsertSkillDamage( kBuffFactor_.m_BuffIdentity, m_vecSkillTypeRate );
}

/** @brief : June 32e75b70: a user erases this buff's entry (June 31fe2990) */
/*virtual*/ void CX2BuffChangeSkillDamageBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->EraseSkillDamage( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 90: CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet (June vt 33535594), Phase 6
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : June 32e849c0. No factor or not a user: false. A restack on a started behavior whose stack count is 0 erases the buff's entry
		first. The blocks are re-read from the factor and the entry is inserted (June 3200ae70: only when the buff is not there yet).
*/
/*virtual*/ bool CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) || NULL == pUser )
		return false;

	if ( DidStart() && 0 == pBuffTemplet_->GetAccumulationCountNow() )
		pUser->EraseSkillDamageBySkillID( pBuffTemplet_->GetBuffIdentity() );

	BuffReadSkillIDRateBlocks( *pBehaviorFactor, m_vecBlock );
	pUser->InsertSkillDamageBySkillID( pBuffTemplet_->GetBuffIdentity(), m_vecBlock );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( 90 ) )
		CX2BuffCapture::Line( "first apply BBT 90 (skill damage by skill id): BTI %d, %d block(s), first rate %g",
			static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<int>( m_vecBlock.size() ), m_vecBlock.empty() ? 0.0f : m_vecBlock.front().m_fRate );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet( *this ) );
}

/** @brief : June 32e7ef50: [ rate, id1..idn, -1.0 ] per block */
/*virtual*/ void CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	BuffWriteSkillIDRateBlocks( m_vecBlock, vecFactors_.back() );
}

/** @brief : June 32e84b90: the same walk as SetFactor, and the insert when the unit is a user. */
/*virtual*/ void CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	BuffReadSkillIDRateBlocks( kFactor_, m_vecBlock );

	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->InsertSkillDamageBySkillID( kBuffFactor_.m_BuffIdentity, m_vecBlock );
}

/** @brief : June 32e75b90: a user erases this buff's entry (June 31fe29e0) */
/*virtual*/ void CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->EraseSkillDamageBySkillID( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 91: CX2BuffChargeMpHitBySkillIDBehaviorTemplet (June vt 335355d0), Phase 6
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : SetFactor
	@brief : June 32e84d70, 90's twin on the MP-charge list (June 3200af50 inserts only when the buff is not there yet).
*/
/*virtual*/ bool CX2BuffChargeMpHitBySkillIDBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) || NULL == pUser )
		return false;

	if ( DidStart() && 0 == pBuffTemplet_->GetAccumulationCountNow() )
		pUser->EraseChargeMpHitBySkillID( pBuffTemplet_->GetBuffIdentity() );

	BuffReadSkillIDRateBlocks( *pBehaviorFactor, m_vecBlock );
	pUser->InsertChargeMpHitBySkillID( pBuffTemplet_->GetBuffIdentity(), m_vecBlock );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( 91 ) )
		CX2BuffCapture::Line( "first apply BBT 91 (charge MP hit by skill id): BTI %d, %d block(s), first rate %g",
			static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<int>( m_vecBlock.size() ), m_vecBlock.empty() ? 0.0f : m_vecBlock.front().m_fRate );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChargeMpHitBySkillIDBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChargeMpHitBySkillIDBehaviorTemplet( *this ) );
}

/** @brief : June 32e7f2e0: [ rate, id1..idn, -1.0 ] per block */
/*virtual*/ void CX2BuffChargeMpHitBySkillIDBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	BuffWriteSkillIDRateBlocks( m_vecBlock, vecFactors_.back() );
}

/** @brief : June 32e84f40 */
/*virtual*/ void CX2BuffChargeMpHitBySkillIDBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	BuffReadSkillIDRateBlocks( kFactor_, m_vecBlock );

	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->InsertChargeMpHitBySkillID( kBuffFactor_.m_BuffIdentity, m_vecBlock );
}

/** @brief : June 32e75bb0: a user erases this buff's entry (June 31fe2a30) */
/*virtual*/ void CX2BuffChargeMpHitBySkillIDBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->EraseChargeMpHitBySkillID( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 98: CX2AddDamageOfSkillSlotType (June vt 33535160), Phase 6
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e75d20: SKILL_SLOT_TYPE, 0 when absent (June's enum SKILL_SLOT_TYPE: SKILL_SLOT_None 0, _A 1, _B 2, _TC 3) */
/*virtual*/ bool CX2AddDamageOfSkillSlotType::ParsingBehavior( KLuaManager& luaManager_ )
{
	LUA_GET_VALUE( luaManager_, "SKILL_SLOT_TYPE", m_iSlotType, 0 );
	return true;
}

/** @function : SetFactor
	@brief : June 32e78080. No factor: false. slot type = (int)v[0] (0 when empty), negative: false. rate = v[1] (0.0 when absent). No unit call.
*/
/*virtual*/ bool CX2AddDamageOfSkillSlotType::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	m_iSlotType = static_cast<int>( (*pBehaviorFactor)[0] );
	if ( m_iSlotType < 0 )
		return false;

	m_fRate = (*pBehaviorFactor)[1];

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( 98 ) )
		CX2BuffCapture::Line( "first apply BBT 98 (skill damage by slot type): BTI %d, slot type %d, rate %g",
			static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), m_iSlotType, m_fRate );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2AddDamageOfSkillSlotType::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2AddDamageOfSkillSlotType( *this ) );
}

/** @brief : June 32e7f890: [ (float)slot type, rate ]. CX2GUUser::GetSkillDamageRateBySlotType reads exactly this. */
/*virtual*/ void CX2AddDamageOfSkillSlotType::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( static_cast<float>( m_iSlotType ) );
	BehaviorFactor.m_vecValues.push_back( m_fRate );
}

/** @brief : June 32e78170: slot type = (int)v[0], rate = v[1] (0.0 when absent) */
/*virtual*/ void CX2AddDamageOfSkillSlotType::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_iSlotType = static_cast<int>( kFactor_[0] );
	m_fRate = kFactor_[1];
}

/** @brief : June 32e75cc0: a user erases this buff's entry from BBT 89's list (June 31fe2990, shared with 89). 98 never inserts there. */
/*virtual*/ void CX2AddDamageOfSkillSlotType::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pUser )
		pUser->EraseSkillDamage( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 99, 100 and 103: the shared body, then CX2BuffChangeRateBySkillTypeBehaviorTemplet and CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : the { skill type, change type, value } triples of a factor, each value times fScale_ (an incomplete trailing triple is ignored) */
/*static*/ void CX2BuffSkillTypeChangeBehaviorTemplet::ReadSkillTypeChanges( const KBuffBehaviorFactor& kFactor_, const float fScale_, OUT vector<SkillTypeChange>& vecChange_ )
{
	const UINT uiSize = kFactor_.m_vecValues.size();
	for ( UINT uiIndex = 0; uiIndex + 2 < uiSize; uiIndex += 3 )
		vecChange_.push_back( SkillTypeChange( static_cast<int>( kFactor_[uiIndex] ), static_cast<int>( kFactor_[uiIndex + 1] ), kFactor_[uiIndex + 2] * fScale_ ) );
}

/** @function : ApplySkillTypeChanges
	@brief : The first-apply / restack arithmetic June 32e7b0d0, 32e7b610 and 32e7bb50 share. First apply: the triples as they are. A restack
		scales the new values by the accumulation multiplier m (1.0 when the limit is below 2 or the multiplier is 0), then replaces the
		list (limit 1) or adds into the entry with the same skill type and change type, but only for BCT_PERCENT, and for BCT_FIX_VALUE too
		when bMergeFixValue_ (99 and 100; 103 merges PERCENT only). A new triple with no matching entry, or any other change type, is dropped.
*/
void CX2BuffSkillTypeChangeBehaviorTemplet::ApplySkillTypeChanges( const KBuffBehaviorFactor& kFactor_, const CX2BuffTemplet* pBuffTemplet_, const bool bMergeFixValue_ )
{
	if ( !DidStart() )
	{
		ReadSkillTypeChanges( kFactor_, 1.0f, m_vecChange );
		return;
	}

	const UINT uiLimit = pBuffTemplet_->GetAccumulationLimit();
	float fMultiplier = 1.0f;
	if ( 2 <= uiLimit )
	{
		fMultiplier = pBuffTemplet_->GetAccumulationMultiflier();
		if ( IsSamef( fMultiplier ) )
			fMultiplier = 1.0f;
	}

	vector<SkillTypeChange> vecNew;
	ReadSkillTypeChanges( kFactor_, fMultiplier, vecNew );

	if ( 1 == uiLimit )
	{
		m_vecChange = vecNew;
		return;
	}

	for ( vector<SkillTypeChange>::const_iterator vItr = vecNew.begin(); vecNew.end() != vItr; ++vItr )
	{
		for ( vector<SkillTypeChange>::iterator vOldItr = m_vecChange.begin(); m_vecChange.end() != vOldItr; ++vOldItr )
		{
			if ( vOldItr->m_iSkillType == vItr->m_iSkillType && vOldItr->m_iChangeType == vItr->m_iChangeType )
			{
				if ( BCT_PERCENT == vItr->m_iChangeType || ( bMergeFixValue_ && BCT_FIX_VALUE == vItr->m_iChangeType ) )
					vOldItr->m_fValue += vItr->m_fValue;
				break;
			}
		}
	}
}

/** @brief : June 32e7f940 / 32e7fcb0 / (103's own, same shape): [ skill type, change type, value ] per triple, all as floats */
/*virtual*/ void CX2BuffSkillTypeChangeBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	for ( vector<SkillTypeChange>::const_iterator vItr = m_vecChange.begin(); m_vecChange.end() != vItr; ++vItr )
	{
		BehaviorFactor.m_vecValues.push_back( static_cast<float>( vItr->m_iSkillType ) );
		BehaviorFactor.m_vecValues.push_back( static_cast<float>( vItr->m_iChangeType ) );
		BehaviorFactor.m_vecValues.push_back( vItr->m_fValue );
	}
}

/** @function : SetFactor
	@brief : June 32e7b0d0 (99) / 32e7b610 (100). No factor or not a user: false; else ApplySkillTypeChanges (PERCENT and FIX_VALUE merge).
*/
/*virtual*/ bool CX2BuffChangeRateBySkillTypeBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) || NULL == BuffNewSkillGetUser( pGameUnit_ ) )
		return false;

	ApplySkillTypeChanges( *pBehaviorFactor, pBuffTemplet_, true );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( static_cast<int>( GetType() ) ) )
		CX2BuffCapture::Line( "first apply BBT %d (%s by skill type): BTI %d, %d change(s)", static_cast<int>( GetType() ),
			BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE == GetType() ? "consume MP rate" : "cool time rate",
			static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<int>( m_vecChange.size() ) );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeRateBySkillTypeBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeRateBySkillTypeBehaviorTemplet( *this ) );
}

/** @brief : June 32e7b530 / 32e7ba70: rebuild the triples from the packet. No unit call: 99 and 100 have no store. */
/*virtual*/ void CX2BuffChangeRateBySkillTypeBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_vecChange.clear();
	ReadSkillTypeChanges( kFactor_, 1.0f, m_vecChange );
}

/** @function : SetFactor
	@brief : June 32e7bb50. User only; no factor: false. Same triples as 99, merged for BCT_PERCENT only. Marks the unit's cooldown-speed table
		dirty on both the first apply and a restack.
*/
/*virtual*/ bool CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) || NULL == pUser )
		return false;

	ApplySkillTypeChanges( *pBehaviorFactor, pBuffTemplet_, false );
	pUser->SetSkillCoolTimeSpeedDirty();

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( BuffNewSkillFirstApply( 103 ) )
		CX2BuffCapture::Line( "first apply BBT 103 (skill cool time speed): BTI %d, %d change(s), first type %d change type %d value %g",
			static_cast<int>( pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID ), static_cast<int>( m_vecChange.size() ),
			m_vecChange.empty() ? 0 : m_vecChange.front().m_iSkillType, m_vecChange.empty() ? 0 : m_vecChange.front().m_iChangeType,
			m_vecChange.empty() ? 0.0f : m_vecChange.front().m_fValue );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet( *this ) );
}

/** @brief : June 32e75ea0: with a unit, a templet and a user unit, mark the cooldown-speed table dirty (the buff is gone on the next rebuild) */
/*virtual*/ void CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	CX2GUUser* pUser = BuffNewSkillGetUser( pGameUnit_ );
	if ( NULL != pBuffTemplet_ && NULL != pUser )
		pUser->SetSkillCoolTimeSpeedDirty();
}
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
//}}

//{{ Iruha : 2026-10-04 // June buff port Phase 7 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
/// One "[JUNE_BUFF] first apply BBT n" line per type and session (the strings the Phase 7 gate looks for in the exe)
static void BuffNewFlagFirstApply( const int iType_, const char* szName_, const KBuffIdentity& BuffIdentity_ )
{
	static bool s_bSeen[5] = { false, false, false, false, false };
	const int iSlot = ( 73 == iType_ ? 0 : 94 == iType_ ? 1 : 95 == iType_ ? 2 : 104 == iType_ ? 3 : 4 );
	if ( s_bSeen[iSlot] )
		return;

	s_bSeen[iSlot] = true;
	CX2BuffCapture::Line( "first apply BBT %d (%s): BTI %d", iType_, szName_, BuffIdentity_.m_eBuffTempletID );
}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 73: CX2BuffIgnoreStateGuardBehaviorTemplet, Phase 7
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e752b0. GetBehaviorFactor failing returns false; on the first apply the unit gets one entry for this buff (June 3227b600). */
/*virtual*/ bool CX2BuffIgnoreStateGuardBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	if ( !DidStart() )
	{
		pGameUnit_->InsertIgnoreStateGuard( pBuffTemplet_->GetBuffIdentity() );
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		BuffNewFlagFirstApply( 73, "ignore state guard", pBuffTemplet_->GetBuffIdentity() );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	}
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffIgnoreStateGuardBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffIgnoreStateGuardBehaviorTemplet( *this ) );
}

/** @brief : June 32e7e960: the header only */
/*virtual*/ void CX2BuffIgnoreStateGuardBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @brief : June 32e728b0: the insert, with no first-apply test */
/*virtual*/ void CX2BuffIgnoreStateGuardBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	pGameUnit_->InsertIgnoreStateGuard( kBuffFactor_.m_BuffIdentity );
}

/** @brief : June 32e75300: EraseIgnoreStateGuard( identity ) (June 3227a870) */
/*virtual*/ void CX2BuffIgnoreStateGuardBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseIgnoreStateGuard( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 94: CX2BuffImmuneBuffDebuffBehaviorTemplet, Phase 7
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e79e30. GetBehaviorFactor failing returns false. A restack whose templet count is 0 first drops this buff's immune-list
		entry (June 3227a910); then the values go to the unit (InsertToGameUnit). A restack with a count above 0 adds a second entry, as in June.
*/
/*virtual*/ bool CX2BuffImmuneBuffDebuffBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	if ( DidStart() && 0 == pBuffTemplet_->GetAccumulationCountNow() )
		pGameUnit_->EraseImmuneBuffList( pBuffTemplet_->GetBuffIdentity() );

	InsertToGameUnit( *pBehaviorFactor, pGameUnit_, pBuffTemplet_->GetBuffIdentity() );
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	BuffNewFlagFirstApply( 94, "immune buff/debuff", pBuffTemplet_->GetBuffIdentity() );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/** @brief : The body June's SetFactor (from @32e79e79) and packet handler share: clear the BTI list; v[0] != 0 puts the BTI in the unit's all-buff
		set (June 322780f0), v[1] != 0 in its all-debuff set (32278120); v[2..] become the BTI list (quirk 1: index 2 is the count); one immune-list
		entry { identity, list } is pushed (3227b6b0).
*/
void CX2BuffImmuneBuffDebuffBehaviorTemplet::InsertToGameUnit( const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_, const KBuffIdentity& BuffIdentity_ )
{
	m_vecBTI.clear();

	const UINT uiSize = static_cast<UINT>( kFactor_.m_vecValues.size() );
	if ( 1 <= uiSize && 0.0f != kFactor_.m_vecValues[0] )
		pGameUnit_->InsertImmuneAllBuff( BuffIdentity_.m_eBuffTempletID );

	if ( 2 <= uiSize && 0.0f != kFactor_.m_vecValues[1] )
		pGameUnit_->InsertImmuneAllDebuff( BuffIdentity_.m_eBuffTempletID );

	for ( UINT uiIndex = 2; uiIndex < uiSize; ++uiIndex )
		m_vecBTI.push_back( static_cast<int>( kFactor_.m_vecValues[uiIndex] ) );

	pGameUnit_->InsertImmuneBuffList( BuffIdentity_, m_vecBTI );
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffImmuneBuffDebuffBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneBuffDebuffBehaviorTemplet( *this ) );
}

/** @brief : June 32e7f670: [ (float)m_bAllBuff, (float)m_bAllDebuff, m_vecBTI... ]. Quirk 2: both bools are always false, so peers get 0 / 0. */
/*virtual*/ void CX2BuffImmuneBuffDebuffBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BehaviorFactor.m_vecValues.push_back( m_bAllBuff ? 1.0f : 0.0f );
	BehaviorFactor.m_vecValues.push_back( m_bAllDebuff ? 1.0f : 0.0f );
	BOOST_FOREACH( const int iBTI, m_vecBTI )
		BehaviorFactor.m_vecValues.push_back( static_cast<float>( iBTI ) );
}

/** @brief : June 32e7a040: SetFactor without the restack erase */
/*virtual*/ void CX2BuffImmuneBuffDebuffBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	InsertToGameUnit( kFactor_, pGameUnit_, kBuffFactor_.m_BuffIdentity );
}

/** @brief : June 32e75bd0: the immune-list entry (3227a910), then the BTI from both sets (32278840, 32278880) */
/*virtual*/ void CX2BuffImmuneBuffDebuffBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffIdentity& BuffIdentity = pBuffTemplet_->GetBuffIdentity();
	pGameUnit_->EraseImmuneBuffList( BuffIdentity );
	pGameUnit_->EraseImmuneAllBuff( BuffIdentity.m_eBuffTempletID );
	pGameUnit_->EraseImmuneAllDebuff( BuffIdentity.m_eBuffTempletID );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 95: CX2BuffImmuneNonControlDebuffBehaviorTemplet, Phase 7
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e75c00. GetBehaviorFactor failing returns false; on the first apply the unit gets one entry for this buff (June 3227aa40) and
		then erases its non-control debuffs (3229dcc0).
*/
/*virtual*/ bool CX2BuffImmuneNonControlDebuffBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	if ( !DidStart() )
	{
		pGameUnit_->InsertImmuneNonControlDebuff( pBuffTemplet_->GetBuffIdentity() );
		pGameUnit_->DeleteNonControlDebuff();
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		BuffNewFlagFirstApply( 95, "immune non-control debuff", pBuffTemplet_->GetBuffIdentity() );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	}
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffImmuneNonControlDebuffBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneNonControlDebuffBehaviorTemplet( *this ) );
}

/** @brief : June 32e7f770: the header only */
/*virtual*/ void CX2BuffImmuneNonControlDebuffBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}

/** @brief : June 32e728d0: the insert only (no first-apply test, no DeleteNonControlDebuff) */
/*virtual*/ void CX2BuffImmuneNonControlDebuffBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	pGameUnit_->InsertImmuneNonControlDebuff( kBuffFactor_.m_BuffIdentity );
}

/** @brief : June 32e75c50: EraseImmuneNonControlDebuff( identity ) (June 32279a10) */
/*virtual*/ void CX2BuffImmuneNonControlDebuffBehaviorTemplet::DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ )
{
	pGameUnit_->EraseImmuneNonControlDebuff( pBuffTemplet_->GetBuffIdentity() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 105: CX2BuffImmuneEDTBehaviorTemplet, Phase 7
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e79770. GetBehaviorFactor failing returns false. June's DidStart() branch calls GetAccumulationCountNow() and drops the
		result (not ported: no effect). The EDT list is rebuilt from the factor's values as int on every apply.
*/
/*virtual*/ bool CX2BuffImmuneEDTBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	m_vecEDT.clear();
	BOOST_FOREACH( const float fValue, pBehaviorFactor->m_vecValues )
		m_vecEDT.push_back( static_cast<int>( fValue ) );

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	BuffNewFlagFirstApply( 105, "immune EDT", pBuffTemplet_->GetBuffIdentity() );
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffImmuneEDTBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneEDTBehaviorTemplet( *this ) );
}

/** @brief : June 32e80390: [ (float)edt... ] */
/*virtual*/ void CX2BuffImmuneEDTBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
	KBuffBehaviorFactor& BehaviorFactor = vecFactors_.back();

	BOOST_FOREACH( const int iEDT, m_vecEDT )
		BehaviorFactor.m_vecValues.push_back( static_cast<float>( iEDT ) );
}

/** @brief : June 32e79810: the same rebuild from the packet's values */
/*virtual*/ void CX2BuffImmuneEDTBehaviorTemplet::SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ )
{
	m_vecEDT.clear();
	BOOST_FOREACH( const float fValue, kFactor_.m_vecValues )
		m_vecEDT.push_back( static_cast<int>( fValue ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBT 97: CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet, Phase 7b
//////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief : June 32e75c70. GetBehaviorFactor failing returns false; the first apply calls the unit's slot 201 with 1.0f (fld1 @32e75c9c). */
/*virtual*/ bool CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet::SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ )
{
	const KBuffBehaviorFactor* pBehaviorFactor = NULL;
	if ( !BuffFactor_.GetBehaviorFactor( GetType(), &pBehaviorFactor ) )
		return false;

	if ( !DidStart() )
	{
		pGameUnit_->ChangeMaxCharacterAddResource( 1.0f );
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
		static bool s_bSeenApply = false;
		if ( !s_bSeenApply )
		{
			s_bSeenApply = true;
			CX2BuffCapture::Line( "first apply BBT 97 (change max character add resource): BTI %d", pBuffTemplet_->GetBuffIdentity().m_eBuffTempletID );
		}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	}

	return true;
}

/*virtual*/ CX2BuffBehaviorTempletPtr CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet::GetClonePtr() const
{
	return CX2BuffBehaviorTempletPtr( new CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet( *this ) );
}

/** @brief : June 32e7f800: the header only */
/*virtual*/ void CX2BuffChangeMaxCharacterAddResourceBehaviorTemplet::GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const
{
	vecFactors_.push_back( KBuffBehaviorFactor( GetType() ) );
}
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG
//}}
