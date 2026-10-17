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
**Open:** R2 review (Opus); asset P10's copy of `XRenderer.fx` is done by this.

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
**Range check** (`flagcheck.py <flag> --range 915bb24b^..915bb24b`): FRAMEWORK `files: 3 PASS, 18 OTHER, 1 FAIL`; NATION_KEYS `2 PASS, 19 OTHER, 1 FAIL`; MOVIE_SUBTITLE `1 PASS, 20 OTHER, 1 FAIL`; LINE_BREAK `5 PASS, 16 OTHER, 1 FAIL`; DEBUG `0 PASS, 21 OTHER, 1 FAIL`. Every `in this flag's files:` line is `0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change`; each `RESULT: FAIL` is the one row `KTDXLIB/KTDXWordWrap.cpp` (new file, `#include "StdAfx.h"` first for the PCH, the rest inside `SERV_IRUHADEV_JUNE_UI_LINE_BREAK`). OTHER = the same files seen through another U1 flag.
**Census** (`ui_dlg_census.py`, run against the deployed exe): `GAP LIST june-only-unbound: 0 names`, `type-aware misses: 0` (was 37 names / 13 misses). The census learned the suffix rule (`FONT_INDEX|POS|LEFT_TOP|RIGHT_BOTTOM|RIGHT_TOP|LEFT_BOTTOM|ADD_SIZE_X|Y` + `_GLOBAL` / `_US` = bound; other nation codes, `IS_FOR_MINOR_UNIT`, `m_PrivateType` and the `AddVillageNpcRelationData*` table keys = `routed`), edited in `ui_dlg_census.py`.

**Open / for the review.** (a) No live March DLG calls any of the new methods (kom_contents grep, 0 hits): June DLGs bring them with their UI ports, so A can only be shown by log lines and by a regression
that default-on `SetChangeFocusByKeyMsg` changes nothing. (b) Live exercisers: `DLG_Map_Local_*_Icon_NEW` read `*_GLOBAL` / `*_US` keys (B), the movies (C), every X2Lib text wrap (D).
(c) The done gate "a June DLG from each group opens in game" is **not met by live content**: it needs a June DLG calling the new methods, and the owning UI port brings it. The play-test checks the groups that have a live exerciser plus the log lines.

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
