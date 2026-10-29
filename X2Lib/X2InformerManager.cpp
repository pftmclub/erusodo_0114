#include "StdAfx.h"
#include ".\x2informermanager.h"

CX2InformerManager::CX2InformerManager(void)
{
	CX2InformerSkill* pInformerSkill = new CX2InformerSkill();
	m_vecInformer.push_back( pInformerSkill );
	CX2InformerQuest* pInformerQuest = new CX2InformerQuest();
	m_vecInformer.push_back( pInformerQuest );
	CX2InformerInven* pInformerInven = new CX2InformerInven();
	m_vecInformer.push_back( pInformerInven );
	CX2InformerCashShop* pInformerCashShop = new CX2InformerCashShop();
	m_vecInformer.push_back( pInformerCashShop );

	//{{ kimhc // 2009-11-02 // 길드 추가
#ifdef	GUILD_MANAGEMENT
	CX2InformerGuildExp* pInformerGuildExp = new CX2InformerGuildExp();
	m_vecInformer.push_back( pInformerGuildExp );
#endif	GUILD_MANAGEMENT
	//}} kimhc // 2009-11-02 // 길드 추가
}

CX2InformerManager::~CX2InformerManager(void)
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		SAFE_DELETE( pInformer );
	}
	m_vecInformer.clear();
}

//{{ kimhc // 2009-11-02 // 길드 추가
#ifdef	GUILD_MANAGEMENT
CX2InformerGuildExp* CX2InformerManager::GetGuildInformer()
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	// June 328545c0: the created informer is returned
	if ( IsCreatedInformerType( CX2Informer::XIT_GUILD_EXP ) == true )
		return static_cast< CX2InformerGuildExp* >( GetInformerByType( CX2Informer::XIT_GUILD_EXP ) );

	CX2InformerGuildExp* pNewInformer = new CX2InformerGuildExp();
	m_vecInformer.push_back( pNewInformer );
	return pNewInformer;
#else SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	CX2InformerGuildExp* pInformer;
	if ( IsCreatedInformerType( CX2Informer::XIT_GUILD_EXP ) == true )
	{
		pInformer = static_cast< CX2InformerGuildExp* >( GetInformerByType( CX2Informer::XIT_GUILD_EXP ) );
	}
	else
	{
		CX2InformerGuildExp* pInformer = new CX2InformerGuildExp();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
#endif SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
}
#endif	GUILD_MANAGEMENT
//}} kimhc // 2009-11-02 // 길드 추가

CX2InformerSkill* CX2InformerManager::GetSkillInformer()
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	// June 328546a0: the created informer is returned
	if ( IsCreatedInformerType( CX2Informer::XIT_SKILL ) == true )
		return (CX2InformerSkill*)GetInformerByType( CX2Informer::XIT_SKILL );

	CX2InformerSkill* pNewInformer = new CX2InformerSkill();
	m_vecInformer.push_back( pNewInformer );
	return pNewInformer;
#else SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	CX2InformerSkill* pInformer;
	if ( IsCreatedInformerType( CX2Informer::XIT_SKILL ) == true )
	{
		pInformer = (CX2InformerSkill*)GetInformerByType( CX2Informer::XIT_SKILL );
	}
	else
	{
		CX2InformerSkill* pInformer = new CX2InformerSkill();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
#endif SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	
}

CX2InformerQuest* CX2InformerManager::GetQuestInformer()
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	// June 32854780: the created informer is returned
	if ( IsCreatedInformerType( CX2Informer::XIT_QUEST ) == true )
		return (CX2InformerQuest*)GetInformerByType( CX2Informer::XIT_QUEST );

	CX2InformerQuest* pNewInformer = new CX2InformerQuest();
	m_vecInformer.push_back( pNewInformer );
	return pNewInformer;
#else SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	CX2InformerQuest* pInformer;
	if ( IsCreatedInformerType( CX2Informer::XIT_QUEST ) == true )
	{
		pInformer = (CX2InformerQuest*)GetInformerByType( CX2Informer::XIT_QUEST );
	}
	else
	{
		CX2InformerQuest* pInformer = new CX2InformerQuest();
		m_vecInformer.push_back( pInformer );
		
	}

	return pInformer;
#endif SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
}

CX2InformerInven* CX2InformerManager::GetInvenInformer()
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	// June 32854860: the created informer is returned
	if ( IsCreatedInformerType( CX2Informer::XIT_INVEN ) == true )
		return (CX2InformerInven*)GetInformerByType( CX2Informer::XIT_INVEN );

	CX2InformerInven* pNewInformer = new CX2InformerInven();
	m_vecInformer.push_back( pNewInformer );
	return pNewInformer;
#else SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	CX2InformerInven* pInformer;
	if ( IsCreatedInformerType( CX2Informer::XIT_INVEN ) == true )
	{
		pInformer = (CX2InformerInven*)GetInformerByType( CX2Informer::XIT_INVEN );
	}
	else
	{
		CX2InformerInven* pInformer = new CX2InformerInven();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
#endif SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
}

CX2InformerCashShop* CX2InformerManager::GetCashShopInformer()
{
#ifdef SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	// June 32854940: the created informer is returned
	if ( IsCreatedInformerType( CX2Informer::XIT_CASH_SHOP ) == true )
		return (CX2InformerCashShop*)GetInformerByType( CX2Informer::XIT_CASH_SHOP );

	CX2InformerCashShop* pNewInformer = new CX2InformerCashShop();
	m_vecInformer.push_back( pNewInformer );
	return pNewInformer;
#else SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
	CX2InformerCashShop* pInformer;
	if ( IsCreatedInformerType( CX2Informer::XIT_CASH_SHOP ) == true )
	{
		pInformer = (CX2InformerCashShop*)GetInformerByType( CX2Informer::XIT_CASH_SHOP );
	}
	else
	{
		CX2InformerCashShop* pInformer = new CX2InformerCashShop();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
#endif SERV_IRUHADEV_JUNE_DIVERGE_INFORMER
}

void CX2InformerManager::Reset()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		pInformer->Reset();
	}
}

void CX2InformerManager::CheckInfo( bool bStateChange )
{
#if 0 
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		pInformer->CheckInfo( bStateChange );
	}
#endif
}

void CX2InformerManager::InvalidInfo()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
			pInformer->InvalidInfo();
	}
}

void CX2InformerManager::ClearAllInfo()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
			pInformer->ClearAllInfo();
	}
}


bool CX2InformerManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	for( int i = (int)m_vecInformer.size()-1; i >= 0; i-- )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
				return true;
		}
	}

	return false;
}

HRESULT CX2InformerManager::OnFrameMove( double fTime, float fElapsedTime )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			pInformer->OnFrameMove( fTime, fElapsedTime );
		}
	}

	return S_OK;
}

bool CX2InformerManager::IsCreatedInformerType( CX2Informer::X2_INFORMER_TYPE informerType )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->GetType() == informerType )
				return true;
		}
	}

	return false;
}

CX2Informer* CX2InformerManager::GetInformerByType( CX2Informer::X2_INFORMER_TYPE informerType )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->GetType() == informerType )
				return pInformer;
		}
	}

	return NULL;
}