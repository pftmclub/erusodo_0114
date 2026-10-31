# June character Chung (Stage 7) - Phase CH1o - Virtuals under LIVE and the June-only user virtuals

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1o` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Planning evidence (2026-10-31)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1o - Done (2026-10-27)" "CH1o2 - Done (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

Every vtable override CH0 marked `CH1o`, under `_LIVE`, and the gaps the CH1a-CH1l4 reviews routed here. Chung's June-only and changed user
virtuals: 7ap 0x6a8 `32bdba60` (skill use `32c17b82`), 7t 0xa4 `IsBerserkByChance` `32c24b90` (`+0x9714 > GetRandomFloat( 0 )`: identify `+0x9714`
from its writer; the base is `X2GameUnit.h` `IsBerserkByChance`), 0x9c `IsFullHyperMode` `32c24b50`, 0xa0 `32c24b60`. A user virtual Chung needs
whose base is not yet in `CX2GUUser` gets the base and its March call site first (Eve's CH1o2 shape); if that is more than ~4 virtuals, split it
as `CH1o2` and write its kickoff and STATUS row.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: build passes with `_LIVE` undefined; the one-file probe with it defined compiles.

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
