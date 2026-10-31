# June character Chung (Stage 7) - Phase CH1p - Pressed-index space, hooks and June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1p` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ag. Pressed-index space, the fifth slot and the A5 / B5 states (from June skill system S5)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1p - Done (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

1. **7ag pressed-index space, together:** Chung's own 4+4 decode (`X2GUChung.cpp:5923`, `:6064-6081`, `> 3` / `- 4`), the encoders
   (`m_eSpecialAttackKeyPressed = idx + 1` `:6087`, `m_iNowSpecialAttack = idx + 1` `:6090`), every reader (`InitCurrentSkillKeyManager(
   m_iNowSpecialAttack - 1 )` `:22981` / `:23478`, `switch( m_eSpecialAttackKeyPressed )` `:17264`, InitComponent's slot loops) moved to June's
   10-slot space as June's `SpecialAttackEventProcess` `32bd8790` does. A reader left behind reads the wrong slot (Eve's 7ag trap).
2. Hooks 7h / 7l / 7o / 7p / 7x for Chung (CH0's checklist rows).
3. June-only script keys CH0 placed here: `COMMAND_HIT_ADD_MP_BONUS` (`32bf4900`, protected float `+0x98c4`, §7 l.246), `IMMUNE_TO_SUPER_ARMOR_DOWN`
   (reader `31fdaaa0`: a CX2GUUser reader, share it), `SIEGE_ANGLE` / `MOBILE_SIEGE_ANGLE` if CH0 found a reader.

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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 per-character parser census; §5 L184-188 keys COMMAND_HIT_ADD_MP_BONUS, SIEGE_ANGLE, MOBILE_SIEGE_ANGLE (INIT_C; §7ag L802-814 pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every >.
- Keys: `COMMAND_HIT_ADD_MP_BONUS` (read in InitComponent, +0x98c4); `IMMUNE_TO_SUPER_ARMOR_DOWN` / `IMMUNITY_EDT_LIST_AT_THIS_STATE` are read by `CX2GUUser` `31fdaaa0`: backlog Stage 2, not here. `HEAD_IK*`, `NOMAL_CAMERA*_RATE`, `ATTACK_ACTION_TYPE` are in no June reader (`ch0_keys.tsv`).
