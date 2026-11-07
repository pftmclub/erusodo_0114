# Stage 36 - Process_ExtraDamage's missing June cases: black-out screens and the timed mark list

Moved out of june-march-divergence (was D1-2c; user, 2026-10-29, drift: `Process_ExtraDamage` is tsv `out`, so no `replace` row). New backlog Stage 36, unplanned: turn it into a plan with `/x2-phase new-plan` when it is picked up.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort; plan it first (`/x2-phase new-plan june-extra-damage`) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md`, "Stage 36" (branch `main`) |
| Depends on | nothing; the divergence D1-2 work it follows is committed (both edit `X2GUUser.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_EXTRA_DAMAGE_<NAME>`, one per logical change (black-out screen; mark list) |
| Log tag | none (add one only if a check needs a log line) |
| Review | the new plan's own review |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1 split (2026-10-29, D1-1 session)" "D1-2b Done (2026-10-29, Opus 5.5; three `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)" "D1-2 review Done (2026-10-29, Opus 5.5; reviews `5e4508ee`, `0c054a37`, `25d16ac1`)"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 35: March behaviour inside June code (user, 2026-10-28)"
```

(the second: only its "Found by D1-2b" bullet), and `june2017_re/funcmap.tsv` rows for `32034e50`, `327995c0` (the `(EDT cases)` rows too).

## Task

D1-2b found two June systems that `Process_ExtraDamage` feeds and March lacks; `DyingStart` (D1-2b, `DYING_START`) cleans both up with
stand-in comments. Port them whole, then replace those stand-in comments with June's cleanup.

1. **Black-out screen.** User `Process_ExtraDamage` `32034e50` (ported in Buff 16, tsv `out`) has June cases March's lacks: `EDT_BLIND`
   (31, `@32035ff7`, `BlackOutExtraDamageStart_Blind`) and `EDT_AMUSEMENT_ITEM_DARK_CLOUD` (158, `@320392cd`, `BlackOutExtraDamageStart`).
   Their state: user handles `+0x9188` / `+0x9194`, effect sets `+0x918c` / `+0x9198`, byte `+0x9190` (ctor `3203e850`, `3204b5e0`), frame
   move `31ff4ec0`. `DyingStart` `32014810` ends them (`BlackOutExtraDamageEnd`, `_Blind`, `@32014e10`..`@32014f3a`). Compare the NPC side
   `327995c0` for the same cases. `EDT_BLIND` is reachable offline (an NPC's blind): that is the playable check for the plan-end smoke.
2. **Timed mark list** `CX2GameUnit +0x3b84` (vector of heap records: +4 timer counted down by `32297740`, +8 / +9 end flags, +0x14 effect
   handle), shown by the unit effect set `+0x41ec` while any listed user is alive (`322919d0`), emptied by `32274880` (end of `DyingStart`)
   and by `3229d9a0` (with the `+0x393c` list). Find the writer (an EDT case of `32034e50` / `327995c0`, or a helper they call), name the
   list by what it marks, and port it with its readers. If the writer turns out to belong to an unported character's skill, port the
   container and its game-unit readers, guard the writer's class (`guards.tsv`), and say so.

Per change: June's whole body under its flag, March's in `#else`; one `MODS.md` entry for the sub-phase, a `JUNE_BUGS.md` row per June bug.
Delegate bytepatch specs you write, builds and flagcheck runs to Sonnet subagents. Write the Done note in the new plan, update its STATUS row, commit with explicit pathspecs.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): `EDT_*` ids by name; register June's name and value under a `SERV_IRUHADEV_`
flag if C++ lacks it. No `June` prefix (user, 2026-10-28): members and functions in the class's own pattern.

## Added by char-raven CH4c (2026-11-07)

- **Two June-only extra-damage pack members.** June's list `CX2GameUnit +0x3fb8` has 37 pointers (ctor `322a2700` @`322a5c20`-`322a5dd2`), March's 35:
  the same members 0-34 plus `+0x3aa4` and `+0x3bc0` (pack members 40 / 41, 0x11c each). Name their EDTs (the reader that maps a type to a member),
  add them to `CX2GameUnit::ExtraDamagePack`, the list and `NUM_OF_EXTRA_DAMAGE_PACK_MEMBER`. Check first: the mark list `+0x3b84` above lies
  inside member 40's bytes (`+0x3aa4`-`+0x3bbf`), so it may be that member, not a separate list. funcmap row `322a2700 @322a5c20-322a5dd2 (Raven CH4c)`.

## Lessons from D1-1 / D1-2b (2026-10-29)

- Uncarved functions: `python june2017_re/funcdis.py --func <va>`; Ghidra's decompile of the XOR-keyed Lua reads is unreadable, use the asm.
- The Write tool strips trailing whitespace: an anchor line that ends in a space or tab needs `@@ anchor_hex`.
- An `#error` guard is proven by `cl /P` of a real `.cpp` before the build (CLAUDE.md build rule 3).
- Finish like any phase (x2-phase "Finish"): flagcheck, build, DEPLOY OK, Done note, commit, and a play-test hand-off (an NPC's blind darkens the screen).
