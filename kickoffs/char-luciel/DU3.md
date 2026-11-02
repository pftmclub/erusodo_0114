# June character Lu/Ciel (Stage 8) - Phase DU3 - June's equip-view rewrite

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel DU3` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EQUIP_VIEW` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL DUO`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/DU3-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase DU - The two-body engine (DU1-DU5)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the DU3 row of "Phase DU": the whole set, for every class (Chung / Ara / Arme overrides included), with Lu/Ciel's override from the class
map. The funcmap rows `Char Chung CH1l0` and the divergence rows marked `stage8` are the starting list; mark those rows done. Play-test: Lu and Ciel
each show their own equipment; three March characters' fashion over a broken item and a PvP item look as June's.

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

NU0 (2026-11-02), `nu0_equip_nesi.tsv` rows `DU3` and `nu0_duo_engine.tsv` rows `DU3`: `3201b930`, `31fd2f00`, `31ffe990` (form 1 builds Ciel's
look), `31fd48a0` (Now anim for a form, vslot 0x570), `31fd63c0`, `31fd9200` / `31fda280` (the `MAJOR:` / `MINOR:` equip dump; labels rest on
strings), `328825c0`, `32882540`. `31fd48a0`, `31fd63c0`, `32882540` have no March symbol. EP 28: June's second ring goes to `EP_AC_RING2` (28), March's
`EP_AC_TEMP4` has the value: register the name (`321693b0` is DU5's, it uses it).

## Amended by CH0 (2026-11-02)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7g L407 item record +48 m_ItemIDForMinorUnit (429 Lu items -> Ciel dummy, 30344 -> 20003; §7g L408 item flags2 bit 2 m_bIsForMinorUnit (2,582 Ciel dummies); §7g L409 Extra accessors 32867660..32867960: unit type 10 -> key 10 (Lu) or 11 (Ciel) by ; §7z L683-688 CX2Eqip::_UpdateSkin 3295ae20 attaches every model for owner unit type 10 when t; §7aa L690-697 minor-unit attach-anim models: 3295dfa0 @3295e2cf-3295e4f6 builds the second ani; §7aa L698-701 UpdateEquipCollisionData pair (329657d0 reads basic items) is every-class, not L; §7as L994-1003 upgrade-weapon glow type 10 cases: InitUpgradeWeaponParticle 32299730, ProcessUp.
