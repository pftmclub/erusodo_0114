# June engine remainder - Phase R14 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R14 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R14 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_EQIP_SKIN` |
| Log tag | `[JUNE_ENGINE] EQSKIN` (`python tools/logs.py --grep "JUNE_ENGINE] EQSKIN"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Starting context" "Phase R14 - Equipment skin and enchant rework" "Phase R14 -- Done"
```

Then the funcmap rows tagged `(R14)` and `JUNE_BUGS.md` rows 275-277.

## Task

Review Phase R14's commit against June (Ghidra, read-only):

- `_UpdateSkin()` `3295ae20`: the all-models bool (group order 6 / 0x31 / 0x36 / 7, the templet NULL test), the three adds (line 0xe3d hyper with
  the NORMAL multi-tex XET, 0xe6d from model i to the first empty slot, 0xe82), the removes (all-models from slot 0), and each attachment value.
- `InitSkin` `32963eb0` + `3295ffd0`: XET order, the weapon test (+0xd5 = 2 / 0x19, or no templet), the hyper name from `m_ModelNameHyper[0]`
  stored in `m_pHyperTexChangeXET`, the per-hyper-model skin anim (Set / Add / ChangeAnim( m_AniName ) / Play / UseDXMatrix).
- The texture chain at all six sites (`3295f250`, `3295ffd0`, `3295dfa0`): >= 13 / 9-12 / 7-8 / < 7, and where < 7 deletes stage 2 and where it does nothing.
- Every new skin add's collision argument: `false` under `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION`, March's default when it is off.
- The routed terms (custom-render vector, Lu/Ciel, frame vectors, mesh +0xa40) are comments only, and R14b / Stage 11 / checklist 7z name them.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_EQIP_SKIN --range <R14 commit>^..<R14 commit>` (then `git log -SSERV_IRUHADEV_JUNE_EQIP_SKIN <R14 commit>..HEAD`).
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase R14 -- Review" into the plan doc, update the
phase's STATUS.md row, check the R14b kickoff, and commit.
