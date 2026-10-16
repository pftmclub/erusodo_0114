# June effect completion - C3p-2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3p-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3p-2 done (no code: a six-gate map, routed items) |
| Flag | none landed: `SERV_IRUHADEV_JUNE_ESC_GATES` stays undefined unless the review finds a gate with a March home |
| Log tag | `JUNE_FX C3p-2` (only if the review ports something) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3p-2 - identify, then port: June's CX2State::HandleMsgByESC gates" "C3p-2 - Done (2026-10-15, Opus 5.5 high; every gate mapped, nothing portable, no code, no build)"
grep -n "(C3p-2)" june2017_re/funcmap.tsv
```

## Task

C3p-2 found nothing to port: two gates equal March, one cannot fire in March, three sit behind June-only systems (tutorial guide -> C3z-4t,
result-UI mode and GFx `closeToTopZorder` -> the Scaleform/GFx row of `JUNE_PORT_BACKLOG_ORDER.md`). Review the map, not a diff.

- **Key test.** Re-read `31f1e1a0 @31f1e1b5-31f1e1e6` raw (`june2017_re/disasm_range.py 31f1e1a0 31f1e3bc`): `g_pKTDXApp+0x1f8` is the DIManager,
  `31c48870` matches March `CKTDIManager::GetActionState` (`KTDXLIB/KTDIManager.cpp:182`), and `31d24fb0( 1 )` is the keyboard's `GetKeyState`.
  Spot-check that `+0x1f8` is the DIManager from another June caller, not only from the March shape.
- **`+0x210` has no March writer.** The Done note says every writer is June's result-UI flow (setter `31ef7be0`, game vslot `0xd0`, GFx
  `notifyInitialize`, PVE `324b0400`, UI msg `0xbfe`). Check Ghidra xrefs to `31ef7be0` for a caller that is a March-era flow (field, village,
  a result state). If one exists, the mode would be non-zero in March and gate 4 needs a phase.
- **Hide modes 2-4 have no March origin.** Re-check the `31d05af0` callers that pass 2, 3 or 4 (the note lists them) and that the toggle
  `31f02f50` only restores or re-applies a mode.
- **Minimap states.** Confirm June `0xf` / `0x17` = GUILD_AGIT_GAME / PVE_GAME in `31d6fd60`, and that March's live branch is the
  `ELSWORD_NEW_BEGINNING`-off `#else` (one `cl /EP` marker probe if in doubt).
- **Routes.** The C3z-4t STATUS row and `kickoffs/june-dungeons/C3z-4t.md` carry the tutorial gate. The backlog Scaleform row carries the
  rest. Both should name the insertion point.

There is no C3p-2 code, so there is no flagcheck range. If the review finds a gate with a March home, port it under
`SERV_IRUHADEV_JUNE_ESC_GATES` (`#else` = March, inside the key-test `if`, before `DIALOG_SHOW_TOGGLE`, June's order), then `tools/build.py --deploy`.
Hand design decisions back to the user. Write "C3p-2 - Review" into the plan doc, update the C3p-2 review STATUS row, and commit with explicit
pathspecs.
