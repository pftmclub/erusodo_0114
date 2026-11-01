# June character Chung (Stage 7) - Phase CH1q - Cannon balls

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1q` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL` (+ `..._CBALL_DEBUG`); June bodies inside March functions also need `_LIVE` |
| Log tag | `[JUNE_CHAR]` (`CHUNG CBALL`; lines only once `_LIVE` is defined) |
| Review | `kickoffs/char-chung/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Context"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.
`june2017_re/char_chung/ch0_cannonball.tsv` is the work list.

## Task

Port Chung's cannon-ball system from `ch0_cannonball.tsv`: March's live code is the base, and every function that differs from June gets June's
whole body inside it (March's in `#else`); what June removed is dropped; June-only pieces are added: `CANNON_BALL_COUNT_AT_DUNGEON_START` /
`_PVP_START` (reader `32bf5430`, `INIT_PERSONAL_INFO`), the hyper-count-3 timer skip `32bf4680` (§7 l.236), `EXTRA_CANNON_BALL_DAMAGE_RATE` /
`USE_CANNON_BALL0` if CH0 found readers, the gauge (`CX2ChungGageData`, `CCannonBallUI`, `CX2ChungMyGageUI`, `PGUWO_ORB_CHUNG`) and June's
`DLG_Chung_Cannonball_NEW.lua` if June reads it instead of March's dialog. Check `ChangeMaxCharacterAddResource` (`X2GUChung.cpp:461`, June
`32bf4880`) still matches. `FIX_CANNON_BALL_SYNC_CALCULATION` sites: `cl /P` says which branch is live today. A `[JUNE_CHAR] CHUNG CBALL` line logs
the start count and each spend / gain (debug flag, retired in the review). If the map is over ~45 functions, split as `CH1q2` with a kickoff and
STATUS row.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: build passes with `_LIVE` undefined; the one-file probe with `_LIVE` defined compiles.

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

## Amended by CH0 (2026-10-31)

- **Functions: 15** (`awk -F'\t' '$7=="CH1q"' june2017_re/char_chung/ch0_class_map.tsv`; inside 13, new 2).
  June addresses: `32bdcf20` `32bdcf70` `32bf3d30` `32bf3e40` `32bf3f60` `32bf4040` `32bf4150` `32bf4680`(new ?) `32bf5280` `32bf5430` `32bf6f20` `32c24c80`(new SetExtraCannonBallReloadCount) `32c24d00` `32c24d40` `32c24db0`.
- **States: 1** (`awk -F'\t' '$8=="CH1q"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_CHARGE_CANNON_BALL`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 keys COMMAND_HIT_ADD_MP_BONUS, SIEGE_ANGLE, MOBILE_SIEGE_ANGLE (INIT_C; §5 L184-188 keys CANNON_BALL_COUNT_AT_DUNGEON_START / _AT_PVP_START (INIT_PERSONAL; §7 L236 32bf4680 skips its timer work when the buff hyper count is 3.
- **Cannon-ball map `ch0_cannonball.tsv` (196 rows):** 111 per-March-function rows, 18 sites (UI, gauge data, game readers, sync), 67 June-only counter touchers (state callbacks: they ride with their states' phases and call this phase's helpers).
- Gauge at unit +0xc04, fields equal to March's `CX2ChungGageData`: now +0x110, max +0x11c, extra now +0x128, extra max +0x134, berserk +0x140 / +0x141, cannon-changed +0x142; now / extra set through `3231d7a0` / `3231d880`.
- This phase owns the counter API (`ch0_hand_phases.tsv`): `32bf3d30` up, `32bf3e40` consume, `32bf3f60` set total (%7 split), `32bf4040` set with floor, `32bf5280` use cannon (Reload.ogg), `32bf5430` start counts (`CANNON_BALL_COUNT_AT_DUNGEON_START` / `_PVP_START`; March reads one key `CANNON_BALL_COUNT_AT_DUNGEON_OR_PVP_START`), `32bf4680` hyper-count-3 timer skip, `32c24c80` / `32c24d00` / `32c24d40` / `32c24db0` setters, and `CSI_CHARGE_CANNON_BALL`.
- Outside the class: UI `32e6d6c0` InitUI, `32e63470` InitCannonBallUI, `32e63990` UpdateCannonBallCountUI (`DLG_Chung_Cannonball_NEW.lua`); gauge-data update `32a34720` (uncarved); DamageEffect `USE_CANNONBALL` reader `31f7aab0 @31f7c535`. `FIX_CANNON_BALL_SYNC_CALCULATION` is defined (Always.h:1993): its `#ifdef` arms at X2GUChung.cpp:6350 / 6369 are live.
- Doubtful pairings to settle first: GetTotalCannonBallCount -> `32bed560`, InitRemodelMagazinePassiveSkill -> `32bf4900`, CreateNotEnoughMPEffect -> `32bf3990`, Chung's InitWakeOrb among `32e5b310`-`32e5b8a0`. June's `GameEditCommandList.lua` binds `AddChungCannonBall_Cheat` / `AddChungExtraCannonBall_Cheat` (unbound in March): dev commands, port only if the dev console needs them.

## Amended by CH1l2 (2026-11-01)

- **`32bf3990` is `CommonStateStart`** (vslot 0x7c4, written in CH1l2), not `CreateNotEnoughMPEffect`: strike that pairing.
- **`32c24c80` is the protected-int setter of `m_CannonBallSystem`'s used count** (`this` = unit `+0x9624`, int at `this + 8`): it is March's `CCannonBallSystem::SetUsedCannonballCountAtThisState`, which CommonStateStart calls with 0. It is not `SetExtraCannonBallReloadCount`; read `32c24d00` / `32c24d40` / `32c24db0` before naming those.
- **`32bed560` is not March's `GetTotalCannonBallCount`.** It returns (full hyper mode ? gage max `+0x11c` : now `+0x110`) + (class 117 and `+0x97a0` ? extra `+0x128` : 0), with no x7. The formula `CommonFrameMoveFuture` writes into the sync int is a different one (now + 7 x extra, March's `FIX_CANNON_BALL_SYNC_CALCULATION` arm), so June has two formulas; port each where June uses it. CH1l2 inlines its own and calls neither (`june_march_divergent.tsv` row `GetTotalCannonBallCount`).
- **Per-state rows `m_vecUseCannonBall`** (`std::vector< std::pair<float, int> >`, June `+0x98d8`) are filled by CommonStateStart from `USE_CANNON_BALL<n>` ({ [1] float, [2] int }); this phase finds the reader and names the two fields.
