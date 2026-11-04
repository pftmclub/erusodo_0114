# June character Elsword (Stage 7) - Phase CH2a - Skills: Sword Man (1)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH2a` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 1 has; the two `c1a` zero rows (21006, 21013) verbatim with a `JUNE_BUGS.md` row. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH2a)`.
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

- **Functions: 26** (`awk -F'\t' '$7=="CH2a"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 17, new 9).
  June addresses: `32cf3920`(new ESSI_SI_A_MEGASLASH_MEMO_READY_Init) `32cf3950`(new ESSI_SI_A_MEGASLASH_MEMO_READY_StateStart) `32cf3970`(new ESSI_SI_A_MEGASLASH_MEMO_READY_StateStartFuture) `32cf39a0`(new ESSI_SI_A_MEGASLASH_MEMO_CHARGE_StateStart) `32cf39b0`(new ESSI_SI_A_MEGASLASH_MEMO_CHARGE_EventProcess) `32cf3a10` `32cf3a40` `32cf3aa0` `32cf3ad0` `32cf3b20` `32cf41d0` `32cf4200` `32cf9920`(new ESSI_SI_A_MEGASLASH_MEMO_FIRE_EventProcess) `32cf9960`(new ESSI_SI_A_MEGASLASH_MEMO_READY_EventProcess) `32cf9990` `32cf99d0` `32cf9d50` `32cf9d80` `32cf9f20` `32cfd840` `32cfd880` `32d08880` `32d0fe90`(new ESSI_SI_A_MEGASLASH_MEMO_CHARGE_FrameMove) `32d10020`(new ESSI_SI_A_MEGASLASH_MEMO_FIRE_FrameMove) `32d101f0` `32d137a0`.
- **States: 10** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SI_A_MEGASLASH_MEMO_READY`, `ESSI_SI_A_MEGASLASH_MEMO_CHARGE`, `ESSI_SI_A_MEGASLASH_MEMO_FIRE`, `ESSI_SI_A_ES_ROLLING`, `ESSI_SI_A_ES_ROLLING_END`, `ESSI_SI2_ES_FLAME_GEYSER`, `ESSI_SI3_ES_UNLIMITED_BLADE`, `ESSI_SI3_ES_UNLIMITED_BLADE_HYPER`, `ESSI_SI1_ES_ASSAULT_SLASH`, `ESSI_SI3_ES_TRIPLE_GEYSER`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 21000 RNSI_SA_ES_MEGASLASH, 21001 RNSI_SA_ES_FLAME_GEYSER, 21002 RNSI_SA_ES_UNLIMITED_BLADE, 21003 RNSI_SA_ES_ASSAULT_SLASH, 21004 RNSI_SA_ES_TRIPLE_GEYSER, 21026 RNSI_SA_ES_MEGASLASH_UPGRADE, 21027 RNSI_SA_ES_FLAME_GEYSER_UPGRADE, 21028 RNSI_SA_ES_UNLIMITED_BLADE_UPGRADE, 21029 RNSI_SA_ES_ASSAULT_SLASH_UPGRADE, 21030 RNSI_SA_ES_TRIPLE_GEYSER_UPGRADE, 21200 RNSI_A_ES_ROLLING, 21201 RNSI_A_ES_ENDURANCE, 21217 RNSI_A_ES_ROLLING_UPGRADE, 21218 RNSI_A_ES_ENDURANCE_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_A_ES_ROLLING (script; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI3_ES_UNLIMITED_BLADE ; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI1_ES_ASSAULT_SLASH (s; §1 L51 numerical-only DAMAGE_DATA_NEXT table in state ESSI_SI1_ES_ASSAULT_SLA; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- The two `c1a` zero rows are **not** Sword Man's: 21006 Mortal Blow (class 10) is CH2b, 21013 Wind Blade (class 11) is CH2c (the plan text said CH2a).

## Amended by CH1n (2026-11-04)

- `SetMegaSlashSkillFunc` (`X2GUElsword_SwordMan.cpp`, `_CORE`) is the default of both June switches in `32d223e0` (@32d23c1e). Its body is still
  March's `ESSI_SI1_ES_MEGA_SLASH` case. Replace it here (under `_SKILL`, March's in `#else`) with June's registration: `ESSI_SI_A_MEGASLASH_MEMO_READY`
  on the normal and hyper slot (Init `32cf3920`, Start `32cf3950`, StartFuture `32cf3970`, EventProcess `32cf9960`), `_CHARGE` 0xea (Start `32cf39a0`,
  FrameMove `32d0fe90`, EventProcess `32cf39b0`) and `_FIRE` 0xeb (FrameMove `32d10020`, EventProcess `32cf9920`); funcmap row `32d223e0@32d23c1e`.

## Amended by CH1p (2026-11-04)

- Held-key skills read `CX2GUUser::m_kSkillSlotKey` (June `+0x8b58`; `Set` `31fce6d0`, `IsKeyReleased` `31fc89a0`, `X2GUUser.h`, `_EVE_CORE`): Mega Slash memo ready / charge StartFuture `32cf3970` and charge EventProcess `32cf39b0`. March has no key reader here (no `CurrentSkillKeyManager`, no `SAKP_*`): port June's calls whole.
