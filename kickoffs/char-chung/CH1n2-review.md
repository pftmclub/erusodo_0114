# June character Chung (Stage 7) - Phase CH1n2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n2 done (its Done note and commit) and the user's play-test of CH1n2 |
| Flag | `SERV_IRUHADEV_JUNE_USER_BASE_STATE_BODIES` (every class), `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none new (bodies only) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1n2 - Done (2026-11-02)"
awk -F'\t' '$5=="DIFF" && $1 !~ /^Riding/' june2017_re/char_chung/ch1n_user_callback_triage.tsv
grep -a "Char Chung CH1n2" june2017_re/funcmap.tsv
```

## Task

Check CH1n2 against June (read-only in Ghidra; `python june2017_re/charport_adis.py chung <va>` for the listings). In order of risk:
- **StartEnd `3201b460`:** the gate (vslot 0x198, `g_pMain +0x16a`, `+0x8b61`), the room test `[0x33775868] +0xd8 == 6` read as `PGT_WINNER` of the PvP
  room (`m_PVPGameType` first in CX2PVPRoom, `m_DifficultyLevel` first in CX2DungeonRoom: confirm with a June reader of each), game type 5, the PvP
  branch (`GameFightNew`, then `m_bStarted`), the dungeon branch (dynamic cast, `+0xc44`, the three dungeon ids, `32070f40` stand-in, defence test,
  `ClearScreen( 10 )`), and that March's `CommonStartStateEnd` (empty) and party-bonus particle are rightly gone.
- **HyperModeStart `31ffd490`:** Open Mind (`0x5f63`, `GetSkillLevelValue` = `31fceab0`, `SetSkillBuffFactors( id, 1, true )` = vslot 0x23c) sits inside
  the `+0x8fc3` branch after the count; the event 0x73d stand-in; `+0x9176` = `m_bReserveHyperModeZero`.
- **THROW_ITEM_FrameMove `31ff92d0`:** both jump tables from the exe bytes (`31ffaa38` with `dec eax`, `31ffaa70` from 0x3fd), which cases set `pEffect`
  (velocity tail `31ffa9b2`) and which do not; the 9 SST names against `Enum.src.lua`; StateEnd's Lofty case; EventProcess's foot test.
- **The facing helper** `UpdateFutureIsRightByArrowKeys` against each inlined copy (walk `31fd23eb`, jump `31fd247b`, dash `31fd262e` after the speed,
  pepper run `31fd1ca4` / `31fd1da4` / `31fd1e77`), walk's 0 speed and jump's untouched speed when no arrow is held.
- **Pepper run:** `+0x8fc1` skip in both JUMP_UP bodies (`m_bOnJumpUpLine`, ctor zero = June ctor `32040f1e`), the `UT_EVE` null-safe templet test, the
  PEPPER_RUN_END stand-in.
- **Dash starts:** common first, the pet getter stand-ins; **DamageAirSmallEventProcess:** `ASUT_DAMAGE` while airborne, and that every class's
  lookup reaches `CheckSkillUseCondition` with it (March's classes and the ported Aisha / Eve).

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_USER_BASE_STATE_BODIES --range <CH1n2 commit>^..<CH1n2 commit>` and `JUNE_ENUM_NAMES` over the four enum headers,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUUser.cpp`, a one-file probe of `X2GUUser.cpp`. Judge the
user's play-test from the hand-off. Write "CH1n2 - Review" into the plan doc, update the STATUS rows (CH1n3 and CH1o become ready), check
`kickoffs/char-chung/CH1n3.md` and commit with explicit pathspecs.
