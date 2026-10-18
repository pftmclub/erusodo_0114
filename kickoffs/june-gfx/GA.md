# June GFx runtime - Phase GA - GFx assets in, byte-exact

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-gfx GA` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | G0 item 4 (where the scene `.swf` load from) for the `.swf` part; the koms can go first |
| Flag | none (data only) |
| Log tag | none |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase GA - GFx assets in, byte-exact"
```

## Task

Do "Phase GA". Extract with `tools/junekom.py` (byte-exact; never elstools for June algo-2), stage into
`24965799\kom_contents\data194_kom`, `data152_kom` (new kom numbers: check `X2Main.cpp`'s 226-kom mount covers them) and
`data036_kom`; add `_manifest.json` entries in the house format (CLAUDE.md, manifests). A name March already has: stop and ask.

`/elstools pack` each, then ask the user before each `/x2-kom-packed copy`. The scene `.swf` go where G0 says. List every
staged file in the Done note (the asset port's P0 census then counts them as same bytes). Done gate as in the section.

## Amended by G0 (2026-10-19)

G0 settled item 4 and widened the list (`june2017_re/g0_gfx_map.tsv`; data facts in the G0 Done note):
- **Scene `.swf` are loose.** June's client reaches them through `GetPullPath` (`31c4e7f0`, every cwd subfolder is a data dir),
  then the opener's disk fallback. Copy all 2,385 June `14JUN17\data\eventscene\*.swf` byte-exact into the March game dir's
  `data\eventscene\`, including `tutorial_back.swf`. Also copy `quest_22254_rosestart.swf` (CWS header over an uncompressed body,
  no XML names it) as June ships it.
  - March already has `eventscene\ranox.swf`, `npc_ranox_png.swf` and `assets.txt`. Compare md5 and ask on a difference.
  - **Ask the user before the copy**, as for a kom copy.
- **data036:** `GFXEventSceneInfo.xml`, the 11 region XMLs (Altera, Besma, Elder, Elysion, Hamel, Peita, Ranox, Ruben, Sander,
  Tutorial_Rose, Velder), `VillageName.xml` (428 B, the list `31eed660` loads them by) and `fontconfig.font`. Leave `fontconfig.txt`
  out: June does not load it.
- **data194:** the whole kom (23 files: `main.gfx`, the screens, `fonts_*.swf`, `gfxfontlib.gfx`).
- **data152:** every `.gfx`, which is the skill cut-ins plus `Common_CutIn_Frame` and the 6 `CUT_IN_TALK_*`, with their `.dds`
  as June ships them.
- **data195 now included** (60 `.gfx`, 165 KB): G4 covers every user skill cut-in, and these are the transcendence / Rose ones.
- **Voice:** the scene XMLs name 2,876 sound files. Of these, 2,793 exist in June koms data179 (1,651), data188 (863), data214
  (138), data174 (54), data186 (28 `Tuto_*`) and a few small koms (070, 167, 168, 078, 178, 176, 175, 171, 173). The other 83 are
  missing from June's data as well.
  - Stage the 2,793 at their June kom numbers.
  - March data179 already has `GFX_ID_5344.ogg` and `GFX_ID_5342.ogg`: compare md5 and ask on a difference.
  - Derive the key list from the XMLs' `soundKey`.
- The inflated size of a CFX file pads to 4096; the header length is the true SWF length. That is a loader fact, not a staging
  problem: stage the bytes as they are.
