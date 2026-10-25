# June character Eve (Stage 7) - Phase CH1g - Class core transcription: Code: Nemesis line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1g` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1f review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1g-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1g"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1g"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1g/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1g` (class 108 states and combos) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1g` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 23** (`awk -F'\t' '$6=="CH1g"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 23).
  June addresses: `32966ea0` `32966ec0` `32966ee0` `329685d0` `32968640` `32968650` `329686f0` `32968700` `32968710` `329709a0` `32970b50` `32970c30` `32970f10` `32971240` `32975bc0` `32975e20` `32975e60` `32975eb0` `3298db90` `3298dd30` `3298de80` `3298e090` `3298e710`.
- **States: 9** (`awk -F'\t' '$8=="CH1g"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_ECN_COMBO_ZZX_READY`, `ENSI_ECN_COMBO_ZZX_LOOP`, `ENSI_ECN_COMBO_ZZX_END`, `ENSI_NS_COMBO_XXZ`, `ENSI_NS_COMBO_XXZZ8`, `ENSI_NS_COMBO_XXZZ8Z`, `ENSI_NS_DASH_COMBO_ZX`, `ENSI_NS_DASH_COMBO_ZXX`, `ENSI_ECN_DASH_JUMP_XZ`.
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d170a0` CKTDXDeviceHolder::XMeshReadyInBackground (KTDXLIB/KTDXDeviceHolder.cpp:115) (confirmed).
