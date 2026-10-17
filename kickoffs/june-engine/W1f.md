# June engine remainder - Phase W1f - CanDown's June signature (DROPPABLE_MONSTER_ROAD, separate out line index) and its callers

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then type `/x2-phase june-engine W1f` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | W1e (done: the link gates in `CanDown`). Not alongside W2 or another session editing `KTDGLineMap.*` |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` (proposed) |
| Log tag | `[JUNE_ENGINE] WORLD` |
| Review | `kickoffs/june-engine/W1f-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1c -- Done" "Phase W1e -- Done" "Phase W1f - CanDown's June signature and its callers"
grep -a "(W1e)" june2017_re/funcmap.tsv
```

and `june2017_re/w1c_land_position_callers.tsv` (the shape of a caller table).

## Task

Do "Phase W1f". W1e ported June's `CanDown` (`31c6c310`, `ret 0x2c`) body except what needs its signature:

1. **`DROPPABLE_MONSTER_ROAD`.** June's `AddLine_LUA` (`31c705c0 @31c716be`) reads it into `LineData +0xa4` (bool, default false; the key is
   XOR-encrypted, decoded in W1e). `CanDown`'s `LT_MONSTER_ROAD` case (`@31c6c491-31c6c4ae`) tests the line when `bIsMonster`, or when `+0xa4`
   and the 12th argument (`[EBP+0x30]`) are both set. March parses neither. June data: only the five `2017_Valentine_Stage*_Linemap` files
   (`june_src_fixed/data040_kom/`); say whether March ships them.
2. **The out line index.** June reads the entry index from `param_7` (`[EBP+0x1c]`: the `bOnLine` head and the no-hit fallback) and writes the result
   to `param_8` (`[EBP+0x20]`, `@31c6f86c`); March has one `int* pLineIndex` for both. A June caller that passes NULL or another pointer for `param_7`
   skips the head and the fallback.
3. **The callers.** 342 June call sites (`xrefs_to 0x31c6c310`). Read each site's pushes by script (12 dwords: 3 pos, radius, out pos, in index,
   out index, then `bDownRadius`, `bIsMonster`, `bOnLine`, `bDroppable`) and table them like W1c's (`june2017_re/w1f_candown_callers.tsv`). Only
   the sites that differ from March's call shape matter: `param_12 != 0`, or `param_7 != param_8`. The user line helper `32010440 @3201104a`
   passes the same pointer twice and four zeros (March's shape). Map those sites to March and port them; route class sites to the checklist.

Port as W1c did: June's full signature, March's 7-arg form an inline forwarder that keeps March's behaviour (`pLineIndex` twice, `false`).
Gate: one `[JUNE_ENGINE] WORLD droppable` line the first time a non-monster lands on a droppable monster road (only if any March map has one).
