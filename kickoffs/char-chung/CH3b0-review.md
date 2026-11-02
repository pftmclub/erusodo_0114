# June character Chung (Stage 7) - Phase CH3b0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3b0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | Phase CH3b0 done (its Done note and commit) and the user's CH3b0 play-test / "check logs" |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` |
| Log tag | `[JUNE_CHAR]` (`SUMMON`, `CHUNG SKILL remote mortar`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence" "CH3b0 - Done (2026-11-02, Opus 5.5)"
```
Then the CH3b0 rows at the end of `june2017_re/funcmap.tsv` (grep `(CH3b0)`) and `june2017_re/char_chung/ch0_summons.tsv`.

## Task

Review CH3b0's commit against June:
- `322ece70`: the shared `CX2GUUser::SetSummonHelperInfo` keeps Eve's behaviour bit for bit for her flags (0, 0x240, 0x260) and adds 0x1 / 0x2 / 0x4 / 0x8 /
  0x400 right (the NPC's own stat offsets `+0xd14` / `+0xd18` / `+0xd1c` / `+0xd28` / `+0xd2c`, the `- 1.0` before the 0x200 split). Eve's two copies are only
  hidden (`#ifndef _CHUNG_SKILL`), so her `X2Game.cpp` call sites reach the base: confirm with a one-file look, not a rebuild.
- `3230c1a0` cases 0x35f / 0x58b / 0x9dd / 0x44c: the skill ids (`31fcf520`), `m_FlagList[1]` = `+0x8752`, `+0x96e4` = `m_iShot`, `32bdb250` = `m_eArtilleryStrikeType`
  (1 / 2), `+0x98e8`, `32c11940`.
- Mortar list: `32c0a500` (game type `== 1`, Useful Tactic SA 2 truncation), `32c0a420` (bKill), `32c10d20` (front erase at the cap, 50.0, `IsMyUnit`, level =
  now state data `+0x10`), the die-handler hook `3230b7b0 @3230bc08`.
- The play-test log lines the Done note lists, read with `logs.py`.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH3b0 commit>^..<CH3b0 commit>`, plus `git log -S JUNE_CHAR_CHUNG_SKILL <c>..HEAD`
for later commits. Paste its `files:` / `in this flag's files:` / `RESULT:` lines. `junediverge.py --check-all` must print `RESULT: PASS`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "CH3b0 - Review" into the plan doc, update the STATUS.md row (CH2c's dependency), check
`kickoffs/char-chung/CH2c.md`, and commit.
