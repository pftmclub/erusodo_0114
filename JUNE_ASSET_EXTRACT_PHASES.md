# June asset extraction: June's non-lua assets, byte-exact, in March koms

Plan key `june-asset-extract` · branch `mods/june-buff-port` · flag `SERV_IRUHADEV_KOM_LONG_NAMES` · log tag `[KOM_LONG]` ·
kickoffs `kickoffs/june-asset-extract/` · backlog Stage 14 · created 2026-10-07.

## Context

The user asked (2026-10-07) whether June 2017's non-lua/kim assets, already extracted from June's koms, can be packed into March
(V.0.3) koms and load in the March client **without changing the asset files**. An investigation the same day answered it:

- **The formats are compatible; no asset type needs per-file conversion** (see Established facts).
- **What breaks is the extraction.** elstools cuts 18,571 June files short, and about 4,900 of them are damaged where it matters.
  The extracted `data101_kom` folder is also corrupt.
- **Three rules break a naive pack:**
  - the manifest algorithm values;
  - cross-kom duplicate names;
  - names over 59 characters, which March can never find.

The user chose the full path: a byte-exact June extractor, a packing helper that enforces the rules, and a flagged engine fix for
long names. The phases follow from that.

This plan does the reverse-engineering items 1-3 of the deferred June loader plan
(`C:\Users\Iruha\.claude\plans\i-want-you-to-humble-dolphin.md`, memory `june-loader-port-deferred`). That loader port, which reads
June koms natively, **stays deferred**. This plan only extracts files and repacks them as V.0.3.

## Rules

- **Never overwrite June's extracted tree in place without a backup.** X2 writes exact copies only where they differ, and backs up
  the file it replaces first.
- **The packing rules are enforced by the tool, not by memory:**
  - non-lua is `"0"` and lua/txt is `"2"` in `_manifest.json`;
  - a June file whose name March already has goes into **March's** kom number;
  - no name may appear in two koms;
  - no name may be longer than 59 characters until X4's flag is on.
- **Kom packing is Claude's job; deploying it is not unasked.** Run `/elstools pack dataNNN` yourself. **Ask before every
  `/x2-kom-packed copy`.**
- **Read-only RE.** Ghidra through the `ghidra` skill. Rows go to `june2017_re/funcmap.tsv`, tagged `XKOM X0`.
- **One flag** (`SERV_IRUHADEV_KOM_LONG_NAMES`, X4). Original code stays in `#else`. `KGCMassFileManager.cpp` carries Korean
  comments; check its encoding first, and if it is CP949, edit it with `tools/bytepatch.py` only.
- **June's `14JUN17\data\*.kom` archives and the March game dir are read-only inputs.** No phase writes to them.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, unifdef or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_ASSET_EXTRACT_PHASES.md "<heading>"` |
| June kom reader (from X1 on) | `python tools/junekom.py list / extract / verify ...` |
| Pack / deploy a kom | `/elstools pack dataNNN`, then ask, then `/x2-kom-packed copy dataNNN` (`tools/kom.py`) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_KOM_LONG_NAMES --worktree --files <files>` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag KOM_LONG` (x2-check-logs); KEM_ERROR lines are in `ErrorLog.txt` |
| June client | Ghidra, `ghidra` skill, `unpacked_x2.exe` (read-only) |

