# June buff port - Phase 13 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 13 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 13 done (its Done note and commits); the plan requires this review **before** the user copies `data036_packed.kom` into the game (the packed kom exists, the copy is the user's go) |
| Flags | `SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF` (turned on by 13) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 13 — Adopt June buff data and ship" "Phase 13 — Done"
```

## Task

Review the staged set, not the C++ (Phase 13 changed one line of C++: the `_STORE_BUFF` define).

1. **Staged files equal the source.** `march2014/update/data036/` and `kom_contents/data036_kom/` hold
   the same four compiled files (`BattleFieldBuff{Templet,Factor}{,_INT}.lua`, md5 equal), compiled from
   `june_src_fixed/data036_kom/*.src.lua` unchanged (md5 of the `.src.lua` beside them). Both manifests:
   the new keys are `"2"`, CRLF, 2-space indent, no trailing newline, and the JSON parses.
   Decompile the staged `.lua` with `elstools decompile-batch -v mar-2014` into scratch and confirm the
   entry counts: 929 + 72 templets, 1130 + 69 factors.
2. **The packed kom.** `elstools inspect` on `kom_contents/data036_packed.kom`: 188 entries, the 4 buff
   files + `Enum.lua` + `item.kim` are the only differences from the live `data036.kom` (+2 added, ~4 changed).
3. **Enum.** The 4 members 13 added (`SKILL_SLOT_TYPE` table, `UT_LUCIEL = 10`, `ST_SPECIAL_ACTIVE_GRADE_1/2/3 =
   14/15/16`) equal June's `Enum.src.lua` (:17838, :312, :7133/:7140/:7145). Re-run the scratch pass: the real
   LuaJIT driver (`elstools/vendor/luajit/luajit.exe`, stub manager, `__index` logger on `_G`) loads all four
   files with 0 runtime errors, 0 duplicates, and only `STR_ID_*` unknowns (defined by `StringID_def.lua`).
   Then re-run the static member pass: the only `Table.Member` left unresolved is
   `RENDER_TYPE.SMRT_CARTOON_BLACK_EDGE_ALPHA` (June has the same nil; JUNE_BUGS row).
4. **The 8 census exclusions** added to `june2017_re/buff_census.py` `NOT_READ_BY_JUNE_TYPE` rest on the
   plan's RE notes (not a fresh look). Spot-check 3 in Ghidra, reading the assembly: `32d4b020` (100: does
   it read any `BUFF_RELATION_TYPE` under `SKILL_CATEGORY_n`?), `32e75360` (75 templet) and `32e755a0` (79 templet).
   Any key June does read is a real census gap: report it, do not ship.
5. **`item.kim`.** Two WORDs changed (offsets 20107070, 20107414: 362 -> 481, 363 -> 482), found by
   the id+132 pointer. Check against the live `item.kim`: no other byte differs, and the live items 85003830 /
   85003831 still point at those offsets.
6. **Kept on purpose.** The `BFI_DEBUFF_TELEPORT_CONSUME_MP` alias stays because `NewSkillTempletVer2.lua`
   (live kom, bytecode) still names it; confirm with a string search of the live kom's copy and that no
   June-only name collides at 290.
7. **Expected load summary** (written before the run): `templets 1001 loaded, 0 failed, 0 duplicate; factors 1130 loaded +
   0 placeholder(s); _INT factors 69 loaded + 0 placeholder(s); ordinal rejects 0 (+ 0 _INT)`. If the user has
   copied and played, compare the log to it (the Done note says which lines to read).

Hygiene: `python tools/flagcheck.py JUNE_BUFF_STORE_BUFF --range <13's code commit>^..HEAD`; paste its
`files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors (restage, repack, redeploy);
hand design decisions back to the user. Write "Phase 13 — Review" into the plan doc, update the STATUS.md row,
and commit. After the review passes, the next step is Phase 14 (`kickoffs/june-buff/14.md`).
