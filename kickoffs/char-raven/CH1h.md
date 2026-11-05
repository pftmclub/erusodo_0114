# June character Raven (Stage 7) - Phase CH1h - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1h` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1g2 review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1h-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1h - Done" "CH1h - Review"
awk -F'\t' '$7=="CH1h"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1h` (overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32c40b20`; ctor `32c3ece0` / dtor `32c54c10` offset map; 0x9c / 0xa0 only if CH0 does not give them to CH1q; the uncarved ones read from the listing)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GURaven.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A class-range callee (`0x32c25a20`-`0x32c6a63d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py raven <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (`*=` where June multiplies; March assigns in the Shadow Step / Shadow Buster
  states, checklist §7 l.256).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- `X2GURaven.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1h` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

## Amended by CH0 (2026-11-05)

- **Functions: 37** (`awk -F'\t' '$7=="CH1h"' june2017_re/char_raven/ch0_class_map.tsv`; inside 29, new 8).
  June addresses: `32c25a20` `32c25a60` `32c25aa0` `32c273c0` `32c273f0`(new ?) `32c281f0` `32c28230` `32c28810` `32c38340`(new SetSpecificValueByBuffTempletID) `32c383b0`(new UnSetSpecificValueByBuffTempletID) `32c38410` `32c3c970` `32c3ca00` `32c3da10` `32c3db70` `32c3deb0` `32c3e2c0` `32c3ea70` `32c409b0` `32c40b20` `32c41000` `32c43080`(new ?) `32c4c4e0` `32c4e7c0` `32c50fb0` `32c54960` `32c54c10` `32c56be0` `32c5d540`(new ?) `32c63660`(new ?) `32c68f90`(new ?) `32c68ff0` `32c69f60` `32c69f70` `32c69f80` `32c6a450`(new ?) `32c6a620`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ao L966 transcend: Raven hyper effect start 32c4e7c0 (vslot 49) returns at onc.
- 0x9c / 0xa0 (`32c69f60` / `32c68ff0`, uncarved) are March's inline `m_bHyperAura` pair (`X2GURaven.h:483-484`, byte +0x97c8): stay here. 0x73c / 0x748 are CH4a. HyperModeBuffEffectStart / End `32c4e7c0` / `32c50fb0` and AttackResultByType `32c56be0` (+ June-only arc helper `32c54960`) hold rage lines: call CH1q's functions (declared). The transcended return in `32c4e7c0` (7ao) is CH3d's to check. The dtor `32c54c10` calls ClearNasodBall. June ReInit `32c409b0` has no overheat cleanup (March's is dropped).
