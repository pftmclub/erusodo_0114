# June quest system - Phase Q0a - Engine and data map

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q0a` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | - |
| Flag | none (read-only) |
| Log tag | `[JUNE_QUEST]` (none written) (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase Q0a - Engine and data map"
```

## Task

Do "Phase Q0a". It is read-only: Ghidra read-only, no code, no flag, no build, no pack. Delegate the reads, greps and mechanical
Ghidra dumps to Sonnet / Haiku subagents, and keep the June-vs-March judgment yourself.

Start from the planning evidence in `june2017_re/quest_plan/`:
- `q_parser_keys.txt`: the keys each June parser reads, in order.
- `q_june_quest_keys.tsv` / `q_june_quest_types.tsv`: the data side.
- `q_xor_quest.tsv`: the decoded strings.

Then March's `X2Lib/X2QuestManager.h/.cpp`: the parsers at `:497-884`, `LoadQuestCondition :2192`, `LoadClearCondition :2345-3086`,
and the `default: return false` at `:3079`.

Settle each of these, with a June address:
- every new key's member and meaning;
- the difficulty-only sub-quest (no `m_eDungeonID`);
- the `QR_*` entries;
- whether `AddGroupChainQuest` is bound (if not: a JUNE_BUGS row, keep June's behaviour);
- the chain-day rule;
- the `_LA` switch;
- the 17 `IsCompleteQuest` callers;
- the `NEED_ONGOING_QUEST_ID` and `MapData REQUIRE_COMPLETE_*` readers.

Write `q0a_engine_map.tsv` and funcmap rows `Quest Q0a`. Amend `Q1.md`, `Q3.md`, `Q4.md` and `Q6.md` under a "## Amended by Q0a"
heading. Done gate as in the section.
