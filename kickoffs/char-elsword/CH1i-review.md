# June character Elsword (Stage 7) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1i review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1i - Done (2026-11-04)"
awk -F'\t' '$7=="CH1i"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the four CH1i functions and the one callee against June (`python june2017_re/charport_adis.py elsword 32cf5210 32d05960 32d08c30 32d1b720`; `32028190` is the base `CX2GUUser` virtual, already read for Chung). The risk sits in these:
- **SetEquippedSkillLevel `32cf5210`**: the block order ( 0x5208 / 0x5212 / 0x5218 / 0x52d5 / 0x52db / 0x52d6 / 0x522b / 0x5242 ), each `bl == 1 || id` guard, and the state ids passed ( 0xea 0xeb; 0xbe 0xbf; 0xc0; 0xca-0xcd; 0x103; 0xe4-0xe7; 0x106; 0x107-0x109 ). The CH1i names were derived, not read, for 0xca-0xcd ( `ESSI_A_ELK_ROLLING_SMASH` + `_HYPER` + `_LANDING` + `_LANDING_HYPER`, from the Lua order ) and 0xe4-0xe7 ( the March `ESSI_SI_A_ES_DEFENCE_*` four ): confirm them against a registration of those ids in `InitState` `32d23ff0` / `32d1f8f0` / `32d223e0`, or against the Lua `ESSI_*` order, and fix a wrong name. The Windmill tail ( null unit returns the whole function, `GetSkillLevel( id, 0, 0, 1 )` max 1, `SA_EFFECTIVE_TIME` -> `+0x95a0` ), and the unchecked templet before 0x106.
- **IsInJobLine `32d05960`**: the class constants ( 0xa / 0x64, 0xb / 0x65, 0x16 / 0x70 ) against `X2Unit.h`, line 1 / 2 / 3 mapping, no-unit = false.
- **CanRidingState `32d08c30`**: the ten state ids ( 13 14 19 20 76 77 75 81 82 206 ) against `X2GUUser.h` / `ESSI_SWORD_WAIT` ( June 206; March's value is set by CH1m ).
- **HyperModeFrameMove `32d1b720`**: the two timers 0.05 / 0.83 and `bSound` = 0.
- **SetEquippedSkillLevelStateData**: matches the Chung body ( `X2GUChung.cpp` ) and June `3201f850`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h` ( then `--range <commit>^..<commit>` ); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` ( a Git Bash `/D` argument is path-converted: use `-D` ). Fix clear transcription errors under the same flag ( then `tools/build.py --deploy` ).
Write "CH1i - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1j.md`, commit with explicit pathspecs.
