# June character Raven (Stage 7) - Phase CH4b - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4b` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH4a review. One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN PASSIVE`) |
| Review | `kickoffs/char-raven/CH4b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4b - Done" "CH4b - Review"
```
Then the Raven lines of the checklist rows named below (`june2017_re/char_raven/plan_checklist_rows.md` gives the line numbers), the
`CH4b` rows of `ch0_*.tsv`, and Elsword's matching CH4 Done note as the worked example.

## Task

`SetDamageExceptionProcess` `32c5c080` whole (June-only, 7b): 24417 `RNSI_P_RBM_OPEN_MIND` -> `32007b10( 0xf, 24417 )`; 24420
`RNSI_P_RRF_INFINITY_RAGE` with `ATTACK_TRIGGER_TYPE` 0x12 -> `DT_MAGIC` and `DamageEffect_RRF_ExplosiveHand_Impact_NEW`; 24422
`RNSI_P_RVC_WIN_AND_WIN` with 0x13 -> vslot 0x238 and the `329cf6d0` push (= March's `PushBuffFactor`, Eve CH4b). Elsword's `32d1e0e0` is the
worked shape. The overrides CH0 gave this phase: `GetActualMPConsume` / `GetActualCoolTime` 0x6a0 / 0x6a4 (`32c38180` / `32c382d0`), buff-templet
set / unset 0x24c / 0x250 (`32c38340` / `32c383b0`), all uncarved; §7 l.281 if a Raven function calls `3201ad10`.

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Raven: CH4b)`. Done gate: each item's effect visible in
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

## Amended by CH0

(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)
