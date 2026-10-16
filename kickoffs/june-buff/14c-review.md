# June buff port - Phase 14c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 14c review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase 14c done (its Done note and commit; play-test of the tutorial checks passed) |
| Flag | `SERV_IRUHADEV_JUNE_TUTORIAL_MODE` (+ the emulator's `SERV_IRUHADEV_OFFLINE_FIRST_SELECT`) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 14c — CX2Main +0x16a mode flag (June-only)" "Phase 14c — Done"
```

Also `june2017_re/c14c_tutorial_reads.tsv` (every reader and setter caller, one row each) and the
funcmap rows `31d716c0`, `31f5e290`, `31f5d4a0`, `31f5da70`, `327d2160`, `31fbde80`, `327cf620`.
The commit range is in the Done note.

## Task

Review Phase 14c against June. The flag turned out to be March's own `m_bIsPlayingTutorial`, so the
phase is a diff: only reads March lacked were ported.
- **Completeness.** Re-run `python june2017_re/sweep_main_tutorial.py` (91 rows) and confirm every
  `[reg+0x16a]` / `[reg+0x16b]` hit on a CX2Main base has a row; add the 5 callers of the guide
  manager getter `31fbde80` and the 20 setter calls. Check that no other out-of-line getter of
  `+0x16a` exists.
- **Every PORTED row.** Read June's code at the cited address and the ported hunk side by side:
  same polarity, same co-conditions in the same `if`, and June-only terms named in the comment, not
  invented. Spot-check at least: the clamp (`@32049b37-32049b6e`), RT_FLY (`@31feb404-31feb47a`),
  SetShowUI (`32154820`), the quick-quest pair (`323ddfe0`, `323e1bf0 @323e1c67`), the select-unit
  first-select bool (`kEvent +0x12a` in GET `320e9a80` = `m_bFirstSelect`), and the key pair
  (`31f5e290` / `31f5d4a0` / `327d2160`).
- **Every HAS row** you spot-check must match March's read at the cited line.
- **The listed (not ported) rows**: confirm each really needs a June-only system or would break March
  (`327f0685`'s auth gate, `327ca142`'s end delay, `324d1654` QuitGame, `3255b5c3` CREATE_UNIT_ACK).
- **The #else path.** Flag off = March: inline setter back, no latch, no KeyPad members, `m_bFirstSelect`
  false from the emulator.
- The two deviations: the key restore runs only with the latch set and a non-empty copy (JUNE_BUGS #89),
  and `find()` replaces `at()` in the apply loop. Confirm both are needed and nothing else deviates.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_TUTORIAL_MODE --range <range from the Done note> --with SERV_IRUHADEV_OFFLINE_FIRST_SELECT`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row
and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 14c — Review" into the plan doc, update the 14c STATUS.md row,
and commit. After the review the buff port's own scope is complete; no next kickoff to write.
