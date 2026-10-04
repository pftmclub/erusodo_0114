/**@file : X2BuffBehaviorTemplet.h
   @breif : 버프의 행동을 정의하는 Templet 파일
*/

#pragma once

class CX2BuffTemplet;
class CX2BuffBehaviorTemplet;
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::intrusive_ptr<CX2BuffBehaviorTemplet> CX2BuffBehaviorTempletPtr;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
typedef boost::shared_ptr<CX2BuffBehaviorTemplet> CX2BuffBehaviorTempletPtr;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR

/** @class : CX2BuffBehaviorTemplet
	@brief : 버프의 행동 방식을 정의하는 클래스
	@date : 2012/7/16/
*/
class CX2BuffBehaviorTemplet
{
public:
	static bool ParsingCombinationBehavior( KLuaManager& luaManager_, OUT vector<CX2BuffBehaviorTempletPtr>& vecBuffBehaviorTempletPtr_ );
	
	CX2BuffBehaviorTemplet( const CX2BuffBehaviorTemplet& rhs_ ) : m_eType( rhs_.m_eType ), m_bStart( rhs_.m_bStart )
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
        , m_uRefCount(0)
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
	{}
	virtual ~CX2BuffBehaviorTemplet() {}

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ ) {}
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ ) {}
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ ) = NULL;
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const = NULL;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const = NULL;
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ ) {}
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
	/// June behavior vtable slot 7 (base 32e72980 returns false): re-apply after ACCUMULATION_DECREASE_WHEN_FINISH
	/// lowered the stack count (CX2BuffTemplet::ReapplyBehaviorForAccumulationDecrease)
	virtual bool OnAccumulationCountDecreased( CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ ) { return false; }
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// June behavior vtable slots 3 and 9/10/11, reached only from June-only content (CX2BuffTemplet::
	/// SetAccumulationCountNow = 3236a430, CX2GameUnit::OperateBuffBehaviorValue = 3227c360). Slot 3 (base
	/// 32e864d0: true) re-applies the behavior at a stack count set from outside; slots 9/10/11 (base
	/// 3208ddc0: empty) add to, multiply or overwrite its value.
	virtual bool OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ ) { return true; }
	virtual void AddValueByOperation( const float fValue_ ) {}
	virtual void MultiplyValueByOperation( const float fValue_ ) {}
	virtual void SetValueByOperation( const float fValue_ ) {}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	
	bool SetFactorFromPacketTemplateMothod( const KBuffFactor& kBuffFactor_, CX2GameUnit* pGameUnit_ );
	bool DidStart() const { return m_bStart; }
	void SetStart(bool val) { m_bStart = val; }

#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	// 해당 BBT 타입을 받아와야 할 경우가 있어서 추가, GetType 과 동일
	BUFF_BEHAVIOR_TYPE GetBuffBehaviorType() const { return m_eType; }
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편

#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
    void    AddRef()    {   ++m_uRefCount; }
    void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR


protected:
	CX2BuffBehaviorTemplet() : m_eType( BBT_NONE ), m_bStart( false )
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
        , m_uRefCount(0)
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR    
    {}
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
    CX2BuffBehaviorTemplet& operator = ( const CX2BuffBehaviorTemplet& );
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR

	BUFF_BEHAVIOR_TYPE GetType() const { return m_eType; }
	void SetType(BUFF_BEHAVIOR_TYPE val) { m_eType = val; }

	bool		ParsingBehaviorTemplateMethod( KLuaManager& luaManager_, const char* pszTableName_ );
	virtual bool ParsingBehavior( KLuaManager& luaManager_ ) { return true; }
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ ) {};

private:
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
    unsigned                                        m_uRefCount;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR
	BUFF_BEHAVIOR_TYPE		m_eType;
	bool					m_bStart;
};

IMPLEMENT_INTRUSIVE_PTR( CX2BuffBehaviorTemplet );


/** @class : CX2BuffChangeNowHpPerSecondBehaviorTemplet
	@brief : 초당 HP가 변경 되는 행동을 정의 하는 클래스
	@date : 2012/7/16/
*/
class CX2BuffChangeNowHpPerSecondBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,							/// 초당 HP가 변화되는 타입
		FO_MULTIPLIER_OR_VALUE,					/// 배율 혹은 수치
		FO_PERCENT = FO_MULTIPLIER_OR_VALUE,	/// 수치타입이 아닌 경우에는 MULTIPLIER자리가 %
		FO_RELATION_TYPE,						/// 연동타입
		FO_ATTACK_VALUE,						/// 공격력
		FO_END									/// enum 개수
	};

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	enum SKILL_LEVEL
	{
		SL_VALUE = 30,
	};
#endif //UPGRADE_SKILL_SYSTEM_2013

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowHpPerSecondBehaviorTemplet ); }


#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );

protected:
	CX2BuffChangeNowHpPerSecondBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_fResultValue( 0.0f ),	m_delegateChangeHpByTypeFunc(), 
		m_delegateGetValueOrPercentPerSecondFunc(),
		m_eChangeType( BCT_SWAP_VALUE ), m_bDrawText( false )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	void SelectDelegate();
	void ChangeHpByValue( CX2GameUnit* pGameUnit_ );
	void ChangeHpByPercent( CX2GameUnit* pGameUnit_ );

	float GetFixValuePerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const;
	float GetValuePerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const;
	float GetPercentPerSecondByPercent( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const;
	void DrawText( CX2GameUnit* pGameUnit_, const float fDeltaValue_ );

private:
	typedef srutil::delegate1<void, CX2GameUnit*> DelegateChangeHpByType;
	typedef srutil::delegate2<float, const KBuffBehaviorFactor*, const float> DelegateGetValueOrPercentPerSecond;

	CKTDXCheckElapsedTime		m_CheckElapsedTimeForInterval;	/// 몇 초마다 HP를 변경해야하는지를 판단하기 위한 멤버
	float						m_fResultValue;					/// 초당 변경될 수치 또는 %(중첩등 모든 계산을 다 끝낸 결정값)
	DelegateChangeHpByType		m_delegateChangeHpByTypeFunc;	/// 초당 값에 의해 변경되는지 %에 의해 변경되는지에 따라 다른 멤버 함수 실행
	DelegateGetValueOrPercentPerSecond m_delegateGetValueOrPercentPerSecondFunc;	/// 초당 변경될 수치 또는 %를 얻는 함수 델리게이트
	BUFF_CHANGE_TYPE			m_eChangeType;				/// 초당 변화되는 것이 % 인지 값인지 구분
	bool						m_bDrawText;				/// 변화되는 양을 표시할 것인가
};

