# June enum names - Phase N3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-enum-names N3 review` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N3 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Rules" "Phase N3 - Eve (\`X2GuEve.cpp\`)" "Phase N3 - Done"
```

## Task

Review N3's commit (`X2Lib/X2GuEve.cpp`, `X2Lib/X2DamageManager.h`). What it could get wrong:

- **A name is not the value's real name.** The 93 sites were probed (`enumprobe.py`: every new name equals the literal it replaces), so arithmetic is covered. Spot-check 6 names against `june_src_fixed/data036_kom/Enum.src.lua` for the *meaning*, not the value: `RNSI_SA_ECB_ENERGETIC_HEART` (was the misnomer `BOOST_ATTACK_MP`, 25029), `NUI_ECA_OBERON_GUARD` (0x82b), `BTI_BUFF_FERDINAND` (409), `BFI_BUFF_CODE_RECOVERY_ECB_PVP` (0x45f), the `RNSI_A_*_AURA_*` accel cases, `DTT_EVE_MANEUVER_ATTACK` (5).
- **The one header edit.** The scanner offered `DTT_END` for the literal 5 (a count that happens to equal June's `DTT_EVE_MANEUVER_ATTACK`). N3 registered `DTT_EVE_MANEUVER_ATTACK = 5` after `DTT_END` in N2's block, inside `#ifndef ADD_EVE_SYSTEM_2014` (that macro is off in the US build; March's own member sits under it). Confirm nothing else breaks if `ADD_EVE_SYSTEM_2014` were ever defined, and that no implicit-numbered member follows.
- **The `JuneEveAccelCoreParticleName` rewrite** (`X2GuEve.cpp`, the last block, ~l.44600): the `if( 0x765d <= id && id <= 0x765f )` range became three `case` labels (30301 / 30302 / 30303). Confirm the old range really covered only those three ids (30301..30303 = 3 values) and the `#else` keeps the range.
- **Aliases kept.** Local constants (`JUNE_CH1A_P_EBS_OPEN_MIND`, `JUNE_RNSI_A_EBS_ELECTRONIC_MOVE`, `JUNE_CH1L_*`, `JUNE_CH1K_*`) now initialise from the enum member; the duplicates (25419, 25223, 0x655 / 0x656, 0x180 / 0x207) are still separate names. Decide whether collapsing them (about 40 use sites) is worth a follow-up; it is not needed for the plan's gate.
- **Left as numbers on purpose:** `JUNE_CH1L_ENSI_SA_ECE_REASON_PRIDE_ACTION = 242` (CH3b), the four `0x8fe1..0x8fe4` STR_ID constants (class X).

Hygiene: `python tools/flagcheck.py JUNE_ENUM_NAMES --range <N3 commit>^..<N3 commit>`. Paste its `files:` / `RESULT:` lines. The "25 `#else`/`#endif` without the flag token" line is the bare `#else` of the house style; the two "deleted lines" in `git diff` are blank-line alignment, flagcheck's unifdef proves flag-off = base.

Fix clear errors under the same flag (then `tools/build.py --deploy`); write "Phase N3 - Review" into the plan doc, set the STATUS row, and commit.
