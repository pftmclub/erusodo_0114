# June character Aisha (Stage 5 pilot) - Phase CH4 - Passives and class mechanics

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH2b review (CH2 was split by CH0); not alongside CH3a / CH3b (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`, `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` (or CH0's split) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`, `ARME MEMO`) |
| Review | `kickoffs/pilot-char-revamp/CH4-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)" "7aj. Tree-UI class pieces June keyed to one class (from June skill system S7b)" "7ak. Skill page decide ACK class pieces (from June skill system S7d)"
```
Then the `ch4` rows of CH0's tsvs, and CH2's Done note.

## Task

Do "Phase CH4". Map each June mechanism in Ghidra before porting it (memorize: slot-data type 5 `328d9ac0`, tooltip `328e9160`, decide ACK
`32ce1770`, and the class side CH0 found). Delegate the mechanical reads, builds and log reads to subagents.
- If memorize needs a save change, snapshot `db_backup/` first. The rung is forward-only; propose it to the user before writing it.
- Edit CP949 files with `bytepatch.py` only.

## Amended by CH0

Written by CH0 (2026-10-20). Depends now read **CH2b review** (CH2 was split), and CH4 runs before or after CH3a / CH3b, never beside them.

Filters: `awk -F'\t' '$6=="CH4"' ch0_class_map.tsv` (20 functions), `awk -F'\t' 'NF==7 && $6=="CH4"' ch0_state_skill.tsv` (25 skill ids:
types 1 / 5 / 8), `awk -F'\t' '$5 ~ /CH4/' ch0_checklist_rows.tsv`.
- **Overrides:** `SetDamageExceptionProcess` `32cccc70` (vslot 0x2b8; 7b and the D8 Fitness 2413 -> Magical Staff 22409 swap, whole),
  `SetSpecificValueByBuffTempletID` `32cddec0` / `UnSet...` `32cde300` (0x24c / 0x250, Magical Makeup transform), `GetAdditionalAccelBuffTime`
  `32cc1b90` (0x7ac), `GetActualMPConsume` `32ccfe30` (0x6a0), `GetActualCoolTime` `32ce1cc0` (0x6a4, June-only override), `InitPassiveSkillState`
  `32cecd70` (0x690), the two skill-memo float hooks `32cc28b0` (0x428) and `32cc5c30` (0x73c), and their helpers.
- **Memorize:** 7ah (the 10-slot dialog's memorize statics), 7aj (slot data type 5 `328d9ac0`, `SESAPT_MEMORIZE` picture `328e0460`, mouse-in
  29059 `328ec719`, tooltip line `328e9160`; class helper `32ce1650` called from `328dfaa0`), 7ak (`32ce1770` on the decide-page ACK).
- **Buff skills (type 5):** the state is the shared `AVSI_SI_AV_COMMON_BUFF` (CH1); CH4 brings the buff data rows.

## From CH1c (2026-10-20)

