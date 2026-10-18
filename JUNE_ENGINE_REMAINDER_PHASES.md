# June engine remainder: renderer contract, render types, UI framework, world timeline

Backlog Stage 2 (`JUNE_PORT_BACKLOG_ORDER.md`). Plan key `june-engine`, kickoffs in `kickoffs/june-engine/`, STATUS block
`june-engine`. Written 2026-10-16 by the Stage 2 planning session (Opus 5.5 high, brief `kickoffs/june-backlog/stage2-plan.md`).

## Context

Stage 2 was planned as "the 7 missing `RENDER_TYPE`s, `CUSTOM_RENDER_STATE` and June's `.fx` set", plus four measurements that
size Stages 5, 8 and 9. The measurements below change that picture:

- **The 7 "missing RENDER_TYPEs" are not render types.** June's `RENDER_TYPE` is March's, 19 members with the same values, in Lua
  and in C++ (June's technique table `337299c0` has 19 entries, 0-18). The 7 names are `SKIN_MESH_RENDER_TYPE.SMRT_*`, a table
  March's `Enum.lua` already has. March implements only `SMRT_UV_ANIM` (`KTDGXSkinAnim.cpp:934`, `:1089`).
- **The real renderer gap is a contract, not an enum:**
  - June's `CARTOON_TEX_TYPE` is renumbered (19 ramps; RED..SCARLET moved up by 3);
  - June's renderer has 33 effect parameters to March's 19 (rim light, colour grading, view-independent edge, texture ops,
    screen size, `g_mView`, `g_vEyeDir`);
  - June compiles `XRenderer.fx` with **`D3DXMACRO`s** March never passes;
  - two June-only renderers exist (`XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx`);
  - `CUSTOM_RENDER_STATE` is a June engine table with no March reader.
- **Models: no new base model** for any March character. June added motions (animation sets) and parts, not skeletons.
- **UI framework: small gap.** No new control type; 15 methods and 21 table keys over 30 / 21 of 1,519 June UI scripts.
- **World-event timeline** (from june-effect C9h-7): June-only, ticked from `CX2World::OnFrameMove`; the loader is not mapped yet.

## Rules

1. **June wins; no partial updates.** Where June changed a function March has, port June's whole function in the same pass.
   June bugs are kept verbatim with a `JUNE_BUGS.md` row.
2. **Flags.** One `SERV_IRUHADEV_JUNE_*` flag per logical change, in `KTDXLIB/Always.h`, original code in `#else`, `#endif FLAG`.
   Proposed names are in each phase; an RE phase may split a flag and says so in its Done note.
3. **June data is byte-exact.** `.fx`, `.bmp` and other assets are never edited (memory `june-assets-byte-exact-no-edits`). Their
   copies belong to june-asset-port (see *Asset routing*); this plan ports the C++ that makes them render as June.
4. **Lua.** `Enum.lua` (data036) is a core-list BOM source file: edits go through `luasrc.py` / `kom.py`; ask before every
   `/x2-kom-packed copy`. No new `.lua` files.
5. **Character material goes to the checklist** (memory `hand-char-port-material-to-char-ports`): class scripts that use a new
   render state, cartoon ramp or SMRT type are rows in `JUNE2017_CHARACTER_PORT_CHECKLIST.md`; this plan ports the engine side.
6. **Code waits for june-effect C12.** RE phases (Ghidra read-only, no code) can run now and alongside anything. Code phases run
   after C12 on the current branch, and never alongside another session editing `KTDGXRenderer.*`, `KTDGXSkinAnim.*` or
   `KTDXApp.cpp`.
7. **Opus phases delegate** mundane reads, greps, builds and log reads to subagents (memory `opus-phases-delegate-mundane-work`).

Flag family `SERV_IRUHADEV_JUNE_RENDER_*` / `SERV_IRUHADEV_JUNE_UI_*` / `SERV_IRUHADEV_JUNE_WORLD_*`. Log tag **`[JUNE_ENGINE]`**
(bracketed, so `python tools/logs.py --tag JUNE_ENGINE` works; each phase uses a sub-tag after it, e.g. `[JUNE_ENGINE] CTT`).

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "<heading>"` |
| June kom reader | `python tools/junekom.py list / extract` (never extract by hand) |
| June raw bytes at a VA (tables, strings) | `june2017_re/disasm_range.py`; for data, read the section from `unpacked_x2.exe` (image base `0x31aa0000`) |
| Ghidra | `ghidra` skill (read-only); verdict rows go to `june2017_re/funcmap.tsv`, tag `Engine <phase>` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| March `.lua` as source | `python tools/luasrc.py` (x2-lua), `/elstools pack`, then ask, then `/x2-kom-packed copy` |
| Flag hygiene | `python tools/flagcheck.py <FLAG> --worktree --files <files>` |
| Build + deploy | `python tools/build.py --deploy` (background) |
| Play-test logs | `python tools/logs.py --tag JUNE_ENGINE` |
| UI census | `python -I june2017_re/ui_dlg_census.py --workdir C:/Users/Iruha/AppData/Local/Temp/uidlg` (about 35 s) |

## Investigation ledger (measured 2026-10-16)

### M1 - shaders

June's `data041` has 7 `.fx` (`junekom.py list data041`); March's live and staged `data041` have 4, the same sizes. Diff:
`june2017_re/s2_fx_diff.tsv` (113 rows).

| File | Verdict | March load site |
|---|---|---|
| `GCSkin.fx`, `RenderState.fx`, `XPostEffect.fx` | same bytes | none / `KTDGGrayScale.cpp:16` / `KTDGPostEffect.cpp:147` |
| `XRenderer.fx` | differs: June 65,196 B, March 42,468 B, +697 / -137 lines | `KTDGXRenderer.cpp:1193`, `D3DXCreateEffect` with `pDefines = NULL` (`:1202`, `:1215`) |
| `XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx` | **June-only** (contradicts the 2026-10-08 asset census, which counted no June-only `.fx`) | none in March. June: `XEnvBoxRenderer.fx` string `3344dad0`; `XHeatShimmerRenderer.fx` loader `31d0b200` (**R0: `31d0b200` loads `XEnvBoxRenderer.fx`; the heat shimmer loader is `31ce5e90`**) |
| `XRenderer_X2TOOL.fx` | June-only, tool build | none in either client |

**June's `XRenderer.fx` contract** (June C++ handle init `31d3f480`, confirmed in disassembly):
- **Macros.** `X2APP_RENDERING_ENHANCE_DISABLE_SHADER30` only when renderer `+0x58 == 0` (`CMP byte [EBP+0x58],BL / JNZ` at
  `31d3f615`); `X2APP_RENDERING_ENHANCE_ENVIRONMENT_BOX` (""), `..._VIEW_INDEPENDENT_EDGE` (`"2.0"`) and `..._MIPMAP` ("") always.
  The `.fx` has these commented out in-file (`:30-31`), so loaded March's way it compiles June's shader-3.0 path with rim light
  present but **without** the environment box, colour grading, view-independent edge and mipmap paths June turns on.
- **Techniques.** Table `337299c0`, 19 entries = March's 19 names. June's file also has `COLOR_WIREFRAME`. Every shared
  technique moves to `vs_3_0` / `ps_3_0` (`vsArray*30`), with the `DISABLE_SHADER30` branch as the fallback.
- **Parameters.** Table `33729a58`, 33 entries (`0x20` loop bound at `31d3f7a2`). New in June: `g_mView` 15, `g_vEyeDir` 16,
  `g_iTextureOp_1/2` 17-18, `g_bIsUseEnvBoxRimLIght` 19, `g_vRimLightColor` 20, `g_fRimLightPower` 21, `g_fRimLightWide` 22,
  `g_fRimLightForced` 23, `g_bIsUseRimLight` 24, `g_bIsUseColorGrading` 29, `g_bIsUseViewIndependentEdge` 30,
  `g_fScreenSizeX/Y` 31-32. March sets none of them (no lookup or setter in KTDXLIB / X2Lib / X2). Every name March looks up
  (`KTDGXRenderer.cpp:25-43`, `:70-96`) exists in June's file with the same type.
- `RenderParam` gains a rim-light member (Lua key string `useRimLight`, `3353ca10`). **R0 correction (2026-10-16):** `useRimLight` is
  a Scaleform option-UI key; `RenderParam` gains two world-filled ints instead (R0 map item 3).

### M2 - render enums

Census: `june2017_re/s2_render_type_users.tsv`.

- **`RENDER_TYPE`:** identical, 19 members (June `Enum.src.lua:42-62`, March `Enum.lua:42-62`, `KTDGXRenderer.h:8-37`).
- **`SKIN_MESH_RENDER_TYPE`** (June `Enum.src.lua:17526`, March `Enum.lua:12419-12430`, same values): `NONE -1`, `REAL_COLOR 0`,
  `CARTOON_BLACK_EDGE 1`, `CARTOON_BLACK_EDGE_ALPHA 2`, `UV_ANIM 3`, `INVSRC_ALPHA_NOT_CULL 4`, `DESTALPHA_SRCALPHA_CCW 5`,
  `CUSTOM 6`, `CARTOON_ALPHA 7`. March parses the key (`X2GUNPC.cpp:4633`, `:4685`) and implements only `UV_ANIM`; other types draw
  normally (`KTDGXSkinAnim.cpp:934`). June Lua users: `SMRT_UV_ANIM` 24 data037 NPC scripts; `SMRT_CARTOON_BLACK_EDGE_ALPHA` once,
  as `RENDER_TYPE.SMRT_...` (nil in June too: a June bug).
- **`CARTOON_TEX_TYPE`, renumbered.** June: NORMAL 0, NORMAL_TWOTONE 1, NORMAL_USER1/2 2-3, RED 4, GREEN 5, VIOLET 6,
  NORMAL_COUNTER 7, DAMAGE 8, INVINCIBLE 9, PETRIFIED 10, SKYBLUE 11, SCARLET 12, RIM_LIGHT_WHITE 13, RED 14, BLUE 15, DARK 16,
  USER1/2 17-18, END 19. March C++ `KTDGXRenderer.h:39-55` has 0-9 (+ the Ranox stand-in `CTT_RIM_LIGHT_BLUE = 10` under
  `SERV_IRUHADEV_RANOX_RIM_LIGHT`, `CTT_END` 11); March `Enum.lua:63-74` has `CTT_RIM_LIGHT_BLUE = 10` but `CTT_END = 10`.
  June's binary loads the 19 ramps in June order plus `CartoonDamage_Raid.bmp` (strings `3344f900`-`3344fc5c`).
  March's mesh-player `.txt` reader accepts only `CTT_NORMAL` / `CTT_RED` (`KTDGXMeshPlayer.cpp:4709-4716`).
  June Lua users of names March lacks: `RIM_LIGHT_RED` (`WI_Boss_Raid_ELTRION_MK2_Stage3`), `RIM_LIGHT_DARK`
  (`WI_Boss_Raid_PERKISAS_Stage1`).
- **`CUSTOM_RENDER_STATE`** (June `Enum.src.lua:17473`, CRS_NONE 0 .. CRS_EMOTION_INVISIBLE 9): no March table, no March reader.
  March's unrelated `X2GUAdd.h:485-491` enum shares the name with different values (`QUICKSILVER_FRENZY_BUFF` 2 vs June's 4).
  June Lua users: `CRS_INVISIBLE` 7 files (Add_NasodRuler, Ain_Apostasia, Ain_Erbluhen_Emotion, Ara_Martial_Artist,
  Luciel_Demonio, Rose_Ranger, RidingPet_Rakshasa), `CRS_UNIT_FADE_WHITE_WITHOUT_WEAPON` 1 (Add_NasodRuler). The checklist
  (`JUNE2017_CHARACTER_PORT_CHECKLIST.md:152`) records the Add / Ara script truncation. June reader known so far: `31fce500`
  (riding pet, `X2RidingPet.cpp:1404`).

### M3 - character models

Core-file evidence: `june2017_re/s2_model_core.tsv` (366 rows). March side = the original `kom_contents/dataNNN.kom` (the live koms
already carry June bytes for 953 swept names, e.g. `Motion_ARA_Lobby.x`).

**All 9 March characters: new motions only, no new base model.** Templet motion file names are unchanged
(`X2UnitManager.cpp:854`, `:872-879`; June `UnitTemplet.src.lua`); every shared KSM core motion keeps its frame count and bone
parent map; the base meshes' bone-name sets are identical. What changed:

| Character | Core motion files changed | Animation sets (unique names) | New motion files |
|---|---|---|---|
| Elsword | 11/11 | 235 -> 300 | - (UC_150 reuses SK / LK) |
| Aisha | 10/11 | 207 -> 273 | - |
| Rena | 11/11 | 252 -> 315 | - |
| Raven | 11/11 | 266 -> 341 | - (sphere containers added) |
| Eve | 11/11 | 222 -> 276 | - |
| Chung | 11/11 | 286 -> 347 | - (sphere containers added) |
| Ara | 9/9 | 223 -> 281 | `Motion_Ara_LS.x`, `Motion_Ara_AA.x` |
| Elesis | 9/9 | 206 -> 260 | `Motion_EL_CA.x`, `Motion_EL_DK.x` |
| Add | 7/7 | 175 -> 232 | `Motion_ADD_AT / _DE / _MM / _TT.x` |

Also: face `.xet` 3.5-5x larger for all 9; face `.x` differs for 6; hair meshes differ for most (some name new textures);
53-54 June-only emotion motions per character. Geometry change is not proven, only bytes. **Lu/Ciel, Rose, Ain** are June-only in
every bucket (`Motion_Lu.x` data208, `Motion_ROSE.X` data190, `Motion_Ain.X` data209; basic meshes 41870-76 / 42020-26 /
42153-59).

**Consequence:** Stage 5/7 character ports need no skeleton or skin-mesh engine work; they take June's motion files whole (asset
port P8) and the face XET with them. Stage 8 is data plus new unit types.

### M4 - UI framework

Script `june2017_re/ui_dlg_census.py`, output `june2017_re/ui_dlg_census.tsv`. March UI bindings: 20 classes,
`KTDXLIB/KTDXApp.cpp:1897-2574`. June: 1,519 UI scripts. Every `Create*` control June calls is bound in March: **no new control
type.** Unknown table keys are ignored silently (`LUA_GET_VALUE` with defaults); unknown methods are nil-call errors.

| Gap (June files) | Owner |
|---|---|
| `SetDisableUnderWindow` (11): C++ exists, `KTDGUIDialog.h:366`, unbound | **U1** |
| nation-suffix keys `POS_<CC>`, `FONT_INDEX_<CC>`, `LEFT_TOP_<CC>`, `RIGHT_BOTTOM_GLOBAL` (1-6 each; `POS_US` 3, `FONT_INDEX_US` 2) | **U1** |
| `LINE_BREAK_LENGTH` (4), `LINE_BREAK_TYPE` (2) | **U1** |
| `SetMovieSubtitleIndex` (4) | **U1** |
| `IS_FOR_MINOR_UNIT` (5, slot data) | **U1** |
| `ROTATION` (3): March's reader is `#ifdef DLL_BUILD` (`KTDGUIControl.cpp:770`), dead in the client | **U0** decides (live in June?) |
| one file each: `CPictureData:SetColor`, `CPictureData:GetSize`, `AddDummyInt_Flag`, `SetFont_Flag`, `SetCaretOffSet`, `SetGroupOffset`, `SetChangeFocusByKeyMsg`, `SetCustomMsgMouseOverOnItem` / `OutOnItem`, `SetScrollBarOffsetPos`, key `m_PrivateType` | **U1** |
| `GetNationFlag` (7), `CPictureData:SetChangeUVPoint` (1) | Atlas **A4** (`ATLAS_PORT_PHASES.md` ledger) |
| `AddVillageNpcRelationDataByQuest_LUA` / `ByVillage_LUA` | Elysion **E1** (june-effect C9h-2e) |
| 2,107 `STR_ID_<n>` and 327 other names from Lua tables March lacks (290 scripts); 651 missing fields of March tables | Lua data, not framework: the owning ports' `Enum` / string adoption (Stage 9 sweep for whatever is left) |

### M5 - effect-engine check

**Pending june-effect C11** (running 2026-10-16 in a peer session; this session did not rerun its census or trial load). Phase
**Z** records C11's Done note here and confirms which Stage 0 effect items closed.

### World-event timeline (from june-effect C9h-7)

`CX2World::OnFrameMove` `32802140` calls `32801580( dt )`, which walks world `+0x34`'s active list (`+0x294`, int indices) into a
0x1c-byte entry array (`+0x2a4`). Each entry's current 0x34-byte block advances a timer and fires three tracks once their time is
reached: 0x58-byte rows -> `327fc8c0`, 0x38 -> `328004a0`, 0x3c -> `32800e10`. Per C9h-7: type 1 sound (device auto-release),
type 3 world `+0x18c/+0x190`, types 4-8 game / unit actions. Loader, Lua keys and March-data reach: not mapped (W0).

**Correction (W0, 2026-10-16): this is not a June-only system.** It is March's own line-map trigger runtime (`WORLD_TRIGGER`, live):
`32801580` = `CX2World::DoTriggerFrameMove`, the three handlers = `DoTriggerMesh` / `DoTriggerLine` / `DoTriggerEtc`, world `+0x34`
= the line map, `+0x294` / `+0x2a4` = `LINEMAP_TRIGGER::m_vecNowTrigger` / `m_vecTriggerSeg`, loader `CKTDGLineMap::AddTrigger_LUA`
`31c77e20`. Type 3 is the trigger camera, not a world colour. The gap is a delta, see *Phase W0 -- Done*.

### Asset routing (june-asset-port owns the copies)

| Data | June kom | Plan phase |
|---|---|---|
| `XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx`, `XRenderer_X2TOOL.fx` (June-only) | data041 | june-asset-port **P4a** (inert until R3 / R4) |
| 10 June-only `Cartoon*.bmp` ramps (`_Twotone`, `_User1/2`, `RimLight_*` x6, `Damage_Raid`) | data042 | june-asset-port **P4a** |
| `XRenderer.fx` (differs) | data041 | june-asset-port **P10**, which must wait for **R2** (dated note in P10, 2026-10-16) |

## Phases

| Phase | What | Run with | Code | Play-test |
|---|---|---|---|---|
| R0 | Renderer contract RE: June's renderer, `RenderParam`, cartoon table, setters, macros, `+0x58`, SMRT switch, CRS, the two June-only renderers | Opus 5.5 high | no | no |
| R0b | Render / skin-mesh sweep: June vs March function by function, outside R0's list | Opus 5.5 high | no | no |
| R1 | `CARTOON_TEX_TYPE` to June's numbering, 19 ramps + `Damage_Raid` | Sonnet 5.5 high + review | yes | yes |
| R2 | `XRenderer.fx` loader macros, the 14 new parameters, rim light in `RenderParam` | Sonnet 5.5 high + review | yes | yes |
| R3 | Environment box / colour grading (`XEnvBoxRenderer.fx`) | Sonnet 5.5 high + review (Opus if R0 leaves gaps) | yes | yes |
| R4 | Heat shimmer RE (2026-10-18: RE only; the shimmer is dead in June, the port moved to R4a-R4d) | Opus 5.5 high | no | no |
| R4a | Pass-1 layer (object post-effect slots, DG pass-1 lists, dialog render pass) + heat-shimmer object (added by R4) | Opus 5.5 high + review | yes | yes |
| R4b | Frame render `31be3800`, stage slots, render targets (MSAA), `ANTIALIASING_LEVEL` (added by R4) | Opus 5.5 high + review | yes | yes |
| R4c | State render split: `CX2State` + the March state classes into June's three pass slots (added by R4) | Opus 5.5 high + review | yes | yes |
| R4d | Game render split: `CX2Game` + the March game classes (added by R4) | Opus 5.5 high + review | yes | yes |
| R4f | Software cursor `CKTDXCursor` (app `+0x290`, drawn at frame end; added by R4b) | Opus 5.5 high (map, then port) + review | yes | yes |
| R4g | Render-target call sites take June's arguments (opens, four-argument begins, last-kill shot under AA; added by R4b) | Opus 5.5 high (map, then port) + review | yes | yes |
| R5 | `CUSTOM_RENDER_STATE`: table, reader, render | Sonnet 5.5 high + review | yes | yes |
| R6 | `SKIN_MESH_RENDER_TYPE` 1, 2, 4-7, and (R0b) June's whole skin-anim draw | Opus 5.5 high (map, then port) + review | yes | yes |
| R7 | Hookable textures: per-mesh hook id, device-manager table, `HookableTextureData.lua` (added by R0b) | Opus 5.5 high (map, then port) + review | yes | yes |
| R8 | DG manager render lists: state blocks, over-UI chains, front-UI dialog bands, batching, state-manager reset (added by R0b) | Opus 5.5 high (map, then port) + review | yes | yes |
| R9 | Render small deltas: particle RT_ADD_*, effect alpha (RHW, skin), `+0x1ac`, outline clamp, state-manager lost device, skin-mesh loads (added by R0b) | Sonnet 5.5 high + Opus review | yes | yes |
| R10 | Skin-anim playback sweep: the non-render `CKTDGXSkinAnim` functions (added by R0b) | Opus 5.5 high | no | no |
| R11 | Skin-anim playback and anim-set bookkeeping: play type 3, transition bytes, `+0x294` backup map, mixed-motion list, bone refresh (added by R10) | Sonnet 5.5 high + Opus review | yes | yes |
| R12 | Skin-anim device ownership: June's reference counting of meshes and XETs, and its callers (added by R10) | Opus 5.5 high (map, then port) + review | yes | yes |
| U0 | UI framework RE: June's implementation of each U1 gap | Opus 5.5 high | no | no |
| U1 | UI framework port | Sonnet 5.5 high + review | yes | yes |
| W0 | World-event timeline RE: loader, data, reach | Opus 5.5 high | no | no |
| W1 | World-event timeline port (conditional on W0) | Sonnet 5.5 high + review | yes | yes |
| W1b | `AUTO_DISABLE_LINE`: map the physics fallback, then port (added by W0) | Opus 5.5 high + review | yes | yes |
| W2 | Portal-line markers: `PVE_PotalGate`, `SHOW_PORTAL`, `LINE_PORTAL_TYPE` (added by W0) | Opus 5.5 high + review | yes | yes |
| Z | Close-out: M5 from C11, backlog, MODS.md | Sonnet 5.5 medium | no | no |

**Model choice.** The RE phases (R0, R0b, U0, W0) decide what June does from the binary and settle new mechanisms: Opus, where a
wrong reading costs a whole port. The code phases transcribe a written map: Sonnet high, each with a scoped Opus review because the
renderer fails visibly but subtly (a wrong blend or ramp index looks "almost right"). R3 moves to Opus if R0 cannot map the
environment box fully.

**Order (R0b, 2026-10-17).** R7 and R9 come after R2 (`KTDGXRenderer.cpp`); R8 not alongside R4 (`KTDGManager.*`) or U1 (dialog
manager); R6 after R2 as before; R10 is read-only and runs now.

**Order (R10, 2026-10-17).** R11 and R12 edit `KTDGXSkinAnim.*`: one at a time, never alongside R6. R12 comes after R11 (both
change `CleanAnimStructure` and `SetAnimXSkinMesh`).

**Order (R4, 2026-10-18).** R4a -> R4b -> R4c -> R4d, one at a time. R4a edits `KTDGManager.*` and the dialog manager (not alongside R8-style
work or U1); R4b edits `KTDXApp.cpp` (check U1 / Atlas Touches), the render-target class and `X2GameOption.*`; R4c / R4d edit the `X2State*` /
`X2Game*` render functions.

**Order.** R0, R0b, U0 and W0 can run now, in parallel (read-only). Code: R1 -> R2 -> R3 / R4 / R5 / R6 (R3 needs R2; the others
need only R0 and touch different files, but run one at a time: they share `KTDGXRenderer.*` / `KTDGXSkinAnim.*` and the build).
U1 runs after U0, independent of the R chain apart from the build lock and `KTDXApp.cpp` (also touched by Atlas A4: check STATUS
Touches). W1 after W0. W1, W1b and W2 (added by W0) share `X2World.cpp` / `KTDGLineMap.*`: one at a time, and none alongside
atlas E5.

## Phase R0 - Renderer contract RE

**Run with:** Opus 5.5 high. No code. Funcmap rows `Engine R0`.

Map, with addresses, everything the R1-R6 transcriptions need:
1. **Renderer object.** `31d3f480` (handle init): the handle array (+0x8c..), technique handles, every member it writes, and
   **what `+0x58` is** (option, device caps, or both; who writes it). If it is a game option, its UI goes to Stage 9.
2. **Every setter** of the 14 new parameters (`33729a58` indices 15-32) and where in the frame they are set, per render type.
   Which values come from `RenderParam`, the camera, the device (screen size) or constants.
3. **`RenderParam`** in June: new members (`useRimLight` key `3353ca10`, rim colour / power / wide / forced?), their Lua keys and
   readers (`32eb7d50` is the buff displayer's; find the unit / NPC / mesh-player / particle ones), defaults, and the copy
   ctor / assignment if the layout changed.
4. **Cartoon ramps.** June's ramp load (the 20 strings `3344f900`-`3344fc5c`): table order, where `CartoonDamage_Raid.bmp` is used
   (not in the enum), every reader indexed by `CARTOON_TEX_TYPE`, and June's mesh-player `.txt` `CARTOONTEXTYPE` reader (string
   `33449a74`, error `33449f5c`): which names it accepts.
5. **`SKIN_MESH_RENDER_TYPE`** switch in June's skin-anim render (March implements UV_ANIM only): every case.
6. **`CUSTOM_RENDER_STATE`:** where June declares it (C++ enum and Lua binding), every reader (`31fce500` and the rest: units, NPC,
   riding pet, emotions), and what each CRS value renders.
7. **June-only renderers:** the owner of `XEnvBoxRenderer.fx` (string `3344dad0`) and of `XHeatShimmerRenderer.fx` (`31d0b200`),
   their per-frame callers, parameters, and **reach**: which June data or option turns each on. Reach 0 offline goes into the
   phase's row as a decision for the user, with a recommendation.
8. Record the funcmap rows owed by the planning session (see *Funcmap rows written by planning*) as confirmed or corrected.

**Done gate:** a section "R0 map" per item (short; tables to `june2017_re/r0_*.tsv`), every R1-R6 kickoff checked against it and
amended in `kickoffs/june-engine/`, class-specific readers handed to the checklist.

### R0 map (2026-10-16, Opus 5.5)

No code, so there is no flag or build. The text of this map landed in peer commit `e2f2029e` (June engine U0, which staged it by accident); the evidence, funcmap rows and kickoff notes are in the R0 commit. Evidence is in `june2017_re/r0_renderer_contract.tsv`, and the funcmap rows are tagged
`Engine R0`.

1. **Renderer object** (`CKTDGXRenderer`: vtable `3344f8ec`, ctor `31d3c270`, OnResetDevice `31d3f480`).
   - Members: ramps `+0x08` [20], `+0x58`..`+0x88` new, techniques `+0x8c` [19], parameter handles `+0xd8` [33], then March's tail
     at `+0x15c`. The new rim and grading members are `+0x18c` / `+0x18d` / `+0x190`.
   - **`+0x58` comes from device caps only, not an option:** `PixelShaderVersion >= 3.0`. Its UI does not go to Stage 9.
   - `+0x84` / `+0x88` hold the anisotropy in use and the cap; `+0x82` is the mipmap cap.
   - `+0x80` (env box) is set from `USE_ENV_BOX && +0x58`; `+0x81` (view-independent edge) from `USE_VIEW_INDEPENDENT_EDGE`.
     Both are written by the option apply `32421110`.
2. **Setters** (`BeginRender` `31d3cf80`, `OnFrameRender` `31d3e7d0`, ret 0x28):
   - `g_mView` is set from the camera view and `g_vEyeDir` from camera look-at minus eye; `g_iTextureOp_1/2` = 1, or per texture stage.
   - Screen size comes from a device global, and the view-independent edge from `+0x81` (cartoon cases only).
   - Rim: `g_bIsUseRimLight` = `+0x18d` = (cartoon case with a ramp) && **`+0x18c`**. `CX2Main` vslot 1 (`31d90300`) copies the
     `RIM_LIGHT` option into it every frame (`CX2Main +0x280` = `CX2GameOption +0x100`; the option object is at `CX2Main +0x180`).
   - When the env box is on, rim colour, power, wide and forced come from the env box (`+0x68..+0x7c`) and `g_bIsUseEnvBoxRimLIght` = 1.
     Otherwise the rim ramp `+0x190` goes to stage 2.
   - `g_bIsUseColorGrading` = `+0x5b` (needs an env-box LUT).
   - **`EFFECT_PARAMETER` is renumbered, not appended:** March's 15-18 (`g_fColor` .. `g_iShaderIndex`) become June's 25-28.
3. **`RenderParam`** gains two ints before `bAlphaTestEnable`, which moves from `+0x9c` to `+0xa4` (+8 bytes):
   - `+0x9c` is the world cartoon index, used only when `cartoonTexType == CTT_NORMAL` (0..3 = NORMAL / TWOTONE / USER1 / USER2).
   - `+0xa0` is the world rim ramp, clamped to 13..18 with 13 (white) as the fallback.
   - **No Lua key fills them.** Five per-frame fills copy `CX2World +0x1a4 / +0x1a8`: user, square / agit unit, pet, NPC and riding pet
     (tsv `rpfill`; pet, square / agit unit and riding pet by RTTI, user and NPC by address only). The world values come from Lua `SetCartoonTextureIndex` / `SetRimLightTextureIndex` and default to 0 (candidate), so the rim
     ramp falls back to white.
   - **Correction to the ledger: `useRimLight` (`3353ca10`) is a Scaleform option-UI key,** not a `RenderParam` member.
4. **Ramps.**
   - June loads 20 ramps in June's enum order, plus `CartoonDamage_Raid.bmp` at slot 19 (= `CTT_END`). The skin-anim pass treats
     cartoon type 19 like `CTT_DAMAGE` (ADD_ render types). No writer of 19 was found in R0; R1's sweep looks for one.
   - The `.txt` `CARTOONTEXTYPE` reader (`31c864f0`) still accepts only `CTT_NORMAL` / `CTT_RED`, with RED = 4.
   - Raw-int CTT stores: the pet, riding-pet and world-object Lua readers (tsv `ctt-reader`).
5. **SMRT** (`31d30380`): all 8 cases are in the tsv (`smrt`). R6 ports 0-2 and 4-7; case 3 is March's existing port, re-checked
   against the tsv (`alphaBlend` from mesh `+0xc8`).
6. **CRS.**
   - The reader is the generic state key `CUSTOM_RENDER_STATE = { start, end, CRS }` (`31fce500`). It is called from the user state
     loader (`32057b80`) and the riding-pet state loader (`32d2f560`), which copies its triple into the rider. **It is not a riding-pet
     reader.**
   - The consumer is the user render setup `31fd4460`: buff override `+0x8e64`, else the state's CRS inside its time window. It
     renders cases 3 and 5-9; 0-2, 4 and >9 take the default path `32275140`. A second consumer `320643f0` is left to R5.
   - There is no NPC reader. Class users go to checklist 7w.
7. **June-only renderers.**
   - **Env box** (`CKTDGEnvBoxMap`, OnResetDevice `31d0b200`, *not* heat shimmer): **reach 0.** Only Lua `g_pWorld:SetEnvBoxMap`
     creates it, and none of June's 7,347 `.lua` call it or `g_pEnvBoxMap`. With it, colour grading is unreachable too. Open question 5.
   - **Heat shimmer** (`DGManager +0x384`, OnResetDevice `31ce5e90`): a full-screen composite in `CKTDXApp`'s frame render
     (`31be3800`). Stages draw a second pass into `EffectRenderTarget`, which is distorted through June-only
     `PerturbationMap.png` (data045). **Reach is not 0:** it runs whenever the scene render targets exist. They are gated by option
     `+0x34`, whose writer R0 did not find. R4 starts with an Opus RE step (R4 kickoff), not a user question.
   - **Options:** June defaults are `RIM_LIGHT` off (data002 `GameOptions.lua:40`), view-independent edge on, anisotropy 0 (stays 1),
     env box off. March has none of these keys. Open question 4.
8. **Planning rows:**
   - `31d3f480` is confirmed (`+0x58` settled).
   - `31d0b200` is **corrected** (env box; heat shimmer is `31ce5e90`).
   - `31c86270` is **corrected** (it is `ProcessRenderType`, `KTDGXMeshPlayer.cpp:4661`, not a static name table).
   - `337299c0` and `33729a58` stand. The world rows are W0's.

Kickoffs amended: R1 (stores, slot 19, `.txt` RED = 4), R2 (no Lua key, renumbered enum, `RenderParam` +8, option chain), R3 (reach 0,
waits on question 5), R4 (Opus RE first), R5 (generic reader, rider copy, cases), R6 (tsv cases).

## Phase R0b - Render and skin-mesh sweep

**Run with:** Opus 5.5 high. No code.

The backlog's "any other June engine-side difference in renderer or skin-mesh code March already has". Compare June with March
function by function for `CKTDGXRenderer`, `CKTDGXSkinAnim` (render paths), `CKTDXDeviceXSkinMesh` (load), `CKTDGXMeshPlayer`
render, `CKTDGParticleSystem` render, `CKTDGPostEffect`, `CKTDGGrayScale` and the `CKTDGManager` render lists, excluding what R0
and earlier june-effect phases already mapped (funcmap `Effect *` rows). Each difference gets a funcmap row and either a new phase
here (R7+) or an owner. **Done gate:** every function of the list has a verdict row (same / differs -> phase / owner).

### Phase R0b -- Done

**2026-10-17, Opus 5.5.** No code, no flag, no build. Evidence: `june2017_re/r0b_render_sweep.tsv` (139 rows, one per March function
of the list plus the June-only functions found; columns verdict / owner / evidence with June addresses) and 88 funcmap rows tagged
`Engine R0b`. Verdicts: 56 same, 37 dead or tool-only in both, 10 already mapped (R0 / Effect), 35 differ, 1 unsure (R10).

**Corrections.**
- **`31d3e7d0` is `CKTDGXRenderer::DrawFrame`, not the XMesh `OnFrameRender`.** Its ret is 0x28, its callers are skin-anim only, and it makes 6 `_SetNowTexture` calls.
  - The XMesh `OnFrameRender` is `31d3ca70` and equals March's.
  - So every R0 "OnFrameRender" setter applies to skin meshes only. R2 is amended.
- **`31cf16b0` is the particle skin-mesh draw; the mesh draw is `31cf10c0`** (jump table `31cf781c`). Both funcmap rows are rewritten in place.
  - C10b-2's mesh-draw port is right, but its comment cites the wrong address (R9).

**Same / dead.**
- **Renderer:** `SetColor`, `_CommitChanges`, `Begin` / `NextRenderGroup`, `NextRender` and the XMesh `OnFrameRender` match March.
- **Particles and mesh player:** `Prepare` of both, the billboard and render-type parsers, and particle-system reset / lost (`S_OK` in both) match March.
- **Skin mesh:** the KSM / D3DX parse matches March (`ReadKSM`, `Optimize`, `Convert`, frame lists, collision prefixes, hierarchy).
- **Skin anim:** `Prepare` and the light-flow setters match March.
- **DG manager:** `Begin` / `EndRender`, the non-alpha render, `Add` / `Remove`, `MapLayerGroup`, the UI / non-alpha sorts and `IsInFrustum( obj, float& )` match March.
- **Dead in both:** `CKTDGPostEffect` (`ADD_POSTEFFECT` off; it compiles empty) and `CKTDGGrayScale` (no caller in March, absent in June) are dead in both, as are `_RenderBoundingSphere` and the layer renders. The serializers are tool-only.

**Differences and owners** (details and addresses in the sweep):
- **R2:** `DrawFrame` setters (above); `RenderParam` ctor `31c8f020` confirms the +8 layout.
- **R4 (pass 1 / heat shimmer):**
  - `CKTDXStage` gains pass virtuals `0xc` / `0x10` / `0x14` / `0x18`; KTDXApp `31be3800` runs pass 0, then pass 1 into the effect target.
  - `CKTDGObject` gains vslot `0x10` (post-effect draw) and vslot `0x2c` (is post-effect). The particle predicate reads March's already-parsed `m_bPostEffect`.
  - DG helpers `31ce80c0` / `31ce9470` / `31cda1a0`, `FrontUIRender` pass 1, and the shimmer in the ctor / dtor / reset.
  - Vslots `0x20` / `0x24` (Set/GetScaleByUnit made virtual) have no override, so there is no behaviour change. R4 kickoff amended.
- **R6 (now Opus):** June rebuilt the whole skin-anim draw `31d30380`:
  - the per-mesh RenderParam pass `31d2f000` (mesh `+0xa2c` / `+0x980`) replaces the fixed `ARA_CHARACTER_BASE` pass;
  - the lists are de-duplicated by name;
  - meshes are hidden by `+0x851` / `+0x8d0`;
  - 0x114-byte frames carry a visible byte at `+0xf8`;
  - mesh `+0xa30` defaults to 0.01f.
  R6 kickoff amended.
- **R7 (new): hookable textures.** `_SetNowTexture` has a 10th argument, the skin-mesh hook id `+0xa40`, which `DrawFrame` passes. It drives a device-manager table filled by June-only `HookableTextureData.lua` (data036, 4,599 rows: per-job and cash-item face swaps). March has none of it.
- **R8 (new): DG render lists.**
  - The state blocks go from 12 to 14 (additive texture, two sampler-carrying dialog blocks; March's UIDialog block is gone).
  - The over-UI chains go from 2 to 3 (split at layer 500).
  - `FrontUIRender` draws dialogs in two bands (0..3, then 4..500 above the fade). This finishes Atlas E7's partial port.
  - The main hint loop no longer batches with `BeginRenderGroup`.
  - The not-cull test moved before the show test; an alpha-sort tie-break and a 3rd frustum argument are added.
  - `CKTDGStateManager` Restore / Invalidate moved out of the DG reset.
- **R9 (new): small deltas.**
  - Particle `RT_ADD_*` names.
  - Effect alpha in the RHW and skin-mesh particle draws (a C10b-2 gap).
  - Sequence alpha `+0x1ac` (ctor 1.0, no writer found).
  - Outline clamp `31cfe050`.
  - The renderer `OnLostDevice` no longer resets the effect state manager; June's manager is a `CKTDXStage` with its own handler (`31d184f0`).
  - The XET name rule and close-by-pointer in the skin-mesh load / unload.
- **R10 (new, RE):** skin-anim playback (a 4th play type, transition bytes, `std::map +0x294`, dtor). Outside this sweep's render list.
- **R1:** the dtor closes 20 ramps; the note is in the R1 review kickoff.
- **Deferred June-loader plan:** `_Load`'s buffer acquisition (file reads + XOR / zlib, three sources) differs. Its parse is March's. March reads the June meshes repacked, so there is nothing to port now.

**Open for the user:** none. Every difference has an owner and every new phase follows June. The R7 / R8 play-tests need the user later.

## Phase R1 - CARTOON_TEX_TYPE to June

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX`. Sub-tag `[JUNE_ENGINE] CTT`.

- C++ enum to June's values (`KTDGXRenderer.h:39`); the ramp table loads June's 19 files in June's order, plus `Damage_Raid` where
  R0 says.
- **Supersedes the Ranox stand-in** `SERV_IRUHADEV_RANOX_RIM_LIGHT` (value 10, `CartoonSkyBlue.bmp`, `KTDGXRenderer.cpp:456-549`,
  `:1307`): flag-on uses June's `CTT_RIM_LIGHT_BLUE = 15` and `CartoonRimLight_Blue.bmp`. Keep the Ranox flag's code reachable.
- Sweep for raw-integer CTT uses (literals, arrays sized by `CTT_END`, saved values) in `KTDXLIB` / `X2Lib`; every hit is fixed
  under the flag or listed as safe.
- `Enum.lua` (data036, core list): June's `CARTOON_TEX_TYPE` table verbatim (fixes the `CTT_END` 10 / 11 mismatch); pack, ask,
  copy.
- Mesh-player `.txt` reader to June's accepted names (R0 item 4).
- **Needs the 10 June-only ramps live** (data042: `_Twotone`, `_User1/2`, `RimLight_*` x6, `Damage_Raid`). Without them the Ranox
  secret boss loses the rim light it has today through the stand-in. They come either from asset P4a or from a small data042
  pull-forward (open question 3). A ramp still missing at load logs once per name (degrade, no popup).
- **The exe and data036 move together** (as the K2 `item.kim` rule): with the flag off, June's `Enum.lua` values (RED 4 ..
  SCARLET 12, BLUE 15) index March's 10/11-slot ramp array; with the flag on, March's old `Enum.lua` maps RED (1) to TWOTONE.
  Deploy the exe and copy data036 in the same hand-off, play-test only when both are live, and revert both together (the
  `MODS.md` line names the flag **and** `Enum.lua`).
- **Play-test:** a Ranox secret boss (rim light blue), a petrified / invincible unit, a cartoon NPC: colours as before.

### Phase R1 -- Done

**2026-10-17, Sonnet 5.5.** Flag `SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX` (`Always.h:7546`). Code is one commit; data is outside the repo (kom_contents).

- **Open question 3 (ramps): recommendation taken, pull forward.** The user had not answered; the 10 June-only `Cartoon*.bmp` were staged with
  `kom.py stage-june` (June bytes, `data042_kom`, manifest `0`), packed and copied. The pack also shows 11 `~ changed` data042 files
  (`CartoonFire.BMP` .. `UIEdge.tga`): they were already staged from 2026-09-22 and are byte-identical to June's, so the copy only makes live
  match staged. `kom_backup_tools\data042.kom.20261017_192312` holds the old archive.
- **Header** (`KTDGXRenderer.h`): June's enum (`NORMAL` 0, `TWOTONE` 1, `USER1/2` 2-3, `RED` 4 .. `SCARLET` 12, `RIM_LIGHT_WHITE/RED/BLUE/DARK/USER1/USER2` 13-18,
  `CTT_END` 19), explicit values. The Ranox member is in the `#else` enum only. `m_pCartoonTexList` is `[CTT_END + 1]` (20).
- **Renderer** (`KTDGXRenderer.cpp`): ctor / dtor loop all 20 slots (R0b's dtor note). `OnResetDevice` loads June's 20 ramps in June's order
  (`31d3f8c1-31d3fd50`, slot 19 = `CartoonDamage_Raid.bmp`) with the original loading in `#else`. A missing ramp logs once per name:
  `[JUNE_ENGINE] CTT ramp <i> missing: <file>`; one summary line `[JUNE_ENGINE] CTT ramps loaded N/20 (CTT_END 19, RIM_LIGHT_BLUE 15)` (ErrorLog.txt).
- **Ranox stand-in:** `CTT_RIM_LIGHT_BLUE` is June's 15, but the rim map cannot be a cel ramp (see Play-test 1), so the stand-in is mapped to the
  `CTT_SKYBLUE` ramp until R2.
- **Sweep (raw-int CTT uses): none needing a fix.** Every C++ store is a `CTT_*` symbol (units, NPC, pet, riding pet, world object, buff
  displayer, particle, eqip, unit viewer) or a Lua `CARTOON_TEX_TYPE.<name>` through `LUA_GET_VALUE_ENUM` / `static_cast`, so they follow the enum and
  `Enum.lua`. `X2World::SetRimLightTextureIndex` casts the Lua int, valid 0..18 (19 = disabled sentinel, as before). June's 120 `.lua`
  uses are all names (no raw ints, no `CTT_END`, no writer of 19), so slot 19 stays loaded and unreached. `X2UICouponBox` `CTT_*` and
  `CSI_CTT_*` skills are unrelated.
- **`.txt` reader (`KTDGXMeshPlayer.cpp:4709`): no change.** It maps `CTT_NORMAL` / `CTT_RED` to the enum symbols, which are June's 0 and 4 now. Nothing else accepted, as June.
- **`Enum.lua`** (data036, core list): `CARTOON_TEX_TYPE` = June's table verbatim plus one comment line; `luasrc.py refresh` + `ledger --check` PASS.
  Packed and copied (`kom_backup_tools\data036.kom.20261017_192311`); only `Enum.lua` changed vs live.
- `MODS.md`: row + paragraph (revert: flag **and** `Enum.lua` together). No JUNE_BUGS row (nothing kept from June that is a bug).

Hygiene: `files: 2 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s),
0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
Deploy: `DEPLOY OK: X2_offline.exe is 19,760,640 B, dated 2026-10-17 19:24:04, md5 08737e3d17e9a597dc927df175864325, equal to X2/US_SERVICE/x2.exe (2026-10-17 19:23:58); built at main 7ccddf89 + 3 uncommitted code file(s).` The
three new literals (`CartoonRimLight_Blue.bmp`, `CartoonDamage_Raid.bmp`, the two `[JUNE_ENGINE] CTT` strings) are in the exe.

**Play-test 1 (2026-10-17): check 3 FAILED, units and boss black.** Cause: June's `CartoonRimLight_*.bmp` are 256x8 additive rim overlays (black except the
far edge), not cel ramps; binding one in cel stage 1 multiplies units to black. Fix (same flag, `SetRimLightTexType` in `KTDGXRenderer.h`): the Ranox stand-in maps
`CTT_RIM_LIGHT_BLUE` to `CTT_SKYBLUE` (its pre-R1 tint), other rim types stay disabled as before R1. R2 binds the rim ramp in stage 2 and removes this mapping.
Redeploy: `DEPLOY OK: X2_offline.exe is 19,760,640 B, dated 2026-10-17 19:43:39, md5 0f3a14452992575ffd46e3d4c68773e2, equal to X2/US_SERVICE/x2.exe (2026-10-17 19:43:33); built at main 02ddf7b3 + 1 uncommitted code file(s).` Re-test check 3 only.

**Play-test (user, Freils uid 18, Chung Tactical Trooper; tag `[JUNE_ENGINE] CTT`):** all three checks need exe + data036 + data042 live (they are).
1. *Start-up.* Launch `start_offline.bat`, reach character select, quit. Look for `[JUNE_ENGINE] CTT ramps loaded 20/20` in ErrorLog.txt and no `ramp ... missing`.
2. *Colours as before.* Ranox Geyser Expert (33290), console `/stage 2`, `/invin 0`: let a monster hit Freils (red damage flash), watch a petrified
   or invincible unit if one appears, and look at a cartoon NPC. Look for: same tints as the last regression run, no white or black models.
3. *Ranox Secret Hell boss rim light.* Enter Ranox Secret Hell (41271), console `/stage 4` (the boss stage = the last stage; check the log for world
   `WI_RANOX_SECRET_BOSS_STAGE`). Look for: a blue tint on units as today. If it is a different blue or washed out, report it (see the stand-in note).
   If a model goes white / wrong in 2: that is `Enum.lua` and the exe out of step (check data036 md5 `a7376f93...`).

### Phase R1 -- Review

**2026-10-17, Opus 5.5. Verdict: PASS, no transcription error.** Reviewed `02ddf7b3` + `05317e97` against R0 item 4, R0b's dtor row and June.

- **Enum:** all 20 values equal June's `Enum.src.lua` `CARTOON_TEX_TYPE` (`june_src_fixed/data036_kom`). The staged data036 `Enum.lua` table
  equals it too: BOM kept, plus one comment line.
- **Ghidra spot-checks (3/3 pass):**
  - **Ramp order.** `OnResetDevice` `31d3f480` builds a 20-entry wstring array in slot order at `31d3f8c1-31d3fcb3`, from `3344fc5c`
    (`CartoonNormal.bmp`, length 0x11) to `3344f900` (`CartoonDamage_Raid.bmp`, length 0x16). The order equals R1's `s_apwszCartoonTex`
    name for name, including the `CartoonFire.BMP` case.
  - **Load loop.** The loop `31d3fcce-31d3fd50` walks 0x14 slots from `this+8` and loads only a NULL slot (device manager `31cded10`), as
    R1 does.
  - **Dtor.** `31d3c480` closes 0x14 slots through `31cdd8d0`. R1's ctor and dtor loops are `0..CTT_END` inclusive (R0b note covered).
  - **`.txt` reader.** `31c864f0` accepts `CTT_NORMAL`, and `CTT_RED` (= 4), else `Invalid cartoonTexType` (`33449f5c`). March's
    `KTDGXMeshPlayer.cpp:4713` maps the same two names to the enum symbols, which are now June's values.
- **Read sites:** `m_pCartoonTexList` is read only in `BeginRender`'s 3 cartoon sites (`KTDGXRenderer.cpp:476/518/558`) and in the load loop.
  The override is range-checked, and the `CTT_END` sentinel is never used as an index: the `CTT_END !=` guard sits at all three sites.
  The `#else` paths are March's code (+ Ranox), untouched.
- **Raw-int sweep (independent):**
  - Every C++ store uses a `CTT_*` symbol. The Lua-fed casts (`X2BuffDisplayerTemplet.cpp:157/215/301`, `X2GUNPC.cpp:4350`,
    `X2Pet.cpp:1966`, `X2RidingPet.cpp:689`, the `RenderParam.cartoonTexType` binding `KTDXApp.cpp:1889`) are unchecked in March and stay
    so. They are only safe because the data uses names.
  - A scan of all 6,537 `.lua` in `kom_contents` found 11 files with a cartoon key: the BattleField buff templets / factor (+ `_INT`),
    `RenderDivisionItemTemplet`, `PET_ANCIENT_PPORU_HALLOW_SPRITE`, `Ranox_Secret_Boss_Stage` and `Enum`. Every one uses
    `CARTOON_TEX_TYPE.CTT_*` names; no file passes a bare number.
- **Play-test (log):** the 19:45 run on the fixed exe (`0f3a1445`) logged `[JUNE_ENGINE] CTT ramps loaded 20/20 (CTT_END 19, RIM_LIGHT_BLUE 15)`
  with no `ramp ... missing`. It entered 41271 and reached the stage-4 boss (NPC 2241). Checks 1-3 had no failure report.
- **Fixed (doc only):** the `Always.h` flag description still said the stand-in uses `CartoonRimLight_Blue.bmp`. It now names the
  `CTT_SKYBLUE` mapping (comment only, so no rebuild; the deployed exe is unaffected).
- **Routed to R2** (kickoff amended): R2's `SetRimLightTextureIndex` port must bypass R1's `SetRimLightTexType` mapping
  (`RIM_LIGHT_BLUE -> SKYBLUE`, 13-18 -> disabled, `KTDGXRenderer.h:238-246`) under R2's flag, and bind the real rim ramp in stage 2.

Hygiene (range `02ddf7b3^..05317e97`): `files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` /
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
0 encoding change(s)` / `RESULT: PASS`. The review's comment fix (`--worktree --files KTDXLIB/Always.h`): `1 DEFINE` / `1 deleted line(s)`
(the replaced comment line inside the flag's own define block) / `RESULT: PASS`. No later commit touches the flag.

## Phase R2 - XRenderer contract

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT`. Sub-tag
`[JUNE_ENGINE] FX`.

- June's four macros at `D3DXCreateEffect`, with `+0x58` as R0 maps it (log the chosen path once).
- The 33-entry parameter table and June's setters (R0 item 2), `RenderParam`'s rim-light members and their Lua readers.
- `COLOR_WIREFRAME` only if R0 finds a June caller.
- Works with March's `XRenderer.fx` too (the new handles are NULL: setters skip), so it can land before asset P10.
- **Gate for asset P10:** after R2 is reviewed, P10 copies June's file.
- **Play-test:** after P10, a village, a dungeon and character select (P10's own list), plus one rim-lit unit.

### Phase R2 -- Done

**2026-10-17, Sonnet 5.5.** Three flags (all in `Always.h`, `#error` guards): `JUNE_RENDER_SHADER_CONTRACT` (renderer), `JUNE_RENDER_RIM_OPTION` (option chain),
`JUNE_RENDER_WORLD_CARTOON` (world indices + unit fills). Evidence: `june2017_re/funcmap.tsv` rows `Engine R2` (BeginRender case map, DrawFrame tail,
OnResetDevice macros, `SetAnisotropy`), June `BeginRender` read from capstone (`31d3cf80` is not a Ghidra function).

- **Parameters:** `EFFECT_PARAMETER` is June's 33 (15-24 inserted, March 15-18 -> 25-28) with June's names; setters `_JuneSet*` skip a NULL handle, so March's
  `XRenderer.fx` still runs. Static check: June's `XRenderer.fx` (data041) contains all 33 names and 19 techniques.
- **Effect creation:** four `D3DXMACRO`s (DISABLE_SHADER30 only without shader 3, ENVIRONMENT_BOX, VIEW_INDEPENDENT_EDGE = "2.0", MIPMAP). Ctor reads caps:
  `+0x58` shader 3 (caps only), `+0x88` max anisotropy, `+0x82` mipmap -> stage 0 MIPFILTER. `SetAnisotropy` (`3242ed40`) and the OnResetDevice sampler block ported.
- **BeginRender:** reset rim ramp to 13 and `g_iTextureOp_1/2` = 1; black / color edge cases set screen size + view-independent edge; ramp pick (CTT_NORMAL takes the world
  index 0..3) to stage 1, world rim ramp (13..18, else 13) to stage 2 (only when the loaded fx has `g_bIsUseRimLight`); rim this draw = ramp bound && `RIM_LIGHT`
  (cartoon case never rims); grading flag; `g_mView`, `g_vEyeDir`, `g_fEye` also without fog. R1's rim mapping is bypassed, its stage-1 substitution gone.
- **DrawFrame** (skin meshes only, R0b): `_JuneSetUpperStages` = stage 1/2 texture ops, rim ramp (COLOROP ADD, ADDRESSU CLAMP pushed) or env-box rim values, grading LUT.
- **RenderParam** +2 ints (`bAlphaTestEnable` moves); five fills copy `CX2World` (user, NPC, pet, riding pet, square / agit unit). `CX2World::SetCartoonTextureIndex` new,
  `SetRimLightTextureIndex` stores on the world (Ranox renderer-wide stand-in superseded).
- **Options:** `RIM_LIGHT` (off), `USE_VIEW_INDEPENDENT_EDGE` (on), `ANISOTROPIC_FILTER_SAMPLE` (0), `USE_ENV_BOX` (off): load, save, `ApplyAllOption`; `CX2Main::OnFrameMove` copies
  `RIM_LIGHT` into the renderer first thing each frame. UI stays with Stage 9.
- **Env box (R3):** state members and the DrawFrame env branch exist, nothing writes them (reach 0).
- **Deviations from June:** the color-edge and cartoon cases index the ramp list unchecked in June; guarded here (a missing ramp is skipped). `+0x5a` / `+0x65` (untraced bytes) not ported.
  R0's "SetAnisotropy is ignored" was wrong: below 1 sample it resets the filters to LINEAR (funcmap row). `COLOR_WIREFRAME` not added.
- **June quirk kept:** JUNE_BUGS 255 (`g_bIsUseRimLight` only written by the skin-mesh draw).

**Hygiene** (`--worktree --files`): `JUNE_RENDER_SHADER_CONTRACT` RESULT PASS (2 PASS~, 0 deleted, 0 non-ASCII, 0 EOL mismatch). The two other flags: all own rows PASS / PASS~, `X2Main.cpp` OTHER
(sibling flag + a peer's U1 flag in the same file); the shared `Always.h` row is only a peer's uncommitted U1 blocks. Range check recorded in the commit message.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,804,672 B, dated 2026-10-17 21:19:47, md5 94b8520e61305072a3134e8d67237848, equal to X2/US_SERVICE/x2.exe (2026-10-17 21:19:31); built at main 129bae3e + 34 uncommitted code file(s).`
(includes a peer's uncommitted U1 files). The `[JUNE_ENGINE] FX` strings and the four option keys are in the exe.

**How to see it** (log tag `[JUNE_ENGINE] FX`, ErrorLog.txt; March `XRenderer.fx` is still live, so no rim can show yet):
1. Start the game to character select: `[JUNE_ENGINE] FX loaded: shader3 1, ... handles 19/33, rim shader 0, missing: g_mView ...` (33/33 once June's fx is in data041).
2. Ranox Secret Hell (41271), `/stage 4`: units look as after R1 (no black models); `[JUNE_ENGINE] FX world indices: cartoon 0, rim 15 -> ramp 15` proves the world -> unit chain.
3. After asset P10 copies June's fx: set `RIM_LIGHT = TRUE` in the game dir `GameOptions.lua`, repeat 2: blue rim on units (`FX rim draw` line).

**Pulled forward (user, 2026-10-17):** June's `XRenderer.fx` (+ the three already-staged June `.fx`: GCSkin, RenderState, XPostEffect) staged with `kom.py stage-june`, packed and copied: `COPY OK data041 ... 13112 bytes, 4 entries, md5 027c8a0104c6c4faa8f25a358a87d6e9` (backup `kom_backup_tools\data041.kom.20261017_213340`); `RIM_LIGHT = TRUE` added to the game dir `GameOptions.lua` (backup `GameOptions.lua.bak_pre_rimlight`). So log line 1 should now read `handles 33/33, rim shader 1`, and the rim check is live. Revert: copy the backup kom over `data041.kom`.
**Play-test log read (user run 21:35-21:40):** `FX loaded: shader3 1, mipmap cap 1, aniso max 16, handles 33/33, rim shader 1`; `FX world indices: cartoon 0, rim 15 -> ramp 15` in Ranox Secret Hell; no renderer errors. Rim draws alternate ramp 15 (body) / 13 (weapon): June's weapon copies the owner's member RenderParam, which carries no world index (JUNE_BUGS 266, renumbered from 256 by the R2 review, kept). The rim-draw log was uncapped (5,721 lines); now 12 lines. Redeploy: `DEPLOY OK: X2_offline.exe is 19,804,672 B, dated 2026-10-17 21:42:12, md5 65ae9c0e5f50e969a89499143973fd17, equal to X2/US_SERVICE/x2.exe (2026-10-17 21:42:06); built at main 3d26d68f + 1 uncommitted code file(s).` Visual verdict from the user still owed.
**Open:** R2 review (Opus); asset P10's copy of `XRenderer.fx` is done by this.

### Phase R2 -- Review

**2026-10-17, Opus 5.5. Verdict: PASS, no code change, no rebuild** (live exe stays md5 `65ae9c0e`). Commits `c46a281b`, `8330ecc7`, `80757b8f`.

- **Enum / table:** `EFFECT_PARAMETER` and `s_akEffectParameterName` = June's 33 in June's order. Each handle offset the June code uses maps to the same index
  (`+0xd8` base: `+0x108` g_bFog 12, `+0x110` g_fEye 14, `+0x114` g_mView 15, `+0x118` g_vEyeDir 16, `+0x11c/+0x120` texture ops 17/18, `+0x124..+0x138` 19-24,
  `+0x14c` grading 29, `+0x150` edge 30, `+0x154/+0x158` screen 31/32).
- **Ghidra / capstone spot-checks (3, all match):**
  1. `BeginRender` `31d3cf80`. Jump table `31d3e4dc` = the March `RENDER_TYPE` order (fades 17/18).
     - Prologue `31d3d29c`: `+0x190` = 13, both texture ops = 1.
     - Ramp pick `31d3c230` = `type ? type : (world > 3u ? 0 : world)`.
     - Black edge `31d3d4ab`: guard `type < 20` with the ramp loaded, then stage 1 / stage 2 / flag `[esp+0x2f]` / `+0x190`.
     - Color edge `31d3d6e9`: no guard, stores `+0x190`, rim = `+0x18c` alone (`31d3d5b0`). Cartoon jumps into the real-color tail `31d3d334` (no rim).
     - Grading `+0x5b` = case flag `[esp+0x13]` && `+0x80` && `+0x59` (`31d3d384`). Cases 7/8 run the edge helper `31d3d89e`. No-fog `g_fEye` (`31d3d203`).
     - `g_mView` = app `+0xdc`, `g_vEyeDir` = normalize( look-at - eye ), w 0 (`31d3daa6`).
     - Screen size = back-buffer desc `3376d9ac` Width / Height.
  2. `DrawFrame` tail `31d3eb34-31d3ee8c`:
     - Stage 1 / 2 ops.
     - Env box rim: colour (r, g, b, 1), power `+0x74`, forced `+0x7c`, wide `+0x78`.
     - No env box: rim ramp `+0x190` clamped, pushes COLOROP ADD (7) and sampler 2 ADDRESSU CLAMP (3).
     - Grading LUT in stage 3.
     - The pushes are popped by March's existing `PopStates( iStackTop )` after the frame render, as June's are.
  3. X2Lib side:
     - Option apply `32421110`: `+0x81` / `SetAnisotropy( +0x104 )` / `+0x80` = option && `+0x58`.
     - `CX2Main` frame move `31d90360`: `SetRimLight( +0x280 )` first.
     - Square / agit fill `323ed6b0`: square, else wedding, else field world, then `327fb1f0` / `327fb210` into `+0x9c` / `+0xa0`.
- **`#else` paths** are March's code untouched (Ranox stand-in and the `NULL` macro `D3DXCreateEffect` calls intact).
- **Deviations, all inert with June's fx (none fixed):**
  - `m_bRimShader` gates the BeginRender stage-2 rim bind and `m_bRimLightThisDraw`. June binds it unconditionally, but with June's file this flag is 1.
  - Black edge also rejects `type < 0`.
  - The `DrawFrame` rim ramp bind checks for `NULL`.
  - Only the color-edge / cartoon guards were in the Done note; these complete the list.
- **Cosmetic, not fixed:** the saved `GameOptions.lua` spacing differs from June's writer `3241ef40` strings (`USE_VIEW_INDEPENDENT_EDGE\t\t\t= `,
  `ANISOTROPIC_FILTER_SAMPLE  = `). Lua parses both the same. The key order and the TRUE / FALSE tokens match.
- **Hygiene** (`--range c46a281b^..80757b8f`; the range also holds U1's `915bb24`):
  - SHADER_CONTRACT `files: 1 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 31 OTHER (other flags), 2 FAIL, 0 ERROR` / `1 deleted line(s), 0 non-ASCII, 0 EOL, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: FAIL`
  - RIM_OPTION `files: 1 PASS, 1 PASS~, 0 DEFINE, 31 OTHER, 2 FAIL, 0 ERROR` / `0 deleted` / `RESULT: FAIL`
  - WORLD_CARTOON `files: 5 PASS, 2 PASS~, 0 DEFINE, 26 OTHER, 2 FAIL, 0 ERROR` / `0 deleted` / `RESULT: FAIL`
  - Both FAIL rows are explained, so neither is a finding:
    - `Always.h` (`c46a281`, `flag-off differs at stripped line 7711`): R2's three blocks carry `#ifndef X / #error / #endif X` dependency guards. The define-file rule
      accepts only comments, blanks and the `#define`. Each block is otherwise comment + `#define` (diff read).
    - `KTDXWordWrap.cpp` (`915bb24`): U1's new file, not R2's.
  - The 1 deleted line is `80757b8f`'s log-cap line, replaced inside the flag's own block.
  - OTHER = the sibling R2 flags and the U1 flags in shared files.
  - `git log -S SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT 80757b8f..HEAD`: nothing.
- **Bookkeeping (U1 review note):** R2's JUNE_BUGS row 256 (weapon takes no world rim ramp) collided with U1's 256-261; renumbered to **266** (262-265 are
  held by a W1b peer), in `JUNE_BUGS.md`, the Done note above and the `KTDGXRenderer.cpp` log-cap comment (comment only, no rebuild).
- **Rim play-test PASSED (user, 2026-10-17):** the blue body rim shows in Ranox Secret Hell `/stage 4` (log: 33/33 handles, world rim 15 -> ramp 15). R2 is closed.
- **Next:** asset P10's copy step was done here (data041 carries June's `XRenderer.fx`). P10 is now verification only (kickoff note added). R3 / R7 / R9 are unblocked.

## Phase R3 - Environment box and colour grading

**Run with:** Sonnet 5.5 high (Opus 5.5 high if R0 leaves gaps), then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_ENVBOX`.
Depends on R2. `XEnvBoxRenderer.fx` techniques `BlendLUTTechnique`, `DrawEnvBoxTechnique`; the rim-light env-box parameters in
`XRenderer.fx`. Port the owner R0 maps, its data and option. If R0 measured no reach, the row asks the user first.

### Phase R3 -- Done

**2026-10-17, Sonnet 5.5.** One flag, `SERV_IRUHADEV_JUNE_RENDER_ENVBOX` (`Always.h`, `#error` guard on `JUNE_RENDER_SHADER_CONTRACT`). User answer 2026-10-16: follow June, port the env box whole,
inactive. Evidence: `june2017_re/funcmap.tsv` rows `(R3)` (15), read from the Ghidra decompile and, for every number and sign, from the capstone disassembly.

- **`CKTDGEnvBoxMap`** (new `KTDXLIB/KTDGEnvBoxMap.h/.cpp`, a `CKTDGObject`, added to `KTDXLIB_2010.vcxproj`; header reached through `KTDGManager.h`). Port of ctor / dtor, the 4-vertex
  quad (`31d09900`), `OnResetDevice` (two 256x16 `@CKTDGEnvBoxMap@BlendedLUT0@/1@` targets, LUT textures reopened, `XEnvBoxRenderer.fx`, techniques / params), `OnLostDevice`,
  `LoadScript` (Lua global `g_pEnvBoxMap`, then reset + sort by priority / id), `SetLUT`, `AddEnvBox_LUA` (the 14 keys, defaults, matrix = YawPitchRoll * Translation), the box
  distance (`31d09130`, axis signs checked against the x87 code) and `Render` (`31d0a410`): boxes in range, LUT stacking until the weight is used up, ping-pong blend passes
  (first pass up to 4 LUTs, then 3 + the previous target), rim blend. Begin / End effect follow `31d094c0` / `31d097c0` (state manager, `Begin( 7 )`, `BeginPass( technique )`).
- **Renderer** (`KTDGXRenderer.h/.cpp`): `JuneSetEnvBoxLut` / `JuneSetEnvBoxRim` write the R2 members (`+0x59`, `+0x5c/+0x60`, `+0x68..+0x7c`) plus the new `[+0x64]` flag (June has no reader);
  `IsEnvBoxEnable()` = `[+0x80]`.
- **Lua / world:** `CKTDGEnvBoxMap` bound in `KTDXApp.cpp` (`AddEnvBox_LUA`, `SetLUT`, `31be67a0`); `CX2World::SetEnvBoxMap` (`327fd080`, bound in `X2Main.cpp`), `m_pEnvBoxMap`
  (June `+0x40`) with reset / lost / destroy hooks, and `JuneRenderEnvBox` called from `CX2Game::OnFrameRender` after the main world and each additional world with the
  camera unit's position (June `322d7260`; the world render gives it `GetMyUnit()->GetPos()`).
- **Not ported (no behaviour):** the two `31cda140` calls around the env box in `327fb5f0` only reach the map's empty draw slot; `327fc450` (an empty-name map), `31d09c70` and
  `31d0a270` (entry setters) have no caller in June; the dev command `/useEnvBox` stays with the dev console.
- **Deviations (guards, not June):** a missing `XEnvBoxRenderer.fx` (asset-port P4a ships it) logs KEM 49 and one `[JUNE_ENGINE] ENVBOX` line, then the map stays inactive (June would crash
  in `BeginEffect`); a LUT texture that fails to reopen is skipped; the blend target is cleared with `BeginRender( false )` + `Clear( 0xff000000 )` (June's 4-argument `BeginRender`).
- **June quirks kept:** JUNE_BUGS 268 (`g_pEnvBoxMap` global never cleared), 269 (blended rim power starts at 0), 270 (`BLEND_WEIGHT` forced to 0 / 1 by `BLEND_LENGTH`; missing vectors zeroed).

**Hygiene** (`--worktree --files`): RESULT PASS for all own files except `KTDGEnvBoxMap.cpp`, whose `#include "StdAfx.h"` sits outside the flag (PCH requirement, as U1's `KTDXWordWrap.cpp`);
`Always.h` OTHER = a peer's uncommitted blocks. 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 encoding changes.
Range (`--range 92f0ea8f^..92f0ea8f`): `files: 8 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 1 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: FAIL` (the one FAIL is `KTDGEnvBoxMap.cpp`'s PCH `#include "StdAfx.h"`, accepted).
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,841,536 B, dated 2026-10-17 22:39:22, md5 ea4a74b48006d7eacdc02768fa36fe9c, equal to X2/US_SERVICE/x2.exe (2026-10-17 22:39:17); built at main cc6a579d + 18 uncommitted code file(s).`
(a peer's build compiled my files; the exe holds `ENVBOX hook alive`, `AddEnvBox_LUA`, `SetEnvBoxMap`, `g_pEnvBoxMap`, `@CKTDGEnvBoxMap@BlendedLUT0@` and `XEnvBoxRenderer.fx`).

**How to see it** (log tag `[JUNE_ENGINE] ENVBOX`, ErrorLog.txt; no visual check exists, as in June nothing creates an env box): enter any dungeon or village; the first world render writes
`[JUNE_ENGINE] ENVBOX hook alive: Lua class CKTDGEnvBoxMap bound 1, AddEnvBox_LUA 1, SetLUT 1, g_pWorld:SetEnvBoxMap 1, renderer env box switch 0, map in this world 0`.
Switch 0 is the `USE_ENV_BOX = FALSE` default. **Open:** none for the user; asset-port P4a ships `XEnvBoxRenderer.fx`.

### Phase R3 -- Review

**2026-10-17, Opus 5.5. Verdict: PASS with one transcription fix** (no design question for the user). Reviewed `92f0ea8f` + `08f9741e` against
the 15 `(R3)` funcmap rows and June's code in Ghidra (read-only).

**Ghidra spot-checks (all four read line by line against the port):**
- `Render` 31d0a410: in-range test (enabled, `( 1 - d / length ) * weight`, `d >= 0 && w > 0`); LUT and rim stacking last-first while the
  remaining weight `> 0.01`; the blend test `count != 1 || w0 < 1`; chunks 4 then 3, weights `w[n - done - 1 - i]`, `g_vEtcParam = ( done, chunk, 0, 0 )`,
  target ping-pong from 0, previous target on stage 3, `SetFVF 0x102` / stride 0x14 / strip of 2, `EndRender` even when `BeginEffect` fails;
  renderer writes `+0x5c / +0x60 / +0x59` (single LUT: texture in `+0x5c`; blended: target in `+0x60`), rim `+0x68..+0x7c` in the order colour,
  power, wide, forced, `+0x64`; blended rim seeds colour / wide / forced from the first box, power from 0 (JUNE_BUGS 269). **Match.**
- `AddEnvBox_LUA` 31d0b720: KLuaManager on the table ( `32fead40( L, 1, 0 )` ), the 14 keys with their defaults and box offsets, missing
  position / rotation / scale zeroed, `LUT_NUM` range check, id = size - 1, the weight tail and the 0.001 floor (JUNE_BUGS 270); vectors are
  `lua_isuserdata` + `lua_tinker::read<D3DXVECTOR3>` (31c7e850 -> 32fe4de0 / 31c20c40), as the port. **One mismatch, fixed (below).**
- `GetBoxDistance` 31d09130: axes `( -m11, m12, m13 ) ( m21, -m22, m23 ) ( m31, m32, -m33 )` against the matrix offsets `+0x44..+0x6c`, the
  `-( 2 s + d )` fold, max-or-0, -1 for a bad or disabled box. **Match.**
- World / game hook 327fb5f0, 322d7260: `GS_PLAY`, main world then each `+0x1ac` world, unit = `GetMyUnit` (vslot 0x98 = 3231f550), renderer
  `+0x80` gate, `+0x40` map null check. June's first `31cda140` call is the line map (`+0x13c` / `+0x34`) = March's `CX2World::OnFrameRender`, the second
  is the map's empty draw (`+0x13d`). Per additional world June does line map then env box per world; the port runs every world's line map first, then
  every env box. The last world still writes the renderer last, so the final state is the same. **Match.**

**Fix (same flag):** `JuneGetBool` took a Lua boolean and compared the double with 0.0. June's getter 32fe9e40 (KNCSDK `KLuaManager::GetValue( bool )`)
takes numbers only (`lua_isnumber` 32fe4d70) and uses `ftol( v ) != 0` (32fe25f0, `CVTTSD2SI`). So `BOX_ENABLED = false` written as a Lua boolean keeps the
default `true`, and `0.5` reads as false. Now as June. No data creates a box, so nothing changes in game.

**Also checked:** `#else` paths: every call site is insert-only (no March line replaced). The renderer setters touch only R2's members plus `+0x64`. The dtor
clears `+0x59 / +0x5c / +0x60 / +0x64` and leaves the rim colours. World reset / lost call the map first (327fb6b0 / 327fb800). The `BeginRender( false )` +
`Clear( 0xff000000 )` deviation = June's `BeginRender( 1, 1, 0, 0xff000000 )` (both clear target + Z to opaque black).

**Hygiene:** `--range 92f0ea8f^..08f9741e`: `files: 8 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 1 FAIL, 0 ERROR` /
`0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` /
`RESULT: FAIL`. The one FAIL is `KTDGEnvBoxMap.cpp`'s PCH `#include "StdAfx.h"` outside the flag, accepted as U1's `KTDXWordWrap.cpp`. Fix (`--worktree
--files KTDXLIB/KTDGEnvBoxMap.cpp`): `1 PASS` / `RESULT: PASS`. Its 7 deleted lines are the old body of `JuneGetBool`, inside the flag's own new file.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,811,840 B, dated 2026-10-17 23:16:42, md5 42d6430006064e9adeef5e46c61c233b, equal to X2/US_SERVICE/x2.exe (2026-10-17 23:16:37); built at main 830818ab + 10 uncommitted code file(s).`
A peer's build compiled the fix (edit 22:56, `KTDGEnvBoxMap.obj` 23:06, `KTDXLIB.lib` 23:06, exe 23:16); mine had nothing to relink.
**User owes:** nothing (the env box stays inactive, as in June; R3's `ENVBOX hook alive` log line remains the only check). Next: `/x2-phase june-engine R4`.

## Phase R4 - Heat shimmer

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER`. Port the owner of
`31d0b200` (`HeatShimmerTechnique`) and its callers, per R0. Reach rule as R3. **R0: the owner is `DGManager +0x384`
(OnResetDevice `31ce5e90`); `31d0b200` is the env box. Reach is gated, not 0: see R0 map item 7 and the R4 kickoff.**

### Phase R4 -- Done (RE only, 2026-10-18, Opus 5.5)

No code, no flag, no build. R4's RE step changed its scope, so the port moves to four new phases (R4a-R4d below; the user picked "port all,
inactive" on 2026-10-18). Evidence: `june2017_re/r4_frame_stage_sweep.tsv` (47 rows), funcmap rows tagged `Engine R4`.

**RE item 1: the shimmer cannot run in June.** Option `+0x34` / `+0x35` (scene / effect render target) is 0 in the option-data ctor
(`3242fbd0` @3242fcb4, block at option `+4`), is never read from Lua (loader `324214a0`) or saved (`3241ef40`), is zeroed again by the low-spec
preset (@32428357), and its only setter `3241cd20` has no caller (no call, jump or pointer in the image). The first `Apply( true )`
(`3259bb20` -> `32421110` -> `31be4d10`) therefore leaves KTDXApp `+0x26c` = 0, `31be4230` never makes `EffectRenderTarget`, and
`31be3800`'s pass 1 and shimmer composite never run. Reach = 0, as the env box (open question 5).

**What else `31be3800` holds** (reachable in June, so it is ported for real):
- `ANTIALIASING_LEVEL` (option `+0x30`, June default 0 in data002 `GameOptions.lua` and data036 `DefaultGameOptions.lua`): with a level
  above 0 the scene draws into a multisampled `SceneRenderTarget` and is stretched to the back buffer (`31cdb5c0`). March has no AA option.
- The stage pass split. `CKTDXStage` gains `0xc( pass )` alpha, `0x10( pass )` front UI, `0x14` render-all (`31d28530`), `0x18`
  after-frame. The frame runs `0x8`, then `0xc( 0 )`, then `0x10( 0 )` over the stage chain, with `MAXANISOTROPY` = the renderer's value only
  around `0x8` and `D3DRS_MULTISAMPLEANTIALIAS` off around `0x10`. June overrides the three slots in all 38 render classes (RTTI, tsv). In
  March they are one `OnFrameRender` each: 21 classes override it, 7 inherit, and 12 June classes are not in March at all.
- June's render-target class carries a multisample surface plus a 4-argument `BeginRender` (`31cdb3c0` / `31cdb550` / `31cdb5c0` /
  `31cdf310`).
- `PerturbationMap.png` is opened at app init (`31be4510`, data045, asset port).

**RE item 2** (what pass 1 draws): R0b / R8 already mapped it. The new facts are the per-class slot functions (tsv `state` / `game` rows),
and that `CX2Game` is not a stage in June: each state calls its game's three slots.
**RE item 3:** `g_fPerturbationScale` and `g_color` are never set by June's C++ (the `.fx` defaults apply). `g_fTime` = app time and
`g_mViewProj` = KTDXApp `+0x15c` (`31ce5710`).

**Open, routed to the port phases:**
- KTDXApp `+0x290` (`31ce5400( 0 )` at frame end) is unnamed. R4b names it against March's `CKTDXApp` members.
- `g_pMain +0x444`, a second text manager drawn by `CX2State`'s `0x18`, is R4c's.
- `31d28530` (render-all) has no caller found. R4b ports it as the base slot.
- GFx (`31c43b20`, `31c456e0`) belongs to june-gfx.
- The 12 June-only classes take the split when their own plans port them.

**User owes:** nothing for R4. Next: `/x2-phase june-engine R4a` on Opus 5.5 / high.

## Phase R4a - Pass-1 layer and heat-shimmer object

**Run with:** Opus 5.5 high, then an Opus review, then a user play-test (nothing should change). Flags `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER`
(object) and `SERV_IRUHADEV_JUNE_RENDER_PASS1` (pass argument and post-effect lists). Port, from the tsv rows `shimmer` and `pass1`:
- the heat-shimmer object at DG `+0x384`: ctor, reset, lost, begin, end and dtor. A missing `XHeatShimmerRenderer.fx` logs and stays inactive, as R3.
- `CKTDGObject` vslots `0x10` / `0x2c` with the mesh / particle overrides.
- DG `31ce80c0`, `31ce9470`, FrontUIRender pass 1, `31cda1a0`.
- the dialog manager's pass argument, and the dialog render `31cd7810` whole (R8's amendment), plus the guide control and `RenderDragSlot`.

Every March caller passes pass 0, so nothing in game changes. Gate: build, and one `[JUNE_ENGINE] SHIMMER` line at device reset
(effect loaded or missing).

### Phase R4a -- Done

**2026-10-18, Opus 5.5 high.** Flags `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER` and `SERV_IRUHADEV_JUNE_RENDER_PASS1` (`Always.h`; PASS1 `#error`s
without `JUNE_RENDER_DG_LISTS`, `JUNE_UI_DIALOG_BANDS`, `JUNE_RENDER_DG_STATES`, `JUNE_MESHPLAYER`, `JUNE_PARTICLE`). Commit: `git log -S
SERV_IRUHADEV_JUNE_RENDER_PASS1`. 15 funcmap rows `(R4a)`; JUNE_BUGS 273-274.

**Heat shimmer** (new `KTDGHeatShimmer.h/.cpp`): ctor / dtor, reset, lost, begin, end, commit transcribed. The DG manager creates it after the
renderer, deletes it, and resets and loses it at June's position (after the renderer, before the fonts). A missing `XHeatShimmerRenderer.fx`
logs `KEM_ERROR49` plus one `[JUNE_ENGINE] SHIMMER` line and stays inactive; nothing calls BeginEffect (R4b's frame would).

**Pass 1** (every March caller passes 0):
- `CKTDGObject` gets `OnFrameRender_PostEffectDraw` (vslot 0x10, base empty `31f3f070`) and `IsPostEffect` (0x2c, base false `31e4c550`), plus
  the inline helper `OnFrameRenderPostEffect` (`31cda1a0`). Mesh and particle overrides: draw, and `alpha && bPostEffect`.
- DG: `_RenderObjectRenderHintVectorPass1` (`31ce80c0` = March's batched loop + the post-effect skip), `ObjectChainAlphaRenderPass1`
  (`31ce9470`), `FrontUIRender( pass )` pass 1 (`31ce94a0`). The two pass-0 band calls now carry the pass.
- Dialog manager: both renders take the pass and give it to every dialog render; the guide block is DG `+0x70` in pass 1; the drag slot takes
  it. June's order is (bFront, pass, min, max); March keeps (bFront, min, max, pass), so its three-argument calls are unchanged.
- `CKTDGUIDialog::OnFrameRender( bForce, pass )` (`31cd7810`) **ported whole**; the March function is the `#else`.
- `RenderDragSlot( pass )` (`31ccfee0`): June's drag test before the state push.
- Control members `SetBlockPostEffect` / `SetBlockStringPostEffect` (`+0x241` / `+0x242`) with their Lua bindings (`KTDXApp.cpp`).
- June's second dialog unit viewer `+0x164` (setter `31cd20c0`, June-only callers): NULL in March.

**Two pass-0 changes come with the whole-function port** (June wins):
1. The dialog render has no DIALOG_SHOW_TOGGLE early return. While the UI is hidden, the manager's hide list (R8) draws its message boxes, which
   now show; March showed only `DLG_ChatBox_EditBox`.
2. The `LOG_LOAD_DLG_SCRIPT` overlay (in-house, LCtrl) is gone.

The `D3DRS_MULTISAMPLEANTIALIAS = TRUE` push is D3D's default, so nothing changes visually. The hidden-control-name test (`@31cd7aa9`) is a
comment only. Its set (manager `+0xf0`) does not exist in March, and June's mode-1 preset fills only the dialog-name set.

**Routed to new R4e:** the controls' own pass argument (control vslot 8 ( pass ) in every control class, `+0x242` string gate), and
`CX2UnitViewerUI`'s vslot 0x10 / 0x2c overrides (`31f97960` / `31fae370`).

**Hygiene** (`flagcheck --worktree --files` on the 17 code files): PASS1 `files: 12 PASS, 4 OTHER, 1 FAIL`; HEAT_SHIMMER `1 PASS, 15 OTHER,
1 FAIL`; 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 encoding changes in both. The FAIL is `KTDGHeatShimmer.cpp`'s `#include "StdAfx.h"`
before the flag (the accepted PCH pattern, as R3 / U1). The OTHER rows are the sibling flag. Always.h's `-1` is a peer's uncommitted line.

`DEPLOY OK: X2_offline.exe is 19,872,256 B, dated 2026-10-18 03:33:13, md5 24cb4c156c99b7e3175e604571bf3397, equal to X2/US_SERVICE/x2.exe
(2026-10-18 03:33:08); built at main 0c6d6a15 + 18 uncommitted code file(s).` (strings `[JUNE_ENGINE] SHIMMER`, `SetBlockStringPostEffect` in the exe).

**How to see it in game:** ErrorLog.txt gets a `[JUNE_ENGINE] SHIMMER ... is not in any kom yet` line at each device reset (the first may be too early for ErrorLog; a resolution change makes another). UI, dialog
3D models (character info, cash shop), the skill-tree drag icon and the over-UI particles look as before.

**Play-test 2026-10-18: PASSED** (user + log; exe md5 3f645dcc = main 21783037, which contains f12afc69): ErrorLog has the
`[JUNE_ENGINE] SHIMMER ... is not in any kom yet` line twice (lines 625 and 628, at the two start-up device resets); 0 EXCEPTION, clean exit.
User owes nothing for R4a; next `/x2-phase june-engine R4a review` on Opus 5.5 / high.

### Phase R4a -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS. No findings, no code change, no rebuild** (the play-tested exe 3f645dcc stays deployed).

**Hygiene** (`flagcheck --range f12afc69^..f12afc69`, the phase's one commit):
- HEAT_SHIMMER `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 15 OTHER (other flags), 1 FAIL, 0 ERROR` / `RESULT: FAIL`
- PASS1 `files: 12 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 4 OTHER (other flags), 1 FAIL, 0 ERROR` / `RESULT: FAIL`
- Both: 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 encoding changes. The FAIL is `KTDGHeatShimmer.cpp`'s `#include "StdAfx.h"`
  before the flag (the accepted PCH pattern, as R3 / U1). OTHER = the `Always.h` block and the sibling flag's files.
  `KTDXLIB_2010.vcxproj` (+2, the two new files) is outside flagcheck.

**Ghidra spot-checks (read-only), all match the port:**
- `31ce5e90` shimmer reset: no-op once the effect exists; the data-pointer test; `KEM_ERROR49` (0x31) / `KEM_ERROR50` (0x32); `D3DXCreateEffect`
  flags `0x20800`; then the state manager (`0x11c`, app `+0x20c`), GetTechniqueByName, the 5 params (stride 8), SetTechnique, OnResetDevice.
  The function returns nothing that its caller reads, so the port's HRESULTs are unobservable.
- `31ce94a0` FrontUIRender: pass 1 = flagged-only gate, dialog manager `0x30( 1, 1 )`, `31ce80c0` on `+0x15c` / `+0x16c` (not
  layer-exclusive). Pass 0 = band `( 0, 3 )`, fade, `+0x14c` / `+0x15c`, band `( 4, 500 )`, `+0x16c`.
- `31cd7810` dialog render: show / self-delete returns; `+0x70` / `+0x68` block by pass; control loop NULL, `+0xc5`, pass 1 only `+0x241`;
  the end loop in pass 0 only (no NULL test, as March); the 3D gate is unit-or-particles, with `0xa1` (MSAA) = 1 pushed around it; second viewer
  `+0x164` after the first; particles go through the inlined `31cda140` / `31cda1a0`. No hide test, no LCtrl overlay.
- `31cda1a0` / `31cda140`: GetShowObject (`31fae370` through `+0xc`), Prepare != NORENDER (-1), [vslot 0x2c], `+0x173` = 1, then vslot 0x10 / 0xc.
  `31fae370` is also `CX2UnitViewerUI`'s vslot 0x2c (folded body, funcmap row `(R4a)`; routed to R4e).
- `31ce80c0`: the NULL entry logs 422 (0x1a6) and skips; a non-post-effect entry jumps to `@31ce8c08`, past the index increment (JUNE_BUGS 274);
  BeginRenderGroup / NextRenderGroup (`31d3c640` / `31d3c740`) kept; four fog sets (`31d3c030`).

**Against March:**
- Every `#else` is March's code plus the earlier June flags (DG_STATES, DIALOG_BANDS). The dialog render's `#else` keeps `DIALOG_SHOW_TOGGLE`
  (`Always.h:3270`) and `LOG_LOAD_DLG_SCRIPT` (`InHouse5.h:668`), both defined, so the Done note's two pass-0 changes are real, and both come from June.
- The overloads resolve every March call as before: the dialog manager's `( bool, int = 0 )` / `( bool, int, int, int = 0 )` and the dialog's
  `( bool = false, int = 0 )`. No March class overrides the new virtuals by name (the other `IsPostEffect` are script-token members,
  `KTDGXMeshPlayer.h:211`, `KTDGParticleSystem.h:185`).
- RenderDragSlot: only the order of the test and the push changed, and nothing is drawn outside the test.
- MSAA: only the dialog render pushes `D3DRS_MULTISAMPLEANTIALIAS`, nothing in March sets it FALSE, and the back buffer is not multisampled, so it is a no-op.

**For R4b** (its kickoff now has a note): pass 1's entry points are `FrontUIRender( 1 )`, `ObjectChainAlphaRenderPass1()` and
`GetJuneHeatShimmer()->BeginEffect / EndEffect`. JUNE_BUGS 274 makes a pass-1 batch read the wrong range (and a NULL entry before it could be
dereferenced), which is harmless only while pass 1 stays dead, as June's option does.

**User owes:** nothing. Next: `/x2-phase june-engine R4b` on Opus 5.5 / high.

## Phase R4b - Frame render, render targets, anti-aliasing option

**Run with:** Opus 5.5 high, then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_FRAME`. Depends on R4a. Port the
tsv `frame` and `option` rows:
- `31be3800` whole, the four stage loops, and the `CKTDXStage` slots `0xc` / `0x10` / `0x14` / `0x18` (base bodies as June's: empty, render-all).
- the render-target lifecycle (`31be4230`, `31be2ef0`, `31be4cf0`, `31be4d10`), `31be4510` and `31be34c0`.
- the render-target class's multisample surface and 4-argument `BeginRender`.
- the DG `BeginRender` split.
- `ANTIALIASING_LEVEL` load / save / apply, option `+0x34` / `+0x35` (default 0, the low-spec zero, the caller-less setter).
- a `JUNE_BUGS.md` row: the RT-pair setter has no caller, so the shimmer is dead.

Until R4c a March class still draws everything in `0x8`, which is March's order. Play-test: the game looks unchanged; then
`ANTIALIASING_LEVEL = 2` in the game dir's `GameOptions.lua` gives smoothed edges, with a `[JUNE_ENGINE] FRAME` line naming the level and the targets.

### Phase R4b -- Done

Built + deployed 2026-10-18 (Opus 5.5). Two flags (R4b split the kickoff's one): `SERV_IRUHADEV_JUNE_RENDER_TARGET` (the render-target
class, a new surface manager, the state manager's multisample table) and `SERV_IRUHADEV_JUNE_RENDER_FRAME` (frame, stage slots, the app's
targets, the option; needs RENDER_TARGET, RENDER_STATE_RESET, RENDER_SHADER_CONTRACT, RENDER_PASS1). 17 funcmap rows `(R4b)`, JUNE_BUGS 280-281.

**Frame** (`31be3800`): March's prologue and transforms, then June's two paths. No scene target: DG `BeginRender` (`31ce6600`), sampler-0
MAXANISOTROPY = the renderer's (`GetAnisotropy`, new getter) around the `0x8` loop, then 1, `0xc( 0 )`, `0x10( 0 )` (MULTISAMPLEANTIALIAS
pushed off), `EndRender`. Scene target: the countdown alone (`JuneBeginRenderCountdown`, `31ce63f0`), scene `BeginRender( 1, 1, 0, clear
colour )`, the same loops, `EndRenderToBackBuffer`; with an effect target, pass 1 and the shimmer composite (`BeginEffect( 0, identity )`,
`SetDeviceTexture( 0 )`, full-screen quad, `EndEffect`). Then the `0x18` loop. `CKTDXStage` gets `OnFrameRenderAlpha( pass )`,
`OnFrameRenderFrontUI( pass )`, `OnFrameRenderAll()`, `OnFrameRenderAfter()` (June bodies). Every March class still draws in `OnFrameRender`.

**Render target** (`31cdc5d0` / `31cdb950` / `31cdb310` / `31cdb3c0` / `31cdb550` / `31cdb5c0`, open `31cdf310`): + use-depth, multisample
type / quality; `BeginRender` saves the bound target / depth at every call (March: at reset), so nested targets restore to the scene target;
`EndRender` resolves the multisample surface; a FAILED device opens as NULL. The multisample and depth surfaces come from **June's
`CKTDGDeviceSurfaceManager`** (new `KTDGDeviceSurfaceManager.h/.cpp`; app `+0x208`, a stage before the device manager; R4 had not
mapped it). March callers keep the defaults (own depth, no multisample; `BeginRender( bClear )` = `( bClear, bClear, 0, 0 )`); March-only
`CopySurfaceToBackBuffer` / `CopyBackBufferToSurface` read the state manager's back buffer, the surface March saved at reset.

**App** (`31be4230` / `31be2ef0` / `31be4cf0` / `31be4d10` / `31be4510` / `31be34c0`): targets made after SetViewport in OnResetDevice,
released first in OnLostDevice; ctor switches 1 / 1 until the start-up `ApplyAllOption( true )` (JUNE_BUGS 280). `PerturbationMap.png`
opens from X2.cpp after `g_pMain` (data045: a missing-file error until asset-port P4a). Corrections to R4: `31be34c0`'s arguments are
UVs, not x / y / w / h; the RT texture's `0x54` call is AddDirtyRect, not GenerateMipSubLevels.

**Option:** `ANTIALIASING_LEVEL` (int, default / miss 0) loaded before RIM_LIGHT, saved as `\tANTIALIASING_LEVEL  = %d, \n` before
RIM_LIGHT, applied only on a forced apply (level, then the RT pair); the low-spec preset zeroes it and the RT pair, and (June's same block)
RIM_LIGHT / ANISOTROPIC_FILTER_SAMPLE / USE_ENV_BOX, the last three under `SERV_IRUHADEV_JUNE_RENDER_RIM_OPTION` (an R2 gap fixed under its
flag). Setters `SetAntiAliasingLevel` (`3241ccf0`) and the caller-less `SetSceneEffectRenderTarget` (`3241cd20`).

**Named:** KTDXApp `+0x290` = `CKTDXCursor` (RTTI, vtable `3344c980`), a June-only software cursor -> new **R4f**. June's per-site target
arguments (env box use-depth 0, LastKillShot, minimap, MoveData) and the last-kill shot while the frame is in the scene target -> new **R4g**.
GFx (`31c43b20`, `31c456e0`, the option page `32ee07d3` that calls `3241ccf0`) -> june-gfx.

**Deviations (not June):** one `[JUNE_ENGINE] FRAME` line per target (re)build; the RT reset guards a NULL surface manager and logs
KEM_ERROR80 when no depth surface came back (June tests a void call's leftover EAX); March's clear-screen timer kept (June: tick count).

**Hygiene:** `flagcheck --worktree --files <18 files>`: FRAME `files: 7 PASS, 0 PASS~, 0 DEFINE, 10 OTHER, 1 FAIL`; TARGET `files: 7 PASS,
1 PASS~, 0 DEFINE, 2 OTHER, 1 FAIL`; both `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 encoding change(s)`.
OTHER = the other R4b flag's hunks (and RIM_OPTION in X2GameOption.cpp); the one FAIL is `KTDGDeviceSurfaceManager.cpp`'s `#include
"StdAfx.h"` PCH line, as R4a's KTDGHeatShimmer.cpp. PASS~ = one blank line before the new RT functions.

**Deploy:** DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to
X2/US_SERVICE/x2.exe (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s). The exe holds `[JUNE_ENGINE] FRAME
ANTIALIASING_LEVEL`, `@CKTDXAPP@SceneRenderTarget@`, `PerturbationMap.png` and the save string.

**In game:** (1) default `ANTIALIASING_LEVEL = 0`: the game looks as before; ErrorLog shows a `[JUNE_ENGINE] FRAME` line at start-up with
both switches 1 ("scene target made, effect target made"), then one with switches 0 and no targets. (2) `ANTIALIASING_LEVEL = 2` in the game
dir's `GameOptions.lua`: smoother edges, the FRAME line says `-> 2` and "scene target made". (3) On exit the file keeps the key.

**Play-test PASSED 2026-10-18** (user: the game looks fine at the default and at level 2; log, exe 0818bfeb, run 07:43: FRAME lines 1024x768 then 1366 / 1365x768 with both start-up switches 1 ( scene + effect made ), then the forced apply `2 -> 2` ( NONMASKABLE levels 4, quality 1 ) and switches 0 / 0: multisampled scene target, no effect target; no KEM_ERROR76-80; 0 EXCEPTION; GameOptions.lua rewritten at exit with `ANTIALIASING_LEVEL  = 2,`).

**User owes:** nothing. Next: `/x2-phase june-engine R4b review` on Opus 5.5 / high.

### Phase R4b -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS. No findings, no code change, no rebuild** (the play-tested exe 0818bfeb stays deployed).

**Hygiene** (`flagcheck --range 84ce49f2^..84ce49f2`, the phase's one code commit; `git log -S<FLAG> 84ce49f2..HEAD` finds no later commit):
- RENDER_TARGET `files: 7 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 10 OTHER (other flags), 1 FAIL, 0 ERROR` / `RESULT: FAIL`
- RENDER_FRAME `files: 7 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 11 OTHER (other flags), 1 FAIL, 0 ERROR` / `RESULT: FAIL`
- FRAME `--with JUNE_RENDER_TARGET` (both stripped): `files: 9 PASS, 0 PASS~, 1 DEFINE, 8 OTHER, 1 FAIL, 0 ERROR`; `KTDXApp.cpp` / `.h`, OTHER
  in the single-flag runs because they carry both flags, PASS with both stripped, so their `#else` is March's code byte for byte.
- All runs: 0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 encoding changes. The one FAIL is `KTDGDeviceSurfaceManager.cpp`'s
  `#include "StdAfx.h"` PCH line (R4a precedent). The other OTHER rows are the other R4b flag; PASS~ is the blank line before the new RT
  functions. `KTDXLIB_2010.vcxproj` lists the new `.cpp` unconditionally (flag off: an empty unit, as R4a's shimmer file).

**Ghidra spot-checks (read only), all match:**
- `31be3800`: render count + three transforms, then the no-target path (`31ce6600`, sampler 0 / 10 = renderer `+0x84` around `31be2cd0`,
  then 1, `31be2d30( 0 )`, `31be2d90( 0 )`, `31ce6140`) and the target path (`31ce63f0`, `BeginRender( 1, 1, 0, ARGB of DG +0xc0..+0xcc )`
  = `D3DXCOLOR`'s DWORD cast, same loops, `31cdb5c0`, or `31cdb550` / `( 1, 1, 1, 0xff000000 )` / passes 1 / `31cdb550` / shimmer with
  technique 0 (an `int&`, the low dword of the 8-byte local) + identity / slot `0x14( 0 )` / `31be34c0( 0, 0, 1, 1 )` / `31ce5d00`). GFx,
  `0x18` loop and `31ce5400( 0 )` as the note routes them.
- `31cdb3c0` / `31cdb550` / `31cdb5c0` (disassembled): save target + depth per call, multisample surface or texture surface, depth unless keep,
  stencil on formats 0x49 / 0x4b / 0x4f / 0x53; EndRender `StretchRect( ms, tex surface, NONE )` + texture slot 0x54 = AddDirtyRect;
  to-back-buffer filter 2 when `+0x98 / +0x9c` differ from `3376d9c4 / c8`.
- `31cdb950`: format `+0x54` or the saved target's, never written back; surface manager only with `+0x5c`, app and `+0x208`; depth
  `31d00d20` / `31d00f20` by the multisample surface; error codes 0x4c (target), 0x4f (surface level), 0x50 (depth) = KEM_ERROR76 / 79 / 80, as ported.
- `31d00b30` / `31d00d20` / `31d00ff0` (disassembled): the cache hit jumps out (`@31d00bf3 -> 31d00c45`); Purge re-AddRefs a held surface
  and unlinks a sole-owner one, as ported.
- `31be4230` (disassembled @31be43b9): the effect target opens non-multisampled when `+0x26d` is 0 and multisampled otherwise, an if / else
  (the decompile shows a fall-through). `31be6510`: state restore, SetViewport, `31be4230`, SetProjection, the stage resets, as placed.
- `31be34c0` (FPU stack traced): v0 `( -0.5, -0.5, u0, v0 )`, v1 `( W-0.5, -0.5, u1, v0 )`, v2 `( -0.5, H-0.5, u0, v1 )`, v3 `( W-0.5,
  H-0.5, u1, v1 )`; ZENABLE / ZWRITEENABLE 0; DVB `( 3, TRIANGLESTRIP, 2, v, 1 )` = `DVB_TYPE_XYZRHW_DIFFUSE_TEX1`.
- `32421110` @32421263-324212a3: level then the pair, each only on a forced apply. `324214a0` @32428335-3242836a: `<= 60` MB video or
  `< 300` MB physical, then `+0x30`, word `+0x34`, `+0x100` / `+0x104` / `+0x108` zeroed, as ported.

**March reach:** no March class defines the four new `CKTDXStage` names, so the loops call only the empty base slots and every March
class still draws in `OnFrameRender`. March `BeginRender( bClear )` = `( bClear, bClear, false, 0 )`; `m_D3DFormat` has no other reader.

**Notes (no change):** the Done note's "June tests a void call's leftover EAX" is loose: `31d00b30` / `31d00d20` return an HRESULT
(`0x8876086b` without a device). The port's void + NULL test is behaviour-equal on every path read (the RT desc is always
`D3DUSAGE_RENDERTARGET`). `X2GameOption.h` declares the two setters inside `#ifdef VERTICAL_SYNC_OPTION` (defined, `Always.h:2486`);
undefining that flag would fail the compile, not silently.

**User owes:** nothing. Next: `/x2-phase june-engine R4c` on Opus 5.5 / high (or R4f / R4g, both now unblocked).

## Phase R4c - State render split

**Run with:** Opus 5.5 high (transcribe each June slot), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT`.
Depends on R4b. Split `CX2State` and every March state class in the tsv `state` rows into June's `0x8` / `0xc( pass )` / `0x10( pass )`
(and `CX2State`'s `0x18`).
- Each slot is transcribed from June: dialog handles, pass arguments, gates.
- Earlier ports inside the March bodies move with their code.
- In-house extras (`FIRST_SCREEN_TEST`) stay guarded in `0x10`.
- `CX2TFieldGame`, `CX2SquareGame` and `CX2WeddingGame` (plain classes) split with their states.
- Until R4d each state calls its game's March `OnFrameRender` from `0x8`.

Play-test: login, server select, village, dungeon and PvP rooms, a dungeon, the square, and loading screens look and behave as before.

### Phase R4c -- Done

Built + deployed 2026-10-18 (Opus 5.5). Flag `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT` (needs `JUNE_RENDER_FRAME`, `JUNE_UI_DIALOG_BANDS`).
**Split in two** (the kickoff's size rule): R4c did `CX2State` and the thunk / simple states; the game and room states are new **R4c2**. June's
game-state front-UI slots are rewritten bodies with dozens of new callees (e.g. DungeonGame `324cff10`, Field `3250aa10`, rooms `324e4c40` /
`325374d0`), each to map. 8 funcmap rows `(R4c)`.

**`CX2State`**: `0x8` = particle culling on, sort, non-alpha, off (`31ef75b0`); `0xc( pass )` = culling on, pass 1 -> `ObjectChainAlphaRenderPass1`
else `ObjectChainAlphaRender`, off (`31ef75f0`); `0x10( pass )` = `FrontUIRender( pass )`, text manager `+0x440`, game edit when set, minimap
`UpdateEventNotice` on pass 0 only, cursor when set (`31ef7640`); `0x18` = DG manager and dialog manager NULL checks, then in the flagged-only
mode (`+0x110`, never set in March) dialog-manager vslot `0x30( 1, 0 )` (`31ef76b0`).
**`g_pMain +0x444` resolved**: a second `CX2TextManager` (ctor `32744c10`, made right after `+0x440` @31e4abb5, freed @31d93b3f, frame-moved
by `CX2State::OnFrameMove` @31f0d945). Its only writer is `CX2NoticeManager` (`+0x448`, RTTI on vtable `334ab704`, ctor `325ad9a0( 150 )`,
`325acf90` adds the text, `325ad410` checks it, called from `CX2StateMenu::OnFrameMove` @324727ad). In June, notices are drawn after the frame,
above GFx; March puts them in the main text manager. The member, its draw in `0x18` and the notice manager's switch -> **R4h**.
**Callee pass arguments**: June hands the pass to the text manager (`32744860`), the game edit (`32746640`: no draw when pass == 1) and the
cursor (`32868a00`); March's take none. The slots call March's on every pass June does. Pass 1 never runs (R4 RE item 1) -> **R4h**.

| Class | June slots | Port |
|---|---|---|
| `CX2StateMenu`, `TrainingSchool`, `CommonBG`, `CreateUnit`, `Join` | thunks / inherit | nothing to add (March's OnFrameRender thunk = June `0x8`) |
| `CX2StateStartUp` | inherits (no override) | `0x8` override dropped; `FIRST_SCREEN_TEST` (on here) background in `0x10` after the base, pass 0 |
| `CX2StateLogin` | thunks `32595060..80` | `FIRST_SCREEN_TEST` background moved from `0x8` to `0x10` after the base, pass 0 |
| `CX2StateServerSelect` | `3255d4a0 / c0 / e0` | `0x8` (March's body) and `0xc` skip while `m_bStateChangingToUnitCreate`; `0x10` does **not** (March skipped all) |
| `CX2StatePvPLobby` | `32543210 / 30 / 60` | tutorial: no menu slots; `0x10` draws only the loading dialog `( false, pass )` |
| `CX2StateLoading` | `3258b800 / 30 / 70` | no culling; `0x10` NULL-checks the cursor; no `LOADING_ANIMATION_TEST` (off here) |
| `CX2StateDungeonResult` (+ Training), `CX2StatePVPResult` | `3250e4e0..` / `325169a0..` | each slot gated by `m_bInitUIFail` (`+0x310` / `+0x32c`) |
| DungeonRoom, PVPRoom, DungeonGame (+ PVE / Training / Arcade), BattleField, PVPGame, Field, Square, Wedding | R4c2 | **interim**: empty `0xc` / `0x10` overrides keep March's single `OnFrameRender`; DungeonRoom / PVPRoom call the menu's three slots in place of the old base call |

Visible change from June: server select keeps its UI while it changes to unit creation; the minimap event notice draws only on pass 0
(same in practice). `0x18` runs for every state (also the R4c2 ones, as June) and draws nothing while the flagged mode is unset.

Hygiene (`flagcheck --worktree --files` on the 26 code files + Always.h): `files: 25 PASS, 0 PASS~, 0 DEFINE, 2 OTHER (other flags), 0 FAIL,
0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched` / `RESULT: PASS`. The 2 OTHER rows are peers' uncommitted
hunks in the same files (Always.h tail, `X2StateLoading.cpp` `JUNE_SKILL_NOTE`), not part of this commit. After the commit,
`--range f754f8c1^..f754f8c1`: `files: 26 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s)` / `RESULT: PASS`.
Deploy: `DEPLOY OK: X2_offline.exe is 19,972,608 B, dated 2026-10-18 09:06:45, md5 d5457f55f7daf176675ebdc4ce262055, equal to
X2/US_SERVICE/x2.exe (2026-10-18 09:06:40); built at main a2b20c4c + 77 uncommitted code file(s)` (the peers' in-flight R4e / S-phase files
are in this exe too; two earlier runs stopped on the build lock and on a peer's `JUNE_RENDER_DRAW_PASS` `#error`, since fixed by them).

**How to see it in game**: everything looks as before (the gate). Server select -> character creation: the server-select UI stays drawn while
the state changes. No `[JUNE_ENGINE] SPLIT` log line was added (the slots run every frame; a broken split shows as missing or doubled UI).

**Play-test PASSED 2026-10-18** (user: every gate screen as before; log: 0 EXCEPTION, UNHANDLED only `E_DISABLE_HB_CHECK_REQ`). The user saw
the quest-end story scene stay invisible in the village (cursor = `XCS_EVENT_SCENE`): that is JUNE_BUGS 253 (R8's dialog bands skip layers
501-505; kept as June 2026-10-17, owned by june-gfx), in `CX2StateField`, which R4c did not split.

**User owes:** nothing. Next: `/x2-phase june-engine R4c review` on Opus 5.5 / high; R4c2 after the review.

### Phase R4c -- Review

Reviewed 2026-10-18 (Opus 5.5 high) against `f754f8c1` and June. **One finding, fixed under the same flag.**

**Slots match** (Ghidra / `disasm_range.py`): `CX2State` `31ef75b0` (culling 33773660 on, sort 31ceb5f0, non-alpha 31ce93a0, off), `31ef75f0`
(`pass == 1` -> 31ce9470 else 31ce9440), `31ef7640` (31ce94a0( pass ), +0x440 text manager no NULL check, +0x43c game edit when set, event
notice on pass 0 with `g_pInstanceData` and `+8` checks, cursor `+0x64` when set), `31ef76b0` (+0x1fc / +0x38c NULL checks, `+0x110 == 1` ->
vslot 0x30( 1, 0 ), then +0x444 -> R4h); Loading `3258b800 / 30 / 70` (no culling, cursor NULL check); ServerSelect `3255d4a0 / c0` skip on
`+0x385`, `e0` plain thunk; DungeonResult `3250e4e0 / 500 / 520` gate on `+0x310`; PvPLobby `32543260` (+0x16a / +0x16c, `+0x4a8` handle ->
31cd7810( 0, pass ), else menu 32459ea0). Every `#else` is March's body. Results and ServerSelect bodies were base-call plus gate in March, so no
draw order moved. StartUp / Login `FIRST_SCREEN_TEST` backgrounds still cover the whole frame as March's did.

**Finding: March's forced single frames drew only `0x8`.** R4 recorded `CKTDXStage` vslot 0x14 (`31d28530`, `0x8` + `0xc( 0 )` + `0x10( 0 )`) as
having no caller. A BeginScene scan finds 34: June draws every `BeginScene` / render / `EndScene` / `Present` frame through it, from
`CX2Main::CreateStateChangeDLG` (@31d807c0, `m_pNowState` vtable +0x14, clear 0x32323232), 24 frames in `CX2StateLoading` (3258c6d2..32592efc)
and 9 game-state frames. March's sites still called `OnFrameRender()`, which after R4c is the sort + non-alpha only. So the state-change message
box, the first-load gauge (`FIRST_GAME_LOADING_THREAD_TEST` is off, so these frames are live) and the start-up splash were not drawn in those frames.
**Fix:** `OnFrameRenderAll()` under `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT`, March's call in `#else`: `X2Main.cpp` (2, one per
`DXUT_KTDXLIB_SAFE_ON_RESET` branch), `X2StateLoading.cpp` (25), `X2StateStartUp.cpp` (2, the in-house `FIRST_SCREEN_TEST` splash). A forced
frame on an unsplit R4c2 state is unchanged (`0x8` still draws everything, its `0xc` / `0x10` are empty). DungeonGame (June `324d9f80`), PVPGame
(`325297f0`) and the unmapped `324b06e0` / `3249fd50` go to R4c2 (kickoff updated). Funcmap row `31d28530 callers (R4c review)`.

Hygiene: `flagcheck --range f754f8c1^..HEAD`: `files: 26 PASS, 0 PASS~, 2 DEFINE, 54 OTHER, 0 FAIL, 0 ERROR` / `1 deleted line(s)` /
`RESULT: PASS`. The OTHER rows are peers' commits in the range (R4e, S3b, S3c). The deleted line is S3c's `JUNE_SKILL_TYPE` comment in `Always.h`
(8a842484). The fix, `--worktree --files` on the 3 files: `files: 3 PASS, 0 FAIL` / `0 deleted line(s)` / `RESULT: PASS`.
Build: **deploy pending.** My three files compiled clean, but X2Lib stopped at the peer's uncommitted S5 edits (`JUNE_SKILL_SLOT_10`:
`X2GUUser.cpp` / `X2UserSkillTree.cpp` `USI_SPECIAL_ATTACK_5`, `X2OfflineSkill.cpp` `m_wstrSkillSlotBEndDate`). Following the shared-build
rule I did not re-run. The first exe built after S5 compiles carries the fix.

**How to see it in game:** select a character -> the loading screen's gauge advances while loading; changing state (village -> dungeon room
and similar) shows the state-change box; the start-up splash shows during the boot load.

**User owes:** a short play-test on the first exe built after S5 compiles. Next: `/x2-phase june-engine R4c2` on Opus 5.5 / high.

## Phase R4c2 - Game and room state split

**Run with:** Opus 5.5 high (transcribe from Ghidra), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT`
(same flag; R4c2 replaces R4c's interim overrides). Added by R4c (2026-10-18). Depends on R4c. The tsv `state` rows R4c left:
`CX2StateDungeonRoom` (`324e2660 / 324e2690 / 324e4c40`), `CX2StatePVPRoom` (`32531450 / 32531480 / 325374d0`), `CX2StateDungeonGame` (+ PVE /
Training; `324cc530 / 324cc5c0 / 324cff10`), `CX2StateBattleField` (`324b92e0 / 324b9340 / 324bb230`), `CX2StatePVPGame` (`32521370 / 32521410 /
325214c0`), `CX2StateField` (`32507650 / 32507790 / 3250aa10`), `CX2StateSquareGame` (`324c7070 / 324c70d0 / 324c7150`), `CX2StateWeddingGame`
(`324b60c0 / 324b60e0 / 324b63c0`), and the plain classes `CX2TFieldGame` (`323a9f20`, `323b7e10`, vtable `+0xc / +0x10 / +0x14`),
`CX2SquareGame` (`323a4b10 / 323a4b50 / 323a5040`), `CX2WeddingGame` (`323a0a80 / 323a0ac0 / 323a13d0`).
- Transcribe each slot whole; map every callee of the `0x10` slots (many take the pass). Earlier ports in the March bodies move with their
  code (`JUNE_TUTORIAL_MODE`, `JUNE_MOVIE_DESCRIPTION` in DungeonGame). Remove R4c's interim overrides and the two interim base calls.
- Until R4d each game state calls its game's March `OnFrameRender` where June calls the game's `0x8`, and nothing for the game's `0xc` / `0x10`.

Play-test: village, field, dungeon and PvP rooms, a dungeon (also the tutorial), a PvP match, a battlefield, the square, results look as before.

## Phase R4d - Game render split

**Run with:** Opus 5.5 high, then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_GAME_SPLIT`. Depends on R4c2. Split
`CX2Game` (`322d7260` / `322cc8f0` / `3230aa60`) and the five March game classes in the tsv `game` rows, transcribing each `0x10` UI slot from
June. The `CX2Game::OnFrameRender` June blocks (env box, Elysion script B, dramatic message, skip-start) move into the matching slot. Play-test:
dungeon, PvP and battlefield HUDs as before.

## Phase R4e - UI control pass argument and the other post-effect overrides

**Run with:** Opus 5.5 high (map, then port), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_UI_CONTROL_PASS` (proposed).
Added by R4a (2026-10-18). Depends on R4a; it can run before or after R4b-R4d (pass 1 has no caller until R4b).
- June's control render is `vslot 8 ( pass )` in every control class (button `31ca37f0`, radio `31cab2b0`, slot `31cb1ef0`, static `31cbec10`,
  IME edit `31cb9ff0`, ...). The pass reaches the texture / vertex draws, and strings draw in a pass other than 0 only with `+0x242`
  (`SetBlockStringPostEffect`, R4a's member). Map each class, then port whole, plus the two call sites R4a marked (dialog render
  `@31cd7b6c / @31cd7b9a`, guide control `@31d05695`).
- The other June overrides of `CKTDGObject` vslots `0x10` / `0x2c` (`CX2UnitViewerUI` `31f97960` / `31fae370`, and any other vtable that
  overrides them).

Every March caller passes 0, so nothing in game changes. Gate: build + deploy; UI unchanged.

### Phase R4e -- Done

**2026-10-18, Opus 5.5 high.** Split into two flags (`Always.h`): `SERV_IRUHADEV_JUNE_RENDER_DRAW_PASS` (the 2D primitives, `#error`s
without `JUNE_RENDER_DG_STATES`) and `SERV_IRUHADEV_JUNE_UI_CONTROL_PASS` (the controls, `#error`s without `DRAW_PASS` and `JUNE_RENDER_PASS1`).
Commit: `git log -S SERV_IRUHADEV_JUNE_UI_CONTROL_PASS`. 15 funcmap rows `(R4e)`; JUNE_BUGS 287.

**Map.** June has 16 classes deriving `CKTDGUIControl` (RTTI sweep), each with vslot 8 = `OnFrameRender( pass )`, `ret 4`. U1 had three
labels rotated: `31ca37f0` is the list box, `31cb1ef0` the button and `31cd16a0` the slot (vtables `3344b20c` / `3344b794` / `3344c454`).
U1's port is unaffected, because each class has its own `RewrapPending` call. The pass goes further than the controls:

- **Draw primitives** (`DRAW_PASS`): the three `CKTDXDeviceTexture::Draw` overloads and the render-target `Draw` take a last bool and push
  DG `+0x40` (R8's additive block) instead of `+0x38`.
- **Font** (`DRAW_PASS`): the font is **`CKTDGDeviceFont`** in June too (vtable `3344de70`, RTTI `3361154c`; June has no FreeType class, and
  `Always_US.h:17` `#undef`s `USE_FREE_TYPE`). Its `Flush` pushes a second state table in pass 1 (`337295d8`, font `+0x1000bc`). The table
  has alpha test on (`NOTEQUAL 0`), SRC and DEST blend both ZERO and colour = diffuse, a mask that writes zero under every glyph. Buffering,
  OutTextXY and OutTextMultiline, plus the `CUKFont` wrappers OutTextXY, OutProjectionText, OutTextMultiline and Draw2DText, hand the pass down.
- **Controls** (`UI_CONTROL_PASS`), in pass 1:
  - Button, radio, slot and static: strings draw only with `+0x242` (`SetBlockStringPostEffect`), and the font and edges get the pass.
    Button `DrawEdge`, radio `DrawCheckedEdge` and static `DrawEdge` are ported whole: March's body with the pass on every edge `Draw`.
  - The static's pictures draw once with no SRC/DEST blend push, and its movies are skipped.
  - The slot skips its additional pictures.
  - Edit, IME edit and list box draw their background only. The scroll bar's held-arrow repeat runs in pass 0 only.
  - The control list passes the pass to both bars, gated on the X bar's `+0x241` for both (JUNE_BUGS 287).
  - Check box, combo, navigation and slider ignore the pass and get the new signature only. So do March's `CKTDGUIListControl` (not in
    June) and `CX2TalkBoxManagerUI` (its imp's pass is R4i), so they stay overrides.
- **Call sites:** the dialog render's control loop (June `@31cd7b6c` / `@31cd7b9a`) and the guide control (`@31d05695`) pass the pass.
  Children inside controls get 0, as June's do (`@31ca45f2`, `@31cc5a3a`).

**Routed:** the `CKTDGObject` vslot `0x10` / `0x2c` overrides go to new **R4i**. The sweep of every `CKTDGObject` RTTI class finds three
beyond R4a's mesh and particle: `CX2UnitViewerUI`, `CX2Equip` and `CX2TalkBoxManager`. R4i also takes the talk-box imp's
`OnFrameRender( pass )` (`32e930c0`, party leader `32f21da0`). The size rule decided that split: the viewer's post-effect draw alone is
about 1,300 decompiled lines. The static's June-only list `+0x268` (untextured quads drawn in every pass; no March member, no filler
known) goes to new **R4j**.

**Hygiene** (`flagcheck --worktree --files` on the 42 code files): UI_CONTROL_PASS `files: 34 PASS, 0 PASS~, 0 DEFINE, 8 OTHER, 0 FAIL,
0 ERROR` / `0 deleted, 0 non-ASCII, 0 EOL-mismatched`; DRAW_PASS `files: 8 PASS, ..., 34 OTHER, 0 FAIL` / `0 deleted, 0 non-ASCII,
3 EOL-mismatched` (the device-font ctor insert in a mixed-EOL file); `RESULT: PASS` for both. Each flag's OTHER rows are the other flag's files.
Range check after commit: `flagcheck --range f96952cd^..f96952cd`: UI_CONTROL_PASS `34 PASS, 9 OTHER, 0 FAIL, 0 ERROR` / `0 deleted, 0 non-ASCII, 0 EOL`, DRAW_PASS `8 PASS, 35 OTHER, 0 FAIL, 0 ERROR` / `0 deleted, 0 non-ASCII, 3 EOL`; `RESULT: PASS` for both.

`DEPLOY OK: X2_offline.exe is 19,973,120 B, dated 2026-10-18 09:18:28, md5 d4eef83688b181e6dbdb7d59656cc4a8, equal to X2/US_SERVICE/x2.exe
(2026-10-18 09:18:23); built at main eb5280c7 + 52 uncommitted code file(s).` (the 10 non-R4e files are peers' S-phase work.)

**How to see it in game:** nothing changes. Every March caller passes 0, and render pass 1 has no caller yet. The UI (buttons, tabs, radio
buttons, slots, edit boxes, list and combo boxes, scroll bars, text) should look and behave as before.

**Play-test 2026-10-18: PASSED** (log; exe md5 d4eef836 = this phase's deploy): ErrorLog 09:50-10:01 has the usual `[JUNE_ENGINE]` DG-chain and
movie-subtitle lines and nothing new, 0 EXCEPTION, clean exit (census written). User owes nothing for R4e; next
`/x2-phase june-engine R4e review` on Opus 5.5 / high.

### Phase R4e -- Review

**2026-10-18, Opus 5.5 high: PASS, no findings, no code change, no rebuild** (the deployed exe stays `d4eef836`).

- **Font** (`CKTDGDeviceFont`): June `Flush` `31d1bae0` is March's plus one branch, pass `== 1` selects font `+0x1000bc` over `+0x1000b4` (`@31d1bb2d`).
  The page bounds and the per-page outline / plain draw match March. June's table `337295d8` was read from the file: 15 `KState`s
  (RS 27/1, 14/0, 15/1, 25/6, 24/0, 19/1, 20/1; TSS 1/3, 3/0, 4/4, 5/2, 6/0; SAMP 6/2, 5/2, 7/2). It matches `s_akStatesJunePostEffect`
  entry for entry. Its only reference is the ctor store `@31d1c72f`.
- **Live branches:** under `USE_DT_VCENTER`, each live `Buffering` in OutTextXY gets the pass, and so does each `Flush` in OutTextXY and
  OutTextMultiline. The page-full `Flush` inside `Buffering` gets it too.
- **Texture:** `31d48f90` picks DG `+0x40` / `+0x44` when its last byte is 1, and `GetRSICKTDXDeviceTextureAdditive` is that `+0x40` member.
- **Controls, re-read in Ghidra:**

  | Control | June | What was checked |
  |---|---|---|
  | Button | `31cb1ef0` | Edge before and after with the pass; picture without it; string gate `pass == 0 \|\| +0x242`; font gets the pass |
  | Static | `31cbec10` | Edge gets the pass. Pass 1: `RenderVertex( .., 1 )` only when `+0x50 > 0`, with no SRC/DEST push; otherwise 0x13 / 0x14 are pushed. `+0x242` gate `@31cbeed3`; all 4 font calls get the pass; movies skipped when pass is 1. The `+0x268` loop is R4j's |
  | List box | `31ca37f0` | Visibility, then the font NULL path, then the bounding box, then the pass-1 return; children get 0 (`@31ca45f2`) |
  | Control list | `31cca350` | Both bars test the X bar's `+0x241` (JUNE_BUGS 287 holds); nothing else in the body, matching March's live code |
  | Dialog | `31cd7810` `@31cd7b6c` / `@31cd7b9a` | Both push the same stack argument `[ESP+0x1c8]`, the pass |
  | Guide | `31d053f0` `@31d05695` | Pushes `[EBP+0xc]` |

  Radio, slot, edit, IME and scroll bar were checked from the diff only, not re-opened in Ghidra. Each change sits where its funcmap
  row cites it: the slot's pass-0 gate on its additional pictures, the edit and IME pass-1 returns after the background, and the scroll
  bar's pass-0 repeat.
- **Edge helpers:** a normalised diff of each new copy against its `#else` copy covered button `DrawEdge`, radio `DrawCheckedEdge` and
  static `DrawEdge`. Only the Korean side comments differ, plus `tempColor` becoming `tempColor, 0.f, pass` (0.f is the rotation default),
  so March's geometry is kept.
- **Overrides:** every class deriving `CKTDGUIControl` in `KTDXLIB` / `X2Lib` declares `OnFrameRender( int iRenderPass_ = 0 )` under the
  flag, with the old one in `#else`. That covers the base, the 14 engine controls and `CX2TalkBoxManagerUI`; `CKTDGUIItem` inherits the static's. The only parameterless declaration
  is the dead first `CKTDGUIScrollBar` inside the `/* */` at `KTDGUIScrollBar.h:3`. The unflagged `->OnFrameRender()` calls on child
  controls and scroll bars in `KTDGUIComboBox.cpp` / `KTDGUIListBox.cpp` compile to pass 0, which is June's (`@31ca45f2`, `@31cc5a3a`).
- **Hygiene** (`flagcheck --range f96952cd^..f96952cd`):
  - UI_CONTROL_PASS: `files: 34 PASS, 0 PASS~, 0 DEFINE, 9 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched,
    0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
  - DRAW_PASS: `files: 8 PASS, 0 PASS~, 0 DEFINE, 35 OTHER, 0 FAIL, 0 ERROR` / `0 deleted, 0 non-ASCII, 3 EOL-mismatched, 0 ..., 0 ...` / `RESULT: PASS`.
  - The OTHER rows are `Always.h` plus the other flag's files. The 3 EOL lines are the device-font ctor insert in a mixed-EOL file, the same lines the Done note named.

User owes nothing. R4i (needs this review) and R4j are ready.

## Phase R4f - Software cursor (CKTDXCursor)

**Run with:** Opus 5.5 high (map, then port), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_CURSOR` (proposed).
Added by R4b (2026-10-18). Depends on R4b. KTDXApp `+0x290` is an embedded `CKTDXCursor` (RTTI, vtable `3344c980`, ctor `31ce50e0`, dtor
`31ce5090`), drawn last in every frame by `31ce5400( 0 )` (`31be3800 @31be3c7c`); March has no such class. Map every slot and every writer
of its fields (texture, position, offset, size, shown / hidden), decide whether June's flow ever shows it, then port the class, the member,
the frame-end call and the writers. Gate: build + deploy; the cursor as June's flow says (or unchanged when it stays hidden).

## Phase R4g - Render-target call sites take June's arguments

**Run with:** Opus 5.5 high (map, then port), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_TARGET_CALLERS`
(proposed). Added by R4b (2026-10-18). Depends on R4b; not alongside R4d / R4e on the same files. R4b kept every March render-target site on
March's defaults (own depth, no multisample, `BeginRender( bClear )`); June passes per-site arguments to `OpenRenderTargetTexture`
(`31cdf310`: env box `31d0b200`, LastKillShot `32306600`, minimap `32e583d0`, MoveData `31c96daa`, `31c40220`) and to the four-argument
`BeginRender` (`31cdb3c0` callers). Also: how June takes the last-kill shot when the frame is in the scene target (`ANTIALIASING_LEVEL > 0`).
Gate: minimap, last-kill shot (also at level 2), colour grading and UI move data look as before.

## Phase R4h - Notice text manager and the slot callees' pass argument

**Run with:** Opus 5.5 high (map, then port), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_NOTICE_TEXT`
(proposed; a second flag for the pass arguments if they split cleanly). Added by R4c (2026-10-18). Depends on R4c; not alongside another
`X2Main.*` / `X2NoticeManager.*` editor.
- **Notice text manager.** June's `g_pMain +0x444` is a second `CX2TextManager` (ctor `32744c10` @31e4abb5, freed @31d93b3f, frame-moved
  @31f0d945 beside `+0x440`) that only `CX2NoticeManager` (`+0x448`, ctor `325ad9a0`, `325acf90` / `325ad410`) writes; `CX2State`'s `0x18`
  draws it after the frame (`31ef76b0`, `32744860( 0 )`). Port the member, its lifetime, its frame-move and draw, and the notice manager's
  functions that differ from March (whole; also its frame-move moving to `CX2StateMenu::OnFrameMove` @324727ad if that is June's only one).
- **Pass arguments** of the slot callees: `CX2TextManager` render `32744860( pass )` (font draw `31c3ae80`'s last argument), game edit
  `32746640( pass )` (no draw when pass == 1), `CX2Cursor` render `32868a00( pass )` (with R4f if the cursor port owns it), and the ones R4c2
  finds. Every caller passes June's pass; pass 1 stays unreachable, so the gate is "unchanged".

Gate: GM / item notices still appear (now drawn after the frame); UI unchanged.

## Phase R4i - Post-effect object overrides and the talk-box pass

**Run with:** Opus 5.5 high (map, then port), then an Opus review, then a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_POST_EFFECT_OBJECTS`
(proposed). Added by R4e (2026-10-18). Depends on R4e (`DRAW_PASS`, `UI_CONTROL_PASS`).
- `CKTDGObject` vslot `0x10` (`OnFrameRender_PostEffectDraw`) / `0x2c` (`IsPostEffect`) overrides, from R4e's RTTI sweep:
  - `CX2UnitViewerUI` (vtable `33471184`): `31f97960` (about 1,300 decompiled lines; compare with March `OnFrameRender_Draw`,
    X2UnitViewerUI.cpp:1438) / `31fae370`. `31fae370` is `GetShowObject`'s body, folded, i.e. `IsPostEffect() = GetShowObject()`.
  - `CX2Equip` (vtable `334c9b50`): `3295caa0` / `3295bd50`. `3295bd50` is true when `+0x8ec` is set and any effect sequence in
    `+0x8f0..+0x8f4` (found through `31cefdd0( handle, 1 )`) is a post-effect sequence.
  - `CX2TalkBoxManager` (vtable `335358e0`): `32e903e0` (the imp's vslot `0xc` with the pass; `Draw` `32e903b0` pushes 0) / `31fae370`.
- `CX2TalkBoxManagerImp::OnFrameRender( pass )` (imp vslot `0xc`: base `32e930c0`, party leader `32f21da0`) with its draw callees.
  Then `CX2TalkBoxManagerUI` (`32eec4a0`) hands the pass to it; R4e gave the UI class the signature only.

Every caller of pass 1 is still unreached (R4b's frame has no post-effect pass), so the gate is "unchanged": the dialog 3D models, the
equipped-item effects and the talk boxes look as before.

## Phase R4j - Static colour-quad list (June-only)

**Run with:** Opus 5.5 high (RE first: find the filler), then the port if March can reach it. Flag `SERV_IRUHADEV_JUNE_UI_STATIC_QUADS`
(proposed). Added by R4e (2026-10-18). Depends on R4e.
- June's `CKTDGUIStatic` has a vector at `+0x268` that March lacks (March's next member is the movie list, June `+0x288`). Its render
  `31cbec10` (`@31cbee6a-31cbeed1`) walks it in every pass. Each entry gets `31c9b660` (it places the entry's 0x1c-byte vertices at
  `( src + dialog pos ) * resolution`), then `31c954a0`, which draws type 2 with 4 vertices through `31c94e70` (untextured
  `DrawPrimitiveUP`).
- RE: who fills `+0x268` (a Lua binding or June-only UI code), what the entry type is, and whether any June script or dialog uses it. If
  nothing March loads can create one, record that and close the phase; otherwise port the member, its filler and the draw.

## Phase R5 - CUSTOM_RENDER_STATE

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_CUSTOM_RENDER_STATE`.

- June's table in `Enum.lua` (fixes the Add / Ara truncation, checklist `:152`); keep March's `X2GUAdd.h` enum separate.
- The engine reader(s) and render behaviour per R0 item 6, for units, NPC and riding pet (`X2RidingPet.cpp:1404`).
- Class-specific use (Add, Ara, Ain, Lu/Ciel, Rose) goes to the checklist; on March data only `Ara_Martial_Artist` and the Add
  script reach it once those scripts are adopted.
- **Play-test:** whatever March-data path R0 finds; if none, a dev-console Lua call on the player's unit.

## Phase R6 - SKIN_MESH_RENDER_TYPE

**Run with:** Opus 5.5 high (R0b: map the whole draw, then port), then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKIN_MESH_RENDER_TYPE`. June's cases 1, 2, 4-7
(R0 item 5) in `KTDGXSkinAnim`'s typed pass, next to the existing UV_ANIM case. **R0b:** June rebuilt the whole
`OnFrameRender_Draw` (`31d30380` + `31d2f000`); port all of it (kickoff amendments).

## Phase R7 - Hookable textures

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_HOOKABLE_TEXTURE`.
Added by R0b.
- **The system:** `_SetNowTexture` (`31d3e530`) takes the skin-mesh hook id `+0xa40` (passed by `DrawFrame` `@31d3ea6d`) and swaps a texture through the
  device-manager table filled by `AddHookableTextureBaseData` / `AddHookableTextureChangeData`.
- **The data:** June-only `HookableTextureData.lua` (data036), with per-job faces (ids 44, 45, 69, ...) and cash faces (41806, ...).
- **Map first:** the table, the id writers and the loader.

## Phase R8 - DG manager render lists

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_DG_LISTS`.
Added by R0b. It covers the pass-0 half of June's `CKTDGManager`; pass 1 stays with R4.
- 14 render-state blocks.
- Three over-UI chains, with the 500-layer split.
- `FrontUIRender`'s two dialog bands.
- The main hint loop without group batching.
- The not-cull order, the alpha tie-break `+0x14c` and the third frustum argument.
- `CKTDGStateManager` Restore / Invalidate moved out of the DG reset.

### Phase R8 -- Done

**2026-10-17, Opus 5.5.** All five items mapped and ported for render pass 0. The flag was split into seven, all defined in
`Always.h` with `#error` dependency guards. Evidence is 17 funcmap rows tagged `Engine R8`; the state tables were decoded from the binary.

| Flag | What landed (June) |
|---|---|
| `JUNE_RENDER_DG_STATES` | 14 blocks (`31ce6310`). The dialog block (+0x68) = March's 10 sampler pushes + `akRSICKTDGUIDialog`, same order, so the dialog render and `RenderDragSlot` push one block. Two additive blocks (+0x40 / +0x70) and their getters; only pass 1 reads them. |
| `JUNE_UI_DIALOG_BANDS` | Dialog manager render as `31d053f0` / `31d05700`. Adds a layer-band overload, the flagged-only mode (+0x110 / dialog +0x1e0, default off) and the dialog block around the guide-control loop. The `LOG_LOAD_DLG_SCRIPT` overlay is gone. |
| `JUNE_RENDER_DG_LISTS` | A third over-UI chain, with alpha over-UI split at layer 500. `FrontUIRender` pass 0 draws: dialogs 0..3, fade, over-UI, alpha < 500, dialogs 4..500, alpha >= 500. One throttled `[JUNE_ENGINE] DG chains` line. |
| `JUNE_RENDER_FRUSTUM_TEST` | `CKTDGObject::IsInFrustum( eye, frustum, fAddRadius = 0 )` as `31c829e0`. It keeps the culling plane at +0x18c and no longer tests not-cull; the sort tests not-cull first. `CX2WorldObjectMesh`'s override takes the same signature (R13). |
| `JUNE_RENDER_ALPHA_SORT_KEY` | Alpha-sort tie-break on +0x14c, with the particle `CreateSequence` writing its handle there (`31cf92a1`). |
| `JUNE_RENDER_HINT_NO_GROUP` | Batches draw once, each object through the per-object path (no `BeginRenderGroup`). Batched meshes now push their own RenderParam states, as single ones did. |
| `JUNE_RENDER_STATE_RESET` | State-manager Restore / Invalidate move into `CKTDXApp::OnResetDevice` / `OnLostDevice` (`31be656e` / `31be6728`), with the DXUT adapter and device type. |

**Corrections to R0b:** the additive blocks blend `ZERO` / `ZERO` (value 1), not `ONE` (JUNE_BUGS 251). The +0x68 / +0x70 choice and the
texture draws' new last bool are the **render pass**, so their users are R4's pass 1. `31cd7810` is `CKTDGUIDialog::OnFrameRender( bForce, pass )`,
not a DG pass render.

**Hygiene** (`flagcheck --worktree --files <14 files>`): RESULT PASS for all seven flags. Every other row is OTHER, from the sibling R8 flags.
PASS~ = one blank line each in `KTDXApp.cpp` / `KTDGUIDialogManager.cpp`. 0 deleted lines, 0 non-ASCII, 0 EOL mismatches.
Range check after commit (`--range b1f197ca^..b1f197ca`): RESULT PASS for all seven flags, with 0 FAIL / ERROR.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,764,736 B, dated 2026-10-17 20:08:37, md5 9ff6fb3c54e07a6426464c9ff5b00e92` (built at main 82adc267 + this phase's 14 files).

**Routed:**
- **R4 (pass 1):** the pass argument through the dialog manager / dialog / controls / textures, the additive blocks' users, the rest of
  `31cd7810` (pass-1 control filter +0x241, +0xc5 skip; not compared line by line), and `31be4230` / `31be2ef0` (render-target create / release
  in the app reset / lost). R4 kickoff amended.
- **june-gfx:** the GFx reset / lost in the app handlers (`31c43e60` / `31c43440`).
- **New R13:** `CX2WorldObjectMesh` June render. There is no frustum override (vtable `334bc484`), and Prepare `327f9020` is rebuilt.
- **Not ported, no owner:** the June-only writers of +0x110 / +0x1e0, and the non-zero third-argument callers `31fcf0f0` / `31fcf380`
  (funcmap rows list the addresses). Hide modes 2-5 stay Effect C3p-2's routed rows.

**Quirks kept** (JUNE_BUGS 251-253): the additive blocks blend `ZERO` / `ZERO`. The two band calls each draw the hidden-UI list, the guide
descriptions and the dragged slot, so those draw twice per frame. Not-cull objects skip the show test and the distance update in the sort. Front dialogs above layer 500 are never drawn (the Lua event scene's object layers at 502, as in June; 253).

**How to see it in game** (any character, e.g. Freils, Chung Tactical Trooper, uid 18):
1. Enter any dungeon. During the stage-start fade, pop-ups and option windows (layers 4..500) sit above the black, and the in-game HUD fades in under it.
2. ErrorLog.txt gets `[JUNE_ENGINE] DG chains inner .. over .. alpha<500 .. alpha>=500 .. culled ..` lines as the chains change.
3. Regressions to look for: no missing or black UI, the skill-tree drag icon still shows, and Alt+Tab or a resolution change keeps textures (state reset).
**Play-test 2026-10-17: PASSED.** The user reports the UI looks and behaves normally. ErrorLog.txt has 27 `[JUNE_ENGINE] DG chains` lines
(village `over 7`; dungeon `over 9`, alpha < 500 at 0-4, culled 56-100), and the alpha >= 500 chain stayed at 0, so it was not exercised.
Review kickoff: `kickoffs/june-engine/R8-review.md`.

### Phase R8 -- Review

**2026-10-17, Opus 5.5. PASS, with one transcription fix** (`JUNE_UI_DIALOG_BANDS`, mode-1 hide list) and JUNE_BUGS 253 corrected to
visible. Everything below was re-read from `unpacked_x2.exe` (Ghidra, plus a raw read of the file).
1. **State tables.** `33728270` (12), `337284e0` (20) and `33728620` (23) were re-decoded, and the counts in `31ce6310` match (`+0x44` 0xc,
   `+0x6c` 0x14, `+0x74` 0x17). Every entry equals `akRSICKTDXDeviceTextureAdd` / `akRSICKTDGUIDialogJune` / `akRSICKTDGUIDialogAdd`, in
   order. SRC / DEST = 1 (ZERO) is confirmed (JUNE_BUGS 251).
2. **Dialog manager.** `31d053f0` / `31d05700` = `_OnFrameRenderJune`:
   - Hide path: `+0xbc && !+0x34`, then the `+0xd4` list. June's loop has no show test because `31cd7810` tests `+0x6e` itself, so March's
     `GetShow()` is equivalent.
   - Flagged-only branch: `+0x1e0` and show, no band.
   - Band test: in the normal branch only, inclusive at both ends (`@31d058bf`).
   - Guide controls: drawn under the `+0x68` block. `31c7b670` / `31c19b90` are the KD3D scope plus the push.
   - Drag slot: drawn only when `bFront`.
   **Fix:** June's mode-1 preset (`32597410 @3259b25e-3259b318`) is March's four names followed by `DLG_Exit_Party_MsgBox` (`334a9fb4`).
   The port had only the four, so the fifth was added under the same flag. Nothing changes in game: only June's `DLG_UI_ExitPartyMsgBox.lua`
   names that dialog, and March never opens it.
3. **Chain sort and FrontUIRender.**
   - Split: `cmp 0x1f4; jl` (`@31ceb767`), so layer < 500 goes to `+0x15c` and 500 and above to `+0x16c` (= `< 500`).
   - Order: not-cull `+0x17c` is tested before the vslot 0x34 call. InstantShow `+0x173` is cleared and set where March does it.
   - Sorts: `+0x14c`, `+0x15c`, `+0x16c`, `+0x13c`.
   - FrontUIRender pass 0: the order and the `+0x110` test inside each KD3D scope = the port.
   - Comparator `31cebc00`: the left key < right key (signed) only on equal layer and equal distance (`FUCOMPP`, `TEST AH,0x44`) = the port.
     The particle key store `@31cf92a1` = the handle.
4. **Frustum.** `31c829e0` = the port:
   - radius = max scale x `+0x15c` (a radius <= 0 returns true);
   - the third float is added only when it is not 0;
   - `+0x18c` = -1, then the first plane with dot < -r;
   - culled flag `+0x174`.
   The wrapper `31ce6c90` has no not-cull test (= the port). The sort is June's only reader of `+0x17c`.
5. **Hint loop.** Both batch sites of `31ce6eb0` were checked:
   - with a stateID set, the inline pop to the stack top comes first;
   - then one pass over `iStart..iIndex`: state switch, `31d3c030`, vslot 0xc;
   - no Begin / NextRenderGroup;
   - the stateID is cleared after the batch (`@31ce7662`; site 2 runs into the function end).
6. **Layers above 500.** A `.text` scan for dialog-manager vslot 0x30 / 0x38 calls finds:
   - `31ce935c` / `31ce93de` (bFront 0);
   - FrontUIRender's three;
   - `31ef76b0`, which calls vslot 0x30 (1, 0) only while `+0x110` == 1.
   So nothing else draws a front dialog of layer 501+. June still builds the Lua scene's object dialogs as front, layer 502
   (`@325b3981-325b3986`). But June's Lua `PlayEventScene` (`327ccad0`) calls `g_pData+0x274` -> `3226b130`, the GFx scene player
   (`Tutorial_Back.swf`, GFx `+0x1c8`), so June never shows a Lua scene's objects. **This client still plays quest start / end scenes through
   the Lua `CX2EventScene`** (`X2UIQuestNew.cpp:938` / `:1524` / `:1660` / `:1901`). Its skin meshes and textures are drawn only by their
   502 dialog (`X2EventScene.cpp:870` / `:1099` / `:1130`), so since R8 those characters are invisible. **User 2026-10-17: keep June as it
   is, no FIX flag,** until june-gfx plays scenes through GFx. JUNE_BUGS 253 is now marked visible, and the G0 kickoff carries the gap.
7. **State reset.** `31be6510` calls `RestoreDeviceObjects( settings[0] AdapterOrdinal, settings[1] DeviceType )` (`@31be656e`) after
   BeginFullSync and before SetViewport. `31be66b0` calls `InvalidateDeviceObjects` (`@31be6728`) after the stages, EndFullSync and GFx.
   Both = the port. March has no other caller of either.

**Hygiene** (`--range b1f197ca^..b1f197ca`): RESULT PASS for all seven flags, with 0 FAIL / ERROR. Every OTHER row is a sibling R8 flag
in the same file. DG_LISTS and HINT_NO_GROUP have no file of their own: their lines are all in `KTDGManager.cpp`'s shared row.
`git log -S` finds no later touch. The review fix: `flagcheck JUNE_UI_DIALOG_BANDS --worktree --files KTDGUIDialogManager.cpp` PASS
(+6 -1; the -1 is a comment inside the flag).
**Play-test log** (exe = R8's deploy, md5 9ff6fb3c): 27 `[JUNE_ENGINE] DG chains` lines; alpha >= 500 never filled; no event scene played.
**Build:** not deployed. Both `build.py --deploy` runs (20:38, 20:41) failed in `KTDXApp.cpp` on a peer's unfinished U1 edits
(`SetGroupOffset`, combo-box setters), the block R11 also hit. The fix is committed and ships with the next successful build from `main`.
It needs no play-test of its own, since March never opens the fifth dialog.
**Deployed** 2026-10-17 21:19:47 by a peer's `build.py --deploy` of `main` 129bae3e (contains 3d4e2d43): `X2_offline.exe` md5 94b8520e;
the string `DLG_Exit_Party_MsgBox` is in the exe.
**Routed:** the hide-mode lists go to june-gfx G0 (kickoff amended): the `+0xd0` map, the presets in `32597410`, and the readers OnFrameMove
`31d06100` and `31d05b80`, which also read `+0xd4`. Also the Lua-scene layer gap above. Funcmap rows tagged `Engine R8 review`.

## Phase R9 - Render small deltas

**Run with:** Sonnet 5.5 high, then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_SMALL_DELTAS` (split if
independent). Added by R0b. The items are all mapped in the sweep:
- particle `RT_ADD_*` names;
- effect alpha in the RHW and skin-mesh particle draws;
- sequence `+0x1ac`;
- outline clamp `31cfe050` (map particle system `+0xdc` first);
- effect state manager lost device (`31d184f0`);
- skin-mesh XET name rule and close-by-pointer.

## Phase R10 - Skin-anim playback sweep

**Run with:** Opus 5.5 high. No code. Added by R0b: the non-render `CKTDGXSkinAnim` functions (`OnFrameMove` `31d2bd80`, `ChangeAnim`
with a June 4th play type, transition bytes `+0x854` / `+0x855`, `CleanAnimStructure` with `std::map +0x294`, the dtor), function by
function as R0b. Funcmap rows `Engine R10`. **Done gate:** every function has a verdict row.

### Phase R10 -- Done

**2026-10-17, Opus 5.5.** No code, no flag, no build. Evidence: `june2017_re/r10_skinanim_sweep.tsv` (39 rows, owner and June
addresses per row) and 38 funcmap rows `Engine R10`. Verdicts: 26 differ, 10 same, 1 mapped earlier, 1 dead in March
(`CopyModelFrameStructure`), 1 tool-only (`GetAnimNameList`). March's flags were settled with a `/Zs` probe on KTDXLIB `US_SERVICE`:
all 16 skin-anim macros are ON (incl. `JIGGLEBONE_TEST`, `ANIMATIONCONTROLLER_FIX`, `FIX_SKIN_ANIM_ONE_WAIT_BUG`, `SEPARATION_MOTION`,
`MODIFY_AFTER_IMAGE`, `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`).

**Same:** `Reset`, `BuildCloneFrameStructure` / `List`, `LinkCollisionDataFrame`, `PreLoad`, `Stop`, both `UpdateAnimationFrame`
recursions (`31d36bd0`, jiggle `31d361c0`, constants checked), `CopyFrame` (`3322a390`, after-image), `IsInModelXSkinMesh` (dead in both).

**June layout (for R11 / R12):** `+0x190` anim mesh, `+0x194` anim XET; `+0x198` / `+0x1a8` / `+0x1b8` / `+0x1c8` / `+0x1d8` model mesh /
ani XET / ani data / multi-tex / tex-change lists (AddModel push order); `+0x1f0` / `+0x1fc` collision / attack; `+0x254` anim-set vector;
`+0x264` / `+0x274` / `+0x284` name / index / clone-frame maps; `+0x294` backup map; `+0x2a4` state; `+0x2a8` play type; `+0x84c` jiggle unit
anim; `+0x850` apply move bone; `+0x852` process flag; `+0x854` / `+0x855` transition bytes; `+0x864` mixed motion names.

**Differences -> R11 (new, playback and anim-set bookkeeping):**
- Play type 3 (no Lua name, no literal caller) and the transition bytes in `OnFrameMove` and the three `ChangeAnim`s.
- `ChangeAnim( int )` bounds check; `ChangeAnim( name )` finds without inserting.
- Lua `SetTransition` (world object) writes June's anim `+0x854`. The local binding of 2026-09-23 (`SERV_IRUHADEV_RANOX_WORLD_METHODS`,
  `X2Main.cpp:3831`) points it at March's own object member instead; `Ranox_AshesVillage_Stage2.lua` calls it.
- The `+0x294` backup map (`ResetAnimSet`) and `CleanAnimStructure`'s anim-set release check (KEM 436).
- `MixAnim`'s mixed-name list `+0x864`. Nothing clears it, so a re-mix after a second `SetAnimXSkinMesh` is skipped (June quirk, keep).
- `SetAnimXSkinMesh` logs KEM 434 for a mesh without an animation controller; `BuildCloneFrame` gains a NULL guard.
- `31d29950` refreshes bones without advancing time. Its chain is unported: `CEffect` main-effect scale `31f6fca0` -> `CXMeshInstance` `31c83520`.

**Differences -> R12 (new, device ownership):**
- June's skin anim holds a device reference on everything it stores:
  - `AddModelXSkinMesh` AddRefs, unless `bCloseDeviceAtDestructor`; `m_vecAddedSkinMesh` is gone. `AddAnimXSkinMesh` AddRefs too.
  - `Remove` / `Clear` / `CleanAnimStructure` close.
  - `CleanAnimStructure( mesh, xet )` swaps the anim mesh and anim XET; the dtor is `CleanAnimStructure( NULL, NULL )`. This resolves R0b's "unsure" dtor.
- `SetAnimXET` and the four `Replace*XET` take references. Two of them are no-ops in March (they assign a local copy); June fixes both.
- `ReplaceMultiTexXET( int )` returns void, so `X2Eqip.cpp:2784` / `2822` must stop closing the old XET.
- `ClearModelXSkinMeshList` also deletes the collision data.
- `AddModelXSkinMesh` gains `( file, line )` and logs KEM 433 when the frame counts differ.
- `31d2f250` adds a mesh's collision data only; `CX2GUUser`'s motion init calls it twice, and March has no such call. R12's map decides how it meets March's lists.

**KEM ids:** June adds 433 (AddModel), 434 (`SetAnimXSkinMesh`), 435 (the draw `@31d304ab`, R6) and 436 (`CleanAnimStructure`). March's
enum ends at 432 (`KTDXErrorDefine.h`); R11 adds the four as one block.
**Not swept (outside this kickoff):** the header inlines `SetPlaySpeed`, `SetAnimationTime`, the `Predict_*` / `IntervalTimer` timers.
**Open for the user:** none. Every difference has an owner, and both new phases follow June.

## Phase R11 - Skin-anim playback and anim-set bookkeeping

**Run with:** Sonnet 5.5 high, then an Opus review and a user play-test. Added by R10; every item is mapped in
`june2017_re/r10_skinanim_sweep.tsv` (owner R11). Flags: `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK` (items 1-2),
`SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS` (items 3-5), `SERV_IRUHADEV_JUNE_RENDER_SKIN_BONE_REFRESH` (item 6). Not alongside R6 or R12.
1. **Playback:** the transition bytes `+0x854` (ctor 1) and `+0x855`, play type 3 in `OnFrameMove`, and the three `ChangeAnim`s whole.
2. **`SetTransition`:** bind the Lua name to June's setter `327f69d0` (anim `+0x854`). First read June's world-object frame move to see
   what its own `ChangeAnim` calls pass, and port both halves.
3. **Anim sets:** the `+0x294` backup map, the `CleanAnimStructure` release check and its two log lines, the KEM 433-436 enum block.
4. **Mixed motions:** the `MixAnim` name list `+0x864`, kept never-cleared, with a `JUNE_BUGS.md` row.
5. **Small:** the `SetAnimXSkinMesh` KEM 434 log and the `BuildCloneFrame` NULL guard.
6. **Bone refresh:** `31d29950`, `CXMeshInstance` `31c83520` and the call in the `CEffect` main-effect scale (`31f6fca0` `@31f6fd70`).

### Phase R11 -- Done

**2026-10-17, Sonnet 5.5. Code committed (`b0412386`). None of my own `build.py --deploy` runs succeeded, but the live exe already contains the code (see Build).** Flags `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK`
(items 1-2), `..._SKIN_ANIM_SETS` (3-5), `..._SKIN_BONE_REFRESH` (6), all in `Always.h` after the R8 block. Evidence: `june2017_re/r10_skinanim_sweep.tsv` rows owner R11.

- **Playback** (`KTDGXSkinAnim.h/.cpp`): members `m_bJuneLoopTransition` (+0x854) and `m_bJuneLastTransition` (+0x855), both true in the ctor (dword `0x01010000`
  at +0x852). `OnFrameMove` (`31d2bd80`, read raw: no Ghidra function): XAP_LOOP restarts with `ChangeAnim( idx, +0x854, 0.0f )`; play type 3 (C++ name only,
  `XAP_JUNE_LOOP_LAST_TRANSITION`, nothing added to `Enum.lua`) restarts the same way with +0x855. `ChangeAnim( int )`: signed `index < size` check, else false
  (`31d2b5e0`). `ChangeAnim( name )`: `m_mAnimSetList.find`, no insert (`31d33fd0` -> `31d39100`). The +0x855 write is in `ChangeAnim( pAS )` (`31d29340 @31d296fc`)
  and, as June does, in the two callers. In the name version the March anchor line carries a trailing tab, so that write sits before the `ChangeAnim( pAS )` call; the
  value is the same and `ChangeAnim( pAS )` writes it last.
- **`SetTransition`:** June's world-object frame move (`327f887b..327f8bdc`) passes the object's own `+0x21c` to all six `ChangeAnim( name )` calls, as March's already
  does; only the Lua setter differs. `CX2WorldObjectSkinMesh::SetTransition_LUA` now writes the skin anim's +0x854 (`327f69d0`; no anim: nothing). The binding
  (`X2Main.cpp:3831`) is unchanged, so the March `m_bTransition` write is gone with the flag on. Its only Lua user: `Ranox_AshesVillage_Stage2.lua:50`, the
  **cave** mesh `ASHS_VILLAGE_S2_CAVE.X` (the cart at :63 does not call it).
- **Anim sets** (`31d31c10`, `31d2f9a0`): `m_mapJuneAnimSetBackup` (+0x294) cleared first in `ResetAnimSet`, filled `(i, pAS)` after the name/index maps, cleared with
  the three name maps. `CleanAnimStructure`: even pointer >= 0xff Release; even and < 0xff non-zero -> KEM 436 `...Release Failed crash1 index = N Ani = <mesh> addr = <hex>`,
  slot NULL, Release of the backup copy; odd -> KEM 436 `...Release Failed index = N Ani = <mesh>`, no Release. Device closes and the two args stay with R12.
  `KEM_ERROR433..436` as one block after 432 (flag SKIN_ANIM_SETS; 433 / 435 are used by R12 / R6).
- **Mix / small:** `MixAnim` (`31d34140`): `std::find` of the motion mesh's device id in `m_vecJuneMixedMotion` (+0x864) first -> true; `push_back` after the new
  controller's sets are registered; never cleared -> **JUNE_BUGS row 254**. `SetAnimXSkinMesh`: `ErrorLog( KEM_ERROR434 )` for a mesh without a controller
  (`31d32783`). `BuildCloneFrame`: NULL clone/source returns (`31d319a1-31d319ab`).
- **Bone refresh:** `CKTDGXSkinAnim::JuneRefreshBones` = `31d29950` (false without anim mesh or clone root; matrix or DXMatrix branch; the same inlined
  `UpdateAnimationFrame` walk as `UpdateAnimationFrameStructure`, no AdvanceTime). `CXMeshInstance::JuneRefreshScale` = `31c83520`: scale = `m_vScale * m_vManualSize
  * ScaleByUnit * m_vSize` per axis in June's multiplication order, applied to the skin anim's matrix **only when it has no manager object** (June skips the Scale
  otherwise), then the refresh. Called from `CEffect::SetMainEffectScale` right after `SetScale` (`31f6fca0 @31f6fd70`); the March site that ports it.
- **Logs** (`ErrorLog.txt`, each site capped at 3 lines): `[JUNE_ENGINE] SKANIM loop restart without transition`, `play type 3 restart, transition N`,
  `ResetAnimSet backup map N sets`, `MixAnim remembers <name> ( N mixed )`, `MixAnim skipped, already mixed: <name>`, `bone refresh`, plus uncapped `clean crash1` /
  `clean odd pointer` and the `BuildCloneFrame NULL guard` / `SetAnimXSkinMesh mesh without an animation controller` lines.
- Also `MODS.md` (3 rows), `JUNE_BUGS.md` row 254.

Hygiene (`flagcheck --worktree --files <my 8 files>`, per flag): PLAYBACK `files: 1 PASS, 7 OTHER`, SETS `1 PASS, 7 OTHER`, BONE_REFRESH `3 PASS, 5 OTHER`;
every flag `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
The OTHER rows are my own other two flags in the same files, and `Always.h` (which also holds a peer's uncommitted U1 defines; only my three blocks are committed).

**Build: live exe contains R11, built by another session.** My own `build.py --deploy` runs failed in KTDXLIB, only in a peer's unfinished U1 edits (`KTDXApp.cpp`, untracked `KTDXWordWrap.cpp`). The W1 session's deploy (`5ab2a08b`, `DEPLOY OK` md5 `8ffc762a`, `X2_offline.exe` 19,778,560 B, 2026-10-17 20:35:54, equal to `X2/US_SERVICE/x2.exe`) built the shared working tree after my edits were applied, so it carries them. Checked in the deployed exe (checked 2026-10-17): all five `[JUNE_ENGINE] SKANIM` literals (`bone refresh`, `MixAnim remembers`, `loop restart without transition`, `play type 3`, `ResetAnimSet backup`) and the UTF-16 `Release Failed crash1` text are present, so KTDXLIB and X2Lib compiled and linked with the R11 code. That exe was built before the R11 commit and from a tree that also held other sessions' uncommitted changes, so it is not a clean build of `b0412386`; a later `build.py --deploy` of a compiling tree replaces it with one. The play-test below can run on the current exe.

**Play-test hand-off (after the deploy; user, Freils uid 18, Chung Tactical Trooper; tag `[JUNE_ENGINE] SKANIM`):**
1. *Ranox Ashes Village stage 2 cave loop.* Enter Ranox Ashes Village Expert (33282), console `/stage 2`, `/invin 0`; stay about a minute. Look for: the cave
   scenery keeps animating without a visible pop; `[JUNE_ENGINE] SKANIM loop restart without transition` in ErrorLog.txt (fires when the cave's "Normal" anim wraps; if
   the anim is long it may need waiting). If the cave jumps or freezes at the wrap: report, the setter is `SetTransition_LUA`.
2. *Main-effect scale change.* Any effect whose damage-effect scale changes while it plays (a giant-unit skill or a `SetScale`d mesh effect); look for
   `[JUNE_ENGINE] SKANIM bone refresh` and an effect mesh that resizes at once without one frame at the old size. If no skill is known, skip and say so.
3. *MixAnim / class change.* Start a dungeon, then change class with the dev console (auth level 3, END key) or open the character viewer; look for
   `SKANIM MixAnim remembers <name>` and, on a repeat, `MixAnim skipped, already mixed` (June behaviour, JUNE_BUGS 254). Also grep ErrorLog.txt for `Release Failed`: none expected.

**Play-test (user, 2026-10-17): all 3 checks passed; logs read.** Run on the W1-deployed exe (md5 `8ffc762a`, run 21:06-21:10), ErrorLog.txt: `SKANIM ResetAnimSet backup map 3 / 16 / 31 sets`; `MixAnim remembers MOTION_LIRE_LOBBY.X ( 1 mixed )`, `MOTION_LIRE_FIELD.X`, `MOTION_LIRE_EMOTION.X ( 2 mixed )`; `SKANIM bone refresh` x3 (21:06:55-56, the Add dash-jump XX bullet); `SKANIM loop restart without transition` x3 (21:07:13, :25, :36, Ashes Village stage 2 cave). No `Release Failed` / KEM 434 / 436 / `BuildCloneFrame NULL guard` lines. Not exercised, as expected: play type 3 (no caller in June's data), `MixAnim skipped, already mixed`.

### Phase R11 -- Review

**2026-10-17, Opus 5.5. PASS: no behaviour finding. One comment citation fixed (no rebuild: comment only, binary unchanged).** Read against June's own code
(Ghidra decompile + disasm of every function below; `disasm_range.py` for `31d2bd80`'s switch, `327f69d0` and `327f8840..327f8be0`).

| Item | June | Verdict |
|---|---|---|
| OnFrameMove play types | jump table `31d2ed54`: 0 `31d2d566`, 1 `31d2d5b3`, 2 `31d2d5f2`, 3 `31d2d631`. LOOP: `+0x2ac` = 1, `ChangeAnim( +0x224, +0x854, 0.0f )`, `+++0x248`; type 3 the same with `+0x855`; ONE / ONE_WAIT set state 0 / 2 and count 1 | match |
| Ctor | `31d322d7` `mov [esi+0x852], 0x01010000` (+0x854 = +0x855 = 1); `31d322e1` `+0x864` init | match |
| `ChangeAnim( int )` `31d2b5e0` | `cmp ebx, size; jl` (signed) else false; NULL set -> false; `+0x224` = index; `ChangeAnim( pAS )`; `+0x855` = transition @`31d2b6b1` | match; the comment cited `31d33fd0 @31d340d4` (the name version), now `31d2b5e0 @31d2b6b1` |
| `ChangeAnim( name )` `31d33fd0` | `31d39100` find; 0 -> return; name, index loop, `ChangeAnim( pAS )`, `+0x855` @`31d340d4` after the call | match (March writes before the call; `31d29340` writes the same value @`31d296fc`, nothing reads it between) |
| `CleanAnimStructure` `31d2f9a0` | `ptr & 0x80000001` is MSVC's signed `% 2 == 0`, i.e. `(ptr & 1) == 0`; unsigned `< 0xff`; 0 skipped silently; crash1: KEM `0x1b4` (436), slot 0, `+0x294` find -> Release + 0; odd: KEM 436, slot 0, no Release; `+0x294` cleared after the three name maps | match (June clears `+0x294` after the vector resize, March before it: no difference) |
| `ResetAnimSet` `31d31c10` | `31d06e90` on `+0x294` @`31d31c5c` first; insert `(i, pAS)` only when `GetAnimationSet` >= 0 | match; June inserts **before** the name / index maps (@`31d31dbb`), the Done note says after: independent maps, no difference |
| `MixAnim` `31d34140` | find of mesh `+8` (device id) @`31d34175` before the `+0x84c` and `+0x244` checks -> true; `push_back` @`31d34297` after the register loop and the mixer's Release | match |
| `SetAnimXSkinMesh` / `BuildCloneFrame` | NULL -> false, no log; mesh `+0x84c` == 0 -> KEM `0x1b2` (434) then false. `31d319a1-31d319ab` NULL clone / source -> end | match |
| Bone refresh `31d29950` | `+0x190` / `+0x324` NULL -> false; `+0x31c` (DXMatrix) branch; billboard `+0x320`; inlined walk, no AdvanceTime | match |
| `31c83520` | `+0x3c0` NULL -> nothing; X `((+0x510 * +0x504) * SBU.x) * +0x4ec`, Y / Z `(SBU * (+0x514 * +0x508)) * +0x4f0 / +0x4f4`; `Scale` only if anim `+0xc` (manager) == 0; refresh either way | match: March's `m_vSize` / `m_vManualSize` / `m_vScale` sit at +0 / +0x18 / +0x24, as `+0x4ec` / `+0x504` / `+0x510` |
| `31f6fca0` | writes `+0x510..+0x518`, then `31c83520` | match |
| `SetTransition` `327f69d0` | `if( [ecx+0x218] ) [eax+0x854] = arg` | match. All six world-object `ChangeAnim( name )` calls (`327f887b`, `892f`, `89d0`, `8a8b`, `8aeb`, `8bdc`) push `byte [esi+0x21c]` |

Ghidra spot-checks (read-only): `327f69d0`, `31d2b5e0`, the jump table `31d2ed54`: all three as the Done note says.

Not findings, noted: `JuneRefreshScale` multiplies `m_vManualSize` on top of a `m_vSize` that March's own frame move has already multiplied in place
(`KTDGXMeshPlayer.cpp:2073`); June's `31c83520` reads the same two fields, so this is verbatim. The crash1 / odd log guards a NULL anim mesh that June
dereferences: log text only.

Hygiene (`flagcheck --range b0412386^..b0412386`):
- PLAYBACK `files: 1 PASS, 0 PASS~, 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`
- SETS `files: 1 PASS, 0 PASS~, 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR` / same zero line / `RESULT: PASS`
- BONE_REFRESH `files: 3 PASS, 0 PASS~, 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / same zero line / `RESULT: PASS`
- Every OTHER row is one of R11's other two flags in a shared file, or `Always.h`. `git log -S SERV_IRUHADEV_JUNE_RENDER_SKIN b0412386..HEAD`: nothing.
- The review fix (`--worktree --files KTDXLIB/KTDGXSkinAnim.cpp`, PLAYBACK): `1 PASS`, `1 deleted line(s)` = the comment line replaced inside the flag's own block, `RESULT: PASS`.

Build: no rebuild for a comment. The live exe (R8 review's deploy, md5 `94b8520e`) was built from `main` at `129bae3e` and carries R11 code from a clean tree, so the
Done note's "not a clean build" caveat no longer applies. The user owes nothing. R12's kickoff now lists R11's final state.

## Phase R12 - Skin-anim device ownership

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS`.
Added by R10; rows with owner R12 in `june2017_re/r10_skinanim_sweep.tsv`. After R11; not alongside R6.
- **Map first:** each March caller of `AddModelXSkinMesh` (the `bCloseDeviceAtDestructor` cases), `RemoveModelXSkinMesh`,
  `ReplaceMultiTexXET( int )`, `SetAnimXSkinMesh` and `ClearModelXSkinMeshList`, against June's call site. The goal: no reference
  closed twice and none leaked. Also March's device AddRef (`+0x48` is a plain int in March; `JuneCloseDeviceByPointer` is ported, Effect C9h-7a).
- **Port** the ownership model whole: the R12 rows, `CleanAnimStructure( mesh, xet )` and the dtor.
- **`AddModelXSkinMesh`:** `( file, line )` and the KEM 433 log.
- **`31d2f250` and its two `CX2GUUser` calls:** port per June once the map shows how June's motion collision meets March's lists.

### Phase R12 -- Done

**2026-10-17, Opus 5.5.** Flags `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS`, `_REFS_DEBUG` (short-lived) and
`SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION`. Map: `june2017_re/r12_skinanim_ref_sites.tsv` (156 March sites paired with June's 113; 5 callers
change, 27 not compiled, 0 unsure); 10 funcmap rows `(R12)`.

**Device AddRef** = March's `CKTDXDevice::RefDevice()` (June `31d4d2f0`, InterlockedIncrement on `+0x48`); close = `JuneCloseDeviceByPointer`
(C9h-7a). Both wrapped as `CKTDGXSkinAnim::JuneRefDevice` / `JuneCloseDevice` (the close behind June's device-manager NULL test).

**Ported (REFS):** `AddModelXSkinMesh` refs the mesh unless `bCloseDeviceAtDestructor`, and every XET, takes `( const WCHAR* file, int line )`
(June's 50 callers pass `__FILEW__`, so wide; March's callers keep the defaults) and logs KEM 433; `m_vecAddedSkinMesh` is gone. `AddAnimXSkinMesh`
(NULL test + ref), `RemoveModelXSkinMesh` and `ClearModelXSkinMeshList` (closes, plus collision delete) close; `CleanAnimStructure( mesh, xet )`
closes the four lists after R11's anim-set block and swaps the anim mesh / XET; `SetAnimXSkinMesh` calls it, the dtor passes `NULL, NULL`.
`SetAnimXET` and the four `Replace*XET` as June (`ReplaceAniXET` / `ReplaceMultiTexXET( from, to )` were no-ops in March). `ReplaceMultiTexXET( int )`
returns void; `X2Eqip.cpp:2784` / `2822` stop closing the old XET (June `3295f336` / `3295f485`).

**Balance:** every other site balances unchanged. The anim's refs are its own; callers keep closing what they opened. The one June
`bCloseDeviceAtDestructor = 1` site (`Weapon::LoadDataForNPC`) matches March. Our NPC attach port (`X2GUNPC.cpp:4694`, June `327a97d0` passes 0)
keeps 1: same net count. The map's pre-existing leaks (weapon XET, viewer XET shadowing, VioletMage transform XETs) are March's and June's; not touched.

**Item 4 (MOTION_COLLISION):** March's skin parts each carry the full skeleton with `COLLISION_SPHERE1_Face` / `2_Upbody` / `3_Lowbody` and
`ATTACK_SPHERE1_Rfoot` / `2_Lfoot` (checked in `Mesh_Elsword_Basic_LowBody_Sorted.X`, `..._Elite_Upbody_30000`), so every part added its own
copy. June adds the motion's once (`31fe5860 @31fe5c25` -> `JuneAddCollisionData`, = `31d2f250`), and `_UpdateSkin()` (`3295ae20`) and
`CKTDGXSkinAfterImage::AddMesh` (`3322a41e`) pass `bAddCollisionData = 0`. Ported all three. June's second call (`@31fe63a5`) is the unit-type-10
motion pair: character-port material (no March counterpart).

**Deviation (logging only):** KEM 433 reaches ErrorLog.txt for the first 20 mismatches. June's service build buffers ErrorLogMsg in memory;
ours writes the file and stdout per call. The SKREF line carries the total.

**Routed:** June's `_UpdateSkin()` / `_UpdateSkin( int )` / `ChangeEnchantLevel` reworks (multi-tex XET at add time, a third add, enchant
textures by level 7 / 9 / 11 / 13) and the unmapped `32963eb0` -> new **R14**.

Hygiene (`flagcheck --worktree --files` on the 6 code files, per flag): RESULT PASS x3; 0 FAIL, 0 ERROR, 0 deleted, 0 non-ASCII, 0 EOL-mismatched
inserted lines; every OTHER row is one of this phase's three flags. Commit `7ba525ce`; `--range 7ba525ce^..7ba525ce`: REFS 0 PASS / 7 OTHER,
REFS_DEBUG 1 PASS / 6 OTHER, MOTION_COLLISION 2 PASS / 5 OTHER, each 0 FAIL / 0 ERROR / 0 deleted, RESULT PASS x3. Build: `DEPLOY OK: X2_offline.exe is 19,841,536 B, dated 2026-10-17 22:39:22,
md5 ea4a74b48006d7eacdc02768fa36fe9c` (built at `cc6a579d` + uncommitted peer files; SKREF and KEM 433 strings checked in the exe).

**How to see it:** `python tools/logs.py --grep "JUNE_ENGINE] SKREF"`.
- `SKREF motion collision Motion_... collision 3 attack 2` once per user unit.
- `SKREF dead anims N refs R closes C unbalanced 0 ... devices D` at most once per 2 s. Unbalanced should stay 0. D after a return to the
  village should match the first village visit's figure (no growth).
- Equipment, enchant glow and after-images look as before. Monsters still hit you, and you still hit them with kicks.

**Play-test 2026-10-17 (user: all checks run; check 1 FAILED, Lire's kicks never hit).** Log: `SKREF motion collision MOTION_LIRE_COMMON.X
collision 0 attack 0` (x3). March's motion yields no collision data, so MOTION_COLLISION left Lire with none. Ownership is clean:
`unbalanced 0` over 8,354 dead anims (17,038 refs = closes), `kem433 0`, village device count 1066-1075 on each return (no growth).
**Fix:** `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION` turned OFF in `Always.h` (code kept; March's per-part collision back). The trace
(why June's motion carries the spheres: June's mesh loader `31d4f8b0` vs March's `bInValidRadius_` radius test, and which `MOTION_*.X` ships)
is the new **R12b**. Redeploy: ``DEPLOY OK: X2_offline.exe is 19,811,840 B, dated 2026-10-17 23:15:59, md5 e03a4e99268e8e20125a6cd731fdf5c2` (built at `830818ab` + uncommitted peer files; `SKREF dead anims` in the exe, `SKREF motion collision` gone)`.

### Phase R12 -- Review

**2026-10-18, Opus 5.5. PASS on the skin-anim code; 1 caller finding fixed (`X2GUNPC.cpp` attach block), `_REFS_DEBUG` turned off.** Read against June's
code (Ghidra decompile; `disasm_range.py` for the uncarved `31d28f80..31d291d4` and the NPC call site `327a8dd9..327a9880`).

| Item | June | Verdict |
|---|---|---|
| `AddModelXSkinMesh` `31d2f3f0` | NULL mesh -> return; `+0x850 != +0x328` -> KEM `0x1b1` with `3344f350` / `3344f3a8` (file != NULL), args anim frame, mesh frame, name; ref mesh if `!bClose`, then each non-NULL ani / multi-tex / tex-change XET; 5 pushes (ani data NULL) | match (texts byte-equal) |
| `CleanAnimStructure` `31d2f9a0` | after the anim-set / `+0x294` block: close each non-NULL `+0x198` / `+0x1a8` / `+0x1c8` / `+0x1d8` (not `+0x1b8` ani data), 5 resizes, collision / attack delete, `+0x244` Release, then swap `+400` / `+0x194`: ref new, close old if device manager | match |
| `SetAnimXSkinMesh` / dtor | `31d32750` stores through Clean before `GetCloneAC`; dtor `31d31500` calls `31d2f9a0( 0, 0 )` | match |
| `RemoveModelXSkinMesh` `31d2b2f0` | same collision erases; mesh erased then closed; each XET slot closed + nulled before its erase; ani data erased only | match (June erases the mesh slot without March's bound test; the index comes from that list's own loop) |
| `ClearModelXSkinMeshList` `31d2ba80` | close the 4 lists, clear 5, delete collision / attack | match |
| `SetAnimXET` `31d28f80`, `ReplaceAniXET` `31d28fd0`, `ReplaceMultiTexXET` `31d29070` / `31d2b280`, `ReplaceChangeXET` `31d290f0` / `31d29170` | `from == slot && slot != to`; ref new, close old (device-manager test), store; ani version nulls `+0x1b8[i]` when in range; `( int )` ignores the index, empty list -> ref + push | match, every instruction |
| `X2Eqip` `ChangeEnchantLevel` | runs only when the member XET is NULL, so slot 0 held NULL; the anim's ref on the new XET is closed by Clean, the member's own by the dtor (`X2Eqip.cpp:364` / `368`) | balanced |
| `X2GUNPC.cpp:4694` (ours, `JUNE_ATTACH_SKIN_MESH_ANIM`) | `327a97d0` pushes `bClose 0`, `bAddCollision 1`, XET in the tex-change slot; the mesh from `31cdef40` (its find `31cdccd0` / add `31cde970` both take a reference) lives in a local only (@`327a901a`), and `RemoveModelXSkinMesh` has no NPC caller | **finding:** R12 said "same net count"; it is not. June leaks the open's reference, ours closed it. Fixed under REFS: `false` as June, `true` in `#else`; **JUNE_BUGS 271** |
| MOTION_COLLISION (OFF) | `31d2f250`: `+0x8b8` / `+0x8c4` loops, new `0x9c`, copy, link, KEM 432, push to `+0x1f0` / `+0x1fc`, no list push, no ref | transcription matches; flag-off is March (the X2GUUser / X2Eqip / AfterImage hunks are all inside `#ifdef`) |

KEM 433's 20-line cap is the logging deviation the Done note names; the SKREF total carried the count (0 in both runs).

**Logs.** R12 run: `unbalanced 0` over 8,354 dead anims, village devices 1066-1075 on each return. A second run (W1c exe `2455a22f`, carries R12, game
still open at the read): `unbalanced 0` over 6,756 dead anims (13,831 refs = closes), `kem433 0`, `KEM_ERROR433` 0; device count rises and falls
with scene changes (745..1338), no monotonic growth. Balance proven: `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG` is now commented out (R12b
defines it again for its own gate). The 4694 fix keeps the SKREF balance (the extra reference is the open's, outside the anim).

Hygiene (`flagcheck --range 7ba525ce^..2bb92772`):
- REFS `files: 0 PASS, 0 PASS~, 0 DEFINE, 8 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`
- REFS_DEBUG `files: 1 PASS, 0 PASS~, 0 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR` / same zero line / `RESULT: PASS`
- MOTION_COLLISION `files: 2 PASS, 0 PASS~, 1 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / `1 deleted line(s)` = the `#define` that 2bb92772 commented out / `RESULT: FAIL` only for "not #defined anywhere": the intended OFF state, not a hygiene fault.
- Every OTHER row is one of R12's three flags in a shared file (`KTDGXSkinAnim.*`, `X2Eqip.cpp`, `X2GUUser.cpp`, `KTDXDeviceManager.h`, `KTDGXSkinAfterImage.cpp`, `Always.h`). The one later commit on these files (`2d9ad701`, W1c) is another flag.
- The review fix (`--worktree --files X2Lib/X2GUNPC.cpp KTDXLIB/Always.h`): REFS `1 PASS, 1 OTHER` (Always.h), 0 deleted, `RESULT: PASS`; REFS_DEBUG `RESULT: FAIL` for "not #defined" only (now off on purpose), 0 deleted.

Build: `DEPLOY OK: X2_offline.exe is 19,810,304 B, dated 2026-10-18 02:39:54, md5 94e0af9e07b7fd621128d5ee1f729e26` (built at `55f0542e` + this fix + peer files `X2TFieldNpc.h` / `JUNE_FIELD_NPC_HEAD_BONE`; `SKREF dead anims` gone from the exe, 0 hits).

**Play-test 2026-10-18: PASSED** (user: all behave normally; exe md5 `94e0af9e`, clean exit). Lire's kicks hit; Lanox Fire Temple Expert: 24 `JUNE_ATTACH_SKIN attach` lines (20 Fire Devil Fighter, 4 Berserker), shells and fighting normal, no crash on leaving; 0 EXCEPTION, KEM 432 / 433 0, no SKREF lines (debug off as intended). User owes nothing. Next: R12b, R14.

## Phase R12b - Motion collision source (trace)

**Run with:** Opus 5.5 high (trace, then port), then a user play-test. Flags: R12's `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION` (off) and
whatever the trace adds. Added by R12's play-test: with June's switch on, March's `MOTION_LIRE_COMMON.X` gave 0 collision / 0 attack data,
so Lire's kicks never hit. June's game hits, so this is a port gap, not a June quirk.
- **Trace** June's mesh loader collision path: `31d4f8b0` (12 references to `"COLLISION_SPHERE"` `334532cc` / `"COLLISION_LINE_START"`) against
  March's `XSkinMeshProxy::ComputeCollisionData` / `AddCollisionData` (`KTDXDeviceXSkinMesh.cpp:574-`), above all the radius test
  (`bInValidRadius_`, `D3DXFrameCalculateBoundingSphere` on a frame with no geometry), and where June calls it for a motion mesh.
- **Files:** which `MOTION_LIRE_COMMON.X` (and other `MOTION_*.X`) the March koms ship, March's or June's, and whether its `COLLISION_*` frames
  carry geometry or a stored radius (`tools/junekom.py` for June's copy; never edit a June asset).
- **Port** what makes June's motion yield the spheres, then turn MOTION_COLLISION back on. Gate: `SKREF motion collision MOTION_LIRE_COMMON.X
  collision 3 attack 2` (or June's real counts), and the user's kicks hit.

### Phase R12b -- Trace 1 (open)

**2026-10-18, Opus 5.5 high. No code, nothing built.** Result: June's own code plus June's own data give a June user unit **no** body or
kick spheres. That cannot be true of the live game, so the route June really uses is still unfound and nothing was ported. User
(2026-10-18): trace again on Opus 5.5 **xhigh**; `kickoffs/june-engine/R12b.md` rewritten for trace 2. Kicks hit today because
`SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION` stays OFF (March: every skin part adds its own spheres).

Proven (each item checked in Ghidra or by a raw byte scan of `unpacked_x2.exe`):
1. **Files.** `Motion_Lire_Common.x` is KSM v0x200 in March (data033, 1,316,379 B) and June (data201, 6,457,469 B). Both carry
   `COLLISION_SPHERE1_Face` / `2_Upbody` / `3_Lowbody` and `ATTACK_SPHERE1_Rfoot` / `2_Lfoot` with stored radius 0 (KSMFrame +0x44), no mesh
   container and no child. June's own Elsword / Arme / Raven / Eve / Chung / Ara / EL / Add `_Common` motions (data199-207) are the same. In March's
   koms, 73 KSM motions carry collision frames with every radius 0 (every user `_Common` and emotion motion), while 1,344 KSM files store a radius
   above 0 (NPC motions). The parts (`Mesh_Lire_Basic_*_Sorted.X`, March and June) are D3DX (`xof 0303bzip` / `txt`) with sphere geometry.
2. **Loader `31d4f8b0` = March.** Its six collision copies (3 buffer sources x KSM / D3DX) match March. KSM reads the stored radius (frame +0xd0
   from KSMFrame +0x44 @31d503b6) and drops 0 (fld / fldz / fucompp / test ah,0x44 / jp) for SPHERE, LINE and ATTACK_*; GUARD computes when 0.
   D3DX calls D3DXFrameCalculateBoundingSphere. Nothing else writes a mesh's lists (+0x8b8 / +0x8c4): a byte scan finds only the ctor, the dtor
   and the `_Load` swap (31d5892d / 31d58adf).
3. **Copy into a skin anim: a closed set.** Raw call scans match Ghidra's xrefs: LinkCollisionDataFrame `31d2ed70` (6), CollisionData ctor
   `31d379d0` (42: the loader + VioletMage skill code `32cd*`), assign `31d38950` (6). Only three functions copy: AddModelXSkinMesh `31d2f3f0`
   (param 7 = bAddCollisionData), AddAnimXSkinMesh `31d2f720` (0 callers) and JuneAddCollisionData `31d2f250` (InitMotion only). Neither
   function's address is stored anywhere in the exe (no indirect calls), there is no `jmp` tail call to either, and the KEM 433 strings have
   one use each (no inlined copy).
   All 51 direct AddModelXSkinMesh calls: `june2017_re/r12b_addmodel_calls.tsv` (file / line / bClose / bAdd; Ghidra lists 49 and misses
   32989843 `X2GuEve.cpp` and 326f3c37 `X2Pet.cpp`). Every X2Eqip add into the unit anim (+0x1c0) passes 0. The bAdd = 1 sites are the NPC
   weapon, attach-anim items (3295dfa0), riding pet, sub-equip, UI / viewer, field and square units, Rose's cannon (31fd9970 / 31fd9af0), Eve
   (32989843) and Lire's ChargeSoul (32c85d50, its own anim +0x9630). None adds a user's body part with 1.
4. **Consumers.** June's `CX2GUUser::UpdateEquipCollisionData( vector& )` = `31fd8c00`. It is March's loop over UT_ATTACH_ANIM (+0x1c8 == 2)
   normal / hyper anims, then the unit anim's +0x1f0 / +0x1fc into the sets +0x200 / +0x210. June adds a riding-pet branch: the pet anim's
   collision list (+0x238), and the unit's own collision list is skipped when pet +0x478 != 1. UnitToUnit `32625000` -> `32611970` takes the
   defender's +0x200 set. So June's hit spheres do come from the unit anim's lists.
5. **InitMotion `31fe5860`** opens templet +0x178 (`m_CommonMotion`) twice with `"Bip01"` through OpenXSkinMesh `31cdef40` (plain cache or
   load), then calls JuneAddCollisionData( +0x87cc ) on +0x87d4. With radius 0 everywhere, that adds nothing.

Leads for trace 2 are in the kickoff. Rows tagged `(R12b)` in `june2017_re/funcmap.tsv`.

### Phase R12b -- Done (trace 2 + port)

**2026-10-18, Opus 5.5 xhigh. Route found; trace 1's item 5 was wrong.** June's InitMotion (`31fe5860`) opens the templet's
**`m_GameMotion0`** (templet `+0x178`, e.g. `Motion_Lire.x`) as `m_pMotion` / `m_pMotionFuture`, and mixes `MOTION_FILE_NAME` (the class
Lua's `Motion_*_Common.x`) first. March does the reverse. Proof of `+0x178`: the templet parser `3237aa40` stores `m_GameMotion0` at
node pair + 4 + 0x178 (`@3237c865`), and June's own "clear the mix list when `m_GameMotion0` is empty" test reads `+0x178` (`@31fe5d1f`).
June's `m_GameMotion0` files store their sphere radii (body 52 / 44 / 47, feet 39 / 39 on most classes); the `_Common` files store 0.
Leads 1-4 are all closed as "no other route": 6 list pushes into anim `+0x1f0` / `+0x1fc` over all `.text`; mesh lists read only by the
three copy functions; hit test 32611970 = March's math on the defender `+0x200` set, no fallback; eqip `+0x1c0` = unit `+0x87d4`.
Funcmap rows tagged `(R12b)` (8 new).

**Landed** (commit: this one). `X2Lib/X2GUUser.cpp`, `KTDXLIB/Always.h`:
- `SERV_IRUHADEV_JUNE_RENDER_MOTION_ORDER` (new): main motion = `m_GameMotion0`, `MOTION_FILE_NAME` first in the mix list. `#error` if
  MOTION_COLLISION is on without it, or without `SEPARATION_MOTION`.
- `SERV_IRUHADEV_JUNE_RENDER_MOTION_CASH_EMOTION` (new): `31ee9150`, the unit type's `Motion_<Class>_Emotion_Cash25` appended after
  the emotion motion. Those files are June-only (data199-209, june-asset-port P2-P4); until then `OpenXSkinMesh` returns NULL and logs
  KEM_ERROR428 once per name.
- `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION` **on again**; `_SKIN_ANIM_REFS_DEBUG` on for this play-test (off at the review).
- No skin part adds a second copy: every X2Eqip add into the unit anim passes `false, false` under MOTION_COLLISION (R12); the
  remaining default-collision adds of a user motion are the bunker-cannon ride (`Motion_BunkerCannon.x`, = June 31fd9970 / 31fd9af0).

**Data** (user, 2026-10-18). March's data033 `Motion_Raven.x`, `Motion_CHUNG.x` and `Motion_EL.x` store radius 0 (March relied on the
parts). Staged June's byte-exact `Motion_Raven.x`, `Motion_CHUNG.x`, `Motion_EL.x`, `Motion_EL_SK.x` into `kom_contents/data033_kom`
(`kom.py stage-june`; March originals in `kom_backup_stage_june\20261018_064307`). The user first asked for every character motion;
the census (`june2017_re/r12b_anim_loss.tsv`, 43 rows) showed June renamed the animations its class scripts play (Lire `ComboX3` ->
`Lire_ComboX3`, Elesis `SI_A_EL_LEAP_CRASH_*` -> `SI_A_SSK_LEAP_CRASH_*`, ...), so the user chose the 4 sphere files now and the full
swap with each class's character port (checklist 7ad; june-asset-port P5 kickoff amended). The other 83 staged files were restored
to March's bytes. Pack: `PACK OK data033` (vs live: ~4 changed = the 4 files). Copy (user OK): `COPY OK data033: ... 131234168 bytes, 165 entries, md5 176b993a66996c3a82370ffe38414df5`.
Known cost, accepted by the user: 6 Elesis skill animations (`SI_A_EL_LEAP_CRASH_Start/Jump/Landing`, `SI_A_EL_RUSHING_SWORD`,
`SI_SA_SSK_SPIRAL_BLAST`, `SI_SA_SSK_VICTORIOUS_SWORD`) are gone until the Elesis port / skill S3.

**Hygiene.** `flagcheck --worktree --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h` with all four flags stripped: X2GUUser.cpp PASS (+47,
insert-only); Always.h FAIL on comment lines only (three flags' define blocks, +1 / +3 / +30, insert-only; the one deleted line in the
worktree is a peer's). Range `74dd25e6^..74dd25e6`, MOTION_ORDER `--with` the other three: `files: 1 PASS, 0 PASS~, 0 DEFINE,
0 OTHER, 1 FAIL, 0 ERROR` / `0 deleted line(s)` / `RESULT: FAIL` = Always.h's comment lines (multi-flag define shape), X2GUUser.cpp PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to
X2/US_SERVICE/x2.exe (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s)` (peers' files, incl. skill S3's
uncommitted reform). The exe carries the Cash25 names and the `SKREF motion collision` line.

**How to see it in game:** `python tools/logs.py --grep "JUNE_ENGINE] SKREF"` -> `SKREF motion collision MOTION_LIRE.X collision 3
attack 2` per Lire unit (Raven: collision 3 attack 4; others 3 / 2). Monsters hurt every class; kicks and combos hit.
**Routed:** the viewer half of the Cash25 append (`31faa6f0`) -> backlog Stage 10; the full motion swap -> checklist 7ad.

**Play-test 2026-10-18: PASSED** (user: kicks and combos hit, monsters hurt; exe md5 `0818bfeb`, run after the deploy). Log, one line per
user unit: `SKREF motion collision MOTION_ADD.X / MOTION_ELSWORD.X / MOTION_LIRE.X / MOTION_CHUNG.X / MOTION_EL.X collision 3 attack 2`.
Raven (`MOTION_RAVEN.X`, expected 3 / 4) is not in this run's log. KEM 428 once per class for `MOTION_<CLASS>_EMOTION_CASH25.X`, as
expected. Ownership clean (`unbalanced 0`, 0 EXCEPTION; UNHANDLED = the `E_DISABLE_HB_CHECK_REQ` baseline). New: `kem433 66` when the
Elesis unit loads (`Anim Frame : 79, Mesh Frame : 80` for March's `MESH_EL_*_SORTED.X` parts): June's `Motion_EL.x` lacks one
unnamed frame March's file and parts carry; every named bone matches, so it is a warning only. It goes with June's Elesis meshes in
the Elesis port (checklist 7ad).

### Phase R12b -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change.** The only edit is `_SKIN_ANIM_REFS_DEBUG` off again (`Always.h:7853`).
Ghidra and the June exe were read only.

- **Main motion:** `31fe5860 @31fe5a48-31fe5b4c` opens unit `+0x867c` -> `+0xaf0` (templet) `+0x178` twice (SSO test `+0x18c >= 8`),
  stored to `+0x87c8` / `+0x87cc`. `+0x178` = `m_GameMotion0`: the parser `3237aa40` decodes key `337a20ac` = `m_GameMotion0`
  (xorstrings.tsv:3514) and stores it at `ESI + 0x17c` (`@3237c865`, templet = ESI + 4); `@31fe5d1f` compares `+0x178` with `L""`. The port matches.
- **Mix order:** `@31fe5c78` pushes the local at `ESP+0x38` (the `MOTION_FILE_NAME` string) first. Then grade 2 pushes `+0x1b0` (GameMotion2)
  and falls into `+0x194` (GameMotion1), grade 1 pushes `+0x194`, and both push `+0x15c` (Emotion). After that comes `31ee9150( templet[0], 1, vec )`
  (`@31fe5cfa`), then the `+0x178 == L""` clear. June inlines the grade (class 10-199: 1 + (class >= 100), else 0). That equals March's
  `GetX2UnitClassGrade` (`X2Unit.h:2084`) for every March class, since all are below 200. The `#else` paths are March's lines, untouched.
- **`31ee9150`:** the jump table at `31ee9470`, read from `unpacked_x2.exe`, maps 1 Elsword, 2 Arme, 3 Lire, then 4-9 Raven / EVE / CHUNG / Ara / EL / Add,
  10 Lu (bool == 1) or Ciel, 11 Rose, 12 Ain. Strings `3345c238`..`3345bf7c` match the port character for character, including the
  `.x` / `.X` case. Any other type does nothing.
- **No second copy:** under MOTION_COLLISION every add into the unit anim (`X2Eqip.cpp:2074 / 2092 / 2099 / 2171 / 2201`) and every AfterImage
  add (`KTDGXSkinAfterImage.cpp:77`, `:208`, `:215`) passes `false, false`. `X2Eqip.cpp:2600 / 2689` add with collision into the item's
  *own* attach anims, which are separate objects (R14b scope), not the unit's.
- **Play-test log:** checked from the R12b Done note above. That run's log has since been overwritten by a later launch, whose log has no unit
  load yet. The lines were `collision 3 attack 2` for Add / Elsword / Lire / Chung / Elesis, Raven not played, `unbalanced 0`, 0 EXCEPTION.
  A Raven run (expected 3 / 4) stays optional.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_RENDER_MOTION_ORDER --range 74dd25e6^..74dd25e6 --with ..._CASH_EMOTION ..._MOTION_COLLISION ..._SKIN_ANIM_REFS_DEBUG`:
`files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 1 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: FAIL`. The FAIL is `Always.h` (+34, insert-only). The three `//` "ON again" comment lines are inserted under the commented
MOTION_COLLISION define, inside its block, so flag-off differs only on comment lines (the multi-flag define shape). `X2GUUser.cpp` is PASS (+47).
The review edit, `flagcheck SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG --worktree --files KTDXLIB/Always.h`, gives `1 DEFINE`, `1 deleted line(s)`
(the `#define` turned into a comment), and `RESULT: FAIL` "not #defined anywhere". Both are intended, since the debug flag is off.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,944,960 B, dated 2026-10-18 08:09:08, md5 00703adcea5d79dfdd6c61b040922363, equal to X2/US_SERVICE/x2.exe (2026-10-18 08:09:03); built at main 7a43b7a3 + 6 uncommitted code file(s)` (peers' files plus Always.h).

No open questions.

## Phase R14 - Equipment skin and enchant rework

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_EQIP_SKIN` (an RE pass
may split it). Added by R12 (an unowned June branch). After R12; not alongside another session editing `X2Eqip.cpp`.
- **`CX2Eqip::_UpdateSkin()` `3295ae20`**: three `AddModelXSkinMesh` calls (March two), the multi-tex XET (`+0x808`) passed at add time where
  March passes NULL, `bAddCollisionData = 0` (R12 already ports that value under `JUNE_RENDER_MOTION_COLLISION`).
- **`_UpdateSkin( int )` `3295bdf0`** and the unmapped **`32963eb0`** (X2Eqip line `0xf45`, `L"Bip01"`): compare whole.
- **`ChangeEnchantLevel` `3295f250`**: level < 7 deletes stage 2; else `13Gang_Protect.tga` (>= 13), `11Gang_Protect.tga` (9-12),
  `Gate_Protect_G01.tga` (< 9) via `31d40160`, against March's `ENCHANT_WEAPON_EFFECT_LEVEL_*` / `ADD_UPGRADE_WEAPON_PARTICLE` choice. R12 ported
  only the `ReplaceMultiTexXET` ownership. The textures are June assets (check they are in the March koms; if not, route to june-asset-port).
- Funcmap rows `3295ae20 / 3295bdf0 / 32963eb0 / 3295f250 (R12)`.

### Phase R14 -- Done

**2026-10-18, Opus 5.5.** One flag, `SERV_IRUHADEV_JUNE_EQIP_SKIN` (the RE split nothing out); code in `X2Eqip.cpp` only. Funcmap rows `(R14)`.

**Map** (June vs March):
- `_UpdateSkin( int )` `3295bdf0` = March (LastErrorLog, the same Remove / Add, NULL multi-tex, collision 0 = March's `false, false`). Not changed.
- `_UpdateSkin()` `3295ae20`, rewritten: an "all models" bool = templet and `IsDivisionItem` group 6 / 0x31 / 0x36 / 7 (`32809f40`, a twin of
  `32807db0`), OR the eqip's custom-render vector (+0x92c) holds 2+ entries, OR owner unit type 10 with ctor flag +0x15d4 clear and model[1] set.
  With it the normal add attaches models i..first empty (the third add, line 0xe6d) and the removes take models 0..first empty. All three adds
  pass the NORMAL multi-tex XET (+0x808), the hyper add too.
- `32963eb0` = **`CX2Eqip::InitSkin`** (`L"Bip01"` is `OpenXSkinMesh`'s default bone, as March). It calls `3295ffd0` first: the four XETs always;
  for a weapon position (templet +0xd5 = 2 / 0x19) or no templet, `MULTI_TEX_%s_%dl` into the normal multi-tex XET and `MULTI_TEX_HYPER_%s_%dl`
  (from the HYPER model name) into +0x814 = `m_pHyperTexChangeXET`, each with the enchant texture. Then the models (LoadEquip + OpenXSkinMesh =
  March) and, new, one skin anim per hyper model (Set / Add with the hyper multi-tex + tex-change XETs, collision 0, `ChangeAnim( m_AniName )`,
  loop, UseDXMatrix).
- `ChangeEnchantLevel` `3295f250` and `InitAttatchAnim` `3295dfa0`: the chain >= 13 `13Gang_Protect.tga`, 9-12 `11Gang_Protect.tga`, 7-8
  `Gate_Protect_G01.tga`, < 7 delete stage 2 (hyper attach-anim: nothing). March: >= 11 11Gang, 7-10 G01. ChangeEnchantLevel is March otherwise.

**Ported:** InitSkin with `3295ffd0`, whole except the routed hooks; `_UpdateSkin()` whole, the bool from its division-item term only; the chain
at all six sites (`JuneSetEnchantStage2`, one gate line each). Each new skin add's collision argument sits under
`JUNE_RENDER_MOTION_COLLISION` (OFF until R12b: March's `true`). `JUNE_BUGS.md` 275-277: the hyper multi-tex XET over the hyper tex-change XET,
the hyper add with the normal multi-tex XET, the hyper skin anim on the normal anim name.

**Routed:**
- Stage 11 (`JUNE_PORT_BACKLOG_ORDER.md` Stage 11, bullet added): the custom-render vector term (+0x92c from `32810450`), InitSkin's per-item
  frame vectors (`32812f40` -> frame +0x108 / +0xfc via `329656d0`) and the mesh item id (`31cdcc70` -> mesh +0xa40). Groups 6 / 0x31 / 0x36 / 7
  are filled by Stage 11's parser `32814170`, so the all-models bool reads false until then.
- Checklist 7z: the Lu/Ciel term (unit type 10; +0x15d4 is the ctor argument the model-name selector `32867880` takes).
- New **R14b**: the rest of June's `InitAttatchAnim` `3295dfa0` (second anim / model arrays +0x584 / +0x5e4, item type 9 / 0xb index remap,
  appearance templet `325bb740`, division-item anim sharing, jiggle `3295b230`).
- `13Gang_Protect.tga` is June-only (June data045, 262,188 B) and not in the March koms -> june-asset-port P4a. Until then +13 logs `MISSING`
  and shows no glow. `11Gang_Protect.tga` and `Gate_Protect_G01.tga` are in March data045.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_EQIP_SKIN --worktree --files X2Lib/X2Eqip.cpp KTDXLIB/Always.h`: RESULT PASS; X2Eqip.cpp PASS~ (one blank line before the helper block, outside the flag), Always.h OTHER (peers' uncommitted defines; this flag's hunk is 17 inserted lines); 0 FAIL, 0 ERROR, 0 deleted, 0 non-ASCII, 0 EOL-mismatched inserted lines, 0 encoding changes. Strings `EQSKIN stage2`, `13Gang_Protect.tga`, `InitAttatchAnimHyper` checked in the deployed exe.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,872,256 B, dated 2026-10-18 03:21:50, md5 ce79dccff14191fdfb02268e1cd90a81, equal to X2/US_SERVICE/x2.exe (2026-10-18 03:21:45); built at main 60eac05c + 29 uncommitted code file(s)`

**How to see it** (save set for the play-test, backup `db_backup/els_db_20261018_030906_pre_sql.sql`; weapons were reyaa uid 810 +10,
Freils 361 +10, Iivue 579 +0, ravbe 735 +0): reyaa's Grendized Alterasia Type-B Bow +7 = G01 glow (as March); Freils' Cobo's Special Cannon
+9 = **11Gang glow** (March: G01); Iivue's Wild Tracker Drone +11 = 11Gang (as March); ravbe's Cobo Sword for Beginners +13 = **no glow**
until P4a (March: 11Gang). `python tools/logs.py --grep "JUNE_ENGINE] EQSKIN"`: `EQSKIN stage2 InitAttatchAnim item <id> level <n> tex <name> loaded`
(`MISSING` for 13Gang). Skin equipment and awakening look as before (no division-item groups loaded yet).

**Play-test 2026-10-18: PASSED** (user: all checks normal, awakening via `SetHyperModeCount( 3 )`). Log (exe md5 3f645dcc, main 21783037, contains R14): 40 `EQSKIN stage2` lines, ErrorLog.txt 821-1074: reyaa item 111870 +7 `Gate_Protect_G01.tga loaded` (821), Freils 111199 +9 `11Gang_Protect.tga loaded` (895), Iivue 116934 +11 `11Gang_Protect.tga loaded` (927), ravbe 131644 +13 `13Gang_Protect.tga MISSING` (979, as expected until P4a); each fashion weapon's ChangeEnchantLevel line matches (133441, 41200, 229990, 131649); 0 EXCEPTION.

### Phase R14 -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change.** Nothing was rebuilt; the deployed exe (md5 3f645dcc, play-tested) already carries R14.
All five functions were re-read whole in Ghidra (read-only), so this is more than the 2-3 spot-checks the kickoff asks for. None failed.

- **`_UpdateSkin()` `3295ae20`**: the bool is templet (+0x198) non-NULL && `32809f40` on g_pData +0x34 (disasm @3295ae3a) for 6, 0x31, 0x36, 7
  in that order, with templet[0] = item id. `32809f40` and `IsDivisionItem` `32807db0` read the same map (+0x230, end +0x234) and then the
  set, so March's `IsDivisionItem( type, id )` is the right call. The other two OR terms are comments only.
  - Hyper branch: attachment 1 removes models 0..first NULL (all-models) or model i; attachment 2 skips; otherwise the add at line 0xe3d
    ( hyper i, +0x81c, **+0x808**, +0x814 ), then attachment = 2.
  - Normal branch (attachment != 1): 2 removes hyper i; all-models adds models i..first NULL (0xe6d), otherwise model i (0xe82), each with
    ( +0x80c, +0x808, +0x804 ); then attachment = ( model i != NULL ).
  - Not shown / not skin: attachment 1 removes (all-models: 0..first NULL), 2 removes hyper i; attachment = 0.
  - The port matches line for line.
- **InitSkin `32963eb0`**: calls `3295ffd0` first, then the models (LoadEquip 32851d60, OpenXSkinMesh with `L"Bip01"`). With +0x1c5, for each
  non-NULL hyper model (+0x5c4): close the old +0x564 anim, new 0x874 / 31d32040, SetAnimXSkinMesh( mesh, +0x81c, 0 ), AddModelXSkinMesh(
  mesh, +0x81c, +0x818, +0x814, 0, 0, 0xf45 ), ChangeAnim( +0x85c, 0, 0 ), Play 0, +0x31c = 1. Then XEM 0x276d when model[0] (and hyper[0])
  is NULL, else `_UpdateSkin()`. ChangeEnchantLevel's hyper loop walks the same +0x564 array, which confirms it is `m_pHyperXSkinAnim`.
  Matches.
- **`3295ffd0`**: four OpenXET( name, 0 ) calls, unconditional: +0x824 -> +0x804, +0x840 -> +0x80c, +0x878 -> +0x814, +0x894 -> +0x81c.
  The weapon test is `templet == NULL || +0xd5 == 0x19 || +0xd5 == 2` (EP_AC_WEAPON = 25 and EP_WEAPON_HAND = 2, `X2Unit_PreHeader.h`).
  Then OpenXET( name, 1 ): `MULTI_TEX_%s_%dl` from +0x644 -> +0x808, and `MULTI_TEX_HYPER_%s_%dl` from +0x724 (`m_ModelNameHyper[0]`) ->
  +0x814 @32960287 (JUNE_BUGS 275). Matches.
- **Texture chain, six sites** (`3295ffd0` normal and hyper, `3295dfa0` normal @3295e10c and hyper, `3295f250` normal and hyper):
  >= 13 13Gang, 9..12 11Gang (the `< 0xb && < 9` test), 7..8 G01, each `31d40160( "*.*", tex, 7 )`.
  - Below 7: stage 2 is deleted, with a NULL test, at both `3295ffd0` sites, at `3295dfa0` normal, and at both `3295f250` sites (in its own
    `< 7` branch = March's else). The `3295dfa0` hyper site does nothing. The port does the same at each site.
  - The set path has no NULL test in June either (the multi-tex XET is opened with create = 1).
- **Collision argument**: each new add (the three in `_UpdateSkin()`, the hyper skin-anim add in InitSkin) passes `false, false` (= June's
  0, 0) under `JUNE_RENDER_MOTION_COLLISION`, and the defaults (`false, true` = March) while that flag is off (it is off today, Always.h `//#define`).
- **Routed terms** are comments only (custom-render vector, Lu/Ciel, frame vectors, mesh +0xa40). Each has an owner:
  JUNE_PORT_BACKLOG_ORDER.md Stage 11 (the R14 bullet), checklist 7z, and `kickoffs/june-engine/R14b.md` (the rest of `3295dfa0`;
  checked, it needs no change). JUNE_BUGS 275-277 were checked against the same addresses and are correct.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_EQIP_SKIN --range a5aeb19b^..a5aeb19b`:
`files: 0 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 11 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. `git log -SSERV_IRUHADEV_JUNE_EQIP_SKIN a5aeb19b..HEAD` lists nothing.
- X2Eqip.cpp PASS~: one blank line after the helper block.
- The OTHER rows come from a5aeb19b being committed from a stale snapshot (see a40bb330). It restored S2's `JUNE_SKILL_TEMPLET` / `_DEBUG`
  (Always.h +21, X2DamageEffect, X2DamageManager, X2GUUser.h/.cpp, X2Main, X2SkillTree.h/.cpp, X2UserSkillTree.h/.cpp).
- It also dropped W1e: Always.h -17 (the `JUNE_LINEMAP_CANDOWN_LINK` define and its `_DEBUG` block) and KTDGLineMap.cpp -93. a40bb330
  restored both; all 17 deleted Always.h lines are in HEAD.
- Always.h's other +17 lines are this flag's block (with the `#error` on `JUNE_EQUIP_EFFECT_SET`, which is defined).

No open questions. Next: R14b.

## Phase R14b - Attach-anim equipment init (June rework)

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM` (an RE pass
may split it). Added by R14 (an unowned June branch). After the R14 review; not alongside another session editing `X2Eqip.cpp`.
- **June's attach-anim init `3295dfa0`** against March `InitAttatchAnim`: a second anim / model array (+0x584 / +0x5e4), the item-type 9 / 0xb and
  `EP_WEAPON_HAND` / `EP_AC_WEAPON` index remap (4..7 -> 0..3), the appearance templet `325bb740` (item flag +8 bit 2), the model-name selector
  `32867210`, `329657d0`, the jiggle value `3295b230`, the division-item anim sharing (anim +0x84c / +0x840 / +0x844), the hyper half and the tail.
- R14 already ported this function's enchant texture choice (`JUNE_EQIP_SKIN`). Lu/Ciel minor-unit material goes to the checklist (7v / 7z).
- Funcmap rows `3295dfa0 (R12)` / `(R14)`, `32961280` (ctor).

### Phase R14b -- Done

**2026-10-18, Opus 5.5.** One flag, `SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM` (the RE split nothing out); code in `X2Eqip.cpp` `InitAttatchAnim` only.
Funcmap rows `(R14b)` (10 rows). No June bug kept, so no `JUNE_BUGS.md` row.

**Map** (June `3295dfa0` vs March):
- XET opens, the `MULTI_TEX_%s_%dl` names, the normal / hyper anim builds (Set / Add collision 1 / ChangeAnim( name, false ) / XAP_LOOP /
  UseDXMatrix) and the templet-type Add remap of the normal half = March. R14's texture chain and R12's adds are untouched.
- **Add frame gate (new):** when the OWNER unit (+0x1b4 -> +0xaf0) is Add and the item is on EP_WEAPON_HAND / EP_AC_WEAPON, slot i gets a model
  only where the unit skin (+0x1c0) has `Weapon0<i+1>`. March opened all 8 slots (each model 0); slots 7 / 8 had no frame, so no matrix and no frame move, yet were still drawn.
- **Hyper half (new):** the same Add remap to hyper model 0 (March's hyper loop opened slot i).
- **Jiggle (changed):** normal and hyper anims jiggle for `DIVISION_ITEM_TEMPLET_TYPE` 30 (`DITT_ITEM_ID_JIGGLE_TYPE1`, `32809f40`), tension 1.5
  (33471a1c), expand 0 / 4. June moved March's `ITEM_ID_JIGGLE_TYPE1` ids (`X2Define.h:559`) into `RenderDivisionItemTemplet.lua`, so they jiggle
  again when Stage 11 fills group 30. `3295b230` is June's `GetJiggleBone` (group 30 -> 1.5, else -1.0); in this function only the minor-unit
  block calls it, so March's inline `GetJiggleBone` stays as is.
- **Tail:** the Add `Weapon0<n>` lookup null-checks the unit skin; an attach frame named `"TraceLandPos"` binds the unit skin's clone frame root
  (skin anim +0x324 = `m_pCloneFrameRoot`, proven by `31d32620` = March `KTDGXSkinAnim.cpp:401-407`). 95 June kim items use it, all with June-only meshes.
- Named: `329657d0` (minor-unit item by position), `325bb740` (`GetItemTemplet`), `32867210` / `328671e0` (`GetModelName` / `GetAttachFrameName`),
  `329658b0` (`SetUnitXSkinAnim`), `32807e00` / `3295a570` (DITT 72 record, change every attach anim). The "appearance templet + flags +8 bit 2"
  from the kickoff is `GetItemTemplet` + `m_bIsForMinorUnit` (flags2 bit 2) -> `m_ItemIDForMinorUnit` (+0x30): Lu/Ciel material.

**Routed** (each a comment at its site):
- Checklist **7aa**: the Lu/Ciel minor-unit anim / model pair (+0x584 / +0x5e4). Checklist **7ab**: the Rose weapon frames (0xb, slots 1..4 -> frame 1).
- Backlog **Stage 11** (bullet added): the group-30 data, and the DITT 72 (`DITT_LOOP_ANIM_ITEM_ID`) tail (`32807e00` -> `3295a570`, eqip +0x15f0).

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM --worktree --files X2Lib/X2Eqip.cpp KTDXLIB/Always.h`: RESULT PASS; X2Eqip.cpp
PASS +109, Always.h OTHER (peers' uncommitted defines; this flag's hunk is 16 inserted lines); 0 FAIL, 0 ERROR, 0 deleted, 0 non-ASCII,
0 EOL-mismatched inserted lines, 0 encoding changes.
**Range:** `flagcheck SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM --range d523b1d9^..d523b1d9`: X2Eqip.cpp PASS +109, Always.h DEFINE +17; 0 FAIL, 0 ERROR, 0 deleted; RESULT: PASS.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to X2/US_SERVICE/x2.exe (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s)`

**How to see it** (save set for the play-test: LunaticPsykerT uid 36 wears Add's Basic Dynamo 89; jtLordKnight uid 25 wears Miracle Alchemist
Light (Elsword) 85811, a `TraceLandPos` item whose mesh March lacks; backup `db_backup/els_db_20261018_064009_pre_additem.sql`):
- LunaticPsykerT in a village and a dungeon, awakening on and off: the dynamos look as before. Log: `EQATTACH item 89 ... addslots_skipped 2`
  (Weapon07 / 08 absent), `hyper_remap` 1 when the item can awaken.
- jtLordKnight: `EQATTACH item 85811 ... tracelandpos 1` (no visible mesh until the asset port).
- Every other class: weapons unchanged; an eqip with an `ITEM_ID_JIGGLE_TYPE1` id logs `jiggle_lost 1` (no jiggle until Stage 11).
Strings `] EQATTACH item `, `TraceLandPos`, `addslots_skipped` checked in the deployed exe (the build that deployed it was a peer's, from the same tree).

**Play-test 2026-10-18: PASSED** (user: LunaticPsykerT's dynamos float normally, awakened and not, no stray dynamo; the other four characters' weapons and glow unchanged). Log (exe md5 0818bfeb; field, village, dungeon, village; 0 EXCEPTION): ErrorLog.txt 838-942 four `EQATTACH item 89 unittype 9 pos 2 addslots_skipped 2` (Weapon07 / 08 got no model), `hyper 0` (Basic Dynamo has no hyper model, so the hyper remap was not exercised); 947-1000 five `EQATTACH item 85811 ... tracelandpos 1`, no crash; no line for reyaa / Freils / Iivue / ravbe; no `jiggle_lost`.

### Phase R14b -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change.** Nothing was rebuilt; the deployed exe (md5 0818bfeb, play-tested) already carries R14b.
`3295dfa0` was re-read whole in Ghidra (decompile + disasm, read-only), plus `32809f40` and `3295a570`. Every kickoff point matches.

- **Layout:** June eqip +0x198 templet, +0x1a0 item id, +0x1a8 UID, +0x1b4 owner, +0x1c0 unit skin anim, +0x1c5 `m_bCanHyperMode` follow March's
  member order (`X2Eqip.h:306-324`). Owner +0xaf0 -> [0] is `m_pUnitTemplet->m_UnitType` (`X2Unit.h:290`, `:1236`; funcmap `31e4d0e0`).
- **Add frame gate:** owner non-NULL, +0xaf0 non-NULL, type 9, and templet +0xd5 is 2 or 0x19. Then `Weapon0%d` (i+1) and 31d38a20 on +0x1c0.
  A NULL skin or no frame skips the slot (no model, no anim); otherwise -> `LAB_3295e20b` = the normal path. The index remap after it tests
  templet +0xd2 == 9 (templet unit type = March). The port matches, including the `GetUnitTemplet()` NULL test.
- **Jiggle:** templet non-NULL && `32809f40`( 0x1e, templet[0] ). That is the (type, id) twin of `IsDivisionItem` (map +0x230 / end +0x234, then the
  set; `X2ItemManager.h:606`). It sets +0x84c (unit skin, ref-counted = `SetUnitXSkinAnim`), +0x844 = 1.5 (33471a1c), +0x840 = 1, and +0x848
  = 0 normal / 4 hyper, which is `SetJiggleMesh( true, 1.5f, 0 / 4 )` (`KTDGXSkinAnim.h:63-68`). `JIGGLEBONE_TEST` is defined (`Always.h:3102`),
  so this code is live. `3295b230` is called only in the minor-unit block (+0x584); the normal and hyper anims have no other jiggle.
- **Hyper half** (+0x1c5): +0xd2 == 9 and pos 2 / 0x19 -> `m_ModelNameHyper[0]` (+0x724 + idx*0x1c). No owner gate here in June, and the port has none.
- **Tail:** the Add branch null-checks +0x1c0 before `Weapon0<n>`. The default branch runs GetCloneFrame( name ) (NULL skin = 0). Then
  31c1c1a0( 0, size, `"TraceLandPos"`, 12 ) == 0 replaces that frame with skin +0x324 (`m_pCloneFrameRoot`, `GetCloneFrameRoot()`
  `KTDGXSkinAnim.h:297`), or 0 when there is no skin. That matches `wcscmp == 0`. The Rose branch (0xb, pos 2, i-1 < 4 -> frame 1) and the DITT 72
  tail are comments only. The DITT 72 comment matches `3295a570` (+0x1c8 == 2: ChangeAnim( name, true ) on normal / hyper-if-can / minor).
- **Untouched:** R14's texture chain (@3295e10c and the hyper site) and R12's AddModelXSkinMesh calls sit outside every R14b hunk.
- **Routed:** checklist 7aa / 7ab (`JUNE2017_CHARACTER_PORT_CHECKLIST.md:646`, `:655`), backlog Stage 11 R14b bullet (`JUNE_PORT_BACKLOG_ORDER.md:398`),
  10 funcmap rows `(R14b)`.

**Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM --range d523b1d9^..d523b1d9`:
`files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. `git log -SSERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM d523b1d9..HEAD -- X2Lib KTDXLIB` lists nothing.

No open questions. The hyper remap was not exercised in the play-test (Basic Dynamo has no hyper model). It is a two-line index change, the same
as the normal half's, and needs no extra test.


## Phase R13 - World object mesh render

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_WORLD_MESH`.
Added by R8 (an unowned June branch). Not alongside R8's review or W1 / W1b / W2 (`X2World*`).
- **June has no `CX2WorldObjectMesh::IsInFrustum` override.** Vtable `334bc484` (RTTI COL `3361b658`) has the base `31c829e0` at slot 0x34.
  March's subset culling (`X2WorldObjectMesh.h:120`, `m_vecDrawSubset`) must be checked against June's draw: does June cull subsets somewhere else?
- **Prepare `327f9020`** (slot 8) is rebuilt:
  - a distance fade (+0x1ec / +0x1f0 against +0x148);
  - the layer forced to 0xff, with the old one kept at +0x324;
  - a vslot 0x44 test with DG +0x1c0 / +0x1cc.
- Map the class whole (ctor, Prepare, Draw, the Lua keys feeding the new members), then port it as June has it. Since R8 the override takes
  `fAddRadius_`; R13 decides whether it stays.

### Phase R13 -- Done

**2026-10-17, Opus 5.5.** Every slot of vtable `334bc484` was mapped and is in 9 funcmap rows tagged `(R13)`. Three flags are in `Always.h`.

**The map.** The members R8 called new are March's own:
- `+0x1ec` / `+0x1f0` are the base `m_fHideDistance` / `m_fHideNearDistance`.
- `+0x2ec..+0x2f4` are `m_fNowAlpha`, `m_bCanHide`, `m_bNotRenderWhenHidden`, `m_bAnyCamera` and `m_BillboardType`.
- No Lua key is new; `SetCanHide` and the others are March's keys.
- "vslot 0x44" is `RayHit( eye, lookAt, &dist, matrix )`, the hide test, not a cull.
- The only June-only member is `+0x324`, the saved layer (ctor `@327fa190` = -1).
- Draw, OnFrameMove, SetLOD, both RayHits and the ctor match March.

**Subset culling (item 2).** June still has `m_vecDrawSubset` (`+0x310`) and Draw still passes it. June's subset cull is March's two-argument
`IsInFrustum`, now at slot 0x48 (`327fa210`). Once the base took a third float it overrides nothing, and a `.text` scan finds no caller.
So June culls world meshes only as whole objects, through the base test, and the empty vector draws every subset (JUNE_BUGS 267, kept).
**`fAddRadius_` (item 3):** the R8 three-argument override is dropped. The class declares June's non-overriding two-argument test.

| Flag | What landed (June) |
|---|---|
| `JUNE_RENDER_WORLD_MESH` | Prepare as `327f9020`. The can-hide test runs only for `m_bAnyCamera` meshes. March's `g_pX2Game` / `CS_NORMAL` gate is gone, so villages hide too, and a `(true, false)` mesh never hides. A hidden mesh is drawn at layer 0xff; its own layer is kept in `m_iJuneSavedLayer` and restored once it is visible. The fade constants are June's 0.3f / 4.0. Also the two-argument `IsInFrustum` above. |
| `JUNE_RENDER_CAMERA_MESH_HIDDEN` | `CKTDGCamera` `+0x19d` (DG `+0x319`) is cleared at the top of `UpdateCamera` (`31d02e50 @31d02e75`; this corrects the C3v row's `31d02c70` label) and set by a hidden mesh (`@327f921b`). |
| `JUNE_RENDER_WORLD_MESH_DEBUG` | Short-lived. One `[JUNE_ENGINE] WMESH ... render on` line, then per mesh its first `hide:` and first `show:` line. |

**Routed:**
- The flag's only reader is the user unit (`31ff03d0 @31ff0e4f`, user `+0x160`) -> **R5** (kickoff amended).
- `CX2WorldObjectSkinMesh::OnFrameRender_Prepare` `327f6e40` also differs and also sets the flag -> **new R13b**.

**Hygiene** (`flagcheck --worktree --files` the 5 code files): RESULT PASS for all three flags.
- `X2WorldObjectMesh.cpp` and `KTDGCamera.cpp` are PASS; the two headers are PASS~ (one blank line each).
- The other rows are OTHER, from the sibling R13 flags or `Always.h`.
- 0 deleted lines, 0 non-ASCII lines, 0 EOL mismatches.
- Range check after the commit: `--range 3312700d^..3312700d`, RESULT PASS for all three flags (0 FAIL / ERROR, 0 deleted lines).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,816,448 B, dated 2026-10-17 22:29:20, md5 f0f46ed7dd266cfe32e847564cdc31a0` (built at main 3f478e9d + peers' uncommitted files; both WMESH strings found in the deployed exe).

**How to see it in game** (Freils, Chung Tactical Trooper, uid 18):
1. Elysion village: walk so that one of the four `Elysion_villeage_canhide*.Y` buildings is between the camera and Freils. It fades to about 30%,
   and Freils stays visible through it; walk out and it fades back. March never faded these in a village.
2. ErrorLog.txt has one `[JUNE_ENGINE] WMESH June world-mesh render on` line. Each building that hides also writes one `WMESH hide:`
   line (`layer N -> 255`) and one `WMESH show:` line (`layer restored to N`).
3. Any dungeon or field: no world mesh vanishes up close or at the screen edge. Whole-object culling only drops meshes wholly off screen.
**Play-test 2026-10-18: PASSED** (user: both checks fine; exe md5 `2455a22f`, a later `main` deploy at `2bb92772` that carries R13). ErrorLog: one `WMESH ... render on` line, and hide + restore for all four Elysion meshes (`canhide`, `02`, `04`, `03`; layer 5 -> 255 -> 5). The field visited was F03 (40037, `Field_Elysion_Stage2`, no can-hide mesh), with no pop-in. 0 EXCEPTION; the only UNHANDLED were the 3x `E_DISABLE_HB_CHECK_REQ` baseline.

**Play-test follow-ups (2026-10-18; neither is R13's code):**
- **Elysion Hugo shows no `[Z]` mark: fixed** (flag `SERV_IRUHADEV_JUNE_FIELD_NPC_HEAD_BONE`, `X2TFieldNpc.h`). The in-area / quest marks are
  anchored on `GetHeadBonePos`, and June's (`325d1ac0`) picks the bone per NPC: billboard `NPC_Pivit` + 300, Yuno (`Motion_NPC_NONO.X`)
  `NPC_Yuno_GearAxisroot`, Hugo (house 40134, `Motion_NPC_Elsion_Hugo.x`) `Cylinder01`, Parasite `Parasite_Body`, `NUI_MANA_EATER.x`
  `Bip01_Head`, the MAG_NOM mount `Head`, else `Bip01_HeadNub`. March knew only the billboard and `Bip01_HeadNub`, which Hugo's mesh lacks, so his
  mark sat at the world origin. This corrects Effect C9h-5's "same" verdict for block 41 (`june2017_re/c9h5_fieldnpc_onframemove_diff.md`).
  `DEPLOY OK: X2_offline.exe is 19,810,304 B, dated 2026-10-18 02:39:54, md5 94e0af9e07b7fd621128d5ee1f729e26` (the three June bone / mesh names found in the exe). **Play-test 2026-10-18: PASSED** (user: Hugo's `[Z]` mark shows).
- **Stray tree warp: routed to new W1e.** The log shows pad 512 firing (02:29:05) and then 512 -> 513 (02:29:09), although 512 sits on the
  far-right stub (line 130), reachable only through pad 510, which never fired. Elysion's line map gives line 2 (the back walkway, -2789..-2398)
  `BEFORE_LINE = 130`, a line 11,705 units away. March's `CanDown` switches to `beforeLine` when a landing hits a line's exact start
  (`KTDGLineMap.cpp:1355`, `:1421`), so landing on the line 1 / 2 joint puts the unit on line 130.

### Phase R13 -- Review

**2026-10-18, Opus 5.5. PASS, no transcription error.** The only code change is that `_WORLD_MESH_DEBUG` is off, since the play-test passed.

**Prepare `327f9020`** (capstone, every branch):
- **Distance gates.** `jp` after `fcom 0, +0x1ec` skips when the hide distance is <= 0. `jnp` after `fcompp` goes NORENDER when `+0x148 > hide`.
  The near test (`test ah, 0x41` / `je`) goes NORENDER when `+0x148 < near`. The load gate follows (`+0x198` LOD -> `+0x239` / `+0x234`, else `+0x238` / `+0x230`).
- **Hide test.** It is gated on `+0x2f0 == 1 && +0x2f2 == 1` only (`@327f90b7`, `@327f90c9`). `fDist` is the `fldz` stored at `@327f90d9`, so it starts at 0.
  The call passes DG `+0x1c0` / `+0x1cc`, which are the embedded camera's eye `+0x44` / at `+0x50` (camera = DG `+0x17c`), i.e. `GetCamera().GetEye()` / `GetLookAt()`.
- **Hidden branch.** `+0x324` is saved only when it is -1 (`@327f912e`). `+0x164 = 0xff` is a plain store, as is March's `SetLayer`.
  `0.3f < alpha` -> `alpha -= dt * 4.0`, else `alpha = 0.3`, and NORENDER when `+0x2f1 == 1`.
- **Visible branch** (also reached from the gate). `alpha += dt * 4.0`, capped at `+0x278 * +0x1d4` when `cap < alpha`. The restore of `+0x324` happens only here.
- **Alpha object.** `canHide && hide` sets `+0x144 = 1` and the camera `+0x319` (when DG is non-NULL). Otherwise `alpha >= cap` -> `+0x2a8`, else 1. All of this matches the port line for line.
- **Members.** June's `SetCanHide` thunk `31e4c650` (registered at `31dbcfec`, xorstring `31dbcf6c`) writes `+0x2f0` and `+0x2f2`.
  So `+0x2f2` is `m_bAnyCamera` and `+0x2f1` is `m_bNotRenderWhenHidden`, which matches March's declaration order (`X2WorldObjectMesh.h:109-111`).

**Slot 0x48 is dead (JUNE_BUGS 267 stands).** I searched three ways, not with R13's frustum-adjacency filter:
- **Literal references.** The only 4-byte literal of `327fa210` anywhere in the image is the vtable slot `334bc4cc`, and there is no `E8` / `E9` to it.
- **Vtables.** Every vtable with the base frustum test `31c829e0` at slot 0x34 was listed. The world-object family (`334bc37c`, `334bc3cc`, `334bc4dc`) ends at slot 0x40, and only `334bc484` has slots 0x44 / 0x48.
  So a caller would need a static `CX2WorldObjectMesh*`; March has no such caller.
- **Virtual calls.** All 145 slot-0x48 virtual calls in June's code were checked; none is in the world-object code (`327f6xxx-327fbxxx`) or in `CX2World::` (`327fb2xx-3280xxxx`).
  The 5 sites with a DG / eye-looking operand are `IDirect3DDevice9` slot 18 (`31ce6ae8`, `31ce9f1d`, `31cea468`) or a one-argument call on a state member `+0x488` (`324e35c4`, `3253240c`).

**`UpdateCamera` = `31d02e50`.** It starts after int3 padding at `31d02e4f`, and the `+0x19d = 0` at `@31d02e75` is its first store, before the `+0x90` test.
However, June's `UpdateCamera` also differs from March's in ways R13 left out. These are the `+0x90` / `+0x94` path, the roll `+0x68` tracked by `+0x84`, and the roll multiplied in after `LookAtLH`.
Its 10-argument `SetTracking` `31d01b70` differs too: the C3v review rows 2479-2480 recorded them as "not ported (engine camera, outside C3v)", and no phase owned them.
By the no-partial-updates rule the function has to go to June whole. That takes June's 11 `SetTracking` callers mapped against March's 17 (`X2Camera.cpp`), so it is a phase, not a review fix: **new R13c**.

**Layout.** `KTDGCamera.h` is included only by `KTDGManager.h` (from `KTDX.h:1348`, after `Always.h` at `:190`) and by `KTDGCamera.cpp` after `stdafx.h`, so `CKTDGCamera` has one size in every TU.

**Hygiene** (`flagcheck --range 3312700d^..3312700d`, one run per flag):
- `JUNE_RENDER_WORLD_MESH`: files 1 PASS, 1 PASS~, 3 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL mismatches; RESULT: PASS.
- `JUNE_RENDER_CAMERA_MESH_HIDDEN`: files 1 PASS, 1 PASS~, 3 OTHER, 0 FAIL, 0 ERROR; 0 deleted, 0 non-ASCII, 0 EOL mismatches; RESULT: PASS.
- `JUNE_RENDER_WORLD_MESH_DEBUG`: files 0 PASS, 5 OTHER, 0 FAIL, 0 ERROR; RESULT: PASS.
- The OTHER rows are `Always.h` plus the sibling R13 flag's files; `_DEBUG` lives wholly inside `WORLD_MESH` blocks, so all of its rows are OTHER.

**Review change:** `//#define SERV_IRUHADEV_JUNE_RENDER_WORLD_MESH_DEBUG` (`Always.h`). Its code stays, for R13b / R13c to re-enable if needed.
Range check after the commit (`--range a8e59e12^..a8e59e12`, `_WORLD_MESH_DEBUG`): files 1 DEFINE (+1 -1, the `#define` only), 0 FAIL row, 0 ERROR; RESULT: FAIL only because the flag is now defined nowhere and its old `#define` line counts as deleted. R12's review `918a4ef9` gets the same verdict for `_REFS_DEBUG`, so this is expected.
`DEPLOY OK: X2_offline.exe is 19,872,256 B, dated 2026-10-18 03:21:50, md5 ce79dccff14191fdfb02268e1cd90a81 (built at main 60eac05c + peers' uncommitted files; the WMESH string is absent from the exe)`

**Kickoffs:**
- R13b is still right after R13; it gained one line: name the skin mesh's `+0x2f1` from its own `SetCanHide` binder (the mesh's writes `+0x2f0` / `+0x2f2`).
- R5 still owns the flag's reader.
- New `R13c.md` / `R13c-review.md`.

## Phase R13b - World skin-mesh render (Prepare)

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_WORLD_SKIN_MESH`.
Added by R13 (an unowned June branch). After R13; not alongside R12 if R12's map reaches `X2WorldObjectSkinMesh.*`.
- **`CX2WorldObjectSkinMesh::OnFrameRender_Prepare` `327f6e40`** differs from March:
  - an anim visibility gate (`+0x354` / `+0x355`, the `+0x228..+0x22c` list, anim `+0x173 = 0`);
  - the hide test on `+0x2f0 && +0x2f1` through vslot 0x40, with the eye and lookAt passed by value;
  - the 0.3 / 4.0 fade and camera `+0x19d = 1` (`@327f7098`; R13's `JUNE_RENDER_CAMERA_MESH_HIDDEN` setter); no layer swap.
- Map the class's vtable whole (as R13 did for `334bc484`), then port what differs. Funcmap row `327f6e40 (R13)`.

### Phase R13b -- Done

**2026-10-18, Opus 5.5.** The skin mesh's vtable `334bc3cc` (RTTI `.?AVCX2WorldObjectSkinMesh@@`, 17 slots) is mapped whole, in 6 funcmap rows tagged `(R13b)`.
Its own slots are Prepare, RayHit, OnFrameMove, Draw, Reset / Lost and the dtor; the other 9 are the base's or folded with the mesh's.

**The map.**
- **`SetCanHide`** is thunk `31e4c6a0` (binder `31d95080`, name `@31dbea96` in the skin-mesh block). It writes `+0x2f0` = `m_bCanHide` and `+0x2f1` =
  `m_bAnyCamera`, as March's header. The class has no `m_bNotRenderWhenHidden`, so the R13 review's doubt is settled: June's gate is `canHide && anyCamera`.
- **Prepare `327f6e40` differs in two places only:** the hide test drops March's `g_pX2Game` / `CS_NORMAL` test and needs `m_bAnyCamera`, and a hidden
  skin mesh sets the camera's world-mesh-hidden flag (`@327f7098`). Gates, fade (0.3f / 4.0), render-param copy and alpha-object rules are March's. No layer swap.
- **RayHit `327f6a70` differs:** it inlines June's `LineToSphere`, the change Effect C8a found in the damage manager. The point tested is the closest point on
  the eye-lookAt segment, clamped to its ends; a hit needs `dist < radius + 1` (strict); the impact is halfway from the centre. March projects onto the
  unclamped ray. Only this class's RayHit is switched; `KTDXCollision.h` stays March's (C8a's scope).
- **The rest is March's**: OnFrameMove `327f83a0` branch for branch (play-type jump table `327f8cf8`, `Play` values = `XAP_*`, IsHost culling), Draw,
  Reset / Lost, ctor / dtor. The only June-only member store is `+0x368 = -1` (`m_iTriggerID`, already ported by W0). OnFrameMove's `AddModelXSkinMesh`
  passes June's `__FILEW__` / 151; R12 added those parameters for the KEM 433 log text and no March caller passes them, so this caller doesn't either.

| Flag | What landed (June) |
|---|---|
| `JUNE_RENDER_WORLD_SKIN_MESH` | Prepare's hide test as `327f6e40`: only `m_bAnyCamera` skin meshes hide, in villages too; a `SetCanHide(true, false)` skin mesh never hides (261 such in June's world Lua, 18 are any-camera). A hidden one sets `SetWorldMeshHidden( true )` (R13's setter; `#error` without `JUNE_RENDER_CAMERA_MESH_HIDDEN`). |
| `JUNE_RENDER_WORLD_SKIN_MESH_RAYHIT` | RayHit as `327f6a70` (clamped segment, strict `< r + 1`, halfway impact, `-1` miss). |
| `JUNE_RENDER_WORLD_SKIN_MESH_DEBUG` | Short-lived. One `[JUNE_ENGINE] WSKIN June world skin-mesh render on` line, then each skin mesh's first `WSKIN hide:` line (name, alpha). |

**Hygiene** (`flagcheck --worktree --files` the 3 code files): RESULT PASS for all three flags.
- The skin-mesh `.cpp` rows are OTHER (the three flags nest); the header is PASS for `_DEBUG`.
- `Always.h`'s `-1` is a peer's deleted blank line (W1e block), not this phase's.
- 0 deleted lines, 0 non-ASCII lines, 0 EOL mismatches, 0 encoding changes.
- Range check after the commit: `--range 29e57897^..29e57897`, RESULT PASS for all three flags (0 FAIL / ERROR, 0 deleted lines).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,873,280 B, dated 2026-10-18 06:34:55, md5 6e1b71e8bdb7cbe25ec57d44d4c11aa3` (built at main 68e3302f + peers' uncommitted files; both WSKIN strings found in the deployed exe).

**How to see it in game** (Freils, Chung Tactical Trooper, uid 18):
1. Besma Village: the horse (`BESMA_VILLAGE_HORSE.X`, world position 1300, 0, 1000) is a `SetCanHide(true, true)` skin mesh. Walk so it is between
   the camera and Freils: it fades to about 30% and back once you walk out. March never faded a skin mesh in a village.
2. ErrorLog.txt: one `[JUNE_ENGINE] WSKIN June world skin-mesh render on` line, and `WSKIN hide: BESMA_VILLAGE_HORSE.X alpha ...` when it first fades.
3. Any dungeon: animated world objects play, show and cull as before.

**Play-test 2026-10-18.** Run 1 (exe 6e1b71e8): the render-on line, no hide: the deployed `Besma_Village.lua` is March's (horse at 0,0,0,
`SetCanHide(false, false)`), so check 1 had nothing to fade and moves to new **W3** (June village world Lua, user's call). Run 2 **PASSED** (exe
0818bfeb, a later `main` deploy at 74dd25e6 that carries R13b): `WSKIN hide: ASHS_VILLAGE_S2_CAIN.X alpha 1` at 07:45:19 in Ash Covered Village
stage 2 (June script, any-camera skin mesh); 0 EXCEPTION, the only UNHANDLED the 2x `E_DISABLE_HB_CHECK_REQ` baseline.

### Phase R13b -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change.** Every item was re-read in `funcdis.py` disassembly (Ghidra has carved none of the three).
Commit `29e57897`; `_WORLD_SKIN_MESH_DEBUG` is now off.

- **Prepare `327f6e40`: matches.**
  - The hide gate is `cmp [+0x2f0], 1` and `cmp [+0x2f1], 1` only (`@327f6f1d`, `@327f6f29`), with no game or camera-state test.
  - Thunk `31e4c6a0` stores arg 1 at `+0x2f0` and arg 2 at `+0x2f1` (`ret 8`), so `+0x2f1` is `m_bAnyCamera`.
  - RayHit is `call [vtbl+0x40]` with a NULL hit distance (`push ebp`, ebp = 0); lookAt (DG `+0x1cc`) is pushed first and eye (DG `+0x1c0`) last.
  - Hit: `fld 0.3f; fcom alpha; test ah,5; jp` subtracts only when `0.3f < alpha` (NaN or `<=` stores 0.3). The `DE E9` `fsubp` gives `alpha - elapsed * 4.0`.
  - No hit: `alpha + elapsed * 4.0` is stored, then capped when `cap < alpha` (`test ah,5`).
  - The camera write `@327f7098` is only in the `+0x2f0 == 1 && hide` branch, after a NULL test of DG (`g_pKTDXApp +0x1fc`).
  - Otherwise `test ah,0x41; jp` after `fld alpha; fld cap; fcompp` restores `+0x2a8` when `alpha >= cap`; when `alpha < cap` or NaN it forces alpha.
  - The constants are `3345993c` = 0.3f and `33437838` = 4.0 (double). `elapsed * 4` is exact, and a float sum or difference rounded through a double
    intermediate gives the float March's `4.0f` does, so the claim holds.
- **RayHit `327f6a70`: matches.** The x87 stack was re-derived from the raw bytes, not the mnemonics.
  - `DE EA` (`st2 = st2 - st0`) gives `d = end - start`.
  - `D8 E3`, `DE E2` and `D8 6C` all give `toCentre = centre - start`.
  - The projection `<= 0` (or NaN) keeps the start (`test ah,0x41; jne`).
  - `|d|^2 > proj` interpolates (`DE F9`: `t = proj / |d|^2`); `<=` takes the end. The `test ah,0x41; jp` parity sense was checked on all four outcomes.
  - `fcompp` with `r + 1` (`334355b0` = 1.0 double) on top misses on `r + 1 <= dist`, so a hit is strict `dist < r + 1`. NaN misses.
  - The impact is `centre + normalize( closest - centre ) * ( dist * 0.5 )`, `*pHit = |impact - start|`, and a miss writes -1 (`334380f0`).
  - **No other reader.** A linear scan of `.text` found 195 slot-0x40 virtual calls. Only one passes two by-value vec3s: Prepare's (`327f6f87`).
    No June caller reads the skin mesh's hit distance.
- **OnFrameMove `327f83a0` "= March": confirmed by spot-check.**
  - The jump table `327f8cf8` (index `+0x314 - 2`, `ja` above 3) is LOOP `327f896f`, PINGPONG `327f8a1b`, OCC_ONE `327f8b07` and OCC_ONE_WAIT `327f8bef`.
    That is `WOSMPY_LOOP..WOSMPY_OCCASIONAL_ONE_WAIT` (2..5).
  - The `Play` arguments are `push 0` = XAP_LOOP, `push ebx` (ebx = 1) = XAP_ONE and `push 2` = XAP_ONE_WAIT, as March's `:136-266`.
  - The states tested are 2 = XAS_WAIT and 0 = XAS_STOP.
  - Culling runs when `g_pX2Game` (`3372c0fc`, the guarded pointer) is set and not (`+0x2c0` != 0 && `+0x2c0` == `+0x2b8`). That is March's
    `IsHost()` (`X2Game.h:476`), which is symmetric in the two uids. Host: anim `+0x174 = 0`.
- **`AddModelXSkinMesh` `__FILEW__` / 151 not passed: agreed, no route.** They only label the KEM 433 log text (R12's parameters). Behaviour is the same.

**Hygiene** (`flagcheck --range 29e57897^..29e57897`):

| Flag | `files:` | `in this flag's files:` | RESULT |
|---|---|---|---|
| `_WORLD_SKIN_MESH` | 0 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR | 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 bare `#else`/`#endif`, 0 encoding | PASS |
| `_RAYHIT` | 0 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR | (same) | PASS |
| `_DEBUG` | 1 PASS, 0 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR | (same) | PASS |

Every OTHER row comes from the three flags sharing one commit:
- `Always.h` +33 holds all three define blocks.
- In the skin-mesh `.cpp` (+90), the `_DEBUG` and `_RAYHIT` blocks nest in or sit beside `_WORLD_SKIN_MESH`.
- The header (+3) holds only the `_DEBUG` member.

There are no deleted lines.

**Debug retired:** `//#define SERV_IRUHADEV_JUNE_RENDER_WORLD_SKIN_MESH_DEBUG` (`bytepatch`, one line; CRLF and the high-byte count unchanged).
**Deploy pending.** The review build (08:09) stopped in X2 on a peer's uncommitted `Always.h` hunk: `#error SERV_IRUHADEV_JUNE_SKILL_NOTE needs
REFORM_SKILL_NOTE_UI`, which is skill-system work. Nothing was deployed. The change only drops log lines, so the next `main` deploy carries it.
The live exe still writes the WSKIN lines; that is harmless.

## Phase R13c - Camera tracking and roll (UpdateCamera whole)

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK`.
Added by the R13 review, because R13 edited `CKTDGCamera::UpdateCamera` (`31d02e50`) but ported only the mesh-hidden reset. Not alongside a session editing `X2Camera.*` or `KTDGCamera.*`.
What June changed in the engine camera, from the C3v review rows 2479-2480 and the R13 row for `31d02e50`:
- **`UpdateCamera` `31d02e50..31d0321b`.**
  - A bool `+0x90` (written by `SetTracking`'s 9th argument) takes a June-only path that uses `+0x94` (`@31d02e6d`).
  - A view roll `+0x68` is tracked by the rate `+0x84 * dt` (`@31d02fbc`).
  - After `D3DXMatrixLookAtLH` (`@31d030f9`), when `|+0x68| > FLT_EPSILON`, a rotation of `+0x68` is multiplied in (thunks `33086abc`, `33086a26`).
- **`SetTracking` `31d01b70`** takes 10 arguments (`ret 0x28`).
  - It adds a 7th channel: `+0x84 = ( roll target - +0x68 ) / Time`.
  - The 9th argument goes to `+0x90`.
  - The threshold is `Time > 0.001` (`0x33431564`), against March's `> 0`, and `m_fTrackTime` is set to 0 when it fails.
  - When `g_pKTDXApp +0x2d4 == 1`, `at.x` / `at.y` are copied into `to.x` / `to.y`.
- **The point-like setter `31d01640`** also applies the roll after its `LookAtLH` (`@31d016c5`). `Init` `31d01460` sets `+0x68 = 0`.
- **Map `CKTDGCamera` whole** against March's `KTDGCamera.h/.cpp`: the ctor `31d03220`, the setters, and the shake / impact helpers `31d01d50` / `31d02c70`. Port what differs.
- **Callers.** June has 11 direct calls to `31d01b70`; March has 17 `SetTracking` calls in `X2Camera.cpp`. Map each June caller's roll and `+0x90` arguments onto March's sites, and give the March-only sites June's defaults.
  Some of those callers were moved by C3v (`NomalTrackingCamera` `31fb3640`, 10 args); keep C3v's flags as they are.

### Phase R13c -- Done

**2026-10-18, Opus 5.5 high.** `CKTDGCamera` mapped whole (`31d01310..31d03570`, vtable `3344d770`), ported where it differs; commit in the STATUS row.
Flags `SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK` (+ short-lived `_DEBUG`) and, split off as its own June change, `SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK`.
Funcmap rows `(R13c)` (10); caller table `june2017_re/r13c_settracking_callers.tsv`.

**Layout.** June inserts the roll at `+0x68` (inside `CameraData`: PopCamera `31d03430` copies 0x1a dwords), so `m_Track` / `m_TrackAt` move to
`+0x6c` / `+0x78`; then roll rate `+0x84`, track times `+0x88` / `+0x8c`, linear byte `+0x90`, linear step time `+0x94`; the shake fields follow at
March's order (`+0x98..`), impact `+0xf0..+0x10c`, `m_bFixedUpVec` `+0x110`, stack `+0x114`, random shake `+0x12c`, up-side-down `+0x198`,
shake lock `+0x19c`, R13's mesh-hidden `+0x19d`.

**Ported (`_CAMERA_TRACK`):**
- `SetTracking` `31d01b70` (10 args): app `+0x2d4` copies `at.x / at.y` into `to.x / to.y`; `Time > 0.001f`; roll rate; `m_fTrackTime = 0` on the
  failing branch (where the raw roll target goes into the rate); `+0x90 = bLinear`.
- `UpdateCamera` `31d02e50`: step time = `+0x94` (then 0) when `+0x90`, else `dt`; roll `+= rate * step` inside the tracking block; shake / impact /
  up-side-down keep the real `dt`; after `LookAtLH`, `view = view * RotationZ( roll )` when `|roll| > FLT_EPSILON`.
  Thunk `33086abc` is `D3DXMatrixRotationZ` (bound import, resolved against the June dir's `d3dx9_43.dll` exports).
- `Point` (roll 10th), `UpdateViewTransform` (roll 3rd, rate reset), June-only `PointAndUpdateViewTransform` `31d01640`, ctor, `CameraData::m_fRoll`.
- `CKTDXApp::SetLookAtDependOnEye` (`+0x2d4`, ctor 0, Lua bind after `GetVec3`, as June `31c154dc`).
- `CX2Camera::OnFrameMove` `31fb5800`: returns before anything (UpdateCamera included) while linear tracking has no step time pending.

**Ported (`_CAMERA_SHAKE_LOCK`):** `+0x19c` at the top of the eight shake setters (`31d01490..31d015d0`; `UpDownCrashCamera` / `LeftRightCrashCamera`
reach it through `ShakeUpDown` / `ShakeLeftRight`). ShakeRandom, ImpactForward and UpSideDown have none. The lock stays clear until R13d ports its writers.

**Same as March:** Move / MoveRel / LookAt / LookAtRel / UpVec / Rotate X-Y-Z / GetCameraDistance / Push / Pop (apart from the struct), Reset `31d01460`
(no callers; it was the C3v review's "Init"), and `UpdateCameraShake` / `ShakeRandom` / `UpdateCameraImpact` by member and call census (not line-diffed).

**Callers.** June's 11 `SetTracking` sites map onto March's 14 live calls (FixedCamera's four are one in June); all pass `( 0, 0.f )` or pass their own
caller's values through. March keeps its `CX2Camera` signatures (C3v's call) and passes June's defaults. There are no March-only sites. `UpdateViewTransform`'s two callers pass roll 0.

**Routed:**
- Ain (checklist **7af**): `LINEARMOVING = TRUE` exists only in `Ain_Apostasia.lua` / `Ain_Erbluhen_Emotion.lua`, read by the Lua camera table `31fb5920`
  into June's 12-arg `PartsLookTrackingCamera`; the user SUB_CAMERA time sequencer `31ff5440` sets `+0x94` (`31fae7b0`). `TILTANGLE` stays dormant.
- New **R13d** (`kickoffs/june-engine/R13d.md`): the shake-lock writers (`CX2CashShop::SetOpen` `@3223e0a5` / `@3223e144`, game-camera creation
  `@322dfedb`), the `PointAndUpdateViewTransform` callers (`324baa7c`, `328c43bc`, `328c4466`), and the two non-user sequencers `3276c6f0` / `32d2e520`.
- `SetLookAtDependOnEye(true)`'s only June user is `Raid_Red_Dragon_Phase2.lua`. Its resets are in `CX2NewBossRaidManager` and come with the raid.

**Hygiene** (`flagcheck --worktree --files` the 6 files):
- `_CAMERA_TRACK` (with `_DEBUG`): 2 PASS, 1 PASS~, 3 OTHER (Always.h + the two camera files, which share the second flag), 0 FAIL; 0 deleted; RESULT: PASS.
- `_CAMERA_SHAKE_LOCK`: 6 OTHER, 0 FAIL; 0 deleted; RESULT: PASS.
- `_CAMERA_TRACK` with both others: 3 PASS, 2 PASS~, 1 OTHER (Always.h), 0 FAIL; 0 deleted, 0 non-ASCII; RESULT: PASS. Every code file reduces to March.

**Deploy:** no R13c build of its own landed (queued builds stopped at peers' uncommitted `Always.h` `#error`s and a `KTDGFontManager.cpp` signature change). It shipped in a later `main` deploy: `X2_offline.exe` 19,973,120 B, 2026-10-18 09:18:28, md5 `d4eef83688b181e6dbdb7d59656cc4a8` = build.py deploy of `eb5280c7` (+52 peer files), which contains `87c5aa0e`; the `[JUNE_ENGINE] CAMTRACK` and `SetLookAtDependOnEye` strings are in the exe.

**Play-test 2026-10-18 (log):** run started 09:50:43 on that exe. After the console line `g_pKTDXApp:SetLookAtDependOnEye( true )` in a dungeon, ErrorLog has 48 `CAMTRACK SetTracking: linear 0, roll target 0, roll now 0, time 0.3, look-at-depends-on-eye 1` lines (09:52:14-09:52:41: 20 at once, then one a second, the throttle), none after `false`. Roll and linear stay 0 as the caller table says. 0 EXCEPTION; UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ` baseline. **User: "exactly right"**: with the switch on the camera dropped to the character's height and looked straight on, and it returned to normal after `false`. **Play-test PASSED.**

**How to see it in game:** nothing in March content sets a roll, the linear byte or the lock, so camera behaviour should be unchanged. The one live
switch is the console line `g_pKTDXApp:SetLookAtDependOnEye( true )`: every `SetTracking` then aims the eye straight at the look-at point (no side angle),
and logs `[JUNE_ENGINE] CAMTRACK SetTracking ... look-at-depends-on-eye 1`. June has no caller that passes a non-zero roll or `+0x90 = 1` outside Ain's
skill cameras (checklist 7af), so no dungeon boss intro reaches either path in this build.

### Phase R13c -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no transcription error, no code change except retiring `_DEBUG`.** Reviewed `87c5aa0e` (the only R13c code commit;
`463a91fa` / `fb5337a8` are notes) against June in Ghidra (`31d01640`, `31d01a30`, `31d01b70`, `31d02e50`) and raw capstone (`june2017_re/disasm_range.py`
over `31d01310..31d03570` and `31fb5800..31fb5915`, where Ghidra has no function).

- **`SetTracking` `31d01b70`:** args `[esp+4..0x28]` = `XTo YTo ZTo XAt YAt ZAt Time atTime bLinear fRollTo` (`ret 0x28`). App `+0x2d4 == 1` stores `XAt` / `YAt`
  into the `XTo` / `YTo` slots before the branch. `fld 0.001; fcomp Time; test ah,5; jp else` = `Time > 0.001`, NaN to the else. Rates: `+0x6c..+0x74` eye,
  `+0x78..+0x80` look-at, `fdivrp` gives `+0x84 = ( fRollTo - +0x68 ) / Time`. Else: raw values, `+0x84 = fRollTo`, track time 0 (`@31d01c3a`). Then `+0x88 = track time`,
  `+0x90 = bLinear`, `+0x8c = atTime`. All = port.
- **`UpdateCamera` `31d02e50`:** the x87 stack was traced through each branch. `+0x90 == 1` loads `+0x94` into the step local and stores 0 back. Both tracking branches multiply by the step.
  The time step clamps `<= 0` to 0. Roll `+0x68 += +0x84 * step` at the join `31d02fbc` (reached from both branches; the `TrackTime == 0` jump goes past it to
  `31d02fca`). The `TrackAt` block uses the step. `UpdateCameraShake` / `UpdateCameraImpact` / up-side-down use the real `dt` (`[esp+0x6c]`). After `LookAtLH`,
  `|roll - 0.0| > FLT_EPSILON` (`test ah,0x41; jne skip`) -> `RotationZ( &tmp, roll )`, then `Multiply( view, view, &tmp )` (push order `tmp, view, view`): **view x roll,
  roll on the right** = port. `IsSamef` is `fabs > FLT_EPSILON` -> false, the same sense.
- **`31d01640` / `31d01a30`:** argument offsets traced; the 10th / 3rd arg goes to `+0x68`. `31d01640` zeroes `+0x88` / `+0x8c` only. `31d01a30` copies eye / look-at into
  `m_Track` / `m_TrackAt`, zeroes `+0x84` / `+0x88` / `+0x8c`, and applies the same roll tail. = port.
- **`OnFrameMove` `31fb5800`:** `[esi+0x38]` = March's `CKTDGCamera& m_kCamera`; `+0x90 != 0` and `|+0x94| <= FLT_EPSILON` (also NaN) -> `xor eax,eax; ret 0xc`
  (S_OK, nothing else runs) = port.
- **Shake lock:** `cmp byte [ecx+0x19c],1` opens exactly `31d01490 / 4c0 / 4f0 / 520 / 540 / 570 / 5a0 / 5d0`, plus the inlined `31d01cd0` / `31d01d10`.
  ImpactForward `31d015f0`, UpSideDown `31d01630` and ShakeRandom `31d02b00` have none. The only other `+0x19c` access in the camera range is the ctor's
  word clear `@31d03383`. The census of new members in the range finds only Point, Reset, `31d01640`, `31d01a30`, SetTracking, UpdateCamera and the ctor, whose memset of 0x68 bytes at `+4` covers the roll.
- **Callers:** Ghidra lists exactly the table's 11 xrefs to `31d01b70`. Spot-check `31fb3623`: `push 0` (linear), and roll = the same x87 value as `atTime`
  (0.0, as the table says), Time `0x3345993c`. March has 14 live `SetTracking` calls (17 lines, 3 commented) and none outside the camera files.
- **Layout:** `KTDGCamera.h` comes only via `KTDGManager.h` (`KTDX.h:1351`), `KTDXApp.h` at `KTDX.h:1393`, and `Always.h` at `:190`; `KTDGCamera.cpp` opens with
  `stdafx.h`. So every TU sees one size of `CameraData` / `CKTDGCamera` / `CKTDXApp`.
- **Not line-diffed (as the phase said):** `31d01d50` / `31d02c70` were read here only for new members (none).

**Hygiene** (`flagcheck --range 87c5aa0e^..87c5aa0e`; `git log -S` finds no later commit touching the flags):
- `_CAMERA_TRACK --with _DEBUG`: files: 2 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR; 0 deleted line(s), 0 non-ASCII; RESULT: PASS.
- `_CAMERA_SHAKE_LOCK`: files: 0 PASS, 0 PASS~, 0 DEFINE, 6 OTHER (other flags), 0 FAIL, 0 ERROR; 0 deleted line(s); RESULT: PASS.
- With both: files: 3 PASS, 2 PASS~, 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR; 0 deleted line(s); RESULT: PASS.
- OTHER rows: `Always.h` is always OTHER (other flags' defines). `KTDGCamera.cpp/.h` share both flags, and the combined run passes them. The three files without a shake-lock line are
  OTHER for that flag only. The PASS~ rows are blank lines next to the blocks. No deleted lines.

**Review change:** `//#define SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG` (`Always.h`; play-test passed). Its code stays, so R13d can re-enable it to log its callers.
**Deploy:** pending. The review build (`.build/logs/20261018-101214`) stopped in X2Lib at a peer's uncommitted S5 skill-slot edits (61 errors in
`X2Game.cpp` / `X2KeyPad.cpp` / `X2Unit.cpp` / `X2GameUnit.h`, none in R13c's files), so nothing was deployed. The change only turns logging off, so the next
`main` deploy carries it. Until then the deployed exe (`d4eef836` or later) still writes `CAMTRACK` lines when the console switch is on.

## Phase R13d - Camera members' outside callers (shake lock, Point-and-view)

**Run with:** Opus 5.5 high (map, then port), then a user play-test and an Opus review. Added by R13c, which ported `CKTDGCamera` whole; these are
June's callers of its new members that live outside the camera classes (plan rule 1: the June functions they sit in differ from March's there).
Not alongside a session editing `X2CashShop.cpp`, `X2Game.cpp`, `X2StateField.cpp` or `X2StateBattleField.cpp`.
- **Shake-lock writers** (camera `+0x19c`, `CKTDGCamera::SetShakeLock`): `CX2CashShop::SetOpen` `3223d680` sets it at `@3223e0a5` and `@3223e144`
  (`g_pX2Game->GetX2Camera()->GetCamera()`); the game-camera creation `322dfed2` clears it at `@322dfedb` after `new CX2Camera` (`31fb6e30`). No other
  writer clears it, so June keeps shakes off after the shop opens in a game until the next game (a `JUNE_BUGS.md` row if confirmed).
- **`PointAndUpdateViewTransform` callers** (June `31d01640`): `324ba910` `@324baa7c` (eye `( 0,-5000,-1300 )`, like March `X2StateField.cpp:956`) and
  `328c41f0` `@328c43bc` / `@328c4466`.
- **Camera sequencers** `3276c6f0` `@3276c9b6` and `32d2e520` `@32d2e88f`: the same SUB_CAMERA-by-time shape as the user's `31ff5440` (checklist 7af).
  Name the classes; route class-only ones to the checklist.
- Flags proposed: `SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK_SET`, `SERV_IRUHADEV_JUNE_RENDER_CAMERA_POINT_VIEW`. Log sub-tag `CAMTRACK`.

## Phase U0 - UI framework RE

**Run with:** Opus 5.5 high. No code. Funcmap rows `Engine U0`.

For each U1 gap in the M4 table: June's binding and implementation (address), the C++ member(s) it needs, and its March
equivalent if any. Settle `ROTATION` (live in June's client, or `DLL_BUILD` there too). Settle which nation suffix June's US build
reads for `POS_<CC>` / `FONT_INDEX_<CC>` (US = `NF_US`, memory of C9h-4's nation reader). Re-run `ui_dlg_census.py` against the live
tree and record the gap list. **Done gate:** every gap has an address and a one-line port spec in U1's kickoff, or is routed.

### U0 Done (2026-10-16, Opus 5.5)

No code, so there is no flag, build or flagcheck. 33 funcmap rows `Engine U0` (`june2017_re/funcmap.tsv`). The census re-run
(`ui_dlg_census.py`) wrote a `ui_dlg_census.tsv` identical to HEAD: same 37 june-only-unbound rows, same 13 type-aware misses as M4.
The per-item port spec is in `kickoffs/june-engine/U1.md`, which is the source of truth for U1.

- **ROTATION is live in June's client.** `SetPointData_` (`31c97ce0`) reads it into `UIPointData +0x44`, and both `SetPoint` overloads
  (`31c9a700`, `31c948a0`) rotate about the centre when it is non-zero (`31c9de90` = March `Rotate( center, deg )`). March has the
  member, reader and rotate only under `DLL_BUILD`, which `cl /P` shows is off in US_SERVICE (`RESIZE_TEXTURE` on, so June's layout =
  March's plus `fRotDegree`). U1 lifts only those three pieces, not `DLL_BUILD`.
- **The nation suffix June's US build reads is `US`, and `_GLOBAL` always.**
  - `CKTDXApp +0x2e8` is a string, `"NONE"` from the ctor. The CX2Main ctor sets it once from `m_dwNationFlag`, after the config
    script (June: `Config_US_Service.lua` -> `SetNationFlag(NF_US)` -> `0x400` -> `"US"`).
  - Three helpers read `KEY_GLOBAL` then `KEY_<nation>` after the plain key, with the later read winning: int `31c9e2d0`, vector
    `31c9ec20`, float `31c9e1f0`. Their callers are FONT_INDEX, POS, LEFT_TOP, RIGHT_BOTTOM, RIGHT_TOP, LEFT_BOTTOM, ADD_SIZE_X/Y.
  - `AddDummyInt_Flag` and `SetFont_Flag` take a table and read the `GLOBAL` key, then the nation-name key.
  - The writer/reader set is closed: one writer (`31e4af22`) and five readers. `POS_BR`, `LEFT_TOP_DE` and the other non-US keys
    are read by the same code but never match under US.
- **Gaps that need more than a binding** (members, consumers and defaults are in the funcmap rows):
  - `SetChangeFocusByKeyMsg` gates March's own Tab/arrow chain (`KTDGUIDialog.cpp:670-702`); the default is on.
  - `SetCaretOffSet` shifts the IME caret.
  - `SetGroupOffset` is only read by a dialog method `31cd2af0`. That method's one caller is X2Lib `32dca200`, the synthesis UI,
    which stays with that UI's port.
  - The combo OnItem messages are fired by the hover loop.
  - `SetScrollBarOffsetPos` feeds the scrollbar's `SetOffsetPos` and its mouse coordinates.
  - `LINE_BREAK_*` wraps the string at load and re-wraps it when X2Lib sets the pending byte.
- **Missed by the census:** `CMovieData:AddSubtitleData` is used by three datai015 `Movie_*.lua` scripts, outside the DLG corpus.
  `SetMovieSubtitleIndex` is one piece of a four-part chain: index map -> per-frame subtitle update `31cc07b0` -> `AddSubtitleData`
  -> the movie script loader `31c9b970`, which sets `g_pMovieData`. March's movie code is live (`REFORM_ENTRY_POINT` and
  `MOVIE_TEST_BASE` on, `cl /P`). The three scripts and their `.avi` files are not in March: U1 adopts the scripts, and the movies are
  loose files that june-asset-port P9 copies. June's movie open (`31c9c250`) loads `<movie>.lua` for every movie and only logs
  when the script is missing.
- **A diff of June's KTDX binder against March's `class_def`** found four more June-only UI bindings with no June Lua caller:
  `SetBlockPostEffect`, `SetBlockStringPostEffect`, `SetForceShowDescriptionGuide` and `CPictureData:SetFlicker`. They are not
  ported.
- **Routed:**
  - `GetNationFlag` and `SetChangeUVPoint` (`31c9a7d0`) go to Atlas A4, which also owns the `CPictureData +0xb0` reset that June's
    `SetPoint_LUA` does; this is noted in A4.
  - `AddVillageNpcRelationData*` goes to Elysion E1.
  - `IS_FOR_MINOR_UNIT` (`CX2SlotItem` vslot 11, `32e284b0`) is Lu/Ciel material: checklist 7v.
  - `m_PrivateType` (PvP map list, `3253a900`) is PvP only and not ported, because the offline client has no PvP.
  - The `STR_ID_*` names stay with the Stage 9 sweep (M4).
- **Flag split for U1:** `SERV_IRUHADEV_JUNE_UI_FRAMEWORK`, `_JUNE_UI_NATION_KEYS`, `_JUNE_UI_LINE_BREAK`, `_JUNE_UI_MOVIE_SUBTITLE`.
- **Line-break layering, decided 2026-10-16 (user: follow June).** June keeps one copy of the word-wrap helpers (`31d43ef0` /
  `31d42bc0` = March X2Lib `CWordLineHandler`), and it is in the **engine** (`31d42bc0`-`31d45f20`, between `CKTDXDeviceSound` and
  `CKTDXDevice`). Engine UI code and about 20 X2Lib callers call that one copy. U1 group D moves the two helpers into KTDXLIB, where
  March's versions need only `g_pKTDXApp`, and makes X2Lib forward to them.

## Phase U1 - UI framework port

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_UI_FRAMEWORK` (U0 may split it: e.g. a separate flag
for the nation-suffix readers). Port U0's list. `KTDXApp.cpp` bindings are also touched by Atlas A4 (`GetNationFlag`): never in the
same session. **Done gate:** `ui_dlg_census.py` shows 0 `june-only-unbound` rows apart from the routed ones; a June DLG from each
group opens in game.

### Phase U1 -- Done (2026-10-17, Sonnet 5.5)

Four flags in `KTDXLIB/Always.h` plus the short-lived `SERV_IRUHADEV_JUNE_UI_DEBUG` (`[JUNE_ENGINE] UI` lines in `ErrorLog.txt`). Commit: the one that adds this note. Funcmap: 17 rows `Engine U1`
(`june2017_re/funcmap.tsv`, they correct four U0 rows). JUNE_BUGS.md rows 256-261. `DEPLOY OK: X2_offline.exe is 19,804,672 B, dated 2026-10-17 21:19:47, md5 94b8520e61305072a3134e8d67237848, equal to X2/US_SERVICE/x2.exe (2026-10-17 21:19:31); built at main 129bae3e + 34 uncommitted code file(s).` That exe also holds the R2 session's uncommitted work (it deployed the same md5, `c46a281b`). Strings checked in the deployed exe: all 9 new method names, `g_pMovieData`, `[JUNE_ENGINE] UI NATIONKEY` / `MOVIE`.

**A -- `JUNE_UI_FRAMEWORK`.** All 8 items.
- Bound: `Dialog:SetDisableUnderWindow`, `CPictureData:GetSize` / `SetColor`, `Control:SetChangeFocusByKeyMsg` (default true, gates the Tab / arrow chain in `KTDGUIDialog.cpp` `MsgProc`, June `31cd8a6e`),
  `IMEEditBox:SetCaretOffSet`, `RadioButton:SetGroupOffset`, `ComboBox:SetCustomMsgMouseOverOnItem` / `OutOnItem` / `SetScrollBarOffsetPos`.
- ROTATION: `UIPointData::fRotDegree`, its reader and both `SetPoint` rotates are live outside `DLL_BUILD` (`#ifndef DLL_BUILD` twins, `DLL_BUILD` itself untouched).
- **U0 corrections found while transcribing:** (1) `SetCaretOffSet` shifts the caret and composition rectangle's **top and bottom**, not x, by `ftol( float( ( caretY / n ) * n ) * offset )`
  where `n` = `31d4bc40` = `CKTDGUIUniBuffer::GetLineCount( caret )` (March has it). A zero count faults in June; the port skips the shift. (2) The dialog method `31cd2af0` calls vslot `0x60`, which is
  `SetEnable` (not a check call): ported as `CKTDGUIDialog::SetEnableRadioButtonByGroupOffset( group, offset, bEnable )`, unchecking a disabled radio. Its X2Lib caller `32dca200` (synthesis UI) is **not U1's**.
  (3) The combo hover loop sends Over on every move and Out again after the loop (JUNE_BUGS). (4) `SetScrollBarOffsetPos` must be called before `InitScrollBar` (June's script order); the scroll bar's
  `SetOffsetPos` runs in `InitScrollBar_LUA`.
- Members June never initialises start at March's value: caret offset `0.0f`, radio group offset `0` (JUNE_BUGS).

**B -- `JUNE_UI_NATION_KEYS`.** `CKTDXApp` gets the nation-name string ("NONE", then "US": `CX2Main` ctor, right after the config script, `X2Main.cpp`; nothing loads UI in between, so
this is June's store point; switch keyed on March's `NF_*`, June's name strings). The three readers `GetValueWithNationKey( int / float / D3DXVECTOR2 )` read `KEY_GLOBAL` then `KEY_US` after the plain key at
June's 8 sites (FONT_INDEX, POS, LEFT_TOP, RIGHT_BOTTOM x2, RIGHT_TOP, LEFT_BOTTOM, ADD_SIZE_X/Y). `AddDummyInt_Flag` and `SetFont_Flag` bound.

**C -- `JUNE_UI_MOVIE_SUBTITLE`.** All four parts. `CMovieData` rows + `AddSubtitleData` + `GetSubtitle`; the script `<movie stem>.lua` is loaded with `LoadLuaTinker` after `g_pMovieData` is set
(`KEM_ERROR72` / `73` = June's `ErrorLogMsg` 0x48 / 0x49, no popup); the playing time follows the newest frame (`frame_time` / 1e7, `31c935f0`);
`CKTDGUIStatic::SetMovieSubtitleIndex` + the per-frame update (wrapped to 770 px with June's plain wrap). `Movie_Title`, `Movie_Henir_Start`, `Movie_Elysion_Clear` adopted (ljconv bytecode, `datai015`, packed:
`PACK OK datai015`; **not copied**). Not ported: June's string-ID debug prefix in `AddSubtitleData` (`31c9da10` string-manager flag `+0x40`). The three `.avi` are P9's.

**D -- `JUNE_UI_LINE_BREAK`.** New engine file `KTDXLIB/KTDXWordWrap.{h,cpp}` (added to `KTDXLIB_2010.vcxproj`) holds June's wrap helpers:
- `31cbff90` (plain, via `31d471f0`), `31d43ef0` (colour; **it is March's `GetStrByLineBreakColorInX2Main`**, EU branch), `31d42bc0` (ellipse, = March's), `31d45dd0` (type dispatch), IsSpace / IsSplit.
- `UIStringData` gets `LINE_BREAK_LENGTH` / `LINE_BREAK_TYPE` / `bRewrapPending`; `SetStringData_` wraps after the nation font override; `RewrapPending` is called in the seven
  OnFrameRender sites (button, radio, slot, static, combo x2, list box -- U0 listed five).
- **Visible change:** X2Lib's `GetStrByLineBreakInX2Main` and `GetStrByLineBreakColorInX2Main` now forward to the engine, so every X2Lib caller (tooltips, quest text, ...) wraps at spaces like June's US client
  instead of March's mid-word cut. `BasicLineBreak` has no X2Lib caller and no June twin; the `WithEllipse` family is compiled only under `ELLIPSE_GLOBAL` (off here), so X2Lib keeps its own copy.

**Hygiene** (`flagcheck.py <flag> --worktree --files ...`, before the range run below): all five flags `worktree --files <my 24 files>`: 0 deleted lines, 0 non-ASCII, 0 EOL-mismatch, 0 `#else`/`#endif` without the flag token, 0 encoding changes; per flag the only FAIL is `KTDXWordWrap.cpp`'s `#include "StdAfx.h"` (PCH needs it first, outside the flag).
**Play-test 1 fix (2026-10-17):** the first play-test showed 26k `UI WRAP` lines: `KLOG` is a two-statement macro (`KSuperAssert.h:109`), so `if ( ++n <= 6 ) KLOG(...)` logged every call. All eight throttled debug sites now have braces. `DEPLOY OK: X2_offline.exe is 19,804,672 B, dated 2026-10-17 21:41:43, md5 9e41b0aae9fe81efed31031c1b5bd8ba, equal to X2/US_SERVICE/x2.exe (2026-10-17 21:41:38); built at main aed5707b + 7 uncommitted code file(s).` Log result of that run: `UI NATION name=US`, `UI NATIONKEY hit LEFT_TOP_GLOBAL` / `LEFT_TOP_US`, `UI MOVIE subtitle script NOT loaded` x10 (`KEM_ERROR136` for `MOVIE_CREATE_*.LUA`; `MOVIE_TITLE.LUA` is asked for x3 and is in the packed, uncopied `datai015`), no `lua error`.
**Play-test passed (2026-10-17, user):** character-create movies still play; `datai015` copied (`COPY OK`, md5 91511e57); log after the copy: `UI MOVIE subtitle script loaded, rows 5` (`Movie_Title`), no new `lua error`. The subtitle text itself was not seen (no trigger reached the title movie's video).
**Range check** (`flagcheck.py <flag> --range 915bb24b^..915bb24b`): FRAMEWORK `files: 3 PASS, 18 OTHER, 1 FAIL`; NATION_KEYS `2 PASS, 19 OTHER, 1 FAIL`; MOVIE_SUBTITLE `1 PASS, 20 OTHER, 1 FAIL`; LINE_BREAK `5 PASS, 16 OTHER, 1 FAIL`; DEBUG `0 PASS, 21 OTHER, 1 FAIL`. Every `in this flag's files:` line is `0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change`; each `RESULT: FAIL` is the one row `KTDXLIB/KTDXWordWrap.cpp` (new file, `#include "StdAfx.h"` first for the PCH, the rest inside `SERV_IRUHADEV_JUNE_UI_LINE_BREAK`). OTHER = the same files seen through another U1 flag.
**Census** (`ui_dlg_census.py`, run against the deployed exe): `GAP LIST june-only-unbound: 0 names`, `type-aware misses: 0` (was 37 names / 13 misses). The census learned the suffix rule (`FONT_INDEX|POS|LEFT_TOP|RIGHT_BOTTOM|RIGHT_TOP|LEFT_BOTTOM|ADD_SIZE_X|Y` + `_GLOBAL` / `_US` = bound; other nation codes, `IS_FOR_MINOR_UNIT`, `m_PrivateType` and the `AddVillageNpcRelationData*` table keys = `routed`), edited in `ui_dlg_census.py`.

**Open / for the review.** (a) No live March DLG calls any of the new methods (kom_contents grep, 0 hits): June DLGs bring them with their UI ports, so A can only be shown by log lines and by a regression
that default-on `SetChangeFocusByKeyMsg` changes nothing. (b) Live exercisers: `DLG_Map_Local_*_Icon_NEW` read `*_GLOBAL` / `*_US` keys (B), the movies (C), every X2Lib text wrap (D).
(c) The done gate "a June DLG from each group opens in game" is **not met by live content**: it needs a June DLG calling the new methods, and the owning UI port brings it. The play-test checks the groups that have a live exerciser plus the log lines.

### Phase U1 -- Review (2026-10-17, Opus 5.5)

**Verdict: PASS, no code fix, no rebuild.** Reviewed `915bb24b` + `3d26d68f` (and `bad3ace2`, `c8a50144`, both docs only) against U0, `U1.md` and the 17 `Engine U1` funcmap rows. The live exe (`9e41b0aa`, R2's later redeploy `65ae9c0e`) already holds all of U1.

- **Ghidra spot-checks (3/3 match):**
  - `31cbff90` = `CKTDXWordWrap::GetStrByLineBreak`. It matches statement for statement: the `+3` / `+8` versus `+2` / `+7` skip when `iLastCutPoint == i`, the `"
"` / `"-
"` split on `iNowLineWidth < nowTextLen`, and the break check after an insertion (no check after a token skip).
  - `31c9a310` = `GetSubtitle`. The three-way compare and its result match: `BL` is 0 for no rows (output untouched), 0 with output `""` for a miss, 1 for text. The caller `31cc07b0` wraps with `( text, 0x302, UIStringData +0 = fontIndex, 0 )` only on true, as ported.
  - `31cc1660` = combo `HandleMouse`. The scroll bar gets `pt - ( +0x460, +0x464 )`. The hover loop sends Out (only when the old focus differs, is `>= 0` and the message is set), then Over (always), and Out again after the loop. All are `0x467` = March's `SendInternelEvent( KM_UI_CONTROL_CUSTOM_EVENT )`.
- **Also re-read in June:**
  - Focus gate `31cd8a6e`: `KEYUP`, focus not NULL, `[focus+0x244] == 1`, then Tab / arrows. This is the port's gate.
  - Caret shift `31cb9670`: it is **unconditional**. The too-long `JLE` lands on the shift at `31cb9807`, so the port's spot after the `if` is right, and March's `s_ptCompString` store (`:2766`) picks up the new top, as in June. The `0 < lineCount` guard only skips June's divide-by-zero fault (JUNE_BUGS 257).
  - `AddSubtitleData` `31c9da10`: `-1` gives an empty text, and `STR_ID_EMPTY` is `-1` (`StringID_def1.h:7`).
  - Nation switch: the names match funcmap `31e46300 @31e4ae03` (June's `DEV` has no March `NF_` and is not used under US).
- **Checked in the diff:**
  - Binding names and targets for all 9 June methods.
  - Defaults: focus-by-key true, combo -1 / -1 / 0 / 0, caret 0, group offset 0, line-break 0 / 0 / false, subtitle 0 / 0.
  - The 8 nation-key sites, each placed after its plain read.
  - ROTATION, in `#ifndef DLL_BUILD` twins; `DLL_BUILD` itself is untouched.
  - The seven `RewrapPending` sites. The slot site is in `OnFrameRender`; the hunk header reads `LButtonMouseUp`, which is git's heuristic.
  - `X2Lib/WordWrap.cpp` is insertion only, and both `#else` bodies are March's bytes.
- **Hygiene** (`--range 915bb24b^..915bb24b`; nothing after `3d26d68f` touches these flags, `git log -S` is empty):
  - FRAMEWORK: `files: 3 PASS, 18 OTHER, 1 FAIL`.
  - NATION_KEYS: `2 PASS, 19 OTHER, 1 FAIL`.
  - MOVIE_SUBTITLE: `1 PASS, 20 OTHER, 1 FAIL`.
  - LINE_BREAK: `5 PASS, 16 OTHER, 1 FAIL`.
  - DEBUG: `0 PASS, 21 OTHER, 1 FAIL`. DEBUG `--range 3d26d68f^..3d26d68f`: `files: 6 PASS, 0 OTHER, 0 FAIL`, `RESULT: PASS`.
  - Every `in this flag's files:` line: 0 deleted, 0 non-ASCII, 0 EOL, 0 encoding.
  - The only FAIL row is `KTDXLIB/KTDXWordWrap.cpp` line 1, `#include "StdAfx.h"`. The new file needs the PCH include first, and the rest of the file is inside the flag. It is accepted as U1 said.
  - OTHER = the same U1 files seen through another U1 flag.
  - The DEBUG run counts one `#else` without the token. It is the plain `#else` of the `JUNE_UI_NATION_HIT` macro (`KTDGUIControl.cpp:99`), debug-only, and goes away when `_JUNE_UI_DEBUG` is removed.
- **Done-gate decision (review).** Census `GAP LIST june-only-unbound: 0`, the exe strings and the play-test (B: `LEFT_TOP_GLOBAL` / `_US` hits; C: `Movie_Title` 5 rows; D: every X2Lib wrap) are enough. Group A has no live caller, and its code matches June (spot-check 3 plus the focus gate). A June DLG is **not** adopted just to test it. The UI port that brings `DLG_Mail_Shop_Popup_Send` / `DLG_Magical_Closet_ComboBox` (or the skill-tree UIs S6/S7) exercises A in its own play-test.
- **Bookkeeping clash (not U1's).** R2's later commit `80757b8f` added a second `JUNE_BUGS.md` row **256** (weapon rim ramp) after U1's 256-261. `JUNE_BUGS.md` has a peer's uncommitted edits, so it is not touched here. The R2 review kickoff now says to renumber R2's row.
- **Still open:** `SERV_IRUHADEV_JUNE_UI_DEBUG` is short-lived. Turn it off at plan close-out (Z) or when a UI port no longer needs the lines.

## Phase W0 - World-event timeline RE

**Run with:** Opus 5.5 high. No code. Funcmap rows `Engine W0`.

The loader of world `+0x34`'s tracks (who fills `+0x2a4` / `+0x294`), the Lua keys and the file they come from, the three track
records and every type's action, and the reach: which June worlds carry tracks and which of those March has (worlds that June
dungeon ports will add count; name them). **Done gate:** W1's kickoff written, or W1 marked "not needed" with the data search.

## Phase W0 -- Done

**2026-10-16, Opus 5.5 high, no code / flag / build.** Funcmap: 14 rows `Engine W0` (`june2017_re/funcmap.tsv`, from `32801580 (W0)`),
which correct rows 2123 and 2538-2541. Census: `june2017_re/w0_trigger_census.py` -> `w0_trigger_census.tsv` (47 June linemaps).

- **Not a new system.** The "timeline" is March's `WORLD_TRIGGER` line-map trigger runtime (correction under the ledger paragraph).
  June's tick equals March's `DoTriggerFrameMove`. The mesh and etc calls pass the segment's `TRIGGER_ID` in addition. Activation
  (`SetActiveTrigger_LUA`, `TRIGGER_ID` / `TRIGGER_ON_LINE` lines) is already ported. `327edc70` is the sub-stage `TRIGGER_LIST`
  loader and equals March.
- **Loader `AddTrigger_LUA` `31c77e20`.** Every key is decoded (funcmap row `31c77e20`). It is March's, apart from `AUTO_DISABLE_LINE`,
  line case 8 (`STOP`, `DIRECT_MOVE_DATA`) and etc case 7's `MOTIONNAME_EFFECTSETNAME`. Ranox's `EFFECT_NAME` / `EFFECT_POS_*` are
  not June keys.
- **Handlers.** Mesh 1-3, line 1-6, etc 3/4/5/6/8 equal March (after Lanox P10). The June deltas and their owners:

| Delta (June address) | March today | Owner |
|---|---|---|
| `GetTriggerMesh` `327fc680` accepts only meshes whose `SetTriggerID` (`+0x368`) is < 0 or == the firing trigger's id | name match only; Ranox stores the id, unread | **W1** |
| etc 7 `PLAY_EFFECTSET` `328012b5-32801484`: mesh token + effect-set token, played at the mesh by the first user | Ranox invention: `EFFECT_NAME` at my unit | **W1** |
| etc 1 sound `32800e82-32800edb`: `OpenSound` + `Play` + auto-release on the channel (`31cdd4e0`) | `OpenSound` + `Play`, never released | **W1** |
| `AUTO_DISABLE_LINE` (line tail `328005b9-328007c0`, `31c6b8e0`, unit `+0x3e48`, user and NPC physics) | ignored | **W1b** (Opus: the physics fallback is not fully mapped) |
| line 7 retype to `LT_POTAL` -> `322e5820` PotalGate particles; the same function drives June's `StageLoad` portal markers (`327cd4cf-327cd80c`), plus `SHOW_PORTAL` / `LINE_PORTAL_TYPE` | `PORTAL_LINEMAP_TEST` marker (`X2DungeonGame.cpp:893-919`), keys unread | **W2** (one system: no partial update) |
| line 8 `TAT_LINE_CHANGE_MOVABLE_DATA` | none | **atlas E5** (`SERV_IRUHADEV_JUNE_MOVABLE_LINE`); not with W1 |

- **Reach** (from the census). 97 triggers in 47 June linemaps.
  - All 31 linemaps March has are staged as June bytecode, so the rows above are live and ignored today:
    - `Ranox_FireTemple_StageBoss` has `AUTO_DISABLE_LINE` x2.
    - `Ranox_Geyser_stage0` has `PLAY_EFFECTSET` x2.
    - `Ranox_Secret_Stage2` has an `LT_POTAL` retype x1.
    - Geyser and Sander Secret call `SetTriggerID`.
  - The 16 absent linemaps:
    - **Atlas / Elysion own 14** (A8 AtlasDevelopment, A10 Tower, E8 SkyCrossroads, E9 Adrian, E10 Soless). These phases stage data
      only; this plan owns the engine side, apart from E5's movable lines.
    - **Erda** (`LINE_PORTAL_TYPE` and retypes) belongs to backlog Stage 19.
    - **`HAMEL_GREAT_SEAWAY_PVPMAP`** is PvP, so the offline client cannot reach it.
- **Data present.** `PVE_PotalGate1-5` and `_Close01/02` are in staged data036 `GameMajorParticle36.txt`. `EffectSet_STEELPUMP_Move_FX`
  and `Effectset_ELYSION_ADRIAN_EVENT3` are in staged `EffectSet.lua`.
- **Flags.** `SERV_IRUHADEV_JUNE_WORLD_EVENT` is split into one flag per change (W1: `_JUNE_WORLD_TRIGGER_MESH_ID`, `_EFFECTSET`, `_SOUND`;
  W1b: `_AUTO_DISABLE`; W2: `_JUNE_WORLD_PORTAL_GATE`).
- **Open questions for the user:** none. Atlas line `ATLAS_PORT_PHASES.md:639` ("keep `MOTIONNAME_EFFECTSETNAME` inert") now carries a
  dated note pointing to W1.

## Phase W1 - World-event timeline port

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_WORLD_EVENT`. Conditional on W0. The type-1 sound uses
the device auto-release that june-effect C9h-7 settled.

*Scope set by W0 (2026-10-16):* three flags in place of `_JUNE_WORLD_EVENT`, each transcribed from its funcmap row (`Engine W0`):
1. `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID`: `GetTriggerMesh( name, iTriggerId )` with June's id filter. `DoTriggerFrameMove` passes
   the segment's `m_iTriggerId` to `DoTriggerMesh` / `DoTriggerEtc`.
2. `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_EFFECTSET`: `MOTIONNAME_EFFECTSETNAME` read (etc case 7) and the mesh-anchored play. Ranox's
   `EFFECT_NAME` / `EFFECT_POS_*` read and play stay in `#else`.
3. `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_SOUND`: etc case 1 registers June's auto-release on the device's channel. Needs
   `SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE` (`#error` guard). The FMOD callback in `KTDXDeviceManager.cpp` is a file-static today: expose a
   public `JuneRegisterAutoRelease( FMOD_CHANNEL* )` (June `31cdd4e0`) under the new flag.

### Phase W1 -- Done

**2026-10-17, Sonnet 5.5 high.** Three flags (the planned `_WORLD_EVENT` split by W0), all in `Always.h` with `#error` guards (`_EFFECTSET` needs
`_MESH_ID` + `RANOX_TRIGGER_PLAY_EFFECTSET`; `_MESH_ID` needs `RANOX_TRIGGER_ID`; `_SOUND` needs `JUNE_SOUND_AUTO_RELEASE`). Every change is insert-only, the
original code sits in `#else`. Commit: see `git log -S SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID`.

| Flag | What landed (June address) |
|---|---|
| `JUNE_WORLD_TRIGGER_MESH_ID` | `GetTriggerMesh( name, iTriggerId )` (`327fc680`): an `OT_SKIN_MESH` is a candidate only if `GetTriggerID() < 0` or `== iTriggerId`, then March's upper-case name match. `DoTriggerMesh` / `DoTriggerEtc` take the id and `DoTriggerFrameMove` passes `triggerSeg.m_iTriggerId` (`32801580` @3280169c / @328017bc). Log `[JUNE_ENGINE] WORLD mesh-id reject: trigger T mesh M carries trigger id N`, once per (trigger, mesh id, name), only when the name matched. |
| `JUNE_WORLD_TRIGGER_EFFECTSET` | Etc case 7: loader reads `MOTIONNAME_EFFECTSETNAME` into `m_wstrActionName` (`31c77e20` @31c7b041); `DoTriggerEtc` (`328012b5-32801484`) splits at the first space (none -> nothing), `GetTriggerMesh( token1, id )`, first non-NULL user unit, `PlayEffectSet( token2, user, NULL, false, -1, -1, (1,1,1), true, mesh->GetPos(), user->GetRotateDegree(), (0,0,0) )`. Any missing piece does nothing. Log `[JUNE_ENGINE] WORLD effectset: trigger T played '<value>' at x,y,z`. Ranox `EFFECT_NAME` / `EFFECT_POS_*` read and play are the `#else`. |
| `JUNE_WORLD_TRIGGER_SOUND` | `CKTDXDeviceManager::JuneRegisterAutoRelease( FMOD_CHANNEL* )` (`31cdd4e0`, returns the FMOD result); etc case 1 calls it when the device's channel is not NULL, after `OpenSound` + `Play( false, false )` (as March). `PlaySound` now registers through the same function. |

- **Mesh position:** `CX2WorldObject::GetPos()` = `GetMatrix().GetPos()`, and `CKTDGObject::GetMatrix()` goes through the manager object when set, which is
  June `32802ae0` (object +0x110..0x118 through the manager object when +0xc != 0).
- **June's trailing `1.0f`** in `321e1e60` is `fForceDownValueRate`; March's `PlayEffectSet` has no such parameter (already settled by june-effect), not passed.
- **No new JUNE_BUGS row.** Adrian Stage1's value (no space) plays nothing, which is JUNE_BUGS 244 already.
- **Built with peers' uncommitted edits** (`KTDGXMeshPlayer.*`, `KTDGXSkinAnim.*`, `KTDXApp.*`, `X2DamageEffect.cpp`, `KTDXWordWrap.*`, `Always.h` second hunk): not mine, not committed here.
  `Always.h` is committed as a partial (my hunk only).
- **MODS.md** rows for these flags go in at Phase Z (MODS.md is peer-dirty now).

**Hygiene** (`flagcheck --worktree --files`, per flag): `_MESH_ID` files 1 PASS + 5 OTHER, RESULT PASS; `_EFFECTSET` RESULT PASS; `_SOUND` RESULT PASS; 0 deleted
lines, 0 non-ASCII inserted lines, 0 `#else`/`#endif` without token, 0 encoding change. `_MESH_ID` reports 12 EOL-mismatched inserted lines in `X2World.h`: that file mixes
LF and CRLF and the inserts take their anchors' EOL (anchors are LF), as the house rule says.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,778,560 B, dated 2026-10-17 20:35:54, md5 8ffc762a45b36b586c365c5a119f3ef9, equal to X2/US_SERVICE/x2.exe (2026-10-17 20:35:48); built at main 508fd3cc + 18 uncommitted code file(s).`
No kom changed (data only as already staged). User owes the play-test below.

**Play-test hand-off.** Log tag `JUNE_ENGINE` (`python tools/logs.py --grep "JUNE_ENGINE\] WORLD"` also works). Never launch for me; start `start_offline.bat` yourself.
Character: `reyaa` (Lire, Grand Archer, Lv 85, uid 12). Dev console (END key, auth level 3) is optional here; clear the monsters or walk past them.
1. **Geyser effect set + id filter.** Enter Ranox Geyser Normal (`DI_RANOX_GEYSER_NORMAL` 33290), stage 0 (`Ranox_Geyser_stage0_linemap`). Three trigger ids; the three `Motion_GEYSER_PUMP.x` pumps carry `SetTriggerID` 0, 2 and 3, the `Motion_GEYSER_STEELPUMP.x` mesh carries none.
   - Trigger 1 (the steel pump moving): plays `EffectSet_STEELPUMP_Move_FX` at the steel pump (twice, at 0 s and 0.5 s), then the steel pump jumps to (11450, 510, -500).
     Look for: the effect at the pump (not at your feet), and two `[JUNE_ENGINE] WORLD effectset: trigger 1 played 'Motion_GEYSER_STEELPUMP.x EffectSet_STEELPUMP_Move_FX' at ...` lines in `ErrorLog.txt`.
     If not: no line = trigger 1 never fired or the mesh was not found; send me the `WORLD` lines.
   - Triggers 0 and 2 (`WorkEnd` on `Motion_GEYSER_PUMP.x`): only the pump whose id equals the trigger's id stops. Look for a `mesh-id reject: trigger 0 mesh MOTION_GEYSER_PUMP.X carries trigger id 2` style line (once per pair), and the other pumps keep running.
   Do **not** test case 7 on Adrian Stage1: its value has no space, June plays nothing there and so does the port (JUNE_BUGS 244).
2. **Sound release.** Enter Ranox Fire Temple Normal (`DI_RANOX_FIRETEMPLE_NORMAL` 33310), stage 1 (`Ranox_FireTemple_Stage1_LineMap`, plays `AMB_Earthquake02.ogg` in its `PLAY_SOUND` rows). The sound plays once, then the dungeon carries on without a stall or an error line.
   `FinalHall` has no `PLAY_SOUND` rows, only camera shake, so it is not a sound check.
3. **Regression.** Any older dungeon with mesh triggers (Hamel Frozen Water Hall stage 3 statue, `Trigger_Hammel_Statue.ogg`): the statue still animates, no `mesh-id reject` for it (its meshes carry no id).

**Play-test PASSED (user, 2026-10-17; logs read).** 0 EXCEPTION, UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ` baseline, clean exit. Geyser trigger 1 played the
effect set twice at the steel pump (10940,650,-6303, then 11450,510,-500 after the move), and `mesh-id reject: trigger 2 mesh MOTION_GEYSER_PUMP.X carries trigger id 0`
was logged. Only pumps listed before the accepted one can be rejected, because `GetTriggerMesh` returns at the first accepted mesh.

Next: `/x2-phase june-engine W1 review` on Opus 5.5 / high.

### Phase W1 -- Review

**2026-10-17, Opus 5.5 high. Verdict: PASS, no findings, no code change, no build.** Reviewed `5ab2a08b` (the only commit for the three flags; no later
commit touches `X2World.*`, `KTDGLineMap.cpp` or `KTDXDeviceManager.*`). Ghidra re-read, read-only: `327fc680`, `32800e10` (cases 1 and 7, prologue),
`31cdd4e0`, `32802ae0`, `31c19d90`.

| Item | June | Port | |
|---|---|---|---|
| Id filter | `327fc680`: type (+0x194) == 2 AND ( +0x368 < 0 OR +0x368 == arg 2 ), then the upper-case name compare | `GetTriggerID() >= 0 && != iTriggerId` -> skip, before the name compare; March's unset id is -1 (`X2WorldObjectSkinMesh.cpp:50`) | ok |
| Segment id passed | `32800e5b`: arg 2 (esp+0x2b0) -> esp+0x1c, pushed to `327fc680` @32801352 | `DoTriggerMesh` / `DoTriggerEtc( trigger, triggerSeg.m_iTriggerId )` | ok |
| Case-7 split | find( L" " 3344ff0c, 0 ) @32801314, -1 -> out; token 1 = substr( 0, i ) -> `GetTriggerMesh`; token 2 = substr( i + 1, -1 ) -> effect name | same; checks reordered (no side effects) | ok |
| User | size vslot 0xf0 once, `GetUserUnit` vslot 0xb4 until non-NULL | same loop (size re-read per pass: no behavioural difference) | ok |
| `PlayEffectSet` args @328013d6-32801461 | ( token2, user, 0, 0, -1, -1, scale (1,1,1) by value, 1, &pos, &GetRotateDegree() (vslot 0x150, returns a reference), &(0,0,0), 1.0 ) | same order against March's `X2EffectSet.h:1048`; trailing 1.0 = `fForceDownValueRate`, absent in March (settled by june-effect) | ok |
| Mesh position | `32802ae0`: +0xc == 0 -> own +0x10 matrix, else the root of the +0xc chain (`31c19d90`); reads +0x110..+0x118 | `CX2WorldObject::GetPos()` = `GetMatrix().GetPos()`, and `CKTDGObject::GetMatrix()` recurses through `m_pManagerObject` to the root (`KTDGObject.h:159`) | ok |
| Sound | `OpenSound( name, 10, 0, -1.f )`, `Play( 0, 0 )`, `31cdd4e0( device +0x60 )`; `31cdd4e0` = channel != 0 -> SetCallback( channel, `31cdcca0` ) | March's OpenSound + Play kept; `JuneRegisterAutoRelease( channel )` (NULL -> -1, else the FMOD result); `PlaySound` routes through it | ok |
| Key | `MOTIONNAME_EFFECTSETNAME` (`31c77e20` @31c7b041) | `KTDGLineMap.cpp` case `TAT_ETC_PLAY_EFFECTSET` | ok |
| `#else` | | every hunk insert-only (0 deleted lines); March / Ranox code sits unchanged in `#else`; the reject log is ours (diagnostic, only on a name match) | ok |

**Hygiene** (`flagcheck <flag> --range 5ab2a08b^..5ab2a08b`; a `..HEAD` range times out, and `git log 5ab2a08b..HEAD` on the six files is empty):
- `_MESH_ID`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / `RESULT: PASS`
- `_EFFECTSET`: `files: 1 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / `RESULT: PASS`
- `_SOUND`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR` / `RESULT: PASS`
- OTHER rows: `Always.h` holds the three defines and their `#error` guards; `X2World.cpp` carries all three flags, so each run lists it as the other two's;
  `KTDGLineMap.cpp`, `KTDXDeviceManager.*` and `X2World.h` belong to one flag each. 0 deleted lines, 0 encoding changes. The 12 EOL-mismatched lines in
  `X2World.h` take their LF anchors' ending (that file mixes LF and CRLF), as the Done note says.

**Open questions:** none. Not checked in Ghidra: June's skin-mesh ctor value for +0x368. The play-test settles it: the Geyser steel pump has no
`SetTriggerID` and trigger 1 found it, so the unset value is < 0 in March as June's data needs.

Next: `/x2-phase june-engine W1b` on Opus 5.5 / high (kickoff `kickoffs/june-engine/W1b.md` checked; W1 no longer blocks it).

## Phase W1b - AUTO_DISABLE_LINE

**Run with:** Opus 5.5 high (map, then port), then an Opus review. Flag `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE`. Map first:
the second-candidate selection in `CX2GUUser::PhysicProcess` `@320195be-32019a10` and the NPC fallback `@32782deb-32782ea4`. Then
port, from the funcmap rows: the `LineData` fields (`+0x128` flag, `+0x12c` user UIDs, `+0x13c` NPC UIDs), the parse, the line tail,
`31c6b8e0`, the unit `+0x3e48` bookkeeping and both physics gates. Test case: `Ranox_FireTemple_StageBoss`, lines 3 and 4.

### Phase W1b -- Done

**2026-10-17, Opus 5.5 high.** One flag, `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE` (`Always.h`, `#error` without `WORLD_TRIGGER`). Insert-only: June adds
behaviour March never had, so there is no `#else`. Commit: `git log -S SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE`. Funcmap tag `Engine W1b`
(rows `32015b30 @320195be`, `3277fbf0 @32782d70` raised to `confirmed`; `32063650 @3206bc6c` annotated; new `31c7bb00`, `31c7da50`, `3204ea20`).

**Map (the two W0 candidates).**
- **User** `@32019901-320199dc`: there is no second candidate inside the loop. `@320195d5-320198fa` is `GetNearestLine` inlined (March `X2GUUser.cpp:24205-24208`). The gate runs on both paths, also when the current line was kept. "esp+0x7c / esp+0x98..0xa0" is the frame's **entry** line and land position (`+0x7e54` / `+0x7e20`, read at `32015bb2-32015c1b` before any re-land, never written again). Unlisted unit: `position.x/z = positionOrg`, `position.y = max( y, entryLand.y )`, `landPosition = entryLand`, line = entry line.
- **NPC** `@32782deb-32782ea4`: March's `CanDown == false` branch (`X2GUNPC.cpp:27245-27268`) with the entry land position: line = `m_iPositionBackup_LineIndex` (`+0x6860`), x/z = `positionOrg` (`GetPositionBackup`), `NOT_USE_LAND_CONNECT` height clamp, then the `X2OPTIMIZE_WALL_TRAP_BUG_FIX` speed reset (`+0x85fc` = `m_pInitData`, `+0x843c` = `bFootOnLine`, `+0x308` / `+0x314` = `nowSpeed.y` / `fDownAccel`). It sits at the end of the walking branch; the `AI_FLY` branch jumps past it (`@32781f19-32781f42`).

| Piece | June | March site |
|---|---|---|
| `TRIGGER_DATA_LINE::m_bJuneAutoDisableLine` + `AUTO_DISABLE_LINE` read, every line row | `+0x25`, `31c77e20 @31c790a3` | `KTDGLineMap.h`, `AddTrigger_LUA` after `LINE_ID` |
| `LineData` flag, `vector<__int64>` user / `vector<int>` NPC UIDs; `Init` clears them | `+0x128 / +0x12c / +0x13c`, `31c7da50 @31c7dc57-dcc8` (ctor `31c7e4b0`, dtor `31c7d800` = the vectors' own) | `LineData`, `Init()` |
| `JuneAutoDisableRelease( line, uid, bUser )` | `31c6b8e0`: invalid / unmarked -> true; listed -> erase, true; else false (NPC: low dword) | `KTDGLineMap.cpp` before `CanDown` |
| `DoTriggerLine` tail, after every action | `328005b9-328007c0` | `X2World.cpp`, after the switch |
| `CX2GameUnit::m_iJuneAutoDisableLine` | `+0x3e48`; -1 in user `Init` `3204ea20 @3204ef71` | ctor -1 (stand-in, JUNE_BUGS 263) + `CX2GUUser::Init` after the riding block |
| User bookkeeping | `@3206bba4-3206bd03`, after LAND_CHECK | `CommonFrameMove` after `X2GUUser.cpp:19731` |
| User gate (+ entry snapshot after `positionOrg = position = ...`) | `@32019901-320199dc` | `PhysicProcess` before the two stores at `:24211` |
| NPC gate + bookkeeping (+ entry land snapshot) | `@32782d64-32782ede` | `PhysicProcess` before `X2GUNPC.cpp:27277`'s brace |

- **Deviation, routed:** June's `31c7bb00` (`GetLandPosition`) takes a bool that lets `LT_MONSTER_ROAD` lines count (users false, NPCs true). March's has no
  bool and no skip, so the tail uses March's `GetLandPosition( pos, LINE_RADIUS, &line )`. This matters only for a unit over a monster road. It is a June delta
  to a March function with many callers: new phase **W1c** (`kickoffs/june-engine/W1c.md`, STATUS row).
- **JUNE_BUGS 262-265:** the NPC ignores the release result and keeps `+0x3e48`; the NPC `+0x3e48` is never initialised (port: -1); the user mixes the Now
  land line with the Future index; the lists grow on each fire of a looping trigger.

**Hygiene** (`flagcheck --worktree --files`, 8 files): 7 PASS, 1 DEFINE, 0 FAIL; 0 deleted lines, 0 non-ASCII inserted lines, 0 EOL-mismatched lines,
0 `#else`/`#endif` without token, 0 encoding change. Range `b8653b47^..b8653b47`: `files: 7 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,809,280 B, dated 2026-10-17 22:10:14, md5 acbb882777d398c1676cb09e5335c772, equal to X2/US_SERVICE/x2.exe (2026-10-17 22:10:09); built at main 82dd3ce4 + 12 uncommitted code file(s).`
The 4 code files that are not mine are peer R13's (`KTDGCamera.*`, `X2WorldObjectMesh.*`, its `Always.h` block), built in, not committed here. No kom changed.

**Play-test hand-off.** Log tag `JUNE_ENGINE` (`python tools/logs.py --grep "WORLD auto-disable"`). The line map is June bytecode, already staged.
Ranox Fire Temple Normal (`DI_RANOX_FIRETEMPLE_NORMAL` 33310, level 75+), stage 3 = `WI_RANOX_FIRETEMPLE_STAGEBOSS`. Boss `NUI_FIRE_DEVIL_CHAIN_IPLITAN`:
under 60 % HP it enters `ATTACK_B_EVENT_FIRE` and fires trigger 1. Trigger 1 drops the side platforms (lines 0 and 2), then enables the bungee lines 3 / 4 under
them (785 units lower) with `AUTO_DISABLE_LINE`. Line 1 (centre) is never touched.
1. **Marking.** Stand on the left or right platform when the boss fires. Look for two `[JUNE_ENGINE] WORLD auto-disable: line 3 marked, users listed N, npcs listed M`
   lines (line 3 and line 4). You (and AI-party bots, which are NPCs) standing over a side platform count. You fall onto the bungee line as June does.
2. **Nobody else lands.** Stand on the centre line 1 when it fires (listed 0 on both lines), then walk off its edge towards a side: you cannot drop onto line
   3 / 4. You stay held at the edge of line 1 (the gate keeps you on your entry line).
3. **Release.** As a listed unit, leave line 3 (the bungee teleports you away, or jump off). Look for `[JUNE_ENGINE] WORLD auto-disable release: line 3 user <uid>`
   (and `npc <uid>` for listed bots/monsters). Afterwards you cannot land on line 3 again.
If no `marked` line appears: the boss never reached the FIRE state, or the trigger did not fire; send me the `WORLD` lines.

**Play-test PASSED (user, 2026-10-17; logs read).** Run on a later deploy (`cc6a579d`, md5 ea4a74b4, contains `b8653b47`). 0 EXCEPTION, UNHANDLED = the 2x
`E_DISABLE_HB_CHECK_REQ` baseline. Run 1 (side platform): `line 3 marked, users listed 1, npcs listed 0`, `line 4 marked, users listed 0`, then 3 s later
`release: line 3 user 12`. Run 2 (centre platform): lines 3 and 4 marked with 0 listed, and the user could not drop onto them.

Next: `/x2-phase june-engine W1b review` on Opus 5.5 / high.

### Phase W1b -- Review

**2026-10-17, Opus 5.5 high: PASS, 0 findings.** No code change, no rebuild. Commit reviewed: `b8653b47`. `git log -S` finds no later commit that touches the flag.

Ghidra re-read (disassembly, read-only), all five sites:
- **`31c6b8e0`, the three returns.** An invalid index (`JS` / `JGE`) or an unmarked line (`+0x128 != 1`) returns `AL = 1`. A listed UID is erased (`31c7d1e0` user / `325f3d20` NPC) and returns 1. Anything else returns 0, an empty list included. The user list compares 8 bytes, the NPC list compares the UID's low dword. The port matches.
- **`DoTriggerLine` tail `328005b9-328007c0`.** The order is `+0x25 == 1`, linemap non-NULL, `+0x128 = 1`. Each user / NPC is seeded with vtable `+0x12c( 0 )` (`GetLastTouchLineIndex( false )`) and placed at vtable `+0x138( 0 )` (`GetPos( false )`). `31c7bb00` is called with radius `33441b9c` = 30.0 = `LINE_RADIUS`, its bool is 0 for users and 1 for NPCs (W1c), and the result is compared with `LineData +0`. Users push the 8-byte UID, NPCs the low dword. The port matches. Its extra `g_pX2Game` NULL test and the log line are harmless.
- **User gate `@32019901-320199dc`, entry snapshot `@32015bb2-32015c1b`.** The entry line is `movzx` from `+0x7e54` and the entry land comes from `+0x7e20..28`. A listed unit is matched on the full 64-bit UID. An unlisted unit gets `x / z = positionOrg` (`esp+0x50 / 0x58`), `y = max( y, entryLand.y )`, `landPosition = entryLand` and line = entry. The port matches.
- **NPC gate `@32782d64-32782ede`.** The listed test compares the `int` entry, sign-extended with `CDQ`, against the full UID. The port's `(UidType)` cast is that. An unlisted NPC gets line = `+0x6860`, `x / z = positionOrg`, the `NOT_USE_LAND_CONNECT` (`& 0x20`) clamp against the entry land, then the wall-trap reset (`InitData +1`, `+0x843c`, `speed.y < 0`). After that `+0x3e48 = line` on both paths, and the release (UID 8 bytes, bUser 0) runs with its result ignored. `X2OPTIMIZE_NPC_NONHOST_SIMULATION`, which guards the port's line store, is defined (`KTDX.h:281`), so June's unconditional store is equal.
- **User bookkeeping `@3206bba4-3206bd03`.** It runs only with a world and a linemap, and reads the Now land line (`+0x80f4`). If `+0x3e48 > -1` and differs from the Future line `+0x7e54`, it calls release, and a true result sets `+0x3e48 = -1`. A marked land line then stores `+0x3e48 = +0x7e54`. The port matches, and JUNE_BUGS 264 records the mix.

`LineData` is `new`ed and deleted, never `memset` or `memcpy`d. `Init` clears the flag and both lists, and the vectors' own copy constructor and destructor stand for June's `31c7e4b0` / `31c7d800`. There is no `#else` because the change is insert-only, so March's code is untouched.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE --range b8653b47^..b8653b47`: `files: 7 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`, `RESULT: PASS`. It reports 0 deleted lines, 0 non-ASCII inserted lines, 0 EOL mismatches and 0 encoding changes.

**Owes:** nothing. W2's kickoff now notes the tail's position in `DoTriggerLine`. Next: W2 or W1c (they share `KTDGLineMap.*`, so one at a time).

## Phase W1c - GetLandPosition monster-road bool

**Run with:** Opus 5.5 high (map, then port), then an Opus review. Flag `SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD` (proposed). Added by W1b (an unowned June
branch): June's `CKTDGLineMap::GetLandPosition` `31c7bb00` takes a bool before the radius that lets `LT_MONSTER_ROAD` lines count; March's has neither. Map the
whole function and every caller's bool, then port it whole (rule 1). Not alongside W2 or atlas E5 (`KTDGLineMap.*`).

### Phase W1c -- Done

**2026-10-17, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD` (`Always.h`), plus the short-lived `_DEBUG` (first 12 skipped hits
logged; remove in the review once the play-test passed). Commit: `git log -S SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD`. Funcmap row `31c7bb00`
raised to the full pseudocode (`Engine W1c`). Caller table: `june2017_re/w1c_land_position_callers.tsv` (83 rows).

**Map.** June's `31c7bb00` (`ret 0x1c`) is March's `GetLandPosition` (`KTDGLineMap.cpp:1069`) plus one test: after the wall skip, an `LT_MONSTER_ROAD` (7)
line is skipped unless `bMonster` (`@31c7bbc1`). The rest matches: `LineToLine` is inlined with `eps = fRadius` and the same branches and constants
(`-1.0`, `0.0`, `1.0`, `0.5`, `999999.0` read from the exe); the hit test is `|d| < r` and higher `y`; the two index fallbacks have the same tie rules.
March's existing code therefore behaves as June's `true`.

**Callers (83 June sites: 16 pass `false`, 67 `true`).** Mapped by three Sonnet subagents; the bools were read from the push sequence by script; the
`false` identities were spot-checked (`31fff4a0` = `CX2GUUser::SetPos`).
- `false`, 10 March sites: `CX2GUUser` `InitPosition` (`:2677`), LAND_CHECK (`:19746`), PhysicProcess start / end (`:24143`, `:24156`; one merged June
  call), `SetPos` (`:33529`); `CX2RidingPet` `SetPos` / `InitPos`; `CX2SquareUnit::PhysicProcess` start / end (merged); `CX2World` DoTriggerLine user loop
  (W1b's); `CX2PartyManager::RememberComeBackInfo...`; `CX2StateServerSelect::Handler_EGS_STATE_CHANGE_FIELD_REQ`. `X2StateBeginning.cpp:3923` has the
  same body but no June call site (R26 dead), so it keeps March.
- `true` (March's behaviour, unchanged): 51 March sites (NPC, pet, path finder x30, AI, damage effect, Eve, TField, Lua wrapper).
- No March call (22): Ain / Rose / Luciel, the Lu/Ciel minor body, Add / Elesis additions -> checklist **7y**; the three `CheckPos` Lua bindings
  (already `absent` rows in `c3z4_class_gap.tsv`); a June-only come-back snapshot (packet `0x4f3`, nothing offline); `CX2PETAI::Moving`'s road
  re-seat block -> new **W1d** (`kickoffs/june-engine/W1d.md`, STATUS row).

**Port.** `KTDGLineMap.h`: June's 4-arg declaration; March's 3-arg `int*` overload becomes an inline forwarder that passes `true`; two
bool-taking wrappers for `unsigned char*` / `unsigned short*` (the latter under `JUNE_LINE_INDEX16`). `KTDGLineMap.cpp`: the definition takes the bool,
and the skip goes after the wall test. 10 call sites `#ifdef` / `#else` with the original call kept.

**Hygiene** (`flagcheck --worktree --files`, 9 files): 8 PASS, 1 OTHER (`Always.h`: at the time, the peer's uncommitted R12 line, since committed);
0 deleted lines, 0 non-ASCII inserted, 0 EOL-mismatched, 0 `#else`/`#endif` without token, 0 encoding change. Range: `2d9ad701^..2d9ad701`: `files: 8 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,811,840 B, dated 2026-10-17 23:17:49, md5 2455a22f44376fee754466a090095c80, equal to X2/US_SERVICE/x2.exe (2026-10-17 23:17:44); built at main 2bb92772 + 10 uncommitted code file(s).` (Second build after touching the six X2Lib files: a peer's build had compiled them mid-edit.)
`KTDGEnvBoxMap.cpp` (peer R3/R12, uncommitted) is built in, not committed here. No kom changed.

**Play-test hand-off.** Log tag `JUNE_ENGINE` (`python tools/logs.py --grep "WORLD monster-road"`). Soless Stage 0 (`WI_SOLESS_STAGE0`) has monster
roads at lines 14, 15, 65, 68 (June bytecode, live since E10). Expect `[JUNE_ENGINE] WORLD monster-road skip: line N would land y ...` (at most 12) when
the player is over one. The player never stands on a monster road, and monsters still use them (they walk and land as before).

**Play-test PASSED (user, 2026-10-18; logs read).** Run on the W1c deploy (md5 2455a22f). 0 EXCEPTION; UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ`
baseline; clean exit. Soless (33890): the 12 capped `monster-road skip` lines, all at 02:26:07 in the hidden stage (`Elysion_Soless_Stage1_Hide_LineMap`,
line 65 = `LT_MONSTER_ROAD` at x 9885, z 2975-3455). The player was about 1,500 above it, and its land y fell 10 per frame (it rides a moving platform; lines 34 / 35
carry 2 followers each). User: monsters and the player's own landing behave normally. Ranox Fire Temple boss retest (W1b's site, now `false`):
`line 3 marked, users listed 1`, `line 4 marked, users listed 0`, then `release: line 3 user 12` 3 s later, as in W1b's pass.

### Phase W1c -- Review

**2026-10-18, Opus 5.5 high: PASS, 0 findings.** Commit reviewed: `2d9ad701` (the only code commit; `e9b8e601` / `b3d29353` are notes). The play-test
passed, so the short-lived `_DEBUG` flag is removed (its `#define` + `#error` guard in `Always.h`, its 13-line block in `KTDGLineMap.cpp`).

Ghidra re-read (disassembly, read-only):
- **Core `31c7bb00`.** Args: hidden ret `+8`, pos `+0xc..+0x14`, bool `+0x18`, radius `+0x1c`, `pLineIndex` `+0x20` (`ret 0x1c`). Filter order
  `@31c7bbab-bbca`: `bEnable` (`+8`), `LT_WALL` (`+4 == 1`), then `+4 == 7` with `byte [EBP+0x18] == 0` -> next line, then the height test `@31c7bbd0`.
  March's enum gives `LT_WALL = 1`, `LT_MONSTER_ROAD = 7`. The port's skip sits in the same place. Hit test `@31c7c904-c99b`: `|dx|+|dy|+|dz|` vs `r`,
  a miss when `r <= sum`, i.e. March's `LineToLine` `true_intersection` (`KTDXCollision.h:1765`), then the higher-`y` test and `line = i`.
  First fallback `@31c7cd4b-ce60`: only when `line == -1`; `*pLineIndex` taken, `0 <= idx < n` and non-NULL entry, start kept when `dStart <= dEnd`
  (March: end only when `dStart > dEnd`). An out-of-range index stays as it is and skips the second pass in both. Second pass `@31c7ce6e-cf84`:
  enabled lines, strict `<` on start then end, index only (`ret` unchanged), as March. Index written back `@31c7cf97`.
- **`false` sites.** `32010440 @32010ff8` (`FSTP [ESP]` radius, `PUSH 0`, pos, ret), one call for March's PhysicProcess start / end;
  `323eef70 @323f0491` (`PUSH 0`), in the `X2SquareUnit.cpp` module, decompiled: start `beforeLine (+0x1c) == -2` / end `nextLine (+0x20) == -2`
  merged into the one call, `CanDown` otherwise, which is March's `X2SquareUnit.cpp:2976-3010`; `32d2db50 @32d2dca1` (`PUSH 0`, radius `33441b9c`,
  `bIsRight` stored to `+0x388` and `iLineIndex = -1` just before), which is March's `CX2RidingPet::SetPos`. All three pass `false`.
- **Overloads.** The only 4-arg candidates take `bool` second, and March has no 4-arg call, so no March call changed overload. Every 3-arg March call
  (`int*`, `unsigned char*`, `unsigned short*`) reaches the 4-arg body with `true` through its old wrapper. The 12 edited calls pass `false` with
  `int*` / `unsigned short*` (`LAND_CHECK`'s `lastTouchLineIndex`, `JUNE_LINE_INDEX16` on) / `(int*) NULL`, exact matches. `GetLandPosition_LUA` keeps true.
- **Medium rows.** The Eve merged pairs and the 30 path-finder rows all pass `true` (`june_bMonster` 1 on every row), so site order cannot change a bool.
- **Coverage.** Every March `CKTDGLineMap::GetLandPosition` call is in the caller table except `CX2Game::GetOfflinePartyBotPos` (`X2Game.cpp:8599`), our
  AI-party bot placement; the bots are NPCs, so March's `true` is the NPC value. The rest of the 135 grep lines are unit `GetLandPosition()` getters.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_LINEMAP_LAND_MONSTER_ROAD --range 2d9ad701^..2d9ad701 --with ..._DEBUG`: `files: 8 PASS, 0 PASS~ (blank/marker only),
1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s),
0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Debug removal, `--worktree --files KTDGLineMap.cpp Always.h`: `KTDGLineMap.cpp`
PASS (-13), `Always.h` OTHER (a peer's uncommitted R12 `SKIN_ANIM_REFS_DEBUG` line; not in this commit); the 7 + 13 deleted lines are the `_DEBUG` block, `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,804,672 B, dated 2026-10-18 03:02:17, md5 7777696ad6668295a3608d1e5f1e9125, equal to X2/US_SERVICE/x2.exe (2026-10-18 03:02:12); built at main 3d6c7922 + 13 uncommitted code file(s).` (The 13 include peers' uncommitted files; only `Always.h` / `KTDGLineMap.cpp` are this review's.)

**Owes:** nothing (no play-test: the removed block only logged). Next: W1d or W2 (W2 shares `KTDGLineMap.*` with nothing open now).

## Phase W1d - Pet re-seat on the master's road type

**Run with:** Opus 5.5 high (map, then port), then a user play-test. Flag `SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT` (proposed). Added by W1c (an unowned
June branch): June's `CX2PETAI::Moving` (`32ef68b0`, block around `@32ef6a95`) re-seats a pet onto the nearest other line (`31c688c0`, then
`CX2PET::SetPosition` `326eb090`) when, in a field or square game, its line type differs from its master's. March's `Moving` (`X2PetAI.cpp:400`)
has no such block. Map `32ef68b0` whole against March, then port it whole (rule 1).

### Phase W1d -- Done

**2026-10-18, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT` (`Always.h`), plus the short-lived `_DEBUG` (the first 20 re-seats
logged; remove in the review once the play-test passed). Commit: `git log -S SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT`. Funcmap rows `(W1d)`:
`32ef68b0`, `326eb090`, `31c688c0`, `323a8dd0 / 323a4dd0`. JUNE_BUGS 278.

**Map.** June's `CX2PETAI::Moving` `32ef68b0` (`ret 4`) was read whole against March's (`X2PetAI.cpp:400`). There is one delta, the block at
`@32ef69b2-32ef6b08`. It runs every frame, after the final-dest copies and before the random jump, and only in a field or square game:
- The master is `GetSquareUnitByUID( m_MasterUid )` (`+0xd8/+0xdc`; `323a8dd0` on `g_pTFieldGame`, otherwise `323a4dd0` on `g_pSquareGame`).
- The pet's line is `m_iLastTouchLineIndex` (`+0x3c`); the master's is the square unit's byte `m_SyncData.m_LastTouchLineIndex` (`+0x3d4`).
  Both are bounds-checked to `LineData*` or NULL.
- The **pet's own** position (`m_pPet +0x42c` = `m_petSyncData.position`, not the master's as the kickoff said) is landed (`31c7bb00`,
  `bMonster` true, radius 30 `@33441b9c` = `LINE_RADIUS`, no index).
- `GetNearOtherLine( land, &out, +0x3c, &idx = -1, 200 @33457460 )` is called. If it returns true and the pet line's type (`LineData +4`) differs
  from the master line's, the pet is re-seated with `326eb090` = `CX2PET::SetPosition( out, m_bIsRight +0x40 )`.

The rest of `32ef68b0` is March's, checked branch by branch:
- the line-map pick;
- the random jump (`+0xc0`, AI data `+0x60..+0x70`, `RandomFloat( 0, 100 )` `@334355b8`, `100 -` double `@3342f360`);
- the fly / walk path-finder calls with March's argument lists (`32f298f0` / `32f287e0`; fly only when not fleeing and `m_bNowStateFly`);
- `m_bIsTargetFront`;
- the move-state switch (`+0x84`, the `IsSpeedTime` gate `326e2f50` on the walk-only DIR_CHANGE);
- the fly interval (`+0xd0` / `+0x88`) and the walk / dash interval (`+0xc4` / `+0x54`, `moveSplitRange` / `runGap` squared);
- the send (`PushAIMessage` `326e2df0`).

The callees equal March's. `31c688c0` = `GetNearOtherLine` (`KTDGLineMap.cpp:2733`): false for an out-of-range or NULL last line, skips
disabled lines and `LT_WALL`, takes `OTHER_ROAD` from `NORMAL` with `GetNearest` inlined (999999 sentinel `@33449110`, the start + dir / end - dir
fallback) and `NORMAL` from `OTHER_ROAD`. `326eb090` = `X2Pet.cpp:3129`: `GetLandPosition( pos, true, 30, &idx )`, then the inlined `IsOnLine`,
then the stores.

**Port.** `X2PetAI.cpp`: the block after `bool bAIIsRight = false;`, insert-only. The 3-arg `GetLandPosition( pos, LINE_RADIUS, (int*) NULL )` is
used (W1c: it forwards `true`, June's value), so the block does not depend on W1c's flag. June's two quirks are kept, as JUNE_BUGS 278:
- the found line's type is never compared to the master's;
- a master line index out of range is read through NULL.

**Hygiene** (`flagcheck --worktree --files X2Lib/X2PetAI.cpp KTDXLIB/Always.h`): X2PetAI.cpp PASS~ (+45, 1 blank line outside the flag);
Always.h OTHER (peers' uncommitted W1e-review / R13b lines, not in this commit); 0 deleted lines, 0 non-ASCII, 0 EOL-mismatched, 0 encoding
change; `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,873,280 B, dated 2026-10-18 06:34:55, md5 6e1b71e8bdb7cbe25ec57d44d4c11aa3, equal to X2/US_SERVICE/x2.exe (2026-10-18 06:34:50); built at main 68e3302f + 6 uncommitted code file(s).`
A peer's build (the same tree) compiled this phase's files. `grep -a "WORLD pet re-seat"` on the deployed exe = 1. Built in but not committed
here: the peers' `KTDGLineMap.cpp`, `X2SkillTree.cpp`, `X2WorldObjectSkinMesh.*`. No kom changed.

**Play-test hand-off.** Elder village (`WI_VILLAGE_ELDER`, `Linemap_Elder_village.lua`, data040, June bytecode). It has two staircases of
`LT_OTHER_ROAD`, lines 7-10 at x -304 and lines 11-14 at x 314. The bottom entry is `LT_NORMAL` line 17 (z 1027); line 10 / 14 carry
`OTHER_ROAD_UP`. With a pet summoned, stand on line 17 at a staircase foot and press Up (`X2TFieldGame.cpp:1528`; the warp is `SetPos` at
`:1542`). Expect `[JUNE_ENGINE] WORLD pet re-seat: pet line 17 type 0, master line 10 type 9 -> line N type 9 at ...` (N one of the stair lines 7-14; at most 20 lines) and
the pet appearing on the stair. Press Down to go back, and the reverse line (`type 9 ... type 0`) follows.
**Play-test PASSED (log, 2026-10-18).** The run used the W1d deploy (md5 6e1b71e8). 0 EXCEPTION; UNHANDLED = the 2x `E_DISABLE_HB_CHECK_REQ`
baseline; clean exit. Elder village, left staircase (x -304), reyaa's pet Chioo, 4 re-seat lines, both directions:
`pet line 16 type 0, master line 13 type 9 -> line 13 type 9 at -304,1051.56,1105.83` (stair foot),
`pet line 10 type 9, master line 5 type 0 -> line 5 type 0 at -304,1812,2470` (top road),
then the same pair again via stair line 10 (`master line 35 type 9`, a different other-road line than the pet's target: June's compare is by
type only). The logged indices are the engine's; they differ from the hand-off's `june_src_fixed` numbering (17 / 7-14).

### Phase W1d -- Review

**2026-10-18, Opus 5.5 high. PASS, no transcription error, no code change except dropping the short-lived `_DEBUG`.**
- **The block** (`@32ef69b2-32ef6b08`, re-read in the disassembly): the field test comes first (`[0x33775870]` -> `323a8dd0` with `ECX` = the
  field game), then the square game (`[0x3377586c]` -> `323a4dd0`), both on `+0xd8/+0xdc`; a NULL master skips. Pet line = `+0x3c` bounds-checked
  against the line map's `+0x190/+0x194` vector (`[ESP+0x18]` or 0); master line = `MOVZX` byte `+0x3d4` (March `unsigned char
  m_LastTouchLineIndex`, `X2SquareUnit.h:90`), same check, `EDI` or 0. Land: `m_pPet +0xa0`, position `+0x42c..+0x434`, pushes NULL, 30
  (`@33441b9c`), 1, so `GetLandPosition( pos, true, 30, NULL )`; the port's 3-arg form resolves to the `int*` forwarder that passes `true`
  (`KTDGLineMap.h`, W1c). `GetNearOtherLine( land, &out = 0, +0x3c, &idx = -1, 200 @33457460 )`, argument order traced through the stack.
  `CMP AL,1`, then `[pet line +4]` vs `[master line +4]` (`lineType`, `LineData +4`), then `326eb090( &out, byte +0x40 )`. The port matches all of it.
- **The rest of `32ef68b0` = March** (`X2PetAI.cpp:400`): line-map pick; jump (`+0xd5` not flying, `+0xc0` vs AI `+0x60`, up `+0x64/+0x68`, down
  `+0x6c/+0x70`, `100 -` chance); fly / walk path-finder argument lists (`+0x44` doubled for fly, `+0x1c` lost-target height); `+0xb4`; the switch
  (`IsSpeedTime` only on the walk DIR_CHANGE, fly sets `{9, 0x11..0x16}`); fly interval `+0xd0/+0x88`; walk / dash `+0xc4/+0x54`,
  `moveSplitRange` / `runGap` squared with March's `>=`; send when changed and non-zero.
- **Callees.** `31c688c0` = March `GetNearOtherLine` (`KTDGLineMap.cpp:2713` in today's tree): NULL / out-of-range last line false; skips self,
  NULL, `!bEnable`, `LT_WALL` (1); OTHER_ROAD-from-NORMAL with `GetNearest( pos, &out, i )` inlined (999999, the start + dir / end - dir fallback),
  NORMAL-from-OTHER_ROAD nearest only. `326eb090` = March `CX2PET::SetPosition` (`X2Pet.cpp:3129`; June returns void, the caller ignores it):
  `GetAnyEnabledLine`, `+0x439`, land `( pos, true, 30, &idx )`, inlined `IsOnLine` / `GetNearestWithBound` (skips types 1 / 4 / 5 / 8 =
  wall + the `bIsMonster` unit / jump-up roads, wraps from idx, stops at 0), the stores and the `+180`.
- **JUNE_BUGS 278** holds: after `CMP AL,1` only the two `+4` reads come before the call (no read of the found line), and `EDI` is 0 when the byte
  index is at or past the line count.
- **Hygiene** (`flagcheck --range 4f035996^..4f035996 --with ..._DEBUG`): `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other
  flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0
  #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. PASS~ = the one blank line after `#endif`.
- **`_DEBUG` removed** (play-test passed): the `#define` block in `Always.h` (13 lines) and the log block in `X2PetAI.cpp` (12 lines).
  `flagcheck --worktree --files X2Lib/X2PetAI.cpp KTDXLIB/Always.h`: X2PetAI.cpp PASS (-12, the debug block); Always.h OTHER (the peers' +107
  uncommitted lines, not in this commit); `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to X2/US_SERVICE/x2.exe (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s).`
  A peer's build (same tree) linked this edit; `WORLD pet re-seat` in the deployed exe = 0 (was 1). Nothing to play-test: the re-seat itself is
  unchanged and passed.

## Phase W1e - Line links to a far line (Elysion line 2 `BEFORE_LINE = 130`)

**Run with:** Opus 5.5 high (map, then port), then a user play-test. Flag `SERV_IRUHADEV_JUNE_LINEMAP_FAR_LINK` (proposed). Added by R13's play-test
(an unowned June branch). Not alongside W2 or another session editing `KTDGLineMap.*`.
- **Symptom (play-test 2026-10-18):** in the Elysion village, landing near the red-tree pad (506) on the back walkway moves the unit to the far-right
  stub (line 130), where pad 512 fires after 1.5 s and drops it at 513 (the blue-tree side).
- **Data:** June's own `Elysion_villeage_LineMap.lua` gives line 2 `BEFORE_LINE = 130`; its real neighbour, line 1, ends at its start (-2789). It is
  the only far link in the map (a gap scan of all 131 lines; line 130 also has `NEXT_LINE = 130`).
- **Engine:** March's `CanDown` (`KTDGLineMap.cpp:1238`) follows `beforeLine` with no distance test (`:1355`, `:1421`, `:1475`). June's `CanDown`
  is `31c6c310` (more arguments; about 1,500 decompiled lines). Find out whether June validates a link, rebuilds links at load, or follows
  them as March does. If June guards, port the whole function (rule 1). If not, June's game showed this too: add a JUNE_BUGS row and ask the
  user (memory `keep-june-bugs-verbatim-without-asking`: a verbatim result the June game never showed is a port gap).

### Phase W1e -- Done

**2026-10-18, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK` (`Always.h`; renamed from the proposed `_FAR_LINK`: June's rule is
about the link's type, not its length) plus the short-lived `_DEBUG`. Commit: `git log -S SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK`. Funcmap rows `(W1e)`.

**Map: June guards; it does not follow blindly, and it rebuilds nothing.** `AddLine_LUA` (`31c705c0`) reads `BEFORE_LINE` / `NEXT_LINE` as given
(+0x1c / +0x20, default -1). The gate is in `CanDown` (`31c6c310`, `ret 0x2c`), read from the disassembly (`[EBP+0x28]` = `bIsMonster`). Every place
March switches to a link, June adds `lineType == 7` (`LT_MONSTER_ROAD`) `&& bIsMonster`:
- the loop, linked line's type: higher / start `@31c6cbad-31c6cbdb`, higher / end `@31c6e5ee-31c6e620`, lower / start `@31c6f60d-31c6f624`, lower / end
  `@31c6f6b7-31c6f6d5`. Refused = March's own else (`@31c6e597` / `@31c6f53c`: line i only while `touch` is false; `@31c6f652` / `@31c6f6ff`: line i, pointL2);
- the no-hit fallback, the **entry** line's own type (`@31c6f7dc`, `@31c6f82a`), behind a bounds test (`@31c6f756` signed, `@31c6f763` unsigned);
  a NULL entry line returns false (`@31c6f77d`);
- one more guard: the hit test is skipped without `g_pKTDXApp +0x1c0` (`@31c6c57d`) = `m_pCollision` (ctor `31c17ed0`: viewport zeroed, `new(0x1c)`, device manager, March's order).
The rest is March's: switch on `lineType - 1` (wall skipped; unit road / jump-up skipped for monsters), the bounding-box precheck, `GetDistance3Sq`
(`31c7b5c0`) against `0.0001`, March's quirk of `startPos` for a lower-branch next follow, `pos.y < 0` -> false.
So for a user, June never takes a link in `CanDown`. Elysion line 130 is `LT_NORMAL`, so line 2 keeps the landing: the warp was March's alone.
- **Walk follower:** June's `IsOnLineConnect` (inlined in the user line helper `32010440`) = March's (any enabled link that is not a
  monster road, no distance test). It runs only after `CanDown` fails, under `bLandConnect`; unchanged.
- **Routed to new W1f** (an unowned June branch): the `DROPPABLE_MONSTER_ROAD` line key (`+0xa4`, `@31c716be`; only June's 2017 Valentine
  maps) with `CanDown`'s 12th argument, and June's separate out line index (`param_8`); both need the 342-site caller map. `32010440 @3201104a`
  passes March's shape (same index pointer twice, four zeros).

**Port** (`KTDGLineMap.cpp`, insert-only, `#else` = March): the 6 gates, the fallback bounds / NULL test, the collision skip. `_DEBUG`:
`JuneCanDownLinkLog` before the 4 loop sites.

**Hygiene** (`flagcheck --worktree --files KTDXLIB/KTDGLineMap.cpp --with ..._DEBUG`): 1 PASS (+93), 0 deleted, 0 non-ASCII, 0 EOL-mismatched,
0 `#else`/`#endif` without token, 0 encoding change; RESULT PASS. Range (`--base 4c3719c8^ --files KTDGLineMap.cpp Always.h`, the final tree): `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (Always.h: other flags), 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`. Commits: `4c3719c8` (its tree was built before S2's `c91ce6c6` landed, so it undid S2; R14's `a5aeb19b` then carried S2 back and dropped W1e), `a40bb330` (restores W1e). Net HEAD: W1e and S2 complete.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,872,256 B, dated 2026-10-18 03:21:50, md5 ce79dccff14191fdfb02268e1cd90a81, equal to X2/US_SERVICE/x2.exe (2026-10-18 03:21:45); built at main 60eac05c + 29 uncommitted code file(s).` (A peer's build linked the exe with W1e's files in it; `WORLD far link ` found once in the deployed exe.)

**Play-test hand-off:** see STATUS / the hand-off in this phase's session. Log: `python tools/logs.py --grep "WORLD far link"`.

**Play-test PASSED (user, 2026-10-18; logs read).** Run on the deploy of `main 21783037` (md5 3f645dcc, carries W1e). ErrorLog: one
`[JUNE_ENGINE] WORLD far link refused: map Elysion_villeage_LineMap.lua line 2 before 130 type 0 gap 11704.5 (unit)` (06:08:15): the landing that
used to warp stayed on line 2. Only the pads the user stepped on fired (506 -> 507, 508 -> 509); no pad 512. 0 EXCEPTION; UNHANDLED = the 2x
`E_DISABLE_HB_CHECK_REQ` baseline.

### Phase W1e -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no transcription error.** Re-read in Ghidra (`31c6c310`, disassembly + decompile):
- **Loop gates.** Higher / start `@31c6cbad`: `[EDX+0x1c] > -1`, linked `+0x8` (bEnable) `== 1`, linked `+0x4` (lineType) `== 7`, `[EBP+0x28]`
  (bIsMonster) `== 1`, every miss -> `@31c6e597` = `if !touch { retVal = pointL2; line = i }` then `touch = 1` (`@31c6e5ba`). Higher / end
  `@31c6e5f2-31c6e620`: the same on `+0x20`, miss -> `@31c6f53c` (same else). Lower / start `@31c6f60d-31c6f624` and lower / end `@31c6f6be-31c6f6d5`
  add the NULL test; miss -> `@31c6f652` / `@31c6f6ff` (-> `@31c6f5e7`): `line = i`, `retVal = pointL2`. Follow = March's (`+0x30` endPos for
  before, `+0x24` startPos for next). `LT_MONSTER_ROAD` is 7 in March's enum (`KTDGLineMap.h`: NORMAL 0 .. BUNGEE 6, MONSTER_ROAD 7). Port = all four.
- **Fallback** `@31c6f747`: NULL index pointer -> `@31c6f87c`, `JS` (negative) / unsigned `JBE` (>= size) -> `@31c6f87c` (the `pos.y < 0` tail, then
  `return touch` = false), NULL entry line `@31c6f77d` -> `@31c6c3dd` (`XOR AL,AL`, pOutPos untouched). `dist2 <= dist1` -> next (`TEST AH,0x41`),
  `link > 0`, `dist < fRadius`, then the **entry** line's `+0x4 == 7` and bIsMonster (`@31c6f7dc` / `@31c6f82a`). Port = this.
- **Collision skip** `@31c6c577-31c6c584`: `[0x337676e8]` (g_pKTDXApp) `+0x1c0 == 0` -> `@31c6f722` = `i++` (a `continue`). Port = this.
- **Unchanged body = March's:** switch on `lineType - 1` (8-entry table `31c6f898`; type 1 wall skipped; 4 / 5 / 8 skipped for a monster; 7 needs
  bIsMonster, its `+0xa4` / arg-12 escape is W1f's); bounding-box precheck; lower branch's `line` range test -> `return false` (`@31c6f578` / `@31c6f804`)
  and NULL best line -> `touch = 1` (`@31c6e56b`); `-1` / `< -1` link cases (`@31c6f5cf` / `@31c6e5ba`); `0.0001` (`33449190`) tests; `pos.y < 0`
  -> false (`@31c6f87c`).

**Hygiene** (kickoff command + `--files KTDXLIB/KTDGLineMap.cpp KTDXLIB/Always.h`, range `4c3719c8^..a40bb330`):
`files: 3 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 93 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
`RESULT: PASS`. The 2 OTHER rows: `4c3719c` Always.h (-21 = S2's lines its stale tree undid) and `a5aeb19` Always.h (R14's own block, S2 carried
back, W1e's define dropped). The 93 deleted lines are `a5aeb19` dropping W1e's `KTDGLineMap.cpp` block, re-added whole by `a40bb33`.
Net `4c3719c8^..HEAD` on both files: 0 deleted lines.

**`_DEBUG` removed** (play-test passed): the `#define`, its two `#error` guards and comment (Always.h, 11 lines), and `JuneCanDownLinkLog`
plus its 4 call blocks (`KTDGLineMap.cpp`, 42 lines). `flagcheck --worktree --files KTDXLIB/KTDGLineMap.cpp --with ..._DEBUG`: 1 PASS (-42),
0 FAIL, RESULT PASS; no `CANDOWN_LINK_DEBUG` left in `KTDXLIB` / `X2Lib` / `X2`.
**Deploy:** `DEPLOY OK: X2_offline.exe is 19,873,280 B, dated 2026-10-18 06:34:55, md5 6e1b71e8bdb7cbe25ec57d44d4c11aa3, equal to X2/US_SERVICE/x2.exe (2026-10-18 06:34:50); built at main 68e3302f + 6 uncommitted code file(s).` (A peer's build linked the edit; `WORLD far link` is found 0 times in the deployed exe.)

No open question. W1f (`kickoffs/june-engine/W1f.md`, checked) owns the signature extras.

## Phase W1f - CanDown's June signature and its callers

**Run with:** Opus 5.5 high (map, then port), then a user play-test only if a March map has a droppable monster road. Flag
`SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` (proposed). Added by W1e (an unowned June branch). Not alongside W2 or another session editing `KTDGLineMap.*`.
- **`DROPPABLE_MONSTER_ROAD`:** June `AddLine_LUA` `31c705c0 @31c716be` -> `LineData +0xa4` (bool, default false). `CanDown`'s monster-road case
  (`@31c6c491-31c6c4ae`) tests the line for a monster, or when `+0xa4` and the 12th argument (`[EBP+0x30]`) are both set. Only the five
  `2017_Valentine_Stage*_Linemap` files use it.
- **Out line index:** June reads the entry index from `param_7` (`[EBP+0x1c]`) and writes the result to `param_8` (`[EBP+0x20]`, `@31c6f86c`).
- **Callers:** 342 June sites; table them (`june2017_re/w1f_candown_callers.tsv`), port the ones that differ from March's shape.

### Phase W1f -- Done

**2026-10-18, Opus 5.5 high.** Flag `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` (`Always.h`). Commit: `git log -S SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE`.
Funcmap rows `(W1f)`. Caller table: `june2017_re/w1f_candown_callers.tsv` (343 rows).

**Map** (`31c6c310` disassembly). `[EBP+0x1c]` (in index) is read only at the `bOnLine` head (`@31c6c320`) and the no-hit fallback (`@31c6f747`); NULL skips
both. `[EBP+0x20]` (out index) is written only when `touch` (`@31c6f86c`). The `LT_MONSTER_ROAD` case (`@31c6c491-31c6c4ae`) tests the line when `bIsMonster`,
else only when `LineData +0xa4` and `[EBP+0x30]` (`bDroppable`) are both set. `AddLine_LUA` stores 0 when the key is absent (`@31c716ec-31c716f0`); June's
LineData init `31c7da50` does not write `+0xa4` (March's `Init` sets false: no loaded line differs).
**Data:** only June's `2017_Valentine_Stage1-5_Linemap` (11-23 lines each) set the key. March ships none of them (a byte scan of all of `kom_contents` finds no
`DROPPABLE_MONSTER_ROAD`), so no gate log was added and there is no play-test.

**Callers** (pushes read by script from June's raw bytes, wrapper branches followed; 6 sites hand-read; the 19 that differ identified by two Sonnet
subagents, the four ported ones re-read here). 324 sites use March's shape (same pointer twice or NULL twice, `bDroppable` 0), so the forwarder covers them.
Of the 19 that differ:
- **Ported, 4 sites:** `CEffect::FollowLine` (`@31f71146`: in NULL, `bOnLine` false), `CEffect::CreateDieDamageEffect` (`@31f82653`: in NULL),
  `CX2DropItemManager::OnFrameMove` x2 (`@3273c114`, `@3273c599`: in NULL, `bDroppable` true; a drop item may rest on a droppable monster road).
- **Equivalent, no change, 3 sites:** pet `SCT_AI_DOWN`, Chung Artillery Strike, Aisha Hell Drop. June passes NULL out; March's out locals are never read.
- **New W1g:** `CX2SimplePathFinder::FollowTargetPathFly` (`32f27b10`) adds a fly-down `CanDown` gate (`@32f27ea2`) and differs elsewhere; whole function.
- **Class code -> checklist 7ac:** Ara `ProcessLockOn` + Swallow Assault x2, June-only helpers (Add / Luciel / Arme / Ara / Rena Grand Archer). Luciel's
  states come with the class.

**Port.** `KTDGLineMap.h`: `LineData::m_bJuneDroppableMonsterRoad`; June's 9-arg `CanDown` declaration; March's 7-arg form becomes an inline forwarder
(index twice, `false`). `KTDGLineMap.cpp`: the key parse, the definition with `const int* pLineIndex` (in) + `pOutLineIndex`, the droppable case, the
out write. 4 call sites `#ifdef` / `#else` with the original call kept.

**Hygiene** (`flagcheck --worktree --files`, 5 files): 4 PASS (`KTDGLineMap.cpp` +22, `.h` +21, `X2DamageEffect.cpp` +10, `X2DropItemManager.cpp` +10);
`Always.h` FAIL from the peer's uncommitted R12b lines only (its `_REFS_DEBUG` re-define, a dropped blank line), not committed here. 0 non-ASCII,
0 EOL-mismatched, 0 `#else`/`#endif` without token, 0 encoding change. Range (`153ef842^..153ef842`): `files: 4 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`. Committed through a private index (`GIT_INDEX_FILE`): peers were staging into the shared one.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to X2/US_SERVICE/x2.exe (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s).`
`DROPPABLE_MONSTER_ROAD` found once in the deployed exe.

**How to see it in game:** nothing should change. Drop items, damage effects that follow a line, and death effects still land as before. No open question.

### Phase W1f -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change, no build.** The source was June's `unpacked_x2.exe` (Ghidra + `disasm_range.py`), read only.

- **Signature.** In `31c6c310`, `[EBP+0x1c]` is read twice. At `@31c6c320` it goes into EAX and `*in` into EBX, both used only by the head
  block and overwritten before the loop (`@31c6c401`, `@31c6c47f`). At `@31c6f747` it is the fallback, NULL-tested and bounds-tested.
  `[EBP+0x20]` is touched only at `@31c6f86c` (NULL test, then a write of EDI). Both the loop touch and the fallback touch reach it through
  `@31c6f85d`. `RET 0x2c` = 11 dwords = the 9-arg form. March's body reads `pLineIndex` only at the head (`KTDGLineMap.cpp:1241`,
  `JUNE_MOVABLE_LINE`) and in the fallback (`:1501-1508`) and writes only `pOutLineIndex` (`:1556`); `const int*` makes any other write a
  compile error. The forwarder (`KTDGLineMap.h`) passes `pLineIndex` twice and `false`. Since the head and fallback reads come before the
  write, March's single-pointer behaviour is unchanged.
- **Monster road** `@31c6c491-31c6c4ae`: `bIsMonster` -> test; else `+0xa4 == 0` -> skip; else `[EBP+0x30] != 0` -> test, else skip. This is
  the port's `!monster && ( !droppable || !bDroppable ) -> continue`. The loader default (`@31c716ec-31c716f0`, absent = 0) was not
  re-read: the Done note's reading was taken as is.
- **Four ported sites**, pushes read right to left (`bDroppable, bOnLine, bIsMonster, bDownRadius, out, in, pOutPos, radius, pos`):
  - `@31f71146`: `0,0,0,0, &local, 0, &local`.
  - `@31f82653`: `0,1,0,0, &local, 0, &local`, radius `[0x33441b9c]` = 30.0 = `LINE_RADIUS`. It sits inside `TEST EAX,EAX / JNZ` after
    `31c83970` (`@31f82571`), the `pMeshInst == NULL` branch, like March `X2DamageEffect.cpp:7533`.
  - `@3273c114` / `@3273c599`: `1,1,0,0, &local, 0, &local`, radius `[0x3349bcbc]` = 69.0.

  March's four 9-arg lines (`X2DamageEffect.cpp:7541` / `:8059`, `X2DropItemManager.cpp:170` / `:193`) are the same calls.
- **324 "March shape" rows**, 5 spot-checked:
  - `31fd749d` / `31fd74d6` is the inlined 16-bit index wrapper (`movzx` into a local). One branch pushes `&local` twice and writes it
    back; the other pushes `0, 0`. Flags `0,1,0,1`.
  - `31fd713f`: the same wrapper, same pointer twice.
  - `32c8cd12`: `0, 0`; `bOnLine` / `bDownRadius` = EBX (1).
  - `32b22fb3`: `push edx` twice; EBX = 1 (`@32b22e43`).
- **Equivalent sites.** June's pushes at `32bfe8c3` are `0,1,0,1, 0, &local, &local`. In March, `vTempLandPosition` /
  `iTempLastLineIndex` (`X2Pet.cpp:2915-2919`) and `vLandPos` / `iNewLineIndex` (`X2GUChung.cpp:17149-17152`) are never read after the
  call. Aisha's `iNewLineIndex` (`X2GUArme_VioletMage.cpp:16479-16486`) is only reset, never read. The verdicts hold. The memo NPC is in
  checklist 7ac.
- **Overloads** (Sonnet grep, depth-aware argument count over `X2Lib`, `KTDXLIB`, `X2`): no 8-arg call, and exactly the four 9-arg calls
  above, all under the flag. Every 4th argument is a pointer. The 4-, 5-, 6- and 7-arg calls and the `JUNE_LINE_INDEX16` wrappers all
  resolve to the forwarder.
- **Hygiene** (`--range 153ef842^..153ef842`): `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. `git log -S` also finds `4c3719c8` / `a5aeb19b` / `a40bb330`, whose messages or diffs name the flag. Not hygiene rows.
- **Correction to the Done note's "nothing should change".** March's original calls at all four ported lines passed
  `&lineIndex` (= 0) with `bOnLine` true, so March's head tested **line 0**. If line 0 is `CAN_DOWN` false, March returned false: the drop
  item fell back to `GetNearestLine`, and the effect did not follow. June (NULL in) never makes that test, and the port follows June. This
  is visible only on a map whose line 0 cannot be gone down. No play-test asked for.

## Phase W1g - FollowTargetPathFly as June's

**Run with:** Opus 5.5 high (map, then port), then a user play-test. Flag `SERV_IRUHADEV_JUNE_PATHFINDER_FLY` (proposed). Added by W1f (an unowned June
branch). Kickoff `kickoffs/june-engine/W1g.md`. June `32f27b10` vs March `X2SimplePathFinder.cpp:760`: the fly-down `CanDown` gate `@32f27ea2` (in = copy of
`iCurrLineDataIndex`, out NULL, `bDownRadius` 1, `bIsMonster` 1, `bOnLine` 0), the destination line's `+0xf4` / `+0xf8` read, the road-type early return,
`GetLandPosition( pos, true, ... )`. Port the whole function.

### Phase W1g -- Done

**2026-10-18, Opus 5.5 high.** Flags `SERV_IRUHADEV_JUNE_PATHFINDER_FLY` + short-lived `SERV_IRUHADEV_JUNE_PATHFINDER_FLY_DEBUG` (`Always.h`; `#error` without
W1f's `JUNE_LINEMAP_CANDOWN_SIGNATURE`). Commit: `git log -S SERV_IRUHADEV_JUNE_PATHFINDER_FLY`. Funcmap row `32f27b10 (W1g)`.

**Map** (`32f27b10` decompile + disassembly, read whole against March `X2SimplePathFinder.cpp:760`). One delta: when `cur.y > dest.y + fDestArriveHeight + 10`
(10.0 double `@33438088`), June copies `vCurrPos` to a scratch and calls `CanDown( vCurrPos, 30 @33441b9c, &scratch, &copy of iCurrLineDataIndex, NULL,
true, true, false, false )` (`@32f27e51-32f27ea2`; the copy is `[ESP+0x34]`, stored `@32f27b72`); a false return clears `bFlyDown` (`[ESP+0x12]`, `@32f27eb3`),
so the unit keeps its height (`FLY_*_FRONT` / `_BACK`, or `MS_WAIT` once in the arrival band). Everything else is March's:
- `RET 0x30`: the 12 arguments stay (`fLineEndDetectRange` is unused in both);
- the head's `+0xf4` / `+0xf8` read is `m_lineGroupID` (`X2OPTIMIZE_LINEMAP_LINEGROUP`, KTDX.h:298); types 4 / 5 / 8 = `LT_JUMP_UP_REL` / `_ABS` / `LT_UNIT_ROAD`,
  `rand` odd -> `+0x10` (`m_MoveState`) = 0xb `MS_FLY_LEFT_FRONT`, else 0x11 `MS_FLY_RIGHT_FRONT`, as March;
- every `GetLandPosition` is `31c7bb00( .., true, 2000 / 30, .. )` = March's 3-arg forwarder (W1c);
- bFlyFront, arrival band, turn (0.3 `@3345993c` / -0.3 `@3348434c`), both side checks (before / next line, start / end pos, `+0x95` monster-allowed
  early return, `31c8ee10` / `31cfd6b0` = `LineGroupID` == / != against `this + 0` = `m_pPrevLineGroup`) and the 12-state select = March.
No June quirk to record (no `JUNE_BUGS.md` row).

**Port.** `X2SimplePathFinder.cpp`: one insert-only block after March's `bFlyDown = true;` (the 9-arg `CanDown`, no `#else` needed). `_DEBUG` logs the
first 20 refusals and 5 passes: `[JUNE_ENGINE] WORLD fly-down refused / allowed` in `ErrorLog.txt`.

**Hygiene** (`flagcheck --worktree --files`): `X2SimplePathFinder.cpp` PASS +37; `Always.h` OTHER = peers' uncommitted `Always.h` lines (R12b `REFS_DEBUG`,
S3b `JUNE_SKILL_NOTE`), not in this commit; `files: 1 PASS, 0 PASS~, 0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, 0 non-ASCII, 0 EOL-mismatched,
0 encoding change, `RESULT: PASS`. Range (`e2006aa7^..e2006aa7`): `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`.

**Deploy.** My three `build.py --deploy` runs lost the lock to two peer builds; the fourth failed in KTDXLIB on a peer's uncommitted S3b line
(`Always.h(8043): #error SERV_IRUHADEV_JUNE_SKILL_NOTE needs REFORM_SKILL_NOTE_UI`), not on W1g. The deployed exe is a peer's `build.py --deploy` of this
shared tree, whose dirty list includes `X2Lib/X2SimplePathFinder.cpp` and `Always.h` with W1g in them (edited 07:49): `last_deploy.json`: X2_offline.exe
19,944,960 B, deployed 2026-10-18 08:09:08, md5 00703adcea5d79dfdd6c61b040922363, HEAD 7a43b7a3 + 6 dirty code files. `WORLD fly-down refused` and
`WORLD fly-down allowed` are each found once in it.

**How to see it in game:** Sander Garpai Rock Normal (33210) stage 0 spawns 12 Wing Harpies and 9 Giant Harpies (`FLY_CHASE_MOVE` = fly units,
`X2GUNPCAI.cpp:257`, chase path `:1517-1527`); stand on the ground below them. They still come down to you; a harpy over a drop with no line under it stays
level (refused line). Open question: none.

**Play-test PASSED 2026-10-18** (user + `logs.py --grep "WORLD fly-down"`, run after deploy, exe d4eef836): 5 `allowed` lines over line 2
(y 790, land y 350 = dest y 350); 9 `refused` lines over line 10 (y 529 -> 505, dest y 394 + height 100, no line below); 0 EXCEPTION, UNHANDLED = the
2x `E_DISABLE_HB_CHECK_REQ` baseline.

### Phase W1g -- Review

**2026-10-18, Opus 5.5 high. Verdict: PASS, no code change; `_DEBUG` off.** The source was June's `unpacked_x2.exe` (Ghidra: decompile and disassembly of
`32f27b10`, disassembly of `31c6c310`), read only. Below, `S` = ESP after the four prologue pushes (`SUB ESP,0xd8` + EBX/EBP/ESI/EDI).

- **Arguments.** ESI = `[S+0xec]` = arg 1 `vCurrPos`. EDI = arg 2 `iCurrLineDataIndex`, copied to `[S+0x34]` `@32f27b72`. `[S+0x7c]` = arg 3 `vFinalDestPos`.
  EAX = arg 4. EBX = arg 9 `pLineMap`. `[S+0x104]` = arg 7 height, `[S+0x108]` / `[S+0x110]` = args 8 / 10 area / gap. `RET 0x30` = 12 dwords = March's 12 arguments.
- **Gate `@32f27e3f-32f27ea2`.** Pushes, last argument first: `0` bDroppable, `0` bOnLine, `1` bIsMonster, `1` bDownRadius, `0` out index, `&[S+0x34]` in index,
  `&[S+0x84]` out position (just filled with `vCurrPos`), radius `[0x33441b9c]` (30 = `LINE_RADIUS`), then the vector by value. The bool order is fixed by the callee:
  `31c6c310` tests `[EBP+0x2c]` = bOnLine before its first in-index read `@31c6c31c`. March's call binds the 9-arg declaration (`KTDGLineMap.h:1297`,
  `const int* pInLineIndex`) with the same order. `31c6c310` only reads `[EBP+0x1c]` (`@31c6c320`, `@31c6f747`), so `[S+0x34]` keeps `iCurrLineDataIndex` for the
  side checks, as March's `iCurrLineDataIndex` does. The scratch `[S+0x84..0x8c]` is not the right vector (`[S+0x90]`), the destination direction (`[S+0x9c]`) or a
  live temporary. The out position is discarded, as in the port.
- **Flags and compares.** `[S+0x13]` = bFlyUp (set `@32f27e23`), `[S+0x12]` = bFlyDown (set `@32f27e9d`). The fail path `@32f27eb3` clears only `[S+0x12]`.
  Arrival: `0x17` (MS_FLY_UP) from `[S+0x13]`, `0x18` (MS_FLY_DOWN) from `[S+0x12]`. Traced on the x87 stack: up when `cur.y < dest.y + h - 10`, gate when
  `dest.y + h + 10 < cur.y`, with 10.0 the double `@33438088`. Same as March `:875` / `:881`.
- **Rest = March, re-read.** Enum numbering: `MS_FLY_LEFT_FRONT` = 0xb ... `MS_FLY_RIGHT_BACK_DOWN` = 0x16, `MS_FLY_UP` / `_DOWN` = 0x17 / 0x18
  (`X2SimplePathFinder.h`). Line types: `LT_JUMP_UP_REL` / `_ABS` / `LT_UNIT_ROAD` = 4 / 5 / 8 (`KTDGLineMap.h`).
  - Head: `+0xf4` / `+0xf8` group; types 4 / 5 / 8 do `rand` odd -> 0xb, else 0x11 (`NEG/SBB/AND 0xfffffffa/ADD 0x11`). The switch is state 0 or 0xb..0x18,
    which is exactly March's 15 cases. NULL current line -> state 0, turn 0. `GetLandPosition( vCurrPos, true, 2000, &-1 )`.
  - bFlyFront `@32f27da0-32f27dec` = `!(L < gap - area) || L > gap + area`. Arrival band `@32f27eb8-32f27ed0` = `gap - area <= L <= gap + area`.
    Turn: 0xb..0xd with `fIsRight > 0.3` `@3345993c`, or 0x11..0x13 with `fIsRight < -0.3` `@3348434c` -> turn 0, MS_WAIT.
  - Side-check state set: {0x11, 0xb, 0x14, 0xe, 0x15, 0xf, 0x16, 0x10} = March `:969-972`. `fIsRight >= 0` uses beforeLine `+0x1c`, `startPos - right*R - up*R`,
    jump `- right * fMaxJumpRight / pow(2, i)`, and ends at 0x11. The other side uses nextLine `+0x20`, `endPos + right*R - up*R`, `+`, and ends at 0xb.
  - Each landing: `!= [S+0x34]`, non-NULL, type not 4 / 5 / 8, `+0x95` == 0 -> return, `31c8ee10` (== final group) or `31cfd6b0` (!= `this + 0`).
    `m_iDestLineDataIndex` is reset after the down check. Final 12-state select = March `:1193-1234`.
- **Play-test lines.** The only caller of `FollowTargetPathFly` is `CX2GUNPCAI` (`X2GUNPCAI.cpp:1527`, the fly chase; pets use `SimpleFollowTargetPathFly`),
  so both logged branches came from fly NPCs. The lines do not name the unit. The refused run's y 529 -> 505 is movement outside this function (the gate only
  withholds the `_DOWN` states); not traced.
- **Hygiene** (`--range e2006aa7^..e2006aa7`): `files: 1 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. `_DEBUG` off (`--worktree --files KTDXLIB/Always.h`): `1 DEFINE`, `RESULT: PASS`. The 1 deleted line is the `#define`, now commented with the date.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,971,584 B, dated 2026-10-18 10:11:33, md5 2475b432c08a3011bf58842f7ff272d2`, built at main `b359e5c1` + 8
  uncommitted code files (also peers' work in progress, e.g. S5 `JUNE_SKILL_SLOT_10`). No play-test needed: `_DEBUG` off only removes log lines.

## Phase W2 - Portal-line markers

**Run with:** Opus 5.5 high (map, then port), then an Opus review. Flag `SERV_IRUHADEV_JUNE_WORLD_PORTAL_GATE`. One system:
- `322e5820` and the game `+0x7bc` handle list. Map its lifecycle, `322f5d70` / `32306600`.
- June's `StageLoad` portal block, which replaces `PORTAL_LINEMAP_TEST`'s `Peita_Teleport_MagicSquare01` in every dungeon (a visible
  change).
- The `SHOW_PORTAL` / `LINE_PORTAL_TYPE` / `PORTAL_KEEPER` parse in `AddLine_LUA` `31c705c0`. Map type 3's keys and every reader of
  the open conditions.
- `DoTriggerLine` case 7's call.

If the keeper consumers turn out to be Erda-only gameplay, route them to backlog Stage 19 with a dated note. Test case:
`Ranox_Secret_Stage2` (retype) and any dungeon's portal line.

## Phase W3 - June village world Lua

**Run with:** Sonnet 5.5 high (census, then adopt), then a user play-test; say "this needs a higher model" and stop if a June world-object method or key has
no March binding. Log tag `[JUNE_ENGINE] WVIL`. No code flag unless the census routes one (then a separate engine phase, never a Lua edit).
Added 2026-10-18 by the R13b play-test (user: "replace all the village's lua with June"). Nobody owned it: the asset port leaves `.lua` to the Lua ports,
and the 2026-10-09 ljconv adoption took NPC / dungeon scripts, not village world scripts. The deployed `data040` `Besma_Village.lua` is still March's (horse at
0,0,0, `SetCanHide(false, false)`; June: 1300,0,1000, `(true, true)`), so R13b's village check had nothing to fade. Standing rules: June's file wins at the same
name, June behaviour wins even when it drops a March feature (memories `june-asset-wins-over-march-at-same-filename`, `june-behaviour-wins-even-when-it-removes-features`).
- **Scope:** every village world script in `kom_contents/data040_kom/` and its line-map script (`*_Village*.lua`, `*_village*.lua`, `LINEMAP_*` / `Linemap_*` /
  `*_LineMap.lua` of those villages), with the seasonal variants (`_Festival`, `_Snow`, `_XMAS`), replaced by June's through `luasrc.py adopt-june` (ljconv bytecode,
  no edit). Core-list files (`luasrc.py core --check`: `WorldData.lua`, the `DLG_Map_*` dialogs, ...) stay as they are. Files already June (Ranox, Elysion,
  Karuso) are skipped and listed. Dungeon / field world scripts are out of scope.
- **Census first** (`june2017_re/w3_village_lua.tsv`): per file the March name, June's name (case can differ), June-only or March-only, and its closure: every
  mesh / texture / line map / sound it names that March's kom_contents lacks (stage June's copy, as `npc_mesh_census.py --seed` does), every world-object Lua
  method or key March does not bind (route, do not edit), and line-map ids / start positions that June moved (village spawn and portal positions live in
  `WorldData` / village data, so a moved line map must still land the player and NPCs).
- **Adopt, pack, ask, copy** (`/elstools pack data040`, then ask before `/x2-kom-packed copy`).
- **Gate:** each adopted village loads with no new missing-file, Lua or `KEM_ERROR` line and State.log lists its meshes; in Besma Village
  the horse fades (R13b's check 1, `WSKIN hide: BESMA_VILLAGE_HORSE.X`).

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. Record june-effect C11's result as M5 (which Stage 0 effect items closed), re-run
`ui_dlg_census.py`, update `JUNE_PORT_BACKLOG_ORDER.md` Stage 2, `MODS.md` (every flag + revert), and memory.

## Funcmap rows written by planning

Tagged `Engine S2` in `june2017_re/funcmap.tsv`: `31d3f480`, `337299c0`, `33729a58`, `31c86270`, `31d0b200`,
`32801580`, `327fc8c0`, `328004a0`, `32800e10`. R0 / W0 confirm or correct them.

## Open questions (planning, 2026-10-16)

None blocks a phase today. Decided by planning, recorded here:

1. **Asset-port P10 waits for R2** (dated note in `JUNE_ASSET_PORT_PHASES.md` P10 and its STATUS row). Copying June's
   `XRenderer.fx` before R2 renders a configuration June never shipped (no environment box, edge or mipmap paths, rim light off).
2. **The 7 "missing render types" are dropped as a work item.** They are `SKIN_MESH_RENDER_TYPE` (R6 ports the 6 unimplemented
   cases); backlog fact 7 carries a dated correction.

Open for the user:

3. **R1's ramps.** R1 needs the 10 June-only `Cartoon*.bmp` from June's data042, which asset-port P4a ships several waves later.
   Either pull just those 10 forward as a small data042 stage (through `kom.py stage-june`, pack, ask before copy) when R1 runs,
   or hold R1 until P4a is live. **Recommendation: pull forward**: 10 inert files, no consumer until R1, and it keeps the
   renderer chain (R1 -> R2 -> asset P10) off the asset waves' timeline.

Deferred to R0 (each comes back to the user with a recommendation):
- what `+0x58` is; if it is a June graphics option, its option UI goes to Stage 9 and R2 picks the default June ships;
- whether the environment box (R3) and heat shimmer (R4) have any reach in June or March data; with none, recommend porting them
  anyway behind their flags (June parity, memory `plan-unowned-june-branches-without-asking`), or dropping, for the user to pick.

Answered by R0 (2026-10-16). `+0x58` is device caps only. Heat shimmer has reach (R4 does an RE step first). Two questions remain
for the user:

4. **Rim light needs a June graphics option, and June ships it off.**
   - In June, rim light draws only when `RIM_LIGHT = TRUE` in `GameOptions.lua`. The default is `FALSE` (data002), and the June
     option dialog has a toggle for it.
   - March has neither the key nor the UI. So R1/R2 with June's code verbatim *remove* the Ranox rim light that the stand-in shows
     today.
   - With the option on, June's world default (rim index 0, clamped to 13) puts a white rim on every cartoon unit in every world.
   - **Recommendation:** port June's four option keys (`RIM_LIGHT`, `USE_VIEW_INDEPENDENT_EDGE`, `ANISOTROPIC_FILTER_SAMPLE`,
     `USE_ENV_BOX`) into March's option load and save in R2, with June's defaults. The user sets `RIM_LIGHT = TRUE` in the game
     dir's `GameOptions.lua` to play-test, and Stage 9 adds the toggle.
   - The alternative is a labelled default-on stand-in flag until Stage 9.
   - **Answered (user, 2026-10-16): follow June.** R2 ports the four option keys with June's defaults (`RIM_LIGHT` off); no
     default-on stand-in. Rim light shows only with `RIM_LIGHT = TRUE` in the game dir's `GameOptions.lua`; Stage 9 adds the toggle.
5. **Environment box and colour grading have no reach.**
   - Nothing in June's data creates an env box: no Lua calls `SetEnvBoxMap` or `g_pEnvBoxMap`. The only other trigger is the dev
     command `/useEnvBox`.
   - **Recommendation: drop R3's `CKTDGEnvBoxMap` / `XEnvBoxRenderer.fx` port.** R2 still ports the renderer side verbatim, so
     `+0x80` / `+0x59` stay false and the env-box rim and grading branches stay off, as they are in June. A port could not be
     play-tested.
   - The alternative is to port it behind `SERV_IRUHADEV_JUNE_RENDER_ENVBOX` for parity, untestable.
   - **Answered (user, 2026-10-16): follow June.** June ships the env box engine, so R3 ports it whole behind
     `SERV_IRUHADEV_JUNE_RENDER_ENVBOX`, inactive as in June (no data creates a box). Its check is a build plus a log line, not a
     visual play-test.

## Play-test hand-off (every code phase)

Dev console first (auth level 3, END key); copy-ready commands with real values; per check: where / paste / what it does / look
for / if not. The log tag is `[JUNE_ENGINE]` plus the phase's sub-tag.
