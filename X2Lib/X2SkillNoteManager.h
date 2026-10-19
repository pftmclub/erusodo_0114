#pragma once

/*
	#현재 선택된 캐릭터 클래스의 메모 리스트를 관리 하는 클래스
*/
#ifdef REFORM_SKILL_NOTE_UI
class CX2SkillNoteManager
{
public:
	struct MemoData
	{
		int iMemoItemID;
		int iSkillID;
		int iMemoItemNum;	// 메모 아이템 소지량
		char cClassLine;		// 정렬을 위한 클래스 라인 구분
			// 노전~2차 (0~2)
	};

	// 메모 리스트 정렬 클래스
	struct MemoListSort : public std::binary_function< MemoData, MemoData, bool >
	{
		bool operator()( const MemoData& _Left, const MemoData& _Right ) const
		{
			if( (_Left.iMemoItemNum > 0) == (_Right.iMemoItemNum > 0) )
			{
				// 전직 낮은 순 우선
				if( _Left.cClassLine < _Right.cClassLine )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if ( (_Left.iMemoItemNum > 0) > (_Right.iMemoItemNum > 0) )
			{
				return true;
			}
			else
			{
				return false;
			}
		};
	};

	struct MemoListClassSort : public std::binary_function< MemoData, MemoData, bool >
	{
		bool operator()( const MemoData& _Left, const MemoData& _Right ) const
		{
			// 전직 낮은 순 우선
			if( _Left.cClassLine < _Right.cClassLine )
			{
				return true;
			}
			else
			{
				return false;
			}
		};
	};
public:
	static CX2SkillNoteManager* GetInstance();
	static void ResetSkillNoteManager();
	static void DestroyInstance();
	///////////////////////////////////////////////////////

	// 스크립트 파싱 관련
	void AddSkillNoteTemplet_LUA();
	bool OpenScriptFile();

	// 클래스 설정
	void SetUnitClass( CX2Unit::UNIT_CLASS eUnitClass_ ) { m_eUnitClass = eUnitClass_; }
	CX2Unit::UNIT_CLASS GetUnitClass() const { return m_eUnitClass; } 

	// 메모 리스트 갱신
	void ResetMemoList(); 
	void UpdateMemoPossession();
	void UpdateSortMemoList();

	// 메모 리스트 관리
	const vector<MemoData>& GetUsableMemoListIfIHave(){ return m_vecUsableMemoListIfIhave; }
	int GetSkillIDByMemoItemID( int iMemoItemID_, bool bIsMySkill_ = true ) const;

	// ~End Public Function
private:

	// ~End Private Function
private:
	CX2SkillNoteManager();
	~CX2SkillNoteManager();
	
	static CX2SkillNoteManager*							m_pManager;
	//////////////////////////////////////////////////////

	// 현재 선택된 캐릭터의 유닛 클래스
	CX2Unit::UNIT_CLASS									m_eUnitClass; 

	// key : UnitClass,  val : ( 메모아이템ID, 스킬ID)	
	map< CX2Unit::UNIT_CLASS, vector<pair<int, int>> >	m_mapSkillNoteTemplet;	

	// (보유 했을 때)현재 선택된 캐릭터의 클래스가 사용 가능한 메모 리스트	
	vector<MemoData>									m_vecUsableMemoListIfIhave;	
};

#endif // REFORM_SKILL_NOTE_UI

#ifdef SERV_IRUHADEV_JUNE_SKILL_NOTE
#ifdef REFORM_SKILL_NOTE_UI
#error SERV_IRUHADEV_JUNE_SKILL_NOTE is written for builds where OnlyGlobal/Always_Global.h undefines REFORM_SKILL_NOTE_UI (US): merge it into the class above first
#endif REFORM_SKILL_NOTE_UI
/// June2017 CX2SkillNoteManager (0x54 bytes: singleton 324550a0 at 337a34dc, destroy 32455240, ctor 32454ed0; JUNE_SKILL_SYSTEM_PHASES.md
/// "Phase S3b"). This build undefines REFORM_SKILL_NOTE_UI (Always_Global.h:39), so March's class above, with its memo-list UI half
/// (June +0x34 / +0x44, 32452cb0 / 32450e20 / 32452ea0) and its binding, is not compiled. This class is June's loader (32452270), the
/// old AddSkillNoteTemplet table (32453160, March's body; June's data calls it 0 times) and June's memo half.
class CX2SkillNoteManager
{
public:
	static CX2SkillNoteManager* GetInstance();							/// June 324550a0
	static void ResetSkillNoteManager();
	static void DestroyInstance();										/// June 32455240

