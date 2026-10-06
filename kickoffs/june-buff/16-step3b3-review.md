# June buff port - Phase 16 step 3b, part 3 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b3 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b3 done (`fed40d3`, `23ea90a`, `576b635`) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 6b -- Done (2026-10-04)" "Phase 16-step3b3 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b3" carry the decodes.

## Task

Review 16-step3b3's three commits against June:
- **Item 0.** The bool map: every vtable call at `+0x6a0`/`+0x69c` pushes 1, every class override of slot 424 hard-codes 1 into
  `31fd1840`, and Add's `GetActualDPConsume` (`329e78a6`) is the only 0. Check June's order: BBT 99 (`3203a250`) runs before the cut,
  equality takes `1 - r`, the hyper test is `templet +4 == 0xd`, and the BBT 63 rate is multiplied last. Also confirm that
  `GetActualMPConsumeJune` is the only body: the virtual must only forward, and no class override may lose its override (signatures unchanged).
- **Item 1.** The 14 sites plus Nasod Watch:
  - each case's skill matches what Phase 6b passes;
  - Mortar's socket term comes before the `-= 1`;
  - none of the five stat-scale terms survives with the flag on;
  - the null-templet branch takes the all-skill term (`@322ed2b0`);
  - `32759630` = `+0x8ce0` x `+0x8ce4`, which settles 6b's Ferdinand question.
  Re-check June's rate argument for Evoke and Anger of Elf against March's own `fPowerRate` in those two cases.
- **Item 2.**
  - Every SAT case in `322ff6d0`/`322ff65c` that reads `[esp+0x60]`/`[esp+0x64]`: there should be only HP up, MP up, HP % up and MP % up.
  - `[64]` is 0 without a `CX2Unit`, and a user without the socket heals exactly as March did.
  - JUNE_BUGS #54's wording.

Hygiene is one command:
`python tools/flagcheck.py JUNE_SOCKET_PARITY --range fed40d3^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_BUFF_BEHAVIOR_PARITY`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR
is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors
under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user (the Done note's open item: the
socket term on the four no-power-rate summons and the default case). Write "Phase 16-step3b3 -- Review" into the plan doc,
update the STATUS row, check `16-step3b4.md` and `16-step3b5.md`, and commit.
