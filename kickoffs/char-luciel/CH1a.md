# June character Lu/Ciel (Stage 8) - Phase CH1a - Class core: Class skeleton

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1a` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1a0 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1a-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1a row: create `X2Lib/X2GULuciel.h` / `.cpp` (ASCII, CRLF; added to `X2Lib_2010.vcxproj` and `.filters` inside the build's
normal item groups), with the class declaration from the class map (every member at June's role, Hungarian names), ctor / dtor, the three own
virtuals, `MemoConditionLuciel` and the `+0x8ac8` class index (InitState's switch: 37 / 38 / 39 / 200 -> 0, 127 -> 1, 128 -> 2, 129 -> 3, by
enum name). The whole file sits inside `#ifdef SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE`. Gate: the build passes; `junediverge.py --check` / `enumscan.py --check` on both files PASS.

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

- **Functions: 21 to transcribe** (+ 4 thunks) (`awk -F'\t' '$6=="CH1a"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae6130` PreloadAuraAccelResources `32aeaa10` `32aeb1e0` UpGageNowMpOfForm `32aeb3f0` MemoConditionLuciel::MemoConditionLuciel `32b0fa00` `32b13d30` ReleaseTransformedSkinAnim `32b13e20` SetFormSkinAnim `32b14860` `32b14db0` FindDamageEffectIndexEntry `32b14f70` `32b1bd90` CX2GULuciel `32b1eb10` DeleteMinorParticle `32b20100` `32b34920` `32b35540` `32b38f30` EndTransformedForm `32b39380` ~CX2GULuciel `32b3acf0` `32b4cb80` `32b4d1c0` `32b4e660` ~CX2GULuciel (scalar deleting dtor).
  Still `?` (name it by the class pattern from the hint in col 7): `32aeaa10` `32b0fa00` `32b14860` `32b14f70` `32b20100` `32b34920` `32b35540` `32b3acf0` `32b4cb80` `32b4d1c0`.
- **Depends on CH1a0** (gauge class, transform link `IsTransformedForm` / `SetTransformedForm` 31fc9d90 / 31fc9d50, switching guide): CH1a calls them by their March names.
- Class-map names for CH1a: ctor `32b1bd90`, dtor `32b39380`, scalar dtor `32b4e660`, own virtuals 0x880 `32b14db0` FindDamageEffectIndexEntry, 0x884 `32b3acf0` `?` (a skill-use check with a `[S%1%]%2%` message, shaped like `CanUseSpecialAttack` 0x7f8: name it), 0x888 `32ae6130` PreloadAuraAccelResources; MemoConditionLuciel ctor `32aeb3f0` (vtable `334f036c`; its IsCondition `32ae5150` is CH4c's).
- **States enum** for `X2GULuciel.h`: 515 registered ids (7-592; 7-130 shared USI, 154-592 the class's); 585 LSI names across both scripts (28 alias lines in Duo); neither June's `Enum.lua` nor the live one has an `LSI_` entry. State ids are 16-bit already (`SERV_IRUHADEV_JUNE_USER_STATE_ID16`, `Always.h:6188`).
- **June ids are not March's USI from 24 up:** June inserts a state at 24 (`USI_DAMAGE_GROGGY` is 25 in June, 24 in March's enum). Name ids from `ch0_reg.tsv`, never by counting March's enum.
- **Register `NUI_SA_LDN_GARDEN_OF_HAURES` = 2269** in `X2UnitManager.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`: the dtor (`@32b394c7`) and ReInit (`@32b37713`, CH1k2) look the NPC up (`ch0_summons.tsv` last row). 1906 / 1908 are already named.
