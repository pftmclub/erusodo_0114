# June effect completion - C3z-4s review - June's CX2SimpleDLG_WithLuaFunc

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4s review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4s done (its Done note and commit); best after its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SIMPLE_DLG` (+ `SERV_IRUHADEV_JUNE_SIMPLE_DLG_DEBUG`) |
| Log tag | `JUNE_FX C3z-4s` (`offline_server.log`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4s - Done"
```

## Task

Review C3z-4s's commit against June. The class code is in `X2Lib/X2SimpleDLG_WithLuaFunc.h` and its bodies in `X2Game.cpp`
(after `CX2Game::DeleteSimpleDLG`). Things this port could get wrong:
- `326b1400`: the six `Move` calls. Check the position, colour and `bDirect` of each against the disassembly (not Ghidra's
  decompile, which repeats the arguments). Also check the random draw (`31d28c20`) and the bound sums.
- `326b1940` / `326b2070`: which argument is the tens-zero test and which is the hide-when-zero test (disassembly: stack offsets
  after 2 vs 3 pushes).
- `326b2170` (MoreDigit): the clamp to `10^n - 1`, the place loop, and that place 0 always shows.
- `326b1720` (gauge cut): the corner order (June +0x3c LT, +0x44 RT, +0x4c LB, +0x54 RB vs March `VP_*`) and the `RB.x` source
  (leftBottom.x). Check `SetSizeX` vs `SetSizeY( , true )`.
- `326b18c0` and the `CX2GameOption::SetIsSkillUITypeA` tail: `g_pMain+0x278` = `m_GameOption` (+0x180) + `m_bIsSkillUITypeA`
  (+0xf8), the sign of the 44 move, and the `+0x11` initial value 1.
- Lifetime: the game ctor sets NULL (@32307743), the dtor deletes (@32308b85), and `SAFE_DELETE_DIALOG` on the handle.
- The skipped June block in `SetIsSkillUITypeA` (`g_pData+0x6c +0xd8 == 6` -> `32ed4720`): confirm it is unrelated and route
  it to a row if no row owns it.

Hygiene is one command: `python tools/flagcheck.py JUNE_SIMPLE_DLG --range <C3z-4s commit>^..<C3z-4s commit> --with JUNE_SIMPLE_DLG_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Spot-check 2-3 items in Ghidra (read-only). Fix clear transcription
errors under the same flag (then `tools/build.py --deploy`). Once the play-test passed, turn `_DEBUG` off. Write
"C3z-4s - Review" into the plan doc, update the STATUS row, and commit.
