#include "StdAfx.h"
#include ".\x2baseunitviewerui.h"

//CX2BaseUnitViewerUI::CX2BaseUnitViewerUI(void)
//{
//}
//
//CX2BaseUnitViewerUI::~CX2BaseUnitViewerUI(void)
//{
//}

#ifdef SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
//////////////////////////////////////////////////////////////////////////
// June effect completion C9c-2b: June2017's unit-viewer effect player, kept on the base viewer class as in
// June (CX2BaseUnitViewerUI, vtable 3347113c). Rows: june2017_re/funcmap.tsv, tags "Effect C9b" / "Effect C9c-2a".
// The viewer runs its own player, not CX2EffectSet::PlayEffectSet: Play 32959640 makes 0x40-byte entries and
// creates nothing, OnFrameMove 329585f0 traces them, creates them (32957980) and plays their sounds (329582b0).

// X2EffectSet.cpp: the no-set variant of June 321dfc40 (mesh trigger runner), called from 329585f0 @32958b12
void JuneRunViewerMeshTriggers( CKTDGXMeshPlayer::CXMeshInstance* pMesh_, CX2EffectSet::EffectData* pEffectData_,
								std::vector<CX2EffectSet::MeshTriggerData>& vecTrigger_, CKTDGXSkinAnim* pSkinAnim_,
								int iCode1_, int iCode2_ );

// June 0x40-byte entry (Play 32959640 @detail). Mesh and particle entries share +0x3c in June; two handle fields here.
struct JuneViewerFxEntry
{
	CX2EffectSet::EffectData*							m_pEffectData;		// +0x00
	CKTDGXSkinAnimPtr									m_pXSkinAnim;		// +0x04, ref-counted
	int													m_iGroup;			// +0x08
	CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hTraceTargetMesh;	// +0x0c, -1 = none
	std::wstring										m_wstrSetName;		// +0x10
	std::vector<CX2EffectSet::MeshTriggerData>			m_vecTrigger;		// +0x2c, copy of MESH_TRIGGER_DATA_LIST (meshes)
	CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMesh;			// +0x3c (mesh entries)
	CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeq;				// +0x3c (particle entries)

	JuneViewerFxEntry()
	: m_pEffectData( NULL )
	, m_iGroup( 0 )
	, m_hTraceTargetMesh( INVALID_MESH_INSTANCE_HANDLE )
	, m_hMesh( INVALID_MESH_INSTANCE_HANDLE )
	, m_hSeq( INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
	}
};

// June 0x30 pending sound (ctor 32959970): the templet's SoundData, played 0, group
struct JuneViewerFxSound
{
	CX2DamageEffect::SoundData	m_SoundData;
	bool						m_bPlayed;
	int							m_iGroup;

	JuneViewerFxSound() : m_bPlayed( false ), m_iGroup( 0 ) {}
};

// June 8-byte playing sound { CKTDXDeviceSound*, group } (+0x1c4)
struct JuneViewerFxPlayingSound
{
	CKTDXDeviceSound*	m_pSound;
	int					m_iGroup;

	JuneViewerFxPlayingSound() : m_pSound( NULL ), m_iGroup( 0 ) {}
};

static bool JuneViewerFxIsSamef( float fA_, float fB_ )
{
	return fabs( fA_ - fB_ ) <= 0.0009f;
}

// frame position of a bone on the viewer's skin, 0 when the frame is missing (32957980: combineMatrix _41/_42/_43)
static D3DXVECTOR3 JuneViewerFxFramePos( CKTDGXSkinAnim* pXSkinAnim_, const WCHAR* pwszBone_ )
{
	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	if( NULL == pXSkinAnim_ || NULL == pwszBone_ || L'\0' == pwszBone_[0] )
		return vPos;
	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pXSkinAnim_->GetCloneFrame( pwszBone_ );
	if( NULL == pFrame )
		return vPos;
	vPos.x = pFrame->combineMatrix._41;
	vPos.y = pFrame->combineMatrix._42;
	vPos.z = pFrame->combineMatrix._43;
	return vPos;
}

// 31cb2510: BONE_NAME, the riding pet's bone when BONE_NAME is empty
static const WCHAR* JuneViewerFxBoneName( CX2EffectSet::EffectData* pEffectData_ )
{
#ifdef RIDING_SYSTEM
	if( true == pEffectData_->m_PositionBoneName.empty() )
		return pEffectData_->m_PosRidingPetBoneName.c_str();
#endif RIDING_SYSTEM
	return pEffectData_->m_PositionBoneName.c_str();
}

// 31d2b080( skin, 2 ): the lowest combineMatrix _42 over every frame of the skin (March has no frame list accessor; the
// frame tree holds the same frames)
static void JuneViewerFxLowestFrameY( CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_, float& fMinY_, bool& bFound_ )
{
	while( NULL != pFrame_ )
	{
		if( false == bFound_ || pFrame_->combineMatrix._42 < fMinY_ )
		{
			fMinY_ = pFrame_->combineMatrix._42;
			bFound_ = true;
		}
		JuneViewerFxLowestFrameY( static_cast<CKTDXDeviceXSkinMesh::MultiAnimFrame*>( pFrame_->pFrameFirstChild ), fMinY_, bFound_ );
		pFrame_ = static_cast<CKTDXDeviceXSkinMesh::MultiAnimFrame*>( pFrame_->pFrameSibling );
	}
}

