#include "StdAfx.h"
#include ".\ktdguidialogmanager.h"
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
#include "KTDGGFXManager.h"
#endif SERV_IRUHADEV_JUNE_GFX_GATES


#define MA_

static inline int _CompareLayer( CKTDGUIDialogType pkIn1_, CKTDGUIDialogType pkIn2_ )
{
    ASSERT( pkIn1_ != NULL && pkIn2_ != NULL );
    if ( !pkIn1_->GetFront() )
    {
        if ( pkIn2_->GetFront() )
            return -1;
    }
    else
    {
        if ( !pkIn2_->GetFront() )
            return 1;
    }//if.. else..

    if ( pkIn1_->GetLayer() < pkIn2_->GetLayer() )
        return -1;
    else if ( pkIn1_->GetLayer() > pkIn2_->GetLayer() )
        return 1;
    return 0;
}


CKTDGUIDialogManager::CKTDGUIDialogManager(void) 
: m_iModalDialogCount( 0 )
, m_iDialogListChangeCount( 0 )
{
	m_bShow = true;

#ifdef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
	m_mapFontToUKFont.clear();
#endif

#ifdef DIALOG_SHOW_TOGGLE
	m_bCanHide = false;
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
	m_iJuneHideMode = 0;			// 31d08180: the empty preset
	m_iJuneHidePrevMode = 0;
	_BuildJuneHidePresets();
#endif SERV_IRUHADEV_JUNE_GFX_GATES
#endif
#ifdef SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
	m_bJuneFlaggedDialogsOnly = false;	// June +0x110 ( ctor 31d067e0 @31d06a5b )
#endif SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
}

CKTDGUIDialogManager::~CKTDGUIDialogManager(void)
{
    ClearAllDlg();

#ifndef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
	m_mapFontList.clear();
#endif
	m_mapUKFontList.clear();

#ifdef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
	m_mapFontToUKFont.clear();
#endif

	RegisterLuaBind();

	CKTDGUIUniBuffer::Uninitialize();
	CKTDGUIIMEEditBox::Uninitialize();
}

void CKTDGUIDialogManager::RegisterLuaBind()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pDialogManager", this );
}


void CKTDGUIDialogManager::_InsertIntoDialogListTop( CKTDGUIDialogType pkInDialog_ )
{
// 외부에서 m_listDialog 를 iteration 하는 도중이라도 m_listDialog에 insert 하는 것은 안전하다.

    ASSERT( pkInDialog_ != NULL && pkInDialog_->GetUIDialogManager() == this );

    CKTDGUIDialogList::iterator iter = m_listDialog.begin();
    while( iter != m_listDialog.end() )
    {
        ASSERT( *iter != NULL );
        ASSERT( *iter != pkInDialog_ );
        if ( _CompareLayer( pkInDialog_, (*iter) ) < 0 )
        {
			break;
        }//if
        iter++;
    }//for


    m_listDialog.insert( iter, pkInDialog_ );
	m_setDialog.insert( pkInDialog_ );
	ASSERT( m_listDialog.size() == m_setDialog.size() );

	IncreaseDialogListChangeCount();
	pkInDialog_->SetDialogListChangeCount( m_iDialogListChangeCount );

	CountModalDialog( pkInDialog_->Get() );

}//CKTDGUIDialogManager::_InsertIntoDialogList()


void CKTDGUIDialogManager::_InsertIntoDialogListBottom( CKTDGUIDialogType pkInDialog_ )
{
// 외부에서 m_listDialog 를 iteration 하는 도중이라도 m_listDialog에 insert 하는 것은 안전하다.

    ASSERT( pkInDialog_ != NULL && pkInDialog_->GetUIDialogManager() == this );

    CKTDGUIDialogList::iterator iter = m_listDialog.begin();
    while( iter != m_listDialog.end() )
    {
        ASSERT( *iter != NULL );
        ASSERT( *iter != pkInDialog_ );
        if ( _CompareLayer( pkInDialog_, (*iter) ) <= 0 )
        {
			break;
        }//if
        iter++;
    }//for

    m_listDialog.insert( iter, pkInDialog_ );
	m_setDialog.insert( pkInDialog_ );
	ASSERT( m_listDialog.size() == m_setDialog.size() );

	IncreaseDialogListChangeCount();
	pkInDialog_->SetDialogListChangeCount( -m_iDialogListChangeCount );

	CountModalDialog( pkInDialog_->Get() );


}//CKTDGUIDialogManager::_InsertIntoDialogList()


bool CKTDGUIDialogManager::_RemoveFromDialogList( CKTDGUIDialogType pkInDialog_ )
{
// 외부에서 m_listDialog 를 iteration 하는 도중일 경우 m_vecIterator, m_vecReverseIterator 내의 iterator들을 적절하게 업데이트한다.

    ASSERT( pkInDialog_ != NULL && pkInDialog_->GetUIDialogManager() == this );

#ifdef _DEBUG // 2009-1-13
    CKTDGUIDialogList::reverse_iterator ritor = m_listDialog.rend();
    CKTDGUIDialogType  pritor = *ritor;
#endif // _DEBUG // 2009-1-13

    for( CKTDGUIDialogList::iterator iter = m_listDialog.begin();
        iter != m_listDialog.end(); iter++ )
    {
        ASSERT( *iter != NULL );
        if ( *iter == pkInDialog_ )
        {
            BOOST_TEST_FOREACH( CKTDGUIDialogList::iterator&, iterForward, m_vecIterator )
            {
                if ( iterForward != m_listDialog.end() && *iterForward == pkInDialog_ )
                    iterForward++;
            }//BOOST_TEST_FOREACH()

            BOOST_TEST_FOREACH( CKTDGUIDialogList::iterator&, iterReverse, m_vecReverseIterator )
            {
                if ( iterReverse != m_listDialog.end() && *iterReverse == pkInDialog_ )
                {
                    if ( iterReverse == m_listDialog.begin() )
                        iterReverse = m_listDialog.end();
                    else
                        iterReverse--;
                }//if
            }//BOOST_TEST_FOREACH()

            m_listDialog.erase( iter );
			m_setDialog.erase( pkInDialog_ );
			ASSERT( m_listDialog.size() == m_setDialog.size() );

			IncreaseDialogListChangeCount();
			pkInDialog_->SetDialogListChangeCount( 0 );

			CountModalDialog();
			
            return true;
        }//if
    }//for

    return false;
}//CKTDGUIDialogManager::_RemoveFromDialogList()


//{{ robobeg : 2008-01-07
bool CKTDGUIDialogManager::UnmanageDlg( CKTDGUIDialogType pDialog )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

    if ( !CheckDlg( pDialog ) )
        return false;

    bool bRemoved = _RemoveFromDialogList( pDialog );
    ASSERT( bRemoved );

    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetUIDialogManager( IN OUT pDialog, NULL );

    return true;
}//CKTDGUIDialogManager::UnmanageDlg()
//}} robobeg : 2008-01-07


