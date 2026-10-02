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
class CX2JuneBuffCapture : public KIruhaDisplayErrorSink
{
public:
	CX2JuneBuffCapture();
	virtual ~CX2JuneBuffCapture();
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

private:
	bool			m_bOwner;
	vector<string>	m_vecError;
};
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER

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

#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
private:
	bool AddBuffFactorINT( KLuaManager& luaManager_, const UINT uiBuffFactorID_ );	/// June 326c3f30
	void PushBuffFactor( KLuaManager& luaManager_, const UINT uiBuffFactorID_, IN OUT vector<CX2BuffFactorPtr>& vecBuffFactorPtr_, IN OUT UINT& uiPlaceholder_ );
	void LoadINTScriptFile( const wchar_t* wszScriptFileName_, const char* szJuneLoader_ );
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
	
};