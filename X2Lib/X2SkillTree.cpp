#include "StdAfx.h"
#include ".\x2skilltree.h"
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET

//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
#ifdef PRINT_INGAMEINFO_TO_EXCEL
#include "BasicExcel.hpp"
using namespace YExcel;
#endif PRINT_INGAMEINFO_TO_EXCEL
//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

/*static*/ const WCHAR* CX2SkillTree::GetAccelCoreParticleName( const SKILL_ID eAccelSkillID_ )
{
	switch ( eAccelSkillID_ )
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
		return L"BuffAttackCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
		return L"BuffMagickCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
		return L"ShieldBuffCore";
		break;
#else //UPGRADE_SKILL_SYSTEM_2013
	case CX2SkillTree::SI_SA_COMMON_POWER_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_POWER_ADRENALIN:
		return L"BuffAttackCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_MAGIC_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_MAGIC_ADRENALIN:
		return L"BuffMagickCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_SHIELD_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_SHIELD_ADRENALIN:
		return L"ShieldBuffCore";
		break;
#endif //UPGRADE_SKILL_SYSTEM_2013

	case CX2SkillTree::SI_SA_COMMON_AURA_SPEED_ACCEL:
		return L"BuffSpeed_Core";
		break;

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 공통 스킬 개편, 김종훈
	case CX2SkillTree::SI_SA_COMMON_AURA_CRITICAL_ACCEL:				
		return L"BuffCritical_Core";
		break;
	case CX2SkillTree::SI_SA_COMMON_AURA_ADDATK_ACCEL:
		return L"BuffAddAtk_Core";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL:			
		return L"BuffElDefence_Core";
		break;
#endif // UPGRADE_SKILL_SYSTEM_2013 // 공통 스킬 개편, 김종훈

	default:
		return NULL;
		break;
	}
}




CX2SkillTree::CX2SkillTree(void)
//{{ kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
: m_bSkillSlotChanging( false )			// 스킬슬롯체인지 중인가? (SkillSlotChangeReq를 전송한 후 Ack를 아직 받지 못한 상태인가?)
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
, m_iUsedSPoint( 0 )
, m_iUsedCSPoint( 0 )
#endif // UPGRADE_SKILL_SYSTEM_2013
//}} kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	m_mapGetSkillInfo.clear();
#endif // UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL
	m_vecNowLearnSkill.clear();
#endif SERV_SUB_QUEST_LEARN_NEW_SKILL
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	m_bJuneScratch = false;
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
}

CX2SkillTree::~CX2SkillTree(void)
{
	BOOST_TEST_FOREACH( SkillTempletMap::value_type&, value, m_mapSkillTemplet )
	{
		SAFE_DELETE( value.second );
	}
	m_mapSkillTemplet.clear();
}


bool CX2SkillTree::OpenScriptFile( const WCHAR* pFileName, const WCHAR* pSkillDataFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pSkillTree", this );

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	/// 데미지 배율 정보를 먼저 얻기 위해, SkillData.lua를 먼저 파싱한다.
	if( g_pKTDXApp->LoadLuaTinker( pSkillDataFileName ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, pSkillDataFileName );
		return false;
	}
#endif // UPGRADE_SKILL_SYSTEM_2013

	if( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, pFileName );
		return false;
	}
	
#ifdef LUA_TRANS_DEVIDE
#ifdef UPGRADE_SKILL_SYSTEM_2013
	if( g_pKTDXApp->LoadLuaTinker( L"NewSkillTempletVer2Trans.lua" ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, L"NewSkillTempletVer2Trans.lua" );
		return false;
	}
#else //UPGRADE_SKILL_SYSTEM_2013
	if( g_pKTDXApp->LoadLuaTinker( L"NewSkillTempletTrans.lua" ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, L"NewSkillTempletTrans.lua" );
		return false;
	}
#endif //UPGRADE_SKILL_SYSTEM_2013
#endif LUA_TRANS_DEVIDE

#ifndef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	/// 데미지 배율 정보를 먼저 얻기 위해, SkillData.lua를 먼저 파싱하도록 위치 이동
	if( g_pKTDXApp->LoadLuaTinker( pSkillDataFileName ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, pSkillDataFileName );
		return false;
	}
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	JuneOpenCommandIconData();		/// June 322083e0 @322092f2: 32207dd0( L"CommandIconData.lua" ), then the post-process 32206050
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	OpenScriptFilePostProcess();
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	JuneLogLoadSummary();
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET

	return true;
}


void CX2SkillTree::OpenScriptFilePostProcess()
{

	BOOST_TEST_FOREACH( UnitSkillTreeTempletMap::value_type&, value, m_mapUnitSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		BOOST_TEST_FOREACH( SkillTreeTempletMap::value_type&, value2, mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			skillTreeTemplet.m_iFollowingSkill = 0;
#else // UPGRADE_SKILL_SYSTEM_2013
			skillTreeTemplet.m_vecFollowingSkill.resize(0);
#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}



	BOOST_TEST_FOREACH( UnitSkillTreeTempletMap::value_type&, value, m_mapUnitSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		BOOST_TEST_FOREACH( SkillTreeTempletMap::value_type&, value2, mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetSkillTreeTemplet( value.first, static_cast<SKILL_ID>( skillTreeTemplet.m_iPrecedingSkill ) );
			if( NULL == pPrecedingSkillTreeTemplet )
				continue;

			SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
			pPrecedingSkillTemplet_NotConst->m_iFollowingSkill= value2.first;
#else // UPGRADE_SKILL_SYSTEM_2013
			BOOST_TEST_FOREACH( int, iPrecedingSkillID, skillTreeTemplet.m_vecPrecedingSkill )
			{
				const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetSkillTreeTemplet( value.first, (SKILL_ID) iPrecedingSkillID );
				if( NULL == pPrecedingSkillTreeTemplet )
					continue;

				SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
				pPrecedingSkillTemplet_NotConst->m_vecFollowingSkill.push_back( value2.first );
			}
#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}

	//{{ oasis907 : 김상윤 [2009.12.4] //

	BOOST_TEST_FOREACH( GuildSkillTreeTempletMap::value_type&, value, m_mapGuildSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		BOOST_TEST_FOREACH( SkillTreeTempletMap::value_type&, value2, mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			skillTreeTemplet.m_iFollowingSkill = 0;
#else // UPGRADE_SKILL_SYSTEM_2013
			skillTreeTemplet.m_vecFollowingSkill.resize(0);
#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}



	BOOST_TEST_FOREACH( GuildSkillTreeTempletMap::value_type&, value, m_mapGuildSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		BOOST_TEST_FOREACH( SkillTreeTempletMap::value_type&, value2, mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
				const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetGuildSkillTreeTemplet( value.first, static_cast<SKILL_ID>( skillTreeTemplet.m_iPrecedingSkill ) );
				if( NULL == pPrecedingSkillTreeTemplet )
					continue;

				SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
				pPrecedingSkillTemplet_NotConst->m_iFollowingSkill = value2.first;
#else // UPGRADE_SKILL_SYSTEM_2013
			BOOST_TEST_FOREACH( int, iPrecedingSkillID, skillTreeTemplet.m_vecPrecedingSkill )
			{
				const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetGuildSkillTreeTemplet( value.first, (SKILL_ID) iPrecedingSkillID );
				if( NULL == pPrecedingSkillTreeTemplet )
					continue;

				SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
				pPrecedingSkillTemplet_NotConst->m_vecFollowingSkill.push_back( value2.first );
			}
#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}
	//}} oasis907 : 김상윤 [2009.12.4] //
}




bool CX2SkillTree::AddSkillTreeTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

	CX2Unit::UNIT_CLASS eUnitClass = CX2Unit::UC_NONE;
	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "m_eUnitClass",				eUnitClass,				CX2Unit::UNIT_CLASS,		CX2Unit::UC_NONE		);
	LUA_GET_VALUE_ENUM( luaManager, "m_iSkillID",				eSkillID,				SKILL_ID,					SI_NONE					);

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID, 1 );
#endif // UPGRADE_SKILL_SYSTEM_2013
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}

	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"m_iMasterLevel",			skillTreeTemplet.m_iMaxSkillLevel,	0				);
	LUA_GET_VALUE( luaManager,		"m_iRowIndex",				skillTreeTemplet.m_iTier,			0				);
	LUA_GET_VALUE( luaManager,		"m_iColumnIndex",			skillTreeTemplet.m_iIndexInTier,	0				);

	int iPrecedingSkillID = 0;
#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM
	/// June's tree parser does not read m_iPrecedingSkill ( S2 kept it while the data was March's; June's unit rows carry none )
#else SERV_IRUHADEV_JUNE_SKILL_REFORM
	LUA_GET_VALUE( luaManager,		"m_iPrecedingSkill",		skillTreeTemplet.m_iPrecedingSkill,	0				);
#endif SERV_IRUHADEV_JUNE_SKILL_REFORM


	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapUnitSkillTreeTemplet[ (int)eUnitClass ];
	mapSkillTreeTemplet[ eSkillID ] = skillTreeTemplet;

#else // UPGRADE_SKILL_SYSTEM_2013

	CX2Unit::UNIT_CLASS eUnitClass = CX2Unit::UC_NONE;
	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "UNIT_CLASS",				eUnitClass,				CX2Unit::UNIT_CLASS,		CX2Unit::UC_NONE		);
	LUA_GET_VALUE_ENUM( luaManager, "SKILL_ID",					eSkillID,				SKILL_ID,					SI_NONE					);

	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID, 1 );
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}
	
			
	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"MAX_SKILL_LEVEL",			skillTreeTemplet.m_iMaxSkillLevel,	0				);
	LUA_GET_VALUE( luaManager,		"TIER_INDEX",				skillTreeTemplet.m_iTier,			0				);
	LUA_GET_VALUE( luaManager,		"INDEX_IN_TIER",			skillTreeTemplet.m_iIndexInTier,	0				);



	std::wstring wstrPrecedingSkill = L"";
	LUA_GET_VALUE( luaManager,		"m_vecPrecedingSkill",		wstrPrecedingSkill, 		L""		);

	if( false == wstrPrecedingSkill.empty() )
	{
		WCHAR* pBegin = const_cast<WCHAR*>(wstrPrecedingSkill.c_str());
		WCHAR* pEnd = pBegin + wstrPrecedingSkill.length();


		while( true )
		{
			int iSkillID = TokenizeInt( &pBegin, pEnd );
			if( iSkillID > 0 )
				skillTreeTemplet.m_vecPrecedingSkill.push_back( iSkillID );
			else 
				break;
		}
	}
	

	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapUnitSkillTreeTemplet[ (int)eUnitClass ];
	mapSkillTreeTemplet[ eSkillID ] = skillTreeTemplet;

#endif // UPGRADE_SKILL_SYSTEM_2013

	return true;

}


bool CX2SkillTree::AddSkillTemplet_LUA()
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	/// Phase S2: June's loader (32216320). Under _DEBUG the scratch load can still run March's body (m_bJuneMarchParser) to compare the two.
		return AddSkillTemplet_JuneLUA();
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING


	bool bInHouseTestOnly = false;
	LUA_GET_VALUE( luaManager,		"IN_HOUSE_TEST_ONLY",				bInHouseTestOnly,	false );


	
#if defined( _SERVICE_ ) || defined( _OPEN_TEST_ ) || defined( _FOREIGN_ ) || defined( _IN_HOUSE_SERVICE_READY_QA_ )
	if( true == bInHouseTestOnly )
		return false;
#endif // defined( _SERVICE_ ) || defined( _OPEN_TEST_ ) || defined( _FOREIGN_ ) || defined( _IN_HOUSE_SERVICE_READY_QA_ )

#ifdef UPGRADE_SKILL_SYSTEM_2013

	SkillTemplet* pSkillTemplet = new SkillTemplet();

	LUA_GET_VALUE_ENUM( luaManager,		"m_eType",						pSkillTemplet->m_eType,							SKILL_TYPE,			ST_NONE					);

	/// 길드 스킬 설정
	switch ( pSkillTemplet->m_eType )
	{
	case ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
	case ST_GUILD_PASSIVE:
		{
			pSkillTemplet->m_bGuildSkill = true;
		} break;
	}

	LUA_GET_VALUE_ENUM( luaManager,		"m_eID",						pSkillTemplet->m_eID,							SKILL_ID,			SI_NONE					);

	if( true == luaManager.BeginTable( "m_SkillCoolTime" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecSkillCoolTime		);
	if( true == luaManager.BeginTable( "m_MPConsumption" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecMPConsumption		);
	if( true == luaManager.BeginTable( "m_RequireCharacterLevel" ) )	SetSkillVlaue( luaManager,						pSkillTemplet->m_vecRequireCharacterLevel	);

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	LUA_GET_VALUE( luaManager,			"m_usCBConsumption",			pSkillTemplet->m_usCBConsumption,				0		);
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

#ifdef ARA_CHARACTER_BASE
	LUA_GET_VALUE( luaManager,			"m_usFPGainCount",				pSkillTemplet->m_usFPGainCount,					0		);
	LUA_GET_VALUE( luaManager,			"m_usFPConsumtion",				pSkillTemplet->m_usFPConsumtion,				0		);
#endif

	LUA_GET_VALUE( luaManager,			"m_wstrName",					pSkillTemplet->m_wstrName,						L""		);

	LUA_GET_VALUE( luaManager,			"m_wstrMainDesc",				pSkillTemplet->m_wstrMainDesc,					L""		);

#ifdef HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC // 김태환
	/// 스킬 효과 툴팁은 30레벨까지 설정 되어 있지 않다면, 자동으로 채우지 말자.
	if( true == luaManager.BeginTable( "m_EffectiveDesc" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecEffectiveDesc, false);
#else //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	if( true == luaManager.BeginTable( "m_EffectiveDesc" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecEffectiveDesc		);
#endif //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC

	LUA_GET_VALUE( luaManager,			"m_bBornSealed",				pSkillTemplet->m_bBornSealed,					false	);

	LUA_GET_VALUE( luaManager,			"m_wstrIconTextureName",		pSkillTemplet->m_wstrIconName,					L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrIconTexturePieceName",	pSkillTemplet->m_wstrIconPieceName,				L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrImageTextureName",		pSkillTemplet->m_wstrImageName,					L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrImageTexturePieceName",	pSkillTemplet->m_wstrImagePieceName,			L""		);

	SKILL_POWER_RATE_TYPE ePowerRate = SPRT_BASIC_CLASS;
	LUA_GET_VALUE_ENUM( luaManager,		"m_ePowerRate",					ePowerRate,										SKILL_POWER_RATE_TYPE,			SPRT_BASIC_CLASS	);
	SetSkillPowerRate( ePowerRate, pSkillTemplet->m_vecPowerRate );
#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
	pSkillTemplet->m_eSkillPowerRateType = ePowerRate;
#endif //SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
	/// buff port 16-step3b5: the skill-group power sockets match this (June 31fe14f0 reads templet +0x174)
	pSkillTemplet->m_eJunePowerRateType = ePowerRate;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

	LUA_GET_VALUE_ENUM( luaManager,		"m_eActiveSkillUseCondtion",	pSkillTemplet->m_eActiveSkillUseCondtion,		ACTIVE_SKILL_USE_CONDITION,		ASUT_NONE	);

	LUA_GET_VALUE( luaManager,			"m_bShowSkillLevel",			pSkillTemplet->m_bShowSkillLevel,				false	);

	LUA_GET_VALUE( luaManager,			"m_iRequireLearnSkillPoint",	pSkillTemplet->m_iRequireLearnSkillPoint,		0		);

	LUA_GET_VALUE( luaManager,			"m_iRequireUpgradeSkillPoint",	pSkillTemplet->m_iRequireUpgradeSkillPoint,		0		);

	LUA_GET_VALUE( luaManager,			"m_usWOSType",					static_cast<int>( pSkillTemplet->m_iWOSType ),	0		);

#ifdef NO_MORE_ERROR_TEST
	if( true == bInHouseTestOnly )
	{
		InHouseLog( L"사내에만 적용되는 skilltemplet이 있습니다." );
		InHouseLog( pSkillTemplet->m_wstrName.c_str() );
	}
#endif NO_MORE_ERROR_TEST

	SetSkillStat( luaManager, pSkillTemplet->m_vecStat );

	std::map<SKILL_ID, SkillTemplet*>::iterator it = m_mapSkillTemplet.find( pSkillTemplet->m_eID );
	if( it == m_mapSkillTemplet.end() )
	{
		m_mapSkillTemplet[ pSkillTemplet->m_eID ] = pSkillTemplet;
	}
	else
	{
		ASSERT( !"SKILL is Duplication!" );	/// 스킬 중복!
		return true;
	}

	float fValue = 0.f;

	if( true == luaManager.BeginTable( "SA_EFFECTIVE_TIME" ) )					SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_TIME, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_COUNT" ) )          		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_COUNT, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SKILL_ID" ) )                 		SetSkillAbilityMap( &luaManager, SA_SKILL_ID, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_COMBO_ID" ) )                 		SetSkillAbilityMap( &luaManager, SA_COMBO_ID, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_RATE" ) )           		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_RATE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_REL" ) )                  		SetSkillAbilityMap( &luaManager, SA_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_REL" ) )            		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_PHYSIC_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_MAGIC_REL" ) )            		SetSkillAbilityMap( &luaManager, SA_DEF_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_ATK_PHYSIC_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_TEAM_ATK_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_ATK_MAGIC_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_DEF_PHYSIC_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_DEF_MAGIC_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_DAMAGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGED_REL" ) )              		SetSkillAbilityMap( &luaManager, SA_DAMAGED_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_UNFIXED_DEFENSE_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_UNFIXED_DEFENSE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SUMMON_ATK_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_SUMMON_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_HP_ABS" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_HP_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_HP_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_HP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_MP_ABS" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_MP_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_MP_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_MP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TELEPORT_RANGE_ABS" ) )       		SetSkillAbilityMap( &luaManager, SA_TELEPORT_RANGE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_CHARGING_TIME" ) )            		SetSkillAbilityMap( &luaManager, SA_CHARGING_TIME, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_REL_TO_MP_ABS_1" ) )       		SetSkillAbilityMap( &luaManager, SA_HP_REL_TO_MP_ABS_1, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_REL_TO_MP_ABS_2" ) )       		SetSkillAbilityMap( &luaManager, SA_HP_REL_TO_MP_ABS_2, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_CONSUME_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_MP_CONSUME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_GAIN_ON_HIT_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_MP_GAIN_ON_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SCOPE_ABS" ) )                		SetSkillAbilityMap( &luaManager, SA_SCOPE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_FIRE_COUNT" ) )    		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_FIRE_COUNT, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_RANGE_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_RANGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_SIZE" ) )          		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_SIZE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_PIERCING" ) )      		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_PIERCING, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MOVE_SPEED" ) )               		SetSkillAbilityMap( &luaManager, SA_MOVE_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_JUMP_SPEED" ) )                   	SetSkillAbilityMap( &luaManager, SA_JUMP_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_TO_ATK_MAGIC" ) )      	SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_TO_ATK_MAGIC, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_TO_ATK_PHYSIC" ) )      	SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_TO_ATK_PHYSIC, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_READY_TIME_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_READY_TIME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_MELEE_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_MELEE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_RANGE_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_RANGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_ABS" ) )               	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_REGENERATION_ABS" ) )          	SetSkillAbilityMap( &luaManager, SA_MP_REGENERATION_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_INTAKE_ABS" ) )                	SetSkillAbilityMap( &luaManager, SA_MP_INTAKE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_ANI_SPEED" ) )             	SetSkillAbilityMap( &luaManager, SA_TARGET_ANI_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_MOVE_SPEED" ) )            	SetSkillAbilityMap( &luaManager, SA_TARGET_MOVE_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_JUMP_SPEED" ) )            	SetSkillAbilityMap( &luaManager, SA_TARGET_JUMP_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_ATK_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_TARGET_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_INTAKE_REL_DAMAGE" ) )         	SetSkillAbilityMap( &luaManager, SA_HP_INTAKE_REL_DAMAGE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_DAM_ABS" ) )         	SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_DAM_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_RATE_ABS" ) )        	SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_RATE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_CRITICAL_DAM_ABS" ) )         	SetSkillAbilityMap( &luaManager, SA_DEF_CRITICAL_DAM_ABS, pSkillTemplet );
	//{{ 2011-04 에 패치될 청 캐시 스킬
#ifdef	CASH_SKILL_FOR_CHUNG_2011_04
	if( true == luaManager.BeginTable( "SA_DEF_CRITICAL_RATE_ABS" ) )        	SetSkillAbilityMap( &luaManager, SA_DEF_CRITICAL_RATE_ABS, pSkillTemplet );
#endif	CASH_SKILL_FOR_CHUNG_2011_04				
	//}} 2011-04 에 패치될 청 캐시 스킬
#ifdef GUILD_SKILL
	if( true == luaManager.BeginTable( "SA_BACK_SPEED_X" ) )                 	SetSkillAbilityMap( &luaManager, SA_BACK_SPEED_X, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ITEM_SPECIAL_ABILITY_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_ITEM_SPECIAL_ABILITY_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_GAIN_GET_HIT_REL" ) )          	SetSkillAbilityMap( &luaManager, SA_MP_GAIN_GET_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SOUL_GAIN_ON_HIT_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_SOUL_GAIN_ON_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SOUL_GAIN_GET_HIT_REL" ) )        	SetSkillAbilityMap( &luaManager, SA_SOUL_GAIN_GET_HIT_REL, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_HP_GAIN_REL_MAX_HP" ) )           	SetSkillAbilityMap( &luaManager, SA_HP_GAIN_REL_MAX_HP, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ED_GAIN_REL" ) )                  	SetSkillAbilityMap( &luaManager, SA_ED_GAIN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL" ) )	SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EXP_GAIN_REL" ) )                 	SetSkillAbilityMap( &luaManager, SA_EXP_GAIN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_ABSORB_REL_MAX_HP" ) )     	SetSkillAbilityMap( &luaManager, SA_DAMAGE_ABSORB_REL_MAX_HP, pSkillTemplet );
#endif GUILD_SKILL



	if( true == luaManager.BeginTable( "SA_STRONG_MIND_ACTIVE_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_STRONG_MIND_ACTIVE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_STRONG_MIND_SPECIAL_ACTIVE_REL" ) ) SetSkillAbilityMap( &luaManager, SA_STRONG_MIND_SPECIAL_ACTIVE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_RETAKE_ABS" ) )     				SetSkillAbilityMap( &luaManager, SA_MP_RETAKE_ABS, pSkillTemplet );


	if( true == luaManager.BeginTable( "SA_RESIST_FIRE_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_FIRE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_WATER_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_WATER_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_GREEN_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_GREEN_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_WIND_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_WIND_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_LIGHT_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_LIGHT_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_DARK_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_DARK_ABS, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_FIRST_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_FIRST_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_SECOND_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_SECOND_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_THIRD_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_THIRD_REL, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_REMOTE_SPEED_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_REMOTE_SPEED_ABS, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_DAMAGE_TYPE_CHANGE" ) )     		SetSkillAbilityMap( &luaManager, SA_DAMAGE_TYPE_CHANGE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_TIME" ) )     				SetSkillAbilityMap( &luaManager, SA_DAMAGE_TIME, pSkillTemplet );	

	//{{ 김상훈 : 2010.11.12
#ifdef NEW_SKILL_2010_11
	if ( true == luaManager.BeginTable( "SA_RECOVER_MP_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RECOVER_MP_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_MANA" ) )     				SetSkillAbilityMap( &luaManager, SA_TARGET_MANA, pSkillTemplet );
#endif NEW_SKILL_2010_11
	//}} 김상훈 : 2010.11.12

	//{{ JHKang / 강정훈 / 2010/11/22
#ifdef NEW_SKILL_2010_11
	if ( true == luaManager.BeginTable( "SA_TARGET_MANA_REL" ) )				SetSkillAbilityMap( &luaManager, SA_TARGET_MANA_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SIZE_ABS" ) )						SetSkillAbilityMap( &luaManager, SA_SIZE_ABS, pSkillTemplet );
#endif NEW_SKILL_2010_11
	//}} JHKang / 강정훈 / 2010/11/22

#ifdef NEW_SKILL_2010_11 // oasis907 : 김상윤 [2010.11.24] 
	if ( true == luaManager.BeginTable( "SA_REFLECT_REL" ) )					SetSkillAbilityMap( &luaManager, SA_REFLECT_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EL_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_EL_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_HP_REL" ) )					SetSkillAbilityMap( &luaManager, SA_TARGET_HP_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_HITRATE_REL" ) )			SetSkillAbilityMap( &luaManager, SA_TARGET_HITRATE_REL, pSkillTemplet );
#endif NEW_SKILL_2010_11

	//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	if ( true == luaManager.BeginTable( "SA_CHARGE_CANNONBALL" ) )				SetSkillAbilityMap( &luaManager, SA_CHARGE_CANNON_BALL, pSkillTemplet );
#endif	NEW_CHARACTER_CHUNG
	//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

	//{{ kimhc // 2011.1.14 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	if ( true == luaManager.BeginTable( "SA_FIRE_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_FIRE_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_WATER_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_WATER_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_NATURE_DEFENCE_ABS" ) )			SetSkillAbilityMap( &luaManager, SA_NATURE_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_WIND_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_WIND_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_LIGHT_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_LIGHT_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_DARK_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_DARK_DEFENCE_ABS, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_EDT_FIRE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_FIRE_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_FROZEN_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_FROZEN_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_ICE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_ICE_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_POISON_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_POISON_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_Y_PRESSED_RESIST_REL" ) )		SetSkillAbilityMap( &luaManager, SA_EDT_Y_PRESSED_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_LEG_WOUND_RESIST_REL" ) )		SetSkillAbilityMap( &luaManager, SA_EDT_LEG_WOUND_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_STUN_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_STUN_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_CURSE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_CURSE_RESIST_REL, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_EXPAND_CB" ) )						SetSkillAbilityMap( &luaManager, SA_EXPAND_CB, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ANI_SPEED" ) )						SetSkillAbilityMap( &luaManager, SA_ANI_SPEED, pSkillTemplet );
#endif	CHUNG_FIRST_CLASS_CHANGE
	//}} kimhc // 2011.1.14 // 청 1차 전직

	//{{ kimhc // 2011-04-01 // 2011-04 에 패치될 청 캐시 스킬
#ifdef	CASH_SKILL_FOR_CHUNG_2011_04
	if ( true == luaManager.BeginTable( "SA_ACCURACY_PERCENT_ABS" ) )			SetSkillAbilityMap( &luaManager, SA_ACCURACY_PERCENT_ABS, pSkillTemplet );
#endif	CASH_SKILL_FOR_CHUNG_2011_04
	//}} kimhc // 2011-04-01 // 2011-04 에 패치될 청 캐시 스킬

#ifdef CHUNG_SECOND_CLASS_CHANGE
	if ( true == luaManager.BeginTable( "SA_CRITICAL_EVASION_REL" ) )			SetSkillAbilityMap( &luaManager, SA_CRITICAL_EVASION_REL, pSkillTemplet );
#endif CHUNG_SECOND_CLASS_CHANGE

#ifdef CHUNG_SECOND_CLASS_CHANGE
	if ( true == luaManager.BeginTable( "SA_DECREASE_REL" ) )					SetSkillAbilityMap( &luaManager, SA_DECREASE_REL, pSkillTemplet );
#endif CHUNG_SECOND_CLASS_CHANGE

#ifdef ELSWORD_SHEATH_KNIGHT
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FIRST_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FIRST_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_SECOND_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_SECOND_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_THIRD_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_THIRD_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FOURTH_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FOURTH_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FIFTH_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FIFTH_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TELEPORT_SPEED_REL" ) )			SetSkillAbilityMap( &luaManager, SA_TELEPORT_SPEED_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "HP_CONSUME_REL" ) )					SetSkillAbilityMap( &luaManager, HP_CONSUME_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_MP_BURN_ABS" ) )					SetSkillAbilityMap( &luaManager, SA_MP_BURN_ABS, pSkillTemplet );
#endif ELSWORD_SHEATH_KNIGHT

#ifdef	SERV_TRAPPING_RANGER_TEST
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_01" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_01, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_02" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_02, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_03" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_03, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_04" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_04, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_01_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_01_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_02_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_02_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_03_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_03_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_04_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_04_RATE, pSkillTemplet );
#endif	SERV_TRAPPING_RANGER_TEST

#ifdef RAVEN_WEAPON_TAKER
	if( true == luaManager.BeginTable( "SA_EVASION_REL" ) )     				SetSkillAbilityMap( &luaManager, SA_EVASION_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_REDUCE_EDT_TIME_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_REDUCE_EDT_TIME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_OVERHEAT_HP" ) )     				SetSkillAbilityMap( &luaManager, SA_OVERHEAT_HP, pSkillTemplet );
#endif RAVEN_WEAPON_TAKER

#ifdef EVE_ELECTRA
	if( true == luaManager.BeginTable( "SA_SCOPE_ABS_HYPER" ) )     			SetSkillAbilityMap( &luaManager, SA_SCOPE_ABS_HYPER, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_TIME_HYPER" ) )     		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_TIME_HYPER, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_AIR_MOVE_COUNT" ) )     			SetSkillAbilityMap( &luaManager, SA_AIR_MOVE_COUNT, pSkillTemplet );
#endif EVE_ELECTRA

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	if( true == luaManager.BeginTable( "SA_NOT_CANNONBALL_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_NOT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_INSERT_CANNONBALL_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_INSERT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_BE_SHOT_CANNONBALL_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_BE_SHOT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_CANNONBALL_DAMAGE_UP_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_CANNONBALL_DAMAGE_UP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE" ) )SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SIEGE_SHELLING_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_SIEGE_SHELLING_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_DAMAGED_ATTACK_BASE" ) )  SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_DAMAGED_ATTACK_BASE, pSkillTemplet );
#endif

