# June character Elsword (Stage 7) - Phase CH1k - Class core: skill-use overrides and SpecialAttackEventProcess

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1k` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1j review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1k-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1k"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1k` (the skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied) and SpecialAttackEventProcess `32cf3290` (uncarved) with its parts `32d1bd10` / `32cf5760` / `32d0d8e0`) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1k` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 15** (`awk -F'\t' '$7=="CH1k"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 9, new 6).
  June addresses: `32cf3290` `32cf5660` `32cf5760` `32d00fe0`(new ?) `32d08b30`(new IsNotCommonMoveState) `32d08d20`(new ?) `32d08d70`(new AfterImageFrameMove) `32d09050`(new ?) `32d09270` `32d09490` `32d0b5a0` `32d0d8e0` `32d1bd10` `32d1dbd0`(new ?) `32d1ddf0`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L251 class skill-use function sets the cooldown then calls ApplySkillCoolTi; §7 L260 quick-slot special-item prefix at the top of SpecialAttackEventProcess; §7 L261 dev free-skill gate around the cool-time / MP / hyper-stone block of t; §7 L263 riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup; §7ai L852 JuneShowSkillNameStateString callers; §7al L888 new CX2GUUser hooks for every class: 0x654, 0x850, 0x860, 512, UpdateE.
- The ctor `32d09490` (caller outside the class) and `InitializeElswordMinorParticleArray` `32cf5660` (called inside the ctor @32d0b07b). 0x840 `32d00fe0` (`?`): called from DamageReact @320478dd, applies skill 0x539c's buff when `GetApplyWeaponExtraDamageType` != 0. 0x850 `AfterImageFrameMove`.
- **Routed by CH1b (2026-11-04):** the ctor `32d09490` zeroes `m_fNoDownAtStandUpAttackRate` (`+0x9684`, declared by CH1b under `_LIVE`) @`32d0b2c7`; the reaction states `32cf5ad0` / `32cf5c90` / `32cf5e10` test `0 < it`.
