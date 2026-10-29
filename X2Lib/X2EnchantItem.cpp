#include "StdAfx.h"
#include ".\x2enchantitem.h"


CX2EnchantItem::CX2EnchantItem(void)
{
	OpenScriptFile( L"AttribEnchantItem.lua" );
	OpenScriptFile( L"AttribEnchantRequire.lua" );
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	OpenScriptFile( L"AttribEnchantRequireED.lua" );	/// June ctor 3270eca0: the ED cost moved into Lua
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}

CX2EnchantItem::~CX2EnchantItem(void)
{
#ifndef X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	EnchantDataMap::iterator iter;
	for( iter = m_mapEnchantData.begin(); iter != m_mapEnchantData.end(); iter++ )
	{
		EnchantData* pEnchantData = iter->second;
		SAFE_DELETE( pEnchantData );
	}
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	m_mapEnchantData.clear();

#ifndef X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	for( iter = m_mapEnchantDataForNPC.begin(); iter != m_mapEnchantDataForNPC.end(); iter++ )
	{
		EnchantData* pEnchantData = iter->second;
		SAFE_DELETE( pEnchantData );
	}
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	m_mapEnchantDataForNPC.clear();

	
}

bool CX2EnchantItem::OpenScriptFile( WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2EnchantItem", this );

//{{ robobeg : 2008-10-28
	//KLuaManager luaManager;
    //KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//}} robobeg : 2008-10-28
	return g_pKTDXApp->LoadLuaTinker( pFileName );	
}

const CX2EnchantItem::EnchantData* CX2EnchantItem::GetEnchantData( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType ) const
{
	EnchantDataMap::const_iterator mit;
	mit = m_mapEnchantData.find( extraDamageType );
	if ( mit != m_mapEnchantData.end() )
	{
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        return &mit->second;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
		return mit->second;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	}

	return NULL;
}

const CX2EnchantItem::EnchantData* CX2EnchantItem::GetNPCEnchantData( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType ) const
{
	EnchantDataMap::const_iterator mit;
	mit = m_mapEnchantDataForNPC.find( extraDamageType );
	if ( mit != m_mapEnchantDataForNPC.end() )
	{
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        return &mit->second;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
		return mit->second;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	}

	return NULL;
}

bool CX2EnchantItem::AddEnchantData_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
    CX2EnchantItem::EnchantData* pEnchantData = NULL;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	CX2EnchantItem::EnchantData* pEnchantData = new CX2EnchantItem::EnchantData();
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR

    CX2DamageManager::EXTRA_DAMAGE_TYPE eExtrDamageType;
	LUA_GET_VALUE_ENUM( luaManager, "m_ExtraDamageType", eExtrDamageType, CX2DamageManager::EXTRA_DAMAGE_TYPE,	CX2DamageManager::EDT_NONE );

	bool bIsForNPC = false;
	LUA_GET_VALUE(		luaManager, "bIsForNPC",					bIsForNPC,					false );
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
    if ( bIsForNPC == false )
    {
        EnchantDataMap::_Pairib ib = m_mapEnchantData.insert( EnchantDataMap::value_type( eExtrDamageType, EnchantData() ) );
        if ( ib.second == false )
            return false;
        pEnchantData = &ib.first->second;
    }
    else
    {
        EnchantDataMap::_Pairib ib = m_mapEnchantDataForNPC.insert( EnchantDataMap::value_type( eExtrDamageType, EnchantData() ) );
        if ( ib.second == false )
            return false;
        pEnchantData = &ib.first->second;
    }
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
    pEnchantData->m_ExtraDamageType = eExtrDamageType;
	
	LUA_GET_VALUE(		luaManager, "m_fRate",						pEnchantData->m_fRate,		0.0f );
	LUA_GET_VALUE(		luaManager, "m_Time",						pEnchantData->m_Time,		0 );

	LUA_GET_VALUE(		luaManager, "m_FirstDamagePercent",		pEnchantData->m_FirstDamagePercent,		0 );
	LUA_GET_VALUE(		luaManager, "m_SecondDamagePercent",		pEnchantData->m_SecondDamagePercent,	0 );
	LUA_GET_VALUE(		luaManager, "m_ThirdDamagePercent",		pEnchantData->m_ThirdDamagePercent,		0 );

	LUA_GET_VALUE(		luaManager, "m_FirstSlowPercent",			pEnchantData->m_FirstSlowPercent,		0 );
	LUA_GET_VALUE(		luaManager, "m_SecondSlowPercent",			pEnchantData->m_SecondSlowPercent,		0 );
	LUA_GET_VALUE(		luaManager, "m_ThirdSlowPercent",			pEnchantData->m_ThirdSlowPercent,		0 );

	LUA_GET_VALUE(		luaManager, "m_FirstDefenceDebuffPercent",			pEnchantData->m_FirstDefenceDebuffPercent,		0 );
	LUA_GET_VALUE(		luaManager, "m_SecondDefenceDebuffPercent",		pEnchantData->m_SecondDefenceDebuffPercent,		0 );
	LUA_GET_VALUE(		luaManager, "m_ThirdDefenceDebuffPercent",			pEnchantData->m_ThirdDefenceDebuffPercent,		0 );


	LUA_GET_VALUE(		luaManager, "m_fTimeStun",					pEnchantData->m_fTimeStun,				0.0f );
	
	LUA_GET_VALUE(		luaManager, "m_HPDrainPercent",			pEnchantData->m_HPDrainPercent,			0 );
	LUA_GET_VALUE(		luaManager, "m_MPDrain",					pEnchantData->m_MPDrain,				0 );
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 32708f00: five scalars and three int lists after m_MPDrain; a missing key reads 0. June's list loop
	/// has no bound (a 4th entry would overwrite the next field); the port stops at 3.
	LUA_GET_VALUE(		luaManager, "m_fAttribReiterationRate",	pEnchantData->m_fAttribReiterationRate,	0.0f );
	LUA_GET_VALUE(		luaManager, "m_fBlazeMpDrain",			pEnchantData->m_fBlazeMpDrain,			0.0f );
	LUA_GET_VALUE(		luaManager, "m_fPiercingDamageFactor",	pEnchantData->m_fPiercingDamageFactor,	0.0f );
	LUA_GET_VALUE(		luaManager, "m_iShockDebuffTime",		pEnchantData->m_iShockDebuffTime,		0 );
	LUA_GET_VALUE(		luaManager, "m_fShockDebuffDamageFactor",	pEnchantData->m_fShockDebuffDamageFactor,	0.0f );
	if ( luaManager.BeginTable( "m_arrMovementSpeedDecrease" ) == true )
	{
		for ( int i = 1, iValue = 0; i <= 3 && luaManager.GetValue( i, iValue ) == true; i++ )
			pEnchantData->m_arrMovementSpeedDecrease[i - 1] = static_cast<float>( iValue );
		luaManager.EndTable();
	}
	if ( luaManager.BeginTable( "m_arrJumpSpeedDecrease" ) == true )
	{
		for ( int i = 1, iValue = 0; i <= 3 && luaManager.GetValue( i, iValue ) == true; i++ )
			pEnchantData->m_arrJumpSpeedDecrease[i - 1] = static_cast<float>( iValue );
		luaManager.EndTable();
	}
	if ( luaManager.BeginTable( "m_arrAttackSpeedDecrease" ) == true )
	{
		for ( int i = 1, iValue = 0; i <= 3 && luaManager.GetValue( i, iValue ) == true; i++ )
			pEnchantData->m_arrAttackSpeedDecrease[i - 1] = static_cast<float>( iValue );
		luaManager.EndTable();
	}
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

