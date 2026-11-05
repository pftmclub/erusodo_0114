# June character Raven (Stage 7) - Phase CH1o - The virtuals under LIVE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1o` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GURaven.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-raven/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1o - Done" "CH1o - Review"
```
Then CH0's Done note, the earlier Raven CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_raven/ch0_*.tsv`. Elsword's matching sub-phase is the worked example.

## Task

The virtual overrides under `_LIVE` that CH1h-CH1k wrote but nothing calls yet; June-only and changed user virtuals Raven overrides (vslot 0x4ac
special-item state, the vslot 510 lookup `32c58c30` behind the quick-slot prefix March already has); Raven's own `SetSkillLevelStateData`
(`X2GURaven.cpp:23019-23214`) with June's `( 3201ad10( skill ) + 1.0 )` term (checklist §7 l.286), or hoisted as June did; the gaps the
CH1a-CH1k reviews routed here. Done gate: the build passes with `_LIVE` undefined, the one-file probe with it defined; no play-test.

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

- **Functions: 9** (`awk -F'\t' '$7=="CH1o"' june2017_re/char_raven/ch0_class_map.tsv`; new 9).
  June addresses: `32c28100`(new ?) `32c28880`(new ?) `32c35fc0`(new ?) `32c3c7a0`(new ?) `32c3d1c0`(new ?) `32c4e5b0`(new ?) `32c69d80`(new ?) `32c69f40`(new ?) `32c69f50`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L286 SetSkillLevelStateData hoisted into CX2GUUser; March copy X2GURaven.cp; §7am L909 delegate-list registrations in InitPassiveSkillState: Raven 32c57660 p.
- 9 triage functions: 8 unsettled callback targets (3 reached only through the `MemoConditionRaven` vtable `33511404`) and `32c3d1c0` (called from base `31fef53b`; AppendEnchantParticle). CanParryingState / AddRageGuage moved to CH1q.
