# June character Rena (Stage 7) - Phase CH1q - Natural force (ADD_RENA_SYSTEM)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1q` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1p review (Open decision 1 answered 2026-11-07: (a)). One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*`, `X2Gage*.*`, `X2DamageManager.*`, `X2DamageEffect.cpp`, `X2EffectSet.*`, `X2GUNPC.*`, `CommonPacket.h` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (+ `_NF_DEBUG`; defined only with `_LIVE`) |
| Log tag | none (nothing runs until `_LIVE`); `[JUNE_CHAR]` (`RENA NF`) from CH1 final |
| Review | `kickoffs/char-rena/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Open decisions (for the user)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1q - Done" "CH1q - Review"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Done (2026-10-27)" "CH1q - Review (2026-10-27)"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and `june2017_re/char_rena/ch0_nf.tsv`. Raven's CH1q (rage)
and Eve's Nasod Core are the worked examples.

## Task

- **Open decision 1 = (a)** (user, 2026-11-07): as Eve's Nasod Core, March's dormant `ADD_RENA_SYSTEM` code is the base, June's whole body
  wherever it differs.
- Port natural force from `ch0_nf.tsv`: `_NF` defines `ADD_RENA_SYSTEM` (only with `_LIVE`) with its `ServerDefine.h` second home
  (`CommonPacket.h` is reached before `Always.h`) and a `SECOND_HOMES` row in `tools/flagcheck.py`; then June's whole body inside every March NF function
  that differs: class tail `:23576-24004` (`CreateGageData`, `AttackResultByType`, `Init`, `IsHyperActiveSkill`), `X2GUUser.*`, the first-attack /
  relate-skill copies in `X2DamageManager` / `X2DamageEffect` / `X2EffectSet`, `X2GUNPC.*` incl. `AttackResultByType` `32773ee0`, `CX2RenaGageData` /
  `CX2RenaMyGageUI`, `X2GageManager`, `CAC_NATURAL_FORCE` and `KGamePlayStatus::GetNaturalForce()`.
- June-only pieces under plain names: `FriendshipNatureData` (RTTI `3373d6fc`), `32cb50c0` / `32cb6290( 5 )`, the `ApplyHyperModeBuff` Rena branch
  (`32298590 @322985ec`, checklist l.959).
- Resolve each June-port arm the flag flips to June's branch: `X2DamageManager.h:1386` / `:1663`, `.cpp:314` / `:2202` / `:3038` / `:3506`
  (`m_iNaturalForceType`, June `+0x56c`), `X2GUUser.h:1410`, `X2GUUser.cpp:10893`, `X2DamageEffect.cpp:5217` (`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`'s child
  sequence), `X2GUNPC.cpp:11404`. Drop the studio "NF:" overlay (`X2GUUser.cpp:29719`) and dev fill key (`X2Game.cpp:5855`) if June has none.
- The emulator: `CAC_NATURAL_FORCE` for Lire in `X2OfflineStatTable.cpp:289-313`; whether June keeps NF across a relog decides a save column (no schema
  change unless June's packet carries it; snapshot `db_backup/` first). Sub-tag `NF`; a temporary `_NF_DEBUG` for CH1 final's play-test lines. Split if
  `ch0_nf.tsv` is over ~45 functions (CH1q2: gauge UI and emulator).

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

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
