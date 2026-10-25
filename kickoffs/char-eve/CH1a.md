# June character Eve (Stage 7) - Phase CH1a - Class core transcription: movement and slot-head states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1a` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments); may run beside CH0b (inert code). One at a time with CH1b-CH1l: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1a"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1a"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1a/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1a` (wait, walk, jump, dash, charge MP, throw item, power attack, common buff) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1a` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 44** (`awk -F'\t' '$6=="CH1a"' june2017_re/char_eve/ch0_class_map.tsv`; helper 4, state-fn 40).
  June addresses: `32965f00` `32965f50` `32965fa0` `32965fb0` `32965ff0` `32966020` `32966040` `32966070` `32966080` `329660b0` `329660d0` `329695f0` `32969620` `32970880` `32976e40` `32976e90` `32976f00` `32977670` `329776b0` `32977810` `32977a70` `32977af0` `32977b80` `32977c10` `32977d30` `32977f80` `3297aad0` `3297acf0` `3297ae70` `3297b070` `3297b1f0` `32981680` `32981910` `32981ba0` `32981eb0` `329821a0` `329824c0` `32982790` `32982a00` `32982c00` `32982d30` `329cc0d0` `329cc170` `329cc1f0`.
- **States: 20** (`awk -F'\t' '$8=="CH1a"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_WAIT_STATE_CHANGE`, `ENSI_START_INTRUDE`, `ENSI_WAIT`, `ENSI_WALK`, `ENSI_JUMP_READY`, `ENSI_JUMP_UP`, `ENSI_JUMP_DOWN`, `ENSI_JUMP_LANDING`, `ENSI_DASH`, `ENSI_DASH_END`, `ENSI_DASH_JUMP`, `ENSI_DASH_JUMP_LANDING`, `ENSI_DASH_JUMP_POWER_LANDING`, `ENSI_JUMP_FLY_UP`, `ENSI_JUMP_FLY_DOWN`, `ENSI_JUMP_DASH`, `ENSI_SI1_COMMON_BUFF`, `ENSI_SI1_COMMON_BUFF_HYPER`, `ENSI_THROW_ITEM`, `ENSI_WALL_LANDING`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 5** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `32071a80` CX2GameUnit::GetDashSpeed (X2GameUnit.h:2799) (confirmed); `32d4e7f0` CX2ComboGuide::ShowComboCommand( wstring, wstring, bool ) (X2ComboGuide.cpp:350) (confirmed).
