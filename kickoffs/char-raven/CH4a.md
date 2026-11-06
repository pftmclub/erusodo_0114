# June character Raven (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4a` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN PASSIVE`) |
| Review | `kickoffs/char-raven/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4a - Done" "CH4a - Review"
```
Then the Raven lines of the checklist rows named below (`june2017_re/char_raven/plan_checklist_rows.md` gives the line numbers), the
`CH4a` rows of `ch0_*.tsv`, and Elsword's matching CH4 Done note as the worked example.

## Task

`InitPassiveSkillState` `32c57660` (vslot 0x690) and Raven's passive ids (types 1 / 8: 10 + 27 rows), their objects and readers; checklist
7b Burning Nasod Hand whole (the 24401 setter `+0x9688` here, the `+0x4108 = 2` write `32c43120 @32c43ac4` if CH1 did not take it: never one
without the other and June's `AT_COMMAND` tags; MODS.md l.2557-2559); 7am's 3 delegate callbacks (`32c3df50`, `32c4e5b0`, `32c28160`) replacing
March's own pushes; 7an vslot 0x73c `32c3c7f0` and 0x748 `32c3c820` (soul gain when hit; the base and its `DamageReact` call are Chung CH4b's),
both uncarved (`charport_adis.py raven`), unless CH1i took them; every March `SI_*` passive id read as June's `RNSI_*` (plan Rules). The engine
hooks they need (Aisha's / Eve's / Chung's / Elsword's CH4 ported the delegate lists, the user passive hooks and June's base
`InitPassiveSkillState` `31fe19d0`): reuse them. Check the engine readers of each passive (Elsword's CH4a2 found a user state key).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Raven: CH4a)`. Done gate: each item's effect visible in
game or in a named log line.

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

## Amended by CH0 (2026-11-05)

- **Functions: 7** (`awk -F'\t' '$7=="CH4a"' june2017_re/char_raven/ch0_class_map.tsv`; inside 1, new 6).
  June addresses: `32c28160`(new ?) `32c3c7f0`(new ?) `32c3c820`(new ?) `32c3df50`(new ?) `32c57660` `32c68a40`(new ?) `32c69e10`(new ?).
- **Skills: 23** (part 2 of `ch0_state_skill.tsv`, col 6): 106 RNSI_P_COMMON_HYPER_AURA, 24400 RNSI_P_RF_PHYSIC_MAGIC_ATTACK, 24401 RNSI_P_ROT_BURNING_NASOD_HAND, 24402 RNSI_P_RRF_EXPLOSIVE_NASOD_HAND, 24403 RNSI_P_RRF_CONFIDENCE_VICTORY, 24404 RNSI_P_RFF_HARDEN_BODY, 24405 RNSI_P_RRF_SPIRITUALIZED_FURY, 24406 RNSI_P_RST_EXQUISITE_SWORDMANSHIP, 24407 RNSI_P_RBM_BLADE_MASTERY, 24408 RNSI_P_RBM_PRIDE_VICTORY, 24409 RNSI_P_RBM_SURELY_VICTORY, 24410 RNSI_P_RBM_EMERGENCY_ESCAPE, 24411 RNSI_P_RWT_BEHAVIOR_OF_MERCENARY, 24412 RNSI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY, 24413 RNSI_P_RVC_REVERSAL_SOLDIER, 24414 RNSI_P_RVC_SURVIVAL_TECHNIQUE_OF_MERCENARY, 24415 RNSI_P_RVC_OVERHEAT_MODE, 24417 RNSI_P_RBM_OPEN_MIND, 24418 RNSI_P_RBM_COUNTER, 24419 RNSI_P_RRF_OPEN_MIND, 24420 RNSI_P_RRF_INFINITY_RAGE, 24421 RNSI_P_RVC_OPEN_MIND, 24422 RNSI_P_RVC_WIN_AND_WIN.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §5 L184-198 key BURNING_NASOD_HAND_POSSIBLE x24; §7b L327 Burning Nasod Hand: 32c57660 (+0x9688 = skill 24401 learned); 32c43120; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7am L909 delegate-list registrations in InitPassiveSkillState: Raven 32c57660 p; §7an L933 vslot 0x73c / 0x748 overrides: Raven 32c3c7f0 (0x73c) and 32c3c820 (0x.
- **Passive ids:** `ch0_passive_ids.tsv`: 32 live March `SI_*` reads whose June id differs (e.g. `SI_P_RRF_EXPLOSIVE_NASOD_HAND` 4410 -> 24402, `SI_P_RVC_OVERHEAT_MODE` 4418 -> 24415, `SI_A_RF_SHADOW_STEP` 4205 -> 24200); 20 more renamed under another class tag (`SI_P_RRF_BURNING_NASOD_HAND` 4408 -> `RNSI_P_ROT_BURNING_NASOD_HAND` 24401). Burning Nasod Hand: the 24401 setter here pairs with CH1j's +0x4108 write. 0x73c `32c3c7f0` / 0x748 `32c3c820` (uncarved) are here (7an).
- **Amended by CH1j (2026-11-06):** `InitPassiveSkillState` `32c57660` writes `+0x9688` = `m_bBurningNasodHandLearned` (CH1j's `CommonStateStart` `32c43380` reads it with `BURNING_NASOD_HAND_POSSIBLE`; with 24401 not learned the Burning Nasod Hand damage-type change never applies). `32c3df50` stores SA `0x94` of 24409 (`RNSI_P_RBM_SURELY_VICTORY`, not yet in `X2SkillTree.h`) into `m_fSurelyVictoryCoolTime` (`+0x959c`, counted down by CH1j's `CommonFrameMove`) and zeroes `+0x9598`: confirm that pairing. June's `CommonFrameMove` has no inline Survival Technique of Mercenary block (March `:5817-5854` is dropped under `_LIVE`): check `CheckAndDoServivalTechniqueOfMercenary` (March `:27274`) against June's 7am delegate and `m_bActiveSurvivalTechniqueOfMercenary` / `m_bEnableSurvivalTechniqueOfMercenary`.
- **Amended by CH1k review (2026-11-06):** June's ctor `32c3ece0` stores nothing for March's `m_fChanceOfExplosiveNasodHand` (March ctor `.1f`) or `m_fAttackPowerRateShadowStepPunisher` (`1.f`); March's `InitPassiveSkillState` resets both (0.f / 1.f, `X2GURaven.cpp:2412` / `:2450`) before any reader. When June's `32c57660` replaces it, decide whether each member survives in June (find its offset in `32c57660`, or drop it with its readers) and name the offset in `ch1_offsets.tsv`.
