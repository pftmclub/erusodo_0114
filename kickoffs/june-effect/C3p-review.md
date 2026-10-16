# June effect completion - C3p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3p review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3p done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SKIP_START` (+ `_DEBUG`) |
| Log tag | `JUNE_FX C3p` (`python tools/logs.py --grep "JUNE_FX C3p"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3p - Done"
grep -n "(C3p)" june2017_re/funcmap.tsv
```

## Task

Review C3p's commit against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`, `vtslot.py`):

- **Parser**: `32784c00 @3278ba67-3278bbcb` (bit `0x4d`, `DIRECTION_STATE`, a miss stores 0); the port reads it before `FLIP_DIR_END`.
- **Arming** `327ac9d0 @327af25b-327af360`: empty-list reset, armed = 1, push of NPC `+0x270` (= `GetUID()`); placed before C3o's
  cinematic store. If C3f-1 has landed by then, confirm its 2b block sits before this one.
- **End of a DIRECTION_STATE state** `3277ee10 @3277f863-3277f9b7`: the order disarm -> `322d8070` (`m_bShowBossName` on every
  NPC) -> my unit `+0xde8` = 0 -> `322d8010`, and the placement after `DoCustomFunctionByBuffInCommonStateEnd`.
- **ESC**: `31f1e1a0` is `CX2State::HandleMsgByESC` (vslot `0x1e8`; `0x1ec` = `HandleMsgByESCImp`), and the skip returns false
  before Imp. `322f4c60`'s party test against `m_vecPartyMemberData.size() > 1`.
- **Skip** `322f11d0`: the `|t + 1| > FLT_EPSILON` test, armed-only NPC walk, `StateChange( WAIT_STATE, false )`, clear. The walk is
  over a copy (reentrancy); check that this matches June's cached begin/end.
- **Hint** `32305be0`: strings 31002 / 31003, font 2, ( 970, 740 ), white, `FS_SHELL`, black outline, `DT_RIGHT`; last call in
  `3230aa60` = the end of March's `CX2Game::OnFrameRender`.
- **The subtitle clear** (`31ef7370( 27, -1 )`) is nested under C3z-4b's `SERV_IRUHADEV_JUNE_MOVIE_DESCRIPTION`; confirm it compiles in
  once that flag is committed.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKIP_START --range <C3p commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKIP_START_DEBUG`.
Turn `SERV_IRUHADEV_JUNE_SKIP_START_DEBUG` off when the play-test has been read. Fix clear transcription errors under the same flag;
hand design decisions back to the user.
