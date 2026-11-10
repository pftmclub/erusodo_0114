# June character Rena (Stage 7) - Phase CH4b - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH4b` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH4a (play-test passed; no review of its own) and CH3c (play-test passed). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA PASSIVE`) |
| Review | `kickoffs/char-rena/CH4b-review.md` (Opus 5.5 high; the phase writes it when done). **It covers CH4a + CH4b** (user, 2026-11-10): both Done notes, `flagcheck --range` over both, `--check-all`, retires `_PASSIVE_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH4b - Done" "CH4b - Review"
```
Then the Rena lines of the checklist rows named below (`june2017_re/char_rena/plan_checklist_rows.md` gives the line numbers), the
`CH4b` rows of `ch0_*.tsv`, and Raven's matching CH4 Done note as the worked example.

## Task

`SetDamageExceptionProcess` `32cad510` whole (June-only, 7b: per-skill triggers, Powerful Bowstring's ignore-defence write `32cada25`, funcmap l.925 / l.202 /
l.1289; read the per-skill list from the listing). The overrides CH0 gave this phase: `GetActualMPConsume` / `GetActualCoolTime` 0x6a0 / 0x6a4, buff-templet
set / unset 0x24c / 0x250 (uncarved where so); `MemoConditionRena` (RTTI `3373d738`, vtable from CH0) and the memo readers; §7 l.281 power-rate socket range
if a Rena function calls `3201ad10`. Addresses not named here: CH0 names them.

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Rena: CH4b)`. Done gate: each item's effect visible in game or in a named log line; Powerful
Bowstring / Sharp Kick act on June's ids; 0 Rena `no_state` rows left except passives with no state in June.

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

## Amended by CH0 (2026-11-07)

- **Functions: 4** (`awk -F'\t' '$7=="CH4b"' june2017_re/char_rena/ch0_class_map.tsv`; inside 2, new 2).
  June addresses: `32c6d4d0`(new ?) `32c7be00`(new GetActualCoolTime) `32c85690` `32cad510`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L330 per-class SetDamageExceptionProcess blocks: funcmap 32cad510 (Rena); §7an L939 GetCharacterSpecializationB() readers: row lists Rena 32c85838; §7an L278 Spec A / Spec B readers (l.278): Rena 32ca7130, 32cb6290 (spec A), 32c.
- MemoConditionRena vtable `33516854` (TD `3373d730`): slot 1 `32c6d4d0` (decide). GetActualCoolTime 0x6a4 `32c7be00` reads the `BOW_SKILL_STATE` flag `+0x981d`. SetDamageExceptionProcess `32cad510`.

## Amended by CH4a (2026-11-10)

- D8's other half is still March's: `SetDamageExceptionProcess` keeps `SI_P_LSR_POWERFUL_BOWSTRING` 3405 and `SI_P_LCR_SHARP_KICK` 3400 (two blocks near
  `X2GULire_ElvenRanger.cpp` 34968 / 35017 after CH4a). June's `32cad510` whole replaces them with 23405 / 23400; `RNSI_P_RGA_POWERFUL_BOWSTRING` is registered now.
- The attack-result list (`m_vecDelegateProcessInAttackResult`, June +0x66ac) now holds Rena's four June callbacks; nothing in CH4b pushes to it.
- Not found in CH4a's functions (CH1k amendment): the writer of user action `0x28` (Soft Body, `NoStateChangeActionFrameMove`). Look in the overrides
  CH4b ports (`32c85690`, the damage path) before closing; if June has no writer there, say so in the CH4b review.
