#include "StdAfx.h"
#include ".\x2effectset.h"
#include "Offline/X2OfflineLog.h"

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
// JUNE_FX ES_CREATE <set> <effect> <key> <detail>: one line per June creation-time branch
// taken (plan Phase 4a), at most one a second per (set, key, detail). The position and
// offset helpers are shared with the per-frame trace update, so a line can come from
// there too. Runtime, so it goes to offline_server.log.
static void JuneFxEsCreate( const CX2EffectSet::EffectSetInstance* pInstance_, const CX2EffectSet::EffectData* pEffectData_,
						   const WCHAR* pwszKey_, const WCHAR* pwszDetail_ )
{
	if( NULL == pInstance_ || NULL == pInstance_->m_pEffectSetData )
		return;

	std::wstring wstrThrottleKey = pInstance_->m_pEffectSetData->m_wstrEffectSetName;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszKey_;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszDetail_;

	static std::map<std::wstring, DWORD> s_mapLastLoggedTime;
	const DWORD dwNow = timeGetTime();
	std::map<std::wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrThrottleKey );
	if( itThrottle != s_mapLastLoggedTime.end() && dwNow - itThrottle->second < 1000 )
		return;
	s_mapLastLoggedTime[ wstrThrottleKey ] = dwNow;

	CX2OfflineLog::Server( L"JUNE_FX ES_CREATE %s %s %s %s", pInstance_->m_pEffectSetData->m_wstrEffectSetName.c_str(),
		( NULL != pEffectData_ ? pEffectData_->m_EffectName.c_str() : L"-" ), pwszKey_, pwszDetail_ );
}

// JUNE_FX ES_UPDATE / ES_DELETE <set> <effect> <key> <detail>: the same for the per-frame update and
// the delete pass (plan Phase 4b), at most one a second per (tag, set, key, detail).
static void JuneFxEsRuntime( const WCHAR* pwszTag_, const CX2EffectSet::EffectSetInstance* pInstance_,
							 const CX2EffectSet::EffectData* pEffectData_, const WCHAR* pwszKey_, const WCHAR* pwszDetail_ )
{
	if( NULL == pInstance_ || NULL == pInstance_->m_pEffectSetData )
		return;

	std::wstring wstrThrottleKey = pwszTag_;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pInstance_->m_pEffectSetData->m_wstrEffectSetName;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszKey_;
	wstrThrottleKey += L"|";
	wstrThrottleKey += pwszDetail_;

	static std::map<std::wstring, DWORD> s_mapLastLoggedTime;
	const DWORD dwNow = timeGetTime();
	std::map<std::wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrThrottleKey );
	if( itThrottle != s_mapLastLoggedTime.end() && dwNow - itThrottle->second < 1000 )
		return;
	s_mapLastLoggedTime[ wstrThrottleKey ] = dwNow;

	CX2OfflineLog::Server( L"JUNE_FX %s %s %s %s %s", pwszTag_, pInstance_->m_pEffectSetData->m_wstrEffectSetName.c_str(),
		( NULL != pEffectData_ ? pEffectData_->m_EffectName.c_str() : L"-" ), pwszKey_, pwszDetail_ );
}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
// June2017 FUN_321ee600 @ 321ee76b-321ee7c9, once per effect before it is created: the target-mesh
// index is max( BONE_TRACE_TARGET_MESH_INDEX, 0 ), and the entry counts when it is in range with a
// live handle, unless the effect is itself a BONE_TRACE_TARGET_MESH with a negative index (the rule
// GetEffectRotation uses). TRACE_MATRIX_IN_FIELD reads it.
static bool JuneGetCreationTargetMesh( const CX2EffectSet::EffectSetInstance* pInstance_, const CX2EffectSet::EffectData* pEffectData_,
									  OUT CKTDGXMeshPlayer::CXMeshInstanceHandle& hTargetMesh_ )
{
	hTargetMesh_ = INVALID_MESH_INSTANCE_HANDLE;
	if( pEffectData_->m_iBoneTraceTargetMeshIndex < 0 && true == pEffectData_->m_bBoneTraceTargetMesh )
		return false;

	const int iTargetMeshIndex = ( pEffectData_->m_iBoneTraceTargetMeshIndex < 0 ) ? 0 : pEffectData_->m_iBoneTraceTargetMeshIndex;
	CX2EffectSet::EFFECT_TYPE eTargetMeshEffectType = CX2EffectSet::ET_INVALID;
	return pInstance_->GetTargetMesh( iTargetMeshIndex, hTargetMesh_, eTargetMeshEffectType );
}

// June2017 FUN_321de0c0, EffectInstance::SetShowObject( bool ): one switch on EFFECT_TYPE. A damage effect
// goes through CEffect::SetShowObject (FUN_31f75570); a mesh or a particle sequence is looked up in the
// player its type names and gets CKTDGObject::SetShowObject's non-forced form, which June inlines.
// (June's particle sequence also hides each of its particles, FUN_31cf1cd0: a KTDXLIB particle-system
// difference, Phase 10's, not reproduced here.)
static void JuneSetEffectInstanceShow( CX2EffectSet::EffectInstance* pEffectInstance_, bool bShow_ )
{
	if( NULL == pEffectInstance_ || NULL == pEffectInstance_->m_pEffectData )
		return;

	CKTDGXMeshPlayer* pMeshPlayer = NULL;
	CKTDGParticleSystem* pParticleSystem = NULL;
	switch( pEffectInstance_->m_pEffectData->m_eEffectType )
	{
	case CX2EffectSet::ET_DAMAGE_EFFECT:
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			if( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance_->m_hDamageEffect ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			if( CX2DamageEffect::CEffect* pEffect = pEffectInstance_->m_pDamageEffect )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				pEffect->SetShowObject( bShow_ );
		}
		return;
	case CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR:     pMeshPlayer = g_pData->GetUIMajorXMeshPlayer(); break;
	case CX2EffectSet::ET_MESH_PLAYER_UI_MINOR:     pMeshPlayer = g_pData->GetUIMinorXMeshPlayer(); break;
	case CX2EffectSet::ET_MESH_PLAYER_GAME_MAJOR:   pMeshPlayer = ( NULL != g_pX2Game ) ? g_pX2Game->GetMajorXMeshPlayer() : NULL; break;
	case CX2EffectSet::ET_MESH_PLAYER_GAME_MINOR:   pMeshPlayer = ( NULL != g_pX2Game ) ? g_pX2Game->GetMinorXMeshPlayer() : NULL; break;
	case CX2EffectSet::ET_PARTICLE_UI_MAJOR:        pParticleSystem = g_pData->GetUIMajorParticle(); break;
	case CX2EffectSet::ET_PARTICLE_UI_MINOR:        pParticleSystem = g_pData->GetUIMinorParticle(); break;
	case CX2EffectSet::ET_PARTICLE_GAME_MAJOR:      pParticleSystem = ( NULL != g_pX2Game ) ? g_pX2Game->GetMajorParticle() : NULL; break;
	case CX2EffectSet::ET_PARTICLE_GAME_MINOR:      pParticleSystem = ( NULL != g_pX2Game ) ? g_pX2Game->GetMinorParticle() : NULL; break;
	default:
		return;
	}

	if( NULL != pMeshPlayer && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance_->m_hMeshPlayer )
	{
		if( CKTDGXMeshPlayer::CXMeshInstance* pInstance = pMeshPlayer->GetMeshInstance( pEffectInstance_->m_hMeshPlayer ) )
			pInstance->SetShowObject( bShow_ );
	}
	else if( NULL != pParticleSystem && INVALID_PARTICLE_SEQUENCE_HANDLE != pEffectInstance_->m_hParticleSequence )
	{
		if( CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticleSystem->GetInstanceSequence( pEffectInstance_->m_hParticleSequence ) )
			pSeq->SetShowObject( bShow_ );
	}
}

