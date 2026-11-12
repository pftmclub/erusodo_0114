# June character Elesis (Stage 7) - Phase CH1q - Way of the Sword: her class side and the gauge twins

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1q` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_WOS` (defined only with `_LIVE`) (+ `..._WOS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS WOS`) where the phase logs; otherwise none |
| Review | `kickoffs/char-elesis/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1q - Done (2026-11-04)" "CH1q - Review (2026-11-04)" "CH1q2 - Done (2026-11-04)" "CH1q2 - Review (2026-11-04)"
awk -F'\t' '$7=="CH1q"' june2017_re/char_elesis/ch0_class_map.tsv
cat june2017_re/char_elesis/ch0_wos.tsv
```

## Task

Way of the Sword from `ch0_wos.tsv`: June's whole body inside every March function of her class that differs, March's in `#else`. The shared
half is Elsword's and already June's (`SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`): call it, never copy it. The known pieces: Start / End Vigor `32b74760`
/ `32b74930` and Start / End Destruction `32b7f0b0` / `32b74bf0` (both Ends also erase `BTI_BUFF_GALE_WAY_OF_SWORD_DARK_KNIGHT` /
`BTI_BUFF_ANNIHILATION_WAY_OF_SWORD_DARK_KNIGHT` and play the `P_SDK_DARK_FORCE` end sets), `WayOfSwordStateChange` `32b7f370`, the two state-end
hooks `32b74f80` / `32b74fc0`, `GetFinalDamage_WayOfSword` 0x74 `32b74ed0` (1.25 / 1.5 by game type, times `+0x971c`), `SetCriticalPercent_WayOfSword`
0x78 `32b4ef90`, the state / point accessors 0x674 / 0x678 / 0x67c, `CreateGageData` 0xd0 `32b7ef90`; **the gain getters 0x720 / 0x724 / 0x72c**
(`32b903e0` / `32b903f0` / `32b90400`, members `+0x9714` / `+0x9718` / `+0x9720` with their writers): add the base virtuals from June (`3208e1b0` /
`3208e270`) and replace the two "CX2GUEL not ported" stand-ins (`X2DamageEffect.cpp:5543`, `X2GUUser.cpp:14837`) with the call; `_CONST_EL_::GALE_*`
against June's constants (`WayOfSword.lua` `Const_Elesis_*` only if June's code reads them); `CX2ElesisGageData::InitWhenGameIsOver` (`X2GageData.cpp:1129`,
June `32a34c60`: the state is no longer reset), `CX2ElesisMyGageUI::InitUI` (June `32e6e060` order) and `RefreshWayOfSwordUI` (vslot 0x5c `32e69e70`);
the snapshot pair 0x740 `32b6ac80` / 0x744 `32b6acb0` as `StoreGageSnapshot` / `RestoreGageSnapshot` (she keeps the point at `+0x92b8`, not the state;
the Crest bases stay unported: char-add); the emulator's `CAC_WSP` answer checked. Settle divergence rows 417 / 420 / 421 / 424 / 427. Split as CH0
says (CH1q2 for the gauge side if over ~45 functions). Sub-tag `WOS`; the debug lines stay until this phase's review. Full build only if this phase
hands off a play-test (it normally does not: CH1 final tests it).

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

- **Functions:** `awk -F'\t' '$7=="CH1q"' june2017_re/char_elesis/ch0_class_map.tsv` = 18 (8 listing-only: col 9 `n`; 13 `inside`, 5 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L261, L281, L844, L952.
- **`ch0_wos.tsv` rows:** 77 (filter on its `proposed_phase` column).
- **`ch0_blood.tsv` rows:** 3 (filter on its `proposed_phase` column).
- **`ch0_classn.tsv` rows:** 1 (filter on its `proposed_phase` column).
- `june2017_re/char_elesis/ch0_wos.tsv` (84 rows, `proposed_phase` = last column): 28 code-bearing functions (18 class incl. 5 June-only: `GetWayOfSwordPointGainRate` 0x720, `GetVigorHpIntakeRate` 0x724, 0x72c, `StoreGageSnapshot` / `RestoreGageSnapshot`; 3 gauge data; 4 gauge UI incl. `RefreshWayOfSwordUI`; the gauge manager arm is CHNa's; 2 stand-in sites): **no split**. Start Vigor / Destruction pick the Dark Knight buffs on job line 3 (BFI 473 / 471, PvP 472); both Ends erase BTI 435 / 434 and play the `DARK_FORCE` end sets: register `BFI_BUFF_GALE_WAY_OF_SWORD_DARK_KNIGHT` 473, `BFI_BUFF_ANNIHILATION_WAY_OF_SWORD_DARK_KNIGHT` 471, `..._PVP_DARK_KNIGHT` 472, `BTI_BUFF_GALE_WAY_OF_SWORD_DARK_KNIGHT` 435, `BTI_BUFF_ANNIHILATION_WAY_OF_SWORD_DARK_KNIGHT` 434 in `X2BuffFactor.h` under `JUNE_ENUM_NAMES`. `GetFinalDamage_WayOfSword` multiplies the Destruction result by `+0x971c` (Dark Force SA_DAMAGE_REL). `+0x9714` / `+0x9718` / `+0x971c` default 1 / 0 / 1 (ctor, InitPassiveSkillState) and come from Dark Force 28410 (SA 147 / 49 / 15, CH4a). The emulator already answers `CAC_WSP` (comments at `X2OfflineStatTable.cpp:292-295` are stale). June does not read `WayOfSword.lua`'s `Const_Elesis_*` (its constants equal March's `GALE_*`).
