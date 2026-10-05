# June buff port - Phase 11b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 11b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 11b done (its Done note and commit) |
| Flags | `SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF` (commented out: user, 2026-10-05), `_REPAIR_DISCOUNT`, `_DURATION_TIME` |
| Log tag | `[JUNE_BUFF]` (no new lines in 11b; the emulator's `BUFF     activated buff` lines mark the test buffs) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 11 — RE (2026-10-05)" "Phase 11b — Done (2026-10-05)"
```

The "Buff Phase 11b" rows of `june2017_re/funcmap.tsv` name every June function; JUNE_BUGS #39, #40.

## Task

Review 11b's commit against June (Ghidra, read-only; read the assembly, not the decompile):

1. **Clear-all `3229fb10`.** The bool's two effects (gage save only when set; flagged templets
   skip the finish calls) and that `322ae690` is a move helper, so the vector is emptied whole
   (JUNE_BUGS #39). The nine callers' arguments and their March homes: `323080c0` = `~CX2Game`,
   CX2Game vtable 3348bf94 slots 55-59 = `DeleteUserUnitByUID/ByData/DeleteNPCUnit/ByUID/DeleteAllNPCUnit`.
2. **The open point: June's base `DyingStart`.** March clears all buffs there (`X2GameUnit.cpp`
   `CX2GameUnit::DyingStart`); 11b could not locate June's (the user vtable slot order around
   `0x7bc`-`0x7c4` does not match the header note at plan line ~1290). Find it (e.g. from an NPC
   or user `DieFront` state start's call), say whether June still clears there and with which
   argument, and fix the port's call under `_STORE_BUFF` if it differs. The flag is off, so the
   fix is dormant.
   Decide the Ranox call site together with it: `CX2GameUnit::EraseAllBuffAndExtraDamage`
   (`SERV_IRUHADEV_RANOX_ERASE_BUFF`, ours, not June's) calls `FinishAndClearAllBuff()`, so with the
   flag on it also stops saving to the gage. Phase 13 turns the flag on only after this review.
3. **Gage save `32494560`** and its predicate `32499c10`: the whole-vector filter, the GT_PVE
   gate (no March type), and that the only caller passes 0 (so `3229ff80` is unreachable).
4. **Repair discount `32363f00`**: the six BTI deltas (jump table `32364480`), 876's set-to-1.0
   left inert (no March event map; JUNE_BUGS #40), no clamp, and the ten callers' `>= 0`.
5. **`SetDurationTime` `32e608f0`**: the record lookup (`32e5cb30`, flag `+0x20`), the end-time
   arithmetic on low dwords, the two colours (UIStringData `color` `+0x2c`), minutes vs seconds,
   and that "0 or below" leaves the text alone.
6. **Recorded, not ported**: slot 250 (`3201ffb0`, the Crest of Time snapshot/restore and BTI
   807), slot 251 (`3204dcd0`, Luciel), `32163070` (item 100005712). Check that no March home
   was missed (in particular for BTI 807: `g_pData+0x6c` = the PVP room, type 8).

Hygiene, one command per flag (`_STORE_BUFF` needs its `#define` uncommented for the run, then
put back, because the tool requires a define):
`python tools/flagcheck.py <FLAG> --range <11b commit>^..<11b commit> --with JUNE_BUFF_STORE_BUFF JUNE_BUFF_REPAIR_DISCOUNT JUNE_BUFF_DURATION_TIME`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 11b — Review" into the plan doc, update the STATUS.md
row, and commit.