#ifdef SERV_CHUNG_TACTICAL_TROOPER
	if( true == luaManager.BeginTable( "SA_FIRST_HYPER_EFFECTIVE" ) )			SetSkillAbilityMap( &luaManager, SA_FIRST_HYPER_EFFECTIVE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SECOND_HYPER_EFFECTIVE" ) )     	SetSkillAbilityMap( &luaManager, SA_SECOND_HYPER_EFFECTIVE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_THIRD_HYPER_EFFECTIVE" ) )			SetSkillAbilityMap( &luaManager, SA_THIRD_HYPER_EFFECTIVE, pSkillTemplet );
#endif SERV_CHUNG_TACTICAL_TROOPER

#ifdef BALANCE_CODE_NEMESIS_20121213
	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_RATE_ATK_BASE" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_RATE_ATK_BASE, pSkillTemplet );
#endif //BALANCE_CODE_NEMESIS_20121213
	if( true == luaManager.BeginTable( "SA_PROJECTILE_CURVE_SPEED_REL" ) )		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_CURVE_SPEED_REL, pSkillTemplet );	
	if( true == luaManager.BeginTable( "SA_DP_GAIN_REL" ) )					SetSkillAbilityMap( &luaManager, SA_DP_GAIN_REL, pSkillTemplet );	
	if( true == luaManager.BeginTable( "SA_DP_USE_REL" ) )						SetSkillAbilityMap( &luaManager, SA_DP_USE_REL, pSkillTemplet );	
	
	CX2BuffTempletManager::GetInstance()->GetBuffFactorPtrFromBuffFactorList( luaManager, pSkillTemplet->m_vecBuffFactorPtr, pSkillTemplet->m_vecBuffFactorID );

#else // UPGRADE_SKILL_SYSTEM_2013

	SkillTemplet* pSkillTemplet = new SkillTemplet();

	LUA_GET_VALUE( luaManager,		"m_bGuildSkill",			pSkillTemplet->m_bGuildSkill,		false		);

	LUA_GET_VALUE_ENUM( luaManager, "m_eType",					pSkillTemplet->m_eType,				SKILL_TYPE,					ST_NONE				);
	LUA_GET_VALUE_ENUM( luaManager, "m_eID",					pSkillTemplet->m_eID,				SKILL_ID,					SI_NONE				);
	LUA_GET_VALUE( luaManager,		"m_iSkillLevel",			pSkillTemplet->m_iSkillLevel,		1												);
	LUA_GET_VALUE( luaManager,		"m_fSkillCoolTime",			pSkillTemplet->m_fSkillCoolTime,	0.f												);
	LUA_GET_VALUE( luaManager,		"m_fMPConsumption",			pSkillTemplet->m_fMPConsumption,	0.f												);

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	LUA_GET_VALUE( luaManager,		"CB_CONSUMPTION",			pSkillTemplet->m_usCBConsumption,	0 );
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

#ifdef ARA_CHARACTER_BASE
	LUA_GET_VALUE( luaManager,		"FP_GAIN_COUNT",			pSkillTemplet->m_usFPGainCount,	0 );
	LUA_GET_VALUE( luaManager,		"FP_CONSUME_COUNT",			pSkillTemplet->m_usFPConsumtion,	0 );
#endif

	LUA_GET_VALUE( luaManager,		"m_Name",						pSkillTemplet->m_wstrName,					L""	);
	LUA_GET_VALUE( luaManager,		"m_Desc",						pSkillTemplet->m_wstrMainDesc,				L""	);

	LUA_GET_VALUE( luaManager,		"m_bBornSealed",			pSkillTemplet->m_bBornSealed,		false											);

	LUA_GET_VALUE( luaManager,		"m_wstrTextureName",			pSkillTemplet->m_wstrIconName,				L""	);
	LUA_GET_VALUE( luaManager,		"m_wstrTexturePieceName",		pSkillTemplet->m_wstrIconPieceName,			L""	);

	LUA_GET_VALUE( luaManager,		"m_fPowerRate",				pSkillTemplet->m_fPowerRate,		1.f												);

	LUA_GET_VALUE_ENUM( luaManager, "m_eActiveSkillUseCondtion",	pSkillTemplet->m_eActiveSkillUseCondtion,	ACTIVE_SKILL_USE_CONDITION,		ASUT_NONE	);

	LUA_GET_VALUE( luaManager,		"m_bShowSkillLevel",			pSkillTemplet->m_bShowSkillLevel,		false		);




#ifdef NO_MORE_ERROR_TEST
	if( true == bInHouseTestOnly )
	{
		InHouseLog( L"사내에만 적용되는 skilltemplet이 있습니다." );
		InHouseLog( pSkillTemplet->m_wstrName.c_str() );
	}
