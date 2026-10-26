# June character Eve (Stage 7) - Phase CH1j2 - Class core transcription: Code: Battle Seraph line (split from CH1j by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1j2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1j20 review (Opus helper phase). One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1j2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1j2"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1j2"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1j2/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1j2` (class 116 and the EBS / ECB states and combos) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1j2` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 46** (`awk -F'\t' '$6=="CH1j2"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, state-fn 45).
  June addresses: `329677d0` `32967830` `32967850` `32967860` `32967890` `329678b0` `329678c0` `329678e0` `329678f0` `32967910` `32967920` `329687e0` `32974000` `329740b0` `32974100` `32974150` `329766f0` `329784c0` `32978500` `329786e0` `32978770` `32978890` `32978ad0` `32978bf0` `32979a00` `32979da0` `32979fe0` `3297a110` `3297a240` `3297e2a0` `32995020` `32995120` `32995470` `32995550` `32995680` `32995ba0` `32995c10` `32995d60` `32995e40` `32995f70` `32995fc0` `32996020` `329962a0` `329965a0` `329a8840` `329ad6c0`.
- **States: 9** (`awk -F'\t' '$8=="CH1j2"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_ECB_DASH_XX`, `ENSI_EBS_DASH_JUMP_COMBO_ZFrontX`, `ENSI_EBS_DASH_JUMP_COMBO_ZUpX`, `ENSI_EBS_DASH_JUMP_COMBO_ZDownX`, `ENSI_EBS_DASH_JUMP_COMBO_XZ`, `ENSI_EBS_BOOST_DASH_START`, `ENSI_EBS_BOOST_DASH`, `ENSI_EBS_BOOST_DASH_AIR_START`, `ENSI_EBS_BOOST_DASH_AIR`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 9** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `32071a80` CX2GameUnit::GetDashSpeed (X2GameUnit.h:2799) (confirmed); `31fce710` user (candidate).

## Amended by CH1e (2026-10-26)

- `ENSI_ECB_DASH_XX` (script id 237) is already an enumerator (CH1e added it for the dash X EventProcess).
- `32979da0` (`ENSI_EBS_BOOST_DASH_AIR.EventProcess`) calls `32978c60`: CH1e transcribed it as `CX2GUEve::June_StateChangeBoostAttackIfPossible()` (June's rewrite of March's `StateChangeBoostAttackIfPossible`); call that, do not re-transcribe it.
- CH1e added `CX2GUEve::June_TryGetEquippedSkillAbilityValue( eType, iSkillID, bUpgradeSwap, float& )` (June `32007ce0`); reuse it for that call.

## Amended by CH1j20 (2026-10-26)

- `31fce710` is `CX2GUUser::JuneIsEquippedSkillKeyPressed( m_InputData, eSkillID )` (`X2GUUser.h`, flag `_CORE`; funcmap `Char Eve CH1j20`): June's call passes `unit +0x86da` and the skill id. Call it; do not re-transcribe it.
