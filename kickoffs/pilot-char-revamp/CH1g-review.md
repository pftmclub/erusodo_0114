# June character Aisha (Stage 5 pilot) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1g review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1g - Done"
awk -F'\t' '$6=="CH1g"' june2017_re/char_arme/ch0_class_map.tsv      # the 23 June functions (two have no member yet, see below)
grep -a "Char CH1g" june2017_re/funcmap.tsv                            # the 23 rows the phase wrote
python june2017_re/charport_consts.py CH1g                             # every constant, with its width
```

The `June_*` members of CH1g are in the seventh block (`//{{ ... Phase CH1g`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1f's in the header. Four enum members were added after `AVSI_AEM_ZZXX_END`: `AVSI_ABM_DASH_JUMP_ZXX` (238), `AVSI_ADW_DASH_JUMP_X2` (239), `AVSI_ADW_DASH_JUMP_X3` (240), `AVSI_ADW_ZZZBackZ` (244).
**Not carved in Ghidra (read with `june2017_re/funcdis.py --func <va>`):** `32cb8570`, `32cb8640`, `32cc1620`, `32cc16f0`, `32cc45c0`, `32cc4980`, `32cc4c90`, `32cc5050`, `32cc5260`, `32cc73e0`, `32ce72f0` (its range also holds the function before it; start at `32ce72f0`).

## Task

**Part 1 - the two StartFutures the Sonnet phase did not write (this needs Opus).** `32cb8bf0` (`AVSI_ABM_DASH_JUMP_ZX.StartFuture`) and `32cb8c80` (`AVSI_ADW_DASH_JUMP_X2.StartFuture`) are the same body: they copy `syncData.position` (`+0x7e3c..0x7e44`) into a local, call **`32021390( 1000.0, -600.0, &pos, 5 )`**, write the position back, then `CommonStateStartFuture` (vslot `0x7c0`).
`32021390` is an unmapped CX2GUUser method (checklist 7ac, `ch0_checklist_rows.tsv` L747, "landing prediction", June-only; it adds to `nowSpeed.y` and calls `32015b30` `n` times, then asks the line map with `31c6c310` and clamps y). Port it (a `June*` member, whole, with its line-map callee mapped to March's), then write both StartFutures. Record the helper in `funcmap.tsv`.

**Part 2 - review the 23 written functions.** What the phase could get wrong:
1. **ABM Z1 / Z3 (`32cbfe50`, `32cc00f0`).** Z1: the animation end now comes before `SKILL_CANCEL_AFTER( 0.01 )` (March: after). Z3: the ADW back-key branch (`0xf4`, no flag clear), the plain window, flag -> Z4 `0xcb`, `0.647` + back -> walk `0xe`, double-tap -> dash (flag cleared), then the skill / hyper cancel and the end (`0.1` / WAIT) as the last `else`. Check the `fcom` directions in the listing; the decompile hides them.
2. **The four shared bodies** (`32cc16f0`, `32cc1900`, `32cc4980`, `32cc5050`): the listings are byte-identical after normalising jump targets. Confirm, and that `0x8c98` is the walk, `0x8c90` the skill, `0x8c9c` the dash cancel time.
3. **`32cc45c0` / `32cc4c90`.** End waits (no fall); flag -> state `0xee` / `0xf0`; window 0 with oneX / oneZ; then skill, walk, bwalk (`0x8c94`), dash, oneZ / oneX (`0x8c88`) ground cancels. The two differ only in the state and the key (`diff` of the normalised listings).
4. **`32cc5260`** (ADW_ZZZBackZ EventProcess): order skill, bwalk, walk, dash, then pureZ / pureX on `0x8c8c` (`m_fAttackCancelAfter`).
5. **`32ce72f0` / `32cc73e0` / `32cb8d10`.** FrameMove is `32ce61f0`'s shape with `5 x +0x95bc`; the failure effect is `( 0, 180, 0 )`; StartFuture sets the dash speed when the held key faces the facing; EndFuture turns the facing around.
6. **`32cdc0f0`** (ABM Z2a FrameMove): the Advanced Teleportation memo block (skill memo `0x92f95` = 602005 = `SMI_AISHA_MEMO5`) with the super armor `( 0, 0.5 )`; confirm `FUN_31fca9e0( 0x92f95 )` is `GetEqippedSkillMemo`.
7. **`= March` bodies** (`32cc0000`, `32cc0390`, `32cc0500`, `32cc05a0`, `32cc06e0`, `32cc0b80`, `32cc1620`, `32cb8570`): constants (`charport_consts.py CH1g`) and call order only.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1g commit>^..<CH1g commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1g -- Review" into the plan doc under the Done note, update the CH1g row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1h.md` still matches, and commit with explicit pathspecs.
