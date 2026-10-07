# June effect completion - C4b - identify June's NPC Fury mode (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C4b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C4 review (done 2026-10-07) |
| Flag | none (no code); C4c's flag will be `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | none (its port, C4c, is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4 - Review (2026-10-07, Opus 5.5 high)"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context"
grep -n "Effect C4 review" june2017_re/funcmap.tsv
```

The rebuild plan's Starting context predates `tools/`. Use the x2-phase skill's tools instead of
anything it says to do by hand. Ghidra work uses the `ghidra` skill (read-only); verdicts go into
`june2017_re/funcmap.tsv` with the tag `Effect C4b`.

## Why this phase exists

The C4 review identified the block June runs in `CX2GameUnit::GetFinalDamage` just before the
`DIVIDE_DAMAGE_FROM_RANGE` call (`32284aa0 @32285757-322857e8`): an NPC attacker whose current state
has `FURY_MODE` (StateData bitset bit 80) and whose byte `+0x8fb7` is 1 has its physic and magic
damage multiplied by the state's `ATTACK_POWER_RATE_FURY` (StateData `+0x414`). March has no Fury
mode at all (no key, no flag, no member). It is not a DamageData key and not per-class, so it is
not C4's and not a character-port item: an unowned June system, which gets its own phase (standing
rule: follow June, don't ask). Scope it to the **whole** system, not just the GetFinalDamage multiply
(no partial updates).

## Task

Identify, with June addresses, everything the C4c port needs:

1. **Parse.** In the NPC state parser `32784c00`: `FURY_MODE` (`@3278a088`, bitset bit `0x50`),
   `ATTACK_POWER_RATE_FURY` (`@327963cb`, `+0x414`) and `ANIM_SPEED_RATE_FURY` (`@327965a4`): the
   getter, the default each one gets when absent, the StateData offset, and where March's
   `CX2GUNPC` state parse (`StateData`, `X2GUNPC.h` `FLAG_*` enum) would take each. Check whether
   June's `FLAG_*` enum otherwise lines up with March's (bit 80 is June's numbering; the March port
   adds a new enum value, it does not reuse 80).
2. **The NPC byte `+0x8fb7`.** Every writer and reader (`june2017_re/offsetprobe.py` or a capstone
   sweep over `CX2GUNPC` code): what turns Fury on and off (an HP threshold, a Lua bind, a trigger,
   a server packet?), and whether any of it is server-driven (offline: design a stand-in only if
   June needs a value only a server sends; say so and stop for the user).
3. **Every reader** of `FURY_MODE`, `+0x414`, the `ANIM_SPEED_RATE_FURY` offset and `+0x8fb7`:
   the GetFinalDamage multiply is one; find the animation-speed consumer and any UI, effect or
   sound tied to Fury (e.g. a Lua bind such as `IsFuryMode` in the `CX2GUNPC` lua_tinker list).
4. **Reach.** Which June scripts use the keys (`june_src_fixed/`: `RAID_ELTRION_MK2_*`,
   `Raid_Red_Dragon_Phase2`, any others), whether any March NPC script or adopted June script could
   reach it in this build, and what a play-test would need (a test NPC script staged in data036 is
   acceptable if nothing in March reaches it).

## Done gate

funcmap rows tagged `Effect C4b` for every address above (no `candidate` left for the system); a
"C4b - Done" note under C4 in the plan doc with the port list for C4c (files, members, call sites,
defaults, JUNE_BUGS candidates); `kickoffs/june-effect/C4c.md` updated with those findings; STATUS
rows for C4b/C4c updated. No code, no build.
