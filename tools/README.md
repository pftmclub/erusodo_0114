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
| `createchar.py` | hand-written INSERTs adding a test character to els_db.sql (backup, June exp / free skills, worn promotion costume, +10 top-tier gear in the bag, slot/name guards) | x2-create-char |
| `elsdb.py` | throwaway sqlite scripts against els_db.sql: `q` (read-only SQL), `x` (mutating SQL: guards, backup, one transaction), `chars`, `inv`, `addchar`, `setchar`, `additem` (kim-driven bag/stack/equip placement), `delitem`, `setting`, `backup(s)`; every write refuses while the game runs, backs up first, rolls back on error / `--dry-run` | x2-db |
| `kom.py` | hand-run `elstools pack` / `compile-dir` and copying `dataNNN_packed.kom` to the game dir (manifest/algo checks, the Lua pack gate (BOM text that parses / March bytecode / ledger exception, else refused), `compile` = fold `X.src.lua` into BOM-source `X.lua`, entry diff vs live, backup, md5 verify); `stage-june <name\|list-file> [--dry-run]` copies June's exact file (junekom decode) into the right `kom_contents/dataNNN_kom/` and its `_manifest.json` (refuses long names, multi-kom names, undecodable files) | elstools, x2-kom-packed |
| `luasrc.py` | hand-made `luajit -b` / decompile guesses when a March `.lua` had to change: `find` / `convert` (T0 proof: source compiles byte-identical to the shipped bytecode, else an ljd decompile proven T1 by `tools/ljbc.py`; snapshot + ledger) / `decompile` / `check` (T1) / `t2` (report only) / `selftest` / `calibrate` / `restore` / `refresh` / `ledger --check` over `kom_contents`, sources from sibling `X.src.lua` or the studio `ClientScript\` / `_ClientScript\`; also imported by `kom.py` for the pack gate and compile fold; overrides `X2_KOM_BYTECODE`, `X2_LUA_LEDGER`, `X2_STUDIO_SCRIPT_DIRS` | x2-lua |
| `adoptjune.py` (via `luasrc.py adopt-june / core / iter-audit`) | hand-run `junekom.py extract` + `ljconv.py` + copying into `kom_contents` for June `.lua`: `adopt-june` (default June bytecode via ljconv, staged with `kom.py stage-june` placement; `--source` for core-list files = decompile + T1 -> BOM source + ledger row), the core list `tools/data/lua_core_list.tsv` (`core --check / --add / --set / --classify`), `iter-audit` (pairs/next/foreach tables a text load may reorder); scratch targets via `X2_KOM_CONTENTS`, `X2_LUA_LEDGER`, `X2_LUA_CORE_LIST` | x2-adopt-june |
| `kim.py` | hand-written byte probes of `item.kim`: `info` / `dump` / `find` (name, `--where FIELD<op>VALUE` filters with enum names, `--cols`/`--sort`/`--count`/`--tsv`/`--json`, capped at 50 rows) / `set` / `census` / `assets` / `diff` / `kims`; kim is a path or alias (`staged` default, `june`, `march`, `march_patched`); March (188 B) and June (228 B) records told apart by stride (map: `june_item_kim/k0_record_map.tsv`) | x2-kim |
| `junekom.py` | elstools on June 2017 koms, which cuts 1-7 bytes off V.0.4 Blowfish entries: `list` / `extract` / `verify` / `selftest`, June's own decode for V.0.3 + V.0.4 algo 0/2/3 (strict inflate, size == manifest), header checks, `verify --compare` against an extract tree (read-only on the `.kom`s; June's `*_kom` tree only with `--overwrite --backup`) | - |
| `luabind.py` | finding out at play-test that a June script calls a method March never bound (the function dies at that line, one deduped ErrorLog line): `check NAME [NAME ...]` / `check --dir june_src_fixed/data037_kom` (census), exit 1 on an unbound name not in `tools/data/lua_unbound_known.tsv`; `logs.py` also prints the run's "can't find" names | x2-phase, x2-adopt-june |
| `skillicons.py` | finding out at play-test that a skill shows an empty icon: `check 25000-25999 [more ranges]` / `check all` reads June's `NewSkillTempletVer2.src.lua` (icon + skill-tree image) against `kom_contents` (texture present, piece in its `.TET`), exit 1 on a miss, and prints the `kom.py stage-june` line for the missing sheets | x2-phase (character ports) |
| `mdscan.py` | (older) reading a CrashRpt minidump against X2.map | - |
| `enumscan.py` | grepping June code for raw ids that are enum values: `python tools/enumscan.py` (writes `june2017_re/enum_literals.tsv`: June-code literals of the survey files, each with family, class A = C++ member / B = in June or live Enum.lua only / C = in no Enum / D = lead / X = no-enum family such as STR_ID, and the candidate name), `--check FILE...` (exit 1 while an A or B row remains; the `#else` of `SERV_IRUHADEV_JUNE_ENUM_NAMES` is skipped), `--verify [N]` (compile-probes N parsed C++ enumerators, `enumprobe.py`'s trick), `--summary`, `--reuse` (cached `cl /P`), `--small`, `--dropped`; the hand-written `verdict` / `evidence` columns (N1) survive a rewrite; plan `JUNE_ENUM_NAMES_PHASES.md` | - |

Shared state (every worktree deploys into the same game directory):
`%LOCALAPPDATA%\x2build\build.lock` and `last_deploy.json`. Per-tree build logs: `.build/logs/`.
Paths can be overridden with `X2_GAME_DIR` and `X2_BUILD_STATE`.

Validation done when they were written (2026-10-04):
- `build.py`: incremental build of all four projects; deploy of an identical exe; lock wait,
  lock timeout (exit 75) and stale-lock recovery; deploy refused while the game ran.
  2026-10-18: build coalescing (`last_build.json`, 13 cases incl. failed / `--only` / other
  config / older exe) and X2 with `BuildProjectReferences=false` after both libs built in the
  same run (the X2 step had been rebuilding both libs, ~10 min); STALE LINK guard.
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
