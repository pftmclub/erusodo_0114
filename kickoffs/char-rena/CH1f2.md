# June character Rena (Stage 7) - Phase CH1f2 - Class core: Grand Archer line (`LGA` / `RGA`) (CH0 split of CH1f)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1f2` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1f review. One at a time with the other `X2GULire_ElvenRanger.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1f2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1f - Done" "CH1f - Review"
awk -F'\t' '$7=="CH1f2"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1f2` (Grand Archer line: `LGA` / `RGA`) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

- **Functions: 37** (`awk -F'\t' '$7=="CH1f2"' june2017_re/char_rena/ch0_class_map.tsv`; inside 7, new 30).
  June addresses: `32c6d550`(new LESI_RGA_DASH_JUMP_ZXX_StateStartFuture) `32c6d5f0`(new LESI_RGA_GUNGNIR_READY_Init) `32c6d680`(new LESI_RGA_GUNGNIR_READY_StateEnd) `32c78880` `32c78be0` `32c78f10` `32c78f40` `32c81340`(new LESI_RGA_DASH_JUMP_ZXX_EventProcess) `32c815a0`(new LESI_RGA_DASH_JUMP_XXZ_FrameMoveFuture) `32c815d0`(new LESI_RGA_DASH_JUMP_XXZ_EventProcess) `32c82d10`(new LESI_RGA_GUNGNIR_CANCEL_EventProcess) `32c82f90`(new LESI_RGA_GUNGNIR_CONTROL_FIRE_EventProcess) `32c86890`(new ?) `32c958e0` `32c95b20` `32c96320` `32ca02a0`(new LESI_RGA_DASH_JUMP_ZXX_FrameMove) `32ca0a90`(new LESI_RGA_DASH_JUMP_XXZ_FrameMove) `32ca0f80`(new LESI_RGA_GUNGNIR_READY_FrameMove) `32ca1240`(new LESI_RGA_GUNGNIR_READY_EventProcess) `32ca1300`(new LESI_RGA_GUNGNIR_CANCEL_StateStart) `32ca1360`(new LESI_RGA_GUNGNIR_CONTROL_IDLE_EventProcess) `32ca1a00`(new LESI_RGA_GUNGNIR_CONTROL_IDLE_StateStart) `32ca1bb0`(new LESI_RGA_GUNGNIR_CONTROL_FIRE_StateStart) `32ca1cc0`(new LESI_RGA_GUNGNIR_CONTROL_FIRE_StateEnd) `32ca1d10`(new LESI_RGA_GUNGNIR_CONTROL_LEFT_StateStart) `32ca1d70`(new LESI_RGA_GUNGNIR_CONTROL_RIGHT_StateStart) `32ca1dd0`(new LESI_RGA_GUNGNIR_CONTROL_UP_StateStart) `32ca1e30`(new LESI_RGA_GUNGNIR_CONTROL_DOWN_StateStart) `32ca1e90`(new LESI_RGA_GUNGNIR_CONTROL_UP_LEFT_StateStart) `32ca1ef0`(new LESI_RGA_GUNGNIR_CONTROL_UP_RIGHT_StateStart) `32ca1f50`(new LESI_RGA_GUNGNIR_CONTROL_DOWN_LEFT_StateStart) `32ca1fb0`(new LESI_RGA_GUNGNIR_CONTROL_DOWN_RIGHT_StateStart) `32ca7750`(new ?) `32ca9740`(new ?) `32ca9c20`(new LESI_RGA_GUNGNIR_READY_StateStart) `32ca9c50`(new LESI_RGA_GUNGNIR_CONTROL_FIRE_FrameMove).
- **States: 23** (`awk -F'\t' '$8=="CH1f2"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_LGA_START`, `LESI_LGA_WIN`, `LESI_LGA_WIN2`, `LESI_LGA_LOSE`, `LESI_LGA_LOSE2`, `LESI_LGA_ZZX`, `LESI_LGA_ZZXX`, `LESI_LGA_DASH_JUMP_ZX`, `LESI_RGA_DASH_JUMP_ZXX`, `LESI_RGA_DASH_JUMP_XXZ`, `LESI_RGA_GUNGNIR_READY`, `LESI_RGA_GUNGNIR_CONTROL_IDLE`, `LESI_RGA_GUNGNIR_CONTROL_FIRE`, `LESI_RGA_GUNGNIR_CONTROL_LEFT`, `LESI_RGA_GUNGNIR_CONTROL_UP_RIGHT`, `LESI_RGA_GUNGNIR_CONTROL_DOWN_LEFT`, `LESI_RGA_GUNGNIR_CONTROL_DOWN_RIGHT`, `LESI_RGA_GUNGNIR_CONTROL_LEFT`, `LESI_RGA_GUNGNIR_CONTROL_RIGHT`, `LESI_RGA_GUNGNIR_CONTROL_UP`, `LESI_RGA_GUNGNIR_CONTROL_DOWN`, `LESI_RGA_GUNGNIR_CANCEL`, `LESI_RGA_GUNGNIR_CONTROL_UP_LEFT`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7ac L782 June-only CanDown with ( in ptr, out NULL ) in Rena Grand Archer LESI_.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1f2`): 2; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- `LESI_LGA_ZZXX` Start `32c6c130` is CH1q's (March `LESI_LGA_ZZXX_StateStart` only under `ADD_RENA_SYSTEM`, `.cpp:16891`). 7ac: `LESI_RGA_GUNGNIR_READY` CanDown call `@32ca111a`.
- **Amended by CH1f (2026-11-09):** `LESI_RGA_DASH_JUMP_XXZ_FROM_X2` (June id 270, the second registration of the 269 table) exists as a `_CORE` enum member; CH1f's dash-jump X2 EventProcess changes into it. `IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_` (first branch of a flagged follow-up chain) and `COMBO_HIT_GROUP_VALUE` ( `_CONST_RENA_CH1F_`, the 100 `SetNowDamageRelateSkillData` takes ) are in `X2GULire_ElvenRanger.cpp` under `_CORE`; `RNSI_A_RENA_SIEGEMODE` / `_UPGRADE` (23201 / 23221) are in `X2SkillTree.h`.