// 32957170( out vec3, bUseLand, skin ): the viewer's position; with bUseLand and a skin, y = the skin's lowest bone
D3DXVECTOR3 CX2BaseUnitViewerUI::JuneViewerFxLandPosition( bool bUseLand_, CKTDGXSkinAnim* pXSkinAnim_ )
{
	D3DXVECTOR3 vPos = GetMatrix().GetPos();
	if( true == bUseLand_ && NULL != pXSkinAnim_ )
	{
		float fMinY = 0.f;
		bool bFound = false;
		JuneViewerFxLowestFrameY( pXSkinAnim_->GetCloneFrameRoot(), fMinY, bFound );
		if( true == bFound )
			vPos.y = fMinY;
	}
	return vPos;
}

// 31fae120
CX2BaseUnitViewerUI::CX2BaseUnitViewerUI(void)
: m_bJuneFxShow( false )
, m_bJuneFxLandDirty( false )
, m_iJuneFxTriggerCode1( 0 )
, m_iJuneFxTriggerCode2( 0 )
{
}

// 31fae1a0: stop group 1, then every sound (group 5), then free the four vectors. The entries of the other groups
// have no handle to release in June (the derived dtor 31fab920 stopped groups 0..4 first), so they are only freed.
CX2BaseUnitViewerUI::~CX2BaseUnitViewerUI(void)
{
	JuneViewerFxStop( 1 );
	JuneViewerFxStopSounds( 5 );

	for( size_t i = 0; i < m_vecJuneFxMesh.size(); ++i )
		delete m_vecJuneFxMesh[i];
	m_vecJuneFxMesh.clear();
	for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
		delete m_vecJuneFxParticle[i];
	m_vecJuneFxParticle.clear();
}

// 32959640( skin, set name, group ): the templet is the UI effect set's; nothing is created here (32957980 creates)
void CX2BaseUnitViewerUI::JuneViewerFxPlay( CKTDGXSkinAnimPtr pXSkinAnim_, const std::wstring& wstrSetName_, int iGroup_ )
{
#ifdef SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET
	// June 32959640 @32959680: the first test is the skin anim (TEST EBX,EBX; JZ 3295987e = the tail, which does nothing for it).
	// C9c-2b's port lacked it; the per-emotion call of CX2UnitViewerUI::PlayEmotion passes the skin anim unchecked.
	if( NULL == pXSkinAnim_ )
		return;
#endif SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET
	if( NULL == g_pData || NULL == g_pData->GetUIEffectSet() )
		return;

	CX2EffectSet::EffectSetData* pSetData = g_pData->GetUIEffectSet()->GetEffectSetTemplet( wstrSetName_ );
	if( NULL == pSetData )
	{
		// June dereferences the templet without a test; a missing set is logged once instead of faulting
		static std::set<std::wstring> s_setLogged;
		if( true == s_setLogged.insert( wstrSetName_ ).second )
		{
			WCHAR wszLog[512] = {0};
			StringCchPrintfW( wszLog, ARRAY_SIZE(wszLog), L"JUNE_FX UNIT_VIEWER_FX_PLAY %s NOT PLAYED: the set is not in UIEffectSet.lua (data036 not repacked?)", wstrSetName_.c_str() );
			ErrorLogMsg( XEM_ERROR142, wszLog );
		}
		return;
	}

	int iMeshCount = 0;
	int iParticleCount = 0;
	for( UINT iEffect = 0; iEffect < pSetData->m_vecpEffectData.size(); ++iEffect )
	{
		CX2EffectSet::EffectData* pEffectData = pSetData->m_vecpEffectData[iEffect];
		if( NULL == pEffectData )
			continue;

		// only the UI-major mesh player and particle system are taken, every other EFFECT_TYPE is skipped
		if( CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR == pEffectData->m_eEffectType )
		{
			JuneViewerFxEntry* pEntry = new JuneViewerFxEntry;
			pEntry->m_pEffectData = pEffectData;
			pEntry->m_pXSkinAnim = pXSkinAnim_;
			pEntry->m_iGroup = iGroup_;
			pEntry->m_wstrSetName = wstrSetName_;
			pEntry->m_vecTrigger = pEffectData->m_vecMeshTriggerData;
			m_vecJuneFxMesh.push_back( pEntry );
			m_bJuneFxShow = true;
			++iMeshCount;
		}
		else if( CX2EffectSet::ET_PARTICLE_UI_MAJOR == pEffectData->m_eEffectType )
		{
			JuneViewerFxEntry* pEntry = new JuneViewerFxEntry;
			pEntry->m_pEffectData = pEffectData;
			pEntry->m_pXSkinAnim = pXSkinAnim_;
			pEntry->m_iGroup = iGroup_;
			pEntry->m_wstrSetName = wstrSetName_;
			m_vecJuneFxParticle.push_back( pEntry );
			m_bJuneFxShow = true;
			++iParticleCount;
		}
	}

	// the templet's SOUND%d list (+0x70): each copied into a pending sound (32959970 / 32959e40)
	for( UINT iSound = 0; iSound < pSetData->m_vecSoundData.size(); ++iSound )
	{
		JuneViewerFxSound* pSound = new JuneViewerFxSound;
		pSound->m_SoundData = pSetData->m_vecSoundData[iSound];
		pSound->m_iGroup = iGroup_;
		m_vecJuneFxPendingSound.push_back( pSound );
	}

	// June @3295987e then points the BONE_TRACE_TARGET_MESH links at the entries' own +0x3c, which is still -1 here
	// (nothing is created yet): a no-op, so nothing is stored. The links are made by the create pass.

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
	WCHAR wszPlay[512] = {0};
	StringCchPrintfW( wszPlay, ARRAY_SIZE(wszPlay), L"JUNE_FX UNIT_VIEWER_FX_PLAY %s group=%d meshes=%d particles=%d sounds=%d",
		wstrSetName_.c_str(), iGroup_, iMeshCount, iParticleCount, static_cast<int>( pSetData->m_vecSoundData.size() ) );
	ErrorLogMsg( XEM_ERROR142, wszPlay );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
}

