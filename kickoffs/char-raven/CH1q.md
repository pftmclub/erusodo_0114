# June character Raven (Stage 7) - Phase CH1q - Nasod core, rage gauge and hyper orbs

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1q` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time: `X2GURaven.*`, `X2GUUser.*`, `X2GageUI.*`, `X2GageManager.*`, `X2DamageManager.*`, `X2DamageEffect.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`, `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE` (+ `_RAGE_DEBUG`) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-raven/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1q - Done" "CH1q - Review"
```
Then CH0's Done note, the earlier Raven CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_raven/ch0_*.tsv`. Elsword's matching sub-phase is the worked example.

## Task

Port June's Nasod core / rage from `ch0_rage.tsv`: June's bodies inside March's functions (class members and getters `X2GURaven.h:1674-1679` /
`:458-479`, `CreateNasodBall` `:13686`, the `m_CannonBallCount` sync, the rage fill `X2GUUser.cpp:12000-12011` and the guard / parry hooks `:10262`,
`:37195`, `DTT_RAVEN_RAGE_ATTACK` and the `UPGRADE_RAVEN` blocks of `X2DamageManager.cpp` / `X2DamageEffect.cpp`, `CX2RavenMyGageUI` and
`X2GageManager.cpp:167-176`), June-only pieces under plain names (vslots 0x9c / 0xa0 if CH0 put them here; the hyper orb ball `32e6d900` at
June's position, checklist 7aq, as Eve's hyper ball), the Specialization B readers `32c49410` / `32c507da` if CH0 put them here (7an). Shared
pieces whole (no partial updates). Flag `SERV_IRUHADEV_JUNE_CHAR_RAVEN_RAGE`, sub-tag `RAGE`; a temporary `_RAGE_DEBUG` for CH1 final's
play-test lines.

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

## Amended by CH0

(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)
