# June item.kim adoption - Phase K2 review

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-item-kim K2 review` |
| Plan doc | `JUNE_ITEM_KIM_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | K2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_ITEM_KIM`, `SERV_IRUHADEV_KIM_LAYOUT_CHECK`, `SERV_IRUHADEV_JUNE_KIM_MINIME_REMAP` |
| Log tag | `[JUNE_KIM]` (`python tools/logs.py --tag JUNE_KIM`) |

## Read first

```sh
python tools/docsection.py JUNE_ITEM_KIM_PHASES.md "Rules" "Phase K0 - Done" "Phase K2 - Loader port and data swap" "Phase K2 - Done" "Phase K2 review"
```

Then `june_item_kim/k0_record_map.tsv` (the 228-byte map) and `june_item_kim/k2_assets_staged.tsv` (what D1 staged).

## Task

Review K2's commit against K0's map and the plan's "Phase K2 review" list:

1. **Offsets.** Every `KItemFormatTemplet` member sits at K0's June offset (+44 damage range, +48/+52, +56 name, +68 attach[8],
   +100/+104 dates, +108 model[8], +140 the six strings, +164 the nine pool offsets, +200 the shorts, +208 the bytes, +224 desc-in-shop
   type, 228 total). Prove it with an `offsetof` probe (`#pragma message` / `cl /P`), not by reading. Also `KItemFormatStatData` (28 B,
   Def at [5]/[6]) and `KItemFormatStatRelLVData` (6 B).
2. **Flag hygiene.** `python tools/flagcheck.py <FLAG> --range <K2 commit>^..HEAD` for all three flags. Paste the `files:` /
   `in this flag's files:` / `RESULT:` lines. Explain every OTHER row; the only added `#else` code that is not March's is the
   `== 188` static assert (the plan asks for it).
3. **Layout check.** It refuses a March kim under a flag-on build and a June kim under a flag-off build. Prove both by reading the code
   path (`IruhaKimFileStride` samples index 1, every 997th and the last; the smallest matching stride wins) and the log line; you can
   also run the stride logic in Python over both kims (`march_data\data036\item.kim` and the staged June one).
4. **Kom diff.** For `data036` the packed kom differs from the pre-K2 live kom (backup in `kom_backup_tools/`) by `item.kim` only;
   for the 25 asset koms only by the `k2_assets_staged.tsv` names (added, algo 0). Same md5-per-entry method as buff Phase 13 review.
5. **Ghidra spot-checks (read-only).** The Extra key remap (`32867660`), the grade-6 colour (`32867e80`), and each grade-6 / item
   type 14 site K2 extended against the June address its Done note cites. If one fails, re-check them all.
6. **Guards.** Each guard in the Done note skips (never clamps) and logs once per kind with a count (`JuneKimGuard`: hits 1, 10, 100...).

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "Phase K2 - Review" into the plan doc, update the K2 review row in STATUS.md, check `kickoffs/june-item-kim/K3.md`, and commit.
