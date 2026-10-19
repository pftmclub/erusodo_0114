# June character Aisha (Stage 5 pilot) - Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | split by CH0: run CH3a, then CH3b (CH2b review first); not alongside CH4 |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH3-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Violet Mage and the first jobs (classes 2, 14, 15, 23)" "Phase CH3 - Skills: second jobs (104, 105, 113) and hyper actives"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7t. The hyper cut-in: data, and the class hooks that pick the berserk / Lu/Ciel names (from June effect completion C3t)"
```
Then CH2's Done note and review (the recipe and what the review corrected), and the `ch3` rows of CH0's tsvs.

## Task

Do "Phase CH3" with CH2's recipe, plus the hyper actives and the three `SHOW_HYPER_CUT_IN_TIME` states. Same rules as CH2: CP949 files go
through `bytepatch.py`, damage numbers go to `MODS.md`, and anything the map does not settle is "this needs Opus".

## Amended by CH0

Written by CH0 (2026-10-20). **Split: run `CH3a` (Elemental Master + Void Princess: 32 skill ids, 36 state functions) and then `CH3b`
(Dimension Witch + the hyper actives 22600-22605: 22 ids, 15 functions)**; their kickoff files carry the filters. This file stays as the
shared brief.
- Attached rows: 7ac Hell Drop (`32cd95d0` @`32cd97ba`, CH3a), 7af SUB_CAMERA sequencer `31ff5440` in the ring skills (Fire Ring / Dark Ring
  CH3a, Energy Ring CH3b), 7c alpha site in Circle Flame (`32cd7470`, CH3a), 7t hyper-active cut-in option (CH3b).
- June data gaps the closure found (keep verbatim, `JUNE_BUGS.md` rows): `AVP_DestructionBlackHollE5`, `DamageEffect_SA_AVP_ABYSS_FIELD_WeaponFX`,
  `AEM_ICICLE_SPEAR_Die_P02` are named by June's own data and defined nowhere.
- The Body skills (Elemental / Shadow / Energetic Body, ids 22205 / 22210 / 22215 + upgrades) have no state: June tests their ids in class
  code (CH1i's `SpecialAttackNoStageChange` `32ccbca0` and neighbours). Their play-test belongs here once CH1i's code is live.
