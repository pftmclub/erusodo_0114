# June character Elsword (Stage 7) - Phase CH1h - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1h` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1g2 review (char-chung Z done 2026-11-03). One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1h-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1h"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1h` (lifecycle, render and reaction virtuals below vslot 0x600, DamageReact, the ctor / dtor offset map; uncarved overrides read from the listing) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GUElsword_SwordMan.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A class-range callee (`0x32cf2da0`-`0x32d2763f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py elsword <va>` (Ghidra stays read-only).
- An alpha-tag site (7c, `c10a_alpha_sites.tsv`) inside one of these functions is written here.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- `X2GUElsword_SwordMan.*` are CP949 (`.cpp` mixed endings, `.h` mostly LF): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
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

## Amended by CH0 (2026-11-03)

- **Functions: 32** (`awk -F'\t' '$7=="CH1h"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 23, new 9).
  June addresses: `32cf2da0` `32cf2e20` `32cf5190` `32cf51d0` `32cf5530` `32cf55a0` `32cf56a0` `32d059b0`(new ?) `32d05b60`(new ?) `32d05b70`(new ?) `32d06fd0` `32d08590` `32d08ec0` `32d0b870` `32d0c780`(new ?) `32d0d3f0` `32d0d540` `32d0d600`(new ?) `32d0d7a0`(new ?) `32d17e90` `32d17ea0` `32d1a5d0` `32d1a920` `32d1ac60` `32d1b120` `32d1b9d0`(new ?) `32d1f690` `32d26ff0`(new ?) `32d27110` `32d27120` `32d27130`(new GetSpecialItemUsableExtraStateID) `32d27620`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L262 vslot 0x4ac GetSpecialItemUsableExtraStateID returns 0xce; §7 L291 tutorial flag read in CX2GUElsword_SwordMan::DamageReact 32d1af6c; §7c L352 29 alpha-tag sites 32d0d660 .. 32d1ecc0.
- 0x24c / 0x250 (`32d1f690` / `32d0b870`) stay here: they switch on buff ids 3 / 163-165 / 19 and set class fields (Chung sent his to CH4a; Elsword's read no passive).
- ReInit, DamageReact, InitComponent, InitEffect hold Way of the Sword parts: call CH1q's functions (declared, `body in CH1q`). DamageReact calls `CX2DamageManager::CalcWayOfSwordPointOnDamageReact` by its March name (June `326106c0` differs; allow-listed until CH1q).
- Slot names (`ch0_vtable_slots.tsv`, `ch0_unmapped_identified.tsv`): 0xbc `GetFinalDamageToBeChangedByActiveSkill` (inside), 0x4ac `GetSpecialItemUsableExtraStateID` (returns 0xce `ESSI_SWORD_WAIT`), 0x508 / 0x50c empty overrides (called from `GetFinalDamage` 32284d43 / 32284d62), 0x510 applies passive 0x53a9's buff after `ProcessSocketEffectEx` (DamageReact @320478c9). `32d270f0` / `32d27100` are compiler this-adjust thunks (n/a).
