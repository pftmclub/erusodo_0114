# June character Eve (Stage 7) - Phase CH3a2 - Eve summon setup: June's `SetUserSummonedNPCInfo` cases

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH3a2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH3a review (added by it 2026-10-28). One at a time: `X2GuEve.*`, `X2Game.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |
| Review | `kickoffs/char-eve/CH3a2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3a - Review (2026-10-28, Opus 5.5): PASS after one fix"
```
Then `june2017_re/summon_npc_skill_map.tsv` (Eve rows), the funcmap rows for `322ece70` (lines with `322ece70` in column 1: flag meanings
0x1 / 0x2 / 0x4 / 0x8 / 0x10 / 0x20 / 0x40 / 0x200 / 0x400 / 0x800) and `3230c1a0`, and the model: `CX2GUEve::June_SetAtomicShieldSummonInfo`
(`X2GuEve.cpp`) with its call in `X2Game.cpp`'s `case CX2UnitManager::NUI_ATOMIC_SHIELD_EVE`.

## Task

The CH3a review found that March's `CX2Game::SetUserSummonedNPCInfo` cases for Eve's summons read the old `SI_*` skill ids (e.g. Atomic
Shield read `SI_SA_ENS_ATOMIC_SHIELD` 5008). The June skill tree equips `RNSI_*` ids, so `GetEquippedActiveSkillTemplet` finds nothing and the
summon gets default stats, power rate and scale. June's `3230c1a0` reads `31fcf430( 31fcf520( npcId ) )` (the summoning skill after the
upgrade swap) and calls `322ece70( npc, user, flags, 1.0 )` with a per-case flag word.

Port June's case, whole, for each remaining Eve summon (CH3a review did 682 Atomic Shield):
- 384 / 519 `NUI_NASOD_WATCH_EVE` / `_MEMO` (25000 Dimension Link Sentry, CH2a; funcmap flags 0x240), March case reads `SI_SA_EN_DIMENSION_LINK` / `SI_A_EN_DIMENSION_LINK`.
- 411 / 520 `NUI_GUARDIAN_SP2_EVE` / `_MEMO` (25013 Sonic Wave, CH2b), March reads `SI_SA_EAT_SONIC_WAVE`.
- 1621 `NUI_SI_SA_AT_SURFACE_CUTING` (25015, CH2b), 1622 `NUI_SI_SA_EEP_ELECTRONIC_FIELD` (25020, CH3b's skill), 2091 `NUI_ECA_OBERON_GUARD` (25014, CH2b): no March case at all.
- 1454 `NUI_SI_HA_FERDINAND` (25601, CH3d's skill; flags 0x260), March reads `SI_HA_EEP_LINK_OVERCHARGE_ILLUSION`.

For each: read the case in `3230c1a0` (jump table / compare chain from `@3230c3ae`) and every flag path of `322ece70` it takes; one
`CX2GUEve::June_Set<Summon>SummonInfo` member per case (or one shared member when two cases have the same listing), called from the March case
under the flag with March's body in `#else`, or from a new case where March has none. Reuse what exists: `June_TryGetEquippedSkillAbilityValue`
(32007ce0), `June_GetLearnedSkillAbilityValue` (32007b10), `June_GetSkillLevel` (31fceab0), `GetSummonPowerRateBySocket` / `ByBuff`, the
0x200 number store (`JuneStoreSummonGroupPowerRateNumber`, `X2Game.cpp`), and `SetRealatedSkillLevel` (NPC `+0x8e74`, written by every
`322ece70` call). A summon whose case does something the map does not settle: "this needs Opus", stop.

Done gate: one `[JUNE_CHAR] EVE SKILL ... npc` log line per summon with its skill id, level and stats; each summon has June's size and
damage in a play-test (save unit 59 and the CH2 units, ask for slot swaps).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.
