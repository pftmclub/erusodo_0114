# June character Raven (Stage 7) - Phase CH1p - Pressed-index space, hooks and June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1p` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GURaven.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-raven/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1p - Done" "CH1p - Review"
```
Then CH0's Done note, the earlier Raven CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_raven/ch0_*.tsv`. Elsword's matching sub-phase is the worked example.

## Task

1. 7ag: Raven's own 4+4 pressed-index decode (`X2GURaven.cpp:5854-5871`), `CountUsedSkill` `:5780`, the `m_eSpecialAttackKeyPressed` write
   `:5876`, `GetSkillSlotIndexUsed` `:10842` and his entry in `X2GUUser.cpp`'s unported-class list: every reader together, as June.
2. Hooks 7h (`32c2b360`, unless CH0 gave it to a skill phase), 7l, 7o (`RRF_DASH_JUMP_COMBO_XX` -5000 arrives with the script), 7p, 7x
   (`AT_COMMAND`, 46 attacks: the user-layer gate `32045340 @320453a0`).
3. The June-only script keys CH0 gives an owner here (`ch0_keys.tsv`).
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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-198 per-character parser census; §7l L472 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7x L669 combo hits carry June's AT_COMMAND tag; adopt the script with its tags; §7ag L806 pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every >.
- No June-only key is owned here: the 7 `BBT_*` / `BFT_*` keys are engine buff sub-table keys, the camera pair is Stage 9's, `IMMUNITY_EDT_LIST_AT_THIS_STATE` backlog Stage 2's (`ch0_keys.tsv`). The 6 march-only-site keys drop no March input.
- **Amended by CH1k (2026-11-06):** June's use path (`32c25af0` / `32c25ba0`) never calls `CountUsedSkill` (March's `SpecialAttackEventProcess` `:5780` does), and the lookup `32c58c30` does not write `m_eSpecialAttackKeyPressed`: the cooldown `32c43bf0` writes it (`+0x8ac0` = chosen slot + 1). CH1p owns the pressed-index space and every reader.
