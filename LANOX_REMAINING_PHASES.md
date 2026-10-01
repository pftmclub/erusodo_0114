# Lanox Remaining Phases

Ten Lanox-specific items still open after `LANOX_DEFECT_PHASES.md`, found by
play-testing at ~80% port completion (user report, 2026-09-30; items 10-11,
Phases 9-10, were reported later the same day). This document
is self-contained. Paste the "Starting context" block plus any one phase into
a fresh conversation. The not-Lanox-specific item (June Local Map / Party UI
parity) is in `JUNE_UI_PARITY_PHASES.md`, and two items here depend on it
(Phase 3, and half of Phase 2).

**Do not re-open** `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md`,
or the `mods/lanox-offline` branch.

**Fix philosophy, stated by the user:** fixes stay as close to the
original/studio code as possible. No band-aids: no hardcoded tables, no
invented curves, and no guessed values standing in for a file or a code path
that should actually be read. **No partial June updates:** if June changed a
function March already has, port the whole June function (diff every case),
not just the Ranox case. June-only cases for content March lacks (Elysion,
etc.) are omitted with a comment and never invented.

**Asset-conflict priority, stated by the user, 2026-10-01:** when a shared
atlas/asset file has content in June that a Lanox fix needs, and live March's
copy of that same file also carries *other*, unrelated content at the same
name — adopt June's file wholesale. Don't merge/splice to preserve March's
existing content. June's asset wins; do not spend effort preserving the
March-only content in that file. (Established on Phase 1's `Image11`: live
`datai022.kom`'s copy carried 4 unrelated, unreferenced event-icon keys;
per this rule the fix now replaces the whole file with June's 8-key version
instead of block-copying just the new key in.)

