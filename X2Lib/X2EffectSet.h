#pragma once

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
#include    "indexed_list.h"
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE




class CX2GameUnit;
class CX2SquareUnit;
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::intrusive_ptr<CX2SquareUnit>    CX2SquareUnitPtr;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::shared_ptr<CX2SquareUnit>    CX2SquareUnitPtr;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR

#ifdef SERV_PET_SYSTEM
class CX2PET;
#endif

#ifdef RIDING_SYSTEM
class CX2RidingPet;
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
class CX2TFieldNpc;
#endif SERV_IRUHADEV_ELYSION_VILLAGE
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::intrusive_ptr<CX2RidingPet> CX2RidingPetPtr;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::shared_ptr<CX2RidingPet> CX2RidingPetPtr;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
#endif //RIDING_SYSTEM

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK

struct  CX2EffectSet_EffectSetInstance_TAG {};
typedef KHandleType<int,CX2EffectSet_EffectSetInstance_TAG>     CX2EffectSet_EffectSetInstanceHandle;
#define INVALID_EFFECTSET_HANDLE    (CX2EffectSet_EffectSetInstanceHandle::invalid_handle())

#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK


class CX2EffectSet
{
public: 

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK

    typedef CX2EffectSet_EffectSetInstanceHandle    Handle;

#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    typedef int Handle;
    static const Handle INVALID_HANDLE = Handle(-1);
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK


	enum EFFECT_SET_MODE
	{
		ESM_INVALID,
		ESM_GAME,			// 게임내에서
		ESM_FIELD,			// 마을에서
#ifdef SERV_PET_SYSTEM
		ESM_COMMON,			// 게임 & 마을
#endif
	};


	enum EFFECT_TYPE
	{
		ET_INVALID,
		ET_DAMAGE_EFFECT,
		ET_MESH_PLAYER_UI_MAJOR,
		ET_MESH_PLAYER_UI_MINOR,
		ET_MESH_PLAYER_GAME_MAJOR,
		ET_MESH_PLAYER_GAME_MINOR,
		ET_PARTICLE_UI_MAJOR,
		ET_PARTICLE_UI_MINOR,
		ET_PARTICLE_GAME_MAJOR,
		ET_PARTICLE_GAME_MINOR,
	};

	enum EFFECT_SET_TIMER_TYPE
	{
		ESTT_ANIM,
		ESTT_CUSTOM,
	};



	struct CameraShakeData 
	{
		CKTDGCamera::SHAKE_TYPE m_eShakeType;
		float			m_fStartAnimTime;
		int				m_iLimitDistanceSq;			/// 플레이어와 이 거리 내에 있어야 카메라 크래시 발생 (제곱값)
		float			m_fOneDirSpeed;
		float			m_fOneDirAccel;
		float			m_fTwoDirGap;
		float			m_fTwoDirTime;
		float			m_fTwoDirTimeGap;
		bool			m_bFixedLookVec;
		int				m_iEyeShakeCount;
		CMinMax<float>	m_EyeMoveSpeed;
		CMinMax<float>	m_EyeMoveRange;
		int				m_iLookAtShakeCount;
		CMinMax<float>	m_LookAtMoveSpeed;
		CMinMax<float>	m_LookAtMoveRange;

#if defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)
// #ifdef EFFECT_TOOL
		static bool IsSamef_( float a, float b = 0.f )
		{
			if( fabs( a - b ) > 0.0009 )
				return false;
			return true;
		}
		bool operator== ( const CameraShakeData& rhs_ )
		{
			if( m_eShakeType				!= rhs_.m_eShakeType			) return false;
			if( m_iLimitDistanceSq			!= rhs_.m_iLimitDistanceSq		) return false;

			if(false == IsSamef_(m_fStartAnimTime	,rhs_.m_fStartAnimTime	)) return false;
			if(false == IsSamef_(m_fOneDirSpeed		,rhs_.m_fOneDirSpeed	)) return false;
			if(false == IsSamef_(m_fOneDirAccel		,rhs_.m_fOneDirAccel	)) return false;
			if(false == IsSamef_(m_fTwoDirGap		,rhs_.m_fTwoDirGap		)) return false;
			if(false == IsSamef_(m_fTwoDirTime		,rhs_.m_fTwoDirTime		)) return false;
			if(false == IsSamef_(m_fTwoDirTimeGap	,rhs_.m_fTwoDirTimeGap	)) return false;

			if( m_bFixedLookVec				!= rhs_.m_bFixedLookVec			) return false;
			if( m_iEyeShakeCount			!= rhs_.m_iEyeShakeCount		) return false;
			if( m_iLookAtShakeCount			!= rhs_.m_iLookAtShakeCount		) return false;
			if( m_EyeMoveSpeed.m_Max		!= rhs_.m_EyeMoveSpeed.m_Max	) return false;
			if( m_EyeMoveSpeed.m_Min		!= rhs_.m_EyeMoveSpeed.m_Min	) return false;
			if( m_EyeMoveRange.m_Max		!= rhs_.m_EyeMoveRange.m_Max	) return false;
			if( m_EyeMoveRange.m_Min		!= rhs_.m_EyeMoveRange.m_Min	) return false;
			if( m_LookAtMoveSpeed.m_Max		!= rhs_.m_LookAtMoveSpeed.m_Max	) return false;
			if( m_LookAtMoveSpeed.m_Min		!= rhs_.m_LookAtMoveSpeed.m_Min	) return false;
			if( m_LookAtMoveRange.m_Max		!= rhs_.m_LookAtMoveRange.m_Max	) return false;
			if( m_LookAtMoveRange.m_Min		!= rhs_.m_LookAtMoveRange.m_Min	) return false;

			return true;
		}
// #endif // EFFECT_TOOL
#endif //defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		CameraShakeData() :
		m_eShakeType( CKTDGCamera::DECT_INVALID ),
		m_fStartAnimTime(0.f),
		m_iLimitDistanceSq( LIMIT_DISTANCE_SQ ),
		m_fOneDirSpeed(0.f),
		m_fOneDirAccel(0.f),
		m_fTwoDirGap(0.f),
		m_fTwoDirTime(0.f),
		m_fTwoDirTimeGap(0.f),
		m_bFixedLookVec(false),
		m_iEyeShakeCount(0),
		m_EyeMoveSpeed(0.f, 0.f),
		m_EyeMoveRange(0.f, 0.f),
		m_iLookAtShakeCount(0),
		m_LookAtMoveSpeed(0.f, 0.f),
		m_LookAtMoveRange(0.f, 0.f)
		{
		}
	};



#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
	// June2017 ANIMTIME_ON_START_GAME / _SQUARE / _UNITVIEWER sub-table of a trigger
	// entry, parsed in AddEffectSetTemplet_LUA from FUN_321e48e0 @ 321eb792-321ebb80.
	struct AnimTimeOnStartData
	{
		bool			m_bOnce;		// ONCE, default false
		float			m_fTime;		// TIME, default 0
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		bool			m_bFirstUse;	// June2017 runtime byte after ONCE (+1), 1 in the ctor FUN_321f3be0: TIME applies while it is set
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

		bool operator==( const AnimTimeOnStartData& rhs_ ) const
		{
			return m_bOnce == rhs_.m_bOnce && m_fTime == rhs_.m_fTime;
		}

		AnimTimeOnStartData()
		: m_bOnce( false )
		, m_fTime( 0.f )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		, m_bFirstUse( true )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		{
		}
	};

