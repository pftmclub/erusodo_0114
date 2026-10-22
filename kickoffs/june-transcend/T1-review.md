# June transcendence - Phase T1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T1 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T1 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT` (`Always.h` and `ServerDefine.h`) |
| Log tag | `[JUNE_TRANSCEND] UNIT` / `DB` |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T1 - The flag: unit, wire, save, emulator, switches" "T1 - Done"
```

T0's map: `june2017_re/transcend/t0_readers.tsv` (owner T1 rows) and funcmap rows `Transcend T0`.

## Task

Review T1's commit against June in Ghidra (read-only):
- **Member position and wire order.** `m_iUnitClassTran` sits right after `m_cUnitClass` in `KUnitInfo`, `KRoomUserInfo`, `KSquareUserInfo`, `KFieldUserInfo`, with Put and Get in the same place (June setters `3235eb20` +0x17, `32360090` +0x23, `32361e00` +0x4c, `323614c0` +0x26). Check `CommonPacket.h` copy operators (Square, Field) copy it.
- **Unit.** `UnitData::m_iUnitClassTran` is June CX2Unit `+0x3c`; setter `3234b4a0`; the four setters copy it.
- **Rung v22.** `SCHEMA_VERSION` ladder, `UNIT_COLUMNS` order against `ReadUnitRow` (column 23), the `ALTER`, `iFrom < 22`.
- **Stand-ins.** Each `#else` keeps the old body; the `!= 0` vs `== 1` tests (`31fceab0` tests `== 1`; the save only holds 0 / 1) ; `IsForbiddenQuest` compare `-1 != tail && my != tail && not QT_EVENT` against `3232e510 @3232ea22`.
- **Not touched on purpose** (T2 / T4): slot-4 / 9 skips, emulator equip refusal, 42030 refusal, `X2StateMenu.cpp:1885`.

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_UNIT --range <T1 commit>^..<T1 commit>`; paste its `files:` / `in this flag's files:` / `RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T1 - Review" into the plan doc, update the T1 row of `STATUS.md`, check that `kickoffs/june-transcend/T2.md` and `T4.md` still read right, and commit.
