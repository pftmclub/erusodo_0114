---
name: x2-kom-packed
description: Copy a packed kom (kom_contents/dataNNN_packed.kom) into the March game dir as its real name dataNNN.kom, with backup and verification, via tools/kom.py copy. Use when the user types /x2-kom-packed copy data036, or right after /elstools pack when the new archive has to reach the game.
---

# /x2-kom-packed copy - deploy a packed kom

**Ask the user before every copy, every time** (user, 2026-10-06). It overwrites a live game archive,
so a pack finishing is not permission, and neither is an earlier approval for a different kom or
phase. Say which kom(s) and what the pack changed (the `vs live` diff from `pack`), then wait for a
yes. Typing `/x2-kom-packed copy data036` is itself the yes for that archive in that message. Packing
needs no such ask.

```sh
python tools/kom.py copy data036               # kom_contents/data036_packed.kom -> <game dir>/data036.kom
python tools/kom.py copy data036 data005       # several
python tools/kom.py status data036             # in sync? and what `copy` would change
```

Game dir: `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`. `/x2-kom-packed copy data036`
maps straight onto the first line (`036`, `36`, `datai001` work too). Pack first (`/elstools pack`);
when you need a kom deployed for a play-test, run pack yourself, then ask before the copy, and report both lines.

What it does, in order: refuses if `X2_offline.exe` is running (exit 3: ask the user to close the
game, then rerun; never kill it) -> refuses if the kom dir has a file newer than the packed kom
(exit 4: pack again; `--force` only if told) -> prints the entry diff against the live kom -> refuses
if the packed kom **drops** entries the live kom has (exit 4; the dir is out of step, `--allow-removed`
only if the user wants them gone) -> backs the live kom up to
`<kom_contents>/../kom_backup_tools/dataNNN.kom.<stamp>` (newest 5 kept) -> copies to `dataNNN.kom.part`
and atomically replaces -> verifies size, md5 and entry count.

Report the `COPY OK ...` line verbatim (it carries size, entries and md5) and the backup path. If
live and packed are already identical it says so and copies nothing. A `COPY FAILED` with a sharing
violation means the game (or Defender) holds the file; say so, do not retry in a loop.

Restore: copy the backup over the live name with the game closed. The new file is a fresh inode, so a
hard link the old `dataNNN.kom` shared with another directory is detached, not rewritten.

Never launch the game afterwards (ask). The play-test hand-off should name the kom and what to look
for, per x2-phase.
