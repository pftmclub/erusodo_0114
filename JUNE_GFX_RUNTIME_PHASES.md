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
| G6 | Cursor vs the GFx layer: June's per-frame draw order (dialogs, GFx display, `CKTDXCursor`, the scene's cursor) against March's; port only if they differ (added 2026-10-19 from the quest-system Q5b play-test) | Opus 5.5 high + review | maybe | yes if code |

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
- **Scene `.swf`:** all 2,385 June `eventscene\*.swf` (309 MB) are now loose in `...\24965799\data\eventscene\`: 2,383 copied, `ranox.swf` and
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

### Phase G2 - Review (2026-10-19, Opus 5.5 high)

**Verdict: PASS with three small corrections** (flag `SERV_IRUHADEV_JUNE_GFX_MANAGER`; `_DEBUG` now off). Checked in Ghidra against
the decompile or disassembly of every G2 function. Values that live in data were read from `unpacked_x2.exe`.

**Matches June (no change).**
- **Layout and init:** ctor `31c45cc0` (+0x104 = -1, +0x108 = 1, +0x114..+0x11c = 0). Init `31c456e0` (states, wrapper, main.gfx,
  device viewport into the wrapper and +0xbc, +0x7c = timeGetTime; a main.gfx failure deletes the wrapper).
- **Loader states** `31c451d0`, in June's order: log, opener (8), clipboard (0x14), HAL, Renderer2D (June returns early if it is
  missing), AS2 (0x21), AS3 (0x22), the four image handlers, FontProviderWin32 (0x11), IME (skipped), sound, font config pick and
  apply. Sound `31c43700`: `Audio( r, 0.1f, 15, 2 = AudioMaster )`, volume 100.0, unmute.
- **Render:** `31be3800 @31be3c56` tests `+0x108 == 1` after the heat-shimmer block, before `31be2e90` (= `JuneStageChainRenderAfter`).
  `31c43b20` = Advance( +0xd4, 2 ) (viewport from +0x44..+0x58, `Advance( dt, 2, true )`), then Capture, BeginFrame, Display, EndFrame,
  Apply on the D3DSBT_ALL block. The block brackets every HAL draw, so March's cached render states are not leaked.
- **dt / lost / reset:** dt `31c430e0` = ( now - +0x7c ) / 1000.0. Lost `31c43440` releases the block, then PrepareForReset. Reset
  `31c43e60` makes the block, then either InitHAL with `{ 0, ConfigFlags 1 = NoSceneCalls, 0, 0, 0, queue 0x100, device, pp }` or
  RestoreAfterReset, then the viewport. Nothing on the D3D side leaks across a reset.
- **Input.** MsgProc `31be3f20` order, below `0x465`:
  - GFx mouse first. It swallows only when it returns 1, +0x108 is 1, and dialog +0x110 is 0.
  - Then the stage chain, then the GFx keys, which run even when a stage took the message.
- **Mouse** `31c44040`:
  - Gates: +0x11c, then +0x115 (wheel only).
  - LBUTTONUP and RBUTTONDOWN / RBUTTONUP need a hit first. A hit LBUTTONDOWN swallows and clears March's focus.
  - Wheel: 120.0 / 3.0. WM_ACTIVATEAPP pastes the clipboard.
- **Keys** `31c43890` / `31c432e0` / `31c431c0`:
  - The message set and the key table `33726390` match (35 pairs, all VK = code).
  - The Ctrl rule and the modifier renumbering match.
  - Swallow = HandleEvent bit 1 (`SHR 1; AND 1`).
- **IME and main loop:** IME `31c435b0` and the main-loop filter `31c43ab0` (hwnd = `msg.hwnd`) match. Enter / arrows `31c43480` match.
- **AS callback `31c45ed0`:** only `MSG` with a stage returns 1 (`@31c45f9d mov al, bl`). Every other path ends at `@31c4651a xor al, al`.
  The adapter walk `31d1ed00` stops on `AL == 1`. The listener vtable `33446ea8` is name `31c48570`, callback `31c45ed0`. The port
  returns the same values.
- **Wrappers:** all 14 names, argument types (Int for the UI ids, Number + StringW + Number for the two loads, StringW for showOkPopup)
  and counts match. `isOpenUI` / `isVisibleI` / `closeToTopZorder` return only a Boolean result.
- **Opener** `31c47a00`: lower-case url, then June's LoadDataFile `31c4c170` (kom only), then a memory file, else the SDK disk
  FileOpener. March's GetMemberFile does the kom formats; main.gfx and fontconfig.font load (G2 play-test). Kom first is June's order.
  The plan text's "LoadDataFile, so March's loose-first rule applies" predates G0 and is superseded.
- **`#else` path:** every March site keeps its original code (range flagcheck: 0 deleted lines). With the flag off, the new .cpp files
  are empty units and the `.vcxproj` include dirs are per file.

**Corrected (same flag).**
1. **Kom lookup miss log.** `31c4c170` logs a miss as error `0x87` (= `KEM_ERROR135`, via `31bd0f50`) before returning empty. G2's
   `LoadDataFile_MassFileOnly` was silent, and its header comment said June logs nothing. It now logs as June does
   (`KGCMassFileManager.cpp`). The fontlib check `31c4bcb0` logs nothing in June and stays silent.
   In game: a scene `.swf` opened by full path, and the SDK's `.gfx`-before-`.swf` probe, each add one KEM_ERROR135 line before the
   disk opens the file.
2. **Result pointer.** `moveToTopZorder` / `changeCharacter` / `ShowUI` / `HideUI` passed a result Value; June passes NULL. No effect
   in game.
3. **Dtor.** `31c45970` never deletes the +0x98 wrapper: JUNE_BUGS 325, **not kept**, since a live movie would outlive the Scaleform
   System. June's HAL Release ends in `~HAL -> ShutdownHAL`, which is the same as the port's explicit call.

Also fixed: a stray `0x14` byte in the GA Done note (a heredoc had read `\24965799` as octal).

**For G3** (G3 kickoff amended): the scene adapter is reached for every non-`MSG` name, because "GFXManager" returns 0 there. With
`_DEBUG` off, `JuneGfx*` are no longer bound; G3's `PlayEventScene` is its own binding.

