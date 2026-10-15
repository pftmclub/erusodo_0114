# June dungeons - C3y-1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons C3y-1b review` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (branch: the current working branch); this phase's text and the earlier Done notes still sit in `JUNE2017_EFFECT_COMPLETION_PHASES.md`, which the commands below read; write the new Done note into `JUNE_DUNGEONS_PHASES.md` |
| Depends on | C3y-1b done (its Done note and commit) and its play-test (Henir Space stages 22 / 32-35) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_ANIM_WAIT` |
| Log tag | `JUNE_FX C3x` (`python tools/logs.py --grep "JUNE_FX C3x"`; C3y-1b adds no log line of its own) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3y-1b - port: Henir Space's 5 missing boss worlds + the other Sander secret worlds" "C3y-1b - Done"
grep 327f6990 june2017_re/funcmap.tsv
```

## Task

Review C3y-1b's commit against June:

1. **The binding.** Re-read `327f6990` and `31d28d30` (read-only, `june2017_re/disasm_range.py` or Ghidra): is +0x218 June's
   `m_pXSkinAnim` (compare June's `PlayAnim_LUA` `327f71a0` with March's `IsValidMesh`), and +0x2a4 `m_AnimState`? Is the binder
   entry at `31dbffdd` on `CX2WorldObjectSkinMesh` (the class name use `31dbdce8`)? Does the March member call the same thing in the
   same order, with no extra state (`m_bPendingPlayAnim` untouched)?
2. **The data.** All 13 `WORLD_ID` values in data036 `Enum.lua` equal June's `Enum.src.lua`; the 13 WorldData rows equal June's,
   matched by `worldID`; `world_census.py` still shows the 13 as `same`; `luasrc.py ledger --check` / `core --check` PASS.
3. **The audit.** Spot-check 3 of the 55 receiver:method pairs (one per receiver type) against a `class_def` on the right class with a
   defined guard. Re-run the `SetAnimWait` search over all of `june_src_fixed` (one caller expected).
4. **Assets.** `npc_mesh_census.py --worlds` on the 26 names reports 0 needed (it rewrites `world_asset_need.tsv`: restore it with git
   afterwards). data166 / data182 are new koms in the mounted range.
   **Routed from the C3z-1 review (2026-10-15):** `python june2017_re/dungeon_map_parity.py` lists 3 June-only data180 textures in the
   closure of C3y-1b's meshes, so the Elysion Adrian / Atlas Development rows (and Henir Space's `as_missing 3`) are not 0/0:
   `elysion_adrian_column.dds`, `elysion_adrian_road01.dds` (named by `elysion_adrian_bossstage_g01_air.y`) and
   `field_elysion_atlas_rest_bridge_a.dds` (named by `elysion_atlas_s1_g01_ground.y`). Stage them from `dungeon_map_replace.tsv`
   (`kom.py stage-june`), pack data180, ask before `copy`; the gate is those rows at 0/0.

Hygiene: `python tools/flagcheck.py JUNE_WORLD_ANIM_WAIT --range <C3y-1b commit>^..HEAD`. Paste its `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Read the play-test with `python tools/logs.py --grep "JUNE_FX C3x"` and ErrorLog's `lua error while loading` lines: no line may name
worlds 618 / 645 / 654 / 675 / 710 or their scripts. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`);
hand design decisions back to the user. Write "C3y-1b - Review" into the plan doc, update the STATUS row, check `C3y-1c.md`, commit.
