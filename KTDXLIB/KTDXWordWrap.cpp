#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_UI_LINE_BREAK
#include "KTDXWordWrap.h"

// June2017 31d42a60: IsSpaceLatter. Same set as March CWordLineHandler::IsSpaceLatter (June string 3344bee8 = " \t").
bool CKTDXWordWrap::IsSpaceLatter( WCHAR wcChar )
{
	if ( wcChar == L'\0' )
		return false;

	static const WCHAR s_wszSpaceLatters[] = L" \t";
	return wcschr( s_wszSpaceLatters, wcChar ) != NULL;
}

// June2017 31d42b10: IsSplitLatter (June string 3344bef0, 30 characters, identical to March's list).
bool CKTDXWordWrap::IsSplitLatter( WCHAR wcChar )
{
	if ( wcChar == L'\0' )
		return false;

	static const WCHAR s_wszSplitLatters[] = L"~!@#$%^&*_+{}|:\"<>?`-=[]\\;',./";
	return wcschr( s_wszSplitLatters, wcChar ) != NULL;
}

// June2017 31cbff90 (the plain wrap; 31d471f0 is its thin wrapper). Word-aware: the break goes after the last space / split
// character, and a word wider than the line is cut with "-\n". A "#C" color token that starts exactly at the cut point is
// skipped one character further than elsewhere (the +3 / +8 versus +2 / +7 below), which is June's behavior, not March's.
std::wstring CKTDXWordWrap::GetStrByLineBreak( const WCHAR* pOrgStr, int width, int fontIndex, CKTDGFontManager::CUKFont* pFont )
{
	if ( pOrgStr == NULL )
		return L"";

	std::wstring tempText = pOrgStr;
	const int constTextMaxLen = (int)( g_pKTDXApp->GetResolutionScaleX() * width );

	int nowTextLen = 0;
	int iLastCutPoint = 0;
	int iLastCharWidth = 0;
	int iNowLineWidthFromLastCurPoint = 0;

	if ( pFont == NULL )
		pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( fontIndex );

	if ( pFont != NULL )
	{
		for ( int i = 0; i < (int)tempText.length(); i++ )
		{
			WCHAR tmpWChar = tempText[i];

			WCHAR tmpNextWChar = L'\0';
			if ( ( i + 1 ) < (int)tempText.length() )
				tmpNextWChar = tempText[i+1];

			if ( tmpWChar == L'\n' )
			{
				nowTextLen = 0;
				iNowLineWidthFromLastCurPoint = 0;
				iLastCutPoint = i;
				continue;
			}

			if ( tmpWChar == L'#' && i + 1 < (int)tempText.size() && ( tempText[i+1] == L'C' || tempText[i+1] == L'c' ) )
			{
				if ( i + 2 < (int)tempText.size() )
				{
					if ( tempText[i+2] == L'x' || tempText[i+2] == L'X' )
					{
						if ( iLastCutPoint == i )
							i += 3;
						else
							i += 2;
					}
					else
					{
						if ( iLastCutPoint == i )
							i += 8;
						else
							i += 7;
					}
					continue;
				}
			}

			int iUniCharSize = pFont->GetWidth( tmpWChar );
			nowTextLen += iUniCharSize;

			if ( IsSpaceLatter( tmpWChar ) )
			{
				iLastCutPoint = i + 1;
				iNowLineWidthFromLastCurPoint = 0;
			}
			else if ( IsSplitLatter( tmpWChar ) && ( IsSpaceLatter( tmpNextWChar ) || IsSplitLatter( tmpNextWChar ) ) )
			{
				iLastCutPoint = i + 1;
				iNowLineWidthFromLastCurPoint = 0;
			}
			else
			{
				iLastCharWidth = iUniCharSize;
				iNowLineWidthFromLastCurPoint += iUniCharSize;
			}

			if ( nowTextLen >= constTextMaxLen )
			{
				if ( iNowLineWidthFromLastCurPoint < nowTextLen )
				{
					std::wstring wsTemp = L"\n";

					if ( (int)tempText.size() >= iLastCutPoint )
						tempText.insert( tempText.begin() + iLastCutPoint, wsTemp.begin(), wsTemp.end() );

					i += 1;
					nowTextLen = iNowLineWidthFromLastCurPoint;
				}
				else
				{
					std::wstring wsTemp = L"-\n";

					if ( i != 0 )
						tempText.insert( tempText.begin() + ( i - 1 ), wsTemp.begin(), wsTemp.end() );

					i += 2;
					nowTextLen = iLastCharWidth;
				}
				iNowLineWidthFromLastCurPoint = nowTextLen;
			}

			if ( i >= (int)tempText.length() )
				break;
		}
	}

#ifdef SERV_IRUHADEV_JUNE_UI_DEBUG
	static int s_iLogPlain = 0;
	if ( ++s_iLogPlain <= 6 )
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] UI WRAP plain width " << width << " font " << fontIndex << " chars " << (int)lstrlenW( pOrgStr ) << " -> " << (int)tempText.size() << fileout;
#endif SERV_IRUHADEV_JUNE_UI_DEBUG
	return tempText;
}