**Hygiene.**
- Phase range `5d61dd0c^..5d61dd0c --with _DEBUG`: `files: 9 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 2 FAIL, 0 ERROR`, 0 deleted lines,
  `RESULT: FAIL`. The 2 FAILs are the PCH `#include "StdAfx.h"` of the two new .cpp files (empty units with the flag off), as G1.
- Review worktree (4 files): `3 PASS, 1 DEFINE, 0 FAIL`, `RESULT: PASS`. The 7 deleted lines are G2's own flagged lines: 4 wrappers,
  2 comments, and the `_DEBUG` define.

**Deploy.** `DEPLOY OK: X2_offline.exe is 25,096,704 B, dated 2026-10-19 06:45:31, md5 644d22933d7545af553ff9170fd9ddb6, equal to
X2/US_SERVICE/x2.exe (2026-10-19 06:45:24); built at main 8971693b + 4 uncommitted code file(s).` `JuneGfxLoadWidget` and `MGR gfx: ` are
gone from the exe; `[JUNE_GFX] MGR ` and `main.gfx` are in. **How to see it:** a regression check only (hand-off below); nothing new is
visible. **Open questions:** none.

**Regression play-test PASSED 2026-10-19** (user: village, dungeon, Alt-Tab, exit; exe md5 644d2293). ErrorLog: `[JUNE_GFX] MGR main.gfx loaded, font config "US" ( 1 fontlib fonts_Tahoma.swf, 1 map, 9133 tr )` plus the sound and IME lines; 0 KEM_ERROR135 (nothing reached the disk fallback); 0 EXCEPTION, baseline UNHANDLED only (2x E_DISABLE_HB_CHECK_REQ), clean exit.

## Phase G3 - Event scenes

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE`. Sub-tag `SCENE`.

- Port `CX2GFXEventScene` whole from G0 item 5 onto `CX2Data` (June's `+0x274`; March's Lua `CX2EventScene` stays): loader, Play /
  queue / End / IsPlaying, the end-frame callback, the input block (`+0x115`), text from `fontconfig.font`, voice.
- June's `PlayEventScene` binding `327ccad0`, and every `+0x274` / IsPlaying reader G0 classified as "port in G3" (no partial update:
  June's IsPlaying covers both scene classes, so March's readers of the Lua scene's IsPlaying change with it).
- Quest-UI callers (`323cb2e0` and friends) are the quest-system plan's: log nothing, port nothing, note them in the Done note.

**Done gate:** at the dev console, `PlayEventScene` with `Quest_22329_Start` plays Elysion7's scene with Elsword's lines and voice,
keys are ignored while it plays, it ends cleanly, and the Lua scenes March already plays (if G0 finds a reachable one) still play.

### Phase G3 - Done (2026-10-19, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE`)

**What landed** (read from Ghidra `3226aed0` / `3226b130` / `3226b6c0` / `32e59900` / `32e59f10` / `31d20430` and friends; 12 funcmap rows `GFx G3`):
- `X2Lib/X2GFXEventScene.{h,cpp}` (new): `CX2GFXEventScene` on `CX2Data` (`GetGFXEventScene`, `ResetGFXEventScene` from the loading state right after the Lua scene's). Data: `VillageName.xml` -> 11 region files, then `GFXEventSceneInfo.xml`, one map by `questID` (first wins). Play / queue-once / End (bool = drop the queue) / IsPlaying (flag **or** the Lua scene) / frame move (arrows, F1, F8, attack eaten; Enter = next; Esc starts the 0.6 s skip timer) / MsgProc (click = next, right release = skip timer) / voice (`OpenSound(name,10,true,-1)` + `Play(false,false)`) / `IsRecentlyPlayed`. Scene UI hide = `SetShow(false)` + mini-map / world map follow, quest list reset on show (June `32269f20`).
- `KTDXLIB/KTDGGFXManager.{h,cpp}`: the Scaleform half. G2's adapter gained June's **DataBinding wrapper** (`dataBindingInit` puts `createBindingImpl` / `deleteBindingImpl` native functions on the AS object; bindings `name -> { null | bool, observers }`; Notify = `InvokeSelf` on each observer closure). `CKTDGGFXEventSceneBridge` = June's scene adapter: listener `EventScene` answering `gameDataEventScene` with `endEventScene` / `playEventSceneSound` / `getEventSceneData` / `playedEventSceneAndGetMent`, and the bindings `EventScene.nextScene` / `EventScene.bExactFit`.
- Lua `PlayEventScene( name )` and `IsRecentlyPlayedEventScene( name )` (`X2Main::RegisterLuabind`). Readers: StateChange (End(true) first), MsgProc (GFX first), TFieldGame unit hide, Informer, TalkBoxManager frame move, StateDungeonGame::QuitGame (tutorial branch), DungeonGame::Release tail (`ClearRecentlyPlayed`). They call `CX2Data::JuneIsEventScenePlaying()` = June's `3226a010`, so the Lua scene's IsPlaying now reaches the TalkBox reader too (as in June).
- Project: `X2Lib/X2Lib_2010.vcxproj` + `X2GFXEventScene.cpp` / `.h` (no include dirs: X2Lib uses `../libxml` already and only the plain `KTDGGFXManager.h`); KTDXLIB unchanged (G2's per-file Scaleform dirs cover the manager .cpp).
- `JUNE_BUGS.md` 328-330. Always.h also checks `SERV_IRUHADEV_JUNE_GFX_MANAGER` and `_SOUND_AUTO_RELEASE`.

**Not ported, with owners:** quest UI `323cb2e0` / `323e4cb0` (quest-system; `SetNotifyNewEpicQuest` is the hook for `+0x70`); `31d71810` gate = no March function (its callers are June-only); `31efccf0` = June's tutorial-skip ACK (packet `0x6c4`, gated on `IsEnableCode(0x93a)`, never set offline) with End's skip popup `@3226bb05` (REQ `0x6c5`) and `31f0bdd0` = March's `SetNowState` (already there); `CX2GuildAgitGame` absent; End's tutorial guide / profession manager (Stage 9), `3232e000` quest manager reset, `31d87420`, the Aisha-only `CX2Game +0xb38 = 2.8f` (`@3226bf38`, unidentified in March); `TalkBoxManager`'s `g_pData +0x288` byte gate; QuitGame's other tutorial gates; `CX2GUAdd 32ab4480` (a time/scene gate, no March function found by name or fields). Software cursor (`3226c250`, `31ce50d0`, `32868250` = R4f): the scene uses March's `CX2Cursor` `XCS_EVENT_SCENE` (same `DLG_Eventscene_Cursor.dds`).
**Data gap:** the black layout is the particle `EventScene_BlackLayOut` of **June's** `GameMajorParticle_INT.txt` (data036); March's mounted copy (datai015) lacks it, so no dimming is drawn and Play logs it. `kom.py stage-june` would replace March's file wholesale (a particle script: the asset port's `.txt` wave owns that), so G3 left it.
**Guard added (not June):** Play refuses and logs when `main.gfx` did not load, instead of hiding the UI for a movie that is not there.

**Hygiene** (`flagcheck --worktree --files`, 14 files): `files: 7 PASS, 5 PASS~, 1 DEFINE, 0 OTHER, 1 FAIL, 0 ERROR`; the FAIL is the new file's `#include "StdAfx.h"` PCH line only; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without token, 0 encoding change(s)`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,175,040 B, dated 2026-10-19 07:38:42, md5 12560fa17b990fec520b956c8fcea394` (a peer's build compiled this tree; strings `[JUNE_GFX] SCENE`, `gameDataEventScene`, `EventScene.nextScene`, `dataBindingInit`, `createBindingImpl`, `PlayEventScene` verified in the exe).
**How to see it:** dev console (auth level 3, END): `PlayEventScene("Quest_22329_Start")` anywhere with a village or dungeon loaded. Log: `python tools/logs.py --grep "JUNE_GFX SCENE"`.

