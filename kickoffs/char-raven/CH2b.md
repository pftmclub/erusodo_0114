# June character Raven (Stage 7) - Phase CH2b - Skills: Sword Taker (16)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH2b` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2a review (and any `CH2b0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`) |
| Review | `kickoffs/char-raven/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH2b - Done" "CH2b - Review"
```
Then CH0's and CH1's Done notes, and the `CH2b` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-16 skill ids class 4 lacks. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- March vs June damage per skill in `MODS.md`. No `NUM use` lines: the engine's `NUMERICAL_DEBUG` is retired (CH2a review). Re-define `_SKILL_DEBUG` for this phase's `RAVEN SKILL` lines; the CH2b review retires it.
- The hand-off also plays CH2a's Seven Burst (24028) and Flame Sword (24218, ground and air), never played in CH2a.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH2b)`.
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

- **Functions: 27** (`awk -F'\t' '$7=="CH2b"' june2017_re/char_raven/ch0_class_map.tsv`; inside 26, new 1).
  June addresses: `32c263e0` `32c264a0`(new RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_Init) `32c26770` `32c26820` `32c26cb0` `32c27370` `32c27380` `32c27390` `32c273a0` `32c273b0` `32c27be0` `32c30a30` `32c30af0` `32c30b50` `32c31030` `32c31090` `32c33280` `32c33310` `32c37d90` `32c37dc0` `32c37fc0` `32c38030` `32c3aab0` `32c3e8a0` `32c4df80` `32c5ccc0` `32c5cd80`.
- **States: 9** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_raven/ch0_state_skill.tsv`): `SI_A_RBM_ONE_FLASH`, `RSI_SA_RST_SHOCKWAVE`, `RSI_SI1_SOUL_TAKER_BERSERKER_BLADE`, `RVC_SI_A_BLEEDING_SLICER`, `RVC_SI_A_BLEEDING_SLICER_GROUND`, `RSI_A_ROT_ARMOR_BREAK`, `RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB`, `RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_HYPER`, `RVC_SI_A_BLEEDING_SLICER_FLY`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 24011 RNSI_SA_RST_SHOCKWAVE, 24012 RNSI_SA_RST_BERSERKER_BLADE, 24013 RNSI_SA_RST_HYPER_SONIC_STAB, 24030 RNSI_SA_RST_SHOCKWAVE_UPGRADE, 24033 RNSI_SA_RST_BERSERKER_BLADE_UPGRADE, 24046 RNSI_SA_RST_HYPER_SONIC_STAB_UPGRADE, 24207 RNSI_A_RST_ONE_FLASH, 24208 RNSI_A_RST_BLEEDING_SLICER, 24209 RNSI_A_RST_ARMOR_BREAK, 24220 RNSI_A_RST_ONE_FLASH_UPGRADE, 24232 RNSI_A_RST_BLEEDING_SLICER_UPGRADE, 24233 RNSI_A_RST_ARMOR_BREAK_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_A_BLEEDING_SLICER_GRO; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- Bleeding Slicer (24208 / 24232 `RNSI_A_RST_BLEEDING_SLICER`, tables `RVC_SI_A_BLEEDING_SLICER*`) moved here from CH1g2 by CH0.
