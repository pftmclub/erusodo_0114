#include "stdafx.h"

#include "X2UIUnitManager.h"

CX2UIUnitManager::CX2UIUnitManager()
{
	Initialize();
}

CX2UIUnitManager::~CX2UIUnitManager()
{
	ClearAllUIUnit();
}

void CX2UIUnitManager::Initialize()
{
	CreateUIUnit( CX2UnitManager::NUI_ANNE );	
}

CX2UIUnitPtr CX2UIUnitManager::CreateUIUnit( int iUnitID )
{
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
	return CreateUIUnit( iUnitID, std::wstring() );		// June's other callers pass L""
#else SERV_IRUHADEV_ELYSION_VILLAGE
	const CX2UnitManager::NPCUnitTemplet* pNPCTemplet = 
		g_pData->GetUnitManager()->GetNPCUnitTemplet( (CX2UnitManager::NPC_UNIT_ID) iUnitID );

	if( pNPCTemplet == NULL )
		return CX2UIUnitPtr();

	ASSERT( false == pNPCTemplet->m_LuaFileNameUI.empty() );
	CX2UIUnitPtr pkUnit = CX2UIUnit::CreateUIUnit( pNPCTemplet->m_LuaFileNameUI.c_str() );
	ASSERT( pkUnit != NULL );
	m_mapUIUnit[iUnitID] = pkUnit;

	return pkUnit;
#endif SERV_IRUHADEV_ELYSION_VILLAGE
}

CX2UIUnit* CX2UIUnitManager::GetUIUnit( int iUnitID )
{
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
	return GetUIUnit( iUnitID, std::wstring() );
#else SERV_IRUHADEV_ELYSION_VILLAGE
	CX2UIUnitPtr pkUnit;

	std::map< int, CX2UIUnitPtr >::iterator iU = m_mapUIUnit.find( iUnitID );
	if ( iU == m_mapUIUnit.end() )
	{
		pkUnit = CreateUIUnit( iUnitID );
	}
	else
	{
		pkUnit = (*iU).second;
	}

	return pkUnit.get();
#endif SERV_IRUHADEV_ELYSION_VILLAGE
}

#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
// June 32806910: an empty name builds from the NPC templet's m_LuaFileNameUI; a non-empty one from that file, marked as an override.
CX2UIUnitPtr CX2UIUnitManager::CreateUIUnit( int iUnitID, const wstring& wstrFileName )
{
	if( NULL == g_pData || NULL == g_pData->GetUnitManager() )
		return CX2UIUnitPtr();

	const CX2UnitManager::NPCUnitTemplet* pNPCTemplet =
		g_pData->GetUnitManager()->GetNPCUnitTemplet( (CX2UnitManager::NPC_UNIT_ID) iUnitID );
	if( pNPCTemplet == NULL )
		return CX2UIUnitPtr();

	CX2UIUnitPtr pkUnit;
	if( true == wstrFileName.empty() )
	{
		pkUnit = CX2UIUnit::CreateUIUnit( pNPCTemplet->m_LuaFileNameUI.c_str() );
	}
	else
	{
		pkUnit = CX2UIUnit::CreateUIUnit( wstrFileName.c_str() );
		if( pkUnit != NULL )
			pkUnit->SetFileNameOverride( true );
	}
	m_mapUIUnit[iUnitID] = pkUnit;

	return pkUnit;
}

// June 32806b30: a cached unit is rebuilt when the requested file name differs from what it was built with (32ee37a0).
CX2UIUnit* CX2UIUnitManager::GetUIUnit( int iUnitID, const wstring& wstrFileName )
{
	CX2UIUnitPtr pkUnit;

	std::map< int, CX2UIUnitPtr >::iterator iU = m_mapUIUnit.find( iUnitID );
	if ( iU == m_mapUIUnit.end() )
	{
		pkUnit = CreateUIUnit( iUnitID, wstrFileName );
	}
	else if( (*iU).second != NULL && true == (*iU).second->IsFileNameChanged( iUnitID, wstrFileName ) )
	{
		m_mapUIUnit.erase( iU );
		pkUnit = CreateUIUnit( iUnitID, wstrFileName );
	}
	else
	{
		pkUnit = (*iU).second;
	}

	return pkUnit.get();
}

// June 32806800: erase one NPC id's cached unit (the bye step uses it instead of ClearAllUIUnit).
void CX2UIUnitManager::ClearUIUnit( int iUnitID )
{
	std::map< int, CX2UIUnitPtr >::iterator iU = m_mapUIUnit.find( iUnitID );
	if ( iU != m_mapUIUnit.end() )
		m_mapUIUnit.erase( iU );
}
#endif SERV_IRUHADEV_ELYSION_VILLAGE


void CX2UIUnitManager::ClearAllUIUnit()
{
	m_mapUIUnit.clear();
}
