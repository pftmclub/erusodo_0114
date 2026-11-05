# June character Raven (Stage 7) - Phase CH2c - Skills: Over Taker (17)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH2c` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2b review (and any `CH2c0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH2c - Done" "CH2c - Review"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-17 skill ids class 4 lacks; the March-only templets `RAVEN_OVER_TAKER_GUARDIAN_STRIKE[_HYPER]` / `_XX_FRONT_X` and their June names (7u); `OT_SP1a_ChargedBolt` verbatim with a `JUNE_BUGS.md` row if CH0 found June plays a name its motion lacks. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH2c)`.
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

- **Functions: 23** (`awk -F'\t' '$7=="CH2c"' june2017_re/char_raven/ch0_class_map.tsv`; inside 23).
  June addresses: `32c26540` `32c265d0` `32c26660` `32c26850` `32c26880` `32c268e0` `32c26970` `32c27500` `32c27940` `32c30c30` `32c30cb0` `32c30d70` `32c310d0` `32c31130` `32c31170` `32c31280` `32c38470` `32c38510` `32c38710` `32c39670` `32c48de0` `32c49730` `32c5ce40`.
- **States: 9** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_A_RRF_HELL_DIVE`, `RSI_A_RRF_HELL_DIVE_UPGRADE`, `RSI_SA_ROT_VALKYRIESJAVELIN`, `RSI_SA_ROT_VALKYRIESJAVELIN_LANDING`, `RSI_SI1_OVER_TAKER_CHARGED_BOLT`, `RSI_A_RF_MEGADRILL_BREAK`, `RSI_A_ROT_WEAPON_BREAK`, `RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE`, `RSI_A_RF_MEGADRILL_BREAK_UPGRADE`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 24004 RNSI_SA_ROT_VALKYRIESJAVELIN, 24005 RNSI_SA_ROT_CHARGED_BOLT_HEDGEHOG, 24006 RNSI_SA_ROT_GUARDIAN_STRIKE, 24029 RNSI_SA_ROT_VALKYRIESJAVELIN_UPGRADE, 24032 RNSI_SA_ROT_CHARGED_BOLT_HEDGEHOG_UPGRADE, 24041 RNSI_SA_ROT_GUARDIAN_STRIKE_UPGRADE, 24202 RNSI_A_ROT_HELL_DIVE, 24203 RNSI_A_ROT_MEGADRILL_BREAK, 24204 RNSI_A_ROT_WEAPON_BREAK, 24219 RNSI_A_ROT_HELL_DIVE_UPGRADE, 24228 RNSI_A_ROT_MEGADRILL_BREAK_UPGRADE, 24229 RNSI_A_ROT_WEAPON_BREAK_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RRF_HELL_DIVE (script ; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RRF_HELL_DIVE_UPGRADE ; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RF_MEGADRILL_BREAK (sc; §1 L50 numerical-only DAMAGE_DATA_NEXT table in state RSI_A_RF_MEGADRILL_BREA; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RF_MEGADRILL_BREAK_UPG; §1 L50 numerical-only DAMAGE_DATA_NEXT table in state RSI_A_RF_MEGADRILL_BREA; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §2 L122 "IGNORE_DEFENCE_RATE and _ON_PVP x2 each in RAVEN_FIGHTER, status ""to; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L636 March-only effect templets the re-adoption dropped: RAVEN_SEVEN_BURST_; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- **Depends on CH0's Opus helper phase CH2c0** (`31fd9f40`).
- Megadrill Break (24203) calls June-only `31fd9f40` (left-hand bone scale, CH2c0) and reads `IGNORE_DEFENCE_RATE` (engine reader ported). `RSI_SA_ROT_CHARGED_BOLT_BLOOD` is dead data in June: the bare `OT_SP1a_ChargedBolt` it names is never played, so no `JUNE_BUGS.md` row.