// 329594c0( group ): destroys one group's meshes and particles, then its sounds. There is no "all" value here.
void CX2BaseUnitViewerUI::JuneViewerFxStop( int iGroup_ )
{
	if( NULL != g_pData && NULL != g_pData->GetUIMajorXMeshPlayer() )
	{
		for( size_t i = 0; i < m_vecJuneFxMesh.size(); )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxMesh[i];
			if( NULL != pEntry && iGroup_ == pEntry->m_iGroup )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != pEntry->m_hMesh )
					g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( pEntry->m_hMesh );
				delete pEntry;
				m_vecJuneFxMesh.erase( m_vecJuneFxMesh.begin() + i );
			}
			else
				++i;
		}
	}

	if( NULL != g_pData && NULL != g_pData->GetUIMajorParticle() )
	{
		for( size_t i = 0; i < m_vecJuneFxParticle.size(); )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
			if( NULL != pEntry && iGroup_ == pEntry->m_iGroup )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != pEntry->m_hSeq )
					g_pData->GetUIMajorParticle()->DestroyInstanceHandle( pEntry->m_hSeq );
				delete pEntry;
				m_vecJuneFxParticle.erase( m_vecJuneFxParticle.begin() + i );
			}
			else
				++i;
		}
	}

	JuneViewerFxStopSounds( iGroup_ );
}

// 32959250( group ): group 5 = every sound; any other group stops and drops that group's sounds only
void CX2BaseUnitViewerUI::JuneViewerFxStopSounds( int iGroup_ )
{
	for( size_t i = 0; i < m_vecJuneFxPlayingSound.size(); )
	{
		JuneViewerFxPlayingSound* pPlaying = m_vecJuneFxPlayingSound[i];
		if( NULL == pPlaying || 5 == iGroup_ || iGroup_ == pPlaying->m_iGroup )
		{
			if( NULL != pPlaying )
			{
				if( NULL != pPlaying->m_pSound )
				{
					pPlaying->m_pSound->Stop();
					SAFE_CLOSE( pPlaying->m_pSound );
				}
				delete pPlaying;
			}
			m_vecJuneFxPlayingSound.erase( m_vecJuneFxPlayingSound.begin() + i );
		}
		else
			++i;
	}

	for( size_t i = 0; i < m_vecJuneFxPendingSound.size(); )
	{
		JuneViewerFxSound* pPending = m_vecJuneFxPendingSound[i];
		if( NULL == pPending || 5 == iGroup_ || iGroup_ == pPending->m_iGroup )
		{
			delete pPending;
			m_vecJuneFxPendingSound.erase( m_vecJuneFxPendingSound.begin() + i );
		}
		else
			++i;
	}
}

