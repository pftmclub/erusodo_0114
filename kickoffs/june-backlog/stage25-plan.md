# June backlog - Stage 25 planning - June fonts and text rendering

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-fonts` |
| Output | `JUNE_FONTS_PHASES.md` + `kickoffs/june-fonts/` + a `june-fonts` block in `STATUS.md` |
| Branch | `main` |
| Depends on | nothing. This session is read-only (no code, no build); it needs Ghidra with `unpacked_x2.exe` open. |
| Written | 2026-10-17 (user: "port June's font, make it a stage in the backlog") |

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 25: June fonts and text rendering (user, 2026-10-17)" "Standing rules (every stage, every kickoff)"
python tools/junekom.py list data196      # June's 26 font files
```
Then March's side: `KTDXLIB/KTDGFontManager.cpp` (`InstallFont` ~:1839 / ~:1875, the `D3DXCreateFont` paths ~:260-333),
`KTDXLIB/KTDGFreeTypeFont.cpp` (face load ~:220-290, glyph render ~:397-438, and the glyph-texture upload), `X2Lib/X2Main.cpp`
~:1538 and `InstallFont` ~:8005, `KTDXLIB/Always.h` ~:495-515 (`USE_FREE_TYPE` block). Grep the March and June config Lua for
`InstallFont` (it is Lua-bound) before concluding only `Gulim.ttc` is installed.

## Task

1. **Measure March:** which faces load at run time and from which kom (`datai022`), which text paths use FreeType and which use
   GDI / `D3DXCreateFont`, and the glyph texture's format, filter and alpha handling. Ask the user for a screenshot of the text
   they mean, so the plan targets the path that draws it.
2. **Identify June in Ghidra** (cite addresses, add funcmap rows tagged `(Fonts S25)`): the `InstallFont` calls and default face,
   the FreeType load / render flags and size / hinting setup, the glyph-texture upload and sampler state, and any font list in
   June's config Lua (`june_src_fixed/`).
3. **Diff and route:** what differs (files, faces, flags, rendering). If June's rasterizing equals March's, say so plainly: then the
   change is the font files and face choice only, and the "no anti-aliasing" impression is answered by that.
4. Write the plan in the `new-plan` shape: an asset phase (June's font files byte-exact into the right kom, `/elstools pack`, ask
   before copy) and a code phase (one `SERV_IRUHADEV_JUNE_FONT*` flag, March's code in `#else`), each with a play-test that compares
   before / after screenshots of the same screen. Log tag, flag name, and mark this STATUS row done.
