# June buff port - Phase 16 step 3b, part 3 - summon skill damage, item HP recovery, skill-group power sockets

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b3` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b2 (done) and its review |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b3-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Step 1 RE" "Phase 16-step3b2 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b2" carry the decodes this phase starts from.

## Task

16-step3b2 split these off at the item boundary (too large for one session). One commit per item. Port
June's whole function where it differs, cite June addresses, byte-level CP949 edits. Count each reader's
reach on June's `OptionItemData.src.lua` before building, and say it per reader.

0. **Item 4, the MP cost cut** (user decision 2026-10-06: "follow June"). June's `GetActualMPConsume` is slot 423
   `3203cf60( mp, skill, bool )` (RET 0xc); slot 424 `31fd1840( skill, level, bool )` builds the MP and passes the bool
   through, and the classes override 424 (March's class `GetActualMPConsume` overrides). With the bool set June
   multiplies by `1 + 32358970( 0xb ) x 0.01` and cuts the MP by `max( 1 - r, 0.8 )` for the `+0x74dc` result r > 0 on a
   non-hyper skill (`@3203cffb`, `@3203d02d`); Phase 6b left both out for lack of the argument. Port the argument as June
   has it: name `32358970` and slot 471 (`31fdbe20`) and the alternate templet `323511f0` first, then map every June
   caller's bool (the slot refresh `31fd6880` passes 1; `3200f680` and the class sites vary) onto March's callers.
   Retire 16-step3b2's `socket MP cost cut … not applied` line.
1. **Item 3b, the summon helper's socket term** (`322ece70 @322ed2bd`). June's power rate for every summoned
   NPC is `(1 + m_fSkillDamageUpRate [+ m_fAllSkillDamageIncreaseRate unless the summoning skill is type 13])
   x [esp+0xb0] x (3201ad10 + 1) x 31fd7cc0 x 31fea430 x GAA(AT_SPECIAL)`, and the stat scale reads no socket
   field. March (`X2Game.cpp` `SetUserSummonedNPCInfo`, June `3230c1a0`, 21 calls of `322ece70`) instead puts
   `1 + m_fSkillDamageUpRate` into `fStatScale` in five cases and has no all-skill term. Map June's 21 call
   sites to March's cases (Phase 6b did the same mapping for `GetSummonPowerRateByBuff`; reuse it), then move
   the socket term to the power rate for each. Decide first what `[esp+0xb0]` is per case.
2. **Item 5, HP recovery** (`322fbc60` = `CX2Game::UseItemSpecialAbility`, `@322fbd4f`). June's rates:
   `[esp+0x60]` = 1.0, or the guild skill `0x1f7` ability `0x37` (Master of Alchemist); `[esp+0x64]` =
   the user's `m_fHPRecoveryRate` result - 1.0 (`+0x6ff0`). The HP-up cases use `value x ([64] + [60])`
   (`@322fbdf3`, `@322fc258`, `@322fd47a`); the percent case uses `value / 100 x (skill ? [60] : 1) x ([64] + 1)`
   (`@322fd965`). March (`X2Game.cpp` `UseItemSpecialAbility`) multiplies by `fIncreaseRateFromMasterOfAlchemistSkill`
   and adds Chung's metabolism boost. Map June's SAT switch (`322ff65c`/`322ff6d0`, 0x2a cases) to March's,
   then port each case's rate as June has it. JUNE_BUGS #46's NPC 2.0 is unreachable here (the user is `esi`).
3. **Item 9, the rest of the loader keys.**
   - `m_fSkillAdditionalPowerRate` with `m_eSkillIDForAdditionalPowerRate = "Group N"` and the `m_SkillType` /
     `m_PowerRate` table: **2,491 June sockets** (accessories, e.g. ID 90000 "Group 1001" 0.1). Not loaded yet.
     Find where the loader stores them (the key strings are built at `@3272db9e`, `@3272e298`, `@3272e85d`;
     find the `GetValue` calls and the stores). Step 1's table says `+0xb8`, but no `GetSocketData` caller and no
     `+0x87b4` site reads templet/aggregate `+0xb8`, so the reader is elsewhere: look for a per-skill map the
     group string fills, and its reader on the skill power rate path.
   - `m_fAllStatRelLV` (30 sockets): find its destination (the `+0x254..+0x268` RelLV block?) and reader.
   - Settled by 16-step3b2, no work: `m_bSocketExtraction` / `m_iSocketApply` / `m_iSocketCondition` (only
     June's item socket-extraction system reads them, `32883300`/`32883450`); the first `m_sBuffLevel` (`+0x296`,
     no reader found).

A reader that cannot be settled from the disassembly stays dormant with one `[JUNE_BUFF]` line and is listed
as open. Item 10 (the aura's socket side) is `16-step3b4`.
