# June character Eve (Stage 7) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1f review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1f - Done"
awk -F'	' '$6=="CH1f"' june2017_re/char_eve/ch0_class_map.tsv      # 25 rows
grep -a "Char Eve CH1f" june2017_re/funcmap.tsv                      # 25 PORTED rows
```

The 25 members are in the sixth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (it starts with the heading comment "Exotic Gear / Architecture ground states"); declarations at the end of the class in `X2Lib/X2GuEve.h`. No new enumerators, fields or helpers.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1f --out <scratchpad dir>` (the listing runs on past the end of a function that has no Ghidra object: stop at the next class-map address) and a Ghidra decompile where a function object exists (`329847f0` `32986440` `32986f00`; none for `32986710` `3296daa0` `3296e580` `329795a0` `329797d0`). You decide:

- **The Architecture / Empress power factor** in `329847f0` (`EVE_AT_COMBO_ZZZdownZZ`), `32986440` (`EVE_AT_DASH_COMBO_ZZ`) and `32986710` (`EVE_AT_DASH_COMBO_ZZZ`): `GetPowerRate()` x `+0x4064` (read as `m_SkillRelatedData.m_fStatAtkRateOfSummoned`, the Genocide Ripper factor) x `+0x97c0`, force-down `+0x97c4`, range 1.0, land position 0. Check `[esp+0x60]` / `[esp+0x10]` really is the `+0x4064` copy, and that `+0x4064` is `GetStatAtkRateOfSummoned()` (the Aisha CH3c row says "user +0x4064").
- **`329847f0` Exotic Gear half**: the two `Eve_ComboZ5Down_Particle01` sequences (AddRotate, AxisAngle, land position, then the alpha tag inside the null check), the `--vAngle.z += -30` of the first and `vAngle.y += 180`, `z += -30` of the second, `UpDownCrashCameraNoReset( 5, 0.15 )` between them, both `EVE_EG_ZZZdownZZ` effects with the default tail.
- **Class form**: every class test in the 25 reads the templet (`+0xaf0 -> +4`); the jump tables (`18 108` / `19 109`, `18 19 108 109`, `19` alone for the ZZ Init) are in the Done note.
- **EventProcess chains**: `3296d630` (SKILL_CANCEL_AFTER( 0.01 ), flag 0 at 0.53 -> state 192 for EG / Nemesis, the `C` skip list, 0.8 / 0.6), `3296daa0` (skip list `ZXCAS`, 1.5 / 0.75 / 1.0, the EG / Nemesis branch with `+0x8c8c` ... `+0x8c9c`, default = animation end only), `3296e580` (ZXC list, 1.2 / 0.935), `329795a0` = `329797d0` (dash-jump count, landing, the four cancels).
- **The 7c alpha tags**: `32984a5b`, `32984c26`.
- **No JUNE_BUGS row was written** (nothing in the 25 looks unintended: `--vAngle.z` is March's own); say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1f commit>^..<CH1f commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1f -- Review" into the plan doc under the Done note, update the CH1f row in `STATUS.md`, check that `kickoffs/char-eve/CH1g.md` still matches, and commit with explicit pathspecs.
