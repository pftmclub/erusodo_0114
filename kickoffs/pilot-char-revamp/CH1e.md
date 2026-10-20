# June character Aisha (Stage 5 pilot) - Phase CH1e - Class core transcription: High Magician line states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1e` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1d done (one at a time: CH1a-CH1i all edit `X2GUArme_VioletMage.*`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1 wires it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/pilot-char-revamp/CH1e-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1e"' june2017_re/char_arme/ch0_class_map.tsv           # the functions: june_fn, kind, states, March pair
awk -F'\t' 'NF==8 && $8=="CH1e"' june2017_re/char_arme/ch0_state_skill.tsv   # their states: June callbacks (thunk>target), March callbacks
awk -F'\t' '$5 ~ /CH1e/' june2017_re/char_arme/ch0_checklist_rows.tsv      # checklist rows that land here
```
Then CH0's Done note. Pull a checklist section by its heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1e` (the HM and AEM job states of InitState 32cea0c0 and the HM / AEM-named common states; 50 functions after CH0c) from Ghidra (`decompile_function` at its `june_fn`; read the
listing with `june2017_re/funcdis.py` where the decompile is unclear), **whole**, as a new member of `CX2GUArme_VioletMage` named
`June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), declared and defined under `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`.
- **Inert by design.** Nothing calls the new members yet and March's functions stay untouched; CH1 switches the registration and the
  virtuals over. Do not register states, change `InitState*` or edit a virtual's body here.
- A callee inside the class range (`0x32cb6e00`-`0x32cf2da0`) belongs to the phase the class map gives it: an earlier phase's member exists;
  a later one's gets a declaration with `// June <va>: body in <phase>`. An engine callee is called through its March counterpart
  (`funcmap.tsv`). An unmapped engine callee, or a mechanism the map does not explain, is **"this needs Opus"**: stop and say so.
- Keep June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row per quirk). Class code multiplies `m_fNowStatePowerRate` (June
  `+0x7cc8`), never the entry's `m_fPowerRate` (checklist 7i). Pass June's BFI arguments to `SetBuffFactorToGameUnitByBuffFactorID`
  (7ai, `june2017_re/s3e_bybfi_callers.tsv` part 2).
- `X2GUArme_VioletMage.*` are CP949: `bytepatch.py` only. Check each `#ifdef` branch you insert into with `cl /P`.
- Done gate: every class-map row with phase `CH1e` has a `June_` member (the Done note lists them with their June addresses); the build
  passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0

Written by CH0 (2026-10-20). Counts are from `ch0_class_map.tsv`. CH1's class core is about 370 functions, so CH0 split the transcription
into CH1a-CH1i (Sonnet, inert) and left the wiring, the script and the play-test to CH1 (Opus).

## From CH0c (2026-10-20)

CH0c corrected the registration map (`ch0_state_skill.tsv` june_registration / june_callbacks, the class map's states column). The awk filter above reads the corrected map; registration claims in comments come from it only. Moves for this phase:
- in: 32cb7450 (`HIGH_MAGICIAN_COMBO_X3back` EndFuture), 32cbe200 (`HIGH_MAGICION_DASH_ATTACK_X2` EventProcess), 32cc3080 (`AHM_DASH_JUMP_ZXX_FINISH` EventProcess), 32cc3ea0 (`AEM_DASH_JUMP_X3` EventProcess) from CH1h; 32cb7d00 from CH1c; 32cb8bd0, 32cc37e0, 32cc3b70 from CH1d
- out: 32cb7db0 (`AVP_ZZfrontZZ` StartFuture), 32cb8be0 (`ADM_DASH_JUMP_ZX` FrameMoveFuture), 32cc6a30 (`DARK_MAGICIAN_DASH_COMBO_Z2a` / `ABM_DASH_COMBO_Z2a` Start) to CH1f; 32cb8bf0 (`ABM_DASH_JUMP_ZX` StartFuture) to CH1g

## From CH1c (2026-10-20)

- `AVSI_AEM_ZZX` (June id 233) is already a member of `ARME_VIOLEMAGE_STATE_ID` (CH1c added it for `June_ComboZ2EventProcess`); do not add it again. Its FrameMove / EventProcess (`32ce2470`, `32cc30b0`) are yours.
- `AVSI_AEM_XZ` (198) and `AVSI_AEM_XXfrontX` (id 170, shared with `AVSI_COMBO_X3front`) are the targets of CH1c's X1 / X2 EventProcess; CH1c names the base state for a shared id.
- The combo template and its macros: see the CH1c Done note and the start of the CH1c block (same advice as CH1d's "From CH1c").