	// June2017 mesh-player trigger entry: one MESH_TRIGGER_DATA_LIST/TRIGGER_DATA%d, or the
	// single inline MESH_TRIGGER_DATA. Defaults are June's ctor FUN_321f3be0; the parser
	// overrides USE_MESH_TRANSITION to true for list entries only. Parse only; consumed
	// in Phase 4a.
	struct MeshTriggerData
	{
		float						m_fTriggerTime;						// TRIGGER_TIME, default -1
		std::wstring				m_wstrMotionName;					// MOTION_NAME
		int							m_iPlayType;						// PLAY_TYPE, default 2 (binary-only; March's CKTDGXSkinAnim::XAP_ONE_WAIT is 2)
		std::vector<int>			m_vecBoundMotionType;				// BOUND_MOTION_TYPE_LIST (the data uses EMOTION_TYPE values)
		bool						m_bStopTracingOnTrigger;			// STOP_TRACING_ON_TRIGGER
		bool						m_bTriggerOnce;						// TRIGGER_ONCE
		bool						m_bBlockTrigger;					// BLOCK_TRIGGER
		float						m_fBlockTriggerTime;				// BLOCK_TRIGGER_TIME, default -1
		std::vector<std::wstring>	m_vecTriggerEffectSetName;			// TRIGGER_EFFECTSET_LIST/[]/EFFECT_SET_NAME, non-empty names only
		float						m_fEffectSetToggleVisibilityTimer;	// EFFECT_SET_TOGGLE_VISIBILITY_TIMER, default -1
		bool						m_bUseEffectSetToggleVisibilityTimer;	// set by the parser when that timer is not -1
		bool						m_bUseMeshTransition;				// USE_MESH_TRANSITION
		AnimTimeOnStartData			m_AnimTimeOnStartGame;				// ANIMTIME_ON_START_GAME
		AnimTimeOnStartData			m_AnimTimeOnStartSquare;			// ANIMTIME_ON_START_SQUARE (binary-only)
		AnimTimeOnStartData			m_AnimTimeOnStartUnitViewer;		// ANIMTIME_ON_START_UNITVIEWER (binary-only)
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 runtime state of a list entry (FUN_321dfc40), preset by the entry ctor FUN_321f3be0
		std::wstring				m_wstrTriggeredAnimName;			// +0x2c: the skin's anim when the entry last fired
		int							m_iTriggeredPlayCount;				// +0x48: the skin's play count then
		bool						m_bArmed;							// +0x4c
		bool						m_bFiredOnce;						// +0x72: TRIGGER_ONCE, never again
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

		bool operator==( const MeshTriggerData& rhs_ ) const
		{
			if( m_fTriggerTime						!= rhs_.m_fTriggerTime						) return false;
			if( m_wstrMotionName					!= rhs_.m_wstrMotionName					) return false;
			if( m_iPlayType							!= rhs_.m_iPlayType							) return false;
			if( m_vecBoundMotionType				!= rhs_.m_vecBoundMotionType				) return false;
			if( m_bStopTracingOnTrigger				!= rhs_.m_bStopTracingOnTrigger				) return false;
			if( m_bTriggerOnce						!= rhs_.m_bTriggerOnce						) return false;
			if( m_bBlockTrigger						!= rhs_.m_bBlockTrigger						) return false;
			if( m_fBlockTriggerTime					!= rhs_.m_fBlockTriggerTime					) return false;
			if( m_vecTriggerEffectSetName			!= rhs_.m_vecTriggerEffectSetName			) return false;
			if( m_fEffectSetToggleVisibilityTimer	!= rhs_.m_fEffectSetToggleVisibilityTimer	) return false;
			if( m_bUseEffectSetToggleVisibilityTimer	!= rhs_.m_bUseEffectSetToggleVisibilityTimer	) return false;
			if( m_bUseMeshTransition				!= rhs_.m_bUseMeshTransition				) return false;
			if( false == ( m_AnimTimeOnStartGame		== rhs_.m_AnimTimeOnStartGame		) ) return false;
			if( false == ( m_AnimTimeOnStartSquare		== rhs_.m_AnimTimeOnStartSquare		) ) return false;
			if( false == ( m_AnimTimeOnStartUnitViewer	== rhs_.m_AnimTimeOnStartUnitViewer	) ) return false;
			return true;
		}

		MeshTriggerData()
		: m_fTriggerTime( -1.f )
		, m_wstrMotionName( L"" )
		, m_iPlayType( 2 )
		, m_bStopTracingOnTrigger( false )
		, m_bTriggerOnce( false )
		, m_bBlockTrigger( false )
		, m_fBlockTriggerTime( -1.f )
		, m_fEffectSetToggleVisibilityTimer( -1.f )
		, m_bUseEffectSetToggleVisibilityTimer( false )
		, m_bUseMeshTransition( false )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		, m_iTriggeredPlayCount( 0 )
		, m_bArmed( false )
		, m_bFiredOnce( false )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		{
		}
	};
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

	struct EffectData
	{
		EFFECT_TYPE		m_eEffectType;
		std::wstring	m_EffectName;	
		std::wstring	m_EffectNameHyper;
		float			m_fStartAnimTime;
		std::wstring	m_PositionBoneName;
		// hoons.09-09-25.무기 본에 이펙트를 붙일경우..
		std::wstring	m_PositionWeaponBoneName;
		int				m_iWeaponIndex;
		int				m_iModelIndex;
		// hoons.09-09-25.
#ifdef RIDING_SYSTEM
		std::wstring	m_PosRidingPetBoneName;
#endif //RIDING_SYSTEM
		D3DXVECTOR3		m_vPositionOffset;
		bool			m_bTrace;
		bool			m_bTraceMore;				// 파티클이 trace될 때 bone의 움직임 보다 느리게 따라오는 문제가 있을 때 사용

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		bool			m_bTraceMatrix;
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

#ifdef ARA_CHANGE_CLASS_FIRST
		bool			m_bReverseY;
#endif
		
		bool			m_bLandPosition;			// m_PositionBoneName이 null string 인 경우에 unit의 위치를 사용할 것인지 unit의 land position을 사용할 지

		D3DXVECTOR3		m_vRotateRel;
		bool			m_bApplyUnitRotation;		// 캐릭터가 라인맵에 따라 회전한 각도를 이펙트의 각도에 적용시킬지

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		bool			m_bApplyUnitRotationMirror;		// 캐릭터가 라인맵에 따라 회전한 각도를 이펙트의 각도에 적용시키면서 x축에 대해서는 방향에 따라 반전 설정
#endif // SERV_ARA_CHANGE_CLASS_SECOND
		
		bool			m_bApplyInstanceScale;		// EffectSetInstance에 지정된 scale을 적용할지 말지
		D3DXVECTOR3		m_vScale;
		D3DXVECTOR3		m_vScaleOnLeft;


		bool			m_bStartWhenTraceTargetDead;	// m_pTraceTargetGameUnit이 죽거나 없어졌을 때 반드시 생성되야 하는 이펙트
		bool			m_bPassiveEffect;				// effectsetinstacne의 life time에 영향을 주지 않는 이펙트 이다
		bool			m_bUseSubAttackListSet;			// 게임유닛의 SubAttackListSet을 사용할 것인지 여부(MeshPlayer만 사용가능)

#ifdef FIX_ICE_HEATER_EVENT
		bool			m_bSetOverUI;					// UI 위로 표시되는 기능 추가 ( Particle만 사용가능 )
#endif // FIX_ICE_HEATER_EVENT

#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
		bool			m_bBoneTraceTargetMesh;
#endif // MODIFY_EFFECT_SET_TRACE_BONE

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		bool			m_bTraceUserTargetForce;		// BoneTraceTargetMesh가 적용중 이라도, 해당 설정이 되어 있으면 유닛을 Trace 한다.
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

#ifdef MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE // 오현빈
		// 이펙트셋보다 지정된 거리 이상 멀어지면 생성하지 않도록 하기 위한 거리 값 저장 변수
		float			m_fLimitDistanceSq;
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 per-EFFECT%d keys, parsed in AddEffectSetTemplet_LUA from
		// FUN_321e48e0 @ 321e5cf8-321ec0cf (see the citations there). Parse only in this
		// phase; the consumer of each is Phase 4a/4b's to transcribe.
		std::wstring	m_wstrGroupName;						// GROUP_NAME (binary-only)
		bool			m_bZOffsetUseUnitIsRight;				// ZOFFSET_USE_UNIT_IS_RIGHT
		bool			m_bTraceMatrixExcludeScale;				// TRACE_MATRIX_EXCLUDE_SCALE; when set, the parser also sets m_bTraceMatrix
		bool			m_bTraceMatrixInField;					// TRACE_MATRIX_IN_FIELD
		bool			m_bUseIsRightWhenCreate;				// USE_IS_RIGHT_WHEN_CREATE
		bool			m_bFollowUnitRotation;					// FOLLOW_UNIT_ROTATION
		bool			m_bFollowUnitRotationCParticle;			// FOLLOW_UNIT_ROTATION_CPARTICLE
		bool			m_bFollowUnitViewerScale;				// FOLLOW_UNIT_VIEWER_SCALE
		bool			m_bUseEffectScaleInUnitViewer;			// USE_EFFECT_SCALE_IN_UNIT_VIEWER
		int				m_iBoneTraceTargetMeshIndex;			// BONE_TRACE_TARGET_MESH_INDEX, default -1
		bool			m_bNotAllowMemo;						// NOT_ALLOW_MEMO
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		D3DXVECTOR3		m_vJuneMemoScale;						// June2017 EffectData +0x108..+0x110 (ctor 321f4a20 @321f4ba5: 0,0,0): the instance's memo scale, copied in before GetEffectScale (321ee600 @321eede8)
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		int				m_iApplyEffectAlphaConditionByOption;	// APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION; unsupported, effect-alpha option absent

