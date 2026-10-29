/**@file : X2BuffTempletManager.h
   @breif : 버프 템플릿을 관리하는 매니저 클래스 정의 헤더
*/

#pragma once

#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
/** @class : CX2JuneBuffCapture
	@brief : June buff port Phase 3. Collects the DISPLAY_ERROR texts raised while one buff entry
		parses (the KTDXCommonFunc.h sink, per thread) so they become one [JUNE_BUFF] line in
		ErrorLog.txt instead of modal MessageBoxes. A capture nested inside another on the same
		thread is a no-op; the outermost one owns the entry and reports it.
*/
class CX2BuffCapture : public KIruhaDisplayErrorSink
{
public:
	CX2BuffCapture();
	virtual ~CX2BuffCapture();
	virtual void OnDisplayError( const char* szError_ );

	bool IsOwner() const { return m_bOwner; }

	/// Parse failed: "[JUNE_BUFF] unsupported <reason> in <szEntry_> -> <szOutcome_>: <every text>".
	/// Parsed but raised errors anyway: "[JUNE_BUFF] warning in <szEntry_>: <every text>". Else silent.
	void Report( const bool bParsed_, const char* szEntry_, const char* szOutcome_ ) const;

	/// "[JUNE_BUFF] <printf>" to ErrorLog.txt
	static void Line( const char* szFormat_, ... );
	/// For a factory/parse switch's default branch: raises "unsupported <wszKind_> <uiType_>" (captured
	/// like any DISPLAY_ERROR, so it becomes the line's reason) and returns false like DISPLAY_ERROR.
	static bool UnsupportedType( const WCHAR* wszKind_, const UINT uiType_ );
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
	/// June buff port 4b-ii (the user's 2026-10-04 decision: follow June, no popup, always log). DISPLAY_ERROR in
	/// the buff runtime files lands here: inside a capture it is collected as before; otherwise it raises no
	/// MessageBox (June's buff code raises none) and writes "[JUNE_BUFF] runtime error, BTI <n>: <text>" to
	/// ErrorLog.txt once per buff and text (CX2JuneBuffRuntimeScope names the buff). Returns false.
	static bool RuntimeError( const WCHAR* wszError_ );
	static bool RuntimeError( const char* szError_ );	/// DisplayErrorMessage's narrow overload, same rules
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

private:
	bool			m_bOwner;
	vector<string>	m_vecError;
};
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @class : CX2JuneBuffRuntimeScope
	@brief : June buff port 4b-ii. Names the buff (BTI) whose runtime code runs on this thread, for the line
		CX2JuneBuffCapture::RuntimeError writes and for its once-per-buff-and-text dedupe. Scopes nest: the
		innermost names the buff and the previous name comes back when it ends. 0 = no buff named.
*/
class CX2BuffRuntimeScope
{
public:
	explicit CX2BuffRuntimeScope( const int iBuffTempletID_ );
	~CX2BuffRuntimeScope();
	static int GetBuffTempletID();

private:
	int		m_iPrevBuffTempletID;
};
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/** @class : CX2BuffTempletManager
	@brief : 버프 템플릿을 관리하는 매니저 클래스
	@date : 2012/7/17/
*/
class CX2BuffTempletManager
{
public:
	static bool GetCombination( KLuaManager& luaManager_, OUT vector<UINT>& vecType_ );

public:
	static CX2BuffTempletManager* GetInstance()
	{
		if ( NULL == pInstance )
			pInstance = new CX2BuffTempletManager;
		
		return pInstance;
	}

	static void DestroyInstance() { SAFE_DELETE( pInstance ); }

private:
	static CX2BuffTempletManager* pInstance;

public:
	void OpenScriptFileForTemplet();
	void OpenScriptFileForFactor();

	void AddBuffTemplet_LUA();
	void AddBuffFactor_LUA();

	CX2BuffTempletPtr GetBuffTempletPtr( const BUFF_TEMPLET_ID eBuffTempletID_ ) const;
	CX2BuffFactorPtr GetBuffFactorPtr( const UINT uiBuffFactorNumber_ ) const;