#endif NO_MORE_ERROR_TEST






	if( luaManager.BeginTable( "m_Stat" ) == true )
	{
		float fBaseHP;		
		float fAtkPhysic;		
		float fAtkMagic;		
		float fDefPhysic;		
		float fDefMagic;		

		LUA_GET_VALUE( luaManager,		"m_fBaseHP",			fBaseHP,			0.f		);
		LUA_GET_VALUE( luaManager,		"m_fAtkPhysic",			fAtkPhysic,			0.f		);
		LUA_GET_VALUE( luaManager,		"m_fAtkMagic",			fAtkMagic,			0.f		);
		LUA_GET_VALUE( luaManager,		"m_fDefPhysic",			fDefPhysic,			0.f		);
		LUA_GET_VALUE( luaManager,		"m_fDefMagic",			fDefMagic,			0.f		);

		pSkillTemplet->m_Stat.m_fBaseHP 			= fBaseHP;		
		pSkillTemplet->m_Stat.m_fAtkPhysic 			= fAtkPhysic;			
		pSkillTemplet->m_Stat.m_fAtkMagic 			= fAtkMagic;		
		pSkillTemplet->m_Stat.m_fDefPhysic 			= fDefPhysic;			
		pSkillTemplet->m_Stat.m_fDefMagic 			= fDefMagic;		

		luaManager.EndTable(); // m_Stat
	}

	SkillKey skillKey;
	skillKey.m_eID			= pSkillTemplet->m_eID;
	skillKey.m_iSkillLevel	= pSkillTemplet->m_iSkillLevel;
	SkillTempletMap::iterator it = m_mapSkillTemplet.find( skillKey );
	ASSERT( it == m_mapSkillTemplet.end() );
	if( it == m_mapSkillTemplet.end() )
	{
		m_mapSkillTemplet[ skillKey ] = pSkillTemplet;
	}
	else
	{
		// 스킬 중복
		return true;
	}






	float fValue = 0.f;

	if( true == luaManager.GetValue( L"SA_EFFECTIVE_TIME", fValue ) )					pSkillTemplet->m_mapSkillAbility[ SA_EFFECTIVE_TIME ] = fValue;
	if( true == luaManager.GetValue( L"SA_EFFECTIVE_COUNT", fValue ) )          		pSkillTemplet->m_mapSkillAbility[ SA_EFFECTIVE_COUNT ] = fValue;
	if( true == luaManager.GetValue( L"SA_SKILL_ID", fValue ) )                 		pSkillTemplet->m_mapSkillAbility[ SA_SKILL_ID ] = fValue;
	if( true == luaManager.GetValue( L"SA_COMBO_ID", fValue ) )                 		pSkillTemplet->m_mapSkillAbility[ SA_COMBO_ID ] = fValue;
	if( true == luaManager.GetValue( L"SA_EFFECTIVE_RATE", fValue ) )           		pSkillTemplet->m_mapSkillAbility[ SA_EFFECTIVE_RATE ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_REL", fValue ) )                  		pSkillTemplet->m_mapSkillAbility[ SA_ATK_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_PHYSIC_REL", fValue ) )           		pSkillTemplet->m_mapSkillAbility[ SA_ATK_PHYSIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_MAGIC_REL", fValue ) )            		pSkillTemplet->m_mapSkillAbility[ SA_ATK_MAGIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_DEF_PHYSIC_REL", fValue ) )           		pSkillTemplet->m_mapSkillAbility[ SA_DEF_PHYSIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_DEF_MAGIC_REL", fValue ) )            		pSkillTemplet->m_mapSkillAbility[ SA_DEF_MAGIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_TEAM_ATK_PHYSIC_REL", fValue ) )      		pSkillTemplet->m_mapSkillAbility[ SA_TEAM_ATK_PHYSIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_TEAM_ATK_MAGIC_REL", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_TEAM_ATK_MAGIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_TEAM_DEF_PHYSIC_REL", fValue ) )      		pSkillTemplet->m_mapSkillAbility[ SA_TEAM_DEF_PHYSIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_TEAM_DEF_MAGIC_REL", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_TEAM_DEF_MAGIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_DAMAGE_REL", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_DAMAGE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_DAMAGED_REL", fValue ) )              		pSkillTemplet->m_mapSkillAbility[ SA_DAMAGED_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_UNFIXED_DEFENSE_REL", fValue ) )      		pSkillTemplet->m_mapSkillAbility[ SA_UNFIXED_DEFENSE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_SUMMON_ATK_REL", fValue ) )           		pSkillTemplet->m_mapSkillAbility[ SA_SUMMON_ATK_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_MAX_HP_ABS", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_MAX_HP_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_MAX_HP_REL", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_MAX_HP_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_MAX_MP_ABS", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_MAX_MP_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_MAX_MP_REL", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_MAX_MP_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_TELEPORT_RANGE_ABS", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_TELEPORT_RANGE_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_CHARGING_TIME", fValue ) )            		pSkillTemplet->m_mapSkillAbility[ SA_CHARGING_TIME ] = fValue;
	if( true == luaManager.GetValue( L"SA_HP_REL_TO_MP_ABS_1", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_HP_REL_TO_MP_ABS_1 ] = fValue;
	if( true == luaManager.GetValue( L"SA_HP_REL_TO_MP_ABS_2", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_HP_REL_TO_MP_ABS_2 ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_CONSUME_REL", fValue ) )           		pSkillTemplet->m_mapSkillAbility[ SA_MP_CONSUME_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_GAIN_ON_HIT_REL", fValue ) )       		pSkillTemplet->m_mapSkillAbility[ SA_MP_GAIN_ON_HIT_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_SCOPE_ABS", fValue ) )                		pSkillTemplet->m_mapSkillAbility[ SA_SCOPE_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_PROJECTILE_FIRE_COUNT", fValue ) )    		pSkillTemplet->m_mapSkillAbility[ SA_PROJECTILE_FIRE_COUNT ] = fValue;
	if( true == luaManager.GetValue( L"SA_PROJECTILE_RANGE_REL", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_PROJECTILE_RANGE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_PROJECTILE_SIZE", fValue ) )          		pSkillTemplet->m_mapSkillAbility[ SA_PROJECTILE_SIZE ] = fValue;
	if( true == luaManager.GetValue( L"SA_PROJECTILE_PIERCING", fValue ) )      		pSkillTemplet->m_mapSkillAbility[ SA_PROJECTILE_PIERCING ] = fValue;
	if( true == luaManager.GetValue( L"SA_MOVE_SPEED", fValue ) )               		pSkillTemplet->m_mapSkillAbility[ SA_MOVE_SPEED ] = fValue;
	if( true == luaManager.GetValue( L"SA_JUMP_SPEED", fValue ) )                   	pSkillTemplet->m_mapSkillAbility[ SA_JUMP_SPEED ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_PHYSIC_TO_ATK_MAGIC", fValue ) )      	pSkillTemplet->m_mapSkillAbility[ SA_ATK_PHYSIC_TO_ATK_MAGIC ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_MAGIC_TO_ATK_PHYSIC", fValue ) )      	pSkillTemplet->m_mapSkillAbility[ SA_ATK_MAGIC_TO_ATK_PHYSIC ] = fValue;
	if( true == luaManager.GetValue( L"SA_READY_TIME_REL", fValue ) )               	pSkillTemplet->m_mapSkillAbility[ SA_READY_TIME_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_FORCE_DOWN_REL", fValue ) )               	pSkillTemplet->m_mapSkillAbility[ SA_FORCE_DOWN_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_FORCE_DOWN_MELEE_REL", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_FORCE_DOWN_MELEE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_FORCE_DOWN_RANGE_REL", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_FORCE_DOWN_RANGE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_FORCE_DOWN_ABS", fValue ) )               	pSkillTemplet->m_mapSkillAbility[ SA_FORCE_DOWN_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_REGENERATION_ABS", fValue ) )          	pSkillTemplet->m_mapSkillAbility[ SA_MP_REGENERATION_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_INTAKE_ABS", fValue ) )                	pSkillTemplet->m_mapSkillAbility[ SA_MP_INTAKE_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_TARGET_ANI_SPEED", fValue ) )             	pSkillTemplet->m_mapSkillAbility[ SA_TARGET_ANI_SPEED ] = fValue;
	if( true == luaManager.GetValue( L"SA_TARGET_MOVE_SPEED", fValue ) )            	pSkillTemplet->m_mapSkillAbility[ SA_TARGET_MOVE_SPEED ] = fValue;
	if( true == luaManager.GetValue( L"SA_TARGET_JUMP_SPEED", fValue ) )            	pSkillTemplet->m_mapSkillAbility[ SA_TARGET_JUMP_SPEED ] = fValue;
	if( true == luaManager.GetValue( L"SA_TARGET_ATK_REL", fValue ) )               	pSkillTemplet->m_mapSkillAbility[ SA_TARGET_ATK_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_HP_INTAKE_REL_DAMAGE", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_HP_INTAKE_REL_DAMAGE ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_CRITICAL_DAM_ABS", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_ATK_CRITICAL_DAM_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_CRITICAL_RATE_ABS", fValue ) )        	pSkillTemplet->m_mapSkillAbility[ SA_ATK_CRITICAL_RATE_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_DEF_CRITICAL_DAM_ABS", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_DEF_CRITICAL_DAM_ABS ] = fValue;
	//{{ 2011-04 에 패치될 청 캐시 스킬
#ifdef	CASH_SKILL_FOR_CHUNG_2011_04
	if( true == luaManager.GetValue( L"SA_DEF_CRITICAL_RATE_ABS", fValue ) )        	pSkillTemplet->m_mapSkillAbility[ SA_DEF_CRITICAL_RATE_ABS ] = fValue;
#endif	CASH_SKILL_FOR_CHUNG_2011_04				
	//}} 2011-04 에 패치될 청 캐시 스킬
#ifdef GUILD_SKILL
	if( true == luaManager.GetValue( L"SA_BACK_SPEED_X", fValue ) )                 	pSkillTemplet->m_mapSkillAbility[ SA_BACK_SPEED_X ] = fValue;
	if( true == luaManager.GetValue( L"SA_ITEM_SPECIAL_ABILITY_REL", fValue ) )     	pSkillTemplet->m_mapSkillAbility[ SA_ITEM_SPECIAL_ABILITY_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_GAIN_GET_HIT_REL", fValue ) )          	pSkillTemplet->m_mapSkillAbility[ SA_MP_GAIN_GET_HIT_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_SOUL_GAIN_ON_HIT_REL", fValue ) )         	pSkillTemplet->m_mapSkillAbility[ SA_SOUL_GAIN_ON_HIT_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_SOUL_GAIN_GET_HIT_REL", fValue ) )        	pSkillTemplet->m_mapSkillAbility[ SA_SOUL_GAIN_GET_HIT_REL ] = fValue;

	if( true == luaManager.GetValue( L"SA_HP_GAIN_REL_MAX_HP", fValue ) )           	pSkillTemplet->m_mapSkillAbility[ SA_HP_GAIN_REL_MAX_HP ] = fValue;
	if( true == luaManager.GetValue( L"SA_ED_GAIN_REL", fValue ) )                  	pSkillTemplet->m_mapSkillAbility[ SA_ED_GAIN_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL", fValue ) )	pSkillTemplet->m_mapSkillAbility[ SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_EXP_GAIN_REL", fValue ) )                 	pSkillTemplet->m_mapSkillAbility[ SA_EXP_GAIN_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_DAMAGE_ABSORB_REL_MAX_HP", fValue ) )     	pSkillTemplet->m_mapSkillAbility[ SA_DAMAGE_ABSORB_REL_MAX_HP ] = fValue;
#endif GUILD_SKILL



	if( true == luaManager.GetValue( L"SA_STRONG_MIND_ACTIVE_REL", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_STRONG_MIND_ACTIVE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_STRONG_MIND_SPECIAL_ACTIVE_REL", fValue ) )   pSkillTemplet->m_mapSkillAbility[ SA_STRONG_MIND_SPECIAL_ACTIVE_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_MP_RETAKE_ABS", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_MP_RETAKE_ABS ] = fValue;


	if( true == luaManager.GetValue( L"SA_RESIST_FIRE_ABS", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_RESIST_FIRE_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_RESIST_WATER_ABS", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_RESIST_WATER_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_RESIST_GREEN_ABS", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_RESIST_GREEN_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_RESIST_WIND_ABS", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_RESIST_WIND_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_RESIST_LIGHT_ABS", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_RESIST_LIGHT_ABS ] = fValue;
	if( true == luaManager.GetValue( L"SA_RESIST_DARK_ABS", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_RESIST_DARK_ABS ] = fValue;

	if( true == luaManager.GetValue( L"SA_ATK_MAGIC_FIRST_REL", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_ATK_MAGIC_FIRST_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_MAGIC_SECOND_REL", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_ATK_MAGIC_SECOND_REL ] = fValue;
	if( true == luaManager.GetValue( L"SA_ATK_MAGIC_THIRD_REL", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_ATK_MAGIC_THIRD_REL ] = fValue;
	
	if( true == luaManager.GetValue( L"SA_REMOTE_SPEED_ABS", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_REMOTE_SPEED_ABS ] = fValue;

	if( true == luaManager.GetValue( L"SA_DAMAGE_TYPE_CHANGE", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_DAMAGE_TYPE_CHANGE ] = fValue;	
	if( true == luaManager.GetValue( L"SA_DAMAGE_TIME", fValue ) )     					pSkillTemplet->m_mapSkillAbility[ SA_DAMAGE_TIME ] = fValue;	

//{{ 김상훈 : 2010.11.12
#ifdef NEW_SKILL_2010_11
	if ( true == luaManager.GetValue( L"SA_RECOVER_MP_ABS", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_RECOVER_MP_ABS ] = fValue;	
	if ( true == luaManager.GetValue( L"SA_TARGET_MANA", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_TARGET_MANA ] = fValue;	
#endif NEW_SKILL_2010_11
//}} 김상훈 : 2010.11.12

	//{{ JHKang / 강정훈 / 2010/11/22
#ifdef NEW_SKILL_2010_11
	if ( true == luaManager.GetValue( L"SA_TARGET_MANA_REL", fValue ) )	pSkillTemplet->m_mapSkillAbility[ SA_TARGET_MANA_REL ] = fValue;
	if ( true == luaManager.GetValue( L"SA_SIZE_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_SIZE_ABS ] = fValue;
#endif NEW_SKILL_2010_11
	//}} JHKang / 강정훈 / 2010/11/22

#ifdef NEW_SKILL_2010_11 // oasis907 : 김상윤 [2010.11.24] 
	if ( true == luaManager.GetValue( L"SA_REFLECT_REL", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_REFLECT_REL ] = fValue;
	if ( true == luaManager.GetValue( L"SA_EL_DEFENCE_ABS", fValue ) )					pSkillTemplet->m_mapSkillAbility[ SA_EL_DEFENCE_ABS ] = fValue;
	if ( true == luaManager.GetValue( L"SA_TARGET_HP_REL", fValue ) )					pSkillTemplet->m_mapSkillAbility[ SA_TARGET_HP_REL ] = fValue;
	if ( true == luaManager.GetValue( L"SA_TARGET_HITRATE_REL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_TARGET_HITRATE_REL ] = fValue;
#endif NEW_SKILL_2010_11

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	if ( true == luaManager.GetValue( L"SA_CHARGE_CANNONBALL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_CHARGE_CANNON_BALL ] = fValue;
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

//{{ kimhc // 2011.1.14 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	if ( true == luaManager.GetValue( L"SA_FIRE_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_FIRE_DEFENCE_ABS ]			= fValue;
	if ( true == luaManager.GetValue( L"SA_WATER_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_WATER_DEFENCE_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_NATURE_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_NATURE_DEFENCE_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_WIND_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_WIND_DEFENCE_ABS ]			= fValue;
	if ( true == luaManager.GetValue( L"SA_LIGHT_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_LIGHT_DEFENCE_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_DARK_DEFENCE_ABS", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_DARK_DEFENCE_ABS ]			= fValue;

	if ( true == luaManager.GetValue( L"SA_EDT_FIRE_RESIST_REL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_EDT_FIRE_RESIST_REL ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_FROZEN_RESIST_REL", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_EDT_FROZEN_RESIST_REL ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_ICE_RESIST_REL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_EDT_ICE_RESIST_REL ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_POISON_RESIST_REL", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_EDT_POISON_RESIST_REL ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_Y_PRESSED_RESIST_REL", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_EDT_Y_PRESSED_RESIST_REL ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_LEG_WOUND_RESIST_REL", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_EDT_LEG_WOUND_RESIST_REL ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_STUN_RESIST_REL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_EDT_STUN_RESIST_REL ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_EDT_CURSE_RESIST_REL", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_EDT_CURSE_RESIST_REL ]		= fValue;

	if ( true == luaManager.GetValue( L"SA_EXPAND_CB", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_EXPAND_CB ]				= fValue;
	if ( true == luaManager.GetValue( L"SA_ANI_SPEED", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_ANI_SPEED ]				= fValue;
#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.14 // 청 1차 전직

	//{{ kimhc // 2011-04-01 // 2011-04 에 패치될 청 캐시 스킬
#ifdef	CASH_SKILL_FOR_CHUNG_2011_04
	if ( true == luaManager.GetValue( L"SA_ACCURACY_PERCENT_ABS", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_ACCURACY_PERCENT_ABS ]		= fValue;
#endif	CASH_SKILL_FOR_CHUNG_2011_04
	//}} kimhc // 2011-04-01 // 2011-04 에 패치될 청 캐시 스킬

#ifdef CHUNG_SECOND_CLASS_CHANGE
	if ( true == luaManager.GetValue( L"SA_CRITICAL_EVASION_REL", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_CRITICAL_EVASION_REL ]		= fValue;
#endif CHUNG_SECOND_CLASS_CHANGE

#ifdef CHUNG_SECOND_CLASS_CHANGE
	if ( true == luaManager.GetValue( L"SA_DECREASE_REL", fValue ) )					pSkillTemplet->m_mapSkillAbility[ SA_DECREASE_REL ]		= fValue;
#endif CHUNG_SECOND_CLASS_CHANGE

#ifdef ELSWORD_SHEATH_KNIGHT
	if ( true == luaManager.GetValue( L"SA_ATK_ADD_DAMAGE_FIRST_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_ATK_ADD_DAMAGE_FIRST_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_ATK_ADD_DAMAGE_SECOND_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_ATK_ADD_DAMAGE_SECOND_ABS ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_ATK_ADD_DAMAGE_THIRD_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_ATK_ADD_DAMAGE_THIRD_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_ATK_ADD_DAMAGE_FOURTH_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_ATK_ADD_DAMAGE_FOURTH_ABS ]	= fValue;
	if ( true == luaManager.GetValue( L"SA_ATK_ADD_DAMAGE_FIFTH_ABS", fValue ) )		pSkillTemplet->m_mapSkillAbility[ SA_ATK_ADD_DAMAGE_FIFTH_ABS ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_TELEPORT_SPEED_REL", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_TELEPORT_SPEED_REL ]	= fValue;
	if ( true == luaManager.GetValue( L"HP_CONSUME_REL", fValue ) )						pSkillTemplet->m_mapSkillAbility[ HP_CONSUME_REL ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_MP_BURN_ABS", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_MP_BURN_ABS ]		= fValue;
#endif ELSWORD_SHEATH_KNIGHT

#ifdef	SERV_TRAPPING_RANGER_TEST
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_01", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_01 ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_02", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_02 ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_03", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_03 ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_04", fValue ) )						pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_04 ]		= fValue;

	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_01_RATE", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_01_RATE ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_02_RATE", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_02_RATE ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_03_RATE", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_03_RATE ]		= fValue;
	if ( true == luaManager.GetValue( L"SA_SKILLDAMAGE_MULTIPLE_04_RATE", fValue ) )				pSkillTemplet->m_mapSkillAbility[ SA_SKILLDAMAGE_MULTIPLE_04_RATE ]		= fValue;
#endif	SERV_TRAPPING_RANGER_TEST

#ifdef RAVEN_WEAPON_TAKER
	if( true == luaManager.GetValue( L"SA_EVASION_REL", fValue ) )     					pSkillTemplet->m_mapSkillAbility[ SA_EVASION_REL ]				= fValue;
	if( true == luaManager.GetValue( L"SA_REDUCE_EDT_TIME_REL", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_REDUCE_EDT_TIME_REL ]		= fValue;
	if( true == luaManager.GetValue( L"SA_OVERHEAT_HP", fValue ) )     					pSkillTemplet->m_mapSkillAbility[ SA_OVERHEAT_HP ]				= fValue;
#endif RAVEN_WEAPON_TAKER

#ifdef EVE_ELECTRA
	if( true == luaManager.GetValue( L"SA_SCOPE_ABS_HYPER", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_SCOPE_ABS_HYPER ]			= fValue;
	if( true == luaManager.GetValue( L"SA_EFFECTIVE_TIME_HYPER", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_EFFECTIVE_TIME_HYPER ]		= fValue;
	if( true == luaManager.GetValue( L"SA_AIR_MOVE_COUNT", fValue ) )     				pSkillTemplet->m_mapSkillAbility[ SA_AIR_MOVE_COUNT ]			= fValue;
#endif EVE_ELECTRA

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	if( true == luaManager.GetValue( L"SA_NOT_CANNONBALL_REL", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_NOT_CANNONBALL_REL ]			= fValue;
	if( true == luaManager.GetValue( L"SA_INSERT_CANNONBALL_REL", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_INSERT_CANNONBALL_REL ]			= fValue;
	if( true == luaManager.GetValue( L"SA_BE_SHOT_CANNONBALL_REL", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_BE_SHOT_CANNONBALL_REL ]			= fValue;
	if( true == luaManager.GetValue( L"SA_CANNONBALL_DAMAGE_UP_REL", fValue ) )     	pSkillTemplet->m_mapSkillAbility[ SA_CANNONBALL_DAMAGE_UP_REL ]			= fValue;
	if( true == luaManager.GetValue( L"SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE", fValue ) )  pSkillTemplet->m_mapSkillAbility[ SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE ]			= fValue;
	if( true == luaManager.GetValue( L"SA_SIEGE_SHELLING_REL", fValue ) )     			pSkillTemplet->m_mapSkillAbility[ SA_SIEGE_SHELLING_REL ]			= fValue;
	if( true == luaManager.GetValue( L"SA_EFFECTIVE_DAMAGED_ATTACK_BASE", fValue ) )    pSkillTemplet->m_mapSkillAbility[ SA_EFFECTIVE_DAMAGED_ATTACK_BASE ]			= fValue;
#endif

#ifdef SERV_CHUNG_TACTICAL_TROOPER
	if( true == luaManager.GetValue( L"SA_FIRST_HYPER_EFFECTIVE", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_FIRST_HYPER_EFFECTIVE ]			= fValue;
	if( true == luaManager.GetValue( L"SA_SECOND_HYPER_EFFECTIVE", fValue ) )     		pSkillTemplet->m_mapSkillAbility[ SA_SECOND_HYPER_EFFECTIVE ]			= fValue;
	if( true == luaManager.GetValue( L"SA_THIRD_HYPER_EFFECTIVE", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_THIRD_HYPER_EFFECTIVE ]			= fValue;
#endif SERV_CHUNG_TACTICAL_TROOPER

#ifdef BALANCE_CODE_NEMESIS_20121213
	if( true == luaManager.GetValue( L"SA_ATK_CRITICAL_RATE_ATK_BASE", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_ATK_CRITICAL_RATE_ATK_BASE ]	= fValue;
#endif //BALANCE_CODE_NEMESIS_20121213
	if( true == luaManager.GetValue( L"SA_PROJECTILE_CURVE_SPEED_REL", fValue ) )			pSkillTemplet->m_mapSkillAbility[ SA_PROJECTILE_CURVE_SPEED_REL ]	= fValue;	
	if( true == luaManager.GetValue( L"SA_DP_GAIN_REL", fValue ) )							pSkillTemplet->m_mapSkillAbility[ SA_DP_GAIN_REL ]	= fValue;	
	if( true == luaManager.GetValue( L"SA_DP_USE_REL", fValue ) )							pSkillTemplet->m_mapSkillAbility[ SA_DP_USE_REL ]	= fValue;	

	CX2BuffTempletManager::GetInstance()->GetBuffFactorPtrFromBuffFactorList( luaManager, pSkillTemplet->m_vecBuffFactorPtr, pSkillTemplet->m_vecBuffFactorID );

#endif // UPGRADE_SKILL_SYSTEM_2013

	return true;
}

#ifdef LUA_TRANS_DEVIDE
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
bool CX2SkillTree::AddSkillTempletTrans_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
	
	SkillTemplet* pSkillTemplet = new SkillTemplet();
	
	LUA_GET_VALUE_ENUM( luaManager,		"m_eID",						pSkillTemplet->m_eID,							SKILL_ID,			SI_NONE					);
	LUA_GET_VALUE_ENUM( luaManager,		"m_eType",						pSkillTemplet->m_eType,							SKILL_TYPE,			ST_NONE					);
	LUA_GET_VALUE( luaManager,			"m_wstrName",					pSkillTemplet->m_wstrName,						L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrMainDesc",				pSkillTemplet->m_wstrMainDesc,					L""		);
	if( true == luaManager.BeginTable( L"m_EffectiveDesc" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecEffectiveDesc		);
	
	// 여기서 치환 해주어야 함.
	SkillTempletMap::iterator mit;
	mit = m_mapSkillTemplet.find( pSkillTemplet->m_eID );
	if( mit == m_mapSkillTemplet.end() )
	{
		wstringstream wstrStream;
		wstrStream << L"NewSkillTempletVer2Trans.lua And NewSkillTempletVer2.lua Not Equal m_eID..." << (pSkillTemplet->m_eID) << L"_";
		ErrorLogMsg( XEM_ERROR105, wstrStream.str().c_str() );	
		// 에러로그 출력
		SAFE_DELETE(pSkillTemplet); 
		return false;	
	}
	else
	{
		mit->second->m_eType = pSkillTemplet->m_eType;
		mit->second->m_wstrName = pSkillTemplet->m_wstrName;		
		mit->second->m_wstrMainDesc = pSkillTemplet->m_wstrMainDesc;		
		mit->second->m_vecEffectiveDesc = pSkillTemplet->m_vecEffectiveDesc;		
	}

	return true;
}
#else //UPGRADE_SKILL_SYSTEM_2013
bool CX2SkillTree::AddSkillTempletTrans_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );

	SkillTemplet* pSkillTemplet = new SkillTemplet();

	LUA_GET_VALUE( luaManager,		"m_bGuildSkill",			pSkillTemplet->m_bGuildSkill,		false		);
	LUA_GET_VALUE_ENUM( luaManager, "m_eID",					pSkillTemplet->m_eID,				SKILL_ID,					SI_NONE				);
	LUA_GET_VALUE_ENUM( luaManager, "m_eType",					pSkillTemplet->m_eType,				SKILL_TYPE,					ST_NONE				);
	LUA_GET_VALUE( luaManager,		"m_Name",					pSkillTemplet->m_Name,				L""												);
	LUA_GET_VALUE( luaManager,		"m_Desc",					pSkillTemplet->m_Desc,				L""												);
	LUA_GET_VALUE( luaManager,		"m_iSkillLevel",			pSkillTemplet->m_iSkillLevel,		1												);	

	// 여기서 치환 해주어야 함.
	SkillKey skillKey;
	skillKey.m_eID			= pSkillTemplet->m_eID;
	skillKey.m_iSkillLevel	= pSkillTemplet->m_iSkillLevel;
	SkillTempletMap::iterator mit;
	mit = m_mapSkillTemplet.find( skillKey );
	if( mit == m_mapSkillTemplet.end() )
	{
		wstringstream wstrStream;
		wstrStream << L"NewSkillTempletTrans.lua And NewSkillTemplet.lua Not Equal m_eID&&m_iSkillLevel..." << (pSkillTemplet->m_Desc) << L"_";
		ErrorLogMsg( XEM_ERROR105, wstrStream.str().c_str() );	
		// 에러로그 출력
		SAFE_DELETE(pSkillTemplet); 
		return false;
	}
	else
	{
		mit->second->m_bGuildSkill = pSkillTemplet->m_bGuildSkill;
		mit->second->m_eType = pSkillTemplet->m_eType;
		mit->second->m_Name = pSkillTemplet->m_Name;
		mit->second->m_Desc = pSkillTemplet->m_Desc;
	}

	return true;
}
#endif //UPGRADE_SKILL_SYSTEM_2013
#endif LUA_TRANS_DEVIDE

//{{ 2009. 8. 5  최육사		봉인 스킬 아이템 정보
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

bool CX2SkillTree::AddSealSkillInfo_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	int							iUnsealItemID = 0;					/// 아이템 아이디
	CX2Unit::UNIT_CLASS_TYPE	eUnitClassType = CX2Unit::UCT_NONE;	/// 사용 가능한 전직 클래스
	vector<SKILL_ID>			vecSkillID;							/// 모든 전직별 봉인 스킬 아이디 저장 컨테이너

	/// 아이템 아이디 파싱
	LUA_GET_VALUE( luaManager,		"m_iItemID",		iUnsealItemID,		0 );

	/// 사용 가능한 전직 클래스 파싱
	LUA_GET_VALUE_ENUM( luaManager, "m_eUnitClassType",	eUnitClassType,		CX2Unit::UNIT_CLASS_TYPE,		CX2Unit::UCT_NONE		);
	
	/// 봉인 스킬 아이디 파싱해서 컨테이너에 저장
	if( true == luaManager.BeginTable( "m_SkillID" ) )
	{
		int iSkillID	= 0;	/// 스킬 아이디
		int iTableIndex = 1;	/// 테이블 인덱스

		while( luaManager.GetValue( iTableIndex, iSkillID ) == true )
		{
			vecSkillID.push_back( static_cast<SKILL_ID>( iSkillID ) );
			++iTableIndex;
		}

		luaManager.EndTable();
	}

	/// 맵에 저장
	SealSkillItemInfo kInfo;
	kInfo.m_iUnitClassType	= static_cast<int>( eUnitClassType );
	kInfo.m_vecSkillID		= vecSkillID;
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	/// June 3220a350 takes m_iItemIDList (several unseal items share one group; each is a key of the seal map). June's data only uses
	/// the list. March's data only has the scalar m_iItemID, so an empty list falls back to it until Phase S3 flips the data.
	{
		vector<int> vecUnsealItemID;
		if( true == luaManager.BeginTable( "m_iItemIDList" ) )
		{
			int iListItemID = 0;
			int iListIndex = 1;
			while( luaManager.GetValue( iListIndex, iListItemID ) == true )
			{
				vecUnsealItemID.push_back( iListItemID );
				++iListIndex;
			}
			luaManager.EndTable();
		}
#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM
		/// June 3220a350 reads only m_iItemIDList; the scalar fallback served March's data until S3
#else SERV_IRUHADEV_JUNE_SKILL_REFORM
		if( true == vecUnsealItemID.empty() )
			vecUnsealItemID.push_back( iUnsealItemID );
#endif SERV_IRUHADEV_JUNE_SKILL_REFORM

		for( size_t iItem = 0; iItem < vecUnsealItemID.size(); ++iItem )
			m_mapSealSkillItemTemplet.insert( std::make_pair( vecUnsealItemID[iItem], kInfo ) );
		return true;
	}
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	m_mapSealSkillItemTemplet.insert( std::make_pair( iUnsealItemID, kInfo ) );
	return true;
}

#else // UPGRADE_SKILL_SYSTEM_2013

bool CX2SkillTree::AddSealSkillInfo_LUA( int iUnsealItemID, int iSkillID, char cUnitClass )
{
	if( iUnsealItemID <= 0  ||  iSkillID <= 0  ||  cUnitClass <= 0 )
		return false;

	SealSkillItemInfo kInfo;
	kInfo.m_iSkillID = iSkillID;
	kInfo.m_cUnitClass = cUnitClass;
	m_mapSealSkillItemTemplet.insert( std::make_pair( iUnsealItemID, kInfo ) );
	return true;
}

#endif // UPGRADE_SKILL_SYSTEM_2013
//}}



const CX2SkillTree::SkillTreeTemplet* CX2SkillTree::GetSkillTreeTemplet( int iUnitClass, SKILL_ID eSkillID )
{
	KTDXPROFILE();

	UnitSkillTreeTempletMap::iterator it = m_mapUnitSkillTreeTemplet.find( iUnitClass );
	if( it == m_mapUnitSkillTreeTemplet.end() )
		return 0;

	SkillTreeTempletMap& mapSkillTreeTemplet = it->second;
	SkillTreeTempletMap::iterator itSkillTreeTemplet = mapSkillTreeTemplet.find( eSkillID );
	if( itSkillTreeTemplet == mapSkillTreeTemplet.end() )
		return 0;

	return &itSkillTreeTemplet->second;
}
// oasis907 : 김상윤 [2009.11.25] // 본섭 파싱 위해 주석 해제
#ifdef GUILD_SKILL
//{{ oasis907 : 김상윤 //// 2009-11-16 //// 
const CX2SkillTree::SkillTreeTemplet* CX2SkillTree::GetGuildSkillTreeTemplet( int iGuildClass, SKILL_ID eSkillID )
{
	KTDXPROFILE();

	if(iGuildClass != 0)
	{
		ASSERT( !"Guild Class Not Implemented Yet!!" );
	}

	GuildSkillTreeTempletMap::iterator it = m_mapGuildSkillTreeTemplet.find( iGuildClass );
	if( it == m_mapGuildSkillTreeTemplet.end() )
		return 0;

	SkillTreeTempletMap& mapSkillTreeTemplet = it->second;
	SkillTreeTempletMap::iterator itSkillTreeTemplet = mapSkillTreeTemplet.find( eSkillID );
	if( itSkillTreeTemplet == mapSkillTreeTemplet.end() )
		return 0;

	return &itSkillTreeTemplet->second;
}
//}} oasis907 : 김상윤 //// 2009-11-16 //// 
#endif GUILD_SKILL

int CX2SkillTree::GetMaxSkillLevel( int iUnitClass, SKILL_ID eSkillID )
{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
#ifndef _SERVICE_
	/// 개발자 기능 - 30레벨까지 적용 가능하도록 설정
	if( true == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
	{
		return MAX_LIMITED_SKILL_LEVEL;
	}
#endif _SERVICE_
#endif // UPGRADE_SKILL_SYSTEM_2013

	const SkillTreeTemplet* pSkillTreeTemplet = GetSkillTreeTemplet( iUnitClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return 0;

	return pSkillTreeTemplet->m_iMaxSkillLevel;
}
#ifdef GUILD_SKILL
//{{ oasis907 : 김상윤 //// 2009-11-16 //// 
int CX2SkillTree::GetMaxGuildSkillLevel( int iGuildClass, SKILL_ID eSkillID )
{
	const SkillTreeTemplet* pSkillTreeTemplet = GetGuildSkillTreeTemplet( iGuildClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return 0;

	return pSkillTreeTemplet->m_iMaxSkillLevel;
}
//}} oasis907 : 김상윤 //// 2009-11-16 //// 
#endif GUILD_SKILL

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

const CX2SkillTree::SkillTemplet* CX2SkillTree::GetSkillTemplet( SKILL_ID eSkillID ) const
{
	KTDXPROFILE();

#ifdef EFFECT_TOOL // 이펙트 툴에서 스킬 템플릿 열지 않도록 변경
	return NULL;
#endif // EFFECT_TOOL

	SkillTempletMap::const_iterator it = m_mapSkillTemplet.find( eSkillID );
	if( m_mapSkillTemplet.end() != it )
	{
		SkillTemplet* pSkillTemplet = it->second;
		return pSkillTemplet;
	}

	if ( CX2SkillTree::SI_NONE != eSkillID )		/// 스킬 템플릿에 없으며, 스킬 아이디가 0도 아닐 때
		/// 이상한 스킬 아이디로 스킬 템플릿을 찾으려 해요!!
		ASSERT( !L"Not Find Skill Templet From Unknown SKill ID!!!" );
	

	return NULL;
}

#else // UPGRADE_SKILL_SYSTEM_2013

const CX2SkillTree::SkillTemplet* CX2SkillTree::GetSkillTemplet( SKILL_ID eSkillID, int iSkillLevel ) const
{
	SkillKey skillKey;
	skillKey.m_eID			= eSkillID;
	skillKey.m_iSkillLevel	= iSkillLevel;

	SkillTempletMap::const_iterator it = m_mapSkillTemplet.find( skillKey );
	if( m_mapSkillTemplet.end() != it )
	{
		SkillTemplet* pSkillTemplet = it->second;
		return pSkillTemplet;
	}

	return NULL;
}

#endif // UPGRADE_SKILL_SYSTEM_2013

// 09.05.22 태완
const CX2SkillTree::SkillTreeTempletMap& CX2SkillTree::GetSkillTreeTempletMap( int eUnitClass )
{
	UnitSkillTreeTempletMap::iterator it = m_mapUnitSkillTreeTemplet.find(eUnitClass);
	if( it != m_mapUnitSkillTreeTemplet.end() )
	{
		return it->second;
	}

	ASSERT( !"UnitClass Not Found!!" );
	it = m_mapUnitSkillTreeTemplet.begin();
	return it->second;

}

#ifdef X2TOOL
CX2SkillTree::SkillTreeTempletMap& CX2SkillTree::GetSkillTreeTempletMap4Tool( int eUnitClass )
{
	UnitSkillTreeTempletMap::iterator it = m_mapUnitSkillTreeTemplet.find(eUnitClass);
	if( it != m_mapUnitSkillTreeTemplet.end() )
	{
		return it->second;
	}

	ASSERT( !"UnitClass Not Found!!" );
	it = m_mapUnitSkillTreeTemplet.begin();
	return it->second;
}
#endif

bool CX2SkillTree::IsUnitTypeDefaultSkill( SKILL_ID eSkillID )
{
	switch( eSkillID )
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	case SI_A_ES_MEGASLASH:
	case SI_A_AV_CHAIN_FIRE_BALL:
	case SI_A_LE_RAIL_STINGER:
	case SI_A_RF_MAXIMUM_CANNON:
	case SI_A_EN_ILLUSION_STRIKE:
#else //UPGRADE_SKILL_SYSTEM_2013
	case SI_SA_ES_MEGASLASH:
	case SI_SA_AV_CHAIN_FIRE_BALL:
	case SI_SA_LE_RAIL_STINGER:
	case SI_SA_RF_MAXIMUM_CANNON:
	case SI_SA_EN_ILLUSION_STRIKE:
#endif //UPGRADE_SKILL_SYSTEM_2013
#ifdef ELSWORD_SHEATH_KNIGHT
	case SI_A_ES_ROLLING:
#else
	case SI_A_ES_COUNTER:
#endif ELSWORD_SHEATH_KNIGHT
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
	case SI_A_AV_FIRE_ROAD:
#else
	case SI_A_AV_TELEPORT:
#endif
#ifdef SERV_TRAPPING_RANGER_TEST
	case SI_A_LE_RISING_FALCON:
#else
	case SI_A_LE_SIEGEMODE:
#endif SERV_TRAPPING_RANGER_TEST
#ifdef RAVEN_WEAPON_TAKER
	case SI_A_RF_BREAKING_FIST:
#else
	case SI_A_RF_BURNING_RUSH:
#endif RAVEN_WEAPON_TAKER
#ifdef EVE_ELECTRA
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		case SI_A_EN_PHOTON_BLINK:
#else //UPGRADE_SKILL_SYSTEM_2013
		case SI_A_EVE_PHOTON_BLINK:
#endif //UPGRADE_SKILL_SYSTEM_2013
#else
	case SI_A_EN_CLOAKING:
#endif EVE_ELECTRA
#ifdef ARA_CHARACTER_BASE
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	case SI_A_AM_WIND_WEDGE:
	case SI_A_AM_GAIN_FORCE:
#else //UPGRADE_SKILL_SYSTEM_2013
	case SI_SA_ARA_WIND_WEDGE:
	case SI_A_ARA_GAIN_FORCE:
#endif //UPGRADE_SKILL_SYSTEM_2013
#endif
		{
			return true;
		} break;
	}

	return false;
}


bool CX2SkillTree::AddGuildSkillTreeTemplet_LUA()

{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "m_iSkillID",				eSkillID,				SKILL_ID,					SI_NONE					);

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID, 1 );
#endif // UPGRADE_SKILL_SYSTEM_2013
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}

	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"m_iMasterLevel",			skillTreeTemplet.m_iMaxSkillLevel,		0			);
	LUA_GET_VALUE( luaManager,		"m_iRowIndex",				skillTreeTemplet.m_iTier,				0			);
	LUA_GET_VALUE( luaManager,		"m_iColumnIndex",			skillTreeTemplet.m_iIndexInTier,		0			);

	int iPrecedingSkillID = 0;
	LUA_GET_VALUE( luaManager,		"m_iPrecedingSkill",		skillTreeTemplet.m_iPrecedingSkill,		0			);

	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapGuildSkillTreeTemplet[ 0 ];

#else // UPGRADE_SKILL_SYSTEM_2013

	int eGuildClass = 0;
	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "GUILD_CLASS",				eGuildClass,				int,						0			);
	LUA_GET_VALUE_ENUM( luaManager, "SKILL_ID",					eSkillID,				SKILL_ID,					SI_NONE					);



	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID, 1 );
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}


	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"MAX_SKILL_LEVEL",			skillTreeTemplet.m_iMaxSkillLevel,	0				);
	LUA_GET_VALUE( luaManager,		"TIER_INDEX",				skillTreeTemplet.m_iTier,			0				);
	LUA_GET_VALUE( luaManager,		"INDEX_IN_TIER",			skillTreeTemplet.m_iIndexInTier,	0				);



	std::wstring wstrPrecedingSkill = L"";
	LUA_GET_VALUE( luaManager,		"m_vecPrecedingSkill",		wstrPrecedingSkill, 		L""		);

	if( false == wstrPrecedingSkill.empty() )
	{
		WCHAR* pBegin = const_cast<WCHAR*>(wstrPrecedingSkill.c_str());
		WCHAR* pEnd = pBegin + wstrPrecedingSkill.length();


		while( true )
		{
			int iSkillID = TokenizeInt( &pBegin, pEnd );
			if( iSkillID > 0 )
				skillTreeTemplet.m_vecPrecedingSkill.push_back( iSkillID );
			else 
				break;
		}
	}


	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapGuildSkillTreeTemplet[ (int)eGuildClass ];

#endif // UPGRADE_SKILL_SYSTEM_2013

	mapSkillTreeTemplet[ eSkillID ] = skillTreeTemplet;


	return true;

}



#ifdef GUILD_SKILL
//{{ oasis907 : 김상윤 //// 2009-11-12 //// 

const CX2SkillTree::SkillTreeTempletMap& CX2SkillTree::GetGuildSkillTreeTempletMap( int eGuildClass )
{
	GuildSkillTreeTempletMap::iterator it = m_mapGuildSkillTreeTemplet.find(eGuildClass);
	if( it != m_mapGuildSkillTreeTemplet.end() )
	{
		return it->second;
	}
	/*
	//{{ oasis907 : 김상윤 //// 2009-11-17 //// 임시
	ASSERT( !"GuildClass Not Found!!" );
	*/
	return nulltemplet;


	it = m_mapGuildSkillTreeTemplet.begin();
	return it->second;

}
//}} oasis907 : 김상윤 //// 2009-11-12 //// 
#endif GUILD_SKILL


//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
#ifdef PRINT_INGAMEINFO_TO_EXCEL
/*
void CX2SkillTree::PrintSkillInfo_ToExcel()
{
	SkillTemplet* pSkillTemplet = NULL;
	map<SkillKey, SkillTemplet*>::iterator iter;
	wofstream of(L"스킬 목록.txt");
	of.imbue(std::locale("kor",locale::ctype));

	for( iter = m_mapSkillTemplet.begin(); iter != m_mapSkillTemplet.end(); iter++ )
	{
		pSkillTemplet = iter->second;
		of << endl << pSkillTemplet->m_eID << endl << pSkillTemplet->m_Name << endl << endl << pSkillTemplet->m_Desc <<endl;
		of << L"----------------------------------------------------------------------------------------"<< endl;
	}

	of.close();
}
*/

void CX2SkillTree::PrintSkillInfo_ToExcel()
{
	BasicExcel e;
	e.New(1);
	
	BasicExcelWorksheet* sheet = e.GetWorksheet((size_t)0);
	int cal = 1;
	SkillTemplet* pSkillTemplet = NULL;
	map<SkillKey, SkillTemplet*>::iterator iter;
	
	// Title 출력
	sheet->Cell(0,0)->SetWString(L"스킬 ID");	
	sheet->Cell(0,1)->SetWString(L"스킬 이름");	
	sheet->Cell(0,2)->SetWString(L"스킬 설명");	

	// 내용 출력
	for( iter = m_mapSkillTemplet.begin(); iter != m_mapSkillTemplet.end(); iter++ )
	{
		pSkillTemplet = iter->second;
		if(sheet)
		{
			sheet->Cell(cal,0)->SetInteger(pSkillTemplet->m_eID);
			sheet->Cell(cal,1)->SetWString(pSkillTemplet->m_wstrName.c_str());
			sheet->Cell(cal,2)->SetWString(pSkillTemplet->m_wstrMainDesc.c_str());	
		}
		cal++;
	}
	e.SaveAs("SKILL_LIST.xls");
}
#endif PRINT_INGAMEINFO_TO_EXCEL
//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

//////////////////////////////////////////////////////////////////////////
bool CX2SkillTree::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{
	case EGS_RESET_SKILL_ACK:
		{
			return Handler_EGS_RESET_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_INIT_SKILL_TREE_ACK:
		{
			return Handler_EGS_INIT_SKILL_TREE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GET_SKILL_ACK:
		{
			return Handler_EGS_GET_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_SKILL_SLOT_ACK:
		{
			return Handler_EGS_CHANGE_SKILL_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_SKILL_SLOT_NOT:
		{
			return Handler_EGS_CHANGE_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPIRE_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_UPDATE_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPAND_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_EXPIRE_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPIRE_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

#ifdef SERV_SKILL_SLOT_CHANGE_INVENTORY
	case EGS_SKILL_SLOT_CHANGE_ITEM_NOT:
		{
			return Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( hWnd, uMsg, wParam, lParam );
		} break;
#endif SERV_SKILL_SLOT_CHANGE_INVENTORY

	case EGS_UNSEAL_SKILL_NOT:
		{
			return Handler_EGS_UNSEAL_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;

#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	case EGS_JUNE_ACQUIRE_SKILL_NOT:
		{
			return Handler_EGS_JUNE_ACQUIRE_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;
#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	case EGS_JUNE_ACTIVE_SKILL_MEMO_ACK:
		{
			return Handler_EGS_JUNE_ACTIVE_SKILL_MEMO_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK:
		{
			return Handler_EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK( hWnd, uMsg, wParam, lParam );
		} break;
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

	case EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK:
		{
			Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( hWnd, uMsg, wParam, lParam );
			return true;
		} break;
#ifdef GUILD_SKILL

	case EGS_RESET_GUILD_SKILL_ACK:
		{
			return Handler_EGS_RESET_GUILD_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_INIT_GUILD_SKILL_ACK:
		{
			return Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GET_GUILD_SKILL_ACK:
		{
			return Handler_EGS_GET_GUILD_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_RESET_GUILD_SKILL_NOT:
		{
			return Handler_EGS_RESET_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_INIT_GUILD_SKILL_NOT:
		{
			return Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( hWnd, uMsg, wParam, lParam );
		} break;



	case EGS_GET_GUILD_SKILL_NOT:
		{
			return Handler_EGS_GET_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;


/*
	case EGS_CHANGE_GUILD_SKILL_SLOT_ACK:
		{
			return Handler_EGS_CHANGE_GUILD_SKILL_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_CHANGE_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

*/

	case EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;
/*
	case EGS_UPDATE_CASH_GUILD_SKILL_POINT_NOT:
		{
			return Handler_EGS_UPDATE_CASH_GUILD_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPAND_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_EXPIRE_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPIRE_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_UNSEAL_GUILD_SKILL_NOT:
		{
			return Handler_EGS_UNSEAL_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;
*/

//}} oasis907 : 김상윤 //// 2009-11-12 //// 
#endif GUILD_SKILL

#ifdef SKILL_PAGE_SYSTEM //JHKang
	case EGS_GET_NEXT_SKILL_PAGE_ED_ACK:
		{
			return Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_SKILL_PAGE_ACK:
		{
			return Handler_EGS_EXPAND_SKILL_PAGE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK:
		{
			return Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( hWnd, uMsg, wParam, lParam );
		} break;
#endif //SKILL_PAGE_SYSTEM
	}

	return false;
}




bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID )
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	/// June 3220e3b0 whole: the checks, then the change is queued. JuneFrameMoveSkillSlotQueue ( 322105c0 ) sends it ( 3220e5f0 ).
	/// clock() - instance +0x14c over 1 s ( _DAT_334355b0 = 1.0, divisor 1000 ) = IsElapsedPacketTiming's default
	if( false == g_pInstanceData->IsElapsedPacketTiming() )
		return false;

	/// instance +0x191 = the game-start REQ already sent: accepted and dropped
	if( true == g_pInstanceData->GetSendGameStartReqPacket() )
		return true;

	if( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	/// In a game: a slot change still cooling down ( 3221c560 reads +0x939c ) is refused only above level 10 ( vslot 0x20c )
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
		false == g_pX2Game->GetMyUnit()->IsValideSlotChange() && 10 < g_pX2Game->GetMyUnit()->GetUnitLevel() )
		return false;

	/// B slots 5-8 need slot-state key 0, the B-TC slot 9 key 1; an unequip ( skill 0 ) always passes
	const CX2UserSkillTree& kJuneUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	const bool bJuneSlotB = ( 5 <= iSlotID && iSlotID <= 8 );
	if( ( false == kJuneUserSkillTree.IsJuneSlotUsable( 0 ) && true == bJuneSlotB && 0 != static_cast<int>( eSkillID ) ) ||
		( false == kJuneUserSkillTree.IsJuneSlotUsable( 1 ) && 9 == iSlotID && 0 != static_cast<int>( eSkillID ) ) )
		return false;

	/// the same ( slot, skill ) already queued: accepted, not queued twice
	const std::pair< int, int > pairJuneChange( iSlotID, static_cast<int>( eSkillID ) );
	if( m_deqJuneSkillSlotChange.end() != std::find( m_deqJuneSkillSlotChange.begin(), m_deqJuneSkillSlotChange.end(), pairJuneChange ) )
		return true;

	m_deqJuneSkillSlotChange.push_back( pairJuneChange );	/// 3221d830
	return true;
#else SERV_IRUHADEV_JUNE_SKILL_ACQUIRE

	// 게임 시작 직후에 skill slot 바꾸기를 시도하면 안되게 막는다. 버그 플레이 때문에. 
	// 장착스킬 정보를 방에 들어왔을 때 받고, 게임 시작 직후에 다시 받는데, 
	// 스킬 사용시에 MP 소모량 정보가 게임시작전에는 변경이 되는데 게임시작 후에는 갱신이 안되기 때문에 생기는 문제
	if( false == g_pInstanceData->IsElapsedPacketTiming() )
	{
		return false;
	}
	//g_pInstanceData->GetTimerPacketTiming().restart();


//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
#ifdef MODIFY_INFINITE_SKILL_BUG
	// Change_state_Game_start_req를 전송하면 직후에는 던전 게임이 시작 할 때 까지 스킬슬롯을 바꿀수 없음
	if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
		return true;
#endif MODIFY_INFINITE_SKILL_BUG
//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

	if ( g_pData->GetMyUser() == NULL )
		return false;

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return false;
	
#ifdef FIX_SKILL_SLOT_CHANGE_BUG
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
		false == g_pX2Game->GetMyUnit()->IsValideSlotChange() ) // 슬롯 변경 유효 여부 검사
	{
		return false;
	}
#endif // FIX_SKILL_SLOT_CHANGE_BUG

	KEGS_CHANGE_SKILL_SLOT_REQ kPacket;
	kPacket.m_iSlotID = iSlotID;
	kPacket.m_iSkillID = static_cast<int>(eSkillID);

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_SKILL_SLOT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_SKILL_SLOT_ACK );

	//{{ kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	SetSkillSlotChanging( true );			// 스킬슬롯체인지 중
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	//}} kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정

#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	return true;
}


bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_SKILL_SLOT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );	

	g_pMain->DeleteServerPacket( EGS_CHANGE_SKILL_SLOT_ACK );

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{

// #ifdef FIX_SKILL_SLOT_CHANGE_BUG
// 		if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() && 
// 			false == g_pX2Game->GetMyUnit()->IsValideSlotChange() ) // 슬롯 변경 유효 여부 검사
// 		{
// 			return true;
// 		}
// #endif // FIX_SKILL_SLOT_CHANGE_BUG


		CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
#ifdef MODIFY_INFINITE_SKILL_BUG
		// 2010-03-23 // Change_state_Game_Start_Not를 받은 직후에는 바꿀 수 없음
		if ( g_pInstanceData->GetReceiveGameStartNotPacket() == false )
#endif MODIFY_INFINITE_SKILL_BUG
//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정
		{
			pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID, 
				static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iSkillID ) );

#ifdef SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE
			if ( -1 != kEvent.m_iSlotID2 )
			{
				pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID2, 
					static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iSkillID2 ) );
			}
#endif // SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE


			if( NULL != g_pData->GetUIManager()  )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
			}

			//{{ kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
			SetSkillSlotChanging( false );			// 스킬슬롯체인지 완료
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
			//}} kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
		}

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
		{
#ifdef FIX_SKILL_SLOT_CHANGE_BUG
			g_pX2Game->GetMyUnit()->SetCanNotInputTime_LUA(0.5f); // 일정 시간 스킬 사용 불가 상태로 변경
#endif // FIX_SKILL_SLOT_CHANGE_BUG
			g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( kEvent.m_iSlotID, kEvent.m_iSkillID );

#ifdef SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE
			if ( -1 != kEvent.m_iSlotID2 )
			{
				g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( kEvent.m_iSlotID2, kEvent.m_iSkillID2 );
			}
#endif // SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE
 		}

#ifdef FIX_SKILL_SLOT_COOLTIME_IN_VILLAGE //2013.08.09
		if ( NULL != g_pTFieldGame && NULL != g_pData->GetMyUser() )
		{
			g_pData->GetMyUser()->ChangeEquippedSkillState( kEvent.m_iSlotID, kEvent.m_iSkillID );

			#ifdef SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE
			if ( -1 != kEvent.m_iSlotID2 )
			{
				g_pData->GetMyUser()->ChangeEquippedSkillState( kEvent.m_iSlotID2, kEvent.m_iSkillID2 );
			}
			#endif // SERV_SKILL_SLOT_CHANGE_PACKET_INTEGRATE
		}
#endif //FIX_SKILL_SLOT_COOLTIME_IN_VILLAGE
	}
	return true;
}


bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );	


	// 게임 룸 상태일 경우 처리
	CX2Unit::UnitData* pUnitData = NULL;
	if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_ROOM )
	{
		CX2StatePVPRoom* pkStatePVPRoom = (CX2StatePVPRoom*) g_pMain->GetNowState();
		CX2Room::SlotData* pkSlotData = g_pX2Room->GetSlotDataByUnitUID( kEvent.m_iUnitUID );
		if ( pkSlotData != NULL )
		{
			pUnitData = &pkSlotData->m_pUnit->AccessUnitData();
		}
	}

	if ( pUnitData == NULL )
		return false;


	pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID, (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );

	return true;
}


#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

#ifdef SKILL_PAGE_SYSTEM //JHKang
bool CX2SkillTree::Handler_EGS_GET_SKILL_REQ( USHORT usPageNumber_/* = 0*/ )
#else //SKILL_PAGE_SYSTEM
bool CX2SkillTree::Handler_EGS_GET_SKILL_REQ()
#endif //SKILL_PAGE_SYSTEM
{
#ifdef DISABLE_REDUDANT_PACKET_TEST
	if( true == g_pMain->IsWaitingServerPacket( EGS_GET_SKILL_ACK ) )
	{
		return false;
	}
#endif DISABLE_REDUDANT_PACKET_TEST

	if (	NULL == g_pData ||
			NULL == g_pData->GetSkillTree() )
	{
		return false;
	}

	const std::map<int, KGetSkillInfo>& mapSkillInfo = g_pData->GetSkillTree()->GetMapSkillInfo();

	if ( 0 == mapSkillInfo.size() )
		return false;

	KEGS_GET_SKILL_REQ kPacket;
	kPacket.m_mapSkillList = mapSkillInfo;
#ifdef SKILL_PAGE_SYSTEM //JHKang
	USHORT usSkillPage = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;

	if ( usPageNumber_ > 0 )
		usSkillPage = usPageNumber_;

	kPacket.m_iActiveSkillPageNumber = usSkillPage;
#endif //SKILL_PAGE_SYSTEM

#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL
	kPacket.m_vecNowLearnSkill = m_vecNowLearnSkill;
#endif SERV_SUB_QUEST_LEARN_NEW_SKILL

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_SKILL_ACK );

	return true;
}

#else // UPGRADE_SKILL_SYSTEM_2013

bool CX2SkillTree::Handler_EGS_GET_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
{

#ifdef DISABLE_REDUDANT_PACKET_TEST
	if( true == g_pMain->IsWaitingServerPacket( EGS_GET_SKILL_ACK ) )
	{
		return false;
	}
#endif DISABLE_REDUDANT_PACKET_TEST



	if ( g_pData->GetMyUser() == NULL )
		return false;

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return false;


	KEGS_GET_SKILL_REQ kPacket;
	kPacket.m_iSkillID = (int) eSkillID;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_SKILL_ACK );

	return true;
}

#endif // UPGRADE_SKILL_SYSTEM_2013




bool CX2SkillTree::Handler_EGS_GET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );	


	g_pMain->DeleteServerPacket( EGS_GET_SKILL_ACK );
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_25108 ), g_pMain->GetNowState() );

			if (	NULL == g_pData ||
					NULL == g_pData->GetMyUser() ||
					NULL == g_pData->GetMyUser()->GetSelectUnit() ||
					NULL == g_pData->GetUIManager() ||
					NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			/// 남은 포인트 연산
			pUnitData->m_iSPoint	= kEvent.m_iRemainSP;
			pUnitData->m_iCSPoint	= kEvent.m_iRemainCSP;

#ifdef SKILL_PAGE_SYSTEM //JHKang
			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );
#endif //SKILL_PAGE_SYSTEM

			std::map<int, KGetSkillInfo>::iterator it = kEvent.m_mapSkillList.begin();

			for ( ; it != kEvent.m_mapSkillList.end(); ++it )
			{
				KGetSkillInfo getSkillInfo = it->second;

#ifdef SKILL_PAGE_SYSTEM //JHKang
				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( getSkillInfo.m_iSkillID ), 
																getSkillInfo.m_iSkillLevel, getSkillInfo.m_iSpendSkillCSPoint,
																pUnitData->m_UserSkillTree.GetUsingPage() );
#else //SKILL_PAGE_SYSTEM
				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( getSkillInfo.m_iSkillID ), 
					getSkillInfo.m_iSkillLevel, getSkillInfo.m_iSpendSkillCSPoint );
#endif //SKILL_PAGE_SYSTEM

				bool bEnableSkillEquipped = true;
				const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( getSkillInfo.m_iSkillID );

				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillId );
				switch( pSkillTemplet->m_eType )
				{
				case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE:
					{
						if( g_pData != NULL && 
							g_pData->GetUIManager() != NULL && 
							g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
						{
							g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
						}
					} break;
				}
			}

			// 사운드
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillUp.ogg", false, false );

			if  ( true == g_pData->GetUIManager()->GetUISkillTree()->GetShow() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);

			/// 스킬 추가 레벨 효과 갱신
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelByBuff();
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();
		}

#else // UPGRADE_SKILL_SYSTEM_2013

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			bool bEnableSkillEquipped = true;
			const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iSkillID );

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
			{
				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( eSkillId, kEvent.m_iSkillLevel, kEvent.m_iSkillCSPoint );
				pUnitData->m_iSPoint	= kEvent.m_iSPoint;
				pUnitData->m_iCSPoint	= kEvent.m_iCSPoint;
			}

			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillId, kEvent.m_iSkillLevel );
			switch( pSkillTemplet->m_eType )
			{
			case CX2SkillTree::ST_ACTIVE:
			case CX2SkillTree::ST_BUFF:
			case CX2SkillTree::ST_SPECIAL_ACTIVE:
#ifdef ADDED_RELATIONSHIP_SYSTEM
			case CX2SkillTree::ST_RELATIONSHIP_SKILL:
#endif // ADDED_RELATIONSHIP_SYSTEM
#ifdef FINALITY_SKILL_SYSTEM //JHKang
			case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
#endif //FINALITY_SKILL_SYSTEM
				{
					// 장착 가능한 스킬을 새로 배웠다면
					if( kEvent.m_iSkillLevel == 1 )
					{
						// 빈 슬롯을 검색해서
						int EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, false );
						if( EmptySlot != -1 )
						{
							// 장착해준다
							Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot, eSkillId );
						}
						else if( pUnitData->m_UserSkillTree.GetEnabledSkillSlotB() )
						{
							EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, true );
							if( EmptySlot != -1 )
							{
								Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot+EQUIPPED_SKILL_SLOT_COUNT, eSkillId );
							}
						}
					}
					// 					else
					// 					{
					// 						// 새로 장착 가능한 스킬이 아니고
					// 						if ( NULL != g_pX2Game )	// 게임 중이면
					// 						{
					// 							// 현재 장착 중인 스킬인지 알아봄
					// 							const int iEquippedSlotAIndex
					// 								= pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( eSkillId, false );
					// 
					// 							if ( -1 != iEquippedSlotAIndex )	// 슬롯 A에서 장착 중인 스킬이라면
					// 							{
					// 								g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( pUnitData, eSkillId, iEquippedSlotAIndex, false );
					// 								g_pX2Game->UpdateSkillSlotUI();
					// 							}
					// 							else	// 슬롯 A에서 장착 중인 스킬이 아니고
					// 							{
					// 								const int iEquippedSlotBIndex
					// 									= pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( eSkillId, true );
					// 
					// 								if ( -1 != iEquippedSlotBIndex )	// 슬롯 B에서 장착 중인 스킬이라면
					// 								{
					// 									g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( pUnitData, eSkillId, iEquippedSlotBIndex, true );
					// 									g_pX2Game->UpdateSkillSlotUI();
					// 								}
					// 							}
					// 						}
					// 					}

				} break;
			case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE:
				{
					if( g_pData != NULL && 
						g_pData->GetUIManager() != NULL && 
						g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
					{
						g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
					}
				} break;
			}

			if( NULL != g_pData->GetUIManager() &&
				true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
			{
				//CX2Unit::UnitData* pUnitData = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);

			}

			// 사운드
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillUp.ogg", false, false );

			if(pUnitData->m_UserSkillTree.IsMaxSkillLevel( (SKILL_ID)kEvent.m_iSkillID ))
			{
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillMaster.ogg", false, false );
				const SkillTreeTempletMap& refSkillTreeTempletMap = GetSkillTreeTempletMap( (int)pUnitData->m_UnitClass );
				SkillTreeTempletMap::const_iterator it = refSkillTreeTempletMap.find((SKILL_ID)kEvent.m_iSkillID);
				if( it != refSkillTreeTempletMap.end() )
				{
					const CX2SkillTree::SkillTreeTemplet& refSkillTreeTemplet = it->second;
					if(false == refSkillTreeTemplet.m_vecFollowingSkill.empty())
					{
						// 후행 스킬이 있으면 (화살표가 열릴 것이다)
						g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillRootOpen.ogg", false, false );
					}
				}
			}			
		}

#endif // UPGRADE_SKILL_SYSTEM_2013

		return true;
	}


	return false;

}

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
bool CX2SkillTree::Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eDelSkillID )
#else // UPGRADE_SKILL_SYSTEM_2013
bool CX2SkillTree::Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
#endif // UPGRADE_SKILL_SYSTEM_2013
{

#ifdef DISABLE_REDUDANT_PACKET_TEST
	if( true == g_pMain->IsWaitingServerPacket( EGS_RESET_SKILL_ACK ) )
	{
		return false;
	}
#endif DISABLE_REDUDANT_PACKET_TEST


	KEGS_RESET_SKILL_REQ kPacket;

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	kPacket.m_iSkillID = static_cast<int>( eDelSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
	kPacket.m_iSkillID = (int) eSkillID;
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SKILL_PAGE_SYSTEM //JHKang
	kPacket.m_iActiveSkillPageNumber = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;
#endif //SKILL_PAGE_SYSTEM

	g_pData->GetServerProtocol()->SendPacket( EGS_RESET_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESET_SKILL_ACK );

	return true;
}

void CX2SkillTree::Hander_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ )
{
	KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ kPacket;

	kPacket.m_iYear		= iYear_;
	kPacket.m_iMonth	= iMonth_;
	kPacket.m_iDay		= iDay_;
	kPacket.m_iHour		= iHour_;
	kPacket.m_iMinute	= iMinute_;

	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK );
}

void CX2SkillTree::Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pChatBox->AddChatLog( kEvent.m_wstrCspEndDate.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
	}
}




bool CX2SkillTree::Handler_EGS_RESET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetSkillTree() )
	{
		return false;
	}

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_RESET_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

			/// 변경 안내 팝업
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_25108 ), g_pMain->GetNowState() );

			CX2Unit::UNIT_CLASS		eUnitClass		= static_cast<CX2Unit::UNIT_CLASS>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
			CX2SkillTree::SKILL_ID	eDeleteSkillID	= static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID );
			CX2UserSkillTree& userSkillTree	= pUnitData->m_UserSkillTree;

			
			/// 스킬 트리 템플릿 반환
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet	= g_pData->GetSkillTree()->GetSkillTreeTemplet( eUnitClass, eDeleteSkillID );

			if ( NULL == pSkillTreeTemplet )
				return false; 

			/// 현재 스킬을 선행 스킬로 한 스킬이 찍혀 있는지 검사
			const bool bHaveFollowingSkill = 0 < userSkillTree.GetSkillLevel( static_cast<CX2SkillTree::SKILL_ID>( pSkillTreeTemplet->m_iFollowingSkill ) ) ? true : false ;

		#ifdef SKILL_PAGE_SYSTEM //JHKang
			/// 초기화 될 스킬이 기본 스킬 이거나, 후행 스킬이 찍혀 있다면 1레벨로 초기화 시키자
			if ( true == isDefaultSkill( eDeleteSkillID ) || true == bHaveFollowingSkill )
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 1, 0,
													pUnitData->m_UserSkillTree.GetUsingPage() );
			/// 그 외엔 0레벨로 초기화
			else
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 0, 0,
													pUnitData->m_UserSkillTree.GetUsingPage() );
		#else //SKILL_PAGE_SYSTEM
			/// 초기화 될 스킬이 기본 스킬 이거나, 후행 스킬이 찍혀 있다면 1레벨로 초기화 시키자
			if ( true == isDefaultSkill( eDeleteSkillID ) || true == bHaveFollowingSkill )
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 1, 0 );
			/// 그 외엔 0레벨로 초기화
			else
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 0, 0 );
		#endif //SKILL_PAGE_SYSTEM

			pUnitData->m_iSPoint	= kEvent.m_iSPoint;
			pUnitData->m_iCSPoint	= kEvent.m_iCSPoint;

#ifdef SKILL_PAGE_SYSTEM //JHKang
			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );
