# June effect completion - C3z-5b - the DungeonData readers C3z-5 routed

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-5b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-5 (its loader, accessors and `SERV_IRUHADEV_JUNE_DUNGEON_CHECK_CONDITION`); best before or with C3y, which makes the June-only rows live |
| Flag | one `SERV_IRUHADEV_JUNE_*` per family (e.g. `..._DCC_RIDING_PET`, `..._DCC_EVENT_DUNGEON`, `..._DUNGEON_ADDITIONAL_TICKET`, `..._LAST_CLEAR_DUNGEON`) |
| Log tag | `JUNE_FX C3z-5b` |
| Review | write `kickoffs/june-effect/C3z-5b-review.md` when done |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-5 - Done (2026-10-11, Opus 5.5 high; 5 flags)"
```

The work list is `june2017_re/c3z5_key_readers.tsv`: every row with verdict ROUTED. Each row names June's call
sites. DUNGEON_CHECK_CONDITION is read only through g_pData `31edf8f0(id, dcc)` -> `327d9df0`. Its 23 callers are listed
with the bit each pushes.

## Task

Port each family with its consumers, one flag each, the original code in `#else`, citing June addresses. Where June's
consumer needs a system March lacks, write June's no-system answer instead.

1. **DCC 8 NOT_SUMMON_RIDING_PET.** It is live today: Henir's row sets it. Its sites are:
   - `31fcf040`: March `CX2GUUser::StartMotionStateChange`; see the note at `X2GUUser.cpp:29341`;
   - `321f9480`, `321f99e0`, `321fa590`: the riding-pet summon request, the stamina frame-move and the riding-usable check;
   - `3204ea20`, in GUUser.

   Map each site to its `X2RidingPetManager.cpp` / `X2GUUser.cpp` counterpart first.
2. **DCC 2 EVENT_DUNGEON.** Seven June callers: `323de34c`, `322c35f0`, `32931d00`, `32932456`, `322f3a48`, `322cea85`,
   `322fbf42`. March's `CX2Dungeon::IsEventDungeon` / `CX2DungeonManager::IsEventDungeon` are hard-coded lists equal to the
   13 shared DCC_EVENT rows. Match June's callers against March's IsEventDungeon callers. Then either move the lists onto
   the DCC lookup, or port the callers that differ.
3. **The DCC bits only June-only rows set:**
   - 1 SINGLE_DUNGEON: `32437b4c`, `322b64d0`, `322c6812`; also `X2PartyUI.cpp:313`;
   - 9 NOT_USE_PET_AUTO_USE_ITEM: `3200c030`;
   - 10 NOT_SHOW_DUNGEON_MAP_UI: `324ce110`;
   - 12 NOT_USE_RESURRECTION_STONE: `322ff710`; first confirm which bit it passes;
   - 13 NOT_USE_RELATIONSHIP_SKILL: `3202d3c0`;
   - 14 NOT_RETRY_DUNGEON: `324d1120`;
   - `322c4708`: identify the bit.
4. **June's `CX2Unit::GetLastClearDungeonID`** (`32350560`) and the UpdatePartyDLG inactive-dungeon fallback that uses it
   (`322c30c0`-`322c3150`, today Ruben normal, `X2PartyUI.cpp` "June asks the unit for a replacement"). It reads
   m_SuitableLevelFixValue. Its helpers are:
   - `322cac30`;
   - `327d97b0` (IsDefenceDungeon);
   - `327d9d50` (secret open: March answers true, `X2LocalMapUI.cpp:1420`);
   - `327da3e0` (mode by type);
   - `324f25f0` / `324f2360` (local-map templet).
   March's other callers of GetLastClearDungeonID are `X2PartyManager.cpp:534` / `:639`, `X2StateBeginning.cpp:2336` and
   `X2StateServerSelect.cpp:4372` / `:4762`.
5. **`m_fExtraValue`** (Henir, 0.65). It is a protected float at +0x9c. In June's NPC stat fetch `32384d10` @3238500f it
   scales a stat in Henir practice mode (party mode 1). Identify the stat, then port it into March's NPC stat path, or
   record the no-system answer.
6. **The June-only-row keys:** `additionalTicket` (+0x70, a vector of {id, count}; 194 June rows), `m_bShowNpcKillCount`
   (+0xbc), `m_bAprilFoolsDungeon` (+0xbd; no June row sets it), `m_bNoDarkBackGroundBy60S` (+0xbe).
   - Load each one with June's default (`327e0f80`; the sites are in the table).
   - Find its readers by dataflow from `327da150` and from CX2Dungeon's m_pDungeonData, then port them.
   - A reader that needs a system March lacks gets its no-system answer.

Per-class reads go to the character ports (checklist 7s). Play-test with the character and job named from `els_db.sql`.
Henir needs level 80, e.g. reyaa (Lire Grand Archer, unit 12) with a riding pet.
