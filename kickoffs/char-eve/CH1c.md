# June character Eve (Stage 7) - Phase CH1c - Class core transcription: riding

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1c` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1b review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1c"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1c"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1c/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1c` (the 27 `ENSI_RIDING*` tables and their callbacks) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1c` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 24** (`awk -F'\t' '$6=="CH1c"' june2017_re/char_eve/ch0_class_map.tsv`; state-fn 24).
  June addresses: `329ac330` `329cbff0` `329cc000` `329cc010` `329cc020` `329cc030` `329cc040` `329cc060` `329cc070` `329cc080` `329cc090` `329cc0a0` `329cc0f0` `329cc100` `329cc110` `329cc120` `329cc130` `329cc140` `329cc180` `329cc190` `329cc1a0` `329cc1b0` `329cc1c0` `329cc1d0`.
- **States: 37** (`awk -F'\t' '$8=="CH1c"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_PEPPER_RUN_READY`, `ENSI_PEPPER_RUN`, `ENSI_PEPPER_RUN_END`, `ENSI_PEPPER_RUN_JUMP_UP`, `ENSI_PEPPER_RUN_JUMP_DOWN`, `ENSI_AMUSEMENT_ITEM_PHOENIX_RUN`, `ENSI_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `ENSI_DANCE_Z`, `ENSI_DANCE_X`, `ENSI_DANCE_WAIT`, `ENSI_RIDING_START`, `ENSI_RIDING_ON`, `ENSI_RIDING_OFF`, `ENSI_RIDING_WAIT_HABIT`, `ENSI_RIDING_WAIT`, `ENSI_RIDING_WALK`, `ENSI_RIDING_JUMP_UP`, `ENSI_RIDING_JUMP_DOWN`, `ENSI_RIDING_JUMP_LANDING`, `ENSI_RIDING_DASH`, `ENSI_RIDING_DASH_END`, `ENSI_RIDING_DASH_JUMP`, `ENSI_RIDING_DASH_JUMP_LANDING`, `ENSI_RIDING_DAMAGE_FRONT`, `ENSI_RIDING_DAMAGE_FRONT`, `ENSI_RIDING_ATTACK_Z`, `ENSI_RIDING_ATTACK_X`, `ENSI_RIDING_JUMP_ATTACK_Z`, `ENSI_RIDING_ATTACK_SPECIAL`, `ENSI_RIDING_DIE`, `ENSI_RIDING_SPECIAL_MOVE`, `ENSI_RIDING_HYPER_MODE`, `ENSI_RIDING_SPECIFIC`, `ENSI_RIDING_ATTACK_X_READY`, `ENSI_RIDING_ATTACK_X_READY_2`, `ENSI_RIDING_ATTACK_X_END`, `ENSI_RIDING_DAMAGE_BACK`.

## Amended by the CH1b review (2026-10-26)

- `ENSI_RIDING_DIE`'s callbacks are CH1b's members: StartFuture / Start / FrameMove / EventProcess reach `329694b0` / `329af800` / `329af920` / `32969510` (`June_ENSI_DIE_FRONT_*`, through the thunks `329cc530` `329cf5c0` `329cf5d0` `329cc540`), End is `CX2GUUser`'s `3202a7d0`. Transcribe nothing for it here; CH1's registration points at the CH1b members.
