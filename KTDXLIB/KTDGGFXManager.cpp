#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_GFX_MANAGER
#include "KTDGGFXManager.h"

// Scaleform's own headers. Include dirs ( Libs\ExternalLib\Scaleform\Src and \Include ) are set per file in KTDXLIB_2010.vcxproj
// for this .cpp only ( as for KTDGFXRuntime.cpp ), so no March #include resolves differently.
#include "GFx/GFx_Player.h"
#include "GFx/GFx_Loader.h"
#include "GFx/GFx_Log.h"
#include "GFx/GFx_Event.h"
#include "GFx/GFx_FontLib.h"
#include "GFx/GFx_ImageCreator.h"
#include "GFx/GFx_AS2Support.h"
#include "GFx/GFx_AS3Support.h"
#include "GFx/GFx_Audio.h"
#include "GFx/GFx_FontProviderWin32.h"
#include "Render/Renderer2D.h"
#include "Render/D3D9/D3D9_HAL.h"
#include "Render/ImageFiles/JPEG_ImageFile.h"
#include "Render/ImageFiles/PNG_ImageFile.h"
#include "Render/ImageFiles/TGA_ImageFile.h"
#include "Render/ImageFiles/DDS_ImageFile.h"
#include "Sound/Sound_SoundRendererFMOD.h"
#include "Kernel/SF_File.h"

#pragma comment( lib, "imm32.lib" )		// ImmIsUIMessageW / ImmGetContext / ImmSetConversionStatus ( 31c43ab0, 31c435b0 )

// The AS3 VM's class table: the SDK leaves Classes::ClassRegistrationTable for the application to define, by including AS3_Global.h in
// exactly one .cpp ( as FxPlayer does ). It names every AS3 class, so libgfx_as3's XML class pulls in expat ( Scaleform's expat.lib;
// the client has no expat of its own, its libxml is a different library ).
#include "GFx/AS3/AS3_Global.h"
#pragma comment( lib, "expat.lib" )

// Two more zlib entry points than G1's six ( KTDGFXRuntime.cpp ): libgfx's ZLibFile::Reset wants inflateReset and AS3 ByteArray.compress
// wants compress2. Same shim: cdecl wrappers over the client's ZLIB_WINAPI zlib 1.2.3 ( zlibstat.lib: _inflateReset@4, _compress2@20 ).
extern "C"
{
	int __cdecl sfgfx_inflateReset( z_streamp strm )		{ return inflateReset( strm ); }
	int __cdecl sfgfx_compress2( Bytef* dest, uLongf* destLen, const Bytef* source, uLong sourceLen, int level )
															{ return compress2( dest, destLen, source, sourceLen, level ); }
}
#pragma comment( linker, "/alternatename:_inflateReset=_sfgfx_inflateReset" )
#pragma comment( linker, "/alternatename:_compress2=_sfgfx_compress2" )

namespace SF	= Scaleform;
namespace SFG	= Scaleform::GFx;
namespace SFR	= Scaleform::Render;

// ---------------------------------------------------------------------------------------------------------------------------------------
// Log lines. Everything the manager logs during Init ( OnCreateDevice, right after the koms mount ) is held and written at the first frame,
// since a line logged that early never reaches ErrorLog.txt ( memory errorlog-loses-mount-time-lines ).
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	CRITICAL_SECTION			s_csGfxLog;
	bool						s_bGfxLogInit		= false;
	bool						s_bGfxLogFlushed	= false;
	std::vector< std::string >	s_vecGfxLogPending;
#ifdef SERV_IRUHADEV_JUNE_GFX_DEBUG
	int							s_iGfxSdkLogLines	= 0;		// GFx's own log mirrored to ErrorLog.txt ( DEBUG only ), capped
	const int					GFX_SDK_LOG_CAP	= 300;
#endif SERV_IRUHADEV_JUNE_GFX_DEBUG

	void GfxLogLine( const std::string& strLine )
	{
		if ( s_bGfxLogInit == false )
		{
			InitializeCriticalSection( &s_csGfxLog );
			s_bGfxLogInit = true;
		}
		EnterCriticalSection( &s_csGfxLog );
		if ( s_bGfxLogFlushed == false )
		{
			s_vecGfxLogPending.push_back( strLine );
		}
		else
		{
			KLOG( "ErrorLog.txt" ) << strLine.c_str() << fileout;
		}
		LeaveCriticalSection( &s_csGfxLog );
	}

	void GfxLog( const char* szFormat, ... )
	{
		char szBuf[2048] = { 0, };
		va_list args;
		va_start( args, szFormat );
		_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
		va_end( args );
		GfxLogLine( std::string( "[JUNE_GFX] MGR " ) + szBuf );
	}

	std::string GfxToUTF8( const wchar_t* wsz )
	{
		if ( wsz == NULL || wsz[0] == 0 )
			return std::string();
		int iLen = WideCharToMultiByte( CP_UTF8, 0, wsz, -1, NULL, 0, NULL, NULL );
		std::string str( iLen > 0 ? iLen - 1 : 0, '\0' );
		if ( iLen > 1 )
			WideCharToMultiByte( CP_UTF8, 0, wsz, -1, &str[0], iLen, NULL, NULL );
		return str;
	}

	std::wstring GfxFromUTF8( const char* sz )
	{
		if ( sz == NULL || sz[0] == 0 )
			return std::wstring();
		int iLen = MultiByteToWideChar( CP_UTF8, 0, sz, -1, NULL, 0 );
		std::wstring wstr( iLen > 0 ? iLen - 1 : 0, L'\0' );
		if ( iLen > 1 )
			MultiByteToWideChar( CP_UTF8, 0, sz, -1, &wstr[0], iLen );
		return wstr;
	}

	// June reads its .gfx / fontconfig.font out of the koms through the mass-file map only ( r3dGFXFileOpener 31c47a00 -> 31c4c170,
	// ConfigParser 31d22280 -> 31c4c3d0, fontlib check 31c4bcb0 ); KGCMassFileManager::LoadDataFile_MassFileOnly is that lookup.
	bool GfxLoadFromKom( const std::wstring& wstrName, std::vector< unsigned char >& vecOut )
	{
		vecOut.clear();
		if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDeviceManager() == NULL )
			return false;
		KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR Info =
			g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile_MassFileOnly( wstrName );
		if ( Info->pRealData == NULL || Info->size <= 0 )
			return false;
		vecOut.assign( (const unsigned char*)Info->pRealData, (const unsigned char*)Info->pRealData + Info->size );
		return true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// GFxPlayerLog ( RTTI .?AVGFxPlayerLog@@, 0x824 B, ctor 31c46880, vtable 33446d0c ): June's is the FxPlayer sample's console log