// June2017 FUN_321dee80, SOUND%d, called once per effect instance from OnFrameMove_UpdateEffect
// (@ 321e07ee-321e07f7), so a set with no created effect plays nothing.
static void JuneOnFrameMoveSound( CX2EffectSet::EffectSetInstance* pInstance_ )
{
	if( NULL == pInstance_ || NULL == pInstance_->m_pEffectSetData )
		return;

	// @ 321deeb5-321def45: on the first call the templet's entries are copied, each not yet played
	if( false == pInstance_->m_bSoundPlayDataCopied )
	{
		pInstance_->m_bSoundPlayDataCopied = true;
		pInstance_->m_vecSoundPlayData.clear();
		for( UINT iSound = 0; iSound < pInstance_->m_pEffectSetData->m_vecSoundData.size(); ++iSound )
		{
			CX2EffectSet::EffectSetInstance::SoundPlayData soundPlayData;
			soundPlayData.m_SoundData = pInstance_->m_pEffectSetData->m_vecSoundData[iSound];
			pInstance_->m_vecSoundPlayData.push_back( soundPlayData );
		}
	}

	for( int iSound = 0; iSound < static_cast<int>( pInstance_->m_vecSoundPlayData.size() ); ++iSound )
	{
		CX2EffectSet::EffectSetInstance::SoundPlayData& soundPlayData = pInstance_->m_vecSoundPlayData[iSound];
		const CX2DamageEffect::SoundData& soundData = soundPlayData.m_SoundData;

		// @ 321def80-321defaf: USE_GLOBAL_TIME against the set's elapsed time, else the set skin anim's
		// EventTimer (FUN_31d28e10 = CKTDGXSkinAnim::EventTimer); no skin anim, no sound
		bool bTime = false;
		if( true == soundData.bUseGlobalTime )
			bTime = ( soundData.fEventTime < pInstance_->m_fElapsedTime );
		else if( pInstance_->m_pXSkinAnim != NULL )
			bTime = pInstance_->m_pXSkinAnim->EventTimer( soundData.fEventTime );

		if( true == bTime && false == soundPlayData.m_bPlayed )
		{
			// @ 321defbf-321df046: at the pet's, else the owner's (GetPos, vfunc 0x138), else the square
			// unit's position
			D3DXVECTOR3 vSoundPos( 0.f, 0.f, 0.f );
	#ifdef SERV_PET_SYSTEM
			if( NULL != pInstance_->m_pPet )
				vSoundPos = pInstance_->m_pPet->GetPos();
			else
	#endif SERV_PET_SYSTEM
			if( NULL != pInstance_->GetOwnerUnit() )
				vSoundPos = pInstance_->GetOwnerUnit()->GetPos();
			else if( pInstance_->m_pSquareUnit != NULL )
				vSoundPos = pInstance_->m_pSquareUnit->GetPos();

			// @ 321df04a-321df0c6: RATE is rolled on every call until it plays; OpenSound( name, 10, true,
			// -1 ), Set3DPosition, Play( false, true ): a LOOP sound is not looped by the device
			if( rand() % 100 < soundData.iRate )
			{
				CKTDXDeviceSound* pSound = g_pKTDXApp->GetDeviceManager()->OpenSound( soundData.fileName, 10, true, -1.f );
				if( NULL != pSound )
				{
					pSound->Set3DPosition( vSoundPos );
					pSound->Play( false, true );
					soundPlayData.m_bPlayed = true;
					pInstance_->m_vecpPlayingSound.push_back( pSound );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxEsRuntime( L"ES_UPDATE", pInstance_, NULL, L"SOUND%d", soundData.fileName.c_str() );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
			}
		}

		// @ 321df0cb-321df12d: LOOP re-arms the entry at the end of each anim cycle (m_bAnimEnd, skin +0x2ac)
		// unless USE_GLOBAL_TIME; any other entry is dropped once played
		if( true == soundData.bLoop )
		{
			if( false == soundData.bUseGlobalTime && pInstance_->m_pXSkinAnim != NULL && true == pInstance_->m_pXSkinAnim->IsAnimationEnd() )
				soundPlayData.m_bPlayed = false;
		}
		else if( true == soundPlayData.m_bPlayed )
		{
			pInstance_->m_vecSoundPlayData.erase( pInstance_->m_vecSoundPlayData.begin() + iSound );
			--iSound;
		}
	}

	// @ 321df13e-321df1ea: a sound that has stopped is stopped and closed
	for( int iSound = 0; iSound < static_cast<int>( pInstance_->m_vecpPlayingSound.size() ); ++iSound )
	{
		CKTDXDeviceSound* pSound = pInstance_->m_vecpPlayingSound[iSound];
		if( NULL != pSound && false == pSound->IsPlaying() )
		{
			pSound->Stop();
			SAFE_CLOSE( pSound );
			pInstance_->m_vecpPlayingSound.erase( pInstance_->m_vecpPlayingSound.begin() + iSound );
			--iSound;
		}
	}
}

static CKTDGXMeshPlayer::CXMeshInstance* JuneGetFieldTargetMesh( CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh_ );

// June2017 FUN_321e0080 @ 321e0d1a-321e0f89, TRACE_MATRIX_IN_FIELD per frame, UI-major particles only
// (case 6), after the move and the land height. The bone is the GROUP_NAME child's, else BONE_NAME
// (@ 321e0d45-321e0d6a).
static void JuneTraceMatrixInFieldParticleUpdate( CX2EffectSet::EffectSetInstance* pInstance_, CX2EffectSet::EffectInstance* pEffectInstance_,
												  CX2EffectSet::EffectData* pEffectData_, CKTDGParticleSystem::CParticleEventSequence* pSequence_,
												  bool bTargetMeshLive_, CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh_, const D3DXVECTOR3& vEffectPos_ )
{
	if( NULL == pSequence_ || false == pEffectData_->m_bTraceMatrixInField )
		return;

	const WCHAR* pwszBoneName = ( true == pEffectInstance_->m_bGroupChild ) ? pEffectInstance_->m_wstrGroupChildBoneName.c_str() : pEffectData_->m_PositionBoneName.c_str();

	if( true == pEffectData_->m_bTraceMatrix && false == bTargetMeshLive_ )
	{
		// @ 321e0d87-321e0e5d: the set's own skin anim (tested @ 321e0d8e; unlike creation, no owner test).
		// The frame's matrix, then the position plus POSITION_OFFSET, SetPosition( pos, false, false ).
		if( NULL == pInstance_->m_pXSkinAnim )
			return;
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pInstance_->m_pXSkinAnim->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;
		pSequence_->SetUseDXMatrix( pFrame->combineMatrix );
		pSequence_->SetPosition( pSequence_->GetPosition() + pEffectData_->m_vPositionOffset, false, false );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxEsRuntime( L"ES_UPDATE", pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"particle_matrix=instance_bone(321e0dc2)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
	else if( true == bTargetMeshLive_ )
	{
		// @ 321e0e62-321e0f84: the target mesh, looked up in the UI-major mesh player whatever its type.
		// June does not test the mesh's skin anim (@ 321e0ebb); a mesh without one is skipped here.
		CKTDGXMeshPlayer::CXMeshInstance* pTargetMesh = JuneGetFieldTargetMesh( hTargetMesh_ );
		if( NULL == pTargetMesh || NULL == pTargetMesh->GetXSkinAnim() )
			return;
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pTargetMesh->GetXSkinAnim()->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;
		if( true == pEffectData_->m_bTraceMatrix )
		{
			pSequence_->SetUseDXMatrix( pFrame->combineMatrix );
			pSequence_->SetPosition( pSequence_->GetPosition() + pEffectData_->m_vPositionOffset, false, false );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsRuntime( L"ES_UPDATE", pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"particle_matrix=target_mesh_bone(321e0ee1)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		else
		{
			// @ 321e0f78-321e0f84: without TRACE_MATRIX the effect position again, SetPosition( pos, false, false )
			pSequence_->SetPosition( vEffectPos_, false, false );
		}
	}
}

// June2017 FUN_3264ca10: an emotion folded onto the three sitting ones (1 ET_SITWAIT, 2 ET_SITREADY,
// 3 ET_STANDUP, as March's CX2Unit::EMOTION_TYPE numbers them), anything else 0. June's switch also folds
// its cash-emotion ids (25-27, 54-59), which March's enum does not have.
static int JuneFoldTriggerEmotion( int iEmotion_ )
{
	switch( iEmotion_ )
	{
	case 1: case 0x1a: case 0x37: case 0x3a: return 1;
	case 2: case 0x19: case 0x36: case 0x39: return 2;
	case 3: case 0x1b: case 0x38: case 0x3b: return 3;
	default: return 0;
	}
}

static bool JuneTriggerBoundTo( const CX2EffectSet::MeshTriggerData& kTrigger_, int iEmotion_ )
{
	for( UINT i = 0; i < kTrigger_.m_vecBoundMotionType.size(); ++i )
	{
		if( kTrigger_.m_vecBoundMotionType[i] == iEmotion_ )
			return true;
	}
	return false;
}

// June2017 FUN_321dfc40, a mesh effect's triggers, called by OnFrameMove_UpdateEffect @ 321e037b / 321e0404
// with the mesh, the inline MESH_TRIGGER_DATA, the EffectInstance's copy of MESH_TRIGGER_DATA_LIST, the set's
// skin anim and the set. (Its other caller, the unit viewer 329585f0, passes no set but two viewer
// emotions; that branch belongs to the unit viewer and is not reproduced.)
static void JuneRunMeshTriggers( CX2EffectSet* pEffectSet_, CKTDGXMeshPlayer::CXMeshInstance* pMesh_, CX2EffectSet::EffectData* pEffectData_,
								 CX2EffectSet::EffectInstance* pEffectInstance_, CX2EffectSet::EffectSetInstance* pInstance_ )
{
	// @ 321dfc6b-321dfc9b: nothing without the set's skin anim or while the mesh is blocked (+0x635)
	if( NULL == pMesh_ || NULL == pInstance_->m_pXSkinAnim || true == pMesh_->GetTriggerBlocked() )
		return;
	CKTDGXSkinAnim* pSkinAnim = &(*pInstance_->m_pXSkinAnim);

	std::vector<CX2EffectSet::MeshTriggerData>& vecTrigger = pEffectInstance_->m_vecMeshTriggerData;
	if( true == vecTrigger.empty() )
	{
		// @ 321dffd0-321e0033: the inline MESH_TRIGGER_DATA, once (mesh +0x634) its TRIGGER_TIME (>= 0) is
		// reached: the motion at the mesh's own speed (-1), with transition, from 0. The mesh restores the
		// previous anim when it ends (+0x194 is 1 for a mesh with no list).
		const CX2EffectSet::MeshTriggerData& kTrigger = pEffectData_->m_MeshTriggerData;
		if( 0.f <= kTrigger.m_fTriggerTime && false == pMesh_->GetTriggerAnimPlayed() && kTrigger.m_fTriggerTime <= pSkinAnim->GetNowAnimationTime() )
		{
			pMesh_->ChangeAnimByTrigger( kTrigger.m_wstrMotionName.c_str(), static_cast<CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE>( kTrigger.m_iPlayType ), -1.f, true, 0.f );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsRuntime( L"ES_UPDATE", pInstance_, pEffectData_, L"MESH_TRIGGER_DATA", kTrigger.m_wstrMotionName.c_str() );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		return;
	}

	for( UINT iTrigger = 0; iTrigger < vecTrigger.size(); ++iTrigger )
	{
		CX2EffectSet::MeshTriggerData& kTrigger = vecTrigger[iTrigger];
		if( true == kTrigger.m_bFiredOnce )     // @ 321dfce0
			continue;

		// @ 321dfcea-321dfdde: the owner's emotion, folded, must be in BOUND_MOTION_TYPE_LIST. A set without a
		// square unit asks its game unit (user +0x89d8, March GetNowEmotionId); one with a square unit asks it
		// (+0x608). June reads the user field from any game unit; a non-user owner gives 0 here.
		bool bGame = false;
		bool bSquare = false;
		if( pInstance_->m_pSquareUnit == NULL )
		{
			if( null == pInstance_->m_optrGameUnit )
				continue;
			int iEmotion = CX2Unit::ET_NONE;
			if( CX2GameUnit::GUT_USER == pInstance_->m_optrGameUnit->GetGameUnitType() )
				iEmotion = static_cast<CX2GUUser*>( pInstance_->m_optrGameUnit.GetObservable() )->GetNowEmotionId();
			bGame = JuneTriggerBoundTo( kTrigger, JuneFoldTriggerEmotion( iEmotion ) );
		}
		else
		{
			bSquare = JuneTriggerBoundTo( kTrigger, JuneFoldTriggerEmotion( pInstance_->m_pSquareUnit->GetNowEmotionId() ) );
		}
		if( false == bGame && false == bSquare )
			continue;

		// @ 321dfde0-321dfe49: armed when the skin plays another anim than when it last fired, or the same
		// one's play count changed while it is not past GetMaxAnimationTime() (max - transition)
		if( 0 != kTrigger.m_wstrTriggeredAnimName.compare( pSkinAnim->GetNowAnimName() ) ||
			( kTrigger.m_iTriggeredPlayCount != pSkinAnim->GetNowPlayCount() && pSkinAnim->GetNowAnimationTime() <= pSkinAnim->GetMaxAnimationTime() ) )
			kTrigger.m_bArmed = true;

		// @ 321dfe4d-321dfe6f: fires once armed and TRIGGER_TIME is reached
		if( false == kTrigger.m_bArmed || kTrigger.m_fTriggerTime > pSkinAnim->GetNowAnimationTime() )
			continue;

		// @ 321dfe75-321dfecf: ANIMTIME_ON_START_GAME / _SQUARE gives the start time while its first-use byte
		// is set; ONCE clears that byte
		float fStartTime = 0.f;
		CX2EffectSet::AnimTimeOnStartData* pAnimTimeOnStart = ( true == bGame ) ? &kTrigger.m_AnimTimeOnStartGame : &kTrigger.m_AnimTimeOnStartSquare;
		if( true == pAnimTimeOnStart->m_bFirstUse )
		{
			fStartTime = pAnimTimeOnStart->m_fTime;
			pAnimTimeOnStart->m_bFirstUse = ( false == pAnimTimeOnStart->m_bOnce );
		}

		// @ 321dfed2-321dff24: the motion at speed 1 with USE_MESH_TRANSITION; then the skin's play count and
		// anim, now the motion's, are kept
		pMesh_->ChangeAnimByTrigger( kTrigger.m_wstrMotionName.c_str(), static_cast<CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE>( kTrigger.m_iPlayType ), 1.f, kTrigger.m_bUseMeshTransition, fStartTime );
		kTrigger.m_iTriggeredPlayCount = pSkinAnim->GetNowPlayCount();
		kTrigger.m_wstrTriggeredAnimName = pSkinAnim->GetNowAnimName();
		kTrigger.m_bArmed = false;

		if( true == kTrigger.m_bTriggerOnce )           // @ 321dff20-321dff2a
			kTrigger.m_bFiredOnce = true;
		if( true == kTrigger.m_bStopTracingOnTrigger )  // @ 321dff2e-321dff3c
			pInstance_->m_bStopTracingOnTrigger = true;
		// @ 321dff40-321dff83: every TRIGGER_EFFECTSET_LIST name on this mesh (PlayEffectSetByMeshPlayer,
		// 321df9c0), always as ET_MESH_PLAYER_UI_MAJOR (June pushes 2 whatever the mesh's player)
		for( UINT iSet = 0; iSet < kTrigger.m_vecTriggerEffectSetName.size(); ++iSet )
			pEffectSet_->PlayEffectSetByMeshPlayer( kTrigger.m_vecTriggerEffectSetName[iSet].c_str(), pMesh_->GetHandle(), CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR );
		if( true == kTrigger.m_bUseEffectSetToggleVisibilityTimer )  // @ 321dff89-321dff96
		{
			pInstance_->m_bToggleVisibilityPending = true;
			pInstance_->m_fToggleVisibilityTimer = kTrigger.m_fEffectSetToggleVisibilityTimer;
		}
		if( true == kTrigger.m_bBlockTrigger )          // @ 321dff99-321dffb1
			pMesh_->SetTriggerBlock( kTrigger.m_fBlockTriggerTime );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxEsRuntime( L"ES_UPDATE", pInstance_, pEffectData_, L"MESH_TRIGGER_DATA_LIST", kTrigger.m_wstrMotionName.c_str() );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
}

// June2017 FUN_321ee600 @ 321efa34-321efa5d (the same in all eight cases): TRACE_MATRIX_IN_FIELD looks
// the target mesh up in g_pData's UI-major mesh player, whatever the entry's EFFECT_TYPE. In the field
// there is no game, and effect sets play on the UI players.
static CKTDGXMeshPlayer::CXMeshInstance* JuneGetFieldTargetMesh( CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh_ )
{
	if( NULL == g_pData || NULL == g_pData->GetUIMajorXMeshPlayer() )
		return NULL;

	return g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( hTargetMesh_ );
}

/** @function : JuneTraceMatrixInFieldMesh
	@brief : June2017 TRACE_MATRIX_IN_FIELD for the mesh cases of FUN_321ee600: @ 321ef8ee-321efb99
			 (UI major), 321efe84-321f00b8 (UI minor), 321f03b6-321f05da (game major) and
			 321f088a-321f0aae (game minor), after the scale and before the land height.
*/
static void JuneTraceMatrixInFieldMesh( CX2EffectSet::EffectSetInstance* pInstance_, CX2EffectSet::EffectData* pEffectData_,
									   CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance_ )
{
	if( NULL == pMeshInstance_ || false == pEffectData_->m_bTraceMatrixInField )
		return;

	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	const bool bTargetMesh = JuneGetCreationTargetMesh( pInstance_, pEffectData_, hTargetMesh );
	const WCHAR* pwszBoneName = pEffectData_->m_PositionBoneName.c_str();

	if( true == pEffectData_->m_bTraceMatrix && NULL != pInstance_->GetOwnerUnit() && false == bTargetMesh )
	{
		// @ 321ef94f-321efa24: BONE_NAME is looked up in the new mesh's own skeleton (June mesh +0x3c0 =
		// m_pXSkinAnim), not the owner's. Its combined matrix becomes the mesh's DX matrix
		// (FUN_31f8ad90 = SetUseDXMatrix), and POSITION_OFFSET is added to the position once more.
		// June does not test the skin anim; a mesh without one would fault, so it is skipped here.
		if( NULL == pMeshInstance_->GetXSkinAnim() )
		{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"skipped=mesh_has_no_skin_anim" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			return;
		}

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pMeshInstance_->GetXSkinAnim()->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;

		pMeshInstance_->SetUseDXMatrix( pFrame->combineMatrix );
		pMeshInstance_->SetPos( pMeshInstance_->GetPos() + pEffectData_->m_vPositionOffset );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"mesh_matrix=own_bone(321ef97d)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
	else if( true == bTargetMesh )
	{
		// @ 321efa29-321efb77: the bone of the target mesh. With TRACE_MATRIX its combined matrix and the
		// offset, as above; without, the mesh is moved to the bone (FUN_321f2990 = GetBonePos).
		CKTDGXMeshPlayer::CXMeshInstance* pTargetMesh = JuneGetFieldTargetMesh( hTargetMesh );
		if( NULL == pTargetMesh || NULL == pTargetMesh->GetXSkinAnim() )
		{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"skipped=target_mesh_not_in_ui_major(321efa5d)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			return;
		}

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pTargetMesh->GetXSkinAnim()->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;

		if( true == pEffectData_->m_bTraceMatrix )
		{
			pMeshInstance_->SetUseDXMatrix( pFrame->combineMatrix );
			pMeshInstance_->SetPos( pMeshInstance_->GetPos() + pEffectData_->m_vPositionOffset );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"mesh_matrix=target_mesh_bone(321efaad)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		else
		{
			pMeshInstance_->SetPos( pTargetMesh->GetBonePos( pwszBoneName ) );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"mesh_pos=target_mesh_bone(321efb68)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}
}

/** @function : JuneTraceMatrixInFieldParticle
	@brief : June2017 TRACE_MATRIX_IN_FIELD for the particle cases of FUN_321ee600: @ 321f0ce3-321f0f45
			 (UI major), 321f11da-321f1448 (UI minor), 321f1738-321f19b1 (game major) and
			 321f1c12-321f1e7e (game minor), after the scale and before SET_OVER_UI. With GROUP_NAME
			 June uses the group child's bone (@ 321f0d10-321f0d3b), which the caller passes.
*/
static void JuneTraceMatrixInFieldParticle( CX2EffectSet::EffectSetInstance* pInstance_, CX2EffectSet::EffectData* pEffectData_,
										   CKTDGParticleSystem::CParticleEventSequence* pSequence_, const WCHAR* pwszBoneName_ )
{
	if( NULL == pSequence_ || false == pEffectData_->m_bTraceMatrixInField )
		return;

	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	const bool bTargetMesh = JuneGetCreationTargetMesh( pInstance_, pEffectData_, hTargetMesh );
	// @ 321f0d10-321f0d3b: the GROUP_NAME child's bone when the effect has GROUP_NAME, else BONE_NAME
	const WCHAR* pwszBoneName = pwszBoneName_;

	if( true == pEffectData_->m_bTraceMatrix && NULL != pInstance_->GetOwnerUnit() && false == bTargetMesh )
	{
		// @ 321f0d64-321f0e08: unlike the mesh cases, the bone comes from the set's own skin anim (June
		// instance +0x8 = m_pXSkinAnim, tested @ 321f0d67). Its combined matrix becomes the sequence's
		// DX matrix (FUN_321f2940), and POSITION_OFFSET is added to the position once more
		// (FUN_31c19f20 / FUN_31cf1a40( pos, false, false )).
		if( NULL == pInstance_->m_pXSkinAnim )
			return;

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pInstance_->m_pXSkinAnim->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;

		pSequence_->SetUseDXMatrix( pFrame->combineMatrix );
		pSequence_->SetPosition( pEffectData_->m_vPositionOffset + pSequence_->GetPosition() );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"particle_matrix=instance_bone(321f0d88)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
	else if( true == bTargetMesh )
	{
		// @ 321f0e0d-321f0f2c: the bone of the target mesh, as in the mesh cases
		CKTDGXMeshPlayer::CXMeshInstance* pTargetMesh = JuneGetFieldTargetMesh( hTargetMesh );
		if( NULL == pTargetMesh || NULL == pTargetMesh->GetXSkinAnim() )
		{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"skipped=target_mesh_not_in_ui_major(321f0e41)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			return;
		}

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pTargetMesh->GetXSkinAnim()->GetCloneFrame( pwszBoneName );
		if( NULL == pFrame )
			return;

		if( true == pEffectData_->m_bTraceMatrix )
		{
			pSequence_->SetUseDXMatrix( pFrame->combineMatrix );
			pSequence_->SetPosition( pEffectData_->m_vPositionOffset + pSequence_->GetPosition() );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"particle_matrix=target_mesh_bone(321f0e91)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		else
		{
			pSequence_->SetPosition( pTargetMesh->GetBonePos( pwszBoneName ) );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"TRACE_MATRIX_IN_FIELD", L"particle_pos=target_mesh_bone(321f0f24)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}
}

// June2017 FUN_321ee600 @ 321f0f49-321f0f63 (likewise @ 321f144c, 321f19b1, 321f1e82): SET_OVER_UI sets
// the particle's m_bOverUI (June +0x170) and also its layer, June +0x164 = m_Layer (placed by the field
// order of June's CKTDGObject ctor FUN_31d28a20), to 500 = XDL_DIRECTING. March set only m_bOverUI.
static void JuneSetOverUI( CX2EffectSet::EffectSetInstance* pInstance_, CX2EffectSet::EffectData* pEffectData_,
						  CKTDGParticleSystem::CParticleEventSequence* pSequence_ )
{
	pSequence_->SetOverUI( true );
	pSequence_->SetLayer( XDL_DIRECTING );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	JuneFxEsCreate( pInstance_, pEffectData_, L"SET_OVER_UI", L"layer=XDL_DIRECTING(321f0f59)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}

// Plan rule 5 at run time: a key whose system this build lacks is logged once per key per templet, at the
// June site that would act on it, as "JUNE_FX unsupported <KEY> in <templet>: <system> absent". It is not
// a diagnostic, so it is not behind SERV_IRUHADEV_JUNE_FX_DEBUG. The parse-time lines (ErrorLog.txt,
// SERV_IRUHADEV_JUNE_EFFECTSET_PARSE) use the same system wording.
static void JuneFxUnsupported( const CX2EffectSet::EffectSetData* pEffectSetData_, const WCHAR* pwszKey_, const WCHAR* pwszScope_,
							  const WCHAR* pwszSystem_, int iValue_, const WCHAR* pwszJuneSite_ )
{
	if( NULL == pEffectSetData_ )
		return;

	static std::set<std::wstring> s_setLogged;
	// the templet-level and per-effect reads are separate rows, but each is logged once per templet: the
	// scope (" EFFECT<i>") is printed, and only whether there is one goes into the key
	std::wstring wstrLogKey = pwszKey_;
	wstrLogKey += ( L'\0' == pwszScope_[0] ) ? L"" : L"(effect)";
	wstrLogKey += L"|";
	wstrLogKey += pEffectSetData_->m_wstrEffectSetName;
	if( false == s_setLogged.insert( wstrLogKey ).second )
		return;

	CX2OfflineLog::Server( L"JUNE_FX unsupported %s in %s%s: %s absent (value %d, June %s)", pwszKey_,
		pEffectSetData_->m_wstrEffectSetName.c_str(), pwszScope_, pwszSystem_, iValue_, pwszJuneSite_ );
}

/** @function : JuneGroupChildPosition
	@brief : June2017 FUN_321ee600 @ 321eeeeb-321ef1ef, one pass of the GROUP_NAME loop: the bone
			 <base>_GROUPCHILD_<n>, the position there, and (with BONE_NAME) that position in the target
			 mesh's BONE_NAME frame. False ends the loop (@ 321f2581): the bone is on neither the holder
			 nor the creation target mesh.
*/
static bool JuneGroupChildPosition( CX2EffectSet::EffectSetInstance* pInstance_, CX2EffectSet::EffectData* pEffectData_,
								   const std::wstring& wstrGroupBase_, int iGroupChild_, int iCreationTargetMeshIndex_, bool bCreationTargetMesh_,
								   OUT std::wstring& wstrBoneName_, OUT D3DXVECTOR3& vPos_, OUT D3DXVECTOR3& vOffset_ )
{
	// @ 321eeeeb-321ef01d: <base> + L"_GROUPCHILD_" (3347e540) + n formatted with L"%d" (3346a6b0)
	WCHAR wszChild[16] = {0};
	StringCchPrintfW( wszChild, ARRAY_SIZE(wszChild), L"%d", iGroupChild_ );
	wstrBoneName_ = wstrGroupBase_;
	wstrBoneName_ += L"_GROUPCHILD_";
	wstrBoneName_ += wszChild;
	vOffset_ = D3DXVECTOR3( 0.f, 0.f, 0.f );

	// @ 321ef020-321ef067: the position at that bone (FUN_321dd590), which also says whether the bone exists
	bool bBoneFound = false;
	vPos_ = pInstance_->GetEffectPositionByBoneName( pEffectData_, wstrBoneName_, bBoneFound );
	if( false == bBoneFound )
	{
		// @ 321ef069-321ef0c9: a bone the holder lacks still counts when the creation target mesh's own
		// skeleton has it (FUN_321dc910, then mesh +0x3c0 = m_pXSkinAnim -> FUN_31d38a20 = GetCloneFrame). The
		// position is left as FUN_321dd590 returned it (for the owner, its head bone).
		if( false == bCreationTargetMesh_ )
			return false;
		CKTDGXMeshPlayer::CXMeshInstance* pTargetMesh = pInstance_->GetMeshInstanceByTargetMesh( iCreationTargetMeshIndex_ );
		if( NULL == pTargetMesh || NULL == pTargetMesh->GetXSkinAnim() ||
			NULL == pTargetMesh->GetXSkinAnim()->GetCloneFrame( wstrBoneName_.c_str() ) )
			return false;
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		JuneFxEsCreate( pInstance_, pEffectData_, L"GROUP_NAME", L"child_bone_on_target_mesh_only(321ef0c9)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}

	// @ 321ef0ce-321ef1ef: with BONE_NAME, the position is taken into the target mesh's BONE_NAME frame: the
	// inverse of its combined matrix (FUN_330869f6 = D3DXMatrixInverse( out, NULL, m )), D3DXVec3Transform
	// (FUN_33086a08, a 4-float result), then / w. Both D3DX calls are named by shape; their IAT slots
	// (333bce78, 333bce6c) are bound in the dump.
	if( false == pEffectData_->m_PositionBoneName.empty() )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pTargetMesh = pInstance_->GetMeshInstanceByTargetMesh( iCreationTargetMeshIndex_ );
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;
		if( NULL != pTargetMesh && NULL != pTargetMesh->GetXSkinAnim() )
			pFrame = pTargetMesh->GetXSkinAnim()->GetCloneFrame( pEffectData_->m_PositionBoneName.c_str() );
		if( NULL != pFrame )
		{
			D3DXMATRIX matInverse;
			D3DXMatrixInverse( &matInverse, NULL, &pFrame->combineMatrix );
			D3DXVECTOR4 vTransformed;
			D3DXVec3Transform( &vTransformed, &vPos_, &matInverse );
			const float fInverseW = 1.f / vTransformed.w;
			vOffset_ = D3DXVECTOR3( vTransformed.x * fInverseW, vTransformed.y * fInverseW, vTransformed.z * fInverseW );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"GROUP_NAME", L"offset_in_target_mesh_bone(321ef140)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
		else
		{
			// June still copies its stack vector into the EffectInstance (@ 321f24ac-321f24f1), uncomputed. That
			// value cannot be reproduced; the offset stays at the ctor's (0,0,0).
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsCreate( pInstance_, pEffectData_, L"GROUP_NAME", L"offset_not_reproduced=no_target_mesh_bone(321ef0ea)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}

	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	std::wstring wstrJuneFx = L"child=";
	wstrJuneFx += wstrBoneName_;
	JuneFxEsCreate( pInstance_, pEffectData_, L"GROUP_NAME", wstrJuneFx.c_str() );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	return true;
}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME




#pragma DMLEE_NOTE( "effect set에 사운드 작업할 수 있게 기능 추가해야 함" )

#ifndef X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
CKTDXSimpleHandleInterface< CX2EffectSet::EffectSetInstance > CX2EffectSet::s_HandleManager;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE


CX2EffectSet::CX2EffectSet(
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    unsigned char ucSystemID 
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK        
    )
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
: m_coInstanceHandleList( LIST_NUM, 64 )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
{
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    m_ucSystemID = ucSystemID & 0x3;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_vecpEffectSetInstance.reserve( 64 );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}

CX2EffectSet::~CX2EffectSet(void)
{
	ClearTempletAndInstance();
}




void CX2EffectSet::ClearTempletAndInstance()
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    StopEffectSetAll();
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	BOOST_TEST_FOREACH( EffectSetInstance*, pEffectSetInstance, m_vecpEffectSetInstance )
	{
		SAFE_DELETE( pEffectSetInstance );
	}
	m_vecpEffectSetInstance.clear();
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE


	BOOST_TEST_FOREACH( EffectSetDataMap::value_type&, value, m_mapEffectSetTemplet )
	{
		SAFE_DELETE( value.second );
	}
	m_mapEffectSetTemplet.clear();

#ifdef EFFECT_TOOL
	m_vecEffectSetTemplet.clear();
	m_mapEffectSetDescription.clear();
#endif //EFFECT_TOOL
}


void CX2EffectSet::OnFrameMove( double fTime, float fElapsedTime )
{

	KTDXPROFILE();
	OnFrameMove_CreateEffect(fTime, fElapsedTime);
	OnFrameMove_DeleteEffect();
	OnFrameMove_UpdateEffect();
	OnFrameMove_StartCameraShake();

}

void CX2EffectSet::OnFrameMove_CreateEffect( double fTime, float fElapsedTime )
{

	KTDXPROFILE();

	// 이펙트 생성 및 effectsetinstance framemove
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    BOOST_TEST_FOREACH( EffectSetInstance*, pEffectSetInstance, m_vecpEffectSetInstance )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		pEffectSetInstance->OnFrameMove( fTime, fElapsedTime );

		bool bTraceTargetDead = false;
		for( UINT i=0; i<pEffectSetInstance->m_vecEffectInstanceCreated.size(); i++ )
		{
			if( true == pEffectSetInstance->m_vecEffectInstanceCreated[i] )
				continue;

			EffectData* pEffectData = pEffectSetInstance->m_pEffectSetData->m_vecpEffectData[i];
			if( NULL == pEffectData )
				continue;




			if( false == bTraceTargetDead && 
				true == pEffectData->m_bStartWhenTraceTargetDead &&
				null != pEffectSetInstance->m_optrTraceTargetGameUnit &&
				NULL != g_pX2Game &&
				pEffectSetInstance->m_optrTraceTargetGameUnit->GetNowHp() <= 0.f )						// trace target이 죽으면 이펙트 바로 생성 
			{
				bTraceTargetDead = true;
			}



			if( false == bTraceTargetDead &&
				0.f != pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime ) // 0초에 시작이면 무조건 바로 생성
			{
				EFFECT_SET_TIMER_TYPE eTimingType = pEffectSetInstance->m_pEffectSetData->m_eEventTimerType;
				if( false == pEffectSetInstance->m_bUseAnimationTiming &&
					ESTT_ANIM == eTimingType )
				{
					eTimingType = ESTT_CUSTOM;
				}
#ifdef EFFECT_TOOL
				eTimingType = ESTT_CUSTOM;
#endif //EFFECT_TOOL
				switch( eTimingType )
				{
				default:
				case ESTT_ANIM:
					{
#ifdef SERV_PET_SYSTEM
						if( pEffectSetInstance->m_pPet != NULL )
						{
							if( pEffectSetInstance->m_pPet->GetXSkinAnim() != NULL &&
								pEffectSetInstance->m_pPet->GetXSkinAnim()->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
#ifdef RIDING_SYSTEM
						else if ( NULL != pEffectSetInstance->GetOwnerUnit() && 
								  NULL != pEffectSetInstance->GetOwnerUnit()->GetRidingPet() && 
								  true == pEffectSetInstance->GetOwnerUnit()->GetRidingOn() )
						{
							if( pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr() != NULL &&
								pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr()->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
#endif //RIDING_SYSTEM
						else
						{
							if( 
#ifdef  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
#endif  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX
                                pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}						
#else
						if( 
#ifdef  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
#endif  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX                            
                            pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
							continue;
#endif						
					} break;

				case ESTT_CUSTOM:
					{
						if( pEffectSetInstance->m_fElapsedTime < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
						{
							continue;
						}
					} break;
				}
			}

			pEffectSetInstance->m_vecEffectInstanceCreated[i] = true;

			if( true == bTraceTargetDead &&
				false == pEffectData->m_bStartWhenTraceTargetDead )
			{
				continue;
			}

#ifdef MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE
			// 거리 제한이 지정되어 있다면
			if ( pEffectData->m_fLimitDistanceSq > 0.f && 
				NULL != g_pX2Game && 
				NULL != g_pX2Game->GetMyUnit() )
			{
#ifdef _IN_HOUSE_
				if( ET_DAMAGE_EFFECT == pEffectData->m_eEffectType )
					DISPLAY_ERROR(L" DamageEffect에는 LIMIT_DISTANCE를 설정 하지 마세요! ");
#endif // _IN_HOUSE_

				// 거리 제한에 걸리면 만든 것으로 체크하고 실제로 만들지는 않도록 설정
				const D3DXVECTOR3& vMyUserPos = g_pX2Game->GetMyUnit()->GetPos();
				if( GetDistance3Sq( vMyUserPos, pEffectSetInstance->m_vPosition ) > pEffectData->m_fLimitDistanceSq )
				{
					pEffectSetInstance->m_vecEffectInstanceCreated[i] = true;
					continue;
				}
			}

#endif // MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321ee600 @ 321eee07-321eee21: rotation and scale are taken once per effect, before the
			// position, and every GROUP_NAME child below shares them
			const D3DXVECTOR3 vEffectRotation = pEffectSetInstance->GetEffectRotation( pEffectData );
			const D3DXVECTOR3 vEffectScale = pEffectSetInstance->GetEffectScale( pEffectData );

			// @ 321ee76b-321ee7c9: the creation target mesh, index max( BONE_TRACE_TARGET_MESH_INDEX, 0 )
			const int iJuneCreationTargetMeshIndex = ( pEffectData->m_iBoneTraceTargetMeshIndex < 0 ) ? 0 : pEffectData->m_iBoneTraceTargetMeshIndex;
			CKTDGXMeshPlayer::CXMeshInstanceHandle hJuneCreationTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
			const bool bJuneCreationTargetMesh = JuneGetCreationTargetMesh( pEffectSetInstance, pEffectData, hJuneCreationTargetMesh );

			// @ 321eee45-321eeeb7: GROUP_NAME, particle types only. The base is GROUP_NAME up to its first L"_GROUP"
			// (3347e55c; all of it when there is none). Each pass of the loop below creates one particle at the
			// bone <base>_GROUPCHILD_<n>, n = 1, 2, ..., until a bone is missing (@ 321eeec0-321ef0ce; next pass
			// @ 321f2550-321f257c; exit @ 321f2581). Such an effect creates nothing at BONE_NAME itself. Without
			// GROUP_NAME the loop runs once. The studio code below is the loop body and is not re-indented.
			bool bGroupName = false;
			std::wstring wstrGroupBase;
			if( ( ET_PARTICLE_UI_MAJOR == pEffectData->m_eEffectType || ET_PARTICLE_UI_MINOR == pEffectData->m_eEffectType ||
				  ET_PARTICLE_GAME_MAJOR == pEffectData->m_eEffectType || ET_PARTICLE_GAME_MINOR == pEffectData->m_eEffectType ) &&
				false == pEffectData->m_wstrGroupName.empty() )
			{
				wstrGroupBase = pEffectData->m_wstrGroupName.substr( 0, pEffectData->m_wstrGroupName.find( L"_GROUP" ) );
				bGroupName = true;
			}

			for( int iGroupChild = 1; ; ++iGroupChild )
			{
			std::wstring wstrGroupChildBoneName;
			D3DXVECTOR3 vGroupChildOffset( 0.f, 0.f, 0.f );
			D3DXVECTOR3 vEffectPos( 0.f, 0.f, 0.f );
			if( true == bGroupName )
			{
				if( false == JuneGroupChildPosition( pEffectSetInstance, pEffectData, wstrGroupBase, iGroupChild, iJuneCreationTargetMeshIndex,
													 bJuneCreationTargetMesh, wstrGroupChildBoneName, vEffectPos, vGroupChildOffset ) )
				{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					WCHAR wszJuneFx[64] = {0};
					StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"children=%d(321f2581)", iGroupChild - 1 );
					JuneFxEsCreate( pEffectSetInstance, pEffectData, L"GROUP_NAME", wszJuneFx );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
					break;
				}
			}
			else
			{
				// @ 321ef229-321ef260: GetEffectPosition (FUN_321dcd30)
				vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );
			}
			// @ 321f0d10-321f0d3b (likewise @ 321f1207, 321f1765, 321f1c2b): TRACE_MATRIX_IN_FIELD takes the child's bone
			const WCHAR* pwszJuneParticleBoneName = ( true == bGroupName ) ? wstrGroupChildBoneName.c_str() : pEffectData->m_PositionBoneName.c_str();
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			const D3DXVECTOR3 vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );
			const D3DXVECTOR3 vEffectRotation = pEffectSetInstance->GetEffectRotation( pEffectData );
			const D3DXVECTOR3 vEffectScale = pEffectSetInstance->GetEffectScale( pEffectData );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

			CX2DamageEffect::CEffect* pEffect = NULL;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
			CKTDGParticleSystem::CParticleEventSequence* pParticleSequence = NULL;

			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
				#ifdef SERV_PET_SYSTEM
					CX2GameUnit* pUnit = pEffectSetInstance->GetOwnerUnit();
					bool bPetDamageEffect = ( NULL != pEffectSetInstance->m_pPet ? true : false );
				#endif

					float fPowerRate = pEffectSetInstance->m_fPowerRate;
					if( fPowerRate < 0.f )
					{
#ifdef SERV_PET_SYSTEM
						if( NULL != pUnit )
						{
							if( bPetDamageEffect == true )
							{
								if( pUnit->GetRemainHyperModeTime() > 0.f )
									fPowerRate = 1.2f;
								else
									fPowerRate = 1.f;
							}
							else
							{
								fPowerRate = pUnit->GetPowerRate();
							}							
						}
						else
						{
							ASSERT( !"effectset owner invalid" );
							pUnit = NULL;
							pEffectSetInstance->m_optrGameUnit.Reset();
						}
#else
						if( null != pEffectSetInstance->m_optrGameUnit )
						{
							fPowerRate = pEffectSetInstance->m_optrGameUnit->GetPowerRate();
						}
						else
						{
							ASSERT( !"effectset owner invalid" );
							pEffectSetInstance->m_optrGameUnit.Reset();
						}
#endif
					}
					
					fPowerRate *= pEffectSetInstance->m_fPowerRateScale;

					if ( NULL != g_pX2Game )
					{
#ifdef SERV_PET_SYSTEM
						if( bPetDamageEffect == true )
						{
						float fPowerRatePet = 1.f;
						// 펫 친밀도가 70%이상일 경우 배율 증가
						if( pEffectSetInstance->m_pPet != NULL && pEffectSetInstance->m_pPet->GetNowIntimacy() >= 0.7f )
							fPowerRatePet = ( pEffectSetInstance->m_pPet->GetNowIntimacy() - 0.6f + 1.f );
						pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectName.c_str(), 
								fPowerRate * fPowerRatePet, vEffectPos, vEffectRotation, vEffectRotation );
						}
						else
						{
							if( true == pEffectSetInstance->m_bHyper &&
								false == pEffectData->m_EffectNameHyper.empty() )
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectNameHyper.c_str(), 
									fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
							}
							else
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectName.c_str(), 
									fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
							}
						}
#else
						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectNameHyper.c_str(), 
								fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
						}
						else
						{
							pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectName.c_str(), 
								fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
						}
#endif
					}

					if( NULL != pEffect )
					{
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pEffect->SetScale( vScaleAtTraceMatrix );
						}
						else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
							pEffect->SetScale( vEffectScale );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321ef68e-321ef6a0 -> FUN_321db1b0, tag 1 (@ 321db216-321db340):
						// APPLY_DAMAGE_EFFECT_VELOCITY changes the main effect's velocity (June mesh +0x534 =
						// m_vVelocity), added when the table's [4] is 0 and multiplied per component otherwise.
						// The same modifier is pushed onto the mesh (FUN_31c8a9b0), so its later velocity events
						// apply it again (CXMeshEvent_Velocity::OnFrameMove).
						if( true == pEffectData->m_bApplyDamageEffectVelocity )
						{
							CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = pEffect->GetMainEffect();
							if( NULL != pMainEffect )
							{
								const bool bAdd = pEffectData->m_bApplyDamageEffectVelocity4thIsZero;
								const D3DXVECTOR3& vValue = pEffectData->m_vApplyDamageEffectVelocity;
								D3DXVECTOR3 vVelocity = pMainEffect->GetVelocity();
								if( true == bAdd )
								{
									vVelocity += vValue;
								}
								else
								{
									vVelocity.x *= vValue.x;
									vVelocity.y *= vValue.y;
									vVelocity.z *= vValue.z;
								}
								pMainEffect->SetVelocity( vVelocity );
								pMainEffect->AddVelocityModifier( bAdd, vValue );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
								JuneFxEsCreate( pEffectSetInstance, pEffectData, L"APPLY_DAMAGE_EFFECT_VELOCITY", true == bAdd ? L"added(321db251)" : L"multiplied(321f2ac0)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
							}
						}

						// @ 321ef6b5-321ef6d5: USE_IS_RIGHT_WHEN_CREATE gives the damage effect's follow-line
						// facing (June CEffect +0x135 = m_bFollowLineRight) the facing the set was created with
						if( true == pEffectData->m_bUseIsRightWhenCreate )
						{
							pEffect->SetFollowLineRight( pEffectSetInstance->m_bIsRightWhenCreate );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							JuneFxEsCreate( pEffectSetInstance, pEffectData, L"USE_IS_RIGHT_WHEN_CREATE", true == pEffectSetInstance->m_bIsRightWhenCreate ? L"follow_line_right=1(321ef6cf)" : L"follow_line_right=0(321ef6cf)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
						}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef RIDING_SYSTEM
						//if ( NULL != pEffect->GetLockOnData() )
						{
							if ( NULL != pUnit && CX2GameUnit::GUT_USER == pUnit->GetGameUnitType() )
							{
								CX2GUUser* pUser = static_cast<CX2GUUser*>(pUnit);
#ifdef LOT_NEARST_UID_VECTOR_IN_RANGE_ADD //김창한
								pUser->SetLockOn( pEffect, 0, pEffect->GetLockOnData().m_LockOnType );
#else //LOT_NEARST_UID_VECTOR_IN_RANGE_ADD
								pUser->SetLockOn( pEffect );
#endif //LOT_NEARST_UID_VECTOR_IN_RANGE_ADD

#ifdef ADD_RENA_SYSTEM //김창한
								//스킬 관련 데이터 값, 첫번째 타격인지 체크하는 값을 상속받음.
								//관련 값들이 초기값이 아닐때만 상속 -> DamageEffect에서 CreateEffectSet이 된 경우가 해당됨.
								if( CX2DamageManager::FAC_NONE != pEffectSetInstance->m_eFirstAttack )
									pEffect->GetDamageData().m_eFirstAttack = pEffectSetInstance->m_eFirstAttack;

								if( 0 != pEffectSetInstance->m_RelateSkillData.m_byteRelateData )
									pEffect->GetDamageData().m_RelateSkillData = pEffectSetInstance->m_RelateSkillData;
#endif //ADD_RENA_SYSTEM

							}
						}
#endif //RIDING_SYSTEM
					}

				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
				{

					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos, 
							vEffectRotation, vEffectRotation );
					}
					else
					{
						pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos, 
							vEffectRotation, vEffectRotation );
					}

					if( NULL != pMeshInstance )
					{
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pMeshInstance->SetScale( vScaleAtTraceMatrix );
						}
						else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
							pMeshInstance->SetScale( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321ef8ee (UI major): TRACE_MATRIX_IN_FIELD
						JuneTraceMatrixInFieldMesh( pEffectSetInstance, pEffectData, pMeshInstance );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

						if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
#ifdef SERV_PET_SYSTEM
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
#endif
						}
					}

				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos, 
							vEffectRotation, vEffectRotation );
					}
					else
					{
						pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos, 
							vEffectRotation, vEffectRotation );
					}


					if( NULL != pMeshInstance )
					{
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pMeshInstance->SetScale( vScaleAtTraceMatrix );
						}
						else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
							pMeshInstance->SetScale( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321efe84 (UI minor): TRACE_MATRIX_IN_FIELD
						JuneTraceMatrixInFieldMesh( pEffectSetInstance, pEffectData, pMeshInstance );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

						if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
#ifdef SERV_PET_SYSTEM
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
#endif

						}
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos, 
								vEffectRotation, vEffectRotation );
						}
						else
						{
							pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos, 
								vEffectRotation, vEffectRotation );
						}

						if( NULL != pMeshInstance )
						{
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pMeshInstance->SetScale( vScaleAtTraceMatrix );
							}
							else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
								pMeshInstance->SetScale( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321ee600 @ 321f03b6 (game major): TRACE_MATRIX_IN_FIELD
							JuneTraceMatrixInFieldMesh( pEffectSetInstance, pEffectData, pMeshInstance );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

							if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
							{
								if( NULL != pOwnerGameUnit )
								{
									pMeshInstance->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}
				} break;

			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos, 
								vEffectRotation, vEffectRotation );
						}
						else
						{
							pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos, 
								vEffectRotation, vEffectRotation );
						}

						if( NULL != pMeshInstance )
						{
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pMeshInstance->SetScale( vScaleAtTraceMatrix );
							}
							else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
								pMeshInstance->SetScale( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321ee600 @ 321f088a (game minor): TRACE_MATRIX_IN_FIELD
							JuneTraceMatrixInFieldMesh( pEffectSetInstance, pEffectData, pMeshInstance );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

							if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
							{
								if( NULL != pOwnerGameUnit )
								{
									pMeshInstance->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}
				
				} break;

			case ET_PARTICLE_UI_MAJOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pParticleSequence = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
					}
					else
					{
						pParticleSequence = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos );
					}

					if( pParticleSequence != NULL )
					{
						if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
						{
							pParticleSequence->SetAxisAngle( vEffectRotation );
							pParticleSequence->SetAddRotate( vEffectRotation );
						}

#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
						}
						else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
							pParticleSequence->SetScaleFactor( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f0ce3 (UI major): TRACE_MATRIX_IN_FIELD
						JuneTraceMatrixInFieldParticle( pEffectSetInstance, pEffectData, pParticleSequence, pwszJuneParticleBoneName );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef FIX_ICE_HEATER_EVENT
						/// 파티클에 대하여 Set Over UI 기능 추가
						if ( true == pEffectData->m_bSetOverUI )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							JuneSetOverUI( pEffectSetInstance, pEffectData, pParticleSequence );
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							pParticleSequence->SetOverUI( true );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#endif // FIX_ICE_HEATER_EVENT

						if( pParticleSequence->GetUseLand() == true )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
#ifdef SERV_PET_SYSTEM
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
#endif
						}						
					}
				} break;

			case ET_PARTICLE_UI_MINOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pParticleSequence = g_pData->GetUIMinorParticle()->CreateSequence( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
					}
					else
					{
						pParticleSequence = g_pData->GetUIMinorParticle()->CreateSequence( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos );
					}


					if( pParticleSequence != NULL )
					{
						if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
						{
							pParticleSequence->SetAxisAngle( vEffectRotation );
							pParticleSequence->SetAddRotate( vEffectRotation );
						}
						
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
															       vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
						}
						else
							pParticleSequence->SetScaleFactor( vEffectScale );
#else //FIX_EFFECT_SCALE_BY_UNIT_SCALE
						//if( true == pEffectData->m_bApplyInstanceScale )
						//{
						//	pParticleSequence->SetScaleFactor( pEffectSetInstance->m_vScale );
						//}
						// 위에 스케일은 effectset에서 지정한 scale을 적용시키지 않는다. 
						// 구현이유는 모름.. 통일성있게 수정함.. 						
						pParticleSequence->SetScaleFactor( vEffectScale ); // modified by wonpok. 2010.03.02
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
						

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f11da (UI minor): TRACE_MATRIX_IN_FIELD
						JuneTraceMatrixInFieldParticle( pEffectSetInstance, pEffectData, pParticleSequence, pwszJuneParticleBoneName );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef FIX_ICE_HEATER_EVENT
						/// 파티클에 대하여 Set Over UI 기능 추가
						if ( true == pEffectData->m_bSetOverUI )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							JuneSetOverUI( pEffectSetInstance, pEffectData, pParticleSequence );
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							pParticleSequence->SetOverUI( true );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#endif // FIX_ICE_HEATER_EVENT

						if( pParticleSequence->GetUseLand() == true )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
#ifdef SERV_PET_SYSTEM
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
#endif
						}						
					}
				} break;

			case ET_PARTICLE_GAME_MAJOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pParticleSequence = g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
						}
						else
						{
							pParticleSequence = g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos );
						}

						if ( pParticleSequence != NULL )
						{
							if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
							{
								pParticleSequence->SetAxisAngle( vEffectRotation );
								pParticleSequence->SetAddRotate( vEffectRotation );
							}

#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
							}
							else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
								pParticleSequence->SetScaleFactor( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321ee600 @ 321f1738 (game major): TRACE_MATRIX_IN_FIELD
							JuneTraceMatrixInFieldParticle( pEffectSetInstance, pEffectData, pParticleSequence, pwszJuneParticleBoneName );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef FIX_ICE_HEATER_EVENT
							/// 파티클에 대하여 Set Over UI 기능 추가
							if ( true == pEffectData->m_bSetOverUI )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
								JuneSetOverUI( pEffectSetInstance, pEffectData, pParticleSequence );
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
								pParticleSequence->SetOverUI( true );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#endif // FIX_ICE_HEATER_EVENT

							if( pParticleSequence->GetUseLand() == true )
							{
								if( NULL != pOwnerGameUnit )
								{
									pParticleSequence->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}

				} break;

			case ET_PARTICLE_GAME_MINOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pParticleSequence = g_pX2Game->GetMinorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
						}
						else
						{
							pParticleSequence = g_pX2Game->GetMinorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos );
						}

						if( pParticleSequence != NULL )
						{
							if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
							{
								pParticleSequence->SetAxisAngle( vEffectRotation );
								pParticleSequence->SetAddRotate( vEffectRotation );
							}

#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x, 
									vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y, 
									vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
							}
							else
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
								pParticleSequence->SetScaleFactor( vEffectScale );

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321ee600 @ 321f1c12 (game minor): TRACE_MATRIX_IN_FIELD
							JuneTraceMatrixInFieldParticle( pEffectSetInstance, pEffectData, pParticleSequence, pwszJuneParticleBoneName );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef FIX_ICE_HEATER_EVENT
							/// 파티클에 대하여 Set Over UI 기능 추가
							if ( true == pEffectData->m_bSetOverUI )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
								JuneSetOverUI( pEffectSetInstance, pEffectData, pParticleSequence );
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
								pParticleSequence->SetOverUI( true );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#endif // FIX_ICE_HEATER_EVENT

							if( pParticleSequence->GetUseLand() == true )
							{
								if( NULL != pOwnerGameUnit )
								{
									pParticleSequence->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}					
				} break;
			}


#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321ee600 @ 321f1ec8-321f1ee4: the effect-alpha option. The effect's own value is used only
			// when it is 1; any other value takes the instance's (+0x1bc, see _CreateEffectSetInstance). It then
			// goes to the damage effect (+0xb32), the mesh (FUN_31c83620) or the particle (+0x1b0, @ 321f254a).
			// This build has no effect-alpha option, so a set using the key is logged only (plan rule 5).
			if( 0 != pEffectData->m_iApplyEffectAlphaConditionByOption )
			{
				WCHAR wszJuneScope[32] = {0};
				StringCchPrintfW( wszJuneScope, ARRAY_SIZE(wszJuneScope), L" EFFECT%d", (int) i );
				JuneFxUnsupported( pEffectSetInstance->m_pEffectSetData, L"APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION", wszJuneScope,
					L"effect-alpha option", pEffectData->m_iApplyEffectAlphaConditionByOption, L"321f1ec8" );
			}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
					if( NULL != pEffect )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        pEffectInstance->m_hDamageEffect		= pEffect->GetHandle();
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_pDamageEffect		= pEffect;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_hMeshPlayer			= INVALID_MESH_INSTANCE_HANDLE;
						pEffectInstance->m_hParticleSequence	= INVALID_PARTICLE_SEQUENCE_HANDLE;
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f1f76-321f20f4: every BONE_TRACE_TARGET_MESH damage
						// effect is pushed onto the target-mesh list as {main effect handle,
						// ET_MESH_PLAYER_GAME_MAJOR (June's tag 4), SCALE or (1,1,1)}
						// (@ 321f1f87-321f2034). Only the push that makes the count 1 sets
						// is-target-mesh and, with a valid unit, right / dir / Z
						// (@ 321f2039-321f20ee). Unlike March, June leaves the instance's bone-trace
						// scale alone here; only the entry holds it.
						if( true == pEffectData->m_bBoneTraceTargetMesh )
						{
							EffectSetInstance::TargetMeshData targetMeshData;
							targetMeshData.m_hTargetMesh = pEffect->GetMainEffectHandle();
							targetMeshData.m_eEffectType = ET_MESH_PLAYER_GAME_MAJOR;
							if( false == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								targetMeshData.m_vScale = pEffectData->m_vScale;
							if( true == pEffectSetInstance->m_vecTargetMeshData.empty() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectSetInstance->m_hTargetMesh )
							{
								// March-only preset (PlayEffectSetByMeshPlayer): it stays entry 0
								EffectSetInstance::TargetMeshData presetData;
								presetData.m_hTargetMesh	= pEffectSetInstance->m_hTargetMesh;
								presetData.m_eEffectType	= pEffectSetInstance->m_eTargetMeshEffectType;
								presetData.m_vScale			= pEffectSetInstance->m_vBoneTraceTargetMeshScale;
								pEffectSetInstance->m_vecTargetMeshData.push_back( presetData );
							}
							pEffectSetInstance->m_vecTargetMeshData.push_back( targetMeshData );

							if( 1 == pEffectSetInstance->m_vecTargetMeshData.size() )
							{
								pEffectSetInstance->m_hTargetMesh			= targetMeshData.m_hTargetMesh;	// entry 0 mirror
								pEffectSetInstance->m_eTargetMeshEffectType	= targetMeshData.m_eEffectType;
								pEffectSetInstance->m_bIsTargetMesh			= true;
								if( null != pEffectSetInstance->m_optrGameUnit )
								{
									if( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
										pEffectSetInstance->m_bIsTargetMeshRight = true;
									pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
									pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
								}
							}
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							WCHAR wszJuneFx[64] = {0};
							StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"registered_index=%d(damage)", (int) pEffectSetInstance->m_vecTargetMeshData.size() - 1 );
							JuneFxEsCreate( pEffectSetInstance, pEffectData, L"BONE_TRACE_TARGET_MESH", wszJuneFx );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
						}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
						if( true == pEffectData->m_bBoneTraceTargetMesh && 
							INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
						{
							pEffectSetInstance->m_hTargetMesh = pEffect->GetMainEffectHandle();
							pEffectSetInstance->m_eTargetMeshEffectType = ET_MESH_PLAYER_GAME_MAJOR;
							pEffectSetInstance->m_bIsTargetMesh = true;
		#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
							if ( null != pEffectSetInstance->m_optrGameUnit )
							{ 
								/// 유닛의 IsRight 값으로 타겟 매시의 방향을 설정
								if ( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
									pEffectSetInstance->m_bIsTargetMeshRight = true;

					#ifdef FIX_TARGET_MESH_OFFSET_POS // 김태환
								/// 유닛의 방향 벡터 값으로 타겟 매시의 방향 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
								/// 유닛의 Z 벡터 값으로 타겟 매시의 Z 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
					#endif // FIX_TARGET_MESH_OFFSET_POS
							}
		#endif // SERV_ARA_CHANGE_CLASS_SECOND

		#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 Trace일 때 BoneTraceTargetMesh의 스케일 값 빼주기 위하여, 스케일 값 저장
							if ( true == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = D3DXVECTOR3( 1.f, 1.f, 1.f );
							else
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = pEffectData->m_vScale;
		#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
						}
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
					}
				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
			case ET_MESH_PLAYER_UI_MINOR:
			case ET_MESH_PLAYER_GAME_MAJOR:
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( NULL != pMeshInstance )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        pEffectInstance->m_hDamageEffect		= INVALID_DAMAGE_EFFECT_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_pDamageEffect		= NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_hMeshPlayer			= pMeshInstance->GetHandle();
						pEffectInstance->m_hParticleSequence	= INVALID_PARTICLE_SEQUENCE_HANDLE;
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f2130-321f2166: the EffectInstance takes its own copy of
						// MESH_TRIGGER_DATA_LIST (June EffectInstance +0x4c), for Phase 4b's UpdateEffect. A
						// non-empty list also clears mesh +0x194 (set to 1 in the init list of its ctor FUN_31c8d140
						// @ 31c8d18e, before March's first member m_Handle at June +0x198). Phase 4b found its
						// reader, the mesh's OnFrameMove (31c87b80 @ 31c87d0e): the anim a trigger replaced is
						// restored only while it is 1. It is March's m_bRestoreAnimAfterTrigger.
						if( false == pEffectData->m_vecMeshTriggerData.empty() )
						{
							pEffectInstance->m_vecMeshTriggerData = pEffectData->m_vecMeshTriggerData;
							pMeshInstance->SetRestoreAnimAfterTrigger( false );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							WCHAR wszJuneFx[64] = {0};
							StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"copied=%d restore_after_trigger=0(321f2166)", (int) pEffectInstance->m_vecMeshTriggerData.size() );
							JuneFxEsCreate( pEffectSetInstance, pEffectData, L"MESH_TRIGGER_DATA_LIST", wszJuneFx );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
						}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

						if ( pEffectData->m_bUseSubAttackListSet && null != pEffectSetInstance->m_optrGameUnit )
							pEffectSetInstance->m_optrGameUnit->InsertPairSubAttackListSet( pEffectInstance->m_hMeshPlayer, pMeshInstance->GetXSkinAnim() );
					
#ifdef _IN_HOUSE_
						//{{AFX
						if( true == pEffectInstance->m_pEffectData->m_bApplyUnitRotation &&
							CKTDGMatrix::BT_NONE != pMeshInstance->GetTempletData()->billBoardType )
						{
							WCHAR wszMsg[1024] = L"";
							StringCchPrintfW( wszMsg, ARRAY_SIZE(wszMsg), L"%s, %s, %s, %d", 
								pEffectSetInstance->m_pEffectSetData->m_wstrEffectSetName.c_str(), 
								pEffectInstance->m_pEffectData->m_EffectName.c_str(),
								pMeshInstance->GetTempletData()->templetName.c_str(),
								(int) pMeshInstance->GetTempletData()->billBoardType );
							StateLog( wszMsg );

						}
						//}}AFX
#endif _IN_HOUSE_
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f21f7-321f2414: every BONE_TRACE_TARGET_MESH mesh
						// player is pushed onto the target-mesh list as {handle, its EFFECT_TYPE,
						// SCALE or (1,1,1)} (@ 321f2208-321f22b3). is-target-mesh is set when the
						// count is 1 (@ 321f22b8-321f22da). Unlike the damage-effect case, right (set,
						// never cleared) / dir / Z follow every push with a valid unit
						// (@ 321f22e0-321f2394), and so does the instance's bone-trace scale
						// (@ 321f239a-321f2414).
						if( true == pEffectData->m_bBoneTraceTargetMesh )
						{
							EffectSetInstance::TargetMeshData targetMeshData;
							targetMeshData.m_hTargetMesh = pMeshInstance->GetHandle();
							targetMeshData.m_eEffectType = pEffectData->m_eEffectType;
							if( false == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								targetMeshData.m_vScale = pEffectData->m_vScale;
							if( true == pEffectSetInstance->m_vecTargetMeshData.empty() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectSetInstance->m_hTargetMesh )
							{
								// March-only preset (PlayEffectSetByMeshPlayer): it stays entry 0
								EffectSetInstance::TargetMeshData presetData;
								presetData.m_hTargetMesh	= pEffectSetInstance->m_hTargetMesh;
								presetData.m_eEffectType	= pEffectSetInstance->m_eTargetMeshEffectType;
								presetData.m_vScale			= pEffectSetInstance->m_vBoneTraceTargetMeshScale;
								pEffectSetInstance->m_vecTargetMeshData.push_back( presetData );
							}
							pEffectSetInstance->m_vecTargetMeshData.push_back( targetMeshData );

							if( 1 == pEffectSetInstance->m_vecTargetMeshData.size() )
							{
								pEffectSetInstance->m_hTargetMesh			= targetMeshData.m_hTargetMesh;	// entry 0 mirror
								pEffectSetInstance->m_eTargetMeshEffectType	= targetMeshData.m_eEffectType;
								pEffectSetInstance->m_bIsTargetMesh			= true;
							}
							if( null != pEffectSetInstance->m_optrGameUnit )
							{
								if( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
									pEffectSetInstance->m_bIsTargetMeshRight = true;
								pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
								pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
							}
							pEffectSetInstance->m_vBoneTraceTargetMeshScale = targetMeshData.m_vScale;
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
							WCHAR wszJuneFx[64] = {0};
							StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"registered_index=%d(mesh)", (int) pEffectSetInstance->m_vecTargetMeshData.size() - 1 );
							JuneFxEsCreate( pEffectSetInstance, pEffectData, L"BONE_TRACE_TARGET_MESH", wszJuneFx );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
						}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
						if( true == pEffectData->m_bBoneTraceTargetMesh && 
							INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
						{
							pEffectSetInstance->m_hTargetMesh = pMeshInstance->GetHandle();
							pEffectSetInstance->m_eTargetMeshEffectType = pEffectData->m_eEffectType;
							pEffectSetInstance->m_bIsTargetMesh = true;
		#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
							/// 유닛의 IsRight 값으로 타겟 매시의 방향을 설정
							if ( null != pEffectSetInstance->m_optrGameUnit )
							{
								if ( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
									pEffectSetInstance->m_bIsTargetMeshRight = true;

				#ifdef FIX_TARGET_MESH_OFFSET_POS // 김태환
								/// 유닛의 방향 벡터 값으로 타겟 매시의 방향 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
								/// 유닛의 Z 벡터 값으로 타겟 매시의 Z 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
				#endif // FIX_TARGET_MESH_OFFSET_POS
							}
		#endif // SERV_ARA_CHANGE_CLASS_SECOND

		#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
							/// 행렬 Trace일 때 BoneTraceTargetMesh의 스케일 값 빼주기 위하여, 스케일 값 저장
							if ( true == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = D3DXVECTOR3( 1.f, 1.f, 1.f );
							else
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = pEffectData->m_vScale;
		#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
						}
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME


					}
				} break;

			case ET_PARTICLE_UI_MAJOR:
			case ET_PARTICLE_UI_MINOR:
			case ET_PARTICLE_GAME_MAJOR:
			case ET_PARTICLE_GAME_MINOR:
				{
					if( NULL != pParticleSequence )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        pEffectInstance->m_hDamageEffect		= INVALID_DAMAGE_EFFECT_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_pDamageEffect		= NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_hMeshPlayer			= INVALID_MESH_INSTANCE_HANDLE;
						pEffectInstance->m_hParticleSequence	= pParticleSequence->GetHandle();
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f2483-321f24f1: a GROUP_NAME child keeps the flag (EffectInstance +0x2c),
						// its bone (+0x30) and, with BONE_NAME, its offset in the target mesh's BONE_NAME frame (+0x20)
						if( true == bGroupName )
						{
							pEffectInstance->m_bGroupChild = true;
							pEffectInstance->m_wstrGroupChildBoneName = wstrGroupChildBoneName;
							if( false == pEffectData->m_PositionBoneName.empty() )
								pEffectInstance->m_vGroupChildOffset = vGroupChildOffset;
						}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						// June2017 FUN_321ee600 @ 321f24f4-321f251d: the new particle takes the set's show flag (+0xdc)
						JuneSetEffectInstanceShow( pEffectInstance, pEffectSetInstance->m_bShowObject );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
					}
				} break;
			}
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321ee600 @ 321f2550-321f257c: the next GROUP_NAME child; without GROUP_NAME the effect is
			// done after one pass (@ 321f25a7)
			if( false == bGroupName )
				break;
			}	// for( iGroupChild )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		}
	}
}

void CX2EffectSet::OnFrameMove_DeleteEffect()
{
	KTDXPROFILE();

	// 이펙트 삭제
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	for( UINT j=0; j<m_vecpEffectSetInstance.size(); j++ )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterNext = iter;    ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
        {
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            continue;
        }
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        EffectSetInstance* pEffectSetInstance = m_vecpEffectSetInstance[j];
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		
#ifdef MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 FUN_321e4020 @ 321e4140-321e419d: a set that traces target meshes is deleted only when no
		// entry holds a handle any more (HasTargetMeshHandle). March tested m_hTargetMesh, entry 0's mirror
		// alone, so a set whose first target mesh was gone while a later one lived was deleted early.
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		if( true == pEffectSetInstance->m_bIsTargetMesh &&
			INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh &&
			true == pEffectSetInstance->HasTargetMeshHandle() )
			JuneFxEsRuntime( L"ES_DELETE", pEffectSetInstance, NULL, L"BONE_TRACE_TARGET_MESH", L"kept=entry0_gone_another_holds_a_handle(321e4180)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		if( true == pEffectSetInstance->m_bIsTargetMesh &&
			false == pEffectSetInstance->HasTargetMeshHandle() )
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		if( true == pEffectSetInstance->m_bIsTargetMesh &&
			INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			JuneFxEsRuntime( L"ES_DELETE", pEffectSetInstance, NULL, L"BONE_TRACE_TARGET_MESH", L"deleted=no_target_mesh_handle_left(321e4197)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			--j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}
#endif //MODIFY_EFFECT_SET_TRACE_BONE

#ifdef TRACE_UNIT_DIE_DELETE_EFFECTSET
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 FUN_321e4020 @ 321e41a2-321e4231: the templet's DELETE_EFFECTSET_AT_TRACE_UNIT_DIE
		// (EffectSetData +0x58) deletes the set exactly as the two instance flags March already has
		// (+0xd0 / +0xd1) do: when the trace target is gone or its HP (+0xc04 -> +0x20, decoded) is not
		// above 0.
		if( ( true == pEffectSetInstance->m_pEffectSetData->m_bDeleteEffectSetAtTraceUnitDie ||
			  ( true == pEffectSetInstance->m_bCreateEffectSetTraceUnit &&
				true == pEffectSetInstance->m_bTraceUnitDieDeleteEffectSet ) ) &&
			( null == pEffectSetInstance->m_optrTraceTargetGameUnit ||
			0 >= pEffectSetInstance->m_optrTraceTargetGameUnit->GetNowHp() ))
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		if( true == pEffectSetInstance->m_bCreateEffectSetTraceUnit &&
			true == pEffectSetInstance->m_bTraceUnitDieDeleteEffectSet &&
			( null == pEffectSetInstance->m_optrTraceTargetGameUnit || 
			0 >= pEffectSetInstance->m_optrTraceTargetGameUnit->GetNowHp() ))
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			if( true == pEffectSetInstance->m_pEffectSetData->m_bDeleteEffectSetAtTraceUnitDie )
				JuneFxEsRuntime( L"ES_DELETE", pEffectSetInstance, NULL, L"DELETE_EFFECTSET_AT_TRACE_UNIT_DIE", L"deleted=trace_unit_dead(321e4223)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			--j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}
#endif //TRACE_UNIT_DIE_DELETE_EFFECTSET

		// game에 없는 user의 포인터를 가지고 있다면 지운다
		if( ESM_GAME == pEffectSetInstance->m_eMode )
		{
			if( NULL != g_pX2Game && null == pEffectSetInstance->m_optrGameUnit )
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			    --j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}

#ifdef SERV_PET_SYSTEM
		if( pEffectSetInstance->m_pPet != NULL )
		{
			if( g_pData->GetPetManager()->IsValidPet(pEffectSetInstance->m_pPet) == false )
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			    --j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}
#endif

		if( pEffectSetInstance->m_pEffectSetData->m_iDeleteShakeCount > 0 )
		{
			if( pEffectSetInstance->m_iCurrDeleteShakeCount >= pEffectSetInstance->m_pEffectSetData->m_iDeleteShakeCount )
			{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				// June2017 FUN_321e4020 @ 321e42ec-321e432c: the same limit (templet + instance +0xcc, which
				// nothing in March sets)
				JuneFxEsRuntime( L"ES_DELETE", pEffectSetInstance, NULL, L"DELETE_SHAKE_COUNT", L"deleted=shake_limit(321e431e)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			    --j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
			else
			{
				if( null != pEffectSetInstance->m_optrTraceTargetGameUnit && 
					NULL != g_pX2Game )
				{

					switch( pEffectSetInstance->m_optrTraceTargetGameUnit->GetGameUnitType() )
					{
					case CX2GameUnit::GUT_USER:
						{
							CX2GUUser* pCX2GUUser = static_cast<CX2GUUser*>( pEffectSetInstance->m_optrTraceTargetGameUnit.GetObservable() );
							if( pCX2GUUser != NULL )
							{
								if( pCX2GUUser->GetIsRight() != pCX2GUUser->GetIsRightBefore() )
								{
									++pEffectSetInstance->m_iCurrDeleteShakeCount;
								}
							}

						} break;

					case CX2GameUnit::GUT_NPC:
						{

							//CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( (int)m_HitterUID );
							//if( pNPC != NULL )
							//{
							//	if( pNPC->GetIsRight() != pNPC->GetIsRightBefore() )
							//	{
							//		m_iFollowHitter--;
							//	}
							//}

						} break;
					}
				}
			}
		}

		if( pEffectSetInstance->m_fLifeTime > 0.f )
		{
			if( pEffectSetInstance->m_fElapsedTime > pEffectSetInstance->m_fLifeTime )
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			    --j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}

		bool bEffectSetInstanceAlive = false;
		for( UINT i=0; i<pEffectSetInstance->m_vecEffectInstanceCreated.size(); i++ )
		{
			if( false == pEffectSetInstance->m_vecEffectInstanceCreated[i] )
			{
				bEffectSetInstanceAlive = true;
				break;
			}
		}

		for( UINT i=0; i<pEffectSetInstance->m_vecpEffectInstance.size(); i++ )
		{
			EffectInstance* pEffectInstance = pEffectSetInstance->m_vecpEffectInstance[i];
			if( false == pEffectInstance->m_bAlive )
				continue;

			EffectData* pEffectData = pEffectInstance->m_pEffectData;
			switch( pEffectData->m_eEffectType )
			{
			default:
				{
					ASSERT( !"no type effect" );
					pEffectInstance->m_bAlive = false;
				} break;
			case ET_DAMAGE_EFFECT:
				{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					if( NULL != g_pX2Game && false == g_pX2Game->GetDamageEffect()->IsLiveInstanceHandle( pEffectInstance->m_hDamageEffect ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					if( NULL != g_pX2Game && false == g_pX2Game->GetDamageEffect()->IsLiveInstance( pEffectInstance->m_pDamageEffect ) )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					{
						pEffectInstance->m_bAlive = false;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        pEffectInstance->m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						pEffectInstance->m_pDamageEffect = NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					}
				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
				{
					if( false == g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( false == g_pData->GetUIMinorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMinorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_UI_MAJOR:
				{
					if( false == g_pData->GetUIMajorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_UI_MINOR:
				{
					if( false == g_pData->GetUIMinorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_GAME_MAJOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMajorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_GAME_MINOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMinorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			}

			if( true == pEffectInstance->m_bAlive &&
				false == pEffectData->m_bPassiveEffect )
			{
				bEffectSetInstanceAlive = true;
			}
		}


		if( false == bEffectSetInstanceAlive )
		{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + j );
			--j;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}
	}
}



void CX2EffectSet::OnFrameMove_UpdateEffect()
{

	KTDXPROFILE();

	// 이펙트 위치 갱신	
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	BOOST_TEST_FOREACH( EffectSetInstance*, pEffectSetInstance, m_vecpEffectSetInstance )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 FUN_321e0080 @ 321e0105-321e0140, once per set, before the set's effects: a pending toggle
		// (a trigger's EFFECT_SET_TOGGLE_VISIBILITY_TIMER) counts down by the frame time and flips the set's
		// show flag at 0. The frame time is the instance's own tick of this frame (OnFrameMove_CreateEffect
		// ticks every live instance first), the value June is passed.
		if( true == pEffectSetInstance->m_bToggleVisibilityPending )
		{
			pEffectSetInstance->m_fToggleVisibilityTimer -= ( pEffectSetInstance->m_fElapsedTime - pEffectSetInstance->m_fPrevElapsedTime );
			if( pEffectSetInstance->m_fToggleVisibilityTimer <= 0.f )
			{
				pEffectSetInstance->SetShowObject( false == pEffectSetInstance->m_bShowObject );
				pEffectSetInstance->m_bToggleVisibilityPending = false;
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, NULL, L"EFFECT_SET_TOGGLE_VISIBILITY_TIMER",
					( true == pEffectSetInstance->m_bShowObject ) ? L"show=1(321e013b)" : L"show=0(321e013b)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			}
		}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		for( UINT i=0; i<pEffectSetInstance->m_vecpEffectInstance.size(); i++ )
		{
			EffectInstance* pEffectInstance = pEffectSetInstance->m_vecpEffectInstance[i];
			EffectData* pEffectData = pEffectInstance->m_pEffectData;
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 @ 321e0170-321e01e9: the target-mesh entry this effect uses, under 4a's
			// index rule (max( BONE_TRACE_TARGET_MESH_INDEX, 0 ); none for a BONE_TRACE_TARGET_MESH with a
			// negative index). REVERSE_Y reads it too (@ 321e1547).
			CKTDGXMeshPlayer::CXMeshInstanceHandle hJuneTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
			const bool bJuneTargetMeshLive = JuneGetCreationTargetMesh( pEffectSetInstance, pEffectData, hJuneTargetMesh );

			// June2017 FUN_321e0080 @ 321e01e9-321e021c, before the trace test and whether or not the effect
			// is alive: in a set that traces target meshes, an effect whose entry is gone is hidden
			// (EffectInstance::SetShowObject( false ), FUN_321de0c0) and skipped. TRACE_USER_TARGET_FORCE
			// and a BONE_TRACE_TARGET_MESH with a negative index are exempt. March instead broke out of the
			// set's loop after moving the first such traced effect (the #else after the position below).
			if( true == pEffectSetInstance->m_bIsTargetMesh &&
				false == pEffectData->m_bTraceUserTargetForce &&
				false == bJuneTargetMeshLive &&
				( false == pEffectData->m_bBoneTraceTargetMesh || pEffectData->m_iBoneTraceTargetMeshIndex >= 0 ) )
			{
				JuneSetEffectInstanceShow( pEffectInstance, false );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"BONE_TRACE_TARGET_MESH", L"hidden=target_mesh_gone(321e0217)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				continue;
			}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 @ 321e02fc-321e0409 (cases 2-5): a mesh effect runs its triggers
			// (FUN_321dfc40). The same cases also push the set's alpha (+0x6c) into the mesh; that is the
			// effect-alpha system's, absent (see the plan).
			{
				CKTDGXMeshPlayer* pTriggerMeshPlayer = NULL;
				switch( pEffectData->m_eEffectType )
				{
				case ET_MESH_PLAYER_UI_MAJOR:	pTriggerMeshPlayer = g_pData->GetUIMajorXMeshPlayer(); break;
				case ET_MESH_PLAYER_UI_MINOR:	pTriggerMeshPlayer = g_pData->GetUIMinorXMeshPlayer(); break;
				case ET_MESH_PLAYER_GAME_MAJOR:	pTriggerMeshPlayer = ( NULL != g_pX2Game ) ? g_pX2Game->GetMajorXMeshPlayer() : NULL; break;
				case ET_MESH_PLAYER_GAME_MINOR:	pTriggerMeshPlayer = ( NULL != g_pX2Game ) ? g_pX2Game->GetMinorXMeshPlayer() : NULL; break;
				default: break;
				}
				if( NULL != pTriggerMeshPlayer && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
				{
					if( CKTDGXMeshPlayer::CXMeshInstance* pTriggerMesh = pTriggerMeshPlayer->GetMeshInstance( pEffectInstance->m_hMeshPlayer ) )
						JuneRunMeshTriggers( this, pTriggerMesh, pEffectData, pEffectInstance, pEffectSetInstance );
				}
			}

			// June2017 FUN_321e0080 @ 321e0777-321e07e9, per effect: SYNC_SHOW_BY_CREATE_UNIT (getter
			// FUN_321f2b00) keeps the set's show flag equal to the owner's GetShowObject() (+0x172 with the
			// manager chain, +0xc); a change goes to every effect (SetShowObject, FUN_321de4f0)
			if( true == pEffectSetInstance->m_pEffectSetData->m_bSyncShowByCreateUnit && null != pEffectSetInstance->m_optrGameUnit )
			{
				const bool bOwnerShow = pEffectSetInstance->m_optrGameUnit->GetShowObject();
				if( pEffectSetInstance->m_bShowObject != bOwnerShow )
				{
					pEffectSetInstance->SetShowObject( bOwnerShow );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"SYNC_SHOW_BY_CREATE_UNIT",
						( true == bOwnerShow ) ? L"show=1(321e07e9)" : L"show=0(321e07e9)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
			}

			// June2017 FUN_321e0080 @ 321e07ee-321e07f7: SOUND%d (FUN_321dee80), once per effect instance
			JuneOnFrameMoveSound( pEffectSetInstance );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

			if( false == pEffectData->m_bTrace 
				&& false == pEffectData->m_bTraceMore 
				//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
				&& false == pEffectData->m_bTraceMatrix
				//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
				)
				continue;

			if( false == pEffectInstance->m_bAlive )
				continue;


#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 @ 321e0825-321e0986: a GROUP_NAME child particle (4a's EffectInstance
			// members) follows its own bone. With BONE_NAME, the offset kept at creation is taken through
			// GetCombineMatrix (FUN_321dc550 @ 321e083e) with D3DXVec3Transform, then divided by w
			// (@ 321e0855-321e0914); without it, GetEffectPositionByBoneName( child bone ) (FUN_321dd590
			// @ 321e0933). Every other effect: GetEffectPosition (@ 321e0964), as March.
			D3DXVECTOR3 vEffectPos( 0.f, 0.f, 0.f );
			bool bJuneMoveEffect = true;
			if( true == pEffectInstance->m_bGroupChild )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					const D3DXMATRIX* pGroupChildMatrix = pEffectSetInstance->GetCombineMatrix( pEffectData );
					if( NULL != pGroupChildMatrix )
					{
						D3DXVECTOR4 vGroupChild( 0.f, 0.f, 0.f, 1.f );
						D3DXVec3Transform( &vGroupChild, &pEffectInstance->m_vGroupChildOffset, pGroupChildMatrix );
						const float fInvW = 1.f / vGroupChild.w;
						vEffectPos = D3DXVECTOR3( vGroupChild.x * fInvW, vGroupChild.y * fInvW, vGroupChild.z * fInvW );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
						JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"GROUP_NAME", L"moved=child_offset_through_matrix(321e085a)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
					}
					else
					{
						// @ 321e0845: with no matrix June jumps past the computation and still moves the
						// particle, to whatever its stack slot held. Not reproduced: the particle stays put.
						bJuneMoveEffect = false;
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
						JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"GROUP_NAME", L"not_moved=no_matrix(321e0845)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
					}
				}
				else
				{
					bool bJuneBoneFound = false;
					vEffectPos = pEffectSetInstance->GetEffectPositionByBoneName( pEffectData, pEffectInstance->m_wstrGroupChildBoneName, bJuneBoneFound );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"GROUP_NAME", L"moved=child_bone(321e0933)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
			}
			else
			{
				vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );
			}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			D3DXVECTOR3 vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 has no break here: its target-mesh test is per effect, at the top of the
			// loop (@ 321e01e9-321e021c).
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef MODIFY_EFFECT_SET_TRACE_BONE

			// 타겟 메시가 사라지면 더이상 위치 갱신 하지 않기
	#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
			if( true == pEffectSetInstance->m_bIsTargetMesh &&
				false == pEffectData->m_bTraceUserTargetForce &&
				INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
	#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			if( true == pEffectSetInstance->m_bIsTargetMesh &&
				INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
	#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			
				break;

#endif // MODIFY_EFFECT_SET_TRACE_BONE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			if( true == bJuneMoveEffect )	// June2017 FUN_321e0080 @ 321e0845: a group child with no matrix is not moved
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
					if ( NULL != g_pX2Game &&
						NULL != g_pX2Game->GetDamageEffect() )
                    {
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                        if ( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						if ( CX2DamageEffect::CEffect* pEffect = pEffectInstance->m_pDamageEffect )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					    {
						    pEffect->SetPos( vEffectPos );
					    }
                    }

				} break;

			case ET_MESH_PLAYER_UI_MAJOR:				
				{
					if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
						INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( NULL != pInstance )
						{
							pInstance->SetPos( vEffectPos );
						}
					}
				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
						INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( NULL != pInstance )
						{
							pInstance->SetPos( vEffectPos );
						}
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( pInstance != NULL )
						{
							pInstance->SetPos(vEffectPos);
						}
					}
				}
				break;
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( pInstance != NULL )
						{
							pInstance->SetPos(vEffectPos);
						}
					}
				} break;
			case ET_PARTICLE_UI_MAJOR:
				{
					if( g_pData->GetUIMajorParticle() != NULL && 
						pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321e0080 @ 321e0ccf-321e0d14 (case 6): a particle that uses the land (m_bUseLand, June +0x31c)
							// takes the owner's land height every frame (GetLandPos().y, vfunc 0x134, the call June's
							// creation makes @ 321f0fa6-321f0fbf where March calls SetLandPosition)
							if( true == pSeq->GetUseLand() && null != pEffectSetInstance->m_optrGameUnit )
								pSeq->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							// June2017 FUN_321e0080 @ 321e0d1a-321e0f89: case 6 alone refreshes TRACE_MATRIX_IN_FIELD
							JuneTraceMatrixInFieldParticleUpdate( pEffectSetInstance, pEffectInstance, pEffectData, pSeq, bJuneTargetMeshLive, hJuneTargetMesh, vEffectPos );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						}
					}
				} break;

			case ET_PARTICLE_UI_MINOR:
				{
					if( g_pData->GetUIMinorParticle() != NULL && 
						pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321e0080 @ 321e1032-321e107f (case 7): a particle that uses the land (m_bUseLand, June +0x31c)
							// takes the owner's land height every frame (GetLandPos().y, vfunc 0x134, the call June's
							// creation makes @ 321f0fa6-321f0fbf where March calls SetLandPosition)
							if( true == pSeq->GetUseLand() && null != pEffectSetInstance->m_optrGameUnit )
								pSeq->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						}
					}
				} break;

			case ET_PARTICLE_GAME_MAJOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321e0080 @ 321e1127-321e1157 (case 8, then the case-7 land code): a particle that uses the land (m_bUseLand, June +0x31c)
							// takes the owner's land height every frame (GetLandPos().y, vfunc 0x134, the call June's
							// creation makes @ 321f0fa6-321f0fbf where March calls SetLandPosition)
							if( true == pSeq->GetUseLand() && null != pEffectSetInstance->m_optrGameUnit )
								pSeq->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						}
					}
				}
				break;

			case ET_PARTICLE_GAME_MINOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMinorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
							// June2017 FUN_321e0080 @ 321e1127-321e1157 (case 9, joined with case 8): a particle that uses the land (m_bUseLand, June +0x31c)
							// takes the owner's land height every frame (GetLandPos().y, vfunc 0x134, the call June's
							// creation makes @ 321f0fa6-321f0fbf where March calls SetLandPosition)
							if( true == pSeq->GetUseLand() && null != pEffectSetInstance->m_optrGameUnit )
								pSeq->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
						}
					}						
				}
				break;
			}			

			//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 @ 321e0fa4-321e0fc5: skipped while the instance's stop-tracing flag is set
			// (STOP_TRACING_ON_TRIGGER, stage 3). The matrix is June's GetCombineMatrix (FUN_321dc550).
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			if( true == pEffectData->m_bTraceMatrix && true == pEffectSetInstance->m_bStopTracingOnTrigger )
				JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"TRACE_MATRIX", L"skipped=stop_tracing_on_trigger(321e0fb5)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			if ( true == pEffectData->m_bTraceMatrix && false == pEffectSetInstance->m_bStopTracingOnTrigger )
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			if ( true == pEffectData->m_bTraceMatrix )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			{
				const D3DXMATRIX* pCombineMatrix = pEffectSetInstance->GetCombineMatrix( pEffectData );			

				if ( NULL != pCombineMatrix )
				{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					// June2017 FUN_321dc550 @ 321dc78e-321dc81d: holders March had no matrix for
					if( ESM_GAME != pEffectSetInstance->m_eMode || null == pEffectSetInstance->m_optrGameUnit )
						JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"TRACE_MATRIX",
							( pEffectSetInstance->m_pSquareUnit != NULL ) ? L"matrix=square_unit_viewer(321dc7cf)" : L"matrix=pet(321dc81b)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
					switch( pEffectData->m_eEffectType )
					{
					case ET_DAMAGE_EFFECT:
						{
							if( NULL != g_pX2Game &&
								NULL != g_pX2Game->GetDamageEffect() )
                            {
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                                if ( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect ) )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						        if ( CX2DamageEffect::CEffect* pEffect = pEffectInstance->m_pDamageEffect )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
							    {
								    CKTDGXMeshPlayer::CXMeshInstance* pInstance = pEffect->GetMainEffect();
								    if( NULL != pInstance )
								    {
									    pInstance->SetUseDXMatrix( *pCombineMatrix );						
								    }
							    }
                            }
						}
						break;

					case ET_MESH_PLAYER_UI_MAJOR:				
						{
							if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( NULL != pInstance )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );						
								}
							}
						} break;
					case ET_MESH_PLAYER_UI_MINOR:
						{
							if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( NULL != pInstance )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );						
								}
							}
						} break;
					case ET_MESH_PLAYER_GAME_MAJOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( pInstance != NULL )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );						
								}
							}
						}
						break;
					case ET_MESH_PLAYER_GAME_MINOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( pInstance != NULL )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );						
								}
							}
						} break;
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
					// June2017 FUN_321e0080 @ 321e1425-321e146b (case 6): the UI-major particle takes the matrix
					// too; March moved only the game-major one, under NEW_HENIR_TEST
					case ET_PARTICLE_UI_MAJOR:
						{
							if( NULL != g_pData && NULL != g_pData->GetUIMajorParticle() && INVALID_PARTICLE_SEQUENCE_HANDLE != pEffectInstance->m_hParticleSequence )
							{
								CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
								if( NULL != pSeq )
								{
									pSeq->SetUseDXMatrix( *pCombineMatrix );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
									JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"TRACE_MATRIX", L"particle_ui_major(321e145f)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
								}
							}
						} break;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef NEW_HENIR_TEST
					case ET_PARTICLE_GAME_MAJOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
							{
								CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
								if( pSeq != NULL )
								{
									pSeq->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						} break;
#endif NEW_HENIR_TEST

					default:
						break;
					}
				} // if
			} // if
			//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

