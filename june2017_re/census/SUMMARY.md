# June 2017 coverage census - summary (june-census V2, 2026-11-09)

How much of the June 2017 exe (55,556 functions) is still to port, and roughly how long it takes at the current pace.
Inputs: `coverage.tsv` (V1), `exclusions.tsv` + `v2_exclusions.py`, `v2_sample.tsv` + `v2_sample.py`, git history.

## What is left

| | functions | file |
|---|---|---|
| studio code with no funcmap row and no stage naming its address or class | **19,577** | `coverage.tsv` (V1) |
| ... attached by address / name to a stage that already exists | 9,014 | `exclusions.tsv`, `stage` = S0-S33 |
| ... in the three new stages (S38 field core 169, S39 game events 148, S40 triage 9,641) | 9,958 | `exclusions.tsv` |
| ... excluded (network glue, serializers, PvP, anti-tamper, telemetry) | 596 | `exclusions.tsv`, `decision` = exclude |
| ... parked for a user decision (`CX2BossRaidManager`) | 9 | `exclusions.tsv`, `decision` = park |
| `unknown` bucket (no evidence either way); 3,401 lie between close studio anchors | 16,776 | `coverage.tsv` |

Largest attachments: S7 characters 2,457 (Ara 461, Add 333, Elesis ~270, shared `CX2GUUser` 546, ...), S9 UI 2,009, S2 engine 1,988, S22 world 725.

**How the groups were made.** March's link map (`X2/US_SERVICE/X2.map`) gives each March function its `.obj`. 4,783 June functions with a
funcmap symbol or class anchor to an `.obj`; June keeps a file's functions together (4,337 of 4,782 neighbouring anchor pairs share an
`.obj`). An unowned function between two anchors of the same `.obj` belongs to that file. Between two different files it is a **gap**: a gap
whose neighbours (or decrypted strings, `func_strings.tsv`) name one system goes to that system's stage; the rest (191 gaps, 9,641 functions)
cannot be placed by address and go to Stage 40, which identifies them in Ghidra and routes them.

**How much of a group is real work** (`v2_sample.tsv`, 300 random unowned studio functions decompiled): logic 189, trivial 59, STL 37,
serializer 15. A hand check of 12 "logic" rows: 7 gameplay bodies, 5 container / ctor / dtor. So about **35-67 % (mid 51 %)** of an
unowned function count is code a phase transcribes; the rest arrives with the types it serves. The biggest gap (`CX2GUUser`..`X2Data`,
1,133 + 2,074 unknown) is mostly packet serializers.

## Estimate (sessions = a phase or a review; a range, not a date)

| term | low | mid | high | basis |
|---|---|---|---|---|
| census work functions | 6,438 | 11,115 | 18,363 | port groups x work share (35 / 51 / 67 %); mid adds 3,401 unknown, high 8,436; low / mid drop 6 % of S40 as excluded later |
| functions per implementation phase | 40 | 30 | 20 | char-raven / char-rena commits: 20-45 "fns inside" per phase |
| census sessions (x reviews x plan overhead) | 266 | 734 | 1,919 | review ratio 0.5 / 0.8 / 0.9 per phase (closed plans), +10 % planning / Z |
| Stage 40 triage | 10 | 15 | 25 | ~10-20 gaps per Opus session |
| open STATUS rows already planned | 120 | 150 | 180 | 153 rows not done today (char-luciel 51, char-rena 27, june-engine 16, asset-port 16, ...) |
| unplanned characters (Ara, Elesis, Add; Rose, Ain) | 230 | 275 | 342 | 40-54 sessions per March character (Eve 54, Chung 51, Elsword 37, Raven 36); new ones 55-90 (Lu/Ciel planned 53) |
| unplanned system stages (19 + Stage 9) | 192 | 288 | 480 | closed system plans: 6-34 sessions (hud 7, result 8, transcend 11, quest 24, skill 28); Stage 9 40 / 60 / 100 |
| **total sessions** | **818** | **1,462** | **2,946** | |
| pace (sessions per week) | 215 | 190 | 150 | git: W42 167, W43 171, W44 214, W45 200 distinct phase / review ids |
| **weeks** | **~4** | **~8** | **~20** | |

**Assumptions, each a reason the number can move:**
1. Pace stays at today's 5-9 parallel plans a week; fewer parallel sessions scale the weeks up linearly.
2. One review per phase (as in the character ports). Dropping reviews on small phases saves up to a third.
3. A phase covers 20-40 June functions. Systems March lacks (S39, S40 output, events) sit at the low end: new classes and emulator handlers.
4. The work share (51 %) comes from 300 functions and a 12-row hand check; its error is roughly +/- 15 points.
5. `unknown` is half library: low counts none of it, mid the 3,401 between close studio anchors, high every unknown between studio anchors.
6. Counted once only where the evidence allows: an unplanned character's class code is in both its port and the S7 attachment
   (~1,000 functions, ~60 sessions, inside the range).
7. Not modelled: play-test time on the user's side, regressions, asset / Lua work outside functions beyond the rows already planned.

## Decision for the user

`CX2BossRaidManager` (9 functions here; world mission, wedding and mentor code sits in S40 gaps) is server-driven multiplayer.
Recommendation: exclude wedding and mentor (they need a second player); let Stage 40 identify boss raid and world mission, and route them to
Stage 22 if they are solo-playable dungeon content.
