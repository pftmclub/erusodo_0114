#include "StdAfx.h"
#include ".\x2damageeffect.h"
#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG2

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE
// JUNE_FX <unsupported|deferred> <KEY> in <templet>: <why>. The rule-5 line for a June per-frame
// branch this build cannot take (plan Phase 7a), once per (tag, key, templet) for the session.
// Code review fix: no-op when SERV_IRUHADEV_OFFLINE is off -- CX2OfflineLog only exists under it
// (X2OfflineLog.h), so this keeps SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME independently buildable
// and revertible, rather than tying it to offline mode by accident.
static void JuneFxCeOnce( const WCHAR* pwszTag_, const WCHAR* pwszEffect_, const WCHAR* pwszKey_, const WCHAR* pwszWhy_ )
{
#ifdef SERV_IRUHADEV_OFFLINE
	const WCHAR* pwszEffectName = ( NULL != pwszEffect_ ? pwszEffect_ : L"-" );
	std::wstring wstrOnceKey = pwszTag_;
	wstrOnceKey += L"|";
	wstrOnceKey += pwszKey_;
	wstrOnceKey += L"|";
	wstrOnceKey += pwszEffectName;

	static std::set<std::wstring> s_setLogged;
	if( false == s_setLogged.insert( wstrOnceKey ).second )
		return;

	CX2OfflineLog::Server( L"JUNE_FX %s %s in %s: %s", pwszTag_, pwszKey_, pwszEffectName, pwszWhy_ );
#else SERV_IRUHADEV_OFFLINE
	UNREFERENCED_PARAMETER( pwszTag_ );
	UNREFERENCED_PARAMETER( pwszEffect_ );
	UNREFERENCED_PARAMETER( pwszKey_ );
	UNREFERENCED_PARAMETER( pwszWhy_ );
#endif SERV_IRUHADEV_OFFLINE
}

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
// JUNE_FX CE_FRAME <effect> <key> <detail>: one line per June per-frame branch taken in
// CEffect::OnFrameMove / FollowLine (plan Phase 7a), at most one a second per (effect, key).
// The detail (times, positions) is not part of the throttle key. Runtime, so it goes to
// offline_server.log. No-op when SERV_IRUHADEV_OFFLINE is off, same reason as JuneFxCeOnce above.
static void JuneFxCeFrame( const WCHAR* pwszEffect_, const WCHAR* pwszKey_, const WCHAR* pwszDetail_ )
{
#ifdef SERV_IRUHADEV_OFFLINE
	const WCHAR* pwszEffectName = ( NULL != pwszEffect_ ? pwszEffect_ : L"-" );
	std::wstring wstrThrottleKey = pwszEffectName;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszKey_;

	static std::map<std::wstring, DWORD> s_mapLastLoggedTime;
	const DWORD dwNow = timeGetTime();
	std::map<std::wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrThrottleKey );
	if( itThrottle != s_mapLastLoggedTime.end() && dwNow - itThrottle->second < 1000 )
		return;
	s_mapLastLoggedTime[ wstrThrottleKey ] = dwNow;

	CX2OfflineLog::Server( L"JUNE_FX CE_FRAME %s %s %s", pwszEffectName, pwszKey_, pwszDetail_ );
#else SERV_IRUHADEV_OFFLINE
	UNREFERENCED_PARAMETER( pwszEffect_ );
	UNREFERENCED_PARAMETER( pwszKey_ );
	UNREFERENCED_PARAMETER( pwszDetail_ );
#endif SERV_IRUHADEV_OFFLINE
}

// JUNE_FX CE_LOCKON <effect> <branch> <detail>: one line per June lock-on branch taken (plan
// Phase 7b), at most one a second per (effect, branch); the detail is printf-formatted and not
// part of the throttle key. No-op when SERV_IRUHADEV_OFFLINE is off, as JuneFxCeFrame above.
static void JuneFxLockOn( const WCHAR* pwszEffect_, const WCHAR* pwszBranch_, const WCHAR* pwszFormat_, ... )
{
#ifdef SERV_IRUHADEV_OFFLINE
	const WCHAR* pwszEffectName = ( NULL != pwszEffect_ ? pwszEffect_ : L"-" );
	std::wstring wstrThrottleKey = pwszEffectName;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszBranch_;

	static std::map<std::wstring, DWORD> s_mapLastLoggedTime;
	const DWORD dwNow = timeGetTime();
	std::map<std::wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrThrottleKey );
	if( itThrottle != s_mapLastLoggedTime.end() && dwNow - itThrottle->second < 1000 )
		return;
	s_mapLastLoggedTime[ wstrThrottleKey ] = dwNow;

	WCHAR wszDetail[256] = L"";
	va_list args;
	va_start( args, pwszFormat_ );
	StringCchVPrintfW( wszDetail, ARRAY_SIZE(wszDetail), pwszFormat_, args );
	va_end( args );
	CX2OfflineLog::Server( L"JUNE_FX CE_LOCKON %s %s %s", pwszEffectName, pwszBranch_, wszDetail );
#else SERV_IRUHADEV_OFFLINE
	UNREFERENCED_PARAMETER( pwszEffect_ );
	UNREFERENCED_PARAMETER( pwszBranch_ );
	UNREFERENCED_PARAMETER( pwszFormat_ );
#endif SERV_IRUHADEV_OFFLINE
}

// JUNE_FX CE_HIT <effect> <branch> <detail>: one line per June hit-response branch taken (plan
// Phase 7d: AttackResult, the hit / die / CREATE_DAMAGE_EFFECT spawners, SetDamageDataNext), at
// most one a second per (effect, branch); the detail is printf-formatted and not part of the
// throttle key. No-op when SERV_IRUHADEV_OFFLINE is off, as JuneFxCeFrame above.
static void JuneFxHit( const WCHAR* pwszEffect_, const WCHAR* pwszBranch_, const WCHAR* pwszFormat_, ... )
{
#ifdef SERV_IRUHADEV_OFFLINE
	const WCHAR* pwszEffectName = ( NULL != pwszEffect_ ? pwszEffect_ : L"-" );
	std::wstring wstrThrottleKey = pwszEffectName;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszBranch_;

	static std::map<std::wstring, DWORD> s_mapLastLoggedTime;
	const DWORD dwNow = timeGetTime();
	std::map<std::wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrThrottleKey );
	if( itThrottle != s_mapLastLoggedTime.end() && dwNow - itThrottle->second < 1000 )
		return;
	s_mapLastLoggedTime[ wstrThrottleKey ] = dwNow;

	WCHAR wszDetail[256] = L"";
	va_list args;
	va_start( args, pwszFormat_ );
	StringCchVPrintfW( wszDetail, ARRAY_SIZE(wszDetail), pwszFormat_, args );
	va_end( args );
	CX2OfflineLog::Server( L"JUNE_FX CE_HIT %s %s %s", pwszEffectName, pwszBranch_, wszDetail );
#else SERV_IRUHADEV_OFFLINE
	UNREFERENCED_PARAMETER( pwszEffect_ );
	UNREFERENCED_PARAMETER( pwszBranch_ );
	UNREFERENCED_PARAMETER( pwszFormat_ );
#endif SERV_IRUHADEV_OFFLINE
}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
// June2017 FUN_31f8c280 = LockOnData::operator= (effect completion C7, funcmap row 31f8c280): every member is copied except the
// pre-lock-on hit set (June LockOnData +0x40, March m_setPreLockOnTarget), which the destination keeps. March's implicit operator=
// copies the set too since Phase 7b added it, so June's two callers (the die spawner's INHERITLOCKONDATA @31f83126 and the
// CREATE_DAMAGE_EFFECT loop @31f89e2b) use this instead.
static void JuneAssignLockOnData( CX2DamageEffect::LockOnData& kDest_, const CX2DamageEffect::LockOnData& kSource_ )
{
	std::set<UidType> setKeptPreLockOnTarget;
	setKeptPreLockOnTarget.swap( kDest_.m_setPreLockOnTarget );
	kDest_ = kSource_;
	kDest_.m_setPreLockOnTarget.swap( setKeptPreLockOnTarget );
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
CX2DamageEffect::CX2DamageEffect(void)
//{{ robobeg : 2008-10-28
: m_LuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true )
//}} robobeg : 2008-10-28
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
, m_coInstanceHandleList( LIST_NUM, 128 )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    m_bInCriticalLoop = false;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
	m_fElapsedTime		= 0.0f;
	//m_iCurrUniqueIndex	 = 0;

#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_InstanceList.reserve( 64 );
	m_ReserveInstanceList.reserve( 64 );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

}

CX2DamageEffect::~CX2DamageEffect(void)
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

    KInstanceHandleList::iterator iterNext;
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_LIVE);
        iter != iterEnd;
        iter = iterNext )
    {
        iterNext = iter; ++iterNext;
        CEffect* pCEffect =iter->m_pInstance;
        iter->m_pInstance = NULL;
        iter->m_eListType = LIST_FREE;
        SAFE_DELETE( pCEffect );
    }
    iterEnd = m_coInstanceHandleList.end(LIST_RESERVE);
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_RESERVE);
        iter != iterEnd;
        iter = iterNext )
    {
        iterNext = iter; ++iterNext;
        CEffect* pCEffect =iter->m_pInstance;
        iter->m_pInstance = NULL;
        iter->m_eListType = LIST_FREE;
        SAFE_DELETE( pCEffect )
    }
    m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin(LIST_FREE), LIST_LIVE );
    m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin(LIST_FREE), LIST_RESERVE );

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		SAFE_DELETE( pCEffect );
	}
	m_InstanceList.clear();

	for( int i = 0; i < (int)m_ReserveInstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_ReserveInstanceList[i];
		SAFE_DELETE( pCEffect );
	}
	m_ReserveInstanceList.clear();
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}

void CX2DamageEffect::OpenScriptFile( const WCHAR* pFilename )
{

    if ( g_pKTDXApp->LoadAndDoMemory( &m_LuaManager, pFilename ) == false )
    {
        return;
    }
}

HRESULT CX2DamageEffect::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	m_fElapsedTime = fElapsedTime;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    if ( m_coInstanceHandleList.empty( LIST_RESERVE ) == false )
    {
        KInstanceHandleList::iterator iterNext;
        KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_RESERVE );
        for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_RESERVE );
            iter != iterEnd;
            iter = iterNext )
        {
            iterNext = iter; ++iterNext;
            CEffect* pCEffect = iter->m_pInstance;
            if ( pCEffect != NULL && true == pCEffect->IsCreationDelayElapsed() == true )
            {
                iter->m_eListType = LIST_LIVE;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.end( LIST_LIVE ), iter );
            }
        }
    }
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	for( int i = 0; i < (int)m_ReserveInstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_ReserveInstanceList[i];
		if( NULL != pCEffect && true == pCEffect->IsCreationDelayElapsed() )
		{
			m_InstanceList.push_back( pCEffect );
			m_ReserveInstanceList.erase( m_ReserveInstanceList.begin() + i );
			i--;
		}
	}
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    ASSERT( m_bInCriticalLoop == false );
    m_bInCriticalLoop = true;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
            iter != iterEnd;
            ++iter )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        CEffect* pCEffect = iter->m_pInstance;
        if ( pCEffect == NULL
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            || pCEffect->GetLive() == false
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            )
            continue;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CEffect* pCEffect = m_InstanceList[i];
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
			pCEffect->OnFrameMove( fTime, fElapsedTime );
#else	DAMAGE_EFFECT_WORK
		if( m_LuaManager.BeginTable( pCEffect->GetName() ) == true )
		{
			pCEffect->OnFrameMove( fTime, fElapsedTime );
			m_LuaManager.EndTable();
		}
#endif	DAMAGE_EFFECT_WORK		
		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
	}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
            iter != iterEnd;
            iter = iterNext )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterNext = iter; ++iterNext;
        CEffect* pCEffect = iter->m_pInstance;
        if ( pCEffect == NULL )
        {
            iter->m_eListType = LIST_FREE;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            continue;        
        }
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CEffect* pCEffect = m_InstanceList[i];
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		if( pCEffect->GetLive() == false )
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iter->m_pInstance = NULL;
            iter->m_eListType = LIST_FREE;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            SAFE_DELETE( pCEffect );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			SAFE_DELETE( pCEffect );
			m_InstanceList.erase( m_InstanceList.begin() + i );
			i--;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		}
	}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    m_bInCriticalLoop = false;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX

	return S_OK;
}



CX2DamageEffect::CEffect* CX2DamageEffect::CreateInstanceParabolic_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 vPos, 
																	   D3DXVECTOR3 vTargetPos, D3DXVECTOR3 vAcceleration, float fTimeToReachTarget, float fTimeToLive )
{
	// 이 함수는 몬스터만 사용해야 합니다. power rate 에 기존 코드와의 호환성 유지를 위해 1이 더해지기 때문입니다.
	if( NULL != pGameUnit )
	{
		ASSERT( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() );
	}


	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );


	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 1.f + 1.f;	
	if( NULL != pGameUnit )
	{
		fPowerRate = pGameUnit->GetPowerRate() + 1.f;
	}

	return CreateInstanceParabolic( pGameUnit, wstrName.c_str(), fPowerRate, vPos, vTargetPos, vAcceleration, fTimeToReachTarget, fTimeToLive );
}



CX2DamageEffect::CEffect*  CX2DamageEffect::CreateInstance_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos )
{

	// 이 함수는 몬스터만 사용해야 합니다. power rate 에 기존 코드와의 호환성 유지를 위해 1이 더해지기 때문입니다.
	if( NULL != pGameUnit )
	{
		ASSERT( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() );
	}


	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );


	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 1.f + 1.f;	
	if( NULL != pGameUnit )
	{
		fPowerRate = pGameUnit->GetPowerRate() + 1.f;
	}
	
	return CreateInstance( 
		pGameUnit, wstrName.c_str(), fPowerRate, pos,
		pGameUnit->GetRotateDegree(),
		pGameUnit->GetRotateDegree(), fLandPos, false );
}

#ifdef CREATEINSTANCE_WITH_LIFETIME_IN_LUA
CX2DamageEffect::CEffect*  CX2DamageEffect::CreateInstanceWithLifeTime_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, float fLifeTimeRate  )
{
	// 이 함수는 몬스터만 사용해야 합니다. power rate 에 기존 코드와의 호환성 유지를 위해 1이 더해지기 때문입니다.
	if( NULL != pGameUnit )
	{
		ASSERT( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() );
	}

	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );

	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 1.f + 1.f;	
	if( NULL != pGameUnit )
	{
		fPowerRate = pGameUnit->GetPowerRate() + 1.f;
	}

	return CreateInstance( 
		pGameUnit, wstrName.c_str(), fPowerRate, pos,
		pGameUnit->GetRotateDegree(),
		pGameUnit->GetRotateDegree(), fLandPos, false, -1.f, 1.f, fLifeTimeRate  );
}
#endif //CREATEINSTANCE_WITH_LIFETIME_IN_LUA

CX2DamageEffect::CEffect*  CX2DamageEffect::CreateInstance_LUA2( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot )
{

	// 이 함수는 몬스터만 사용해야 합니다. power rate 에 기존 코드와의 호환성 유지를 위해 1이 더해지기 때문입니다.
	if( NULL != pGameUnit )
	{
		ASSERT( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() );
	}


	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );

	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 1.f + 1.f;	
	if( NULL != pGameUnit )
	{
		fPowerRate = pGameUnit->GetPowerRate() + 1.f;
	}
	

	return CreateInstance( 
		pGameUnit, wstrName.c_str(), fPowerRate, pos,
		vRot,
		vRot, fLandPos, false );
}



CX2DamageEffect::CEffect*  CX2DamageEffect::CreateInstance_LUA3 (CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot, float fReserveTime )
{

	// 이 함수는 몬스터만 사용해야 합니다. power rate 에 기존 코드와의 호환성 유지를 위해 1이 더해지기 때문입니다.
	if( NULL != pGameUnit )
	{
		ASSERT( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() );
	}


	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );

	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 1.f + 1.f;	
	if( NULL != pGameUnit )
	{
		fPowerRate = pGameUnit->GetPowerRate() + 1.f;
	}


	return CreateInstance( 
		pGameUnit, wstrName.c_str(), fPowerRate, pos,
		vRot,
		vRot, fLandPos, true, fReserveTime  );
}

#ifdef SERV_PET_SYSTEM
CX2DamageEffect::CEffect*  CX2DamageEffect::CreateInstanceByPet_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot )
{
	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );

	// note!! 기존에 powerrate = 2.f를 기준으로 모든 몬스터의 projectile 공격력이 정해져 있어서 그대로 간다.
	float fPowerRate = 0.f;
	fPowerRate = 1.f;	
	if( pGameUnit->GetRemainHyperModeTime() > 0.f )
		fPowerRate = 1.2f;

	float fPowerRatePet = 1.f;
	// 펫 친밀도가 70%이상일 경우 배율 증가
	if( pGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser *pMaster = (CX2GUUser *)pGameUnit;		
		CX2PET *pPet = g_pData->GetPetManager()->GetPet( pMaster->GetUnitUID() );
		
		if( pPet != NULL && pPet->GetNowIntimacy() >= 0.7f )
			fPowerRatePet = ( pPet->GetNowIntimacy() - 0.6f + 1.f );
	}

	return CreateInstance( 
		pGameUnit, wstrName.c_str(), fPowerRate * fPowerRatePet, pos,
		vRot,
		vRot, fLandPos, false 
#ifdef PET_SKILL_EFFECT_LINE_MAP
		, -1.f, -1.f, 1.f, -1, -1.f, 0, -1.f, false, true
#endif //PET_SKILL_EFFECT_LINE_MAP		
		);
}
#endif

CKTDGXMeshPlayer::CXMeshInstance* CX2DamageEffect::CEffect::GetMainEffect()
{
	if( NULL == g_pX2Game ||
		NULL == g_pX2Game->GetMajorXMeshPlayer() )
		return NULL;

	if( INVALID_MESH_INSTANCE_HANDLE == m_hMeshInstHandle )
		return NULL;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	return pMeshInst;
}

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
// June2017 FUN_31f75570, CEffect::SetShowObject( bool ): the main effect (GetMainEffect, 31f6eb30), then
// every particle of three consecutive CEffect lists through the game's major particle system. June's
// lists sit at +0x864 / +0x874 / +0x884, 0x10 apart, as March's m_StartParticleDataList /
// m_PassiveParticleDataList / m_TimeParticleDataList do (offsetof probe: 0x614 / 0x624 / 0x634), and
// June reads ParticleData's handle at +0x94 (March m_hSeq, probe 0x90). The three lists are treated
// alike, so their order does not matter. Each show is CKTDGObject::SetShowObject's non-forced form,
// which June inlines.
void CX2DamageEffect::CEffect::SetShowObject( bool bShow_ )
{
	CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = GetMainEffect();
	if( NULL != pMainEffect )
		pMainEffect->SetShowObject( bShow_ );

	if( NULL == g_pX2Game || NULL == g_pX2Game->GetMajorParticle() )
		return;

	vector<ParticleData*>* apParticleDataList[] = { &m_StartParticleDataList, &m_PassiveParticleDataList, &m_TimeParticleDataList };
	for( int iList = 0; iList < ARRAY_SIZE( apParticleDataList ); ++iList )
	{
		for( UINT iParticle = 0; iParticle < apParticleDataList[iList]->size(); ++iParticle )
		{
			ParticleData* pParticleData = (*apParticleDataList[iList])[iParticle];
			if( NULL == pParticleData )
				continue;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pParticleData->m_hSeq );
			if( NULL != pSeq )
				pSeq->SetShowObject( bShow_ );
		}
	}
}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME


void CX2DamageEffect::DestroyAllInstances()
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    if ( IsInCriticalLoop() == true )
    {
        KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
        for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_LIVE);
            iter != iterEnd;
            ++iter )
        {
            CEffect* pCEffect =iter->m_pInstance;
            if ( pCEffect != NULL )
                pCEffect->_SetDead();
        }
        iterEnd = m_coInstanceHandleList.end(LIST_RESERVE);
        for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_RESERVE);
            iter != iterEnd;
            ++iter )
        {
            CEffect* pCEffect =iter->m_pInstance;
            if ( pCEffect != NULL )
                pCEffect->_SetDead();
        }
    }
    else
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    {
        KInstanceHandleList::iterator iterNext;
        KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
        for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_LIVE);
            iter != iterEnd;
            iter = iterNext )
        {
            iterNext = iter; ++iterNext;
            CEffect* pCEffect =iter->m_pInstance;
            iter->m_pInstance = NULL;
            iter->m_eListType = LIST_FREE;
            SAFE_DELETE( pCEffect );
        }
        iterEnd = m_coInstanceHandleList.end(LIST_RESERVE);
        for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin(LIST_RESERVE);
            iter != iterEnd;
            iter = iterNext )
        {
            iterNext = iter; ++iterNext;
            CEffect* pCEffect =iter->m_pInstance;
            iter->m_pInstance = NULL;
            iter->m_eListType = LIST_FREE;
            SAFE_DELETE( pCEffect )
        }
        m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin(LIST_FREE), LIST_LIVE );
        m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin(LIST_FREE), LIST_RESERVE );
    }

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		SAFE_DELETE( pCEffect );
	}
	m_InstanceList.clear();

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

bool CX2DamageEffect::IsLiveInstance( CEffect* pInstance )
{
	if( NULL == pInstance )
		return false;
	CEffectHandle handle = pInstance->GetHandle();
    if ( handle == INVALID_DAMAGE_EFFECT_HANDLE )
        return false;

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = handle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( handle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    DecomposeHandle( dwHandle, wIndex, wStamp );
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
        return false;
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance != pInstance || info.m_wStamp != wStamp || info.m_eListType != LIST_LIVE
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        || pInstance->GetLive() == false
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        )
        return false;
    return true;
}

bool CX2DamageEffect::IsLiveInstanceHandle( CEffectHandle handle )
{
    if ( handle == INVALID_DAMAGE_EFFECT_HANDLE )
        return false;

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = handle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( handle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    DecomposeHandle( dwHandle, wIndex, wStamp );
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
        return false;
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance == NULL || info.m_wStamp != wStamp || info.m_eListType != LIST_LIVE
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        || info.m_pInstance->GetLive() == false
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX        
        )
        return false;
    return true;
}

CX2DamageEffect::CEffect*  CX2DamageEffect::GetInstance( CEffectHandle handle, bool bLiveOnly )
{
    if ( handle == INVALID_DAMAGE_EFFECT_HANDLE )
        return NULL;

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = handle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( handle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    DecomposeHandle( dwHandle, wIndex, wStamp );
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
        return NULL;
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance == NULL || info.m_wStamp != wStamp || bLiveOnly == true && 
        ( info.m_eListType != LIST_LIVE
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            || info.m_pInstance->GetLive() == false
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX     
         )
        )
        return NULL;
    return info.m_pInstance;
}

void    CX2DamageEffect::DestroyInstance_LUA( CEffect* pInstance )
{
    DestroyInstance( pInstance, false );
}

void    CX2DamageEffect::DestroyInstanceHandle( CEffectHandle& hInstance, bool bSilently )
{
	if( hInstance == INVALID_DAMAGE_EFFECT_HANDLE )
		return;

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = hInstance.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( hInstance );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    DecomposeHandle( dwHandle, wIndex, wStamp );
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
    {
        hInstance = INVALID_DAMAGE_EFFECT_HANDLE;
        return;
    }
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    CEffect* pInstance = info.m_pInstance;
    if ( pInstance == NULL || info.m_wStamp != wStamp )
    {
        hInstance = INVALID_DAMAGE_EFFECT_HANDLE;
        return;
    }

#ifdef NEW_SKILL_2010_11
    if ( bSilently == false && info.m_eListType == LIST_LIVE
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        && pInstance->GetLive() == true
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
		)
		pInstance->SetDieDamageEffect();
#endif NEW_SKILL_2010_11

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f83830 (DestroyInstanceHandle) @31f838c2-31f838c5 calls FUN_31f886c0 here,
	// after the die block and before the critical-loop test, whatever bSilently says. 31f886c0
	// copies the parent's SET_DIE_WHEN_MASTER_EFFECT_DIE list (CEffect+0x8d4, @31f886ec-31f886f6)
	// and destroys each child with DestroyInstanceHandle( child, false ) (@31f88714-31f88723), so
	// children play their own die effects. It has no other caller: a parent that just runs out of
	// life, and the manager's own delete loops (which SAFE_DELETE directly), leave children alone.
	{
		std::vector<CEffectHandle> vecDieWithMasterChildEffects;
		pInstance->JuneTakeDieWithMasterChildEffects( vecDieWithMasterChildEffects );
		for( size_t iDieWithMaster = 0; iDieWithMaster < vecDieWithMasterChildEffects.size(); iDieWithMaster++ )
			DestroyInstanceHandle( vecDieWithMasterChildEffects[iDieWithMaster], false );

		// The children's die effects can create instances, and m_coInstanceHandleList stores in a
		// std::vector, so 'info' may be stale now. Finish through a fresh lookup: the list is empty
		// and bSilently skips the die block that already ran above.
		if( false == vecDieWithMasterChildEffects.empty() )
		{
			DestroyInstanceHandle( hInstance, true );
			return;
		}
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    if ( IsInCriticalLoop() == true )
    {
        pInstance->_SetDead();
        hInstance = INVALID_DAMAGE_EFFECT_HANDLE;
        return;
    }
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX

    info.m_pInstance = NULL;
    info.m_eListType = LIST_FREE;
    SAFE_DELETE( pInstance );
    m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), wIndex );
    hInstance = INVALID_DAMAGE_EFFECT_HANDLE;
}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
void CX2DamageEffect::DestroyInstance( CEffect*& pInstance, bool bSilently )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
void CX2DamageEffect::DestroyInstance( CEffect* pInstance )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
{
	if( NULL == pInstance )
		return;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	CEffectHandle handle = pInstance->GetHandle();
    if ( handle == INVALID_DAMAGE_EFFECT_HANDLE )
    {
        pInstance = NULL;
        return;
    }

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = handle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( handle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    DecomposeHandle( dwHandle, wIndex, wStamp );
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
    {
        pInstance = NULL;
        return;
    }
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance != pInstance || info.m_wStamp != wStamp )
    {
        pInstance = NULL;
        return;
    }

#ifdef NEW_SKILL_2010_11
    if ( info.m_eListType == LIST_LIVE
        && bSilently == false
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        && pInstance->GetLive() == true
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
        )
	    pInstance->SetDieDamageEffect();
#endif NEW_SKILL_2010_11

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    if ( IsInCriticalLoop() == true )
    {
        pInstance->_SetDead();
        pInstance = NULL;
        return;
    }
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX

    info.m_pInstance = NULL;
    info.m_eListType = LIST_FREE;
    SAFE_DELETE( pInstance );
    m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), wIndex );
    pInstance = NULL;

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		if( pInstance == pCEffect )
		{
			//{{ JHKang / 강정훈 / 2010/11/24
#ifdef NEW_SKILL_2010_11
			pCEffect->SetDieDamageEffect();
#endif NEW_SKILL_2010_11
			//}} JHKang / 강정훈 / 2010/11/24

			SAFE_DELETE( pCEffect );
			m_InstanceList.erase( m_InstanceList.begin() + i );
			return;
		}
	}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

}

#ifdef EVE_ELECTRA

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

void CX2DamageEffect::DestroyInstanceHandleSilently( CEffectHandle& hInstance )
{
    DestroyInstanceHandle( hInstance, true );
}


void CX2DamageEffect::DestroyInstanceSilently( CEffect*& pInstance )
{
    DestroyInstance( pInstance, true );
}

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

void CX2DamageEffect::DestroyInstanceSilently( CEffect*& pInstance )
{

	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		if( pInstance == pCEffect )
		{
			SAFE_DELETE( pCEffect );
			pInstance = NULL;
			m_InstanceList.erase( m_InstanceList.begin() + i );
			return;
		}
	}


}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

void CX2DamageEffect::AddLaserGroupHitUnit( int _iLaserGroupID, CX2DamageManager::HitUnit _hitUnit )
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        ++iter )
    {
        CEffect* pCEffect = iter->m_pInstance;
        if ( pCEffect != NULL && 
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            pCEffect->GetLive() == true &&
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            _iLaserGroupID == pCEffect->GetLaserGroupID() )
        {
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            // 이 호출은 side effect 없음
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
			g_pData->GetDamageManager()->AddHitUnit( pCEffect->GetDamageData(), _hitUnit);

			//int tmp = i;
        }
    }

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		if( _iLaserGroupID == pCEffect->GetLaserGroupID() )
		{
			g_pData->GetDamageManager()->AddHitUnit( pCEffect->GetDamageData(), _hitUnit);

			int tmp = i;
		}
	}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}
#endif EVE_ELECTRA



CX2DamageEffect::CEffect* CX2DamageEffect::CreateInstanceParabolic( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate, 
																   const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTargetPos, const D3DXVECTOR3& vAcceleration, 
																   const float fTimeToReachTarget, const float fTimeToLive, const int iMeshPlayerIndex /*= 0*/ )
{


	ASSERT( NULL != pName );
	if( NULL == pName )
	{
#ifdef NO_MORE_ERROR_TEST
		InHouseLog( "이름이 없는 데미지 이펙트 생성 시도" );
#endif NO_MORE_ERROR_TEST

		return NULL;
	}

	CEffect* pCEffect = NULL;

    char aszBufUtf8[255] = {0,};
    WideCharToMultiByte( CP_UTF8, 0, pName, -1, aszBufUtf8, 255, NULL, NULL );

	if( m_LuaManager.BeginTable( aszBufUtf8 ) == true )
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
        {
            if ( m_coInstanceHandleList.storage_size() >= 0x10000L )
            {
                return NULL;
            }
            m_coInstanceHandleList.push_back_default(LIST_FREE);
            iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
            iterEmpty->m_eListType = LIST_FREE;
            ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
        }
        WORD   wIndex = (WORD) iterEmpty.GetIndex();
        CEffectHandle   hHandle;
        DWORD   dwHandle = 0;
        do
        {
            ++iterEmpty->m_wStamp;
            dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
            hHandle.SetValue( (int) dwHandle );
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
            hHandle = (int) dwHandle;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        } while ( hHandle == INVALID_DAMAGE_EFFECT_HANDLE );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		pCEffect = new CEffect( this, &m_LuaManager, pGameUnit, pName, fPowerRate, 
			vPos, vTargetPos, vAcceleration, 0.f, -1.f, fTimeToReachTarget, fTimeToLive, iMeshPlayerIndex
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            , hHandle
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            );

		if( pCEffect->GetLive() == false )
		{
#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
			{
			static map<wstring, DWORD> s_mapLastLoggedTime_livecheck;
			DWORD dwNow_livecheck = timeGetTime();
			wstring wstrKey_livecheck = pName;
			map<wstring, DWORD>::iterator itThrottle_livecheck = s_mapLastLoggedTime_livecheck.find( wstrKey_livecheck );
			if( itThrottle_livecheck == s_mapLastLoggedTime_livecheck.end() || dwNow_livecheck - itThrottle_livecheck->second >= 1000 )
			{
				s_mapLastLoggedTime_livecheck[ wstrKey_livecheck ] = dwNow_livecheck;
				CX2OfflineLog::Server( L"[RANOX_FX] effect not live: %s", pName );
			}
			}
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
			SAFE_DELETE( pCEffect );
			m_LuaManager.EndTable();
			return NULL;
		}
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterEmpty->m_pInstance = pCEffect;
        iterEmpty->m_eListType = LIST_LIVE;
        m_coInstanceHandleList.splice( m_coInstanceHandleList.end( LIST_LIVE ), iterEmpty );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		m_InstanceList.push_back( pCEffect );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		m_LuaManager.EndTable();
	}
	else
		return NULL;

	//PostProcessCreateInstance( pCEffect, fForceDownValueRate, fLifeTimeRate, iAddHitCount, fHitGap );

	return pCEffect;

}

														  

#ifdef EVE_ELECTRA
CX2DamageEffect::CEffect* CX2DamageEffect::CreateInstance( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate, 
															float x, float y, float z, 
															float angleXDegree, float angleYDegree, float angleZDegree,
															float moveAxisXDegree, float moveAxisYDegree, float moveAxisZDegree,
															float fLandPos /*= 0*/, bool bReserve /*= false*/, 															
															float fReserveTime /*= -1.f*/, 
															float fForceDownValueRate /*= 1.f*/, float fLifeTimeRate /*= 1.f*/, 
															int iAddHitCount /*= -1*/, float fHitGap /*= -1.f*/, const int iMeshPlayerIndex /*= 0*/, 
															float fHitAddMp /*= -1.f*/, bool bInsertFront /*= false */
#ifdef PET_SKILL_EFFECT_LINE_MAP
															, bool bPetSkillEffect /* = false*/
#endif //PET_SKILL_EFFECT_LINE_MAP
															)
#else
CX2DamageEffect::CEffect* CX2DamageEffect::CreateInstance( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate, 
	float x, float y, float z, 
	float angleXDegree, float angleYDegree, float angleZDegree,
	float moveAxisXDegree, float moveAxisYDegree, float moveAxisZDegree,
	float fLandPos /*= 0*/, bool bReserve /*= false*/, float fReserveTime /*= -1.f*/, 
	float fForceDownValueRate /*= 1.f*/, float fLifeTimeRate /*= 1.f*/, 
	int iAddHitCount /*= -1*/, float fHitGap /*= -1.f*/, const int iMeshPlayerIndex /*= 0*/, 
	float fHitAddMp /*= -1.f*/ )
#endif EVE_ELECTRA
{

	ASSERT( NULL != pName );
	if( NULL == pName )
	{
#ifdef NO_MORE_ERROR_TEST
		InHouseLog( "이름이 없는 데미지 이펙트 생성 시도" );
#endif NO_MORE_ERROR_TEST

		return NULL;
	}




	CEffect* pCEffect = NULL;

    char aszBufUtf8[255] = {0,};
    WideCharToMultiByte( CP_UTF8, 0, pName, -1, aszBufUtf8, 255, NULL, NULL );

	if( m_LuaManager.BeginTable( aszBufUtf8 ) == true )
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
        {
            if ( m_coInstanceHandleList.storage_size() >= 0x10000L )
            {
                return NULL;
            }
            m_coInstanceHandleList.push_back_default(LIST_FREE);
            iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
            iterEmpty->m_eListType = LIST_FREE;
            ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
        }
        WORD   wIndex = (WORD) iterEmpty.GetIndex();
        CEffectHandle  hHandle;
        DWORD   dwHandle = 0;
        do
        {
            ++iterEmpty->m_wStamp;
            dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
            hHandle.SetValue( (int) dwHandle );
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
            hHandle = (int) dwHandle;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        } while ( hHandle == INVALID_DAMAGE_EFFECT_HANDLE );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

		pCEffect = new CEffect( this, &m_LuaManager, pGameUnit, pName, fPowerRate, D3DXVECTOR3(x,y,z),
			D3DXVECTOR3(angleXDegree,angleYDegree,angleZDegree),
			D3DXVECTOR3(moveAxisXDegree,moveAxisYDegree,moveAxisZDegree),
			fLandPos, fReserveTime, 0.f, 0.f, iMeshPlayerIndex, 
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            hHandle,
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            fHitAddMp 
#ifdef PET_SKILL_EFFECT_LINE_MAP
			, bPetSkillEffect
#endif //PET_SKILL_EFFECT_LINE_MAP
			);

#ifdef FIX_EFFECT_REFLECT
		if ( NULL == pCEffect )
			return NULL;
#endif

		if( pCEffect->GetLive() == false )
		{
#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
			{
			static map<wstring, DWORD> s_mapLastLoggedTime_livecheck;
			DWORD dwNow_livecheck = timeGetTime();
			wstring wstrKey_livecheck = pName;
			map<wstring, DWORD>::iterator itThrottle_livecheck = s_mapLastLoggedTime_livecheck.find( wstrKey_livecheck );
			if( itThrottle_livecheck == s_mapLastLoggedTime_livecheck.end() || dwNow_livecheck - itThrottle_livecheck->second >= 1000 )
			{
				s_mapLastLoggedTime_livecheck[ wstrKey_livecheck ] = dwNow_livecheck;
				CX2OfflineLog::Server( L"[RANOX_FX] effect not live: %s", pName );
			}
			}
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
			SAFE_DELETE( pCEffect );
			m_LuaManager.EndTable();
			return NULL;
		}

#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
		
		if( NULL != pGameUnit && pCEffect->GetApplyScaleByUnit() == true )
		{
			if( pGameUnit->GetScaleByUnit() != D3DXVECTOR3( 1, 1, 1 ) )
			{
#ifdef VERIFY_STAT_BY_BUFF
				pCEffect->SetScaleByUnit( pGameUnit->GetVec3ScaleByUnit() );
#else	VERIFY_STAT_BY_BUFF
				pCEffect->SetScaleByUnit( pGameUnit->GetScaleByUnit() );
#endif // VERIFY_STAT_BY_BUFF

			}
		}

#endif GIANT_UNIT_GIANT_EFFECT_TEST

		if( bReserve == true )
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iterEmpty->m_pInstance = pCEffect;
            iterEmpty->m_eListType = LIST_RESERVE;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.end( LIST_RESERVE ), iterEmpty );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			m_ReserveInstanceList.push_back( pCEffect );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = ( pCEffect != NULL ) ? pCEffect->GetMainEffect() : NULL;
			if( NULL != pMeshInstance &&
				fReserveTime > 0.f )
			{
				pMeshInstance->SetDelayTime( fReserveTime );
			}
		}
		else
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iterEmpty->m_pInstance = pCEffect;
            iterEmpty->m_eListType = LIST_LIVE;
#ifdef  EVE_ELECTRA
            if ( bInsertFront == true )
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_LIVE ), iterEmpty );
            else
                m_coInstanceHandleList.splice( m_coInstanceHandleList.end( LIST_LIVE ), iterEmpty );
#else   EVE_ELECTRA
            m_coInstanceHandleList.splice( m_coInstanceHandleList.end( LIST_LIVE ), iterEmpty );
#endif  EVE_ELECTRA

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef EVE_ELECTRA
			if( bInsertFront == true)
			{
				vector<CEffect*> vecTempInstanceList;

				vecTempInstanceList.push_back(pCEffect);
				for( UINT i=0; i< m_InstanceList.size(); ++i )
				{
					vecTempInstanceList.push_back( m_InstanceList[i]);
				}
				m_InstanceList = vecTempInstanceList;
			}
			else
				m_InstanceList.push_back( pCEffect );
#else
			m_InstanceList.push_back( pCEffect );
#endif EVE_ELECTRA

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		}

		m_LuaManager.EndTable();
	}
	else
		return NULL;


	PostProcessCreateInstance( pCEffect, fForceDownValueRate, fLifeTimeRate, iAddHitCount, fHitGap );


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pCEffect->GetMainEffect();
	if( NULL != pMeshInstance )
	{
		if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
		{
			if( NULL != pGameUnit )
			{
				pMeshInstance->SetLandPosition( pGameUnit->GetLandPos().y );
#ifdef RAVEN_WEAPON_TAKER
				pCEffect->SetLandPositionOnLineMap( pGameUnit->GetLandPos() );
#endif RAVEN_WEAPON_TAKER
			}
		}
	}

#ifdef EFFECT_USE_LOG
	if( NULL != g_pMain->GetGameEdit() &&
		CX2GameEdit::ELL_EFFECT_LOG_1 <= g_pMain->GetGameEdit()->GetEffectLogLevel() )
	{
		dbg::clog << L"(데미지이펙트)" << pName << dbg::endl;
		KLOG("EffectLog.txt")  << L"DamageEffect : " << pName << fileout;
	}
#endif // EFFECT_USE_LOG

#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
	if( NULL == pCEffect )
	{
		static map<wstring, DWORD> s_mapLastLoggedTime_noblock;
		DWORD dwNow_noblock = timeGetTime();
		wstring wstrKey_noblock = pName;
		map<wstring, DWORD>::iterator itThrottle_noblock = s_mapLastLoggedTime_noblock.find( wstrKey_noblock );
		if( itThrottle_noblock == s_mapLastLoggedTime_noblock.end() || dwNow_noblock - itThrottle_noblock->second >= 1000 )
		{
			s_mapLastLoggedTime_noblock[ wstrKey_noblock ] = dwNow_noblock;
			CX2OfflineLog::Server( L"[RANOX_FX] no DamageEffect block: %s", pName );
		}
	}
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG2

	return pCEffect;
}


void CX2DamageEffect::PostProcessCreateInstance( CEffect* pCEffect, float fForceDownValueRate, float fLifeTimeRate, int iAddHitCount, float fHitGap )
{
	if( NULL != pCEffect )
	{
		if( 1.f != fForceDownValueRate )
		{
			pCEffect->GetDamageData().fForceDownValue *= fForceDownValueRate;
#ifdef INHERIT_FORCEDOWNRATE
			pCEffect->SetForceDownRate(fForceDownValueRate);
#endif INHERIT_FORCEDOWNRATE
		}

		if( 1.f != fLifeTimeRate )
		{
            CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pCEffect->GetMainEffect();
			if( NULL != pMeshInstance )
			{
				pMeshInstance->SetMaxLifeTime( pMeshInstance->GetMaxLifeTime() * fLifeTimeRate );
			}

			pCEffect->SetPassiveparticleTriggerTimeRate( fLifeTimeRate );
		}

		if( iAddHitCount > 0 && fHitGap <= 0.f )
		{
			fHitGap = pCEffect->GetDamageData().fHitGap;
		}

		if( iAddHitCount > 0 && fHitGap >= 0.f )
		{
			int iNewHitCount = pCEffect->GetDamageTime() + iAddHitCount;
			pCEffect->SetDamageTime( iNewHitCount );
			pCEffect->GetDamageData().fHitGap = fHitGap;
			pCEffect->GetDamageData().bReAttack = true;
		}
	}
}


void CX2DamageEffect::UnitDeleteProcess( CX2GameUnit* pGameUnit )
{

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    if ( IsInCriticalLoop() == true )
    {
        KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
        for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
            iter != iterEnd; 
            ++iter )
        {
            CEffect* pCEffect = iter->m_pInstance;
            if ( pCEffect != NULL && pGameUnit == pCEffect->GetOwnerUnit() )
            {
                pCEffect->_SetDead();
            }
        }
        iterEnd = m_coInstanceHandleList.end( LIST_RESERVE );
        for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_RESERVE );
            iter != iterEnd; 
            ++iter )
        {
            CEffect* pCEffect = iter->m_pInstance;
            if ( pCEffect != NULL && pGameUnit == pCEffect->GetOwnerUnit() )
            {
                pCEffect->_SetDead();
            }
        }
    }
    else
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
    {
        KInstanceHandleList::iterator iterNext;
        KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
        for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
            iter != iterEnd; 
            iter = iterNext )
        {
            iterNext = iter; ++iterNext;
            CEffect* pCEffect = iter->m_pInstance;
            if ( pCEffect == NULL )
            {
                iter->m_eListType = LIST_FREE;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            }
            else if ( pGameUnit == pCEffect->GetOwnerUnit() )
            {
                iter->m_pInstance = NULL;
                iter->m_eListType = LIST_FREE;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
                SAFE_DELETE( pCEffect );
            }
        }
        iterEnd = m_coInstanceHandleList.end( LIST_RESERVE );
        for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_RESERVE );
            iter != iterEnd; 
            iter = iterNext )
        {
            iterNext = iter; ++iterNext;
            CEffect* pCEffect = iter->m_pInstance;
            if ( pCEffect == NULL )
            {
                iter->m_eListType = LIST_FREE;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            }
            else if ( pGameUnit == pCEffect->GetOwnerUnit() )
            {
                iter->m_pInstance = NULL;
                iter->m_eListType = LIST_FREE;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
                SAFE_DELETE( pCEffect );
            }
        }
    }


#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	for( int i = 0; i < (int)m_InstanceList.size(); i++ )
	{
		CEffect* pCEffect = m_InstanceList[i];
		
		if ( pCEffect == NULL )
			continue;

		if( pGameUnit == pCEffect->GetOwnerUnit() )
		{
			SAFE_DELETE( pCEffect );
			m_InstanceList.erase( m_InstanceList.begin() + i );
			i--;
		}
	}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}






/////////////////////////////////////////////////////////////////////////////
// CEffect
/////////////////////////////////////////////////////////////////////////////

CX2DamageEffect::CEffect::CEffect( CX2DamageEffect* pManager, KLuaManager* pLuaManager, CX2GameUnit* pGameUnit, 
								  const WCHAR* pName, float fPowerRate, D3DXVECTOR3 pos, D3DXVECTOR3 angleDegree, D3DXVECTOR3 moveAxisDegree, 
								  float fLandPos /*= 0.f*/, float fCreationDelayTime /*= -1.f*/, float fParabolicTimeToReachTarget /*= 0.f*/, float fParabolicTimeToLive /*= 0.f*/,
								  const int iMeshPlayerIndex /*= 0*/, 
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                                  CEffectHandle  hHandle /*=INVALID_DAMAGE_EFFECT_HANDLE*/,
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                                  float fHitAddMp /*=-1.f*/ 
#ifdef PET_SKILL_EFFECT_LINE_MAP
								  , bool bPetSkillEffect /*= false*/
#endif //PET_SKILL_EFFECT_LINE_MAP
								  )
#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
: m_vHitDamageScaleByUnit( 1, 1, 1 )
, m_vDieDamageScaleByUnit( 1, 1, 1 )
#endif GIANT_UNIT_GIANT_EFFECT_TEST
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
, m_hHandle( hHandle )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
#ifdef STICKY_DAMAGE_EFFECT_TEST
, m_bEnableAttack( true )
, m_bTraceAttackedUnit( false )
, m_fLifeTimeAfterAttack( -1.f )
, m_pTraceUnit( NULL )
, m_vTracePosition( 0, 0, 0 )
#endif STICKY_DAMAGE_EFFECT_TEST

, m_bFollowLineNew( false )

#ifdef FIX_EFFECT_REFLECT
, m_iReflectCount( 2 )
#endif

#ifdef FIX_AFTER_IMAGE
, m_pAfterImage( NULL )
, m_fDurationTime( 0.f )
#endif

#ifdef SERV_RENA_NIGHT_WATCHER
, m_iIndex( -1 )					/// 해당 DamageEffect의 Index ( 지연 폭발용 DamageEffect 인식 )
, m_iType( DET_NONE )			/// 해당 DamageEffect의 Type  ( 현재 화살의 DamageEffect가 어떤 것인지 인식 )
, m_iOrderNum( -1 )				/// 폭발의 화살 발사 순서
, m_fAddDieEffectDamage( 0.f )	/// 해당 데미지 이펙트의 fPhysic, fMagic에 추가되는 수치
#endif SERV_RENA_NIGHT_WATCHER
#ifdef SERV_EVE_BATTLE_SERAPH
, m_bCheckSpectro( false )
#endif
, m_bFollowLineRight( false )
#ifdef NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE
, m_bShouldNotifyToOwnerUnitWhenDie( false )
#endif // NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE
{
	KTDXPROFILE();

#ifdef FIX_AFTER_IMAGE
	INIT_VECTOR2( m_afterImageTime, -1.f, -1.f );
#endif

#ifdef RAVEN_WEAPON_TAKER
	m_vLandPositionOnLineMap = D3DXVECTOR3( 0, 0, 0 );
#endif RAVEN_WEAPON_TAKER

	m_bFirstFrame			= true;
	m_bLive					= true;
	m_pManager				= pManager;
	m_pLuaManager			= pLuaManager;
	m_Pos					= pos;
	m_BeforePos				= D3DXVECTOR3(0,0,0);
	m_AngleDegree			= angleDegree;
	m_MoveAxisDegree		= moveAxisDegree;
	m_fLandPos				= fLandPos;
	m_fPowerRate			= fPowerRate;

	m_bUseDXMatrix			= false;

	m_DamageData.attackerType		= CX2DamageManager::AT_EFFECT;
	m_DamageData.m_bMeleeAttack		= false;	/// 데미지이펙트는 기본적으로 원거리 공격으로 셋팅
	m_DamageData.optrAttackerGameUnit		= pGameUnit;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    m_DamageData.hAttackerEffect	= hHandle;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_DamageData.pAttackerEffect	= this;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	m_Name					= pName;

	if( NULL != g_pX2Game && NULL != pGameUnit )
	{
		m_optrOwnerUnit	= pGameUnit;
		if( null != m_optrOwnerUnit )
			m_bFollowLineRight	= m_optrOwnerUnit->GetIsRight();
#ifdef PET_SKILL_EFFECT_LINE_MAP
		if ( pGameUnit != NULL && bPetSkillEffect == true )
		{
			CX2GUUser *pMaster = (CX2GUUser *)pGameUnit;
			if ( pMaster != NULL && g_pData != NULL && g_pData->GetPetManager() != NULL )
			{	
				CX2PET *pPet = g_pData->GetPetManager()->GetPet( pMaster->GetUnitUID() );
				if ( pPet != NULL )
					m_bFollowLineRight = pPet->GetIsRight();
			}

		}
#endif //PET_SKILL_EFFECT_LINE_MAP
	}
	else
	{
		m_optrOwnerUnit.Reset();
	}
	
	m_hMeshInstHandle		= INVALID_MESH_INSTANCE_HANDLE;

	m_DamageTime						= 1;
	m_DamageTimeNow						= 0;
#ifdef CHUNG_SECOND_CLASS_CHANGE
	m_PrevDamageTime					= 0;
#endif
	m_bAttackedByUnit					= false;
	m_bAttackedByEffect					= false;
	m_bReflect							= false;
	m_fDamageDataChangeTime				= -1.0f;
	m_fDamageDataChangeTimeGlobalTime	= -1.0f;
		

	m_fCreationDelayTime				= fCreationDelayTime;

#ifdef NEW_SKILL_TREE
	m_vPassiveParticleScale	= D3DXVECTOR3(1.f,1.f,1.f);
	m_vHitDamageScale		= D3DXVECTOR3(1.f,1.f,1.f);
	m_vDieDamageScale		= D3DXVECTOR3(1.f,1.f,1.f);

	m_fPassiveParticleTriggerTimeRate = -1.f;
	m_bReflected			= false;
#endif NEW_SKILL_TREE

#ifdef SKILL_CASH_10_TEST
	m_fReactAnimSpeed	= 0.f;
	m_bFollowOnLine		= false;
#endif

#ifdef RAVEN_WEAPON_TAKER
	m_bMustOnLine		= false;
#endif RAVEN_WEAPON_TAKER

	//{{ kimhc // 2010.2.3 //	이브 2차 전직
#ifdef	EVE_SECOND_CLASS_CHANGE
	m_vMoveAngle				= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// 움직이는 각도 Rotate에 plus 함
#endif	EVE_SECOND_CLASS_CHANGE
	//}} kimhc // 2010.2.3 //	이브 2차 전직

	m_bApplyScaleByUnit = true;
#ifdef INHERIT_FORCEDOWNRATE
	m_fForceDownRate		= 1.f;
	m_bInheritForceDownRate	= false;
#endif INHERIT_FORCEDOWNRATE

#ifdef EVE_ELECTRA
	m_iLaserGroupID			= 0;
#endif EVE_ELECTRA

#ifdef DAMAGE_EFFECT_GROUP_DAMAGE
	m_bGroupDamage = false;			/// 데미지 이펙트 그룹 데미지 설정
#endif DAMAGE_EFFECT_GROUP_DAMAGE

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	m_bUseCannonBall = false;
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

	m_StartParticleDataList.reserve( 8 );
	m_PassiveParticleDataList.reserve( 8 );
	m_TimeParticleDataList.reserve( 8 );
	m_CameraDataList.reserve( 4 );
	m_ScreenDataList.reserve( 4 );
	m_SoundDataList.reserve( 4 );
	
	m_DamageEffectDataList.reserve( 4 );

	m_vecHitDamageEffect.reserve( 4 );
	m_vecDieDamageEffect.reserve( 4 );




	//자신의 테이블에서 메인 이펙트와 데미지 데이타, 기타 옵션들을 읽어온다.
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @ 31f7b42c-31f7b824: reads APPLIED_SKILL_ID (int getter 32fe9fd0,
	// default 0, @31f7b483) and, if that is 0, APPLIED_SKILL_ID_NOT_AFFECT_MEMO (same getter,
	// @31f7b5ec) inside a BeginTable("DAMAGE_DATA")/EndTable bracket, then calls FUN_31fcf3e0
	// (candidate, not identified this phase: reads a "memo" pointer at user+0x867c and looks the
	// skill id up in it) to decide whether every "%s_ULTIMATE" read below tries the _ULTIMATE
	// table first. The "applied skill ID" system is absent (JUNE2017_EFFECT_KEY_MATRIX.md), so
	// both ids are parsed, stored and logged only; the flag is forced false, which reproduces
	// today's March behaviour exactly (every "_ULTIMATE" read below is skipped).
	m_bUseUltimateDamageData = false;
	if( true == m_pLuaManager->BeginTable( "DAMAGE_DATA" ) )
	{
		PLUA_GET_VALUE( m_pLuaManager, "APPLIED_SKILL_ID",					m_iAppliedSkillID,					0 );
		PLUA_GET_VALUE( m_pLuaManager, "APPLIED_SKILL_ID_NOT_AFFECT_MEMO",	m_iAppliedSkillIDNotAffectMemo,		0 );
		m_pLuaManager->EndTable();
	}
	else
	{
		m_iAppliedSkillID = 0;
		m_iAppliedSkillIDNotAffectMemo = 0;
	}
#ifdef SERV_IRUHADEV_OFFLINE
	// Code review fix: this needs CX2OfflineLog, which only exists under SERV_IRUHADEV_OFFLINE.
	if( 0 != m_iAppliedSkillID || 0 != m_iAppliedSkillIDNotAffectMemo )
	{
		static map<wstring, DWORD> s_mapLastLoggedTime_appliedSkillId;
		DWORD dwNow_appliedSkillId = timeGetTime();
		map<wstring, DWORD>::iterator itThrottle_appliedSkillId = s_mapLastLoggedTime_appliedSkillId.find( pName );
		if( itThrottle_appliedSkillId == s_mapLastLoggedTime_appliedSkillId.end() || dwNow_appliedSkillId - itThrottle_appliedSkillId->second >= 1000 )
		{
			s_mapLastLoggedTime_appliedSkillId[ pName ] = dwNow_appliedSkillId;
			CX2OfflineLog::Server( L"JUNE_FX unsupported APPLIED_SKILL_ID/_NOT_AFFECT_MEMO in %s: applied-skill-ID system absent (%d/%d)",
				pName, m_iAppliedSkillID, m_iAppliedSkillIDNotAffectMemo );
		}
	}
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	wstring mainEffName = L"";
	if( 0 == iMeshPlayerIndex )
	{
		PLUA_GET_VALUE( m_pLuaManager, "MAIN_EFFECT_NAME", mainEffName, L"" );
	}
	else
	{
		char tableName[32] = "";
		StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "MAIN_EFFECT_NAME%d", iMeshPlayerIndex );
		PLUA_GET_VALUE( m_pLuaManager, tableName, mainEffName, L"" );
	}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7b874->31f8ba80: MAIN_EFFECT_NAME_ULTIMATE, wstring, default L"", read only
	// when m_bUseUltimateDamageData (forced false above); March's MAIN_EFFECT_NAME above already
	// covers the fallback June takes when this read is skipped or fails.
	m_strMainEffectNameUltimate = L"";
	if( true == m_bUseUltimateDamageData )
		PLUA_GET_VALUE( m_pLuaManager, "MAIN_EFFECT_NAME_ULTIMATE", m_strMainEffectNameUltimate, L"" );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	ASSERT( false == mainEffName.empty() );

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
    if ( g_pX2Game != NULL )
    {
	    if( fParabolicTimeToLive > 0.f )
	    {
		    const D3DXVECTOR3 vTargetPos = m_AngleDegree;
		    const D3DXVECTOR3 vAcceleration = m_MoveAxisDegree;
		    pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstanceParabolic( static_cast<CKTDGObject*>( m_optrOwnerUnit.GetObservable() ), mainEffName.c_str(), m_Pos, vTargetPos, vAcceleration, fParabolicTimeToReachTarget, fParabolicTimeToLive );
	    }
	    else
	    {
		    pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( m_optrOwnerUnit.GetObservable() ),  mainEffName.c_str(), m_Pos, m_AngleDegree, m_MoveAxisDegree );
	    }
    }


	if( NULL == pMeshInst )
	{
#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
		{
		static map<wstring, DWORD> s_mapLastLoggedTime_nomesh;
		DWORD dwNow_nomesh = timeGetTime();
		map<wstring, DWORD>::iterator itThrottle_nomesh = s_mapLastLoggedTime_nomesh.find( mainEffName );
		if( itThrottle_nomesh == s_mapLastLoggedTime_nomesh.end() || dwNow_nomesh - itThrottle_nomesh->second >= 1000 )
		{
			s_mapLastLoggedTime_nomesh[ mainEffName ] = dwNow_nomesh;
			CX2OfflineLog::Server( L"[RANOX_FX] no XMesh templet: %s", mainEffName.c_str() );
		}
		}
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG2
		m_bLive = false;
	}
	else
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION
        pMeshInst->SetPerFrameSimulation( true );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION
#ifdef RIDING_SYSTEM
		float fLiveTime = -1.f;
		PLUA_GET_VALUE( m_pLuaManager, "MAIN_EFFECT_TIME", fLiveTime, -1.f );
		if ( fLiveTime > -1.f  )
			pMeshInst->SetMaxLifeTime( fLiveTime );
#endif //RIDING_SYSTEM
		m_hMeshInstHandle = pMeshInst->GetHandle();
		pMeshInst->SetLandPosition( fLandPos );

		m_vMainEffectAxisAngleDegree = pMeshInst->GetMatrix().GetRotateDegree();
		m_vMainEffectPosition = pMeshInst->GetMatrix().GetPos();


		bool bNotCull = false;
		PLUA_GET_VALUE( m_pLuaManager, "NOT_CULL", bNotCull, false );
		if( bNotCull == true )
			pMeshInst->SetBoundingRadius( 0.0f );

	}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7bff3: APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION, int getter
	// 32fe9fd0, default 0 (June: NEG AL / SBB AL,AL / AND AL,[temp], same shape as the
	// EffectSet-level read of this key, Phase 2's hand-verified note). No in-scope June data
	// uses it at this scope (binary-only); the effect-alpha option itself is absent from this
	// build (see JUNE2017_EFFECT_KEY_MATRIX.md's *Absent systems*), so it is stored and never
	// wired into behaviour, same as the EffectSet-level one (Phase 2).
	PLUA_GET_VALUE( m_pLuaManager, "APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION",	m_iApplyEffectAlphaConditionByOption,	0 );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	PLUA_GET_VALUE( m_pLuaManager, "USE_CANNONBALL",						m_bUseCannonBall,		false );
	float fUseCannonBallDamageRel = 1.f;
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7c343: USE_ADVANCE_COLLISIOIN_CHECK (studio typo, kept), bool
	// getter 32fe9e40, default false
	PLUA_GET_VALUE( m_pLuaManager, "USE_ADVANCE_COLLISIOIN_CHECK",			m_bUseAdvanceCollisionCheck,	false );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
	// Phase 8c: every SetDamageDataFromLUA call below (DAMAGE_DATA[_ULTIMATE], DAMAGE_DATA_NEXT and its
	// numbered chain) prints this templet's name in its rule-5 JUNE_FX unsupported lines. Restored when
	// the ctor returns.
	CX2DamageManager::JuneDamageDataTempletScope kJuneDamageDataTempletScope( g_pData->GetDamageManager(), GetName() );
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7c63f/31f7c66c: two SetDamageDataFromLUA calls. "DAMAGE_DATA_ULTIMATE"
	// is tried first only when m_bUseUltimateDamageData (forced false above); "DAMAGE_DATA" is read
	// unconditionally right after (June ignores its return value). With the flag forced false this
	// reduces to exactly March's single call below, restated so the _ULTIMATE wiring is in place for
	// when Phase 7/8 can turn the flag on. m_bUsedDamageDataUltimate records which happened, for
	// Phase 8's DAMAGE_DATA/DAMAGE_DATA_ULTIMATE key work.
	m_bUsedDamageDataUltimate = false;
	if( true == m_bUseUltimateDamageData )
		m_bUsedDamageDataUltimate = g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_DamageData, *pLuaManager, "DAMAGE_DATA_ULTIMATE", m_fPowerRate );
	if( false == m_bUsedDamageDataUltimate )
		g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_DamageData, *pLuaManager, "DAMAGE_DATA", m_fPowerRate );
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_DamageData, *pLuaManager, "DAMAGE_DATA", m_fPowerRate );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7c6b7-31f7c854: IGNORE_MI_HUGE, bool getter 32fe9e40 into a
	// stack local (June never persists it), default false; when false it feeds a scale
	// override (31f6fdc0, not identified this phase) that overwrites the main effect's
	// (1,1,1) scale with a per-effect value at +0x7d0. Stored as a member so Phase 7 can
	// wire that override in.
	PLUA_GET_VALUE( m_pLuaManager, "IGNORE_MI_HUGE",						m_bIgnoreMIHuge,		false );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f7aab0 @31f7c795-31f7c83e (Phase 7a): with IGNORE_MI_HUGE false, June loads
	// s = [this+0x7d0] (m_DamageData +0x5d0; m_DamageData is +0x200, the pointer OnFrameMove hands
	// DamageCheck @31f8a1a9) and calls 31f6fdc0( s, s, s ), then copies s to +0xb24/+0xb28/+0xb2c.
	// NOT transcribed. No June writer of that DamageData field was found: SetDamageDataFromLUA
	// 32632390 stores +0x5c4..+0x5fc but never +0x5d0/+0x5dc, and a store scan of the DamageEffect
	// (31f50000-31f90000) and DamageManager (32600000-32680000) regions finds no direct write. So
	// the scale's value is unknown and there is no honest March value to feed it. Deferred with
	// its source; see JUNE2017_EFFECT_KEY_MATRIX.md.
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

	
	if(fHitAddMp >= 0.f)
	{
		m_DamageData.fHitAddMP = fHitAddMp;
	}

	if( m_DamageData.attackType != CX2DamageManager::AT_SPECIAL )
	{
		if( NULL != g_pX2Game && null != m_optrOwnerUnit )
		{
			// 패시브 스킬에 따라 강제다운 수치 조정
			if( 1.f != m_optrOwnerUnit->GetSkillRelatedData().GetForceDownValueRateOfRangeAttack() )
			{
				m_DamageData.fForceDownValue *= m_optrOwnerUnit->GetSkillRelatedData().GetForceDownValueRateOfRangeAttack();
			}		
		}		
	}
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN

#ifndef UPGRADE_SKILL_SYSTEM_2013
	else
	{
		fUseCannonBallDamageRel = 0.3f;
	}
#endif // UPGRADE_SKILL_SYSTEM_2013

	if( m_bUseCannonBall == true && null != m_optrOwnerUnit && m_optrOwnerUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser *pUser = static_cast<CX2GUUser*>( m_optrOwnerUnit.GetObservable() );
#ifdef SERV_CHUNG_TACTICAL_TROOPER
		if( ( pUser->GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || pUser->GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER ) 
			&& pUser->GetCannonBallDamageUp() > 0.f )
#else  SERV_CHUNG_TACTICAL_TROOPER
		if( pUser->GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN && pUser->GetCannonBallDamageUp() > 0.f )
#endif SERV_CHUNG_TACTICAL_TROOPER
		{			
			float fUpDamage = pUser->GetCannonBallDamageUp() * fUseCannonBallDamageRel;
			float fUpPhysicDamage = m_DamageData.damage.fPhysic;
			float fUpMagicDamage = m_DamageData.damage.fMagic;
			fUpPhysicDamage = (fUpPhysicDamage * fUpDamage);
			fUpMagicDamage = (fUpMagicDamage * fUpDamage);

			m_DamageData.damage.fPhysic					+= fUpPhysicDamage;
			m_DamageData.damage.fMagic					+= fUpMagicDamage;

			float fCannonBallDamageUp = m_fPowerRate + (m_fPowerRate * fUpDamage);
			m_DamageData.m_fDecreaseForceDown = fCannonBallDamageUp * pUser->GetForceDownRelDamageRateBase();
			
			if( m_DamageData.m_ExtraDamage.m_bFixedDamage == false )
			{
				m_DamageData.m_ExtraDamage.m_DamagePerSec	*= fCannonBallDamageUp;
				m_DamageData.m_ExtraDamage.m_Damage			*= fCannonBallDamageUp;
			}	
		}
	}
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

	PLUA_GET_VALUE_ENUM( m_pLuaManager, "EFFECT_TYPE", m_EffectType, CX2DamageEffect::EFFECT_TYPE, DEET_NONE );
	PLUA_GET_VALUE( m_pLuaManager, "DAMAGE_TIME",							m_DamageTime,						1 );
	PLUA_GET_VALUE( m_pLuaManager, "ATTACKED_BY_UNIT",						m_bAttackedByUnit,					false );
	PLUA_GET_VALUE( m_pLuaManager, "ATTACKED_BY_EFFECT",					m_bAttackedByEffect,				false );
	PLUA_GET_VALUE( m_pLuaManager, "REFLECT",								m_bReflect,							false );
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7d380: USE_OWNER_ATTACK_SPEED, bool getter 32fe9e40, default false
	PLUA_GET_VALUE( m_pLuaManager, "USE_OWNER_ATTACK_SPEED",				m_bUseOwnerAttackSpeed,				false );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f7aab0 @31f7ae10-31f7aec1: the USE_OWNER_ATTACK_SPEED window cache
	// (+0x85c/+0x860) starts at -1.0 (0x334380f0), so CEffect::OnFrameMove fills it on the first
	// frame. @31f7b279: the DAMAGE_DATA_CHANGE_TIME counter (+0x8f8) starts at 0.
	m_fOwnerAttackSpeedAttackTimeStart	= -1.f;
	m_fOwnerAttackSpeedAttackTimeEnd	= -1.f;
	m_iDamageDataChangeTimeIndex		= 0;
	m_bInheritDamageFromMaster		= false;
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
	// June2017 FUN_31f7aab0 (CEffect ctor), effect completion C6b: the spawn gates +0xb44-+0xb47, +0xb5c/+0xb60 and +0xb64/+0xb65
	// start at 0 (funcmap rows CEffect+0xb44.. tagged Effect C6a). Only class code sets them, so in March they stay 0.
	m_bInheritIgnoreStateDefence		= false;
	m_bInheritCountAsOwnerSkillHit		= false;
	m_bInheritBuffFactorToChild			= false;
	m_bInheritIgnoreDefence				= false;
	m_bInheritMainEffectLifeTimeRate	= false;
	m_fMainEffectLifeTimeRate			= 0.f;
	m_bHyper							= false;
	m_bInheritAttackTriggerType			= false;
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	PLUA_GET_VALUE( m_pLuaManager, "ATTACK_TIME_START",						m_AttackTime.x,						0.0f );
	PLUA_GET_VALUE( m_pLuaManager, "ATTACK_TIME_END",						m_AttackTime.y,						100.0f );
	PLUA_GET_VALUE( m_pLuaManager, "USE_GLOBAL_TIME",						m_bGlobalTime,						false );
	PLUA_GET_VALUE( m_pLuaManager, "DAMAGE_DATA_CHANGE_TIME",				m_fDamageDataChangeTime,			-1.0f );
	PLUA_GET_VALUE( m_pLuaManager, "DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME",	m_fDamageDataChangeTimeGlobalTime,	-1.0f );
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f64b20 (shared by both keys, selected by a bool arg), called from the ctor
	// @31f7d894 (DAMAGE_DATA_CHANGE_TIME_ULTIMATE) / @31f7d8a5 (DAMAGE_DATA_CHANGE_TIME) and
	// @31f7d8ba (_GLOBAL_TIME_ULTIMATE) / @31f7d8cc (_GLOBAL_TIME): semantics-differ from the
	// scalar March already reads just above. At this scope June reads the key as an indexed
	// float array -- BeginTable(key), then GetValue(1..), stopping at the first miss -- not a
	// single float; an "_ULTIMATE" table (tried first when m_bUseUltimateDamageData) that
	// BeginTable's but is empty still counts as read and skips the base table (32fea820's
	// first-index miss still EndTable()s and returns true). Current March/Ranox data writes
	// this key as a plain number (`KEY = 0.5`), which fails BeginTable and leaves these vectors
	// empty, so this is additive: it changes nothing until June's own table-shaped data is
	// adopted (Phase 11). Kept beside the existing scalar members, whose consumers this does
	// not touch.
	m_vecDamageDataChangeTime.clear();
	bool bReadUltimateChangeTime = false;
	if( true == m_bUseUltimateDamageData && true == m_pLuaManager->BeginTable( "DAMAGE_DATA_CHANGE_TIME_ULTIMATE" ) )
	{
		bReadUltimateChangeTime = true;
		float fChangeTime = 0.f;
		for( int i = 1; true == m_pLuaManager->GetValue( i, fChangeTime ); i++ )
			m_vecDamageDataChangeTime.push_back( fChangeTime );
		m_pLuaManager->EndTable();
	}
	if( false == bReadUltimateChangeTime && true == m_pLuaManager->BeginTable( "DAMAGE_DATA_CHANGE_TIME" ) )
	{
		float fChangeTime = 0.f;
		for( int i = 1; true == m_pLuaManager->GetValue( i, fChangeTime ); i++ )
			m_vecDamageDataChangeTime.push_back( fChangeTime );
		m_pLuaManager->EndTable();
	}

	m_vecDamageDataChangeTimeGlobalTime.clear();
	bool bReadUltimateChangeTimeGlobal = false;
	if( true == m_bUseUltimateDamageData && true == m_pLuaManager->BeginTable( "DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME_ULTIMATE" ) )
	{
		bReadUltimateChangeTimeGlobal = true;
		float fChangeTimeGlobal = 0.f;
		for( int i = 1; true == m_pLuaManager->GetValue( i, fChangeTimeGlobal ); i++ )
			m_vecDamageDataChangeTimeGlobalTime.push_back( fChangeTimeGlobal );
		m_pLuaManager->EndTable();
	}
	if( false == bReadUltimateChangeTimeGlobal && true == m_pLuaManager->BeginTable( "DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME" ) )
	{
		float fChangeTimeGlobal = 0.f;
		for( int i = 1; true == m_pLuaManager->GetValue( i, fChangeTimeGlobal ); i++ )
			m_vecDamageDataChangeTimeGlobalTime.push_back( fChangeTimeGlobal );
		m_pLuaManager->EndTable();
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	PLUA_GET_VALUE( m_pLuaManager, "CLEAR_HIT_UNIT_ON_DAMAGE_DATA_CHANGE",	m_bClearHitUnitOnChangeDamageData,	false );
	PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE",							m_bFollowLine,						false );

	PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE_NEW",						m_bFollowLineNew,						false );

	ASSERT( false == m_bFollowLineNew || true == m_bFollowLine );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7dbcd->31f8b370 / FUN_31f7dbeb->31f8b370: FOLLOW_LINE_ULTIMATE (to-implement)
	// and FOLLOW_LINE_NEW_ULTIMATE (binary-only), bool, default false, read only when
	// m_bUseUltimateDamageData (forced false above); the existing FOLLOW_LINE/FOLLOW_LINE_NEW
	// reads above already cover the fallback June takes when these are skipped or fail.
	m_bFollowLineUltimate = false;
	m_bFollowLineNewUltimate = false;
	if( true == m_bUseUltimateDamageData )
	{
		PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE_ULTIMATE",				m_bFollowLineUltimate,		false );
		PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE_NEW_ULTIMATE",			m_bFollowLineNewUltimate,	false );
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

//{{ kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록
#ifdef	EVE_SECOND_CLASS_CHANGE
	PLUA_GET_VALUE( m_pLuaManager, "REFRESH_LAND_POS",						m_bRefreshLandPos,					true );
#endif	EVE_SECOND_CLASS_CHANGE
//}} kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록


#ifdef SKILL_CASH_10_TEST
	PLUA_GET_VALUE( m_pLuaManager, "REACT_ANIMSPEED",						m_fReactAnimSpeed,					0.f );
	PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_ON_LINE",						m_bFollowOnLine,					false );
	
#endif

#ifdef RAVEN_WEAPON_TAKER
	PLUA_GET_VALUE( m_pLuaManager, "MUST_ON_LINE",							m_bMustOnLine,						false );
#endif RAVEN_WEAPON_TAKER

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0: FOLLOW_LINE_WITH_CUSTOM_ROTATE (@31f7dfea, bool getter 32fe9e40,
	// default false), FOLLOW_LINE_GRAVITY (@31f7e306, same shape), TRACE_OWNER_BONE (@31f7e469,
	// wstring getter 32feae90, default L"") and TRACE_OWNER_BONE_ONLY_ROTATE (@31f7e5d9, same
	// shape).
	PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE_WITH_CUSTOM_ROTATE",		m_bFollowLineWithCustomRotate,		false );
	PLUA_GET_VALUE( m_pLuaManager, "FOLLOW_LINE_GRAVITY",					m_bFollowLineGravity,				false );
	PLUA_GET_VALUE( m_pLuaManager, "TRACE_OWNER_BONE",						m_strTraceOwnerBone,				L"" );
	PLUA_GET_VALUE( m_pLuaManager, "TRACE_OWNER_BONE_ONLY_ROTATE",			m_strTraceOwnerBoneOnlyRotate,		L"" );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE



#ifdef STICKY_DAMAGE_EFFECT_TEST

	PLUA_GET_VALUE( m_pLuaManager, "TRACE_ATTACKED_UNIT",				m_bTraceAttackedUnit,		false );
	PLUA_GET_VALUE( m_pLuaManager, "LIFE_TIME_AFTER_ATTACK",			m_fLifeTimeAfterAttack,		-1.f );
	PLUA_GET_VALUE( m_pLuaManager, "TRACE_ATTACKED_UNIT_POSITION_X",	m_vTracePosition.x,			0.f );
	PLUA_GET_VALUE( m_pLuaManager, "TRACE_ATTACKED_UNIT_POSITION_Y",	m_vTracePosition.y,			0.f );
	PLUA_GET_VALUE( m_pLuaManager, "TRACE_ATTACKED_UNIT_POSITION_Z",	m_vTracePosition.z,			0.f );

#endif STICKY_DAMAGE_EFFECT_TEST

	if( m_bFollowLine == true )
		FollowLine();

	LoadLockOnData();
	LoadBlackHoleData();
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	LoadDieCheckCondition();
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE


#ifdef _SIN_MOVE_TEST_
	LoadSinMoveData();
#endif _SIN_MOVE_TEST_
	
	//{{ kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	LoadDieDamageEffect();
	LoadStartParticle();
	LoadPassiveParticle();
	LoadTimeParticle();
	LoadCameraData();
	LoadScreenData();
	LoadSoundData();
	LoadDamageEffect();
	LoadHitParticle();
	LoadHitDamageEffect();
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 (ctor) @31f7e852-31f7e867: HIT_EFFECT_SET%d, DIE_CREATE_NPC%d,
	// TOUCH_LAND_DAMAGE_EFFECT%d, in that call order. Phase 6b: parse only.
	LoadHitEffectSet();
	LoadDieCreateNpc();
	LoadTouchLandDamageEffect();
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	m_DamageEffectDataInLua.m_pDamageDataNext = new CX2DamageManager::DamageData();
	*(m_DamageEffectDataInLua.m_pDamageDataNext) = m_DamageData;

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	m_DamageEffectDataInLua.m_pDamageDataNext->m_vecBuffFactorPtr.clear();
#endif //UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
	// June2017 FUN_31f79c20 @31f79ce9 (effect completion C6b): the unnumbered NEXT table resets the effect-set index stamp
	// (DamageData +0x638) to -1 after the parent copy and before SetDamageDataFromLUA. June resets +0x5e8 (the attack serial)
	// to -1 at @31f79ce3 too, which Phase 9b did not port (open june-buff item, left alone). The numbered NEXT2.. entries below
	// reset only +0x5e8 (@31f79e07), so they keep the parent copy's +0x638.
	m_DamageEffectDataInLua.m_pDamageDataNext->m_iJuneDamageEffectIndex = -1;
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
	// June2017 FUN_31f7aab0 @31f7e86e/31f7e875 -> FUN_31f79c20: "DAMAGE_DATA_NEXT_ULTIMATE" is
	// tried first only when m_bUseUltimateDamageData (forced false above); "DAMAGE_DATA_NEXT" is
	// read only if that fails or was never tried -- unlike DAMAGE_DATA/_ULTIMATE, the base key's
	// own read is skipped on an _ULTIMATE success. With the flag forced false this is exactly
	// March's unconditional "DAMAGE_DATA_NEXT" read below.
	bool bDamageDataNextIsUltimate = false;
	bool bDamageDataNextOk = false;
	if( true == m_bUseUltimateDamageData )
		bDamageDataNextOk = g_pData->GetDamageManager()->SetDamageDataFromLUA( m_DamageEffectDataInLua.m_pDamageDataNext, *pLuaManager, "DAMAGE_DATA_NEXT_ULTIMATE", m_fPowerRate );
	if( true == bDamageDataNextOk )
		bDamageDataNextIsUltimate = true;
	else
		bDamageDataNextOk = g_pData->GetDamageManager()->SetDamageDataFromLUA( m_DamageEffectDataInLua.m_pDamageDataNext, *pLuaManager, "DAMAGE_DATA_NEXT", m_fPowerRate );
	if( false == bDamageDataNextOk )
	{
		SAFE_DELETE( m_DamageEffectDataInLua.m_pDamageDataNext );
	}
	else
	{
		// June2017 FUN_31f79c20 @31f79cef-31f79f9f: once the unnumbered table is read, it loops
		// reading "DAMAGE_DATA_NEXT2", "DAMAGE_DATA_NEXT3", ... (or "..._ULTIMATE2", "..._ULTIMATE3"
		// on the branch above) into new entries of their own, each copied from m_DamageData exactly
		// as the unnumbered entry above is (not chained from the previous entry), stopping at the
		// first table that does not exist and freeing that last, failed entry. Numbering starts at
		// 2: there is no "DAMAGE_DATA_NEXT1" or "..._ULTIMATE1" (confirmed against june_lua's
		// DamageEffect*.src.lua, which has no NEXT1 and does have NEXT2 and up).
		char szDamageDataNextChainKey[64] = "";
		for( int iDamageDataNextChain = 2; ; iDamageDataNextChain++ )
		{
			StringCchPrintfA( szDamageDataNextChainKey, ARRAY_SIZE(szDamageDataNextChainKey), bDamageDataNextIsUltimate ? "DAMAGE_DATA_NEXT_ULTIMATE%d" : "DAMAGE_DATA_NEXT%d", iDamageDataNextChain );
			CX2DamageManager::DamageData* pDamageDataNextChainEntry = new CX2DamageManager::DamageData();
			*pDamageDataNextChainEntry = m_DamageData;
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
			pDamageDataNextChainEntry->m_vecBuffFactorPtr.clear();
#endif //UPGRADE_SKILL_SYSTEM_2013
			if( false == g_pData->GetDamageManager()->SetDamageDataFromLUA( pDamageDataNextChainEntry, *pLuaManager, szDamageDataNextChainKey, m_fPowerRate ) )
			{
				SAFE_DELETE( pDamageDataNextChainEntry );
				break;
			}
			m_DamageEffectDataInLua.m_vecDamageDataNextChain.push_back( pDamageDataNextChainEntry );
		}
	}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	if ( false == g_pData->GetDamageManager()->SetDamageDataFromLUA( m_DamageEffectDataInLua.m_pDamageDataNext, *pLuaManager, "DAMAGE_DATA_NEXT", m_fPowerRate ) )
		SAFE_DELETE( m_DamageEffectDataInLua.m_pDamageDataNext );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(일명 독구름 작업)

#ifdef RAVEN_SECOND_CLASS_CHANGE
	// damage times 변경
	if( pGameUnit != NULL )
	{
		bool bChangeDamageTimes = false;
		PLUA_GET_VALUE( m_pLuaManager, "CHANGE_DAMAGE_TIME",				bChangeDamageTimes,		false );

		int iDamageTimes = pGameUnit->GetDamageTimes();
		if( bChangeDamageTimes == true && iDamageTimes >= 0 )
		{
			m_DamageTime = iDamageTimes;
		}
	}
#endif

	PLUA_GET_VALUE( m_pLuaManager, "APPLY_SCALE_BY_UNIT",					m_bApplyScaleByUnit,	true );
#ifdef INHERIT_FORCEDOWNRATE
	PLUA_GET_VALUE( m_pLuaManager, "INHERIT_FORCEDOWNRATE",					m_bInheritForceDownRate,	false );
#endif INHERIT_FORCEDOWNRATE

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0: INHERIT_CRITICAL_RATE (@31f7ed68, bool getter 32fe9e40, default
	// false) and INHERIT_CRITICAL_DAMAGE_RATE (@31f7ef09, same shape)
	PLUA_GET_VALUE( m_pLuaManager, "INHERIT_CRITICAL_RATE",				m_bInheritCriticalRate,				false );
	PLUA_GET_VALUE( m_pLuaManager, "INHERIT_CRITICAL_DAMAGE_RATE",			m_bInheritCriticalDamageRate,			false );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef FIX_AFTER_IMAGE
	LoadAfterImage();
#endif

#ifdef DAMAGE_EFFECT_GROUP_DAMAGE
	PLUA_GET_VALUE(	m_pLuaManager, "GROUP_DAMAGE",				m_bGroupDamage,				false	);		/// 그룹 데미지 설정 추가
#endif DAMAGE_EFFECT_GROUP_DAMAGE
	

#ifdef ADDED_RELATIONSHIP_SYSTEM
	// 자기 자신 or 인연에게만 적용되는 데미지 이펙트.
	PLUA_GET_VALUE(	m_pLuaManager, "ONLY_RELATIONSHIP",		m_bOnlyRelationShip,		false	);	
#endif // ADDED_RELATIONSHIP_SYSTEM

	

#ifdef NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE
	PLUA_GET_VALUE( m_pLuaManager, "NOTIFY_TO_OWNER_WHEN_DIE",	m_bShouldNotifyToOwnerUnitWhenDie,	false );
#endif // NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE

#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
	/// SetLinkMainEffectByPos() 를 쓰기 위해 필요한 길이 배율
	PLUA_GET_VALUE( m_pLuaManager, "LINK_EFFECT_RATE",			m_fLinkEffectRate,		-1.f );
#endif //SERV_ADD_LUNATIC_PSYKER
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
	// June2017 FUN_31f7aab0 (CEffect ctor) @31f7f612-31f7f62b (effect completion C7): right after LINK_EFFECT_RATE, the
	// create-damage-effect rate (June +0xb18) starts at -1.0 (0x334380f0), then the owner given to the ctor, when there is one,
	// is told about the new effect (vslot 0x328). The base does nothing; class overrides (checklist 7a) seed the rate.
	m_fJuneCreateDamageEffectRate = -1.f;
	if( NULL != pGameUnit )
		pGameUnit->JuneOnDamageEffectCreated( this );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT


#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f7aab0 @31f7f6b3: RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT, float getter
	// 32fea370, destination [ESI+0xb48], default -1.f (memory constant 0x334380f0, confirmed
	// byte-exact against the June binary; the same constant is ATT/DamageEffect_ATT's
	// LIMIT_DISTANCE default, see the Phase 3 hand-verified notes).
	PLUA_GET_VALUE( m_pLuaManager, "RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT",	m_fResetMainEffectLifeTimeRateByHit,	-1.f );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	// JUNE_FX CE_TEMPLET <name> <key>=<v> ..., only when this CEffect uses a non-default
	// Phase-5 key. Mirrors X2EffectSet's ES_TEMPLET line (Phase 2).
	{
		WCHAR wszJuneFx[1024] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX CE_TEMPLET %s", pName );
		const size_t uiCeNameOnly = wcslen( wszJuneFx );
		WCHAR wszCeKey[192] = {0};
		if( true == m_bUseAdvanceCollisionCheck )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" USE_ADVANCE_COLLISIOIN_CHECK=1" );
		if( true == m_bUseOwnerAttackSpeed )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" USE_OWNER_ATTACK_SPEED=1" );
		if( true == m_bIgnoreMIHuge )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" IGNORE_MI_HUGE=1" );
		if( true == m_bInheritCriticalRate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" INHERIT_CRITICAL_RATE=1" );
		if( true == m_bInheritCriticalDamageRate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" INHERIT_CRITICAL_DAMAGE_RATE=1" );
		if( -1.f != m_fResetMainEffectLifeTimeRateByHit )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT=%.3f", m_fResetMainEffectLifeTimeRateByHit );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( true == m_bFollowLineWithCustomRotate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_LINE_WITH_CUSTOM_ROTATE=1" );
		if( true == m_bFollowLineGravity )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_LINE_GRAVITY=1" );
		if( false == m_strTraceOwnerBone.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" TRACE_OWNER_BONE=%s", m_strTraceOwnerBone.c_str() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( false == m_strTraceOwnerBoneOnlyRotate.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" TRACE_OWNER_BONE_ONLY_ROTATE=%s", m_strTraceOwnerBoneOnlyRotate.c_str() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( true == m_bUsedDamageDataUltimate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" DAMAGE_DATA_ULTIMATE=1" );
		if( false == m_strMainEffectNameUltimate.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" MAIN_EFFECT_NAME_ULTIMATE=%s", m_strMainEffectNameUltimate.c_str() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( true == m_bFollowLineUltimate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_LINE_ULTIMATE=1" );
		if( true == m_bFollowLineNewUltimate )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_LINE_NEW_ULTIMATE=1" );
		if( false == m_vecDamageDataChangeTime.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" DAMAGE_DATA_CHANGE_TIME[]=%d", (int)m_vecDamageDataChangeTime.size() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( false == m_vecDamageDataChangeTimeGlobalTime.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME[]=%d", (int)m_vecDamageDataChangeTimeGlobalTime.size() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( false == m_DamageEffectDataInLua.m_vecDamageDataNextChain.empty() )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" DAMAGE_DATA_NEXT_CHAIN[]=%d", (int)m_DamageEffectDataInLua.m_vecDamageDataNextChain.size() );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( 0 != m_iApplyEffectAlphaConditionByOption )
		{
			StringCchPrintfW( wszCeKey, ARRAY_SIZE(wszCeKey), L" APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION=%d", m_iApplyEffectAlphaConditionByOption );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszCeKey );
		}
		if( wcslen( wszJuneFx ) != uiCeNameOnly )
			CX2OfflineLog::Server( L"%s", wszJuneFx );
	}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// Code review fix: these two are ctor-time templet facts (never change after
	// construction), so log them once here instead of every frame in OnFrameMove.
	// June2017 FUN_31f88790 @31f88928-31f88943 (APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION) and
	// @31f889e9-31f88a8b (TRACE_OWNER_BONE_ONLY_ROTATE): see those addresses for what June does
	// at run time. Both are rule-5 unsupported systems here (effect-alpha option; Rose/UT_ROSE).
	if( 0 != m_iApplyEffectAlphaConditionByOption )
		JuneFxCeOnce( L"unsupported", GetName(), L"APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION", L"effect-alpha option absent" );
	if( false == m_strTraceOwnerBoneOnlyRotate.empty() )
		JuneFxCeOnce( L"unsupported", GetName(), L"TRACE_OWNER_BONE_ONLY_ROTATE", L"Rose (UT_ROSE) absent" );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

	m_fElapsedTime = 0.f;
}; 



CX2DamageEffect::CEffect::~CEffect()
{
	KTDXPROFILE();


	//BOOST_TEST_FOREACH( CKTDXDeviceSound*, pSound, m_vecPlayingLoopSound )
	//{
	//	if( NULL != pSound )
	//	{
	//		pSound->Stop();
	//	}
	//}
	//m_vecPlayingLoopSound.clear();


#ifdef CLOSE_SOUND_TEST
	m_SoundCloseManager.CloseSound();
#endif CLOSE_SOUND_TEST



	if( m_hMeshInstHandle != INVALID_MESH_INSTANCE_HANDLE &&
		g_pX2Game != NULL && 
		g_pX2Game->GetMajorXMeshPlayer() )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstHandle );
	}


	for( int i = 0; i < (int)m_StartParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_StartParticleDataList[i];
#ifdef BALANCE_PATCH_20120329
		pParticleData->DestroyParticleSequence();
#endif
		SAFE_DELETE( pParticleData );
	}
	m_StartParticleDataList.clear();

	for( int i = 0; i < (int)m_PassiveParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_PassiveParticleDataList[i];
		pParticleData->DestroyParticleSequence();
		SAFE_DELETE( pParticleData );
	}
	m_PassiveParticleDataList.clear();

	for( int i = 0; i < (int)m_TimeParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_TimeParticleDataList[i];
		pParticleData->DestroyParticleSequence();
		SAFE_DELETE( pParticleData );
	}
	m_TimeParticleDataList.clear();

	m_CameraDataList.clear();
	m_ScreenDataList.clear();
	m_SoundDataList.clear();

#ifdef FIX_AFTER_IMAGE
	if( m_pAfterImage != NULL )
		SAFE_DELETE_KTDGOBJECT( m_pAfterImage );
#endif


	for ( int i = 0; i < (int)m_DamageEffectDataList.size(); i++ )
	{
		DamageEffectData* pDamageEffectData = m_DamageEffectDataList[i];
		SAFE_DELETE( pDamageEffectData );
	}
	m_DamageEffectDataList.clear();


	for ( int i = 0; i < (int)m_vecHitDamageEffect.size(); i++ )
	{
		DamageEffectData* pDamageEffectData = m_vecHitDamageEffect[i];
		SAFE_DELETE( pDamageEffectData );
	}
	m_vecHitDamageEffect.clear();

	for ( int i = 0; i < (int)m_vecDieDamageEffect.size(); i++ )
	{
		DamageEffectData* pDamageEffectData = m_vecDieDamageEffect[i];
		SAFE_DELETE( pDamageEffectData );
	}
	m_vecDieDamageEffect.clear();

#ifdef NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE
	// 데미지 이펙트를 생성한 유닛 객체에게만 불려 짐
	if( true == m_bShouldNotifyToOwnerUnitWhenDie &&
		null != m_optrOwnerUnit )
	{
		m_optrOwnerUnit->DieDamageEffectProc(m_DamageData);
	}
#endif // NOTIFY_TO_OWNER_UNIT_WHEN_DAMAGE_EFFECT_DIE

	if( true == m_BlackHoleData.m_bEnabled && NULL != g_pX2Game )
	{
		for( int i=0; i<g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit(i);
			if( NULL == pUnit )
				continue;

			if( null != m_optrOwnerUnit && pUnit->GetTeam() == m_optrOwnerUnit->GetTeam() )
				continue;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            pUnit->RemoveEffectiveBlackHoleDamageEffect( GetHandle() );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			pUnit->RemoveEffectiveBlackHoleDamageEffect( this );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		}
	}

	//{{ kimhc // 2009-10-20 // 독구름 작업 관련 추가
#ifdef	DAMAGE_EFFECT_WORK
	for ( UINT i = 0; i < m_DamageEffectDataInLua.m_vecHitParticle.size(); i++ )
	{
		ParticleData* pParticleData = m_DamageEffectDataInLua.m_vecHitParticle[i];
		SAFE_DELETE( pParticleData );
	}
	m_DamageEffectDataInLua.m_vecHitParticle.clear();

	for ( UINT i = 0; i < m_DamageEffectDataInLua.m_vecHitDamageEffect.size(); i++ )
	{
		DamageEffectData* pDamageEffectData = m_DamageEffectDataInLua.m_vecHitDamageEffect[i];
		SAFE_DELETE( pDamageEffectData );
	}
	m_DamageEffectDataInLua.m_vecHitDamageEffect.clear();

	for ( UINT i = 0; i < m_DamageEffectDataInLua.m_vecDieDamageEffect.size(); i++ )
	{
		DamageEffectData* pDamageEffectData = m_DamageEffectDataInLua.m_vecDieDamageEffect[i];
		SAFE_DELETE( pDamageEffectData );
	}
	m_DamageEffectDataInLua.m_vecDieDamageEffect.clear();

	SAFE_DELETE( m_DamageEffectDataInLua.m_pDamageDataNext );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	for( size_t iDamageDataNextChain = 0; iDamageDataNextChain < m_DamageEffectDataInLua.m_vecDamageDataNextChain.size(); iDamageDataNextChain++ )
		SAFE_DELETE( m_DamageEffectDataInLua.m_vecDamageDataNextChain[iDamageDataNextChain] );
	m_DamageEffectDataInLua.m_vecDamageDataNextChain.clear();

	// Code review fix: Phase 6b's m_vecTouchLandDamageEffect/m_vecHitEffectSet are
	// vector<T*>, allocated with new in LoadTouchLandDamageEffect/LoadHitEffectSet, and
	// were never freed here (m_vecDieCreateNpc is a value type, so it needs nothing).
	for( size_t iTouchLand = 0; iTouchLand < m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.size(); iTouchLand++ )
		SAFE_DELETE( m_DamageEffectDataInLua.m_vecTouchLandDamageEffect[iTouchLand] );
	m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.clear();

	for( size_t iHitEffectSet = 0; iHitEffectSet < m_DamageEffectDataInLua.m_vecHitEffectSet.size(); iHitEffectSet++ )
		SAFE_DELETE( m_DamageEffectDataInLua.m_vecHitEffectSet[iHitEffectSet] );
	m_DamageEffectDataInLua.m_vecHitEffectSet.clear();
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#endif	DAMAGE_EFFECT_WORK
		//}} kimhc // 2009-10-20 // 독구름 작업 관련 추가

}

void CX2DamageEffect::CEffect::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();



#ifdef STOP_UNIT_STOP_EFFECT_TEST

	m_StopTime.OnFrameMove( fTime, fElapsedTime );
	if( m_StopTime.m_fTimeLeft > 0.f )
	{
		return; 
	}

#endif STOP_UNIT_STOP_EFFECT_TEST



	m_fElapsedTime = fElapsedTime;
    if ( g_pX2Game == NULL )
    {
		SetDieDamageEffect();
		m_hMeshInstHandle = INVALID_MESH_INSTANCE_HANDLE;
		m_bLive			= false;
    }
    else if( g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstHandle ) == false
#ifdef RAVEN_WEAPON_TAKER
		|| g_pX2Game->GetMajorXMeshPlayer()->IsDyingByCrash( m_hMeshInstHandle ) == true
#endif RAVEN_WEAPON_TAKER
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		|| true == JuneEvaluateDieCheckCondition()
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		)
	{
		SetDieDamageEffect();
		g_pX2Game->GetMajorXMeshPlayer()->PlayEnd( m_hMeshInstHandle );
		m_hMeshInstHandle = INVALID_MESH_INSTANCE_HANDLE;
		m_bLive			= false;
	}
	else
	{   
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
		ASSERT( pMeshInst != NULL );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		// June2017 FUN_31f88790 @31f88943-31f889e9: TRACE_OWNER_BONE. Every frame the main effect
		// moves to that bone of the owner. With no owner it uses g_pX2Game->GetMyUnit() instead
		// (CX2Game vtable slot 0x98 = 3231f550). The lookup is GetBonePos( name, out ), the bool
		// form (unit vtable slot 0x80); if the bone is not found, the unit's GetPos( false ) (slot
		// 0x138) is used. June writes the found position straight into the mesh position (+0x3f0)
		// and calls SetPos 31c1a370, which is only that store, for the fallback. Both are SetPos here.
		if( false == m_strTraceOwnerBone.empty() && NULL != pMeshInst )
		{
			CX2GameUnit* pTraceOwnerBoneUnit = ( null != m_optrOwnerUnit ) ? m_optrOwnerUnit.GetObservable() : g_pX2Game->GetMyUnit();
			if( NULL != pTraceOwnerBoneUnit )
			{
				D3DXVECTOR3 vTraceOwnerBonePos( 0.f, 0.f, 0.f );
				if( true == pTraceOwnerBoneUnit->GetBonePos( m_strTraceOwnerBone.c_str(), vTraceOwnerBonePos ) )
				{
					pMeshInst->SetPos( vTraceOwnerBonePos );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxCeFrame( GetName(), L"TRACE_OWNER_BONE", m_strTraceOwnerBone.c_str() );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
				else
				{
					pMeshInst->SetPos( pTraceOwnerBoneUnit->GetPos( false ) );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxCeFrame( GetName(), L"TRACE_OWNER_BONE", L"bone not found, unit GetPos" );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
			}
		}

#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME


		if( m_bUseDXMatrix == true )
		{
			pMeshInst->SetUseDXMatrix( m_DXMATRIX );
		}
		m_vMainEffectAxisAngleDegree = pMeshInst->GetMatrix().GetRotateDegree();
		m_vMainEffectPosition = pMeshInst->GetMatrix().GetPos();


		if( m_bFollowLine == true )
		{
			FollowLine();
		}

		for( int i = 0; i < (int)m_PassiveParticleDataList.size(); i++ )
		{
			ParticleData* pParticleData = m_PassiveParticleDataList[i];		
			SetParticleData( pParticleData );
		}

		for( int i = 0; i < (int)m_TimeParticleDataList.size(); i++ )
		{
			ParticleData* pParticleData = m_TimeParticleDataList[i];

			if( INVALID_PARTICLE_SEQUENCE_HANDLE != pParticleData->m_hSeq )
			{	
				if( g_pX2Game->GetMajorParticle()->IsLiveInstanceHandle( pParticleData->m_hSeq ) == true 
					&& true == pParticleData->bTrace )
				{
					SetParticleData( pParticleData );
				}
				else
				{
					pParticleData->m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
				}
			}
#ifdef DAMAGE_EFFECT_TIMED_PARTICLE_GLOBAL_TIME
			else if( true == pParticleData->bUseGlobalTime )
			{
#ifdef ELSWORD_SHEATH_KNIGHT
				if( pMeshInst->EventTimerGlobal( pParticleData->fEventTime ) == true )
#else
				if(pParticleData->fEventTime < pMeshInst->GetGlobalTime() )
#endif ELSWORD_SHEATH_KNIGHT
				{
					pParticleData->m_hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL, pParticleData->particleName.c_str(), 0,0,0, -1, -1, -1, pParticleData->triggerCount, pParticleData->triggerTime );
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pParticleData->m_hSeq );
					if( NULL != pSeq )
					{
						pSeq->SetLandPosition( m_fLandPos );
					}

					SetParticleData( pParticleData );
				}
			}
			else if( pMeshInst->GetXSkinAnim()->EventTimer( pParticleData->fEventTime ) == true )
#endif DAMAGE_EFFECT_TIMED_PARTICLE_GLOBAL_TIME
			{
				pParticleData->m_hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL, pParticleData->particleName.c_str(), 0,0,0, -1, -1, -1, pParticleData->triggerCount, pParticleData->triggerTime );
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pParticleData->m_hSeq );
				if( NULL != pSeq )
				{
					pSeq->SetLandPosition( m_fLandPos );
				}
				
				SetParticleData( pParticleData );
			}
		}

		if( null != m_optrOwnerUnit )
		{
			for( int i = 0; i < (int)m_CameraDataList.size(); i++ )
			{
				CameraData* pCameraData = &m_CameraDataList[i];	
				if( pMeshInst->GetXSkinAnim()->EventTimer( pCameraData->fEventTime ) == true )
				{
					if( pCameraData->unitSelect == DEUS_FOCUS && m_optrOwnerUnit->IsFocusUnit() != true )
						continue;
					if( pCameraData->unitSelect == DEUS_MY && m_optrOwnerUnit->IsMyUnit() != true )
						continue;

					switch( pCameraData->crashType )
					{
					case CKTDGCamera::DECT_UP:
						g_pX2Game->GetX2Camera()->GetCamera().UpCrashCamera( pCameraData->fSpeed, pCameraData->fAccel );
						break;

					case CKTDGCamera::DECT_DOWN:
						g_pX2Game->GetX2Camera()->GetCamera().DownCrashCamera( pCameraData->fSpeed, pCameraData->fAccel );
						break;

					case CKTDGCamera::DECT_UP_DOWN:
						g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( pCameraData->fGap, pCameraData->fTime );
						break;

					case CKTDGCamera::DECT_UP_DOWN_NO_RESET:
						g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCameraNoReset( pCameraData->fGap, pCameraData->fTime );
						break;


					case CKTDGCamera::DECT_LEFT:
						{
							g_pX2Game->GetX2Camera()->GetCamera().LeftCrashCamera( pCameraData->fSpeed, pCameraData->fAccel );
						} break;
					case CKTDGCamera::DECT_RIGHT:
						{
							g_pX2Game->GetX2Camera()->GetCamera().RightCrashCamera( pCameraData->fSpeed, pCameraData->fAccel );
						} break;
					case CKTDGCamera::DECT_LEFT_RIGHT:
						{
							g_pX2Game->GetX2Camera()->GetCamera().LeftRightCrashCamera( pCameraData->fSpeed, pCameraData->fAccel );
						} break;
					case CKTDGCamera::DECT_LEFT_RIGHT_NO_RESET:
						{
							g_pX2Game->GetX2Camera()->GetCamera().LeftRightCrashCameraNoReset( pCameraData->fSpeed, pCameraData->fAccel );
						} break;


					case CKTDGCamera::DECT_RANDOM:
						{
							g_pX2Game->GetX2Camera()->GetCamera().ShakeRandom( 15, pCameraData->fSpeed*0.8f, pCameraData->fSpeed*1.2f, pCameraData->fGap*0.8f, pCameraData->fGap*1.2f, 
																						15, pCameraData->fSpeed*0.8f, pCameraData->fSpeed*1.2f, pCameraData->fGap*0.8f, pCameraData->fGap*1.2f, 
																						true ); // todo!!!! 스크립트에 입력할 수 있게 해야함
						} break;

					}
					m_CameraDataList.erase( m_CameraDataList.begin() + i );
					i--;
				}
			}

			for( int i = 0; i < (int)m_ScreenDataList.size(); i++ )
			{
				ScreenData* pScreenData = &m_ScreenDataList[i];	
				if( pMeshInst->GetXSkinAnim()->EventTimer( pScreenData->fEventTime ) == true )
				{
					if( pScreenData->unitSelect == DEUS_FOCUS && m_optrOwnerUnit->IsFocusUnit() != true )
						continue;
					if( pScreenData->unitSelect == DEUS_MY && m_optrOwnerUnit->IsMyUnit() != true )
						continue;

					g_pKTDXApp->GetDGManager()->ClearScreen( pScreenData->clearCount, pScreenData->color );

					m_ScreenDataList.erase( m_ScreenDataList.begin() + i );
					i--;
				}
			}
		}

		
		OnFrameMove_SoundData();

		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
		// 현재 열려있는 damage effect 테이블을 잠시 닫고 
		m_pLuaManager->EndTable();
#endif	DAMAGE_EFFECT_WORK		
		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
		
		for( int i = 0; i < (int)m_DamageEffectDataList.size(); i++ )
		{
			DamageEffectData* pData = m_DamageEffectDataList[i];

		#ifdef LOOP_CREATE_DAMAGE_EFFECT // 김태환
			if ( NULL == pData )
				continue;

			/// 반복 종료 시간을 넘겼다면, 해당 정보는 제거
			if ( pMeshInst->GetNowLifeTime() >= pData->fLoopEndTime )
			{
				SAFE_DELETE( pData );
				m_DamageEffectDataList.erase( m_DamageEffectDataList.begin() + i );
				i--;

				continue;
			}

			/// 반복 생성을 고려한 쿨타임 연산 = 생성 시간 + ( 반복 간격 * ( 생성된 횟수 ) )
			const float fCheckEventTime = pData->fEventTime + ( pData->fLoopInterval * static_cast<float>( pData->uiCreateCount ) );

			if( pMeshInst->EventTimerGlobal( fCheckEventTime ) == true )
		#else //LOOP_CREATE_DAMAGE_EFFECT
		#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			//////////////////////////////////////////////////////////////////////////
			// Author: Iruha
			// Date: 2026-09-27
			// Description: June2017 CREATE_DAMAGE_EFFECT loop scheduling. LOOP_END_TIME expiry
			// (FUN_31f88790 @31f8932f-31f89342: compares the main mesh's GetNowLifeTime() to
			// fLoopEndTime) and the per-spawn check time fEventTime + fLoopInterval*uiCreateCount
			// (@31f893c3-31f893f7) are byte-identical to the studio's own dead LOOP_CREATE_DAMAGE_EFFECT
			// code above, which is never defined under US_SERVICE (LoadDamageEffect's parse comment).
			if ( pMeshInst->GetNowLifeTime() >= pData->fLoopEndTime )
			{
				SAFE_DELETE( pData );
				m_DamageEffectDataList.erase( m_DamageEffectDataList.begin() + i );
				i--;
				continue;
			}
			const float fCheckEventTime = pData->fEventTime + ( pData->fLoopInterval * static_cast<float>( pData->uiCreateCount ) );
			if( pMeshInst->EventTimerGlobal( fCheckEventTime ) == true )
		#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			if( pMeshInst->EventTimerGlobal( pData->fEventTime ) == true )
		#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		#endif //LOOP_CREATE_DAMAGE_EFFECT
			{
				if( null != m_optrOwnerUnit )
				{

					D3DXVECTOR3 vAxisAngleDegree( 0, 0, 0);
					D3DXVECTOR3 vRotateAngleDegree( 0, 0, 0);
					D3DXVECTOR3 vPosition(0,0,0);


					switch( pData->posType )
					{
					case DEPT_BONE:
						{
							if(pData->bApplyRotateOrigin == true)
							{
								vAxisAngleDegree = m_AngleDegree;
								vRotateAngleDegree = vAxisAngleDegree;
							}
							else if( pData->bApplyRotate == true )
							{
								vAxisAngleDegree = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
								vRotateAngleDegree = vAxisAngleDegree;
							}
		#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
							else if( pData->bApplyMatrix == true )
							{
								D3DXMATRIX* pCombineMatrix = &(pMeshInst->GetXSkinAnim()->GetCloneFrame(pData->boneName.c_str())->combineMatrix);
								if( pCombineMatrix != NULL )
								{
		#ifdef DAMAGE_EFFECT_BUG_FIX
									D3DXVECTOR3 vRot = GetDecomposeAngle( pCombineMatrix, GetDecomposeScale(pCombineMatrix) );
		#else
									D3DXVECTOR3 vRot = GetDecomposeAngle( pCombineMatrix );
		#endif
									vRot.x = D3DXToDegree(vRot.x);
									vRot.y = D3DXToDegree(vRot.y);
									vRot.z = D3DXToDegree(vRot.z);
									vAxisAngleDegree = vRot;
									vRotateAngleDegree = vAxisAngleDegree;
								}
							}
		#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN
							else
							{
								vAxisAngleDegree = pData->vRotateAngleDegree;
								vRotateAngleDegree = vAxisAngleDegree;
							}
							vPosition = pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() ); 
						}
						break;

					case DEPT_UNIT_BONE:
						{
							if( null != m_optrOwnerUnit )
							{
								vAxisAngleDegree = m_optrOwnerUnit->GetRotateDegree();

								if(pData->bApplyRotateOrigin == true)
								{
									vAxisAngleDegree = m_AngleDegree;
									vRotateAngleDegree = vAxisAngleDegree;
								}
								else if( pData->bApplyRotate == true )
								{
									vRotateAngleDegree = vAxisAngleDegree;
								}

								vPosition = m_optrOwnerUnit->GetBonePos( pData->boneName.c_str() );
							}
						}
						break;

					case DEPT_IMPACT_POINT:
						{
							vPosition = m_DamageData.impactPoint;
						}
						break;
		#ifdef RAVEN_WEAPON_TAKER
					case DEPT_DIE_POS:
						{
							vPosition = m_vLandPositionOnLineMap;
						}
						break;
		#endif RAVEN_WEAPON_TAKER

					case DEPT_POS:
						{
							if(pData->bApplyRotateOrigin == true)
							{
								vAxisAngleDegree = m_AngleDegree;
								vRotateAngleDegree = vAxisAngleDegree;
							}
							else if( pData->bApplyRotate == true )
							{
								vAxisAngleDegree = pMeshInst->GetMatrix().GetRotateDegree();
								vRotateAngleDegree = vAxisAngleDegree;

		#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
								if( pData->bApplyRotatedOffset == true )
								{
									vRotateAngleDegree = vRotateAngleDegree + pData->vRotateAngleDegree;
								}
		#endif
							}
							else
							{
								vAxisAngleDegree = pData->vRotateAngleDegree;
								vRotateAngleDegree = vAxisAngleDegree;
							}

							vPosition = pMeshInst->GetMatrix().GetPos();
						}
						break;
					}                   
					//{{ oasis907 : 김상윤 [2010.2.10] // 
					if(pData->bApplyRotatedOffset == true)
					{
						D3DXVECTOR3 vDirVector = pMeshInst->GetDirVec();

						if( 0.f != vDirVector.y )
						{
							vDirVector.y = 0.f;
							D3DXVec3Normalize( &vDirVector, &vDirVector );
						}
						if( 0.f != pData->vOffset.x )
						{
							vPosition += vDirVector * pData->vOffset.x;
						}
						if( 0.f != pData->vOffset.y )
						{
							vPosition.y += pData->vOffset.y;
						}
						if( 0.f != pData->vOffset.z )
						{
							if( vDirVector.y != 0.f )
							{
								vDirVector.y = vDirVector.z;
								vDirVector.z = vDirVector.x;
								vDirVector.x = -vDirVector.y;
								vDirVector.y = 0.f;
								D3DXVec3Normalize( &vDirVector, &vDirVector );
							}
							else
							{
								vDirVector.y = vDirVector.z;
								vDirVector.z = vDirVector.x;
								vDirVector.x = -vDirVector.y;
								vDirVector.y = 0.f;
							}
							vPosition += vDirVector * pData->vOffset.z;
						}
					}
					//}} oasis907 : 김상윤 [2010.2.10] // 
					else
					{
						vPosition += pData->vOffset;
					}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
					// June2017 (inlined loop) @31f89b9d-31f89c1f: LOOP_ADD_POS_X/Y/Z steps world-space,
					// cumulative through the spawn counter (count * addPos, computed fresh from the base
					// position every spawn rather than an accumulated running sum), added after the
					// rotated-offset/plain-offset composition above and before CreateInstance.
					vPosition += pData->vLoopAddPos * static_cast<float>( pData->uiCreateCount );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

					if( m_pManager != NULL )
					{
		#ifdef INHERIT_FORCEDOWNRATE
						float fForceDownRate = 1.f;
						if(true == m_bInheritForceDownRate)
						{
							fForceDownRate *= m_fForceDownRate;
						}
						CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
							vPosition, vRotateAngleDegree, vAxisAngleDegree, m_fLandPos, false, -1.f, fForceDownRate);
		#ifdef ADDITIONAL_MEMO

						if ( NULL != pEffect )
						{
							/// 버프팩터를 상속함
							if ( pData->bInheritBuffFactor && !GetDamageData().m_vecBuffFactorPtr.empty() )
								pEffect->GetDamageData().m_vecBuffFactorPtr = GetDamageData().m_vecBuffFactorPtr;

							if( GetLockOnData().m_LockOnType != CX2DamageEffect::LOT_NONE && pEffect->GetLockOnData().m_LockOnType != CX2DamageEffect::LOT_NONE )
							{
								LockOnData& kLockOnData = pEffect->GetLockOnData();
								LOCK_ON_TYPE lockontype = kLockOnData.m_LockOnType;

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
								// June2017 FUN_31f88790 @31f89e2b (effect completion C7): June's LockOnData::operator= (31f8c280), which leaves the
								// child's pre-lock-on hit set alone instead of handing it the parent's.
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
								if( false == GetLockOnData().m_setPreLockOnTarget.empty() )
									JuneFxHit( GetName(), L"CREATE_LOCKON_COPY", L"%s parentPreLockOn=%d childKept=%d", pData->damageEffectName.c_str(),
										(int)GetLockOnData().m_setPreLockOnTarget.size(), (int)kLockOnData.m_setPreLockOnTarget.size() );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
								JuneAssignLockOnData( kLockOnData, GetLockOnData() );
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
								kLockOnData = GetLockOnData();
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
								kLockOnData.m_LockOnType = lockontype;
							}								
		#endif
							
		#ifdef ADD_GAME_STAGE_DELETE_DAMAGEEFFECT
							if( pData->bAutoDie )
							{
		#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                                g_pX2Game->AddRemoveDamageEffect( pEffect->GetHandle() );
		#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
								g_pX2Game->AddRemoveDamageEffect( pEffect );
		#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
							}
		#endif //ADD_GAME_STAGE_DELETE_DAMAGEEFFECT

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
							//////////////////////////////////////////////////////////////////////////
							// Author: Iruha
							// Date: 2026-09-27
							// Description: June2017 CREATE_DAMAGE_EFFECT loop, post-creation keys.
							// SET_DIE_WHEN_MASTER_EFFECT_DIE @31f89e77-31f89e95: register the child's handle
							// on the parent (June CEffect+0x8d4); CX2DamageEffect::DestroyInstanceHandle
							// destroys them with the parent (June 31f886c0).
							// INHERIT_LIFE_TIME @31f89dc1-31f89dfc: copy the parent main effect's
							// GetNowLifeTime() onto the child main effect, so it continues the parent's life
							// clock instead of starting at 0.
							// INHERIT_DAMAGE @31f89cf5-31f89cfb: propagate the flag onto the child. Its readers
							// are not DamageReact/AttackResult (Phase 7d): FUN_31f8fdb0 @31f8feb1-31f8fecc copies
							// the damage below, and SetDamageDataNext 31f799c0 @31f79b54-31f79b75 carries it on.
							if( true == pData->bSetDieWhenMasterEffectDie )
								m_vecDieWithMasterChildEffects.push_back( pEffect->GetHandle() );

							if( true == pData->bInheritLifeTime )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pChildMainEffect = pEffect->GetMainEffect();
								if( NULL != pChildMainEffect )
									pChildMainEffect->SetNowLifeTime( pMeshInst->GetNowLifeTime() );
							}

							if( true == pData->bInheritDamage )
							{
								pEffect->SetInheritDamageFromMaster( true );
								// June2017 FUN_31f8fdb0 @31f8feb1-31f8fecc (Phase 7d; called @31f89d8b, after the flag
								// is set): the child takes the parent's damage in place of its own -- DamageData +0x38
								// PHYSIC / +0x3c MAGIC (SetDamageDataFromLUA 32632390@326357ec / @3263588d), two floats.
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
								// Effect completion C6b: this copy is now JuneInheritFromParent's b66 branch (31f8fdb0 @31f8feb1), called below
								// once the flag is set, so it is not applied twice.
#else SERV_IRUHADEV_JUNE_SPAWN_INHERIT
								pEffect->GetDamageData().damage.fPhysic	= m_DamageData.damage.fPhysic;
								pEffect->GetDamageData().damage.fMagic	= m_DamageData.damage.fMagic;
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
								JuneFxHit( GetName(), L"INHERIT_DAMAGE_SPAWN", L"%s physic=%.3f magic=%.3f",
									pData->damageEffectName.c_str(), m_DamageData.damage.fPhysic, m_DamageData.damage.fMagic );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
							}
							// June2017 FUN_31f88790 @31f89d52-31f89d82 (effect completion C7): the CREATE_DAMAGE_EFFECT loop inlines 31f8af50's two adds
							// (no CALL, so the C6a xref scan missed them), after SetScale and before 31f8fdb0: INHERIT_CRITICAL_RATE /
							// INHERIT_CRITICAL_DAMAGE_RATE add the parent's CRITICAL_RATE / APPLY_CRITICAL_DAMAGE_PERCENT onto the child's.
							if( true == m_bInheritCriticalRate )
								pEffect->GetDamageData().m_fCriticalRate += m_DamageData.m_fCriticalRate;
							if( true == m_bInheritCriticalDamageRate )
								pEffect->GetDamageData().m_fApplyCriticalDamage += m_DamageData.m_fApplyCriticalDamage;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							if( true == m_bInheritCriticalRate || true == m_bInheritCriticalDamageRate )
								JuneFxHit( GetName(), L"CREATE_DAMAGE_EFFECT_INHERIT", L"%s critRate=%d->%.3f critDamage=%d->%.3f",
									pData->damageEffectName.c_str(), (int)m_bInheritCriticalRate, pEffect->GetDamageData().m_fCriticalRate,
									(int)m_bInheritCriticalDamageRate, pEffect->GetDamageData().m_fApplyCriticalDamage );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
							// June2017 FUN_31f88790 @31f89d8b: 31f8fdb0 on every CREATE_DAMAGE_EFFECT child, unconditionally, once the INHERIT_DAMAGE flag
							// (@31f89cfb) is set. June calls it ahead of INHERIT_BUFF_FACTOR; the fields it writes are independent of that block.
							JuneInheritFromParent( pEffect );
							// @31f89dbc: 31f64c00 right after INHERIT_BUFF_FACTOR; INHERIT_LIFE_TIME in between only sets the main-effect clock.
							JuneCopyAttackTriggerToChild( pEffect );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							CX2OfflineLog::Server( L"JUNE_FX CE_LOOP %s CREATE_DAMAGE_EFFECT %s spawn=%d/%d pos=(%.3f,%.3f,%.3f) addPos=(%.3f,%.3f,%.3f) dieWithMaster=%d inheritLifeTime=%d inheritDamage=%d",
								GetName(), pData->damageEffectName.c_str(), (int)pData->uiCreateCount, (int)pData->uiLoopEndCount,
								vPosition.x, vPosition.y, vPosition.z, pData->vLoopAddPos.x, pData->vLoopAddPos.y, pData->vLoopAddPos.z,
								(int)pData->bSetDieWhenMasterEffectDie, (int)pData->bInheritLifeTime, (int)pData->bInheritDamage );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

		#else //INHERIT_FORCEDOWNRATE
							CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
								vPosition, vRotateAngleDegree, vAxisAngleDegree, 
								m_fLandPos, false );
		#endif INHERIT_FORCEDOWNRATE

		#ifdef INHERIT_FORCEDOWNRATE
							pEffect->SetInheritForceDownRate(m_bInheritForceDownRate);
		#ifdef SERV_RAVEN_VETERAN_COMMANDER
							if( -1 != pData->iGroupID )		/// 그룹 데미지 설정
								pEffect->SetLaserGroupID( pData->iGroupID );
		#endif SERV_RAVEN_VETERAN_COMMANDER

		#endif INHERIT_FORCEDOWNRATE

		#ifdef ADD_RENA_SYSTEM //김창한
							//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
							pEffect->GetDamageData().m_eFirstAttack = m_DamageData.m_eFirstAttack;
							pEffect->GetDamageData().m_RelateSkillData = m_DamageData.m_RelateSkillData;
		#endif //ADD_RENA_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
							// June2017 FUN_31f88790 @31f89ee0-31f89f10: the other three callees at the end of the spawn: 31f649d0 with the entry's +0xb0,
							// then 31f65e40 / 31f65f90 when the entry carries +0xb4 / +0xb5 (31f64c00 ran above, @31f89dbc).
							pEffect->JuneApplyInheritDamageRate( pData->fInheritDamageRate );
							if( true == pData->bInheritIgnoreDefence )
								pEffect->JuneSetIgnoreDefenceDown( m_DamageData.m_bIgnoreDefence );
							if( true == pData->bInheritIgnoreStateDefence )
								pEffect->JuneSetIgnoreStateDefenceDown( m_DamageData.m_bIgnoreStateDefence );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT

		#ifdef LOOP_CREATE_DAMAGE_EFFECT // 김태환
							++pData->uiCreateCount;		/// 생성 횟수 갱신

							/////////////////////////////////////////////////////////////////////////////////////
							/// 아래 구문에서 생성 횟수에 따라 지워버런 후 Continue를 시켜 버리니, 
							/// 기능을 추가하시려면 이 주석 위에 작업해 주세요!
							/////////////////////////////////////////////////////////////////////////////////////

							/// 반복 생성 횟수를 만족하면, 지워 버리자.
							if ( pData->uiLoopEndCount <= pData->uiCreateCount )
							{
								SAFE_DELETE( pData );
								m_DamageEffectDataList.erase( m_DamageEffectDataList.begin() + i );
								i--;

								/// 혹시 이후 코드에서 pData를 쓸 지도 모르니, continue 시켜 버리자.
								continue;
							}

							continue;
		#else //LOOP_CREATE_DAMAGE_EFFECT
		#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
							// June2017 (inlined loop) @31f89f15-31f89f27: increment the spawn counter, then
							// delete this entry once it reaches LOOP_END_COUNT, else keep it for the next spawn.
							++pData->uiCreateCount;
							if ( pData->uiLoopEndCount <= pData->uiCreateCount )
							{
								SAFE_DELETE( pData );
								m_DamageEffectDataList.erase( m_DamageEffectDataList.begin() + i );
								i--;
								continue;
							}
							continue;
		#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		#endif //LOOP_CREATE_DAMAGE_EFFECT
						}

					}

				}

				SAFE_DELETE( pData );
				m_DamageEffectDataList.erase( m_DamageEffectDataList.begin() + i );
				i--;
			}
		}


		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
		m_pLuaManager->BeginTable( GetName() );
#endif	DAMAGE_EFFECT_WORK		
		//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

		if( m_DamageData.bReAttack == true )
		{
			for( CX2DamageManager::HitUnitVector::iterator iter = m_DamageData.hitUnitList.begin(); 
                iter != m_DamageData.hitUnitList.end(); )
			{
				CX2DamageManager::HitUnit* pHitUnit = &*iter;
				pHitUnit->fRemainGap -= fElapsedTime;
				if( pHitUnit->fRemainGap <= 0.0f )
				{
#ifdef  X2OPTIMIZE_STL_CONTAINER_USAGE
                    iter = EraseUnorderlyUsingSwap( m_DamageData.hitUnitList, iter );
#else   X2OPTIMIZE_STL_CONTAINER_USAGE
					iter = m_DamageData.hitUnitList.erase( iter );
#endif  X2OPTIMIZE_STL_CONTAINER_USAGE
				}
                else
                {
                    ++iter;
                }
			}
		}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		// June2017 FUN_31f88790 @31f8a00b-31f8a164: USE_OWNER_ATTACK_SPEED rescales the attack window
		// by the owner's item attack speed, and only once. It runs only while either cached value is
		// still negative (the ctor seeds both with -1). It needs an owner whose m_GameUnitType is
		// GUT_USER (+0x264; the NPC and User ctors store 1 and 0 there @327629bd/32042362). The
		// speed is the XOR-protected user float at +0x485c/+0x4864; June's ResetAnimSpeed 31ffe190
		// reads it where March's calls GetAnimSpeedValue() (X2GUUser.cpp:26639). A speed above 1
		// moves the start earlier to start * ( 2 - f ), clamped at 0. A speed below 1 moves the end
		// later to end * ( 2 - f ), keeping the original end if that came out negative. This was
		// checked by emulating the x87 code on concrete inputs, not only by hand.
		float fJuneAttackTimeStart	= m_AttackTime.x;
		float fJuneAttackTimeEnd	= m_AttackTime.y;
		if( true == m_bUseOwnerAttackSpeed && null != m_optrOwnerUnit
			&& CX2GameUnit::GUT_USER == m_optrOwnerUnit->GetGameUnitType() )
		{
			if( m_fOwnerAttackSpeedAttackTimeStart < 0.f || m_fOwnerAttackSpeedAttackTimeEnd < 0.f )
			{
				const float fOwnerAttackSpeed = m_optrOwnerUnit->GetAnimSpeedValue();
				m_fOwnerAttackSpeedAttackTimeStart = ( fOwnerAttackSpeed > 1.f ) ? ( 1.f - ( fOwnerAttackSpeed - 1.f ) ) * fJuneAttackTimeStart : fJuneAttackTimeStart;
				if( m_fOwnerAttackSpeedAttackTimeStart < 0.f )
					m_fOwnerAttackSpeedAttackTimeStart = 0.f;
				m_fOwnerAttackSpeedAttackTimeEnd = ( fOwnerAttackSpeed < 1.f ) ? ( 1.f - ( fOwnerAttackSpeed - 1.f ) ) * fJuneAttackTimeEnd : fJuneAttackTimeEnd;
				if( m_fOwnerAttackSpeedAttackTimeEnd < 0.f )
					m_fOwnerAttackSpeedAttackTimeEnd = fJuneAttackTimeEnd;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				WCHAR wszJuneFxSpeed[160] = L"";
				StringCchPrintfW( wszJuneFxSpeed, ARRAY_SIZE(wszJuneFxSpeed), L"speed=%.3f window=(%.3f,%.3f)->(%.3f,%.3f)", fOwnerAttackSpeed,
					fJuneAttackTimeStart, fJuneAttackTimeEnd, m_fOwnerAttackSpeedAttackTimeStart, m_fOwnerAttackSpeedAttackTimeEnd );
				JuneFxCeFrame( GetName(), L"USE_OWNER_ATTACK_SPEED", wszJuneFxSpeed );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			}
			fJuneAttackTimeStart	= m_fOwnerAttackSpeedAttackTimeStart;
			fJuneAttackTimeEnd		= m_fOwnerAttackSpeedAttackTimeEnd;
		}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME


#ifdef STICKY_DAMAGE_EFFECT_TEST

		if( true == m_bEnableAttack )
#endif STICKY_DAMAGE_EFFECT_TEST

#ifdef X2OPTIMIZE_DAMAGE_EFFECT_TEST
		if( g_pMain->GetDamageEffectTest() == true )
#endif//X2OPTIMIZE_DAMAGE_EFFECT_TEST
		{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			// June2017 FUN_31f88790 @31f8a166-31f8a201: USE_GLOBAL_TIME, the same test as March's, against
			// the global time (mesh +0x3e0) or the skin-anim time (+0x3c0 -> +0x214). It uses the window
			// USE_OWNER_ATTACK_SPEED may have rescaled just above. June's DamageCheck takes three more
			// arguments (0, -1, -1) than March's; that belongs to Phase 9.
			if ( m_bGlobalTime == true )
			{
				if( fJuneAttackTimeStart <= pMeshInst->GetGlobalTime()
					&& fJuneAttackTimeEnd > pMeshInst->GetGlobalTime() )
				{
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					WCHAR wszJuneFxWindow[128] = L"";
					StringCchPrintfW( wszJuneFxWindow, ARRAY_SIZE(wszJuneFxWindow), L"global window=(%.3f,%.3f) t=%.3f",
						fJuneAttackTimeStart, fJuneAttackTimeEnd, pMeshInst->GetGlobalTime() );
					JuneFxCeFrame( GetName(), L"USE_GLOBAL_TIME", wszJuneFxWindow );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
					g_pData->GetDamageManager()->DamageCheck( &m_DamageData );
				}
			}
			else
			{
				if( fJuneAttackTimeStart <= pMeshInst->GetXSkinAnim()->GetNowAnimationTime()
					&& fJuneAttackTimeEnd > pMeshInst->GetXSkinAnim()->GetNowAnimationTime() )
				{
					g_pData->GetDamageManager()->DamageCheck( &m_DamageData );
				}
			}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			if ( m_bGlobalTime == true )
			{
				if( m_AttackTime.x <= pMeshInst->GetGlobalTime()
					&& m_AttackTime.y > pMeshInst->GetGlobalTime() )
				{
					g_pData->GetDamageManager()->DamageCheck( &m_DamageData );
				}
			}
			else
			{
				if( m_AttackTime.x <= pMeshInst->GetXSkinAnim()->GetNowAnimationTime()
					&& m_AttackTime.y > pMeshInst->GetXSkinAnim()->GetNowAnimationTime() )
				{
					g_pData->GetDamageManager()->DamageCheck( &m_DamageData );
				}
			}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		}

		

		if( g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstHandle ) == false
#ifdef RAVEN_WEAPON_TAKER
			|| g_pX2Game->GetMajorXMeshPlayer()->IsDyingByCrash( m_hMeshInstHandle ) == true
#endif RAVEN_WEAPON_TAKER
			)
		{
			SetDieDamageEffect();
			g_pX2Game->GetMajorXMeshPlayer()->PlayEnd( m_hMeshInstHandle );
			m_hMeshInstHandle = INVALID_MESH_INSTANCE_HANDLE;
			m_bLive			= false;
			return;
		}

#ifdef SERV_IRUHADEV_JUNE_TOUCH_LAND
		// June2017 FUN_31f88790 (CEffect::OnFrameMove) @31f8a2e8-31f8a38b (effect completion C7): right after the die check, the
		// touch-land gate. The main mesh must be bouncing off land this frame: gravity pulls down (m_Gravity.y < 0, mesh +0x47c), it
		// was falling last frame (m_vVelocityBefore.y < 0, +0x550) and rises now (m_vVelocity.y > 0, +0x538); all strict, NaN fails
		// (x87-emulated in Phase 7e, june2017_re/x87test.py). No once-flag: 31f83380 runs on every frame the gate holds.
		// @31f8a2a4-31f8a2e3: June first runs GetLandPosition( m_BeforePos, LINE_RADIUS ) and never reads the result. March's
		// GetLandPosition is const, so the probe changes nothing and is not repeated here.
		if( pMeshInst->GetGravity().y < 0.f && pMeshInst->GetVelocityBefore().y < 0.f && pMeshInst->GetVelocity().y > 0.f )
		{
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxHit( GetName(), L"TOUCH_LAND_GATE", L"entries=%d gravity.y=%.1f velocityBefore.y=%.1f velocity.y=%.1f",
				(int)m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.size(), pMeshInst->GetGravity().y,
				pMeshInst->GetVelocityBefore().y, pMeshInst->GetVelocity().y );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneTouchLandDamageEffect();
		}
#endif SERV_IRUHADEV_JUNE_TOUCH_LAND
		if( pMeshInst->GetXSkinAnim()->EventTimer( m_fDamageDataChangeTime ) == true && m_fDamageDataChangeTime != -1.0f )
		{
			m_fDamageDataChangeTime = -1.0f;

			// kimhc // 2009-10-22 // 데미지 이펙트에 DAMAGE_DATA_NEXT가 나오지 않는 문제 수정(일명 독구름 작업)
			SetDamageDataNext( &m_DamageData );

			if( true == m_bClearHitUnitOnChangeDamageData )
			{
				m_DamageData.hitUnitList.resize(0);
			}
		}
		if(  m_fDamageDataChangeTimeGlobalTime != -1.0f && pMeshInst->EventTimerGlobal( m_fDamageDataChangeTimeGlobalTime ) == true )
		{
			m_fDamageDataChangeTimeGlobalTime = -1.0f;

			// kimhc // 2009-10-22 // 데미지 이펙트에 DAMAGE_DATA_NEXT가 나오지 않는 문제 수정(일명 독구름 작업)
			SetDamageDataNext( &m_DamageData );

			if( true == m_bClearHitUnitOnChangeDamageData )
			{
				m_DamageData.hitUnitList.resize(0);
			}
		}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		// June2017 FUN_31f88790 @31f8a394-31f8a490: in June, DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME) is an
		// array (Phase 5) and both arrays share one counter (+0x8f8). The check runs while the
		// counter is inside both the array and the NEXT chain (+0xd8; index 0 = DAMAGE_DATA_NEXT,
		// 1.. = NEXT2..). The plain array's entry is tested with the skin-anim EventTimer
		// (31d28e10), then the GLOBAL_TIME array's, at the counter as it now stands, with
		// EventTimerGlobal (31c82d60). A hit swaps in that chain entry (31f799c0), advances the
		// counter, and clears the hit list under CLEAR_HIT_UNIT_ON_DAMAGE_DATA_CHANGE (+0x91c).
		// June has no scalar form. March's scalar blocks above stay for the scalar-shaped data this
		// tree ships, and the arrays are empty on that data, so this stays dormant until June's own
		// data is adopted (Phase 11).
		for( int iJuneChangeTimeKind = 0; iJuneChangeTimeKind < 2; iJuneChangeTimeKind++ )
		{
			const vector<float>& vecJuneChangeTime = ( 0 == iJuneChangeTimeKind ) ? m_vecDamageDataChangeTime : m_vecDamageDataChangeTimeGlobalTime;
			if( true == vecJuneChangeTime.empty()
				|| m_iDamageDataChangeTimeIndex >= (int)vecJuneChangeTime.size()
				|| m_iDamageDataChangeTimeIndex >= GetDamageDataNextChainSize() )
				continue;

			const float fJuneChangeTime = vecJuneChangeTime[ m_iDamageDataChangeTimeIndex ];
			const bool bJuneChangeTimeHit = ( 0 == iJuneChangeTimeKind )
				? pMeshInst->GetXSkinAnim()->EventTimer( fJuneChangeTime )
				: pMeshInst->EventTimerGlobal( fJuneChangeTime );
			if( false == bJuneChangeTimeHit )
				continue;

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			WCHAR wszJuneFxChange[96] = L"";
			StringCchPrintfW( wszJuneFxChange, ARRAY_SIZE(wszJuneFxChange), L"%s index=%d time=%.3f",
				( 0 == iJuneChangeTimeKind ? L"anim" : L"global" ), m_iDamageDataChangeTimeIndex, fJuneChangeTime );
			JuneFxCeFrame( GetName(), L"DAMAGE_DATA_CHANGE_TIME", wszJuneFxChange );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			SetDamageDataNextByChainIndex( &m_DamageData, m_iDamageDataChangeTimeIndex );
			m_iDamageDataChangeTimeIndex++;
			if( true == m_bClearHitUnitOnChangeDamageData )
			{
				m_DamageData.hitUnitList.resize(0);
			}
		}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

		if( m_bFirstFrame == true )
		{
			SetStartParticle();
			SetPassiveParticle();
			SetTimeParticle();
			SetCameraData();
			SetScreenData();
			SetSoundData();
			SetDamageEffect();
			m_bFirstFrame = false;
		}

		OnFrameMove_BlackHoleData(fElapsedTime);
		OnFrameMove_LockOnData( fElapsedTime );
		OnFrameMove_SineMoveData(fElapsedTime);

#ifdef FIX_AFTER_IMAGE
		if ( IsSamef(m_afterImageTime.x, -1.f) == false 
			 && IsSamef(m_afterImageTime.y, -1.f) == false )
		{
#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH )
#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
			if( g_pMain->GetGameOption().GetOptionList().m_bEffect == true )
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION			
			{
				if( m_pAfterImage != NULL && GetMainEffect() != NULL && 
					GetMainEffect()->GetXSkinMesh() != NULL &&
					GetMainEffect()->GetXSkinAnim() != NULL && 
					GetMainEffect()->GetState() == CKTDGXMeshPlayer::IS_PLAY )
				{
					m_fDurationTime += fElapsedTime;

					if( m_afterImageTime.x <= GetMainEffect()->GetXSkinAnim()->GetNowAnimationTime()
						&& m_afterImageTime.y > m_fDurationTime )
					{
						m_pAfterImage->Enable();
						m_pAfterImage->OnFrameMove( fTime, m_fElapsedTime );
					}
					else
					{
						m_fDurationTime = 0.f;
					}
				}
			}
		}
		else
		{
			if( m_pAfterImage != NULL )
				m_pAfterImage->Disable();

			m_fDurationTime = 0.f;
		}
#endif


#ifdef STICKY_DAMAGE_EFFECT_TEST
		OnFrameMove_Trace();

#endif STICKY_DAMAGE_EFFECT_TEST


		m_BeforePos = pMeshInst->GetPos();
		
		if( IsSamef(pMeshInst->GetTempletData()->elasticCoeffX.GetRandomNumInRange(), 0.f) == false && m_bRefreshLandPos == true )
		{
			int iLineIndex = -1;
			D3DXVECTOR3 landPosition = g_pX2Game->GetWorld()->GetLineMap()->GetLandPosition( m_BeforePos, LINE_RADIUS, &iLineIndex );
			if( iLineIndex < 0 )
			{
				landPosition = g_pX2Game->GetWorld()->GetLineMap()->GetNearest_LUA( m_BeforePos );
			}
			pMeshInst->SetLandPosition( landPosition.y );
		}

		//if ( pMeshInst->GetIsUsedLand() == true )
		if( pMeshInst->GetIsCrashLand() == true 
			//{{ kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록
#ifdef	EVE_SECOND_CLASS_CHANGE
			 && m_bRefreshLandPos == true
#endif	EVE_SECOND_CLASS_CHANGE
				//}} kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록
		)
		{			
			unsigned char* pLineIndex = NULL;
			D3DXVECTOR3 landPosition = g_pX2Game->GetWorld()->GetLineMap()->GetLandPosition( m_BeforePos, LINE_RADIUS * 3, pLineIndex );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			// June2017 FUN_31f88790 @31f8a95c-31f8aa21: the crash-land REFRESH_LAND_POS update is skipped
			// when the probe comes back with y == -1.0 (0x334380f0; the fucompp/jnp was emulated). The
			// land height and the line-map land position then stay as they were. June's probe (31c7bb00)
			// takes one more leading bool argument (1) than March's GetLandPosition; that is a line-map
			// change and is not ported. The first REFRESH_LAND_POS block above (@31f8a816-31f8a956)
			// matches March and has no such guard.
			if( -1.f != landPosition.y )
			{
				pMeshInst->SetLandPosition( landPosition.y );
#ifdef RAVEN_WEAPON_TAKER
				SetLandPositionOnLineMap( landPosition );
#endif RAVEN_WEAPON_TAKER
			}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			else
			{
				JuneFxCeFrame( GetName(), L"REFRESH_LAND_POS", L"crash-land probe y == -1, skipped" );
			}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			pMeshInst->SetLandPosition( landPosition.y );
#ifdef RAVEN_WEAPON_TAKER
			SetLandPositionOnLineMap( landPosition );
#endif RAVEN_WEAPON_TAKER
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		}
		
	}	
}

void CX2DamageEffect::CEffect::OnFrameMove_SoundData()
{
    if ( g_pX2Game == NULL )
        return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	ASSERT( pMeshInst != NULL );

	if( NULL == pMeshInst )
		return; 


	for( int i = 0; i < (int)m_SoundDataList.size(); i++ )
	{
		SoundData* pSoundData = &m_SoundDataList[i];

		bool bIsRightTimingToPlaySound = false;
		if( pSoundData->bUseGlobalTime == true )
		{
			if( pSoundData->fEventTime < pMeshInst->GetGlobalTime() )
			{
				bIsRightTimingToPlaySound = true;
			}
		}
		else
		{
			if( pMeshInst->GetXSkinAnim()->EventTimer( pSoundData->fEventTime ) == true )
			{
				bIsRightTimingToPlaySound = true;
			}
		}

		if( true == bIsRightTimingToPlaySound )
		{
			if( rand()%100 < pSoundData->iRate )
			{
				CKTDXDeviceSound* pSound = g_pKTDXApp->GetDeviceManager()->OpenSound( pSoundData->fileName.c_str() );
				if( NULL != pSound )
				{


#pragma DMLEE_NOTE( "loop sound 는 채널 문제 수정하고 나서 사용가능" )


					pSound->Set3DPosition( pMeshInst->GetPos() );
					pSound->Play();


// 					if( true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
// 						true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() )
// 					{
// 						pSound->Set3DPosition( pMeshInst->GetPos() );
// 					}



					//if( true == pSoundData->bLoop )
					//{
					//	m_vecPlayingLoopSound.push_back( pSound );
					//}


#ifdef CLOSE_SOUND_TEST
					m_SoundCloseManager.AddSound( pSoundData->fileName );
#endif CLOSE_SOUND_TEST
				}
			}

			m_SoundDataList.erase( m_SoundDataList.begin() + i );
			i--;
		}
	}
}

void CX2DamageEffect::CEffect::OnFrameMove_BlackHoleData( float fElapsedTime )
{
    if ( g_pX2Game == NULL )
        return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	ASSERT( pMeshInst != NULL );

	if( NULL == pMeshInst )
		return; 



	if( true == m_BlackHoleData.m_bEnabled && 
		null != m_optrOwnerUnit )
	{
		D3DXVECTOR3 vPos = pMeshInst->GetPos();

		for( int i=0; i<g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit(i);
			if( NULL == pUnit )
				continue;

			if( pUnit->GetTeam() == m_optrOwnerUnit->GetTeam() )
				continue;

			if( pUnit->GetNowHp() <= 0.f )
				continue;


			if( true == m_BlackHoleData.m_bDragOnlyHittedUnit )
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                if( false == pUnit->IsEffectiveBlackHoleDamageEffect( GetHandle() ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				if( false == pUnit->IsEffectiveBlackHoleDamageEffect( this ) )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				{
					continue;
				}
			}


			// 그외 블랙홀에 빨려들지 않을 다른 조건 검사 
			switch( pUnit->GetGameUnitType() )
			{
			case CX2GameUnit::GUT_USER:
				{
					CX2GUUser* pUser = (CX2GUUser*) pUnit;

					if( pUser->GetSyncData().nowState == pUser->GetDamageDownLandFront() ||
						pUser->GetSyncData().nowState == pUser->GetDamageDownLandBack() ||
						pUser->GetSyncData().nowState == pUser->GetDamageAirDownLanding() )
					{
						continue;
					}

				} break;

			case CX2GameUnit::GUT_NPC:
				{
					CX2GUNPC* pNPC = (CX2GUNPC*) pUnit;

					if( true == pNPC->GetNeverMove() )
						continue;

					if( false == pNPC->GetIsFallDownMonster() )		
						continue;

					if( true == pNPC->GetSiegeMode() )
						continue;

					if( pNPC->GetNowStateID() == pNPC->GetDownDamageLandFront() ||
						pNPC->GetNowStateID() == pNPC->GetDownDamageLandBack() )
					{
						continue;
					}

					switch( pNPC->GetNPCTemplet().m_ClassType )
					{
					case CX2UnitManager::NCT_THING_GATE:
					case CX2UnitManager::NCT_THING_BOX:			
					case CX2UnitManager::NCT_THING_TRAP:		
					case CX2UnitManager::NCT_THING_DEVICE:		
					case CX2UnitManager::NCT_THING_HOUSE:		
					case CX2UnitManager::NCT_THING_WORLD_BLOCK:
					case CX2UnitManager::NCT_THING_CHECKER:
						//{{ JHKang / 강정훈 / 2010/12/07 / 버프 및 디버프 받지 않는 NPC TYPE
#ifdef NEW_SKILL_2010_11
					case CX2UnitManager::NCT_THING_NOBUFF:		/// 버프/디버프에 영향을 받지 않음()
#endif NEW_SKILL_2010_11
						//}} JHKang / 강정훈 / 2010/12/07 / 버프 및 디버프 받지 않는 NPC TYPE
						continue;
					}
#ifdef SERV_EVENT_VALENTINE_DUNGEON
					switch ( pNPC->GetNPCTemplet().m_nNPCUnitID )
					{
					case CX2UnitManager::NUI_VALENTINE_POISON_CUP_CAKE:
						continue;
					}
#endif //SERV_EVENT_VALENTINE_DUNGEON
#ifdef SERV_EVENT_VALENTINE_DUNGEON_INT
					switch( pNPC->GetNPCTemplet().m_nNPCUnitID )
					{
					case CX2UnitManager::NUI_EVENT_VALENTINEGIFT_BOX:
						continue;
					}
#endif SERV_EVENT_VALENTINE_DUNGEON_INT
				} break;
			}


			D3DXVECTOR3 vDir = vPos - pUnit->GetPos( true );
			float fDist = D3DXVec3Length( &vDir );
			if( fDist > m_BlackHoleData.m_fDraggingRange )
			{
				if( CX2GameUnit::GUT_USER == pUnit->GetGameUnitType() )
				{
					CX2GUUser* pUser = (CX2GUUser*) pUnit;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                    pUser->RemoveEffectiveBlackHoleDamageEffect( GetHandle() );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					pUser->RemoveEffectiveBlackHoleDamageEffect( this );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				}
				continue;
			}

			//const float MAGIC_ABSOLUTE_BLACKHOLE_RANGE = 200.f; // 이 안에 들어오면 무조건 안 움직이게 잡아버린다.
			//if( fDist < MAGIC_ABSOLUTE_BLACKHOLE_RANGE )
			//{
			//	pUnit->SetFixedUnitPosition( vPos );
			//}
			//else
			{
				D3DXVec3Normalize( &vDir, &vDir );
				ASSERT( m_BlackHoleData.m_fDraggingRange != 0.f );

				float fSpeed = (1.5f - fDist / m_BlackHoleData.m_fDraggingRange);
				if( fSpeed > 1.f )
				{
					fSpeed = 1.f;
				}
				fSpeed *= m_BlackHoleData.m_fDraggingSpeed;
				pUnit->SetDisplaceUnitPosition( vDir * fSpeed * fElapsedTime );
#ifdef ARA_CHARACTER_BASE
				if ( true == m_BlackHoleData.m_bCanPassUnit )
					pUnit->SetCanPassUnit( m_BlackHoleData.m_bCanPassUnit );
#endif
			}


			// 블랙홀에 빨려들어가고 있는 중에 각성이나 필살기를 사용할 수 없다. 
			if( CX2GameUnit::GUT_USER == pUnit->GetGameUnitType() )
			{
				CX2GUUser* pUser = (CX2GUUser*) pUnit;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                pUser->AddEffectiveBlackHoleDamageEffect( GetHandle() );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				pUser->AddEffectiveBlackHoleDamageEffect( this );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			}
		}
	}
}






void CX2DamageEffect::CEffect::OnFrameMove_LockOnData(  float fElapsedTime_ )
{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f83aa0 (CEffect::OnFrameMove_LockOnData) @31f83aa0-31f858b7, re-expressed on
	// March's members; the March body is intact in #else. Offsets and per-case evidence are in
	// june2017_re/funcmap.tsv (rows 31f83aa0.prelude, 31f83aa0.cases, 31f8cc80).
	if ( g_pX2Game == NULL )
		return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
		return;

	// @31f83b70-31f83bcf: TIME_UNLOCK_FROM_START. New: on the frame it runs out, the effect's own
	// position becomes the ONLY_FRONT reference point (June +0x994 = m_vecSaveOwnerPos).
	if ( 0.0f < m_LockOnData.m_fTimeUnlockFromStart )
	{
		m_LockOnData.m_fTimeUnlockFromStart -= fElapsedTime_;
		if ( m_LockOnData.m_fTimeUnlockFromStart <= 0.0f )
			m_LockOnData.m_vecSaveOwnerPos = pMeshInst->GetPos();
		return;
	}

	// @31f83bd4-31f83c07: TIME_UNLOCK_AFTER_START, unchanged.
	if( m_LockOnData.m_fUnLockTime > 0.f )
		m_LockOnData.m_fUnLockTime -= fElapsedTime_;
	if( m_LockOnData.m_fUnLockTime <= 0.f )
		return;

	// @31f83c0d-31f83c26 (new): UNLOCK_START_AFTER_ATTACK. No steering until the effect has hit that
	// many times (June +0x850 = m_DamageTimeNow); then the gate switches itself off.
	if( 0 < m_LockOnData.m_iUnlockStartAfterAttack )
	{
		if( m_DamageTimeNow < m_LockOnData.m_iUnlockStartAfterAttack )
			return;
		m_LockOnData.m_iUnlockStartAfterAttack = -1;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"UNLOCK_START_AFTER_ATTACK", L"released at hit %d", m_DamageTimeNow );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}

	// @31f83c2c-31f83c45 (new): RETARGET_COOLTIME_AFTER_ATTACK's countdown, started by the hit-count
	// re-targets of cases 6 and 8. While it runs no case runs, so the effect keeps its heading.
	if( 0.f < m_LockOnData.m_fRetargetCooltimeNow )
	{
		m_LockOnData.m_fRetargetCooltimeNow -= fElapsedTime_;
		return;
	}

	// @31f83c4a-31f83c92: FUN_31f75e20 removes the mesh when June +0x9d4 is set, which only happens in
	// XS_PVE_GAME (LoadLockOnData @31f7354c). March has no such state: not ported.

	switch( m_LockOnData.m_LockOnType )	// jump table 31f858e4, index = type - 1
	{
	case LOT_POS_BLACK_HOLE:	// @31f83cb6, unchanged
		pMeshInst->SetBlackHolePos( m_LockOnData.m_LockOnPos );
		break;

	case LOT_UID_BLACK_HOLE:	// @31f83ce1, unchanged
		{
			const CX2GameUnit* pTarget = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			if( NULL == pTarget )
				pTarget = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
			if( NULL != pTarget )
			{
				D3DXVECTOR3 pos = pTarget->GetPos();
				pos.y += 100.0f;
				pMeshInst->SetBlackHolePos( pos );
			}
		}
		break;

	case LOT_POS_VECTOR:	// @31f83d7f, unchanged
		VectorLockOn( m_LockOnData.m_LockOnPos );
		break;

	case LOT_UID_VECTOR:	// @31f83d92-31f844a6
		{
			const CX2GameUnit* pTarget = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			const bool bTargetIsUser = ( NULL != pTarget );
			if( NULL == pTarget )
				pTarget = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
			if( NULL == pTarget )
				break;

			const CX2GameUnit* pOwnerUnit = GetOwnerUnit();
			if ( NULL == pOwnerUnit )
				pOwnerUnit = g_pX2Game->GetMyUnit();
			if ( NULL == pOwnerUnit )
				break;

			bool bFoundCollision = false;
			VectorLockOn( JuneGetNearestCollisionPos( pTarget, pOwnerUnit->GetPos(), bFoundCollision ) );

			// @31f8407a-31f840a9: re-target when the target died or -- new -- when it is invincible and
			// NOT_LOCKON_INVINCIBLE_TARGET is set (FUN_31f63d80).
			const bool bDead = !( 0.f < pTarget->GetNowHp() );
			const bool bInvincibleSkip = ( true == m_LockOnData.m_bNotLockonInvincibleTarget && true == pTarget->GetInvincible() );
			if( false == bDead && false == bInvincibleSkip )
				break;
			if( null == m_optrOwnerUnit )
				break;

			if( true == bTargetIsUser )
				m_LockOnData.m_LockOnUnitUID = -1;
			else
				m_LockOnData.m_LockOnNPCUID = -1;

			// @31f840d8-31f8412c / @31f8445b-31f84489: June's seeded pick FUN_322dadd0 replaces March's
			// CX2Game::GetLockOnTarget (different index, owner-range filter). Its out bool is the byte
			// that held the collision-found flag, so a miss writes -1 where that flag points.
			bool bUserUnit = bFoundCollision;
			const UidType uid = JunePickLockOnTargetBySeed( m_LockOnData.m_iRandomSeed, bUserUnit );
			JuneSetLockOnTargetUID( uid, bUserUnit );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxLockOn( GetName(), L"UID_VECTOR_RETARGET", L"reason=%s seed=%d -> uid=%I64d user=%d", ( bDead ? L"dead" : L"invincible" ), m_LockOnData.m_iRandomSeed, uid, ( bUserUnit ? 1 : 0 ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		break;

	case LOT_NEARST_UID_VECTOR:	// @31f84a13-31f84dfa, unchanged: June's FUN_322d10b0 = March GetLockOnNearstTarget
		{
			const CX2GameUnit* pGameUnit = NULL;
			if ( -1 != m_LockOnData.m_LockOnUnitUID )
				pGameUnit = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			else if ( -1 != m_LockOnData.m_LockOnNPCUID )
				pGameUnit = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
			if ( NULL == pGameUnit )
				break;

			const CX2GameUnit* pOwnerUnit = GetOwnerUnit();
			if ( NULL == pOwnerUnit )
				pOwnerUnit = g_pX2Game->GetMyUnit();
			if ( NULL == pOwnerUnit )
				break;

			bool bFoundCollision = false;
			VectorLockOn( JuneGetNearestCollisionPos( pGameUnit, pOwnerUnit->GetPos(), bFoundCollision ) );

			if( 0.f >= pGameUnit->GetNowHp() && null != m_optrOwnerUnit )
			{
				m_LockOnData.m_LockOnUnitUID	= -1;
				m_LockOnData.m_LockOnNPCUID		= -1;

				bool bUserUnit = false;
				const UidType gameUnitUID = g_pX2Game->GetLockOnNearstTarget( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), pOwnerUnit->GetPos(), bUserUnit );
				JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );
			}
		}
		break;

	case LOT_NEARST_UID_VECTOR_IN_RANGE:	// @31f84dfa-31f856fe
		{
			const CX2GameUnit* pTarget = NULL;
			if ( -1 != m_LockOnData.m_LockOnUnitUID )
				pTarget = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			else if ( -1 != m_LockOnData.m_LockOnNPCUID )
				pTarget = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );

			// New: the ranges are DUNGEON_/PVP_TARGET_RANGE_MIN_MAX (their defaults are March's hardcoded
			// 150/750 and 150/300), and every pick is FUN_322d7670, handed the per-effect target list only
			// when NOT_TARGET_PRE_LOCK_ON_TARGET is non-zero (@31f851ed, @31f853ac, @31f85571).
			const D3DXVECTOR2 vRange = JuneGetLockOnTargetRange();
			D3DXVECTOR2 vRangeScaled = vRange;
			if( m_LockOnData.m_fRange > 0.f )
				vRangeScaled = vRange * m_LockOnData.m_fRange;
			const bool bUseTargetList = ( NTPLOTT_NONE != m_LockOnData.m_iNotTargetPreLockOnTarget );

			if ( NULL != pTarget )
			{
				if( LOCSD_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop )	// @31f84e6f-31f84ec9, unchanged
				{
					pMeshInst->SetDirSpeed( m_LockOnData.m_fSaveDirSpeed );
					m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_NOT_SAVE_DIR_SPEED;
				}

				const CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();
				if ( NULL == pOwnerUnit )
					break;

				bool bFoundCollision = false;
				const D3DXVECTOR3 vPos = JuneGetNearestCollisionPos( pTarget, pOwnerUnit->GetPos(), bFoundCollision );
				VectorLockOn( vPos );

				// @31f85175-31f852a2: the hit-count re-target, unscaled range as in March. New: it needs a
				// live owner, and it starts the RETARGET_COOLTIME_AFTER_ATTACK countdown.
				if ( m_DamageTimeNow > m_PrevDamageTime && null != m_optrOwnerUnit )
				{
					m_PrevDamageTime = m_DamageTimeNow;
					m_LockOnData.m_fRetargetCooltimeNow = m_LockOnData.m_fRetargetCooltimeAfterAttack;

					bool bUserUnit = false;
					const UidType gameUnitUID = JuneGetLockOnNearstTargetInRange( m_optrOwnerUnit->GetTeam(), vPos, vRange, bUseTargetList, bUserUnit );
					JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );
				}

				// @31f852a2-31f85444: the target died -- re-target around the main effect.
				if( 0.f < pTarget->GetNowHp() || null == m_optrOwnerUnit )
					break;
				m_LockOnData.m_LockOnUnitUID	= -1;
				m_LockOnData.m_LockOnNPCUID		= -1;
				if( NULL == GetMainEffect() )
					break;

				bool bUserUnit = false;
				const UidType gameUnitUID = JuneGetLockOnNearstTargetInRange( m_optrOwnerUnit->GetTeam(), GetMainEffect()->GetPos(), vRangeScaled, bUseTargetList, bUserUnit );
				JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );
			}
			else
			{
				// @31f85444-31f856f9: no target.
				if ( NULL == GetMainEffect() || null == m_optrOwnerUnit
					|| m_DamageTime < m_DamageTimeNow || m_DamageTimeNow < m_PrevDamageTime )
					break;

				bool bUserUnit = false;
				const UidType gameUnitUID = JuneGetLockOnNearstTargetInRange( m_optrOwnerUnit->GetTeam(), GetMainEffect()->GetPos(), vRangeScaled, bUseTargetList, bUserUnit );
				JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );

				if( LOCSD_NOT_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop	// @31f85628-31f85686, unchanged
					&& m_LockOnData.m_LockOnUnitUID < 0 && m_LockOnData.m_LockOnNPCUID < 0 )
				{
					m_LockOnData.m_fSaveDirSpeed = pMeshInst->GetDirSpeed();
					pMeshInst->SetDirSpeed( 0.f );
					m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_SAVE_DIR_SPEED;
				}

				JuneLockOnSteerForward( pMeshInst );	// @31f85688-31f856f9
			}
		}
		break;

#ifdef SERV_9TH_NEW_CHARACTER
	case LOT_UID_BONE_POS:	// @31f856fe-31f858b1, unchanged; the bone name now comes from LOCK_ON_TARGET_BONE_NAME
		{
			CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			if( pCX2GUUser != NULL )
			{
				D3DXVECTOR3 vBonePos = pCX2GUUser->GetBonePos( m_LockOnData.m_wstrTargetBoneName.c_str() );
				if ( D3DXVECTOR3( 0.f, 0.f, 0.f ) == vBonePos )
					vBonePos = pCX2GUUser->GetPos();
				VectorLockOn( vBonePos );
			}
			else
			{
				CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
				if ( NULL != pCX2GUNPC )
				{
					D3DXVECTOR3 vBonePos = pCX2GUNPC->GetBonePos( m_LockOnData.m_wstrTargetBoneName.c_str() );
					if ( D3DXVECTOR3( 0.f, 0.f, 0.f ) == vBonePos )
						vBonePos = pCX2GUNPC->GetPos();
					VectorLockOn( vBonePos );
				}
			}
		}
		break;
#endif SERV_9TH_NEW_CHARACTER

	case LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE:	// @31f844b1-31f84a0e, June-only
		{
			const CX2GameUnit* pTarget = NULL;
			if ( -1 != m_LockOnData.m_LockOnUnitUID )
				pTarget = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			else if ( -1 != m_LockOnData.m_LockOnNPCUID )
				pTarget = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );

			const D3DXVECTOR2 vRange = JuneGetLockOnTargetRange();	// never scaled by TARGET_RANGE in this case

			if ( NULL != pTarget )
			{
				if( LOCSD_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop )	// @31f84518-31f84570
				{
					pMeshInst->SetDirSpeed( m_LockOnData.m_fSaveDirSpeed );
					m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_NOT_SAVE_DIR_SPEED;
				}

				const CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();
				if ( NULL == pOwnerUnit )
					break;

				// @31f845c7-31f8465c: aim at the target's lock-on point as seen from m_Pos (June +0x100),
				// raised by LOCK_ON_OFFSET_Y_POS.
				D3DXVECTOR3 vAim = JuneGetLockOnAimPos( pTarget, m_Pos );
				vAim.y += m_LockOnData.m_fLockOnOffsetYPos;
				VectorLockOn( vAim );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				JuneFxLockOn( GetName(), L"AFTER_ATTACK_IN_RANGE", L"aim=(%.0f,%.0f,%.0f) hits=%d", vAim.x, vAim.y, vAim.z, m_DamageTimeNow );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

				// @31f84667-31f84728: the hit-count re-target, a random pick around the aim point
				// (FUN_322daa10); starts RETARGET_COOLTIME_AFTER_ATTACK. No owner test here in June.
				if ( m_DamageTimeNow > m_PrevDamageTime )
				{
					m_PrevDamageTime = m_DamageTimeNow;
					m_LockOnData.m_fRetargetCooltimeNow = m_LockOnData.m_fRetargetCooltimeAfterAttack;
					m_LockOnData.m_LockOnUnitUID	= -1;
					m_LockOnData.m_LockOnNPCUID		= -1;

					bool bUserUnit = false;
					const UidType gameUnitUID = JuneGetLockOnRandomTargetInRange( pOwnerUnit->GetTeam(), vAim, vRange, bUserUnit );
					JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );
				}

				// @31f84728-31f847e4: the target died -- re-target around the main effect.
				if( 0.f < pTarget->GetNowHp() )
					break;
				m_LockOnData.m_LockOnUnitUID	= -1;
				m_LockOnData.m_LockOnNPCUID		= -1;
				if( NULL == GetMainEffect() )
					break;

				bool bUserUnit = false;
				const UidType gameUnitUID = JuneGetLockOnRandomTargetInRange( pOwnerUnit->GetTeam(), GetMainEffect()->GetPos(), vRange, bUserUnit );
				JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );
			}
			else
			{
				// @31f847f7-31f84a0e: no target.
				if ( NULL == GetMainEffect() || m_DamageTime < m_DamageTimeNow || m_DamageTimeNow < m_PrevDamageTime )
					break;

				const CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();
				if ( NULL == pOwnerUnit )
					break;

				bool bUserUnit = false;
				const UidType gameUnitUID = JuneGetLockOnRandomTargetInRange( pOwnerUnit->GetTeam(), GetMainEffect()->GetPos(), vRange, bUserUnit );
				JuneSetLockOnTargetUID( gameUnitUID, bUserUnit );

				if( LOCSD_NOT_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop	// @31f848f0-31f8496f
					&& m_LockOnData.m_LockOnUnitUID < 0 && m_LockOnData.m_LockOnNPCUID < 0 )
				{
					m_LockOnData.m_fSaveDirSpeed = pMeshInst->GetDirSpeed();
					pMeshInst->SetDirSpeed( 0.f );
					m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_SAVE_DIR_SPEED;
				}

				JuneLockOnSteerForward( pMeshInst );	// @31f8497c-31f84a0e
			}
		}
		break;
	}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
    if ( g_pX2Game == NULL )
        return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	ASSERT( pMeshInst != NULL );

	if( NULL == pMeshInst )
		return; 

	//{{ kimhc // 2011.1.18 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	// Unlock 시간이 주어졌을 때 Unlock 시간이 경과하지 않았으면 리턴
	if ( 0.0f < m_LockOnData.m_fTimeUnlockFromStart )
	{
		m_LockOnData.m_fTimeUnlockFromStart -= fElapsedTime_;
		return;
	} // if	
#endif	CHUNG_FIRST_CLASS_CHANGE
	//}} kimhc // 2011.1.18 // 청 1차 전직

#ifdef UPGRADE_RAVEN
	if( m_LockOnData.m_fUnLockTime > 0.f )
		m_LockOnData.m_fUnLockTime -= fElapsedTime_;

	if( m_LockOnData.m_fUnLockTime <= 0.f )
		return;
#endif

	//락온 기능
	switch( m_LockOnData.m_LockOnType )
	{
	case LOT_POS_BLACK_HOLE:
		{
			pMeshInst->SetBlackHolePos( m_LockOnData.m_LockOnPos );
		}
		break;

	case LOT_UID_BLACK_HOLE:
		{
			CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			if( pCX2GUUser != NULL )
			{
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 100.0f;
				pMeshInst->SetBlackHolePos( pos );
			}
			else
			{
				CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
				if( pCX2GUNPC != NULL )
				{
					D3DXVECTOR3 pos = pCX2GUNPC->GetPos();
					pos.y += 100.0f;
					pMeshInst->SetBlackHolePos( pos );
				}
			}
		}
		break;

	case LOT_POS_VECTOR:
		{
			VectorLockOn( m_LockOnData.m_LockOnPos );
		}
		break;

	case LOT_UID_VECTOR:
		{
			CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			if( pCX2GUUser != NULL )
			{
				//D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				//pos.y += 100.0f;

				D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );

				/*vPos = pCX2GUUser->GetPos();
				vPos.y += 100.f;*/

				CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();

				const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();

				const CKTDXCollision::CollisionDataListSet& AttackListSet = pCX2GUUser->GetCollisionListSet();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				bool    bInit = false;
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
				{
					BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
					{
						D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
						if ( bInit == false )
						{
							bInit = true;
							vPos = vCollPos;
							continue;
						}
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
						float fDist = GetDistance( vOwnerUnitPos, vCollPos );
						float fOldDist = GetDistance( vOwnerUnitPos, vPos );

						if ( fDist <= fOldDist )
							vPos = vCollPos;
					}
				}
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				if ( bInit == false )
				{
					vPos = pCX2GUUser->GetPos();
					vPos.y += 100.0f;
				}
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				VectorLockOn( vPos );

				if( pCX2GUUser->GetNowHp() <= 0.f && 
					null != m_optrOwnerUnit )
				{
					m_LockOnData.m_LockOnUnitUID = -1;

					bool bUserUnit;
					UidType uid = g_pX2Game->GetLockOnTarget( (CX2Room::TEAM_NUM)m_optrOwnerUnit->GetTeam(), m_LockOnData.m_iRandomSeed, bUserUnit );
					if( bUserUnit == true )
						m_LockOnData.m_LockOnUnitUID = uid;
					else
						m_LockOnData.m_LockOnNPCUID = (int) uid;
				}
			}
			else
			{
				CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
				if( pCX2GUNPC != NULL )
				{
					//D3DXVECTOR3 pos = pCX2GUNPC->GetPos();
					//pos.y += 100.0f;
					D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
					
					CX2GameUnit* pOwnerUnit = GetOwnerUnit();
					if ( NULL == pOwnerUnit )
						pOwnerUnit = g_pX2Game->GetMyUnit();

					const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();
					
					const CKTDXCollision::CollisionDataListSet& AttackListSet = pCX2GUNPC->GetCollisionListSet();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
					bool    bInit = false;
#else   X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
					const CKTDXCollision::CollisionDataList* collisionDataList = *AttackListSet.begin();
					CKTDXCollision::CollisionData* collisionData = *collisionDataList->begin();
					vPos = collisionData->GetPointStart();
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX

					BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
					{
						BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
						{
							D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
                            if ( bInit == false )
                            {
                                bInit = true;
                                vPos = vCollPos;
                                continue;
                            }
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
							float fDist = GetDistance( vOwnerUnitPos, vCollPos );
							float fOldDist = GetDistance( vOwnerUnitPos, vPos );

							if ( fDist <= fOldDist )
								vPos = vCollPos;
						}
					}
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
                    if ( bInit == false )
                    {
					    vPos = pCX2GUNPC->GetPos();
					    vPos.y += 100.0f;
                    }
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
					VectorLockOn( vPos );


					if( pCX2GUNPC->GetNowHp() <= 0.f && 
						null != m_optrOwnerUnit )
					{
						m_LockOnData.m_LockOnNPCUID = -1;

						bool bUserUnit;
						UidType uid = g_pX2Game->GetLockOnTarget( (CX2Room::TEAM_NUM)m_optrOwnerUnit->GetTeam(), m_LockOnData.m_iRandomSeed, bUserUnit );
						if( bUserUnit == true )
							m_LockOnData.m_LockOnUnitUID = uid;
						else
							m_LockOnData.m_LockOnNPCUID = (int) uid;
					}
				}
			}
		}
		break;

	case LOT_NEARST_UID_VECTOR:
		{
			const CX2GameUnit* pGameUnit = NULL;

			if ( -1 != m_LockOnData.m_LockOnUnitUID )
			{
				pGameUnit = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			} // if
			else if ( -1 != m_LockOnData.m_LockOnNPCUID )
			{
				pGameUnit = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
			} // else if

			if ( NULL != pGameUnit )
			{
				/*D3DXVECTOR3 vPos = pGameUnit->GetPos();
				vPos.y += 100.0f;*/

				D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );

				CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();

				const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();

				const CKTDXCollision::CollisionDataListSet& AttackListSet = pGameUnit->GetCollisionListSet();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				bool    bInit = false;
#else   X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				const CKTDXCollision::CollisionDataList* collisionDataList = *AttackListSet.begin();
				CKTDXCollision::CollisionData* collisionData = *collisionDataList->begin();
				vPos = collisionData->GetPointStart();
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX

				BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
				{
					BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
					{
						D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
						if ( bInit == false )
						{
							bInit = true;
							vPos = vCollPos;
							continue;
						}
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
						float fDist = GetDistance( vOwnerUnitPos, vCollPos );
						float fOldDist = GetDistance( vOwnerUnitPos, vPos );

						if ( fDist <= fOldDist )
							vPos = vCollPos;
					}
				}
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				if ( bInit == false )
				{
					vPos = pGameUnit->GetPos();
					vPos.y += 100.0f;
				}
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX

				VectorLockOn( vPos );

				// 타겟을 잃었으면
				if( 0.f >= pGameUnit->GetNowHp() && null != m_optrOwnerUnit )
				{
					// 기존의 타켓을 초기화
					m_LockOnData.m_LockOnUnitUID	= -1;
					m_LockOnData.m_LockOnNPCUID		= -1;

					CX2GameUnit* pOwnerUnit = GetOwnerUnit();
					if ( NULL == pOwnerUnit )
						pOwnerUnit = g_pX2Game->GetMyUnit();

					const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();

					bool bUserUnit = false;
					UidType gameUnitUID = 
						g_pX2Game->GetLockOnNearstTarget( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vOwnerUnitPos, bUserUnit );

					if( bUserUnit == true )
						m_LockOnData.m_LockOnUnitUID = gameUnitUID;
					else
						m_LockOnData.m_LockOnNPCUID = static_cast<int>( gameUnitUID );
				} // if
			} // if 
		} break;

	case LOT_NEARST_UID_VECTOR_IN_RANGE:
		{
			const CX2GameUnit* pGameUnit = NULL;

			if ( -1 != m_LockOnData.m_LockOnUnitUID )
			{
				pGameUnit = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );
			} // if
			else if ( -1 != m_LockOnData.m_LockOnNPCUID )
			{
				pGameUnit = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );
			} // else if

			if ( NULL != pGameUnit )
			{

#ifdef BALANCE_PATCH_20131107
				if( LOCSD_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop )
				{// 이제 타겟을 찾았으므로 저장된 dir_speed 값이 있다면 그 값으로 바꿔준다.
					CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
					if( NULL != pMeshInst )
					{
						pMeshInst->SetDirSpeed(m_LockOnData.m_fSaveDirSpeed);
						m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_NOT_SAVE_DIR_SPEED;
					}
				}
#endif //BALANCE_PATCH_20131107

				D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
				
				//vPos = pGameUnit->GetPos();
				//vPos.y += 100.f;

				CX2GameUnit* pOwnerUnit = GetOwnerUnit();
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();

				const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();

				const CKTDXCollision::CollisionDataListSet& AttackListSet = pGameUnit->GetCollisionListSet();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
                bool    bInit = false;
#else   X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				const CKTDXCollision::CollisionDataList* collisionDataList = *AttackListSet.begin();
				CKTDXCollision::CollisionData* collisionData = *collisionDataList->begin();
				vPos = collisionData->GetPointStart();
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX

				BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
				{
					BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
					{
						D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
                        if ( bInit == false )
                        {
                            bInit = true;
                            vPos = vCollPos;
                            continue;
                        }
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX

						float fDist = GetDistance( vOwnerUnitPos, vCollPos );
						float fOldDist = GetDistance( vOwnerUnitPos, vPos );

						if ( fDist <= fOldDist )
							vPos = vCollPos;
					}
				}

				//D3DXVECTOR3 vPos = pGameUnit->GetPos();
				//vPos.y += 100.f;
#ifdef  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
                if ( bInit == false )
                {
					vPos = pGameUnit->GetPos();
					vPos.y += 100.0f;
                }
#endif  X2OPTIMIZE_LOCKONDATA_COLLISION_CHECK_CRASH_BUG_FIX
				VectorLockOn( vPos );

				if ( m_DamageTimeNow > m_PrevDamageTime )
				{
					m_PrevDamageTime = m_DamageTimeNow;

					bool bUserUnit = false;
					UidType gameUnitUID = -1;

					D3DXVECTOR2 vRange = (g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME) ? D3DXVECTOR2( 150.f, 300.f) : D3DXVECTOR2( 150.f, 750.f);
#ifdef BALANCE_DEADLY_CHASER_20130214
					if ( true == m_LockOnData.m_bIsNotTargetPreLockOnTarget )
					{
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit, true );
					}
					else
#endif //BALANCE_DEADLY_CHASER_20130214
					{
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit );
					}

					if( bUserUnit == true )
						m_LockOnData.m_LockOnUnitUID = gameUnitUID;
					else
						m_LockOnData.m_LockOnNPCUID = static_cast<int>( gameUnitUID );
				}

				// 타겟을 잃었으면
				if( 0.f >= pGameUnit->GetNowHp() && null != m_optrOwnerUnit )
				{
					// 기존의 타켓을 초기화
					m_LockOnData.m_LockOnUnitUID	= -1;
					m_LockOnData.m_LockOnNPCUID		= -1;


#ifdef EVE_ELECTRA
					if( GetMainEffect() != NULL)
#endif EVE_ELECTRA
					{
						D3DXVECTOR3 vPos = GetMainEffect()->GetPos();

						bool bUserUnit = false;
						UidType gameUnitUID = -1;

						D3DXVECTOR2 vRange = ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME ? D3DXVECTOR2( 150.f, 300.f) : D3DXVECTOR2( 150.f, 750.f) );

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
						if( m_LockOnData.m_fRange > 0.f )
						{
							vRange = vRange * m_LockOnData.m_fRange;
						}
#endif

#ifdef BALANCE_DEADLY_CHASER_20130214
						if ( true == m_LockOnData.m_bIsNotTargetPreLockOnTarget )
						{
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit, true );
						}
						else
#endif //BALANCE_DEADLY_CHASER_20130214
						{
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit );
						}

						if( bUserUnit == true )
							m_LockOnData.m_LockOnUnitUID = gameUnitUID;
						else
							m_LockOnData.m_LockOnNPCUID = static_cast<int>( gameUnitUID );
					} // if
				}  // if
			} // if
			else
			{
				if ( NULL != GetMainEffect() && 
					m_DamageTime >= m_DamageTimeNow && m_DamageTimeNow >= m_PrevDamageTime
#ifdef SUMMON_NPC_GUIDANCE_MISSILE_CRASH_FIX
					&& null != m_optrOwnerUnit
#endif //SUMMON_NPC_GUIDANCE_MISSILE_CRASH_FIX
					)
				{
					D3DXVECTOR3 vPos = GetMainEffect()->GetPos();

					bool bUserUnit = false;
					UidType gameUnitUID = -1;

					D3DXVECTOR2 vRange = ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME ? D3DXVECTOR2( 150.f, 300.f) : D3DXVECTOR2( 150.f, 750.f) );

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
					if( m_LockOnData.m_fRange > 0.f )
					{
						vRange = vRange * m_LockOnData.m_fRange;
					}
#endif

#ifdef BALANCE_DEADLY_CHASER_20130214
					if ( true == m_LockOnData.m_bIsNotTargetPreLockOnTarget )
					{
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit, true );
					}
					else
#endif //BALANCE_DEADLY_CHASER_20130214
					{
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( m_optrOwnerUnit->GetTeam() ), vPos, vRange, bUserUnit );
					}
					
					if( bUserUnit == true )
						m_LockOnData.m_LockOnUnitUID = gameUnitUID;
					else
						m_LockOnData.m_LockOnNPCUID = static_cast<int>( gameUnitUID );

#ifdef BALANCE_PATCH_20131107
					if( LOCSD_NOT_SAVE_DIR_SPEED == m_LockOnData.m_eNotFindTargetMoveStop )
					{		
						CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
						if( NULL != pMeshInst )
						{
							if( ( m_LockOnData.m_LockOnUnitUID < 0 ) && ( m_LockOnData.m_LockOnNPCUID < 0 ) )
							{//타겟이 없으므로 dir_Speed를 저장하고 dir_Speed값을 0으로 만든다.
								m_LockOnData.m_fSaveDirSpeed = pMeshInst->GetDirSpeed();
								pMeshInst->SetDirSpeed(0.f);
								m_LockOnData.m_eNotFindTargetMoveStop = LOCSD_SAVE_DIR_SPEED;
							}
						}
					}
#endif //BALANCE_PATCH_20131107

				}
			}
		} break;
#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			case LOT_UID_BONE_POS:		/// 록온된 유저의 특정 본을 록온
				{
					CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID );

					if( pCX2GUUser != NULL )
					{
						/// Bone Pos 반환
						D3DXVECTOR3 vBonePos = pCX2GUUser->GetBonePos( m_LockOnData.m_wstrTargetBoneName.c_str() );

						/// Bone Pos가 없다면, 그냥 해댱 유저 위치 설정 하자
						if ( D3DXVECTOR3( 0.f, 0.f, 0.f ) == vBonePos )
							vBonePos = pCX2GUUser->GetPos();

						VectorLockOn( vBonePos );
					}
					else
					{
						CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID );

						if ( NULL != pCX2GUNPC )
						{
							/// Bone Pos 반환
							D3DXVECTOR3 vBonePos = pCX2GUNPC->GetBonePos( m_LockOnData.m_wstrTargetBoneName.c_str() );

							/// Bone Pos가 없다면, 그냥 해댱 NPC 위치 설정 하자
							if ( D3DXVECTOR3( 0.f, 0.f, 0.f ) == vBonePos )
								vBonePos = pCX2GUNPC->GetPos();

							VectorLockOn( vBonePos );
						}
					}
				} break;
#endif //SERV_9TH_NEW_CHARACTER
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
}




void CX2DamageEffect::CEffect::OnFrameMove_SineMoveData( float fElapsedTime )
{
#ifdef _SIN_MOVE_TEST_
	//**

    if ( g_pX2Game == NULL )
        return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	//ASSERT( pMeshInst != NULL );
	if( NULL == pMeshInst )
		return; 

	if(m_SinMoveData.m_bSinMove)
	{
		D3DXVECTOR3 Position;
		Position = pMeshInst->GetPos();

		float sinvalue = sin(m_SinMoveData.m_fCurrentTime/m_SinMoveData.m_fTime*3.141592f*2);

		// subtracting old sin movement from position
		Position.y = Position.y - m_SinMoveData.m_fMove * sinvalue;

		// update time
		m_SinMoveData.m_fCurrentTime = m_SinMoveData.m_fCurrentTime + fElapsedTime;
		if(m_SinMoveData.m_fCurrentTime > m_SinMoveData.m_fTime )
		{
			m_SinMoveData.m_fCurrentTime = m_SinMoveData.m_fCurrentTime - m_SinMoveData.m_fTime;
			if( null != m_optrOwnerUnit )
			{
				float RandomSeed = GetOwnerUnit()->GetRandomFloat();
				m_SinMoveData.m_fTime = (m_SinMoveData.m_fTimeMax-m_SinMoveData.m_fTimeMin)*RandomSeed + m_SinMoveData.m_fTimeMin;
				m_SinMoveData.m_fMove = (m_SinMoveData.m_fMoveMax-m_SinMoveData.m_fMoveMin)*RandomSeed + m_SinMoveData.m_fMoveMin;
			}
			else
			{
				m_SinMoveData.m_fTime = (m_SinMoveData.m_fTimeMin + m_SinMoveData.m_fTimeMax) / 2;
				m_SinMoveData.m_fMove = (m_SinMoveData.m_fMoveMin + m_SinMoveData.m_fMoveMax) / 2;
			}

		}

		// add new sin movement to position
		sinvalue = sin(m_SinMoveData.m_fCurrentTime/m_SinMoveData.m_fTime*3.141592f*2);
		Position.y = Position.y + m_SinMoveData.m_fMove * sinvalue;

		pMeshInst->SetPos(Position);

	}
#endif _SIN_MOVE_TEST_
}



void CX2DamageEffect::CEffect::OnFrameMove_Trace()
{
#ifdef STICKY_DAMAGE_EFFECT_TEST

	if( false == m_bTraceAttackedUnit )
		return; 



	if( NULL == m_pTraceUnit || g_pX2Game == NULL )
		return; 


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
		return; 

	

	if( false == g_pX2Game->IsValidUnit( m_pTraceUnit ) )
	{
		m_pTraceUnit = NULL;

		pMeshInst->SetMaxLifeTime( 0.f );

		return;
	}


	D3DXVECTOR3 vPos = m_pTraceUnit->GetPos();
	vPos += m_vTracePosition.x * m_pTraceUnit->GetDirVector();
	vPos.y += m_vTracePosition.y;
	vPos += m_vTracePosition.z * m_pTraceUnit->GetZVector();

	pMeshInst->SetPos( vPos );

	pMeshInst->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );
	pMeshInst->SetDirSpeed( 0.f );
#endif STICKY_DAMAGE_EFFECT_TEST


}


//
//void CX2DamageEffect::CEffect::OnFrameRender()
//{
//	KTDXPROFILE();
//}




void CX2DamageEffect::CEffect::DamageReact( CX2DamageManager::DamageData* pDamageData )
{
#if defined( _SERVICE_ ) 
	// checked
	ELSWORD_VIRTUALIZER_START
#endif

	CX2GameUnit* pDefenderGameUnit = ( null != pDamageData->optrDefenderGameUnit ? pDamageData->optrDefenderGameUnit.GetObservable() : NULL );

    if ( g_pX2Game != NULL )
    {
	    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pDefenderGameUnit ),  L"RevengeImpact", 
													    pDamageData->impactPoint,
													    50, 50, 1, 1 );

	    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pDefenderGameUnit ),  L"DamageImpact", 
		    pDamageData->impactPoint,
		    100, 200, 2, 10 );
	    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pDefenderGameUnit ),  L"DamageImpactCore", 
		    pDamageData->impactPoint,
		    100, 200, 2, 10 );
	    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pDefenderGameUnit ),  L"DamageImpactSlash", 
		    pDamageData->impactPoint,
		    50, 50, 2, 2 );

	    g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera();
    }

	/*
	if( m_bReflect == true )
	{
		D3DXVECTOR3 angle = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
		angle.z += 180.0f;
		m_pManager->CreateInstance( NULL,  pDamageData->pAttackerUnit, m_Name.c_str(),
														pMeshInst->GetXSkinAnim()->GetMatrix().GetPos(),
														angle, angle, m_fLandPos, true );
	}
	*/

	for( int i = 0; i < (int)m_StartParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_StartParticleDataList[i];
		if( pParticleData->bForceDieHit == true )
		{
            if ( g_pX2Game != NULL )
			    g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
			SAFE_DELETE( pParticleData );
			m_StartParticleDataList.erase( m_StartParticleDataList.begin() + i );
			i--;
		}			
	}

	for( int i = 0; i < (int)m_PassiveParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_PassiveParticleDataList[i];
		if( pParticleData->bForceDieHit == true )
		{
            if ( g_pX2Game != NULL )
			    g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
			SAFE_DELETE( pParticleData );
			m_PassiveParticleDataList.erase( m_PassiveParticleDataList.begin() + i );
			i--;
		}	
	}

	for( int i = 0; i < (int)m_TimeParticleDataList.size(); i++ )
	{
		ParticleData* pParticleData = m_TimeParticleDataList[i];
		if( pParticleData->bForceDieHit == true )
		{
            if ( g_pX2Game != NULL )
			    g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
			SAFE_DELETE( pParticleData );
			m_TimeParticleDataList.erase( m_TimeParticleDataList.begin() + i );
			i--;
		}	
	}

	m_bLive = false;

#if defined( _SERVICE_ ) 
	ELSWORD_VIRTUALIZER_END
#endif
}

void CX2DamageEffect::CEffect::AttackResult()
{
#if defined( _SERVICE_ )
	// checked
	ELSWORD_VIRTUALIZER_START
#endif

	KTDXPROFILE();
    
    if( m_DamageData.reActResult == CX2DamageManager::RT_NO_DAMAGE )
        return;

	CX2GameUnit* pOwnerGameUnit = ( null != m_optrOwnerUnit ? m_optrOwnerUnit.GetObservable() : NULL );
	CX2GameUnit *pDefenderGameUnit = m_DamageData.optrDefenderGameUnit.GetObservable();

    bool bIsValidUnit = false;

    if( NULL != g_pX2Game && NULL != pOwnerGameUnit )
    {
        bIsValidUnit = true;

        if( CX2GameUnit::GUT_NPC == pOwnerGameUnit->GetGameUnitType() )
        {
            CX2GUNPC *pNPC = static_cast<CX2GUNPC*>( pOwnerGameUnit );
            pNPC->SetAttackedUnit();
        }

#ifdef SERV_PET_SYSTEM

		if( NULL != pDefenderGameUnit )
		{
			pOwnerGameUnit->SetGameUnitAttackedByMe( pDefenderGameUnit );
		}
#endif

#ifdef SERV_NEW_DEFENCE_DUNGEON // 적용날짜: 2013-04-14
		/// 일반 공격 타격시 이펙트 셋 생성하는 함수 검사
		pOwnerGameUnit->PlayEffectSetNormalAttackByBuff( m_DamageData.attackType );
#endif // SERV_NEW_DEFENCE_DUNGEON

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		if ( 0.f < m_DamageData.fHitAddHPPer )		/// 공격 성공시, 체력 회복 설정
			pOwnerGameUnit->UpNowHpPerHitOthers( m_DamageData.fHitAddHPPer );
#endif // SERV_ARA_CHANGE_CLASS_SECOND
#ifdef ADD_MEMO_1ST_CLASS //김창한
		if( 0.f < m_DamageData.fHitAddHPbyAttackPower ) /// 공격 성공시, 체력 회복 설정(공격자 물마공 평균에 비례)
			pOwnerGameUnit->UpNowHpPerHitbyAttackPower( m_DamageData.fHitAddHPbyAttackPower );
#endif //ADD_MEMO_1ST_CLASS
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
		// June2017 CEffect::AttackResult 31f85910 @31f85a9b-31f85b36 (effect completion C3), after the two HP terms above (+0x404 @31f85a2f, +0x5a0 @31f85a77):
		// HIT_ADD_MP_PERCENT (+0x410, CEffect +0x610): |x - 0.0| > FLT_EPSILON ([334490f8]) -> owner->322736f0( x, 140.0 [33443958] ).
		if( fabsf( m_DamageData.m_fHitAddMpPercent ) > 1.1920928955078125e-07f )
			pOwnerGameUnit->JuneUpNowMpPerHitOthersPercent( m_DamageData.m_fHitAddMpPercent, 140.f );
		// @31f85b07-31f85b24: an effect whose owner is a user marks the owner's attack as successful unless IGNORE_ATTACK_SUCCESS (+0x628, CEffect
		// +0x828, @31f85b10) is set: 31fcee10( owner, true, true ) = unitCondition.bHit of the FUTURE frame. June then also runs 31fcee40( owner, false )
		// @31f85b2d (a short hit counter at unit +0x80ce, no March member; not ported, see JUNE2017_EFFECT_KEY_MATRIX.md).
		if( CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() && false == m_DamageData.m_bIgnoreAttackSuccess )
			static_cast<CX2GUUser*>( pOwnerGameUnit )->JuneSetAttackSuccess( true, true );
#if defined( SERV_IRUHADEV_JUNE_FX_DEBUG ) && defined( SERV_IRUHADEV_OFFLINE )
		{
			static bool s_bJuneC3AttackSuccessLogged = false;
			if( false == s_bJuneC3AttackSuccessLogged && CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() && false == m_DamageData.m_bIgnoreAttackSuccess )
			{
				s_bJuneC3AttackSuccessLogged = true;
				CX2OfflineLog::Server( L"JUNE_FX C3 ATTACK_SUCCESS first user-owned effect hit %s: owner bHit (future frame) set", m_Name.c_str() );
			}
		}
#endif // SERV_IRUHADEV_JUNE_FX_DEBUG && SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
    }      


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
	if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshInstHandle && g_pX2Game != NULL )
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );


	if( NULL != pDefenderGameUnit )
	{
		if( true == m_BlackHoleData.m_bEnabled && 
			true == m_BlackHoleData.m_bDragOnlyHittedUnit )
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            pDefenderGameUnit->AddEffectiveBlackHoleDamageEffect( GetHandle() );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			pDefenderGameUnit->AddEffectiveBlackHoleDamageEffect( this );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		}


#ifdef STICKY_DAMAGE_EFFECT_TEST
		if( true == m_bTraceAttackedUnit && 
			NULL == m_pTraceUnit &&
			true == m_bEnableAttack )
		{
			m_bEnableAttack = false;

			m_pTraceUnit = pDefenderGameUnit;
			
			if( m_fLifeTimeAfterAttack > 0.f )
			{
				ASSERT( CKTDGXMeshPlayer::LT_TIME == pMeshInst->GetTempletData()->lifeType );
				pMeshInst->SetNowLifeTime( 0.f );
				pMeshInst->SetMaxLifeTime( m_fLifeTimeAfterAttack );

				m_fLifeTimeAfterAttack = -1.f;
			}
		}
#endif STICKY_DAMAGE_EFFECT_TEST


	}	

	switch( m_DamageData.reActResult )
	{
		case CX2DamageManager::RT_NO_DAMAGE:        
			return;
			break;

		case CX2DamageManager::RT_REVENGE:
#ifdef NEW_SKILL_2010_11 // oasis907 : 김상윤 [2010.11.1] // 룬 슬레이어 - 마법저항 수련
		case CX2DamageManager::RT_REFLECT:
#endif NEW_SKILL_2010_11
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                int iReflectCount = 0;
                if ( m_DamageData.hAttackerEffect == GetHandle() )
                    iReflectCount = GetReflectCount();
                else if ( CEffect* pEffect = m_pManager->GetInstance( m_DamageData.hAttackerEffect ) )
                {
                    iReflectCount = pEffect->GetReflectCount();
                }
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				int iReflectCount = ((CEffect*)m_DamageData.pAttackerEffect)->GetReflectCount();
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				
				if ( iReflectCount > 0 && g_pX2Game != NULL )
				{
					g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  L"RevengeImpactSlashCW", 
						m_DamageData.impactPoint, 
						50, 50, 1, 3 );
					g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  L"RevengeImpactSlashCCW", 
						m_DamageData.impactPoint, 
						50, 50, 1, 3 );
					g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  L"RevengeImpact", 
						m_DamageData.impactPoint,
						50, 50, 1, 1 );
					CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
					pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"RevengeImpactTick", 
						m_DamageData.impactPoint,
						200, 200, 2, 20 );

					if( pSeq != NULL )
						pSeq->SetLandPosition( m_fLandPos );
				}				

                if ( g_pX2Game != NULL )
                {
				    g_pX2Game->GetWorld()->SetWorldColor( 0xff000000 );
				    g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );
                }

#ifdef FIX_EFFECT_REFLECT
				if( m_bReflect == true && iReflectCount > 0  )
#else
				if( m_bReflect == true )
#endif
				{
					//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
					m_pLuaManager->EndTable();
#endif	DAMAGE_EFFECT_WORK		
					//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

					m_bLive = false;
#ifdef FIX_EFFECT_REFLECT
					D3DXVECTOR3 angle = D3DXVECTOR3(0.f, 0.f, 0.f);
					if ( NULL != pMeshInst && NULL != pMeshInst->GetXSkinAnim() )
					{
						angle = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();

						angle.z += 180.0f;
						CX2DamageEffect::CEffect* pReflectEffect = m_pManager->CreateInstance( pDefenderGameUnit, m_Name.c_str(), m_fPowerRate,
							pMeshInst->GetXSkinAnim()->GetMatrix().GetPos(),
							angle, angle, m_fLandPos );

						//{{ 09.06.10 태완 : 반사시 3D Plain이고 SetRotate = false인 Passive 파티클 뒤집히게
						if( NULL != pReflectEffect )
						{
							pReflectEffect->SetReflected(true);
							pReflectEffect->SetReflectCount( --iReflectCount );
						}						
					}
#else
					D3DXVECTOR3 angle = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
					angle.z += 180.0f;
					CX2DamageEffect::CEffect* pReflectEffect = m_pManager->CreateInstance( pDefenderGameUnit, m_Name.c_str(), m_fPowerRate,
						pMeshInst->GetXSkinAnim()->GetMatrix().GetPos(),
						angle, angle, m_fLandPos );

					//{{ 09.06.10 태완 : 반사시 3D Plain이고 SetRotate = false인 Passive 파티클 뒤집히게
					if( NULL != pReflectEffect )
					{
						pReflectEffect->SetReflected(true);
					}
					//}}
#endif

					//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
					m_pLuaManager->BeginTable( m_Name.c_str() );
#endif	DAMAGE_EFFECT_WORK		
					//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

					return;
				}
			}
			break;

		case CX2DamageManager::RT_CREATE_DAMAGE_EFFECT:
			{
				if( NULL != pOwnerGameUnit )
				{
					if( NULL != pMeshInst )
					{
						D3DXVECTOR3 angle = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
#ifdef ADD_RENA_SYSTEM //김창한
						//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
						CX2DamageEffect::CEffect* pDamageEffect = m_pManager->CreateInstance( pOwnerGameUnit,m_DamageData.m_wstrCreateDamageEffectName.c_str(), m_fPowerRate,
							m_DamageData.impactPoint, angle, angle, m_fLandPos );

						if( NULL != pDamageEffect )
						{
							pDamageEffect->GetDamageData().m_eFirstAttack = m_DamageData.m_eFirstAttack;
							pDamageEffect->GetDamageData().m_RelateSkillData = m_DamageData.m_RelateSkillData;
						}
#else //ADD_RENA_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
						// June2017 FUN_31f85910 @31f86140: RT_CREATE_DAMAGE_EFFECT runs 31f8fdb0 on the new child (the other four callees are
						// not called here). March threw the pointer away on this branch, so the ADD_RENA_SYSTEM branch above is not the live one.
						CX2DamageEffect::CEffect* pJuneCreatedEffect = m_pManager->CreateInstance( pOwnerGameUnit,m_DamageData.m_wstrCreateDamageEffectName.c_str(), m_fPowerRate,
							m_DamageData.impactPoint, angle, angle, m_fLandPos );
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
						// June2017 CEffect::AttackResult 31f85910 @31f86117-31f86138 (effect completion C3): every non-null RT_CREATE_DAMAGE_EFFECT child gets
						// SetScale( m_DamageData +0x498 = CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z, default 1,1,1 ) via 31f6ff60 -> 31f6fdc0, before 31f8fdb0 @31f86140.
						// June multiplies the main-effect part by a per-instance base (+0xb24, (1,1,1) from the ctor unless DamageData +0x5d0 differs); March's
						// SetScale is absolute, which equals June's for that default base, and the hit / die spawners already call it with (1,1,1) defaults.
						if( NULL != pJuneCreatedEffect )
							pJuneCreatedEffect->SetScale( m_DamageData.m_vCreateDamageEffectNameScale );
#if defined( SERV_IRUHADEV_JUNE_FX_DEBUG ) && defined( SERV_IRUHADEV_OFFLINE )
						{
							static bool s_bJuneC3NameScaleLogged = false;
							if( false == s_bJuneC3NameScaleLogged && NULL != pJuneCreatedEffect )
							{
								s_bJuneC3NameScaleLogged = true;
								CX2OfflineLog::Server( L"JUNE_FX C3 NAME_SCALE first RT_CREATE_DAMAGE_EFFECT child of %s: SetScale( %.3f, %.3f, %.3f )", m_Name.c_str(),
									m_DamageData.m_vCreateDamageEffectNameScale.x, m_DamageData.m_vCreateDamageEffectNameScale.y, m_DamageData.m_vCreateDamageEffectNameScale.z );
							}
						}
#endif // SERV_IRUHADEV_JUNE_FX_DEBUG && SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
						JuneInheritFromParent( pJuneCreatedEffect );
						// @31f86148 (effect completion C7): then 31f6ea20, whether or not the child was created (it null-tests inside).
						JuneApplyCreateDamageEffectRate( pJuneCreatedEffect );
#else SERV_IRUHADEV_JUNE_SPAWN_INHERIT
						m_pManager->CreateInstance( pOwnerGameUnit,m_DamageData.m_wstrCreateDamageEffectName.c_str(), m_fPowerRate,
							m_DamageData.impactPoint, angle, angle, m_fLandPos );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
#endif //ADD_RENA_SYSTEM

					}
				}
			} break;



		case CX2DamageManager::RT_CREATE_EFFECT_SET:
			{
				if( NULL != pOwnerGameUnit )
				{
					//D3DXVECTOR3 angle = pMeshInst->GetXSkinAnim()->GetMatrix().ATTACK_TIME_END();


					if( NULL != g_pX2Game )
					{
						CX2GameUnit* pTraceUnit = NULL;
						if( true == m_DamageData.m_bCreateEffectSetTraceUnit )
						{
							pTraceUnit = m_DamageData.optrDefenderGameUnit.GetObservable();
						}
#ifdef TRACE_UNIT_DIE_DELETE_EFFECTSET
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
						// June2017 FUN_31f85910 @31f861a1-31f861e2: CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME
						// overrides the created set's fLifeTime with this CEffect's own main-effect mesh
						// (m_hMeshInstHandle, the same handle GetMainEffect() reads) remaining life
						// (MaxLifeTime - NowLifeTime); default is PlayEffectSet's own -1.f, unchanged.
						float fMainEffectLifeTime = -1.f;
						if( m_DamageData.m_bCreateEffectSetMainEffectLifetime && NULL != pMeshInst )
							fMainEffectLifeTime = pMeshInst->GetMaxLifeTime() - pMeshInst->GetNowLifeTime();

#ifdef SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET
						// June2017 FUN_31f85910 @31f86158-31f8672a (effect completion C6b): handle starts at -1 (@31f86193) and every path falls through to the
						// GetEffectSetInstance block below. The branch is on the trace-unit pointer, not on CREATE_EFFECT_SET_TRACE_UNIT: the flag with no
						// defender takes the no-trace path.
						CX2EffectSet::Handle hEffectSet = INVALID_EFFECTSET_HANDLE;
						if( NULL != pTraceUnit )
						{
							// @31f861e2-31f86521: the field-boss collision-box retarget. The gate holds only in a June raid field (unreachable in March), and
							// then the set traces the nearest NUI_FIELD_BOSS_COLLISION_BOX_1 instead; otherwise March's call. LIFTTIME applies on this path only.
							CX2GameUnit* pJuneBoxUnit = JuneFindFieldBossCollisionBox( pTraceUnit );
							hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), pOwnerGameUnit,
								( NULL != pJuneBoxUnit ? pJuneBoxUnit : pTraceUnit ), false, m_fPowerRate, fMainEffectLifeTime );
						}
						else if( NULL != pMeshInst )
						{
							// @31f8662d-31f86711: no trace unit. Only with the main-effect mesh (without one, no effect set at all). The set is played at the mesh's
							// position (+0x3f0), rotation (+0x5b4/+0x5b8/+0x5bc, GetRotateDegree) and direction (31c82e40 = CXMeshInstance::GetDirVec: (1,0,0)
							// rotated by the rotation plus the local rotation), life -1: LIFTTIME is not applied. June's trailing 1.0 has no March parameter.
							hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), pOwnerGameUnit, NULL, false, m_fPowerRate,
								-1.f, D3DXVECTOR3( 1, 1, 1 ), true, pMeshInst->GetPos(), pMeshInst->GetRotateDegree(), pMeshInst->GetDirVec() );
						}
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
						JuneFxHit( GetName(), L"CREATE_EFFECT_SET", L"%s path=%s mesh=%d handleValid=%d", m_DamageData.m_wstrCreateEffectSetName.c_str(),
							( NULL != pTraceUnit ? L"trace_unit" : L"no_trace_unit" ), (int)( NULL != pMeshInst ), (int)( INVALID_EFFECTSET_HANDLE != hEffectSet ) );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#else SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET
						CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), pOwnerGameUnit, pTraceUnit, false, m_fPowerRate, fMainEffectLifeTime );
#endif SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET
#else //SERV_IRUHADEV_JUNE_DAMAGEDATA
						CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), pOwnerGameUnit, pTraceUnit, false, m_fPowerRate );
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA

						CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSet );
						if( NULL != pInstance )
						{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
							// June2017 FUN_31f85910 @31f86755-31f86788: CREATE_EFFECT_SET_MAIN_EFFECT_DELAYTIME
							// writes the same main-effect life delta onto the instance's own delay timer.
							if( m_DamageData.m_bCreateEffectSetMainEffectDelaytime && NULL != pMeshInst )
								pInstance->m_fDelayTime = pMeshInst->GetMaxLifeTime() - pMeshInst->GetNowLifeTime();
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA

#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
							// June2017 FUN_31f85910 @31f8678b-31f867ad (effect completion C5): TRACE_NEAREST_BONE, after the
							// DELAYTIME write and before the m_bTraceUnitDieDeleteEffectSet copy (@31f867b2). With a trace unit
							// and the main-effect mesh (ebp = the mesh instance GetMeshInstance returned, [ebp + 0x3f0] its
							// position), 321da350 stores the trace unit's nearest collision frame onto the instance.
							if( m_DamageData.m_bTraceNearestBone && NULL != pTraceUnit && NULL != pMeshInst )
								pInstance->JuneSetTraceNearestBone( pMeshInst->GetPos(), pTraceUnit );
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA

							pInstance->m_bTraceUnitDieDeleteEffectSet = m_DamageData.m_bTraceUnitDieDeleteEffectSet;
							pInstance->m_bCreateEffectSetTraceUnit = m_DamageData.m_bCreateEffectSetTraceUnit;

#ifdef ADD_RENA_SYSTEM //김창한
							//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
							pInstance->m_eFirstAttack = m_DamageData.m_eFirstAttack;
							pInstance->m_RelateSkillData = m_DamageData.m_RelateSkillData;
#endif //ADD_RENA_SYSTEM
						}

#else //TRACE_UNIT_DIE_DELETE_EFFECTSET
						CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), pOwnerGameUnit, pTraceUnit, false, m_fPowerRate
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                            , -1.f, D3DXVECTOR3( 1, 1, 1 ), false, D3DXVECTOR3( 0, 0, 0 ), D3DXVECTOR3( 0, 0, 0 ), D3DXVECTOR3( 0, 0, 0 ), m_bSkipNpcReactionSimulation
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                            );
#endif //TRACE_UNIT_DIE_DELETE_EFFECTSET
					}
				}

			} break;

		default:
			{
				if( bIsValidUnit == true )
				{
					if( NULL != pOwnerGameUnit &&  NULL != pOwnerGameUnit->GetComboManager() )
					{
#ifdef DUNGEON_ITEM
						if( m_DamageData.m_fHpPercentUp > 0.f || m_DamageData.m_fMpPercentUp > 0.f )
							break;
#endif


#ifdef SERV_CHUNG_TACTICAL_TROOPER		/// 택티컬 필드는 예외
						if( m_DamageData.m_fIncreaseMP > 0.f )
							break;
#endif SERV_CHUNG_TACTICAL_TROOPER

						if ( m_DamageData.m_bNoDamage )
							break;

						pOwnerGameUnit->GetComboManager()->AddCombo();

#ifdef SERV_PET_SYSTEM
						if(pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER)
						{
							CX2PET *pPet = g_pData->GetPetManager()->GetPet( pOwnerGameUnit->GetUnitUID() );
							if( pPet != NULL )
							{
								pPet->AttackResult( m_DamageData.fHitAddMP );
							}
						}						
#endif
						if( pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
						{
							CX2GUNPC *pNpc = static_cast< CX2GUNPC* >( pOwnerGameUnit );
							CX2AllyNPCAI* pNPCAI = (CX2AllyNPCAI*) pNpc->GetNPCAI();
							if ( pNPCAI != NULL && pNPCAI->GetAIType() == CX2NPCAI::NAT_ALLY 
								&& NULL != pNPCAI->GetAllyGameUnit() )
							{
								CX2GUUser *pUser = static_cast<CX2GUUser*>( pNPCAI->GetAllyGameUnit() );
								if( pUser->GetComboManager() != NULL )
									pUser->GetComboManager()->AddCombo();
							}
						}

#ifdef SERV_ARME_DIMENSION_WITCH
						if( (	pOwnerGameUnit->GetHitCreateDamageEffectRate() > 0.f && 
								pOwnerGameUnit->GetRandomFloat() <= pOwnerGameUnit->GetHitCreateDamageEffectRate() ) ||
							(	m_DamageData.m_fCreateDamageEffectRate > 0.f &&
								pOwnerGameUnit->GetRandomFloat() <= m_DamageData.m_fCreateDamageEffectRate ) )
						{
							D3DXVECTOR3 angle = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
							// June2017 CEffect::AttackResult 31f85910 @31f86a45-31f86a76 (effect completion C3): the random-spawn child gets SetScale( DamageData
							// +0x498 ) when the owner is a user (+0x264 == 0). June runs 31fcd640( owner ) first (the class callback at +0x8dc0, none in March).
							CX2DamageEffect::CEffect* pJuneRandomEffect = m_pManager->CreateInstance( pOwnerGameUnit,m_DamageData.m_wstrCreateDamageEffectName.c_str(), 
								pOwnerGameUnit->GetHitCreateDamageEffectPowerRate(),
								m_DamageData.impactPoint, angle, angle, m_fLandPos );
							if( NULL != pJuneRandomEffect && CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
								pJuneRandomEffect->SetScale( m_DamageData.m_vCreateDamageEffectNameScale );
#else SERV_IRUHADEV_JUNE_DAMAGEDATA
							m_pManager->CreateInstance( pOwnerGameUnit,m_DamageData.m_wstrCreateDamageEffectName.c_str(), 
								pOwnerGameUnit->GetHitCreateDamageEffectPowerRate(),
								m_DamageData.impactPoint, angle, angle, m_fLandPos );
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
						}						
#endif

						pOwnerGameUnit->DoDelegateProcessInAttackResult( m_DamageData );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
						// June2017 FUN_31f85910 @31f86a9d-31f86acb, default case: MAKE_TARGET stores the
						// defender's UnitUID/GameUnitType onto the owner. No March reader yet.
						if( m_DamageData.m_bMakeTarget && NULL != pDefenderGameUnit )
						{
							pOwnerGameUnit->SetMakeTarget( pDefenderGameUnit->GetUnitUID(), pDefenderGameUnit->GetGameUnitType() );
						}
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA
					}
				}
			}
			break;
	}

#ifdef SKILL_CASH_10_TEST
	// 이펙트 타격시 메쉬플레이어의 속성값을 바꾼다.
	// ex:타격시 메쉬 이동속도를 낮춘다.
	if( m_fReactAnimSpeed > 0.f && pMeshInst != NULL )
	{
		 pMeshInst->SetAnimSpeed( m_fReactAnimSpeed );
	}
#endif

	if( bIsValidUnit == true )
	{
		if( pOwnerGameUnit != NULL && pOwnerGameUnit->IsLocalUnit() == true )
		{
#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			/// 애드일 땐, 타격시 DP 수치 올려 주자
			if ( CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
			{
				CX2GUUser* pUser = static_cast< CX2GUUser* >( pOwnerGameUnit );

				if ( NULL != pUser && NULL != pUser->GetUnit() && CX2Unit::UT_ADD == pUser->GetUnit()->GetType() )
				{
					/// 프레임당 한번만 DP 회복 처리
					if ( false == pUser->GetUpDPByAttackThisFrame() )
					{
						pOwnerGameUnit->UpNowDPValueByHit( m_DamageData );
						pUser->SetUpDPByAttackThisFrame( true );
					}
				}
			}
#endif //SERV_9TH_NEW_CHARACTER


			if( pOwnerGameUnit->GetUpMPThisFrame() == false 
#ifdef ADD_DAMAGE_TYPE_BUFF
				&& CX2DamageManager::DT_BUFF != m_DamageData.damageType 
#endif //ADD_DAMAGE_TYPE_BUFF
				)
			{
				int		redAdv						= 0;
				int		blueAdv						= 0;
				float	fIncreaseMpRate				= 1.0f;		// 피*타격시 마나 + 증가치(1.0은 증가가 없는 비율)
				float	fIncreateSoulRate			= 1.0f;		// 피*타격시 각성 + 증가치

#ifdef ELSWORD_WAY_OF_SWORD
				int iOwnerUnitWayOfSwordState = 0;
				if ( pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					CX2GUUser* pUser = static_cast< CX2GUUser* >( pOwnerGameUnit );
					if( CX2Unit::UT_ELSWORD == pUser->GetUnit()->GetType() )
					{
						iOwnerUnitWayOfSwordState = pUser->GetWayOfSwordState();
						float fWSPGain = 0.f;

						fWSPGain = g_pData->GetDamageManager()->CalcWayOfSwordPoint(m_DamageData.m_WayofSwordType, m_DamageData.damage.GetAllDamage(), iOwnerUnitWayOfSwordState );
						pUser->SetWayOfSwordPoint( pUser->GetWayOfSwordPoint() + fWSPGain );

						fIncreaseMpRate *= g_pData->GetDamageManager()->CalcVigorMpUpOnAttack(m_DamageData.m_WayofSwordType, iOwnerUnitWayOfSwordState );
					}
#ifdef NEW_CHARACTER_EL // 검의 길
					else if( CX2Unit::UT_ELESIS == pUser->GetUnit()->GetType() )
					{
						iOwnerUnitWayOfSwordState = pUser->GetWayOfSwordState();
						float fWSPGain = 0.f;

						fWSPGain = g_pData->GetDamageManager()->CalcWayOfSwordPoint(m_DamageData.m_WayofSwordType, m_DamageData.damage.GetAllDamage(), iOwnerUnitWayOfSwordState );
						pUser->SetWayOfSwordPoint( pUser->GetWayOfSwordPoint() + fWSPGain );

						fIncreaseMpRate  *= g_pData->GetDamageManager()->CalcVigorMpUpOnAttack_Elesis(m_DamageData.m_WayofSwordType, iOwnerUnitWayOfSwordState );
					}
#endif //NEW_CHARACTER_EL
				}

#endif ELSWORD_WAY_OF_SWORD
				//{{ kimhc // 2009-11-17 // 길드 스킬 중 정신집중, 집중된 마력, 격노, 분노해방
#ifdef	GUILD_SKILL
				if ( pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					CX2GUUser* pUser = static_cast< CX2GUUser* >( pOwnerGameUnit );
					BYTE byMemberShipGrade = pUser->GetUnit()->GetUnitData().m_byMemberShipGrade;

					const CX2SkillTree::SkillTemplet* pSkillTemplet;

					// 집중된 마력

					pSkillTemplet = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER, byMemberShipGrade);

					if( NULL != pSkillTemplet )
					{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
	
						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER, true ) );	/// 스킬 레벨
	
						fIncreaseMpRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillTempletLevel );
	#else // UPGRADE_SKILL_SYSTEM_2013
						fIncreaseMpRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL );
	#endif // UPGRADE_SKILL_SYSTEM_2013
					}
					
					// 분노해방

					pSkillTemplet = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_LIBERATION_OF_ANGER, byMemberShipGrade);

					if( NULL != pSkillTemplet)
					{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_GP_COMMON_LIBERATION_OF_ANGER, true ) );	/// 스킬 레벨

						fIncreateSoulRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_ON_HIT_REL, iSkillTempletLevel );
#else // UPGRADE_SKILL_SYSTEM_2013
						fIncreateSoulRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_ON_HIT_REL );
#endif // UPGRADE_SKILL_SYSTEM_2013
					}
				}

#endif	GUILD_SKILL
				//}} kimhc // 2009-11-17 // 길드 스킬 중 정신집중, 집중된 마력, 격노, 분노해방
				if( g_pX2Room != NULL && g_pData->GetPVPRoom() != NULL )
				{
					if( g_pX2Room->GetRoomType() == CX2Room::RT_PVP )
					{
						if( g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM
							|| g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM_DEATH )
						{
							int redNum		= g_pX2Room->GetTeamNum( CX2Room::TN_RED, true );
							int blueNum		= g_pX2Room->GetTeamNum( CX2Room::TN_BLUE, true );
							if( redNum > blueNum )
							{
								blueAdv = redNum - blueNum;
							}
							else if( redNum < blueNum )
							{
								redAdv = blueNum - redNum;
							}
						}					
					}
				}
				//{{ dmlee 2008.04.11 캐시 아이템 착용시
				switch( pOwnerGameUnit->GetGameUnitType() )
				{
				case CX2GameUnit::GUT_USER:
					{
						CX2GUUser* pGUUser = static_cast<CX2GUUser*>( pOwnerGameUnit );
						if( pGUUser->GetCashItemAbility().m_fUpMPAtAttackOrDamage > 0.f )
						{
							pGUUser->UpNowMp( pGUUser->GetCashItemAbility().m_fUpMPAtAttackOrDamage );
						}
					} break;
				}
				//}} dmlee 2008.04.11 캐시 아이템 착용시

				if( NULL != pOwnerGameUnit && pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER &&
					CX2DamageManager::AT_SPECIAL != m_DamageData.attackType )
				{
					CX2GUUser* pGUUser = static_cast<CX2GUUser*>( pOwnerGameUnit );
					pGUUser->ProcessAttackResult();
				}


				float fUpMPScaleBySkill = 1.f;
#ifdef NEW_SKILL_TREE
				if( m_DamageData.attackType != CX2DamageManager::AT_SPECIAL && 
					CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
				{
					CX2GUUser* pUser = static_cast<CX2GUUser*>( pOwnerGameUnit );
					
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ESK_GET_MANA_UP, true );

					if( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ESK_GET_MANA_UP );

						if( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
						}
					}
#else //UPGRADE_SKILL_SYSTEM_2013
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_GET_MANA_UP );

					if( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_GET_MANA_UP, iSkillLevel );

						if( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL );
						}
					}
#endif //UPGRADE_SKILL_SYSTEM_2013

				}
#endif NEW_SKILL_TREE

#ifdef BALANCE_GRAND_ARCHER_20121213
				if( m_DamageData.attackType != CX2DamageManager::AT_SPECIAL && 
					CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
				{
					CX2GUUser* pUser = static_cast<CX2GUUser*>( pOwnerGameUnit );

	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE, true );
					if( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE );

						if( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
						}
					}
	#else // UPGRADE_SKILL_SYSTEM_2013
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE );
					if( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE, iSkillLevel );

						if( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL );
						}
					}
	#endif // UPGRADE_SKILL_SYSTEM_2013
				}
#endif //BALANCE_GRAND_ARCHER_20121213
				//{{ JHKang / 강정훈 / 2011.7.4 / 청 2차 전직 데들리 체이서 - 정밀한 사격술(마나 회수)
#ifdef CHUNG_SECOND_CLASS_CHANGE
				if ( CX2DamageManager::DT_MAGIC == m_DamageData.damageType &&
					CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
				{
					CX2GUUser* pUser = static_cast<CX2GUUser*>( pOwnerGameUnit );

	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_ACCURATE_MARKMANSHIP, true );
					if ( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CDC_ACCURATE_MARKMANSHIP );

						if ( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
						}
					}
	#else // UPGRADE_SKILL_SYSTEM_2013
					int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_ACCURATE_MARKMANSHIP );
					if ( iSkillLevel > 0 )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CDC_ACCURATE_MARKMANSHIP, iSkillLevel );

						if ( NULL != pSkillTemplet )
						{
							fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL );
						}
					}
	#endif // UPGRADE_SKILL_SYSTEM_2013
				}
#endif

				//}}
				
				if( pOwnerGameUnit->GetTeam() == CX2Room::TN_RED )
				{
					pOwnerGameUnit->UpNowMpPerHitOthersWithDamageEffect( m_DamageData.fHitAddMP, redAdv, fUpMPScaleBySkill * fIncreaseMpRate );

#ifdef DUNGEON_ITEM

	#ifdef SERV_CHUNG_TACTICAL_TROOPER			/// 택티컬 필드는 예외
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f && 
						m_DamageData.m_fIncreaseMP <= 0.f )
	#else  SERV_CHUNG_TACTICAL_TROOPER
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f )
	#endif SERV_CHUNG_TACTICAL_TROOPER

#else
					if( pOwnerGameUnit->GetHyperModeCount() < 3 )
#endif
					{
						//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
						/// 버프 적용 구조로 변경
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + pOwnerGameUnit->GetHyperChargeSpeed() ) ) + (2.0f * redAdv) ) * fIncreateSoulRate );
			#else // SERV_ADD_LUNATIC_PSYKER
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( pOwnerGameUnit->GetSocketData().m_fHyperModeChargeRate, pOwnerGameUnit->GetUnitLevel() ) ) ) + (2.0f * redAdv) ) * fIncreateSoulRate );
			#endif //SERV_ADD_LUNATIC_PSYKER
						//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
					}
				}
				else if( pOwnerGameUnit->GetTeam() == CX2Room::TN_BLUE )
				{
					pOwnerGameUnit->UpNowMpPerHitOthersWithDamageEffect( m_DamageData.fHitAddMP, blueAdv, fUpMPScaleBySkill * fIncreaseMpRate );

#ifdef DUNGEON_ITEM

	#ifdef SERV_CHUNG_TACTICAL_TROOPER			/// 택티컬 필드는 예외
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f && 
						m_DamageData.m_fIncreaseMP <= 0.f )
	#else  SERV_CHUNG_TACTICAL_TROOPER
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f )
	#endif SERV_CHUNG_TACTICAL_TROOPER

#else
					if( pOwnerGameUnit->GetHyperModeCount() < 3 )
#endif
					{
						//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
						/// 버프 적용 구조로 변경
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + pOwnerGameUnit->GetHyperChargeSpeed() ) ) + (2.0f * blueAdv) ) * fIncreateSoulRate );
		#else // SERV_ADD_LUNATIC_PSYKER
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( pOwnerGameUnit->GetSocketData().m_fHyperModeChargeRate, pOwnerGameUnit->GetUnitLevel() ) ) ) + (2.0f * blueAdv) ) * fIncreateSoulRate );
		#endif //SERV_ADD_LUNATIC_PSYKER
						//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
					}
				}
				else
				{
					pOwnerGameUnit->UpNowMpPerHitOthersWithDamageEffect( m_DamageData.fHitAddMP, 0, fUpMPScaleBySkill * fIncreaseMpRate );

#ifdef DUNGEON_ITEM

	#ifdef SERV_CHUNG_TACTICAL_TROOPER			/// 택티컬 필드는 예외
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f && 
						m_DamageData.m_fIncreaseMP <= 0.f )
	#else  SERV_CHUNG_TACTICAL_TROOPER
					if( pOwnerGameUnit->GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f)
	#endif SERV_CHUNG_TACTICAL_TROOPER

#else
					if( pOwnerGameUnit->GetHyperModeCount() < 3 )
#endif
					{
						//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
						/// 버프 적용 구조로 변경
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + pOwnerGameUnit->GetHyperChargeSpeed() ) ) ) * fIncreateSoulRate );
		#else // SERV_ADD_LUNATIC_PSYKER
						pOwnerGameUnit->UpNowSoul( ( (1.0f * (1.0f + CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( pOwnerGameUnit->GetSocketData().m_fHyperModeChargeRate, pOwnerGameUnit->GetUnitLevel() ) ) ) ) * fIncreateSoulRate );
		#endif // SERV_ADD_LUNATIC_PSYKER
						
						//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
					}
				}
				pOwnerGameUnit->SetUpMPThisFrame( true );



				if( CX2GameUnit::GUT_NPC == pOwnerGameUnit->GetGameUnitType() )
				{
					CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pOwnerGameUnit );

					if( NULL != pNPC->GetNPCAI() &&
						CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )
					{
						CX2AllyNPCAI* pNPCAI = static_cast<CX2AllyNPCAI*>( pNPC->GetNPCAI() );

						CX2GameUnit *pGameUnit = pNPCAI->GetAllyGameUnit();
					
						if( NULL != pGameUnit )
						{
							pGameUnit->UpNowMp( m_DamageData.fHitAddMP * fUpMPScaleBySkill * fIncreaseMpRate );
						}
					}
				}


				// fix!! 요고는 damage effect 쪽에만 구현되어있음. guuser, gunpc쪽에도 구현해줘야함
				if( CX2GameUnit::GUT_USER == pOwnerGameUnit->GetGameUnitType() )
				{
					if( 0.f != m_DamageData.attackerGet.m_fHP )
					{
						pOwnerGameUnit->UpNowHp( m_DamageData.attackerGet.m_fHP );
					}

					if( 0.f != m_DamageData.attackerGet.m_fMP )
					{
						if( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
						{
							pOwnerGameUnit->UpNowMp( m_DamageData.attackerGet.m_fMP * fUpMPScaleBySkill * fIncreaseMpRate );
						}
						else if( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON )
						{
							pOwnerGameUnit->UpNowMp( m_DamageData.attackerGet.m_fMP*0.8f * fUpMPScaleBySkill * fIncreaseMpRate );
						}
					}
				}
			}

			if( CX2GameUnit::GUT_NPC == pOwnerGameUnit->GetGameUnitType() )
			{
				if( 0.f != m_DamageData.attackerGet.m_fHP )
				{
					pOwnerGameUnit->UpNowHp( m_DamageData.attackerGet.m_fHP );
				}

				if( 0.f != m_DamageData.attackerGet.m_fMP )
				{
					if( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
					{
						pOwnerGameUnit->UpNowMp( m_DamageData.attackerGet.m_fMP );
					}
					else if( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON )
					{
						pOwnerGameUnit->UpNowMp( m_DamageData.attackerGet.m_fMP*0.8f );
					}
				}

				CX2GUNPC *pUserNpc = static_cast<CX2GUNPC*>( pOwnerGameUnit );

				if( pUserNpc != NULL && pUserNpc->GetNPCAI() != NULL && 
					pUserNpc->GetNPCAI()->GetAIType() == CX2NPCAI::NAT_ALLY &&
					m_DamageData.fCameraCrashGap > 0.f &&
					m_DamageData.fCameraCrashTime > 0.f &&
                    g_pX2Game != NULL )
				{
					g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );
				}
			}

#ifdef SERV_CHUNG_TACTICAL_TROOPER		///  택티컬 필드의 스톱 타임( 0.f )은 적용되면 않된다.
			
	#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			if( 0.f >= m_DamageData.m_fIncreaseMP && CX2DamageManager::DT_BUFF != m_DamageData.damageType )
	#else // SERV_9TH_NEW_CHARACTER
			if( 0.f >= m_DamageData.m_fIncreaseMP )
	#endif // SERV_9TH_NEW_CHARACTER
			
#endif SERV_CHUNG_TACTICAL_TROOPER
				pOwnerGameUnit->SetStopTime( m_DamageData.fStopTimeAtt );	
		}
	
		if( pOwnerGameUnit != NULL && pOwnerGameUnit->IsMyUnit() == true )
		{
			if( m_DamageData.techPoint > 0 && g_pX2Game != NULL )
				g_pX2Game->AddTechPoint( m_DamageData.techPoint );

//#ifdef ATTACK_DELAY_GAGE
//			if( CX2GameUnit::GUT_USER == m_pOwnerUnit->GetGameUnitType() )
//			{
//				CX2GUUser* pGUUser = (CX2GUUser*)m_pOwnerUnit;
//				pGUUser->AddAttackDelay(  ); // 일단 어택딜레이 게이지 사용안함
//			}
//#endif ATTACK_DELAY_GAGE
		}
	
		if(  pOwnerGameUnit != NULL && pOwnerGameUnit->IsFocusUnit() == true )
		{
			if( m_DamageData.fCameraCrashGap > 0.f &&
				m_DamageData.fCameraCrashTime > 0.f )
			{
                if ( g_pX2Game != NULL )
				    g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );
			}

#ifdef CLEAR_SCREEN_ON_HIT
	#ifdef VERTICAL_SYNC_OPTION
			if ( false == g_pMain->GetGameOption().GetFlashEffect() )
			{
				g_pKTDXApp->GetDGManager()->ClearScreen( m_DamageData.clearScreen );
				g_pKTDXApp->GetDGManager()->SetClearScreenColor( m_DamageData.clearScreenColor );
			}
	#else
			g_pKTDXApp->GetDGManager()->ClearScreen( m_DamageData.clearScreen );
			g_pKTDXApp->GetDGManager()->SetClearScreenColor( m_DamageData.clearScreenColor );
	#endif
#endif // CLEAR_SCREEN_ON_HIT
		}
	
		if ( CX2DamageManager::RT_DUMMY_DAMAGE != m_DamageData.reActResult &&
			pOwnerGameUnit != NULL && pOwnerGameUnit->IsMyUnit() )
			pOwnerGameUnit->SetHitCount( pOwnerGameUnit->GetHitCount() + 1 );
	}
 
	bool bCreateHitEffect = true;
#ifdef SKILL_CASH_10_TEST

	if( NULL != pDefenderGameUnit 
		&& !(pDefenderGameUnit->EmptyAbsorbEffectAttackList())
		&& true == m_DamageData.bCanReflexMagic )
			bCreateHitEffect = false;	
#endif

#ifdef SEASON3_MONSTER_2011_7
	bool bCreateAbsorbMagicEffect = false;
	if( pDefenderGameUnit != NULL && pDefenderGameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
	{
		CX2GUNPC* pGUNPC = (CX2GUNPC*) pDefenderGameUnit;
#ifdef SERV_ARME_DIMENSION_WITCH	// ADW_DISTORTION
		if( pGUNPC->GetAbsorbMagicState() == true && 
			( m_DamageData.bCanReflexMagic == true || true == pGUNPC->GetAbsorbMagicSpecial() &&
			CX2DamageManager::AT_EFFECT == m_DamageData.attackerType && CX2DamageManager::AT_NORMAL == m_DamageData.attackType ) )
#else
		if( pGUNPC->GetAbsorbMagicState() == true && m_DamageData.bCanReflexMagic == true )
#endif
		{
			bCreateHitEffect = false;
			bCreateAbsorbMagicEffect = true;
		}
	}
#endif SEASON3_MONSTER_2011_7

	if( bCreateHitEffect == true)
	{		
		SetHitParticle();
		SetHitDamageEffect();
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		// June2017 FUN_31f85910 @31f87b16-31f87b1a: HIT_EFFECT_SET%d runtime, right after SetHitDamageEffect().
		JuneSetHitEffectSet( GetOwnerUnit(), pDefenderGameUnit );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

		//{{ kimhc // 2010.02.10 //	네메시스 패시브 나소드 기어 강화
#ifdef	EVE_SECOND_CLASS_CHANGE
		if ( NULL != GetOwnerUnit() && GetOwnerUnit()->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			CX2GUUser* pUser = static_cast< CX2GUUser* >( GetOwnerUnit() );
			if ( pUser->GetUnitClass() == CX2Unit::UC_EVE_CODE_NEMESIS )
			{
				if ( pUser->DidHitNasodWeapon() == true &&
					 g_pX2Game != NULL &&
					 g_pX2Game->GetEffectSet() != NULL )
				{
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_NasodWeapon", GetOwnerUnit(),
						NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ), true, m_DamageData.impactPoint );
						pUser->SetHitNasodWeapon( false );
				}
			}
		}
#endif	EVE_SECOND_CLASS_CHANGE
		//}} kimhc // 2010.1.28 //	네메시스 패시브 나소드 기어 강화

	}
#ifdef SEASON3_MONSTER_2011_7
	else if(bCreateAbsorbMagicEffect == true)
	{
#ifdef SERV_ARME_DIMENSION_WITCH	// ADW_DISTORTION
		CX2GUNPC* pGUNPC = (CX2GUNPC*) pDefenderGameUnit;
		
		if ( NULL != pGUNPC && true == pGUNPC->GetAbsorbMagicSpecial() && g_pX2Game != NULL )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ABSORB_MAGIC_Distortion", pDefenderGameUnit, NULL, false, -1.f, -1.f,
													  D3DXVECTOR3(1, 1, 1), true, m_DamageData.impactPoint );

			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ABSORB_MAGIC_Distortion_02", pDefenderGameUnit, NULL, false, -1.f, -1.f,
				D3DXVECTOR3(1, 1, 1), true, pGUNPC->GetBonePos( L"effect_Dummy01" ) );
		}
		else
		{
            if ( g_pX2Game != NULL )
			    g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ABSORB_MAGIC", pDefenderGameUnit);
		}
#else
        if ( g_pX2Game != NULL )
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ABSORB_MAGIC", pDefenderGameUnit);
#endif
	}
#endif SEASON3_MONSTER_2011_7
	else
	{
		// 마법흡수 관련 파티클 생성
		SetSuckMagicParticle(L"Flare_Particle_Elsword_Phoenix_Talon01");
		SetSuckMagicParticle(L"Condense_Fire_Elsword_Phoenix_Talon01");
		SetSuckMagicParticle(L"Condense_Fire_Elsword_Phoenix_Talon02");

        if ( g_pX2Game != NULL )    
		g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera();
	}

	m_DamageTimeNow++;
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f85910 (AttackResult) @31f881b6-31f881fe: RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT.
	// After the hit counter moves and before the DAMAGE_TIME die test, a rate >= 0 (0 included: the
	// clock restarts) puts the main effect's life clock at that fraction of its max life (mesh
	// +0x448 m_NowLifeTime = +0x44c m_MaxLifeTime * rate; June 31c82fbf / 31c8d44d). The default -1
	// skips it. June's out-of-line copy 31f75fc0 is the same test and has no caller.
	if( 0.f <= m_fResetMainEffectLifeTimeRateByHit )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pResetMainEffect = GetMainEffect();
		if( NULL != pResetMainEffect )
		{
			pResetMainEffect->SetNowLifeTime( pResetMainEffect->GetMaxLifeTime() * m_fResetMainEffectLifeTimeRateByHit );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxHit( GetName(), L"RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT", L"rate=%.3f now=%.3f max=%.3f hit=%d/%d",
				m_fResetMainEffectLifeTimeRateByHit, pResetMainEffect->GetNowLifeTime(), pResetMainEffect->GetMaxLifeTime(),
				(int)m_DamageTimeNow, (int)m_DamageTime );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	if( m_DamageTime <= m_DamageTimeNow )
	{
		m_bLive = false;

		for( int i = 0; i < (int)m_StartParticleDataList.size(); i++ )
		{
			ParticleData* pParticleData = m_StartParticleDataList[i];
			if( pParticleData->bForceDieHit == true )
			{
                if ( g_pX2Game != NULL )     
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
				SAFE_DELETE( pParticleData );
				m_StartParticleDataList.erase( m_StartParticleDataList.begin() + i );
				i--;
			}			
		}

		for( int i = 0; i < (int)m_PassiveParticleDataList.size(); i++ )
		{
			ParticleData* pParticleData = m_PassiveParticleDataList[i];
			if( pParticleData->bForceDieHit == true )
			{
                if ( g_pX2Game != NULL )
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
				SAFE_DELETE( pParticleData );
				m_PassiveParticleDataList.erase( m_PassiveParticleDataList.begin() + i );
				i--;
			}	
		}
		for( int i = 0; i < (int)m_TimeParticleDataList.size(); i++ )
		{
			ParticleData* pParticleData = m_TimeParticleDataList[i];
			if( pParticleData->bForceDieHit == true )
			{
                if ( g_pX2Game != NULL )
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( pParticleData->m_hSeq );
				SAFE_DELETE( pParticleData );
				m_TimeParticleDataList.erase( m_TimeParticleDataList.begin() + i );
				i--;
			}	
		}
	}

#ifdef UPGRADE_RAVEN
	if( NULL != pOwnerGameUnit && pOwnerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pGUUser = static_cast<CX2GUUser*>( pOwnerGameUnit );
		pGUUser->AttackResultByType(m_DamageData);

#ifdef PVP_ZOOM_CAMERA
		pGUUser->SetSubZoomCameraTimer();
#endif

	}
#endif

#if defined( _SERVICE_ )
	ELSWORD_VIRTUALIZER_END
#endif
}

#ifdef NEW_SKILL_TREE

bool CX2DamageEffect::CEffect::SetMainEffectScale( float fX, float fY, float fZ )
{
	if( m_hMeshInstHandle != INVALID_MESH_INSTANCE_HANDLE )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle ) : NULL;
		if( pMeshInst != NULL )
		{
			pMeshInst->SetScale( fX, fY, fZ );
			return true;
		}
	}

	return false;
}
bool CX2DamageEffect::CEffect::SetMainEffectScale( D3DXVECTOR3 vScale )
{
	return SetMainEffectScale( vScale.x, vScale.y, vScale.z );
}

void CX2DamageEffect::CEffect::SetPassiveParticleScale( float fX, float fY, float fZ )
{
	m_vPassiveParticleScale = D3DXVECTOR3(fX,fY,fZ);
	//for( int i = 0; i < static_cast<int>(m_PassiveParticleDataList.size()); ++i )
	//{
	//	ParticleData* pData = m_PassiveParticleDataList[i];
	//	if( pData != NULL )
	//	{
	//		pData->vScale.x	= fX;
	//		pData->vScale.y	= fY;
	//		pData->vScale.z	= fZ;
	//	}
	//}
	//return true;
}
void CX2DamageEffect::CEffect::SetPassiveParticleScale( D3DXVECTOR3 vScale )
{
	m_vPassiveParticleScale = vScale;
}

void CX2DamageEffect::CEffect::SetHitDamageEffectScale( float fX, float fY, float fZ )
{
	m_vHitDamageScale = D3DXVECTOR3(fX,fY,fZ);
	//for( int i = 0; i < static_cast<int>(m_vecHitDamageEffect.size()); ++i )
	//{
	//	DamageEffectData* pData = m_vecHitDamageEffect[i];
	//	if( pData != NULL )
	//	{
	//		pData->vMainEffectScale.x = fX;
	//		pData->vMainEffectScale.y = fY;
	//		pData->vMainEffectScale.z = fZ;

	//		pData->vPassiveParticleScale.x = fX;
	//		pData->vPassiveParticleScale.y = fY;
	//		pData->vPassiveParticleScale.z = fZ;
	//	}
	//}

	//return true;
}


void CX2DamageEffect::CEffect::SetHitDamageEffectScale( D3DXVECTOR3 vScale )
{
	m_vHitDamageScale = vScale;
}

void CX2DamageEffect::CEffect::SetDieDamageEffectScale( float fX, float fY, float fZ )
{
	m_vDieDamageScale = D3DXVECTOR3(fX,fY,fZ);
	//for( int i = 0; i < static_cast<int>(m_vecDieDamageEffect.size()); ++i )
	//{
	//	DamageEffectData* pData = m_vecDieDamageEffect[i];
	//	if( pData != NULL )
	//	{
	//		pData->vMainEffectScale.x = fX;
	//		pData->vMainEffectScale.y = fY;
	//		pData->vMainEffectScale.z = fZ;

	//		pData->vPassiveParticleScale.x = fX;
	//		pData->vPassiveParticleScale.y = fY;
	//		pData->vPassiveParticleScale.z = fZ;
	//	}
	//}

	//return true;
}
void CX2DamageEffect::CEffect::SetDieDamageEffectScale( D3DXVECTOR3 vScale )
{
	m_vDieDamageScale = vScale;
}

void CX2DamageEffect::CEffect::SetScale(float fX, float fY, float fZ)
{
	SetMainEffectScale(fX,fY,fZ);
	SetPassiveParticleScale(fX,fY,fZ);
	SetHitDamageEffectScale(fX,fY,fZ);
	SetDieDamageEffectScale(fX,fY,fZ);
}

void CX2DamageEffect::CEffect::SetScale(D3DXVECTOR3 vScale)
{
	SetScale(vScale.x,vScale.y,vScale.z);
}

#endif NEW_SKILL_TREE



#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
	void CX2DamageEffect::CEffect::SetScaleByUnit( const D3DXVECTOR3& vScale )
	{
		ASSERT( 0.f != vScale.x ); 
		ASSERT( 0.f != vScale.y ); 
		ASSERT( 0.f != vScale.z ); 

		SetMainEffectScaleByUnit( vScale );
		//SetPassiveParticleScaleByUnit( vScale );
		SetHitDamageEffectScaleByUnit( vScale );
		SetDieDamageEffectScaleByUnit( vScale );
	}


	bool CX2DamageEffect::CEffect::SetMainEffectScaleByUnit( const D3DXVECTOR3& vScale )
	{
		if( m_hMeshInstHandle != INVALID_MESH_INSTANCE_HANDLE )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle ) : NULL;
			if( pMeshInst != NULL )
			{
				pMeshInst->SetScaleByUnit( vScale );
				return true;
			}
		}

		return false;
	}


#endif GIANT_UNIT_GIANT_EFFECT_TEST
#ifdef EVE_ELECTRA
	D3DXVECTOR3 CX2DamageEffect::CEffect::GetMainEffectScaleByUnit()
	{
		D3DXVECTOR3	vScale = D3DXVECTOR3(1.f, 1.f, 1.f);
		if( m_hMeshInstHandle != INVALID_MESH_INSTANCE_HANDLE )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle ) : NULL;
			if( pMeshInst != NULL )
			{
#ifdef VERIFY_STAT_BY_BUFF
				vScale = pMeshInst->GetVec3ScaleByUnit();
#else
				vScale = pMeshInst->GetScaleByUnit();
#endif // VERIFY_STAT_BY_BUFF

			}
		}
		return vScale;
	}
#endif EVE_ELECTRA








void CX2DamageEffect::CEffect::SetStartParticle()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	
	if ( m_DamageEffectDataInLua.m_vecStartParticle.size() != 0 )
	{
		BOOST_TEST_FOREACH( ParticleData*, pStartParticleData, m_DamageEffectDataInLua.m_vecStartParticle )
		{
			if ( pStartParticleData != NULL )
				SetParticleTable( pStartParticleData );
		}

		//m_StartParticleDataList.reserve( m_DamageEffectDataInLua.m_vecStartParticle.size() );

		/*std::copy( m_DamageEffectDataInLua.m_vecStartParticle.begin(),
			m_DamageEffectDataInLua.m_vecStartParticle.end(), m_StartParticleDataList.begin() );*/

		m_StartParticleDataList = m_DamageEffectDataInLua.m_vecStartParticle;

		// m_StartParticleDataList 가 소멸자에서 메모리를 해제해 주므로 여기서는 하지 않는다.
		m_DamageEffectDataInLua.m_vecStartParticle.clear();
	}
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "START_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();
		if( pData != NULL )
			m_StartParticleDataList.push_back( pData );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)

	
}

void CX2DamageEffect::CEffect::SetPassiveParticle()
{

	KTDXPROFILE();



	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	
	if ( m_DamageEffectDataInLua.m_vecPassiveParticle.size() != 0 )
	{
		BOOST_TEST_FOREACH( ParticleData*, pPassiveParticleData, m_DamageEffectDataInLua.m_vecPassiveParticle )
		{
			if ( pPassiveParticleData != NULL )
			{
				SetParticleTable( pPassiveParticleData, true, GetPassiveparticleTriggerTimeRate() );

				if( m_bReflected == true )
				{
					if( pPassiveParticleData->bApplyRotate == false )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorParticle()->GetInstanceSequence( pPassiveParticleData->m_hSeq ) : NULL;
						if(pSeq != NULL && pSeq->GetParticleType() == CKTDGParticleSystem::PT_3D_PLANE)
						{
							pPassiveParticleData->vScale.x = pPassiveParticleData->vScale.x * -1.f;
						}

					}
				}
				//}}

				pPassiveParticleData->vScale.x	*= m_vPassiveParticleScale.x;
				pPassiveParticleData->vScale.y	*= m_vPassiveParticleScale.y;
				pPassiveParticleData->vScale.z	*= m_vPassiveParticleScale.z;

//#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
//				pPassiveParticleData->vScale.x	*= m_vPassiveParticleScaleByUnit.x;
//				pPassiveParticleData->vScale.y	*= m_vPassiveParticleScaleByUnit.y;
//				pPassiveParticleData->vScale.z	*= m_vPassiveParticleScaleByUnit.z;
//#endif GIANT_UNIT_GIANT_EFFECT_TEST
//


			}
		}

		//m_PassiveParticleDataList.reserve( m_DamageEffectDataInLua.m_vecPassiveParticle.size() );

		/*std::copy( m_DamageEffectDataInLua.m_vecPassiveParticle.begin(),
			m_DamageEffectDataInLua.m_vecPassiveParticle.end(), m_PassiveParticleDataList.begin() );*/

		m_PassiveParticleDataList = m_DamageEffectDataInLua.m_vecPassiveParticle;
		
		// m_PassiveParticleDataList 가 소멸자에서 메모리를 해제해 주므로 여기서는 하지 않는다.
		m_DamageEffectDataInLua.m_vecPassiveParticle.clear();
	}
#else	DAMAGE_EFFECT_WORK
//{{AFX

	int index = 0;
	while( m_pLuaManager->BeginTable( "PASSIVE_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable( true, GetPassiveparticleTriggerTimeRate() );	
		if( pData != NULL )
		{
			m_PassiveParticleDataList.push_back( pData );

#ifdef NEW_SKILL_TREE
			//{{ 09.06.10 태완 : 데미지이펙트 반사시 3D_PLANE이고 ApplyRotate == false인 Passive 이펙트 뒤집기
			// 글리터 네크로맨서 유령 때문에 예외처리
			if(m_bReflected == true)
			{
				if( pData->bApplyRotate == false )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorParticle()->GetInstanceSequence( pData->m_hSeq ) : NULL;
					if(pSeq != NULL && pSeq->GetParticleType() == CKTDGParticleSystem::PT_3D_PLANE)
					{
						pData->vScale.x = pData->vScale.x * -1.f;
					}

				}
			}
			//}}

			pData->vScale.x	*= m_vPassiveParticleScale.x;
			pData->vScale.y	*= m_vPassiveParticleScale.y;
			pData->vScale.z	*= m_vPassiveParticleScale.z;
#endif NEW_SKILL_TREE

//
//#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
//			pData->vScale.x	*= m_vPassiveParticleScaleByUnit.x;
//			pData->vScale.y	*= m_vPassiveParticleScaleByUnit.y;
//			pData->vScale.z	*= m_vPassiveParticleScaleByUnit.z;
//#endif GIANT_UNIT_GIANT_EFFECT_TEST


		}

		index++;

		m_pLuaManager->EndTable();
	}
//}}AFX

#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetTimeParticle()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	
	if ( m_DamageEffectDataInLua.m_vecTimeParticle.size() != 0 )
	{
		BOOST_TEST_FOREACH( ParticleData*, pTimeParticleData, m_DamageEffectDataInLua.m_vecTimeParticle )
		{
			if ( pTimeParticleData != NULL )
				SetParticleTable( pTimeParticleData, false );
		}

		//m_TimeParticleDataList.reserve( m_DamageEffectDataInLua.m_vecTimeParticle.size() );
		/*std::copy( m_DamageEffectDataInLua.m_vecTimeParticle.begin(),
			m_DamageEffectDataInLua.m_vecTimeParticle.end(), m_TimeParticleDataList.begin() );*/

		m_TimeParticleDataList = m_DamageEffectDataInLua.m_vecTimeParticle;

		// m_TimeParticleDataList 가 소멸자에서 메모리를 해제해 주므로 여기서는 하지 않는다.
		m_DamageEffectDataInLua.m_vecTimeParticle.clear();
	}
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "TIME_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable( false );	
		if( pData != NULL )
			m_TimeParticleDataList.push_back( pData );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetSuckMagicParticle(WCHAR *particleName)
{
	KTDXPROFILE();
	
	ParticleData* pData = LoadParticleSuckMagic(particleName);
	SAFE_DELETE( pData );
}

void CX2DamageEffect::CEffect::SetHitParticle()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecHitParticle.size() == 0 )
		return;

	BOOST_TEST_FOREACH( ParticleData*, pHitParticleData, m_DamageEffectDataInLua.m_vecHitParticle )
	{
		if ( pHitParticleData != NULL )
		{
			ParticleData* pTempParticleData= new ParticleData( *pHitParticleData );
			SetParticleTable( pTempParticleData );
			SAFE_DELETE( pTempParticleData );
		}
	}

	// 다단 히트시 여러번 불러질 수 있으므로 m_DamageEffectDataInLua.m_vecHitParticle는 여기서 소멸해 주지 않는다.
	//m_DamageEffectDataInLua.m_vecHitParticle.clear();	
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "HIT_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();
		SAFE_DELETE( pData );
		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetHitDamageEffect()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecHitDamageEffect.size() != 0 )
	{
		BOOST_TEST_FOREACH( DamageEffectData*, pHitDamageEffect, m_DamageEffectDataInLua.m_vecHitDamageEffect )
		{
			if ( pHitDamageEffect != NULL )
			{
				DamageEffectData* pTempHitDamageEffect= new DamageEffectData( *pHitDamageEffect );

				m_vecHitDamageEffect.push_back( pTempHitDamageEffect );
			}
		}

		//m_vecHitDamageEffect = m_DamageEffectDataInLua.m_vecHitDamageEffect;

		// 다단 히트시 여러번 불러질 수 있으므로 m_DamageEffectDataInLua.m_vecHitDamageEffect는 여기서 소멸해 주지 않는다.
		//m_DamageEffectDataInLua.m_vecHitDamageEffect.clear();
	}
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();


	int index = 0;
	while( m_pLuaManager->BeginTable( "HIT_DAMAGE_EFFECT", index ) == true )
	{

		DamageEffectData* pData = new DamageEffectData;

		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",		pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",			pData->boneName,		L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,	true	);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",		pData->bApplyRotateOrigin,	false	);

		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_X",	pData->vMainEffectScale.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Y",	pData->vMainEffectScale.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Z",	pData->vMainEffectScale.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_X",	pData->vPassiveParticleScale.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Y",	pData->vPassiveParticleScale.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Z",	pData->vPassiveParticleScale.z,	0.0f	);



		//PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE",		pData->bForceDie,		false	);
		//PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE_HIT",	pData->bForceDieHit,	false	);
		//PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_COUNT",	pData->triggerCount,	-1		);
		//PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_TIME",		pData->triggerTime,		-1.0f	);
		//PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		pData->fEventTime,		-1.0f	);

		m_vecHitDamageEffect.push_back( pData );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)

	CreateHitDamageEffect( m_vecHitDamageEffect );

	for( UINT i=0; i<m_vecHitDamageEffect.size(); i++ )
	{
		DamageEffectData* pData = m_vecHitDamageEffect[i];
		SAFE_DELETE( pData );
	}
	m_vecHitDamageEffect.erase( m_vecHitDamageEffect.begin(), m_vecHitDamageEffect.end() );

}
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
// June2017 FUN_31f70130: HIT_EFFECT_SET%d runtime, June-only (no March equivalent). AttackResult
// (FUN_31f85910 @31f87b1a) calls it right after SetHitDamageEffect() -- i.e. inside March's own
// bCreateHitEffect == true branch (AttackResult, after SetHitDamageEffect()). Disassembly of
// FUN_31f70130 (not the decompile, which mis-split the pushes across two calls -- see
// june2017_re/README.md's "Method notes"): AttackResult pushes PUSH EBP then PUSH EDI @31f87b16-31f87b17, so inside
// 31f70130 the callee-local EBP (read from the stack slot closest to the return address, i.e. the
// LAST thing the caller pushed) is AttackResult's EDI -- the owner, per @31f85981's -0x1a0
// owner-optr deref -- and the callee-local EBX is AttackResult's own EBP, reloaded @31f87aa1 from a
// spill slot as the cached defender. 31f70130 then pushes EBX(defender), EBP(owner), then the name
// pointer, in that order, for CX2EffectSet::PlayEffectSet's right-to-left push -- i.e.
// PlayEffectSet( name, pOwner, pDefender, false, m_fPowerRate ), with the trailing
// fLifeTime/vScale/bCustomPosition/position/rotation/dir args all at exactly March's own defaults
// (FLD [0x334380f0]=-1.f for fLifeTime, three FLD1 for vScale, PUSH 0 for bCustomPosition, three
// zeroed vec3 locals) -- so June passes March's defaults explicitly rather than differing from
// them. @31f701f6-31f70200: both units must be non-null or the whole call is skipped. Entry +0x0
// (EVENT_TIME) is parsed but never loaded by FUN_31f70130; no delay is implemented.
void CX2DamageEffect::CEffect::JuneSetHitEffectSet( CX2GameUnit* pOwnerGameUnit, CX2GameUnit* pDefenderGameUnit )
{
	if( NULL == g_pX2Game || NULL == g_pX2Game->GetEffectSet() )
		return;
	if( NULL == pOwnerGameUnit || NULL == pDefenderGameUnit )
		return;

	for( UINT i = 0; i < m_DamageEffectDataInLua.m_vecHitEffectSet.size(); i++ )
	{
		HitEffectSetData* pData = m_DamageEffectDataInLua.m_vecHitEffectSet[i];
		if( NULL == pData || true == pData->strEffectSetName.empty() )
			continue;

		g_pX2Game->GetEffectSet()->PlayEffectSet( pData->strEffectSetName.c_str(), pOwnerGameUnit, pDefenderGameUnit, false, m_fPowerRate );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxHit( GetName(), L"HIT_EFFECT_SET", L"%s", pData->strEffectSetName.c_str() );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
// June2017 FUN_31f70330: DIE_CREATE_NPC%d runtime, June-only (no March equivalent). Called on every
// die path immediately before FUN_31f83400 (March's own SetDieDamageEffect(), which already calls
// CreateDieDamageEffect() at X2DamageEffect.cpp:6151). Confirmed by an E8-relative-call scan of the
// whole June .text (not just the decompile): exactly 6 callers of 31f70330 and 6 of 31f83400, and
// they pair 1:1 -- OnFrameMove @31f88847/31f8aa28/31f8aa7b, DestroyInstanceHandle @31f838b6, the
// CEffect*-taking DestroyInstance @31f839e3, and the lock-on prelude (FUN_31f75e20 gate) @31f83c59.
// March calls SetDieDamageEffect() from exactly 5 active sites (DestroyInstanceHandle,
// DestroyInstance(CEffect*&), and OnFrameMove's three die branches), covering 5 of June's 6 -- the
// 6th, OnFrameMove_LockOnData's 31f75e20 prelude, never calls SetDieDamageEffect() at all in March.
// That is a gap Phase 7b left (LockOnData's die path), not this phase's to close; recorded here so
// it is not lost. One JuneDieCreateNpc() at the top of SetDieDamageEffect() reproduces the pairing
// at every site March currently reaches.
//
// Argument mapping to CX2Game::CreateNPCReq (X2Game.h:587; SERV_INSERT_GLOBAL_SERVER,
// SERV_TRAPPING_RANGER_TEST and SUMMON_MONSTER_CARD_SYSTEM+SERV_NEW_DEFENCE_DUNGEON are all defined
// here, so the 14-parameter overload with eAllyTeam and eCreateMonsterType applies) was settled by
// disassembling FUN_31f8b010 (@31f8b010-31f8b084, RET 0x48 = 18 stack dwords including the gate) and
// tracking ESP through its interleaved pushes: the 17 forwarded values land 1:1 on CreateNPCReq's 14
// declared parameters (vPos and iAllyUID each span multiple dwords). Two of June's constants match
// March enum values exactly -- CX2Room::TN_MONSTER == 2 and TN_NONE == -1, CX2GUNPC::NCT_NONE == 0 --
// which is independent, second-source confirmation of the whole ordering, not just the position
// count. X2GUChung.cpp:17128 has an existing March call in the same shape (level=0, eTeam=GetTeam(),
// eAIType=NAT_ALLY, iAllyUID=GetUnitUID()), a third, independent corroboration.
// - unitID = entry NPC_ID (+0x4). EVENT_TIME (+0x0) is parsed but never loaded by FUN_31f70330.
// - level = 0 (the literal constant FUN_31f8b010 forwards, not the owner's actual level)
// - bActive = true, bNoDrop = true, iKeyCode = -1 (all March's own defaults, passed explicitly)
// - vPos = CEffect +0x1f4/+0x1f8/+0x1fc = m_vMainEffectPosition. Confirmed: OnFrameMove's only writer
//   of +0x1f4 is @31f88b25, right after +0x1e8's GetRotateDegree() store, matching March's own
//   m_vMainEffectPosition = pMeshInst->GetMatrix().GetPos(), X2DamageEffect.cpp:2457.
// - bRight = the owner's slot-0x170 vcall result -> GetIsRight(). Confirmed the slot: it has
//   no Ghidra function (disassembled raw, GUUser vtable 334781f4+0x170 = 3208dff0), a bare
//   `mov al,[ecx+off]; ret 0` -- a trivial stored-bool getter, the exact shape GetIsRight()'s
//   own body has, though the byte offset itself was not cross-checked against GetIsRight()'s
//   March member.
// - fDelayTime = 0.0f.
// - eTeam = the owner's XOR-protected +0x278..+0x280 value -> GetTeam(), confirmed by the
//   TN_MONSTER==2 compare inside FUN_322e67c0, the packet-fill routine FUN_31f8b010 forwards into.
// - eAIType = CX2NPCAI::NAT_ALLY (June passes the literal 1).
// - iAllyUID = the owner's slot-0x54 vcall result -> GetUnitUID().
// - bElite = false, eAllyTeam = CX2Room::TN_NONE, eCreateMonsterType = CX2GUNPC::NCT_NONE (June's
//   three remaining literals: 0, -1 -> TN_NONE(-1), 0 -> NCT_NONE(0)).
// The gate (a slot-0x198 vcall on the owner, forwarded into FUN_31f8b010 as its own first parameter
// and short-circuiting the whole call when zero) is re-expressed as GetOwnerUnit()->IsMyUnit(), the
// existing local-ownership check this file already uses for other per-owner-only effects (camera
// crash @X2DamageEffect.cpp:2529, screen shake @:2589). Confirmed, not just inferred by position:
// slot 0x198 has no Ghidra function (disassembled raw, GUUser vtable 334781f4+0x198 = 31ff1f30),
// and its decompile is exactly `g_pX2Game->GetMyUnit() == this`, i.e. March's own IsMyUnit().
void CX2DamageEffect::CEffect::JuneDieCreateNpc()
{
	if( NULL == g_pX2Game || null == m_optrOwnerUnit )
		return;

#ifdef SERV_IRUHADEV_JUNE_DIECREATENPC_DTOR_GUARD
	// GetOwnerUnit() is safe (non-virtual), but IsMyUnit()/GetIsRight() below (both called on the
	// owner) are pure virtual in CX2GameUnit (X2GameUnit.h:2438,2449). When this CEffect is torn
	// down because its OWN owner unit's destructor (~CX2GameUnit) is running -- e.g.
	// CX2DungeonSubStage::DeleteAllNPCUnit force-deleting a unit that still has a live attack effect
	// registered via MakeEffectSetBeDeletedWhenDie() -- the derived vtable (CX2GUNPC etc.) has
	// already unwound to CX2GameUnit's own, and either call is R6025. 2026-09-29 crash:
	// X2_offline.exe.41620.dmp, Burned Forest (dungeon 33270) sub-stage teardown, via
	// JuneDieCreateNpc -> SetDieDamageEffect -> DestroyInstanceHandle ->
	// EffectInstance::~EffectInstance -> ... -> CX2GameUnit::~CX2GameUnit.
	CX2GameUnit* pOwnerGameUnitDtorCheck = GetOwnerUnit();
	if( NULL != pOwnerGameUnitDtorCheck && pOwnerGameUnitDtorCheck->IsInGameUnitDestructor() )
		return;
#endif SERV_IRUHADEV_JUNE_DIECREATENPC_DTOR_GUARD
	CX2GameUnit* pOwnerGameUnit = GetOwnerUnit();
	if( NULL == pOwnerGameUnit || false == pOwnerGameUnit->IsMyUnit() )
		return;

	for( UINT i = 0; i < m_DamageEffectDataInLua.m_vecDieCreateNpc.size(); i++ )
	{
		DieCreateNpcData& data = m_DamageEffectDataInLua.m_vecDieCreateNpc[i];

		g_pX2Game->CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)data.iNpcID, 0, true, m_vMainEffectPosition,
			pOwnerGameUnit->GetIsRight(), 0.f, true, -1,
			(CX2Room::TEAM_NUM)pOwnerGameUnit->GetTeam(), CX2NPCAI::NAT_ALLY, pOwnerGameUnit->GetUnitUID(), false,
			CX2Room::TN_NONE, CX2GUNPC::NCT_NONE );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxHit( GetName(), L"DIE_CREATE_NPC", L"npcId=%d pos=(%.1f,%.1f,%.1f) team=%d",
			data.iNpcID, m_vMainEffectPosition.x, m_vMainEffectPosition.y, m_vMainEffectPosition.z, (int)pOwnerGameUnit->GetTeam() );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

void CX2DamageEffect::CEffect::SetDieDamageEffect()
{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	JuneDieCreateNpc();	// June2017: FUN_31f70330 pairs with FUN_31f83400 (this function) at all six confirmed June die-path call sites
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecDieDamageEffect.size() != 0 )
	{
		BOOST_TEST_FOREACH( DamageEffectData*, pDieDamageEffect, m_DamageEffectDataInLua.m_vecDieDamageEffect )
		{
			if ( pDieDamageEffect != NULL )
			{
				DamageEffectData* pTempDieDamageEffect= new DamageEffectData( *pDieDamageEffect );
				if ( NULL != pTempDieDamageEffect )
					m_vecDieDamageEffect.push_back( pTempDieDamageEffect );
			}
		}

		// 다단 히트시 여러번 불러질 수 있으므로 m_DamageEffectDataInLua.m_vecDieDamageEffect는 여기서 소멸해 주지 않는다.
		//m_DamageEffectDataInLua.m_vecDieDamageEffect.clear();

		//m_vecDieDamageEffect = m_DamageEffectDataInLua.m_vecDieDamageEffect;
		//m_DamageEffectDataInLua.m_vecDieDamageEffect.clear();
	}
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();


	int index = 0;
	while( m_pLuaManager->BeginTable( "DIE_DAMAGE_EFFECT", index ) == true )
	{

		DamageEffectData* pData = new DamageEffectData;

		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,				CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,			L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,		true	);

		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);

		//PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE",		pData->bForceDie,		false	);
		//PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE_HIT",	pData->bForceDieHit,	false	);
		//PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_COUNT",	pData->triggerCount,	-1		);
		//PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_TIME",		pData->triggerTime,		-1.0f	);
		//PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		pData->fEventTime,		-1.0f	);

		m_vecDieDamageEffect.push_back( pData );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)

	if ( m_vecDieDamageEffect.empty() == false )
	{
		CreateDieDamageEffect( m_vecDieDamageEffect );
	}

	for( UINT i=0; i<m_vecDieDamageEffect.size(); i++ )
	{
		DamageEffectData* pData = m_vecDieDamageEffect[i];
		SAFE_DELETE( pData );
	}
	m_vecDieDamageEffect.erase( m_vecDieDamageEffect.begin(), m_vecDieDamageEffect.end() );


}


void CX2DamageEffect::CEffect::SetCameraData()
{
#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
	if( g_pMain->GetGameOption().GetOptionList().m_bDynamicCamera == false )
		return;
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION

	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecCameraData.size() == 0 )
		return;

	//m_CameraDataList.reserve( m_DamageEffectDataInLua.m_vecCameraData.size() );

	/*std::copy( m_DamageEffectDataInLua.m_vecCameraData.begin(), 
		m_DamageEffectDataInLua.m_vecCameraData.end(), m_CameraDataList.begin() );*/

	m_CameraDataList = m_DamageEffectDataInLua.m_vecCameraData;

	m_DamageEffectDataInLua.m_vecCameraData.clear();
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "CAMERA", index ) == true )
	{
		CameraData data;
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "UNIT_SELECT",		data.unitSelect,	UNIT_SELECT,	DEUS_ALL );
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "CRASH_TYPE",		data.crashType,		CKTDGCamera::SHAKE_TYPE,		CKTDGCamera::DECT_UP_DOWN );
		PLUA_GET_VALUE(			m_pLuaManager, "SPEED",				data.fSpeed,		0 );
		PLUA_GET_VALUE(			m_pLuaManager, "ACCEL",				data.fAccel,		0 );
		PLUA_GET_VALUE(			m_pLuaManager, "GAP",				data.fGap,			0 );
		PLUA_GET_VALUE(			m_pLuaManager, "TIME",				data.fTime,			0 );
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,	0 );				

		m_CameraDataList.push_back( data );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetScreenData()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecScreenData.size() == 0 )
		return;

	//m_ScreenDataList.reserve( m_DamageEffectDataInLua.m_vecScreenData.size() );

	/*std::copy( m_damageeffectdatainlua.m_vecscreendata.begin(), 
		m_damageeffectdatainlua.m_vecscreendata.end(), m_screendatalist.begin() );*/


	m_ScreenDataList = m_DamageEffectDataInLua.m_vecScreenData;

	m_DamageEffectDataInLua.m_vecScreenData.clear();
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "SCREEN", index ) == true )
	{
		ScreenData data;
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "UNIT_SELECT",		data.unitSelect,	UNIT_SELECT,	DEUS_ALL );
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,	0		);
		PLUA_GET_VALUE(			m_pLuaManager, "COUNT",				data.clearCount,	1		);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_A",			data.color.a,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_R",			data.color.r,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_G",			data.color.g,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_B",			data.color.b,		1.0f	);

		m_ScreenDataList.push_back( data );

		index++;

		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetSoundData()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecSoundData.size() == 0 )
		return;

	//m_SoundDataList.reserve( m_DamageEffectDataInLua.m_vecSoundData.size() );

	/*std::copy( m_DamageEffectDataInLua.m_vecSoundData.begin(), 
		m_DamageEffectDataInLua.m_vecSoundData.end(), m_SoundDataList.begin() );*/

	/*std::vector< SoundData >::reverse_iterator vrItr = m_DamageEffectDataInLua.m_vecSoundData.rbegin();

	while( vrItr != m_DamageEffectDataInLua.m_vecSoundData.rend() )
	{
		m_SoundDataList.push_back( *vrItr );
		
	}*/

	m_SoundDataList = m_DamageEffectDataInLua.m_vecSoundData;

	m_DamageEffectDataInLua.m_vecSoundData.clear();
#else	DAMAGE_EFFECT_WORK
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "SOUND", index ) == true )
	{
		SoundData data;
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,		0		);
		PLUA_GET_VALUE(			m_pLuaManager, "FILE_NAME",			data.fileName,			L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "RATE",				data.iRate,				100		);
		PLUA_GET_VALUE(			m_pLuaManager, "USE_GLOBAL_TIME",	data.bUseGlobalTime,	false	);

		PLUA_GET_VALUE(			m_pLuaManager, "LOOP",	data.bLoop,	false	);


		m_SoundDataList.push_back( data );
		index++;
		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

void CX2DamageEffect::CEffect::SetDamageEffect()
{
	//{{ kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
	if ( m_DamageEffectDataInLua.m_vecCreateDamageEffect.empty() )
		return;

	m_DamageEffectDataList = m_DamageEffectDataInLua.m_vecCreateDamageEffect;

	// m_DamageEffectDataList 가 소멸자에서 메모리를 해제해 주므로 여기서는 하지 않는다.
	m_DamageEffectDataInLua.m_vecCreateDamageEffect.clear();
#else	DAMAGE_EFFECT_WORK
	int index = 0;
	while( m_pLuaManager->BeginTable( "CREATE_DAMAGE_EFFECT", index ) == true )
	{
		DamageEffectData* pData = new DamageEffectData;
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",			pData->fEventTime,	-1.0f		);
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",		pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",			pData->boneName,		L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,	true	);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",	pData->bApplyRotateOrigin,	false	);

		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);



		m_DamageEffectDataList.push_back( pData );
		index++;
		m_pLuaManager->EndTable();
	}
#endif	DAMAGE_EFFECT_WORK
	//}} kimhc // 2009-10-20 // 기존의 Set함수를 Load와 Set 으로 분류(일명 독구름 작업)
}

CX2DamageEffect::ParticleData* CX2DamageEffect::CEffect::LoadParticleSuckMagic( WCHAR *particleName )
{
	KTDXPROFILE();   

	ParticleData* pData = new ParticleData;

	pData->posType = DEPT_IMPACT_POINT;
	pData->particleName = particleName;
	pData->vScale = D3DXVECTOR3(1.f, 1.f, 1.f);

	CKTDGParticleSystem::CParticleEventSequence* pSeq = ( g_pX2Game != NULL ) 
        ? g_pX2Game->GetMajorParticle()->CreateSequence( NULL, pData->particleName.c_str(), 0,0,0 ) : NULL;

	if( NULL == pSeq )
	{
		SAFE_DELETE( pData );
		return NULL;
	}

    pData->m_hSeq = pSeq->GetHandle();

	pSeq->SetLandPosition( m_fLandPos );

	SetParticleData( pData );	

	return pData;
}

//{{ kimhc // 2009-10-20 // 기존의 LoadParticleTable을 Load와 Set 으로 분류(일명 독구름 작업)
#ifdef	DAMAGE_EFFECT_WORK
CX2DamageEffect::ParticleData* CX2DamageEffect::CEffect::LoadParticleTable()
{
	KTDXPROFILE();   

	ParticleData* pData = new ParticleData;

	PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",	pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
	PLUA_GET_VALUE(			m_pLuaManager, "PARTICLE_NAME",	pData->particleName,	L""		);
	PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",	pData->bApplyRotate,	true	);
	PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE",		pData->bForceDie,		false	);
	PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE_HIT",	pData->bForceDieHit,	false	);
	PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_COUNT",	pData->triggerCount,	-1		);
	PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_TIME",	pData->triggerTime,		-1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",		pData->boneName,		L""		);
	PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",	pData->fEventTime,		-1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",		pData->fOffsetX,		0.0f	);  // added by wonpok
	PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",		pData->fOffsetY,		0.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",		pData->fOffsetZ,		0.0f	);
	PLUA_GET_VALUE(         m_pLuaManager, "APPLY_ROTATED_OFFSET", pData->bApplyRotatedOffset  , false ); // added by lucidash : 2010.11.24

	//{{ kimhc // 2010.2.9 //	이브 2차전직 아토믹 블래스터에 필요로 추가
#ifdef	EVE_SECOND_CLASS_CHANGE
	PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",		pData->vRotateAngleDegree.x,	0.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",		pData->vRotateAngleDegree.y,	0.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",		pData->vRotateAngleDegree.z,	0.0f	);
#endif	EVE_SECOND_CLASS_CHANGE
	//}} kimhc // 2010.2.9 //	이브 2차전직 아토믹 블래스터에 필요로 추가

#ifdef DAMAGE_EFFECT_TIMED_PARTICLE_GLOBAL_TIME
	PLUA_GET_VALUE(			m_pLuaManager, "USE_GLOBAL_TIME",	pData->bUseGlobalTime,	false	);
#endif DAMAGE_EFFECT_TIMED_PARTICLE_GLOBAL_TIME

//{{ kimhc // 2010.2.3 //	이브 2차 전직
#ifdef	EVE_SECOND_CLASS_CHANGE
	// 프레임당 움직이는 각
	PLUA_GET_VALUE(			m_pLuaManager, "MOVE_ANGLE_X_PER_FRAME", pData->vMoveAnglePerFrame.x, 0.0f );
	PLUA_GET_VALUE(			m_pLuaManager, "MOVE_ANGLE_Y_PER_FRAME", pData->vMoveAnglePerFrame.y, 0.0f );
	PLUA_GET_VALUE(			m_pLuaManager, "MOVE_ANGLE_Z_PER_FRAME", pData->vMoveAnglePerFrame.z, 0.0f );
#endif	EVE_SECOND_CLASS_CHANGE
//}} kimhc // 2010.2.3 //	이브 2차 전직
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",		pData->vScale.x,		1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",		pData->vScale.y,		1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",		pData->vScale.z,		1.0f	);

	PLUA_GET_VALUE(			m_pLuaManager, "TRACE",			pData->bTrace,			true	);
#ifdef EVE_ELECTRA
	PLUA_GET_VALUE(			m_pLuaManager, "TRACE_MORE",	pData->bTraceMore,		false	);
#endif EVE_ELECTRA
#ifdef HIT_PARTICLE_TRACE_UNIT
	PLUA_GET_VALUE(			m_pLuaManager, "TRACE_DEFENDER_UNIT",	pData->bTraceDefenderUnit,		false	);
#endif HIT_PARTICLE_TRACE_UNIT

#ifdef EVE_ELECTRA
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_BY_MAINEFFECT_SCALE_Y",		pData->bScaleByMainEffectScaleY,		false	);
#endif EVE_ELECTRA

#ifdef BALANCE_PATCH_20120329
	PLUA_GET_VALUE(			m_pLuaManager, "SHOW_INNER_LANDHEIGHT",		pData->fShowLandHeight,		99999.f	);
	
#endif

#ifdef ARA_CHANGE_CLASS_FIRST
	PLUA_GET_VALUE(			m_pLuaManager, "REVERSE_Y",					pData->m_bReverseY,		false	);
#endif // ARA_CHANGE_CLASS_FIRST

#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
	PLUA_GET_VALUE_ENUM( m_pLuaManager, "SHOW_LEVEL", pData->m_iShowLevel, CX2GameOption::OptionLevel, (int) CX2GameOption::OL_LOW );
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f66e60 (LoadParticleTable) @31f68d90-31f68efe: APPLY_START_PARTICLE_SCALE, int
	// getter 32fe9fd0, default 0. Read unconditionally regardless of which wrapper called this
	// function (START/PASSIVE/TIME/HIT_PARTICLE). Phase 6b: parse only.
	PLUA_GET_VALUE(			m_pLuaManager, "APPLY_START_PARTICLE_SCALE",	pData->iApplyStartParticleScale,	0	);
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	return pData;
	
}

void CX2DamageEffect::CEffect::SetParticleTable( ParticleData* pData, bool bCreate /*= true*/, float fTriggerTimeRate /*= -1.f*/ )
{
	if( bCreate == true )
	{
        pData->m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = ( g_pX2Game != NULL )
            ? g_pX2Game->GetMajorParticle()->CreateSequence( NULL, pData->particleName.c_str(), 0,0,0, -1, -1, -1, pData->triggerCount, pData->triggerTime ) : NULL;
        if ( pSeq == NULL )
        {
			//SAFE_DELETE( pData );
            return;
        }
        pData->m_hSeq = pSeq->GetHandle();

		pSeq->SetLandPosition( m_fLandPos );

#ifdef NEW_SKILL_TREE
		if( fTriggerTimeRate != -1.f && pSeq->GetTriggerTime() != -1.f )
		{
			float fTriggerTime = pSeq->GetTriggerTime() * fTriggerTimeRate;	//모든 트리거 타임 셋팅이 끝난후 호출하여 Rate를 적용해준다.
			pSeq->SetTriggerTime( fTriggerTime );
		}
#endif NEW_SKILL_TREE

		SetParticleData( pData );
	}

#ifdef HIT_PARTICLE_TRACE_UNIT
	if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle()->IsLiveInstanceHandle( pData->m_hSeq ) == true 
		&& true == pData->bTraceDefenderUnit )
	{
		if( null != m_DamageData.optrDefenderGameUnit )
		{
			m_DamageData.optrDefenderGameUnit->AddTraceUnitHitParticle( pData->m_hSeq );
		}
	}
#endif HIT_PARTICLE_TRACE_UNIT
}
#else	DAMAGE_EFFECT_WORK
CX2DamageEffect::ParticleData* CX2DamageEffect::CEffect::LoadParticleTable( bool bCreate, float fTriggerTimeRate )
{
	KTDXPROFILE();   

	ParticleData* pData = new ParticleData;

	PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",	pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
	PLUA_GET_VALUE(			m_pLuaManager, "PARTICLE_NAME",	pData->particleName,	L""		);
	PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",	pData->bApplyRotate,	true	);
	PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE",		pData->bForceDie,		false	);
	PLUA_GET_VALUE(			m_pLuaManager, "FORCE_DIE_HIT",	pData->bForceDieHit,	false	);
	PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_COUNT",	pData->triggerCount,	-1		);
	PLUA_GET_VALUE(			m_pLuaManager, "TRIGGER_TIME",	pData->triggerTime,		-1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",		pData->boneName,		L""		);
	PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",	pData->fEventTime,		-1.0f	);
    PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",		pData->fOffsetX,		0.0f	);  // added by wonpok
    PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",		pData->fOffsetY,		0.0f	);
    PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",		pData->fOffsetZ,		0.0f	);

	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",		pData->vScale.x,		1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",		pData->vScale.y,		1.0f	);
	PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",		pData->vScale.z,		1.0f	);

	PLUA_GET_VALUE(			m_pLuaManager, "TRACE",			pData->bTrace,			true	);
	PLUA_GET_VALUE(         m_pLuaManager, "APPLY_ROTATED_OFFSET" , pData->bApplyRotatedOffset ,  false ); // added by lucidash.

	if( bCreate == true )
	{
		pData->m_hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL, pData->particleName.c_str(), 0,0,0, -1, -1, -1, pData->triggerCount, pData->triggerTime );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pData->m_hSeq );
		if( NULL == pSeq )
		{
			SAFE_DELETE( pData );
			return NULL;
		}
		
		pSeq->SetLandPosition( m_fLandPos );

#ifdef NEW_SKILL_TREE
		if( fTriggerTimeRate != -1.f && pSeq->GetTriggerTime() != -1.f )
		{
			float fTriggerTime = pSeq->GetTriggerTime() * fTriggerTimeRate;	//모든 트리거 타임 셋팅이 끝난후 호출하여 Rate를 적용해준다.
			pSeq->SetTriggerTime( fTriggerTime );
		}
#endif NEW_SKILL_TREE

		SetParticleData( pData );
	
	}

	return pData;
}
#endif	DAMAGE_EFFECT_WORK
//}} kimhc // 2009-10-20 // 기존의 LoadParticleTable을 Load와 Set 으로 분류(일명 독구름 작업)

void CX2DamageEffect::CEffect::CreateHitDamageEffect( vector<DamageEffectData*>& vecHitDamageEffect )
{
	KTDXPROFILE();

	if( NULL == g_pX2Game ||
		null == m_optrOwnerUnit ||
		false == g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstHandle ) )
	{
		return;
	}


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	ASSERT( pMeshInst != NULL );
	if( pMeshInst == NULL )
		return;


	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK	
	// 현재 열려있는 damage effect 테이블을 잠시 닫고 
	m_pLuaManager->EndTable();
#endif	DAMAGE_EFFECT_WORK		
	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

	


	for( UINT i=0; i<vecHitDamageEffect.size(); i++ )
	{
		DamageEffectData* pData = vecHitDamageEffect[i];


		D3DXVECTOR3 vAxisAngleDegree( 0, 0, 0);
		D3DXVECTOR3 vRotateAngleDegree( 0, 0, 0);
		D3DXVECTOR3 vPosition(0,0,0);

#ifdef SERV_RENA_NIGHT_WATCHER
		if( DET_ARROW_OF_EXPLOSION != GetType() )
#endif SERV_RENA_NIGHT_WATCHER
		{
			// 본섭 DamageEffect.lua 롤백하지 않고 반영시키기 위해 주석 해제
			// oasis907 : 김상윤 [2010.11.4] // 그랜드 아처 - 진동 사격(패시브)
			if(GetOwnerUnit() != NULL )
			{
				if(GetOwnerUnit()->GetRandomFloat() > GetDamageData().m_fHitDamageEffectInvokeRate )
					continue;
			}
		}

		switch( pData->posType )
		{
		case DEPT_BONE:
			{
				if( pData->bApplyRotate == true )
				{
					vAxisAngleDegree = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
					vRotateAngleDegree = vAxisAngleDegree;
				}
				else
				{
					vAxisAngleDegree = pData->vRotateAngleDegree;
					vRotateAngleDegree = vAxisAngleDegree;
				}
				vPosition = pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() ); 
			}
			break;

		case DEPT_UNIT_BONE:
			{
				if( null != m_optrOwnerUnit )
				{
					vAxisAngleDegree = m_optrOwnerUnit->GetRotateDegree();

					if( pData->bApplyRotate == true )
					{
						vRotateAngleDegree = vAxisAngleDegree;
					}

					vPosition = m_optrOwnerUnit->GetBonePos( pData->boneName.c_str() );
				}
			}
			break;

		case DEPT_IMPACT_POINT:
			{
#ifdef NEW_SKILL_2010_11 
				//{{ oasis907 : 김상윤 [2010.11.22] // 그랜드 아처 - 진동 사격(패시브)
				if(pData->bApplyRotateOrigin == true)
				{
					vAxisAngleDegree = m_AngleDegree;
					vRotateAngleDegree = vAxisAngleDegree;
				}
				//}} bApplyRotate 도 처리하고 싶지만 기본으로 true가 되어있어 기존 데미지 이펙트에 미칠 영향이 커 처리 안함.
#endif NEW_SKILL_2010_11
				vPosition = m_DamageData.impactPoint;
			}
			break;

#ifdef RAVEN_WEAPON_TAKER
		case DEPT_DIE_POS:
#endif RAVEN_WEAPON_TAKER
		case DEPT_POS:
			{
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
				if( pData->bOnlyApplyRotateOffset == true )
				{
					vAxisAngleDegree = pMeshInst->GetMatrix().GetRotateDegree();
					vRotateAngleDegree = pData->vRotateAngleDegree + vAxisAngleDegree;
				}
				else
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE
				{
					if( pData->bApplyRotate == true )
					{
						vAxisAngleDegree = pMeshInst->GetMatrix().GetRotateDegree();
						vRotateAngleDegree = vAxisAngleDegree;
					}
					else
					{
						vAxisAngleDegree = pData->vRotateAngleDegree;
						vRotateAngleDegree = vAxisAngleDegree;
					}
				}
				vPosition = pMeshInst->GetMatrix().GetPos();			
				
			}
			break;
		}

#ifdef SPECIAL_USE_ITEM
		// oasis907 : 김상윤 [2010.2.10] // 
		if(pData->bApplyRotatedOffset == true)
		{
			D3DXVECTOR3 vDirVector = pMeshInst->GetDirVec();

			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}
			if( 0.f != pData->vOffset.x )
			{
				vPosition += vDirVector * pData->vOffset.x;
			}
			if( 0.f != pData->vOffset.y )
			{
				vPosition.y += pData->vOffset.y;
			}
			if( 0.f != pData->vOffset.z )
			{
				if( vDirVector.y != 0.f )
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}
				else
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
				}
				vPosition += vDirVector * pData->vOffset.z;
			}
		}
		else
		{
			vPosition += pData->vOffset;
		}
#else
		vPosition += pData->vOffset;
#endif SPECIAL_USE_ITEM

		if( m_pManager != NULL )
		{
			float fLandPos = min( m_fLandPos, vPosition.y );

#ifdef INHERIT_FORCEDOWNRATE
			float fForceDownRate = 1.f;
			if(true == m_bInheritForceDownRate)
			{
				fForceDownRate *= m_fForceDownRate;
			}
			CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
															vPosition, vRotateAngleDegree, vAxisAngleDegree, 
															m_fLandPos, false, -1.f, fForceDownRate);
#else
			CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
															vPosition, vRotateAngleDegree, vAxisAngleDegree, 
															fLandPos, false );
#endif INHERIT_FORCEDOWNRATE

			if( pEffect != NULL )
			{
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				// June2017 FUN_31f810c0 @31f8198f: 31f8fdb0 (the parent's state to the child) comes right after the crit inherit 31f8af50
				// and before INHERIT_BUFF_FACTOR. March's crit block sits after the buff-factor copy here; the fields are independent.
				JuneInheritFromParent( pEffect );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				/// 버프팩터를 상속함
				if ( pData->bInheritBuffFactor && !GetDamageData().m_vecBuffFactorPtr.empty() )
					pEffect->GetDamageData().m_vecBuffFactorPtr = GetDamageData().m_vecBuffFactorPtr;
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f810c0 (CreateHitDamageEffect, reached from AttackResult through SetHitDamageEffect 31f831e0) -> FUN_31f8af50 @31f81987: INHERIT_CRITICAL_RATE /
				// INHERIT_CRITICAL_DAMAGE_RATE add the parent's CRITICAL_RATE / APPLY_CRITICAL_DAMAGE_PERCENT
				// onto the child's own (DamageData +0x510 / +0x59c, SetDamageDataFromLUA 32632390@3263c197 /
				// @3263e629). Only the hit, die and touch-land spawners call it, not the
				// CREATE_DAMAGE_EFFECT loop or RT_CREATE_DAMAGE_EFFECT. @31f819bd-31f819fe: INHERIT_LIFE_TIME
				// puts the child main effect on the parent's life clock, as the CREATE_DAMAGE_EFFECT loop does.
				if( true == m_bInheritCriticalRate )
					pEffect->GetDamageData().m_fCriticalRate += m_DamageData.m_fCriticalRate;
				if( true == m_bInheritCriticalDamageRate )
					pEffect->GetDamageData().m_fApplyCriticalDamage += m_DamageData.m_fApplyCriticalDamage;
				if( true == pData->bInheritLifeTime )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pParentMainEffect = pMeshInst;
					CKTDGXMeshPlayer::CXMeshInstance* pChildMainEffect = pEffect->GetMainEffect();
					if( NULL != pParentMainEffect && NULL != pChildMainEffect )
						pChildMainEffect->SetNowLifeTime( pParentMainEffect->GetNowLifeTime() );
				}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				if( true == m_bInheritCriticalRate || true == m_bInheritCriticalDamageRate || true == pData->bInheritLifeTime )
					JuneFxHit( GetName(), L"HIT_DAMAGE_EFFECT_INHERIT", L"%s critRate=%d->%.3f critDamage=%d->%.3f lifeTime=%d",
						pData->damageEffectName.c_str(), (int)m_bInheritCriticalRate, pEffect->GetDamageData().m_fCriticalRate,
						(int)m_bInheritCriticalDamageRate, pEffect->GetDamageData().m_fApplyCriticalDamage, (int)pData->bInheritLifeTime );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef NEW_SKILL_2010_11
	#ifdef SERV_RENA_NIGHT_WATCHER
				if( 0.f < GetDamageData().m_fHitDamageEffectDamageRate )
	#endif SERV_RENA_NIGHT_WATCHER
				{
					// oasis907 : 김상윤 [2010.11.4] // 그랜드 아처 - 진동 사격(패시브)
					pEffect->GetDamageData().damage.fPhysic *= GetDamageData().m_fHitDamageEffectDamageRate;
					pEffect->GetDamageData().damage.fMagic	*= GetDamageData().m_fHitDamageEffectDamageRate;
				}
#endif NEW_SKILL_2010_11

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f810c0 @31f81a4f-31f81a9e -> 31f6fdc0 (June CEffect::SetScale: main, passive, hit and
				// die scales): the child takes this entry's SCALE_X/Y/Z times the parent's hit damage-effect
				// scale, component by component. March parsed the entry scale and never applied it (Phase 7d).
				pEffect->SetScale( D3DXVECTOR3( pData->vScale.x * m_vHitDamageScale.x, pData->vScale.y * m_vHitDamageScale.y, pData->vScale.z * m_vHitDamageScale.z ) );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				if( D3DXVECTOR3( 1.f, 1.f, 1.f ) != pData->vScale )
					JuneFxHit( GetName(), L"HIT_DAMAGE_EFFECT_SCALE", L"%s entry=(%.3f,%.3f,%.3f) parent=(%.3f,%.3f,%.3f)",
						pData->damageEffectName.c_str(), pData->vScale.x, pData->vScale.y, pData->vScale.z, m_vHitDamageScale.x, m_vHitDamageScale.y, m_vHitDamageScale.z );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				pEffect->SetScale( m_vHitDamageScale );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
				pEffect->SetScaleByUnit( m_vHitDamageScaleByUnit );
#endif GIANT_UNIT_GIANT_EFFECT_TEST

#ifdef INHERIT_FORCEDOWNRATE
				pEffect->SetInheritForceDownRate(m_bInheritForceDownRate);
#endif INHERIT_FORCEDOWNRATE

#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInstHitEffect = pEffect->GetMainEffect();
				if( NULL != pMeshInstHitEffect && pData->fLifeTime > 0.f )
					pMeshInstHitEffect->SetMaxLifeTime( pData->fLifeTime );
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

#ifdef ADD_RENA_SYSTEM //김창한
				//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
				pEffect->GetDamageData().m_eFirstAttack = m_DamageData.m_eFirstAttack;
				pEffect->GetDamageData().m_RelateSkillData = m_DamageData.m_RelateSkillData;
#endif //ADD_RENA_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				// June2017 FUN_31f810c0 @31f81afd-31f81b35: the post-create tail, after the first-attack copy: 31f649d0 (the entry's +0xb0
				// down the child's tree), 31f64c00, then 31f65e40 / 31f65f90 when the entry carries +0xb4 / +0xb5.
				JuneSpawnTail( pEffect, pData );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
			}
		}

	}

	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
	// 기존의 damage effect 테이블을 다시 열어놓는다
	m_pLuaManager->BeginTable( this->GetName() );
#endif	DAMAGE_EFFECT_WORK		
	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

}


void CX2DamageEffect::CEffect::CreateDieDamageEffect( vector<DamageEffectData*>& vecDieDamageEffect )
{
	KTDXPROFILE();

	if( NULL == g_pX2Game || null == m_optrOwnerUnit )
	{
		return;
	}

	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
	// 현재 열려있는 damage effect 테이블을 잠시 닫고 
	m_pLuaManager->EndTable();
#endif	DAMAGE_EFFECT_WORK		
	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

	for( UINT i=0; i<vecDieDamageEffect.size(); i++ )
	{
		DamageEffectData* pData = vecDieDamageEffect[i];



		D3DXVECTOR3 vAxisAngleDegree( 0, 0, 0);
		D3DXVECTOR3 vRotateAngleDegree( 0, 0, 0);
		D3DXVECTOR3 vPosition(0,0,0);


		switch( pData->posType )
		{
		case DEPT_BONE:
			{
				//{{ JHKang / 강정훈 / 2010/11/24 / 이펙트 종료 시 본 선택
#ifdef NEW_SKILL_2010_11
				if ( false == g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstHandle ) )
					return;

				CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
				ASSERT( pMeshInst != NULL );
				if( pMeshInst == NULL )
					return;

				if( pData->bApplyRotate == true )
				{
					vAxisAngleDegree = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
					vRotateAngleDegree = vAxisAngleDegree;
				}
				else
				{
					vAxisAngleDegree = pData->vRotateAngleDegree;
					vRotateAngleDegree = vAxisAngleDegree;
				}
				vPosition = pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() ); 
#else
				// not implemented yet!!
				continue;

				//if( pData->bApplyRotate == true )
				//{
				//	vAxisAngleDegree = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();
				//	vRotateAngleDegree = vAxisAngleDegree;
				//}
				//vPosition = pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() );
#endif NEW_SKILL_2010_11
				//}} JHKang / 강정훈 / 2010/11/24
			}
			break;

		case DEPT_UNIT_BONE:
			{
				if( null != m_optrOwnerUnit )
				{
					vAxisAngleDegree = m_optrOwnerUnit->GetRotateDegree();
					if( pData->bApplyRotate == true )
					{
						vRotateAngleDegree = vAxisAngleDegree;
					}

					vPosition = m_optrOwnerUnit->GetBonePos( pData->boneName.c_str() );
				}
			}
			break;

		case DEPT_IMPACT_POINT:
			{
#ifdef NEW_SKILL_2010_11 
				//{{ oasis907 : 김상윤 [2010.11.22] // 그랜드 아처 - 진동 사격(패시브)
				if(pData->bApplyRotateOrigin == true)
				{
					vAxisAngleDegree = m_AngleDegree;
					vRotateAngleDegree = vAxisAngleDegree;
				}
				//}} bApplyRotate 도 처리하고 싶지만 기본으로 true가 되어있어 기존 데미지 이펙트에 미칠 영향이 커 처리 안함.
#endif NEW_SKILL_2010_11
				vPosition = m_DamageData.impactPoint;
			}
			break;
#ifdef RAVEN_WEAPON_TAKER
		case DEPT_DIE_POS:
			{
				if( g_pX2Game->GetMajorXMeshPlayer() &&
					INVALID_MESH_INSTANCE_HANDLE != m_hMeshInstHandle )
				{
					vPosition = m_vLandPositionOnLineMap;

					if( vPosition.y < 0.f )
						continue;

					if( pData->bApplyRotate == true )
					{
						vAxisAngleDegree = m_vMainEffectAxisAngleDegree;
						vRotateAngleDegree = vAxisAngleDegree;
					}
					else
					{
						vAxisAngleDegree = pData->vRotateAngleDegree;
						vRotateAngleDegree = vAxisAngleDegree;
					}
				}
				else
					vPosition = D3DXVECTOR3( 0.f, -1.f, 0.f );
			}
			break;
#endif RAVEN_WEAPON_TAKER

		case DEPT_POS:
			{
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
				if( pData->bOnlyApplyRotateOffset == true )
				{
					vAxisAngleDegree = m_vMainEffectAxisAngleDegree;
					vRotateAngleDegree = pData->vRotateAngleDegree + vAxisAngleDegree;
				}
				else
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE
				{
					if( pData->bApplyRotate == true )
					{
						vAxisAngleDegree = m_vMainEffectAxisAngleDegree;
						vRotateAngleDegree = vAxisAngleDegree;
					}
					else
					{
						vAxisAngleDegree = pData->vRotateAngleDegree;
						vRotateAngleDegree = vAxisAngleDegree;
					}
				}

				vPosition = m_vMainEffectPosition;
			}
			break;
		}

#ifdef SPECIAL_USE_ITEM
		// oasis907 : 김상윤 [2011.12.20]
		if(pData->bApplyRotatedOffset == true)
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
			D3DXVECTOR3	vDirVector = D3DXVECTOR3(0.f, 0.f, 0.f);
			if( pMeshInst == NULL)
			{
				if( g_pX2Game->GetWorld() != NULL && g_pX2Game->GetWorld()->GetLineMap() != NULL )
				{
					D3DXVECTOR3 outPos;
					int			lineIndex = 0;
					if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( GetPos() , LINE_RADIUS, &outPos, &lineIndex, false ) == true )
					{
						const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( lineIndex );
						if( pLineData != NULL )
						{
							vDirVector = pLineData->dirVector;
							if( false == m_bFollowLineRight )
							{
								vDirVector.x = -vDirVector.x;
								vDirVector.z = -vDirVector.z;
							}
						}
					}
				}
			}
			else
			{
				vDirVector = pMeshInst->GetDirVec();
			}

			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}
			if( 0.f != pData->vOffset.x )
			{
				vPosition += vDirVector * pData->vOffset.x;
			}
			if( 0.f != pData->vOffset.y )
			{
				vPosition.y += pData->vOffset.y;
			}
			if( 0.f != pData->vOffset.z )
			{
				if( vDirVector.y != 0.f )
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}
				else
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
				}
				vPosition += vDirVector * pData->vOffset.z;
			}
		}
		else
		{
			vPosition += pData->vOffset;
		}
#else
		vPosition += pData->vOffset;
#endif SPECIAL_USE_ITEM

		if( m_pManager != NULL )
		{
			float fLandPos = min( m_fLandPos, vPosition.y );

#ifdef INHERIT_FORCEDOWNRATE
			float fForceDownRate = 1.f;
			if(true == m_bInheritForceDownRate)
			{
				fForceDownRate *= m_fForceDownRate;
			}
#ifdef SERV_RENA_NIGHT_WATCHER
			CEffect* pEffect = NULL;
			if( pData->bLandCrashOnly == true || pData->bLifeTimeOnly == true )
			{
				if( (pData->bLandCrashOnly == true && g_pX2Game->GetMajorXMeshPlayer()->IsDyingByCrash( m_hMeshInstHandle ) == true ) ||
					(pData->bLifeTimeOnly == true && g_pX2Game->GetMajorXMeshPlayer()->IsDyingByCrash( m_hMeshInstHandle ) == false ) )
				{
					pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
						vPosition, vRotateAngleDegree, vAxisAngleDegree, 
						fLandPos, false, -1.f, fForceDownRate);
				}
			}   
			else
			{
				pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
					vPosition, vRotateAngleDegree, vAxisAngleDegree, 
					fLandPos, false, -1.f, fForceDownRate);
			}



#else
			CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
															vPosition, vRotateAngleDegree, vAxisAngleDegree, 
															fLandPos, false, -1.f, fForceDownRate);
#endif SERV_RENA_NIGHT_WATCHER

#else
			CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
															vPosition, vRotateAngleDegree, vAxisAngleDegree, 
															fLandPos, false );
#endif INHERIT_FORCEDOWNRATE
			if( pEffect != NULL )
			{	
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				// June2017 FUN_31f82210 @31f82f3e: 31f8fdb0 right after the crit inherit 31f8af50, before INHERIT_BUFF_FACTOR (as the hit spawner).
				// It copies m_iIndex too, which March already did for die children under SERV_RENA_NIGHT_WATCHER below.
				JuneInheritFromParent( pEffect );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				/// 버프팩터를 상속함
				if ( pData->bInheritBuffFactor && !GetDamageData().m_vecBuffFactorPtr.empty() )
					pEffect->GetDamageData().m_vecBuffFactorPtr = GetDamageData().m_vecBuffFactorPtr;
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f82210 (CreateDieDamageEffect, reached from the die paths through 31f83400) -> FUN_31f8af50 @31f82f36: INHERIT_CRITICAL_RATE /
				// INHERIT_CRITICAL_DAMAGE_RATE add the parent's CRITICAL_RATE / APPLY_CRITICAL_DAMAGE_PERCENT
				// onto the child's own (DamageData +0x510 / +0x59c, SetDamageDataFromLUA 32632390@3263c197 /
				// @3263e629). Only the hit, die and touch-land spawners call it, not the
				// CREATE_DAMAGE_EFFECT loop or RT_CREATE_DAMAGE_EFFECT. @31f82f74-31f82fb5: INHERIT_LIFE_TIME
				// puts the child main effect on the parent's life clock, as the CREATE_DAMAGE_EFFECT loop does.
				if( true == m_bInheritCriticalRate )
					pEffect->GetDamageData().m_fCriticalRate += m_DamageData.m_fCriticalRate;
				if( true == m_bInheritCriticalDamageRate )
					pEffect->GetDamageData().m_fApplyCriticalDamage += m_DamageData.m_fApplyCriticalDamage;
				if( true == pData->bInheritLifeTime )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pParentMainEffect = GetMainEffect();
					CKTDGXMeshPlayer::CXMeshInstance* pChildMainEffect = pEffect->GetMainEffect();
					if( NULL != pParentMainEffect && NULL != pChildMainEffect )
						pChildMainEffect->SetNowLifeTime( pParentMainEffect->GetNowLifeTime() );
				}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				if( true == m_bInheritCriticalRate || true == m_bInheritCriticalDamageRate || true == pData->bInheritLifeTime )
					JuneFxHit( GetName(), L"DIE_DAMAGE_EFFECT_INHERIT", L"%s critRate=%d->%.3f critDamage=%d->%.3f lifeTime=%d",
						pData->damageEffectName.c_str(), (int)m_bInheritCriticalRate, pEffect->GetDamageData().m_fCriticalRate,
						(int)m_bInheritCriticalDamageRate, pEffect->GetDamageData().m_fApplyCriticalDamage, (int)pData->bInheritLifeTime );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f82210 @31f82fb5-31f83010 -> 31f6fdc0 (June CEffect::SetScale: main, passive, hit and
				// die scales): the child takes this entry's SCALE_X/Y/Z times the parent's die damage-effect
				// scale, component by component. March parsed the entry scale and never applied it (Phase 7d).
				pEffect->SetScale( D3DXVECTOR3( pData->vScale.x * m_vDieDamageScale.x, pData->vScale.y * m_vDieDamageScale.y, pData->vScale.z * m_vDieDamageScale.z ) );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				if( D3DXVECTOR3( 1.f, 1.f, 1.f ) != pData->vScale )
					JuneFxHit( GetName(), L"DIE_DAMAGE_EFFECT_SCALE", L"%s entry=(%.3f,%.3f,%.3f) parent=(%.3f,%.3f,%.3f)",
						pData->damageEffectName.c_str(), pData->vScale.x, pData->vScale.y, pData->vScale.z, m_vDieDamageScale.x, m_vDieDamageScale.y, m_vDieDamageScale.z );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				pEffect->SetScale( m_vDieDamageScale );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
				pEffect->SetScaleByUnit( m_vDieDamageScaleByUnit );
#endif GIANT_UNIT_GIANT_EFFECT_TEST

#ifdef INHERIT_FORCEDOWNRATE
				pEffect->SetInheritForceDownRate(m_bInheritForceDownRate);
#endif INHERIT_FORCEDOWNRATE

#ifdef SERV_RENA_NIGHT_WATCHER
				pEffect->SetIndex( GetIndex() );					/// 인덱스 상속
				pEffect->SetType( GetType() );						/// 타입 상속

				if( DET_NONE != GetType() )		/// 설정된 타입에 해당하는 동작 검사
				{
					if( null != GetDamageData().optrAttackerGameUnit )
						GetDamageData().optrAttackerGameUnit->CheckDamageEffectType( pEffect );
				}
#endif SERV_RENA_NIGHT_WATCHER

#ifdef SERV_RENA_NIGHT_WATCHER
				AddDieEffectDamage( pEffect );
#endif SERV_RENA_NIGHT_WATCHER

#ifdef SERV_RAVEN_VETERAN_COMMANDER
				if( -1 != pData->iGroupID )		/// 그룹 데미지 설정
					pEffect->SetLaserGroupID( pData->iGroupID );
#endif SERV_RAVEN_VETERAN_COMMANDER
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInstDieEffect = pEffect->GetMainEffect();
				if( NULL != pMeshInstDieEffect && pData->fLifeTime > 0.f )
					pMeshInstDieEffect->SetMaxLifeTime( pData->fLifeTime );
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

#ifdef ADD_RENA_SYSTEM //김창한
				//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
				pEffect->GetDamageData().m_eFirstAttack = m_DamageData.m_eFirstAttack;
				pEffect->GetDamageData().m_RelateSkillData = m_DamageData.m_RelateSkillData;
#endif //ADD_RENA_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f82210 @31f83110-31f83126 (effect completion C7): INHERITLOCKONDATA (entry +0x9c, DIE entries only) gives
				// the child the parent's LockOnData through June's LockOnData::operator= (31f8c280): the child's own lock-on type is not
				// kept and the parent's type is not tested (unlike the CREATE_DAMAGE_EFFECT loop's copy), and the child keeps its own
				// pre-lock-on hit set. March parsed the key and never read it.
				if( true == pData->bInheritLockOnData )
				{
					JuneAssignLockOnData( pEffect->GetLockOnData(), GetLockOnData() );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxHit( GetName(), L"INHERITLOCKONDATA", L"%s lockOnType=%d unit=%I64d npc=%d", pData->damageEffectName.c_str(),
						(int)pEffect->GetLockOnData().m_LockOnType, pEffect->GetLockOnData().m_LockOnUnitUID, pEffect->GetLockOnData().m_LockOnNPCUID );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
				// June2017 FUN_31f82210 @31f83137-31f8316f: the same tail as the hit spawner, after the first-attack copy.
				JuneSpawnTail( pEffect, pData );
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
			}
		}
	}

	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)
#ifndef	DAMAGE_EFFECT_WORK		
	// 기존의 damage effect 테이블을 다시 열어놓는다
	m_pLuaManager->BeginTable( this->GetName() );
#endif	DAMAGE_EFFECT_WORK		
	//{{ kimhc // 2009-10-20 // CEffect 전과 후에 BeginTable, EndTable을 호출 하므로 삭제,(일명 독구름 작업)

}



void CX2DamageEffect::CEffect::SetParticleData( ParticleData* pData )
{
	KTDXPROFILE();

	if ( g_pX2Game == NULL || pData == NULL || pData->m_hSeq == INVALID_PARTICLE_SEQUENCE_HANDLE )
		return;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pData->m_hSeq );
	if( NULL == pSeq )
	{
		pData->m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
		return; 
	}


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
	{
		return;
	}

	//{{ kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록
#ifdef	EVE_SECOND_CLASS_CHANGE
	m_vMoveAngle += pData->vMoveAnglePerFrame;
#endif	EVE_SECOND_CLASS_CHANGE
	//}} kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록



	switch( pData->posType )
	{
	case DEPT_BONE:
		{
			D3DXVECTOR3 angleDegree = pMeshInst->GetXSkinAnim()->GetMatrix().GetRotateDegree();

			//{{ kimhc // 2010.2.3 //	이브 2차 전직
#ifdef	EVE_SECOND_CLASS_CHANGE
			angleDegree += pData->vRotateAngleDegree;
#endif	EVE_SECOND_CLASS_CHANGE
			//}} kimhc // 2010.2.3 //	이브 2차 전직

			
			//if ( pData->bApplyRotatedOffset == false )
			{
				if( pData->bApplyRotate == true )
				{
					//{{ kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록
#ifdef	EVE_SECOND_CLASS_CHANGE
					angleDegree	+= m_vMoveAngle;
#endif	EVE_SECOND_CLASS_CHANGE
					//}} kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록

					pSeq->SetAxisAngle( angleDegree );
					pSeq->SetAddRotate( angleDegree );
				}
			}
						

			// modified by wonpok.
			//{{ lucidash // 2010.11.24 // particle의 offset 을 rotation direction을 고려 해서 더하기.
			//if(pData->bTrace == true)
			//{
				D3DXVECTOR3 framePosition = pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() );
				D3DXVECTOR3 vPosition(0,0,0);

				if ( pData->bApplyRotatedOffset == true )
				{
					D3DXVECTOR3 vDirVector = pMeshInst->GetDirVec();
					

					if( 0.f != vDirVector.y )
					{
						vDirVector.y = 0.f;
						D3DXVec3Normalize( &vDirVector, &vDirVector );
					}
					if( 0.f != pData->fOffsetX )
					{
						vPosition += vDirVector * pData->fOffsetX;
					}
					if( 0.f != pData->fOffsetY )
					{
						vPosition.y += pData->fOffsetY;
					}
					if( 0.f != pData->fOffsetZ )
					{
						if( vDirVector.y != 0.f )
						{
							vDirVector.y = vDirVector.z;
							vDirVector.z = vDirVector.x;
							vDirVector.x = -vDirVector.y;
							vDirVector.y = 0.f;
							D3DXVec3Normalize( &vDirVector, &vDirVector );
						}
						else
						{
							vDirVector.y = vDirVector.z;
							vDirVector.z = vDirVector.x;
							vDirVector.x = -vDirVector.y;
							vDirVector.y = 0.f;
						}
						vPosition += vDirVector * pData->fOffsetZ;
					}

					/*pData->fOffsetX = vPosition.x;
					pData->fOffsetY = vPosition.y;
					pData->fOffsetZ = vPosition.z;*/
				}
				else
				{
					vPosition = D3DXVECTOR3( pData->fOffsetX ,pData->fOffsetY , pData->fOffsetZ );
				}
				

				//framePosition += D3DXVECTOR3(pData->fOffsetX, pData->fOffsetY, pData->fOffsetZ);
				framePosition += vPosition;
#ifdef EVE_ELECTRA
				pSeq->SetPosition(framePosition, pData->bTraceMore, pData->bTraceMore ); 
#else
				pSeq->SetPosition(framePosition); 
#endif EVE_ELECTRA
			//}		
			//}} lucidash // 2010.11.24 // particle의 offset 을 rotation direction을 고려 해서 더하기.

			//pSeq->SetPosition( pMeshInst->GetXSkinAnim()->GetCloneFramePosition( pData->boneName.c_str() ) );

#ifdef BALANCE_PATCH_20120329
				if( pData->fShowLandHeight <= 50000.f && g_pX2Game->GetWorld() != NULL && g_pX2Game->GetWorld()->GetLineMap() != NULL )
				{
					int iLineIndex = 0;
					D3DXVECTOR3 vPos = g_pX2Game->GetWorld()->GetLineMap()->GetLandPosition( framePosition, 3.f, &iLineIndex );

					if( iLineIndex < 0 || framePosition.y - vPos.y > pData->fShowLandHeight )
						pSeq->SetShowObject(false);
					else
						pSeq->SetShowObject(true);
				}			
#endif
		}
		

		break;

	case DEPT_UNIT_BONE:
		{
			if( null != m_optrOwnerUnit )
			{
				

				//{{ kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록
#ifdef	EVE_SECOND_CLASS_CHANGE
				D3DXVECTOR3 angleDegree = m_optrOwnerUnit->GetRotateDegree();
				angleDegree += pData->vRotateAngleDegree;

				pSeq->SetAxisAngle( angleDegree + m_vMoveAngle );
				if( pData->bApplyRotate == true )
					pSeq->SetAddRotate( angleDegree + m_vMoveAngle );
#else	EVE_SECOND_CLASS_CHANGE
				pSeq->SetAxisAngle( m_optrOwnerUnit->GetRotateDegree() );
				if( pData->bApplyRotate == true )
					pSeq->SetAddRotate( m_optrOwnerUnit->GetRotateDegree() );
#endif	EVE_SECOND_CLASS_CHANGE
				//}} kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록
				
				//if(pData->bTrace == true)
#ifdef EVE_ELECTRA
					pSeq->SetPosition( m_optrOwnerUnit->GetBonePos( pData->boneName.c_str() ), pData->bTraceMore, pData->bTraceMore ); 
#else
					pSeq->SetPosition( m_optrOwnerUnit->GetBonePos( pData->boneName.c_str() ) );
#endif EVE_ELECTRA
			}
		}
		break;

	case DEPT_IMPACT_POINT:
		{
			pSeq->SetPosition( m_DamageData.impactPoint );
#ifdef HIT_PARTICLE_TRACE_UNIT
			if( pData->bTraceDefenderUnit == true)
			{
				if( null != m_DamageData.optrDefenderGameUnit  )
				{
					D3DXVECTOR3 vEffectPos = m_DamageData.optrDefenderGameUnit->GetPos();
					vEffectPos.y += 110.f;
#ifdef EVE_ELECTRA
					pSeq->SetPosition( vEffectPos, pData->bTraceMore, pData->bTraceMore ); 
#else
					pSeq->SetPosition( vEffectPos );
#endif EVE_ELECTRA
					
				}
			}
#endif HIT_PARTICLE_TRACE_UNIT
		}
		break;

	case DEPT_POS:
		{
			D3DXVECTOR3 angleDegree = pMeshInst->GetMatrix().GetRotateDegree();

			//{{ kimhc // 2010.2.3 //	이브 2차 전직
#ifdef	EVE_SECOND_CLASS_CHANGE
			angleDegree += pData->vRotateAngleDegree;
#endif	EVE_SECOND_CLASS_CHANGE
			//}} kimhc // 2010.2.3 //	이브 2차 전직

			if( pData->bApplyRotate == true )
			{
				//{{ kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록
#ifdef	EVE_SECOND_CLASS_CHANGE
				angleDegree	+= m_vMoveAngle;
#endif	EVE_SECOND_CLASS_CHANGE
				//}} kimhc // 2010.2.3 //	매 프레임 Rotate Angle이 바뀌도록

				pSeq->SetAxisAngle( angleDegree );
				pSeq->SetAddRotate( angleDegree );
			}
#ifdef EVE_ELECTRA
			pSeq->SetPosition( pMeshInst->GetMatrix().GetPos(), pData->bTraceMore, pData->bTraceMore ); 
#else
			pSeq->SetPosition( pMeshInst->GetMatrix().GetPos() );
#endif EVE_ELECTRA
			
		}
		break;

	default:
		break;
	}

	pSeq->SetScaleFactor( pData->vScale );
#ifdef EVE_ELECTRA
	if( pData->bScaleByMainEffectScaleY == true )
	{
		float fScaleY = pMeshInst->GetScale().y;
		pSeq->SetScaleFactor(D3DXVECTOR3( fScaleY, fScaleY, 1.f) );
	}
#endif EVE_ELECTRA


#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
	pSeq->SetScaleByUnit( pMeshInst->GetScaleByUnit() );
#endif GIANT_UNIT_GIANT_EFFECT_TEST


#ifdef ARA_CHANGE_CLASS_FIRST
	if( true == pData->m_bReverseY && 
		CKTDGMatrix::BT_NONE != pSeq->GetBillBoardType() )
	{
		if( null != m_optrOwnerUnit && false == m_optrOwnerUnit->GetIsRight() )
		{
			pSeq->SetReverseY( true );
		}
	}
#endif // ARA_CHANGE_CLASS_FIRST

}





void CX2DamageEffect::CEffect::FollowLine()
{
	D3DXVECTOR3 outPos;
	int			lineIndex = 0;

    if ( g_pX2Game == NULL )
        return;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
	{
		return;
	}

	if( g_pX2Game->GetWorld() != NULL && g_pX2Game->GetWorld()->GetLineMap() != NULL )
	{
		bool bUpdateLandPos = false;
		float fLineRadius = LINE_RADIUS;
		if( IsSamef( pMeshInst->GetTempletData()->elasticCoeffX.GetRandomNumInRange(), 0.f ) == false )
		{
			bUpdateLandPos = true;
			fLineRadius -= 15.f;
		}

		if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( pMeshInst->GetPos() , fLineRadius, &outPos, &lineIndex, false ) == true )
		{
			const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( lineIndex );
			if( pLineData != NULL )
			{
				D3DXVECTOR3 angle = pLineData->dirDegree;
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				// June2017 FUN_31f70ee0 (FollowLine) @31f711b8-31f7121d: FOLLOW_LINE_WITH_CUSTOM_ROTATE
				// (+0xabd) keeps the main effect's own x and z rotation (CXMeshInstance m_vRotate,
				// +0x5b4/+0x5bc) where March uses zeroes. The line's y direction is used either way.
				if( true == m_bFollowLineWithCustomRotate )
				{
					angle.x = pMeshInst->GetRotateDegree().x;
					angle.z = pMeshInst->GetRotateDegree().z;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxCeFrame( GetName(), L"FOLLOW_LINE_WITH_CUSTOM_ROTATE", L"kept own x/z rotation" );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
				else
				{
					angle.x = 0.0f;
					angle.z = 0.0f;
				}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				angle.x = 0.0f;
				angle.z = 0.0f;
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
				if( m_bFollowLineRight == false )
				{
					angle.y += 180.0f;
				}
				pMeshInst->SetRotateDegree( angle );
				pMeshInst->SetMoveAxisAngleDegree( angle );

				if( IsSamef(m_BeforePos.x, outPos.x) == false || IsSamef(m_BeforePos.z, outPos.z) == false )
				{
					//pMeshInst->SetLandPosition( outPos.y );

					#ifdef RAVEN_WEAPON_TAKER
					if( true == m_bMustOnLine )
					{
						if( pMeshInst->GetPos().y - outPos.y > 200 )
						{
							pMeshInst->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );
							pMeshInst->SetDirSpeed( 0.f );
							pMeshInst->PlayEnd();
						}
					}
					#endif RAVEN_WEAPON_TAKER
					if( true == m_bFollowLineNew )
					{
						if( fabs(outPos.y - pMeshInst->GetPos().y) > 150.f )
						{
							outPos.y = pMeshInst->GetPos().y;
						}
					}
					else
					{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
						// June2017 FUN_31f70ee0 @31f71359-31f713c6: FOLLOW_LINE_GRAVITY (+0x138). When the line is
						// within 30 of the main effect (|outPos.y - pos.y| < 30.0, 0x33439c68), June adds 20.0
						// (0x334307f0) to outPos.y and clears the mesh's move-enable flag. That flag is June mesh
						// +0x633; CXMeshInstance::OnFrameMove 31c87b80 reads it to zero the velocity before
						// integrating. Farther away, June runs this same March assignment and sets the flag back
						// to 1. March's CXMeshInstance has no such flag, and the +20 without the freeze would be
						// a behaviour neither build has. So the near-line case is NOT transcribed: it is deferred
						// to Phase 10 (KTDGXMeshPlayer) and logged, and the March assignment below runs for both.
						if( true == m_bFollowLineGravity && fabs( outPos.y - pMeshInst->GetPos().y ) < 30.f )
							JuneFxCeOnce( L"deferred", GetName(), L"FOLLOW_LINE_GRAVITY", L"near-line case needs the CXMeshInstance move-enable flag (June mesh +0x633), Phase 10" );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
						outPos.y = pMeshInst->GetPos().y;
					}

					pMeshInst->SetPos( outPos );
				}				
#ifdef ADD_UPDATE_LANDPOS
				else if( bUpdateLandPos == true && pMeshInst->GetToggleRotae() == false )
				{
					pMeshInst->SetToggleRotate(true);
					m_bFollowLineRight = !m_bFollowLineRight;
					D3DXVECTOR3 vRot = pMeshInst->GetRotateDegree();
					if( m_bFollowLineRight == false )
					{
						vRot.y += 180.f;						
					}
					else
					{
						vRot.y -= 180.f;
					}
					pMeshInst->SetRotateDegree( vRot );
					pMeshInst->SetMoveAxisAngleDegree( vRot );
				}
#endif
			}
		}
#ifdef SKILL_CASH_10_TEST
		else
		{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			// June2017 FUN_31f70ee0 @31f71402-31f717e5: with FOLLOW_ON_LINE (+0xabc) and FOLLOW_LINE_GRAVITY
			// (+0x138) both set, June skips March's FOLLOW_ON_LINE step below. Instead it scans every line
			// of the line map, skipping disabled ones and LT_WALL (1) / LT_MONSTER_ROAD (7) (June's
			// LINE_TYPE values, which are March's). On each segment it takes the point nearest the main
			// effect: the start if the projection onto the normalized direction is <= 0, the end if it
			// is >= the segment length, else start + dir * t. It keeps the nearest of those; a later
			// line wins a tie, and the running minimum starts at 9999999.0 (0x3344915c). If a point was
			// found the main effect goes there, 100.0 higher (0x3342f360). Otherwise it goes to
			// m_BeforePos at its own current height. Either way June then goes straight to the
			// MUST_ON_LINE check below and skips the toggle-rotate step. June's LineData reads (type
			// +0x4, enable +0x8, start +0x24, end +0x30) match March's CKTDGLineMap::LineData field for
			// field. 33086a02 is taken as D3DXVec3Normalize and 32fe2370 as sqrt from how they are used:
			// their IAT slots hold dump-time DLL addresses, so the names cannot be read off.
			if( true == m_bFollowOnLine && true == m_bFollowLineGravity )
			{
				CKTDGLineMap* pJuneLineMap = g_pX2Game->GetWorld()->GetLineMap();
				const D3DXVECTOR3 vJuneMeshPos = pMeshInst->GetPos();
				float fJuneNearestDistance = 9999999.f;
				int iJuneNearestLine = -1;
				D3DXVECTOR3 vJuneNearestPos( 0.f, 0.f, 0.f );
				for( int iJuneLine = 0; iJuneLine < pJuneLineMap->GetNumLineData(); iJuneLine++ )
				{
					const CKTDGLineMap::LineData* pJuneLineData = pJuneLineMap->GetLineData( iJuneLine );
					if( NULL == pJuneLineData || false == pJuneLineData->bEnable
						|| CKTDGLineMap::LT_MONSTER_ROAD == pJuneLineData->lineType
						|| CKTDGLineMap::LT_WALL == pJuneLineData->lineType )
						continue;

					D3DXVECTOR3 vJuneSegment = pJuneLineData->endPos - pJuneLineData->startPos;
					D3DXVECTOR3 vJuneSegmentDir;
					D3DXVec3Normalize( &vJuneSegmentDir, &vJuneSegment );
					const D3DXVECTOR3 vJuneToMesh = vJuneMeshPos - pJuneLineData->startPos;
					const float fJuneProjection = D3DXVec3Dot( &vJuneToMesh, &vJuneSegmentDir );
					D3DXVECTOR3 vJuneCandidate;
					if( fJuneProjection <= 0.f )
						vJuneCandidate = pJuneLineData->startPos;
					else if( D3DXVec3Length( &vJuneSegment ) <= fJuneProjection )
						vJuneCandidate = pJuneLineData->endPos;
					else
						vJuneCandidate = pJuneLineData->startPos + vJuneSegmentDir * fJuneProjection;

					const D3DXVECTOR3 vJuneToCandidate = vJuneCandidate - vJuneMeshPos;
					const float fJuneDistance = D3DXVec3Length( &vJuneToCandidate );
					if( fJuneDistance <= fJuneNearestDistance )
					{
						fJuneNearestDistance	= fJuneDistance;
						iJuneNearestLine		= iJuneLine;
						vJuneNearestPos			= vJuneCandidate;
					}
				}

				// Code review fix: iJuneNearestLine is only ever assigned from iJuneLine, already
				// bounds-checked and non-NULL by the loop above, so re-checking both here was dead.
				if( iJuneNearestLine >= 0 )
				{
					vJuneNearestPos.y += 100.f;
					pMeshInst->SetPos( vJuneNearestPos );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					WCHAR wszJuneFxGravity[96] = L"";
					StringCchPrintfW( wszJuneFxGravity, ARRAY_SIZE(wszJuneFxGravity), L"nearest line=%d distance=%.1f", iJuneNearestLine, fJuneNearestDistance );
					JuneFxCeFrame( GetName(), L"FOLLOW_LINE_GRAVITY", wszJuneFxGravity );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
				else
				{
					m_BeforePos.y = pMeshInst->GetPos().y;
					pMeshInst->SetPos( m_BeforePos );
				}
			}
			else
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
			if(m_bFollowOnLine)
			{
				D3DXVECTOR3 vPos = m_BeforePos;
				m_BeforePos.y = pMeshInst->GetPos().y;
				pMeshInst->SetPos( m_BeforePos );
				if( bUpdateLandPos == true && pMeshInst->GetToggleRotae() == false )
				{
 					pMeshInst->SetToggleRotate(true);
 					m_bFollowLineRight = !m_bFollowLineRight;
 					D3DXVECTOR3 vRot = pMeshInst->GetRotateDegree();
 					if( m_bFollowLineRight == false )
 					{
 						vRot.y += 180.f;				
 					}
 					else
 					{
 						vRot.y -= 180.f;
 					}
 					pMeshInst->SetRotateDegree( vRot );
					pMeshInst->SetMoveAxisAngleDegree( vRot );
				}
			}

#ifdef RAVEN_WEAPON_TAKER
			if( true == m_bMustOnLine )
			{
				pMeshInst->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );
				pMeshInst->SetDirSpeed( 0.f );
				pMeshInst->PlayEnd();
			}
#endif RAVEN_WEAPON_TAKER

		}
#endif
	}
}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
// June2017 FUN_31f7aab0 (ctor) @31f7e7a4-31f7e7da: tries "LOCK_ON_DATA_ULTIMATE" first when
// m_bUseUltimateDamageData is set (forced false -- unsupported system, see Phase 5), falling
// back to "LOCK_ON_DATA" only when that table is absent. With the flag forced false, only the
// base table is read today; kept parseable for when Phase 7/8 can identify FUN_31fcf3e0.
void CX2DamageEffect::CEffect::LoadLockOnData()
{
	if( true == m_bUseUltimateDamageData )
	{
		if( true == m_pLuaManager->BeginTable( "LOCK_ON_DATA_ULTIMATE" ) )
		{
			LoadLockOnDataBlock();
			m_pLuaManager->EndTable();
			return;
		}
	}

	if( m_pLuaManager->BeginTable( "LOCK_ON_DATA" ) == true )
	{
		LoadLockOnDataBlock();
		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadLockOnDataBlock()
{
	PLUA_GET_VALUE_ENUM( m_pLuaManager, "LOCK_ON_TYPE", m_LockOnData.m_LockOnType, LOCK_ON_TYPE, LOT_NONE );
	PLUA_GET_VALUE( m_pLuaManager, "CURVE_SPEED",	m_LockOnData.m_fCurveSpeed, 0.0f );
	//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	PLUA_GET_VALUE( m_pLuaManager, "IN_DISTANCE_TO_DIE_EFFECT",	m_LockOnData.m_fInDistanceToDieEffect, -1.0f );
#endif	NEW_CHARACTER_CHUNG
	//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG

	//{{ kimhc // 2011.1.18 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	// 생성 후 몇초 이후부터 Lock을 수행하기 위해 사용
	PLUA_GET_VALUE( m_pLuaManager, "TIME_UNLOCK_FROM_START",	m_LockOnData.m_fTimeUnlockFromStart, 0.0f );
#endif	CHUNG_FIRST_CLASS_CHANGE
	//}} kimhc // 2011.1.18 // 청 1차 전직
#ifdef UPGRADE_RAVEN
	PLUA_GET_VALUE( m_pLuaManager, "TIME_UNLOCK_AFTER_START",	m_LockOnData.m_fUnLockTime, 9999.f );
#endif
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	PLUA_GET_VALUE( m_pLuaManager, "TARGET_RANGE",	m_LockOnData.m_fRange, -1.f );
#endif
#ifdef BALANCE_DEADLY_CHASER_20130214
	// 이전 타겟팅 한 대상을 재 타겟 하지 않도록 하는 기능
	PLUA_GET_VALUE( m_pLuaManager, "NOT_TARGET_PRE_LOCK_ON_TARGET", m_LockOnData.m_bIsNotTargetPreLockOnTarget, false );

#ifdef _IN_HOUSE_
	//NOT_TARGET_PRE_LOCK_ON_TARGET 기능은 LOT_NEARST_UID_VECTOR_IN_RANGE에서만 사용 가능.
	if( true == m_LockOnData.m_bIsNotTargetPreLockOnTarget && 
		LOT_NEARST_UID_VECTOR_IN_RANGE != m_LockOnData.m_LockOnType
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		// June2017: LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE reads the key too (FUN_322daa10)
		&& LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE != m_LockOnData.m_LockOnType
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
		)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), L"LOT_NEARST_UID_VECTOR_IN_RANGE 타입에서만 NOT_TARGET_PRE_LOCK_ON_TARGET 적용 가능", NULL );
	}
#endif //_IN_HOUSE_
#endif //BALANCE_DEADLY_CHASER_20130214

#ifdef FINALITY_SKILL_SYSTEM //김창한
	PLUA_GET_VALUE( m_pLuaManager, "ONLY_FRONT",	m_LockOnData.m_bIsOnlyFront, false );
	if( m_LockOnData.m_bIsOnlyFront == true && m_optrOwnerUnit != null )
	{
		m_LockOnData.m_bSaveIsRight = m_optrOwnerUnit->GetIsRight();
		m_LockOnData.m_iSaveLastTouchLineIndex = m_optrOwnerUnit->GetLastTouchLineIndex();
		m_LockOnData.m_vecSaveOwnerPos = m_optrOwnerUnit->GetPos();
	}
#endif //FINALITY_SKILL_SYSTEM

#ifdef LOCK_ON_USER_ONLY_ON
	PLUA_GET_VALUE( m_pLuaManager, "ONLY_TARGET_ATTACK",	m_LockOnData.m_bIsOnlyTargetAttack, false );
#endif //LOCK_ON_USER_ONLY_ON

#ifdef BALANCE_PATCH_20131107
	//기능 자체가 범위안에 적이 없으면 멈추는 기능이므로
	//NOT_FIND_TARGET_MOVE_STOP 기능은 LOT_NEARST_UID_VECTOR_IN_RANGE 에서만 사용가능.
	//또한 MeshPlayer 자체에서 DirSpeed값을 Initial로 했을때만을 고려해서 작업함.
	//at 0.200 등으로 중간에 변하는 속도에 대해서는 고려되지 않음.
	bool bNotFindTargetMoveStop = false;
	PLUA_GET_VALUE( m_pLuaManager, "NOT_FIND_TARGET_MOVE_STOP",	bNotFindTargetMoveStop, false );
	m_LockOnData.m_eNotFindTargetMoveStop = (bNotFindTargetMoveStop)? LOCSD_NOT_SAVE_DIR_SPEED : LOCSD_NONE;
#endif //BALANCE_PATCH_20131107

	// June2017 FUN_31f71930, Phase 6b: keys March did not read before this phase. Parse only --
	// runtime use (the AT_* mode selection, the INIT_TARGET lookup) is Phase 7b.
	// Code review fix: m_LockOnData.m_iRandomSeed is already live (OnFrameMove_LockOnData's
	// re-target branch reads it unconditionally, X2DamageEffect.cpp's GetLockOnTarget calls),
	// so writing straight into it here was not parse-only -- it would change targeting the
	// moment any LOCK_ON_DATA table carries this key. Stored in a new, inert member instead;
	// Phase 7b decides whether/how it should feed the live field.
	PLUA_GET_VALUE( m_pLuaManager, "RANDOM_SEED",					m_LockOnData.m_iRandomSeedFromLua,				-1		);	/// June @31f7359b, int getter 32fe9fd0, default -1
	PLUA_GET_VALUE( m_pLuaManager, "UNLOCK_START_AFTER_ATTACK",	m_LockOnData.m_iUnlockStartAfterAttack,				-1		);	/// June @31f724d0, int getter 32fe9fd0, default -1
	PLUA_GET_VALUE( m_pLuaManager, "RETARGET_COOLTIME_AFTER_ATTACK",	m_LockOnData.m_fRetargetCooltimeAfterAttack,		-1.f	);	/// June @31f726a6, float getter 32fea370, default -1.f
	PLUA_GET_VALUE( m_pLuaManager, "Y_DIR_INIT",					m_LockOnData.m_bYDirInit,							false	);	/// June @31f72d4e, bool getter 32fe9e40, default false
	PLUA_GET_VALUE( m_pLuaManager, "LOCK_ON_OFFSET_Y_POS",			m_LockOnData.m_fLockOnOffsetYPos,					100.f	);	/// June @31f72e3d, float getter 32fea370, default 100.f (binary-only)
	PLUA_GET_VALUE( m_pLuaManager, "LOCK_ON_TARGET_BONE_NAME",		m_LockOnData.m_strLockOnTargetBoneName,			L""		);	/// June @31f72fbe, wstring getter 32feae90, default L""
	PLUA_GET_VALUE( m_pLuaManager, "INIT_TARGET",					m_LockOnData.m_bInitTarget,							false	);	/// June @31f73691, bool getter 32fe9e40, default false
	PLUA_GET_VALUE( m_pLuaManager, "NOT_LOCKON_INVINCIBLE_TARGET",	m_LockOnData.m_bNotLockonInvincibleTarget,			false	);	/// June @31f73846, bool getter 32fe9e40, default false

	// NOT_TARGET_PRE_LOCK_ON_TARGET semantics-differ (matrix): June also reads this same key as
	// an int (getter 32fe9fd0, default 0, @31f721dd), alongside March's existing bool above. Kept
	// as a separate member; deciding how the two coexist is Phase 7b's, as with Phase 5's
	// DAMAGE_DATA_CHANGE_TIME.
	PLUA_GET_VALUE( m_pLuaManager, "NOT_TARGET_PRE_LOCK_ON_TARGET",	m_LockOnData.m_iNotTargetPreLockOnTarget,			0		);

	// June "LOCK_ON_RANGE_FROM_OWNER" @31f73202-31f7354b: June selects one of these four members
	// at ctor time by the effect's current game mode (dungeon/field/raid-field/pvp), read from
	// *(int*)(DAT_3377585c+0x8c) -- not identified. Parsed unconditionally here (every member
	// present, regardless of mode); the mode-based selection is Phase 7b's.
	m_LockOnData.m_fLockOnRangeFromOwnerAtDungeon	= 99999.f;
	m_LockOnData.m_fLockOnRangeFromOwnerAtField		= 1000.f;
	m_LockOnData.m_fLockOnRangeFromOwnerAtRaidField	= 99999.f;
	m_LockOnData.m_fLockOnRangeFromOwnerAtPvp		= 99999.f;
	if( true == m_pLuaManager->BeginTable( "LOCK_ON_RANGE_FROM_OWNER" ) )
	{
		m_LockOnData.m_bHasLockOnRangeFromOwner = true;
		PLUA_GET_VALUE( m_pLuaManager, "AT_DUNGEON",		m_LockOnData.m_fLockOnRangeFromOwnerAtDungeon,		99999.f	);	/// June @31f73197->31f734c9
		PLUA_GET_VALUE( m_pLuaManager, "AT_FIELD",			m_LockOnData.m_fLockOnRangeFromOwnerAtField,		1000.f	);	/// June @31f7339a->31f73415
		PLUA_GET_VALUE( m_pLuaManager, "AT_RAID_FIELD",	m_LockOnData.m_fLockOnRangeFromOwnerAtRaidField,	99999.f	);	/// June @31f732aa->31f734c9 (binary-only)
		PLUA_GET_VALUE( m_pLuaManager, "AT_PVP",			m_LockOnData.m_fLockOnRangeFromOwnerAtPvp,			99999.f	);	/// June @31f73468->31f734c9
		m_pLuaManager->EndTable();
	}

	// June "DUNGEON_TARGET_RANGE_MIN_MAX"/"PVP_TARGET_RANGE_MIN_MAX" @31f73225-31f73438: a plaintext
	// sub-table read positionally (index 1 = min, index 2 = max), reset to both defaults unless
	// both indices parse and max is strictly greater than min.
	m_LockOnData.m_fDungeonTargetRangeMin = 150.f;
	m_LockOnData.m_fDungeonTargetRangeMax = 750.f;
	if( true == m_pLuaManager->BeginTable( "DUNGEON_TARGET_RANGE_MIN_MAX" ) )
	{
		m_LockOnData.m_bHasDungeonTargetRangeMinMax = true;
		float fMin = 150.f, fMax = 750.f;
		if( false == m_pLuaManager->GetValue( 1, fMin ) || false == m_pLuaManager->GetValue( 2, fMax ) || fMax <= fMin )
		{
			fMin = 150.f;
			fMax = 750.f;
		}
		m_LockOnData.m_fDungeonTargetRangeMin = fMin;
		m_LockOnData.m_fDungeonTargetRangeMax = fMax;
		m_pLuaManager->EndTable();
	}

	m_LockOnData.m_fPvpTargetRangeMin = 150.f;
	m_LockOnData.m_fPvpTargetRangeMax = 300.f;
	if( true == m_pLuaManager->BeginTable( "PVP_TARGET_RANGE_MIN_MAX" ) )
	{
		m_LockOnData.m_bHasPvpTargetRangeMinMax = true;
		float fMin = 150.f, fMax = 300.f;
		if( false == m_pLuaManager->GetValue( 1, fMin ) || false == m_pLuaManager->GetValue( 2, fMax ) || fMax <= fMin )
		{
			fMin = 150.f;
			fMax = 300.f;
		}
		m_LockOnData.m_fPvpTargetRangeMin = fMin;
		m_LockOnData.m_fPvpTargetRangeMax = fMax;
		m_pLuaManager->EndTable();
	}

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	// JUNE_FX LOD_TEMPLET <name> <key>=<v> ..., only when this LOCK_ON_DATA uses a non-default
	// Phase-6b key. Ctor-time only, so no throttling map is needed.
	{
		WCHAR wszJuneFxLod[768] = {0};
		StringCchPrintfW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L"JUNE_FX LOD_TEMPLET %s", GetName() );
		const size_t uiLodNameOnly = wcslen( wszJuneFxLod );
		WCHAR wszLodKey[192] = {0};
		if( -1 != m_LockOnData.m_iRandomSeedFromLua )
		{
			StringCchPrintfW( wszLodKey, ARRAY_SIZE(wszLodKey), L" RANDOM_SEED=%d", m_LockOnData.m_iRandomSeedFromLua );
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), wszLodKey );
		}
		if( -1 != m_LockOnData.m_iUnlockStartAfterAttack )
		{
			StringCchPrintfW( wszLodKey, ARRAY_SIZE(wszLodKey), L" UNLOCK_START_AFTER_ATTACK=%d", m_LockOnData.m_iUnlockStartAfterAttack );
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), wszLodKey );
		}
		if( true == m_LockOnData.m_bYDirInit )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" Y_DIR_INIT=1" );
		if( true == m_LockOnData.m_bInitTarget )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" INIT_TARGET=1" );
		if( true == m_LockOnData.m_bNotLockonInvincibleTarget )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" NOT_LOCKON_INVINCIBLE_TARGET=1" );
		if( false == m_LockOnData.m_strLockOnTargetBoneName.empty() )
		{
			StringCchPrintfW( wszLodKey, ARRAY_SIZE(wszLodKey), L" LOCK_ON_TARGET_BONE_NAME=%s", m_LockOnData.m_strLockOnTargetBoneName.c_str() );
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), wszLodKey );
		}
		if( true == m_LockOnData.m_bHasLockOnRangeFromOwner )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" LOCK_ON_RANGE_FROM_OWNER=1" );
		if( true == m_LockOnData.m_bHasDungeonTargetRangeMinMax )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" DUNGEON_TARGET_RANGE_MIN_MAX=1" );
		if( true == m_LockOnData.m_bHasPvpTargetRangeMinMax )
			StringCchCatW( wszJuneFxLod, ARRAY_SIZE(wszJuneFxLod), L" PVP_TARGET_RANGE_MIN_MAX=1" );
		if( wcslen( wszJuneFxLod ) != uiLodNameOnly )
			CX2OfflineLog::Server( L"%s", wszJuneFxLod );
	}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f71930 @31f7355c-31f73647: June has ONE seed member (+0x948). RANDOM_SEED is
	// read straight into it (-1 when absent = March's LockOnData() default) and the re-target picks
	// read it (@31f84106, @31f84489). This reverses Phase 6b's parse-only split. LoadLockOnData runs
	// only from the ctor (X2DamageEffect.cpp:1885), before skill code can SetLockOnRandomSeed, so a
	// seed set by skill code still wins, as in June.
	m_LockOnData.m_iRandomSeed = m_LockOnData.m_iRandomSeedFromLua;
#ifdef SERV_9TH_NEW_CHARACTER
	// @31f73109: likewise ONE bone-name member (+0x9ac), the one LOT_UID_BONE_POS reads (@31f856fe).
	// March never assigned m_wstrTargetBoneName, so that case always fell back to GetPos.
	m_LockOnData.m_wstrTargetBoneName = m_LockOnData.m_strLockOnTargetBoneName;
#endif SERV_9TH_NEW_CHARACTER

	JuneSelectLockOnRangeFromOwner();

	// @31f73732-31f737e2: INIT_TARGET -- an immediate seeded pick (FUN_322dadd0) at load, when the
	// owner is live. The out bool starts true (@31f7376d).
	if( true == m_LockOnData.m_bInitTarget && null != m_optrOwnerUnit )
	{
		bool bUserUnit = true;
		const UidType uid = JunePickLockOnTargetBySeed( m_LockOnData.m_iRandomSeed, bUserUnit );
		JuneSetLockOnTargetUID( uid, bUserUnit );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"INIT_TARGET", L"seed=%d -> uid=%I64d user=%d", m_LockOnData.m_iRandomSeed, uid, ( bUserUnit ? 1 : 0 ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
void CX2DamageEffect::CEffect::LoadLockOnData()
{
	if( m_pLuaManager->BeginTable( "LOCK_ON_DATA" ) == true )
	{
		PLUA_GET_VALUE_ENUM( m_pLuaManager, "LOCK_ON_TYPE", m_LockOnData.m_LockOnType, LOCK_ON_TYPE, LOT_NONE );
		PLUA_GET_VALUE( m_pLuaManager, "CURVE_SPEED",	m_LockOnData.m_fCurveSpeed, 0.0f );
		//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
		PLUA_GET_VALUE( m_pLuaManager, "IN_DISTANCE_TO_DIE_EFFECT",	m_LockOnData.m_fInDistanceToDieEffect, -1.0f );
#endif	NEW_CHARACTER_CHUNG
		//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.18 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
		// 생성 후 몇초 이후부터 Lock을 수행하기 위해 사용
		PLUA_GET_VALUE( m_pLuaManager, "TIME_UNLOCK_FROM_START",	m_LockOnData.m_fTimeUnlockFromStart, 0.0f );
#endif	CHUNG_FIRST_CLASS_CHANGE
		//}} kimhc // 2011.1.18 // 청 1차 전직
#ifdef UPGRADE_RAVEN
		PLUA_GET_VALUE( m_pLuaManager, "TIME_UNLOCK_AFTER_START",	m_LockOnData.m_fUnLockTime, 9999.f );
#endif
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		PLUA_GET_VALUE( m_pLuaManager, "TARGET_RANGE",	m_LockOnData.m_fRange, -1.f );
#endif
#ifdef BALANCE_DEADLY_CHASER_20130214
		// 이전 타겟팅 한 대상을 재 타겟 하지 않도록 하는 기능
		PLUA_GET_VALUE( m_pLuaManager, "NOT_TARGET_PRE_LOCK_ON_TARGET", m_LockOnData.m_bIsNotTargetPreLockOnTarget, false );

	#ifdef _IN_HOUSE_
		//NOT_TARGET_PRE_LOCK_ON_TARGET 기능은 LOT_NEARST_UID_VECTOR_IN_RANGE에서만 사용 가능.
		if( true == m_LockOnData.m_bIsNotTargetPreLockOnTarget && 
			LOT_NEARST_UID_VECTOR_IN_RANGE != m_LockOnData.m_LockOnType )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), L"LOT_NEARST_UID_VECTOR_IN_RANGE 타입에서만 NOT_TARGET_PRE_LOCK_ON_TARGET 적용 가능", NULL );
		}
	#endif //_IN_HOUSE_
#endif //BALANCE_DEADLY_CHASER_20130214

#ifdef FINALITY_SKILL_SYSTEM //김창한
		PLUA_GET_VALUE( m_pLuaManager, "ONLY_FRONT",	m_LockOnData.m_bIsOnlyFront, false );
		if( m_LockOnData.m_bIsOnlyFront == true && m_optrOwnerUnit != null )
		{
			m_LockOnData.m_bSaveIsRight = m_optrOwnerUnit->GetIsRight();
			m_LockOnData.m_iSaveLastTouchLineIndex = m_optrOwnerUnit->GetLastTouchLineIndex();
			m_LockOnData.m_vecSaveOwnerPos = m_optrOwnerUnit->GetPos();
		}
#endif //FINALITY_SKILL_SYSTEM

#ifdef LOCK_ON_USER_ONLY_ON
		PLUA_GET_VALUE( m_pLuaManager, "ONLY_TARGET_ATTACK",	m_LockOnData.m_bIsOnlyTargetAttack, false );
#endif //LOCK_ON_USER_ONLY_ON

#ifdef BALANCE_PATCH_20131107
		//기능 자체가 범위안에 적이 없으면 멈추는 기능이므로
		//NOT_FIND_TARGET_MOVE_STOP 기능은 LOT_NEARST_UID_VECTOR_IN_RANGE 에서만 사용가능.
		//또한 MeshPlayer 자체에서 DirSpeed값을 Initial로 했을때만을 고려해서 작업함.
		//at 0.200 등으로 중간에 변하는 속도에 대해서는 고려되지 않음.
		bool bNotFindTargetMoveStop = false;
		PLUA_GET_VALUE( m_pLuaManager, "NOT_FIND_TARGET_MOVE_STOP",	bNotFindTargetMoveStop, false );
		m_LockOnData.m_eNotFindTargetMoveStop = (bNotFindTargetMoveStop)? LOCSD_NOT_SAVE_DIR_SPEED : LOCSD_NONE;
#endif //BALANCE_PATCH_20131107

		m_pLuaManager->EndTable();
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE



#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
// June2017 FUN_31f7aab0 (ctor) @31f7e7d3-31f7e7e9: tries "BLACKHOLE_DATA_ULTIMATE" first when
// m_bUseUltimateDamageData is set (forced false), falling back to "BLACKHOLE_DATA" -- the same
// shape as LoadLockOnData above. No new keys inside the table itself.
void CX2DamageEffect::CEffect::LoadBlackHoleData()
{
	if( true == m_bUseUltimateDamageData )
	{
		if( true == m_pLuaManager->BeginTable( "BLACKHOLE_DATA_ULTIMATE" ) )
		{
			m_BlackHoleData.m_bEnabled = true;
			LoadBlackHoleDataBlock();
			m_pLuaManager->EndTable();
			return;
		}
	}

	if( m_pLuaManager->BeginTable( "BLACKHOLE_DATA" ) == true )
	{
		m_BlackHoleData.m_bEnabled = true;
		LoadBlackHoleDataBlock();
		m_pLuaManager->EndTable();
	}
	else
	{
		m_BlackHoleData.m_bEnabled = false;
	}
}

void CX2DamageEffect::CEffect::LoadBlackHoleDataBlock()
{
	PLUA_GET_VALUE( m_pLuaManager, "DRAGGING_SPEED", m_BlackHoleData.m_fDraggingSpeed, 1.f );
	PLUA_GET_VALUE( m_pLuaManager, "DRAGGING_RANGE", m_BlackHoleData.m_fDraggingRange, 1.f );

	PLUA_GET_VALUE( m_pLuaManager, "DRAG_ONLY_HITTED_UNIT", m_BlackHoleData.m_bDragOnlyHittedUnit, false );
#ifdef ARA_CHARACTER_BASE
	PLUA_GET_VALUE( m_pLuaManager, "CAN_PASS_UNIT", m_BlackHoleData.m_bCanPassUnit, false );
#endif
}
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
void CX2DamageEffect::CEffect::LoadBlackHoleData()
{
	if( m_pLuaManager->BeginTable( "BLACKHOLE_DATA" ) == true )
	{
		m_BlackHoleData.m_bEnabled = true;
		PLUA_GET_VALUE( m_pLuaManager, "DRAGGING_SPEED", m_BlackHoleData.m_fDraggingSpeed, 1.f );
		PLUA_GET_VALUE( m_pLuaManager, "DRAGGING_RANGE", m_BlackHoleData.m_fDraggingRange, 1.f );

		PLUA_GET_VALUE( m_pLuaManager, "DRAG_ONLY_HITTED_UNIT", m_BlackHoleData.m_bDragOnlyHittedUnit, false );
#ifdef ARA_CHARACTER_BASE
		PLUA_GET_VALUE( m_pLuaManager, "CAN_PASS_UNIT", m_BlackHoleData.m_bCanPassUnit, false );
#endif

		m_pLuaManager->EndTable();
	}
	else
	{
		m_BlackHoleData.m_bEnabled = false;
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE


#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
// June2017 FUN_31f66700 @31f66700-31f66e4f, called from the ctor @31f7f94e-31f7f976 for both
// "AND_OPERATION" (bIsOrOperation=false, @31f7f95f) and "OR_OPERATION" (bIsOrOperation=true,
// @31f7f96e). Each opens a numbered "CONDITION_%d" sub-table (a base name with the trailing
// underscore baked in, since March's BeginTable(name,index) does a plain "%s%d"). Reads
// DAMAGE_EFFECT_DIE_CONDITION_TYPE first (int getter 32fe9fd0, default DEDCT_NONE=-1, @31f66826);
// a failed read or a DEDCT_NONE value drops the whole entry (June never pushes it, @31f669df/
// 31f669f9). The type then picks which of the other two keys is meaningful: DEDCT_DAMAGE_TIME
// (@31f66a01) reads CHECK_DAMAGE_TIME_VALUE (float, default -1.f, @31f66c68) and IS_OVER (bool,
// default true, @31f66d1a); DEDCT_LOCK_ON_TARGET (@31f66a08) reads IS_EXIST (bool, default
// false, @31f66ab9). Parse only; combining entries by bIsOrOperation into an actual die
// decision is Phase 7c.
void CX2DamageEffect::CEffect::LoadDieCheckConditionBlock( const char* pTableName, bool bIsOrOperation )
{
	if( false == m_pLuaManager->BeginTable( pTableName ) )
		return;

	int index = 0;
	while( m_pLuaManager->BeginTable( "CONDITION_", index ) == true )
	{
		int iConditionType = -1;
		bool bGotType = m_pLuaManager->GetValue( "DAMAGE_EFFECT_DIE_CONDITION_TYPE", iConditionType );
		if( false == bGotType || DEDCT_NONE == (CX2DamageEffect::DAMAGE_EFFECT_DIE_CONDITION_TYPE)iConditionType )
		{
			m_pLuaManager->EndTable();
			index++;
			continue;
		}

		DieCheckCondition data;
		data.conditionType = (CX2DamageEffect::DAMAGE_EFFECT_DIE_CONDITION_TYPE)iConditionType;
		data.bIsOrOperation = bIsOrOperation;

		if( DEDCT_DAMAGE_TIME == data.conditionType )
		{
			PLUA_GET_VALUE( m_pLuaManager, "CHECK_DAMAGE_TIME_VALUE",	data.fCheckDamageTimeValue,	-1.0f	);
			PLUA_GET_VALUE( m_pLuaManager, "IS_OVER",					data.bIsOver,				true	);
		}
		else if( DEDCT_LOCK_ON_TARGET == data.conditionType )
		{
			PLUA_GET_VALUE( m_pLuaManager, "IS_EXIST",					data.bIsExist,				false	);
		}

		m_vecDieCheckCondition.push_back( data );

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		{
			static map<wstring, DWORD> s_mapLastLoggedTime_dieCondition;
			DWORD dwNow_dieCondition = timeGetTime();
			map<wstring, DWORD>::iterator itThrottle_dieCondition = s_mapLastLoggedTime_dieCondition.find( GetName() );
			if( itThrottle_dieCondition == s_mapLastLoggedTime_dieCondition.end() || dwNow_dieCondition - itThrottle_dieCondition->second >= 1000 )
			{
				s_mapLastLoggedTime_dieCondition[ GetName() ] = dwNow_dieCondition;
				CX2OfflineLog::Server( L"JUNE_FX DDE_CONDITION %s %s%d type=%d isOr=%d checkTime=%.3f isOver=%d isExist=%d",
					GetName(), bIsOrOperation ? L"OR_OPERATION" : L"AND_OPERATION", index, (int)data.conditionType, (int)data.bIsOrOperation,
					data.fCheckDamageTimeValue, (int)data.bIsOver, (int)data.bIsExist );
			}
		}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

		index++;
		m_pLuaManager->EndTable();
	}

	m_pLuaManager->EndTable();
}

void CX2DamageEffect::CEffect::LoadDieCheckCondition()
{
	if( true == m_pLuaManager->BeginTable( "DAMAGE_EFFECT_DIE_CHECK_CONDITION" ) )
	{
		LoadDieCheckConditionBlock( "AND_OPERATION", false );
		LoadDieCheckConditionBlock( "OR_OPERATION", true );
		m_pLuaManager->EndTable();
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-09-27
// Description: June2017 DAMAGE_EFFECT_DIE_CHECK_CONDITION runtime (FUN_31f765b0 the combiner,
// FUN_31f76010 the per-entry check). Empty list -> false (31f765ba-31f765c6). Non-empty:
// AND of the AND_OPERATION entries (accumulator starts true, ANDed per entry @31f76602),
// ANDed with the OR of the OR_OPERATION entries (accumulated separately @31f765f4) only when
// at least one OR entry exists (@31f76609-31f76614; unchanged when there is none).
// DEDCT_DAMAGE_TIME (@31f760cc): compares m_DamageTimeNow to (int)CHECK_DAMAGE_TIME_VALUE,
// greater-than when IS_OVER else less-than. DEDCT_LOCK_ON_TARGET (@31f76025): true when a
// locked-on unit or NPC UID is set and still resolves to a live unit
// (g_pX2Game->GetUserUnitByUID/GetNPCUnitByUID, the same lookup used everywhere else in this
// file), compared against IS_EXIST.
bool CX2DamageEffect::CEffect::JuneEvaluateDieCheckCondition()
{
	if( true == m_vecDieCheckCondition.empty() )
		return false;

	bool bAndAccumulator = true;
	bool bOrAccumulator = false;
	bool bHaveOrEntry = false;

	for( size_t i = 0; i < m_vecDieCheckCondition.size(); i++ )
	{
		const DieCheckCondition& data = m_vecDieCheckCondition[i];
		bool bEntryResult = false;

		if( DEDCT_DAMAGE_TIME == data.conditionType )
		{
			int iCheckDamageTimeValue = (int)data.fCheckDamageTimeValue;
			bEntryResult = ( true == data.bIsOver ) ? ( m_DamageTimeNow > iCheckDamageTimeValue ) : ( m_DamageTimeNow < iCheckDamageTimeValue );
		}
		else if( DEDCT_LOCK_ON_TARGET == data.conditionType )
		{
			bool bTargetExists = false;
			if( NULL != g_pX2Game )
			{
				if( -1 != m_LockOnData.m_LockOnUnitUID )
					bTargetExists = ( NULL != g_pX2Game->GetUserUnitByUID( m_LockOnData.m_LockOnUnitUID ) );
				else if( -1 != m_LockOnData.m_LockOnNPCUID )
					bTargetExists = ( NULL != g_pX2Game->GetNPCUnitByUID( m_LockOnData.m_LockOnNPCUID ) );
			}
			bEntryResult = ( data.bIsExist == bTargetExists );
		}

		if( true == data.bIsOrOperation )
		{
			bOrAccumulator = bOrAccumulator || bEntryResult;
			bHaveOrEntry = true;
		}
		else
		{
			bAndAccumulator = bAndAccumulator && bEntryResult;
		}
	}

	if( true == bHaveOrEntry )
		bAndAccumulator = bAndAccumulator && bOrAccumulator;

	return bAndAccumulator;
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME


//**
#ifdef _SIN_MOVE_TEST_

void CX2DamageEffect::CEffect::LoadSinMoveData()
{
	if( m_pLuaManager->BeginTable( "SIN_MOVE_DATA" ) == true )
	{
		m_SinMoveData.m_bSinMove = true;
		
		PLUA_GET_VALUE( m_pLuaManager, "SIN_MOVE_MAX",			m_SinMoveData.m_fMoveMax,					0.0f );
		PLUA_GET_VALUE( m_pLuaManager, "SIN_MOVE_MIN",			m_SinMoveData.m_fMoveMin,					0.0f );
		PLUA_GET_VALUE( m_pLuaManager, "SIN_MOVE_TIME_MAX",		m_SinMoveData.m_fTimeMax,					0.0f );
		PLUA_GET_VALUE( m_pLuaManager, "SIN_MOVE_TIME_MIN",		m_SinMoveData.m_fTimeMin,					0.0f );
		
		m_pLuaManager->EndTable();

		if( null != m_optrOwnerUnit )
		{
			float RandomSeed = m_optrOwnerUnit->GetRandomFloat();
			m_SinMoveData.m_fTime = (m_SinMoveData.m_fTimeMax-m_SinMoveData.m_fTimeMin)*RandomSeed + m_SinMoveData.m_fTimeMin;
			m_SinMoveData.m_fMove = (m_SinMoveData.m_fMoveMax-m_SinMoveData.m_fMoveMin)*RandomSeed + m_SinMoveData.m_fMoveMin;
		}
		else
		{
			m_SinMoveData.m_fTime = (m_SinMoveData.m_fTimeMin + m_SinMoveData.m_fTimeMax) / 2;
			m_SinMoveData.m_fMove = (m_SinMoveData.m_fMoveMin + m_SinMoveData.m_fMoveMax) / 2;
		}	
	}
}
#endif _SIN_MOVE_TEST_

//kimhc // 2010-12-17 // D3DXVECTOR3를 인자로 넘기는 것을 const D3DXVECTOR3&로 변경함
void CX2DamageEffect::CEffect::VectorLockOn( const D3DXVECTOR3& targetPos )
{
    if ( g_pX2Game == NULL )
        return;

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f73a10 @31f73a3c-31f73a55: nothing is steered while the aim point is farther from
	// the owner than LOCK_ON_RANGE_FROM_OWNER (FUN_31f647c0). 99999 unless a LOCK_ON_DATA table is
	// loaded in a battle field (1000 by default there), see JuneSelectLockOnRangeFromOwner.
	if( true == JuneIsOutOfLockOnRangeFromOwner( targetPos ) )
	{
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"RANGE_FROM_OWNER_REJECT", L"range=%.0f dist=%.0f", m_LockOnData.m_fLockOnRangeFromOwner,
			( null != m_optrOwnerUnit ? GetDistance( targetPos, m_optrOwnerUnit->GetPos() ) : -1.f ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		return;
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
	{
		return;
	}

//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	// m_fInDistanceToDieEffect 변수에 값이 지정 되어 있으면 판단함
	if ( 0.0f < m_LockOnData.m_fInDistanceToDieEffect )
	{
		// 사정 거리내에 들어오면
		if ( GetDistance( targetPos, pMeshInst->GetPos() ) < m_LockOnData.m_fInDistanceToDieEffect )
		{
			m_hMeshInstHandle = INVALID_MESH_INSTANCE_HANDLE;
			return;
			
		}
	}
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG
#ifdef FINALITY_SKILL_SYSTEM //김창한
	if( true == m_LockOnData.m_bIsOnlyFront )
	{
		const CKTDGLineMap::LineData* pCurrLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( m_LockOnData.m_iSaveLastTouchLineIndex );
		if(pCurrLineData != NULL)
		{
			D3DXVECTOR3 vFinalDestDir = targetPos - m_LockOnData.m_vecSaveOwnerPos;
			D3DXVec3Normalize( &vFinalDestDir, &vFinalDestDir );
			D3DXVECTOR3 vRightVec = pCurrLineData->dirVector;
			D3DXVec3Normalize( &vRightVec, &vRightVec );

			float fIsRight = D3DXVec3Dot( &vRightVec, &vFinalDestDir );
			bool bRight = true;
			if( fIsRight > 0.f )
				bRight = true;
			else
				bRight = false;

			if( bRight != m_LockOnData.m_bSaveIsRight )
				return;
		}
	}
#endif //FINALITY_SKILL_SYSTEM

	D3DXVECTOR3 axis;	// 회전축
	D3DXVECTOR3 relVec = targetPos - pMeshInst->GetPos();	// 목표와의 상대적 벡터
	D3DXVec3Normalize( &relVec, &relVec );

	//float estTime = D3DXVec3Length(&relVec) / speed;
	//relVec = (targetPos + targetVel*estTime) - pos;		// 예측형 유도
	D3DXVECTOR3 nowVec = pMeshInst->GetManualDirDegree();
	nowVec.x = D3DXToRadian(nowVec.x);
	nowVec.y = D3DXToRadian(nowVec.y);
	nowVec.z = D3DXToRadian(nowVec.z);
	D3DXVec3Normalize( &nowVec, &nowVec );

	D3DXVECTOR3 gapVec = relVec - nowVec;
	nowVec += gapVec * m_LockOnData.m_fCurveSpeed * m_fElapsedTime;

	//D3DXVec3Normalize( &nowVec, &nowVec );
		
	pMeshInst->UseManualDirDegree( true );
	D3DXVECTOR3 nowVecDegree;
	nowVecDegree.x = D3DXToDegree(nowVec.x);
	nowVecDegree.y = D3DXToDegree(nowVec.y);
	nowVecDegree.z = D3DXToDegree(nowVec.z);

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// June2017 FUN_31f73a10 @31f73d77-31f73db5: Y_DIR_INIT negates the new heading's y once, on the
	// first steer that gets this far (fmul QWORD [33436c90] = -1.0).
	if( true == m_LockOnData.m_bYDirInit )
	{
		m_LockOnData.m_bYDirInit = false;
		nowVecDegree.y *= -1.f;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"Y_DIR_INIT", L"dir=(%.1f,%.1f,%.1f)", nowVecDegree.x, nowVecDegree.y, nowVecDegree.z );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	pMeshInst->SetManualDirDegree( nowVecDegree );
	
	//nowVec = GetDirVecToDegree( nowVec );
	//pMeshInst->SetRotateDegree( nowVec );

	/*
	D3DXVec3Cross( &axis, &nowVec, &relVec );

	float inner = D3DXVec3Dot( &nowVec, &relVec );// 내적값

	// 방향의 수정. 상하좌우 90도 이상이면 유도하지 않는다.
	if( inner > 0 )
	{
		// 회전각 : 적당한 크기의 상수 각을 잡아 돌려준다.
		float theta = m_LockOnData.m_fCurveSpeed * m_fElapsedTime;

		// 외적을 통해 나온 축을 중심으로 회전시킨다.
		// 벡터만 따로 회전시키기는 애매하므로,
		// 행렬에 집어넣은 뒤 DirectX에서 지원하는 회전 method를 이용한다.

		D3DXMATRIX velMat(				// 벡터의 행렬화
			0.0F, 0.0F, 0.0F, nowVec.x,	// 4번째 열이 속도
			0.0F, 0.0F, 0.0F, nowVec.y,
			0.0F, 0.0F, 0.0F, nowVec.z,
			0.0F, 0.0F, 0.0F, 0.0F );
		D3DXMATRIX newVelMat;

		// 회전변환용 행렬 생성
		D3DXMATRIX matRotAxis;
		D3DXMatrixRotationAxis( &matRotAxis, &axis, theta );

		newVelMat = matRotAxis * velMat;		// 회전시킨다

		// 변환된 행렬로부터 속도 성분을 뽑아낸다.
		nowVec = D3DXVECTOR3( newVelMat._14, newVelMat._24, newVelMat._34 );
	}
	pMeshInst->UseManualDirDegree( true );
	D3DXVECTOR3 nowVecDegree;
	nowVecDegree.x = D3DXToDegree(nowVec.x);
	nowVecDegree.y = D3DXToDegree(nowVec.y);
	nowVecDegree.z = D3DXToDegree(nowVec.z);

	pMeshInst->SetManualDirDegree( nowVecDegree );
	nowVec = GetDirVecToDegree( nowVec );
	pMeshInst->SetRotateDegree( nowVec );

	//pos += frameTime * vel * speed;		// 유도탄의 위치 변경
	//speed += frameTime * 30.0;			// 유도탄 가속
	*/
}






#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
// ---- June2017 lock-on runtime helpers (plan Phase 7b; evidence in june2017_re/funcmap.tsv) ----

void CX2DamageEffect::CEffect::JuneSetLockOnTargetUID( UidType uid_, bool bUserUnit_ )
{
	if( true == bUserUnit_ )
		m_LockOnData.m_LockOnUnitUID = uid_;
	else
		m_LockOnData.m_LockOnNPCUID = static_cast<int>( uid_ );
}

// June2017 FUN_31f647c0 (RET 0xc): true only when vPos_ is strictly farther from the owner than the
// LOCK_ON_RANGE_FROM_OWNER value selected at load (June +0x9d0); false with no owner.
bool CX2DamageEffect::CEffect::JuneIsOutOfLockOnRangeFromOwner( const D3DXVECTOR3& vPos_ )
{
	if( null == m_optrOwnerUnit )
		return false;
	return GetDistance( vPos_, m_optrOwnerUnit->GetPos() ) > m_LockOnData.m_fLockOnRangeFromOwner;
}

// June2017 FUN_31f71930 @31f73130-31f73540: choose the LOCK_ON_RANGE_FROM_OWNER value for the state
// the effect is created in (June compares *(DAT_3377585c+0x8c) with its own X2_STATE numbers; March's
// differ, so the March enum names are used). With the table: AT_PVP in XS_PVP_GAME, AT_RAID_FIELD /
// AT_FIELD in XS_BATTLE_FIELD, else AT_DUNGEON. Without it: 1000 in a non-raid battle field, else
// 99999. June's XS_PVE_GAME row (5000, and no AT_* read) has no March state.
void CX2DamageEffect::CEffect::JuneSelectLockOnRangeFromOwner()
{
	const int iState = ( NULL != g_pMain ) ? static_cast<int>( g_pMain->GetNowStateID() ) : static_cast<int>( CX2Main::XS_INVALID );
	bool bRaidField = false;
#ifdef FIELD_BOSS_RAID
	// June 32481f80( 32482650() ): current battle field's type in 2..4. Re-expressed as March's
	// boss-raid field test -- same state test; that the two field flags agree is a candidate only.
	if( CX2Main::XS_BATTLE_FIELD == iState && NULL != g_pData )
		bRaidField = g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField();
#endif FIELD_BOSS_RAID

	if( true == m_LockOnData.m_bHasLockOnRangeFromOwner )
	{
		if( CX2Main::XS_PVP_GAME == iState )
			m_LockOnData.m_fLockOnRangeFromOwner = m_LockOnData.m_fLockOnRangeFromOwnerAtPvp;			// @31f73438-31f734d8
		else if( CX2Main::XS_BATTLE_FIELD == iState )
			m_LockOnData.m_fLockOnRangeFromOwner = ( true == bRaidField )								// @31f7323f-31f73433
				? m_LockOnData.m_fLockOnRangeFromOwnerAtRaidField : m_LockOnData.m_fLockOnRangeFromOwnerAtField;
		else
			m_LockOnData.m_fLockOnRangeFromOwner = m_LockOnData.m_fLockOnRangeFromOwnerAtDungeon;		// @31f7315d-31f734d8
	}
	else
	{
		m_LockOnData.m_fLockOnRangeFromOwner = ( CX2Main::XS_BATTLE_FIELD == iState && false == bRaidField )	// @31f734e5-31f73540
			? 1000.f : 99999.f;
	}
}

// DUNGEON_/PVP_TARGET_RANGE_MIN_MAX, PVP in XS_PVP_GAME (June: state 9 selects +0x978, e.g. @31f851b8).
D3DXVECTOR2 CX2DamageEffect::CEffect::JuneGetLockOnTargetRange() const
{
	if( NULL != g_pMain && CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID() )
		return D3DXVECTOR2( m_LockOnData.m_fPvpTargetRangeMin, m_LockOnData.m_fPvpTargetRangeMax );
	return D3DXVECTOR2( m_LockOnData.m_fDungeonTargetRangeMin, m_LockOnData.m_fDungeonTargetRangeMax );
}

// The filters all of June's target searches share (322dadd0, 322daa10, 322d7670, 322d10b0): enemy
// team, not invincible (vtable 0x180), alive (FUN_31e4f900), and for an NPC not a boss-raid NPC (vtable
// 0x3c8: User returns 0, NPC tests MG_BOSS_NPC) nor NCT_THING_TRAP / NCT_THING_CHECKER (0x66 / 0x6a).
// Not ported: June also skips an NPC whose byte +0x8fb6 is 1 -- that member is not identified.
bool CX2DamageEffect::CEffect::JuneIsLockOnCandidate( const CX2GameUnit* pUnit_, int iTeam_ )
{
	if( NULL == pUnit_ )
		return false;
	if( pUnit_->GetTeam() == iTeam_ )
		return false;
	if( true == pUnit_->GetInvincible() )
		return false;
	if( 0.f >= pUnit_->GetNowHp() )
		return false;
	if( CX2GameUnit::GUT_NPC == pUnit_->GetGameUnitType() )
	{
#ifdef FIELD_BOSS_RAID
		if( true == pUnit_->GetIsBosRaidNPC() )
			return false;
#endif FIELD_BOSS_RAID
		const CX2GUNPC* pNPC = static_cast<const CX2GUNPC*>( pUnit_ );
		switch( pNPC->GetNPCTemplet().m_ClassType )
		{
		case CX2UnitManager::NCT_THING_TRAP:
		case CX2UnitManager::NCT_THING_CHECKER:
			return false;
		default:
			break;
		}
	}
	return true;
}

// The collision point of pUnit_ nearest vFrom_ (ties go to the later point), else its position
// raised by 100 -- March's inline loop from OnFrameMove_LockOnData, which June repeats per case
// (e.g. @31f83e15-31f8403f).
D3DXVECTOR3 CX2DamageEffect::CEffect::JuneGetNearestCollisionPos( const CX2GameUnit* pUnit_, const D3DXVECTOR3& vFrom_, OUT bool& bFound_ )
{
	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	bFound_ = false;
	const CKTDXCollision::CollisionDataListSet& AttackListSet = pUnit_->GetCollisionListSet();
	BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
	{
		BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
		{
			const D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
			if ( false == bFound_ )
			{
				bFound_ = true;
				vPos = vCollPos;
				continue;
			}
			if ( GetDistance( vFrom_, vCollPos ) <= GetDistance( vFrom_, vPos ) )
				vPos = vCollPos;
		}
	}
	if ( false == bFound_ )
	{
		vPos = pUnit_->GetPos();
		vPos.y += 100.0f;
	}
	return vPos;
}

// Game-unit vtable slot 0x3e4 called with an empty bone name, as every lock-on caller does
// (DAT_3344f1f4 = L""). CX2GUUser 31fd61a0: GetPos(). CX2GUNPC 3275ac50: take the frame name of the
// collision point nearest vFrom_ (32278580: strict <, from 99999; CollisionData +0x24 = m_FrameName),
// that bone's position, and the unit position; within 700 (3344ce98) of each other -> the position,
// else whichever is nearer vFrom_ (a tie -> the bone). Not ported: the tail @3275ae4f-3275b01d, which
// in a flagged battle field (32481fa0) with an unidentified mode test (3266ce90 / 3266ce80) falls back
// to the position when the bone position is zero.
D3DXVECTOR3 CX2DamageEffect::CEffect::JuneGetLockOnAimPos( const CX2GameUnit* pUnit_, const D3DXVECTOR3& vFrom_ )
{
	if( CX2GameUnit::GUT_NPC != pUnit_->GetGameUnitType() )
		return pUnit_->GetPos();

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
			const float fDist = GetDistance( vFrom_, collisionData->GetPointStart() );
			if( fDist < fNearest )
			{
				fNearest = fDist;
				wstrNearestFrame = collisionData->m_FrameName;
			}
		}
	}

	const D3DXVECTOR3 vBonePos = pUnit_->GetBonePos( wstrNearestFrame.c_str() );	// vtable slot 0x84
	const D3DXVECTOR3 vPos = pUnit_->GetPos();
	if( GetDistance( vPos, vBonePos ) <= 700.f )
		return vPos;
	return ( GetDistance( vFrom_, vBonePos ) > GetDistance( vFrom_, vPos ) ) ? vPos : vBonePos;
}

#ifdef SERV_IRUHADEV_JUNE_DAMAGE_MATH
// June effect completion C4. June 32622780 (a DamageData thiscall; the only caller is GetFinalDamage @322857f6): DIVIDE_DAMAGE_FROM_RANGE.
// It runs when the range map is non-empty (this+0x5b8), the game and its damage-effect manager exist (game +0x3b4), the DamageData's
// attacker effect handle (+0x14, resolved by 31f64190) gives an effect and the defender (+0x470) is live. dist = the distance (31c7b560)
// from the effect's position (+0x100 = m_Pos) to the defender's lock-on aim point (vtable slot 0x3e4 with an empty bone name, queried from
// the effect position = JuneGetLockOnAimPos). The map is walked in ascending RANGE order and the rate is the value of the last entry
// whose RANGE is not above dist; 1.0 before any. Only a rate below 1.0 scales anything: physic and magic locals are both multiplied.
// Polarities from june2017_re's x87 emulation (scratchpad c4_x87.py): the walk stops at the first RANGE with dist < RANGE, so a NaN
// distance takes every entry; the multiply needs rate < 1.0 strictly (1.0, above 1.0 and NaN do nothing: JUNE_BUGS).
void CX2DamageEffect::CEffect::JuneApplyDivideDamageFromRange( const CX2DamageManager::DamageData& damageData_, float& fPhysic_, float& fMagic_ )
{
	if ( true == damageData_.m_mapDivideDamageFromRange.empty() )		// @326227c0-@326227c8
		return;
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )	// @326227ce-@3262283d
		return;

	CX2GameUnit* pDefender = damageData_.optrDefenderGameUnit.GetObservable();		// @32622843-@3262285b
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( damageData_.hAttackerEffect );	// 31f64190 @3262288a
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	CEffect* pEffect = NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	if ( NULL == pDefender || NULL == pEffect )						// @3262288f-@32622899
		return;

	const D3DXVECTOR3 vEffectPos = pEffect->GetPos();					// @3262289f-@326228b3
	const D3DXVECTOR3 vAimPos = JuneGetLockOnAimPos( pDefender, vEffectPos );	// vslot 0x3e4 @326228ec
	const float fDist = GetDistance( vEffectPos, vAimPos );			// 31c7b560 @326228f4

	float fRate = 1.f;												// @32622909
	for ( std::map<float,float>::const_iterator it = damageData_.m_mapDivideDamageFromRange.begin();
		it != damageData_.m_mapDivideDamageFromRange.end() && !( fDist < it->first ); ++it )	// @32622920-@32622932
	{
		fRate = it->second;											// @32622934
	}

	if ( fRate < 1.f )												// @3262294e-@32622959 (compare with the double 1.0 @334355b0)
	{
		fPhysic_ *= fRate;											// @3262295b-@32622964
		fMagic_ *= fRate;											// @32622967-@32622969
#if defined( SERV_IRUHADEV_JUNE_FX_DEBUG ) && defined( SERV_IRUHADEV_OFFLINE )
		static bool s_bRangeLogged = false;
		if ( false == s_bRangeLogged )
		{
			s_bRangeLogged = true;
			CX2OfflineLog::Server( L"JUNE_FX DIVIDE_DAMAGE_FROM_RANGE scaled a hit (first seen): defender %I64d, distance %.1f, rate %.3f",
				pDefender->GetUnitUID(), fDist, fRate );
		}
#endif // SERV_IRUHADEV_JUNE_FX_DEBUG && SERV_IRUHADEV_OFFLINE
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGE_MATH
// June2017 FUN_322dadd0 (RET 0x14): the seeded pick. Candidates pass JuneIsLockOnCandidate and have
// their aim point (slot 0x3e4 from the owner) inside LOCK_ON_RANGE_FROM_OWNER; the pick is
// eligible[ (unsigned)( seed % modulus + 1 ) % count ] (IDIV @322dae65, DIV @322db035). March's
// CX2Game::GetLockOnTarget walks the list cyclically instead and finds nothing at seed -1.
UidType CX2DamageEffect::CEffect::JunePickLockOnTargetBySeed( int iSeed_, OUT bool& bUserUnit_, int iModulus_ /*= 30*/ )
{
	if( NULL == g_pX2Game || null == m_optrOwnerUnit )
		return -1;

	const int iTeam = m_optrOwnerUnit->GetTeam();
	const D3DXVECTOR3 vOwnerPos = m_optrOwnerUnit->GetPos();
	const int iKey = iSeed_ % iModulus_ + 1;

	std::vector<const CX2GameUnit*> vecEligible;
	const std::vector<CX2GameUnit*>& vecUnit = g_pX2Game->GetUnitList();
	for( UINT uiIndex = 0; uiIndex < vecUnit.size(); ++uiIndex )
	{
		const CX2GameUnit* pUnit = vecUnit[ uiIndex ];
		if( false == JuneIsLockOnCandidate( pUnit, iTeam ) )
			continue;
		if( true == JuneIsOutOfLockOnRangeFromOwner( JuneGetLockOnAimPos( pUnit, vOwnerPos ) ) )
			continue;
		vecEligible.push_back( pUnit );
	}

	if( true == vecEligible.empty() )
	{
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"SEED_PICK", L"seed=%d key=%d eligible=0 -> none", iSeed_, iKey );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		return -1;
	}

	const UINT uiPick = static_cast<UINT>( iKey ) % static_cast<UINT>( vecEligible.size() );
	const CX2GameUnit* pPick = vecEligible[ uiPick ];
	bUserUnit_ = ( CX2GameUnit::GUT_USER == pPick->GetGameUnitType() );
	const UidType uid = pPick->GetUnitUID();
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"SEED_PICK", L"seed=%d key=%d eligible=%u index=%u -> uid=%I64d", iSeed_, iKey,
		static_cast<UINT>( vecEligible.size() ), uiPick, uid );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	return uid;
}

// June2017 FUN_322d7670: March's GetLockOnNearstTargetInRange (range.x < d < range.y, nearest), with
// BALANCE_DEADLY_CHASER's game-wide pick list replaced by this effect's own hit list and its
// NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE. The list is used only when the caller passes it (the value is
// non-zero). On a miss (@322d783f-322d78cd): CLEAR empties it, SAVE_ONE empties it when it holds more
// than one unit, SAVE_HIT_UNIT forgets the saved unit and searches again if that unit is listed, else
// empties it. June discards that second search's result and returns -1 (the studio's code; kept).
UidType CX2DamageEffect::CEffect::JuneGetLockOnNearstTargetInRange( int iTeam_, const D3DXVECTOR3& vPos_, const D3DXVECTOR2& vRange_, bool bUseTargetList_, OUT bool& bUserUnit_ )
{
	if( NULL == g_pX2Game )
		return -1;

	const int iListType = ( true == bUseTargetList_ ) ? m_LockOnData.m_iNotTargetPreLockOnTarget : static_cast<int>( NTPLOTT_NONE );
	std::set<UidType>& setList = m_LockOnData.m_setPreLockOnTarget;
	const CX2GameUnit* pNearst = NULL;
	float fNearst = vRange_.y;
	int iSkippedByList = 0;

	const std::vector<CX2GameUnit*>& vecUnit = g_pX2Game->GetUnitList();
	for( UINT uiIndex = 0; uiIndex < vecUnit.size(); ++uiIndex )
	{
		const CX2GameUnit* pUnit = vecUnit[ uiIndex ];
		if( false == JuneIsLockOnCandidate( pUnit, iTeam_ ) )
			continue;

		if( true == bUseTargetList_ )
		{
			const UidType uid = pUnit->GetUnitUID();
			if( NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT == iListType && uid == m_LockOnData.m_SaveHitUnitUID )
			{
				++iSkippedByList;
				continue;
			}
			if( NTPLOTT_NONE != iListType && false == setList.empty() && setList.end() != setList.find( uid ) )
			{
				++iSkippedByList;
				continue;
			}
		}

		const float fDistance = GetDistance( vPos_, pUnit->GetPos() );
		if( fDistance < fNearst && fDistance > vRange_.x )
		{
			pNearst = pUnit;
			fNearst = fDistance;
		}
	}

	if( NULL != pNearst )
	{
		bUserUnit_ = ( CX2GameUnit::GUT_USER == pNearst->GetGameUnitType() );
		const UidType uid = pNearst->GetUnitUID();
		if( true == bUseTargetList_ && NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT == iListType )
			m_LockOnData.m_SaveHitUnitUID = uid;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"IN_RANGE_PICK", L"list=%d set=%u skipped=%d range=(%.0f,%.0f) -> uid=%I64d dist=%.0f",
			iListType, static_cast<UINT>( setList.size() ), iSkippedByList, vRange_.x, vRange_.y, uid, fNearst );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		return uid;
	}

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"IN_RANGE_MISS", L"list=%d set=%u skipped=%d range=(%.0f,%.0f)",
		iListType, static_cast<UINT>( setList.size() ), iSkippedByList, vRange_.x, vRange_.y );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	if( false == bUseTargetList_ )
		return -1;

	switch( iListType )
	{
	case NTPLOTT_TARGET_LIST_CLEAR:
		setList.clear();
		break;
	case NTPLOTT_TARGET_LIST_SAVE_ONE:
		if( setList.size() > 1 )
			setList.clear();
		break;
	case NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT:
		if( setList.end() != setList.find( m_LockOnData.m_SaveHitUnitUID ) )
		{
			m_LockOnData.m_SaveHitUnitUID = -1;
			JuneGetLockOnNearstTargetInRange( iTeam_, vPos_, vRange_, bUseTargetList_, bUserUnit_ );
		}
		else
			setList.clear();
		break;
	default:
		break;
	}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"TARGET_LIST_MISS_RULE", L"list=%d -> set=%u", iListType, static_cast<UINT>( setList.size() ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	return -1;
}

// June2017 FUN_322daa10 (LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE only): a RANDOM pick among the
// candidates within range.y of vPos_ (squared test, 31c7b5c0; range.x unused), not on this effect's
// hit list, with the aim point inside LOCK_ON_RANGE_FROM_OWNER. Index (unsigned)( rand() % 100 ) %
// count. On a miss: CLEAR and SAVE_HIT_UNIT empty the list, SAVE_ONE when it holds more than one.
UidType CX2DamageEffect::CEffect::JuneGetLockOnRandomTargetInRange( int iTeam_, const D3DXVECTOR3& vPos_, const D3DXVECTOR2& vRange_, OUT bool& bUserUnit_ )
{
	if( NULL == g_pX2Game || null == m_optrOwnerUnit )
		return -1;

	const D3DXVECTOR3 vOwnerPos = m_optrOwnerUnit->GetPos();
	const int iListType = m_LockOnData.m_iNotTargetPreLockOnTarget;
	std::set<UidType>& setList = m_LockOnData.m_setPreLockOnTarget;
	const float fRangeSq = vRange_.y * vRange_.y;

	std::vector<const CX2GameUnit*> vecEligible;
	const std::vector<CX2GameUnit*>& vecUnit = g_pX2Game->GetUnitList();
	for( UINT uiIndex = 0; uiIndex < vecUnit.size(); ++uiIndex )
	{
		const CX2GameUnit* pUnit = vecUnit[ uiIndex ];
		if( false == JuneIsLockOnCandidate( pUnit, iTeam_ ) )
			continue;
		if( NTPLOTT_NONE != iListType && false == setList.empty() && setList.end() != setList.find( pUnit->GetUnitUID() ) )
			continue;
		const D3DXVECTOR3 vDiff = pUnit->GetPos() - vPos_;
		if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
			continue;
		if( true == JuneIsOutOfLockOnRangeFromOwner( JuneGetLockOnAimPos( pUnit, vOwnerPos ) ) )
			continue;
		vecEligible.push_back( pUnit );
	}

	if( false == vecEligible.empty() )
	{
		const UINT uiPick = static_cast<UINT>( rand() % 100 ) % static_cast<UINT>( vecEligible.size() );
		const CX2GameUnit* pPick = vecEligible[ uiPick ];
		bUserUnit_ = ( CX2GameUnit::GUT_USER == pPick->GetGameUnitType() );
		const UidType uid = pPick->GetUnitUID();
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxLockOn( GetName(), L"RANDOM_PICK", L"list=%d set=%u eligible=%u index=%u -> uid=%I64d",
			iListType, static_cast<UINT>( setList.size() ), static_cast<UINT>( vecEligible.size() ), uiPick, uid );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		return uid;
	}

	switch( iListType )
	{
	case NTPLOTT_TARGET_LIST_CLEAR:
	case NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT:
		setList.clear();
		break;
	case NTPLOTT_TARGET_LIST_SAVE_ONE:
		if( setList.size() > 1 )
			setList.clear();
		break;
	default:
		break;
	}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"RANDOM_MISS", L"list=%d -> set=%u", iListType, static_cast<UINT>( setList.size() ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	return -1;
}

// June2017 FUN_31f83aa0 @31f8497c-31f84a0e (cases 6 and 8, no-target path): with a keeping list type
// (NOT_CLEAR / SAVE_ONE / SAVE_HIT_UNIT) the effect steers at the point one DirSpeed ahead along its
// heading at its own height -- it levels off. 31c82e40 = CXMeshInstance::GetDirVec, mesh +0x5cc =
// DirSpeed.
void CX2DamageEffect::CEffect::JuneLockOnSteerForward( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_ )
{
	switch( m_LockOnData.m_iNotTargetPreLockOnTarget )
	{
	case NTPLOTT_TARGET_LIST_NOT_CLEAR:
	case NTPLOTT_TARGET_LIST_SAVE_ONE:
	case NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT:
		break;
	default:
		return;
	}
	if( NULL == pMeshInst_ )
		return;

	const float fDirSpeed = pMeshInst_->GetDirSpeed();
	const D3DXVECTOR3 vDir = pMeshInst_->GetDirVec();
	const D3DXVECTOR3 vMeshPos = pMeshInst_->GetPos();
	const D3DXVECTOR3 vAhead( vMeshPos.x + fDirSpeed * vDir.x, vMeshPos.y, vMeshPos.z + fDirSpeed * vDir.z );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"STEER_FORWARD", L"list=%d speed=%.0f", m_LockOnData.m_iNotTargetPreLockOnTarget, fDirSpeed );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	VectorLockOn( vAhead );
}

// June2017 FUN_3262b6e0 (CX2DamageManager::EffectToUnit) @3262f389-3262f3ca / @3262f4b9-3262f4fb:
// after a non-laser-group hit is recorded, a non-zero NOT_TARGET_PRE_LOCK_ON_TARGET puts the hit
// unit on this effect's list (set<__int64>::insert 32641140). June fills the list nowhere else.
void CX2DamageEffect::CEffect::JuneOnLockOnHitUnit( CX2GameUnitoPtr optrHitUnit_ )
{
	if( NTPLOTT_NONE == m_LockOnData.m_iNotTargetPreLockOnTarget || null == optrHitUnit_ )
		return;
	const UidType uid = optrHitUnit_->GetUnitUID();
	m_LockOnData.m_setPreLockOnTarget.insert( uid );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxLockOn( GetName(), L"TARGET_LIST_INSERT", L"list=%d uid=%I64d -> set=%u", m_LockOnData.m_iNotTargetPreLockOnTarget, uid,
		static_cast<UINT>( m_LockOnData.m_setPreLockOnTarget.size() ) );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

void CX2DamageEffect::CEffect::SetPos( const D3DXVECTOR3& pos )
{
	m_Pos = pos;

	if( NULL != GetMainEffect() )
	{
		GetMainEffect()->SetPos( pos );
	}


	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetMajorParticle() )
	{

		BOOST_TEST_FOREACH( ParticleData*, pParticleData, m_PassiveParticleDataList )
		{
			if( INVALID_PARTICLE_SEQUENCE_HANDLE != pParticleData->m_hSeq )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pParticleData->m_hSeq );
				if( NULL != pSeq && pParticleData->posType == CX2DamageEffect::DEPT_POS)
				{
					pSeq->SetPosition( pos );
				}
			}
		}
	}
}

//{{ kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(독구름)
#ifdef DAMAGE_EFFECT_WORK
void CX2DamageEffect::CEffect::LoadDieDamageEffect()
{
	KTDXPROFILE();

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f6e7e0 @31f6e7e3-31f6e822 (the wrapper's own inline gate: owner-unit/memo
	// pointer checks, then FUN_31fcf3e0([ESI+0x7c4]) -- the same unidentified skill-memo callee
	// Phase 5's ctor calls to compute m_bUseUltimateDamageData, not literally that member, but
	// forced off for the same reason: the system it reads is absent). When it passes,
	// FUN_31f68f30 is called with "DIE_DAMAGE_EFFECT_ULTIMATE" first (@31f6e824); only if that
	// loop reads nothing at all (index 0 missing -> false, @31f6a775) does the wrapper fall back
	// to "DIE_DAMAGE_EFFECT" (@31f6e834) -- the loop below, left byte-identical. Forced false
	// today, so this block is dead; kept parseable for when Phase 7/8 can identify FUN_31fcf3e0.
	if( true == m_bUseUltimateDamageData )
	{
		bool bDieUltimateFoundAny = false;
		int indexUlt = 0;
		while( m_pLuaManager->BeginTable( "DIE_DAMAGE_EFFECT_ULTIMATE", indexUlt ) == true )
		{
			bDieUltimateFoundAny = true;

			DamageEffectData* pData = new DamageEffectData;

			PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,				CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
			PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
			PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,			L""		);
			PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,		true	);
			PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,	false	);
			PLUA_GET_VALUE(			m_pLuaManager, "ONLY_APPLY_ROTATE",		pData->bOnlyApplyRotateOffset,		false	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "LAND_CRASH_ONLY",		pData->bLandCrashOnly,	false	);
			PLUA_GET_VALUE(			m_pLuaManager, "LIFE_TIME_ONLY",		pData->bLifeTimeOnly,	false	);
			PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );
			PLUA_GET_VALUE(			m_pLuaManager, "INHERITLOCKONDATA",	pData->bInheritLockOnData,	false	);
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",				pData->vScale.x,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",				pData->vScale.y,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",				pData->vScale.z,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",	pData->bInheritLifeTime,	false	);

			m_DamageEffectDataInLua.m_vecDieDamageEffect.push_back( pData );

			indexUlt++;
			m_pLuaManager->EndTable();
		}
		if( true == bDieUltimateFoundAny )
			return;
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	int index = 0;
	while( m_pLuaManager->BeginTable( "DIE_DAMAGE_EFFECT", index ) == true )
	{

		DamageEffectData* pData = new DamageEffectData;

		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,				CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,			L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,		true	);

#ifdef SPECIAL_USE_ITEM
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,	false	);
#endif SPECIAL_USE_ITEM

#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		PLUA_GET_VALUE(			m_pLuaManager, "ONLY_APPLY_ROTATE",		pData->bOnlyApplyRotateOffset,		false	);
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);

#ifdef SERV_RENA_NIGHT_WATCHER
		PLUA_GET_VALUE(			m_pLuaManager, "LAND_CRASH_ONLY",		pData->bLandCrashOnly,	false	);
		PLUA_GET_VALUE(			m_pLuaManager, "LIFE_TIME_ONLY",		pData->bLifeTimeOnly,	false	);
#endif SERV_RENA_NIGHT_WATCHER

#ifdef SERV_RAVEN_VETERAN_COMMANDER
		PLUA_GET_VALUE(			m_pLuaManager, "SET_GROUP_ID",			pData->iGroupID,		-1	);		/// 그룹 데미지 설정 추가
#endif SERV_RAVEN_VETERAN_COMMANDER

		/// 버프팩터를 상속 받을 것인가?
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
		// June2017 FUN_31f68f30 (die damage-effect body, via wrapper FUN_31f6e7e0)
		PLUA_GET_VALUE(			m_pLuaManager, "INHERITLOCKONDATA",	pData->bInheritLockOnData,	false	);	/// June @31f6a1fe->31f6a2ee, bool, default false
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",				pData->vScale.x,			1.0f	);	/// June @31f6a473->31f6a4de, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",				pData->vScale.y,			1.0f	);	/// June @31f6a520->31f6a58b, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",				pData->vScale.z,			1.0f	);	/// June @31f6a5cd->31f6a638, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",	pData->bInheritLifeTime,	false	);	/// June @31f6a343->31f6a433, bool, default false
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

		m_DamageEffectDataInLua.m_vecDieDamageEffect.push_back( pData );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		// JUNE_FX DDE_TEMPLET <effect> DIE_DAMAGE_EFFECT<n> <key>=<v> ..., only for a non-default Phase-6a key
		{
			WCHAR wszJuneFxDde[512] = {0};
			StringCchPrintfW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L"JUNE_FX DDE_TEMPLET %s DIE_DAMAGE_EFFECT%d", GetName(), index );
			const size_t uiDdeNameOnly = wcslen( wszJuneFxDde );
			WCHAR wszDdeKey[128] = {0};
			if( true == pData->bInheritLockOnData )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" INHERITLOCKONDATA=1" );
			if( 1.0f != pData->vScale.x || 1.0f != pData->vScale.y || 1.0f != pData->vScale.z )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" SCALE=(%.3f,%.3f,%.3f)", pData->vScale.x, pData->vScale.y, pData->vScale.z );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( true == pData->bInheritLifeTime )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" INHERIT_LIFE_TIME=1" );
			if( wcslen( wszJuneFxDde ) != uiDdeNameOnly )
			{
				static map<wstring, DWORD> s_mapLastLoggedTime_dieTemplet;
				DWORD dwNow_dieTemplet = timeGetTime();
				map<wstring, DWORD>::iterator itThrottle_dieTemplet = s_mapLastLoggedTime_dieTemplet.find( GetName() );
				if( itThrottle_dieTemplet == s_mapLastLoggedTime_dieTemplet.end() || dwNow_dieTemplet - itThrottle_dieTemplet->second >= 1000 )
				{
					s_mapLastLoggedTime_dieTemplet[ GetName() ] = dwNow_dieTemplet;
					CX2OfflineLog::Server( L"%s", wszJuneFxDde );
				}
			}
		}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadStartParticle()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "START_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();

#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
		{
			if( pData->m_iShowLevel >= g_pMain->GetGameOption().GetOptionList().m_eEffect )
				m_DamageEffectDataInLua.m_vecStartParticle.push_back( pData );
		}
#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
			m_DamageEffectDataInLua.m_vecStartParticle.push_back( pData );
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadPassiveParticle()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "PASSIVE_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();

#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
		{
			if( pData->m_iShowLevel >= g_pMain->GetGameOption().GetOptionList().m_eEffect )
				m_DamageEffectDataInLua.m_vecPassiveParticle.push_back( pData );
		}
#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
			m_DamageEffectDataInLua.m_vecPassiveParticle.push_back( pData );
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadTimeParticle()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "TIME_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();

#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
		{
			if( pData->m_iShowLevel >= g_pMain->GetGameOption().GetOptionList().m_eEffect )
				m_DamageEffectDataInLua.m_vecTimeParticle.push_back( pData );
		}
#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
			m_DamageEffectDataInLua.m_vecTimeParticle.push_back( pData );
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadCameraData()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "CAMERA", index ) == true )
	{
		CameraData data;
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "UNIT_SELECT",		data.unitSelect,	UNIT_SELECT,	DEUS_ALL );
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "CRASH_TYPE",		data.crashType,		CKTDGCamera::SHAKE_TYPE,		CKTDGCamera::DECT_UP_DOWN );
		PLUA_GET_VALUE(			m_pLuaManager, "SPEED",				data.fSpeed,		0 );
		PLUA_GET_VALUE(			m_pLuaManager, "ACCEL",				data.fAccel,		0 );
		PLUA_GET_VALUE(			m_pLuaManager, "GAP",				data.fGap,			0 );
		PLUA_GET_VALUE(			m_pLuaManager, "TIME",				data.fTime,			0 );
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,	0 );				

		m_DamageEffectDataInLua.m_vecCameraData.push_back( data );

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadScreenData()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "SCREEN", index ) == true )
	{
		ScreenData data;
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "UNIT_SELECT",		data.unitSelect,	UNIT_SELECT,	DEUS_ALL );
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,	0		);
		PLUA_GET_VALUE(			m_pLuaManager, "COUNT",				data.clearCount,	1		);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_A",			data.color.a,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_R",			data.color.r,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_G",			data.color.g,		1.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "COLOR_B",			data.color.b,		1.0f	);

		m_DamageEffectDataInLua.m_vecScreenData.push_back( data );

		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadSoundData()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "SOUND", index ) == true )
	{
		SoundData data;
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",		data.fEventTime,		0		);
		PLUA_GET_VALUE(			m_pLuaManager, "FILE_NAME",		data.fileName,			L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "RATE",				data.iRate,				100		);
		PLUA_GET_VALUE(			m_pLuaManager, "USE_GLOBAL_TIME",	data.bUseGlobalTime,	false	);

		PLUA_GET_VALUE(			m_pLuaManager, "LOOP",	data.bLoop,	false	);	// 동작안할꺼임..... 왜냐.. 플레이하는곳 note확인해보삼
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
		// June2017 FUN_31f73f30 (LoadSoundData). Phase 6b: parse only.
		PLUA_GET_VALUE(			m_pLuaManager, "DELETE_WHEN_EFFECT_END",	data.bDeleteWhenEffectEnd,		false	);	/// June @31f744af, bool getter 32fe9e40, default false
		PLUA_GET_VALUE(			m_pLuaManager, "THREE_DIMENSIONAL_SOUND",	data.bThreeDimensionalSound,	true	);	/// June @31f74649, bool getter 32fe9e40, default true
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
		
		m_DamageEffectDataInLua.m_vecSoundData.push_back( data );
		index++;
		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadDamageEffect()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "CREATE_DAMAGE_EFFECT", index ) == true )
	{
		DamageEffectData* pData = new DamageEffectData;
		PLUA_GET_VALUE(			m_pLuaManager, "EVENT_TIME",			pData->fEventTime,	-1.0f		);
		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",		pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",			pData->boneName,		L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,	true	);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",	pData->bApplyRotateOrigin,	false	);

		//{{ oasis907 : 김상윤 [2010.2.10] //  
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,	false	);
		//}}

		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_MATRIX",			pData->bApplyMatrix,	false	);
		PLUA_GET_VALUE(			m_pLuaManager, "AUTO_DIE",				pData->bAutoDie,		false	);		
#endif

#ifdef SERV_RAVEN_VETERAN_COMMANDER
		PLUA_GET_VALUE(			m_pLuaManager, "SET_GROUP_ID",			pData->iGroupID,		-1 );		/// 그룹 데미지 설정 추가
#endif SERV_RAVEN_VETERAN_COMMANDER

		/// 버프팩터를 상속 받을 것인가?
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );

#ifdef LOOP_CREATE_DAMAGE_EFFECT // 김태환
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_INTERVAL",			pData->fLoopInterval,	0.f		);	/// 반복 생성 간격
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_END_COUNT",		pData->uiLoopEndCount,	1		);	/// 반복 생성 종료 횟수
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_END_TIME",			pData->fLoopEndTime,	99999.f );	/// 반복 생성 종료 시간
#endif //LOOP_CREATE_DAMAGE_EFFECT

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
		// June2017 FUN_31f6ac60 (CREATE_DAMAGE_EFFECT%d loader). LOOP_CREATE_DAMAGE_EFFECT above is
		// never defined under US_SERVICE (census.py found these three keys to-implement despite the
		// dead code above already naming them), so these are fresh reads under our own flag.
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_INTERVAL",					pData->fLoopInterval,				0.0f		);	/// June @31f6bf7a->31f6c063, float, default 0.f
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_END_COUNT",				pData->uiLoopEndCount,				1			);	/// June @31f6c0b7->31f6c1b0, int, default 1
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_END_TIME",					pData->fLoopEndTime,				99999.0f	);	/// June @31f6c202->31f6c2eb, float, default 99999.f
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_ADD_POS_X",				pData->vLoopAddPos.x,				0.0f		);	/// June @31f6c343->31f6c43c, float, default 0.f
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_ADD_POS_Y",				pData->vLoopAddPos.y,				0.0f		);	/// June @31f6c490->31f6c589, float, default 0.f
		PLUA_GET_VALUE(			m_pLuaManager, "LOOP_ADD_POS_Z",				pData->vLoopAddPos.z,				0.0f		);	/// June @31f6c5dd->31f6c6d6, float, default 0.f (binary-only: no June data uses it)
		PLUA_GET_VALUE(			m_pLuaManager, "SET_DIE_WHEN_MASTER_EFFECT_DIE",	pData->bSetDieWhenMasterEffectDie,	false		);	/// June @31f6c752->31f6c8eb, bool, default false
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",				pData->bInheritLifeTime,			false		);	/// June @31f6c943->31f6ca33, bool, default false
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_DAMAGE",					pData->bInheritDamage,				false		);	/// June @31f6ca85->31f6cb7e, bool, default false
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

		
		m_DamageEffectDataInLua.m_vecCreateDamageEffect.push_back( pData );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		// JUNE_FX DDE_TEMPLET <effect> CREATE_DAMAGE_EFFECT<n> <key>=<v> ..., only for a non-default Phase-6a key
		{
			WCHAR wszJuneFxDde[512] = {0};
			StringCchPrintfW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L"JUNE_FX DDE_TEMPLET %s CREATE_DAMAGE_EFFECT%d", GetName(), index );
			const size_t uiDdeNameOnly = wcslen( wszJuneFxDde );
			WCHAR wszDdeKey[128] = {0};
			if( 0.0f != pData->fLoopInterval )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" LOOP_INTERVAL=%.3f", pData->fLoopInterval );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( 1 != pData->uiLoopEndCount )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" LOOP_END_COUNT=%d", pData->uiLoopEndCount );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( 99999.0f != pData->fLoopEndTime )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" LOOP_END_TIME=%.3f", pData->fLoopEndTime );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( 0.0f != pData->vLoopAddPos.x || 0.0f != pData->vLoopAddPos.y || 0.0f != pData->vLoopAddPos.z )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" LOOP_ADD_POS=(%.3f,%.3f,%.3f)", pData->vLoopAddPos.x, pData->vLoopAddPos.y, pData->vLoopAddPos.z );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( true == pData->bSetDieWhenMasterEffectDie )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" SET_DIE_WHEN_MASTER_EFFECT_DIE=1" );
			if( true == pData->bInheritLifeTime )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" INHERIT_LIFE_TIME=1" );
			if( true == pData->bInheritDamage )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" INHERIT_DAMAGE=1" );
			if( wcslen( wszJuneFxDde ) != uiDdeNameOnly )
			{
				static map<wstring, DWORD> s_mapLastLoggedTime_createTemplet;
				DWORD dwNow_createTemplet = timeGetTime();
				map<wstring, DWORD>::iterator itThrottle_createTemplet = s_mapLastLoggedTime_createTemplet.find( GetName() );
				if( itThrottle_createTemplet == s_mapLastLoggedTime_createTemplet.end() || dwNow_createTemplet - itThrottle_createTemplet->second >= 1000 )
				{
					s_mapLastLoggedTime_createTemplet[ GetName() ] = dwNow_createTemplet;
					CX2OfflineLog::Server( L"%s", wszJuneFxDde );
				}
			}
		}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		index++;
		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadHitParticle()
{
	KTDXPROFILE();

	int index = 0;
	while( m_pLuaManager->BeginTable( "HIT_PARTICLE", index ) == true )
	{
		ParticleData* pData = LoadParticleTable();
		
#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if( pData != NULL )
		{
			if( pData->m_iShowLevel >= g_pMain->GetGameOption().GetOptionList().m_eEffect )
				m_DamageEffectDataInLua.m_vecHitParticle.push_back( pData );
		}
#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		if ( pData != NULL )
			m_DamageEffectDataInLua.m_vecHitParticle.push_back( pData );
#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
		
		index++;

		m_pLuaManager->EndTable();
	}
}

void CX2DamageEffect::CEffect::LoadHitDamageEffect()
{
	KTDXPROFILE();

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
	// June2017 FUN_31f6e850 @31f6e853-31f6e892: byte-identical gate shape to FUN_31f6e7e0
	// (LoadDieDamageEffect's comment above), forced off for the same reason. When it passes,
	// FUN_31f6ceb0 is called with "HIT_DAMAGE_EFFECT_ULTIMATE" first (@31f6e894); only if that
	// loop reads nothing at all does the wrapper fall back to "HIT_DAMAGE_EFFECT" (@31f6e8a4)
	// -- the loop below, left byte-identical. Forced false today, so this block is dead; kept
	// parseable for when Phase 7/8 can identify FUN_31fcf3e0.
	if( true == m_bUseUltimateDamageData )
	{
		bool bHitUltimateFoundAny = false;
		int indexUlt = 0;
		while( m_pLuaManager->BeginTable( "HIT_DAMAGE_EFFECT_ULTIMATE", indexUlt ) == true )
		{
			bHitUltimateFoundAny = true;

			DamageEffectData* pData = new DamageEffectData;

			PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,					CX2DamageEffect::POSITION_TYPE, DEPT_BONE	);
			PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,			L""		);
			PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,					L""		);
			PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,				true	);
			PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",	pData->bApplyRotateOrigin,			false	);
			PLUA_GET_VALUE(			m_pLuaManager, "ONLY_APPLY_ROTATE",		pData->bOnlyApplyRotateOffset,		false	);
			PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,			false	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_X",	pData->vMainEffectScale.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Y",	pData->vMainEffectScale.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Z",	pData->vMainEffectScale.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_X",	pData->vPassiveParticleScale.x,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Y",	pData->vPassiveParticleScale.y,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Z",	pData->vPassiveParticleScale.z,	0.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",				pData->vScale.x,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",				pData->vScale.y,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",				pData->vScale.z,			1.0f	);
			PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",	pData->bInheritLifeTime,	false	);

			m_DamageEffectDataInLua.m_vecHitDamageEffect.push_back( pData );

			indexUlt++;
			m_pLuaManager->EndTable();
		}
		if( true == bHitUltimateFoundAny )
			return;
	}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

	int index = 0;
	while( m_pLuaManager->BeginTable( "HIT_DAMAGE_EFFECT", index ) == true )
	{

		DamageEffectData* pData = new DamageEffectData;

		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,						CX2DamageEffect::POSITION_TYPE, DEPT_BONE	);
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,			L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,					L""		);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,				true	);
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",	pData->bApplyRotateOrigin,			false	);

#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		PLUA_GET_VALUE(			m_pLuaManager, "ONLY_APPLY_ROTATE",		pData->bOnlyApplyRotateOffset,		false	);
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

#ifdef SPECIAL_USE_ITEM
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,			false	);
#endif SPECIAL_USE_ITEM


		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_X",	pData->vMainEffectScale.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Y",	pData->vMainEffectScale.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "MAIN_EFFECT_SCALE_Z",	pData->vMainEffectScale.z,	0.0f	);

		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_X",	pData->vPassiveParticleScale.x,	0.0f	);
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Y",	pData->vPassiveParticleScale.y,	0.0f	);        
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Z",	pData->vPassiveParticleScale.z,	0.0f	);

		/// 버프팩터를 상속 받을 것인가?
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
		// June2017 FUN_31f6ceb0 (hit damage-effect body, via wrapper FUN_31f6e850)
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",				pData->vScale.x,			1.0f	);	/// June @31f6ddb3->31f6de1e, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",				pData->vScale.y,			1.0f	);	/// June @31f6de60->31f6decb, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",				pData->vScale.z,			1.0f	);	/// June @31f6df0d->31f6df78, float, default 1.f
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",	pData->bInheritLifeTime,	false	);	/// June @31f6e536->31f6e65f, bool, default false
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

		m_DamageEffectDataInLua.m_vecHitDamageEffect.push_back( pData );
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		// JUNE_FX DDE_TEMPLET <effect> HIT_DAMAGE_EFFECT<n> <key>=<v> ..., only for a non-default Phase-6a key
		{
			WCHAR wszJuneFxDde[512] = {0};
			StringCchPrintfW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L"JUNE_FX DDE_TEMPLET %s HIT_DAMAGE_EFFECT%d", GetName(), index );
			const size_t uiDdeNameOnly = wcslen( wszJuneFxDde );
			WCHAR wszDdeKey[128] = {0};
			if( 1.0f != pData->vScale.x || 1.0f != pData->vScale.y || 1.0f != pData->vScale.z )
			{
				StringCchPrintfW( wszDdeKey, ARRAY_SIZE(wszDdeKey), L" SCALE=(%.3f,%.3f,%.3f)", pData->vScale.x, pData->vScale.y, pData->vScale.z );
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), wszDdeKey );
			}
			if( true == pData->bInheritLifeTime )
				StringCchCatW( wszJuneFxDde, ARRAY_SIZE(wszJuneFxDde), L" INHERIT_LIFE_TIME=1" );
			if( wcslen( wszJuneFxDde ) != uiDdeNameOnly )
			{
				static map<wstring, DWORD> s_mapLastLoggedTime_hitTemplet;
				DWORD dwNow_hitTemplet = timeGetTime();
				map<wstring, DWORD>::iterator itThrottle_hitTemplet = s_mapLastLoggedTime_hitTemplet.find( GetName() );
				if( itThrottle_hitTemplet == s_mapLastLoggedTime_hitTemplet.end() || dwNow_hitTemplet - itThrottle_hitTemplet->second >= 1000 )
				{
					s_mapLastLoggedTime_hitTemplet[ GetName() ] = dwNow_hitTemplet;
					CX2OfflineLog::Server( L"%s", wszJuneFxDde );
				}
			}
		}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

		index++;

		m_pLuaManager->EndTable();
	}
}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE
// June2017 FUN_31f64c80: HIT_EFFECT_SET%d, June-only (no March equivalent). ctor call @31f7e854.
// Phase 6b: parse only; runtime is Phase 7e. binary-only: no in-scope June data uses this sub-table.
void CX2DamageEffect::CEffect::LoadHitEffectSet()
{
	int index = 0;
	while( m_pLuaManager->BeginTable( "HIT_EFFECT_SET", index ) == true )
	{
		HitEffectSetData* pData = new HitEffectSetData;
		PLUA_GET_VALUE(		m_pLuaManager, "EVENT_TIME",		pData->fEventTime,			-1.0f	);	/// June @31f64cde, float getter 32fea370
		PLUA_GET_VALUE(		m_pLuaManager, "EFFECT_SET_NAME",	pData->strEffectSetName,	L""		);	/// June @31f64e18, wstring getter 32feae90

		m_DamageEffectDataInLua.m_vecHitEffectSet.push_back( pData );
		index++;
		m_pLuaManager->EndTable();
	}
}

// June2017 FUN_31f66230: DIE_CREATE_NPC%d, June-only (no March equivalent). ctor call @31f7e85b.
// Phase 6b: parse only; runtime is Phase 7e.
void CX2DamageEffect::CEffect::LoadDieCreateNpc()
{
	int index = 0;
	while( m_pLuaManager->BeginTable( "DIE_CREATE_NPC", index ) == true )
	{
		DieCreateNpcData data;
		PLUA_GET_VALUE(		m_pLuaManager, "EVENT_TIME",	data.fEventTime,	-1.0f	);	/// June @31f662f6, float getter 32fea370
		PLUA_GET_VALUE(		m_pLuaManager, "NPC_ID",		data.iNpcID,		0		);	/// June @31f663d1, int getter 32fe9fd0

		m_DamageEffectDataInLua.m_vecDieCreateNpc.push_back( data );
		index++;
		m_pLuaManager->EndTable();
	}
}

// June2017 FUN_31f76a90: TOUCH_LAND_DAMAGE_EFFECT%d, June-only (no March equivalent). ctor call
// @31f7e862. Reuses DamageEffectData -- same field set as CREATE/DIE/HIT_DAMAGE_EFFECT (confirmed
// by matching June's decrypted key order 1:1), except EVENT_TIME is never read at this scope.
// Phase 6b: parse only; runtime (the touch-land trigger itself) is Phase 7e.
void CX2DamageEffect::CEffect::LoadTouchLandDamageEffect()
{
	int index = 0;
	while( m_pLuaManager->BeginTable( "TOUCH_LAND_DAMAGE_EFFECT", index ) == true )
	{
		DamageEffectData* pData = new DamageEffectData;

		PLUA_GET_VALUE_ENUM(	m_pLuaManager, "POSITION_TYPE",			pData->posType,				CX2DamageEffect::POSITION_TYPE, DEPT_BONE );	/// June @31f76b93
		PLUA_GET_VALUE(			m_pLuaManager, "DAMAGE_EFFECT_NAME",	pData->damageEffectName,	L""		);	/// June @31f76cd9
		PLUA_GET_VALUE(			m_pLuaManager, "BONE_NAME",				pData->boneName,		L""		);	/// June @31f76e5e
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE",			pData->bApplyRotate,	true	);	/// June @31f76f58
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATE_ORIGIN",	pData->bApplyRotateOrigin,	false	);	/// June @31f77090
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //이창한
		PLUA_GET_VALUE(			m_pLuaManager, "ONLY_APPLY_ROTATE",		pData->bOnlyApplyRotateOffset,		false	);	/// June @31f77235
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE
		PLUA_GET_VALUE(			m_pLuaManager, "APPLY_ROTATED_OFFSET",	pData->bApplyRotatedOffset,	false	);	/// June @31f773bf
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_X",				pData->vRotateAngleDegree.x,	0.0f	);	/// June @31f7755a
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Y",				pData->vRotateAngleDegree.y,	0.0f	);	/// June @31f77631
		PLUA_GET_VALUE(			m_pLuaManager, "ROTATE_Z",				pData->vRotateAngleDegree.z,	0.0f	);	/// June @31f77708
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_X",				pData->vOffset.x,	0.0f	);	/// June @31f777df
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Y",				pData->vOffset.y,	0.0f	);	/// June @31f778b6
		PLUA_GET_VALUE(			m_pLuaManager, "OFFSET_Z",				pData->vOffset.z,	0.0f	);	/// June @31f7798d
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_X",				pData->vScale.x,		1.0f	);	/// June @31f77a68
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Y",				pData->vScale.y,		1.0f	);	/// June @31f77b33
		PLUA_GET_VALUE(			m_pLuaManager, "SCALE_Z",				pData->vScale.z,		1.0f	);	/// June @31f77c01
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_X",	pData->vPassiveParticleScale.x,	0.0f	);	/// June @31f77ceb (binary-only)
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Y",	pData->vPassiveParticleScale.y,	0.0f	);	/// June @31f77e85 (binary-only)
		PLUA_GET_VALUE(			m_pLuaManager, "PASSIVE_PARTICLE_Z",	pData->vPassiveParticleScale.z,	0.0f	);	/// June @31f7801f (binary-only)
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_BUFF_FACTOR",	pData->bInheritBuffFactor,	false );	/// June @31f781be
		PLUA_GET_VALUE(			m_pLuaManager, "INHERIT_LIFE_TIME",		pData->bInheritLifeTime,	false	);	/// June @31f78362

		m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.push_back( pData );
		index++;
		m_pLuaManager->EndTable();
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE

void CX2DamageEffect::CEffect::SetDamageDataNext( CX2DamageManager::DamageData* pDamageData )
{
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	// Code review fix: share the field-carry body with SetDamageDataNextByChainIndex
	// below (index 0 = m_pDamageDataNext) instead of keeping two copies that could
	// silently drift apart the next time a field is added here.
	SetDamageDataNextByChainIndex( pDamageData, 0 );
#else SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
	if ( m_DamageEffectDataInLua.m_pDamageDataNext != NULL )
	{
		// 기존의 DamageData의 값중 DamageDataNext에서도 유지해야 할것들 미리 대입
		m_DamageEffectDataInLua.m_pDamageDataNext->hitUnitList = pDamageData->hitUnitList;
		m_DamageEffectDataInLua.m_pDamageDataNext->m_bApplyExtraDamage = pDamageData->m_bApplyExtraDamage;

#ifdef PVP_SEASON2
		m_DamageEffectDataInLua.m_pDamageDataNext->m_fPvpRate = pDamageData->m_fPvpRate;
#endif

#ifdef EVE_ELECTRA
		m_DamageEffectDataInLua.m_pDamageDataNext->m_iLaserGroupID = pDamageData->m_iLaserGroupID;
		if( m_iLaserGroupID > 0 )
		{
			m_DamageEffectDataInLua.m_pDamageDataNext->damage = pDamageData->damage;
			m_DamageEffectDataInLua.m_pDamageDataNext->fHitAddMP = pDamageData->fHitAddMP;
			m_DamageEffectDataInLua.m_pDamageDataNext->m_fRateModifier = pDamageData->m_fRateModifier;
		}
#endif EVE_ELECTRA

#ifdef ADD_RENA_SYSTEM //김창한
		//스킬, 관련 데이터 값, 첫타인지 체크 하는 값을 상속함
		m_DamageEffectDataInLua.m_pDamageDataNext->m_eFirstAttack = pDamageData->m_eFirstAttack;
		m_DamageEffectDataInLua.m_pDamageDataNext->m_RelateSkillData = pDamageData->m_RelateSkillData;
#endif //ADD_RENA_SYSTEM

		// 다시 DamageData에 넣어줌
		*pDamageData =  *(m_DamageEffectDataInLua.m_pDamageDataNext);
	}	
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
}

#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME
// June2017 FUN_31f799c0 (SetDamageDataNext), Phase 7a. June keeps every NEXT table in one vector
// (+0xd8) and indexes it with the DAMAGE_DATA_CHANGE_TIME counter. Here that vector is
// m_pDamageDataNext (index 0) followed by m_vecDamageDataNextChain (index 1 = NEXT2, ...); Phase 5
// reads the numbered tables only once the unnumbered one exists. A NULL entry does nothing, as in
// June (@31f799d2). The fields carried over are March's own list from SetDamageDataNext above,
// unchanged. June's body has the same shape (hit list, extra-damage flag, PvP rate, the
// laser-group damage/MP copy, then the struct copy), plus an XOR-protected MP check with no March
// counterpart. A field-by-field comparison belongs to Phase 8.
int CX2DamageEffect::CEffect::GetDamageDataNextChainSize() const
{
	if( NULL == m_DamageEffectDataInLua.m_pDamageDataNext )
		return 0;
	return 1 + (int)m_DamageEffectDataInLua.m_vecDamageDataNextChain.size();
}

void CX2DamageEffect::CEffect::SetDamageDataNextByChainIndex( CX2DamageManager::DamageData* pDamageData, int iChainIndex )
{
	CX2DamageManager::DamageData* pDamageDataNext = NULL;
	if( 0 == iChainIndex )
		pDamageDataNext = m_DamageEffectDataInLua.m_pDamageDataNext;
	else if( 0 < iChainIndex && iChainIndex <= (int)m_DamageEffectDataInLua.m_vecDamageDataNextChain.size() )
		pDamageDataNext = m_DamageEffectDataInLua.m_vecDamageDataNextChain[ iChainIndex - 1 ];

	if ( pDamageDataNext != NULL )
	{
		pDamageDataNext->hitUnitList = pDamageData->hitUnitList;
		pDamageDataNext->m_bApplyExtraDamage = pDamageData->m_bApplyExtraDamage;

#ifdef PVP_SEASON2
		pDamageDataNext->m_fPvpRate = pDamageData->m_fPvpRate;
#endif

#ifdef EVE_ELECTRA
		pDamageDataNext->m_iLaserGroupID = pDamageData->m_iLaserGroupID;
		if( m_iLaserGroupID > 0 )
		{
			pDamageDataNext->damage = pDamageData->damage;
			pDamageDataNext->fHitAddMP = pDamageData->fHitAddMP;
			pDamageDataNext->m_fRateModifier = pDamageData->m_fRateModifier;
		}
#endif EVE_ELECTRA

#ifdef ADD_RENA_SYSTEM
		pDamageDataNext->m_eFirstAttack = pDamageData->m_eFirstAttack;
		pDamageDataNext->m_RelateSkillData = pDamageData->m_RelateSkillData;
#endif //ADD_RENA_SYSTEM

		// June2017 FUN_31f799c0 @31f79b54-31f79b75 (Phase 7d): an INHERIT_DAMAGE child keeps the damage
		// it took from its parent across a DAMAGE_DATA_NEXT change, instead of the next table's own.
		if( true == m_bInheritDamageFromMaster )
		{
			pDamageDataNext->damage.fPhysic	= pDamageData->damage.fPhysic;
			pDamageDataNext->damage.fMagic	= pDamageData->damage.fMagic;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxHit( GetName(), L"INHERIT_DAMAGE_NEXT", L"chain=%d physic=%.3f magic=%.3f",
				iChainIndex, pDamageData->damage.fPhysic, pDamageData->damage.fMagic );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}

#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
		// June2017 FUN_31f799c0 @31f79b87/8d (effect completion C6b): the effect-set index stamp (DamageData +0x638) is carried across a NEXT
		// change. June copies neither +0x5e0 nor +0x5e8 there, which matches Phase 9b.
		pDamageDataNext->m_iJuneDamageEffectIndex = pDamageData->m_iJuneDamageEffectIndex;
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
		*pDamageData = *pDamageDataNext;
	}
}
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
//////////////////////////////////////////////////////////////////////////
// June2017 effect completion C6b: the spawner tail (funcmap rows tagged Effect C6a). June keeps every DAMAGE_DATA_NEXT table in one vector (+0xd8)
// and every spawn entry in four lists on the instance (die +0x8, CREATE +0x78, hit +0x98, touch-land +0xa8). March keeps the first NEXT table in
// m_pDamageDataNext (NULL when the effect has none, where June's vector is simply empty) and NEXT2.. in m_vecDamageDataNextChain; the lists are
// m_DamageEffectDataInLua's, parsed per instance, so a change reaches only this effect and what it spawns later.
void CX2DamageEffect::CEffect::JuneGetNextTables( std::vector<CX2DamageManager::DamageData*>& vecTables_ )
{
	if( NULL != m_DamageEffectDataInLua.m_pDamageDataNext )
		vecTables_.push_back( m_DamageEffectDataInLua.m_pDamageDataNext );
	// June's three callees do not null-test the vector's entries (JUNE_BUGS); March's chain entries are never NULL, so none is added.
	vecTables_.insert( vecTables_.end(), m_DamageEffectDataInLua.m_vecDamageDataNextChain.begin(), m_DamageEffectDataInLua.m_vecDamageDataNextChain.end() );
}

void CX2DamageEffect::CEffect::JuneCollectSpawnEntries( std::vector<DamageEffectData*>& vecEntries_ )
{
	// June 31ed7480 x3 builds die + CREATE + hit; the touch-land list (by value in June) is walked beside it.
	vecEntries_.insert( vecEntries_.end(), m_DamageEffectDataInLua.m_vecDieDamageEffect.begin(), m_DamageEffectDataInLua.m_vecDieDamageEffect.end() );
	// June's CREATE list: SetDamageEffect moves March's from m_vecCreateDamageEffect to m_DamageEffectDataList, so only one of the two holds entries.
	vecEntries_.insert( vecEntries_.end(), m_DamageEffectDataInLua.m_vecCreateDamageEffect.begin(), m_DamageEffectDataInLua.m_vecCreateDamageEffect.end() );
	vecEntries_.insert( vecEntries_.end(), m_DamageEffectDataList.begin(), m_DamageEffectDataList.end() );
	vecEntries_.insert( vecEntries_.end(), m_DamageEffectDataInLua.m_vecHitDamageEffect.begin(), m_DamageEffectDataInLua.m_vecHitDamageEffect.end() );
	vecEntries_.insert( vecEntries_.end(), m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.begin(), m_DamageEffectDataInLua.m_vecTouchLandDamageEffect.end() );
}

// June 31f8fdb0 (called by the hit, die, touch-land and CREATE_DAMAGE_EFFECT spawners and RT_CREATE_DAMAGE_EFFECT): this = the parent, the
// argument = the child (NULL returns). The gate flags are set only by class code, so in March only the unconditional m_iIndex, m_bHyper and
// alpha-option copies and the child's own b66 branch act.
void CX2DamageEffect::CEffect::JuneInheritFromParent( CEffect* pChild_ )
{
	if( NULL == pChild_ )
		return;

	// b45: the child's DamageData +0x5fd and its flag
	if( true == m_bInheritCountAsOwnerSkillHit )
	{
		pChild_->m_DamageData.m_bCountAsOwnerSkillHit	= m_DamageData.m_bCountAsOwnerSkillHit;
		pChild_->m_bInheritCountAsOwnerSkillHit			= m_bInheritCountAsOwnerSkillHit;
	}
	// b44: IGNORE_STATE_DEFENCE (DamageData +0x541)
	if( true == m_bInheritIgnoreStateDefence )
	{
		pChild_->m_DamageData.m_bIgnoreStateDefence		= m_DamageData.m_bIgnoreStateDefence;
		pChild_->m_bInheritIgnoreStateDefence			= m_bInheritIgnoreStateDefence;
	}
	// b46: the buff-factor vector INHERIT_BUFF_FACTOR copies (DamageData +0x40), whole (31e9bef0)
	if( true == m_bInheritBuffFactorToChild )
	{
		pChild_->m_DamageData.m_vecBuffFactorPtr		= m_DamageData.m_vecBuffFactorPtr;
		pChild_->m_bInheritBuffFactorToChild			= m_bInheritBuffFactorToChild;
	}
	// b47: IGNORE_DEFENCE (+0x528) and not IGNORE_DEFENCE_ON_PVP (+0x529); 31f65e40 and tag 16 set both. June as is (JUNE_BUGS)
	if( true == m_bInheritIgnoreDefence )
	{
		pChild_->m_DamageData.m_bIgnoreDefence			= m_DamageData.m_bIgnoreDefence;
		pChild_->m_bInheritIgnoreDefence				= m_bInheritIgnoreDefence;
	}
	// b5c: the rate, and the child main effect's max life times it when the rate is above 0
	if( true == m_bInheritMainEffectLifeTimeRate )
	{
		pChild_->m_fMainEffectLifeTimeRate = m_fMainEffectLifeTimeRate;
		CKTDGXMeshPlayer::CXMeshInstance* pChildMainEffect = pChild_->GetMainEffect();
		if( NULL != pChildMainEffect && m_fMainEffectLifeTimeRate > 0.f )
			pChildMainEffect->SetMaxLifeTime( pChildMainEffect->GetMaxLifeTime() * m_fMainEffectLifeTimeRate );
		pChild_->m_bInheritMainEffectLifeTimeRate = m_bInheritMainEffectLifeTimeRate;
	}
	// the child's own b66 (INHERIT_DAMAGE, set only by the CREATE_DAMAGE_EFFECT loop @31f89cfb): the child takes the parent's damage
	// (DamageData +0x38 / +0x3c), the copy March's CREATE loop did inline since Phase 7d
	if( true == pChild_->m_bInheritDamageFromMaster )
	{
		pChild_->m_DamageData.damage.fPhysic			= m_DamageData.damage.fPhysic;
		pChild_->m_DamageData.damage.fMagic				= m_DamageData.damage.fMagic;
	}
	// unconditionally: b64 (m_bHyper), b00 (m_iIndex), b32 (the effect-alpha option)
	pChild_->m_bHyper									= m_bHyper;
#ifdef SERV_RENA_NIGHT_WATCHER
	pChild_->m_iIndex									= m_iIndex;
#endif SERV_RENA_NIGHT_WATCHER
	pChild_->m_iApplyEffectAlphaConditionByOption		= m_iApplyEffectAlphaConditionByOption;

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
#ifdef SERV_RENA_NIGHT_WATCHER
	const int iJuneChildIndex = pChild_->m_iIndex;	// the child's m_iIndex after the copy (June +0xb00): Rena's CheckDamageEffectType looks effects up by it
#else SERV_RENA_NIGHT_WATCHER
	const int iJuneChildIndex = -1;
#endif SERV_RENA_NIGHT_WATCHER
	JuneFxHit( GetName(), L"SPAWN_INHERIT", L"child=%s gates ignoreState=%d ownerSkillHit=%d buffFactor=%d ignoreDefence=%d lifeRate=%d(%.3f) trigger=%d hyper=%d childInheritDamage=%d index=%d",
		pChild_->GetName(), (int)m_bInheritIgnoreStateDefence, (int)m_bInheritCountAsOwnerSkillHit, (int)m_bInheritBuffFactorToChild,
		(int)m_bInheritIgnoreDefence, (int)m_bInheritMainEffectLifeTimeRate, m_fMainEffectLifeTimeRate, (int)m_bInheritAttackTriggerType,
		(int)m_bHyper, (int)pChild_->m_bInheritDamageFromMaster, iJuneChildIndex );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}

// June 31f649d0: this = the child, called with the spawning entry's +0xb0. Returns unless the rate differs from 1.0 by more than FLT_EPSILON
// (@31f649e0; the constants 334355b0 / 334490f8). Entry +0xb0 starts at 1.0 and only this function and class code change it, so in March
// every call is a no-op.
void CX2DamageEffect::CEffect::JuneApplyInheritDamageRate( float fRate_ )
{
	if( fabs( (double)fRate_ - 1.0 ) <= (double)FLT_EPSILON )
		return;

	m_DamageData.damage.fPhysic	*= fRate_;
	m_DamageData.damage.fMagic	*= fRate_;

	std::vector<CX2DamageManager::DamageData*> vecTables;
	JuneGetNextTables( vecTables );
	for( size_t i = 0; i < vecTables.size(); i++ )
	{
		vecTables[i]->damage.fPhysic	*= fRate_;
		vecTables[i]->damage.fMagic		*= fRate_;
	}

	// only for a rate >= 0 (fldz; fcom): the entries' own rates follow, so a grandchild spawned later inherits it
	if( fRate_ >= 0.f )
	{
		std::vector<DamageEffectData*> vecEntries;
		JuneCollectSpawnEntries( vecEntries );
		for( size_t i = 0; i < vecEntries.size(); i++ )
		{
			if( NULL != vecEntries[i] )
				vecEntries[i]->fInheritDamageRate *= fRate_;
		}
	}
}

// June 31f64c00: this = the parent, the argument = the child. The parent's ADDITIONAL_ATTACK_TRIGGER_TYPE values 0x1d and 0x1e go onto the
// child's list (June Enum.lua DTT_RCR_TRANSCENDENCE_REVOLVER / _NOT_REVOLVER, Rose; March's DAMAGE_TRIGGER_TYPE ends at 6, so literals). Then,
// under b65, the child takes the parent's ATTACK_TRIGGER_TYPE and the whole list (32853ff0 assign, replacing the pushes).
void CX2DamageEffect::CEffect::JuneCopyAttackTriggerToChild( CEffect* pChild_ )
{
	if( NULL == pChild_ )
		return;

	for( std::vector<int>::const_iterator it = m_DamageData.m_vecAdditionalAttackTriggerType.begin(); it != m_DamageData.m_vecAdditionalAttackTriggerType.end(); ++it )
	{
		if( 29 == *it || 30 == *it )
			pChild_->m_DamageData.m_vecAdditionalAttackTriggerType.push_back( *it );
	}

	if( true == m_bInheritAttackTriggerType )
	{
		pChild_->m_bInheritAttackTriggerType				= true;
		pChild_->m_DamageData.m_eDamageTrigger				= m_DamageData.m_eDamageTrigger;
		pChild_->m_DamageData.m_vecAdditionalAttackTriggerType	= m_DamageData.m_vecAdditionalAttackTriggerType;
	}
}

// June 31f65e40: this = the child, called with the parent's IGNORE_DEFENCE when the spawning entry's +0xb4 is set. Own IGNORE_DEFENCE and
// IGNORE_DEFENCE_ON_PVP, every NEXT table's, and +0xb4 = 1 on every spawn entry, true even when the value is false, so a descendant takes
// the parent's value over its own Lua one (JUNE_BUGS). The NEXT tables are not null-tested in June (JUNE_BUGS).
void CX2DamageEffect::CEffect::JuneSetIgnoreDefenceDown( bool bValue_ )
{
	m_DamageData.m_bIgnoreDefence		= bValue_;
	m_DamageData.m_bIgnoreDefenceOnPvp	= bValue_;

	std::vector<CX2DamageManager::DamageData*> vecTables;
	JuneGetNextTables( vecTables );
	for( size_t i = 0; i < vecTables.size(); i++ )
	{
		vecTables[i]->m_bIgnoreDefence		= bValue_;
		vecTables[i]->m_bIgnoreDefenceOnPvp	= bValue_;
	}

	std::vector<DamageEffectData*> vecEntries;
	JuneCollectSpawnEntries( vecEntries );
	for( size_t i = 0; i < vecEntries.size(); i++ )
	{
		if( NULL != vecEntries[i] )
			vecEntries[i]->bInheritIgnoreDefence = true;
	}
}

// June 31f65f90: as 31f65e40 for IGNORE_STATE_DEFENCE (DamageData +0x541) and the entries' +0xb5.
void CX2DamageEffect::CEffect::JuneSetIgnoreStateDefenceDown( bool bValue_ )
{
	m_DamageData.m_bIgnoreStateDefence = bValue_;

	std::vector<CX2DamageManager::DamageData*> vecTables;
	JuneGetNextTables( vecTables );
	for( size_t i = 0; i < vecTables.size(); i++ )
		vecTables[i]->m_bIgnoreStateDefence = bValue_;

	std::vector<DamageEffectData*> vecEntries;
	JuneCollectSpawnEntries( vecEntries );
	for( size_t i = 0; i < vecEntries.size(); i++ )
	{
		if( NULL != vecEntries[i] )
			vecEntries[i]->bInheritIgnoreStateDefence = true;
	}
}

// The hit and die spawners' tail in June's order (31f810c0 @31f81afd-31f81b35, 31f82210 @31f83137-31f8316f): this = the parent,
// the argument = the child just created, pData = the spawning entry.
void CX2DamageEffect::CEffect::JuneSpawnTail( CEffect* pChild_, const DamageEffectData* pData_ )
{
	if( NULL == pChild_ || NULL == pData_ )
		return;

	pChild_->JuneApplyInheritDamageRate( pData_->fInheritDamageRate );
	JuneCopyAttackTriggerToChild( pChild_ );
	if( true == pData_->bInheritIgnoreDefence )
		pChild_->JuneSetIgnoreDefenceDown( m_DamageData.m_bIgnoreDefence );
	if( true == pData_->bInheritIgnoreStateDefence )
		pChild_->JuneSetIgnoreStateDefenceDown( m_DamageData.m_bIgnoreStateDefence );

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxHit( GetName(), L"SPAWN_TAIL", L"child=%s rate=%.3f entryIgnoreDefence=%d entryIgnoreStateDefence=%d parentTrigger=%d",
		pChild_->GetName(), pData_->fInheritDamageRate, (int)pData_->bInheritIgnoreDefence, (int)pData_->bInheritIgnoreStateDefence,
		(int)m_bInheritAttackTriggerType );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}

// June 31f6ea20 (effect completion C7, funcmap rows 31f6ea20 / CEffect+0xb18): this = the parent, the argument = the child made by
// RT_CREATE_DAMAGE_EFFECT (only caller, AttackResult @31f86148). Nothing happens unless the child exists and the rate (June +0xb18)
// is above 0; only class code sets the rate (checklist 7a), so in March it stays -1 and this returns.
// The rate test is June's polarity (@31f6ea3a-31f6ea49: fcom against 0, test ah,0x41, jnp return): it returns on <= 0 only, so a
// NaN rate goes on (x87-emulated in the C7 review); "<= 0.f" keeps that under /fp:precise, "!( > 0.f )" would not.
void CX2DamageEffect::CEffect::JuneApplyCreateDamageEffectRate( CEffect* pChild_ )
{
	if( NULL == pChild_ || m_fJuneCreateDamageEffectRate <= 0.f )
		return;

	CX2DamageManager::DamageData& kChildData = pChild_->GetDamageData();
	switch( kChildData.damageType )		// DamageData +0x0: DT_PHYSIC scales PHYSIC, DT_MAGIC scales MAGIC, any other type neither
	{
	case CX2DamageManager::DT_PHYSIC:
		kChildData.damage.fPhysic *= m_fJuneCreateDamageEffectRate;
		break;

	case CX2DamageManager::DT_MAGIC:
		kChildData.damage.fMagic *= m_fJuneCreateDamageEffectRate;
		break;

	default:
		break;
	}
	// HIT_ADD_MP (DamageData +0x3f8; a protected float in June, whose re-store and registration 323101d0 March has no counterpart for)
	kChildData.fHitAddMP *= m_fJuneCreateDamageEffectRate;
	// TECH_POINT (DamageData +0x34, int) times the rate truncated to an int (ftol 32fe25f0), so a rate below 1 zeroes it: June as is (JUNE_BUGS)
	kChildData.techPoint *= static_cast<int>( m_fJuneCreateDamageEffectRate );

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxHit( GetName(), L"CREATE_DAMAGE_EFFECT_RATE", L"child=%s rate=%.3f type=%d physic=%.3f magic=%.3f hitAddMP=%.3f techPoint=%d",
		pChild_->GetName(), m_fJuneCreateDamageEffectRate, (int)kChildData.damageType, kChildData.damage.fPhysic, kChildData.damage.fMagic,
		kChildData.fHitAddMP, kChildData.techPoint );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT

#ifdef SERV_IRUHADEV_JUNE_TOUCH_LAND
//////////////////////////////////////////////////////////////////////////
// June2017 effect completion C7 (old Phase 10c step 3): TOUCH_LAND_DAMAGE_EFFECT%d's runtime.
// June 31f83380, called from OnFrameMove @31f8a38b when the gate holds: each entry of the instance's touch-land list (+0xa8, by value)
// is copied into a scratch list (+0xa38, 31f8e750), the spawner 31f81ba0 spawns from the copy, and the copy is freed (31f8c660 /
// 31f8b5f0). The spawner and its tail only read the entries, so March spawns from the instance's own list directly.
void CX2DamageEffect::CEffect::JuneTouchLandDamageEffect()
{
	CreateTouchLandDamageEffect( m_DamageEffectDataInLua.m_vecTouchLandDamageEffect );
}

// June 31f81ba0 (read in full: june2017_re/ghidra/dis_31f81ba0.txt; funcmap row 31f81ba0). The shape of CreateHitDamageEffect, with
// these differences: no POSITION_TYPE switch (the position is always m_BeforePos), the rotation is m_AngleDegree or zero, m_fLandPos
// goes to CreateInstance as is, and the post-create steps run in June's order.
void CX2DamageEffect::CEffect::CreateTouchLandDamageEffect( vector<DamageEffectData*>& vecTouchLandDamageEffect )
{
	KTDXPROFILE();

	// @31f81bae-31f81ca2: the guard chain CreateHitDamageEffect has (g_pX2Game, a live owner, a live main mesh)
	if( NULL == g_pX2Game ||
		null == m_optrOwnerUnit ||
		false == g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstHandle ) )
	{
		return;
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstHandle );
	if( NULL == pMeshInst )
		return;

	for( UINT i = 0; i < vecTouchLandDamageEffect.size(); i++ )
	{
		DamageEffectData* pData = vecTouchLandDamageEffect[i];

		D3DXVECTOR3 vAxisAngleDegree( 0, 0, 0 );
		D3DXVECTOR3 vRotateAngleDegree( 0, 0, 0 );

		// @31f81cd4-31f81d24: the invoke-rate skip, the same test as CreateHitDamageEffect (funcmap 31f810c0 @31f811ed-31f81259)
#ifdef SERV_RENA_NIGHT_WATCHER
		if( DET_ARROW_OF_EXPLOSION != GetType() )
#endif SERV_RENA_NIGHT_WATCHER
		{
			if( GetOwnerUnit() != NULL )
			{
				if( GetOwnerUnit()->GetRandomFloat() > GetDamageData().m_fHitDamageEffectInvokeRate )
					continue;
			}
		}

		// @31f81d2c-31f81d5d: APPLY_ROTATE_ORIGIN (entry +0x74) takes m_AngleDegree for both angles, else both stay zero.
		// The entry's APPLY_ROTATE, ROTATE_X/Y/Z, POSITION_TYPE and BONE_NAME are parsed and never read here.
		if( true == pData->bApplyRotateOrigin )
		{
			vAxisAngleDegree = m_AngleDegree;
			vRotateAngleDegree = vAxisAngleDegree;
		}

		// @31f81d61-31f81d80: the position is always m_BeforePos
		D3DXVECTOR3 vPosition = m_BeforePos;

		// @31f81d84-31f81f03: APPLY_ROTATED_OFFSET (entry +0x75) as CreateHitDamageEffect's block (GetDirVec 31c82e40), else the plain offset
		if( true == pData->bApplyRotatedOffset )
		{
			D3DXVECTOR3 vDirVector = pMeshInst->GetDirVec();

			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}
			if( 0.f != pData->vOffset.x )
			{
				vPosition += vDirVector * pData->vOffset.x;
			}
			if( 0.f != pData->vOffset.y )
			{
				vPosition.y += pData->vOffset.y;
			}
			if( 0.f != pData->vOffset.z )
			{
				if( vDirVector.y != 0.f )
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}
				else
				{
					vDirVector.y = vDirVector.z;
					vDirVector.z = vDirVector.x;
					vDirVector.x = -vDirVector.y;
					vDirVector.y = 0.f;
				}
				vPosition += vDirVector * pData->vOffset.z;
			}
		}
		else
		{
			vPosition += pData->vOffset;
		}

		if( m_pManager != NULL )
		{
			// @31f81f1a-31f81f35: INHERIT_FORCEDOWNRATE's rate (+0xacd ? +0xad0 : 1.0); @31f81f5c-31f81feb: CreateInstance with
			// m_fLandPos as is (no min with the position) and the rest at March's defaults
			float fForceDownRate = 1.f;
			if( true == m_bInheritForceDownRate )
				fForceDownRate = m_fForceDownRate;
			CEffect* pEffect = m_pManager->CreateInstance( m_optrOwnerUnit.GetObservable(), pData->damageEffectName.c_str(), m_fPowerRate,
															vPosition, vRotateAngleDegree, vAxisAngleDegree,
															m_fLandPos, false, -1.f, fForceDownRate );

			if( pEffect != NULL )
			{
				// @31f81ffd: 31f8af50, INHERIT_CRITICAL_RATE / INHERIT_CRITICAL_DAMAGE_RATE
				if( true == m_bInheritCriticalRate )
					pEffect->GetDamageData().m_fCriticalRate += m_DamageData.m_fCriticalRate;
				if( true == m_bInheritCriticalDamageRate )
					pEffect->GetDamageData().m_fApplyCriticalDamage += m_DamageData.m_fApplyCriticalDamage;

				// @31f82005: 31f8fdb0, the parent's state
				JuneInheritFromParent( pEffect );

				// @31f8200a-31f8202f: INHERIT_BUFF_FACTOR (entry +0x80), only when the parent has buff factors
				if ( pData->bInheritBuffFactor && !GetDamageData().m_vecBuffFactorPtr.empty() )
					pEffect->GetDamageData().m_vecBuffFactorPtr = GetDamageData().m_vecBuffFactorPtr;

				// @31f82034-31f82070: INHERIT_LIFE_TIME (entry +0xad): the child main effect continues the parent's life clock
				if( true == pData->bInheritLifeTime )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pParentMainEffect = GetMainEffect();
					CKTDGXMeshPlayer::CXMeshInstance* pChildMainEffect = pEffect->GetMainEffect();
					if( NULL != pParentMainEffect && NULL != pChildMainEffect )
						pChildMainEffect->SetNowLifeTime( pParentMainEffect->GetNowLifeTime() );
				}

				// @31f82076-31f820ad: the hit damage-effect damage rate (DamageData +0x524) when above 0
				if( 0.f < GetDamageData().m_fHitDamageEffectDamageRate )
				{
					pEffect->GetDamageData().damage.fPhysic *= GetDamageData().m_fHitDamageEffectDamageRate;
					pEffect->GetDamageData().damage.fMagic	*= GetDamageData().m_fHitDamageEffectDamageRate;
				}

				// @31f820ad-31f8210b: SetScale( entry SCALE_X/Y/Z times the parent's hit damage-effect scale +0xa5c )
				pEffect->SetScale( D3DXVECTOR3( pData->vScale.x * m_vHitDamageScale.x, pData->vScale.y * m_vHitDamageScale.y, pData->vScale.z * m_vHitDamageScale.z ) );
#ifdef GIANT_UNIT_GIANT_EFFECT_TEST
				// @31f82110-31f82119: SetScaleByUnit( +0xaa0 ), the same member the hit spawner passes
				pEffect->SetScaleByUnit( m_vHitDamageScaleByUnit );
#endif GIANT_UNIT_GIANT_EFFECT_TEST

#ifdef INHERIT_FORCEDOWNRATE
				// @31f8211e-31f82124
				pEffect->SetInheritForceDownRate( m_bInheritForceDownRate );
#endif INHERIT_FORCEDOWNRATE

#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE
				// @31f8212a-31f8214e: max life (mesh +0x44c, a plain store as March's SetMaxLifeTime) from the entry's life time when above 0
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInstTouchLandEffect = pEffect->GetMainEffect();
				if( NULL != pMeshInstTouchLandEffect && pData->fLifeTime > 0.f )
					pMeshInstTouchLandEffect->SetMaxLifeTime( pData->fLifeTime );
#endif SERV_ELESIS_SECOND_CLASS_CHANGE

#ifdef ADD_RENA_SYSTEM
				// @31f82154-31f82167: the first-attack copy
				pEffect->GetDamageData().m_eFirstAttack = m_DamageData.m_eFirstAttack;
				pEffect->GetDamageData().m_RelateSkillData = m_DamageData.m_RelateSkillData;
#endif ADD_RENA_SYSTEM

				// @31f8216e-31f821b5: the spawner tail (31f649d0, 31f64c00, 31f65e40 / 31f65f90)
				JuneSpawnTail( pEffect, pData );

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				JuneFxHit( GetName(), L"TOUCH_LAND", L"%s pos=(%.1f,%.1f,%.1f) rotateOrigin=%d rotatedOffset=%d scale=(%.3f,%.3f,%.3f) forceDown=%.3f",
					pData->damageEffectName.c_str(), vPosition.x, vPosition.y, vPosition.z, (int)pData->bApplyRotateOrigin,
					(int)pData->bApplyRotatedOffset, pData->vScale.x, pData->vScale.y, pData->vScale.z, fForceDownRate );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			}
		}
	}
}
#endif SERV_IRUHADEV_JUNE_TOUCH_LAND
#ifdef SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET
// June2017 FUN_31f85910 @31f861e2-31f86521 (effect completion C6b; funcmap rows Effect C5 review): RT_CREATE_EFFECT_SET with a trace unit looks for the
// field boss's collision box. Gate, all of which must hold:
//   1. 32481fa0: June state 0x15 (XS_BATTLE_FIELD) and the current field (GetBattleFieldIdWhereIam, [g_pData +0x124 +0x10]) passes 3266cdc0, a switch on
//      exactly 40033, 40034, 40039-40044 (June VMI_BOSS_RAID_PERKISAS* / VMI_BOSS_RAID_ELTRION_MK2_*, which March's enum lacks);
//   2. the unit held by the raid object at g_pMain +0x115c (+0x8, 3266cb90, an observer) exists and its UID equals the trace unit's.
// Then the nearest NPC with ID 0x621 (NUI_FIELD_BOSS_COLLISION_BOX_1) to the impact point wins: distance 31c7b560( impactPoint, GetPos( 0 ) ), strict <,
// from 99999 (33441c60), no alive check. NULL (no gate, no unit, no box) leaves March's own call. March has no raid object, so part 2 is a stub that
// returns NULL and says so once in the log (plan rule 5); part 1 cannot hold in March either, so the search below is unreachable today.
CX2GameUnit* CX2DamageEffect::CEffect::JuneFindFieldBossCollisionBox( CX2GameUnit* pTraceUnit_ )
{
	if( NULL == pTraceUnit_ || NULL == g_pX2Game || NULL == g_pMain || NULL == g_pData )
		return NULL;

	if( CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() )
		return NULL;

	switch( g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam() )
	{
	case 40033: case 40034: case 40039: case 40040: case 40041: case 40042: case 40043: case 40044:
		break;
	default:
		return NULL;
	}

	// part 2: June's raid object (g_pMain +0x115c) does not exist here, so there is no boss unit to compare with the trace unit's UID
	CX2GameUnit* pJuneRaidBossUnit = NULL;
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	static bool s_bJuneRaidObjectLogged = false;
	if( false == s_bJuneRaidObjectLogged )
	{
		s_bJuneRaidObjectLogged = true;
		JuneFxHit( GetName(), L"CREATE_EFFECT_SET", L"raid object (g_pMain +0x115c) absent in March: boss unit stubbed to NULL, field=%u", g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam() );
	}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	if( NULL == pJuneRaidBossUnit || pJuneRaidBossUnit->GetUnitUID() != pTraceUnit_->GetUnitUID() )
		return NULL;

	CX2GameUnit* pNearestBox = NULL;
	float fNearestDistance = 99999.f;
	for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( NULL == pUnit || CX2GameUnit::GUT_NPC != pUnit->GetGameUnitType() )
			continue;
		if( false == static_cast<CX2GUNPC*>( pUnit )->IsUnitID( CX2UnitManager::NUI_FIELD_BOSS_COLLISION_BOX_1 ) )
			continue;

		const D3DXVECTOR3 vDiff = pUnit->GetPos() - m_DamageData.impactPoint;
		const float fDistance = D3DXVec3Length( &vDiff );
		if( fDistance < fNearestDistance )
		{
			fNearestDistance = fDistance;
			pNearestBox = pUnit;
		}
	}
	return pNearestBox;
}
#endif SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET
#endif DAMAGE_EFFECT_WORK
//}} kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(독구름)

#ifdef EVE_ELECTRA
void CX2DamageEffect::CEffect::SetLaserGroupID( int _iLaserGroupID )
{
	m_iLaserGroupID					= _iLaserGroupID;
	m_DamageData.m_iLaserGroupID	= _iLaserGroupID;
}
#endif EVE_ELECTRA


void CX2DamageEffect::ParticleData::DestroyParticleSequence()
{
	if( m_hSeq != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = ( g_pX2Game != NULL ) ? g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeq ) : NULL;
		if( NULL != pSeq )
		{
			if( bForceDie == true )
			{
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeq );
			}
			else
			{
				pSeq->SetAutoDie();
			}
		}

		m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

#ifdef FIX_AFTER_IMAGE
void CX2DamageEffect::CEffect::LoadAfterImage()
{
	if( m_pLuaManager->BeginTable( "AFTER_IMAGE" ) == true )
	{
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_STARTTIME",	m_afterImageTime.x,	-1.0f );
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_ENDTIME",	m_afterImageTime.y,	-1.0f );
		X2_LAYER eAfterImageLayer = XL_EFFECT_0;
		PLUA_GET_VALUE_ENUM( m_pLuaManager, "AFTER_IMAGE_LAYER", eAfterImageLayer, X2_LAYER, XL_EFFECT_0 );
		D3DXCOLOR coAfterImage = 0xffcccccc;
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_COLOR_R", coAfterImage.r, 0.797f );
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_COLOR_G", coAfterImage.g, 0.797f );
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_COLOR_B", coAfterImage.b, 0.797f );
		PLUA_GET_VALUE( m_pLuaManager, "AFTER_IMAGE_COLOR_A", coAfterImage.a, 1.f );

		if ( m_pAfterImage == NULL )
		{
			if( NULL != GetMainEffect() && NULL != GetMainEffect()->GetXSkinAnim() )
			{
				m_pAfterImage = CKTDGXSkinAfterImage::CreateSkinAfterImage( GetMainEffect()->GetXSkinAnim(), 10, eAfterImageLayer );

				if ( m_pAfterImage != NULL )
				{
					m_pAfterImage->SetAfterImageColor( coAfterImage );
					m_pAfterImage->AddMesh( GetMainEffect()->GetXSkinMesh() );
				}
			}
		}

		m_pLuaManager->EndTable();
	}
	else
	{
		m_afterImageTime.x	= -1.0f;
		m_afterImageTime.y	= -1.0f;
	}
}
#endif

#ifdef SERV_RENA_NIGHT_WATCHER
void CX2DamageEffect::CEffect::AddDieEffectDamage( CX2DamageEffect::CEffect* pCEffect )
{
	if( 0.f < GetAddDieEffectDamage() )		/// 추가 데미지 값이 설정되어 있을 때, 대입
	{
		if( 0.f < pCEffect->GetDamageData().damage.fPhysic )
			pCEffect->GetDamageData().damage.fPhysic +=  GetAddDieEffectDamage();
		if( 0.f < pCEffect->GetDamageData().damage.fMagic )
			pCEffect->GetDamageData().damage.fMagic +=  GetAddDieEffectDamage();
	}
}
#endif SERV_RENA_NIGHT_WATCHER

#ifdef SERV_RENA_NIGHT_WATCHER
CX2DamageEffect::CEffect* CX2DamageEffect::GetInstanceByIndex( int iIndex )
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end(LIST_LIVE);
    for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        ++iter )
    {
        CX2DamageEffect::CEffect* pEffect = iter->m_pInstance;
        if ( pEffect != NULL && 
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX
            pEffect->GetLive() == true &&
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_INFINITE_LOOP_BUG_FIX            
            pEffect->GetIndex() == iIndex )
            return pEffect;
    }

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	vector<CEffect*>::iterator vIt = m_InstanceList.begin();

	while( vIt != m_InstanceList.end() )			//땅에 박힌 화살의 Index를 지닌 DamageEffedt를 현재 생성된 DamageEffect Vector에서 검색
	{
		CX2DamageEffect::CEffect* pEffect = *vIt;

		if ( iIndex == pEffect->GetIndex() )
		{
			return pEffect;
		}
		else
		{
			++vIt;
		}
	}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	return NULL;
}
#endif SERV_RENA_NIGHT_WATCHER

#ifdef BOOST_SINGLETON_POOL_TEST
	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CX2DamageEffect::ParticleData)> ParticleDataPool;
	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CX2DamageEffect::DamageEffectData)> DamageEffectDataPool;
	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CX2DamageEffect::CEffect)> CEffectPool;



	void* CX2DamageEffect::ParticleData::operator new(size_t s)
	{
		return ParticleDataPool::malloc();
	}

	void CX2DamageEffect::ParticleData::operator delete(void *p)
	{
		ParticleDataPool::free(p);
	}

	void* CX2DamageEffect::DamageEffectData::operator new(size_t s)
	{
		return DamageEffectDataPool::malloc();
	}

	void CX2DamageEffect::DamageEffectData::operator delete(void *p)
	{
		DamageEffectDataPool::free(p);
	}


	void* CX2DamageEffect::CEffect::operator new(size_t s)
	{
		return CEffectPool::malloc();
	}

	void CX2DamageEffect::CEffect::operator delete(void *p)
	{
		CEffectPool::free(p);
	}
#endif BOOST_SINGLETON_POOL_TEST

#ifdef EFFECT_TOOL
bool CX2DamageEffect::IsValidDamageEffectByName( const WCHAR* pName )
{
	if( true == m_LuaManager.BeginTable( pName ) )
	{
		m_LuaManager.EndTable();
		return true;
	}

	return false;
}
wstring CX2DamageEffect::GetMainMeshNameByDamageEffectName( const WCHAR* pName )
{
	wstring mainEffName = L"";

	if( true == m_LuaManager.BeginTable( pName ) )
	{
		LUA_GET_VALUE( m_LuaManager, "MAIN_EFFECT_NAME", mainEffName, L"" );
		m_LuaManager.EndTable();
	}

	return mainEffName;
}
// 데미지 이펙트 내부에서 출력하는 파티클 리스트 얻기
void CX2DamageEffect::GetParticleListByEffectName( IN const WCHAR* pName_, OUT vector<wstring>& vecParticleList_ )
{
	vecParticleList_.clear();

	wstring wstrParticleName = L"";
	if( true == m_LuaManager.BeginTable( pName_ ) )
	{
		// START_PARTICLE
		int index = 0;
		while( m_LuaManager.BeginTable( "START_PARTICLE", index ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, "PARTICLE_NAME",	wstrParticleName,	L""		);

			++index;
			m_LuaManager.EndTable();
			vecParticleList_.push_back( wstrParticleName );
		}

		// PASSIVE_PARTICLE
		index = 0;
		while( m_LuaManager.BeginTable( "PASSIVE_PARTICLE", index ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, "PARTICLE_NAME",	wstrParticleName,	L""		);

			++index;
			m_LuaManager.EndTable();
			vecParticleList_.push_back( wstrParticleName );
		}

		// TIME_PARTICLE
		index = 0;
		while( m_LuaManager.BeginTable( "TIME_PARTICLE", index ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, "PARTICLE_NAME",	wstrParticleName,	L""		);

			++index;
			m_LuaManager.EndTable();
			vecParticleList_.push_back( wstrParticleName );
		}
		m_LuaManager.EndTable();
	}
}
/** @function : GetParticleListByEffectName
	@brief : 데미지 이펙트에서 사용하는 파티클 얻기.
*/
void CX2DamageEffect::GetParticleListByEffectName( IN const WCHAR* pName_, OUT vector<ParticleData*>& vecParticleList_ )
{
 	BOOST_FOREACH( ParticleData* pParticleData, vecParticleList_)
 	{
 		SAFE_DELETE(pParticleData);
 	}
	vecParticleList_.clear();

	wstring wstrParticleName = L"";
	if( true == m_LuaManager.BeginTable( pName_ ) )
	{
		int index = 0;
		while( m_LuaManager.BeginTable( "START_PARTICLE", index ) == true )
		{
			ParticleData* pData = LoadParticleTable();
			if( pData != NULL )
				vecParticleList_.push_back( pData );
			index++;
			m_LuaManager.EndTable();
		}

		index = 0;
		while( m_LuaManager.BeginTable( "PASSIVE_PARTICLE", index ) == true )
		{
			ParticleData* pData = LoadParticleTable();	
			if( pData != NULL )
				vecParticleList_.push_back( pData );
			index++;
			m_LuaManager.EndTable();
		}

		index = 0;
		while( m_LuaManager.BeginTable( "TIME_PARTICLE", index ) == true )
		{
			ParticleData* pData = LoadParticleTable();	
			if( pData != NULL )
				vecParticleList_.push_back( pData );
			index++;
			m_LuaManager.EndTable();
		}

		m_LuaManager.EndTable();
	}
}
// 데미지 이펙트 내부에서 생성하는 데미지 이팩트 리스트 얻기
void CX2DamageEffect::GetDamageEffectListByEffectName( IN const WCHAR* pName_,  OUT vector<wstring>& vecEffectList_ )
{
	vecEffectList_.clear();

	wstring wstrEffectName = L"";
	if( true == m_LuaManager.BeginTable( pName_ ) )
	{
		// CREATE_DAMAGE_EFFECT
		int index = 0;
		while( m_LuaManager.BeginTable( "CREATE_DAMAGE_EFFECT", index ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, "DAMAGE_EFFECT_NAME",	wstrEffectName,	L""		);

			++index;
			m_LuaManager.EndTable();
			vecEffectList_.push_back( wstrEffectName );
		}

		// DIE_DAMAGE_EFFECT0
		index = 0;
		while( m_LuaManager.BeginTable( "DIE_DAMAGE_EFFECT", index ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, "DAMAGE_EFFECT_NAME",	wstrEffectName,	L""		);

			++index;
			m_LuaManager.EndTable();
			vecEffectList_.push_back( wstrEffectName );
		}

		m_LuaManager.EndTable();
	}
}
CX2DamageEffect::ParticleData* CX2DamageEffect::LoadParticleTable()
{
	ParticleData* pData = new ParticleData;

	LUA_GET_VALUE_ENUM(	m_LuaManager, "POSITION_TYPE",	pData->posType,			CX2DamageEffect::POSITION_TYPE, DEPT_BONE );
	LUA_GET_VALUE(			m_LuaManager, "PARTICLE_NAME",	pData->particleName,	L""		);
	LUA_GET_VALUE(			m_LuaManager, "APPLY_ROTATE",	pData->bApplyRotate,	true	);
	LUA_GET_VALUE(			m_LuaManager, "FORCE_DIE",		pData->bForceDie,		false	);
	LUA_GET_VALUE(			m_LuaManager, "FORCE_DIE_HIT",	pData->bForceDieHit,	false	);
	LUA_GET_VALUE(			m_LuaManager, "TRIGGER_COUNT",	pData->triggerCount,	-1		);
	LUA_GET_VALUE(			m_LuaManager, "TRIGGER_TIME",	pData->triggerTime,		-1.0f	);
	LUA_GET_VALUE(			m_LuaManager, "BONE_NAME",		pData->boneName,		L""		);
	LUA_GET_VALUE(			m_LuaManager, "EVENT_TIME",	pData->fEventTime,		-1.0f	);
	LUA_GET_VALUE(			m_LuaManager, "OFFSET_X",		pData->fOffsetX,		0.0f	);  // added by wonpok
	LUA_GET_VALUE(			m_LuaManager, "OFFSET_Y",		pData->fOffsetY,		0.0f	);
	LUA_GET_VALUE(			m_LuaManager, "OFFSET_Z",		pData->fOffsetZ,		0.0f	);
	LUA_GET_VALUE(         m_LuaManager, "APPLY_ROTATED_OFFSET", pData->bApplyRotatedOffset  , false ); // added by lucidash : 2010.11.24

	LUA_GET_VALUE(			m_LuaManager, "ROTATE_X",		pData->vRotateAngleDegree.x,	0.0f	);
	LUA_GET_VALUE(			m_LuaManager, "ROTATE_Y",		pData->vRotateAngleDegree.y,	0.0f	);
	LUA_GET_VALUE(			m_LuaManager, "ROTATE_Z",		pData->vRotateAngleDegree.z,	0.0f	);
	LUA_GET_VALUE(			m_LuaManager, "USE_GLOBAL_TIME",	pData->bUseGlobalTime,	false	);
	LUA_GET_VALUE(			m_LuaManager, "MOVE_ANGLE_X_PER_FRAME", pData->vMoveAnglePerFrame.x, 0.0f );
	LUA_GET_VALUE(			m_LuaManager, "MOVE_ANGLE_Y_PER_FRAME", pData->vMoveAnglePerFrame.y, 0.0f );
	LUA_GET_VALUE(			m_LuaManager, "MOVE_ANGLE_Z_PER_FRAME", pData->vMoveAnglePerFrame.z, 0.0f );
	LUA_GET_VALUE(			m_LuaManager, "SCALE_X",		pData->vScale.x,		1.0f	);
	LUA_GET_VALUE(			m_LuaManager, "SCALE_Y",		pData->vScale.y,		1.0f	);
	LUA_GET_VALUE(			m_LuaManager, "SCALE_Z",		pData->vScale.z,		1.0f	);

	LUA_GET_VALUE(			m_LuaManager, "TRACE",			pData->bTrace,			true	);
	LUA_GET_VALUE(			m_LuaManager, "TRACE_MORE",	pData->bTraceMore,		false	);
	LUA_GET_VALUE(			m_LuaManager, "TRACE_DEFENDER_UNIT",	pData->bTraceDefenderUnit,		false	);
	LUA_GET_VALUE(			m_LuaManager, "SCALE_BY_MAINEFFECT_SCALE_Y",		pData->bScaleByMainEffectScaleY,		false	);
	LUA_GET_VALUE(			m_LuaManager, "SHOW_INNER_LANDHEIGHT",		pData->fShowLandHeight,		99999.f	);

	return pData;

}
#endif //EFFECT_TOOL



#ifdef FIELD_BOSS_RAID // 지정한 위치에서 가까운 적 유도 시키기
void CX2DamageEffect::CEffect::SetLockOnNearstTarget( IN const D3DXVECTOR3& vMyPos_, int iTeam_ , int randomOffset/* = 0 */)
{
	bool bUserUnit;
	UidType uid = ( g_pX2Game != NULL ) ? g_pX2Game->GetLockOnNearstTarget( static_cast<CX2Room::TEAM_NUM>(iTeam_), vMyPos_, bUserUnit ) : 0;

	if( bUserUnit == true )
		SetLockOnUnitUID( uid );
	else
		SetLockOnNPCUID( (int)uid );

	SetLockOnRandomSeed( randomOffset );
}
#endif // FIELD_BOSS_RAID

#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
/** @function	: SetLinkMainEffectByPos
	@brief		: 해당 데미지 이펙트의 Main Mesh를 두 위치 사이에 연결 시켜 주는 함수
	@param		: 이펙트 위치, 상대 위치, 길이 배율 ( Main Effect 길이에 따른 스케일 설정 배율 ), 두 이펙트 간 거리
				  ( 참고로, DamageEffect_APT_Pylon_Additional_Shock 의 길이 배율은 800.f 이다. )
	@return		: 연결 성공 여부
*/
const bool CX2DamageEffect::CEffect::SetLinkMainEffectByPos( IN const D3DXVECTOR3& vMyPos_, IN const D3DXVECTOR3& vTargetPos_, 
															 IN const float fDistance_ /*= 99999.f*/ )
{
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = GetMainEffect();		/// 연결 데미지 이펙트의 Main Mesh

	if ( NULL == pMeshInst )
		return false;

	/// 두 이펙트 사이 거리 설정 ( 거리 인자값 않들어 왔다면, 직접 구해주자 )
	float fDistance = fDistance_;

	if ( true == IsSamef( fDistance_, 99999.f ) )
		fDistance = GetDistance( vMyPos_, vTargetPos_ );

	if ( 0.f >= fDistance )
		fDistance = 1.f;

	float fLinkEffectRate = GetLinkEffectRate();

	/// 길이 배율 ( Main Effect 길이에 따른 스케일 설정 배율 )
	/// 작을수록 길어 진다.
	/// ( 참고로, ADD_PYLON_DYNAMO_SPARK01.X 의 길이 배율은 800.f 이다. )
	/// 동적으로 값을 연산할 수 있는 방법이 없을까...
	if ( 0.f > fLinkEffectRate )
	{
		ASSERT( ! "LinkEffectRate is InValid!!!" );
		fLinkEffectRate = 800.f;
	}

	/// 두 파일런 간 데미지 이펙트 매시 연결 설정 ( 셰도우 링커 참고 )
	D3DXVECTOR3 vResultPos( vTargetPos_.x - vMyPos_.x, vTargetPos_.y - vMyPos_.y, vTargetPos_.z - vMyPos_.z );
	pMeshInst->SetPos( vMyPos_ );
	vResultPos = GetDirVecToDegree( vResultPos );
	pMeshInst->SetRotateDegree( vResultPos );
	pMeshInst->SetMoveAxisAngleDegree( vResultPos );
	pMeshInst->SetScale( fDistance / fLinkEffectRate, 1.f, 1.f );

	return true;
}
#endif //SERV_ADD_LUNATIC_PSYKER