// ( GetStdHandle( STD_OUTPUT_HANDLE ) + colour attributes 7 / 0xd ). A GUI exe has no console, so the text goes to the debugger, and under
// SERV_IRUHADEV_JUNE_GFX_DEBUG it is mirrored to ErrorLog.txt as "[JUNE_GFX] MGR gfx: ...".
// ---------------------------------------------------------------------------------------------------------------------------------------
class GFxPlayerLog : public SF::Log
{
public:
	virtual void LogMessageVarg( SF::LogMessageId messageId, const char* pfmt, va_list argList )
	{
		char szBuf[1024] = { 0, };
		SF::Log::FormatLog( szBuf, sizeof( szBuf ), messageId, pfmt, argList );
		OutputDebugStringA( szBuf );
#ifdef SERV_IRUHADEV_JUNE_GFX_DEBUG
		if ( s_iGfxSdkLogLines < GFX_SDK_LOG_CAP )
		{
			++s_iGfxSdkLogLines;
			std::string strLine( szBuf );
			while ( strLine.empty() == false && ( strLine[ strLine.size() - 1 ] == '\n' || strLine[ strLine.size() - 1 ] == '\r' ) )
				strLine.erase( strLine.size() - 1 );
			GfxLogLine( std::string( "[JUNE_GFX] MGR gfx: " ) + strLine );
		}
#endif SERV_IRUHADEV_JUNE_GFX_DEBUG
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------------
// r3dGFXFileOpener ( RTTI .?AVr3dGFXFileOpener@@, vtable 33446d2c; only OpenFile 31c47a00 is June's, GetFileModifyTime / OpenFileEx are the
// SDK's ). The url is lower-cased and looked up as a kom member ( 31c4c170, which logs a miss as KEM_ERROR135 ); a member becomes a memory file ( June decodes algo-2 there,
// March's GetMemberFile does its own formats ). A miss, or an empty member, goes to the SDK FileOpener::OpenFile ( 33087ed0 ): SysFile on
// disk with the url as given. That is how the loose eventscene\X.swf ( GetPullPath full paths ) open.
// ---------------------------------------------------------------------------------------------------------------------------------------
class GfxKomFileBuffer
{
protected:
	explicit GfxKomFileBuffer( std::vector< unsigned char >& vecData ) { m_vecData.swap( vecData ); }
	std::vector< unsigned char >	m_vecData;
};

// MemoryFile neither copies nor owns its buffer: the buffer base is declared first, so it is built ( and holds the kom bytes ) before
// MemoryFile is handed their address
class GfxKomFile : protected GfxKomFileBuffer, public SF::MemoryFile
{
public:
	GfxKomFile( const char* szURL, std::vector< unsigned char >& vecData )
		: GfxKomFileBuffer( vecData ), SF::MemoryFile( szURL, &m_vecData[0], (int)m_vecData.size() )
	{
	}
};

class r3dGFXFileOpener : public SFG::FileOpener
{
public:
	virtual SF::File* OpenFile( const char* purl, int flags, int modes )
	{
		std::wstring wstrName = GfxFromUTF8( purl );
		std::transform( wstrName.begin(), wstrName.end(), wstrName.begin(), towlower );

		std::vector< unsigned char > vecData;
		if ( GfxLoadFromKom( wstrName, vecData ) == true && vecData.empty() == false )
			return new GfxKomFile( purl, vecData );

		return SFG::FileOpener::OpenFile( purl, flags, modes );
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------------
// FxPlayerTextClipboard ( RTTI .?AVFxPlayerTextClipboard@@, vtable 33446bd4 ): OnTextStore ( 31c46550 ) puts a GFx copy on the Windows
// clipboard as CF_UNICODETEXT; the paste side is read on WM_ACTIVATEAPP ( 31c44040 -> 31c43de0 ).
// ---------------------------------------------------------------------------------------------------------------------------------------
class FxPlayerTextClipboard : public SFG::TextClipboard
{
public:
	virtual void OnTextStore( const wchar_t* ptext, SF::UPInt len )
	{
		if ( OpenClipboard( NULL ) == FALSE )
			return;
		EmptyClipboard();
		HGLOBAL hMem = GlobalAlloc( GMEM_DDESHARE, ( len + 1 ) * sizeof( wchar_t ) );
		wchar_t* pDest = (wchar_t*)GlobalLock( hMem );
		wcscpy_s( pDest, len + 1, ptext );
		GlobalUnlock( hMem );
		SetClipboardData( CF_UNICODETEXT, hMem );
		CloseClipboard();
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------------
// FontConfig ( FxPlayer FontConfigParser, transcribed from June: ConfigParser 31d22280, FontConfigSet::Parse 31d227e0, tokenizer 31d21b60,
// line reader 31d224e0, matcher 31d220d0, string token 31d22fe0, map style 31d21f40, FontConfig ctor 31d23c10, Apply 31d21e10,
// FontTranslator::Translate 31d225d0 ). The SDK ships no FontConfigParser ( only a PS3 provider ).
// ---------------------------------------------------------------------------------------------------------------------------------------
class FontTranslator : public SFG::Translator		// RTTI .?AVFontTranslator@GFx@Scaleform@@, 0x14 B, vtable 3344dfcc
{
public:
	virtual unsigned GetCaps() const { return Cap_StripTrailingNewLines; }		// 330673a0: return 2
	virtual void Translate( TranslateInfo* ptranslateInfo )					// 31d225d0
	{
		std::map< std::wstring, std::wstring >::const_iterator it = m_mapTable.find( ptranslateInfo->GetKey() );
		if ( it != m_mapTable.end() )
			ptranslateInfo->SetResult( it->second.c_str(), it->second.size() );
	}
	// 31d226d0 -> 31d239f0: a hash Add; fontconfig.font has no repeated key ( 9,133 distinct ), so the first value is kept as in a lookup
	void AddTranslation( const std::wstring& wstrKey, const std::wstring& wstrValue ) { m_mapTable.insert( std::make_pair( wstrKey, wstrValue ) ); }
	size_t GetCount() const { return m_mapTable.size(); }
private:
	std::map< std::wstring, std::wstring >	m_mapTable;
};

class FontConfig : public SF::RefCountBase< FontConfig, SF::Stat_Default_Mem >		// RTTI .?AVFontConfig@GFx@Scaleform@@, 0x24 B
{
public:
	FontConfig( const std::wstring& wstrName ) : m_wstrName( wstrName ), m_iMapCount( 0 ) { m_pTranslator = *new FontTranslator(); }

	// 31d21e10
	void Apply( SFG::Loader& kLoader )
	{
		kLoader.SetFontLib( NULL );
		kLoader.SetFontMap( m_pFontMap );
		kLoader.SetTranslator( m_pTranslator );
		if ( m_vecFontLibFiles.empty() == false )
		{
			SF::Ptr< SFG::FontLib > pFontLib = *new SFG::FontLib();
			kLoader.SetFontLib( pFontLib );
			for ( size_t i = 0; i < m_vecFontLibFiles.size(); ++i )
			{
				SF::Ptr< SFG::MovieDef > pDef = *kLoader.CreateMovie( m_vecFontLibFiles[i].c_str(), SFG::Loader::LoadAll, 0 );
				if ( pDef )
					pFontLib->AddFontsFrom( pDef, false );		// 31b796a0( def, 0 )
			}
		}
	}

	std::wstring						m_wstrName;			// +8
	std::vector< std::string >			m_vecFontLibFiles;	// +0x10 / +0x14 ( UTF-8 )
	SF::Ptr< SFG::FontMap >				m_pFontMap;			// +0x1c, made by the first map line
	SF::Ptr< FontTranslator >			m_pTranslator;		// +0x20, made by the ctor
	int									m_iMapCount;
};

class GfxConfigParser
{
public:
	enum TOKEN_TYPE { TT_ERROR = 0, TT_EOL = 1, TT_EOF = 2, TT_CHAR = 3, TT_STRING = 4, TT_IDENT = 5, TT_NUMBER = 6 };
	struct Token
	{
		TOKEN_TYPE		eType;
		size_t			uStart;
		size_t			uLen;
		const char*		szError;
	};

	// 31d22280: the file comes from the koms ( 31c4c3d0 ); UTF-16 LE / BE by BOM, UTF-8 BOM skipped, else read as UTF-8
	explicit GfxConfigParser( const char* szFileName ) : m_uPos( 0 ), m_iLine( 1 ), m_iErrorIndex( -1 ), m_iTokenLine( 1 )
	{
		std::vector< unsigned char > vecData;
		if ( GfxLoadFromKom( GfxFromUTF8( szFileName ), vecData ) == false || vecData.empty() == true )
			return;
		if ( vecData.size() >= 2 && vecData[0] == 0xff && vecData[1] == 0xfe )
		{
			size_t uCount = vecData.size() / 2 - 1;
			m_wstrText.resize( uCount );
			for ( size_t i = 0; i < uCount; ++i )
				m_wstrText[i] = (wchar_t)( vecData[ 2 + i * 2 ] | ( vecData[ 3 + i * 2 ] << 8 ) );
		}
		else if ( vecData.size() >= 2 && vecData[0] == 0xfe && vecData[1] == 0xff )
		{
			size_t uCount = vecData.size() / 2 - 1;
			m_wstrText.resize( uCount );
			for ( size_t i = 0; i < uCount; ++i )
				m_wstrText[i] = (wchar_t)( ( vecData[ 2 + i * 2 ] << 8 ) | vecData[ 3 + i * 2 ] );
		}
		else
		{
			size_t uSkip = ( vecData.size() >= 3 && vecData[0] == 0xef && vecData[1] == 0xbb && vecData[2] == 0xbf ) ? 3 : 0;
			std::string strUTF8( (const char*)&vecData[ uSkip ], vecData.size() - uSkip );
			m_wstrText = GfxFromUTF8( strUTF8.c_str() );
		}
	}

	bool	HasData() const		{ return m_wstrText.empty() == false; }
	int		GetLine() const		{ return m_iTokenLine; }
	int		GetErrorIndex() const { return m_iErrorIndex; }
	const char* GetErrorText() const { return m_vecTokens[ m_iErrorIndex ].szError; }

	// 31d224e0: one line of tokens, up to and including the EOL / EOF / error token; false at the end of the file
	bool NextLine()
	{
		m_iErrorIndex = -1;
		m_iTokenLine = m_iLine;
		m_vecTokens.clear();
		for ( ;; )
		{
			Token kToken = ReadToken();
			if ( kToken.eType == TT_ERROR && m_iErrorIndex == -1 )
				m_iErrorIndex = (int)m_vecTokens.size();
			m_vecTokens.push_back( kToken );
			if ( kToken.eType < TT_CHAR )
				return !( m_vecTokens.size() < 2 && kToken.eType == TT_EOF );
		}
	}

	// 31d220d0: %i keyword ( case-insensitive ), %s string, %I identifier, %n number; any other character must be that punctuation token.
	// The whole format must be used and the next token must end the line.
	bool Match( const wchar_t* wszFormat, const wchar_t* wszKeyword = NULL, std::wstring* pStr1 = NULL, std::wstring* pStr2 = NULL,
		std::wstring* pStr3 = NULL, std::wstring* pStr4 = NULL, float* pfNumber = NULL )
	{
		std::wstring* apStr[4] = { pStr1, pStr2, pStr3, pStr4 };
		int iStr = 0;
		size_t uToken = 0;
		for ( const wchar_t* p = wszFormat; *p != 0; ++p, ++uToken )
		{
			if ( uToken >= m_vecTokens.size() )
				return false;
			const Token& kToken = m_vecTokens[ uToken ];
			if ( *p != L'%' )
			{
				if ( kToken.eType != TT_CHAR || m_wstrText[ kToken.uStart ] != *p )
					return false;
				continue;
			}
			++p;
			if ( *p == L'i' )
			{
				if ( kToken.eType != TT_IDENT || wszKeyword == NULL )
					return false;
				std::wstring wstrIdent = m_wstrText.substr( kToken.uStart, kToken.uLen );
				if ( wcslen( wszKeyword ) != wstrIdent.size() || _wcsicmp( wstrIdent.c_str(), wszKeyword ) != 0 )
					return false;
			}
			else if ( *p == L's' || *p == L'I' )
			{
				if ( kToken.eType != ( *p == L's' ? TT_STRING : TT_IDENT ) )
					return false;
				if ( iStr < 4 && apStr[ iStr ] != NULL )
					*apStr[ iStr ] = TokenString( kToken );
				++iStr;
			}
			else if ( *p == L'n' )
			{
				if ( kToken.eType != TT_NUMBER )
					return false;
				if ( pfNumber != NULL )
					*pfNumber = (float)atof( GfxToUTF8( m_wstrText.substr( kToken.uStart, kToken.uLen ).c_str() ).c_str() );
			}
			// any other letter: the token is skipped ( 31d22220 )
			if ( *p == 0 )
				break;
		}
		return uToken < m_vecTokens.size() && ( m_vecTokens[ uToken ].eType == TT_EOL || m_vecTokens[ uToken ].eType == TT_EOF );
	}

private:
	// 31d21b60 ( whitespace 31d21ae0: space and tab; identifier start 31d21b10; number start 31d21b40 )
	Token ReadToken()
	{
		Token kToken = { TT_EOF, 0, 0, "" };
		const size_t uLen = m_wstrText.size();
		if ( uLen == 0 || m_uPos == uLen )
			return kToken;
		while ( m_uPos < uLen && ( m_wstrText[ m_uPos ] == L' ' || m_wstrText[ m_uPos ] == L'\t' ) )
			++m_uPos;
		if ( m_uPos < uLen && m_wstrText[ m_uPos ] == 0x1a && ++m_uPos == uLen )
			return kToken;
		if ( m_uPos >= uLen )
			return kToken;

		const wchar_t c = m_wstrText[ m_uPos ];
		if ( c == L'\r' && m_uPos + 1 < uLen && m_wstrText[ m_uPos + 1 ] == L'\n' )
		{
			++m_iLine;
			m_uPos += 2;
			kToken.eType = TT_EOL;
			return kToken;
		}
		if ( c == L'\n' )
		{
			++m_iLine;
			++m_uPos;
			kToken.eType = TT_EOL;
			return kToken;
		}
		if ( ( c >= L'a' && c <= L'z' ) || ( c >= L'A' && c <= L'Z' ) || c == L'_' )
		{
			kToken.eType = TT_IDENT;
			kToken.uStart = m_uPos++;
			while ( m_uPos < uLen )
			{
				const wchar_t d = m_wstrText[ m_uPos ];
				if ( !( ( d >= L'a' && d <= L'z' ) || ( d >= L'A' && d <= L'Z' ) || d == L'_' || ( d >= L'0' && d <= L'9' ) ) )
					break;
				++m_uPos;
			}
			kToken.uLen = m_uPos - kToken.uStart;
			return kToken;
		}
		if ( ( c >= L'0' && c <= L'9' ) || c == L'.' )
		{
			bool bDot = ( c == L'.' );
			kToken.eType = TT_NUMBER;
			kToken.uStart = m_uPos++;
			while ( m_uPos < uLen )
			{
				const wchar_t d = m_wstrText[ m_uPos ];
				if ( ( d < L'0' || d > L'9' ) && d != L'.' )
					break;
				if ( d == L'.' )
				{
					if ( bDot )
						break;
					bDot = true;
				}
				++m_uPos;
			}
			kToken.uLen = m_uPos - kToken.uStart;
			return kToken;
		}
		++m_uPos;
		if ( c != L'"' )
		{
			kToken.eType = TT_CHAR;
			kToken.uStart = m_uPos - 1;
			kToken.uLen = 1;
			return kToken;
		}
		const size_t uStart = m_uPos;
		while ( m_uPos < uLen && m_wstrText[ m_uPos ] != L'"' )
			++m_uPos;
		if ( m_uPos >= uLen )
		{
			kToken.eType = TT_ERROR;
			kToken.szError = "closing quote \" character expected";
			return kToken;
		}
		++m_uPos;
		kToken.eType = TT_STRING;
		kToken.uStart = uStart;
		kToken.uLen = m_uPos - uStart - 1;
		return kToken;
	}

	// 31d22fe0: LF dropped; "\n" becomes CR ( 0x0d, June's own choice: Flash breaks lines on \r ); any other "\x" stays as the two characters
	std::wstring TokenString( const Token& kToken ) const
	{
		std::wstring wstrOut;
		bool bEscape = false;
		for ( size_t i = 0; i < kToken.uLen; ++i )
		{
			const wchar_t c = m_wstrText[ kToken.uStart + i ];
			if ( c == L'\n' )
				continue;
			if ( bEscape )
			{
				bEscape = false;
				if ( c == L'n' )
				{
					wstrOut += (wchar_t)0x0d;
				}
				else
				{
					wstrOut += L'\\';
					wstrOut += c;
				}
			}
			else if ( c == L'\\' )
			{
				bEscape = true;
			}
			else
			{
				wstrOut += c;
			}
		}
		return wstrOut;
	}

	std::wstring			m_wstrText;
	size_t					m_uPos;			// +0x10
	int						m_iLine;		// +0x14
	std::vector< Token >	m_vecTokens;
	int						m_iErrorIndex;
	int						m_iTokenLine;
};

class FontConfigSet
{
public:
	// 31d21f40: map font style, chained from MFF_Original ( 0x10 ); June warns and keeps the previous flags on a bad word
	static unsigned ParseStyle( unsigned uPrev, const std::wstring& wstrStyle, int iLine )
	{
		unsigned uFlag = 0;
		if		( _wcsicmp( wstrStyle.c_str(), L"bold" ) == 0 )			uFlag = 2;
		else if ( _wcsicmp( wstrStyle.c_str(), L"normal" ) == 0 )		uFlag = 0;
		else if ( _wcsicmp( wstrStyle.c_str(), L"italic" ) == 0 )		uFlag = 1;
		else if ( _wcsicmp( wstrStyle.c_str(), L"fauxbold" ) == 0 )		uFlag = 8;
		else if ( _wcsicmp( wstrStyle.c_str(), L"fauxitalic" ) == 0 )	uFlag = 4;
		else if ( _wcsicmp( wstrStyle.c_str(), L"noautofit" ) == 0 )	uFlag = 0x20;
		else
		{
			GfxLog( "Warning: FontConfig(%d) - unknown map font style '%s'", iLine, GfxToUTF8( wstrStyle.c_str() ).c_str() );
			return uPrev;
		}
		if ( uPrev == 0x10 )
			return uFlag;
		if ( ( uPrev == 0 && ( uFlag & 3 ) != 0 ) || ( uFlag == 0 && ( uPrev & 3 ) != 0 ) )
		{
			GfxLog( "Warning: FontConfig(%d) - unexpected map font style '%s'", iLine, GfxToUTF8( wstrStyle.c_str() ).c_str() );
			return uPrev;
		}
		return uPrev | uFlag;
	}

	static void AddMap( FontConfig* pConfig, const std::wstring& wstrFrom, const std::wstring& wstrTo, unsigned uFlags, float fScale )
	{
		if ( !pConfig->m_pFontMap )
			pConfig->m_pFontMap = *new SFG::FontMap();
		pConfig->m_pFontMap->MapFont( wstrFrom.c_str(), wstrTo.c_str(), (SFG::FontMap::MapFontFlags)uFlags, fScale );
		++pConfig->m_iMapCount;
	}

	// 31d227e0 ( June's errors go to stderr; here they are log lines )
	void Parse( GfxConfigParser& kParser )
	{
		m_vecConfigs.clear();
		if ( kParser.HasData() == false )
			return;

		std::wstring a, b, c, d;
		float fScale = 1.0f;
		while ( kParser.NextLine() == true )
		{
			if ( kParser.GetErrorIndex() != -1 )
			{
				GfxLog( "Error: FontConfig(%d) - %s", kParser.GetLine(), kParser.GetErrorText() );
				break;
			}
			if ( kParser.Match( L"[%i]", L"FontConfig" ) == true )
			{
				m_vecConfigs.push_back( *new FontConfig( L"Unnamed" ) );
				continue;
			}
			if ( kParser.Match( L"[%i%s]", L"FontConfig", &a ) == true )
			{
				m_vecConfigs.push_back( *new FontConfig( a ) );
				continue;
			}
			if ( kParser.Match( L"" ) == true )
				continue;
			if ( m_vecConfigs.empty() == true )
			{
				GfxLog( "Error: FontConfig(%d) - file mappings outside of [FontConfig] section", kParser.GetLine() );
				break;
			}
			FontConfig* pConfig = m_vecConfigs.back();
			fScale = 1.0f;
			if ( kParser.Match( L"%i%s=%s", L"map", &a, &b ) == true )
				AddMap( pConfig, a, b, 0x10, 1.0f );
			else if ( kParser.Match( L"%i%s=%s%I", L"map", &a, &b, &c ) == true )
				AddMap( pConfig, a, b, ParseStyle( 0x10, c, kParser.GetLine() ), 1.0f );
			else if ( kParser.Match( L"%i%s=%s%n", L"map", &a, &b, NULL, NULL, &fScale ) == true )
				AddMap( pConfig, a, b, 0x10, fScale );
			else if ( kParser.Match( L"%i%s=%s%I%n", L"map", &a, &b, &c, NULL, &fScale ) == true )
				AddMap( pConfig, a, b, ParseStyle( 0x10, c, kParser.GetLine() ), fScale );
			else if ( kParser.Match( L"%i%s=%s%I%I", L"map", &a, &b, &c, &d ) == true )
				AddMap( pConfig, a, b, ParseStyle( ParseStyle( 0x10, c, kParser.GetLine() ), d, kParser.GetLine() ), 1.0f );
			else if ( kParser.Match( L"%i%s=%s%I%I%n", L"map", &a, &b, &c, &d, &fScale ) == true )
				AddMap( pConfig, a, b, ParseStyle( ParseStyle( 0x10, c, kParser.GetLine() ), d, kParser.GetLine() ), fScale );
			else if ( kParser.Match( L"%i%s", L"fontlib", &a ) == true )
				pConfig->m_vecFontLibFiles.push_back( GfxToUTF8( a.c_str() ) );	// parser +0x18 path prefix: empty for a kom file name
			else if ( kParser.Match( L"%i%s=%s", L"tr", &a, &b ) == true )
				pConfig->m_pTranslator->AddTranslation( a, b );
			else
			{
				GfxLog( "Error: FontConfig(%d) - unexpected statement", kParser.GetLine() );
				break;
			}
		}
	}

	std::vector< SF::Ptr< FontConfig > >	m_vecConfigs;		// mgr +0xf8 / +0xfc
};

// ---------------------------------------------------------------------------------------------------------------------------------------
// The AS -> C++ side. June's movie carries a CLIKDataBindingAdapter ( RTTI .?AVCLIKDataBindingAdapter@@, 0x24 B, ctor 31d1f7b0 ) as its
// ExternalInterface. Its Callback ( 31d1ed00 ) walks a std::map< name, listener > and stops at the first listener that returns true;
// AddListener is 31d1f690. The manager's own listener ( mgr +4, name "GFXManager", 31c48570 ) is callback 31c45ed0. Stage 9's screens
// add theirs to the same map.
// ---------------------------------------------------------------------------------------------------------------------------------------
class IGfxListener
{
public:
	virtual ~IGfxListener() {}
	virtual std::string	GetListenerName() const = 0;
	virtual bool		OnExternalCall( SFG::Movie* pMovie, const char* szMethod, const SFG::Value* pArgs, unsigned uArgCount ) = 0;
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	// vtable slot 3 ( +0xc ), called by 31d1ed90 when the AS side adds an observer to a binding name; the walk stops at the first listener that answers true
	virtual bool		OnBindingCreated( const std::string& /*strName*/ ) { return false; }
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
};

#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
// ---------------------------------------------------------------------------------------------------------------------------------------
// Phase G3. June wraps the adapter in a second ExternalInterface ( RTTI-less, vtable 3344deb8, 0x14 B, Callback 31d20430 ): the method
// "dataBindingInit" is answered there and everything else is forwarded to the adapter's listener walk. The adapter itself ( vtable 3344deec,
// ctor 31d1f7b0 ) also owns the binding table the AS3 class gfxAS3.gameData.DataBinding talks to: name -> { value object, observers }.
//   dataBindingInit( obj )       : obj.createBindingImpl / obj.deleteBindingImpl = native functions ( 31d1f0a0 / 31d1f3a0 ), table 3344de9c
//   createBindingImpl( name, a, b ): a known name gets the observer ( a, b ) appended ( 31d1fb40 pair, 31d1f000 ), then the listeners are told
//   deleteBindingImpl( name, a, b ): removes the observer whose two Values equal ( a, b ) ( 31d1f230 )
//   Notify( name )               : the value object's current value ( trigger = null, bool ) is passed to every observer's closure
//                                  ( 31d1ee10 -> 31d1ec40: ObjectInterface slot 7 = InvokeClosure on the first Value )
// The two value objects the event scene registers: "EventScene.nextScene" ( vtable 33471e28, GetValue 31fc76f0 = Null ) and
// "EventScene.bExactFit" ( vtable 334c9084, GetValue 32948f70 = Boolean at +8 ).
// ---------------------------------------------------------------------------------------------------------------------------------------
enum GFX_BINDING_KIND
{
	JGBK_TRIGGER	= 0,
	JGBK_BOOL		= 1,
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	JGBK_SIMPLE_WIDGET	= 2,	// G4: "SimpleWidgetPlayer.playSimpleWidget" ( vtable 33471e40, GetValue 31fc7720 ): { widgetID, fileName, parameter1, parameter2 }
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	JGBK_RESULT_INFO	= 3,	// R1: "GameResult.updateDungeonInfo" ( DW_DUNGEON_RESULT_VALUE, vtable 3353a0b0, GetValue 32eeb1d0 )
	JGBK_RESULT_ITEM	= 4,	// R1: "GameResult.getItemN" ( vtable 3353a080, GetValue 32eeaff0 ): { itemID, iconPath, iNum }
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
};

struct KGfxBindingObserver
{
	SFG::Value	m_kClosure;		// record +0x00: the Value the notify invokes
	SFG::Value	m_kExtra;		// record +0x18: compared on delete only
};

struct KGfxBinding
{
	int			m_iKind;
	bool		m_bValue;
	std::vector< KGfxBindingObserver >	m_vecObserver;
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	KGfxResultInfo	m_kResultInfo;		// R1: the 0x88 B value object
	KGfxResultItem	m_kResultItem;		// R1: the 0x40 B value object
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	int			m_iWidgetID;		// value object +8
	std::string	m_strFileName;		// +0xc
	int			m_iParameter1;		// +0x28
	int			m_iParameter2;		// +0x2c
	KGfxBinding() : m_iKind( JGBK_TRIGGER ), m_bValue( false ), m_iWidgetID( 0 ), m_iParameter1( 0 ), m_iParameter2( 0 ) {}
#else SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	KGfxBinding() : m_iKind( JGBK_TRIGGER ), m_bValue( false ) {}
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
};

// The link a native function keeps to its owner: the movie can outlive the C++ object that registered the function, a late call is dropped
class CGfxOwnerLink : public SF::RefCountBase< CGfxOwnerLink, SF::Stat_Default_Mem >
{
public:
	explicit CGfxOwnerLink( void* pOwner ) : m_pOwner( pOwner ) {}
	void*	m_pOwner;
};

// June's 0xc B function objects ( vtable 3344ded0 / 33531fc8, Call 32eee800 ): the Params go to a plain function with the owner
class CGfxFunctionHandler : public SFG::FunctionHandler
{
public:
	typedef void ( *PFN_CALLBACK )( void* pOwner, int iID, const SFG::FunctionHandler::Params& kParams );
	CGfxFunctionHandler( CGfxOwnerLink* pLink, PFN_CALLBACK pfn, int iID ) : m_pLink( pLink ), m_pfn( pfn ), m_iID( iID ) {}
	virtual void Call( const Params& kParams )
	{
		if ( m_pLink && m_pLink->m_pOwner != NULL && m_pfn != NULL )
			m_pfn( m_pLink->m_pOwner, m_iID, kParams );
	}
private:
	SF::Ptr< CGfxOwnerLink >	m_pLink;
	PFN_CALLBACK					m_pfn;
	int								m_iID;
};
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

class CLIKDataBindingAdapter : public SFG::ExternalInterface
{
public:
	virtual void Callback( SFG::Movie* pmovieView, const char* methodName, const SFG::Value* args, unsigned argCount )
	{
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
		// 31d20430: "dataBindingInit" is answered by the wrapper ( June does not check the argument count or type )
		if ( methodName != NULL && strcmp( methodName, "dataBindingInit" ) == 0 )
		{
			if ( argCount >= 1 && pmovieView != NULL && ( args[0].IsObject() || args[0].IsDisplayObject() ) )
			{
				SFG::Value kObject( args[0] );
				AttachBindingFunction( pmovieView, kObject, "createBindingImpl", 0 );
				AttachBindingFunction( pmovieView, kObject, "deleteBindingImpl", 1 );
			}
			return;
		}
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
		for ( std::map< std::string, IGfxListener* >::iterator it = m_mapListener.begin(); it != m_mapListener.end(); ++it )
		{
			if ( it->second != NULL && it->second->OnExternalCall( pmovieView, methodName, args, argCount ) == true )
				break;
		}
	}
	void AddListener( IGfxListener* pListener )
	{
		if ( pListener == NULL )
			return;
		m_mapListener[ pListener->GetListenerName() ] = pListener;
	}
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	CLIKDataBindingAdapter() : m_pOwnerLink( *new CGfxOwnerLink( this ) ), m_pMovie( NULL ) {}
#else SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	CLIKDataBindingAdapter() : m_pOwnerLink( *new CGfxOwnerLink( this ) ) {}
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	~CLIKDataBindingAdapter() { m_pOwnerLink->m_pOwner = NULL; }

#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	// June's adapter +0x10: the movie the notify hands every value object's GetValue ( 31d1ee10 -> 31d1ec40 ). Set by CKTDGGFXUI::Load, cleared with it.
	void SetMovie( SFG::Movie* pMovie ) { m_pMovie = pMovie; }

	// 31fc75e0: the playSimpleWidget value object's four fields ( id +8, fileName +0xc, parameter1 +0x28, parameter2 +0x2c )
	void SetBindingWidget( const std::string& strName, int iWidgetID, const char* szFileName, int iParameter1, int iParameter2 )
	{
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( strName );
		if ( it == m_mapBinding.end() )
			return;
		it->second.m_iWidgetID		= iWidgetID;
		it->second.m_strFileName	= ( szFileName != NULL ) ? szFileName : "";
		it->second.m_iParameter1	= iParameter1;
		it->second.m_iParameter2	= iParameter2;
	}
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN

	void RemoveListener( const std::string& strName )		// 31d1f550
	{
		m_mapListener.erase( strName );
	}

	void AddBinding( const std::string& strName, int iKind )	// 31d1f900: the table takes a fresh record for the name
	{
		KGfxBinding kBinding;
		kBinding.m_iKind = iKind;
		m_mapBinding[ strName ] = kBinding;
	}
	void RemoveBinding( const std::string& strName )		// 31d1f600
	{
		m_mapBinding.erase( strName );
	}
	bool HasBinding( const std::string& strName ) const
	{
		return m_mapBinding.find( strName ) != m_mapBinding.end();
	}
	void SetBindingBool( const std::string& strName, bool bValue )		// the bool value object's +8 ( 32e59900 @32e59a8a )
	{
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( strName );
		if ( it != m_mapBinding.end() )
			it->second.m_bValue = bValue;
	}
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	void SetBindingResultInfo( const std::string& strName, const KGfxResultInfo& kInfo )	// 32eeae30: the 0x88 B value object
	{
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( strName );
		if ( it != m_mapBinding.end() )
			it->second.m_kResultInfo = kInfo;
	}
	void SetBindingResultItem( const std::string& strName, const KGfxResultItem& kItem )	// 32eea6a0: the 0x40 B value object
	{
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( strName );
		if ( it != m_mapBinding.end() )
			it->second.m_kResultItem = kItem;
	}
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
	void ClearBindings()
	{
		m_mapBinding.clear();
	}
	// 31d1ee10 + 31d1ec40. June walks the live vector; a copy is walked here so an AS callback that deletes its binding cannot invalidate it.
	void NotifyBinding( const std::string& strName )
	{
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( strName );
		if ( it == m_mapBinding.end() )
			return;
		SFG::Value kValue;
		if ( it->second.m_iKind == JGBK_BOOL )
			kValue = SFG::Value( it->second.m_bValue );
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
		else if ( it->second.m_iKind == JGBK_SIMPLE_WIDGET )
			BuildWidgetValue( it->second, &kValue );
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
		else if ( it->second.m_iKind == JGBK_RESULT_INFO )
			BuildResultInfoValue( it->second.m_kResultInfo, &kValue );
		else if ( it->second.m_iKind == JGBK_RESULT_ITEM )
			BuildResultItemValue( it->second.m_kResultItem, &kValue );
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
		else
			kValue.SetNull();
		std::vector< KGfxBindingObserver > vecObserver = it->second.m_vecObserver;
		for ( size_t i = 0; i < vecObserver.size(); ++i )
		{
			if ( vecObserver[i].m_kClosure.IsClosure() )
				vecObserver[i].m_kClosure.InvokeSelf( NULL, &kValue, 1 );
		}
	}
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
private:
	std::map< std::string, IGfxListener* >	m_mapListener;	// +0x18
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
	std::map< std::string, KGfxBinding >	m_mapBinding;	// +0xc
	SF::Ptr< CGfxOwnerLink >				m_pOwnerLink;
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	SFG::Movie*									m_pMovie;		// +0x10 ( not owned: the wrapper clears it before its movie goes )

	// GetValue 31fc7720( movie, out ): only with a movie ( June's "if param_2 != 0" ): a new object with widgetID, fileName, parameter1, parameter2
	void BuildWidgetValue( const KGfxBinding& kBinding, SFG::Value* pValue )
	{
		if ( m_pMovie == NULL )
			return;
		m_pMovie->CreateObject( pValue );
		// G4 review: June sets the three numbers as VT_UInt ( type 4 @31fc776b, the same for parameter1 / parameter2 ) and fileName as VT_String ( 6 )
		pValue->SetMember( "widgetID",		SFG::Value( (SF::UInt32)kBinding.m_iWidgetID ) );
		pValue->SetMember( "fileName",		SFG::Value( kBinding.m_strFileName.c_str() ) );
		pValue->SetMember( "parameter1",	SFG::Value( (SF::UInt32)kBinding.m_iParameter1 ) );
		pValue->SetMember( "parameter2",	SFG::Value( (SF::UInt32)kBinding.m_iParameter2 ) );
	}
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	// GetValue 32eeb1d0 ( DW_DUNGEON_RESULT_VALUE ): only with a movie. Types as June sets them: UInt 4, Number 5, Boolean 2, StringW 7.
	void BuildResultInfoValue( const KGfxResultInfo& kInfo, SFG::Value* pValue )
	{
		if ( m_pMovie == NULL )
			return;
		m_pMovie->CreateObject( pValue );
		pValue->SetMember( "totalRank",			SFG::Value( (SF::UInt32)kInfo.m_iTotalRank ) );
		pValue->SetMember( "sec",				SFG::Value( (SF::UInt32)kInfo.m_iSec ) );
		pValue->SetMember( "ED",				SFG::Value( (SF::UInt32)kInfo.m_iED ) );
		pValue->SetMember( "hitRank",			SFG::Value( (SF::UInt32)kInfo.m_iHitRank ) );
		pValue->SetMember( "comboRank",			SFG::Value( (SF::UInt32)kInfo.m_iComboRank ) );
		pValue->SetMember( "technicalRank",		SFG::Value( (SF::UInt32)kInfo.m_iTechnicalRank ) );
		pValue->SetMember( "beAttackedRank",	SFG::Value( (SF::UInt32)kInfo.m_iBeAttackedRank ) );
		pValue->SetMember( "totalEXP",			SFG::Value( (SF::UInt32)kInfo.m_iTotalEXP ) );
		pValue->SetMember( "expRate",			SFG::Value( (SF::Double)kInfo.m_fExpRate ) );
		pValue->SetMember( "oldExpRate",		SFG::Value( (SF::Double)kInfo.m_fOldExpRate ) );
		pValue->SetMember( "levelUp",			SFG::Value( kInfo.m_bLevelUp ) );
		pValue->SetMember( "level",				SFG::Value( (SF::Double)kInfo.m_iLevel ) );
		pValue->SetMember( "bExactFit",			SFG::Value( g_pKTDXApp->GetResolutionScaleY() < 1.0f ) );	// 32eeae30: +0x1bc < 1.0, set when the value is filled
		pValue->SetMember( "fSizeX",			SFG::Value( (SF::Double)(float)g_pKTDXApp->GetViewport().Width ) );	// 32eeae30: ( float ) app +0x1a4
		pValue->SetMember( "resonanceLevel",	SFG::Value( (SF::Double)kInfo.m_iResonanceLevel ) );
		pValue->SetMember( "showExitButton",	SFG::Value( kInfo.m_bShowExitButton ) );
		pValue->SetMember( "unitType",			SFG::Value( (SF::UInt32)kInfo.m_iUnitType ) );
		pValue->SetMember( "bestSec",			SFG::Value( (SF::UInt32)kInfo.m_iBestSec ) );

		// 32eeb1d0 @32eeba5f..: category_<rank><category> and its four siblings, ranks in the list's order
		for ( size_t i = 0; i < kInfo.m_vecCategory.size(); ++i )
		{
			const std::vector< KGfxResultEntry >& vecEntry = kInfo.m_vecCategory[ i ];
			for ( size_t j = 0; j < vecEntry.size(); ++j )
			{
				const KGfxResultEntry& kEntry = vecEntry[ j ];
				char szName[ 260 ] = { 0, };
				sprintf_s( szName, sizeof( szName ), "category_%d%d", (int)j, (int)i );
				pValue->SetMember( szName, SFG::Value( (SF::UInt32)kEntry.m_iCategory ) );
				sprintf_s( szName, sizeof( szName ), "userName_%d%d", (int)j, (int)i );
				pValue->SetMember( szName, SFG::Value( kEntry.m_wstrName.c_str() ) );
				sprintf_s( szName, sizeof( szName ), "detailValue_%d%d", (int)j, (int)i );
				pValue->SetMember( szName, SFG::Value( (SF::Double)kEntry.m_fDetailValue ) );
				sprintf_s( szName, sizeof( szName ), "IsMyUnit_%d%d", (int)j, (int)i );
				pValue->SetMember( szName, SFG::Value( kEntry.m_bIsMyUnit ) );
				sprintf_s( szName, sizeof( szName ), "unitType_%d%d", (int)j, (int)i );
				pValue->SetMember( szName, SFG::Value( (SF::UInt32)kEntry.m_iUnitType ) );
			}
		}

		pValue->SetMember( "playerCount",		SFG::Value( (SF::UInt32)kInfo.m_iPlayerCount ) );
		pValue->SetMember( "rewardType",		SFG::Value( (SF::UInt32)kInfo.m_iRewardType ) );
		pValue->SetMember( "expBonusStringSize",SFG::Value( (SF::UInt32)kInfo.m_vecExpBonusString.size() ) );
		for ( size_t i = 0; i < kInfo.m_vecExpBonusString.size(); ++i )
		{
			char szName[ 260 ] = { 0, };
			sprintf_s( szName, sizeof( szName ), "expBonusString_%d", (int)i );
			pValue->SetMember( szName, SFG::Value( kInfo.m_vecExpBonusString[ i ].c_str() ) );
		}
		pValue->SetMember( "edBonusStringSize",	SFG::Value( (SF::UInt32)kInfo.m_vecEdBonusString.size() ) );
		for ( size_t i = 0; i < kInfo.m_vecEdBonusString.size(); ++i )
		{
			char szName[ 260 ] = { 0, };
			sprintf_s( szName, sizeof( szName ), "edBonusString_%d", (int)i );
			pValue->SetMember( szName, SFG::Value( kInfo.m_vecEdBonusString[ i ].c_str() ) );
		}
		pValue->SetMember( "reTryButtonShow",	SFG::Value( kInfo.m_bReTryButtonShow ) );
	}

	// GetValue 32eeaff0: { itemID, iconPath, iNum }
	void BuildResultItemValue( const KGfxResultItem& kItem, SFG::Value* pValue )
	{
		if ( m_pMovie == NULL )
			return;
		m_pMovie->CreateObject( pValue );
		pValue->SetMember( "itemID",	SFG::Value( (SF::UInt32)kItem.m_iItemID ) );
		pValue->SetMember( "iconPath",	SFG::Value( kItem.m_wstrIconPath.c_str() ) );
		pValue->SetMember( "iNum",		SFG::Value( (SF::UInt32)kItem.m_iNum ) );
	}
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET

	// the table 3344de9c: ( name, native function ) pairs, set on the object dataBindingInit hands over
	void AttachBindingFunction( SFG::Movie* pMovie, SFG::Value& kObject, const char* szName, int iID )
	{
		SF::Ptr< CGfxFunctionHandler > pHandler = *new CGfxFunctionHandler( m_pOwnerLink.GetPtr(), &CLIKDataBindingAdapter::OnBindingFunction, iID );
		SFG::Value kFunction;
		pMovie->CreateFunction( &kFunction, pHandler );
		kObject.SetMember( szName, kFunction );
	}

	static void OnBindingFunction( void* pOwner, int iID, const SFG::FunctionHandler::Params& kParams )
	{
		CLIKDataBindingAdapter* pAdapter = static_cast< CLIKDataBindingAdapter* >( pOwner );
		if ( iID == 0 )
			pAdapter->CreateBindingImpl( kParams );
		else
			pAdapter->DeleteBindingImpl( kParams );
	}

	// 31d1f0a0: args ( name, a, b ). June reads the three Values without a count or type test.
	void CreateBindingImpl( const SFG::FunctionHandler::Params& kParams )
	{
		if ( kParams.ArgCount < 3 || kParams.pArgs == NULL || kParams.pArgs[0].IsString() == false )
			return;
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( kParams.pArgs[0].GetString() );
		if ( it == m_mapBinding.end() )
			return;
		KGfxBindingObserver kObserver;
		kObserver.m_kClosure	= kParams.pArgs[1];
		kObserver.m_kExtra		= kParams.pArgs[2];
		it->second.m_vecObserver.push_back( kObserver );
		// 31d1ed90: the listeners are told the name; the scene's ( 32e597d0 ) and the manager's answer false, so nothing follows from it
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
		const std::string strBindingName = kParams.pArgs[0].GetString();
		for ( std::map< std::string, IGfxListener* >::iterator itL = m_mapListener.begin(); itL != m_mapListener.end(); ++itL )
		{
			if ( itL->second != NULL && itL->second->OnBindingCreated( strBindingName ) == true )
				break;
		}
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
	}

	// 31d1f3a0 / 31d1f230: the first observer whose two Values equal ( a, b ) goes
	void DeleteBindingImpl( const SFG::FunctionHandler::Params& kParams )
	{
		if ( kParams.ArgCount < 3 || kParams.pArgs == NULL || kParams.pArgs[0].IsString() == false )
			return;
		std::map< std::string, KGfxBinding >::iterator it = m_mapBinding.find( kParams.pArgs[0].GetString() );
		if ( it == m_mapBinding.end() )
			return;
		std::vector< KGfxBindingObserver >& vecObserver = it->second.m_vecObserver;
		for ( size_t i = 0; i < vecObserver.size(); ++i )
		{
			if ( vecObserver[i].m_kExtra == kParams.pArgs[2] && vecObserver[i].m_kClosure == kParams.pArgs[1] )
			{
				vecObserver.erase( vecObserver.begin() + i );
				break;
			}
		}
	}
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
};

// ---------------------------------------------------------------------------------------------------------------------------------------
// CKTDGGFXUI ( RTTI .?AVCKTDGGFXUI@@, 0x98 B, ctor 31c40220, vtable 33443a64 ): the root movie wrapper at mgr +0x98.
// Left out, for Stage 9: the "CharacterView" render-target image ( +0x84 / +0x88 / +0x8c..+0x90 1024 x 1024, +0x8 name map, 31c40030 /
// 31c3ff40 / 31c3f6c0 ) that the root UI's character panel draws into.
// ---------------------------------------------------------------------------------------------------------------------------------------
class CKTDGGFXUI
{
public:
	CKTDGGFXUI( SFG::Loader* pLoader, IGfxListener* pListener )
		: m_pLoader( pLoader ), m_pListener( pListener ), m_iBufW( 0 ), m_iBufH( 0 ), m_iLeft( 0 ), m_iTop( 0 ), m_iWidth( 0 ), m_iHeight( 0 )
	{
	}
	~CKTDGGFXUI()
	{
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
		if ( m_pAdapter )
			m_pAdapter->ClearBindings();	// the observers' closure Values go before their movie
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
		if ( m_pAdapter )
			m_pAdapter->SetMovie( NULL );
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
		m_RootValue.SetUndefined();		// a managed Value goes before its movie
		m_hDisplay.Clear();
		m_pMovie = NULL;
	}

	// 31c3f3d0
	bool Load( const char* szFile )
	{
		if ( m_pLoader == NULL )
			return false;
		SF::Ptr< SFG::MovieDef > pDef = *m_pLoader->CreateMovie( szFile, SFG::Loader::LoadAll, 0 );		// mgr vslot 1 31c43170
		if ( !pDef )
			return false;
		m_pMovie = *pDef->CreateInstance( SFG::MemoryParams(), true, NULL );		// June's MemoryParams are the defaults ( 0.25 / ~0 / ... )
		if ( !m_pMovie )
			return false;
		m_pMovie->SetMouseCursorCount( 1 );								// 4.0 vslot 0xa4
		m_pMovie->SetControllerCount( 1 );								// 4.0 vslot 0xac
		m_pMovie->Advance( 0.f, 0, true );								// 4.0 vslot 0x5c
		m_pMovie->SetBackgroundAlpha( 0.f );							// 4.0 vslot 0x7c
		m_pMovie->SetViewScaleMode( SFG::Movie::SM_ExactFit );			// ( 2 )
		m_pMovie->SetViewAlignment( SFG::Movie::Align_TopLeft );		// ( 5 )
		m_pMovie->HandleEvent( SFG::SetFocusEvent() );					// event 8
		m_pAdapter = *new CLIKDataBindingAdapter();						// 31c3f340 -> +0x60
		m_pMovie->SetExternalInterface( m_pAdapter );
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
		if ( m_pAdapter )
			m_pAdapter->SetMovie( m_pMovie.GetPtr() );
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
		if ( m_pAdapter && m_pListener != NULL )
			m_pAdapter->AddListener( m_pListener );
		return true;
	}

	// 31c3f060
	void SetViewport( int iBufW, int iBufH, int iLeft, int iTop, int iWidth, int iHeight )
	{
		if ( m_pMovie )
			m_pMovie->SetViewport( SFG::Viewport( iBufW, iBufH, iLeft, iTop, iWidth, iHeight ) );
		m_iBufW = iBufW;	m_iBufH = iBufH;	m_iLeft = iLeft;	m_iTop = iTop;	m_iWidth = iWidth;	m_iHeight = iHeight;
	}

	// 31c3faf0
	void Advance( float fElapsed, unsigned uCatchUp )
	{
		if ( !m_pMovie )
			return;
		m_pMovie->SetBackgroundAlpha( 0.f );
		m_pMovie->SetViewport( SFG::Viewport( m_iBufW, m_iBufH, m_iLeft, m_iTop, m_iWidth, m_iHeight ) );
		m_pMovie->Advance( fElapsed, uCatchUp, true );
		m_hDisplay = m_pMovie->GetDisplayHandle();
	}

	// 31c3f6e0
	void Display( SFR::Renderer2D* pRenderer )
	{
		SFG::MovieDisplayHandle hDisplay = m_pMovie->GetDisplayHandle();
		if ( hDisplay.NextCapture( pRenderer->GetContextNotify() ) == true )
			pRenderer->Display( hDisplay.GetRenderEntry() );
	}

	unsigned HandleEvent( const SFG::Event& kEvent )		// 31c3f130
	{
		if ( !m_pMovie )
			return 0;
		return m_pMovie->HandleEvent( kEvent );
	}

	bool HitTest( float x, float y )						// 31c3f100
	{
		if ( !m_pMovie )
			return false;
		return m_pMovie->HitTest( x, y, SFG::Movie::HitTest_ShapesNoInvisible, 0 );
	}

	// 31c3f150: only when the root Value is an object / array / display object
	void Invoke( const char* szMethod, SFG::Value* pResult, const SFG::Value* pArgs, unsigned uCount )
	{
		if ( m_RootValue.IsObject() || m_RootValue.IsArray() || m_RootValue.IsDisplayObject() )
			m_RootValue.Invoke( szMethod, pResult, pArgs, uCount );
	}

	void SetRootValue( const SFG::Value& kValue ) { m_RootValue = kValue; }		// 31c3f650 ( AS RegisterMainUI )
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
	CLIKDataBindingAdapter* GetAdapter() const { return m_pAdapter.GetPtr(); }			// vslot 1 of the wrapper ( 3226aed0: mgr +0x98 vslot 1 )
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

private:
	SFG::Loader*						m_pLoader;			// +0x40 ( June keeps the manager; the loader is its +8 )
	IGfxListener*					m_pListener;		// +0x5c ( mgr +4 )
	SF::Ptr< SFG::Movie >				m_pMovie;			// +0x3c
	SFG::MovieDisplayHandle				m_hDisplay;			// +0x38
	int									m_iBufW, m_iBufH, m_iLeft, m_iTop, m_iWidth, m_iHeight;	// +0x44..+0x58
	SF::Ptr< CLIKDataBindingAdapter >	m_pAdapter;			// +0x60
	SFG::Value							m_RootValue;		// +0x68..+0x70
};

#ifdef SERV_IRUHADEV_JUNE_GFX_BLEND
// June's D3D9::HAL::applyBlendModeImpl ( 33214cd0 ): two tables, Erase / Alpha keep the destination
class GfxHAL : public SFR::D3D9::HAL
{
public:
	GfxHAL() : SFR::D3D9::HAL( NULL ) {}

	virtual void applyBlendModeImpl( SFR::BlendMode mode, bool sourceAc = false, bool forceAc = false )
	{
		if ( (unsigned)mode > 14 )		// Overwrite / OverwriteAll / FullAdditive: 4.2-internal, no June row ( June clamps to 0 )
		{
			SFR::D3D9::HAL::applyBlendModeImpl( mode, sourceAc, forceAc );
			return;
		}
		if ( pDevice == NULL )			// [this+0x4c0] == 0 -> return
			return;

		// the profiler override ( this+0x2b0 ) is applied by the base applyBlendMode before this call
		const unsigned uMode = (unsigned)mode;
		FillFlags &= ~( SFR::FF_Multiply | SFR::FF_Invert );		// this+0x4c4 bit 0 = FF_Multiply: set for 3 ( Multiply ) and 6 ( Darken ); June never sets Invert
		if ( uMode == 3 || uMode == 6 )
			FillFlags |= SFR::FF_Multiply;

		// T20 = 337493f0, stride 0x14: op, src, dst, srcAlpha, dstAlpha. T12 = 33749520, stride 0xc: op, src ( same as T20 ), dst. D3D enum values
		static const struct { DWORD dwOp, dwSrc, dwDst, dwSrcA, dwDstA; } s_aT20[15] =
		{
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		//  0 None
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		//  1 Normal
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		//  2 Layer
			{ D3DBLENDOP_ADD,         D3DBLEND_DESTCOLOR, D3DBLEND_ZERO,        D3DBLEND_DESTCOLOR, D3DBLEND_ZERO },		//  3 Multiply
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		//  4 Screen
			{ D3DBLENDOP_MAX,         D3DBLEND_SRCALPHA,  D3DBLEND_ONE,         D3DBLEND_SRCALPHA, D3DBLEND_ONE },		//  5 Lighten
			{ D3DBLENDOP_MIN,         D3DBLEND_SRCALPHA,  D3DBLEND_ONE,         D3DBLEND_SRCALPHA, D3DBLEND_ONE },		//  6 Darken
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		//  7 Difference
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_ONE,         D3DBLEND_ZERO, D3DBLEND_ONE },		//  8 Add
			{ D3DBLENDOP_REVSUBTRACT, D3DBLEND_SRCALPHA,  D3DBLEND_ONE,         D3DBLEND_ZERO, D3DBLEND_ONE },		//  9 Subtract
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		// 10 Invert
			{ D3DBLENDOP_ADD,         D3DBLEND_ZERO,      D3DBLEND_ZERO,        D3DBLEND_ONE, D3DBLEND_ONE },		// 11 Alpha
			{ D3DBLENDOP_ADD,         D3DBLEND_ZERO,      D3DBLEND_ZERO,        D3DBLEND_ONE, D3DBLEND_ONE },		// 12 Erase
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		// 13 Overlay
			{ D3DBLENDOP_ADD,         D3DBLEND_SRCALPHA,  D3DBLEND_INVSRCALPHA, D3DBLEND_ONE,  D3DBLEND_INVSRCALPHA },		// 14 Hardlight
		};
		static const struct { DWORD dwOp, dwDst; } s_aT12[15] =
		{
			{ D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA }, { D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA }, { D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA },
			{ D3DBLENDOP_ADD, D3DBLEND_ZERO },																		//  3 Multiply
			{ D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA },
			{ D3DBLENDOP_MAX, D3DBLEND_ONE }, { D3DBLENDOP_MIN, D3DBLEND_ONE },										//  5 Lighten, 6 Darken
			{ D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA },
			{ D3DBLENDOP_ADD, D3DBLEND_ONE }, { D3DBLENDOP_REVSUBTRACT, D3DBLEND_ONE },								//  8 Add, 9 Subtract
			{ D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA },
			{ D3DBLENDOP_ADD, D3DBLEND_ONE }, { D3DBLENDOP_ADD, D3DBLEND_ONE },										// 11 Alpha, 12 Erase: destination kept
			{ D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA }, { D3DBLENDOP_ADD, D3DBLEND_INVSRCALPHA },
		};

		const DWORD dwSrc = ( sourceAc && s_aT20[uMode].dwSrc == D3DBLEND_SRCALPHA ) ? D3DBLEND_ONE : s_aT20[uMode].dwSrc;
		if ( ( VP.Flags & SFR::Viewport::View_AlphaComposite ) == 0 && forceAc == false )		// simple path
		{
			pDevice->SetRenderState( D3DRS_BLENDOP, s_aT12[uMode].dwOp );
			pDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
			pDevice->SetRenderState( D3DRS_SRCBLEND, dwSrc );
			pDevice->SetRenderState( D3DRS_DESTBLEND, s_aT12[uMode].dwDst );
			return;
		}
		pDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_BLENDOP, s_aT20[uMode].dwOp );
		pDevice->SetRenderState( D3DRS_BLENDOPALPHA, s_aT20[uMode].dwOp );
		pDevice->SetRenderState( D3DRS_SRCBLEND, dwSrc );
		pDevice->SetRenderState( D3DRS_DESTBLEND, s_aT20[uMode].dwDst );
		pDevice->SetRenderState( D3DRS_SRCBLENDALPHA, s_aT20[uMode].dwSrcA );
		pDevice->SetRenderState( D3DRS_DESTBLENDALPHA, s_aT20[uMode].dwDstA );
	}
};
#endif SERV_IRUHADEV_JUNE_GFX_BLEND

// ---------------------------------------------------------------------------------------------------------------------------------------
// The manager's Scaleform-typed half
// ---------------------------------------------------------------------------------------------------------------------------------------
class CKTDGGFXManagerImpl : public IGfxListener
{
public:
	CKTDGGFXManagerImpl( HWND hWnd )
		: m_pUI( NULL ), m_pStateBlock( NULL ), m_dwLastTime( 0 ), m_pMessageStage( NULL ), m_hWnd( hWnd ), m_fElapsed( 0.f ),
		  m_iFontConfig( -1 ), m_bShow( true ), m_sIMECompositionLength( 0 ), m_bIMEComposing( false ), m_bLButtonDownHit( false ),
		  m_bEventScenePlaying( false ), m_iLA( 0 ), m_bMouseOn( false ), m_iIMETextFieldID( 0 ), m_bMainLoaded( false )
	{
		ZeroMemory( &m_kPresentParams, sizeof( m_kPresentParams ) );
		ZeroMemory( &m_kViewport, sizeof( m_kViewport ) );
		// +0x118 = 1 only when g_pMain +0x708 == 1 ( 31aa20c3 ): that byte is June's Axeso5 "_LA" switch, which March does not have, and
		// nothing reads +0x118. Kept at 0.
	}

	~CKTDGGFXManagerImpl()
	{
		// June's dtor 31c45970 never deletes the wrapper at +0x98 ( an exit-time leak, JUNE_BUGS 325 ). Not kept: a live movie would outlive
		// the Scaleform System that KTDGFXRuntime_Shutdown destroys right after this. June's HAL Release ends in ~HAL -> ShutdownHAL, as below.
		SAFE_DELETE( m_pUI );				// the movie and its Values before the loader states, the renderer and the HAL
		SAFE_RELEASE( m_pStateBlock );
		m_pRenderer = NULL;
		if ( m_pHAL && m_pHAL->IsInitialized() )
			m_pHAL->ShutdownHAL();
		m_pHAL = NULL;
		m_pSoundRenderer = NULL;
	}

	// ---- IJuneGfxListener ( 31c48570 / 31c45ed0 ) --------------------------------------------------------------------------------
	virtual std::string GetListenerName() const { return "GFXManager"; }

	virtual bool OnExternalCall( SFG::Movie* /*pMovie*/, const char* szMethod, const SFG::Value* pArgs, unsigned uArgCount )
	{
		if ( szMethod == NULL )
			return false;
		if ( strcmp( szMethod, "MSG" ) == 0 && uArgCount == 1 && pArgs[0].IsInt() )
		{
			// the stage's MsgProc( hwnd, 0x467 = KM_UI_CONTROL_CUSTOM_EVENT, n, 0 )
			if ( m_pMessageStage == NULL )
				return false;
			m_pMessageStage->MsgProc( m_hWnd, CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, (WPARAM)pArgs[0].GetInt(), 0 );
			return true;
		}
		if ( strcmp( szMethod, "isShippingBuild" ) == 0 )
		{
			SetMemberOf( pArgs, uArgCount, "isShipping", SFG::Value( true ) );
		}
		else if ( strcmp( szMethod, "DataStringTable" ) == 0 )
		{
			// June reads args[1]'s data word as the id whatever its type ( a Number would give the double's low dword ): JUNE_BUGS row
			int iID = 0;
			if ( uArgCount >= 2 )
			{
				if ( pArgs[1].IsInt() )
					iID = pArgs[1].GetInt();
				else if ( pArgs[1].IsUInt() )
					iID = (int)pArgs[1].GetUInt();
				else if ( pArgs[1].IsNumber() )
				{
					double dNumber = pArgs[1].GetNumber();
					memcpy( &iID, &dNumber, sizeof( iID ) );
				}
			}
			std::wstring wstrString = GET_STRING( iID );
			SetMemberOf( pArgs, uArgCount, "string", SFG::Value( wstrString.c_str() ) );
		}
		else if ( strcmp( szMethod, "RegisterMainUI" ) == 0 && uArgCount == 1 )
		{
			if ( m_pUI != NULL )
				m_pUI->SetRootValue( pArgs[0] );
		}
		else if ( strcmp( szMethod, "NowCounTry" ) == 0 && uArgCount == 1 )
		{
			SetMemberOf( pArgs, uArgCount, "NowClientCountry", SFG::Value( (SF::SInt32)4 ) );		// June's literal
		}
		else if ( strcmp( szMethod, "NowServiceMode" ) == 0 && uArgCount == 1 )
		{
			SetMemberOf( pArgs, uArgCount, "NowClientMode", SFG::Value( (SF::SInt32)3 ) );			// June's literal
		}
		return false;
	}

	// June calls the object's SetMember with no type test ( a non-object would fault there ); the test here only stops that crash
	static void SetMemberOf( const SFG::Value* pArgs, unsigned uArgCount, const char* szMember, const SFG::Value& kValue )
	{
		if ( uArgCount < 1 )
			return;
		SFG::Value kObject( pArgs[0] );
		if ( kObject.IsObject() || kObject.IsArray() || kObject.IsDisplayObject() )
			kObject.SetMember( szMember, kValue );
	}

	// ---- loader states ( 31c451d0 ) -----------------------------------------------------------------------------------------------
	void InitLoaderStates()
	{
		m_Loader.SetLog( SF::Ptr< SF::Log >( *new GFxPlayerLog() ) );
		m_Loader.SetFileOpener( SF::Ptr< SFG::FileOpener >( *new r3dGFXFileOpener() ) );
		m_Loader.SetTextClipboard( SF::Ptr< SFG::TextClipboard >( *new FxPlayerTextClipboard() ) );

#ifdef SERV_IRUHADEV_JUNE_GFX_BLEND
		m_pHAL = *SF_NEW GfxHAL();									// 3321a190( 0 ) -> +0x3c, June's applyBlendMode tables
		GfxLog( "blend: June applyBlendMode tables" );
#else
		m_pHAL = *SF_NEW SFR::D3D9::HAL( NULL );						// 3321a190( 0 ) -> +0x3c
#endif SERV_IRUHADEV_JUNE_GFX_BLEND
		m_pRenderer = *SF_NEW SFR::Renderer2D( m_pHAL );				// 31af9560( HAL ) -> +0x40
		if ( !m_pRenderer )
		{
			GfxLog( "Renderer2D not made: no GFx states set" );		// June returns 0 here ( Init ignores it )
			return;
		}

		m_Loader.SetAS2Support( SF::Ptr< SFG::ASSupport >( *new SFG::AS2Support() ) );		// 0x21
		m_Loader.SetAS3Support( SF::Ptr< SFG::ASSupport >( *new SFG::AS3Support() ) );		// 0x22

		// four handler instances ( 337458bc / 33723454 / 337458b8 / 337458b4, no RTTI ): taken as the FxPlayer set, JPEG / PNG / TGA / DDS
		SF::Ptr< SFG::ImageFileHandlerRegistry > pImageRegistry = *new SFG::ImageFileHandlerRegistry();
		pImageRegistry->AddHandler( &SFR::JPEG::FileReader::Instance );
		pImageRegistry->AddHandler( &SFR::PNG::FileReader::Instance );		// fails cleanly under the client's libpng 1.2.24 ( G1 note )
		pImageRegistry->AddHandler( &SFR::TGA::FileReader::Instance );
		pImageRegistry->AddHandler( &SFR::DDS::FileReader::Instance );
		m_Loader.SetImageFileHandlerRegistry( pImageRegistry );

		m_Loader.SetFontProvider( SF::Ptr< SFG::FontProviderWin32 >( *new SFG::FontProviderWin32( ::GetDC( NULL ) ) ) );		// 0x11

		// IME manager ( 33030d00, 0x70 B, ime.xml / IME.swf, state 0x17 ): the SDK has SF_ENABLE_IME off and no Win32 IME source, and
		// neither file is in June's data. Not made; the IME events below still go to the movie as June sends them.
		GfxLog( "IME manager skipped ( SDK without IME support; no ime.xml / IME.swf in June's data )" );

		InitSound();

		GfxConfigParser kParser( "fontconfig.font" );
		m_kFontConfigs.Parse( kParser );
		m_iFontConfig = m_kFontConfigs.m_vecConfigs.empty() ? -1 : 0;		// 31c43d00
		if ( kParser.HasData() == false )
			GfxLog( "fontconfig.font not found in the koms ( data036 ): no font config" );
		FontConfig* pConfig = ( m_iFontConfig != -1 ) ? PickFontConfig() : NULL;
		if ( pConfig != NULL )
			pConfig->Apply( m_Loader );

		// a 0x50 B object ( 31b01930 ) handed to the AMP server singleton ( 31abe360 ) vslot 0x30, which is "return false" ( 31e4c550 ):
		// June runs without AMP. Nothing to port.
	}

	// 31c43700
	void InitSound()
	{
		if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDSManager() == NULL || g_pKTDXApp->GetDSManager()->GetSystem() == NULL )
		{
			GfxLog( "sound: no FMOD system on KTDS, GFx sound off" );
			return;
		}
		m_pSoundRenderer = *SF::Sound::SoundRendererFMOD::CreateSoundRenderer();		// 33221b20 -> +0x110
		if ( !m_pSoundRenderer )
			return;
		// the client's FMOD Ex C handle is the same object as the C++ FMOD::System
		FMOD::System* pSystem = reinterpret_cast< FMOD::System* >( g_pKTDXApp->GetDSManager()->GetSystem() );
		if ( m_pSoundRenderer->Initialize( pSystem, true, true ) == false )
		{
			GfxLog( "sound: SoundRendererFMOD::Initialize failed, GFx sound off" );
			return;
		}
		m_Loader.SetAudio( SF::Ptr< SFG::Audio >( *new SFG::Audio( m_pSoundRenderer, 0.1f, 15, SFG::AudioBase::AudioMaster ) ) );	// 0x1c
		m_pSoundRenderer->SetMasterVolume( 100.f );		// vslot 0x1c ( 334355b8 )
		m_pSoundRenderer->Mute( false );				// vslot 0x24
		GfxLog( "sound: FMOD renderer up on KTDS's FMOD system" );
	}

	// 31c44ff0: the first config whose fontlib files are all kom members ( 31c4bcb0 ), else the next one round; NULL when none has them
	FontConfig* PickFontConfig()
	{
		const int iStart = m_iFontConfig;
		if ( iStart == -1 )
			return NULL;
		for ( ;; )
		{
			FontConfig* pConfig = m_kFontConfigs.m_vecConfigs[ m_iFontConfig ];
			size_t i = 0;
			for ( ; i < pConfig->m_vecFontLibFiles.size(); ++i )
			{
				if ( g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile_MassFileOnly(
						GfxFromUTF8( pConfig->m_vecFontLibFiles[i].c_str() ) ) == false )
					break;
			}
			if ( i == pConfig->m_vecFontLibFiles.size() )
				return pConfig;
			GfxLog( "Fontlib file '%s' cannot be found. Skipping config '%s'..", pConfig->m_vecFontLibFiles[i].c_str(),
				GfxToUTF8( pConfig->m_wstrName.c_str() ).c_str() );
			m_iFontConfig = ( m_iFontConfig + 1 ) % (int)m_kFontConfigs.m_vecConfigs.size();
			if ( m_iFontConfig == iStart )
				return NULL;
		}
	}

	// ---- Init ( 31c456e0 ) --------------------------------------------------------------------------------------------------------
	void Init( IDirect3DDevice9* pDevice )
	{
		InitLoaderStates();
		SAFE_DELETE( m_pUI );
		m_pUI = new CKTDGGFXUI( &m_Loader, this );		// 31c465b0
		if ( m_pUI->Load( "main.gfx" ) == false )
		{
			SAFE_DELETE( m_pUI );
			GfxLog( "main.gfx FAILED to load ( data036 ): no GFx movie" );
			return;
		}
		D3DVIEWPORT9 kViewport;
		if ( pDevice != NULL && SUCCEEDED( pDevice->GetViewport( &kViewport ) ) )
		{
			m_pUI->SetViewport( kViewport.Width, kViewport.Height, 0, 0, kViewport.Width, kViewport.Height );
			m_kViewport = kViewport;			// +0xbc
		}
		m_dwLastTime = timeGetTime();			// +0x7c
		m_bMainLoaded = true;

		FontConfig* pConfig = ( m_iFontConfig != -1 && m_iFontConfig < (int)m_kFontConfigs.m_vecConfigs.size() )
			? (FontConfig*)m_kFontConfigs.m_vecConfigs[ m_iFontConfig ] : NULL;
		if ( pConfig != NULL )
		{
			GfxLog( "main.gfx loaded, font config \"%s\" ( %d fontlib%s%s, %d map, %d tr )", GfxToUTF8( pConfig->m_wstrName.c_str() ).c_str(),
				(int)pConfig->m_vecFontLibFiles.size(), pConfig->m_vecFontLibFiles.empty() ? "" : " ",
				pConfig->m_vecFontLibFiles.empty() ? "" : pConfig->m_vecFontLibFiles[0].c_str(), pConfig->m_iMapCount,
				(int)pConfig->m_pTranslator->GetCount() );
		}
		else
		{
			GfxLog( "main.gfx loaded, no font config applied" );
		}
	}

	// ---- per frame / device -------------------------------------------------------------------------------------------------------
	void OnFrameMove()		// 31c430e0
	{
		DWORD dwNow = timeGetTime();
		m_fElapsed = (float)( dwNow - m_dwLastTime ) / 1000.f;
		m_dwLastTime = dwNow;
	}

	void OnFrameRender()	// 31c43b20
	{
		if ( m_pUI != NULL )
			m_pUI->Advance( m_fElapsed, 2 );
		if ( m_pStateBlock == NULL )
			return;
		m_pStateBlock->Capture();
		if ( m_pRenderer )
		{
			m_pRenderer->BeginFrame();
			if ( m_pUI != NULL )
				m_pUI->Display( m_pRenderer );
			m_pRenderer->EndFrame();
		}
		m_pStateBlock->Apply();
	}

	void OnLostDevice()		// 31c43440
	{
		SAFE_RELEASE( m_pStateBlock );
		if ( m_pHAL && m_pHAL->IsInitialized() )
			m_pHAL->PrepareForReset();
	}

	void OnResetDevice( IDirect3DDevice9* pDevice )		// 31c43e60
	{
		if ( pDevice != NULL )
			pDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock );
		if ( !m_pHAL )
			return;
		if ( m_pHAL->IsInitialized() == false )
		{
			// the first call is DXUT's first reset after OnCreateDevice ( Init ran before it )
			SFR::D3D9::HALInitParams kParams( pDevice, m_kPresentParams, SFR::D3D9::HALConfig_NoSceneCalls );
			kParams.RenderQueueSize = 0x100;
			if ( m_pHAL->InitHAL( kParams ) == false )
				GfxLog( "D3D9 HAL InitHAL failed: the movie will not draw" );
			// wrapper +0x80 = HAL vslot 0xa8 ( the texture manager ) and 31c40030( device ) feed the Stage 9 character view
		}
		else
		{
			m_pHAL->RestoreAfterReset();
		}
		if ( m_pUI != NULL && pDevice != NULL )
		{
			D3DVIEWPORT9 kViewport;
			if ( SUCCEEDED( pDevice->GetViewport( &kViewport ) ) )
			{
				m_pUI->SetViewport( kViewport.Width, kViewport.Height, 0, 0, kViewport.Width, kViewport.Height );
				m_kViewport = kViewport;
			}
		}
	}

	void OnResize( const D3DPRESENT_PARAMETERS& kPresentParams ) { m_kPresentParams = kPresentParams; }		// 31c430c0

	// ---- input --------------------------------------------------------------------------------------------------------------------
	// 31c435b0: IME messages. Composition state ( +0x10a / +0x10c ) and the 0x46c forward to the stage ( March: KM_GAME_BASE, which no
	// March stage handles ), then the movie's IME event; true when the movie answers HE_NoDefaultAction.
	bool OnIMEEvent( UINT uMsg, WPARAM wParam, LPARAM lParam, HWND hWnd, bool bPreProcess )
	{
		if ( uMsg > WM_IME_COMPOSITION )
		{
			if ( uMsg == WM_IME_NOTIFY && wParam == IMN_SETOPENSTATUS && m_iIMETextFieldID == 3 )
			{
				// 32fe469a / 32fe4694 taken as ImmGetContext / ImmSetConversionStatus by their call shapes [I]; June never releases the
				// context ( JUNE_BUGS row )
				HIMC hImc = ImmGetContext( g_pKTDXApp->GetHWND() );
				if ( hImc != NULL )
					ImmSetConversionStatus( hImc, 0, 0 );
			}
		}
		else if ( uMsg == WM_IME_COMPOSITION )
		{
			if ( m_bIMEComposing == true )
			{
				m_sIMECompositionLength = (short)wParam;
				if ( m_pMessageStage != NULL )
					m_pMessageStage->MsgProc( m_hWnd, CKTDXApp::KM_GAME_BASE, WM_IME_COMPOSITION, wParam );
			}
		}
		else if ( uMsg == WM_IME_STARTCOMPOSITION )
		{
			m_sIMECompositionLength = 0;
			m_bIMEComposing = true;
			if ( m_pMessageStage != NULL )
				m_pMessageStage->MsgProc( m_hWnd, CKTDXApp::KM_GAME_BASE, WM_IME_STARTCOMPOSITION, wParam );
		}
		else if ( uMsg == WM_IME_ENDCOMPOSITION )
		{
			m_bIMEComposing = false;
			m_sIMECompositionLength = 0;
			if ( m_pMessageStage != NULL )
				m_pMessageStage->MsgProc( m_hWnd, CKTDXApp::KM_GAME_BASE, WM_IME_ENDCOMPOSITION, wParam );
		}
		if ( m_pUI == NULL )
			return false;
		SFG::IMEWin32Event kEvent( bPreProcess ? SFG::IMEEvent::IME_PreProcessKeyboard : SFG::IMEEvent::IME_Default,
			(SF::UPInt)hWnd, uMsg, (SF::UPInt)wParam, (SF::UPInt)lParam, bPreProcess ? 0 : 1 );
		return ( m_pUI->HandleEvent( kEvent ) & SFG::Movie::HE_NoDefaultAction ) != 0;
	}

	// 31c43ab0
	void MainLoopFilter( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		BOOL bIMEMessage = ImmIsUIMessageW( NULL, uMsg, wParam, lParam );
		if ( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || bIMEMessage != FALSE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP )
			OnIMEEvent( uMsg, wParam, lParam, hWnd, true );
	}

	// 31c43030
	static unsigned GetModifierBits()
	{
		unsigned u = 0;
		if ( ( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0 )	u = 4;
		if ( ( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0 )	u |= 1;
		if ( ( GetKeyState( VK_MENU ) & 0x8000 ) != 0 )		u |= 2;
		if ( ( GetKeyState( VK_NUMLOCK ) & 1 ) != 0 )		u |= 8;
		if ( ( GetKeyState( VK_CAPITAL ) & 1 ) != 0 )		u |= 0x10;
		if ( ( GetKeyState( VK_SCROLL ) & 1 ) != 0 )		u |= 0x20;
		return u;
	}

	// 31c431c0: KeyDown ( 5 ) / KeyUp ( 6 ); modifier bits renumbered to KeyModifiers
	unsigned SendKeyEvent( unsigned uCode, unsigned char ucAscii, unsigned uMods, bool bDown )
	{
		if ( m_pUI == NULL || uCode == 0 )
			return 0;
		SF::UInt8 ucStates = ( uMods >> 2 ) & 1;					// shift -> Key_ShiftPressed
		if ( ( uMods & 1 ) != 0 )			ucStates |= 2;			// ctrl -> Key_CtrlPressed
		if ( ( ( uMods >> 1 ) & 1 ) != 0 )	ucStates |= 4;			// alt -> Key_AltPressed
		if ( ( ( uMods >> 3 ) & 1 ) != 0 )	ucStates |= 0x10;		// num lock -> Key_NumToggled
		if ( ( ( uMods >> 4 ) & 1 ) != 0 )	ucStates |= 8;			// caps lock -> Key_CapsToggled
		if ( ( ( uMods >> 5 ) & 1 ) != 0 )	ucStates |= 0x20;		// scroll lock -> Key_ScrollToggled
		SFG::KeyEvent kEvent( bDown ? SFG::Event::KeyDown : SFG::Event::KeyUp, (SF::Key::Code)uCode, ucAscii, 0, SF::KeyModifiers( ucStates ) );
		return m_pUI->HandleEvent( kEvent );
	}

	// 31c432e0
	bool OnKeyEvent( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( m_pUI == NULL )
			return false;
		const unsigned uVK = (unsigned)wParam;
		unsigned uCode = 0;
		if ( uVK - 0x41 <= 0x19 || uVK - 0x70 <= 0xe || uVK - 0x30 <= 9 || uVK - 0x60 <= 0xf )
		{
			uCode = uVK;		// letters, F1-F15, digits, the numpad block
		}
		else
		{
			// table 33726390 ( 35 { VK, GFx key } pairs, all equal ); June's loop reads on into the next data until a zero dword, which no
			// VK can match
			static const unsigned s_auKeyTable[] = { 8, 9, 12, 13, 16, 17, 18, 19, 20, 27, 32, 33, 34, 35, 36, 37, 38, 39, 40, 45, 46, 47,
				144, 145, 186, 187, 188, 189, 190, 191, 192, 219, 220, 221, 222 };
			for ( size_t i = 0; i < sizeof( s_auKeyTable ) / sizeof( s_auKeyTable[0] ); ++i )
			{
				if ( s_auKeyTable[i] == uVK )
				{
					uCode = uVK;
					break;
				}
			}
		}
		unsigned char ucAscii = 0;
		if ( uCode != 0 )
		{
			BYTE abyKeyState[256];
			GetKeyboardState( abyKeyState );
			WORD wChar = 0;
			if ( ToAscii( uVK, ( (unsigned)lParam >> 16 ) & 0xff, abyKeyState, &wChar, 0 ) > 0 )
				ucAscii = (unsigned char)wChar;
		}
		const bool bDown = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN );
		const unsigned uMods = GetModifierBits();
		unsigned uResult = 0;
		if ( uCode != VK_CONTROL && ( ( uMods & 1 ) == 0 || uCode == 'V' || uCode == 'C' ) )
			uResult = SendKeyEvent( uCode, ucAscii, uMods, bDown );
		return ( ( uResult >> 1 ) & 1 ) != 0;
	}

	// 31c43480: Enter with the IME text field id 4 / 1, arrows with id 1 go to the stage as 0x46d ( March: XGM_SYSTEM_BASE, unhandled )
	bool OnEnterOrArrow( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam )
	{
		if ( wParam == VK_RETURN )
		{
			if ( m_iIMETextFieldID != 4 && m_iIMETextFieldID != 1 )
				return false;
			if ( m_pMessageStage != NULL )
				m_pMessageStage->MsgProc( m_hWnd, 0x46d, VK_RETURN, lParam );
			return true;
		}
		if ( wParam < VK_LEFT || wParam > VK_DOWN || m_iIMETextFieldID != 1 )
			return false;
		if ( m_pMessageStage != NULL )
			m_pMessageStage->MsgProc( m_hWnd, 0x46d, wParam, lParam );
		return true;
	}

	// 31c43890
	bool MsgProcKey( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( m_bEventScenePlaying == true )
			return false;
		switch ( uMsg )
		{
		case WM_INPUTLANGCHANGE:
		case WM_IME_STARTCOMPOSITION:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_COMPOSITION:
		case WM_IME_NOTIFY:
		case WM_IME_CHAR:
		case WM_IME_KEYDOWN:
			return OnIMEEvent( uMsg, wParam, lParam, hWnd, false );
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			if ( OnEnterOrArrow( uMsg, wParam, lParam ) == false && OnKeyEvent( uMsg, wParam, lParam ) == true )
				return true;
			return false;
		case WM_CHAR:
			if ( m_pUI != NULL && ( m_pUI->HandleEvent( SFG::CharEvent( (SF::UInt32)wParam ) ) & SFG::Movie::HE_NoDefaultAction ) != 0 )
				return true;
			return false;
		default:
			return false;
		}
	}

	// 31c44040
	bool MsgProcMouse( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( m_bMouseOn == false )
			return false;
		if ( m_bEventScenePlaying == true && uMsg != WM_MOUSEWHEEL )
			return false;
		const unsigned uX = (unsigned)lParam & 0xffff;
		const unsigned uY = (unsigned)lParam >> 16;
		m_bLButtonDownHit = false;
		if ( ( uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP ) && m_pUI != NULL
			&& m_pUI->HitTest( (float)uX, (float)uY ) == false )
			return false;

		if ( uMsg < WM_LBUTTONDOWN )
		{
			if ( uMsg == WM_MOUSEMOVE )
			{
				if ( m_pUI != NULL )
					m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseMove, 0, (float)uX, (float)uY, 0.f, 0 ) );
				return false;
			}
			if ( uMsg != WM_ACTIVATEAPP )
				return false;
			// the paste side of FxPlayerTextClipboard ( 31c43de0 )
			if ( OpenClipboard( NULL ) == FALSE )
				return false;
			HANDLE hData = GetClipboardData( CF_UNICODETEXT );
			if ( hData != NULL )
			{
				const wchar_t* wszText = (const wchar_t*)GlobalLock( hData );
				if ( wszText != NULL )
				{
					SF::Ptr< SFG::TextClipboard > pClipboard = m_Loader.GetTextClipboard();
					if ( pClipboard )
						pClipboard->SetText( wszText );
					GlobalUnlock( hData );
				}
			}
			CloseClipboard();
			return false;
		}

		switch ( uMsg )
		{
		case WM_LBUTTONDOWN:
			if ( m_pUI != NULL && m_pUI->HitTest( (float)uX, (float)uY ) == true )
			{
				SetCapture( hWnd );
				m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseDown, 0, (float)uX, (float)uY, 0.f, 0 ) );
				m_bLButtonDownHit = true;
				CKTDGUIDialog::ClearFocus();		// 31cd2030: March's focused UI control loses focus
				return true;
			}
			ClearFocusAS();
			return false;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return m_pUI != NULL && ( m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseUp, 0, (float)uX, (float)uY, 0.f, 0 ) )
				& SFG::Movie::HE_NoDefaultAction ) != 0;
		case WM_RBUTTONDOWN:
			SetCapture( hWnd );
			return m_pUI != NULL && ( m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseDown, 1, (float)uX, (float)uY, 0.f, 0 ) )
				& SFG::Movie::HE_NoDefaultAction ) != 0;
		case WM_RBUTTONUP:
			ReleaseCapture();
			return m_pUI != NULL && ( m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseUp, 1, (float)uX, (float)uY, 0.f, 0 ) )
				& SFG::Movie::HE_NoDefaultAction ) != 0;
		case WM_MOUSEWHEEL:
			{
				POINT kOrigin = { 0, 0 };
				ClientToScreen( hWnd, &kOrigin );
				const int iY = (short)( (unsigned)lParam >> 16 ) - kOrigin.y;
				const float fX = (float)( (short)lParam - kOrigin.x );
				const float fY = (float)iY;
				if ( m_pUI != NULL && m_pUI->HitTest( fX, fY ) == true )
				{
					const float fWheel = (float)(int)(short)( (unsigned)wParam >> 16 );
					const float fDelta = ( (float)(int)fWheel / 120.f ) * 3.f;
					m_pUI->HandleEvent( SFG::MouseEvent( SFG::Event::MouseWheel, 0, fX, fY, fDelta, 0 ) );
					// 31d23f00 / 31d23f40 on the DirectInput mouse ( app +0x1f8 +0x44 ): position reads, results unused
					if ( g_pKTDXApp->GetDIManager() != NULL && g_pKTDXApp->GetDIManager()->GetMouse() != NULL )
					{
						g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos();
						g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos();
					}
					return true;
				}
			}
			return false;
		default:
			return false;
		}
	}

	// ---- the 14 ActionScript wrappers ---------------------------------------------------------------------------------------------
	void InvokeIntArg( const char* szMethod, int iArg, SFG::Value* pResult )
	{
		if ( m_pUI == NULL )
			return;
		SFG::Value kArg( (SF::SInt32)iArg );
		m_pUI->Invoke( szMethod, pResult, &kArg, 1 );
	}
	bool InvokeIntArgBool( const char* szMethod, int iArg )
	{
		if ( m_pUI == NULL )
			return false;
		SFG::Value kResult;
		InvokeIntArg( szMethod, iArg, &kResult );
		return kResult.IsBool() ? kResult.GetBool() : false;
	}
	void ClearFocusAS()		// 31c43150
	{
		if ( m_pUI != NULL )
			m_pUI->Invoke( "clearFocus", NULL, NULL, 0 );
	}

	SFG::Loader					m_Loader;				// +8 ( ZlibSupport + FileOpener from the Loader's default arguments, as June's ctor )
	SF::Ptr< SFR::D3D9::HAL >	m_pHAL;					// +0x3c
	SF::Ptr< SFR::Renderer2D >	m_pRenderer;			// +0x40
	D3DPRESENT_PARAMETERS		m_kPresentParams;		// +0x44
	DWORD						m_dwLastTime;			// +0x7c
	IDirect3DStateBlock9*		m_pStateBlock;			// +0x80
	CKTDXStage*					m_pMessageStage;		// +0x84
	CKTDGGFXUI*					m_pUI;					// +0x98
	HWND						m_hWnd;					// +0xb8
	D3DVIEWPORT9				m_kViewport;			// +0xbc
	float						m_fElapsed;				// +0xd4
	FontConfigSet				m_kFontConfigs;			// +0xf8
	int							m_iFontConfig;			// +0x104
	bool						m_bShow;				// +0x108
	short						m_sIMECompositionLength;// +0x10a
	bool						m_bIMEComposing;		// +0x10c
	SF::Ptr< SF::Sound::SoundRendererFMOD >	m_pSoundRenderer;	// +0x110
	bool						m_bLButtonDownHit;		// +0x114
	bool						m_bEventScenePlaying;	// +0x115
	int							m_iLA;					// +0x118
	bool						m_bMouseOn;				// +0x11c
	int							m_iIMETextFieldID;		// June's app +0x28c
	bool						m_bMainLoaded;
	// +0x18 ( 9 dwords, copied out by vslot 2 31c431a0 ) and +0xd8 ( a String read by the clipboard path ) have no other reader in June;
	// +0x8c ( a std::map, ctor only ) has none: not carried.
};

