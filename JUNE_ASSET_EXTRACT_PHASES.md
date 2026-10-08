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

- **June files are byte-exact, inside data included (user, 2026-10-08).** No hand edits to a June asset, not even a one-byte
  repair such as the `Wait01` -> `Wait` length patch found in X1. If March cannot load or use June's exact file, do not touch the
  file. Plan a phase that traces June's loader for that format in Ghidra (`ghidra` skill, `unpacked_x2.exe`) and ports the support
  into March behind a `SERV_IRUHADEV_` flag.
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

### Phase X0 - Done

2026-10-08, Opus 5.5. This was read-only RE: no code, no flag, no build. Ghidra functions are renamed `KOM_*`, `Blowfish_*` and
`SHA1_*`. Evidence is in 18 `XKOM X0` rows in `june2017_re/funcmap.tsv` and in two tables:
- `june2017_re/v04_manifest_keys.tsv` (200 keys);
- `june2017_re/v04_seed_table.tsv` (300 MappedIDs).

**Neither stop condition fired.** June checks nothing per entry, and the seed table is static.

1. **Manifest.**
   - `KOM_OpenMassFile` 31c4ddb0 checks the manifest adler32 against header `0x40` @31c4e67c. It hashes the stored bytes, before
     decryption.
   - For V.0.4 only, it calls `KOM_XorManifest12` 31c4ade0 @31c4e6df, keyed `0x33447114 + (size%200)*12`. This is a bytewise XOR
     with a key that repeats every 12 bytes.
   - `KOM_ParseManifest` 31c4d350 checks the FileTime sum (hex text) == `0x3C` @31c4dd27 and the count == `0x34` @31c4dd46.
   - Back in 31c4ddb0, the last offset must equal the file size @31c4e777.
   - **Proved on all 77 V.0.4 manifests:** the adler32, count and FileTime sum match, and the XML parses.
2. **MappedID to seed.**
   - The parser rotates the 8-char MappedID **right** by `Size & 7` (@31c4db44), so `key[i] = id[(i-r) mod 8]`. It looks the result
     up in hash_map `0x3376d550` (`MappedIdMap_find` 31c55490 @31c4dc6f) and stores the index (1..300) at entry `+0x30`. A miss or a
     V.0.3 entry stores 0.
   - The map is **static**: 300 inserts in `KOM_MgrCtor_BuildMappedIdMap` 31c516d0. The plan expected about 998 seeds, a figure
     from elstools' notes; the binary has 300.
   - `seed = sum mod 2^32 of the 5 dwords at 0x33445428 + idx*20` (31c47a00 @31c47b3d), printed with `"%u"`. The key is SHA-1 of
     that decimal string, 20 bytes.
   - **All 105,354 V.0.4 entries resolve,** algo 0 and algo 3 included.
3. **Blowfish** (`Blowfish_Init` 31d26c20, `Blowfish_Crypt` 31d26270).
   - P (`3344e170`) and S (`3344e1b8`) are **standard**: 0 diffs from pi over 1,042 words.
   - Words are big-endian. Mode 0 = ECB decrypt (1 = CBC decrypt, 2 = CBC encrypt). The IV is zero and unused.
   - **Tail rule:** 31d26270 throws "Incorrect buffer length" unless `len % 8 == 0`, so June passes `len - (len & 7)` (@31c47bee..bf5).
     **The last 1-7 bytes are stored plaintext**: they are the zlib adler32 tail. elstools loses them. That is the truncation.
   - **No per-entry check.** `Checksum` = `FileTime` = adler32 of the stored payload (3,469/3,469 entries in 5 koms). June stores it
     at entry `+0x28`, and `KOM_ReadMemberRaw` 31c4b940 never reads it (it reads +0x1c/20/24/2c/30 only). A V.0.3 repack is
     unaffected.
