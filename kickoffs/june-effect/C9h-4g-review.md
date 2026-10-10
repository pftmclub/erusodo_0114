# June effect completion - C9h-4g review - USEFUL_STRING, the NOT_SELECT keys, the reserved state ids, USI_WAIT_STATE_CHANGE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4g review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4g done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_USEFUL_STRING`, `_NOT_SELECT_UNIT_OFFSET`, `_RESERVED_STATE_IDS`, `_WAIT_STATE_CHANGE` (+ `SERV_IRUHADEV_JUNE_C9H4G_DEBUG`) |
| Log tag | `JUNE_FX C9h-4g` (`python tools/logs.py --grep "JUNE_FX C9h-4g"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4g - Done"
grep -n "(C9h-4g)" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4g code against June in Ghidra (read-only) and fix transcription errors under the same flags. The things it could get wrong:
- **USEFUL_STRING** (`320506f0 @32056b0e-32056bf2`, `32057b80 @32060dda-32060e5e`): the clear, `BeginTable( "USEFUL_STRING", i, true )` from 0, `resize( i + 1 )`,
  value `[1]` as a wide string, a miss = `L""` (`3344f1f4` is an empty string, `31f54a10` = assign), no row cap. The getter `32a78fa0`: `bFuture ? +0x8f40 : +0x8f30`,
  unsigned `size > index`. Confirm `JuneParseUsefulString` / `JuneGetUsefulString` match, and that the vector element is a wide string (stride `0x1c`).
- **NOT_SELECT keys** (`@32056bf2-32056edd`, `@32060e5e-32061149`): value 1 -> `+0x8f54`, 2 -> `+0x8f58`, both zeroed first, bool `+0x8f5c` with a miss = true; re-run the
  positional decrypt of both stack-built key names (`USE_NOT_SELECT_MOTION_DIFFERENCE`, the Now one too) and confirm the two parses write the same members (JUNE_BUGS 155).
- **Reserved ids**: `USI_EVENT_*` 10 before `USI_BOUNDING_*`, `USI_WAIT_STATE_CHANGE` after, in June's order; `june2017_re/enumprobe.py` (USI_END 102, USI_WAIT_STATE_CHANGE 101,
  `CSI_A_CIP_ARTILLERY_NOVA_FIRE` 269, `ASI_DASH_COMBO_X` 135) against data036 `Enum.lua` (`GUSI_END + 83..97`). Confirm nothing in March keys on a literal user state id
  between 88 and 269 that the shift moves (C3d-1's census said none is saved; re-check `X2TrainingGameUI.cpp`'s `(const char)` compares).
- **WAIT_STATE_CHANGE**: `31fc8620` compare direction (state time strictly above Future[0][0], NaN stays) and the `USI_WAIT` target; the delegate `31fcd0b0`'s anim test;
  the registered-state guard is a labelled March-safety deviation, not June.
- **_DEBUG**: turn `SERV_IRUHADEV_JUNE_C9H4G_DEBUG` off once the logs were read (rebuild + deploy; the kom does not change).
- **Hygiene:** `python tools/flagcheck.py <each flag> --with JUNE_C9H4G_DEBUG --range <first C9h-4g commit>^..HEAD`.

Write "C9h-4g - Review" into the plan doc, update the STATUS row, check `kickoffs/june-effect/C9h-4h.md`, and commit with explicit pathspecs.