**G3 fix 1 (2026-10-19, after the first play-test): crash on `PlayEventScene`.** Minidump (`tools/mdscan.py`): AV in `JPEGInputImpl_jpeglib` ~ `CreateSwfJpeg2HeaderOnly` <- `GFx_DefineBitsJpeg3Loader` while loading `Elysion7.swf`. Cause: the SDK's `libgfx.lib` is built on **libjpeg 8d** (`JPEG_LIB_VERSION` 80), the client links CxImage's **6b** (`jpeg.lib`) under the same names, so the first JPEG in a .swf took 6b's "Wrong JPEG library version" path (G1 kept "the client's jpeg" as the one copy; G2's widgets had no JPEG). Fix, flag `SERV_IRUHADEV_JUNE_GFX_JPEG8`: `tools/sfjpeg.py` writes `KTDXLIB/sfjpeg.lib` = the SDK's libjpeg 8d (46 objects) plus libgfx's `JPEG_ImageReader/Writer/Common` with every `_jpeg_*` symbol renamed `_Jpeg_*` (same-length COFF edit, 168 names / 355 entries); `KTDXApp.cpp` links it before `libgfx.lib`; `build.py` regenerates it when missing (`*.lib` is git-ignored). Map check: `_Jpeg_CreateDecompress` from `sfjpeg:jpeg8_jdapimin.obj`, GFx JPEG classes from `sfjpeg:gfx_JPEG_ImageReader.obj`, CxImage still on `jpeg:Jdapimin.obj`. `DEPLOY OK: X2_offline.exe is 25,305,600 B, dated 2026-10-19 08:03:19, md5 99d486a17048b2581a4ad98c302bc0f3` (a peer's build compiled the tree).

### Phase G3 - Review (2026-10-19, Opus 5.5 high)

**Verdict: PASS. No code change, no rebuild** (the deployed exe stays md5 `99d486a1`).

