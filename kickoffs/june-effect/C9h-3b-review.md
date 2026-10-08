# June effect completion - C9h-3b review - the riding pet's StateChange / StateChangeFuture rewrite (riding-pet-local keys)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-3b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-3b done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_STATE` (+ `_DEBUG`) |
| Log tag | `JUNE_FX RIDINGPET_STATE` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_STATE"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-3b - Done" "C9h-3 - Done"
grep -n "C9h-3b" june2017_re/funcmap.tsv
```

## Task

Review the C9h-3b code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **Slot order of the manager pairs.** The Done note decoded the INIT_STATE keys from the XOR-built strings (funcmap row `32d36600`). Re-check three of them
  directly: k3 WALK (`+0x4ec`), k7 DASH (`+0x4fc`), k21 SPECIAL_MOVE (`+0x534`). Then re-read `@32d30a3f..@32d30b87` for which pair goes with which state and which
  float of the pair is multiplied (the second for ANIM_SPEED, the first of pair 4 for PASSIVE_SPEED_X `@32d35055`).
- **OnFrameMove's first-change gate** (`32d2ec80 @32d2f19f..@32d2f1c6`): `+0x1c0` is `m_bInitPos`; the gate runs after SetPos; the second GetUserInfo / SetPos runs only in
  the (game or village) branch after the skin anim's OnFrameMove; `PlayAnimAtPlayEnd` is the last statement before the return. Behaviour change on March data: before the first
  StateChange `m_bInitPos` stays false, so InitPos (manager NotifyShowObjectChanged) runs its body.
- **The check `32d3cff0`:** the `-1.0` test on `+0x5b8` (`334380f0`), the `>=` against the anim time, the return value while waiting, and that ECX of the `31fc8810` call is the user.
- **The user hooks:** `DoFrameMoveFuture` (pet non-null, GetRidingOn not tested, true skips the event process) and the clear at the top of `StateChange( int, bool )`.
  The `+0x620` arm (`32031e60`) is deliberately not ported (routed to C9h-4 item 7); confirm the comment says so.
- **Key spellings and defaults** against June's `RidingPet_Optimus_Prime.src.lua` and `june_src_fixed/data037_kom`: `ONGROUND_CHANGE_STATE { OVERLAP_TIME, STATE_NAME }`,
  `ANIM_NAME_AT_PLAY_END`, `ANIM_START_TIME`, `TRANSFORM`, `APPLY_RIDINGPET_ANIM_MOVE`, `ORIGINAL_SCALE_TIME`, `TRANSFORM_SCALE_TIME`, `CHECK_RIDINGPET_SPACIAR_BUFF`,
  `TARGET_RANGE`, `LIMIT_Z_DEPTH`, `DEGREE_RANGE`, `VALUE_POS`.
- **Not ported on purpose:** `IsValidUser` (`32d2c800`, same as March :751, no edit), `+0x5b0/+0x5b4` dead stores, the user-side June keys (C9h-4), the transformer consumers (Stage 17).
- **JUNE_BUGS 113-115** match what June does.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_RIDINGPET_STATE --range <first C9h-3b commit>^..HEAD --with JUNE_RIDINGPET_STATE_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "C9h-3b - Review" into the plan doc, update the
phase's STATUS.md row, check the next kickoff (`C9h-4.md`, then `C9h-3a.md` if it is still open), and commit.
