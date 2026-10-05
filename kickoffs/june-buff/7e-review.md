# June buff port - Phase 7e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7e review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 7e done (its Done note and commits `f20a31c`, `726607a`) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_START_DELAY`, `SERV_IRUHADEV_JUNE_BUFF_RIDING_KEY_WAIT`, and 7e's changes under `SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM` (+ `JUNE_BUFF_DEBUG`) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 7d -- Review" "Phase 7e -- Done"
```

The "Buff Phase 7e" rows of `june2017_re/funcmap.tsv` hold the RE (3230f110, 31fcf040, 3206eaf0,
327f4e30, 322f5140 corrected, 322f11d0, 322f4c60, 324cf360, 3200c030, 3200bc20 corrected,
329031f0, 321f7130, 329cc370, 32cebe80, 32563a8c). `JUNE_BUGS.md` #13 changed to latent.

## Task

Review the two 7e commits against June. Check these first, because 7e corrected earlier records:

1. **The countdown's home.** 7e put both countdowns at the end of `CX2GUUser::OnFrameMove`, after
   the Now pop loop, not in `CommonFrameMove` as the 7d review wrote. Confirm `@32070053` is in
   `3206eaf0` (ret 0xc, fElapsedTime at `[ebp+0x10]`) and that June runs it for every user.
2. **`31fcf040`.** Its riding test is `GetUnit()->GetRidingPetInfo()` (Init's test), not
   `GetRidingOn()` (StageLoad's). Both StateChange calls pass bDirectChange 0. The two tests left
   out (dungeon-data bit 8, `32932230`'s ID ranges) must have no March home.
3. **`322f5140`'s Henir path.** Party `+0x38 == 1` was pinned as `DM_HENIR_PRACTICE` from the
   writer `32563a8c`. Re-check that against March's `X2LocalMapUI.cpp` mapping.
4. **SAT 18 / 34 / 38** in `CheckQuickSlotItemSpecialAbility`: each test in June's order, the arm
   before `322f5140`, no ACK wait on SAT 34's pre-start allow, SAT 38's game-type 4 test.
5. **The riding-key wait reader** sits right after `CheckSkillUsePVPOfficial` in all eight March
   classes (in `CanUseSpecialAttack` for Ara and Elesis). Ara's `CanUseSpecialAttack` has many
   callers (the FP-cancel sites); check whether June's FP-cancel sites reach slot 510 too, and
   whether the 7d `+0x8d78` prefix (top of Ara's `SpecialAttackEventProcess`) then misses them.
6. **Recorded, not ported** (the Done note's list): say whether any of them has a March home
   after all.

Hygiene is one command per flag, not a fresh script:
- `python tools/flagcheck.py JUNE_BUFF_START_DELAY --range f20a31c^..HEAD --with JUNE_BUFF_DEBUG`
- `python tools/flagcheck.py JUNE_BUFF_RIDING_KEY_WAIT --range f20a31c^..HEAD`
- `python tools/flagcheck.py JUNE_BUFF_QUICKSLOT_ITEM --range f20a31c^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_START_DELAY`

Paste their `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 7e -- Review" into the plan doc, update the 7e review row
in STATUS.md, write or check the next phase's kickoff file, and commit.
