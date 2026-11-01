# June character Chung (Stage 7) - Phase CH1l30 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l30 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l30 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the shared `CX2GUUser::IsNotCommonMoveState` it completed) |
| Log tag | none (nothing new runs: the two added tests are false for every March class) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l30 - Done (2026-11-01)"
grep -a "Char Chung CH1l30:" june2017_re/funcmap.tsv
```

## Task

Check each CH1l30 reading in Ghidra / the exe bytes (read-only). In order of risk:
- **`31fd7640` = `CX2GUUser::IsNotCommonMoveState`:** the eleven state getters (vslots 0x438-0x45c and member `+0x7d04`) against March's names in the body
  (X2GUUser.cpp, `_ARME_CORE`), the riding range 0x4b-0x54 against March's `USI_RIDING_WAIT_HABIT`..`USI_RIDING_DASH_JUMP_LANDING` (`cl /P`), and the two
  tests the phase added: vslot 0x520 base `3208e710` (Now == vslot 0x438) and 0x578 base `31e4c550` (false). Are Rose (`32ae37f0`) and Luciel (`32b15340`)
  really the only overriders (`june2017_re/vtslot.py 0x520 CX2GU`, `0x578`), and is every call through those slots in 31fd7640 / 31fd78a0 (the mov+call scan)?
  Do the names `IsWaitState` / `IsClassCommonMoveState` hide nothing in a March subclass?
- **`3288fe90` = March's `GetCBConsumption`:** +0x84 / +0x278 minus the slot rows' bases +0x48 / +0x23c = 0x3c = `m_usCBConsumption` in March's
  `SkillSlotData` under `VERIFY_STAT_BY_BUFF` / `NEW_CHARACTER_CHUNG` / `ARA_CHARACTER_BASE` (`cl /P` the layout; stride 100), and the bound `> 4`.
- **`31f66620` = `DamageEffectData::operator new`:** pool `31f8c540` requested size 0xb8 vs ctor `31f8bf00`'s size row; the pool's critical-section mutex.
- **`31d70b60`:** the stand-in routing (Stage 12) matches Arme's and Eve's comment; the CH1l3 kickoff's address range `@32bf08b4-@32bf0929`.

Hygiene: `flagcheck JUNE_CHAR_ARME_CORE --range <CH1l30 commit>^..<CH1l30 commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUUser.*`
(the two `NPC_UNIT_ID` rows at X2GUUser.h:2550-2551 predate CH1l30). Write "CH1l30 - Review" into the plan doc, update the STATUS row (CH1l3 becomes
ready), check `kickoffs/char-chung/CH1l3.md` and commit with explicit pathspecs.
