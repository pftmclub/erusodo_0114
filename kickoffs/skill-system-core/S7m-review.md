# June skill system core - Phase S7m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7m review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7m done (its Done note and commit); no play-test is owed (nothing is reachable offline) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT`, `SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET` |
| Log tag | none (no S7m log lines) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7m - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `(S7m)`; `JUNE_BUGS.md` row 355.

## Task

Review Phase S7m against Ghidra (read-only):
- `X2Unit.cpp` `JuneSumSocketOption`, the `_UNIT_SOCKET_EDT` block, against `32364790 @32364af3-32364bb2`: the gate is templet `+0x324 != 0` (= `EDT_NONE` 0), the copy is
  `3208c500` (= March `ExtraDamageData::Copy`?, check that `Copy` copies what `3208c500` copies), and `+0x344` = templet `+0x334` x the float, with no 0.0 branch.
  Also confirm that `+0x324` really is `m_SocketExtraDamage` (loader `32726850 @3272926f`, case 0) and that the user side `3204f190 @3204f5fc-3204f83d` matches March's
  `CX2GUUser::SumSocketOption` (types 1 / 3 / other when `fAtkMagic == 0`).
- `X2Unit.cpp` `JuneGetPartySocketOptionSources` against `31ed40c0` whole (with `31ed23a0`): the room / game gates, slot states 1 / 2, the mine / others test `@31ed4223`,
  the set entry (rate 1.0, level `@31ed42cb-31ed4313`), the equipped-item gates `@31ed43c2-31ed4420`, the two entries `@31ed4426-31ed4554` (list order: random + templet, then
  `+0x5c`). The phase used March's `GetSetIDNPartsNum` for `32350b80`: check `32350b80` against it.
- `JuneIsPartyShareSocketID` against `31d70d30`; `JuneGetPartyShareSocketOptions` against `32365d30 @323667e9-32366c26` and `3206d770 @3206df43-3206e326`
  (the mine-id collection, the filter order, no dedup across members = JUNE_BUGS 355).
- `X2GUUser.cpp`: the party pass sits where June's does (`3206d770 @3206df16`, right after the equipment loop, before the pet).

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT --range <phase commit>^..<phase commit>` and the same for `_PARTY_SOCKET`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7m -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (`S7p.md`), and commit.
