# June buff port - Phase 16 step 3b, part 6 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b6 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b6 done (see its STATUS row for the commit); never in parallel with 16-step3b7 (X2GUUser.cpp) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for the fire / skip lines) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b6 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b6" carry the decodes (`3206d480`, `3204f190 @3204fc22`,
`32298590 @32298ce2`, `32a71a90`, `329e2410`, `31d6fd60`).

## Task

Review 16-step3b6 against June:
- **The fill, `3204f190 @3204fc22..fcd3`.** The place order (`+0x190` DungeonOnly first, then `+0x191` PVPOnly), the five
  fields and their sources (`+0x27c`, `+0x278`, `+0x290`, `+0x1f8`, `+0x4`), and that `+0x1f8` is `m_fIntervalTime` and
  `+0x4` is `m_ID` in March's `SocketData`. Confirm the push happens for every socket whose type is not -1, buff or not.
- **`3206d480`, entry by entry.** The cool-time skip direction (`@3206d50c..517`), the place jump table (bytes `@3206d740`,
  targets `@3206d734`) mapped through June's state names (`31d6fd60`) onto March's `XS_*` by name, the type table
  (`@3206d754`), where each type sets the cool time (type 0 only with a non-null buff, even with a null clone; type 2
  outside the game test; type 3 only when Fever applied), UpNowHp's second argument 0.0, and the display arguments
  (`value x 100.0`, GetPos(false), GetDirVector, true).
- **Callers.** That March's two `ApplyHyperModeBuff` (X2GameUnit.cpp, X2GUAdd.cpp) already match June's guards for the list
  call, that no other June function calls `3206d480`, and that no March path still reads `m_fSocketOptionHyperCoolTime`
  in the US build.
- **Homes.** The checklist section 7 row for Add / Ain, the open `32083190( 0 )` item, and that HMET_FEVER is still named in
  16-step3c's kickoff.
- Hygiene: `flagcheck.py JUNE_SOCKET_PARITY --range <3b6 first commit>^..HEAD --with JUNE_BUFF_DEBUG`.

Fix only transcription errors, under the same flag. Anything else goes back to the user.
