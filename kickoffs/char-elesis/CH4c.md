# June character Elesis (Stage 7) - Phase CH4c - Dark Knight / Crimson Avenger blood hooks (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH4c` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH4b. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_BLOOD` (+ `..._BLOOD_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS BLOOD`) |
| Review | `kickoffs/char-elesis/CH4c-review.md` (Opus 5.5 high; the phase writes it when done); the CH4 stage review: covers CH4a-CH4c, retires `_PASSIVE_DEBUG` and `_BLOOD_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
cat june2017_re/char_elesis/ch0_blood.tsv
```

## Task

Do CH4c from `ch0_blood.tsv`: the three hit hooks 0x4f8 `32b7beb0`, 0x4fc `32b898a0` and 0x734 `32b7bad0` (blood sucking heals with
`DisplayAddHPInformation`, bleeding applies `EffectSet_B_EL_SDC_Bleeding`), **with their base virtuals and every base call site from June** (the
base is the shared stub `33093ad0`; checklist l.454-455 and `Always.h:6204` say the hooks are inert until her port; members `+0x9748` / `+0x974c`
named from their readers); Blood Runaway `32b87880` (BFI 462 registered with this code under `SERV_IRUHADEV_JUNE_ENUM_NAMES`, BTI 426 erased above 4
stacks; `plan_checklist_rows.md` item 13) and its passive erase `32b734d0`; `32b7e3b0`; the Madness chain `32b6ff20` (HP ratio reads); the
transcendence berserker chain `32b7e9a0` (BFI 913 `TRANSCENDENCE_1_ON_ALL` passing `x`: 7ai); the per-frame list 0x730 `32b816a0` with `32b81220`
over the vector at `+0x9700` (32-byte elements). Remove the `_BLOOD` stand-in guards CH1j / CH2d / CH3c / CH4a left. No HP cost was found by callee
name in planning: if June's skills spend HP, it is in these bodies; say so in the Done note either way. Play-test on the 35 and 125 units: blood
sucking heals, bleeding ticks, Blood Runaway stacks and ends, Madness switches with HP. Then the CH4 stage review covers CH4a-CH4c. You are on
Opus: delegate the reads, probes and builds.

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

- **Functions:** `awk -F'\t' '$7=="CH4c"' june2017_re/char_elesis/ch0_class_map.tsv` = 11 (0 listing-only: col 9 `n`; 1 `inside`, 10 `new`: col 5; proposed names col 6, `?` = name by the class pattern from the body).
- **Checklist rows** (`june2017_re/char_elesis/ch0_checklist_rows.tsv` col 5): L245, L278, L457, L458, L929.
- **`ch0_wos.tsv` rows:** 2 (filter on its `proposed_phase` column).
- **`ch0_blood.tsv` rows:** 19 (filter on its `proposed_phase` column).
- `june2017_re/char_elesis/ch0_blood.tsv` (30 rows): 0x4f8 `OnAttackerDamageReact_Slot4F8` = Dark Force HP intake in Vigor (getter 0x724), 0x4fc `_Slot4FC` = Judgment bleeding heal (BFI 465, BTI 99, chance `+0x974c`), 0x734 `OnDefenderDamageReact` = Blood Revenge (BFI 464; `+0x9740` / `+0x9744`; **add the two base virtuals and their calls**: the June user DamageReact `320462a0` calls 0x730 @`320499f7` and 0x734 @`32049a0c`; March has neither, while 0x4f8 / 0x4fc are already called, `X2GUUser.cpp:11355`), Blood Runaway `32b87880`, Madness `32b6ff20`, transcendence berserker `32b7e9a0`, the Shadow Edge list (0x730 `32b816a0` discards its lookup: June no-op kept verbatim; `32b81220` / `32b81300` / `32b813d0`). No split (10 functions + the base hooks).
