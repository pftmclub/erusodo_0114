# Lanox Remaining Phases

Eight Lanox-specific items still open after `LANOX_DEFECT_PHASES.md`, found by
play-testing at ~80% port completion (user report, 2026-09-30). This document
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

| Phase | Item | Depends on | Kind |
|---|---|---|---|
| 0 | TET-key census (all Ranox UI) | - | read-only |
| 1 | Dungeon image on result screen / party list | 0 | data |
| 2 | Local-map dungeon names (+ progress bars via June-UI P3) | 0 | data |
| 3 | Lanox tab in the dungeon list | June-UI P2 | cleanup + verify |
| 4 | World map: June tiles, Ranox labels | 0 | data (+ maybe lua) |
| 5 | M-key field map follows the character | - | C++ (Ghidra) |
| 6 | Hide Secret Hell | - | data (+ maybe C++) |
| 7 | Distant Ruin demon colours | - | C++ (Ghidra) |
| 8 | Burned Forest background | - | investigate first |

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
    sit alongside)
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

## Verification - batching

Phases 1, 2 and 4 are data-only and can share one pack/play-test pass. Phases
5 and 7 each need a build. For every phase, "done" means: live-kom extraction
confirms the bytes (data) or `#pragma message` / a log line proves the code
compiled in (C++), and the user's play-test confirms the behaviour.
