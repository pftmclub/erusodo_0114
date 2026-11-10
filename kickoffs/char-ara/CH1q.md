# June character Ara (Stage 7) - Phase CH1q - Force power and berserk

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1q` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_FP` (defined only with `_LIVE`) (+ `..._FP_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA FP`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1q - Done" "CH1q - Review"
awk -F'\t' '$7=="CH1q"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Force power and berserk from `ch0_fp.tsv`: June's whole body inside every March function that differs, March's in `#else`. The known pieces:
`SetForcePower` `32ba6ef0` with the `+0x6d04` spend conversion (checklist l.257 / l.943), `UpForcePower`, FP cancel and the byte `+0x9621` (funcmap
l.1083), `IsFullHyperMode` / `SetFullHyperMode` (vslots 0x9c `32ba6cc0` / 0xa0 `32b96a00`; rows in `june_march_divergent.tsv` left to this port: replace
them), hyper effect start / end `32bb22d0` / `32bb2740` with the buff hyper count (`CX2GameUnit::GetHyperModeCountByBuff`, l.236-237, and its readers),
`IsHyperModeState` `32bd85f0` (l.603 / l.613: also picks the berserk sound set), `GetHypetMpdeStateID` `32ba46c0`, `ChangeMaxCharacterAddResource`
`32baafc0` (l.257), the gauge snapshot 0x740 `32ba4840` / 0x744 `32ba7b50` as `StoreGageSnapshot` / `RestoreGageSnapshot` (l.258; the Crest bases stay
unported, as Rena's), the 7 `INIT_COMPONENT` keys' consumers, `CX2AraGageData` / `CX2AraMyGageUI` against June's vtables, and the emulator's
`CAC_FORCE_POWER` answer (`X2OfflineStatTable.cpp:306`). Split as CH0 says (CH1q3 for the gauge UI if over ~45 functions). Sub-tag `FP`; the debug
lines stay until this phase's review. Full build only if this phase hands off a play-test (it normally does not: CH1 final tests it).

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