#ifdef ARA_CHANGE_CLASS_FIRST
			if ( true == pEffectData->m_bReverseY )
			{
				switch( pEffectData->m_eEffectType )
				{
				case ET_PARTICLE_GAME_MAJOR:
					{
						if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );

				#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
							if( pSeq != NULL )
							{
								CKTDGMatrix::BILLBOARD_TYPE eBillboardType = pSeq->GetBillBoardType();

								if ( CKTDGMatrix::BT_NONE != eBillboardType )
								{
									/// 타겟 매시가 설정되어 있다면, 타겟 매시의 IsRight 값을 사용한다.
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
									// June2017 FUN_321e0080 @ 321e1547-321e1562: the target mesh's facing (+0x101) when the
									// index rule gives this effect a live entry and it is not TRACE_USER_TARGET_FORCE
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
									JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"REVERSE_Y",
										( true == bJuneTargetMeshLive && false == pEffectData->m_bTraceUserTargetForce ) ? L"facing=target_mesh(321e155b)" : L"facing=unit(321e1583)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
									if ( true == bJuneTargetMeshLive &&
										 false == pEffectData->m_bTraceUserTargetForce )
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
					#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
									if ( INVALID_MESH_INSTANCE_HANDLE != pEffectSetInstance->m_hTargetMesh &&
										 false == pEffectData->m_bTraceUserTargetForce )
					#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
									if ( INVALID_MESH_INSTANCE_HANDLE != pEffectSetInstance->m_hTargetMesh )
					#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
										pSeq->SetReverseY( !( pEffectSetInstance->m_bIsTargetMeshRight ) );
									/// 타겟 매시가 설정되어 있지 않다면, 유저의 IsRight 값을 사용한다.
									else if ( null != pEffectInstance->m_optrGameUnit )
										pSeq->SetReverseY( !( pEffectInstance->m_optrGameUnit->GetIsRight() ) );
								}
							}
				#else // SERV_ARA_CHANGE_CLASS_SECOND
							if( pSeq != NULL && null != pEffectInstance->m_optrGameUnit && false == pEffectInstance->m_optrGameUnit->GetIsRight() )
							{
								CKTDGMatrix::BILLBOARD_TYPE eBillboardType = pSeq->GetBillBoardType();
								
								if ( CKTDGMatrix::BT_NONE != eBillboardType )
								{
									pSeq->SetReverseY( true );
								}
							}
				#endif // SERV_ARA_CHANGE_CLASS_SECOND
						}
					} break;
				}
			}
