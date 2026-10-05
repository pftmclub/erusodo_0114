---
name: elstools
description: Pack, compile and inspect March .kom archives with ./elstools via tools/kom.py. Use when the user types /elstools pack data036 (or compile / status), or whenever a change to kom_contents/dataNNN_kom/ has to be packed into dataNNN_packed.kom - the user no longer packs by hand, you do.
---

# /elstools - pack March koms

Workdir is the March `kom_contents` (`F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents`).
Archive `dataNNN` = dir `dataNNN_kom/` in, `dataNNN_packed.kom` out. `data036`, `036`, `36`,
`data036_kom`, `datai001` all name an archive. Run from the Trunk root; never retype elstools by hand.

```sh
python tools/kom.py pack data036              # = elstools pack -i <kom_contents>/data036_kom -o <kom_contents>/data036_packed.kom -v mar-2014
python tools/kom.py pack data036 data005      # several in one call
python tools/kom.py compile data036           # stale X.src.lua -> X.lua bytecode (mar-2014), per file
python tools/kom.py status [data036]          # what needs a pack / a copy (no arg: archives touched in the last 2 days)
```

`/elstools pack data036` maps straight onto the first line. Packing does **not** deploy: the game
reads `<game dir>/data036.kom`, so after a pack run `/x2-kom-packed copy data036` (skill) unless the
user only asked for the pack. When *you* need a kom packed for a play-test, run the pack yourself
(no ask needed), then **ask the user before the copy**, every time; once they agree, copy and put
"packed and copied, md5 ..." in the hand-off. Do not ask them to run the script.

## What pack checks (read its output before reporting)

- **REFUSED: unlisted files** - `elstools pack` is directory-driven, not manifest-driven: it packs
  every file in the dir except `*.src.lua` and `_manifest.json`, and a file the manifest does not
  list gets algo 2 whatever its type. Fix the manifest, do not use `--allow-unlisted` unless the user
  says so. Rule (CLAUDE.md memory): `.lua` -> `"2"`; every non-lua -> `"0"`; in data036 every `.txt`
  and `item.kim` is `"2"` (particle/mesh-player). Manifest format byte-for-byte: CRLF, 2-space
  indent, `"name": "v",`, no trailing newline after `}`; edit it with `bytepatch.py` or a byte-level
  script, not a JSON dump.
- **WARN manifest entry has no file** - that entry is not packed; usually a typo against an unlisted file.
- **WARN stale X.src.lua** - the `.src.lua` is newer than its `.lua`. If those edits should ship, run
  `compile` first. Never use `elstools compile-dir`: it overwrites every X.lua that has an X.src.lua,
  including files deliberately converted another way (June `deepcopy` files via `ljconv.py`).
- **vs live kom: +added -removed ~changed** - confirms the change you made is actually in the pack
  (a pack that shows `~0 changed` after you edited something means the edit is not in the dir or the
  manifest). `-removed` entries mean the dir is out of step with the live archive (data101 is);
  `copy` refuses those.
- Verified after the run: entry set = the dir, per-entry algo = manifest, old packed kom replaced
  atomically only when all of it holds. Last line is `PACK OK <name>: <path>`.

Exit codes: 0 ok, 1 failed, 2 usage, 3 game running, 4 refused. Quote the `PACK OK` line.

## Other elstools subcommands (no wrapper)

`inspect` (`elstools/elstools.exe inspect -q -i <kom>` -> JSON name/size/comp/algo, no hash),
`extract` / `extract-dir`, `decompile-batch`. Call them directly with absolute paths, `-q`, stdin
closed (drive from Python with `stdin=subprocess.DEVNULL`; a bash `while read` parent starved it),
and `-v mar-2014` where the command takes a version (config default is jun-2014). June `.lua` source
is read from `june_src_fixed/`, not re-decompiled.

Never launch the game after packing. Overrides: `X2_KOM_CONTENTS`, `X2_GAME_DIR`.