#ifndef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
	if ( bIsForNPC == false )
		m_mapEnchantData.insert( std::make_pair( pEnchantData->m_ExtraDamageType, pEnchantData ) );
	else
		m_mapEnchantDataForNPC.insert( std::make_pair( pEnchantData->m_ExtraDamageType, pEnchantData ) );
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR

	return true;
}

bool CX2EnchantItem::AddEnchantRequire_LUA( int iAttribEnchantType, int iCharLv )
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	std::map< int, int > mapItemGradeTable;
	std::pair< int, int > pairKey;
	int iLow = 0;
	int iNormal = 0;
	int iRare = 0;
	int iElite = 0;
	int iUnique = 0;
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	int iLegend = 0;	/// June 3270bb90: LEGEND -> IG_LEGEND, optional (a missing key reads 0, no fail_proc)
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	// 테이블 키값
	pairKey.first = iAttribEnchantType;
	pairKey.second = iCharLv;

	// 아이템별 요구 수량
	LUA_GET_VALUE_RETURN( luaManager, "LOW",		iLow,	 0, goto fail_proc; );
	LUA_GET_VALUE_RETURN( luaManager, "NORMAL",		iNormal, 0, goto fail_proc; );
	LUA_GET_VALUE_RETURN( luaManager, "RARE",		iRare,	 0, goto fail_proc; );
	LUA_GET_VALUE_RETURN( luaManager, "ELITE",		iElite,	 0, goto fail_proc; );
	LUA_GET_VALUE_RETURN( luaManager, "UNIQUE",		iUnique, 0, goto fail_proc; );
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	LUA_GET_VALUE( luaManager, "LEGEND",		iLegend, 0 );
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_LOW,	  iLow ) );
	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_NORMAL, iNormal ) );
	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_RARE,   iRare ) );
	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_ELITE,  iElite ) );
	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_UNIQUE, iUnique ) );
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	mapItemGradeTable.insert( std::make_pair( CX2Item::IG_LEGEND, iLegend ) );
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	m_mapEnchantRequireInfo.insert( std::make_pair( pairKey, mapItemGradeTable ) );
	return true;

fail_proc:
	START_LOG( cerr, L"스크립트 파싱 실패!" )
		<< BUILD_LOG( iLow )
		<< BUILD_LOG( iNormal )
		<< BUILD_LOG( iRare )
		<< BUILD_LOG( iElite )
		<< BUILD_LOG( iUnique )
		<< END_LOG;
	return false;
}



bool CX2EnchantItem::GetAttribEnchantRequireMagicStoneCount( bool bWeapon, int iCurrEnchantedCount, int iEqipLevel, int iItemGrade, int& iRequireCount )
{
	std::pair< int, int > pairKey;
	pairKey.first  = GetAttribEnchantType( bWeapon, iCurrEnchantedCount );
	pairKey.second = iEqipLevel;

	MapEnchantRequire::const_iterator mit;
	mit = m_mapEnchantRequireInfo.find( pairKey );
	if( mit == m_mapEnchantRequireInfo.end() )
		return false;

	std::map< int, int >::const_iterator mitGrade;
	mitGrade = mit->second.find( iItemGrade );
	if( mitGrade == mit->second.end() )
		return false;

	iRequireCount = mitGrade->second;
	return true;
}

bool CX2EnchantItem::GetAttribEnchantRequireED( bool bWeapon, int iCurrEnchantedCount, int iEqipLevel, int iItemGrade, int& iRequireED )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 327089e0: AttribEnchantRequireED.lua's GetAttribEnchantRequiredED( count, bWeapon, level, grade ),
	/// truncated to int, always true. Without the script, March's formula below runs and says so once.
	{
		lua_State* pLuaState = g_pKTDXApp->GetLuaBinder()->GetLuaState();
		lua_getglobal( pLuaState, "GetAttribEnchantRequiredED" );
		const bool bHasFunction = ( lua_isfunction( pLuaState, -1 ) != 0 );
		lua_pop( pLuaState, 1 );
		if ( true == bHasFunction )
		{
			iRequireED = static_cast<int>( lua_tinker::call<double>( pLuaState, "GetAttribEnchantRequiredED", iCurrEnchantedCount, bWeapon, iEqipLevel, iItemGrade ) );
			return true;
		}
		static bool s_bMissingLogged = false;
		if ( false == s_bMissingLogged )
		{
			s_bMissingLogged = true;
			CX2BuffCapture::Line( "enchant: GetAttribEnchantRequiredED is not loaded (AttribEnchantRequireED.lua missing from data036); March's ED formula is used" );
		}
	}
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	if( true == bWeapon )
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	// 맨 처음 속성을 설치하는 경우
			{
				iRequireED = ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100;
				return true;
			} break;

		case 1: // 2번째 속성을 설치하는 경우
			{
				iRequireED = ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 3;
				return true;

			} break;

		case 2: // 3번째 속성을 설치하는 경우
			{
				iRequireED = ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 6;
				return true;

			} break;

		case 3: // 3번째 속성을 빼는 경우
			{
				iRequireED = ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 12;
				return true;

			} break;


		default:
			{
				ASSERT( !"NO" );
			} break;
		}
	}
	else
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	// 맨 처음 속성을 설치하는 경우
			{
				iRequireED = static_cast<int>( ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 0.25f );
				return true;

			} break;

		case 1: // 2번째 속성을 설치하는 경우
			{
				iRequireED = static_cast<int>( ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 0.75f );
				return true;

			} break;

		case 2: // 2번째 속성을 빼는 경우
			{
				iRequireED = static_cast<int>( ( ( iEqipLevel + GetRequireEDFactor( iItemGrade ) ) * 100 ) * 1.5f );
				return true;

			} break;

		default:
			{
				ASSERT( !"NO" );
			} break;
		}
	}

	return false;
}








CX2DamageManager::EXTRA_DAMAGE_TYPE CX2EnchantItem::GetExtraDamageType( CX2EnchantItem::ENCHANT_TYPE eEnchantedType )
{
	switch( eEnchantedType )
	{
	case CX2EnchantItem::ET_BLAZE:		
		{
			return CX2DamageManager::EDT_ENCHANT_BLAZE;
		} break;
	case CX2EnchantItem::ET_WATER:		
		{
			return CX2DamageManager::EDT_ENCHANT_FROZEN;
		} break;
	case CX2EnchantItem::ET_NATURE:		
		{
			return CX2DamageManager::EDT_ENCHANT_POISON;
		} break;
	case CX2EnchantItem::ET_WIND:			
		{
			return CX2DamageManager::EDT_ENCHANT_PIERCING;
		} break;
	case CX2EnchantItem::ET_LIGHT:		
		{
			return CX2DamageManager::EDT_ENCHANT_SHOCK;
		} break;
	case CX2EnchantItem::ET_DARK:		
		{
			return CX2DamageManager::EDT_ENCHANT_SNATCH;
		} break;
	}



	return CX2DamageManager::EDT_NONE;
}



CX2DamageManager::EXTRA_DAMAGE_TYPE CX2EnchantItem::GetExtraDamageType( const std::vector< CX2EnchantItem::ENCHANT_TYPE >& vecAttribEnchant )
{
	if( vecAttribEnchant.empty() == true )
		return CX2DamageManager::EDT_NONE;

	CX2EnchantItem::ItemEnchantedAttribute enchantedAttribute;
	for( int i=0; i<(int)vecAttribEnchant.size(); i++ )
	{
		if( i >= 3 )
			break;

		enchantedAttribute.m_aEnchantedType[i] = vecAttribEnchant[i];
	}

	return GetExtraDamageType( enchantedAttribute );
}







