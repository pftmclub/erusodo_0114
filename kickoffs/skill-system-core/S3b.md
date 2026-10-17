# June skill system core - Phase S3b - June skill notes (memo)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3b` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S3 (June's templets loaded; the 3 March note rows already dropped) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_NOTE` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] NOTE` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S3b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase S0a - Templet and tree RE"
```

Then the `SkillNoteTemplet.lua` row of `june2017_re/s0a_skill_data_closure.tsv`, and `june_src_fixed/data036_kom/SkillNoteTemplet.src.lua`.

## Task

Added by S0a (2026-10-17). June's skill-note (memo) system is unowned elsewhere, so it gets its own phase (memory
`plan-unowned-june-branches-without-asking`).

- **RE first (Opus).** Map June's `CX2SkillNoteManager`:
  - the loader (`32452270`, file name @`3245252a`);
  - the bindings `AddSkillNoteTemplet 32453160`, `AddNewSkillNoteTemplet 32454840` (new), `AddMemoIDMatchItemID` and
    `AddRemovableMemoItemID`;
  - the `MEMO_ID` enum;
  - the readers, i.e. how an equipped memo changes a skill. Cross-check funcmap's memo rows (`31fcf3e0`, `328905b0`, `32205c00`).
  - Compare each with March's `X2SkillNoteManager.cpp` (March binds only `AddSkillNoteTemplet`, `X2Main.cpp:5483`).
- **Port** June's manager and bindings under the flag. Adopt June's `SkillNoteTemplet.lua` (ljconv) into data036.
- **Emulator:** bring the note handlers and the `unit_skill_note` table to June's memo ids. The 3 March rows were dropped by S3.
- **Gate:** the `[JUNE_SKILL] NOTE` load line counts 86 `AddNewSkillNoteTemplet`, 23 `AddMemoIDMatchItemID` and
  7 `AddRemovableMemoItemID`; equipping one memo on a ported character logs its effect. If no ported character has a memo skill
  yet, the load counts are the gate and the play-test waits for that port.
- Stop and ask only where copying June breaks March.
