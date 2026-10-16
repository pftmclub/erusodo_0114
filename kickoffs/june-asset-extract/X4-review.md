# June asset extraction - Phase X4 review

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-asset-extract X4 review` |
| Plan doc | `JUNE_ASSET_EXTRACT_PHASES.md` (branch `main`) |
| Depends on | Phase X4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_KOM_LONG_NAMES` |
| Log tag | `[KOM_LONG]` (`python tools/logs.py --tag KOM_LONG`; the line is written to `ErrorLog.txt` by KLOG) |

## Read first

```sh
python tools/docsection.py JUNE_ASSET_EXTRACT_PHASES.md "Rules" "Phase X4 - SERV_IRUHADEV_KOM_LONG_NAMES" "Phase X4 - Done" "Phase X4 review"
```

## Task

Review X4's commit against `KTDXLIB/KGCMassFileManager.{h,cpp}` (no Ghidra: this is a March-only engine change).

1. **Flag off is the old code, byte for byte.** Every insert is `#ifdef`; the only replaced line (`AddMassFile`'s
   `m_mapMassFile.insert`) is intact in the `#else`.
2. **Names of 59 characters or fewer take the old path.** The side map gets an entry only when `MultiByteToWideChar(..., NULL, 0)`
   returns more than `MASSFILE_NAMESIZE` (60, the count includes the NUL). Check the boundary: 59 chars -> 60 -> old path.
3. **No overrun wherever the 60-char struct is copied.** List every `strFileName` copy in the Done note's design check and confirm
   none of them now receives the long name. The long name lives only in `CMassFile::m_mapLongMemberFileName` and the
   `m_mapMassFile` key.
4. **Index alignment.** The side-map key is `m_vecMemberFileHeader.size()` at the time the `Name` attribute is read, which is before
   that member's `push_back`. `AddMassFile` asks for the same index `i`. Confirm that no path pushes a member without passing the
   `Name` branch first, and that the map is cleared next to `m_vecMemberFileHeader.clear()`.
5. **Upper-casing matches the lookup.** The side map uses `MakeUpperCase( std::wstring& )`, which is the overload `LoadDataFile`
   uses. `KTDXCommonFunc.h:75-90` shows the two overloads are the same `towupper` loop.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_KOM_LONG_NAMES --range <X4 commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a
finding.

**Log check, if the user has run the X4 console check:** `logs.py --tag KOM_LONG` shows
`[KOM_LONG] found TRIGGER_HAMEL_FROZEN_WATER_HALL_STAGE3_STONE_STATUE_WATER_A.X` and `ErrorLog.txt` has no `KEM_ERROR136` for it.
The registration line (`[KOM_LONG] <kom> <name>`) is NOT expected: in this build, nothing logged while the koms are mounted reaches
`ErrorLog.txt` (X4 Done, play-test 1). Also review the lookup log `KomLongNameLogFound`: it sits on the three live hit paths only
(`LoadTwoDataFiles` first and second file, `LoadDataFile`), never in the `MASS_FILE_FIRST` branches.

**One X5 risk to record, not to fix here:** a caller-side fixed-size name buffer could still cut a name before it reaches
`LoadDataFile`. Examples are a particle, mesh or texture name field, or a `WCHAR[64]` in a templet struct. `LoadDataFile(std::string)`
converts into `WCHAR[255]`, so the manager itself is safe. X5's play-test of the long names is where such a buffer would show up.
Name any you find in the review so X5 knows where to look.

Fix clear errors under the same flag (then `tools/build.py --deploy`). Hand design decisions back to the user. Write
"Phase X4 - Review" into the plan doc, update the X4 review row in STATUS.md, check `kickoffs/june-asset-extract/X5.md`, and commit
with explicit pathspecs.
