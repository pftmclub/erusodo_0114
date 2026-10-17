# Atlas/Elysion port - Phase E7 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E7 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E7 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_ELYSION_SCRIPT_B` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E7 — Elysion-half bindings + additional world" "Phase E7 — Done"
grep -a "Atlas E7" june2017_re/funcmap.tsv
```

Use the ghidra skill (read-only); `june2017_re/funcdis.py` / `disasm_range.py` for addresses Ghidra has not carved.
Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E7's commit against June, body by body (each C++ comment names the June address). Opus re-reads in raw disassembly; delegate the pulls.
- **Additional world:** `AddWorldData_LUA` 32805770 @32805f81-32805fee (key order, index loop), `32803f60`'s additional half (@32804248-328042a6:
  ctor args, OpenScriptFile second arg, 327fbeb0, then SetShowObject / PlayEffectBGM / Play3DEffectBGM( 0 )), the three callers (327cd1b0,
  32d52fa0, 32d54ae0 incl. the clear first), Release 323080c0 @323083ff, frame move @32309b77, render 322d7260, reset 322d7320 / lost 322d73b0
  (the +0x1ac gate on all four), `SetShowWorld` 322cc040, `PlayWorldAllSound` 322cc060, the two index forms, `StopBGM` 327fdb80's name clear.
- **CX2Game queries:** 322dd990 (filter, the erase loop), 322d0030, 322dc3c0 (signed `%`), 322cfc00 (index with empty slots), and the nearest core
  322d55b0: argument order from the wrapper pushes (322d7060 / 322d7090), best = max range at entry, the team / ally-team fallback, the NPC class
  filter, the raid-object skip left out (no March object), the front test (32293b80 vs vslot 0x170), the three range compares.
- **Fades:** 31ce6270 / 31ce62c0 and the draw 31ce94a0 (colour alpha overwritten); plain fades now black ( 322cff70 ); the white-on-black-texture
  question (FadeInOut.dds is a black DXT1, colour op unset in the texture draw states): check how June draws a white fade.
- **Units:** AddEffectSetDeleteCondition_LUA 3227a670, thorns 32278150 + 322780b0 + the HP-text queue ( 3229f11f-3229f4e6: 0.2 s, > 1 green,
  < 0 red, the second queue's cl gate not ported), call sites @3204a383 / @327b4c9b, the simple timer ( 32299890 / 3226e3c0 / 322963a0 /
  32299af0 / 32296400 / 32296520 ), state cool time 3275eb70 / 3275ebc0, stored NPC trio, VectorUidType ( dead slot test, PopValue -1 / 0 ),
  small gage ( 32e5d580 = DrawHpGage? confirm against March X2GageUI.cpp ), sync dummy ( host only, packet 0x1d not ported ), StartAlphaFade
  3275a240 / 3275a320 ( the fade-end branch by the rate's sign ), CAN_PASS_UNIT_AT_FIRST_FRAME, SHOW_GAGE / INVINCIBLE_ATTACK_TYPE ( no reader ),
  HEIGHT_CHECK_RANGE ( 32f06860 @32f073c1-32f07410 ), IsSceneEnded and its clears, SetParticleSize, BlackHole accessors, StopEffectSetSilent
  ( 321db020 -> 321e3ee0 -> 321e3a80 -> 321e2a30's bSilently; March stops at once ).
- **JUNE_BUGS** rows added by E7 are right; the census `--phase E7 --region elysion` (with flag probe) shows no E7-owned name or key open.
