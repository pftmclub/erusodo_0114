#ifdef SERV_IRUHADEV_JUNE_GFX_RUNTIME
#pragma once

// June GFx runtime, Phase G1 ( SERV_IRUHADEV_JUNE_GFX_RUNTIME ).
// Plain header: it must not pull in Scaleform, so KTDXApp.cpp and every other March file stay on March's own include paths.
// The Scaleform headers are included by KTDGFXRuntime.cpp only ( per-file include dirs in KTDXLIB_2010.vcxproj ).

// Scaleform::GFx::System up / down. June builds its GFx manager in the CKTDXApp ctor ( 31c17ed0 @31c19677 ); the System
// object has to exist before any GFx object, so G1 brings it up at that same spot and G2 builds the manager after it.
void KTDGFXRuntime_Init();
void KTDGFXRuntime_Shutdown();
bool KTDGFXRuntime_IsUp();

// Called from CKTDXApp::OnFrameMove: writes "[JUNE_GFX] RUNTIME Scaleform 4.2.22 up" to ErrorLog.txt once, at the first
// frame ( a line written while the koms mount never reaches ErrorLog.txt ).
void KTDGFXRuntime_FirstFrameLog();
#endif SERV_IRUHADEV_JUNE_GFX_RUNTIME
