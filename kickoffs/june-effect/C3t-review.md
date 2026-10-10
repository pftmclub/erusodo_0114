# June effect completion - C3t review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3t review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3t done (its Done note and commit). No play-test exists: the port is dormant |
| Flag | `SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME` |
| Log tag | `JUNE_FX C3t` (C3t writes no log lines) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3t - identify, then port: June's user SHOW_HYPER_CUT_IN_TIME"
grep -n "(C3t)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```

## Task

Review C3t's commit against June `32014fc0`, `32057b80 @32060505` / `@3205e0d0`, `32063650 @3206ce0a` and `3237aa40 @3237f346-3237fafa`.
What C3t could get wrong:
- **The templet offsets.** The parser base is the map node + 0xc, so each parser offset is 4 above the reader's. Re-derive it from the `unitType`
  store `@3237ad18`, and check that the five keys map to `+0x2dc / +0x2f8 / +0x314 / +0x330 / +0x34c`.
- **The berserk flag.** `@3205e0d0`: its position (before `BeginTable("SOUND_SET")`, inside March's `ARA_CHARACTER_BASE` block), the unsigned
  `ja` on `GetHyperModeCount` (March's returns the Future syncData count; June's `3208e300` reads protected `+0x7eb4`), and that vslot `0xa4` is
  not `IsFullHyperMode` (`0x9c`).
- **The reader body.** The `EventTimerOneshot` equivalence, `bHyper` = 0, the `SkillCutIn` one-argument constructor, the bone name, the
  `GetShowActiveSkillShow` call left on March's body (C3t-1 owns June's), and the null checks marked as ours.
- **The Lu/Ciel branches** are left as comments plus checklist 7t: confirm that is reach-equivalent (no second body, no unit type 10 in March).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME --range <C3t's first commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or
ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "C3t - Review" into the plan doc, update the STATUS.md row,
and commit.
