# June engine remainder - Phase R12 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R12 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R12 done (its Done note, commit, and the user's play-test log) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS`, `SERV_IRUHADEV_JUNE_RENDER_MOTION_COLLISION`, short-lived `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_REFS_DEBUG` |
| Log tag | `[JUNE_ENGINE] SKREF` (`python tools/logs.py --grep "JUNE_ENGINE] SKREF"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase R10 -- Done" "Phase R12 - Skin-anim device ownership" "Phase R12 -- Done"
```

Then the rows with owner R12 in `june2017_re/r10_skinanim_sweep.tsv`, the `(R12)` funcmap rows, and the `CHANGED` / `balanced, kept` rows of
`june2017_re/r12_skinanim_ref_sites.tsv`.

## Task

Review R12's commit against June's code (Ghidra read-only; `june2017_re/disasm_range.py` for the uncarved `31d28f80`, `31d29070`, `31d290f0`, `31d29170`).
Things this phase could get wrong:

- **One reference per stored device, one close per reference.** `AddModelXSkinMesh` `31d2f3f0` (mesh unless `bCloseDeviceAtDestructor`, then each
  non-NULL XET), `RemoveModelXSkinMesh` `31d2b2f0` (mesh closed after its erase, XET slots nulled), `ClearModelXSkinMeshList` `31d2ba80` (no close of the
  ani-data list; collision and attack deleted), `CleanAnimStructure` `31d2f9a0` (the closes after R11's anim-set block, the swap at the tail).
- **The anim mesh / XET:** `SetAnimXSkinMesh` stores them through `CleanAnimStructure( mesh, xet )` before `GetCloneAC`, and March's two late assignments are
  compiled out; the dtor closes them with `( NULL, NULL )`.
- **The Replace functions:** the `from == slot && from != to` test, ref before close, `ReplaceAniXET` nulling the ani-data slot, `ReplaceMultiTexXET( int )`'s
  empty-list push.
- **Callers:** that no March caller still closes a device the skin anim now closes (the map's 156 rows), above all the two `X2Eqip` `ChangeEnchantLevel`
  sites, and `X2GUNPC.cpp:4694` keeping `bCloseDeviceAtDestructor = true` (June passes 0; the net count is the same: is that right?).
- **MOTION_COLLISION:** the `InitMotion` call sits after `m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion )` only, and the play-test log shows the motion's
  collision count > 0 (if it is 0, units cannot be hit: revert the flag first).
- **KEM 433:** the frame comparison (`GetFrameNum()` = June mesh `+0x850`, anim `m_FrameNum` = `+0x328`), the two texts, and the 20-line cap
  (a logging deviation, named in the Done note).

Read the play-test log: `SKREF ... unbalanced 0`, and the device count back in the village matching the first visit.

Hygiene is one command per flag, not a fresh script: `python tools/flagcheck.py <FLAG> --range <R12 commit>^..HEAD` for each of the three flags. Paste its
`files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flags (then
`tools/build.py --deploy`), and remove `_REFS_DEBUG` if the log already proved the balance. Hand design decisions back to the user. Write
"Phase R12 -- Review" into the plan doc, update the phase's STATUS.md row, check R14's kickoff, and commit.
