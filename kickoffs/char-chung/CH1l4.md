# June character Chung (Stage 7) - Phase CH1l4 - Class core: CommonFrameMove

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1l4` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l3 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1l4-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1l4"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1l4"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1l4/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1l4` (CommonFrameMove 0x7cc and the helpers only it reaches) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1l4` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 6** (`awk -F'\t' '$7=="CH1l4"' june2017_re/char_chung/ch0_class_map.tsv`; inside 6).
  June addresses: `32c058a0` `32c08010` `32c080f0` `32c09390` `32c09ad0` `32c0d570`.
- `31f66620` (pooled alloc) is mapped by CH1l30: it is `CX2DamageEffect::DamageEffectData::operator new` (boost pool, element 0xb8), so `32c058a0`'s
  `31f66620( 0xb8 )` + ctor `31f8bf00` is `new CX2DamageEffect::DamageEffectData`, as March's carpet-bombing loop (X2GUChung.cpp:29183) writes it.
- The hyper-count-3 timer skip `32bf4680` and `32bf3f60` are CH1q's: declare them.

## Amended by CH1l2 (2026-11-01)

- CH1l2's `CommonFrameMoveFuture` writes the cannon ball count (now + 7 x extra in mobile armored mode) to `m_FrameDataFuture.syncData.iClassSyncInt` (June `+0x7ec0`) and no longer to `m_CannonBallCount`. March's `CommonFrameMove` sets the count of a unit that is not mine from `m_FrameDataNow.syncData.m_CannonBallCount`; check which Now field June's `CommonFrameMove` reads (`iClassSyncInt` is June `+0x8160`) and that the Future -> Now copy carries it.