/** @class : CX2BuffChangeNowMpPerSecondBehaviorTemplet
	@brief : 초당 Mp가 변경 되는 행동을 정의 하는 클래스
	@date : 2012/7/16/
*/
class CX2BuffChangeNowMpPerSecondBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 초당 Mp가 변화되는 타입
		FO_VALUE_OR_PERCENT,		/// 수치 or %
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowMpPerSecondBehaviorTemplet ); }

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	virtual bool OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ );	/// June 32e7c680
	virtual void AddValueByOperation( const float fValue_ );		/// June 32e893a0
	virtual void MultiplyValueByOperation( const float fValue_ );	/// June 32e89430
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

protected:
	CX2BuffChangeNowMpPerSecondBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_CheckElapsedTimeForInterval( 0.0f ),
		m_fResultValue( 0.0f ), m_eChangeType( BCT_SWAP_VALUE ), m_bDrawText( false )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	void SelectDelegate();
	void ChangeMpByValue( CX2GameUnit* pGameUnit_ );
	void ChangeMpByPercent( CX2GameUnit* pGameUnit_ );

	float GetValueOrPercentPerSecond( const KBuffBehaviorFactor* pBehaviorFactor_, const float fRegist_ ) const;
	void DrawText( CX2GameUnit* pGameUnit_, const float fDeltaValue_ );

private:
	typedef srutil::delegate1<void, CX2GameUnit*> DelegateChangeMpByType;
	typedef srutil::delegate1<float, const float> DelegateGetValueOrPercentPerSecond;

	CKTDXCheckElapsedTime		m_CheckElapsedTimeForInterval;	/// 몇 초마다 Mp를 변경해야하는지를 판단하기 위한 멤버
	float						m_fResultValue;					/// 초당 변경될 수치 또는 %(중첩등 모든 계산을 다 끝낸 결정값)
	DelegateChangeMpByType		m_delegateChangeMpByTypeFunc;	/// 초당 값에 의해 변경되는지 %에 의해 변경되는지에 따라 다른 멤버 함수 실행
	BUFF_CHANGE_TYPE			m_eChangeType;				/// 초당 변화되는 것이 % 인지 값인지 구분
	bool						m_bDrawText;				/// 변화되는 양을 표시할 것인가
};

/** @class : CX2BuffReverseLeftRightBehaviorTemplet
	@brief : 일정 시간마다 좌우를 반전 시키는 행동 클래스
	@date : 2012/7/21/
*/
class CX2BuffReverseLeftRightBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffReverseLeftRightBehaviorTemplet ); }

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffReverseLeftRightBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_CheckElapsedTimeForInterval( 0.0f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

	/// 복사생성자는 선언하지 않음(현재는 KBuffIdentity를 깊은 복사 할 이유 없음)
private:
	CKTDXCheckElapsedTime		m_CheckElapsedTimeForInterval;	/// 몇 초마다 좌우를 반전 시켜야 하는지
};

/** @class : CX2BuffAttackImpossibleBehaviorTemplet
	@brief : 해당 버프가 걸려있는 동안에는 공격(일반+스킬) 불가 행동
	@date : 2012/7/22/
*/
class CX2BuffIdentityBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffIdentityBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffIdentityBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}
};

/** @class : CX2BuffChangeStatBehaviorTemplet
	@brief : 해당 버프가 걸려있는 동안에 스탯 변경
	@date : 2012/7/23/
*/
class CX2BuffChangeStatBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_RELATION_TYPE,			/// CHANGE_TYPE이 연동인 경우 무엇과 연동될것인지
		FO_MULTIPLIER,				/// 배율(CHANGE_TYPE이 REATION 일때 사용)
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStatBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
	virtual bool OnAccumulationCountDecreased( CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );	/// June 32e73500
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// June ChangeStat vslots 14/15: GetBuffOwnerUID (32ec5980, returns 0) and SetBuffOwnerUID (33093ad0, empty).
	/// Only June's _HaveBuffOwner subclass (BBT 93, Phase 5b) overrides them; SetFactor calls 15 on every apply.
	virtual UidType GetBuffOwnerUID() const { return 0; }
	virtual void SetBuffOwnerUID( const UidType ) {}
	/// June ChangeStat slot 3 (32e786d0) and slot 11 (32e86500: a plain store to m_fResultValue)
	virtual bool OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ );
	virtual void SetValueByOperation( const float fValue_ ) { m_fResultValue = fValue_; }
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

protected:
	CX2BuffChangeStatBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_eRelationType( BRT_END ),
		m_fResultValue( 0.0f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	float GetValue( const KBuffBehaviorFactor* pBehaviorFactor_, CX2GameUnit* pGameUnit_, const float fRegist_ ) const;
	bool MustUseOptionData() const;

private:
	BUFF_CHANGE_TYPE			m_eChangeType;
	BUFF_RELATION_TYPE			m_eRelationType;
	float						m_fResultValue;
};

//{{ Iruha : 2026-10-04 // June buff port Phase 5b (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
/** @class : CX2BuffChangeNowForceDownPerSecondBehaviorTemplet
	@brief : BBT 79. June CX2BuffChangeNowForceDownPerSecondBehaviorTemplet (create 32e86e90): every TIME_INTERVAL seconds
		the force-down gage of the unit is raised by the buff's value. The change type is fixed at BCT_FIX_VALUE by June's
		constructor, so the templet's BUFF_CHANGE_TYPE is never read.
*/
class CX2BuffChangeNowForceDownPerSecondBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// always BCT_FIX_VALUE (the parse rejects any other)
		FO_VALUE,					/// force down per tick (after the level slice, June 32d478aa)
		FO_END						/// enum end
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeNowForceDownPerSecondBehaviorTemplet ); }

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );

