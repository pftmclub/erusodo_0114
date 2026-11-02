# June character Chung (Stage 7) - Phase CH3b - Skills: Deadly Chaser (111)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3a2 review and CH3b0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH3b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
```
Then CH0's, CH1's and the CH2 Done notes, and the `CH3b` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Scope: the skill ids class 111 has that classes 6 / 21 do not; Artillery Strike / Missile Shower 6019 / Quantum Ballista 6011 with checklist 7r row 1 (`CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE` StateStartFuture `32bff3d0 @32bff412-32bff420`: the three `SetFocusCamera / SetFocusCameraForce / SetApplyCinematicUI( false )` writes move there from March's READY EventProcess, no `IsMyUnit` test; JUNE_BUGS 164) and 7ac (`@32bfe8c3`, the CanDown call shape). Same method as CH2 (its kickoff's Task): each id's case with June's state functions whole under `_SKILL`, attached rows,
effect closure, the upgrade-id templet swap, raw ids for unnamed rows, "this needs Opus" for an unsettled function.
- `NUM use` lines; March vs June damage in `MODS.md`. CP949: `bytepatch.py` only. Mark rows `(Chung: CH3b)`.
- Hand-off: the unit (18 Freils 117, 42 RapidT / 50 Chung99 111, 43 NovaT 110, or a CH0b unit), each skill id and its A / B slot (not 4 / 9 on an
  un-transcended unit), what June's motion and effect look like.

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

## Amended by CH0 (2026-10-31)

- **Functions: 38** (`awk -F'\t' '$7=="CH3b"' june2017_re/char_chung/ch0_class_map.tsv`; inside 35, new 3).
  June addresses: `32bd9ab0` `32bd9b70` `32bd9b80` `32bd9ba0` `32bd9c70` `32bda300` `32bdb130` `32bdb160` `32bdb1b0` `32bdb1d0` `32bdba90`(new CSI_SA_CDC_SHOOTING_BIND_StateStartFuture) `32bdbab0`(new CSI_SA_CDC_SHOOTING_BIND_StateEndFuture) `32bdbb40`(new CSI_SA_CDC_BURNING_PUNISHER_Init) `32be2730` `32be28b0` `32be28e0` `32be2930` `32be2d30` `32be2d60` `32be3f50` `32be3fe0` `32be4070` `32be8360` `32beb940` `32beba10` `32bee140` `32bee210` `32bf0b50` `32bf3900` `32bfc310` `32bfd2b0` `32bfd4b0` `32bffd20` `32c079f0` `32c07cf0` `32c0df20` `32c0df80` `32c0dfe0`.
- **States: 20** (`awk -F'\t' '$8=="CH3b"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_SI_SA_CDC_BULLET_BLITZ`, `CSI_SI_SA_CDC_BULLET_BLITZ_HYPER`, `CSI_SI_SA_CSG_HEAVY_RAILGUN`, `CSI_SI_SA_CSG_HEAVY_RAILGUN_HYPER`, `CSI_SI_SA_CSG_SHOOTING_STAR_READY`, `CSI_SI_SA_CSG_SHOOTING_STAR_READY_HYPER`, `CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK`, `CSI_SI_SA_CSG_SHOOTING_STAR_END`, `CSI_SI_A_CSG_RUMBLE_SHOT`, `CSI_SI_SA_CSG_COMET_CRASHER_READY`, `CSI_SI_SA_CSG_COMET_CRASHER_READY_HYPER`, `CSI_SI_SA_CSG_COMET_CRASHER_LOOP`, `CSI_SI_SA_CSG_COMET_CRASHER_FINISH`, `CSI_A_CDC_RAPID_CANNON_READY`, `CSI_A_CDC_RAPID_CANNON_LOOP`, `CSI_A_CDC_RAPID_CANNON_FINAL`, `CSI_A_CDC_STUN_SHOT`, `CSI_SA_CDC_SHOOTING_BIND`, `CSI_SA_CDC_EXCITING_CRASH`, `CSI_SA_CDC_BURNING_PUNISHER`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 26015 RNSI_SA_CSG_SHOOTING_STAR, 26016 RNSI_SA_CSG_HEAVY_RAILGUN, 26017 RNSI_SA_CDC_COMET_CRASHER, 26018 RNSI_SA_CDC_BULLET_BLITZ, 26039 RNSI_SA_CDC_SHOOTING_BIND, 26040 RNSI_SA_CDC_EXCITING_CRASH, 26051 RNSI_SA_CSG_SHOOTING_STAR_UPGRADE, 26053 RNSI_SA_CSG_HEAVY_RAILGUN_UPGRADE, 26054 RNSI_SA_CDC_COMET_CRASHER_UPGRADE, 26055 RNSI_SA_CDC_BULLET_BLITZ_UPGRADE, 26063 RNSI_SA_CDC_BURNING_PUNISHER, 26210 RNSI_A_CDC_RUMBLE_SHOT, 26211 RNSI_A_CDC_GATLING_SHOT, 26227 RNSI_A_CDC_STUN_SHOT, 26233 RNSI_A_CDC_RUMBLE_SHOT_UPGRADE, 26234 RNSI_A_CDC_GATLING_SHOT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7r L570 Artillery Strike (Missile Shower 6019, Quantum Ballista 6011): move th; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim; §7ac L774 CanDown call shape: Artillery Strike Ready @32bfe8c3 (X2GUChung.cpp:17.
- 7r row 1 (Artillery Strike `32bff3d0`) and 7ac (`@32bfe8c3`). `CSI_A_CDC_RAPID_CANNON_READY` is defined twice in June's script: JUNE_BUGS row.

## Amended by CH1l4 (2026-11-01)

- CH1l4 declared `m_fShootingBindCoolTimeLeft` (June `+0x98d4`, ctor 0; counts down in `CommonFrameMove`, read by `ShownSkillCoolTime` = CH1o). Two CH3b functions write it (`32bdba9d`: 10.0; `32bdbace`: a skill value from `32007b10`); CH1o's `32bdba60` reads it for skill 26039 (`RNSI_SA_CDC_SHOOTING_BIND`). Rename it if the states say it is another skill's.
- CH1l4 declared `m_mapBombardServiceEffect` (June `+0x97f8`, `std::map< int, CX2EffectSet::Handle >`); June's only filler `32c08970` has no caller (JUNE_BUGS #502). Artillery Strike's own `m_iBombardServiceTarget` code stays March's until a June reader is found.

## Amended by CH1o (2026-11-02)

- CH1o wrote `ShownSkillCoolTime` (`32bdba60`): `RNSI_SA_CDC_SHOOTING_BIND` shows 0 while `m_fShootingBindCoolTimeLeft > 0`, so your two writers make it visible.
- `32bfeb80` (CH0 row, 647 lines: a switch on `+0x8ac0` over per-slot byte flags `+0x86f2..+0x872e`, `GetDistance`, `HaveThisBuff`) is called only from the
  Artillery Strike control EventProcess `32bff350` (`@32bff353`): yours with 7r / 7ac.
