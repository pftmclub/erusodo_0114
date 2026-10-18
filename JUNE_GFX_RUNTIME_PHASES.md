# June Scaleform GFx runtime: manager, root movie, event scenes, talk cut-ins

Plan key `june-gfx` · branch `main` · created 2026-10-17 (planning session of `/x2-phase new-plan quest-system`, split out on the
user's call) · log tag `[JUNE_GFX]` · flags `SERV_IRUHADEV_JUNE_GFX_*` · kickoffs `kickoffs/june-gfx/`.

## Context

The backlog's Stage 4 (quest system) and its "Open decisions" table gate June's GFx event scenes on a Scaleform runtime decision
(`JUNE_PORT_BACKLOG_ORDER.md`, "Stage 4" and "Open decisions that gate stages"). On 2026-10-17 the user supplied a Scaleform SDK
and decided:

1. **Split.** This runtime is its own plan, runnable now. The quest-system plan is written once Stage 1 (Atlas/Elysion) is closed
   and skill-system S1 has extracted June's tables; it builds on this plan's event-scene port
   (`kickoffs/june-backlog/stage4-plan.md`).
2. **Scope: event scenes and talk cut-ins.** June's root UI screens (`main.gfx` hosts them: cash shop, option, PvE/PvP result,
   bingo, job professional, tutorial guide) stay with backlog Stage 9. This plan loads `main.gfx` because the scenes and cut-ins are
   widgets inside it, and ports no screen.
3. **Guild quests: parse and log** (recorded here for the quest-system plan).

**The SDK** (`Libs/ExternalLib/Scaleform`, git-ignored like all of `Libs/`), checked 2026-10-17:

| Item | Fact |
|---|---|
| Version | GFx **4.2.22** (`Include/GFxVersion.h`). June's exe embeds **4.0.15** (its AS2 `gfxVersion` literal, `unpacked_x2.exe` file offset `0x1b10948`). Same 4.x line. |
| Libs | `Lib/Win32/Msvc10/Release/`: `libgfx`, `libgfx_as2`, `libgfx_as3`, `libgfxrender_d3d9`, `libgfxexpat`, `expat`, `libjpeg`, `libpng`, `zlib127`, `pcre`. Release only. |
| CRT | Every lib asks for `MSVCRT` = `/MD`; the client's three projects are `MultiThreadedDLL` under `US_SERVICE`. VC10, Win32: match. |
| Content | June's 509 kom `.gfx` (data152 / data194 / data195) are `CFX`/`GFX`/`CWS` v10 with exporter `0x401`, all AS3 by `FileAttributes` but one; `GFx_LoaderImpl.cpp:112` reads `0x401`'s code-offset table and `:227` has no version gate. 2,384 loose `eventscene\*.swf` are `CWS` v10 AS3; one does not inflate (G0 names it). |
| Not supplied, not needed | Video (June has no GFx video: no `.usm` / CRI / video player). IME lib (`SF_ENABLE_IME` is undefined in `GFxConfig.h:328`). FMOD sound renderer lib (June links one but registers no Audio state, sweep item 3). |

**June's runtime** (planning sweep 2026-10-17, `june2017_re/gfx_planning_sweep.md`, [D] seen / [I] inferred; image base `0x31aa0000`):
- `CKTDGGFXManager` (0x120 B) is created in the CKTDXApp ctor `31c17ed0` (`[esi+0x1c8]` @31c196be), ctor `31c45cc0`, init `31c456e0`
  (loader + state bag `31c451d0`, movie `31c465b0`, `main.gfx` via `31c3f3d0`); render `31c43b20` from CKTDXApp render `31be3800`
  @31be3c68 (state-block capture / advance / display / apply) unless `+0x108`; dt `31c430e0` from OnFrameMove; lost `31c43440`, reset
  `31c43e60`, resize `31c430c0`; input: main loop `31c43ab0`, MsgProc mouse `31c44040` first, keys / char / IME `31c43890` after the
  stages (a 1 return swallows the message; `+0x115` = scene playing -> keys ignored).
- File opener `r3dGFXFileOpener` (vtable `33446d2c`, OpenFile `31c47a00`): mass-file lookup `31c4c170`, algo-2 decode, a memory file.
  No loose fallback was seen [I], but the scene `.swf` ship loose in June: G0 settles where they come from.
- `fontconfig.font` (UTF-16, `[FontConfig "US"]`, 1.4 MB, data036) holds the `GFX_ID_n` text; `fontconfig.txt` is a KR_OPEN_TEST
  copy June does not load.
- 14 ActionScript wrappers over `31c3f150` (Invoke): `openUI`, `isOpenUI`, `closeToTopZorder`, `moveToTopZorder`, `unloadWidget`,
  `unloadUserWidget`, `loadUserWidget`, `loadEventSceneEndFrame`, `clearFocus`, `changeCharacter`, `showOkPopup`, `ShowUI`, `HideUI`,
  `isVisibleI`. Most screens bypass them (13 adapter / callback pairs, ~39 `DW_*` wrappers): Stage 9.
