#include <StdAfx.h>


#ifdef  X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

/*static*/
const CX2Item::KItemFormatStatData  CX2Item::KItemFormatTemplet::ms_kDummyStatData;
/*static*/
const CX2Item::KItemFormatSpecialAbility    CX2Item::KItemFormatTemplet::ms_kDummySpecialAbility;
/*static*/
const D3DXVECTOR3     CX2Item::KItemFormatTemplet::ms_vZero(0,0,0);
/*static*/
const D3DXVECTOR3     CX2Item::KItemFormatTemplet::ms_vOne(1,1,1);

/*static*/
CX2Item::KItemFormatHeader    CX2Item::KItemFormatTemplet::ms_kDummyItemFormatHeader;
/*static*/
const BYTE*                    CX2Item::KItemFormatTemplet::ms_pData = (const BYTE*) &CX2Item::KItemFormatTemplet::ms_kDummyItemFormatHeader;

//#ifdef ADD_ITEM_TEMPLET_ITEM
/*static*/
const CX2Item::KItemFormatStatRelLVData  CX2Item::KItemFormatTemplet::ms_kDummyStatRelLVData;
//#endif //ADD_ITEM_TEMPLET_ITEM

void    CX2Item::KItemFormatTemplet::_ResetData()
{
    ms_pData = (const BYTE*) &ms_kDummyItemFormatHeader;
}


const wchar_t*      CX2Item::KItemFormatSpecialAbility::GetStringValue1() const
{
    if ( m_dwOffset_StringValue1 == 0 )
        return L"Group 0";
    return  CX2Item::KItemFormatTemplet::GetWideString( m_dwOffset_StringValue1 );
}


const wchar_t*      CX2Item::KItemFormatTemplet::GetNameColor_() const
{
	switch( GetItemGrade() )
	{
	case CX2Item::IG_UNIQUE:
		return  L"#CFFCC99";
	case CX2Item::IG_ELITE:
		return L"#CFF00FF";
	case CX2Item::IG_RARE:
		return L"#CFFFF00";
	case CX2Item::IG_NORMAL:
		return L"#CFFFFFF";
	case CX2Item::IG_LOW:
		return L"#C888888";
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
	case CX2Item::IG_LEGEND:
		return L"#CFF6900";		/// June item.kim K2: June 32867e80, jump table 32867ebc, grade 6
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
	}
	
	return L"#CFFFFFF";
}

/*static*/
const wchar_t*  CX2Item::KItemFormatTemplet::GetWideString( DWORD dwOffset_ )
{
    if ( dwOffset_ != 0 )
    {
        WORD   wLength = *( (const WORD*) ( ms_pData + dwOffset_ ) );
        if ( wLength != 0 )
        {
            return  (const wchar_t*) ( ms_pData + dwOffset_ + sizeof(WORD) );
        }
    }//if
    return  L"";
}

/*static*/
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::_GetD3DXVECTOR( DWORD dwOffset_, bool bDefaultOne )
{
    if ( dwOffset_ != 0 )
    {
        return *( (const D3DXVECTOR3*) ( ms_pData + dwOffset_ ) );
    }//if
    return  ( bDefaultOne == true ) ? ms_vOne : ms_vZero;
}


