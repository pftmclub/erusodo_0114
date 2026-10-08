# June effect completion - C10e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C10e review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C10e done and play-tested (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGE_REACT_HOOKS`, `SERV_IRUHADEV_JUNE_SOCKET_COOLTIME_ON_HIT` |
| Log tag | `JUNE_FX socket-cooltime` in `offline_server.log` (unbracketed: `python tools/logs.py --grep "JUNE_FX socket-cooltime"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C10 - Decided"
grep "Effect C10d" june2017_re/funcmap.tsv
```

The C10e Done note is at the end of that section. Also read the phase's commits (`git log --oneline --grep="C10e"`).

## Task

Review C10e's commits against June (read-only in Ghidra) and the kickoff `kickoffs/june-effect/C10e.md`:

- **Hooks.**
  - User `DamageReact` `320462a0`: the `0x4f8`/`0x4fc` call (`@320495d8-3204963b`), `322cfca0` (`@32049678`), the attacker's `0x304` (`@3204972c`), and their order against March's stasis block and the C10b-1 HP-rate call.
  - NPC `327b1f30`: the hook pair sits **inside** the level-damage / attacker gate (`@327b4434-327b4482`); the socket block, `322cfca0` and `0x304` sit at function scope in that order, before slot 6 (`@327b480a`).
- **`322cfca0`.** The two named constants (8 / 2) against June's `Enum.lua`; strict `!=` on the attack type; `*= 0.5f`; the PvP room getter.
- **Socket 2000145.** The five sites:
  - ctor, the reset before the sum, the set in `SumSocketOption` (`@3204fff5`, socket id `SocketData +4`), the `OnFrameMove` tick (`@3207001b`, `+0x2b0` = `m_fElapsedTime`);
  - the NPC consumer: roll `< 0.03` (a **double** constant `3344f248`), cool-down `<= 0`, the five-argument-plus call `( ST_ACTIVE, -5.0, false, BCT_FIX_VALUE, 1.0, true, 0 )`, cool-down = 0.5.
- **JUNE_BUGS 136 / 137.**

Hygiene is one command per flag: `python tools/flagcheck.py <FLAG> --range <first C10e commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and deleted line (both flags edit the same
files, so each sees the other as OTHER).

Check the play-test log (`JUNE_FX socket-cooltime`): the flag line for the equipped accessory, at least one `proc=1` roll, and the
active skills' cool-down dropping by 5 s in game. If the play-test has not happened, say so; do not call the phase reviewed.

Spot-check 2-3 sites directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "C10e - Review" under C10 in the plan,
update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
