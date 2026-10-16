# June effect completion - C3x-1 review - June's Festival / CodeEvent world variants

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3x-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3x-1 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_FESTIVAL` |
| Log tag | `JUNE_FX C3x-1` (`python tools/logs.py --grep "JUNE_FX C3x-1"`; JUNE_FX lines are unbracketed) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3x-1 - Done"
grep -n "(C3x-1)" june2017_re/funcmap.tsv
grep -n "^| 198 " JUNE_BUGS.md
```

## Task

Review C3x-1's commit against June's `AddWorldData_LUA` (`32805770`) and `CreateWorld` (`32803d10`, plus the main-world pick of
`32803f60`). The things this phase could get wrong:

- **The festival cutoff and its direction.** Re-read @32805b98-32805bc2 (the third `tm`: sec, min, hour, mday, mon, year, isdst)
  and @32805be2-32805bf2 (`JG` / `JL` / `JNC`). The port swaps only while now < 2016-03-16 04:00 local. If the branch is the other
  way round, every village would load its `_Festival` map today.
- **What is swapped.** @32805aeb copies `+0x20` to `+0x3c` before the gate; @32805d87 / @32805dc1 read the festival name into a temp
  and assign it to `+0x20` only when it is not empty. The port's `dataFileName_Original` / `dataFileName` must match.
- **The code-event pick.** Key 0x2e (46), node byte `+0x10` == 1, `+0x68` not empty, and a NULL `g_pInstanceData` means plain
  (@32803e63-32803f13). March's `IsEnableCode` returns the node's bool; confirm that is the same test.
- **Coverage.** Every June path that opens a world script from WorldData: `32803d10`, `32803f60` (main world picks, additional
  worlds plain), `32804810` (uncalled). March has one `CreateWorld` for the first two; the additional worlds belong to Elysion.
- **Assets.** 30 variant scripts in data040 and 42 of the 43 assets they need (data023 / 025 / 026) were staged.
  `Village_ADBoard.dds` (June data216, a new kom number) was left to the asset port. Check `june2017_re/c3x1_world_asset_need.tsv`
  against the kom dirs, and spot-check one staged `.Y` (inflate with `fxdata.xfile_text`, its textures resolve).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_WORLD_FESTIVAL --range <C3x-1 commit>^..<C3x-1 commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "C3x-1 - Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.