bool CKTDGUIDialogManager::AddDlg( CKTDGUIDialogType pDialog )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

    if ( pDialog == NULL || pDialog->GetUIDialogManager() != NULL )
        return false;

    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetUIDialogManager( IN OUT pDialog, this );

//{{ robobeg : 2009-01-07
	// Setting Key ID
	//static int iDlgKeyID = 0;
	//iDlgKeyID++;

 //   ASSERT( m_mapDialogKey.find( iDlgKeyID ) == m_mapDialogKey.end() );
	//m_mapDialogKey[iDlgKeyID] = pDialog;
 //   CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetKeyID( *pDialog, iDlgKeyID );
//}} robobeg : 2009-01-07

    _InsertIntoDialogListTop( pDialog );

//#ifdef HEAVY_LOG_TEST 
//	string dialogName;
//	ConvertWCHARToChar( dialogName, pDialog->GetName() );
//	dialogName += " - AddDlg완료";
//	DialogLog( dialogName.c_str() );
//#endif HEAVY_LOG_TEST

	return true;
}


//{{ seojt // 2009-8-18, 18:11
//bool CKTDGUIDialogManager::AddDlg( CKTDGUIDialogHandle hDialog_ )
//{
//    if( hDialog_.IsValid() && CKTDGUIDialog::IsValidUID(hDialog_.GetHandle()) )
//    {
//        return AddDlg( CKTDGUIDialog::GetObjectByUID( hDialog_.GetHandle() ) );
//    }//if
//
//    return false;
//}//CKTDGUIDialogManager::AddDlg()
//}} seojt // 2009-8-18, 18:11


bool CKTDGUIDialogManager::DeleteDlg( CKTDGUIDialogType pDialog )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

    if ( !CheckDlg( pDialog ) )
        return false;


//#ifdef HEAVY_LOG_TEST
//	string dialogName;
//	ConvertWCHARToChar( dialogName, pDialog->GetName() );
//	dialogName += " - DeleteDlg완료";
//	DialogLog( dialogName.c_str() );
//#endif HEAVY_LOG_TEST



    // CKTDGUIDialog destructor에서 UnmanageDlg 를 호출하므로 아래 코드는 불필요하다.
    // 하지만 코드 읽을 때 이해를 돕기 위해... - robobeg
    UnmanageDlg( pDialog );

#ifdef DIALOG_HANDLE_TEST // 2009-8-19 by jintaeks
    CKTDGUIDialog::Destroy( pDialog.GetHandle() );
    pDialog.Invalidate();
#else
    SAFE_DELETE( pDialog );
#endif // DIALOG_HANDLE_TEST // 2009-8-19

    return true;
}


//{{ seojt // 2009-8-18, 18:02, qff
//bool CKTDGUIDialogManager::DeleteDlg(CKTDGUIDialogHandle hDialog_)
//{
//    if( hDialog_.IsValid() && CKTDGUIDialog::IsValidUID(hDialog_.GetHandle()) )
//    {
//        return DeleteDlg( CKTDGUIDialog::GetObjectByUID( hDialog_.GetHandle() ) );
//    }//if
//
//    return false;
//}//CKTDGUIDialogManager::DeleteDlg
//}} seojt // 2009-8-18, 18:02


void CKTDGUIDialogManager::ClearAllDlg()
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

    while( !m_listDialog.empty() )
    {
        CKTDGUIDialogType pkUIDialog = m_listDialog.front();

//      CKTDGUIDialog destructor에서 UnmanageDlg 를 호출하므로 아래 코드는 불필요하다.
//      하지만 코드 읽을 때 이해를 돕기 위해... - robobeg
        UnmanageDlg( pkUIDialog );

#ifdef DIALOG_HANDLE_TEST // 2009-8-19 by jintaeks
        CKTDGUIDialog::Destroy( pkUIDialog.GetHandle() );
        pkUIDialog.Invalidate();
#else
        SAFE_DELETE( pkUIDialog );
#endif // DIALOG_HANDLE_TEST // 2009-8-19
    }//

//{{ robobeg : 2009-01-07
    //m_mapDialogKey.clear();
//}} robobeg : 2009-01-07

#ifdef HEAVY_LOG_TEST
	string dialogName = "- ClearAllDlg 불림";
	DialogLog( dialogName.c_str() );
#endif HEAVY_LOG_TEST
}

bool CKTDGUIDialogManager::ClearAllOKAndCancelMsgDlg()
{
    unsigned uSize = m_vecIterator.size();

    m_vecIterator.push_back( m_listDialog.begin() );

    while( m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType  pTempDialog = *m_vecIterator.back();
        ASSERT( pTempDialog != NULL );
        m_vecIterator.back()++;

        if ( pTempDialog->GetOKAndCancelMsgBox() == true )
        {
#ifdef HEAVY_LOG_TEST
			string dialogName;
			ConvertWCHARToChar( dialogName, pTempDialog->GetName() );
			dialogName += " - DeleteDlg완료";
			DialogLog( dialogName.c_str() );
#endif HEAVY_LOG_TEST

//      CKTDGUIDialog destructor에서 UnmanageDlg 를 호출하므로 아래 코드는 불필요하다.
//      하지만 코드 읽을 때 이해를 돕기 위해... - robobeg
            UnmanageDlg( pTempDialog );

#ifdef DIALOG_HANDLE_TEST // 2009-8-19 by jintaeks
            CKTDGUIDialog::Destroy( pTempDialog.GetHandle() );
            pTempDialog.Invalidate();
#else
            SAFE_DELETE( pTempDialog );
#endif // DIALOG_HANDLE_TEST // 2009-8-19
        }//if
    }//for

    m_vecIterator.pop_back();

    ASSERT( uSize == m_vecIterator.size() );

	return true;
}


bool CKTDGUIDialogManager::CheckDlg( CKTDGUIDialogType pDialogToCheck )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.
	KTDXPROFILE();

    if ( pDialogToCheck == NULL )
        return false;	

	if( m_setDialog.find( pDialogToCheck ) == m_setDialog.end() )
		return false;

	
    //BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
    //{
    //    if ( pDialog == pDialogToCheck )
    //    {
    //        ASSERT( pDialogToCheck->GetUIDialogManager() == this );
    //        return true;
    //    }
    //}

    if ( pDialogToCheck->GetUIDialogManager() == NULL )
    {
        ASSERT( !"CKTDGUIDialogManager::CheckDlg() : pDialogToCheck is not managed by this manager." );
        return false;
    }

	ASSERT( pDialogToCheck->GetUIDialogManager() == this );
	return true;

    //ASSERT( !"CKTDGUIDialogManager::CheckDlg() : pDialogToCheck may be an invalid pointer." );
    //return false;


}





