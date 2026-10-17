#ifdef SERV_IRUHADEV_JUNE_UI_LINE_BREAK
#pragma once

// June 2017 engine word-wrap helpers (june-engine Phase U1 group D).
// June keeps ONE copy of the line-break helpers and it sits in the engine, between CKTDXDeviceSound (31d411c0) and
// CKTDXDevice (31d4d300): 31d42a60 .. 31d45f20 (+ 31cbff90). March had them in X2Lib (CWordLineHandler) only;
// under SERV_IRUHADEV_JUNE_UI_LINE_BREAK CWordLineHandler forwards here.

class CKTDXWordWrap
{
	public:
		/// June 31d42a60 / 31d42b10 (= March CWordLineHandler::IsSpaceLatter / IsSplitLatter, same character sets)
		static bool			IsSpaceLatter( WCHAR wcChar );
		static bool			IsSplitLatter( WCHAR wcChar );

		/// June 31cbff90 (reached through 31d471f0) = March GetStrByLineBreakInX2Main, June's word-aware algorithm.
		/// pFont != NULL overrides the font looked up by fontIndex (June's 5th argument).
		static std::wstring	GetStrByLineBreak( const WCHAR* pOrgStr, int width, int fontIndex, CKTDGFontManager::CUKFont* pFont = NULL );

		/// June 31d43ef0 = March GetStrByLineBreakColorInX2Main (keeps the active #C color across inserted line breaks)
		static std::wstring	GetStrByLineBreakColor( const WCHAR* pOrgStr, int width, int fontIndex );

		/// June 31d42bc0 = March GetStrByLineBreakInX2MainWithEllipse
		static std::wstring	GetStrByLineBreakWithEllipse( const WCHAR* pOrgStr, int width, int fontIndex, const int nLineNum, bool& bEllipse );

		/// June 31d45dd0: LINE_BREAK_TYPE dispatch. type 0 -> Color wrap, type > 0 -> ellipse wrap with type lines, type < 0 -> unchanged.
		static std::wstring	GetStrByLineBreakByType( const WCHAR* pOrgStr, int width, int iType, int fontIndex );
};

#endif SERV_IRUHADEV_JUNE_UI_LINE_BREAK
