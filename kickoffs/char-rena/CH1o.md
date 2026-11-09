# June character Rena (Stage 7) - Phase CH1o - The virtuals under LIVE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1o` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-rena/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1o - Done" "CH1o - Review"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_rena/ch0_*.tsv`. Raven's matching sub-phase is the worked example.

## Task

Wire the virtuals under `_LIVE`: June-only and changed user virtuals Rena overrides (vslots from `ch0_class_map.tsv`; 0x9c / 0xa0 / 0xa4 are not
overridden), `SetSkillLevelStateData` `:22990-23003` with the `( 3201ad10( skill ) + 1.0 )` term (checklist §7 l.286), and the gaps the CH1a-CH1k
reviews routed to this phase. Done gate: the build passes with `_LIVE` undefined and the one-file probe with it defined compiles; no play-test.

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

- **Functions: 5** (`awk -F'\t' '$7=="CH1o"' june2017_re/char_rena/ch0_class_map.tsv`; new 5).
  June addresses: `32c6dbd0`(new ?) `32c6dd10`(new ?) `32c6ddc0`(new ?) `32c83580`(new ?) `32c85b20`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L286 SetSkillLevelStateData hoisted into CX2GUUser; March copy X2GULire_Elv.
- **Callees this Opus phase maps itself** (`ch0_unmapped_identified.tsv`): `31c8a5c0` ? June-only: looks a name up in the mesh-player templet map (31cda540), then cop.
- Five class functions called from base code (31fef532 / 31fedfec / 31fca1be / 3229a79c): `32c83580` (+ `32c6dbd0`), `32c6dd10`, `32c6ddc0`, `32c85b20`: read each (Raven's `32c3d1c0` precedent) and give it a home. Plus `SetSkillLevelStateData` (§7 l.286, March `.cpp:22990`).

- **Routed from CH1h (2026-11-09): two user virtuals Rena overrides that March does not declare.** `ProcessAttackResultPassive` ( June `32ca9ca0`, user vslot 0x1e8, base `32011b80` ) and `ProcessEradicationHit` ( `32cb3200`, vslot 0x318, base `3208ddc0` = empty ) are written under `_CORE`, non-virtual, referenced by nothing. Add the two virtuals to the base class under the `_LIVE` gate and make them virtual in `X2GULire_ElvenRanger.h`. The 0x1e8 base body is CH4b's ( BTI 0x382 holder, 1 s timer, `EffectSet_Celestial_Blitz_Attack` ); its caller is `AttackResult` right after the `+0x66ac` run in `CEffect` / user / NPC ( funcmap `32011b80` ). The caller of 0x318 is unlocated ( no other class overrides it; June passes a `DamageData*` ): find it in `X2DamageEffect` / `X2GameUnit`. `32c6d870` ( vslot 0x1ec ) is a `jmp` to the base and needs no override. **CH1h review:** June's `ProcessAttackResultPassive` calls the 0x1e8 base first ( `@32ca9cc9`, `32011b80( pDamageData )` ); the `_CORE` body leaves that call out until the base exists, so add it when you make the function virtual.