Paths:
- June data: `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\` (258 `dataNNN.kom` / `dataiNNN.kom` + `*_kom\` extracts).
- March staging: `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\dataNNN_kom\`; game dir `...\24965799\data`.
- Cached manifests (name/size/algo): `june2017_re/assets_cache/inspect/*.json` via `june2017_re/fxdata.py inspect()`.

## Established facts (verified 2026-10-07, investigation session)

**Formats are compatible.** A header census covered all 217,019 June non-lua/kim files. It was compared against March's own content
(10,895 files, excluding the June ports), plus a template census of 78k meshes and a map of every March loader:

| Type | Evidence | Verdict |
|---|---|---|
| `.dds/.tga/.bmp` | DXT1/3/5 and plain RGB only, 0 DX10/BC7. March: `D3DXCreateTextureFromFileInMemoryEx`, Mip=1 (`KTDXDeviceTexture.cpp:141`), same `d3dx9_43.dll` as June. | drop-in |
| `.x/.y` | `xof 0303` txt/bin/bzip. `KSM`/`KME` v0x200 (`KTDXDeviceXSkinMesh.h:24`, `KTDXDeviceXMesh.h:13`). KAC v0x100. All under 255 frames. KME FVF 0x152 = `D3DFVF_MESHVERTEX`. June uses the same 16 stock X templates as March. | drop-in |
| `.tet/.xet` | `TET `/`XET ` v0x0100 on both sides (`KTDXDeviceTET_Preprocessing.h:5`, `KTDXDeviceXET_Preprocessing.h:5`). | drop-in |
| `.ogg/.wav` | All Vorbis or PCM. FMOD Ex 4.44 (`KTDXDeviceSound.cpp:61`). | drop-in |
| `XRenderer.fx` | June's is a superset. All 38 names `KTDGXRenderer.cpp:16-105` looks up exist. Rim light defaults off; colour grading and the edge macro are commented out in the file. | drop-in (confirm no KEM_ERROR50) |
| fonts | FreeType from kom (`NO_LOCAL_FONT`), face 0 of a `.ttc` only. | drop-in |
| `.swf/.gfx` | No Scaleform loader in March. | out of scope |
| BGM, `.avi`, `.ess` | March loads them from disk only. | copy loose, not a kom |

**Extraction damage.** Extracted sizes were checked against June's kom manifests:
- **All 18,571 short files are V.0.4 algo-2 (Blowfish) entries.** Every V.0.3 entry (algo 0/2/3) and every V.0.4 algo-0/3 entry
  extracted at exact size.
- **Harmless:**
  - 4,432 meshes, where only bytes after the MSZIP stream are lost;
  - 8,008 TGAs, which lose the footer only (pixels intact);
  - 1,161 DDS, whose top mip is intact (March uses Mip=1).
- **Broken:**
  - 3,835 meshes, with the last MSZIP block cut;
  - 262 KSM, which lose the KAC tail;
  - 784 DDS, with the top mip cut;
  - 60 TGAs (21 with pixels cut, 39 RLE).
- 51 `march2014/update/` files are short copies, and 49 of them are live (list: `JUNE2017_ASSET_CLOSURE.md` "Extractor defect").
- **`elstools_fixed.rar` holds the same `elstools.exe`** (same size and mtime). It is not a fix.
- **`data101_kom` extract is corrupt.** 117 files are wrong: `velder_bridge_burning_Stage1_bridge_stain01_ap.Y` is 17 B where the
  manifest says 1,465. Read from `data101.kom` directly (V.0.3, algo 0, plain zlib; payloads follow the manifest in order), it gives
  the valid `xof 0303bzip` file.

**June container facts**, from the loader plan and to be cited from June's code in X0:
- The header is `0x34` count, `0x3C` FileTime sum, `0x40` adler32, `0x44` manifest size, manifest at `0x48`. Payloads are back to back.
- V.0.4 manifest: XOR with a 12-byte key `table[xmlsize % 200]`. The table is at VA `0x33447114`.
- V.0.4 entry: `MappedID` (8 chars, rotated by `Size & 7`) maps to a seed. If seed > 0, the entry is Blowfish with key
  SHA-1(`"%d" % seed`) (`FUN_31c47a00`, init `31d26c20`, crypt `31d26270`). Otherwise it uses key-C XOR-CRC.
- The elstools truncation correlates with `CompressedSize % 8` in 5..7. Expect a last-partial-block rule.
- V.0.3 algo 2: key C `fd e1 08 ec 0d 09 02 dc de 4c 8c 64 16 62 7d e4 97 16 ff dc`, March's `XORCRCDecrypt` routine (tested).
- Algo 3: `Elsword-file-editor/elswordkom/crypto.cpp` `decrypt_algorithm` (tested). It inflates from offset 4 to `1b 4b 4c 82` bytecode.

**March packing rules:**
- **Algorithm values.** March XOR-decrypts only on `LoadAndDoMemory` (lua and particle/mesh-player txt). Every other type is plain
  zlib, so its manifest value must be `"0"`. An algo-2 texture fails `uncompress` with `KEM_ERROR390`. `kom.py pack` gives an
  unlisted file algo 2.
- **Duplicates.** 10,355 June names exist in March under a different kom number. A second copy is a duplicate:
  - under `_IN_HOUSE_` it pops a MessageBox per duplicate (`KGCMassFileManager.cpp:519-546`);
  - `NOT_ALLOW_DUPLICATE_FILES` (`Always.h:3245`) drops the archive.

  March's live koms have no duplicates today. (An earlier count of 25k came from the same kom cached twice.)
- **Long names.** 5 June names are over 59 characters. The kom key is cut to 59 (`MASSFILE_NAMESIZE` 60, the XML read at
  `KGCMassFileManager.cpp:1880-1895`), but the live lookup (`#else` of `MASS_FILE_FIRST` at `:915`, `WCHAR_DIR` on) uses the full
  name, so the file is never found. The three `effectset_moonlight_rhapsody_debuff_accumulation_display_0N.tga` also collide once cut.
  The other two are `trigger_hamel_frozen_water_hall_stage3_stone_statue_water_a.x` and
  `elysion_soless_attackc_pickupsword_readytoattacka_dash_combo.ogg`.

## Phase X0 - V.0.4 container RE (Ghidra)

**Scope.** Identify only: no tool, no game code.

**Run with:** Opus 5.5 high.

**Read list:** Context, Starting context, Established facts.

1. **Manifest key table.** Dump the 200×12 bytes at VA `0x33447114` into `june2017_re/v04_manifest_keys.tsv`. Prove it by
   decrypting all 77 V.0.4 manifests: the XML parses, the FileTime sum equals header `0x3C`, and the entry count matches.
2. **MappedID to seed.**
   - Decode `FUN_31c4d350`'s rotate, the lookup (`FUN_31aa9150`, `FUN_31c55490`, `FUN_31c57000`) and the seed table (about 998
     entries).
   - Dump the table to `june2017_re/v04_seed_table.tsv`. Every MappedID in the 77 manifests must resolve.
3. **Blowfish.**
   - Find out whether the P-array/S-box at `3344e170`/`3344e1b8` are standard; diff them against Blowfish's pi constants.
   - Find out the byte order and mode (ECB/CBC, IV) of `31d26270`.
   - **Find the last-partial-block rule** (the elstools bug), and whether `Checksum`/`FileTime` are checked per entry.
4. **Algo 3.** Cite June's own code for the +4 offset and trailer rule (`crypto.cpp` is the tested reference).
5. **Done gate:**
   - "Phase X0 - Done" is written here, under ~60 lines, with every item answered by a June address. Rows go to `funcmap.tsv`
     (`XKOM X0`).
   - One hand-decoded V.0.4 algo-2 entry that elstools truncates, `CHUNG_GIGANTIC_IMPACT_Bottom_Broken_SP.dds`, inflates to 22000 B
     and its adler32 matches.
   - The X1 kickoff is amended with anything X0 changed.

**Stop and ask** if June validates something per entry that a repack cannot reproduce, or if the seed table is built at runtime
from data we do not have.

## Phase X1 - tools/junekom.py

**Scope.** A read-only June kom reader, stdlib only, with a pure-Python Blowfish using X0's tables.

**Run with:** Opus 5.5 high.

**Read list:** Starting context, Established facts, X0's Done note.

1. **Subcommands:**
   - `list <kom>`: name, size, compressed size, algo, MappedID, seed;
   - `extract <kom> [--names ...] --out <dir>`;
   - `verify [<kom>...]`: decode every entry; check size == manifest and the zlib adler32; report per kom.
2. **Coverage:** V.0.3 algo 0/2/3 and V.0.4 algo 0/2/3. Lua stays as bytecode (algo 3 output); decompiling is not this tool's job.
3. Add a line to `tools/README.md`.
4. **Done gate:**
   - `verify` over all 258 June koms reports 0 failures;
   - the 51 known-short `march2014/update/` files come out at June's manifest size;
   - `data101.kom`'s 117 files come out valid.

## Phase X2 - Re-extract and re-baseline

**Scope.** Data only.

**Run with:** Sonnet 5.5 high.

1. **Write exact copies.** For every June extracted file whose size or hash differs from `junekom.py extract`, back up the old file
   (`14JUN17\data\_extract_backup\dataNNN_kom\`) and write the exact one. Write a list of what changed.
2. **March copies of short files.** List every `march2014/update/` and `kom_contents` file that is a short June copy, with its kom.
   That is X5's repack list.
3. **Asset closure.** Rerun `june2017_re/assets.py` (or update `JUNE2017_ASSET_CLOSURE.md` "Extractor defect") so the
   truncated-in-March and same-prefix buckets are resolved.
4. **Done gate:** a re-run of the size census shows 0 June files short of the manifest, and the repack list is in the Done note or a
   `.tsv`.

## Phase X3 - Packing helper

**Scope.** Tooling.

**Run with:** Sonnet 5.5 high.

1. Add `tools/kom.py stage-june <name|list-file>`. For each June file:
   - pick the kom: March's number if March's live inspect has the name, else June's number;
   - copy the exact June file into `kom_contents/dataNNN_kom/`;
   - set its `_manifest.json` value: lua/txt `"2"`, everything else `"0"`. Keep the house JSON format: CRLF, 2-space indent, no
     trailing newline.
2. **Refuse:**
   - a name that already sits in a different staged kom;
   - a name over 59 characters, unless `--long-names` is given (for after X4);
   - a file X2 did not verify.

   Print what it staged, per kom.
3. **Done gate:** a dry run over the X2 repack list shows the kom chosen per file and 0 refusals except the 5 long names.

## Phase X4 - SERV_IRUHADEV_KOM_LONG_NAMES

**Scope.** KTDXLIB engine change, independent of X0-X3.

**Run with:** Opus 5.5 high.

1. **Design check** (in the Done note):
   - every user of `MASSFILE_NAMESIZE` / `strFileName` (the XML read at `:1880-1895`, the insert at `:504`, `GetFileName`, the packer
     paths `:2981`/`:3233`, `MASSFILE_MEMBERFILEINFO` copies);
   - whether any on-disk format (kom V.0.1/V.0.2 binary member headers) uses the 60-char size. If one does, keep it and key long names
     through a side path instead of growing the struct.
2. **Port.** With the flag on, a member name up to the XML's full length is stored and looked up by its full uppercased name. Names of
   59 characters or fewer behave exactly as before. Log `[KOM_LONG]` once per long name registered.
3. **Done gate:**
   - flagcheck PASS;
   - build + deploy;
   - the review kickoff is written.

## Phase X4 review

**Run with:** Sonnet 5.5 high.

Check flag hygiene, and that flag-off is byte-identical in behaviour. Check that no buffer is overrun anywhere the 60-char struct is
copied, and that names of 59 characters or fewer take the old path.

## Phase X5 - Trial pack and play-test

**Run with:** Sonnet 5.5 high, then the user's play-test.

1. **Stage** with `stage-june` (X3):
   - the 49 live short June copies, using X2's list;
   - the 5 long-name files, when X4 is deployed.
2. **Pack and ask.**
   - `/elstools pack` each touched kom;
   - diff old against new with `elstools inspect`: only the staged names change, and they show algo 0;
   - **ask before each copy**.
3. **Hand-off:** copy-ready, per check: where / paste / what it does / look for / if not.
   - one previously broken mesh, e.g. a Ranox map (`Spa_Ranox_G01_water01.y`, Ranox dungeon);
   - a boss name plate (`HQ_BOSS_NAME_ALUMINUM.dds`);
   - one long-name asset.
4. **Logs:**
   - `ErrorLog.txt` has no KEM_ERROR88/89/343/390/95/96/87/136/132 for the staged names;
   - `logs.py --tag KOM_LONG` lists the long names.

## Out of scope (routed, not dropped)

- **Reading June koms natively:** the deferred June loader port (memory `june-loader-port-deferred`).
- **Scaleform `.swf/.gfx`:** backlog Stage 9 (UI).
- **June-only shader parameters** (rim light, env box): backlog Stage 2 (`.fx` contract).
- **June particle/mesh-player `.txt` keys:** june-effect completion phases.
- **The 49 HQ_Shop_*460 DDS with a stray volume cap, and 4 odd-header TGAs:** checked when a phase first stages one. They are not
  known to fail.
