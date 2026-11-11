# June character Add (Stage 7) - Phase CH1q - DP and formation mode

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH1q` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time with CH1-CH4: they all edit `X2GUAdd.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_DP` (defined only with `_LIVE`) (+ `..._DP_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD DP`) where the phase logs; otherwise none |
| Review | `kickoffs/char-add/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH0 - Add map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CH1q - Done" "CH1q - Review"   # the worked example, once written; else Ara's
awk -F'\t' '$7=="CH1q"' june2017_re/char_add/ch0_class_map.tsv
```

## Task

DP and formation mode from `ch0_dp.tsv`: June's whole body inside every March function that differs, March's in `#else`. Known pieces: `UpNowDPValue` vslot 0x2a4 `329e5b00` / body `329e56c0` (+ `329e5140`, `329e5180`; every spend x `1 - result(+0x6bb0)`), formation entry `32a08580` with the Fantasy Tracer refund from DP actually spent and no decrease term (`JUNE_BUFF_PORT_ARCHIVE.md:1711-1719`), `ChangeMaxCharacterAddResource` `329e31e0`, `ApplyHyperModeBuff` `329e2410` whole (BFI 846 / 315, skills 29401 / 29406 / 29411; `RidingHyperModeEnd`'s base-clear skip, divergent row 371), hyper hooks `329e2340` / `329e2360` with `UAI_RELEASE_FORMATION_MODE` (l.260), `CanHyperMode` `32a31550` and the five vslot-343 callers (l.261), hyper StateStart `32a08150` with `+0x9330` (l.968), spec A `329e59f0` (l.278), `GetActualDPConsume` `329e7820` (the `false` caller of the June MP-consume base), the 0x740 gauge snapshot `329e2f80` / 0x744 `329e3000` as `StoreGageSnapshot` / `RestoreGageSnapshot` (base call per CH2a0: left out as a labelled stand-in if CH2a0 has not run), `CX2AddGageData` against June's vtable, `SAT_ADD_DP_PERCENT_UP` (item 100001054, l.412), `PartyFeverAllUser`'s DP step (divergent row 113), the `ConvertAndAdd` 0.5 check (l.279), and the buff port's R1 edit replaced by June's body. Split as CH0 says (CH1q3 for the gauge if over ~45 functions). Full build only if this phase hands off a play-test (normally not: CH1 final tests it).

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
