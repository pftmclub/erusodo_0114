# June engine remainder - Phase R11 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R11 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R11 done (its Done note, commits, and the user's play-test log) |
| Flag | `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK`, `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS`, `SERV_IRUHADEV_JUNE_RENDER_SKIN_BONE_REFRESH` |
| Log tag | `[JUNE_ENGINE] SKANIM` (`python tools/logs.py --tag JUNE_ENGINE`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase R10 -- Done" "Phase R11 - Skin-anim playback and anim-set bookkeeping" "Phase R11 -- Done"
```

Then the rows with owner R11 in `june2017_re/r10_skinanim_sweep.tsv`.

## Task

Review R11's commit(s) against the sweep rows and June's code (Ghidra read-only, `june2017_re/disasm_range.py` for the functions Ghidra has not carved:
`31d2bd80` OnFrameMove, `327f69d0` SetTransition). Things this phase could get wrong:

- **OnFrameMove play types** (`31d2d550-31d2d679`): LOOP restarts with `+0x854`, type 3 with `+0x855`, both `ChangeAnim( index, byte, 0.0f )`, `m_bAnimEnd` and the count as in
  March; ONE / ONE_WAIT untouched. The `+0x854` / `+0x855` defaults (ctor dword `0x01010000` at `+0x852`).
- **The three `ChangeAnim`s:** the signed bounds check, find-without-insert (and that nothing else relied on `operator[]` leaving a NULL entry), the `+0x855` write.
  The name version's write sits before the `ChangeAnim( pAS )` call in March (its anchor line carries a trailing tab); `ChangeAnim( pAS )` writes the same value last.
- **`CleanAnimStructure`** (`31d2f9a0`): the even / `< 0xff` / odd branches, the two log texts, the backup-map Release only in the crash1 branch, `+0x294` cleared
  with the three name maps. `ResetAnimSet` (`31d31c10`): the map is cleared first and filled from the succeeded `GetAnimationSet` only.
- **`MixAnim`** (`31d34140`): the find comes before the mesh / AC checks, the push_back after the new controller's sets are registered and released; never cleared.
- **Bone refresh:** `JuneRefreshBones` vs `31d29950` (clone-root test, matrix vs DXMatrix branch), `JuneRefreshScale` vs `31c83520` (multiplication order, the
  manager-object skip), and the call after `SetScale` in `SetMainEffectScale` vs `31f6fca0 @31f6fd70`.
- **`SetTransition`:** June's own world-object `ChangeAnim` calls (`327f887b..327f8bdc`) still pass the object's `+0x21c`; March's frame move does too.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first R11 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flags (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "Phase R11 -- Review" into the plan doc, update the phase's STATUS.md row, check R12's
kickoff against R11's final state (R12 owns the device refs in `CleanAnimStructure`), and commit.