#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
// ---------------------------------------------------------------------------------------------------------------------------------------
// CKTDGGFXEventSceneBridge ( Phase G3 ): June's CX2GFXEventSceneAdapter ( RTTI .?AVCX2GFXEventSceneAdapter@@, 0x10 B, ctor 32e59c50, vtable
// 33531fb4 ). It is a listener on the root movie's adapter named "EventScene" ( 32e5aa80 ) whose OnExternalCall ( 32e59900 ) answers the
// AS call "gameDataEventScene( obj )" by putting four native functions on obj ( table 33531f58 ), and it owns the data bindings
// "EventScene.nextScene" / "EventScene.bExactFit" ( ctor @32e59ca7..32e59d2a, dtor 32e59820 ).
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	const char*	JGES_BINDING_NEXT_SCENE	= "EventScene.nextScene";
	const char*	JGES_BINDING_EXACT_FIT	= "EventScene.bExactFit";

	void GfxSceneLog( const char* szFormat, ... )
	{
		char szBuf[2048] = { 0, };
		va_list args;
		va_start( args, szFormat );
		_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
		va_end( args );
		GfxLogLine( std::string( "[JUNE_GFX] SCENE " ) + szBuf );
	}
}

class CKTDGGFXEventSceneBridge::CImpl : public IGfxListener
{
public:
	CImpl( CKTDGGFXManagerImpl* pMgrImpl, IGfxEventSceneHost* pHost )
		: m_pHost( pHost ), m_pLink( *new CGfxOwnerLink( this ) )
	{
		if ( pMgrImpl != NULL && pMgrImpl->m_pUI != NULL )
			m_pAdapter = pMgrImpl->m_pUI->GetAdapter();
		if ( m_pAdapter )
		{
			m_pAdapter->AddListener( this );													// 32e59c50 -> 31d1f690
			m_pAdapter->AddBinding( JGES_BINDING_NEXT_SCENE, JGBK_TRIGGER );					// @32e59cc1: the 8 B trigger object
			m_pAdapter->AddBinding( JGES_BINDING_EXACT_FIT, JGBK_BOOL );						// @32e59d2a: the 0xc B bool object, false
		}
		GfxSceneLog( "bridge: %s", m_pAdapter ? "listener + 2 bindings on the root movie's adapter" : "no root movie ( GFx scenes cannot play )" );
	}

