# June character Elsword (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1o review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` |
| Log tag | none (nothing runs until `_LIVE`; the velocity helper has no caller until CH2d) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1o - Done (2026-11-04)"
grep -a "Char Elsword CH1o" june2017_re/funcmap.tsv
grep -a "^GetNearestNPCUnitUID" tools/data/june_march_divergent.tsv
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1o - Review (2026-11-02)"   # Chung's review, the worked example
```

## Task

Check CH1o against June (read-only in Ghidra and the exe bytes). In order of risk:
- **`GetNearestNPCUnitUID` `322d2680` = March (no code):** the gage tests @322d26d4-322d2701 (NULL gage data alive, NULL HP gage dead, `0 < fNow`),
  vslot 0x174 != 4, the distance compare @322d273c, against March `X2Game.cpp` `GetNearestNPCUnitUID` and `CX2GameUnit::GetNowHp()`. If June differs
  after all, the row goes back to open and the replacement lands with the fix.
- **The hook names (CH1h had them swapped):** in `GetFinalDamage` `32284aa0`, EBP = DD +0xc (attacker), EBX = DD +0x470 (defender); vslot 0x508 on EBP
  with `[esp+0x28]` / `[esp+0x2c]` (`fPhysicAttack` / `fMagicAttack`), 0x50c on EBX with `[esp+0x48]` / `[esp+0x44]` (`fPhysicDefence` / `fMagicDefence`);
  each gated by `+0x264 == 0` (`GUT_USER`). Is March's insertion point (after `SERV_TRAPPING_RANGER_TEST`) the same point in June's flow?
- **vslot 0x510:** the call @320478c6 sits after the two `ProcessSocketEffectEx` calls inside the enchant-or-buff test, before `SetSpecificValueByEnchant`;
  the March insertion is in that block and in the live branch (`cl /P` shows it). Elsword's body calls the empty base first (`_LIVE`).
- **`ChangeMainEffectVelocity`:** `31f75ef0` modes (0 add, 2 set, else multiply) and mesh `+0x534` = `m_vVelocity` (funcmap row `mesh+0x54c`, March
  `KTDGXMeshPlayer.h` `GetVelocity` / `SetVelocity`); `31f76580` builds ( x, 0, 0 ); JUNE_BUGS 526.
- **Decisions:** `IsInJobLine` (0x670) and `SetEquippedSkillLevelStateData` (0x7a8) stay plain class members (no `CX2GUUser` base), as Chung's; check that no
  engine code calls either slot.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck` on `JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE` with `--range <CH1o commit>^..<CH1o commit>` and `--files` (the 7 code
files), `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` on the four `.cpp`, one-file probes of the four `.cpp` with
and without `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Write "CH1o - Review" into the plan doc, update the STATUS rows (CH1p becomes ready), check
`kickoffs/char-elsword/CH1p.md` and commit with explicit pathspecs.
