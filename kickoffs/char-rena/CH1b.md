# June character Rena (Stage 7) - Phase CH1b - Class core: reactions

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1b` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1a review. One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1b - Done" "CH1b - Review"
awk -F'\t' '$7=="CH1b"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1b` (damage (21 `LESI_DAMAGE*`), stand-up (6), die (2), start (2), win / lose blocks, wall, amusement (2), frozen, transformed) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GULire_ElvenRanger.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A March block under `ADD_RENA_SYSTEM` inside a function you transcribe stays as it is (dead) until CH1q: the June body's natural-force lines go under
  `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (an `#ifdef` inside the `_LIVE` body), never under the studio macro.
- A class-range callee (`0x32c6a8d0`-`0x32cb6c6d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py rena <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GULire_ElvenRanger.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF):
  `bytepatch.py` only; check each `#ifdef` branch with `cl /P` (`ADD_RENA_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, `SERV_TRAPPING_RANGER_TEST`, ...).
- Done gate: every class-map row with phase `CH1b` is written (the Done note lists them with June addresses and `inside` / `new`); the build
  passes with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class
  files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

## Amended by CH0 (2026-11-07)

- **Functions: 36** (`awk -F'\t' '$7=="CH1b"' june2017_re/char_rena/ch0_class_map.tsv`; inside 32, new 4).
  June addresses: `32c6aa40` `32c6aa70` `32c6ad20` `32c6ad50` `32c6ad70` `32c6adb0` `32c6add0` `32c6ae10` `32c6ae30` `32c6ae70` `32c6ae90` `32c6aed0` `32c6af10` `32c6de20` `32c6de80` `32c6e5e0` `32c6e630` `32c6ea30` `32c6ea80` `32c6ee80` `32c6f230` `32c6f290` `32c6f2f0` `32c6f350` `32c6f3b0` `32c6f440` `32c6f490` `32c6f510` `32ca2010`(new ?) `32ca5290` `32ca52d0` `32ca6e90`(new ?) `32ca9230` `32ca9360` `32cb61e0`(new ?) `32cb6310`(new ?).
- **States: 40** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_DIE_FRONT`, `LESI_DIE_BACK`, `LESI_START_INTRUDE`, `LESI_START`, `LESI_WIN`, `LESI_WIN2`, `LESI_LOSE`, `LESI_LOSE2`, `LESI_DAMAGE_GROGGY`, `LESI_DAMAGE_SMALL_FRONT`, `LESI_DAMAGE_SMALL_BACK`, `LESI_DAMAGE_BIG_FRONT`, `LESI_DAMAGE_BIG_BACK`, `LESI_DAMAGE_DOWN_FRONT`, `LESI_DAMAGE_DOWN_BACK`, `LESI_DAMAGE_STANDUP_FRONT`, `LESI_DAMAGE_STANDUP_BACK`, `LESI_DAMAGE_BOUNDING_DOWN`, `LESI_DAMAGE_BOUNDING_LANDCRASH`, `LESI_DAMAGE_BOUNDING_UP`, `LESI_DAMAGE_AIR_SMALL`, `LESI_DAMAGE_AIR_DOWN`, `LESI_DAMAGE_AIR_DOWN_INVINCIBLE`, `LESI_DAMAGE_AIR_DOWN_LANDING`, `LESI_DAMAGE_AIR_FALL`, `LESI_DAMAGE_AIR_UP`, `LESI_DAMAGE_AIR_FLY_FRONT`, `LESI_DAMAGE_AIR_FLY_BACK`, `LESI_DAMAGE_REVENGE`, `LESI_STANDUP_ROLLING_FRONT_FRONT`, `LESI_STANDUP_ROLLING_FRONT_BACK`, `LESI_STANDUP_ROLLING_BACK_FRONT`, `LESI_STANDUP_ROLLING_BACK_BACK`, `LESI_STANDUP_ATTACK_FRONT`, `LESI_STANDUP_ATTACK_BACK`, `LESI_WALL_LANDING`, `LESI_P_LE_AGILE_MOVEMENT_FRONT_FRONT`, `LESI_P_LE_AGILE_MOVEMENT_FRONT_BACK`, `LESI_P_LE_AGILE_MOVEMENT_BACK_FRONT`, `LESI_P_LE_AGILE_MOVEMENT_BACK_BACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.
- The four `LESI_P_LE_AGILE_MOVEMENT_*` states (ids 242-245) are here: InitState registers them with March's `StandUpRolling*` callbacks (`.cpp:1816-1838`).

## Amended by CH1a (2026-11-07)

- `Always.h` already holds `_LIVE` (`//#define`, `#error` guard for `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`) and `_NF` (`//#define`, needs `_LIVE`); CH1a had no `new` row, so **`_CORE` is not defined yet**: the first phase with a `new` row defines it and adds `_LIVE needs _CORE` to the guard block.
- File-local helpers go in `_CONST_RENA_<phase>_` after the `_CONST_RENA_` block at the top of the `.cpp` (CH1a's `_CONST_RENA_CH1A_` holds `IsMPChargeBlockedByGame()` = CX2Game +0xc20, always false).
- `ADD_RENA_SYSTEM` arms: write June's branch plain when every name it uses exists in March without the macro; use `_NF` only for lines that need an `ADD_RENA_SYSTEM` member (`m_eSaveStateSkillId`, ...).
- Generating a `bytepatch` spec from a bodies file with a small script worked (anchors need exact lines incl. trailing whitespace; short functions anchor on the `}` plus the next lines); `junediverge --check` passed with PlaySound calls.

## Amended by CH1a review (2026-11-07)

- `322cdfa0` (`g_pX2Game->GetSlideShot()`, byte 0 = `IsPresentNow()`) and `31aadcb0` (`g_pX2Game != NULL`) are now `confirmed` in `funcmap.tsv`: call them by their March names.
