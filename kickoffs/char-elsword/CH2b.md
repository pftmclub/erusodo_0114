# June character Elsword (Stage 7) - Phase CH2b - Skills: Sword Knight (10)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH2b` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2a review (and any Opus `CH2b0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH2b` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 10 adds. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH2b)`.
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

- **Functions: 11** (`awk -F'\t' '$7=="CH2b"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 7, new 4).
  June addresses: `32cf3b00`(new ESSI_SI2_ES_SPIRAL_BLAST_Init) `32cf3f10` `32cf4530` `32cf4de0`(new ESSI_SA_ESK_MORTAL_BLOW_FrameMove) `32cf9e50`(new ESSI_SI2_ES_SPIRAL_BLAST_EventProcess) `32cfa6c0` `32cfa740` `32cfadc0` `32cfc430` `32cfed60` `32d05530`(new ESSI_SA_ESK_MORTAL_BLOW_EventProcess).
- **States: 6** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_A_ESK_ARMOR_BREAK`, `ESSI_SI_A_ES_KICK`, `ESSI_SI2_ES_SPIRAL_BLAST`, `ESSI_SA_ESK_MORTAL_BLOW`, `ESSI_A_ELK_IMPACT_SMASH`, `ESSI_SA_ESK_DOUBLE_SLASH`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 21005 RNSI_SA_ESK_SPIRAL_BLAST, 21006 RNSI_SA_ESK_MORTAL_BLOW, 21007 RNSI_SA_ESK_DOUBLE_SLASH, 21031 RNSI_SA_ESK_SPIRAL_BLAST_UPGRADE, 21034 RNSI_SA_ESK_MORTAL_BLOW_UPGRADE, 21043 RNSI_SA_ESK_DOUBLE_SLASH_UPGRADE, 21202 RNSI_A_ESK_KICK, 21203 RNSI_A_ESK_IMPACT_SMASH, 21204 RNSI_A_ESK_ARMOR_BREAK, 21219 RNSI_A_ESK_KICK_UPGRADE, 21225 RNSI_A_ESK_IMPACT_SMASH_UPGRADE, 21226 RNSI_A_ESK_ARMOR_BREAK_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_A_ESK_ARMOR_BREAK (scri; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_A_ES_KICK (script L1; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_A_ELK_IMPACT_SMASH (scr; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §1 L37-105 blocks that index past their skill data need a JUNE_BUGS row; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §2 L124 APPLY_LEARNED_SKILL_BUFF_FACTOR_ID and _RELATIVE_SKILL_ID x1 each (8b ; §2 L125 bold APPLY_LEARNED_..._RELATIVE_BUFF_TEMPLATE_ID x1; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- `c1a` zero row: **21006 `RNSI_SA_ESK_MORTAL_BLOW`** (`c1a_numerical_blocks.tsv` l.731, `DamageEffect_ESK_Mortal_Blow_Ultimate_Fire_Ground` INDEX 2): verbatim + `JUNE_BUGS.md` row. Checklist L124 / L125 (`APPLY_LEARNED` keys in `ESSI_A_ESK_ARMOR_BREAK`) are here.

## Amended by CH2a (2026-11-04)

- `SetEquippedSkillFuncBySkillSlotIndex` already holds `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` June cases (Flame Geyser, Unlimited Blade, Assault Slash, Triple Geyser, Rolling) ahead of the LIVE `default:`: add this phase's ids beside them (June's registration, March's cases untouched). Use `ELSWORD_EVENT_TIMER_ONESHOT( fTime_ )` (June's one-shot on the drawn animation) and `ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_CANCEL` / `_DASH_CANCEL` (7h) instead of copying the `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` pairs. State registrations that June makes by id (Rolling's end state) are a local `UserUnitStateData` + `m_StateList[ id ]` inside the case. `JUNE_BUGS.md` rows 528 / 529 are CH2a's.

## Amended by the CH2a review (2026-11-05)

- `_SKILL_DEBUG` is retired (`Always.h`, `//#define`): define it again for this phase's log lines; the CH2b review retires it.
- A skill-level read in a June body takes June's fourth argument: `GetSkillLevel( id, false, 0, true )` when June pushes `( id, 0, 0, 1 )` (upgrade swap; CH2a review fix).
- `RNSI_SA_ES_*` names for 21001-21004 / 21027-21030 exist (`X2SkillTree.h`, `JUNE_ENUM_NAMES`). `ReservedSkillStart` (cool time / MP at state start) is Elsword's since `2432120b`.