protected:
	CX2BuffChangeNowForceDownPerSecondBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_CheckElapsedTimeForInterval( 0.0f ), m_fResultValue( 0.0f ), m_eChangeType( BCT_FIX_VALUE )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	CKTDXCheckElapsedTime		m_CheckElapsedTimeForInterval;	/// TIME_INTERVAL (June +0x14 target, +0x18 sum)
	float						m_fResultValue;					/// force down per tick (June +0x1c)
	BUFF_CHANGE_TYPE			m_eChangeType;					/// BCT_FIX_VALUE (June +0x10)
};

/** @class : CX2BuffChangeStateByConditionBehaviorTemplet
	@brief : BBT 80-84 (*_BY_CONDITION). June CX2BuffChangeStateByConditionBehaviorTemplet (create 32e86c00, vt 335350ac):
		ChangeStat's stack and store handling with a value taken from the number of users in the game (BVC_RALLY_OF_HERO).
		80/81 write the attack stores of BBT 7/8 (main and range member), 82/83 the defence stores of 9/10, 84 the max HP store.
*/
class CX2BuffChangeStateByConditionBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// BUFF_CHANGE_TYPE
		FO_VALUE_CALCULATE_TYPE,	/// BUFF_VALUE_CALCULATE_TYPE
		FO_VALUE_COUNT,				/// table length (counted layout)
		FO_VALUE_0,					/// v[3]
		FO_VALUE_1,					/// v[4]
		FO_USER_NUM,				/// v[5], appended by CX2BuffFactor::SetGameUnitChangeStateByConditionBehavior
		FO_END						/// enum end
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStateByConditionBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeStateByConditionBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_eValueCalculateType( BVC_RALLY_OF_HERO ), m_fResultValue( 0.0f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	float GetValue( const KBuffBehaviorFactor* pBehaviorFactor_ ) const;

private:
	BUFF_CHANGE_TYPE			m_eChangeType;			/// June +0x10
	BUFF_VALUE_CALCULATE_TYPE	m_eValueCalculateType;	/// June +0x14
	float						m_fResultValue;			/// June +0x18
};

/** @class : CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner
	@brief : BBT 93. June CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner (create 32e86740, vt 33534ce4): ChangeStat plus the UID of
		the unit that applied the buff (June +0x20). ChangeStat's SetFactor hands it over through SetBuffOwnerUID (vslot 15).
		Slots 3, 6, 7, 8 and 12 are ChangeStat's; only GetFactor and the packet differ. June's owner aggregator (3227c620, reached
		only through CX2GUNPC vslot 183) has no caller in June's .text and is not ported.
*/
class CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner : public CX2BuffChangeStatBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner ); }

	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual UidType GetBuffOwnerUID() const { return m_uidBuffOwner; }									/// June 32ec60c0
	virtual void SetBuffOwnerUID( const UidType uidBuffOwner_ ) { m_uidBuffOwner = uidBuffOwner_; }		/// June 32e86510

protected:
	CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner() : CX2BuffChangeStatBehaviorTemplet(), m_uidBuffOwner( 0 )
	{}

private:
	UidType						m_uidBuffOwner;
};

/** @class : CX2BuffIgnoreDefenceBehaviorTemplet
	@brief : BBT 88 (Phase 5c). June class vt 33535558 (ctor 32e890b0): the unit that has the buff loses IGNORE_DEFENCE_RATE of
		its defence against every hit (CX2GameUnit::GetIgnoreDefenceRate, read by the defender in GetFinalDamage). One entry per
		buff in the unit's ignore-defence list; no restack (June 32e7d260 inserts on the first apply only).
		Slots: SetFactor 32e7d260, GetClonePtr 32e7d370, GetFactor 32e7ec40, DoFinish 32e75b00, packet 32e7d3f0. OnFrameMove,
		ParsingBehavior and the new June slots 3/7/9-11 are the base ones.
*/
class CX2BuffIgnoreDefenceBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffIgnoreDefenceBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffIgnoreDefenceBehaviorTemplet() : CX2BuffBehaviorTemplet(), m_fIgnoreDefenceRate( 1.0f )
	{}

private:
	KProtectedType<float>		m_fIgnoreDefenceRate;	/// June +0x10, set to 1.0 by the ctor
};
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_STAT
//}}

//{{ Iruha : 2026-10-04 // June buff port Phase 6 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
/// June buff port Phase 6: the data the behavior classes below hold. X2Main.h includes this header BEFORE X2BuffTemplet.h, so the types live here;
/// X2BuffTemplet.h's CX2SkillTypeRateByBuff / CX2SkillIDRateByBuff (the unit list elements) alias them.
typedef std::pair<int, float>	JuneSkillTypeRate;		/// BBT 89: { SKILL_TYPE, rate }
struct JuneSkillIDRateBlock							/// BBT 90 and 91: one { rate, skill IDs } block
{
	float			m_fRate;
	vector<int>		m_vecSkillID;

	JuneSkillIDRateBlock() : m_fRate( 1.0f ) {}
};

/** @class : CX2BuffDoubleAttackBuffBehaviorTemplet
	@brief : BBT 75 (Phase 6). June class vt 33535070. Same shape as March's BBT 56 class (CX2BuffDoubleAttackBehaviorTemplet) but on its own
		unit list (June +0x65b4, beside 56's +0x65a4), and read on the ATTACKER in both DamageReacts. Only BCT_PERCENT is accepted, and a
		mismatch returns false with no popup. ParsingBehavior reads BUFF_CHANGE_TYPE with default 0 (56's one fails without it).
		Slots: ParsingBehavior 32e75360, SetFactor 32e77be0, GetFactor 32e7e9f0, DoFinish 32e75540, packet 32e754f0.
*/
class CX2BuffDoubleAttackBuffBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// BUFF_CHANGE_TYPE
		FO_VALUE,					/// the rate
		FO_END,
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffDoubleAttackBuffBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffDoubleAttackBuffBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_fRate( 0.f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE				m_eChangeType;	/// June +0x10
	float							m_fRate;		/// June +0x14
};

