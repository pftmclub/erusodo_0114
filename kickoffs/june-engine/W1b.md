# June engine remainder - Phase W1b - AUTO_DISABLE_LINE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1b` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W0 (done 2026-10-16); june-effect C12 (code gate). Not alongside W1, W2 or atlas E5 (same files) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE` |
| Log tag | `[JUNE_ENGINE] WORLD` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/W1b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W0 -- Done" "Phase W1b - AUTO_DISABLE_LINE"
grep -a "Engine W0" june2017_re/funcmap.tsv     # rows 328004a0 (W0), 31c6b8e0, 32063650 @3206bc6c, 32015b30 @320195be, 3277fbf0 @32782d70
```

March code: `KTDXLIB/KTDGLineMap.h/.cpp` (`LineData`, its init / copy, `AddTrigger_LUA`), `X2Lib/X2World.cpp` (`DoTriggerLine`),
`X2Lib/X2GUUser.cpp` (`CommonFrameMove`, `PhysicProcess` near `:24172`), `X2Lib/X2GUNPC.cpp` (`PhysicProcess`), the game-unit base.

## Task

**First map, then port.** Two funcmap rows are `candidate`:
- the user's nearest-line fallback `32015b30 @320195be-32019a10`: how the second candidate (`esp+0x7c`, positions `esp+0x98..0xa0`)
  is chosen inside the enabled-line loop, and what `@3201996d-320199dc` does with it;
- the NPC fallback `3277fbf0 @32782deb-32782ea4` (line from `+0x6860`, the `+0x85fc` / `+0x843c` speed reset).

Write both as pseudocode with addresses in the funcmap rows (raise them to `confirmed`), and set them against March's code
(`X2GUUser.cpp:24172`, `X2GUNPC.cpp` `PhysicProcess`).

Then port June's whole mechanism:
- `LineData` `+0x128` flag, `+0x12c` user UIDs (`__int64`) and `+0x13c` NPC UIDs (`int`), with their init, copy and clear (June
  `31c7da50`, `31c7e4b0`, `31c7d800`);
- `AUTO_DISABLE_LINE` read for every line row (`31c77e20` @31c790a3);
- the `DoTriggerLine` tail (`328005b9-328007c0`): who counts as "on this line" (`31c7bb00`'s arguments);
- the release helper `31c6b8e0`;
- the game-unit `+0x3e48` (init -1), the user `CommonFrameMove` bookkeeping (`@3206bc6c-3206bd03`) and the NPC one (`@32782ea6-32782ede`,
  return value ignored: June quirk, keep it with a `JUNE_BUGS.md` row);
- both physics gates.

**Done gate:** build rc=0 and DEPLOY OK. A `[JUNE_ENGINE] WORLD auto-disable` line per marked line and per release. Play-test:
`Ranox_FireTemple_StageBoss`, lines 3 and 4. The boss fires the trigger. Units standing there keep standing, nobody else can land
after they leave. The AI-party bots count as NPCs and go through the `+0x13c` list.
