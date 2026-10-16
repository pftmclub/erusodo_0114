# June effect completion - C9b - identify the unit-viewer effect-set consumer (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C9 (done 2026-10-07; it left this half unported) |
| Flag | none (no code) |
| Log tag | `[JUNE_FX]` |
| Review | none |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9 - Done"
```

Also read: `june2017_re/funcmap.tsv` rows `3237a1f0`, `323760f0`, `31ee6100`, `31fa3010` (tagged `Effect C2` / `Effect C9`).

## Task

`UnitViewerEffectSetTable.lua` has 18 `g_pUnitManager:AddUnitViewerEffectSet( motion, UNIT_TYPE, UNIT_CLASS, effect set, float, bool, DUO_TYPE )`
calls. June's bind (`3237a1f0`), its store (unit-manager map `+0xf8`) and its lookup (`323760f0`) are confirmed; C9 did not port them
because the consumer is unidentified and the bulk loader's address in March is unconfirmed.

Identify, in Ghidra, and write funcmap rows (`Effect C9b`):
1. The bulk loader `32597410` (calls `31ee6100` @`3259a982`): what it is, and which March function is its counterpart (where March should open the file).
2. `31fa3010` (20+ call sites from `323fad80`): which class it belongs to (the unit viewer / lobby unit?), what its args mean (`param_5` 0/1/2),
   what the 0x30-byte request it pushes (`31fad720` = push_back) contains, and **what drains that vector** (the function that plays the effect set).
3. `31fa8ce0` (from `31faa550`) and `32043380` (many callers): whether they are the same pattern in other classes.
4. Whether March's `CX2UnitViewerUI::PlayAnim` (`X2Lib/X2UnitViewerUI.h:66`) can take the same hook, and which March effect-set manager plays it.

End with a port list for a Sonnet phase (C9c: bind `AddUnitViewerEffectSet`, store, lookup, load, consumer, stage and pack the file),
or a reason to decline. Do not write game code. `DUO_TYPE`, `UNIT_TYPE.UT_NONE` and `UNIT_CLASS.UC_NONE` already exist in March's `Enum.lua`.
