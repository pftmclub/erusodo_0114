# June character Aisha (Stage 5 pilot) - Phase CH3a2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3a2 review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH3a2 done (its Done note and commit) |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (the three skills, the line-map probe), `SERV_IRUHADEV_JUNE_AIMING_TARGET_OWN_LINE` (DoAimingTarget) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH3a2 - Done"
grep -a "(CH3a2)" june2017_re/funcmap.tsv
grep -a "^| 390 " JUNE_BUGS.md
```

The code is the `//{{ ... Phase CH3a2` blocks in `X2Lib/X2GUArme_VioletMage.cpp` / `.h`, `CKTDGLineMap::June_GetVerticalCrossLinePos` in
`KTDXLIB/KTDGLineMap.cpp`, and the one-line `JUNE_AIMING_TARGET_OWN_LINE` site in `CX2GUUser::DoAimingTarget` (`X2Lib/X2GUUser.cpp`).
Ghidra decompiles all of them (`curl .../decompile_function?address=0x...`); `june2017_re/funcann.py <va>` gives the listing.

## Task

Review CH3a2's commit against June:
1. **`31c69af0`** (the hard one): every branch of the segment-segment closest point (non-degenerate, s / t clamping order, the "both out"
   re-projection, the parallel branch's midpoint and end-point picks), the `10000` / `100` thresholds, the prefilter, and the bool's early
   return (`@31c6ac89-31c6ac9f`). Check one branch in the listing, not only the decompile.
2. **Lightning Shower**: the fire block (radius formula, parity, the facing mirror, `+10` y, the two `CanDown` argument lists, that the
   probe runs only when the first test fails and the second from y 99999 succeeds, the land y), the normal FrameMove's missing cut-in /
   world colour, the loop FrameMove's early returns (no CommonFrameMove on `count >= max` or a failed FlushMp), and the five callbacks
   registered as March's (`= March` row in funcmap).
3. **`31ff5440` = `DoAimingTarget`**: confirm the only difference from March is the own-line index (`@31ff5920`), including the NPC skip
   (`+0x85f8` templet class, vslot 0x180) and the HP read (`+0xc04`).
4. **Fire / Dark Ring**: the SA ids (0xc5-0xc7 = `SA_DEGREE_X / Y / Z`, 0x1d = `SA_SCOPE_ABS`), the coefficient (tan, -tan, radians),
   the PlayEffectSet arguments, the registration (Fire Ring sets the hyper flag, Dark Ring does not).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL --range <CH3a2 commit>^..<CH3a2 commit>` and the same for
`SERV_IRUHADEV_JUNE_AIMING_TARGET_OWN_LINE`. Paste the `files:` / `in this flag's files:` / `RESULT:` lines; a FAIL is a finding.

Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH3a2 -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH3b.md` (its Energy Ring note
points at `June_PlayRingOnAimedTarget`), and commit.