#endif //SKILL_PAGE_SYSTEM

	#else // UPGRADE_SKILL_SYSTEM_2013

			// 스킬 트리 갱신
			int iSkillLevel = 0;
			int iSkillCSPoint = 0;
			pUnitData->m_UserSkillTree.GetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID, iSkillLevel, iSkillCSPoint );
			iSkillLevel -= 1;
			if( iSkillCSPoint > 0 )
			{
				iSkillCSPoint -= kEvent.m_iRestoreCSPoint;
			}
			pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID, iSkillLevel, iSkillCSPoint ); 

			pUnitData->m_iSPoint	+= kEvent.m_iRestoreSPoint;
			pUnitData->m_iCSPoint	+= kEvent.m_iRestoreCSPoint;

	#endif // UPGRADE_SKILL_SYSTEM_2013

			// 스킬 초기화 아이템을 사용했기 때문에 인벤 정보를 갱신한다
			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI도..
				if( NULL != g_pData->GetUIManager() &&
					true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN))
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
			}


			// UI 갱신 및 장착 스킬 갱신
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID, 1 );
#endif // UPGRADE_SKILL_SYSTEM_2013

			if ( NULL == pSkillTemplet )
				return false;

			switch( pSkillTemplet->m_eType )
			{
			case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE:
				{
					if( g_pData != NULL && 
						g_pData->GetUIManager() != NULL && 
						g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
					{
						g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
					}
				} break;

			case CX2SkillTree::ST_BUFF:
			case CX2SkillTree::ST_ACTIVE:
			case CX2SkillTree::ST_SPECIAL_ACTIVE:
#ifdef ADDED_RELATIONSHIP_SYSTEM
			case CX2SkillTree::ST_RELATIONSHIP_SKILL:
#endif // ADDED_RELATIONSHIP_SYSTEM
#ifdef FINALITY_SKILL_SYSTEM //JHKang
			case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
#endif //FINALITY_SKILL_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_SKILL_TYPE
			/// June 322112d0 @32211750: the slot-clear group is 5, 6, 7, 12-16 ( 17 / 18 fall through, as in June )
			case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_1:
			case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_2:
			case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_3:
#endif SERV_IRUHADEV_JUNE_SKILL_TYPE
				{
					for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT*2; i++ )
					{
#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10
						bool bSlotB = (i >= EQUIPPED_SKILL_SLOT_COUNT) ? true : false;
#else SERV_IRUHADEV_JUNE_SKILL_SLOT_10
						bool bSlotB = (i > 3) ? true : false;
#endif SERV_IRUHADEV_JUNE_SKILL_SLOT_10
#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10
						int iSlotIndex = (i >= EQUIPPED_SKILL_SLOT_COUNT) ? i-EQUIPPED_SKILL_SLOT_COUNT : i;
#else SERV_IRUHADEV_JUNE_SKILL_SLOT_10
						int iSlotIndex = (i > 3) ? i-4 : i;
#endif SERV_IRUHADEV_JUNE_SKILL_SLOT_10
						const CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( iSlotIndex, bSlotB );
						if( NULL != pSkillSlotData )
						{
							if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
							{
								//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
#ifdef ARA_CHARACTER_BASE
								pUnitData->m_UserSkillTree.SetSkillSlot( iSlotIndex, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, 0, 0, bSlotB );
#else
								pUnitData->m_UserSkillTree.SetSkillSlot( iSlotIndex, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, 0, bSlotB );
#endif
#else	NEW_CHARACTER_CHUNG
								pUnitData->m_UserSkillTree.SetSkillSlot( iSlotIndex, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, bSlotB );
#endif	NEW_CHARACTER_CHUNG
								//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
							}
						}
					}

				} break;
			}

			if( NULL != g_pData->GetUIManager() &&
				true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);				
			}

			// 사운드
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );
#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
			/// June 322112d0 @3221186a: 3235a160 at the end of the OK path
			if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
				g_pData->GetMyUser()->GetSelectUnit()->JuneResetHudMaxHpMp();
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP


		

		}
		return true;
	}
	return false;
}



bool CX2SkillTree::Handler_EGS_INIT_SKILL_TREE_REQ( UidType itemUID )
{
	KEGS_INIT_SKILL_TREE_REQ kPacket;
	kPacket.m_iItemUID = itemUID;
#ifdef SKILL_PAGE_SYSTEM //JHKang
	kPacket.m_iActiveSkillPageNumber = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;
#endif //SKILL_PAGE_SYSTEM

	g_pData->GetServerProtocol()->SendPacket( EGS_INIT_SKILL_TREE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_INIT_SKILL_TREE_ACK );

	return true;
}


bool CX2SkillTree::Handler_EGS_INIT_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_SKILL_TREE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_INIT_SKILL_TREE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();


			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI도..
				if( NULL != g_pData->GetUIManager() && 
					NULL != g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN) )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList(kEvent.m_vecInventorySlotInfo);
                    
				}
			}

			pUnitData->m_iSPoint = kEvent.m_iSPoint;
			pUnitData->m_iCSPoint = kEvent.m_iCSPoint;

#ifdef SKILL_PAGE_SYSTEM //JHKang
			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );
#endif //SKILL_PAGE_SYSTEM

			// 스킬트리및 장착스킬 초기화
			pUnitData->m_UserSkillTree.Reset( true, true, false, false );

	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		#ifdef SKILL_PAGE_SYSTEM //JHKang
			pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetUsingPage() );	// 각 전직별 기본 스킬 설정
		#else //SKILL_PAGE_SYSTEM
			pUnitData->m_UserSkillTree.SetDefaultSkill();		/// 각 전직별 기본 스킬 설정
		#endif //SKILL_PAGE_SYSTEM

			/// 퀵슬롯 갱신
			if ( NULL != g_pData->GetUIManager() &&  NULL != g_pData->GetUIManager()->GetUISkillTree() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );
	#else // UPGRADE_SKILL_SYSTEM_2013
			// note!! 캐릭터 추가되면 작업해야함
			// 캐릭터별 기본 스킬 배우기
			switch( g_pData->GetMyUser()->GetSelectUnit()->GetType() )
			{
			case CX2Unit::UT_ELSWORD:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ES_MEGASLASH, 1, 0 );
#ifdef ELSWORD_SHEATH_KNIGHT
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ES_ROLLING, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ES_COUNTER, 1, 0 );
#endif ELSWORD_SHEATH_KNIGHT
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_ES_MEGASLASH, 1, 0 );
#ifdef ELSWORD_SHEATH_KNIGHT
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ES_ROLLING, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ES_COUNTER, 1, 0 );
#endif ELSWORD_SHEATH_KNIGHT
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;

			case CX2Unit::UT_ARME:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AV_CHAIN_FIRE_BALL, 1, 0 );
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AV_FIRE_ROAD, 1, 0 );					
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AV_TELEPORT, 1, 0 );
#endif
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_AV_CHAIN_FIRE_BALL, 1, 0 );
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AV_FIRE_ROAD, 1, 0 );					
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AV_TELEPORT, 1, 0 );
#endif
#endif //UPGRADE_SKILL_SYSTEM_2013
					
				} break;

			case CX2Unit::UT_LIRE:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_LE_RAIL_STINGER, 1, 0 );
#ifdef SERV_TRAPPING_RANGER_TEST
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_LE_RISING_FALCON, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_LE_SIEGEMODE, 1, 0 );
#endif SERV_TRAPPING_RANGER_TEST
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_LE_RAIL_STINGER, 1, 0 );
#ifdef SERV_TRAPPING_RANGER_TEST
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_LE_RISING_FALCON, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_LE_SIEGEMODE, 1, 0 );
#endif SERV_TRAPPING_RANGER_TEST
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;

			case CX2Unit::UT_RAVEN:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_RF_MAXIMUM_CANNON, 1, 0 );
#ifdef RAVEN_WEAPON_TAKER
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_RF_BREAKING_FIST, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_RF_BURNING_RUSH, 1, 0 );
#endif RAVEN_WEAPON_TAKER
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_RF_MAXIMUM_CANNON, 1, 0 );
#ifdef RAVEN_WEAPON_TAKER
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_RF_BREAKING_FIST, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_RF_BURNING_RUSH, 1, 0 );
#endif RAVEN_WEAPON_TAKER
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;

			case CX2Unit::UT_EVE:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_EN_ILLUSION_STRIKE, 1, 0 );
#ifdef EVE_ELECTRA
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_EN_PHOTON_BLINK, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_EN_CLOAKING, 1, 0 );
#endif EVE_ELECTRA
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_EN_ILLUSION_STRIKE, 1, 0 );
#ifdef EVE_ELECTRA
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_EVE_PHOTON_BLINK, 1, 0 );
#else
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_EN_CLOAKING, 1, 0 );
#endif EVE_ELECTRA
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;

				//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
		#ifdef	NEW_CHARACTER_CHUNG
			case CX2Unit::UT_CHUNG:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_CC_BRUTAL_SWING, 1, 0 );
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_CC_RELOAD_CANNON, 1, 0 );
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_CHUNG_BRUTAL_SWING, 1, 0 );
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_CHUNG_RELOAD_CANNON, 1, 0 );
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;

		#endif	NEW_CHARACTER_CHUNG
				//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

		#ifdef ARA_CHARACTER_BASE
			case CX2Unit::UT_ARA:
				{
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AM_WIND_WEDGE, 1, 0 );
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_AM_GAIN_FORCE, 1, 0 );
#else //UPGRADE_SKILL_SYSTEM_2013
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_SA_ARA_WIND_WEDGE, 1, 0 );
					pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_A_ARA_GAIN_FORCE, 1, 0 );
#endif //UPGRADE_SKILL_SYSTEM_2013
				} break;
		#endif
			}
	#endif // UPGRADE_SKILL_SYSTEM_2013


			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
				g_pX2Game->GetMyUnit()->InitEquippedActiveSkillState();
						
			if( NULL != g_pData->GetUIManager() )
			{
				if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
				{
					g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
				}

				if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO) )
				{
					g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
				}
			}
#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
			/// June 3220ea50 @3220ede2: the HUD max MP / HP refresh 3235a160 on my select unit, last on the OK path
			if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
				g_pData->GetMyUser()->GetSelectUnit()->JuneResetHudMaxHpMp();
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP

			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"모든 스킬을 초기화 하였습니다.", m_pNowStage );
		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"스킬 초기화에 실패하였습니다.", m_pNowStage );
		}
		return true;
	}
	return false;
}

bool CX2SkillTree::Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );	


	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

	switch( g_pMain->GetNowStateID() )
	{
	default:
		{
			On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( kEvent );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2687 ), (CKTDXStage*) g_pMain->GetNowState() );
			g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillEquipedSlotUI();
			
			pMyUnit->AccessUnitData().m_wstrCSPointEndDate.clear();

// 			if ( NULL != g_pX2Game )
// 			{
// 				g_pX2Game->GetMyUnit()->UpdateSocketDataAndEnchantData();
// 				g_pX2Game->GetMyUnit()->UpdatePassiveAndActiveSkillState();
// 			}

			

		} break;

	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_DUNGEON_RESULT:
	case CX2Main::XS_PVP_GAME:
	case CX2Main::XS_PVP_RESULT:
		{
			// packet처리를 유보해둔다.
			KEGS_EXPIRE_CASH_SKILL_POINT_NOT* pEvent = new KEGS_EXPIRE_CASH_SKILL_POINT_NOT;
			*pEvent = kEvent;
			g_pMain->AddReservedServerPacket( EGS_EXPIRE_CASH_SKILL_POINT_NOT, pEvent );

			wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
			D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2687 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_2687 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );

		} break;
	}


	return true;
}

bool CX2SkillTree::Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UPDATE_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	

	if( false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return false;
	

	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
	if( pUnitData->m_UnitUID != kEvent.m_iUnitUID )
		return false;


	if( false == kEvent.m_bUpdateEndDateOnly )
	{
#ifdef SKILL_PAGE_SYSTEM //JHKang
		pUnitData->m_iCSPoint = kEvent.m_iCSPoint;

		int iUsingPage = pUnitData->m_UserSkillTree.GetUsingPage();

		for ( int i = 0; i < pUnitData->m_UserSkillTree.GetOpenedPage(); ++i )
		{
			pUnitData->m_UserSkillTree.SetUsingPage( i + 1 );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );
		}

		pUnitData->m_UserSkillTree.SetUsingPage( iUsingPage + 1 );
#else //SKILL_PAGE_SYSTEM
		pUnitData->m_iCSPoint = kEvent.m_iCSPoint;
#endif //SKILL_PAGE_SYSTEM
	}
	pUnitData->m_wstrCSPointEndDate = kEvent.m_wstrCSPointEndDate;


	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(false, true, false);
	}

#ifdef SKILL_PLUS_ITEM_USE_POPUP
	if(kEvent.m_iSkillPointItemID  == SKILL_PLUS_ITEM_ID
#ifdef SERV_EVENT_CASH_SKILL_POINT_ITEM_JP
		|| kEvent.m_iSkillPointItemID  == SKILL_PLUS_ITEM_ID_JP 
		|| kEvent.m_iSkillPointItemID  == SKILL_POINT_10_30DAY_USE_INVEN_JP 
		|| kEvent.m_iSkillPointItemID  == SKILL_POINT_10_15DAY_USE_INVEN_JP 
#endif //SERV_EVENT_CASH_SKILL_POINT_ITEM_JP	
#ifdef SERV_EVENT_CASH_SKILL_POINT_ITEM_TWHK
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_7DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_15DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_30DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_7DAY_USE_INVEN_2
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN_2
#endif SERV_EVENT_CASH_SKILL_POINT_ITEM_TWHK

#ifdef SERV_EVENT_CASH_SKILL_POINT_ITEM_INT
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN_INT
#endif SERV_EVENT_CASH_SKILL_POINT_ITEM_INT

#ifdef SERV_SKILL_5_POINT_7_DAY_EU
		|| kEvent.m_iSkillPointItemID == EVENT_SKILL_5_POINT_7_DAY
#endif SERV_SKILL_5_POINT_7_DAY_EU
#ifdef SERV_EVENT_SKILL_POINT_130_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_130_1DAY_USE_INVEN
#endif SERV_EVENT_SKILL_POINT_130_1DAY_USE_INVEN
#ifdef SERV_EVENT_SKILL_POINT_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_1DAY_USE_INVEN
#endif SERV_EVENT_SKILL_POINT_1DAY_USE_INVEN
#ifdef SERV_EVENT_GNOSIS_HAPP_NEW_YEAR
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_14DAY_USE_INVEN	
#endif SERV_EVENT_GNOSIS_HAPP_NEW_YEAR
#ifdef SERV_LURIEL_GNOSIS
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_15DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_30DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_60DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_15DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_30DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_60DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_7DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_7DAY
#endif //SERV_LURIEL_GNOSIS
#ifdef SERV_EVENT_CASH_SKILL_POINT_ITEM_INT
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN_INT
#endif SERV_EVENT_CASH_SKILL_POINT_ITEM_INT
		)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
#endif SKILL_PLUS_ITEM_USE_POPUP
	//{{ 2011. 11. 30	최육사	패키지 상품 추가
#ifdef SERV_ADD_PACKAGE_PRODUCT
	if(kEvent.m_iSkillPointItemID  == SKILL_POINT_5_USE_INVEN_ITEM_ID || kEvent.m_iSkillPointItemID  == SKILL_POINT_5_USE_INVEN_ITEM_ID_7_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == SKILL_POINT_10_USE_INVEN_ITEM_ID)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
#endif SERV_ADD_PACKAGE_PRODUCT
	//}}
#ifdef SERV_CN_GNOSIS
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_10_USE_INVEN_7_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_1_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_7_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_7_DAY_TYPE2)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_10_USE_INVEN_15_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
#endif SERV_CN_GNOSIS

#ifdef SERV_GNOSIS_BR
	if( kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_7_DAY || kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_15_DAY )
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
#endif SERV_GNOSIS_BR

#ifdef SERV_US_GNOSIS
	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_15_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
#endif SERV_US_GNOSIS

	return true;
}



bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != kEvent.m_iUnitUID )
		return false;

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) kEvent.m_cSkillSlotBExpirationState );
	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );


	// 갱신 
	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}

#ifdef REFORM_UI_SKILLSLOT
	if( NULL != g_pX2Game )
	{
		g_pX2Game->UpdateSkillSlotUI( true );
		g_pX2Game->InitExpandSkillCoolTime();
	}
#endif //REFORM_UI_SKILLSLOT

	return true;
}


bool CX2SkillTree::Handler_EGS_EXPIRE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != g_pData->GetMyUser() ) 
		return false;

	if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );

		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
	#ifdef	NEW_CHARACTER_CHUNG
	#ifdef ARA_CHARACTER_BASE
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
	#else
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, true );
	#endif
	#else	NEW_CHARACTER_CHUNG
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, true );
	#endif	NEW_CHARACTER_CHUNG
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
		
		if( g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
		{
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
		}

		switch( g_pMain->GetNowStateID() )
		{
		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
			} break;

		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_DUNGEON_RESULT:
		case CX2Main::XS_PVP_GAME:
		case CX2Main::XS_PVP_RESULT:
			{
				wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
				D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			} break;
		}
	}

	return true;
}

#ifdef SERV_SKILL_SLOT_CHANGE_INVENTORY
bool CX2SkillTree::Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL == g_pData->GetMyUser() ) 
		return false;

	if( NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SKILL_SLOT_CHANGE_ITEM_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		if ( kEvent.m_cSkillSlotBExpirationState == CX2UserSkillTree::SSBES_EXPIRED )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );

			//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
