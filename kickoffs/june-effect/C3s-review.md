# June effect completion - C3s review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3s review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3s done (its Done note and commit) and its regression play-test |
| Flag | `SERV_IRUHADEV_JUNE_FORCE_CINEMATIC_STATE`, `SERV_IRUHADEV_JUNE_BF_FOCUS_NEEDS_MY_UNIT` |
| Log tag | none (no new log lines; `python tools/logs.py` for the EXCEPTION / UNHANDLED summary) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3s - Done"
grep -n "(C3s)" june2017_re/funcmap.tsv
```

## Task

Review C3s's commit against June (`june2017_re/funcdis.py`, `statedata_keys.py`, Ghidra read-only):

- **Dungeon `327cea70`**: re-walk the branch graph `@327ceaaf-327cebee`. Check that a non-empty `+0x8f8c` reaches
  `CanNotInputAndDisalbeNPCAI( now == GetStateID )` with `+0x8e38` false, and that it replaces **both** focus conditions (not just the
  start-state one). Check the port's `?:` precedence in `X2DungeonGame.cpp`.
- **Battle field `32d530a0`**: the my-unit guard sits before the base call (`@32d530a3-32d530b4`), and the walk equals March with
  `MODIFY_DUNGEON_STAGING` and `SERV_BATTLEFIELD_MIDDLE_BOSS` on (`+0x8e80` = `m_bIsMainBoss`; the `+0x8c4` bytes 0 / 1 =
  `CinematicUI::m_bStart` / `m_bEnd`: confirm the order in March `X2Game.h`).
- **The parse** `327a0fa0 @327ab5d2-327ab610`: re-decode the key (the first letter comes from an untracked register: confirm `F`),
  the table (INIT_COMPONENT, between BUFF_GROUP_ID and NOT_CHECK_INDICATOR), the miss assigning `""`.
- **Jump tables** `0x327ceda8` / `0x327cedcc` / `0x32d532bc`: re-read the 26 bytes and the state numbering (row `31d6fd60`).
- That `X2PVPGame.cpp`'s override and every other `UpdateUnitPointer` reader are left to C3v, and the boss intro to C3w.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_FORCE_CINEMATIC_STATE --range <C3s commit>^..HEAD` and the same for
`SERV_IRUHADEV_JUNE_BF_FOCUS_NEEDS_MY_UNIT`. Fix clear transcription errors under the same flag; hand design decisions back to the user.
Next: `kickoffs/june-effect/C3v.md` and `C3w.md` exist (written by C3s).

Added by C3x (2026-10-10): C3s's `npc_mesh_census.py --all-npc` closure read mesh bytes raw. 373 of its 812 June models are
MSZIP-compressed (`xof 0303bzip0032`), so the textures they name were never seen. C3x taught the closure to inflate them
(`fxdata.xfile_text`). Re-run `--all-npc` (it rewrites `npc_mesh_need_all.tsv`) and stage what it adds. A miss there shows as a
white or missing texture, not a crash.