	~CImpl()
	{
		m_pLink->m_pOwner = NULL;
		if ( m_pAdapter )		// 32e59820 / 32e5a9c0: 31d1f550 ( the listener ), 31d1f600 x 2
		{
			m_pAdapter->RemoveListener( GetListenerName() );
			m_pAdapter->RemoveBinding( JGES_BINDING_NEXT_SCENE );
			m_pAdapter->RemoveBinding( JGES_BINDING_EXACT_FIT );
		}
	}

	// ---- IJuneGfxListener ------------------------------------------------------------------------------------------------------------
	virtual std::string GetListenerName() const { return "EventScene"; }		// 32e5aa80: the 10 chars of 3345c3ec

	// 32e59900
	virtual bool OnExternalCall( SFG::Movie* pMovie, const char* szMethod, const SFG::Value* pArgs, unsigned uArgCount )
	{
		if ( szMethod == NULL || strcmp( szMethod, "gameDataEventScene" ) != 0 )
			return false;					// 32e59900 returns 0 for every other name

		// table 33531f58, in June's order
		static const char* const s_aszFunction[ 4 ] = { "endEventScene", "playEventSceneSound", "getEventSceneData", "playedEventSceneAndGetMent" };
		if ( pMovie == NULL || uArgCount < 1 || ( pArgs[0].IsObject() == false && pArgs[0].IsDisplayObject() == false ) )
			return true;

		SFG::Value kObject( pArgs[0] );
		for ( int i = 0; i < 4; ++i )
		{
			SFG::Value kCurrent;
			kObject.GetMember( s_aszFunction[ i ], &kCurrent );
			if ( kCurrent.IsObject() == false && kCurrent.IsArray() == false && kCurrent.IsDisplayObject() == false )		// @32e59a20: types 8 / 9 / 10 are left alone
			{
				SF::Ptr< CGfxFunctionHandler > pHandler = *new CGfxFunctionHandler( m_pLink.GetPtr(), &CImpl::OnFunction, i );
				SFG::Value kFunction;
				pMovie->CreateFunction( &kFunction, pHandler );
				kObject.SetMember( s_aszFunction[ i ], kFunction );
			}

			// @32e59b45: inside June's loop, so the binding is pushed four times ( an idempotent setExactFit )
			if ( m_pAdapter && m_pAdapter->HasBinding( JGES_BINDING_EXACT_FIT ) )
			{
				const bool bExactFit = ( g_pKTDXApp->GetResolutionScaleY() < 1.0f || g_pKTDXApp->GetResolutionScaleX() < 1.0f );
				m_pAdapter->SetBindingBool( JGES_BINDING_EXACT_FIT, bExactFit );
				m_pAdapter->NotifyBinding( JGES_BINDING_EXACT_FIT );
			}
		}
		return true;
	}

