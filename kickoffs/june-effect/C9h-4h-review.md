# June effect completion - C9h-4h review - the Heavenly Combat Support Kit event states

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4h review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4h done (its Done note), the regression play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_COMMON_SKILL_STATE`, `_EVENT_STATES`, `_EVENT_KIT_PICKUP` |
| Log tag | `JUNE_FX C9h-4h` (`python tools/logs.py --grep "JUNE_FX C9h-4h"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4h - Done"
grep -n "(C9h-4h)" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4h code against June in Ghidra (read-only; raw ranges with `june2017_re/disasm_range.py`, several handlers have no Ghidra function) and fix
transcription errors under the same flags. Nothing reaches these states in play (no drop, no NPC assets), so the review is the only check of the handlers. The things it
could get wrong:
- **x87 compares** (each one's direction, strict vs not, and NaN): `320211d0` / `320212b0` (anim >= qword 0.6; state time > 5), `3200d450` (timer strictly above
  Future[0][0]), `31fc9a20` (state time not below Future[0][0]; signed shot byte > 0), `3200e260` (keyInputStart <= state time, state time < keyInputEnd), the cancel windows
  of `31fcfb70` / `31fcfde0` (time >= 0, anim strictly above).
- **The aim classifier** in `3200d450 @3200d6e0-3200d859` (`JuneEventSetAimByKeys`): the order of the nine branches, which of pureUp/Down/Left/Right each tests, the values
  1..9, and the "<= -750" block (JUNE_BUGS 160). Then the frame moves' jump tables (`3200db78`, `3200e9b8`) against `s_apszJuneEvent*AimState`.
- **The Proton fire start** `3200e9e0 @3200eb43-3200ebf3`: +180 only when facing left, the -800 offset added facing right / subtracted facing left, dir.y zeroed.
- **The NPC create** (`3200d1d0 @3200d36f`, `3200e040 @3200e1e8`): CreateNPCReq's argument order against `JuneDieCreateNpc`'s (fDelayTime 0.3, team, NAT_ALLY, ally UID).
- **`3201bd60( false )`**: the battle-field branch and `!m_bGameEnd && !m_bLastKillCheck`; June's end phase 1 (`327ca110`) has no March counterpart (labelled).
- **`3230c1a0`**: the 4343 branch acts on the NEW NPC (JUNE_BUGS 161), and `SetShowObject( false, true )` only for another team than my unit's.
- **The latch:** closed by `JuneStartEventKit`, opened by `3200dff0`, `3200ed60`, and every exit of `31fcfde0`.
- **Hygiene:** `python tools/flagcheck.py <each flag> --range <first C9h-4h commit>^..HEAD`. The ERROR rows on the `Always.h` dependency guards are the known limit.

Write "C9h-4h - Review" into the plan doc, update the STATUS row, and commit with explicit pathspecs.
