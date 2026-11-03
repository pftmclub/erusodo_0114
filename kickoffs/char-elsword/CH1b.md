# June character Elsword (Stage 7) - Phase CH1b - Class core: reactions

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1b` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1a review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1b"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1b` (damage (23 `ESSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement, revenge) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1b` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 35** (`awk -F'\t' '$7=="CH1b"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 35).
  June addresses: `32cf3360` `32cf3380` `32cf34d0` `32cf3510` `32cf3550` `32cf3590` `32cf35d0` `32cf3610` `32cf3690` `32cf36a0` `32cf3cf0` `32cf3d30` `32cf57f0` `32cf5850` `32cf5890` `32cf58f0` `32cf5a80` `32cf5ad0` `32cf5c50` `32cf5c90` `32cf5e10` `32cf5f90` `32cf6010` `32cf6090` `32cf6110` `32cf6190` `32cf6210` `32cf6370` `32cfa810` `32cfa890` `32d07090` `32d076f0` `32d0da10` `32d0daa0` `32d0edb0`.
- **States: 38** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_START`, `ESSI_WIN`, `ESSI_WIN2`, `ESSI_LOSE`, `ESSI_LOSE2`, `ESSI_DIE_FRONT`, `ESSI_DIE_BACK`, `ESSI_DAMAGE_GROGGY`, `ESSI_DAMAGE_SMALL_FRONT`, `ESSI_DAMAGE_SMALL_BACK`, `ESSI_DAMAGE_BIG_FRONT`, `ESSI_DAMAGE_BIG_BACK`, `ESSI_DAMAGE_DOWN_FRONT`, `ESSI_DAMAGE_DOWN_BACK`, `ESSI_DAMAGE_STANDUP_FRONT`, `ESSI_DAMAGE_STANDUP_BACK`, `ESSI_DAMAGE_BOUNDING_DOWN`, `ESSI_DAMAGE_BOUNDING_LANDCRASH`, `ESSI_DAMAGE_BOUNDING_UP`, `ESSI_DAMAGE_AIR_SMALL`, `ESSI_DAMAGE_AIR_DOWN`, `ESSI_DAMAGE_AIR_DOWN_INVINCIBLE`, `ESSI_DAMAGE_AIR_DOWN_LANDING`, `ESSI_DAMAGE_AIR_FALL`, `ESSI_DAMAGE_AIR_UP`, `ESSI_DAMAGE_AIR_FLY_FRONT`, `ESSI_DAMAGE_AIR_FLY_BACK`, `ESSI_DAMAGE_REVENGE`, `ESSI_STANDUP_ROLLING_FRONT_FRONT`, `ESSI_STANDUP_ROLLING_FRONT_BACK`, `ESSI_STANDUP_ROLLING_BACK_FRONT`, `ESSI_STANDUP_ROLLING_BACK_BACK`, `ESSI_WALL_LANDING`, `ESSI_STANDUP_ATTACK_FRONT`, `ESSI_STANDUP_ATTACK_BACK`, `ESSI_STANDUP_ATTACK_FRONT_NO_DOWN`, `ESSI_STANDUP_ATTACK_BACK_NO_DOWN`, `ESSI_REVENGE_ATTACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8); §7 L281 skill-group power socket rate 3201ad10 called from Elsword range 32d07.
- Die front / back call `End_SwordmanVigor` / `End_SwordmanDestruction` (CH1q): declare them with `// June <va>: body in CH1q`. Checklist L281: `32d076f0` is the Revenge reaction state (here).