// aAttribEnchant : CX2EnchantItem::ENCHANT_TYPE의 배열
CX2DamageManager::EXTRA_DAMAGE_TYPE CX2EnchantItem::GetExtraDamageType( const CX2EnchantItem::ItemEnchantedAttribute& enchantedAttribute )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	// June 32707ce0: the combined type is the sum of each slot's element value. June loops over 4 slots,
	// March items have 3. A sum that names no EDT (for example blaze + piercing + shock) is returned as is,
	// as in June; March mapped that one to EDT_ENCHANT_BLAZE_PIERCING.
	int iComposedEDT = 0;
	for ( int iSlot = 0; iSlot < ARRAY_SIZE( enchantedAttribute.m_aEnchantedType ); iSlot++ )
	{
		switch( enchantedAttribute.m_aEnchantedType[iSlot] )
		{
		case CX2EnchantItem::ET_BLAZE:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_BLAZE;	break;
		case CX2EnchantItem::ET_WATER:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_FROZEN;	break;
		case CX2EnchantItem::ET_NATURE:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_POISON;	break;
		case CX2EnchantItem::ET_WIND:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_PIERCING;	break;
		case CX2EnchantItem::ET_LIGHT:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_SHOCK;	break;
		case CX2EnchantItem::ET_DARK:	iComposedEDT += CX2DamageManager::EDT_ENCHANT_SNATCH;	break;
		}
	}
	return (CX2DamageManager::EXTRA_DAMAGE_TYPE) iComposedEDT;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	int attribEnchantCount[CX2EnchantItem::ET_END] = { 0, };
	for ( int i = 0; i < ARRAY_SIZE( enchantedAttribute.m_aEnchantedType) ; i++ )
	{
		CX2EnchantItem::ENCHANT_TYPE eEnchantedType = enchantedAttribute.m_aEnchantedType[i];
		if( eEnchantedType >= CX2EnchantItem::ET_PREFIX_START &&
			eEnchantedType <= CX2EnchantItem::ET_SUFFIX_END )
		{
			attribEnchantCount[ eEnchantedType ] += 1;
		}
	}


#ifdef TRIPLE_ENCHANT_TEST

	if( 3 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER;
	}
	else if( 3 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER;
	}
	else if( 3 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER;
	}
	else if( 3 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING;
	}
	else if( 3 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK;
	}
	else if( 3 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH;
	}
	// 나중에 triple enchant 코드 적용되고 나면 여기 if들 정리해야함
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] )
	{
		return CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_WIND ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_WATER ] )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH;
	}
	else if ( 2 == attribEnchantCount[ CX2EnchantItem::ET_DARK ] && 1 == attribEnchantCount[ CX2EnchantItem::ET_NATURE ] )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH;
	}
	else 

#endif TRIPLE_ENCHANT_TEST



	if ( attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_WIND ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_DARK ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WATER ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WATER ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_WIND ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WATER ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WATER ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_DARK ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_NATURE ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_NATURE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_WIND ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_PIERCING;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_NATURE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_SHOCK;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_NATURE ] == 1 && attribEnchantCount[ CX2EnchantItem::ET_DARK ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON_SNATCH;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WIND ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_DARK ] == 2 )
	{
		return CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_BLAZE ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_BLAZE;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WATER ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_FROZEN;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_NATURE ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_POISON;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_WIND ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_PIERCING;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_LIGHT ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_SHOCK;
	}
	else if ( attribEnchantCount[ CX2EnchantItem::ET_DARK ] == 1 )
	{
		return CX2DamageManager::EDT_ENCHANT_SNATCH;
	}


	return CX2DamageManager::EDT_NONE;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}












CX2DamageManager::EXTRA_DAMAGE_TYPE CX2EnchantItem::GetExtraDamageType( CX2EnchantItem::ItemEnchantedAttribute currEnchantedAttribute, CX2EnchantItem::ENCHANT_TYPE newEnchantType )
{
	bool bDone = false;
	for( int i=0; i<ARRAY_SIZE( currEnchantedAttribute.m_aEnchantedType ) ; i++ )
	{
		if( currEnchantedAttribute.m_aEnchantedType[i] <= 0 )
		{
			currEnchantedAttribute.m_aEnchantedType[i] = newEnchantType;
			bDone = true;
			break;
		}
	}
	ASSERT( true == bDone );

	return GetExtraDamageType( currEnchantedAttribute );
}


int CX2EnchantItem::GetItemID( ENCHANT_TYPE enchantType )
{
	switch(enchantType)
	{
	case ET_BLAZE:
		{
			return ATI_RED;
		} break;
	case ET_WATER:
		{
			return ATI_BLUE;
		} break;
	case ET_NATURE:
		{
			return ATI_GREEN;
		} break;
	case ET_WIND:
		{
			return ATI_WIND;
		} break;
	case ET_LIGHT:
		{
			return ATI_LIGHT;
		} break;
	case ET_DARK:
		{
			return ATI_DARK;
		} break;

	case ET_RANDOM:
		{
			return ATI_UNKNOWN;
		} break;
	}

	return 0;
}

/*static*/ bool CX2EnchantItem::CanEnchantAttribute( const CX2EnchantItem::ItemEnchantedAttribute& currEnchantedAttribute, CX2EnchantItem::ENCHANT_TYPE newEnchantType )
{
	// 서로 다른 prefix가 2개 이상이거나
	// 서로 다른 suffix가 2개 이상인 경우에 enchant 할 수 없다.

	int attribEnchantCount[CX2EnchantItem::ET_END] = { 0, };
	for ( int i = 0; i < ARRAY_SIZE( currEnchantedAttribute.m_aEnchantedType) ; i++ )
	{
		CX2EnchantItem::ENCHANT_TYPE eEnchantedType = currEnchantedAttribute.m_aEnchantedType[i];
		if( eEnchantedType >= CX2EnchantItem::ET_PREFIX_START &&
			eEnchantedType <= CX2EnchantItem::ET_SUFFIX_END )
		{
			attribEnchantCount[ eEnchantedType ] += 1;
		}
	}


	if( newEnchantType >= CX2EnchantItem::ET_PREFIX_START &&
		newEnchantType <= CX2EnchantItem::ET_SUFFIX_END )
	{
		attribEnchantCount[ newEnchantType ] += 1;
	}




	//////////////////////////////////////////////////////////////////////////
	int iPrefixCount = 0;
	int iSuffixCount = 0;

	for( int i=0; i<ARRAY_SIZE(attribEnchantCount); i++ )
	{
		CX2EnchantItem::ENCHANT_TYPE eEnchantedType = (CX2EnchantItem::ENCHANT_TYPE) i;
		if( eEnchantedType >= ET_PREFIX_START && eEnchantedType <= ET_PREFIX_END )
		{
			if( attribEnchantCount[i] > 0 )
				++iPrefixCount;
		}
		else if( eEnchantedType >= ET_SUFFIX_START && eEnchantedType <= ET_SUFFIX_END )
		{
			if( attribEnchantCount[i] > 0 )
				++iSuffixCount;
		}
	}

	if( iPrefixCount > 1 || iSuffixCount > 1 )
	{
		return false;
	}

	return true;
}



