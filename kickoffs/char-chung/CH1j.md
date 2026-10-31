# June character Chung (Stage 7) - Phase CH1j - Class core: Tactical Trooper line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1j` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1j0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1j-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1j"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1j"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1j/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1j` (the 18 `CSI_CTT*` states) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1j` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 41** (`awk -F'\t' '$7=="CH1j"' june2017_re/char_chung/ch0_class_map.tsv`; inside 36, new 5).
  June addresses: `32bda820` `32bda860`(new CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD_StateStartFuture) `32bda8a0` `32bda8f0` `32bda970` `32bda9b0` `32bdaa00` `32bdaa80` `32bdaac0` `32bdab10` `32bdab50` `32bdaba0` `32bdabe0` `32bdac00` `32bdac50` `32bdac90` `32bdacb0` `32bdad00` `32be6c50` `32be6f40` `32be6fe0` `32be7180` `32be7230` `32be72f0` `32be7350` `32bec410` `32bec950` `32bece90` `32bf00c0`(new ApplySpineAimRotation) `32bf02c0`(new CSI_CTT_DASH_JUMP_COMBO_XX_EventProcess) `32bf07b0` `32bf3630` `32bf3710` `32bf4510` `32c06b00` `32c09e40`(new CreateCannonBulletDamageEffect) `32c0a5b0`(new CSI_CTT_DASH_JUMP_COMBO_XX_FrameMove) `32c0ff80` `32c102d0` `32c104d0` `32c108d0`.
- **States: 12** (`awk -F'\t' '$8=="CH1j"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_CTT_COMBO_ZZX`, `CSI_CTT_COMBO_ZZXX`, `CSI_CTT_DASH_COMBO_XX`, `CSI_CTT_DASH_JUMP_COMBO_XX`, `CSI_CTT_MOBILE_ARMORED_MODE_READY`, `CSI_CTT_MOBILE_ARMORED_MODE_LOOP`, `CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD`, `CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD`, `CSI_CTT_MOBILE_ARMORED_MODE_UNREADY`, `CSI_CTT_MOBILE_ARMORED_MODE_Z`, `CSI_CTT_MOBILE_ARMORED_MODE_X`, `CSI_CTT_COMBO_DASH_X`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8).
- **Depends on CH0's Opus helper phase CH1j0** (`32008660`, `31fe4870`, `31fe9840`).
- Opus helper phase **CH1j0** runs first (`32008660`, `31fe4870`, `31fe9840`).
- `CSI_CTT_MOBILE_ARMORED_MODE_X` (id 308) / `_Z` are registered by the class-117 gate.

## Amended by CH1d2 (2026-10-31)

- `CSI_CTT_DASH_JUMP_COMBO_XX` is already in `CX2GUChung::CHUNG_STATE_ID` (end of the enum, under `_CORE`, June script id 328, appended without `= value`). CH1d2's `AttackDashJumpXEventProcess` (`32befa80`) sets its flag (`bEventFlagList[1]`, class 117 + X, `m_fEventTime[1]`) and switches into it. `32bee7c0` (`AttackDashJumpXStartFuture`, shared with `CSI_ATTACK_DASH_JUMP_X`) is written; the state's own `32bf02c0` / `32c0a5b0` are yours.

## Amended by CH1j0 (2026-11-01)

The three engine callees of `32bf00c0` (`ApplySpineAimRotation`, new) are `CX2GUUser` members now; call them by these names (flags are on,
no `#ifdef` needed beyond the class's own):
- `32008660` -> `D3DXVECTOR3 GetNearestTargetAimDegree( fMaxUpDegree_, fMaxDownDegree_, fYawRangeDegree_, fMaxDistance_ )`
  (`SERV_IRUHADEV_JUNE_USER_AIM_TARGET`). June passes `+0x9878`, `+0x987c`, `+0x9880` and `+0x9884 x +0x97a4` (`32bf00c0`; `32befec0`
  passes `+0x9868` .. `+0x9874 x +0x9728`) and stores the result at `+0x9888` .. `+0x9890`.
- `31fe9840` -> `SetRotateBone( true, L"Bip01_Spine1", D3DXVECTOR3( -1, -1, 0 ), D3DXVECTOR3( yaw gap, -pitch, 0 ) )` and
  `31fe4870` -> `EraseRotateBone( L"Bip01_Spine1" )` (`SERV_IRUHADEV_JUNE_USER_BONE_ROTATION`); the erase runs when |yaw| and |pitch| are
  both <= `FLT_EPSILON`; the function ends with `m_fRotateBoneDegree` (`+0x8a68`) = the result's z.
- `RotateBone` is June's (`SERV_IRUHADEV_JUNE_DIVERGE_ROTATE_BONE`); `OnFrameMove` applies the map (`ApplyRotateBoneMap`), so CH1j writes
  no frame-move code for it.
