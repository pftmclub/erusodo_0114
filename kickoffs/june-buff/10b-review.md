# June buff port - Phase 10b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 10b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 10b done (its Done note and commit, see STATUS.md) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_AURA_PARTY`, `_AURA_REACT`, `_GUILD_REVERSE` (+ Phase 10's `_DAMAGEDATA_KEY`, whose comments and two log strings 10b rewrote) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 10b -- The DamageData buff keys" "Phase 10b -- Done"
```

The "Buff Phase 10b" rows of `june2017_re/funcmap.tsv` hold the RE (15 new rows, plus edits to
`32622990` and `3227a270`). `JUNE_BUGS.md` #22-#24 are 10b's.

## Task

Review the Phase 10b commit against June. Check these first:

1. **The aura gate at all seven sites** (`32623110 @32623c5d`, `32624610 @32624885`,
   `32625000 @326250fa`, `32625670 @326257d7`, `32628890 @326289e3`, `3262b6e0 @3262b89e`,
   `326413c0 @326414ad`): which unit gets `IsMyUnit` (vslot 0x198), which UID goes to
   `IsMyPartyMember` (`32431360`), the same-UID exception (absent in GroundHit only), and the
   attacker each site uses (EffectToUnit: the effect's owner; UnitToEffect: the target is the
   hit effect's owner). Confirm the March insertion points sit at June's place in the filter order.
2. **The reader census:** 10b byte-scanned `.text` for every byte read of `[reg+0x5f0]` and
   classified the hits (DamageData readers, the copy at `31e9ca7f`, a `+0x244` UI class at
   `325c99xx`/`325cbxxx`, the uncalled `32610880`). Re-run a scan of your own and say whether
   any DamageData reader is missing.
3. **`32610df0` (Chance to Reverse):** the guard order (`+0x500`, `+0x5f0`, `damageType != 6`),
   attacker before target, vslot `0x290` = `GuildSkillProcess` with `0x1fd`, and the eight call
   sites: a miss and a hit in UnitToUnit, MeshToUnit, CollisionDataToUnit and EffectToUnit. In
   EffectToUnit the hit call uses `pAttackUnit` (the owner taken at the target's start) where
   June reads the owner off the effect after `AttackResult`: say whether that can differ.
   Also: the helper's one-time hit log prints both UIDs after null-checking only one side; every
   call site passes non-null for both today, so decide whether it needs a guard.
4. **`320471d6` / `327b3122`:** only `PlayEffectSetHittedByBuff` is gated; nothing else in
   either DamageReact reads `+0x5f0`.
5. **LIMIT_APPLY_BUFF_COUNT_EACH_UNIT:** confirm `3227a270` and `322759b0` have no caller of
   any kind (call, jmp, jcc, short jump, pointer, vtable slot), so not porting them matches June.
6. **`32622990`** (team pre-filter): its first test is `IsPve` (`322cc1c0`); confirm there is
   no March path where it would run.

Hygiene is one command per flag, not a fresh script: run each with
`--with JUNE_BUFF_AURA_PARTY JUNE_BUFF_AURA_REACT JUNE_BUFF_GUILD_REVERSE JUNE_BUFF_DAMAGEDATA_KEY JUNE_BUFF_DEBUG`
and `--range <Phase 10b commit>^..<Phase 10b commit>`, for `JUNE_BUFF_AURA_PARTY`,
`_AURA_REACT`, `_GUILD_REVERSE` and `_DAMAGEDATA_KEY`. Paste their `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line (10b's
5 deleted lines are Phase 10's own comments and log strings inside `_DAMAGEDATA_KEY`); a FAIL
or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 10b -- Review" into the plan doc, update the 10b review
row in STATUS.md, and commit.