const wchar_t*  CX2Item::KItemFormatTemplet::GetName() const
{
    return  GetWideString( m_dwOffset_Name );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetDescription() const
{
    return  GetWideString( m_dwOffset_Description );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetDescriptionInShop() const
{
    return  GetWideString( m_dwOffset_DescriptionInShop );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetAttachFrameName( unsigned i ) const
{
    if ( i >= MAX_MODEL_COUNT_A_ITEM )
        return L"";
    return  GetWideString( m_adwOffset_AttachFrameName[ i ] );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetModelName( unsigned i ) const
{
    //ASSERT( m_pTemplet != NULL );
    if ( i >= MAX_MODEL_COUNT_A_ITEM )
        return L"";
    return  GetWideString( m_adwOffset_ModelName[ i ] );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetTextureChangeXETName() const
{
    return  GetWideString( m_dwOffset_TextureChangeXETName );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetAniXETName() const
{
    return  GetWideString( m_dwOffset_AniXETName );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetAniName() const
{
    return  GetWideString( m_dwOffset_AniName );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetShopImage() const
{
    return  GetWideString( m_dwOffset_ShopImage );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetDropViewer() const
{
    return  GetWideString( m_dwOffset_DropViewer );
}

const wchar_t*  CX2Item::KItemFormatTemplet::GetDescriptionInSkillNote() const
{
    return  GetWideString( m_dwOffset_DescriptionInSkillNote );
}

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
/// June item.kim K2: June 328671a0 (readers: the item period UI 3218aba0, the description builder 32e102f0; ported by Stage 16)
const wchar_t*  CX2Item::KItemFormatTemplet::GetExpiredDate() const
{
    return  GetWideString( m_dwOffset_ExpiredDate );
}

/*static*/ void CX2Item::KItemFormatTemplet::KimLine( const char* szFormat_, ... )
{
    char szLine[1024] = { 0, };
    va_list vaList;
    va_start( vaList, szFormat_ );
    _vsnprintf_s( szLine, _countof( szLine ), _TRUNCATE, szFormat_, vaList );
    va_end( vaList );

    KLOG( "ErrorLog.txt" ) << "[JUNE_KIM] " << szLine << fileout;     /// flushed per line, like CX2JuneBuffCapture::Line
}

/*static*/ void CX2Item::KItemFormatTemplet::KimGuard( const char* szKind_, const char* szFormat_, ... )
{
    static std::map< std::string, int > s_mapKimGuardCount;
    const int iCount = ++s_mapKimGuardCount[ std::string( NULL != szKind_ ? szKind_ : "" ) ];

    int iPower = 1;                     /// write hit 1, 10, 100, 1000, ...
    while ( iPower < iCount && iPower < 100000000 )
        iPower *= 10;
    if ( iPower != iCount )
        return;

    char szLine[768] = { 0, };
    va_list vaList;
    va_start( vaList, szFormat_ );
    _vsnprintf_s( szLine, _countof( szLine ), _TRUNCATE, szFormat_, vaList );
    va_end( vaList );

    KimLine( "guard %s hit %d: %s", NULL != szKind_ ? szKind_ : "", iCount, szLine );
}
#endif SERV_IRUHADEV_JUNE_ITEM_KIM

const CX2Item::KItemFormatStatData&  CX2Item::KItemFormatTemplet::GetStatData() const
{
    if ( m_dwOffset_Stat != 0 )
    {
        return  *( (const KItemFormatStatData*) ( ms_pData + m_dwOffset_Stat ) );
    }//if
    return  ms_kDummyStatData;
}

//#ifdef ADD_ITEM_TEMPLET_ITEM
const CX2Item::KItemFormatStatRelLVData&  CX2Item::KItemFormatTemplet::GetStatRelLVData() const
{
	if ( m_dwOffset_StatRelationLevel != 0 )
	{
		return  *( (const KItemFormatStatRelLVData*) ( ms_pData + m_dwOffset_StatRelationLevel ) );
	}//if
	return  ms_kDummyStatRelLVData;
}
//#endif // ADD_ITEM_TEMPLET_ITEM


unsigned  CX2Item::KItemFormatTemplet::GetNumBuffFactorPtr() const
{
    if ( m_dwOffset_BuffFactorIndices != 0 )
    {
        return *( (const WORD*) ( ms_pData + m_dwOffset_BuffFactorIndices ) );
    }
    return  0;
}

#ifndef X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL
CX2BuffFactorPtr CX2Item::KItemFormatTemplet::GetBuffFactorPtr( unsigned i ) const
{
    if ( m_dwOffset_BuffFactorIndices != 0 )
    {
        const WORD* pw = (const WORD*) ( ms_pData + m_dwOffset_BuffFactorIndices );
        if ( i < pw[0] )
        {
            return CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( pw[i+1] );
        }
    }
    return  CX2BuffFactorPtr();
}
#endif  X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL

unsigned  CX2Item::KItemFormatTemplet::GetNumSpecialAbility() const
{
    if ( m_dwOffset_SpecialAbilityList != 0 )
    {
        return *( (const DWORD*) ( ms_pData + m_dwOffset_SpecialAbilityList ) );
    }
    return  0;
}

const CX2Item::KItemFormatSpecialAbility&    CX2Item::KItemFormatTemplet::GetSpecialAbility( unsigned i ) const
{
    if ( m_dwOffset_SpecialAbilityList != 0 )
    {
        const BYTE* pbData = ms_pData + m_dwOffset_SpecialAbilityList;
        DWORD   dwNum = *( (const DWORD*) pbData );
        if ( i < dwNum )
        {
            return *( (const KItemFormatSpecialAbility*) ( pbData + sizeof(DWORD) + sizeof(KItemFormatSpecialAbility)*i ) );
        }
    }
    return ms_kDummySpecialAbility;
}

unsigned  CX2Item::KItemFormatTemplet::GetNumSocketOption() const
{
    if ( m_dwOffset_SocketOptions != 0 )
    {
        return *( (const DWORD*) ( ms_pData + m_dwOffset_SocketOptions ) );
    }
    return  0;
}

int CX2Item::KItemFormatTemplet::GetSocketOption( unsigned i ) const
{
    if ( m_dwOffset_SocketOptions != 0 )
    {
        const BYTE* pbData = ms_pData + m_dwOffset_SocketOptions;
        DWORD   dwNum = *( (const DWORD*) pbData );
        if ( i < dwNum )
        {
            return *( (const int*) ( pbData + sizeof(DWORD) + sizeof(int)*i ) );
        }
    }
    return 0;
}

void        CX2Item::KItemFormatTemplet::GetSocketOptions( std::vector<int>& vecOut_ ) const
{
    vecOut_.resize( 0 );
    if ( m_dwOffset_SocketOptions != 0 && ms_pData != NULL )
    {
        const BYTE* pbData = ms_pData + m_dwOffset_SocketOptions;
        DWORD   dwNum = *( (const DWORD*) pbData );
        if ( dwNum != 0 )
        {
            const int* pPoint = (const int*) ( pbData + sizeof(DWORD) );
            vecOut_.assign( &pPoint[0], &pPoint[dwNum] );
        }//if
    }//if
}

//#ifdef ADD_ITEM_TEMPLET_ITEM
unsigned  CX2Item::KItemFormatTemplet::GetNumRandomSocketOption() const
{
	if ( m_dwOffset_RandomSocketOptions != 0 )
	{
		return *( (const DWORD*) ( ms_pData + m_dwOffset_RandomSocketOptions ) );
	}
	return  0;
}

int CX2Item::KItemFormatTemplet::GetRandomSocketOption( unsigned i ) const
{
	if ( m_dwOffset_RandomSocketOptions != 0 )
	{
		const BYTE* pbData = ms_pData + m_dwOffset_RandomSocketOptions;
		DWORD   dwNum = *( (const DWORD*) pbData );
		if ( i < dwNum )
		{
			return *( (const int*) ( pbData + sizeof(DWORD) + sizeof(int)*i ) );
		}
	}
	return 0;
}

void CX2Item::KItemFormatTemplet::GetRandomSocketOptions( std::vector<int>& vecOut_ ) const
{
	vecOut_.resize( 0 );
	if ( m_dwOffset_RandomSocketOptions != 0 && ms_pData != NULL )
	{
		const BYTE* pbData = ms_pData + m_dwOffset_RandomSocketOptions;
		DWORD   dwNum = *( (const DWORD*) pbData );
		if ( dwNum != 0 )
		{
			const int* pPoint = (const int*) ( pbData + sizeof(DWORD) );
			vecOut_.assign( &pPoint[0], &pPoint[dwNum] );
		}//if
	}//if
}
//#endif // ADD_ITEM_TEMPLET_ITEM


unsigned    CX2Item::KItemFormatTemplet::GetNumAttachedParticleData() const
{
    if ( m_dwOffset_AttachedParticleData != 0 )
    {
        return *( (const DWORD*) ( ms_pData + m_dwOffset_AttachedParticleData ) );
    }
    return  0;
}

void        CX2Item::KItemFormatTemplet::GetAttachedParticleData( unsigned i, const wchar_t** ppwszOutParticleName, const wchar_t** ppwszOutBoneName ) const
{
    if ( ppwszOutParticleName != NULL )
        *ppwszOutParticleName = L"";
    if ( ppwszOutBoneName != NULL )
        *ppwszOutBoneName = L"";
    if ( m_dwOffset_AttachedParticleData != 0 && ms_pData != NULL )
    {
        const BYTE* pbData = ms_pData + m_dwOffset_AttachedParticleData;
        DWORD   dwNum = *( (const DWORD*) pbData );
        if ( i < dwNum )
        {
            const KItemFormatAttachedData* pData = (const KItemFormatAttachedData*)
                ( pbData + sizeof(DWORD) + sizeof(KItemFormatAttachedData)*i );
            if ( ppwszOutParticleName != NULL )
                *ppwszOutParticleName = GetWideString( pData->m_dwOffset_Name );
            if ( ppwszOutBoneName != NULL )
                *ppwszOutBoneName = GetWideString( pData->m_dwOffset_BoneName );
        }
    }
}

unsigned    CX2Item::KItemFormatTemplet::GetNumAttachedMeshData() const
{
    if ( m_dwOffset_AttachedMeshData != 0 && ms_pData != NULL )
    {
        return *( (const DWORD*) ( ms_pData + m_dwOffset_AttachedMeshData ) );
    }
    return  0;
}

void        CX2Item::KItemFormatTemplet::GetAttachedMeshData( unsigned i, const wchar_t** ppwszOutMeshName, const wchar_t** ppwszOutBoneName, bool* pbOutJiggle ) const
{
    if ( ppwszOutMeshName != NULL )
        *ppwszOutMeshName = L"";
    if ( ppwszOutBoneName != NULL )
        *ppwszOutBoneName = L"";
    if ( pbOutJiggle != NULL )
        *pbOutJiggle = false;
    if ( m_dwOffset_AttachedMeshData != 0  )
    {
        const BYTE* pbData = ms_pData + m_dwOffset_AttachedMeshData;
        DWORD   dwNum = *( (const DWORD*) pbData );
        if ( i < dwNum )
        {
            const KItemFormatAttachedData* pData = (const KItemFormatAttachedData*)
                ( pbData + sizeof(DWORD) + sizeof(KItemFormatAttachedData)*i );
            if ( ppwszOutMeshName != NULL )
                *ppwszOutMeshName = GetWideString( pData->m_dwOffset_Name & 0x7fffffff );
            if ( ppwszOutBoneName != NULL )
                *ppwszOutBoneName = GetWideString( pData->m_dwOffset_BoneName );
            if ( pbOutJiggle != NULL )
                *pbOutJiggle = ( pData->m_dwOffset_Name & 0x80000000 ) != 0;
        }
    }
}

#ifndef  X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
/// June item.kim K2: the key June's five Extra readers search for (32867660 and its four siblings, the same switch):
/// UNIT_TYPE 1-9 -> the same, 10 -> 10 (Lu) or 11 (Ciel, the bool), 11 -> 12 (Rose), 12 -> 13 (Ain), anything else -> 0.
/*static*/ DWORD CX2Item::KItemFormatTemplet::_GetExtraKey( CX2Unit::UNIT_TYPE eUnitType_, bool bIsMinorUnit_ )
{
    const DWORD dwUnitType = (DWORD) (BYTE) eUnitType_;     /// June passes the unit type as a byte
    if ( dwUnitType >= 1 && dwUnitType <= 9 )
        return dwUnitType;
    if ( 10 == dwUnitType )
        return bIsMinorUnit_ ? 11 : 10;
    if ( 11 == dwUnitType || 12 == dwUnitType )
        return dwUnitType + 1;
    return 0;
}

const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetFrameOffset( CX2Unit::UNIT_TYPE eUnitType, bool bIsMinorUnit_ ) const
#else
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetFrameOffset( CX2Unit::UNIT_TYPE eUnitType ) const
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
{
    unsigned    uNumFrameOffset = (unsigned) m_byNumFrameOffsets;
    if ( m_dwOffset_Extra != 0 && uNumFrameOffset != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra );
        for( unsigned u = 0; u != uNumFrameOffset; ++u )
        {
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
            if ( pData[u].m_dwType == _GetExtraKey( eUnitType, bIsMinorUnit_ ) )
#else
            if ( (CX2Unit::UNIT_TYPE) pData[u].m_dwType == eUnitType )
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
                return _GetD3DXVECTOR( pData[u].m_dwOffsetOrValue, false );
        }//for
    }//if
    return  ms_vZero;
}

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetRenderScale( CX2Unit::UNIT_TYPE eUnitType, bool bIsMinorUnit_ ) const
#else
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetRenderScale( CX2Unit::UNIT_TYPE eUnitType ) const
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
{
    unsigned    uNumScales = (unsigned) m_byNumRenderScales;
    if ( m_dwOffset_Extra != 0 && uNumScales != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = 
            (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra + sizeof(KItemFormatType_OffsetOrValue)*m_byNumFrameOffsets);
        for( unsigned u = 0; u != uNumScales; ++u )
        {
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
            if ( pData[u].m_dwType == _GetExtraKey( eUnitType, bIsMinorUnit_ ) )
#else
            if ( (CX2Unit::UNIT_TYPE) pData[u].m_dwType == eUnitType )
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
                return _GetD3DXVECTOR( pData[u].m_dwOffsetOrValue, true );
        }//for
    }//if
    return  ms_vOne;
}

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetRenderRotate( CX2Unit::UNIT_TYPE eUnitType, bool bIsMinorUnit_ ) const
#else
const D3DXVECTOR3&  CX2Item::KItemFormatTemplet::GetRenderRotate( CX2Unit::UNIT_TYPE eUnitType ) const
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
{
    unsigned    uNumRotates = (unsigned) m_byNumRenderRotates;
    if ( m_dwOffset_Extra != 0 && uNumRotates != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = 
            (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra 
            + sizeof(KItemFormatType_OffsetOrValue)*(m_byNumFrameOffsets+m_byNumRenderScales) );
        for( unsigned u = 0; u != uNumRotates; ++u )
        {
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
            if ( pData[u].m_dwType == _GetExtraKey( eUnitType, bIsMinorUnit_ ) )
#else
            if ( (CX2Unit::UNIT_TYPE) pData[u].m_dwType == eUnitType )
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
                return _GetD3DXVECTOR( pData[u].m_dwOffsetOrValue, false );
        }//for
    }//if
    return  ms_vZero;
}

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
const wchar_t*  CX2Item::KItemFormatTemplet::GetCommonItemModelName( CX2Unit::UNIT_TYPE eUnitType, bool bIsMinorUnit_ ) const
#else
const wchar_t*  CX2Item::KItemFormatTemplet::GetCommonItemModelName( CX2Unit::UNIT_TYPE eUnitType ) const
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
{
    unsigned    uNumModelNames = (unsigned) m_byNumCommonItemModelNames;
    if ( m_dwOffset_Extra != 0 && uNumModelNames != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = 
            (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra 
            + sizeof(KItemFormatType_OffsetOrValue)
                *(m_byNumFrameOffsets+m_byNumRenderScales+m_byNumRenderRotates) );
        for( unsigned u = 0; u != uNumModelNames; ++u )
        {
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
            if ( pData[u].m_dwType == _GetExtraKey( eUnitType, bIsMinorUnit_ ) )
#else
            if ( (CX2Unit::UNIT_TYPE) pData[u].m_dwType == eUnitType )
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
                return GetWideString( pData[u].m_dwOffsetOrValue );
        }//for
    }//if
    return  L"";
}

#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
const wchar_t*  CX2Item::KItemFormatTemplet::GetCommonItemXETName( CX2Unit::UNIT_TYPE eUnitType, bool bIsMinorUnit_ ) const
#else
const wchar_t*  CX2Item::KItemFormatTemplet::GetCommonItemXETName( CX2Unit::UNIT_TYPE eUnitType ) const
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
{
    unsigned    uNumXETNames = (unsigned) m_byNumCommonItemXETNames;
    if ( m_dwOffset_Extra != 0 && uNumXETNames != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = 
            (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra 
            + sizeof(KItemFormatType_OffsetOrValue)
                *(m_byNumFrameOffsets+m_byNumRenderScales+m_byNumRenderRotates+m_byNumCommonItemModelNames) );
        for( unsigned u = 0; u != uNumXETNames; ++u )
        {
#ifdef SERV_IRUHADEV_JUNE_ITEM_KIM
            if ( pData[u].m_dwType == _GetExtraKey( eUnitType, bIsMinorUnit_ ) )
#else
            if ( (CX2Unit::UNIT_TYPE) pData[u].m_dwType == eUnitType )
#endif SERV_IRUHADEV_JUNE_ITEM_KIM
                return GetWideString( pData[u].m_dwOffsetOrValue );
        }//for
    }//if
    return  L"";
}

#endif  X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL

#ifndef X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL
void            CX2Item::KItemFormatTemplet::GetSlashTraceColors( D3DXCOLOR& dwOut_, D3DXCOLOR& dwOutTip_, D3DXCOLOR& dwOutHyper_, D3DXCOLOR& dwOutHyperTip_, CX2Unit::UNIT_TYPE eUnitType_ /*= false  */) const
#else
void            CX2Item::KItemFormatTemplet::GetSlashTraceColors( D3DXCOLOR& dwOut_, D3DXCOLOR& dwOutTip_, D3DXCOLOR& dwOutHyper_, D3DXCOLOR& dwOutHyperTip_) const
#endif //X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL
{
#ifndef X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL
	switch( eUnitType_ )
	{
	case CX2Unit::UT_ARA:
		{
			dwOut_		= 0xffcc9933;
			dwOutTip_	= 0xffcc9933;
		} break;
	case CX2Unit::UT_ELESIS:
		{
			dwOut_		= 0xfffa7819;
			dwOutTip_	= 0xfffa7819;
		} break;
	default:
		{
			dwOut_ = 0xcc5555ff;
			dwOutTip_ = 0xcc5555ff;
		} break;
	}
#else
	dwOut_ = 0xcc5555ff;
	dwOutTip_ = 0xcc5555ff;
#endif //X2OPTIMIZE_SCRIPT_PREPROCESSING_TOOL

	dwOutHyper_ = 0xccff3333;
	dwOutHyperTip_ = 0xffff3333;

    //ASSERT( m_pTemplet != NULL );
    ASSERT( ms_pData != NULL );
    unsigned    uNumSlashTraces = (unsigned) m_byNumSlashTraces;
    if ( m_dwOffset_Extra != 0 && uNumSlashTraces != 0 )
    {
        const KItemFormatType_OffsetOrValue* pData = 
            (const KItemFormatType_OffsetOrValue*)( ms_pData + m_dwOffset_Extra 
            + sizeof(KItemFormatType_OffsetOrValue)
                *(m_byNumFrameOffsets+m_byNumRenderScales+m_byNumRenderRotates
                +m_byNumCommonItemModelNames+m_byNumCommonItemXETNames) );
        for( unsigned u = 0; u != uNumSlashTraces; ++u )
        {
            switch( pData[u].m_dwType )
            {
            case STT_SLASH_TRACE:
                dwOut_ = pData[u].m_dwOffsetOrValue;
                break;
            case STT_SLASH_TRACE_TIP:
                dwOutTip_ = pData[u].m_dwOffsetOrValue;
                break;
            case STT_HYPER_SLASH_TRACE:
                dwOutHyper_ = pData[u].m_dwOffsetOrValue;
                break;
            case STT_HYPER_SLASH_TRACE_TIP:
                dwOutHyperTip_ = pData[u].m_dwOffsetOrValue;
                break;
            }//switch
        }//for
    }//if
}


const wchar_t*  CX2Item::KItemFormatSetItemData::GetSetName() const
{
    if ( m_dwOffset_SetName == 0 )
        return L"";
    return  CX2Item::KItemFormatTemplet::GetWideString( m_dwOffset_SetName );
}


void    CX2Item::KItemFormatSetItemData::GetSetItemList( vector< DWORD >& vecItemID ) const
{
    vecItemID.resize( 0 );

    if ( m_dwOffset_ItemIDs == 0 )
        return;

    const DWORD*    pDWord = (const DWORD*) ( CX2Item::KItemFormatTemplet::GetData() + m_dwOffset_ItemIDs );
    DWORD   dwNum = *pDWord;
    if ( dwNum == 0 )
        return;
    pDWord++;
    vecItemID.assign( &pDWord[0], &pDWord[dwNum] );
}


void    CX2Item::KItemFormatSetItemData::GetSetItemOptions( const int iNumOfEquippedItems_, IN vector< int >& vecOptions ) const
{
    if ( m_dwOffset_NeedPartsNumNOptions == 0 )
        return;
    DWORD   dwNum = *( (const DWORD*) ( KItemFormatTemplet::GetData() + m_dwOffset_NeedPartsNumNOptions ) );
    if ( dwNum == 0 )
        return;
    const KItemForamtNeedPartsNumAndOption* pNPNO = (const KItemForamtNeedPartsNumAndOption*) ( 
        KItemFormatTemplet::GetData() + m_dwOffset_NeedPartsNumNOptions + sizeof(DWORD) );

    DWORD   dwNumOfEquippedItems = (DWORD) iNumOfEquippedItems_;

    const CX2Item::KItemForamtNeedPartsNumAndOption* pFoundEnd = std::upper_bound( &pNPNO[0], &pNPNO[dwNum], dwNumOfEquippedItems );
    if ( pFoundEnd == &pNPNO[0] )
        return;
    dwNumOfEquippedItems = ( pFoundEnd - 1 )->m_dwNeedPartsNum;
    const KItemForamtNeedPartsNumAndOption* pFound = std::lower_bound( &pNPNO[0], pFoundEnd, dwNumOfEquippedItems );
    if ( pFound >= pFoundEnd )
        return;
    DWORD dwNumOptions = pFoundEnd - pFound;
    vecOptions.reserve( vecOptions.size() + dwNumOptions );
    for( ; pFound != pFoundEnd; ++pFound )
    {
        vecOptions.push_back( pFound->m_iOption );
    }
}


void    CX2Item::KItemFormatSetItemData::GetSetItemNeedPartsNumNOptions( const KItemForamtNeedPartsNumAndOption*& pBegin, 
        const KItemForamtNeedPartsNumAndOption*& pEnd ) const
{
    pBegin = NULL;
    pEnd = NULL;

    if ( m_dwOffset_NeedPartsNumNOptions == 0 )
        return;
    DWORD   dwNum = *( (const DWORD*) ( KItemFormatTemplet::GetData() + m_dwOffset_NeedPartsNumNOptions ) );
    if ( dwNum == 0 )
        return;
    const KItemForamtNeedPartsNumAndOption* pNPNO = (const CX2Item::KItemForamtNeedPartsNumAndOption*) ( 
        CX2Item::KItemFormatTemplet::GetData() + m_dwOffset_NeedPartsNumNOptions + sizeof(DWORD) );

    pBegin = &pNPNO[0];
    pEnd = &pNPNO[dwNum];
}


#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING