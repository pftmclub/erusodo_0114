# June character Raven (Stage 7) - Phase CH3b - Skills: Reckless Fist (107)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH3b` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3a review (and any `CH3b0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3b - Done" "CH3b - Review"
```
Then CH0's and CH1's Done notes, and the `CH3b` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-107 skill ids no first job has; `DamageEffect_RRF_ExplosiveHand_Impact` (7u, JUNE_BUGS 200). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH3b)`.
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

## Amended by CH0 (2026-11-05)

- **Functions: 39** (`awk -F'\t' '$7=="CH3b"' june2017_re/char_raven/ch0_class_map.tsv`; inside 26, new 13).
  June addresses: `32c26570` `32c26a00` `32c26c50` `32c26c90`(new SI_SA_RRF_X_CRASH_StateStart) `32c26e20` `32c277e0` `32c27860` `32c27890` `32c278b0` `32c278d0`(new RSI_A_RRF_ENCHANT_HAND_Init) `32c27c20`(new RSI_SA_RRF_MAXIMUM_DIVE_Init) `32c27c30`(new RSI_SA_RRF_POWER_BOMB_Init) `32c27c70`(new RSI_A_RRF_SPREAD_Init) `32c28140`(new RSI_SA_RRF_DEVASTATING_STRIKE_Init) `32c30c70` `32c315c0` `32c331a0` `32c33230` `32c33830` `32c33850` `32c33930` `32c33960` `32c339a0`(new RSI_SA_RRF_WILD_CHARGE_DASH_StateEnd) `32c33a10` `32c38f70` `32c391f0` `32c393f0` `32c395f0`(new RSI_A_RRF_ENCHANT_HAND_EventProcess) `32c3b0a0`(new RSI_SA_RRF_MAXIMUM_DIVE_EventProcess) `32c3b3e0`(new RSI_SA_RRF_POWER_BOMB_EventProcess) `32c3c050`(new RSI_A_RRF_SPREAD_EventProcess) `32c3c8e0`(new RSI_SA_RRF_DEVASTATING_STRIKE_StateEnd) `32c3c8f0`(new RSI_SA_RRF_DEVASTATING_STRIKE_EventProcess) `32c3e8d0` `32c4a2a0` `32c4a490` `32c4a670` `32c4a6c0` `32c560e0`.
- **States: 17** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_A_RRF_LIMIT_CRUSHER_CHARGE`, `RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK`, `RSI_A_RRF_LIMIT_CRUSHER_AVOID_ATTACK`, `RSI_A_ROT_NUCLEAR`, `RSI_SI2_OVER_TAKER_ARC_ENEMY`, `RSI_SA_RRF_WILD_CHARGE_READY`, `RSI_SA_RRF_WILD_CHARGING`, `RSI_SA_RRF_WILD_CHARGE_DASH`, `RSI_A_RRF_ENCHANT_HAND`, `SI_SA_RRF_X_CRASH`, `RSI_A_RRF_SPREAD`, `RSI_SA_RRF_MAXIMUM_DIVE`, `RSI_SA_RRF_POWER_BOMB`, `RSI_SI2_OVER_TAKER_ARC_ENEMY_UPGRADE`, `RSI_A_RRF_ENCHANT_HAND_UPGRADE`, `RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK_UPGRADE`, `RSI_SA_RRF_DEVASTATING_STRIKE`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 24007 RNSI_SA_RFF_NUCLEAR, 24008 RNSI_SA_RFF_ARC_ENEMY, 24009 RNSI_SA_RRF_WILD_CHARGE, 24010 RNSI_SA_RRF_X_CRASH, 24037 RNSI_SA_RRF_MAXIMUM_DIVE, 24038 RNSI_SA_RRF_POWER_BOMB, 24042 RNSI_SA_RFF_NUCLEAR_UPGRADE, 24043 RNSI_SA_RFF_ARC_ENEMY_UPGRADE, 24044 RNSI_SA_RRF_WILD_CHARGE_UPGRADE, 24045 RNSI_SA_RRF_X_CRASH_UPGRADE, 24057 RNSI_SA_RRF_DEVASTATING_STRIKE, 24205 RNSI_A_RRF_LIMIT_CRUSHER, 24206 RNSI_A_RRF_ENCHANT_HAND, 24224 RNSI_A_RRF_SPREAD, 24230 RNSI_A_RRF_LIMIT_CRUSHER_UPGRADE, 24231 RNSI_A_RRF_ENCHANT_HAND_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RRF_LIMIT_CRUSHER_REVE; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RRF_LIMIT_CRUSHER_AVOI; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RRF_LIMIT_CRUSHER_REVE; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L636 March-only effect templets the re-adoption dropped: RAVEN_SEVEN_BURST_; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
