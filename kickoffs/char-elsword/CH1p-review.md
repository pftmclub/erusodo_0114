# June character Elsword (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1p review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1p - Done (2026-11-04)"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1p - Review (2026-11-02)"   # Chung's review, the worked example
```

## Task

Check CH1p against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The shared reader** `31fcadd0`: A 0-3 + F 4, T 9 (key 1 usable), B 5-8 only when the slot is expanded; `UT_ELSWORD` now gets June's numbering under
  `_LIVE`. Is anything else in `X2GUUser.cpp` that Elsword's `_LIVE` code reaches still class-gated to March's 4 + 4 (grep for other `UT_CHUNG` / `UT_EVE`
  lists next to slot or key code)? Is the new `Always.h` guard (`_LIVE` needs `_CHUNG_LIVE`) right, or should the line stand alone?
- **The class side**: in the `_LIVE` `cl /P` of `X2GUElsword_SwordMan.cpp`, every reader of the pressed index, `m_iChosenSkillSlotIndex` and
  `m_iNowSpecialAttack` is 0-9 / slot + 1, as June's `32cf3290` / `32d1bd10` / `32cf5760`; `CountUsedSkill` is in `#else` only.
- **The `+0x8b58` sweep**: re-run `june2017_re/dispsweep.py 8b58` and check that the 10 class functions are all and only the held-key users in
  `0x32cf2da0-0x32d2763f`, and that each owning kickoff (CH2a, CH2c, CH3a, CH3b) names them.
- **Hooks**: 7h / 7l calls and the 7o / 7p readers are in the `_LIVE` preprocess as the note says; the `UPDOWN_CANCEL_AFTER` site count (18) matches
  `ch0_checklist_rows.tsv` (CH1d2 1, CH1e 10, CH1f 7).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck` on `JUNE_CHAR_ELSWORD_LIVE` with `--range <CH1p commit>^..<CH1p commit> --files X2Lib/X2GUUser.cpp KTDXLIB/Always.h`,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUUser.cpp`, one-file probes of `X2GUUser.cpp` and
`X2GUElsword_SwordMan.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Write "CH1p - Review" into the plan doc, update the STATUS rows
(CH1q becomes ready), check `kickoffs/char-elsword/CH1q.md` and commit with explicit pathspecs.