		// APPLY_DAMAGE_EFFECT_VELOCITY, ET_DAMAGE_EFFECT only. June pushes one entry per
		// table onto a vector with a type tag of 1; this parser can only ever make that
		// one entry, so it is held flat here.
		bool			m_bApplyDamageEffectVelocity;			// the table was present
		D3DXVECTOR3		m_vApplyDamageEffectVelocity;			// [1],[2],[3], default 1 each
		bool			m_bApplyDamageEffectVelocity4thIsZero;	// June keeps only ( [4] == 0 ), [4] default 0

		// ET_MESH_PLAYER_* only
		std::vector<MeshTriggerData>	m_vecMeshTriggerData;	// MESH_TRIGGER_DATA_LIST/TRIGGER_DATA%d
		MeshTriggerData					m_MeshTriggerData;		// MESH_TRIGGER_DATA: TRIGGER_TIME, MOTION_NAME, PLAY_TYPE only
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

#if defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)
// #ifdef EFFECT_TOOL
		static bool IsSamef_( float a, float b = 0.f )
		{
			if( fabs( a - b ) > 0.0009 )
				return false;
			return true;
		}
		bool operator==( const EffectData& rhs_ )
		{
			if( m_eEffectType					!= rhs_.m_eEffectType				) return false;
			if( m_EffectName					!= rhs_.m_EffectName				) return false;
			if( m_EffectNameHyper				!= rhs_.m_EffectNameHyper		    ) return false;

			if( false == IsSamef_( m_fStartAnimTime, rhs_.m_fStartAnimTime)			) return false;
			if( m_PositionBoneName				!= rhs_.m_PositionBoneName		    ) return false;
			if( m_PositionWeaponBoneName		!= rhs_.m_PositionWeaponBoneName	) return false;
			if( m_PosRidingPetBoneName			!= rhs_.m_PosRidingPetBoneName		) return false;
			if( m_iWeaponIndex					!= rhs_.m_iWeaponIndex			    ) return false;
			if( m_iModelIndex					!= rhs_.m_iModelIndex			    ) return false;
			if( m_vPositionOffset				!= rhs_.m_vPositionOffset		    ) return false;
			if( m_bTrace						!= rhs_.m_bTrace					) return false;
			if( m_bTraceMore					!= rhs_.m_bTraceMore				) return false;
			if( m_bTraceMatrix					!= rhs_.m_bTraceMatrix			    ) return false;
			if( m_bReverseY						!= rhs_.m_bReverseY				    ) return false;
			if( m_bLandPosition					!= rhs_.m_bLandPosition			    ) return false;
			if( m_vRotateRel					!= rhs_.m_vRotateRel				) return false;
			if( m_bApplyUnitRotation			!= rhs_.m_bApplyUnitRotation		) return false;
			if( m_bApplyInstanceScale			!= rhs_.m_bApplyInstanceScale	    ) return false;
			if( m_vScale						!= rhs_.m_vScale					) return false;
			if( m_vScaleOnLeft					!= rhs_.m_vScaleOnLeft			    ) return false;
			if( m_bStartWhenTraceTargetDead		!= rhs_.m_bStartWhenTraceTargetDead ) return false;
			if( m_bPassiveEffect				!= rhs_.m_bPassiveEffect			) return false;
			if( m_bUseSubAttackListSet			!= rhs_.m_bUseSubAttackListSet	    ) return false;
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
			if( m_bBoneTraceTargetMesh			!= rhs_.m_bBoneTraceTargetMesh		) return false;
#endif // MODIFY_EFFECT_SET_TRACE_BONE

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
			if( m_bTraceUserTargetForce			!= rhs_.m_bTraceUserTargetForce		) return false;
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

#ifdef MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE // 오현빈
			if( m_fLimitDistanceSq				!= rhs_.m_fLimitDistanceSq			) return false;
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
			if( m_wstrGroupName						!= rhs_.m_wstrGroupName						) return false;
			if( m_bZOffsetUseUnitIsRight			!= rhs_.m_bZOffsetUseUnitIsRight			) return false;
			if( m_bTraceMatrixExcludeScale			!= rhs_.m_bTraceMatrixExcludeScale			) return false;
			if( m_bTraceMatrixInField				!= rhs_.m_bTraceMatrixInField				) return false;
			if( m_bUseIsRightWhenCreate				!= rhs_.m_bUseIsRightWhenCreate				) return false;
			if( m_bFollowUnitRotation				!= rhs_.m_bFollowUnitRotation				) return false;
			if( m_bFollowUnitRotationCParticle		!= rhs_.m_bFollowUnitRotationCParticle		) return false;
			if( m_bFollowUnitViewerScale			!= rhs_.m_bFollowUnitViewerScale			) return false;
			if( m_bUseEffectScaleInUnitViewer		!= rhs_.m_bUseEffectScaleInUnitViewer		) return false;
			if( m_iBoneTraceTargetMeshIndex			!= rhs_.m_iBoneTraceTargetMeshIndex			) return false;
			if( m_bNotAllowMemo						!= rhs_.m_bNotAllowMemo						) return false;
			if( m_iApplyEffectAlphaConditionByOption	!= rhs_.m_iApplyEffectAlphaConditionByOption	) return false;
			if( m_bApplyDamageEffectVelocity		!= rhs_.m_bApplyDamageEffectVelocity		) return false;
			if( m_vApplyDamageEffectVelocity		!= rhs_.m_vApplyDamageEffectVelocity		) return false;
			if( m_bApplyDamageEffectVelocity4thIsZero	!= rhs_.m_bApplyDamageEffectVelocity4thIsZero	) return false;
			if( false == ( m_vecMeshTriggerData		== rhs_.m_vecMeshTriggerData				) ) return false;
			if( false == ( m_MeshTriggerData		== rhs_.m_MeshTriggerData					) ) return false;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

			return true;
		}

// #endif // EFFECT_TOOL
#endif //defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		EffectData() 
		: m_eEffectType( ET_INVALID )
		, m_EffectName( L"" )	
		, m_EffectNameHyper( L"" )
		, m_fStartAnimTime( 0.f )
		, m_PositionBoneName( L"" )
		, m_PositionWeaponBoneName( L"" )
		, m_iWeaponIndex(0)
		, m_iModelIndex(0)
#ifdef RIDING_SYSTEM
		, m_PosRidingPetBoneName( L"" )
#endif //RIDING_SYSTEM
		, m_vPositionOffset( 0, 0, 0)
		, m_bTrace( false )
		, m_bTraceMore( false )
		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		, m_bTraceMatrix( false )
#ifdef ARA_CHANGE_CLASS_FIRST
		, m_bReverseY( false )
#endif
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		, m_bLandPosition( false )
		, m_vRotateRel( 0, 0, 0 )
		, m_bApplyUnitRotation( true )
		, m_bApplyInstanceScale( true )
		, m_vScale( 0, 0, 0)
		, m_vScaleOnLeft( 0, 0, 0)
		, m_bStartWhenTraceTargetDead( false )
		, m_bPassiveEffect( false )
		, m_bUseSubAttackListSet( false )
#ifdef FIX_ICE_HEATER_EVENT
		, m_bSetOverUI( false )
#endif FIX_ICE_HEATER_EVENT
#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
		, m_bBoneTraceTargetMesh( false )
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		, m_bTraceUserTargetForce( false )
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
#ifdef MODIFY_CREATE_EFFECT_SET_LIMIT_DISTANCE // 오현빈
		, m_fLimitDistanceSq( -1.f )
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		, m_bApplyUnitRotationMirror( false )
#endif // SERV_ARA_CHANGE_CLASS_SECOND
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 EffectData ctor FUN_321f4a20 agrees with every default below
		, m_wstrGroupName( L"" )
		, m_bZOffsetUseUnitIsRight( false )
		, m_bTraceMatrixExcludeScale( false )
		, m_bTraceMatrixInField( false )
		, m_bUseIsRightWhenCreate( false )
		, m_bFollowUnitRotation( false )
		, m_bFollowUnitRotationCParticle( false )
		, m_bFollowUnitViewerScale( false )
		, m_bUseEffectScaleInUnitViewer( false )
		, m_iBoneTraceTargetMeshIndex( -1 )
		, m_bNotAllowMemo( false )
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		, m_vJuneMemoScale( 0.f, 0.f, 0.f )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		, m_iApplyEffectAlphaConditionByOption( 0 )
		, m_bApplyDamageEffectVelocity( false )
		, m_vApplyDamageEffectVelocity( 1.f, 1.f, 1.f )
		, m_bApplyDamageEffectVelocity4thIsZero( false )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		{
		}
	};

