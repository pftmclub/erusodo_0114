# June asset port: every remaining June non-lua asset in the March client

Plan key `june-asset-port` · branch `main` · log tag `[KOM_MOUNT]` · flag family `SERV_IRUHADEV_KOM_MOUNT_*` ·
kickoffs `kickoffs/june-asset-port/` · backlog Stage 15 · created 2026-10-08.

## Context

`june-asset-extract` (X0-X5, closed 2026-10-08, `JUNE_ASSET_EXTRACT_PHASES.md`) built the tools:
- `tools/junekom.py`, the byte-exact June extractor;
- `kom.py stage-june`, which enforces the algorithm, placement, duplicate and long-name rules;
- `SERV_IRUHADEV_KOM_LONG_NAMES`.

X5 shipped only the 286 files March already had in a damaged or hand-patched form. The user then asked (2026-10-08) to port **every
June non-lua asset still missing or different**, byte-exact, so the later lua ports find their assets in place.

Decisions (user, 2026-10-08):
- **Every collision is replaced by June's file.** The user's words: "replace existing March assets if they are colliding with the
  June assets". Nothing is held back. When a March consumer breaks because June's file lacks a key it reads, the consumer gets a
  follow-up fix: port the June consumer, or a Ghidra engine phase. The asset is never reverted or patched for it.
- **`.txt` stays with june-effect.** March aborts a `.txt` at its first unknown key, and june-effect owns those keys. This covers the
  34 colliding and 51 June-only particle, mesh-player and other `.txt` files.
- **Scaleform files ship now, inert.** The kom `.gfx`/`.swf`, the event-scene `.xml` and the loose `eventscene/*.swf` go in byte-exact.
  March has no loader, so nothing reads them until Stage 9.

## Rules

1. **June files are byte-exact.** No edit, ever. If March cannot use one, that is a Ghidra-traced engine phase, never a file patch
   (memory `june-assets-byte-exact-no-edits`).
2. **June's `14JUN17\data` and its koms are read-only inputs.**
3. **Staging and packing go through the tools.**
   - Every staging goes through `kom.py stage-june` and every pack through `kom.py pack`.
   - **Ask before every copy:** one AskUserQuestion per wave that lists every kom.
   - The game must be closed.
4. **Gates, in force from P0** (P0 builds the tools):
   - **(a) No-loss.** A pack diff with `-N removed` refuses the copy.
   - **(b) Unexplained change.** An entry that shows `~changed` but is not on the wave list is decoded from both koms and
     byte-compared. Equal bytes pass. Anything else stops that kom.
   - **(c) Global duplicate.** Checked right before each copy, over the would-be-live set: the live koms with this wave's packed koms
     swapped in. Peer sessions stage too, so a check at staging time is not enough.
   - **(d) Staged bytes.** Every staged file equals June's strict decode.
5. **Pairing and order.**
   - An atlas texture and its `.TET` move together.
   - A skin mesh and its paired motion move together when both differ.
   - June-only files land before the replace waves, so a June mesh that names a June-only texture finds it.
6. **Collisions: June replaces March, always.** A predicted or observed break goes to an owner, not to a question:
   - **class-specific consumers** (a class motion name, a weapon bone) go to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (memory
     `hand-char-port-material-to-char-ports`);
   - **any other consumer** goes to the plan that owns it (june-effect, Stage 9 UI, ...);
   - **if March's engine cannot read June's file at all**, a new Ghidra phase (memory `plan-unowned-june-branches-without-asking`).
7. **One flag per logical change.** Use `SERV_IRUHADEV_KOM_MOUNT_DEBUG` (P1, short-lived) and, if P11 runs, its own change. Edit
   `KGCMassFileManager.cpp` (CP949) with `tools/bytepatch.py` only.

## Starting context

Every repeated step is a tool (`tools/README.md`). Do not hand-type msbuild, unifdef, kom packing or log greps.

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_ASSET_PORT_PHASES.md "<heading>"` |
| June kom reader | `python tools/junekom.py list / extract / verify ...` |
| Stage June files | `python tools/kom.py stage-june <list> [--dry-run] [--long-names] [--plan-out F]` |
| Pack / deploy a kom | `/elstools pack dataNNN`, then ask, then `/x2-kom-packed copy dataNNN` (`tools/kom.py`) |
| P0's new gates | `kom.py verify-staged`, `kom.py pack/copy --wave`, `kom.py dupcheck`, `kom.py copy-loose` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply <spec>` (x2-byte-edit) |
| Flag hygiene | `python tools/flagcheck.py SERV_IRUHADEV_KOM_MOUNT_DEBUG --worktree --files <files>` |
| Build + deploy | `python tools/build.py --deploy` (background; x2-build-deploy) |
| Play-test logs | `python tools/logs.py --tag KOM_MOUNT` (x2-check-logs); KEM_ERROR lines are in `ErrorLog.txt` |

