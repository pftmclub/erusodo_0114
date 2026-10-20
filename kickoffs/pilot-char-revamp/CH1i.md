# June character Aisha (Stage 5 pilot) - Phase CH1i - Class core transcription: class overrides from vslot 0x600, and their helpers

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1i` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1h done (one at a time: CH1a-CH1i all edit `X2GUArme_VioletMage.*`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1 wires it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/pilot-char-revamp/CH1i-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1i"' june2017_re/char_arme/ch0_class_map.tsv           # the functions: june_fn, kind, states, March pair
awk -F'\t' 'NF==8 && $8=="CH1i"' june2017_re/char_arme/ch0_state_skill.tsv   # their states: June callbacks (thunk>target), March callbacks
awk -F'\t' '$5 ~ /CH1i/' june2017_re/char_arme/ch0_checklist_rows.tsv      # checklist rows that land here
```
Then CH0's Done note. Pull a checklist section by its heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1i` (the skill flow (SpecialAttackEventProcess 509-512, NoStateChangeActionFrameMove, SpecialAttackNoStageChange), riding, equip view, the Common* overrides, InitEquippedActiveSkillState, and their helpers; 41 functions) from Ghidra (`decompile_function` at its `june_fn`; read the
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
- Done gate: every class-map row with phase `CH1i` has a `June_` member (the Done note lists them with their June addresses); the build
  passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0

Written by CH0 (2026-10-20). Counts are from `ch0_class_map.tsv`. CH1's class core is about 370 functions, so CH0 split the transcription
into CH1a-CH1i (Sonnet, inert) and left the wiring, the script and the play-test to CH1 (Opus).

## From the CH1a review (2026-10-20): the MemoRise mechanic is yours

- vslot `0x718` `32cc7b30` (read from the listing; the decompile fails): `+0x9770 < 3` (unsigned) and the decoded state id == 157 (`AVSI_CHARGE_MP`) and
  `1.5 <= m_FrameDataNow.unitCondition.fStateTime`. It replaces `June_IsMemoRiseChargeActive`.
- The `+0x9760` container (CH1h declares it): filled / consumed by `32ccf4e0` (0x800), `32ced640` (0x7fc), `32cc7f10` (0x880) and `32ce15a0` (not in the
  class map: erase entry `i` if `i < size`, then for the own unit `IsMyUnit` g_pX2Game `322deb80`). Replace every `m_iJune9770` read in CH1a's
  `June_WaitEventProcess` / `June_ChargeMPEventProcess` with its `size()` and delete the stand-in member.
- `322cc030` (CX2Game `+0xb38` = *arg) and its reader `322f4740` (CX2Game per frame, own user only, gated on unit `+0x867c` -> `+0xaf0` == 2 and vslot
  0x718: `+0xb38` += elapsed; at >= 2.8 (`3351aff0`) a 10-slot pass `322eca50` / `322f4480`, then reset). Port both under the flag and replace the no-op
  `June_SetGameFloatB38`. If the 10-slot pass is skill-slot HUD drawing, the drawing goes to CH5 as a bullet; the timer stays here.
- CX2Game `+0xc20` is settled: only its constructor writes it, `June_GameByteC20()` stays false.

## From CH0c (2026-10-20)

CH0c corrected the registration map (`ch0_state_skill.tsv` june_registration / june_callbacks, the class map's states column). The awk filter above reads the corrected map; registration claims in comments come from it only. Moves for this phase:
- in: none
- out: none

## From CH1h (2026-10-20)

- The class map now lists four more CH1i functions: `32cc9680` (helper of `32cc98b0`, vslot 0x834), `32ce6f40` (Angor Hammer mesh, helper of `32ce7480`), `32ce16d3` (the tail of `32ce15a0`) and `32ce1770` (MemoRise container sweep, checklist 7ak; callers `3220f4dc` `3220e995` `328df9db`).
- CH1h wrote `June_ClearMemoRise( const bool* )` (`32ce1900`) and `June_IsMemoRiseEmpty()` (`32cc2880`) on the `m_iJune9770` stand-in. The container's element type is still open: `32ce1770` / `32ce15a0` test each entry with `3288ff80` (a user-skill-tree call), so the entries look like skill ids. Replace the stand-in when you write the filler (`32ccf4e0`, `32ced640`).
- `322deb80` (CX2Game redraws the MemoRise slots; it tests the unit's templet class == 2 and `32cc2880`) is the no-op `June_GameRefreshMemoRise` in the CH1h block's anonymous namespace. Port it here if the memorize UI needs it (else CH4).
- `m_pJuneMpRefund` (`+0x96ac`) is June's slot for what March calls `m_pStrongMindData`.
