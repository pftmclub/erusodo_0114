# June character Aisha (Stage 5 pilot) - Phase CH1k - Base callbacks and the SetEquipped head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1k` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1j2 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (anything that changes what runs today goes behind `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`) |
| Log tag | none (inert until the final CH1) |
| Review | `kickoffs/pilot-char-revamp/CH1k-review.md` (the phase writes it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1j - Done (2026-10-21, Opus)"
grep -a "Char CH1j" june2017_re/funcmap.tsv | grep ROUTED
awk -F'\t' '$6=="CH1" && $2=="helper"' june2017_re/char_arme/ch0_class_map.tsv
```
Then `kickoffs/pilot-char-revamp/CH1.md` item 2, its "From the CH1i review" notes, and checklist 7i (`docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7i. Skill-slot re-equip snapshot call (from June effect completion C9h-6)"`).

## Task

1. **The six June CX2GUUser base callbacks CH1j left on stand-ins**, ported whole as CX2GUUser `June*` members, then registered in
   `June_InitState` / `June_InitStateCommon` in place of the stand-ins:
   - `31fcbed0` / `31fcbf80`: damage small / big EventProcess. They call `SpecialAttackEventProcess( 7 )`, a use condition March's
     `ACTIVE_SKILL_USE_CONDITION` lacks (5 is its last). Find June's meaning of 7 in June's `CheckSkillUseCondition` and the class lookup
     (`32cebe80`, `June_SpecialAttackLookup`), and add what March needs.
   - `31ffbd70` (START StateStart), `3201b390` (START EventProcess), `320004b0` (WIN StateStart), `320007e0` (LOSE StateStart). These are June's rewrites
     of March's `StartEventProcess` / `WinStateStart` / `LoseStateStart`: the pet reactions, `+0x8625`, vslot `0x11c`, and the "Clear" particles in dungeon `0xa1fe`.
   These are engine pieces every June class registers. Port them once, for Stage 7 to reuse.
2. **`SetEquippedSkillFuncBySkillSlotIndex` head** (`32cede00`, `CH1.md` item 2) and the CH1 class-map helpers it calls (`32cb7ce0`, `32cbeee0`,
   `32cbef40`, `32cbfb70`, `32cc10f0`, `32cc2110`, `32cc5990`, `32cc6fb0`, `32cc7200`; `32cf1b10` and `32cc74a0` .. `32cc79b0`). Skill cases stay
   March's until CH2a-CH3b. Check each `#ifdef` branch with `cl /P` (`UPGRADE_SKILL_SYSTEM_2013` pairs).
