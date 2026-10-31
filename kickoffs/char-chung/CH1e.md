# June character Chung (Stage 7) - Phase CH1e - Class core: Fury Guardian line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1e` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1d2 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1e-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1e"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1e"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1e/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1e` (the 14 `CSI_CFG*` states (the 2 `XXX_CSI_CFG_*` tables are dead data: confirm and list them)) from Ghidra, **whole**. The plan's "Transcription rules
(CH1a-CH1l4)" apply:
- **`inside` rows:** June's body inside March's `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one
  `cl /c` of `X2GUChung.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** a plain name in the class's pattern (CH0 proposed it) under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, compiled in every build,
  referenced by nothing yet. Leave `InitState*` alone (CH1m registers).
- A class-range callee (`0x32bd86f0`-`0x32c25a1f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee
  is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai,
  `s3e_bybfi_callers.tsv`).
- `X2GUChung.*` are CP949 (`.cpp` CRLF, `.h` mixed): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1e` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
  with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class files PASS.
  Nothing to play-test: the hand-off says so and names the next phase.

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

## Amended by CH0 (2026-10-31)

- **Functions: 12** (`awk -F'\t' '$7=="CH1e"' june2017_re/char_chung/ch0_class_map.tsv`; inside 10, new 2).
  June addresses: `32bd8ed0` `32bdf650` `32bdf820` `32bdfaf0` `32bdfba0` `32bdfe00` `32be0030` `32be0060` `32be9210`(new CSI_CFG_COMBO_XXFRONTZ_EventProcess) `32be9600`(new CSI_CFG_COMBO_XXFRONTZZ_EventProcess) `32c01be0` `32c1be30`.
- **States: 9** (`awk -F'\t' '$8=="CH1e"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_CFG_COMBO_XXFRONTZ`, `CSI_CFG_COMBO_XXFRONTZZ`, `CSI_CFG_COMBO_DASH_Z`, `CSI_CFG_COMBO_DASH_ZZ`, `CSI_CFG_COMBO_DASH_ZZZ`, `CSI_CFG_COMBO_DASH_JUMP_ZX`, `CSI_CFG_COMBO_DASH_JUMP_ZXX`, `CSI_CFG_COMBO_DASH_JUMP_ZXXLanding`, `CSI_CFG_COMBO_ZZX`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8).
- `CSI_SI_SA_CFG_IRON_HOWLING_HYPER` (a CH3a2 skill state) plays `SI_SA_CFG_IRON_HOWLING`, which no June motion file has: JUNE_BUGS row when CH3a2 writes it; noted here because the `XXX_CSI_CFG_*` dead tables sit beside it.
- The 2 `XXX_CSI_CFG_COMBO_DASH_JUMP_ZXX*` tables and `_CSI_CFG_COMBO_XXFRONTZ` are dead data (no June registration): nothing to write.
- `CSI_CFG_COMBO_XXFRONTZ` / `ZZ` are registered for 20 and 110 (32c1d1d0 case 0x6e falls into 0x14).

## Amended by CH1d (2026-10-31)

- `CSI_CFG_COMBO_XXFRONTZ` is already in `CX2GUChung::CHUNG_STATE_ID` (end of the enum, under `_CORE`, June script id 323). CH1d's `ComboXXEventProcess` (`32bde240`) sets its flag (`bEventFlagList[2]`, job line 1 = classes 20 / 110, `m_fEventTime[0]`) and switches into it, so the state's own `32be9210` / `32c01be0` are the only pieces left here. Add `CSI_CFG_COMBO_XXFRONTZZ` (script id 324) and any other June-only state of this line the same way (an enumerator at the end of the enum under `_CORE`, a one-line comment with June's script id).

## Checked by the CH1d2 review (2026-10-31)

- `CSI_CFG_COMBO_DASH_JUMP_ZX` (June 0xc3) is entered from CH1d2's `ComboDashJumpZEventProcess` (`32bdece0`): flag `bEventFlagList[1]` and `m_fEventTime[1]` (`+0x8c1c` / `+0x8c20` / `+0x8c24`), X plus `IsInJobLine( 1 )` (vslot 0x670). The ZX state's own functions in this phase read the same `m_fEventTime` layout. `IsInJobLine` is still only declared; CH1l writes its body.
- No other change: CH1d2's 21 bodies match June, and this kickoff stands as written.
