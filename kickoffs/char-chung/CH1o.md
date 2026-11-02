# June character Chung (Stage 7) - Phase CH1o - Virtuals under LIVE and the June-only user virtuals

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1o` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n3 review (CH1n split, 2026-11-02). One at a time: `X2GUChung.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Planning evidence (2026-10-31)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1o - Done (2026-10-27)" "CH1o2 - Done (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

Every vtable override CH0 marked `CH1o`, under `_LIVE`, and the gaps the CH1a-CH1l4 reviews routed here. Chung's June-only and changed user
virtuals: 7ap 0x6a8 `32bdba60` (skill use `32c17b82`), 7t 0xa4 `IsBerserkByChance` `32c24b90` (`+0x9714 > GetRandomFloat( 0 )`: identify `+0x9714`
from its writer; the base is `X2GameUnit.h` `IsBerserkByChance`), 0x9c `IsFullHyperMode` `32c24b50`, 0xa0 `32c24b60`. A user virtual Chung needs
whose base is not yet in `CX2GUUser` gets the base and its March call site first (Eve's CH1o2 shape); if that is more than ~4 virtuals, split it
as `CH1o2` and write its kickoff and STATUS row.

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

- **Functions: 24** (`awk -F'\t' '$7=="CH1o"' june2017_re/char_chung/ch0_class_map.tsv`; inside 8, new 16).
  June addresses: `32bdb760`(new ?) `32bdba10`(new ?) `32bdba60`(new ShownSkillCoolTime) `32bdc520` `32bdc600` `32bf0ce0` `32bf40e0` `32bfeb80`(new ?) `32c12910`(new ?) `32c23090`(new ?) `32c23600`(new -) `32c242c0`(new ?) `32c24520`(new ?) `32c245a0`(new -) `32c246f0`(new -) `32c24930`(new ?) `32c24b30`(new ?) `32c24b40`(new ?) `32c24b50` `32c24b60` `32c24b90`(new IsBerserkByChance) `32c24e50` `32c24ed0` `32c258a0`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L259 quick-slot special-item prefix at the top of SpecialAttackEventProcess; §7 L262 riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup; §7t L599 unit vslot 0xa4 override 32c24b90: +0x9714 > GetRandomFloat( 0 ), a be; §7t L610 L614 sound-set picks: Chung JuneIsBerserkByChance is one of the three overr; §7ap L969 0x6a8 JuneShownSkillCoolTime override 32bdba60 (skill use 32c17b82).
- June-only overrides named by CH0 (`ch0_hand_names.tsv`): `ShownSkillCoolTime` 0x6a8 `32bdba60`, `IsNotCommonMoveState` 0x828, `GetDefenderDamageRate` 0xb8, `UpdateSpineAimRotation` 0x774, `AfterInitEquippedActiveSkillState` 0x688, `StoreGageSnapshot` / `RestoreGageSnapshot` 0x740 / 0x744; `?` rows (0x670, 0x2e8, 0x2ec, 0x824, 0x1b8, 0x748) are named from the body by the class pattern.

## Amended by CH1l (2026-11-01)

- CH1l wrote the Chung bodies for the new user virtuals as plain `_CORE` members; this phase decides each user virtual and makes the Chung function an override: `IsInJobLine` (vslot 0x670, base `331a4950` returns false), `StoreGageSnapshot` / `RestoreGageSnapshot` (0x740 / 0x744, bases are the unported Crest of Time pair), `GetIncreaseSoulRateOnDamageReact` (0x748, base + `DamageReact` call are CH4b), `UpdateSpineAimRotation` (0x774, base `31f3f070` is an empty stub). `ScaleHead` (0x66c) already overrides the existing virtual under `_LIVE`.

## Amended by CH1l3 (2026-11-01)

- **User vslot 0x824** (base `3208dd70`, returns 1.0): add the virtual to `CX2GUUser`, call it from `ResetAnimSpeed` (`31ffe2b0`, arg bFuture) and drop March's inline Intense Showtime block there; `CX2GUChung::GetStateAnimSpeedRate` (`32bed8c0`) is written and called by nothing. Proof: `ResetAnimSpeed` with Chung at full hyper and the skill learned.
