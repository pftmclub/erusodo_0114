# June character Rena (Stage 7) - Phase CH2d0 - Trap summons: spawn rules

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH2d0` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2c (play-test passed; no review of its own). One at a time: `X2GULire_ElvenRanger.*`, `X2GUNPC.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP` (+ `..._TRAP_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA TRAP`) |
| Review | `kickoffs/char-rena/CH2d0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH2b2 - Done" "CH2b2 - Review"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH3b0 - Done" "CH3b0 - Review"
```
Then CH0's and CH1's Done notes and `june2017_re/char_rena/ch0_traps.tsv`.

## Task

Do the CH2d0 row of "Phase CH2" (Eve's CH2b2 and Chung's CH3b0 are the worked examples for summon spawn rules). Port from `ch0_traps.tsv` the spawn rules
her traps share: June's `CreateNPCReq` `322e67c0` arguments at the 4 uncarved sites (`32c95882`, `32c99a1a`, `32c9e42e`, `32ca11f2`) against March's 21 sites
(`NUI_ENTANGLE_TRAP_RENA`, `NUI_EVOKE_TRAP_LIRE` / `_MEMO`, `NUI_RENA_ANGER_OF_ELF` / `_DELAY`; Entangle `:15884`, Trap Blade `:17797`, Guide Arrow
`:21554`, Anger of Elf `:22713`), the master / summoned link and the summoned-monster test, the trap's life and removal, `CREATE_TRAP_BLADE`'s reader
`32ca81b0` if CH0 puts it here, and every trap NPC script through `luabind.py check` (unbound names bound whole or `lua_unbound_known.tsv`). Read the
uncarved sites with `charport_adis.py rena <va>`. June's script has no NPC key: the rules live in code. Flag `_TRAP`, sub-tag `TRAP`; a temporary `_TRAP_DEBUG`.
Done gate: the build passes; a trap spawns, links to its owner and expires as June in the hand-off's check (the skills themselves are CH2d / CH3c); say
which sites stay with CH2d / CH3c.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit.

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

## Amended by CH0 (2026-11-07)

- **Functions: 8** (`awk -F'\t' '$7=="CH2d0"' june2017_re/char_rena/ch0_class_map.tsv`; inside 2, new 6).
  June addresses: `32c85900`(new ?) `32c86340`(new ?) `32c863a0`(new ?) `32c87860`(new ?) `32c878f0`(new ?) `32c99690` `32c9e250` `32cb5f60`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L255 IsSummonSkill (54 June skill ids) and the summon cases of 3230c1a0 tha; §7 L280 SetUserSummonedNPCInfo summon stats (322ece70 per-case flags) for ever; §7 L291 Evoke trap NPC scripts Lire_Evoke_trap(.src).lua / _Memo (INIT_COMPONE.
- **Map: `june2017_re/char_rena/ch0_traps.tsv`.** June's live spawns: CreateEvoke `32c99690` (`NUI_EVOKE_TRAP_LIRE` 1020 @32c99a1a, eTeam -1 / eAllyTeam = team: swapped against the others), CreateAngerOfElfTrap `32c9e250` (1420 @32c9e42e), Night Watcher's Gungnir (2083 @32ca11f2, written by CH1g2). June dropped `NUI_EVOKE_TRAP_LIRE_MEMO` (1025) and `NUI_RENA_ANGER_OF_ELF_DELAY` (1421), and its Entangle NPC spawn (`32c95790`, dead). Summon-info setters `32c85900` / `32c86340` / `32c863a0` / `32c87860` / `32c878f0` (called from CX2Game summon code 3230bbbd-3230c8b6) + `32cb5f60`; the Evoke trap scripts `Lire_Evoke_trap.lua` / `_Memo.lua` (l.291, `USE_OWNERS_ENCHANT`) through `luabind.py check`.
