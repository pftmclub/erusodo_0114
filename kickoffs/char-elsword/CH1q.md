# June character Elsword (Stage 7) - Phase CH1q - Way of the Sword

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1q` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time: `X2GUElsword_SwordMan.*`, `X2GageData.*`, `X2GageUI.*`, `X2GageManager.*`, `X2DamageManager.*`, `X2DamageEffect.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (+ `_WOS_DEBUG`) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1q - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

Port June's Way of the Sword from `ch0_wos.tsv`: June's bodies inside March's WoS functions (class `:409-529` / `:7182-7480`,
`CX2ElswordGageData`, `CX2ElswordMyGageUI`, `X2GageManager`, `X2DamageManager::CalcWayOfSwordPoint` / `CalcVigorMpUpOnAttack` and the const
table, the `X2DamageEffect.cpp:5505-5519` caller, the `m_CannonBallCount` sync), June-only pieces under plain names. Shared pieces are ported
whole (no partial updates); Elesis's class-side code stays char-elesis's, and where June's shared body would break her unported March class,
guard it inside the June body with a one-line `stand-in: <class> not ported` comment. `WayOfSword.lua` / `DLG_Elsword_WayOfSword.lua`: `luabind.py
check`, adopted with CH1 final (`--dry-run` here). The vslot 0x740 snapshot override `32d058a0` if CH0 put it here. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`, sub-tag
`WOS`; a temporary `_WOS_DEBUG` for CH1 final's play-test lines.

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
