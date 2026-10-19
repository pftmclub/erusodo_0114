# June skill system core - Phase S7o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7o review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7o done (identification only: no code change, no flag) |
| Flag | none ( nothing ported ) |
| Log tag | none ( no play-test ) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Phase S7o - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7o`.

## Task

S7o stopped at identification: it found the `+0xd80` object unreachable offline. Re-check that verdict in Ghidra (read-only), from the raw disassembly:
- The class: RTTI of vtable `3347ae7c` = `CX2BossRaidLongTimeEvent`; CX2Main ctor @31e46a4c builds it at `+0xd80`; ctor `32163f30` zeroes `+0x124` / `+0x125` / `+0x11c` / `+0x120`.
- **The reach claim**: no store to those four offsets outside the ctor and `32163bb0` case 0x526. Scan every function that calls getter `31d70d70` (xrefs) and the
  class's own methods. Watch for `lea reg,[reg+0x11c]` and indirect stores as well as direct `mov`. One more writer that runs offline overturns the verdict and makes S7o a port.
- That nothing in the emulator ( `X2Lib/Offline/` ) answers or sends a NOT that maps to June's 0x526.
- The four callers' guards ( `31f276b0` @31f27c17, `31f08290` @31f08513 (inverted `jbe`), `324bbe50` @324bc218, `32509d30` @3250a236 ) and their owners
  (CX2StateBattleField / CX2StateField ctors by RTTI).
- `DLG_BossRaid_Enter.lua` is referenced only from `32161070` (xrefs to `3347ab14`).

No hygiene run (no code). If the verdict holds: write "Phase S7o -- Review" into the plan doc, update the S7o STATUS row and commit. If it does not: say so, and
re-scope S7o as a port (kickoff + STATUS row), with the design questions for the user.
