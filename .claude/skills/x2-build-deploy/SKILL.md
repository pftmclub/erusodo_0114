---
name: x2-build-deploy
description: Build the ProjectX2 offline client and deploy it to the game directory with one command (tools/build.py). Use whenever a change to X2/, X2Lib/, KTDXLIB/, X2ServerProtocol/ or KncWX2Server/Common/ needs compiling, deploying, or "the strings are in the deployed exe" proving. Never launches the game.
---

# Build and deploy (ProjectX2 offline client)

Do not type msbuild by hand. Run, from the Trunk root:

```sh
python tools/build.py --deploy        # build X2ServerProtocol, KTDXLIB, X2Lib, X2 (US_SERVICE), then deploy
```

Run it with the Bash tool's `run_in_background: true` (a full X2Lib rebuild is ~4 min; a wait for
another session's build can add more). You are notified when it ends; read the output then.

Other forms:
- `python tools/build.py` - build only.
- `python tools/build.py --deploy-only` - deploy the existing `X2/US_SERVICE/x2.exe`.
- `--only X2Lib,X2` - a subset (dependencies are not rebuilt; use only when you know).
- `--pch force` - invalidate every PCH (normally automatic: a header newer than a project's `.pch` touches that project's PCH source).
- `--deps` - also rebuild luajitLib/luaLib (only after changing them).
- `--config US_INTERNAL` - only when the user explicitly asks.

## Reading the result

- `BUILD FAILED (<project>)` + the error lines; the full log path is printed. Fix and re-run.
- `BUILD OK ... (unchanged: nothing needed relinking)` means your edit did not reach the exe:
  check the flag is defined in `KTDXLIB/Always.h`, then prove it compiled with a `#pragma message`
  inside the guard rather than re-reading `#ifdef`s.
- `DEPLOY OK: X2_offline.exe is N B, dated ..., md5 ..., equal to X2/US_SERVICE/x2.exe (...); built at HEAD abc1234 (+k uncommitted)`
  - paste this line into the phase's Done note. It replaces the hand-written size/mtime/md5 check.
- `DEPLOY REFUSED` (exit 3): the game is running. Ask the user to close it, then `--deploy-only`.
- `DEPLOY FAILED: ... gone` (exit 4): Defender quarantine. Tell the user (Protection History; folder exclusion needs admin).
- `LOCK TIMEOUT` / `COMPILER BUSY` (exit 75): another session is building. Re-run later in the background. Never delete `.build/build.lock` by hand; stale locks (dead pid, >90 min) clear themselves.

## Rules the script cannot enforce

- **Never launch `X2_offline.exe` or `start_offline.bat`.** Ask the user, every time.
- The build includes every uncommitted change in the tree (other sessions share it). The script
  prints the dirty code files; if some are not yours, say so in your report.
- "Strings are in the deployed exe" (a phase's Done gate): after DEPLOY OK, check with
  `python -c "import sys;d=open(r'<game dir>\X2_offline.exe','rb').read();print(b'[JUNE_BUFF] your string' in d)"`
  or the UTF-16 form for wide strings (`'...'.encode('utf-16-le')`).
- Server Lua / data the change needs is packed by the user (CLAUDE.md rule): name the file when reporting done.
