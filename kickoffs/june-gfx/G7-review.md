# June GFx runtime - Phase G7 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-gfx G7 review` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | Phase G7 done (its Done note and commit) and the user's G7 play-test |
| Flag | `SERV_IRUHADEV_JUNE_GFX_BLEND` |
| Log tag | `[JUNE_GFX] MGR` (`python tools/logs.py --grep "JUNE_GFX MGR"`) |

## Read first

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Rules" "Phase G7 - Blend modes (June's applyBlendMode)" "Phase G7 - Done (2026-10-22, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_GFX_BLEND`)"
grep -a "(GFx G7)" june2017_re/funcmap.tsv
```

## Task

Review Phase G7's commit against June. The things it could get wrong:
- **The tables, byte by byte.** Read `T20` (`337493f0`, 15 x 0x14 B) and `T12` (`33749520`, 15 x 0xc B) in `unpacked_x2.exe` yourself
  (the Done note names the PE-section read) and compare every dword with `s_aT20` / `s_aT12` in `JuneGfxHAL::applyBlendModeImpl`
  (`KTDXLIB/KTDGGFXManager.cpp`). D3D values: BLENDOP ADD 1, SUB 2, REVSUB 3, MIN 4, MAX 5; BLEND ZERO 1, ONE 2, SRCALPHA 5,
  INVSRCALPHA 6, DESTCOLOR 9.
- **The body of `33214cd0`**: render-state ids (0xab BLENDOP, 0xce SEPARATEALPHABLENDENABLE, 0x13 SRCBLEND, 0x14 DESTBLEND, 0xd1 BLENDOPALPHA,
  0xcf SRCBLENDALPHA, 0xd0 DESTBLENDALPHA), the simple path reading `T12` for op and dst but `T20` for src, the `sourceAc` rule
  (SRCALPHA -> ONE on both paths), and `Viewport.Flags & 2` == `View_AlphaComposite`.
- **The two judgement calls** in the Done note: (1) `this+0x2b0` needs no code (ProfileViews.OverrideBlend, applied by the base
  `applyBlendMode` before the Impl); (2) `this+0x4c4` bit 0 is `FillFlags` `FF_Multiply`, now set for modes 3 / 6 and `FF_Invert`
  cleared. Re-check the three readers (`33215d70`, `33217c30`, `33218130` -> shader lookups) and say whether clearing `FF_Invert` /
  moving Screen off the multiply shader is right.
- Modes 15-17 reach the base class unchanged.

Hygiene: `python tools/flagcheck.py JUNE_GFX_BLEND --range <G7 commit>^..<G7 commit>`; paste the `files:` / `in this flag's files:` /
`RESULT:` lines. Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under
the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase G7 - Review" into the plan doc,
update the G7 STATUS.md row, and commit.
