# June engine remainder - Phase R4h - Notice text manager and the slot callees' pass argument

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then a user play-test; type `/x2-phase june-engine R4h` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4c; not alongside another session editing `X2Main.*`, `X2NoticeManager.*`, `X2TextManager.*` or `X2State.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_NOTICE_TEXT` (proposed; a second flag for the pass arguments if they split cleanly) |
| Log tag | `[JUNE_ENGINE] NOTICE` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R4h-review.md` (R4h writes it) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4c -- Done" "Phase R4h - Notice text manager and the slot callees' pass argument"
```

Then the funcmap rows tagged `(R4c)`.

## Task

Do "Phase R4h".

- **Notice text manager.** Add June's second `CX2TextManager` (`g_pMain +0x444`: made @31e4abb5, freed @31d93b3f, frame-moved @31f0d945),
  draw it in `CX2State::OnFrameRenderAfter` after the flagged dialogs as `31ef76b0` does, and port the `CX2NoticeManager` functions that
  write it (`325acf90`, `325ad410`, ctor `325ad9a0`; map the rest of the class against March's `X2NoticeManager.cpp`, port whole what
  differs). Check where June frame-moves the notice manager (`CX2StateMenu::OnFrameMove` @324727ad vs March's `CX2Main` @X2Main.cpp:2553).
- **Pass arguments.** `CX2TextManager` render `32744860( pass )`, game edit `32746640( pass )`, `CX2Cursor` render `32868a00( pass )` (agree
  with R4f on who owns the cursor) and any callee R4c2 listed: give March's the pass, and make every slot pass it.
- **Gate (play-test):** a notice (GM / item notice, e.g. from the dev console) still appears; UI otherwise unchanged.
