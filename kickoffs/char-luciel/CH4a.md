# June character Lu/Ciel (Stage 8) - Phase CH4a - Passive state and delegate lists

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH4a` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2d2 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_PASSIVE` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL PASSIVE`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH4a-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH4a` row of "Phase CH4" from the class map. The Done note names, per passive, the stat or log line that shows it.

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

## Amended by CH0 (2026-11-02)

- **Functions: 15 to transcribe** (+ 9 thunks) (`awk -F'\t' '$6=="CH4a"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae5080` UnSetSpecificValueByBuffTempletID `32b0fc50` `32b0fce0` `32b13c80` `32b177b0` `32b17f90` `32b34fe0` StartTransformedForm `32b37d40` SetSpecificValueByBuffTempletID `32b3ca30` CreateFearDamageEffect `32b3d380` `32b3e470` `32b43590` `32b435f0` PlaySoulExtortionDrain `32b45820` InitPassiveSkillState `32b4caf0`.
  Still `?` (name it by the class pattern from the hint in col 7): `32b0fc50` `32b0fce0` `32b13c80` `32b177b0` `32b17f90` `32b3d380` `32b3e470` `32b43590` `32b4caf0`.
- **Skills: 35** (part 2 of `ch0_state_skill.tsv`, col 6): 106 RNSI_P_COMMON_HYPER_AURA, 10400 SI_P_LLC_SEIZED_BY_FEAR, 10401 SI_P_LCG_SOUL_COLLECTOR, 10402 SI_P_LD_READY_FOR_BATTLE, 10403 SI_P_LLC_COMMUNION_EXPAND, 10404 SI_P_LCG_COMMUNION_RECOVERY, 10405 SI_P_LLD_LORD_OF_FEAR, 10406 SI_P_LLD_AURA_OF_FEAR, 10407 SI_P_LLD_THREAT_OF_FEAR, 10408 SI_P_LCN_ENHANCED_SPECTER, 10409 SI_P_LCN_NOBLE_BEING, 10410 SI_P_LCN_TRIBUTE, 10411 SI_P_LCN_NOBILITY, 10412 SI_P_LLD_CREST_OF_FEAR, 30401 RNSI_P_LLC_SEIZED_BY_FEAR, 30402 RNSI_P_LLC_COMMUNION_EXPAND, 30403 RNSI_P_LLD_LORD_OF_FEAR, 30404 RNSI_P_LLD_AURA_OF_FEAR, 30405 RNSI_P_LLD_THREAT_OF_FEAR, 30406 RNSI_P_LCG_SOUL_COLLECTOR, 30407 RNSI_P_LCG_COMMUNION_RECOVERY, 30408 RNSI_P_LCN_ENHANCED_SPECTER, 30409 RNSI_P_LCN_TRIBUTE, 30410 RNSI_P_LCN_NOBILITY, 30411 RNSI_P_LDB_CHEMISTRY, 30412 RNSI_P_LDB_COMMUNION_ABSORB, 30413 RNSI_P_LDN_INVASION_OF_DARKENERGY, 30414 RNSI_P_LDN_MADNESS_CRAVE, 30415 RNSI_P_LDN_MADNESS_DESTROY, 30416 RNSI_P_LLD_OPEN_MIND, 30417 RNSI_P_LLD_THRONE_AUTHORITY, 30418 RNSI_P_LCN_OPEN_MIND, 30419 RNSI_P_LCN_NOBLE_DEVOTION, 30420 RNSI_P_LDN_OPEN_MIND, 30421 RNSI_P_LDN_STABLE_DARKENERGY.
- Tree rows: 30 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7 L240 SetBuffAccumulationCount 322a7660 / AddBuffAccumulationCount 322a8a70 callers in; §7 L257 slot 251 3204dcd0 post-apply hook: Soul Collector, BTI 462, RNSI_P_LCN_TRIBUTE 3; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call; §7am L907 delegate-list registrations: Lu/Ciel 32b43d10 (slot 0x7a4 of 334f06fc) pushes te.
- 15 functions: InitPassiveSkillState 0x690 `32b45820`, the buff-value setters 0x24c / 0x250, and **nine of InitState's ten delegate callbacks** (checklist 7am; `32b3d380`, `32b0fc50`, `32b43590`, `32b435f0`, `32b0fce0`, `32b177b0`, `32b13c80`, `32b17f90`, `32b3e470`) with what only they reach. 35 skill ids are passives here. Checklist 7: the slot-251 Soul Collector hook (`3204dcd0`, buff post-apply) has no March counterpart: add it with its passive.
