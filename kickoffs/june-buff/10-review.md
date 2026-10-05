# June buff port - Phase 10 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 10 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 10 done (its Done note and commit, see STATUS.md) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_GROUP`, `_REACT_APPLY`, `_BINDING`, `_FACTOR_ID`, `_DAMAGEDATA_KEY` (+ `_MEMO_DROP`, off; `JUNE_BUFF_DEBUG`) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 10 -- Buff groups" "Phase 10 -- Done"
```

The "Buff Phase 10" rows of `june2017_re/funcmap.tsv` hold the RE (23 rows). `JUNE_BUGS.md` has
Phase 10's rows (group replaces the base check, the unchecked templet, `_ULTIMATE_PVP`, the two rolls).

## Task

Review the Phase 10 commit against June. Check these first:

1. **`326c31c0` / `327bdb60`:** the default-value switch (0/1 yes, 2 buff only, 3 debuff only, 4 and
   above no), the map value for a listed BTI, "no group with this id = yes", and that the group's
   answer replaces the base one (`MOV BL,AL @327bdbe1`). Confirm templet `+0x64` is `IsDeBuff`.
2. **The six slot-132 callers** that pass a BTI (`322a6960`, `322a6b60`, `322a7660`, `3229af40`,
   `3229b0a0`, `322a88c0`) against the March sites; `3229b0a0` has no March counterpart (say whether
   one exists). The class-file callers pass 0 in June (`320284c0`, `32028790`, `329b4b40`,
   `329b7390`, `32b39e90`, `32c156c0`).
3. **`322a88c0`'s order** and `322a7960` (slot 139): the two rolls, the RESIST text, the 265/266/347/348
   cases, and that the attacker's slot 141 is empty for every March class.
4. **`ParsingBuffFactorID` (`32630d20`):** the order (learned, `_ULTIMATE_PVP`/`_ULTIMATE`, `_PVP`/plain,
   level), `31fceab0`'s two exits (the `SPRT_HYPER_CLASS` one cannot fire here), and where March's
   memo branch sits (kept while `_MEMO_DROP` is off; the user's decision is in the Done note).
5. **The loader and the two Lua adders** (`326c26f0`, `326c3450`, `326bfd80`): presets, the missing
   `EndTable`, the set cleared per call, and the checksum tail being anti-tamper, not a sort.
6. **`EraseDebuffAndAddMP` (`3204d440`)** walks the unit's buffs and erases every member of the set;
   Ara's `32bd32f0` tail (skill 27415) went to the character checklist.

Hygiene is one command per flag, not a fresh script (run each with
`--with JUNE_BUFF_GROUP JUNE_BUFF_REACT_APPLY JUNE_BUFF_BINDING JUNE_BUFF_FACTOR_ID JUNE_BUFF_DAMAGEDATA_KEY JUNE_BUFF_DEBUG JUNE_BUFF_MEMO_DROP`
and `--range <Phase 10 commit>^..HEAD`), for `JUNE_BUFF_GROUP`, `_REACT_APPLY`, `_BINDING`,
`_FACTOR_ID` and `_DAMAGEDATA_KEY`. Paste their `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 10 -- Review" into the plan doc, update the 10 review row
in STATUS.md, check `kickoffs/june-buff/10b.md`, and commit.
