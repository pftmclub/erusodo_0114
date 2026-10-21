# June character Aisha (Stage 5 pilot) - Phase CH3c - Summon cases of SetUserSummonedNPCInfo

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3c` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH3b (its Distortion case `June_SetDistortionSummonInfo` is the pattern); not beside CH4 (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH3c-review.md` (the phase writes it when done) |

## Why this phase exists

CH3b found that `CX2Game::SetUserSummonedNPCInfo` (`X2Lib/X2Game.cpp`, June `3230c1a0`) reads Aisha's summoning skills by **March** ids
(`SI_A_ADM_HELL_DROP`, `SI_A_AEM_STONE_WALL`, ...). Her skill tree holds June ids, so those lookups return NULL: the summon gets no power
rate from the skill and no life time. CH3b fixed the Distortion case (June case `0x443`); the same gap is left in Aisha's other cases. The
checklist row for `3230c1a0` ("port each class's summon cases with the class") is Aisha's subject, so it is a phase here.

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH3b - Done"
grep -a "3230c1a0 case 0x443" june2017_re/funcmap.tsv
grep -a "495\|1575\|STONE_WALL\|^1[0-9]*	NUI_.*AISHA" june2017_re/summon_npc_skill_map.tsv
```
June side: the `FUN_3230c1a0` decompile (`curl .../decompile_function?address=0x3230c1a0`), cases for Aisha's NPC ids (Hell Drop UFO `0x1ef`
and memo UFO `0x627`, Stone Wall, the Petit Angkor bats); March side: `X2Lib/X2Game.cpp` lines of `NUI_UFO_AISHA`, `NUI_UFO_AISHA_DROPPER_MEMO`,
`NUI_STONE_WALL_AISHA` and the commented bat case.

## Task

For each Aisha summon case of June `3230c1a0` (read it first: flags passed to `322ece70`, the skill id from `31fcf520`, the follow-ups such
as `32c86340` / `323115e0`): give the March case June's skill (templet by June id with the upgrade swap, level `GetSkillLevel( id, false, 0, true )`,
power rate and life time as June), as CH3b did for Distortion. Keep March's `SetNPCStat` line. Anything outside Aisha's cases is not this phase.
Done gate: Hell Drop's UFO, Stone Wall and the bats get their power rate and their end from the June skill; a log line per case.
