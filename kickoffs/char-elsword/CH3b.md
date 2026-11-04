# June character Elsword (Stage 7) - Phase CH3b - Skills: Rune Slayer (101)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH3b` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3a review (and any Opus `CH3b0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH3b` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 101 adds, incl. his type-2 row; the effect-set modifier tag 19 (`32d18230`, 7a) if CH0 put it here. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH3b)`.
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

- **Functions: 22** (`awk -F'\t' '$7=="CH3b"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 15, new 7).
  June addresses: `32cf3ea0` `32cf3ec0` `32cf3ee0` `32cf4050` `32cf4080` `32cf44d0` `32cf4770` `32cf4d70`(new ESSI_ERS_COUNTER_GUARD_EventProcess) `32cf4e50`(new ESSI_ERS_COUNTER_GUARD_READY_StateStartFuture) `32cf4ff0`(new ESSI_SA_ERS_EXTREME_RUNE_BUSTER_Init) `32cfad40` `32cfad80` `32cfc3f0` `32cfec80` `32cfff60` `32d05500`(new ESSI_ERS_COUNTER_GUARD_READY_EventProcess) `32d05b80`(new ESSI_ERS_COUNTER_GUARD_END_EventProcess) `32d068d0`(new ESSI_SA_ERS_EXTREME_RUNE_BUSTER_EventProcess) `32d09290` `32d09310`(new ESSI_SA_ERS_EXTREME_RUNE_BUSTER_StateStart) `32d135c0` `32d18230`.
- **States: 14** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SI_A_ERS_RUNE_OF_FIRE`, `ESSI_SA_EMK_PHOENIX_TALON`, `ESSI_SA_EMK_SWORD_FIRE`, `ESSI_SA_EMK_SWORD_FIRE_LANDING`, `ESSI_SA_ERS_LUNA_BLADE`, `ESSI_ERS_COUNTER_GUARD_READY`, `ESSI_ERS_COUNTER_GUARD`, `ESSI_ERS_COUNTER_GUARD_END`, `ESSI_SA_ERS_SPLASH_EXPLOSION`, `ESSI_A_ERS_ENCHANT_BREAK`, `ESSI_SA_ERS_TRIPLE_SABER`, `ESSI_SA_ERS_OVERS_GEYSER`, `ESSI_SA_ERS_EXTREME_RUNE_BUSTER`, `ESSI_SA_EMK_PHOENIX_TALON_UPGRADE`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 21015 RNSI_SA_ERS_PHOENIX_TALON, 21016 RNSI_SA_ERS_SWORD_FIRE, 21017 RNSI_SA_ERS_LUNA_BLADE, 21018 RNSI_SA_ERS_SPLASH_EXPLOSION, 21039 RNSI_SA_ERS_TRIPLE_SABER, 21040 RNSI_SA_ERS_OVERS_GEYSER, 21049 RNSI_SA_ERS_PHOENIX_TALON_UPGRADE, 21050 RNSI_SA_ERS_SWORD_FIRE_UPGRADE, 21051 RNSI_SA_ERS_LUNA_BLADE_UPGRADE, 21052 RNSI_SA_ERS_SPLASH_EXPLOSION_UPGRADE, 21059 RNSI_SA_ERS_EXTREME_RUNE_BUSTER, 21210 RNSI_A_ERS_RUNE_OF_FIRE, 21211 RNSI_A_ERS_COUNTER, 21223 RNSI_A_ERS_ENCHANT_BREAK, 21231 RNSI_A_ERS_RUNE_OF_FIRE_UPGRADE, 21232 RNSI_A_ERS_COUNTER_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_A_ERS_ENCHANT_BREAK (sc; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7a L306 32d18230 RunSlayer_RUNE_GUARD_FIRE pushes modifier tag 19 (main-effect; §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L635 March-only effect templets: X2GUElsword_SwordMan.cpp LORD_KNIGHT_SAND_; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- March-only `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER` / Sword Enchant site `X2GUElsword_SwordMan.cpp:15268` (7u).

## Amended by CH1p (2026-11-04)

- Held-key skills read `CX2GUUser::m_kSkillSlotKey` (June `+0x8b58`; `Set` `31fce6d0`, `IsKeyReleased` `31fc89a0`, `X2GUUser.h`, `_EVE_CORE`): Counter Guard ready / guard StartFuture `32cf4e50` (uncarved: `charport_adis.py elsword`) and guard EventProcess `32cf4d70`. March has no key reader there: port June's calls whole.
