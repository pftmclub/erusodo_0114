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