**Checked against June in Ghidra (read-only):**
- **XML loaders.** `31eecc40` (region files) reads `xPos` / `yPos` / `wordBalloonPosX` / `wordBalloonPosY`; `31eed7a0` (`GFXEventSceneInfo.xml`) reads none of them; both read `questID` / `swfFileName` / `frame` / `soundKey` / `characterNameKey` / `textKey` / `wordBalloonName` / `symbolName` / `delayTime`. Same in `LoadSceneXml( bRegion )`.
- **Play `3226b130`.** Size 0 / 1 name dropped; while playing: queued once by name with the frame (`3226c8f0`); name to `+0x78` before the path test; `31ee3160` / `31ee3250` -> `"%s.swf"`; `GetPullPath` miss = return; `Tutorial_Back.swf` -> `0x2712`; frame -1 -> `loadUserWidget( 0x2711 )`, else `loadEventSceneEndFrame`; `+4`, `+0x115`, `+0xb8` / `+0xbc`, `32269f20`, black layout to `+0xd0`, `unloadWidget( 3 )`. Matches.
- **End `3226b6c0`.** Cursor, `+0x115 = 0`, both unloads, `+4 = 0`, UI show, black layout off, `3226ade0` ment erase; bool: queue (`327464a0`) + `+0xc0` map (`3226ccb0`) dropped; queue not empty -> pop + `Play( name, frame )` and return; else epic-quest `+0x70` -> `323ca640`, `+0x94 = +0x78`, `+0x78` cleared, sound Stop + close. Matches. The unported calls are the ones the Done note names.
- **Frame move `3226c120`.** Five `31c48870` (GA 0x23..0x26 = UP / DOWN / LEFT / RIGHT, 0 = ATTACK_FAST) and two `31d24fb0` (`DIK_F1`, `DIK_F8`) eaten, `DIK_RETURN` -> `32e59800`, `DIK_ESCAPE` -> `+0xb8`, `> _DAT_3344930c` -> `End( 1 )`. Matches.
- **MsgProc `32269e00`** (not a Ghidra function; read raw with `disasm_range.py`): `0x202` -> next scene, true; `0x204` -> true; `0x205` -> `+0xb8 = 1`, false. Matches.
- **IsRecentlyPlayed `3226a130` / `325aece0`.** The Lua half compares `CX2EventScene +0xe0` and clears it on a match. JUNE_BUGS 330 stands.
- **Bridge.** `32e59900`: types 8 / 9 / 10 are left alone; `bExactFit` = app `+0x1bc < 1 || +0x1b8 < 1`, pushed inside the loop. `32e59f10` key names (`frame` ... `wordBalloonPosY`, `symbolName`, `posX` / `posY` at `33532040` / `33532038`, `delayTime`, `arrSymbolData`, `arrEventSceneData`) match the strings in June's exe byte for byte.
- **Readers** (every G0 row targeted G3): StateChange `31d88bf0` (End(1) on IsPlaying, then the Lua scene's own end), MsgProc `31d8e280` (`+0x274` vslot 3 before `+0x278`), TFieldGame `323ae6a0`, Informer `32f2b8e0`, TalkBox `32e90640` (`+0x198 = 0`, return; `+0x288` gate not in March), QuitGame `324d1580` (tutorial branch: the `3226a010` term only, string `0x232`; the other terms and the `0xa931` string belong to the tutorial work), DungeonGame `327d2160` (`g_pMain +0x16c` -> `3226a260`). Nothing missed and nothing extra: `31d71810`, `32ab4480`, `31efccf0`, `31f0bdd0` are closed in the Done note with reasons. Quest-UI callers (`323cb2e0`, `323e4cb0` = the `+0x70` writer) were left alone.

**Notes (no action):**
- June zeroes `+0x68` / `+0x6c` (two dwords after the `+0x4c` narrow string) in the ctor and in End. None of the class's 97 decompiled functions read them, so they were not ported. G0's ctor row says "`+0x58` wstring"; it is a `std::string` at `+0x4c`.
- Two lines are not June's, both deliberate and harmless: Play refuses when `main.gfx` did not load, and End clears `m_bSkipRequested` (`+0xd4`). The second is dead, because March never calls `SetSkipRequested`.

**Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_GFX_EVENTSCENE --range 3dde2b24^..3dde2b24 --with SERV_IRUHADEV_JUNE_GFX_DEBUG`; no later commit touches the flag):
`files: 7 PASS, 5 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 1 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: FAIL`. The FAIL is `X2GFXEventScene.cpp`'s PCH line `#include "StdAfx.h"`, outside the guard of a new file (the same as G1 / G2), so it is not a finding.
Fix 1 (`SERV_IRUHADEV_JUNE_GFX_JPEG8`, `3b566a1f^..3b566a1f`): `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `RESULT: PASS`.

## Phase G4 - Talk cut-ins

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN`. Sub-tag `CUTIN`.

- Port `31d76b40` and every caller G0 item 7 lists that exists in March, plus `playSimpleWidget` `31fc75e0`. Callers in absent
  systems are logged once.

**Done gate:** a talk cut-in shows in the state G0 names (or through the debug hook if no March state reaches it), left and right
variants, and closes.

### Phase G4 - Done (2026-10-19, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN` + `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN_DEBUG`)

**What landed** (Ghidra `31fd95a0` / `31fde0f0` / `31d76b40` / `31fc75e0` / `31fc74e0` / `31fc72e0` / `31fc73a0` / `31fc7720` / `31fc7360` / `31d70c60`; 8 funcmap rows `GFx G4`):
- **The leaf.** `CX2GUUser::ActiveSkillShow( bonePos, ... )` = June `31fd95a0`: hidden dialogs (`31d03920` = `GetHideDialog`, March's own test above it) or `bOnlyLight` draw nothing; else the
  dragged item goes back (`32153240` = `CX2UIManager::SetNowDragItemReturn`, public wrapper `JuneSetNowDragItemReturn`), then `g_pMain->JunePlayCutInWidget( 0xc, <name>.gfx, GetIsRight() == false, 0 )`.
  An early return before March's particle body (kept for flag off). The two other overloads and the 8 June callers reach it through March's existing class code; the position args are not read by June either.
- **`CX2Main`** (`X2GFXEventScene.cpp`, member `m_pJuneSimpleWidgetPlayer` = June `+0xd7c`): `JuneMakeSimpleWidgetPlayer` (`31d70c60`; also at the end of the StartUp state's script load, `3259bb20`, so
  the player exists before main.gfx's AS registers) and `JunePlayCutInWidget` (`31d76b40`: frame `Common_CutIn_Frame.gfx` ( side, 1 ), a talk balloon `CUT_IN_TALK_<R_|L_><rand() % 3 + 1>.gfx` ( 0, 0x3c ), then the cut-in ( side, b )).
  Side: `L_` for a non-zero side, i.e. a unit facing left.
- **`CKTDGGFXSimpleWidgetPlayer`** (`KTDGGFXManager.{h,cpp}`, June's `CX2SimpleWidgetPlayer`): listener `simpleWidgetPlayer` (answers `simpleWidgetFunctionInit` true, nothing else), bindings
  `SimpleWidgetPlayer.playSimpleWidget` and `...clearSimpleWidgetPlayer`; `PlaySimpleWidget` = `31fc75e0` (sets widgetID / fileName / parameter1 / parameter2, notifies). G3's binding table got a third kind:
  an object `{ widgetID, fileName, parameter1, parameter2 }` built with the movie (`31fc7720`); the adapter keeps the movie pointer (June adapter `+0x10`).
- **Not ported, with reasons.** `31fc7360` clearAll (its binding is never registered: JUNE_BUGS 333); the NPC route (`32772080`, textures); `31fde0f0`'s bone lookup (March has it).
- **Callers.** `32014fc0` (ShowHyperSkillCutInByScript, C3t) and 7 class functions all end in `31fde0f0`; March's classes (Add, Ara, Chung, EL, Elsword, Eve, Lire, Raven, Arme) already call the matching overloads. No caller is absent.
- **Names (routed, not ported).** June's tables name cut-ins without an extension; March's name `.dds` textures that are other names, and none of the 52 has a `.gfx` (`june2017_re/g4_cutin_names.tsv`). So a March class
  cut-in shows frame + balloon but no hero art until its class port renames its table: JUNE2017_CHARACTER_PORT_CHECKLIST.md, after 7t. Hyper cut-ins use `UnitTemplet` names (empty in March data).
- **Data:** none new (GA staged data152 `Common_CutIn_Frame`, `CUT_IN_TALK_*`, `HQ_CutIn_*`; data195; data194 `main.gfx`). Project files: none.

**Hygiene** (`flagcheck --worktree --files`, 10 files): `files: 5 PASS, 3 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; OTHER = Always.h's other flags' lines and a peer's `SKILL_MAX_MP` hunk in X2GUUser.cpp (not committed here);
`0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without token, 0 encoding change(s)`. `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,309,696 B, dated 2026-10-19 09:15:01, md5 3aad257b3d2b6fd97bede5edf038828a, equal to X2/US_SERVICE/x2.exe (2026-10-19 09:14:54); built at main 3444f3ae + 14 uncommitted code file(s).` Strings in the exe:
`SimpleWidgetPlayer.playSimpleWidget`, `simpleWidgetFunctionInit`, `CUT_IN_TALK_`, `Common_CutIn_Frame.gfx`, `[JUNE_GFX] CUTIN`, `JuneGfxCutIn`.
**How to see it:** dev console `JuneGfxCutIn( "HQ_CutIn_Elsword_01" )` in a dungeon / field: frame, balloon and the cut-in. Log: `python tools/logs.py --grep "JUNE_GFX CUTIN"` (one `play ui=0xc <name>.gfx side=L|R` line per cut-in).
**Replaces** March's particle cut-in for users: tell the play-tester. **Open (user):** none blocking; the table rename waits for the class ports.

**Play-test PASSED (2026-10-19, user + log):** (1) `JuneGfxCutIn( "HQ_CutIn_Elsword_01" )` showed the frame, the balloon and the art; (2) a real Eve skill showed frame + balloon only (the name was `HQ_CutIn_Eve_EP_1.dds.gfx`: March's `.dds` table name, the checklist item, not a bug of this phase); (3) dialog hide showed nothing. Log: 4 + 3 `[JUNE_GFX] CUTIN play` lines, 0 EXCEPTION, UNHANDLED = baseline `E_DISABLE_HB_CHECK_REQ`. **The frame is black by design:** `Common_CutIn_Frame_I1/I8.dds` are black DXT3 with alpha (a slanted band, a wedge, dark radial lines), so there is no colour to lose. **Not exercised:** the left-facing variant (all 7 plays were `side=R`); the review checks `L_` against June's strings `33456858` / `3345685c` in Ghidra instead.

### Phase G4 -- Review (2026-10-19, Opus 5.5 high): PASS, one transcription fix

**Checked in Ghidra (read-only), all against the port:**
- `31fd95a0` (leaf): `31d03920` on `g_pKTDXApp +0x1fc -> +0x38c` (GetHideDialog) == 1 or `[ebp+0x30]` (bOnlyLight, @31fd95ef; `[ebp+0x14]` = the name) skip all; `g_pData +0x2cc` != 0 -> `32153240`; name narrowed with code page 0 + `L".gfx"` (`31e572c0`); `31d76b40( 0xc, name, vslot 0x170() == 0, 0 )`: vslot 0x170 takes no argument (the `PUSH EBX` @31fd9820 is 31d76b40's last argument) and is GetIsRight (funcmap `321dc470`). Port matches, placed at the top of the leaf after March's own `DIALOG_SHOW_TOGGLE` test; March's body below is particles only (FocusLine / OutLine / CutIn_* / CutIn_Talk_*), nothing else dropped.
- `31d76b40`: `31d70c60` first; nothing without `+0xd7c`; UI 0xc -> `Common_CutIn_Frame` (0x12 chars) + `.gfx` ( side, 1 ), then `CUT_IN_TALK_` + ( side == 0 ? `3345685c` : `33456858` ) + `rand() % 3 + 1` + `.gfx` ( 0, 0x3c ); then the name ( side, b ) for every UI id. Bytes from `unpacked_x2.exe`: `33456858` = `"L_"`, `3345685c` = `"R_"`, so non-zero side (facing left) = `L_`, as ported.
- `31fc7720` (GetValue): only with a movie; members in order `widgetID` (+8), `fileName` (+0xc), `parameter1` (+0x28), `parameter2` (+0x2c), as ported. **Finding:** June builds the three numbers as `VT_UInt` (type 4 @31fc776b and the two parameter stores), the port as `SInt32` (`VT_Int`). Fixed under the same flag (`SF::UInt32`, `KTDGGFXManager.cpp` BuildWidgetValue). No in-game difference expected (values 0xc / 0 / 1 / 0x3c).
- Also matched: `31fc75e0` (this +4 == 0 -> 0; `31d1ef00` binding, +8 / string / +0x28 / +0x2c, `31d1ee10` notify, 1), `31d70c60` (once, adapter from `g_pKTDXApp +0x1c8 -> +0x98` vslot 1 or 0), `31fc74e0` (listener, then the 0x30 B playSimpleWidget value and the 8 B clear trigger).
- Callers: `ShowHyperSkillCutInByScript` (C3t) and the class code reach the leaf through March's overloads; no override of `CX2GUUser::ActiveSkillShow`; `CX2GUNPC::ActiveSkillShow` stays on its texture route (untouched).

**Debug off:** `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN_DEBUG` commented in Always.h (every use is under it; `JuneGfxCutIn` no longer in the exe).

**Hygiene.** Phase range `flagcheck SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN --range a826cb95^..a826cb95 --with ..._DEBUG`:
`files: 6 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` /
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
Review worktree (`--files KTDGGFXManager.cpp Always.h`): `RESULT: PASS`; Always.h OTHER = peers' uncommitted flags; the 3 deleted lines are the three `SInt32` SetMember lines of this flag's own block, replaced.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,315,840 B, dated 2026-10-19 09:42:16, md5 6c49167a65a05f0a776f1a9508dd6978, equal to X2/US_SERVICE/x2.exe (2026-10-19 09:42:09); built at main c3fb3adf + 5 uncommitted code file(s).`

**Log** (the 09:26 run, exe 3aad257b, i.e. the S7j play-test): 5 `[JUNE_GFX] CUTIN play ui=0xc` lines, sides L, R, L, R, L: **the left variant was exercised there** (G4's own run had only R). Each `HQ_CutIn_Elsword_LK_1.dds.gfx` play is followed by `KEM_ERROR135` for that name (the March `.dds` table names, checklist after 7t), 0 EXCEPTION, clean exit.
**Owes:** nothing (no play-test needed for the type fix). Next: `/x2-phase june-gfx G5` on Sonnet 5.5 high.

## Phase G5 - Owed gates

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_GATES`. Sub-tag `GATE`.

- `GetShowActiveSkillShow`'s `isOpenUI(0xb)` term (`31fd3180 @31fd3228`) on top of C3t-1's port.
- The C3p-2 ESC gates G0 item 8 assigns here (`closeToTopZorder` at least). The result-UI mode and anything else that needs a Stage 9
  screen are written into the backlog's Stage 9 section, not ported.
- The movie term goes wherever G0 item 8 says (the AVI movie table is not GFx).

**Done gate:** with a debug-opened UI 0xb, the cut-in hides as June's option says; ESC closes the top GFx widget first; with no
widget open, ESC and cut-ins behave as before.

### Phase G5 - Done (2026-10-19, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_GFX_GATES` + `_GATES_DEBUG`, and `SERV_IRUHADEV_JUNE_GFX_DEBUG` back on)

**What landed** (Ghidra / raw disassembly; 9 funcmap rows `GFx G5`; JUNE_BUGS 336):
- **Cut-in term.** `GetShowActiveSkillShow` (`31fd3180 @31fd3228`): the GFx manager present and `isOpenUI( 0xb )` true -> false, after C3t-1's terms. UI 0xb is opened only by June's cash-shop code, so it is false until Stage 9. The `g_pMain +0x80a` term is June's AVI movie flag, not GFx: left out with a comment, owned by the new backlog **Stage 26**.
- **ESC.** `CX2State::HandleMsgByESC` (`31f1e1a0 @31f1e262`): `closeToTopZorder` first (true -> return false; June then closes the profession manager, absent in March), before the hide-toggle block. Inside it, `31d03a10` (current hide mode 2, 3 or 4 -> return false without unhiding).
- **Hide-mode mechanism whole** (`KTDGUIDialogManager`): `JuneSetShow( bShow, mode )` = `31d05af0` (mode rule, previous id, preset copy; March's one-argument `SetShow` = mode 0, so the Lua binding is unchanged); the five presets of the StartUp loader `32597410` (mode 1: 5 names, 4: 19, 3: 11, 2: 18, 5: 1; ids from the code, 3 = `ebx`, 5 = `esi`), built in the manager ctor; the name list read by OnFrameMove (`31d06100`, walks a copy) and by the R8 render hide path (its six literals replaced; mode 1 = the same names, now in June's sorted order); `JuneToggleHide` = `31f02f50` (modes, and the GFx `+0x108` write) at the two March DELETE-key sites (`X2State.cpp` = `31f036a0`, `X2StatePVPGame.cpp` = `32523dc0`). `X2Main`'s DELETE handler has no June twin and keeps the plain toggle.
- **Corrections.** `31d05b80` is not a list reader (it is GetDialog by UTF-8 name): G0 row 119 and the R8 review were wrong there. Sets 2 and 3 of the preset struct are never filled (JUNE_BUGS 336): the toggle's opening delete loop is dead and not ported.
- **Stage 9 / backlog notes written** (`JUNE_PORT_BACKLOG_ORDER.md`): the cash shop (UI 0xb), hide-mode entries 2-5, the result-UI mode `+0x210`, the profession close, the tutorial guide (C3z-4t); new Stage 26 (AVI movie system).
- Project files: none.

**Hygiene** (`flagcheck --worktree --files` on the 6 code files): `files: 4 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`; OTHER = Always.h (peers' uncommitted flags beside this flag's block); `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without token, 0 encoding change(s)`. `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,350,656 B, dated 2026-10-19 11:57:00, md5 b5a84db238c64efbfc12bbb564b82b34, equal to X2/US_SERVICE/x2.exe (2026-10-19 11:56:52); built at main e0b19fc5 + 20 uncommitted code file(s).` (`COALESCED` onto a peer's build that started after this request.) Strings in the exe: `GATE ESC closeToTopZorder`, `GATE cutin: unit`, `GATE hide: SetShow`, `JuneGfxOpenUI`.

**How to see it:** dev console `JuneGfxOpenUI( 11 )` (log `debug: openUI( 11 ), isOpenUI -> n`), cast a cut-in skill, press ESC, press DELETE. Log: `python tools/logs.py --grep "JUNE_GFX GATE"` and `--grep "JUNE_GFX MGR debug"`.
**Debug on until the G5 review:** `SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG`, `SERV_IRUHADEV_JUNE_GFX_DEBUG`.
**Open:** if `openUI( 11 )` reports `isOpenUI -> 0` (main.gfx has no widget for UI 0xb without the cash shop), the cut-in term cannot be shown in game; the review then reads it from Ghidra only. No question for the user.

**Play-test, part 1 (2026-10-19, user + log; exe b5a84db2, then diagnostic exe 642e712f):**
- Cut-in term PASSED: with `JuneGfxOpenUI( 11 )` the June cash shop (Item Mall, GFx) opened over the field; `isOpenUI( 0xb ) = 1` and two `GATE cutin ... show 1 becomes 0` lines for the Eve cast (no cut-in).
- ESC with UI 0xb open: March's menu opened every time (`closeToTopZorder -> 0` x5). Diagnostic (debug-only line in `CKTDGGFXManager::closeToTopZorder`): `type 2 isBool 1 bool 0, isOpenUI( 0xb ) 1, isVisibleI( 0xb ) 1`, i.e. the AS returns a real Boolean false for UI 0xb, so the port reads it right. June closes this shop through `unloadWidget( 0xb )` / its own C++ (`32228498`), so the false branch is June-faithful. The true branch (ESC taken) is still unproven in game: no closeable widget was tried.
- DELETE toggle (`GATE hide` lines) not exercised yet; only the `SetShow( 1, 0 ) -> mode 0` lines of state entries.

### Phase G5 -- Review (2026-10-19, Opus 5.5 high): PASS, no findings, no code change

**Checked in Ghidra / raw disassembly (`june2017_re/disasm_range.py`), read-only, all against the port:**
- **Cut-in term** `31fd3180 @31fd3228-31fd325e`: after C3t-1's terms have set `BL` (option 5 -> vslot 0x198 hyper / `32431360`), `g_pKTDXApp +0x1c8` non-NULL and `31c44710( 0xb ) == 1` -> `return false`; then `g_pMain +0x80a == 1` -> false; else `BL`. The port sits after every C3t-1 term with only the debug line before `return`; the movie term is left out with its comment and owned by backlog Stage 26.
- **ESC** `31f1e1a0 @31f1e262-31f1e2f3`: GFx non-NULL and `31c43bb0` (closeToTopZorder) == 1 -> the profession close (`32396dc0` / `3239d190`) -> `return false` either way (tail `31f1e245` = `xor al,al; ret`); else `g_pInstanceData` / dialog manager / `31d03920` GetHideDialog -> `31d03a10` (mode 2-4) -> `return false`, else the unhide. Port order is the same: closeToTopZorder right after the ESC key test, before March's `DIALOG_SHOW_TOGGLE` block, the mode rule first inside it. The two gates before it (tutorial guide, result-UI `+0x210`) and the profession close are backlog notes (Stage 9 / C3z-4t), checked present.
- **Hide modes.** `31d05af0` (`+0x34 = bShow`; mode = bShow ? 0 : mode ? mode : 1; `31d05a20` find + `31d08460` assign) = `JuneSetShow`. `31f02f50` = `JuneToggleHide`: the struct copy's id / previous id, `31d03940` on each; now 2-4 -> `SetShow( 0, 0 )`, GFx `+0x108 = 0`; previous 2-4 -> `SetShow( 0, previous )`, else `SetShow( old == 0, 0 )`, both `+0x108 = 1`; the opening loop walks the struct's second set (`+0xe4` head), never filled. OnFrameMove `31d06100`: `+0xbc && !+0x34` -> walk `+0xd4`, GetDialog `31d05070`, valid `31d41d10` -> `31cd6ed0`. The port walks a copy (June walks the live set; a frame move that changes the mode would leave June's iterator dangling); behaviour is the same otherwise.
- **Presets** `32597410 @3259b24e-3259bafb`: each block builds a struct (`31d08180`), stores the id, inserts the names into the first set (struct +8 = `+0xd0`), adds it with `31d06540`. Mode 1 (@3259b26e) 5 names, 4 (@3259b365) 19, 3 (`EBX` @3259b63d; no `DLG_ChatBox_EditBox`, no quick slots) 11, 2 (@3259b800) 18 incl. `DLG_Dungeon_Game_Start_Count` / `_Continue`, 5 (`ESI` @3259bab6) `DLG_SlideShotScripted_TalkBox`. Names and order match `s_aJuneHidePresetRows`; sorting wide vs narrow is the same for these ASCII names.

**Debug off:** `SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG` and `SERV_IRUHADEV_JUNE_GFX_DEBUG` commented in Always.h (`JuneGfxOpenUI` and the `GATE` lines leave the exe).

**Hygiene.** The kickoff's `--range b224be51^..HEAD` crosses peers' commits and FAILs on one of them (`5383d63 X2Lib/X2UIQuestNewJune.cpp`, a quest-UI new file, not this flag); per phase commit instead:
`flagcheck SERV_IRUHADEV_JUNE_GFX_GATES --range b224be51^..b224be51 --with SERV_IRUHADEV_JUNE_GFX_DEBUG`: `files: 4 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` /
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS` (OTHER = Always.h, the flag's own define block beside peers' flags).
`flagcheck SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG --range c2e1d35c^..c2e1d35c`: `files: 1 PASS, 0 FAIL` / `RESULT: PASS`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,381,376 B, dated 2026-10-19 13:10:38, md5 01c5f8d1bdcb1c04c201029a1384fe1d, equal to X2/US_SERVICE/x2.exe (2026-10-19 13:10:30); built at main e3a34f3b + 3 uncommitted code file(s).` (`JuneGfxOpenUI` / `GATE ESC closeToTopZorder` no longer in the exe.)
**Follow-up (user: ESC does not close the hand-opened GFx shop):** June does not close it with ESC either. `closeToTopZorder` is false for `0xb`; the field states' ESC handler `32464b20` (vslot 0x1ec) sees `CX2CashShop` open, closes only an inner popup (`322273f0`) and swallows ESC; the shop closes through `CX2CashShop::SetOpen( false )` (`3223d680` -> `unloadWidget( 0xb )`). Our build opens March's menu instead because the debug command opened the widget with no June cash shop behind it. Stage 9 note extended; funcmap rows `GFx G5 review`. Not a G5 defect.
**Owes:** nothing (debug-off only; no play-test). The ESC-true branch and hide modes 2-4 stay unproven in game until Stage 9 brings a closeable widget and the result UI. Next: `/x2-phase june-gfx Z` on Sonnet 5.5 medium.

## Phase G6 - Cursor and the GFx layer

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER` (only if code is needed). Sub-tag `SCENE`. Added after the Z close-out.

- **Why.** quest-system Q5b's play-test (user, 2026-10-19): in a GFx event scene the cursor draws in front of the Scaleform layer; the user asked whether June draws it behind. G3 reused March's `CX2Cursor` (`XCS_EVENT_SCENE`) and never compared the order with June's.
- Map June's per-frame order: dialog passes, `CKTDGGFXManager` display (`31be3800` frame split), `CKTDXApp +0x290` `CKTDXCursor` (`31ce5400( 0 )` @`31be3c7c`), the scene's Play / End cursor calls (`32868250` / `32868790` / `31ce50d0`). Compare with March. Equal: write the map, no code. Different: port the order under the flag; the `CKTDXCursor` class stays R4f's.

**Done gate:** the map (addresses + order) in this section and `(GFx G6)` funcmap rows; with code, build + deploy and the user sees the cursor where June puts it with a click still advancing the scene.

### Phase G6 - Done (2026-10-19, Opus 5.5 high; flag `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER`)

**Answer: June draws the scene cursor IN FRONT of the movie, as the frame's very last draw.** March drew it before GFx (under the movie;
where it looked in front, the movie was transparent there). June and March differed, so the order is ported.

**June's frame (`31be3800`), in order:** 1. stage passes 0x8 / 0xc( 0 ) / 0x10( 0 ) (`31be2cd0` / `31be2d30` / `31be2d90`): the world, the
dialog manager's front UI and **`CX2Cursor`** (state +0x64, `32868a00` from `CX2State` front UI `31ef7640`); 2. the scene / effect target
branch (pass 1); 3. **GFx** `31c43b20` when manager +0x108 == 1 (@31be3c56..31be3c68); 4. the 0x18 pass `31be2e90` (@31be3c6f); 5. **`CKTDXCursor`
`31ce5400( 0 )`** on app +0x290 (@31be3c7c).
**The scene's cursor calls:** Play `3226b130`: @3226b542 `32868250` on the CX2Cursor (+0xb0 show = 0, +0xb8 = 1: no draw, frame move
`32868670` and hardware cursor `32868740` stop); @3226b56b `3226c250( 1, -40, -40 )` -> `31ce5520( 1 )`: CKTDXCursor shown with
DLG_Eventscene_Cursor.dds, offset -40. End `3226b6c0`: @3226b71d `32868790( 0 )` (CX2Cursor normal, +0xb8 = 0), @3226b748 `31ce50d0`
(CKTDXCursor hidden). `31ce5520` has no other caller: June's after-GFx cursor exists only for GFx scenes (no client call asks the movie to draw
a cursor; the `.swf` contents were not inspected). The cursor is never hidden for the scene; it hides after 5 s without mouse movement (`31ce51b0`, the same rule as CX2Cursor).
June's scene cursor matches March's `XCS_EVENT_SCENE` draw (`FIX_EVENT_SCENE_CURSOR_PICK_POINT_POSITION`: same texture, 128 x 128 `3344c984`,
-40 offset `33486f20`), so only the draw position changes.

**What landed** (the class stays R4f's): `CX2Cursor` gets an after-GFx mode (`SetJuneDrawAfterGFx`, `JuneOnFrameRenderAfterGFx`):
while it is on, `OnFrameRender` returns at every state's own call (CX2State front UI and the unsplit states' `OnFrameRender`), and
`CX2State::OnFrameRenderAfter` (the 0x18 pass, after GFx; no state overrides it) draws it. `CX2GFXEventScene` Play turns it on after
the `XCS_EVENT_SCENE` switch, and End turns it off. The slide shot and March's Lua scene keep `XCS_EVENT_SCENE` before GFx, as June's
CX2Cursor does. Files: `Always.h`, `X2Cursor.h/.cpp`, `X2State.cpp`, `X2GFXEventScene.cpp`. No JUNE_BUGS row (nothing kept verbatim).
10 funcmap rows `(GFx G6)`. `kickoffs/june-engine/R4f.md` now names what R4f inherits: port the real `CKTDXCursor` after the 0x18 pass,
`32868250`'s CX2Cursor switch-off, and retire this flag in the same phase so the cursor is never drawn twice.

**Hygiene:** `flagcheck JUNE_GFX_CURSOR_ORDER --worktree --files <5>`: `files: 4 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag
token, 0 encoding change(s)`; `RESULT: PASS`. The OTHER row is `Always.h`: a peer's uncommitted block in the worktree (not in this commit).
Guards `JUNE_GFX_EVENTSCENE` / `JUNE_RENDER_STAGE_SPLIT` are top-level defines above line 9424 (nesting scan), never `#undef`'d.

**Deploy:** `DEPLOY OK: X2_offline.exe is 25,415,168 B, dated 2026-10-19 17:08:42, md5 cad1ab82212b7fc4806800ba17607729, equal to
X2/US_SERVICE/x2.exe (2026-10-19 17:08:33); built at main 4edc83db + 10 uncommitted code file(s).` (COALESCED: a peer's build started
16:55, after these edits were on disk.)

**How to see it:** dev console `PlayEventScene("Quest_22329_Start")`: the 128 px event-scene cursor stays visible over the whole movie,
including over its characters and boxes. A click still advances the scene, and after End the normal cursor is back.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.
- `MODS.md` lines for every flag, with revert steps (the `.vcxproj` hunks included).
- Dated notes in `JUNE_PORT_BACKLOG_ORDER.md`: Stage 4's Scaleform decision answered (runtime, this plan); Stage 9 gets the root-UI
  screens, wrappers' callers and any gate G5 routed; Stage 12 gets `GFX_SKILL_CUT`; Stage 6 / 8 know the cut-in kom `data195`.
- Memory, and `SERV_IRUHADEV_JUNE_GFX_DEBUG` off unless a phase recorded a reason to keep it.

### Phase Z - Done (2026-10-19, Sonnet 5.5 medium; docs only: no code, no flag change, no build)

- `MODS.md`: entries for G3 (`JUNE_GFX_EVENTSCENE`), G3 fix 1 (`JUNE_GFX_JPEG8`), G5 (`JUNE_GFX_GATES`) and a close-out entry with the flag dependency order and every `.vcxproj` hunk (G1, G2, G3). G1 / G2 / G4 already had theirs.
- `JUNE_PORT_BACKLOG_ORDER.md` dated notes: Stage 4 (Scaleform runtime done, what quest-system calls and owns), Stage 6 and 8 (cut-in art `.gfx` in data152 / data195, names via the character checklist; Stage 8 also the Rose / Ain scenes),
  Stage 9 (root-UI screens and the unported wrapper callers; G5's gate note was already there), Stage 12 (`GFX_SKILL_CUT`).
- Memory `june-gfx-runtime-plan` rewritten: plan complete; corrects the stale "skill cut-ins stay textures" line (June's user skill cut-in is a GFx widget set, G4).
- `_DEBUG`: `JUNE_GFX_DEBUG`, `JUNE_GFX_TALK_CUTIN_DEBUG`, `JUNE_GFX_GATES_DEBUG` are all commented in `Always.h`; the deployed exe (G5 review, md5 01c5f8d1) already has them off, so no build or deploy.
- Still unproven in game, with owners: ESC-true branch and hide modes 2-5 (Stage 9), the left-facing cut-in `L_` variant (checked in Ghidra only), black layout layer (asset port). Next for the owner of each: see the notes above.

## Open questions (planning, 2026-10-17)

None block G0 / G1 / GA. G0 may raise one: if `CX2GFXEventScene` needs a packet or server value for a scene (none seen), it goes to
the user per memory `design-missing-june-pieces`.

## Play-test hand-off (every code phase)

Dev console first (auth level 3, END key). Copy-ready commands with real values; name the character and job from `els_db.sql`. For
each check: where / paste / what it does / look for / if not. The log tag is `[JUNE_GFX]` plus the phase's sub-tag.
