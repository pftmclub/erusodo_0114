# June effect completion - C3r review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3r review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3r done (its Done note and commit) and its regression play-test |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW` (C3o's), `SERV_IRUHADEV_JUNE_CUTIN_MY_UNIT_GUARD` |
| Log tag | none (no log lines; the play-test is visual) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done" "C3r - Done"
grep -n "(C3r)" june2017_re/funcmap.tsv
```

## Task

Review C3r's commit against June (Ghidra read-only, `june2017_re/funcdis.py`, `xordec2.py`). The user side differs from the NPC's in four
ways; check each:

- **The parse** `32057b80 @32060286-32060497`: re-decode the keys around it (`xordec2.py` on the function: `CHANGE_WORLD_COLOR` @3205ff7d
  -> `+0x927c`, `SHOW_SKILL_CUT_IN_TIME` @32060119 -> `+0x9278`, then the table, then `SHOW_HYPER_CUT_IN_TIME` -> `+0x9280`). Confirm the
  `@32060290` store is `+0x9278`'s miss default (`ebx = edi + 0x9278` @32060255) and that the port sits right after March's
  `SHOW_SKILL_CUT_IN_TIME` line in `CX2GUUser::CommonStateStart` (the Now parse, not a Future one).
- **The gate** `31fd3260`: `+0x927c == 1` first, then `0 <= START` (`test ah, 0x41; jp`), then START <= t <= END; t from `32071d80`
  (`+0x8e90` ? `+0x8e88` : `+0x87d4` = `m_pXSkinAnim`, not the Future anim `+0x87d0`).
- **The caller** `32063650 @3206cdb7-3206cdcb`: true -> `32001460`, false -> `32296be0`; and that `32001460` = March's
  `ChangeWorldColorByHyperMode` (`32001380` gate, `+0xc04 / +0xe8` = remaining hyper time, constants `334387b0`, `334350e8`, `33474d80`).
- **`32001380`** = `GetShowCutInAndChangeWorldColor`: the jump table `3200144c` (types 1-2 true, 3-5 the battle-field test) and the
  `GetMyUnit()` NULL test `@320013f5`, ported as `SERV_IRUHADEV_JUNE_CUTIN_MY_UNIT_GUARD`.
- **The NPC state-end restore** (C3o missed it): `3277ee10` ends in `jmp 0x32296be0` @3277fbe0 on every path; the port is the last statement
  of March's `CX2GUNPC::CommonStateEnd`. Re-run the raw rel32 scan for `32296be0` (3 sites) and `31fd3260` (1 site).
- **The withdrawn `3204f190 @3204fb99` reader**: EBX there is the socket templet (it reads `+0x27c` a few lines on), so that block is the
  summon-NPC socket, not world colour (routed to C3u). Confirm.
- **JUNE_BUGS 156** is withdrawn: check the user `CommonStateEnd` fade (`32015250 @3201572f-3201574f`, March `X2GUUser.cpp` after
  `TRANSITION`).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW --range <C3r commit>^..HEAD --with SERV_IRUHADEV_JUNE_CUTIN_MY_UNIT_GUARD`.
Fix clear transcription errors under the same flag; hand design decisions back to the user.
