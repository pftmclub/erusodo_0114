# June effect completion - C9h-7a review - June's sound-device auto-release

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-7a review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-7a done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE` (+ `_DEBUG`, turn it off in the review if the play-test is clean) |
| Log tag | `JUNE_FX SOUND_RELEASE` (`python tools/logs.py --grep "JUNE_FX SOUND_RELEASE"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-7a - Done" "C9h-7 - Done"
grep -n "Effect C9h-7" june2017_re/funcmap.tsv
awk -F'\t' '$3 !~ /^1/' june2017_re/c9h7_playsound_census.tsv
```

## Task

Review the C9h-7a code against June in Ghidra (read-only) and fix transcription errors under the same flag. It could get wrong:
- **The callback `31cdcca0`** (no Ghidra function; `python june2017_re/disasm_range.py 31cdcca0 31cdccc2`): `type == 0` test, queue call on `g_pKTDXApp+0x1f4`, `ret 0x10`. The March port is `JuneSoundAutoReleaseCallback` in `KTDXDeviceManager.cpp`.
- **`31cdd8d0` against `JuneCloseDeviceByPointer`**: count <= 0 return, count > 1 one decrement, else unref + find by device ID + pointer compare + delete. Confirm there is no path in June where the count > 1 branch also erases.
- **The drain** `31cde8d0` runs every frame (the `jne @31cde8ea` target is `31cde8fb`, after `ProcessForegroundQueue`'s call).
- **The census** `c9h7_playsound_census.tsv`: re-trace the 4 resolved registers (`3251094c`, `325130c5`, `325131b2` = `ebp` 0; `32e2b608` = `ebx` 1) and the 6 "dropped-handle" mappings (Square MsgProc `324c7ce0`, `327f4210` Go, `3250f680` = DungeonResult OnFrameMove, `3293b5e0`, `32952930`): name each June function's class and check the March site is the same one. The Done note's mapping of `3293b5e0` -> `X2LocalMapUI.cpp` (both sites) and `32952930` -> `X2UIQuickSlot.cpp` (both sites) was made from the sound name and the March context, not from a decompile of those two functions: confirm or correct.
- **Missed March call sites**: a raw sweep of every `->PlaySound(` in `X2Lib`, `X2`, `KTDXLIB` whose receiver is the device manager (not a game unit) and that stores the handle across frames, against the audit `.tsv`.
- **Play-test**: what the log showed (queue / close lines with `deleted=1`, any `ref=` > 1), and whether the Add check crashed (JUNE_BUGS 125: report, never fix).
- **Hygiene:** `python tools/flagcheck.py JUNE_SOUND_AUTO_RELEASE --with JUNE_SOUND_AUTO_RELEASE_DEBUG --range <first C9h-7a commit>^..HEAD`.

Write "C9h-7a - Review" into the plan doc, update the row, check `kickoffs/june-effect/C9h-8.md` (next), commit.
