# June character Rena (Stage 7) - Phase CH2c - Skills: Sniping Ranger (13)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH2c` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2b review (and any `CH2c0`). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2c - Done" "CH2c - Review"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-13 skill ids classes 3 / 12 lack; `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` (7u); the Guide Arrow `:21554` if CH0 puts it here. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH2c)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence; learn only the bank under test), each
  cast twice (cool time, MP), what June's motion and effect look like.

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

- **Functions: 39** (`awk -F'\t' '$7=="CH2c"' june2017_re/char_rena/ch0_class_map.tsv`; inside 33, new 6).
  June addresses: `32c6b720` `32c6b7e0` `32c6b7f0` `32c6b800` `32c6b810` `32c6bb20` `32c6be30` `32c6bef0` `32c6c970` `32c6c9c0` `32c6ca70` `32c6cae0`(new LESI_A_LGA_RAPID_SHOT_READY_StateStart) `32c6d690`(new LESI_A_LSR_MIND_OF_SHARPSHOOTER_Init) `32c75ed0` `32c75fa0` `32c76840` `32c76880` `32c76dd0` `32c76e30` `32c7c470` `32c7c670` `32c7c870` `32c7c950` `32c7c990` `32c7cbe0` `32c831f0`(new LESI_A_LSR_MIND_OF_SHARPSHOOTER_EventProcess) `32c83220`(new LESI_A_LSR_MIND_OF_SHARPSHOOTER_FrameMove) `32c86620`(new ?) `32c86720` `32c86d60` `32c86d90` `32c86f10` `32c86f40` `32c88050`(new LESI_A_LGA_RAPID_SHOT_READY_FrameMoveFuture) `32c953d0` `32c9c340` `32c9cc10` `32c9d250` `32c9d420`.
- **States: 11** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SI2_LE_GUIDED_ARROW`, `LESI_SI2_LE_GUIDED_ARROW_HYPER`, `LESI_SI3_LSR_CRAZY_SHOT`, `LESI_SI3_LSR_CRAZY_SHOT_HYPER`, `LESI_SA_LSR_WINDWARD`, `LESI_A_LSR_SHOOTING_MAGNUM`, `LESI_A_LGA_ARC_SHOT`, `LESI_A_LGA_RAPID_SHOT_READY`, `LESI_A_LGA_RAPID_SHOT_ATTACK`, `LESI_A_LGA_RAPID_SHOT_FINISH`, `LESI_A_LSR_MIND_OF_SHARPSHOOTER`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 23012 RNSI_SA_RSR_GUIDED_ARROW, 23013 RNSI_SA_RSR_WINDWARD, 23014 RNSI_SA_RSR_CRAZY_SHOT, 23032 RNSI_SA_RSR_GUIDED_ARROW_UPGRADE, 23035 RNSI_SA_RSR_WINDWARD_UPGRADE, 23048 RNSI_SA_RSR_CRAZY_SHOT_UPGRADE, 23208 RNSI_A_RSR_ARC_SHOT, 23209 RNSI_A_RSR_SHOOTING_MAGNUM, 23210 RNSI_A_RSR_MIND_OF_SHARPSHOOTER, 23212 RNSI_A_RSR_RAPID_SHOT, 23223 RNSI_A_RSR_ARC_SHOT_UPGRADE, 23233 RNSI_A_RSR_SHOOTING_MAGNUM_UPGRADE, 23234 RNSI_A_RSR_MIND_OF_SHARPSHOOTER_UPGRADE, 23236 RNSI_A_RSR_RAPID_SHOT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7n L493 "user-sync class-skill float: Rena 32c88050 (Grand Archer RANDOM_SHOT_; §7u L638 March-only effect templets the re-adoption dropped: DamageEffect_LGA_A; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.

## Amended by CH2b review (2026-11-10)

- **Debug flag:** the review retired `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (`//#define`, `Always.h:12067`); CH2c re-defines it for its own `[JUNE_CHAR] RENA SKILL` lines (the equip line already exists) and its review retires it again.
- **Skill icon:** `skillicons.py check` found 23210 (Mind of Sharpshooter) `NO_PIECE` (`DLG_Lena_SKILL_ScreenShot_04`); stage it from June (`kom.py stage-june`, then pack) so the gate passes.
- **Shapes from CH2a / CH2b:** registration as in "Amended by CH2a" (hyper rows copy Init only where June's hyper row names one: Back Kick, Low Kick, Assault Impact did). A once-per-state MP cost is `CX2GUUser::FlushMpOnce` (June `3208dcb0`, a `CX2GameUnit` vslot `0xe8`; cleared in `CommonStateStartFuture`); a held-key follow-up is `ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_` (Back Kick).
