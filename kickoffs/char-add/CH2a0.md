# June character Add (Stage 7) - Phase CH2a0 - Crest of Time and death-prevention engine (shared CX2GUUser)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH2a0` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH1 review; before the skill phase CH0 names for skills 207 / 29212 / 29230 |
| Flag | `SERV_IRUHADEV_JUNE_USER_CREST_OF_TIME` (class-generic; + `..._CREST_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD CREST`) |
| Review | `kickoffs/char-add/CH2a0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH2 - Skills: Nasod Ruler and the first jobs (classes 9, 32, 33, 36)"
grep -n "Crest of Time\|death-prevention\|death prevention" june2017_re/char_add/plan_routed_items.md june2017_re/char_add/plan_checklist_rows.md
cat june2017_re/char_add/ch0_crest.tsv
```

## Task

Port the Crest of Time / death-prevention engine whole (routed here by char-chung CH4b, 2026-11-03; Add's `ANSI_SA_COMMON_CREST_OF_TIME` is
June's only cast of BTI 436 / 926): the user slot-250 pre-apply hook `3201ffb0` (BTI 436 / 926 / 807), the snapshot / restore bases `32009780` /
`3204da60` (vslots 0x740 / 0x744; members `+0x92a4..+0x92c4`; the clear must reach Chung's `m_iSnapshotCannonBallCount` / `Ex`), the
death-prevention gate `320072d0` in `UpNowHp` / `SetNowHp` after the BBT 101 refusal and before the base call (also skills 27409 / 23402), the trigger
`3202b9a0`; then every class override calls the base as June's does: Chung's (CH1l), Elsword's `32d058a0` / `32d058d0`, Rena's `32c852f0` /
`32c89230`, Ara's `32ba4840` / `32ba7b50`, Elesis's `32b6ac80` / `32b6acb0` (if her port has landed them; else a note in her plan) and Add's
`329e2f80` / `329e3000`: each earlier port left the base call out as a labelled stand-in (`ch0_crest.tsv` lists the sites); restore them under this
flag. The cast itself is the skill phase's. Full build only if the phase hands off a play-test (it should: a unit with skill 27409 or a buff of BTI
436 survives a lethal hit once, logged `ADD CREST`; pick a check the offline client reaches, `/additem` / `/x2-db` setup, never PvP).

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
