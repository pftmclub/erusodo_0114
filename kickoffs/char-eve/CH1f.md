# June character Eve (Stage 7) - Phase CH1f - Class core transcription: Exotic Gear line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1f` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1e review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1f-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1f"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1f"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1f/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1f` (class 18 states and combos) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1f` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 25** (`awk -F'\t' '$6=="CH1f"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 25).
  June addresses: `329662b0` `32968510` `32968560` `32968570` `329685c0` `3296b690` `3296b6c0` `3296b7a0` `3296b880` `3296b8a0` `3296d440` `3296d470` `3296d550` `3296d630` `3296d8b0` `3296d990` `3296da80` `3296daa0` `3296e580` `329795a0` `329797d0` `329847f0` `32986440` `32986710` `32986f00`.
- **States: 6** (`awk -F'\t' '$8=="CH1f"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_COMBO_ZZZdownZZ`, `ENSI_DASH_COMBO_ZZ`, `ENSI_DASH_COMBO_ZZZ`, `ENSI_DASH_COMBO_XZ`, `ENSI_EEG_DASH_JUMP_ZX`, `ENSI_EEG_DASH_JUMP_ZZX`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 2** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `32290490` CX2GameUnit::UpDownCrashCameraNoReset (X2GameUnit.cpp:13104) (confirmed).
