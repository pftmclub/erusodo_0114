# June character Chung (Stage 7) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH5 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH5 done (its Done note and commit) and the user's play-test + "check logs" |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_UI` |
| Log tag | `[JUNE_CHAR]` (`CHUNG UI cut-in set`, `CHUNG UI hyper cut-in`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH5 - Done (2026-11-02)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH5 - Review (2026-10-28, Opus 5.5): PASS, no code change"
```

## Task

Check CH5 against June (read-only Ghidra):
- **The table `333a96b0`:** 21 `SkillCutIn` entries at `337adca8` (stride 0x30, set stride 0x90 as `32bf0800` reads it: `337adca8 + set * 0x90`). Names in order:
  sets 0 / 1 / 2 / 5 = `HQ_CutIn_Chung_01`, `_11`, `_11`; set 3 = `_IP_01`, `_IP_11`, `_IP_11`; set 4 = `_DC_01`, `_DC_11`, `_DC_11`; set 6 = `_TT_01`, `_TT_11`, `_TT_11`.
  The last five entries are built by `32078b70` (= March's one-argument `SkillCutIn`: 512 x 512 at ( 256, 512 )); the first sixteen inline the same stores. Confirm the
  set order against InitState's class index (`32c22540`: 6 -> 0, 20 -> 1, 21 -> 2, 110 -> 3, 111 -> 4, 27 -> 5, 117 -> 6) and that March's table keeps the `#else`.
- **`UnitTemplet.lua`:** compare Chung's 7 blocks with `june_src_fixed/data036_kom/UnitTemplet.src.lua` (CRLF normalised only), and the other 48 against
  `kom_backup_tools/data036.kom.20261102_131543`'s extracted file. `luasrc.py ledger --check`, `core --check`, `luabind.py check UnitTemplet`.
- **Play-test lines:** one `CHUNG UI cut-in set <n> index 0 / 1 name HQ_CutIn_Chung_...` per class tested, each beside a `[JUNE_GFX] CUTIN play`; `CHUNG UI hyper cut-in`
  only for classes whose script sets `SHOW_HYPER_CUT_IN_TIME` (CH3d's hyper actives; none before then is not a failure).
- Hygiene: `flagcheck.py JUNE_CHAR_CHUNG_UI --range <CH5 commit>^..<CH5 commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp`.

Write "CH5 - Review" into the plan doc, update the STATUS row (CH6 waits on CH3d, CH4c and CH5 reviews), check `kickoffs/char-chung/CH6.md` and commit.
