# June enum names - Phase N4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-enum-names N4 review` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Rules" "Phase N4 - Aisha" "Phase N4 - Done"
```

## Task

Review N4's commit (`X2Lib/X2GUArme_VioletMage.cpp` only, plus the two-line `USER_ACTION` family in `tools/enumscan.py`). What it could get wrong:

- **A wrong name for a right value.** The generator replaced each `JUNE_*` constant by the enum member the scanner found for its value, and checked the name tail against the constant's. Three tails differed and were settled by hand: `JUNE_RNSI_A_AVP_SUMMON_BAT_HEAVY` (22217) -> `RNSI_A_AVP_SUMMON_BAT_ADD_TRANS` (June's name), `JUNE_CH1I_A_SHADOW_BODY_NAME` (2218) -> `SI_A_AVP_SHADOW_BODY` (March's name; June's own Enum has none, the JUNE_BUGS quirk is kept as a trailing comment at its one use), `JUNE_CH4C_SA_COMMON_AURA_SHIELD_ACCEL` (202) -> `SI_SA_COMMON_AURA_SHIELD_ACCEL` (the scanner picked `SA_DFG_CONSUME_REL`, a `SKILL_ABILITY_TYPE`). Check those three against `June_*` call sites and `june_src_fixed/data036_kom/Enum.src.lua`.
- **The 4 `JUNE_CH1I_UAI_*` stay numbers.** `0x25`, `0x37`-`0x39` are `USER_ACTION_ID` values that equal a BTI / BFI by coincidence; the scanner now reads `UAI_` as a family with no table (class X). Confirm nothing else in the six June files relies on the old class.
- **Behaviour must not change.** The throw-away `static_assert` probe covered 123 unique (member, literal) pairs, 0 errors, and a deliberately wrong pair failed. Re-run a spot check of 6 pairs by hand with `june2017_re/enumprobe.py`.
- **Class D lead `0x8b3` (l.~31068)** is `IF_EVENT_ENABLED( 0x8b3 )`, an event code, not `NUI_SECRET_SUMMON_MAG_NOM`. Agree or reclassify.

Hygiene: `python tools/flagcheck.py JUNE_ENUM_NAMES --range <N4 commit>^..<N4 commit>`; paste its `files:` / `RESULT:` lines. Gate: `python tools/enumscan.py --check X2Lib/X2GUArme_VioletMage.cpp` exits 0.

Fix clear errors under the same flag (then `tools/build.py --deploy`); write "Phase N4 - Review" into the plan doc, set the STATUS row, and commit.
