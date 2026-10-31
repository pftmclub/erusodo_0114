# June character Chung (Stage 7) - Phase CH1f - Class core: Iron Paladin line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1f` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1e review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1f-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1f"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1f"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1f/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1f` (the 16 `CSI_CIP*` states) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1f` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 27** (`awk -F'\t' '$7=="CH1f"' june2017_re/char_chung/ch0_class_map.tsv`; inside 25, new 2).
  June addresses: `32bda3a0` `32bda3b0` `32bda3c0` `32bda410` `32bda420` `32bda430` `32bda440` `32bda450` `32bda460` `32bda470` `32bda480` `32bda490` `32bda4a0` `32bda4b0` `32bda4c0` `32be42c0` `32be4500` `32be4710` `32be4810` `32be48e0` `32be4b40` `32be9930`(new CSI_CIP_COMBO_DASHJUMP_XX_EventProcess) `32be9960`(new CSI_CIP_COMBO_DASHJUMP_XX_LANDING_EventProcess) `32c01fd0` `32c02280` `32c02530` `32c02920`.
- **States: 10** (`awk -F'\t' '$8=="CH1f"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_CIP_COMBO_DASH_X`, `CSI_CIP_COMBO_DASH_XX`, `CSI_CIP_COMBO_DASH_XXX`, `CSI_CIP_COMBO_DASH_XXXX`, `CSI_CIP_COMBO_DASH_XXXZ`, `CSI_CIP_COMBO_DASHJUMP_XX`, `CSI_CIP_COMBO_DASHJUMP_XX_LANDING`, `CSI_CIP_POWER_DOWN_LANDING`, `CSI_CIP_COMBO_POWER_DOWN_LANDING_Z`, `CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8).
- `LEAP_ATTACK_LANDING` is one state (id 228) whose table name is picked at run time (class 110 -> CIP name, else CFG name); one set of callbacks `32c23430` / `32c23440` / `32c23f80`.

## Amended by CH1d2 (2026-10-31)

- `CSI_CIP_COMBO_DASHJUMP_XX` is already in `CX2GUChung::CHUNG_STATE_ID` (end of the enum, under `_CORE`, June script id 325, appended without `= value`). CH1d2's `AttackDashJumpXEventProcess` (`32befa80`) sets its flag (`bEventFlagList[0]`, class 110 + X, `m_fEventTime[0]`) and switches into it; the state's own `32be9930` / `32c01be0`-side functions are yours. `AttackDashXEventProcess` (`32bdea80`) already switches Iron Paladin into `CSI_CIP_COMBO_DASH_XX` on Z at 0.53333.

## Amended by CH1e (2026-10-31)

- `32c01be0` (`CFGComboDashZFrameMove`, shared by `CSI_CFG_COMBO_XXFRONTZ`, `CSI_CFG_COMBO_DASH_Z` and `CSI_CIP_COMBO_DASHJUMP_XX_LANDING`) was written in CH1e (June body = March's). The class map's CH1f rows do not repeat it; CH1m registers `CSI_CIP_COMBO_DASHJUMP_XX_LANDING` with that function.

## Checked by the CH1e review (2026-10-31)

- The five cancel members June reads are `m_fAttackCancelAfter` `+0x8c8c` (Z / X through vslots `0x4b0` / `0x4b4`), `m_fSkillCancelAfter` `+0x8c90`, `m_fBWalkCancelAfter` `+0x8c94`, `m_fWalkCancelAfter` `+0x8c98`, `m_fDashCancelAfter` `+0x8c9c` (`X2GUUser.h:4755-4759`). In CH1e every one of them is ground-gated (`+0x7e1c`); `CFGComboXXFrontZEventProcess` is the worked example.