#endif
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e0080 @ 321e159a-321e1b05, still inside the trace block: FOLLOW_UNIT_ROTATION
			// (EffectData +0x104) turns the effect with its unit (GetUnitRotation, FUN_321ddea0 @ 321e15b5).
			// The damage main effect and the meshes take it as their rotation (FUN_31c1a3a0 = SetRotateDegree:
			// June's rotate event 31c84e10 writes +0x5a8 / +0x5c0 as March's writes m_vRotateLocal /
			// m_vRotateFinal, so +0x5b4 is m_vRotate). Particles 6 and 8 add it to their m_AddRotateRel and set
			// it as m_AddRotate (FUN_31c1a030 = SetAddRotate, the call June's creation makes where March calls
			// SetAddRotate; +0x3e0 / +0x3ec = probe 0x2b4 / 0x2c0); FOLLOW_UNIT_ROTATION_CPARTICLE (+0x1e4)
			// also sets it on every particle (FUN_31cf1df0). Types 7 and 9 are left alone.
			if( true == pEffectData->m_bFollowUnitRotation )
			{
				const D3DXVECTOR3 vUnitRotation = pEffectSetInstance->GetUnitRotation( pEffectData );
				CKTDGXMeshPlayer::CXMeshInstance* pFollowMesh = NULL;
				CKTDGParticleSystem* pFollowParticleSystem = NULL;
				switch( pEffectData->m_eEffectType )
				{
				case ET_DAMAGE_EFFECT:
					if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
					{
	#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						if( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect ) )
	#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
						if( CX2DamageEffect::CEffect* pEffect = pEffectInstance->m_pDamageEffect )
	#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
							pFollowMesh = pEffect->GetMainEffect();
					}
					break;
				case ET_MESH_PLAYER_UI_MAJOR:
					if( NULL != g_pData->GetUIMajorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
						pFollowMesh = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( pEffectInstance->m_hMeshPlayer );
					break;
				case ET_MESH_PLAYER_UI_MINOR:
					if( NULL != g_pData->GetUIMinorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
						pFollowMesh = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance( pEffectInstance->m_hMeshPlayer );
					break;
				case ET_MESH_PLAYER_GAME_MAJOR:
					if( NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
						pFollowMesh = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( pEffectInstance->m_hMeshPlayer );
					break;
				case ET_MESH_PLAYER_GAME_MINOR:
					if( NULL != g_pX2Game && NULL != g_pX2Game->GetMinorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
						pFollowMesh = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( pEffectInstance->m_hMeshPlayer );
					break;
				case ET_PARTICLE_UI_MAJOR:
					pFollowParticleSystem = ( NULL != g_pData ) ? g_pData->GetUIMajorParticle() : NULL;
					break;
				case ET_PARTICLE_GAME_MAJOR:
					pFollowParticleSystem = ( NULL != g_pX2Game ) ? g_pX2Game->GetMajorParticle() : NULL;
					break;
				default:
					break;
				}

				if( NULL != pFollowMesh )
				{
					pFollowMesh->SetRotateDegree( vUnitRotation );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
					JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"FOLLOW_UNIT_ROTATION", L"mesh_rotate(321e1691)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				}
				else if( NULL != pFollowParticleSystem && INVALID_PARTICLE_SEQUENCE_HANDLE != pEffectInstance->m_hParticleSequence )
				{
					if( CKTDGParticleSystem::CParticleEventSequence* pSeq = pFollowParticleSystem->GetInstanceSequence( pEffectInstance->m_hParticleSequence ) )
					{
						const D3DXVECTOR3 vAddRotate = pSeq->GetAddRotateRel() + vUnitRotation;
						pSeq->SetAddRotate( vAddRotate );
						if( true == pEffectData->m_bFollowUnitRotationCParticle )
							pSeq->SetRotateToParticles( vAddRotate );
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
						JuneFxEsRuntime( L"ES_UPDATE", pEffectSetInstance, pEffectData, L"FOLLOW_UNIT_ROTATION",
							( true == pEffectData->m_bFollowUnitRotationCParticle ) ? L"particle_add_rotate+cparticle(321e1afd)" : L"particle_add_rotate(321e1acc)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
					}
				}
			}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		}
	}
}

void CX2EffectSet::OnFrameMove_StartCameraShake()
{
	KTDXPROFILE();
	
	if ( NULL == g_pX2Game )
		return;

	//camera shake
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterNext;
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	BOOST_TEST_FOREACH( EffectSetInstance*, pEffectSetInstance, m_vecpEffectSetInstance )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		EffectSetData* pEffectSetData = pEffectSetInstance->m_pEffectSetData;

		for ( UINT i=0; i<pEffectSetData->m_vecpCameraShakeData.size(); i++ )
		{
			if( true == pEffectSetInstance->m_vecCameraShakeDone[i] )
				continue;

			CameraShakeData* pCameraShakeData = pEffectSetData->m_vecpCameraShakeData[i];


			if( 0.f != pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime ) // 0초에 시작이면 무조건 바로 생성
			{
#ifdef EFFECT_TOOL						
				if( pEffectSetInstance->m_fElapsedTime < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
				{
					continue;
				}
#else
				switch( pEffectSetInstance->m_pEffectSetData->m_eEventTimerType )
				{
				default:
				case ESTT_ANIM:
					{
#ifdef SERV_PET_SYSTEM						
						if( pEffectSetInstance->m_pPet != NULL )
						{
							if( pEffectSetInstance->m_pPet->GetXSkinAnim() != NULL && 
								pEffectSetInstance->m_pPet->GetXSkinAnim()->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
#ifdef RIDING_SYSTEM
						else if ( NULL != pEffectSetInstance->GetOwnerUnit() && NULL != pEffectSetInstance->GetOwnerUnit()->GetRidingPet() )
						{
							if( pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr() != NULL &&
								pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr()->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
#endif //RIDING_SYSTEM
						else
						{
							if( 
#ifdef  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
#endif  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX                           
                                pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
#else
						//if( false == pEffectSetInstance->m_pXSkinAnim->EventTimer( pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime ) )
						if( 
#ifdef  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
#endif  X2OPTIMIZE_EFFECTSET_NULL_CHECK_BUG_FIX                            
                            pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
							continue;
#endif						
					} break;

				case ESTT_CUSTOM:
					{
						//if( pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime <= pEffectSetInstance->m_fPrevElapsedTime ||
						//	pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime > pEffectSetInstance->m_fElapsedTime )
						if( pEffectSetInstance->m_fElapsedTime < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
						{
							continue;
						}
					} break;
				}
#endif //EFFECT_TOOL
			}


			pEffectSetInstance->m_vecCameraShakeDone[i] = true;
#ifdef EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE
			if( ( NULL != g_pX2Game->GetMyUnit() && GetDistance3Sq( g_pX2Game->GetMyUnit()->GetPos(), pEffectSetInstance->m_vPosition ) < pCameraShakeData->m_iLimitDistanceSq ) || IGNORE_DISTANCE == pCameraShakeData->m_iLimitDistanceSq )
#else //EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE
#ifdef FIX_OBSERVER_MODE_CRASH
			if ( NULL != g_pX2Game->GetMyUnit() && GetDistance3Sq( g_pX2Game->GetMyUnit()->GetPos(), pEffectSetInstance->m_vPosition ) < pCameraShakeData->m_iLimitDistanceSq )
#else  FIX_OBSERVER_MODE_CRASH
			if ( GetDistance3Sq( g_pX2Game->GetMyUnit()->GetPos(), pEffectSetInstance->m_vPosition ) < pCameraShakeData->m_iLimitDistanceSq )
#endif FIX_OBSERVER_MODE_CRASH
#endif //EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE
			{
				switch( pCameraShakeData->m_eShakeType )
				{
				case CKTDGCamera::DECT_UP:
					{
						g_pX2Game->GetX2Camera()->GetCamera().UpCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;
				case CKTDGCamera::DECT_DOWN:
					{
						g_pX2Game->GetX2Camera()->GetCamera().DownCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;
				case CKTDGCamera::DECT_UP_DOWN:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeUpDown( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime, pCameraShakeData->m_fTwoDirTimeGap );
					} break;
				case CKTDGCamera::DECT_UP_DOWN_NO_RESET:
					{
						g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCameraNoReset( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime );
					} break;

				case CKTDGCamera::DECT_LEFT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().LeftCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;

				case CKTDGCamera::DECT_RIGHT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().RightCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;

				case CKTDGCamera::DECT_LEFT_RIGHT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeLeftRight( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime, pCameraShakeData->m_fTwoDirTimeGap );
					} break;
				case CKTDGCamera::DECT_LEFT_RIGHT_NO_RESET:
					{
						g_pX2Game->GetX2Camera()->GetCamera().LeftRightCrashCameraNoReset( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime );
					} break;



				case CKTDGCamera::DECT_RANDOM:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeRandom( pCameraShakeData->m_iEyeShakeCount, pCameraShakeData->m_EyeMoveSpeed.m_Min, pCameraShakeData->m_EyeMoveSpeed.m_Max, 
							pCameraShakeData->m_EyeMoveRange.m_Min, pCameraShakeData->m_EyeMoveRange.m_Max, 
							pCameraShakeData->m_iLookAtShakeCount, pCameraShakeData->m_LookAtMoveSpeed.m_Min, pCameraShakeData->m_LookAtMoveSpeed.m_Max, 
							pCameraShakeData->m_LookAtMoveRange.m_Min, pCameraShakeData->m_LookAtMoveRange.m_Max, 
							pCameraShakeData->m_bFixedLookVec );
					} break;
				}
			}
		}

	}
}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
CX2EffectSet::EffectSetInstance*   CX2EffectSet::_CreateEffectSetInstance( 
    EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr skinAnimPtr, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit,
#ifdef  SERV_PET_SYSTEM
    CX2PET *pPet
#endif  SERV_PET_SYSTEM
    )
{
    if ( pEffectSetData == NULL )
        return NULL;

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321e1bf0 @ 321e1c68-321e1c77 runs FUN_321d9f30 before creating anything.
	// With OVERLAP set, a templet that already has a live instance with this trace target
	// (null matches null) gets no new instance, and PlayEffectSet returns an invalid handle.
	if( true == pEffectSetData->m_bOverlap )
	{
		KInstanceHandleList::iterator iterLiveEnd = m_coInstanceHandleList.end( LIST_LIVE );
		for( KInstanceHandleList::iterator iterLive = m_coInstanceHandleList.begin( LIST_LIVE ); iterLive != iterLiveEnd; ++iterLive )
		{
			EffectSetInstance* pLiveInstance = iterLive->m_pInstance;
			if( NULL != pLiveInstance && pEffectSetData == pLiveInstance->m_pEffectSetData &&
				pTraceTargetGameUnit == pLiveInstance->m_optrTraceTargetGameUnit.GetObservable() )
			{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
				JuneFxEsCreate( pLiveInstance, NULL, L"OVERLAP", L"refused_new_instance(321d9f30)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
				return NULL;
			}
		}
	}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

    KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
    if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
    {
        if ( m_coInstanceHandleList.storage_size() >= 0x10000 )
        {
            return NULL;
        }
        m_coInstanceHandleList.push_back_default(LIST_FREE);
        iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
    }


	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, pSquareUnit
#ifdef  SERV_PET_SYSTEM
        , pPet 
#endif  SERV_PET_SYSTEM
        );
    ASSERT( pEffectSetInstance != NULL );
    m_coInstanceHandleList.splice( m_coInstanceHandleList.end(LIST_LIVE), iterEmpty );
    iterEmpty->m_pInstance = pEffectSetInstance;

    WORD   wIndex = (WORD) iterEmpty.GetIndex();
    Handle  handle;
    DWORD   dwHandle = 0;
    do
    {
        ++iterEmpty->m_wStamp;
        dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        handle.SetValue( (int) dwHandle );
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        handle = (int) dwHandle;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    } while ( handle == INVALID_EFFECTSET_HANDLE );
    pEffectSetInstance->SetHandle( handle );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321e1bf0 @ 321e1dd7-321e1e0c, after the instance ctor: the instance's effect-alpha option
	// (+0x1bc) is the templet's APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION when non-zero, else a value derived
	// from the game options (FUN_322d4d10), for every set. This build has neither, so a set using the key is
	// logged only (plan rule 5).
	if( 0 != pEffectSetData->m_iApplyEffectAlphaConditionByOption )
		JuneFxUnsupported( pEffectSetData, L"APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION", L"", L"effect-alpha option",
			pEffectSetData->m_iApplyEffectAlphaConditionByOption, L"321e1dd7" );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

    return  pEffectSetInstance;
}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

CX2EffectSet::Handle CX2EffectSet::PlayEffectSet( const wstring& wstrEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit /*= NULL*/, 
												 bool bHyper /*= false*/, float fPowerRate /*= -1.f*/, float fLifeTime /*= -1.f */, 
												 D3DXVECTOR3 vScale /*= D3DXVECTOR3( 1, 1, 1 )*/, 
												 bool bCustomPosition /*= false*/, const D3DXVECTOR3& vPosition/* = D3DXVECTOR3(0, 0, 0)*/, 
												 const D3DXVECTOR3& vRotationDegree /*= D3DXVECTOR3(0, 0, 0)*/, const D3DXVECTOR3& vDirVector /*= D3DXVECTOR3(0, 0, 0)*/)
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
	{
#ifdef SERV_IRUHADEV_RANOX_EFFECT_DEBUG
		static map<wstring, DWORD> s_mapLastLoggedTime;
		DWORD dwNow = timeGetTime();
		map<wstring, DWORD>::iterator itThrottle = s_mapLastLoggedTime.find( wstrEffectSetName );
		if( itThrottle == s_mapLastLoggedTime.end() || dwNow - itThrottle->second >= 1000 )
		{
			s_mapLastLoggedTime[ wstrEffectSetName ] = dwNow;
			CX2OfflineLog::Server( L"[RANOX_EFFECT_MISSING] EffectSet not found: %s", wstrEffectSetName.c_str() );
		}
#endif SERV_IRUHADEV_RANOX_EFFECT_DEBUG
		return INVALID_EFFECTSET_HANDLE;
	}

	if( NULL == pGameUnit )
		return INVALID_EFFECTSET_HANDLE;



	CKTDGXSkinAnimPtr skinAnimPtr;
	switch( pGameUnit->GetGameUnitType() )
	{
	case CX2GameUnit::GUT_USER:
		{
			CX2GUUser* pUser = (CX2GUUser*) pGameUnit;
			skinAnimPtr = pUser->GetXSkinAnimPtr();
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
			// June2017 FUN_321e1e60 @ 321e1f51-321e1f95, user owners only: APPLIED_SKILL_ID looks the skill's memo up
			// on the user (FUN_31fdbbb0, user +0x867c) and, for a type-2 entry, sets the templet's memo scale
			// (EffectSetData +0x60), which GetEffectScale applies unless NOT_ALLOW_MEMO. This build has no
			// applied-skill-ID system, so a set using the key is logged only (plan rule 5).
			if( 0 != pEffectSetData->m_iAppliedSkillID )
				JuneFxUnsupported( pEffectSetData, L"APPLIED_SKILL_ID", L"", L"applied-skill-ID system", pEffectSetData->m_iAppliedSkillID, L"321e1f57" );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		} break;

	case CX2GameUnit::GUT_NPC:
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) pGameUnit;
			skinAnimPtr = pNPC->GetXSkinAnimPtr();
		} break;
	}

	if( skinAnimPtr == NULL )
		return INVALID_EFFECTSET_HANDLE;


#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, CX2SquareUnitPtr() );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef SERV_PET_SYSTEM
	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, CX2SquareUnitPtr(), NULL );
#else
	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, CX2SquareUnitPtr() );
#endif

	if( INVALID_EFFECTSET_HANDLE == pEffectSetInstance->m_hHandle )
	{
		SAFE_DELETE( pEffectSetInstance );
		return INVALID_EFFECTSET_HANDLE;
	}

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE



#ifdef EFFECT_USE_LOG
	if( NULL != g_pMain->GetGameEdit() &&
		CX2GameEdit::ELL_EFFECT_LOG_1 <= g_pMain->GetGameEdit()->GetEffectLogLevel() )
	{
		dbg::clog << L"(이펙트셋)" << wstrEffectSetName << dbg::endl;
		KLOG("EffectLog.txt")  << L"EffectSet : " << wstrEffectSetName.c_str() << fileout;
	}
#endif // EFFECT_USE_LOG

	pEffectSetInstance->m_bHyper = bHyper;
	pEffectSetInstance->m_fPowerRate = fPowerRate;
	pEffectSetInstance->m_fLifeTime = fLifeTime;
	pEffectSetInstance->m_vScale = vScale;

/// 각 파티클 클래스 내에서 유닛 스케일을 적용해 주므로, 중복 적용 구문 제거
#ifndef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환

#ifdef GIANT_UNIT_GIANT_EFFECT_TEST

	if( true == pEffectSetData->m_bAffectedByUnitScale )
	{
		if( NULL != pGameUnit )
		{
			if( pGameUnit->GetScaleByUnit() != D3DXVECTOR3( 1, 1, 1 ) )
			{
				pEffectSetInstance->m_vScale.x *= pGameUnit->GetScaleByUnit().x;
				pEffectSetInstance->m_vScale.y *= pGameUnit->GetScaleByUnit().y;
				pEffectSetInstance->m_vScale.z *= pGameUnit->GetScaleByUnit().z;
			}
		}
	}

#endif GIANT_UNIT_GIANT_EFFECT_TEST

#endif // FIX_EFFECT_SCALE_BY_UNIT_SCALE



	pEffectSetInstance->m_bCustomPosition = bCustomPosition;
	if( true == bCustomPosition )
	{
		pEffectSetInstance->m_vPosition			= vPosition;
		pEffectSetInstance->m_vRotationDegree	= vRotationDegree;
		pEffectSetInstance->m_vDirVector		= vDirVector;
		pEffectSetInstance->m_vZVector			= CX2EffectSet::GetZVector( vDirVector );
	}
	else
		pEffectSetInstance->m_vPosition = pGameUnit->GetPos();	// 커스텀포지션이 아닌 경우 이펙트를 발생 시킨 유저의 위치를 넣어줌
	
#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_vecpEffectSetInstance.push_back( pEffectSetInstance );	
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	return pEffectSetInstance->m_hHandle;
}


CX2EffectSet::Handle CX2EffectSet::PlayEffectSet( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( pSquareUnit == NULL )
		return INVALID_EFFECTSET_HANDLE;

	if( pSquareUnit->GetUnitViewer() == NULL )
		return INVALID_EFFECTSET_HANDLE;



	CKTDGXSkinAnimPtr skinAnimPtr = pSquareUnit->GetUnitViewer()->GetXSkinAnimPtr();
	if( skinAnimPtr == NULL )
		return INVALID_EFFECTSET_HANDLE;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, skinAnimPtr, NULL, NULL, pSquareUnit );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef SERV_PET_SYSTEM
	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, NULL, NULL, pSquareUnit, NULL );
#else
	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, NULL, NULL, pSquareUnit );
#endif

	if( INVALID_EFFECTSET_HANDLE == pEffectSetInstance->m_hHandle )
	{
		SAFE_DELETE( pEffectSetInstance );
		return INVALID_EFFECTSET_HANDLE;
	}
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef EFFECT_USE_LOG
	if( NULL != g_pMain->GetGameEdit() &&
		CX2GameEdit::ELL_EFFECT_LOG_1 <= g_pMain->GetGameEdit()->GetEffectLogLevel() )
	{
		dbg::clog << L"(이펙트셋)" << wstrEffectSetName << dbg::endl;
		KLOG("EffectLog.txt")  << L"EffectSet : " << wstrEffectSetName.c_str() << fileout;
	}
#endif // EFFECT_USE_LOG

#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_vecpEffectSetInstance.push_back( pEffectSetInstance );	
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	return pEffectSetInstance->m_hHandle;
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetCustomPos( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, const D3DXVECTOR3& vPos )
{
	CX2EffectSet::Handle hEffectset = PlayEffectSet(wstrEffectSetName, pSquareUnit );
	if( INVALID_EFFECTSET_HANDLE != hEffectset )
	{
		EffectSetInstance* pEffect = g_pData->GetGameEffectSet()->GetEffectSetInstance( hEffectset );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        if( NULL != pEffect )
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
		if( NULL != hEffectset)
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
			pEffect->SetEffectPosition( vPos );
	}

	return hEffectset;
}
#ifdef SERV_PET_SYSTEM
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByPet_LUA( const char* strEffectSetName, CX2PET* pPet )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, strEffectSetName );

	return PlayEffectSetByPet( wstrEffectSetName, pPet );
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByPet( const wstring& wstrEffectSetName, CX2PET* pPet )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( pPet == NULL )
		return INVALID_EFFECTSET_HANDLE;

	CKTDGXSkinAnim* skinAnim = pPet->GetXSkinAnim();
	if( skinAnim == NULL )
		return INVALID_EFFECTSET_HANDLE;

	CKTDGXSkinAnimPtr tempPtr;


#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, tempPtr, NULL, NULL, CX2SquareUnitPtr(), pPet );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, tempPtr, NULL, NULL, CX2SquareUnitPtr(), pPet );
	if( INVALID_EFFECTSET_HANDLE == pEffectSetInstance->m_hHandle )
	{
		SAFE_DELETE( pEffectSetInstance );
		return INVALID_EFFECTSET_HANDLE;
	}
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	pEffectSetInstance->m_bHyper = false;
	pEffectSetInstance->m_fPowerRate = -1.f;
	pEffectSetInstance->m_fLifeTime = -1.f;
	pEffectSetInstance->m_vScale = D3DXVECTOR3( 1.f, 1.f, 1.f );

#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_vecpEffectSetInstance.push_back( pEffectSetInstance );	
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	return pEffectSetInstance->m_hHandle;
}
#endif




void CX2EffectSet::StopEffectSet( CX2EffectSet::Handle& hHandle )
{
	if( INVALID_EFFECTSET_HANDLE == hHandle )
		return;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = hHandle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( hHandle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    if ( DecomposeHandle( dwHandle, wIndex, wStamp ) == false )
    {
        ASSERT( 0 );
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
    {
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance == NULL || info.m_wStamp != wStamp )
    {
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    EffectSetInstance* pEffectSetInstance = info.m_pInstance;
    info.m_pInstance = NULL;
    m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), wIndex );
    hHandle = INVALID_EFFECTSET_HANDLE;
    SAFE_DELETE( pEffectSetInstance );

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	for( UINT i=0; i<m_vecpEffectSetInstance.size(); i++ )
	{
		EffectSetInstance* pEffectSetInstance = m_vecpEffectSetInstance[i];
		if( NULL != pEffectSetInstance )
		{
			if( pEffectSetInstance->m_hHandle == hHandle )
			{
				SAFE_DELETE( pEffectSetInstance );

				m_vecpEffectSetInstance.erase( m_vecpEffectSetInstance.begin() + i );
				break;
			}
		}
	}

	hHandle = INVALID_EFFECTSET_HANDLE;

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}

void CX2EffectSet::StopEffectSetAll()
{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
    {
        iterNext = iter; ++iterNext;
        KInstanceHandleInfo& info = *iter;
        EffectSetInstance* pESInstance = info.m_pInstance;
        info.m_pInstance = NULL;
        SAFE_DELETE( pESInstance );
    }
    m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin( LIST_FREE ), LIST_LIVE );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	BOOST_TEST_FOREACH( EffectSetInstance*, pEffectSetInstance, m_vecpEffectSetInstance )
	{
		SAFE_DELETE( pEffectSetInstance );
	}
	m_vecpEffectSetInstance.resize( 0 );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}






CX2EffectSet::EffectSetInstance* CX2EffectSet::GetEffectSetInstance( CX2EffectSet::Handle hHandle )
{
	if( INVALID_EFFECTSET_HANDLE == hHandle )
		return NULL;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = hHandle.GetValue();
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    DWORD   dwHandle = static_cast<DWORD>( hHandle );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    if ( DecomposeHandle( dwHandle, wIndex, wStamp ) == false )
    {
        ASSERT( 0 );
        return NULL;
    }
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
        return NULL;
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    EffectSetInstance* pInstance = info.m_pInstance;
    if ( pInstance == NULL || info.m_wStamp != wStamp )
        return NULL;
    return  pInstance;

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	return s_HandleManager.GetObjectByHandle( hHandle );

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}









CX2EffectSet::EffectSetData* CX2EffectSet::GetEffectSetTemplet( const wstring& wstrEffectSetName )
{
	if( true == wstrEffectSetName.empty() )
		return NULL;

	EffectSetDataMap::iterator it = m_mapEffectSetTemplet.find( wstrEffectSetName );
	if( it != m_mapEffectSetTemplet.end() )
	{
		return it->second;
	}

	return NULL;
}



void CX2EffectSet::RegisterLuaBind()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pEffectSet",		this );	
}


