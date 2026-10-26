# June character Eve (Stage 7) - Phase CH1l4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l4 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (names registered under `SERV_IRUHADEV_JUNE_ENUM_NAMES`) |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l4 - Done"
awk -F'	' '$6=="CH1l4"' june2017_re/char_eve/ch0_class_map.tsv         # 35 rows: 28 helper June_ members + the CommonFrameMove override, 6 resolved to March / existing code
grep -a "Char Eve CH1l4" june2017_re/funcmap.tsv                        # 36 rows
```

The members are in the CH1l4 `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (about +3600 lines, namespace `_CONST_EVE_JUNE_CH1l4_`); declarations at the end of the class and inside `ENERGY_NEEDLES_Data`, `EveElectraSystem`, `EveElectraLaserData`, `EveElectraEffectData`, `EveElectraLockonData`. `CX2GUNPC::June_SetBasicScale` is in `X2GUNPC.*`.
Registered names: `RNSI_P_ECB_HYPER_OPTICAL_RESEARCH` 25411 and `JUNE_MEMO_ID` in `X2SkillTree.h`; `BTI_BUFF_CODE_RECOVERY_ECB` 672, `BTI_BUFF_QUEENS_CONTROL` 786, `BTI_BUFF_SENTRY` / `_ATOMIC_SHIELD` / `_OBERON` / `_OPHELIA` 410-413 and `BFI_BUFF_QUEENS_CONTROL` 812 in `X2BuffFactor.h`; `DTT_EVE_HYPER_OPTICAL_RESEARCH_CHECK` 39 in `X2DamageManager.h`. JUNE_BUGS 438-441.

## Task

Review against June. **Delegate the mundane reads** (`python june2017_re/charport_adis.py eve --phase CH1l4 --out <scratchpad dir>`, Ghidra dumps). You decide:

- **`329bc6e0` (laser, 2385 decompile lines) and `329c5bc0` (CommonFrameMove, 2157).** Both were written from the annotated decompile plus March's code as the template. Check the control flow against the listing: the laser's `m_bDie` paths, the plane / Giga Stream branches, the induction and fusion lock-on ranges (1600 / 800 in the plane, 1500 / 400 in the Giga Stream, x0.5 in PvP), `+0xb10` (`SetCheckSpectro`), the Taser rate (`+0x34`, ability 0xa9); in CommonFrameMove the Open Mind floor (skill 25415, `SA_TIME_ABS`, clock `+0x9c44`), the buff-gated summon cleanup (409-413 -> NPC 1454 / 384+519 / 682 / 1621 / 1622), the cloak and Queen's Throne order, the Mega Electron Ball and Taser blocks, and the tail calls (`329b5230` is left to CH1q).
- **Engine mappings made by shape, not by a funcmap row:** `327623f0` = `CX2GUNPC::June_SetBasicScale` (vslot 0x270, JUNE_BUGS 438); `3297b370`'s CEffect `+0x650` / `+0x648` = `DamageData::hitUnitList` / `bReAttack`; the `EffectSetInstance` rotation field read in `329c12a0` (`m_vRotationDegree` compiled, the offset is unchecked); the `32070510` receiver in `329a3760` (assumed the Thousand Stars templet) and the factor behind its cool-time floor.
- **Fields named by use, nothing in this phase writes them:** `+0x9770`, `+0x9c10`, `+0x9c28`, `+0x9c2c`, `+0x9c44` (`m_iJuneClock9c44`, written by `DoParrying` `329b4060`, CH1q).
- **`EveElectraEffectData`'s two extra fields** (+0x8c handle, +0x90 handle vector) were added because `329c12a0` reads them; confirm the constructor and copy sites (`3297b6b0`, `329cdf80`).
- **`SFI_SPEED_13`** is June's 11 (`enumscan` and `ev2.py` agree); the memo test `31fca9e0` is false in June, so the cloak's memo branch is dropped.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1l4 commit>^..<CH1l4 commit>` and the same for `JUNE_ENUM_NAMES`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1l4 - Review" into the plan doc under the Done note, update the CH1l4 row in `STATUS.md`, check that `kickoffs/char-eve/CH1l5.md` still matches, and commit with explicit pathspecs.
