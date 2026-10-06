# June buff port - Phase 16-step3b2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b2 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 16-step3b2 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 16 -- Step 1 RE" "Phase 16-step3b2 -- Done"
```

The decodes are the `funcmap.tsv` rows tagged "Buff Phase 16-step3b2".

## Task

Review 16-step3b2's commits against June (Ghidra read-only, raw disassembly; run x87 blocks under Unicorn
rather than reading them). What this phase could get wrong:

- `GetFinalDamage` (`32284aa0`): the three attacker terms' gates and polarities (`@322852db..32285481`), the
  piercing cap 0.2 and its per-side placement in DT_MIX (`@32285849..32285bef`), the boss term's attacker
  result and the grade source, the my-HP-few defence's position before the clamp. Check each insertion point
  against the March order the Done note names.
- User `DamageReact`: the boss/52fc terms sit inside "attacker is an NPC"; the 1.0 cap; the critical
  threshold (`32273480` = panic + ..., `+0x2b28`/`+0x2bc4` named as `m_Panic`, worth one independent check);
  `GetJuneCriticalDamageRate` against both jump tables (`3204a834`, `327b5390`) and the NPC-attacker path.
- `32275ba0`: the AT_NORMAL branch adds `+0x2ec` (not multiplies); other attack types pass their own type.
- Item 6: the map is cleared once per `InitSocketData` before the first `SumSocketOption`, filled per socket after
  the Hamel block (skipped by the SIEE `continue`, as June's tail is), and the 0.8 floor in
  `ApplySkillCoolTimeRateByBuff`.
- Decision points the Done note hands to the user (item 4's bool, the boss-grade source): check the facts they
  rest on, do not decide them.

Hygiene: `python tools/flagcheck.py JUNE_SOCKET_PARITY --range e002c27^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_DAMAGEDATA EXTRA_NPC_STAT`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and deleted line.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); write "Phase 16-step3b2 --
Review", update STATUS.md, check `16-step3b3.md` / `16-step3b4.md`, and commit.
