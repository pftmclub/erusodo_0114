# June character Add (Stage 7) - Phase CH0 - Add map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH0` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs now, alongside char-ara and char-elesis) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each later review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-11-11)" "Phases" "Phase CH0 - Add map (read-only)" "Phase CH0c - Add on the creation screen (March classes)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Phase CH-N - Add's June-only class paths: Arc Tracer (33) -> Mastermind (123), Time Tracer (36) -> Diabolic Esper (126)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "Phase CH0 - Elesis map (read-only)"   # plus its CH0 Done note once written
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH0 - Done (2026-11-10)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7" "Phase CH-N - A June-only later class path (template; not run for Aisha)"
cat kickoffs/char-luciel/NU4.md   # June's create-unit functions ("Amended by NU0")
```

## Task

Do "Phase CH0" step 0 and items 1-14 (class `CX2GUAdd`, June range `0x329cf870`-`0x32a316dd`: 733 carved + 58 listing-only overrides, vtable
`334daaec`). Ghidra is read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known addresses and the scripted runs; you keep
the June-vs-March verdicts, the sub-phase assignment, the `inside` / `new` column and the DP, nasod, Crest of Time, summon, CH-N and creation maps.

- **Step 0 first:** the registration map from `charport_states.py` (June's InitState wrapper / body, `SetEquippedSkillFuncBySkillSlotIndex`
  `32a28e00`, listing-only), then `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by hand in the tsv. Copy Elesis's / Ara's
  builders into `char_add/` with Add's range; add `add` to `charport_adis.py` and make sure it reads the 58 listing-only overrides from the raw bytes
  (never create functions in Ghidra). Map every switch in `32a28e00`. Settle the range's end (`32a316dd`, the helpers `32a31740`-`32a317d0`, the
  gauge-data helpers from `32a31900`).
- **Assign every function to the named sub-phases** (CH1a-CH1q2, CH0c, CH2a0 / CH2b0 / CH2a-CH2d, CH3a-CH3d, CH4a-CH4b, CH5, CHNa / CHNb). A group
  over ~45 functions (~25 if mostly listing-only) is split with a digit suffix: write its kickoff and STATUS row. Unmapped helpers (item 6) get
  `CH<n>0` Opus kickoffs and rows.
- **The creation-screen map (item 13) comes first among the maps**: CH0c is the next phase and the user's open point (2026-11-11: "Add right now
  doesn't have any entry"). Name every June function, every `iLimitType` reader, the files to stage, and the exact stat rows (item 12).
- **The asset list** (item 5): routes a / b / c unioned, effect closure, the motion target kom (no March `data207`), the motion question.
- **Enum gate (item 3):** `python tools/luabind.py check Add_NasodRuler` ENUM + STATE: every gap with the phase that fills it.
- **Rebuild the two unfinished planning digests** as `ch0_march_side.md` (with `cl /P` states of `SERV_9TH_NEW_CHARACTER`, `SERV_ADD_LUNATIC_PSYKER`,
  `SERV_LIMIT_UNIT`) and `ch0_june_side.md` (key functions by vslot): the planning numbers in "Planning evidence" are leads until you check them.
- Fill each kickoff's "Amended by CH0" section. Drifted findings go to their owner with a brief (scope rule).
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Add CH0`. List plan-doc inconsistencies you meet.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).
