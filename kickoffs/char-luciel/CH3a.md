# June character Lu/Ciel (Stage 8) - Phase CH3a - Dread Lord (127) skills

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-luciel CH3a` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH2d2 review, CH3a0 review
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_SKILL` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL SKILL` / `NUM`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | none of its own (user, 2026-11-10): the CH3b review covers CH3a, CH3b; do not write `CH3a-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence" "Phase CH1 - Deploy unit: the class core"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the `CH3a` row of "Phase CH3 - Skills: second jobs (127, 128, 129), hyper actives, transcendence", from `ch0_state_skill.tsv` / `ch0_class_map.tsv` (CH0's amendment names the filters). Each skill is tested
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

- **Functions: 26 to transcribe** (+ 25 thunks) (`awk -F'\t' '$6=="CH3a"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32ae6cf0` LSI_SA_LLC_OBLIVION_Init `32ae7b50` LSI_SA_LLD_DANCING_BLADE_Init `32ae7bc0` LSI_SA_LLD_DANCING_BLADE_StateStartFuture `32ae7bf0` LSI_SA_LLD_DANCING_BLADE_LOOP_StateStartFuture `32ae7c10` LSI_SA_LLD_DANCING_BLADE_StateEndFuture `32ae7c40` LSI_SA_LLD_PHANTOM_BLADE_Init `32ae7c90` LSI_SA_LLD_PHANTOM_BLADE_EventProcess `32ae7d20` LSI_SA_LLD_SUMMON_DARKNESS_Init `32ae7e30` LSI_SA_LLD_SUPREME_PUNISHMENT_Init `32ae81b0` LSI_A_LLD_F_FEAR_ABSORPTION_Init `32ae8240` LSI_A_LLD_F_CHAINS_OF_BONDAGE_Init `32ae8360` LSI_A_LLD_F_CHAINS_OF_BONDAGE_StateStartFuture `32ae9f40` LSI_A_LLD_BLUFF_LU_Init `32aea120` LSI_SA_LLD_FEAR_TOMORROW_Init `32aea1b0` LSI_SA_LLD_DEAD_CROSS_Init `32aea5b0` LSI_SA_LLD_DEMONIC_CANNON_Init `32b000f0` LSI_SA_LLC_OBLIVION_EventProcess `32b018d0` LSI_SA_LLD_SUPREME_PUNISHMENT_EventProcess `32b021a0` LSI_A_LLD_F_CHAINS_OF_BONDAGE_EventProcess `32b1fc90` `32b20260` `32b2b9c0` LSI_SA_LLC_OBLIVION_FrameMove `32b2f1c0` LSI_SA_LLD_PHANTOM_BLADE_ATTACK_StateStartFuture `32b2f3f0` LSI_SA_LLD_SUMMON_DARKNESS_FrameMove `32b30970` LSI_A_LLD_F_CHAINS_OF_BONDAGE_FrameMove `32b3c3b0` LSI_SA_LLD_DANCING_BLADE_EventProcess.
  Still `?` (name it by the class pattern from the hint in col 7): `32b1fc90` `32b20260`.
- **States: 19** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_luciel/ch0_state_skill.tsv`): `LSI_SA_LLC_OBLIVION`, `LSI_SA_LLD_DANCING_BLADE`, `LSI_SA_LLD_DANCING_BLADE_LOOP`, `LSI_SA_LLD_PHANTOM_BLADE`, `LSI_SA_LLD_PHANTOM_BLADE_ATTACK`, `LSI_SA_LLD_PHANTOM_BLADE_FAIL`, `LSI_SA_LLD_SUMMON_DARKNESS`, `LSI_SA_LLD_SUPREME_PUNISHMENT`, `LSI_A_LLD_F_FEAR_ABSORPTION`, `LSI_A_LLD_M_FEAR_ABSORPTION`, `LSI_A_LLD_F_CHAINS_OF_BONDAGE`, `LSI_A_LLD_F_CHAINS_OF_BONDAGE_DASH_ATTACK`, `LSI_A_LLD_M_CHAINS_OF_BONDAGE`, `LSI_A_LLD_M_CHAINS_OF_BONDAGE_DASH_ATTACK`, `LSI_A_LLD_BLUFF_LU`, `LSI_A_LLD_BLUFF_CIEL`, `LSI_SA_LLD_FEAR_TOMORROW`, `LSI_SA_LLD_DEAD_CROSS`, `LSI_SA_LLD_DEMONIC_CANNON`.
- **Skills: 19** (part 2 of `ch0_state_skill.tsv`, col 6): 10018 SI_SA_LLD_DANCING_BLADE, 30012 RNSI_SA_LLD_SUPREME_PUNISHMENT, 30013 RNSI_SA_LLD_SUMMON_DARKNESS, 30014 RNSI_SA_LLD_OBLIVION, 30015 RNSI_SA_LLD_PHANTOM_BLADE, 30035 RNSI_SA_LLD_PHANTOM_BLADE_ADD_TRANS, 30055 RNSI_SA_LLD_FEAR_TOMORROW, 30056 RNSI_SA_LLD_DEAD_CROSS, 30064 RNSI_SA_LLD_SUPREME_PUNISHMENT_UPGRADE, 30065 RNSI_SA_LLD_SUMMON_DARKNESS_UPGRADE, 30066 RNSI_SA_LLD_OBLIVION_UPGRADE, 30067 RNSI_SA_LLD_PHANTOM_BLADE_UPGRADE, 30082 RNSI_SA_LLD_PHANTOM_BLADE_ADD_TRANS_UPGRADE, 30083 RNSI_SA_LLD_DEMONIC_CANNON, 30204 RNSI_A_LLD_FEAR_ABSORPTION, 30206 RNSI_A_LLD_CHAINS_OF_BONDAGE, 30304 RNSI_A_LLD_BLUFF, 30308 RNSI_A_LLD_FEAR_ABSORPTION_UPGRADE, 30309 RNSI_A_LLD_CHAINS_OF_BONDAGE_UPGRADE.
- Tree rows: 16 (part 3, col 10).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L54 numerical-only DAMAGE_DATA table LSI_SA_LLD_DANCING_BLADE (Luciel_Duo.lua L18637; §1 L54 numerical-only DAMAGE_DATA table LSI_SA_LLD_DANCING_BLADE_LOOP (Luciel_Duo.lua L; §1 L54 numerical-only DAMAGE_DATA table LSI_SA_LLD_PHANTOM_BLADE (Luciel_Duo.lua L18959; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_F_FEAR_ABSORPTION (Luciel_Duo.lua L19; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_M_FEAR_ABSORPTION (Luciel_Duo.lua L19; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_F_CHAINS_OF_BONDAGE_DASH_ATTACK (Luci; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_M_CHAINS_OF_BONDAGE_DASH_ATTACK (Luci; §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_BLUFF_LU (Luciel_Duo.lua L22109); §1 L54 numerical-only DAMAGE_DATA table LSI_A_LLD_BLUFF_CIEL (Luciel_Duo.lua L22160); §1 L37-105 blocks that index past their skill data need a JUNE_BUGS row; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §2 L126 DAMAGE_RATE_TO_SUPERAMOR x2; §4 L178-181 June-only SKILL_ID members fail silently (52 SI_ 10000-10699 + 143 RNSI_ 30001-3; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry; §7y L680 GetLandPosition bMonster true in Luciel class code; §7ae L713-720 June skill-tree ids with no March class state (reform ids 21000+, RNSI_*); §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi; §7ai L835-841 pass June skip / adjust / level / creator at every ported BFI call.
- **Depends on CH3a0** (summon NPC rows). Prefix `LLD` = 127. Summon Darkness spawns 1906 from helper `32b20260` (`@32b20486`).
