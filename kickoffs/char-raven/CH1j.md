# June character Raven (Stage 7) - Phase CH1j - Class core: Common* overrides and CommonFrameMove

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1j` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1i review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1j-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1j - Done" "CH1j - Review"
awk -F'\t' '$7=="CH1j"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1j` (Common* overrides 0x7c0-0x7ec (uncarved 0x7c0 `32c25ab0`, 0x7c4 `32c43380`, 0x7d4 `32c25ac0`, 0x7d8 `32c25ad0`, 0x7dc `32c25ae0`, 0x7e8 `32c3d1e0`), CommonFrameMove and the helpers only it reaches) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1j` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 9** (`awk -F'\t' '$7=="CH1j"' june2017_re/char_raven/ch0_class_map.tsv`; inside 8, new 1).
  June addresses: `32c25ab0` `32c25ac0` `32c25ad0` `32c25ae0` `32c3d1e0` `32c40ce0` `32c42f90`(new ?) `32c43380` `32c55240`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L327 Burning Nasod Hand: 32c57660 (+0x9688 = skill 24401 learned); 32c43120.
- CommonStateStart 0x7c4 `32c43380` (uncarved) holds the `BURNING_NASOD_HAND_POSSIBLE` read @32c43911 and the +0x4108 = 2 write @32c43ac4: write it whole. The write only bites with June's 46 `AT_COMMAND` tags and 24401 learned, so it pairs with CH4a's setter (checklist 7b). CommonFrameMove `32c55240` calls `DoFrameMoveNasodBall` (CH1q) and `DeleteOverHeatObject` (CH1g2): declare them. March's class-side hyper ball block (`:5321-5363`) and the `SA_OVERHEAT_HP` re-read (`:5446`) are absent in June.
