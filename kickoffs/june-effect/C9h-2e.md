# June effect completion - C9h-2e - June's code-event handler tail `32344880` and state virtual `+0x100` (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2e` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2d (it ported the handler's data copy and left the tail) |
| Flag | none (no code; the port phase this one plans gets its own flag) |
| Log tag | none |
| Review | none (identify only); this phase writes the port kickoff and its STATUS row, or a routing line |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2d - Done"
grep -n "^31f00970\|^32344880" june2017_re/funcmap.tsv
```

March: `X2Lib/X2State.cpp` `Handler_EGS_UPDATE_CODE_EVENT_NOT` (the `SERV_NEW_YEAR_EVENT_2014` / `SERV_EVENT_CHECK_POWER` blocks June dropped).

## Task

June's `Handler_EGS_UPDATE_CODE_EVENT_NOT` (`31f00970`) ends, after copying the packet's map and two sets, with two calls March has no
counterpart for. C9h-2d ported the copy and left both calls:
1. **`32344880`** (`__fastcall`, one caller: `31f00970 @31f00a61`). It runs only when the global at `33775870` is non-null, takes a list
   through `324f1e10` / `324f23a0`, and for each id calls `32340320` (a lookup over a vector at `this+0xac..0xb0` of 0x388-byte rows,
   each gated by a code-event id at row `+0xc` through the instance's map `+0xa18`), then one of `323b7ff0` / `323a9ed0` / `323b8970`,
   with vectors at `this+0xcc` (0x388 rows) / `+0xdc` (0x14 rows).
2. **The state virtual at vtable `+0x100`**, called last on the state.

Identify: what class `33775870` / `33775860` are, what the 0x388 row is and which Lua file fills it, whether March has the class (and
the lists) at all, what the virtual does (name it from the slot in March's `CX2State`), and which part of a code event reaches a March
player. Then write the port kickoff (own flag, own `STATUS.md` row) or a routing line to the backlog.
