# June character Rena (Stage 7) - Phase CH1f - Class core: Sniping Ranger line (`SNIPING`, `SIEGE`, `LSR` / `RSR`)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1f` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1e review. One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1f-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1f - Done" "CH1f - Review"
awk -F'\t' '$7=="CH1f"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1f` (Sniping Ranger line: `SNIPING`, `SIEGE`, `LSR` / `RSR`) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1f` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 43** (`awk -F'\t' '$7=="CH1f"' june2017_re/char_rena/ch0_class_map.tsv`; inside 38, new 5).
  June addresses: `32c6af50` `32c6af60` `32c6afb0` `32c6aff0` `32c6b030` `32c6b070` `32c6b1b0` `32c6b1c0` `32c6b2b0` `32c6d500`(new LESI_RSR_DASH_ZX_StateStart) `32c6d520`(new LESI_RSR_DASH_ZX_StateStartFuture) `32c6e450` `32c6f560` `32c73500` `32c73960` `32c74800` `32c74840` `32c74a10` `32c77f90` `32c7b1b0` `32c7b530` `32c7b570` `32c7ba40` `32c7bd20`(new ?) `32c810e0`(new LESI_RSR_DASH_ZX_EventProcess) `32c83c90` `32c83d00` `32c83f70` `32c84020` `32c840d0` `32c87c20` `32c87d30` `32c8aa40` `32c90620` `32c909e0` `32c928f0` `32c92df0` `32c99a90` `32c99ec0` `32c9a300` `32c9faa0`(new LESI_RSR_DASH_ZX_FrameMove) `32ca5330` `32ca5aa0`.
- **States: 20** (`awk -F'\t' '$8=="CH1f"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_LSR_START`, `LESI_LSR_WIN`, `LESI_LSR_WIN2`, `LESI_LSR_LOSE`, `LESI_LSR_LOSE2`, `LESI_SIEGE_READY`, `LESI_SIEGE_WAIT`, `LESI_SIEGE_ATTACK`, `LESI_SIEGE_ATTACK_X`, `LESI_SIEGE_END`, `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_X`, `LESI_SNIPING_RANGER_COMBO_X2`, `LESI_SNIPING_RANGER_COMBO_X3`, `LESI_SNIPING_RANGER_COMBO_X4`, `LESI_RSR_DASH_ZX`, `LESI_SNIPING_RANGER_DASH_COMBO_X1`, `LESI_SNIPING_RANGER_DASH_COMBO_X2`, `LESI_SNIPING_RANGER_DASH_JUMP_COMBO_X2`, `LESI_SNIPING_RANGER_DASH_JUMP_COMBO_X3`, `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1f`): 10; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX` Start / FrameMove / EventProcess (`32c6ac20` / `32c8b2c0` / `32c6e590`) are CH1q's (March: only under `ADD_RENA_SYSTEM`); its FrameMoveFuture `32c83c90` is June-only and stays here (`new`).
- **Amended by CH1d2 (2026-11-09):** `LESI_RSR_DASH_ZX` (June id 267) and `LESI_RGA_DASH_JUMP_XXZ` (269) already exist as `_CORE` enum members in `X2GULire_ElvenRanger.h`; CH1d2's dash Z1 EventProcess (class 13 / 103, `oneX`, flag 2 / event time 2) and dash-jump X1 EventProcess (class 103, `oneZ`) change into them. Do not add them again.
- **Amended by CH1e review (2026-11-09):** the chain macros `ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_` and `RENA_GROUND_*` are under `_CORE`, so `new` rows may use them. `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` is still under `_LIVE`: a `new` row that needs it moves its guard to `_CORE` (a flagcheck deleted-line pair, like CH1e).
