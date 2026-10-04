# tools/

Repeatable steps of the offline-client workflow, so no session re-types them. Each script
has `--help`. Run from the Trunk root. Python 3, standard library only.

| Script | Replaces | Skill |
|---|---|---|
| `build.py` | the hand-typed msbuild chain, PCH touching, deploy copy + size/mtime/md5 check | x2-build-deploy |
| `logs.py` | grepping ErrorLog.txt / offline_*.log and guessing whether they are from the new exe | x2-check-logs |
| `flagcheck.py` | the per-review "fresh byte-level unifdef" scripts, `file` + `git diff --stat` hygiene | x2-byte-edit |
| `bytepatch.py` | ad-hoc Python byte patches of CP949 sources | x2-byte-edit |
| `docsection.py` | reading a 750 KB plan doc "in full" | x2-phase |
| `mdscan.py` | (older) reading a CrashRpt minidump against X2.map | - |

Shared state (every worktree deploys into the same game directory):
`%LOCALAPPDATA%\x2build\build.lock` and `last_deploy.json`. Per-tree build logs: `.build/logs/`.
Paths can be overridden with `X2_GAME_DIR` and `X2_BUILD_STATE`.

Validation done when they were written (2026-10-04):
- `build.py`: incremental build of all four projects; deploy of an identical exe; lock wait,
  lock timeout (exit 75) and stale-lock recovery; deploy refused while the game ran.
- `flagcheck.py`: `--selftest`, and reproduces three verdicts earlier reviews recorded by hand
  (TEMPLET_PARITY vs `9133871^` including X2Game.h's 19 EOL-mismatched lines; BEHAVIOR_PARITY
  part 1's 3 blank lines in X2BuffFactor.cpp; OFFLINE_AUTH_LEVEL's one blank line in each
  X2OfflineDB file). Phase 6 (`JUNE_BUFF_NEW_SKILL`) passes.
- `bytepatch.py`: CRLF insert/replace in X2GUUser.cpp and LF insert in the LF-native
  X2UserSkillTree.cpp (scratch copies); hex anchor on a Korean line; ambiguous anchor refused
  with nothing written; high-byte count and encoding class unchanged.
- `.claude/hooks/encoding_guard.py`: blocks Edit/MultiEdit/Write on CP949 files (forward- and
  backslash paths), passes ASCII/UTF-8 files and new files, fails open on bad input; the post
  check reports a lost BOM. It runs once it is registered in `.claude/settings.json`.
