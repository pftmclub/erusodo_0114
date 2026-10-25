# June character Eve (Stage 7) - Phase CH4c - El Crystal and Code Recovery

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4c` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH4b review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_UI` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE` / `EVE UI`) |
| Review | `kickoffs/char-eve/CH4c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Eve lines of the checklist rows named below (`june2017_re/char_eve/plan_checklist_rows.md` gives the line numbers), the `CH4c` rows of `ch0_*.tsv`, and Aisha's matching CH4 Done note as the worked example.

## Task

Scope: the Code Recovery mechanic behind the 10 June-only `CODE_RECOVERY_*` `INIT_COMPONENT` keys (reader `329c47e0`, `ch0_keys.tsv`), the 7aj equipped-slot icon override for class type `0x1a` (El Crystal index at tree `+0x134`; March's `UpdateEveElectraSkillSlotIcon` becomes June's tail), the 7ah statics `Static_EveRecoveryGuide` / `_Cooltime` / `Button_EveRecoveryToolTip`, the 7n sync float, the 7a modifier. June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Eve: CH4c)`. Done gate: each item's effect visible in game or in a named log line.

## Amended by CH0 (2026-10-26)

- **Functions: 2** (`awk -F'\t' '$6=="CH4c"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, state-fn 1).
  June addresses: `329b4860` `329b57a0`.
- **States: 1** (`awk -F'\t' '$8=="CH4c"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_EVE_RECOVERY`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §5 L179-195 (per-character parser census); §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `31e920b0` BUFF_FACTOR::~BUFF_FACTOR (X2BuffFactor.h:652, implicit) (candidate).
- **Code Recovery:** state `ENSI_EVE_RECOVERY` (id 238; Start `329b57a0`, which reaches `329b4860`); the `INIT_COMPONENT` members `329c47e0` fills (`+0x9c68..+0x9c74`, see CH1p's amendment). 7ah statics, 7aj icon. **7n is CH2c's, not here:** writer `3297d730` and reader `329a67b0` are both Electra functions.

## Amended by the CH1c review (2026-10-26)

- **Port the 7ah statics `322e65c0` / `322dee90` here and add their calls.** CH1k (`329aece0`, `329c3da0`) and CH1l2 (`CommonFrameMoveFuture` `32980f90`) leave each call out and mark the spot `// June <va>: call added in CH4c`, because a declaration without a body does not link (VS2010 LNK2019 even when the caller is never called). `grep -an "call added in CH4c" X2Lib/X2GuEve.cpp` lists the sites.
