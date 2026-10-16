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
| `XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx` | **June-only** (contradicts the 2026-10-08 asset census, which counted no June-only `.fx`) | none in March. June: `XEnvBoxRenderer.fx` string `3344dad0`; `XHeatShimmerRenderer.fx` loader `31d0b200` |
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
- `RenderParam` gains a rim-light member (Lua key string `useRimLight`, `3353ca10`).

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
| R6 | `SKIN_MESH_RENDER_TYPE` 1, 2, 4-7 | Sonnet 5.5 high + review | yes | yes |
| U0 | UI framework RE: June's implementation of each U1 gap | Opus 5.5 high | no | no |
| U1 | UI framework port | Sonnet 5.5 high + review | yes | yes |
| W0 | World-event timeline RE: loader, data, reach | Opus 5.5 high | no | no |
| W1 | World-event timeline port (conditional on W0) | Sonnet 5.5 high + review | yes | yes |
| Z | Close-out: M5 from C11, backlog, MODS.md | Sonnet 5.5 medium | no | no |

**Model choice.** The RE phases (R0, R0b, U0, W0) decide what June does from the binary and settle new mechanisms: Opus, where a
wrong reading costs a whole port. The code phases transcribe a written map: Sonnet high, each with a scoped Opus review because the
renderer fails visibly but subtly (a wrong blend or ramp index looks "almost right"). R3 moves to Opus if R0 cannot map the
environment box fully.

**Order.** R0, R0b, U0 and W0 can run now, in parallel (read-only). Code: R1 -> R2 -> R3 / R4 / R5 / R6 (R3 needs R2; the others
need only R0 and touch different files, but run one at a time: they share `KTDGXRenderer.*` / `KTDGXSkinAnim.*` and the build).
U1 runs after U0, independent of the R chain apart from the build lock and `KTDXApp.cpp` (also touched by Atlas A4: check STATUS
Touches). W1 after W0.

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

## Phase R0b - Render and skin-mesh sweep

**Run with:** Opus 5.5 high. No code.

The backlog's "any other June engine-side difference in renderer or skin-mesh code March already has". Compare June with March
function by function for `CKTDGXRenderer`, `CKTDGXSkinAnim` (render paths), `CKTDXDeviceXSkinMesh` (load), `CKTDGXMeshPlayer`
render, `CKTDGParticleSystem` render, `CKTDGPostEffect`, `CKTDGGrayScale` and the `CKTDGManager` render lists, excluding what R0
and earlier june-effect phases already mapped (funcmap `Effect *` rows). Each difference gets a funcmap row and either a new phase
here (R7+) or an owner. **Done gate:** every function of the list has a verdict row (same / differs -> phase / owner).

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
`31d0b200` (`HeatShimmerTechnique`) and its callers, per R0. Reach rule as R3.

## Phase R5 - CUSTOM_RENDER_STATE

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_CUSTOM_RENDER_STATE`.

- June's table in `Enum.lua` (fixes the Add / Ara truncation, checklist `:152`); keep March's `X2GUAdd.h` enum separate.
- The engine reader(s) and render behaviour per R0 item 6, for units, NPC and riding pet (`X2RidingPet.cpp:1404`).
- Class-specific use (Add, Ara, Ain, Lu/Ciel, Rose) goes to the checklist; on March data only `Ara_Martial_Artist` and the Add
  script reach it once those scripts are adopted.
- **Play-test:** whatever March-data path R0 finds; if none, a dev-console Lua call on the player's unit.

## Phase R6 - SKIN_MESH_RENDER_TYPE

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKIN_MESH_RENDER_TYPE`. June's cases 1, 2, 4-7
(R0 item 5) in `KTDGXSkinAnim`'s typed pass, next to the existing UV_ANIM case.

## Phase U0 - UI framework RE

**Run with:** Opus 5.5 high. No code. Funcmap rows `Engine U0`.

For each U1 gap in the M4 table: June's binding and implementation (address), the C++ member(s) it needs, and its March
equivalent if any. Settle `ROTATION` (live in June's client, or `DLL_BUILD` there too). Settle which nation suffix June's US build
reads for `POS_<CC>` / `FONT_INDEX_<CC>` (US = `NF_US`, memory of C9h-4's nation reader). Re-run `ui_dlg_census.py` against the live
tree and record the gap list. **Done gate:** every gap has an address and a one-line port spec in U1's kickoff, or is routed.

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

## Phase W1 - World-event timeline port

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_WORLD_EVENT`. Conditional on W0. The type-1 sound uses
the device auto-release that june-effect C9h-7 settled.

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

## Play-test hand-off (every code phase)

Dev console first (auth level 3, END key); copy-ready commands with real values; per check: where / paste / what it does / look
for / if not. The log tag is `[JUNE_ENGINE]` plus the phase's sub-tag.
