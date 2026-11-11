# June character Elesis (Stage 7) - Phase CH1p - 7ag, hooks and the June-only script keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1p` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ELESIS CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-elesis/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1p - Done" "CH1p - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1p"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

7ag pressed-index space: `UT_ELESIS` joins June's set in `CommonSpecialAttackEventProcess` (`X2GUUser.cpp:26778-26797`, whose comment says
"Elesis, Add not ported") under her `_LIVE`, with her decode, `CountUsedSkill` (`X2GUEL.cpp:3157`), `CanUseSpecialAttack`'s `> 3` slot test
(`:3012`) and every reader together (Ara's CH1p Done note is the shape; `_LIVE` needs `JUNE_SKILL_SLOT_10`: add the `Always.h` guard); hooks 7h /
7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here (`EFFECT_SET_LIST_EX_NO_TRANS` x4, `_EX_TRANS_1` x4, `_EX_APPLIED_SKILL` x8,
`EFFECT_SET_LIST_ULTIMATE_EX_APPLIED_SKILL` x1, `RELATIVE_SKILL_LEARN_CHECK` x8, `IGNORE_STATE_DEFENCE` x2, `IGNORE_DEFENCE_DECLINE` x2: the loader
side; a reader in the effect or damage engine with no March counterpart is its own `CH1p0`: stop and write its kickoff); the reserved-buff path
(7ai, checklist l.870-871): her ids for Endurance Power and Flame Sword as June's script keys them, so CH1 final's `[JUNE_SKILL] SBUF` lines appear.
Probe with `_LIVE`; no full build.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).