/** @class : CX2BuffChangeSkillDamageBehaviorTemplet
	@brief : BBT 89 (Phase 6; class, store and reader only, the host is Phase 6b). June class vt 33535378. The unit gets one list entry per
		buff: the { SKILL_TYPE, rate } pairs (CX2GUUser::GetSkillDamageRateByBuff multiplies the entry rate for the skill's type into the
		skill's power rate). ParsingBehavior is the shared default (32ec5990).
		Slots: SetFactor 32e7a9e0, slot 7 32e7c020, GetFactor 32e7ecf0, DoFinish 32e75b70, packet 32e7af60.
*/
class CX2BuffChangeSkillDamageBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillDamageBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
	virtual bool OnAccumulationCountDecreased( CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );	/// June slot 7, 32e7c020

protected:
	CX2BuffChangeSkillDamageBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}

private:
	vector<JuneSkillTypeRate>	m_vecSkillTypeRate;	/// June +0x10
};

/** @class : CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet
	@brief : BBT 90 (Phase 6; class, store and reader only). June class vt 33535594. The unit gets one list entry per buff: { rate, skill IDs }
		blocks, read through the same CX2GUUser::GetSkillDamageRateByBuff as 89 (outside its skill-templet check).
		Slots: SetFactor 32e849c0, GetFactor 32e7ef50, DoFinish 32e75b90, packet 32e84b90.
*/
class CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}

private:
	vector<JuneSkillIDRateBlock>	m_vecBlock;	/// June +0x10, element 0x14
};

/** @class : CX2BuffChargeMpHitBySkillIDBehaviorTemplet
	@brief : BBT 91 (Phase 6, complete). June class vt 335355d0, the twin of 90 on the MP-charge list: CX2GUUser::GetChargeMpRateByBuff
		scales the attacker's hit MP gain for a hit whose APPLIED_SKILL_ID is in a block's list.
		Slots: SetFactor 32e84d70, GetFactor 32e7f2e0, DoFinish 32e75bb0, packet 32e84f40.
*/
class CX2BuffChargeMpHitBySkillIDBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChargeMpHitBySkillIDBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChargeMpHitBySkillIDBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}

private:
	vector<JuneSkillIDRateBlock>	m_vecBlock;	/// June +0x10, element 0x14
};

/** @class : CX2AddDamageOfSkillSlotType
	@brief : BBT 98 (Phase 6; class and reader only, the host is Phase 6b). June class vt 33535160. No unit store: CX2GUUser::
		GetSkillDamageRateBySlotType walks the live buff factors and multiplies the skill's power rate by m_fRate when the skill sits in
		a slot row of this slot type. A negative slot type rejects the factor.
		Slots: ParsingBehavior 32e75d20, SetFactor 32e78080, GetFactor 32e7f890, DoFinish 32e75cc0, packet 32e78170.
*/
class CX2AddDamageOfSkillSlotType : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2AddDamageOfSkillSlotType ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2AddDamageOfSkillSlotType() : CX2BuffBehaviorTemplet(),
		m_iSlotType( 0 ), m_fRate( 0.f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	int								m_iSlotType;	/// June +0x10, SKILL_SLOT_TYPE
	float							m_fRate;		/// June +0x14
};

/** @class : CX2BuffSkillTypeChangeBehaviorTemplet
	@brief : The shared body of BBT 99, 100 and 103 (Phase 6). June has three classes whose member (+0x10, element { int skill type, int
		change type, float value }), GetFactor and first-apply/restack arithmetic are the same; they differ in the merge rule and in
		what happens besides. The concrete classes below pick the rule and the extras.
		The factor is [ SKILL_TYPE, BUFF_CHANGE_TYPE, value ] per block.
*/
class CX2BuffSkillTypeChangeBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	struct SkillTypeChange
	{
		int		m_iSkillType;
		int		m_iChangeType;
		float	m_fValue;

		SkillTypeChange() : m_iSkillType( 0 ), m_iChangeType( 0 ), m_fValue( 0.0f ) {}
		SkillTypeChange( const int iSkillType_, const int iChangeType_, const float fValue_ )
			: m_iSkillType( iSkillType_ ), m_iChangeType( iChangeType_ ), m_fValue( fValue_ ) {}
	};

	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;

protected:
	CX2BuffSkillTypeChangeBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}

	static void ReadSkillTypeChanges( const KBuffBehaviorFactor& kFactor_, const float fScale_, OUT vector<SkillTypeChange>& vecChange_ );
	void ApplySkillTypeChanges( const KBuffBehaviorFactor& kFactor_, const CX2BuffTemplet* pBuffTemplet_, const bool bMergeFixValue_ );

	vector<SkillTypeChange>			m_vecChange;	/// June +0x10
};

/** @class : CX2BuffChangeRateBySkillTypeBehaviorTemplet
	@brief : BBT 99 CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE and BBT 100 CHANGE_COOLTIME_RATE_BY_SKILLTYPE (Phase 6; class and readers only, the hosts
		are Phase 6b). June vt 335353b4 / 335353f0, byte-identical, so one C++ class serves both; the type comes from GetType(). No unit
		store: CX2GUUser::ApplySkillTypeConsumeMpRate / ApplySkillTypeCoolTimeRate walk the live buff factors. DoFinish is empty.
		Slots (99 / 100): SetFactor 32e7b0d0 / 32e7b610, GetFactor 32e7f940 / 32e7fcb0, packet 32e7b530 / 32e7ba70.
*/
class CX2BuffChangeRateBySkillTypeBehaviorTemplet : public CX2BuffSkillTypeChangeBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeRateBySkillTypeBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );

protected:
	CX2BuffChangeRateBySkillTypeBehaviorTemplet() : CX2BuffSkillTypeChangeBehaviorTemplet()
	{}
};