// fRandValue1, fRandValue2 : 0~1사이의 값입니다
bool CX2EnchantItem::ProcessEnchant( CX2GameUnit* pDefender, CX2DamageManager::DamageData* pDamageData, float fRandValue1, float fRandValue2 )
{
	KTDXPROFILE();

	if( NULL == pDamageData )
		return false;

#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 32708b00 @32708b40: the attacker's slot 111 (an NPC with USE_OWNERS_ENCHANT answers with its owner's weapon EDT)
	if( null != pDamageData->optrAttackerGameUnit &&
	    pDamageData->optrAttackerGameUnit->GetWeaponEnchantExtraDamageType() == CX2DamageManager::EDT_NONE )
		return false;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	if( null != pDamageData->optrAttackerGameUnit &&
	    pDamageData->optrAttackerGameUnit->GetEnchantExtraDamageType() == CX2DamageManager::EDT_NONE )
		return false;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	if ( pDamageData->attackType == CX2DamageManager::AT_SPECIAL )
		return false;



#ifdef GRAPPLING_TEST
	switch( pDamageData->reActType )
	{
	case CX2DamageManager::RT_GRAPPLE:
	case CX2DamageManager::RT_THROW:
		{
			return false;
		} break;
	}
#endif GRAPPLING_TEST



	CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
	CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
	int iCount = 0;
	
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	if( null != pDamageData->optrAttackerGameUnit )	/// June @32708b94: slot 111, as above
		iCount = DecomposeEnchantExtraDamage( pDamageData->optrAttackerGameUnit->GetWeaponEnchantExtraDamageType(), eDecomposedEDT1, eDecomposedEDT2 );
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	if( null != pDamageData->optrAttackerGameUnit )
		iCount = DecomposeEnchantExtraDamage( pDamageData->optrAttackerGameUnit->GetEnchantExtraDamageType(), eDecomposedEDT1, eDecomposedEDT2 );
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY


	CX2DamageManager::EXTRA_DAMAGE_TYPE eBaseDecomposedEDT1 = CX2DamageManager::EDT_NONE;
	CX2DamageManager::EXTRA_DAMAGE_TYPE eBaseDecomposedEDT2 = CX2DamageManager::EDT_NONE;


	bool bGetExtraDamage = false;

	if( NULL != pDefender )
	{
		if( 1 == iCount )
		{
			eBaseDecomposedEDT1 = GetBaseExtraDamageType( eDecomposedEDT1 );

			bGetExtraDamage = pDefender->ProcessEnchantExtraDamage( pDamageData, eBaseDecomposedEDT1, eDecomposedEDT1, fRandValue1 );
		}
		else if( 2 == iCount )
		{
			eBaseDecomposedEDT1 = GetBaseExtraDamageType( eDecomposedEDT1 );
			eBaseDecomposedEDT2 = GetBaseExtraDamageType( eDecomposedEDT2 );

			bGetExtraDamage = pDefender->ProcessEnchantExtraDamage( pDamageData, eBaseDecomposedEDT1, eDecomposedEDT1, fRandValue1 );
			if ( pDefender->ProcessEnchantExtraDamage( pDamageData, eBaseDecomposedEDT2, eDecomposedEDT2, fRandValue2 ) == true )
				bGetExtraDamage = true;
		}
		else
		{
			ASSERT( !"impossible" );
		}
	}

	return bGetExtraDamage;
	
}
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
/// June buff port Phase 15: June 32708c50 (g_pMain state 9 = XS_PVP_GAME and the connected channel's class 9 = PCC_FREE -> 0)
/*static*/ bool CX2EnchantItem::IsEnchantStatScaleOn()
{
	if ( NULL != g_pMain && CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID() &&
		KPVPChannelInfo::PCC_FREE == g_pMain->GetPVPChannelClass( g_pMain->GetConnectedChannelID() ) )
		return false;
	return true;
}
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

//{{ JHKang // 강정훈 // 2010. 10. 6 // 자동 줄나누기 두 번 실행 방지
#ifdef FIX_TOOLTIP
wstring CX2EnchantItem::GetToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem, bool bShowName /*= true*/, bool bTotal /*= false*/ )
#else
wstring CX2EnchantItem::GetToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem, bool bShowName )
#endif FIX_TOOLTIP
//}} JHKang // 강정훈 // 2010. 10. 6 // 자동 줄나누기 두 번 실행 방지
{
	if( CX2DamageManager::EDT_NONE == extraDamageType )
		return L"";


	wstring enchantDesc = L"";
	if ( bShowName == true )
	{
		enchantDesc = g_pData->GetDamageManager()->GetExtraDamageName( extraDamageType );

		if ( enchantDesc.empty() == false )
		{
			enchantDesc = L"[" + enchantDesc;
			enchantDesc = enchantDesc + L"]";
			enchantDesc += L"\n";
		}
	}


	CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
	CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
	int iCount = DecomposeEnchantExtraDamage( extraDamageType, eDecomposedEDT1, eDecomposedEDT2 );

	if( 1 == iCount )
	{
		enchantDesc += GetToolTipForPure( eDecomposedEDT1, pItem );
	}
	else if( 2 == iCount )
	{
		enchantDesc += GetToolTipForPure( eDecomposedEDT1, pItem );
		enchantDesc += GetToolTipForPure( eDecomposedEDT2, pItem );
	}
	else
	{
		ASSERT( !"not possible" );
	}

#ifdef FIX_TOOLTIP
	if ( false == bTotal )
	{
		const int iWidth = static_cast<int>( DXUTGetBackBufferSurfaceDesc()->Width / 3 / g_pKTDXApp->GetResolutionScaleX() );
#ifdef CLIENT_GLOBAL_LINEBREAK
		enchantDesc = CWordLineHandler::GetStrByLineBreakColorInX2Main( enchantDesc.c_str(), iWidth, SLOT_MANAGER_FONT_INDEX );
#else //CLIENT_GLOBAL_LINEBREAK
		enchantDesc = g_pMain->GetStrByLineBreakColor( enchantDesc.c_str(), iWidth, SLOT_MANAGER_FONT_INDEX );
#endif //CLIENT_GLOBAL_LINEBREAK
	}
#endif FIX_TOOLTIP

	return enchantDesc;
}

wstring CX2EnchantItem::GetEnchantResistName( CX2DamageManager::EXTRA_DAMAGE_TYPE enchantedCombination, bool bTwoLine /*= false*/ )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 3270ca30 (emulated): masters/double masters use June's strings (general.ess, same text in March's);
	/// all 27 combined types decompose (March named 9); 4-slot types get STR_ID_EMPTY as in June
	int iStringIndex = STR_ID_EMPTY;
	switch( enchantedCombination )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:	iStringIndex = STR_ID_144;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:	iStringIndex = STR_ID_145;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON:	iStringIndex = STR_ID_146;	break;
	case CX2DamageManager::EDT_ENCHANT_PIERCING:	iStringIndex = STR_ID_147;	break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:	iStringIndex = STR_ID_148;	break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:	iStringIndex = STR_ID_149;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:	iStringIndex = 43298;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:	iStringIndex = 43299;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:	iStringIndex = 43300;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:	iStringIndex = 43301;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:	iStringIndex = 43302;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:	iStringIndex = 43303;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:	iStringIndex = 43277;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:	iStringIndex = 43278;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:	iStringIndex = 43279;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:	iStringIndex = 43280;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:	iStringIndex = 43281;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:	iStringIndex = 43282;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			DecomposeEnchantExtraDamage( enchantedCombination, eDecomposedEDT1, eDecomposedEDT2 );
			wstring wstrName = L"";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT1 )
				wstrName += GetEnchantResistName( eDecomposedEDT1 );
			if( false == wstrName.empty() )
				wstrName += ( true == bTwoLine ) ? L"\n" : L" + ";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT2 )
				wstrName += GetEnchantResistName( eDecomposedEDT2 );
			return wstrName;
		} break;
	}
	return GET_STRING( iStringIndex );
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	int iStringIndex = STR_ID_EMPTY;

	switch( enchantedCombination )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:						iStringIndex = STR_ID_144; 		break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:						iStringIndex = STR_ID_145; 		break;
	case CX2DamageManager::EDT_ENCHANT_POISON:						iStringIndex = STR_ID_146; 		break;
	case CX2DamageManager::EDT_ENCHANT_PIERCING:					iStringIndex = STR_ID_147; 		break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:						iStringIndex = STR_ID_148; 		break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:						iStringIndex = STR_ID_149; 		break;



	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:				iStringIndex = STR_ID_4757; 	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:				iStringIndex = STR_ID_4758; 	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:				iStringIndex = STR_ID_4759; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:				iStringIndex = STR_ID_4760; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:				iStringIndex = STR_ID_4761; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:				iStringIndex = STR_ID_4762; 	break;




	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:				
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:				
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:		
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			int iCount = DecomposeEnchantExtraDamage( enchantedCombination, eDecomposedEDT1, eDecomposedEDT2 );

			ASSERT( CX2DamageManager::EDT_NONE != eDecomposedEDT1 );
			ASSERT( CX2DamageManager::EDT_NONE != eDecomposedEDT2 );


			wstring wstrName = L"";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT1 ) 
			{
				wstrName += GetEnchantResistName( eDecomposedEDT1 );
			}

			if( false == wstrName.empty() )
			{
				if( true == bTwoLine )
				{
					wstrName += L"\n";
				}
				else
				{
					wstrName += L" + ";
				}
			}

			if( CX2DamageManager::EDT_NONE != eDecomposedEDT2 ) 
			{
				wstrName += GetEnchantResistName( eDecomposedEDT2 );
			}
			
			return wstrName;
		} break;
	}

	return GET_STRING( iStringIndex );
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}




