# Atlas/Elysion port - Phase A0 review (census method)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A0 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A0 done (its Done note and commit) |
| Flag | none (tooling; no game code, so no flagcheck, no build) |
| Log tag | none |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase A0" "The exit metric"
```

Then run `python june2017_re/region_census.py --limit 0 --json <scratch>.json` (about 1 minute) and read `june2017_re/region_census_baseline.tsv`.

## Task

Review the **method** of `june2017_re/region_census.py` v1, not the game. Spot-check at least 3 items each way, directly in the June source (`june_src_fixed/`) or the staged files:

- **False-positive direction** (a row says OPEN but is fine): one `ids.missing` row, one `bind.unbound` row, one `koms.unstaged` row.
- **False-negative direction** (something the census should flag and does not): pick a region script or NUI by hand and check that it is in the file set / NUI closure; one `TABLE.MEMBER` from a region dungeon script; one `STR_ID` or asset a region unit script names.

Also judge the three ledger corrections (WI 41 -> 40, NUI 203 -> 201, effect-set premise false) and the decision to count "region set" and "missing now" separately. Known limits to confirm rather than fix: bindings are name matches (receiver class is A1b's), bytecode-only files (`ELYSION_SOLESS`, `Elysion_Tower_Stage3`) are scanned as strings, `--phase` is a line + check-prefix filter, mesh-internal textures are not followed.

Fix clear census bugs in place and re-run; hand design decisions to the user. Write "Phase A0 -- Review" into the plan doc (under the Done note), update the A0 row in `STATUS.md`, and commit with explicit pathspecs. The next phases (A1a, A1b, can run at the same time) have kickoffs already.
