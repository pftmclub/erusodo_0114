# June character Aisha (Stage 5 pilot) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1k review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the head's call and the Power Attack default sit behind the undefined `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`) |
| Log tag | none (inert until the final CH1) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1k - Done (2026-10-21, Opus)"
grep -a "Char CH1k" june2017_re/funcmap.tsv
```

## Task

Review CH1k's commit against the June listings:
- **CX2GUUser base callbacks:** `31fcbed0` / `31fcbf80` (200 above land, condition 7 before the animation-end test, 45 = LINE_RADIUS * 1.5, the
  14-key skip and 0.93 in the big one), `31ffbd70` (the PvP block left out), `3201b390` (code event 2143, host, 1.0 / 4.5), `320004b0` / `320007e0`
  (pet reactions, `+0x8625`, vslot `0x11c` with state `+0x218` + 5.0, the five `Clear_*` sequences in 41470 on the game major particle system).
- **`31fca9f0`:** the new case 6 / 7 against the listing, and that no live caller passes 6 or 7 yet.
- **`32cede00` head (`June_SetEquippedSkillFuncHead`):** bounds, the four slot tables, the transcendence branch, type cases 5 / 0xc / 6-7-0xd..0x10,
  the 22600-22605 inner switch and the `> 22600` Power Attack, the LIVE wrap in March's `SetEquippedSkillFuncBySkillSlotIndex` (`#else` keeps March's head).
- **`31fcc740` / `31fcc9d0`:** the cancel chain order (SKILL, awakening, WALK, DASH, BWALK; each on ground).
- **The 19 skill sub-state members:** the Meteor / Lightning Shower pairs, Guillotine Press loop, Circle Flame, the Ruin loop (`+0x97a4..+0x97b4`),
  the Hell Drop controls (memo test dropped: `31fca9e0` returns false).
- Two decisions to check: state `+0x218` read as its absent value 0 (WIN / LOSE invincibility = 5 s), and `JUNE_ARME_INPUT_8726` = false (CH1m).

Hygiene is one command:
`python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1k commit>^..<CH1k commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1k -- Review" into the plan doc, update the CH1k row in
STATUS.md, check the CH1l kickoff (its "From CH1k" notes), and commit.
