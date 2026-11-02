# June character Lu/Ciel (Stage 8) - Phase CH1a0 - Engine pieces the class core calls first: the Lu/Ciel gauge class, the transform link, the hold-key selector, the switching guide (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1a0` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH0, NU1 review, DU1 review; one at a time in `X2GUUser.*` |
| Flag | the owning shared flag (`SERV_IRUHADEV_JUNE_CHAR_*` for `CX2GUUser` / engine code, class-agnostic) or `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | kickoffs/char-luciel/CH1a0-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Phase CH0 - Lu/Ciel class map (read-only)" "CH0 - Done (2026-11-02)"
awk -F'\t' '$6=="CH1a0"' june2017_re/char_luciel/ch0_unmapped_identified.tsv
```

## Task

CH0 found callees outside Lu/Ciel's class range that no doc maps and that a Sonnet phase would stop on ("this needs Opus"). Map each in Ghidra, name it, give it a March home (an existing March function when June's namesake does the same; else a new one under the flag, by the owning class's pattern, no `June` in the name; class-agnostic code stays in `CX2GUUser` / the engine), and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). A funcmap row per function (`Char Luciel CH1a0`) so the class phases call it by its March name. Check first whether Chung's, Eve's or Aisha's port already holds it (`funcmap.tsv`, `junediverge.py --list`).

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address below has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `junediverge.py --check` and `enumscan.py --check` PASS.

No hardcoded enum literals and no `June` prefix (CLAUDE.md rules): use the member, register June's name and value under a flag if C++ lacks it; names say what the thing is, never an address or phase tag.

## Amended by CH0 (2026-11-02)

- **Callees to map and port** (`ch0_unmapped_identified.tsv` col 6):
  - `31fc9d50` CX2GUUser flag-pair setter ( mode 0 -> +0x8f6e, 1 -> +0x8f6f, 2 -> both ) (confirmed; callers' phases CH1a CH4a)
  - `31fc9d90` CX2GUUser flag-pair getter ( +0x8f6e / +0x8f6f ): 0 = first, 1 = second, 2 = either, 3 = both-or-second (confirmed; callers' phases CH1a CH1k CH1k2 CH1l CH1l2 CH1l3 CH3c CH4a)
  - `322e6300` CX2Game: Lu/Ciel switching-guide UI ( Static_LucielSwitchingGuide / g_pStatic_LucielCooltime / Button_V_Key_ToolTip ) show with the V key string (confirmed; callers' phases CH1a CH1l2)
  - `32a327a0` Lu/Ciel gauge data: KProtectedType<int> setter with clamp ( value cell +0x114 ) (candidate; callers' phases CH1a CH1k CH1k2 CH1l2)
  - `31fc9f00` CX2GUUser: apply an add ( mode 1 ) or multiply cool-time to the three linked slots ( +0x8f70 table floats at +8 / +0xc / +0x10 ) (candidate; callers' phases CH1k)
  - `32498e40` Lu/Ciel gauge data class ctor ( 0x130-byte CX2GageData subclass, vtable 3349b2a4 ) (candidate; callers' phases CH1k2)
  - `32a38250` Lu/Ciel gauge data class copy ctor ( base copy 322b24f0, protected cells copied by value ) (candidate; callers' phases CH1k2)
  - `31fc8900` CX2GUUser: hold-key selector ( this+0x8b58 index 0..9 -> byte of the input struct at +0x86da ); no March counterpart (candidate; callers' phases CH2b CH2c CH2c2 CH2d2 CH3a)
  - `31fd0490` CX2GUUser: start the linked skill: MP cost ( vslot 0x6a0 ) and cool-time ( vslot 0x6a4 ) of the slot skill via 31fc9e80, vslot 0xf0( -MP ), clears +0x (candidate; callers' phases CH2d CH2d2 CH3c CH3c2)
  - `32030160` CX2GUUser: linked-slot key press check; prints the "not ready" info text ( strings 0x9f5 / 0xe2 ) through InfoTextManager::PushText 322db770 (candidate; callers' phases CH2d CH2d2 CH3c CH3c2)
  - `32039dc0` CX2GUUser: StateChange( 320348c0 ) into the state of the linked skill ( skill id 31fdc680( +0x8f70 +0x14 ), state by vslot 0x57c ) (candidate; callers' phases CH2d CH2d2 CH3c CH3c2)
  - `31fd0440` CX2GUUser: key-pressed flag of the current skill-slot row ( +0x8713 when the skill-slot swap is on, else +0x86f3 ) (candidate; callers' phases CH2d2 CH3c2)
