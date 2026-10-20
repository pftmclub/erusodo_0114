# June character Aisha (Stage 5 pilot) - Phase CH1h - Class core transcription: class overrides below vslot 0x600, and their helpers

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1h` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1g done (one at a time: CH1a-CH1i all edit `X2GUArme_VioletMage.*`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1 wires it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/pilot-char-revamp/CH1h-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1h"' june2017_re/char_arme/ch0_class_map.tsv           # the functions: june_fn, kind, states, March pair
awk -F'\t' 'NF==8 && $8=="CH1h"' june2017_re/char_arme/ch0_state_skill.tsv   # their states: June callbacks (thunk>target), March callbacks
awk -F'\t' '$5 ~ /CH1h/' june2017_re/char_arme/ch0_checklist_rows.tsv      # checklist rows that land here
```
Then CH0's Done note. Pull a checklist section by its heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1h` (the lifecycle, render and reaction overrides (dtor, Init*, Create*ParticleArray, OnFrameMove, DamageReact, Win / Lose / Draw, HyperModeBuffEffectStart, ...) and the helpers only they call; 62 functions after CH0c) from Ghidra (`decompile_function` at its `june_fn`; read the
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
- Done gate: every class-map row with phase `CH1h` has a `June_` member (the Done note lists them with their June addresses); the build
  passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0

Written by CH0 (2026-10-20). Counts are from `ch0_class_map.tsv`. CH1's class core is about 370 functions, so CH0 split the transcription
into CH1a-CH1i (Sonnet, inert) and left the wiring, the script and the play-test to CH1 (Opus).

## From the CH1a review (2026-10-20)

- The constructor `32cc99d0` builds a container at `+0x9760` (`31c265f0` @ the ctor; size at `+0x9770`, CH1a's stand-in `m_iJune9770`) and sets
  `+0x9778` (`m_hJuneMemoRiseLoop`) and `+0x977c` (a second effect-set handle, used by `32cd86cf` / `32cd8900`) to -1 (`@32ccb01c` / `@32ccb022`;
  again `@32ccb7c3` / `@32ccb7cb`). Identify the element type (its users: `32ce15a0`, `32ce4180`, `32ce1900`, `32cc7f10`, `32ccf4e0`, `32ced640`,
  `32ceca70`) and declare the container; CH1i fills it and retires `m_iJune9770`.

## From CH0c (2026-10-20)

CH0c corrected the registration map (`ch0_state_skill.tsv` june_registration / june_callbacks, the class map's states column). The awk filter above reads the corrected map; registration claims in comments come from it only. Moves for this phase:
- in: none
- out: 32cb7450, 32cbe200, 32cc3080, 32cc3ea0 to CH1e; 32cbc3e0 to CH1c; 32cbdd20 to CH1d; 32cbfaa0 to CH1f (the seven "job branch the extractor skips" state callbacks; they were never yours)