bool CKTDGUIDialogManager::CheckModalDlg()
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();
	
	if( m_iModalDialogCount > 0 )
		return true;

	return false;
}




bool CKTDGUIDialogManager::CheckFrontModalDlg( CKTDGUIDialogType pPivotDialog )
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    if ( !CheckDlg( pPivotDialog ) )
        return false;

	for( CKTDGUIDialogList::reverse_iterator iter = m_listDialog.rbegin(); 
		iter != m_listDialog.rend(); iter++ )
	{
		CKTDGUIDialogType pDialog = *iter;
		ASSERT( pDialog != NULL );

		if( pDialog == NULL )
			continue;

		if ( pDialog == pPivotDialog )
			return false;

		if ( pDialog->GetShow() == true && 
			pDialog->GetEnable() == true && 
			pDialog->GetModal() == false )
			return true;
	}//for

	return false;
}




CKTDGUIDialogType CKTDGUIDialogManager::GetFirstFrontModalDlg()
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    for( CKTDGUIDialogList::reverse_iterator iter = m_listDialog.rbegin();
        iter != m_listDialog.rend(); iter++ )
    {
		CKTDGUIDialogType pDialog = *iter;
        ASSERT( pDialog != NULL );
        if ( pDialog->GetShow() == true && pDialog->GetModal() == false )
            return pDialog;
    }//for

	return NULL;
}

void CKTDGUIDialogManager::CountModalDialog( CKTDGUIDialog* pDialog /*= NULL*/ )
{
	KTDXPROFILE();


	if( NULL == pDialog )
	{
		m_iModalDialogCount = 0;

		BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
		{
			ASSERT( pDialog != NULL );
			CKTDGUIDialog* pDialogRawPointer = pDialog->Get();
			if( false == pDialogRawPointer->GetModal() && 
				true == pDialogRawPointer->GetShow() &&
				false == pDialogRawPointer->GetHasSlot() ) // slot 가지고 있는지는 왜 체크 하지?? 일단 기존 코드대로 남겨둠, 나중에 확인하고 수정해야함
			{
				m_iModalDialogCount += 1;
			}
		}
	}
	else
	{
		if( false == pDialog->GetModal() && 
			true == pDialog->GetShow() &&
			false == pDialog->GetHasSlot() ) // slot 가지고 있는지는 왜 체크 하지?? 일단 기존 코드대로 남겨둠, 나중에 확인하고 수정해야함
		{
			m_iModalDialogCount += 1;
		}
	}
}


bool CKTDGUIDialogManager::CheckMostFrontDlg( CKTDGUIDialogType pDialog )
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    if ( !CheckDlg( pDialog ) )
        return false;

    ASSERT( !m_listDialog.empty() );

    if ( !m_listDialog.empty() )
    {
        if ( m_listDialog.back() == pDialog )
            return true;
        return false;
    }//if

	return false;
}

bool CKTDGUIDialogManager::ChangeLayer( CKTDGUIDialogType pDialog, int layerToChange )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.
	KTDXPROFILE();

    if ( !CheckDlg( pDialog ) )
        return false;

    if ( pDialog->GetLayer() == layerToChange )
        return true;

    _RemoveFromDialogList( pDialog );

    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetLayer( IN OUT pDialog, layerToChange );

    _InsertIntoDialogListTop( pDialog );

    return true;
}


bool CKTDGUIDialogManager::ChangeFront( CKTDGUIDialogType pDialog, bool bFront )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    if ( !CheckDlg( pDialog ) )
        return false;

    if ( pDialog->GetFront() == bFront )
        return true;

    _RemoveFromDialogList( pDialog );

    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetFront( IN OUT pDialog, bFront );

    _InsertIntoDialogListTop( pDialog );

    return true;
}//CKTDGUIDialogManager::ChangeFront()


bool CKTDGUIDialogManager::ChangeSequence( CKTDGUIDialogType pDialog, bool bTop )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();


    if ( !CheckDlg( pDialog ) )
        return false;



	// 매 프레임 이 함수를 호출하는 경우가 있어서 성능개선을 위해서 일단 이렇게 dialog list에 변화가 없는 경우에 dialog를 다시 뺐다가 넣는 작업을 피하도록 한다.
	if( abs( pDialog->GetDialogListChangeCount() ) == m_iDialogListChangeCount )
	{
		if( true == bTop ) 
		{
			if( pDialog->GetDialogListChangeCount() > 0 ) // dialog list에 추가될 때 _InsertIntoDialogListTop()으로 추가되었다
				return true;
		}
		else
		{
			if( pDialog->GetDialogListChangeCount() < 0 ) // dialog list에 추가될 때 _InsertIntoDialogListBottom()으로 추가되었다
				return true;
		}
	}



    bool bRet = _RemoveFromDialogList( pDialog );
    ASSERT( bRet );
    if ( bTop )
        _InsertIntoDialogListTop( pDialog );
    else
        _InsertIntoDialogListBottom( pDialog );

	return true;
}


//{{ seojt // 2009-8-18, 18:13
//bool CKTDGUIDialogManager::ChangeSequence(CKTDGUIDialogHandle hDialog_, bool bTop_)
//{
//    if( hDialog_.IsValid() && CKTDGUIDialog::IsValidUID(hDialog_.GetHandle()) )
//    {
//        return ChangeSequence( CKTDGUIDialog::GetObjectByUID( hDialog_.GetHandle() ), bTop_ );
//    }//if
//
//    return false;
//}//CKTDGUIDialogManager::ChangeSequence()
//}} seojt // 2009-8-18, 18:13


bool CKTDGUIDialogManager::ChangeSequence( CKTDGUIDialogType pDialogToChange, CKTDGUIDialogType pPivotDialog, bool bOver, bool bChangeLayer )
{
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

	if ( CheckDlg( pDialogToChange ) == false || CheckDlg( pPivotDialog ) == false || pDialogToChange == pPivotDialog )
		return false;

    if ( !bChangeLayer && ( pDialogToChange->GetFront() != pPivotDialog->GetFront()
        || pDialogToChange->GetLayer() != pPivotDialog->GetLayer() ) )
        return false;

    bool bRet = _RemoveFromDialogList( pDialogToChange );
    ASSERT( bRet );

    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetFront( IN OUT pDialogToChange, pPivotDialog->GetFront() );
    CKTDGUIDialog::CKTDGUIDialogManagerAccess::SetLayer( IN OUT pDialogToChange, pPivotDialog->GetLayer() );

    for( CKTDGUIDialogList::iterator iter = m_listDialog.begin(); iter != m_listDialog.end(); iter++ )
    {
        CKTDGUIDialogType pDialog = *iter;
        ASSERT( pDialog != NULL );
        if ( pDialog == pPivotDialog )
        {
			if( true == bOver )
				iter++;

			m_listDialog.insert( iter, pDialogToChange );
			m_setDialog.insert( pDialogToChange );
			IncreaseDialogListChangeCount();
			pDialogToChange->SetDialogListChangeCount( m_iDialogListChangeCount );

            return true;
        }//if
    }//if


    ASSERT( 0 );	
    return false;
}

