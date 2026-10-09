# June effect completion - C3e-2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3e-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3e-2 done (its Done note and commit) and its play-test log read |
| Flag | `SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE` (with `SERV_IRUHADEV_JUNE_FX_DEBUG` for the log lines) |
| Log tag | `JUNE_FX C3e-2` (`python tools/logs.py --grep "JUNE_FX C3e-2"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3e-2 - Done (2026-10-09, Sonnet 5.5 high)"
grep -n "(C3e-2)" june2017_re/funcmap.tsv
awk -F'\t' '$1=="N"' june2017_re/c3e_user_sync_fields.tsv
grep -n "^| 151 \|^| 152 " JUNE_BUGS.md
```

## Task

Review C3e-2's commit against the June binary (Ghidra read-only).

1. **The roll `31fe22d0`.** RET 0xc with three OUT strings; the index counter advances for every entry walked (a miss stores the size);
   map keys 9 / 0xb / 0xc; `+0x8abc` cleared after each store. Check the class Init call sites (e.g. Eve `329baafb`) take the strings and bind
   START / WIN exactly once afterwards, so no class file needed an edit.
2. **StateChange re-bind `31fea640`.** Only a map hit writes Future N (and only if it differs); a miss leaves it alone. The only caller is
   `320348c0 @320348dc`, right after the riding-pet reset (xrefs_to 31fea640). Look for a second StateChange variant or a vtable user.
3. **DoStateEndStart re-bind.** `3203c449-3203c531`: Now nowState `& 0xffff` against 9 / 0xb / 0xc, Now N, before the state-data lookup at `3203c536`.
   Confirm March's insertion (before `m_NowStateData = m_StateList[...]`) and that `DoStateEndStartFuture` has no counterpart in June.
4. **`31fd65f0`.** LOSE's OnEventProcess = `320718d0` only while `+0x8abc` is set, else cleared (March LOSE registers none: grep). The protect-registry
   re-registration (`31c594b0`, `+0x7cfc == 1`) is not ported: confirm it is a June-only protection list and nothing reads the rebound entry through it.
5. **N on the wire.** Bit `1 << 30` last after BuffSyncCount (`0x40000000`, 1 byte): Serialize / Deserialize order against `32d5be30` / `32d5c2e0`,
   `All` includes it, default 0, sender copy, SetData, Future -> Now through the whole-struct copy.
6. **The play-test log.** The roll line per class (list size, index, table), then each re-bind line with the same index and table; count lists whose
   percentages sum below 100 (JUNE_BUGS 151) and any empty list (152); no EXCEPTION.
7. **Routing.** Stage 21 (Photozone) row in `JUNE_PORT_BACKLOG_ORDER.md` and the unported WIN OnEventProcess (`320718d0`, C9h-4g) are correct.
8. **Hygiene.** `python tools/flagcheck.py JUNE_COMMON_RANDOM_STATE --range <first commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"C3e-2 - Review" under the phase in the plan, update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