	void FireNextScene()		// 32e59800
	{
		if ( m_pAdapter )
			m_pAdapter->NotifyBinding( JGES_BINDING_NEXT_SCENE );
	}

	bool IsBound() const { return m_pAdapter.GetPtr() != NULL; }

private:
	static void OnFunction( void* pOwner, int iID, const SFG::FunctionHandler::Params& kParams )
	{
		static_cast< CImpl* >( pOwner )->DoFunction( iID, kParams );
	}

	void DoFunction( int iID, const SFG::FunctionHandler::Params& kParams )
	{
		switch ( iID )
		{
		case 0:		// endEventScene 32e597e0
			if ( m_pHost != NULL )
				m_pHost->OnSceneAsEnd();
			break;

		case 1:		// playEventSceneSound 32e59d40: a String argument, UTF-8, at most 0xff wide characters
			if ( m_pHost != NULL && kParams.pArgs != NULL && kParams.ArgCount >= 1 && kParams.pArgs[0].IsString() )
			{
				std::wstring wstrKey = GfxFromUTF8( kParams.pArgs[0].GetString() );
				if ( wstrKey.size() > 0xfe )
					wstrKey.resize( 0xfe );
				m_pHost->OnSceneAsPlaySound( wstrKey );
			}
			break;

		case 2:		// getEventSceneData 32e59f10
			DoGetEventSceneData( kParams );
			break;

		case 3:		// playedEventSceneAndGetMent 32e59e00
			DoPlayedEventSceneAndGetMent( kParams );
			break;
		}
	}

