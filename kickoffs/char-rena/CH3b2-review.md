# June character Rena (Stage 7) - Phase CH3b2 review ( engine: only-target hits on NPCs; core-list data: Gungnir's targeting NPC )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3b2 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3b2 done (commit `1cb6fa14`, the user's play-test + "check logs" recorded in its Done note) |
| Flag | `SERV_IRUHADEV_JUNE_ONLY_TARGET_ATTACK_NPC` (no debug define of its own) |
| Log tag | `JUNE_FX CE_HIT` ( `--grep "JUNE_FX CE_HIT"` ), `[JUNE_CHAR]` ( `SUMMON npc 2083`, `RENA SKILL wind blast lock on` ) |
| Why its own review | review cadence (user, 2026-11-10): `X2DamageManager.*`, `Enum*` and core-list Lua keep their own review; this does not fold into the CH3b review |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH3b2 - Done (2026-11-10)"
grep -n "Char Rena CH3b2" june2017_re/funcmap.tsv
```
Plus `MODS.md` "Stage 7 Rena, CH3b2".

## Task

Review CH3b2 against June:
- **`X2DamageManager.cpp` `EffectToUnit`** under the flag, against June `3262b6e0` @3262ba99-3262bb4f (Ghidra read-only): the `+0x9a0` gate, the type switch on `+0x264` ( 0 user: vslot 0x54 against `+0x938/+0x93c`; 1 NPC: `+0x270` against `+0x940`; other types skipped ), its place after the `ONLY_RELATIONSHIP` test, March's block intact in `#else`.
- **The stand-in** ( @3262baca-3262bb22, the boss-raid follow-target retarget behind `32481fa0` ): confirm `32481fa0` is false in this build ( the field ids it tests ) and that `32757490` = March's `m_sFollowTargetUnitData` layout as the funcmap row claims.
- **Every reader** of `m_bIsOnlyTargetAttack` ( grep `X2Lib` ): which damage effects set `ONLY_TARGET_ATTACK` ( June `DamageEffect.lua` ), and whether any PvP / user-only effect changes behaviour ( it should not: the user branch is March's compare ).
- **Data.** `data036 Enum.lua` `NUI_RENA_TARGETING_GUNGNIR = 2083` = June `Enum.src.lua:3402`; the `char-rena CH3b2` block at the end of `data037 NPCTemplet.lua` = June `NPCTemplet.src.lua:18660` byte for byte in content; `luasrc.py ledger --check`, `core --check`, `luabind.py check Rena_Targeting_Gungnir`. Other C++-only NUI ids a Rena skill spawns that still lack an `NPCTemplet.lua` row ( grep `CreateNPCReq( CX2UnitManager::NUI_` in `X2GULire_ElvenRanger.cpp` against the file ): any miss is a finding.
- **Play-test.** The CH3b2 Done note's play-test lines: `CE_HIT DamageEffect_SA_RSR_WindBlast_Arrow` + `..._Explosion`, `SUMMON npc 2083`, `ES_CREATE EffectSet_SA_RGA_Gungnir_Loop`.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_ONLY_TARGET_ATTACK_NPC --range 1cb6fa14^..1cb6fa14`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2DamageManager.cpp`, a one-file `/Zs` probe of the cpp ( no play-test of its own unless a fix lands: then full build ).
Fix clear errors in place (bytepatch only). Write "CH3b2 - Review" in the plan doc, update the STATUS row, and check the next kickoff (`CH3b-review.md`).
