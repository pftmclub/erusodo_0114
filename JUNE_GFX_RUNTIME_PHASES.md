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
