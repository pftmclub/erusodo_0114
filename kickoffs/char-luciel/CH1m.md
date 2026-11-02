# June character Lu/Ciel (Stage 8) - Phase CH1m - Class core: Registration

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1m` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1l3 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1m-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1m row: InitState `32b43d10` and InitStateCommon `32b3e8b0` (294 registrations through `31fd89b0`), the start / win / lose
rolls, under `_CORE` / `_LIVE` as the Chung CH1m did. Check it against the raw listing with `charport_regsim.py` (or a `char_luciel` copy of
`ch1m_regcheck.py`): every registration's callbacks = the class map's.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-11-02)

- **Functions: 4 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1m"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b3e8b0` InitStateCommon `32b43d10` InitState `32b4ce90` `32b4cf30`.
  Still `?` (name it by the class pattern from the hint in col 7): `32b4ce90` `32b4cf30`.
- **States: 20** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_WAIT_STATE_CHANGE`, `LSI_START_INTRUDE`, `LSI_START`, `LSI_WIN`, `LSI_WIN2`, `LSI_LOSE`, `LSI_LOSE2`, `LSI_LU_HYPER_MODE`, `LSI_LLD_LU_HYPER_MODE`, `LSI_LCN_LU_HYPER_MODE`, `LSI_LDN_LU_HYPER_MODE`, `LSI_CIEL_HYPER_MODE`, `LSI_LLD_CIEL_HYPER_MODE`, `LSI_LCN_CIEL_HYPER_MODE`, `LSI_LDN_CIEL_HYPER_MODE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_END`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L425 USI_WAIT_STATE_CHANGE registered in every class InitState; WIN OnEventProcess = ; §7l L470 USI_BOUNDING_DOWN / _LANDCRASH / _UP registered in every class InitState with th.
- Registration map `ch0_reg.tsv`: 364 rows (InitStateCommon 294, InitState 65 = 3 rolls + `LSI_WAIT_STATE_CHANGE` + 42 BOUNDING_TARGET + 19 LDB dash combos, user event states 5); `ch0c_listing_check.py` 364 / 364 (5 std::string-named rows: rolls 9 / 11 / 12 from `31fe22d0`, hyper 23 / 204 via `31fdd490`, 8 names by class).
- InitState `32b43d10` sets `+0x8ac8` by class (37 / 38 / 39 / 200 -> 0, 127 -> 1, 128 -> 2, 129 -> 3; no registration is gated on it) and pushes **ten delegate callbacks** (checklist 7am): declare them, bodies in CH4a (`32b14bc0` AddCombinationPointOnHit is CH1p's).
