# June skill system core - Phase S7g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7g review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7g done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM` |
| Log tag | `[JUNE_SKILL] HUD` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7g - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7g`.

## Task

Review Phase S7g against Ghidra (read-only):
- `CX2CashShop::JuneOpenAtItemList` (`X2CashShop.cpp`) against the raw `3224a5d0` (no Ghidra function; `python june2017_re/disasm_range.py 3224a5d0 3224a67b`):
  the `+0x134` guard (ported as `m_pBuyItemPopUp`), the product filter of `32229e00` (`32807970`, 0x4d-byte entries), the unconditional clear, and the call's place in
  `3224a680` (@3224ac87, after the menu-type tabs, before `+0x31c = 0`) vs March's `SetOpen` (`m_iMenuTypeCallingCashShop = 0`). Judge the mapping of
  `32229e00` / `322470a0( 1 )` onto March's `BuyItemPopup` `Reset` / `SetOpen( true )` / `SetChoicedItemBuy( false )`.
- The two call sites: `328ec476` (127273) and `32478410` (274333, case 0xa5 of `32476050` = March `SMUCM_DISABLE_QUICK_SLOT_MOUSE_UP`: check the case number against
  the SMUCM enum with `cl /P` or the jump table), and the four "no March UI" sites (`32398159`, `324a6fbf`, `3260d801`, `3260dff1`).
- The item-1 identification that routed S7j: `324943c0` = `CX2GageManager::GetInstance`, `3248a800` / `3248d3a0` = SetMaxMp / SetMaxHp, `3235a160`'s use of the CX2Unit
  socket aggregate `+0xe1c`, and the `3234b9d0` skill-id list (17 `RNSI_*` ids, March's 2402 / 5402 / 5410 / 7402 absent).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM --range <first phase commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7g -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7j), and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG`:** the retire note lives in `S8-review.md`; leave it there.
