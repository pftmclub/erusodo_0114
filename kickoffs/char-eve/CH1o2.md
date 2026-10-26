# June character Eve (Stage 7) - Phase CH1o2 - June-only and changed user virtuals at their March call sites

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1o2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (Eve's overrides, undefined until CH1 final); one new `SERV_IRUHADEV_JUNE_*` flag per engine hook (base virtual + March call site), never under an Eve flag |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1o2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1o - Done"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1l - Done (2026-10-21, Opus)"
grep -a -E "^0x(56c|584|65c|6a8|6ac|77c|828)	vslot" june2017_re/char_eve/ch1_offsets.tsv
```
Aisha's CH1l item 2 (0x654 / 0x850 / 0x860 hooks) is the worked example. The `June_Vslot*` bodies are in `X2Lib/X2GuEve.cpp` (CH1k / CH1l blocks).

## Task

CH1o wired every override whose March twin already exists. These remain, because each needs a base virtual and a June call site in March code first (CH1o's Done note has the RE):

| Slot | Eve member | June caller | March counterpart to find | Base (June) |
|---|---|---|---|---|
| 0x3ac | `June_ModifyFinalDamageByGameUnitType( DD*, float&, float& )` | `GetFinalDamage` @322857fb: 0x3ac for every attack type but AT_SPECIAL (0x3b0) | `X2GameUnit.cpp:3205` (`GetFinalDamage`, `pAttDamageData` in scope; March calls its 4-arg form for AT_NORMAL only); Ara's live March override stays | empty |
| 0x7b0 | `June_ApplyAuraAcceleratorBuffToGameUnit( ..., uiIndex )` | find (vslot 0x7b0 call sites) | March's 3-arg virtual (`X2GUUser.h:3519`) and its callers; what June passes as the index | read the User vtable `334781f4` +0x7b0 |
| 0x56c | `June_Vslot56c` (skill-slot icon override) | `328e0aa0` @328e0ee4 (called by `328e18d0`) | inline icon code in `X2Game.cpp` (~14100-14340) | `ret 0xc` |
| 0x6a8 | `June_Vslot6a8` (shown cool time) | `322e07e0` @322e132c / @322e1442 (called by `32308c20`) | skill-slot cool-time text in `X2Game.cpp` (~14900, `OutBox_Skill_Recharge`) | returns the float |
| 0x65c | `June_Vslot65c` (owner's crit bonus for a summon) | NPC getter `3275a820` (protected `+0x4d48` + arg + owner vslot 0x65c) | the March `CX2GUNPC` getter it replaces (find who calls `3275a820`) | 0.0 |
| 0x584 | `June_Vslot584` (can target) | `32f0e260` (via `32f10e60`, class unknown) | identify first | `!HaveThisBuff( 984 )` |
| 0x6ac | `June_Vslot6ac` (move ally summons) | `CX2DungeonGame::SubStageStart` `327cf620` @327cfb79 | March `SubStageStart` | `32001660` (read: not empty) |

For each: name the virtual (June-role name, the `June*` user-vslot convention), add it to `CX2GUUser` / `CX2GameUnit` with June's base body, call it where June calls it, and override it in `X2GuEve.h` under `_LIVE` with the `June_Vslot*` body (rename the member to the settled name). A base whose June body differs from March's existing behaviour is an engine change for every class: check what the other classes' June vtables hold at the slot and route anything that is not Eve's to its owner (scope rule). 0x77c (only Eve's own CH2a / CH3b states call it) and 0x828 (the idle-timer reset, Stage 9) need no hook; confirm and note.

`X2GuEve.*`, `X2GUUser.*`, `X2Game.cpp`, `X2GameUnit.*` are CP949: `bytepatch.py` only. Done gate: the build passes with `_LIVE` undefined, and the `/Zs` probe with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` compiles the new path (`june2017_re/census.py` `cl_cmd`); no play-test.
