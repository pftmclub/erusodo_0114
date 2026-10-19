#pragma once

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-10-19
// Description: June quest system Phase Q4c - dungeon result ranks offline.
//
//              The room server ranks a run with its own Lua: KResultProcess::Result_DUNGEON
//              (KncWX2Server/CenterServer/ResultProcess.cpp:1944-2048) calls the functions of
//              ResultData_new.lua (CnSimLayer.cpp:371, SERV_DUNGEON_RANK_NEW) with the room user
//              (KRoomUser) as pUnitInfo and the room's KDungeonMonsterManager as pMonsterManager.
//              Each axis function ends in pUnitInfo:SetResultData( fBonus, nRank ), and the C++ reads
//              the rank and the EXP percent back with GetRank / GetPercent.
//
//              This module loads that same file into the client's Lua state the shipped way
//              (LoadDataFile probe, then LoadAndDoMemory encrypted with a plaintext fallback, the
//              shape of X2OfflineStatTable.cpp), and binds two small objects whose method names are
//              exactly the server's Lua names (ResultProcess.cpp:68-97). Nothing in the Lua is
//              transcribed: if the file is not packed, every rank stays RT_F and the log says which
//              file is missing.
//
//              The file is server-only (KncWX2Server/ServerResource/US/ResultData_new.lua); the user
//              XOR-packs it into data036.kom.
//////////////////////////////////////////////////////////////////////////

#ifdef SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK

/// pUnitInfo: the KRoomUser getters the 8 functions call (RoomUser.h:128, :251-260; bound at
/// ResultProcess.cpp:69-86). Every figure is the client's own KDungeonPlayResultInfo, which is
/// what KRoomUser holds (RoomUser.cpp:421), except the base HP: the room user's game stat
/// (m_kRoomUserInfo.m_kGameStat.m_iBaseHP, base + gear).
class CX2OfflineRankUnit
{
public:
	CX2OfflineRankUnit( const KDungeonPlayResultInfo& kPlay, int iBaseHP );

	int		GetComboScore_LUA()				{ return m_kPlay.m_ComboScore; }
	int		GetTechnicalScore_LUA()			{ return m_kPlay.m_TechScore; }
	int		GetBaseHP_LUA()					{ return m_iBaseHP; }
	int		GetTotalGivenDamamge_LUA()		{ return m_kPlay.m_iTotalGivenDamamge; }
	int		GetTotalAttackedDamage_LUA()	{ return m_kPlay.m_iTotalAttackedDamage; }
	int		GetTotalRecoveryHP_LUA()		{ return m_kPlay.m_iTotalRecoveryHP; }
	int		GetUsingKindOfSkill_LUA()		{ return m_kPlay.m_iUsingKindOfSkill; }

	/// KRoomUser::SetResultData_LUA (RoomUser.h:286). m_bSet tells a call that ran from one that
	/// errored ( lua_tinker::call returns 0 either way ).
	void	SetResultData_LUA( double fPercent, int nRank )	{ m_fPercent = (float)fPercent; m_nRank = nRank; m_bSet = true; }

	KDungeonPlayResultInfo	m_kPlay;
	int						m_iBaseHP;
	float					m_fPercent;
	int						m_nRank;
	bool					m_bSet;
};

/// pMonsterManager: KDungeonMonsterManager's five die counts (DungeonMonsterManager.h:65-69; bound at
/// ResultProcess.cpp:90-95 under the Lua class name KRoomMonsterManager).
class CX2OfflineRankMonsters
{
public:
	enum DIE_TYPE
	{
		DT_NORMAL = 0,
		DT_LOW_ELITE,
		DT_HIGH_ELITE,
		DT_MIDDLE_BOSS,
		DT_BOSS,
		DT_END,
	};

	explicit CX2OfflineRankMonsters( const int aiDie[ DT_END ] );

	int		GetNormalNpcDieCount_LUA()		{ return m_aiDie[ DT_NORMAL ]; }
	int		GetLowEliteNpcDieCount_LUA()	{ return m_aiDie[ DT_LOW_ELITE ]; }
	int		GetHighEliteNpcDieCount_LUA()	{ return m_aiDie[ DT_HIGH_ELITE ]; }
	int		GetMiddleBossDieCount_LUA()		{ return m_aiDie[ DT_MIDDLE_BOSS ]; }
	int		GetBossDieCount_LUA()			{ return m_aiDie[ DT_BOSS ]; }

	/// CXSLUnitManager::GetMonsterTypeFactor (XSLUnitManager.cpp:238-269) with bIsAttribNpc false:
	/// the emulator places no attribute monsters (Handlers_Room.cpp m_mapAttribNpcInfo.clear() at
	/// every spawn), and the server's other source of one is admin-create only
	/// (DungeonRoom.cpp:9349, SERV_CREATE_ELITE_FOR_ADMIN). MG_MIDDLE_BOSS_NPC 2 -> middle boss,
	/// MG_BOSS_NPC 1 -> boss, anything else -> normal.
	static DIE_TYPE	DieTypeOfGrade( char cMonsterGrade );

	int		m_aiDie[ DT_END ];
};

class CX2OfflineDungeonRank
{
public:
	static CX2OfflineDungeonRank*	Instance();
	static void						Release();

	/// One run's ranks, Result_DUNGEON's alive branch (ResultProcess.cpp:1944-2048) for a party of one.
	/// Fills the five rank fields of kUnit and fRankBonusRate ( the four percents summed, capped at
	/// 0.15, :2046-2048 ). False ( kUnit untouched, rate 0 ) when ResultData_new.lua is not loaded.
	bool	Compute( int iDungeonIDAndDif, int iPlaySec, const KDungeonPlayResultInfo& kPlay, int iBaseHP,
					 const int aiDie[ CX2OfflineRankMonsters::DT_END ],
					 OUT KDungeonUnitResultInfo& kUnit, OUT float& fRankBonusRate );

	/// ResultProcess:Add*( ... ) at the top of the file (ResultProcess.cpp:57-62). The party bonus
	/// rates they set feed party EXP only; offline is a party of one, so they are logged and kept.
	void	AddDungeonPartyBonus_LUA( float fTwo, float fThree, float fFour );
	void	AddDungeonPartyDifficultyBonus_LUA( float fNormal, float fHard, float fExpert );
	void	AddDungeonModePartyBonus_LUA( float fNormal, float fHenir, float fSecret );

private:
	enum AXIS
	{
		AXIS_COMBO = 0,
		AXIS_TECHNICAL,
		AXIS_DAMAGED,
		AXIS_TIME,
	};

	CX2OfflineDungeonRank();
	~CX2OfflineDungeonRank();

	bool	EnsureLoaded();
	bool	HasFunctions( lua_State* pLuaState ) const;

	/// One axis. The client's lua_tinker::call ( LuaCommon/lua_tinker.h:1015-1300 ) pushes nil on both error paths, so
	/// the stack stays balanced; the pad and lua_settop are a belt only ( KNCSDK's copy, the server's, is the one whose
	/// error path eats a slot ). Returns the rank SetResultData set, or RT_F with a log line.
	int		CallAxis( lua_State* pLuaState, AXIS eAxis, CX2OfflineRankUnit& kUnit,
					  CX2OfflineRankMonsters& kMonsters, int iDungeonIDAndDif, int iPlaySec,
					  int iEndNumMember, int iPartyTotalGivenDamage, OUT float& fPercent );

	static CX2OfflineDungeonRank*	ms_pInstance;

	bool	m_bLoadAttempted;
	bool	m_bLoaded;
	int		m_iAddCalls;
};

#endif SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK
