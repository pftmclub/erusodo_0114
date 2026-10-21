# June character Aisha (Stage 5 pilot) - Phase CH4a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4a review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH4a done (its Done note and commit) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`, `SERV_IRUHADEV_JUNE_MP_CHANGE_TEXT` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL passive init`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)" "CH4a - Done (2026-10-22, Opus 5.5)"
grep -a "Char CH4a" june2017_re/funcmap.tsv
grep -a "^| 395 " JUNE_BUGS.md
```

The code is the `//{{ ... Phase CH4a` block at the end of `X2Lib/X2GUArme_VioletMage.cpp`, the `InitPassiveSkillState` / `June_FlushMpWithRefund`
switches in the same file, and the `SERV_IRUHADEV_JUNE_MP_CHANGE_TEXT` blocks in `X2Lib/X2GameUnit.cpp` / `.h`. `june2017_re/funcdis.py --func <va>`
gives listings (most of these functions are not carved in Ghidra).

## Task

Review CH4a's commit against June, in the listing, not only the decompile:
1. **`32cecd70`, block by block:** each skill id, the level call's base flag (`32890620( id, 1 | 0, 0, 1 )`), which templet the ability read uses
   (`32206450` skill tree vs `32890720` user templet, the `ecx` of each `32206e50`), the SA numbers, the init values and the order of the MP-regen
   set / reset calls (`329ce830`, `31fd2c20`, `31fd9e00`, the `0x9d` state test). Check the field-to-member map in the Done note.
2. **The Strong Mind object** (`@32ced132`-`32ced341`, ctor `32cf2190`): field offsets against `JuneMpRefundData` and against the readers
   `32cc2b90`, `32cce4b0`, `32ccf4e0`; JUNE_BUGS 395.
3. **`32278070` and `3229ecc0` `@3229ed1f`-`3229f175`:** the 0.2 s timer, the sum, the two strings (`33488394`, `33478b78`), the pic-chars
   (`g_pData +0x158` / `+0x154`), the `cl` flag and what the HP half does with it.
4. Is anything March's `InitPassiveSkillState` set, and June's does not, still read by live code (`m_pStrongMindData`, `m_bIceBall`,
   `m_fShadowOfDisasterRate`, Sprinter's walk / dash multipliers)?

5. **`SERV_IRUHADEV_JUNE_DEV_UNLIMITED_SKILL`** (dev convenience, not June; standing rule): `JuneIsDevUnlimitedSkillUse()` repeats March's gate in
   `SpecialAttackNoStageChange` (`X2GUArme_VioletMage.cpp` ~5192) exactly, and the June lookup skips the whole block March skips.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE --range <CH4a commit>^..<CH4a commit>` and the same for
`SERV_IRUHADEV_JUNE_MP_CHANGE_TEXT`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. A FAIL is a finding.

`SERV_IRUHADEV_MP_REGEN_BOOST` was commented out for CH4a's play-test (user, 2026-10-22): ask the user whether to re-define it now or keep it off
for the CH4b-CH4d play-tests.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH4a -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH4b.md`, and commit.
