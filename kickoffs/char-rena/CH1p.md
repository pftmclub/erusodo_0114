# June character Rena (Stage 7) - Phase CH1p - Pressed-index space, hooks and June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1p` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-rena/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1p - Done" "CH1p - Review"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_rena/ch0_*.tsv`. Raven's matching sub-phase is the worked example.

## Task

Port 7ag, the pressed-index space (her decode, `CountUsedSkill`, every reader together); hooks 7h / 7l / 7o / 7p / 7x; and the June-only script keys CH0
gave an owner here (`BOW_SKILL_STATE` x26, `IMMUNE_TO_SUPER_ARMOR_DOWN` x10 with readers `32ca81b0` / `31fdaaa0`; `CREATE_TRAP_BLADE` stays CH2d0's unless CH0
moved it). Done gate: the build passes with `_LIVE` undefined and the one-file probe with it defined compiles; no play-test.

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

## Amended by CH0 (2026-11-07)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L183-L198 per-character parser census; §7l L472 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7ag L807 pressed-index space 5 + 5: CommonSpecialAttackEventProcess and every >.
- CH0 found no June-only key for this phase: `BOW_SKILL_STATE` is CH1j + CH4b, `IMMUNE_TO_SUPER_ARMOR_DOWN` is routed to backlog Stage 2 (per-state immunity lists, reader `31fdaaa0`). Scope: 7ag and the hooks 7h / 7l / 7o / 7p / 7x as the plan says.
