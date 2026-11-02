# June port backlog - Stage 8 planning - the new characters

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan char-rose` (set ready by char-luciel Z; later: `char-ain`) |
| Output | plan key `char-<name>` (`JUNE_CHARACTER_<NAME>_PHASES.md`, `kickoffs/char-<name>/`, a `char-<name>` STATUS block); one character per planning session |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 8); template: `JUNE_CHARACTER_LUCIEL_PHASES.md` (and its "Template for Stage 8", written by char-luciel Z) |
| Depends on | char-luciel Z: the NU (new-character foundation) and DU (two-body engine) flags are class-agnostic and Rose / Ain widen them |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_<PREFIX>_*` (none in this planning session) |
| Log tag | `[JUNE_CHAR] <PREFIX>` |
| Review | none (a planning session) |

Lu/Ciel was planned first (2026-11-02, `char-luciel`).

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Context" "Rules" "Phases" "Phase NU0 - New-unit and two-body engine map (read-only)" "Phase NU - New-character foundation (NU1-NU4)" "Phase Z - Close-out"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 8: new characters (Lu/Ciel, Rose, Ain)" "Standing rules (every stage, every kickoff)" "Open decisions that gate stages"
```
Then `june2017_re/char_luciel/nu0_type_switches.tsv`: its Rose / Ain column records June's case at every per-type switch.

## Order and why

| # | Character | Plan key | Why here |
|---|---|---|---|
| 1 | Lu/Ciel | `char-luciel` | Planned 2026-11-02. He builds the foundation (`UT_END`, the 2-byte class, creation, templet keys) and the two-body engine. |
| 2 | Rose | `char-rose` | `UT_ROSE` 11, base `UC_ROSE_GUNNER` 201, plus 40-43 / 130-133. `ROSE_WEAPON_TYPE`, her weapon UI (`32913f40`, lock `0x5e3`) and her index on the form packets `0x4fd` / `0x4fe` (char-luciel DU5 leaves them). Neil the Sniper aim (`3200d3f0` / `3200d900`). 439 of the GFx event scenes are hers. |
| 3 | Ain | `char-ain` | `UT_AIN` 12, base 202 (**no name in June's `Enum.src.lua`**, so it stays a number with a comment), plus 44-46 / 134-136. The `USEFUL_STRING` readers (15 callers of `32a78fa0`, `ParseUsefulString` `31fea4d0`), SetStopTime overrides (`32a38e70` / `32a38ea0`), Lied Schwach reusing `CX2Game::JuneIsUnitInViewCone`. |

## Task (per planning session)

Plan one character in the Lu/Ciel plan's shape. **NU0 / NU1-NU4 are not repeated.** The character's own rows in each NU / DU site are a
`NU` sub-phase of its plan that widens the existing flags (`#if` over the new unit type), never a copy.
- Starter items: derive the rows from `item.kim` by the pattern `plan_starter_items.tsv` documents. The gear is the `Cobo ... for Beginners` block
  (Rose 113306-113310, Ain 113312-113316). The costume is "<Char>'s Promotion ..." (Rose 42027-42031, Ain 42160-42164), and each has an extra
  promotion hair (42032 / 42165) to list for the user. The result is a labelled stand-in under the character's own flag (backlog open-decisions
  table).
- Stat rows: the same question as Lu/Ciel's (whatever the user answered after char-luciel CH0).
- Re-run `census.py`, `charport_scan.py` and `charport_keys.py <script>`; CH0 builds the class map with `charport_states.py` and
  `ch0c_listing_check.py`; name the CH1-CH4 sub-phases up front; give every unmapped callee an Opus sub-phase; put the transcendence rows in CH3d.

Output: the phase doc, `kickoffs/char-<name>/`, a STATUS block, and this brief's STATUS row updated. No code, no build. Design questions go to the
user at the end, each with a recommendation.
