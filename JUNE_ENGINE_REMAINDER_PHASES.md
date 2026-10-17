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
| R4 | Heat shimmer (`XHeatShimmerRenderer.fx`) | Sonnet 5.5 high + review | yes | yes |
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

## Phase R2 - XRenderer contract

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT`. Sub-tag
`[JUNE_ENGINE] FX`.

- June's four macros at `D3DXCreateEffect`, with `+0x58` as R0 maps it (log the chosen path once).
- The 33-entry parameter table and June's setters (R0 item 2), `RenderParam`'s rim-light members and their Lua readers.
- `COLOR_WIREFRAME` only if R0 finds a June caller.
- Works with March's `XRenderer.fx` too (the new handles are NULL: setters skip), so it can land before asset P10.
- **Gate for asset P10:** after R2 is reviewed, P10 copies June's file.
- **Play-test:** after P10, a village, a dungeon and character select (P10's own list), plus one rim-lit unit.

## Phase R3 - Environment box and colour grading

**Run with:** Sonnet 5.5 high (Opus 5.5 high if R0 leaves gaps), then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_ENVBOX`.
Depends on R2. `XEnvBoxRenderer.fx` techniques `BlendLUTTechnique`, `DrawEnvBoxTechnique`; the rim-light env-box parameters in
`XRenderer.fx`. Port the owner R0 maps, its data and option. If R0 measured no reach, the row asks the user first.

## Phase R4 - Heat shimmer

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RENDER_HEAT_SHIMMER`. Port the owner of
`31d0b200` (`HeatShimmerTechnique`) and its callers, per R0. Reach rule as R3. **R0: the owner is `DGManager +0x384`
(OnResetDevice `31ce5e90`); `31d0b200` is the env box. Reach is gated, not 0: see R0 map item 7 and the R4 kickoff.**

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

## Phase R12 - Skin-anim device ownership

**Run with:** Opus 5.5 high (map, then port), then an Opus review and a user play-test. Flag `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS`.
Added by R10; rows with owner R12 in `june2017_re/r10_skinanim_sweep.tsv`. After R11; not alongside R6.
- **Map first:** each March caller of `AddModelXSkinMesh` (the `bCloseDeviceAtDestructor` cases), `RemoveModelXSkinMesh`,
  `ReplaceMultiTexXET( int )`, `SetAnimXSkinMesh` and `ClearModelXSkinMeshList`, against June's call site. The goal: no reference
  closed twice and none leaked. Also March's device AddRef (`+0x48` is a plain int in March; `JuneCloseDeviceByPointer` is ported, Effect C9h-7a).
- **Port** the ownership model whole: the R12 rows, `CleanAnimStructure( mesh, xet )` and the dtor.
- **`AddModelXSkinMesh`:** `( file, line )` and the KEM 433 log.
- **`31d2f250` and its two `CX2GUUser` calls:** port per June once the map shows how June's motion collision meets March's lists.

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

## Phase W1b - AUTO_DISABLE_LINE

**Run with:** Opus 5.5 high (map, then port), then an Opus review. Flag `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE`. Map first:
the second-candidate selection in `CX2GUUser::PhysicProcess` `@320195be-32019a10` and the NPC fallback `@32782deb-32782ea4`. Then
port, from the funcmap rows: the `LineData` fields (`+0x128` flag, `+0x12c` user UIDs, `+0x13c` NPC UIDs), the parse, the line tail,
`31c6b8e0`, the unit `+0x3e48` bookkeeping and both physics gates. Test case: `Ranox_FireTemple_StageBoss`, lines 3 and 4.

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
