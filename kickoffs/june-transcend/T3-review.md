# June transcendence - Phase T3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T3 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T3 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_ITEM` |
| Log tag | `[JUNE_TRANSCEND] ITEM` (`python tools/logs.py --grep "JUNE_TRANSCEND ITEM"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T3 - The transcendence items" "T3 - Done"
```

T0's map: `june2017_re/transcend/t0_readers.tsv` (owner T3 rows) and funcmap rows `Transcend T0` / `Transcend T3`.

## Task

Review T3's commit against June in Ghidra (read-only):
- **The map.** `3280a090` call by call against `CX2ItemManager::IsTranscendentalUnitType` (class 0 / empty set -> true, the three code-event clauses and their
  if / else-if shape, the final set lookup), `328189d0` / `3280b6c0` / `32807fe0` (insert-if-absent, getter -1), and the load position in `ResetItemManager`.
- **The right-click.** `321aa5d0 @321bf5ef-321bf88c` against the `case 288493` in `X2UIInventory.cpp`: the order of the three tests and the three NetError ids
  (1726 / 1727 / 1675 -> JUNE_BUGS 414). Confirm that the 163200 / 163316 / 163323 / 163325 / 161854 cases have no March counterpart (cash-shop claim only) and that
  the routing to Stage 9 (`JUNE_PORT_BACKLOG_ORDER.md`, "Transcendence in the June UI") is right.
- **NetError.** The six texts in `X2NetError.cpp` against June's table (`337379b8`; the strings are at 0x1950398 .. 0x19508e0 of `unpacked_x2.exe`).
- **Emulator** (stand-ins, labelled in `MODS.md`): the book (use from the bag and claim from the shop), type 0 / type 1 refusals in the claim path, the
  transcendence class tickets 288323-288325 (class from item.kim; same class 1676, another character 1675, off the allow-list through `JuneTranscendCheck`),
  the medal's untranscended refusal now 1686, the three new shop products.

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_ITEM --range <T3 commit>^..<T3 commit>`; paste its `files:` / `in this flag's files:` / `RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T3 - Review" into the plan
doc, update the T3 row of `STATUS.md`, check that `kickoffs/june-transcend/T4-review.md` / `T5.md` still read right, and commit.