	void GetBuffFactorPtrFromBuffFactorList( IN KLuaManager& luaManager_, OUT vector<CX2BuffFactorPtr>& vecBuffFactorPtr_, OUT vector<UINT>& vecBuffFactorID_ );
//{{ Iruha : 2026-10-05 // June buff port Phase 10 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_GROUP
	/// June's BUFF_GROUP_APPLY_DEFAULT_VALUE (Enum.lua): what a group does with a BTI its list does not name
	enum BUFF_GROUP_APPLY_DEFAULT_VALUE
	{
		BGADV_NONE					= 0,
		BGADV_ACTIVE				= 1,
		BGADV_ACTIVE_JUST_BUFF		= 2,
		BGADV_ACTIVE_JUST_DEBUFF	= 3,
		BGADV_DEACTIVE				= 4,
		BGADV_END					= 5,
	};

	void OpenScriptFileForBuffGroup();		/// June 326c26f0: UnitApplyBuffGroup.lua, right after the factor files
	void AddBuffGroup_LUA();				/// June 326c3450
	void AddOfficiallyBuffList_LUA();		/// June 326bfd80
	bool IsApplyBuffByGroup( const BUFF_TEMPLET_ID eBuffTempletID_, const int iBuffGroupID_ ) const;	/// June 326c31c0
	bool IsOfficiallyDebuff( const BUFF_TEMPLET_ID eBuffTempletID_ ) const;	/// June 326bfd50
#endif SERV_IRUHADEV_JUNE_BUFF_GROUP
//}}

#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
private:
	bool AddBuffFactorINT( KLuaManager& luaManager_, const UINT uiBuffFactorID_ );	/// June 326c3f30
	void PushBuffFactor( KLuaManager& luaManager_, const UINT uiBuffFactorID_, IN OUT vector<CX2BuffFactorPtr>& vecBuffFactorPtr_, IN OUT UINT& uiPlaceholder_ );
	void LoadINTScriptFile( const wchar_t* wszScriptFileName_, const char* szLoader_ );
	void LogLoadSummary() const;
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER

private:
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	CX2BuffTempletManager()
		: m_uiTempletFailed( 0 )
		, m_uiTempletDuplicate( 0 )
		, m_uiFactorPlaceholder( 0 )
		, m_uiFactorPlaceholderINT( 0 )
		, m_uiFactorOrdinalReject( 0 )
		, m_uiFactorOrdinalRejectINT( 0 )
	{}
#else SERV_IRUHADEV_JUNE_BUFF_LOADER
	CX2BuffTempletManager()
	{}
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER

private:

	map<BUFF_TEMPLET_ID, CX2BuffTempletPtr>	m_mapBuffTempletPtr;
	vector<CX2BuffFactorPtr>				m_vecBuffFactorPtr;		/// 버프에 의해서 발생하는 버프 팩터용
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	vector<CX2BuffFactorPtr>				m_vecBuffFactorPtrINT;	/// June mgr+0x20: _INT factors, id 10001+ at [id-10001] (326c3f30, 326c32a0)
	UINT	m_uiTempletFailed;			/// templets whose ParsingScript failed (not inserted, as in June/March)
	UINT	m_uiTempletDuplicate;		/// parsed templets whose id was already in the map (the first is kept)
	UINT	m_uiFactorPlaceholder;		/// NULL slots in m_vecBuffFactorPtr: passed the ordinal check, failed to parse
	UINT	m_uiFactorPlaceholderINT;	/// same, m_vecBuffFactorPtrINT
	UINT	m_uiFactorOrdinalReject;	/// ids that failed size == id-1 (dropped, as in June/March)
	UINT	m_uiFactorOrdinalRejectINT;	/// ids >= 10000 that failed size == id-10001
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
//{{ Iruha : 2026-10-05 // June buff port Phase 10 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_GROUP
	/// June's 0x18-byte group entry (326c3450 builds it, 326c4ff0 pushes it)
	struct BuffGroup
	{
		int								m_iBuffGroupID;			/// BUFF_GROUP_ID (+0)
		int								m_iApplyDefaultValue;	/// BUFF_GROUP_APPLY_DEFAULT_VALUE (+4)
		map<BUFF_TEMPLET_ID, bool>		m_mapBuffTemplet;		/// BUFF_TEMPLET_LIST { BTI, apply } (+8)
		BuffGroup() : m_iBuffGroupID( 0 ), m_iApplyDefaultValue( 0 ) {}
	};
	vector<BuffGroup>			m_vecBuffGroup;				/// June mgr+0x30, in load order (326c31c0 takes the first id match)
	std::set<BUFF_TEMPLET_ID>	m_setOfficiallyDebuff;		/// June mgr+0x40; replaces CX2Game's hard-coded list
#endif SERV_IRUHADEV_JUNE_BUFF_GROUP
//}}
	
};