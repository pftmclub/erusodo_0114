# June character Ara (Stage 7) - Phase CH4b - Overrides and the memo

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH4b` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH4a. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE` (+ `..._PASSIVE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA PASSIVE`) |
| Review | `kickoffs/char-ara/CH4b-review.md` (Opus 5.5 high; the phase writes it when done); the CH4 stage review: covers CH4a-CH4b, retires `_PASSIVE_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --list   # then the CH4b Done note once Rena has one
```

## Task

Do CH4b: `GetActualMPConsume` 0x6a0 `32b96730` (uncarved); 0x3ac `32ba45d0` (March's live 4-arg `ModifyFinalDamageByGameUnitType` body moves to June's
override, checklist l.991) and 0x6a8 `32b958e0` (also called from its skill use `32bcff21`); 0x428 `32b956a0` (l.940); vslot 485 socket pass `32badc80`
with its list `32d3f040` (l.284: override the empty user hook, call `AddJuneSkillAdditionalPowerRateBySlot`, never edit `SumSocketOption`); the
effect-set hooks 0x4c8 `32bc1530` (l.424); `MemoConditionAra` (RTTI `3373d604`, vtable `33502680`, `32ba46f0`) and the memo readers (`APPLY_MEMO`,
`EFFECT_SET_LIST_EX_MEMO`); `P_AYR_GOOD_ENERGY_BALL` on CH2c0's path; §7 l.281 power-rate socket range if an Ara function calls `3201ad10`.
Play-test; then the CH4 stage review covers CH4a-CH4b. You are on Opus: delegate the reads, probes and builds.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 9** (`awk -F'\t' '$7=="CH4b"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 2, new 7).
  June addresses: `32b955c0`(new ?) `32b956a0`(new GetDefenceRateBySkill) `32b958e0`(new ShownSkillCoolTime) `32b96730` `32ba45d0` `32ba46f0`(new ?) `32ba47f0`(new ?) `32badc80`(new SumSocketOptionAfterSlots) `32bc1530`(new ApplyEffectSetScaleBySkill).
- **Checklist rows: 6** (`awk -F'\t' '$5 ~ /(^| )CH4b( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L237 buff hyper count readers / writers: 32ba6cc0 (vslot 39), 32b; L284 vslot 485 override 32badc80 (Ara vtable 33505314): skill-gro; L328 skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0 ; L424 EffectSetToPlay +0x34 / +0x38 and user slots 0x4c4 / 0x4c8 a; L940 vslot 0x428 JuneGetDefenceRateBySkill: Ara 32b956a0 (the ove; L991 Ara: 0x3ac JuneModifyFinalDamageByGameUnitType 32ba45d0 (mov.
- **CH-N (class path) rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH4b( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **Identified callees first needed here: 2** (`awk -F'\t' '$8 ~ /(^| )CH4b( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `31fca980` `32d3f040`.
- 0x300 `32ba47f0` CEffect hook (skill 27411: effect +0x3f0 x (100 - `+0x9740`) x 0.01 when `+0x973c` != 0). MemoCondition `32b955c0` / `32ba46f0`.

## Amended by CH4a (2026-11-12)

- **Members CH4a writes, CH4b reads.** `m_fSpiritualEyeDefenceRate` ( `X2GUAra.h`, June `+0x9748`, ctor -1, Asura's Spiritual Eye 27414 `SA_UNFIXED_DEFENSE_REL` ) is what `GetDefenceRateBySkill` 0x428 `32b956a0` returns when above 0; `m_fEnergyReinforceForceDownPercent` ( June `+0x9740`, ctor -1, 27411 `SA_FORCE_DOWN_MELEE_REL` ) is the `+0x9740` of the CEffect hook 0x300 `32ba47f0`, gated on `m_fMpGainPerForcePowerInFullHyperMode` ( `+0x973c` ). Both are written by `InitPassiveSkillState` under `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE`; CH4b uses that flag ( or widens it ) for the readers.
- **The CH4 stage review** also covers CH4a: retire `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE_DEBUG`, re-read `32bcd9c0`, `32bca010`, `32ba6440` and the four delegates from the exe, and give `X2GUUser.*` ( `ProcessPassiveReturnFromDeath`, the `ProcessSkillAtDie` Ara block ), `X2GameUnit.*` ( `ReserveToFinishDebuffTempletsFromGameUnit` ) and the enum headers their own verdict lines.
- **User report ( 2026-11-12, CH4a play-test ): Yama Raja's energy ball in her command attacks looks wrong.** It is mostly black with a faint yellow line; the user expects a yellow core before it explodes. Investigate first: which damage effect / effect set the command-attack energy ball plays ( CH2c's `32bc2900` projectile set, Demigod's divided set, Open Mind's spirit ball `+0x44c` life in `OnDamageEffectCreated` ), whether its mesh / texture / `.tet` is June's ( `kom.py stage-june` if March's same-name asset is still there ), and whether a render or colour key ( alpha / color factor in `EffectSet.lua`, a June-only CEffect key logged as `JUNE_FX unsupported` ) is dropped. Fix in this phase if it is Ara's; otherwise route it.
- **Conversion Health branch of the 0x780 hook not seen in the CH4a logs.** Unit 48 hit with `DAMAGE_EFFECT_A_ALD_CONVERSION_HEALTH` ( June data `HIT_ADD_HP_PERCENT = 0.06`, `APPLIED_SKILL_ID = 27210` ), but no `conversion health hp recovery` line. Check the hook's DamageData for that hit ( `m_iAppliedSkillID`, `fHitAddHPPer` ) and the `HP_RECOVERY` row lookup, in the stage review.
- **Carried from the CH4a re-test ( 2026-11-12 ):** `weird fox thunder` confirmed ( critical rate 0.150 x3 ). Still unseen, test them in CH4b's play-test: `quick piercing debuff time` ( unit 104 slot 7 = 27241; June's hook tests the applied id 27217 only, so first check which `APPLIED_SKILL_ID` the upgrade's attacks carry in June's data: if it is 27241, no line is June's behaviour ) and `resurrection` / `resurrection gate fired` ( unit 48, `/invin 0`, 5+ force power, take a lethal hit ).