- **Event scenes:** `CX2GFXEventScene` at `g_pData+0x274` (ctor `3226aed0`, made by `31edc620`), data from `GFXEventSceneInfo.xml`
  (`31eed7a0`); Play `3226b130`, queue `3226bff0`, End `3226b6c0`, IsPlaying `3226a010` (= GFx flag or the Lua scene's `325ae1e0`).
  Callers: the Lua binding `PlayEventScene` `327ccad0` and the quest UI `323cb2e0` (-> quest-system). March's Lua `CX2EventScene`
  stays alive at `+0x278` (June keeps both).
- **Talk cut-ins:** `31d76b40` builds `CUT_IN_TALK_*` / `Common_CutIn_Frame` for UI id 0xc and plays it through
  `SimpleWidgetPlayer.playSimpleWidget` `31fc75e0`. **Skill cut-ins stay textures in June** (`32014fc0` -> `SkillCutIn` `32078b70`);
  `GFX_SKILL_CUT` in `CharCustomData` (`326525d0`) has no consumer found (Stage 12 note).

**Reach today.** The scene XMLs (`GFXEventSceneInfo.xml` + 11 region files, data036) key 493 scenes by quest: 439 are `_ROSE_`, 4 Rose
tutorial, 21 `_AIN_` (Stage 8 characters), and the rest are the Elysion quests 22329-22340 (quest-system + Elysion). So every code
phase here is proven through a debug Lua hook until those stages land; that is the plan's design, not a gap.

## Rules

1. **June wins; no partial updates; June bugs verbatim** with a `JUNE_BUGS.md` row. Cite June addresses; never port raw offsets.
2. **Flags.** One `SERV_IRUHADEV_JUNE_GFX_*` flag per logical change in `KTDXLIB/Always.h`, original code in `#else`,
   `#endif FLAG`. **The libraries come in through `#pragma comment( lib, ... )` inside the flag** (the `KTDXApp.cpp:12-36` pattern),
   so flag-off links what it links today. Project-file edits (include / library directories) are inert when the flag is off; add
   Scaleform's include dirs **per file** on the new GFx `.cpp` files only, so no March `#include` resolves differently. Each Done note
   lists its `.vcxproj` hunks, since `flagcheck` does not read project files.
3. **Data.** June's GFx files go in **byte-exact** (memory `june-assets-byte-exact-no-edits`), into the kom numbers June uses, decoded
   with `tools/junekom.py` (never elstools: it truncates June algo-2). No `.lua` is invented: the debug hook is a C++ binding typed at
   the dev console. `/elstools pack`, then ask before every `/x2-kom-packed copy`.
4. **Asset-port coordination.** These files are also in `JUNE_ASSET_PORT_PHASES.md`'s waves (new kom numbers in P2/P3, `eventscene\`
   in P9). Phase GA stages only what this plan needs, byte-exact at June's own kom number. The asset port's P0 census compares
   md5s against the staged files, so those waves then count them as same bytes. GA's Done note lists what it staged.
5. **Offline only.** No server code; the emulator is touched only if a scene packet turns up (none is expected).
6. **Opus phases delegate** reads, greps, builds and log reads (memory `opus-phases-delegate-mundane-work`).
7. **Never launch `X2_offline.exe`**: ask the user.

