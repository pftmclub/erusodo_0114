# June skill system core - Phase S7d - Skill pages, the reset item's UI, the HUD refresh

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7d` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7b (June's tree class, which hosts the page radios); not alongside S7c (both edit the tree class and `Handlers_Skill.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_PAGE` |
| Log tag | `[JUNE_SKILL] PAGE` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7d-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S5 -- Done (2026-10-18, Sonnet 5.5 high)" "Phase S7b - Done (2026-10-18, Opus 5.5 high)" "Phase S7d - Skill pages"
```
`MARCH_2014_MIGRATION.md` lines 1068-1100 (`SetUsingPage`: page numbers `>= 1`).

## Task

Split out of S7b (user, 2026-10-18: S7b = tree class, S7c = memo, S7d = pages).

**Pages (S0b amendment, 2026-10-17).** All three `Skill_Page` radios send 29049, which sets tree `+0x4b8`, refreshes, and sends the
decide REQ `3220f160` (REQ `0x47a` / ACK `0x47b`). The ACK handler is `31f1fd90`, and June's ACK carries a whole `KUnitInfo` (funcmap).
Expand-by-ED sends its REQ directly. Cash sets the shop category to `0x11`. `BUY_CONFIRM` -> `322149e0` when ED >= `+0x12c` (candidate
expand REQ). `BuySkillPageDialog` is `328dd1f0`, `UpdateSkillPageUI` is `328e6970` (adds the `0xffff` / `0x72b7` event-page state).
The emulator still has no handler or ignore row for the three page packets: give each June's handler or an `X2OfflineIgnore` row with a
reason. S7b leaves the page radios' UICustomEventProc cases as named stubs and calls March's page UI; replace them.

**Carried from S3c (2026-10-18).** June's `Handler_EGS_RESET_SKILL_ACK` (`322112d0`) also writes the page's SP / CSP into tree `+0x4a8` /
`+0x4ae` and refreshes the UI through `3214f050` / `328e7b70`. Port those with the page packets (S5 owns the slot loop). With the SP path
removed (S7b, user 2026-10-18) say what June shows from those fields, if anything.

**The tree reset item.** June's only reset is the item-driven `INIT_SKILL_TREE_REQ` (`3220e000`, ACK `3220ea50`, S7 finding). Check the
reset item's client path ends in S7b's tree (`UpdateUI`) and that the bar is re-filled (S7 noted: after a reset the emulator re-slots
castables while the client's Reset empties the bar until relog).

**HUD refresh (from S7).** `CX2Unit 3235a160` (HUD stat refresh through the June-only singleton `324943c0`, 23 callers): S7b's verdict is
in its Done note; port it here if S7b routed it here.

**Play-test:** switch skill pages (June's page UI), buy a page with ED, use the reset item and check the tree and the bar.
