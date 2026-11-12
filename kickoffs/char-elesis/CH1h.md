# June character Elesis (Stage 7) - Phase CH1h - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elesis CH1h` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), the previous CH1 sub-phase (the char-ara Z gate was dropped, user 2026-11-12). One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1k review covers it (its `_DEBUG` define, if any, stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1h - Done" "CH1h - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1h"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1h` (the ctor `32b70ee0` / dtor `32b70990` offset map, Init `32b52560`, ReInit `32b81900`, InitEffect `32b4ef40`, InitComponent `32b81710` (its Start Vigor / Start Destruction calls under `_WOS`), DamageReact `32b73ff0` (her tutorial test `32b74065`), HyperModeBuffEffectStart 0xc4 `32b74300`, the particle arrays 0x540 `32b70330` / 0x548 `32b4ef70` / 0x54c `32b70300`, the state-id getters 0x4ac `32b90420` / 0x4b0 `32c69f70` / 0x4b4 `32b90410` (return the state by name: June's `0xbb` / `0xa4` / `0xa9` are not March's numbers), DetonationStateChange 0x4d4 `32b73e10`, 0x534 `32b90430`, 0x558 `32b6fb80`; the hit hooks 0x4f8 / 0x4fc / 0x734 only as declarations (CH4c)) from Ghidra and the raw listing, **whole**. The plan's "Transcription rules
(CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUEL` function under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE`, March's body in `#else`, `#endif SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE`. Add `_LIVE` to
  `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of `X2GUEL.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (the build's own
  flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*` alone
  (CH1m registers).
- **`listing-only` rows** (no Ghidra function): read with `python june2017_re/charport_adis.py elesis <va>` (Ghidra stays read-only). If the
  listing does not make a function's control flow plain, say "this needs Opus" for that function, list it in the Done note and go on with the rest.
- Way of the Sword lines that call a June piece CH1q has not written go under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_WOS`, summon lines under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SUMMON`, blood-hook lines under
  `SERV_IRUHADEV_JUNE_CHAR_ELESIS_BLOOD` (an `#ifdef` inside the `_LIVE` body); March's live Way of the Sword calls stay March's until CH1q.
- A class-range callee (`0x32b4e770`-`0x32b90b6f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee
  is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GUEL.cpp` / `X2GUEl.h` are CP949 with mixed CRLF / LF: `bytepatch.py`
  only, each inserted line with the EOL of its neighbours; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with this phase is written (the Done note lists them with June addresses, `inside` / `new` and `carved` /
  `listing-only`); the build passes with `_LIVE` undefined (one-file probe build, no full build: no play-test) and the one-file probe passes with
  it defined; `junediverge.py --check` and `enumscan.py --check` on the class files PASS. Nothing to play-test: the hand-off says so and names the
  next phase.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).

## Amended by CH0 (2026-11-12)

- **Functions:** `awk -F'\t' '$7=="CH1h"' june2017_re/char_elesis/ch0_class_map.tsv` = 24 (11 listing-only: col 9 `n`; 18 `inside`, 6 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L267, L296, L988.
- Overrides named by body: 0x29c `GetReduceDebuffDurationRate` `32b6fc40` (reads `+0x95d0`), 0x4ac `32b90420` returns June state 0xbb = id 187 = `ESI_SWORD_WAIT` (by name). Unnamed for this phase: 0x534 `32b90430` (base `3208e8c0` state test, then a class state-group test; name by body) and 0x558 `32b6fb80` (a gauge-data ratio, base returns 0). ReInit / InitComponent carry Way of the Sword lines (`ch0_wos.tsv`: ReInit nudges the point and ends + re-enters TOWARD outside PvP, InitComponent ends with `RefreshWayOfSwordUI`): those lines under `_WOS`. ReInit calls `32b81300` (Shadow Edge list, CH4c): declare it.
