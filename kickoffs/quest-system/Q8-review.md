# June quest system - Phase Q8 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q8 review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q8 done (its Done note and commit), user play-test of quest 22336's clear movie |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_MOVIE` |
| Log tag | `[JUNE_QUEST] MOVIE` (`python tools/logs.py --grep "JUNE_QUEST] MOVIE"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q8 - Scene data and clear movies" "Q8 - Done"
```
Then `june2017_re/funcmap.tsv` rows tagged `(Quest Q8)` and the new code: `CX2Main::AddDungeonClearMovieTemplet_LUA` .. `JuneCheckClearMovieEnd`
(end of `X2Lib/X2Main.cpp`, before `LoadLuaEnum`), `X2State.cpp` cases `SUCM_CLOSE_DUNGEON_CLEAR_MOVIE*`, `X2UIQuestNew.cpp` complete requests.

## Task

Q8 ported June's movie table and quest clear movie from a single Opus reading. Re-read in Ghidra (`disassemble_function`, not the decompile):
- **Parser `31d8cb90`:** the key defaults (`m_dungeonName` -1, `m_eMovieCategory` 0 via `-(found) & value`, `m_iQuestID` 0), `m_eDungeonID` pushed unfiltered,
  and that `31e93c70` is a plain push_back.
- **`31d7a240`:** the stack layout Q8 read (8 dword args, `RET 0x20`: the 0x1c-byte request by value, then the admin bool at `[esp+0x44]` after the prologue),
  the row test (`31c1c1a0` = file name non-empty, then quest == `+0xc0`), `31d79960` first, and that the request is parked only on a match.
- **`31d880b0` / `31d883a0`:** the sound / music order (`+0x808` = option `+0x41` = `m_bSound` via `32420aa0`; `+0x809` = `+0x40` = `m_bMusic` via `3241dba0`),
  the mute before the name test, the dialog arguments (0.07, layer 0), `SetShowUI( 0, 1 )`, vslot 0x1c = Bandi `Play`, `+0x8c` = show; the stop's state
  switch (June 7 / 8 = PvP lobby / room by StateChange's cases `325485f0` / `3253b590`; 14 / 15 with `323a91b0` = `GetJoinNpc`) and its order
  (restore, delete, `31d822b0`, `31d79960`, `31d87560( 0 )`, `32438fa0`).
- **`31d822b0`:** `0xf2` when `+0x842` (then cleared), else `0xf0`; the ACK wait (`0xf1`, `g_pMain +0x144`); the profession-manager call left out.
- **End check** `31d90300 @31d91398-31d91419`: `31c936e0` returns true on status 5 (PLAYEND) or 2 (READY).
- **Skip box** `31f23e70` cases 0xbd8 / 0xbd9: (250, 300), string 0x7797 = 30615, OK 0xbd9, Cancel -1, the box handle at state `+0x1e4`.
- **Complete requests** `323d24f0` / `323d1fc0`: order (31d7a240, then 31d75950 -> 31d78bf0, then start / stop), the admin path's `+0x178` clear, and the
  double send when a start fails (JUNE_BUGS).
- **`31d87560`** was kept as a call with a log line; its two bodies (state vslot 0xb4 = battle-field event 0x3af `324bafc0`, user emotion 0x11 `32044a80`)
  went to backlog Stage 26. Confirm neither runs in the field or village states (field vslot 0xb4 = `3208ddc0`, a no-op).
- **Scope:** the `+0x80a` readers Q8 routed (Stage 26 brief) - re-check the 28-hit census in the Done note.

Hygiene: `python tools/flagcheck.py JUNE_QUEST_MOVIE --range <Q8 commit>^..<Q8 commit>` (peers share these files; `..HEAD` can time out).
Expected: Always.h OTHER (peers' defines), X2Main.cpp / X2UIQuestNew.cpp OTHER only if a peer's hunk landed in the same commit. Explain every row.

Play-test log check: `python tools/logs.py --grep "JUNE_QUEST] MOVIE"` - the request line (table rows, the movie), the start line (video status 3 = playing),
the end or skip, then the stop line naming the quest; `offline_packets.log` shows QUEST_COMPLETE_REQ only after the movie.