/** @class : CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet
	@brief : BBT 103 CHANGE_SKILL_COOL_TIME_SPEED (Phase 6, complete). June vt 3353542c. NO packet override (June slot 13 is the empty base),
		so peers never receive it: it is the local player's cooldown speed. SetFactor and DoFinish only mark the unit's cooldown-speed
		table dirty; CX2GUUser rebuilds it from every live type-103 factor (June 32039fb0). The merge applies to BCT_PERCENT only.
		Slots: SetFactor 32e7bb50, DoFinish 32e75ea0.
*/
class CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet : public CX2BuffSkillTypeChangeBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet() : CX2BuffSkillTypeChangeBehaviorTemplet()
	{}
};
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
//}}

//{{ Iruha : 2026-10-04 // June buff port Phase 7 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG
/** @class : CX2BuffIgnoreStateGuardBehaviorTemplet
	@brief : BBT 73 IGNORE_STATE_GUARD. June class with SetFactor 32e752b0, packet 32e728b0, GetFactor 32e7e960 (header only), DoFinish 32e75300.
		While the unit has one, every hit it takes ignores its damage reduction and guard (CX2GameUnit::GetFinalDamage @322862bf).
*/
class CX2BuffIgnoreStateGuardBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffIgnoreStateGuardBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffIgnoreStateGuardBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}
};

/** @class : CX2BuffImmuneBuffDebuffBehaviorTemplet
	@brief : BBT 94 IMMUNE_BUFF_DEBUFF. June class (ctor 32e871d0, clone 32e7a930) with SetFactor 32e79e30, packet 32e7a040, GetFactor 32e7f670,
		DoFinish 32e75bd0. ALL_BUFF / ALL_DEBUFF put the buff's BTI in the unit's all-buff / all-debuff sets; the IMMUNE_BUFF_TEMPLATE_ID list goes
		into the unit's immune list. CX2GameUnit::IsImmuneBuffByBuffBehavior (June 322a80a0) reads all three. Two June quirks are kept verbatim:
		the list is read from index 2, which is the count (so the count itself becomes an immune BTI: 1 = BTI_DEBUFF_SILENCE for a one-entry or
		empty list), and GetFactor sends two bytes (June +0x20 / +0x21) that nothing but the ctor and the clone ever writes, so peers always
		receive ALL_BUFF = ALL_DEBUFF = 0.
*/
class CX2BuffImmuneBuffDebuffBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneBuffDebuffBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffImmuneBuffDebuffBehaviorTemplet() : CX2BuffBehaviorTemplet(), m_bAllBuff( false ), m_bAllDebuff( false )
	{}

	void InsertToGameUnit( const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_, const KBuffIdentity& BuffIdentity_ );	/// the part SetFactor and the packet share

private:
	bool			m_bAllBuff;		/// June +0x20: zeroed by the ctor, copied by the clone, written nowhere else
	bool			m_bAllDebuff;	/// June +0x21: the same
	vector<int>		m_vecBTI;		/// June +0x10: factor values [2..] as int (count first, then the BTIs)
};

/** @class : CX2BuffImmuneNonControlDebuffBehaviorTemplet
	@brief : BBT 95 IMMUNE_NON_CONTROL_DEBUFF. June class with SetFactor 32e75c00, packet 32e728d0 (insert only, no delete), GetFactor 32e7f770
		(header only), DoFinish 32e75c50. The first apply also erases every non-control debuff the unit already has (CX2GameUnit::
		DeleteNonControlDebuff, June 3229dcc0); while it is on, new ones are refused (IsImmuneNonControlDebuffByBuffBehavior, June 3227b790).
*/
class CX2BuffImmuneNonControlDebuffBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneNonControlDebuffBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffImmuneNonControlDebuffBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}
};

/** @class : CX2BuffImmuneEDTBehaviorTemplet
	@brief : BBT 105 IMMUNE_EDT. June class with SetFactor 32e79770, packet 32e79810, GetFactor 32e80390; DoFinish is the empty base (33093ad0).
		No unit store: the templet's EDT list is the store, and CX2GameUnit::IsImmuneEDTByBuff (June 322a0400) reads it back through
		GetBuffFactor. ProcessEnchantExtraDamage then refuses those enchant extra-damage types.
*/
class CX2BuffImmuneEDTBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffImmuneEDTBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );

protected:
	CX2BuffImmuneEDTBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}

private:
	vector<int>		m_vecEDT;		/// June +0x10: the factor's values as int (EXTRA_DAMAGE_TYPE)
};
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG
//}}

/** @class : CX2BuffCreateBuffFactortBehaviorTemplet
	@brief : 해당 버프가 걸려있는 동안에 BuffFactor 적용
	@date : 2012/7/23/
*/
class CX2BuffCreateBuffFactortBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_BUFF_FACTOR_ID,
		FO_SKILL_ID,
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffCreateBuffFactortBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffCreateBuffFactortBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_uiBuffFactorId( 0 )
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		, m_uiSkillId( 0 )
#endif //UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		, m_bApplyToSpecialAttack( false )
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	{}

private:
	UINT						m_uiBuffFactorId;
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	UINT						m_uiSkillId;
#endif //UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	bool						m_bApplyToSpecialAttack;	/// June 32e860a0 (+0x18): APPLY_TO_SPECIAL_ATTACK, sent as the 3rd GetFactor value
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
};

/** @class : CX2BuffAbsorbEffectAttackBehaviorTemplet
	@brief : 이펙트 공격을 흠수하여 MP 회복
	@date : 2012/7/27/
*/
class CX2BuffAbsorbEffectAttackBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_ABSORB_VALUE,			/// 발사체 한개당 회복량
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffAbsorbEffectAttackBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffAbsorbEffectAttackBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_fAbsorbValue( 0.0f )
	{}

private:
	float						m_fAbsorbValue;
};