wstring CX2EnchantItem::GetEnchantResistToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE enchantedCombination, bool bTwoLine /*= false*/ )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 3270d0f0 (emulated): masters/double masters use June's strings (general.ess, same text in March's);
	/// all 27 combined types decompose (March named 9); 4-slot types get STR_ID_EMPTY as in June
	int iStringIndex = STR_ID_EMPTY;
	switch( enchantedCombination )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:	iStringIndex = STR_ID_150;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:	iStringIndex = STR_ID_151;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON:	iStringIndex = STR_ID_152;	break;
	case CX2DamageManager::EDT_ENCHANT_PIERCING:	iStringIndex = STR_ID_153;	break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:	iStringIndex = STR_ID_154;	break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:	iStringIndex = STR_ID_155;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:	iStringIndex = STR_ID_4763;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:	iStringIndex = STR_ID_4764;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:	iStringIndex = STR_ID_4765;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:	iStringIndex = STR_ID_4766;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:	iStringIndex = STR_ID_4767;	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:	iStringIndex = STR_ID_4768;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:	iStringIndex = 43271;	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:	iStringIndex = 43272;	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:	iStringIndex = 43273;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:	iStringIndex = 43274;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:	iStringIndex = 43275;	break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:	iStringIndex = 43276;	break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			DecomposeEnchantExtraDamage( enchantedCombination, eDecomposedEDT1, eDecomposedEDT2 );
			wstring wstrName = L"";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT1 )
				wstrName += GetEnchantResistToolTip( eDecomposedEDT1 );
			if( false == wstrName.empty() )
				wstrName += ( true == bTwoLine ) ? L"\n" : L", ";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT2 )
				wstrName += GetEnchantResistToolTip( eDecomposedEDT2 );
			return wstrName;
		} break;
	}
	return GET_STRING( iStringIndex );
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	int iStringIndex = STR_ID_EMPTY;

	switch( enchantedCombination )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:						iStringIndex = STR_ID_150; 		break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:						iStringIndex = STR_ID_151; 		break;
	case CX2DamageManager::EDT_ENCHANT_POISON:						iStringIndex = STR_ID_152; 		break;
	case CX2DamageManager::EDT_ENCHANT_PIERCING:					iStringIndex = STR_ID_153; 		break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:						iStringIndex = STR_ID_154; 		break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:						iStringIndex = STR_ID_155; 		break;



	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:				iStringIndex = STR_ID_4763; 	break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:				iStringIndex = STR_ID_4764; 	break;
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:				iStringIndex = STR_ID_4765; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:				iStringIndex = STR_ID_4766; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:				iStringIndex = STR_ID_4767; 	break;
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:				iStringIndex = STR_ID_4768; 	break;


	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:				
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:				
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:			
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:		
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			int iCount = DecomposeEnchantExtraDamage( enchantedCombination, eDecomposedEDT1, eDecomposedEDT2 );

			ASSERT( CX2DamageManager::EDT_NONE != eDecomposedEDT1 );
			ASSERT( CX2DamageManager::EDT_NONE != eDecomposedEDT2 );


			wstring wstrName = L"";
			if( CX2DamageManager::EDT_NONE != eDecomposedEDT1 ) 
			{
				wstrName += GetEnchantResistToolTip( eDecomposedEDT1 );
			}

			if( false == wstrName.empty() )
			{
				if( true == bTwoLine )
				{
					wstrName += L"\n";
				}
				else
				{
					wstrName += L", ";
				}
			}

			if( CX2DamageManager::EDT_NONE != eDecomposedEDT2 ) 
			{
				wstrName += GetEnchantResistToolTip( eDecomposedEDT2 );
			}

			return wstrName;
		} break;
	}

	return GET_STRING( iStringIndex );
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

}

wstring CX2EnchantItem::GetToolTipForPure( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 3270d7b0 (decoded: june2017_re funcmap / Phase 15 Done note): every value comes from EnchantData; the
	/// "weapon's average attack power" lines print DamagePercent x 2. June still scans the sockets and runs
	/// CalculateItemStat into a local nothing reads (no x87 stack load in the function); that dead block is not ported.
	const CX2EnchantItem::EnchantData* pEnchantData = GetEnchantData( extraDamageType );
	if ( pEnchantData == NULL )
		return L"";
	if ( NULL == pItem )
		return L"";

	wstring tempDesc = L"";
	switch( extraDamageType )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_BLAZE_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43283, "fif", pEnchantData->m_fRate * 100.0f, pEnchantData->m_Time, pEnchantData->m_fAttribReiterationRate * 100.0f ) );
			wstrstm << GET_REPLACED_STRING( ( 43284, "ff", pEnchantData->m_FirstDamagePercent * 2.0f, pEnchantData->m_fBlazeMpDrain ) );
			wstrstm << GET_REPLACED_STRING( ( 43285, "ff", pEnchantData->m_SecondDamagePercent * 2.0f, pEnchantData->m_fBlazeMpDrain ) );
			wstrstm << GET_REPLACED_STRING( ( 43286, "ff", pEnchantData->m_ThirdDamagePercent * 2.0f, pEnchantData->m_fBlazeMpDrain ) );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_FROZEN_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43287, "fif", pEnchantData->m_fRate * 100.0f, pEnchantData->m_Time, pEnchantData->m_fAttribReiterationRate * 100.0f ) );
			wstrstm << GET_REPLACED_STRING( ( 43288, "ii", static_cast<int>( pEnchantData->m_arrAttackSpeedDecrease[0] ), pEnchantData->m_FirstDefenceDebuffPercent ) );
			wstrstm << GET_REPLACED_STRING( ( 43289, "ii", static_cast<int>( pEnchantData->m_arrAttackSpeedDecrease[1] ), pEnchantData->m_SecondDefenceDebuffPercent ) );
			wstrstm << GET_REPLACED_STRING( ( 43290, "ii", static_cast<int>( pEnchantData->m_arrAttackSpeedDecrease[2] ), pEnchantData->m_ThirdDefenceDebuffPercent ) );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_POISON_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43291, "fif", pEnchantData->m_fRate * 100.0f, pEnchantData->m_Time, pEnchantData->m_fAttribReiterationRate * 100.0f ) );
			wstrstm << GET_REPLACED_STRING( ( 43292, "fi", pEnchantData->m_FirstDamagePercent * 2.0f, static_cast<int>( pEnchantData->m_arrMovementSpeedDecrease[0] ) ) );
			wstrstm << GET_REPLACED_STRING( ( 43293, "fi", pEnchantData->m_SecondDamagePercent * 2.0f, static_cast<int>( pEnchantData->m_arrMovementSpeedDecrease[1] ) ) );
			wstrstm << GET_REPLACED_STRING( ( 43294, "fi", pEnchantData->m_ThirdDamagePercent * 2.0f, static_cast<int>( pEnchantData->m_arrMovementSpeedDecrease[2] ) ) );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_PIERCING_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43295, "ff", pEnchantData->m_fRate * 100.0f, pEnchantData->m_fPiercingDamageFactor ) );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SHOCK_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43296, "ffif", pEnchantData->m_fRate * 100.0f, pEnchantData->m_fTimeStun, pEnchantData->m_iShockDebuffTime, pEnchantData->m_fShockDebuffDamageFactor * 100.0f ) );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SNATCH_MASTER:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( 43297, "fii", pEnchantData->m_fRate * 100.0f, pEnchantData->m_HPDrainPercent * 2, pEnchantData->m_MPDrain ) );
			wstrstm << GET_STRING( STR_ID_163 );
			tempDesc += wstrstm.str().c_str();
		}
		break;

	default:
		break;
	}
	return tempDesc;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	const CX2EnchantItem::EnchantData* pEnchantData = GetEnchantData( extraDamageType );
	if ( pEnchantData == NULL )
		return L"";

