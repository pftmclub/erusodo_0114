# June character Eve (Stage 7) - Phase CH0b - Every June Eve asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH0b` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_EVE_ASSET` |
| Log tag | `[JUNE_CHAR]` (`EVE DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Eve asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH0b - Done (2026-10-20)"
```
Then CH0's Done note and `june2017_re/char_eve/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", the pilot's way (Aisha's CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_eve.txt`.
2. `kom.py stage-june --dry-run --plan-out ...` then the real run over `ch0b_stage_list.tsv`, byte-exact.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve. **If CH0 found a dropped animation March plays,**
   pack but hold the motion koms for CH1 final and say so.
4. Hand over one play-test: one Eve per job tier (the save has units 19 / 27 Empress, 22 Nasod, 41 Electra; add others with
   `/x2-create-char` if CH0 asks), a costume and a weapon, three skills, village and dungeon.

A file the list missed (a missing-file line in the play-test) is staged here and added to `ch0_assets.tsv`. Never keep March's copy to make
something load.

## Amended by CH0 (2026-10-26)

- **Checklist rows** (`ch0_checklist_rows.tsv`): §7ad L723-744 (motions).
- **The list:** `june2017_re/char_eve/ch0b_stage_list.tsv`, **5,197 files** (4,648 missing, 549 March copies; texture 3,446, mesh 1,450, sound 219, motion 65, other 17). 9,042 more of her files are June's bytes already. `kom.py stage-june --dry-run` over the list: 5,197 staged, 0 refused, placement equal to the list's column. Rebuild with `python june2017_re/char_eve/ch0_build_assets.py eve` (~4 min).
- **Koms:** 40 placement koms (largest data018 2,538, data192 695, data130 402, datai010 317, data198 293, datai023 249, data094 191; the map is the `# stage kom map` line). **One new kom dir: `data203`** (54 June emotion motions, no March archive yet: the copy creates a new `data203.kom`, say so when asking); `data203 -> data033` 10 base motions.
- **The motion question (answered): no animation is lost.** June moved `SI_SA_ENS_ATOMIC_BLASTER` from `Motion_EVE_NS.x` to `Motion_EVE_EG.x` and `SI_A_EBS_TASER_PILUM` from `Motion_EVE_BS.x` to `Motion_EVE_PR.x`; March's Code: Nemesis row loads EG + NS and Battle Seraph loads PR + BS (UnitTemplet l.664-689, 1053-1078), so March's script still finds both. `Motion_EVE_Common.x` only renames `PR_LobbyDownlanding` -> `PR_LobbyDownLanding` (its one March reader, `X2UnitViewerUI.cpp:4855`, is in a dead `#else`). Common is June's bytes already. **No koms are held for CH1**, but copy the 11 base `Motion_EVE*.x` together (never NS without EG, or BS without PR). Evidence: `ch0_motion_question.tsv`.
- **Readers a June asset could break:** none found beyond the motion moves above.
- **Not staged here, with owners:** the 4 `_INT.txt` particle / mesh-player definition files (March's, datai015; shared by every character; Eve's templets already compare June-equal: effect plan); 243 `Tutorial_Eve_*` / `Tuto_Eve_*` files (june-dungeons); `PET_Mystic_EVE_3` + pet assets (Stage 17); `POPUP_TR_*` (Stage 6, done there); 113 names neither March nor June has (`dangling`).
- **Play-test units:** the save has 22 (Nasod 5, lv99), 19 / 27 (Empress 109, lv80), 41 (Electra 26, lv90); create Exotic Gear 18, Architecture 19, Code: Nemesis 108 and Battle Seraph 116 with `/x2-create-char`.