#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
// Per-OpenScriptFile counters for the JUNE_FX ES_LOAD summary line: how many
// templets reached the June templet-key block, and how many set a June key.
static int s_iJuneFxTempletCount = 0;
static int s_iJuneFxTempletWithJuneKeyCount = 0;
// Phase 3: how many EFFECT%d entries reached the June per-effect block, how many of
// them set a June key, and how many SOUND%d entries were read.
static int s_iJuneFxEffectCount = 0;
static int s_iJuneFxEffectWithJuneKeyCount = 0;
static int s_iJuneFxSoundCount = 0;
#endif SERV_IRUHADEV_JUNE_FX_DEBUG

bool CX2EffectSet::OpenScriptFile( const wstring& wstrFileName )
{
	RegisterLuaBind();

	if( true == wstrFileName.empty() )
		return false;

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	s_iJuneFxTempletCount = 0;
	s_iJuneFxTempletWithJuneKeyCount = 0;
	s_iJuneFxEffectCount = 0;
	s_iJuneFxEffectWithJuneKeyCount = 0;
	s_iJuneFxSoundCount = 0;
	const bool bJuneFxLoaded = g_pKTDXApp->LoadLuaTinker( wstrFileName.c_str() );
	{
		// load-time: offline_server.log is not open yet (the offline server starts on
		// first connect), so this goes to ErrorLog.txt
		WCHAR wszJuneFx[512] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX ES_LOAD %s loaded=%d templets=%d with_june_templet_key=%d effects=%d with_june_effect_key=%d sounds=%d",
			wstrFileName.c_str(), bJuneFxLoaded ? 1 : 0, s_iJuneFxTempletCount, s_iJuneFxTempletWithJuneKeyCount,
			s_iJuneFxEffectCount, s_iJuneFxEffectWithJuneKeyCount, s_iJuneFxSoundCount );
		ErrorLogMsg( XEM_ERROR142, wszJuneFx );
	}
	return bJuneFxLoaded;
#else
	return g_pKTDXApp->LoadLuaTinker( wstrFileName.c_str() );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}

bool CX2EffectSet::AddEffectSetTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING


	EffectSetData* pEffectSetData = new EffectSetData;
	LUA_GET_VALUE( luaManager,		"NAME",			pEffectSetData->m_wstrEffectSetName,			L""		);

	ASSERT( false == pEffectSetData->m_wstrEffectSetName.empty() );
	if( true == pEffectSetData->m_wstrEffectSetName.empty() )
		goto effect_set_error_proc;


	ASSERT( NULL == GetEffectSetTemplet( pEffectSetData->m_wstrEffectSetName ) );
	if( NULL != GetEffectSetTemplet( pEffectSetData->m_wstrEffectSetName ) )
		goto effect_set_error_proc;



	LUA_GET_VALUE_ENUM( luaManager,		"EVENT_TIMER_TYPE",			pEffectSetData->m_eEventTimerType,			EFFECT_SET_TIMER_TYPE,	ESTT_ANIM );
	LUA_GET_VALUE( luaManager,		"AFFECTED_BY_UNIT_SCALE",		pEffectSetData->m_bAffectedByUnitScale,		true );

	//LUA_GET_VALUE( luaManager,		"DELETE_WHEN_TRACE_TARGET_DEAD",	pEffectSetData->m_bDeleteWhenTraceTargetDead,				false	);
	LUA_GET_VALUE( luaManager,		"DELETE_SHAKE_COUNT",				pEffectSetData->m_iDeleteShakeCount,						-1	);
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
	// June2017 FUN_321e48e0 (AddEffectSetTemplet_LUA) @ 321e48e0-321e5cf8, the templet
	// level: everything before BeginTable( "EFFECT", n ) @ 321e5cf8. June reads ten keys
	// there, in this order: NAME, EVENT_TIMER_TYPE, STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE,
	// AFFECTED_BY_UNIT_SCALE, DELETE_SHAKE_COUNT, DELETE_EFFECTSET_AT_TRACE_UNIT_DIE,
	// OVERLAP, APPLIED_SKILL_ID, SYNC_SHOW_BY_CREATE_UNIT,
	// APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION. The four March already reads above match
	// June's getter and default. The six below are new; their reads are independent, so
	// they sit together here rather than interleaved in June's order. Getters:
	// 32fe9e40 = bool, 32fe9fd0 = int. Parse only; runtime is Phase 4a/4b.
	// June2017 FUN_321e48e0 @ 321e4c9b-321e4f3c: STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE, bool, default false (on failure the getter's AL=0 is stored), no post-processing
	LUA_GET_VALUE( luaManager,		"STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE",	pEffectSetData->m_bStopCreateNewEffectWhenStateChange,	false	);	// consumed in Phase 4a
	// June2017 FUN_321e48e0 @ 321e531f-321e5560: DELETE_EFFECTSET_AT_TRACE_UNIT_DIE, bool, default false (AL=0 stored), no post-processing
	LUA_GET_VALUE( luaManager,		"DELETE_EFFECTSET_AT_TRACE_UNIT_DIE",		pEffectSetData->m_bDeleteEffectSetAtTraceUnitDie,		false	);	// consumed in Phase 4b
	// June2017 FUN_321e48e0 @ 321e5594-321e5626: OVERLAP, bool, default false (BL=0 stored), no post-processing
	LUA_GET_VALUE( luaManager,		"OVERLAP",									pEffectSetData->m_bOverlap,								false	);	// consumed in Phase 4a
	// June2017 FUN_321e48e0 @ 321e5675-321e579e: APPLIED_SKILL_ID, int, default 0 (stack temp zeroed @ 321e5631; result = success ? value : 0), stored as int, no post-processing
	LUA_GET_VALUE( luaManager,		"APPLIED_SKILL_ID",							pEffectSetData->m_iAppliedSkillID,						0		);	// unsupported, see below
	// June2017 FUN_321e48e0 @ 321e57fa-321e599c: SYNC_SHOW_BY_CREATE_UNIT, bool, default false (BL=0 stored), no post-processing
	LUA_GET_VALUE( luaManager,		"SYNC_SHOW_BY_CREATE_UNIT",					pEffectSetData->m_bSyncShowByCreateUnit,				false	);	// consumed in Phase 4a
	// June2017 FUN_321e48e0 @ 321e5a5d-321e5cec: APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION, int getter, default 0
	// (stack temp zeroed @ 321e59a3). June keeps only the low byte: NEG AL / SBB AL,AL /
	// AND AL,[temp] then a byte store @ 321e5cec, so the member is char-sized in June. The
	// values are June's APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION enum table (AEACBO_NONE=0,
	// AEACBO_NOT_APPLY=1, ...), which March's Enum.lua does not have (Phase 11).
	LUA_GET_VALUE( luaManager,		"APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION",	pEffectSetData->m_iApplyEffectAlphaConditionByOption,	0		);	// unsupported, see below

	// Plan rule 5: two keys belong to systems this build does not have. They are stored
	// and logged once per templet, with no behaviour. Both tests are for a non-default
	// value: an explicit 0 behaves exactly like an absent key, and every June data use is
	// non-zero (skill IDs; AEACBO_NOT_APPLY=1). Load-time, so ErrorLog.txt, not
	// offline_server.log (it is not open yet).
	if( 0 != pEffectSetData->m_iAppliedSkillID )
	{
		WCHAR wszJuneFx[512] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX unsupported APPLIED_SKILL_ID in %s: applied-skill-ID system absent (value %d)",
			pEffectSetData->m_wstrEffectSetName.c_str(), pEffectSetData->m_iAppliedSkillID );
		ErrorLogMsg( XEM_ERROR142, wszJuneFx );
	}
	if( 0 != pEffectSetData->m_iApplyEffectAlphaConditionByOption )
	{
		WCHAR wszJuneFx[512] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX unsupported APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION in %s: effect-alpha option absent (value %d)",
			pEffectSetData->m_wstrEffectSetName.c_str(), pEffectSetData->m_iApplyEffectAlphaConditionByOption );
		ErrorLogMsg( XEM_ERROR142, wszJuneFx );
	}

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	// JUNE_FX ES_TEMPLET <name> <key>=<v> ..., only for templets that set a June key
	{
		++s_iJuneFxTempletCount;
		WCHAR wszJuneFx[512] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX ES_TEMPLET %s", pEffectSetData->m_wstrEffectSetName.c_str() );
		const size_t uiNameOnly = wcslen( wszJuneFx );
		WCHAR wszKey[128] = {0};
		if( true == pEffectSetData->m_bStopCreateNewEffectWhenStateChange )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE=1" );
		if( true == pEffectSetData->m_bDeleteEffectSetAtTraceUnitDie )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" DELETE_EFFECTSET_AT_TRACE_UNIT_DIE=1" );
		if( true == pEffectSetData->m_bOverlap )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" OVERLAP=1" );
		if( 0 != pEffectSetData->m_iAppliedSkillID )
		{
			StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" APPLIED_SKILL_ID=%d", pEffectSetData->m_iAppliedSkillID );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
		}
		if( true == pEffectSetData->m_bSyncShowByCreateUnit )
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" SYNC_SHOW_BY_CREATE_UNIT=1" );
		if( 0 != pEffectSetData->m_iApplyEffectAlphaConditionByOption )
		{
			StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION=%d", pEffectSetData->m_iApplyEffectAlphaConditionByOption );
			StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
		}
		if( wcslen( wszJuneFx ) != uiNameOnly )
		{
			++s_iJuneFxTempletWithJuneKeyCount;
			ErrorLogMsg( XEM_ERROR142, wszJuneFx );
		}
	}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

	
	
	// damageeffect, meshplayer, particle 
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
	// Plan rule 5 logs an unsupported key once per templet, so the per-EFFECT%d
	// APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION below reports only the first effect using it.
	bool bJuneFxEffectAlphaLogged = false;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
	int iEffectTableIndex = 0;
	while( true == luaManager.BeginTable( "EFFECT", iEffectTableIndex ) )
	{
		EffectData* pEffectData = new EffectData;

		// 공통 스크립트
		LUA_GET_VALUE_ENUM( luaManager, "EFFECT_TYPE",				pEffectData->m_eEffectType,					EFFECT_TYPE,	ET_INVALID		);
		if( ET_INVALID == pEffectData->m_eEffectType )
			goto effect_error_proc;

		LUA_GET_VALUE( luaManager,		"NAME",						pEffectData->m_EffectName,					L""		);
		ASSERT( false == pEffectData->m_EffectName.empty() );
		if( true == pEffectData->m_EffectName.empty() )
			goto effect_error_proc;

		LUA_GET_VALUE( luaManager,		"HYPER_NAME",				pEffectData->m_EffectNameHyper,				L""		);



		LUA_GET_VALUE( luaManager,		"START_TIME",				pEffectData->m_fStartAnimTime,				0.f		);
		LUA_GET_VALUE( luaManager,		"BONE_NAME",				pEffectData->m_PositionBoneName,			L""		);
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321e6261-321e632b: GROUP_NAME, string, default L"", no post-processing; binary-only (no June data uses it)
		LUA_GET_VALUE( luaManager,		"GROUP_NAME",				pEffectData->m_wstrGroupName,				L""		);
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_NAME",			pEffectData->m_PositionWeaponBoneName,		L""		);
		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_WEAPON_INDEX",	pEffectData->m_iWeaponIndex,				0		);
		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_MODEL_INDEX",	pEffectData->m_iModelIndex,					0		);
		
#ifdef RIDING_SYSTEM
		LUA_GET_VALUE( luaManager,		"RIDING_PET_BONE_NAME",		pEffectData->m_PosRidingPetBoneName,		L""		);
#endif //RIDING_SYSTEM

		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_X",		pEffectData->m_vPositionOffset.x,			0.f		);
		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_Y",		pEffectData->m_vPositionOffset.y,			0.f		);
		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_Z",		pEffectData->m_vPositionOffset.z,			0.f		);
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321e6f61-321e7115: ZOFFSET_USE_UNIT_IS_RIGHT, bool, default false (AL=0 stored), no post-processing
		LUA_GET_VALUE( luaManager,		"ZOFFSET_USE_UNIT_IS_RIGHT",	pEffectData->m_bZOffsetUseUnitIsRight,		false	);
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		
		LUA_GET_VALUE( luaManager,		"TRACE",					pEffectData->m_bTrace,						false	);
		LUA_GET_VALUE( luaManager,		"TRACE_MORE",				pEffectData->m_bTraceMore,					false	);

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		LUA_GET_VALUE( luaManager,		"TRACE_MATRIX",				pEffectData->m_bTraceMatrix,					false	);
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321e7445-321e7610: TRACE_MATRIX_EXCLUDE_SCALE, bool, default false (AL=0 stored).
		// Post-processing @ 321e760b-321e7610: when it is set, June also sets TRACE_MATRIX
		// (CMP byte [ESI],1 / MOV byte [EDI+TRACE_MATRIX],1). This changes a member March
		// already consumes, for sets that use the key only.
		LUA_GET_VALUE( luaManager,		"TRACE_MATRIX_EXCLUDE_SCALE",	pEffectData->m_bTraceMatrixExcludeScale,	false	);
		if( true == pEffectData->m_bTraceMatrixExcludeScale )
			pEffectData->m_bTraceMatrix = true;
		// June2017 FUN_321e48e0 @ 321e766e-321e77e2: TRACE_MATRIX_IN_FIELD, bool, default false (AL=0 stored), no post-processing
		LUA_GET_VALUE( luaManager,		"TRACE_MATRIX_IN_FIELD",		pEffectData->m_bTraceMatrixInField,			false	);
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

#ifdef ARA_CHANGE_CLASS_FIRST
		LUA_GET_VALUE( luaManager,		"REVERSE_Y",				pEffectData->m_bReverseY,					false	);
#endif
				

		LUA_GET_VALUE( luaManager,		"USE_LAND_POSITION",		pEffectData->m_bLandPosition,				false	);
		LUA_GET_VALUE( luaManager,		"ROTATE_X",					pEffectData->m_vRotateRel.x,				0.f		);
		LUA_GET_VALUE( luaManager,		"ROTATE_Y",					pEffectData->m_vRotateRel.y,				0.f		);
		LUA_GET_VALUE( luaManager,		"ROTATE_Z",					pEffectData->m_vRotateRel.z,				0.f		);
		LUA_GET_VALUE( luaManager,		"APPLY_UNIT_ROTATION",		pEffectData->m_bApplyUnitRotation,			true	);

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		/// 회전값 설정시 좌 우 완벽한 대칭을 이루기 위해, 방향에 따른 x회전 값 반전 기능 추가
		LUA_GET_VALUE( luaManager,		"APPLY_UNIT_ROTATION_MIRROR",	pEffectData->m_bApplyUnitRotationMirror, false );

		/// 위의 기능이 설정되어 있다면, APPLY_UNIT_ROTATION은 해제
		if ( true == pEffectData->m_bApplyUnitRotationMirror )
			pEffectData->m_bApplyUnitRotation = false;
#endif // SERV_ARA_CHANGE_CLASS_SECOND

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321e8114-321e8b63: five bools read after
		// APPLY_UNIT_ROTATION_MIRROR and before APPLY_INSTANCE_SCALE, each with default
		// false (the getter's AL=0 stored on failure) and no post-processing:
		// USE_IS_RIGHT_WHEN_CREATE @ 321e8114-321e82bb, FOLLOW_UNIT_ROTATION @ 321e830c-321e846f,
		// FOLLOW_UNIT_ROTATION_CPARTICLE @ 321e84da-321e86dd, FOLLOW_UNIT_VIEWER_SCALE @
		// 321e8738-321e88dc, USE_EFFECT_SCALE_IN_UNIT_VIEWER @ 321e894f-321e8b63.
		LUA_GET_VALUE( luaManager,		"USE_IS_RIGHT_WHEN_CREATE",			pEffectData->m_bUseIsRightWhenCreate,			false	);
		LUA_GET_VALUE( luaManager,		"FOLLOW_UNIT_ROTATION",				pEffectData->m_bFollowUnitRotation,				false	);
		LUA_GET_VALUE( luaManager,		"FOLLOW_UNIT_ROTATION_CPARTICLE",	pEffectData->m_bFollowUnitRotationCParticle,	false	);
		LUA_GET_VALUE( luaManager,		"FOLLOW_UNIT_VIEWER_SCALE",			pEffectData->m_bFollowUnitViewerScale,			false	);
		LUA_GET_VALUE( luaManager,		"USE_EFFECT_SCALE_IN_UNIT_VIEWER",	pEffectData->m_bUseEffectScaleInUnitViewer,		false	);
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE


		LUA_GET_VALUE( luaManager,		"APPLY_INSTANCE_SCALE",		pEffectData->m_bApplyInstanceScale,			true	);	// 주의!! templet의 scale을 적용할지 말지 정하는 것이 아니라, instance에 지정된 scale을 적용할지 말지 결정하는 변수 입니다

		LUA_GET_VALUE( luaManager,		"SCALE_X",					pEffectData->m_vScale.x,					0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Y",					pEffectData->m_vScale.y,					0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Z",					pEffectData->m_vScale.z,					0.f		);


		LUA_GET_VALUE( luaManager,		"SCALE_X_ON_LEFT",			pEffectData->m_vScaleOnLeft.x,				0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Y_ON_LEFT",			pEffectData->m_vScaleOnLeft.y,				0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Z_ON_LEFT",			pEffectData->m_vScaleOnLeft.z,				0.f		);



	
		LUA_GET_VALUE( luaManager,		"START_WHEN_TRACE_TARGET_DEAD",		pEffectData->m_bStartWhenTraceTargetDead,		false	);
		LUA_GET_VALUE( luaManager,		"PASSIVE_EFFECT",		pEffectData->m_bPassiveEffect,		false	);

		LUA_GET_VALUE( luaManager,		"USE_SUB_ATTACK_LIST_SET",		pEffectData->m_bUseSubAttackListSet,		false	);

#ifdef FIX_ICE_HEATER_EVENT
		/// 파티클에 대하여 Set Over UI 기능 추가
		LUA_GET_VALUE( luaManager,		"SET_OVER_UI",					pEffectData->m_bSetOverUI,					false	);
#endif FIX_ICE_HEATER_EVENT

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321e9aa9-321e9c8d: BONE_TRACE_TARGET_MESH_INDEX, int, default -1 (MOV dword [ESI],0xffffffff), no post-processing
		LUA_GET_VALUE( luaManager,		"BONE_TRACE_TARGET_MESH_INDEX",	pEffectData->m_iBoneTraceTargetMeshIndex,	-1		);
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
		/*	
			BONE_TRACE_TARGET_MESH로 설정되어 있는 EffectSet은 절대로 TRACE_MATRIX를 사용할 수 없다.
			다만, TRACE_USER_TARGET_FORCE가 설정되어 있다면 위의 두 기능을 사용할 수 있다.
			BONE_TRACE_TARGET_MESH를 설정하면 해당 이펙트 셋은 무조건 타겟 매시를 Trace 하는데,
			TRACE_USER_TARGET_FORCE 를 통해 해당 유닛을 Trace 할 수 있게 된다.
		*/

		LUA_GET_VALUE( luaManager,		"BONE_TRACE_TARGET_MESH",		pEffectData->m_bBoneTraceTargetMesh,		false	);
#endif // MODIFY_EFFECT_SET_TRACE_BONE

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		LUA_GET_VALUE( luaManager,		"TRACE_USER_TARGET_FORCE",		pEffectData->m_bTraceUserTargetForce,		false	);

		/// BONE_TRACE_TARGET_MESH와 TRACE_MATRIX가 설정 되어 있는데, TRACE_USER_TARGET_FORCE가 설정 되어 있지 않다면 경고 발생
		if ( false  == pEffectData->m_bBoneTraceTargetMesh && true == pEffectData->m_bTraceUserTargetForce )
		{
			/// TRACE_USER_TARGET_FORCE는 BONE_TRACE_TARGET_MESH 에서만 쓸 수 있어요!
			//ASSERT( !"TRACE_USER_TARGET_FORCE is Only Can Use BONE_TRACE_TARGET_MESH !!!" );

		}

		/// BONE_TRACE_TARGET_MESH와 TRACE_MATRIX가 설정 되어 있는데, TRACE_USER_TARGET_FORCE가 설정 되어 있지 않다면 경고 발생
		if ( true  == pEffectData->m_bBoneTraceTargetMesh && true == pEffectData->m_bTraceMatrix &&
			 false == pEffectData->m_bTraceUserTargetForce )
		{
			/// BONE_TRACE_TARGET_MESH는 TRACE_MATRIX를 사용할 수 없어요!
			//ASSERT( !"BONE_TRACE_TARGET_MESH is Do Not Use TRACE_MATRIX !!!" );
		}
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

#ifdef MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE
		float fLimitDistance = -1.f;
		LUA_GET_VALUE( luaManager,		"LIMIT_DISTANCE",				fLimitDistance,				-1.f	);
		if( 0.f < fLimitDistance )
		{
			pEffectData->m_fLimitDistanceSq = fLimitDistance * fLimitDistance;
		}
#endif // MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 FUN_321e48e0 @ 321ea221-321ea326: NOT_ALLOW_MEMO, bool, default false (XOR EBX,EBX @ 321ea308, then MOV [ESI],BL), no post-processing
		LUA_GET_VALUE( luaManager,		"NOT_ALLOW_MEMO",							pEffectData->m_bNotAllowMemo,						false	);	// consumed in Phase 4a
		// June2017 FUN_321e48e0 @ 321ea3ac-321ea634: APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION, int
		// getter, default 0, low byte kept (NEG AL / SBB AL,AL / AND AL,[temp]) - the same
		// read as the templet level's. Unsupported (plan rule 5): logged once per templet.
		LUA_GET_VALUE( luaManager,		"APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION",	pEffectData->m_iApplyEffectAlphaConditionByOption,	0		);
		if( 0 != pEffectData->m_iApplyEffectAlphaConditionByOption && false == bJuneFxEffectAlphaLogged )
		{
			bJuneFxEffectAlphaLogged = true;
			WCHAR wszJuneFx[512] = {0};
			StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX unsupported APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION in %s EFFECT%d: effect-alpha option absent (value %d)",
				pEffectSetData->m_wstrEffectSetName.c_str(), iEffectTableIndex, pEffectData->m_iApplyEffectAlphaConditionByOption );
			ErrorLogMsg( XEM_ERROR142, wszJuneFx );
		}

		// June2017 FUN_321e48e0 @ 321ea63a-321ea64b: which sub-tables are read depends on
		// EFFECT_TYPE. 1 (ET_DAMAGE_EFFECT) reads APPLY_DAMAGE_EFFECT_VELOCITY; 2..5 (the four
		// ET_MESH_PLAYER_*) read MESH_TRIGGER_DATA_LIST and then MESH_TRIGGER_DATA; every other
		// type reads neither. June's and March's EFFECT_TYPE tables number these alike.
		if( ET_DAMAGE_EFFECT == pEffectData->m_eEffectType )
		{
			// June2017 FUN_321e48e0 @ 321ebf7c-321ec080: APPLY_DAMAGE_EFFECT_VELOCITY, a
			// positional table. [1],[2],[3]: float by index (getter 32fea820), default 1 each
			// (FLD1). [4]: int by index (32fea4e0), default 0. June builds one entry (ctor
			// 321f2a60) holding ( [4] == 0 ), a type tag of 1 and the three floats, and pushes
			// it (321f3690). What [4] and the tag mean is Phase 4a's to read at the consumer.
			if( true == luaManager.BeginTable( "APPLY_DAMAGE_EFFECT_VELOCITY" ) )
			{
				pEffectData->m_bApplyDamageEffectVelocity = true;
				if( false == luaManager.GetValue( 1, pEffectData->m_vApplyDamageEffectVelocity.x ) )
					pEffectData->m_vApplyDamageEffectVelocity.x = 1.f;
				if( false == luaManager.GetValue( 2, pEffectData->m_vApplyDamageEffectVelocity.y ) )
					pEffectData->m_vApplyDamageEffectVelocity.y = 1.f;
				if( false == luaManager.GetValue( 3, pEffectData->m_vApplyDamageEffectVelocity.z ) )
					pEffectData->m_vApplyDamageEffectVelocity.z = 1.f;
				int iVelocity4th = 0;
				if( false == luaManager.GetValue( 4, iVelocity4th ) )
					iVelocity4th = 0;
				pEffectData->m_bApplyDamageEffectVelocity4thIsZero = ( 0 == iVelocity4th );
				luaManager.EndTable();	// APPLY_DAMAGE_EFFECT_VELOCITY
			}
		}
		else if( ET_MESH_PLAYER_UI_MAJOR <= pEffectData->m_eEffectType && ET_MESH_PLAYER_GAME_MINOR >= pEffectData->m_eEffectType )
		{
			// June2017 FUN_321e48e0 @ 321ea682-321ebbec: MESH_TRIGGER_DATA_LIST. Each
			// TRIGGER_DATA%d is built on a fresh entry (ctor 321f3be0, whose defaults
			// MeshTriggerData() carries), pushed, and the list is stored on the effect.
			if( true == luaManager.BeginTable( "MESH_TRIGGER_DATA_LIST" ) )
			{
				int iTriggerDataIndex = 0;
				while( true == luaManager.BeginTable( "TRIGGER_DATA", iTriggerDataIndex ) )
				{
					MeshTriggerData kTriggerData;
					// @ 321ea70a-321ea7f5: TRIGGER_TIME, float, default -1 ([334380f0])
					LUA_GET_VALUE( luaManager,		"TRIGGER_TIME",			kTriggerData.m_fTriggerTime,			-1.f	);
					// @ 321ea83b-321ea91d: MOTION_NAME, string, default L""
					LUA_GET_VALUE( luaManager,		"MOTION_NAME",			kTriggerData.m_wstrMotionName,			L""		);
					// @ 321ea95c-321eaa20: PLAY_TYPE, int, default 2; binary-only
					LUA_GET_VALUE( luaManager,		"PLAY_TYPE",			kTriggerData.m_iPlayType,				2		);
					// @ 321eaa27-321eaa99: BOUND_MOTION_TYPE_LIST, int by index (32fea4e0) from 1,
					// each pushed, until the first index that does not read
					if( true == luaManager.BeginTable( "BOUND_MOTION_TYPE_LIST" ) )
					{
						int iBoundMotionType = 0;
						for( int iBoundIndex = 1; true == luaManager.GetValue( iBoundIndex, iBoundMotionType ); ++iBoundIndex )
						{
							kTriggerData.m_vecBoundMotionType.push_back( iBoundMotionType );
							iBoundMotionType = 0;
						}
						luaManager.EndTable();	// BOUND_MOTION_TYPE_LIST
					}
					// @ 321eaafb-321eaf47: three bools, default false each (STOP_TRACING_ON_TRIGGER and
					// TRIGGER_ONCE store AL=0; BLOCK_TRIGGER stores BL after XOR BL,BL @ 321eaed6)
					LUA_GET_VALUE( luaManager,		"STOP_TRACING_ON_TRIGGER",	kTriggerData.m_bStopTracingOnTrigger,	false	);
					LUA_GET_VALUE( luaManager,		"TRIGGER_ONCE",				kTriggerData.m_bTriggerOnce,			false	);
					LUA_GET_VALUE( luaManager,		"BLOCK_TRIGGER",			kTriggerData.m_bBlockTrigger,			false	);
					// @ 321eaf49-321eb095: BLOCK_TRIGGER_TIME, float, default -1
					LUA_GET_VALUE( luaManager,		"BLOCK_TRIGGER_TIME",		kTriggerData.m_fBlockTriggerTime,		-1.f	);
					// @ 321eb09c-321eb2f3: TRIGGER_EFFECTSET_LIST, entries by index (BeginTable(int)
					// 32fe99b0) from 1 until one does not open; EFFECT_SET_NAME, string, default
					// L""; only a non-empty name is pushed (CMP [size],0 @ 321eb271)
					if( true == luaManager.BeginTable( "TRIGGER_EFFECTSET_LIST" ) )
					{
						int iEffectSetNameIndex = 1;
						while( true == luaManager.BeginTable( iEffectSetNameIndex ) )
						{
							std::wstring wstrTriggerEffectSetName;
							LUA_GET_VALUE( luaManager,	"EFFECT_SET_NAME",			wstrTriggerEffectSetName,				L""		);
							if( false == wstrTriggerEffectSetName.empty() )
								kTriggerData.m_vecTriggerEffectSetName.push_back( wstrTriggerEffectSetName );
							luaManager.EndTable();	// TRIGGER_EFFECTSET_LIST[iEffectSetNameIndex]
							++iEffectSetNameIndex;
						}
						luaManager.EndTable();	// TRIGGER_EFFECTSET_LIST
					}
					// @ 321eb373-321eb5de: EFFECT_SET_TOGGLE_VISIBILITY_TIMER, float, default -1.
					// Post-processing @ 321eb5c7-321eb5de: a read value other than -1 sets a flag
					// (the ctor clears it); a failed read leaves -1, so the test below matches.
					LUA_GET_VALUE( luaManager,		"EFFECT_SET_TOGGLE_VISIBILITY_TIMER",	kTriggerData.m_fEffectSetToggleVisibilityTimer,	-1.f	);
					if( -1.f != kTriggerData.m_fEffectSetToggleVisibilityTimer )
						kTriggerData.m_bUseEffectSetToggleVisibilityTimer = true;
					// @ 321eb635-321eb78a: USE_MESH_TRANSITION, bool, default TRUE (MOV byte [..],1)
					// for list entries, although the ctor sets false
					LUA_GET_VALUE( luaManager,		"USE_MESH_TRANSITION",		kTriggerData.m_bUseMeshTransition,		true	);
					// @ 321eb792-321ebb80: ANIMTIME_ON_START_GAME / _SQUARE / _UNITVIEWER (the last
					// two binary-only), each ONCE, bool, default false, then TIME, float, default 0
					if( true == luaManager.BeginTable( "ANIMTIME_ON_START_GAME" ) )
					{
						LUA_GET_VALUE( luaManager,	"ONCE",						kTriggerData.m_AnimTimeOnStartGame.m_bOnce,			false	);
						LUA_GET_VALUE( luaManager,	"TIME",						kTriggerData.m_AnimTimeOnStartGame.m_fTime,			0.f		);
						luaManager.EndTable();	// ANIMTIME_ON_START_GAME
					}
					if( true == luaManager.BeginTable( "ANIMTIME_ON_START_SQUARE" ) )
					{
						LUA_GET_VALUE( luaManager,	"ONCE",						kTriggerData.m_AnimTimeOnStartSquare.m_bOnce,		false	);
						LUA_GET_VALUE( luaManager,	"TIME",						kTriggerData.m_AnimTimeOnStartSquare.m_fTime,		0.f		);
						luaManager.EndTable();	// ANIMTIME_ON_START_SQUARE
					}
					if( true == luaManager.BeginTable( "ANIMTIME_ON_START_UNITVIEWER" ) )
					{
						LUA_GET_VALUE( luaManager,	"ONCE",						kTriggerData.m_AnimTimeOnStartUnitViewer.m_bOnce,	false	);
						LUA_GET_VALUE( luaManager,	"TIME",						kTriggerData.m_AnimTimeOnStartUnitViewer.m_fTime,	0.f		);
						luaManager.EndTable();	// ANIMTIME_ON_START_UNITVIEWER
					}

					pEffectData->m_vecMeshTriggerData.push_back( kTriggerData );
					luaManager.EndTable();	// TRIGGER_DATA(iTriggerDataIndex)
					++iTriggerDataIndex;
				}
				luaManager.EndTable();	// MESH_TRIGGER_DATA_LIST
			}

			// June2017 FUN_321e48e0 @ 321ebbf1-321ebf61: MESH_TRIGGER_DATA, the one inline entry
			// (June's EffectData ctor 321f4a20 builds it with 321f3be0). Three keys only:
			// TRIGGER_TIME, float, default -1; MOTION_NAME, string, default L""; PLAY_TYPE,
			// int, default 2 (binary-only).
			if( true == luaManager.BeginTable( "MESH_TRIGGER_DATA" ) )
			{
				LUA_GET_VALUE( luaManager,		"TRIGGER_TIME",			pEffectData->m_MeshTriggerData.m_fTriggerTime,		-1.f	);
				LUA_GET_VALUE( luaManager,		"MOTION_NAME",			pEffectData->m_MeshTriggerData.m_wstrMotionName,	L""		);
				LUA_GET_VALUE( luaManager,		"PLAY_TYPE",			pEffectData->m_MeshTriggerData.m_iPlayType,			2		);
				luaManager.EndTable();	// MESH_TRIGGER_DATA
			}
		}

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		// JUNE_FX ES_EFFECT <set> EFFECT<n> <key>=<v> ..., only for effects that set a June key
		{
			++s_iJuneFxEffectCount;
			WCHAR wszJuneFx[1024] = {0};
			StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX ES_EFFECT %s EFFECT%d", pEffectSetData->m_wstrEffectSetName.c_str(), iEffectTableIndex );
			const size_t uiNameOnly = wcslen( wszJuneFx );
			WCHAR wszKey[256] = {0};
			if( false == pEffectData->m_wstrGroupName.empty() )
			{
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" GROUP_NAME=%s", pEffectData->m_wstrGroupName.c_str() );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( true == pEffectData->m_bZOffsetUseUnitIsRight )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" ZOFFSET_USE_UNIT_IS_RIGHT=1" );
			if( true == pEffectData->m_bTraceMatrixExcludeScale )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" TRACE_MATRIX_EXCLUDE_SCALE=1" );
			if( true == pEffectData->m_bTraceMatrixInField )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" TRACE_MATRIX_IN_FIELD=1" );
			if( true == pEffectData->m_bUseIsRightWhenCreate )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" USE_IS_RIGHT_WHEN_CREATE=1" );
			if( true == pEffectData->m_bFollowUnitRotation )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_UNIT_ROTATION=1" );
			if( true == pEffectData->m_bFollowUnitRotationCParticle )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_UNIT_ROTATION_CPARTICLE=1" );
			if( true == pEffectData->m_bFollowUnitViewerScale )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" FOLLOW_UNIT_VIEWER_SCALE=1" );
			if( true == pEffectData->m_bUseEffectScaleInUnitViewer )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" USE_EFFECT_SCALE_IN_UNIT_VIEWER=1" );
			if( -1 != pEffectData->m_iBoneTraceTargetMeshIndex )
			{
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" BONE_TRACE_TARGET_MESH_INDEX=%d", pEffectData->m_iBoneTraceTargetMeshIndex );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( true == pEffectData->m_bNotAllowMemo )
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L" NOT_ALLOW_MEMO=1" );
			if( 0 != pEffectData->m_iApplyEffectAlphaConditionByOption )
			{
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION=%d", pEffectData->m_iApplyEffectAlphaConditionByOption );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( true == pEffectData->m_bApplyDamageEffectVelocity )
			{
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" APPLY_DAMAGE_EFFECT_VELOCITY=(%g,%g,%g,4th_is_zero=%d)",
					pEffectData->m_vApplyDamageEffectVelocity.x, pEffectData->m_vApplyDamageEffectVelocity.y, pEffectData->m_vApplyDamageEffectVelocity.z,
					pEffectData->m_bApplyDamageEffectVelocity4thIsZero ? 1 : 0 );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( false == pEffectData->m_vecMeshTriggerData.empty() )
			{
				const MeshTriggerData& kFirst = pEffectData->m_vecMeshTriggerData[0];
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" MESH_TRIGGER_DATA_LIST=%d first=(%s@%g bound=%d effectsets=%d)",
					static_cast<int>( pEffectData->m_vecMeshTriggerData.size() ), kFirst.m_wstrMotionName.c_str(), kFirst.m_fTriggerTime,
					static_cast<int>( kFirst.m_vecBoundMotionType.size() ), static_cast<int>( kFirst.m_vecTriggerEffectSetName.size() ) );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( false == pEffectData->m_MeshTriggerData.m_wstrMotionName.empty() || -1.f != pEffectData->m_MeshTriggerData.m_fTriggerTime )
			{
				StringCchPrintfW( wszKey, ARRAY_SIZE(wszKey), L" MESH_TRIGGER_DATA=(%s@%g play=%d)", pEffectData->m_MeshTriggerData.m_wstrMotionName.c_str(),
					pEffectData->m_MeshTriggerData.m_fTriggerTime, pEffectData->m_MeshTriggerData.m_iPlayType );
				StringCchCatW( wszJuneFx, ARRAY_SIZE(wszJuneFx), wszKey );
			}
			if( wcslen( wszJuneFx ) != uiNameOnly )
			{
				++s_iJuneFxEffectWithJuneKeyCount;
				ErrorLogMsg( XEM_ERROR142, wszJuneFx );
			}
		}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

		// 이펙트 종류별 script
		switch( pEffectData->m_eEffectType )
		{
		case ET_DAMAGE_EFFECT:
			{
				ASSERT( !pEffectData->m_bUseSubAttackListSet );	/// MeshPlayer만 사용가능
#ifdef FIX_ICE_HEATER_EVENT 
				/// 파티클에 대하여 Set Over UI 기능 추가
				ASSERT( !pEffectData->m_bSetOverUI );			/// Particle만 사용가능
#endif // FIX_ICE_HEATER_EVENT

			} break;
		case ET_MESH_PLAYER_UI_MAJOR:
		case ET_MESH_PLAYER_UI_MINOR:
		case ET_MESH_PLAYER_GAME_MAJOR:
		case ET_MESH_PLAYER_GAME_MINOR:
			{
#ifdef FIX_ICE_HEATER_EVENT 
				/// 파티클에 대하여 Set Over UI 기능 추가
				ASSERT( !pEffectData->m_bSetOverUI );			/// Particle만 사용가능
#endif // FIX_ICE_HEATER_EVENT
			} break;
		case ET_PARTICLE_UI_MAJOR:
		case ET_PARTICLE_UI_MINOR:
		case ET_PARTICLE_GAME_MAJOR:
		case ET_PARTICLE_GAME_MINOR:
			{
				ASSERT( !pEffectData->m_bUseSubAttackListSet );	/// MeshPlayer만 사용가능
#ifdef MODIFY_EFFECT_SET_TRACE_BONE
				ASSERT( !pEffectData->m_bBoneTraceTargetMesh );
#endif //MODIFY_EFFECT_SET_TRACE_BONE
			} break;
		}
		

		pEffectSetData->m_vecpEffectData.push_back( pEffectData );
		luaManager.EndTable();		// EFFECT(iEffectTableIndex)
		++iEffectTableIndex;
		continue;

