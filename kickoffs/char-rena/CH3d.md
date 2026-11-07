# June character Rena (Stage 7) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3d` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*`, `X2Offline*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`), `SERV_IRUHADEV_JUNE_CHAR_RENA_TRANSCEND` |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`) |
| Review | `kickoffs/char-rena/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3d - Done" "CH3d - Review"
```
Then CH0's Done note and the `CH3d` rows of `ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.

## Task

Do the CH3d row of "Phase CH3": hyper actives (type 13, 6 rows; June's hyper switch in `32cb06d0`), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the
transcendence rows the template lists: classes 102 / 103 / 114 on the offline transcend allow-list (`CX2OfflineServer::TranscendCheck`, `Handlers_Quest.cpp`),
`LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Lena.lua` (March ships a same-name file: June's replaces it), checklist 7ao whole (hyper
effect start `32c9b3d0`, uncarved; the four base helpers `31fcac90` / `31fcad70` / `31fceab0` `== 1` / `31ff1e80`; l.969), `SPRT_HYPER_CLASS` skills and
`TRANSCENDENTAL_SKILL_DATA` rows, `GetHyperModeTableName` (`31fdd490`) and `HQ_Cutin_RWS/RGA/RNW_Transcendence`, `list_TR_*` rows in `GetCharacterImageName`
(T6c) read from June's row pushes, `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER` present. Then `python tools/skillicons.py check <Rena's skill-id range>`
= PASS.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps. CP949 files through `bytepatch.py`. Mark rows `(Rena: CH3d)`.
Hand-off: a transcended unit (`settranscend`) per second job, the hyper-class skills in A-5 / B-5 (hyper-class passives read level 0 until transcended),
the transcended picture and cut-in.

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

- **Functions: 22** (`awk -F'\t' '$7=="CH3d"' june2017_re/char_rena/ch0_class_map.tsv`; inside 22).
  June addresses: `32c6d0b0` `32c6d1a0` `32c6d1c0` `32c6d290` `32c6d350` `32c6d3f0` `32c7f320` `32c7f350` `32c7f3a0` `32c7f6c0` `32c7f6f0` `32c7f720` `32c7faa0` `32c7fad0` `32c86b20` `32c88330` `32c9eb20` `32c9ece0` `32c9ef20` `32c9efd0` `32c9f490` `32ca7020`.
- **States: 8** (`awk -F'\t' '$8=="CH3d"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_HA_LWS_SPIRAL_STIKE_READY`, `LESI_HA_LWS_SPIRAL_STIKE_SPINING`, `LESI_HA_LWS_SPIRAL_STIKE_FLYING_ATTACK`, `LESI_HA_LGA_CRYOTRON_BOLT_GUIDE_SHOT`, `LESI_HA_LGA_CRYOTRON_BOLT_CHARGE_SHOT`, `LESI_HA_LGA_CRYOTRON_BOLT_DOWNLANDING`, `LESI_HA_LNW_INNOCENT_START`, `LESI_HA_LNW_INNOCENT_END`.
- **Skills: 6** (part 2 of `ch0_state_skill.tsv`, col 6): 23600 RNSI_HA_RWS_SPIRAL_STRIKE, 23601 RNSI_HA_RGA_SHOOTER_DESTINY, 23602 RNSI_HA_RLW_INNOCENT, 23603 RNSI_HA_RWS_SPIRAL_STRIKE_UPGRADE, 23604 RNSI_HA_RGA_SHOOTER_DESTINY_UPGRADE, 23605 RNSI_HA_RLW_INNOCENT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7t L592 SHOW_HYPER_CUT_IN_TIME in the class script; §7t L594 June UnitTemplet.lua m_HyperCutIn / m_BerserkCutIn keys and the HQ_Cut; §7t L607 hyper active skills: cut-in shown whatever the option says (type 13); §7u L638 March-only effect templets the re-adoption dropped: DamageEffect_LGA_A; §7u L644 Cryotron Bolt GUIDED_ARROW is a June case rename (Guided_Arrow); the R; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ag L814 TC slot A-5 / B-5 states and the Stage 6 skips; §7ao L969 transcend: Rena hyper effect start 32c9b3d0 (vslot 49) returns at once; §7ao L951 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH3d`): 3; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- **Callees this Opus phase maps itself** (`ch0_unmapped_identified.tsv`): `322dea10` ? game-wide unit scan: for every unit in the vector at this+0x304 (not the exclu.
- 7ao: HyperModeBuffEffectStart `32c9b3d0` (transcended return) and the three `HQ_Cutin_R{WS,GA,NW}_Transcendence` rows.
