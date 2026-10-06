# June buff port - Phase 16 step 3b, part 6 - identify June's periodic unit list (3206d480)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b6` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b5 (both edit X2GUUser.cpp; never in parallel) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` if it ports anything (`_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b6-review.md` if code lands |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b4 -- Review"
```

The `funcmap.tsv` rows for `3206d480`, `31fe0b90` and `31fd8b70` carry the decode so far.

## Task

Step 1 is read-only. The 16-step3b4 review found `3206d480` writing June's per-socket cool-time store
(`+0x8b98`) and decided (user, 2026-10-06) to identify it here.

1. **Identify it.** It walks a list of 24-byte entries on the unit's `CX2Unit` (`+0x87b4` -> `+0x280` / `+0x284`). It skips an
   entry whose key (entry[0]) still has a cool time, and one that the game type (`+0x8c`: 9, or 0xb/0x12/0x15/0x17/0x1a) excludes.
   Then it switches on entry[1]: 0 = a buff (`32089f60` + slot 0x224), 1 = `3204d440`, 2 = a heal scaled by `3342f360`
   (`322d37d0`), 3 = `322d4f80` behind a game check. Each fired entry restarts its cool time (`31fe0b90`).
   - Name the March system it belongs to: who fills `CX2Unit +0x280`, what its data file is, and what the three callers are
     (`32298590`, `32a71a90`, `329e2410`).
   - Say whether its keys can collide with socket IDs in the shared store.
2. **Decide the home.** Say whether it is a buff-port system (port it here, whole, onto 3b4's `GetJuneSocketCoolTime` /
   `SetJuneSocketCoolTime`), a class one (checklist section 7, D8 rule), or another plan's. If March has no such list, say so;
   do not invent its data. Hand the choice back to the user if it is not clear-cut.