// June2017 31d43ef0: the wrap with the active "#C" color carried over each inserted break (March GetStrByLineBreakColorInX2Main,
// whose CLIENT_GLOBAL_EU_WORDWRAP branch is what June compiles unconditionally).
std::wstring CKTDXWordWrap::GetStrByLineBreakColor( const WCHAR* pOrgStr, int width, int fontIndex )
{
	if ( pOrgStr == NULL )
		return L"";

	std::wstring tempText = pOrgStr;
	const int constTextMaxLen = (int)( g_pKTDXApp->GetResolutionScaleX() * width );

	int nowTextLen = 0;
	int iLastCutPoint = 0;
	int iLastCharWidth = 0;
	int iNowLineWidthFromLastCurPoint = 0;

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( fontIndex );
	if ( pFont != NULL )
	{
		std::wstring wstrPrevColorkey = L"";
		for ( int i = 0; i < (int)tempText.size(); i++ )
		{
			WCHAR tmpWChar = tempText[i];

			WCHAR tmpNextWChar = L'\0';
			if ( ( i + 1 ) < (int)tempText.length() )
				tmpNextWChar = tempText[i+1];

			if ( tmpWChar == L'\n' )
			{
				nowTextLen = 0;
				tempText.insert( tempText.begin() + i + 1, wstrPrevColorkey.begin(), wstrPrevColorkey.end() );
				wstrPrevColorkey = L"";
				iLastCutPoint = i;
				iNowLineWidthFromLastCurPoint = 0;
				continue;
			}

			if ( tempText[i] == L'#' && i + 1 < (int)tempText.size() && ( tempText[i+1] == L'C' || tempText[i+1] == L'c' ) )
			{
				if ( i + 2 < (int)tempText.size() )
				{
					if ( tempText[i+2] == L'x' || tempText[i+2] == L'X' )
					{
						wstrPrevColorkey = L"";
						i += 2;
					}
					else
					{
						wstrPrevColorkey = tempText.substr( i, 8 );
						i += 7;
					}
					continue;
				}
			}

			int iUniCharSize = pFont->GetWidth( tmpWChar );
			nowTextLen += iUniCharSize;

			if ( IsSpaceLatter( tmpWChar ) )
			{
				iLastCutPoint = i + 1;
				iNowLineWidthFromLastCurPoint = 0;
			}
			else if ( IsSplitLatter( tmpWChar ) && ( IsSpaceLatter( tmpNextWChar ) || IsSplitLatter( tmpNextWChar ) ) )
			{
				iLastCutPoint = i + 1;
				iNowLineWidthFromLastCurPoint = 0;
			}
			else
			{
				iLastCharWidth = iUniCharSize;
				iNowLineWidthFromLastCurPoint += iUniCharSize;
			}

			if ( nowTextLen >= constTextMaxLen )
			{
				if ( nowTextLen <= iNowLineWidthFromLastCurPoint )
				{
					std::wstring wsTemp = L"-\n";
					wsTemp += wstrPrevColorkey;

					if ( i != 0 )
						tempText.insert( tempText.begin() + ( i - 1 ), wsTemp.begin(), wsTemp.end() );

					i += (int)( 2 + wstrPrevColorkey.length() );

					nowTextLen = iLastCharWidth;
					iNowLineWidthFromLastCurPoint = iLastCharWidth;
				}
				else
				{
					std::wstring wsTemp = L"\n";
					wsTemp += wstrPrevColorkey;

					if ( (int)tempText.size() >= iLastCutPoint )
						tempText.insert( tempText.begin() + iLastCutPoint, wsTemp.begin(), wsTemp.end() );

					i += (int)( 1 + wstrPrevColorkey.length() );

					nowTextLen = iNowLineWidthFromLastCurPoint;
				}
			}

			if ( i >= (int)tempText.size() )
				break;
		}
	}

#ifdef SERV_IRUHADEV_JUNE_UI_DEBUG
	static int s_iLogColor = 0;
	if ( ++s_iLogColor <= 6 )
		KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] UI WRAP color width " << width << " font " << fontIndex << " chars " << (int)lstrlenW( pOrgStr ) << " -> " << (int)tempText.size() << fileout;
