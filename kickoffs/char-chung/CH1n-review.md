# June character Chung (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1n - Done (2026-11-02)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1n - Review (2026-10-27)"
grep -a "Char Chung CH1n:" june2017_re/funcmap.tsv
```

## Task

Check CH1n against June (read-only in Ghidra). In order of risk:
- **The head of `32c1f530`, against the listing** (the decompile merges paths through shared labels): bounds before `3207c1a0`; the four tables
  `33507b00` / `7b14` / `7b28` / `7b3c` from the exe bytes; `0x101` on the hyper entry; `31fca8e0( id, 1 )`; the memo test (`31fdbcc0`, then
  `32206450( memo, 1 )`) and its cases 0x65ae / 0x65c4, 0x666c, 0x666d / 0x667b, 0x6671 with their default (nothing). For each case: table names,
  the callbacks behind each `LAB_` thunk, which slots the hyper entry copies (`+0xd/+0xe/+0x11/+0x12` FrameMove / EventProcess; Rapid Cannon sets
  StartFuture / Start / EventProcess and `+0x10`), the sub-states (0xe9 / 0xea, 0x13e / 0x13f) and which delegates survive each `320775a0` Init.
- **Type switch:** case 5, 6 / 7 / 0xd-0x10 (`> 0x67e8` switch, `== 0x67e8`, else the id switch), 0xc + 900, default; and that `@32c220ae` (both
  defaults) equals `SetLunaticBlowSkillFunc` and March's Lunatic Blow case. Confirm `ST_*` values 5 / 6 / 7 / 12 / 13-16 in March's enum.
- **Call site:** March's head and type switch are the `#else`; the id switch's `default:` under `_LIVE` calls the helper; nothing follows the id switch.
- **Enum names:** the 9 values against `Enum.src.lua`; no other `SKILL_ID` member shares them.
- **The base-callback triage** (`june2017_re/char_chung/ch1n_user_callback_triage.tsv`): re-read at least the 16 rows CH1n2 owns and 5 SAME rows
  against the listings (`charport_adis.py chung <va>`); a SAME that is really DIFF moves to CH1n2 (kickoff) or Stage 17 (riding). Check that the
  5 kept rows are what the Done note says.
- `32c24400` = March's `CreateFuryGuardianGuardSkill` (class map row).

Hygiene: `flagcheck` for `_CORE` / `_LIVE` (`--range <CH1n commit>^..<CH1n commit> --files X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`) and `JUNE_ENUM_NAMES
--files X2Lib/X2SkillTree.h`, `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp`, a one-file probe of
`X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Write "CH1n - Review" into the plan doc, update the STATUS rows (CH1n2 becomes
ready), check `kickoffs/char-chung/CH1n2.md` and commit with explicit pathspecs.
