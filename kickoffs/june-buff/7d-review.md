# June buff port - Phase 7d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7d review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 7d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM` (requires `_AWAKE_LOCK`) (+ `JUNE_BUFF_DEBUG` for the arm lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 7d -- June's quick-slot" "Phase 7d -- Done"
```

The "Buff Phase 7d" rows of `june2017_re/funcmap.tsv` hold the RE (322fa960 corrected, 32951670,
32951420, 3294f070, 3294cce0, 32950d60, 322f5140, 322fbc60, 32d1bd10 corrected, the getters).
`JUNE_BUGS.md` #12 (now reached) and #13, and the two slot-510 / vslot 0x4ac rows of
`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7 came from this phase.

## Task

Review the Phase 7d commit against June. Check these first, because 7d corrected earlier records:

1. **One caller.** `322fa960` is called only from `329515d9` (in `32951420`); 7c's `32024510` /
   `3200edb0` call `322fbc60` = `UseItemSpecialAbility`. Re-run a raw call scan.
2. **Argument roles.** 3rd arg = allow byte (only cleared, except Value2 == 4 sets it), 4th =
   the quick slot's `+0xe4` ACK wait (set on accept and in the default case). Check
   `@322fad57`/`@322fad69`/`@322fbbe1` and the push order at `@329515a7-d9`.
3. **Slot 510 is not `NoStateChangeActionFrameMove`.** 7d found it returns bool, calls
   `31fcadd0` (CommonSpecialAttackEventProcess) and is called first by slot 509
   (`32cf3290`, SpecialAttackEventProcess), so the prefix went to the top of each March class's
   `SpecialAttackEventProcess`. Confirm in two classes besides Elsword. June gates only the 510
   lookup; 509 itself does nothing of March's before calling 510 (7d checked Elsword `32cf3290` and
   Ara `32b90c20`). Confirm that the early return skips nothing June keeps outside 510, e.g. Ara's
   `FIX_SKILL_CANCEL_ARA` reserved-skill block and `CanUseSpecialAttack` at the top of March's Ara/EL/Add
   functions, and any non-skill input handled inside a class's `SpecialAttackEventProcess`.
4. **Each ported case** of `CX2Game::CheckQuickSlotItemSpecialAbility` against the switch:
   16 (riding, Future skill ID via `+0x7cb0`), 18 (Value1 13/14 + item 100005189, Value2 4,
   bLandConnect `+0x7fb4` / `+0x3c5`, the pre-start branch, riding, the six states including
   vslot `0x4ac`, the four gates), 34, 39, default. The Future/Now mapping of `+0x7cb0`/`+0x7cbc`
   is a candidate (member order); say whether anything settles it.
5. **What was left out** as June-only: SAT 38 and 43, the GT 4 riding refusal, the PvP item bar
   (`+0x101`, PvP room `+0xf0 == 2`), event items 291687/100003917, June's pet item check, the
   group-0x67 gate, the cooldown bypass, the pre-start allow paths (`322f5140`). Challenge any
   that March actually has.
6. **KeyEventProcess routing**: pet GM check after the cooldown, `CheckQuickSlotItemSpecialAbility`
   only when allowed and a game exists, ACK wait cleared on refusal (`@329517d0`), `#else` = March.

Hygiene is one command:
`python tools/flagcheck.py JUNE_BUFF_QUICKSLOT_ITEM --range <the 7d code commit>^..HEAD --with JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding.

Spot-check items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions
back to the user. Write "Phase 7d -- Review" into the plan doc, update the 7d review row in
STATUS.md, and commit.
