# June character Rena (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1m` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time: `X2GULire_ElvenRanger.*`, `Always.h` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-rena/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1m - Done" "CH1m - Review"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_rena/ch0_*.tsv`. Raven's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32cadc90` (vslot 0x7a4) and `InitStateCommon` whole (find InitStateCommon from vtable `335185bc`): cut-in set index (March's switch
`:1672-1715`), base event states, the START / WIN / LOSE roll (June's `INIT_COMMON_RANDOM_STATE` is keyed by class name), the hyper-mode table name
(`GetHyperModeTableName`, `31fdd490`), the per-job blocks for 3 / 12 / 13 / 24 / 102 / 103 / 114, every callback pointed at the function CH1a-CH1k wrote
(March's own name for `inside` rows, the new name for `new` rows), all from `ch0_state_skill.tsv` only (CH0 checked it against the listing). March's
registration stays in the `#else`, under `_LIVE`. March-only states June drops are not registered; June quirks are kept with a `JUNE_BUGS.md` row.
Reuse Raven's `ch1m_regsim.py` / `ch1m_regcheck.py` (per-class path simulation) with Rena's addresses. Done gate: the build passes with `_LIVE`
undefined, and the one-file probe with it defined compiles the new path; no play-test.

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

## Amended by CH0

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