Flags (debug: `SERV_IRUHADEV_JUNE_GFX_DEBUG`): `_JUNE_GFX_RUNTIME` (G1), `_JUNE_GFX_MANAGER` (G2), `_JUNE_GFX_EVENTSCENE` (G3),
`_JUNE_GFX_TALK_CUTIN` (G4), `_JUNE_GFX_GATES` (G5). Log tag **`[JUNE_GFX]`**, sub-tags `RUNTIME`, `MGR`, `SCENE`, `CUTIN`, `GATE`:
`python tools/logs.py --tag JUNE_GFX`. Funcmap tag `GFx <phase>`.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "<heading>"` |
| The planning sweep | `june2017_re/gfx_planning_sweep.md` (78 lines, read whole) |
| Ghidra | `ghidra` skill (read-only in G0 and reviews) |
| June files | `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\` (`data152_kom`, `data194_kom`, `data195_kom`, `data036_kom`, `eventscene\`); `python tools/junekom.py list / extract` for byte-exact copies |
| Pack / copy | `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --tag JUNE_GFX` |
| Dev console | auth level 3 (`python tools/elsdb.py setting auth_level 3`), END key |

## Phases

| Phase | What | Run with | Code | Play-test |
|---|---|---|---|---|
| G0 | RE: close the sweep's [I] items into a port map (manager layout, render split, input, opener, scene + talk-cut-in classes, AS2/AS3, every `+0x274` reader) | Opus 5.5 high | no | no |
| G1 | SDK link + `Scaleform::System` up/down; prove the link (dupes with CxImage's jpeg/png/zlib) | Sonnet 5.5 high | yes | regression only |
| GA | Stage the GFx assets byte-exact (data194, data152, data036 XML + `fontconfig.font`, the scene `.swf`) | Sonnet 5.5 high | data | no |
| G2 | `CKTDGGFXManager` whole: loader, opener, fonts, D3D9 HAL, `main.gfx`, render / dt / lost / reset / resize, input, the 14 wrappers; debug hook | Opus 5.5 high + review | yes | yes |
| G3 | `CX2GFXEventScene`: XML loader, Play / queue / End / IsPlaying, `PlayEventScene` binding, input block, text + voice | Sonnet 5.5 high + Opus review | yes | yes (debug hook) |
| G4 | Talk cut-ins: `31d76b40` and its callers, `playSimpleWidget` | Sonnet 5.5 high + Opus review | yes | yes |
| G5 | The gates the backlog owes: `GetShowActiveSkillShow`'s GFx term, C3p-2's ESC gates that read the manager | Sonnet 5.5 high + Opus review | yes | yes |
| Z | Close-out | Sonnet 5.5 medium | no | no |

**Model choice.** Opus for G0 (a wrong reading of the render or input order costs every later phase) and for G2 (a new engine
mechanism on the shared D3D9 device: render-state leaks and device-reset crashes are the failure mode). Sonnet for G1 / GA (mechanical,
with a written gate) and for G3-G5 (transcription from G0's map), each code phase with a scoped Opus review.

**Order and parallelism.** G0, G1 and GA can run now and together (G0 read-only, G1 code, GA data). G2 needs all three. G3, G4 and
G5 need the G2 review and run one at a time (build lock, shared `Always.h` block). G1 and G2 edit `KTDXApp.cpp`: not alongside
june-engine U1. Nothing here depends on Atlas/Elysion or the skill system.

## Phase G0 - Port map

**Run with:** Opus 5.5 high, read-only. Output `june2017_re/g0_gfx_map.tsv` + funcmap rows `GFx G0`; amend the G1-G5 kickoffs.

1. `CKTDGGFXManager` member layout (0x120 B): every offset its methods read or write, named; the ctor / init / dtor order; which
   states the loader gets (`31c451d0`: opener id 8, AS2Support id 0x21, clipboard, the Win32 object id 0x11, IME id 0x17, font
   config). **Settle AS2 vs AS3:** the content is AS3 by header, the sweep saw AS2Support only; find where AS3 support is registered,
   or prove the root movie runs under AS2. G1 links what this says.
2. The render path inside `31c43b20` (`31c3faf0` / `31c3f6e0`): advance vs display, viewport, the state block, where in CKTDXApp's
   render it sits relative to March's UI and the cursor, and what `+0x108` is.
3. Input: `31c43ab0`, `31c44040`, `31c43890` in full; what returns 1; the `+0x115` set / clear sites.
4. The opener `31c47a00` / `31c4c170`: does it fall back to disk, and how does June's `CX2GFXEventScene` reach `eventscene\X.swf`
   (a loose path, a kom entry, a `loadUserWidget` URL resolved by `main.gfx`)? Name the one `.swf` that does not inflate.
5. `CX2GFXEventScene` whole: ctor, loader `31eed7a0` (only `GFXEventSceneInfo.xml`, or the 11 region XMLs too, and how), Play /
   queue / End / IsPlaying, the callback that reports the end frame, how `soundKey` (`GFX_ID_n.ogg`) is played and where those files
   live, how `textKey` / `characterNameKey` are resolved (`fontconfig.font` `tr` lines), and the `PlayEventScene` binding `327ccad0`'s
   arguments.
6. Classify all 32 `g_pData+0x274` readers and the 11 `+0x278` readers: March system present (port in G3), quest UI (quest-system),
   root-UI screen (Stage 9), absent system (log). Same for the callers of the 14 wrappers that this plan's scope touches.
7. Talk cut-ins: `31d76b40`'s callers and arguments (who shows a talk cut-in, in which game state), `31fc75e0`.
8. The owed gates: `31fd3180 @31fd3228` (`isOpenUI(0xb)`, and whether the `g_pMain+0x80a` movie term is the AVI movie table, not
   GFx), and C3p-2's `31f1e1a0` gates (`closeToTopZorder` @31f1e262, the result-UI mode `+0x210`, the profession-manager close,
   hide modes 2-4): which G5 ports and which go to Stage 9 or the quest-system plan.

**Done gate:** every row in the map is [D] or marked open with a reason; G1-G5 kickoffs amended; no open question that blocks G1.

### Phase G0 - Done (2026-10-19, Opus 5.5 high; read-only: no code, no flag, no build)

**Output.**
- `june2017_re/g0_gfx_map.tsv`: about 95 rows. Every row is `D` except 7 that are `open`, each with its reason and owner phase:
  `+0x18`, `+0x8c`, `+0xd8`, `+0x118`, `31d71810`, `31efccf0`, `31f0bdd0`.
- 27 funcmap rows tagged `GFx G0`.
- Kickoffs amended: G1, GA, G2, G3, G4, G4-review, G5, G5-review. Each ends in "Amended by G0".
- Scratch evidence (session scratchpad, not committed): decompiles, the `+0x274` / `+0x278` census, the manager-member scan.

**Settled.**
1. **AS2 vs AS3: both are registered; content is AS3.**
   - June sets AS2Support (0x21), then AS3Support (0x22: `33193b90` registers the SymbolClass / DoAbc tag loaders).
   - `main.gfx` is `gfxAS3.*`.
   - G1 links `libgfx_as2` + `libgfx_as3`.
2. **Scene `.swf`: loose, through `GetPullPath`.**
   - Play resolves `X.swf` and `Tutorial_Back.swf` with `31c4e7f0`, which is March's `GetPullPath` (every cwd subfolder is a data
     dir), then calls `loadUserWidget`.
   - The opener `31c47a00` tries the kom first, then falls back to the SDK's disk `OpenFile`. That corrects the sweep's "no loose
     fallback".
   - The one `.swf` that does not inflate is `quest_22254_rosestart.swf`: a CWS header over an uncompressed body, named by no XML.
3. **The 11 region XMLs are loaded.**
   - `31eed660` reads `VillageName.xml` (data036, 428 B, 11 names), then each `<NAME>.xml` into `g_pData +0x4ac`.
   - `GFXEventSceneInfo.xml` (`31eed7a0`) goes into the scene's `+0xc0` map.
   - The 493 rows are 442 region + 51 GFX.
4. **Movie term: `g_pMain +0x80a` is June's AVI DungeonClearMovie system, not GFx.** March has none. G5 leaves it out, and a backlog
   row owns it.

**Corrections to the planning sweep / Context.**
- `+0x108` means **GFx shown** (render when `== 1`), not "hide".
- `+0x40` is Renderer2D, and the Loader is embedded at `+8`.
- June **does** set an Audio state: an FMOD SoundRenderer on KTDS's FMOD system (`31c43700`). The SDK has it only as `Src/Sound`
  source.
- Keys (`31c43890`) run after the stage chain **even when a stage handled the message**.
- `+0x84` is the current CX2State. It receives AS "MSG" and the IME / key forwards as MsgProc 0x467 / 0x46c / 0x46d.
- **"Talk cut-ins" are June's user skill cut-ins.** `31d76b40`'s only caller is `31fd95a0` = CX2GUUser::ActiveSkillShow. That
  plays `<cut-in>.gfx` plus `Common_CutIn_Frame` plus a random `CUT_IN_TALK_L/R_1-3`, and no particle cut-in. NPCs keep textures.
  G4 is rescoped to that, and GA adds data195.
- The SDK has no FxPlayer FontConfigParser and no Win32 IME. G2 transcribes June's parser and skips IME; June ships no `ime.xml` /
  `IME.swf` either.
- 11 of the briefed `+0x274` / `+0x278` sites read other objects. The real counts are 26 and 6.

**Routing.**
- **G3:** the scene class, both Lua bindings (`PlayEventScene`, `IsRecentlyPlayedEventScene`), and the readers in StateChange,
  MsgProc, TFieldGame, DungeonGame, Informer, TalkBox, Add and DungeonGame::Release. JUNE_BUGS 253 is fixed by the GFx scene.
- **G4:** the ActiveSkillShow GFx body and its 8 callers.
- **G5:** `isOpenUI(0xb)` (cash-shop UI: false until Stage 9), `closeToTopZorder`, and the R8-review hide-mode mechanism whole (map
  `+0xd0`, presets `32597410`, readers `31d06100` / `31d05b80`).
- **quest-system:** `323cb2e0`, `323e4cb0`.
- **Stage 9:** hide-mode entry points 2-5, the result-UI mode, the profession close, the tutorial guide, the other wrapper callers.
- **Logged absent:** CX2GuildAgitGame.

**Data found (for GA).**
- 2,385 loose `eventscene\*.swf` (318 MB).
- `main.gfx` + 22 files in data194; 426 `.gfx` in data152; 60 in data195.
- `fontconfig.font` (US) has keys `GFX_ID_0..9132`. 384 Elysion text references (Quest_22337-22346) use missing keys, as in June:
  that is a JUNE_BUGS row for G3.
- 2,793 of the 2,876 `soundKey` files are in June koms 179 / 188 / 214 / 174 / 186 (+ small). 83 are missing in June too.

**How to see it in game:** nothing (read-only). **Open questions for the user:** none block G1. G4's widening is June's behaviour.
Its play-test will show GFx cut-ins in place of March's particle ones.

## Phase G1 - SDK link and system init

**Run with:** Sonnet 5.5 high. Flag `SERV_IRUHADEV_JUNE_GFX_RUNTIME`. Sub-tag `RUNTIME`.

- Add the Scaleform library directory to `KTDXLIB` / `X2` (every US configuration), inert. Add `Src` + `Include` as per-file include
  dirs on the new GFx `.cpp` files only (Rules 2).
- Under the flag, `#pragma comment( lib, ... )` for the libs G0 item 1 names (expect `libgfx`, `libgfx_as3` and/or `libgfx_as2`,
  `libgfxrender_d3d9`, `libgfxexpat`, `expat`, `pcre`). **Duplicate symbols:** KTDXLIB's CxImage may carry its own jpeg / png / zlib.
  Link Scaleform's `libjpeg` / `libpng` / `zlib127` only if the link needs them; on an `LNK2005`, keep one copy and record which in
  the Done note.
