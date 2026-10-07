# June effect completion - C4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C4c review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C4c done (its Done note and commits); best after its play-test |
| Flag | `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` and `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED` |
| Log tag | `JUNE_FX FURY` in `offline_server.log` (unbracketed: `python tools/logs.py --grep "JUNE_FX FURY"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4b - Done (2026-10-07, Opus 5.5 high; identification only, no code)" "C4c - Done (2026-10-07, Opus 5.5 high)"
grep -n "Effect C4b" june2017_re/funcmap.tsv
```

Also read `kickoffs/june-effect/C4c.md` (the port list) and the phase's commits (`git log --oneline --grep="C4c"`).

## Task

Review C4c against the June binary and its kickoff:
- **Parse** (`32784c00`): `FURY_MODE` bool default false after `FALL_DOWN`; `FURY_DATA` table only, both rates 1.0 on a miss
  and in the StateData ctor (`32393dc0`); the table sits outside `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`.
- **GetFinalDamage** (`@32285757-322857e8`): NPC type + `dynamic_cast`, byte, state flag, nonzero-local tests, rate untested,
  before the DIVIDE call.
- **ResetAnimSpeed** (`327615d0`): replace (not `*=`), 0 skipped, factor order (born, buff, `+0x8b6c`, `+0x8fdc`, comeback x4),
  no `UpdateResultValue` at entry; the `#else` is March's body intact.
- **CheckAndUpdateAdditionalStat** (`32290920 @322909a2`): update only on must-change, then the virtual; users get the empty base.
  C4c checked that every `CX2AdditionalStatByBuff` setter sets must-change (`X2BuffTemplet.cpp:789-835`), so the gated update
  cannot leave the anim result at its ctor 0; re-check that nothing else writes the anim stat without the flag.
- JUNE_BUGS 94-98 against the disassembly.

Hygiene is one command per flag: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_NPC_FURY_MODE --with JUNE_NPC_ANIM_SPEED --range <first C4c commit>^..HEAD`
and the same for `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`. Paste the `files:` / `in this flag's files:` / `RESULT:` lines and explain
every OTHER row and deleted line; a FAIL or ERROR is a finding.

**Test staging.** C4c staged Mars (`kom_contents/data037_kom/Thief_Thin_Zero.src.lua`, lines marked `C4c TEST STAGING`; the
original bytecode is `kom_contents_backup_june_fx_c4c/data037_kom/Thief_Thin_Zero.lua`). If the play-test has passed and the
staging is still there: copy the backup `.lua` back over `data037_kom/Thief_Thin_Zero.lua`, delete
`data037_kom/Thief_Thin_Zero.src.lua` (the restored `.lua` must be md5 `ab2aad2e21178bb6ca67adb4a4b02faa`),
`python tools/kom.py pack data037` (expect `~1 changed: Thief_Thin_Zero.lua` vs the staged live kom), then ask the user before
`/x2-kom-packed copy data037`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under
the same flags (then `tools/build.py --deploy`); hand design decisions back to the user. Write "C4c - Review" into the plan doc,
update the C4c row in STATUS.md, check `kickoffs/june-effect/C4d.md`, and commit.