// /** @class : CX2BuffChangeMpHitBehaviorTemplet
// 	@brief : 타격시 MP 회복량
// 	@date : 2012/7/27/
// */
// class CX2BuffChangeMpHitBehaviorTemplet	: public CX2BuffBehaviorTemplet
// {
// public:
// 	enum FACTOR_ORDER
// 	{
// 		FO_MULTIPLIER,				/// 배율
// 		FO_END						/// enum 개수
// 	};
// 
// 	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeMpHitBehaviorTemplet ); }
// 
// 	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
// 	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
// 	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
// 	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
// 	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
// 
// protected:
// 	CX2BuffChangeMpHitBehaviorTemplet() : CX2BuffBehaviorTemplet(),
// 		m_fMultiplier( 1.0f )
// 	{}
// 
// private:
// 	float						m_fMultiplier;
// };
// 
// /** @class : CX2BuffChangeMpHittedBehaviorTemplet
// 	@brief : 타격시 MP 회복량
// 	@date : 2012/7/27/
// */
// class CX2BuffChangeMpHittedBehaviorTemplet	: public CX2BuffBehaviorTemplet
// {
// public:
// 	enum FACTOR_ORDER
// 	{
// 		FO_MULTIPLIER,				/// 배율
// 		FO_END						/// enum 개수
// 	};
// 
// 	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeMpHittedBehaviorTemplet ); }
// 
// 	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
// 	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
// 	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
// 	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
// 	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
// 
// protected:
// 	CX2BuffChangeMpHittedBehaviorTemplet() : CX2BuffBehaviorTemplet(),
// 		m_fMultiplier( 1.0f ), m_eChangeType;
// 	{}
// 
// private:
// 	float						m_fMultiplier;
// 	BUFF_CHANGE_TYPE			m_eChangeType;
// };
// 
// /** @class : CX2BuffChangeHyperModeBehaviorTemplet
// 	@brief : 각성모드로 변경시키는 버프행동 (각성구슬이 있다는 전제하에 수행)
// 	@date : 2012/8/7/
// */
class CX2BuffChangeHyperModeBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_HYPER_MODE_TIME,			/// 각성유지시간
		FO_HYPER_MODE_COUNT,		/// 몇각성인가(각성했을 때의 구슬 갯수)
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeHyperModeBehaviorTemplet ); }

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeHyperModeBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_uiHyperModeCount( 0 ), m_fRemainTime( 0.0f )
	{}

private:
	UINT						m_uiHyperModeCount;	/// 몇각성인가
	float						m_fRemainTime;		/// 남은 각성 시간 보여주기용...
};

/** @class : CX2BuffResetSkillCoolTimeBehaviorTemplet
	@brief : 스킬의 쿨타임을 초기화 시켜주는 행동
	@date : 2012/8/7/
*/
class CX2BuffResetSkillCoolTimeBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_END,						/// enum 개수
	};
#endif // UPGRADE_SKILL_SYSTEM_2013

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffResetSkillCoolTimeBehaviorTemplet ); }

#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	
protected:
	CX2BuffResetSkillCoolTimeBehaviorTemplet() : CX2BuffBehaviorTemplet()
		, m_eSkillTypeToResetCoolTime( 0 )
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환
		, m_eChangeType( BCT_FIX_VALUE )
		, m_fCoolTime( 0.f )
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
		, m_fCoolTimeRate ( 1.f ) 
		, m_bIsRelativeAllSkill ( false)
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		, m_bForceSet( false ), m_vecIgnoreSkillID(), m_vecForceSkillID(), m_bApplyOnce( false ), m_bApplied( false )
		, m_bDrawText( false ), m_iSkillSlotType( 0 )
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	UINT				m_eSkillTypeToResetCoolTime;	/// 쿨타임을 리셋할 스킬타입
#ifdef UPGRADE_SKILL_SYSTEM_2013 /// 김태환
	BUFF_CHANGE_TYPE	m_eChangeType;					/// 변수 설정 타입
	float				m_fCoolTime;					/// 설정할 쿨타임
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
	float				m_fCoolTimeRate;				// 설정한 쿨타임 (배율)
	bool				m_bIsRelativeAllSkill;			// 스킬 타입과는 관계 없이 모든 장착 가능 스킬 타입에 대해 영향을 미칠 것인가?
#endif // ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME					// 김종훈 / 13-07-04 / 모든 착용 중인 스킬의 쿨타임 최대치로 설정되게 하는 BBT 추가
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// June 32e78e90 (offsets in June's class)
	bool				m_bForceSet;			/// +0x21 FORCE_SET: SWAP ignores "only lower it"; lets relationship/hyper skills through
	vector<int>			m_vecIgnoreSkillID;		/// +0x24 IGNORE_SKILL_ID: skills the reset skips
	vector<int>			m_vecForceSkillID;		/// +0x34 FORCE_SKILL_ID: skills the reset always takes
	bool				m_bApplyOnce;			/// +0x44 APPLY_ONCE: reset on the first frame only
	bool				m_bApplied;				/// +0x45 runtime: set on every reset
	bool				m_bDrawText;			/// +0x46 DRAW_TEXT: June 32e857d0 floats a text when a slot changed
	int					m_iSkillSlotType;		/// +0x48 SKILL_SLOT_TYPE: 0 any, 1 A, 2 B, 3 transcendence (none in March)

	void DrawText( CX2GameUnit* pGameUnit_, const BUFF_CHANGE_TYPE eChangeType_, const float fCoolTime_, const float fCoolTimeRate_ ) const;
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

};

/** @class : CX2BuffChangeUnitScaleBehaviorTemplet
	@brief : 유닛의 크기를 변경하는 행동 템프릿
	@date : 2012/8/7/
*/
class CX2BuffChangeUnitScaleBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeUnitScaleBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeUnitScaleBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_vScale( 0.0f, 0.0f, 0.0f ), m_bUseTimeToReturn( false )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	D3DXVECTOR3			m_vScale;
	bool				m_bUseTimeToReturn;
};

/** @class : CX2BuffCustomFunctionBehaviorTemplet
	@brief : 하드코딩된 처리를 CommonStateStart, CommonStateFrameMove, CommonStateEnd에서 수행하도록 하는 행동 템플릿
	@date : 2012/8/7/
*/
class CX2BuffCustomFunctionBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffCustomFunctionBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffCustomFunctionBehaviorTemplet() : CX2BuffBehaviorTemplet(), m_kBuffBehaviorFactor()
	{}

