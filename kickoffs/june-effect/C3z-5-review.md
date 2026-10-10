# June effect completion - C3z-5 review - June's DungeonData fields March's client loader did not read

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-5 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-5 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_DUNGEON_ADEQUATE_MEMBERS`, `_JUNE_DUNGEON_FIX_MAX_DIFFICULTY`, `_JUNE_DUNGEON_SUITABLE_LEVEL`, `_JUNE_HENIR_HARD`, `_JUNE_DUNGEON_CHECK_CONDITION` |
| Log tag | `JUNE_FX C3z-5` (unbracketed: `logs.py --grep "JUNE_FX C3z-5"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-5 - Done (2026-10-11, Opus 5.5 high; 5 flags)"
```

The per-key table is `june2017_re/c3z5_key_readers.tsv`.

## Task

Review C3z-5's commit against June's binary (Ghidra, read-only):

1. **Loader** (`327e0f80`): each new key's default when absent. The table says MinAdeq / MaxAdeq -1, FixMax false,
   SuitableFix 2, RequireCompleteQuest 0, HardRequireItemLevel -1, HardMin / HardMax 0, DCC cleared, then bit N for
   0 <= N < 15. Check them in the disassembly (e.g. `327e2517` OR ESI,-1; `327e4462`; `327e1999`; `327e386b`).
2. **Accessors**: `327da980`, `327da920`, `327da9f0`, `327db250`, `327d9df0` against `X2DungeonManager.cpp`. Watch two things:
   - `327da9f0` resolves the top-difficulty sibling from the row's own difficulty (+0xb0);
   - the accessors use a map find, never March's inserting `operator[]`.
3. **Consumers**, one by one against the June sites in the Done note:
   - UpdatePartyDLG: both FixMax spots, and the Henir item level;
   - UpdateDifficultyButton;
   - UpdateAdequateMemberCount: the red/green rule and the `1` used when there is no party;
   - UpdateAdequateLevel;
   - SelectDungeonDifficulty;
   - the party-list join check;
   - IsActiveDungeon;
   - the local map click;
   - MakeDungeonNameString;
   - the two DCC 11 sites.
4. **Routing**: confirm every C3z-5b item in the table is in `kickoffs/june-effect/C3z-5b.md`.

Hygiene, one command per flag (each with the other four as `--with`):
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_DUNGEON_FIX_MAX_DIFFICULTY --range <C3z-5 commit>^..HEAD --with SERV_IRUHADEV_JUNE_DUNGEON_ADEQUATE_MEMBERS SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL SERV_IRUHADEV_JUNE_HENIR_HARD SERV_IRUHADEV_JUNE_DUNGEON_CHECK_CONDITION`
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "C3z-5 - Review" into the plan doc, update the C3z-5 STATUS.md row, and commit.