- `Scaleform::System` init (with a GFx heap) in the CKTDXApp ctor where June builds the manager, shutdown in the dtor. One use-time
  line (first frame, not mount time: memory `errorlog-loses-mount-time-lines`): `[JUNE_GFX] RUNTIME Scaleform 4.2.22 up`.

**Done gate:** flag on and off both link; exe size delta in the Done note; the line in the log; a village / dungeon regression shows
no change.

### Phase G1 - Done (2026-10-19, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_GFX_RUNTIME`)

**What landed.**
- `Always.h`: the flag block. `KTDXApp.cpp`: under the flag `#pragma comment( lib )` for `libgfx`, `libgfx_as2`, `libgfx_as3`, `libgfxrender_d3d9`, `pcre`
  (G0 item 1: both AS libs); `KTDGFXRuntime_Init()` in the CKTDXApp ctor (June ctor `31c17ed0` @`31c19677` builds its manager there),
  `KTDGFXRuntime_Shutdown()` in the dtor, `KTDGFXRuntime_FirstFrameLog()` at the top of `OnFrameMove`.
- New `KTDXLIB/KTDGFXRuntime.{h,cpp}`: a `Scaleform::GFx::System` (default SysAlloc and root heap), the one-time line
  `[JUNE_GFX] RUNTIME Scaleform 4.2.22 up` to ErrorLog.txt at the first frame, and the link shims below. The `.h` has no Scaleform include.
