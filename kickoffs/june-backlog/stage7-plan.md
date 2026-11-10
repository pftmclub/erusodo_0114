# June port backlog - Stage 7 planning - the remaining March characters

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan char-eve` (the first in the order below; later: `char-chung`, `char-elsword`, `char-raven`, `char-rena`, `char-ara`, `char-elesis`, `char-add`) |
| Output | plan key `char-<name>` (`JUNE_CHARACTER_<NAME>_PHASES.md`, `kickoffs/char-<name>/`, a `char-<name>` STATUS block); one character per planning session |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 7); template: `JUNE_CHARACTER_AISHA_PHASES.md` "Template for Stage 7" |
| Depends on | pilot-char-revamp Z (done 2026-10-22). Stage 6 does not block a character's non-transcendence phases; each plan lists its transcendence rows as waiting on `june-transcend` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_<PREFIX>_*` (none in this planning session) |
| Log tag | `[JUNE_CHAR] <PREFIX>` |
| Review | none (a planning session) |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7" "Phases" "CH1 split (2026-10-21, user)" "CH4 split (2026-10-22, Opus)" "Phase CH-N - A June-only later class path (template; not run for Aisha)" "CH6 - Done (2026-10-22, Sonnet 5.5)"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 7: remaining March characters" "Standing rules (every stage, every kickoff)"
```
Then the character's own checklist rows (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7, grep the class name) and the `june2017_re/charport_scan.py` output.

## Order and why (set by Z, 2026-10-22; a planning session may move a row with a stated reason)

| # | Character | Plan key | Why here |
|---|---|---|---|
| 1 | Eve | `char-eve` | Smallest gap (3) and no blocker: a second character proves the template without new decisions. Its extras (167 alpha tags, 7ag slot encoders, El Crystal statics) are the 7c / 7ag / 7ah traps Aisha already walked. |
| 2 | Chung | `char-chung` | Gap 15, no June-only class, no decision; `ChangeMaxCharacterAddResource` is ported. |
| 3 | Elsword | `char-elsword` | Gap 20, no blocker. (Planning, 2026-11-03: the `DAMAGE_SKILL_ID` x103 of `Elsword_DamageData.lua` are Rose's ids 31011-31607, so there is no Elsword table job.) |
| 4 | Raven | `char-raven` | Gap 25, the biggest plain character; after three clean runs the template should be tight. (Planning, 2026-11-05: Chung-sized, 440 carved + 28 uncarved; no summons; Nasod core / rage is CH1q, overheat CH4c.) |
| 5 | Rena | `char-rena` | Gap 12 would put her second, but she needs the **`ADD_RENA_SYSTEM` decision** (natural force, 77 uses). Ask it when she is scheduled; if the user answers before Eve closes, move her to #2. (Planning, 2026-11-07: Raven-sized, 467 carved + 24 uncarved; trap summons = CH2d0; natural force = CH1q, asked at the end of planning.) |
| 6 | Ara | `char-ara` | Gap 14; first run of **CH-N** (classes 34, 124), the stat-row decision, `CUSTOM_RENDER_STATE`, 2 new motion files. After the plain characters, so CH-N is written once from the most mature template. (Planning, 2026-11-10: 540 carved + 32 uncarved, 331 states, no summons; 34 / 124 are Little Specter / Asura, March has 31 / 119 already; CH-N = CHNa / CHNb under the class-generic `SERV_IRUHADEV_JUNE_CLASS_PATH` Elesis and Add reuse; force power / berserk = CH1q, Specter / tails = CH1q2.) |
| 7 | Elesis | `char-elesis` | Gap 20; CH-N for 35 / 125 (reuses Ara's), first live reserved-buff path (7ai). (char-elsword CH0, 2026-11-03: the shared Way of the Sword pieces, damage-manager math incl. her `32622550` / `32622660` and `UpdateElswordSkillSlotEffect`, are ported whole by char-elsword CH1q / CH1q2; her class side stays hers: vtable `334fa61c` (GetState `32b52790`, Start / End Vigor `32b74760` / `32b74930`, Start / End Destruction `32b7f0b0` / `32b74bf0`, DamageReact `32b73ff0`, GetFinalDamage `32b74ed0`, create-gage `32b7ef90`, Crest of Time pair `32b6ac80` / `32b6acb0`), gauge data vtable `3349b184`, gauge UI `3349ac34` with `DLG_Elesis_WayOfSword.lua`; rows `elesis` of `june2017_re/char_elsword/ch0_wos.tsv`. CH1q2 found in hers what Elsword's had: gauge data `InitWhenGameIsOver` `32a34c60` no longer resets the state, gauge UI `InitUI` `32e6e060` order, vslot 0x5c `32e69e70` = `RefreshWayOfSwordUI` (the base virtual exists under `_WOS`).) (char-elsword CH4a2, 2026-11-05: the user state key `CREATE_EFFECT_SET` and its play are ported class-agnostic under `SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET`; her two writers of the rate `m_fCreateEffectSetRate` (June `+0x8e50`), `32b74500` @`32b7471b` and `32b89260` @`32b8930d`, are hers: until they land, her script's `EffectSet_Elesis_SBH_Blaze_Wing_Soul_Ignition` rows play at rate 0.) |
| 8 | Add | `char-add` | Skeleton March kit, 4 June-only classes (33, 36, 123, 126), no stat rows for 9 / 32 / 122, 4 new motion files: last, with everything proven. Add overrides vslot 0x524 (`32a315a0`): its calls to `329cc340` need Add's own now-state id (funcmap row `329cc340 (CH2b0)`). Crest of Time (char-chung CH4b, 2026-11-03): Add's `ANSI_SA_COMMON_CREST_OF_TIME` is the only June cast of BTI 436 / 926, so its engine comes with this port whole: user slot 250 pre-apply hook `3201ffb0` (BTI 436 / 926 / 807), the snapshot / restore bases `32009780` / `3204da60` (vslots 0x740 / 0x744, members `+0x92a4..+0x92c4`; the clear must reach Chung's `m_iSnapshotCannonBallCount` / `Ex`), the death-prevention gate `320072d0` in `UpNowHp` / `SetNowHp` (also 27409 / 23402), and every class's 0x740 / 0x744 override (Chung's written in CH1l; Elsword's `32d058a0` / `32d058d0`; Rena's 0x740 `32c852f0` in char-rena CH1i, member `m_iSnapshotNaturalForceCount` +0x92b4; checklist section 7 death-prevention row). |

Gap alone did not predict effort (Aisha's gap of 1 took about 96 commits); the order follows blockers and decisions first, gap second.

## Task (per planning session)

Plan one character as `JUNE_CHARACTER_<NAME>_PHASES.md` in the Aisha plan's shape, using its corrected template:
- re-run `census.py`, `charport_scan.py` and `charport_keys.py <Class>`; run `ch0c_listing_check.py` on the new registration map in CH0 (step 0);
- name the CH1 / CH2 / CH3 / CH4 sub-phases up front from the map (more than ~25 functions or more than one engine piece = split), each with model, flag and review;
- list every `unmapped` callee per skill and give each an Opus sub-phase;
- include CH-N only when the character has a templet-backed June-only class;
- put the character's transcendence rows in a "waits for Stage 6" list, not in a phase;
- the character's checklist section-7 rows go to the phase that owns them, with the `(<Name>: CHn)` mark made by that phase.
- skill-level reads in the character's CH1 kickoffs call `CX2GUUser::GetSkillLevel` / `GetLearnedSkillAbilityValue` (Eve CH1a2, flag `SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`; renamed by june-march-divergence) and never copy them into the class.
- skill-use reads in the character's CH1 kickoffs call the seven `CX2GUUser` helpers `UserNoStateChangeActionFrameMove` / `MarkSkillSlotUsed` / `ReserveSkillUse` / `StateChangeBySkillSlot` / `SkillMemoMpRefund` / `CheckSkillUseNotPve` / `CheckSkillUseNotDungeon` and the fields `m_iChosenSkillID` / `m_iChosenSkillSlotIndex` / `m_fReservedSkillMp` / `m_fChosenSkillMp` / `m_abSkillSlotUsed` / `m_abHyperSkillSlotUsed` (Eve CH1l5, flag `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`) and never copy them into the class. The dev free-skill gate is `CX2GUUser::IsDevUnlimitedSkillUse()`.
- (char-chung planning, 2026-10-31) No `June_*` members from CH1a on: a June body goes inside March's same-name function under `_LIVE` (one-file probe compile), a function March lacks gets a plain name under `_CORE`. CH0's class map carries an `inside` / `new` column for it. `JUNE_CHARACTER_CHUNG_PHASES.md` is the first plan written this way.

Output: the phase doc, `kickoffs/char-<name>/`, a STATUS block. No code, no build. Design questions go to the user at the end, each with a recommendation.
