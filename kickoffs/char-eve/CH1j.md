# June character Eve (Stage 7) - Phase CH1j - Class core transcription: Electra line (CH1j2 takes Battle Seraph)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1j` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1i review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1j-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1j"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1j"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1j/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1j` (class 26 and the EPR / PR states and combos) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1j` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 30** (`awk -F'\t' '$6=="CH1j"' june2017_re/char_eve/ch0_class_map.tsv`; helper 9, state-fn 21).
  June addresses: `32967450` `32967470` `32967480` `32967490` `329674b0` `32972af0` `32972b30` `32972b80` `32972ed0` `3297a8e0` `3297b510` `3297ccf0` `3297d050` `32980000` `32990c50` `32991940` `32991c40` `32991c80` `32991e10` `32991f50` `32992190` `32992210` `32992440` `32992f60` `329a4c30` `329a5430` `329cd320` `329cdb50` `329cdd90` `329ce4a0`.
- **States: 4** (`awk -F'\t' '$8=="CH1j"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_EPR_COMBO_ZZZfrontZ`, `ENSI_EPR_COMBO_XXZ`, `ENSI_EPR_DASH_COMBO_ZZ`, `ENSI_EPR_DASH_COMBO_ZZZ`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 1** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31f63d10` CX2DamageEffect::CEffect::SetLaserGroupID( int ) (X2DamageEffect.cpp:10757) (confirmed); `320017d0` user (candidate); `32290350` CX2GameUnit::UpDownCrashCamera (X2GameUnit.cpp:13091) (confirmed); `31f8b1c0` KObserverPtr<CX2GameUnit> operator bool (GetObservable() != NULL) (candidate); `31fd7500` user (candidate); `32000a90` user (candidate).

## Amended by CH1d (2026-10-26)

- `329a4c30` / `329a5430` call `m_EveElectraSystem.June_CreatePlane` (June `329a1150`, CH1d) and, if they add a damage effect, `June_AddEffectToSystem` (`32980580`); never March's `CreatePlane` / `AddEffectToSystem`.
