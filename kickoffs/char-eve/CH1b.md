# June character Eve (Stage 7) - Phase CH1b - Class core transcription: reactions

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1a review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1b"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1b"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1b/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1b` (damage (the 21 `ENSI_DAMAGE*` tables), down / stand-up, die, the job win / lose blocks) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1b` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 39** (`awk -F'\t' '$6=="CH1b"' june2017_re/char_eve/ch0_class_map.tsv`; helper 6, state-fn 33).
  June addresses: `32966110` `32966150` `32966190` `329661d0` `32966210` `32966220` `32966260` `32966270` `329694b0` `32969510` `32969550` `329695b0` `32969660` `329696b0` `32969820` `32969860` `329699d0` `32969af0` `32969b70` `32969bf0` `32969c70` `32969cf0` `32969d70` `3298cfe0` `32997c10` `3299b810` `3299bcd0` `3299d140` `329ae670` `329af800` `329af920` `329af950` `329afa70` `329cc050` `329cc0b0` `329cc0c0` `329cc150` `329cc160` `329cc1e0`.
- **States: 29** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_DIE_FRONT`, `ENSI_DIE_BACK`, `ENSI_DAMAGE_GROGGY`, `ENSI_DAMAGE_SMALL_FRONT`, `ENSI_DAMAGE_SMALL_BACK`, `ENSI_DAMAGE_BIG_FRONT`, `ENSI_DAMAGE_BIG_BACK`, `ENSI_DAMAGE_DOWN_FRONT`, `ENSI_DAMAGE_DOWN_BACK`, `ENSI_DAMAGE_STANDUP_FRONT`, `ENSI_DAMAGE_STANDUP_BACK`, `ENSI_DAMAGE_BOUNDING_DOWN`, `ENSI_DAMAGE_BOUNDING_LANDCRASH`, `ENSI_DAMAGE_BOUNDING_UP`, `ENSI_DAMAGE_AIR_SMALL`, `ENSI_DAMAGE_AIR_DOWN`, `ENSI_DAMAGE_AIR_DOWN_INVINCIBLE`, `ENSI_DAMAGE_AIR_DOWN_LANDING`, `ENSI_DAMAGE_AIR_FALL`, `ENSI_DAMAGE_AIR_UP`, `ENSI_DAMAGE_AIR_FLY_FRONT`, `ENSI_DAMAGE_AIR_FLY_BACK`, `ENSI_DAMAGE_REVENGE`, `ENSI_STANDUP_ROLLING_FRONT_FRONT`, `ENSI_STANDUP_ROLLING_FRONT_BACK`, `ENSI_STANDUP_ROLLING_BACK_FRONT`, `ENSI_STANDUP_ROLLING_BACK_BACK`, `ENSI_STANDUP_ATTACK_FRONT`, `ENSI_STANDUP_ATTACK_BACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `32783a90` CX2GUNPC::AttachParticle_LUA( bool, char*, char*, D3DXVECTOR3 ) (X2GUNPC.cpp:29395) (confirmed).
