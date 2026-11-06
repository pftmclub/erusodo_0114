# June character Raven (Stage 7) - Phase CH4c - Veteran Commander overheat and memo readers

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4c` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3c2 review and CH4b review. One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN PASSIVE`) |
| Review | `kickoffs/char-raven/CH4c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4c - Done" "CH4c - Review"
```
Then the Raven lines of the checklist rows named below (`june2017_re/char_raven/plan_checklist_rows.md` gives the line numbers), the
`CH4c` rows of `ch0_*.tsv`, and Elsword's matching CH4 Done note as the worked example.

## Task

Veteran Commander overheat from `ch0_overheat.tsv`, as June: the mode passive (`SI_P_RVC_OVERHEAT_MODE`'s June id), `CreateOverHeatObject`
(`X2GURaven.cpp:21337`), the `SA_OVERHEAT_HP` readers, `X2Game.cpp`'s VC slot texture / Shadow Step reads (`:14374-14442`, `:14949-14959`), the
35-member extra-damage pack (`X2GameUnit.h:8-42`); the pieces CH1g2 / CH3c did not take. `MemoConditionRaven` (vtable `33511404`, ctor
`32c3c780`) and the memo readers. The Specialization B readers `32c49410` / `32c507da` unless CH1q took them (7an).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Raven: CH4c)`. Done gate: each item's effect visible in
game or in a named log line.

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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7aa L701 m_RavenLeftArmItemID basic-item pair (every class): June reader 329657.
- **Map: `june2017_re/char_raven/ch0_overheat.tsv`** (50 rows, col 7 owner: 24 CH4c). June reads `SA_OVERHEAT_HP` (slot 125) / `_PVP` (145) only in InitPassiveSkillState (24415 @32c58307); March's CommonFrameMove re-read has no twin. `_CanOverheat` / `_BurnOverheatHP` are not located; the extra-damage pack is 37 in June vs March's 35; the VC skill-slot icon swap is June `328e0aa0`. Left arm: June's reader `329657d0` is inside `3295dfa0` (june-engine R14c ports it whole); arm hiding `3201c3a0` is already ported; CH4c locates the viewer and skill-state readers.

## Amended by CH4a (2026-11-07)

- **Taken by CH4a's `InitPassiveSkillState` `32c57660` (whole):** the Overheat Mode id 24415 ( `RNSI_P_RVC_OVERHEAT_MODE`, `m_iAddOverHeatLevel` /
  `m_fAddOverHeatBuffDurationTime` ), Behavior of Mercenary's `SA_OVERHEAT_HP` / `_PVP` into `m_fOverheatDamageRate` ( the SA reads are 24411's
  0x91 / 0x7d @32c57f0a / @32c57f11 and 24412's 0x7d x 0.25 @32c57f89 into `m_fLimitOverHeatHP`; @32c58307 is 24415's level read, SA 1 ), Survival Technique ( `+0x9734` and the
  HP-crossing delegate `32c4e5b0` ). CH4c keeps the overheat object, `_CanOverheat` / `_BurnOverheatHP` readers, `X2Game.cpp`'s VC slot, the memo class
  and the left-arm readers.
