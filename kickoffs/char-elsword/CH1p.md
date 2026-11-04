# June character Elsword (Stage 7) - Phase CH1p - Pressed-index space, hooks and June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1p` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1p - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

7ag pressed-index space, every reader together: his own 4+4 decode (`X2GUElsword_SwordMan.cpp:5232-5249`), `CountUsedSkill` `:5152`,
`GetSkillSlotIndexUsed` `:12896`, and his entry in the unported-class list at `X2GUUser.cpp:26532` (Aisha / Eve / Chung did theirs: their Done
notes are the worked example). Hooks 7h / 7l / 7o / 7p / 7x as CH0 assigned them; the June-only script keys CH0 gave an owner here (not the
Stage 9 camera pair).

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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 per-character parser census; §7l L470 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7o L507 SET_LIMIT_MAX_G_SPEED -4000 in <prefix>DAMAGE_BOUNDING_DOWN; §7p L546 TIME_STOP_IMMUNE_STATE = TRUE in the class script; §7x L667 combo hits carry June's AT_COMMAND tag; adopt the script with its tags; §7ag L804 pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every >.
- 7h: 24 functions (not 25; `[reg+0x8ca4]` scan, 48 sites) are written by their own phases (`ch0_func_rows.tsv`); CH1p owns the shared cancel rule. No June-only key is owned here (the camera pair is Stage 9's, `IMMUNITY_EDT_LIST_AT_THIS_STATE` backlog Stage 2).

## Amended by CH1k (2026-11-04)

- June's skill-use path (`32cf3290` -> lookup `32d1bd10` -> use `32cf5760` -> cooldown `32d0d8e0`) never calls `CountUsedSkill`; March's `SpecialAttackEventProcess` called it under `DUNGEON_RANK_NEW`. CH1k's `_LIVE` body follows June, so the 7ag readers of that counter see no Elsword skill use until this phase decides.
