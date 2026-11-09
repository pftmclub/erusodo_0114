# June character Rena (Stage 7) - Phase CH2b - Skills: Combat Ranger (12)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH2b` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2a review (and any `CH2b0`). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2b - Done" "CH2b - Review"
```
Then CH0's and CH1's Done notes, and the `CH2b` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-12 skill ids class 3 lacks; `Effectset_Rena_DiveKickBombingA` / `B` / `B_Memo` (March-only templets read by March's script, 7u) if CH0 puts them here. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH2b)`.
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

- **Functions: 19** (`awk -F'\t' '$7=="CH2b"' june2017_re/char_rena/ch0_class_map.tsv`; inside 19).
  June addresses: `32c6b950` `32c6ba10` `32c6ba20` `32c6ba70` `32c6c710` `32c6c790` `32c6c860` `32c6cf80` `32c76630` `32c76700` `32c76800` `32c7a6c0` `32c7ae20` `32c7d3a0` `32c84ea0` `32c86e80` `32c86eb0` `32c86ee0` `32cad390`.
- **States: 9** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SI1_LCR_SPINNING_KICK`, `LESI_SI2_LCR_CRESCENT_KICK`, `LESI_SI2_LCR_CRESCENT_KICK_HYPER`, `LESI_SI3_LCR_DIVE_KICK_BOMBING`, `LESI_SI3_LCR_DIVE_KICK_BOMBING_HYPER`, `LESI_A_LE_RISING_FALCON`, `LESI_A_LE_RISING_FALCON_LANDING`, `LESI_A_LWS_BACK_KICK`, `LESI_A_LWS_ASSAULT_IMPACT`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 23005 RNSI_SA_RCR_CRESCENT_KICK, 23006 RNSI_SA_RCR_SPINNING_KICK, 23007 RNSI_SA_RCR_DIVE_KICK_BOMBING, 23031 RNSI_SA_RCR_CRESCENT_KICK_UPGRADE, 23034 RNSI_SA_RCR_SPINNING_KICK_UPGRADE, 23043 RNSI_SA_RCR_DIVE_KICK_BOMBING_UPGRADE, 23202 RNSI_A_RCR_RISING_FALCON, 23203 RNSI_A_RCR_BACK_KICK, 23204 RNSI_A_RCR_LOW_KICK, 23206 RNSI_A_RCR_ASSAULT_IMPACT, 23222 RNSI_A_RCR_RISING_FALCON_UPGRADE, 23228 RNSI_A_RCR_BACK_KICK_UPGRADE, 23229 RNSI_A_RCR_LOW_KICK_UPGRADE, 23231 RNSI_A_RCR_ASSAULT_IMPACT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L56 numerical-only DAMAGE_DATA table in state LESI_SI1_LCR_SPINNING_KICK (; §1 L56 numerical-only DAMAGE_DATA_NEXT table in state LESI_SI1_LCR_SPINNING_K; §1 L56 numerical-only DAMAGE_DATA table in state LESI_A_LE_RISING_FALCON (scr; §1 L56 numerical-only DAMAGE_DATA_NEXT table in state LESI_A_LE_RISING_FALCON; §1 L56 numerical-only DAMAGE_DATA_LAST table in state LESI_A_LE_RISING_FALCON; §1 L56 numerical-only DAMAGE_DATA table in state LESI_A_LWS_ASSAULT_IMPACT (s; §1 L56 numerical-only DAMAGE_DATA_NEXT table in state LESI_A_LWS_ASSAULT_IMPA; §1 L56 numerical-only DAMAGE_DATA + numerical override live (the 12 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L638 March-only effect templets the re-adoption dropped: DamageEffect_LGA_A; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- `LESI_A_LE_RISING_FALCON_LANDING` (id 227, registered by InitState) is here with Rising Falcon; its FrameMove `32cad390` reads `DAMAGE_DATA_LAST` (@32cad4d9).

## Amended by CH1n (2026-11-09)

- **Slot state:** Low Kick (23204) registers `LESI_SA_LCR_LOW_KICK` ( 295 ) by table name in its id-switch case (InitStateCommon also registers it `@97`, kept). The id switch takes `GetUpgradeBaseSkillID( eSkillID_ )` (`CH2a.md`, Amended by CH1n).

## Amended by CH1p (2026-11-09)

- **Held-key test (7ag):** June's Back Kick EventProcess `32c84ea0` (inside March's `LESI_A_LWS_BACK_KICK_EventProcess`) adds a skill-key re-press March never had: inside the `m_fEventTime[0]` key window (animation time strictly between `+0x8c10` and `+0x8c14`, `@32c84f52`-`@32c84f8d`), `GetEquippedActiveSkillTemplet( GetFutureStateSkillID(), true )` with templet `+0x178` != 0, then `IsEquippedSkillKeyPressed( m_InputData, GetFutureStateSkillID() )` (`31fce710` `@32c84fd9`; `CX2GUUser`, under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`), then `31fc7b40( 1 )` `+0x118` != 0, then user vslot `0xe8`( `32007b10( 0x8f, id, 0 )` ) true -> `m_bEventFlagList[0]` (`+0x7d85`) = true; on a later frame, with the flag set and the time past `+0x8c18` or the animation's `+0x2ac` set, StateChange `0x127` and the flag cleared (`@32c84f22`-`@32c84f32`). (CH1p review: window, vslot `0xe8` and the flag added.) Port it with the state; no `SAKP_*` / `m_kSkillSlotKey` here.

## Amended by CH2a (2026-11-09)

- **Registration shape:** CH2a put the Rena cases in the second skill switch, which is `switch( GetUpgradeBaseSkillID( eSkillID_ ) )` under `_LIVE` (`X2GULire_ElvenRanger.cpp`, "char-rena CH2a: June 32cb06d0 id switch"): add each skill's case inside the existing `#ifdef SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` block (before it ends, ahead of the CH1n default), both the base and the upgrade id as labels, table names from `ch0_skill_listing_rows.tsv`. Where June's hyper row names the same function addresses as the normal row, the hyper state copies the normal callbacks; June's hyper rows have no Init.
- **Helpers:** the foot-on-a-line cancels are `RENA_GROUND_WALK_CANCEL` / `_BWALK_` / `_DASH_` (CH1, `_CORE`) and `RENA_GROUND_ZX_CANCEL_EX` (CH2a, `_SKILL`); a state's `m_fSkillCancelAfter` etc. come from the script. A June FrameMove that lost its particles and damage effects keeps `ChangeWorldColorByHyperMode`, the cut-in and `CommonFrameMove` (the state's effect set carries the rest); June's memo tests call the always-false stub `31fca9e0` and are not written.
- **Debug flag:** `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` is defined for CH2a's play-test (its review retires it with `//#define`); CH2b re-defines it for its own `[JUNE_CHAR] RENA SKILL` lines (the equip line already exists).
- **Tooling:** CH2a's bytepatch spec was generated from per-function body files (`gen_spec.py`, a scratchpad script: function located by name, brace-matched, `#ifdef` after the `{` and `#endif` after the last body line); copy the idea, not the script.
