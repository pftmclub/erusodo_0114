# June character Chung (Stage 7) - Phase CH1n2 - The non-riding base callbacks June changed

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n2` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GUUser.*` |
| Flag | a new `SERV_IRUHADEV_JUNE_USER_BASE_STATE_BODIES` (every class; not Chung's `_LIVE`) in `KTDXLIB/Always.h` |
| Log tag | none new (bodies only) |
| Review | `kickoffs/char-chung/CH1n2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1n - Done (2026-11-02)" "CH1n - Review (2026-11-02)"
awk -F'\t' '$5=="DIFF" && $1 !~ /^Riding/' june2017_re/char_chung/ch1n_user_callback_triage.tsv
```

## Task

Split from CH1n (scope: 69 of the 127 `CX2GUUser` callbacks Chung registers differ from March; 48 `Riding*` rows went to Stage 17, 5 rows stay as decided).
Port June's whole body inside March's own `CX2GUUser` member, `#ifdef SERV_IRUHADEV_JUNE_USER_BASE_STATE_BODIES` June `#else` March, for the 16 rows:
`WalkFrameMoveFuture` `31fd23e0`, `JumpFrameMoveFuture` `31fd2470`, `DashFrameMoveFuture` `31fd25e0`, `DashStartFuture` `31fcbe80` (vslot 0x5fc),
`DashJumpLandingStartFuture` `31fcbe40`, `DamageAirSmallEventProcess` `31fcc150`, `StartEnd` `3201b460`, `HyperModeStart` `31ffd490`,
`PEPPER_RUN_FrameMoveFuture` `31fd1c90`, `PEPPER_RUN_END_FrameMove` `31fd98b0`, `PEPPER_RUN_JUMP_UP_StateStartFuture` `31fd1d20`,
`PEPPER_RUN_JUMP_UP_FrameMoveFuture` `31fd1d80`, `PEPPER_RUN_JUMP_DOWN_FrameMoveFuture` `31fd1e60`, `THROW_ITEM_FrameMove` `31ff92d0`,
`THROW_ITEM_EventProcess` `31fcbc10`, `THROW_ITEM_StateEnd` `31ffaad0`.
- The triage tsv is a lead list from a Sonnet pass: re-read each row against the listing (`python june2017_re/charport_adis.py chung <va>`), and
  check each body for callers outside this list first (`junediverge.py --list`, `june_march_divergent.tsv`: every class gets the change).
- Identify what the triage left open: `31f4b1e0( g_pData +0x64, 0x73d, ... )` (HyperModeStart and the riding hyper rows), byte `+0x8fc1`
  (pepper-run jump skip), `+0x8fc3` (hyper gauge gate), `31f54a10` on `+0x93bc`, the extra `SPECIAL_ITEM_*` cases of THROW_ITEM (each item id by
  its `Enum`/item name; a June item March's item.kim lacks is noted, not invented), `StartEnd`'s game-type / tutorial gate and dungeon 0xa1e0 (by name).
  An unmapped engine callee that needs a system March lacks gets a stand-in under its own flag and an owner, never an invented body.
- Kept as decided (do not port): StartStateStart's PvP round particles, StartEventProcess's `3200ac50` (Stage 19), HyperModeEnd's Add stand-in,
  RandomLose's `find` guard.
- `DamageAirSmallEventProcess` calls `SpecialAttackEventProcess( ASUT_DAMAGE )` while airborne: Aisha CH1k added condition 6; check each ported
  class's lookup answers it.
- A row with an emulator-visible effect (none expected) keeps `X2OfflineDB` / handlers consistent.

CP949: `bytepatch.py` only; `cl /P` each `#ifdef`. One-line comments with the June address. No hardcoded enum literals, no `June` in names
(CLAUDE.md). Done gate: build passes, flag on; `flagcheck`, `junediverge --check`, `enumscan --check` clean; a play-test hand-off for Aisha or Eve
(already live): walk / jump / dash with both arrows held, a skill used while hit in the air, a throw item, a pepper run.
