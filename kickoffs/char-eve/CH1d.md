# June character Eve (Stage 7) - Phase CH1d - Class core transcription: Nasod ground combos

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1d` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1c review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1d"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1d"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1d/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1d` (the base Z / X chains) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1d` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 45** (`awk -F'\t' '$6=="CH1d"' june2017_re/char_eve/ch0_class_map.tsv`; helper 7, state-fn 38).
  June addresses: `329673f0` `32967410` `32967430` `32969e20` `32969e40` `3296a1d0` `3296a1f0` `3296a700` `3296a720` `3296adc0` `3296ade0` `3296b080` `3296b320` `3296b380` `3296b3e0` `3296b400` `3296b9f0` `3296bd90` `3296c850` `3296cab0` `3296cbd0` `3296ce30` `3297b6b0` `3297c830` `32980580` `32983440` `329834a0` `32983690` `329836f0` `329838e0` `32983940` `32983b30` `32983b90` `32983f60` `32984060` `32984d60` `32985710` `32985930` `32991a10` `329a1150` `329afc70` `329b0260` `329cdc80` `329cdf80` `329ce570`.
- **States: 13** (`awk -F'\t' '$8=="CH1d"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_COMBO_Z`, `ENSI_COMBO_ZZ`, `ENSI_COMBO_ZZZ`, `ENSI_COMBO_ZZZZ`, `ENSI_COMBO_ZZZX`, `ENSI_COMBO_ZZZdownZ`, `ENSI_COMBO_X`, `ENSI_COMBO_XX`, `ENSI_COMBO_XXX`, `ENSI_COMBO_XXfrontX`, `ENSI_COMBO_XXfrontXX`, `ENSI_COMBO_XXdownX`, `ENSI_COMBO_XXdownXX`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 19** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `32290490` CX2GameUnit::UpDownCrashCameraNoReset (X2GameUnit.cpp:13104) (confirmed); `31be3020` CKTDXApp::GetProj3DPos( const D3DXVECTOR3& ) (KTDXApp.cpp:3374) (confirmed); `33086b6a` D3DXPlaneFromPointNormal (d3dx9 import thunk) (confirmed).
