# June transcendence - Phase T6c - Transcended class pictures (select screen, HUD, character info, inventory)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-transcend T6c` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T6 done (it is). Independent of T6b |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE` |
| Log tag | `[JUNE_TRANSCEND] IMAGE` (`python tools/logs.py --grep "JUNE_TRANSCEND] IMAGE"`) |
| Review | `kickoffs/june-transcend/T6c-review.md` (the phase writes it when done) |

## Why this phase exists (user, 2026-10-24)

A transcended character should show its transcended class picture on the character select screen (and in the HUD, character info and inventory), as in June. Today only the skill tree does.

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T6c - Transcended class pictures"
grep -n "31ee0de0" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Standing rules (every stage, every kickoff)"
```

Evidence: `june2017_re/transcend/t6c_image_callers.tsv` (T6c's planning scan: every one of the 40 June calls to `CX2Data::GetCharacterImageName` `31ee0de0`, its image type and
whether the unit's `+0x3c` flag is pushed directly; "no / indirect" is a lead, not a verdict) and `t0_readers.tsv` rows marked `UI Stage 9`.

## What the planning check found (2026-10-24)

- June's character select state (`CX2StateServerSelect`, June `3255xxxx-3257xxxx`) calls `31ee0de0( out, out2, class, CIT, unit +0x3c )` at `325659c9` and `3256ed1b` with
  `CIT_UnitSlot` (9), and at `3256ebe7` with a June-only type `0xa`. So the select-screen slot picture is the transcended one for a transcended unit.
- March's select screen uses the old four-argument lookup; T2 added the fifth argument and Aisha EM / VP / DW rows only for 160by160, gray, 50by50, 20by20, gauge and party
  (`X2Data.cpp`, funcmap row `31ee0de0 (Transcend T2)`). The unit-slot rows and type `0xa` are missing.
- The other flag-passing callers: inventory `321aa423` (UnitSlot), dungeon clear `324ce67b`, 160by160 `32515156` / `32602375` / `328888c5`, 50by50 `324939f9`, 20by20 several,
  HUD portrait (`CX2MyGageUI`, MyGage type 7: `32e5f45b`, `32e62e3c`, `32e67511`, `32e69199`, `32e6cce9`), character info `3291e505`/`3291df90`.

## Task

1. Identify June's `0xa` image type and what it shows on the select screen (read `3256ebe7` and its caller in Ghidra). Add it only if the select screen needs it.
2. Add the unit-slot (and `0xa` if needed) transcended rows for Aisha EM / VP / DW to the image table, and stage June's picture files if March's data lacks them (`kom.py stage-june`,
   byte-exact, no edits; the replace rule applies).
3. Pass the unit's transcended flag at the March select-screen sites, then the HUD portrait, character info, inventory, class name / icon (`32ed5180`) and result image sites that exist in March.
   Port each call site whole. Sites whose screen March does not have are listed, not ported.
4. Log one `[JUNE_TRANSCEND] IMAGE` line per picture lookup that took the transcended row (throttled).

Done gate: ArmeEM / ArmeVP / ArmeDW (transcendence 1) show a different picture from an untranscended Aisha on the select screen, in the HUD portrait and in character info; the log shows the lookups.
The play-test hand-off names the characters and what to compare on screen, with no unit or slot numbers.