effect_error_proc:
		SAFE_DELETE( pEffectData );
		luaManager.EndTable();		// EFFECT(iEffectTableIndex)
		++iEffectTableIndex;
	}

	// camera shake
	int iCameraShakeTableIndex = 0;
	while( true == luaManager.BeginTable( "CAMERA_SHAKE", iCameraShakeTableIndex ) )
	{
		CameraShakeData* pCameraShakeData = new CameraShakeData;

		LUA_GET_VALUE_ENUM( luaManager, "SHAKE_TYPE",				pCameraShakeData->m_eShakeType,				CKTDGCamera::SHAKE_TYPE,	CKTDGCamera::DECT_INVALID	);
		if( CKTDGCamera::DECT_INVALID == pCameraShakeData->m_eShakeType )
			goto camera_shake_error_proc;

		LUA_GET_VALUE( luaManager,		"START_TIME",				pCameraShakeData->m_fStartAnimTime,				0.f		);

		/// 카메라 크래시 발생 거리제한
		int iLimitDistance = 0;
		LUA_GET_VALUE( luaManager,		"LIMIT_DISTANCE",			iLimitDistance,			0		);
#ifdef EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE
		if ( IGNORE_DISTANCE == iLimitDistance )	//ignore인 경우에 해당 값을 그대로 대입
			pCameraShakeData->m_iLimitDistanceSq = IGNORE_DISTANCE;
		else if ( 0 != iLimitDistance )	// 입력 값이 있는 경우만 해당 값을 제곱하여 대입
			pCameraShakeData->m_iLimitDistanceSq = iLimitDistance * iLimitDistance;
#else //EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE
		if ( 0 != iLimitDistance )	// 입력 값이 있는 경우만 해당 값을 제곱하여 대입
			pCameraShakeData->m_iLimitDistanceSq = iLimitDistance * iLimitDistance;
#endif //EFFECTSET_CAMERASHAKE_DISTANCE_IGNORE

		LUA_GET_VALUE( luaManager,		"ONE_DIR_SPEED",			pCameraShakeData->m_fOneDirSpeed,				0.f		);
		LUA_GET_VALUE( luaManager,		"ONE_DIR_ACCEL",			pCameraShakeData->m_fOneDirAccel,				0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_GAP",				pCameraShakeData->m_fTwoDirGap,					0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_TIME",				pCameraShakeData->m_fTwoDirTime,				0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_TIME_GAP",			pCameraShakeData->m_fTwoDirTimeGap,				0.05f	);


		LUA_GET_VALUE( luaManager,		"FIXED_LOOK_VEC",			pCameraShakeData->m_bFixedLookVec,				false	);
		LUA_GET_VALUE( luaManager,		"EYE_SHAKE_COUNT",			pCameraShakeData->m_iEyeShakeCount,				0 );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_SPEED_MIN",		pCameraShakeData->m_EyeMoveSpeed.m_Min,			0.f );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_SPEED_MAX",		pCameraShakeData->m_EyeMoveSpeed.m_Max,			0.f );

		LUA_GET_VALUE( luaManager,		"EYE_MOVE_RANGE_MIN",		pCameraShakeData->m_EyeMoveRange.m_Min,			0.f );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_RANGE_MAX",		pCameraShakeData->m_EyeMoveRange.m_Max,			0.f );

		LUA_GET_VALUE( luaManager,		"LOOK_AT_SHAKE_COUNT",		pCameraShakeData->m_iLookAtShakeCount,			0 );
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_SPEED_MIN",	pCameraShakeData->m_LookAtMoveSpeed.m_Min,		0.f	);
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_SPEED_MAX",	pCameraShakeData->m_LookAtMoveSpeed.m_Max,		0.f	);

		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_RANGE_MIN",	pCameraShakeData->m_LookAtMoveRange.m_Min,		0.f	);
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_RANGE_MAX",	pCameraShakeData->m_LookAtMoveRange.m_Max,		0.f	);

		pEffectSetData->m_vecpCameraShakeData.push_back( pCameraShakeData );
		luaManager.EndTable();		// EFFECT(iCameraShakeTableIndex)
		++iCameraShakeTableIndex;
		continue;

camera_shake_error_proc:
		SAFE_DELETE( pCameraShakeData );
		luaManager.EndTable();		// EFFECT(iCameraShakeTableIndex)
		++iCameraShakeTableIndex;
	}

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
	// sound
	// June2017 FUN_321e48e0 @ 321edd29-321ee2a7: SOUND%d, read after CAMERA_SHAKE%d. Each
	// entry is filled and pushed with no rejection path: EVENT_TIME, float, default 0
	// @ 321edde8; FILE_NAME, string, default L"" @ 321edee9; RATE, int, default 100
	// (ESI=0x64) @ 321edfd4, binary-only; USE_GLOBAL_TIME, bool, default false @ 321ee086;
	// LOOP, bool, default false @ 321ee1c6. That is CX2DamageEffect::SoundData exactly. June
	// also presets one more byte of its entry to 1 with no Lua read: runtime state, Phase 4a.
	int iSoundTableIndex = 0;
	while( true == luaManager.BeginTable( "SOUND", iSoundTableIndex ) )
	{
		CX2DamageEffect::SoundData kSoundData;
		LUA_GET_VALUE( luaManager,		"EVENT_TIME",				kSoundData.fEventTime,				0.f		);
		LUA_GET_VALUE( luaManager,		"FILE_NAME",				kSoundData.fileName,				L""		);
		LUA_GET_VALUE( luaManager,		"RATE",						kSoundData.iRate,					100		);
		LUA_GET_VALUE( luaManager,		"USE_GLOBAL_TIME",			kSoundData.bUseGlobalTime,			false	);
		LUA_GET_VALUE( luaManager,		"LOOP",						kSoundData.bLoop,					false	);

		pEffectSetData->m_vecSoundData.push_back( kSoundData );
		luaManager.EndTable();		// SOUND(iSoundTableIndex)
		++iSoundTableIndex;
	}
#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	if( false == pEffectSetData->m_vecSoundData.empty() )
	{
		s_iJuneFxSoundCount += static_cast<int>( pEffectSetData->m_vecSoundData.size() );
		WCHAR wszJuneFx[512] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"JUNE_FX ES_SOUND %s count=%d first=%s@%g rate=%d",
			pEffectSetData->m_wstrEffectSetName.c_str(), static_cast<int>( pEffectSetData->m_vecSoundData.size() ),
			pEffectSetData->m_vecSoundData[0].fileName.c_str(), pEffectSetData->m_vecSoundData[0].fEventTime, pEffectSetData->m_vecSoundData[0].iRate );
		ErrorLogMsg( XEM_ERROR142, wszJuneFx );
	}
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE




	m_mapEffectSetTemplet[ pEffectSetData->m_wstrEffectSetName ] = pEffectSetData;

#ifdef EFFECT_TOOL
	m_vecEffectSetTemplet.push_back( pEffectSetData );
#endif //EFFECT_TOOL
	return true;

effect_set_error_proc:
	SAFE_DELETE( pEffectSetData );
	return false;
}






//////////////////////////////////////////////////////////////////////////
CX2EffectSet::EffectInstance::~EffectInstance()
{
	switch( m_pEffectData->m_eEffectType )
	{
	default:
		{
			ASSERT( !"no type effect" );
		} break;

	case ET_DAMAGE_EFFECT:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetDamageEffect() )
			{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
                if ( m_hDamageEffect != INVALID_DAMAGE_EFFECT_HANDLE )
                    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffect );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
				if( NULL != m_pDamageEffect )
					g_pX2Game->GetDamageEffect()->DestroyInstance( m_pDamageEffect );
                m_pDamageEffect = NULL;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
			}
		} break;

	case ET_MESH_PLAYER_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pData->GetUIMinorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pX2Game->GetMinorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_PARTICLE_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pData->GetUIMinorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;
	}
}



//////////////////////////////////////////////////////////////////////////
CX2EffectSet::EffectSetInstance::~EffectSetInstance()
{
	
	//if( NULL != g_pX2Game )
	//{
	//	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqStickShake );
	//}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    m_hHandle = INVALID_EFFECTSET_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	s_HandleManager.ReleaseHandle( m_hHandle );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	BOOST_TEST_FOREACH( EffectInstance*, pEffectInstance, m_vecpEffectInstance )
	{
		SAFE_DELETE( pEffectInstance );
	}
	m_vecpEffectInstance.clear();

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321e3a80 -> FUN_321da200: every sound the set started is stopped and closed
	for( UINT iSound = 0; iSound < m_vecpPlayingSound.size(); ++iSound )
	{
		CKTDXDeviceSound* pSound = m_vecpPlayingSound[iSound];
		if( NULL != pSound )
		{
			pSound->Stop();
			SAFE_CLOSE( pSound );
		}
	}
	m_vecpPlayingSound.clear();
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
}

#ifdef SERV_PET_SYSTEM
CX2EffectSet::EffectSetInstance::EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit, CX2PET *pPet)
#else
CX2EffectSet::EffectSetInstance::EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit )
#endif
: m_hHandle( INVALID_EFFECTSET_HANDLE )
, m_pXSkinAnim( pSkinAnim )
, m_optrGameUnit( pGameUnit )
, m_optrTraceTargetGameUnit( pTraceTargetGameUnit )
, m_pSquareUnit( pSquareUnit )
, m_bHyper( false )
, m_bCustomPosition( false )
, m_vPosition( 0, 0, 0 )		
, m_vRotationDegree( 0, 0, 0 )
, m_vDirVector( 0, 0, 0 )
, m_vZVector( 0, 0, 0 )
, m_fDelayTime( 0.f )
, m_fPowerRate( 0.f )
, m_fPowerRateScale( 1.f )
, m_fLifeTime( 0.f )
, m_vScale( 1, 1, 1 )
, m_bDamageEffectScale( false )
, m_pEffectSetData( pEffectSetData ) 
, m_fPrevElapsedTime( 0.f )
, m_fElapsedTime( 0.f )
, m_bUseAnimationTiming( true )
, m_iCurrDeleteShakeCount( 0 )
#ifdef SERV_PET_SYSTEM
, m_pPet( pPet )
#endif
, m_vLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, 0.f) )
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
, m_eTargetMeshEffectType(ET_INVALID)
, m_hTargetMesh ( INVALID_MESH_INSTANCE_HANDLE )
, m_bIsTargetMesh( false ) 
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
, m_bIsTargetMeshRight( false ) 
#endif // SERV_ARA_CHANGE_CLASS_SECOND
#ifdef FIX_TARGET_MESH_OFFSET_POS // 김태환
, m_vecTargetMeshDirVector( D3DXVECTOR3(1.f, 0.f, 0.f) )
, m_vecTargetMeshZVector( D3DXVECTOR3(1.f, 0.f, 0.f) )
#endif // FIX_TARGET_MESH_OFFSET_POS
#ifdef TRACE_UNIT_DIE_DELETE_EFFECTSET
, m_bCreateEffectSetTraceUnit( false )
, m_bTraceUnitDieDeleteEffectSet( true )
#endif //TRACE_UNIT_DIE_DELETE_EFFECTSET
//, m_hSeqStickShake( INVALID_PARTICLE_SEQUENCE_HANDLE )
#ifdef ADD_RENA_SYSTEM //김창한
, m_eFirstAttack( CX2DamageManager::FAC_NONE )
#endif //ADD_RENA_SYSTEM
#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
, m_vBoneTraceTargetMeshScale( 1.f, 1.f, 1.f )
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE
{
//	if( NULL == pEffectSetData )
//		return;

// #ifdef SERV_PET_SYSTEM
// 	if( pPet == NULL && pSkinAnim == NULL )
// 		return;
// #else
// 	if( pSkinAnim == NULL )
// 		return;
// #endif

	if( NULL != pGameUnit )
	{
		m_eMode = ESM_GAME;
	}
	else if( pSquareUnit != NULL )
	{
		m_eMode = ESM_FIELD;
	}
#ifdef SERV_PET_SYSTEM
	else if( pPet != NULL )
	{
		m_eMode = ESM_COMMON;
	}
#endif
	else
	{
		m_eMode = ESM_FIELD;
		//return;
	}

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321df230 @ 321df51c-321df576, alongside the mode: the creator's facing is kept for
	// USE_IS_RIGHT_WHEN_CREATE. The game unit's GetIsRight() (vfunc 0x170), else the square unit's
	// (+0x3d5), else the pet's (+0x439), else false.
	m_bIsRightWhenCreate = false;
	if( NULL != pGameUnit )
		m_bIsRightWhenCreate = pGameUnit->GetIsRight();
	else if( pSquareUnit != NULL )
		m_bIsRightWhenCreate = pSquareUnit->GetIsRight();
	#ifdef SERV_PET_SYSTEM
	else if( pPet != NULL )
		m_bIsRightWhenCreate = pPet->GetIsRight();
	#endif SERV_PET_SYSTEM

	// June2017 FUN_321df230 @ 321df323 clears the stop-tracing byte (+0x54) that STOP_TRACING_ON_TRIGGER sets
	m_bStopTracingOnTrigger = false;
	// ... sets the show flag (+0xdc, @ 321df3e3) and clears the sound copy flag (+0x181, @ 321df6f8)
	m_bShowObject = true;
	m_bSoundPlayDataCopied = false;
	m_bToggleVisibilityPending = false;
	m_fToggleVisibilityTimer = 0.f;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME


	m_vecEffectInstanceCreated.resize(0);
	for( UINT i=0; i<pEffectSetData->m_vecpEffectData.size(); i++ )
	{
		m_vecEffectInstanceCreated.push_back( false );
	}

	m_vecCameraShakeDone.resize(0);
	for( UINT i=0; i<pEffectSetData->m_vecpCameraShakeData.size(); i++ )
	{
		m_vecCameraShakeDone.push_back( false );
	}

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    m_hHandle = INVALID_EFFECTSET_HANDLE;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_hHandle = s_HandleManager.GetNextValidHandle();
	s_HandleManager.AssignHandle( m_hHandle, this );
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
}

