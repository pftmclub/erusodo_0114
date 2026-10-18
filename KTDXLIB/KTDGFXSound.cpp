#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_GFX_MANAGER
// June GFx runtime, Phase G2: Scaleform's FMOD sound renderer. June's manager makes one on KTDS's FMOD system ( 31c43700 ) for GFx::Audio;
// the SDK ships it as source only ( Src/Sound, no lib ), while GFx::Audio itself is in libgfx.lib. The three SDK files are compiled here,
// under the flag, so a flag-off build compiles nothing of them. Per-file include dirs ( KTDXLIB_2010.vcxproj ): Scaleform's Src / Include
// and KTDXLIB\FMOD\inc ( the client's FMOD Ex 4.44.05, whose fmod.hpp the renderer includes as <fmod.hpp> ).
#include "../Libs/ExternalLib/Scaleform/Src/Sound/Sound_Sound.cpp"
#include "../Libs/ExternalLib/Scaleform/Src/Sound/Sound_SoundRenderer.cpp"
#include "../Libs/ExternalLib/Scaleform/Src/Sound/Sound_SoundRendererFMOD.cpp"
#endif SERV_IRUHADEV_JUNE_GFX_MANAGER
