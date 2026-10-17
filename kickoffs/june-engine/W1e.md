# June engine remainder - Phase W1e - line links to a far line (Elysion line 2 BEFORE_LINE = 130)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then type `/x2-phase june-engine W1e` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W1c (done: June's `GetLandPosition`). Not alongside W2 or another session editing `KTDGLineMap.*` |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_FAR_LINK` (proposed) |
| Log tag | `[JUNE_ENGINE] WORLD` (`python tools/logs.py --tag JUNE_ENGINE`); the warp lines are `[ELYDBG] Portal:` / `InternalPlace:` |
| Review | `kickoffs/june-engine/W1e-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R13 -- Done" "Phase W1c -- Done" "Phase W1e - Line links to a far line (Elysion line 2 \`BEFORE_LINE = 130\`)"
```

Data: `june_src_fixed/data040_kom/Elysion_villeage_LineMap.src.lua` (line 2 = the third `AddLine_LUA`). Warp pads: deployed
`kom_contents/data034_kom/DLG_Map_Village.lua`, start pos 502-513. March code: `CKTDGLineMap::CanDown` (`KTDXLIB/KTDGLineMap.cpp:1238`), the
line loader (`:255` reads `BEFORE_LINE`), and every other `beforeLine` follower (`grep -n beforeLine KTDXLIB/KTDGLineMap.cpp`).

## Task

Do "Phase W1e".

1. **Map June's handling of `beforeLine` / `nextLine`.** `CanDown` is `31c6c310` (June signature, funcmap row 157). Find the switch to the
   before line when a landing hits a line's exact start, and the same in the line loader (`AddLine_LUA` and any post-load link pass) and the
   walk / follow functions. Does June test the linked line's end against this line's start, rebuild links from the geometry, or follow them blindly?
2. **Decide with the evidence.** If June guards, port the whole function(s) as June has them (rule 1). If June follows blindly, June's game had this
   warp too: add a `JUNE_BUGS.md` row and ask the user whether to keep it or design a labelled stand-in (memory `design-missing-june-pieces`).
3. Gate: one throttled `[JUNE_ENGINE] WORLD` line when a far link is refused (or followed), naming the map, the two lines and the gap.

Play-test: Elysion village (chat `elysion`), walk and jump around the back walkway's left end (line 1 / 2 joint, x about -2789, near the red-tree
pad 506). No warp to the blue-tree side unless you stand on a pad. In the log, no `[ELYDBG] Portal: start pos 512` without a `510` before it.