	bool OpenScriptFile();												/// June 32452270
	void AddSkillNoteTemplet_LUA();										/// June 32453160
	void AddNewSkillNoteTemplet_LUA();									/// June 32454840
	void AddMemoIDMatchItemID_LUA( int iMemoID_, int iItemID_ );		/// June 32452120
	void AddRemovableMemoItemID_LUA( int iItemID_ );					/// June 32450f20

	/// The readers. A skill id goes through 32207210 (an [Enhanced] id reads as its base) where June does it, noted per function.
	bool HasJuneMemoClass( int iUnitClass_ ) const;																	/// June 324511a0
	const SJuneSkillMemoTemplet* GetJuneMemo( int iSkillID_, int iMemoID_, int iUnitClass_ ) const;				/// June 324510e0
	const SJuneSkillMemoTemplet* GetJuneMemoByIndex( int iSkillID_, int iIndex_, int iUnitClass_ ) const;			/// June 32451140
	void GetJuneMemoList( int iSkillID_, int iRequireLevel_, int iUnitClass_, int iMemoID_,
		OUT std::vector<const SJuneSkillMemoTemplet*>& vecMemo_ ) const;											/// June 32451250
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// June 32452150 returns a bool ( ret 0xc, tested CMP AL,1 by 3235b070 ): the class has a row with the memo and the base skill.
	/// S3b read it as a value getter; it had no caller.
	bool HasJuneMemo( int iSkillID_, int iMemoID_, int iUnitClass_ ) const;										/// June 32452150
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	std::vector<float> GetJuneMemoValue( int iSkillID_, int iMemoID_, int iUnitClass_ ) const;						/// June 32452150
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	int GetJuneSkillIDByTranscendenceValue( int iValue_, int iUnitClass_ ) const;									/// June 32451ff0
	void GetJuneMemoMatchItemIDList( int iMemoID_, OUT std::vector<int>& vecItemID_ ) const;						/// June 32450f50
	void GetJuneRemovableMemoItemIDList( OUT std::vector<int>& vecItemID_ ) const;									/// June 32451210
	static int GetJuneMemoState( int iSkillID_, int iMemoID_, int iUnitLevel_, int iUnitClass_ );					/// June 32455150: 0 locked, 1 open, 2 active
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// S7c: the memo texts and the two memo requests
	static std::wstring GetJuneMemoName( int iMemoID_ );															/// June 32453420
	std::wstring GetJuneMemoEffectDesc( int iSkillID_, int iMemoID_, int iUnitClass_ ) const;						/// June 324513e0
	void SendJuneActiveMemoReq( int iSkillID_, int iMemoID_ );														/// June 32453060
	void SendJuneDeactiveMemoReq( int iSkillID_, int iMemoID_, UidType iItemUID_ );									/// June 324530d0
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

private:
	CX2SkillNoteManager();
	~CX2SkillNoteManager();

	static CX2SkillNoteManager*								m_pManager;

	/// June +0x4: UNIT_CLASS -> memo rows (the class's own rows after its parents', 32454840)
	std::map< int, std::vector<SJuneSkillMemoTemplet> >	m_mapJuneMemoTemplet;
	/// June +0x14: MEMO_ID -> item id, a multimap (31ef1410, the insert CX2UserSkillTree +0x458 also uses)
	std::multimap<int, int>								m_mmapJuneMemoMatchItemID;
	/// June +0x24: the item ids that remove a memo
	std::set<int>										m_setJuneRemovableMemoItemID;
	/// June +0x34: UNIT_CLASS -> ( memo item id, skill id ), AddSkillNoteTemplet's table (March's m_mapSkillNoteTemplet)
	std::map< int, std::vector< std::pair<int, int> > >	m_mapSkillNoteTemplet;

	/// "[JUNE_SKILL] NOTE" load counts
	int		m_iJuneOldTempletCalls;
	int		m_iJuneNewTempletCalls;
	int		m_iJuneMatchCalls;
	int		m_iJuneRemovableCalls;
	int		m_iJuneOwnRows;
	int		m_iJuneInheritedRows;
	int		m_iJuneZeroMemoRows;
	int		m_iJuneZeroSkillRows;
	int		m_iJuneMissingParents;
	int		m_iJuneDuplicateClasses;
};
#endif SERV_IRUHADEV_JUNE_SKILL_NOTE