private:
	KBuffBehaviorFactor		m_kBuffBehaviorFactor;
};

/** @class : CX2BuffEffectSetWithDamageBehaviorTemplet
	@brief : 데미지를 주는 이펙트셋 행동 템플릿(버프를 사용한 스테이트의PowerRate와 난입한 유저가 이펙트셋을 실행 시킬때 상태의 PowerRate가 다른 경우가 많으므로 데미지가 있는 EffectSet의 경우 PowerRate를 동기화하는 Behavior 사용)
	@date : 2012/8/27/
*/
class CX2BuffEffectSetWithDamageBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffEffectSetWithDamageBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffEffectSetWithDamageBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_fPowerRate( 0.0f ), m_bNotUse( false )
	{
		m_vecHandleEffectSet.clear();
		m_vecWstrEffectSetName.clear();
	}
	
	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	void PlayEffectSet( CX2GameUnit* pGameUnit_ );

private:
	vector<CX2EffectSet::Handle>		m_vecHandleEffectSet;
	vector<wstring>						m_vecWstrEffectSetName;	/// 여러 이펙트셋을 지정가능
	float								m_fPowerRate;
	bool								m_bNotUse;				/// 템플릿에 EffectSet이 지정되어 있어도, Factor에서 이펙트셋을 사용 여부를 어떻게 지정했는지를 저장하는 변수
};

/** @class : CX2BuffStunBehaviorTemplet
	@brief : 기절 스테이트로 고정시키는 템플릿
	@date : 2012/8/29/
*/
class CX2BuffStunBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffStunBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffStunBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{}	
};

/** @class : CX2BuffFinishOtherBuffDebuffBehaviorTemplet
	@brief : 다른 버프, 디버프를 종료 시키는 행동
	@date : 2012/9/2/
*/
class CX2BuffFinishOtherBuffDebuffBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffFinishOtherBuffDebuffBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;

	/// 필요가 없을 것 같아서 삭제
	//virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	
protected:
	CX2BuffFinishOtherBuffDebuffBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{
		m_vecBuffTempletIDToBeFinished.clear();
	}
	
	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
	inline void EraseBuffTempletFromGameUnit( CX2GameUnit* pGameUnit_, const BUFF_TEMPLET_ID eExceptBuffTempletID_ );

private:
	vector<BUFF_TEMPLET_ID>		m_vecBuffTempletIDToBeFinished;
};

/** @class : CX2BuffFixStateBehaviorTemplet
	@brief : 현재 스테이트로 고정시키는 템플릿
	@date : 2012/9/4/
*/
class CX2BuffFreezeBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffFreezeBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

protected:
	CX2BuffFreezeBehaviorTemplet() : CX2BuffBehaviorTemplet()
		, m_fAnimCurrentTime( 0.0f ), m_bIsDown( false )
	{}	

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	float	m_fAnimCurrentTime;
	bool	m_bIsDown;				/// 버프 종료시 다운 스테이트로 넘어갈 것인지
};

/** @class : CX2BuffCanPassUnitBehaviorTemplet
	@brief : 버프가 진행되는 동안 유닛의 CanPassUnit을 설정 하는 행동 템플릿
	@date : 2012/9/5/
*/
class CX2BuffCanPassUnitBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffCanPassUnitBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE

protected:
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffCanPassUnitBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_bCanPassUnit( false ), m_bCanPassUnitBeforeBuff( false )
	{}

	/// June 32e72890: restores the can-pass flag SetFactor saved
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#else SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	CX2BuffCanPassUnitBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_bCanPassUnit( false )
	{}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	bool	m_bCanPassUnit;
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	bool	m_bCanPassUnitBeforeBuff;	/// June +0x11: the unit's can-pass flag when SetFactor last ran
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
};

/** @class : CX2BuffNeverMoveBehaviorTemplet
	@brief : 버프가 진행되는 동안 유닛을 제자리 고정 시키는 행동 템프릿
	@date : 2012/9/5/
*/
class CX2BuffNeverMoveBehaviorTemplet: public CX2BuffBehaviorTemplet
{
public:

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffNeverMoveBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_, float fElapsedTime_ );
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	virtual void OnFrameMove( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

protected:
#ifdef NEVER_MOVE_GRAVITY_APPLY
	CX2BuffNeverMoveBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_vPosition( 0.0f, 0.0f, 0.0f ), m_bIsGravity( false )
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		, m_bCanDown( false ), m_vOffsetPosition( 0.0f, 0.0f, 0.0f ), m_bDeleteBuffWhenDie( false )
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );
#else
	CX2BuffNeverMoveBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_vPosition( 0.0f, 0.0f, 0.0f )
	{}
#endif //NEVER_MOVE_GRAVITY_APPLY



private:
	D3DXVECTOR3		m_vPosition;
#ifdef NEVER_MOVE_GRAVITY_APPLY
	bool			m_bIsGravity;	//중력을 적용할것인가. true면 중력 적용, false면 위치 고정
#endif //NEVER_MOVE_GRAVITY_APPLY
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	bool			m_bCanDown;				/// June 32e7a4a0 (+0x1d): CAN_DOWN. Parsed and sent only; June's store takes a constant false
	D3DXVECTOR3		m_vOffsetPosition;		/// June (+0x20): OFFSET_POSITION, added to the anchor position
	bool			m_bDeleteBuffWhenDie;	/// June (+0x2c): DELETE_BUFF_WHEN_DIE, frees the anchor once the unit dies
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
};

/** @class : CX2BuffChangeAttackByTypeBehaviorTemplet
	@brief : 공격 타입에 따른 데미지를 증가시키는 행동 템프릿
	@date : 2012/11/03/
*/
class CX2BuffChangeAttackByTypeBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_ATTACK_TYPE,				/// 공격 타입
		FO_END,						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeAttackByTypeBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeAttackByTypeBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_fResultValue( 0.f ), m_eAttackType( CX2DamageManager::AT_NORMAL )
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
		, m_iAdditionalAttackType( -1 )
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE				m_eChangeType;
	float							m_fResultValue;
	CX2DamageManager::ATTACK_TYPE	m_eAttackType;
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	int								m_iAdditionalAttackType;	/// June 32e77070 (+0x1c): ADDITIONAL_ATTACK_TYPE, factor value 3 (-1 for none)
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
};