#ifdef GET_EFFECTSET_POSITION_IN_LUA
D3DXVECTOR3 CX2EffectSet::GetEffectPosition_LUA( CX2EffectSet::Handle hHandle, int iEffectIndex )
{
	D3DXVECTOR3 vPos( 0, 0, 0 );

	EffectSetInstance* pEffectSetInstance = GetEffectSetInstance( hHandle );
	if( pEffectSetInstance == NULL )
		return vPos;

	EffectData* pEffectData = pEffectSetInstance->m_pEffectSetData->m_vecpEffectData[iEffectIndex];
	if( pEffectData == NULL )
		return vPos;

	vPos = pEffectSetInstance->GetEffectPosition( pEffectData );

	return vPos;
}
#endif GET_EFFECTSET_POSITION_IN_LUA

D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectPosition( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vPos(0, 0, 0);

	if( NULL == pEffectData )
		return vPos;

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321dcd30 @ 321dcd9d-321dce07 picks the target-mesh index. An effect that is
	// itself a BONE_TRACE_TARGET_MESH uses BONE_TRACE_TARGET_MESH_INDEX as it is, so the
	// default -1 traces no mesh; any other effect clamps the index to 0. FUN_321dcac0 (see
	// GetEffectPositionByTargetMesh) follows, and on success the offset (FUN_321d9820) and
	// return.
	{
		int iTargetMeshIndex = pEffectData->m_iBoneTraceTargetMeshIndex;
		if( false == pEffectData->m_bBoneTraceTargetMesh && iTargetMeshIndex < 0 )
			iTargetMeshIndex = 0;

		if( true == GetEffectPositionByTargetMesh( vPos, pEffectData, iTargetMeshIndex ) )
		{
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			if( 0 < pEffectData->m_iBoneTraceTargetMeshIndex )
			{
				WCHAR wszJuneFx[64] = {0};
				StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"position_from_index=%d", iTargetMeshIndex );
				JuneFxEsCreate( this, pEffectData, L"BONE_TRACE_TARGET_MESH_INDEX", wszJuneFx );
			}
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
			SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			return vPos;
		}
	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		// March traced m_hTargetMesh here even for an effect that is itself a target mesh
		if( true == pEffectData->m_bBoneTraceTargetMesh && 0 > pEffectData->m_iBoneTraceTargetMeshIndex &&
			INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
			JuneFxEsCreate( this, pEffectData, L"BONE_TRACE_TARGET_MESH", L"self_traces_no_mesh(321dcd9d)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
	if( true == GetEffectPositionByTargetMesh( vPos, pEffectData ) )
	{
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		return vPos;
	}
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

	if( true == m_bCustomPosition )
	{
		vPos = m_vPosition;
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		return vPos;
	}

#ifdef EFFECT_TOOL
	SetEffectPoistion_EffectTool( pEffectData, vPos );
	return vPos;
#endif // EFFECT_TOOL


	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return vPos;

			if( null != m_optrTraceTargetGameUnit )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					if( 0 == pEffectData->m_PositionBoneName.compare( L"_HEAD_BONE" ) )
					{
						vPos = m_optrTraceTargetGameUnit->GetHeadBonePos();
					}
					else
					{
						m_optrTraceTargetGameUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos );
					}
				}
				else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
				{
					vPos = m_optrTraceTargetGameUnit->GetWeaponBonePos( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
				}
				else 
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_optrTraceTargetGameUnit->GetLandPos();
					}
					else
					{
						vPos = m_optrTraceTargetGameUnit->GetPos();
					}
				}

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			}
			else if( null != m_optrGameUnit )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
	#ifdef MODIFY_GET_BONE_POS
					if( 0 == pEffectData->m_PositionBoneName.compare( L"_HEAD_BONE" ) )
					{
						vPos = m_optrGameUnit->GetHeadBonePos();
					}
					// 없는 본에 대한 참조를 시도 한다면 헤드본으로 연결 하도록 수정
					else if( false == m_optrGameUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos ) )
					{
						vPos = m_optrGameUnit->GetHeadBonePos();
					}
	#else
					if( 0 == pEffectData->m_PositionBoneName.compare( L"_HEAD_BONE" ) )
					{
						vPos = m_optrGameUnit->GetHeadBonePos();
					}
					else
					{
					vPos = m_optrGameUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str() );
					}
	#endif //MODIFY_GET_BONE_POS
				}
				else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
				{
					vPos = m_optrGameUnit->GetWeaponBonePos( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
				}
#ifdef RIDING_SYSTEM
				else if ( NULL != GetOwnerUnit() && NULL != GetOwnerUnit()->GetRidingPet() && false == pEffectData->m_PosRidingPetBoneName.empty() )
				{
					GetOwnerUnit()->GetRidingPet()->GetBonePos( pEffectData->m_PosRidingPetBoneName.c_str(), vPos ) ;
				}
#endif //RIDING_SYSTEM
				else 
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_optrGameUnit->GetLandPos();
					}
					else
					{
						vPos = m_optrGameUnit->GetPos();
					}
				}

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			}
			else 
			{
				return vPos;
			}
		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{

				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					vPos = m_pSquareUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str() );
				}
				else 
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_pSquareUnit->GetLandPosition();
					}
					else
					{
						vPos = m_pSquareUnit->GetPos();
					}
				}

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			}
			
		} break;

#ifdef SERV_PET_SYSTEM
	case ESM_COMMON:
		{
			if( m_pPet != NULL )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					vPos = m_pPet->GetBonePos( pEffectData->m_PositionBoneName.c_str() );
				}
				else 
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_pPet->GetLandPosition_LUA();
					}
					else
					{
						vPos = m_pPet->GetPos();
					}
				}

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			}
		}
		break;
#endif
	}

	return vPos;
}


D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectScale( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vScale( 1, 1, 1 );

	if( 0.f != pEffectData->m_vScale.x )
	{
		vScale.x *= pEffectData->m_vScale.x;
		vScale.y *= pEffectData->m_vScale.y;
		vScale.z *= pEffectData->m_vScale.z;
	}

	if( 0.f != pEffectData->m_vScaleOnLeft.x &&
		false == IsUnitRight() )
	{
		vScale.x *= pEffectData->m_vScaleOnLeft.x;
		vScale.y *= pEffectData->m_vScaleOnLeft.y;
		vScale.z *= pEffectData->m_vScaleOnLeft.z;
	}

	//if( true == pEffectData->m_bApplyInstanceScale ) : 원본
	// UnitScale의 중복 곱을 피하기 위해 Effectset을 타고온 DamageEffect는 UnitScale을 적용하지 않는다.
	if( m_bDamageEffectScale == true || (true == pEffectData->m_bApplyInstanceScale && pEffectData->m_eEffectType != ET_DAMAGE_EFFECT) )	
	{
		vScale.x *= m_vScale.x;
		vScale.y *= m_vScale.y;
		vScale.z *= m_vScale.z;
	}
	
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	// June2017 FUN_321dddb0 @ 321dde5f-321dde89: unless NOT_ALLOW_MEMO, the scale is multiplied by the memo scale
	// APPLIED_SKILL_ID sets (EffectData +0x108, copied from the instance @ 321eede8). March has no memo scale,
	// so the key changes nothing here; it is logged only (plan rule 5).
	if( true == pEffectData->m_bNotAllowMemo )
		JuneFxUnsupported( m_pEffectSetData, L"NOT_ALLOW_MEMO", L"", L"applied-skill-ID system", 1, L"321dde5f" );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	return vScale;
}



D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectRotation( EffectData* pEffectData )
{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	KTDXPROFILE();

	// June2017 FUN_321df7e0 @ 321df80c-321df860: for rotation the target-mesh index is
	// BONE_TRACE_TARGET_MESH_INDEX clamped to 0. The entry counts only if it is in range with
	// a live handle, and not when the effect is itself a BONE_TRACE_TARGET_MESH with a negative
	// index.
	const int iTargetMeshIndex = ( pEffectData->m_iBoneTraceTargetMeshIndex < 0 ) ? 0 : pEffectData->m_iBoneTraceTargetMeshIndex;
	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	EFFECT_TYPE eTargetMeshEffectType = ET_INVALID;
	const bool bTargetMesh = !( pEffectData->m_iBoneTraceTargetMeshIndex < 0 && true == pEffectData->m_bBoneTraceTargetMesh ) &&
							 true == GetTargetMesh( iTargetMeshIndex, hTargetMesh, eTargetMeshEffectType );

	D3DXVECTOR3 vEffectRotation = pEffectData->m_vRotateRel;
	// @ 321df860-321df8f4: APPLY_UNIT_ROTATION adds the target mesh's rotation, or the unit's
	// when there is no target mesh or TRACE_USER_TARGET_FORCE is set
	if( true == pEffectData->m_bApplyUnitRotation )
	{
		if( true == bTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh( iTargetMeshIndex );
			if( NULL != pMeshInstance )
				vEffectRotation += pMeshInstance->GetRotateDegree();
		}
		else
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}
	}
	// @ 321df8f9-321df98c: APPLY_UNIT_ROTATION_MIRROR does the same, then flips x when not right
	else if( true == pEffectData->m_bApplyUnitRotationMirror )
	{
		if( true == bTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh( iTargetMeshIndex );
			if( NULL != pMeshInstance )
				vEffectRotation += pMeshInstance->GetRotateDegree();
		}
		else
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}

		if( false == IsUnitRight() )
			vEffectRotation.x *= -1.f;
	}

	// @ 321df98c: the local rotation
	vEffectRotation += m_vLocalRotateDegree;

	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	if( true == bTargetMesh && 0 < pEffectData->m_iBoneTraceTargetMeshIndex &&
		( true == pEffectData->m_bApplyUnitRotation || true == pEffectData->m_bApplyUnitRotationMirror ) )
	{
		WCHAR wszJuneFx[64] = {0};
		StringCchPrintfW( wszJuneFx, ARRAY_SIZE(wszJuneFx), L"rotation_from_index=%d", iTargetMeshIndex );
		JuneFxEsCreate( this, pEffectData, L"BONE_TRACE_TARGET_MESH_INDEX", wszJuneFx );
	}
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG

	return vEffectRotation;
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	KTDXPROFILE();

	D3DXVECTOR3 vEffectRotation = pEffectData->m_vRotateRel;
	if( true == pEffectData->m_bApplyUnitRotation )
	{
#ifdef MODIFY_EFFECT_SET_TRACE_BONE
	#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
	#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh )
	#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
			if( NULL != pMeshInstance )
			{
				vEffectRotation += pMeshInstance->GetRotateDegree();
			}
		}
		else
#endif //MODIFY_EFFECT_SET_TRACE_BONE
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}
	}
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
	/// 회전값 설정시 좌 우 완벽한 대칭을 이루기 위해, 방향에 따른 x회전 값 반전 기능 추가
	else if ( true == pEffectData->m_bApplyUnitRotationMirror )
	{
#ifdef MODIFY_EFFECT_SET_TRACE_BONE
	#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
	#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh )
	#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
			if( NULL != pMeshInstance )
			{
				vEffectRotation += pMeshInstance->GetRotateDegree();
			}
		}
		else
#endif //MODIFY_EFFECT_SET_TRACE_BONE
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}

		if( false == IsUnitRight() )
		{
			vEffectRotation.x *= -1.f;
		}
	}
#endif // SERV_ARA_CHANGE_CLASS_SECOND

	vEffectRotation += m_vLocalRotateDegree;

	return vEffectRotation;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
}

D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetUnitRotation( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vRotation( 0, 0, 0 );

#ifdef ADD_PET_NINE_TAIL_FOX		/// 펫에도 APPLY_UNIT_ROTATION 적용되도록 수정
	if( NULL == m_pPet && NULL == pEffectData )
#else  ADD_PET_NINE_TAIL_FOX
	if( NULL == pEffectData )
#endif ADD_PET_NINE_TAIL_FOX
		return vRotation;

#ifdef MODIFY_EFFECT_SET_TRACE_BONE

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
	if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
	if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh )
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
		if( NULL != pMeshInstance )
		{
			pMeshInstance->GetRotateDegree();
		}
	}

#endif //MODIFY_EFFECT_SET_TRACE_BONE

	if( true == m_bCustomPosition )
	{
		return m_vRotationDegree;
	}

	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return vRotation;

			if( null != m_optrTraceTargetGameUnit )
			{
				return m_optrTraceTargetGameUnit->GetRotateDegree();
			}
			else if ( null != m_optrGameUnit )
			{
				return m_optrGameUnit->GetRotateDegree();
			}
			else
				return vRotation;

		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{
				return m_pSquareUnit->GetDirDegree();
			}
		
		} break;
#ifdef SERV_PET_SYSTEM
	case ESM_COMMON:
		{
			if( m_pPet != NULL )
			{
				return m_pPet->GetDirDegree();
			}
		} break;
#endif
	}

	return vRotation;
}


bool CX2EffectSet::EffectSetInstance::IsUnitRight()
{
	KTDXPROFILE();

	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return false;

			if( null != m_optrTraceTargetGameUnit )
			{
				return m_optrTraceTargetGameUnit->GetIsRight();
			}
			else if( null != m_optrGameUnit )
			{
				return m_optrGameUnit->GetIsRight();
			}
			else
				return false;

		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{
				if( NULL == g_pTFieldGame )
					return false;

				return m_pSquareUnit->GetIsRight();
			}
		} break;
	}

	return false;
}

//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
/** @function : GetCombineMatrix
	@brief : June2017 FUN_321dc550. March's owner branch with the target-mesh index rule, plus the bone
			 frame of a square unit and of a pet, where March returned NULL; so TRACE_MATRIX (and a
			 GROUP_NAME child's offset) follows those holders too. The frame's combineMatrix (+0x90), or NULL.
*/
const D3DXMATRIX* CX2EffectSet::EffectSetInstance::GetCombineMatrix( const EffectData* pEffectData ) const
{
	// @ 321dc56e-321dc789: a game-mode set with a live owner
	if( ESM_GAME == m_eMode && null != m_optrGameUnit )
	{
		if( false == pEffectData->m_PositionBoneName.empty() )
		{
			// @ 321dc5c0-321dc6b2: the target mesh under the index rule, unless TRACE_USER_TARGET_FORCE. With
			// the game and its major mesh player up, a mesh or bone that is not found gives no matrix;
			// otherwise the owner's bone is used.
			const int iTargetMeshIndex = ( pEffectData->m_iBoneTraceTargetMeshIndex < 0 ) ? 0 : pEffectData->m_iBoneTraceTargetMeshIndex;
			CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
			EFFECT_TYPE eTargetMeshEffectType = ET_INVALID;
			if( false == ( pEffectData->m_iBoneTraceTargetMeshIndex < 0 && true == pEffectData->m_bBoneTraceTargetMesh ) &&
				true == GetTargetMesh( iTargetMeshIndex, hTargetMesh, eTargetMeshEffectType ) &&
				false == pEffectData->m_bTraceUserTargetForce &&
				NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( hTargetMesh );
				if( NULL == pMesh )
					return NULL;
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = pMesh->GetCloneFrame( pEffectData->m_PositionBoneName.c_str() );
				return ( NULL != pMultiAnimFrame ) ? &pMultiAnimFrame->combineMatrix : NULL;
			}
			return m_optrGameUnit->GetCombineMatrixFromBoneName( pEffectData->m_PositionBoneName );
		}
		else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromWeaponBoneName( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
		}
	#ifdef RIDING_SYSTEM
		else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromRidingPetBoneName( pEffectData->m_PosRidingPetBoneName );
		}
	#endif //RIDING_SYSTEM
		return NULL;
	}

	// @ 321dc78e-321dc7d1: a square unit's BONE_NAME, on its unit viewer's skin anim
	if( m_pSquareUnit != NULL )
	{
		if( NULL == m_pSquareUnit->GetUnitViewer() || NULL == m_pSquareUnit->GetUnitViewer()->GetXSkinAnim() )
			return NULL;
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pSquareUnit->GetUnitViewer()->GetXSkinAnim()->GetCloneFrame( pEffectData->m_PositionBoneName.c_str() );
		return ( NULL != pMultiAnimFrame ) ? &pMultiAnimFrame->combineMatrix : NULL;
	}
#ifdef SERV_PET_SYSTEM
	// @ 321dc7d3-321dc81d: a pet's BONE_NAME. June tests m_bLoaded (+0x510) and the skin anim (+0x2a4),
	// which is March's GetXSkinAnim().
	if( NULL != m_pPet )
	{
		if( NULL == m_pPet->GetXSkinAnim() )
			return NULL;
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pPet->GetXSkinAnim()->GetCloneFrame( pEffectData->m_PositionBoneName.c_str() );
		return ( NULL != pMultiAnimFrame ) ? &pMultiAnimFrame->combineMatrix : NULL;
	}
#endif SERV_PET_SYSTEM
	// @ 321dc81f-321dc8c1: two more holders (a riding pet, +0xd8, and +0xe0) that March's instance does not have
	return NULL;
}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
const D3DXMATRIX* CX2EffectSet::EffectSetInstance::GetCombineMatrix( const EffectData* pEffectData ) const
{
	if( ESM_GAME == m_eMode && null != m_optrGameUnit )
	{
		if( false == pEffectData->m_PositionBoneName.empty() )
		{
			/// BONE_TRACE_TARGET_MESH가 설정 되어 있다면, 해당 매시의 본을 사용
	#ifdef MODIFY_EFFECT_SET_TRACE_BONE
		#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
			if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce && 
				 NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
		#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
		#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );

				if ( NULL != pMesh  )
				{
					CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = pMesh->GetCloneFrame( pEffectData->m_PositionBoneName.c_str() );

					if ( NULL != pMultiAnimFrame )
						return &pMultiAnimFrame->combineMatrix;
				}
			}
			else
	#endif // MODIFY_EFFECT_SET_TRACE_BONE	
				return m_optrGameUnit->GetCombineMatrixFromBoneName( pEffectData->m_PositionBoneName );			
		}
		else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromWeaponBoneName( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );			
		}
#ifdef RIDING_SYSTEM
		else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromRidingPetBoneName( pEffectData->m_PosRidingPetBoneName );
		}
#endif //RIDING_SYSTEM
	}

	return NULL;
}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

CX2GameUnit* CX2EffectSet::EffectSetInstance::GetOwnerUnit() const
{
	{
		if( null != m_optrGameUnit )
			return m_optrGameUnit.GetObservable();
		else if( NULL != m_pPet )
			return m_pPet->GetMaster();
		else
			return NULL;
	}
}

/*static*/ D3DXVECTOR3 CX2EffectSet::GetZVector( const D3DXVECTOR3& vDirVector )
{
	D3DXVECTOR3 vZVector = vDirVector;
	if( vZVector.y != 0.f )
	{
		vZVector.y = vZVector.z;
		vZVector.z = vZVector.x;
		vZVector.x = -vZVector.y;
		vZVector.y = 0.f;
		D3DXVec3Normalize( &vZVector, &vZVector );
	}
	else
	{
		vZVector.y = vZVector.z;
		vZVector.z = vZVector.x;
		vZVector.x = -vZVector.y;
		vZVector.y = 0.f;
	}
	return vZVector;
}

#ifdef CREATEINSTANCE_WITH_LIFETIME_IN_LUA
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetWithLifetime_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, float fLifeTime  )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit, NULL, false, -1.f, fLifeTime  );
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetWithLifetimePos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, float fLifeTime, D3DXVECTOR3 vPosition  )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );
	D3DXVECTOR3 vScale( 1.0, 1.0, 1.0 );
	return PlayEffectSet( wstrEffectSetName, pGameUnit, NULL, false, -1.f, fLifeTime, vScale, true, vPosition  );
}
#endif //CREATEINSTANCE_WITH_LIFETIME_IN_LUA

//{{ kimhc // 2010.4.19 // 비밀던전 작업
#ifdef SERV_SECRET_HELL
CX2EffectSet::Handle CX2EffectSet::PlayEffectSet_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit );
}
#ifdef VELDER_BOSS_5
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos , D3DXVECTOR3 vRotationDegree)
#else
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos )
#endif VELDER_BOSS_5
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

#ifdef VELDER_BOSS_5
	return PlayEffectSet( wstrEffectSetName, pGameUnit, NULL, false, -1.0f, -1.0f, D3DXVECTOR3( 1, 1, 1 ), 
		true, vPos, vRotationDegree);
#else
	return PlayEffectSet( wstrEffectSetName, pGameUnit, NULL, false, -1.0f, -1.0f, D3DXVECTOR3( 1, 1, 1 ), 
		true, vPos );
#endif VELDER_BOSS_5
}

//{{ JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시
#ifdef SEASON3_MONSTER_2010_12
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetTraceTarget_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit, pTraceTargetGameUnit );
}
#endif SEASON3_MONSTER_2010_12
//}} JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시

void CX2EffectSet::StopEffectSet_LUA( CX2EffectSet::Handle hHandle )
{
	StopEffectSet( hHandle );
}
#endif SERV_SECRET_HELL
//}} kimhc // 2010.4.19 // 비밀던전 작업


#ifdef EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가
bool CX2EffectSet::MergeEffectSetTemplet( EffectSetData* pNewEffectSetTemplet_, EffectSetData* pOrgEffectSetTemplet_ )
{
	if ( NULL != pNewEffectSetTemplet_ ) 
	{
		if ( NULL != pOrgEffectSetTemplet_ )
		{
			m_mapEffectSetTemplet.erase( pOrgEffectSetTemplet_->m_wstrEffectSetName );
			KLOG("DevScriptTable_Log.txt")  << L"Merge : " << pNewEffectSetTemplet_->m_wstrEffectSetName.c_str() << L"\t: EffectSetTemplet 을 변경 하였습니다." << fileout;
		}
		else
		{
			KLOG("DevScriptTable_Log.txt")  << L"Merge : " << pNewEffectSetTemplet_->m_wstrEffectSetName.c_str() << L"\t: EffectSetTemplet 을 추가 하였습니다." << fileout;
		}
		m_mapEffectSetTemplet[pNewEffectSetTemplet_->m_wstrEffectSetName] = pNewEffectSetTemplet_;
		return true;
	}
	return false;
}
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가

#if defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)
// #ifdef EFFECT_TOOL

bool CX2EffectSet::CreateEffectSetTemplet( EffectSetData* pEffectSetData_ )
{
	// 중복 체크.
	EffectSetDataMap::iterator it = m_mapEffectSetTemplet.find( pEffectSetData_->m_wstrEffectSetName );
	if( it != m_mapEffectSetTemplet.end() )
	{
		return false;
	}

	EffectSetData * pNewEffectSetData = new EffectSetData();
	(*pNewEffectSetData ) = (*pEffectSetData_);
	// 맵 추가
	m_mapEffectSetTemplet[ pNewEffectSetData->m_wstrEffectSetName ] = pNewEffectSetData;
	// 벡터 추가.
#ifdef EFFECT_TOOL
	m_vecEffectSetTemplet.push_back( pEffectSetData_ );
#endif // EFFECT_TOOL
	return true;
}

#endif // defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

#ifdef EFFECT_TOOL
bool CX2EffectSet::DeleteEffectSetTemplet( const wstring& wstrEffectSetName_ )
{
	// 존재 여부 체크.
	EffectSetDataMap::iterator mit = m_mapEffectSetTemplet.find( wstrEffectSetName_ );
	if( mit == m_mapEffectSetTemplet.end() )
	{
		return false;
	}

	//맵 제거.
	m_mapEffectSetTemplet.erase( mit );

	//벡터 제거
	std::vector< EffectSetData* >::iterator it = m_vecEffectSetTemplet.begin();
	for( ; it != m_vecEffectSetTemplet.end(); ++it )
	{
		if( (*it)->m_wstrEffectSetName == wstrEffectSetName_ )
		{
			m_vecEffectSetTemplet.erase( it );
			break;
		}
	}

	return true;
}
bool CX2EffectSet::ChangeNameEffectSetTemplet( const wstring& wstrEffectSetName_,  const wstring& wstrChangeEffectSetName_ )
{
	// 존재 여부 체크.
	EffectSetDataMap::iterator mit = m_mapEffectSetTemplet.find( wstrEffectSetName_ );
	if( mit == m_mapEffectSetTemplet.end() )
	{
		return false;
	}
	// 벡터 이름 변경
	std::vector< EffectSetData* >::iterator it = m_vecEffectSetTemplet.begin();
	for( ; it != m_vecEffectSetTemplet.end(); ++it )
	{
		if( (*it)->m_wstrEffectSetName == wstrEffectSetName_ )
		{
			(*it)->m_wstrEffectSetName = wstrChangeEffectSetName_;
			break;
		}
	}

	// 맵 추가.
	// 변경 전 이름의 key값을 가지고 있는 맵은 NULL 가리키도록 변경.	
	m_mapEffectSetTemplet[wstrChangeEffectSetName_] = mit->second;
	mit->second = NULL;
		
	return true;
}

void CX2EffectSet::AddEffectSet_Description_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	m_mapEffectSetDescription;

	wstring wstrEffectSetName;
	LUA_GET_VALUE( luaManager,		"NAME",   wstrEffectSetName,			L""		);

	wstring wstrEffectSetDesc;
	luaManager.GetValue( "DESC", wstrEffectSetDesc);

	m_mapEffectSetDescription[wstrEffectSetName] = wstrEffectSetDesc;
}
void CX2EffectSet::SetEffectSetVersion_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	m_mapEffectSetDescription;

	LUA_GET_VALUE( luaManager,		"VERSION",   m_wstrToolVersion,			L""		);
}
wstring CX2EffectSet::GetEffectSetDesc( const WCHAR* pEffectSetName_ )
{
	if( m_mapEffectSetDescription.find( pEffectSetName_ ) != m_mapEffectSetDescription.end() )
	{
		return m_mapEffectSetDescription[pEffectSetName_];
	}

	return L"";
}
void CX2EffectSet::AddEffectSet_Description( wstring wstrEffectSetName_, wstring wstrEffectSetDesc_ )
{
	m_mapEffectSetDescription[wstrEffectSetName_] = wstrEffectSetDesc_;
}

