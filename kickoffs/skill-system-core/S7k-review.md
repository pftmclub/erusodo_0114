# June skill system core - Phase S7k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7k review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7k done (its Done note and commits) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET`, `SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS`, `SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP` |
| Log tag | `[JUNE_SKILL] HUD` (`python tools/logs.py --tag JUNE_SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7k - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7k`; `JUNE_BUGS.md` rows 339-340.

## Task

Review Phase S7k against Ghidra (read-only):
- `CX2Unit::JuneSumSocketOption` (`X2Unit.cpp`) against `32364790`, from the raw disassembly: the PVP_ONLY / DUNGEON_ONLY skip (`@32364823`), the rank / PVE branches
  and the ConvertAndAdd arguments ( socket, level = arg 3 ( EBP ), type ), the `+0x48` map, `+0x38` / `+0x28`, the `+0x374` / `+0x384` front inserts, the summon push
  into the unit's `+0xe20`, the hyper entry ( socket ID -1 ), the eight cool-time adds ( slots 1-4, then the all-skill cut to 1-4 ).
- `JuneInitSocketData` against `32365d30`: the source order, the equipment level ( `+0xd1` byte or `+0xb4` ), the float ( `328842f0`'s `+0x20` ), the pet block's
  state gate covering only the pick-up flag, the riding pet mapping ( June `321f5b10` on manager `+0x40` -> `+0xec`, March the templet's list ), the tail ( `320804f0( 0 )`,
  `3235a080` ). `JuneSumSetItemSocketOption` against `323652b0` ( mode 0 / 2, level fallback ).
- `JuneResetHudMaxHpMp` against `3235a160`: fill( 2 ) -> cap -> fill( 0 ), the MP formula, the HP sum and its float order, min( x 0.5, cap ).
- `GetUnitStat` against `32358b00` ( 4 skills, order, base values, the 1.0 / 5e-8 doubles ) and `GetEqipStat` against `3234e210` ( lists, group test, PvP-equip bit 7,
  floats [6..12] ); `JuneGetEquipGroup` against `3286a4b0`.
- The call sites ( Done note table ): spot-check 3 against the raw disassembly ( placement before / after the neighbouring calls ).

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..<last phase commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7k -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7h, then S7m), and commit.
