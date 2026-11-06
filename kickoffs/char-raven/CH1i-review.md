# June character Raven (Stage 7) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1i review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1i - Done"
python june2017_re/charport_adis.py raven 32c28270     # SetEquippedSkillLevel (462 lines)
python june2017_re/charport_adis.py raven 32c4f640     # HyperModeFrameMove
```

## Task

Review the two CH1i rows against the listings. Check as hard as the Done note's claims:
- **`SetEquippedSkillLevel` `32c28270`:** all 17 blocks (skill id -> June state ids, in listing order), the `bChangeAll || id == X` test per block (the last block also accepts `0x5e99`, its templet is `0x5e88`), and every state id against `ch0_state_enum_names.tsv` column 2 (the exe's registration ids; column 5 is Enum.lua's other numbering and must not be used). Two ids have no table row and were inferred: `0xfa` = 250 = `RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH` (249 END and 251 OVER_END around it) and `0x13d` = 317 = `RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END_UPGRADE` (315 / 316 are its siblings); confirm or correct from the listing neighbours / `RAVEN_FIGHTER.src.lua`. `GetEquippedActiveSkillTemplet( id, true )` = June's "memo swap first". The dirty-state batch (`3207c1a0` / `31fdf970`) is left out as in Elsword and Chung.
- **Enum names:** the 18 `RNSI_` skill names (`X2SkillTree.h`, `JUNE_ENUM_NAMES`) against `Enum.src.lua`, the 10 `RAVEN_STATE_ID` members (`X2GURaven.h`, `_CORE`) against the registration ids (305-309, 312, 313, 315, 316, 317).
- **`HyperModeFrameMove` `32c4f640`:** the inlined `EventTimerOneshot( 0.001 )` (`fcom` + `test ah, 0x41` + `jne` skips when 0.001 <= the anim's `+0x24c`, `CKTDGXSkinAnim::EventTimer` `31d28e10`); the HP test (`fdivr`: now / max, `fcomp 0.5`, `test ah, 5` + `jp` skips unless ratio < 0.5); `GetSkillLevel( 0x5f5d, 1, 0, 1 )`; `GetSkillTemplet( 0x5f5d, 1 )` (the 1 is a flag: March's one-argument overload); `GetSkillAbilityValue( 0x42, level, 0, 0.0 )` with `0x42` = `SA_MP_RETAKE_ABS` in March's enum (check the index); vslot `0xf0` = `UpNowMp` (`ch0_vtable_all.tsv`); the last call is `( 0.05, 0.83, false )`.
- The CH1h review's comment fix in `GetHyperModeTime` (comment only).
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`; `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, not `/D`), `cl /EP` string counts, `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range <commit>^..<commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1j kickoff in the Review note.
