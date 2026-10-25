# June character Eve (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1m` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l4 review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 split (2026-10-21, user)"
```
Then CH0's Done note, the earlier CH1 Done notes / reviews (their routed gaps), and the `CH1m` rows of `june2017_re/char_eve/ch0_*.tsv`. Aisha's matching sub-phase (CH1j / CH1k / CH1l / CH1m) is the worked example.

## Task

Port June's `InitState` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE roll `31fe22d0`, the hyper-mode table name (`31fdd490`), the per-job blocks for 5 / 18 / 19 / 26 / 108 / 109 / 116, every callback pointed at its `June_*` member, all from `ch0_state_skill.tsv` only (CH0 checked it against the listing). March's registration stays in the `#else`. Everything new is under `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`, which you add to `Always.h` **commented out** (`//#define`): it stays undefined until CH1 final. March-only states June drops are not registered; June quirks (a registration with no script table) are kept with a `JUNE_BUGS.md` row.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GuEve.*` / `X2GUUser.*` are CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit (the `SKILL_SLOT_10` / `UPGRADE_SKILL_SYSTEM_2013` pairs). Done gate: the build passes with `_LIVE` undefined, and a `cl /P` probe with it defined compiles the new path; no play-test.

## Amended by CH0 (2026-10-26)

- **Functions: 3** (`awk -F'\t' '$6=="CH1m"' june2017_re/char_eve/ch0_class_map.tsv`; override 1, registrar 1, state-fn 1).
  June addresses: `329b8030` `329ba9c0` `329cc0e0`.
- **States: 47** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_START`, `ENSI_WIN`, `ENSI_WIN2`, `ENSI_LOSE`, `ENSI_LOSE2`, `ENSI_EG_START`, `ENSI_EG_WIN`, `ENSI_EG_WIN2`, `ENSI_EG_LOSE`, `ENSI_EG_LOSE2`, `ENSI_AT_START`, `ENSI_AT_WIN`, `ENSI_AT_WIN2`, `ENSI_AT_LOSE`, `ENSI_AT_LOSE2`, `ENSI_PR_START`, `ENSI_PR_WIN`, `ENSI_PR_WIN2`, `ENSI_PR_LOSE`, `ENSI_PR_LOSE2`, `ENSI_NS_START`, `ENSI_NS_WIN`, `ENSI_NS_WIN2`, `ENSI_NS_LOSE`, `ENSI_NS_LOSE2`, `ENSI_EP_START`, `ENSI_EP_WIN`, `ENSI_EP_WIN2`, `ENSI_EP_LOSE`, `ENSI_EP_LOSE2`, `ENSI_BS_START`, `ENSI_BS_WIN`, `ENSI_BS_WIN2`, `ENSI_BS_LOSE`, `ENSI_BS_LOSE2`, `ENSI_HYPER_MODE`, `ENSI_ECN_HYPER_MODE`, `ENSI_ECE_HYPER_MODE`, `ENSI_ECB_HYPER_MODE`, `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_END`, `ENSI_A_EAT_OBERON_GUARD_HYPER`, `ENSI_SA_EEG_ATOMIC_SHIELD_HYPER`, `ENSI_SI1_ILLUSION_STRIKE_HYPER`.
- **Registrars:** InitState `329ba9c0` (vslot 0x7a4; cut-in set `+0x22b2` = 0 for 5 / 18 / 19, 1 for 108, 2 for 109, 3 for 116, **none for Electra 26, as March**; sub-index `+0x22b3` 1 / 2 for 18 / 19), InitStateCommon `329b8030` (109 registrations), `3202b110` (5 user event states `USI_EVENT_*`, ids 111-113 / 115 / 116), the roll `31fe22d0` (ids 9 / 11 / 12: the 35 START / WIN / LOSE tables), hyper mode id 23 named by `31fdd490` (`ENSI_HYPER_MODE`, `ENSI_ECN/ECE/ECB_HYPER_MODE`).
- **Class gates** in InitState (decompile lines, `GATES` in `ch0_build_state_skill.py`): 18 / 108, 19 / 109 and 26 / 116 blocks; a switch on the class (0x12 / 0x6c, 0x1a / 0x74, 0x6d) and three ifs (108, 109, 116) that feed the 6 `~` rows; a 19 / 109 tail (`ENSI_DASH_JUMP_COMBO_XZ`, `ENSI_DASH_COMBO_ZX`).
- **Listing check:** `ch0c_listing_check.py` reproduces 158 / 158 rows of `ch0_reg.tsv`; the 6 `~` rows and 4 std::string rows were also checked by hand.
- **June registers with no table:** `ENSI_RIDING_DAMAGE_BACK` (id 86): June's script defines `ENSI_RIDING_DAMAGE_FRONT` twice and no BACK (the second FRONT wins), and `ENSI_HA_EBS_PSYCHIC_ARTILLERY` twice: `JUNE_BUGS.md` rows. **Dead script tables:** 22 (verdict `june-dead-data`). **March-only, dropped:** `ENSI_A_EAT_OBERON_GUARD_HYPER`, `ENSI_SA_EEG_ATOMIC_SHIELD_HYPER`, `ENSI_SI1_ILLUSION_STRIKE_HYPER`.
- `USI_WAIT_STATE_CHANGE` id 0x82 (7h).
