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

## Amended by CH0 (2026-11-02)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 keys DUO_TYPE x390 / x158; §5 L184-188 keys MINOR_UNIT_ANIM_NAME x247 / x108, MINOR_UNIT_SPEED_X x4, CP_AMOUNT x2; §5 L184-188 keys MINOR_UNIT_AFTER_IMAGE x8 / x4, MINOR_UNIT_SPEED_TIME0-4 (x129 Duo / x133 D; §5 L184-188 key MINOR_UNIT_MOTION_FILE_NAME x1 (INIT_MOTION); §5 L184-188 Demonio keys USE_NOT_SELECT_MOTION_BEFORE_POS x13, USE_NOT_SELECT_UNIT_ANIM_TIME; §7 L241 AnimPlay extra skin anims +0x8e84 / +0x8e88 (31fe8130 / 31fe8050), clears +0x808; §7 L243 unit vslot 152 = user byte +0x8e90 (setter 3204e030), second after-image +0x6758; §7 L244 bone rotation second pair +0x8e84 / +0x8e88 (31fd93e0 @31fd954e, 32063650 @32065; §7 L274 last-hit serial +0x40dc read at 31fd0311 (user) and 3275a0f5 (NPC) for unit type; §7 L275 SocketData_DuoUnit (vt 33478ab0, 0x488) built by the user ctor 32042060 for type; §7n L496 user-sync class-skill float: Rose, Ain and Lu/Ciel drive it too; §7o L511-513 minor unit's second PhysicParam +0x36c, MINOR_UNIT_PASSIVE_SPEED_X/Y, APPLY_MOVE; §7o L519 walk-speed rescale uses the main unit (funcmap C3h rows 3205419d, 32015b30 @3201; §7o L530-532 MINOR_UNIT_SPEED_TIME(_ULTIMATE) twins (+0x8604, 262 June rows), main / minor li; §7y L680-681 Lu/Ciel's minor-body block in CX2GUUser::PhysicProcess (@320194bb, bMonster fals; §7av L1036 Lu/Ciel future anim = +0x8e90 ? +0x8e84 : m_pXSkinAnimFuture (dual body) in the .
- **Callees to map and port** (`ch0_unmapped_identified.tsv` col 6):
  - `31fd6260` CX2GUUser: loop m_ViewEqipList ( +0x8834 ) applying a show/hide with the active-body test ( +0x8e90 / +0x8e91 ) (candidate; callers' phases CH1a CH1l)
  - `31fdd8e0` CX2GUUser::CommonStateEndFuture ( vslot 0x7d8 base body, June version ) (confirmed; callers' phases CH1l2)
- **CH1a0 waits for this review** (the transform link sits beside the duo members, and CH1a calls `31fd6260`).
- Keys `CX2GUUser::CommonStateStart(Future)` parses for Lu/Ciel (`ch0_keys.tsv`), beyond the list NU0 gave: `FORCE_DYNAMIC_CAMERA_INDEX` @3205c0e8, `RETAIN_CAMERA_STATE` @3205b934, `SUPER_ARMOR_CUSTOM_COLOR` @3205941f (CH3c reads it), `MINOR_UNIT_AFTER_IMAGE` @3205b69b, `MINOR_UNIT_SPEED_X` @32052a1b / @320535e4; `MINOR_UNIT_MOTION_FILE_NAME` in InitMotion @31fe5fc6. InitSystem `3202b9e0` reads `LU` / `CIEL` / `DUNGEON_SPEECH_LUCIEL` (@3202bcf4 / @3202bd56 / @3202bc76).