#ifdef PVP_SEASON2_SOCKET
	//float fOrgDamage = 0.f;
	float fAtkPhysic = 0.f;
	float fAtkMagic = 0.f;
	int iAddLevel = 0;
	bool bLevelLinkedStat = false;

	if ( NULL == pItem )
		return L"";

    UINT uiNumSocketOption = pItem->GetItemTemplet()->GetNumSocketOption();
	for ( UINT i = 0; i < uiNumSocketOption; ++i )
	{
        int socketOption = pItem->GetItemTemplet()->GetSocketOption(i);
		if( 0 == socketOption )
			continue;

		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOption );
		if( pSocketData == NULL )
			continue;

		if ( NULL != pSocketData && true == pSocketData->m_bIsLevelLinkedStat )
		{
			fAtkPhysic += pSocketData->m_Stat.m_fAtkPhysic;
			fAtkMagic += pSocketData->m_Stat.m_fAtkMagic;
			iAddLevel = pSocketData->m_iAddLevel;
			bLevelLinkedStat = true;
		}
	}
#endif

	wstring tempDesc = L"";
	switch( extraDamageType )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
		{
			wstringstream wstrstm;

			wstrstm << GET_REPLACED_STRING( ( STR_ID_156, "f", pEnchantData->m_fRate * 100.0f ) );

			if ( pItem != NULL )
			{
#ifdef PVP_SEASON2_SOCKET
				float fOrgDamage = 0.f;

				if ( true == bLevelLinkedStat )
					fOrgDamage = ( fAtkPhysic + fAtkMagic ) * ( g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel() + iAddLevel );
				else
                {
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
					// 스탯테이블을 통해 스탯 계산
					CX2Item::KItemFormatStatData kCalculateStat;
					g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
					const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
					const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05

#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
                }

#else
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
					// 스탯테이블을 통해 스탯 계산
					CX2Item::KItemFormatStatData kCalculateStat;
					g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
					const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
					const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
#endif
				float fFinalDamage = fOrgDamage * pEnchantData->m_FirstDamagePercent / 100.0f;

#ifdef FIX_TOOLTIP
				wstrstm << GET_REPLACED_STRING( ( STR_ID_157, "if", pEnchantData->m_Time, fFinalDamage ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_SecondDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_158, "if", pEnchantData->m_Time, fFinalDamage ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_ThirdDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_159, "if", pEnchantData->m_Time, fFinalDamage ) );
#else
				wstrstm << GET_REPLACED_STRING( ( STR_ID_157, "fi", fFinalDamage, pEnchantData->m_Time ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_SecondDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_158, "fi", fFinalDamage, pEnchantData->m_Time ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_ThirdDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_159, "fi", fFinalDamage, pEnchantData->m_Time ) );
#endif FIX_TOOLTIP
				tempDesc += wstrstm.str().c_str();
			}
			else
			{
#ifdef FIX_TOOLTIP
				wstrstm << GET_REPLACED_STRING( ( STR_ID_160, "ii", pEnchantData->m_Time, pEnchantData->m_FirstDamagePercent ) );
				wstrstm << GET_REPLACED_STRING( ( STR_ID_161, "ii", pEnchantData->m_Time, pEnchantData->m_SecondDamagePercent ) );
				wstrstm << GET_REPLACED_STRING( ( STR_ID_162, "ii", pEnchantData->m_Time, pEnchantData->m_ThirdDamagePercent ) );
#else
                wstrstm << GET_REPLACED_STRING( ( STR_ID_160, "ii", pEnchantData->m_FirstDamagePercent, pEnchantData->m_Time ) );
				wstrstm << GET_REPLACED_STRING( ( STR_ID_161, "ii", pEnchantData->m_SecondDamagePercent, pEnchantData->m_Time ) );
				wstrstm << GET_REPLACED_STRING( ( STR_ID_162, "ii", pEnchantData->m_ThirdDamagePercent, pEnchantData->m_Time ) );
#endif FIX_TOOLTIP
				
				wstrstm << GET_STRING( STR_ID_163 );

				tempDesc += wstrstm.str().c_str();
			}
			
		}
		break;


	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
		{
			wstringstream wstrstm;

            wstrstm << GET_REPLACED_STRING( ( STR_ID_164, "f", pEnchantData->m_fRate * 100.0f ) );

#ifdef FIX_TOOLTIP
			wstrstm << GET_REPLACED_STRING( ( STR_ID_165, "iii", pEnchantData->m_Time, pEnchantData->m_FirstSlowPercent, pEnchantData->m_FirstDefenceDebuffPercent ) );
			wstrstm << GET_REPLACED_STRING( ( STR_ID_166, "iii", pEnchantData->m_Time, pEnchantData->m_SecondSlowPercent, pEnchantData->m_SecondDefenceDebuffPercent ) );
			wstrstm << GET_REPLACED_STRING( ( STR_ID_167, "iii", pEnchantData->m_Time, pEnchantData->m_ThirdSlowPercent, pEnchantData->m_ThirdDefenceDebuffPercent ) );
#else
			wstrstm << GET_REPLACED_STRING( ( STR_ID_165, "iii", pEnchantData->m_FirstSlowPercent, pEnchantData->m_FirstDefenceDebuffPercent, pEnchantData->m_Time ) );
			wstrstm << GET_REPLACED_STRING( ( STR_ID_166, "iii", pEnchantData->m_SecondSlowPercent, pEnchantData->m_SecondDefenceDebuffPercent, pEnchantData->m_Time ) );
			wstrstm << GET_REPLACED_STRING( ( STR_ID_167, "iii", pEnchantData->m_ThirdSlowPercent, pEnchantData->m_ThirdDefenceDebuffPercent, pEnchantData->m_Time ) );
#endif FIX_TOOLTIP
			tempDesc += wstrstm.str().c_str();
			
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
		{
			wstringstream wstrstm;

			wstrstm << GET_REPLACED_STRING( ( STR_ID_168, "f", pEnchantData->m_fRate * 100.0f ) );

			if ( pItem != NULL )
			{
#ifdef PVP_SEASON2_SOCKET
				float fOrgDamage = 0.f;

				if ( true == bLevelLinkedStat )
					fOrgDamage = ( fAtkPhysic + fAtkMagic ) * ( g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel() + iAddLevel );
				else
                {
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
						// 스탯테이블을 통해 스탯 계산
						CX2Item::KItemFormatStatData kCalculateStat;
						g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
						const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
						const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
                }
#else
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
					// 스탯테이블을 통해 스탯 계산
					CX2Item::KItemFormatStatData kCalculateStat;
					g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
					const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
					const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
#endif
				float fFinalDamage = fOrgDamage * pEnchantData->m_FirstDamagePercent / 100.0f;
				
#ifdef FIX_TOOLTIP
				wstrstm << GET_REPLACED_STRING( ( STR_ID_169, "iif", pEnchantData->m_Time, pEnchantData->m_FirstSlowPercent, fFinalDamage  ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_SecondDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_171, "iif", pEnchantData->m_Time, pEnchantData->m_SecondSlowPercent, fFinalDamage ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_ThirdDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_172, "iif", pEnchantData->m_Time, pEnchantData->m_ThirdSlowPercent, fFinalDamage ) );
#else
				wstrstm << GET_REPLACED_STRING( ( STR_ID_169, "fi", fFinalDamage, pEnchantData->m_Time ) );
				wstrstm << L"           " << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_FirstSlowPercent, pEnchantData->m_Time ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_SecondDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_171, "fi", fFinalDamage, pEnchantData->m_Time ) );
				wstrstm << L"           " << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_SecondSlowPercent, pEnchantData->m_Time ) );
				fFinalDamage = fOrgDamage * pEnchantData->m_ThirdDamagePercent / 100.0f;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_172, "fi", fFinalDamage, pEnchantData->m_Time ) );
				wstrstm << L"           " << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_ThirdSlowPercent, pEnchantData->m_Time ) );