4. **Decode dispatch** is by algorithm (`KOM_ReadMemberRaw` 31c4b940 @31c4ba19..26):
   - **algo 0:** inflated in 31c4b940.
   - **algo 2:** raw, then `KOM_DecodeMemberAlgo2` 31c47a00.
     - seed index > 0 (every V.0.4 entry): Blowfish as in item 3.
     - index 0 (V.0.3): key C (`fde108ec…`) CRC cipher with **no trailer**. Its seed loop is the plain step
       `crc=(crc>>8)^T[b^k^crc_lo]` (@31c47d58..80), March's `CRC_32.h` `CalculateWithoutEncrypt`, followed by
       `CalculateAndDecrypt`. This agrees with Established facts.
   - **algo 3:** buffer `CompressedSize+4` (@31c4ba26), then `KOM_LoadAlgo3KeyC` 31c310e0, in every version.
     - Its seed loop is the **NOT form** (@31c31476..8c), as in crypto.cpp.
     - It regenerates 4 bytes at `buf+CompressedSize` from the reverse table `CRC32_GetTable()+0x400` (@31c3156e..31c31691).
     - It sets `src += 4` when `CompressedSize >= 4` (@31c3169f) and calls `uncompress(.., CompressedSize)` @31c31707.
     - The same inlined sequence also appears in 31d77310 (x3), 31f57330, 31cd5910, 325c2100 and 31e46300. These were not examined.
       The patcher variant 31c4efd0 was not examined either.
5. **Gate.** `CHUNG_GIGANTIC_IMPACT_Bottom_Broken_SP.dds` (`data002.kom`, CompressedSize 12735, %8 = 7) decodes by hand:
   - it inflates to **22000 B**;
   - its zlib adler32 `8e4a4e09` matches;
   - elstools' 21,996 B is a prefix of it.
6. **Sample proof** (scratchpad oracle): 1,504 entries across all 77 V.0.4 koms and about a quarter of the V.0.3 koms, every
   algorithm and every `CompressedSize % 8` class. **0 failures:**

   | Version | algo 0 | algo 2 | algo 3 |
   |---|---|---|---|
   | V.0.4 | 92 | 877 | 27 |
   | V.0.3 | 85 | 421 | 2 |

   The full census is X1's `verify`.

**For X1:** `kickoffs/june-asset-extract/X1.md` carries the pitfalls:
- the two seed loops;
- the plaintext tail;
- the name codepage;
- runtime: pure-Python Blowfish takes about 2 s per entry including key setup, so cache the 300 schedules and use multiprocessing.

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

### Phase X1 - Done

2026-10-08, Opus 5.5. This phase is tooling only: no flag, no build, no deploy.

`tools/junekom.py` provides `list`, `extract`, `verify` and `selftest`, plus a line in `tools/README.md`. It ports X0's oracle step for
step and dispatches by algorithm, as X0's Done note item 4 describes.
- **Blowfish.** P/S are regenerated from pi, not read from the exe. It decrypts ECB over `len & ~7` and leaves the tail as stored.
  Each process caches one unrolled decryptor per seed index, at about 1 MB/s.
- **Strict decode.** Inflate must reach the end of the stream with no trailing bytes, zlib's own adler32 must pass, and the size must
  equal the manifest.
