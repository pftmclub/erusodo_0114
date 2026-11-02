# June character Lu/Ciel (Stage 8) - Phase DU2 - Damage-state getter virtuals

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel DU2` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU1 review |
| Flag | `SERV_IRUHADEV_JUNE_USER_DAMAGE_STATE_GETTERS` |
| Log tag | none |
| Review | kickoffs/char-luciel/DU2-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase DU - The two-body engine (DU1-DU5)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the DU2 row of "Phase DU": the five `CX2GUUser` virtuals with base bodies returning March's constants, and the two readers (the
`PhysicProcess_Jumping` test under `SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE`, StateChange's force-down flush `320348c0 @32034931-32034994`) calling
them. Check every class's vtable consumer: no March class overrides a slot it should not. Gate: probes on and off; an existing character jumps,
falls and is knocked down as before.

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

NU0 (2026-11-02), `nu0_duo_engine.tsv` rows `DU2`. Base targets (vtable `334781f4`): 0x114 `3208de60` returns 30 (`USI_DAMAGE_DOWN_FRONT`),
0x118 `3208de70` 31 (`_DOWN_BACK`), 0x490 `330de330` 37 (`_AIR_DOWN_LANDING`), 0x494 `3208dec0` 38 (`_AIR_FALL`), 0x458 `3208de10`
(`USI_RIDING_JUMP_DOWN` when vslot 0x200 == 1, else `USI_JUMP_DOWN`). **There are more readers than the two the plan names:**
- `PhysicProcess_Jumping` `31fd6ce0` (all five, two passes) and its landing-line helper `31fd7350` (0x118 / 0x494 / 0x490 / 0x458; not in funcmap);
- StateChange `320348c0` (0x114 @32034943, 0x118 @32034960, 0x490 @3203497d; **no 0x494** there);
- `CommonFrameMove` `32063650` (@32067680-32067726), `CommonEventProcess` `31fe7610` (@31fe7944, @31fe7992), `ForceDownAndDetonationProcess`
  `3202a9c0` (@3202a9e7, @3202aa1e);
- the state-id predicates vslot 0x530 `3208e7d0`, 0x534 `3208e8c0` (all four), 0x538 `3208eb60`; `32296570`; DamageEffect `31f6eea0`; Aisha's
  InitState `32cea0c0`.
Port the getters, then route every reader that has a March counterpart (map each in funcmap first; a reader with no March body is recorded).