- **`.vcxproj` hunks** (flagcheck does not read them; all inert with the flag off):
  `X2/X2_2010.vcxproj` `LibraryPath` of `US_INTERNAL` and `US_SERVICE`: `$(SolutionDir)\Libs\ExternalLib\Scaleform\Lib\Win32\Msvc10\Release;` prepended.
  `KTDXLIB/KTDXLIB_2010.vcxproj`: the same prepend on both `LibraryPath` lines (the lib is static, so only X2's counts); a new
  `<ClCompile Include="KTDGFXRuntime.cpp">` whose `AdditionalIncludeDirectories` is `...\Scaleform\Src;...\Scaleform\Include` (this file only,
  all configs, so no March `#include` resolves differently); `<ClInclude Include="KTDGFXRuntime.h" />`.

**Link record (the "keep one copy" rule).** `libgfx` alone left 8 unresolved: `deflate / deflateEnd / deflateInit_ / inflate / inflateEnd / inflateInit_`
(AMP profiler code, `Amp_Message`) and `png_longjmp / png_set_longjmp_fn` (PNG reader). The client carries **zlib 1.2.3 as `zlibstat.lib` built
`ZLIB_WINAPI` (names `_deflate@8`)** and **libpng 1.2.24 (`png.lib`, CxImage's, used for PNG screenshots)**; Scaleform ships zlib 1.2.7 (cdecl) and
libpng 1.5.13. Tried and rejected: linking `zlib127.lib` = LNK2005 on `_tr_*`, `_dist_code`, `_length_code`, `_deflate_copyright` (two zlib versions
would share one set of internals); `libpng.lib` beside `png.lib` would pair 1.5 functions with a 1.2.24 `png_struct`. **Kept: the client's zlib, png and
jpeg as the one copy; Scaleform's `zlib127`, `libpng`, `libjpeg`, `libgfxexpat`, `expat` are not linked.** The 8 symbols come from shims in
`KTDGFXRuntime.cpp`: six cdecl wrappers forwarding to zlibstat (`/alternatename:_deflate=_sfgfx_deflate` ...; `z_stream` has the same layout in 1.2.3 and
1.2.7, and `deflateInit_` / `inflateInit_` check only the version's first character), and two PNG functions over 1.2.24's `png_struct` (its first member is
the `jmp_buf`, `cximage/png/png.h:1137`). **No `Sound::` symbols were left unresolved**, so the three `Src/Sound` files are not compiled in G1; G2 creates
the renderer and may need them (G0's note stands).
**Known limit, for G2 / G3:** libgfx's PNG reader passes `"1.5.13"` to `png_create_read_struct`, and libpng 1.2.24 rejects a differing major.minor, so a
GFx PNG *file* load fails cleanly (NULL). `.gfx` / `.swf` content does not go through it; AMP's compress path is never called. If G2 / G3 meet an
external PNG, the fix is a renamed private libpng, not these shims. JUNE_BUGS: none (nothing of June's is copied).

**Hygiene.** `flagcheck JUNE_GFX_RUNTIME --worktree --files KTDXLIB/Always.h KTDXLIB/KTDXApp.cpp KTDXLIB/KTDGFXRuntime.{h,cpp}`:
`files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 1 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`.
The one FAIL is `KTDGFXRuntime.cpp`'s `#include "StdAfx.h"`: a PCH-using `.cpp` must include it first, outside the flag (the flag is only visible
after it), so with the flag off the file is an empty translation unit. Everything else in the file is inside the flag.

**Deploy.** `DEPLOY OK: X2_offline.exe is 21,188,608 B, dated 2026-10-19 04:11:19, md5 bf47e88ac5b34d7d0b370f89aaa964ba, equal to X2/US_SERVICE/x2.exe
(2026-10-19 04:11:13); built at main 879eb555 + 6 uncommitted code file(s).` The strings `[JUNE_GFX] RUNTIME Scaleform ` and `4.2.22` are in the exe.
**Not measured:** a flag-off build and the exe size delta (no flag-off exe exists; a second full build would hold the shared queue ~11 min). The flag-off
equivalence rests on flagcheck (`KTDXApp.cpp` reduces to the base byte-for-byte; the new `.cpp` is empty) and on the libs being named only inside the flag.
The first build of this phase took 3 attempts: a `KLOG` in an `if/else` without braces (it expands to two statements), then the link errors above.

**How to see it in game.** Start the game and reach the first frame: `ErrorLog.txt` holds `[JUNE_GFX] RUNTIME Scaleform 4.2.22 up`
(`python tools/logs.py --tag JUNE_GFX`). Nothing else changes on screen. **Open questions:** none.

## Phase GA - GFx assets in, byte-exact

**Run with:** Sonnet 5.5 high. No code, no flag.

- `data194` (new kom number: `main.gfx`, the font libs, and the screens it imports, all of it byte-exact because the root movie
  imports them), `data152` (talk cut-ins and the cash skill cut-ins together, as June ships them), and `data036`:
  `GFXEventSceneInfo.xml`, the 11 region XMLs, `VillageName.xml`, `fontconfig.font` (June-only names; ask before replacing anything
  March has under the same name).
- The scene `.swf`: wherever G0 item 4 says June loads them from (loose `eventscene\` or a kom).
- `data195` (transcendence + Rose cut-ins) is not needed here: leave it to the asset port.
- Pack, ask, copy. List each staged file in the Done note (Rules 4).

**Done gate:** `PACK OK` / `COPY OK` lines; md5 of three sample files equal to June's `junekom.py` extract.

### Phase GA - Done (2026-10-19, Sonnet 5.5 high; data only: no code, no flag, no build)

**Staged** with `tools/kom.py stage-june` (June's strict decoder, byte-exact; no commit needed for `kom_contents`, it is outside the repo).
Full list, 4,645 rows: `june2017_re/ga_staged_files.tsv` (name, kom, size, state). Counts by kom:

| Kom | Files | What | Live state before |
|---|---|---|---|
| data194 | 131 | the whole kom: `main.gfx`, screens, `fonts_*.swf`, `gfxfontlib.gfx` + their `.dds` | new |
| data152 | 1,428 | 426 `.gfx` (skill cut-ins, `Common_CutIn_Frame`, 6 `CUT_IN_TALK_*`) + 1,002 `.dds` | new |
| data195 | 279 | 60 `.gfx` (transcendence / Rose cut-ins) + 219 `.dds` | new |
| data036 | 14 | `GFXEventSceneInfo.xml`, 11 region XMLs, `VillageName.xml`, `fontconfig.font`; `Ranox.xml` already identical (not rewritten); `fontconfig.txt` left out | +13 |
| data179 | 1,651 | voice `.ogg`; `GFX_ID_5342` / `5344` already identical | +1,649 |
| data188 / 214 / 174 / 186 | 863 / 138 / 54 / 28 | voice `.ogg` (186 = `Tuto_*`) | new |
| data070 / 078 / 178 | 11 / 7 / 6 | voice `.ogg` | +11 / +7 / +6 |
| data167 / 168 / 171 / 173 / 175 / 176 | 10 / 10 / 3 / 2 / 4 / 6 | voice `.ogg` | new |

- Voice list = unique `soundKey` of `GFXEventSceneInfo.xml` + the 11 region XMLs: 2,878 names, **2,793 staged**; 85 not in any June kom
  (83 files + 2 blank lines) -> `june2017_re/ga_missing_sound_keys.txt`. They are missing in June too: not a GA gap.
- **Mount range:** `X2Main.cpp:802` `SERV_IRUHADEV_KOM_FILE_COUNT_226` (defined `Always.h:3913`) mounts `data001`..`data226`, so every new number
  (152, 167, 168, 171, 173, 174, 175, 176, 186, 188, 194, 195, 214) is covered. No client change.
- **Name collisions:** none replaced (0 replaced, 3 identical: `Ranox.xml`, 2 voice files). Nothing needed asking.
- **Scene `.swf`:** all 2,385 June `eventscene\*.swf` (309 MB) are now loose in `...965799\data\eventscene\`: 2,383 copied, `ranox.swf` and
  `npc_ranox_png.swf` identical (skipped), `tutorial_back.swf` and `quest_22254_rosestart.swf` included. A full md5 + size verify of all 2,385: 0 failures.
  The dir now also holds March's `assets.txt` (2,386 entries).

**Gate lines** (pack and copy, additions only; the `~0 changed, -0 removed` diff was checked for the 5 existing koms):
- `PACK OK` x18: data036 070 078 152 167 168 171 173 174 175 176 178 179 186 188 194 195 214.
- `COPY OK data194: ...\data194.kom 32764543 bytes, 131 entries, md5 c78c49acc1cbdc469565461eccec5643`
- `COPY OK data152: ... 160531668 bytes, 1428 entries, md5 8a300dbfcd09573f2a0ccd717aa99929`
- `COPY OK data036: ... 13841577 bytes, 256 entries, md5 4b1fa6107e63eb2a967e79d9989c0530` (all 18 `COPY OK` lines were printed; the rest follow the same shape).
- **Sample md5 = `junekom.py extract`** (4 files, all equal to the staged file): `main.gfx 308dd6945995b051b4fae0a9c0d62a10`, `fontconfig.font 6eb65292e7ac4de4a0b06c9abc422696`, `GFX_ID_2371.ogg 2ae6e003701b84d554ee4549c6cf2008`, `Balloon_I3.dds d48dee6e8ab98b577b7b19d3dcbd143b`.

**Notes.** The pack's "23 stale `X.src.lua`" warning on data036 is a peer's pre-existing state; the diff vs live shows only the 13 added names, so no Lua was repacked.
A kom backup of each overwritten live kom is in `kom_backup_tools/`. **How to see it in game:** nothing yet (data only). Starting the game with these
koms mounted is the only regression check (a longer mount); G1 will exercise them. **Open questions:** none.

## Phase G2 - The GFx manager

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_MANAGER`. Sub-tag `MGR`.

- Port `CKTDGGFXManager` from G0's map: ctor / init / dtor; loader, state bag, `r3dGFXFileOpener` over `KGCMassFileManager`
  (`LoadDataFile`, so March's loose-first rule applies the same way), `fontconfig.font` "US", the D3D9 HAL on the app's device;
  `main.gfx` as the root movie.
- Hook it where June does: CKTDXApp ctor (`+0x1c8`-equivalent member), render (state block capture / apply), OnFrameMove dt, lost /
  reset / resize, the main loop and MsgProc input order with June's swallow rule.
- The 14 ActionScript wrappers, all of them (the screens that call most of them are Stage 9, but the wrappers are the manager's own).
- Debug hook under `SERV_IRUHADEV_JUNE_GFX_DEBUG`: a C++ Lua binding, e.g. `JuneGfxOpenUI( id )` / `JuneGfxLoadWidget( name )`,
  typed at the dev console.

**Done gate:** `[JUNE_GFX] MGR main.gfx loaded` with the font config name; a debug-loaded widget draws over the village and goes
away; Alt-Tab and a resolution change survive with the movie up; mouse and keys reach March's UI when no GFx widget is open; no
render-state leak (the world and March's UI look the same before and after).

### Phase G2 - Done (2026-10-19, Opus 5.5 high; flags `SERV_IRUHADEV_JUNE_GFX_MANAGER` + `SERV_IRUHADEV_JUNE_GFX_DEBUG`)

**What landed** (June addresses per item in `june2017_re/funcmap.tsv`, tag `GFx G2`, 15 rows).
- New `KTDXLIB/KTDGGFXManager.{h,cpp}`: June's manager (RTTI `CKTDGGGFXManager`) behind a plain header. In it, transcribed from Ghidra:
  the loader states (`31c451d0`), `r3dGFXFileOpener` (kom member first through a new kom-only `KGCMassFileManager::LoadDataFile_MassFileOnly`,
  then the SDK disk opener), `FxPlayerTextClipboard`, `GFxPlayerLog` (debugger output; mirrored to ErrorLog under `_DEBUG`), AS2 + AS3, four image
  handlers, `FontProviderWin32`, the FMOD sound renderer on KTDS's FMOD Ex system, June's FxPlayer font-config parser over `fontconfig.font`
  (pick / apply / `FontTranslator`), the D3D9 HAL + Renderer2D, the movie wrapper `CKTDGGFXUI` with `main.gfx` as root, the
  `CLIKDataBindingAdapter` listener map and the manager's AS callback (`MSG`, `isShippingBuild`, `DataStringTable`, `RegisterMainUI`,
  `NowCounTry`, `NowServiceMode`), the input path (main-loop filter, IME events, mouse, keys, Enter / arrows) and all 14 AS wrappers.
- New `KTDXLIB/KTDGFXSound.cpp`: the SDK's three `Src/Sound` files (GFx::Audio is in libgfx; the FMOD renderer ships as source only).
- Hooks: `KTDXApp` ctor (manager after the System) / dtor (before it), `JuneOpenPerturbationMap` = Init, OnFrameMove end = dt + the held init
  lines, OnFrameRender before `JuneStageChainRenderAfter` (state block Capture / Apply around BeginFrame / Display / EndFrame), reset
  (after the stage loop) / lost (after the full sync), MsgProc (mouse first; stages; keys always after), DXUT's live PeekMessage loop,
  `CX2Main::StateChange` (clear before RemoveStage, set after InsertStage), `CX2StateServerSelect` ctor (mouse input on), a no-op
  `CKTDXStage::JuneGfxSetMovieUI` (vslot 0x2c). Dev-console binds `JuneGfxOpenUI` / `JuneGfxLoadWidget` / `JuneGfxUnloadWidget` / `JuneGfxShow`.
- Link (the G1 "one copy" rule kept): `AS3_Global.h` included once (the app defines the AS3 class table); `expat.lib` linked (AS3 XML; the
  client has no expat); `inflateReset` / `compress2` shimmed over zlibstat like G1's six. `.vcxproj` hunks (`KTDXLIB_2010.vcxproj`, inert flag
  off): `KTDGGFXManager.cpp` (Scaleform `Src` + `Include`), `KTDGFXSound.cpp` (the same + `Src\Sound` + `$(ProjectDir)FMOD\inc`), `KTDGGFXManager.h`.

**Stand-ins and left-outs (labelled in code).** Dialog-manager `+0x110` (June-only modal byte) read false. The resize hook `31be2a00` has no
March point: DXUT's present params are fed at reset. App `+0x28c` (IME text-field id) lives on the manager, 0 until Stage 9's
`getSelectedIMETextFieldObjectID` (`32664210`) sets it. `+0x118` stays 0 (its writer keys off June's Axeso5 `_LA` byte; no reader). The IME
manager is skipped with a log line (SDK without IME, no `ime.xml` / `IME.swf`). The wrapper's "CharacterView" render target and CX2State's
vslot-0x2c handler list are Stage 9's. The four image handlers have no RTTI: taken as JPEG / PNG / TGA / DDS [I]. Message ids are June's values:
0x467 (March `KM_UI_CONTROL_CUSTOM_EVENT`), 0x46c / 0x46d (no March handler: inert until Stage 9). JUNE_BUGS 323-324.

**Hygiene.** `flagcheck JUNE_GFX_MANAGER --worktree --with _DEBUG --files <the 12 code files>`: `files: 8 PASS, 0 PASS~, 0 DEFINE, 2 OTHER,
2 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. The FAILs are
the PCH `#include "StdAfx.h"` of the two new .cpp (empty units with the flag off, as G1). OTHER = a peer's uncommitted S7e lines in `Always.h`
and `X2Main.cpp` (not in this commit). The first build stopped in S7e's `X2UISkillTreeNew2015.cpp` (theirs, rebuilt after their fix); the
second on 17 link externals (fixed above).

**Deploy.** `DEPLOY OK: X2_offline.exe is 25,098,240 B, dated 2026-10-19 05:27:48, md5 f9d277ac5a539d97ea741547ff065b5e, equal to
X2/US_SERVICE/x2.exe (2026-10-19 05:27:40); built at main f35c1c08 + 20 uncommitted code file(s).` (+3.9 MB over G1: the AS3 class set.)
Strings `[JUNE_GFX] MGR `, `main.gfx`, `fontconfig.font`, `GFXManager`, `JuneGfxLoadWidget` are in the exe.

**How to see it in game.** ErrorLog at the first frame: `[JUNE_GFX] MGR main.gfx loaded, font config "US" ( 1 fontlib fonts_Tahoma.swf, 1 map,
9133 tr )` plus the sound / IME lines. In a village, `JuneGfxLoadWidget( 10002, "Tutorial_Back.swf" )` draws the GFx widget over the world,
`JuneGfxUnloadWidget( 10002 )` removes it. **Open questions:** none.

**Play-test PASSED 2026-10-19** (user: all five checks; exe md5 f9d277ac). ErrorLog: the `main.gfx loaded` line above, `sound: FMOD renderer up`, `IME manager skipped`; main.gfx's own callbacks `NowCountry 4` / `NowServiceMode 3` and its start-up `tutorial_guide.gfx` widget; widget 10002 loaded and unloaded twice (Alt-Tab and resolution change with it up). The SDK's `Loader failed to open ...Tutorial_Back.gfx` is its .gfx-before-.swf probe (the .swf then opens); the Scale9Grid warnings are main.gfx content, visible through the `_DEBUG` log mirror. 0 EXCEPTION, baseline UNHANDLED only, clean exit.

## Phase G3 - Event scenes

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE`. Sub-tag `SCENE`.

- Port `CX2GFXEventScene` whole from G0 item 5 onto `CX2Data` (June's `+0x274`; March's Lua `CX2EventScene` stays): loader, Play /
  queue / End / IsPlaying, the end-frame callback, the input block (`+0x115`), text from `fontconfig.font`, voice.
- June's `PlayEventScene` binding `327ccad0`, and every `+0x274` / IsPlaying reader G0 classified as "port in G3" (no partial update:
  June's IsPlaying covers both scene classes, so March's readers of the Lua scene's IsPlaying change with it).
- Quest-UI callers (`323cb2e0` and friends) are the quest-system plan's: log nothing, port nothing, note them in the Done note.

**Done gate:** at the dev console, `PlayEventScene` with `Quest_22329_Start` plays Elysion7's scene with Elsword's lines and voice,
keys are ignored while it plays, it ends cleanly, and the Lua scenes March already plays (if G0 finds a reachable one) still play.

## Phase G4 - Talk cut-ins

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN`. Sub-tag `CUTIN`.

- Port `31d76b40` and every caller G0 item 7 lists that exists in March, plus `playSimpleWidget` `31fc75e0`. Callers in absent
  systems are logged once.

**Done gate:** a talk cut-in shows in the state G0 names (or through the debug hook if no March state reaches it), left and right
variants, and closes.

## Phase G5 - Owed gates

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_GATES`. Sub-tag `GATE`.

- `GetShowActiveSkillShow`'s `isOpenUI(0xb)` term (`31fd3180 @31fd3228`) on top of C3t-1's port.
- The C3p-2 ESC gates G0 item 8 assigns here (`closeToTopZorder` at least). The result-UI mode and anything else that needs a Stage 9
  screen are written into the backlog's Stage 9 section, not ported.
- The movie term goes wherever G0 item 8 says (the AVI movie table is not GFx).

**Done gate:** with a debug-opened UI 0xb, the cut-in hides as June's option says; ESC closes the top GFx widget first; with no
widget open, ESC and cut-ins behave as before.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.
- `MODS.md` lines for every flag, with revert steps (the `.vcxproj` hunks included).
- Dated notes in `JUNE_PORT_BACKLOG_ORDER.md`: Stage 4's Scaleform decision answered (runtime, this plan); Stage 9 gets the root-UI
  screens, wrappers' callers and any gate G5 routed; Stage 12 gets `GFX_SKILL_CUT`; Stage 6 / 8 know the cut-in kom `data195`.
- Memory, and `SERV_IRUHADEV_JUNE_GFX_DEBUG` off unless a phase recorded a reason to keep it.

## Open questions (planning, 2026-10-17)

None block G0 / G1 / GA. G0 may raise one: if `CX2GFXEventScene` needs a packet or server value for a scene (none seen), it goes to
the user per memory `design-missing-june-pieces`.

## Play-test hand-off (every code phase)

Dev console first (auth level 3, END key). Copy-ready commands with real values; name the character and job from `els_db.sql`. For
each check: where / paste / what it does / look for / if not. The log tag is `[JUNE_GFX]` plus the phase's sub-tag.
