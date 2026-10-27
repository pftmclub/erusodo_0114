# June character Eve (Stage 7) - Phase CH2b2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2b2 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SUMMON_UNIQUE` (names under `SERV_IRUHADEV_JUNE_ENUM_NAMES`) |
| Log tag | `[JUNE_SUMMON]` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b2 - Done (2026-10-27, Opus 5.5)"
```

Rows: `june2017_re/funcmap.tsv` (grep `CH2b2`), `june2017_re/char_eve/ch2b2_summon_buff_map.tsv`, `june2017_re/char_eve/ch2b2_createnpc_census.tsv`.

## Task

Review CH2b2 against June (`python june2017_re/charport_adis.py eve <va>` for the raw listings):
- `3200bec0` gate by gate against `CX2GUUser::JuneKillOlderSummonsOfSameID` (`X2GUUser.cpp`): the `31fc96b0` gate, the unit-list walk (game vslots 0xf8 / 0xb0),
  `+0x264`, templet id with the `id != 0` skip, the gage / HP test (`test ah,1`: NaN HP is kept, as `GetNowHp() <= 0.f`), `+0x8b40` = `m_bSelfDestruction`, AI type,
  owner UID, vslot 0xf4 = `SetNowHp`.
- `31fc96b0`: all 28 cases and BFI values against the switch (re-run the unicorn map or read the jump table at `31fc9840` / `31fc986c`), and each registered
  `NUI_*` / `BFI_*` value against `june_src_fixed/data036_kom/Enum.src.lua`.
- The call site: March's `SetUserSummonedNPCInfo` tail must run for Oberon (create type `NCT_CHANGESTAGE_REMAIN`, `FINALITY_SKILL_SYSTEM`) and before `AddUnit`
  (`X2Game.cpp` CreateNPC); compare June's gate @`3230d983..3230d998` (create type 0 / 3, id != 0x10c1) and say whether the 0x10c1 skip matters offline.
- Interaction with existing ports: `June_ClearGuardAndSummon`, `June_SetSelfDestructSummonedNPC`, the Proton Bomb event case ( `JuneGetEventBombNpcUID` ), Chung's `SetSelfDestructSummonedNPC`.

Hygiene is one command:
`python tools/flagcheck.py JUNE_SUMMON_UNIQUE --range <CH2b2 commit>^..<CH2b2 commit>` (then `git log -SJUNE_SUMMON_UNIQUE <commit>..HEAD`).
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2b2 - Review" into the plan doc,
update the STATUS.md row, check `kickoffs/char-eve/CH2b4.md`, and commit.