void CX2EffectSet::EffectSetInstance::SetEffectPoistion_EffectTool( IN const EffectData* pEffectData_, OUT D3DXVECTOR3& vPos_ )
{
	wstring wstrTargetBoneName = L"";
	if( false == pEffectData_->m_PosRidingPetBoneName.empty() )
	{
		wstrTargetBoneName.assign( pEffectData_->m_PosRidingPetBoneName );
	}
	else if( false == pEffectData_->m_PositionWeaponBoneName.empty() )
	{
		wstrTargetBoneName.assign( pEffectData_->m_PositionWeaponBoneName );
	}
	else
	{
		wstrTargetBoneName.assign( pEffectData_->m_PositionBoneName );
	}

	if( NULL == g_pX2Game )
		return;


	if( null != m_optrGameUnit )
	{
		if( false == wstrTargetBoneName.empty() )
		{
			if( 0 == wstrTargetBoneName.compare( L"_HEAD_BONE" ) ||
				false == m_optrGameUnit->GetBonePos( wstrTargetBoneName.c_str(), vPos_ ))
			{
				vPos_ = m_optrGameUnit->GetHeadBonePos();
			}
		}
		else 
		{
			if( true == pEffectData_->m_bLandPosition )
			{
				vPos_ = m_optrGameUnit->GetLandPos();
			}
			else
			{
				vPos_ = m_optrGameUnit->GetPos();
			}
		}

		SetEffectSetOffset( vPos_, pEffectData_->m_vPositionOffset );
	}
}
#else
	#ifdef EXPAND_DEVELOPER_SCRIPT
	bool CX2EffectSet::DeleteEffectSetTemplet( EffectSetData* pEffectSetData_ )
	{
		// 존재 여부 체크.
		EffectSetDataMap::iterator mit = m_mapEffectSetTemplet.find( pEffectSetData_->m_wstrEffectSetName );
		if( mit == m_mapEffectSetTemplet.end() )
		{
			return false;
		}

		//맵 제거.
		m_mapEffectSetTemplet.erase( mit );

		return true;
	}
	#endif // EXPAND_DEVELOPER_SCRIPT
#endif //EFFECT_TOOL

/** @function : SetEffectSetOffset
	@brief : 이펙트셋에 설정된 오프셋값을 출력 위치에 적용 시켜 주는 함수
	@param : vPos_  ( 현재 위치에 대한 위치값. OUT 인자)
	         vOffsetPos_ ( EffectSet에 담겨있는 오프셋 정보 )
*/

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
void CX2EffectSet::EffectSetInstance::SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_, IN const EffectData* pEffectData_ )
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
void CX2EffectSet::EffectSetInstance::SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_ )
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
{
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
	if ( NULL == pEffectData_ )
		return;

	// June2017 FUN_321d9820 @ 321d9847-321d9889: the unit whose facing orients the offset is the
	// trace target when that is valid, else the owner. March always used the owner.
	CX2GameUnit* pFacingUnit = ( null != m_optrTraceTargetGameUnit ) ? m_optrTraceTargetGameUnit.GetObservable() : m_optrGameUnit.GetObservable();
	// @ 321d9889-321d9896: the target-mesh index is max( BONE_TRACE_TARGET_MESH_INDEX, 0 )
	const int iTargetMeshIndex = ( pEffectData_->m_iBoneTraceTargetMeshIndex < 0 ) ? 0 : pEffectData_->m_iBoneTraceTargetMeshIndex;
	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	EFFECT_TYPE eTargetMeshEffectType = ET_INVALID;
	const bool bTargetMesh = true == GetTargetMesh( iTargetMeshIndex, hTargetMesh, eTargetMeshEffectType ) &&
							 false == pEffectData_->m_bTraceUserTargetForce;

	if( NULL != pFacingUnit ) // ESM_GAME
	{
		// @ 321d98a3-321d98c2: June guards its target-mesh branch here with CMP index,count /
		// JBE, which skips it for every in-range index. Only an index past the end of the list
		// (an out-of-bounds read, not reproduced) would enter it. So June always offsets a game
		// unit's effect by the unit's facing, even one that traces a target mesh. March offset
		// those by the target mesh's dir / Z (FIX_TARGET_MESH_OFFSET_POS).

		// X, @ 321d9a43-321d9b08
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = pFacingUnit->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == pFacingUnit->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		// Y, @ 321d9b0b-321d9b23
		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		// Z, @ 321d9b26-321d9c2a. ZOFFSET_USE_UNIT_IS_RIGHT (@ 321d9b3a) makes the Z offset follow
		// the facing: it is subtracted while the unit faces left.
		if( 0.f != vOffsetPos_.z )
		{
			if( true == pEffectData_->m_bZOffsetUseUnitIsRight && false == pFacingUnit->GetIsRight() )
				vPos_ -= pFacingUnit->GetZVector() * vOffsetPos_.z;
			else
				vPos_ += pFacingUnit->GetZVector() * vOffsetPos_.z;
		}

	#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
		if( true == pEffectData_->m_bZOffsetUseUnitIsRight && 0.f != vOffsetPos_.z )
			JuneFxEsCreate( this, pEffectData_, L"ZOFFSET_USE_UNIT_IS_RIGHT", true == pFacingUnit->GetIsRight() ? L"z_added(321d9b3a)" : L"z_subtracted(321d9b3a)" );
		if( pFacingUnit != m_optrGameUnit.GetObservable() && ( 0.f != vOffsetPos_.x || 0.f != vOffsetPos_.z ) )
			JuneFxEsCreate( this, pEffectData_, L"POSITION_OFFSET", L"facing=trace_target(321d9847)" );
		if( true == bTargetMesh && ( 0.f != vOffsetPos_.x || 0.f != vOffsetPos_.y || 0.f != vOffsetPos_.z ) )
			JuneFxEsCreate( this, pEffectData_, L"BONE_TRACE_TARGET_MESH", L"offset=unit_facing(321d98c0)" );
	#endif SERV_IRUHADEV_JUNE_FX_DEBUG
	}
	else if ( null != m_pSquareUnit ) // ESM_FIELD, @ 321d9c2f-321d9d43, as March
	{
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pSquareUnit->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pSquareUnit->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pSquareUnit->GetZVector() * vOffsetPos_.z;
		}
	}
	else if( NULL != m_pPet ) // ESM_COMMON, @ 321d9d48-321d9e65, as March
	{
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pPet->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pPet->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pPet->GetZVector() * vOffsetPos_.z;
		}
	}
	// @ 321d9e6a-321d9f0f: no unit, square or pet. A live target-mesh entry at the index,
	// without TRACE_USER_TARGET_FORCE, gets the plain add, as March.
	else if ( true == bTargetMesh )
	{
		vPos_.x += (0.f != vOffsetPos_.x) ? vOffsetPos_.x : 0.f;
		vPos_.y += (0.f != vOffsetPos_.y) ? vOffsetPos_.y : 0.f;
		vPos_.z += (0.f != vOffsetPos_.z) ? vOffsetPos_.z : 0.f;
	}
#else SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
	if ( NULL == pEffectData_ )
		return;
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

	if( null != m_optrGameUnit ) // ESM_GAME
	{
#ifdef SERV_ARA_CHANGE_CLASS_SECOND
		/// 타겟 매시가 설정되어 있다면, 타겟 매시의 방향에 따라 OffSet 위치를 적용한다.
	#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh &&
			  false == pEffectData_->m_bTraceUserTargetForce )
	#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh )
	#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		{
	#ifdef FIX_TARGET_MESH_OFFSET_POS // 김태환
			if ( false == IsSamef( vOffsetPos_.x, 0.f ) )
			{
				/// 방향벡터의 Y 값을 제거
				D3DXVECTOR3 vDirVector = m_vecTargetMeshDirVector;

				if( 0.f != vDirVector.y )
				{
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}

				/// 방향에 따른 OffSet X 설정
				vPos_ += m_bIsTargetMeshRight ? ( vOffsetPos_.x * vDirVector ) : ( -vOffsetPos_.x * vDirVector );
			}

			/// 방향에 따른 OffSet Y 설정
			if ( false == IsSamef( vOffsetPos_.y, 0.f ) )
				vPos_.y += vOffsetPos_.y;

			/// 방향에 따른 OffSet Z 설정
			if ( false == IsSamef( vOffsetPos_.z, 0.f ) )
				vPos_ += vOffsetPos_.z * m_vecTargetMeshZVector;
	#else // FIX_TARGET_MESH_OFFSET_POS
			if ( false == IsSamef( vOffsetPos_.x, 0.f ) )
				vPos_.x += m_bIsTargetMeshRight ? vOffsetPos_.x : -vOffsetPos_.x;

			if ( false == IsSamef( vOffsetPos_.y, 0.f ) )
				vPos_.y += m_bIsTargetMeshRight ? vOffsetPos_.y : -vOffsetPos_.y;

			if ( false == IsSamef( vOffsetPos_.z, 0.f ) )
				vPos_.z += m_bIsTargetMeshRight ? vOffsetPos_.z : -vOffsetPos_.z;
	#endif // FIX_TARGET_MESH_OFFSET_POS
		}
		else
#endif //SERV_ARA_CHANGE_CLASS_SECOND
		{
			// X축 오프셋 적용
			if( 0.f != vOffsetPos_.x )
			{
				D3DXVECTOR3 vDirVector = m_optrGameUnit->GetDirVector();
				if( 0.f != vDirVector.y )
				{
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}

				if( true == m_optrGameUnit->GetIsRight() )
					vPos_ += vDirVector * vOffsetPos_.x;
				else
					vPos_ -= vDirVector * vOffsetPos_.x;
			}

			// Y축 오프셋 적용
			if( 0.f != vOffsetPos_.y )
			{
				vPos_.y += vOffsetPos_.y;
			}

			// Z축 오프셋 적용
			if( 0.f != vOffsetPos_.z )
			{
				vPos_ += m_optrGameUnit->GetZVector() * vOffsetPos_.z;
			}
		}
	}
	else if ( null != m_pSquareUnit ) // ESM_FIELD
	{
		// X축 오프셋 적용
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pSquareUnit->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pSquareUnit->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		// Y축 오프셋 적용
		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		// Z축 오프셋 적용
		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pSquareUnit->GetZVector() * vOffsetPos_.z;
		}
	}
	else if( NULL != m_pPet ) // ESM_COMMON
	{
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pPet->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pPet->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pPet->GetZVector() * vOffsetPos_.z;
		}
	}
#ifdef MODIFY_EFFECT_SET_TRACE_BONE

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
	else if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData_->m_bTraceUserTargetForce )
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
	else if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh)
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
	{
		vPos_.x += (0.f != vOffsetPos_.x) ? vOffsetPos_.x : 0.f;
		vPos_.y += (0.f != vOffsetPos_.y) ? vOffsetPos_.y : 0.f;
		vPos_.z += (0.f != vOffsetPos_.z) ? vOffsetPos_.z : 0.f;
	}

#endif //MODIFY_EFFECT_SET_TRACE_BONE
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
}

#ifdef FIX_ICE_HEATER_EVENT

/** @function	: PlayEffectSetByMeshPlayer
	@brief		: 매시에 이펙트 셋을 설정하는 함수
	@param		: 이펫트셋 이름(wstrEffectSetName). 매시 플레이어 객체 핸들(hTargetMesh), 메쉬 이펙트 타입(eTargetMeshType)
	@return		: 이펙트셋 핸들 값
*/

CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByMeshPlayer( const WCHAR* wstrEffectSetName, 
															CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh, EFFECT_TYPE	eTargetMeshType )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( INVALID_MESH_INSTANCE_HANDLE == hTargetMesh)
		return INVALID_EFFECTSET_HANDLE;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
    if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
    {
        if ( m_coInstanceHandleList.storage_size() >= 0x10000 )
        {
            return INVALID_EFFECTSET_HANDLE;
        }
        m_coInstanceHandleList.push_back_default(LIST_FREE);
        iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
    }
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	
	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, CKTDGXSkinAnimPtr(), NULL, NULL, CX2SquareUnitPtr(), NULL );
#ifndef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	if( INVALID_EFFECTSET_HANDLE == pEffectSetInstance->m_hHandle )
	{
		SAFE_DELETE( pEffectSetInstance );
		return INVALID_EFFECTSET_HANDLE;
	}
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	// 타겟 메쉬 정보 설정
	pEffectSetInstance->m_hTargetMesh = hTargetMesh;
	pEffectSetInstance->m_eTargetMeshEffectType = eTargetMeshType;
	pEffectSetInstance->m_bIsTargetMesh = true;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    m_coInstanceHandleList.splice( m_coInstanceHandleList.end(LIST_LIVE), iterEmpty );
    iterEmpty->m_pInstance = pEffectSetInstance;
    WORD   wIndex = (WORD) iterEmpty.GetIndex();
    Handle handle;
    DWORD   dwHandle = 0;
    do
    {
        ++iterEmpty->m_wStamp;
        dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        handle.SetValue( (int) dwHandle );
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        handle = (int) dwHandle;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    } while ( handle == INVALID_EFFECTSET_HANDLE );
    pEffectSetInstance->SetHandle( handle );
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	m_vecpEffectSetInstance.push_back( pEffectSetInstance );	
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE	
	return pEffectSetInstance->m_hHandle;
}

#endif // FIX_ICE_HEATER_EVENT


#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
/** @function : GetMeshInstanceByTargetMesh
	@brief : TargetMesh 의 메쉬 인스턴스 객체 포인터 얻기
			 (타입에 따라 얻어오는 방법이 다르기 때문에 함수로 뺐음)
*/
CKTDGXMeshPlayer::CXMeshInstance* CX2EffectSet::EffectSetInstance::GetMeshInstanceByTargetMesh()
{
	if( INVALID_MESH_INSTANCE_HANDLE == m_hTargetMesh ||
		ET_INVALID == m_eTargetMeshEffectType )
		return NULL;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
	switch( m_eTargetMeshEffectType )
	{
	case ET_MESH_PLAYER_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorXMeshPlayer() )
			{
				pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorXMeshPlayer() )
			{
				pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	default:
		break;
	}

	return pMeshInstance;
}
bool CX2EffectSet::EffectSetInstance::GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData )
{
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
	if( INVALID_MESH_INSTANCE_HANDLE == m_hTargetMesh ||
		true == pEffectData->m_bTraceUserTargetForce ||
		ET_INVALID == m_eTargetMeshEffectType )
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
	if( INVALID_MESH_INSTANCE_HANDLE == m_hTargetMesh ||
		ET_INVALID == m_eTargetMeshEffectType )
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		return false;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
	if( NULL != pMeshInstance )
	{
		if( false == pEffectData->m_PositionBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos ) ;
		}
		else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PositionWeaponBoneName.c_str(), vPos ) ;
		}
		else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PosRidingPetBoneName.c_str() );
		}
		else
		{
			vPos = pMeshInstance->GetPos();
		}
	}
	else
	{
		m_hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
		m_eTargetMeshEffectType = ET_INVALID;
		return false;
	}

	return true;
}
#endif // MODIFY_EFFECT_SET_TRACE_BONE

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
/** @function : GetTargetMesh
	@brief : June2017 target-mesh entry iIndex_ (EffectSetInstance +0xf0 in June): true when it
			 is in range with a live handle. When the list is empty but m_hTargetMesh was set
			 directly (PlayEffectSetByMeshPlayer, a March path), that single mesh is entry 0.
*/
bool CX2EffectSet::EffectSetInstance::GetTargetMesh( IN int iIndex_, OUT CKTDGXMeshPlayer::CXMeshInstanceHandle& hTargetMesh_, OUT EFFECT_TYPE& eEffectType_ ) const
{
	hTargetMesh_ = INVALID_MESH_INSTANCE_HANDLE;
	eEffectType_ = ET_INVALID;
	if( iIndex_ < 0 )
		return false;

	if( true == m_vecTargetMeshData.empty() )
	{
		if( 0 != iIndex_ )
			return false;
		hTargetMesh_ = m_hTargetMesh;
		eEffectType_ = m_eTargetMeshEffectType;
	}
	else
	{
		if( iIndex_ >= static_cast<int>( m_vecTargetMeshData.size() ) )
			return false;
		hTargetMesh_ = m_vecTargetMeshData[iIndex_].m_hTargetMesh;
		eEffectType_ = m_vecTargetMeshData[iIndex_].m_eEffectType;
	}

	return INVALID_MESH_INSTANCE_HANDLE != hTargetMesh_;
}

/** @function : InvalidateTargetMesh
	@brief : June2017 FUN_321dcac0, its failure path: a mesh that is gone resets its entry to
			 {-1, 0}. Entry 0 also resets the March single-mesh mirror.
*/
void CX2EffectSet::EffectSetInstance::InvalidateTargetMesh( IN int iIndex_ )
{
	if( iIndex_ >= 0 && iIndex_ < static_cast<int>( m_vecTargetMeshData.size() ) )
	{
		m_vecTargetMeshData[iIndex_].m_hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
		m_vecTargetMeshData[iIndex_].m_eEffectType = ET_INVALID;
	}

	if( 0 == iIndex_ )
	{
		m_hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
		m_eTargetMeshEffectType = ET_INVALID;
	}
}

/** @function : HasTargetMeshHandle
	@brief : June2017 FUN_321e4020 @ 321e4140-321e4180: true while some target-mesh entry still holds a
			 handle. June tests the stored handle (CMP [entry],-1), not whether the mesh lives: an entry
			 becomes -1 only when a position lookup finds its mesh gone (InvalidateTargetMesh). With the
			 list empty, March's single mesh (PlayEffectSetByMeshPlayer) is the one entry, as in GetTargetMesh.
*/
bool CX2EffectSet::EffectSetInstance::HasTargetMeshHandle() const
{
	if( true == m_vecTargetMeshData.empty() )
		return INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh;

	for( UINT iEntry = 0; iEntry < m_vecTargetMeshData.size(); ++iEntry )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != m_vecTargetMeshData[iEntry].m_hTargetMesh )
			return true;
	}
	return false;
}

/** @function : SetShowObject
	@brief : June2017 FUN_321de4f0: every effect of the set is shown or hidden (EffectInstance::SetShowObject,
			 FUN_321de0c0), then the set keeps the flag (+0xdc, @ 321deab1 / 321deac7)
*/
void CX2EffectSet::EffectSetInstance::SetShowObject( bool bShow_ )
{
	for( UINT iEffect = 0; iEffect < m_vecpEffectInstance.size(); ++iEffect )
		JuneSetEffectInstanceShow( m_vecpEffectInstance[iEffect], bShow_ );
	m_bShowObject = bShow_;
}

/** @function : GetMeshInstanceByTargetMesh
	@brief : June2017 FUN_321dc910: March's GetMeshInstanceByTargetMesh() with an index. The
			 mesh instance of entry iIndex_, found through the mesh player its type names.
*/
CKTDGXMeshPlayer::CXMeshInstance* CX2EffectSet::EffectSetInstance::GetMeshInstanceByTargetMesh( IN int iIndex_ )
{
	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	EFFECT_TYPE eEffectType = ET_INVALID;
	if( false == GetTargetMesh( iIndex_, hTargetMesh, eEffectType ) || ET_INVALID == eEffectType )
		return NULL;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
	switch( eEffectType )
	{
	case ET_MESH_PLAYER_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
				pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( hTargetMesh );
		} break;
	case ET_MESH_PLAYER_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorXMeshPlayer() )
				pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance( hTargetMesh );
		} break;
	case ET_MESH_PLAYER_GAME_MAJOR:
		{
			if( NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
				pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( hTargetMesh );
		} break;
	case ET_MESH_PLAYER_GAME_MINOR:
		{
			if( NULL != g_pX2Game && NULL != g_pX2Game->GetMinorXMeshPlayer() )
				pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( hTargetMesh );
		} break;
	default:
		break;
	}

	return pMeshInstance;
}

/** @function : GetEffectPositionByTargetMesh
	@brief : June2017 FUN_321dcac0: March's GetEffectPositionByTargetMesh() with an index. The
			 entry must be live and typed, and the effect must not have TRACE_USER_TARGET_FORCE.
			 Bone order (BONE_NAME, WEAPON_BONE_NAME, RIDING_PET_BONE_NAME, else the mesh
			 position) and the discarded riding-pet result are the same in June and March.
*/
bool CX2EffectSet::EffectSetInstance::GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData, IN int iIndex_ )
{
	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	EFFECT_TYPE eEffectType = ET_INVALID;
	if( false == GetTargetMesh( iIndex_, hTargetMesh, eEffectType ) || ET_INVALID == eEffectType ||
		true == pEffectData->m_bTraceUserTargetForce )
		return false;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh( iIndex_ );
	if( NULL == pMeshInstance )
	{
		InvalidateTargetMesh( iIndex_ );
		return false;
	}

	if( false == pEffectData->m_PositionBoneName.empty() )
	{
		pMeshInstance->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos ) ;
	}
	else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
	{
		pMeshInstance->GetBonePos( pEffectData->m_PositionWeaponBoneName.c_str(), vPos ) ;
	}
	else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
	{
		pMeshInstance->GetBonePos( pEffectData->m_PosRidingPetBoneName.c_str() );
	}
	else
	{
		vPos = pMeshInstance->GetPos();
	}

	return true;
}

/** @function : GetEffectPositionByTargetMesh
	@brief : June2017 FUN_321dcc20: FUN_321dcac0 (above) with a caller-supplied bone in place of BONE_NAME,
			 for GROUP_NAME. With it empty the order is WEAPON_BONE_NAME, then RIDING_PET_BONE_NAME (here the
			 result is kept), else the mesh position. bBoneFound_ is written only by a bone lookup
			 (FUN_321f29e0 = the mesh's GetBonePos( name, out ), KTDGXMeshPlayer.h).
*/
bool CX2EffectSet::EffectSetInstance::GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData, IN int iIndex_,
																		   IN const std::wstring& wstrBoneName_, OUT bool& bBoneFound_ )
{
	CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
	EFFECT_TYPE eEffectType = ET_INVALID;
	if( false == GetTargetMesh( iIndex_, hTargetMesh, eEffectType ) || ET_INVALID == eEffectType ||
		true == pEffectData->m_bTraceUserTargetForce )
		return false;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh( iIndex_ );
	if( NULL == pMeshInstance )
	{
		InvalidateTargetMesh( iIndex_ );
		return false;
	}

	const WCHAR* pwszBoneName = NULL;
	if( false == wstrBoneName_.empty() )
		pwszBoneName = wstrBoneName_.c_str();
	else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		pwszBoneName = pEffectData->m_PositionWeaponBoneName.c_str();
	else if( false == pEffectData->m_PosRidingPetBoneName.empty() )
		pwszBoneName = pEffectData->m_PosRidingPetBoneName.c_str();
	else
	{
		vPos = pMeshInstance->GetPos();
		return true;
	}

	// June does not test the mesh's skin anim here and would fault without one; here it is a missing bone
	if( NULL == pMeshInstance->GetXSkinAnim() )
	{
		vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
		bBoneFound_ = false;
		return true;
	}
	bBoneFound_ = pMeshInstance->GetBonePos( pwszBoneName, vPos );
	return true;
}

/** @function : GetEffectPositionByBoneName
	@brief : June2017 FUN_321dd590: GetEffectPosition (FUN_321dcd30) with a caller-supplied bone in place of
			 BONE_NAME, which also says whether the bone exists. Its two callers are the GROUP_NAME loop of
			 FUN_321ee600 (@ 321ef040) and UpdateEffect (FUN_321e0080 @ 321e0933, Phase 4b). Both pass
			 <base>_GROUPCHILD_<n>, so the empty-name branches of FUN_321dd590 (weapon bone, riding-pet bone,
			 position) are unreachable and not transcribed: an empty name returns (0,0,0), not found. The
			 June-only holders (instance override bone +0x130, UI viewer +0xdd, +0xe0, riding pet +0xd8) are
			 keyless and absent here, as in GetEffectPosition.
*/
D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectPositionByBoneName( IN EffectData* pEffectData, IN const std::wstring& wstrBoneName_, OUT bool& bBoneFound_ )
{
	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	bBoneFound_ = false;		// @ 321dd5fe
	if( NULL == pEffectData || true == wstrBoneName_.empty() )
		return vPos;

	// @ 321dd5ea-321dd674: the target-mesh index as in GetEffectPosition, then FUN_321dcc20; on success the
	// offset (FUN_321d9820 @ 321ddd53) and return
	int iTargetMeshIndex = pEffectData->m_iBoneTraceTargetMeshIndex;
	if( false == pEffectData->m_bBoneTraceTargetMesh && iTargetMeshIndex < 0 )
		iTargetMeshIndex = 0;
	if( true == GetEffectPositionByTargetMesh( vPos, pEffectData, iTargetMeshIndex, wstrBoneName_, bBoneFound_ ) )
	{
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
		return vPos;
	}

	// @ 321dd681-321dd6a6: a custom position; the bone counts as not found
	if( true == m_bCustomPosition )
	{
		vPos = m_vPosition;
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
		return vPos;
	}

	const WCHAR* pwszBoneName = wstrBoneName_.c_str();
	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return vPos;

			if( null != m_optrTraceTargetGameUnit )
			{
				// @ 321dda83-321ddb21: the trace target's bone (unit vfunc 0x80 = GetBonePos( name, out ), as
				// GetEffectPosition's MODIFY_GET_BONE_POS call); no head-bone fallback
				if( 0 == wstrBoneName_.compare( L"_HEAD_BONE" ) )
					vPos = m_optrTraceTargetGameUnit->GetHeadBonePos();
				else
					bBoneFound_ = m_optrTraceTargetGameUnit->GetBonePos( pwszBoneName, vPos );
			}
			else if( null != m_optrGameUnit )
			{
				// @ 321ddb8f-321ddc15: the owner's bone, else its head bone (vfunc 0xfc = GetHeadBonePos), not found
				if( 0 == wstrBoneName_.compare( L"_HEAD_BONE" ) )
					vPos = m_optrGameUnit->GetHeadBonePos();
				else
				{
					bBoneFound_ = m_optrGameUnit->GetBonePos( pwszBoneName, vPos );
					if( false == bBoneFound_ )
						vPos = m_optrGameUnit->GetHeadBonePos();
				}
			}
			else
			{
				return vPos;
			}

			SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
		} break;

	case ESM_FIELD:
		{
			// @ 321dd8a9: the square unit's bone. FUN_321f3020 is the bool form of FUN_321f2f20 (= CX2SquareUnit::
			// GetBonePos, X2SquareUnit.h): its unit viewer's skin anim, else not found.
			if( m_pSquareUnit != NULL )
			{
				if( NULL != m_pSquareUnit->GetUnitViewer() && NULL != m_pSquareUnit->GetUnitViewer()->GetXSkinAnim() )
					bBoneFound_ = m_pSquareUnit->GetUnitViewer()->GetXSkinAnim()->GetCloneFramePosition( pwszBoneName, vPos );
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}
		} break;

#ifdef SERV_PET_SYSTEM
	case ESM_COMMON:
		{
			// @ 321dd72f: the pet's bone. FUN_321f2b80 is the bool form of FUN_31e4d9e0 (= CX2PET::GetBonePos,
			// X2PET.h): its skin anim, else not found.
			if( m_pPet != NULL )
			{
				if( NULL != m_pPet->GetXSkinAnim() )
					bBoneFound_ = m_pPet->GetXSkinAnim()->GetCloneFramePosition( pwszBoneName, vPos );
				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}
		} break;
#endif SERV_PET_SYSTEM
	}

	return vPos;
}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef SERV_9TH_NEW_CHARACTER // 김태환
/** @function	: SetPos
	@brief		: 이펙트 셋의 위치를 강제로 바꾸는 함수 ( Trace가 설정되어 있다면, 바꿔도 원상태로 돌아옴 )
	@param		: 변경 위치
*/
void CX2EffectSet::EffectSetInstance::SetPos( IN const D3DXVECTOR3& vOffsetPos_ )
{
	BOOST_FOREACH( EffectInstance* pEffectInstance, m_vecpEffectInstance )
	{
		if ( NULL == pEffectInstance )
			continue;

		EffectData* pEffectData = pEffectInstance->m_pEffectData;

		if( NULL == pEffectData )
			continue;

		/// Trace가 설정되어 있다면, 동작 시키지 말자.
		if( true == pEffectData->m_bTrace ||
			true == pEffectData->m_bTraceMore ||
			true == pEffectData->m_bTraceMatrix )
		{
			ASSERT( ! "Can Not Change Position!!! This EffectSet is Setting Trace" );
			continue;
		}

		if( false == pEffectInstance->m_bAlive )
			continue;

		switch( pEffectData->m_eEffectType )
		{
		case ET_DAMAGE_EFFECT:
			{
				if ( NULL != g_pX2Game &&
					 NULL != g_pX2Game->GetDamageEffect() )
				{
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect );

					if ( NULL != pEffect )
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					if ( CX2DamageEffect::CEffect* pEffect = pEffectInstance->m_pDamageEffect )
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
					{
						pEffect->SetPos( vOffsetPos_ );
					}
				}

			} break;

		case ET_MESH_PLAYER_UI_MAJOR:				
			{
				if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
					INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( NULL != pInstance )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_MESH_PLAYER_UI_MINOR:
			{
				if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
					INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( NULL != pInstance )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_MESH_PLAYER_GAME_MAJOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( pInstance != NULL )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			}
			break;
		case ET_MESH_PLAYER_GAME_MINOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )						
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( pInstance != NULL )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_PARTICLE_UI_MAJOR:
			{
				if( g_pData->GetUIMajorParticle() != NULL && 
					pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			} break;

		case ET_PARTICLE_UI_MINOR:
			{
				if( g_pData->GetUIMinorParticle() != NULL && 
					pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			} break;

		case ET_PARTICLE_GAME_MAJOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			}
			break;

		case ET_PARTICLE_GAME_MINOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMinorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )						
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}						
			}
			break;
		}		
	}
}
#endif //SERV_9TH_NEW_CHARACTER