- `InitPassiveSkillState` (`32cecd70`) builds the object at `+0x96ac` (`32cf2190`, 0x2c bytes: chance at `+0x10` = skill 22415 `SA_EFFECTIVE_RATE`, the refund rate = a protected float at `+0x20` = its ability `0x42`; the 22405 values sit in the container at `+0x14`)
  and sets the combo fireball element (`+0x9630`: `32ced3dc` 0, `32ced402` 2; where it becomes 1 (ice) was not found in CH1c's scan of the class range: find it), `+0x9634` (ball power), `+0x9638`, `+0x963c` / `+0x9640` (gravity). CH1c's inert `m_pJuneMpRefund` (`JuneMpRefundData { m_fChance, m_fRate }`) and
  `m_iJuneBallElement` are the stand-ins to replace; `June_FlushMpWithRefund` (`32cc2b90`) and `June_CreateComboFireBall` (`32ce4180`) read them.

## From CH1c review (2026-10-20)

- **The refund's MP text.** June `32cc2b90` refunds through `32278070`, not bare UpNowMp: `UpNowMp( x )` (vslot `0xf0`), then `x + 0.005` is pushed onto the unit's MP-text queue `+0x68ac` (timer `+0x68a8`).
  `3229ecc0` shows the queue as "MP +N" before the HP queue, and when it showed text that frame it only restarts the HP timer (see `JuneFrameMoveHpChangeText`, `X2GameUnit.cpp`, and the HP twin `322780b0`, Atlas E7).
  When the refund object goes live, port `32278070` and the MP half of `3229ecc0` (read the display branch in Ghidra: picture and text), and switch `June_FlushMpWithRefund`'s `UpNowMp` to it. `32278070` has more than 20 callers in other classes' code, so it is an engine member of `CX2GameUnit`.

## From CH1h (2026-10-20)

The class map now lists `32ce9f90`, the callback of the skill 22416 passive (skill level via `31fceab0( 0x5790 )`, damage trigger 0x11 via `32610fb0`): its thunk `32cf2d70` is stored inside `InitPassiveSkillState` `32cecd70` @`32ced61f`.

## From the CH1h review (2026-10-21)

- **`322deb80` is yours: the CX2Game memorize-slot HUD.** For the own unit, with CX2Unit `+0xaf0` unit type 2 and a non-empty MemoRise list (`32cc2880`), it sets up `Memorize_Slot_%d_TypeA` / `_TypeB` (game option `+0x278` picks the set) and `UseMemorizedSkillHotKey_A` / `_B` for each entry. An entry's skill id (`32cc7df0`) below 1 hides the slot, otherwise `3288fff0` looks the slot up. These are the 10-slot dialog's memorize statics (7ah). Port it with them and replace the no-op `June_GameRefreshMemoRise` in the CH1h block. Callers: `328e0ff0`, `32ce1770`, `32ccf4e0`, `32ce16d3`, `32ce1900`, `32ceca70`, `31f5ea00`.
- June-only members your overrides write: `+0x9780` float (SetDamageExceptionProcess `32cccc70`); `+0x9790` / `+0x9794` / `+0x97a0` floats (InitPassiveSkillState `32cecd70`, GetActualMPConsume reads `+0x97a0`); the `+0x9848` vector (helpers `32cc91b0`, `32cdbd00`: settle its element type there).

## From CH1i (2026-10-21)

- **Ported in CH1i because the skill-use steps call them:** `June_AddMemoRise` ( `32ceca70` ), `June_GetMemoRiseEntry` ( `32cc7df0` ), `June_SelectSkillSlotKind` ( `32cc7e60` ), `June_EraseMemoRise` ( `32ce15a0` ), `June_SweepMemoRise` ( `32ce1770` ). The container is `std::deque<int> m_deqJuneMemoRise`; `June_ClearMemoRise` / `June_IsMemoRiseEmpty` use it. Your `InitPassiveSkillState` ( `32cecd70` ) does not need to re-port them.
- **The memorize-slot HUD is yours, both halves:** `322deb80` ( `June_GameRefreshMemoRise`, the no-op in the CH1h block ) and `322f4740`, the CX2Game per-frame pass ( own Aisha only, vslot `0x718` true: `+0xb38` += elapsed, `Aisha_Memorise_P0n` particles, `Static_Skill_Cool_Time_Fade`; it resets `+0xb38` at 2.8 s ( `3351aff0` ) ). `322cc030` ( `June_SetGameFloatB38` ) only stores that float; nothing in `June_*` calls it yet.
- `JuneMpRefundData` ( `+0x96ac` ) now is June's Strong Mind object: it derives `TimeLimited` ( `+4` timer ) and has `m_fSpecialActiveSkillRate` ( `+8` ), `m_fActiveSkillRate` ( `+0xc` ), `m_fChance` ( `+0x10` ), `m_fMPRestoreRate` ( `+0x14` protected ), `m_fRate` ( `+0x20` protected ). `June_SpecialAttackCooldown` and `June_CommonFrameMove` read them; you build the object.
- `m_bJune978d` ( June `+0x978d` ) must be set by the Mana Flow Overdrive passive ( 22410 ) for `June_ManaflowOverdriveFrameMove` ( `32cc5b20` ) to run.
