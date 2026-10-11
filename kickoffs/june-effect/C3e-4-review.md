# June effect completion - C3e-4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3e-4 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3e-4 done (its Done note and commit) and its play-test log read. **Not with** C3e-5, the C3f-1 / C3v reviews (X2GUUser.cpp, Always.h) |
| Flag | `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS` (with `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS_DEBUG` for the log lines; turn the DEBUG flag OFF here) |
| Log tag | `JUNE_FX C3e-4` (`python tools/logs.py --grep "JUNE_FX C3e-4"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3e-4 - Done (2026-10-12, Opus 5.5 high)"
grep -n "(C3e-3)\|(C3e-4)" june2017_re/funcmap.tsv
cat june2017_re/c3e3_stop_time_callers.tsv
grep -n "^| 18[3-9] \|^| 190 " JUNE_BUGS.md
```

## Task

Review C3e-4's commit against the June binary (Ghidra read-only).

1. **User setters `31ffe4f0` / `31ffe750`.** The early return is `IsMyUnit && +0x7dc4 && !bForce` (Stop2: no force term). The UID is -1 unless
   `+0x264 == GUT_USER` (0). The entry is pushed, fStopTime is not written, and the linemap max is kept. Check that arg 2 is truly unread.
2. **The tick** (`3206eaf0 @3206fa63-3206fd7c`). Check the trim threshold and keep count, the clamp, that an entry at 0 is not removed, and the
   A-erase condition. The empty-by-erase path sets 0; afterwards `back()` is used, or the countdown when the list is empty. The gate is
   `0 < a || 0 < b` with no decrement. March `JuneTickHitStopList` runs before the gate in the IsLocalUnit block.
3. **CommonStateStart `@32060990-32060b96`.** The key is `TIME_STOP_IMMUNE_STATE` (decode xor string `3379d464`), the miss value is false,
   and the store happens only when changed. March stores it in a standalone `KProtectedType<bool>` instead of the Future StateParam tail
   (`+0x7dc4`). Nothing copies that slot after init; confirm nothing else in June reads `+0x7dc4` / `+0x8064`.
4. **NPC `32772aa0` / `32772c10`.** The key choice `bDamageDataStop ? +0x8e82 : +0x8e81`. The `IMMUNITY_DAMAGE_DATA_STOP_TIME` read sits
   after `IMMUNITY_TIME_STOP` in `327a0fa0`, and the ctor clears it.
5. **Callers.**
   - Rows 1, 2, 6, 9 and 13 pass true.
   - Rows 16 and 17 pass the attacker: StopAll's self call gets NULL, the others get `pMyUnit`.
   - Every other engine row relies on the defaults `( false, NULL, false )`. Re-check the list against the tsv.
   - Grep for any new caller since.
6. **A.**
   - Bit `1 << 22`, 1 byte, serialized right after stateChangeNum (`32d5be30` / `32d5c2e0`), in `All`.
   - SyncData member + Init, SendFrameData copy, SetData.
   - The writer is only in `StateChange( int, bool )` (`320348c0 @32034b37`; `32034c00` has none: JUNE_BUGS 188), after DoFrameMoveFuture and
     before the push. The clear is in DoStateEndStartFuture `@31fe6aab`.
7. **The play-test log.**
   - `applied` lines with `dmgData 1` for the IMMUNITY_TIME_STOP-only boss.
   - `blocked by IMMUNITY_DAMAGE_DATA_STOP_TIME` for the both-keys monster.
   - user `pushed` lines, and no `refused` line (no March class state has the key).
   - No `user tick` line (the A erase cannot fire offline).
   - No EXCEPTION.
8. **Hygiene.** `python tools/flagcheck.py JUNE_HIT_STOP_LISTS --with JUNE_HIT_STOP_LISTS_DEBUG --range <first commit>^..HEAD`. Then turn
   `SERV_IRUHADEV_JUNE_HIT_STOP_LISTS_DEBUG` off in `KTDXLIB/Always.h` (comment the `#define`), and `tools/build.py --deploy`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"C3e-4 - Review" under the phase in the plan, update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