	// 32e59e00: ( frame ) -> the scene's ment id for that frame ( 3226a2b0 ), its voice GFX_ID_<id>.ogg, and the text key "GFX_ID_<id>" returned
	void DoPlayedEventSceneAndGetMent( const SFG::FunctionHandler::Params& kParams )
	{
		if ( kParams.pArgs == NULL || kParams.ArgCount < 1 || kParams.pMovie == NULL || kParams.pRetVal == NULL )
			return;
		const SFG::Value& kArg = kParams.pArgs[0];
		int iFrame = 0;
		if ( kArg.IsInt() )
			iFrame = kArg.GetInt();
		else if ( kArg.IsUInt() )
			iFrame = (int)kArg.GetUInt();
		else if ( kArg.IsNumber() )
		{
			double dNumber = kArg.GetNumber();
			memcpy( &iFrame, &dNumber, sizeof( iFrame ) );		// June reads the data word whatever the type ( @32e59e45 )
		}
		else
			return;

		int iMentID = 0;
		wchar_t wszBuf[ 256 ] = { 0, };
		if ( m_pHost != NULL )
		{
			iMentID = m_pHost->OnSceneAsGetMentID( iFrame );
			swprintf_s( wszBuf, 256, L"GFX_ID_%d.ogg", iMentID );		// 33531ff8
			m_pHost->OnSceneAsPlaySound( wszBuf );
		}
		swprintf_s( wszBuf, 256, L"GFX_ID_%d", iMentID );				// 33531fe4
		kParams.pMovie->CreateStringW( kParams.pRetVal, wszBuf );		// Movie slot 0x88
	}

