# June character Chung (Stage 7) - Phase CH2b0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH2b0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2b0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (the helpers are unreferenced until CH2b) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH2b0 - Done (2026-11-02)"
grep -a "(CH2b0)" june2017_re/funcmap.tsv
```

## Task

Check CH2b0's three helpers against June's listings (read-only Ghidra `disassemble_function`):
- **`31fcf430`**: the `+0x867c` null test returns the argument, else `LEA ECX,[EAX+0x494]` then `JMP 328905b0`; that `CX2GUUser::GetUpgradeSwappedSkillID` does the
  same through `m_UserSkillTree.GetUpgradeSwappedSkillID`.
- **`329cc340`**: vslot `0x524` is `GetNowStateSkillID` (funcmap row 1054), the `31fca8e0( id, 1 )` call and the `+0x178` test, so `IsEquippedSkillUpgraded`
  (`X2GUUser.cpp`, June `32071e70`) on the now-state skill is the same function.
- **`31fc8900`**: the ten offsets against March's `InputData` (`X2GameUnit.h`; June has `Sit` at `+0x4c`, so `Skill5` / `SkillEx5` are `+0x50` / `+0x54` groups),
  the `one*` byte (`+1` in each group, beside `31fc89a0`'s `pure*`), and `false` for an index outside 0-9 (`XOR AL,AL`).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE --range <CH2b0 commit>^..<CH2b0 commit> --files X2Lib/X2GUUser.h X2Lib/X2GUUser.cpp`
(paste `files:` / `in this flag's files:` / `RESULT:`; the one `tok!` is the plain `#endif` closing the `#error` guard, Eve's pattern), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h` (the 2 B rows at `X2GUUser.h:2550-2551` predate CH2b0).
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "CH2b0 - Review" into the plan doc, update the STATUS row (CH2b -> ready),
check `kickoffs/char-chung/CH2b.md` "Amended by CH2b0" names the right symbols, and commit.
