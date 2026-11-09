# June character Rena (Stage 7) - Phase CH1g2 - Class core: Night Watcher combos, dash XZ / XZZ, start / win / lose (CH0 split of CH1g)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1g2` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g review. One at a time with the other `X2GULire_ElvenRanger.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1g2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Done" "CH1g - Review"
awk -F'\t' '$7=="CH1g2"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g2` (Night Watcher combos, dash XZ / XZZ, start / win / lose) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1g` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 29** (`awk -F'\t' '$7=="CH1g2"' june2017_re/char_rena/ch0_class_map.tsv`; inside 12, new 17).
  June addresses: `32c6d5b0`(new LESI_RNW_DASH_XZZ_START_StateEndFuture) `32c6d5c0`(new LESI_RNW_DASH_XZZ_END_StateEndFuture) `32c75160` `32c754d0` `32c75840` `32c81b30`(new LESI_RNW_DASH_XZ_EventProcess) `32c81e80`(new LESI_RNW_DASH_XZZ_START_EventProcess) `32c82100`(new LESI_RNW_DASH_XZZ_LOOP_EventProcess) `32c82390`(new LESI_RNW_DASH_XZZ_FULL_CHARGE_END_EventProcess) `32c825f0`(new LESI_RNW_DASH_XZZ_END_EventProcess) `32ca0d30`(new LESI_RNW_DASH_XZ_StateEnd) `32ca0d90`(new LESI_RNW_DASH_XZZ_START_StateEnd) `32ca0df0`(new LESI_RNW_DASH_XZZ_LOOP_StateEnd) `32ca0e60`(new LESI_RNW_DASH_XZZ_FULL_CHARGE_END_StateEnd) `32ca0ec0`(new LESI_RNW_DASH_XZZ_END_StateEnd) `32ca62f0` `32ca6350` `32ca63b0` `32ca7690`(new LESI_RNW_DASH_XZ_StateStart) `32ca76b0`(new LESI_RNW_DASH_XZZ_START_StateStart) `32ca76d0`(new LESI_RNW_DASH_XZZ_LOOP_StateStart) `32ca76f0`(new LESI_RNW_DASH_XZZ_FULL_CHARGE_END_StateStart) `32ca7710`(new LESI_RNW_DASH_XZZ_END_StateStart) `32ca9490` `32ca94c0` `32ca94e0` `32ca9510` `32ca9530` `32ca9550`.
- **States: 13** (`awk -F'\t' '$8=="CH1g2"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_COMBO_RNW_ZZX`, `LESI_COMBO_RNW_ZZXX`, `LESI_COMBO_RNW_ZZXXX`, `LESI_RNW_DASH_XZ`, `LESI_RNW_DASH_XZZ_START`, `LESI_RNW_DASH_XZZ_LOOP`, `LESI_RNW_DASH_XZZ_END`, `LESI_RNW_DASH_XZZ_FULL_CHARGE_END`, `LESI_RNW_START`, `LESI_RNW_WIN`, `LESI_RNW_WIN2`, `LESI_RNW_LOSE`, `LESI_RNW_LOSE2`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1g2`): 3; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- `LESI_RNW_DASH_XZZ_FULL_CHARGE_END` End `32ca0e60` spawns `NUI_RENA_TARGETING_GUNGNIR` (2083) through `CreateNPCReq` `@32ca11f2` (fDelay 0.3f): write the call with June's arguments (`ch0_traps.tsv` row 4); CH2d0 owns the spawn rule. A `LESI_RNW_WIN2` roll sets byte `+0x9714` = 1 (InitState `32cadc90 @~209`): CH1m's, noted for the win state.
- **Amended by CH1g (2026-11-09):** `CreateTrapBlade` (`32ca6410`), `DeleteTrapBlade` (`32c96e50`), `SetPriorPlannedBlow` (`32ca69e0`), `EndPriorPlannedBlow` (`32ca6b10`), `CheckPriorPlannedBlow` (`32ca6be0`) and `InsertPriorPlannedBlowEffect` (`32c9a5b0`) are March functions with June = March: CH1g wrote them under `_LIVE`, so the Night Watcher rows that call them (CH0 listed them as `new ?`) call them by name. `LESI_RTR_DOUBLE_JUMP_ZX_*` (271) is CH1g's and shared with Night Watcher: do not write it again.
