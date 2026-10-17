# Atlas/Elysion port - Phase E1b - June's CX2TFieldNpc, everything but OnFrameMove

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1b` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E1a (June's house templet keys + relation data; done) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |
| Review | `kickoffs/atlas/E1b-review.md` (Opus 5.5; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Village engine" "Phase E1 — Split (user, 2026-10-17)" "Phase E1a — Done" "Phase E1a — Review (2026-10-17)"
cat june2017_re/e1_createfieldnpc_diff.md      # June CreateFieldNpc 325ca980, 24 blocks
cat june2017_re/e1_talkbye_diff.md             # talk 325ccf60, bye 325ca2f0, helpers, UIUnitManager
python tools/docsection.py june2017_re/e1_relation_runtime.md "June HouseTemplet (0x618) offsets -> key"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Port June's `CX2TFieldNpc` whole except `OnFrameMove` (E1c ports that and the effect set), reading the new templet fields E1a
parses. Whole functions, June addresses cited:
- ctor `325d13e0` / dtor `325cd7c0` members (2nd skin +0x1c4/1c8/1cc, MultiTex xet +0x1ec, motion map +0x1f4, motion queue
  +0x204, UI file +0x1d0, anim speeds +0x5d8/5dc/5e0, the June-only flags +0x5e8..+0x60a, talk strings +0x4b4.. / +0x534..,
  +0x524 map, +0x1a6 in-conversation), and the effect-set handle +0x190 set to -1 (E1c plays it);
- `CreateFieldNpc( world, templet, name )` `325ca980` whole (name override, CUSTOM_NPC_LOACION position `325c95e0`, 6-id static
  list, 3-id shadow gate, 2nd skin, motion map, CUSTOM_MOTION_NAME, UI file copy, the friendship-manager templet override as
  June's call site only: the manager is backlog Stage 18);
- the talk step `325ccf60` and the bye step `325ca2f0` (June outlined them from March's OnFrameMove; March's inline blocks call
  them under the flag), `325ca920`, `325ca950` SetEnable, `325c9130` SetShowField, `325c9000` SetShowObject, `325c8ce0`
  ResetNpc, `325c8250` SetCanTalkNpc, `325c8210`, the quest-conditional message / motion overrides `325ca0b0` / `325ca1b0`
  (templet +0x590 rows), NpcPlaySound `325c9c00`, the render-param routine `325d1040` (find its caller first);
- `CX2UIUnitManager::GetUIUnit( id, fileName )` `32806b30`, `CreateUIUnit` `32806910`, `32ee37a0`, `ClearUIUnit( id )`
  `32806800`, and the `CX2UIUnit` members they need (+0x190 file name, +0x35c override flag);
- `CX2TFieldNpcShop::SetShopType`'s 4-argument form and June's shop button ids (0x17..0x25; ids 3..0x15 equal March's), as far
  as CreateFieldNpc / the talk step call them; a button March has no UI for is the user's call (list it, ask once).
Friendship branches (code event 2215): June's gate and calls only (Stage 18 owns the manager; whichever lands second wires them).
Add JUNE_BUGS rows for every quirk kept.

**Done when:** every function above is in under the flag with its June address; build rc=0; existing villages unchanged except
the listed June behaviour changes (static list, shadow gate, talk / bye motions from the motion map), each named in the Done note
and in the play-test hand-off (talk to and leave a March village NPC, a static NPC (postbox), the billboard).
