# June effect completion - C3j-2 review (covers C3j-1 too)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3j-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3j-2 done (its Done note and commit) and its play-test read. Replaces `C3j-1-review.md` (folded in here) |
| Flag | `SERV_IRUHADEV_JUNE_BOUNDING_STATE` (+ `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG`, turn it OFF here) |
| Log tag | `JUNE_FX C3j-1` and `JUNE_FX C3j-2` (offline_server.log; `logs.py --grep "JUNE_FX C3j"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3j-1 - Done" "C3j-2 - Done"
grep -n "(C3j-1)\|(C3j-2)" june2017_re/funcmap.tsv
grep -n "^| 154 " JUNE_BUGS.md
```

## Task

Review C3j-1's and C3j-2's commits against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`):

- **C3j-1's fallbacks** (`327989e0 @327992a2-327992fd`): order DOWN, LANDCRASH, UP; each a `== 0` test on the value just stored;
  `+0x4098` = `m_CommonState.m_DownDamageAir`, the type byte `+0x14f` = `m_acStateType[ STATE_TYPE_DOWN_DAMAGE_AIR_LANDING ]`, `+0x8690` = `m_UpDamage`.
- **C3j-2's registration** (`3279dc80 @3279dd68-3279ddb0`, `3275f280`, `3275cfc0`, `3275d900`): key tested on the INIT_STATE table, the id
  formula (`++counter + index + 4` = March `GUSI_END + ( index - 1 ) + counter`), insertion before NONE..END, the resize by the counter.
  The id-over-127 guard is ours (signed `char` ids): confirm it cannot fire for a script with fewer than 120 states.
- **C3j-2's clone** (`@3279f937-3279faf4`, `32761f80`): source chain (`> 0` tests, WAIT then START), the `( 5, size )` guards and the empty
  anim-name test, the per-type flags (bits 0x1f / 0x24 / 0x2b / 4 / 0x2e / 0x36 / 0x3d mapped through `32784c00`'s keys), DOWN's -4000,
  the event lists (types, next states arr[0x22] / arr[0x23] / arr[6], the 0.2 ANIM_TIMER and its hint push, HAVE_TARGET -1,
  CHECK_NEVER_MOVE_TRUE), and UP's copy of the source's NEGATIVE_Y_SPEED / FOOT_ON_LINE_TRUE events in order.
- **The FIX flag is gone**: CommonStateStart's tail compiles June's copy unconditionally inside the main flag.
- **The play-test result** in the Done note: the `ids` lines show default ids at the end of the state vector, no `overwrite` line on an RT_UP launch.
- **Turn `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG` off** (comment out the `#define`, add a dated "OFF" line to its block), then
  `tools/build.py --deploy`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_BOUNDING_STATE --range <C3j-1 commit>^..HEAD --files X2Lib/X2GUNPC.cpp` and the same
with `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG --files X2Lib/X2GUNPC.cpp KTDXLIB/Always.h`. (`Always.h` under the main flag gives an ERROR
on C3i's pre-existing `#if defined(...)` dependency line, which flagcheck cannot evaluate; that is not a finding.) Paste the
`files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and deleted line.

Write "C3j-2 - Review" into the plan doc, update the STATUS rows (C3j-1 review and C3j-2 review), and commit with explicit pathspecs.