// 32957980: the create pass, the end of 329585f0. An entry with an effect, a skin and no handle is created once its
// START_TIME is reached, at the bone plus POSITION_OFFSET, with the viewer as owner.
void CX2BaseUnitViewerUI::JuneViewerFxCreatePass()
{
	if( NULL == g_pData )
		return;

	if( NULL != g_pData->GetUIMajorXMeshPlayer() )
	{
		for( size_t i = 0; i < m_vecJuneFxMesh.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxMesh[i];
			if( NULL == pEntry || NULL == pEntry->m_pEffectData || NULL == pEntry->m_pXSkinAnim.get()
				|| INVALID_MESH_INSTANCE_HANDLE != pEntry->m_hMesh )
				continue;

			CX2EffectSet::EffectData* pEffectData = pEntry->m_pEffectData;
			CKTDGXSkinAnim* pXSkinAnim = pEntry->m_pXSkinAnim.get();
			if( false == JuneViewerFxIsSamef( pEffectData->m_fStartAnimTime, 0.f ) && pEffectData->m_fStartAnimTime > pXSkinAnim->GetNowAnimationTime() )
				continue;

			const D3DXVECTOR3 vPos = JuneViewerFxFramePos( pXSkinAnim, pEffectData->m_PositionBoneName.c_str() ) + pEffectData->m_vPositionOffset;
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( this, pEffectData->m_EffectName.c_str(),
				vPos, GetMatrix().GetRotate(), GetMatrix().GetRotate() );
			if( false == g_pData->GetUIMajorXMeshPlayer()->IsLiveInstance( pMesh ) )
			{
				// June retries every frame; the log line is written once per set and effect
				static std::set<std::wstring> s_setLoggedMesh;
				if( true == s_setLoggedMesh.insert( pEntry->m_wstrSetName + L"/" + pEffectData->m_EffectName ).second )
				{
					WCHAR wszLog[512] = {0};
					StringCchPrintfW( wszLog, ARRAY_SIZE(wszLog), L"JUNE_FX UNIT_VIEWER_FX_CREATE %s mesh %s NOT CREATED (not in UIMajorXMeshPlayer.txt?), retried every frame",
						pEntry->m_wstrSetName.c_str(), pEffectData->m_EffectName.c_str() );
					ErrorLogMsg( XEM_ERROR142, wszLog );
				}
				continue;
			}

			pEntry->m_hMesh = pMesh->GetHandle();
			pMesh->SetRotateDegree( pEffectData->m_vRotateRel );
			// June: mesh +0x15c = 0.0 (unidentified, not ported); +0x194 = 0 when the trigger list is non-empty
			if( false == pEntry->m_vecTrigger.empty() )
				pMesh->SetRestoreAnimAfterTrigger( false );

#ifdef MODIFY_EFFECT_SET_TRACE_BONE
			// a mesh that is not itself a BONE_TRACE_TARGET_MESH takes the first live one of its set as its trace target
			if( false == pEffectData->m_bBoneTraceTargetMesh )
			{
				for( size_t iMesh = 0; iMesh < m_vecJuneFxMesh.size(); ++iMesh )
				{
					JuneViewerFxEntry* pMeshEntry = m_vecJuneFxMesh[iMesh];
					if( NULL != pMeshEntry && pMeshEntry != pEntry && NULL != pMeshEntry->m_pEffectData && true == pMeshEntry->m_pEffectData->m_bBoneTraceTargetMesh
						&& pMeshEntry->m_wstrSetName == pEntry->m_wstrSetName && INVALID_MESH_INSTANCE_HANDLE != pMeshEntry->m_hMesh
						&& NULL != g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( pMeshEntry->m_hMesh ) )
					{
						pEntry->m_hTraceTargetMesh = pMeshEntry->m_hMesh;
						break;
					}
				}
			}
			// a BONE_TRACE_TARGET_MESH mesh becomes the trace target of every particle entry of the same set
			if( true == pEffectData->m_bBoneTraceTargetMesh )
			{
				for( size_t iParticle = 0; iParticle < m_vecJuneFxParticle.size(); ++iParticle )
				{
					JuneViewerFxEntry* pParticleEntry = m_vecJuneFxParticle[iParticle];
					if( NULL != pParticleEntry && pParticleEntry->m_wstrSetName == pEntry->m_wstrSetName )
						pParticleEntry->m_hTraceTargetMesh = pEntry->m_hMesh;
				}
			}
#endif MODIFY_EFFECT_SET_TRACE_BONE

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			WCHAR wszCreate[512] = {0};
			StringCchPrintfW( wszCreate, ARRAY_SIZE(wszCreate), L"JUNE_FX UNIT_VIEWER_FX_CREATE %s mesh %s bone=%s", pEntry->m_wstrSetName.c_str(),
				pEffectData->m_EffectName.c_str(), pEffectData->m_PositionBoneName.c_str() );
			ErrorLogMsg( XEM_ERROR142, wszCreate );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}

	if( NULL != g_pData->GetUIMajorParticle() )
	{
		for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
			if( NULL == pEntry || NULL == pEntry->m_pEffectData || NULL == pEntry->m_pXSkinAnim.get()
				|| INVALID_PARTICLE_SEQUENCE_HANDLE != pEntry->m_hSeq )
				continue;

			CX2EffectSet::EffectData* pEffectData = pEntry->m_pEffectData;
			CKTDGXSkinAnim* pXSkinAnim = pEntry->m_pXSkinAnim.get();
			if( false == JuneViewerFxIsSamef( pEffectData->m_fStartAnimTime, 0.f ) && pEffectData->m_fStartAnimTime > pXSkinAnim->GetNowAnimationTime() )
				continue;

			const D3DXVECTOR3 vPos = JuneViewerFxFramePos( pXSkinAnim, pEffectData->m_PositionBoneName.c_str() ) + pEffectData->m_vPositionOffset;
			CKTDGParticleSystem::CParticleEventSequenceHandle hSeq = g_pData->GetUIMajorParticle()->CreateSequenceHandle( this,
				pEffectData->m_EffectName.c_str(), vPos, -1.f, -1.f, -1, -1, -1.f, true );
			CKTDGParticleSystem::CParticleEventSequence* pSeq = ( INVALID_PARTICLE_SEQUENCE_HANDLE != hSeq )
				? g_pData->GetUIMajorParticle()->GetInstanceSequence( hSeq ) : NULL;
			if( NULL == pSeq )
			{
				// June retries every frame; the log line is written once per set and effect
				static std::set<std::wstring> s_setLoggedParticle;
				if( true == s_setLoggedParticle.insert( pEntry->m_wstrSetName + L"/" + pEffectData->m_EffectName ).second )
				{
					WCHAR wszLog[512] = {0};
					StringCchPrintfW( wszLog, ARRAY_SIZE(wszLog), L"JUNE_FX UNIT_VIEWER_FX_CREATE %s particle %s NOT CREATED (not in UIMajorParticle.txt?), retried every frame",
						pEntry->m_wstrSetName.c_str(), pEffectData->m_EffectName.c_str() );
					ErrorLogMsg( XEM_ERROR142, wszLog );
				}
				continue;
			}

			pEntry->m_hSeq = hSeq;
			pSeq->SetAddRotateRel( pEffectData->m_vRotateRel );
			// USE_EFFECT_SCALE_IN_UNIT_VIEWER, except in the PvP room (June: g_pMain state != 8)
			if( true == pEffectData->m_bUseEffectScaleInUnitViewer && NULL != g_pMain && CX2Main::XS_PVP_ROOM != g_pMain->GetNowStateID() )
				pSeq->SetScaleFactor( pEffectData->m_vScale );
			// June sets seq +0x45a = 1 (write-only) and, for an unmanaged viewer, takes the particle out of the DG
			// manager: the viewer's own Draw (32957330) is then its only way on screen
			if( false == IsManagedByDGManager() && NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDGManager() )
				g_pKTDXApp->GetDGManager()->RemoveObjectChain( pSeq );
			if( true == pSeq->GetUseLand() )
				pSeq->SetLandPosition( JuneViewerFxLandPosition( true, pXSkinAnim ).y );

			// the particle's trace target: the first live mesh of the same set that is a BONE_TRACE_TARGET_MESH
#ifdef MODIFY_EFFECT_SET_TRACE_BONE
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				for( size_t iMesh = 0; iMesh < m_vecJuneFxMesh.size(); ++iMesh )
				{
					JuneViewerFxEntry* pMeshEntry = m_vecJuneFxMesh[iMesh];
					if( NULL != pMeshEntry && NULL != pMeshEntry->m_pEffectData && true == pMeshEntry->m_pEffectData->m_bBoneTraceTargetMesh
						&& pMeshEntry->m_wstrSetName == pEntry->m_wstrSetName && INVALID_MESH_INSTANCE_HANDLE != pMeshEntry->m_hMesh
						&& NULL != g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( pMeshEntry->m_hMesh ) )
					{
						pEntry->m_hTraceTargetMesh = pMeshEntry->m_hMesh;
						break;
					}
				}
			}
#endif MODIFY_EFFECT_SET_TRACE_BONE

#ifdef SERV_IRUHADEV_JUNE_FX_DEBUG
			WCHAR wszCreate[512] = {0};
			StringCchPrintfW( wszCreate, ARRAY_SIZE(wszCreate), L"JUNE_FX UNIT_VIEWER_FX_CREATE %s particle %s bone=%s", pEntry->m_wstrSetName.c_str(),
				pEffectData->m_EffectName.c_str(), pEffectData->m_PositionBoneName.c_str() );
			ErrorLogMsg( XEM_ERROR142, wszCreate );
#endif SERV_IRUHADEV_JUNE_FX_DEBUG
		}
	}
}

