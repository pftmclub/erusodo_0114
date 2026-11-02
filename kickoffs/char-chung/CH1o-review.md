# June character Chung (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1o review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit); no play-test (every change is under the undefined `_LIVE`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1o - Done (2026-11-02)"
awk -F'\t' '$7=="CH1o"' june2017_re/char_chung/ch0_class_map.tsv
grep -a "ResetAnimSpeed" tools/data/june_march_divergent.tsv tools/data/june_diverge_allow.tsv
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7av. A class's state animation rate, user vslot 0x824 (from char-chung CH1o)"
```

## Task

Check CH1o against June (read-only; `python june2017_re/charport_adis.py chung --phase CH1o --out <scratch>` and `charport_adis.py chung 31ffe190`; read
listings, not decompiles). In order of risk:
- **`ResetAnimSpeed` `31ffe190`:** the 0x824 call `@31ffe2b0` sits after the item / Elsword-vigor rates and before the `m_fAnimSpeedRate` floor, as the `_LIVE`
  branch puts it; March's Lire block still compiles (`int iSkillLevel = 0`). Confirm the routed rest (`+0x92a0`, `+0x8e90` / `+0x8e84`, Lire's override) is
  everything else that differs, and that no other June caller reads user vslot 0x824 (CH1o's sweep: `31d30380` and `322de6c0` are other objects).
- **`ShownSkillCoolTime` `32bdba60`:** arguments ( cool `[esp+4]`, id `[esp+8]` ), `fldz; fcom [+0x98d4]; test ah,5; jnp` = return 0 when `0 < +0x98d4`;
  id `0x65b7` = `RNSI_SA_CDC_SHOOTING_BIND`.
- **`IsBerserkByChance` `32c24b90`:** `fcompp` order (member is ST0) and `test ah,0x41; jne` = true when member > roll; `+0x9714`'s writers in
  `InitPassiveSkillState` (`@32c1757e` 0, `@32c175d3` skill 26408 `SA_EFFECTIVE_RATE`) are CH4a's.
- **`InitRemodelMagazinePassiveSkill` `32bf40e0`:** id `0x6726` = 26406 `RNSI_P_CSG_REMODEL_MAGAZINE` (registered by CH1o), `GetSkillLevel( id, 1, 0, 1 )`,
  `SA_EXPAND_CB` (0x66), gauge setter `32a36ba0` = `SetMaxCannonBallCount` (protected `+0x11c`).
- **The verdict rows:** `32c24b50` / `32c24b60` = March; `32bdc520` / `32bdc600` / `32bf0ce0` / `32c24930` = March; `32c24e50` / `32c24ed0` = the dungeon / PvP
  start-count setters (CH0's March names were wrong); the routed rows (CH3b `32bfeb80`, CH4a `32bdba10` / `32c12910`, CH4b 0x740 / 0x744, CH4c `32bdb760`)
  and the n/a rows (std templates, the dead `32c08970`'s callees, dtor adjustor thunks `32c24b30` / `32c24b40`).
- **The 0x670 / 0x774 decision:** June calls both slots only from class code on the class itself, so they stay plain members; spot-check two callers each.

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1o commit>^..<CH1o commit>` (FAIL only for the `//#define`), the same for `_CORE` and
`JUNE_ENUM_NAMES`, `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`, one-file probes of
`X2GUChung.cpp` and `X2GUUser.cpp` with `_LIVE` off and on. Write "CH1o - Review" into the plan doc, update the STATUS rows (CH1p becomes ready), check
`kickoffs/char-chung/CH1p.md` and commit with explicit pathspecs.
