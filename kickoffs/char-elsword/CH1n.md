# June character Elsword (Stage 7) - Phase CH1n - Base callbacks and the skill-slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1n` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1n - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

The `CX2GUUser` base callbacks Elsword registers: start from Chung's CH1n / CH1n2 triage (`june2017_re/char_chung/ch1n_user_callback_triage.tsv`;
its 16 rows were replaced for every class) and port only rows it did not cover, under their own flag where they are not Elsword's.
`SetEquippedSkillFuncBySkillSlotIndex` `32d223e0` head: bounds, slot tables, the 7i rule (`32d223e0` is the 7i row), the nested hyper switch
0x5461-0x5465, and the dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`). Per-skill cases are CH2 / CH3's.

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

## Amended by CH0 (2026-11-03)

- **Functions: 1** (`awk -F'\t' '$7=="CH1n"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 1).
  June addresses: `32d223e0`.
- **States: 1** (`awk -F'\t' '$8=="CH1n"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SUMMON_MAGIC_SPEAR`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L326 skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0; §7i L442 SetEquippedSkillFuncBySkillSlotIndex: drop the 31fd2af0 snapshot call,; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- `ESSI_SUMMON_MAGIC_SPEAR` (id 286) registers only `CX2GUUser` callbacks (`31ffba50` / `31fcbd10`). The skill switch read from the listing: 143 rows for 127 ids (`ch0_skill_listing_rows.tsv`; 22 passives produce no state).