// 329582b0: the sound pass, the end of 329585f0, only while a pending sound exists
void CX2BaseUnitViewerUI::JuneViewerFxSoundPass()
{
	if( true == m_vecJuneFxPendingSound.empty() )
		return;

	// the skin is the first non-null one of the particle entries, else of the mesh entries. June does not test it and
	// faults in EventTimer when a set has SOUND%d but no UI-major mesh or particle; here such a set plays nothing
	CKTDGXSkinAnim* pXSkinAnim = NULL;
	for( size_t i = 0; NULL == pXSkinAnim && i < m_vecJuneFxParticle.size(); ++i )
	{
		if( NULL != m_vecJuneFxParticle[i] )
			pXSkinAnim = m_vecJuneFxParticle[i]->m_pXSkinAnim.get();
	}
	for( size_t i = 0; NULL == pXSkinAnim && i < m_vecJuneFxMesh.size(); ++i )
	{
		if( NULL != m_vecJuneFxMesh[i] )
			pXSkinAnim = m_vecJuneFxMesh[i]->m_pXSkinAnim.get();
	}
	if( NULL == pXSkinAnim || NULL == g_pKTDXApp )
		return;

	for( int iSound = 0; iSound < static_cast<int>( m_vecJuneFxPendingSound.size() ); ++iSound )
	{
		JuneViewerFxSound* pPending = m_vecJuneFxPendingSound[iSound];
		if( NULL == pPending )
			continue;
		const CX2DamageEffect::SoundData& soundData = pPending->m_SoundData;

		// USE_GLOBAL_TIME is not read here; no Set3DPosition (the sound is opened without 3D)
		if( true == pXSkinAnim->EventTimer( soundData.fEventTime ) && false == pPending->m_bPlayed && rand() % 100 < soundData.iRate )
		{
			CKTDXDeviceSound* pSound = g_pKTDXApp->GetDeviceManager()->OpenSound( soundData.fileName, 10, false, -1.f );
			if( NULL != pSound )
			{
				pSound->Play( false, true );
				pPending->m_bPlayed = true;
				JuneViewerFxPlayingSound* pPlaying = new JuneViewerFxPlayingSound;
				pPlaying->m_pSound = pSound;
				pPlaying->m_iGroup = pPending->m_iGroup;
				m_vecJuneFxPlayingSound.push_back( pPlaying );
			}
		}

		// LOOP re-arms at the end of each anim cycle; any other entry is dropped once played
		if( true == soundData.bLoop )
		{
			if( true == pXSkinAnim->IsAnimationEnd() )
				pPending->m_bPlayed = false;
		}
		else if( true == pPending->m_bPlayed )
		{
			delete pPending;
			m_vecJuneFxPendingSound.erase( m_vecJuneFxPendingSound.begin() + iSound );
			--iSound;
		}
	}

	// a sound that has stopped is stopped and closed
	for( size_t i = 0; i < m_vecJuneFxPlayingSound.size(); )
	{
		JuneViewerFxPlayingSound* pPlaying = m_vecJuneFxPlayingSound[i];
		if( NULL != pPlaying && NULL != pPlaying->m_pSound && false == pPlaying->m_pSound->IsPlaying() )
		{
			pPlaying->m_pSound->Stop();
			SAFE_CLOSE( pPlaying->m_pSound );
			delete pPlaying;
			m_vecJuneFxPlayingSound.erase( m_vecJuneFxPlayingSound.begin() + i );
		}
		else
			++i;
	}
}

