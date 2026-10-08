# June effect completion - C9h-2b review - the cash-shop pet preview and the viewer hide sites

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2b done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_PET_PREVIEW` |
| Log tag | `JUNE_FX PET_PREVIEW` (`python tools/logs.py --grep "JUNE_FX PET_PREVIEW"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2b - Done" "C9h - Done"
grep -n "Effect C9h-2b" june2017_re/funcmap.tsv
```

## Task

Review the C9h-2b code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **ResetPetViewer vs `32228c90`**:
  - the guard (viewer and type != -1; nothing at all for -1);
  - the stops before the item test;
  - the `0 <= id && prev != item` test and the else-branch hide;
  - the scale: riding 0.5; pets 35-36 1.4 (`3344f240` is a float); else 1.0;
  - `Wait_Transform` -> `Wait`, played as XAP_LOOP (0) with transition true;
  - SetOverUI, then the position `(260,-30,0)`. That position is the user's decision (2026-10-08). Check only that `+0x51c` really has no other writer.
- **PetPreview vs `32234b10`**:
  - the `< 1` tests;
  - rand() runs only when both lookups hit, and odd picks riding;
  - the order of the trailing hide.
- **The pre-equip-list test**: `32234380` was ported as March's `CheckPreEqipItem` plus a templet-null test. Re-check the two claims behind that:
  - `+0x528` is set only on June's unit-type-10 path;
  - June's InsertPreEquipItem `3224cb20` uses `32234380` where March's calls `CheckPreEqipItem`.
- **The hides**: SetOpen `@3223e56d`, UpdateDLGPetViewerUI `@3223c763`, take-off-all `322383c0 @322385b0`. Check the order and that every no-argument `ResetPetViewer()` in March is replaced (`grep -n "ResetPetViewer()" X2Lib/*.cpp`): one left behind would now hide nothing.
- **Pet info**:
  - Select_Tab `32902000 @32902243` and SetShow `32902700 @32903116`: riding viewer `+0xb8` first, then the pet viewer `+0x3c`;
  - that `+0x3c` / `+0xb8` really are `m_pNPCViewerUI` / `m_pRidingPetViewerUI` (a `cl` offsetof probe needs the members' access; a sizeof walk of the header works).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_PET_PREVIEW --range <first C9h-2b commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "C9h-2b - Review" into the plan doc, update the
phase's STATUS.md row, check the next kickoff (`C9h-2c.md`), and commit.
