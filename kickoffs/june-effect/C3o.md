# June effect completion - C3o - June's NPC CommonStateStart tail and state type GAME_START_STATE (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3o` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3j (found there). **Not with** a phase that edits the end of `CX2GUNPC::CommonStateStart` (C3f-1 item 2b, C3j-1) |
| Flag | one new `SERV_IRUHADEV_JUNE_*` per logical change found (name them after what they turn out to be) |
| Log tag | `JUNE_FX C3o` |
| Review | `kickoffs/june-effect/C3o-review.md` if code lands |

## Read first

```sh
grep -n "(C3j)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3j - Done"
```

## Task

C3j mapped June's `CX2GUNPC::CommonStateStart` (`327ac9d0`) tail: March's last block (InsertBuffFactor, June `32761b40 @327af1f7`) is
followed by June-only code. Two pieces are owned elsewhere (the force-down `nowSpeed.y = 0` at `@327af202-327af255` -> C3f-1 item 2b;
`3275a070 @327af3aa` -> C3j). The rest has no owner:

1. `@327af25b-327af360`: a block gated by the now state data's flag `0x2000` (`31e8a990` +8) and a check on the global `0x3372c0fc`, which
   touches the object at `+0xb40 / +0xb44 / +0xb48` of that global (`322d8010`, `31f8b860` with the NPC's `+0x270`). Name the flag (the
   state-data parser `32784c00` sets it), the global and what the block registers.
2. `@327af38e`: `+0x8e39` = state flag `0x8000`. Name the key and every reader of `+0x8e39`.
3. After `@327af3bb` (the `+0x40dc = -1` reset is checklist 7l's): vslot `0x3bc`'s float into `32776e10`, the `+0x244 / +0x248` stores, then
   `CHANGE_WORLD_COLOR_DATA` (`0x33478ee0`) through the function's end (`327af612`). March's `CX2GUNPC` has no CHANGE_WORLD_COLOR_DATA.
4. State type `0x30 GAME_START_STATE` (June's key table `0x334b9178`, entry 48; March's table ends at SUMMON_END_STATE / LINK_JUMP_STATE).
   One June script uses it (`data037 Raid_Waiting`). Find its readers (the `+0x86xx` member InitState fills from type 0x30) and decide with
   the user's rule on June-only systems (port the engine side if March can reach it; a raid-only path with no March raid gets a checklist
   or backlog row instead).

Verdicts into `june2017_re/funcmap.tsv`, tagged `Effect C3o`. Port what March can reach, under its own flag, at June's position in the
tail (after C3f-1's 2b block, around C3j's bounding block as June orders them). Play-test is a regression unless shipped data uses a key.
