# June buff port - Phase 16 step 3b, part 5 - skill-group power sockets and m_fAllStatRelLV

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b5` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16-step3b3 (done) and 16-step3b4 (both edit X2GUUser.cpp and X2SocketItem.cpp/.h; never in parallel with either) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b5-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Step 1 RE" "Phase 16-step3b3 -- Done" "Phase 16-step3b4 -- Review"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b3" for `32726850 @3272e4b5..3272eb3e`, `3201ad10`, `32727838`
and `32358970` carry the decode this phase starts from.

## Task

Item 9's rest, split off by 16-step3b3 (located and decoded, not ported). Port June's whole function where it
differs, cite June addresses, byte-level CP949 edits. Count each reader's reach on June's `OptionItemData.src.lua`
(`m_fSkillAdditionalPowerRate` 2,491 entries, `m_fAllStatRelLV` 30) and on the save's 13 sockets.

0. **First: June's `GetBasicReducedDamagePercent` (`3201aff0`)** (user, 2026-10-06, from the 16-step3b4 review). June keys
   each reduction socket with `m_fIntervalTime` > 0 on the per-socket cool-time store `+0x8b98` (inline: find `322aa950 @3201b0ab`,
   `operator[] 3207a5e0 @3201b10b`, interval `+0x1f8`); March's `CX2GUUser::GetBasicReducedDamagePercent` still uses the single
   `m_fSocketOptionCoolTime`. Port the whole June function (also `32000320`, the set options' sum, and `322cc1c0`, called
   `@3201b026`, not decoded yet) onto 3b4's `GetJuneSocketCoolTime` / `SetJuneSocketCoolTime`. If nothing else then reads `m_fSocketOptionCoolTime`,
   say so (its tick `CheckSocketOptionCoolTime` stays in `#else`).
1. **The group table.** `32715680` looks "Group N" up in the `CX2SocketItem` manager's map `+0x40` (find `32076c90`,
   values = a list of skill IDs). Find who fills that map (its inserter, the Lua binder or loader that feeds it, and the
   June data file with the groups) before anything else. If the groups live in a server-only or missing file, stop
   and say which (CLAUDE.md server-Lua rule); do not invent group contents.
2. **The loader** (`32726850 @3272e4b5..3272eb3e`): the string is split on spaces; each later token other than `"0"`
   is a group ID (members inserted) or, when no group matches, a skill ID; every insert is `{ skill, rate }` into the
   templet map `+0x48` (first insert wins); the float goes to `+0xb8`; the `m_SkillType` / `m_PowerRate` table keys
   fill the vectors `+0x98` / `+0xa8`. Add the members to March's `SocketData` under the flag.
3. **The merge.** Decode how `ConvertAndAdd`'s body `3207cd70` (and the rank-linked copy `320925f0`) carry `+0x48`,
   `+0x98`/`+0xa8` and `+0xb8` into the aggregate.
4. **The readers.** `3201ad10( skill )` (aggregate `+0x48` by skill, then the transcendence and upgrade-base fallbacks,
   plus June's ability and item terms, which are June-only: 0 here) has 17 callers. The core ones (`32024b00` x2,
   `3201f850`, `320462a0`, the summon helper `322ece70` x2, `3236df00`, `32370430`, `32773ee0`) are this phase's;
   name each March host first. Class callers (`329968a0`, `329fdbe0`, `32ad2bb0`, `32c058a0`, `32d076f0`, `3297762a`,
   `32bec286`) go to checklist section 7 (D8 rule). The `+0x98`/`+0xa8` vector reader is `32024f71` (Step 1 table).
   The summon helper's two calls differ (3b3 review, funcmap `322ed3ae`): `@322ed2f7` multiplies the power rate as
   `( 3201ad10 + 1 )` (March hosts: the 14 sites beside `GetSummonPowerRateBySocket` and `JuneApplySummonPowerRate` in
   `X2Game.cpp`); `@322ed394` runs only with flag 0x200 and stores `3201ad10 + 1` to the NPC double `+0x8800`: find its reader first.
5. **`m_fAllStatRelLV`** (`32727838`, `+0x268`, a sixth float after the five `KItemStatRelLVData` members): find its
   reader (start from June's `KItemStatRelLVData` users) and port it, or leave it dormant with one line if none.

6. **Comment fix from the 3b4 review** (no behaviour change): in `ProcessSocketEffectEx`'s SIEE 20 case, the comment
   "June's state IDs are GUSI_END + n; March's USI are n - 1" holds for the hit block only. Reword it: hit block June = March + 1,
   riding block June = March + 5 (riding wait `0x4c`, funcmap `3208ddd0`), walk/dash = March.

A reader that cannot be settled from the disassembly stays dormant with one `[JUNE_BUFF]` line and is listed as open.
If the phase is too large for one session, split it at the item boundary the way 16-step3b2 did.
