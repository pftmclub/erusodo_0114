# June character Raven (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1m` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time: `X2GURaven.*`, Always.h |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-raven/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1m - Done" "CH1m - Review"
```
Then CH0's Done note, the earlier Raven CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_raven/ch0_*.tsv`. Elsword's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32c66fe0` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE
roll `31fe22d0` (June's `INIT_COMMON_RANDOM_STATE` is keyed by class name), the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), the
per-job blocks for 4 / 16 / 17 / 25 / 106 / 107 / 115, every callback pointed at the function CH1a-CH1k wrote (March's own name for `inside` rows,
the new name for `new` rows), all from `ch0_state_skill.tsv` only (CH0 checked it against the listing). March's registration stays in the
`#else`, under `_LIVE`. March-only states June drops are not registered; June quirks are kept with a `JUNE_BUGS.md` row. Reuse Elsword's
`ch1m_regsim.py` / `ch1m_regcheck.py` (per-class path simulation) with Raven's addresses. Done gate: the build passes with `_LIVE` undefined, and
the one-file probe with it defined compiles the new path; no play-test.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Nothing runs until `_LIVE`: no play-test; the hand-off names the next phase.

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

## Amended by CH0 (2026-11-05)

- **Functions: 5** (`awk -F'\t' '$7=="CH1m"' june2017_re/char_raven/ch0_class_map.tsv`; inside 5).
  June addresses: `32c511c0` `32c51b70` `32c5d660` `32c63670` `32c66fe0`.
- **States: 5** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_raven/ch0_state_skill.tsv`): `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L428 USI_WAIT_STATE_CHANGE registered in every class InitState; §7l L472 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7ao L940-945 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- Registrars: InitStateCommon **`32c5d660`** (called from InitState `32c66fe0` before base `3202b110`), the class switch's three line registrars `32c511c0` (16 / 106), `32c51b70` (17 / 107), `32c63670` (25 / 115), and InitPassiveSkillState `32c57660` (6 Shadow Step states; the function is CH4a's). `charport_states.py init 32c5d660 32c66fe0 3202b110 32c511c0 32c51b70 32c63670 32c57660` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **178 / 178**, no hand rows (START / WIN / LOSE rows 9 / 11 / 12 and the hyper row read through the call reader). State ids: one C++ enum `RAVEN_STATE_ID` holds every prefix; 206 live June tables have no C++ member (raw id + comment, as Eve) and June's registered ids differ from the C++ values for all 109 shared names: register by June's id (`ch0_state_enum_names.tsv`). 26 March-only states dropped.