	struct EffectSetData
	{
		std::wstring				m_wstrEffectSetName;
		std::vector<EffectData*>	m_vecpEffectData;
		std::vector<CameraShakeData*> m_vecpCameraShakeData;
		EFFECT_SET_TIMER_TYPE		m_eEventTimerType;
		bool						m_bAffectedByUnitScale;

		//bool			m_bDeleteWhenTraceTargetDead;			// m_pTraceTargetGameUnit이 죽거나 없어지면 이펙트도 사라진다
		int				m_iDeleteShakeCount;					// 0보다 클 때 유효하고 이 회수만큼 흔들면 이펙트도 사라진다
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		// June2017 templet-level keys, parsed in AddEffectSetTemplet_LUA from
		// FUN_321e48e0 (see the citation there). Parse only in this phase.
		bool			m_bStopCreateNewEffectWhenStateChange;	// STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE; consumed in Phase 4a
		bool			m_bDeleteEffectSetAtTraceUnitDie;		// DELETE_EFFECTSET_AT_TRACE_UNIT_DIE; consumed in Phase 4b
		bool			m_bOverlap;								// OVERLAP; consumed in Phase 4a
		int				m_iAppliedSkillID;						// APPLIED_SKILL_ID; unsupported, applied-skill-ID system absent
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		D3DXVECTOR3		m_vJuneMemoScale;						// June2017 EffectSetData +0x60..+0x68 (ctor 321f3f70 @321f3f90: 1,1,1): an MI_HUGE memo's v0, written when a user with that memo plays the set (321e1e60 @321e1f8c) and never reset (JUNE_BUGS)
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		bool			m_bSyncShowByCreateUnit;				// SYNC_SHOW_BY_CREATE_UNIT; consumed in Phase 4a
		int				m_iApplyEffectAlphaConditionByOption;	// APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION; unsupported, effect-alpha option absent
		// June2017 SOUND%d entries, parsed in AddEffectSetTemplet_LUA from FUN_321e48e0 @
		// 321edd29-321ee2a7. June's entry is field for field March's CX2DamageEffect::SoundData.
		std::vector<CX2DamageEffect::SoundData>	m_vecSoundData;	// SOUND%d; consumed in Phase 4a
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
#if defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)
// #ifdef EFFECT_TOOL
		bool operator==( const EffectSetData& rhs_) const
		{
			if( m_wstrEffectSetName != rhs_.m_wstrEffectSetName ) return false;
			if( m_eEventTimerType != rhs_.m_eEventTimerType ) return false;
			if( m_bAffectedByUnitScale != rhs_.m_bAffectedByUnitScale ) return false;
			if( m_iDeleteShakeCount != rhs_.m_iDeleteShakeCount ) return false;
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
			if( m_bStopCreateNewEffectWhenStateChange != rhs_.m_bStopCreateNewEffectWhenStateChange ) return false;
			if( m_bDeleteEffectSetAtTraceUnitDie != rhs_.m_bDeleteEffectSetAtTraceUnitDie ) return false;
			if( m_bOverlap != rhs_.m_bOverlap ) return false;
			if( m_iAppliedSkillID != rhs_.m_iAppliedSkillID ) return false;
			if( m_bSyncShowByCreateUnit != rhs_.m_bSyncShowByCreateUnit ) return false;
			if( m_iApplyEffectAlphaConditionByOption != rhs_.m_iApplyEffectAlphaConditionByOption ) return false;
			if( m_vecSoundData.size() != rhs_.m_vecSoundData.size() ) return false;
			for( UINT iSound = 0; iSound < m_vecSoundData.size(); ++iSound )
			{
				const CX2DamageEffect::SoundData& lhsSound = m_vecSoundData[iSound];
				const CX2DamageEffect::SoundData& rhsSound = rhs_.m_vecSoundData[iSound];
				if( lhsSound.fEventTime != rhsSound.fEventTime ||
					lhsSound.fileName != rhsSound.fileName ||
					lhsSound.iRate != rhsSound.iRate ||
					lhsSound.bUseGlobalTime != rhsSound.bUseGlobalTime ||
					lhsSound.bLoop != rhsSound.bLoop )
					return false;
			}
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

			if( m_vecpEffectData.size() != rhs_.m_vecpEffectData.size() ) return false;

			int i = 0;
			BOOST_FOREACH( EffectData* pData, m_vecpEffectData )
			{
				if( (*pData) == (*rhs_.m_vecpEffectData[i]) )
					++i;
				else
					return false;
			}

			if( m_vecpCameraShakeData.size() != rhs_.m_vecpCameraShakeData.size() ) return false;
			i = 0;
			BOOST_FOREACH( CameraShakeData* pData, m_vecpCameraShakeData )
			{
				if( (*pData) == (*rhs_.m_vecpCameraShakeData[i]) )
					++i;
				else
					return false;
			}

			return true;
		}		
		EffectSetData& operator=( const EffectSetData& rhs )
		{
			m_wstrEffectSetName = rhs.m_wstrEffectSetName;
			m_eEventTimerType = rhs.m_eEventTimerType;
			m_bAffectedByUnitScale = rhs.m_bAffectedByUnitScale;
			m_iDeleteShakeCount = rhs.m_iDeleteShakeCount;
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
			m_bStopCreateNewEffectWhenStateChange = rhs.m_bStopCreateNewEffectWhenStateChange;
			m_bDeleteEffectSetAtTraceUnitDie = rhs.m_bDeleteEffectSetAtTraceUnitDie;
			m_bOverlap = rhs.m_bOverlap;
			m_iAppliedSkillID = rhs.m_iAppliedSkillID;
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
			m_vJuneMemoScale = rhs.m_vJuneMemoScale;
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
			m_bSyncShowByCreateUnit = rhs.m_bSyncShowByCreateUnit;
			m_iApplyEffectAlphaConditionByOption = rhs.m_iApplyEffectAlphaConditionByOption;
			m_vecSoundData = rhs.m_vecSoundData;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE

			m_vecpEffectData.clear();
			BOOST_FOREACH( CX2EffectSet::EffectData* pData, rhs.m_vecpEffectData )
			{
				CX2EffectSet::EffectData* pTempData = new CX2EffectSet::EffectData;
				*pTempData = *pData;
				m_vecpEffectData.push_back( pTempData );
			}

			m_vecpCameraShakeData.clear();
			BOOST_FOREACH( CX2EffectSet::CameraShakeData* pData, rhs.m_vecpCameraShakeData )
			{
				CX2EffectSet::CameraShakeData* pTempData = new CX2EffectSet::CameraShakeData;
				*pTempData = *pData;
				m_vecpCameraShakeData.push_back( pTempData );
			}

			return *this;
		}
// #endif // EFFECT_TOOL
#endif // defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		EffectSetData()
		: m_wstrEffectSetName( L"" )
		, m_eEventTimerType( ESTT_ANIM )
		, m_bAffectedByUnitScale( true )
		//, m_bDeleteWhenTraceTargetDead( false )
		, m_iDeleteShakeCount( -1 )
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		, m_bStopCreateNewEffectWhenStateChange( false )
		, m_bDeleteEffectSetAtTraceUnitDie( false )
		, m_bOverlap( false )
		, m_iAppliedSkillID( 0 )
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		, m_vJuneMemoScale( 1.f, 1.f, 1.f )
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		, m_bSyncShowByCreateUnit( false )
		, m_iApplyEffectAlphaConditionByOption( 0 )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
		{
		}

