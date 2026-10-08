# June effect completion - C9h-4b review - June's sound records on the NPC, the square unit and the event scene

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4b done (its Done note), play-test logs read |
| Flags | `SERV_IRUHADEV_JUNE_NPC_SOUND` (+ `_DEBUG`), `SERV_IRUHADEV_JUNE_SQUARE_SOUND` (+ `_DEBUG`) |
| Log tags | `JUNE_FX NPC_SOUND`, `JUNE_FX SQUARE_SOUND` (`python tools/logs.py --grep "JUNE_FX NPC_SOUND"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4b - Done" "C9h-4 - Done"
grep -n "Effect C9h-4b" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4b code against June in Ghidra (read-only) and fix transcription errors under the same flags. The things it could get wrong:
- **The two decoded state keys.** `STOP_SOUND_AT_STATE_END` (bit `0x4e`, `@3278dacf`) and `CUSTOM_STATE_DELETE_SOUND` (bit `0x13`, `@327894a7`) were
  decoded from XOR buffers by script; re-decode both by hand, and check that bit `0x4e` is the same bit CommonStateEnd tests (`word 2 & 0x4000` `@3277f9cb`) and
  bit `0x13` the one CommonStateStart tests (`word 0 & 0x80000` `@327adaa7`). The port keeps its own flag numbers, so only the key names and the two tests matter.
- **The +0x8f74 list is June-only.** Re-check by a fresh `june2017_re/dispsweep.py 8f74` that the accesses are exactly: the ctors, the player push, ReInit, the dtor,
  CommonStateEnd (flagged stop + unconditional clear), and `3275cf80` (no caller). A stop site missing from that list is a finding. Re-confirm that the SOUND_PLAY_LOOP loop
  pushes into `+0x8f5c` (March `m_vecLoopSound`) and that `327bbe73` (the die block) never touches `+0x8f74`.
- **Where each stop sits in March.** NPC DamageReact (`@327b52a8`, March's own guard), CommonStateEnd (`32276ca0` between `3228a200` and `3228a120`; the +0x8f74 stop after
  `322755a0`, before `3229fb10`), CommonStateStart (`@327adaa7`), ReInit, the dtor.
- **ALL_OPTION.** One rotated loop at `@3278e849-3278f11b`, appended to set 0 only, after the SOUND_SET tables (JUNE_BUGS 121); the seven keys' getters against the
  record offsets, as in the C9h-4a review.
- **The square player** `323f17b0`: due test, the riding-pet override (only when the pet and its anim both exist), the roll before the my-unit gate, device args
  `( name, loop +0x40, 3D +0x2c, 1 )` and the 4th argument left out on purpose (`31cdd4e0` on a freshly opened device: confirm it cannot change what a stored handle points at).
- **The stopper** `323f1730`: only the riding-off path in this phase; the riding pet's village block `@32d35124` is C9h-4e's.
- **Regression:** on March data the NPC attach lists stay `DCS_NONE`; the visible change is the played-sound list (every NPC sound is stopped at the dtor / ReInit). The
  square player has no records (no March `SQUARE_*.lua` carries `SOUND_PLAY`); only the empty stop at dismount runs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py JUNE_NPC_SOUND --range <first C9h-4b commit>^..HEAD --with JUNE_NPC_SOUND_DEBUG` and the same for `JUNE_SQUARE_SOUND`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions back to the user. Write "C9h-4b - Review" into the plan
doc, update the phase's STATUS.md row, check the next kickoff (`C9h-4e.md` needs 4a..4d and C9h-3a), and commit.
