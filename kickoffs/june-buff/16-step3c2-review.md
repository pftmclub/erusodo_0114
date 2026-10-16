# June buff port - Phase 16 step 3c, part 2 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3c2 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16-step3c2 done (see its STATUS row for the commit) |
| Flags | `SERV_IRUHADEV_JUNE_SOCKET_EDT` for 156 (now also `#error`s without `_JUNE_BUFF_BEHAVIOR_PARITY` / `_JUNE_BUFF_ENUM`); `SERV_IRUHADEV_JUNE_SOCKET_FEVER` for HMET_FEVER (needs `_SOCKET_PARITY`) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3c — Step 1, read-only (2026-10-07, Opus 5.5 high)" "Phase 16-step3c2 — Done (2026-10-07, Opus 5.5 high)"
```

The `funcmap.tsv` rows carry the decodes; the "Buff Phase 16-step3c2" notes are appended to `32034e50 (EDT cases)`,
`327995c0 (EDT cases)`, `322d4f80`, `3206d480 @3206d6ab`, `32070e40` and `32893e00`. The new rows are `3234ded0`, `3208dd20`
and `3208dc40`.

## Task

Review 16-step3c2 against June:
- **EDT 156** in `CX2GameUnit::JuneProcessSocketExtraDamage` (user `@32039514`, NPC `@3279d323`):
  - Check the gate order: `attackType == AT_COMMAND`, attacker `GUT_USER`, then a `CX2Unit`. The NULL guard is a deviation, JUNE_BUGS #72.
  - The time is `g_pKTDXApp +0x80` = `m_dTime` (from the June ctor `31c17ed0`). It is stored as a float. The compare is `now - time <= 1.0` -> nothing.
  - Check the 10th-hit branch: the clamp, then `3234ded0( rate, false )`, then erase BTI 778, then `( 0, now )`.
  - Check the below-10 branch: BFI 804 at the raw EDD level, then `( count, old time )` (JUNE_BUGS #71).
  - The two `CX2GUUser` members are zeroed in the ctor only (`@32041e20/26`).
- **`CX2UserSkillTree::ReduceLeftSkillCoolTimeByRateJune`** against `32893e00`:
  - The gage-manager null return.
  - Bar A, then bar B.
  - The filter. It reuses `IsResetCoolTimeTarget( type, templet, true, bool )`; confirm that matches the inlined code.
  - The write happens only on a change, but `SetMySkillCoolTimeList` runs either way.
  - Gage index `i` / `i + 4` (June `+5`).
  - The lists are **not** cleared.
- **HMET_FEVER** (`3206d480 @3206d6ab`):
  - The gate: party manager, `DoIHaveParty`, `g_pX2Game`, game `+0xbd4` clear.
  - `PartyFeverAllUser( 30.0, true )`, then the cool time from the shared tail (EDI = entry `+0xc`).
  - A closed gate starts no cool time.
- **`CX2Game::PartyFeverAllUser( float, bool )`** against `322d4f80`, line by line:
  - The flag is set only with `bTitle` and cleared at the end if it is set.
  - The loop: state != `GUSI_DIE`, HP > 0, Add `UpNowDPValue( 3000, 1 )`, else `ForceChangeHyperModeWithoutMotion`.
  - The MP line. June's `SetNowMp( GetMaxMp(), IsMyUnit() )` bool was not ported: `322b3010` clamps as March and only adds June-only tallies (`+0x100/+0x104/+0x10c`). Confirm that reading.
  - The sound order.
  - The four sequences, with the Title swap.
  - The callers: March's `X2PartyManager.cpp` passes `false` (June `32434f60 @324350c9`), and `323c5aa8` is June-only.
- `JUNE_BUGS` rows 71-72: right reading, right reach.

Hygiene is one command per flag:
`python tools/flagcheck.py JUNE_SOCKET_EDT --range <first 3c2 commit>^..HEAD` and
`python tools/flagcheck.py JUNE_SOCKET_FEVER --range <first 3c2 commit>^..HEAD`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines into the review.
- Explain every OTHER row and every deleted line.
- A FAIL or ERROR is a finding.

Spot-check 2-3 points directly in Ghidra (read-only). If one fails, re-check the whole phase.
Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`. Hand design decisions back to the user.
Write "Phase 16-step3c2 — Review" into the plan doc, update the STATUS.md row, and commit.