#endif FIX_TOOLTIP
				tempDesc += wstrstm.str().c_str();
			}
			else
			{
#ifdef FIX_TOOLTIP
				wstrstm << GET_REPLACED_STRING( ( STR_ID_173, "iii", pEnchantData->m_Time, pEnchantData->m_FirstSlowPercent, pEnchantData->m_FirstDamagePercent ) );

				wstrstm << GET_REPLACED_STRING( ( STR_ID_174, "iii", pEnchantData->m_Time, pEnchantData->m_SecondSlowPercent, pEnchantData->m_SecondDamagePercent ) );

				wstrstm << GET_REPLACED_STRING( ( STR_ID_175, "iii", pEnchantData->m_Time, pEnchantData->m_ThirdSlowPercent, pEnchantData->m_ThirdDamagePercent ) );
#else
				wstrstm << GET_REPLACED_STRING( ( STR_ID_173, "ii", pEnchantData->m_FirstDamagePercent, pEnchantData->m_Time ) );
				wstrstm << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_FirstSlowPercent, pEnchantData->m_Time ) );
				
				wstrstm << GET_REPLACED_STRING( ( STR_ID_174, "ii", pEnchantData->m_SecondDamagePercent, pEnchantData->m_Time ) );
				wstrstm << L"           " << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_SecondSlowPercent, pEnchantData->m_Time ) );
				
				wstrstm << GET_REPLACED_STRING( ( STR_ID_175, "ii", pEnchantData->m_ThirdDamagePercent, pEnchantData->m_Time ) );
				wstrstm << L"           " << GET_REPLACED_STRING( ( STR_ID_170, "ii", pEnchantData->m_ThirdSlowPercent, pEnchantData->m_Time ) );
#endif FIX_TOOLTIP

				wstrstm << GET_STRING( STR_ID_163 );

				tempDesc += wstrstm.str().c_str();
			}
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( STR_ID_176, "f", pEnchantData->m_fRate * 100.0f ) );

			wstrstm << GET_STRING( STR_ID_163 );

			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
		{
			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( STR_ID_177 , "ff", pEnchantData->m_fRate * 100.0f, pEnchantData->m_fTimeStun ) );

			tempDesc += wstrstm.str().c_str();
		}
		break;

	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		{
			wstringstream wstrstm;

			if ( pItem != NULL )
			{
#ifdef PVP_SEASON2_SOCKET
				float fOrgDamage = 0.f;

				if ( true == bLevelLinkedStat )
					fOrgDamage = ( fAtkPhysic + fAtkMagic ) * ( g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel() + iAddLevel );
				else
                {
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
						// 스탯테이블을 통해 스탯 계산
						CX2Item::KItemFormatStatData kCalculateStat;
						g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
						const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
						const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
                }
#else
                {
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
						// 스탯테이블을 통해 스탯 계산
						CX2Item::KItemFormatStatData kCalculateStat;
						g_pData->GetItemStatCalculator().CalculateItemStat(kCalculateStat, pItem->GetItemTemplet());
						const CX2Item::KItemFormatStatData& stat = kCalculateStat;
	#else
						const CX2Item::KItemFormatStatData& stat = pItem->GetItemTemplet()->GetStatData();
	#endif //SERV_NEW_ITEM_SYSTEM_2013_05
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    const CX2Stat::Stat& stat = pItem->GetItemTemplet()->GetStat();
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
                    fOrgDamage = stat.m_fAtkPhysic + stat.m_fAtkMagic;
                }
#endif
				float fFinalDamage = fOrgDamage * pEnchantData->m_HPDrainPercent / 100.0f;

				wstrstm << GET_REPLACED_STRING( ( STR_ID_178, "fii", pEnchantData->m_fRate * 100.0f, (int)fFinalDamage, pEnchantData->m_MPDrain ) );
				
			}
			else
			{
				wstrstm << GET_REPLACED_STRING( ( STR_ID_179, "fii", pEnchantData->m_fRate * 100.0f, pEnchantData->m_HPDrainPercent, pEnchantData->m_MPDrain ) );
				wstrstm << GET_STRING( STR_ID_163 );
			}
		
			tempDesc += wstrstm.str().c_str();
		}
		break;


	default:
		{
			ASSERT( !"NO" );
		} break;
	}

	return tempDesc;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}



int	CX2EnchantItem::GetAttribEnchantType( bool bWeapon, int iCurrEnchantedCount )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 32707fc0 (emulated): weapon 0..3 -> SINGLE/DUAL/TRIPLE/QUADRA, armour 0..2 -> ARMOR/DUAL/TRIPLE_ARMOR
	if( true == bWeapon )
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	return CX2EnchantItem::AET_SINGLE_WEAPON;
		case 1:	return CX2EnchantItem::AET_DUAL_WEAPON;
		case 2:	return CX2EnchantItem::AET_TRIPLE_WEAPON;
		case 3:	return CX2EnchantItem::AET_QUADRA_WEAPON;
		}
	}
	else
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	return CX2EnchantItem::AET_ARMOR;
		case 1:	return CX2EnchantItem::AET_DUAL_ARMOR;
		case 2:	return CX2EnchantItem::AET_TRIPLE_ARMOR;
		}
	}
	return AET_NONE;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	if( true == bWeapon )
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	// 맨 처음 마법석을 설치하는 경우
			{
				return CX2EnchantItem::AET_SINGLE_WEAPON;
			} break;

		case 1: // 2번째 마법석을 설치하는 경우
			{
				return CX2EnchantItem::AET_DUAL_WEAPON;
			} break;

		case 2: // 3번째 마법석을 설치하는 경우
			{
				return CX2EnchantItem::AET_TRIPLE_WEAPON;
			} break;
		}
	}
	else
	{
		switch( iCurrEnchantedCount )
		{
		case 0:	// 맨 처음 마법석을 설치하는 경우
			{
				return CX2EnchantItem::AET_ARMOR;
			} break;

		case 1: // 2번째 마법석을 설치하는 경우
			{
				return CX2EnchantItem::AET_DUAL_ARMOR;
			} break;
		}
	}


	return AET_NONE;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

}

