# June skill system core - Phase S7k - The unit's socket aggregate and June's HUD max MP / max HP refresh

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7k` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7j (June's `ResetMaxMp` `3234b9d0`, ported as `CX2Unit::ResetMaxMp( base, group )` under `SERV_IRUHADEV_JUNE_SKILL_MAX_MP`) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` (suggested: `_UNIT_SOCKET` for the aggregate, `_HUD_MAXHPMP` for the refresh and its callers) |
| Log tag | `[JUNE_SKILL] HUD` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7k-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7j - Done (2026-10-19, Opus 5.5 high)" "Phase S7g - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7j` and `Skill S7g`, and the Buff Phase 16 rows for `32091080` (SocketData), `32364790`, `3206d770`
(the user-side InitSocketData this mirrors) and `3204f190` (the user-side SumSocketOption, already ported in March as `CX2GUUser::SumSocketOption`).

## What S7j already decoded (verify, do not re-derive)

- **`3235a160` (`__fastcall`, this = CX2Unit).** Guard: `CX2GageManager::GetInstance()` and `g_pData +0x68` (my user) `+0x90` (select unit). Then:
  `+0xe1c` vslot 1( 2 ); save the aggregate's decrypted stat dword [0] (`+0x2f0` KProtectedType: key `+0x2f4`, 13 dwords from `+0x2f8`) to a local; vslot 1( `+0xe40 == 1` ).
  **MP** = `ResetMaxMp( 300, 0 )` x ( 1 + aggregate vslot 0xc( 1 ) + `32358970( 5 )` x 0.01 ) + aggregate vslot 0x10( 1 ) -> `3248a800( uid, mp, 0 )`; unit type
  10 (`+0xaf0`) repeats it with `ResetMaxMp( 300, 1 )`, vslots ( 0 ) and idx 1 (no type 10 in March: Lu/Ciel). Constants: `3344f348` = 300.0f, `334436a0` = 0.01.
  **HP**: a `CX2Stat::Stat` (13 floats, `32e89900` ctor) sums `32358b00` (unit stat, all 13), then only floats [6..12] of `3234e210( tmp, 0, +0xe40 == 1 )` (equip),
  `32890230` on `+0x494` (skill), `328902f0( tmp, byte +0x988 )` on `+0x494` (guild), `326f8080` on `g_pData +0x28c` (title) and the aggregate's own stat;
  HP = sum[6] + min( sum[6] x 0.5 ( `3342ebc8` ), the saved stat[0] ) -> `3248d3a0( uid, hp )`. Raw: `curl .../disassemble_function?address=0x3235a160`.
- **June's `CX2Stat::Stat` order:** [0..5] = March's `ExtraStat` (`m_fIncreaseHPRate`, `m_fIncreaseMPRate`, atk / def rates), [6..10] = `m_fBaseHP`, `m_fAtkPhysic`,
  `m_fAtkMagic`, `m_fDefPhysic`, `m_fDefMagic` (check [9] / [10] and the two June extras [11] / [12] against `326f8080`, which writes [6] [7] [8] [11] [12]).
- **The aggregate** (`32363ca0` @32363d53): a `CX2SocketItem::SocketData` (`new 0x468`, vtable `33478a98`) for every unit but type 10 (`32091730`, 0x488).
  vslot 0xc = `32091320` = stat[1] = `m_fIncreaseMPRate` (ignores its argument), vslot 0x10 = `32091310` = `fild [+0x22c]` = `m_iAddMPValue`. So the HP cap is
  `m_fIncreaseHPRate` (an absolute HP amount after ConvertAndAdd), MP rate / add = the user-side `CX2GUUser::ResetMaxMP` terms.
- **The fill.** CX2Unit vtable `3348ffe4`: slot 1 `32365d30` (equipment sockets + random options + the `32883300` set check, pet `g_pData +0x328`, riding pet
  `321f7130`, title, premium buffs ...: ~690 decompiled lines), slot 2 `323652b0` (two lists, mode 0 / 1 / 2), the per-list sum `32364790` (= the user's
  `3204f190` on unit data, ~630 lines; also called by `32365690`). The mode (0 / 1 / 2) and `3286a4b0` only filter for unit type 10 (`local != 0` only then),
  so in March every mode sums everything.

## Task

Added by S7j (2026-10-19, user split S7j: item 1 in S7j, items 2-4 here). Port whole, in this order; design questions go to the user.

1. **The CX2Unit socket aggregate**: a `CX2SocketItem::SocketData` owned by CX2Unit (alloc / free as `32363ca0` / the dtor), filled by June's slot 1 / slot 2 /
   `32364790` / `32365690` on unit data. Compare each step with March's `CX2GUUser::InitSocketData` / `SumSocketOption` (already June-ported in Buff 16 / C3r / C10d)
   and say where the unit version differs (no game-state gates? `param_3` rate? the `+0xe78` cool-time map?).
2. **The HUD refresh `3235a160`** as decoded above, on March's `CX2GageManager::SetMaxMp / SetMaxHp` (no idx). Compare `32358b00` / `3234e210` / `32890230` /
   `328902f0` / `326f8080` with March's `GetUnitStat` / `GetEqipStat` / `GetSkillStat` / `GetGuildSkillStat` / the title stat, and port June's where it differs.
   `32358970` (ability map) is June-only: 0, as in Buff Phase 16 and S7j.
3. **The 23 callers of `3235a160`** (Ghidra xrefs): `31f1fd90` (0x735), `31f276b0` (class change, S7h), `321a29a0`, `321a30d0`, `3220ea50` (INIT ACK), `3220ee30` x2,
   `322112d0` (RESET ACK), `323b0770` (grant NOT), `32457ef0`, `324631a0`, `32509d30`, `3260c140`, `3260c5d0`, `3260ca30`, `326cc1c0`, `326d0d10` x2, `327032c0`,
   `32703390`, `32dd6ef0`, `32dedb20`, `321ff5d0`. Map each to March and add the call; a caller with no March counterpart is noted, not invented.
4. **The village HUD seed.** March sets the gage maxima only from the emulator's `KGamePlayStatus` (`X2StateServerSelect.cpp:4127` / `:5037`, the studio's own
   refresh commented out at `:4136` / `:5044`; emulator fill `Offline/X2OfflineServer.cpp:1146-1177`). Decide with June whether the select-unit path calls `3235a160`.

**Play-test:** unit 19 Iivue (Code Empress, `RNSI_P_EAT_QUEENS_POTENTIAL` 25405 level 3 = +125 max MP in June's table): in a village the HUD MP reads
"x/425+(equip / socket)" and the guide marks move; reset or relearn the passive and the HUD changes at once; in a dungeon the max MP matches the S7j log line.
