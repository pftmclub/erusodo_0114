# June character Rena (Stage 7) - Phase CH1i - Class core: overrides 0x600-0x7bc

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1i` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1h review. One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1i-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1i - Done" "CH1i - Review"
awk -F'\t' '$7=="CH1i"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1i` (overrides 0x600-0x7bc (`InitPassiveSkillState` `32ca78b0` only as a declaration: CH4a owns it)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1i` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 8** (`awk -F'\t' '$7=="CH1i"' june2017_re/char_rena/ch0_class_map.tsv`; inside 4, new 4).
  June addresses: `32c6a920` `32c7bed0`(new ShownSkillCoolTime) `32c852f0`(new ?) `32c85970` `32c863d0`(new ?) `32c881f0`(new ?) `32c886b0` `32ca5310`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L258 death-prevention pre-apply: gauge snapshot overrides (vslot 0x740 / 0x; §7ap L987 June slots CH1o2 gave a base: Lire 0x6a8 32c7bed0 (skill use 32cac5a0).
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1i`): 1; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- 0x740 `32c852f0` (calls base `32009780`) stays here; 0x744 `32c89230` writes the NF gauge (`32a317d0`, `32a36df0`) and is CH1q's. ShownSkillCoolTime 0x6a8 `32c7bed0` (7ap).

- **Amended by CH1f (2026-11-09):** `UpdateSpineAimRotation()` (June `32c863d0`, user vslot 0x774) is declared in `X2GULire_ElvenRanger.h` with `m_vSpineAimDegree` (June +0x97d4, the aim angles); CH1f's siege wait / Z FrameMove call it and the Z shot reads the member, but the **body is this phase's** (read `32c863d0`: `GetNearestTargetAimDegree( +0x97c4 .. +0x97d0 )` into +0x97d4, `SetRotateBone( Bip01_Spine1 )` / `EraseRotateBone`, +0x8a68 = pitch; Chung's `CX2GUChung::UpdateSpineAimRotation` is the twin). The four members +0x97c4..+0x97d0 are probably the constructor's (CH1h, as Chung's `m_fSpineAimMaxUpDegree` ...): check there and declare them when this phase needs them.

- **Amended by CH1h (2026-11-09):** `m_afSiegeAngle[4]` ( June +0x97c4 .. +0x97d0 ) is declared in the header and filled by `InitComponent` from the INIT_COMPONENT table's `SIEGE_ANGLE` ( up degree, down degree, yaw range, distance ), so `UpdateSpineAimRotation` ( `32c863d0` ) reads `m_afSiegeAngle[0..3]` ( `GetNearestTargetAimDegree( up, down, yaw, distance )` ), as Chung's `m_afSiegeAngle`. Nothing else to declare for it.
