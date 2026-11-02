# June character Lu/Ciel (Stage 8) - Phase DU1 - Second body in `CX2GUUser`

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel DU1` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU3 review, char-chung Z (user, 2026-11-02: every Lu/Ciel code phase waits for it) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_DUO_BODY` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL DUO`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/DU1-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase DU - The two-body engine (DU1-DU5)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the DU1 row of "Phase DU" over `nu0_duo_engine.tsv` (rows assigned DU1). Each June function is ported whole, inside March's `CX2GUUser`
function under the flag. Every other class must see no change: the review diffs a `cl /P` of `X2GUUser.cpp` with the flag on and off for a
non-Lu/Ciel path. Nothing is visible until CH1; no play-test beyond an existing character's pass with no new log line.

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

## Amended by NU0

NU0 (2026-11-02), `june2017_re/char_luciel/nu0_duo_engine.tsv` (2,964 rows: DU1 440, DU1-accessor 2,248, DU2 48, DU3 6, DU5 123, out-of-scope 90,
Ain / Rose 9):
- **The members** mirror the first body at +0x6b4: `+0x8e7c` / `+0x8e80` second Future / Now mesh, `+0x8e84` second Future skin anim, `+0x8e88`
  second Now skin anim (refcounted), `+0x8e8c` second Spine frame (first `+0x8898`), `+0x8e90` active body, `+0x8e91` dual enabled (word write with
  `+0x8e92` end-duo-at-anim-end), `+0x8e93` latch; bone frames `+0x4214..+0x4228`. Writers: `+0x8e90` only ctor `32042471` and setter `3204e030`;
  `+0x8e88` ctor `32040e4e`, `32042433`, `31fe633a`; `+0x8e91` `32042479`, `32050e19`, `32059a27`, `32061d38`, `32063a5e`; `SetDuoEnabled` `32092a00`.
  `32762670` / `3276331b` are CX2GUNPC's (out).
- **The accessor (the bulk).** June's `GetXSkinAnimFuture` / `GetXSkinAnim` are duo-aware inline accessors (`32070cf0`: `+0x8e90 ? +0x8e84 :
  +0x87d0`; Now `32071d60` / `32071d80`; other body `32070d10` / `32071db0`, NULL when `+0x8e91` is 0), inlined into every class (Eve's range alone:
  420 reads). The 2,248 `DU1-accessor` rows are those inlines: one change in `X2GUUser.h:2092-2094` (and the Future twin), not per site.
  Every other class has form 0, so it reads March's member.
- **Second body creation:** only `InitMotion` `31fe5860 @31fe5f31-31fe6670`, gated on templet type `UT_LUCIEL`: mesh from the unit-script key
  `MINOR_UNIT_MOTION_FILE_NAME` then NU3's GameMotion0-2 / Emotion members; mesh `31cdef40` into `+0x8e7c` / `+0x8e80`, a 0x874 skin anim into
  `+0x8e84`, a clone into `+0x8e88`, MixAnim, 6 bone frames + the Spine frame. The look (equip list, form 1) is DU3's.
- **Sync** (c3e rows B / E / F / G / I; H is Ain's): `Serialize` `32d5c2e0`, `Deserialize` `32d5be30`, `SendFrameData` `3203d4d0`, `SetData`
  `3201f5f0`, converters `32071ea0` / `320829b0`. Bits: 0x10000 float (+0x7ea8 / +0x8148), 0x20000-0x80000 second position (+0x7e48.. / +0x80e8..),
  0x100000 short (+0x7ecc / +0x816c), 0x200000 class-skill float (+0x7ed8 / +0x8178), 0x1000000 active body (+0x7f5c / +0x81fc, writer `31fe9e40`).
- **Not-selected body** `32015b30 @32019f9b-3201a1c7`: runs when `32070d10` is non-NULL; offset = vslot 0x16c direction * `+0x8f54`, y `+0x8f58`,
  plus the rotated delta when `+0x8f5c`; `+0x8f5d` = `USE_NOT_SELECT_MOTION_BEFORE_POS` (state start copies `+0x7e48` to `+0x8f60`). Key parse
  Future `@32056bf2` / `@32057139`, Now `@32060e5e` / `@320613a6`. `USE_PHYSICPROCESS_NOT_SELECT_UNIT` / `..._ANIM_TIME_IN_SPEED` members not
  settled (`+0x8f50` candidate): read them first.
- **`DUO_TYPE`** parse `32050230` (Future) / `32057b80` (Now) into `+0x8acc`: 3 off (`UpdateDuo` `3204df80`), 0 / 1 body (`SetActiveBody`
  `3204e030`), 2 both. Resets `31fd4290` / `31fd4340` zero `+0x24c` on the second Now / Future anim.
- Also DU1: the dtor `3203a620`, `CommonStateStart(Future)`, `CommonFrameMove(Future)` `32061bf0` / `32063650`, the bone / hand functions
  (`31fd64a0`, `31fd93e0`, `31fda680`, `31fdc0e0` / `c310`, `31fde420` / `e6d0` / `ed80`), `THROW_ITEM_FrameMove` `31ff92d0`, `3200d1d0`,
  `3200e040`, `32015250`, the per-body protected float pairs `+0x88c8` / `+0x88d4`, and from the type switches (`nu0_type_switches.tsv`): the
  form-byte weapon-effect bones `X2GUUser.cpp:30828` (`31ff8510`), `X2GameUnit.cpp:11909` (`3228c540`), `:19465` (`32292610`), `SetUnitReForm`'s
  second skin (`31fda0d0`, the live `UT_LUCIEL: break` stand-in), and `PlaySoundRevive` (`31fdb390`, `LuVoice_` / `CielVoice_Revival.ogg` by `+0x8e90`).
- `dispsweep.py` misreads `cmp [reg+disp32], imm` and drops 66 / F2 / F3 prefixes: rescan with capstone's `disp_offset`.
