# June engine remainder - Phase W1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_MESH_ID`, `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_EFFECTSET`, `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_SOUND` |
| Log tag | `[JUNE_ENGINE] WORLD` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W0 -- Done" "Phase W1 - World-event timeline port" "Phase W1 -- Done"
grep -a "Engine W0" june2017_re/funcmap.tsv
```

## Task

Review Phase W1's commit(s) against W0's funcmap rows and the June addresses they cite. Check:
- the `GetTriggerMesh` id filter (`327fc680`) and that `DoTriggerFrameMove` passes the segment id;
- the case-7 split (first `L" "`) and the token order (mesh, then effect set);
- `PlayEffectSet`'s argument list against `32801300-32801466`: scale, power / lifetime, custom-position flag, position source,
  rotation (`GetRotateDegree`), direction (0,0,0);
- the sound auto-release registration (`32800e82-32800edb`, `31cdd4e0`);
- the `MOTIONNAME_EFFECTSETNAME` key name;
- that every `#else` path is March's / Ranox's code unchanged.

Hygiene is one command per flag: `python tools/flagcheck.py <flag> --range <first phase commit>^..HEAD`. Paste its `files:` /
`RESULT:` lines into the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase W1 -- Review" into the plan doc, update the
STATUS row, check W1b's kickoff, and commit.
