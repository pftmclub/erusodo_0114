# June asset port - Phase P4a - June-only into existing koms, data001-075

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase june-asset-port P4a` |
| Plan doc | `JUNE_ASSET_PORT_PHASES.md` (branch `main`) |
| Depends on | P3 |
| Flag | none (P1's `SERV_IRUHADEV_KOM_MOUNT_DEBUG` must be on in the deployed exe) |
| Log tag | `[KOM_MOUNT]` (`python tools/logs.py --tag KOM_MOUNT`; KEM_ERROR lines in `ErrorLog.txt`, duplicate names in `existfileinfo.txt`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ASSET_PORT_PHASES.md "Rules" "Starting context" "Phase P4 - June-only into existing March koms (P4a / P4b / P4c)" "Play-test hand-off (every wave)"
```

## Task

Do wave P4a of "Phase P4": the June-only files whose kom is data001-075 (`june2017_re/p0_waves.tsv`).
- **Gate (b) matters here:** every kom is a repack of a live archive. A `~changed` entry outside the wave that is not decode-equal
  stops that kom.
- **data036:** if it is in this wave, first check STATUS and `git status` for a peer holding uncommitted `data036_kom` changes.

Before any copy, ask once with AskUserQuestion, listing every kom in the wave (Rules 3).

Run the four gates in order:
1. `verify-staged`;
2. `pack --wave`: `-0 removed`, and every `~changed` is in the wave or decode-equal;
3. `dupcheck` over the would-be-live set;
4. `copy --wave`, then check md5.

The hand-off is boot and walk, dev console first. The log check is the plan doc's "Play-test hand-off (every wave)".