		~EffectSetData()
		{
			BOOST_TEST_FOREACH( EffectData*, pEffectData, m_vecpEffectData )
			{
				SAFE_DELETE( pEffectData );
			}
			m_vecpEffectData.clear();


			BOOST_TEST_FOREACH( CameraShakeData*, pCameraShakeData, m_vecpCameraShakeData )
			{
				SAFE_DELETE( pCameraShakeData );
			}
			m_vecpCameraShakeData.clear();
		}
	};



	struct EffectInstance
	{
		bool												m_bAlive;
		EffectData*											m_pEffectData;
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        CX2DamageEffect::CEffectHandle       						m_hDamageEffect;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CX2DamageEffect::CEffect*							m_pDamageEffect;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshPlayer;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleSequence;
		CX2GameUnitoPtr										m_optrGameUnit;
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 EffectInstance +0x4c: its own copy of MESH_TRIGGER_DATA_LIST, taken at creation
		// (FUN_321ee600 @ 321f2130-321f2144, copy FUN_321f47f0), for Phase 4b's UpdateEffect
		std::vector<MeshTriggerData>						m_vecMeshTriggerData;
		// June2017 EffectInstance +0x2c / +0x30 / +0x20 (the ctor FUN_321f4c30 clears all three), set by
		// FUN_321ee600 @ 321f2483-321f24f1 on each GROUP_NAME child particle: the flag, the child's bone
		// <base>_GROUPCHILD_<n>, and, with BONE_NAME, the child's position in the target mesh's BONE_NAME
		// frame (@ 321ef0ce-321ef1ef). Phase 4b's UpdateEffect reads all three (FUN_321e0080
		// @ 321e0825-321e0986, 321e0d45-321e0d69).
		bool												m_bGroupChild;
		std::wstring										m_wstrGroupChildBoneName;
		D3DXVECTOR3											m_vGroupChildOffset;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
		bool												m_bJuneDestroySilently;	/// Atlas E7: June EffectInstance +0x1c, copied from the set's +0x1b0 in its dtor ( 321e3a80 @321e3ad7 )
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

		EffectInstance() :
		m_bAlive( true ),
		m_pEffectData( NULL ),
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        m_hDamageEffect( INVALID_DAMAGE_EFFECT_HANDLE ),
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		m_pDamageEffect( NULL ),
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		m_hMeshPlayer( INVALID_MESH_INSTANCE_HANDLE ),
		m_hParticleSequence( INVALID_PARTICLE_SEQUENCE_HANDLE ),
		m_optrGameUnit()	/// Destructor 처리는 해주지 않는다
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		, m_bGroupChild( false )
		, m_vGroupChildOffset( 0.f, 0.f, 0.f )
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
		, m_bJuneDestroySilently( false )
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B
		{
		}

		~EffectInstance();
	};



#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST
	// June2017 effect-set buff list (effect completion C6c; funcmap rows Effect C6a2). One entry of EffectSetInstance +0x160 (0x18 B, pushed
	// by 321f4690): the buff factor id, its level and the modifiers June's class code fills (321e3730 / 321e3830, checklist 7a; the
	// data-driven attach 321e3650 pushes none). A Mod is 16 B: byte kind (0 = behavior, else finalizer), type, operation, value.
	struct BuffFactorListMod
	{
		bool		m_bFinalizer;	/// +0
		int			m_iType;		/// +4
		int			m_iOperation;	/// +8: 0 add, 1 multiply, 2 set
		float		m_fValue;		/// +0xc

		BuffFactorListMod() : m_bFinalizer( false ), m_iType( 0 ), m_iOperation( 0 ), m_fValue( 0.f ) {}
	};
	struct BuffFactorListEntry
	{
		UINT						m_uiBuffFactorID;	/// +0
		int							m_iLevel;			/// +4
		std::vector<BuffFactorListMod>	m_vecMod;		/// +8

		BuffFactorListEntry() : m_uiBuffFactorID( 0 ), m_iLevel( 0 ) {}
	};
#endif SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST
	struct EffectSetInstance
	{
		EFFECT_SET_MODE					m_eMode;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        Handle                          m_hHandle;
#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CKTDXSimpleHandleInterface< EffectSetInstance >::Handle m_hHandle;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
		CKTDGXSkinAnimPtr				m_pXSkinAnim;

		CX2GameUnitoPtr					m_optrGameUnit;				// owner
		CX2GameUnitoPtr					m_optrTraceTargetGameUnit;		// trace target
		CX2SquareUnitPtr				m_pSquareUnit;				// owner	
		bool							m_bHyper;	
	

		bool							m_bCustomPosition;
		D3DXVECTOR3						m_vPosition;				
		D3DXVECTOR3						m_vRotationDegree;			
		D3DXVECTOR3						m_vDirVector;				
		D3DXVECTOR3						m_vZVector;
		
																							   
		float							m_fDelayTime;				// effectset instance에 속한 이펙트를 이 시간 만큼 지난 후에 생성한다
		float							m_fPowerRate;				// effect set 안에서 damage effect가 생성될 때 power rate
		float							m_fPowerRateScale;			// damage effect 의 powerrate에 곱해진다
		float							m_fLifeTime;				// effect set이 살아있는 시간

		D3DXVECTOR3						m_vScale;
		bool							m_bDamageEffectScale;				// 어쩌구저쩌구 다무시하고 무조건 위 스케일값 적용시킨다.

		EffectSetData*					m_pEffectSetData;
		std::vector< bool >				m_vecEffectInstanceCreated;		// 이펙트가 생성되었는지 안되었는지
		std::vector<EffectInstance*>	m_vecpEffectInstance;
		std::vector< bool >				m_vecCameraShakeDone;
		float							m_fPrevElapsedTime;
		float							m_fElapsedTime;
#ifdef SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION
		BYTE							m_byEffectAlphaCondition;		// June2017 EffectSetInstance +0x1bc (ctor default 0 @321df516): APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION
#endif SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_FX
		D3DXVECTOR3						m_vJuneMemoScale;				// June2017 EffectSetInstance +0x150..+0x158: 0,0,0 in the ctor (321df230 @321df495), then the templet's (@321df584)
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_FX


		bool							m_bUseAnimationTiming;		// 유닛의 애니메이션 타이밍에 맞춰서 이펙트를 생성하는지 아닌지

		int				m_iCurrDeleteShakeCount;					// 0보다 클 때 유효하고 이 회수만큼 흔들면 이펙트도 사라진다
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqStickShake;

#ifdef TRACE_UNIT_DIE_DELETE_EFFECTSET
		bool			m_bCreateEffectSetTraceUnit;				// 지금 effectset이 trace 기능을 사용하는지
		bool			m_bTraceUnitDieDeleteEffectSet;				// trace하던 유닛이 죽으면 그 이펙트를 지운다.
#endif //TRACE_UNIT_DIE_DELETE_EFFECTSET

#ifdef SERV_IRUHADEV_JUNE_DAMAGEDATA
		/// June2017 EffectSetInstance +0x130 (a std::wstring, ctor 321df230 @321df42c): the trace target's nearest collision
		/// frame name, stored by TRACE_NEAREST_BONE (321da350). While it is non-empty, GetEffectPosition and
		/// GetEffectPositionByBoneName take the trace target's bone of that name instead of the effect's own bone.
		/// Effect completion C5. Instances are never reused, so there is no reset path.
		std::wstring					m_wstrTraceNearestBone;
		bool JuneSetTraceNearestBone( IN const D3DXVECTOR3& vPos_, IN CX2GameUnit* pTraceUnit_ );
#endif SERV_IRUHADEV_JUNE_DAMAGEDATA

#ifdef SERV_PET_SYSTEM
		CX2PET *m_pPet;
#endif

