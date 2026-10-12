# June effect completion - C3e-5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3e-5 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3e-5 done (its Done note and commit) and its play-test log read. **Not with** the C3f port or the C3g review (X2GUUser.cpp / X2GUNPC.cpp DamageReact) |
| Flag | `SERV_IRUHADEV_JUNE_SUPER_ARMOR_HIT_STOP` and `SERV_IRUHADEV_JUNE_DAMAGE_REACT_ORDER` (with `SERV_IRUHADEV_JUNE_SUPER_ARMOR_HIT_STOP_DEBUG` for the log lines; turn the DEBUG flag OFF here) |
| Log tag | `JUNE_FX C3e-5` (`python tools/logs.py --grep "JUNE_FX C3e-5"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3e-5 - Done (2026-10-12, Opus 5.5 high)"
grep -n "(C3e-5)" june2017_re/funcmap.tsv
grep -n "^| 191 " JUNE_BUGS.md
```

## Task

Review C3e-5's commit against the June binary (Ghidra read-only; `june2017_re/disasm_range.py --x87` for the uncarved ranges).

1. **The hit-stop.** User `320462a0 @32046cbd-32046d84`, NPC `327b1f30 @327b314f-327b3215`.
   - Re-derive each x87 test: fStopTimeDef `> 0` (`test ah,5` / `jp`) and cooldown `<= 0` (`test ah,1` / `jne` to the full path).
   - Confirm JUNE_BUGS 191 (full stop inside the cooldown) from the bytes, not from the Done note.
   - Check the NPC has no fStopTimeDef test and clears `m_fCoolTime` (`+0x864c`) on every path, including the table-skip one.
2. **Tables** `3204a7bc` / `3204a7cc` and `327b536c` / `327b537c`. Re-read all 18 entries and the three floats; check `JuneDamageReactHitStop`'s
   cases against them.
3. **Cooldown** `+0x68d0`: the ctor 0, the tick's place (after CheckAndUpdateAdditionalStat) and its clamp. Confirm the ported tick uses the
   OnFrameMove argument the way June does (`[esp+0x140]`).
4. **Order** (`SERV_IRUHADEV_JUNE_DAMAGE_REACT_ORDER`).
   - Walk June `@32046783-32046fb9` block by block and check the port's flag-on order against it: front test, Future copy, ADD_BACKATTACK
     (whose place in June was not found; check it), revenge detection, revenge block, case, drain, last attacker, Absorb, back speed.
   - Check the copies (case, drain, back speed) against March's originals line for line.
   - Check the UT_ARA test (`31e4d0e0`).
5. **Hygiene:** `python tools/flagcheck.py JUNE_SUPER_ARMOR_HIT_STOP --with JUNE_DAMAGE_REACT_ORDER JUNE_SUPER_ARMOR_HIT_STOP_DEBUG --range <C3e-5 commit>^..HEAD`.
   Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Fix clear transcription errors under the same flags, turn the DEBUG flag off, build and deploy (`tools/build.py --deploy`). Write
"C3e-5 - Review" into the plan doc, update the STATUS rows and commit.