	// 32e59f10: returns { arrEventSceneData: [ { frame, soundKey, characterNameKey, textKey, wordBalloonName, wordBalloonPosX, wordBalloonPosY,
	// arrSymbolData: [ { symbolName, posX, posY, delayTime } ] } ] }
	void DoGetEventSceneData( const SFG::FunctionHandler::Params& kParams )
	{
		if ( m_pHost == NULL || kParams.pMovie == NULL || kParams.pRetVal == NULL )
			return;
		SFG::Movie* pMovie = kParams.pMovie;
		pMovie->CreateObject( kParams.pRetVal );			// @32e59f7f
		std::vector< KGfxSceneFrame > vecFrame;
		if ( m_pHost->OnSceneAsGetFrames( vecFrame ) == false )
			return;											// June returns the empty object

		SFG::Value kArray;
		pMovie->CreateArray( &kArray );
		for ( size_t i = 0; i < vecFrame.size(); ++i )
		{
			const KGfxSceneFrame& kFrame = vecFrame[ i ];
			SFG::Value kFrameObject;
			pMovie->CreateObject( &kFrameObject );
			kFrameObject.SetMember( "frame",			SFG::Value( (SF::SInt32)kFrame.m_iFrame ) );
			kFrameObject.SetMember( "soundKey",			SFG::Value( kFrame.m_strSoundKey.c_str() ) );
			kFrameObject.SetMember( "characterNameKey",	SFG::Value( kFrame.m_strCharacterNameKey.c_str() ) );
			kFrameObject.SetMember( "textKey",			SFG::Value( kFrame.m_strTextKey.c_str() ) );
			kFrameObject.SetMember( "wordBalloonName",	SFG::Value( kFrame.m_strWordBalloonName.c_str() ) );
			kFrameObject.SetMember( "wordBalloonPosX",	SFG::Value( (SF::SInt32)kFrame.m_iWordBalloonPosX ) );
			kFrameObject.SetMember( "wordBalloonPosY",	SFG::Value( (SF::SInt32)kFrame.m_iWordBalloonPosY ) );

			SFG::Value kSymbolArray;
			pMovie->CreateArray( &kSymbolArray );
			for ( size_t j = 0; j < kFrame.m_vecSymbol.size(); ++j )
			{
				const KGfxSceneSymbol& kSymbol = kFrame.m_vecSymbol[ j ];
				SFG::Value kSymbolObject;
				pMovie->CreateObject( &kSymbolObject );
				kSymbolObject.SetMember( "symbolName",	SFG::Value( kSymbol.m_strSymbolName.c_str() ) );
				kSymbolObject.SetMember( "posX",		SFG::Value( (SF::SInt32)kSymbol.m_iPosX ) );
				kSymbolObject.SetMember( "posY",		SFG::Value( (SF::SInt32)kSymbol.m_iPosY ) );
				kSymbolObject.SetMember( "delayTime",	SFG::Value( (SF::SInt32)kSymbol.m_iDelayTime ) );
				kSymbolArray.PushBack( kSymbolObject );
			}
			kFrameObject.SetMember( "arrSymbolData", kSymbolArray );
			kArray.PushBack( kFrameObject );
		}
		kParams.pRetVal->SetMember( "arrEventSceneData", kArray );		// @32e5a8eb
	}

	IGfxEventSceneHost*				m_pHost;
	SF::Ptr< CGfxOwnerLink >		m_pLink;
	SF::Ptr< CLIKDataBindingAdapter >	m_pAdapter;
};

CKTDGGFXEventSceneBridge::CKTDGGFXEventSceneBridge( CKTDGGFXManager* pManager, IGfxEventSceneHost* pHost )
	: m_pImpl( new CImpl( pManager != NULL ? pManager->m_pImpl : NULL, pHost ) )
{
}

CKTDGGFXEventSceneBridge::~CKTDGGFXEventSceneBridge()
{
	SAFE_DELETE( m_pImpl );
}

bool CKTDGGFXEventSceneBridge::IsBound() const			{ return m_pImpl->IsBound(); }
void CKTDGGFXEventSceneBridge::FireNextScene()			{ m_pImpl->FireNextScene(); }
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
// ---------------------------------------------------------------------------------------------------------------------------------------
// CKTDGGFXSimpleWidgetPlayer ( Phase G4 ): June's CX2SimpleWidgetPlayer ( vtable 33471da4: slot 1 31fc79a0 = the listener name "simpleWidgetPlayer",
// slot 2 31fc73a0 = the external-call callback ). Ctor 31fc74e0 adds itself as a listener on the root movie's adapter and registers two data
// bindings: "SimpleWidgetPlayer.playSimpleWidget" ( value object 0x30 B, vtable 33471e40 ) and "SimpleWidgetPlayer.clearSimpleWidgetPlayer" ( the
// 8 B trigger ); dtor 31fc72e0 removes the listener and both. main.gfx's AS3 SimpleWidgetPlayer observes playSimpleWidget and loads the widget.
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	const char*	JGSW_BINDING_PLAY	= "SimpleWidgetPlayer.playSimpleWidget";			// 33471d7c
	const char*	JGSW_BINDING_CLEAR	= "SimpleWidgetPlayer.clearSimpleWidgetPlayer";		// 33471d50
}

class CKTDGGFXSimpleWidgetPlayer::CImpl : public IGfxListener
{
public:
	explicit CImpl( CKTDGGFXManagerImpl* pMgrImpl )
	{
		if ( pMgrImpl != NULL && pMgrImpl->m_pUI != NULL )
			m_pAdapter = pMgrImpl->m_pUI->GetAdapter();
		if ( m_pAdapter )
		{
			m_pAdapter->AddListener( this );									// 31fc74e0 -> 31d1f690
			m_pAdapter->AddBinding( JGSW_BINDING_PLAY, JGBK_SIMPLE_WIDGET );	// 31d1f900: the 0x30 B value object
			m_pAdapter->AddBinding( JGSW_BINDING_CLEAR, JGBK_TRIGGER );			// 31d1f900: the 8 B trigger, notified by nobody in June
		}
	}

	~CImpl()
	{
		if ( m_pAdapter )		// 31fc72e0: 31d1f550 ( the listener ), 31d1f600 x 2
		{
			m_pAdapter->RemoveListener( GetListenerName() );
			m_pAdapter->RemoveBinding( JGSW_BINDING_PLAY );
			m_pAdapter->RemoveBinding( JGSW_BINDING_CLEAR );
		}
	}

	virtual std::string GetListenerName() const { return "simpleWidgetPlayer"; }		// 31fc79a0: 18 chars of 33471e84

	// 31fc73a0: the AS call "simpleWidgetFunctionInit" is answered ( true ) and does nothing: June copies the first argument, makes a 0xc B function
	// object ( vtable 33471e00, owner = the player ) and drops it again without setting it anywhere
	virtual bool OnExternalCall( SFG::Movie* /*pMovie*/, const char* szMethod, const SFG::Value* /*pArgs*/, unsigned /*uArgCount*/ )
	{
		return ( szMethod != NULL && strcmp( szMethod, "simpleWidgetFunctionInit" ) == 0 );
	}

	bool IsBound() const { return m_pAdapter.GetPtr() != NULL; }

	// 31fc75e0
	bool PlaySimpleWidget( int iUIID, const char* szFileName, int iParam1, int iParam2 )
	{
		if ( !m_pAdapter || szFileName == NULL )
			return false;
		if ( m_pAdapter->HasBinding( JGSW_BINDING_PLAY ) == false )		// 31d1ef00 returned 0
			return false;
		m_pAdapter->SetBindingWidget( JGSW_BINDING_PLAY, iUIID, szFileName, iParam1, iParam2 );
		m_pAdapter->NotifyBinding( JGSW_BINDING_PLAY );					// 31d1ee10
		return true;
	}

private:
	SF::Ptr< CLIKDataBindingAdapter >	m_pAdapter;
};

CKTDGGFXSimpleWidgetPlayer::CKTDGGFXSimpleWidgetPlayer( CKTDGGFXManager* pManager )
	: m_pImpl( new CImpl( pManager != NULL ? pManager->m_pImpl : NULL ) )
{
}

CKTDGGFXSimpleWidgetPlayer::~CKTDGGFXSimpleWidgetPlayer()
{
	SAFE_DELETE( m_pImpl );
}

bool CKTDGGFXSimpleWidgetPlayer::IsBound() const															{ return m_pImpl->IsBound(); }
bool CKTDGGFXSimpleWidgetPlayer::PlaySimpleWidget( int iUIID, const char* szFileName, int iParam1, int iParam2 )	{ return m_pImpl->PlaySimpleWidget( iUIID, szFileName, iParam1, iParam2 ); }
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN

#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
// ---------------------------------------------------------------------------------------------------------------------------------------
// CKTDGGFXResultBridge ( Phase R1 ): June's CX2UIDungeonResultAdapter ( vtable 3353a060: slot 1 32eeb1a0 = the listener name "UIDungeonResult", slot 2
// 32eea3c0 = the external-call callback, slot 3 32eea220 = the binding-created notice ). The AS call "gameDataResultFunctionInit( obj )" gets the five native
// functions of the table 33539ea8 ( function object vtable 3353a074, the same Call 32eee800 as the scene's ); the owner is the dungeon state.
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	const char*	JGRS_LISTENER_NAME		= "UIDungeonResult";							// 3353a09c
	const char*	JGRS_BINDING_INFO		= "GameResult.updateDungeonInfo";				// 3353a03c
	const char*	JGRS_BINDING_TAB_KEY	= "GameResult.onTabKeyProcess";					// 33539be4
	const char*	JGRS_BINDING_SHOW_RETRY	= "GameResult.showRetryButton";					// 33539f28
	const char*	JGRS_BINDING_ENABLE_RETRY = "GameResult.enableRetryButton";				// 33539f08
	const int	JGRS_ITEM_COUNT			= 12;
	// table 33539ea8, in June's order ( the spelling "notfy" is June's )
	const char* const s_aszResultFunction[ 5 ] = { "notifyCloseResultUI", "notfyRetryButton", "notifyInitialize", "notfyitemSlotArrowLeft", "notfyitemSlotArrowRight" };

	std::string GfxResultItemBindingName( int iIndex )
	{
		char szName[ 64 ] = { 0, };
		sprintf_s( szName, sizeof( szName ), "GameResult.getItem%d", iIndex );		// 3349f68c is the state's "GameResult.getItem%d"
		return szName;
	}
}

#ifdef SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
// June 3353a074 / Call 32eee800 for the five result callbacks: their targets ignore the owner, so the id goes to the dispatcher the adapter registered
static void ( *s_pfnResultDispatch )( int iID ) = NULL;

class CGfxResultFunctionHandler : public SFG::FunctionHandler
{
public:
	explicit CGfxResultFunctionHandler( int iID ) : m_iID( iID ) {}
	virtual void Call( const Params& /*kParams*/ )
	{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		char szBuf[ 128 ] = { 0, };
		_snprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, "[JUNE_RESULT] WIDGET call %d%s", m_iID, s_pfnResultDispatch == NULL ? " dropped" : "" );
		KLOG( "ErrorLog.txt" ) << szBuf << fileout;
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( s_pfnResultDispatch != NULL )
			s_pfnResultDispatch( m_iID );
	}
private:
	int		m_iID;
};
#endif SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL

class CKTDGGFXResultBridge::CImpl : public IGfxListener
{
public:
	CImpl( CKTDGGFXManagerImpl* pMgrImpl, IGfxResultHost* pHost )
		: m_pHost( pHost ), m_pLink( *new CGfxOwnerLink( this ) )
	{
		if ( pMgrImpl != NULL && pMgrImpl->m_pUI != NULL )
			m_pAdapter = pMgrImpl->m_pUI->GetAdapter();
		if ( m_pAdapter )		// 32eea7e0 -> 31d1f690, then 31d1f900 x 16
		{
			m_pAdapter->AddListener( this );
			m_pAdapter->AddBinding( JGRS_BINDING_INFO, JGBK_RESULT_INFO );
			for ( int i = 0; i < JGRS_ITEM_COUNT; ++i )
				m_pAdapter->AddBinding( GfxResultItemBindingName( i ), JGBK_RESULT_ITEM );
			m_pAdapter->AddBinding( JGRS_BINDING_TAB_KEY, JGBK_TRIGGER );
			m_pAdapter->AddBinding( JGRS_BINDING_SHOW_RETRY, JGBK_BOOL );
			m_pAdapter->SetBindingBool( JGRS_BINDING_SHOW_RETRY, true );		// the 0xc B bool objects start at 1
			m_pAdapter->AddBinding( JGRS_BINDING_ENABLE_RETRY, JGBK_BOOL );
			m_pAdapter->SetBindingBool( JGRS_BINDING_ENABLE_RETRY, true );
		}
	}

	~CImpl()
	{
		m_pLink->m_pOwner = NULL;
		if ( m_pAdapter )		// 32eea090: 31d1f550 ( the listener ), then 31d1f600 x 16
		{
			m_pAdapter->RemoveListener( GetListenerName() );
			m_pAdapter->RemoveBinding( JGRS_BINDING_INFO );
			for ( int i = 0; i < JGRS_ITEM_COUNT; ++i )
				m_pAdapter->RemoveBinding( GfxResultItemBindingName( i ) );
			m_pAdapter->RemoveBinding( JGRS_BINDING_TAB_KEY );
			m_pAdapter->RemoveBinding( JGRS_BINDING_SHOW_RETRY );
			m_pAdapter->RemoveBinding( JGRS_BINDING_ENABLE_RETRY );
		}
	}

	// ---- IJuneGfxListener ------------------------------------------------------------------------------------------------------------
	virtual std::string GetListenerName() const { return JGRS_LISTENER_NAME; }

