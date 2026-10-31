# June character Chung (Stage 7) - Phase CH1b - Class core: reactions

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1a review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1b"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1b"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1b/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1b` (damage (21 `CSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1b` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 27** (`awk -F'\t' '$7=="CH1b"' june2017_re/char_chung/ch0_class_map.tsv`; inside 26, new 1).
  June addresses: `32bd88f0` `32bd8910` `32bd89d0` `32bd8a10` `32bd8e00` `32bd8e50` `32bd8e90` `32bdc940` `32bdc9a0` `32bdc9e0` `32bdca40` `32bdcba0` `32bdcc40` `32bdcd90` `32bdcde0` `32bdce20` `32bdcea0` `32bdf400` `32bedb80` `32bedd30` `32bedee0` `32bf8c50` `32c02ee0` `32c033b0` `32c04140`(new EraseTargetingNPC) `32c0dd90` `32c0de40`.
- **States: 33** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_DIE_FRONT`, `CSI_DIE_BACK`, `CSI_DAMAGE_GROGGY`, `CSI_DAMAGE_SMALL_FRONT`, `CSI_DAMAGE_SMALL_BACK`, `CSI_DAMAGE_BIG_FRONT`, `CSI_DAMAGE_BIG_BACK`, `CSI_DAMAGE_DOWN_FRONT`, `CSI_DAMAGE_DOWN_BACK`, `CSI_DAMAGE_STANDUP_FRONT`, `CSI_DAMAGE_STANDUP_BACK`, `CSI_DAMAGE_BOUNDING_DOWN`, `CSI_DAMAGE_BOUNDING_LANDCRASH`, `CSI_DAMAGE_BOUNDING_UP`, `CSI_DAMAGE_AIR_SMALL`, `CSI_DAMAGE_AIR_DOWN`, `CSI_DAMAGE_AIR_DOWN_INVINCIBLE`, `CSI_DAMAGE_AIR_DOWN_LANDING`, `CSI_DAMAGE_AIR_FALL`, `CSI_DAMAGE_AIR_UP`, `CSI_DAMAGE_AIR_FLY_FRONT`, `CSI_DAMAGE_AIR_FLY_BACK`, `CSI_DAMAGE_REVENGE`, `CSI_STANDUP_ROLLING_FRONT_FRONT`, `CSI_STANDUP_ROLLING_FRONT_BACK`, `CSI_STANDUP_ROLLING_BACK_FRONT`, `CSI_STANDUP_ROLLING_BACK_BACK`, `CSI_STANDUP_ATTACK_FRONT`, `CSI_STANDUP_ATTACK_BACK`, `CSI_STANDUP_CANNON_ATTACK_FRONT`, `CSI_STANDUP_CANNON_ATTACK_BACK`, `CSI_WALL_LANDING`, `CSI_AMUSEMENT_ITEM_PHOENIX_RUN_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8); §7l L469 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla.
- June registers `CSI_RIDING_DAMAGE_BACK` with no script table (CH1c's riding row; JUNE_BUGS row there).

## Amended by CH1a review (2026-10-31)

- `python june2017_re/charport_adis.py chung --phase CH1b --out <scratch dir>` now works (it read Eve's phase column before and printed nothing).
- `june2017_re/char_chung/ch1_offsets.tsv` no longer carries Eve's class fields (0x95bc-0x9c74 were hers); it has Chung's from CH1a. Add a row
  for each Chung field you identify, so later listings name it.
- The `_LIVE` probe the review ran: `june2017_re/census.py`'s `cl_cmd( 'X2GUChung.cpp', ... )` with `/P` and `/Fi` dropped and `/Zs /EHsc
  /DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` added, run in `X2Lib/` with `cl_env()`: exit 0 and no `error` line is a pass.
