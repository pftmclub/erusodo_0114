# June character Lu/Ciel (Stage 8) - Phase CH2d2 - Diablura (39) skills, part 2

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH2d2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2d review, CH2d20 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/CH2d2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH2d2` row of "Phase CH2 - Skills: base and first jobs (200, 37, 38, 39)", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
from an A and a B slot on the body that casts it; the hand-off gives the unit, class, skill id and slot, never slot 4 / 9 before
transcendence.

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

## Amended by CH0 (2026-11-02)

- **Functions: 31 to transcribe** (+ 30 thunks) (`awk -F'\t' '$6=="CH2d2"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b04640` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DEATH_TOUCH_EventProcess `32b048c0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_END_EventProcess `32b04910` LSI_SA_LDB_ALTAR_OF_EVIL_END_EventProcess `32b04d30` LSI_A_LDB_F_ABYSS_STRIKE_EventProcess `32b13360` LSI_SA_LDB_NIGHTMARE_TARGET_FrameMove `32b133d0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_READY_EventProcess `32b13500` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_Z_EventProcess `32b14180` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_JUMP_EventProcess `32b16f50` LSI_SA_LDB_NIGHTMARE_StateStartFuture `32b17090` `32b178b0` LSI_SA_LDB_NIGHTMARE_EventProcess `32b17c20` LSI_SA_LDB_NIGHTMARE_TARGET_EventProcess `32b30ec0` LSI_SA_LDB_NIGHTMARE_TARGET_StateStartFuture `32b31260` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_WAIT_EventProcess `32b31480` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_WALK_EventProcess `32b316a0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_JUMP_LANDING_EventProcess `32b318e0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DEATH_TOUCH_FrameMove `32b358b0` CreateAltarOfEvilProjectile `32b36290` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_EventProcess `32b364e0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_END_EventProcess `32b36750` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DASH_JUMP_LANDING_EventProcess `32b380a0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_READY_StateStart `32b380f0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_COMBO_XXX_READY_StateStart `32b38110` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_DEATH_TOUCH_StateStart `32b38140` LSI_A_LDB_F_PHANTOM_RUSH_FrameMove `32b38340` LSI_A_LDB_M_PHANTOM_RUSH_CameraMove `32b38540` LSI_A_LDB_F_PHANTOM_RUSH_TARGET_StateStartFuture `32b3a2d0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_END_StateEnd `32b3e720` `32b43910` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_START_StateStartFuture `32b43ae0` LSI_SA_LDB_ALTAR_OF_EVIL_TRANSFORM_START_StateStart.
  Still `?` (name it by the class pattern from the hint in col 7): `32b17090` `32b3e720`.
- **States:** the ones whose callbacks are listed above (the state table's col 8 keeps the unsplit `CH2d`).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7y L680 GetLandPosition bMonster true in Luciel class code; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- **Depends on CH2d20** (transform HUD).
