# June character Elsword (Stage 7) - Phase CH2d - Skills: Sheath Knight (22)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH2d` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2c review (and any Opus `CH2d0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH2d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH2d` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 22 adds. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH2d)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence), what June's motion and effect look like.

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

## Amended by CH0 (2026-11-03)

- **Functions: 19** (`awk -F'\t' '$7=="CH2d"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 18, new 1).
  June addresses: `32cf4310` `32cf4370` `32cf4400` `32cf4890` `32cf48e0` `32cfde50` `32cfdee0` `32cfdf70` `32cfe840` `32d005e0` `32d007e0` `32d14100` `32d15f80` `32d162d0` `32d16c00`(new ?) `32d189a0` `32d18a60` `32d1b740` `32d1f3a0`.
- **States: 7** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_A_ETK_SWORD_SHIELD`, `ESSI_SI_SA_ETK_SWORD_FALL`, `ESSI_SI_SA_ETK_CRESCENT_CUT`, `ESSI_SI_A_EIS_FATAL_SLAP`, `ESSI_SI_A_EIS_SPIRAL_STING`, `ESSI_SI_SA_ETK_FINAL_STRIKE`, `ESSI_SI_SA_ETK_FINAL_STRIKE_UPGRADE`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 21019 RNSI_SA_ETK_SWORD_FALL, 21020 RNSI_SA_ETK_CRESCENT_CUT, 21021 RNSI_SA_ETK_FINAL_STRIKE, 21033 RNSI_SA_ETK_SWORD_FALL_UPGRADE, 21036 RNSI_SA_ETK_CRESCENT_CUT_UPGRADE, 21053 RNSI_SA_ETK_FINAL_STRIKE_UPGRADE, 21212 RNSI_A_ETK_SWORD_SHIELD, 21213 RNSI_A_ETK_FATAL_SLAP, 21214 RNSI_A_ETK_MIRAGE_STING, 21221 RNSI_A_ETK_SWORD_SHIELD_UPGRADE, 21233 RNSI_A_ETK_FATAL_SLAP_UPGRADE, 21234 RNSI_A_ETK_MIRAGE_STING_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_A_EIS_SPIRAL_STING (; §1 L51 numerical-only DAMAGE_DATA_NEXT table in state ESSI_SI_A_EIS_SPIRAL_ST; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7c L352 29 alpha-tag sites 32d0d660 .. 32d1ecc0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- `32d16c00` (Crescent Cut) calls June-only `31f76580`: CH1o writes it; call it by CH1o's name.
  (CH1o: it is the main effect's velocity, not its scale: `CX2DamageEffect::CEffect::ChangeMainEffectVelocity( 1, x )` @32d17062, `_CORE`; JUNE_BUGS 526.) Alpha tags (7c): `32d14100` x9 (Sword Fall FrameMove).

## Amended by CH2c review (2026-11-05)

- A `reg` row in `ch0_skill_listing_rows.tsv` with an empty callbacks column may have lost its register name (state 262 in CH2c froze the character). Read that state's block in `32d223e0` in Ghidra before registering it with no callbacks. CH2d's ids have no such row; CH3a's state 204 does.