#ifdef ARA_CHARACTER_BASE
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
#else
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, true );
#endif
#else	NEW_CHARACTER_CHUNG
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, true );
#endif	NEW_CHARACTER_CHUNG
			//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

			if( g_pData->GetUIManager() != NULL &&
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
			}

			switch( g_pMain->GetNowStateID() )
			{
			default:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
				} break;

			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_DUNGEON_RESULT:
			case CX2Main::XS_PVP_GAME:
			case CX2Main::XS_PVP_RESULT:
				{
					wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
					D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

					g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
					g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				} break;
			}
		}
		else if ( kEvent.m_cSkillSlotBExpirationState == CX2UserSkillTree::SSBES_NOT_EXPIRED )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_NOT_EXPIRED );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );

			if( NULL != g_pData->GetUIManager() &&
				true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
			}
		}
	}
#ifdef REFORM_UI_SKILLSLOT
	if( NULL != g_pData && NULL != g_pData->GetGuildManager() && NULL != g_pData->GetUIManager()->GetUISkillTree())
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}
	
	if( NULL != g_pX2Game )
	{
		if ( NULL != g_pX2Game->GetMyUnit() && g_pX2Game->GetMyUnit()->IsMyUnit() )
		{
			g_pX2Game->UpdateSkillSlotUI( true);
			g_pX2Game->InitExpandSkillCoolTime();
		}
	}
#endif //REFORM_UI_SKILLSLOT

#ifdef SKILL_SLOT_UI_TYPE_B
	if( NULL != g_pMain)
	{
		g_pMain->GetGameOption().SetIsSkillUITypeA( g_pMain->GetGameOption().GetIsSkillUITypeA() );				
	}
#endif //SKILL_SLOT_UI_TYPE_B

	// 오현빈 // 2013-08-23 // 쿨타임 관리 벡터 초기화 구문 추가
	CX2GageManager::GetInstance()->InitMySkillCoolTimeList( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree );

	return true;
}
#endif SERV_SKILL_SLOT_CHANGE_INVENTORY





bool CX2SkillTree::Handler_EGS_UNSEAL_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UNSEAL_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.AddSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );


	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillTree( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );
		
		g_pData->GetUIManager()->GetUISkillTree()->OpenDLGSkillUnsealed( true );
		g_pData->GetUIManager()->GetUISkillTree()->UpdateDLGSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );
#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
		/// June 3220ee30 @3220f004: 3235a160 after the tree redraw ( June runs that block only when AddSkillUnsealed 328914b0 returns true )
		if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			g_pData->GetMyUser()->GetSelectUnit()->JuneResetHudMaxHpMp();
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
	}


	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}
#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
	/// June 3220ee30 @3220f035: 3235a160 again, after the quest manager's 3232ecd0 ( June calls it twice )
	if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		g_pData->GetMyUser()->GetSelectUnit()->JuneResetHudMaxHpMp();
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP

	return true;
}


#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
/// June 3220e5f0: the send half of the slot-change queue
bool CX2SkillTree::JuneSendChangeSkillSlotReq( int iSlotID, CX2SkillTree::SKILL_ID eSkillID )
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_CHANGE_SKILL_SLOT_ACK ) )
		return false;

	KEGS_CHANGE_SKILL_SLOT_REQ kPacket;
	kPacket.m_iSlotID = iSlotID;
	kPacket.m_iSkillID = static_cast<int>( eSkillID );

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_SKILL_SLOT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_SKILL_SLOT_ACK );

#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	SetSkillSlotChanging( true );		/// tree +0x90
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE

	/// In a game my unit takes no input until the ACK: +0xde8 ( m_fCanNotInputTime ) = 10 ( _DAT_3344a51c ); the ACK sets 0.5
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
		g_pX2Game->GetMyUnit()->SetCanNotInputTime_LUA( 10.f );

	return true;
}

/// June 322105c0: one queued change per frame; it leaves the queue only once it was sent
void CX2SkillTree::JuneFrameMoveSkillSlotQueue()
{
	if( true == m_deqJuneSkillSlotChange.empty() )
		return;

	const std::pair< int, int > pairFront = m_deqJuneSkillSlotChange.front();
	if( true == JuneSendChangeSkillSlotReq( pairFront.first, static_cast<CX2SkillTree::SKILL_ID>( pairFront.second ) ) &&
		false == m_deqJuneSkillSlotChange.empty() )
	{
		m_deqJuneSkillSlotChange.pop_front();
	}
}

/// June 32206d30: every tree skill of the class whose required-level list holds ( level / 5 ) * 5
void CX2SkillTree::JuneGetSkillListByLevel( int iUnitLevel, int iUnitClass, OUT std::vector< int >& vecSkillID_ ) const
{
	UnitSkillTreeTempletMap::const_iterator mitClass = m_mapUnitSkillTreeTemplet.find( iUnitClass );
	if( mitClass == m_mapUnitSkillTreeTemplet.end() )
		return;

	const int iLevelBand = ( iUnitLevel / 5 ) * 5;
	SkillTreeTempletMap::const_iterator mitRow;
	for( mitRow = mitClass->second.begin(); mitRow != mitClass->second.end(); ++mitRow )
	{
		const SkillTemplet* pTemplet = GetSkillTemplet( mitRow->first );
		if( NULL == pTemplet )
			continue;

		/// +0x174 == 5 ( SPRT_HYPER_CLASS ) is skipped unless my unit is transcendent ( CX2Unit +0x3c ), which March has no field for ( Stage 6 )
		if( SPRT_HYPER_CLASS == pTemplet->m_eJunePowerRateType )
			continue;

		for( UINT i = 0; i < pTemplet->m_vecRequireCharacterLevel.size(); ++i )
		{
			if( iLevelBand == pTemplet->m_vecRequireCharacterLevel[i] )
			{
				vecSkillID_.push_back( static_cast<int>( pTemplet->m_eID ) );
				break;
			}
		}
	}
}

/// The block June's three level-up handlers end with ( 323ae130, 32473f70, 325346e0 ). It reads my own unit's level and class
/// whichever unit the NOT is for ( JUNE_BUGS ), and it only runs below level 11.
void CX2SkillTree::JuneAutoEquipOnLevelUp()
{
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	const int iLevel = pUnit->GetUnitData().m_Level;
	if( 11 <= iLevel )
		return;

	std::vector< int > vecSkillID;
	JuneGetSkillListByLevel( iLevel, static_cast<int>( pUnit->GetClass() ), vecSkillID );

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_SKILL] ACQ level-up L=%d: %d skill(s) at band %d", iLevel, static_cast<int>( vecSkillID.size() ), ( iLevel / 5 ) * 5 );
#endif SERV_IRUHADEV_OFFLINE

	if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
		g_pData->GetUIManager()->GetUISkillTree()->JuneAutoEquipNewSkills( vecSkillID );
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM
/// June 3220d2b0 ( filter { skill 0, level, class, memo 0 } ): for every tree skill of the class, type 1 ( no base skill ) or 3 ( +0x178
/// set ) when its first required level is the level, 4 when a later required level is, and 2 for each memo row of the level whose
/// skill is this one ( 32451250 ). A hyper-class row ( +0x174 == 5 ) is skipped unless my unit is transcendent ( CX2Unit +0x3c ),
/// which March has no field for ( as 32206d30 ).
void CX2SkillTree::JuneGetNewSkillOrMemoByLevel( int iLevel_, int iUnitClass_, OUT JuneNewSkillOrMemoMap& mapNew_ ) const
{
	UnitSkillTreeTempletMap::const_iterator mitClass = m_mapUnitSkillTreeTemplet.find( iUnitClass_ );
	if( mitClass == m_mapUnitSkillTreeTemplet.end() )
		return;

	SkillTreeTempletMap::const_iterator mitRow;
	for( mitRow = mitClass->second.begin(); mitRow != mitClass->second.end(); ++mitRow )
	{
		const SkillTemplet* pTemplet = GetSkillTemplet( mitRow->first );
		if( NULL == pTemplet )
			continue;

		if( SPRT_HYPER_CLASS == pTemplet->m_eJunePowerRateType )
			continue;

		const int iFirstLevel = ( true == pTemplet->m_vecRequireCharacterLevel.empty() ) ? 0 : pTemplet->m_vecRequireCharacterLevel[0];
		if( iLevel_ == iFirstLevel )
			JuneAddNewSkillOrMemo( ( pTemplet->m_iDeleteSkillIDByUpgrade < 1 ) ? 1 : 3, pTemplet, mapNew_ );

		for( UINT i = 0; i < pTemplet->m_vecRequireCharacterLevel.size(); ++i )
		{
			if( iLevel_ == pTemplet->m_vecRequireCharacterLevel[i] )
			{
				if( 0 < i )
					JuneAddNewSkillOrMemo( 4, pTemplet, mapNew_ );
				break;
			}
		}

		/// @3220d4a0: the memo rows are re-read for every skill ( June )
		std::vector<const SJuneSkillMemoTemplet*> vecMemo;
		CX2SkillNoteManager::GetInstance()->GetJuneMemoList( 0, iLevel_, iUnitClass_, 0, vecMemo );
		for( UINT i = 0; i < vecMemo.size(); ++i )
		{
			if( NULL != vecMemo[i] && vecMemo[i]->m_iSkillID == static_cast<int>( pTemplet->m_eID ) )
				JuneAddNewSkillOrMemo( 2, pTemplet, mapNew_ );
		}
	}
}

/// June 3220b990: add the templet under its type unless a templet with the same id ( +8 ) is there already
/*static*/ void CX2SkillTree::JuneAddNewSkillOrMemo( int iType_, const SkillTemplet* pTemplet_, IN OUT JuneNewSkillOrMemoMap& mapNew_ )
{
	if( NULL == pTemplet_ )
		return;

	JuneNewSkillOrMemoMap::iterator mit = mapNew_.find( iType_ );
	if( mit == mapNew_.end() )
	{
		std::vector< const SkillTemplet* > vecTemplet;
		vecTemplet.push_back( pTemplet_ );
		mapNew_.insert( std::make_pair( iType_, vecTemplet ) );
		return;
	}

	for( UINT i = 0; i < mit->second.size(); ++i )
	{
		if( NULL != mit->second[i] && mit->second[i]->m_eID == pTemplet_->m_eID )
			return;
	}
	mit->second.push_back( pTemplet_ );
}

/// June 31d885e0 ( a CX2Main method in June; called for my own unit by 323ae130 @323ae277, 32473f70 @324744af, 325346e0 @32534832 ):
/// only for a class with memo rows ( 324511a0 ), what opens at my unit's level goes to the emblem manager's +0x64 rows
/// ( skill id, type, 1 ) in type order, and emblem 17 is shown again ( 32447d70( 0x11, true ) ) when anything opened
void CX2SkillTree::JuneShowNewSkillOrMemoEmblem() const
{
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	const int iUnitClass = static_cast<int>( pUnit->GetClass() );
	if( false == CX2SkillNoteManager::GetInstance()->HasJuneMemoClass( iUnitClass ) )
		return;

	const int iLevel = pUnit->GetUnitData().m_Level;
	JuneNewSkillOrMemoMap mapNew;
	JuneGetNewSkillOrMemoByLevel( iLevel, iUnitClass, mapNew );

	int iRows = 0;
	for( JuneNewSkillOrMemoMap::const_iterator mit = mapNew.begin(); mit != mapNew.end(); ++mit )
	{
		for( UINT i = 0; i < mit->second.size(); ++i )
		{
			if( NULL == mit->second[i] )
				continue;

			CX2EmblemManager::GetInstance()->JunePushNewSkillOrMemo(
				D3DXVECTOR3( static_cast<float>( mit->second[i]->m_eID ), static_cast<float>( mit->first ), 1.f ) );
			++iRows;
#ifdef SERV_IRUHADEV_OFFLINE
			CX2OfflineLog::Server( L"[JUNE_SKILL] EMBLEM row: skill %d type %d", static_cast<int>( mit->second[i]->m_eID ), mit->first );
#endif SERV_IRUHADEV_OFFLINE
		}
	}

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_SKILL] EMBLEM level-up L=%d class %d: %d type(s), %d row(s)", iLevel, iUnitClass,
		static_cast<int>( mapNew.size() ), iRows );
#endif SERV_IRUHADEV_OFFLINE

	if( false == mapNew.empty() )
		CX2EmblemManager::GetInstance()->PlayEmblem( CX2EmblemManager::EI_GET_NEW_SKILL_OR_MEMO, true );
}
#endif SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM

/// June's server-pushed skill grant. June handles it in its states ( village 323b0770, menu 324631a0, 32535be0 ); March routes skill
/// packets here from every state ( CX2State::UIServerEventProc ).
bool CX2SkillTree::Handler_EGS_JUNE_ACQUIRE_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_JUNE_ACQUIRE_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return true;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	if( pUnit->GetUID() != kEvent.m_iUnitUID )
		return true;

	pUnit->AccessUnitData().m_UserSkillTree.AddJuneAcquiredSkills( kEvent.m_vecSkillData );	/// 328924c0

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_SKILL] ACQ NOT unitUID=%I64d: %d skill(s) added", (__int64)kEvent.m_iUnitUID, static_cast<int>( kEvent.m_vecSkillData.size() ) );
#endif SERV_IRUHADEV_OFFLINE

	if( NULL != g_pData->GetUIManager() )
	{
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// every copy, when the tree UI exists ( 3214f050 ): 328df480( the granted skills ) -> 328ded60, an upgraded skill takes its base's
		/// tree slot and memo buttons
		if( NULL != g_pData->GetUIManager()->GetUISkillTree() )
			g_pData->GetUIManager()->GetUISkillTree()->JuneReplaceUpgradedSkillSlots( kEvent.m_vecSkillData );
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
		/// 32535be0's copy redraws the tree when the skill window is shown ( UpdateUI( 0, 1, 0 ) ); every copy also lights the new skills'
		/// memo buttons ( 328df480 -> 328ded60 ), which come with June's tree UI ( S7b )
		if( NULL != g_pData->GetUIManager()->GetUISkillTree() && true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_SKILL ) )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( false, true, true );

		/// the character window's stats ( 321524d0( 0x13 ) -> 3214ef80 -> 3291cfb0 )
		if( true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_CHARINFO ) && NULL != g_pData->GetUIManager()->GetUICharInfo() )
			g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	}

	/// June then calls CX2Unit 3235a160, a HUD stat refresh through a June-only singleton ( 324943c0 ) March does not have ( S7b note )
#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
	/// skill Phase S7k: that is the HUD max MP / HP refresh ( 323b0770 @323b0888, 324631a0 @3246334e ); 324943c0 is March's CX2GageManager
	if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		g_pData->GetMyUser()->GetSelectUnit()->JuneResetHudMaxHpMp();
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
	return true;
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
/// June 31f1bdb0 @31f1c008..31f1c162 ( the same block in 31f1c1d0 ): with a game whose my-unit ( vslot 0x98 ) has unit data, each of the five
/// slots of bar A then bar B ( 3288fe00( i, b ) ) that holds a skill ( > 0 ) is rebuilt: 31fd84c0( unit data, skill, i, b ), March's
/// 4-argument ChangeEquippedSkillState ( 31fd6880 = CalcEquippedSkillLevelPlusAndMPCostBySlotID, slots 493 / 495 / 494 ).
/*static*/ void CX2SkillTree::JuneRefreshEquippedSkillsForMemo()
{
	if( NULL == g_pX2Game || NULL == g_pX2Game->GetMyUnit() || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CX2Unit::UnitData& kUnitData = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
	for( int i = 0; i < 5; ++i )
	{
		for( int iBar = 0; iBar < 2; ++iBar )
		{
			const CX2UserSkillTree::SkillSlotData* pSlot = kUnitData.m_UserSkillTree.GetSkillSlot( i, 1 == iBar );
			if( NULL == pSlot )
				continue;
			const CX2SkillTree::SKILL_ID eSkillID = pSlot->m_eID;
			if( static_cast<int>( eSkillID ) <= 0 )
				continue;
			g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( kUnitData, eSkillID, i, 1 == iBar );
		}
	}
}

/// June 31f1bdb0 ( ACK 0x555 ): wait over and OK -> "Selected trait has been registered." ( 0x8b98, 250 x 300 ); with my unit, the memo goes
/// into its tree ( 328923e0( skill, memo ) ), the equipped skills are rebuilt, and the tree UI redrawn ( 328e7b70( 1, 1, 0 ) ).
bool CX2SkillTree::Handler_EGS_JUNE_ACTIVE_SKILL_MEMO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_JUNE_ACTIVE_SKILL_MEMO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( false == g_pMain->DeleteServerPacket( EGS_JUNE_ACTIVE_SKILL_MEMO_ACK ) || false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return true;

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), GET_STRING( 35736 ), g_pMain->GetNowState(), -1, -1.f );

	if( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return true;

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.AddJuneActiveMemo( kEvent.m_iSkillID, kEvent.m_iMemoID );
	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO ACK activated skill=%d memo=%d", kEvent.m_iSkillID, kEvent.m_iMemoID );

	JuneRefreshEquippedSkillsForMemo();

	if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, false );
	return true;
}

/// June 31f1c1d0 ( ACK 0x557 ): wait over and OK -> "Selected trait has been removed." ( 0x8b99 ); with my unit: the inventory takes the
/// changed slots ( 3286d590( vec, 0 ) ), the memo leaves the tree ( 32891050( skill, memo ) ), the equipped skills are rebuilt and the tree
/// UI redrawn ( 1, 1, 0 ). Then, unit or not, an open inventory window takes the slots too ( 321524d0( 0x1f ) -> 32196a00( vec, 1, 1 ) ).
bool CX2SkillTree::Handler_EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_JUNE_DEACTIVE_SKILL_MEMO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( false == g_pMain->DeleteServerPacket( EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK ) || false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return true;

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), GET_STRING( 35737 ), g_pMain->GetNowState(), -1, -1.f );

	if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
		pUnit->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
		pUnit->AccessUnitData().m_UserSkillTree.RemoveJuneActiveMemo( kEvent.m_iSkillID, kEvent.m_iMemoID );
		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO ACK deactivated skill=%d memo=%d, %d inventory slot(s)", kEvent.m_iSkillID, kEvent.m_iMemoID,
			static_cast<int>( kEvent.m_vecInventorySlotInfo.size() ) );

		JuneRefreshEquippedSkillsForMemo();

		if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, false );
	}

	if( NULL != g_pData->GetUIManager() && true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_INVEN ) &&
		NULL != g_pData->GetUIManager()->GetUIInventory() )
		g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo, true );
	return true;
}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
void CX2SkillTree::On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent )
{
	if( kEvent.m_iUnitUID <= 0 )
		return;

	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();


	if( pMyUnit->GetUID() == kEvent.m_iUnitUID )
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		/// 스킬 습득 정보 초기화 하고, 패킷으로 온 인자로 설정 후 스킬 슬롯 내 레벨 0인 스킬 해제
		if ( NULL != pMyUnit )
  	#ifdef SKILL_PAGE_SYSTEM //JHKang
		{
			int iUsingPage = pMyUnit->AccessUnitData().m_UserSkillTree.GetUsingPage();

			for ( int i = 0; i < pMyUnit->AccessUnitData().m_UserSkillTree.GetOpenedPage(); ++i )
			{
				pMyUnit->AccessUnitData().m_UserSkillTree.SetUsingPage( i + 1 );
				pMyUnit->AccessUnitData().m_UserSkillTree.ExpireCashSkillPoint( kEvent.m_vecMapHaveSKill[i] );
				int iSP = kEvent.m_vecRetrievedSPoint[i];
				pMyUnit->AccessUnitData().m_UserSkillTree.AddSkillPoint( iSP );
				pMyUnit->AccessUnitData().m_UserSkillTree.SetCashSkillPoint( 0 );

				if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
					g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
			}
			pMyUnit->AccessUnitData().m_UserSkillTree.SetUsingPage( iUsingPage + 1 );
			pMyUnit->AccessUnitData().m_iSPoint = pMyUnit->AccessUnitData().m_UserSkillTree.GetSkillPoint( iUsingPage + 1 );
			pMyUnit->AccessUnitData().m_iCSPoint = pMyUnit->AccessUnitData().m_UserSkillTree.GetCashSkillPoint( iUsingPage + 1 );

			if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
		}

	#else //SKILL_PAGE_SYSTEM
			pMyUnit->AccessUnitData().m_UserSkillTree.ExpireCashSkillPoint( kEvent.m_mapHaveSKill );
			  
		pMyUnit->AccessUnitData().m_iSPoint += kEvent.m_iRetrievedSPoint;
		pMyUnit->AccessUnitData().m_iCSPoint = 0;

		if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
	#endif //SKILL_PAGE_SYSTEM

#else // UPGRADE_SKILL_SYSTEM_2013
		pMyUnit->AccessUnitData().m_UserSkillTree.ExpireCashSkillPoint();

		pMyUnit->AccessUnitData().m_iSPoint += kEvent.m_iRetrievedSPoint;
		pMyUnit->AccessUnitData().m_iCSPoint = 0;

		if( NULL != g_pData->GetUIManager() &&
			true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
		{
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
		}
#endif // UPGRADE_SKILL_SYSTEM_2013
	}

}








#ifdef GUILD_SKILL

bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
{
	// 서버 길드 스킬 작업 완료시까지 주석
	
	//{{ oasis907 : 김상윤 [2009.11.19] // 길드마스터 아닌 등급의 길드 스킬 조정 클라이언트 차단
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}
	//}} oasis907 : 김상윤 [2009.11.19] //


	#ifdef DISABLE_REDUDANT_PACKET_TEST
	if( true == g_pMain->IsWaitingServerPacket( EGS_GET_GUILD_SKILL_ACK ) )
	{
	return false;
	}
	#endif DISABLE_REDUDANT_PACKET_TEST


	if ( g_pData->GetMyUser() == NULL )
	return false;

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
	return false;


	KEGS_GET_GUILD_SKILL_REQ kPacket;
	kPacket.m_iGuildSkillID = (int) eSkillID;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_GUILD_SKILL_ACK );

	return true;
}




bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// oasis907 : 김상윤 [2009.11.19] // ACK 구현 완료

	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );	


	g_pMain->DeleteServerPacket( EGS_GET_GUILD_SKILL_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
		return true;
	}

	return false;
	
}




bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// oasis907 : 김상윤 [2009.11.19] // NOT 구현 완료


	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );	



	// oasis907 : 김상윤 [2009.11.19] // 던전 안에서의 실시간 스킬 변경 반영, 기존의 것으로도 가능
	// 게임 룸 상태일 경우 처리
	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
	{
	#ifdef SKILL_PAGE_SYSTEM //JHKang
		pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iGuildSkillID, kEvent.m_iGuildSkillLv,
			kEvent.m_iGuildSkillCSPoint, pUnitData->m_UserSkillTree.GetUsingPage() );
	#else //SKILL_PAGE_SYSTEM
		pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iGuildSkillID, kEvent.m_iGuildSkillLv, kEvent.m_iGuildSkillCSPoint );
	#endif //SKILL_PAGE_SYSTEM
		pUnitData->m_iGuildSPoint	= kEvent.m_iGuildSPoint;
		pUnitData->m_iGuildCSPoint	= kEvent.m_iGuildCSPoint;
	}

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( (SKILL_ID)kEvent.m_iGuildSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( (SKILL_ID)kEvent.m_iGuildSkillID, kEvent.m_iGuildSkillLv );
#endif // UPGRADE_SKILL_SYSTEM_2013
	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_BUFF:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
#ifdef ADDED_RELATIONSHIP_SYSTEM
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
#endif // ADDED_RELATIONSHIP_SYSTEM
#ifdef FINALITY_SKILL_SYSTEM //JHKang
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
#endif //FINALITY_SKILL_SYSTEM
		{
			// oasis907 : 김상윤 [2009.11.27] // 길드 스킬에는 액티브 스킬 없으므로 주석
			/*
			// 장착 가능한 스킬을 새로 배웠다면
			if(kEvent.m_iSkillLevel == 1)
			{
				// 빈 슬롯을 검색해서
				int EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, false );
				if( EmptySlot != -1 )
				{
					// 장착해준다
					Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot, (CX2SkillTree::SKILL_ID)kEvent.m_iSkillID );
				}
				else if( pUnitData->m_UserSkillTree.GetEnabledSkillSlotB() )
				{
					EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, true );
					if( EmptySlot != -1 )
					{
						Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot+EQUIPPED_SKILL_SLOT_COUNT, (CX2SkillTree::SKILL_ID)kEvent.m_iSkillID );
					}
				}
			}
			*/

		} break;
	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE:
	case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
		{
			if( g_pData != NULL && 
				g_pData->GetUIManager() != NULL && 
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
			}
		} break;
	}

	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}


	// 사운드
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillUp.ogg", false, false );

	if(pUnitData->m_UserSkillTree.IsMaxSkillLevel( (SKILL_ID) kEvent.m_iGuildSkillID ))
	{
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillMaster.ogg", false, false );
		//const SkillTreeTempletMap& refSkillTreeTempletMap = GetSkillTreeTempletMap( (int)pUnitData->m_UnitClass );

		const SkillTreeTempletMap& refSkillTreeTempletMap = GetGuildSkillTreeTempletMap( (int) 0 ); // oasis
		SkillTreeTempletMap::const_iterator it = refSkillTreeTempletMap.find((SKILL_ID) kEvent.m_iGuildSkillID);
		if( it != refSkillTreeTempletMap.end() )
		{
			const CX2SkillTree::SkillTreeTemplet& refSkillTreeTemplet = it->second;

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			if(0 != refSkillTreeTemplet.m_iFollowingSkill )
			{
				// 후행 스킬이 있으면 (화살표가 열릴 것이다)
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillRootOpen.ogg", false, false );
			}
#else // UPGRADE_SKILL_SYSTEM_2013
			if(false == refSkillTreeTemplet.m_vecFollowingSkill.empty())
			{
				// 후행 스킬이 있으면 (화살표가 열릴 것이다)
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillRootOpen.ogg", false, false );
			}
#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}

	//	switch( pSkillTemplet->m_Type )
	//	{
	//	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	//	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	//	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	//	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	//		{
	//			if( g_pData != NULL && 
	//				g_pData->GetUIManager() != NULL && 
	//				g_pData->GetUIManager()->GetUICharInfo() != NULL)
	//			{
	//				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	//			}
	//		} break;
	//	}
	//}


	//// 스킬 슬롯이 비었거나 장착된 스킬보다 높은 레벨의 동일 스킬이면 바로 장착
	//EquipSkillWhenGetSkill( GetGetSkillID() );

	// oasis907 : 김상윤 [2009.12.3] // 마스터 님이 길드 스킬 포인트를 사용했습니다.
	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4795, "L", g_pData->GetGuildManager()->GetMyGuildInfo()->m_wstrGuildMasterNickName)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
	
	return true;

}







// oasis907 : 김상윤 [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
{

#ifdef DISABLE_REDUDANT_PACKET_TEST
	if( true == g_pMain->IsWaitingServerPacket( EGS_RESET_GUILD_SKILL_ACK ) )
	{
		return false;
	}
#endif DISABLE_REDUDANT_PACKET_TEST


	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}


	KEGS_RESET_GUILD_SKILL_REQ kPacket;
	kPacket.m_iGuildSkillID = (int) eSkillID;	

	g_pData->GetServerProtocol()->SendPacket( EGS_RESET_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESET_GUILD_SKILL_ACK );

	return true;
}

// oasis907 : 김상윤 [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_RESET_GUILD_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// 스킬 초기화 아이템을 사용했기 때문에 인벤 정보를 갱신한다
			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI도..
				if( NULL != g_pData->GetUIManager() &&
					true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN))
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
			}
			// 사운드
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );

		}
		return true;
	}
	return false;
}


// oasis907 : 김상윤 [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	// 스킬 트리 갱신
	int iSkillLevel = 0;
	int iSkillCSPoint = 0;
	pUnitData->m_UserSkillTree.GetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, iSkillLevel, iSkillCSPoint );
	iSkillLevel -= 1;
	if( iSkillCSPoint > 0 )
	{
		iSkillCSPoint -= kEvent.m_iRestoreGuildCSPoint;
	}
#ifdef SKILL_PAGE_SYSTEM //JHKang
	pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, iSkillLevel, iSkillCSPoint,
		pUnitData->m_UserSkillTree.GetUsingPage() ); 
#else //SKILL_PAGE_SYSTEM
	pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, iSkillLevel, iSkillCSPoint ); 
#endif //SKILL_PAGE_SYSTEM

	pUnitData->m_iGuildSPoint	+= kEvent.m_iRestoreGuildSPoint;
	pUnitData->m_iGuildCSPoint	+= kEvent.m_iRestoreGuildCSPoint;


	// UI 갱신 및 장착 스킬 갱신
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID );
#else // UPGRADE_SKILL_SYSTEM_2013
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, 1 );
#endif // UPGRADE_SKILL_SYSTEM_2013
	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE:
	case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
		{
			if( g_pData != NULL && 
				g_pData->GetUIManager() != NULL && 
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
			}
		} break;

	case CX2SkillTree::ST_BUFF:
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
#ifdef ADDED_RELATIONSHIP_SYSTEM
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
#endif // ADDED_RELATIONSHIP_SYSTEM
#ifdef FINALITY_SKILL_SYSTEM //JHKang
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
#endif //FINALITY_SKILL_SYSTEM
		{
			// oasis907 : 김상윤 [2009.11.27] // 길드 스킬에는 액티브 스킬 없으므로 주석
			/*
			for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT; i++ )
			{
				CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( i, false );
				if( NULL != pSkillSlotData )
				{
					if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
					{
						pUnitData->m_UserSkillTree.SetSkillSlot( i, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, false );
					}
				}


				pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( i, true );
				if( NULL != pSkillSlotData )
				{
					if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
					{
						pUnitData->m_UserSkillTree.SetSkillSlot( i, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, true );
					}
				}
			}
			*/
		} break;
	}

	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}

	// 사운드
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );

	int iRestoreGuildSPoint = kEvent.m_iRestoreGuildSPoint + kEvent.m_iRestoreGuildCSPoint;


	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4810, "L", g_pData->GetGuildManager()->GetMyGuildInfo()->m_wstrGuildMasterNickName)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
	// oasis907 : 김상윤 [2009.12.3] // 길드 스킬포인트가 1 생성되었습니다.
	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4797, "i", iRestoreGuildSPoint)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

	return true;
}