		D3DXVECTOR3						m_vLocalRotateDegree;

#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
		// 이펙트셋은 기본적으로 게임 유닛 객체를 기준으로 이동 하게 되어 있음.
		// 이 변수를 사용하면, 위치의 주체를 게임유닛이 아닌 해당 이펙트셋 내의 특정 메쉬로 선택 할 수 있음.
		// BUT 주체가 되는 메쉬는 게임 유닛 객체의 위치에 종속 시킬 수 없음

		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hTargetMesh; // 파티클을 특정 본에 붙일 때 타겟팅 할 메쉬 객체 핸들
		EFFECT_TYPE				m_eTargetMeshEffectType;
		bool					m_bIsTargetMesh;
#endif // MODIFY_EFFECT_SET_TRACE_BONE

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		bool					m_bIsTargetMeshRight;		/// 타겟 매시의 방향 ( 처음 생성시 유닛의 IsRight 값 )
#endif // SERV_ARA_CHANGE_CLASS_SECOND

#ifdef FIX_TARGET_MESH_OFFSET_POS // 김태환
		D3DXVECTOR3				m_vecTargetMeshDirVector;	/// 타겟 매시의 방향 벡터 ( 처음 생성시 유닛의 방향 벡터 값 )
		D3DXVECTOR3				m_vecTargetMeshZVector;		/// 타겟 매시의 Z 벡터 ( 처음 생성시 유닛의 Z 벡터 값 )
#endif // FIX_TARGET_MESH_OFFSET_POS


#ifdef ADD_RESOURCE_ERROR_LOG
		set<wstring>					m_setEmptyBoneName;
#endif //ADD_RESOURCE_ERROR_LOG

#ifdef ADD_RENA_SYSTEM //김창한
		CX2DamageManager::FIRST_ATTACK_CHECK		m_eFirstAttack;					/// 첫번째 공격을 성공했는가?
		CX2DamageManager::DamageRelateSkillData		m_RelateSkillData;
#endif //ADD_RENA_SYSTEM

#ifdef FIX_EFFECT_SCALE_BY_UNIT_SCALE // 김태환
		D3DXVECTOR3		m_vBoneTraceTargetMeshScale;	/// 행렬 Trace일 때, BoneTraceTargetMesh의 스케일 값 빼주기 위한 저장 변수
#endif //FIX_EFFECT_SCALE_BY_UNIT_SCALE

#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 target-mesh list: EffectSetInstance +0xf0 in June, with 0x14-byte entries
		// {handle, EFFECT_TYPE, scale}. FUN_321ee600 pushes one entry per
		// BONE_TRACE_TARGET_MESH effect it creates (@ 321f1f76-321f20f4 damage effect,
		// @ 321f21f7-321f2414 mesh player), and BONE_TRACE_TARGET_MESH_INDEX picks an entry.
		// Entry 0 is mirrored into m_hTargetMesh / m_eTargetMeshEffectType above, which the
		// functions Phase 4b has not transcribed yet still read.
		struct TargetMeshData
		{
			CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hTargetMesh;
			EFFECT_TYPE								m_eEffectType;
			D3DXVECTOR3								m_vScale;		// SCALE, or (1,1,1) when its x is 0

			TargetMeshData()
			: m_hTargetMesh( INVALID_MESH_INSTANCE_HANDLE )
			, m_eEffectType( ET_INVALID )
			, m_vScale( 1.f, 1.f, 1.f )
			{
			}
		};
		std::vector<TargetMeshData>		m_vecTargetMeshData;

		// June2017 EffectSetInstance +0x180: the creator's facing when the set was created
		// (FUN_321df230 @ 321df51c-321df576), read by USE_IS_RIGHT_WHEN_CREATE
		bool							m_bIsRightWhenCreate;
#ifdef SERV_IRUHADEV_JUNE_SPAWN_INHERIT
		// June2017 effect completion C6b: two instance stamps (funcmap rows tagged Effect C6a2). June +0x1b8,
		// ctor 0 (321df230 @321df510): the damage-effect index the unit's vslot 0x248 wrote at play time.
		// June +0x15c, ctor -1 (@321df4a9): the owner's state when the set was played (PlayEffectSet @321e20bf).
		int								m_iDamageEffectIndex;
		int								m_iOwnerStateIDOnPlay;
#endif SERV_IRUHADEV_JUNE_SPAWN_INHERIT
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST
		// June2017 EffectSetInstance +0x160 (effect completion C6c): the buff factors attached by AttachBuffFactorToEffectSet (321e3650), empty
		// at construction (321df230 @321df4b3/b9), freed with the instance (dtor 321e3a80). OnFrameMove_CreateEffect hands it to each damage
		// effect the set creates (321ee480 @321ef689).
		std::vector<BuffFactorListEntry>	m_vecBuffFactorList;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST

		// June2017 EffectSetInstance +0x54, cleared by the ctor FUN_321df230 (@ 321df323): set only by a
		// mesh trigger's STOP_TRACING_ON_TRIGGER (FUN_321dfc40 @ 321dff2e-321dff3c, Phase 4b stage 3).
		// While it is set, OnFrameMove_UpdateEffect skips TRACE_MATRIX (FUN_321e0080 @ 321e0fb5).
		bool							m_bStopTracingOnTrigger;

		// June2017 EffectSetInstance +0xdc: the set's show flag, 1 in the ctor FUN_321df230. SetShowObject
		// (FUN_321de4f0) hands it to every effect; SYNC_SHOW_BY_CREATE_UNIT keeps it equal to the owner's,
		// and a particle takes it when it is created (FUN_321ee600 @ 321f251d).
		bool							m_bShowObject;
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
		// June2017 EffectSetInstance +0xdd, 0 in the ctor FUN_321df230 (@321df3e3, a word store 0x0001 for +0xdc/+0xdd): set to 1
		// only by PlayEffectSet( name, skin ) (321e28d0, a set played on a unit viewer). GetEffectPosition and
		// GetEffectPositionByBoneName then read the viewer bone's screen position, and a particle created on the set is a
		// 2D, resolution-converted, over-UI sequence (FUN_321ee600 @321f2522). The +0xe0 holder beside it is C9h-3's.
		bool							m_bPlayedOnUnitViewer;
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
		// June2017 EffectSetInstance +0xd8 (C9h-3a): the riding pet a set played through PlayEffectSet( name, CX2RidingPetPtr )
		// (321e2790) follows. Zero in the ctor (FUN_321df230 @321df3dd), an intrusive ref (+8) that the dtor releases (FUN_321e3a80
		// @321e3d79), so the pet outlives a set that is still playing on it. The mode is ESM_COMMON (3) with no pet (+0xd4) set. June's
		// +0xe0 beside it is the field NPC's (CX2TFieldNpc, C9h-5), not this one.
		CX2RidingPetPtr					m_pRidingPetHolder;
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
		// June2017 EffectSetInstance +0xe0 (Atlas E1c): the village NPC a set played through PlayEffectSet( name, CX2TFieldNpc* ) (321e26d0)
		// follows; mode ESM_FIELD. A raw pointer (no ref), null in the ctor; the NPC's dtor stops the set (325cd7c0).
		CX2TFieldNpc*					m_pFieldNpcHolder;
#endif SERV_IRUHADEV_ELYSION_VILLAGE

		// June2017 EffectSetInstance +0x55 / +0x58: a trigger's EFFECT_SET_TOGGLE_VISIBILITY_TIMER (FUN_321dfc40
		// @ 321dff89-321dff96), counted down by OnFrameMove_UpdateEffect (FUN_321e0080 @ 321e0105-321e0140).
		// June's ctor does not initialise +0x55; March starts it false.
		bool							m_bToggleVisibilityPending;
		float							m_fToggleVisibilityTimer;

		// June2017 SOUND%d playback state (FUN_321dee80): +0x181 the copy was taken, +0x184 the copy of the
		// templet's entries with a played flag each (0x2c bytes an entry: the 0x28-byte entry, then the flag
		// at +0x28), +0x194 the sounds it started (closed when they stop, or by the dtor, FUN_321da200)
		struct SoundPlayData
		{
			CX2DamageEffect::SoundData	m_SoundData;
			bool						m_bPlayed;

			SoundPlayData() : m_bPlayed( false ) {}
		};
		bool							m_bSoundPlayDataCopied;
		std::vector<SoundPlayData>		m_vecSoundPlayData;
		std::vector<CKTDXDeviceSound*>	m_vecpPlayingSound;
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
		bool							m_bJuneStopSilently;	/// Atlas E7: June EffectSetInstance +0x1b0 ( set by StopEffectSet( h, true ) 321e3ee0 @321e3f61 )
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