	// 32eea3c0
	virtual bool OnExternalCall( SFG::Movie* pMovie, const char* szMethod, const SFG::Value* pArgs, unsigned uArgCount )
	{
		if ( szMethod == NULL || strcmp( szMethod, "gameDataResultFunctionInit" ) != 0 )
			return false;
		if ( pMovie == NULL || uArgCount < 1 || ( pArgs[0].IsObject() == false && pArgs[0].IsDisplayObject() == false ) )
			return true;

		SFG::Value kObject( pArgs[0] );
		for ( int i = 0; i < 5; ++i )
		{
			SFG::Value kCurrent;
			kObject.GetMember( s_aszResultFunction[ i ], &kCurrent );
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
			{
				char szBuf[ 160 ] = { 0, };
				if ( kCurrent.IsObject() == false && kCurrent.IsArray() == false && kCurrent.IsDisplayObject() == false )
					_snprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, "[JUNE_RESULT] WIDGET init: %s attached", s_aszResultFunction[ i ] );
				else
					_snprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, "[JUNE_RESULT] WIDGET init: %s kept (type %d)", s_aszResultFunction[ i ], static_cast< int >( kCurrent.GetType() ) );
				KLOG( "ErrorLog.txt" ) << szBuf << fileout;
			}
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
			if ( kCurrent.IsObject() == false && kCurrent.IsArray() == false && kCurrent.IsDisplayObject() == false )		// @32eea4c6: types 8 / 9 / 10 are left alone
			{
#ifdef SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
				SF::Ptr< CGfxResultFunctionHandler > pHandler = *new CGfxResultFunctionHandler( i );
#else SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
				SF::Ptr< CGfxFunctionHandler > pHandler = *new CGfxFunctionHandler( m_pLink.GetPtr(), &CImpl::OnFunction, i );
#endif SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
				SFG::Value kFunction;
				pMovie->CreateFunction( &kFunction, pHandler );
				kObject.SetMember( s_aszResultFunction[ i ], kFunction );
			}
		}
		return true;
	}

	// 32eea220: answers true for every name ( June's walk stops here ); the two names it knows are notified at once
	virtual bool OnBindingCreated( const std::string& strName )
	{
		if ( m_pAdapter )
		{
			if ( strName == JGRS_BINDING_INFO )
				m_pAdapter->NotifyBinding( JGRS_BINDING_INFO );
			else if ( strName == "GameResult.updatePveInfo" )		// 33539c00: June's PvE twin shares this slot; no such binding here, so a no-op
				m_pAdapter->NotifyBinding( strName );
		}
		return true;
	}

	bool IsBound() const { return m_pAdapter.GetPtr() != NULL; }

	void UpdateDungeonInfo( const KGfxResultInfo& kInfo )		// 32eeae30
	{
		if ( !m_pAdapter )
			return;
		m_pAdapter->SetBindingResultInfo( JGRS_BINDING_INFO, kInfo );
		m_pAdapter->NotifyBinding( JGRS_BINDING_INFO );
	}

	void SetItem( int iIndex, const KGfxResultItem& kItem )		// 32eea6a0
	{
		if ( !m_pAdapter )
			return;
		const std::string strName = GfxResultItemBindingName( iIndex );
		if ( m_pAdapter->HasBinding( strName ) == false )
			return;
		m_pAdapter->SetBindingResultItem( strName, kItem );
		m_pAdapter->NotifyBinding( strName );
	}

	void SetBool( const char* szBinding, bool bValue )			// 32eea1c0 / 32eea1f0
	{
		if ( !m_pAdapter )
			return;
		if ( m_pAdapter->HasBinding( szBinding ) == true )
			m_pAdapter->SetBindingBool( szBinding, bValue );
		m_pAdapter->NotifyBinding( szBinding );
	}

#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	void NotifyTabKey()		// 32eea070
	{
		if ( m_pAdapter )
			m_pAdapter->NotifyBinding( JGRS_BINDING_TAB_KEY );
	}
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW

private:
	static void OnFunction( void* pOwner, int iID, const SFG::FunctionHandler::Params& /*kParams*/ )
	{
		static_cast< CImpl* >( pOwner )->DoFunction( iID );
	}

	void DoFunction( int iID )
	{
		if ( m_pHost == NULL )
			return;
		switch ( iID )
		{
		case 0:		m_pHost->OnResultAsClose();			break;		// 32eea2c0
		case 1:		m_pHost->OnResultAsRetry();			break;		// 32eea2e0
		case 2:		m_pHost->OnResultAsInitialize();	break;		// 32eea300
		case 3:		m_pHost->OnResultAsArrowLeft();		break;		// 32eea320
		case 4:		m_pHost->OnResultAsArrowRight();	break;		// 32eea370
		}
	}

	IGfxResultHost*					m_pHost;
	SF::Ptr< CGfxOwnerLink >		m_pLink;
	SF::Ptr< CLIKDataBindingAdapter >	m_pAdapter;
};

CKTDGGFXResultBridge::CKTDGGFXResultBridge( CKTDGGFXManager* pManager, IGfxResultHost* pHost )
	: m_pImpl( new CImpl( pManager != NULL ? pManager->m_pImpl : NULL, pHost ) )
{
}

CKTDGGFXResultBridge::~CKTDGGFXResultBridge()
{
	SAFE_DELETE( m_pImpl );
}

bool CKTDGGFXResultBridge::IsBound() const															{ return m_pImpl->IsBound(); }
void CKTDGGFXResultBridge::UpdateDungeonInfo( const KGfxResultInfo& kInfo )						{ m_pImpl->UpdateDungeonInfo( kInfo ); }
void CKTDGGFXResultBridge::SetItem( int iIndex, const KGfxResultItem& kItem )					{ m_pImpl->SetItem( iIndex, kItem ); }
void CKTDGGFXResultBridge::ShowRetryButton( bool bShow )											{ m_pImpl->SetBool( JGRS_BINDING_SHOW_RETRY, bShow ); }
void CKTDGGFXResultBridge::EnableRetryButton( bool bEnable )										{ m_pImpl->SetBool( JGRS_BINDING_ENABLE_RETRY, bEnable ); }
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
void CKTDGGFXResultBridge::NotifyTabKey()																{ m_pImpl->NotifyTabKey(); }
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW
#ifdef SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
void CKTDGGFXResultBridge::SetFunctionDispatcher( void ( *pfnDispatch )( int iID ) )					{ s_pfnResultDispatch = pfnDispatch; }
#endif SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET

// ---------------------------------------------------------------------------------------------------------------------------------------
// CKTDGGFXManager
// ---------------------------------------------------------------------------------------------------------------------------------------
CKTDGGFXManager::CKTDGGFXManager( HWND hWnd )
	: m_pImpl( new CKTDGGFXManagerImpl( hWnd ) )
{
}

CKTDGGFXManager::~CKTDGGFXManager()
{
	SAFE_DELETE( m_pImpl );
}

void CKTDGGFXManager::Init( IDirect3DDevice9* pDevice )								{ m_pImpl->Init( pDevice ); }
void CKTDGGFXManager::OnFrameMove()													{ m_pImpl->OnFrameMove(); }
void CKTDGGFXManager::OnFrameRender()												{ m_pImpl->OnFrameRender(); }
void CKTDGGFXManager::OnLostDevice()												{ m_pImpl->OnLostDevice(); }
void CKTDGGFXManager::OnResetDevice( IDirect3DDevice9* pDevice )					{ m_pImpl->OnResetDevice( pDevice ); }
void CKTDGGFXManager::OnResize( const D3DPRESENT_PARAMETERS& kPresentParams )		{ m_pImpl->OnResize( kPresentParams ); }
void CKTDGGFXManager::MainLoopFilter( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )	{ m_pImpl->MainLoopFilter( hWnd, uMsg, wParam, lParam ); }
bool CKTDGGFXManager::MsgProcMouse( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )	{ return m_pImpl->MsgProcMouse( hWnd, uMsg, wParam, lParam ); }
bool CKTDGGFXManager::MsgProcKey( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )		{ return m_pImpl->MsgProcKey( hWnd, uMsg, wParam, lParam ); }
void CKTDGGFXManager::SetMessageStage( CKTDXStage* pStage )						{ m_pImpl->m_pMessageStage = pStage; }
CKTDXStage* CKTDGGFXManager::GetMessageStage() const								{ return m_pImpl->m_pMessageStage; }

void CKTDGGFXManager::openUI( int iUIID )				{ m_pImpl->InvokeIntArg( "openUI", iUIID, NULL ); }
bool CKTDGGFXManager::isOpenUI( int iUIID )				{ return m_pImpl->InvokeIntArgBool( "isOpenUI", iUIID ); }
// moveToTopZorder / changeCharacter / ShowUI / HideUI: June builds a result Value but passes NULL to Invoke ( 31c44860 / 31c44940 /
// 31c44ae0 / 31c44bc0 ); G2 review corrected the result pointer to NULL
void CKTDGGFXManager::moveToTopZorder( int iUIID )		{ m_pImpl->InvokeIntArg( "moveToTopZorder", iUIID, NULL ); }
void CKTDGGFXManager::unloadWidget( int iUIID )			{ m_pImpl->InvokeIntArg( "unloadWidget", iUIID, NULL ); }
void CKTDGGFXManager::unloadUserWidget( int iUIID )		{ m_pImpl->InvokeIntArg( "unloadUserWidget", iUIID, NULL ); }
void CKTDGGFXManager::clearFocus()						{ m_pImpl->ClearFocusAS(); }
void CKTDGGFXManager::changeCharacter( int iUnitClass )	{ m_pImpl->InvokeIntArg( "changeCharacter", iUnitClass, NULL ); }
void CKTDGGFXManager::ShowUI( int iUIID )				{ m_pImpl->InvokeIntArg( "ShowUI", iUIID, NULL ); }
void CKTDGGFXManager::HideUI( int iUIID )				{ m_pImpl->InvokeIntArg( "HideUI", iUIID, NULL ); }
bool CKTDGGFXManager::isVisibleI( int iUIID )			{ return m_pImpl->InvokeIntArgBool( "isVisibleI", iUIID ); }

bool CKTDGGFXManager::closeToTopZorder()
{
	if ( m_pImpl->m_pUI == NULL )
		return false;
	SFG::Value kResult;
	m_pImpl->m_pUI->Invoke( "closeToTopZorder", &kResult, NULL, 0 );
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG
	// G5 play-test 1: ESC never got a true with UI 0xb open. What the call really returned ( the type is the GFx Value type: 2 = Boolean )
	{
		char szBuf[200] = { 0, };
		_snprintf( szBuf, sizeof( szBuf ) - 1, "[JUNE_GFX] GATE closeToTopZorder raw: type %d isBool %d bool %d, isOpenUI( 0xb ) %d, isVisibleI( 0xb ) %d",
			static_cast< int >( kResult.GetType() ), kResult.IsBool() ? 1 : 0, ( kResult.IsBool() && kResult.GetBool() ) ? 1 : 0,
			isOpenUI( 0xb ) ? 1 : 0, isVisibleI( 0xb ) ? 1 : 0 );
		GfxLogLine( szBuf );
	}
#endif SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG
	return kResult.IsBool() ? kResult.GetBool() : false;
}

void CKTDGGFXManager::loadUserWidget( int iUIID, const WCHAR* wszPath, int iArg1, int iArg2 )
{
	if ( m_pImpl->m_pUI == NULL )
		return;
	SFG::Value akArgs[4] = { SFG::Value( (SF::Double)iUIID ), SFG::Value( wszPath ), SFG::Value( (SF::Double)iArg1 ), SFG::Value( (SF::Double)iArg2 ) };
	m_pImpl->m_pUI->Invoke( "loadUserWidget", NULL, akArgs, 4 );
}

void CKTDGGFXManager::loadEventSceneEndFrame( int iUIID, const WCHAR* wszPath, int iFrame )
{
	if ( m_pImpl->m_pUI == NULL )
		return;
	SFG::Value akArgs[3] = { SFG::Value( (SF::Double)iUIID ), SFG::Value( wszPath ), SFG::Value( (SF::Double)iFrame ) };
	m_pImpl->m_pUI->Invoke( "loadEventSceneEndFrame", NULL, akArgs, 3 );
}

void CKTDGGFXManager::showOkPopup( std::wstring wstrMessage )
{
	if ( m_pImpl->m_pUI == NULL )
		return;
	SFG::Value kArg( wstrMessage.c_str() );
	m_pImpl->m_pUI->Invoke( "showOkPopup", NULL, &kArg, 1 );
}

bool CKTDGGFXManager::IsMovieLoaded() const						{ return m_pImpl->m_pUI != NULL; }
bool CKTDGGFXManager::GetShow() const							{ return m_pImpl->m_bShow; }
void CKTDGGFXManager::SetShow( bool bShow )						{ m_pImpl->m_bShow = bShow; }
bool CKTDGGFXManager::GetEventScenePlaying() const				{ return m_pImpl->m_bEventScenePlaying; }
void CKTDGGFXManager::SetEventScenePlaying( bool bPlaying )		{ m_pImpl->m_bEventScenePlaying = bPlaying; }
bool CKTDGGFXManager::GetLastLButtonDownHit() const				{ return m_pImpl->m_bLButtonDownHit; }
void CKTDGGFXManager::SetMouseInputEnabled( bool bEnabled )		{ m_pImpl->m_bMouseOn = bEnabled; }
bool CKTDGGFXManager::GetMouseInputEnabled() const				{ return m_pImpl->m_bMouseOn; }
void CKTDGGFXManager::SetIMETextFieldID( int iID )				{ m_pImpl->m_iIMETextFieldID = iID; }
int  CKTDGGFXManager::GetIMETextFieldID() const					{ return m_pImpl->m_iIMETextFieldID; }

void CKTDGGFXManager::FirstFrameLog()
{
	if ( s_bGfxLogFlushed == true )
		return;
	if ( s_bGfxLogInit == false )
	{
		InitializeCriticalSection( &s_csGfxLog );
		s_bGfxLogInit = true;
	}
	EnterCriticalSection( &s_csGfxLog );
	for ( size_t i = 0; i < s_vecGfxLogPending.size(); ++i )
	{
		KLOG( "ErrorLog.txt" ) << s_vecGfxLogPending[i].c_str() << fileout;
	}
	s_vecGfxLogPending.clear();
	s_bGfxLogFlushed = true;
	LeaveCriticalSection( &s_csGfxLog );
}

#ifdef SERV_IRUHADEV_JUNE_GFX_DEBUG
// ---------------------------------------------------------------------------------------------------------------------------------------
// Dev-console hooks ( typed at the console as Lua, bound in CKTDXApp::RegisterLuabind )
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	CKTDGGFXManager* GfxDebugManager()
	{
		CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetGFXManager() : NULL;
		if ( pManager == NULL || pManager->IsMovieLoaded() == false )
		{
			GfxLog( "debug: no GFx movie ( main.gfx not loaded )" );
			return NULL;
		}
		return pManager;
	}
}

void GfxOpenUI_LUA( int iUIID )
{
	CKTDGGFXManager* pManager = GfxDebugManager();
	if ( pManager == NULL )
		return;
	pManager->openUI( iUIID );
	GfxLog( "debug: openUI( %d ), isOpenUI -> %d", iUIID, pManager->isOpenUI( iUIID ) ? 1 : 0 );
}

void GfxLoadWidget_LUA( int iUIID, const char* szName )
{
	CKTDGGFXManager* pManager = GfxDebugManager();
	if ( pManager == NULL || szName == NULL )
		return;
	// as CX2GFXEventScene::Play ( 3226b130 ): GetPullPath on the name, then loadUserWidget( id, full path, 0, 0 )
	WCHAR wszName[MAX_PATH] = { 0, };
	MultiByteToWideChar( CP_ACP, 0, szName, -1, wszName, MAX_PATH );
	WCHAR* wszPath = g_pKTDXApp->GetDeviceManager()->GetPullPath( wszName );
	std::wstring wstrPath = ( wszPath != NULL ) ? wszPath : wszName;
	pManager->loadUserWidget( iUIID, wstrPath.c_str(), 0, 0 );
	GfxLog( "debug: loadUserWidget( %d, \"%s\", 0, 0 )", iUIID, GfxToUTF8( wstrPath.c_str() ).c_str() );
}

void GfxUnloadWidget_LUA( int iUIID )
{
	CKTDGGFXManager* pManager = GfxDebugManager();
	if ( pManager == NULL )
		return;
	pManager->unloadUserWidget( iUIID );
	GfxLog( "debug: unloadUserWidget( %d )", iUIID );
}

void GfxShow_LUA( bool bShow )
{
	CKTDGGFXManager* pManager = GfxDebugManager();
	if ( pManager == NULL )
		return;
	pManager->SetShow( bShow );
	GfxLog( "debug: GFx shown ( +0x108 ) = %d", bShow ? 1 : 0 );
}
#endif SERV_IRUHADEV_JUNE_GFX_DEBUG
#endif SERV_IRUHADEV_JUNE_GFX_MANAGER