int	CX2EnchantItem::GetRequireEDFactor( int iItemGrade )
{
	switch( iItemGrade )
	{
	case CX2Item::IG_LOW:
	case CX2Item::IG_NORMAL:
		return 0;

	case CX2Item::IG_RARE:
		return 2;

	case CX2Item::IG_ELITE:
		return 4;

	case CX2Item::IG_UNIQUE:
		return 6;

	default:
		return 0;
	}
}


/*static*/ int CX2EnchantItem::DecomposeEnchantExtraDamage( CX2DamageManager::EXTRA_DAMAGE_TYPE eComposedEDT, 
										 CX2DamageManager::EXTRA_DAMAGE_TYPE& eDecomposedEDT1, CX2DamageManager::EXTRA_DAMAGE_TYPE& eDecomposedEDT2 )
{
	switch( eComposedEDT )
	{
	case CX2DamageManager::EDT_NONE:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_NONE;
			eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			return 0;
		} break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		{
			eDecomposedEDT1 = eComposedEDT;
			eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			return 1;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
		{				
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	// June buff port Phase 15: June 32708080's cases March lacks (4-slot types), then June's default.
	case CX2DamageManager::EDT_ENCHANT_QUADRA_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_PIERCING_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SHOCK_MASTER:
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SNATCH_MASTER:
		{
			eDecomposedEDT1 = eComposedEDT;
			eDecomposedEDT2 = CX2DamageManager::EDT_NONE;
			return 1;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_BLAZE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_BLAZE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_BLAZE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_DOUBLE_PIERCING:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_DOUBLE_SHOCK:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_PIERCING_BLAZE:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_PIERCING_FROZEN:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_PIERCING_POISON:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_DOUBLE_SNATCH:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SHOCK_BLAZE:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SHOCK_FROZEN:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SHOCK_POISON:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SNATCH_BLAZE:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_BLAZE;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SNATCH_FROZEN:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_FROZEN;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH;
			return 2;
		} break;

	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_SNATCH_POISON:
		{
			eDecomposedEDT1 = CX2DamageManager::EDT_ENCHANT_POISON;
			eDecomposedEDT2 = CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH;
			return 2;
		} break;

	default:
		{
			// June: an unknown value keeps the caller's eDecomposedEDT1 and returns 2 with the rest in
			// eDecomposedEDT2 (March asserted and returned 0). Kept verbatim, JUNE_BUGS.md.
			eDecomposedEDT2 = (CX2DamageManager::EXTRA_DAMAGE_TYPE)( (int)eComposedEDT - (int)eDecomposedEDT1 );
			return 2;
		} break;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	default:
		{
			ASSERT( !"NO" );
		} break;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	}

	return 0;
}







CX2DamageManager::EXTRA_DAMAGE_TYPE CX2EnchantItem::GetBaseExtraDamageType( CX2DamageManager::EXTRA_DAMAGE_TYPE eEDT )
{
	switch( eEDT )
	{
	case CX2DamageManager::EDT_NONE:
		{
			return CX2DamageManager::EDT_NONE;			
		} break;
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_BLAZE;
		} break;
	
	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_FROZEN;
		} break;
	
	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_POISON;
		} break;

	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
		{
			return CX2DamageManager::EDT_ENCHANT_PIERCING;
		} break;
	
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
		{
			return CX2DamageManager::EDT_ENCHANT_SHOCK;
		} break;
	
	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		{
			return CX2DamageManager::EDT_ENCHANT_SNATCH;
		} break;

#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	// June buff port Phase 15: June 327087e0's 4-slot cases (the rest equal March's).
	case CX2DamageManager::EDT_ENCHANT_QUADRA_BLAZE_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_BLAZE;
		} break;
	case CX2DamageManager::EDT_ENCHANT_QUADRA_FROZEN_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_FROZEN;
		} break;
	case CX2DamageManager::EDT_ENCHANT_QUADRA_POISON_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_POISON;
		} break;
	case CX2DamageManager::EDT_ENCHANT_QUADRA_PIERCING_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_PIERCING;
		} break;
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SHOCK_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_SHOCK;
		} break;
	case CX2DamageManager::EDT_ENCHANT_QUADRA_SNATCH_MASTER:
		{
			return CX2DamageManager::EDT_ENCHANT_SNATCH;
		} break;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	default:
		{
			ASSERT( !"NO" );
		} break;
	}

	return CX2DamageManager::EDT_NONE;

}


/*static*/ int CX2EnchantItem::GetEnchantResistValue( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 327088f0 (emulated): March's two groups, plus 135 for the double masters (a 3-attribute armour, AET_TRIPLE_ARMOR)
	switch( extraDamageType )
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:	case CX2DamageManager::EDT_ENCHANT_FROZEN:	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_PIERCING:	case CX2DamageManager::EDT_ENCHANT_SHOCK:	case CX2DamageManager::EDT_ENCHANT_SNATCH:
		return (int)CX2EnchantItem::EAR_VALUE;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
		return (int)CX2EnchantItem::EAR_MASTER_VALUE;
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		return 135;
	}
	return 0;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY

	switch( extraDamageType )
	{

	case CX2DamageManager::EDT_ENCHANT_BLAZE:						
	case CX2DamageManager::EDT_ENCHANT_FROZEN:						
	case CX2DamageManager::EDT_ENCHANT_POISON:						
	case CX2DamageManager::EDT_ENCHANT_PIERCING:					
	case CX2DamageManager::EDT_ENCHANT_SHOCK:						
	case CX2DamageManager::EDT_ENCHANT_SNATCH:	
		{
			return (int)CX2EnchantItem::EAR_VALUE;
		} break;


	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:				
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:				
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:				
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:				
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:				
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:				
		{
			return (int)CX2EnchantItem::EAR_MASTER_VALUE;
		} break;
	}

	return 0;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}

/*static*/ const wstring CX2EnchantItem::GetSlashTraceTextureName( CX2Unit::UNIT_TYPE eUnitTYpe, CX2DamageManager::EXTRA_DAMAGE_TYPE eExtraDamageType )
{
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	/// June 32708de0 (emulated): no unit-type switch; the 3/4-slot frozen and poison types pick ice/poison, everything else fire
	switch( eExtraDamageType )
	{
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:	case CX2DamageManager::EDT_ENCHANT_QUADRA_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_PIERCING:	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_SHOCK:	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_FROZEN_SNATCH:
		return L"Elsword_SlashTrace_Texture_Ice.dds";
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:	case CX2DamageManager::EDT_ENCHANT_QUADRA_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_PIERCING:	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_SHOCK:	case CX2DamageManager::EDT_ENCHANT_TRIPPLE_POISON_SNATCH:
		return L"Elsword_SlashTrace_Texture_Poision.dds";
	}
	return L"Elsword_SlashTrace_Texture_Fire.dds";
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	switch( eUnitTYpe )
	{
	case CX2Unit::UT_ELSWORD:
	case CX2Unit::UT_ARME:
	case CX2Unit::UT_LIRE:
	case CX2Unit::UT_RAVEN:
	case CX2Unit::UT_EVE:
	case CX2Unit::UT_CHUNG:	
	case CX2Unit::UT_ARA:
	case CX2Unit::UT_ELESIS:
	case CX2Unit::UT_ADD:
		{
			switch( eExtraDamageType )
			{
			case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
				{
					return L"Elsword_SlashTrace_Texture_Fire.dds";
				} break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
				{
					return L"Elsword_SlashTrace_Texture_Ice.dds";
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
				{
					return L"Elsword_SlashTrace_Texture_Poision.dds";
				} break;
			}
		} break;
	}

	ASSERT( "NOT GOOD" );
	return L"Elsword_SlashTrace_Texture_Fire.dds";
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
}