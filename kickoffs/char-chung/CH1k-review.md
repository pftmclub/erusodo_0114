# June character Chung (Stage 7) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1k review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` / `_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1k - Done (2026-11-01)"
grep -a "Char Chung CH1k:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH1k --out <scratch>     # June listings, 38 rows
```

## Task

Check each CH1k transcription against June in Ghidra (read-only). The ones that differ from March, in order of risk:
- **`DamageReact` `32c0cf90`:** the 7p hit stop (`@32c0d18c` / `d19c` / `d1b4`: attacker, own, own second stop), the guard-damage branch's `GetInvincible`
  (June `bAttackerFront && !GetInvincible()`, March `bAttackerFront` only), the reduce-percent sums (100 / 80 + guard), the `m_fBeShotCannonBallRelTime`
  block, the two mesh-handle clears (slots 0x11 / 0x12 = `CHUNG_MAJOR_MII_CSG_COMBO_DASH_ZX_MINI_GUN_L` / `_R`) at the end for every reAct type.
- **`ReInit` `32c0ca60`, `InitComponent` `32bf4900`:** the PvP / PvE test (game type 1 or 4), which start count each branch reads (`+0x9674` dungeon /
  `+0x9680` PvP; LoadPersonalInfo `32bf5430` keys `CANNON_BALL_COUNT_AT_DUNGEON_START` / `_AT_PVP_START`), the training game's 3, the
  INIT_COMPONENT tables and their member offsets (`+0x9868`, `+0x9878` = the spine aim limits CH1j named, `+0x9898`, `+0x98c4`).
- **`HyperModeBuffEffectStart` `32c063d0` / `End` `32c06750`:** the transcended (`CX2Unit +0x3c`) and awakening gates, the cannon ball refill at the start,
  `+0x98b8 = -100` at the end. `ApplyRenderParam` `32beeed0` with `IsNowStateChaosCannon` (`32beeb90`, 12 contiguous states).
- **The five `_CORE` functions** (`GetDefenderDamageRate` `32bed3d0`, `GetDamageRateOnCommanderMarkedDefender` `32bed7e0`,
  `GetDamageRateOnExcitingCrashDefender` `32bed840`, `IsNowStateChaosCannon`, `GetOpenMindScaledValueInGuardState` `32beec40`): ability ids, skill ids,
  names. `32beec40` is a stand-in (needs Opus): find June's `CX2GameUnit +0x567c` member and the vslot 0x1b8 caller (it is read by common code).
- **Dtor `32c0c4e0`** (Heavy Field NPC 2525), `GetComboXStateID` `32beaf70` (class 111 -> 206), the ctor map `june2017_re/char_chung/ch1k_ctor_map.tsv`
  (the rows without a March member: which phase maps them), `DeleteMajorMesh` `32bf50e0` against the 19 June slots and March's `CHUNG_MAJOR_MII_END`.
- The `= March` rows (copies written in the `_LIVE` branch): spot-check four of them.

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range <CH1k commit>^..<CH1k commit>` (its "-9" is a git alignment
artifact of the LoadPromotionItemID copy: `git diff --patience` shows 0 deleted lines), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.*`,
both one-file probes. Write "CH1k - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1l0.md` and commit with explicit pathspecs.