Paths:
- **June data:** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`. It holds 258 koms (`data001-226`, `datai001-032`),
  the exact `*_kom\` extracts (verified by X2), and loose `music\`, `movie\` and `eventscene\`.
- **March staging:** `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\dataNNN_kom\`.
- **March game dir:** `...\24965799\data`.
- **Intermediates** go to `.build\` or `kom_contents`, never the session scratchpad: C: has about 10 GB free, F: about 700 GB.

## Established facts (2026-10-08, planning session)

**Census.** All 222,811 June entries were compared with the live March koms and `kom_contents` (md5 of June's exact extract against
the staged March file). P0 regenerates this table as `june2017_re/p0_census.tsv`.

| Bucket | Files | Raw size | Action |
|---|---|---|---|
| June-only | 117,562 | 10.5 GB | add: 33,819 into 66 kom numbers March has no archive for, 83,692 into 151 existing March koms |
| Same bytes | 92,457 | 4.7 GB | nothing |
| Differs (March's own version) | 4,397 | 0.9 GB | replace with June's (Rule 6), except the 34 `.txt` |
| March-unstaged | 1,066 (data101 1,065, data118 1) | 28 MB | restore into `kom_contents` first: a repack of data101 today would delete them |
| `.lua` | 7,329 | | lua ports, not here |

By type:
- **June-only:** dds 35,966; tga 26,699; ogg 25,426; x 19,792; y 5,015; xet 3,198; tet 826; gfx 503; txt 51; wav 16; xml 12; ttf 12;
  bmp 10; swf 6.
- **Differs:** tga 1,548; dds 989; x 847; y 622; xet 219; tet 111; txt 34; ogg 25; xml 1 (`AutoSetupGameOptionWeightConfig.xml`);
  fx 1 (`XRenderer.fx`).

**Loose files** (March loads them from disk only):

| Dir | New | Differs | Note |
|---|---|---|---|
| `music\` | 79 | 34 | |
| `movie\` | 32 `.avi` | 0 | |
| `eventscene\` | 2,383 `.swf` | | inert |

**Engine facts:**
- **New kom numbers mount.** `X2Main.cpp:799-830` mounts `data1..226` and `datai1..32` under
  `SERV_IRUHADEV_KOM_FILE_COUNT_226`. June's numbering is the same range.
- **A mount reads headers only.** Payloads are read on demand (`KGCMassFileManager.cpp:1826-1893`, `:2068`). Offsets are DWORDs, and
  the largest June kom is 262 MB.
- **Duplicates are loud.** Under `NOT_ALLOW_DUPLICATE_FILES` (`Always.h:3245`, defined), `AddMassFile` (`:576`) does three things:
  - deletes the whole kom;
  - shows a MessageBox;
  - under `_IN_HOUSE_`, appends the name to `existfileinfo.txt` (`:510`). That is a plain `fopen` append, so it survives the
    mount-time ErrorLog loss.
- **Mount-time ErrorLog lines are lost** (X4, memory `errorlog-loses-mount-time-lines`), so proofs are use-time lines.
- **The March `X2_offline.exe` is not LargeAddressAware.** June's `x2.exe` is, and June ships `increaseva.bat`.

## Phases

| Phase | What | Run with | Build | Play-test |
|---|---|---|---|---|
| P0 | Tooling, census, restore | Sonnet 5.5 high | no | no |
| P1 | Mount diagnostic + baseline | Sonnet 5.5 high | yes | boot to village |
| P1 review | | Sonnet 5.5 high | | |
| P2 | Pilot: 5 new kom numbers | Sonnet 5.5 high | no | boot + walk |
| P3 | Remaining new kom numbers (61) | Sonnet 5.5 medium | no | boot + walk |
| P4a-c | June-only into existing koms, 3 waves | Sonnet 5.5 medium | no | boot + walk per wave |
| P5 | Collision key forecast | Opus 5.5 high | no | no |
| P5 review | | Opus 5.5 high | | |
| P6 | Replace: textures + sounds (no atlas change) | Sonnet 5.5 high | no | visual tour |
| P7 | Replace: TET/XET + paired atlases | Sonnet 5.5 high | no | UI tour |
| P8 | Replace: meshes + motions | Sonnet 5.5 high | no | NPC/char tour |
| P9 | Loose: music, movie, eventscene | Sonnet 5.5 high | no | listen |
| P10 | `XRenderer.fx` | Sonnet 5.5 high | no | render tour |
| P11 | (conditional) LargeAddressAware | Opus 5.5 high | yes | memory |
| P12 | Close-out census | Sonnet 5.5 medium | no | no |

## Phase P0 - Tooling, census, restore

**Run with:** Sonnet 5.5 high. No build.

1. **Census script** `june2017_re/asset_port_census.py`:
   - one row per June entry: bucket, June kom, March kom, name, size, ext, route;
   - excluded types carry their route;
   - writes `june2017_re/p0_census.tsv` and `june2017_re/p0_waves.tsv` (the per-kom wave table for P2-P4 and P6-P8).
   - The planning session's script compared June's exact extract with the staged March file by md5, through `kom.py live_names()`
     and `staged_names()`. Re-create it there; it must reproduce the counts in "Established facts".
2. **Restore the 1,066 March-unstaged files** into `kom_contents` byte-exact from the live V.0.3 koms, using `junekom.Kom` (it reads
   V.0.3 algo 0) or elstools extract.
   - Gate: `kom.py pack data101 data118` shows `+0 / -0 / ~0` against live.
   - These koms are not copied, because nothing changed.
3. **New `kom.py` subcommands and options.** Reuse `stage-june`'s June index and strict decode and the `inspect`/`entry_map`/`diff`
   helpers, and update `tools/README.md`.
   - `verify-staged <list|kom>`: gate (d). Seed it from `.build/x5_review.py`.
   - `pack ... --wave <list>` / `copy ... --wave <list>`: gates (a) and (b), with decode-and-compare for every unexplained `~changed`.
   - `dupcheck [--with dataNNN ...]`: gate (c) across every live `data*`/`datai*` kom, with packed replacements swapped in.
   - `copy-loose <list> --dest music|movie|eventscene`:
     - backs up to `kom_backup_tools\loose\<stamp>\`;
     - writes `.part`, then replaces;
     - verifies md5;
     - refuses while the game runs.
4. **Settle X5's data048 "~487 changed"** with gate (b) and record the result in the Done note. X5 cleared it on equal decompressed
   size only.

**Done gate:**
- census TSVs committed, with the counts matching "Established facts";
- the restore pack diff is `+0/-0/~0`;
- each new subcommand has been exercised on a scratch `X2_KOM_CONTENTS` and on data048.

## Phase P1 - Mount diagnostic and baseline

**Run with:** Sonnet 5.5 high. Build + deploy.

1. **Flag `SERV_IRUHADEV_KOM_MOUNT_DEBUG`** (short-lived) in `KTDXLIB/Always.h`, code in `KTDXLIB/KGCMassFileManager.cpp` (CP949,
   `bytepatch.py`).
   - **In `AddMassFile`,** record per kom: the kom name, member count, registered count (inserts that succeeded) and mount
     milliseconds. After the last mount, also record the process `PrivateUsage` (`GetProcessMemoryInfo`).
   - **On the first `LoadDataFile` call after mounting** (use-time, so it reaches ErrorLog), log:
     - one `[KOM_MOUNT] <kom> files=<n> registered=<n> ms=<n>` line per kom;
     - one `[KOM_MOUNT] total koms=<n> entries=<n> ms=<n> private=<MB>` line.
   - Inserts only; nothing per frame.
2. **A comparator** (a `logs.py` option or a small script): for every kom, `files == elstools inspect file_count` and
   `registered == files`. A kom that was dropped or never mounted is an absent line.
3. **Baseline play-test:** boot to a village and walk. Record the total mount milliseconds and private MB in the Done note.

**Done gate:** a `[KOM_MOUNT]` line for every live kom; the comparator is clean; the baseline numbers are recorded.

## Phase P2 - Pilot: 5 new kom numbers

**Run with:** Sonnet 5.5 high.

- **Koms:**
  - data152: 1,002 files, about 2 GB raw, the largest;
  - data179: 3,952 files, the most;
  - data224;
  - data214;
  - datai027: an INT kom, because `datai` mounts through its own loop.
- **Steps, in order:**
  1. `stage-june` (each kom reads `NEW kom dir`);
  2. `verify-staged`;
  3. `pack`: the diff says "no live kom, new archive", and the entry count equals the wave count;
  4. `dupcheck`;
  5. ask;
  6. `copy`.
- **Play-test (boot and walk).** All of these must hold:
  - `[KOM_MOUNT]` lists the 5 new koms with `registered == files`;
  - no MessageBox appeared;
  - `existfileinfo.txt` is unchanged;
  - 0 KEM_ERROR.
- **Measurements:** compare mount milliseconds and private MB with P1's baseline. If private memory rose by more than about 300 MB,
  or the total passes about 1.4 GB, schedule P11 before P4.

## Phase P3 - Remaining new kom numbers

**Run with:** Sonnet 5.5 medium.

- The other 61 kom numbers March has no archive for, about 28.8k files.
- Same procedure and gates as P2, in one wave, or two if P2's measurements say so.
- The inert Scaleform files in these koms go in too.

## Phase P4 - June-only into existing March koms (P4a / P4b / P4c)

**Run with:** Sonnet 5.5 medium, one session per wave.

- **Scope:** 151 koms, 83,692 files, about 5 GB raw.
- **Waves:**
  - P4a `data001-075`;
  - P4b `data076-150`;
  - P4c `data151-226` plus `datai`.
- **All four gates apply,** because every kom is a repack of a live archive. Gate (b) catches staged dirs that drifted from live.
- **data036 is shared** with the lua ports, the item.kim work and the 12 event-scene `.xml`. Pack it only when no peer session holds
  an uncommitted `data036_kom` change (STATUS Touches).
- **Inert Scaleform** files go in the same waves.
- **Play-test per wave:** boot and walk one village, one field and one dungeon whose koms are in the wave. P2's gate applies.

## Phase P5 - Collision key forecast

**Run with:** Opus 5.5 high. No build.

Nothing is held. P5 predicts which March content will need follow-up work once June's files replace March's, so each owner knows before
P6-P8 land.

1. **`june2017_re/asset_keys.py`**, with parsers written from March's own loaders:

   | Format | Parse | Source |
   |---|---|---|
   | TET | keys | reuse `june2017_re/buff_icon_check.py` `tet_keys` |
   | XET | keys | `KTDXDeviceXET_Preprocessing.h` |
   | KSM/KME | animation-set and frame names | `KTDXDeviceXSkinMesh.*`, `KTDXDeviceXMesh.*` |
   | xof | frame and animation-set names, including MSZIP `bzip` | - |

   Start from `june2017_re/c9g_animation_name_census.py` `motion_names`.
2. **March reference index:**
   - string constants of every live March lua, from the bytecode constant scan (XOR'd where needed, memory `client-lua-xor-key`);
   - March particle and mesh-player `.txt` keys;
   - C++ string literals in `X2Lib`/`KTDXLIB`;
   - texture names embedded in meshes and XETs (`june2017_re/fxdata.py` `embedded_textures`).
3. **Output** `june2017_re/p5_collision_forecast.tsv`, one row per colliding file. Columns:
   - verdict: `clean` or `will-break`;
   - the keys June lacks;
   - the March readers;
   - the owner (Rule 6).
   - Each `will-break` row becomes a row in its owner's doc in the same commit.
   - A pixel redraw at an unchanged key is `clean` (memory `june-asset-wins-over-march-at-same-filename`).
   - `AutoSetupGameOptionWeightConfig.xml` is forecast against its C++ reader, and is replaced anyway.

**Done gate:** every colliding non-`.txt` file has a row, and every `will-break` row has been handed to its owner.

## Phase P6 / P7 / P8 - Replace waves

**Run with:** Sonnet 5.5 high. Depends on P4 (June-only files first) and P5.

| Wave | Files |
|---|---|
| P6 | `.dds`/`.tga`/`.bmp` with no `.TET` change, plus the 25 `.ogg` |
| P7 | `.TET`/`.XET` with their paired textures |
| P8 | `.x`/`.y` meshes and motions, paired |

Each wave runs `stage-june <wave list>`, then the four gates, then ask, then copy, then play-test.

**Hand-off:**
- Pick real places from the wave list: a village atlas key, a boss portrait, an NPC whose motion changed. Look for June's art and
  motion, and no KEM_ERROR for those names.
- Also visit one or two P5 `will-break` items, to confirm the forecast.

**A break on a replaced file is not reverted.** June's file stays, and the break gets a row with its owner, or a new phase if there
is no owner. The backup in `kom_backup_tools` exists only for a revert the user explicitly asks for.

## Phase P9 - Loose files

**Run with:** Sonnet 5.5 high.

Run `copy-loose`, with backups:
- `music\`: 79 new, and 34 differing (replaced);
- `movie\`: 32 `.avi`;
- `eventscene\`: 2,383 `.swf` (inert).

**Check:** a field whose BGM differs, such as Hamel `field_hamel001.ogg`, plays June's track. No error is logged.

## Phase P10 - XRenderer.fx

**Run with:** Sonnet 5.5 high.

- **Why its own phase:** this is the global shader.
- **Pre-check:** March's 38 looked-up names (`KTDGXRenderer.cpp:16-105`) all exist in June's file.
- **Stage and copy** data041, with only this change, after asking.
- **Play-test:** a village, a dungeon and the character select. Look for:
  - no `KEM_ERROR50`;
  - no black or untextured meshes.
- **Revert:** if the screen breaks, revert data041 from its backup (the one revert this plan allows without asking, because a broken
  renderer blocks every other play-test), then open a Ghidra phase.
- **Already shipped:** the June-only `.fx` files and the `Cartoon*.bmp` ramps go in P4, inert.
- **Waits for june-engine R2** *(2026-10-16, Stage 2 planning)*: June compiles `XRenderer.fx` with `D3DXMACRO`s and sets 14 parameters
  March does not, so the pre-check above passes and the result is still not June's render. P10 copies after R2's review
  (`JUNE_ENGINE_REMAINDER_PHASES.md` M1). Also: June ships 3 June-only `.fx` (`XEnvBoxRenderer`, `XHeatShimmerRenderer`,
  `XRenderer_X2TOOL`, all data041), which the 2026-10-08 census did not count; P0's census should show them (P4a, inert).

## Phase P11 - LargeAddressAware (conditional)

**Run with:** Opus 5.5 high. Runs only if the P2/P4 memory numbers call for it.

- **Proposal:** `LargeAddressAware=true` for `X2/X2_2010.vcxproj` `US_SERVICE`, matching June's exe.
- **Why it needs the user:** it is a linker setting, so it cannot sit behind a source flag. **Propose it to the user first.**

## Phase P12 - Close-out

**Run with:** Sonnet 5.5 medium.

1. **Re-run the census.** Expected:
   - June-only: 0, except excluded types;
   - differs: 0, except the `.txt` files (june-effect);
   - unstaged: 0.
2. **Bookkeeping:** update backlog Stage 15 and STATUS.
3. **Debug flag:** remove `SERV_IRUHADEV_KOM_MOUNT_DEBUG` (`flagcheck --range`), or keep it if the user wants it.

## Excluded and routed

| Item | Route |
|---|---|
| `.lua` (7,329) | lua ports |
| `.kim` | june-item-kim |
| `.txt` (85: 34 colliding, 51 June-only) | june-effect (user, 2026-10-08) |
| `DialogLog.txt` | dropped (June's own log) |
| Root-dir binaries and configs (`unpacked_x2.exe`, dlls, `.ini`) | not assets |
| P5 `will-break` rows | not exclusions: the file is replaced and the consumer fix is routed per Rule 6 |

The `.txt` row includes `GameOptionPreset.txt`, `fontconfig.txt`, `UnitTitle*.txt`, `NUI_ERIS_BOSS_Map03_A.txt`,
`Npc_All_Ariel_Face_Talk_Dark.txt` and `AIN_POWER_OF_GOD_06_1.txt`.

## Play-test hand-off (every wave)

1. `stage-june` reports 0 refused, and `verify-staged` 0 problems.
2. `pack`: `-0 removed`, `+added == wave count` per kom, and every `~changed` is in the wave or decode-equal.
3. `dupcheck` reports 0.
4. `copy`: md5 of each live kom equals the packed kom.
5. **Log check, scoped to the phase:**
   - `logs.py --tag KOM_MOUNT` has a line for every kom, with `registered == files == inspect`;
   - `existfileinfo.txt` has not grown, and no duplicate MessageBox appeared;
   - 0 KEM_ERROR88/89/343/390/95/96/87/136/132/50;
   - mount milliseconds and private MB are recorded against the baseline.