- **Extract layout** is `<out>/<stem>_kom/<Name>`, with `\\?\` long paths.
- **Writing June's tree.** It refuses to write anywhere under `14JUN17\` without `--overwrite --backup <dir>`. Identical files are
  skipped, and each replaced file is backed up first.

**Gates, all met:**
1. **Selftest.**
   - Two standard Blowfish vectors pass.
   - CHUNG (`data002`, csize 12735, %8 = 7) decodes to 22000 B, adler32 `8e4a4e09`.
2. **`verify --compare <June data>`:** 258 koms, **222,811 entries ok, 0 failed, 0 header problems** (V.0.3 too), 7.3 GB, 921 s on 15
   jobs. The two `*_packed.kom` files in June's dir are not June archives and are skipped by name.

   | version | algo 0 | algo 2 | algo 3 |
   |---|---|---|---|
   | V.0.3 | 44,252 | 72,973 key C | 232 |
   | V.0.4 | 16,262 | 81,901 Blowfish | 7,191 |

   - **Per-entry Checksum** equals the adler32 of the stored payload on **all 222,811** entries. June never reads it.
   - **Compare against the elstools extracts:**
     - 202,980 same;
     - **18,571 short-prefix**, every one a Blowfish entry (the Established-facts count, exactly);
     - **117 different**, all in `data101`;
     - 1,143 missing: 1,087 in `data101_kom` and 56 in `datai026_kom` / `datai027_kom`.
   - Per kom: `june2017_re/x1_verify_per_kom.tsv`. Per-entry compare rows (X2's work list, about 20k rows, not committed):
     `python tools/junekom.py verify --compare "<June data>" --report <file>`, which writes `<file>_compare.tsv`.
3. **`data101`.**
   - All 2,183 entries decode.
   - The 117 that differ from the corrupt extract all start `xof 0303bzip`.
   - `velder_bridge_burning_Stage1_bridge_stain01_ap.Y` = 1,465 B.
4. **The 51 `march2014/update/` files:** all 51 come out at June's manifest size, each from exactly one June kom
   (`june2017_re/x1_short51.tsv`). 48 update copies are strict byte prefixes of June's file. Three are not:
   - **`Motion_NPC_Ranox_FACEOFF.x`, `Motion_NPC_Ranox_SYOTA.x` (data013):** the cut copy was **hand-patched**. The final string length
     was changed 6 -> 4, so `Wait01` reads as `Wait`. June's file has `Wait01` intact.
   - **`DLG_Common_Button03_NEW.TET` (data063):** not a short copy. June's entry is algo 0 (never truncated), and June's own extract
     equals `junekom`'s output. The 2,464 B update file is a different version (0x1a entries vs June's 0x1c). The closure table filed
     it as truncated by mistake. X2 corrects that row.

**User rule (2026-10-08):** June files go in byte-exact, including their inside data (see Rules). The two patched KSMs are restaged
from June in X5. If March then fails to use one, that is an engine phase, not an asset edit.

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

### Phase X2 - Done

2026-10-08, Sonnet 5.5. Data only: no flag, no build, no deploy. Nothing was written to a `.kom` or the game dir.

1. **Work list** (`junekom.py verify --compare`, 1,194 s): 258 koms, 222,811 entries ok, 0 failed; vs June's extracts
   18,571 short-prefix + 117 different + 1,143 missing, exactly X1's numbers.
2. **Exact copies** (`extract --all --overwrite --backup`, 856 s): `EXTRACT OK: new 1143, replaced 18688 (=18571+117), identical 202980,
   refused 0, failed 0`. Originals of the 18,688 replaced files are in `14JUN17\data\_extract_backup\dataNNN_kom\`.
3. **March copies** (`x2_repack_list.tsv`, 281 rows, 16 koms; columns name / March kom / June kom / class / March size / June size / note).
   Compared every `march2014/update/` and `kom_contents/` file with a June name against June's now-exact tree. Rows are keyed by
   (name, March kom); the March kom number always equals June's, no name sits in two March koms, none is over 59 characters.
   - **277 short-prefix** (March is a strict prefix of June's file): 14 dds, 130 tga, 111 x, 22 y. 49 of them are `update/` files;
     data014 holds 182. One update file is not in X1's 51: `BossRaid_Abyss_Smog.dds` 87507 vs 87508 (why X1 missed it was not
     investigated; the other 228 never came through `update/`).
   - **2 hand-patched:** `Motion_NPC_Ranox_FACEOFF.x`, `_SYOTA.x` (data013), in `update/` and staged.
   - **2 different-version:** `DLG_Common_Button03_NEW.TET` is not truncated; March has 26 entries and 26 names, June 28, and every
     March name is in June (June adds `ERDA_ISLAND_NORMAL`, `ERDA_ISLAND_OVER`), so the key check passes. `DLG_Common_Button03_NEW.tga`
     is the same size and header with 59,325 pixel bytes different (it is not in X1's 51). Neither needs a user decision.
   - 4 `*_Sorted.X` meshes (Lire/Ara cash gear, 2-23 B short of June) are March-native versions, not short copies; they stay out.
   - **Cross-check:** `kom.py stage-june x2_repack_list.tsv --dry-run` (X3) = 16 koms, 281 replaced, 0 new, 0 refused.
4. **Not on the list, decided by me, shown for the record:** `june2017_re/x2_march_native_diff.tsv` has 4,397 non-lua files that are
   in `kom_contents/` under a June name but differ from June in content (1549 tga, 989 dds, 847 x, 622 y, 219 xet, 112 tet, 34 txt,
   25 ogg, ...). None is in `update/`, so none was ever ported from June: they are March's own versions. X5 does not restage them.
   If the user wants June's copy wholesale for any of them (the "June asset wins" memory), that is a separate scoped phase. Also
   excluded: 4,872 `.lua` differences (different compile) and 213 `_manifest.json` name coincidences.
5. **Closure doc.** `JUNE2017_ASSET_CLOSURE.md` is generated; its "Extractor defect" section was rewritten in `june2017_re/assets.py`
   (fixed by `junekom.py`, TET different-version, Ranox hand-patched, repack list named) and regenerated. Verdicts now: same 3,535
   (was 3,159), differs 76 (59), unverified 2 (1), june-only missing 3,620 (3,622), march-only 4, nowhere 156; **same-prefix 364,
   truncated-in-march 15 and update-only 12 are gone** (the 15 plus the update copies show as `differs`, with a note, and are all on
   the repack list). The 2 unverified are `Blur_16.dds` and `Velder_Gate_Stage2_Wall_BlueTile01.dds`: no March copy at manifest size.

**Gate.** `verify --compare` re-run (997 s): `same 222811`, 0 short-prefix, 0 different, 0 missing; `VERIFY OK: 258 koms, 222811 entries ok,
0 failed, 0 header problems`. Files: `.build/junekom/x2_before*.tsv`, `x2_after*.tsv` (not committed).

**For X5:** the plan text says "49 live short copies"; the real count is 281 in 16 koms (kickoff updated). Owes the user: `/cost`.

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

### Phase X3 - Done

2026-10-08, Sonnet 5.5. Tooling only: no flag, no build, no deploy. `tools/kom.py stage-june <name|list-file>` (docstring and
`tools/README.md` updated).

- **Source of bytes.** Imports `junekom`, builds the name index over all 258 June koms (about 8 s), and strict-decodes every June copy
  of a name (end of stream, no trailing bytes, adler32, size == manifest). It does not read June's `*_kom` extract dirs and does not
  depend on X2 having run. Several copies: identical bytes pick one (the kom with an existing staged dir, then the lowest number);
  differing bytes refuse and list the koms. Checked with a synthetic pair (same entry twice: one hash; two entries: two hashes).
  Real June has 0 names in more than one kom.
- **Kom choice.** March's number if a live kom (`elstools inspect` of every `data/*.kom`, cached in `.build/`) or a staged
  `kom_contents/dataNNN_kom/` already has the name, else June's number. The March spelling of the name is kept. A name March has in
  several koms is refused (none exist today). A kom number with no staged dir gets a new one and is labelled `NEW kom dir`.
- **Manifest.** `.lua`/`.txt`/`.kim` `"2"`, everything else `"0"` (live data036: every `.txt` and `item.kim` are 2). A key already
  listed keeps its value and the plan line says so (two lua at `"0"` are the user's parked exceptions). Keys are appended by text:
  an existing manifest keeps its own line ending (153 of 190 are LF, 37 CRLF), a new one is CRLF, 2-space indent, no trailing newline.
- **Refusals:** not in June; undecodable or disagreeing copies; name over 59 characters unless `--long-names`; path separator.
  Refusals do not stop the others; exit code 4 if any. A replaced staged file is first copied to
  `<kom_contents>\..\kom_backup_stage_june\<stamp>\dataNNN_kom\`. Writes are `.part` + replace. `--plan-out F` writes a TSV.
  Never writes into June's tree or the game dir.

**Gate (dry run).** X2 has not run, so the list is the 51 names of `june2017_re/x1_short51.tsv` (the closure's "Extractor defect"
table is the same 51) plus the 5 long names:
- 51 names: 8 koms, 51 replace, 0 refused: data002 17, data013 7, data014 1, data023 19, data043 3, data063 2, data142 1, data215 1.
  Every file is already staged in March (the short copy), so every line reads `replace`. Plan: `.build/x3_dry51.tsv` (not committed).
- 51 + 5 long: the 5 long names are the only refusals (63, 63, 63, 61, 64 characters).
- With `--long-names` the 5 land in: `EffectSet_Moonlight_Rhapsody_..._Display_01/02/03.tga` -> data048 (new, June's kom; March has
  no copy); `Trigger_Hamel_..._Water_A.x` -> data110 (already staged and identical to June);
  `Elysion_Soless_..._Dash_Combo.ogg` -> **data197, which has no live archive and no staged dir**. X5 must confirm the client
  mounts a new data197 (its kickoff says so).
- Real write tested in a scratch `X2_KOM_CONTENTS`: a replace (backup made; bytes equal June's and differ from the hand-patched
  March copy at the `Wait01` bytes), a new file in an existing LF manifest, a new kom dir with a CRLF manifest, and the three
  refusals. Nothing in the real `kom_contents` was written.

**Owes the user:** `/cost`. Next: X2 (the repack list), X4, then X5 (kickoff written).

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

### Phase X4 - Done

2026-10-08, Opus 5.5. Flag `SERV_IRUHADEV_KOM_LONG_NAMES` (`KTDXLIB/Always.h`), code in `KTDXLIB/KGCMassFileManager.{h,cpp}`.
Both files are CP949 and were edited with `bytepatch.py`. Every change is an insert. The header is not in the STATUS Touches column,
but it had to change for the new member.

**Design check.**
- **The struct stays at 60 characters.** `MASSFILE_MEMBERFILEHEADER` is the on-disk V.0.1/V.0.2 member header: `LoadMassFile` reads
  `sizeof()` bytes per member, about `:1803`. V.0.3 also uses the size: the XML branch's `offset` (`:1866`) subtracts
  `sizeof(MASSFILE_MEMBERFILEHEADER) * count`, and `_GetMemberFile`'s `posRealDataStart` (`:1976`) adds it back. The two cancel,
  so V.0.3 would survive a bigger struct, but the binary formats rule it out. Long names are therefore kept in a side map.
- **Users of the name.**
  - The XML read (`:1880-1895`): `MultiByteToWideChar` into a `WCHAR[60]` fails outright for 60+ characters, and the copy is then
    cut to 59.
  - `AddMassFile`'s `m_mapMassFile.insert` (`:504`) was the only place the cut name became a key.
  - `LoadDataFile` (`:919`), `LoadTwoDataFiles` and `IsValidFile` look the map up by the full uppercased wstring, using 255-char
    buffers, so they work unchanged.
  - The by-name `CMassFile::GetMemberFile` linear scan has no callers.
  - `GetFileName` and the `SaveMassFile`/`ModifyMassFile` packer paths (`:2981`/`:3233`) write the 60-char on-disk header. They
    are tool-only and untouched.
  - `pOut->strFileName` (`_GetMemberFile`, `:1961`) still carries the cut copy. Its only reader outside the manager is
    `KTDXApp.h:567`, and only for `.lua`, as error text.
  - `MASSFILE_MEMBERFILEINFO_POINTER::operator=` uses `StringCchCopyW` into 60, and the input is never long.
- **What the flag does.**
  - In the XML `Name` branch, a name longer than 59 characters (`MultiByteToWideChar` returns more than 60 including the NUL) also
    keeps its full `MakeUpperCase(wstring)` form in `CMassFile::m_mapLongMemberFileName[index]`. The map is cleared next to
    `m_vecMemberFileHeader.clear()`.
  - `AddMassFile` registers that full name instead of the cut one and logs `[KOM_LONG] <kom> <name>` through `KLOG("ErrorLog.txt")`.
    If the insert fails, the line gets a ` DUPLICATE (not registered)` suffix.
  - Names of 59 characters or fewer never get a side-map entry and run the original expression.
  - The line is written when the koms are mounted, in the `CX2Main` constructor (`X2Main.cpp:1407`). It survives:
    `ErrorLogMsg` writes through the same `KLOG` stream, which appends (`KSuperAssert.cpp:128`, `ios::app`). The only code that
    deletes `ErrorLog.txt` is `CrashPostCallBack` (`X2Main.cpp:429`), after a crash.
  - A non-`WCHAR_DIR` build fails with `#error`.

**Not inert today.** A scan of every live game-dir kom (104,578 names) found one name over 59 characters:
`data110.kom` `Trigger_Hamel_Frozen_Water_Hall_Stage3_Stone_Statue_Water_A.x` (61). March's own content uses it as a line-map
trigger mesh (`data040_kom/Hamel_Frozen_Water_Hall_Stage3_LineMap.lua`, `MESH_NAME`). Until now that mesh was never found. With the
flag on, it is registered at startup and should load.

**Hygiene** (`flagcheck --worktree --files` the 3 files): `files: 2 PASS, 1 DEFINE, 0 OTHER, 0 FAIL`, `0 encoding change(s)`,
`RESULT: PASS`. The one deleted line flagcheck reports in `Always.h` is not X4's. HEAD (peer commit `7ba2f9d`, C9h-2d) has two blank
lines after `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP` and the working tree has one. X4's commit is built from HEAD plus the X4 block,
so that line is left as the peer committed it.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,352,064 B, dated 2026-10-08 08:51:36, md5 6877fb3fd3f219f2998652161d43dddd, equal to
X2/US_SERVICE/x2.exe (2026-10-08 08:51:31); built at mods/june-buff-port 4bd51f2 + 3 uncommitted code file(s)` (the 3 are X4's). The
strings `[KOM_LONG] ` and ` DUPLICATE (not registered)` are in the deployed exe.

**How to see it:**
1. Launch, then `logs.py --tag KOM_LONG`. Expect exactly one line, for data110's trigger mesh.
2. Optional, in game: Hamel, Frozen Water Hall, stage 3. Expect the stone-statue water trigger mesh and no `KEM_ERROR136` for it.

The other 5 long names are staged in X5.

**Open:** none. **Owes the user:** a launch for the log line, and `/cost`.

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
