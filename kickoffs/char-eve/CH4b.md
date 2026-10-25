# June character Eve (Stage 7) - Phase CH4b - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH4a review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE` / `EVE UI`) |
| Review | `kickoffs/char-eve/CH4b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Eve lines of the checklist rows named below (`june2017_re/char_eve/plan_checklist_rows.md` gives the line numbers), the `CH4b` rows of `ch0_*.tsv`, and Aisha's matching CH4 Done note as the worked example.

## Task

Scope: `SetDamageExceptionProcess` `329bc560` whole (June-only: no March body; 7b), the other class overrides CH0 lists (MP consume, cool time, buff time), the 7an reader `329b407b`, the 7am NPC-side push (owner `UT_EVE`, skill 25417), 7ae's `32895390` rule if Eve reaches it. June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Eve: CH4b)`. Done gate: each item's effect visible in game or in a named log line.

## Amended by CH0 (2026-10-26)

- **Functions: 4** (`awk -F'\t' '$6=="CH4b"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, override 3).
  June addresses: `32979420` `329a79c0` `329bc560` `329cf6d0`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7b L310-322 (SetDamageExceptionProcess per-class block); §7am L866 L875-876 (delegate lists).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31e920b0` BUFF_FACTOR::~BUFF_FACTOR (X2BuffFactor.h:652, implicit) (candidate); `3234d840` engine (unknown).
- `SetDamageExceptionProcess` `329bc560` calls `329cf6d0` (`DamageData::PushBuffFactor` variants, shared with Rose and Ara: port it once, in the shared place). Overrides: `329a79c0` GetActualMPConsume (0x6a0), `32979420` GetActualCoolTime (0x6a4); Eve overrides neither 0x428 nor 0x73c.
- 7an: the reader `329b407b` sits in `329b3cc0` (CH1l3 transcribes it); CH4b checks the hook. 7am: the NPC-side push `3275a120` (owner `UT_EVE`, skill 25417).
