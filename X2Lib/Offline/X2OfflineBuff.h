#ifdef SERV_IRUHADEV_OFFLINE_ITEM_BUFF
#pragma once

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-10-05
// Description: Offline mode - the item-granted premium buffs (June buff port
//              Phase 11, SERV_IRUHADEV_OFFLINE_ITEM_BUFF; user decision B,
//              2026-10-05: the emulator owns them, as the GameServer does).
//
//              This is the March GameServer's SERV_SERVER_BUFF_SYSTEM, reduced
//              to one player. The transcription source and every citation is in
//              JUNE_BUFF_PORT_ARCHIVE.md, "Phase 11 - the March server's buff
//              system":
//
//                BuffTemplet.lua (+ _INT)  CXSLBuffManager::AddBuffInfo_LUA
//                                          (XSLBuffManager.cpp:34), read the
//                                          server's way - the client's
//                                          CX2PremiumBuffManager keeps none of
//                                          BUFF_FACTOR / m_bEndBuffAt*.
//                the active buffs          KUserServerBuffManager::m_mapBuff,
//                                          persisted in els_db.sql (unit_buff,
//                                          schema v14) because live keeps them in
//                                          the LoginServer's RAM across sessions.
//                an item's buff            KGSUser::ActivateItemBuff
//                                          (GSUserFunction.cpp:17117) and the
//                                          pre-checks of EGS_USE_ITEM_IN_INVENTORY
//                                          (GSUserInventory.cpp:4394-4468).
//
//              The server sends the client the list, never the effect: the
//              client's own CX2PremiumBuffManager world-buff handlers apply each
//              buff's factor (EGS_UPDATE_BUFF_INFO_IN_ROOM_NOT) or show its icon
//              (EGS_UPDATE_BUFF_INFO_NOT). CX2OfflineServer::SendUpdateBuffInfo
//              (Handlers_Buff.cpp) is the sender.
//
//              BuffTemplet.lua calls BuffManager:AddBuffInfo({...}), the same
//              global the client's CX2PremiumBuffManager binds. This object takes
//              the global for its own run and hands it back afterwards, so
//              neither parser ever swallows the other's rows.
//////////////////////////////////////////////////////////////////////////


#include "X2OfflineDB.h"

class CX2OfflineBuff
{
public:
	/// One BuffTemplet.lua row as CXSLBuffManager::BuffTemplet holds it.
	struct KTemplet
	{
		int					m_iBuffID;
		double				m_fBuffPeriodSecond;
		int					m_iFactorID;			///< the first BUFF_FACTOR entry, if > 0
		std::vector< int >	m_vecRoomType;			///< USING_BUFF_ROOM_TYPE, entries >= 0
		std::vector< int >	m_vecOption;			///< SOCKET_OPTION, entries > 0

		bool				m_bEndBuffAtTheLeaveDungeonRoom;
		bool				m_bEndBuffAtTheEndDungeonGame;
		bool				m_bEndBuffAtTheLeavePvpRoom;
		bool				m_bEndBuffAtTheEndPvpGame;		///< parsed only, as live (no reader)
		bool				m_bEndBuffAtTheUserDied;
		bool				m_bKeepBuffAtTheUserLogOut;		///< parsed only, as live (no reader)
		bool				m_bEndBuffAtTheLeaveBattleFieldRoom;

#ifdef SERV_IRUHADEV_JUNE_BUFF_NOT_USE_IN
		/// June's server-only keys (stand-in, user decision 2026-10-05): HENIR is live,
		/// PVE (June game type 4) and HUGE_BOSS_RAID never fire here.
		bool				m_bNotUseInPve;
		bool				m_bNotUseInHenir;
		bool				m_bNotUseInHugeBossRaid;
#endif SERV_IRUHADEV_JUNE_BUFF_NOT_USE_IN

		KTemplet();

		/// CXSLBuffManager::BuffTemplet::CheckRoomType (XSLBuffManager.cpp:140): an
		/// empty USING_BUFF_ROOM_TYPE is never in any room's list.
		bool	CheckRoomType( int iRoomType ) const;
	};

	static CX2OfflineBuff*	Instance();
	static void				Release();

	/// The two files, once per process. Logs the row count per file, or which
	/// file is missing and what to do about it.
	void	EnsureLoaded();

	/// Bound into Lua as BuffManager:AddBuffInfo while this object owns the global.
	bool	AddBuffInfo_LUA();

	const KTemplet*	GetTemplet( int iBuffID );

	/// EGS_USE_ITEM_IN_INVENTORY_REQ's pre-checks for the buff items
	/// (GSUserInventory.cpp:4394-4468). false = refuse with iErr, item kept.
	bool	CheckUseItem( UidType nUnitUID, int iItemID, __int64 iTempCode, OUT int& iErr );

	/// KGSUser::ActivateItemBuff. False for an item that is not a server buff item
	/// (the caller then leaves it to the item's own effect); true when the item
	/// is one, whether or not every buff in its group could be activated.
	bool	ActivateItemBuff( UidType nUnitUID, int iItemID, int iNumItemBuffFactor );

	/// The end flags. Each returns true when it removed something (the caller
	/// then resends the list).
	bool	OnLeaveRoom( UidType nUnitUID, int iRoomType );	///< OnLeaveDungeonRoom / OnLeaveBattleField / OnLeavePVPRoom
	bool	OnEndDungeonGame( UidType nUnitUID );
	bool	OnUserDied( UidType nUnitUID );

	/// KUserServerBuffManager::CheckBuffPeriod: start != end and now > end.
	bool	CheckBuffPeriod( UidType nUnitUID );

	/// KUserServerBuffManager::GetBuffList. iRoomType < 0 = unfiltered. bHenir
	/// drives the NOT_USE_IN_HENIR stand-in.
	void	GetBuffList( UidType nUnitUID, int iRoomType, bool bHenir, OUT std::vector< KBuffInfo >& vecOut );

private:
	CX2OfflineBuff();

	bool	ActivateBuff( UidType nUnitUID, int iBuffID, __int64 tUseTime );	///< KGSUser::ActivateBuff + the manager's
	bool	DeactivateBuff( UidType nUnitUID, int iBuffID );					///< KGSUser::DeactivateBuff (silent when not active)
	bool	IsBuffActivated( UidType nUnitUID, int iBuffID );

	bool	RunScript( const wchar_t* wszFileName );

	static CX2OfflineBuff*		ms_pInstance;

	bool						m_bLoadAttempted;
	std::map< int, KTemplet >	m_mapTemplet;
	int							m_iRowsThisFile;
};

#endif SERV_IRUHADEV_OFFLINE_ITEM_BUFF
