# June port backlog - Stage 6 planning - transcendence, on Aisha

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort for the RE, then type `/x2-phase new-plan june-transcend` |
| Output | plan key `june-transcend` (`kickoffs/june-transcend/`, a `june-transcend` STATUS block) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 6), output `JUNE_TRANSCENDENCE_PHASES.md` (new) |
| Depends on | pilot-char-revamp Z (done 2026-10-22); Stage 4 quests (the class-change quest) |
| Flag | one family per phase, e.g. `SERV_IRUHADEV_JUNE_TRANSCEND_*` (none in this planning session: no code) |
| Log tag | `JUNE_TRANSCEND` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 6: transcendence system, on the pilot" "Open decisions that gate stages" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4a - Done (2026-10-22, Opus 5.5)" "CH4b - Done (2026-10-22, Opus 5.5)" "CH3b - Done (2026-10-21, Sonnet 5.5)" "CH6 - Done (2026-10-22, Sonnet 5.5)"
grep -n "Stage 6" JUNE_CHARACTER_AISHA_PHASES.md
```

## Task

Plan Stage 6 on Aisha (the only fully ported June character). First RE (read-only Ghidra) how June represents a transcended unit
(class ids vs a flag; `31fceab0` returns 0 for an untranscended unit) and whether new class ids need `GBase_Item` rows; if so, raise the
live-DB decision before planning further.

What Aisha's port left waiting for this stage (all inert or dormant today, each with the June address in her Done notes):
- the **transcendence slot**: skill slots 4 / 9 (A-5 / F); `SetEquippedSkillFunc` / `ChangeEquippedSkillState` skip index 4, `USI_SPECIAL_ATTACK_5` has no state, `31fcadd0` returns 4 / 9 (CH1's 7ag conversion);
- the **transcendence passives** 22415 Dark Deal and 22416 Open Mind (CH4a / CH4b; callback `32ce9f90`, power-rate type from `31fceab0`);
- `June_UnitFlag3c`, `AVSI_HYPER_MODE` vs the transcended hyper-mode branch (off today), and the hyper cut-in names `HQ_Cutin_AEM/AVP/ADW_Transcendence` (CH5);
- the **awakening effect-set family** (`32011d20`, `31fd66b0`, `32011f50`, `3204b5e0`, `32298590`, `32296cb0`; backlog bullet);
- `DLG_Transcendental_Change_Guide_Aisha`, the class-change quest `QT_CHANGE_TRANSCENDENTAL_CLASS`, `TranscendentalClassItem`, `TRANSCENDENTAL_SKILL_DATA`, `USI_TRANSCENDENTAL_HYPER_MODE`, the `SDT_*_TRANSCEND*` sockets and the emulator side;
- the **resonance system** (`3247b460`, unit `+0xa99` / `+0xa9d`) found by june-result R0: identify first, it may need its own stage.

Output:
- `JUNE_TRANSCENDENCE_PHASES.md`, in the Aisha plan's template shape (split by engine piece up front, a switch flag where needed, Opus review each);
- one kickoff per phase in `kickoffs/june-transcend/`;
- a STATUS block;
- funcmap rows tagged `Transcend S6`.

Play-test through Aisha's seven existing units (44-53) in the save. No code, no build. Open design questions go to the user at the end, each with a recommendation.