#endif SERV_IRUHADEV_JUNE_UI_DEBUG
	return tempText;
}

// June2017 31d42bc0: the plain wrap, then everything after line nLineNum - 1 is replaced by "..." (bEllipse tells whether text was cut).
std::wstring CKTDXWordWrap::GetStrByLineBreakWithEllipse( const WCHAR* pOrgStr, int width, int fontIndex, const int nLineNum, bool& bEllipse )
{
	std::wstring wstrText = GetStrByLineBreak( pOrgStr, width, fontIndex, NULL );

	if ( nLineNum < 1 )
		return wstrText;

	std::wstring wstrTempName;
	int ilastCutPoint = (int)wstrText.find( L"\n" );
	int iPos = (int)wstrText.find( L"\n" );
	int lineNum = 0;

	while ( lineNum < nLineNum - 1 )
	{
		if ( iPos == (int)std::wstring::npos || (int)wstrText.size() == iPos + 1 )
		{
			wstrTempName += wstrText.substr( ( ilastCutPoint + 1 ) * lineNum, (int)wstrText.size() - ( ( ilastCutPoint + 1 ) * lineNum ) );

			bEllipse = false;
			return wstrTempName;
		}

		wstrTempName += wstrText.substr( ( ilastCutPoint + 1 ) * lineNum, ilastCutPoint + 1 );
		iPos = (int)wstrText.find( L"\n", ++iPos );
		lineNum++;
	}

	if ( iPos == (int)std::wstring::npos || (int)wstrText.size() == iPos + 1 )
	{
		if ( (int)wstrText.size() > ( ( ilastCutPoint + 1 ) * lineNum ) )
			wstrTempName += wstrText.substr( ( ilastCutPoint + 1 ) * lineNum, (int)wstrText.size() - ( ( ilastCutPoint + 1 ) * lineNum ) );

		bEllipse = false;
	}
	else
	{
		wstrTempName += wstrText.substr( ( ilastCutPoint + 1 ) * lineNum, iPos - ( ilastCutPoint + 1 ) * lineNum - 3 );
		wstrTempName += L"...";

		bEllipse = true;
	}

	return wstrTempName;
}

// June2017 31d45dd0
std::wstring CKTDXWordWrap::GetStrByLineBreakByType( const WCHAR* pOrgStr, int width, int iType, int fontIndex )
{
	if ( iType == 0 )
		return GetStrByLineBreakColor( pOrgStr, width, fontIndex );

	if ( iType > 0 )
	{
		bool bEllipse = false;
		return GetStrByLineBreakWithEllipse( pOrgStr, width, fontIndex, iType, bEllipse );
	}

	return pOrgStr != NULL ? std::wstring( pOrgStr ) : std::wstring( L"" );
}

// June2017 31d45f20: wraps the string again when X2Lib marked it (bRewrapPending); called from the control's OnFrameRender.
void CKTDGUIControl::UIStringData::RewrapPending()
{
	if ( bRewrapPending == true && 0 < iLineBreakLength )
	{
		std::wstring wstrText = msg;
		msg = CKTDXWordWrap::GetStrByLineBreakByType( wstrText.c_str(), iLineBreakLength, iLineBreakType, fontIndex );
		bRewrapPending = false;
	}
}

#endif SERV_IRUHADEV_JUNE_UI_LINE_BREAK