CKTDGUIDialog* CKTDGUIDialogManager::GetDialog_LUA( const char* pName )
{
	KTDXPROFILE();


// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	if( NULL == pName || pName[0] == NULL )
		return NULL;


	wstring wstrName = L"";
	ConvertUtf8ToWCHAR( wstrName, pName );

	if( true == wstrName.empty() )
		return NULL;

	CKTDGUIDialogType& pDialog = GetDialog( wstrName.c_str() );
	if( pDialog != NULL )
		return pDialog->Get();

	return NULL;



//    BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
//    {
//        ASSERT( pDialog != NULL );
//		
//		if( 0 == wstrName.compare( pDialog->GetName() ) )
//        {
//#ifdef DIALOG_HANDLE_TEST // 2009-8-19 by jintaeks
//            return CKTDGUIDialog::GetObjectByUID( pDialog.GetHandle() );
//#else
//			return pDialog;
//#endif // DIALOG_HANDLE_TEST // 2009-8-19
//        }//if
//    }//BOOST_TEST_FOREACH()
//
//
//	return NULL;
}



CKTDGUIDialogType CKTDGUIDialogManager::GetDialog( const WCHAR* pName )
{
	KTDXPROFILE();

// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	if( NULL == pName )
		return NULL;

	if( m_mapDialogListChangeCountWhenLastGetDialog[ pName ] == m_iDialogListChangeCount )
	{
		CKTDGUIDialogMap::iterator it = m_mapDialog.find( pName );
		if( it != m_mapDialog.end() )
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}



    BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
    {
        ASSERT( pDialog != NULL );
		if ( wcscmp( pName, pDialog->GetName() ) == 0 )
		{
			m_mapDialog[ pName ] = pDialog;
			m_mapDialogListChangeCountWhenLastGetDialog[ pName ] = m_iDialogListChangeCount;
			return pDialog;
		}
    }


	m_mapDialogListChangeCountWhenLastGetDialog[ pName ] = m_iDialogListChangeCount;
	return NULL;
}

#ifdef PRIORITY_EVERY_UI
bool CKTDGUIDialogManager::ExistDialogAtMousePos()
{
	KTDXPROFILE();

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	CKTDGUIDialogSet::iterator sitDialog;

	for( sitDialog = m_setDialog.begin(); sitDialog != m_setDialog.end(); ++sitDialog )
//#else
//	CKTDGUIDialogList::iterator sitDialog;
//
//	for( sitDialog = m_listDialog.begin(); sitDialog != m_listDialog.end(); ++sitDialog )
//#endif
	{
		if( *sitDialog == NULL )
			continue;

		if( ( *sitDialog )->GetControlMouseOver() != NULL )
			return true;
	}

	return false;
}
#endif PRIORITY_EVERY_UI




