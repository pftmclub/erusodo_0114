# June character Lu/Ciel (Stage 8) - Phase CH1l - Class core: Overrides, band 2, part 1

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH1l` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1k2 review; one at a time in `X2GULuciel.*`
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | kickoffs/char-luciel/CH1l-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Transcribe every function the class map gives phase `CH1l` (`awk -F'\t' '$6=="CH1l"' june2017_re/char_luciel/ch0_class_map.tsv`;
CH0's amendment names the column), whole, under the plan's "Transcription rules (CH1b-CH1l)". Leave `InitState*` alone (CH1m registers).
Gate: every row written (the Done note lists them with June addresses); build passes; `junediverge.py --check` and `enumscan.py --check` on the
class files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

- **Functions: 32 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1l"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae9bf0` CanPlayEmotion `32ae9c00` GetTransformedSkillStateID `32ae9de0` `32ae9f20` `32aea690` InitEnchantWeaponEffectUnitSpecific `32aeb2c0` SetNowMpFromStoredValue `32b0fa20` `32b12fd0` COMMON_BUFF_FrameMove `32b13b60` GetSkinAnimByForm `32b143e0` `32b14e10` CanRidingState `32b15340` IsClassCommonMoveState `32b154f0` CreateMajorParticleArray `32b15900` GetSkillIconTexture `32b16220` SkillIconOverride `32b18220` UpdateNotRenderingOtherEquipList `32b1c960` `32b34680` CheckBoundingTarget `32b37730` AttackResultByType `32b37780` CreateMinorParticleArray `32b38080` HyperModeFrameMove `32b4d5b0` GetJumpDownStateID `32b4d810` GetDamageDownFrontStateID `32b4d850` GetDamageDownBackStateID `32b4d990` GetDamageAirDownInvincibleStateID `32b4d9d0` GetDamageAirDownLandingStateID `32b4da10` GetDamageAirFallStateID `32b4da90` GetDamageAirFlyFrontStateID `32b4dad0` GetDamageAirFlyBackStateID `32b4db10` GetDamageRevengeStateID `32b4db50` GetDanceWaitStateID `32b4dbe0` IsHyperModeState.
  Still `?` (name it by the class pattern from the hint in col 7): `32ae9de0` `32ae9f20` `32b0fa20` `32b143e0` `32b1c960`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L251 skill-use function 32b1eef0 (vtable +0x800, also slot 512 the reserved-skill hoo; §7 L276 Spec A / B readers 32b14860 (A and B) and 32b37910 (B); §7 L282 vslot 485 (+0x794) override 32b0e670 (CX2GULuciel vtable 334f06fc): skill-group ; §7l L474 virtual 32b34680 (slot 0x568): target check via JuneIsBoundingLandCrash 0x2f4 / ; §7q L557-562 Luciel getter bodies 32b4d810 (0x114), 32b4d850 (0x118), 32b4d990 (0x48c), 32b4d; §7t L602 user vslot 0x51c override 32b4dbe0: state 0x17 or a Luciel state (@32b4dc12) -> ; §7t L611 sound sets: JuneIsHyperModeState also picks the HYPER / BERSERK sound set in Com; §7al L880-888 new CX2GUUser hooks: 0x654, 0x850 (after-image frame move), 0x860, 512, 0x638; §7an L927 GetCharacterSpecializationB readers Lu/Ciel 32b14950 / 32b3794d.
- The DU2 getters (0x114, 0x118, 0x458, 0x48c, 0x490, 0x494) and the rest of the band from 0x49c: names from June's registration ids (`ch0_reg.tsv`), not March's enum.