// 329585f0: the per-frame update. Walks the mesh entries, then the particle entries, then the create and sound passes.
// June quirks kept (JUNE_BUGS.md): TRACE is never read (every live entry follows its bone), TRACE_MATRIX adds
// POSITION_OFFSET to its own position every frame, dead entries are not erased, only one land particle is refreshed per
// dirty flag, and the show byte (+0x190) is written into every effect each frame and never cleared.
/*virtual*/
HRESULT CX2BaseUnitViewerUI::OnFrameMove( double fTime, float fElapsedTime )
{
	if( NULL == g_pData )
		return S_OK;

	CKTDGXMeshPlayer* pMeshPlayer = g_pData->GetUIMajorXMeshPlayer();
	if( NULL != pMeshPlayer )
	{
		for( size_t i = 0; i < m_vecJuneFxMesh.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxMesh[i];
			if( NULL == pEntry || NULL == pEntry->m_pEffectData || NULL == pEntry->m_pXSkinAnim.get() || INVALID_MESH_INSTANCE_HANDLE == pEntry->m_hMesh )
				continue;
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pMeshPlayer->GetMeshInstance( pEntry->m_hMesh );
			if( NULL == pMesh )
				continue;

			CX2EffectSet::EffectData* pEffectData = pEntry->m_pEffectData;
			CKTDGXSkinAnim* pXSkinAnim = pEntry->m_pXSkinAnim.get();
			const bool bNoTarget = ( INVALID_MESH_INSTANCE_HANDLE == pEntry->m_hTraceTargetMesh );
			const bool bOwnTarget = ( pEntry->m_hTraceTargetMesh == pEntry->m_hMesh );

			if( true == pEffectData->m_bTraceMatrix && ( true == bNoTarget || true == bOwnTarget ) )
			{
				// (a) the viewer skin's frame matrix; POSITION_OFFSET is added to the mesh's own position every frame
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pXSkinAnim->GetCloneFrame( JuneViewerFxBoneName( pEffectData ) );
				if( NULL != pFrame )
				{
					pMesh->SetUseDXMatrix( pFrame->combineMatrix );
					pMesh->SetPos( pMesh->GetPos() + pEffectData->m_vPositionOffset );
				}
			}
			else if( true == bNoTarget )
			{
				// (b) the bone's position plus POSITION_OFFSET; USE_LAND_POSITION moves it to Bip01_Footsteps
				const WCHAR* pwszBone = ( true == pEffectData->m_bLandPosition ) ? L"Bip01_Footsteps" : JuneViewerFxBoneName( pEffectData );
				pMesh->SetPos( JuneViewerFxFramePos( pXSkinAnim, pwszBone ) + pEffectData->m_vPositionOffset );
			}
			else
			{
				// (c) the target mesh's skin
				CKTDGXMeshPlayer::CXMeshInstance* pTarget = pMeshPlayer->GetMeshInstance( pEntry->m_hTraceTargetMesh );
				if( NULL != pTarget && NULL != pTarget->GetXSkinAnim() )
				{
					const WCHAR* pwszBone = JuneViewerFxBoneName( pEffectData );
					if( true == pEffectData->m_bTraceMatrix )
					{
						CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pTarget->GetXSkinAnim()->GetCloneFrame( pwszBone );
						if( NULL != pFrame )
						{
							pMesh->SetUseDXMatrix( pFrame->combineMatrix );
							pMesh->SetPos( pMesh->GetPos() + pEffectData->m_vPositionOffset );
						}
					}
					else
						pMesh->SetPos( pTarget->GetBonePos( pwszBone ) );
				}
			}

			if( true == pEffectData->m_bFollowUnitViewerScale )
				pMesh->SetScale( GetMatrix().GetScale() );
#ifdef FIX_ICE_HEATER_EVENT
			pMesh->SetOverUI( pEffectData->m_bSetOverUI );
#endif FIX_ICE_HEATER_EVENT
			// m_bShowObject = viewer +0x190, NotifyShowObjectChanged when GetShowObject() changed
			pMesh->SetShowObject( m_bJuneFxShow );

			// the triggers (321dfc40, no set, the two viewer codes)
			JuneRunViewerMeshTriggers( pMesh, pEffectData, pEntry->m_vecTrigger, pXSkinAnim, m_iJuneFxTriggerCode1, m_iJuneFxTriggerCode2 );
		}
	}

	CKTDGParticleSystem* pParticleSystem = g_pData->GetUIMajorParticle();
	if( NULL != pParticleSystem )
	{
		for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
			if( NULL == pEntry || NULL == pEntry->m_pEffectData || NULL == pEntry->m_pXSkinAnim.get() || INVALID_PARTICLE_SEQUENCE_HANDLE == pEntry->m_hSeq )
				continue;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticleSystem->GetInstanceSequence( pEntry->m_hSeq );
			if( NULL == pSeq )
				continue;

			CX2EffectSet::EffectData* pEffectData = pEntry->m_pEffectData;
			CKTDGXSkinAnim* pXSkinAnim = pEntry->m_pXSkinAnim.get();
			const bool bNoTarget = ( INVALID_MESH_INSTANCE_HANDLE == pEntry->m_hTraceTargetMesh );
			bool bTraceUserTargetForce = false;
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
			bTraceUserTargetForce = pEffectData->m_bTraceUserTargetForce;
#endif FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

			if( true == pEffectData->m_bTraceMatrix && ( true == bNoTarget || true == bTraceUserTargetForce ) )
			{
				// (a) the viewer skin's frame matrix; POSITION_OFFSET is added to the sequence's own position every frame
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pXSkinAnim->GetCloneFrame( JuneViewerFxBoneName( pEffectData ) );
				if( NULL != pFrame )
				{
					pSeq->SetUseDXMatrix( pFrame->combineMatrix );
					pSeq->SetPosition( pSeq->GetPosition() + pEffectData->m_vPositionOffset, false, false );
				}
			}
			else if( true == bNoTarget )
			{
				// (b) the bone's position plus POSITION_OFFSET; USE_LAND_POSITION moves it to Bip01_Footsteps.
				// June's EffectSet_SpaceMoonRabbit_motionFX special case is June-only and dead in March.
				const WCHAR* pwszBone = ( true == pEffectData->m_bLandPosition ) ? L"Bip01_Footsteps" : JuneViewerFxBoneName( pEffectData );
				pSeq->SetPosition( JuneViewerFxFramePos( pXSkinAnim, pwszBone ) + pEffectData->m_vPositionOffset, false, false );
			}
			else if( NULL != pMeshPlayer )
			{
				// (c) the target mesh's skin
				CKTDGXMeshPlayer::CXMeshInstance* pTarget = pMeshPlayer->GetMeshInstance( pEntry->m_hTraceTargetMesh );
				if( NULL != pTarget && NULL != pTarget->GetXSkinAnim() )
				{
					const WCHAR* pwszBone = JuneViewerFxBoneName( pEffectData );
					if( true == pEffectData->m_bTraceMatrix )
					{
						CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = pTarget->GetXSkinAnim()->GetCloneFrame( pwszBone );
						if( NULL != pFrame )
						{
							pSeq->SetUseDXMatrix( pFrame->combineMatrix );
							pSeq->SetPosition( pSeq->GetPosition() + pEffectData->m_vPositionOffset, false, false );
						}
					}
					else
						pSeq->SetPosition( pTarget->GetBonePos( pwszBone ), false, false );
				}
			}

			if( true == pEffectData->m_bFollowUnitViewerScale )
				pSeq->SetScaleFactor( GetMatrix().GetScale() );
#ifdef FIX_ICE_HEATER_EVENT
			pSeq->SetOverUI( pEffectData->m_bSetOverUI );
#endif FIX_ICE_HEATER_EVENT
			// June also writes seq +0x415 = 0 here (unidentified, not ported)
			pSeq->SetShowObject( m_bJuneFxShow );

			// the land height, refreshed while the viewer's land-dirty byte (+0x1d4) is set
			if( true == m_bJuneFxLandDirty && true == pSeq->GetUseLand() )
			{
				const float fLandY = JuneViewerFxLandPosition( true, pXSkinAnim ).y;
				if( false == JuneViewerFxIsSamef( pSeq->GetLandPosition(), fLandY ) )
				{
					pSeq->SetLandPosition( fLandY );
					m_bJuneFxLandDirty = false;
				}
			}
		}
	}

	JuneViewerFxCreatePass();
	JuneViewerFxSoundPass();
	return S_OK;
}

