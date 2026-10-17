#pragma once

class CX2UIUnitManager
{

public:
	CX2UIUnitManager();
	virtual ~CX2UIUnitManager();

	void Initialize();
	CX2UIUnitPtr CreateUIUnit( int iUnitID );
	CX2UIUnit* GetUIUnit( int iUnitID );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
	CX2UIUnitPtr CreateUIUnit( int iUnitID, const wstring& wstrFileName );	// June 32806910
	CX2UIUnit* GetUIUnit( int iUnitID, const wstring& wstrFileName );		// June 32806b30
	void ClearUIUnit( int iUnitID );										// June 32806800
#endif SERV_IRUHADEV_ELYSION_VILLAGE
	void ClearAllUIUnit();


protected:
	std::map< int, CX2UIUnitPtr > m_mapUIUnit;

};