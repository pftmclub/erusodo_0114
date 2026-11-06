# June character Raven (Stage 7) - Phase CH2d - Skills: Weapon Taker (25)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH2d` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2c review (and any `CH2d0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH2d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH2d - Done" "CH2d - Review"
```
Then CH0's and CH1's Done notes, and the `CH2d` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-25 skill ids class 4 lacks; `Effectset_A_RWT_FLAME_SWORD_MEMO` (7u). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH2d)`.
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

- **Functions: 41** (`awk -F'\t' '$7=="CH2d"' june2017_re/char_raven/ch0_class_map.tsv`; inside 39, new 2).
  June addresses: `32c27060` `32c270e0` `32c27110` `32c27120` `32c27130` `32c27160` `32c27180` `32c27190` `32c27930`(new RSI_A_RWT_MENTAL_BREAK_Init) `32c27a30` `32c355a0` `32c355d0` `32c35730` `32c357e0` `32c359d0` `32c35bc0` `32c35bf0` `32c35c20` `32c39630`(new RSI_A_RWT_MENTAL_BREAK_EventProcess) `32c39c70` `32c3e980` `32c3e9b0` `32c3e9e0` `32c42200` `32c422e0` `32c4b7d0` `32c4b9e0` `32c4edc0` `32c52b30` `32c52cc0` `32c52f30` `32c53110` `32c532f0` `32c59f30` `32c5a020` `32c5a1e0` `32c5a310` `32c5a400` `32c60510` `32c609c0` `32c60b30`.
- **States: 17** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_A_RWT_BARRAGE_ATTACK`, `RSI_A_RWT_BARRAGE_ATTACK_STAND`, `RSI_SA_RWT_HARPOON_SPEAR`, `RSI_SA_RWT_HARPOON_SPEAR_FIRE`, `RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT`, `RSI_SA_RWT_REVOLVER_CANNON`, `RSI_SA_RWT_REVOLVER_CANNON_LOOP`, `RSI_SA_RWT_REVOLVER_CANNON_FINISH`, `RSI_SA_RWT_REVOLVER_CANNON_END`, `RSI_SA_RWT_REVOLVER_CANNON_OVER_END`, `RSI_A_RVC_NAPALM_GRENADE`, `RSI_A_RWT_MENTAL_BREAK`, `RSI_SA_RWT_HELLFIRE_GATLING`, `RSI_SA_RWT_HELLFIRE_GATLING_LOOP`, `RSI_SA_RWT_HELLFIRE_GATLING_END`, `RSI_SA_RWT_HELLFIRE_GATLING_OVER_END`, `RSI_A_RWT_BARRAGE_ATTACK_AIR`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 24018 RNSI_SA_RWT_HARPOON_SPEAR, 24019 RNSI_SA_RWT_REVOLVER_CANNON_ORDNANCE_PENETRATOR, 24020 RNSI_SA_RWT_HELLFIRE_GATLING, 24031 RNSI_SA_RWT_HARPOON_SPEAR_UPGRADE, 24034 RNSI_SA_RWT_REVOLVER_CANNON_ORDNANCE_PENETRATOR_UPGRADE, 24051 RNSI_SA_RWT_HELLFIRE_GATLING_UPGRADE, 24212 RNSI_A_RWT_BARRAGE_ATTACK, 24213 RNSI_A_RWT_NAPALM_GRENADE, 24214 RNSI_A_RWT_MENTAL_BREAK, 24221 RNSI_A_RWT_BARRAGE_ATTACK_UPGRADE, 24236 RNSI_A_RWT_NAPALM_GRENADE_UPGRADE, 24237 RNSI_A_RWT_MENTAL_BREAK_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7 L256 class code scales the cached state power rate (+0x7cc8): March Raven A; §7 L256 (d) June class functions that DO write the cache: 32c4b9e0 and 32c5d3f0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- `RSI_A_RWT_BARRAGE_ATTACK_AIR` FrameMove `32c4b9e0` does `+0x7cc8 *= 1.2` (24212). `RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT` is here; `_ShowOverheatIcon` is CH1g2's.

## Amended by CH1p (2026-11-06)

- The `SAKP_*` held-key tests of Revolver Cannon (`32c59f30`, LOOP `32c5a020`), Harpoon Spear (`32c5a1e0`) and Hellfire Gatling (`32c5a310`, LOOP `32c5a400`) are already
  June's `IsEquippedSkillKeyPressed( m_InputData, GetFutureStateSkillID() )` under `_LIVE`; the whole-body port keeps that call and replaces the `_LIVE` block.
