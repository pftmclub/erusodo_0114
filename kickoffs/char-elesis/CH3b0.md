# June character Elesis (Stage 7) - Phase CH3b0 - Summons and vslots 0x24c / 0x250 (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH3b0` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH3a. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SUMMON` (+ `..._SUMMON_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS SUMMON`) |
| Review | `kickoffs/char-elesis/CH3b0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH3 - Skills: second jobs (120, 121, 125), summons, hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH2d0 - Done (2026-11-10)" "CH2d0 - Review (2026-11-10)"   # the last summon-spawn phase
cat june2017_re/char_elesis/ch0_summon.tsv
```

## Task

Do CH3b0 from `ch0_summon.tsv`: June's four summons and the two vslots that hold the first of them.
- **Vslots 0x24c `32b89260` (`SetSpecificValueByBuffTempletID`) and 0x250 `32b74500` (`UnSetSpecificValueByBuffTempletID`) whole**, inside March's
  overrides if she has them, else new: the two writers of `m_fCreateEffectSetRate` (June `+0x8e50`, `@32b8930d` / `@32b7471b`; the member and its
  reader are char-elsword CH4a2's, flag `SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET`: widen, never copy) and the Fire Blossom spawn `@32b746e8`
  through June's `CX2Game::CreateNPCReq` forwarder `31f8b010` (my unit only).
- **Fire Blossom** `32b79db0` (`@32b7a058`, `NUI_ELESIS_FIRE_BLOSSOM` 1567) against March's spawn (`X2GUEL.cpp:3667`): June's body inside March's.
- **Wall of Prominence** `32b7a0a0`: `NUI_SI_SA_SBH_WALL_OF_PROMINENCE` 1635 (`@32b7a4f5`) and `_FIREKING` 1971 (`@32b7a442`).
- **Blood Cutter**: the spawn call `@32b8b3b7` of `NUI_SI_SA_SCA_BLOOD_CUTTER` 1641 as a helper CH3c's state body calls.
- `CheckSummonSpecialAttack` `3202daf0` in `CanUseSpecialAttack` (CH1k left the call under `_SUMMON`); the related-skill-level write (`+0x8e74`)
  `ELESIS_FIRE_BLOSSOM.lua`'s `GetRelatedSkillLevel` reads (the pilot omitted it: `plan_checklist_rows.md` item 15).
- The three NPC ids registered by June's names in `X2UnitManager.h` under `SERV_IRUHADEV_JUNE_ENUM_NAMES`; `python tools/luabind.py check` on the
  four NPC scripts (every unbound method bound whole from June or listed in `lua_unbound_known.tsv`); their `NPCTemplet` rows and meshes present
  (CH0b staged them; `npc_mesh_census.py`).
Spawn rules (count limit, owner link, position, lifetime, what despawns them) are read from June, never taken from March's Fire Blossom. Funcmap
rows `Char Elesis CH3b0`. Probe compile; full build only if this phase hands off a play-test (CH3b's and CH3c's play-tests exercise the spawns).

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
