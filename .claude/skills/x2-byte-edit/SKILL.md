---
name: x2-byte-edit
description: Edit CP949/ANSI (Korean-comment) source files byte-safely with tools/bytepatch.py, and prove flag hygiene (insert-only, #else keeps the original, flag-off reduces to the base) with tools/flagcheck.py. Use for any edit to studio files under X2/, X2Lib/, KTDXLIB/, X2ServerProtocol/, KncWX2Server/ that are not UTF-8, when the encoding hook blocks an Edit, and for every phase's or review's hygiene check.
---

# Byte-safe edits and flag hygiene

## Is the file safe for the Edit tool?

`python tools/bytepatch.py info <file>` prints the encoding class. `non-utf8 (CP949/ANSI)` means
**never** use Edit/Write on it (the project hook blocks it anyway). ASCII and UTF-8 files are fine
with Edit.

## Editing a CP949 file

1. Find and see the exact anchor bytes:
   ```sh
   python tools/bytepatch.py find X2Lib/X2GUUser.cpp "void CX2GUUser::DamageReact"
   python tools/bytepatch.py show X2Lib/X2GUUser.cpp 9170:9185    # \t \r \xNN visible
   ```
   Pick whole ASCII lines as anchors. For a line with Korean bytes, use `@@ anchor_hex` with its
   bytes (without the EOL), or anchor on the ASCII line next to it.
2. Write the spec **with the Write tool** into your scratchpad (never a bash heredoc: it eats
   backslashes and once wrote a raw NUL into a .cpp). Block contents are literal, no escaping:
   ```
   @@ file X2Lib/X2GUUser.cpp
   @@ insert_after
   @@ anchor
   		if ( CX2DamageManager::AT_SPECIAL != pDamageData->attackType && IsDoubleAttackByBuff() )
   			m_bDoubleAttack = true;
   @@ text
   #ifdef SERV_IRUHADEV_FOO
   		// June 3203c7de: ...
   		DoFoo();
   #endif SERV_IRUHADEV_FOO
   @@ end
   ```
   Ops: `insert_after`, `insert_before`, `replace` (prefer `#ifdef NEW / #else ORIGINAL / #endif`
   built with `replace` over deleting anything). `occurrence=N` when the anchor repeats.
   Several edits and several `@@ file` sections can share one spec.
3. `python tools/bytepatch.py apply spec.txt --dry-run`, read it, then apply without `--dry-run`.
   All files are planned before any is written; a failure writes nothing.
   Inserted lines take the EOL of their anchor line (CRLF regions stay CRLF, LF-native files such
   as `X2UserSkillTree.cpp`/`X2DamageManager.cpp` stay LF).

## Hygiene check (every phase before commit; every review)

```sh
python tools/flagcheck.py JUNE_BUFF_NEW_SKILL --with JUNE_BUFF_DEBUG            # flag lifetime + uncommitted
python tools/flagcheck.py JUNE_BUFF_NEW_SKILL --range 890abd2..HEAD             # one phase's commits
python tools/flagcheck.py JUNE_BUFF_NEW_SKILL --worktree --files X2Lib/A.cpp X2Lib/A.h   # before committing: only your files
python tools/flagcheck.py OFFLINE_AUTH_LEVEL --base c67ccab^ --at c67ccab       # doc style: reduce to a base
```

Verdicts: `PASS` byte-for-byte; `PASS~` only blank/`//{{ //}}` lines outside the flag differ;
`DEFINE` Always.h comment + `#define`; `OTHER` the change is inside other named flags (an edit to
an earlier phase's flagged code: say which in the Done note); `FAIL` an unflagged change (fix it);
`ERROR` a conditional the tool cannot evaluate (never treated as a pass; resolve by hand).
Once any file in a commit mentions the flag, every other code file that commit changed is
checked too: an edit outside every `SERV_IRUHADEV_` flag there is a FAIL (the forgotten
`#ifdef`). New untracked files count in `--worktree`. A flag with no `#define` fails.
The summary line also counts deleted lines (insert-only rule), non-ASCII inserted lines,
EOL-mismatched inserts, `#else`/`#endif` without the flag token, and encoding changes.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines into the Done note's hygiene row.

The tool reproduces the verdicts the earlier reviews recorded by hand (TEMPLET_PARITY vs
`9133871^` incl. X2Game.h's 19 LF-mismatched lines, BEHAVIOR_PARITY part 1, OFFLINE_AUTH_LEVEL),
and `--selftest` covers the parser. A review that doubts a result cross-checks that one file by
hand; it does not rewrite the tool per review.
