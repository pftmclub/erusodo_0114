# June character Ara (Stage 7) - Phase CH4a - Passives, the 0x780 hook and delegates

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH4a` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3d review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE` (+ `..._PASSIVE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA PASSIVE`) |
| Review | none of its own: the CH4b review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH4a - Done (2026-11-10)"
```

## Task

Do CH4a: `InitPassiveSkillState` `32bcd9c0` (vslot 0x690) whole on June's ids and Ara's passive ids, their objects and readers; the passive /
attack hook 0x780 `32bca010` whole (checklist l.330; Ara reaches it instead of a 0x2b8 override); the delegate registrations (l.920: `32bcd9c0`,
`32bb0d10`) replacing March's own pushes (l.930); death prevention with 27409 (l.258, the `32bc5a50` test, in `CX2GUUser::UpNowHp` / `SetNowHp`: widen
the existing gate); `GetReduceDebuffDurationRate` `32bd32f0`'s 27415 tail (l.267, the `SERV_IRUHADEV_JUNE_BUFF_GROUP` block); the plan's passive-id
check. Play-test per passive (CH0 names the stat or log line). No review of its own: CH4b's covers it; `_PASSIVE_DEBUG` stays on. You are on Opus:
delegate the reads, probes and builds.

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

- **Functions: 17** (`awk -F'\t' '$7=="CH4a"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 5, new 12).
  June addresses: `32b95910`(new ProcessDemigodHitCount) `32b95980`(new MarkRestrictionEnergyHit) `32b959a0`(new ProcessGainGoodEnergyBallOnHit) `32ba6440`(new ?) `32ba7e40`(new ProcessAsuraOpenMindOnHit) `32baf810` `32bb0d10` `32bc5a50`(new ?) `32bca010`(new DamageExceptionClassHook) `32bcd9c0` `32bd32f0` `32bd5470`(new ?) `32bd5480`(new ?) `32bd5490`(new ?) `32bd6b10`(new ?) `32bd7a90`(new ?) `32bd7c00`.
- **Skills: 22** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 106 RNSI_P_COMMON_HYPER_AURA, 27400 RNSI_P_ALH_FILLED_POWER, 27401 RNSI_P_ASD_VIGOR_ENERGIZE, 27402 RNSI_P_ASD_FULLMOON_SLASH, 27403 RNSI_P_ASD_POWER_OF_TIGER, 27404 RNSI_P_ASD_BRUTAL_PIERCING, 27405 RNSI_P_ALD_ENERGY_BULLET_REINFORCE, 27406 RNSI_P_AYR_ENERGY_REINFORCE, 27407 RNSI_P_AYR_HOWLING, 27408 RNSI_P_AYR_HELL_GATE_OPEN, 27409 RNSI_P_AYR_RESURRECTION, 27410 RNSI_P_ALS_FILLED_POWER, 27411 RNSI_P_ASR_ENERGY_REINFORCE, 27412 RNSI_P_ASR_LOAD_OF_ASURA, 27413 RNSI_P_ASR_FOX_SMILE, 27414 RNSI_P_ASR_SPIRITUAL_EYE, 27415 RNSI_P_ASD_OPEN_MIND, 27416 RNSI_P_ASD_VALOR_DEVOTION, 27417 RNSI_P_AYR_OPEN_MIND, 27418 RNSI_P_AYR_DEMIGOD, 27419 RNSI_P_ASR_OPEN_MIND, 27420 RNSI_P_ASR_SPIRITISM_WEIRD_FOX.
- **Checklist rows: 7** (`awk -F'\t' '$5 ~ /(^| )CH4a( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L258 death prevention (Ara pieces): skill 27409 learned with +0x8; L267 CX2GUAra::GetReduceDebuffDurationRate 32bd32f0 @32bd339e-32b; L330 Ara's vslot 0x780 32bca010 (27415 RNSI_P_ASD_OPEN_MIND / 274; L713-L745 an id of a listed type with no case plays the class's first ; L920 delegate-list registrations in InitPassiveSkillState: Ara 32; L930 March's own pushes (Elsword, Raven, Ara, Elesis, Add) are re.
- **Force-power rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH4a( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 1, june-only 1).
- **CH-N (class path) rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH4a( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **Identified callees first needed here: 7** (`awk -F'\t' '$8 ~ /(^| )CH4a( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `3221e930` `32225030` `3226fce0` `322a97f0` `327c67f0` `32ed3950` `32282190`.
- 0x29c `32bd32f0` `GetReduceDebuffDurationRate` (27415 tail, l.267); 0x328 `32ba6440` float hook (27417: FP-gated roll adding skill value 0x1f; 27420: count on state skill 27216). `ch0_passive_ids.tsv`: 65 March `SI_*` reads, 41 differ (March 7000s vs June 27000s), 9 matched across class tags (e.g. `SI_SA_ALH_PEERLESSNESS_SLASH` 7005 = `RNSI_SA_ASD_PEERLESSNESS_SLASH` 27012), 12 dead branches. `ch0_enum_names.tsv`: 136 of 139 June ids need registering.
- **Amended by the CH1b review (2026-11-10):** June never sets `+0x9640` `m_bAbleQuickStand` (only the ctor `32bc6bf0` zeroes it; no class function queries 7400 `SI_P_ALH_QUICK_STAND`): June's `InitPassiveSkillState` body drops March's quick-stand setter (`X2GUAra.cpp:1874-1886`), so the DamageDown quick-stand arms are dead, as in June.
- **Amended by CH1o (2026-11-11):** Quicken Guard (Sakra Devanam, `m_fBlockingRate` / `m_fTurningRate`) is rolled in `CX2GUUser::DamageReact`: June's gate `@32047441` is `attackType != AT_SPECIAL` (`AT_COMMAND` passes), March's `X2GUUser.cpp:10297` is `== AT_NORMAL`. Compare that block (`@320473f8`-`@3204759e`, getters `32b92fa0` / `32b92fb0`) with March's and port June's gate with the passive.
