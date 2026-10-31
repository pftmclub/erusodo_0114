# June character Chung (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1m` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l4 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1m - Done (2026-10-26)" "CH1m - Review (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32c22540` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE roll
`31fe22d0`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), the per-job blocks for 6 / 20 / 21 / 27 / 110 / 111 / 117, every
callback pointed at the function CH1a-CH1l4 wrote (March's own name for `inside` rows, the new name for `new` rows), all from `ch0_state_skill.tsv`
only (CH0 checked it against the listing). March's registration stays in the `#else`, under `_LIVE`. March-only states June drops are not
registered; June quirks (a registration with no script table) are kept with a `JUNE_BUGS.md` row. Reuse Eve's `ch1m_regsim.py` / `ch1m_regcheck.py`
(per-class path simulation) with Chung's addresses.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: the build passes with `_LIVE` undefined, and the one-file probe with it defined compiles the new path; no play-test.

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

(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)
