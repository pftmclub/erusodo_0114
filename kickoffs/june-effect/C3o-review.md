# June effect completion - C3o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3o review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3o done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI` (+ `_DEBUG`), `SERV_IRUHADEV_JUNE_NPC_STATE_START_EFFECT_SET`, `SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW` |
| Log tag | `JUNE_FX C3o` (`python tools/logs.py --grep "JUNE_FX C3o"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done"
grep -n "(C3o)" june2017_re/funcmap.tsv
```

## Task

Review C3o's commit against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`, `xordec2.py`):

- **The two parser bits** in `32784c00`: re-decode the keys at `@32789e92-3278a036` (bit `0x4f`, `IS_APPLY_CINEMATIC_UI`,
  a miss stores 1) and `@3278ba7b-3278bbb5` (bit `0x4d`, `DIRECTION_STATE`, a miss stores 0). Confirm FALL_DOWN is bit `0x1a`.
- **The tail order** in `327ac9d0 @327af25b-327af612`: cinematic store before `3275a070`, effect-set call and world-colour parse after
  it; the `+0x40dc` reset in between is checklist 7l's. Confirm `+0x8f18` is reset earlier (`@327adab4`) like March's
  `m_iEffectSetToPlayProgress = 0`.
- **CHANGE_WORLD_COLOR_DATA**: the two pre-set -1 stores, the miss defaults (START 0, END `3346ede8` = 9999), the keys
  (`START_TIME` / `END_TIME`), and that the table is read from the open state table (`+0x2b4`).
- **The window** in `327b53b0 @327b54bb-327b557d`: the two compares (`test ah, 0x41; jp` then `test ah, 1; jne`), the colour argument
  order (run the x87 bytes `@327b5532-327b555e` through `x87test.py`'s popping harness), and `32296be0` = FadeWorldColor( origin, 1 ) +
  both times -1, only when START >= 0.
- **`+0x8e39` readers / writers**: the raw disp32 scan in the Done note; confirm `327cea70` and `32d530a0` read it only inside the
  focus-camera branch.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI --range <C3o commit>^..HEAD --with SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI_DEBUG SERV_IRUHADEV_JUNE_NPC_STATE_START_EFFECT_SET SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW`.
Turn `SERV_IRUHADEV_JUNE_NPC_CINEMATIC_UI_DEBUG` off when the play-test has been read. Fix clear transcription errors under the same
flag; hand design decisions back to the user.
