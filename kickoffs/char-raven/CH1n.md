# June character Raven (Stage 7) - Phase CH1n - Base callbacks and the skill-slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1n` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time: `X2GURaven.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-raven/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1n - Done" "CH1n - Review"
```
Then CH0's Done note, the earlier Raven CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_raven/ch0_*.tsv`. Elsword's matching sub-phase is the worked example.

## Task

1. The `CX2GUUser` base callbacks Raven registers: start from Chung's CH1n2 and Elsword's CH1n triage (`june2017_re/char_elsword/ch1n_user_callback_triage.tsv`);
   port only the rows they did not cover, widening their `#if` with Raven's `_LIVE`, never copying a base into the class.
2. `SetEquippedSkillFuncBySkillSlotIndex` `32c641a0` head: bounds, slot tables, the skill-type switch and the 6-case hyper pre-switch, the 7i rule
   (drop the snapshot call; the state byte `+0x11` is answered by `IsJuneSkillSlotState`), the dev free-skill gate
   (`CX2GUUser::IsDevUnlimitedSkillUse()`). The per-skill cases stay March's until CH2 / CH3.
Done gate: the build passes with `_LIVE` undefined, the one-file probe with it defined; no play-test.

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

- **Functions: 2** (`awk -F'\t' '$7=="CH1n"' june2017_re/char_raven/ch0_class_map.tsv`; inside 1, new 1).
  June addresses: `32c641a0` `32c69e30`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L328 skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0; §7i L444 SetEquippedSkillFuncBySkillSlotIndex Raven 32c641a0: drop the snapshot; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch.
- `32c641a0` has three switches (skill type 0xd-0x10, the 6-case hyper switch 0xc / 0x6019-0x601d, the skill-id switch 0x5dc1-0x5eaf): `ch0_skill_listing_rows.tsv` (listing emulator) = 181 rows for 126 ids; 23 ids (106, 24400-24415, 24417-24422, types 1 / 8) leave at the type switch.
