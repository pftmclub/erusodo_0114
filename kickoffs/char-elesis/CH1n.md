# June character Elesis (Stage 7) - Phase CH1n - Base callbacks and the skill-slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1n` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ELESIS CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-elesis/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1n - Done" "CH1n - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1n"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

The `CX2GUUser` base callbacks Elesis registers (start from Ara's CH1n triage: only rows earlier characters did not cover);
`SetEquippedSkillFuncBySkillSlotIndex` `32b8c470` head (bounds, slot tables, the 7i rule, the pre-switches of its four jump tables; June drops the
running-skill snapshot call and sets `+0x11`: `plan_checklist_rows.md` item 14; the slot-state byte stores of checklist l.444 are dropped: March
answers through `IsJuneSkillSlotState`) and the dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`). She has no second skill switch (vslot
0x68c is the base's): nothing to write for `InitAquiredActiveSkillState`. Probe with `_LIVE`; no full build.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).

## Amended by CH0 (2026-11-12)

- **Functions:** `awk -F'\t' '$7=="CH1n"' june2017_re/char_elesis/ch0_class_map.tsv` = 1 (0 listing-only: col 9 `n`; 1 `inside`, 0 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L447.
- `june2017_re/char_elesis/ch0_skill_switches.tsv` (287 rows): (a) 31 entries from 28031 (28031 / 28060 Juggernaut Buster, 28032 / 28061 Blazing Dance), (b) skill type 5..16, (c) 28000..28236 (237 entries, 55 targets, 96 ids register), (d) 28601..28605; compares 28600 and 900. `ch0_skill_listing_rows.tsv` 167 rows; 12 states get their hyper callbacks copied by `329cc2c0` (Ghidra shows none: the listing is right). No second skill switch (0x68c = the base `3208ddc0`). The four delegates CH0 first saw here (`32b52470`, `32b524b0`, `32b6fd50`, `32b6f820`) are InitPassiveSkillState's (CH4a).