	private:
		EffectSetInstance() {}; // do not use
	
	public: 
		~EffectSetInstance();
#ifdef SERV_PET_SYSTEM
			EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit, CX2PET *pPet);
#else
		EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit );
#endif


		void OnFrameMove( double fTime, float fElapsedTime )
		{
			m_fPrevElapsedTime = m_fElapsedTime;
			m_fElapsedTime += fElapsedTime;
		}
		
		D3DXVECTOR3 GetEffectPosition( EffectData* pEffectData );
		D3DXVECTOR3 GetEffectScale( EffectData* pEffectData );
		D3DXVECTOR3 GetEffectRotation( EffectData* pEffectData );
		D3DXVECTOR3 GetUnitRotation( EffectData* pEffectData );
		bool IsUnitRight();
#ifdef EFFECT_TOOL
		void SetEffectPoistion_EffectTool( IN const EffectData* pEffectData_, OUT D3DXVECTOR3& vPos_ );
#endif // EFFECT_TOOL

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		const D3DXMATRIX* GetCombineMatrix( const EffectData* pEffectData ) const;
			//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		
		void SetPowerRateScale(const float val) { m_fPowerRateScale = val; }
		void SetUseAnimationTiming(const bool val) { m_bUseAnimationTiming = val; }

		void SetEffectScale( const D3DXVECTOR3& vScale, bool bDamageEffectSclae = false )
		{
			m_vScale = vScale;
			m_bDamageEffectScale = bDamageEffectSclae;
		}
		void SetLocalRotateDegree( const D3DXVECTOR3& vRot )
		{
			m_vLocalRotateDegree = vRot;
		}
#ifdef ADD_PET_NINE_TAIL_FOX		/// bCustomPosition 설정 가능하도록 수정
		void SetEffectPosition( const D3DXVECTOR3 &vPos, bool bCustomPosition = true )
		{
			m_bCustomPosition = bCustomPosition;

			m_vPosition = vPos;
		}
#else  ADD_PET_NINE_TAIL_FOX
		void SetEffectPosition( D3DXVECTOR3 &vPos )
		{
			m_bCustomPosition = true;
		}
#endif // ADD_PET_NINE_TAIL_FOX

		CX2GameUnit* GetOwnerUnit() const;

#ifdef FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE // 김태환
		void SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_, IN const EffectData* pEffectData_ );
#else //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE
		void SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_ );
#endif //FIX_BONE_TRACE_TARGET_MESH_UNIT_TRACE

#ifdef MODIFY_EFFECT_SET_TRACE_BONE // 오현빈
		CKTDGXMeshPlayer::CXMeshInstance* GetMeshInstanceByTargetMesh();
		bool GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData );
#endif // MODIFY_EFFECT_SET_TRACE_BONE
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
		// June2017 target-mesh access by index (see m_vecTargetMeshData)
		bool GetTargetMesh( IN int iIndex_, OUT CKTDGXMeshPlayer::CXMeshInstanceHandle& hTargetMesh_, OUT EFFECT_TYPE& eEffectType_ ) const;
		void InvalidateTargetMesh( IN int iIndex_ );
		CKTDGXMeshPlayer::CXMeshInstance* GetMeshInstanceByTargetMesh( IN int iIndex_ );
		bool GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData, IN int iIndex_ );
		// June2017 FUN_321dcc20 / FUN_321dd590: the two above with a caller-supplied bone, for GROUP_NAME
		bool GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData, IN int iIndex_, IN const std::wstring& wstrBoneName_, OUT bool& bBoneFound_ );
		D3DXVECTOR3 GetEffectPositionByBoneName( IN EffectData* pEffectData, IN const std::wstring& wstrBoneName_, OUT bool& bBoneFound_ );
		// June2017 FUN_321e4020 @ 321e4140-321e4180: some target-mesh entry still holds a handle
		bool HasTargetMeshHandle() const;
		// June2017 FUN_321de4f0: the set's show flag, handed to every effect (EffectInstance::SetShowObject)
		void SetShowObject( bool bShow_ );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
        Handle  GetHandle() const { return m_hHandle; }
        void    SetHandle( Handle handle )  { m_hHandle = handle; }
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef SERV_9TH_NEW_CHARACTER // 김태환
		void SetPos( IN const D3DXVECTOR3& vOffsetPos_ );
#endif //SERV_9TH_NEW_CHARACTER

	};

#ifndef X2OPTIMIZE_HANDLE_VALIDITY_CHECK
#ifndef X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	typedef CKTDXSimpleHandleInterface< CX2EffectSet::EffectSetInstance >::Handle Handle;
	static const Handle INVALID_HANDLE = CKTDXSimpleHandleInterface< CX2EffectSet::EffectSetInstance >::INVALID_HANDLE;

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK

public:
	CX2EffectSet(
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
            unsigned char ucSystemID = 0
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK        
        );
	~CX2EffectSet(void);


	void OnFrameMove( double fTime, float fElapsedTime );

	void OnFrameMove_CreateEffect( double fTime, float fElapsedTime );
	void OnFrameMove_DeleteEffect();
	void OnFrameMove_UpdateEffect();
	void OnFrameMove_StartCameraShake();

#ifdef SERV_PET_SYSTEM
	CX2EffectSet::Handle PlayEffectSetByPet_LUA( const char* strEffectSetName, CX2PET* pPet );
	CX2EffectSet::Handle PlayEffectSetByPet( const wstring& wstrEffectSetName, CX2PET* pPet );
#endif

	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit );
#ifdef SERV_IRUHADEV_JUNE_PET_VIEWER_FX
	CX2EffectSet::Handle PlayEffectSet( const WCHAR* pwszEffectSetName, CKTDGXSkinAnimPtr pXSkinAnim );	// June 321e28d0, no owner: unit viewers
#endif SERV_IRUHADEV_JUNE_PET_VIEWER_FX
#ifdef SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2RidingPetPtr pRidingPet );	// June 321e2790, on the UI effect set
#endif SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2TFieldNpc* pFieldNpc );	// June 321e26d0, the village NPC's EFFECT_SET_NAME (Atlas E1c)
#endif SERV_IRUHADEV_ELYSION_VILLAGE
	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit = NULL, 
		bool bHyper = false, float fPowerRate = -1.f, float fLifeTime = -1.f, D3DXVECTOR3 vScale = D3DXVECTOR3( 1, 1, 1 ), 
		bool bCustomPosition = false, const D3DXVECTOR3& vPosition = D3DXVECTOR3( 0, 0, 0 ), const D3DXVECTOR3& vRotationDegree = D3DXVECTOR3( 0, 0, 0 ), const D3DXVECTOR3& vDirVector = D3DXVECTOR3( 0, 0, 0 ) );

#ifdef HANABI_VILLAGE
	CX2EffectSet::Handle PlayEffectSetEvent( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, D3DXVECTOR3 vPosition );
#endif HANABI_VILLAGE
	CX2EffectSet::Handle PlayEffectSetCustomPos( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, const D3DXVECTOR3& vPos );
	void StopEffectSet( CX2EffectSet::Handle& hHandle );
	void StopEffectSetAll();
#ifdef CREATEINSTANCE_WITH_LIFETIME_IN_LUA
	CX2EffectSet::Handle PlayEffectSetWithLifetime_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, float fLifeTime );
	CX2EffectSet::Handle PlayEffectSetWithLifetimePos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, float fLifeTime, D3DXVECTOR3 vPosition = D3DXVECTOR3( 0, 0, 0 ) );
#endif //CREATEINSTANCE_WITH_LIFETIME_IN_LUA
#ifdef GET_EFFECTSET_POSITION_IN_LUA
	D3DXVECTOR3 GetEffectPosition_LUA( Handle hHandle, int iEffectIndex );
#endif GET_EFFECTSET_POSITION_IN_LUA

	//{{ kimhc // 2010.4.19 // 비밀던전 작업
#ifdef SERV_SECRET_HELL
	CX2EffectSet::Handle PlayEffectSet_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit );

#ifdef VELDER_BOSS_5 // oasis907 : 김상윤 [2010.7.30] // Effect의 Rotation Degree도 필요해서 확장했습니다.
	CX2EffectSet::Handle PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos , D3DXVECTOR3 vRotationDegree);
