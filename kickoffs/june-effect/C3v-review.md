# June effect completion - C3v review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3v review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3v done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_TIMER_FOCUS_UNIT` (+ short-lived `SERV_IRUHADEV_JUNE_C3V_DEBUG`, turn it OFF in this review) |
| Log tag | `python tools/logs.py --grep "JUNE_FX C3v"` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3v - Done"
grep -n "(C3v)" june2017_re/funcmap.tsv
grep -n "^| 182 " JUNE_BUGS.md
```

## Task

Review C3v's commit against June (`june2017_re/funcdis.py`, Ghidra read-only):

- **Base `322d5f70`**: re-walk the four exits (no handle; handle unit != `m_optrFocusUnit` -> cancel `@322d6207`; `!( 0 < time )`
  `@322d5fe0`; expiry `@322d6194`) and the hold path. Check the x87 conditions (`test ah,5 / jp`, `test ah,0x41 / jne`,
  `test ah,0x44 / jnp` on `0x33436c90` = -1.0) against the port's `if`s, and the return value of each exit.
- **Setter `322d63f0`**: argument order through ctor `32311680` (`+0xc` time, `+0x10` distance, `+0x14` / `+0x15` the two bools),
  and that June drops March's `NomalDirectCamera` but keeps the camera `OnFrameMove( 0, 0 )`.
- **Camera calls**: `31fb3c50` (7 args, the 7th unread) = March `NomalDirectCamera( unit, dist, 200, 0, 0, 0 )`; `31fb3640` (10 args):
  read what its last two args do, and say whether March's 8-arg call is enough (reachable only via the unported absolute monster).
- **Overrides**: dungeon / battle field / PvP return values and the base-true early return; the walk moved verbatim (diff the
  `#else` side); `CHUNG_SECOND_CLASS_CHANGE` still ON for PvP.
- **Unit `+0x240`**: the write-only verdict. Optionally sweep reads through the observer base (`[reg+0xa0]` byte after a handle
  deref) to close "not excluded".
- The play-test logged arm -> expire 3.8 s apart for a 1.5 s timer. Confirm that the last-kill slow motion scales the `fElapsedTime` reaching `CX2Game::OnFrameMove` (`X2Game.cpp:2640`); if it doesn't, find why.
- The play-test log: an `arm` with type 1 / 1.50 / 800 followed by `expire` on a boss last kill; no stray `cancel` every frame.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_TIMER_FOCUS_UNIT --range <C3v commit>^..HEAD` (and `_C3V_DEBUG`).
Fix clear transcription errors under the same flag; hand design decisions back to the user. Turn `SERV_IRUHADEV_JUNE_C3V_DEBUG` off.
Next: `kickoffs/june-effect/C3w.md` (written by C3s).
