# June character Aisha (Stage 5 pilot) - Phase CH1d - Class core transcription: dash and jump combos

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1d` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1c done (one at a time: CH1a-CH1i all edit `X2GUArme_VioletMage.*`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1 wires it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/pilot-char-revamp/CH1d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1d"' june2017_re/char_arme/ch0_class_map.tsv           # the functions: june_fn, kind, states, March pair
awk -F'\t' 'NF==8 && $8=="CH1d"' june2017_re/char_arme/ch0_state_skill.tsv   # their states: June callbacks (thunk>target), March callbacks
awk -F'\t' '$5 ~ /CH1d/' june2017_re/char_arme/ch0_checklist_rows.tsv      # checklist rows that land here
```
Then CH0's Done note. Pull a checklist section by its heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1d` (the dash combos, dash attacks and jump attacks every job shares; 26 functions after CH0c) from Ghidra (`decompile_function` at its `june_fn`; read the
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
- Done gate: every class-map row with phase `CH1d` has a `June_` member (the Done note lists them with their June addresses); the build
  passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0

Written by CH0 (2026-10-20). Counts are from `ch0_class_map.tsv`. CH1's class core is about 370 functions, so CH0 split the transcription
into CH1a-CH1i (Sonnet, inert) and left the wiring, the script and the play-test to CH1 (Opus).

## From CH0c (2026-10-20)

CH0c corrected the registration map (`ch0_state_skill.tsv` june_registration / june_callbacks, the class map's states column). The awk filter above reads the corrected map; registration claims in comments come from it only. Moves for this phase:
- in: 32cbdd20 (`AVSI_DASH_COMBO_Z2a` EventProcess, from CH1h), 32cbe510 (`AVSI_DASH_JUMP_ATTACK_X2` EventProcess, from CH1f)
- out: 32cb7400, 32cbc930 (`DARK_MAGICIAN_COMBO_X4down`), 32cb7d60 (`AVP_ZZfrontZZ` Init) to CH1f; 32cb8bd0 (`AEM_DASH_JUMP_X3`), 32cc37e0 / 32cc3b70 (`AEM_ZZXX_LOOP`) to CH1e; 32cb8d10 / 32cc73e0 (`ADW_ZZZBackZ`) to CH1g

## From CH1c (2026-10-20)

CH1c (done, review pending) is the pattern for the combo / dash-combo states; read its Done note ("CH1c - Done", with the key-byte / state-id / vslot tables).
- The combo `EventProcess` is June's cancel template, not March's fixed times: copy the block-local macros `JUNE_ARME_ANY_KEY_C` / `_ZXC`, `JUNE_ARME_FLAG_DUE`, `JUNE_ARME_IN_WINDOW`, `JUNE_ARME_FORWARD_KEY`, `JUNE_ARME_WALK_CANCEL`, `JUNE_ARME_DASH_CANCEL`
  into your own block (`#undef` them at its end) and use the March `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`. Members: `m_fEventTime[ i ]` (+0x8c10, 12 bytes each), `bEventFlagList[ 0..3 ]` (+0x7d85..+0x7d88), `m_fUpDownCancelAfter`, `m_fAttackCancelAfter`.
- A function Ghidra has not carved ("No function found") is read with `python june2017_re/funcdis.py --func <va>` (CH1c: `32cbafb0`, `32ce5970`).
- The fireball helpers exist: `June_FlushMpWithRefund( fMp )` (June `32cc2b90`, every fireball MP spend) and `June_CreateComboFireBall( type, pos, rot )` (June `32ce4180`, types 0..7; type 6 is the High Magician dash combo XX, 5 the High Magician big ball).
- State ids are shared by the job variants (see the CH1c note): write the base state's name for a shared id, and a new `ARME_VIOLEMAGE_STATE_ID` member only for a June-only id (as CH1b / CH1c did).

## From CH1c review (2026-10-20)

CH1c passed review with no fix. Two readings to reuse:
- Class tests: June reads `[ unit +0xaf0 ]->[ 4 ]` or unit `+0x38`; both are `GetUnit()->GetClass()`.
- A direct call to `32278070` is UpNowMp plus the MP-text queue, which CH4 ports. Call `UpNowMp` and add the comment `// June 32278070: MP text in CH4` so CH4 can find the call.
