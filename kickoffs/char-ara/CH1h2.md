# June character Ara (Stage 7) - Phase CH1h2 - Class core: AttackResultByType `32bceca0` (CH1h's routed row)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH1h2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1h (done). One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (the body), `_FP` (blocks that call CH1q's pieces), `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (new, `//#define`, needs `_LIVE`: the skill-passive blocks), `_SPECTER` (new, `//#define`, needs `_LIVE`: the Specter of Thunder block) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1k review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1h - Done (2026-11-11)"
python june2017_re/charport_adis.py ara 32bceca0        # the whole listing (545 lines)
awk -F'\t' '$1=="32bceca0"' june2017_re/funcmap.tsv
```

## Task

CH1h wrote every row of its group except `AttackResultByType` `32bceca0` (override 0x4e0, `inside` row): March's `CX2GUAra::AttackResultByType` (end of `X2GUAra.cpp`, after
CH1h's `_CORE` helpers) keeps its base call and the Gain Force intake. Write June's **whole** body inside it under `_LIVE` (March's body in `#else`), blocks in June's order:
1. Base `CX2GUUser::AttackResultByType`; `DamageData` has trigger `0x44` (`JuneIsAttackTrigger`, `X2DamageManager.h`) and the passive `0x6b18` (`RNSI_P_ASD_VALOR_DEVOTION`) is equipped: force-power
   gain from `GetLearnedSkillAbilityValue`, the overflow turned into MP through the protected int `+0x9644` (`m_iGainMpByFpConversion`) = CH1q's inline `UpForcePower`: under `_FP`, calling the
   declared `UpForcePower`; then `0x6b19` ( `RNSI_P_AYR_OPEN_MIND` ) learned -> user vslot `0x23c` (`SetSkillBuffFactors( 27417, 0, true )`).
2. Gain Force intake states `0x6a40` / `0x6a54` (`RNSI_A_ARA_GAIN_FORCE` / `_UPGRADE`): `+0x96ec` counts hits whose `DD +0x14` equals `+0x96e8` (`ch1_offsets.tsv` `unnamed_96e8`, `unnamed_96ec`).
3. `ProcessFullHyperHitReward( DD )` (`32bc9a60`, CH1q: declare `// June 32bc9a60: body in CH1q`, call under `_FP`).
4. Specter of Thunder block (state byte `+0x95` of `GetStateparam( false )`, cool time `+0x979c`, chance `m_fSpecterOfThunderRate`): `EffectSet_SpecterOfThunder_Buff_Damage` at the hit position: under `_SPECTER`.
5. Get Blood `0x6992` (`RNSI_SA_ASR_GET_BLOOD`), Raid `0x6a52` (`RNSI_A_ASR_RAID`), Mana Drain `0x6a50` (`RNSI_A_ALS_MANA_DRAIN`: MP drain from the defender, `322d39e0` draw-MP-loss text
   = a divergence row, the `EffectSet_SI_A_ALS_MANA_DRAIN_HIT` set and the `SI_A_ALS_MANA_DRAIN_10` particle): under `_SKILL`.
6. `UpForcePowerBySkillHit` (`32bab780`, CH1q): declared, called under `_FP`.
The listing's offsets `+0x96e8 .. +0x97d8` are the `unnamed_*` rows of `june2017_re/char_ara/ch1_offsets.tsv`: name each from its use here, declare it under `_CORE` with the ctor value that row gives and
add it to Ara's constructor. Register missing enum names (`RNSI_*`, `SA_*` 0x2c / 0x42 / 0xbf, the attack-trigger 0x44) under `JUNE_ENUM_NAMES` after checking `Enum.src.lua`; an unmapped engine
callee is "this needs Opus": stop and say so. June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); Done gate as CH1h's (one-file probes off / `_LIVE` / `_LIVE` + `_FP` + `_SKILL` +
`_SPECTER`, `flagcheck`, `junediverge`, `enumscan`; no full build: no play-test).

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara CH1h2` comment (at the start of an existing one-line comment, never a second line).