/** @class : CX2BuffChangeEncahntAttackRateBehaviorTemplet
@brief : 소켓 속성 공격 발동 확률을 증가시키는 행동 템프릿
@date : 2012/11/03/
*/
class CX2BuffChangeEncahntAttackRateBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_END,						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeEncahntAttackRateBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	virtual bool OnAccumulationCountSet( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_, const int iCount_ );	/// June 32e775e0
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeEncahntAttackRateBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_fRate( 0.f )
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
		// 기본 값은 모든 속성 강화
		, m_eEnchantAttributeType ( BEAT_ALL )
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE				m_eChangeType;
	float							m_fRate;
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	BUFF_ENCHANT_ATTRIBUTE_TYPE		m_eEnchantAttributeType;	// 인챈트 속성 타입
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
};

/** @class : CX2BuffChangeEncahntAttackRateBehaviorTemplet
@brief : 소켓 속성 공격 발동 확률을 증가시키는 행동 템프릿
@date : 2012/11/03/
*/
class CX2BuffDoubleAttackBehaviorTemplet : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_END,						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffDoubleAttackBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffDoubleAttackBehaviorTemplet() : CX2BuffBehaviorTemplet(),
		m_eChangeType( BCT_SWAP_VALUE ), m_fRate( 0.f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE				m_eChangeType;
	float							m_fRate;
};

/** @class : CX2BuffChangeAttackSphereScaleBehaviorTemplet
	@brief : 어택박스의 크기를 변경하는 행동 템플릿
	@date : 2013/03/13/
*/
class CX2BuffChangeAttackSphereScaleBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	struct AttackSphereNameAndScale
	{
		wstring		m_wstrAttackSphereName;
		float		m_fScale;

		AttackSphereNameAndScale()
			: m_fScale( 1.0f )
		{
			m_wstrAttackSphereName.clear();
		}
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeAttackSphereScaleBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeAttackSphereScaleBehaviorTemplet() : CX2BuffBehaviorTemplet()
	{
		m_vecAttackSphereNameAndScale.clear();
	}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	vector<AttackSphereNameAndScale>	m_vecAttackSphereNameAndScale;
};

/** @class : CX2BuffAddSkillLevelBehaviorTemplet
	@brief : 스킬레벨을 Up 또는 Down 시키는 행동 템플릿
	@date : 2013/04/16/
*/
class CX2BuffAddSkillLevelBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_SKILL_LEVEL = 0,				///	변화 될 스킬레벨 값
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffAddSkillLevelBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffAddSkillLevelBehaviorTemplet() 
		: CX2BuffBehaviorTemplet(), m_iAddSkillLevel( 0 )
	{}

private:
	int	m_iAddSkillLevel;		/// 상승 또는 감소 시킬 스킬 레벨
};

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환

/** @class : CX2BuffReflectMagicBehaviorTemplet
	@brief : 해당 버프가 걸려있는 동안에는 공격(일반+스킬) 불가 행동
	@date : 2013/7/10/
*/
class CX2BuffReflectMagicBehaviorTemplet	: public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,				/// 변화되는 타입
		FO_VALUE,					/// 값
		FO_END						/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffReflectMagicBehaviorTemplet ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffReflectMagicBehaviorTemplet() : CX2BuffBehaviorTemplet()
		, m_eChangeType( BCT_SWAP_VALUE )
		, m_fResultValue( 0.0f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE			m_eChangeType;
	float						m_fResultValue;
};

#endif // SERV_ARA_CHANGE_CLASS_SECOND

#ifdef FIX_SKILL_BALANCE_AISHA_LENA //JHKang
/** @class : CX2BuffChangeManaConsumeRate
	@brief : 해당 디버프가 걸려있는 스킬사용 시 마나 소비량 변화
	@date : 2013/09/09/
*/
class CX2BuffChangeConsumeMpRate : public CX2BuffBehaviorTemplet
{
public:
	enum FACTOR_ORDER
	{
		FO_CHANGE_TYPE,		/// 변화되는 타입
		FO_SKILL_ID,		/// 스킬 아이디
		FO_VALUE,			/// 값
		FO_END				/// enum 개수
	};

	static CX2BuffBehaviorTempletPtr	CreateBuffBehaviorTempletPtr() { return CX2BuffBehaviorTempletPtr( new CX2BuffChangeConsumeMpRate ); }

	virtual bool SetFactor( const CX2BuffFactor& BuffFactor_, CX2GameUnit* pGameUnit_, const CX2BuffTemplet* pBuffTemplet_ );
	virtual CX2BuffBehaviorTempletPtr GetClonePtr() const;
	virtual void GetFactor( OUT vector<KBuffBehaviorFactor>& vecFactors_ ) const;
	virtual void SetFactorFromPacket( const KBuffFactor& kBuffFactor_, const KBuffBehaviorFactor& kFactor_, CX2GameUnit* pGameUnit_ );
	virtual void DoFinish( CX2GameUnit* pGameUnit_, CX2BuffTemplet* pBuffTemplet_ );

protected:
	CX2BuffChangeConsumeMpRate() : CX2BuffBehaviorTemplet()
		, m_eChangeType( BCT_SWAP_VALUE ), m_fResultValue( 0.0f ), m_uiSkillId( 0 ), m_fInitMp( 0.f )
	{}

	virtual bool ParsingBehavior( KLuaManager& luaManager_ );

private:
	BUFF_CHANGE_TYPE	m_eChangeType;		/// 변화되는 타입
	UINT				m_uiSkillId;		/// 적용될 스킬 아이디
	float				m_fResultValue;		/// 값
	float				m_fInitMp;			/// 초기 마나 사용량
};
#endif //FIX_SKILL_BALANCE_AISHA_LENA