# June character Elsword (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1m` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time: `X2GUElsword_SwordMan.*`, Always.h |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1m - Done (2026-11-01)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32d23ff0` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE
roll `31fe22d0`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), the per-job blocks for 1 / 10 / 11 / 22 / 100 / 101 / 112, every
callback pointed at the function CH1a-CH1k wrote (March's own name for `inside` rows, the new name for `new` rows), all from `ch0_state_skill.tsv`
only (CH0 checked it against the listing). March's registration stays in the `#else`, under `_LIVE`. March-only states June drops are not
registered; June quirks are kept with a `JUNE_BUGS.md` row. Reuse `ch1m_regsim.py` / `ch1m_regcheck.py` (per-class path simulation) with
Elsword's addresses. Done gate: the build passes with `_LIVE` undefined, and the one-file probe with it defined compiles the new path; no play-test.

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

## Amended by CH0 (2026-11-03)

- **Functions: 2** (`awk -F'\t' '$7=="CH1m"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 2).
  June addresses: `32d1f8f0` `32d23ff0`.
- **States: 5** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_elsword/ch0_state_skill.tsv`): `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L426 USI_WAIT_STATE_CHANGE registered in every class InitState; WIN OnEvent; §7l L470 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7ao L940-945 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- InitStateCommon is **one** function, `32d1f8f0` (ids 7-286, no class gate); InitState `32d23ff0` is carved and holds the class gates (`*(+0x867c)+0x38`; ranges in `ch0_build_state_skill.py` GATES), the START / WIN / LOSE roll and id 130. `ch0c_listing_check.py`: **180 / 180** rows; no hand rows.
- Id 162: June registers `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK` when 21216 `RNSI_A_EIS_COMPACT_COUNTER` is learned, else `ESSI_REVENGE_ATTACK` (March gates on `SI_P_ETK_COMPACT_COUNTER`). 7 names registered with no table (`ESSI_ELK_XXXXXX`, `ESSI_RIDING_DAMAGE_BACK`, 5 `USI_EVENT_SNIPER/PROTON_BOMB_*`); `ESSI_RIDING_DAMAGE_FRONT` is defined twice in June's script (a JUNE_BUGS row); 27 March-only states dropped (26 `*_HYPER`, `ESSI_SI1_ES_MEGA_SLASH`); 45 dead tables (`ch0_state_skill.tsv` totals).
