# June character Lu/Ciel (Stage 8) - Phase NU3 - `UnitTemplet`: Lu/Ciel's rows and the June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel NU3` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_DATA` (rows), `SERV_IRUHADEV_JUNE_CHAR_MINOR_UNIT_TEMPLET` (parser) |
| Log tag | `[JUNE_CHAR]` (`LUCIEL DATA`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/NU3-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase NU - New-character foundation (NU1-NU4)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the NU3 row of "Phase NU". Lu/Ciel's 7 rows from `june_src_fixed/data036_kom/UnitTemplet.src.lua:1723-1979` go into the core-list
`UnitTemplet.lua` (every other row stays; `luasrc.py refresh`; pack, ask before copy). March's templet parser reads the 18 June-only keys into new
members, named after June's (NU0 item 5 gives June's parser addresses). The load line prints the row count and the major / minor body names. Gate:
`luasrc.py ledger --check` and `core --check` PASS; the `DATA` line shows 7 rows; no other class's templet changes (diff of the parsed values).

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

## Amended by NU0

NU0 (2026-11-02), `june2017_re/char_luciel/nu0_templet_keys.tsv`:
- **June's parser** is `CX2UnitManager::AddUnitTemplet_LUA` `3237aa40` (keys XOR-decoded: `june2017_re/xorstrings.tsv` gives each key's decode and
  getter VA). Templet offset = parser ESI offset - 4; templet 0x368 bytes. Getters: int `32fe9fd0`, wstring `32feae90`, float `32fea370`. March:
  `X2UnitManager.cpp:836`, struct `CX2Unit::UnitTemplet` `X2Unit.h:329-383`.
- **The 18 keys** (all scalars, no sub-table): `majorUnitName` +0x24 / `minorUnitName` +0x40 (STR_ID ints turned into wstrings by `31c31060`;
  `minorUnitName`'s length at +0x50 is June's "has a second body" test, `3254e410`, `32d85650`), `m_MinorUnitPromotion{Weapon,Body,Leg,Hand,Foot}ItemID`
  +0x1cc..+0x1dc int, `m_MinorUnit{Common,Field,Lobby,Emotion}Motion` +0x1e0 / +0x1fc / +0x218 / +0x234, `m_MinorUnitGameMotion0/1/2` +0x250 / +0x26c
  / +0x288 wstring, `m_fMinorUnitLobbyRotationY` +0x2d8 float, and the three cut-ins already parsed by `SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME`
  (`m_MinorUnitHyperCutIn`, `m_HyperCutIn_Trans`, `m_MinorUnitHyperCutIn_Trans`, `X2Unit.h:377-381`). So 15 new members, plus
  **`m_fLobbyRotationY`** (+0x2d4, every June row; March has none, comment :892-893): its reader `31edf260( unitClass, bMinor )` falls back to a
  per-class constant switch, so add it with the 15 (NU4's lobby preview reads it).
- Readers: `31fe5860` `InitMotion` (Emotion, GameMotion0-2: DU1's second body), `3254e410` (promotion texture preload), `32d85650` (names),
  `31edf260` (lobby rotation). No reader found for Common / Field / Lobby motion (parse them; DU1 / DU5 look again).
