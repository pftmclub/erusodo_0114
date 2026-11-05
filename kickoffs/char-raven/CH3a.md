# June character Raven (Stage 7) - Phase CH3a - Skills: Blade Master (106)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH3a` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2d review (and any `CH3a0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH3a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3a - Done" "CH3a - Review"
```
Then CH0's and CH1's Done notes, and the `CH3a` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-106 skill ids no first job has; the `BLADE_MASTERY` key readers if CH0 put them here. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH3a)`.
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

- **Functions: 34** (`awk -F'\t' '$7=="CH3a"' june2017_re/char_raven/ch0_class_map.tsv`; inside 25, new 9).
  June addresses: `32c268a0` `32c26980` `32c26d10` `32c26d70` `32c26d80` `32c275a0` `32c27630` `32c276d0` `32c27c00`(new RSI_SA_RBM_DEADLY_BLOW_Init) `32c27c10`(new RSI_SA_RBM_SHOOTUP_GENOCIDE_Init) `32c27c60`(new RSI_A_RBM_POINTED_STILETTO_Init) `32c28130`(new RSI_SA_RBM_SHAPT_CHARGE_Init) `32c311b0` `32c31350` `32c33420` `32c33480` `32c334c0` `32c335b0` `32c38770` `32c387d0` `32c38830` `32c38a30` `32c38b20` `32c3ad00`(new RSI_SA_RBM_DEADLY_BLOW_EventProcess) `32c3b040`(new RSI_SA_RBM_SHOOTUP_GENOCIDE_EventProcess) `32c3bd40`(new RSI_A_RBM_POINTED_STILETTO_EventProcess) `32c3c8a0`(new RSI_SA_RBM_SHAPT_CHARGE_EventProcess) `32c3dce0` `32c3e450`(new ?) `32c3e680` `32c3e950` `32c42010` `32c49930` `32c5c4a0`.
- **States: 15** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_A_RBM_SONIC_SLASH_LOOP`, `RSI_A_RBM_SONIC_SLASH_FINISH`, `RSI_A_RST_BLOODY_ACCEL`, `RSI_SA_RBM_GIGA_DRIVE`, `RSI_SA_RST_SHOCKWAVE_FRONT`, `RSI_A_RST_CUT_TENDON`, `SI_SA_RBM_WOLF_FANG_START`, `SI_SA_RBM_WOLF_FANG_STARTLANDING`, `SI_SA_RBM_WOLF_FANG_HIT`, `SI_SA_RBM_WOLF_FANG_HITLANDING`, `RSI_A_RBM_POINTED_STILETTO`, `RSI_SA_RBM_DEADLY_BLOW`, `RSI_SA_RBM_SHOOTUP_GENOCIDE`, `RSI_A_RBM_SONIC_SLASH_LOOP_UPGRADE`, `RSI_SA_RBM_SHAPT_CHARGE`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 24014 RNSI_SA_RBM_BLOODY_ACCEL, 24015 RNSI_SA_RBM_GIGA_DRIVE_LIMITER, 24016 RNSI_SA_RBM_SHOCKWAVE_FRONT, 24017 RNSI_SA_RBM_WOLF_FANG, 24035 RNSI_SA_RBM_DEADLY_BLOW, 24036 RNSI_SA_RBM_SHOOTUP_GENOCIDE, 24047 RNSI_SA_RBM_BLOODY_ACCEL_UPGRADE, 24048 RNSI_SA_RBM_GIGA_DRIVE_LIMITER_UPGRADE, 24049 RNSI_SA_RBM_SHOCKWAVE_FRONT_UPGRADE, 24050 RNSI_SA_RBM_WOLF_FANG_UPGRADE, 24056 RNSI_SA_RBM_SHAPT_CHARGE, 24210 RNSI_A_RBM_SONIC_SLASH, 24211 RNSI_A_RBM_CUT_TENDON, 24222 RNSI_A_RBM_POINTED_STILETTO, 24234 RNSI_A_RBM_SONIC_SLASH_UPGRADE, 24235 RNSI_A_RBM_CUT_TENDON_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RSI_SA_RBM_GIGA_DRIVE (scrip; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RST_CUT_TENDON (script; §1 L50 numerical-only DAMAGE_DATA table in state SI_SA_RBM_WOLF_FANG_START (s; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RBM_POINTED_STILETTO (; §1 L50 numerical-only DAMAGE_DATA table in state RSI_SA_RBM_SHOOTUP_GENOCIDE ; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- `32c3e450` (called by `RSI_SA_RST_SHOCKWAVE_FRONT` FrameMove `32c3e680`) is here by CH0's hand rule.
