# June character Elesis (Stage 7) - Phase CH4b - Overrides and memo

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH4b` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH4a. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_PASSIVE` (+ `..._PASSIVE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS PASSIVE`) |
| Review | none of its own: the CH4c review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH4b - Done"   # the worked example
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4b - Done (2026-11-05)"   # the shared Way of the Sword leftovers
```

## Task

Do CH4b: `SetDamageExceptionProcess` 0x2b8 `32b80f20` whole inside March's (`X2GUEL.cpp:9702`): the first part matches March, the June-only part
`@32b81008-32b811ef` reads 28416 / 28417 (`RNSI_P_SBH_TRANSCENDENCE`); `GetActualMPConsume` 0x6a0 `32b7ebe0` (March `:1844`) and `GetActualCoolTime`
0x6a4 `32b7ee10` (`:1929`, listing-only) with the memo reader `31fca9e0`; 0x428 `32b52260` (l.942: `1 - 0.002 x` a level term when vslot 0x524
answers `0x6d6e`: by name); 0x558 `32b6fb80`; `MemoConditionElesis` (vtable `334fa4f4`, `32b51980` / `32b519a0`, created in the ctor at
`+0x7d10`) and the readers of the four `MEMO` state tables and `BFI_ELESIS_ENDURANCE_MEMO`; in her class, what Elsword's CH4b took for the shared
Way of the Sword leftovers (`CureExtraDamage`, `ResetAnimSpeed`'s vigor factor, `SAT_WAY_OF_SWORD_GAUGE_UP`, `GetActualMPConsume`'s x0.7). Play-test.
No review of its own: CH4c's covers it. You are on Opus: delegate the reads, probes and builds.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).
