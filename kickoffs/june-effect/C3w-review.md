# June effect completion - C3w review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3w review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3w done (its Done note and commit) and its play-test log read |
| Flag | `SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START` (with `SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START_DEBUG` for the log lines; turn the DEBUG flag OFF here) |
| Log tag | `JUNE_FX C3w` (`python tools/logs.py --grep "JUNE_FX C3w"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3w - Done (2026-10-14, Opus 5.5 high)"
grep -n "(C3w)" june2017_re/funcmap.tsv
```

## Task

Review C3w's commit against the June binary (Ghidra read-only; `june2017_re/funcdis.py --func <va>`, `disasm_range.py --x87`).

1. **`327ef160`, the gate.** Re-derive both x87 tests (`@327ef1c0` entry, `@327ef2ef` delay) and the `31c40570` compare at `@327ef30e`
   (is it really "string empty"?). Check the port's flag-on order: intro condition, forced-state skip (no sub-boss test), particles,
   the name with the counter `<= 7`, `m_bBossNameWasShown`.
2. **Forced state.** Confirm `@327ef326-327ef355` (now-state decode of `+0x8454` vs `GetStateID`) is what C3s's
   `GetStateID( name ) == GetGameUnitState()` reads, as ported.
3. **`327cc7c0`.** The 0..7 bounds, the sequence name (`0x334badf0` + the `31ecbf60` format string `0x33458d18`: confirm decimal), the
   `CreateSequence` arguments against March's `D3DXVECTOR3` overload defaults, `ChangeTexForce`, the slot store.
4. **Slots.** The ctor (`327cef70`, all 8 = -1) and `Release` (`327d2240 @327d2656`). Look for any other June reader of `+0xcf8..+0xd14`
   in CX2DungeonGame that the port missed (`dispsweep.py cf8 cfc d00 d04 d08 d0c d10 d14`, dungeon-game functions only).
5. **Data.** `GameMinorParticle.txt` in `kom_contents/data036_kom` holds June's `BOSS_NAME0..7` byte-exact (compare with
   `237311/14JUN17/data/data036_kom/GameMinorParticle.txt`) and nothing else changed (`24965799/kom_backup_c3w/`).
6. **Hygiene:** `python tools/flagcheck.py SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START --with SERV_IRUHADEV_JUNE_SHOW_BOSS_NAME_AT_START_DEBUG --range <C3w commit>^..HEAD`.
   Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Fix clear transcription errors under the same flag, turn the DEBUG flag off, build and deploy (`tools/build.py --deploy`). Write
"C3w - Review" into the plan doc, update the STATUS rows and commit.