HRESULT CKTDGUIDialogManager::OnFrameMove( double fTime, float fElapsedTime )
{

	KTDXPROFILE();

	KTDXPROFILE_BEGIN( "DIALOG_DELETE" );
    
        unsigned uSize = m_vecIterator.size();
        
        m_vecIterator.push_back( m_listDialog.begin() );

        while( m_vecIterator.back() != m_listDialog.end() )
        {
            CKTDGUIDialogType pDialog = *m_vecIterator.back();
            ASSERT( pDialog != NULL );
            m_vecIterator.back()++;

            if ( pDialog->GetIsAutoDeleteEnd() == true )
            {
			    string dialogName;
			    ConvertWCHARToChar( dialogName, pDialog->GetName() );
			    dialogName += " - DeleteDlg시작 by AutoDelete";
			    DialogLog( dialogName.c_str() );

//      CKTDGUIDialog destructor에서 UnmanageDlg 를 호출하므로 아래 코드는 불필요하다.
//      하지만 코드 읽을 때 이해를 돕기 위해... - robobeg
                UnmanageDlg( pDialog );

#ifdef DIALOG_HANDLE_TEST // 2009-8-19 by jintaeks
                CKTDGUIDialog::Destroy( pDialog.GetHandle() );
                pDialog.Invalidate();
#else
                SAFE_DELETE( pDialog );
#endif // DIALOG_HANDLE_TEST // 2009-8-19
            }//if
        }//for
    
        m_vecIterator.pop_back();    

        ASSERT( uSize == m_vecIterator.size() );
	
	KTDXPROFILE_END();

#ifdef DIALOG_SHOW_TOGGLE
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
	// June 31d06100 ( OnFrameMove ): m_bCanHide ( +0xbc ) && !m_bShow ( +0x34 ) -> the current hide mode's name list ( +0xd4 ): GetDialog by name
	// ( 31d05070 ), the dialog's OnFrameMove ( 31cd6ed0 ) when it exists, nothing else. Walks a copy: a dialog's frame move may change the mode.
	if( GetHideDialog() == true )
	{
		const std::vector< std::wstring > vecJuneHideNames( m_setJuneHideNames.begin(), m_setJuneHideNames.end() );
		for( size_t iJuneName = 0; iJuneName < vecJuneHideNames.size(); ++iJuneName )
		{
			CKTDGUIDialogType pDialog = GetDialog( vecJuneHideNames[iJuneName].c_str() );
			if( pDialog != NULL )
				pDialog->OnFrameMove( fTime, fElapsedTime );
		}

		return S_OK;
	}
#else SERV_IRUHADEV_JUNE_GFX_GATES
	if( GetHideDialog() == true )
	{
		CKTDGUIDialogType pDialog = GetDialog( L"DLG_ChatBox_EditBox" );
		if( pDialog != NULL )
			pDialog->OnFrameMove( fTime, fElapsedTime );

		pDialog = GetDialog( L"DLG_MsgBox" );
		if( pDialog != NULL )
			pDialog->OnFrameMove( fTime, fElapsedTime );

		pDialog = GetDialog( L"DLG_OKMsgBox" );
		if( pDialog != NULL )
			pDialog->OnFrameMove( fTime, fElapsedTime );

		pDialog = GetDialog( L"OkAndCancelMsgBox" );
		if( pDialog != NULL )
			pDialog->OnFrameMove( fTime, fElapsedTime );

		return S_OK;
	}
#endif SERV_IRUHADEV_JUNE_GFX_GATES
#endif //DIALOG_SHOW_TOGGLE

	KTDXPROFILE_BEGIN( "DIALOG_FRAME_MOVE" );
        
	//가장 뒤쪽에 있는 다이얼로그가 가장 상단에 위치한 다이얼로그 이므로 뒤쪽부터 처리한다.

        if ( !m_listDialog.empty() )
        {
            unsigned uSize2 = m_vecReverseIterator.size();

            m_vecReverseIterator.push_back( m_listDialog.end() );
            m_vecReverseIterator.back()--;

            while( m_vecReverseIterator.back() != m_listDialog.end() )
            {
                CKTDGUIDialogType pDialog = *m_vecReverseIterator.back();
                ASSERT( pDialog != NULL );
                if ( m_vecReverseIterator.back() == m_listDialog.begin() )
                    m_vecReverseIterator.back() = m_listDialog.end();
                else
                    m_vecReverseIterator.back()--;

                if ( pDialog->GetEnable() == true && pDialog->GetShow() == true )
				{
					string dialogName;
					ConvertWCHARToChar( dialogName, pDialog->GetName() );
					LastErrorLog( dialogName.c_str() );
#ifdef SERV_IRUHADEV_JUNE_RESULT_DIALOG_MODAL
					// June @31d06484: in the flagged-only mode only the flagged dialogs move
					if ( m_bJuneFlaggedDialogsOnly == false || pDialog->GetJuneDrawInFlaggedMode() == true )
						pDialog->OnFrameMove( fTime, fElapsedTime );
#else SERV_IRUHADEV_JUNE_RESULT_DIALOG_MODAL

                    pDialog->OnFrameMove( fTime, fElapsedTime );
#endif SERV_IRUHADEV_JUNE_RESULT_DIALOG_MODAL
				}
/*	
		    if( pDialog->GetModal() == false )
			    break;
*/
            }//while

            m_vecReverseIterator.pop_back();

            ASSERT( uSize2 == m_vecReverseIterator.size() );

        }//if

	KTDXPROFILE_END();

	return S_OK;
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
HRESULT CKTDGUIDialogManager::OnFrameRender( bool bFront, int iRenderPass_ )
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
HRESULT CKTDGUIDialogManager::OnFrameRender( bool bFront )
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
{
#ifdef SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
	// June 31d053f0 ( vslot 0x30 ): no layer band
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
	return _OnFrameRenderJune( bFront, false, 0, 0, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
	return _OnFrameRenderJune( bFront, false, 0, 0 );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
#else SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS


	KTDXPROFILE();

#ifdef DIALOG_SHOW_TOGGLE
	if( GetHideDialog() == true )
	{
		CKTDGUIDialogType pDialog = GetDialog( L"DLG_ChatBox_EditBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"DLG_MsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"DLG_OKMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"OkAndCancelMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		return S_OK;
	}
#endif //DIALOG_SHOW_TOGGLE

	if ( m_bShow == false )
		return S_OK;
	


    unsigned uSize = m_vecIterator.size();

    m_vecIterator.push_back( m_listDialog.begin() );

    while( m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecIterator.back();
        ASSERT( pDialog != NULL );
        m_vecIterator.back()++;
        if ( pDialog->GetFront() != bFront )
            continue;


        if ( pDialog->GetShow() )
            pDialog->OnFrameRender();
    }//while

#ifdef LOG_LOAD_DLG_SCRIPT
	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetPureKeyState(DIK_LCONTROL) == TRUE )
	{
		m_vecIterator.back() = m_listDialog.begin();

		int iNum = 0;

		while( m_vecIterator.back() != m_listDialog.end() )
		{
			CKTDGUIDialogType pDialog = *m_vecIterator.back();
			ASSERT( pDialog != NULL );
			m_vecIterator.back()++;
			if ( pDialog->GetFront() != bFront )
				continue;
			
			if ( pDialog->GetShow() && pDialog->GetShowScriptName() )
			{
				GetUKFont( 4 )->OutTextXY( 412, 100 + 100 * iNum, pDialog->GetScriptFileName().c_str(), D3DXCOLOR(1,0.6f,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
				GetUKFont( 4 )->OutTextXY( 412, 150 + 100 * iNum, pDialog->GetMouseOverStaticName().c_str(), D3DXCOLOR(1,0.6f,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );

				++iNum;
			}
		}//while
	}
#endif //LOG_LOAD_DLG_SCRIPT

    m_vecIterator.back() = m_listDialog.begin();

    while( m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecIterator.back();
        ASSERT( pDialog != NULL );
        m_vecIterator.back()++;
        if ( pDialog->GetFront() != bFront )
            continue;
        if ( pDialog->GetShow() && pDialog->GetGuideControl() != NULL )
		{
            pDialog->GetGuideControl()->OnFrameRender();
		}
    }//while

    m_vecIterator.pop_back();

    ASSERT( uSize == m_vecIterator.size() );

#ifdef NEW_SKILL_TREE_UI
	if(CKTDGUISlot::s_pDragSlot != NULL && bFront)
	{
		CKTDGUISlot::s_pDragSlot->RenderDragSlot();
	}
#endif

	return S_OK;
#endif SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
}

#ifdef SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
HRESULT CKTDGUIDialogManager::OnFrameRender( bool bFront, int iMinLayer_, int iMaxLayer_, int iRenderPass_ )
{
	return _OnFrameRenderJune( bFront, true, iMinLayer_, iMaxLayer_, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
HRESULT CKTDGUIDialogManager::OnFrameRender( bool bFront, int iMinLayer_, int iMaxLayer_ )
{
	return _OnFrameRenderJune( bFront, true, iMinLayer_, iMaxLayer_ );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
}

// June 31d053f0 / 31d05700 in render pass 0 ( the pass argument is R4's ). Differences from March's OnFrameRender above: the
// layer band, the flagged-only mode ( +0x110 / dialog +0x1e0 ), the dialog block pushed around the guide-control loop, no
// LOG_LOAD_DLG_SCRIPT overlay. Kept as June has it ( JUNE_BUGS.md ): the hidden-UI list, the guide controls and the drag slot
// ignore the band, so FrontUIRender's two band calls draw each of them twice.
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
// R4a: every dialog render gets ( false, pass ) ( June @31d054dc, @31d055bb ), the guide-control block is the DG +0x70 block in pass 1
// ( @31d055ee ), the drag slot gets the pass ( @31d056d1 )
HRESULT CKTDGUIDialogManager::_OnFrameRenderJune( bool bFront, bool bLayerBand_, int iMinLayer_, int iMaxLayer_, int iRenderPass_ )
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
HRESULT CKTDGUIDialogManager::_OnFrameRenderJune( bool bFront, bool bLayerBand_, int iMinLayer_, int iMaxLayer_ )
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
{
	KTDXPROFILE();

#ifdef DIALOG_SHOW_TOGGLE
	// June: m_bCanHide ( +0xbc ) && !m_bShow ( +0x34 ) -> only the current hide mode's dialog list ( +0xd4 ), then return. Mode 1
	// ( hide all ) is June's preset ( 32597410 @3259b25e-3259b318 ): March's four names, then DLG_Exit_Party_MsgBox. The show test
	// is June's dialog render's own ( 31cd7810 ). June's modes 2-5 are Effect C3p-2's routed rows.
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
	// G5: the current hide mode's name list ( 31d053f0 / 31d05700 hide path ) instead of mode 1's six literals ( the R8 review's list is preset 1 )
	if( GetHideDialog() == true )
	{
		const std::vector< std::wstring > vecJuneHideNames( m_setJuneHideNames.begin(), m_setJuneHideNames.end() );
		for( size_t iJuneName = 0; iJuneName < vecJuneHideNames.size(); ++iJuneName )
		{
			CKTDGUIDialogType pDialog = GetDialog( vecJuneHideNames[iJuneName].c_str() );
			if( pDialog != NULL && pDialog->GetShow() )
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
				pDialog->OnFrameRender( false, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
				pDialog->OnFrameRender();
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
		}

		return S_OK;
	}
#else SERV_IRUHADEV_JUNE_GFX_GATES
	if( GetHideDialog() == true )
	{
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		CKTDGUIDialogType pDialog = GetDialog( L"DLG_ChatBox_EditBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender( false, iRenderPass_ );

		pDialog = GetDialog( L"DLG_MsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender( false, iRenderPass_ );

		pDialog = GetDialog( L"DLG_OKMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender( false, iRenderPass_ );

		pDialog = GetDialog( L"OkAndCancelMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender( false, iRenderPass_ );
		// R8 review: June's fifth mode-1 name ( 334a9fb4; the June-only DLG_UI_ExitPartyMsgBox.lua, which March never opens )
		pDialog = GetDialog( L"DLG_Exit_Party_MsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender( false, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		CKTDGUIDialogType pDialog = GetDialog( L"DLG_ChatBox_EditBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"DLG_MsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"DLG_OKMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();

		pDialog = GetDialog( L"OkAndCancelMsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();
		// R8 review: June's fifth mode-1 name ( 334a9fb4; the June-only DLG_UI_ExitPartyMsgBox.lua, which March never opens )
		pDialog = GetDialog( L"DLG_Exit_Party_MsgBox" );
		if( pDialog != NULL && pDialog->GetShow() )
			pDialog->OnFrameRender();
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1

		return S_OK;
	}
#endif SERV_IRUHADEV_JUNE_GFX_GATES
#endif //DIALOG_SHOW_TOGGLE

	if ( m_bShow == false )
		return S_OK;

    unsigned uSize = m_vecIterator.size();

    m_vecIterator.push_back( m_listDialog.begin() );

    while( m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecIterator.back();
        ASSERT( pDialog != NULL );
        m_vecIterator.back()++;
        if ( pDialog->GetFront() != bFront )
            continue;

		if ( m_bJuneFlaggedDialogsOnly == true )
		{
			// June @31d0587b: flagged dialogs only, and no band
			if ( pDialog->GetJuneDrawInFlaggedMode() == true && pDialog->GetShow() )
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
				pDialog->OnFrameRender( false, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
				pDialog->OnFrameRender();
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
		}
		else if ( pDialog->GetShow()
			&& ( bLayerBand_ == false || ( iMinLayer_ <= pDialog->GetLayer() && pDialog->GetLayer() <= iMaxLayer_ ) ) )
		{
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
			pDialog->OnFrameRender( false, iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
			pDialog->OnFrameRender();
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
		}
    }//while

	// June @31d0591b-31d059d6: the guide controls of every shown dialog of this side, under the dialog block
    KD3DBEGIN()
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		// June @31d055ee: the DG +0x70 block in pass 1, else +0x68. The guide control's own pass argument ( vslot 8 ( pass ) @31d05695 ) is R4e's
		if ( 1 == iRenderPass_ )
			CKTDGStateManager::PushStates( g_pKTDXApp->GetDGManager()->GetRSICKTDGUIDialogAdditive() );
		else
			CKTDGStateManager::PushStates( g_pKTDXApp->GetDGManager()->GetRSICKTDGUIDialog() );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		CKTDGStateManager::PushStates( g_pKTDXApp->GetDGManager()->GetRSICKTDGUIDialog() );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1

		m_vecIterator.back() = m_listDialog.begin();

		while( m_vecIterator.back() != m_listDialog.end() )
		{
			CKTDGUIDialogType pDialog = *m_vecIterator.back();
			ASSERT( pDialog != NULL );
			m_vecIterator.back()++;
			if ( pDialog->GetFront() != bFront )
				continue;
			if ( pDialog->GetShow() && pDialog->GetGuideControl() != NULL )
			{
#ifdef SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
				pDialog->GetGuideControl()->OnFrameRender( iRenderPass_ );	// June @31d05695
#else SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
				pDialog->GetGuideControl()->OnFrameRender();
#endif SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
			}
		}//while
    KD3DEND()

    m_vecIterator.pop_back();

    ASSERT( uSize == m_vecIterator.size() );

#ifdef NEW_SKILL_TREE_UI
	if(CKTDGUISlot::s_pDragSlot != NULL && bFront)
	{
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		CKTDGUISlot::s_pDragSlot->RenderDragSlot( iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		CKTDGUISlot::s_pDragSlot->RenderDragSlot();
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
	}
#endif

	return S_OK;
}
#endif SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS

HRESULT CKTDGUIDialogManager::OnFrameRenderTopModal()
{

 #ifdef DIALOG_SHOW_TOGGLE
 	if( GetHideDialog() == true )
 		return S_OK;
 #else
 	if ( m_bShow == false )
 		return S_OK;
 #endif

// OnFrameRender 호출 후 바로 리턴하므로 단순 iteration 해도 안전

    for( CKTDGUIDialogList::reverse_iterator iter = m_listDialog.rbegin();
        iter != m_listDialog.rend(); iter++ )
    {
        CKTDGUIDialogType pDialog = *iter;
        ASSERT( pDialog != NULL );

		if( pDialog->GetEnable() && pDialog->GetShow() == true && pDialog->GetModal() == false )
		{
			pDialog->OnFrameRender();
            return S_OK;
		}//if
    }//for

	return S_OK;
}

HRESULT CKTDGUIDialogManager::OnResetDevice()
{

    unsigned uSize = m_vecIterator.size();
    
    m_vecIterator.push_back( m_listDialog.begin() );

    while(m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecIterator.back();
        m_vecIterator.back()++;
        ASSERT( pDialog != NULL );
		if ( pDialog != NULL )
			pDialog->OnResetDevice();
    }//for

    m_vecIterator.pop_back();

    ASSERT( uSize == m_vecIterator.size() );

	return S_OK;
}

HRESULT CKTDGUIDialogManager::OnLostDevice()
{

    unsigned uSize = m_vecIterator.size();

    m_vecIterator.push_back( m_listDialog.begin() );

    while( m_vecIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecIterator.back();
        m_vecIterator.back()++;
        ASSERT( pDialog != NULL );
		if ( pDialog != NULL )
			pDialog->OnLostDevice();
    }//for

    m_vecIterator.pop_back();

    ASSERT( uSize == m_vecIterator.size() );

	return S_OK;
}

bool CKTDGUIDialogManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KTDXPROFILE();

	//가장 뒤쪽에 있는 다이얼로그가 가장 상단에 위치한 다이얼로그 이므로 뒤쪽부터 처리한다.

    if ( m_listDialog.empty() )
        return false;

    unsigned uSize = m_vecReverseIterator.size();

    m_vecReverseIterator.push_back( m_listDialog.end() );
    m_vecReverseIterator.back()--;

    bool    bRet = false;

    while( m_vecReverseIterator.back() != m_listDialog.end() )
    {
        CKTDGUIDialogType pDialog = *m_vecReverseIterator.back();
        if ( m_vecReverseIterator.back() == m_listDialog.begin() )
            m_vecReverseIterator.back() = m_listDialog.end();
        else
            m_vecReverseIterator.back()--;
        ASSERT( pDialog != NULL );

		if( pDialog->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
        {
            bRet = true;
            break;
        }//if

		if( pDialog->GetEnable() == true && pDialog->GetShow() == true &&
			pDialog->GetModal() == false )
        {
            bRet = false;
            break;
        }//if
    }//for

    m_vecReverseIterator.pop_back();

    ASSERT( uSize == m_vecReverseIterator.size() );

#ifdef NEW_SKILL_TREE_UI
	if(CKTDGUISlot::s_pDragSlot != NULL)
	{
		switch( uMsg )
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
			{
				CKTDGUISlot::ReturnDraggingItem();
			} break;
		default:
			break;
		}		
	}
#endif

	return bRet;
}

#ifndef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
void CKTDGUIDialogManager::SetFont( int uiFontID, CKTDGFontManager::CKTDGFont* pFont )
{
	//m_FontList.push_back( pFont );

	if ( m_mapFontList.find( uiFontID ) != m_mapFontList.end() )
		return;

	m_mapFontList.insert( map< int, CKTDGFontManager::CKTDGFont* >::value_type( uiFontID, pFont ) );
}
#endif


void CKTDGUIDialogManager::SetUKFont( int uiUKFontID, CKTDGFontManager::CUKFont* pFont )
{ 
	//m_UKFontList.push_back( pFont ); 

	if ( m_mapUKFontList.find( uiUKFontID ) != m_mapUKFontList.end() )
		return;


	m_mapUKFontList.insert( map< int, CKTDGFontManager::CUKFont* >::value_type( uiUKFontID, pFont ) );
}



bool CKTDGUIDialogManager::IsDraggingDialog()
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
    {
        ASSERT( pDialog != NULL );
        if ( pDialog->GetShow() == true && pDialog->GetEnable() == true
            && pDialog->GetEnableMoveByDrag() == true && pDialog->GetMouseDownForDrag() == true )
            return true;
    }//BOOST_TEST_FOREACH()

	return false;
}

CKTDGUIDialogType CKTDGUIDialogManager::GetCloseDialogByESC()
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

	KTDXPROFILE();

    for( CKTDGUIDialogList::reverse_iterator iter = m_listDialog.rbegin();
        iter != m_listDialog.rend(); iter++ )
    {
		CKTDGUIDialogType pDialog = *iter;
        ASSERT( pDialog != NULL );
		if ( wcscmp( pDialog->GetName(), L"DLG_MsgBox" ) == 0 )
			continue;

		if( (pDialog->GetOKAndCancelMsgBox() == true ||	wcscmp( pDialog->GetName(), L"DLG_OKMsgBox" ) == 0) && pDialog->GetShow() == true )
		{
			g_pKTDXApp->GetDIManager()->SetEnable(true);

			return pDialog;
		}

		if ( pDialog->GetShow() == true && pDialog->GetEnable() == true && pDialog->GetCloseCustomUIEventID() != -1 )
		{
			return pDialog;
		}

		if ( pDialog->GetShow() == true && pDialog->GetEnable() == true && pDialog->GetModal() == false )
			return NULL;
    }//for

	return NULL;
}


void CKTDGUIDialogManager::SetStage( CKTDXStage* pStage )
{
// 여기서 호출하는 CKTDGUIDialog 함수들은 모두 안전
// 본 함수는 외부 iteration 중에 호출되어도 안전하다. 내부적으로도 숨겨진 iteration을 유발하지 않는다.

    BOOST_TEST_FOREACH( CKTDGUIDialogType, pDialog, m_listDialog )
    {
        ASSERT( pDialog != NULL );
        pDialog->SetStage( pStage );
    }//BOOST_TEST_FOREACH()

}

bool CKTDGUIDialogManager::GetFocusInEditBox()
{
	CKTDGUIControl* pControl = CKTDGUIDialog::GetControlFocus();
	if ( pControl != NULL )
	{
		if ( pControl->GetType() == CKTDGUIControl::UCT_IME_EDITBOX ||
			pControl->GetType() == CKTDGUIControl::UCT_EDITBOX )
		{
			return true;
		}
	}

	return false;
}

#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
// ---------------------------------------------------------------------------------------------------------------------------------------
// June GFx runtime G5 ( SERV_IRUHADEV_JUNE_GFX_GATES ): the hide-mode mechanism. June's manager keeps, at +0xc8, a "current mode" struct ( id, previous
// id, then three std::set<std::string> lists at +0xd0 / +0xe0 / +0xf0 ) and, at +0x100, a std::set of preset structs ordered by id. SetShow 31d05af0 copies
// the preset of the wanted mode into the current struct ( 31d05a20 looks it up, 31d08460 assigns it; an unknown mode copies the empty preset 31d08180,
// id 0 ). Only the first list ( +0xd0 ) has a reader ( OnFrameMove 31d06100, the render 31d053f0 / 31d05700 ). The StartUp loader 32597410 fills only that
// list; the other two stay empty, so 31f02f50's first loop ( GetDialog + DeleteDlg 31d05d30 per name of the second list, @31f02fb8-31f030bd ) never runs
// and is not ported. 31d05b80 is not a list reader: it is GetDialog( UTF-8 name ), March's GetDialog( WCHAR ).
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	struct KJuneHidePresetRow
	{
		int				m_iMode;
		const WCHAR*	m_wszName;
	};

	// Rows in the order of the loader's code: mode 1 @3259b25e, mode 4 @3259b351, mode 3 @3259b629 ( id from ebx = 3 ), mode 2 @3259b7ec, mode 5 @3259baa2
	// ( id from esi = 5 ). The names are the loader's wide literals ( 334a9fe0 ... ); June stores them narrow, in a sorted set.
	const KJuneHidePresetRow s_aJuneHidePresetRows[] =
	{
		{ 1, L"DLG_ChatBox_EditBox" }, { 1, L"DLG_MsgBox" }, { 1, L"DLG_OKMsgBox" }, { 1, L"OkAndCancelMsgBox" }, { 1, L"DLG_Exit_Party_MsgBox" },

		{ 4, L"DLG_ChatBox_EditBox" }, { 4, L"DLG_MsgBox" }, { 4, L"DLG_OKMsgBox" }, { 4, L"OkAndCancelMsgBox" },
		{ 4, L"DLG_GAME_SKILL_SLOT_NEW" }, { 4, L"DLG_Menu_Button" }, { 4, L"DLG_UI_MINI_MAP" },
		{ 4, L"DLG_UI_Quick_Slot" }, { 4, L"DLG_UI_Quick_Slot_Item_0" }, { 4, L"DLG_UI_Quick_Slot_Item_1" }, { 4, L"DLG_UI_Quick_Slot_Item_2" },
		{ 4, L"DLG_UI_Quick_Slot_Item_3" }, { 4, L"DLG_UI_Quick_Slot_Item_4" }, { 4, L"DLG_UI_Quick_Slot_Item_5" },
		{ 4, L"DLG_Party_Member_Result_State" }, { 4, L"DLG_Exit_Party_MsgBox" }, { 4, L"ChannelBox" }, { 4, L"mini_map_title" },
		{ 4, L"g_pComboBoxPresent_Select_User" },

		{ 3, L"DLG_MsgBox" }, { 3, L"DLG_OKMsgBox" }, { 3, L"OkAndCancelMsgBox" },
		{ 3, L"DLG_GAME_SKILL_SLOT_NEW" }, { 3, L"DLG_Menu_Button" }, { 3, L"DLG_UI_MINI_MAP" },
		{ 3, L"DLG_Party_Member_Result_State" }, { 3, L"DLG_Exit_Party_MsgBox" }, { 3, L"ChannelBox" }, { 3, L"mini_map_title" },
		{ 3, L"g_pComboBoxPresent_Select_User" },

		{ 2, L"DLG_ChatBox_EditBox" }, { 2, L"DLG_MsgBox" }, { 2, L"DLG_OKMsgBox" }, { 2, L"OkAndCancelMsgBox" },
		{ 2, L"DLG_UI_Quick_Slot" }, { 2, L"DLG_UI_Quick_Slot_Item_0" }, { 2, L"DLG_UI_Quick_Slot_Item_1" }, { 2, L"DLG_UI_Quick_Slot_Item_2" },
		{ 2, L"DLG_UI_Quick_Slot_Item_3" }, { 2, L"DLG_UI_Quick_Slot_Item_4" }, { 2, L"DLG_UI_Quick_Slot_Item_5" },
		{ 2, L"DLG_Party_Member_Result_State" }, { 2, L"DLG_Exit_Party_MsgBox" }, { 2, L"ChannelBox" }, { 2, L"mini_map_title" },
		{ 2, L"g_pComboBoxPresent_Select_User" }, { 2, L"DLG_Dungeon_Game_Start_Count" }, { 2, L"DLG_Dungeon_Game_Continue" },

		{ 5, L"DLG_SlideShotScripted_TalkBox" },
	};
}

void CKTDGUIDialogManager::_BuildJuneHidePresets()
{
	m_mapJuneHidePreset.clear();
	const int iRows = static_cast<int>( sizeof( s_aJuneHidePresetRows ) / sizeof( s_aJuneHidePresetRows[0] ) );
	for( int iRow = 0; iRow < iRows; ++iRow )
		m_mapJuneHidePreset[ s_aJuneHidePresetRows[iRow].m_iMode ].insert( std::wstring( s_aJuneHidePresetRows[iRow].m_wszName ) );
}

// 31d05af0: +0x34 = bShow; mode = bShow ? 0 : ( mode ? mode : 1 ); the current struct becomes the mode's preset ( previous id = the old current id ),
// the empty one ( id 0 ) when no preset has that id.
void CKTDGUIDialogManager::JuneSetShow( bool bShow, int iHideMode )
{
	m_bShow = bShow;

	int iMode = iHideMode;
	if( true == bShow )
		iMode = 0;
	else if( 0 == iMode )
		iMode = 1;

	m_iJuneHidePrevMode = m_iJuneHideMode;
	std::map< int, std::set< std::wstring > >::const_iterator itPreset = m_mapJuneHidePreset.find( iMode );
	if( itPreset != m_mapJuneHidePreset.end() )
	{
		m_iJuneHideMode = iMode;
		m_setJuneHideNames = itPreset->second;
	}
	else
	{
		m_iJuneHideMode = 0;
		m_setJuneHideNames.clear();
	}

#ifdef SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG
	{
		char szBuf[256] = { 0, };
		_snprintf( szBuf, sizeof( szBuf ) - 1, "[JUNE_GFX] GATE hide: SetShow( %d, %d ) -> mode %d ( was %d ), %d names", bShow ? 1 : 0, iHideMode,
			m_iJuneHideMode, m_iJuneHidePrevMode, static_cast<int>( m_setJuneHideNames.size() ) );
		KLOG( "ErrorLog.txt" ) << szBuf << fileout;
	}
#endif SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG
}

// 31f02f50, called by the DELETE-key handlers ( 31f036a0 = CX2State::ProcessGameScoreAndPostEffect, 32523dc0 = the PVP game state; 3249c9d0 is the June-only
// PVE state ). The mode now / before decide: now 2-4 -> SetShow( false, 0 ) ( hide all ), GFx shown = 0; else before 2-4 -> SetShow( false, before ), GFx
// shown = 1; else a plain toggle SetShow( !shown, 0 ), GFx shown = 1. GFx shown is the manager's +0x108 ( G0 map ), set only when the manager exists.
void CKTDGUIDialogManager::JuneToggleHide()
{
	const bool bOldShow = m_bShow;
	const int iNow = m_iJuneHideMode;
	const int iPrev = m_iJuneHidePrevMode;

	bool bGfxShow = true;
	if( true == IsJuneRestoreMode( iNow ) )
	{
		JuneSetShow( false, 0 );
		bGfxShow = false;
	}
	else if( true == IsJuneRestoreMode( iPrev ) )
	{
		JuneSetShow( false, iPrev );
	}
	else
	{
		JuneSetShow( !bOldShow, 0 );
	}

	if( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetJuneGFXManager() )
		g_pKTDXApp->GetJuneGFXManager()->SetShow( bGfxShow );
}
#endif SERV_IRUHADEV_JUNE_GFX_GATES

#ifdef DIALOG_SHOW_TOGGLE
bool CKTDGUIDialogManager::GetHideDialog()
{
	if( m_bCanHide == true && m_bShow == false )
		return true;

	return false;
}
#endif