#else
	CX2EffectSet::Handle PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos);
#endif VELDER_BOSS_5	
	void StopEffectSet_LUA( CX2EffectSet::Handle hHandle );
#endif SERV_SECRET_HELL
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	void StopEffectSetSilent_LUA( CX2EffectSet::Handle hHandle );	/// Atlas E7: June 321db020
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B
	//}} kimhc // 2010.4.19 // 비밀던전 작업

	//{{ JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시
#ifdef SEASON3_MONSTER_2010_12
	CX2EffectSet::Handle PlayEffectSetTraceTarget_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit );
#endif SEASON3_MONSTER_2010_12
	//}} JHKang / 강정훈 / 2011/01/19

	EffectSetInstance* GetEffectSetInstance( CX2EffectSet::Handle hHandle );
#ifdef SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST
	// June2017 321e3650 / 321ee480 (effect completion C6c)
	void AttachBuffFactorToEffectSet( CX2EffectSet::Handle hHandle_, UINT uiBuffFactorID_, int iLevel_ );
	void ApplyBuffFactorListToDamageEffect( CX2DamageEffect::CEffect* pEffect_, const std::vector<BuffFactorListEntry>& vecBuffFactorList_ );
#endif SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST
#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
    bool               IsLiveInstanceHandle( CX2EffectSet::Handle hHandle ) { return GetEffectSetInstance( hHandle ) != NULL; }
    EffectSetInstance* ValidateInstanceHandle( CX2EffectSet::Handle& hHandle )
    {
        EffectSetInstance* pInstance = GetEffectSetInstance( hHandle );
        if ( pInstance == NULL )
            hHandle = INVALID_EFFECTSET_HANDLE;
        return pInstance;
    }
    void                DestroyInstanceHandle( CX2EffectSet::Handle& hHandle ) { StopEffectSet( hHandle ); }
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

	EffectSetData* GetEffectSetTemplet( const wstring& wstrEffectSetName );

	void RegisterLuaBind();
	bool OpenScriptFile( const wstring& wstrFileName );
	bool AddEffectSetTemplet_LUA();
	void ClearTempletAndInstance();
	
	static D3DXVECTOR3 GetZVector( const D3DXVECTOR3& vDirVector );

#ifdef ADD_PET_NINE_TAIL_FOX		/// bCustomPosition 설정 가능하도록 수정
	void SetEffectSetInstPos( CX2EffectSet::Handle hHandle, D3DXVECTOR3 vPos, bool bCustomPosition = true  )
#else  ADD_PET_NINE_TAIL_FOX
	void SetEffectSetInstPos( CX2EffectSet::Handle hHandle, D3DXVECTOR3 vPos )
#endif ADD_PET_NINE_TAIL_FOX
	{
		if( hHandle == INVALID_EFFECTSET_HANDLE )
			return;

		EffectSetInstance *pInst = GetEffectSetInstance( hHandle );
		if( pInst != NULL )
		{
#ifdef ADD_PET_NINE_TAIL_FOX		/// bCustomPosition 인자로 받아와서 설정
			pInst->SetEffectPosition( vPos, bCustomPosition );
#else  ADD_PET_NINE_TAIL_FOX
			pInst->SetEffectPosition( vPos );
#endif ADD_PET_NINE_TAIL_FOX
		}
	}

#ifdef FIX_ICE_HEATER_EVENT
	CX2EffectSet::Handle PlayEffectSetByMeshPlayer( const WCHAR* wstrEffectSetName, 
													CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh, EFFECT_TYPE	eTargetMeshType );
#endif // FIX_ICE_HEATER_EVENT
	
	void CX2EffectSet::SetEffectScale_LUA (CX2EffectSet::Handle hHandle, D3DXVECTOR3 vScale, bool bDamageEffectSclae )
	{
		if( hHandle == INVALID_EFFECTSET_HANDLE )
			return;

		EffectSetInstance *pInst = GetEffectSetInstance( hHandle );
		if( pInst != NULL )
		{
			pInst->SetEffectScale( vScale, bDamageEffectSclae );
		}
	}

#ifdef EFFECT_TOOL 
	std::vector< EffectSetData* >* GetVecEffectSetTemplet(){ return &m_vecEffectSetTemplet;}
	bool CreateEffectSetTemplet( EffectSetData* pEffectSetData_ );
	bool DeleteEffectSetTemplet( const wstring& wstrEffectSetName_ ) ;
	bool ChangeNameEffectSetTemplet( const wstring& wstrEffectSetName_,  const wstring& wstrChangeEffectSetName_ );

	void AddEffectSet_Description_LUA();
	void SetEffectSetVersion_LUA();
	void AddEffectSet_Description( wstring wstrEffectSetName_, wstring wstrEffectSetDesc_ );
	map < std::wstring, std::wstring >& GetEffectSetDescriptionList() { return m_mapEffectSetDescription; }
	wstring GetEffectSetDesc( const WCHAR* pEffectSetName_ );
	wstring GetEffectToolVersion(){ return m_wstrToolVersion;}
#endif //EFFECT_TOOL

#ifdef EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가
public :
	typedef std::map< std::wstring, EffectSetData* > EffectSetDataMap;
	EffectSetDataMap & GetMapEffectSetTemplet(){ return m_mapEffectSetTemplet;}
	bool MergeEffectSetTemplet( EffectSetData* pNewEffectSetTemplet_, EffectSetData * pOrgEffectSetTemplet_ );

#ifndef EFFECT_TOOL
	bool CreateEffectSetTemplet( EffectSetData* pEffectSetData_ );	
	bool DeleteEffectSetTemplet( EffectSetData* pEffectSetData_ ) ;
#endif // EFFECT_TOOL
#endif // EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가

    DWORD   ComposeHandle( WORD wIndex, OUT WORD& wStamp )
    {
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        wStamp &= 0x3fff;
        return wIndex | ( wStamp << 16L ) | ( m_ucSystemID << 30L );
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        return wIndex | ( wStamp << 16L );
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    }
    bool    DecomposeHandle( DWORD dwHandle, OUT WORD& wIndex, OUT WORD& wStamp )
    {
        wIndex = (WORD) dwHandle;
        wStamp = (WORD) ( ( dwHandle & 0xffff0000 ) >> 16L );
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        unsigned char ucSystemID = (unsigned char) ( wStamp >> 14L );
        wStamp &= 0x3fff;
        return ucSystemID == m_ucSystemID;
#else   X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        return true;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    }
#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
    unsigned char GetSystemID()                     { return m_ucSystemID; }
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK

private:

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

    EffectSetInstance*  _CreateEffectSetInstance( 
         EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit,
#ifdef  SERV_PET_SYSTEM
    CX2PET *pPet = NULL
#endif  SERV_PET_SYSTEM
    );

#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
#ifndef EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가
	typedef std::map< std::wstring, EffectSetData* > EffectSetDataMap;
#endif // f EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가
	EffectSetDataMap					m_mapEffectSetTemplet;

#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

    enum    EListType
    {
        LIST_LIVE = 0,
        LIST_FREE = 1,
        LIST_NUM
    };
    struct  KInstanceHandleInfo
    {
        EffectSetInstance*      m_pInstance;
        WORD                    m_wStamp;
        KInstanceHandleInfo()
            : m_pInstance( NULL )
            , m_wStamp(0)
        {
        }
    };
    typedef kog::indexed_list<KInstanceHandleInfo> KInstanceHandleList;
    KInstanceHandleList             m_coInstanceHandleList;

#else   X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	std::vector< EffectSetInstance* >	m_vecpEffectSetInstance;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef EFFECT_TOOL
	std::vector< EffectSetData* > m_vecEffectSetTemplet;
	map < std::wstring, std::wstring >  m_mapEffectSetDescription;
	wstring								m_wstrToolVersion;
#endif //EFFECT_TOOL

#ifndef X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE
	static CKTDXSimpleHandleInterface< CX2EffectSet::EffectSetInstance > CX2EffectSet::s_HandleManager;
#endif  X2OPTIMIZE_PARTICLE_AND_ETC_HANDLE

#ifdef  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
        unsigned char                           m_ucSystemID;
#endif  X2OPTIMIZE_HANDLE_VALIDITY_CHECK
};