// 32957260: Prepare on every live mesh and particle entry
/*virtual*/
RENDER_HINT CX2BaseUnitViewerUI::OnFrameRender_Prepare()
{
	if( NULL != g_pData )
	{
		if( NULL != g_pData->GetUIMajorXMeshPlayer() )
		{
			for( size_t i = 0; i < m_vecJuneFxMesh.size(); ++i )
			{
				JuneViewerFxEntry* pEntry = m_vecJuneFxMesh[i];
				if( NULL == pEntry || INVALID_MESH_INSTANCE_HANDLE == pEntry->m_hMesh )
					continue;
				if( CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( pEntry->m_hMesh ) )
					pMesh->OnFrameRender_Prepare();
			}
		}
		if( NULL != g_pData->GetUIMajorParticle() )
		{
			for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
			{
				JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
				if( NULL == pEntry || INVALID_PARTICLE_SEQUENCE_HANDLE == pEntry->m_hSeq )
					continue;
				if( CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEntry->m_hSeq ) )
					pSeq->OnFrameRender_Prepare();
			}
		}
	}
	return RENDER_HINT_DEFAULT;
}

// 32957330: June draws every live mesh entry, then every live particle entry, that is shown: PushStates( GetRenderStateID() )
// and OnFrameRender_Draw, without testing DG-manager membership. March differs here on purpose (play-test 2026-10-07: the
// cartoon-rendered weapon meshes of ETK/EIS, RTR and RNW came out black-only when drawn a second time from here, outside the
// DG manager's render-hint setup): March chains every mesh at creation, so the DG manager always draws them and they are not
// drawn here; a particle is drawn here only when the viewer is unmanaged, i.e. exactly when the create pass took it out of the
// DG manager (June 32957980: RemoveObjectChain for !IsManagedByDGManager). June's child-sequence draw (31cf4890) has no March
// counterpart that this class can reach: March chains a particle's child sequences itself.
/*virtual*/
void CX2BaseUnitViewerUI::OnFrameRender_Draw()
{
	if( NULL == g_pData || true == IsManagedByDGManager() )
		return;

	if( NULL != g_pData->GetUIMajorParticle() )
	{
		for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
			if( NULL == pEntry || INVALID_PARTICLE_SEQUENCE_HANDLE == pEntry->m_hSeq )
				continue;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEntry->m_hSeq );
			if( NULL != pSeq && true == pSeq->GetShowObject() )
			{
				CKTDGStateManager::PushStates( pSeq->GetRenderStateID() );
				pSeq->OnFrameRender_Draw();
			}
		}
	}
}

// 32957760: the viewer's GetShowObject() goes into every live mesh and particle, NotifyShowObjectChanged on each that changed
/*virtual*/
void CX2BaseUnitViewerUI::NotifyShowObjectChanged()
{
	if( NULL == g_pData )
		return;

	const bool bShow = GetShowObject();
	if( NULL != g_pData->GetUIMajorXMeshPlayer() )
	{
		for( size_t i = 0; i < m_vecJuneFxMesh.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxMesh[i];
			if( NULL == pEntry || INVALID_MESH_INSTANCE_HANDLE == pEntry->m_hMesh )
				continue;
			if( CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( pEntry->m_hMesh ) )
				pMesh->SetShowObject( bShow );
		}
	}
	if( NULL != g_pData->GetUIMajorParticle() )
	{
		for( size_t i = 0; i < m_vecJuneFxParticle.size(); ++i )
		{
			JuneViewerFxEntry* pEntry = m_vecJuneFxParticle[i];
			if( NULL == pEntry || INVALID_PARTICLE_SEQUENCE_HANDLE == pEntry->m_hSeq )
				continue;
			if( CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEntry->m_hSeq ) )
				pSeq->SetShowObject( bShow );
		}
	}
}
#endif SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
