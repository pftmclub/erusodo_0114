#include "StdAfx.h"
#include ".\x2talkboxmanagerui.h"

CX2TalkBoxManagerUI::CX2TalkBoxManagerUI(void)
{
	m_pTalkBoxManagerImp = NULL;

	m_pTalkBoxManagerImp = new CX2TalkBoxManagerImp();
}

CX2TalkBoxManagerUI::~CX2TalkBoxManagerUI(void)
{
	SAFE_DELETE( m_pTalkBoxManagerImp );
}

HRESULT CX2TalkBoxManagerUI::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->OnFrameMove( fTime, fElapsedTime );

	return S_OK;
}
#ifdef SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
// June 32eec4a0 hands the pass to the imp's vslot 0xc; the imp's pass is Phase R4h, so the imp still draws as March
HRESULT CX2TalkBoxManagerUI::OnFrameRender( int iRenderPass_ /*= 0*/ )
#else SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
HRESULT CX2TalkBoxManagerUI::OnFrameRender()
#endif SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
{
	KTDXPROFILE();

	if ( m_bShow == false )
		return S_OK;

	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->OnFrameRender();

	return S_OK;
}


void CX2TalkBoxManagerUI::Push( CX2TalkBoxManagerImp::TalkBox& talkBox )
{
	KTDXPROFILE();

	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->Push( talkBox );
}

bool CX2TalkBoxManagerUI::Delete( int unitUID, bool bTraceUnit, CX2GameUnit::GAME_UNIT_TYPE gameUnitType )
{
	if ( m_pTalkBoxManagerImp != NULL )
		return m_pTalkBoxManagerImp->Delete( unitUID, bTraceUnit, gameUnitType );

	return false;
}

void CX2TalkBoxManagerUI::SetPos( UidType unitUID, D3DXVECTOR2 pos, int boxType, bool bTraceUnit, CX2GameUnit::GAME_UNIT_TYPE gameUnitType )
{
	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->SetPos( unitUID, pos, boxType, bTraceUnit, gameUnitType );
}

void CX2TalkBoxManagerUI::SetPos( int talkBoxID, D3DXVECTOR2 pos, int boxType, bool bTraceUnit /* = false */ , CX2GameUnit::GAME_UNIT_TYPE gameUnitType /* = CX2GameUnit::GUT_USER */ )
{
	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->SetPos( talkBoxID, pos, boxType, bTraceUnit, gameUnitType );
}

int CX2TalkBoxManagerUI::GetMaxStrLen()
{
	if ( m_pTalkBoxManagerImp != NULL )
	{
		return m_pTalkBoxManagerImp->GetMaxStrLen();
	}

	return 0;
}

void CX2TalkBoxManagerUI::Clear()
{
	if ( m_pTalkBoxManagerImp != NULL )
		m_pTalkBoxManagerImp->Clear( );
}

