# June character Chung (Stage 7) - Phase CH1q - Cannon balls

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1q` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL` (+ `..._CBALL_DEBUG`); June bodies inside March functions also need `_LIVE` |
| Log tag | `[JUNE_CHAR]` (`CHUNG CBALL`; lines only once `_LIVE` is defined) |
| Review | `kickoffs/char-chung/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Context"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.
`june2017_re/char_chung/ch0_cannonball.tsv` is the work list.

## Task

Port Chung's cannon-ball system from `ch0_cannonball.tsv`: March's live code is the base, and every function that differs from June gets June's
whole body inside it (March's in `#else`); what June removed is dropped; June-only pieces are added: `CANNON_BALL_COUNT_AT_DUNGEON_START` /
`_PVP_START` (reader `32bf5430`, `INIT_PERSONAL_INFO`), the hyper-count-3 timer skip `32bf4680` (§7 l.236), `EXTRA_CANNON_BALL_DAMAGE_RATE` /
`USE_CANNON_BALL0` if CH0 found readers, the gauge (`CX2ChungGageData`, `CCannonBallUI`, `CX2ChungMyGageUI`, `PGUWO_ORB_CHUNG`) and June's
`DLG_Chung_Cannonball_NEW.lua` if June reads it instead of March's dialog. Check `ChangeMaxCharacterAddResource` (`X2GUChung.cpp:461`, June
`32bf4880`) still matches. `FIX_CANNON_BALL_SYNC_CALCULATION` sites: `cl /P` says which branch is live today. A `[JUNE_CHAR] CHUNG CBALL` line logs
the start count and each spend / gain (debug flag, retired in the review). If the map is over ~45 functions, split as `CH1q2` with a kickoff and
STATUS row.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: build passes with `_LIVE` undefined; the one-file probe with `_LIVE` defined compiles.

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

(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)
