# June character Elsword (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1n review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, `SERV_IRUHADEV_JUNE_ENUM_NAMES` (six names), `SERV_IRUHADEV_JUNE_USER_MACHINE_GUN_FACING` (new, defined) |
| Log tag | none (nothing runs until `_LIVE`; the machine-gun states need `RIDE_MACHINE_GUN`, which is off) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1n - Done (2026-11-04)"
grep -a "Char Elsword CH1n:" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1n - Review (2026-11-02)"   # Chung's review, the worked example
```

## Task

Check CH1n against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The head of `32d223e0`** against the port in `SetEquippedSkillFuncBySkillSlotIndex` (`_LIVE`): bounds, the four tables `33521204` / `3352132c` /
  `33521318` / `33521340`, the memo exit @32d224e4 (`31fdbcc0`, then `32206450` non-null -> return: is there really no memo case?), the type jump table
  `32d23de0` / `32d23df0` (5 / 6, 7, 0xd-0x10 / 0xc / the rest), and each slot pair written (+0x14..+0x5c = which `UserUnitStateData` member).
- **Buff and throw item:** `ESSI_SI_ES_COMMON_BUFF` for both slots (no `_HYPER` table, unlike March), FM `32a77a20` = vslot 0x5f0, EP `31fcbd60`;
  0xc + 900 -> `ESSI_THROW_ITEM`, vslots 0x5f4 / 0x5f8.
- **Hyper switch:** 21600 / 21603, 21601 / 21604, 21602 / 21605 and their callbacks (thunks `32d262f0` / `32d26300` / `32d26310` / `32d26df0` / `32d26e00`,
  `329cbfc0` -> `31fcc740`); the hyper state gets only the EventProcess; `> 21600` -> `@32d23c1e`. The six `RNSI_HA_*` values against June's `Enum.src.lua`.
- **`SetMegaSlashSkillFunc`:** is `@32d23c1e` really the default of both switches (no explicit Mega Slash case), and does March's id-switch `default:`
  under `_LIVE` call it as Chung's does `SetLunaticBlowSkillFunc`?
- **Base callbacks:** `june2017_re/char_elsword/ch1n_user_callback_triage.tsv` (33 rows, 28 SAME / 5 DIFF): spot-check two SAME rows and the four
  machine-gun DIFF rows against the exe; the `SERV_IRUHADEV_JUNE_USER_MACHINE_GUN_FACING` bodies against June (`31ffb130`, `31ffb7f0`, `31fd2280`,
  `31fd2330`); RandomLose stays the decided stand-in.
- **Always.h:** the three new `_LIVE` guards (`JUNE_SKILL_TYPE`, `JUNE_SKILL_NOTE`, `JUNE_DAMAGE_EXCEPTION`) and the new flag's guard.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck` on `JUNE_CHAR_ELSWORD_LIVE`, `JUNE_CHAR_ELSWORD_CORE`, `JUNE_ENUM_NAMES`, `JUNE_USER_MACHINE_GUN_FACING` with `--range <CH1n commit>^..<CH1n commit>`
and `--files`, `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUUser.cpp`,
a one-file probe of `X2GUElsword_SwordMan.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Write "CH1n - Review" into the plan doc,
update the STATUS rows (CH1o becomes ready), check `kickoffs/char-elsword/CH1o.md` and commit with explicit pathspecs.
