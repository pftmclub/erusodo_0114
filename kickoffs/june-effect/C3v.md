# June effect completion - C3v - June's CX2Game::UpdateUnitPointer (timed focus unit, bool return) (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3v` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3s (found there; its two flags are in place) |
| Flag | one `SERV_IRUHADEV_JUNE_*` per logical change found |
| Log tag | `JUNE_FX C3v` |
| Review | `kickoffs/june-effect/C3v-review.md` if code lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3s - Done" "C3v - identify, then port: June's CX2Game::UpdateUnitPointer"
grep -n "(C3s)" june2017_re/funcmap.tsv
```

## Task

C3s found that June's base `CX2Game::UpdateUnitPointer` = `322d5f70` (`funcdis.py --func 322d5f70`) is a rewrite of March
`X2Game.cpp` `CX2Game::UpdateUnitPointer` (`m_fFocusTime` only) and **returns bool**. Name and port it:

- First call `322d5ef0` (likely `UpdateHostGameUnit`; confirm).
- A second focus handle at `+0x2f0` (observer, `-0x1a0` to the unit, `+0x2f4` its bool), compared with `m_optrFocusUnit` (`+0x2e4`).
  `+0x2f8` is a timer decremented by `+0x2d0` (`m_fElapsedTime`: is `+0x2f8` March's `m_fFocusTime`?), `+0x2fc` a float tested
  against `0x33436c90`, bytes `+0x300` (CanNotInputAndDisalbeNPCAI `322cef70` while it runs) and `+0x301` (picks camera call `31fb3640`
  or `31fb3c50` on `+0x3b0`, through `31e55340` and `32420bc0` with `g_pMain +0x180`), `+0x240` on the unit set to 1 / 0, `32311720`
  on `+0x2ec`. Returns 1 while the timed focus holds or on the frame it expires; 0 otherwise.
- Find the writers of `+0x2f0..+0x301` (a Lua binding or packet sets the timed focus) and the reader of the unit's `+0x240`.
- **Signature change**: make the base return bool and add June's `if( base() ) return true;` to every override:
  dungeon `327cea70 @327cea74`, battle field `32d530a0 @32d530b7` (after C3s's my-unit guard, which returns false), and the third
  caller `328ba013` (identify: March `X2PVPGame.cpp` is the only other override). Grep every `UpdateUnitPointer` caller.

Data that drives it decides the play-test (a Lua binding used by shipped dungeon scripts makes it positive).
