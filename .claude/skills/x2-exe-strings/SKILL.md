---
name: x2-exe-strings
description: Prove strings are (or are not) inside the built or deployed exe with tools/exestr.py. Use for every "strings are in the deployed exe" Done gate, after build.py --deploy prints DEPLOY OK, when a log tag or UI string must be shown compiled in, or when a flag-off build must NOT contain a debug tag. Replaces hand-written python one-liners that read the exe.
---

# Exe string check (ProjectX2)

Never write a throwaway script to look for a string in the exe. Run, from the Trunk root:

```sh
python tools/exestr.py "[JUNE_FX] cast" "Code Recovery"     # deployed X2_offline.exe, narrow (cp949/ASCII) and UTF-16LE
python tools/exestr.py --file strings.txt                   # one string per line, '#' comments
python tools/exestr.py --form wide "Electronic Field"       # only the UTF-16LE form counts
python tools/exestr.py --absent "[CH4C_DEBUG]"              # flag-off proof: passes only if NOT present
python tools/exestr.py --exe build "[JUNE_FX]"              # X2/US_SERVICE/x2.exe (before deploy)
python tools/exestr.py -q ...                               # failures and the summary only
```

- Default form `any`: narrow or wide counts. Use `--form wide` for `L"..."` literals when a narrow hit would be a false pass
  (log format strings in `CX2OfflineLog::Server( L"..." )` are wide).
- Exit 0 = every string passed, 1 = a miss, 2 = exe not found.
- Output ends with `EXESTR OK: n of n found in X2_offline.exe, <size> B, dated <mtime>, md5 <md5>`. Paste that line next to the
  `DEPLOY OK` line in the Done note; compare the md5 with `DEPLOY OK`'s.
- Search the literal as it sits in source, without printf arguments (`"[CH4C] recovery"`, not the formatted line).
- A miss after DEPLOY OK means the code did not compile in: check the flag in `KTDXLIB/Always.h`, then prove it with a
  `#pragma message` inside the guard (x2-build-deploy); do not re-read `#ifdef`s.
- Read-only. It never launches the game.