// oasis907 : 김상윤 [2009.11.27] //
bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_REQ( UidType itemUID )
{
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}

	KEGS_INIT_GUILD_SKILL_REQ kPacket;
	kPacket.m_iItemUID = itemUID;

	g_pData->GetServerProtocol()->SendPacket( EGS_INIT_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_INIT_GUILD_SKILL_ACK );

	return true;
}


bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_INIT_GUILD_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//CX2Unit::UnitData* pUnitData = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();


			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI도..
				if( NULL != g_pData->GetUIManager() && 
					NULL != g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN) )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList(kEvent.m_vecInventorySlotInfo);

				}
			}
	
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4798 ), g_pMain->GetNowState() );
			return true;

		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"스킬 초기화에 실패하였습니다.", m_pNowStage );
		}
		return true;
	}
	return false;
}



bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	pUnitData->m_iGuildSPoint = kEvent.m_iGuildSPoint;
	pUnitData->m_iGuildCSPoint = kEvent.m_iGuildCSPoint;

	// 스킬트리및 장착스킬 초기화
	pUnitData->m_UserSkillTree.Reset( false, true, true, false, false ); // 커밋해야함


	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}

	if( NULL != g_pData->GetUIManager() )
	{
		if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO) )
		{
			g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
		}
	}

	//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"모든 스킬을 초기화 하였습니다.", m_pNowStage );

	// oasis907 : 김상윤 [2009.12.3] // 길드 스킬이 초기화 되었습니다.
	g_pChatBox->AddChatLog( GET_STRING(STR_ID_4798), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

	return false;
}





bool CX2SkillTree::Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );	


	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

	// oasis907 : 김상윤 [2009.12.4] // 길드 가입시
	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	pUnit->AccessUnitData().m_UserSkillTree.SetGuildClass(0);
	pUnit->AccessUnitData().m_UserSkillTree.SetAcquiredGuildSkill( kEvent.m_vecGuildSkillData ); 

	// oasis907 : 김상윤 [2009.11.27] // 길드 스킬 포인트, 캐시 스킬 포인트 SET
	pUnit->AccessUnitData().m_iGuildSPoint = kEvent.m_iGuildSPoint;
	pUnit->AccessUnitData().m_iGuildCSPoint = kEvent.m_iGuildCSPoint;
	pUnit->AccessUnitData().m_iMaxGuildCSPoint = 0;
	pUnit->AccessUnitData().m_wstrGuildCSPointEndDate = L"";


	//{{ oasis907 : 김상윤 [2009.12.8] // 길드 스킬에 의한 스탯 변경 
	if( g_pData != NULL && 
		g_pData->GetUIManager() != NULL && 
		g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
	{
		g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	}

	// 길드 스킬 트리창 업데이트 
	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}
	return true;
}


/*

bool CX2SkillTree::Handler_EGS_EXPAND_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != kEvent.m_iUnitUID )
		return false;

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) kEvent.m_cSkillSlotBExpirationState );
	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );


	// 갱신 
	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}

	return true;
}


bool CX2SkillTree::Handler_EGS_EXPIRE_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != g_pData->GetMyUser() ) 
		return false;

	if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );

		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, true );

		if( g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
		{
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
		}

		switch( g_pMain->GetNowStateID() )
		{
		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
			} break;

		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_DUNGEON_RESULT:
		case CX2Main::XS_PVP_GAME:
		case CX2Main::XS_PVP_RESULT:
			{
				wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
				D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			} break;
		}
	}

	return true;
}






bool CX2SkillTree::Handler_EGS_UNSEAL_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UNSEAL_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.AddSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );


	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillTree( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );

		g_pData->GetUIManager()->GetUISkillTree()->OpenDLGSkillUnsealed( true );
		g_pData->GetUIManager()->GetUISkillTree()->UpdateDLGSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );
	}


	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}

	return true;
}
*/

//}} oasis907 : 김상윤 //// 2009-11-12 //// 
#endif GUILD_SKILL

#pragma region Skill Page System
#ifdef SKILL_PAGE_SYSTEM //JHKang
bool CX2SkillTree::Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_GET_NEXT_SKILL_PAGE_ED_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK );
	{
		if (g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( g_pData && g_pData->GetUIManager() && g_pData->GetUIManager()->GetUISkillTree() )
			{
				g_pData->GetUIManager()->GetUISkillTree()->BuySkillPageDialog( kEvent.m_iED );

				return true;
			}
		}
	}

	return false;
}

bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_PAGE_REQ()
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_EXPAND_SKILL_PAGE_ACK ) )
		return false;

	if ( g_pData && g_pData->GetMyUser() && g_pData->GetMyUser()->GetSelectUnit() 
		 && g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED >= 1000 )
	{
		g_pData->GetServerProtocol()->SendID( EGS_EXPAND_SKILL_PAGE_REQ );
		g_pMain->AddServerPacket( EGS_EXPAND_SKILL_PAGE_ACK );

		return true;
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28961 ), g_pMain->GetNowState() );

		return false;
	}
}

bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_EXPAND_SKILL_PAGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
	/// June 32214c60 ( ACK 0x479 ): the wait is cleared ( 31d71ed0, result unused ); OK and the tree UI exists -> the opened page count
	/// ( unit data +0x48e = user tree +0x22 ), the tree UI redrawn ( 328e7b70( 1, 1, 1 ) ), the notice ( STR 0x72ca = 29386 ) and the ED
	/// ( unit data +0x80 ). June adds no skill page, sets no default skill and no SP ( March's AddSkillPage / SetDefaultSkill / SetSkillPoint ).
	g_pMain->DeleteServerPacket( EGS_EXPAND_SKILL_PAGE_ACK );
	if ( false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return false;

	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetUIManager() ||
		 NULL == g_pData->GetUIManager()->GetUISkillTree() )
	{
		return false;
	}

	{
		CX2Unit::UnitData* pJuneUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
		pJuneUnitData->m_UserSkillTree.SetOpenedPage( static_cast<USHORT>( kEvent.m_iTheNumberOfSkillPagesAvailable ) );
		pJuneUnitData->m_UserSkillTree.JuneGrowSkillPages();	/// local: the new page's skill map ( X2UserSkillTree.h )

		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, true );
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29386 ), g_pMain->GetNowState() );
		pJuneUnitData->m_ED = kEvent.m_iED;

		CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE expand ACK: %d page(s) open, ED %d", kEvent.m_iTheNumberOfSkillPagesAvailable, kEvent.m_iED );
	}
	return true;
#else SERV_IRUHADEV_JUNE_SKILL_PAGE

	g_pMain->DeleteServerPacket( EGS_EXPAND_SKILL_PAGE_ACK );
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( NULL == g_pData ||
				 NULL == g_pData->GetMyUser() ||
				 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
				 NULL == g_pData->GetUIManager() ||
				 NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			if ( pUnitData == NULL )
				return false;

			int usDiffValue = kEvent.m_iTheNumberOfSkillPagesAvailable - pUnitData->m_UserSkillTree.GetOpenedPage();
			
			if ( usDiffValue > 1 )
			{
				pUnitData->m_UserSkillTree.AddSkillPage();
				pUnitData->m_UserSkillTree.AddSkillPage();

				pUnitData->m_UserSkillTree.SetOpenedPage( static_cast<USHORT>( kEvent.m_iTheNumberOfSkillPagesAvailable ) );
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );	// 각 전직별 기본 스킬 설정
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );	// 각 전직별 기본 스킬 설정

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );
			}
			else
			{
				pUnitData->m_UserSkillTree.AddSkillPage();
				pUnitData->m_UserSkillTree.SetOpenedPage( static_cast<USHORT>( kEvent.m_iTheNumberOfSkillPagesAvailable ) );
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );	// 각 전직별 기본 스킬 설정

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
			}

			if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, true );
			}

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29386 ), g_pMain->GetNowState() );

			pUnitData->m_ED = kEvent.m_iED;

			return true;
		}
	}

	return false;
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE
}

bool CX2SkillTree::Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( USHORT usPageNumber_ )
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK ) )
		return false;
#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
	/// June 3220f160: in a game, a skill-slot change still cooling down ( my unit +0x939c > 0, i.e. not IsValideSlotChange ) refuses the
	/// switch above level 10 ( vslot 0x20c ), the same guard as the slot REQ 3220e3b0. Then the REQ ( 0x47a, the page ) and the wait ( 0x47b ).
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
		false == g_pX2Game->GetMyUnit()->IsValideSlotChange() && 10 < g_pX2Game->GetMyUnit()->GetUnitLevel() )
		return false;
	CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE decide REQ: page %d", static_cast<int>( usPageNumber_ ) );
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE
	
	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ kPacket;
	kPacket.m_iSkillPagesNumberDecidedToUse = usPageNumber_;

	g_pData->GetServerProtocol()->SendPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );

	return true;
}

bool CX2SkillTree::Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
	/// June 3220f2b0 ( ACK 0x47b, deserializer 3221d290 -> 320e0620 ): the wait is cleared ( result unused ); OK and the tree UI exists ->
	/// the page goes in use and selected ( user tree +0x20 = page - 1, +0x24 = page ), the page's bars come from the ACK's
	/// KJuneSkillExtraData ( 32896130: both bars cleared, then id / level of each vector entry, at most 5 ), its active memos replace the
	/// tree's ( 32892050 + 32892410 ). June reads neither the SP / CSP fields nor m_kUserSkillPageData: the skill levels are not touched.
	g_pMain->DeleteServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );
	if ( false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return false;

	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetUIManager() ||
		 NULL == g_pData->GetUIManager()->GetUISkillTree() )
	{
		return false;
	}

	{
		CX2Unit* pJuneUnit = g_pData->GetMyUser()->GetSelectUnit();
		CX2UserSkillTree& kJuneTree = pJuneUnit->AccessUnitData().m_UserSkillTree;
		const KJuneSkillExtraData& kJunePage = kEvent.m_kJuneSkillExtraData;

		kJuneTree.SetUsingPage( static_cast<USHORT>( kEvent.m_iSkillPagesNumberDecidedToUse ) );
		kJuneTree.SetSelectPage( static_cast<USHORT>( kEvent.m_iSkillPagesNumberDecidedToUse ) );

		/// 32896130: only the first 5 entries of each vector are read; a missing entry leaves an empty slot
		KSkillData aJuneSlotA[ EQUIPPED_SKILL_SLOT_COUNT ];
		KSkillData aJuneSlotB[ EQUIPPED_SKILL_SLOT_COUNT ];
		for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; ++i )
		{
			if ( i < static_cast<int>( kJunePage.m_vecSkillDataA.size() ) )
				aJuneSlotA[i] = kJunePage.m_vecSkillDataA[i];
			if ( i < static_cast<int>( kJunePage.m_vecSkillDataB.size() ) )
				aJuneSlotB[i] = kJunePage.m_vecSkillDataB[i];
		}
		kJuneTree.SetEquippedSkill( aJuneSlotA, aJuneSlotB );

		kJuneTree.ClearJuneActiveMemo();																					/// 32892050
		kJuneTree.MergeJuneActiveMemo( std::multimap< int, int >( kJunePage.m_mapIntInt.begin(), kJunePage.m_mapIntInt.end() ) );	/// 32892410

		/// in a game ( g_pX2Game, my unit vslot 0x98 ): input is held for 0.5 s ( +0xde8 = _DAT_3343777c ) and the equipped skill states rebuilt
		/// ( vslot 0x684( 0 ) ); unit type 2 ( 32ce1770 ) and 9 ( the tree's 328e0290 ) add class pieces: checklist 7ak
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
		{
			g_pX2Game->GetMyUnit()->SetCanNotInputTime_LUA( 0.5f );
			g_pX2Game->GetMyUnit()->InitEquippedActiveSkillState( false );
		}

		/// 3264ad40 ( CX2User::ChangeEquippedSkillState ) for every bar-A entry, then, when slot-state key 0 is above 0 ( 32890ea0 ), every
		/// bar-B entry; June passes the entry's index for both bars, so a B skill is refreshed under the A slot id ( JUNE_BUGS )
		for ( UINT i = 0; i < kJunePage.m_vecSkillDataA.size(); ++i )
			g_pData->GetMyUser()->ChangeEquippedSkillState( static_cast<int>( i ), kJunePage.m_vecSkillDataA[i].m_iSkillID );
		if ( 0 < kJuneTree.GetJuneSlotState( 0 ) )
		{
			for ( UINT i = 0; i < kJunePage.m_vecSkillDataB.size(); ++i )
				g_pData->GetMyUser()->ChangeEquippedSkillState( static_cast<int>( i ), kJunePage.m_vecSkillDataB[i].m_iSkillID );
		}

		/// the tree UI, when shown ( +0xa8 ): 328e7b70( 1, 1, 1 )
		if ( true == g_pData->GetUIManager()->GetUISkillTree()->GetShow() )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, true );

		pJuneUnit->ResetIncreaseSkillLevelByBuff();		/// 323646a0
		pJuneUnit->ResetIncreaseSkillLevelBySocket();	/// 3235a080( 0 )

		CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE decide ACK: page %d in use, bar A %d / B %d entries, %d memo(s)",
			kEvent.m_iSkillPagesNumberDecidedToUse, static_cast<int>( kJunePage.m_vecSkillDataA.size() ),
			static_cast<int>( kJunePage.m_vecSkillDataB.size() ), static_cast<int>( kJunePage.m_mapIntInt.size() ) );
	}
	return true;
#else SERV_IRUHADEV_JUNE_SKILL_PAGE

	g_pMain->DeleteServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( NULL == g_pData ||
				NULL == g_pData->GetMyUser() ||
				NULL == g_pData->GetMyUser()->GetSelectUnit() ||
				NULL == g_pData->GetUIManager() ||
				NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			if ( pUnitData == NULL )
				return false;

			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iSkillPagesNumberDecidedToUse );
			pUnitData->m_UserSkillTree.SetSelectPage( kEvent.m_iSkillPagesNumberDecidedToUse );
			pUnitData->m_iSPoint = kEvent.m_iSPointAvailable;
			pUnitData->m_iCSPoint = kEvent.m_iCSPointAvailable;
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );

			BOOST_FOREACH( const KUserSkillData& userSkillData, kEvent.m_kUserSkillPageData.m_vecUserSkillData )
			{
				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( userSkillData.m_iSkillID ), 
					userSkillData.m_cSkillLevel, userSkillData.m_cSkillCSPoint, pUnitData->m_UserSkillTree.GetUsingPage() );

				bool bEnableSkillEquipped = true;
				const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( userSkillData.m_iSkillID );

				if ( eSkillId == CX2SkillTree::SI_NONE )
					continue;

				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillId );

				switch( pSkillTemplet->m_eType )
				{
				case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE:
					{
						if( g_pData != NULL && 
							g_pData->GetUIManager() != NULL && 
							g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
						{
							g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
						}
					} break;
				}
			}

			pUnitData->m_UserSkillTree.SetEquippedSkill( kEvent.m_kUserSkillPageData.m_aEquippedSkill, 
				kEvent.m_kUserSkillPageData.m_aEquippedSkillSlotB );

			if ( NULL != g_pTFieldGame && NULL != g_pData->GetMyUser() )
			{
				for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; ++i )
				{
					g_pData->GetMyUser()->ChangeEquippedSkillState( i, kEvent.m_kUserSkillPageData.m_aEquippedSkill[i].m_iSkillID );
#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10
					g_pData->GetMyUser()->ChangeEquippedSkillState( i + EQUIPPED_SKILL_SLOT_COUNT, kEvent.m_kUserSkillPageData.m_aEquippedSkillSlotB[i].m_iSkillID );
#else SERV_IRUHADEV_JUNE_SKILL_SLOT_10
					g_pData->GetMyUser()->ChangeEquippedSkillState( i, kEvent.m_kUserSkillPageData.m_aEquippedSkillSlotB[i].m_iSkillID );
#endif SERV_IRUHADEV_JUNE_SKILL_SLOT_10
				}
			}
			
			if ( true == g_pData->GetUIManager()->GetUISkillTree()->GetShow() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true );

			// 스킬 추가 레벨 효과 갱신
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelByBuff();
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();

			return true;
		}
	}

	return false;
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE
}
#endif //SKILL_PAGE_SYSTEM
#pragma endregion 페이지 확장, 결정 패킷 처리





// 태완 : 리소스 나오면 작업
wstring CX2SkillTree::GetSkillLevelTextureName( int iLevel )
{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	return L"DLG_UI_Common_Texture_NewSkill_01.tga";
#else // UPGRADE_SKILL_SYSTEM_2013
	return L"DLG_SKILL_Level_01.tga";
#endif // UPGRADE_SKILL_SYSTEM_2013

}
wstring CX2SkillTree::GetSkillLevelTextureKey( int iLevel )
{

	WCHAR wszText[16] = {0,};
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	StringCchPrintf( wszText, 16, L"LV.%d", iLevel );
#else // UPGRADE_SKILL_SYSTEM_2013
	StringCchPrintf( wszText, 16, L"LV%d", iLevel );
#endif // UPGRADE_SKILL_SYSTEM_2013
	//wsprintf( wszText, L"LV%d", iLevel );
	wstring retval = L"";
	retval += wszText;
	
	return retval;
	
}


//{{ 2009. 8. 5  최육사		봉인 스킬 아이템 정보
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

const CX2SkillTree::SKILL_ID CX2SkillTree::GetUnsealSkillItemInfo( const int iItemID ) const
{
	if ( NULL != g_pData &&
		 NULL != g_pData->GetMyUser() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 NULL != g_pData->GetSkillTree() )
	{
		/// 봉인 스킬 컨테이너에서 스킬 아이템 검색
		map<int, SealSkillItemInfo>::const_iterator mit = m_mapSealSkillItemTemplet.find( iItemID );

		if( mit == m_mapSealSkillItemTemplet.end() )	
			return CX2SkillTree::SI_NONE;

		const int iUnitClass = static_cast<CX2SkillTree::SKILL_ID>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );	/// 유닛 클레스 타입

		const std::vector<CX2SkillTree::SKILL_ID> vVal = mit->second.m_vecSkillID;		/// 해당 봉인 아이템의 봉인 스킬 컨테이너

		/// 해당 케릭터의 스킬이라면, 아이디 반환
		BOOST_FOREACH( CX2SkillTree::SKILL_ID eSkillID, vVal )
		{
			/// 스킬 아이디가 해당 캐릭터의 스킬 트리 템플릿에 존재하는지 검사
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( iUnitClass, eSkillID );

			if ( NULL != pSkillTreeTemplet )
				return eSkillID;
		}
	}

	return CX2SkillTree::SI_NONE;
}

#else // UPGRADE_SKILL_SYSTEM_2013

bool CX2SkillTree::GetUnsealSkillItemInfo( int iItemID, int& iSkillID, char& cUnitClass )
{
	map< int, SealSkillItemInfo >::const_iterator mit = m_mapSealSkillItemTemplet.find( iItemID );
	if( mit == m_mapSealSkillItemTemplet.end() )	
		return false;

	iSkillID = mit->second.m_iSkillID;
	cUnitClass = mit->second.m_cUnitClass;

	return true;
}

#endif // UPGRADE_SKILL_SYSTEM_2013
//}}

#ifdef ADDED_RELATIONSHIP_SYSTEM
/** @function : UnequipRelationSkill
	@breif : 인연 스킬 장착 해제
*/
void CX2SkillTree::UnequipRelationSkill()
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_RELATION_UNEQUIP
	/// June 3220f740: the couple skill ( 900 ) on bar A ( 32890070( 900, 0 ) ), else on bar B as its ten-slot index ( + 5 ), is taken
	/// off through the slot REQ ( 3220e3b0( index, 0 ) ); not equipped -> nothing
	if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
		int iSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, false );
		if( -1 == iSlotIndex )
		{
			iSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, true );
			if( -1 == iSlotIndex )
				return;

			iSlotIndex += 5;
		}

		Handler_EGS_CHANGE_SKILL_SLOT_REQ( iSlotIndex, CX2SkillTree::SI_NONE );
	}
#else SERV_IRUHADEV_JUNE_SKILL_RELATION_UNEQUIP
	// 인연 스킬 장착 해제
	if( NULL != g_pData && 
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
		{
			// A슬롯에서 검색
			int iSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, false );

			// 없다면 B슬롯에서 검색
			if( -1 == iSlotIndex )
				pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, true );

			// B슬롯에도 없다면 미장착 상태
			if( -1 == iSlotIndex )
				return;

			Handler_EGS_CHANGE_SKILL_SLOT_REQ( iSlotIndex, CX2SkillTree::SI_NONE );
		}
	}
#endif SERV_IRUHADEV_JUNE_SKILL_RELATION_UNEQUIP
}
#endif ADDED_RELATIONSHIP_SYSTEM

#ifdef UPGRADE_SKILL_SYSTEM_2013
/** @function	: SetSkillAbilityMap
	@brief		: SKILL_ABILITY_TYPE에 해당하는 값을 저장하는 함수
	@param		: 추가할 SKILL_ABILITY_TYPE, 추가될 SkillTemplet

*/
void CX2SkillTree::SetSkillAbilityMap( KLuaManager* pLuaManager, SKILL_ABILITY_TYPE eSkillAbility, CX2SkillTree::SkillTemplet* pSkillTemplet )
{
	if ( NULL == pLuaManager )
	{
		pLuaManager->EndTable();

		return;
	}

	vector<float>	vecSkillValue;			/// 30 레벨 까지의 스킬 효과 값을 저장하는 객체
	int				iTableIndex		= 1;	/// 테이블 인덱스
	float			fValue			= 0.f;	/// 파싱된 값

	while( pLuaManager->GetValue( iTableIndex, fValue ) == true )		/// 30개의 인자값 파싱
	{
		vecSkillValue.push_back( fValue );
		++iTableIndex;
	}

	pLuaManager->EndTable();				/// 테이블 닫기

	if ( true == vecSkillValue.empty() )	/// 만약 값을 하나도 받지 못하였다면, 0으로 가득 채우자.
	{
		ASSERT( !"SKILL_ABILITY_TYPE Value is Empty!" );	/// 값이 없어요!
		vecSkillValue.push_back( 0.f );		/// 0으로 모두 채우기 위해, 첫번째 인자 추가
	}

	if ( MAX_LIMITED_SKILL_LEVEL >= iTableIndex )	/// 만약 인자수가 최대 레벨 만큼 없다면, 이전 인덱스의 인자로 채워 버리자.
	{
		for( ; iTableIndex > MAX_LIMITED_SKILL_LEVEL; ++iTableIndex )
		{
			vecSkillValue.push_back( vecSkillValue[iTableIndex - 1] );
		}
	}

	/// 인자값 추가
	pSkillTemplet->m_mapSkillAbility.insert( std::make_pair( eSkillAbility, vecSkillValue ) );
}

/** @function	: SetSkillVlaue
	@brief		: 스킬 정보에 해당하는 값을 저장하는 함수
	@param		: 저장할 컨테이너
*/
template< typename T >
#ifdef HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC // 김태환
	void CX2SkillTree::SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue, bool bAutoFill /*= true*/ )
#else //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	void CX2SkillTree::SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue )
#endif //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
{
	/*if ( NULL != pLuaManager )
	{
		pLuaManager.EndTable();

		return;
	}*/

	vecValue.clear();
	int				iTableIndex		= 1;	/// 테이블 인덱스
	T				fValue;					/// 파싱된 값

	while( pLuaManager.GetValue( iTableIndex, fValue ) == true )		/// 30개의 인자값 파싱
	{
		vecValue.push_back( fValue );
		++iTableIndex;
	}

	pLuaManager.EndTable();				/// 테이블 닫기

	/// 만약 인자수가 최대 레벨 만큼 없고, 자동 채우기를 설정 했다면 이전 인덱스의 인자로 채워 버리자.
#ifdef HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC // 김태환
	if ( true != vecValue.empty() && MAX_LIMITED_SKILL_LEVEL >= iTableIndex && true == bAutoFill )
#else //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	if ( true != vecValue.empty() && MAX_LIMITED_SKILL_LEVEL >= iTableIndex )
#endif //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	{
		for( iTableIndex = 1; iTableIndex < MAX_LIMITED_SKILL_LEVEL; ++iTableIndex )
		{
			vecValue.push_back( vecValue[iTableIndex - 1] );
		}
	}
}

/** @function	: SetSkillStat
	@brief		: 스킬 스텟 수치를 저장하는 함수
	@param		: 저장할 컨테이너
*/
void CX2SkillTree::SetSkillStat( KLuaManager& pLuaManager, vector<CX2Stat::Stat>& vecStat )
{
	/// 각 스텟 인자들
	vector<float> vecBaseHP;
	vector<float> vecAtkPhysic;
	vector<float> vecAtkMagic;
	vector<float> vecDefPhysic;
	vector<float> vecDefMagic;

	vecStat.clear();

	/// 스탯 수치 파싱
	if( pLuaManager.BeginTable( "m_Stat" ) == true )
	{
		if( true == pLuaManager.BeginTable( "m_BaseHP" ) )			SetSkillVlaue( pLuaManager, vecBaseHP );
		if( true == pLuaManager.BeginTable( "m_AtkPhysic" ) )		SetSkillVlaue( pLuaManager, vecAtkPhysic );
		if( true == pLuaManager.BeginTable( "m_AtkMagic" ) )		SetSkillVlaue( pLuaManager, vecAtkMagic );
		if( true == pLuaManager.BeginTable( "m_DefPhysic" ) )		SetSkillVlaue( pLuaManager, vecDefPhysic );
		if( true == pLuaManager.BeginTable( "m_DefMagic" ) )		SetSkillVlaue( pLuaManager, vecDefMagic );	

		pLuaManager.EndTable(); // m_Stat
	}

	/// 스텟 벡터에 스텟 수치 대입
	for ( int i = 0; i < MAX_LIMITED_SKILL_LEVEL; ++i )
	{
		/// 만약 스텟 벡터에 값이 없으면, 0으로 대입
		CX2Stat::Stat statvalue;
		statvalue.m_fBaseHP		= 0 <= i && i < static_cast<int>( vecBaseHP.size() ) ? vecBaseHP[i] : 0.f;
		statvalue.m_fAtkPhysic	= 0 <= i && i < static_cast<int>( vecAtkPhysic.size() ) ? vecAtkPhysic[i] : 0.f;
		statvalue.m_fAtkMagic	= 0 <= i && i < static_cast<int>( vecAtkMagic.size() ) ? vecAtkMagic[i] : 0.f;
		statvalue.m_fDefPhysic	= 0 <= i && i < static_cast<int>( vecDefPhysic.size() ) ? vecDefPhysic[i] : 0.f;
		statvalue.m_fDefMagic	= 0 <= i && i < static_cast<int>( vecDefMagic.size() ) ? vecDefMagic[i] : 0.f;

		vecStat.push_back( statvalue );
	}
}

/** @function	: FixOrInsertMapSkillInfoValue
	@brief		: 스킬 습득 혹은 강화시 서버로 넘기기 위한 변동값 저장 컨테이너의 값 수정 혹은 추가 함수
	@param		: 저장할 구조체
*/
void CX2SkillTree::FixOrInsertMapSkillInfoValue( CX2SkillTree::SKILL_ID eSkillID )
{
	if (	NULL == g_pData ||
			NULL == g_pData->GetSkillTree() ||
			NULL == g_pData->GetUIManager() ||
			NULL == g_pData->GetUIManager()->GetUISkillTree() ||
			NULL == g_pData->GetMyUser() ||
			NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	const CX2UserSkillTree&	refUserSkillTree	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;				/// 유저의 스킬 목록
	const int				iSkillLevel			= g_pData->GetUIManager()->GetUISkillTree()->GetSkillLevelInSkillWimdow( eSkillID );	/// 현재 스킬의 레벨

	int						iUnitClass			= static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );				/// 유닛 클래스
	const CX2SkillTree::SkillTreeTemplet*		pSkillTreeTemplet	= g_pData->GetSkillTree()->GetSkillTreeTemplet( iUnitClass, eSkillID );	/// 스킬 트리 템플릿

	/// 만약 올릴 레벨의 스킬이 한계 레벨을 넘어선다면, 패스
	if ( NULL != pSkillTreeTemplet && pSkillTreeTemplet->m_iMaxSkillLevel < iSkillLevel + 1 )
		return;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );				/// 스킬 템플릿

	if ( NULL != pSkillTemplet )
	{
		const int iUnitLevel	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level;						/// 현재 유저 레벨

		/// 만약 올릴 레벨의 스킬이 요구 레벨 제한에 걸리면, 패스
		if ( pSkillTemplet->GetSkillRequireLevelValue( iSkillLevel + 1 ) > iUnitLevel )
			return;

		const int iHaveSPoint	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSPoint - m_iUsedSPoint;		/// 남은 스킬 포인트
		const int iHaveCSPoint	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint - m_iUsedCSPoint;	/// 남은 캐시 스킬 포인트

		/// 소비 스킬 포인트 연산 ( 습득인지 강화인지 )
		const int	iRequireSP			= 0 == iSkillLevel ? pSkillTemplet->m_iRequireLearnSkillPoint : pSkillTemplet->m_iRequireUpgradeSkillPoint;
		int			iSpendSkillCSPoint	= 0;		/// 구조체에 저장할 캐시 스킬 포인트 사용 값

		/// 스킬 포인트 소비 처리
		if ( 0 < iHaveCSPoint )  /// 캐시 스킬 포인트가 있을 때
		{
			if ( iRequireSP <= iHaveCSPoint )	/// 캐시 스킬 포인트가 충분히 있을 때
			{
				iSpendSkillCSPoint	=  iRequireSP;	/// 사용된 캐시 스킬 포인트 설정
				m_iUsedCSPoint		+= iRequireSP;	/// 캐시 스킬 포인트 소비
			}
			else		/// 캐시 스킬 포인트가 부족할 때
			{
				iSpendSkillCSPoint	=  iHaveCSPoint;					/// 남은 캐시 스킬 포인트 전부를 사용
				m_iUsedCSPoint		+= iSpendSkillCSPoint;				/// 캐시 스킬 포인트 소비
				m_iUsedSPoint		+= iRequireSP - iSpendSkillCSPoint;	/// 부족한 포인트를 스킬 포인트로 소비 
			}
		}
		else		/// 캐시 포인트가 없다면, 일반 스킬 포인트 소비
		{
			m_iUsedSPoint += iRequireSP;
		}

		std::map<int, KGetSkillInfo>::iterator it = m_mapGetSkillInfo.find( static_cast<int>( eSkillID ) );

		if ( it != m_mapGetSkillInfo.end() )	/// 해당 스킬이 존재하면, 필요한 값만 수정
		{
			KGetSkillInfo&						getSkillInfo	= it->second;

			const CX2SkillTree::SkillTemplet*	pSkillTemplet	= g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

			getSkillInfo.m_iSkillLevel			+= 1;
			getSkillInfo.m_iSpendSkillCSPoint	+= iSpendSkillCSPoint;

		}
		else									/// 해당 스킬이 존재하지 않으면, 맵에 추가
		{
			KGetSkillInfo getSkillInfo;

			getSkillInfo.m_iSkillID				=  static_cast<int>( eSkillID );
			getSkillInfo.m_iBeforeSkillLevel	=  iSkillLevel;
			getSkillInfo.m_iSkillLevel			=  iSkillLevel + 1;
			getSkillInfo.m_iSpendSkillCSPoint	+= iSpendSkillCSPoint;

			m_mapGetSkillInfo.insert( std::make_pair( static_cast<int>( getSkillInfo.m_iSkillID ), getSkillInfo ) );

#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL
			if( iSkillLevel == 0 )	// 새로 배운 스킬이면
			{
				m_vecNowLearnSkill.push_back(getSkillInfo.m_iSkillID);
			}
#endif SERV_SUB_QUEST_LEARN_NEW_SKILL
		}
	}
	else
	{
		ASSERT( !"FixOrInsertMapSkillInfoValue() SkillTemplet Is Error!!!" );
	}
}

