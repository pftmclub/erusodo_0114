#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_GFX_RUNTIME
#include "KTDGFXRuntime.h"

// Scaleform's own headers. Include dirs ( Libs\ExternalLib\Scaleform\Src and \Include ) are set per file in KTDXLIB_2010.vcxproj
// for this .cpp only, so no March #include resolves differently.
#include "GFx/GFx_Loader.h"
#include "GFxVersion.h"
#include <setjmp.h>

// ---------------------------------------------------------------------------------------------------------------------
// Link shims: ONE zlib and ONE libpng stay in the exe ( the client's ), Scaleform's are not linked ( G1 link record ).
//  - zlibstat.lib is zlib 1.2.3 built with ZLIB_WINAPI ( __stdcall, names "_deflate@8" ); libgfx's AMP code ( Amp_Message ) asks for the
//    plain cdecl names of zlib 1.2.7. Scaleform's zlib127.lib cannot be linked beside it: LNK2005 on the internal cdecl symbols
//    ( _tr_*, _dist_code, _length_code, _deflate_copyright ) and two zlib versions would share one set of them.
//    The six wrappers below forward to the client's zlib; /alternatename aliases libgfx's undecorated names to them.
//    z_stream has the same layout in 1.2.3 and 1.2.7, and deflateInit_ / inflateInit_ compare only the version's first character.
//  - libgfx's PNG reader ( libpng 1.5.13 headers ) asks for png_set_longjmp_fn and png_longjmp, which libpng 1.2.24 ( png.lib, CxImage's ) lacks.
//    In 1.2.24 the png_struct's FIRST member is the jmp_buf ( cximage/png/png.h:1137, PNG_SETJMP_SUPPORTED ), so both are two lines.
//    Known limit: the PNG reader still passes "1.5.13" to png_create_read_struct, and 1.2.24 rejects a differing major.minor, so a GFx
//    PNG file load fails cleanly ( the create returns NULL ). Nothing in G1 loads one; G2 / G3 re-check ( GFx content is .gfx / .swf ).
// ---------------------------------------------------------------------------------------------------------------------
extern "C"
{
	int __cdecl sfgfx_deflate( z_streamp strm, int flush )		{ return deflate( strm, flush ); }
	int __cdecl sfgfx_deflateEnd( z_streamp strm )				{ return deflateEnd( strm ); }
	int __cdecl sfgfx_deflateInit_( z_streamp strm, int level, const char* version, int stream_size )
																{ return deflateInit_( strm, level, version, stream_size ); }
	int __cdecl sfgfx_inflate( z_streamp strm, int flush )		{ return inflate( strm, flush ); }
	int __cdecl sfgfx_inflateEnd( z_streamp strm )				{ return inflateEnd( strm ); }
	int __cdecl sfgfx_inflateInit_( z_streamp strm, const char* version, int stream_size )
																{ return inflateInit_( strm, version, stream_size ); }

	jmp_buf* __cdecl sfgfx_png_set_longjmp_fn( void* png_ptr, void* /*longjmp_fn*/, size_t /*jmp_buf_size*/ )
	{
		return reinterpret_cast<jmp_buf*>( png_ptr );	// libpng 1.2.24: png_struct_def::jmpbuf is the first member
	}
	void __cdecl sfgfx_png_longjmp( void* png_ptr, int val )
	{
		longjmp( *reinterpret_cast<jmp_buf*>( png_ptr ), val );
	}
}
#pragma comment( linker, "/alternatename:_deflate=_sfgfx_deflate" )
#pragma comment( linker, "/alternatename:_deflateEnd=_sfgfx_deflateEnd" )
#pragma comment( linker, "/alternatename:_deflateInit_=_sfgfx_deflateInit_" )
#pragma comment( linker, "/alternatename:_inflate=_sfgfx_inflate" )
#pragma comment( linker, "/alternatename:_inflateEnd=_sfgfx_inflateEnd" )
#pragma comment( linker, "/alternatename:_inflateInit_=_sfgfx_inflateInit_" )
#pragma comment( linker, "/alternatename:_png_set_longjmp_fn=_sfgfx_png_set_longjmp_fn" )
#pragma comment( linker, "/alternatename:_png_longjmp=_sfgfx_png_longjmp" )

namespace
{
	Scaleform::GFx::System*		s_pGFxSystem			= NULL;
	bool						s_bFirstFrameLogged		= false;
}

void KTDGFXRuntime_Init()
{
	if ( s_pGFxSystem != NULL )
		return;

	// default SysAlloc and root heap settings; June's System setup is in its CKTDXApp ctor ( 31c17ed0 )
	s_pGFxSystem = new Scaleform::GFx::System();
}

void KTDGFXRuntime_Shutdown()
{
	if ( s_pGFxSystem == NULL )
		return;

	delete s_pGFxSystem;
	s_pGFxSystem = NULL;
}

bool KTDGFXRuntime_IsUp()
{
	return s_pGFxSystem != NULL;
}

void KTDGFXRuntime_FirstFrameLog()
{
	if ( s_bFirstFrameLogged == true )
		return;
	s_bFirstFrameLogged = true;

	// KLOG expands to two statements: the branches need braces
	if ( s_pGFxSystem != NULL )
	{
		KLOG( "ErrorLog.txt" ) << "[JUNE_GFX] RUNTIME Scaleform " << GFX_VERSION_STRING << " up" << fileout;
	}
	else
	{
		KLOG( "ErrorLog.txt" ) << "[JUNE_GFX] RUNTIME Scaleform " << GFX_VERSION_STRING << " NOT up" << fileout;
	}
}
#endif SERV_IRUHADEV_JUNE_GFX_RUNTIME
