# June character Chung (Stage 7) - Phase CH1g2 - Class core: Shooting Guardian X-front, dash-jump and Head Shot

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1g2` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1g review; one at a time with the other `X2GUChung.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1g2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1g2"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1g2"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1g2/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1g2` (the CSG X-front, dash-jump and Head Shot states CH0 split from CH1g) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1g` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 30** (`awk -F'\t' '$7=="CH1g2"' june2017_re/char_chung/ch0_class_map.tsv`; inside 26, new 4).
  June addresses: `32bd8ee0` `32bd8f00` `32bd8f60` `32bd9080` `32bd90e0` `32bd9140` `32bd9c10` `32bdb7f0`(new CSI_CSG_COMBO_DASH_JUMP_ZX_READY_StateStart) `32be0200` `32be0270` `32be0630` `32be0c30` `32be0cb0` `32be2ad0` `32bea970`(new CSI_CSG_COMBO_DASH_JUMP_ZX_READY_EventProcess) `32bee360`(new CSI_CSG_COMBO_DASH_JUMP_ZX_EventProcess) `32bf9100` `32bf92d0` `32bf92f0` `32bf9310` `32bf9770` `32bf9c30` `32bf9c70` `32bf9f50` `32bfc860` `32bfcc80` `32c0ac40`(new CSI_CSG_COMBO_DASH_JUMP_ZX_FrameMove) `32c181f0` `32c18f30` `32c19710`.
- **States: 9** (`awk -F'\t' '$8=="CH1g2"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON`, `CSI_CSG_COMBO_XFRONTX_FIRE`, `CSI_CSG_COMBO_XFRONTXX`, `CSI_CSG_COMBO_DASH_JUMP_X`, `CSI_CSG_COMBO_DASH_JUMP_XX`, `CSI_CSG_COMBO_DASH_JUMP_XX_Landing`, `CSI_CSG_COMBO_DASH_JUMP_ZX_READY`, `CSI_CSG_COMBO_DASH_JUMP_ZX`, `CSI_SI_A_CSG_HEAD_SHOT`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7u L637 June-side EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE named in Chung_Iron_Can.
- `EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE` is named by June's script and defined in no June EffectSet file: keep it, JUNE_BUGS row (logs `[RANOX_EFFECT_MISSING]`).
- `CSI_SI_A_CSG_HEAD_SHOT` (id 236) is registered by InitStateCommon's class-21 gate, so it is class core; its case id 26220 is `RNSI_A_CSG_HEAD_SHOT_TRANS` (no tree row, 26225 likewise).
- Memo lookups `31fdbcc0` at `32bfcbb0` / `32bfcfd8` / `32bfd1e7` ride with their functions.

## Amended by CH1d2 (2026-10-31)

- `CSI_CSG_COMBO_DASH_JUMP_ZX_READY` is already in `CX2GUChung::CHUNG_STATE_ID` (end of the enum, under `_CORE`, June script id 333, appended without `= value`). CH1d2's `ComboDashJumpZEventProcess` (`32bdece0`) sets its flag (`bEventFlagList[3]`, job line 2 = classes 21 / 111, `m_fEventTime[3]`) and switches into it; the state's own `32bdb7f0` / `32bea970` are yours. `CSI_CSG_COMBO_DASH_JUMP_X`'s switch in `AttackDashJumpXEventProcess` is Deadly Chaser only (JUNE_BUGS 492).
