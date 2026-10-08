# June asset port - Phase P1 review

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-asset-port P1 review` |
| Plan doc | `JUNE_ASSET_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase P1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_KOM_MOUNT_DEBUG` |
| Log tag | `[KOM_MOUNT]` |

## Read first

```sh
python tools/docsection.py JUNE_ASSET_PORT_PHASES.md "Rules" "Phase P1 - Mount diagnostic and baseline" "Phase P1 - Done"
```

## Task

Review P1's commit(s). Check that:
- the counts are taken where `m_mapMassFile.insert` really succeeds, including the `SERV_IRUHADEV_KOM_LONG_NAMES` branch;
- the per-kom store has no per-frame cost and logs exactly once;
- `GetProcessMemoryInfo` links (psapi) in `US_SERVICE`;
- a kom that `AddMassFile` deletes (the duplicate path, `:576`) is visibly absent rather than logged as mounted.

**Hygiene** is one command: `python tools/flagcheck.py SERV_IRUHADEV_KOM_MOUNT_DEBUG --range <first P1 commit>^..HEAD`. Paste its
`files:` / `RESULT:` lines, and explain every OTHER row and every deleted line.

**Then:**
- fix clear defects under the same flag, then `tools/build.py --deploy`;
- write "Phase P1 - Review" into the plan doc;
- update the STATUS row;
- check `P2.md`;
- commit.
