# June character Rena (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH4a` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA PASSIVE`) |
| Review | `kickoffs/char-rena/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH4a - Done" "CH4a - Review"
```
Then the Rena lines of the checklist rows named below (`june2017_re/char_rena/plan_checklist_rows.md` gives the line numbers), the
`CH4a` rows of `ch0_*.tsv`, and Raven's matching CH4 Done note as the worked example.

## Task

`InitPassiveSkillState` `32ca78b0` (vslot 0x690, uncarved: `charport_adis.py rena 32ca78b0`) and Rena's passive ids (types 1 / 8: 3 + 21 rows), their objects
and readers; **D8** (checklist l.330-331): March's Powerful Bowstring 3405 and Sharp Kick 3400 blocks replaced by June's 23405 / 23400 together with the
reform skills (never one without the other); the delegate registrations (l.912-925) replacing March's own pushes; every March `SI_*` passive id read as
June's `RNSI_*` (plan Rules, CH0's list). The engine hooks they need (earlier characters' CH4 ported the delegate lists, the user passive hooks and June's
base `InitPassiveSkillState` `31fe19d0`): reuse them. Check the engine readers of each passive (a user state key may hide there).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Mark rows `(Rena: CH4a)`. Done gate: each passive's effect visible in game or in a named log line
(CH0 names the stat or log line per passive).

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

- **Functions: 15** (`awk -F'\t' '$7=="CH4a"' june2017_re/char_rena/ch0_class_map.tsv`; inside 1, new 14).
  June addresses: `32c6dac0`(new ?) `32c7af80`(new ?) `32c86790`(new ?) `32ca2180`(new ?) `32ca2390`(new ?) `32ca28b0`(new ?) `32ca78b0` `32cb4290`(new ?) `32cb5430`(new ?) `32cb57b0`(new ?) `32cb5970`(new ?) `32cb5dd0`(new ?) `32cb66e0`(new ?) `32cb66f0`(new ?) `32cb6700`(new ?).
- **Skills: 19** (part 2 of `ch0_state_skill.tsv`, col 6): 106 RNSI_P_COMMON_HYPER_AURA, 23400 RNSI_P_RCR_SHARP_KICK, 23401 RNSI_P_RWS_FRIENDSHIP_OF_NATURE, 23402 RNSI_P_RWS_SOFTBODY, 23403 RNSI_P_RWS_POWERFUL_SHOT, 23404 RNSI_P_RSR_HAWK_EYE, 23405 RNSI_P_RGA_POWERFUL_BOWSTRING, 23406 RNSI_P_RGA_COMMUNE_OF_NATURE, 23407 RNSI_P_RGA_VIBRATION_SHOOTING, 23408 RNSI_P_RTR_HUNTERS_ABILITY, 23409 RNSI_P_RNW_PRIOR_PLANNED_BLOW, 23410 RNSI_P_RNW_VITALPOINT_PIERCING, 23411 RNSI_P_RNW_SHARPEN_ARROW, 23412 RNSI_P_RWS_OPEN_MIND, 23413 RNSI_P_RWS_PERSISTENT_MIND, 23414 RNSI_P_RGA_OPEN_MIND, 23415 RNSI_P_RGA_RANDOM_SHOT_PLEASURE, 23416 RNSI_P_RNW_OPEN_MIND, 23417 RNSI_P_RNW_PREDATOR.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7b L331 D8: Rena Powerful Bowstring 3405 -> 23405 and Sharp Kick 3400 -> 23400; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7am L912 delegate-list registrations in InitPassiveSkillState: row says Rena 32; §7am L923 NPC-side delegate 3275a490 (owner UT_LIRE -> Rena's 32ca2180); registr; §7am L926 March's own pushes are replaced by June's set.
- Delegate registrations are in InitPassiveSkillState `32ca78b0` (thunks pushed @32ca7d17 / @32ca7e83 / @32ca8111 / @32ca8169; checklist 7am names `32ca7860`, which is Win): A `32ca2180` (also NPC `3275a490`), `32ca2390`, `32ca28b0`, `32c86790` (+ `32cb5430`, `32cb57b0`, `32cb5970`, `32cb4290`), stubs `32c6dac0`, `32c7af80`, `32cb5dd0`, `32cb66e0`-`32cb6700`. **Passive ids:** `ch0_passive_ids.tsv` (72 live March `SI_*` reads; the June-id column is a title match, verify each): D8 `SI_P_LCR_SHARP_KICK` 3400 -> `RNSI_P_RCR_SHARP_KICK` 23400 and `SI_P_LSR_POWERFUL_BOWSTRING` 3405 -> `RNSI_P_RGA_POWERFUL_BOWSTRING` 23405 (4 live sites each).

- **Amended by CH1j (2026-11-09): members and readers CH1j declared.** `m_bBowSkillState` ( June +0x981d; CommonStateStart sets it from the key `BOW_SKILL_STATE`, CommonStateEnd reads it ): `GetActualCoolTime` `32c7be00 @32c7be7b` reads it ( CH4b ). `m_usRandomRollCount` ( +0x981e; CommonFrameMove clears it ): `32c7af80` takes `GetRandomFloat( 0x10 + count )` and counts it up ( Hunter's-ability style trap roll, skill 0x5b72 ). `m_bPredatorCritical` ( +0x97f0; CommonStateStart sets it from Predator's roll ): its readers are `32ca2390` and the damage code ( identify ). `m_mapRageFungusTargetLockTime` ( +0x97f4, `std::map< std::pair< int, UidType >, float >` ): `32c86790` ( Rage Fungus ( 23042 ) hit target -> value 1.0, insert or assign; called by `32cb5dd0` from InitPassiveSkillState ) fills it, CH1o's `32c85b20` sweeps it. **Check, not CH1j's call:** the Predator stage helpers `GetPredatorDamageStage` / `GetPredatorDamageMultiple` ( `32c83280` / `32c6d800` ) and whether skill 23417 reads other abilities.
