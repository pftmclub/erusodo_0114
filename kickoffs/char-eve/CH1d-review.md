# June character Eve (Stage 7) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1d review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1d - Done"
awk -F'	' '$6=="CH1d"' june2017_re/char_eve/ch0_class_map.tsv      # 45 rows: 38 state functions, 3 EveElectraSystem methods, 4 struct rows
grep -a "Char Eve CH1d" june2017_re/funcmap.tsv                      # 41 PORTED rows and 4 "not ported here" rows
```

The 38 `June_ENSI_COMBO_*` members and the three `EveElectraSystem::June_*` methods are in the fourth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (it starts with the `JUNE_EVE_*` macros); declarations at the end of the class and in `EveElectraSystem` in `X2Lib/X2GuEve.h`.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1d --out <scratchpad dir>` and a Ghidra decompile of each address (`decompile_function`), both give the facts. You decide:

- **The macros** (`JUNE_EVE_ELSE_IF_STATE_CHANGE_ON_EX`, `JUNE_EVE_BWALK / WALK / WALK_..._CANNOT_ATTACKTIME / DASH_CANCEL_AFTER`): read them against June's expansion in `32969e40 @32969ea4-32969f08` (flag arm: `t > stateChange` or the animation has ended) and `3296a1f0 @3296a4cb-3296a5ac` (the three cancels all test `IsOnSomethingFuture`, which March's macros do not). Say whether any CH1d site is *not* this shape.
- **The event-flag EventProcess functions** (Z, ZZ, ZZZ, X, XX, ZZZdownZ): flag order, `m_fEventTime[ i ]` index, the key and class condition of every arm. Class reads: the templet form (`+0xaf0 -> +4`, `JUNE_EVE_TEMPLET_CLASS_IS`) in XX / ZZZ flag 3 / ZZZdownZ StartFuture; the unit-data form (`+0x38`, `JUNE_EVE_UNITDATA_CLASS_IS`) in ZZ, ZZZ flag 4 and ZZZdownZ EventProcess. Check each against the decompile.
- **The FrameMoves with new June content:** X / XX (`FlushMp( 4 * rate )`, BIG fire ball by `GetRandomFloat( 0 ) < m_fJuneElectronBallBigRate`, effect scaled by `m_fAttackPowerRateElectronBall`, `AddEffectToSystem` before `SetInheritForceDownRate`), XXX (`FlushMp( 4 )`, non-EG effect scaled), ZZZdownZ (non-EG effect scaled, force-down rate), XXfrontXX / XXdownXX (Queen's Throne effect set; the down one is a copy: JUNE_BUGS 423).
- **The member reading of +0x97ac .. +0x97c4** (range rate, MP rate, `m_fJuneElectronBallBigRate` +0x97bc, power rate +0x97c0, force-down rate +0x97c4) and `0x9698 = m_iSuccessiveHit_OneZ`; the ctor line `m_fJuneElectronBallBigRate = 0` (June ctor `3299bd70 @3299cb15`).
- **The helpers:** `32980580`, `32991a10`, `329a1150` are March's `AddEffectToSystem`, `DestroyPlaneAndCrystalEffect`, `CreatePlane` (Battle Seraph form) plus the alpha tags; the four struct rows (`3297b6b0` `329cdc80` `329cdf80` `329ce570`) are not ported because June adds two members (+0x8c, +0x90) nothing in the class range reads (CH1l4 kickoff amended). Confirm `329a1150`'s plane math against the listing (the three rotation matrices, the plane / normal stores, the primary-plane choice) since it is the one function copied from March on trust of its first half.
- **The alpha tags:** 19 sites in `c10a_alpha_sites.tsv` (20 calls in code: `329b07d7` serves both the third Light particle and the 0.23 s triangle).
- **The two new enumerators** `ENSI_ECN_COMBO_ZZX_READY` (231) and `ENSI_ECE_COMBO_ZZX` (234): March's C++ enum values differ from June's script ids (ZZ is 120 here, 166 in June), the same open question as `ENSI_EVE_RECOVERY`; CH1m settles the mapping.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1d commit>^..<CH1d commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1d -- Review" into the plan doc under the Done note, update the CH1d row in `STATUS.md`, check that `kickoffs/char-eve/CH1e.md` still matches, and commit with explicit pathspecs.
