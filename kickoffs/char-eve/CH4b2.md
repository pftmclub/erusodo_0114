# June character Eve (Stage 7) - Phase CH4b2 - Summon-time Open Mind stack (`3204e160`)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4b2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH4b (play-test 2026-10-28) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ CH4b's temporary `SERV_IRUHADEV_JUNE_CHAR_EVE_OVERRIDE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE`) |
| Review | `kickoffs/char-eve/CH4b-review.md` covers CH4b and CH4b2 together (amend it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH4b - Done (2026-10-28, Opus 5.5)"
```
Then funcmap rows `3230c1a0 @3230cafb (CH2b2)`, `31fc96b0`, `3200bec0`, `322a8a70`, `322a7660`, `326c3170` and the `(CH4b)` rows; `june2017_re/char_eve/ch2b2_summon_buff_map.tsv`.

## Task

CH4b missed this: CH2b2 routed it to CH4b (plan doc CH2b2 "Branches found", funcmap row `3230c1a0 @3230cafb`), and the CH4b play-test showed no Open Mind x0.85 on unit 61. Port June's `CX2GUUser` function `3204e160( iNPCID )` whole (`ret 4`, range `3204e160..3204e376`), called from `CX2Game::SetUserSummonedNPCInfo` `3230c1a0 @3230cafb` on the summon's owner, between the `3236ca50` stat call and `3200bec0` (`JuneKillOlderSummonsOfSameID`, already ported). You are on Opus: delegate the reads, greps and known-address dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`.
- **Head:** `31fc96b0` (`JuneGetSummonBuffFactorID`, ported) -> 0 returns; else user vslot `0x21c`( bfi, 0, 1, 0, 0 ). Identify `0x21c` (likely a `SetBuffFactorToGameUnitByBuffFactorID` form) and check it against the June skill-start calls that already apply the same factors (`X2GuEve.cpp` `BFI_BUFF_SENTRY` / `OBERON` / `OPHELIA`), so nothing is applied twice where June applies it once.
- **Open Mind 25417 block** (`@3204e1ae-3204e2ea`): learned -> the factor's templet BTI (`326c32a0` +0x28) in {410 SENTRY, 412 OBERON, 413 OPHELIA} (`0x19b` ATOMIC_SHIELD is out): with 25417's buff on (vslot `0x240`) -> `322a8a70( 877 BFI_BUFF_ECE_OPEN_MIND, -1, level )`; else vslot `0x23c( 25417, 0, true )` (`June_SetSkillBuffFactors`), `3229a3d0( 25417, 0, 1 )`, templet `+0x3c` (default 1) -> `322a7660( 877, n, level )`. Port `322a8a70` / `322a7660` (CX2GameUnit stack-count set, "not ported" since Buff Phase 4b-i part 3) and `3229a3d0` whole, in the shared place.
- **Tail** `@3204e2ed`: NPC 762 (`NUI_EVENT_EL_THIEF`) with Rose's 31320 `RNSI_A_ROM_G_EX_MAGNETICFORCE_UPGRADE` -> `3227bfa0` / `3227c110` (+1.0). Port it with the function (no partial updates); it stays dormant until Rose (Stage 8).
- Done gate: unit 61 (`EveEmpress`) casts Enhanced Electronic Field twice: the second cast's `mp consume` line shows x0.85 (`-> 63.75`, or `-> 85.00` with a summon out), and the Open Mind buff's stack count shows in a named log line. Also re-run CH4b's Energy Needles check: unit 60's slot A3 now holds Enhanced Energy Needles 25067 (save backup `els_db_20261028_153811_pre_sql.sql`); cast it, then another skill: `cool time ( ... X -> 0.8X )`.
- Bookkeeping: CH4b's Done note gets a CH4b2 section; the STATUS rows; amend `CH4b-review.md` to name `3204e160` and the helpers; funcmap rows.
