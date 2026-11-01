# June character Chung (Stage 7) - Phase CH1n - Base callbacks and the skill-slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1n - Done (2026-10-27)" "CH1n - Review (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

The CX2GUUser base callbacks Chung registers (CH1m's `june2017_re/char_chung/ch1m_user_callbacks.tsv`: 127 members with their June targets): reuse every base Aisha and Eve already ported (widen its `#if` to
`|| defined(SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE)` instead of copying), port the ones still missing whole. Then the head of
`SetEquippedSkillFuncBySkillSlotIndex` `32c1f530` (March `X2GUChung.cpp:5331` since CH1m): bounds, slot tables, the 7i rule, the cases CH1 owns (900 and the
non-skill rows CH0 lists), and the dev free-skill gate `CX2GUUser::IsDevUnlimitedSkillUse()` in Chung's skill-use check (standing rule 2026-10-22).
Skill cases stay March's until CH2 / CH3.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: build passes with `_LIVE` undefined; the one-file probe with it defined compiles.

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

## Amended by CH0 (2026-10-31)

- **Functions: 2** (`awk -F'\t' '$7=="CH1n"' june2017_re/char_chung/ch0_class_map.tsv`; inside 1, new 1).
  June addresses: `32c1f530` `32c24400`(new -).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L260 dev free-skill gate around the cool-time / MP / hyper-stone block of t; §7i L427-441 SetEquippedSkillFuncBySkillSlotIndex: drop the 31fd2af0 snapshot call,.
- **Callees this Opus phase maps itself:** `31fdf970` June dirty-state batch (iterate a std::map, clear 100-byte state slots); `3207c1a0` June dirty-state map iterator begin (first of the 3207c1a0 / 31fdf970 batch).
- `32c1f530` head: the memo switch `31fdbcc0` call at `32c1f63d`; the dirty-state batch `31fdf970` / `3207c1a0` (no March form) is yours to map.
- The skill rows were rebuilt from the listing (`ch0_skill_listing_emu.py` -> `ch0_skill_listing_rows.tsv`): 16 of 27 address-order guesses were wrong (callbacks belong to the END / FINAL / LANDING state registered through the shared tail at `32c2219e`).
