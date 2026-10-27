# June character Eve (Stage 7) - Phase CH1s review - The third-awakening portrait ball

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1s review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH1s done (its Done note, commit and the diagnostic play-test, plus any fix that play-test produced) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (CH1q's ball); `SERV_IRUHADEV_JUNE_CHAR_EVE_DEBUG` (CH1s's temporary log line, **retired here**) |
| Log tag | `[JUNE_CHAR]` (`EVE NCORE ball`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1s - Done (2026-10-27, diagnostic; play-test pending)"
```
Then checklist **7aq** (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`).

## Task

Review CH1s against June `32e6a210` (the ball), `3208eef0` (user vslot 0xa8, `SetHyperModeCount`) and `322ceaa0` (`g_pData+0x84`). Things this phase could get wrong:
- `+0xf4` as the orb count (one byte written by `3208eef0` through `+0xc04`), and the conclusion that the ball shows only while 3 orbs are held and goes at `HyperModeStart` (June `31ffd490` zeroes the count);
- `+0x84` = the game major system (the Elesis `32b706a0` cross-check), not the UI one;
- the create arguments ( owner 0, 72 / 80 x `+0x1b8` / `+0x1bc`, OverUI `+0x170` ) and the show block ( `GetShow` vslot 0x1c, then `!GetHideDialog` into ShowObject `+0x172` );
- whatever fix the play-test led to, if any: is it June's behaviour, or a March engine gap with its own flag?

**Retire the diagnostic:** remove the `SERV_IRUHADEV_JUNE_CHAR_EVE_DEBUG` define from `Always.h` and its include + block from `X2GageUI.cpp` (`bytepatch.py`), then `tools/build.py --deploy`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_DEBUG --range <CH1s commit>^..<retire commit>` (net zero), `flagcheck JUNE_CHAR_EVE_NCORE --worktree --files X2Lib/X2GageUI.cpp` if a fix landed; `enumscan --check X2Lib/X2GageUI.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the owning flag; hand design decisions back to the user. Write "CH1s - Review" into the plan doc, update the CH1s STATUS.md row (next: the CH1 review), check `kickoffs/char-eve/CH1-review.md` still matches, and commit.
