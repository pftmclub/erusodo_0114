# June effect completion - C9h-2d review - the pet-preview loader, lookup, code-event sets and PetPreView.lua

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2d review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2d done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP` (also `#define`d at the end of `ServerDefine.h`; in `SECOND_HOMES`) |
| Log tag | `JUNE_FX PET_PREVIEW_LOOKUP` (`python tools/logs.py --grep "JUNE_FX PET_PREVIEW_LOOKUP"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2d - Done" "C9h-2c - Done"
grep -n "Effect C9h-2c" june2017_re/funcmap.tsv
```

## Task

Review the C9h-2d code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **Lookup vs `32808770`.** Decompile it again and walk the branches against `CX2ItemManager::GetPreviewIDByItemID` in `X2ItemManager.cpp`:
  - the type test (`[2]`) and the item test (`[0]`);
  - `code == 0`, second id `< 1`: either date empty -> push; else parse both, `start < now` and `now < end`, a parse failure pushes nothing;
  - `code == 0`, second id `>= 1`: no empty test; in window -> swap (only when the second id is not already there: erase the pet, push the second), else the pet when neither id is there;
  - `code != 0`: instance null or the `+0xa94` set misses -> second `< 1` skip, else the fallback; set hits -> second `< 1` push, else the swap;
  - the tail `rand() % size`, `-1` when empty. The decompile drops the tail, so check `@32808bda` in the listing.
- **Loader vs `32812ce0`**: both strings -> dates, only the first -> `atoi`, `<= 0` skip. Check what `31c1c240` (the "empty" test) is, so the empty-date reading is right.
- **`31c55270`**: shift = `+0x470` - `_timezone`, added to the CTime; a failed parse does not shift. `GetServerUtcOffset()` = `+0x470` sign-extended.
- **Packet**: Put/Get order map, set `+0xa94`, set `+0xaa4` (`320de0f0`); the handler's copy (`31f00970`). The handler's tail (`32344880` + vtable `+0x100`) is **not** ported, it is C9h-2e's: check the Done note says so and STATUS has the row.
- **Data**: `kom.py status data036` shows only `PetPreView.lua` changed; the 6-argument binding reads a missing argument as 0 / NULL (`lua_tinker.cpp:313`).

Hygiene is one command: `python tools/flagcheck.py JUNE_PET_PREVIEW_LOOKUP --range <first C9h-2d commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "C9h-2d - Review" into the plan doc, update the phase's STATUS.md row, and commit.
