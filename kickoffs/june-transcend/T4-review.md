# June transcendence - Phase T4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T4 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_SLOT` |
| Log tag | `[JUNE_TRANSCEND] SLOT` (`python tools/logs.py --grep "JUNE_TRANSCEND SLOT"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T4 - The TC slots (A-5 / F and B-5 / T)" "T4 - Done"
```

T0's map: `june2017_re/transcend/t0_readers.tsv` (owner T4 rows) and funcmap rows `Transcend T0`.

## Task

Review T4's commit against June in Ghidra (read-only):
- **The skips.** June `32024b00` loops all five slots of a bar (`SetEquippedSkillLevelCommon`); confirm there is no skip in its callers and that
  `SetEquippedSkillFunc` / `ChangeEquippedSkillState` (`X2GUUser.cpp`) now do the same. The lock is `3220e3b0` (equip refused) and `328e18d0` (unequip).
- **The press.** `31fcadd0`: F (`+0x872b`) returns 4 with no lock test, T (`+0x872f`) returns 9 only while `32890ee0( 1 )`. Aisha's
  `JuneCommonSpecialAttackEventProcess` is that; March's `CommonSpecialAttackEventProcess` now hands a March class no slot for F / T (the T4 guard):
  check no other March class reads `oneSkill5` / `oneSkillEx5`.
- **The click.** `32476050` case 182 (`+0x3c == 0`, item 0x4663f = 288319) and 183 (`32890ea0( 1 ) == 0`, item 0x4663e = 288318): the text when
  `32530a80` (game) and the state is not 0x15, else `+0x31c = 0x32` and the shop on the item. Menu type 0x32 -> tab 3 (`3224a680`): check the mapping
  to March's consume tab (`X2CashShop.cpp:2735`).
- **322d8100.** T4 found it is the guide-4 TC-slot particle, not a cool-time loop (routed to Stage 9 in `JUNE_PORT_BACKLOG_ORDER.md`). Confirm.
- **Emulator.** `X2OfflineSkill.cpp` refusal and key 1 follow the unit flag; the medal case in `Handlers_Inventory.cpp` (its untranscended
  refusal id is 1686 since T3, under `_ITEM` inside this flag's block; T3 review checked the id and text).
- **Stand-in labels.** Key 1 permanent and the medal's consume-only use are labelled in `MODS.md`.

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_SLOT --range <T4 commit>^..<T4 commit>`; paste its `files:` / `in this flag's files:` / `RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T4 - Review" into
the plan doc, update the T4 row of `STATUS.md`, check that `kickoffs/june-transcend/T5.md` still reads right, and commit.
