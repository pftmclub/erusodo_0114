# June buff port - Phase 16 step 3c - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3c review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3c done (see its STATUS row for the commit) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_EDT` (needs `_SOCKET_PARITY`); the three `X2BuffTemplet.cpp` compares are under `SERV_IRUHADEV_JUNE_DAMAGEDATA` |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3c — Step 1, read-only (2026-10-07, Opus 5.5 high)" "Phase 16-step3c — Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3c" carry the decodes (`32726850 @3272926f`, `32034e50` / `327995c0` EDT cases,
`3203c9a0 @3203ce96`, `31e5a860`, `322d4f80`, `32430b80`, `32070e40`, `322a0090`, `32893e00`).

## Task

Review 16-step3c against June:
- **Loader routing** (`32726850 @3272926f`, byte table `32737230`, jump table `3273721c`): 150/159 to the second list
  (`+0x384`), the other 16 to `+0x374`. Nothing else in the `EXTRA_DAMAGE` block changed.
- **The second list's carriage**, five places: the fold (`3204f190 @3204f874`), `InitSocketData` (`3206e917`), the base
  `ReInit` (`322a6071`), the `SetDamageExceptionProcess` copy (`32045340 @3204540d..32045483`: its condition is
  `0 < magic || 0 < physic`, a user attacker, the hit's list empty), and both tails (user `@3203ce96` with per-entry
  `31fe6db0`; NPC `@3279dbba` with `32760210` over the list at the top). Check the roll offset (`SRO_ATTACK_EXTRA_DAMAGE_MULTIPLE` = 4, June `i + 4`)
  and that both tails run for every attack type (June jumps to them on `AT_SPECIAL`).
- **The cases** in `CX2GameUnit::JuneProcessSocketExtraDamage`, case by case against both switches (`320399a8`/`320398a8`,
  `3279d574`/`3279d470`): each effect-set name and its position, rotation and direction source; 145-147's code events
  (2119 / 2154) and the user-only 146/147; 149's scale (`vslot 0x24` = `+0x686c`, taken as `m_vBasicScale` from the June ctor
  order `@322a4f8b`: confirm it is the first of the two scale members); 150/159's BFIs, creator and level; 151's order
  (count 762 on the attacker, erase, magic then physic `*=` `m_DamagePerSec`, else BFI 788 at the level) and its user-only
  `IsMyUnit` gate (`31ff1f30`); 152-155 on the NPC only, at `GetLandPos` (NPC `vslot 0x134` `327559c0`).
- **`GetCalcPerDamage`** (`32622300`): only cases `0x97`/`0x9c` return the raw value; March's other cases already match.
- **The three `X2BuffTemplet.cpp` compares**: the trigger values 3, 6 and 7 (`@3236df5b`, `@3236e306`, `@323704cb`), and that
  March's `DTT_MARK_OF_COMMANDER` really is 3 in this build (`cl /P`: `EVE_ELECTRA` and `SERV_ADD_CHUNG_SHELLING_GUARDIAN` both on).
- `JUNE_BUGS` rows 67-69: right reading, right reach.

Hygiene is one command per flag:
`python tools/flagcheck.py JUNE_SOCKET_EDT --range <first 3c commit>^..HEAD` and
`python tools/flagcheck.py JUNE_DAMAGEDATA --range <first 3c commit>^..HEAD --files X2Lib/X2BuffTemplet.cpp`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line;
a FAIL or ERROR is a finding.

Spot-check 2-3 cases directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "Phase 16-step3c — Review" into the plan doc, update the STATUS.md row, check `16-step3c2.md`, and commit.
