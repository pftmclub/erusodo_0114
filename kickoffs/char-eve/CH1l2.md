# June character Eve (Stage 7) - Phase CH1l2 - Class core transcription: Common* overrides 0x7c0-0x7ec except CommonFrameMove (split from CH1l by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1l2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1l2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1l2"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1l2"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1l2/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1l2` (CommonStateStartFuture / CommonStateStart / CommonFrameMoveFuture / CommonEventProcess / CommonStateEndFuture / CommonStateEnd / CommonRender_Prepare / CreateStepDust) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1l2` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 9** (`awk -F'\t' '$6=="CH1l2"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, override 8).
  June addresses: `32965e20` `32965e30` `32965e50` `32965e70` `329694a0` `32976fb0` `3297a400` `3297c390` `32980f90`.
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31fce710` user (candidate); `320017d0` user (candidate); `3202ed60` user (candidate); `322e65c0` CX2Game::JuneUpdateEveRecoveryUI (June-only CX2Game method, name unsettled) (candidate).
- `CommonFrameMoveFuture` `32980f90` calls the June-only `322e65c0` (Eve Recovery UI): declare it `// June 322e65c0: body in CH4c`.
