# June character Eve (Stage 7) - Phase CH2b2 - How June spawns a summon: one NPC per owner, or many?

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b2` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2b play-test 2 (Oberon slash fix, commit pending). Touches shared summon code, so not beside another `X2Game.*` / `X2GuEve.*` phase |
| Flag | decided by the phase (`SERV_IRUHADEV_JUNE_SUMMON_*`, defined in `KTDXLIB/Always.h` only if a change is needed) |
| Log tag | `[JUNE_SUMMON]` |
| Review | `kickoffs/char-eve/CH2b2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b - Done (2026-10-27, Sonnet 5.5)" "Phase CH2b2 - How June spawns a summon"
```

## Task

**Observed (user play-test 2, 2026-10-27):** every cast of Surface Cuting (25015) spawns a new Oberon; up to 4 Oberons lived together (`[JUNE_CHAR] EVE SKILL OBERON uid ...` lines, one per second per Oberon). The user expects one summon per owner and NPC type ("a new spawn resets the old one"), and suspects the rule is not Surface-Cuting-specific but lives in **how June spawns an NPC**. CH2b found no dedupe in the Eve class code it read; that is not proof there is none.

Find what June does, then make March do the same. Read June in Ghidra (raw listings: `python june2017_re/charport_adis.py eve <va>`, Ghidra has no function for some thunks). Questions, in order:
1. **The create path.** `3299b810` (`June_SummonSurfaceCuting`) -> `31f8b010` (forwarder, first param = the `IsMyUnit` gate) -> `322e67c0` (create-NPC core, 19 stack words) -> what is sent / queued, and what answers it. Does anything in the chain look at existing NPCs owned by the same unit (loop over the NPC list, a per-owner count / cap, a "kill the previous" call, `createType` / `NCT_*`, the `bNeedOnlyOne`-like or `TN_*` args)?
2. **The receive side.** The handler that turns the request into a unit (June's `CX2Game` create-NPC handler, the `E_*NPC*` / `EGS_NPC_UNIT_CREATE_*` event path). The live game created summons on the **server**; offline, `X2Lib/Offline/` answers instead. Is a one-per-owner rule a server rule June's client never shows? Read March's offline answer (`Handlers_*.cpp`, grep `NPC_UNIT_CREATE`) to see what stand-in exists.
3. **NPC-side keys.** June NPC Lua / `NPCTemplet` keys or `CX2GUNPC` members that cap or replace summons (owner UID, summon group, `SetSelfDestruct...`, `June_GetQueensControlCount`-style counters, `JuneSetSelfDestructSummonedNPC`). Which already ported summons use them (Oberon Guard's `June_ClearGuardAndSummon`, `June_SetSelfDestructSummonedNPC`)?
4. **Census.** Every June `CreateNPCReq` caller on the user side (other characters' summons too: list va, NPC id, and whether a replace/cap exists). One row each in a tsv under `june2017_re/` and a short table in the Done note. This is the evidence for "bound to how the NPC spawns" or "Surface Cuting only".

**Outcome, pick the one the evidence supports and say which:**
- June's client has the rule in the create path or NPC keys -> port it whole, under a `SERV_IRUHADEV_JUNE_SUMMON_*` flag, for every summon it covers (not only Oberon). Funcmap rows, a `JUNE_BUGS.md` row for any quirk.
- The rule is a server rule June's client never shows -> a labelled stand-in in the offline answer under its own flag ("design missing June pieces"): keep June's absent-value default; ask the user before choosing the stand-in's behaviour (replace the old one vs refuse the new one).
- June really allows many -> no code change; write that into `JUNE_BUGS.md` / the Done note and tell the user. Do not "fix" it.

Play-test hand-off: Eve (unit 19 `Iivue`, slot 0), cast Surface Cuting 3 times in a row near enemies; expect the count the outcome gives (the `[JUNE_CHAR] EVE SKILL OBERON` debug logger lines show every live Oberon; retire `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG` here or in the CH2b review, whichever runs first).

You are on Opus: delegate the reads, greps, the census and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: the three questions answered with June addresses, the census tsv written, and either a flagged change that passes `flagcheck` + a play-test hand-off, or a recorded "June allows many".

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.