/** @function	: AddSkillPowerRate_LUA
	@brief		: 직업별 데미지 배율 테이블 파싱 함수
*/
void CX2SkillTree::AddSkillPowerRate_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	SKILL_POWER_RATE_TYPE	eSkillPowerRateType = SPRT_NONE;	/// 데미지 배율 아이디
	vector<float>			vecPoewrRate;						/// 데미지 배율 컨테이너

	LUA_GET_VALUE_ENUM( luaManager,	"m_eID", eSkillPowerRateType, SKILL_POWER_RATE_TYPE, SPRT_NONE );

#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	/// June 32209d80: fill flag 0 ( XOR EBX,EBX @32209e53, pushed @32209e8c ), no padding to 30; 32207430 then copies only a 30-entry row
	if( true == luaManager.BeginTable( "m_Value" ) )
		SetSkillVlaue( luaManager, vecPoewrRate, false );
#else SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	if( true == luaManager.BeginTable( "m_Value" ) )
		SetSkillVlaue( luaManager, vecPoewrRate );
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET

	if ( SPRT_NONE != eSkillPowerRateType && true != vecPoewrRate.empty() )		/// 정보가 파싱 되었을 때만, 추가 한다.
		m_mapSkillPowerRateType.insert( make_pair( eSkillPowerRateType, vecPoewrRate ) );
}

void CX2SkillTree::SetSkillPowerRate( IN SKILL_POWER_RATE_TYPE eSkillPowerRateType, OUT vector<float>& vecPowerRate )
{
	std::map<SKILL_POWER_RATE_TYPE, vector<float>>::iterator it = m_mapSkillPowerRateType.find( eSkillPowerRateType );

	if ( it != m_mapSkillPowerRateType.end() )
	{
		vector<float> vecValue = it->second;
		
		if ( vecValue.size() == MAX_LIMITED_SKILL_LEVEL )		/// 30레벨 정보 다 있으면, 대입 시켜 버리자
			vecPowerRate = vecValue;
		else
			ASSERT( !"PowerRate Value Size is Error!!!" );		/// 데미지 배율 정보 파싱 혹은 설정이 잘못된것 같은데요!!!
	}
	else
		ASSERT( !"PowerRate Value Size is Error!!!" );			/// 데미지 배율 정보 파싱 혹은 설정이 잘못된것 같은데요!!!
}

/** @function	: AddDefaultSkill_LUA
	@brief		: 직업별 기본 스킬 테이블 파싱 함수
*/
void CX2SkillTree::AddDefaultSkill_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING	
    TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	int iTableIndex = 1;	/// 테이블 인덱스 ( Lua는 테이블 첫번째 인덱스가 1 )
	int iValue		= -1;	/// 전달값
	int iUnitClass	= -1;	/// 유닛 클래스
	std::vector<int> vecDefaultSkill;

	while( luaManager.GetValue( iTableIndex, iValue ) == true )
	{
		if ( 1 == iTableIndex )		/// 첫번째 인덱스는 클레스 값
			iUnitClass = static_cast<CX2Unit::UNIT_CLASS>( iValue );
		else						/// 이후로는 스킬 아이디
		{
			if ( 0 != iValue )
				vecDefaultSkill.push_back( iValue );
		}

		++iTableIndex;
	}

	m_mapDefaultSkill.insert( std::make_pair( iUnitClass, vecDefaultSkill ) );
}

/** @function	: AddDefaultSkill_LUA
	@brief		: 해당 스킬 아이디가 기본 스킬인지 검사
	@param		: 검사할 스킬 아이디
	@return		: 기본 스킬 여부
*/
bool CX2SkillTree::isDefaultSkill( SKILL_ID eSkillID )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
		 return false;

	const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();


	/// 해당 클래스의 기본 스킬 목록 반환
	const std::map<int, std::vector<int>>::const_iterator mit = m_mapDefaultSkill.find( pUnitData->m_UnitClass );

	if ( mit != m_mapDefaultSkill.end() )
	{
		std::vector<int> vecDefaultSkill = mit->second;		/// 기본 스킬 목록

		/// 인자로 받은 아이디가 기본 스킬 목록에 있는지 검사
		BOOST_FOREACH( int iVal, vecDefaultSkill )
		{
			if ( eSkillID == static_cast<SKILL_ID>( iVal ) )
				return true;		/// 일치하는 아이디가 있으므로, 기본 스킬이다.
		}
	}

	return false;
}

/** @function	: 
	@brief		: 현재 케릭터의 스킬인지 검사
	@param		: 검사할 스킬 아이디
	@return		: 자신의 스킬 여부
*/
bool CX2SkillTree::IsMyClassSkillID( SKILL_ID eSkillID )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return false;
	}

	/// 해당 스킬 아이디가 현제 케릭터의 스킬인지 검사
	const CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

	const SkillTreeTempletMap& mapSkillTreeTemplet = GetSkillTreeTempletMap( static_cast<int>( eUnitClass ) );

	SkillTreeTempletMap::const_iterator mit = mapSkillTreeTemplet.find( eSkillID );

	if ( mapSkillTreeTemplet.end() != mit )
		return true;

	return false;
}

/** @function	: GetUnsealSkillItemRequireClass
	@brief		: 해당 봉인 스킬 아이템이 요구하는 유닛 클래스 레벨 반환
	@param		: 아이템 아이디
	@return		: 유닛 클래스 레벨
*/
const int CX2SkillTree::GetUnsealSkillItemRequireClass( const int iItemID ) const
{
	std::map< int, SealSkillItemInfo >::const_iterator mit = m_mapSealSkillItemTemplet.find( iItemID );

	if ( mit != m_mapSealSkillItemTemplet.end() )
		return mit->second.m_iUnitClassType;

	return 0;
}

/** @function	: GetCalcInitSkillPoint
	@brief		: 레벨에 따른 총 스킬 포인트 반환
	@param		: 유닛 레벨
	@return		: 총 스킬 포인트
*/
const int CX2SkillTree::GetCalcInitSkillPoint( const int iUnitLevel ) const
{
	int iSkillPoint = 0;

	for( int iLevel = 2 ; iLevel <= iUnitLevel ; ++iLevel )
	{
		int iLevelSkillPoint = lua_tinker::call<int>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalcLevelUpIncreaseSkillPoint", iLevel );
		iSkillPoint += iLevelSkillPoint;
	}

	return iSkillPoint;
}

#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
UINT CX2SkillTree::GetSkillLevelUpNumByPowerRateType( SKILL_POWER_RATE_TYPE eSkillPowerRateType_ )
{
	switch( eSkillPowerRateType_ )
	{
	case SPRT_BASIC_CLASS:			/// 기본 전직
		{
			return 4;
		} break;
	case SPRT_FIRST_CLASS:			/// 1차전직
		{
			return 3;
		} break;
	case SPRT_SECOND_CLASS:			/// 2차전직
		{
			return 2;
		} break;
	case SPRT_SECOND_CLASS2:			/// 2차전직2 (후반 스킬) 
		{
			return 1;
		} break;
	// 각성기는 레벨업 소켓에 고려되지 않았기 때문에 주석 처리
// 	case SPRT_HYPER_SKILL:			/// 각성기
// 		{
// 			return 1;
// 		} break;
	default:
		return 0;
		break;
	}
	return 0;
}
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE


#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
bool CX2SkillTree::SkillTemplet::GetSkillEffectiveDescExpand ( OUT wstring & wstrSkillEffectiveDesc, int iLevel ) const
{
	// 스킬 설명을 확장해야 할 스킬 인가?
	// Ex. 디멘션 위치, 에너지 스퍼트
	switch ( m_eID )
	{
	case CX2SkillTree::SI_SA_ABM_ENERGY_SPURT:	
		// 혹시 모를 크래시를 위해 예외 처리..
		if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			float fEnergySpurtAddDamageValue = powf ( static_cast<float> ( g_pData->GetSelectUnitLevel() ), 2.f ) * 35.f * GetSkillPowerRateValue( iLevel );

			wstrSkillEffectiveDesc = g_pMain->GetReplacedWstring ( wstrSkillEffectiveDesc, "i", static_cast<int> ( fEnergySpurtAddDamageValue ) );			
			return true;
		}			
		break;
	default :
		return false;
	}
	return false;
}
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-10-18
// Description: June skill system core, Phase S2 (JUNE_SKILL_SYSTEM_PHASES.md). June's skill templet loader (32216320) with its stored
//              fields (m_DeleteSkillIDByUpgrade, NUMERICAL_DATA<n>, m_ExtraIconInfo<n>, SA 143-202), the upgrade map (32205c00) and the
//              numerical-first ability lookup (32206e50 / 32206fb0). Data stays March's; see MODS.md "June skill templet".
namespace
{
	/// June 32216320: the SA_* tables in June's parse order (june2017_re/s0a_templet_keys.tsv rows 39-240). m_iType is the stored
	/// ability id. SA_TEAM_ATK_MAGIC_REL is stored as 13 (SA_TEAM_DEF_PHYSIC_REL): a studio quirk that March has too, kept.
	struct JuneSkillAbilityKey
	{
		const char* m_pszKey;
		int         m_iType;
	};

	const JuneSkillAbilityKey s_aJuneSkillAbilityKey[] =
	{
		{ "SA_EFFECTIVE_TIME",	1 },
		{ "SA_EFFECTIVE_COUNT",	2 },
		{ "SA_SKILL_ID",	3 },
		{ "SA_COMBO_ID",	4 },
		{ "SA_EFFECTIVE_RATE",	5 },
		{ "SA_ATK_REL",	6 },
		{ "SA_ATK_PHYSIC_REL",	7 },
		{ "SA_ATK_MAGIC_REL",	8 },
		{ "SA_DEF_PHYSIC_REL",	9 },
		{ "SA_DEF_MAGIC_REL",	10 },
		{ "SA_TEAM_ATK_PHYSIC_REL",	11 },
		{ "SA_TEAM_ATK_MAGIC_REL",	13 },
		{ "SA_TEAM_DEF_PHYSIC_REL",	13 },
		{ "SA_TEAM_DEF_MAGIC_REL",	14 },
		{ "SA_DAMAGE_REL",	15 },
		{ "SA_DAMAGED_REL",	16 },
		{ "SA_UNFIXED_DEFENSE_REL",	17 },
		{ "SA_SUMMON_ATK_REL",	18 },
		{ "SA_MAX_HP_ABS",	19 },
		{ "SA_MAX_HP_REL",	20 },
		{ "SA_MAX_MP_ABS",	21 },
		{ "SA_MAX_MP_REL",	22 },
		{ "SA_TELEPORT_RANGE_ABS",	23 },
		{ "SA_CHARGING_TIME",	24 },
		{ "SA_HP_REL_TO_MP_ABS_1",	25 },
		{ "SA_HP_REL_TO_MP_ABS_2",	26 },
		{ "SA_MP_CONSUME_REL",	27 },
		{ "SA_MP_GAIN_ON_HIT_REL",	28 },
		{ "SA_SCOPE_ABS",	29 },
		{ "SA_PROJECTILE_FIRE_COUNT",	30 },
		{ "SA_PROJECTILE_RANGE_REL",	31 },
		{ "SA_PROJECTILE_RANGE_REL_2",	144 },
		{ "SA_PROJECTILE_SIZE",	32 },
		{ "SA_PROJECTILE_PIERCING",	33 },
		{ "SA_MOVE_SPEED",	34 },
		{ "SA_JUMP_SPEED",	35 },
		{ "SA_ATK_PHYSIC_TO_ATK_MAGIC",	36 },
		{ "SA_ATK_MAGIC_TO_ATK_PHYSIC",	37 },
		{ "SA_READY_TIME_REL",	38 },
		{ "SA_FORCE_DOWN_REL",	39 },
		{ "SA_FORCE_DOWN_MELEE_REL",	40 },
		{ "SA_FORCE_DOWN_RANGE_REL",	41 },
		{ "SA_FORCE_DOWN_ABS",	42 },
		{ "SA_MP_REGENERATION_ABS",	43 },
		{ "SA_MP_INTAKE_ABS",	44 },
		{ "SA_TARGET_ANI_SPEED",	45 },
		{ "SA_TARGET_MOVE_SPEED",	46 },
		{ "SA_TARGET_JUMP_SPEED",	47 },
		{ "SA_TARGET_ATK_REL",	48 },
		{ "SA_HP_INTAKE_REL_DAMAGE",	49 },
		{ "SA_ATK_CRITICAL_DAM_ABS",	50 },
		{ "SA_ATK_CRITICAL_RATE_ABS",	51 },
		{ "SA_DEF_CRITICAL_DAM_ABS",	52 },
		{ "SA_DEF_CRITICAL_RATE_ABS",	53 },
		{ "SA_BACK_SPEED_X",	54 },
		{ "SA_ITEM_SPECIAL_ABILITY_REL",	55 },
		{ "SA_MP_GAIN_GET_HIT_REL",	56 },
		{ "SA_SOUL_GAIN_ON_HIT_REL",	57 },
		{ "SA_SOUL_GAIN_GET_HIT_REL",	58 },
		{ "SA_HP_GAIN_REL_MAX_HP",	59 },
		{ "SA_ED_GAIN_REL",	60 },
		{ "SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL",	61 },
		{ "SA_EXP_GAIN_REL",	62 },
		{ "SA_DAMAGE_ABSORB_REL_MAX_HP",	63 },
		{ "SA_STRONG_MIND_ACTIVE_REL",	64 },
		{ "SA_STRONG_MIND_SPECIAL_ACTIVE_REL",	65 },
		{ "SA_MP_RETAKE_ABS",	66 },
		{ "SA_RESIST_FIRE_ABS",	67 },
		{ "SA_RESIST_WATER_ABS",	68 },
		{ "SA_RESIST_GREEN_ABS",	69 },
		{ "SA_RESIST_WIND_ABS",	70 },
		{ "SA_RESIST_LIGHT_ABS",	71 },
		{ "SA_RESIST_DARK_ABS",	72 },
		{ "SA_ATK_MAGIC_FIRST_REL",	73 },
		{ "SA_ATK_MAGIC_SECOND_REL",	74 },
		{ "SA_ATK_MAGIC_THIRD_REL",	75 },
		{ "SA_REMOTE_SPEED_ABS",	76 },
		{ "SA_DAMAGE_TYPE_CHANGE",	77 },
		{ "SA_DAMAGE_TIME",	78 },
		{ "SA_RECOVER_MP_ABS",	79 },
		{ "SA_TARGET_MANA",	80 },
		{ "SA_TARGET_MANA_REL",	81 },
		{ "SA_SIZE_ABS",	82 },
		{ "SA_REFLECT_REL",	83 },
		{ "SA_EL_DEFENCE_ABS",	84 },
		{ "SA_TARGET_HP_REL",	85 },
		{ "SA_TARGET_HITRATE_REL",	86 },
		{ "SA_CHARGE_CANNONBALL",	87 },
		{ "SA_FIRE_DEFENCE_ABS",	88 },
		{ "SA_WATER_DEFENCE_ABS",	89 },
		{ "SA_NATURE_DEFENCE_ABS",	90 },
		{ "SA_WIND_DEFENCE_ABS",	91 },
		{ "SA_LIGHT_DEFENCE_ABS",	92 },
		{ "SA_DARK_DEFENCE_ABS",	93 },
		{ "SA_EDT_FIRE_RESIST_REL",	94 },
		{ "SA_EDT_FROZEN_RESIST_REL",	95 },
		{ "SA_EDT_ICE_RESIST_REL",	96 },
		{ "SA_EDT_POISON_RESIST_REL",	97 },
		{ "SA_EDT_Y_PRESSED_RESIST_REL",	98 },
		{ "SA_EDT_LEG_WOUND_RESIST_REL",	99 },
		{ "SA_EDT_STUN_RESIST_REL",	100 },
		{ "SA_EDT_CURSE_RESIST_REL",	101 },
		{ "SA_EXPAND_CB",	102 },
		{ "SA_ANI_SPEED",	103 },
		{ "SA_ACCURACY_PERCENT_ABS",	104 },
		{ "SA_CRITICAL_EVASION_REL",	105 },
		{ "SA_DECREASE_REL",	106 },
		{ "SA_ATK_ADD_DAMAGE_FIRST_ABS",	107 },
		{ "SA_ATK_ADD_DAMAGE_SECOND_ABS",	108 },
		{ "SA_ATK_ADD_DAMAGE_THIRD_ABS",	109 },
		{ "SA_ATK_ADD_DAMAGE_FOURTH_ABS",	110 },
		{ "SA_ATK_ADD_DAMAGE_FIFTH_ABS",	111 },
		{ "SA_TELEPORT_SPEED_REL",	112 },
		{ "HP_CONSUME_REL",	113 },
		{ "SA_MP_BURN_ABS",	114 },
		{ "SA_SKILLDAMAGE_MULTIPLE_01",	115 },
		{ "SA_SKILLDAMAGE_MULTIPLE_02",	116 },
		{ "SA_SKILLDAMAGE_MULTIPLE_03",	117 },
		{ "SA_SKILLDAMAGE_MULTIPLE_04",	118 },
		{ "SA_SKILLDAMAGE_MULTIPLE_01_RATE",	119 },
		{ "SA_SKILLDAMAGE_MULTIPLE_02_RATE",	120 },
		{ "SA_SKILLDAMAGE_MULTIPLE_03_RATE",	121 },
		{ "SA_SKILLDAMAGE_MULTIPLE_04_RATE",	122 },
		{ "SA_EVASION_REL",	123 },
		{ "SA_REDUCE_EDT_TIME_REL",	124 },
		{ "SA_OVERHEAT_HP",	125 },
		{ "SA_OVERHEAT_HP_PVP",	145 },
		{ "SA_DECREASE_REL_2",	146 },
		{ "SA_SCOPE_ABS_HYPER",	126 },
		{ "SA_EFFECTIVE_TIME_HYPER",	127 },
		{ "SA_AIR_MOVE_COUNT",	128 },
		{ "SA_NOT_CANNONBALL_REL",	129 },
		{ "SA_INSERT_CANNONBALL_REL",	130 },
		{ "SA_BE_SHOT_CANNONBALL_REL",	131 },
		{ "SA_CANNONBALL_DAMAGE_UP_REL",	132 },
		{ "SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE",	133 },
		{ "SA_SIEGE_SHELLING_REL",	134 },
		{ "SA_EFFECTIVE_DAMAGED_ATTACK_BASE",	135 },
		{ "SA_FIRST_HYPER_EFFECTIVE",	136 },
		{ "SA_SECOND_HYPER_EFFECTIVE",	137 },
		{ "SA_THIRD_HYPER_EFFECTIVE",	138 },
		{ "SA_ATK_CRITICAL_RATE_ATK_BASE",	139 },
		{ "SA_PROJECTILE_CURVE_SPEED_REL",	140 },
		{ "SA_DP_GAIN_REL",	141 },
		{ "SA_DP_USE_REL",	142 },
		{ "SA_MP_CONSUMPTION",	143 },
		{ "SA_WAY_OF_SWORD_POINT_MULTIPLIER",	147 },
		{ "SA_COOLTIME_1",	148 },
		{ "SA_COOLTIME_2",	149 },
		{ "SA_COOLTIME_3",	150 },
		{ "SA_MP_GAIN_ON_HIT_REL_2",	151 },
		{ "SA_MP_GAIN_ON_HIT_REL_3",	152 },
		{ "SA_USE_DP_REL_AT_FORMATION_MODE",	153 },
		{ "SA_RECOVERY_VALUE_REL",	154 },
		{ "SA_ADD_MP_VALUE_ABS",	155 },
		{ "SA_ADD_DP_VALUE_ABS",	156 },
		{ "SA_SCOPE_ABS_PVP",	157 },
		{ "SA_SCOPE_ABS_Y",	158 },
		{ "SA_SCOPE_ABS_Z",	159 },
		{ "SA_MP_REGENERATION_ABS_PVP",	160 },
		{ "SA_ATK_PHYSIC_REL_PVP",	161 },
		{ "SA_ATK_MAGIC_REL_PVP",	162 },
		{ "SA_DEF_PHYSIC_REL_PVP",	163 },
		{ "SA_DEF_MAGIC_REL_PVP",	164 },
		{ "SA_TEAM_ATK_PHYSIC_REL_PVP",	165 },
		{ "SA_TEAM_ATK_MAGIC_REL_PVP",	166 },
		{ "SA_TEAM_DEF_PHYSIC_REL_PVP",	167 },
		{ "SA_TEAM_DEF_MAGIC_REL_PVP",	168 },
		{ "SA_TIME_ABS",	169 },
		{ "SA_SOUL_ABS",	170 },
		{ "SA_MAX_HP_REL_PVP",	171 },
		{ "SA_RECOVER_MP_ABS_2",	172 },
		{ "SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE",	173 },
		{ "SA_MAX_INCREASE_MOVEMENT_BY_MANA",	174 },
		{ "SA_MIN_INCREASE_MOVEMENT_BY_MANA",	175 },
		{ "SA_CHAIN_SKILL_ANI_SPEED_REL",	176 },
		{ "SA_HIT_COUNT_1",	177 },
		{ "SA_HIT_COUNT_2",	178 },
		{ "SA_HIT_COUNT_3",	179 },
		{ "SA_FORCE_DOWN_GAIN_REL",	180 },
		{ "SA_MUTATION_CONSUME_MAX_COUNT",	181 },
		{ "SA_DAMAGED_REL_PVP",	182 },
		{ "SA_ECP_REL",	183 },
		{ "SA_SUMMON_ALL_STAT_REL",	184 },
		{ "SA_ATK_PHYSIC_INCREASE_BY_ADDITIONALATTACK",	189 },
		{ "SA_NOT_EQUIP_WITH_SKILLID_SAME_TIME",	190 },
		{ "SA_FP_CANCEL_SKILL",	188 },
		{ "SA_FP_UP_ABS",	191 },
		{ "SA_FP_DOWN_ABS",	192 },
		{ "SA_EFFECTIVE_VALUE_REL",	193 },
		{ "SA_CRITICAL_REL",	194 },
		{ "SA_CRITICAL_DAMAGE_REL",	195 },
		{ "SA_MAXIMIZE_REL",	196 },
		{ "SA_ADD_LEARN_SKILL_LIST_1",	185 },
		{ "SA_ADD_LEARN_SKILL_LIST_2",	186 },
		{ "SA_ADD_LEARN_SKILL_LIST_3",	187 },
		{ "SA_DEGREE_X",	197 },
		{ "SA_DEGREE_Y",	198 },
		{ "SA_DEGREE_Z",	199 },
		{ "SA_DEGREE_MAX",	200 },
		{ "SA_DFG_GAIN_REL",	201 },
		{ "SA_DFG_CONSUME_REL",	202 },
	};

