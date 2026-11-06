# June character Raven (Stage 7) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1k review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1k - Done"
python june2017_re/charport_adis.py raven --phase CH1k --out <scratchpad dir>   # 9 listings: the ctor 32c3ece0 (2,000 lines of KProtectedType registry code), 32c58c30 (lookup) and 32c43bf0 (cooldown) are the big ones
python june2017_re/charport_adis.py raven 32c3e450                               # the cut-in callee of 32c3e770 (CH3a's row)
```

## Task

Review the nine CH1k rows (plus `ReservedSkillStart`) against the listings. Check as hard as the Done note's claims:
- **Lookup `32c58c30`:** the order of the checks (quick-slot wait, `CommonSpecialAttackEventProcess`, templet id against the slot's with 0x8bae, chosen id stored, Winnable Technique `0x5f5c` = 24412 learned with `GetSkillLevel( id, 1, 0, 1 )` then the two `328905b0` swaps `0x5e94` / `0x5e89` against the templet and cond 2 / 4 -> 1, the four `Check*` calls, riding key wait, parrying + `328905b0( 0x5e88 )`, the Shadow Step chain block (`+0x9728`, `+0x9721`, `+0x9704` > 2, stage 0xd8 -> 0x99, returns true at 32c59515 with no MP and no slot write), `GetSkillLevel( id, 0, 0, 1 )` clamped to 1, vslot 0x6a0 `( id, level, 1 )` into `+0x8eb0`, the vslot 0x82c gate and the three refusals 226 / 2549 / 26119 (`31fd0570` for the training test, `GetNumItemByTID( 0x1fca0 )`)).
- **Use `32c25ba0`, event process `32c25af0`:** the caller's condition 3 / 5 (the argument at `[esp+0x18]`, not the templet) gives the three values back; used -> `+0x88bc` = slot + 1.
- **Cooldown `32c43bf0`:** `IsMyUnit`, `MarkSkillSlotUsed`, vslot 0xf0, the finality stone, the level (real, unlike Elsword's JUNE_BUGS 524), `32207210( id )` against 24200 gating `SetSkillCoolTimeLeft`, the rage gain `( +0x6c50 + 1.0 ) x mp / 3.0` (`+0x6c50` = specialization A, Eve CH1q) and its clamp 100, `+0x8ac0` = slot + 1, the 0xd8 reset, `3235a710`, `31fcd3d0`.
- **Constructor `32c3ece0`:** the six `_CORE` inits, the `0.25f` at `m_fLimitOverHeatHP`, and that every March default not named in the Done note equals June's (list in `ch1_offsets.tsv`; the Shadow Step members `+0x9654` / `+0x9664` / `+0x972c` / `+0x9730` are routed to CH2a / CH4a).
- **Equal rows:** `32c25c30` (false), `32c3e770` (March's call), `32c25c40` (base `32008e40`), `32c3e0e0` (`!= 0x9a`).
- `ReservedSkillStart` and the `X2GUUser.h` / `.cpp` chain.
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`; `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, not `/D`), `cl /EP` string counts, `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range <commit>^..<commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1m kickoff (Amended by CH1k) in the Review note.