**Corrected 2026-10-01, same session, after Phase 11.** The first write of
this rule drew the line in the wrong place: it treated "June redrew some
pixels March also uses" as a reason to hold back and merge instead of adopt
(see Phase 11's `DLG_BossState.tga`, where this was tried and reverted). That
is not the actual hazard. **The only case that needs a merge instead of a
wholesale adopt is when June's copy of the file is missing a key March reads,
or has moved it somewhere the art doesn't match** — i.e. adopting would make
a March lookup fail or point at the wrong picture. Redrawn-but-still-present
keys are exactly what "June wins" means: the art changes to June's, which is
the point. Before ruling out a wholesale adopt, check *that* — every key the
in-scope March lua/`DungeonData`/etc. reads is present in June's file at a
UV that still has real art backing it — not whether any pixel changed.
Phase 2's `DLG_Localmap_Title_01.tga` was flagged under the old, wrong
version of this rule; it has not been re-checked against the corrected test
and nothing has been decided there — re-verify it under this definition when
Phase 2 is picked up, rather than assuming either answer.

**Model, effort and review per phase.** The kickoff prompt of each phase is
preceded by a **Run with:** line; the table below repeats it.
- **Model:** Opus 5.5 (`claude-opus-5-5`) for anything that needs Ghidra
  reverse engineering, a whole-function C++ port, or an open-ended root-cause
  hunt. Sonnet 5 (`claude-sonnet-5`) for data staging, table diffs and scripted
  checks whose method is already written down here. Haiku 4.5
  (`claude-haiku-4-5-20251001`) only for mechanical cleanup.
- **Effort:** how much thinking to allow (low / medium / high). High where a
  wrong guess is expensive: enum values, render paths, anything a play-test
  would only catch late.
- **Review:** *required* means that before building/packing, a second pass
  (a fresh session, or a review of `git diff`) checks the change against
  June and the house rules: `SERV_IRUHADEV_` gating with the original in
  `#else`, CP949 bytes intact (`file` + `git diff --stat`), enum values
  matching across C++ / Lua / `XSLLineMap.h`, no invented data. *Light* means
  a diff read of the data change. *None* means read-only or trivial; the
  live-kom check and the play-test are enough.

| Phase | Item | Depends on | Kind | Model | Effort | Review |
|---|---|---|---|---|---|---|
| 0 | TET-key census (all Ranox UI) | - | read-only | Sonnet 5 | medium | none (read-only) |
| 1 | Dungeon image on result screen / party list | 0 | data | Sonnet 5 | medium | light |
| 2 | Local-map dungeon names (+ progress bars via June-UI P3) | 0 | data | Sonnet 5 | high | required if merging |
| 3 | Lanox tab in the dungeon list | June-UI P2 | cleanup + verify | Haiku 4.5 | low | none |
| 4 | World map: June tiles, Ranox labels | 0 | data (+ maybe lua) | Sonnet 5 | medium | light |
| 5 | M-key field map follows the character | - | C++ (Ghidra) | Opus 5.5 | high | required |
| 6 | Hide Secret Hell | - | data (+ maybe C++) | Sonnet 5 | high | required if C++ |
| 7 | Distant Ruin demon colours | - | C++ (Ghidra) | Opus 5.5 | high | required |
| 8 | Burned Forest background | - | investigate first | Sonnet 5 → Opus 5.5 | low → high | none → required if C++ |
| 9 | Silent Night Shelter (rest area) has no NPCs | - | data | Sonnet 5 | medium | light |
| 10 | Fire Temple collapsing floor leaves the player stuck | - | data (C++ already landed) | Opus 5.5 | high | required |
| 11 | Ranox boss face missing from the boss HP bar | - | data | Sonnet 5 | medium | required if merging |

---

## Starting context (paste this with any phase)

- Repo root: `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- Branch: `mods/lanox-port`
- Game directory (deploy target, logs, save):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- Client build (default `US_SERVICE`, see `CLAUDE.md` *Toolchains*):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  TRUNK="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
  DATA="F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data"
  MP="-m -p:MultiProcessorCompilation=true"
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL      -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  ```
  If any header changed (`Always.h`, `X2Define.h`, etc.), run
  `touch X2Lib/stdafx.cpp` before rebuilding. Otherwise the PCH silently
  discards the header edit. The deploy artifact is `X2/US_SERVICE/x2.exe`
  (lowercase). Copy it to `$DATA/X2_offline.exe` and confirm the copy
  programmatically (size and mtime). **Then ask the user before launching,
  every time.**
- Standing constraints (`CLAUDE.md`):
  - Gate every client C++ change behind a `SERV_IRUHADEV_*` flag declared in
    `KTDXLIB/Always.h`. Use the house comment-block style and the
    `#endif SERV_IRUHADEV_FOO` trailing token, and keep the original code in
    `#else`. Reuse `SERV_IRUHADEV_RANOX_PORT` only for Ranox *case additions*.
    A whole-function June port gets its own flag.
  - CP949/ISO-8859 source files: edit at byte level (Python `rb`/`wb`). After
    every edit, verify with `file <path>` and `git diff --stat`.
  - Every `.lua` goes through `elstools compile-batch -v mar-2014` before
    packing. `.src.lua` and `.std.luac` never ship inside a `.kom`. Pack rule:
    `.lua` is algo 2, everything else algo 0 (`_manifest.json` values; CRLF,
    2-space indent, no trailing newline).
  - Packing is the user's job. Verify against the **live** `.kom`
    (`elstools inspect -q -i <kom>` / extract), never the `kom_contents/`
    staging dir.
  - Staging: patches to existing March files go in `march2014/dataNNN/`. New
    files go in `march2014/update/dataNNN/`, keeping June's archive number.
  - **The user's packer builds each kom from
    `kom_contents/dataNNN_kom/` (its `_manifest.json` included), not from
    `march2014/update/`** (proven 2026-09-30: data023 was repacked with the
    smog texture only in `update/`, and came out byte-for-byte the same size
    with 547 files). A new file therefore has to reach
    `kom_contents/dataNNN_kom/` and that dir's manifest as well. Name both
    places in every pack hand-off, and confirm the result with
    `elstools inspect` on the live kom, not by the pack's timestamp.
  - Never launch `X2_offline.exe` without asking. Never delete `els_db.sql`.
    Kill processes by PID only, never by image name.
- Logs, all in the game directory: `ErrorLog.txt` (JUNE_FX / KEM lines),
  `offline_server.log`, `offline_packets.log`, `State.log`,
  `OpenScriptFile.log`.
- Reference trees:
  - March 2014 extract: `...\237311\24965799\kom_contents\dataNNN_kom`
  - June 2017: `...\237311\14JUN17\data\dataNNN_kom` (`.src.lua` decompiles
    sit alongside, but these predate the decompiler opcode fix — prefer
    this repo's `june_src_fixed/data###_kom/<name>.src.lua` when the file is there)
- June binary: `14JUN17/data/unpacked_x2.exe`, loaded in the Ghidra project
  `F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`. Drive it with the
  `ghidra` skill (curl with `--noproxy '*'`, imagebase `0x31aa0000`). Lua key
  strings are XOR-encrypted per string (`june2017_re/xorstrings.tsv`,
  `xordec*.py`). Dialog file names are **plain** wide strings
  (`strings?filter=`).
- TET atlas merge technique (KTETFormatHeader, sorted key-offset array, UV
  array, wide-string table; the TGA is bottom-up, so a pixel copy needs a
  `height-1-y` flip) is written up in `LANOX_DEFECT_PHASES.md` Phases 4, 5 and
  7c. Re-read it before any atlas merge.

---

## Investigation ledger (proven 2026-09-30, do not redo)

- **Item 2.** `DungeonData.lua` (`march2014/data035/DungeonData.src.lua`,
  Ranox blocks at about lines 13292-14768) sets `m_TextureName =
  "DLG_UI_Dungeon_Image10.dds"` with pieces `Ranox_Ashes_Village`,
  `Ranox_Burned_Forest`, `Ranox_FinalHall`, `Ranox_FireTemple`,
  `Ranox_Geyser` and `Ranox_Gateway`. Secret Hell uses
  `DLG_UI_Dungeon_Image11.dds` / `RANOX_MAGI_pit`.
  - Consumers: `X2DungeonManager.cpp:373-375` (parse),
    `X2StateDungeonResult.cpp:2254`, `X2PartyUI.cpp:2194`
    (`GetPicture(0)->SetTex`).
  - Live `data065.kom` has `DLG_UI_Dungeon_Image10.dds` but no `.TET`, and no
    `Image11.*`.
  - June `data065_kom` has `Image10.TET` (632 B), `Image10.dds` (349680 B),
    `Image11.TET` and `Image11.dds`.
- **Item 4 (names).** Ranox's icon file
  (`march2014/update/data217/DLG_Map_Local_Ranox_Icon_NEW.src.lua`, pulled in
  by `..._Back_NEW` via `IncludeLua`) calls
  `SetTex("DLG_Localmap_Title_01.tga", "Ranox_Ashes_Village" | "Ranox_Burned_Forest" | "Ranox_FINALHALL" | "Ranox_FIRETEMPLE" | "Ranox_GateWay" | "Ranox_Geyser" | "Map_Title_Ranox")`.
  - March `kom_contents/data063_kom/DLG_Localmap_Title_01.TET` (4138 B) has
    only `TITLE_*` keys.
  - June `data151_kom/DLG_Localmap_Title_01.TET` (6050 B) has every `RANOX_*`
    key plus `MAP_TITLE_RANOX`, as well as Elysion and other extras.
  - Both TGAs are 1048620 B and differ. June also ships
    `DLG_Localmap_Title_02` and `datai021/DLG_Localmap_Title_INT_01`.
  - Sander's names come from a different mechanism:
    `DLG_Map_Local_Sander_Icon_Info_NEW.lua` (`DLG_UI_Title03/04.tga`
    pictures), looked up per dungeon by `X2LocalMapUI.cpp` (~680-682, 820,
    935, `CreateDungeonClearStars` ~955). June has **no** `*_Icon_Info_NEW`
    files.
- **Item 5.** World-map tiles live in `data057`:
  - March: six `DLG_UI_World_Map0N.dds`, all DXT1 512x512, 131200 B, no mips.
  - June: tiles 01-02 are *uncompressed* 512x512, 786560 B, no FourCC. Tiles
    03-06 are DXT1 with 10 mips, 174904 B.
  - All six `.TET` files (78 B each) and `Cloud_Ranox.tga/.TET` are
    byte-identical.
  - The cloud logic is generic. In `CX2WorldMapUI::UpdateWorldMap`
    (`X2WorldMapUI.cpp:603-660`), a static whose `DummyInt(0)` is the
    village's VMI and whose `DummyInt(1)==1` is a cloud. It is hidden once the
    village button passes `m_RequireUnitLevel`. So the cloud-covered Lanox is
    baked into March's tile art.
  - June's world-map lua also has `Cloud_Elision` / `ELYSION_*` controls.
  - The world-map labels use `DLG_UI_Common_Texture71_NEW.tga` keys
    `TOWN_RANOX` and `FIELD_NAME_RANOX_*` (a known-open atlas in
    `LANOX_TROUBLESHOOTING.md`, `data057`).
- **Item 1.**
  - Path: `X2State.cpp:4293` (DIK_M) → `OpenWorldMap` → `OpenFieldMap`
    (`X2WorldMapUI.cpp:904`, creates `CX2WorldFieldMap`).
  - `GetFieldMapDlg` (684-882) already has Ranox cases.
  - The following have **no** Ranox cases:
    - `CX2FieldMapBase::ResetCameraMatrix(int)` (`X2FieldMapBase.cpp:120-445`,
      per-VMI eye/look-at/fov/aspect, e.g. Ruben
      eye(1274.792,1125.527,-5136.493))
    - `ResetMapPosAndSize(int)` (~475-535)
    - `UpdateNaviMap` (`X2WorldMapUI.cpp:2178`, per-VMI
      `pNavi_->SetBGTex(L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_<X>")`)
    - `X2FieldMiniMap.cpp:318` `ResetWholeMapTexture`
  - `ReLoadingVillageMap` (2437) loops stop at `VMI_SANDER` /
    `..._SANDER_REST_00` / `..._SANDER_FIELD_04`.
  - `IsVillageField()` (`X2LocationManager.h:487`) is bounded the same way.
- **Item 7.** Secret Hell (`DI_RANOX_SECRET_HELL` = 41271) was fully ported by
  RANOX_PORT_PLAN Phase 9 (2026-09-18): dungeon def, 14 stage/LineMap files,
  28 `NUI_SECRET_*` units, `DLG_UI_RANOX_SECRET_FEVER_GAGE.lua`, WorldData
  `WI_RANOX_SECRET_*` 676-684.
  - Exposed in `DLG_Map_Local_Ranox.lua`'s `DungeonIDList`, and through the
    `Ranox_Secret` button (`DLG_Common_Button04_NEW` `Ranox_SECRET_DUNGEON_N/O`)
    and the `SecretDungeonDesc` button (`Button05_NEW`
    `Ranox_SECRET_DUNGEON_LOCK`) in the icon lua.
  - March's `IsSecretDungeon` agrees with June's classifier `FUN_32931950`
    (LANOX_DEFECT_PHASES).
- **Item 8.** Distant Ruin (`VMI_BATTLE_FIELD_RANOX_FIELD_04`,
  `WORLD_ID = WI_BATTLE_FIELD_RANOX_FIELD_03`, spawns from
  `Ranox_Field_03.lua`):
  - The spawn list is identical to June's (`NUI_LOW_FINALHALL_{FIGHTER,BERSERKER,SNIKER,FIREBIRD,STONE}_FD`,
    `..._IGNIA_BOSS`).
  - `NPCTemplet` rows are identical. The unit luas are identical, apart from
    the documented Berserker decompile fix.
  - The `.xet` files and every `NUI_FIRE_DEVIL_FIGHTER_*` TGA are
    byte-identical and present in live `data153.kom`.
  - Structure:
    - The main skin `Motion_FIRE_DEVIL_FIGHTER.x` references `..._Body_A.tga`
      and has `MOTION_CHANGE_TEX_XET = LOW_FINALHALL_FIGHTER_FD.xet`
      (`INCHANT_BODY_A→A_FD_01`, `INCHANT_BODY_B→B_FD_02`).
    - `ATTACH_SKIN_MESH0` is `Motion_FIRE_DEVIL_FIGHTER_Body.x` (Body_A,
      Body_B), with its own `MOTION_CHANGE_TEX_XET = Motion_FIRE_DEVIL.xet`
      (`FIGHTER_BODY_B→INCHANT_BODY_B`) and `SMRT_UV_ANIM`.
    - The Berserker has the same shape.
  - March's `SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH` port
    (`X2GUNPC.cpp:4442-4479`) reads only `ATTACH_MESH_NAME` and
    `SKIN_MESH_RENDERTYPE`. It **ignores the block's `MOTION_CHANGE_TEX_XET`**
    and never applies the unit's `_FD.xet` to the shell.
  - `X2SubEquip.cpp:159-178` forces `RT_LIGHT_FLOW_LOCAL_Z`, which its own
    comment says "replaces rather than overlays" the cel shading.
  - Live `data153.kom` also holds `Motion_FIRE_DEVIL_FIGHTER_INCHANT.x`, which
    no mesh checked so far references.
  - **User screenshot (2026-09-30, Distant Ruins, 2 Fighters + 2 Berserkers
    fought and killed):** the bodies render dark/black. The spikes and shell
    render as a bright white-yellow glow. The one clearly blue element is a
    flame effect on a Berserker attack, which is an effect, not the body.
    This fits the forced `RT_LIGHT_FLOW_LOCAL_Z` shell, but it doesn't prove
    it. The PNG ground truth (Phase 7 step 1) is still needed.
  - **`ErrorLog.txt` from that fight** (20:02:11) has two `OpenTexture` misses:
    `FIRE_DEVIL_BERSERKER_INCHANT_AttackA_Ready_Fire.dds` and `..._01.dds`.
    Both are staged but unpacked; see the next bullet.
- **Staged but never packed: `march2014/update/data002/` (found 2026-09-30).**
  I compared every `march2014/update/dataNNN/` folder against its live
  `.kom` (`elstools inspect`). Only `data002` has gaps: 65 of its 77 staged
  files are live and **12 are not**, although all 12 are in its
  `_manifest.json`.
  - The missing 12:
    - `DEVIL_BERSERKER_Dying.X`
    - `DEVIL_BERSERKER_INCHANT_Dying.X`
    - `ELSION_BOSS_HERNASHIELD_ATTACK_ROCK.DDS`
    - `FIRE_DEVIL_BERSERKER_INCHANT_AttackA_Ready_Fire.dds`
    - `FIRE_DEVIL_BERSERKER_INCHANT_AttackA_Ready_Fire_01.dds`
    - `FIRE_DEVIL_CHAIN_STONE_Attack_A.xet`
    - `IGNIA_AttackA_FireM1.X`
    - `IGNIA_AttackC_Ready01.Y`
    - `LOW_FINALHALL_STONE_FD_01.xet` (the Distant Ruin stone's FD colour
      swap)
    - `MAG_STONE_FD_01.xet`
    - `MAG_STONE_Fire_AttackB.X`
    - `MOUNT_MAG_STONE_Dying.X`
  - All 12 are byte-identical to June's `data002_kom`.
  - Timeline: they were staged 2026-09-23 05:52-05:53. The live `data002.kom`
    was packed 2026-09-22 20:41, so they missed that pack.
  - The `kom_contents/data002_kom` extract *does* contain them, which is
    exactly why only the live archive can be trusted.
  - **Fix: the user repacks `data002.kom` from `march2014/update/data002/`.**
    The manifest is already correct.
  - **Resolved 2026-09-30.** data002.kom was repacked at 20:21:54 and now
    holds 1248 files. All 77 staged files are live at their staged sizes
    (checked with `elstools inspect`).
  - The other non-`.kom` staging dirs (`eventscene`, `movie`, `music`) are
    loose folders and are out of scope for this check.
  - `march2014/dataNNN/` patch files (as opposed to `update/`) were not
    compared.
- **Item 9.** `Ranox_BurnedForest_Stage{0,1,2,Boss}.src.lua` (WorldData
  `WI_RANOX_BURNDEDFOREST_STAGE0..STAGEBOSS` = 583-586) match June's
  byte-for-byte (CR-stripped).
  - Every method they call (30 of them, including `SetFogWorld`,
    `SetOriginalColorFactor`, `SetFeedBackUV` and `SetTex0UVSpeed`) has a
    March binding.
  - All referenced assets exist in staging, and the meshes are byte-identical
    to June's.
  - **Live-kom check (2026-09-30, `elstools inspect` on every live kom):**
    all 126 assets the four stage luas name (meshes, `rain-*.tga`,
    `WorldLayer_Velder_Gate.tga`, the LineMap luas) are live, at sizes equal
    to staging and to June.
  - **Correction: "every mesh-internal texture is present" was wrong.** The
    `.Y` files are MSZIP-compressed DirectX (`xof 0303bzip`). Decompressing
    them shows 11 meshes reference `BossRaid_Abyss_smog.dds`, which is in no
    live kom and not in March's `kom_contents`. June has it in
    `data023_kom` (87507 B). The 11 are every cloud/smoke/smog layer in all
    four stages:
    - `Fire_forest_stage0_AP_Cloud01-03`
    - `Fire_forest_S1_Apha_smog01-04`
    - `Fire_forest_s2_AP_smoke`, `_smoke02`
    - `Fire_forest_stageBoss_AP_Clo01-02`
    `ErrorLog.txt` independently logs `OpenTexture failed!
    BOSSRAID_ABYSS_SMOG.DDS` at 20:08:15, on dungeon entry, and it is the
    only texture miss at load.
  - Two Stage 0 meshes (`Alpha_stem02`, `Tree03`) would not fully decode
    with the scratch MSZIP reader. They are byte-identical to June and the
    engine loaded them without error, so this is a limit of the reader, not
    of the files. Their textures were covered by the log check instead.
  - **User screenshot (2026-09-30, Burnt Forest Normal, Stage 0 area with
    Angry Gripigs):**
    - The upper background (grey fog, dead trees, red smears) looks right.
    - A second, lower layer is the anomaly: a brown/orange band and dark-red
      smeared shapes behind the platforms. That fits the `AP_Cloud` meshes
      (sky layers `XL_SKY_WORLD_BASE_0/1/2`) drawing with no texture.
    - This is **likely, not proven**. Settle it by play-test after repacking.
  - **Staged 2026-09-30:** `march2014/update/data023/BossRaid_Abyss_smog.dds`
    (`cmp`-identical to June). It is the first entry in `_manifest.json`,
    now 51 entries (algo `"0"`, upper-case ordinal order, CRLF). The other 50
    staged data023 files were already live at matching sizes. Repacking
    data023.kom adds only this file.
  - **Repack attempt, 2026-09-30 20:22 — the texture did not get in, so
    the fix is still untested.**
    - data023.kom was rewritten at 20:22:05. It still holds 547 files at
      15011767 B, exactly as before, and `BossRaid_Abyss_smog.dds` is not
      in it.
    - Cause: the packer reads `kom_contents/data023_kom/`, and the texture
      was only staged in `march2014/update/data023/` (see "Starting
      context").
    - `ErrorLog.txt` logs the same `BOSSRAID_ABYSS_SMOG.DDS` miss at
      20:23:24.
  - **Second user screenshot (2026-09-30 ~20:23, Burnt Forest Normal, same
    stage), taken with the texture still missing:**
    - The upper layer (grey fog, dead trees, red smears) is still fine.
    - The lower band is still there: dark-red blotches, and centre-right,
      behind the player's platform, flat rectangular teal / orange / brown
      patches that look like an untextured or wrongly-textured quad rather
      than scenery.
    - The user reports the issue as unchanged. This is consistent with the
      smog texture never having loaded; it is **not** evidence against the
      smog cause.
- **Item 10 (Silent Night Shelter has no NPCs).** Read-only investigation,
  2026-09-30, against staging and June. The live koms have not been checked
  yet.
  - The rest area is `VMI_BATTLE_FIELD_RANOX_REST_00`, reached through
    `DLG_Map_Local_Ranox.lua`'s `BattleFieldRestData`
    (`march2014/update/data217/DLG_Map_Local_Ranox.src.lua:20-22`).
    `X2LocationManager.cpp:326-327` feeds that into `VillageParsing`.
  - `march2014/data034/DLG_Map_Village.src.lua:4056-4065` gives it
    `HouseIDList = { HI_GLAVE_HOUSE, HI_HELLEN_SHOP_HOUSE, HI_BILLBOARD,
    HI_POSTBOX }` and `WORLD_ID = WI_BATTLE_FIELD_RANOX_FIELD_REST_00` (606,
    present in both `Enum.src.lua:2272` and `X2World.h:803`). This is the
    same house list as Sander's rest area (`:4001-4010`).
  - **Cause:** `march2014/data034/DLG_Map_House.src.lua` has **no**
    `COMMON_NPC_LOCATION` row for `WI_BATTLE_FIELD_RANOX_FIELD_REST_00` in
    any of those four houses. Every other rest area, Elder through Sander,
    has one (e.g. Hellen `:1574-1611`, Glave `:2480-2516`). With no row,
    `CX2TFieldGame::AddFieldNpc`'s `bFindNpcPos` gate
    (`X2TFieldGame.cpp:3140-3157`) never creates the NPC node. This is the
    exact mechanism of LANOX_DEFECT_PHASES bug A, which added the
    `WI_VILLAGE_RANOX` rows for the village but not the rest area.
  - June (`14JUN17/data/data034_kom/DLG_Map_House.src.lua`) has all four rows,
    each shaped `{ WORLD_ID, x, y, z }` like March's Sander rows:

    | House | June line | x, y, z |
    |---|---|---|
    | `HI_HELLEN_SHOP_HOUSE` | 2694 | -2240, 22390, -26100 |
    | `HI_POSTBOX` | 3212 | -1030, 22390, -26220 |
    | `HI_BILLBOARD` | 3758 | 450, 22390, -25850 |
    | `HI_GLAVE_HOUSE` | 3919 | 1890, 22390, -25710 |

  - A full per-house diff of the Ranox and rest rows, June vs March staging,
    also found:
    - **4 village rows March lacks** (`WI_VILLAGE_RANOX`): `HI_ADAMS_HOUSE`
      (-3166, 670, -1446), `HI_ED_SHOP_MU` (-3670, 790, 540),
      `HI_EVENT_DOLL` (-3770, 790, 540) and `HI_LUTO` (-3770, 790, 540).
      This accounts for March's 15 `WI_VILLAGE_RANOX` rows against June's 19.
      These matter only if the house is in March's Ranox village
      `HouseIDList` *and* its NPC exists in March. They may relate to the
      `[RANOXDBG]` "empty NPC_ID_LIST" lines (houses 40083, 40090, 40129,
      40130, 40140), which is unconfirmed.
    - **June-only rows to omit, with a comment:** `WI_SPA_RANOX` (Billboard,
      `HI_SPA_INNER_NPC_RANOX`, 3 x `HI_SPA_PPORU`), and `HI_APINK`'s
      Sander-rest row. These are June content; confirm that each world or
      house is absent from March before skipping it.
  - **Separate, unrelated lead (not this cause):**
    - `march2014/data036/MapData.src.lua` has `AddMapData` for
      `VMI_BATTLE_FIELD_SANDER_REST_00` (`:191`) but none for
      `RANOX_REST_00`.
    - `CheckCOBOExpressTicketMapID` returns 14 for Sander rest and 15 for
      `VMI_RANOX`, with no Ranox-rest case (the C++ side is
      `X2Main.cpp:13285`).
    - Record whatever Phase 9 finds here; do not fold it into the NPC fix.
- **Item 11 (Fire Temple collapsing floor, player stuck).**
  - **Mapping:**
    - The user's "stage 2 substage 1" is the dungeon's second stage, meaning
      `STAGE1` / `SUB_STAGE0` (0-based) of
      `march2014/update/data035/Dungeon_Ranox_FireTemple_{Normal,Hard,Expert}.src.lua`.
    - That is world `WI_RANOX_FIRETEMPLE_STAGE1` (602) →
      `Ranox_FireTemple_Stage1.lua` (`march2014/data040/WorldData.src.lua`).
    - All three difficulties use the same world, so one LineMap fix covers
      them all.
  - **Mechanism, from the data:**
    `march2014/update/data040/Ranox_FireTemple_Stage1_LineMap.src.lua`.
    - Line 3 (`LT_NORMAL`, `TRIGGER_ID = 0`) fires trigger 0 (`:518-565`),
      which does three things: plays `FireTemple_Stage1_X_ground.x` anim
      `move01` (the floor visibly collapses), camera shake plus
      `AMB_Earthquake02.ogg`, and at 0.3 s `TAT_LINE_CHANGE_ENABLE` line 3 →
      FALSE.
    - June's trigger 0 **also** carries `TRIGGER_DATA_LINE1/2/3`, which apply
      `TAT_LINE_CHANGE_TYPE` with `CHANGE_LINE_TYPE = LINE_TYPE.LT_NORMAL` to
      lines 11/12/13
      (`14JUN17/data/data040_kom/Ranox_FireTemple_Stage1_LineMap.src.lua:535-`),
      plus `TRIGGER_DATA_ETC2` `TAT_ETC_NPC_AI_ON_OFF`, `NPC_AI_ENABLE = TRUE`.
      The March port commented all four out (`:535-563`), because in 2026-09-18
      neither action existed in March's C++ or Lua enum.
    - Lines 11/12/13 are the pit floor under line 3. They are placed as
      **`LT_MONSTER_ROAD`** (11 at y=2435, 12/13 at y=2675; 12 has
      `BEFORE_LINE = -2`, 13 has `NEXT_LINE = -2`).
    - `CKTDGLineMap::CanDown` (`KTDGLineMap.cpp:1106`, check at `:1145-1148`)
      skips `LT_MONSTER_ROAD` lines when `bIsMonster` is false.
      `CX2GUUser::PhysicProcess` also special-cases a monster-road (or wall)
      down-line (`X2GUUser.cpp:20372`, which sets `bMeOnStart = bMeOnEnd =
      false`).
    - So after the collapse, the player drops onto a floor that is not a
      user floor. **Likely cause** of "animation runs, position frozen"; the
      play-test is the proof.
    - Trigger 1 (`:566-603`, line 26 → pit lines 33/34/35, also
      `LT_MONSTER_ROAD`, mesh `FireTemple_Stage1_X_ground02.x` `move02`) is
      the same bug later in the same stage.
  - **The C++ half already landed; the data half never did.**
    - Ranox Phase 14 (`ad1ec78`, 2026-09-19; `RANOX_PORT_PLAN.md` "Phase 14",
      "Executed 2026-09-19") added `TAT_LINE_CHANGE_TYPE = 7` (flag
      `SERV_IRUHADEV_RANOX_TRIGGER_LINE_CHANGE_TYPE`,
      `KTDGLineMap.h:111-113`, parse `KTDGLineMap.cpp:2700-2706`, run
      `X2World.cpp:1665-1671`, field `CHANGE_LINE_TYPE`) and
      `TAT_ETC_NPC_AI_ON_OFF` (flag `SERV_IRUHADEV_RANOX_TRIGGER_NPC_AI_ON_OFF`,
      `KTDGLineMap.cpp:2755-2763`, field `NPC_AI_ENABLE`), plus `TAT_MESH_MOVE`,
      `TAT_ETC_PLAY_EFFECTSET` and `TAT_ETC_TALK_BOX`.
    - Its own notes say the Lua side was **"still outstanding"**: the names
      must be added to `march2014/data036/Enum.src.lua`
      (`TRIGGER_LINE_ACTION_TYPE` is at `:1608-1614` and currently stops at
      `TELEPORT = 6`), and the commented-out trigger entries restored in
      **four** linemaps:
      - `Ranox_FireTemple_Stage1_LineMap` x2
      - `Ranox_GateWay_Stage1_LineMap` x2
      - `Ranox_Geyser_stage0_linemap` x3
      - `Ranox_Secret_Stage2_LineMap` x1
    - Until then `LUA_GET_VALUE_ENUM` sees a nil global and falls back to
      `TAT_*_NONE`.
  - **Enum-value trap.**
    - March's C++ numbers the etc actions `NPC_AI_ON_OFF = 6`,
      `PLAY_EFFECTSET = 7`, `TALK_BOX = 8` (`KTDGLineMap.h:115-131`).
    - June's `Enum.lua` numbers them `TALK_BOX = 6`, `PLAY_EFFECTSET = 7`,
      `NPC_AI_ON_OFF = 8`.
    - The linemaps use names, so only March's `Enum.lua` and `KTDGLineMap.h`
      (and its server mirror `KncWX2Server/Common/LineMap/XSLLineMap.h`) have
      to agree. **Pasting June's `TRIGGER_ETC_ACTION_TYPE` block would make a
      TALK_BOX trigger dispatch as NPC_AI_ON_OFF.**
    - `TAT_LINE_CHANGE_TYPE = 7` agrees in both, and `TAT_MESH_MOVE = 3`
      needs checking the same way.
  - **NPC_AI_ON_OFF semantics differ from June's intent.**
    - Phase 14's implementation toggles AI for **every** NPC in the game
      (`Always.h:3924-3927`; the Lua has no target field).
    - Triggers 2/3 (`NPC_AI_ENABLE = FALSE`, lines 0/1 and 21-25) were
      emptied to `{}` by the port (`:604-625`). Restoring them would freeze
      every NPC, which may include the user's AI-party bots, from the moment
      the player steps onto line 0 until trigger 0 turns AI back on.
    - This is separate from the stuck-floor fix.

---

## Phase 0 - TET-key census (read-only)

**Goal:** turn "missing atlas key" (items 2, 3, 4, 5, plus the known-open
`DLG_UI_Button17` and `DLG_UI_Common_Texture71_NEW`) into one checklist, and
catch keys nobody has reported yet.

Steps:
1. Collect every `(texture, key)` pair from these sources:
   - every staged Ranox UI lua, meaning each `SetTex`, `SetNormalTex`,
     `SetOverTex`, `SetDownTex` and `IconAni` call in `march2014/**` whose
     file name matches `Ranox|RANOX` or which is one of the patched shared
     dialogs (`DLG_UI_World_Map_NEW`, `DLG_UI_Local_Map_NEW`, the
     `DLG_UI_FIELD_MAP_Ranox_*`)
   - `DungeonData`'s Ranox `m_TextureName` / `m_PieceName`
   - `BattleFieldData`'s `BATTLE_FIELD_TEXTURE_KEY`
2. For each texture, find the **live** `.kom` that wins for it. Extract its
   `.TET` and check the key (case-insensitive, matching the engine lookup).
   Read the archive-precedence notes in `CLAUDE.md` / KGCMassFileManager
   first. If a name appears in more than one archive, record every copy.
3. For every missing key, record whether June has it (and in which archive),
   and whether June's TGA/DDS is byte-identical to March's.
4. Write the table into this doc as "Phase 0 findings". Classify each row as
   *adopt June file* (March's keys are UV-identical in June's copy) or *merge
   keys into March's atlas*.
5. **Sibling census: mesh-internal textures** (added 2026-09-30, after
   Item 9). Burned Forest's missing `BossRaid_Abyss_smog.dds` was a texture
   named only *inside* a `.Y` mesh, so staging checks by file name never saw
   it. Run the same check over every `.Y`/`.X` in `march2014/update/**`:
   - MSZIP-decompress each mesh (`xof 0303bzip`: after the 16-byte header, a
     u32 total size, then chunks of `u16 usize, u16 csize, "CK"`, raw
     deflate with the previous chunk's output as `zdict`; `csize` excludes
     the 4-byte chunk header).
   - Regex out the `*.tga|*.dds` names, then look each one up in the live
     index (every live kom via `elstools inspect -q -i`; its JSON is
     multi-line).
   - Cross-check with `ErrorLog.txt` `OpenTexture` misses. A few meshes
     (e.g. `Fire_forest_stage0_Alpha_stem02.Y`) do not decode with plain
     zlib even though D3DX loads them.
   - Record every miss with June's archive and size, as a separate
     "mesh textures" table.

Deliverable: the findings table. Phases 1, 2 and 4 consume it. Make no
changes.

**Run with:** Sonnet 5, medium effort. Read-only scripting; no code review, but spot-check a few table rows by hand before Phases 1/2/4 consume them.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger",
and "Phase 0". Branch: mods/lanox-port. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch.

Your task is READ-ONLY: build the TET-key census described in Phase 0.
Check every (texture, key) pair used by the staged Ranox UI luas, by the
Ranox rows of DungeonData (m_TextureName/m_PieceName), and by
BattleFieldData's BATTLE_FIELD_TEXTURE_KEY against the LIVE .kom files in
...\237311\24965799\data (elstools inspect/extract), never against the
kom_contents staging dir. For each missing key, record whether June2017
(...\237311\14JUN17\data) has it and whether June's image is byte-identical to
March's. Append the result to LANOX_REMAINING_PHASES.md as "Phase 0
findings", classifying each row as adopt-June-file or merge-keys. Make no
other changes.
```

---

## Phase 0 findings (investigated 2026-10-01)

Read-only. Method: 424 `(texture, key)` pairs were pulled by regex from the
12 in-scope files (the 10 `Ranox|RANOX`-named staged UI luas plus the two
patched shared dialogs, `DLG_UI_World_Map_NEW` and `DLG_UI_Local_Map_NEW`),
plus 2 pairs from `DungeonData`'s Ranox block (`data035`, lines
13290-14781) and 4 from `BattleFieldData`'s Ranox block (`data036`, lines
1779-2045) — 73 distinct textures, 193 `(texture,key)` pairs total after
folding duplicates. **Sanity check:** a raw count of
`SetTex|SetNormalTex|SetOverTex|SetDownTex|IconAni` calls in the same 12
files found 435 vs. the regex's 424; the 11-call gap is a different call
shape (`IconAni(control, "tex", "key")`, a free function, vs. the
`:SetTex("tex","key")` method form the regex targeted) plus one dynamic-key
`SetTex` and one function *definition* line. All 11 were checked by hand:
the 9 `IconAni(...)` calls duplicate pairs already captured from
`SetNormalTex`/`SetOverTex` elsewhere, and the dynamic key (see
`DLG_UI_Common_Texture93_NEW.tga` below) was added manually. No pairs were
missed.

Each texture's **live**-winning `.kom` was found via `elstools inspect -q`
over every live `.kom` (183 files) — see the mount-order note under
"Image11" below for what "winning" means when a name exists in more than
one. TETs were parsed with a from-scratch reader against
`KTDXLIB/KTDXDeviceTET_Preprocessing.h`/`.cpp`'s on-disk layout (header +
sorted `DWORD` key-offset array + `KTETFormatTextureUV` array), verified
against the known-good `DLG_Localmap_Title_01.TET` case in the ledger (53
keys, all `TITLE_*`, matches exactly). Pixel comparisons on the
uncompressed-TGA cases used a from-scratch 18-byte-header TGA reader
(no Pillow available in this environment); the two DDS (DXT-compressed)
cases were compared by file hash and UV/rect geometry only, not decoded
pixel-for-pixel — flagged below where that distinction matters.

**64 of the 73 textures are already fully correct live** (every key present,
matching June where compared) and are not listed below. **9 have at least
one missing or format-mismatched key.**

### A. Missing/mismatched keys

| Texture | Live TET | Missing key(s) | June has it | Image byte-identical to June | Classification |
|---|---|---|---|---|---|
| `DLG_Localmap_Title_01.tga` | `data063.kom`, 53 keys | `Map_Title_Ranox`, `Ranox_Ashes_Village`, `Ranox_Burned_Forest`, `Ranox_FINALHALL`, `Ranox_FIRETEMPLE`, `Ranox_GateWay`, `Ranox_Geyser` (7) | Yes, `data151.kom`, 77 keys | No | **merge-keys** |
| `DLG_UI_Common_Texture93_NEW.tga` | none live | `ROSE_EAGLE_RADAR00`-`09` (10, dynamic key, see note) + the 1 static `ROSE_EAGLE_RADAR05` reference | Yes, `data215.kom`, 66 keys (also has `_LEFT`/`_RIGHT`) | **Yes** | **adopt-June-file** |
| `DLG_UI_Common_Texture_Fieldname_6.tga` | `data057.kom`, 3 keys | `FIELD_NAME_RANOX_1` (1) | Yes, `data057.kom`, 4 keys | No (see pixel note) | **merge-keys** |
| `DLG_UI_Common_Texture_Fieldname_7.tga` | none live (image present, no TET) | `FIELD_NAME_RANOX_2`, `_3`, `_4` (3) | Yes, `data057.kom`, 4 keys | **Yes** | **adopt-June-file** |
| `DLG_UI_Dungeon_Image10.dds` | none live (image present, no TET) | `Ranox_Ashes_Village`, `Ranox_Burned_Forest`, `Ranox_FinalHall`, `Ranox_FireTemple`, `Ranox_Gateway`, `Ranox_Geyser` (6) | Yes, `data065.kom`, 8 keys | No — **different codec**, see note | **adopt-June-file, with a caveat** |
| `DLG_UI_Dungeon_Image11.dds` | `datai022.kom`, 4 keys (unrelated content) | `RANOX_MAGI_pit` (1) | Yes, `data065.kom`, 8 keys | No (different file entirely) | **merge-keys — see mount-order note, changes Phase 1** |
| `DLG_UI_Image_01.tga` | none live | `MonsterFever_1`-`5` (5) | Yes, `data215.kom`, 102 keys | **Yes** | **adopt-June-file** |
| `DLG_UI_Image_08.tga` | none live | `MonsterFerver` (1) | Yes, `data215.kom`, 22 keys | **Yes** | **adopt-June-file** |
| `DLG_UI_Button20_NEW.tga` | `data056.kom`, 49 keys | `RANOX_BTN_NORMAL`, `RANOX_BTN_OVER` (2) | **No — June's copy has 0 Ranox keys either** | n/a | **dead end, confirmed — see note** |

Notes:

- **`DLG_UI_Common_Texture93_NEW.tga` dynamic key.** The regex only caught
  one literal `SetTex("DLG_UI_Common_Texture93_NEW.tga", "ROSE_EAGLE_RADAR05")`
  (`DLG_UI_RANOX_SECRET_FEVER_GAGE.src.lua:109`, a static default). The real
  requirement is a runtime loop at line 89,
  `strTexName = string.format("%s%d", "ROSE_EAGLE_RADAR0", tostring(slot10))`
  for `slot10 = 0..9` — i.e. all 10 digits `ROSE_EAGLE_RADAR00`-`09`. June's
  TET has all 10 (plus `_LEFT`/`_RIGHT`, unused by this lua). Live has none
  of them; the image (`.tga`) is already byte-identical to June's, so this is
  a clean whole-file TET adopt (no pixel work).
- **`DLG_UI_Common_Texture_Fieldname_6.tga` pixel check.** UVs for the 3
  shared keys (`FIELD_NAME_SAND_1/2/3`) are byte-identical between live and
  June (same rects, same canvas size, 1048620 B both). A full pixel sample of
  those 3 rects, however, found real per-pixel differences (roughly a
  quarter of sampled pixels in each rect differ, e.g. `4f606b01` vs
  `81818101` at (128,1)) — **the SAND art itself was redrawn between builds,
  not just extended.** A wholesale TGA swap would silently change Sander's
  already-shipped field-name plates. The only safe merge is: keep live's
  canvas and pixels as-is, and paste in only the new `FIELD_NAME_RANOX_1`
  rect (June's rows 378-503) plus its key entry.
- **`DLG_UI_Dungeon_Image10.dds` is not a format-preserving case.** Live's
  copy (`data065.kom`, 1,398,228 B) is uncompressed ARGB8888 with a full mip
  chain; June's (349,680 B) is DXT5. Both are 512×512, and
  `grep -rli Image10 march2014/` finds exactly one reference anywhere
  (`DungeonData.src.lua`'s own Ranox block — confirmed no non-Ranox dungeon
  uses this file), so there is no collision risk merging this one. But
  because live has **zero** TET keys today, "adopt June's file" here means
  literally replacing the uncompressed art with June's DXT5 art, not
  preserving a March original — Phase 1 should still get eyes on both
  images before committing (pixel content, not just format, could differ;
  this census only confirmed same dimensions and confirmed the formats
  differ, not that the visible art is the same picture at different
  quality).
- **`DLG_UI_Button20_NEW.tga` is a confirmed dead end, both sides.** June's
  copy of this exact file (`data056.kom`, 92 keys) has no `RANOX_BTN_*` key
  at all — Phase 3 is right that this is superseded, and the correct atlas
  is a *different* file, `DLG_UI_Button21.TET` (see section B).

### B. The two "known-open" items named in Phase 0's goal are already resolved live

`LANOX_TROUBLESHOOTING.md` lists `DLG_UI_Button17` (Cobo
Express/destination-dialog `RANOX_ICON`) and `DLG_UI_Common_Texture71_NEW`
(world-map `TOWN_RANOX`/`FIELD_NAME_RANOX_*`/`FIELD_REST_RANOX`) as open
atlas merges still staged under `march2014/update/`, not pack-ready. Both
are **already live, complete, and byte-identical to June** — this census
found no gap in either:

| Texture | Live | June | Byte-identical (TET + TGA) |
|---|---|---|---|
| `DLG_UI_Button17.tga` (`data056.kom`) | 60 keys incl. `RANOX_ICON`, `SANDER_ICON` | 60 keys, same | **Yes**, both files |
| `DLG_UI_Common_Texture71_NEW.tga` (`data057.kom`) | 49 keys incl. `TOWN_RANOX`, `FIELD_NAME_RANOX_0`-`3`, `FIELD_REST_RANOX` | 49 keys, same | **Yes**, both files |

(Note: `data056.kom` also has an unrelated `DLG_UI_Button17_A.tga` in
`datai020.kom` — a different file, not a second copy of the same name — so
there is no mount-order ambiguity here.) `LANOX_TROUBLESHOOTING.md`'s notes
on these two are stale; whatever repack closed them did so before this
session. Phase 4 does not need to touch `DLG_UI_Common_Texture71_NEW.tga`.

As a bonus check (not in Phase 0's scope, but adjacent): Phase 3's claim
that June's `DLG_UI_Button21` atlas already carries `BTN_RANOX_N/O/D` was
verified directly — live's `DLG_UI_Button21.TET`/`.tga` (`data056.kom`, 36
keys) are **already byte-identical to June's**, `BTN_RANOX_D/N/O` included.
(`DLG_UI_Button21_NEW.tga`, a different, larger file, has no Ranox keys on
either side and is not what Phase 3 needs.) Phase 3 needs no atlas work at
all, only the lua adoption it already planned.

### C. Critical: Image11's mount order overturns Phase 1 as currently written

`DLG_UI_Dungeon_Image11.dds` exists in **two** live `.kom`s under the exact
same filename: `datai022.kom` (4 keys — `EVENT_GIFT_FACTORY`,
`EVENT_SECRET`, `KING_NASODS_LAIR`, `MISSION_IMPOSSIBLE`, all in the left
half of the canvas, x 0-204) and **not** `data065.kom` (live has none there
today). `grep -rli Image11 march2014/ X2Lib/ X2/ KTDXLIB/` finds exactly one
reference anywhere in the codebase — `DungeonData.src.lua`'s own
`RANOX_MAGI_pit` line — so none of `datai022.kom`'s 4 existing keys are
currently drawn by anything; they are dead data in this build, but the
house rule (`CLAUDE.md`, "do not delete ported files") still argues against
overwriting them casually.

`KGCMassFileManager` resolves same-named files through a single flat
`std::map` keyed by filename (`AddMassFile`, `KGCMassFileManager.cpp:490`,
plain `operator[]` assignment — last mount wins, not first). `X2Main.cpp`'s
`m_KOMList` is built by pushing `data001.kom..data150.kom` (or 226 under
`SERV_IRUHADEV_KOM_FILE_COUNT_226`) **then** `datai001.kom..datai023.kom`
(or 32), and mounted by iterating that list in push order
(`X2Main.cpp:1406`, `for (i = 0; i < m_KOMList.size(); i++)`). `datai022.kom`
is therefore **always** mounted after `data065.kom`, regardless of which
count constant is active.

**Consequence: Phase 1's current plan — "Stage `Image11.dds`/`.TET` as new
files... in `march2014/update/data065/`" — would never take effect.** Once
packed, `data065.kom` would carry a `DLG_UI_Dungeon_Image11.dds`/`.TET` too,
but `datai022.kom`'s copy, mounted later, would win the filename map every
time, and the Secret Hell result-screen picture would keep resolving to the
old event-icon art, not the newly staged Ranox one — with no error, since
the file exists at that name, just from the wrong archive.

The fix has to either replace `datai022.kom`'s copy outright, or (safer,
given rule 3 above about not destroying ported data) **merge
`RANOX_MAGI_pit`'s rect into `datai022.kom`'s existing atlas**, in unused
space. June's own layout puts `RANOX_MAGI_PIT` at (1,233)-(203,346) — which
**overlaps** live's `MISSION_IMPOSSIBLE` at (1,229)-(203,342) almost
exactly, so it cannot be copied at June's own coordinates. All 4 of live's
existing keys sit in x 0-204; June's own 8-key grid (2 columns × 4 rows)
suggests the free column is x≥206 (June: `ELISION_ATLAS_PIT` etc. at
x=206-408). Placing the new rect in live's x≥206 column at the same
row as June's would avoid the 4 existing keys, but the atlas is DXT3
(both live's and June's Image11.dds are 512×512 DXT3), so any rect whose
left edge isn't a multiple of 4 (206 is not) crosses into a shared
compression block with its neighbor — a direct block-copy would corrupt
that column's edge; Phase 1 will need to decode/recompress the affected
region rather than splice compressed bytes. This wasn't verified pixel-by
-pixel here (no DXT3 decoder was written for this read-only census); it's
a geometry/precedence finding only, flagged for Phase 1 to confirm before
merging.

**Superseded 2026-10-01 (Phase 1 execution).** Phase 1 first built the
block-copy merge described above — confirmed working (1479 blocks, exact
51-block/204px shift, verified byte-for-byte against June's source blocks
and against the 4 untouched original entries) — but the user then decided to
replace the whole file with June's instead, per the new asset-conflict
priority rule above: June's asset wins outright, since the 4 existing keys
are unreferenced dead data. `datai022.kom`'s `DLG_UI_Dungeon_Image11.dds`/
`.TET` are now staged as plain byte-for-byte copies of June's 8-key version;
`EVENT_GIFT_FACTORY`, `EVENT_SECRET`, `KING_NASODS_LAIR` and
`MISSION_IMPOSSIBLE` are gone from live's copy once this packs. The
mount-order finding above (why the fix has to land in `datai022`, not
`data065`) still holds regardless of merge-vs-replace.

### D. `DLG_Localmap_Title_01.tga`: confirmed merge, and the merge is safe

Beyond the missing-key list in section A, two more things were checked
because Phase 2 explicitly requires them before choosing adopt vs. merge:

- **UV identity of existing March keys, checked, and it fails.** Of the 53
  keys live and June share, 51 have byte-identical rects, but **2 do not**:
  `TITLE_ELDER_SECRET` (live `302,200→438,227`; June `302,200→411,227`,
  narrower) and `TITLE_ERDA_ISLAND` (live `302,164→406,181`; June
  `200,200→301,217`, moved entirely). June rearranged its atlas to make room
  for later content (2016 Xmas, Halloween, Elysion, etc. — 23 June-only keys
  beyond the 7 Ranox ones this port needs). **This alone rules out adopting
  June's file wholesale** — doing so would silently break Elder's and Erda
  Island's local-map title plates. Confirms the plan's own precondition
  failed; merge-keys is correct.
- **Overlap check for the 7 new rects.** None of the 7 Ranox keys' rects
  (`Ranox_Ashes_Village` 404,1→505,18; `Ranox_Burned_Forest` 2,128→88,145;
  `Ranox_FinalHall` 82,200→198,223; `Ranox_FireTemple` 303,128→413,145;
  `Ranox_Gateway` 82,225→194,242; `Ranox_Geyser` 414,128→509,145;
  `Map_Title_Ranox` 303,229→432,255) overlap any of live's 53 currently-used
  rects. A scoped merge — copy only those 7 regions from June's TGA into
  live's, plus their 7 key entries — is geometrically safe. (June-only keys
  for unported content — `ELYSION`, `HALLOWEEN_ZOMBIE`, `HERO_DUNGEON`,
  `2016_XMAS`, `TITLE_APOCALYPSE`, etc., 23 total — should be omitted per
  house rule, not copied in.)

### E. Sibling census: mesh-internal textures (Phase 0 step 5)

Every `.X`/`.Y` under `march2014/update/**` (1034 files) was scanned: each
was MSZIP-decompressed (`xof 0303bzip` header, chunked raw-deflate with each
chunk's own output as the next chunk's dictionary — confirmed against the
known case, `Fire_forest_stage0_AP_Cloud01.Y` → `BossRaid_Abyss_smog.dds`,
matching the ledger's Item 9 finding exactly) and searched for
`*.tga`/`*.dds` name strings. **581 decoded; 370 failed to decode with this
from-scratch reader (same class of limit the ledger already noted for
`Alpha_stem02`/`Tree03` — the engine loads these fine, the scratch reader
doesn't); 83 had no `xof/bzip` header at all** (plain/uncompressed `.X`,
skeleton-only files, etc.). 719 texture-name strings were found across the
decoded meshes; checked against all 183 live `.kom`s.

**After removing false positives, 27 distinct texture names are referenced
by a staged mesh but absent from every live `.kom`:**

| Texture | Referenced by (example) | In June | Ranox-relevant? |
|---|---|---|---|
| `BossRaid_Abyss_moon.dds` | `data154/Field_Ranox_Rest_Alpha_moon.Y` | `data023.kom` | Yes (Ranox rest) |
| `Field_Atlas_typo.dds` | `data154/Field_Ranox_Stage3_typo.Y` | `data180.kom` | Yes (Ranox) |
| `Ranox_FinalHall_AP_crack.dds` | `data154/FinalHall_Stage03_AP_crack.Y` | **Not found in June either** | Yes — **unsourceable, see note** |
| `Red_Dragon2_AP_shadow.dds` | `data154/Ranox_village_AP_light.Y` | `data023.kom` | Yes (Ranox) |
| `Village_ADBoard.dds` | `data154/Ranox_villeage_ADplane.Y` | `data216.kom` | Yes (Ranox) |
| `Valen_AP_light.dds` | `data023/Ranox_star.Y` | `data023.kom` | Yes (Ranox) |
| `Valen_AP_light2.dds` | `data023/Ranox_back_light.Y`, `Ranox_gate.Y` | `data023.kom` | Yes (Ranox) |
| `Valen_AP_light3.dds` | `data023/Ranox_back.Y` | `data023.kom` | Yes (Ranox) |
| `Valen_AP_light5.dds` | `data023/Ranox_gate.Y` | `data023.kom` | Yes (Ranox) |
| `spa_villeage_typo.dds` | `data023/Spa_Ranox_text.y` | `data023.kom` | Yes (Ranox) |
| `teddybear_Alpha_smoke.dds` | `data154/ashs_Village_S1_AP_smoke.Y` ×5 | `data023.kom` | Yes (Ashes Village) |
| `NPC_VELDER_HANNA_HAND.dds` | `data154/ashs_Village_S1_red_light03.Y` | Not found as `.dds` (see note) | Yes (Ashes Village), but likely a **false positive** |
| `RAGE_DRYADKENTA_2.dds` | `data002/RAGE_DRYADKENTA_Hair1/2.Y`, `_Tail.Y` | `data002.kom` (separate from a `.tga` of the same stem) | Ambiguous, see note |
| `Dark_Bead.tga` | `data002/Dark_Drabaki_Dead_01.Y` | `data002.kom` | Not obviously Ranox |
| `FireImpactGrow01.dds` | `data002/FireImpactG04.Y` | `data002.kom` | Not obviously Ranox |
| `IPLITAN_Fire01_Red.dds` | `data002/IPLITAN_FireM01.Y`, `_FireM03.Y` | `data002.kom` | Not obviously Ranox |
| `ADE_CG001.dds` | `data002/LITTLEWORKER_Attack_M.Y`, `data184/RHB_Lockon_Stratos_AttackM1.Y` | `data147.kom` | Not obviously Ranox |
| `Roo_highLightMagical.dds` | `data002/Ska_AttackA_sparkLight.X`, `data157/LCG_M_Beam04.Y` | `data157.kom` | Not obviously Ranox |
| `SKA_flame3.dds` | `data002/SKA_flame_spark.X` | `data002.kom` | Not obviously Ranox |
| `SKA_flame6.dds` | `data002/SKA_flame6.X` | `data002.kom` | Not obviously Ranox |
| `ALS_SLICER_01.dds` | `data132/AA_SUPPRESSION_SPECTER_P01.Y` | `data132.kom` | Not obviously Ranox |
| `ALS_VIEW_BETWEEN_TRACE01.dds` | `data002/Ranox_SecretBoss_Magi_AttackA1.X`, `_AttackAC1.X` | `data132.kom` | Yes (Ranox Secret Hell boss) |
| `SCA_Projectile001.dds` | `data142/SCA_Projectile001.Y` | `data142.kom` | Not obviously Ranox |
| `SCA_SLICER_01.dds` | `data142/SCA_FATE_SMASHER_P001.Y` | `data142.kom` | Not obviously Ranox |
| `ADE_GRAVITY_BURSTER_M01.dds` | `data147/ADE_PE01.Y` | `data147.kom` | Not obviously Ranox |
| `ADD_MM_ROCKON01.tga` | `data147/ADD_MM_ROCKON_01.Y` | `data147.kom` | Not obviously Ranox |
| `Gunner_LaserBazookaM1.dds` | `data184/RHB_Lockon_Stratos_AttackM2.Y` | `data184.kom` | Not obviously Ranox |

Notes:

- **3 initial hits were false positives from extension mismatch**, caught by
  re-checking every miss against the live index by *stem* (ignoring
  extension) rather than exact name, and confirmed only when the stem match
  was itself an image file (not coincidentally matching the `.X`/`.Y` mesh's
  own filename): `Ranox_Geyser_Road02.tga`/`Ranox_Geyser_Road02_1.tga`
  referenced by `data154/Field_Ranox_Stage1_Way_C3.Y` — live already has
  both as `.dds`, byte-size-identical to June's `.dds` (32896 B each).
  Dropped from the table above.
- **`NPC_VELDER_HANNA_HAND.dds`** — live has `NPC_VELDER_HANNA_HAND.tga`
  (`data013.kom`, 49702 B), and June has the same `.tga`, not a `.dds`. Very
  likely the same extension-mismatch pattern (probably a false positive),
  kept in the table rather than dropped because, unlike the Road02 case, the
  live/June match is only a `.tga` — no `.dds` exists on *either* side to
  confirm this isn't a second, genuinely-missing texture with a coincidental
  stem.
- **`RAGE_DRYADKENTA_2.dds`** — live has `RAGE_DRYADKENTA_2.tga`
  (`data153.kom`, 49702 B). June has *both*: the same `.tga` (identical size
  to live's) **and** a separate `.dds` (`data002.kom`, 8320 B, a different
  file). Whether the mesh's `.dds` reference is content already covered by
  live's `.tga` or a genuinely distinct asset was not resolved here — flag
  for whoever picks this up (Dryad Kenta is a confirmed Ranox monster, see
  `dryad-kenta-jump-softlock` history).
- **`Ranox_FinalHall_AP_crack.dds` is absent from both live and June by this
  name.** No near-stem match either. Per house rule ("a Lua function ... gets
  no fallback at all" — the same applies here to an unsourceable image),
  this cannot be adopted or merged from anything in this tree; whoever picks
  up the FinalHall stage-3 crack effect needs to either find the real source
  file elsewhere or treat the effect as cosmetically absent.
- **The "not obviously Ranox" rows** (`data002`, `data132`, `data142`,
  `data147`, `data157`, `data184`) surfaced only because Phase 0 step 5
  scans `march2014/update/**` unfiltered, not because anything ties them to
  Lanox — `LITTLEWORKER`, `SKA_*`, `RHB_Lockon_Stratos`, `SCA_*`,
  `AA_SUPPRESSION_SPECTER`, `Dark_Drabaki`, `ADD_MM_ROCKON`, `ADE_PE01` are
  character/skill names with no Ranox tie found by name or folder. They are
  real gaps in whatever port staged those meshes, but are very likely a
  **different, unrelated in-flight effort** (see `june-character-port-checklist`
  history) sharing the same `march2014/update/` tree. Recorded here for
  completeness per the literal Phase 0 step 5 instruction; not this port's
  responsibility to fix.

### F. Summary classification (for Phases 1, 2, 4)

| Item | Texture(s) | Classification |
|---|---|---|
| Phase 1 (Image10) | `DLG_UI_Dungeon_Image10.dds` | adopt-June-file (no collision; verify pixel content, not just format) |
| Phase 1 (Image11 / Secret Hell) | `DLG_UI_Dungeon_Image11.dds` | adopt-June-file wholesale, into `datai022.kom` (not `data065`, see section C) — **superseded 2026-10-01, was merge-keys** |
| Phase 2 (local-map names) | `DLG_Localmap_Title_01.tga` | merge-keys (confirmed by UV mismatch; safe by overlap check) — **re-confirmed under the corrected asset-conflict rule in Phase 2's own section below**: wholesale adopt independently fails on 3 content-mismatched keys (`TITLE_ERDA_ISLAND`, `TITLE_HOLLOWEEN`, `TITLE_HARMEL_SECRET`), not just the original UV argument |
| Phase 4 (world-map labels) | `DLG_UI_Common_Texture71_NEW.tga` | **already done — no work needed, see section B** |
| — (Secret Hell fever gauge) | `DLG_UI_Common_Texture93_NEW.tga`, `DLG_UI_Image_01.tga`, `DLG_UI_Image_08.tga` | adopt-June-file (all 3, byte-identical images) |
| — (field-name plates) | `DLG_UI_Common_Texture_Fieldname_6.tga` | merge-keys, scoped (pixels differ beyond the new rect) |
| — (field-name plates) | `DLG_UI_Common_Texture_Fieldname_7.tga` | adopt-June-file (byte-identical image, TET only) |
| — (Cobo Express / destination icon) | `DLG_UI_Button17.tga` | **already done — no work needed, see section B** |
| Phase 3 (Lanox tab) | `DLG_UI_Button20_NEW.tga` | dead end, confirmed both sides — do not pursue |
| Phase 3 (Lanox tab) | `DLG_UI_Button21.tga` | **already done — no work needed, see section B** |
| — (mesh-internal, Ranox) | 12 names in section E | adopt-June-file (all found in June; stage per the usual rule) |
| — (mesh-internal, unsourceable) | `Ranox_FinalHall_AP_crack.dds` | **no fallback possible — not in live or June** |
| — (mesh-internal, unrelated) | 14 names in section E | out of this port's scope — flagged, not fixed |

---

## Phase 1 - Dungeon image on the result screen and party list [item 2]

**Root cause:** see the ledger. `DLG_UI_Dungeon_Image10.TET` is missing from
live `data065.kom`, and `Image11` is missing entirely, so `SetTex` gets
texSize (0,0) and the picture is invisible.

Steps:
1. **Settled (user, 2026-09-30):** "party list UI doesn't show up" means
   **only the dungeon picture** in the party panel (`X2PartyUI.cpp:2194`),
   which has the same cause as the result screen. The panel itself works.
2. `cmp` June's `data065_kom/DLG_UI_Dungeon_Image10.dds` with March's live
   copy.
   - If identical: stage June's `.TET`.
   - If different: check that every March key in June's TET has identical
     UVs, then stage June's `.dds` + `.TET`.
   Stage `Image11.dds` / `.TET` as new files. Everything goes in
   `march2014/update/data065/`, with a `_manifest.json` update (algo 0).
3. Check the piece names against the TET keys (Phase 0 table).
   `Ranox_Burned_Forest` vs June's key spelling matters.
4. Hand the pack to the user. Verify by extracting live `data065.kom`.

Verification:
- Clear a Ranox dungeon: the result screen shows the dungeon art.
- Open the party panel with a Ranox dungeon selected: the art shows.
- `ErrorLog.txt` has no texture-load error for Image10/11.

**Run with:** Sonnet 5, medium effort. Data staging; light review (read the manifest diff and the UV-identity check output). No C++.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger",
"Phase 0 findings" (if present) and "Phase 1". Branch: mods/lanox-port. Do NOT
consult NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch. Never launch X2_offline.exe without asking first.

Your task: fix the missing Ranox dungeon picture on the dungeon result screen
and in the party panel. Live data065.kom has DLG_UI_Dungeon_Image10.dds but no
.TET, and has no Image11 at all. (Already settled: in the party panel only
the dungeon picture is missing; the panel itself works.) Stage June's
files per Phase 1: byte-compare the .dds first, and check UV identity for any
March keys if the .dds differs. Update the _manifest.json, and hand me the
pack list. After I pack, verify by extracting the LIVE data065.kom.
```

**Done 2026-10-01.** The kickoff's premise ("no Image11 at all") was wrong —
see Phase 0 section C: `Image11` exists live, but in `datai022.kom`, not
`data065.kom`, and last-mount-wins means a copy staged into `data065` (as
step 2 above says) would never be seen. Both fixes ended up as wholesale
June-file adopts, not merges, and Image11's went into `datai022`, not
`data065`:

- **`Image10`**: live's `.dds` (uncompressed, 1,398,228 B, no `.TET`) turned
  out to be stale/wrong art — rendered it and it showed unrelated old
  dungeon pieces, not Ranox content. Replaced wholesale with June's DXT5
  copy (349,680 B) after rendering it and confirming all 6 `DungeonData`
  piece names (`Ranox_Ashes_Village`/`_Burned_Forest`/`_FinalHall`/
  `_FireTemple`/`_Gateway`/`_Geyser`, all uppercased before lookup by both
  `X2StateDungeonResult.cpp:2258` and `X2PartyUI`'s `SetTexureData_`)
  resolve to the matching art. `grep` confirmed no other March dungeon
  references this file. Staged: `march2014/data065/DLG_UI_Dungeon_Image10.dds`
  (patch) + `march2014/update/data065/DLG_UI_Dungeon_Image10.TET` (new file,
  manifest updated) + the matching `kom_contents/data065_kom/` copies.
- **`Image11`**: first built as a block-level merge preserving live's 4
  unrelated keys (verified byte-exact, see the superseded note in section
  C), then replaced with a plain wholesale copy of June's 8-key file per the
  user's 2026-10-01 asset-conflict-priority call (see "Fix philosophy") —
  the 4 old keys (`EVENT_GIFT_FACTORY`, `EVENT_SECRET`, `KING_NASODS_LAIR`,
  `MISSION_IMPOSSIBLE`) are gone once this packs, since nothing referenced
  them. Staged: `march2014/datai022/DLG_UI_Dungeon_Image11.{dds,TET}`
  (patch, no manifest change — both names were already listed) + the
  matching `kom_contents/datai022_kom/` copies.
- A `.TET` reader/writer was written and proven byte-exact round-trip
  against three known files before being used to inspect/build anything.
- **Owed:** packing `data065.kom` and `datai022.kom` from
  `kom_contents/<name>_kom/` is the user's step. Verify against the live
  koms after packing, per the usual convention.

---

## Phase 2 - Dungeon names on the Lanox local map [item 4]

Item 4 is two problems:
- **(a) Names.** The icon lua's `DLG_Localmap_Title_01.tga` `Ranox_*` keys
  don't exist in March's TET. This is fixed here.
- **(b) Progress bars.** Sander's names and progress come from
  `*_Icon_Info_NEW` + `_LimitInfo` + star controls, and June has none of
  those files, so June draws progress some other way. That half is
  `JUNE_UI_PARITY_PHASES.md` Phase 3. This phase does **not** invent
  Icon_Info files for Ranox.
  - **Closed 2026-10-01: matches June.** JUNE_UI_PARITY Phase 0 found that
    June has no difficulty-completed bars on any village. The bars are the
    `Star_Bg`/`Star_1..3` statics in `*_Icon_NEW`, and June removed them
    everywhere. The `*_Icon_Info_NEW` files are orphans in March too. Per
    the user, we follow June: Lanox gets no bars, and the other villages
    lose theirs when JUNE_UI_PARITY Phase 3 adopts June's per-village luas.

Steps (names):
1. Take the Phase 0 row for `DLG_Localmap_Title_01`. Decide between:
   - **Adopt June's `.TET` + `.tga`:** only if every March `TITLE_*` key has
     identical UVs in June's TET and the pixels in those rects match. Script
     it; don't eyeball it.
   - **Merge:** copy the 7 Ranox rects into March's TGA, using the
     LANOX_DEFECT_PHASES Phase 4/5 technique.
2. Stage into `march2014/update/data063/`, or into whichever archive Phase 0
   says wins.
3. Hand the pack to the user. Verify against the live kom.

Verification: on the Lanox local map, each dungeon icon shows its name
plate, and the Map_Title_Ranox header renders. Sander is unchanged.

**Run with:** Sonnet 5, high effort (atlas pixel/UV work is easy to get subtly wrong). Review required if the rects are merged into March's atlas (check the merge script and render the result); light if June's file is adopted outright.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger",
"Phase 0 findings" and "Phase 2". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch. Never launch X2_offline.exe without asking first.

Your task: make the Lanox local-map dungeon NAMES render. The Ranox icon lua
uses DLG_Localmap_Title_01.tga keys Ranox_* / Map_Title_Ranox, which March's
TET (data063, 4138 B) lacks and June's (data151, 6050 B) has. Script a check
of whether June's TET/TGA can be adopted outright (every March TITLE_* key has
identical UVs AND identical pixels). If not, merge the 7 Ranox rects into
March's atlas using the technique in LANOX_DEFECT_PHASES.md Phases 4/5
(bottom-up TGA, height-1-y flip). Stage, update the manifest, and hand me the
pack list. Do NOT create Icon_Info files for progress bars -- that is
JUNE_UI_PARITY_PHASES.md Phase 3.
```

**Done 2026-10-01.** The kickoff's test ("adopt outright only if identical UVs
AND identical pixels") is the **old**, since-corrected version of the
asset-conflict-priority rule (see "Fix philosophy" above). Re-ran the check
under the corrected test instead — every March key present in June's file at
a UV with real, matching-content art backing it — because the doc's own
correction specifically named this file as needing re-verification rather
than a reused answer either way.

- **Re-verification result: wholesale adopt fails the corrected test too**,
  for three different keys, found by rendering colour crops (BGRA, alpha
  composited over mid-grey, written with a from-scratch PNG encoder — the
  alpha-only ASCII art used earlier in this doc can't tell two name plates
  apart, since every plate has the same banner silhouette):
  - **`TITLE_ERDA_ISLAND`** (one of the 2 keys already flagged with a moved
    UV): March's art at this key is **"Underground Waterway"**, not Erda
    Island — the key name is stale/historical, but the plate is live and
    reads that way in-game today. June's art at its own UV for the same key
    reads **"Ereda Island"**. Different dungeon identity entirely; adopting
    would relabel a real, currently-shipped plate.
  - **`TITLE_HOLLOWEEN`** (same UV on both sides, so Phase 0's "51 of 53 are
    byte-identical" check — which only compared UV rects, not pixels — missed
    this): March has real Korean-text art, "마녀 뽀루의 은신처" ("Witch
    Pporu's Hideout"). June's copy at the exact same rect is **fully
    transparent (alpha 0 throughout)** — the art is simply gone. Adopting
    would blank this plate.
  - **`TITLE_HARMEL_SECRET`** (same UV both sides): March reads "Sander's
    Ordeal"; June reads "Temple of Trials" at the same key. Another relabel.
  - The other 48 same-UV keys were also pixel-diffed, not just UV-diffed: all
    show a uniform ~14-23% per-rect byte difference with no exceptions in
    that band. Rendering one (`TITLE_RUBEN_01`, "Tree of El") side by side
    showed no visible difference — consistent with a recompression/AA
    artifact, not a content change. Not exhaustively rendered one-by-one past
    that spot check, but the uniformity of the percentage (vs. the three
    outliers above, at 49.5% and 94.2%) is itself the evidence: a real
    content swap does not land in the same narrow band as every untouched
    plate.
  - `TITLE_ELDER_SECRET` (the other moved-UV key) renders identically in both
    ("Wally's Underground Laboratory"), just resized/recropped — passes the
    corrected test on its own, but doesn't matter once the other three fail
    it; one failing key is enough to rule out a wholesale file swap.
  - **Conclusion: merge, not adopt**, confirmed independently of Phase 0's
    original (UV-only) reasoning.
- **Precedence check:** `DLG_Localmap_Title_01.{TET,tga}` exists in exactly
  one live `.kom`, `data063.kom` — checked across all 206 live `.kom`/`.komi`
  files with `elstools inspect`. No Image11-style mount-order trap here.
- **The merge**, done with a from-scratch TET reader/writer
  (`KTETFormatHeader` + sorted key-offset `DWORD` array + 40-byte
  `KTETFormatTextureUV` array + length-prefixed UTF-16 string table; proven
  byte-exact round-trip against both the live March and June files before
  use) and a from-scratch TGA reader/writer (512×512, 32bpp BGRA,
  uncompressed truecolor, bottom-up storage — confirmed `origin_top=False`
  and header/footer byte-identical on both sides, so the only format work
  needed was the `height-1-y` row flip on read/write):
  - March's existing 53 `TET` entries carried through completely unchanged
    (byte-for-byte re-verified after rebuild).
  - June's 7 Ranox entries (`MAP_TITLE_RANOX`, `RANOX_ASHES_VILLAGE`,
    `RANOX_BURNED_FOREST`, `RANOX_FINALHALL`, `RANOX_FIRETEMPLE`,
    `RANOX_GATEWAY`, `RANOX_GEYSER`) added as-is (keys arrive pre-uppercased
    from June's file, matching the engine's own `MakeUpperCase` lookup
    convention — confirmed call sites in `KTDGUIControl.cpp`/`KTDGUIStatic.cpp`
    uppercase the key before `GetTexUV`). Result: 60 keys, still sorted by
    wide-string order (required for the binary search in `GetTexUV`),
    4692 B.
  - Confirmed no rect overlap, old or new, before touching pixels: none of
    the 7 new rects intersect each other or any of March's 53 existing rects.
  - Pixel copy: for each of the 7 new rects, copied every pixel from June's
    TGA to March's TGA at the *same* (x,y), through the bottom-up flip.
    14,590 pixels touched. **Verified by full re-scan of all 262,144
    pixels**: 0 differ from the original March file outside the 7 rects,
    all 14,590 inside match June's source exactly.
  - Net effect: every one of March's 53 existing plates — including
    `TITLE_ERDA_ISLAND`/Underground Waterway, `TITLE_HOLLOWEEN`, and
    `TITLE_HARMEL_SECRET`/Sander's Ordeal — is byte-for-byte untouched.
    "Sander is unchanged" in the Verification section above still holds, and
    now so does every other village's plate, not just Sander's.
- **No manifest change.** Both `DLG_Localmap_Title_01.TET` and `.tga` are
  already listed in `kom_contents/data063_kom/_manifest.json` at algo `"0"`
  (confirmed by reading the manifest directly) — this is a patch to existing
  file contents, not a new filename, so `march2014/update/data063/` and its
  manifest are untouched. The kickoff prompt's "update the manifest" doesn't
  apply here; noting that explicitly rather than editing a manifest that
  doesn't need it.
- **Staged** (binary assets, intentionally outside git per `march2014/**` in
  `.gitignore` — only `.lua`/`_manifest.json` are tracked):
  - `march2014/data063/DLG_Localmap_Title_01.TET` (patch, 4692 B)
  - `march2014/data063/DLG_Localmap_Title_01.tga` (patch, 1,048,620 B,
    unchanged size)
  - Matching copies written to `kom_contents/data063_kom/` (the user's
    packer reads from here, not from `march2014/update/`), confirmed
    byte-identical to the `march2014/data063/` copies with `cmp`.
- **Owed: pack `data063.kom` from `kom_contents/data063_kom/`.** Verify
  against the **live** kom afterward: `elstools inspect` should show the
  `.TET` at exactly 4692 B (4138 B means the merge didn't land; 6050 B would
  mean June's file landed wholesale instead, which per the findings above
  would be wrong).
- Play-test wording update: on the Lanox local map, the 7 new dungeon icons
  should show their name plates and `Map_Title_Ranox` should render. Every
  other village's plates, Elder/Erda/Harmel/Halloween included, should look
  exactly as they did before this patch — if any of those look different,
  that's a packing problem (stale `kom_contents` copy, or the wrong file
  landed), not an expected side effect.

---

## Phase 3 - Lanox tab in the dungeon list [item 3]

The tab is delivered by `JUNE_UI_PARITY_PHASES.md` Phase 2, which adopts
June's `DLG_UI_Local_Map_new.lua` and its `DLG_UI_Button21` atlas, where
June's `BTN_RANOX_N/O/D` already live. LANOX_DEFECT_PHASES Phase 4's
Button20_NEW merge is **superseded** and must not be done.

This phase covers only the loose ends:
1. `march2014/update/data056/_manifest.json` lists `DLG_UI_Button20_NEW.tga`
   / `.TET`, which are not on disk. Remove those entries, or remove the whole
   manifest if nothing else is listed, so that a data056 pack cannot fail or
   ship stale files. Record this in LANOX_DEFECT_PHASES Phase 4 as
   "superseded by JUNE_UI_PARITY_PHASES Phase 2".
2. If the user wants the tab before June-UI Phase 2 lands, stop and ask. Do
   not quietly revive the Button20 merge.
3. After June-UI Phase 2 ships: verify the Lanox tab exists, selects
   `LMI_RANOX`, shows the Ranox gates, and that its hit-box works.

**Run with:** Haiku 4.5, low effort. Manifest cleanup and a doc note; no review beyond confirming the manifest format (CRLF, 2-space indent, no trailing newline).

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md "Starting
context" and "Phase 3", and LANOX_DEFECT_PHASES.md "Phase 4". Branch:
mods/lanox-port. Your task: clean march2014/update/data056/_manifest.json of
the DLG_UI_Button20_NEW entries whose files are not on disk (keep the house
format: CRLF, 2-space indent, no trailing newline), and mark
LANOX_DEFECT_PHASES Phase 4's atlas merge as superseded by
JUNE_UI_PARITY_PHASES.md Phase 2. Then check whether JUNE_UI_PARITY Phase 2
has shipped. If it has, walk me through the play-test for the Lanox tab. If
it has not, stop and say so.
```

**Status 2026-10-01:**
- Steps 1-2 are done. The data056 manifest no longer lists Button20_NEW; only
  Button17 remains.
- `JUNE_UI_PARITY_PHASES.md` Phase 2 is built and deployed, which delivers
  this tab. See that doc's "Phase 2 result" for the pack list and play-test.
- Step 3 is still owed, and it needs the `data034`/`data215` repack listed
  there first.
- The Lanox tab now sits on **page 2** of June's tab strip. Reach it with the
  right arrow.
- The tab is gated by level only: 68, per March's
  `DLG_Map_Local_Ranox.lua`.
- June's tab loop does **not** test `RequireClearDungeonID` (Behimoss
  Heart). That key gates village entry, not the tab. See June-UI "Phase 2
  result", "Corrections to Phase 0".

---

## Phase 4 - World map: June tiles and Ranox labels [item 5]

**Root cause:** see the ledger. The art is baked into March's six tiles; the
C++ cloud logic works.

Steps:
1. Render all 12 tiles (March and June, 01-06) to PNG in the scratchpad
   (Pillow or a DDS decoder) and look at them.
   - Confirm that June's tiles show Lanox uncovered.
   - Check whether they show unported regions (Elysion) bare.
2. Unported regions: June covers Elysion with the `Cloud_Elision.tga` static.
   If June's tiles draw Elysion bare, port that cloud static into the staged
   `march2014/data034/DLG_UI_World_Map_NEW.src.lua`. **No `VMI_ELYSION`
   exists in March.** Read `UpdateWorldMap` to confirm that a cloud static
   whose `DummyInt(0)` matches no button stays shown, and choose a dummy int
   that cannot be nil. Never pass `VILLAGE_MAP_ID.VMI_ELYSION`, which is nil
   and aborts the dialog script. Stage `Cloud_Elision.tga/.TET` if it's
   needed.
3. Stage June's six `DLG_UI_World_Map0N.dds` in
   `march2014/update/data057/`. The TETs are identical, so leave them. Tiles
   01-02 are uncompressed with no FourCC: a clean load (no `ErrorLog.txt`
   texture error) is a pass criterion.
4. Labels: take Phase 0's rows for `DLG_UI_Common_Texture71_NEW`
   (`TOWN_RANOX`, `FIELD_NAME_RANOX_*`, `FIELD_REST_RANOX`) and adopt or merge
   as in Phase 2. Also check `DLG_Common_Button02_NEW`'s button keys.
5. Check `DLG_UI_World_Map_Front_NEW.lua` (the navi overlay) for the same
   tile/atlas issue.
6. Hand the pack to the user. Verify against the live kom.

Verification:
- Both M (world map) and Esc → world map show June's art.
- The Lanox town and field pins are visible and clickable, with labels.
- At a level below Lanox's requirement, the Ranox cloud still shows (the
  generic logic is intact).
- Elysion is not revealed.
- No texture errors.

**Run with:** Sonnet 5, medium effort. Data staging plus a possible cloud-static lua edit; light review of the lua diff (no nil VMI_ELYSION reference).

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger",
"Phase 0 findings" and "Phase 4". Branch: mods/lanox-port. Do NOT consult
NEW_VILLAGE_PLAN.md, NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the
mods/lanox-offline branch. Never launch X2_offline.exe without asking first.

Your task: replace the old world-map art (Lanox under clouds) with June's.
March's six data057 DLG_UI_World_Map0N.dds tiles are old art; their TETs are
identical to June's, and the cloud show/hide C++ is generic and correct.
Render both sets to PNG and look before adopting. If June's tiles draw
Elysion bare, port June's Cloud_Elision static WITHOUT referencing any
VMI_ELYSION (nil in March -- it aborts the dialog). Also fix the
DLG_UI_Common_Texture71_NEW Ranox labels per Phase 0. Stage, update the
manifests, and hand me the pack list. Tiles 01-02 are uncompressed with no
FourCC; a clean load with no ErrorLog texture error is a pass criterion.
```

**Done 2026-10-01.** Rendered all 12 tiles to PNG (wrote a from-scratch DDS
decoder: DXT1/3/5 block decode plus uncompressed RGB, the latter needed a
fix for `DDSD_LINEARSIZE` vs `DDSD_PITCH` — tiles 01-02's header flags
`0x81007` carry the *total* image size in the "pitch" field, not a per-row
stride, and treating it as a stride produced an all-black decode). Composited
March's and June's six tiles into two 1800x1200 mosaics per
`DLG_UI_World_Map_NEW.src.lua`'s own `LEFT_TOP` grid (3x2, 600px pitch) and
viewed them side by side.

- **June's tiles show Lanox fully revealed** (the volcanic region bottom-right,
  previously entirely cloud-baked in March's art) **and draw Elysion bare**: a
  circular floating-island medallion at the tile seam, roughly (1400,250)-
  (1800,700) in canvas space.
- Staged June's six `DLG_UI_World_Map0N.dds` as a patch in `march2014/data057/`
  (TETs confirmed byte-identical to March's, so left untouched, per Phase 0).
- **Elysion cloud, ported from June, flag `SERV_IRUHADEV_RANOX_PORT`:** June
  covers that medallion with its own static (`g_pStatic_ELISION_CLOUD`, texture
  `Cloud_Elision.tga` key `Bg_Cloud_Elision`, `LEFT_TOP(1300,262)`), gated on
  `VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELYSION_REST_00` — which doesn't exist in
  March and is nil. Added the same static to
  `march2014/data034/DLG_UI_World_Map_NEW.src.lua`, same texture/key/position
  (June's own values, not invented), but with a literal sentinel `DummyInt(0)`
  (`99999`) instead of the nil enum lookup: traced `CX2WorldMapUI::UpdateWorldMap`
  (`X2WorldMapUI.cpp:419-665`, the only code in the client that reads
  `UCT_STATIC`/`GetDummyInt`/`SetShow` together) and confirmed it only
  show/hides a static when some *button*'s `DummyInt(0)` matches it; since no
  button in this dialog carries `99999`, the loop never reaches this static and
  it stays at `CKTDGUIControl`'s construction default (`m_bShow = true`,
  `KTDGUIControl.cpp:16`) forever — permanently shown, which is what unported
  content needs. Compiled with `elstools compile-batch -v mar-2014` and
  round-tripped through `decompile-batch` to confirm the new block survives
  (47956 B compiled); staged the compiled `.lua` into
  `kom_contents/data034_kom/`.
- Staged `Cloud_Elision.tga`/`.TET` (June's bytes, unmodified) as a new file in
  `march2014/update/data057/`, manifest updated (both `march2014/update/
  data057/_manifest.json` and `kom_contents/data057_kom/_manifest.json`).
- **Labels (step 4): already resolved**, confirmed again this session —
  `DLG_UI_Common_Texture71_NEW.TET`/`.tga` live in `data057.kom` are still
  byte-identical to June's (both files), matching Phase 0 section B. No work
  needed.
- **`DLG_Common_Button02_NEW` (step 4's button-key check): already
  byte-identical** live (`data063.kom`) to June's copy, both files. No work
  needed.
- **Navi overlay (step 5): found and fixed a second atlas the plan didn't
  name.** `DLG_UI_World_Map_Front_NEW.lua`'s drag-handle thumbnail reads
  `WORLDMAP_NAVIGATION` out of `DLG_UI_Common_Texture70_NEW.tga` — a *different*,
  smaller (512x512) pre-rendered copy of the same world map, not a scaled
  render of the big tiles. Parsed both TETs (wrote a from-scratch reader
  against `KTETFormatHeader`/`KTETFormatTextureUV`'s on-disk layout) and
  confirmed all 14 of March's existing keys, `WORLDMAP_NAVIGATION` included,
  have byte-identical rects in June's 20-key copy (June only adds 6 unrelated
  keys, `BG_LUCIEL_*`/`BT_SWITCHING_*`) — a clean wholesale-adopt under the
  corrected asset-conflict rule. Cropped and compared the `WORLDMAP_NAVIGATION`
  rect pixel-for-pixel between March's and June's TGA (`navi_compare.png`):
  March's copy is the same old cloud-baked art as the big tiles; June's shows
  Lanox revealed and the Elysion medallion. Adopted June's file wholesale
  (both `.tga` and `.TET`) as a patch in `march2014/data057/`. Also checked
  `DLG_Common_New_Texture23.tga` (the navi's drag-box border) — already
  byte-identical live (`data053.kom`) to June's copy.
- **Caveat the plan didn't anticipate, flagged rather than silently accepted:**
  the navi dialog (`DLG_UI_WORLD_MAP_FRONT`) has no per-village cloud-static
  mechanism of its own — it's one static picture, no `UpdateWorldMap`-style
  show/hide loop reads it. Adopting June's `Texture70` wholesale means the
  small navi-corner thumbnail now shows Lanox revealed *regardless of player
  level* (the big map still gates correctly) and shows Elysion's medallion with
  no cloud over it at all (nothing to gate it on, same nil-VMI problem as the
  big map, but there's no generic mechanism here to attach a sentinel-gated
  cloud to). This is a real, if minor and easy to miss, content leak in one
  266x208px corner view. It meets the letter of the "June asset wins" rule
  (no March key is missing or moved), but is a user call, not an engineering
  one — left as-is pending a decision; a follow-up would need either a painted
  cloud baked directly into the `WORLDMAP_NAVIGATION` rect of `Texture70.tga`,
  or a small navi-specific hide check added to the dialog's open/update path.
- **Full mount-order scan (all 184 live `.kom`s, `data001-230` and all 23
  `datai001-023`)** for every filename touched (`DLG_UI_World_Map01-06.dds`,
  `DLG_UI_Common_Texture70_NEW.TET/.tga`, `Cloud_Elision.TET/.tga`,
  `DLG_UI_World_Map_NEW.lua`) found exactly one live copy of each, in
  `data057.kom`/`data034.kom` as staged — no Image11-style mount-order trap
  here.
- **Position cross-check:** every `SetPoint`/`LEFT_TOP` in
  `march2014/data034/DLG_UI_World_Map_NEW.src.lua` that has a same-named
  counterpart in June's `DLG_UI_World_Map_NEW.src.lua` (55 shared controls,
  7 of them Ranox-named) matches exactly — June's redraw didn't move any
  pin, button or label, so no position drift to port.
- **Simulated the cloud placement** (composited `Cloud_Elision.tga` onto the
  June tile mosaic at `(1300,262)` in Python) before handing off: the cloud
  fully covers the medallion, Lanox stays visible.

**Pack list:**
- `data057.kom`, built from `kom_contents/data057_kom/` (also mirrored in
  `march2014/data057/` and `march2014/update/data057/`, the latter with its
  `_manifest.json` updated):
  - `DLG_UI_World_Map01.dds` / `02.dds` (786560 B, uncompressed RGB24, no
    FourCC) and `03.dds`-`06.dds` (174904 B, DXT1 + 10 mips) — patches,
    June's bytes, TETs untouched (78 B each, unchanged).
  - `DLG_UI_Common_Texture70_NEW.tga` (1048620 B) / `.TET` (1532 B) — patch,
    June's bytes wholesale.
  - `Cloud_Elision.tga` (1048620 B) / `.TET` (92 B) — new file, June's bytes.
- `data034.kom`, built from `kom_contents/data034_kom/`:
  - `DLG_UI_World_Map_NEW.lua` (47956 B compiled bytecode). The `.src.lua`
    sitting alongside it in `kom_contents/data034_kom/` is reference only —
    confirm `kom_contents/data034_kom/_manifest.json` doesn't list it before
    packing, so it doesn't ship.

**Still to do:** the user packs both koms and play-tests. The tiles 01-02
uncompressed-RGB load is the one real engine-behavior unknown here (everything
else is byte-identical reuse of June's own already-shipped data) — check
`ErrorLog.txt` for any `OpenTexture`/`KEM_ERROR13[56]` line naming
`DLG_UI_World_Map01` or `02` after a clean load. Verify: M-key and Esc world
map show June's art; Lanox pins visible and clickable with labels; a
level-below-68 character still sees the Ranox cloud; Elysion not revealed on
the main map (the navi-corner caveat above is separate and known). Never
launch `X2_offline.exe` without asking first.

---

## Phase 5 - M-key field map follows the character [item 1]

**Root cause:** see the ledger. In a Lanox village or field,
`CX2WorldFieldMap` projects the character with a camera that
`ResetCameraMatrix` never set for Ranox VMIs, and `UpdateNaviMap` has no
Ranox background.

Steps (port whole June functions, not just Ranox cases):
1. Find June's `ResetCameraMatrix`. Scan `unpacked_x2.exe`'s raw bytes for
   March's known floats (Ruben eye `1274.792f`, `1125.527f`, `-5136.493f`,
   look-at `2414.343f`, …) as little-endian float32. Map the file offset to a
   VA through the PE section table, then call `get_function_by_address` and
   decompile. Find `ResetMapPosAndSize` the same way, or as a sibling in the
   same class (xrefs from the field-map object's vtable or ctor).
2. Find June's `UpdateNaviMap` via `strings?filter=TownMap_Navigation` →
   `xrefs_to`. Check whether the Ranox navi key exists in any March or June
   `DLG_UI_Common_Texture_Village` TET. If only June has it, add it to the
   Phase 0 table and to this phase's staging.
3. Diff every case in June's three functions against March's. Port the whole
   functions under a new flag, e.g. `SERV_IRUHADEV_JUNE_FIELD_MAP`. Omit
   June-only VMIs March lacks, with a comment.
4. Also check June's version of `CX2FieldMiniMap::ResetWholeMapTexture`
   (`X2FieldMiniMap.cpp:318`), the `ReLoadingVillageMap` bounds
   (`X2WorldMapUI.cpp:2437`, stops at `VMI_SANDER`) and `IsVillageField()`
   (`X2LocationManager.h:487`). Port whatever June extends.
5. Build and deploy. Ask before launching.

Verification: in the Lanox village and in each of the 4 fields plus the rest
area, press M.
- The field map shows the right background.
- The character marker moves as the character walks and matches its real
  position.
- Party-member markers track the same way.
- Ruben, Sander and the others are unchanged.

**Run with:** Opus 5.5, high effort. Ghidra RE plus a whole-function C++ port; code review required before the build is handed over.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger" and
"Phase 5". Branch: mods/lanox-port. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch. Never
launch X2_offline.exe without asking first.

Your task: make the M-key field map follow the character in the Lanox village
and fields. CX2FieldMapBase::ResetCameraMatrix / ResetMapPosAndSize
(X2FieldMapBase.cpp) and CX2WorldMapUI::UpdateNaviMap (X2WorldMapUI.cpp:2178)
have no Ranox cases. Recover June's versions from unpacked_x2.exe in Ghidra
(ghidra skill). Locate ResetCameraMatrix by byte-scanning for March's known
Ruben float constants, and UpdateNaviMap via the "TownMap_Navigation" string
xref. Port the WHOLE June functions (no partial June updates), diffing every
case; omit June-only VMIs with a comment and never invent values. Also check
FieldMiniMap::ResetWholeMapTexture, ReLoadingVillageMap's VMI_SANDER loop
bounds and IsVillageField(). Put everything behind a new SERV_IRUHADEV_ flag,
then build and deploy.
```

---

## Phase 6 - Hide Secret Hell [item 7]

**Investigate first.** The user asked for this explicitly. March never
officially shipped a Lanox secret dungeon, but the port exposes Secret Hell.

Steps:
1. List every entry point and what each does today:
   - `DLG_Map_Local_Ranox.lua` `DungeonIDList` (`DI_RANOX_SECRET_HELL`)
   - the `Ranox_Secret` button and the `SecretDungeonDesc` button in
     `DLG_Map_Local_Ranox_Icon_NEW.lua`
   - the party UI's dungeon picker (how X2PartyUI enumerates dungeons per
     local map)
   - quest references to 41271 (QuestTemplet / subquest data)
   - `IsSecretDungeon` / secret-dungeon entry rules in C++ and in the offline
     emulator (`X2Lib/Offline/`)
   - the auto-party / dungeon-matching lists
   - `DungeonData`'s own `AddDungeonData` block
2. Find how March hides content it does not support. Look for a
   `SCRIPT_SELECT`-style mechanism (`X2WorldMapUI.cpp:466` hides controls
   whose local-map templet is missing) or a precedent where a dungeon exists
   in data but isn't listed. Prefer the studio's own mechanism.
3. Implement the hide **in data where possible**, with restore comments that
   keep the original line commented out plus a one-line restore instruction.
   Touch C++ only if a hardcoded path lists the dungeon anyway, and put that
   behind `SERV_IRUHADEV_RANOX_HIDE_SECRET`. Do **not** delete the ported
   Secret Hell files. Unhiding later must be a revert.
4. Confirm that nothing else still points players at it, e.g. a quest that
   requires clearing it.

Verification:
- The Lanox local map shows no secret-dungeon button or lock plate.
- The party dungeon picker has no Secret Hell entry.
- Normal Lanox dungeons are unaffected.
- `offline_packets.log` has no new UNHANDLED lines.

**Run with:** Sonnet 5, high effort (many entry points to enumerate). Light review for data-only hiding; review required if any C++ is touched.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger" and
"Phase 6", plus RANOX_PORT_PLAN.md "Phase 9" and its findings. Branch:
mods/lanox-port. Never launch X2_offline.exe without asking first.

Your task: hide the Lanox secret dungeon (Secret Hell, DI_RANOX_SECRET_HELL =
41271), which the March client never officially supported. INVESTIGATE
FIRST. Enumerate every entry point (local-map DungeonIDList, the Ranox_Secret
and SecretDungeonDesc icon buttons, the party dungeon picker, quest
references, the IsSecretDungeon classifier, the offline emulator,
auto-party lists) and report what each does today. Then look for March's own
precedent for hiding unsupported content and use it. Hide in data with
restore comments where possible; C++ only if unavoidable, behind a new
SERV_IRUHADEV_ flag. Do not delete any ported Secret Hell file -- unhiding
must be a revert.
```

---

## Phase 7 - Distant Ruin demon colours [item 8]

**Get ground truth before theorising.** The user said "blue iirc".

Steps:
0. Precondition: the user has repacked `data002.kom` (see the ledger's
   "Staged but never packed" bullet). **The repack is done:** all 77 files
   were confirmed live on 2026-09-30. Still to do: ask the user to re-fight
   Distant Ruins and
   re-read `ErrorLog.txt`. The two `..._AttackA_Ready_Fire*.dds` misses must
   be gone. Only then judge the colours, so a missing file isn't mistaken for
   a render-path bug.
1. Render `NUI_FIRE_DEVIL_FIGHTER_Body_A/B`, `..._INCHANT_BODY_A/B`,
   `..._INCHANT_BODY_A_FD_01` and `..._INCHANT_BODY_B_FD_02` (and the
   Berserker equivalents) to PNG. Confirm which ones are blue, and show the
   user.
2. A/B test, cheap: temporarily build with the `SetUVAnimGlow` /
   `RT_LIGHT_FLOW_LOCAL_Z` override in `X2SubEquip.cpp:167-177` disabled,
   behind a short-lived `SERV_IRUHADEV_*_DEBUG` flag. Does the colour change?
   This separates "wrong textures" from "wrong render type".
3. RE June's `ATTACH_SKIN_MESH` handling (the lua key strings are
   XOR-encrypted; see `june2017_re/xorstrings.tsv`, or find it via the
   `CX2SubEquip`-equivalent ctor):
   - how June applies the block's own `MOTION_CHANGE_TEX_XET`
   - whether the unit's `MOTION_CHANGE_TEX_XET` is chained onto attached
     skins
   - what render type `SMRT_UV_ANIM` really maps to, and whether it is an
     overlay pass or a replacement
   - what loads `Motion_FIRE_DEVIL_FIGHTER_INCHANT.x` (present in live
     data153.kom, referenced by neither checked mesh), which is the most
     likely consumer of the `INCHANT_BODY_A` → `_FD_01` swap
4. Port June's parse and render path **whole**, replacing the heuristic in
   `SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH` (X2GUNPC.cpp:4442-4479,
   X2SubEquip.cpp). Update `RANOX_EFFECT_ENGINE_GAPS.md`. The dungeon
   (non-FD) FIRE_DEVIL and `NUI_SECRET_*` variants share the path, so
   re-check them too.

Verification: in Distant Ruin, Twilight Demon Fighter and Berserker match
June's colours as shown by the PNG ground truth. The dungeon FinalHall
variants are unchanged or improved, never regressed.

**Run with:** Opus 5.5, high effort. Ghidra RE of the attach-skin render path and a whole port; code review required, including a check that the dungeon FIRE_DEVIL and NUI_SECRET_* variants did not regress.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger" and
"Phase 7", plus RANOX_EFFECT_ENGINE_GAPS.md's ATTACH_SKIN_MESH section.
Branch: mods/lanox-port. Never launch X2_offline.exe without asking first.

Your task: in Distant Ruin (VMI_BATTLE_FIELD_RANOX_FIELD_04), Twilight Demon
Fighter/Berserker (NUI_LOW_FINALHALL_*_FD) look yellow; I recall blue. All
data is byte-identical to June. The suspect is the
SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH port (X2GUNPC.cpp:4442, X2SubEquip.cpp
:159), which drops ATTACH_SKIN_MESH0's MOTION_CHANGE_TEX_XET, never chains
the unit's _FD.xet, and forces RT_LIGHT_FLOW_LOCAL_Z. My screenshot shows
dark bodies with white-yellow glowing spikes. Step 0 first: confirm that
data002.kom now holds the 12 previously unpacked files (ledger), and ask me
to re-fight and check ErrorLog.txt (the data002 repack is already done
and verified). Then render the body
textures to PNG and show me which are blue. Then do the cheap A/B with the
render override disabled behind a debug flag. Then RE June's attach-skin
path in Ghidra (including what loads Motion_FIRE_DEVIL_FIGHTER_INCHANT.x) and
port it whole.
```

---

## Phase 8 - Burned Forest background [item 9]

**Likely cause found, but the fix has not been tested yet.** See the
ledger's Item 9: `BossRaid_Abyss_smog.dds` was never ported. It is staged in
`march2014/update/data023/`, but the 2026-09-30 repack did not include it,
because the packer reads `kom_contents/data023_kom/`. The anomaly is still
visible in the second screenshot, taken without the texture. The user
deferred further investigation to a later session.

Steps:
1. ~~Live-kom check~~ **Done 2026-09-30** (ledger, Item 9). Only the smog
   texture was missing.
2. ~~Screenshot~~ **Received 2026-09-30** (ledger, Item 9).
3. **Start here.** Get the texture into the live data023.kom:
   - Check `kom_contents/data023_kom/`. If `BossRaid_Abyss_smog.dds` is not
     there, ask the user to copy it from `march2014/update/data023/` and to
     add `"BossRaid_Abyss_smog.dds": "0"` to that dir's `_manifest.json`
     (same house format), then repack. The data023 repack on 2026-09-30
     missed this step.
   - Confirm with `elstools inspect -q -i data023.kom` that it now lists
     `BossRaid_Abyss_smog.dds` at 87507 B and holds 548 files. The pack
     timestamp is not proof.
   - Do **not** put a loose copy in the game dir, which would beat the
     archive and hide a bad repack.
   Then ask the user to re-enter Burned
   Forest, check that `ErrorLog.txt` has no `BOSSRAID_ABYSS_SMOG` miss, and
   compare the lower layer with the 2026-09-30 screenshot, in all four
   stages.
4. If the lower band is fixed, the phase is done. If it survives, work the
   remaining suspects in order:
   - the WorldData 583-586 → file mapping (`march2014/data040/WorldData.lua`)
   - whether the script aborted part-way (`OpenScriptFile.log`)
   - fog: negative density `-0.2` in `SetFogWorld` / `SetFog`. Diff March's
     `CX2World::SetFogWorld`, `CKTDGXMeshPlayer`/world-object `SetFog` and
     the fog render state against June's (Ghidra).
   - the sky layer (`XL_SKY_WORLD_BASE_0`) and `SetProjection(10, 500000, …)`
     far plane
   - `SetOriginalColorFactor` semantics
   - `SetFeedBackUV` / `SetTex0UVSpeed`
5. Fix per the finding. A June engine difference gets a whole-function port
   behind a new flag; missing data gets staged and packed.

Verification: every Burned Forest stage renders its background like June's,
and the other Ranox dungeons are unchanged.

**Run with:** Sonnet 5, low effort for step 3 (the data023 kom_contents copy + repack check; no review). If the band survives the repack, switch to Opus 5.5, high effort, for step 4 (fog/sky/projection RE); review required for any C++ fix.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger" and
"Phase 8". Branch: mods/lanox-port. Never launch X2_offline.exe without
asking first.

Your task: finish the Burned Forest background fix
(WI_RANOX_BURNDEDFOREST_STAGE0..STAGEBOSS = 583-586). The likely cause is
already found: BossRaid_Abyss_smog.dds, used by all 11 cloud/smoke/smog
meshes, was never ported; it is staged in march2014/update/data023/, but the
2026-09-30 data023 repack missed it because the packer reads
kom_contents/data023_kom/ (see the ledger, Item 9). First check that
kom_contents dir; if the texture is not there, ask me to copy it in, add it
to that dir's manifest and repack. Then confirm with elstools inspect that
the LIVE data023.kom has it (87507 B, 548 files). No loose copy in the game
dir. Then have me re-enter every
Burned Forest stage, and check ErrorLog for BOSSRAID_ABYSS_SMOG. If the lower
brown/red band from the 2026-09-30 screenshot is still there, work the
remaining suspect list in order (WorldData mapping, script abort, fog with
negative density, sky layer/projection, colour factors), confirming each
with evidence before fixing.
```

---

## Phase 9 - Silent Night Shelter has no NPCs [item 10]

**Root cause, from data (see the ledger):**
`march2014/data034/DLG_Map_House.src.lua` has no `COMMON_NPC_LOCATION` row
for `WI_BATTLE_FIELD_RANOX_FIELD_REST_00` in `HI_HELLEN_SHOP_HOUSE`,
`HI_POSTBOX`, `HI_BILLBOARD` or `HI_GLAVE_HOUSE`. So `AddFieldNpc`'s
`bFindNpcPos` gate never creates those NPCs. This is the same fix shape as
LANOX_DEFECT_PHASES bug A.

Steps:
1. **Check the base before patching.** Extract `DLG_Map_House.lua` from the
   live `data034.kom` (the winning copy, if the name is in several archives)
   and decompile it. Confirm that it matches
   `march2014/data034/DLG_Map_House.src.lua`, including bug A's
   `WI_VILLAGE_RANOX` rows. If they differ, stop and report which is newer
   before editing anything.
2. Append the four rest rows from June (ledger table) to the four houses'
   `COMMON_NPC_LOCATION` tables. Copy the row shape verbatim from each house's
   existing `WI_BATTLE_FIELD_SANDER_FIELD_REST_00` row and do not restructure
   the table. Add a `-- SERV_IRUHADEV_RANOX_PORT` comment saying where the
   rows came from.
3. Decide the 4 missing `WI_VILLAGE_RANOX` rows (`HI_ADAMS_HOUSE`,
   `HI_ED_SHOP_MU`, `HI_EVENT_DOLL`, `HI_LUTO`):
   - Check whether each house is in the Ranox village's `HouseIDList` in
     `DLG_Map_Village.src.lua`, and whether its house block and NPC exist in
     March.
   - Add the row only where both are true. Otherwise record it as June-only.
   - Cross-check against the `[RANOXDBG]` empty-`NPC_ID_LIST` houses
     (40083, 40090, 40129, 40130, 40140), and note in the ledger whether they
     are the same houses.
4. Leave out the June-only rows (`WI_SPA_RANOX`, `HI_APINK`'s Sander-rest
   row) after confirming that the world or house is absent in March. Do not
   invent them.
5. Separately, look at the `MapData.lua` lead: no `AddMapData` for
   `RANOX_REST_00`, and no Ranox-rest case in `CheckCOBOExpressTicketMapID`.
   Compare with June's `MapData` and record what it affects (warp ticket? map
   list?). Fix it only if it is a real, sourceable gap, as its own step.
6. Compile with `elstools compile-batch -v mar-2014`. Hand-off: copy the
   compiled `DLG_Map_House.lua` into
   `kom_contents/data034_kom/` (it is an existing entry, so the manifest
   already has it at `"2"`; confirm that), then ask the user to repack
   data034. Afterwards verify the live `data034.kom` with `elstools inspect`
   (the size changed) and by extracting and decompiling it.

Verification:
- Enter the Silent Night Shelter: Hellen (shop), Glave, the billboard and
  the postbox all stand at June's positions, and each can be interacted with.
- The M-key/field-map quest marks for them appear too (the other
  `m_vecCommonPos` consumer).
- Sander's rest area and the Lanox village are unchanged.

**Run with:** Sonnet 5, medium effort. Data-only row additions; light review (diff the patched DLG_Map_House against June's rows and confirm nothing else moved).

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger"
(Item 10) and "Phase 9" -- and LANOX_DEFECT_PHASES.md's bug A (the same fix
for the village). Branch: mods/lanox-port. Do NOT consult NEW_VILLAGE_PLAN.md,
NEW_VILLAGE_RANOX_IDENTIFIERS.md, or the mods/lanox-offline branch. Never
launch X2_offline.exe without asking first.

Your task: make the Lanox rest area (Silent Night Shelter,
VMI_BATTLE_FIELD_RANOX_REST_00 / WI_BATTLE_FIELD_RANOX_FIELD_REST_00) show its
NPCs. DLG_Map_House.lua has no COMMON_NPC_LOCATION row for that world in
HI_HELLEN_SHOP_HOUSE / HI_POSTBOX / HI_BILLBOARD / HI_GLAVE_HOUSE; June's four
rows are in the ledger. First confirm that the staged
march2014/data034/DLG_Map_House.src.lua matches the LIVE data034.kom copy.
Then append the four rows (shape copied from each house's Sander-rest row),
decide the 4 missing WI_VILLAGE_RANOX rows per Phase 9 step 3, and leave the
June-only spa/APINK rows out. Compile with -v mar-2014, put the output in
kom_contents/data034_kom/ (check its manifest), and hand me the repack. After
I pack, verify against the live kom.
```

---

## Phase 10 - Fire Temple collapsing floor leaves the player stuck [item 11]

**Likely cause (see the ledger):** June's collapse trigger re-types the pit
floor under the collapsing line from `LT_MONSTER_ROAD` to `LT_NORMAL`. The
March port commented that out, so the player lands on a floor only monsters
can use.
- The engine support (`TAT_LINE_CHANGE_TYPE`, Ranox Phase 14) is already
  compiled in.
- What is missing is the Lua half: the `Enum.lua` names and the restored
  trigger rows.
- The play-test proves it.

Steps:
1. **Settle the enum numbers first (read-only).**
   - Tabulate `TRIGGER_MESH/LINE/ETC_ACTION_TYPE` from three places: March
     `KTDGLineMap.h` with each Phase 14 flag's value, the
     `KncWX2Server/Common/LineMap/XSLLineMap.h` mirror, and June's
     `Enum.src.lua` (`data036_kom`, `:1979-1998` area).
   - The etc enum disagrees: March C++ has NPC_AI 6 / EFFECTSET 7 /
     TALK_BOX 8, and June has TALK_BOX 6 / EFFECTSET 7 / NPC_AI 8.
   - Decide, and ask the user if it is not clear-cut, between:
     - (a) adding the names to March's `Enum.lua` at March's C++ values
       (data-only, no rebuild)
     - (b) renumbering the C++ (and the `XSLLineMap.h` mirror) to June's
       values first, for June parity. This is a header/PCH rebuild; the
       server mirror is values-only, so no wire change.
   - Whichever you choose, the Lua values must equal the compiled C++
     values. **Never paste June's etc block unchanged under option (a).**
2. **Stuck-floor fix.**
   - Add the Phase 14 names to `march2014/data036/Enum.src.lua` at the values
     chosen in step 1: `TAT_LINE_CHANGE_TYPE`, `TAT_ETC_NPC_AI_ON_OFF`,
     `TAT_ETC_PLAY_EFFECTSET`, `TAT_ETC_TALK_BOX` and `TAT_MESH_MOVE`. Put a
     `-- SERV_IRUHADEV_RANOX_PORT` comment on each.
   - Restore `TRIGGER_DATA_LINE1/2/3` in triggers 0 and 1 of
     `march2014/update/data040/Ranox_FireTemple_Stage1_LineMap.src.lua`,
     verbatim from June (lines 11/12/13 and 33/34/35,
     `CHANGE_LINE_TYPE = LINE_TYPE.LT_NORMAL`).
   - Replace the "omitted" comments with a short note that the rows were
     restored, saying when and which flag they need.
3. **NPC_AI_ON_OFF, decided separately.**
   - Before restoring trigger 0/1's `TRIGGER_DATA_ETC2` (AI on) or triggers
     2/3's bodies (AI off), recover June's `DoTriggerEtc`
     `TAT_ETC_NPC_AI_ON_OFF` handler in Ghidra and compare it with Phase 14's
     "every NPC in the game" implementation.
   - If June only affects enemy NPCs, or scopes it some other way, port that
     scoping whole (a new flag, or an extension of the existing one) and then
     restore all four rows.
   - If the semantics can't be settled, leave triggers 2/3 empty and say so.
   - Never restore an AI-off row that can freeze the user's AI-party bots
     (the AI-party bots are NPC units, see `AI_PARTY_PLAN.md`).
4. **The other three linemaps** (`Ranox_GateWay_Stage1_LineMap` x2,
   `Ranox_Geyser_stage0_linemap` x3, `Ranox_Secret_Stage2_LineMap` x1).
   `Enum.lua` is being repacked anyway, so restore their commented entries in
   the same pass, following the same rules: verbatim from June, and
   NPC_AI rows per step 3.
   - Secret Hell interacts with Phase 6 (hiding it). Restore its row anyway,
     because the files stay ported.
   - List every restored row in the ledger.
5. Compile every touched `.lua` with `elstools compile-batch -v mar-2014`.
   Hand-off:
   - `Enum.lua` → `kom_contents/data036_kom/`
   - the linemaps → `kom_contents/data040_kom/` (they are new Ranox files;
     confirm each is in that dir's `_manifest.json` at `"2"`)
   - The user repacks data036 and data040.
   - Verify both live koms with `elstools inspect` (the sizes change), then
     extract and decompile to confirm the restored rows and enum names.
6. If step 1 chose option (b), build and deploy, and ask before launching.

Verification:
- Fire Temple Normal, stage 2 (`STAGE1`): step onto the collapsing floor
  (line 3). The floor animates down, the player falls into the pit and can
  walk left and right there, and the stage continues.
- The same holds at the second collapse (line 26 → pit 33-35).
- Repeat on Hard (same world).
- GateWay stage 1 and Geyser stage 0 triggers behave as in June (mesh moves,
  talk boxes, effect sets).
- AI-party bots keep acting throughout.
- `ErrorLog.txt` shows no Lua error for the touched files.

**Run with:** Opus 5.5, high effort. The enum-number decision and the NPC_AI_ON_OFF Ghidra check are the whole risk; review required for the Enum.lua values against the compiled C++ (and any C++ renumber) before packing.

**Kickoff prompt**:
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Investigation ledger"
(Item 11) and "Phase 10" -- and RANOX_PORT_PLAN.md's "Phase 14" section
(executed 2026-09-19). Branch: mods/lanox-port. Never launch X2_offline.exe
without asking first.

Your task: fix the Fire Temple (Collapsing Temple of Fire) stage 2 collapse.
The floor animates down, but the player lands in a pit whose lines 11/12/13
(and 33/34/35 at the second collapse) are LT_MONSTER_ROAD, so the player is
stuck. June's trigger re-types them with TAT_LINE_CHANGE_TYPE ->
LT_NORMAL. Phase 14 already compiled that action into the C++, but the Lua
half was never done: the Enum.lua names are missing and the trigger rows are
still commented out in 4 linemaps. FIRST settle the enum numbers: March's C++
TRIGGER_ETC_ACTION_TYPE values differ from June's Enum.lua (NPC_AI 6 vs 8,
TALK_BOX 8 vs 6), so never paste June's block unchanged -- the Lua values
must equal the compiled C++. Restore the CHANGE_TYPE rows verbatim from June.
Treat NPC_AI_ON_OFF separately: Phase 14 toggles AI for EVERY NPC, so check
June's handler in Ghidra before restoring any AI-off row (my AI-party bots
must not freeze). Compile with -v mar-2014, stage into
kom_contents/data036_kom and data040_kom (check the manifests), and hand me
the repack list.
```

**Executed 2026-10-01.** The data is staged and the exe is built. The repack
and the play-test are still owed.

- **Enum numbers: option (b).** The etc block's March values (NPC_AI 6,
  TALK_BOX 8) were not studio numbers. Phase 14 invented them. June's
  compiled `AddTrigger_LUA` (`0x31c77e20`) settles it: etc case 6 reads
  `MESSAGE`, case 7 reads `MOTIONNAME_EFFECTSETNAME`, case 8 reads
  `NPC_AI_ENABLE`. New flag `SERV_IRUHADEV_JUNE_LINEMAP_TRIGGER`
  (`KTDXLIB/Always.h`) renumbers both header copies to June's values
  (`TALK_BOX = 6`, `NPC_AI_ON_OFF = 8`). `MESH_MOVE = 3`,
  `LINE_CHANGE_TYPE = 7` and `PLAY_EFFECTSET = 7` already agreed.
  `march2014/data036/Enum.src.lua` now carries all five names at those
  values. June's `TAT_LINE_CHANGE_MOVABLE_DATA = 8` is left out, because
  March has no C++ for it. **The exe and data036 must ship together.** An
  exe without the flag would run a restored NPC_AI row (8) as a talk box.
- **The same flag moves Phase 14's field names to June's.** Phase 14 had
  invented the names `MOVE_POS_X/_Y/_Z`, `TALK_POS_*` and
  `EFFECT_NAME`/`EFFECT_POS_*`. No June row uses any of them, so a verbatim
  GateWay row would have moved its rock to the origin.
  - `TAT_MESH_MOVE` now reads one `MOVE_POS` vector (via
    `LUA_GET_USER_DEFINED_TYPE_VALUE`, the same reader `AddLine_LUA` uses
    for `START_POS`).
  - `TAT_ETC_TALK_BOX` now reads `MESSAGE`, `REMAIN_TIME` (default 2.0,
    stored in `m_fDummyValue01`, the slot June uses) and a `MESSAGE_POS`
    vector.
  - The TALK_BOX runtime now matches June's `DoTriggerEtc` (`0x32800e10`)
    case 6. It shows only in `XS_DUNGEON_GAME` (11) and sets
    `m_fRemainTime` and `m_bUseOutLine`. June's struct offsets line up
    with March's `TalkBox`: `+0x14` trace, `+0x3c` remain, `+0x40` type 2,
    `+0xa0` outline.
- **Mechanism check.** June's `DoTriggerLine` (`0x328004a0`) case 7 is the
  same bare `lineType` store that Phase 14 does. Nothing in March caches
  `lineType` for `LT_MONSTER_ROAD`: load time only builds the wall list
  and the bounds, and line groups never read it. `CanDown` reads it live,
  so the retype reaches the player.
- **NPC_AI_ON_OFF.** June's case 8 is `for every NPC: SetAIEnable(v)`, with
  no host, team or type filter, so Phase 14's "every NPC" matches June.
  - Trigger 0/1's AI-*on* rows (`ETC2`) are restored. They can only wake
    NPCs.
  - Triggers 2/3 (AI *off*) **stay empty**. The AI-party bots are NPCs,
    and restoring those rows would freeze them from line 0 until the
    collapse.
  - Restoring the AI-off rows needs a bot exemption in the C++. It would
    be a deviation from June, and the predicate is already there:
    `IsPvpBot() && IsOfflinePartyBotUID(...)`, as used by
    `X2Game.cpp:5152`. Not done; it is the user's call.
- **Rows restored**, all verbatim from June. Checked by comparing
  comment-stripped March against June: the only remaining differences
  are the two exceptions listed below.
  - `Ranox_FireTemple_Stage1_LineMap`: triggers 0 and 1, `LINE1-3`
    (`CHANGE_TYPE` → `LT_NORMAL` on 11/12/13 and on 33/34/35), plus
    `ETC2` (AI on).
  - `Ranox_GateWay_Stage1_LineMap`: triggers 0 and 1, `MESH1`
    (`TAT_MESH_MOVE` of the two rocks).
  - `Ranox_Geyser_stage0_linemap`: trigger 0 `ETC0` (talk box
    `STR_ID_33824`), trigger 1 `MESH0` (`MESH_MOVE`), trigger 2 `ETC0`
    (talk box `STR_ID_33825`). Both string IDs read the same in March's
    `general.ess` and June's `General.ess`.
  - `Ranox_Secret_Stage2_LineMap`: trigger 2 `LINE1` (`CHANGE_TYPE` →
    `LT_POTAL`).
- **Still omitted**, each with a comment in the file:
  - FireTemple triggers 2/3 (see above).
  - Geyser trigger 1's two `TAT_ETC_PLAY_EFFECTSET` rows. June plays
    `EffectSet_STEELPUMP_Move_FX` at the named trigger mesh (case 7 splits
    `MOTIONNAME_EFFECTSETNAME` on a space and anchors on the first user
    unit). That effect set is **not in March's `EffectSet.lua`**, and one
    argument (a user-unit vfunc `+0x150`) is unresolved.
- **June-only, recorded and not ported:**
  - the `PVE_PotalGate1-5` particles June spawns when a trigger retypes a
    line to `LT_POTAL` (`FUN_322e5820`);
  - `AUTO_DISABLE_LINE`, which needs a new LineData field (`+0x128`) and
    two unit-side calls. `Ranox_FireTemple_StageBoss_LineMap` already
    carries two such rows, and March ignores them;
  - `TAT_LINE_CHANGE_MOVABLE_DATA` (`STOP`, `DIRECT_MOVE_DATA`).
- **Staged.** Each was compiled with `-v mar-2014`, after a drift check
  showed that every pre-edit source reproduces the staged bytecode
  exactly. A decompiled round trip confirmed the names and rows.
  - `Enum.lua` 265,287 → 265,394 B, to `march2014/data036/` and
    `kom_contents/data036_kom/`.
  - The four linemaps to `kom_contents/data040_kom/`. FireTemple goes
    11,487 → 12,149 B, GateWay 14,928 → 15,189 B, Geyser 12,993 →
    13,393 B and Secret 9,546 → 9,651 B. All five were already in their
    manifests at `"2"`.

---

## Phase 11 - Ranox boss face missing from the boss HP bar [item 12]

Reported 2026-10-01, after Phase 1 shipped: Ranox bosses' small face icon in
their own HP bar (`CX2BossGageUI::SetBossGageTexture`, set from each NPC's
`BOSS_GAGE_FACE_TEX`/`BOSS_GAGE_FACE_TEX_PIECE` lua fields, read in
`X2GUNPC.cpp:4260-4266` and applied via `pStatic_BossFace->GetPicture(0)
->SetTex_LUA`) does not show. This is a **different pair of atlas files**
from anything Phase 0 already censused — Phase 0 only walked Ranox *UI* luas
and `DungeonData`/`BattleFieldData`; it never looked at per-monster unit
lua's `BOSS_GAGE_FACE_TEX*` fields. This phase does that census, scoped to
just these two files, and found the same class of gap as Phase 1, twice, in
two different shapes.

**Root cause, investigated and fixed 2026-10-01:** 22 Ranox boss/sub-boss
unit files under `march2014/update/data037/` set `BOSS_GAGE_FACE_TEX`/
`_PIECE`, split across two atlases. (`DLG_BossState.lua`/`DLG_BossState_NEW.lua`,
the dialog scripts for this UI, live in `data034.kom` — unrelated to either
atlas file below.)

| Texture | Live archive | Live keys | June keys | Ranox pieces needed |
|---|---|---|---|---|
| `DLG_BossState.tga` | `data068.kom` | 109 | 152 | `NUI_MOTHERFIG`, `NUI_DRYADKENTA`, `NUI_ALUMINUM`, `NUI_STAIN` |
| `DLG_BossState_02.tga` | `data215.kom` | **0 (no `.TET` at all)** | 74 | `DEVIL_BERSERKER`, `NUI_FIRE_DEVIL_CHAIN_IPLITAN`, `DEVIL_IGNIA`, `NUI_FIRE_DEVIL_IN`, `NUI_FIRE_DEVIL_JIN`, `NUI_FIRE_DEVIL_SKA`, `ALUMINUM`, `MULSAYRENA`, `ALUMINUM_FD`, `NUI_RANOX_MAGI` |

`data068.kom` separately has a **different**, unrelated file
`DLG_BossState02.tga`/`.tet` (no underscore) — not to be confused with
`DLG_BossState_02.tga` in `data215.kom`, which is what the lua files actually
reference.

- **`DLG_BossState_02.tga` (10 Ranox pieces) is the same shape as Phase 1's
  `Image10`: image present, sidecar `.TET` completely absent live.** `cmp`
  confirmed live's `.tga` (1,048,620 B) is byte-identical to June's, and all
  10 needed pieces exist in June's 74-key `.TET`. Zero collision risk —
  **adopted June's `.TET` outright**, no image work.
- **`DLG_BossState.tga` (4 Ranox pieces) was first mis-classified as needing
  a scoped merge, then corrected within this phase.** All 109 keys live
  shares with June have byte-identical UV rects (checked every one, zero
  mismatches, nothing missing or moved) — June did not rearrange this atlas
  and every key March reads still resolves to real art in June's copy. A
  per-key pixel sample did find 5 keys with different pixels despite
  matching UVs (`ERDAISLAND_TOWER`, `ERDAISLAND_TOWER_HAMEL`,
  `ERDAISLAND_TOWER_VELDER`, `NUI_GHOST_CLOWN_THUNDER`, `NUI_PPORU_WILLIAM` —
  June redrew these for its own reasons), and the first pass over this phase
  read that as a Phase-2-`Title_01`-style collision requiring a scoped merge.
  **That was wrong — see the corrected asset-conflict rule in "Fix
  philosophy": a redrawn-but-present key is not a collision, it's what
  "June wins" means.** No key goes missing or stops resolving. **Adopted
  June's `.tga` + `.tet` outright**, same as `_02`. The 5 keys' art changes
  to June's version as a side effect — flagged here since it touches
  content outside Ranox (Erda Island's tower icons, `NUI_GHOST_CLOWN_THUNDER`,
  `NUI_PPORU_WILLIAM`), but nothing stops rendering or resolves wrong. June's
  other 39 new keys over live's 109 (`GLITER_*`, `NUI_KARUSO_*`,
  `NUI_TROCK_*`, `HALLOWEEN_TEDDY`, etc.) come along for free as part of the
  wholesale adopt — harmless, same as `ERDA_ISLAND`/`SANDER_SECRET` riding
  along in Phase 1's `Image10` adopt.

Done:
1. `DLG_BossState_02.tga`: June's `.TET` staged as a new file in
   `march2014/update/data215/DLG_BossState_02.TET`, with a manifest entry
   added to **both** `march2014/update/data215/_manifest.json` and
   `kom_contents/data215_kom/_manifest.json` (confirmed byte-identical to
   each other after the edit). No `.tga` staging — live's was already
   correct and byte-identical to June's.
2. `DLG_BossState.tga`: June's `.tga` + `.tet` copied in whole, byte-for-byte
   (`cmp`-verified), to `march2014/data068/` (patch, no manifest change —
   both filenames were already listed) and to `kom_contents/data068_kom/`.
3. Verified against the **live** koms with `elstools inspect` (not just the
   `kom_contents` manifests): live `data068.kom` currently holds the old
   8,486 B / 109-key `.tet` (180 files total) and live `data215.kom` holds 5
   files with no `.TET` at all — confirming the starting state matches this
   investigation before handing off the pack.
4. Cropped and rendered the 4 new Ranox face rects from the staged
   `DLG_BossState.tga` to confirm real art, not blank space.

Owed: packing `data068.kom` and `data215.kom` from `kom_contents/<name>_kom/`
is the user's step. After packing, re-run `elstools inspect` on both live
koms (`data068.kom` should stay at 180 files with the `.tet` now 12,130 B;
`data215.kom` should go from 5 to 6 files) and parse the live `.TET`s.

Verification:
- Fight the 22 Ranox boss/sub-boss units this phase lists (grep
  `march2014/update/data037/*.src.lua` for `BOSS_GAGE_FACE_TEX` for the full
  list): the boss face icon renders in the HP bar instead of a blank/default
  square.
- Erda Island (and anything else using `ERDAISLAND_TOWER*`,
  `NUI_GHOST_CLOWN_THUNDER`, `NUI_PPORU_WILLIAM`) still shows a boss face —
  it will be June's redrawn art, not March's original, which is expected and
  intentional per the asset-conflict rule.
- `ErrorLog.txt` has no texture-load error for either `DLG_BossState.tga` or
  `DLG_BossState_02.tga`.

**Run with:** Sonnet 5, medium effort. Data staging; both files turned out to
be plain wholesale adopts once the real hazard (a key March reads going
missing or unresolvable, not a pixel redraw) was checked for and found
absent. No review beyond the manifest diffs and the live-kom `elstools
inspect` confirmation above.

**Kickoff prompt (historical — superseded by "Done" above, kept for
record):**
```
Continuing the Lanox/Ranox port. Read LANOX_REMAINING_PHASES.md in the repo
root in full -- especially "Starting context", the "Fix philosophy" section's
2026-10-01 asset-conflict-priority rule, the "Investigation ledger", and
"Phase 11". Branch: mods/lanox-port. Never launch X2_offline.exe without
asking first.

Your task: fix the missing Ranox boss face icon in the boss HP bar. 22 Ranox
unit luas under march2014/update/data037/ set BOSS_GAGE_FACE_TEX/_PIECE,
split across DLG_BossState.tga (data068.kom) and DLG_BossState_02.tga
(data215.kom). DLG_BossState_02.tga is missing its .TET entirely live (image
already byte-identical to June) -- adopt June's .TET, no image work.
DLG_BossState.tga's 109 shared keys have identical UVs to June and nothing
March reads is missing or moved -- 5 of them have different PIXELS (June
redrew Erda Island's tower icons and 2 others), but per the asset-conflict
rule that is not a collision worth blocking on. Adopt June's whole file.
Stage both, update manifests where needed, and hand me the pack list. After I
pack, verify against the LIVE data068.kom and data215.kom.
```

---

## Verification - batching

Phases 1, 2 and 4 are data-only and can share one pack/play-test pass, and so
can Phase 9 and, if step 1 picks option (a), Phase 10. Phases
5 and 7 each need a build. For every phase, "done" means: live-kom extraction
confirms the bytes (data) or `#pragma message` / a log line proves the code
compiled in (C++), and the user's play-test confirms the behaviour.