	/// June g_SkillAbilityName[ 203 ] (3379e6b0, filled in the CX2SkillTree ctor 3221afb0), indexed by SKILL_ABILITY_TYPE. It decides which
	/// NUMERICAL_DATA block belongs to an ability. Slot 12 is the name March's enum gives it; 13 is where the quirk above stores it.
	const WCHAR* const s_apwszJuneSkillAbilityName[ 203 ] =
	{
		L"SA_INVALID", L"SA_EFFECTIVE_TIME", L"SA_EFFECTIVE_COUNT", L"SA_SKILL_ID",
		L"SA_COMBO_ID", L"SA_EFFECTIVE_RATE", L"SA_ATK_REL", L"SA_ATK_PHYSIC_REL",
		L"SA_ATK_MAGIC_REL", L"SA_DEF_PHYSIC_REL", L"SA_DEF_MAGIC_REL", L"SA_TEAM_ATK_PHYSIC_REL",
		L"SA_TEAM_ATK_MAGIC_REL", L"SA_TEAM_DEF_PHYSIC_REL", L"SA_TEAM_DEF_MAGIC_REL", L"SA_DAMAGE_REL",
		L"SA_DAMAGED_REL", L"SA_UNFIXED_DEFENSE_REL", L"SA_SUMMON_ATK_REL", L"SA_MAX_HP_ABS",
		L"SA_MAX_HP_REL", L"SA_MAX_MP_ABS", L"SA_MAX_MP_REL", L"SA_TELEPORT_RANGE_ABS",
		L"SA_CHARGING_TIME", L"SA_HP_REL_TO_MP_ABS_1", L"SA_HP_REL_TO_MP_ABS_2", L"SA_MP_CONSUME_REL",
		L"SA_MP_GAIN_ON_HIT_REL", L"SA_SCOPE_ABS", L"SA_PROJECTILE_FIRE_COUNT", L"SA_PROJECTILE_RANGE_REL",
		L"SA_PROJECTILE_SIZE", L"SA_PROJECTILE_PIERCING", L"SA_MOVE_SPEED", L"SA_JUMP_SPEED",
		L"SA_ATK_PHYSIC_TO_ATK_MAGIC", L"SA_ATK_MAGIC_TO_ATK_PHYSIC", L"SA_READY_TIME_REL", L"SA_FORCE_DOWN_REL",
		L"SA_FORCE_DOWN_MELEE_REL", L"SA_FORCE_DOWN_RANGE_REL", L"SA_FORCE_DOWN_ABS", L"SA_MP_REGENERATION_ABS",
		L"SA_MP_INTAKE_ABS", L"SA_TARGET_ANI_SPEED", L"SA_TARGET_MOVE_SPEED", L"SA_TARGET_JUMP_SPEED",
		L"SA_TARGET_ATK_REL", L"SA_HP_INTAKE_REL_DAMAGE", L"SA_ATK_CRITICAL_DAM_ABS", L"SA_ATK_CRITICAL_RATE_ABS",
		L"SA_DEF_CRITICAL_DAM_ABS", L"SA_DEF_CRITICAL_RATE_ABS", L"SA_BACK_SPEED_X", L"SA_ITEM_SPECIAL_ABILITY_REL",
		L"SA_MP_GAIN_GET_HIT_REL", L"SA_SOUL_GAIN_ON_HIT_REL", L"SA_SOUL_GAIN_GET_HIT_REL", L"SA_HP_GAIN_REL_MAX_HP",
		L"SA_ED_GAIN_REL", L"SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL", L"SA_EXP_GAIN_REL", L"SA_DAMAGE_ABSORB_REL_MAX_HP",
		L"SA_STRONG_MIND_ACTIVE_REL", L"SA_STRONG_MIND_SPECIAL_ACTIVE_REL", L"SA_MP_RETAKE_ABS", L"SA_RESIST_FIRE_ABS",
		L"SA_RESIST_WATER_ABS", L"SA_RESIST_GREEN_ABS", L"SA_RESIST_WIND_ABS", L"SA_RESIST_LIGHT_ABS",
		L"SA_RESIST_DARK_ABS", L"SA_ATK_MAGIC_FIRST_REL", L"SA_ATK_MAGIC_SECOND_REL", L"SA_ATK_MAGIC_THIRD_REL",
		L"SA_REMOTE_SPEED_ABS", L"SA_DAMAGE_TYPE_CHANGE", L"SA_DAMAGE_TIME", L"SA_RECOVER_MP_ABS",
		L"SA_TARGET_MANA", L"SA_TARGET_MANA_REL", L"SA_SIZE_ABS", L"SA_REFLECT_REL",
		L"SA_EL_DEFENCE_ABS", L"SA_TARGET_HP_REL", L"SA_TARGET_HITRATE_REL", L"SA_CHARGE_CANNONBALL",
		L"SA_FIRE_DEFENCE_ABS", L"SA_WATER_DEFENCE_ABS", L"SA_NATURE_DEFENCE_ABS", L"SA_WIND_DEFENCE_ABS",
		L"SA_LIGHT_DEFENCE_ABS", L"SA_DARK_DEFENCE_ABS", L"SA_EDT_FIRE_RESIST_REL", L"SA_EDT_FROZEN_RESIST_REL",
		L"SA_EDT_ICE_RESIST_REL", L"SA_EDT_POISON_RESIST_REL", L"SA_EDT_Y_PRESSED_RESIST_REL", L"SA_EDT_LEG_WOUND_RESIST_REL",
		L"SA_EDT_STUN_RESIST_REL", L"SA_EDT_CURSE_RESIST_REL", L"SA_EXPAND_CB", L"SA_ANI_SPEED",
		L"SA_ACCURACY_PERCENT_ABS", L"SA_CRITICAL_EVASION_REL", L"SA_DECREASE_REL", L"SA_ATK_ADD_DAMAGE_FIRST_ABS",
		L"SA_ATK_ADD_DAMAGE_SECOND_ABS", L"SA_ATK_ADD_DAMAGE_THIRD_ABS", L"SA_ATK_ADD_DAMAGE_FOURTH_ABS", L"SA_ATK_ADD_DAMAGE_FIFTH_ABS",
		L"SA_TELEPORT_SPEED_REL", L"HP_CONSUME_REL", L"SA_MP_BURN_ABS", L"SA_SKILLDAMAGE_MULTIPLE_01",
		L"SA_SKILLDAMAGE_MULTIPLE_02", L"SA_SKILLDAMAGE_MULTIPLE_03", L"SA_SKILLDAMAGE_MULTIPLE_04", L"SA_SKILLDAMAGE_MULTIPLE_01_RATE",
		L"SA_SKILLDAMAGE_MULTIPLE_02_RATE", L"SA_SKILLDAMAGE_MULTIPLE_03_RATE", L"SA_SKILLDAMAGE_MULTIPLE_04_RATE", L"SA_EVASION_REL",
		L"SA_REDUCE_EDT_TIME_REL", L"SA_OVERHEAT_HP", L"SA_SCOPE_ABS_HYPER", L"SA_EFFECTIVE_TIME_HYPER",
		L"SA_AIR_MOVE_COUNT", L"SA_NOT_CANNONBALL_REL", L"SA_INSERT_CANNONBALL_REL", L"SA_BE_SHOT_CANNONBALL_REL",
		L"SA_CANNONBALL_DAMAGE_UP_REL", L"SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE", L"SA_SIEGE_SHELLING_REL", L"SA_EFFECTIVE_DAMAGED_ATTACK_BASE",
		L"SA_FIRST_HYPER_EFFECTIVE", L"SA_SECOND_HYPER_EFFECTIVE", L"SA_THIRD_HYPER_EFFECTIVE", L"SA_ATK_CRITICAL_RATE_ATK_BASE",
		L"SA_PROJECTILE_CURVE_SPEED_REL", L"SA_DP_GAIN_REL", L"SA_DP_USE_REL", L"SA_MP_CONSUMPTION",
		L"SA_PROJECTILE_RANGE_REL_2", L"SA_OVERHEAT_HP_PVP", L"SA_DECREASE_REL_2", L"SA_WAY_OF_SWORD_POINT_MULTIPLIER",
		L"SA_COOLTIME_1", L"SA_COOLTIME_2", L"SA_COOLTIME_3", L"SA_MP_GAIN_ON_HIT_REL_2",
		L"SA_MP_GAIN_ON_HIT_REL_3", L"SA_USE_DP_REL_AT_FORMATION_MODE", L"SA_RECOVERY_VALUE_REL", L"SA_ADD_MP_VALUE_ABS",
		L"SA_ADD_DP_VALUE_ABS", L"SA_SCOPE_ABS_PVP", L"SA_SCOPE_ABS_Y", L"SA_SCOPE_ABS_Z",
		L"SA_MP_REGENERATION_ABS_PVP", L"SA_ATK_PHYSIC_REL_PVP", L"SA_ATK_MAGIC_REL_PVP", L"SA_DEF_PHYSIC_REL_PVP",
		L"SA_DEF_MAGIC_REL_PVP", L"SA_TEAM_ATK_PHYSIC_REL_PVP", L"SA_TEAM_ATK_MAGIC_REL_PVP", L"SA_TEAM_DEF_PHYSIC_REL_PVP",
		L"SA_TEAM_DEF_MAGIC_REL_PVP", L"SA_TIME_ABS", L"SA_SOUL_ABS", L"SA_MAX_HP_REL_PVP",
		L"SA_RECOVER_MP_ABS_2", L"SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE", L"SA_MAX_INCREASE_MOVEMENT_BY_MANA", L"SA_MIN_INCREASE_MOVEMENT_BY_MANA",
		L"SA_CHAIN_SKILL_ANI_SPEED_REL", L"SA_HIT_COUNT_1", L"SA_HIT_COUNT_2", L"SA_HIT_COUNT_3",
		L"SA_FORCE_DOWN_GAIN_REL", L"SA_MUTATION_CONSUME_MAX_COUNT", L"SA_DAMAGED_REL_PVP", L"SA_ECP_REL",
		L"SA_SUMMON_ALL_STAT_REL", L"SA_ADD_LEARN_SKILL_LIST_1", L"SA_ADD_LEARN_SKILL_LIST_2", L"SA_ADD_LEARN_SKILL_LIST_3",
		L"SA_FP_CANCEL_SKILL", L"SA_ATK_PHYSIC_INCREASE_BY_ADDITIONALATTACK", L"SA_NOT_EQUIP_WITH_SKILLID_SAME_TIME", L"SA_FP_UP_ABS",
		L"SA_FP_DOWN_ABS", L"SA_EFFECTIVE_VALUE_REL", L"SA_CRITICAL_REL", L"SA_CRITICAL_DAMAGE_REL",
		L"SA_MAXIMIZE_REL", L"SA_DEGREE_X", L"SA_DEGREE_Y", L"SA_DEGREE_Z",
		L"SA_DEGREE_MAX", L"SA_DFG_GAIN_REL", L"SA_DFG_CONSUME_REL",
	};
}

C_ASSERT( CX2SkillTree::SA_DP_USE_REL == 142 );
C_ASSERT( CX2SkillTree::SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE == 173 );
C_ASSERT( CX2SkillTree::SA_DFG_CONSUME_REL == 202 );
C_ASSERT( CX2SkillTree::SA_END == 203 );
C_ASSERT( CX2SkillTree::SPRT_HYPER_CLASS == 5 );

/*static*/ const WCHAR* CX2SkillTree::GetSkillAbilityName( int iType_ )
{
	if( iType_ <= 0 || iType_ >= static_cast<int>( ARRAY_SIZE( s_apwszJuneSkillAbilityName ) ) )
		return NULL;

	return s_apwszJuneSkillAbilityName[ iType_ ];
}

/// June 320716c0: the row for a level is min( level - 1, count - 1 ), compared as unsigned, so level 0 takes the last row.
/// An empty element gives four zeros.
/*static*/ void CX2SkillTree::SkillTemplet::JuneGetNumericalRow( const JuneNumericalData& data_, int iLevel_, JuneNumericalRow& rowOut_ )
{
	rowOut_.m_fValue = 0.f;
	rowOut_.m_fPvpRate = 0.f;
	rowOut_.m_fTechPoint = 0.f;
	rowOut_.m_fHitAddMP = 0.f;

	if( true == data_.m_vecRow.empty() )
		return;

	unsigned int uiIndex = static_cast<unsigned int>( iLevel_ - 1 );
	const unsigned int uiLast = static_cast<unsigned int>( data_.m_vecRow.size() ) - 1;
	if( uiIndex > uiLast )
		uiIndex = uiLast;

	rowOut_ = data_.m_vecRow[ uiIndex ];
}

/// June 3221e930: the first element whose name matches.
const CX2SkillTree::SkillTemplet::JuneNumericalData* CX2SkillTree::SkillTemplet::JuneFindNumericalData( const WCHAR* pwszName_ ) const
{
	if( NULL == pwszName_ )
		return NULL;

	for( size_t i = 0; i < m_vecJuneNumericalData.size(); ++i )
	{
		if( 0 == m_vecJuneNumericalData[i].m_wstrName.compare( pwszName_ ) )
			return &m_vecJuneNumericalData[i];
	}
	return NULL;
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_NUMERICAL
/// June 320717e0 ( this templet, row out, index, level ): the element at the 1-based index; an index of 0 or past the count gives four zeros
/// ( @32071802: count < index unsigned, or index <= 0 ). The element's row is picked by the clamp 320716c0.
void CX2SkillTree::SkillTemplet::JuneGetNumericalRowByIndex( int iIndex_, int iLevel_, JuneNumericalRow& rowOut_ ) const
{
	if( iIndex_ <= 0 || static_cast<unsigned int>( m_vecJuneNumericalData.size() ) < static_cast<unsigned int>( iIndex_ ) )
	{
		rowOut_.m_fValue = 0.f;
		rowOut_.m_fPvpRate = 0.f;
		rowOut_.m_fTechPoint = 0.f;
		rowOut_.m_fHitAddMP = 0.f;
		return;
	}

	JuneGetNumericalRow( m_vecJuneNumericalData[ iIndex_ - 1 ], iLevel_, rowOut_ );
}

/// June 32078060 ( this templet, row out, name, level ): the first element whose name matches ( wstring::compare ), none gives four zeros.
void CX2SkillTree::SkillTemplet::JuneGetNumericalRowByName( const WCHAR* pwszName_, int iLevel_, JuneNumericalRow& rowOut_ ) const
{
	const JuneNumericalData* pData = JuneFindNumericalData( pwszName_ );
	if( NULL == pData )
	{
		rowOut_.m_fValue = 0.f;
		rowOut_.m_fPvpRate = 0.f;
		rowOut_.m_fTechPoint = 0.f;
		rowOut_.m_fHitAddMP = 0.f;
		return;
	}

	JuneGetNumericalRow( *pData, iLevel_, rowOut_ );
}
#endif SERV_IRUHADEV_JUNE_SKILL_NUMERICAL
/// June 32206e50 / 32206fb0, first half: a NUMERICAL_DATA block named after the ability type. Found: value * ( bPvp ? pvp rate : 1.0 ),
/// no x0.01.
bool CX2SkillTree::SkillTemplet::JuneGetNumericalAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_, float& fOut_ ) const
{
	if( true == m_vecJuneNumericalData.empty() )
		return false;

	const JuneNumericalData* pData = JuneFindNumericalData( CX2SkillTree::GetSkillAbilityName( static_cast<int>( eType_ ) ) );
	if( NULL == pData )
		return false;

	JuneNumericalRow row;
	JuneGetNumericalRow( *pData, iLevel_, row );
	fOut_ = row.m_fValue * ( true == bPvp_ ? row.m_fPvpRate : 1.f );
	return true;
}

/// June 32206fb0 ( type, level, &out, bPvp ): the numerical block first, else the +0x164 map. False, fOut_ untouched, when neither has the type
/// or the level is past the map's table ( @322070a6-322070e3; S2 review fix: it returned true with 0 there ).
bool CX2SkillTree::SkillTemplet::JuneTryGetSkillAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_, float& fOut_ ) const
{
	if( true == JuneGetNumericalAbilityValue( eType_, iLevel_, bPvp_, fOut_ ) )
		return true;

	std::map< CX2SkillTree::SKILL_ABILITY_TYPE, vector<float> >::const_iterator it = m_mapSkillAbility.find( eType_ );
	if( m_mapSkillAbility.end() == it )
		return false;

	const int iIndex = iLevel_ - 1;
	if( 0 > iIndex || static_cast<int>( it->second.size() ) <= iIndex )
		return false;

	fOut_ = it->second[iIndex];
	return true;
}

/// June 32206e50 ( type, level, bPvp )
float CX2SkillTree::SkillTemplet::GetSkillAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_ ) const
{
	float fValue = 0.f;
	if( true == JuneTryGetSkillAbilityValue( eType_, iLevel_, bPvp_, fValue ) )
		return fValue;

	return 0.f;
}

/// June 32205c00: the "[Enhanced]" skill that replaces a base skill, or -1.
int CX2SkillTree::GetUpgradedSkillID( int iSkillID_ ) const
{
	std::map<int, int>::const_iterator it = m_mapUpgradeSkill.find( iSkillID_ );
	if( m_mapUpgradeSkill.end() != it )
		return it->second;

	return -1;
}

/// June 32206d00: is this skill itself an upgraded version ( templet +0x178 != 0 )
bool CX2SkillTree::IsUpgradedSkill( SKILL_ID eSkillID_ ) const
{
	SkillTempletMap::const_iterator it = m_mapSkillTemplet.find( eSkillID_ );
	if( m_mapSkillTemplet.end() == it || NULL == it->second )
		return false;

	return 0 != it->second->m_iDeleteSkillIDByUpgrade;
}

/// June 32207210: the base skill an upgraded skill replaces ( templet +0x178 ), else the id itself
CX2SkillTree::SKILL_ID CX2SkillTree::GetUpgradeBaseSkillID( SKILL_ID eSkillID_ ) const
{
	SkillTempletMap::const_iterator it = m_mapSkillTemplet.find( eSkillID_ );
	if( m_mapSkillTemplet.end() != it && NULL != it->second && 0 != it->second->m_iDeleteSkillIDByUpgrade )
		return static_cast<SKILL_ID>( it->second->m_iDeleteSkillIDByUpgrade );

	return eSkillID_;
}

/// June 3220b740: one SA_* table. A table with no number inserts nothing ( March inserts { 0.f } ).
void CX2SkillTree::JuneSetSkillAbilityMap( KLuaManager* pLuaManager_, SKILL_ABILITY_TYPE eSkillAbility_, SkillTemplet* pSkillTemplet_, const char* pszKey_ )
{
	vector<float>   vecSkillValue;
	int             iTableIndex = 1;
	float           fValue = 0.f;

	while( pLuaManager_->GetValue( iTableIndex, fValue ) == true )
	{
		vecSkillValue.push_back( fValue );
		++iTableIndex;
	}

	pLuaManager_->EndTable();

	m_JuneLoadStats.m_setKeysSeen.insert( pszKey_ );

	if( true == vecSkillValue.empty() )
	{
		++m_JuneLoadStats.m_iEmptySASkipped;
		return;
	}

	pSkillTemplet_->m_mapSkillAbility.insert( std::make_pair( eSkillAbility_, vecSkillValue ) );
}

/// June 32216320, whole. Differences from March's AddSkillTemplet_LUA ( s0a_templet_keys.tsv ): no padding of m_SkillCoolTime /
/// m_MPConsumption / m_RequireCharacterLevel to 30; an empty SA_* table inserts nothing; IN_HOUSE_TEST_ONLY drops the templet;
/// a duplicate id is not added and the function still returns true; new fields m_ExtraIconInfo<n>, m_DeleteSkillIDByUpgrade
/// ( also fills the upgrade map ), NUMERICAL_DATA<n>, and SA ids 143-202. 31c59710 ( anti-tamper registration ) is not ported.
bool CX2SkillTree::AddSkillTemplet_JuneLUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	bool bInHouseTestOnly = false;
	LUA_GET_VALUE( luaManager,      "IN_HOUSE_TEST_ONLY",           bInHouseTestOnly,   false );
	if( true == bInHouseTestOnly )
	{
		++m_JuneLoadStats.m_iInHouseDropped;
		return false;
	}

	SkillTemplet* pSkillTemplet = new SkillTemplet();

	LUA_GET_VALUE_ENUM( luaManager,     "m_eType",                      pSkillTemplet->m_eType,                         SKILL_TYPE,         ST_NONE                 );

	switch ( pSkillTemplet->m_eType )
	{
	case ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
	case ST_GUILD_PASSIVE:
		{
			pSkillTemplet->m_bGuildSkill = true;
		} break;
	}

	LUA_GET_VALUE_ENUM( luaManager,     "m_eID",                        pSkillTemplet->m_eID,                           SKILL_ID,           SI_NONE                 );

	/// a duplicate id: the insert fails, the templet is not added, the function returns true
	if( false == m_mapSkillTemplet.insert( std::make_pair( pSkillTemplet->m_eID, pSkillTemplet ) ).second )
	{
		++m_JuneLoadStats.m_iDuplicate;
		SAFE_DELETE( pSkillTemplet );
		return true;
	}

	/// fill flag 0: no padding to 30 ( March pads these three )
	if( true == luaManager.BeginTable( "m_SkillCoolTime" ) )            SetSkillVlaue( luaManager, pSkillTemplet->m_vecSkillCoolTime, false );
	if( true == luaManager.BeginTable( "m_MPConsumption" ) )            SetSkillVlaue( luaManager, pSkillTemplet->m_vecMPConsumption, false );
	if( true == luaManager.BeginTable( "m_RequireCharacterLevel" ) )    SetSkillVlaue( luaManager, pSkillTemplet->m_vecRequireCharacterLevel, false );

	{
		/// the padding March would have done: arrays of 1-29 entries
		const int iId = static_cast<int>( pSkillTemplet->m_eID );
		const size_t nCool = pSkillTemplet->m_vecSkillCoolTime.size();
		const size_t nMp = pSkillTemplet->m_vecMPConsumption.size();
		const size_t nReq = pSkillTemplet->m_vecRequireCharacterLevel.size();
		if( nCool >= 1 && nCool < static_cast<size_t>( MAX_LIMITED_SKILL_LEVEL ) )
		{
			++m_JuneLoadStats.m_iShortCoolTime;
			JuneLogShortArray( iId, 0, static_cast<int>( nCool ) );
		}
		if( nMp >= 1 && nMp < static_cast<size_t>( MAX_LIMITED_SKILL_LEVEL ) )
		{
			++m_JuneLoadStats.m_iShortMP;
			JuneLogShortArray( iId, 1, static_cast<int>( nMp ) );
		}
		if( nReq >= 1 && nReq < static_cast<size_t>( MAX_LIMITED_SKILL_LEVEL ) )
		{
			++m_JuneLoadStats.m_iShortReqLevel;
			JuneLogShortArray( iId, 2, static_cast<int>( nReq ) );
		}
	}

#ifdef NEW_CHARACTER_CHUNG
	LUA_GET_VALUE( luaManager,          "m_usCBConsumption",            pSkillTemplet->m_usCBConsumption,               0       );
#endif NEW_CHARACTER_CHUNG

#ifdef ARA_CHARACTER_BASE
	LUA_GET_VALUE( luaManager,          "m_usFPGainCount",              pSkillTemplet->m_usFPGainCount,                 0       );
	LUA_GET_VALUE( luaManager,          "m_usFPConsumtion",             pSkillTemplet->m_usFPConsumtion,                0       );
#endif

	LUA_GET_VALUE( luaManager,          "m_wstrName",                   pSkillTemplet->m_wstrName,                      L""     );
	LUA_GET_VALUE( luaManager,          "m_wstrMainDesc",               pSkillTemplet->m_wstrMainDesc,                  L""     );

	if( true == luaManager.BeginTable( "m_EffectiveDesc" ) )            SetSkillVlaue( luaManager, pSkillTemplet->m_vecEffectiveDesc, false );

	LUA_GET_VALUE( luaManager,          "m_bBornSealed",                pSkillTemplet->m_bBornSealed,                   false   );

	LUA_GET_VALUE( luaManager,          "m_wstrIconTextureName",        pSkillTemplet->m_wstrIconName,                  L""     );
	LUA_GET_VALUE( luaManager,          "m_wstrIconTexturePieceName",   pSkillTemplet->m_wstrIconPieceName,             L""     );

	/// m_ExtraIconInfo1, 2, ... while BeginTable succeeds ( new in June, stored for the tree UI )
	for( int iExtra = 1; ; ++iExtra )
	{
		char szExtraKey[ 32 ];
		StringCchPrintfA( szExtraKey, ARRAY_SIZE( szExtraKey ), "m_ExtraIconInfo%d", iExtra );
		if( false == luaManager.BeginTable( szExtraKey ) )
			break;

		std::pair<wstring, wstring> pairExtraIcon;
		LUA_GET_VALUE( luaManager,      "m_wstrTextureName",            pairExtraIcon.first,                            L""     );
		LUA_GET_VALUE( luaManager,      "m_wstrTexturePieceName",       pairExtraIcon.second,                           L""     );
		luaManager.EndTable();

		pSkillTemplet->m_vecExtraIconInfo.push_back( pairExtraIcon );
		++m_JuneLoadStats.m_iExtraIcon;
	}

	LUA_GET_VALUE( luaManager,          "m_wstrImageTextureName",       pSkillTemplet->m_wstrImageName,                 L""     );
	LUA_GET_VALUE( luaManager,          "m_wstrImageTexturePieceName",  pSkillTemplet->m_wstrImagePieceName,            L""     );

	SKILL_POWER_RATE_TYPE ePowerRate = SPRT_BASIC_CLASS;
	LUA_GET_VALUE_ENUM( luaManager,     "m_ePowerRate",                 ePowerRate,                                     SKILL_POWER_RATE_TYPE,          SPRT_BASIC_CLASS    );
	SetSkillPowerRate( ePowerRate, pSkillTemplet->m_vecPowerRate );     /// 32207430: copied only when the type's row has exactly 30 entries
#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
	pSkillTemplet->m_eSkillPowerRateType = ePowerRate;
#endif SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
	pSkillTemplet->m_eJunePowerRateType = ePowerRate;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

	LUA_GET_VALUE_ENUM( luaManager,     "m_eActiveSkillUseCondtion",    pSkillTemplet->m_eActiveSkillUseCondtion,       ACTIVE_SKILL_USE_CONDITION,     ASUT_NONE   );

	LUA_GET_VALUE( luaManager,          "m_bShowSkillLevel",            pSkillTemplet->m_bShowSkillLevel,               false   );
	LUA_GET_VALUE( luaManager,          "m_iRequireLearnSkillPoint",    pSkillTemplet->m_iRequireLearnSkillPoint,       0       );
	LUA_GET_VALUE( luaManager,          "m_iRequireUpgradeSkillPoint",  pSkillTemplet->m_iRequireUpgradeSkillPoint,     0       );
	LUA_GET_VALUE( luaManager,          "m_usWOSType",                  static_cast<int>( pSkillTemplet->m_iWOSType ),  0       );

	/// the skill this one replaces once learned ( June templet +0x178 ); non-zero also fills the upgrade map ( base id -> this id )
	LUA_GET_VALUE( luaManager,          "m_DeleteSkillIDByUpgrade",     pSkillTemplet->m_iDeleteSkillIDByUpgrade,       0       );
	if( 0 != pSkillTemplet->m_iDeleteSkillIDByUpgrade )
		m_mapUpgradeSkill.insert( std::make_pair( pSkillTemplet->m_iDeleteSkillIDByUpgrade, static_cast<int>( pSkillTemplet->m_eID ) ) );

	/// NUMERICAL_DATA1, 2, ... while BeginTable succeeds. Rows are 16 B { value, pvp rate, tech point, hit add mp }; each side array is
	/// clamped to its last entry, m_Pvp_Rate defaults to 1.0 and the other two to 0.
	for( int iNumerical = 1; ; ++iNumerical )
	{
		char szNumericalKey[ 32 ];
		StringCchPrintfA( szNumericalKey, ARRAY_SIZE( szNumericalKey ), "NUMERICAL_DATA%d", iNumerical );
		if( false == luaManager.BeginTable( szNumericalKey ) )
			break;

		SkillTemplet::JuneNumericalData numericalData;
		LUA_GET_VALUE( luaManager,      "m_wstrName",                   numericalData.m_wstrName,                       L""     );
		LUA_GET_VALUE( luaManager,      "m_ParamType",                  numericalData.m_wstrParamType,                  L"i"    );

		vector<float> vecValue;
		vector<float> vecPvpRate;
		vector<float> vecTechPoint;
		vector<float> vecHitAddMP;
		if( true == luaManager.BeginTable( "m_Value" ) )                SetSkillVlaue( luaManager, vecValue, false );
		if( true == luaManager.BeginTable( "m_Pvp_Rate" ) )             SetSkillVlaue( luaManager, vecPvpRate, false );
		if( true == luaManager.BeginTable( "m_Tech_Point" ) )           SetSkillVlaue( luaManager, vecTechPoint, false );
		if( true == luaManager.BeginTable( "m_Hit_Add_MP" ) )           SetSkillVlaue( luaManager, vecHitAddMP, false );
		luaManager.EndTable();

		for( size_t iRow = 0; iRow < vecValue.size(); ++iRow )
		{
			SkillTemplet::JuneNumericalRow row;
			row.m_fValue = vecValue[ iRow ];
			row.m_fPvpRate = true == vecPvpRate.empty() ? 1.f : vecPvpRate[ iRow < vecPvpRate.size() ? iRow : vecPvpRate.size() - 1 ];
			row.m_fTechPoint = true == vecTechPoint.empty() ? 0.f : vecTechPoint[ iRow < vecTechPoint.size() ? iRow : vecTechPoint.size() - 1 ];
			row.m_fHitAddMP = true == vecHitAddMP.empty() ? 0.f : vecHitAddMP[ iRow < vecHitAddMP.size() ? iRow : vecHitAddMP.size() - 1 ];
			numericalData.m_vecRow.push_back( row );
		}

		pSkillTemplet->m_vecJuneNumericalData.push_back( numericalData );
		++m_JuneLoadStats.m_iNumericalBlock;
	}

	SetSkillStat( luaManager, pSkillTemplet->m_vecStat );

	for( int iKey = 0; iKey < static_cast<int>( ARRAY_SIZE( s_aJuneSkillAbilityKey ) ); ++iKey )
	{
		if( true == luaManager.BeginTable( s_aJuneSkillAbilityKey[ iKey ].m_pszKey ) )
			JuneSetSkillAbilityMap( &luaManager, static_cast<SKILL_ABILITY_TYPE>( s_aJuneSkillAbilityKey[ iKey ].m_iType ), pSkillTemplet, s_aJuneSkillAbilityKey[ iKey ].m_pszKey );
	}

	if( false == m_bJuneScratch )
		CX2BuffTempletManager::GetInstance()->GetBuffFactorPtrFromBuffFactorList( luaManager, pSkillTemplet->m_vecBuffFactorPtr, pSkillTemplet->m_vecBuffFactorID );

	return true;
}

/// the padding difference, logged for the first 25 skills of each array and counted for the summary
void CX2SkillTree::JuneLogShortArray( int iSkillID_, int iKind_, int iSize_ )
{
}

/// debug: a runtime read past the end of a short array ( level above the raw size ), once per skill and array
/*static*/ void CX2SkillTree::JuneLogShortArrayReach( int iSkillID_, int iKind_, int iLevel_, int iSize_ )
{
}

void CX2SkillTree::JuneLogLoadSummary()
{
	if( true == m_bJuneScratch )
		return;

	int iTreeRows = 0;
	int iClasses = 0;
	for( UnitSkillTreeTempletMap::const_iterator itClass = m_mapUnitSkillTreeTemplet.begin(); itClass != m_mapUnitSkillTreeTemplet.end(); ++itClass )
	{
		iTreeRows += static_cast<int>( itClass->second.size() );
		++iClasses;
	}

	int iGuildRows = 0;
	for( GuildSkillTreeTempletMap::const_iterator itGuild = m_mapGuildSkillTreeTemplet.begin(); itGuild != m_mapGuildSkillTreeTemplet.end(); ++itGuild )
		iGuildRows += static_cast<int>( itGuild->second.size() );

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_SKILL] LOAD templets=%d tree_rows=%d ( classes=%d ) guild_rows=%d seal_items=%d upgrade_map=%d numerical_blocks=%d extra_icons=%d sa_keys_seen=%d",
		static_cast<int>( m_mapSkillTemplet.size() ), iTreeRows, iClasses, iGuildRows, static_cast<int>( m_mapSealSkillItemTemplet.size() ),
		static_cast<int>( m_mapUpgradeSkill.size() ), m_JuneLoadStats.m_iNumericalBlock, m_JuneLoadStats.m_iExtraIcon,
		static_cast<int>( m_JuneLoadStats.m_setKeysSeen.size() ) );
	CX2OfflineLog::Server( L"[JUNE_SKILL] LOAD rules: empty_sa_skipped=%d duplicate_ids=%d inhouse_dropped=%d short_arrays cool_time/mp/req_level=%d/%d/%d",
		m_JuneLoadStats.m_iEmptySASkipped, m_JuneLoadStats.m_iDuplicate, m_JuneLoadStats.m_iInHouseDropped,
		m_JuneLoadStats.m_iShortCoolTime, m_JuneLoadStats.m_iShortMP, m_JuneLoadStats.m_iShortReqLevel );
#ifdef SERV_IRUHADEV_JUNE_SKILL_TYPE
	/// S3c census: templets per SKILL_TYPE 0-18 ( June Enum.lua ); anything outside counts as out
	{
		int aiType[ ST_CORE_PASSIVE + 1 ] = { 0, };
		int iOut = 0;
		for( SkillTempletMap::const_iterator itType = m_mapSkillTemplet.begin(); itType != m_mapSkillTemplet.end(); ++itType )
		{
			const int iType = ( NULL != itType->second ) ? static_cast<int>( itType->second->m_eType ) : -1;
			if( 0 <= iType && iType <= ST_CORE_PASSIVE )
				++aiType[ iType ];
			else
				++iOut;
		}
		CX2OfflineLog::Server( L"[JUNE_SKILL] TYPE templets per type 0-18: %d %d %d %d %d | %d %d %d | %d | %d %d %d | %d %d | %d %d %d | %d %d out=%d",
			aiType[ 0 ], aiType[ 1 ], aiType[ 2 ], aiType[ 3 ], aiType[ 4 ], aiType[ 5 ], aiType[ 6 ], aiType[ 7 ], aiType[ 8 ],
			aiType[ 9 ], aiType[ 10 ], aiType[ 11 ], aiType[ 12 ], aiType[ 13 ], aiType[ 14 ], aiType[ 15 ], aiType[ 16 ],
			aiType[ 17 ], aiType[ 18 ], iOut );
	}
#endif SERV_IRUHADEV_JUNE_SKILL_TYPE
#endif SERV_IRUHADEV_OFFLINE
}

//////////////////////////////////////////////////////////////////////////
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
